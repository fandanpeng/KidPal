"""Frozen acceptance tests — TaskPackage 261021-p3-record-cancel-lifecycle（server 面）。

承载 story = S-F2.2-AC4（BOOT/「讲错啦」取消丢弃不设恢复——显式丢弃通知 + 断连取消
兜底 + zombie 隔离的实现语义落地）+ S-F1.4-AC6（settle 链回归面归固件齿 RCL-F1）。
上游冻结 = delivery/review/261021-arch-review-p3-record-cancel-lifecycle/PROPOSAL.md
（FROZEN v1.1 + R2 GREEN）+ reviewed plan
delivery/taskpackages/261021-p3-record-cancel-lifecycle/plan.md（根因四条 file:line
实证 + F1-F4 修复件修位表 + 验收种子 1-4）+ 契约 §四 kidpal_story_discard
（2026-10-21 LANDED——只消费冻结契约措辞，本包零契约改动）。

Scope of record（taskpackage acceptance RCL-A1..A4）:
  RCL-A1 kidpal_story_discard 消费分支：累积态会话收到 discard → acc.cancel 落
         （reason="discard" 记审计）、无归档、story_list total 不变不重推、会话 pop、
         sweep 不再命中；幂等 no-op（重复 discard / 非累积态 discard）；连接权威——
         载荷 session_id 不被信任，front 按本连接 session_id 取消。
  RCL-A2 在途引导回合双点存续守卫（mock LLM 延迟注入）：discard/断连中途到达 →
         入口守卫（build_organizing_input None 即返不发 LLM）+ LLM 返回后播报前复查
         routing(sid)=="accumulate" 否则丢弃产出不播报不计轮；mark_guide_round 不
         复活会话壳（先判 accumulate 再 mark）。
  RCL-A3 teardown 收尾：设备腿断连 + 引擎腿静默 → 收尾限时内完成（gather
         FIRST_COMPLETED 不悬挂回归齿）、cancel(reason="ws_disconnect") 落、
         _alert_legs 与会话→连接腿映射摘除。
  RCL-A4 跨连接隔离（zombie 回归齿）：旧连接 zombie 会话 + 新连接在线 → sweep 引导
         注入与自动收尾帧只打旧会话归属连接的腿，新连接腿零帧；提醒/课表两消费点
         全局腿语义不回归。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — front_app.py 面离线装载
（fastapi 缺席本地 stub，261019 先例）；story.py 既有零改（预判零改文件）。

  A) server/gateway/front/front_app.py — [SC-A] discard 消费分支
     （_consume_kidpal_control 内 mtype == "kidpal_story_discard"）:

      * 整条消费并拦截（返回 True）、不下透引擎（契约 §四「网关消费并拦截、
        不下透引擎」）。
      * **连接权威**：按调用入参 session_id（= pump/ws_gateway 的本连接会话键）
        取消——acc.cancel(session_id, reason="discard")；载荷 obj["session_id"]
        不作会话键（仅审计参考，契约 §四「不信载荷中的 session_id 值」）。
      * 累积缓冲销毁不归档：不触发 archive_story、不触发 story_list 重推
        （契约 §四「不触发归档、不触发 story_list 重推」）。
      * 幂等 no-op：会话已释放（含重复 discard、非累积态 discard、未知会话）→
        行为等价 no-op（零帧、零归档、零会话壳新建、零异常）。
      * 审计日志一行：logger "kidpal-front"（info 级），消息含冻结 token
        "story_discard" + 本连接 session_id 值；cancel reason 冻结值 =
        "discard"。

  B) front_app.py — [SC-B] 在途引导回合双点存续守卫（_story_guide_round）:

      * 入口守卫：acc.build_organizing_input(sid) is None（会话已释放/空累积）
        → 直接返回——零 LLM 调用、零播报、零计轮、零会话壳。
      * 播报前复查：LLM 返回后、注入播报与计轮**之前**复查
        acc.routing(sid) == "accumulate"；非累积（discard/断连收尾/收尾词路径已
        cancel）→ 丢弃产出：不播报、不计轮、不留会话壳。
      * mark_guide_round 仅在累积态调用（先判 accumulate 再 mark——mark 内部
        _ensure 会重建会话壳，无守卫的 mark = 复活已取消会话）。
      * 不误杀正常收尾：finish 路径先 cancel(reason="finish") 后演出——在途引导
        与收尾竞态时引导产出被丢弃是正确行为（plan F3④）。

  C) front_app.py ws_gateway — [SC-C] teardown 收尾不悬挂:

      * 双泵等待语义 = 任一泵先结束即收尾（FIRST_COMPLETED 等价）：另一泵限时
        取消，finally 链（_unregister_alert_legs + acc.cancel(session_id,
        reason="ws_disconnect") + [SC-D] 会话腿映射摘除）在设备腿断连后**限时
        内**完成——pump_up 阻塞在引擎腿读帧（记录语境被抑制、无流量）不得拖死
        收尾（根因① front_app.py:1496 await asyncio.gather 悬挂语义退役）。

  D) front_app.py — [SC-D] 腿集会话归口映射缝（名字冻结，builder 落地）:

      def _register_session_legs(session_id, device_ws, engine_ws) -> None
          -- ws_gateway 双腿在拍登记（与 _register_alert_legs 同位）。
      def _unregister_session_legs(device_ws) -> None
          -- finally 摘除：按 device_ws 对象身份摘除该连接的全部登记
             （_unregister_alert_legs 同款身份判据）。
      def _session_legs(session_id) -> list
          -- 查询该会话归属连接的腿 [(device_ws, engine_ws)]；未知/已摘 -> []。
             查询不改状态。

      sweep 引导注入（_story_guide_round 播报）与自动收尾（_story_auto_finish →
      _story_finish_session sweep 路径）只打 _session_legs(sid)——**不**打全局
      _alert_legs；提醒（_deliver_alert_now）与课表（_push_schedule_refresh_to_
      legs）两消费点维持全局 _alert_legs 不动（F3⑤）；_register_alert_legs /
      _unregister_alert_legs 既有语义零偏移。

Shape authority = docs/contracts/interface-contract.md §四 kidpal_story_discard
（2026-10-21 LANDED）+ docs/architecture.md §5.2 场景④（取消链显式化）+
docs/requirements.md v1.66 S-F2.2-AC4 / S-F1.4-AC6。

跨端 wire 形状锚（对照对端实际接受什么）:
  firmware/main/protocols/protocol.cc SendKidpalStoryDiscard —— 发送帧逐字形态
  {"session_id":"...","type":"kidpal_story_discard"}（无第三字段；与
  SendKidpalStoryDelete 先例同构拼串经 SendText 下发）；发送时机 = CloseAudioChannel
  之前同连接先行（同一 TCP 有序，front 泵先消费 discard 再处理断连）。固件半钉在
  firmware/main/test/record_session_live.acceptance.c RSL-A5/RSL-A8（RCL-F1）。

契约条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 验收断言」节）:

| 契约 / 决策条目 | 承载断言 | 性质档 |
|---|---|---|
| 契约 §四 kidpal_story_discard（消费拦截/不归档/不重推/幂等/审计） | test_rcl_a1_* 三件（真泵真跑） | 行为级 |
| 契约 §四「按本连接 session_id 权威取消、不信载荷值」 | test_rcl_a1_discard_connection_authority | 行为级 |
| OQ2 = cancel-on-disconnect 维持 | test_rcl_a3（teardown cancel 落） | 行为级 |
| F2 连接权威裁定 | 同 test_rcl_a1_discard_connection_authority | 行为级 |
| F3 双点存续守卫 + 不引入会话代次 | test_rcl_a2_* 三件（守卫真跑 + 零壳复活） | 行为级 |
| F1 teardown 悬挂修复 | test_rcl_a3（限时收尾真跑） | 行为级 |
| F3⑤ 腿集归口 + 提醒/课表全局腿不动 | test_rcl_a4_* 两件（sweep 定向真跑 + 全局投递真跑） | 行为级 |
| OQ3 复验③重定义 (a)（在途/在播残余立即断） | test_rcl_a2 中途到达两件 | 行为级（时序半；「播报刚停」真机半归真机轮） |

弱于契约处（显式缺口，不得当已兑现）:
  GAP-1 discard 端到端 wire（设备真发帧、TCP 先 discard 后 disconnect 的有序性、
        真机 BOOT 手势链）= live-manual 真机轮（exitCriteria 已申报 awaiting-
        human:real-device）；offline 用假 ws 驱动真实 pump 消费路径承载（EQ-1）。
  GAP-2 teardown 的真实 TLS/websockets 客户端与真引擎腿行为 = live；offline 以
        fake websockets 连接 + 永静默引擎腿承载（EQ-5）。
  GAP-3 审计的落盘面（ECS 日志保留/kplog）不钉——只钉进程 logger "kidpal-front"
        一行的冻结 token。
  GAP-4 start_organizing settle 后设备侧 Idle 清位链（ctx_set(0) / 守卫放行 /
        RaiseReminderQueue）= 设备编译单元 host 不可机判 → 固件齿
        record_dialogue_guide.acceptance.c（RCL-F1 settle 结构锚）+ 真机轮。
  GAP-5 「引导播报刚停、子态回聆听瞬间丢弃」的真机时序（OQ3 (a) 的设备半）=
        真机轮；本文件以「LLM 在途 + 中途 discard/断连」承载服务端时序半。

行为等价假设（显式；不写即默认不成立）:
  EQ-1 「pump_client_to_upstream 真路径 = 现场帧消费路径」（261019 EQ-1 同款，
        已复证）——discard 消费断言经该泵真实驱动可观察。
  EQ-2 「_push_display_payload / _alert_legs 推送 = 设备实际收帧形状」（261019
        EQ-2 同一重建器口径）。
  EQ-4 「LLM 不可达时快失败」：引导/整理回合 provider 调用离线快失败（EQ-4 既有
        环境假设；A1 的 sweep 零命中断言依赖——离线环境 loopback 18789 不在位）。
  EQ-5 「fake websockets 连接 ≈ 真连接上下文」：ws_gateway 内
        from websockets.asyncio.client import connect 的 import 形态在世为前提；
        若 front 改用其它 ws 客户端库或改 import 形态 = 锚漂移，本齿 infra 红
        报出交 acceptance-author 重钉，不由 builder/fixer 改齿。
  EQ-6 「cancel 的 reason 观察 = spy 包装」：对真实 StoryAccumulator 实例的
        cancel 方法做记录-委派包装（真实取消语义真跑，非 mock acc）——reason
        值的断言不替代行为断言（会话 pop / 无归档另行独立断言）。
  EQ-7 「uuid 注入 = 会话键生成」：A3 以 front.uuid 模块属性注入固定 hex 承载
        可预测会话键——uuid4 每连接一次的既有形态在世为前提（漂移 = infra 红
        报出重钉）。

fixture 自含性: 时间真值 = 注入 datetime（零墙钟依赖）；stories/schedule/alerts
  路径 = tmpdir / 环境隔离；fake ws 双桩自含（261019 同款形态）；唯一环境前提 =
  EQ-4 离线快失败。行为 seam 驱动方/消费方闭环：discard（设备泵 → 消费分支）、
  守卫（sweep → 引导回合）、teardown（假设备断连 → ws_gateway finally）、归口
  （映射登记 → sweep 注入）——无缺驱动方盲区；设备端 discard 发送半由 RSL-A8
  结构齿承载驱动方（真机半 GAP-1）。

Coverage map (acceptanceRef -> test):
  RCL-A1 -> test_rcl_a1_discard_cancels_without_archive_and_sweep_stops
         -> test_rcl_a1_discard_idempotent_and_non_accumulating_noop
         -> test_rcl_a1_discard_connection_authority_ignores_payload_session_id
  RCL-A2 -> test_rcl_a2_entry_guard_no_llm_after_session_released
         -> test_rcl_a2_post_llm_recheck_drops_output_on_disconnect_midflight
         -> test_rcl_a2_discard_midflight_via_real_pump
  RCL-A3 -> test_rcl_a3_teardown_completes_within_budget_on_device_disconnect
  RCL-A4 -> test_rcl_a4_session_legs_mapping_seam_register_query_unregister
         -> test_rcl_a4_sweep_frames_scoped_to_zombie_session_legs

Run protocol（offline；-m "not live" 可跑；文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" \\
      server/acceptance/test_record_cancel_lifecycle.acceptance.py

REVISION LOG
  2026-10-21 初版冻结（acceptance-author；RCL-A1..A4；F1-F4 修复件修位表 +
             验收种子 1-4 消费；[SC-A]-[SC-D] seam 契约 = plan F2/F3 修位 +
             契约 §四冻结措辞的机器可判形态；live/真机面如实申报不造 offline
             假齿——261019 同款口径）。
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import json
import sys
import tempfile
import threading
import types
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"

# 冻结协调字面（front 单源；形状不入契约、入本冻结契约）。
DISCARD_TYPE = "kidpal_story_discard"
RECORD_CTX_TYPE = "kidpal_record_context"
START_ORGANIZING_ACTION = "start_organizing"
SPEAK_TYPE = "kidpal_speak"
# [SC-A] 冻结 token：审计日志消息含 "story_discard"（一行留痕的机判锚）。
AUDIT_TOKEN = "story_discard"
# [SC-A] 冻结 reason 值：discard 消费触发的 cancel reason = "discard"。
DISCARD_REASON = "discard"
# [SC-C] 冻结 reason 值：断连兜底 cancel reason（既有 ws_gateway finally 值）。
WS_DISCONNECT_REASON = "ws_disconnect"

_CTX = json.dumps({"type": "kidpal_session_ctx", "source_page": "story"},
                  ensure_ascii=False)


def _discard_frame(session_id="sess-forged-cross"):
    """契约 §四 wire 形态：{"session_id":"...","type":"kidpal_story_discard"}。"""
    return json.dumps({"session_id": session_id, "type": DISCARD_TYPE},
                      ensure_ascii=False)


# --------------------------------------------------------------------------- #
# Module loading (missing/unimportable = RED for missing behaviour)           #
# --------------------------------------------------------------------------- #

_front_module_cache = None


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（M4K1 / 261019 冻结件同款先例）。"""
    try:
        import fastapi  # noqa: F401

        if isinstance(getattr(fastapi, "FastAPI", None), type):
            return
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    class _StubFastAPI:
        def __init__(self, *args, **kwargs):  # noqa: D107
            pass

        def _identity(self, *args, **kwargs):
            def deco(fn):
                return fn

            return deco

        get = post = websocket = _identity

    stub = _PermissiveModule("fastapi")
    stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
    stub.HTTPException = type("HTTPException", (Exception,), {})
    stub.FastAPI = _StubFastAPI
    sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _PermissiveModule("fastapi.responses")


def _load_front_app():
    """Import server/gateway/front/front_app.py by path, offline. (mod|None, err)."""
    global _front_module_cache
    if _front_module_cache is not None:
        return _front_module_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_rcl", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须离线"
                      "可装载（import 期零网络 I/O；fastapi 缺席走本地 stub）。")


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("RCL RED: " + err)
    return mod


@contextlib.contextmanager
def _front_with_stories(stories_dir):
    """把 front 的 STORIES_DIR 指向 tempdir（隔离真实 /opt/kidpal/stories 路径）。"""
    front = _front()
    old_dir, old_acc = front.STORIES_DIR, front._story_acc
    front.STORIES_DIR = str(stories_dir)
    front._story_acc = None
    try:
        yield front
    finally:
        front.STORIES_DIR = old_dir
        front._story_acc = old_acc


def _t0():
    return datetime(2026, 10, 21, 12, 0, 0, tzinfo=dt_timezone.utc)


# --------------------------------------------------------------------------- #
# I/O doubles（261019 冻结件同款形态：只替代 WebSocket 两端）                    #
# --------------------------------------------------------------------------- #


class _FakeUpstream:
    """Engine ws leg double: async-iterable of preloaded frames, records .send()."""

    def __init__(self, frames=()):
        self._frames = list(frames)
        self._pos = 0
        self.sent = []

    def __aiter__(self):
        return self

    async def __anext__(self):
        if self._pos >= len(self._frames):
            raise StopAsyncIteration
        frame = self._frames[self._pos]
        self._pos += 1
        return frame

    async def send(self, message):
        self.sent.append(message)


class _SilentUpstream:
    """永静默引擎腿（A3：记录语境下引擎被抑制、无流量——pump_up 阻塞在读帧上，
    永不返回。根因①悬挂的引擎侧形态）。"""

    def __init__(self):
        self.sent = []

    def __aiter__(self):
        return self

    async def __anext__(self):
        await asyncio.Event().wait()  # never set: engine leg silent forever

    async def send(self, message):
        self.sent.append(message)


class _FakeDeviceSocket:
    """Device ws leg double: starlette-style .receive() dicts + send_text/send_bytes."""

    def __init__(self, incoming=(), disconnect_exc=None):
        self._incoming = list(incoming)
        self._pos = 0
        self._disconnect_exc = disconnect_exc or RuntimeError
        self.sent_text = []
        self.sent_bytes = []

    async def receive(self):
        if self._pos >= len(self._incoming):
            raise self._disconnect_exc()
        msg = self._incoming[self._pos]
        self._pos += 1
        return msg

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(bytes(data))


class _GatewayDeviceSocket:
    """ws_gateway 面的 starlette WebSocket double（A3）：accept/close/headers +
    receive() 立即断连（设备腿断连形态）。"""

    def __init__(self, auth_header=""):
        self.headers = {"authorization": auth_header}
        self.sent_text = []
        self.sent_bytes = []
        self.accepted = False
        self.closed_with = None

    async def accept(self):
        self.accepted = True

    async def close(self, code=1000):
        self.closed_with = code

    async def receive(self):
        raise RuntimeError("device leg disconnected before any frame")

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(bytes(data))


def _device_to_engine(front, session_id, incoming_texts):
    """Run the device->engine uplink pump to completion; returns (upstream, device)."""
    dev = _FakeDeviceSocket(
        incoming=[{"type": "websocket.receive", "text": t} for t in incoming_texts])
    up = _FakeUpstream()
    asyncio.run(front.pump_client_to_upstream(dev, up, session_id))
    return up, dev


async def _device_to_engine_async(front, session_id, incoming_texts):
    """同 _device_to_engine 但在调用方事件循环内运行（A2 中途到达场景）。"""
    dev = _FakeDeviceSocket(
        incoming=[{"type": "websocket.receive", "text": t} for t in incoming_texts])
    up = _FakeUpstream()
    await front.pump_client_to_upstream(dev, up, session_id)
    return up, dev


def _engine_to_device(front, session_id, frames):
    """Run the engine->device downlink pump; returns (upstream, device)."""
    texts = [f if isinstance(f, str) else json.dumps(f, ensure_ascii=False) for f in frames]
    up = _FakeUpstream(frames=texts)
    dev = _FakeDeviceSocket()
    asyncio.run(front.pump_upstream_to_client(up, dev, session_id))
    return up, dev


def _forwarded_frame_types(sent):
    out = []
    for item in sent:
        if isinstance(item, (bytes, bytearray)):
            continue
        try:
            obj = json.loads(item)
        except (TypeError, ValueError):
            out.append("<non-json>")
            continue
        out.append(obj.get("type") if isinstance(obj, dict) else "<non-object>")
    return out


def _forwarded_objects(sent):
    out = []
    for item in sent:
        if isinstance(item, (bytes, bytearray)):
            continue
        try:
            obj = json.loads(item)
        except (TypeError, ValueError):
            continue
        if isinstance(obj, dict):
            out.append(obj)
    return out


def _mcp_envelopes(sent_text):
    out = []
    for text in sent_text:
        try:
            frame = json.loads(text)
        except (TypeError, ValueError):
            continue
        if (isinstance(frame, dict) and frame.get("type") == "mcp"
                and isinstance(frame.get("payload"), dict)):
            payload = frame["payload"]
            params = payload.get("params") or {}
            if payload.get("method") == "tools/call":
                out.append(payload)
    return out


def _display_payloads(sent_text):
    out = []
    for env in _mcp_envelopes(sent_text):
        args = (env.get("params") or {}).get("arguments") or {}
        if args.get("name") != "kidpal/display":
            continue
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _story_list_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_list"]


def _story_text_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_text"]


def _control_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "control"]


def _start_organizing_frames(sent_text):
    return [p for p in _control_frames(sent_text)
            if (p.get("data") or {}).get("action") == START_ORGANIZING_ACTION]


def _speak_frames(sent):
    return [o for o in _forwarded_objects(sent) if o.get("type") == SPEAK_TYPE]


def _record_ctx_frames(sent):
    return [o for o in _forwarded_objects(sent) if o.get("type") == RECORD_CTX_TYPE]


def _alert_envelopes(sent_text):
    return [env for env in _mcp_envelopes(sent_text)
            if (env.get("params") or {}).get("name") == "kidpal/alert"]


@contextlib.contextmanager
def _alert_legs(front, device_ws, engine_ws):
    """临时登记全局推送腿（_register_alert_legs 既有缝），退出时清理。"""
    old = list(getattr(front, "_alert_legs", []))
    front._register_alert_legs(device_ws, engine_ws)
    try:
        yield
    finally:
        front._alert_legs[:] = old


@contextlib.contextmanager
def _guide_legs(front, session_id, device_ws, engine_ws):
    """引导/收尾注入腿登记：[SC-D] 映射缝在场走会话归口（post-fix 形态）；缺席时
    退回全局 _alert_legs 登记（pre-fix 形态——保证守卫类断言的红可归因于守卫本体
    而非映射缝缺席，映射缝缺席由 RCL-A4 专齿承载）。"""
    register = getattr(front, "_register_session_legs", None)
    if callable(register):
        register(session_id, device_ws, engine_ws)
        try:
            yield
        finally:
            front._unregister_session_legs(device_ws)
    else:
        with _alert_legs(front, device_ws, engine_ws):
            yield


class _GatedLLMStub:
    """_openclaw_chat_once 替身：记录调用；gate=True 时阻塞至 release 置位
    （mock LLM 延迟注入——A2 在途回合形态）。EQ-4 离线快失败假设的显式替代。"""

    def __init__(self, reply, gate=False):
        self.calls = []
        self.entered = threading.Event()
        self.release = threading.Event()
        self.reply = reply
        self.gate = gate

    def __call__(self, system_text, user_text, timeout_s=10.0):  # noqa: ANN001, ANN202
        self.calls.append((system_text, user_text))
        self.entered.set()
        if self.gate:
            self.release.wait(timeout=10.0)
        return self.reply


@contextlib.contextmanager
def _llm_stub(front, stub):
    """monkeypatch front._openclaw_chat_once（冻结桩缝，_organize_and_archive 经
    模块全局查找调用——261019 _organize_stub 同款单源）。"""
    old = getattr(front, "_openclaw_chat_once", None)
    front._openclaw_chat_once = stub
    try:
        yield stub
    finally:
        if old is None:
            front.__dict__.pop("_openclaw_chat_once", None)
        else:
            front._openclaw_chat_once = old


@contextlib.contextmanager
def _organize_stub(front, reply):
    """organize 成功桩（261019 同款罐头忠实整理输出：标题行 + 正文行）。"""
    with _llm_stub(front, _GatedLLMStub(reply)) as stub:
        yield stub.calls


def _spy_cancel(acc):
    """对真实 StoryAccumulator 实例的 cancel 做记录-委派包装（EQ-6：真实取消
    语义真跑，reason 可观察）。返回记录列表 [(session_id, reason)]。"""
    calls = []
    orig = acc.cancel

    def _spy(session_id, now=None, *, reason=""):
        calls.append((session_id, reason))
        return orig(session_id, now=now, reason=reason)

    acc.cancel = _spy
    return calls


def _seed_accumulating_session(front, session_id, text):
    """开累积态会话并喂一句孩子的话（ctx 经真泵 + stt 经引擎腿真泵——与现场帧
    消费路径同构，EQ-1）。返回 ctx 泵的 upstream（引擎腿替身）。"""
    up, _dev = _device_to_engine(front, session_id, [_CTX])
    _engine_to_device(front, session_id, [{"type": "stt", "text": text}])
    return up


# =========================================================================== #
# RCL-A1 — kidpal_story_discard 消费（连接权威 / 幂等 / 无归档 / sweep 不再命中）  #
# =========================================================================== #


class RclDiscardAcceptance(unittest.TestCase):
    """RCL-A1: _consume_kidpal_control 新增 discard 分支（契约 §四消费语义）。"""

    maxDiff = None

    # coverage: RCL-A1 -- discard 到达 -> cancel(reason=discard) 落 + 会话 pop +
    #           拦截不下透 + 无归档零重推 + sweep 不再命中 + 审计一行
    def test_rcl_a1_discard_cancels_without_archive_and_sweep_stops(self):
        with tempfile.TemporaryDirectory(prefix="rcl_a1_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                sid = "sess-rcl-a1"
                t0 = _t0()
                _seed_accumulating_session(front, sid, "从前有一只小猫")
                self.assertEqual(acc.routing(sid), "accumulate",
                                 "RCL-A1 RED(前置): ctx(story) 后会话未进累积态 —— "
                                 "既有收敛入口回退，须先复核既有齿。")
                self.assertTrue(acc.build_organizing_input(sid),
                                "RCL-A1 RED(前置): 累积缓冲为空 —— stt 喂入链回退。")
                cancel_calls = _spy_cancel(acc)
                with _guide_legs(front, sid, _FakeDeviceSocket(), _FakeUpstream()):
                    with self.assertLogs("kidpal-front", level="INFO") as cm:
                        up, dev = _device_to_engine(front, sid, [_discard_frame()])

                # ① 拦截不下透（契约 §四「网关消费并拦截、不下透引擎」）。
                self.assertNotIn(DISCARD_TYPE, _forwarded_frame_types(up.sent),
                                 "RCL-A1 RED: kidpal_story_discard 被透传引擎 —— 消费"
                                 "拦截缺失（_consume_kidpal_control 无 discard 分支，"
                                 "设计态红; 契约 §四）。")
                # ② cancel 落（reason=discard 记账）且会话 pop（StoryAccumulator.cancel
                #    pop 会话态 -> routing 回 chat、缓冲释放）。
                self.assertIn((sid, DISCARD_REASON),
                              [(s, r) for s, r in cancel_calls],
                              f"RCL-A1 RED: acc.cancel 未落或 reason != {DISCARD_REASON!r}"
                              f"（实记 {cancel_calls}）—— discard 消费分支未按连接权威"
                              "取消（设计态红 = _consume_kidpal_control 无 discard 分支）。")
                self.assertNotEqual(acc.routing(sid), "accumulate",
                                    "RCL-A1 RED: discard 后会话仍在累积态 —— acc.cancel "
                                    "未落（丢弃 = 素材滞留累积态, 稍后被 sweep 自动归档 "
                                    "= 症状⑤复现）。")
                self.assertNotIn(sid, acc.active_sessions(),
                                 "RCL-A1 RED: 会话未被 pop —— 丢弃即丢弃语义被破"
                                 "（StoryAccumulator.cancel pop 纪律）。")
                self.assertIsNone(acc.build_organizing_input(sid),
                                  "RCL-A1 RED: 丢弃后累积缓冲仍可装配 —— 素材未销毁。")
                # ③ 无归档 + story_list total 不变 + 不重推（契约 §四）。
                self.assertEqual(len(list(Path(tmp).glob("*.md"))), 0,
                                 "RCL-A1 RED: discard 触发了归档 —— 「素材不入册」被破"
                                 "（丢弃反被落盘 = 语义反转, S-F2.2-AC4）。")
                self.assertEqual(_story_list_frames(dev.sent_text), [],
                                 "RCL-A1 RED: discard 触发 story_list 重推 —— 契约 §四"
                                 "「不触发 story_list 重推」被破（丢弃不入册, 无对账面）。")
                # ④ 审计日志一行（冻结 token + 本连接 session_id）。
                audit = [r.getMessage() for r in cm.records
                         if AUDIT_TOKEN in r.getMessage() and sid in r.getMessage()]
                self.assertTrue(audit,
                                f"RCL-A1 RED: 无审计日志行（logger kidpal-front 消息须含"
                                f"{AUDIT_TOKEN!r} + 本连接 session_id; 实收 "
                                f"{[r.getMessage() for r in cm.records]!r}）—— 契约 §四"
                                "「审计日志一行」缺失。")
                # ⑤ sweep 不再命中（纯决策零命中 + sweep 真跑零动作零归档; EQ-4 离线
                #    快失败环境下引导/收尾若误命中也不会因 LLM 挂起）。
                self.assertEqual(acc.stuck_due_sessions(t0 + timedelta(seconds=600),
                                                        timeout_s=1), [],
                                 "RCL-A1 RED: discard 后 stuck_due 仍命中该会话 —— "
                                 "sweep 继续驱动 zombie 会话（症状②复现）。")
                self.assertEqual(acc.auto_finish_due_sessions(t0 + timedelta(seconds=600),
                                                              timeout_s=1), [],
                                 "RCL-A1 RED: discard 后 auto_finish 仍命中该会话。")
                dev2, up2 = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev2, up2):
                    asyncio.run(front._story_timeout_sweep(
                        now=t0 + timedelta(seconds=600)))
                self.assertEqual(len(list(Path(tmp).glob("*.md"))), 0,
                                 "RCL-A1 RED: discard 后 sweep 完成了归档 —— 会话未被"
                                 "取消即收尾（zombie 自动归档 = 症状⑤）。")
                self.assertEqual(_start_organizing_frames(dev2.sent_text), [],
                                 "RCL-A1 RED: discard 后 sweep 下发 start_organizing —— "
                                 "已丢弃会话仍被收尾演出。")
                self.assertEqual(_speak_frames(up2.sent), [],
                                 "RCL-A1 RED: discard 后 sweep 注入引导播报 —— 会话未"
                                 "被取消（引导对着已丢弃的会话说话）。")

    # coverage: RCL-A1 -- 幂等 no-op：重复 discard / 非累积态 discard（契约 §四
    #           「会话已释放后重复/迟到的 discard = 幂等 no-op」）
    def test_rcl_a1_discard_idempotent_and_non_accumulating_noop(self):
        with tempfile.TemporaryDirectory(prefix="rcl_a1_idem_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                sid = "sess-rcl-a1-idem"
                t0 = _t0()
                _seed_accumulating_session(front, sid, "从前有一只小猫")
                # 第一次 discard：取消落（前置）。
                _device_to_engine(front, sid, [_discard_frame()])
                self.assertNotEqual(acc.routing(sid), "accumulate",
                                    "RCL-A1 RED(前置): 首 discard 未取消 —— 消费分支"
                                    "缺席（设计态红）。")
                # 重复 discard ×2：幂等 no-op —— 不崩、不透传、不新增会话壳、零归档、
                # 零 story_list 重推、零播报帧。
                for i in (2, 3):
                    up, dev = _device_to_engine(front, sid, [_discard_frame()])
                    self.assertNotIn(DISCARD_TYPE, _forwarded_frame_types(up.sent),
                                     f"RCL-A1 RED: 第 {i} 次重复 discard 被透传 —— 幂等"
                                     "拦截被破（迟到 discard 不得下透引擎）。")
                    self.assertEqual(_story_list_frames(dev.sent_text), [],
                                     f"RCL-A1 RED: 第 {i} 次重复 discard 重推了 "
                                     "story_list —— 幂等 no-op 被破。")
                    self.assertNotIn(sid, acc.active_sessions(),
                                     f"RCL-A1 RED: 第 {i} 次重复 discard 复活了会话壳"
                                     "（幂等 no-op 不得重建会话态）。")
                self.assertEqual(len(list(Path(tmp).glob("*.md"))), 0,
                                 "RCL-A1 RED: 重复 discard 产生归档 —— 幂等 no-op 被破。")
                # 非累积态 discard（chat 会话 / 未知会话）：消费拦截 + no-op。
                up2, dev2 = _device_to_engine(front, "sess-rcl-a1-chat", [_discard_frame()])
                self.assertNotIn(DISCARD_TYPE, _forwarded_frame_types(up2.sent),
                                 "RCL-A1 RED: 非累积态 discard 被透传 —— 消费拦截只认"
                                 "类型不认状态面被破（契约 §四: 整条消费并拦截）。")
                self.assertNotIn("sess-rcl-a1-chat", acc.active_sessions(),
                                 "RCL-A1 RED: 非累积态 discard 复活了会话壳 —— no-op "
                                 "被破（_ensure 型副作用）。")
                self.assertEqual(len(list(Path(tmp).glob("*.md"))), 0,
                                 "RCL-A1 RED: 非累积态 discard 产生归档。")

    # coverage: RCL-A1 + F2 连接权威 -- 载荷 session_id 不被信任：本连接 discard
    #           不取消其他连接的会话（伪造跨会话 session_id 防线）
    def test_rcl_a1_discard_connection_authority_ignores_payload_session_id(self):
        with tempfile.TemporaryDirectory(prefix="rcl_a1_auth_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                t0 = _t0()
                sid_a = "sess-rcl-a1-connA"
                sid_b = "sess-rcl-a1-connB"
                up_a = _seed_accumulating_session(front, sid_a, "小猫在山上")
                up_b = _seed_accumulating_session(front, sid_b, "小狗在河边")
                sent_a_before = len(up_a.sent)
                # 连接 B 上的 discard 携带伪造载荷 session_id = 连接 A 的会话键。
                up_b2, _dev_b = _device_to_engine(
                    front, sid_b, [_discard_frame(session_id=sid_a)])
                # 连接 A 的会话不受影响（连接权威：载荷值不被信任）。
                self.assertEqual(acc.routing(sid_a), "accumulate",
                                 "RCL-A1 RED: 连接 B 的 discard 取消了连接 A 的会话 —— "
                                 "载荷 session_id 被信任（连接权威被破; 契约 §四「不信"
                                 "载荷中的 session_id 值」; 伪造跨会话取消面）。")
                self.assertIn("小猫在山上", acc.build_organizing_input(sid_a) or "",
                              "RCL-A1 RED: 连接 A 的累积素材被清除 —— 跨连接伪造取消"
                              "波及素材面。")
                self.assertEqual(len(up_a.sent), sent_a_before,
                                 "RCL-A1 RED: 连接 A 的引擎腿收到新帧 —— 伪造 discard "
                                 "波及他人连接（出语境/播报误注入）。")
                # 本连接会话照常取消。
                self.assertNotEqual(acc.routing(sid_b), "accumulate",
                                    "RCL-A1 RED: 连接 B 的 discard 未取消本连接会话 —— "
                                    "连接权威口径下取消落点漂移。")
                self.assertNotIn(DISCARD_TYPE, _forwarded_frame_types(up_b2.sent),
                                 "RCL-A1 RED: 伪造载荷 discard 被透传 —— 拦截缺失。")


# =========================================================================== #
# RCL-A2 — 在途引导回合双点存续守卫（mock LLM 延迟注入）                          #
# =========================================================================== #


class RclInFlightGuardAcceptance(unittest.TestCase):
    """RCL-A2: 入口守卫 + 播报前复查 + mark 不复活会话壳（plan F3①②）。"""

    maxDiff = None

    # coverage: RCL-A2 -- 入口守卫：会话已释放（build_organizing_input None）-> 直接
    #           返回不发 LLM、零播报、零计轮、零会话壳
    def test_rcl_a2_entry_guard_no_llm_after_session_released(self):
        with tempfile.TemporaryDirectory(prefix="rcl_a2_entry_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                sid = "sess-rcl-a2-entry"
                t0 = _t0()
                acc.start(sid, now=t0)
                acc.feed(sid, "从前有一只小猫", now=t0)
                stub = _GatedLLMStub("那后来呢？")
                with _llm_stub(front, stub):
                    # 丢弃先于引导回合（OQ3 (a) 复验③形态：在途/刚停引导时丢弃）。
                    acc.cancel(sid, reason=DISCARD_REASON)
                    dev, up = _FakeDeviceSocket(), _FakeUpstream()
                    with _guide_legs(front, sid, dev, up):
                        asyncio.run(front._story_guide_round(
                            sid, t0 + timedelta(seconds=5)))
                self.assertEqual(stub.calls, [],
                                 "RCL-A2 RED: 引导回合入口守卫缺席 —— 会话已释放仍发 "
                                 "LLM（空素材照发 = 根因③前半, front_app.py:593-600 "
                                 "形态; 设计态红 = 双点守卫缺席）。")
                self.assertEqual(_speak_frames(up.sent), [],
                                 "RCL-A2 RED: 已取消会话的引导产出仍注入播报 —— 孩子按"
                                 "了丢弃后小伴还在说话（在途/在播引导残余未断）。")
                self.assertNotIn(sid, acc.active_sessions(),
                                 "RCL-A2 RED: 引导回合复活了会话壳 —— mark_guide_round "
                                 "经 _ensure 重建已取消会话（先判 accumulate 再 mark "
                                 "守卫缺席; 根因③尾）。")
                self.assertEqual(acc.guide_rounds(sid), 0,
                                 "RCL-A2 RED: 已取消会话仍被计引导轮 —— 不计轮语义被破。")

    # coverage: RCL-A2 -- 播报前复查：LLM 在途期间断连取消到达 -> 产出丢弃不播报
    #           不计轮（mock LLM 延迟注入; mark 不复活会话壳）
    def test_rcl_a2_post_llm_recheck_drops_output_on_disconnect_midflight(self):
        with tempfile.TemporaryDirectory(prefix="rcl_a2_mid_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                sid = "sess-rcl-a2-mid"
                t0 = _t0()
                acc.start(sid, now=t0)
                acc.feed(sid, "从前有一只小猫", now=t0)
                stub = _GatedLLMStub("那后来它去哪了？", gate=True)
                with _llm_stub(front, stub):
                    dev, up = _FakeDeviceSocket(), _FakeUpstream()
                    with _guide_legs(front, sid, dev, up):

                        async def _main():
                            task = asyncio.create_task(front._story_guide_round(
                                sid, t0 + timedelta(seconds=5)))
                            for _ in range(500):
                                if stub.entered.is_set() or task.done():
                                    break
                                await asyncio.sleep(0.01)
                            self.assertTrue(
                                stub.entered.is_set(),
                                "RCL-A2 RED(前置): LLM 回合未进入 —— 引导回合接线回退。")
                            # 断连中途到达：teardown 收尾 cancel（F1 修复后的时序等价
                            # 形态——LLM 在途 + 会话释放竞态）。
                            acc.cancel(sid, reason=WS_DISCONNECT_REASON)
                            stub.release.set()
                            await task

                        asyncio.run(_main())
                self.assertEqual(len(stub.calls), 1,
                                 "RCL-A2 RED(前置): LLM 调用次数 != 1 —— 引导回合形态"
                                 "回退（本 case 断言的是播报前复查, 入口守卫归上一 case）。")
                self.assertEqual(_speak_frames(up.sent), [],
                                 "RCL-A2 RED: LLM 返回后播报前复查缺席 —— 会话已取消仍"
                                 "注入引导播报（产出未丢弃、在播残余未断; 根因③后半 "
                                 "front_app.py:601-602 形态; 设计态红 = 双点守卫缺席）。")
                self.assertNotIn(sid, acc.active_sessions(),
                                 "RCL-A2 RED: 在途引导回合复活了会话壳 —— mark_guide_"
                                 "round 先于/无 accumulate 判定（根因③尾 story.py:695 "
                                 "_ensure 形态的 front 侧守卫缺席）。")
                self.assertEqual(acc.guide_rounds(sid), 0,
                                 "RCL-A2 RED: 已取消会话仍被计轮（复查缺席 = 播报 + 计轮"
                                 "双副作用）。")

    # coverage: RCL-A2 + RCL-A1 -- discard 经真泵中途到达：消费 -> 取消 -> 在途产出
    #           丢弃（端到端时序半; 若消费分支缺席本 case 红因同时覆盖两处缺失）
    def test_rcl_a2_discard_midflight_via_real_pump(self):
        with tempfile.TemporaryDirectory(prefix="rcl_a2_pump_") as tmp:
            with _front_with_stories(tmp) as front:
                acc = front._get_story_acc()
                sid = "sess-rcl-a2-pump"
                t0 = _t0()
                acc.start(sid, now=t0)
                acc.feed(sid, "从前有一只小猫", now=t0)
                stub = _GatedLLMStub("那后来呢？", gate=True)
                with _llm_stub(front, stub):
                    dev, up = _FakeDeviceSocket(), _FakeUpstream()
                    with _guide_legs(front, sid, dev, up):

                        async def _main():
                            task = asyncio.create_task(front._story_guide_round(
                                sid, t0 + timedelta(seconds=5)))
                            for _ in range(500):
                                if stub.entered.is_set() or task.done():
                                    break
                                await asyncio.sleep(0.01)
                            self.assertTrue(stub.entered.is_set(),
                                            "RCL-A2 RED(前置): LLM 回合未进入。")
                            # discard 经真实设备泵到达（本连接权威; 伪造载荷 session_id
                            # ——连接权威断言已由 RCL-A1 专齿承载, 此处只钉时序）。
                            up_d, _dev_d = await _device_to_engine_async(
                                front, sid, [_discard_frame()])
                            self.assertNotIn(
                                DISCARD_TYPE, _forwarded_frame_types(up_d.sent),
                                "RCL-A2 RED: 中途 discard 被透传 —— 消费分支缺席（与"
                                "双点守卫缺席同为设计态红, 归因见 RCL-A1 专齿）。")
                            stub.release.set()
                            await task

                        asyncio.run(_main())
                self.assertNotEqual(acc.routing(sid), "accumulate",
                                    "RCL-A2 RED(前置): discard 中途到达未取消会话 —— "
                                    "消费分支缺席（设计态红, 归因见 RCL-A1）。")
                self.assertEqual(_speak_frames(up.sent), [],
                                 "RCL-A2 RED: discard 中途到达后引导产出仍播报 —— 双点"
                                 "存续守卫缺席（discard 通道的在途残余未断）。")
                self.assertNotIn(sid, acc.active_sessions(),
                                 "RCL-A2 RED: discard 中途到达后引导回合复活会话壳。")
                self.assertEqual(acc.guide_rounds(sid), 0,
                                 "RCL-A2 RED: discard 中途到达后仍计轮。")


# =========================================================================== #
# RCL-A3 — teardown 收尾不悬挂（gather FIRST_COMPLETED 回归齿）                   #
# =========================================================================== #


@contextlib.contextmanager
def _fake_engine_connect(upstream):
    """fake websockets 客户端模块链（EQ-5）：ws_gateway 内
    from websockets.asyncio.client import connect 的 import 形态在世为前提；
    connect(...) 返回让入 upstream 的 async 上下文。"""
    names = ("websockets", "websockets.asyncio", "websockets.asyncio.client")
    saved = {k: sys.modules.get(k) for k in names}
    mod_ws = types.ModuleType("websockets")
    mod_asyncio = types.ModuleType("websockets.asyncio")
    mod_client = types.ModuleType("websockets.asyncio.client")

    class _ConnectCM:
        def __init__(self, *_a, **_k):
            pass

        async def __aenter__(self):
            return upstream

        async def __aexit__(self, *_exc):
            return False

    mod_client.connect = lambda *_a, **_k: _ConnectCM()
    mod_asyncio.client = mod_client
    mod_ws.asyncio = mod_asyncio
    sys.modules["websockets"] = mod_ws
    sys.modules["websockets.asyncio"] = mod_asyncio
    sys.modules["websockets.asyncio.client"] = mod_client
    try:
        yield
    finally:
        for k, v in saved.items():
            if v is None:
                sys.modules.pop(k, None)
            else:
                sys.modules[k] = v


class RclTeardownAcceptance(unittest.TestCase):
    """RCL-A3: 设备腿断连 + 引擎腿静默 -> 收尾限时内完成 + cancel 落 + 腿摘除。"""

    maxDiff = None

    # coverage: RCL-A3 + OQ2 -- gather 不悬挂回归齿（假 ws 关闭触发）+
    #           cancel(reason=ws_disconnect) 落 + _alert_legs 摘除
    def test_rcl_a3_teardown_completes_within_budget_on_device_disconnect(self):
        with tempfile.TemporaryDirectory(prefix="rcl_a3_") as tmp:
            with _front_with_stories(tmp) as front:
                token = "tok-rcl-acceptance-0123456789ab"
                old_token, old_loop = front.DEVICE_TOKEN, front._ensure_alert_loop
                old_schedule = front.SCHEDULE_MD
                front.DEVICE_TOKEN = token
                front._ensure_alert_loop = lambda: None  # 离线 hermetic：不起轮询任务
                front.SCHEDULE_MD = str(Path(tmp) / "no-schedule.md")
                hexval = "a3a3a3a3a3a3a3a3a3a3a3a3a3a3a3a3"
                sid = "sess-" + hexval
                old_uuid = front.uuid
                fake_uuid = types.SimpleNamespace()
                fake_uuid.uuid4 = lambda: types.SimpleNamespace(hex=hexval)
                front.uuid = fake_uuid  # EQ-7：会话键可预测（uuid4 每连接一次形态在世）
                acc = front._get_story_acc()
                acc.start(sid, now=_t0())
                acc.feed(sid, "从前有一只小猫", now=_t0())
                cancel_calls = _spy_cancel(acc)
                dev = _GatewayDeviceSocket(auth_header="Bearer " + token)
                up = _SilentUpstream()
                try:
                    with _fake_engine_connect(up):
                        try:
                            asyncio.run(asyncio.wait_for(
                                front.ws_gateway(dev), timeout=6.0))
                        except (asyncio.TimeoutError, TimeoutError):
                            self.fail(
                                "RCL-A3 RED: 设备腿已断连且引擎腿静默, ws_gateway 收尾未"
                                "在限时内完成 —— gather 双泵悬挂语义在世（front_app.py:1496"
                                " await asyncio.gather(pump_up, pump_dn): pump_dn 已结束"
                                "而 pump_up 阻塞在引擎腿读帧上永不返回, finally 的 cancel/"
                                "腿摘除永不执行 = 根因①; 设计态红 = gather 双泵悬挂语义在世）。")
                finally:
                    front.DEVICE_TOKEN = old_token
                    front._ensure_alert_loop = old_loop
                    front.SCHEDULE_MD = old_schedule
                    front.uuid = old_uuid
                # cancel(reason=ws_disconnect) 落（OQ2: cancel-on-disconnect 维持）。
                self.assertIn((sid, WS_DISCONNECT_REASON),
                              [(s, r) for s, r in cancel_calls],
                              f"RCL-A1/A3 RED: 断连收尾 acc.cancel 未落或 reason != "
                              f"{WS_DISCONNECT_REASON!r}（实记 {cancel_calls}）—— 收尾"
                              "释放链未随收尾完成而执行（OQ2: cancel-on-disconnect 维持）。")
                # 会话 pop + 无归档（未停录断开的累积缓冲不泄漏, HWV-A11 意图）。
                self.assertNotIn(sid, acc.active_sessions(),
                                 "RCL-A3 RED: 断连收尾后会话未 pop —— 未停录断开累积"
                                 "缓冲泄漏（HWV-A11 原意图回退）。")
                self.assertEqual(len(list(Path(tmp).glob("*.md"))), 0,
                                 "RCL-A3 RED: 断连收尾产生了归档 —— 断连 = cancel 丢弃"
                                 "不落盘（cancel-on-disconnect 语义反转）。")
                # _alert_legs 摘除（zombie 腿不再留驻全局投递面）。
                self.assertFalse(
                    any(pair[0] is dev for pair in list(front._alert_legs)),
                    "RCL-A3 RED: 断连收尾后 _alert_legs 仍含本连接腿 —— zombie 腿跨"
                    "重连泄漏（提醒/引导注入会打进已死连接）。")
                # 会话腿映射摘除（[SC-D]；映射缝缺席由 RCL-A4 专齿红, 此处条件断言）。
                query = getattr(front, "_session_legs", None)
                if callable(query):
                    self.assertEqual(query(sid), [],
                                     "RCL-A3 RED: 断连收尾后会话腿映射未摘除 —— zombie "
                                     "归口残留在册（sweep 仍会向死连接定向注帧）。")
                # 建连面真实性（前置性质收尾：设备腿真的被 accept 过——排除假绿路径）。
                self.assertTrue(dev.accepted,
                                "RCL-A3 RED(infra): ws_gateway 未 accept 设备腿 —— "
                                "鉴权门把连接拒之门外, 本 case 驱动路径失真。")


# =========================================================================== #
# RCL-A4 — 跨连接隔离（zombie 回归齿）+ 腿集会话归口映射缝                          #
# =========================================================================== #


class RclSessionLegScopeAcceptance(unittest.TestCase):
    """RCL-A4: sweep 引导注入与自动收尾帧只打旧会话归属连接的腿 + 全局腿不回归。"""

    maxDiff = None

    # coverage: RCL-A4 -- [SC-D] 映射缝四件套（登记/查询/按设备摘除/未知空表）
    def test_rcl_a4_session_legs_mapping_seam_register_query_unregister(self):
        front = _front()
        register = getattr(front, "_register_session_legs", None)
        self.assertTrue(
            callable(register),
            "RCL-A4 RED: front_app.py 无 _register_session_legs —— 腿集会话归口映射缝"
            "未落地（plan F3⑤; 设计态红 = 腿集会话归口映射缺席）。")
        unregister = getattr(front, "_unregister_session_legs", None)
        query = getattr(front, "_session_legs", None)
        self.assertTrue(callable(unregister) and callable(query),
                        "RCL-A4 RED: _unregister_session_legs / _session_legs 缺一 —— "
                        "映射缝不完整（摘除或查询面缺失, [SC-D] 名字冻结）。")
        dev1, up1 = _FakeDeviceSocket(), _FakeUpstream()
        dev2, up2 = _FakeDeviceSocket(), _FakeUpstream()
        old = list(front._alert_legs)
        try:
            front._register_session_legs("sess-map-a", dev1, up1)
            front._register_session_legs("sess-map-b", dev2, up2)
            self.assertEqual([(d, e) for d, e in query("sess-map-a")], [(dev1, up1)],
                             "RCL-A4 RED: 映射查询与登记不一致（会话 -> 腿对失真）。")
            self.assertEqual([(d, e) for d, e in query("sess-map-b")], [(dev2, up2)],
                             "RCL-A4 RED: 映射查询与登记不一致。")
            self.assertEqual(query("sess-unknown"), [],
                             "RCL-A4 RED: 未知会话查询未返回空表（[SC-D] 缺省语义）。")
            # 按 device_ws 身份摘除：该连接全部会话登记移除、他连接不受影响。
            front._unregister_session_legs(dev1)
            self.assertEqual(query("sess-map-a"), [],
                             "RCL-A4 RED: 摘除后旧连接映射仍命中 —— zombie 归口残留。")
            self.assertEqual([(d, e) for d, e in query("sess-map-b")], [(dev2, up2)],
                             "RCL-A4 RED: 摘除连接 A 误伤连接 B 的映射。")
            # 查询不改状态（幂等重查一致）。
            self.assertEqual([(d, e) for d, e in query("sess-map-b")], [(dev2, up2)],
                             "RCL-A4 RED: 查询有副作用（改了映射）。")
        finally:
            front._alert_legs[:] = old
            if callable(unregister):
                unregister(dev1)
                unregister(dev2)

    # coverage: RCL-A4 -- zombie 回归齿：旧连接 zombie 会话（stuck-due 引导 +
    #           auto-finish 收尾）sweep 帧只打旧会话归属腿，新连接腿零帧；
    #           提醒投递全局腿语义不回归（must-stay-green 行为锚）
    def test_rcl_a4_sweep_frames_scoped_to_zombie_session_legs(self):
        with tempfile.TemporaryDirectory(prefix="rcl_a4_") as tmp:
            with _front_with_stories(tmp) as front:
                import gateway.reminders as gw_reminders  # noqa: PLC0415 — path ready
                acc = front._get_story_acc()
                register = getattr(front, "_register_session_legs", None)
                self.assertTrue(
                    callable(register),
                    "RCL-A4 RED: front_app.py 无 _register_session_legs —— 腿集会话"
                    "归口映射缝未落地（设计态红; 本 case 全部定向断言的前提缝）。")
                t0 = _t0()
                old1, old2 = "sess-rcl-a4-old1", "sess-rcl-a4-old2"
                dev_old1, up_old1 = _FakeDeviceSocket(), _FakeUpstream()
                dev_old2, up_old2 = _FakeDeviceSocket(), _FakeUpstream()
                dev_new, up_new = _FakeDeviceSocket(), _FakeUpstream()
                # 旧连接 zombie 腿仍在全局面（根因①悬挂形态：teardown 未跑完 ->
                # unregister 未执行）；新连接在线同登记全局面（真实 ws_gateway 形态）。
                front._register_alert_legs(dev_old1, up_old1)
                front._register_alert_legs(dev_old2, up_old2)
                front._register_alert_legs(dev_new, up_new)
                try:
                    front._register_session_legs(old1, dev_old1, up_old1)
                    front._register_session_legs(old2, dev_old2, up_old2)
                    front._register_session_legs("sess-rcl-a4-new", dev_new, up_new)
                    # zombie 会话①：stuck-due（1 轮引导后无回应）。
                    acc.start(old1, now=t0)
                    acc.feed(old1, "从前有一只小猫", now=t0)
                    acc.mark_guide_round(old1, now=t0 + timedelta(seconds=1))
                    # zombie 会话②：auto-finish due（2 轮无回应 + 计时到）。
                    acc.start(old2, now=t0)
                    acc.feed(old2, "小狗在河边", now=t0)
                    acc.mark_guide_round(old2, now=t0 + timedelta(seconds=1))
                    acc.mark_guide_round(old2, now=t0 + timedelta(seconds=2))
                    with _llm_stub(front, _GatedLLMStub("那后来呢？")):
                        with _organize_stub(front, "小狗回家\n小狗玩完水就回家了。"):
                            asyncio.run(front._story_timeout_sweep(
                                now=t0 + timedelta(seconds=600)))
                finally:
                    front._unregister_alert_legs(dev_old1)
                    front._unregister_alert_legs(dev_old2)
                    front._unregister_alert_legs(dev_new)
                # zombie 引导注入只打旧会话①归属腿。
                self.assertTrue(_speak_frames(up_old1.sent),
                                "RCL-A4 RED: zombie 会话的引导注入未达其归属连接腿 —— "
                                "定向归口把应打的腿也丢了（引导永远到不了孩子耳朵）。")
                # zombie 自动收尾只打旧会话②归属腿（start_organizing + story_text +
                # story_list 恰一帧族）。
                self.assertEqual(len(_start_organizing_frames(dev_old2.sent_text)), 1,
                                 "RCL-A4 RED: zombie 会话自动收尾 start_organizing 未达"
                                 "其归属连接腿（或帧数漂移）—— 收尾演出半缺失。")
                self.assertTrue(_story_text_frames(dev_old2.sent_text),
                                "RCL-A4 RED: zombie 会话收尾 story_text 未达归属腿。")
                self.assertTrue(_story_list_frames(dev_old2.sent_text),
                                "RCL-A4 RED: zombie 会话收尾 story_list 未达归属腿。")
                # 新连接腿零帧（跨连接泄漏回归齿本尊）。
                self.assertEqual(_speak_frames(up_new.sent), [],
                                 "RCL-A4 RED: 旧会话的引导注入打进了新连接引擎腿 —— 全局"
                                 "腿引导注入（_alert_legs 快照形态）在世 = 症状②复现"
                                 "（三次实证延迟 23-55s 的引导注入错连）。")
                self.assertEqual(_start_organizing_frames(dev_new.sent_text), [],
                                 "RCL-A4 RED: 旧会话的自动收尾 start_organizing 打进了"
                                 "新连接设备腿 —— 全局腿收尾在世（整理演出错连 + zombie "
                                 "自动归档故事推给新连接 = 症状⑤复现）。")
                self.assertEqual(_story_text_frames(dev_new.sent_text), [],
                                 "RCL-A4 RED: 新连接收到他人会话的 story_text。")
                self.assertEqual(_story_list_frames(dev_new.sent_text), [],
                                 "RCL-A4 RED: 新连接收到他人会话的 story_list 重推 —— "
                                 "孩子新会话的星星计数被 zombie 归档污染。")
                # 旧连接①的腿也不得收到会话②的收尾帧、②不得收到①的引导（会话级定向,
                # 非仅连接级粗粒度）。
                self.assertEqual(_start_organizing_frames(dev_old1.sent_text), [],
                                 "RCL-A4 RED: 收尾帧溢出到同物理连接外的另一 zombie 腿 —— "
                                 "归口粒度不足（连接级泄漏换了形态）。")
                self.assertEqual(_speak_frames(up_old2.sent), [],
                                 "RCL-A4 RED: 引导注入溢出到另一 zombie 腿 —— 同上。")
                # must-stay-green：提醒投递全局腿语义不回归（_deliver_alert_now 走
                # _alert_legs 全局面——本 case 已显式登记新连接腿）。
                entry = {"id": "biz-rcl-a4", "fire_at": "2026-10-21T08:00:00+00:00",
                         "title": "到点提醒", "speech": "时间到了哦",
                         "view": {"view": "text", "data": {"text": "到点提醒"}}}
                envelope = gw_reminders.build_alert_envelope(entry)
                front._register_alert_legs(dev_new, up_new)
                try:

                    async def _deliver():
                        front._deliver_alert_now("biz-rcl-a4", envelope)
                        await asyncio.sleep(0.05)

                    asyncio.run(_deliver())
                finally:
                    front._unregister_alert_legs(dev_new)
                self.assertTrue(_alert_envelopes(dev_new.sent_text),
                                "RCL-A4 RED: 提醒投递未达新连接 —— 全局腿消费点（提醒）"
                                "被会话归口误伤（F3⑤「两消费点维持全局腿不动」被破）。")
                self.assertTrue(_speak_frames(up_new.sent),
                                "RCL-A4 RED: 提醒语音注入未达新连接引擎腿 —— 全局腿消费"
                                "点（提醒）被误伤。")
                # 全局面在 sweep 后未被 purge（提醒/课表消费面的登记生命周期不动）。
                front._register_alert_legs(dev_new, up_new)
                try:
                    self.assertTrue(
                        any(pair[0] is dev_new for pair in list(front._alert_legs)),
                        "RCL-A4 RED: sweep 后全局 _alert_legs 登记被清 —— 会话归口不得"
                        "摘除全局投递面登记（提醒/课表依赖）。")
                finally:
                    front._unregister_alert_legs(dev_new)


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
