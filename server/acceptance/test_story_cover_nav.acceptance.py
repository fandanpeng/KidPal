"""Frozen acceptance tests — TaskPackage 261104-k5-story-cover-nav (subsystem:
server/gateway —— story_list P4 共享帧消费 + restore 生产纪律 + story_cover 语境三分)。

对齐权威：
  - docs/contracts/interface-contract.md §1.3③（story_list = P4 故事模块共享数据帧;
    total 唯一计数源; `data.restore` 分流; 显式「打开故事」= `{"view":"story_list","data":{}}`
    薄载荷由 front 截获展开进封皮）; §1.5 step 3（白名单 7→9 + story_list 轻量 floor
    增认 `data:{}` 导航薄载荷 + relay 收窄）; §四 kidpal_session_ctx（source_page 闭集
    home/schedule/points_home/points_ledger/story_cover/story; story_cover = 普通问答）。
  - PROPOSAL §3.1 D-07（后台 story_list 只填缓存不抢前台, 恢复/归档/删除恒 restore:true）
    / D-08（story_list 改 P4 共享帧, 空 data 薄载荷只进封皮, 不新增 story_cover view）。
  - requirements S-F2.6-AC4（语音「打开故事」只到封皮）; S-F2.1 补充规则（冷启动恢复）。

覆盖簇（与 acceptance-author 派发单对齐）:
  C6 -> test_c6_story_list_thin_payload_floor：display_schema 放行 `{"view":"story_list",
        "data":{}}`（薄载荷）, 其他畸形仍拒; 且 front 把薄载荷展开为完整非 restore
        story_list 帧（不降级成友好 text）; 白名单零扩（无 story_cover view）。
  C7 -> test_c7_restore_five_call_sites：五调用点 restore 判定——
          :2453 WS 建连（保持 true, 结构锚）/ :1023 ctx 建立（翻 true, 行为）/
          :1047 删除后（翻 true, 行为）/ :1739 归档成功（翻 true, 行为）/
          :1728 归档失败离页帧（保持 False, 行为; human 2026-11-15 裁定「离页导航帧例外」）。
  C8 -> test_c8_context_three_way：story_cover 普通问答（不 acc.start / 不推 story_list /
        不注记录信号）; story 列表仍记录语境（acc.start + 恰一帧 restore story_list）;
        未知 source_page 拒绝并记 warning（绝不回退成 story）。

性质档（acceptance-evidence「契约条目 ↔ 断言强度申报」节）:
  - C6 display_schema 三态 + front 展开 = 行为级（真调 validate_display / build_display_tool_calls）。
  - C7 ctx/delete/archive-success/archive-fail = 行为级（真跑 pump / 真跑 _story_finish_session +
    罐头整理桩）; WS 建连 restore=true = 源锚代理（连段源码切片 — 行为轮已由 M4K1 FIX-A1
    既有冻结断言承载, 本件不重复造重引擎桩）。
  - C8 = 行为级（真跑 pump + log capture）。
  行为等价假设: 「restore:true 抵达设备」在本件以 wire data.restore 判定; 设备侧「只填缓存
  不导航」的行为本体归 host 件 story_cover_nav.acceptance.c KCN-C5（list_load）+ 真机轮
  （K5S-E4）承载, 两件互不替代。

RED baseline（设计态红 = 实现缺席, 非环境问题）:
  - display_schema.story_list floor 现要求 items 必为 list -> `data:{}` 薄载荷被拒（C6 red）;
  - front_app 无 `_expand_story_nav_intent` -> 薄载荷走 Q7 降级成友好 text（C6 red）;
  - ctx / 删除后 / 归档成功三处 `_push_story_list(...)` 缺 restore 参数（缺省 False）-> C7 red;
  - story_cover 语境已正确（front 闭集含 story_cover）-> C8 部分 must-stay-green。
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import json
import logging
import sys
import tempfile
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

try:  # pragma: no cover - markers only
    import pytest  # noqa: F401
except ImportError:  # pragma: no cover
    pass

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"

_story_module_cache = None
_front_module_cache = None


# --------------------------------------------------------------------------- #
# Module loading（缺失/不可装载 = 设计态红, 非集合错误）                        #
# --------------------------------------------------------------------------- #

def _story_module():
    global _story_module_cache
    if _story_module_cache is not None:
        return _story_module_cache
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.story as st  # noqa: PLC0415 — offline import, cached
    _story_module_cache = st
    return st


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（M4K1 冻结件同款；路由装饰器恒等）。"""
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

        def _identity(self, *args, **kwargs):  # noqa: ANN202
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
        return _front_module_cache, None
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关接线面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k5", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = mod
        return mod, None
    except Exception as e:  # noqa: BLE001 — import 期错误 = 红, 不是崩溃
        return None, f"front_app.py failed to import: {e!r}（须离线可装载）。"


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("K5 server RED: " + err)
    return mod


def _display_schema():
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415
    return ds


def _front_source() -> str:
    try:
        return FRONT_APP.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


# --------------------------------------------------------------------------- #
# I/O doubles / fixtures（M4K1 冻结件同款形态）                                 #
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
        self.sent_bytes.append(data)


def _device_to_engine(front, session_id, incoming_texts):
    dev = _FakeDeviceSocket(
        incoming=[{"type": "websocket.receive", "text": t} for t in incoming_texts])
    up = _FakeUpstream()
    asyncio.run(front.pump_client_to_upstream(dev, up, session_id))
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
            if payload.get("method") == "tools/call" and params.get("name") == "kidpal/display":
                out.append(payload)
    return out


def _wire_arguments(envelope) -> dict:
    return ((envelope or {}).get("params") or {}).get("arguments") or {}


def _display_payloads(sent_text):
    out = []
    for env in _mcp_envelopes(sent_text):
        args = _wire_arguments(env)
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


@contextlib.contextmanager
def _organize_stub(front, reply):
    """整理回合桩（front 冻结 seam `_openclaw_chat_once`）返回 reply；None = 归档失败路径。"""
    def _fake(system_text, user_text, timeout_s=10.0, **kwargs):  # noqa: ANN001
        return reply

    old = getattr(front, "_openclaw_chat_once", None)
    front._openclaw_chat_once = _fake
    try:
        yield
    finally:
        if old is None:
            front.__dict__.pop("_openclaw_chat_once", None)
        else:
            front._openclaw_chat_once = old


@contextlib.contextmanager
def _captured_logs():
    records = []

    class _H(logging.Handler):
        def emit(self, record):  # noqa: D105
            records.append(record)

    handler = _H(level=logging.DEBUG)
    root = logging.getLogger()
    prev_level = root.level
    root.addHandler(handler)
    if root.level > logging.DEBUG or root.level == 0:
        root.setLevel(logging.DEBUG)
    try:
        yield records
    finally:
        root.removeHandler(handler)
        root.setLevel(prev_level)


def _ctx_msg(source_page: str) -> str:
    return json.dumps({"type": "kidpal_session_ctx", "source_page": source_page},
                      ensure_ascii=False)


def _seed(stories_dir: Path, n: int):
    st = _story_module()
    made = []
    for i in range(n):
        rec = st.archive_story(
            str(stories_dir), f"正文-{i}", title=f"故事{i}", slug=f"s{i:02d}",
            date=f"2026-09-{i + 1:02d}")
        made.append(rec)
    return made


# ========================================================================== #
# C6 — display_schema 放行薄载荷 + 畸形仍拒 + 白名单零扩 + front 截获展开      #
# ========================================================================== #

class StoryCoverThinPayloadFloor(unittest.TestCase):
    """coverage: S-F2.6 — story_list `data:{}` 导航薄载荷只进封皮（契约 §1.3③ / §1.5）。"""

    maxDiff = None

    def test_c6_story_list_thin_payload_floor(self):
        ds = _display_schema()

        # tooth 1: `{"view":"story_list","data":{}}` = 导航薄载荷 -> 放行（floor 增认）。
        ok, reason = ds.validate_display({"view": "story_list", "data": {}})
        self.assertTrue(
            ok,
            f"C6 RED: story_list 薄载荷 `{{'view':'story_list','data':{{}}}}` 被真门拒绝 "
            f"({reason}) —— 契约 §1.3③/§1.5「增认 data:{{}} 导航薄载荷」未落地（现状 floor "
            "要求 items 必为数组, 薄载荷无 items -> 拒）。")

        # tooth 2: 完整帧（total + items）仍放行（零偏移）。
        ok2, reason2 = ds.validate_display(
            {"view": "story_list", "data": {"total": 0, "items": []}})
        self.assertTrue(ok2, f"C6 RED: 完整 story_list 帧被拒 ({reason2}) —— 回归被破。")

        # tooth 3: 畸形仍拒（放行不得过宽）。
        bad = [
            ({"view": "story_list", "data": {"total": "x", "items": []}}, "total 非整数"),
            ({"view": "story_list", "data": {"total": 0}}, "items 缺失"),
            ({"view": "story_list", "data": {"total": 0, "items": [], "restore": "yes"}},
             "restore 非布尔"),
            ({"view": "story_list", "data": []}, "data 非对象"),
        ]
        for payload, label in bad:
            okx, _ = ds.validate_display(payload)
            self.assertFalse(
                okx, f"C6 RED: 畸形 story_list（{label}）通过真门 —— 薄载荷放行过宽。")

        # tooth 4: 白名单零扩（不新增 story_cover view / 不造 cover_total 帧）。
        self.assertNotIn(
            "story_cover", ds.VALID_VIEWS,
            "C6 RED: display 白名单出现 story_cover view —— ADR-018 明确否决新增 "
            "story_cover view（story_list.total 唯一计数源）。")

    def test_c6_front_expands_thin_payload_not_downgrade(self):
        front = _front()
        with tempfile.TemporaryDirectory(prefix="k5_c6_") as tmp:
            with _front_with_stories(tmp):
                _seed(Path(tmp), 2)
                block = json.dumps({"view": "story_list", "data": {}}, ensure_ascii=False)
                calls = front.build_display_tool_calls([block])
                self.assertEqual(len(calls), 1,
                                 "C6 RED: 薄载荷块中继数 != 1（不得静默丢块）。")
                payload = _display_payloads(
                    [json.dumps({"type": "mcp", "payload": calls[0]})])[0]
                self.assertEqual(
                    payload.get("view"), "story_list",
                    f"C6 RED: 薄载荷未被 front 展开为 story_list 帧, 而是 {payload.get('view')!r} "
                    "（Q7 降级成友好 text）—— `_expand_story_nav_intent` 未落地或展开帧被自降级"
                    "（front_expanded 未参与 Q7 判据）。")
                data = payload.get("data") or {}
                self.assertIn("items", data,
                              "C6 RED: 展开帧缺 items —— 薄载荷未展开成完整 story_list 帧。")
                self.assertIn("total", data,
                              "C6 RED: 展开帧缺 total —— 展开须由 front 依据 story.py 现算。")
                self.assertFalse(
                    data.get("restore") is True,
                    "C6 RED: 展开帧带 restore:true —— 显式「打开故事」= 主动展示进封皮, "
                    "展开帧不得带 restore（契约 §1.3③ 生产纪律）。")


# ========================================================================== #
# C7 — 五调用点 restore 判定（D-07 / 契约 §1.3③ 生产纪律）                       #
# ========================================================================== #

class StoryListRestoreCallSites(unittest.TestCase):
    """coverage: S-F2.6-AC1 / S-F2.6-AC4 — 后台推送恒 restore:true; 离页导航帧例外 False。"""

    maxDiff = None

    def test_c7_ctx_push_restore_true(self):
        with tempfile.TemporaryDirectory(prefix="k5_c7a_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 2)
                _up, dev = _device_to_engine(front, "sess-k5-ctx", [_ctx_msg("story")])
                pushes = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(pushes), 1,
                    f"C7 RED: ctx(story) 产生 {len(pushes)} 帧 story_list（应恰一帧）。")
                self.assertTrue(
                    pushes[0]["data"].get("restore") is True,
                    "C7 RED: ctx 建立恢复推送（:1023）未带 restore:true —— 后台恢复只填缓存"
                    "不抢前台（契约 §1.3③ 生产纪律）被破。")

    def test_c7_delete_repush_restore_true(self):
        with tempfile.TemporaryDirectory(prefix="k5_c7b_") as tmp:
            with _front_with_stories(tmp) as front:
                made = _seed(Path(tmp), 3)
                del_id = made[-1]["id"]
                _up, dev = _device_to_engine(
                    front, "sess-k5-del",
                    [json.dumps({"type": "kidpal_story_delete", "id": del_id},
                                ensure_ascii=False)])
                pushes = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(pushes), 1,
                    f"C7 RED: 删除后重推 {len(pushes)} 帧 story_list（应恰一帧）。")
                self.assertTrue(
                    pushes[0]["data"].get("restore") is True,
                    "C7 RED: 删除执行后重推（:1047）未带 restore:true —— 对账重推只填缓存不抢"
                    "前台被破。")

    def test_c7_archive_success_restore_true(self):
        with tempfile.TemporaryDirectory(prefix="k5_c7c_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 1)
                sid = "sess-k5-fin-ok"
                acc = front._get_story_acc()
                acc.start(sid)
                front._story_feed_from_stt(
                    sid, {"type": "stt", "text": "从前有一只小猫，它去公园玩了一会儿。"})
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                with _organize_stub(front, "小猫去公园\n从前有一只小猫，它去公园玩了一会儿。"):
                    asyncio.run(front._story_finish_session(sid, acc, [(dev, up)]))
                self.assertTrue(
                    _story_text_frames(dev.sent_text),
                    "C7 RED: 归档成功后未下发 story_text（既有链路偏移）。")
                pushes = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(pushes), 1,
                    f"C7 RED: 归档成功后 story_list 推送 {len(pushes)} 帧（应恰一帧）。")
                self.assertTrue(
                    pushes[0]["data"].get("restore") is True,
                    "C7 RED: 归档成功后推送（:1739）未带 restore:true —— 三时点② 后台更新须"
                    "只填缓存不抢前台（契约 §1.3③ D-07）。")

    def test_c7_archive_failure_offpage_restore_false(self):
        """离页导航帧例外（human 2026-11-15 裁定）: 归档失败补 story_list 页面退出帧
        （front_app.py:1728）**保持 restore=False** —— 该帧是「离页导航帧」（把设备从困死的
        整理态带回列表），不是后台数据更新; 若翻 true 设备只填缓存不接管前台 -> 困死复现。
        本断言以断言钉住这道例外, 并留痕「离页导航帧例外」。"""
        with tempfile.TemporaryDirectory(prefix="k5_c7d_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 2)
                sid = "sess-k5-fin-fail"
                acc = front._get_story_acc()
                acc.start(sid)
                front._story_feed_from_stt(
                    sid, {"type": "stt", "text": "从前有一只小猫，它去公园玩了一会儿。"})
                dev = _FakeDeviceSocket()
                up = _FakeUpstream()
                with _organize_stub(front, None):  # 整理回合失败 -> 不归档
                    asyncio.run(front._story_finish_session(sid, acc, [(dev, up)]))
                self.assertFalse(
                    _story_text_frames(dev.sent_text),
                    "C7 RED(前提): 归档失败路径不应下发 story_text —— fixture 或链路偏移。")
                pushes = _story_list_frames(dev.sent_text)
                self.assertTrue(
                    pushes,
                    "C7 RED: 归档失败路径未补 story_list 离页帧 —— 设备困死整理态（261021 真机轮 "
                    "R6 缺陷形态复现）。")
                self.assertFalse(
                    pushes[0]["data"].get("restore") is True,
                    "C7 RED: 归档失败离页帧（:1728）被置 restore:true —— 违反 human 2026-11-15 "
                    "裁定「离页导航帧例外保持 False」（离页帧须接管前台把设备带回列表, 翻 true "
                    "即困死整理态复现; 该例外须契约措辞排除, 见 review-packet）。")

    def test_c7_ws_connect_call_site_restore_true(self):
        """WS 建连恢复推送（:2453）保持 restore=True（结构源锚代理）。"""
        src = _front_source()
        self.assertTrue(bool(src), "C7 RED(infra): front_app.py 源码读取失败。")
        accept_pos = src.find("await websocket.accept()")
        pump_pos = src.find("pump_up = asyncio.create_task")
        self.assertTrue(
            accept_pos != -1 and pump_pos != -1 and accept_pos < pump_pos,
            "C7 RED(infra): front_app.py 建连段锚漂移（accept/pump）—— 须复核本断言锚。")
        connect_seg = src[accept_pos:pump_pos]
        self.assertIn(
            "_push_story_list", connect_seg,
            "C7 RED: WS 建连段无 _push_story_list 调用 —— 建连恢复推送缺失。")
        self.assertIn(
            "restore=True", connect_seg,
            "C7 RED: WS 建连段 _push_story_list 未显式传 restore=True —— 建连恢复推送缺 "
            "data.restore 载体（缺省 False）。")


# ========================================================================== #
# C8 — 语境三分（契约 §四 kidpal_session_ctx; D-08）                            #
# ========================================================================== #

class SessionCtxThreeWay(unittest.TestCase):
    """coverage: S-F2.6-AC4 — story_cover 普通问答 / story 记录语境 / 未知值拒绝。"""

    maxDiff = None

    def test_c8_story_cover_plain_qa(self):
        with tempfile.TemporaryDirectory(prefix="k5_c8a_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 2)
                sid = "sess-k5-cover"
                with _captured_logs() as recs:
                    up, dev = _device_to_engine(front, sid, [_ctx_msg("story_cover")])
                self.assertNotIn(
                    "kidpal_session_ctx", _forwarded_frame_types(up.sent),
                    "C8 RED: source_page=story_cover 的 ctx 被透传引擎 —— 契约 §四须拦截。")
                self.assertEqual(
                    front._get_story_acc().routing(sid), "chat",
                    "C8 RED: story_cover 误切记录语境（routing != chat）—— 封皮发起普通问答, "
                    "不得 acc.start（S-F2.6-AC4 / 契约 §四「普通问答语境」）。")
                self.assertEqual(
                    len(_story_list_frames(dev.sent_text)), 0,
                    "C8 RED: story_cover 触发了 story_list 推送 —— 封皮普通问答不得推列表。")
                self.assertEqual(
                    len(up.sent), 0,
                    "C8 RED: story_cover 向引擎腿注入了记录语境信号 —— 封皮不得注入 "
                    "kidpal_record_context（普通问答无记录语境）。")
                self.assertFalse(
                    any("unknown_source_page" in r.getMessage() for r in recs),
                    "C8 RED: story_cover 被当成未知 source_page 记 warning —— 闭集应含 "
                    "story_cover（契约 §四六值闭集）。")

    def test_c8_story_list_still_records(self):
        with tempfile.TemporaryDirectory(prefix="k5_c8b_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 2)
                sid = "sess-k5-story"
                _up, dev = _device_to_engine(front, sid, [_ctx_msg("story")])
                self.assertEqual(
                    front._get_story_acc().routing(sid), "accumulate",
                    "C8 RED: source_page=story 未切记录语境（routing != accumulate）—— 列表"
                    "记录语境回归被破（S-F2.2-AC1）。")
                pushes = _story_list_frames(dev.sent_text)
                self.assertEqual(
                    len(pushes), 1,
                    f"C8 RED: story 列表 ctx 产生 {len(pushes)} 帧 story_list（应恰一帧）。")
                self.assertTrue(
                    pushes[0]["data"].get("restore") is True,
                    "C8 RED: story 列表 ctx 恢复推送未带 restore:true。")

    def test_c8_unknown_source_page_warns(self):
        with tempfile.TemporaryDirectory(prefix="k5_c8c_") as tmp:
            with _front_with_stories(tmp) as front:
                _seed(Path(tmp), 1)
                sid = "sess-k5-unknown"
                with _captured_logs() as recs:
                    up, dev = _device_to_engine(front, sid, [_ctx_msg("brand_new_page")])
                self.assertNotIn(
                    "kidpal_session_ctx", _forwarded_frame_types(up.sent),
                    "C8 RED: 未知 source_page 的 ctx 被透传引擎。")
                self.assertEqual(
                    front._get_story_acc().routing(sid), "chat",
                    "C8 RED: 未知 source_page 回退成记录语境（routing != chat）—— 契约 §四"
                    "「未知值拒绝并记日志, 不得回退成 story」。")
                self.assertEqual(
                    len(_story_list_frames(dev.sent_text)), 0,
                    "C8 RED: 未知 source_page 触发 story_list 推送。")
                self.assertTrue(
                    any("unknown_source_page" in r.getMessage()
                        and "brand_new_page" in r.getMessage() for r in recs),
                    "C8 RED: 未知 source_page 未记 unknown_source_page warning（契约 §四："
                    "未知值拒绝并记日志）。")


if __name__ == "__main__":  # pragma: no cover
    unittest.main()
