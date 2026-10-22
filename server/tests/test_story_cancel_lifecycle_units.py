"""KidPal 261021-p3-record-cancel-lifecycle 服务端自带测 — 取消/丢弃生命周期实现分支。

与冻结验收的分工：server/acceptance/test_record_cancel_lifecycle.acceptance.py 是**规格**
（RCL-A1..A4，一字不改）；本文件是实现自带的分支 / 边界 / 错误路径测（builder 义务：
单测覆盖分支、边界与至少一个错误路径，不只 happy path）。冻结件未覆盖到的实现细节在
本文件独立钉住：

- [SC-D] 腿集会话归口映射缝的纯函数面（登记 / 查询 / 按 device_ws 身份摘除 / 未知键
  空表 / 查询零副作用）——含「摘除一连接不误伤他连接」的隔离边界；
- discard 消费分支的**错误路径**（非 dict 载荷不消费 / 未知会话不建会话壳 / 重复
  discard 幂等）与连接权威（载荷 session_id 只作审计、不作取消键）；
- 在途引导回合双点守卫（入口守卫 / LLM 返回后复查）与「腿集归口不打全局腿」的正向
  分支；自动收尾同款归口（只打归属连接腿）。

纯 stdlib（front_app 依赖 fastapi，本地缺席走同款 stub）+ 假腿双桩，零网络：
  cd server && python -m pytest -q tests/test_story_cancel_lifecycle_units.py
"""

from __future__ import annotations

import asyncio
import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

SERVER = Path(__file__).resolve().parents[1]
FRONT_APP = SERVER / "gateway" / "front" / "front_app.py"
if str(SERVER) not in sys.path:
    sys.path.insert(0, str(SERVER))


_MISSING = object()
_front_module = None


def _ensure_fastapi_stub() -> None:
    """front_app 离线装载的 fastapi stub（运行环境无 fastapi 时最小替身）。"""
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
    sys.modules.setdefault("fastapi.responses", _PermissiveModule("fastapi.responses"))


def _front():
    global _front_module
    if _front_module is not None:
        return _front_module
    _ensure_fastapi_stub()
    spec = importlib.util.spec_from_file_location("kidpal_front_app_ut", FRONT_APP)
    assert spec is not None and spec.loader is not None
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    _front_module = mod
    return mod


class _Device:
    """设备腿最小双桩（只收 send_text 帧）。"""

    def __init__(self):
        self.sent_text = []

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        pass


class _Engine:
    """引擎腿最小双桩（只收 send 帧）。"""

    def __init__(self):
        self.sent = []

    async def send(self, message):
        self.sent.append(message)


def _display_payloads(sent_text):
    """设备腿 mcp 帧 -> [{"view","data"}]（kidpal/display tools/call 解包）。"""
    out = []
    for frame in sent_text:
        try:
            obj = json.loads(frame)
        except (TypeError, ValueError):
            continue
        if not (isinstance(obj, dict) and obj.get("type") == "mcp"
                and isinstance(obj.get("payload"), dict)):
            continue
        payload = obj["payload"]
        params = payload.get("params") or {}
        if payload.get("method") != "tools/call" or params.get("name") != "kidpal/display":
            continue
        args = params.get("arguments") or {}
        try:
            data = json.loads(args.get("data")) if isinstance(args.get("data"), str) else None
        except (TypeError, ValueError):
            data = None
        out.append({"view": args.get("view"), "data": data})
    return out


def _start_organizing(sent_text):
    return [p for p in _display_payloads(sent_text)
            if p["view"] == "control" and (p["data"] or {}).get("action") == "start_organizing"]


def _views(sent_text):
    return [p["view"] for p in _display_payloads(sent_text)]


def _speak(engine_sent):
    """引擎腿 kidpal_speak 帧（注入走 json.dumps 串，逐帧解码）。"""
    out = []
    for item in engine_sent:
        try:
            obj = json.loads(item) if isinstance(item, (str, bytes, bytearray)) else item
        except (TypeError, ValueError):
            continue
        if isinstance(obj, dict) and obj.get("type") == "kidpal_speak":
            out.append(obj)
    return out


class _FrontSandbox(unittest.TestCase):
    """front 模块全局态的 set/restore 底座（存盘隔离 + 腿面清空 + LLM 桩还原）。"""

    def setUp(self):
        self.front = _front()
        self._tmp = tempfile.TemporaryDirectory(prefix="scl_ut_")
        f = self.front
        self._saved = {
            "stories": f.STORIES_DIR,
            "acc": f._story_acc,
            "alert_legs": list(f._alert_legs),
            "session_legs": dict(f._session_legs_map),
            "llm": getattr(f, "_openclaw_chat_once", _MISSING),
        }
        f.STORIES_DIR = self._tmp.name
        f._story_acc = None
        f._alert_legs.clear()
        f._session_legs_map.clear()
        self.acc = f._get_story_acc()

    def tearDown(self):
        f = self.front
        f._alert_legs[:] = self._saved["alert_legs"]
        f._session_legs_map.clear()
        f._session_legs_map.update(self._saved["session_legs"])
        f.STORIES_DIR = self._saved["stories"]
        f._story_acc = self._saved["acc"]
        if self._saved["llm"] is _MISSING:
            f.__dict__.pop("_openclaw_chat_once", None)
        else:
            f._openclaw_chat_once = self._saved["llm"]
        self._tmp.cleanup()

    def archives(self):
        return list(Path(self._tmp.name).glob("*.md"))


# =========================================================================== #
# [SC-D] 腿集会话归口映射缝 — 纯函数面                                        #
# =========================================================================== #


class SessionLegsMapUnits(_FrontSandbox):
    """[SC-D] _register_session_legs / _session_legs / _unregister_session_legs。"""

    def test_register_query_unknown_and_no_side_effect(self):
        """登记后查询返回（且仅返回）该会话腿对；未知键空表；查询不改状态。"""
        f = self.front
        dev_a, eng_a = _Device(), _Engine()
        f._register_session_legs("sess-a", dev_a, eng_a)
        self.assertEqual(f._session_legs("sess-a"), [(dev_a, eng_a)])
        self.assertEqual(f._session_legs("sess-unknown"), [])
        # 查询零副作用：重复查询结果一致，且返回副本（改动返回列表不污染映射）。
        snapshot = f._session_legs("sess-a")
        snapshot.clear()
        self.assertEqual(f._session_legs("sess-a"), [(dev_a, eng_a)])

    def test_unregister_by_device_identity_isolates_other_connections(self):
        """按 device_ws **对象身份**摘除：本连接全部会话清空、他连接不受影响。"""
        f = self.front
        dev_a, eng_a = _Device(), _Engine()
        dev_b, eng_b = _Device(), _Engine()
        f._register_session_legs("sess-a1", dev_a, eng_a)
        f._register_session_legs("sess-a2", dev_a, eng_a)
        f._register_session_legs("sess-b", dev_b, eng_b)
        f._unregister_session_legs(dev_a)
        self.assertEqual(f._session_legs("sess-a1"), [])
        self.assertEqual(f._session_legs("sess-a2"), [])
        self.assertEqual(f._session_legs("sess-b"), [(dev_b, eng_b)])

    def test_register_empty_session_id_is_noop(self):
        """边界：空/None 会话键不登记（不产生不可查询的孤儿表项）。"""
        f = self.front
        f._register_session_legs("", _Device(), _Engine())
        f._register_session_legs(None, _Device(), _Engine())
        self.assertEqual(f._session_legs_map, {})


# =========================================================================== #
# RCL-A1 实现分支 — discard 消费（连接权威 / 幂等 / 错误路径）                  #
# =========================================================================== #


class _Obj(dict):
    """把 dict 当对象用（_consume_kidpal_control 只做 get 读取）。"""


class DiscardConsumeUnits(_FrontSandbox):
    """_consume_kidpal_control 的 kidpal_story_discard 分支。"""

    def _accumulating(self, sid, text="从前有一只小猫"):
        self.acc.start(sid)
        self.acc.feed(sid, text)

    def test_discard_cancels_with_connection_reason_and_audits(self):
        """累积态收到 discard -> cancel(reason="discard") + 会话 pop + 不归档 + 审计一行。

        错误路径面：cancel 抛异常也不得外抛（消费拦截语义优先）由实现 try/except 承载，
        这里以「未知会话 + 非 dict 载荷」两条边界独立钉住。"""
        f = self.front
        sid = "sess-ut-discard"
        self._accumulating(sid)
        calls = []
        orig = self.acc.cancel

        def _spy(session_id, now=None, *, reason=""):
            calls.append((session_id, reason))
            return orig(session_id, now=now, reason=reason)

        self.acc.cancel = _spy
        try:
            with self.assertLogs("kidpal-front", level="INFO") as cm:
                consumed = asyncio.run(f._consume_kidpal_control(
                    sid, {"session_id": "sess-forged", "type": "kidpal_story_discard"},
                    _Device()))
        finally:
            self.acc.cancel = orig
        self.assertTrue(consumed, "UT RED: discard 未被整条消费（会下透引擎）。")
        self.assertEqual(calls, [(sid, "discard")])
        self.assertNotIn(sid, self.acc.active_sessions())
        self.assertIsNone(self.acc.build_organizing_input(sid))
        self.assertEqual(self.archives(), [])
        audit = [r.getMessage() for r in cm.records
                 if "story_discard" in r.getMessage() and sid in r.getMessage()]
        self.assertTrue(audit, f"UT RED: 审计行缺失（实收 {[r.getMessage() for r in cm.records]!r}）。")

    def test_discard_connection_authority_ignores_payload_session_id(self):
        """载荷 session_id 不被信任：B 连接 discard 不取消 A 连接会话、不碰 A 素材。"""
        f = self.front
        self._accumulating("sess-ut-a", "小猫在山上")
        self._accumulating("sess-ut-b", "小狗在河边")
        consumed = asyncio.run(f._consume_kidpal_control(
            "sess-ut-b", {"session_id": "sess-ut-a", "type": "kidpal_story_discard"},
            _Device()))
        self.assertTrue(consumed)
        self.assertEqual(self.acc.routing("sess-ut-a"), "accumulate")
        self.assertIn("小猫在山上", self.acc.build_organizing_input("sess-ut-a") or "")
        self.assertNotEqual(self.acc.routing("sess-ut-b"), "accumulate")

    def test_discard_idempotent_and_unknown_session_no_shell(self):
        """重复 discard / 未知会话 discard = no-op：不崩、不建会话壳、零归档。"""
        f = self.front
        sid = "sess-ut-idem"
        self._accumulating(sid)
        frame = {"session_id": sid, "type": "kidpal_story_discard"}
        self.assertTrue(asyncio.run(f._consume_kidpal_control(sid, frame, _Device())))
        for _ in range(2):
            self.assertTrue(asyncio.run(f._consume_kidpal_control(sid, frame, _Device())))
        self.assertNotIn(sid, self.acc.active_sessions())
        self.assertEqual(self.acc.active_sessions(), [])
        self.assertEqual(self.archives(), [])
        self.assertTrue(asyncio.run(f._consume_kidpal_control(
            "sess-ut-never", frame, _Device())))
        self.assertNotIn("sess-ut-never", self.acc.active_sessions())

    def test_discard_malformed_payload_is_not_consumed(self):
        """错误路径：非 dict 载荷不进 discard 分支（返回 False，交调用方转发）。"""
        f = self.front
        self.assertFalse(asyncio.run(f._consume_kidpal_control(
            "sess-ut-bad", ["not", "a", "dict"], _Device())))
        self.assertFalse(asyncio.run(f._consume_kidpal_control(
            "sess-ut-bad", None, _Device())))


# =========================================================================== #
# RCL-A2 实现分支 — 在途引导回合双点存续守卫                                    #
# =========================================================================== #


class GuideRoundGuardUnits(_FrontSandbox):
    """_story_guide_round 的入口守卫 / 播报前复查 / 腿集归口。"""

    def test_entry_guard_no_session_skips_llm(self):
        """入口守卫：无会话（build_organizing_input None）-> 零 LLM / 零帧 / 零壳。"""
        f = self.front
        calls = []

        def _llm(system_text, user_text, timeout_s=10.0):
            calls.append(user_text)
            return "那后来呢？"

        f._openclaw_chat_once = _llm
        dev, eng = _Device(), _Engine()
        f._register_session_legs("sess-ut-guide", dev, eng)
        asyncio.run(f._story_guide_round("sess-ut-guide", None))
        self.assertEqual(calls, [])
        self.assertEqual(_speak(eng.sent), [])
        self.assertEqual(self.acc.active_sessions(), [])
        self.assertEqual(self.acc.guide_rounds("sess-ut-guide"), 0)

    def test_post_llm_recheck_drops_output_after_release(self):
        """播报前复查：LLM 在途期间会话被释放 -> 产出丢弃（不播报、不计轮、不复活壳）。"""
        f = self.front
        sid = "sess-ut-recheck"
        self.acc.start(sid)
        self.acc.feed(sid, "从前有一只小猫")
        dev, eng = _Device(), _Engine()
        f._register_session_legs(sid, dev, eng)

        def _llm_releases(system_text, user_text, timeout_s=10.0):
            self.acc.cancel(sid, reason="ws_disconnect")  # 在途期间释放（断连/discard 同形）
            return "那后来呢？"

        f._openclaw_chat_once = _llm_releases
        asyncio.run(f._story_guide_round(sid, None))
        self.assertEqual(_speak(eng.sent), [])
        self.assertEqual(self.acc.guide_rounds(sid), 0)
        self.assertNotIn(sid, self.acc.active_sessions())

    def test_guide_round_speaks_only_to_session_legs(self):
        """正向分支：引导播报只打会话归属连接腿；全局腿零帧；计轮落账。"""
        f = self.front
        sid = "sess-ut-guide-ok"
        self.acc.start(sid)
        self.acc.feed(sid, "从前有一只小猫")
        dev_own, eng_own = _Device(), _Engine()
        dev_global, eng_global = _Device(), _Engine()
        f._register_session_legs(sid, dev_own, eng_own)
        f._register_alert_legs(dev_global, eng_global)
        f._openclaw_chat_once = lambda system_text, user_text, timeout_s=10.0: "那后来呢？"
        asyncio.run(f._story_guide_round(sid, None))
        self.assertEqual([o.get("text") for o in _speak(eng_own.sent)], ["那后来呢？"])
        self.assertEqual(_speak(eng_global.sent), [],
                         "UT RED: 引导注入溢出全局腿 —— 会话归口被破。")
        self.assertEqual(self.acc.guide_rounds(sid), 1)


# =========================================================================== #
# RCL-A4 实现分支 — 自动收尾腿集归口                                            #
# =========================================================================== #


class AutoFinishLegScopeUnits(_FrontSandbox):
    """_story_auto_finish 只打该会话归属连接腿（不碰他连接 / 全局腿）。"""

    def test_auto_finish_scopes_frames_to_owning_legs(self):
        f = self.front
        sid_a, sid_b = "sess-ut-fin-a", "sess-ut-fin-b"
        for sid, text in ((sid_a, "小狗在河边"), (sid_b, "小猫在山上")):
            self.acc.start(sid)
            self.acc.feed(sid, text)
        dev_a, eng_a = _Device(), _Engine()
        dev_b, eng_b = _Device(), _Engine()
        f._register_session_legs(sid_a, dev_a, eng_a)
        f._register_session_legs(sid_b, dev_b, eng_b)
        f._openclaw_chat_once = lambda system_text, user_text, timeout_s=10.0: (
            "小狗回家\n小狗玩完水就回家了。")
        asyncio.run(f._story_auto_finish(sid_a, None))
        self.assertEqual(len(_start_organizing(dev_a.sent_text)), 1)
        self.assertEqual(_views(dev_a.sent_text), ["control", "story_text", "story_list"])
        self.assertEqual(_start_organizing(dev_b.sent_text), [],
                         "UT RED: 收尾帧溢出到另一会话的腿 —— 归口粒度不足。")
        self.assertEqual(dev_b.sent_text, [])
        self.assertEqual(_speak(eng_b.sent), [])
        self.assertNotIn(sid_a, self.acc.active_sessions())
        self.assertEqual(self.acc.routing(sid_b), "accumulate",
                         "UT RED: 收尾波及他会话状态。")
        self.assertEqual(len(self.archives()), 1)


if __name__ == "__main__":
    unittest.main()
