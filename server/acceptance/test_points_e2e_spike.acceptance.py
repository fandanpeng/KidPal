"""Frozen acceptance tests — TaskPackage 261104-k0-points-e2e-spike（server 面）。

承载 story = S-F6.3（设备语音记账与兑换，AC1/AC2 最小通路的 server 承载半）+
S-F6.4（QQ 家长记账与更正，AC1/AC5 最小通路的 server 承载半）+ S-F6.2-AC1（详细账本
全量可及的首尾分页 canary 离线半）+ NFR-PERF-4（离线半 = 无；实测时延归 live-manual
K0PTS-L3）+ D-spike-first（ARQ-261024，261103-star-points-story-cover review.md D 节）。
上游冻结 = delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md（FROZEN v1.0，
D-07/D-08/D-10）+ reviewed plan delivery/taskpackages/261104-k0-points-e2e-spike/plan.md
（铁律约束 + demo 证据形态）+ 门 1 裁定（真实 QQ 最小 producer = 真实 QQ 一次性明确指令
→ 既有 main agent → K0 最小 add CLI，不建临时 skill）。

Scope of record（taskpackage acceptance K0PTS-A1..A6）:
  K0PTS-A1 薄意图截获与最小写入落账：引擎 display 流中 control/add_points 薄意图被
        front 截获（不透传设备）→ points.py 追加分录落账 → 权威 points_home 恰一帧
        自推（restore 缺席 = 语音成功路径接管前台）。模型不产余额/分录 id/revision。
  K0PTS-A2 兑换薄意图门禁与拒绝路径：redeem 只认当前激活目标分值、余额不足拒、拒绝
        不写账；薄意图畸形（points 非正整数 / 缺事由）拒绝执行不落账不推送。
  K0PTS-A3 points 视图白名单与积分来源门禁：display_schema 白名单 7→9、既有 7 零偏移；
        引擎侧完整 points 帧拒转降级（story 单源先例同型）；空 data 导航薄载荷由 front
        展开为完整帧；workspace-kidpal AGENTS.md 薄意图纪律锚。
  K0PTS-A4 restore 推送不抢前台语义（数据面）：QQ/恢复通道推送带 data.restore=true、
        语音成功通道不带 restore；restore 键形状 floor（非布尔拒）；截获缝真接线锚。
  K0PTS-A5 kidpal_points_page 首尾分页 canary：keyset 分页从最新到最早两条 canary 均
        可及；首页 reset=true 无 restore、续页 restore=true reset=false；游标非法/
        known_revision 不符回最新首页 reset=true；limit 越界拒绝不回帧；空账权威 items=[]。
  K0PTS-A6 QQ 最小 add CLI 单源：`python -m gateway.points` 与 front 共享同一 points.py
        执行器与同一 state 文件（无第二账本）；非法输入非零退出不写账。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — points.py 纯模块离线 import，
front_app.py 离线装载（fastapi 缺席本地 stub，M4K1/RDG 先例），display_schema 真门。

  A) server/gateway/points.py（新建；单一确定性执行器，D-10——LLM 只产薄意图、
     本模块独占余额计算与写入；K1 在同一文件上正式化单 writer，本包不另起账本）:

    DEFAULT_TARGET（模块级 dict 常量，**形状与值冻结**）
        当前唯一激活目标（出厂默认 = 游乐场 40，requirements S-F6.5 AC1）:
        {"id": "preset-playground", "name": "游乐场", "points": 40, "asset": "sc-wheel"}
        target 配置管理归 K3/K4；spike 只读此常量。

    def read_state(path) -> dict
        {"revision": int, "balance": int, "entries": [entry, ...]}（entries 时间倒序）。
        文件不存在 -> 零状态 {"revision": 0, "balance": 0, "entries": []}；
        revision 单调递增（每次写 +1，写前 0）；balance == sum(e["delta"])。

    def add_points(path, points, reason, *, now=None) -> dict
        追加一条加分分录并持久化，返回**新分录 dict**（六键冻结形状）:
        {"id": 非空 str（points.py 生成）, "delta": +points, "reason": reason,
         "occurred_at": 带时区 ISO8601 str, "balance_before": int, "balance_after": int}
        拒绝（raise ValueError，不写账）: points 非正整数（bool 不算整数）/ reason
        非非空 str。now=None -> 墙钟（datetime.now(timezone.utc)）；注入 now 用于验收。

    def redeem_points(path, points, *, now=None) -> dict
        兑换落一条负向分录，reason 统一取 f"兑换{DEFAULT_TARGET['name']}"（模型不提供
        target id / 负 delta）。拒绝（raise ValueError，**不写账**）:
        points != DEFAULT_TARGET["points"]（只认激活目标分值，S-F6.3-AC3）或
        当前 balance < points（余额不足）。成功形状同 add_points（delta 为负）。

    def build_points_home(path) -> dict
        契约 §1.3⑦ 完整载荷（**无 restore 键**——restore 由 front 按通道加）:
        {"view": "points_home",
         "data": {"revision": int, "balance": int,
                  "target": {四键同 DEFAULT_TARGET},
                  "recent": [分录六键, ...] 时间倒序最多 3 条}}

    def build_points_ledger_page(path, cursor=None, known_revision=None, limit=6) -> dict
        契约 §1.3⑧ 完整载荷（无 restore 键；分页 wire 规则 = 契约 §四 kidpal_points_page）:
        {"view": "points_ledger",
         "data": {"revision": int, "balance": int, "items": [分录六键, ...] 时间倒序,
                  "page": {"total": int, "next_cursor": str|None,
                           "has_more": bool, "reset": bool}}}
        cursor 缺省/None = 最新首页（page.reset=True）；续页按 (occurred_at, id) 时间
        倒序 keyset 定位（**不用 offset**），正常续页 reset=False；游标非法/过期或
        known_revision 与当前 revision 不一致 -> 回最新首页且 reset=True；
        next_cursor=None 且 has_more=False = 已到最早一页；page.total = 全量分录数
        （不用 items.length 代替）；limit 允许 1–50，越界 raise ValueError。
        空账 -> items=[], page.total=0, balance=0（权威空账，不伪造分录）。

    def main(argv=None) -> int（模块级 CLI 入口 + `if __name__ == "__main__"` 守卫）
        `python -m gateway.points add <points> <reason>` / `python -m gateway.points
        redeem <points>`。state 路径单源 = os.environ["KIDPAL_POINTS_STATE"]（与
        front_app 同一 env 名）。成功 exit 0；非法/拒绝 exit 非 0、**不写账**、说明写
        stderr、不伪造成功回执。这是门 1 裁定的「K0 最小 add CLI」执行器面。

  B) server/gateway/display_schema.py（机械门增量）:

    VALID_VIEWS 增 "points_home" / "points_ledger"（白名单 7→9；既有 7 个零偏移）。
    points 形状 floor（story 轻量 floor 先例对称——只把关 shape、不作逐字段深度校验）:
      data == {} -> 放行（导航薄载荷，front 展开为完整帧）；
      非 {} 完整形状 -> 须含 revision/balance（整数，bool 不算）；target（dict 含
      id/name/points/asset 四键）**仅 points_home 必带**（契约 §1.3⑦ target 形状），
      points_ledger 载荷无 target 字段不查（契约 §1.3⑧）——对账修订：本段原把
      target 四键写成通用完整形状必带项，与 §1.3⑧ 及实现（display_schema.
      _points_floor 只对 points_home 查 target）不符，2026-11-04 planner 裁定按
      tighten-only 注释级修订更正；points_home 另认 recent（list）、points_ledger
      另认 items（list）与 page（dict 含 total/next_cursor/has_more/reset 四键）、
      restore 可选布尔（在场非布尔 -> 拒绝，畸形 restore 帧不入板盲渲染）。
    GATEWAY_DECISION_CONTROL_ACTIONS 增 "add_points" / "redeem_points"（契约 §1.4
      网关决定类）——relay 路径拒转，**self_push=True 亦拒**（与 start_organizing 唯
      一豁免口不同：front 不把控制 points 帧发给任何一端，设备只消费执行后的权威积分
      视图；FORWARDABLE_CONTROL_ACTIONS 不含两 action，决定类与可透传类不相交）。

  C) server/gateway/front/front_app.py（接线面；离线可装载纪律不变）:

    POINTS_STATE_PATH（模块级；os.environ.get("KIDPAL_POINTS_STATE", <部署缺省>)）
        state 文件路径单源（env 名与 CLI 同源）。测试经 monkeypatch 该模块属性隔离。

    _expand_points_nav_intent(payload) -> dict | None（_expand_schedule_intent 同型落位
        build_display_tool_calls、机械门裁决之前）:
        view ∈ {points_home, points_ledger} 且 data == {} -> 由 points.py 当前状态
        展开完整帧（build_points_home / build_points_ledger_page 首页；**不带
        restore**——显式「打开积分本/详细账本」= 主动展示）；其它形状返回 None 原样
        走既有链。展开结果 **不经引擎侧单源降级**（展开即 front 机械产物）。

    引擎侧完整 points 帧拒转（story 单源先例同型）：build_display_tool_calls 对
        引擎来源的非薄载荷 points_home / points_ledger 块降级为 friendly text（新增
        _POINTS_SINGLE_SOURCE_VIEWS 或并入既有单源降级集合均可），绝不把引擎侧完整
        points 帧透传设备（积分来源门禁，契约 §1.5）。

    async def _consume_points_intent(websocket, payload, *, restore=False) -> bool
        薄意图截获执行缝（keyword-only restore，缺省 False）：
        payload 为 control/{add_points, redeem_points} 薄意图 -> 校验（add: points
        正整数非 bool 且 reason 非空 str；redeem: points 正整数）-> 合法：建立
        command_id 并记日志（corr_id 面）-> 调 points.add_points / redeem_points(
        POINTS_STATE_PATH, ...) -> 成功后 _push_display_payload(websocket,
        build_points_home(POINTS_STATE_PATH))，restore=True 时给 data 加
        "restore": True（QQ/恢复通道），缺省 False = 语音成功路径不带 restore（设备
        接管前台跳积分页）；拒绝（畸形 / 执行 ValueError）-> **不写账、不推
        points_home**、返回 True（已消费，控制帧不透传）。非 points 控制帧 -> 返回
        False（既有路由零偏移）。返回 True 的块调用方**不得再中继**（不降级不透传）。
        _relay_display_blocks_to_device 内在 build_display_tool_calls 之前逐块调用
        （截获真接线——两侧 relay 路径收敛于此；只可直调不可达设备 = 空过）。

    _consume_kidpal_control 增 mtype == "kidpal_points_page" 分支（kidpal_story_fetch
        同型）：整条消费并拦截、不下透引擎；cursor 缺省/null = 最新首页；limit 可选
        1–50、越界拒绝并记日志、**不回帧**；畸形载荷 no-op 拦截；以 points.py 当前
        状态 build_points_ledger_page（传 POINTS_STATE_PATH、透传 cursor/
        known_revision/limit）-> 首页（cursor None）不加 restore；续页（cursor 非空）
        加 data["restore"]=True（**续页触发的重置仍带 restore:true**，契约 §四）；
        经 _push_display_payload 恰一帧应答。

  D) server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md（prompt 载荷，薄意图纪律）:

    完整加分句只产 control/add_points {points, reason}、完整兑换句只产
    control/redeem_points {points}；不产余额/激活目标/最近记录/分录 id/前后余额/
    revision/游标/资产/restore，不宣称已记账/兑换成功（权威数据只由 front 依据
    points.py 推送）；缺事由/动作词/分值时不产记账动作、只说普通答复（不完整句不落
    账，S-F6.3-AC1 句式门槛）。

Shape authority = docs/contracts/interface-contract.md §1.3⑦⑧ / §1.4 add_points/
redeem_points 表行 / §1.5 积分来源门禁与白名单 7→9 / §四 kidpal_points_page（均
2026-11-04 LANDED）+ docs/requirements.md v1.71 S-F6.2-AC1 / S-F6.3 / S-F6.4 / NFR-PERF-4。

跨端 wire 形状锚（对照对端实际接受什么；本包固件冻结件 K0PTS-F1..F4 钉消费半）:
  firmware/main/display/view_router.c kidpal_display_route —— view 字符串精确比较，
    "points_home" -> VRT_ROUTE_POINTS_HOME(7) / "points_ledger" -> VRT_ROUTE_POINTS_
    LEDGER(8)（additive 尾部，既有 0..6 零偏移）；未知/NULL -> VRT_ROUTE_UNKNOWN(-1)。
  firmware/main/mcp_server.cc kidpal/display tools/call —— view 与 data Properties
    均 String 型、data 由设备自解析（_display_wire_arguments wire 形状依据）。
  firmware/main/display/points_page.c —— points_home/points_ledger 载荷最小消费
    （restore 分流 + 分页缓存纪律，K0PTS-F2/F3 钉）。
  firmware/main/protocols/protocol.cc —— kidpal_points_page 上行消息构造（K0PTS-F4
    驱动方锚）。

契约条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 验收断言」节）:

| 契约 / 决策条目 | 承载断言 | 性质档 |
|---|---|---|
| D-10 薄意图→确定性写账（spike 半） | test_k0pts_a1（截获/落账/权威推送真跑） | 行为级 |
| 契约 §1.4 add_points 网关决定类（不直接下发设备） | test_k0pts_a1（控制帧不透传 + relay 拒转）+ test_k0pts_a3（决定类登记） | 行为级 |
| 契约 §1.4 redeem_points 只认目标分值 | test_k0pts_a2（校验拒绝真跑） | 行为级 |
| S-F6.3-AC1 完整句记账（server 半） | test_k0pts_a1 + AGENTS.md 锚（test_k0pts_a3） | 行为级 + 源锚代理（prompt 半） |
| S-F6.4-AC1 QQ 记账同构（最小通路 server 半） | test_k0pts_a6（CLI 同一执行器单源真跑） | 行为级 |
| S-F6.4-AC5 / D-07 推送不抢前台（数据面） | test_k0pts_a4（restore kwarg + wire 断言 + floor） | 行为级（真实 QQ 链路归 live-manual L2） |
| S-F6.2-AC1 全量可及（离线半）/ 契约 §四 kidpal_points_page | test_k0pts_a5（keyset 首尾 canary + reset/续页语义真跑） | 行为级 |
| 契约 §1.5 白名单 7→9 + 积分来源门禁 | test_k0pts_a3（真门真跑 + relay 降级 + 薄载荷展开） | 行为级 |
| 契约 §1.3⑦ points_home 权威生产纪律 | test_k0pts_a1（字段逐项）+ test_k0pts_a4（floor） | 行为级 |
| 门 1 裁定最小 add CLI 单源 | test_k0pts_a6（subprocess 真跑 + 单源断言） | 行为级 |
| 既有 display_schema / front relay 行为 | 各 test 内回归守卫半 | 行为级（must-stay-green） |

弱于契约处（显式缺口，不得当已兑现）:
  GAP-1 QQ 通道 → restore 推送的真实接线（真实 QQ 消息流中谁传 restore=True、推送
        是否真达设备）离线不可驱动——本文件只钉 seam kwarg 与 wire 数据面语义；真实
        QQ 双状态 canary（不抢前台 / 原地刷新）归 live-manual K0PTS-L2（K0S-E1/E3，
        spike-evidence.md 真值判定）。
  GAP-2 「按真实结果播报」的 TTS 话术 = 引擎 LLM 面，离线不钉（front 只钉推送半）。
  GAP-3 NFR-PERF-4 记账反馈时延 = live-manual 实测记录（K0PTS-L3），不写死阈值
        （requirements 明示阈值待真机调优定数）。
  GAP-4 短窗去重、删除、目标切换、自定义目标 = K1/K2/K3/K4 边界，本包不钉、不放宽。
  GAP-5 设备真实看屏（余额与原因可见、首尾 canary 看屏）= live-manual（K0PTS-L1/L4），
        文件写入与 pytest 读回不替代消费端目击（spike-first 铁律）。
  GAP-6 QQ CLI 到 main agent 的工具登记/调用链（OpenClaw 侧）= 部署面 + live 半；
        离线只钉 CLI 执行器本身（A6）。

行为等价假设（显式；不写即默认不成立）:
  EQ-1 「_push_display_payload 推送帧 = 设备实际收帧形状」（M4K1-A3 / RDG EQ-2 同
        一重建器口径——kidpal/display mcp 信封 + view/data 双 String wire）。
  EQ-2 「kidpal_display_blocks 帧驱动 = 引擎 display 流真实路径」（RDG start_
        recording 截获同款驱动法，两侧 relay 路径收敛于 _relay_display_blocks_to_device）。
  EQ-3 「subprocess main() 执行 = main agent 工具调用执行同一入口」（同一模块函数 +
        同一 env 名单源；真实 QQ 链路归 live-manual L2）。
  EQ-4 「points.py 文件持久化 = 云端持久账本」（spike 最小形态；K1 在同一文件上补
        锁/原子写/重算/幂等，不另起第二账本）。

Coverage map (acceptanceRef -> test):
  K0PTS-A1 -> test_k0pts_a1_thin_intent_intercept_min_write_authoritative_push
  K0PTS-A2 -> test_k0pts_a2_redeem_target_gate_and_refusal_paths
  K0PTS-A3 -> test_k0pts_a3_points_views_whitelist_source_gate_and_prompt_anchor
  K0PTS-A4 -> test_k0pts_a4_restore_push_cache_only_semantics
  K0PTS-A5 -> test_k0pts_a5_first_last_pagination_canary_via_kidpal_points_page
  K0PTS-A6 -> test_k0pts_a6_qq_minimal_add_cli_single_writer
  coverage: K0PTS-L1 —— live-manual 真实设备语音记账目击（载体 spike-evidence.md，
        不落自动断言）
  coverage: K0PTS-L2 —— live-manual 真实 QQ 双状态 canary（同上）
  coverage: K0PTS-L3 —— live-manual NFR-PERF-4 实测时延记录（同上）
  coverage: K0PTS-L4 —— live-manual 首尾 canary 真机消费目击（同上）。
        四项均不落自动断言、不计入本文件通过面（2026-11-04 注释级修订：原区间/
        斜杠缩写「K0PTS-L1..L4 / L1/L2/L3/L4」按 ARQ-097/260731 收紧规则不被
        acceptance-coverage 机械闸识别，planner 裁定逐 id 显式标记；spike-first
        铁律不变：文件写入与 pytest 读回不替代设备消费端目击）。

Run protocol（offline；-m "not live" 可跑；文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" \\
      server/acceptance/test_points_e2e_spike.acceptance.py

REVISION LOG
  2026-11-04 初版冻结（acceptance-author；K0PTS-A1..A6 server 离线层；K0PTS-L1..L4
             live-manual 层在 taskpackage acceptance[] 单列不落本文件；firmware 半 =
             firmware/main/test/points_spike.acceptance.c K0PTS-F1..F4。设计态红 =
             points.py 不在世 / display_schema 白名单未扩 / front 接线缺席）。
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import json
import os
import subprocess
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
AGENTS_MD = SERVER / "deploy" / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md"

# 冻结协调字面（front 与 CLI 的 state 路径单源；契约 §四 + 门 1 裁定）。
POINTS_STATE_ENV = "KIDPAL_POINTS_STATE"

# 出厂默认激活目标（requirements S-F6.5 AC1；points.py DEFAULT_TARGET 冻结值）。
DEFAULT_TARGET_FROZEN = {
    "id": "preset-playground",
    "name": "游乐场",
    "points": 40,
    "asset": "sc-wheel",
}

# 分页 wire 常量（契约 §四 kidpal_points_page）。
POINTS_PAGE_LIMIT_DEFAULT = 6
POINTS_PAGE_LIMIT_MAX = 50

# 首尾分页 canary 事由 token（与 spike-evidence.md / 设备消费日志按同一 token 关联）。
CANARY_EARLIEST_TOKEN = "CANARY-EARLIEST"
CANARY_LATEST_TOKEN = "CANARY-LATEST"

_TZ_CN = dt_timezone(timedelta(hours=8))
_T_EARLY = datetime(2026, 11, 3, 10, 0, 0, tzinfo=_TZ_CN)
_T_LATE = datetime(2026, 11, 4, 9, 30, 0, tzinfo=_TZ_CN)


# --------------------------------------------------------------------------- #
# Module loading (missing/unimportable = RED for missing behaviour)           #
# --------------------------------------------------------------------------- #

_points_module_cache = None
_front_module_cache = None


def _points_module():
    """server/gateway/points.py 纯模块。缺失 = 设计态红（frozen contract 未实现）。"""
    global _points_module_cache
    if _points_module_cache is not None:
        return _points_module_cache
    if not POINTS_PY.is_file():
        raise AssertionError(
            "K0PTS RED: server/gateway/points.py 不在世 —— 积分确定性执行器（D-10 "
            "spike 半：add/redeem 落账 + 权威载荷生产 + 分页）未实现。")
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.points as pts  # noqa: PLC0415 — offline import, cached
    _points_module_cache = pts
    return pts


def _points_attr(name: str):
    pts = _points_module()
    val = getattr(pts, name, None)
    if val is None:
        raise AssertionError(
            f"K0PTS RED: points.py 无模块级 {name} —— frozen K0PTS offline contract "
            "（seam 名冻结）未实现。")
    return val


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（M4K1 / RDG 冻结件同款先例）。"""
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
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k0pts", FRONT_APP)
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
        raise AssertionError("K0PTS RED: " + err)
    return mod


def _display_schema():
    """真实冻结机械门 gateway.display_schema。"""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415
    return ds


def _front_source() -> str:
    try:
        return FRONT_APP.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


@contextlib.contextmanager
def _front_with_points(points_dir):
    """把 front 的 POINTS_STATE_PATH 指向 tempdir 下的 state.json（隔离真实部署路径）。

    state 路径 = tempdir/state.json（文件名实现自由——本桩只 monkeypatch front 模块
    属性，路径形状由实现自洽；隔离义务 = 绝不触真实 /opt/kidpal 部署面）。"""
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


# --------------------------------------------------------------------------- #
# I/O doubles（M4K1 / RDG 冻结 seam 同款形态：只替代 WebSocket 两端）             #
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


def _engine_to_device(front, session_id, frames):
    """Run the engine->device downlink pump; returns (upstream, device_socket)."""
    texts = [f if isinstance(f, str) else json.dumps(f, ensure_ascii=False) for f in frames]
    up = _FakeUpstream(frames=texts)
    dev = _FakeDeviceSocket()
    asyncio.run(front.pump_upstream_to_client(up, dev, session_id))
    return up, dev


def _mcp_envelopes(sent_text):
    """kidpal/display tools/call envelopes as the device accepts them (type=mcp frames)."""
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


def _control_frames(sent_text):
    return [p for p in _display_payloads(sent_text) if p.get("view") == "control"]


def _relay_envelope_payloads(calls):
    """Reconstruct [{"view","data"}] from build_display_tool_calls outputs."""
    out = []
    for call in calls:
        args = (call.get("params") or {}).get("arguments") or {}
        data = args.get("data")
        try:
            decoded = json.loads(data) if isinstance(data, str) else None
        except (TypeError, ValueError):
            decoded = None
        out.append({"view": args.get("view"), "data": decoded})
    return out


def _source_region(src: str, start_needle: str, end_needle: str) -> str:
    """[start_needle, end_needle) 源切片（接线锚用；找不到 start 返回空串）。"""
    i = src.find(start_needle)
    if i < 0:
        return ""
    j = src.find(end_needle, i + len(start_needle))
    return src[i:] if j < 0 else src[i:j]


def _entry_keys_ok(entry) -> bool:
    """分录六键形状（契约 §1.3⑦ 统一分录形状）。"""
    return (isinstance(entry, dict)
            and set(entry.keys()) == {"id", "delta", "reason", "occurred_at",
                                      "balance_before", "balance_after"}
            and isinstance(entry["id"], str) and entry["id"]
            and isinstance(entry["delta"], int) and not isinstance(entry["delta"], bool)
            and isinstance(entry["reason"], str)
            and isinstance(entry["occurred_at"], str) and entry["occurred_at"]
            and isinstance(entry["balance_before"], int)
            and not isinstance(entry["balance_before"], bool)
            and isinstance(entry["balance_after"], int)
            and not isinstance(entry["balance_after"], bool))


# =========================================================================== #
# K0PTS-A1 — 薄意图截获 + 最小写入落账 + 权威 points_home 自推                    #
# =========================================================================== #


class K0ptsThinIntentAcceptance(unittest.TestCase):
    """K0PTS-A1: 引擎 display 流 control/add_points 截获 → points.py 落账 → 权威推送。"""

    maxDiff = None

    # coverage: K0PTS-A1 / S-F6.3-AC1·AC2（server 半）+ D-10 + D-08-spike +
    #           契约 §1.4 add_points「不直接下发设备」+ §1.3⑦ 权威生产纪律
    def test_k0pts_a1_thin_intent_intercept_min_write_authoritative_push(self):
        pts = _points_module()
        # ---- 执行器真跑：add_points 落账（六键分录 / 余额重算 / revision 递增） ----
        with tempfile.TemporaryDirectory(prefix="k0pts_a1_exec_") as tmp:
            state = str(Path(tmp) / "state.json")
            entry = pts.add_points(state, 5, "今天扫地", now=_T_EARLY)
            self.assertTrue(_entry_keys_ok(entry),
                            f"K0PTS-A1 RED: add_points 分录形状非六键冻结形状: {entry!r}"
                            "（契约 §1.3⑦ 统一分录形状：id/delta/reason/occurred_at/"
                            "balance_before/balance_after）。")
            self.assertEqual(entry["delta"], 5, "K0PTS-A1 RED: 加分分录 delta 非 +5。")
            self.assertEqual(entry["balance_before"], 0, "K0PTS-A1 RED: 首笔 balance_before 非 0。")
            self.assertEqual(entry["balance_after"], 5, "K0PTS-A1 RED: 首笔 balance_after 非 5。")
            self.assertIn("+08:00", entry["occurred_at"],
                          "K0PTS-A1 RED: occurred_at 非带时区 ISO8601（契约 §1.3⑦）。")
            st = pts.read_state(state)
            self.assertEqual(st["balance"], 5, "K0PTS-A1 RED: balance 非分录重算结果。")
            self.assertEqual(st["revision"], 1, "K0PTS-A1 RED: 首笔写后 revision 非 1。")
            self.assertEqual(st["entries"][0]["reason"], "今天扫地",
                             "K0PTS-A1 RED: 分录事由丢失。")
            # 模型不产权威字段（D-10）：执行器入参只有 points/reason——余额等权威值由
            # points.py 计算，API 形状上不存在「模型传余额」的入口。
            import inspect  # noqa: PLC0415
            sig = inspect.signature(pts.add_points)
            for forbidden in ("balance", "revision", "entry_id", "cursor"):
                self.assertNotIn(forbidden, sig.parameters,
                                 f"K0PTS-A1 RED: add_points 入参暴露权威字段 {forbidden!r}"
                                 " —— 模型可产余额/分录 id = D-10 双写者面。")

        # ---- 截获沿真跑：引擎 display 流薄意图 → 恰一帧权威 points_home ----------
        with tempfile.TemporaryDirectory(prefix="k0pts_a1_wire_") as tmp:
            with _front_with_points(tmp) as front:
                sid = "sess-k0pts-a1"
                block = json.dumps({"view": "control",
                                    "data": {"action": "add_points", "points": 5,
                                             "reason": "今天扫地"}},
                                   ensure_ascii=False)
                up, dev = _engine_to_device(
                    front, sid,
                    [{"type": "kidpal_display_blocks", "blocks": [block]}])
                # 落账辅证：state 文件真实写入（辅证，不替代设备目击）。
                st = pts.read_state(str(Path(tmp) / "state.json"))
                self.assertEqual(len(st["entries"]), 1,
                                 "K0PTS-A1 RED: 薄意图截获后未落账 —— front 截获未交 "
                                 "points.py 执行（D-10 执行链断裂）。")
                self.assertEqual(st["entries"][0]["balance_after"], 5,
                                 "K0PTS-A1 RED: 落账余额非权威重算值。")
                # 控制帧不透传：add_points 网关决定类绝不进设备控制路由（契约 §1.4）。
                ctrl = _control_frames(dev.sent_text)
                self.assertFalse(
                    [c for c in ctrl
                     if (c.get("data") or {}).get("action") == "add_points"],
                    f"K0PTS-A1 RED: control/add_points 被透传设备（{len(ctrl)} 帧 "
                    "control）—— 网关决定类不直接下发设备（契约 §1.4）；设备只消费"
                    "执行后的权威积分视图。")
                # 权威 points_home 恰一帧、字段逐项权威、restore 缺席（语音成功路径
                # 接管前台 = 设备主动跳积分页，D-07 例外半）。
                frames = _points_home_frames(dev.sent_text)
                self.assertEqual(
                    len(frames), 1,
                    f"K0PTS-A1 RED: 记账成功后 points_home 下发 {len(frames)} 帧"
                    "（应恰一帧）—— front 未按真实执行结果推送权威视图。")
                data = frames[0].get("data") or {}
                self.assertEqual(data.get("revision"), 1,
                                 "K0PTS-A1 RED: 推送 revision 非权威值。")
                self.assertEqual(data.get("balance"), 5,
                                 "K0PTS-A1 RED: 推送 balance 非权威重算值 —— 模型/ "
                                 "front 自算余额 = D-10 第二账本面。")
                self.assertEqual(data.get("target"), DEFAULT_TARGET_FROZEN,
                                 f"K0PTS-A1 RED: target 非出厂默认激活目标（应为 "
                                 f"{DEFAULT_TARGET_FROZEN!r}）—— S-F6.5 AC1 出厂默认"
                                 "激活恒在，设备进度线无目标即 K0S-E1 证据不可成立。")
                recent = data.get("recent")
                self.assertIsInstance(recent, list, "K0PTS-A1 RED: recent 非数组。")
                self.assertEqual(len(recent), 1,
                                 "K0PTS-A1 RED: recent 应恰含本笔。")
                self.assertEqual(recent[0].get("reason"), "今天扫地",
                                 "K0PTS-A1 RED: 推送 recent 事由丢失 —— 设备看不到"
                                 "「为什么得分」= K0S-E1「权威余额和原因」目击缺半。")
                self.assertEqual(recent[0].get("balance_after"), 5,
                                 "K0PTS-A1 RED: recent 分录 balance_after 权威值缺失。")
                self.assertNotIn("restore", data,
                                 "K0PTS-A1 RED: 语音成功路径推送带 restore —— 设备"
                                 "主动记账成功必须接管前台跳积分页（D-07 例外半；"
                                 "restore 只归 QQ/恢复通道）。")
                # 设备 wire 形状：view/data 双 String（mcp_server.cc 解析依据）。
                envs = _mcp_envelopes(dev.sent_text)
                args = (envs[0].get("params") or {}).get("arguments") or {}
                self.assertIsInstance(args.get("view"), str,
                                      "K0PTS-A1 RED: wire arguments.view 非 String。")
                self.assertIsInstance(args.get("data"), str,
                                      "K0PTS-A1 RED: wire arguments.data 非字符串"
                                      "（设备自解析 data 字符串，对象型被拒）——跨端 "
                                      "wire 形状必须对照对端实际接受什么（brief ②）。")


# =========================================================================== #
# K0PTS-A2 — 兑换门禁与拒绝路径（错误/边界面）                                    #
# =========================================================================== #


class K0ptsRedeemGateAcceptance(unittest.TestCase):
    """K0PTS-A2: redeem 只认激活目标分值、余额不足拒、拒绝不写账；畸形薄意图拒绝。"""

    maxDiff = None

    # coverage: K0PTS-A2 / 契约 §1.4 redeem_points 表行 + S-F6.3-AC3（server 半）
    def test_k0pts_a2_redeem_target_gate_and_refusal_paths(self):
        pts = _points_module()
        target_points = DEFAULT_TARGET_FROZEN["points"]
        with tempfile.TemporaryDirectory(prefix="k0pts_a2_") as tmp:
            state = str(Path(tmp) / "state.json")
            # 金额不匹配（目标 40 说兑 20）→ 拒绝且不写账。
            # 锐化（2026-11-05 变异实放 A2-2 后 planner 裁定）：空账下调 mismatch 会被
            # 余额门先拒、遮蔽「只认激活目标分值」门（删门变异仍 6 passed）。改在
            # 已充值状态（余额 50 ≥ 20）下调 mismatch——拒绝只能来自目标门，变异
            # 即红；用独立充账 state，主 state 保持空账供后续 case 零偏移。
            charged = str(Path(tmp) / "state_charged.json")
            pts.add_points(charged, 50, "帮家里大扫除", now=_T_EARLY)
            with self.assertRaises(ValueError,
                                   msg="K0PTS-A2 RED: 兑换金额 != 激活目标分值未拒绝 "
                                       "（S-F6.3-AC3「只认目标分值」被破）。"):
                pts.redeem_points(charged, target_points - 20, now=_T_EARLY)
            self.assertEqual(len(pts.read_state(charged)["entries"]), 1,
                             "K0PTS-A2 RED: 金额不匹配拒绝仍写账 —— 拒绝必须零落账"
                             "（在册仅充值那 1 笔）。")
            self.assertEqual(pts.read_state(charged)["balance"], 50,
                             "K0PTS-A2 RED: 金额不匹配拒绝动了余额。")
            # 余额不足（目标 40、余额 0）→ 拒绝且不写账。
            with self.assertRaises(ValueError,
                                   msg="K0PTS-A2 RED: 余额不足未拒绝 —— 兑换资格门"
                                       "（balance >= 激活目标分值）被破。"):
                pts.redeem_points(state, target_points, now=_T_EARLY)
            self.assertEqual(len(pts.read_state(state)["entries"]), 0,
                             "K0PTS-A2 RED: 余额不足拒绝仍写账。")
            # 成功路径：加 50 → 兑 40 → 负向分录、事由统一「兑换游乐场」、余额 10。
            pts.add_points(state, 50, "帮家里大扫除", now=_T_EARLY)
            entry = pts.redeem_points(state, target_points, now=_T_LATE)
            self.assertEqual(entry["delta"], -target_points,
                             "K0PTS-A2 RED: 兑换分录 delta 非负目标分值。")
            self.assertEqual(entry["reason"], "兑换游乐场",
                             "K0PTS-A2 RED: 兑换事由未统一取「兑换<目标名>」—— 模型"
                             "不得提供兑换名目（契约 §1.4 redeem_points 行）。")
            self.assertEqual(entry["balance_before"], 50, "K0PTS-A2 RED: balance_before 权威值缺失。")
            self.assertEqual(entry["balance_after"], 10, "K0PTS-A2 RED: balance_after 权威值缺失。")
            # 畸形入参（bool 不是正整数 / reason 空）→ 拒绝不写账。
            before = pts.read_state(state)
            with self.assertRaises(ValueError,
                                   msg="K0PTS-A2 RED: points=True（bool）未被拒绝 —— "
                                       "bool 是 int 子类，须显式排除。"):
                pts.add_points(state, True, "x", now=_T_LATE)
            with self.assertRaises(ValueError,
                                   msg="K0PTS-A2 RED: 缺事由（空 reason）未拒绝 "
                                       "（契约 §1.4：缺事由拒绝执行）。"):
                pts.add_points(state, 5, "  ", now=_T_LATE)
            with self.assertRaises(ValueError,
                                   msg="K0PTS-A2 RED: points=0 未拒绝（正整数门）。"):
                pts.add_points(state, 0, "x", now=_T_LATE)
            self.assertEqual(pts.read_state(state), before,
                             "K0PTS-A2 RED: 畸形入参拒绝路径动了账本。")

        # ---- 薄意图拒绝沿：畸形意图不落账、不推 points_home、控制帧不透传 --------
        with tempfile.TemporaryDirectory(prefix="k0pts_a2_wire_") as tmp:
            with _front_with_points(tmp) as front:
                sid = "sess-k0pts-a2"
                bad_block = json.dumps({"view": "control",
                                        "data": {"action": "add_points", "points": 0}},
                                       ensure_ascii=False)
                up, dev = _engine_to_device(
                    front, sid,
                    [{"type": "kidpal_display_blocks", "blocks": [bad_block]}])
                state_path = Path(tmp) / "state.json"
                self.assertFalse(
                    state_path.exists() and len(pts.read_state(str(state_path))["entries"]),
                    "K0PTS-A2 RED: 畸形薄意图（points=0）被落账 —— 拒绝执行义务被破。")
                self.assertEqual(_points_home_frames(dev.sent_text), [],
                                 "K0PTS-A2 RED: 拒绝路径仍推 points_home —— 设备会"
                                 "看到一次没有发生的记账（成功回执伪造面）。")
                self.assertFalse(
                    [c for c in _control_frames(dev.sent_text)
                     if (c.get("data") or {}).get("action") == "add_points"],
                    "K0PTS-A2 RED: 拒绝的薄意图仍以控制帧透传设备（网关决定类）。")


# =========================================================================== #
# K0PTS-A3 — 白名单 7→9 + 积分来源门禁 + prompt 薄意图纪律锚                       #
# =========================================================================== #


class K0ptsSchemaGateAcceptance(unittest.TestCase):
    """K0PTS-A3: display_schema 白名单/来源门禁真跑 + AGENTS.md 薄意图纪律锚。"""

    maxDiff = None

    # coverage: K0PTS-A3 / 契约 §1.5 白名单 7→9 + 积分来源门禁 + §1.6 prompt 纪律 +
    #           D-08-spike（display 面最小通路）
    def test_k0pts_a3_points_views_whitelist_source_gate_and_prompt_anchor(self):
        ds = _display_schema()
        # ---- 白名单 7→9（additive；既有 7 零偏移） ------------------------------
        views = getattr(ds, "VALID_VIEWS", None)
        self.assertIsInstance(views, (set, frozenset),
                              "K0PTS-A3 RED(infra): VALID_VIEWS 缺失。")
        for v in ("char", "schedule", "story_list", "story_text", "text", "home",
                  "control"):
            self.assertIn(v, views,
                          f"K0PTS-A3 RED: 既有 view {v!r} 从白名单消失 —— additive "
                          "纪律被破（7→9 只增不减）。")
        for v in ("points_home", "points_ledger"):
            self.assertIn(v, views,
                          f"K0PTS-A3 RED: 白名单未增 {v!r} —— 白名单 7→9 未落（契约 "
                          "§1.5，2026-11-04 LANDED）。")
        self.assertEqual(len(set(views)), 9,
                         f"K0PTS-A3 RED: 白名单基数 {len(set(views))} != 9。")

        # ---- 引擎侧完整 points 帧拒转（积分来源门禁） ----------------------------
        complete_home = {"view": "points_home",
                         "data": {"revision": 1, "balance": 5,
                                  "target": dict(DEFAULT_TARGET_FROZEN),
                                  "recent": []}}
        ok, reason = ds.validate_display(complete_home)
        self.assertFalse(
            ok,
            f"K0PTS-A3 RED: 引擎侧完整 points_home 通过了真门 ({reason}) —— 积分来源"
            "门禁缺失（完整 points 帧仅 front 自推可下发，契约 §1.5；引擎侧拒转与 "
            "story 单源同型）。")
        # 引擎侧空 data 导航薄载荷放行（front 展开为完整帧）。
        ok_thin, reason_thin = ds.validate_display({"view": "points_home", "data": {}})
        self.assertTrue(
            ok_thin,
            f"K0PTS-A3 RED: 引擎侧 points_home 空 data 薄载荷被拒 ({reason_thin}) —— "
            "「打开积分本」导航薄载荷须过门由 front 展开（契约 §1.5）。")
        ok_thin_l, _ = ds.validate_display({"view": "points_ledger", "data": {}})
        self.assertTrue(ok_thin_l,
                        "K0PTS-A3 RED: 引擎侧 points_ledger 空 data 薄载荷被拒。")
        # front 自推通路：完整帧放行（front 依据 points.py 单源推送）。
        ok_self, reason_self = ds.validate_display(complete_home, self_push=True)
        self.assertTrue(
            ok_self,
            f"K0PTS-A3 RED: 自推通路拒完整 points_home ({reason_self}) —— front 权威"
            "推送被自拒 = 权威视图永远发不出去。")
        # floor：restore 非布尔拒（畸形 restore 帧不入板盲渲染）。
        bad_restore = {"view": "points_home",
                       "data": {"revision": 1, "balance": 5,
                                "target": dict(DEFAULT_TARGET_FROZEN),
                                "recent": [], "restore": "yes"}}
        self.assertFalse(ds.validate_display(bad_restore, self_push=True)[0],
                         "K0PTS-A3 RED: restore 非布尔未拒 —— floor 缺失。")
        bad_balance = {"view": "points_home",
                       "data": {"revision": 1, "balance": "5",
                                "target": dict(DEFAULT_TARGET_FROZEN), "recent": []}}
        self.assertFalse(ds.validate_display(bad_balance, self_push=True)[0],
                         "K0PTS-A3 RED: balance 非整数未拒 —— 完整形状 floor 缺失。")
        bad_target = {"view": "points_home",
                      "data": {"revision": 1, "balance": 5, "target": {"name": "x"},
                               "recent": []}}
        self.assertFalse(ds.validate_display(bad_target, self_push=True)[0],
                         "K0PTS-A3 RED: target 缺四键未拒（契约 §1.3⑦ target 恒四键）。")
        # ---- 决定类登记：add_points/redeem_points 双路径恒拒 --------------------
        actions = getattr(ds, "GATEWAY_DECISION_CONTROL_ACTIONS", None)
        self.assertIsInstance(actions, (set, frozenset),
                              "K0PTS-A3 RED(infra): GATEWAY_DECISION_CONTROL_ACTIONS 缺失。")
        for a in ("add_points", "redeem_points"):
            self.assertIn(a, actions,
                          f"K0PTS-A3 RED: GATEWAY_DECISION_CONTROL_ACTIONS 不含 {a!r}"
                          " —— 契约 §1.4 网关决定类未登记。")
            self.assertNotIn(
                a, getattr(ds, "FORWARDABLE_CONTROL_ACTIONS", frozenset()),
                f"K0PTS-A3 RED: {a!r} 进了可透传集合 —— 决定类与可透传类必须不相交。")
            ok_relay, _ = ds.validate_display(
                {"view": "control", "data": {"action": a}})
            self.assertFalse(ok_relay,
                             f"K0PTS-A3 RED: 引擎侧 control/{a} 通过 relay 门。")
            ok_self, _ = ds.validate_display(
                {"view": "control", "data": {"action": a}}, self_push=True)
            self.assertFalse(
                ok_self,
                f"K0PTS-A3 RED: control/{a} 借 self_push 放行 —— 与 start_organizing "
                "不同，front 不把控制 points 帧发给任何一端（设备只消费权威积分视图）。")
        # 既有决定类零偏移（must-stay-green）。
        self.assertFalse(ds.validate_display(
            {"view": "control", "data": {"action": "end_session"}})[0],
            "K0PTS-A3 RED: end_session 拒转语义偏移（must-stay-green）。")

        # ---- relay 真跑：薄载荷展开为完整帧；完整帧降级不透传 --------------------
        pts = _points_module()
        with tempfile.TemporaryDirectory(prefix="k0pts_a3_relay_") as tmp:
            with _front_with_points(tmp) as front:
                pts.add_points(front.POINTS_STATE_PATH, 5, "今天扫地", now=_T_EARLY)
                # 薄载荷：截获展开（balance 读自 points.py 当前状态 = 权威单源）。
                calls = front.build_display_tool_calls(
                    [json.dumps({"view": "points_home", "data": {}},
                                ensure_ascii=False)])
                payloads = _relay_envelope_payloads(calls)
                self.assertEqual(len(payloads), 1, "K0PTS-A3 RED: 薄载荷展开帧数 != 1。")
                self.assertEqual(payloads[0].get("view"), "points_home",
                                 "K0PTS-A3 RED: 薄载荷未被展开为 points_home 完整帧"
                                 "（「打开积分本」导航不可达）。")
                self.assertEqual((payloads[0].get("data") or {}).get("balance"), 5,
                                 "K0PTS-A3 RED: 展开帧 balance 非权威值 —— 展开未走 "
                                 "points.py 单源（第二账本面）。")
                # 完整帧：引擎来源降级，不透传完整 points 帧。
                calls2 = front.build_display_tool_calls(
                    [json.dumps(complete_home, ensure_ascii=False)])
                payloads2 = _relay_envelope_payloads(calls2)
                self.assertEqual(len(payloads2), 1,
                                 "K0PTS-A3 RED: 完整 points 块中继数 != 1（ NEVER "
                                 "silently dropped 既有纪律）。")
                self.assertEqual(payloads2[0].get("view"), "text",
                                 f"K0PTS-A3 RED: 引擎侧完整 points_home 未降级（得到 "
                                 f"{payloads2[0].get('view')!r}）—— 积分来源门禁在 "
                                 "relay 沿缺失，完整帧透传设备 = 来源伪造面。")

        # ---- AGENTS.md 薄意图纪律锚（prompt 载荷；源锚代理） --------------------
        self.assertTrue(AGENTS_MD.is_file(),
                        f"K0PTS-A3 RED(infra): {AGENTS_MD} 不在世。")
        prompt = AGENTS_MD.read_text(encoding="utf-8", errors="replace")
        for needle in ("add_points", "redeem_points"):
            self.assertIn(needle, prompt,
                          f"K0PTS-A3 RED: workspace-kidpal AGENTS.md 缺 {needle!r} 薄"
                          "意图纪律 —— 模型不知道只产薄意图，会自行编造记账结果。")
        for forbidden in ("余额", "revision"):
            self.assertIn(forbidden, prompt,
                          f"K0PTS-A3 RED: AGENTS.md 缺「不得生成 {forbidden}」禁令锚 "
                          "—— 权威字段禁产纪律未进 prompt（D-10 的模型半）。")


# =========================================================================== #
# K0PTS-A4 — restore 推送不抢前台语义（数据面）                                   #
# =========================================================================== #


class K0ptsRestoreSemanticsAcceptance(unittest.TestCase):
    """K0PTS-A4: QQ/恢复通道 restore=true 只填缓存；语音成功通道 restore 缺席。"""

    maxDiff = None

    # coverage: K0PTS-A4 / S-F6.4-AC5（server 数据面半）+ D-07 + 契约 §1.3⑦ restore
    def test_k0pts_a4_restore_push_cache_only_semantics(self):
        front = _front()
        # ---- seam 在世：_consume_points_intent(websocket, payload, *, restore) ---
        seam = getattr(front, "_consume_points_intent", None)
        self.assertTrue(
            callable(seam),
            "K0PTS-A4 RED: front_app.py 无 _consume_points_intent —— 薄意图截获执行"
            "缝未落地（K0PTS-A1/A2 的 wire 红可能全部来自本缝缺席；设计态红）。")
        import inspect  # noqa: PLC0415
        sig = inspect.signature(seam)
        self.assertIn("restore", sig.parameters,
                      "K0PTS-A4 RED: _consume_points_intent 无 restore 形参 —— QQ/"
                      "恢复通道与语音成功通道的分流载体缺席（D-07）。")
        self.assertTrue(
            sig.parameters["restore"].kind is inspect.Parameter.KEYWORD_ONLY,
            "K0PTS-A4 RED: restore 非 keyword-only —— 位置参混用会破坏既有调用点零偏移"
            "纪律（RDG self_push 同款冻结形态）。")
        self.assertIs(sig.parameters["restore"].default, False,
                      "K0PTS-A4 RED: restore 缺省非 False —— 语音成功路径必须缺省不"
                      "带 restore（接管前台）。")

        with tempfile.TemporaryDirectory(prefix="k0pts_a4_") as tmp:
            with _front_with_points(tmp) as f:
                # QQ 通道（restore=True）：推送帧 data.restore is True。
                dev = _FakeDeviceSocket()
                payload = {"view": "control", "data": {"action": "add_points",
                                                       "points": 3,
                                                       "reason": "老师表扬"}}
                consumed = asyncio.run(
                    f._consume_points_intent(dev, payload, restore=True))
                self.assertTrue(consumed,
                                "K0PTS-A4 RED: add_points 薄意图未被消费（返回 False "
                                "= 会被当普通控制块中继）。")
                frames = _points_home_frames(dev.sent_text)
                self.assertEqual(len(frames), 1,
                                 f"K0PTS-A4 RED: restore 通道推送 {len(frames)} 帧"
                                 "（应恰一帧）。")
                self.assertIs((frames[0].get("data") or {}).get("restore"), True,
                              "K0PTS-A4 RED: QQ 通道推送 restore 非 True —— restore "
                              "更新会被当成主动展示抢前台（D-07 / S-F6.4-AC5 被破）。")
                self.assertEqual((frames[0].get("data") or {}).get("balance"), 3,
                                 "K0PTS-A4 RED: restore 通道推送余额非权威值。")
                self.assertFalse(
                    [c for c in _control_frames(dev.sent_text)
                     if (c.get("data") or {}).get("action") == "add_points"],
                    "K0PTS-A4 RED: restore 通道仍透传控制帧。")
                # 语音通道（restore 缺省 False）：推送帧 restore 缺席（对照半）。
                dev2 = _FakeDeviceSocket()
                asyncio.run(f._consume_points_intent(
                    dev2, {"view": "control",
                           "data": {"action": "add_points", "points": 2,
                                    "reason": "主动洗碗"}}))
                frames2 = _points_home_frames(dev2.sent_text)
                self.assertEqual(len(frames2), 1,
                                 "K0PTS-A4 RED: 语音通道未推权威帧。")
                self.assertNotIn("restore", frames2[0].get("data") or {},
                                 "K0PTS-A4 RED: 语音成功路径出现 restore —— 设备不会"
                                 "跳积分页（S-F6.3-AC2「自动跳转积分本页」缺半）。")

        # ---- 截获真接线锚：缝在 relay 函数体内被引用（防只可直调的空过缝） -------
        src = _front_source()
        self.assertIn("_consume_points_intent", src,
                      "K0PTS-A4 RED(infra): front_app.py 未引用截获缝（源码锚失真）。")
        relay_fn = getattr(front, "_relay_display_blocks_to_device", None)
        self.assertTrue(callable(relay_fn),
                        "K0PTS-A4 RED(infra): _relay_display_blocks_to_device 缺失"
                        "（既有 relay 面被改写？须复核锚不得改判为绿）。")
        relay_region = _source_region(src, "async def _relay_display_blocks_to_device",
                                      "async def pump_upstream_to_client")
        self.assertIn("_consume_points_intent", relay_region,
                      "K0PTS-A4 RED: 截获缝未被接线进 _relay_display_blocks_to_device "
                      "函数体 —— 只在直调时生效 = 引擎 display 流截获空过（A1 的 wire "
                      "红会掩盖接线缺失；builder 接线后本锚转绿）。")


# =========================================================================== #
# K0PTS-A5 — kidpal_points_page 首尾分页 canary                                   #
# =========================================================================== #


class K0ptsPaginationCanaryAcceptance(unittest.TestCase):
    """K0PTS-A5: keyset 分页首尾两条 canary 均可及 + reset/续页/越界/空账语义。"""

    maxDiff = None

    def _seed_canaries(self, pts, state):
        """两条同形 canary：最早 + 最新（token 随事由，供消费日志按 token 关联）。"""
        pts.add_points(state, 5, f"第一天扫地 {CANARY_EARLIEST_TOKEN}", now=_T_EARLY)
        pts.add_points(state, 3, f"帮忙洗碗 {CANARY_LATEST_TOKEN}", now=_T_LATE)

    # coverage: K0PTS-A5 / S-F6.2-AC1（全量可及离线半）+ 契约 §四 kidpal_points_page
    #           + K0S-E2 离线半 + D-08-spike
    def test_k0pts_a5_first_last_pagination_canary_via_kidpal_points_page(self):
        pts = _points_module()
        with tempfile.TemporaryDirectory(prefix="k0pts_a5_exec_") as tmp:
            state = str(Path(tmp) / "state.json")
            self._seed_canaries(pts, state)
            rev = pts.read_state(state)["revision"]
            self.assertEqual(rev, 2, "K0PTS-A5 RED(前置): 两笔 canary 后 revision 非 2。")
            # 首页（limit=1 步进）：最新 canary 在场、reset=True、无 restore 键。
            first = pts.build_points_ledger_page(state, limit=1)
            data = first.get("data") or {}
            page = data.get("page") or {}
            items = data.get("items") or []
            self.assertEqual(len(items), 1, "K0PTS-A5 RED: limit=1 首页 items 数 != 1。")
            self.assertIn(CANARY_LATEST_TOKEN, items[0].get("reason", ""),
                          f"K0PTS-A5 RED: 首页非最新 canary（{items[0].get('reason')!r}）"
                          " —— 时间倒序被破。")
            self.assertTrue(page.get("reset"),
                            "K0PTS-A5 RED: 首页 page.reset 非 True（契约 §四）。")
            self.assertTrue(page.get("has_more"), "K0PTS-A5 RED: 有续页却 has_more=False。")
            self.assertIsInstance(page.get("next_cursor"), str,
                                  "K0PTS-A5 RED: next_cursor 非字符串。")
            self.assertTrue(page["next_cursor"],
                            "K0PTS-A5 RED: 有续页却 next_cursor 为空。")
            self.assertNotIn("restore", data,
                             "K0PTS-A5 RED: 生产者层带 restore —— restore 归 front "
                             "按通道加（首页显式打开不加 restore）。")
            self.assertEqual(page.get("total"), 2,
                             "K0PTS-A5 RED: page.total 非 2 —— total 是全量分录数，"
                             "不能用 items.length 代替（契约 §1.3⑧）。")
            # 续页：最早 canary 在场、reset=False、到最早（has_more=False + cursor 空）。
            second = pts.build_points_ledger_page(state, cursor=page["next_cursor"],
                                                  known_revision=rev, limit=1)
            data2 = second.get("data") or {}
            page2 = data2.get("page") or {}
            items2 = data2.get("items") or []
            self.assertEqual(len(items2), 1, "K0PTS-A5 RED: 续页 items 数 != 1。")
            self.assertIn(CANARY_EARLIEST_TOKEN, items2[0].get("reason", ""),
                          f"K0PTS-A5 RED: 续页非最早 canary（{items2[0].get('reason')!r})"
                          " —— 首尾两条均可及 = K0S-E2 离线半被破（看到的不只是最近"
                          "一条）。")
            self.assertFalse(page2.get("reset"), "K0PTS-A5 RED: 正常续页 reset 非 False。")
            self.assertFalse(page2.get("has_more"), "K0PTS-A5 RED: 已到最早却 has_more=True。")
            self.assertIsNone(page2.get("next_cursor"),
                              "K0PTS-A5 RED: 已到最早 next_cursor 非空（契约 §四："
                              "null 且 has_more=false = 已到最早一页）。")
            # 分录形状：canary 分录在分页里保持六键统一形状（S-F6.2-AC2 形状承载）。
            self.assertTrue(_entry_keys_ok(items2[0]),
                            f"K0PTS-A5 RED: 分页分录形状非六键: {items2[0]!r}。")
            # 游标非法 → 最新首页 + reset=True。
            bad = pts.build_points_ledger_page(state, cursor="bogus-cursor",
                                               known_revision=rev, limit=1)
            bad_page = (bad.get("data") or {}).get("page") or {}
            self.assertTrue(bad_page.get("reset"),
                            "K0PTS-A5 RED: 非法游标未回首页 reset=True。")
            self.assertIn(CANARY_LATEST_TOKEN,
                          (bad.get("data") or {}).get("items", [{}])[0].get("reason", ""),
                          "K0PTS-A5 RED: 非法游标回退页非最新首页。")
            # known_revision 不符 → 同上（版本重置）。
            stale = pts.build_points_ledger_page(state, cursor=page["next_cursor"],
                                                 known_revision=rev - 1, limit=1)
            stale_page = (stale.get("data") or {}).get("page") or {}
            self.assertTrue(stale_page.get("reset"),
                            "K0PTS-A5 RED: known_revision 与当前 revision 不一致未重置"
                            " —— 设备会跨版本追加缓存（契约 §四明令禁止）。")
            # limit 越界拒绝。
            for bad_limit in (0, POINTS_PAGE_LIMIT_MAX + 1):
                with self.assertRaises(ValueError,
                                       msg=f"K0PTS-A5 RED: limit={bad_limit} 未拒绝"
                                           "（允许 1–50，契约 §四）。"):
                    pts.build_points_ledger_page(state, limit=bad_limit)

        # ---- 设备上行消费沿真跑：kidpal_points_page 拦截应答（不透传引擎） -------
        with tempfile.TemporaryDirectory(prefix="k0pts_a5_wire_") as tmp:
            with _front_with_points(tmp) as front:
                sid = "sess-k0pts-a5"
                self._seed_canaries(pts, str(Path(tmp) / "state.json"))
                rev = pts.read_state(str(Path(tmp) / "state.json"))["revision"]
                # 首页请求（cursor 缺省）→ 恰一帧 points_ledger、restore 缺席、reset。
                up, dev = _device_to_engine(front, sid, [
                    json.dumps({"type": "kidpal_points_page", "cursor": None,
                                "known_revision": rev,
                                "limit": POINTS_PAGE_LIMIT_DEFAULT},
                               ensure_ascii=False)])
                frames = _points_ledger_frames(dev.sent_text)
                self.assertEqual(len(frames), 1,
                                 f"K0PTS-A5 RED: 首页请求应答 {len(frames)} 帧（应恰"
                                 "一帧）—— front 未消费 kidpal_points_page 或未回帧。")
                data = frames[0].get("data") or {}
                self.assertNotIn("restore", data,
                                 "K0PTS-A5 RED: 首页显式打开应答带 restore —— 设备不"
                                 "进详细账本（契约 §四：首页应答不带 restore）。")
                self.assertTrue((data.get("page") or {}).get("reset"),
                                "K0PTS-A5 RED: 首页应答 reset 非 True。")
                self.assertIn(CANARY_LATEST_TOKEN,
                              " ".join(e.get("reason", "") for e in data.get("items", [])),
                              "K0PTS-A5 RED: 首页应答缺最新 canary。")
                # 设备上行帧不透传引擎（整条消费并拦截）。
                self.assertEqual(_forwarded_types(up.sent), [],
                                 "K0PTS-A5 RED: kidpal_points_page 被透传引擎 —— "
                                 "front 整条消费义务被破（契约 §四）。")
                # 续页请求 → restore:true、reset=False、含最早 canary。
                next_cursor = (data.get("page") or {}).get("next_cursor")
                self.assertTrue(next_cursor, "K0PTS-A5 RED(前置): 首页应答未给 next_cursor。")
                up2, dev2 = _device_to_engine(front, sid, [
                    json.dumps({"type": "kidpal_points_page", "cursor": next_cursor,
                                "known_revision": rev,
                                "limit": POINTS_PAGE_LIMIT_DEFAULT},
                               ensure_ascii=False)])
                frames2 = _points_ledger_frames(dev2.sent_text)
                self.assertEqual(len(frames2), 1, "K0PTS-A5 RED: 续页应答帧数 != 1。")
                data2 = frames2[0].get("data") or {}
                self.assertIs(data2.get("restore"), True,
                              "K0PTS-A5 RED: 续页应答 restore 非 true —— 续页是后台"
                              "取数，抢前台会打断孩子浏览（契约 §四）。")
                self.assertFalse((data2.get("page") or {}).get("reset"),
                                 "K0PTS-A5 RED: 正常续页应答 reset 非 False。")
                self.assertIn(CANARY_EARLIEST_TOKEN,
                              " ".join(e.get("reason", "") for e in data2.get("items", [])),
                              "K0PTS-A5 RED: 续页应答缺最早 canary —— 消费端（设备上行"
                              "→front 应答→设备 wire）首尾均可及的离线闭环被破。")
                # limit 越界上行 → 拒绝记日志、不回帧。
                up3, dev3 = _device_to_engine(front, sid, [
                    json.dumps({"type": "kidpal_points_page", "cursor": None,
                                "limit": POINTS_PAGE_LIMIT_MAX + 1},
                               ensure_ascii=False)])
                self.assertEqual(_points_ledger_frames(dev3.sent_text), [],
                                 "K0PTS-A5 RED: limit 越界上行仍回帧（契约 §四：拒绝"
                                 "并记日志、不回帧）。")
                # 空账：权威 items=[]，不伪造分录。
                with tempfile.TemporaryDirectory(prefix="k0pts_a5_empty_") as tmp2:
                    with _front_with_points(tmp2) as front2:
                        up4, dev4 = _device_to_engine(front2, "sess-k0pts-a5-empty", [
                            json.dumps({"type": "kidpal_points_page"}, ensure_ascii=False)])
                        frames4 = _points_ledger_frames(dev4.sent_text)
                        self.assertEqual(len(frames4), 1,
                                         "K0PTS-A5 RED: 空账首页请求未回帧。")
                        data4 = frames4[0].get("data") or {}
                        self.assertEqual(data4.get("items"), [],
                                         "K0PTS-A5 RED: 空账 items 非权威空数组。")
                        self.assertEqual((data4.get("page") or {}).get("total"), 0,
                                         "K0PTS-A5 RED: 空账 total 非 0。")
                        self.assertEqual(data4.get("balance"), 0,
                                         "K0PTS-A5 RED: 空账 balance 非权威 0。")


def _forwarded_types(sent):
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


# =========================================================================== #
# K0PTS-A6 — QQ 最小 add CLI 单源（门 1 裁定执行器面）                             #
# =========================================================================== #


class K0ptsQqCliAcceptance(unittest.TestCase):
    """K0PTS-A6: `python -m gateway.points` 与 front 共享同一执行器与同一 state 文件。"""

    maxDiff = None

    def _run_cli(self, args, state):
        env = dict(os.environ)
        env["KIDPAL_POINTS_STATE"] = state
        env["PYTHONPATH"] = str(SERVER)
        return subprocess.run(
            [sys.executable, "-m", "gateway.points", *args],
            cwd=str(SERVER), env=env, capture_output=True, text=True, timeout=60)

    # coverage: K0PTS-A6 / S-F6.4-AC1（最小通路 server 半）+ 门 1 裁定（最小 add CLI
    #           单源）+ D-10（无第二账本）
    def test_k0pts_a6_qq_minimal_add_cli_single_writer(self):
        pts = _points_module()
        self.assertTrue(callable(getattr(pts, "main", None)),
                        "K0PTS-A6 RED: points.py 无 main CLI 入口 —— 门 1 裁定的最小 "
                        "add CLI 执行器面缺席（真实 QQ producer 无落点）。")
        with tempfile.TemporaryDirectory(prefix="k0pts_a6_") as tmp:
            state = str(Path(tmp) / "state.json")
            # add：exit 0 + 落账。
            r = self._run_cli(["add", "3", "canary-qq-praise"], state)
            self.assertEqual(r.returncode, 0,
                             f"K0PTS-A6 RED: CLI add 非零退出 rc={r.returncode} "
                             f"stderr={r.stderr!r} —— QQ 最小写入通路断裂。")
            st = pts.read_state(state)
            self.assertEqual(len(st["entries"]), 1, "K0PTS-A6 RED: CLI add 未落账。")
            self.assertEqual(st["entries"][0]["delta"], 3,
                             "K0PTS-A6 RED: CLI 落账 delta 非 3。")
            self.assertEqual(st["balance"], 3, "K0PTS-A6 RED: CLI 落账后余额非 3。")
            # 单源：front 权威载荷读到 CLI 写入的同一 state（无第二账本）。
            home = pts.build_points_home(state)
            self.assertEqual((home.get("data") or {}).get("balance"), 3,
                             "K0PTS-A6 RED: build_points_home 读不到 CLI 写入 —— "
                             "CLI 与执行器双账本 = D-10 被破（K3 正式通道的地基）。")
            self.assertIn("canary-qq-praise",
                          " ".join(e.get("reason", "") for e in
                                   (home.get("data") or {}).get("recent", [])),
                          "K0PTS-A6 RED: 权威 recent 缺 CLI canary 分录。")
            # redeem：加 50 后兑 40 成功。
            self.assertEqual(self._run_cli(["add", "50", "big-praise"], state).returncode, 0,
                             "K0PTS-A6 RED: CLI add 50 失败。")
            r2 = self._run_cli(["redeem", str(DEFAULT_TARGET_FROZEN["points"])], state)
            self.assertEqual(r2.returncode, 0,
                             f"K0PTS-A6 RED: CLI redeem 目标分值非零退出 rc={r2.returncode} "
                             f"stderr={r2.stderr!r}。")
            st2 = pts.read_state(state)
            self.assertEqual(st2["balance"], 13, "K0PTS-A6 RED: CLI 兑换后余额非 13。")
            # 非法输入：非零退出 + 不写账 + 不伪造成功回执。
            before = pts.read_state(state)
            r3 = self._run_cli(["add", "abc", "oops"], state)
            self.assertNotEqual(r3.returncode, 0,
                                "K0PTS-A6 RED: CLI 非整数分值 exit 0 —— 拒绝面缺失。")
            r4 = self._run_cli(["redeem", "20"], state)
            self.assertNotEqual(r4.returncode, 0,
                                "K0PTS-A6 RED: CLI 兑换金额不匹配 exit 0（只认激活"
                                "目标分值）—— QQ 家长会看到假成功。")
            self.assertEqual(pts.read_state(state), before,
                             "K0PTS-A6 RED: CLI 拒绝路径动了账本。")
            self.assertTrue(
                (r3.stderr + r3.stdout + r4.stderr + r4.stdout).strip(),
                "K0PTS-A6 RED: CLI 拒绝零输出 —— 家长收不到一次性可判的说明"
                "（S-F6.4-AC4 硬约束的执行器半）。")


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
