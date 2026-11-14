"""Frozen acceptance tests — TaskPackage 261104-k2-points-device-ui（server 面）。

承载 story = S-F6.2（删除误记，AC4 删除重算与负余额设备上行消费半 / AC5 断网数据
面）+ S-F6.3（语音记账与兑换，AC1 不完整句零写入 server 消费面半 + 补充规则积分
语境）+ 契约 docs/contracts/interface-contract.md §四 kidpal_points_delete（单向
幂等删除、权威重推隐式 ack 对账）/ kidpal_session_ctx（points 两值 + selected_id
只读、未知值拒绝记日志不得回退 story）+ §1.3⑦⑧ restore 通道语义（只读消费）+
PROPOSAL delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md D-03 /
D-04 / D-07 / D-08 + K2D-E2（server 半）。

Scope of record（taskpackage exitCriteria K2D-E2 → 验收 case K2D-A1..A4）:
  K2D-A1 kidpal_points_delete 私有上行消费与幂等对账：整条拦截不下透引擎；存在
        id 删除重算 -> 恰一帧 home restore + 恰一帧 ledger 首页 restore+reset
        （隐式 ack）；重复/已不存在 id 幂等 no-op 仍重推；非法 id 拒绝审计零重推；
        负余额帧不被钳位；story 账本零波及。
  K2D-A2 积分语境有界摘要注入：ctx(points_home/points_ledger) 经引擎腿注入
        kidpal_points_context 有界摘要（余额/激活目标/最近 ≤3 条；selected_id
        命中现存分录附分录摘要、未命中不伪造）；注入失败不阻断拦截消费；整条
        拦截。
  K2D-A3 source_page 闭集守卫与 story 记录语境零回归：积分两值不误切累积态、
        不推 story_list、不注 record 信号；未知值拒绝记日志且绝不回退成 story；
        ctx(story) 切累积态 + story_list 推送零回归。
  K2D-A4 不完整句零写入（server 消费面半）：普通 stt 帧不拦截不落账；非 control
        载荷零写入；意图拒绝零写入（回归半）；front 的 points 写入唯一入口 =
        _consume_points_intent / 删除缝（单写入口结构锚）。

本文件不重复钉 K0/K1 已冻结语义（must-stay-green 域）：K0PTS-A1/A2 薄意图截获
与兑换资格门、K0PTS-A3 白名单与来源门禁与 prompt 锚、K0PTS-A4 restore 通道极性、
K0PTS-A5 分页 producer、K1L-A1..A10 执行器与观察缝——各自保持全绿；本文件只钉
K2 新增的 front 消费面（删除上行消费 + 语境注入 + 闭集守卫 + 零写入消费面）。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — front_app.py 补 K2 消费面
（K0/K1 既有语义零放宽、K0/K1 冻结件 must-stay-green）：

  server/gateway/front/front_app.py（在既有 _consume_kidpal_control 上 additive）:

    1) kidpal_points_delete 分支（新；契约 §四 2026-11-04 LANDED 的 front 消费半）:
       {"session_id":"...","type":"kidpal_points_delete","id":"<points-entry-id>"}
       - 整条消费并拦截、不下透引擎，返回 True（无论取值）；
       - id 为非空 str -> 交 gw_points.delete_entry(POINTS_STATE_PATH, id) 执行；
         deleted=True（删除生效，revision+1、余额重算可负）与 deleted=False
         （重复/已不存在 id，幂等 no-op 不写账）均**重推对账帧**（隐式 ack）：
         恰一帧 points_home（data.restore=True）+ 恰一帧 points_ledger 首页
         （data.restore=True、page.reset=True），经 _push_display_payload 下发；
       - 非法 id（非 str / 空串 / 纯空白 -> delete_entry ValueError）-> 拒绝：
         审计日志（WARNING 级、含 "points_delete" 记号）、**零重推**、返回 True
         （已消费，绝不透传）；
       - 执行器异常不外抛炸连接（消费语义优先，失败可见于日志）。
       （wire 事实：对端 front 只信 delete_entry 回执与 points.py 当前状态——
       设备 id 是数字/字符串、字段是否字符串化均以本文件 K2D-A1 断言的
       json.loads 载荷为准；设备侧 id 形状由 host 件 K2PTS-P6/P7 锚定。）

    2) kidpal_session_ctx 积分语境注入（新；契约 §四 points 两值半）:
       - 模块级常量 POINTS_CTX_SIGNAL_TYPE = "kidpal_points_context"（引擎腿
         注入帧 type；沿 RECORD_CTX_SIGNAL_TYPE 同族通道 _send_engine_leg_frames）；
       - source_page ∈ {"points_home","points_ledger"} -> 从 points.py 当前状态
         机械产生**有界摘要**并经引擎腿恰注一帧 {"type": POINTS_CTX_SIGNAL_TYPE,
         "summary": <str>}：摘要含当前余额、激活目标（名称与分值）、最近至多
         HOME_RECENT_MAX(=3) 条分录（points_home 语境）；points_ledger 语境另附
         selected_id 命中分录的摘要（六键值取自 points.py 权威状态）；
       - selected_id 未命中/畸形 -> 常规摘要照注、**不伪造分录**（不含该 id 的
         编造字段）；
       - 注入失败（引擎腿 send 异常等）不阻断 ctx 拦截消费（ADR-013 注入先例
         同款）；返回 True；
       - 摘要为有界注入：不含全量账本（>3 条分录的早期分录不得进入摘要）——
         模型不直接读状态文件（契约 §四）。

    3) source_page 闭集守卫（新；契约 §四「未知值拒绝并记日志，不得回退成
       story」）: 未知取值 -> 记 WARNING 日志（含 source_page 值）、整条消费、
       绝不执行 story 的累积态切换/推送；积分两值**不**触发 acc.start /
       story_list 推送 / kidpal_record_context 注入（普通问答语境）。

  display_schema.py / points.py / K0/K1 冻结面：零改动（本包消费既有形状；
  负余额帧不被 floor 钳位已由 K1L-A3 冻结，本文件 K2D-A1 作回归复核半）。

Shape authority = docs/contracts/interface-contract.md §1.3⑦⑧ / §四
kidpal_session_ctx·kidpal_points_delete（只读）+ docs/requirements.md v1.71
S-F6.2 AC4/AC5 + S-F6.3 AC1/补充规则 + NFR-OFF-1 + PROPOSAL D-03/D-04/D-07/D-08。

契约条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 验收断言」节）:

| 契约 / 决策条目 | 承载断言 | 性质档 |
|---|---|---|
| 契约 §四 kidpal_points_delete 消费半（幂等对账重推） | test_k2d_a1（删除/幂等 no-op/非法 id 三路真跑 + 帧形状逐字段） | 行为级 |
| D-03 / OQ-1 删除重算负余额（重推帧半） | test_k2d_a1（−40 帧不被钳位） | 行为级 |
| D-08 / 契约 §四 session_ctx points 两值注入 | test_k2d_a2（注入帧 + 有界性 + selected_id 命中/未命中） | 行为级 |
| 契约 §四「未知值拒绝记日志、不得回退 story」 | test_k2d_a3（WARNING 日志 + 零累积态 + 绝不按 story 处理） | 行为级 |
| S-F6.3 补充规则 / 积分两值不误切记录语境 | test_k2d_a3（routing 停留 chat + 无 story_list / record 信号）+ M4K1-A2 回归 | 行为级 |
| S-F6.3 AC1 不完整句零写入（消费面半） | test_k2d_a4（stt 零拦截零写入 + 非控制载荷零写入 + 单写入口锚） | 行为级 + 源锚代理（模型半归 prompt，K0PTS-A3 已锚） |
| D-04 设备语音不开放删除（front 半） | test_k2d_a4（_consume_kidpal_control 源内无 add/redeem 调用） | 源锚代理 |
| 既有 story 归档 / K0K1 冻结面 | 各 test 内 must-stay-green 半 | 行为级 |

弱于契约处（显式缺口，不得当已兑现）:
  GAP-K2-1 删除重推对账的端到端活性（设备确认 -> 上行 -> front 消费 -> 权威重推
        -> 设备原地刷新）跨端离线不可闭环：front 半由 K2D-A1 承载、设备半由 host
        件 K2PTS-P3/P7 承载、全链归 K2PTS-L2 真机轮（K1L-L1/L2 挂接同轮）。
  GAP-K2-2 语境摘要的具体文案/措辞为实现自由（注入对象是模型、无儿童可见 UI）；
        本文件钉「含哪些权威事实 + 有界」不钉逐字。
  GAP-K2-3 积分语境注入的触发时机粒度（每次会话一次 vs 每轮刷新 vs 重复 ctx 幂等
        去重）为实现自由（契约只钉「重复发送安全」）；本文件按「一条 ctx 帧至多
        触发一次注入通道」的缝行为钉，重复语义归实现/真机调优。
  GAP-K2-4 NFR-PERF-4 软时延只记录不写阈值（K1L-GAP-6 同口径）。

行为等价假设（显式；不写即默认不成立）:
  EQ-1 直调 _consume_kidpal_control = 上行泵真实消费路径（M4K1 冻结件同款驱动
        形态；泵侧对返回 True 的帧不转发引擎）。
  EQ-2 FakeDeviceSocket / FakeEngineLeg doubles = 设备腿 / 引擎腿真实 send 语义
        （K1L EQ-2 / M4K1 I/O doubles 同口径；wire 形状 = kidpal/display mcp
        信封 + view/data 双 String）。
  EQ-3 tmp 目录 state/stories = /opt/kidpal/points 与 /opt/kidpal/stories 同一
        文件语义（K1L EQ-3 同款）。
  EQ-4 「注入帧经引擎腿可达模型」= kidpal_record_context 既有注入通道同构
        （_send_engine_leg_frames 单源）；通道级失败形态由该单源承载。

Coverage map (acceptanceRef -> test):
  K2D-A1 -> test_k2d_a1_points_delete_private_uplink_idempotent_reconciliation
  K2D-A2 -> test_k2d_a2_points_context_bounded_injection_with_selected_id
  K2D-A3 -> test_k2d_a3_source_page_closure_guard_and_story_zero_regression
  K2D-A4 -> test_k2d_a4_incomplete_sentence_zero_write_and_single_write_entry
  coverage: K2PTS-L1 —— live-manual 真实语音记账/兑换全链（S-F6.3 真机半；载体
            device-round-evidence.md），不落自动断言、不计入本文件通过面；
  coverage: K2PTS-L2 —— live-manual 删除/恢复真机半 + K1L-L1/L2 挂接（QQ 落账
            不抢前台 GAP-1 活性 + 真实消费端分页对账；同载体），不落自动断言。

exitCriteria 映射：K2D-E2 -> A1/A2/A3/A4（server 半；host 半由
firmware/main/test/points_device_ui.acceptance.c 承载）。

Run protocol（offline；-m "not live" 可跑；文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib server/acceptance/test_points_device_ui.acceptance.py

REVISION LOG
  2026-11-05 初版冻结（acceptance-author；K2D-A1..A4 server 离线层；K2PTS-L1/L2
             live-manual 层在 host 件 coverage 标记单列不落本文件。设计态红 =
             kidpal_points_delete 消费分支 / POINTS_CTX_SIGNAL_TYPE 注入缝 /
             source_page 未知值拒绝记日志三处缺席；A3 的 story 半与 A4 的意图
             拒绝半为 must-stay-green 回归，属回归守卫非空过——其绿现状即 K0/K1
             已冻结语义）。
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import inspect
import json
import sys
import tempfile
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
POINTS_PY = GATEWAY_DIR / "points.py"

# 冻结协调字面（front 与 CLI 的 state 路径单源；契约 §四 + 门 1 裁定）。
POINTS_STATE_ENV = "KIDPAL_POINTS_STATE"

# 积分语境注入帧 type（本文件 FROZEN 契约第 2 条冻结值；RECORD_CTX_SIGNAL_TYPE
# 同族通道、同构形态）。
POINTS_CTX_SIGNAL_TYPE_FROZEN = "kidpal_points_context"

# 有界性锚：points_home 语境摘要最多携带的分录条数（契约 §1.3⑦ recent ≤3；
# points.py HOME_RECENT_MAX 同值）。
HOME_RECENT_MAX_FROZEN = 3

_TZ_CN = dt_timezone(timedelta(hours=8))
_T0 = datetime(2026, 11, 5, 10, 0, 0, tzinfo=_TZ_CN)


def _t(offset_seconds: int) -> datetime:
    """T0 + offset 秒（注入时钟；全部时间断言经本 helper，禁 wall-clock）。"""
    return _T0 + timedelta(seconds=offset_seconds)


# --------------------------------------------------------------------------- #
# Module loading (missing/unimportable = RED for missing behaviour)           #
# --------------------------------------------------------------------------- #

_points_module_cache = None
_front_module_cache = None


def _points_module():
    """server/gateway/points.py 纯模块（K1 已冻结在世；缺席 = 回归红）。"""
    global _points_module_cache
    if _points_module_cache is not None:
        return _points_module_cache
    if not POINTS_PY.is_file():
        raise AssertionError(
            "K2D RED: server/gateway/points.py 不在世 —— K1 权威账本回归面丢失。")
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.points as pts  # noqa: PLC0415 — offline import, cached
    _points_module_cache = pts
    return pts


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（K0/K1/M4K1 冻结件同款先例）。"""
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
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k2d", FRONT_APP)
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
        raise AssertionError("K2D RED: " + err)
    return mod


@contextlib.contextmanager
def _front_with_points(points_dir):
    """把 front 的 POINTS_STATE_PATH 指向 tempdir 下的 state.json（隔离真实部署路径）。"""
    front = _front()
    old_path = getattr(front, "POINTS_STATE_PATH", None)
    front.POINTS_STATE_PATH = str(Path(points_dir) / "state.json")
    try:
        yield front
    finally:
        if old_path is None:
            front.__dict__.pop("POINTS_STATE_PATH", None)
        else:
            front.POINTS_STATE_PATH = old_path


@contextlib.contextmanager
def _front_with_points_and_stories(points_dir, stories_dir):
    """POINTS_STATE_PATH 与 STORIES_DIR 双隔离（A1 story 零波及 / A3 story_list 半）。"""
    front = _front()
    old_path = getattr(front, "POINTS_STATE_PATH", None)
    old_dir, old_acc = front.STORIES_DIR, front._story_acc
    front.POINTS_STATE_PATH = str(Path(points_dir) / "state.json")
    front.STORIES_DIR = str(stories_dir)
    front._story_acc = None
    try:
        yield front
    finally:
        if old_path is None:
            front.__dict__.pop("POINTS_STATE_PATH", None)
        else:
            front.POINTS_STATE_PATH = old_path
        front.STORIES_DIR = old_dir
        front._story_acc = old_acc


# --------------------------------------------------------------------------- #
# I/O doubles（K1 冻结 seam 同款形态：设备腿 + 引擎腿）                          #
# --------------------------------------------------------------------------- #


class _FakeDeviceSocket:
    """Device ws leg double: starlette-style receive dict + send_text/send_bytes."""

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


class _FakeEngineLeg:
    """Engine ws leg double: records .send()（kidpal_record_context 注入通道同型）。"""

    def __init__(self, fail_send=False):
        self.sent = []
        self.fail_send = fail_send

    async def send(self, text):
        if self.fail_send:
            raise RuntimeError("simulated engine leg failure")
        self.sent.append(text)


def _mcp_envelopes(sent_text):
    """kidpal/display tools/call envelopes as the device accepts them (type=mcp frames)."""
    out = []
    for text in sent_text:
        try:
            frame = json.loads(text)
        except (TypeError, ValueError):
            continue
        if isinstance(frame, dict) and frame.get("type") == "mcp":
            out.append(frame.get("payload") or {})
    return out


def _display_payloads(sent_text):
    """Reconstruct [{"view","data"}] from kidpal/display envelopes on the device leg."""
    out = []
    for env in _mcp_envelopes(sent_text):
        args = (env.get("params") or {}).get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _points_home_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "points_home"]


def _points_ledger_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "points_ledger"]


def _story_list_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "story_list"]


def _engine_frames(leg):
    """引擎腿收到的注入帧（JSON dict 列表；解析失败跳过）。"""
    out = []
    for text in leg.sent:
        try:
            frame = json.loads(text)
        except (TypeError, ValueError):
            continue
        if isinstance(frame, dict):
            out.append(frame)
    return out


# =========================================================================== #
# K2D-A1 — kidpal_points_delete 私有上行消费与幂等对账                            #
# =========================================================================== #


class K2dPointsDeleteAcceptance(unittest.TestCase):
    """K2D-A1: 删除上行整条消费、幂等 no-op 仍重推、非法 id 拒绝零重推、
    负余额帧不被钳位、story 账本零波及。"""

    maxDiff = None

    # coverage: K2D-A1 / S-F6.2-AC4·AC5（数据面半）+ D-03 + 契约 §四
    #           kidpal_points_delete + K2D-E2
    def test_k2d_a1_points_delete_private_uplink_idempotent_reconciliation(self):
        pts = _points_module()
        with tempfile.TemporaryDirectory(prefix="k2d_a1_pts_") as tmp_pts, \
                tempfile.TemporaryDirectory(prefix="k2d_a1_sto_") as tmp_sto:
            with _front_with_points_and_stories(tmp_pts, tmp_sto) as f:
                sp = f.POINTS_STATE_PATH
                consume = f._consume_kidpal_control
                self.assertTrue(
                    inspect.iscoroutinefunction(consume),
                    "K2D RED(infra): _consume_kidpal_control 非 async —— K1 已冻结"
                    "缝偏移（must-stay-green）。")

                # ---- 账本种子：+50 误记 -> +5 -> 兑 40（激活目标 40）----------
                entry_del = pts.add_points(sp, 50, "误记多加", now=_T0)
                pts.add_points(sp, 5, "今天扫地", now=_t(30))
                pts.redeem_points(sp, 40, now=_t(60))
                st = pts.read_state(sp)
                self.assertEqual(st["balance"], 15,
                                 "K2D RED(前置): 种子账本余额非 15（50+5−40）。")
                rev_before = st["revision"]
                dev = _FakeDeviceSocket()

                # ---- 存在 id 删除：整条消费、恰两帧权威重推（隐式 ack）--------
                obj = {"session_id": "sess-a1", "type": "kidpal_points_delete",
                       "id": entry_del["id"]}
                consumed = asyncio.run(consume("sess-a1", obj, dev))
                self.assertTrue(
                    consumed,
                    "K2D-A1 RED: kidpal_points_delete 未被 _consume_kidpal_control "
                    "消费（返回 False）—— front 删除上行消费分支缺席（设计态红；"
                    "返回 False 会把帧透传引擎 = 语义越界）。")
                homes = _points_home_frames(dev.sent_text)
                ledgers = _points_ledger_frames(dev.sent_text)
                self.assertEqual(len(homes), 1,
                                 f"K2D-A1 RED: 删除后 points_home 重推 {len(homes)} "
                                 "帧（应恰一帧——契约 §四隐式 ack 对账）。")
                self.assertEqual(len(ledgers), 1,
                                 f"K2D-A1 RED: 删除后 points_ledger 重推 {len(ledgers)} "
                                 "帧（应恰一帧首页）。")
                hdata = homes[0].get("data") or {}
                self.assertIs(hdata.get("restore"), True,
                              "K2D-A1 RED: 删除重推 home restore 非 True —— 会抢"
                              "前台（契约 §四：删除重推恒 restore:true）。")
                self.assertEqual(hdata.get("balance"), -35,
                                 "K2D-A1 RED: 删除重推余额非重算权威值 −35（15−50）"
                                 " —— 删除重算允许负余额（OQ-1 / D-03）。")
                self.assertEqual(hdata.get("revision"), rev_before + 1,
                                 "K2D-A1 RED: 删除重推 revision 未递增。")
                ldata = ledgers[0].get("data") or {}
                self.assertIs(ldata.get("restore"), True,
                              "K2D-A1 RED: 删除重推 ledger restore 非 True。")
                self.assertTrue((ldata.get("page") or {}).get("reset"),
                                "K2D-A1 RED: 删除重推 ledger page.reset 非 True —— "
                                "设备必须丢弃旧分页缓存再装首页（契约 §四）。")
                self.assertNotIn("target", ldata,
                                 "K2D-A1 RED: points_ledger 载荷带 target —— K0 "
                                 "ruling：ledger 载荷无 target 字段。")
                item_ids = [it.get("id") for it in (ldata.get("items") or [])]
                self.assertNotIn(entry_del["id"], item_ids,
                                 "K2D-A1 RED: 对账首页仍含已删 id —— 隐式 ack "
                                 "对账失真（设备 reconcile 永不冲销）。")
                self.assertEqual((ldata.get("page") or {}).get("total"), 2,
                                 "K2D-A1 RED: 对账首页 page.total 非 2 —— total "
                                 "是全量分录数，不能用 items.length 顶替。")
                # 整条消费 = 返回 True 已承载拦截语义（泵不转发引擎；M4K1-A2 同判据）。

                # ---- 幂等 no-op：重复/已不存在 id 仍重推对账（不写账）----------
                dev2 = _FakeDeviceSocket()
                consumed2 = asyncio.run(consume(
                    "sess-a1", {"session_id": "sess-a1",
                                "type": "kidpal_points_delete",
                                "id": entry_del["id"]}, dev2))
                self.assertTrue(consumed2,
                                "K2D-A1 RED: 幂等重删未被消费拦截。")
                st2 = pts.read_state(sp)
                self.assertEqual(st2["revision"], rev_before + 1,
                                 "K2D-A1 RED: 幂等 no-op 递增了 revision —— 已不"
                                 "存在的 id 是 no-op（契约 §四），不写账。")
                self.assertEqual(st2["balance"], -35,
                                 "K2D-A1 RED: 幂等 no-op 改动了余额。")
                self.assertEqual(len(_points_home_frames(dev2.sent_text)), 1,
                                 "K2D-A1 RED: 幂等 no-op 未重推 home —— 契约 §四"
                                 "「重复/已不存在 id = 幂等 no-op 仍重推」被破，"
                                 "设备端 pending 永挂。")
                self.assertEqual(len(_points_ledger_frames(dev2.sent_text)), 1,
                                 "K2D-A1 RED: 幂等 no-op 未重推 ledger 对账首页。")

                # ---- 非法 id：拒绝审计、零重推、仍消费拦截 ---------------------
                dev3 = _FakeDeviceSocket()
                with self.assertLogs("kidpal-front", level="WARNING") as logs:
                    for bad_id in ("", 123, None, "   "):
                        consumed3 = asyncio.run(consume(
                            "sess-a1", {"session_id": "sess-a1",
                                        "type": "kidpal_points_delete",
                                        "id": bad_id}, dev3))
                        self.assertTrue(
                            consumed3,
                            f"K2D-A1 RED: 非法 id={bad_id!r} 未被消费拦截 —— "
                            "控制帧绝不透传（透传 = 引擎可见设备私有帧）。")
                self.assertTrue(
                    any("points_delete" in rec.getMessage() for rec in logs.records),
                    "K2D-A1 RED: 非法 id 拒绝未留审计日志（WARNING 含 points_delete "
                    "记号）—— 契约 §四「非法 id 拒绝并审计」被破。")
                self.assertEqual(_points_home_frames(dev3.sent_text), [],
                                 "K2D-A1 RED: 非法 id 仍重推对账帧 —— 拒绝路径"
                                 "零重推（对伪造 id 重推 = 教设备相信伪造上行）。")
                self.assertEqual(_points_ledger_frames(dev3.sent_text), [],
                                 "K2D-A1 RED: 非法 id 仍重推 ledger。")

                # ---- story 账本零波及（删除消费不触 story 面）------------------
                self.assertEqual(list(Path(tmp_sto).glob("*")), [],
                                 "K2D-A1 RED: points 删除消费在 story 目录留下"
                                 "痕迹 —— 两账本独立（积分数字与故事星星不合并）。")


# =========================================================================== #
# K2D-A2 — 积分语境有界摘要注入（session_ctx points 两值 + selected_id）          #
# =========================================================================== #


class K2dPointsContextInjectionAcceptance(unittest.TestCase):
    """K2D-A2: ctx(points_home/points_ledger) 经引擎腿注入 kidpal_points_context
    有界摘要；selected_id 命中附分录摘要、未命中不伪造；注入失败不阻断拦截消费。"""

    maxDiff = None

    # coverage: K2D-A2 / S-F6.3 补充规则（积分语境）+ D-08 + 契约 §四
    #           kidpal_session_ctx + K2D-E2
    def test_k2d_a2_points_context_bounded_injection_with_selected_id(self):
        pts = _points_module()
        front = _front()
        signal_type = getattr(front, "POINTS_CTX_SIGNAL_TYPE", None)
        self.assertEqual(
            signal_type, POINTS_CTX_SIGNAL_TYPE_FROZEN,
            f"K2D-A2 RED: front_app.py 无 POINTS_CTX_SIGNAL_TYPE（或值非 "
            f"{POINTS_CTX_SIGNAL_TYPE_FROZEN!r}）—— 积分语境注入缝（D-08 / 契约 "
            "§四 points 两值）缺席（设计态红；注入帧 type 为本件冻结 wire 契约）。")

        with tempfile.TemporaryDirectory(prefix="k2d_a2_pts_") as tmp_pts, \
                tempfile.TemporaryDirectory(prefix="k2d_a2_sto_") as tmp_sto:
            with _front_with_points_and_stories(tmp_pts, tmp_sto) as f:
                sp = f.POINTS_STATE_PATH
                consume = f._consume_kidpal_control
                # 种子：四条分录（有界性负锚 = 最旧一条被 recent 3 条窗口排除）、
                # 激活目标出厂 40（余额 39 与目标 40 不撞值，断言各自可辨）。
                pts.add_points(sp, 5, "今天扫地", now=_T0)
                pts.add_points(sp, 2, "老师表扬", now=_t(10))
                pts.add_points(sp, 3, "主动洗碗", now=_t(20))
                pts.add_points(sp, 29, "整理房间", now=_t(30))
                st = pts.read_state(sp)
                oldest = st["entries"][-1]
                self.assertEqual(len(st["entries"]), 4, "K2D RED(前置): 种子非 4 条。")
                self.assertEqual(st["balance"], 39, "K2D RED(前置): 种子余额非 39。")

                # ---- ctx(points_home)：恰一帧注入，含余额/目标/最近 ≤3 条 ------
                dev = _FakeDeviceSocket()
                eng = _FakeEngineLeg()
                consumed = asyncio.run(consume(
                    "sess-a2", {"type": "kidpal_session_ctx",
                                "source_page": "points_home"}, dev, upstream=eng))
                self.assertTrue(consumed,
                                "K2D-A2 RED: ctx(points_home) 未被消费拦截。")
                inj = [fr for fr in _engine_frames(eng)
                       if fr.get("type") == POINTS_CTX_SIGNAL_TYPE_FROZEN]
                self.assertEqual(len(inj), 1,
                                 f"K2D-A2 RED: ctx(points_home) 注入 {len(inj)} 帧"
                                 "（应恰一帧）—— 积分语境注入缺席（设计态红）。")
                summary = inj[0].get("summary")
                self.assertIsInstance(
                    summary, str,
                    "K2D-A2 RED: 注入帧无 str summary —— 注入载荷形状偏移（冻结 "
                    '契约：{"type": "kidpal_points_context", "summary": <str>}）。')
                self.assertIn("40", summary,
                              "K2D-A2 RED: 摘要缺激活目标分值 40 —— 模型不知目标就"
                              "无法判「离游乐场还有多远」类问答（S-F6.3 积分语境"
                              "意图）。")
                self.assertIn("游乐场", summary,
                              "K2D-A2 RED: 摘要缺激活目标名。")
                self.assertIn("39", summary,
                              "K2D-A2 RED: 摘要缺当前权威余额 —— 语境摘要必须取自"
                              " points.py 单源（第二账本面 = 语境幻觉源）。")
                self.assertIn("整理房间", summary,
                              "K2D-A2 RED: 摘要缺最近分录 —— 「我为什么得了3分」"
                              "类问答无据可答。")
                self.assertIn("主动洗碗", summary,
                              "K2D-A2 RED: 摘要缺最近第 2 条分录。")
                self.assertNotIn(
                    oldest["reason"], summary,
                    "K2D-A2 RED: 摘要含第 4 条（超出 recent 3 条窗口的）分录 —— "
                    "注入必须**有界**（最近至多 3 条），把全量账本灌给模型 = 无界"
                    "上下文面（契约 §四「有界摘要」）。")

                # ---- ctx(points_ledger) + selected_id 命中：附分录摘要 ---------
                dev2 = _FakeDeviceSocket()
                eng2 = _FakeEngineLeg()
                sel_entry = st["entries"][0]  # 最新分录（delta 29 / 10→39 可辨值）
                consumed2 = asyncio.run(consume(
                    "sess-a2", {"type": "kidpal_session_ctx",
                                "source_page": "points_ledger",
                                "selected_id": sel_entry["id"]}, dev2, upstream=eng2))
                self.assertTrue(consumed2,
                                "K2D-A2 RED: ctx(points_ledger) 未被消费拦截。")
                inj2 = [fr for fr in _engine_frames(eng2)
                        if fr.get("type") == POINTS_CTX_SIGNAL_TYPE_FROZEN]
                self.assertEqual(len(inj2), 1,
                                 "K2D-A2 RED: ctx(points_ledger) 注入帧数 != 1。")
                summary2 = inj2[0].get("summary") or ""
                self.assertIn(sel_entry["reason"], summary2,
                              "K2D-A2 RED: selected_id 命中分录的摘要缺席 —— 详细"
                              "账本语境「这笔为什么是 10→39」类问答无据（契约 §四 "
                              "selected_id 供注入被问分录摘要）。")
                self.assertIn("29", summary2,
                              "K2D-A2 RED: 分录摘要缺 delta。")
                self.assertIn("10", summary2,
                              "K2D-A2 RED: 分录摘要缺变动前余额。")
                self.assertIn("39", summary2,
                              "K2D-A2 RED: 分录摘要缺变动后余额。")

                # ---- selected_id 未命中：常规摘要照注、不伪造分录 ---------------
                dev3 = _FakeDeviceSocket()
                eng3 = _FakeEngineLeg()
                consumed3 = asyncio.run(consume(
                    "sess-a2", {"type": "kidpal_session_ctx",
                                "source_page": "points_ledger",
                                "selected_id": "pt_not_exist"}, dev3, upstream=eng3))
                self.assertTrue(consumed3,
                                "K2D-A2 RED: 未命中 selected_id 的 ctx 未消费。")
                inj3 = [fr for fr in _engine_frames(eng3)
                        if fr.get("type") == POINTS_CTX_SIGNAL_TYPE_FROZEN]
                self.assertEqual(len(inj3), 1,
                                 "K2D-A2 RED: 未命中场景注入帧数 != 1（常规摘要应"
                                 "照注）。")
                summary3 = inj3[0].get("summary") or ""
                self.assertNotIn(
                    "pt_not_exist", summary3,
                    "K2D-A2 RED: 摘要回显了未命中 id —— 不得为不存在的分录编造"
                    "摘要（伪造分录 = 语境幻觉源）。")

                # ---- 注入失败不阻断拦截消费（ADR-013 注入先例同款）--------------
                dev4 = _FakeDeviceSocket()
                eng4 = _FakeEngineLeg(fail_send=True)
                try:
                    consumed4 = asyncio.run(consume(
                        "sess-a2", {"type": "kidpal_session_ctx",
                                    "source_page": "points_home"}, dev4, upstream=eng4))
                except Exception as e:  # noqa: BLE001
                    self.fail(f"K2D-A2 RED: 注入失败外抛炸消费 {e!r} —— 注入失败"
                              "必须降级留痕、不阻断 ctx 拦截消费（record ctx 注入"
                              "同款先例）。")
                self.assertTrue(consumed4,
                                "K2D-A2 RED: 注入失败后 ctx 未消费拦截。")
                # 重复 ctx 帧的注入粒度（每帧恰注 vs 按会话幂等去重）= 实现自由
                # （GAP-K2-3；契约只钉「重复发送安全」，不钉重复注入语义）。


# =========================================================================== #
# K2D-A3 — source_page 闭集守卫与 story 记录语境零回归                            #
# =========================================================================== #


class K2dSourcePageClosureAcceptance(unittest.TestCase):
    """K2D-A3: 积分两值不误切累积态；未知值拒绝记日志且绝不回退成 story；
    ctx(story) 切累积态 + story_list 推送零回归。"""

    maxDiff = None

    # coverage: K2D-A3 / 契约 §四（闭集 + 不得误切记录语境）+ S-F6.3 + M4K1-A2
    #           零回归 + K2D-E2
    def test_k2d_a3_source_page_closure_guard_and_story_zero_regression(self):
        pts = _points_module()
        with tempfile.TemporaryDirectory(prefix="k2d_a3_pts_") as tmp_pts, \
                tempfile.TemporaryDirectory(prefix="k2d_a3_sto_") as tmp_sto:
            with _front_with_points_and_stories(tmp_pts, tmp_sto) as f:
                consume = f._consume_kidpal_control
                pts.add_points(f.POINTS_STATE_PATH, 5, "今天扫地", now=_T0)

                # ---- 积分两值：不切累积态、不推 story_list、不注 record 信号 ----
                for value in ("points_home", "points_ledger"):
                    sid = f"sess-a3-{value}"
                    dev = _FakeDeviceSocket()
                    eng = _FakeEngineLeg()
                    consumed = asyncio.run(consume(
                        sid, {"type": "kidpal_session_ctx", "source_page": value},
                        dev, upstream=eng))
                    self.assertTrue(consumed,
                                    f"K2D-A3 RED: ctx({value}) 未消费拦截。")
                    self.assertEqual(
                        f._get_story_acc().routing(sid), "chat",
                        f"K2D-A3 RED: source_page={value} 误切记录（累积）语境 —— "
                        "契约 §四「P3 积分主页/详细账本均为普通问答语境，不得误切"
                        "记录语境」（故事星星与积分独立）。")
                    self.assertEqual(
                        _story_list_frames(dev.sent_text), [],
                        f"K2D-A3 RED: ctx({value}) 触发了 story_list 推送 —— "
                        "story 恢复重裁语义被积分语境误触（M4K1-A2 三时点①只属 "
                        "story）。")
                    rec_signals = [fr for fr in _engine_frames(eng)
                                   if fr.get("type") == "kidpal_record_context"]
                    self.assertEqual(
                        rec_signals, [],
                        f"K2D-A3 RED: ctx({value}) 注入了 record 语境信号 —— "
                        "ADR-013 等价信号只属 story 记录语境。")

                # ---- 未知值：拒绝记日志、整条消费、绝不回退成 story -------------
                sid_bad = "sess-a3-unknown"
                dev_bad = _FakeDeviceSocket()
                eng_bad = _FakeEngineLeg()
                with self.assertLogs("kidpal-front", level="WARNING") as logs:
                    consumed_bad = asyncio.run(consume(
                        sid_bad, {"type": "kidpal_session_ctx",
                                  "source_page": "whatever_new"}, dev_bad,
                        upstream=eng_bad))
                self.assertTrue(consumed_bad,
                                "K2D-A3 RED: 未知 source_page 未消费拦截。")
                self.assertTrue(
                    any("whatever_new" in rec.getMessage()
                        for rec in logs.records),
                    "K2D-A3 RED: 未知 source_page 未记日志 —— 契约 §四「未知值"
                    "拒绝并记日志」被破（静默吞掉会让新增取值的接线缺陷不可见）。")
                self.assertEqual(
                    f._get_story_acc().routing(sid_bad), "chat",
                    "K2D-A3 RED: 未知 source_page 回退成了 story 累积态 —— 契约 "
                    "§四「不得回退成 story」被破（K5 story_cover 等新值落地前"
                    "必须显式扩闭集，不得蹭 story 语义）。")
                self.assertEqual(
                    _story_list_frames(dev_bad.sent_text), [],
                    "K2D-A3 RED: 未知 source_page 触发了 story_list 推送。")

                # ---- ctx(story) 零回归：切累积态 + story_list 推送（M4K1-A2）----
                sid_story = "sess-a3-story"
                dev_story = _FakeDeviceSocket()
                eng_story = _FakeEngineLeg()
                consumed_story = asyncio.run(consume(
                    sid_story, {"type": "kidpal_session_ctx",
                                "source_page": "story"}, dev_story,
                    upstream=eng_story))
                self.assertTrue(consumed_story,
                                "K2D-A3 RED: ctx(story) 未消费拦截（回归）。")
                self.assertEqual(
                    f._get_story_acc().routing(sid_story), "accumulate",
                    "K2D-A3 RED: ctx(story) 未切累积态 —— 既有记录语境接线被本批"
                    "改动破坏（M4K1-A2 冻结域 must-stay-green）。")
                self.assertTrue(
                    len(_story_list_frames(dev_story.sent_text)) == 1,
                    "K2D-A3 RED: ctx(story) 未推恰一帧 story_list —— 三时点①恢复"
                    "重裁零回归被破（M4K1-A2 冻结域）。")
                rec_story = [fr for fr in _engine_frames(eng_story)
                             if fr.get("type") == "kidpal_record_context"]
                self.assertEqual(len(rec_story), 1,
                                 "K2D-A3 RED: ctx(story) record 信号注入零回归被破"
                                 "（Package B 等价信号恰一帧）。")


# =========================================================================== #
# K2D-A4 — 不完整句零写入（server 消费面半）与单写入口                            #
# =========================================================================== #


class K2dZeroWriteAcceptance(unittest.TestCase):
    """K2D-A4: 普通 stt 帧不拦截不落账；非 control 载荷零写入；意图拒绝零写入
    （K0 冻结回归半）；front 的 points 写入唯一入口结构锚。"""

    maxDiff = None

    # coverage: K2D-A4 / S-F6.3-AC1（server 消费面半）+ D-04 + NFR-OFF-1
    #           + K2D-E2
    def test_k2d_a4_incomplete_sentence_zero_write_and_single_write_entry(self):
        pts = _points_module()
        front = _front()
        with tempfile.TemporaryDirectory(prefix="k2d_a4_pts_") as tmp:
            with _front_with_points(tmp) as f:
                sp = f.POINTS_STATE_PATH

                # ---- 普通 stt 帧（随口事件句）-> 不拦截、零写入 ------------------
                dev = _FakeDeviceSocket()
                consumed = asyncio.run(f._consume_kidpal_control(
                    "sess-a4", {"type": "stt",
                                "text": "我们去游乐场了"}, dev))
                self.assertFalse(
                    consumed,
                    "K2D-A4 RED: 普通 stt 帧被 _consume_kidpal_control 拦截 —— "
                    "对话主路径不得被积分消费面吞掉（普通对话走引擎）。")
                self.assertFalse(
                    Path(sp).exists(),
                    "K2D-A4 RED: 普通对话在磁盘留下 points state —— 不完整句/"
                    "随口事件**零写入**（S-F6.3 AC1：只提事件不带分值动作不落账；"
                    "NFR-OFF-1 无离线补记）。")

                # ---- 非 control 载荷 -> 意图缝拒绝、零写入 ----------------------
                dev2 = _FakeDeviceSocket()
                for payload in ({"view": "points_home", "data": {}},
                                {"view": "text", "data": {"text": "hi"}},
                                {"type": "kidpal_session_ctx",
                                 "source_page": "points_home"}):
                    ret = asyncio.run(f._consume_points_intent(dev2, payload))
                    self.assertFalse(
                        ret,
                        f"K2D-A4 RED: 非 control 载荷 {payload.get('view')} 被"
                        "意图缝消费 —— 薄意图截获只认 control/{add,redeem}_points"
                        "（K0PTS-A1 冻结域）。")
                self.assertFalse(
                    Path(sp).exists(),
                    "K2D-A4 RED: 意图缝对非 control 载荷落了账。")

                # ---- 意图缺分值/事由 -> 拒绝零写入（K0 冻结回归半）---------------
                dev3 = _FakeDeviceSocket()
                for bad in ({"view": "control",
                             "data": {"action": "add_points", "points": 5}},
                            {"view": "control",
                             "data": {"action": "add_points", "points": 0,
                                      "reason": "零分"}},
                            {"view": "control",
                             "data": {"action": "add_points", "points": True,
                                      "reason": "布尔分值"}}):
                    ret = asyncio.run(f._consume_points_intent(dev3, bad))
                    self.assertTrue(
                        ret, f"K2D-A4 RED: 畸形意图 {bad} 未被消费拦截（控制帧"
                        "不透传，K0PTS-A1 冻结域）。")
                self.assertFalse(
                    Path(sp).exists(),
                    "K2D-A4 RED: 缺事由/零分/布尔分值的意图落了账 —— 不完整记账"
                    "意图零写入（S-F6.3 AC1 server 半；K0PTS-A1 冻结回归）。")

                # ---- 单写入口结构锚：ctx/删除/取页路径零 add/redeem 调用 --------
                # （D-10 front 半：落账唯一入口 = _consume_points_intent；D-04：
                #   删除只走 delete_entry。设备语音不开放删除、语境不落账。）
                consume_src = inspect.getsource(front._consume_kidpal_control)
                self.assertNotIn(
                    "gw_points.add_points", consume_src,
                    "K2D-A4 RED: _consume_kidpal_control 内出现 add_points 调用 —— "
                    "设备私有控制帧路径产了写账入口（D-10 单写入口被破）。")
                self.assertNotIn(
                    "gw_points.redeem_points", consume_src,
                    "K2D-A4 RED: _consume_kidpal_control 内出现 redeem_points 调用 "
                    "—— 同上（设备语音不开放删除/直写，D-04）。")
                intent_src = inspect.getsource(front._consume_points_intent)
                self.assertIn(
                    "gw_points.add_points", intent_src,
                    "K2D-A4 RED: 薄意图缝丢失 add_points 执行调用（K0 冻结回归）。")
                mod_src = inspect.getsource(front)
                self.assertEqual(
                    mod_src.count("gw_points.add_points("), 1,
                    "K2D-A4 RED: front 的 add_points 调用点 != 1 —— 薄意图截获是"
                    "唯一写账入口（D-10 单 writer 的 front 半；第二入口 = 第二"
                    "账本面）。")


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
