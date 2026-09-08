"""KidPal 提醒投递面（TaskPackage 260829-schedule-reminder，服务端一半）。

语义权威 = 冻结验收 server/acceptance/test_schedule_reminder.acceptance.py 文件头
（FROZEN OFFLINE CONTRACT module 2），不得偏移。本模块是 gateway 的 **additive 扩展**
（RMD-E3）：消费包 6 的课表数据链路与 OpenClaw 侧写出的 alerts/pending.json（同机交接
文件），自身不解析课表、不合成语音、不自建 HTTP 端点（契约 §三 原轮询形态废止，提醒
一律走 mcp kidpal/alert 推送）。

送达语义（契约 §三 / 决策 23 / spec S-F1.4「重试到确认」）：每分钟轮询到点 -> 经
kidpal/alert 推送 -> 等设备渲染回执，60 秒超时未收回执重推、单档上限 3 次、不做重连
即重推、上限后过期静默作废不补发（儿童产品不追催）。回执 = 设备渲染完成才发（渲染即
确认、即出队），业务 id 与数字传输 id 都收（consume_receipts 就是把 g-relay-fix 留下的
「acked_ids 有人收集、无人消费」缺口接上出队方的那条线）。

有界性（ECS 实测 Mem available 941MB / Swap used 535MB，同机还有引擎与模型）：pending
按分钟读、跟踪表被 MAX_TRACKED 卡住、过期即弃，零大常驻缓存。

跨进程输入必须机械校验：pending.json 由另一进程（OpenClaw cron）写，投递前逐条过
validate_alert_entry（内嵌 view 过**真实** gateway.display_schema.validate_display），
脏条目拒绝并留 reject 审计事件，绝不盲转发给孩子（V27）。

裁定二（eventId 09359e74-005b-45bb-9535-69e19c14ee04）：提醒语音借引擎既有音频链 ——
本模块只构造 {"type":"kidpal_speak","text":<speech>} 内部指令注入**引擎腿**，由引擎
现成入口 speak_txt 走既有 edge-tts provider + 已 live 实证 opus 下行链出音；front 侧
不自建合成、不做 mp3→opus 编码、不注入二进制音频帧（设备腿 send_bytes 恒空 = 可机判
红线）。speech 缺失/为空 -> 无注入指令，弹窗+提示音仍必达（V20 降级不阻塞）。

纯 python，导入期零网络 I/O（display_schema 走懒装载，允许以文件路径被单独导入）。
"""

from __future__ import annotations

import hashlib
import json
import logging
import os
import re
from datetime import datetime, timezone

log = logging.getLogger("kidpal-gateway.reminders")

# --------------------------------------------------------------------------- #
# FROZEN OFFLINE CONTRACT — 常量（重试到确认 + 有界性）
# --------------------------------------------------------------------------- #

POLL_INTERVAL_SEC = 60        # 每分钟轮询 pending.json
RETRY_TIMEOUT_SEC = 60        # 60 秒未收回执即重推
MAX_ATTEMPTS = 3              # 单档重试上限（超限静默作废，不追催）
MAX_PENDING = 64              # 单次读取的条目上界（超出进 rejected）
MAX_TRACKED = 256             # 投递记账上界（先淘汰已 acked/void）
VOID_WINDOW_SEC = RETRY_TIMEOUT_SEC * MAX_ATTEMPTS   # = 180，重试预算耗尽即过期

PENDING_NAME = "pending.json"
TOOL_ALERT = "kidpal/alert"                   # 契约 §四 mcp 工具调用（决策 23）
SPEECH_INJECTION_TYPE = "kidpal_speak"        # 裁定二冻结的内部指令类型字面
JSONRPC = "2.0"
METHOD_TOOLS_CALL = "tools/call"

_ID_RE = re.compile(r"[0-9A-Za-z._-]{1,40}")   # 设备上界 40（与生产侧 reminder_id 同界）
_INT32_MAX = 2 ** 31 - 1                      # 设备侧 valueint 截断区（cJSON int）

_ALERTS_DIR_ENV = "KIDPAL_ALERTS_DIR"
_PENDING_ENV = "KIDPAL_PENDING_JSON"


def pending_path() -> str:
    """alerts/pending.json 交接路径（env 可配，禁止硬编 /root/.openclaw/**）。

    KIDPAL_PENDING_JSON 指定整文件路径；否则 KIDPAL_ALERTS_DIR/pending.json；两者都未
    设时回落到工作目录下的 alerts/pending.json（部署由 systemd Environment= 给绝对路径）。
    """
    explicit = os.environ.get(_PENDING_ENV, "").strip()
    if explicit:
        return explicit
    folder = os.environ.get(_ALERTS_DIR_ENV, "").strip() or os.path.join("alerts")
    return os.path.join(folder, PENDING_NAME)


# --------------------------------------------------------------------------- #
# ISO8601 与节拍
# --------------------------------------------------------------------------- #


def parse_iso(iso) -> datetime | None:
    """带偏移的 ISO8601 解析（接受 Z / ±HH:MM）。非法或**无偏移** -> None。

    无偏移一律算非法：投递侧要把它和带偏移的 now 比较，naive 比较会静默错档（决策 24
    过期即弃走绝对时刻，不按字面）。与设备侧 kidpal_fire_at_minutes 同一口径。"""
    if not isinstance(iso, str):
        return None
    text = iso.strip()
    if not text:
        return None
    if text.endswith("Z") or text.endswith("z"):
        text = text[:-1] + "+00:00"
    try:
        stamp = datetime.fromisoformat(text)
    except ValueError:
        return None
    if stamp.tzinfo is None:
        return None
    return stamp


def to_epoch_min(iso) -> int | None:
    """ISO8601 -> 自 epoch 起的整分钟数（向下取整）。非法 -> None（绝不当成 0）。"""
    stamp = parse_iso(iso)
    if stamp is None:
        return None
    return int(stamp.timestamp() // 60)


def to_epoch_sec(iso) -> float | None:
    stamp = parse_iso(iso)
    return None if stamp is None else stamp.timestamp()


def next_poll_delay(now_epoch) -> float:
    """每分钟轮询的等待秒数：对齐到下一个整分钟，0 < r <= POLL_INTERVAL_SEC。

    非法时钟（<=0 / 非数字）防御回默认节拍（不忙轮询、不倒退）。"""
    try:
        now = float(now_epoch)
    except (TypeError, ValueError):
        return float(POLL_INTERVAL_SEC)
    if now <= 0:
        return float(POLL_INTERVAL_SEC)
    remainder = now % POLL_INTERVAL_SEC
    if remainder == 0:
        return float(POLL_INTERVAL_SEC)   # 恰好在整分钟点 -> 等下一整分钟
    return float(POLL_INTERVAL_SEC) - remainder


# --------------------------------------------------------------------------- #
# 机械校验（V27 纪律落在跨进程输入上）
# --------------------------------------------------------------------------- #


_display_schema_cache = None


def _display_schema():
    """懒取**真实** gateway.display_schema（冻结件要求内嵌 view 过它的 validate_display）。

    先按包导入（生产实态：front 以 server/ 为 WORKING_DIRECTORY 跑 uvicorn），失败则按
    本文件同级路径加载 —— 两条路都是同一份模块，不分叉第二套校验。
    """
    global _display_schema_cache
    if _display_schema_cache is not None:
        return _display_schema_cache
    try:
        from gateway import display_schema as ds
        _display_schema_cache = ds
        return ds
    except ImportError:
        pass
    import importlib.util  # noqa: PLC0415

    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "display_schema.py")
    spec = importlib.util.spec_from_file_location("kidpal_gateway_display_schema", path)
    if spec is not None and spec.loader is not None:
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _display_schema_cache = mod
        return mod
    return None


def validate_alert_entry(entry) -> tuple[bool, str]:
    """§三 提醒条目机械 schema 校验 -> (ok, reason)。拒绝必给原因（审计面），绝不盲转发。"""
    if not isinstance(entry, dict):
        return (False, "alert entry must be a JSON object (契约 §三)")
    rid = entry.get("id")
    if not isinstance(rid, str) or not rid:
        return (False, "id must be a non-empty string (§三 业务幂等 id)")
    if len(rid) > 40:
        return (False, f"id length {len(rid)} exceeds device capacity 40")
    if not _ID_RE.fullmatch(rid):
        return (False, f"id {rid!r} carries characters outside [0-9A-Za-z._-]")
    if parse_iso(entry.get("fire_at")) is None:
        return (False, f"fire_at {entry.get('fire_at')!r} is not ISO8601 with a UTC offset")
    title = entry.get("title")
    if not isinstance(title, str) or not title.strip():
        return (False, "title must be a non-empty string (弹窗大字事项必带)")
    speech = entry.get("speech", "")
    if not isinstance(speech, str):
        return (False, "speech must be a string (缺省=降级仅弹窗+提示音，非串=脏数据)")
    view = entry.get("view")
    if not isinstance(view, dict):
        return (False, "view must be the embedded display object (契约 §三)")
    ds = _display_schema()
    if ds is None:
        return (False, "display_schema unavailable — refuse to forward unchecked")
    ok, reason = ds.validate_display(view)
    if not ok:
        return (False, f"embedded view rejected: {reason}")
    return (True, "ok")


# --------------------------------------------------------------------------- #
# 读侧（同机交接文件）
# --------------------------------------------------------------------------- #


def read_pending(path=None, now_iso=None) -> tuple[list, list]:
    """按分钟读 alerts/pending.json -> (合法条目, 拒绝原因列表)。

    缺文件 -> ([], [])（cron 首次写入前该文件确实不存在 —— planner 260908 ECS 实测，
    安静返回、不报错）；整档非 JSON / alerts 非数组 -> ([], [reason])（不抛异常）；逐条
    机械校验，脏条目进 rejected 带原因；超过 MAX_PENDING 的部分进 rejected（有界）；
    给了 now_iso 时顺手丢弃早已超出重试窗口的历史档（过期即弃，不留内存）。"""
    target = str(path) if path else pending_path()
    try:
        with open(target, "r", encoding="utf-8") as f:
            text = f.read()
    except OSError:
        return ([], [])           # 缺文件/不可读 = 安静空转（下一分钟再看）
    rejected: list = []
    try:
        raw = json.loads(text)
    except (ValueError, TypeError) as e:
        return ([], [f"pending.json is not valid JSON: {e.__class__.__name__}"])
    if not isinstance(raw, dict):
        return ([], ["pending.json root must be an object container {version,alerts}"])
    alerts = raw.get("alerts")
    if not isinstance(alerts, list):
        return ([], ["pending.json 'alerts' must be an array"])

    now_sec = to_epoch_sec(now_iso) if now_iso else None
    entries: list = []
    for item in alerts:
        if len(entries) >= MAX_PENDING:
            ident = item.get("id") if isinstance(item, dict) else None
            rejected.append(f"{ident or 'entry'} dropped: over MAX_PENDING {MAX_PENDING}")
            continue
        ok, reason = validate_alert_entry(item)
        if not ok:
            ident = item.get("id") if isinstance(item, dict) else None
            rejected.append(f"{ident or '<no id>'}: {reason}")
            continue
        if now_sec is not None:
            stamp = to_epoch_sec(item["fire_at"])
            if stamp is not None and now_sec - stamp > VOID_WINDOW_SEC:
                # 已远超重试窗口的历史档不再进内存（网关推送时点判定作废由 dispatcher 落账）
                continue
        entries.append(item)
    return (entries, rejected)


def due_entries(entries: list, now_iso: str) -> list:
    """到点判定：机械校验通过且 fire_at <= now 的条目（闭区间 = 恰好那一刻算到点）。

    非法载荷一律不算到点（校验前置于推送，不盲推）。now 不可解析 -> 空（不猜）。"""
    now_sec = to_epoch_sec(now_iso)
    if now_sec is None:
        return []
    due = []
    for entry in entries or []:
        ok, _reason = validate_alert_entry(entry)
        if not ok:
            continue
        fire = to_epoch_sec(entry.get("fire_at"))
        if fire is None or fire > now_sec:
            continue
        due.append(entry)
    return due


# --------------------------------------------------------------------------- #
# 传输层载荷（契约 §三 载荷 + §四 mcp tools/call）
# --------------------------------------------------------------------------- #


def alert_jsonrpc_id(business_id) -> int:
    """§三 业务字符串 id -> 设备可接受的 JSON-RPC **数字** id（确定性派生）。

    设备 mcp 层只认数字 id（mcp_server.cc ParseMessage 的 cJSON_IsNumber 判据；字符串
    id 被按 Invalid id 整条拒收），而业务 id 必须留在 arguments.id 供设备易失去重 —— 故
    两个 id 分处。派生只用业务 id 本身（不掺随机数/时间），跨重推与跨进程重启得同一
    数字；值域 1..2^31-1 适配设备 int 解析。这件事上游明文划归本包（mcp_relay.
    alert_tool_call 的注记），relay_ack / display_tool_call 冻结语义一律不动。"""
    text = business_id if isinstance(business_id, str) else str(business_id or "")
    digest = hashlib.sha256(f"kidpal-alert:{text}".encode("utf-8")).digest()
    return int.from_bytes(digest[:8], "big") % (_INT32_MAX - 1) + 1


def build_alert_envelope(entry: dict) -> dict:
    """一条 kidpal/alert 的 JSON-RPC tools/call 信封（设备 wire 形态）。

    arguments 恰为 §三 五键（零增减）；嵌套 view 在设备上以 **JSON 字符串** 承载 ——
    设备 mcp Property 只有 bool/int/string 三型，对象型会被拒（与 kidpal/display 的
    data 字符串化同纪律）；arguments.id 保留字符串业务 id（幂等去重与回执关联全靠它）。"""
    view = entry.get("view")
    return {
        "jsonrpc": JSONRPC,
        "id": alert_jsonrpc_id(entry.get("id")),
        "method": METHOD_TOOLS_CALL,
        "params": {
            "name": TOOL_ALERT,
            "arguments": {
                "id": entry.get("id"),
                "fire_at": entry.get("fire_at"),
                "title": entry.get("title"),
                "speech": entry.get("speech", "") or "",
                "view": json.dumps(view, ensure_ascii=False, sort_keys=True),
            },
        },
    }


def build_speech_injection(entry: dict):
    """构造引擎腿内部指令 {"type":"kidpal_speak","text":<speech 原文>}。

    speech 缺失/为空 -> None（降级 = 不注入；弹窗 + 提示音仍必达）。kidpal/alert 路径
    永不产生二进制帧（front 不合成、不编码 —— 裁定二红线）。"""
    speech = entry.get("speech")
    if not isinstance(speech, str) or not speech.strip():
        return None
    return {"type": SPEECH_INJECTION_TYPE, "text": speech}


def build_alert_frames(entry: dict) -> tuple[list, list]:
    """两腿帧集：device_frames = [{"type":"mcp","payload":<envelope>}]（设备只把 type=mcp
    帧送进 McpServer，application.cc:492；多帧不批量）；upstream_frames = 语音注入指令
    （走引擎腿）。语音缺省时 upstream_frames == []。"""
    device_frames = [{"type": "mcp", "payload": build_alert_envelope(entry)}]
    injection = build_speech_injection(entry)
    upstream_frames = [injection] if injection is not None else []
    return (device_frames, upstream_frames)


def _encode(frame) -> str:
    if isinstance(frame, (bytes, bytearray)):
        return bytes(frame).decode("utf-8", "replace")
    return json.dumps(frame, ensure_ascii=False, default=str)


async def deliver_alert(entry, device, upstream) -> dict:
    """真跑两腿投递：设备腿逐帧 send_text，引擎腿 send 注入指令。

    返回 {"delivered","device_frames","speech"} 供上层记账（重试逻辑全靠它）。
    device.send_bytes 永不调用（裁定二红线：出音走引擎既有下行链）；speech 缺失时仍
    delivered=True（弹窗必达，语音是可选增强）。"""
    device_frames, upstream_frames = build_alert_frames(entry)
    sent = 0
    failure = None
    for frame in device_frames:
        try:
            await device.send_text(_encode(frame))
            sent += 1
        except Exception as e:  # noqa: BLE001 — 腿断了要可归因，不能静默当送达
            failure = {
                "category": "delivery",
                "fingerprint": "reminders.device-leg-send-failed",
                "failingStep": "deliver_alert.device_leg",
                "message": f"{e.__class__.__name__}: {e}",
            }
            break
    speech_sent = False
    if sent and upstream is not None:
        for frame in upstream_frames:
            try:
                await upstream.send(_encode(frame))
                speech_sent = True
            except Exception as e:  # noqa: BLE001 — 语音失败不阻塞弹窗（V20 降级）
                log.warning("alert_speech_injection_failed corr_id=%s fingerprint=%s "
                            "failingStep=deliver_alert.engine_leg err=%s "
                            "(降级：仅弹窗+提示音继续)", entry.get("id"),
                            "reminders.engine-leg-injection-failed", e.__class__.__name__)
    result = {
        "delivered": bool(sent),
        "device_frames": sent,
        "speech": speech_sent,   # 真注入成功才算播报（注入失败已在上面按降级继续投递）
    }
    if failure is not None:
        result["error"] = failure          # ErrorEnvelope 形态（K6），供上层按 fingerprint 归因
    corr = entry.get("id") if isinstance(entry, dict) else None
    log.info("alert_deliver corr_id=%s delivered=%s device_frames=%d speech=%s",
             corr, result["delivered"], sent, result["speech"])
    return result


# --------------------------------------------------------------------------- #
# 投递状态机：重试到确认 + 过期静默作废 + 幂等 + 有界 + 审计轨迹
# --------------------------------------------------------------------------- #


class _Slot:
    __slots__ = ("bid", "numeric", "state", "attempts", "next_retry_at", "fire_at_sec")

    def __init__(self, bid, numeric, state, attempts, next_retry_at, fire_at_sec):
        self.bid = bid
        self.numeric = numeric
        self.state = state
        self.attempts = attempts
        self.next_retry_at = next_retry_at
        self.fire_at_sec = fire_at_sec


class AlertDispatcher:
    """重试到确认 + 过期静默作废 + 幂等 + 有界 + 审计轨迹（冻结语义见类内各法）。"""

    def __init__(self, sink, max_attempts: int = MAX_ATTEMPTS,
                 retry_timeout_sec: int = RETRY_TIMEOUT_SEC,
                 void_window_sec: int = VOID_WINDOW_SEC):
        self._sink = sink
        self._max_attempts = int(max_attempts)
        self._retry_timeout_sec = float(retry_timeout_sec)
        self._void_window_sec = float(void_window_sec)
        self._slots: dict = {}          # 业务 id -> _Slot（插入序 = 记账序）
        self._numeric_index: dict = {}  # 数字传输 id -> 业务 id
        self.events: list = []          # 审计轨迹 [{"id","action","attempt","now"}]

    # -- 内部 --------------------------------------------------------------
    def _track_count(self) -> int:
        return len(self._slots)

    def _bounded_remember(self, slot: _Slot, numeric: int) -> None:
        self._slots[slot.bid] = slot
        self._numeric_index[numeric] = slot.bid
        if self._track_count() > MAX_TRACKED:
            # 先淘汰已 acked/void（它们已终结）；仍超限则淘汰最旧（有界性优先于记账完整，
            # 被淘汰的档位仍在 pending.json 里，下一轮按 unseen 重新判定，不会漏推）。
            for bid in [b for b, s in self._slots.items() if s.state in ("acked", "void")]:
                self._drop(bid)
                if self._track_count() <= MAX_TRACKED:
                    return
            for bid in list(self._slots.keys()):
                self._drop(bid)
                if self._track_count() <= MAX_TRACKED:
                    break

    def _drop(self, bid: str) -> None:
        slot = self._slots.pop(bid, None)
        if slot is not None:
            self._numeric_index.pop(slot.numeric, None)

    def _emit(self, bid: str, action: str, attempt: int, now_sec: float) -> None:
        self.events.append({"id": bid, "action": action, "attempt": attempt,
                            "now": datetime.fromtimestamp(now_sec, timezone.utc).isoformat()})
        if len(self.events) > 4 * MAX_TRACKED:
            del self.events[: len(self.events) - 2 * MAX_TRACKED]  # 审计轨迹也有界

    # -- 公开面 ------------------------------------------------------------
    def tick(self, entries: list, now_iso: str) -> list:
        """本分钟推送的业务 id 列表。

        未到点不推；到点推第一次（attempts=1、next_retry_at = now + 60s）；同一 now 再
        tick 同一 id 不重复推（幂等，重算复用 id 不重开预算）；未收回执且 now >=
        next_retry_at 且 attempts < 上限 -> 重推（同一业务 id、同一数字 id）；上限耗尽 ->
        void 静默作废；首次见到就已过 fire_at + VOID_WINDOW 的档位 -> 直接 void 一次都不推
        （= 断网期间未送达档位在**推送时点**判定作废）；非法载荷 -> reject 事件且不推。"""
        now_sec = to_epoch_sec(now_iso)
        if now_sec is None:
            return []
        pushed: list = []
        for entry in sorted(entries or [], key=lambda e: str(e.get("fire_at") or ""))[:MAX_PENDING]:
            ok, reason = validate_alert_entry(entry)
            bid = entry.get("id") if isinstance(entry, dict) else None
            if not ok:
                if isinstance(bid, str) and bid:
                    if bid not in self._slots:   # 同一脏 id 不重复记 reject（有界）
                        self._emit(bid, "reject", 0, now_sec)
                    log.warning("alert_reject corr_id=%s reason=%s (不盲转发)", bid, reason)
                else:
                    log.warning("alert_reject corr_id=<no-id> reason=%s", reason)
                continue
            fire_sec = to_epoch_sec(entry["fire_at"])
            if fire_sec is None or fire_sec > now_sec:
                continue                          # 未到点
            slot = self._slots.get(bid)
            if slot is None:
                numeric = alert_jsonrpc_id(bid)
                if now_sec - fire_sec > self._void_window_sec:
                    # 离线期间未送达的档位：推送时点判定作废，不补发（儿童产品不追催）
                    self._bounded_remember(_Slot(bid, numeric, "void", 0,
                                                 now_sec + self._void_window_sec, fire_sec), numeric)
                    self._emit(bid, "void", 0, now_sec)
                    log.info("alert_void corr_id=%s reason=expired_before_first_push", bid)
                    continue
                slot = _Slot(bid, numeric, "pushing", 0, now_sec, fire_sec)
                self._bounded_remember(slot, numeric)
            if slot.state in ("acked", "void"):
                continue                          # 已出队/已作废：后续 tick 不再推
            if slot.attempts and now_sec < slot.next_retry_at:
                continue                          # 超时窗未到 / 同一 now 幂等
            if slot.attempts >= self._max_attempts:
                slot.state = "void"
                slot.next_retry_at = now_sec + self._void_window_sec
                self._emit(bid, "void", slot.attempts, now_sec)
                log.info("alert_void corr_id=%s attempts=%d reason=retry_budget_exhausted "
                         "(静默作废不补发)", bid, slot.attempts)
                continue
            envelope = build_alert_envelope(entry)
            slot.attempts += 1
            slot.state = "pushing"
            slot.next_retry_at = now_sec + self._retry_timeout_sec
            self._emit(bid, "push", slot.attempts, now_sec)
            pushed.append(bid)
            try:
                self._sink(bid, envelope)
            except Exception as e:  # noqa: BLE001 — 投递腿异常不炸轮询循环
                log.warning("alert_sink_failed corr_id=%s err=%s", bid, e.__class__.__name__)
        return pushed

    def on_receipt(self, rid):
        """设备渲染回执 -> 业务 id（数字传输 id 与业务 id 都收）；未知 id -> None。

        幂等：重复回执不产生第二次出队效果（不会把已 acked 的档位再改一遍状态）。"""
        if isinstance(rid, bool):
            return None
        bid = self._numeric_index.get(rid) if isinstance(rid, int) else rid
        if not isinstance(bid, str):
            return None
        slot = self._slots.get(bid)
        if slot is None:
            return None                 # 凭空回执不记账（未知 id 不得被认成 acked）
        if slot.state == "acked":
            return None                 # 重复回执幂等：不再产生第二次出队效果
        slot.state = "acked"
        self._emit(bid, "ack", slot.attempts, datetime.now(timezone.utc).timestamp())
        log.info("alert_ack corr_id=%s attempts=%d dequeued=true", bid, slot.attempts)
        return bid

    def on_ws_connected(self) -> list:
        """设备连接建立 -> 恒 []：spec 明写「不做重连即重推」（否则设备一醒就被历史提醒轰炸）。"""
        return []

    def state_of(self, bid) -> str:
        slot = self._slots.get(bid) if isinstance(bid, str) else None
        return slot.state if slot is not None else "unseen"

    def attempts(self, bid) -> int:
        slot = self._slots.get(bid) if isinstance(bid, str) else None
        return slot.attempts if slot is not None else 0

    def tracked_count(self) -> int:
        return self._track_count()


def consume_receipts(dispatcher, receipt_ids) -> list:
    """把 front 泵收集到的设备回执喂给 dispatcher（**这条就是 g-relay-fix 留下的「acked_ids
    目前无人消费、未接出队方」缺口的接法**），返回本次被确认出队的业务 id。

    重复回执与未知 id 都不进返回值（幂等）。数字传输 id 由 dispatcher 反查成业务 id。"""
    out: list = []
    for rid in receipt_ids or []:
        bid = dispatcher.on_receipt(rid)
        if bid is None or bid in out:
            continue
        out.append(bid)
    return out


# --------------------------------------------------------------------------- #
# front 泵侧的回执识别（只认领「渲染回执」，不把 accepted 受理帧当送达确认）
# --------------------------------------------------------------------------- #


def alert_receipt_id(message):
    """从设备上行帧里取渲染回执对应的 id（业务 id 优先，退回数字 id）；非渲染回执 -> None。

    设备侧 kidpal/alert 有两个回复：工具回调的同步 accepted（"ack": false，不代表送达）
    与渲染完成后独立发出的回执（"ack": true，带 reminder_id）。渲染即确认才出队（§三），
    所以只有后者能进 consume_receipts —— relay_ack 本体语义不动（它是 display/通用回执
    的冻结面），本函数是提醒路径专用的收窄判定。"""
    payload = message
    if isinstance(payload, str):
        try:
            payload = json.loads(payload)
        except (TypeError, ValueError):
            return None
    if not isinstance(payload, dict):
        return None
    if payload.get("type") == "mcp" and isinstance(payload.get("payload"), dict):
        payload = payload["payload"]
    if not isinstance(payload, dict):
        return None
    result = payload.get("result")
    if not isinstance(result, dict) or result.get("ack") is not True:
        return None
    bid = result.get("reminder_id")
    if isinstance(bid, str) and bid:
        return bid
    rid = payload.get("id")
    return rid if isinstance(rid, int) and not isinstance(rid, bool) else None
