"""Frozen acceptance tests — TaskPackage 261016-p3-story-body-sd-cache（server 面）。

承载 acceptanceRefs：SC-A7（front kidpal_story_fetch 消费——合法 ids 逐篇以既有
story_text 帧应答、形状逐字复用零新字段；非法 / 不存在 id 拒绝 + 审计 + 不回帧；空
ids = no-op；多 id 逐篇顺序）与 SC-A8 的 server 半（fetch 应答不触发星星盖章 / 不产生
任何归档侧效应；卡不在位不留存的板端消费半归 firmware host 齿 + 真机轮）。

契约形状权威 = docs/contracts/interface-contract.md §四 `kidpal_story_fetch` 条（设备
上行 {"session_id","type":"kidpal_story_fetch","ids":[...]}；网关消费并拦截、不下透
引擎；逐 id 校验存在于 story 账本〔哨兵头 + <stories_dir>/<id>.md 存在〕→ 读 MD →
以既有 story_text 帧逐篇应答、每篇一次；非法 / 不存在 id 拒绝 + 审计留痕、不回帧；
空 ids = no-op；下行零新形状）+ §1.3④ story_text 消费语义扩展（fetch 应答不触发盖章
演出、卡不在位当场显示不留存）。

上游冻结 = delivery/review/261017-arch-review-p3-story-body-sd-cache/（决策 24 窄例外 +
ADR-011）；reviewed plan = delivery/taskpackages/261016-p3-story-body-sd-cache/plan.md
（块 2 = 复用 gw_story.story_text() + _push_display_payload，零新字段）。

Nature：行为级——真实 front 泵（pump_client_to_upstream）+ 真实 story.py 落盘/读回 +
真实 display_schema 机械门；I/O double 只替代 WebSocket 两端（M4K1 / K3 冻结先例形态）。

=============================================================================
契约条目 ↔ 齿 的强度申报（acceptance-evidence「契约条目 ↔ 齿」节）
=============================================================================

| 契约 / 决策条目 | 承载齿 | 性质档 |
|---|---|---|
| §四 fetch 合法 ids → 逐篇 story_text 应答（每篇一次） | test_sc_a7_legal_ids_answer_one_story_text_frame_each | 行为级（pump 真消费 + 真读盘 + 真门） |
| §四 应答复用既有帧、零新字段 | 同上（逐帧 data 与 gw_story.story_text(...) 全等 + keys ⊆ {id,title,text,seq}） | 行为级 |
| §四 非法 id（路径穿越 / 分隔符 / 无哨兵头 / 不存在）拒绝 + 审计 + 不回帧 | test_sc_a7_rejects_invalid_and_missing_ids_with_audit | 行为级（真磁盘 + 真日志） |
| §四 空 ids = no-op | test_sc_a7_empty_ids_noop | 行为级 |
| §四 多 id 逐篇顺序 | test_sc_a7_multi_id_frame_order_matches_request | 行为级 |
| §1.3④ fetch 应答不触发盖章 / 无归档侧效应 | test_sc_a8_fetch_answer_has_no_stamp_or_archive_side_effect | 行为级 |
| §1.3④ 帧形状零改动（无 seq 记录不凭空造 seq） | test_sc_a8_seqless_record_keeps_three_key_shape | 行为级 |
| §四 网关消费并拦截、不下透引擎 | 全部齿的「zero forward」断言 | 行为级 |

弱于契约处（显式缺口，不得当已兑现）：
  GAP-1 「卡不在位 / 坏卡时当场显示、不留存」的**板端消费半**在 firmware（host 齿 +
        真机轮），本文件不可达；本文件只证「应答帧本身携带正文、无新的盖章触发字段」。
  GAP-2 「审计日志正文格式」不钉——只断言拒绝时存在含 offending id（或拒绝类 token）
        的日志记录（M4K1-A4 先例口径）。
  GAP-3 「response 的 session 绑定（session_id 回传）」不单独钉——front 自推帧形态沿用
        _push_display_payload 既有帧（契约未要求响应内回传 session_id）。
  GAP-4 「非法 id 与合法 id 混在同一 ids 数组」的语义按契约「逐 id 校验 + 逐篇应答 +
        非法不回帧」钉为**逐条跳过**（合法项照常应答）。若实现改判为整帧拒绝，本齿红
        ——须按契约重裁后由 planner 退回顾问重钉，不得由 builder 放宽。

行为等价假设（显式）：
  EQ-1 「pump_client_to_upstream 真路径 = 现场帧消费路径」：设备上行帧经该泵进入
        _consume_kidpal_control（M4K1-A4 delete 齿同款假设，已复证）。
  EQ-2 「_push_display_payload 的 kidpal/display mcp 信封 = 设备实际收帧形状」：与
        M4K1-A3 冻结齿同一重建器口径。

Coverage map：
  SC-A7 -> test_sc_a7_legal_ids_answer_one_story_text_frame_each
        -> test_sc_a7_rejects_invalid_and_missing_ids_with_audit
        -> test_sc_a7_empty_ids_noop
        -> test_sc_a7_multi_id_frame_order_matches_request
        -> test_sc_a7_malformed_payloads_never_crash_or_forward
  SC-A8 -> test_sc_a8_fetch_answer_has_no_stamp_or_archive_side_effect
        -> test_sc_a8_seqless_record_keeps_three_key_shape

Run protocol（offline，-m "not live" 可跑）：
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_story_fetch.py

REVISION LOG
  2026-10-17 初版冻结（acceptance-author；SC-A7 + SC-A8 server 半）。
"""

from __future__ import annotations

import asyncio
import base64
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

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
CONTRACT = REPO_ROOT / "docs" / "contracts" / "interface-contract.md"

# story_text 帧允许的 data 键（契约 §1.3④：{id,title,text} + 可选 seq）。任何其它键
# = 「零新字段」破。
_STORY_TEXT_KEYS = {"id", "title", "text", "seq"}


# --------------------------------------------------------------------------- #
# Module loading (missing/unimportable = RED for missing behaviour)           #
# --------------------------------------------------------------------------- #

_story_module_cache = None
_front_module_cache = None


def _story_module():
    """server/gateway/story.py 纯模块（读取真实落盘记录 / seq 账本）。"""
    global _story_module_cache
    if _story_module_cache is not None:
        return _story_module_cache
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.story as st  # noqa: PLC0415 — offline import, cached
    _story_module_cache = st
    return st


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（M4K1 / RMD 冻结件同款先例）。"""
    try:
        import fastapi  # noqa: F401

        if isinstance(getattr(fastapi, "FastAPI", None), type):
            return  # real fastapi（或带可用 FastAPI 类的 stub）在场 —— use it
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
        spec = importlib.util.spec_from_file_location("kidpal_front_app_story_fetch", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _front_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须离线可装载"
                      "（import 期零网络 I/O；fastapi 缺席走本地 stub）。")


def _front():
    mod, err = _load_front_app()
    if mod is None:
        raise AssertionError("SC-A7 RED: " + err)
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


# --------------------------------------------------------------------------- #
# I/O doubles（M4K1 冻结 seam 同款形态：只替代 WebSocket 两端）                  #
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
    """Run the device->engine uplink pump; returns (upstream, device_socket)."""
    dev = _FakeDeviceSocket(
        incoming=[{"type": "websocket.receive", "text": t} for t in incoming_texts])
    up = _FakeUpstream()
    asyncio.run(front.pump_client_to_upstream(dev, up, session_id))
    return up, dev


def _forwarded_frame_types(sent):
    """Frame types the front forwarded up to the engine (text frames parsed)."""
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


def _display_payloads(sent_text):
    """Reconstruct [{"view","data"}] from kidpal/display envelopes on the device leg."""
    out = []
    for text in sent_text:
        try:
            frame = json.loads(text)
        except (TypeError, ValueError):
            continue
        if not (isinstance(frame, dict) and frame.get("type") == "mcp"
                and isinstance(frame.get("payload"), dict)):
            continue
        payload = frame["payload"]
        params = payload.get("params") or {}
        if payload.get("method") != "tools/call" or params.get("name") != "kidpal/display":
            continue
        args = params.get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _story_text_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_text"]


def _story_list_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_list"]


@contextlib.contextmanager
def _captured_logs():
    """Capture every log record (audit-log assertions)."""
    records = []

    class _H(logging.Handler):
        def emit(self, record):
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


def _fetch_audit_hit(records, needle: str) -> bool:
    """拒绝路径必须留审计（GAP-2：只判存在含 offending id 或拒绝类 token 的记录）。"""
    reject_tokens = ("reject", "拒绝", "invalid", "unsafe", "no_sentinel", "missing",
                     "outside_dir", "not_found", "story_fetch")
    for rec in records:
        try:
            msg = rec.getMessage()
        except Exception:  # noqa: BLE001
            continue
        if needle and needle in msg:
            return True
        if any(tok in msg for tok in reject_tokens):
            return True
    return False


def _read_record(st, stories_dir, story_id):
    rec = st.read_story_file(str(Path(stories_dir) / f"{story_id}.md"))
    assert rec is not None, f"precondition: seeded record {story_id} unreadable"
    return rec


def _expected_story_text(st, stories_dir, story_id, seq):
    """零新字段基准 = 既有 producer 对「同一读回记录」的输出。

    seq 取归档时 archive_story 返回的入册流水号（public 返回值 = 账本权威），
    不在测试里反推账本内部结构。"""
    rec = _read_record(st, stories_dir, story_id)
    if seq is not None:
        rec = dict(rec)
        rec["seq"] = seq
    return st.story_text(rec)


def _fetch_frame(ids, session_id="sess-fetch", **overrides):
    obj = {"session_id": session_id, "type": "kidpal_story_fetch", "ids": ids}
    obj.update(overrides)
    return json.dumps(obj, ensure_ascii=False)


class _Seeded:
    """3 篇真落盘故事（真 archive_story：哨兵头 + seq 账本 + MD 正文）。"""

    def __init__(self, st, stories_dir):
        self.a = st.archive_story(str(stories_dir), "故事A正文", title="故事A",
                                  slug="alpha", date="2026-09-20")
        self.b = st.archive_story(str(stories_dir), "故事B正文", title="故事B",
                                  slug="beta", date="2026-09-21")
        self.c = st.archive_story(str(stories_dir), "故事C正文", title="故事C",
                                  slug="gamma", date="2026-09-22")


# ========================================================================== #
# SC-A7 — front kidpal_story_fetch 消费                                     #
# ========================================================================== #


class StoryFetchWireAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SC-A7 -- 合法 ids 逐篇以既有 story_text 帧应答（每篇一次、形状逐字复用、
    #                   零新字段、不下透引擎）
    def test_sc_a7_legal_ids_answer_one_story_text_frame_each(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="sc_a7_legal_") as tmp:
            with _front_with_stories(tmp) as front:
                seeded = _Seeded(st, tmp)
                seq_by_id = {seeded.a["id"]: seeded.a.get("seq"),
                             seeded.b["id"]: seeded.b.get("seq"),
                             seeded.c["id"]: seeded.c.get("seq")}
                ids = [seeded.a["id"], seeded.b["id"], seeded.c["id"]]
                up, dev = _device_to_engine(
                    front, "sess-a7-legal", [_fetch_frame(ids)])

                fwd = _forwarded_frame_types(up.sent)
                self.assertNotIn(
                    "kidpal_story_fetch", fwd,
                    "SC-A7 RED: kidpal_story_fetch 被透传给引擎 —— 契约 §四「网关消费并"
                    "拦截、不下透引擎」未实现。")
                frames = _story_text_frames(dev.sent_text)
                self.assertEqual(
                    len(frames), 3,
                    f"SC-A7 RED: 3 个合法 id 仅回 {len(frames)} 帧 story_text（契约：逐篇"
                    "应答、每篇一次）。")
                self.assertEqual(
                    [f["data"]["id"] for f in frames], ids,
                    "SC-A7 RED: 应答帧的正文 id 序列与请求 ids 不一致（可能串篇）。")

                for story_id in ids:
                    expected = _expected_story_text(st, tmp, story_id, seq_by_id[story_id])
                    matching = [f for f in frames if f["data"]["id"] == story_id]
                    self.assertEqual(len(matching), 1,
                                     f"SC-A7 RED: id={story_id} 应答帧数 != 1。")
                    self.assertEqual(
                        matching[0], expected,
                        f"SC-A7 RED: id={story_id} 应答帧形状 != 既有 story_text 帧"
                        "（契约：零新形状 / 零新字段 —— 逐字复用 gw_story.story_text）。")
                    self.assertLessEqual(
                        set(matching[0]["data"].keys()), _STORY_TEXT_KEYS,
                        f"SC-A7 RED: id={story_id} 应答帧出现 story_text 之外的新字段："
                        f"{sorted(set(matching[0]['data'].keys()) - _STORY_TEXT_KEYS)}。")
                    self.assertIn(
                        "seq", matching[0]["data"],
                        f"SC-A7 RED: id={story_id} 账本已有 seq 但应答帧未携带 data.seq"
                        "（契约 §1.3④ 帧形状含 seq；taskpackage impactedTestsSeed 明载"
                        "「ledger 有 seq 时带 seq」）。")

                self.assertEqual(dev.sent_bytes, [],
                                 "SC-A7 RED: fetch 应答出现二进制帧（应为纯文本 mcp 帧）。")
                self.assertEqual(_story_list_frames(dev.sent_text), [],
                                 "SC-A7 RED: fetch 应答附带 story_list 推送（应答应逐篇"
                                 " story_text，列表推送三时点不含 fetch）。")

    # coverage: SC-A7 -- 非法 / 不存在 id 拒绝 + 审计 + 不回帧；混合请求逐条跳过；
    #                   绝不读 stories_dir 以外的文件
    def test_sc_a7_rejects_invalid_and_missing_ids_with_audit(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="sc_a7_reject_") as tmp:
            root = Path(tmp)
            stories = root / "stories"
            stories.mkdir()
            outside = root / "outside.md"
            outside.write_text("不应被当作故事读回\n", encoding="utf-8")
            (stories / "notes.md").write_text("普通杂物，无哨兵头\n", encoding="utf-8")
            with _front_with_stories(stories) as front:
                seeded = _Seeded(st, stories)

                bad_cases = [
                    "../outside",        # 路径穿越
                    "a/b",               # 正斜杠
                    "a\\b",              # 反斜杠
                    "notes",             # 无哨兵头杂物文件
                    "2099-01-01-ghost",  # 不存在
                ]
                for bad in bad_cases:
                    with _captured_logs() as records:
                        up, dev = _device_to_engine(
                            front, "sess-a7-bad", [_fetch_frame([bad])])
                    self.assertNotIn(
                        "kidpal_story_fetch", _forwarded_frame_types(up.sent),
                        f"SC-A7 RED: 非法 id={bad!r} 的 fetch 帧被透传给引擎。")
                    self.assertEqual(
                        _story_text_frames(dev.sent_text), [],
                        f"SC-A7 RED: 非法 id={bad!r} 回了 story_text 帧（契约：拒绝不回帧）。")
                    self.assertTrue(
                        _fetch_audit_hit(records, bad),
                        f"SC-A7 RED: 非法 id={bad!r} 拒绝未留审计日志（契约 §四：拒绝 + "
                        "审计留痕）。")
                self.assertTrue(outside.exists(),
                                "SC-A7 RED: 路径穿越 id 读到了 stories 目录外的文件 —— "
                                "story 账本边界被破。")
                self.assertTrue((stories / "notes.md").exists(),
                                "SC-A7 RED: 无哨兵头杂物文件被读成故事。")

                # 混合请求：合法项照常应答、非法项逐条跳过（GAP-4 钉死语义）。
                with _captured_logs() as records:
                    up, dev = _device_to_engine(
                        front, "sess-a7-mixed",
                        [_fetch_frame([seeded.a["id"], "../outside", seeded.b["id"]])])
                frames = _story_text_frames(dev.sent_text)
                self.assertEqual(
                    [f["data"]["id"] for f in frames],
                    [seeded.a["id"], seeded.b["id"]],
                    "SC-A7 RED: 混合请求未「逐 id 校验 + 逐篇应答」——合法项应照常回帧、"
                    "非法项跳过。")
                self.assertTrue(_fetch_audit_hit(records, "../outside"),
                                "SC-A7 RED: 混合请求中的非法 id 未留审计。")
                self.assertNotIn("kidpal_story_fetch", _forwarded_frame_types(up.sent),
                                 "SC-A7 RED: 混合请求帧被透传给引擎。")

    # coverage: SC-A7 -- 空 ids = no-op（零帧、零下透）
    def test_sc_a7_empty_ids_noop(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="sc_a7_empty_") as tmp:
            with _front_with_stories(tmp) as front:
                _Seeded(st, tmp)
                up, dev = _device_to_engine(
                    front, "sess-a7-empty", [_fetch_frame([])])
                self.assertNotIn(
                    "kidpal_story_fetch", _forwarded_frame_types(up.sent),
                    "SC-A7 RED: 空 ids 的 fetch 帧被透传给引擎 —— 契约：网关消费并拦截。")
                self.assertEqual(_story_text_frames(dev.sent_text), [],
                                 "SC-A7 RED: 空 ids 回了 story_text 帧（契约：空 ids = no-op）。")
                self.assertEqual(_story_list_frames(dev.sent_text), [],
                                 "SC-A7 RED: 空 ids 触发了 story_list 推送。")

    # coverage: SC-A7 -- 多 id 逐篇顺序（帧序 == ids 序）
    def test_sc_a7_multi_id_frame_order_matches_request(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="sc_a7_order_") as tmp:
            with _front_with_stories(tmp) as front:
                seeded = _Seeded(st, tmp)
                ids = [seeded.c["id"], seeded.a["id"], seeded.b["id"]]
                _up, dev = _device_to_engine(
                    front, "sess-a7-order", [_fetch_frame(ids)])
                frames = _story_text_frames(dev.sent_text)
                self.assertEqual(
                    [f["data"]["id"] for f in frames], ids,
                    "SC-A7 RED: 多 id 应答未按 ids 序逐篇下发（契约：逐篇顺序）。")

    # coverage: SC-A7 -- 畸形载荷（缺 ids / ids 非数组 / 元素非字符串 / null）
    #                   不崩、不泄给引擎、不回帧
    def test_sc_a7_malformed_payloads_never_crash_or_forward(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="sc_a7_malformed_") as tmp:
            with _front_with_stories(tmp) as front:
                _Seeded(st, tmp)
                cases = [
                    '{"session_id":"s","type":"kidpal_story_fetch"}',
                    '{"session_id":"s","type":"kidpal_story_fetch","ids":"abc"}',
                    '{"session_id":"s","type":"kidpal_story_fetch","ids":null}',
                    '{"session_id":"s","type":"kidpal_story_fetch","ids":[123]}',
                    '{"session_id":"s","type":"kidpal_story_fetch","ids":[null]}',
                ]
                for frame in cases:
                    up, dev = _device_to_engine(front, "sess-a7-malformed", [frame])
                    self.assertNotIn(
                        "kidpal_story_fetch", _forwarded_frame_types(up.sent),
                        f"SC-A7 RED: 畸形 fetch 帧被透传给引擎：{frame}")
                    self.assertEqual(
                        _story_text_frames(dev.sent_text), [],
                        f"SC-A7 RED: 畸形 fetch 帧回了正文：{frame}")


# ========================================================================== #
# SC-A8 — 契约消费语义（fetch 应答不盖章 / 零归档侧效应）                        #
# ========================================================================== #


class StoryFetchConsumptionAcceptance(unittest.TestCase):
    maxDiff = None

    # coverage: SC-A8 -- fetch 应答不触发星星盖章：server 侧零归档侧效应（不重归档、
    #                   不推进 seq 账本、不推 story_list）
    def test_sc_a8_fetch_answer_has_no_stamp_or_archive_side_effect(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="sc_a8_sideeffect_") as tmp:
            with _front_with_stories(tmp) as front:
                seeded = _Seeded(st, tmp)
                stories = Path(tmp)
                # 快照 stories 目录全部文件（故事 MD + 入册流水号账本），fetch 必须零写入。
                before_files = {p.name: p.read_bytes() for p in stories.iterdir() if p.is_file()}

                _up, dev = _device_to_engine(
                    front, "sess-a8-side",
                    [_fetch_frame([seeded.a["id"], seeded.b["id"], seeded.c["id"]])])

                self.assertEqual(len(_story_text_frames(dev.sent_text)), 3,
                                 "SC-A8 RED: 合法三篇未各回一帧 —— fetch 应答链未实现 / "
                                 "未消费（本齿的侧效应断言以应答成立为前提，故先红在此）。")
                after_files = {p.name: p.read_bytes() for p in stories.iterdir() if p.is_file()}
                self.assertEqual(
                    before_files, after_files,
                    "SC-A8 RED: fetch 应答改写了 stories 目录内容（故事 MD 或 seq 账本）——"
                    "fetch 是只读镜像语义、云端归档为唯一真相源，且不得分配 / 推进 seq。")
                self.assertEqual(
                    _story_list_frames(dev.sent_text), [],
                    "SC-A8 RED: fetch 应答附带了 story_list 推送（三时点不含 fetch）。")
                total_before = st.list_stories(str(stories))["data"]["total"]
                self.assertEqual(total_before, 3,
                                 "SC-A8 RED: fetch 后现存篇数漂移（不得产生新故事）。")

    # coverage: SC-A8 -- 帧形状零改动：无 seq 记录不得凭空造 seq / 不得新增键
    def test_sc_a8_seqless_record_keeps_three_key_shape(self):
        st = _story_module()
        with tempfile.TemporaryDirectory(prefix="sc_a8_seqless_") as tmp:
            stories = Path(tmp)
            story_id = "2026-09-01-manual"
            title_b64 = base64.b64encode("手工正文".encode("utf-8")).decode("ascii")
            (stories / f"{story_id}.md").write_text(
                "<!-- kidpal-story:1 -->\n"
                f"<!-- kidpal-id:{story_id} -->\n"
                "<!-- kidpal-date:2026-09-01 -->\n"
                f"<!-- kidpal-title:{title_b64} -->\n"
                "\n# 手工正文\n\n"
                "<!-- kidpal-body-start -->\n"
                "手工正文内容\n",
                encoding="utf-8",
            )
            with _front_with_stories(stories) as front:
                _up, dev = _device_to_engine(
                    front, "sess-a8-seqless", [_fetch_frame([story_id])])
                frames = _story_text_frames(dev.sent_text)
                self.assertEqual(len(frames), 1,
                                 "SC-A8 RED: 无 seq 记录的 fetch 未回恰一帧。")
                data = frames[0]["data"]
                self.assertEqual(
                    set(data.keys()), {"id", "title", "text"},
                    "SC-A8 RED: 无 seq 记录的应答 data 键 != {id,title,text}——零新字段破"
                    "（不得凭空造 seq、不得新增键）。")
                self.assertEqual(data["id"], story_id,
                                 "SC-A8 RED: 应答正文 id 与请求 id 不一致。")
                self.assertEqual(data["text"], "手工正文内容",
                                 "SC-A8 RED: 应答正文内容与磁盘记录不一致（正文须与所选"
                                 "篇目一致）。")


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
