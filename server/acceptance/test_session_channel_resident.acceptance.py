"""Frozen acceptance tests — TaskPackage 261116-channel-resident-core（server 面 / front）。

承载 story = S-F1.4（提醒必达：常驻通道下会话间隙仍能出音）+ requirements §2.1（结束
三通道）+ S-F2.2-AC4（取消与三式收尾回归）。语义权威 = docs/architecture.md §5.1.2 /
§6.4 + ADR-019 + 契约 §四 listen / kidpal_keepalive / kidpal_session_ctx。技术方案 =
delivery/taskpackages/261116-channel-resident-core/plan.md「技术方案」T5–T9 与「验收方向
修订建议」。

Scope of record（SCRF-B1..B6）:
  SCRF-B1 心跳 producer（front）：env 默认开 / =0 关（回滚开关，极性反于
          KIDPAL_STORY_RESTORE_PUSH）；一个周期设备腿收恰一帧 {"type":"kidpal_keepalive"}
          且 sleep-first（建连当刻零帧）；发送抛异常 → task 静默退出、双泵 finally 三件
          仍执行。
  SCRF-B2 listen stop 分层收尾：① chat（问答）不动作、标记假零注入；② accumulate +
          accum 非空 → 已被 _story_finish_session 消费、收尾三件不得再动（恰一篇归档、
          恰一帧 start_organizing）；③ accumulate + accum 空 → pending_stop 禁止 cancel /
          禁止释放（protected state）；④ pending_stop 重复 stop → mark_pending 幂等、
          不动作；⑤ accumulate 未被归档路径消费（结构上不可达，防御性）→ 不 cancel。
  SCRF-B2b pending 保护 + sweep 释放：+9s 零释放、+10s 由 sweep 释放且 active=false 恰
          一帧入归属腿；先投 stt 走恢复归档则 sweep 不重复释放。
  SCRF-B3 discard→stop 有序不归档（承重齿）：同会话先 discard 后 listen stop → routing
          回 chat、_story_finish_session 未被调、STORIES_DIR 文件数不变。
  SCRF-B4 ctx 边沿兜底分态：accumulate → cancel(reason="session_end_stale") + 释放；
          pending_stop → 不动作；chat 但标记真 → 只释放；重复 ctx(非 story) → 零新增注入。
  SCRF-B4b pending_stop 分支不动作（F-01 ctx 边沿复发防线）：ctx(story)→stop(空)→
          ctx(home) 零 cancel / 零释放 / routing 仍 pending_stop；随后 stt 仍恰一篇归档。
  SCRF-B5 连接级 teardown 保绿（must-stay-green）：断开 → 腿双摘除 +
          acc.cancel(reason="ws_disconnect")。
  SCRF-B6 既有冻结 oracle 保绿回归网（presence 守卫 + 全量 suite 绿留痕）。

===========================================================================
FROZEN OFFLINE CONTRACT（builder implements；fastapi 缺席本地 stub，261019/RCL 先例；
story.py 三缝 release_pending / cancel / PENDING_STOP_SECONDS 零改动）：

  T5（front_app.py ws_gateway）：每连接心跳 task，落 await _push_schedule_connect 之后、
  asyncio.wait({pump_up, pump_dn}, FIRST_COMPLETED) 之前创建；发送形态 = 裸
  websocket.send_text(json.dumps({"type":"kidpal_keepalive"}))（非 display 帧）；循环体
  sleep-first 并整体 try/except Exception: return；finally 内首行 hb.cancel() +
  await asyncio.gather(hb, return_exceptions=True)；绝不进 FIRST_COMPLETED 集合。开关
  KIDPAL_CHANNEL_HEARTBEAT（代码默认开；0/false/off/no 关 = 回滚开关）与
  KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S（默认 30）；模块级常量与 channel_heartbeat_enabled()
  helper 定义在 ws_gateway 之前。

  T6（pump_client_to_upstream，_story_stop_on_listen_stop 之后）：listen/stop 门控后按
  acc.routing 分层——chat + 标记假 → 零动作；accumulate + accum 非空 → 不动作（已被归档
  路径消费）；accumulate + accum 空（pending_stop）与 pending_stop 重复 → 禁止 cancel /
  禁止释放（protected）；未消费兜底分支不 cancel。每连接「记录语境曾置位」标记
  _record_ctx_seen（WeakKeyDictionary 形），置位点 = ctx(story) ctx_push_edge +
  _inject_record_context_enter 调用点；清位单点 = _release_record_context 体内按 legs
  的 device_ws 逐个清。审计一行含冻结 token session_end_boundary。

  T7（_story_timeout_sweep_body）：release_pending 返 True 的分支追加
  await _release_record_context(sid, _session_legs(sid) or list(_alert_legs))。

  T8（_consume_kidpal_control ctx 分支）：收到非 story 合法 ctx 且标记真时——pending_stop
  → 不动作；accumulate → acc.cancel(sid, reason="session_end_stale") + 释放；chat +
  标记真 → 只释放。重复 ctx(非 story) → 零新增注入（幂等）。未知 source_page 仍拒绝 +
  warning 且不释放（既有 elif 行为逐字保留）。

  T9（ws_gateway finally）：腿双摘除 + acc.cancel(reason="ws_disconnect") 逐字保留。

强度申报（契约条目 -> 承载断言 -> 性质档）:
  契约 §四 kidpal_keepalive / 部署极性 -> B1 -> 行为级（真跑 ws_gateway + 真设备腿收帧）。
  F-01 承重单点（记录语境释放） -> B2/B2b/B4/B4b -> 行为级（真泵 + 真 StoryAccumulator +
  真时钟注入；释放经 spy 与真引擎腿帧双承载）。
  S-F2.2-AC4 discard→stop 不归档 -> B3 -> 行为级（真泵 + 真归档落盘计数）。
  OQ2 cancel-on-disconnect -> B5 -> 行为级（真跑 ws_gateway）。
  弱于契约处（显式缺口）:
  GAP-1 心跳「假时钟」以注入短 interval（env KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S）近似
        ——心跳循环用 asyncio.sleep，无注入 now 缝；等价断言 = sleep-first 零前置帧 +
        一个周期恰一帧 + 帧形状。真 30s 节奏归真机轮 A3。
  GAP-2 心跳三方并发写同一 device websocket 的竞态（无发送锁 residual）= 真机轮观测。
  GAP-3 设备 → front → 引擎三源对齐 = 真机轮 E5。
行为等价假设（显式；不写即默认不成立）:
  EQ-1 「pump_client_to_upstream 真路径 = 现场帧消费路径」（RCL EQ-1 同款，已复证）。
  EQ-2 「fake websockets 连接 ≈ 真连接上下文」（RCL EQ-5 同款）。
  EQ-3 「同 device websocket 对象 ≙ 同连接」（_record_ctx_seen 按 device ws 建键）——
        离线真泵跨多次调用复用同一 _FakeDeviceSocket 承载。
===========================================================================
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import json
import os
import sys
import tempfile
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

KEEPALIVE_TYPE = "kidpal_keepalive"
RECORD_CTX_TYPE = "kidpal_record_context"
START_ORGANIZING_ACTION = "start_organizing"
CTX_TYPE = "kidpal_session_ctx"
HEARTBEAT_ENV = "KIDPAL_CHANNEL_HEARTBEAT"
HEARTBEAT_INTERVAL_ENV = "KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S"
# 注入短周期（GAP-1）：sleep-first；一个周期恰一帧的离线近似。
_TEST_INTERVAL_S = "0.2"

# 冻结 reason 值（T6/T8）。
END_STALE_REASON = "session_end_stale"
WS_DISCONNECT_REASON = "ws_disconnect"

# 契约 §四 wire 形态（对端实际接受形状）。
def _ctx_msg(source_page: str) -> str:
    return json.dumps({"type": CTX_TYPE, "source_page": source_page}, ensure_ascii=False)


def _listen_stop_msg() -> str:
    return json.dumps({"type": "listen", "state": "stop"}, ensure_ascii=False)


def _discard_msg() -> str:
    return json.dumps({"type": "kidpal_story_discard"}, ensure_ascii=False)


def _stt_frame(text: str) -> dict:
    return {"type": "stt", "text": text}


# --------------------------------------------------------------------------- #
# Module loading（missing/unimportable = RED for missing behaviour）           #
# --------------------------------------------------------------------------- #

_front_module_cache = None


def _ensure_fastapi_stub() -> None:
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
    global _front_module_cache
    if _front_module_cache is not None:
        return _front_module_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    # 注入短心跳周期（GAP-1）：env 在 import 前落位，模块级常量读到此值。
    os.environ[HEARTBEAT_INTERVAL_ENV] = _TEST_INTERVAL_S
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_scr", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须离线"
                      "可装载（import 期零网络 I/O；fastapi 缺席走本地 stub）。")


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("SCR RED: " + err)
    return mod


@contextlib.contextmanager
def _front_with_stories(stories_dir):
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
    return datetime(2026, 11, 16, 12, 0, 0, tzinfo=dt_timezone.utc)


# --------------------------------------------------------------------------- #
# I/O doubles                                                                  #
# --------------------------------------------------------------------------- #


class _FakeUpstream:
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
    def __init__(self):
        self.sent = []

    def __aiter__(self):
        return self

    async def __anext__(self):
        await asyncio.Event().wait()

    async def send(self, message):
        self.sent.append(message)


class _FakeDeviceSocket:
    """starlette-style receive() dicts + send_text/send_bytes；可跨多次 pump 复用
    （_incoming 顺序消费，_pos 持续）——同对象身份 ≙ 同连接（EQ-3）。"""

    def __init__(self, incoming=(), disconnect_exc=None):
        self._incoming = list(incoming)
        self._pos = 0
        self._disconnect_exc = disconnect_exc or RuntimeError
        self.sent_text = []
        self.sent_bytes = []

    def append_text(self, text):
        self._incoming.append({"type": "websocket.receive", "text": text})

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


class _BlockingDeviceSocket:
    """ws_gateway 面设备腿（B1 心跳）：receive() 永不返回（连接常驻），send_text 记录；
    可配 keepalive 帧发送抛异常（发送失败分支）。"""

    def __init__(self, auth_header="", raise_on_keepalive=False):
        self.headers = {"authorization": auth_header}
        self.sent_text = []
        self.sent_bytes = []
        self.accepted = False
        self._raise_on_keepalive = raise_on_keepalive

    async def accept(self):
        self.accepted = True

    async def close(self, code=1000):
        self.closed_with = code

    async def receive(self):
        await asyncio.Event().wait()

    async def send_text(self, text):
        if self._raise_on_keepalive and KEEPALIVE_TYPE in text:
            raise RuntimeError("injected keepalive send failure")
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(bytes(data))


class _GatewayDeviceSocket:
    """ws_gateway 面设备腿（B5 teardown）：receive() 立即断连。"""

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


def _device_to_engine(front, sid, texts):
    """跑 device→engine 上泵（一次性注入 frames）; 返回 (upstream, device)。"""
    dev = _FakeDeviceSocket(
        incoming=[{"type": "websocket.receive", "text": t} for t in texts])
    up = _FakeUpstream()
    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
    return up, dev


def _engine_to_device(front, sid, frames):
    texts = [f if isinstance(f, str) else json.dumps(f, ensure_ascii=False) for f in frames]
    up = _FakeUpstream(frames=texts)
    dev = _FakeDeviceSocket()
    asyncio.run(front.pump_upstream_to_client(up, dev, sid))
    return up, dev


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


def _record_ctx_frames(sent):
    return [o for o in _forwarded_objects(sent) if o.get("type") == RECORD_CTX_TYPE]


def _release_frames(sent):
    return [o for o in _record_ctx_frames(sent) if o.get("active") is False]


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
            if payload.get("method") == "tools/call":
                out.append(payload)
    return out


def _display_payloads(sent_text):
    out = []
    for env in _mcp_envelopes(sent_text):
        params = env.get("params") or {}
        if params.get("name") != "kidpal/display":
            continue
        args = params.get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _story_list_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_list"]


def _start_organizing_frames(sent_text):
    out = []
    for p in _display_payloads(sent_text):
        if p.get("view") == "control" and (p.get("data") or {}).get("action") == START_ORGANIZING_ACTION:
            out.append(p)
    return out


def _keepalive_frames(sent_text):
    out = []
    for text in sent_text:
        try:
            obj = json.loads(text)
        except (TypeError, ValueError):
            continue
        if isinstance(obj, dict) and obj.get("type") == KEEPALIVE_TYPE:
            out.append(obj)
    return out


def _story_md_files(stories_dir):
    return list(Path(stories_dir).glob("*.md"))


# --------------------------------------------------------------------------- #
# Spies / stubs                                                                #
# --------------------------------------------------------------------------- #


def _spy_cancel(acc):
    calls = []
    orig = acc.cancel

    def _spy(session_id, now=None, *, reason=""):
        calls.append((session_id, reason))
        return orig(session_id, now=now, reason=reason)

    acc.cancel = _spy
    return calls


def _spy_release(front):
    """包装 front._release_record_context 记录 (session_id)（T7/T8 释放承载）。"""
    calls = []
    orig = getattr(front, "_release_record_context", None)
    if not callable(orig):
        return None, calls

    async def _spy(session_id, legs):
        calls.append(session_id)
        return await orig(session_id, legs)

    front._release_record_context = _spy
    return orig, calls


@contextlib.contextmanager
def _release_spy(front):
    calls = []
    orig = getattr(front, "_release_record_context", None)
    if not callable(orig):
        yield calls
        return

    async def _spy(session_id, legs):
        calls.append(session_id)
        return await orig(session_id, legs)

    front._release_record_context = _spy
    try:
        yield calls
    finally:
        front._release_record_context = orig


@contextlib.contextmanager
def _finish_spy(front):
    calls = []
    orig = getattr(front, "_story_finish_session", None)
    if not callable(orig):
        yield calls
        return

    async def _spy(session_id, acc, legs):
        calls.append(session_id)
        return await orig(session_id, acc, legs)

    front._story_finish_session = _spy
    try:
        yield calls
    finally:
        front._story_finish_session = orig


class _LLMStub:
    def __init__(self, reply):
        self.calls = []
        self.reply = reply

    def __call__(self, system_text, user_text, timeout_s=10.0, **kwargs):  # noqa: ANN001
        self.calls.append((system_text, user_text))
        return self.reply


@contextlib.contextmanager
def _llm_stub(front, stub):
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
def _guide_legs(front, session_id, device_ws, engine_ws):
    register = getattr(front, "_register_session_legs", None)
    if callable(register):
        register(session_id, device_ws, engine_ws)
        try:
            yield
        finally:
            front._unregister_session_legs(device_ws)
    else:  # mapping seam absent -> fall back to global alert legs
        old = list(getattr(front, "_alert_legs", []))
        front._register_alert_legs(device_ws, engine_ws)
        try:
            yield
        finally:
            front._alert_legs[:] = old


@contextlib.contextmanager
def _env_var(name, value):
    saved = os.environ.get(name)
    if value is None:
        os.environ.pop(name, None)
    else:
        os.environ[name] = value
    try:
        yield
    finally:
        if saved is None:
            os.environ.pop(name, None)
        else:
            os.environ[name] = saved


@contextlib.contextmanager
def _fake_engine_connect(upstream):
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


@contextlib.contextmanager
def _gateway_env(front, token, tmp, *, fake_uuid_hex=None, quiet_loop=True):
    """ws_gateway 离线跑的前置收窄：DEVICE_TOKEN / SCHEDULE_MD / _ensure_alert_loop /
    uuid（B1/B5 共用）。"""
    old_token = front.DEVICE_TOKEN
    old_schedule = getattr(front, "SCHEDULE_MD", None)
    old_loop = getattr(front, "_ensure_alert_loop", None)
    old_uuid = getattr(front, "uuid", None)
    front.DEVICE_TOKEN = token
    if old_schedule is not None:
        front.SCHEDULE_MD = str(Path(tmp) / "no-schedule.md")
    if quiet_loop and old_loop is not None:
        front._ensure_alert_loop = lambda: None
    if fake_uuid_hex is not None:
        fake_uuid = types.SimpleNamespace()
        fake_uuid.uuid4 = lambda: types.SimpleNamespace(hex=fake_uuid_hex)
        front.uuid = fake_uuid
    try:
        yield
    finally:
        front.DEVICE_TOKEN = old_token
        if old_schedule is not None:
            front.SCHEDULE_MD = old_schedule
        if old_loop is not None:
            front._ensure_alert_loop = old_loop
        if fake_uuid_hex is not None:
            front.uuid = old_uuid


def _token():
    return "tok-scr-acceptance-0123456789ab"


async def _drive_gateway(front, dev, up, *, max_seconds, stop_when):
    """以 fake 引擎腿跑 ws_gateway 至 stop_when(dev) 或 max_seconds，然后 cancel 收尾。"""
    task = asyncio.create_task(front.ws_gateway(dev))
    waited = 0.0
    try:
        while waited < max_seconds:
            await asyncio.sleep(0.02)
            waited += 0.02
            if stop_when(dev):
                break
    finally:
        task.cancel()
        with contextlib.suppress(asyncio.CancelledError, Exception):
            await task


# =========================================================================== #
# SCRF-B1 — 心跳 producer（极性 / 一个周期恰一帧 / sleep-first / 发送失败静默）      #
# =========================================================================== #


class ScrfHeartbeatAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B1 -- 心跳开关极性：channel_heartbeat_enabled() 代码默认开；0/false/
    #            off/no 关（回滚开关，反于 KIDPAL_STORY_RESTORE_PUSH）
    def test_scr_b1_heartbeat_gate_default_on_and_rollback_off(self):
        front = _front()
        helper = getattr(front, "channel_heartbeat_enabled", None)
        self.assertTrue(callable(helper),
                        "SCRF-B1 RED: front_app 无 channel_heartbeat_enabled() helper —— "
                        "心跳部署极性决策面未落（plan T5 冻结名）。")
        # 显式 raw（纯函数，不落墙钟/环境）。
        self.assertTrue(helper(None) is True or helper("") is True,
                        "SCRF-B1 RED: channel_heartbeat_enabled 默认非开 —— 代码默认开"
                        "极性破（会退化成每 ~120s 判死 + 立即重连 churn）。")
        for off in ("0", "false", "off", "no", " FALSE ", "Off"):
            self.assertFalse(helper(off),
                             f"SCRF-B1 RED: channel_heartbeat_enabled({off!r}) 未关 —— "
                             "回滚开关识别集不全（{0,false,off,no}，strip().lower()）。")
        self.assertTrue(helper("1"),
                        "SCRF-B1 RED: channel_heartbeat_enabled('1') 非开 —— 非识别集值"
                        "应一律开（不误关）。")
        # env 缺省（None = 读 os.environ）→ 未设 → 开。
        with _env_var(HEARTBEAT_ENV, None):
            self.assertTrue(helper(),
                            "SCRF-B1 RED: env 未设时 channel_heartbeat_enabled() 非开 —— "
                            "代码默认开偏置破。")

    # coverage: SCRF-B1 -- 一个周期设备腿恰收一帧 {"type":"kidpal_keepalive"}；sleep-first
    #            （建连当刻零帧）
    def test_scr_b1_heartbeat_emits_one_frame_after_interval(self):
        with tempfile.TemporaryDirectory(prefix="scr_b1_emit_") as tmp:
            with _front_with_stories(tmp) as front:
                token = _token()
                with _gateway_env(front, token, tmp):
                    dev = _BlockingDeviceSocket(auth_header="Bearer " + token)
                    up = _SilentUpstream()
                    with _fake_engine_connect(up):
                        # sleep-first：不足一个 interval 的短窗 → 零 keepalive。
                        asyncio.run(_drive_gateway(
                            front, dev, up, max_seconds=0.12,
                            stop_when=lambda d: False))
                        self.assertTrue(dev.accepted,
                                        "SCRF-B1 RED(infra): ws_gateway 未 accept 设备腿"
                                        " —— 鉴权门把连接拒之门外，本 case 驱动路径失真。")
                        self.assertEqual(
                            len(_keepalive_frames(dev.sent_text)), 0,
                            "SCRF-B1 RED: 建连当刻即发心跳（间隔窗内零帧被破）—— 心跳须"
                            " sleep-first（建连即发帧会往假设备腿注入额外帧，test_m4k1/"
                            "m4k3/RCL 三件真跑 ws_gateway 的零偏移前提翻车）。")
                        # 一个周期（>interval）→ 恰一帧。
                        dev2 = _BlockingDeviceSocket(auth_header="Bearer " + token)
                        asyncio.run(_drive_gateway(
                            front, dev2, up, max_seconds=1.0,
                            stop_when=lambda d: len(_keepalive_frames(d.sent_text)) >= 1))
                        ka = _keepalive_frames(dev2.sent_text)
                        self.assertEqual(
                            len(ka), 1,
                            f"SCRF-B1 RED: 一个周期内 keepalive 帧数 = {len(ka)}（冻结"
                            "恰 1）—— 心跳 producer 缺席（0）或非单帧（多帧/重复）。"
                            f" 实收 {dev2.sent_text!r}")
                        self.assertEqual(
                            ka[0], {"type": KEEPALIVE_TYPE},
                            f"SCRF-B1 RED: keepalive 帧形状 = {ka[0]!r}（契约 §四冻结"
                            ' {"type":"kidpal_keepalive"}，裸 send_text 非 display 帧）'
                            " —— 心跳被套 display/JSON-RPC 封套或字段漂移。")

    # coverage: SCRF-B1 -- env KIDPAL_CHANNEL_HEARTBEAT=0 → 零帧（回滚开关有效）
    def test_scr_b1_heartbeat_disabled_sends_zero_frames(self):
        with tempfile.TemporaryDirectory(prefix="scr_b1_off_") as tmp:
            with _front_with_stories(tmp) as front:
                token = _token()
                with _env_var(HEARTBEAT_ENV, "0"):
                    with _gateway_env(front, token, tmp):
                        dev = _BlockingDeviceSocket(auth_header="Bearer " + token)
                        up = _SilentUpstream()
                        with _fake_engine_connect(up):
                            asyncio.run(_drive_gateway(
                                front, dev, up, max_seconds=0.7,
                                stop_when=lambda d: False))
                        self.assertEqual(
                            len(_keepalive_frames(dev.sent_text)), 0,
                            "SCRF-B1 RED: KIDPAL_CHANNEL_HEARTBEAT=0 仍发心跳 —— 回滚"
                            "开关失效（关不掉 = 旧固件收到 keepalive 落 WARN 噪声）。")

    # coverage: SCRF-B1 -- 发送抛异常 → task 静默退出、双泵 finally 三件仍执行
    #            （cancel(ws_disconnect) + 腿摘除；连接收尾不被心跳异常拖垮）
    def test_scr_b1_heartbeat_send_error_is_silent_teardown_still_runs(self):
        with tempfile.TemporaryDirectory(prefix="scr_b1_err_") as tmp:
            with _front_with_stories(tmp) as front:
                token = _token()
                hexval = "e1e1e1e1e1e1e1e1e1e1e1e1e1e1e1e1"
                sid = "sess-" + hexval
                with _gateway_env(front, token, tmp, fake_uuid_hex=hexval):
                    acc = front._get_story_acc()
                    acc.start(sid, now=_t0())
                    cancel_calls = _spy_cancel(acc)
                    dev = _BlockingDeviceSocket(auth_header="Bearer " + token,
                                                raise_on_keepalive=True)
                    up = _SilentUpstream()
                    with _fake_engine_connect(up):
                        # 跑过 >1 个周期（心跳发送必抛）；不因异常外泄崩溃。
                        asyncio.run(_drive_gateway(
                            front, dev, up, max_seconds=0.7,
                            stop_when=lambda d: False))
                    # 双泵 finally 三件：cancel(reason=ws_disconnect) 落（腿摘除同块）。
                    self.assertIn(
                        (sid, WS_DISCONNECT_REASON),
                        [(s, r) for s, r in cancel_calls],
                        f"SCRF-B1 RED: 心跳发送异常后 finally 未落 cancel(ws_disconnect)"
                        f"（实记 {cancel_calls}）—— 心跳异常拖垮收尾链（双泵 finally 三件"
                        "被吞）。")


# =========================================================================== #
# SCRF-B2 — listen stop 分层收尾                                                  #
# =========================================================================== #


class ScrfListenStopLayeredAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B2 ①+② -- chat 问答会话 stop 零动作零注入；accumulate + accum 非空
    #            已被 _story_finish_session 消费、收尾三件不得再动（恰一篇归档 + 恰一帧
    #            start_organizing）
    def test_scr_b2_chat_noop_and_accumulate_consumed_once(self):
        with tempfile.TemporaryDirectory(prefix="scr_b2_a_") as tmp:
            with _front_with_stories(tmp) as front:
                # ① chat 会话：无 ctx，直接 listen stop -> 零动作。
                sid = "sess-b2-chat"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _listen_stop_msg()}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                self.assertEqual(front._get_story_acc().routing(sid), "chat",
                                 "SCRF-B2 RED: chat 问答会话 stop 后被误切记录态。")
                self.assertEqual(_release_frames(up.sent), [],
                                 "SCRF-B2 RED: chat 会话 stop 触发了记录语境释放注入"
                                 "（标记假时须零注入——防每轮问答打一帧无用 active=false）。")
                self.assertEqual(_start_organizing_frames(dev.sent_text), [],
                                 "SCRF-B2 RED: chat 会话 stop 触发收尾演出。")

        with tempfile.TemporaryDirectory(prefix="scr_b2_b_") as tmp:
            with _front_with_stories(tmp) as front:
                # ② accumulate + accum 非空：ctx(story) + stt 喂入 -> listen stop -> 归档一次。
                sid = "sess-b2-acc"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                _engine_to_device(front, sid, [_stt_frame("从前有一只小猫")])
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "accumulate",
                                 "SCRF-B2 RED(前置): ctx(story)+stt 后未进累积态。")
                dev.append_text(_listen_stop_msg())
                with _llm_stub(front, _LLMStub("小猫\n从前有一只小猫。")):
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                files = _story_md_files(tmp)
                self.assertEqual(len(files), 1,
                                 f"SCRF-B2 RED: accumulate 非空 stop 归档篇数 = "
                                 f"{len(files)}（冻结恰 1）—— 收尾三件在归档路径已消费后"
                                 "二次动作（双取消/双收尾 = 0 篇或重复）。")
                org = _start_organizing_frames(dev.sent_text)
                self.assertEqual(len(org), 1,
                                 f"SCRF-B2 RED: start_organizing 帧数 = {len(org)}（冻结"
                                 "恰 1）—— 收尾演出被收尾三件重复下发。")

    # coverage: SCRF-B2 ③+④ -- accumulate + accum 空 → pending_stop：禁止 cancel / 禁止
    #            释放（protected）；重复 stop → mark_pending 幂等、不动作
    def test_scr_b2_pending_protected_and_repeated_stop_idempotent(self):
        with tempfile.TemporaryDirectory(prefix="scr_b2_pend_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b2-pend"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                cancel_calls = _spy_cancel(acc)
                with _release_spy(front) as rel:
                    dev.append_text(_listen_stop_msg())  # accum 空 -> pending_stop
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                    self.assertEqual(acc.routing(sid), "pending_stop",
                                     "SCRF-B2 RED: accum 空 stop 后未落 pending_stop —— "
                                     "SAO-A1/A2 挂起态破（立即 cancel = 0 篇归档）。")
                    self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                     "SCRF-B2 RED: accum 空 stop 触发 acc.cancel —— "
                                     "protected pending 被当刻取消（F-01 承重破: 在途转写"
                                     "被毁 = 0 篇归档）。")
                    self.assertEqual(rel, [],
                                     "SCRF-B2 RED: accum 空 stop 触发记录语境释放 —— "
                                     "pending_stop 受保护, 当刻禁止释放（交 10s sweep）。")
                    # ④ 重复 stop：幂等 no-op。
                    dev.append_text(_listen_stop_msg())
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B2 RED: 重复 stop 后 pending_stop 态漂移 —— "
                                 "mark_pending 幂等破。")
                self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                 "SCRF-B2 RED: 重复 stop 触发了 cancel —— 幂等 no-op 破。")
                self.assertEqual(_story_md_files(tmp), [],
                                 "SCRF-B2 RED: pending 态产生了归档 —— 未到达 stt 不该成篇。")


# =========================================================================== #
# SCRF-B2b — pending 保护与 sweep 释放                                            #
# =========================================================================== #


class ScrfPendingSweepReleaseAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B2b -- listen stop 后 +9s 零释放、+10s 由 sweep 释放且 active=false
    #            恰一帧入归属腿（腿集 = _session_legs(sid) or list(_alert_legs)）
    def test_scr_b2b_sweep_releases_at_ten_seconds(self):
        with tempfile.TemporaryDirectory(prefix="scr_b2b_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b2b"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                dev.append_text(_listen_stop_msg())  # accum 空 -> pending_stop
                t0 = datetime.now(dt_timezone.utc)
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B2b RED(前置): 未落 pending_stop。")
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    # +9s：仍在 10s 窗内 -> 零释放。
                    with _release_spy(front) as rel9:
                        asyncio.run(front._story_timeout_sweep(
                            now=t0 + timedelta(seconds=9)))
                    self.assertEqual(rel9, [],
                                     "SCRF-B2b RED: +9s 就释放了 pending —— 10s 窗口"
                                     "上界（PENDING_STOP_SECONDS=10.0）破。")
                    self.assertEqual(acc.routing(sid), "pending_stop",
                                     "SCRF-B2b RED: +9s pending 态被提前释放。")
                    # +10.5s：过窗 -> sweep 释放 + 恰一帧 active=false。
                    with _release_spy(front) as rel10:
                        asyncio.run(front._story_timeout_sweep(
                            now=t0 + timedelta(seconds=10.5)))
                    self.assertEqual(rel10, [sid],
                                     f"SCRF-B2b RED: +10s sweep 未释放记录语境（实记 "
                                     f"{rel10}）—— pending 超时分支只 release_pending 不调"
                                     "_release_record_context（常驻后设备永久哑火 = 承重"
                                     "缺口; plan T7）。")
                    self.assertEqual(acc.routing(sid), "chat",
                                     "SCRF-B2b RED: sweep 释放后 routing 未回 chat。")
                    rel_frames = _release_frames(up_leg.sent)
                    self.assertEqual(
                        len(rel_frames), 1,
                        f"SCRF-B2b RED: sweep 释放 active=false 帧数 = {len(rel_frames)}"
                        "（冻结恰 1）—— 释放未注入归属引擎腿（腿集须 = _session_legs(sid) "
                        "or list(_alert_legs); 漏发 = 引擎对该连接永久抑制）。")

    # coverage: SCRF-B2b -- 先投 stt 走恢复归档路径则 sweep 不重复释放
    def test_scr_b2b_stt_recovery_prevents_double_release(self):
        with tempfile.TemporaryDirectory(prefix="scr_b2b_rec_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b2b-rec"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                dev.append_text(_listen_stop_msg())
                t0 = datetime.now(dt_timezone.utc)
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B2b RED(前置): 未落 pending_stop。")
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    with _release_spy(front) as rel:
                        # stt 到达 -> pending_stop 恢复归档单源（_story_finish_session 内释放一次）。
                        with _llm_stub(front, _LLMStub("小猫\n从前有一只小猫。")):
                            up_e = _FakeUpstream(frames=[json.dumps(_stt_frame("从前有一只小猫"),
                                                                    ensure_ascii=False)])
                            dev_e = _FakeDeviceSocket()
                            asyncio.run(front.pump_upstream_to_client(up_e, dev_e, sid))
                        self.assertEqual(len(_story_md_files(tmp)), 1,
                                         "SCRF-B2b RED: pending 恢复路径未成一篇归档。")
                        first = list(rel)
                        # +10s sweep：会话已 release -> pending_timed_out 不再命中 -> 不重复释放。
                        asyncio.run(front._story_timeout_sweep(
                            now=t0 + timedelta(seconds=10.5)))
                        self.assertEqual(list(rel), first,
                                         "SCRF-B2b RED: 恢复归档后 sweep 重复释放记录语境"
                                         " —— 双重 active=false（幂等破）。")


# =========================================================================== #
# SCRF-B3 — discard→stop 有序不归档（承重齿）                                      #
# =========================================================================== #


class ScrfDiscardThenStopAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B3 -- 同一 session 依次 discard + listen stop → routing 回 chat、
    #            _story_finish_session 未被调、STORIES_DIR 文件数不变（承重：已丢弃素材
    #            不得被复活成归档）
    def test_scr_b3_discard_then_stop_no_archive(self):
        with tempfile.TemporaryDirectory(prefix="scr_b3_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b3"
                dev = _FakeDeviceSocket(
                    incoming=[{"type": "websocket.receive", "text": _ctx_msg("story")}])
                up = _FakeUpstream()
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                _engine_to_device(front, sid, [_stt_frame("从前有一只小猫")])
                acc = front._get_story_acc()
                self.assertTrue(acc.has_accum(sid),
                                "SCRF-B3 RED(前置): 累积缓冲为空（stt 喂入链回退）。")
                # 同连接同会话：discard 先、listen stop 后（契约 §四顺序约束）。
                dev.append_text(_discard_msg())
                dev.append_text(_listen_stop_msg())
                with _finish_spy(front) as fin:
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                self.assertEqual(acc.routing(sid), "chat",
                                 "SCRF-B3 RED: discard 后 routing 未回 chat —— 丢弃未落"
                                 "（丢弃不入册的反面：素材滞留累积态, 稍后被 sweep 归档）。")
                self.assertEqual(fin, [],
                                 f"SCRF-B3 RED: discard→stop 触发了 _story_finish_session"
                                 f"（实记 {fin}）—— 丢弃素材被复活成归档（契约 §四顺序"
                                 "约束破; S-F2.2-AC4 毁稿）。")
                self.assertEqual(_story_md_files(tmp), [],
                                 "SCRF-B3 RED: discard→stop 落盘了故事 —— 「素材不入册」"
                                 "被破（0 篇归档为承重断言）。")


# =========================================================================== #
# SCRF-B4 / B4b — ctx 边沿兜底分态                                                #
# =========================================================================== #


class ScrfCtxEdgeFallbackAcceptance(unittest.TestCase):
    maxDiff = None

    def _seed_marked(self, front, sid, dev, up):
        """同 device websocket 投 ctx(story)：routing accumulate + _record_ctx_seen 置位。"""
        dev.append_text(_ctx_msg("story"))
        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
        self.assertEqual(front._get_story_acc().routing(sid), "accumulate",
                         "SCRF-B4 RED(前置): ctx(story) 后未进累积态。")

    # coverage: SCRF-B4 -- accumulate + 标记真 → cancel(reason=session_end_stale) + 释放；
    #            重复 ctx(非 story) → 零新增注入（幂等）
    def test_scr_b4_accumulate_cancels_and_releases(self):
        with tempfile.TemporaryDirectory(prefix="scr_b4_acc_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b4-acc"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    self._seed_marked(front, sid, dev, up)
                    acc = front._get_story_acc()
                    cancel_calls = _spy_cancel(acc)
                    with _release_spy(front) as rel:
                        dev.append_text(_ctx_msg("home"))
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                        self.assertIn((sid, END_STALE_REASON), cancel_calls,
                                      f"SCRF-B4 RED: 非 story ctx 后未落 "
                                      f"cancel(reason={END_STALE_REASON!r})（实记 "
                                      f"{cancel_calls}）—— ctx 边沿兜底未按 accumulate 分态"
                                      "取消（常驻时代 cancel-on-disconnect 同族）。")
                        self.assertEqual(rel, [sid],
                                         f"SCRF-B4 RED: 非 story ctx 后未释放记录语境"
                                         f"（实记 {rel}）—— 引擎对该连接永久抑制（哑火）。")
                        self.assertEqual(acc.routing(sid), "chat",
                                         "SCRF-B4 RED: accumulate 兜底后 routing 未回 chat。")
                        # 重复 ctx(非 story)：标记已清（_release_record_context 单源清位）
                        # -> 零新增注入。
                        rel_after = list(rel)
                        dev.append_text(_ctx_msg("home"))
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                        self.assertEqual(list(rel), rel_after,
                                         "SCRF-B4 RED: 重复 ctx(非 story) 再次释放 —— "
                                         "幂等破（重复 ctx 零新增注入）。")

    # coverage: SCRF-B4 -- pending_stop + 标记真 → 不动作（保护在途转写）
    def test_scr_b4_pending_stop_no_action(self):
        with tempfile.TemporaryDirectory(prefix="scr_b4_pend_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b4-pend"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                self._seed_marked(front, sid, dev, up)
                dev.append_text(_listen_stop_msg())  # accum 空 -> pending_stop
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B4 RED(前置): 未落 pending_stop。")
                cancel_calls = _spy_cancel(acc)
                with _release_spy(front) as rel:
                    dev.append_text(_ctx_msg("home"))
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                    self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                     "SCRF-B4 RED: pending_stop 收到非 story ctx 被 cancel "
                                     "—— 在途转写被毁（T8 保护态破）。")
                    self.assertEqual(rel, [],
                                     "SCRF-B4 RED: pending_stop 收到非 story ctx 被释放 —— "
                                     "当刻禁释放（≤10s 上界交 sweep）。")
                    self.assertEqual(acc.routing(sid), "pending_stop",
                                     "SCRF-B4 RED: pending_stop 收到非 story ctx 后态漂移。")

    # coverage: SCRF-B4 -- chat 但标记真 → 只释放（不 cancel、不归档）
    def test_scr_b4_chat_marked_only_release(self):
        with tempfile.TemporaryDirectory(prefix="scr_b4_chat_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b4-chat"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                dev_leg, up_leg = _FakeDeviceSocket(), _FakeUpstream()
                with _guide_legs(front, sid, dev_leg, up_leg):
                    self._seed_marked(front, sid, dev, up)
                    acc = front._get_story_acc()
                    # 让 routing 回 chat 而标记仍真：直接 cancel（pop 会话，不触
                    # _release_record_context -> 标记保持真）。
                    acc.cancel(sid)
                    self.assertEqual(acc.routing(sid), "chat",
                                     "SCRF-B4 RED(前置): cancel 后 routing 非 chat。")
                    cancel_calls = _spy_cancel(acc)
                    with _release_spy(front) as rel:
                        dev.append_text(_ctx_msg("home"))
                        asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                        self.assertEqual(rel, [sid],
                                         f"SCRF-B4 RED: chat+标记真未只释放（实记 {rel}）"
                                         " —— 记录语境残活 = 引擎永久抑制。")
                        self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                         "SCRF-B4 RED: chat+标记真分支误 cancel —— "
                                         "只释放不 cancel（会话已不存在）。")

    # coverage: SCRF-B4b -- pending_stop 分支不动作（F-01 ctx 边沿复发防线）+ 随后 stt
    #            仍恰一篇归档
    def test_scr_b4b_pending_stop_edge_then_stt_archives_once(self):
        with tempfile.TemporaryDirectory(prefix="scr_b4b_") as tmp:
            with _front_with_stories(tmp) as front:
                sid = "sess-b4b"
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                self._seed_marked(front, sid, dev, up)
                dev.append_text(_listen_stop_msg())
                asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                acc = front._get_story_acc()
                self.assertEqual(acc.routing(sid), "pending_stop",
                                 "SCRF-B4b RED(前置): 未落 pending_stop。")
                cancel_calls = _spy_cancel(acc)
                with _release_spy(front) as rel:
                    dev.append_text(_ctx_msg("home"))
                    asyncio.run(front.pump_client_to_upstream(dev, up, sid))
                    self.assertEqual([c for c in cancel_calls if c[0] == sid], [],
                                     "SCRF-B4b RED: ctx 边沿把 pending_stop 取消了 —— F-01"
                                     " 从 ctx 边沿复发（在途转写被毁）。")
                    self.assertEqual(rel, [],
                                     "SCRF-B4b RED: pending_stop 分支触发了释放 —— 保护态破。")
                    self.assertEqual(acc.routing(sid), "pending_stop",
                                     "SCRF-B4b RED: pending_stop 态被非 story ctx 改掉。")
                # 随后 stt 到达 -> 恢复归档恰一篇。
                with _llm_stub(front, _LLMStub("小猫\n从前有一只小猫。")):
                    up_e = _FakeUpstream(frames=[json.dumps(_stt_frame("从前有一只小猫"),
                                                            ensure_ascii=False)])
                    dev_e = _FakeDeviceSocket()
                    asyncio.run(front.pump_upstream_to_client(up_e, dev_e, sid))
                files = _story_md_files(tmp)
                self.assertEqual(len(files), 1,
                                 f"SCRF-B4b RED: pending_stop 恢复路径归档篇数 = "
                                 f"{len(files)}（冻结恰 1）—— 保护态守住了但收尾链断（0 篇）"
                                 "或重复成篇。")


# =========================================================================== #
# SCRF-B5 — 连接级 teardown 保绿（must-stay-green）                                #
# =========================================================================== #


class ScrfTeardownAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B5 -- 设备腿断连 + 引擎腿静默 → 限时收尾完成 + 腿双摘除 +
    #            acc.cancel(reason="ws_disconnect") 逐字不变（OQ2 保绿）
    def test_scr_b5_teardown_completes_cancel_and_legs_removed(self):
        with tempfile.TemporaryDirectory(prefix="scr_b5_") as tmp:
            with _front_with_stories(tmp) as front:
                token = _token()
                hexval = "b5b5b5b5b5b5b5b5b5b5b5b5b5b5b5b5"
                sid = "sess-" + hexval
                with _gateway_env(front, token, tmp, fake_uuid_hex=hexval):
                    acc = front._get_story_acc()
                    acc.start(sid, now=_t0())
                    acc.feed(sid, "从前有一只小猫", now=_t0())
                    cancel_calls = _spy_cancel(acc)
                    dev = _GatewayDeviceSocket(auth_header="Bearer " + token)
                    up = _SilentUpstream()
                    with _fake_engine_connect(up):
                        try:
                            asyncio.run(asyncio.wait_for(
                                front.ws_gateway(dev), timeout=6.0))
                        except (asyncio.TimeoutError, TimeoutError):
                            self.fail(
                                "SCRF-B5 RED: 设备腿已断连且引擎腿静默, ws_gateway 收尾未"
                                "在限时内完成 —— 心跳 task 进了 FIRST_COMPLETED 集合（无限"
                                "任务拖住收尾; plan T5 承重约束）。")
                    self.assertIn(
                        (sid, WS_DISCONNECT_REASON),
                        [(s, r) for s, r in cancel_calls],
                        f"SCRF-B5 RED: 断连收尾 cancel(ws_disconnect) 未落（实记 "
                        f"{cancel_calls}）—— OQ2 cancel-on-disconnect 维持被破。")
                    self.assertNotIn(sid, acc.active_sessions(),
                                     "SCRF-B5 RED: 断连收尾后会话未 pop —— 累积缓冲泄漏。")
                    self.assertFalse(
                        any(pair[0] is dev for pair in list(front._alert_legs)),
                        "SCRF-B5 RED: 断连收尾后 _alert_legs 仍含本连接腿 —— zombie 腿泄漏。")


# =========================================================================== #
# SCRF-B6 — 既有冻结 oracle 保绿回归网（presence 守卫 + suite 绿留痕）              #
# =========================================================================== #


class ScrfOracleRegressionAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SCRF-B6 -- 既有冻结 oracle 文件与关键 frozen 测试函数在世（presence 守卫；
    #            行为绿 = 全量 pytest server/acceptance exit 0 留痕，非本单齿）
    def test_scr_b6_frozen_oracles_present(self):
        oracles = {
            "test_m4k1_story_cloud_link.acceptance.py": (
                "server/acceptance/test_m4k1_story_cloud_link.acceptance.py",
                ("sao", "fix_a1"),),
            "test_m4k3_record_session_live.acceptance.py": (
                "server/acceptance/test_m4k3_record_session_live.acceptance.py",
                ("test_a7_ctx_non_story_pushes_zero_frames",
                 "test_a7_push_seam_lands_in_ctx_consume_not_connect"),),
            "test_record_cancel_lifecycle.acceptance.py": (
                "server/acceptance/test_record_cancel_lifecycle.acceptance.py",
                ("first_completed",),),
            "test_record_front_polish.acceptance.py": (
                "server/acceptance/test_record_front_polish.acceptance.py",
                (),),
        }
        for label, (rel, needles) in oracles.items():
            path = REPO_ROOT / rel
            self.assertTrue(path.is_file(),
                            f"SCRF-B6 RED: 既有冻结 oracle {label} 丢失 —— 心跳/收尾三件"
                            "若改了其断言不得静默删件（须走 planner 扩单通道）。")
            text = path.read_text(encoding="utf-8").lower() if path.is_file() else ""
            for needle in needles:
                self.assertIn(needle, text,
                              f"SCRF-B6 RED: {label} 缺关键 frozen 锚 {needle!r} —— "
                              "既有 oracle 被改弱/删除（回归网破; 任一红不得进 integrator）。")


if __name__ == "__main__":
    unittest.main()