"""Frozen acceptance tests — TaskPackage 261104-k1-points-ledger-core（server 面）。

承载 story = S-F6.2（详细账本与删除误记，AC1 数据半 / AC4 删除重算与负余额）+
S-F6.3（语音记账与兑换，AC3 / AC4 / AC5 / AC6 数据半）+ S-F6.5（奖励场景与激活目标
配置，AC1 / AC2 / AC4 存储半）+ S-F6.1-AC5（负余额显示的数据面半）+ NFR-OFF-1
（离线边界数据面半）+ docs/architecture.md §5.1.2 / §5.2 场景⑥ / §5.3.2 / §5.4 /
§6.5（两类幂等与分页一致性）+ docs/contracts/interface-contract.md §1.3⑦⑧ / §1.4 /
§四 kidpal_points_page·kidpal_points_delete + ADR-018 + PROPOSAL
delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md D-03 / D-04 / D-06 /
D-08 / D-10 与 OQ-1 / OQ-2 / OQ-3（human 裁定 2026-11-03 回灌）。
本包把 K0 最小路径（server/acceptance/test_points_e2e_spike.acceptance.py，14 断言）
正式化为唯一确定性 writer——K0 冻结件必须保持全绿（must-stay-green），本文件不重复
钉 K0 已钉语义，只钉 K1 新增不变量与 producer 侧。

Scope of record（taskpackage exitCriteria K1L-E1..E6 → 验收 case K1L-A1..A10）:
  K1L-A1 单 writer 串行与原子写：进程内并发零丢账 + 跨进程并发零丢账（.lock 文件锁）
        + 持久化失败无半写（旧状态完整、异常上抛、恢复后无幽灵补写）。
  K1L-A2 余额与逐条前后余额全部由剩余分录派生：balance 副本不被信任（篡改副本仍按
        分录重算）、entries 恒时间倒序、负和不钳位、损坏分录拒绝不当空账。
  K1L-A3 删除重算与负余额（OQ-1）：删除按时间序重算全部前后余额（可负）、其余分录
        保留、重复/不存在 id 幂等 no-op、非法 id 拒绝；负余额帧不被 display floor 钳位。
  K1L-A4 兑换资格与真实兑现：只认当前激活目标分值（动态）、余额不足拒（含负余额）、
        兑现即落负向分录、拒绝零落账。
  K1L-A5 传输幂等（command_id）：同 id 重放返首次结果、不新增分录不递增 revision、
        回执与状态同临界区原子持久；权威字段不进执行器入参（D-10 形状面）。
  K1L-A6 短窗语义去重（注入时钟，起步 10 分钟）：同正负分值 + 保守归一化事由命中
        不新增分录、返「已记过」回执 + 当前权威余额；窗口外同文案合法新命令；已删除
        分录不参与匹配；与 command_id 无关且传输幂等检查先于语义去重；兑换侧同规则。
  K1L-A7 keyset 分页 producer：>16 条全量可及（不复用 story_list 16 条窗口）、默认
        limit=6、禁 offset、插入/删除致 revision 变化后续页重置回最新首页、非法游标
        回首页、空账权威 items=[]。
  K1L-A8 目标配置存储：四预置 + 出厂默认激活游乐场 40、自定义 5–200 正整数（bool /
        float / str / 出界拒）、预置不可删、激活中自定义禁删先切换（OQ-2）、切目标
        revision +1 不产生分录（不自动扣分）、历史兑换名目快照不改写、兑换跟随新
        激活目标分值。
  K1L-A9 GAP-1 producer 侧：front 观察缝（_observe_points_revision）——外部写入
        （QQ CLI 同一执行器路径）致 revision 变化 → 权威 points_home restore 帧；
        删除另推 points_ledger 首页 restore 帧（page.reset=true、无 target 键）；
        无变化零帧；state 损坏不抛不推不补写、恢复后按当前权威状态观察。
  K1L-A10 失败路径与离线边界：持久化失败不推成功视图、拒绝/失败零写入、失败后无
        本地写队列补写（NFR-OFF-1 / D-06 数据面半）。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — points.py 在同一文件上正式化
（K0 语义零放宽、K0 冻结件 must-stay-green），front_app.py 补 GAP-1 producer 观察缝。

  A) server/gateway/points.py（K1 新增；K0 既有常量与函数语义不变）:

    DEDUP_WINDOW_SECONDS = 600（模块级 int 常量；短窗起步 10 分钟，S-F6.3-AC5；
        真机调优定数后由后续包回填，本包冻结起步值）。

    PRESET_TARGETS（模块级常量；四预置，requirements S-F6.5-AC1 / arch §5.3.2
        「预置四场景为实现内不可删常量」；id 命名沿契约 §1.3⑦ 示例
        "preset-playground" 的 preset-* 形态扩展冻结）:
        {"id": "preset-choco",     "name": "巧克力",   "points": 20, "asset": "sc-choco"}
        {"id": "preset-movie",     "name": "看电影",   "points": 30, "asset": "sc-movie"}
        {"id": "preset-playground","name": "游乐场",   "points": 40, "asset": "sc-wheel"}
        {"id": "preset-gift",      "name": "礼物",     "points": 50, "asset": "sc-gift"}
        DEFAULT_TARGET 常量保留不变（K0 冻结件依赖；出厂激活 = preset-playground 40）。

    add_points(path, points, reason, *, now=None, command_id=None) 与
    redeem_points(path, points, *, now=None, command_id=None)
        新增 keyword-only command_id（可选，缺省 None = 无传输幂等语义，K0 兼容）：
        - 同一 command_id 重放（跨调用、跨进程重启——回执持久化于 state）在同一锁
          临界区查回执 → 返回首次结果（result["id"] == 首次分录 id），不新增分录、
          不递增 revision（arch §6.5-1）；
        - 命令回执与分录同临界区原子持久（回执不得落后于分录）；
        - 传输幂等检查先于短窗语义去重。

    短窗语义去重（arch §6.5-2；与 command_id 无关，executor 级、QQ 与语音同规则）:
        - 窗口 = DEDUP_WINDOW_SECONDS，以注入 now 判定（禁 wall-clock 依赖验收）；
        - 匹配 = 相同正负号分值 + 保守归一化事由相等（去空白/标点、全半角统一、
          不改时间前缀；不引入模型相似分或模糊阈值）；
        - 匹配对象 = 当前账本现存分录（已删除分录不参与匹配——D-03 分录为事实；
          本行为为 D-04 的冻结解释，planner Step 8b 核对点）；
        - 命中 → 不新增分录、不递增 revision，返回去重回执 dict：
          {"duplicate": True, "balance": <当前权威余额>}（额外键允许）；
        - 未命中 → 正常落账，返回六键分录（K0 冻结形状不变）；
        - redeem 同规则（同激活目标分值 + 事由「兑换<目标名>」天然同形）。

    delete_entry(path, entry_id) -> dict（契约 §四 kidpal_points_delete 执行器面）:
        - 形状合法且存在的 id → 移除该分录，按时间序重算全部 balance_before /
          balance_after 与余额（允许负余额，OQ-1），revision +1，返回回执 dict
          （至少含 "id" / "deleted"=True / "revision" / "balance"）；
        - 形状合法但不存在的 id → 幂等 no-op：不写账、revision 不变，回执
          "deleted"=False（front 据此仍重推对账帧）；
        - 非法 id（非 str / 空串 / 纯空白）→ raise ValueError（拒绝并审计）。

    目标配置存储（S-F6.5 AC1/AC2/AC4 存储半；OQ-2/OQ-3）:
        get_active_target(path) -> dict
            当前激活目标（id/name/points/asset 四键）；出厂态（无 state 文件）=
            preset-playground 40。
        list_targets(path) -> list
            预置四项恒在 + 自定义目标；元素为含 id/name/points/asset 的 dict。
        add_custom_target(path, name, points, *, now=None) -> dict
            name 非空 str；points 为 5–200 正整数（bool / float / str 拒，OQ-3）；
            id 由执行器生成（模型不产）、asset="sc-flag"（通用标记图）；持久化、
            revision +1；越界/畸形 raise ValueError 不写账。
        set_active_target(path, target_id, *, now=None) -> dict
            切激活目标：revision +1、不产生任何分录（切目标不自动扣分）；未知 id
            raise ValueError；返回新激活目标 dict。
        delete_target(path, target_id, *, now=None)
            预置 id → raise ValueError（预置不可删）；当前激活目标（含激活中自定义）
            → raise ValueError（OQ-2：先切换再删）；非激活自定义 → 删除、revision +1；
            未知 id raise ValueError。
        兑换资格门 = 当前激活目标分值（动态；出厂态 == DEFAULT_TARGET["points"]，
        K0PTS-A2 冻结语义在出厂态下零偏移）；历史兑换分录 reason 快照不因切目标改写。

    进程间锁与原子写（arch §5.1.2 / §5.3.2 / §6.5）:
        写入临界区以 state 同目录 `.lock` 文件锁跨进程互斥；原子写保持 tmp+
        os.replace；持久化失败 → 异常上抛、磁盘旧状态完整（无半写、无成功回执）。

  B) server/gateway/front/front_app.py（GAP-1 producer 侧观察缝）:

    async def _observe_points_revision(websocket) -> bool
        - 观察态按连接（websocket 对象粒度）跟踪最近一次观察到的
          (revision, entry_count)；未被观察过的新 websocket → 只建立基线、推 0 帧、
          返回 False（冷启动初推见 GAP-K1-2，不在本缝）；
        - revision 变化且分录数未减（QQ 加分/兑换/切目标等外部写入）→ 经
          _push_display_payload 恰推一帧权威 points_home，data.restore=True，
          revision/balance 为当前权威值；返回 True；
        - revision 变化且分录数减少（删除）→ 在 points_home 之外另推恰一帧权威
          points_ledger 首页：data.restore=True、page.reset=True、无 target 键
          （K0 ruling：points_ledger 载荷无 target 字段）；
        - 无变化 → 推 0 帧、返回 False；
        - state 不可读/损坏 → 不抛异常、推 0 帧、返回 False、不改写 state 文件
          （失败可见于返回值，不伪造、不本地补写，NFR-OFF-1 数据半）。
        框架/循环侧的真实接线（谁、以什么周期调用本缝）= GAP-K1-1，不落离线断言。

  C) server/gateway/display_schema.py（floor 收紧红线）:
      不得引入非负钳位——balance / balance_before / balance_after 整数即可（可负），
      负值权威帧不得被拒（S-F6.1-AC5 数据面半）；非整数仍拒。

Shape authority = docs/contracts/interface-contract.md §1.3⑦⑧ / §1.4 add_points·
redeem_points / §1.5 积分来源门禁 / §四 kidpal_points_page·kidpal_points_delete（均
2026-11-04 LANDED，只读）+ docs/requirements.md v1.71 S-F6.1/S-F6.2/S-F6.3/S-F6.5 /
NFR-OFF-1 + docs/architecture.md §5.1.2/§5.2 场景⑥/§5.3.2/§5.4/§6.5 + ADR-018。

契约条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 验收断言」节）:

| 契约 / 决策条目 | 承载断言 | 性质档 |
|---|---|---|
| D-10 单 writer（front/QQ 共用执行器） | test_k1l_a1（进程内 + 跨进程并发零丢账真跑）+ K0PTS-A6 单源回归 | 行为级（跨进程半概率型，机制锚 = .lock） |
| arch §6.5-1 传输幂等（同 command_id 重放） | test_k1l_a5（重放返首次结果、零新增、revision 不变真跑） | 行为级 |
| D-04 / arch §6.5-2 短窗语义去重 | test_k1l_a6（注入时钟 599s/601s 边界、归一化、窗口外合法真跑） | 行为级 |
| D-03 / OQ-1 删除重算允许负余额 | test_k1l_a3（删除重算链 −40 真跑）+ test_k1l_a2（负和不钳位） | 行为级 |
| S-F6.3-AC3 兑换只认激活目标分值 | test_k1l_a4（动态目标门 + 拒绝零落账） | 行为级 |
| S-F6.5-AC1 四预置 + 出厂默认游乐场 40 | test_k1l_a8（list_targets 逐项值 + get_active_target） | 行为级 |
| S-F6.5-AC2 / OQ-3 自定义 5–200 | test_k1l_a8（5/200 收、4/201/bool/float/str 拒） | 行为级 |
| S-F6.5-AC4 / OQ-2 预置不可删、激活禁删 | test_k1l_a8（双拒绝 + 先切换后删成功） | 行为级 |
| arch §6.5-3 / 契约 §四 分页一致性 | test_k1l_a7（>16 全量、revision 重置、禁 offset、空账） | 行为级 |
| 契约 §四 kidpal_points_delete 幂等/拒绝 | test_k1l_a3（no-op 回执 + 非法 id ValueError） | 行为级 |
| GAP-1 / arch §5.2⑥B producer 侧 restore | test_k1l_a9（观察缝真跑：add→home restore、delete→+ledger 首页） | 行为级（缝级；活性接线归 GAP-K1-1） |
| NFR-OFF-1 / D-06 断网不本地补写（数据半） | test_k1l_a10（失败零写入、无幽灵补写）+ test_k1l_a9（损坏 state 不补写） | 行为级（设备半归 K2/live） |
| S-F6.1-AC5 负余额显示（数据半） | test_k1l_a3 内 display floor 负值放行锚 | 行为级 |
| 既有 display_schema / K0 冻结件行为 | 各 test 内 must-stay-green 半 | 行为级 |

弱于契约处（显式缺口，不得当已兑现）:
  GAP-K1-1 观察缝的活性接线（真实连接生命周期里谁/何时调 _observe_points_revision
        ——轮询 tick 或事件钩子）离线不可驱动；本文件只钉缝行为与帧形状；真实 QQ
        写入 → 设备原地刷新全链归 live-manual / K2 消费半。
  GAP-K1-2 WS 建连冷启动 restore 初推（arch §5.2⑥C）不在本文件断言面（K1L-E5 范围
        = revision 变化推送；冷启动帧形状由 K0PTS-A4/A5 restore 语义承载、初推时机
        归 K2/live 复核）。
  GAP-K1-3 「已记过」话术 / TTS 播报 / 删除确认 UI = K2 设备半；本包只钉回执与帧形状。
  GAP-K1-4 CLI 的 delete / target 子命令与 QQ skill 接线归 K3/K4；本包钉执行器 API
        面（main 既有 add/redeem 语义由 K0PTS-A6 承载，零放宽）。
  GAP-K1-5 跨进程竞争断言本质概率型（无锁时高概率红而非必然红）；确定性机制锚 =
        `.lock` 文件在位 + 变异（去锁）重放复核。
  GAP-K1-6 NFR-PERF-4 软时延只记录不写阈值（沿 K0 GAP-3；requirements 明示待真机
        调优定数）。
  GAP-K1-7 命令回执的持久化内部形状（state 内 receipts 结构）为实现自由，本文件只钉
        重放行为，不锁内部布局。

行为等价假设（显式；不写即默认不成立）:
  EQ-1 执行器 API 直调 = front/QQ 通道真实执行路径（front 经 gw_points.* 源锚由
        K0PTS-A1/A4 钉、CLI 由 K0PTS-A6 钉；本文件沿同一假设）。
  EQ-2 FakeDeviceSocket + _push_display_payload 下发帧 = 设备真实收帧形状（K0 EQ-1
        同口径：kidpal/display mcp 信封 + view/data 双 String wire）。
  EQ-3 tmp 目录 state 文件 = /opt/kidpal/points/state.json 同一文件语义（读写路径、
        原子替换、.lock 同目录布局）。
  EQ-4 「front 按 websocket 对象跟踪观察态」= 真实每连接独立观察（真实 WebSocket
        对象同样可作跟踪键；跨进程/多 worker 部署形态归部署面，不在离线断言面）。

Coverage map (acceptanceRef -> test):
  K1L-A1 -> test_k1l_a1_single_writer_serial_atomic_and_cross_process_lock
  K1L-A2 -> test_k1l_a2_balance_and_running_balances_derived_from_entries
  K1L-A3 -> test_k1l_a3_delete_recompute_negative_balance_and_idempotent_noop
  K1L-A4 -> test_k1l_a4_redeem_active_target_gate_and_real_redemption
  K1L-A5 -> test_k1l_a5_transport_idempotency_command_id_replay
  K1L-A6 -> test_k1l_a6_short_window_semantic_dedup_injected_clock
  K1L-A7 -> test_k1l_a7_keyset_pagination_full_traversal_and_revision_reset
  K1L-A8 -> test_k1l_a8_target_config_storage_presets_custom_bounds_and_guards
  K1L-A9 -> test_k1l_a9_gap1_producer_side_restore_observation
  K1L-A10 -> test_k1l_a10_failure_paths_offline_boundary_no_local_queue
  coverage: K1L-L1 —— live-manual 真实 QQ 写入 → 设备积分页原地刷新目击（载体
        device-round-evidence.md / verifier-evidence，不落自动断言）。
  coverage: K1L-L2 —— live-manual 观察缝活性接线目击（GAP-K1-1；同上不落自动断言）。
        两项均不落自动断言、不计入本文件通过面；离线测试不冒充真机终验（K1L-E6）。

exitCriteria 映射：K1L-E1 → A1/A2/A3/A4；K1L-E2 → A5/A6；K1L-E3 → A7；K1L-E4 → A8；
K1L-E5 → A9/A10（单源执行器回归半由 K0PTS-A6 承载）；K1L-E6 → verifier diff 面核对
+ 本文件头 GAP-K1-1..7 与 K1L-L1/L2 申报。

Run protocol（offline；-m "not live" 可跑；文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib server/acceptance/test_points_ledger_core.acceptance.py

REVISION LOG
  2026-11-05 初版冻结（acceptance-author；K1L-A1..A10 server 离线层；K1L-L1/L2
             live-manual 层在 taskpackage acceptance[] 单列不落本文件。设计态红 =
             delete_entry / command_id / 短窗去重 / 目标配置存储 / .lock 进程间锁 /
             front _observe_points_revision 缺席；A2/A7 及各 test 内 must-stay-green
             半在 K0 现状上即可绿，属回归守卫非空过——其变异点见
             taskpackage mutationEvidence.perAcceptance）。
  2026-11-05 重冻（acceptance-author；planner 逐处对源码独立核验后指定的四处冻结件
             内部缺陷最小修正，binding refs 与断言行为意图零变化，非语义放宽）：
             ①A4 兑换拒绝路径 revision 断言由写死 2 改为「拒绝前后 revision 相等」
             （原漏计 set_active_target 的 +1，与本文件 FROZEN 契约 A 段自相矛盾）；
             ②_assert_chain_contiguous 第二循环由 balance_after 升序「承载分录」查找
             改为 (occurred_at,id) 时间序前驱检查 + 首条 before==0（原 ±1 步长假设
             只对并发用例成立，A4/A6 步长账本误红）；③A9 损坏 state 字节断言期望由
             污染前 good_bytes 改为污染后现抓 corrupt_bytes（原断言照字面要求观察缝
             修复文件，与 NFR-OFF-1 消息相反）；④A9 恢复后权威余额 7→2（+5 已删、
             余额 0 后加 2）。
             sha256 核销：3d92482474732cf414a189a86b06b57cc3442fe65f0e1f73c2aadd70456053a8
             → 修正后重算值见交付报告。重冻后基线 = 实现面在盘全绿（本文件 10/10
             passed），原设计态红基线记录由实现面在盘取代。
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import inspect
import json
import os
import subprocess
import sys
import tempfile
import threading
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

# 出厂默认激活目标（requirements S-F6.5 AC1；points.py DEFAULT_TARGET 冻结值，K0 沿用）。
DEFAULT_TARGET_FROZEN = {
    "id": "preset-playground",
    "name": "游乐场",
    "points": 40,
    "asset": "sc-wheel",
}

# 四预置目标（requirements S-F6.5-AC1；id 命名为本包冻结扩展，见文件头 FROZEN 契约）。
PRESET_TARGETS_FROZEN = (
    {"id": "preset-choco", "name": "巧克力", "points": 20, "asset": "sc-choco"},
    {"id": "preset-movie", "name": "看电影", "points": 30, "asset": "sc-movie"},
    {"id": "preset-playground", "name": "游乐场", "points": 40, "asset": "sc-wheel"},
    {"id": "preset-gift", "name": "礼物", "points": 50, "asset": "sc-gift"},
)
PRESET_IDS_FROZEN = {t["id"] for t in PRESET_TARGETS_FROZEN}

# 短窗去重起步值（S-F6.3-AC5：起步 10 分钟）与分页 wire 常量（契约 §四）。
DEDUP_WINDOW_SECONDS_FROZEN = 600
POINTS_PAGE_LIMIT_DEFAULT = 6
POINTS_PAGE_LIMIT_MAX = 50
# 自定义目标分值边界（OQ-3：5–200 正整数，含端点）。
CUSTOM_POINTS_MIN = 5
CUSTOM_POINTS_MAX = 200

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
    """server/gateway/points.py 纯模块。缺失 = 设计态红（K1 正式化未落）。"""
    global _points_module_cache
    if _points_module_cache is not None:
        return _points_module_cache
    if not POINTS_PY.is_file():
        raise AssertionError(
            "K1L RED: server/gateway/points.py 不在世 —— 单 writer 正式化"
            "（D-10）无承载文件。")
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.points as pts  # noqa: PLC0415 — offline import, cached
    _points_module_cache = pts
    return pts


def _points_attr(name: str):
    """取 points.py 模块级符号；缺席 = K1L 设计态红（frozen seam 未实现）。"""
    pts = _points_module()
    val = getattr(pts, name, None)
    if val is None:
        raise AssertionError(
            f"K1L RED: points.py 无模块级 {name} —— frozen K1L offline contract "
            "（seam 名冻结）未实现。")
    return val


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（K0 / M4K1 / RDG 冻结件同款先例）。"""
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
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k1l", FRONT_APP)
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
        raise AssertionError("K1L RED: " + err)
    return mod


def _display_schema():
    """真实冻结机械门 gateway.display_schema。"""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415
    return ds


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


# --------------------------------------------------------------------------- #
# I/O doubles（K0 冻结 seam 同款形态：只替代 WebSocket 设备端）                    #
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


def _occurred_epoch(value) -> float:
    """occurred_at -> epoch 秒（链检查用；解析失败保守 0.0 退化为按 id 排序）。"""
    try:
        dt = datetime.fromisoformat(str(value).replace("Z", "+00:00"))
    except (TypeError, ValueError):
        return 0.0
    if dt.tzinfo is None:
        dt = dt.replace(tzinfo=dt_timezone.utc)
    return dt.timestamp()


def _assert_chain_contiguous(test, entries):
    """逐条前后余额链连续：entries 按 (occurred_at, id) 时间序排好后，首条
    balance_before == 0、其后每条 balance_before == 上一条 balance_after。
    （并发/重算后的链完整性判据，对任意步长成立；第一循环另核
    before == after − delta 逐条自洽。）"""
    for entry in entries:
        test.assertEqual(
            entry["balance_before"], entry["balance_after"] - entry["delta"],
            f"K1L RED: 分录前后余额与 delta 不自洽: {entry!r} —— balance_before/"
            "balance_after 必须由剩余分录派生（D-03），不得独立存储 stale 值。")
    ordered = sorted(
        entries,
        key=lambda e: (_occurred_epoch(e.get("occurred_at")), str(e.get("id") or "")))
    test.assertEqual(
        ordered[0]["balance_before"], 0,
        f"K1L RED: 时间序首分录 balance_before 非 0: {ordered[0]!r} —— 链必须从"
        "零余额起点派生（D-03）。")
    for prev, cur in zip(ordered, ordered[1:]):
        test.assertEqual(
            cur["balance_before"], prev["balance_after"],
            f"K1L RED: 前后余额链断裂（{prev['balance_after']} -> "
            f"{cur['balance_before']}）—— 逐条前后余额必须按时间序连续派生"
            "（S-F6.2-AC2 数据半）。")


# =========================================================================== #
# K1L-A1 — 单 writer 串行与原子写（进程内 + 跨进程 + .lock + 无半写）              #
# =========================================================================== #


class K1lSingleWriterAtomicityAcceptance(unittest.TestCase):
    """K1L-A1: 并发零丢账、跨进程文件锁、持久化失败无半写、恢复无幽灵补写。"""

    maxDiff = None

    # coverage: K1L-A1 / D-10 + K1L-E1 + arch §5.1.2 / §5.3.2 / §6.5
    def test_k1l_a1_single_writer_serial_atomic_and_cross_process_lock(self):
        pts = _points_module()
        with tempfile.TemporaryDirectory(prefix="k1l_a1_threads_") as tmp:
            state = str(Path(tmp) / "state.json")
            # ---- 进程内并发：8 线程同时加分，零丢账、revision 恰 8、链连续 -------
            barrier = threading.Barrier(8)

            def _worker(i):
                barrier.wait()
                pts.add_points(state, 1, f"并发-{i}", now=_t(i * 2))

            threads = [threading.Thread(target=_worker, args=(i,)) for i in range(8)]
            for th in threads:
                th.start()
            for th in threads:
                th.join()
            st = pts.read_state(state)
            self.assertEqual(len(st["entries"]), 8,
                             f"K1L RED: 8 线程并发写入后分录数 {len(st['entries'])} != 8"
                             " —— 进程内串行化丢失更新（单 writer 临界区被破）。")
            self.assertEqual(st["revision"], 8,
                             "K1L RED: 并发写入后 revision != 8 —— 每次成功写必须 +1。")
            self.assertEqual(st["balance"], 8, "K1L RED: 并发写入后余额 != 8。")
            _assert_chain_contiguous(self, st["entries"])
            # ---- 进程间锁机制锚：state 同目录 .lock 文件在位（arch §5.3.2）------
            self.assertTrue(
                (Path(tmp) / ".lock").is_file(),
                "K1L RED: state 同目录无 .lock —— 进程间锁文件（arch §5.3.2 / §6.5）"
                "未落；front API 与 QQ CLI 双进程只有进程内锁 = 双写者覆盖面。")

        with tempfile.TemporaryDirectory(prefix="k1l_a1_cross_") as tmp:
            state = str(Path(tmp) / "state.json")
            # ---- 跨进程并发：10 个独立进程同时 CLI 同一执行器，零丢账 ------------
            code = (
                "import sys\n"
                "from gateway.points import add_points\n"
                "add_points(sys.argv[1], 1, 'proc-' + sys.argv[2])\n")
            env = dict(os.environ)
            env["PYTHONPATH"] = str(SERVER)
            procs = [
                subprocess.Popen(
                    [sys.executable, "-c", code, state, str(i)],
                    cwd=str(SERVER), env=env,
                    stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
                for i in range(10)]
            for p in procs:
                _, err = p.communicate(timeout=120)
                self.assertEqual(
                    p.returncode, 0,
                    f"K1L RED: 跨进程写入子进程 rc={p.returncode} stderr={err!r} —— "
                    "并发写不得以失败告终（须由 .lock 串行化全部成功）。")
            st = pts.read_state(state)
            self.assertEqual(len(st["entries"]), 10,
                             f"K1L RED: 10 进程并发写入后分录数 {len(st['entries'])} != 10"
                             " —— 跨进程写丢失更新（read-modify-write 无文件锁互斥，"
                             "D-10 单 writer 被破；概率型红见 GAP-K1-5）。")
            self.assertEqual(st["revision"], 10,
                             "K1L RED: 跨进程并发后 revision != 10。")
            self.assertEqual(st["balance"], 10, "K1L RED: 跨进程并发后余额 != 10。")
            _assert_chain_contiguous(self, st["entries"])

        with tempfile.TemporaryDirectory(prefix="k1l_a1_atomic_") as tmp:
            state = str(Path(tmp) / "state.json")
            pts.add_points(state, 5, "首笔", now=_T0)
            before = pts.read_state(state)
            # ---- 持久化失败：os.replace 失败 -> 异常上抛、磁盘旧状态完整 ----------
            real_replace = os.replace

            def _boom_replace(*args, **kwargs):
                raise OSError("simulated persist failure")

            os.replace = _boom_replace
            try:
                with self.assertRaises(
                        (OSError, ValueError),
                        msg="K1L RED: 持久化失败（os.replace 异常）被静默吞掉 —— "
                            "失败必须上抛，不得伪造成功回执。"):
                    pts.add_points(state, 9, "boom", now=_t(60))
            finally:
                os.replace = real_replace
            after = pts.read_state(state)
            self.assertEqual(after, before,
                             "K1L RED: 持久化失败后磁盘状态被改动 —— 原子写必须保证"
                             "失败无半写（tmp+os.replace 旧状态完整）。")
            # ---- 恢复后无幽灵补写：成功一次只多真实一笔 --------------------------
            pts.add_points(state, 3, "恢复后", now=_t(120))
            st = pts.read_state(state)
            self.assertEqual(len(st["entries"]), 2,
                             "K1L RED: 持久化失败恢复后出现幽灵分录 —— 失败尝试不得"
                             "遗留任何待补写队列/暂存（NFR-OFF-1 数据面半）。")
            self.assertEqual(st["balance"], 8, "K1L RED: 恢复后余额非 5+3=8。")
            _assert_chain_contiguous(self, st["entries"])


# =========================================================================== #
# K1L-A2 — 余额与逐条前后余额全部由剩余分录派生                                    #
# =========================================================================== #


class K1lBalanceDerivationAcceptance(unittest.TestCase):
    """K1L-A2: balance 副本不被信任、entries 恒时间倒序、负和不钳位、坏分录拒。"""

    maxDiff = None

    # coverage: K1L-A2 / D-03 + S-F6.2-AC1/AC2（数据半）+ K1L-E1
    def test_k1l_a2_balance_and_running_balances_derived_from_entries(self):
        pts = _points_module()
        with tempfile.TemporaryDirectory(prefix="k1l_a2_") as tmp:
            state = str(Path(tmp) / "state.json")
            e1 = pts.add_points(state, 3, "帮老师收作业", now=_t(0))
            e2 = pts.add_points(state, 2, "主动洗碗", now=_t(60))
            self.assertTrue(_entry_keys_ok(e1) and _entry_keys_ok(e2),
                            "K1L RED: 正常落账分录偏离六键冻结形状。")
            # balance 恒 == sum(delta)，磁盘上的 balance 副本不被信任。
            raw = json.loads(Path(state).read_text(encoding="utf-8"))
            raw["balance"] = 999
            raw["entries"] = [raw["entries"][1], raw["entries"][0]]  # 故意乱序
            Path(state).write_text(
                json.dumps(raw, ensure_ascii=False), encoding="utf-8")
            st = pts.read_state(state)
            self.assertEqual(st["balance"], 5,
                             "K1L RED: 篡改 balance 副本后被读出 —— 余额必须由分录"
                             "重算（D-03），磁盘副本不是权威。")
            self.assertEqual(st["revision"], 2,
                             "K1L RED: read_state 改动了 revision。")
            self.assertEqual([e["id"] for e in st["entries"]], [e2["id"], e1["id"]],
                             "K1L RED: entries 未按 (occurred_at, id) 时间倒序重排"
                             "（契约 §1.3⑦⑧ 排序纪律）。")
            _assert_chain_contiguous(self, st["entries"])
            # 负和不钳位：+5 与 −40 共存 -> 余额 −35 如实派生（OQ-1 数据面）。
            neg_state = str(Path(tmp) / "state_neg.json")
            pts.add_points(neg_state, 5, "small", now=_t(0))
            # 直接构造一条 −40 兑换形分录不可经 redeem（余额不足会拒）——
            # 用文件级构造验证 read_state 派生本身不钳位（兑换真路径在 A3/A4）。
            raw_neg = json.loads(Path(neg_state).read_text(encoding="utf-8"))
            raw_neg["entries"].append({
                "id": "pt_test_neg40", "delta": -40, "reason": "兑换游乐场",
                "occurred_at": _t(3600).isoformat(),
                "balance_before": 5, "balance_after": -35})
            Path(neg_state).write_text(
                json.dumps(raw_neg, ensure_ascii=False), encoding="utf-8")
            st_neg = pts.read_state(neg_state)
            self.assertEqual(st_neg["balance"], -35,
                             "K1L RED: 负余额被非负假设钳位 —— OQ-1 数据面（余额"
                             "如实为剩余分录之和，可负）。")
            # 损坏分录（多余键 / 缺键）-> ValueError，绝不静默当空账覆盖真账本。
            corrupt = str(Path(tmp) / "state_corrupt.json")
            pts.add_points(corrupt, 7, "good", now=_t(0))
            raw_c = json.loads(Path(corrupt).read_text(encoding="utf-8"))
            raw_c["entries"][0]["extra"] = "bogus"
            Path(corrupt).write_text(
                json.dumps(raw_c, ensure_ascii=False), encoding="utf-8")
            with self.assertRaises(
                    ValueError,
                    msg="K1L RED: 形状违约分录未被拒绝 —— 静默当空账会覆盖真账本"
                        "（数据丢失面）。"):
                pts.read_state(corrupt)


# =========================================================================== #
# K1L-A3 — 删除重算、负余额（OQ-1）、幂等 no-op 与非法 id                           #
# =========================================================================== #


class K1lDeleteRecomputeAcceptance(unittest.TestCase):
    """K1L-A3: delete_entry 重算全部前后余额（可负）、保留其余分录、幂等/拒绝面。"""

    maxDiff = None

    # coverage: K1L-A3 / S-F6.2-AC4 + D-03/OQ-1 + 契约 §四 kidpal_points_delete
    #           + S-F6.1-AC5（数据面半）+ K1L-E1
    def test_k1l_a3_delete_recompute_negative_balance_and_idempotent_noop(self):
        pts = _points_module()
        delete_entry = _points_attr("delete_entry")
        with tempfile.TemporaryDirectory(prefix="k1l_a3_") as tmp:
            state = str(Path(tmp) / "state.json")
            plus = pts.add_points(state, 50, "误记的大扫除", now=_t(0))
            pts.redeem_points(state, 40, now=_t(60))   # 50 -> 10
            st_before = pts.read_state(state)
            self.assertEqual(st_before["balance"], 10, "K1L RED(前置): 删除前余额非 10。")
            # ---- 删除那笔 +50：余额重算为 −40，兑换分录前后余额按时间序重算 -----
            receipt = delete_entry(state, plus["id"])
            self.assertIsInstance(receipt, dict, "K1L RED: delete_entry 回执非 dict。")
            self.assertIs(receipt.get("deleted"), True,
                          "K1L RED: 删除回执 deleted 非 True。")
            self.assertEqual(receipt.get("id"), plus["id"],
                             "K1L RED: 删除回执 id 与被删分录不符。")
            self.assertEqual(receipt.get("revision"), 3,
                             "K1L RED: 删除后 revision 非 3（2 笔写 + 1 次删除）。")
            self.assertEqual(receipt.get("balance"), -40,
                             "K1L RED: 删除重算回执余额非 −40 —— OQ-1（删除重算"
                             "允许负余额）被钳位或算错。")
            st = pts.read_state(state)
            self.assertEqual(st["balance"], -40,
                             "K1L RED: 删除后持久余额非 −40（加 50 -> 兑 40 -> 删"
                             "+50，OQ-1 例子原样）。")
            self.assertEqual(len(st["entries"]), 1,
                             "K1L RED: 删除后剩余分录数 != 1 —— 其余分录必须保留"
                             "不清零（S-F6.2 补充规则）。")
            redeem_entry = st["entries"][0]
            self.assertEqual(redeem_entry["reason"], "兑换游乐场",
                             "K1L RED: 剩余兑换分录丢失。")
            self.assertEqual(redeem_entry["balance_before"], 0,
                             "K1L RED: 删除重算后兑换分录 balance_before 非 0 —— "
                             "逐条前后余额必须按剩余分录时间序全量重算（D-03）。")
            self.assertEqual(redeem_entry["balance_after"], -40,
                             "K1L RED: 删除重算后兑换分录 balance_after 非 −40。")
            # ---- 删除后再加分：新分录从负余额继续派生链 -------------------------
            pts.add_points(state, 5, "恢复性加分", now=_t(120))
            st2 = pts.read_state(state)
            self.assertEqual(st2["balance"], -35,
                             "K1L RED: 负余额上继续加分未按 −40+5 派生。")
            _assert_chain_contiguous(self, st2["entries"])
            # ---- 重复/已不存在 id = 幂等 no-op（契约 §四）-----------------------
            before_noop = pts.read_state(state)
            receipt_noop = delete_entry(state, plus["id"])  # 已删除
            self.assertIs(receipt_noop.get("deleted"), False,
                          "K1L RED: 重复删除已不存在 id 未返 deleted=False no-op。")
            receipt_missing = delete_entry(state, "pt_never_existed")
            self.assertIs(receipt_missing.get("deleted"), False,
                          "K1L RED: 删除不存在 id 未返 no-op 回执。")
            self.assertEqual(pts.read_state(state), before_noop,
                             "K1L RED: 幂等 no-op 删除改动了账本/revision（契约 §四"
                             "：重复删除不新增变化，front 仍重推对账由 no-op 回执承载）。")
            # ---- 非法 id 拒绝（契约 §四：非法 id 拒绝并审计）--------------------
            for bad in ("", "   ", None, 123):
                with self.assertRaises(
                        ValueError,
                        msg=f"K1L RED: 非法 entry_id={bad!r} 未拒绝（契约 §四）。"):
                    delete_entry(state, bad)
            self.assertEqual(pts.read_state(state), before_noop,
                             "K1L RED: 非法 id 拒绝路径动了账本。")
            # ---- display floor 不钳位负值（S-F6.1-AC5 数据面半）-----------------
            ds = _display_schema()
            neg_home = {"view": "points_home",
                        "data": {"revision": 3, "balance": -40,
                                 "target": dict(DEFAULT_TARGET_FROZEN),
                                 "recent": [dict(redeem_entry)]}}
            ok, reason = ds.validate_display(neg_home, self_push=True)
            self.assertTrue(
                ok,
                f"K1L RED: 负余额权威 points_home 帧被 display floor 拒 ({reason}) "
                "—— 非负假设钳位被破（S-F6.1-AC5：负数如实可见，front 不另做钳位）。")
            neg_ledger = {"view": "points_ledger",
                          "data": {"revision": 3, "balance": -40,
                                   "items": [dict(redeem_entry)],
                                   "page": {"total": 1, "next_cursor": None,
                                            "has_more": False, "reset": True}}}
            ok2, reason2 = ds.validate_display(neg_ledger, self_push=True)
            self.assertTrue(
                ok2,
                f"K1L RED: 负前后余额分录的 points_ledger 帧被拒 ({reason2})。")
            bad_type = {"view": "points_home",
                        "data": {"revision": 3, "balance": "-40",
                                 "target": dict(DEFAULT_TARGET_FROZEN), "recent": []}}
            self.assertFalse(
                ds.validate_display(bad_type, self_push=True)[0],
                "K1L RED: balance 非整数（字符串）未被 floor 拒。")


# =========================================================================== #
# K1L-A4 — 兑换资格（动态激活目标）、真实兑现、删除后约束                            #
# =========================================================================== #


class K1lRedeemQualificationAcceptance(unittest.TestCase):
    """K1L-A4: 只认当前激活目标分值、余额不足拒、兑现落负向分录、拒绝零落账。"""

    maxDiff = None

    # coverage: K1L-A4 / S-F6.3-AC3 + S-F6.3-AC4（数据半）+ D-03/OQ-1 + K1L-E1
    def test_k1l_a4_redeem_active_target_gate_and_real_redemption(self):
        pts = _points_module()
        set_active_target = _points_attr("set_active_target")
        with tempfile.TemporaryDirectory(prefix="k1l_a4_") as tmp:
            state = str(Path(tmp) / "state.json")
            # 空账兑换 -> 拒绝且零落账（资格门 = 余额 >= 激活目标分值）。
            with self.assertRaises(ValueError,
                                   msg="K1L RED: 空账兑换未拒绝（S-F6.3-AC3）。"):
                pts.redeem_points(state, 40, now=_T0)
            self.assertEqual(pts.read_state(state)["entries"], [],
                             "K1L RED: 空账兑换拒绝仍写账。")
            # 真实兑现：加 63 -> 兑 40（契约 §1.3⑦ 示例 63−40=23）。
            pts.add_points(state, 63, "期中进步奖", now=_t(0))
            entry = pts.redeem_points(state, 40, now=_t(3600))
            self.assertEqual(entry["delta"], -40, "K1L RED: 兑换 delta 非 −40。")
            self.assertEqual(entry["reason"], "兑换游乐场",
                             "K1L RED: 兑换事由非「兑换<激活目标名>」快照。")
            self.assertEqual(entry["balance_before"], 63, "K1L RED: 前余额非 63。")
            self.assertEqual(entry["balance_after"], 23, "K1L RED: 后余额非 23。")
            # 金额不匹配（余额 23 足够 20 但 20 != 激活目标 40）-> 拒。
            rev_before_mismatch = pts.read_state(state)["revision"]
            with self.assertRaises(ValueError,
                                   msg="K1L RED: 兑换金额 != 激活目标分值未拒绝。"):
                pts.redeem_points(state, 20, now=_t(4000))
            self.assertEqual(
                pts.read_state(state)["revision"], rev_before_mismatch,
                "K1L RED: 金额不匹配拒绝递增了 revision —— 拒绝路径零落账零计数。")
            # 余额不足激活目标分值（23 < 40 场景已覆盖；再钉 23 < 50）。
            set_active_target(state, "preset-gift", now=_t(4100))
            rev_before_short = pts.read_state(state)["revision"]
            with self.assertRaises(ValueError,
                                   msg="K1L RED: 余额 23 < 激活目标 50 未拒绝。"):
                pts.redeem_points(state, 50, now=_t(4200))
            self.assertEqual(
                pts.read_state(state)["revision"], rev_before_short,
                "K1L RED: 余额不足拒绝递增了 revision —— 拒绝路径零落账零计数"
                "（切目标自身的 revision+1 属配置写，已计入 rev_before_short）。")
            # 动态目标门：激活切到礼物 50 后，兑 40 拒、兑 50 成。
            pts.add_points(state, 60, "期末进步奖", now=_t(5000))   # 23+60=83
            with self.assertRaises(ValueError,
                                   msg="K1L RED: 激活目标切 50 后兑 40 未拒绝 —— "
                                       "资格门必须跟随当前激活目标（S-F6.5-AC3）。"):
                pts.redeem_points(state, 40, now=_t(5100))
            entry2 = pts.redeem_points(state, 50, now=_t(5200))
            self.assertEqual(entry2["delta"], -50, "K1L RED: 新目标兑换 delta 非 −50。")
            self.assertEqual(entry2["reason"], "兑换礼物",
                             "K1L RED: 新目标兑换事由未取「兑换礼物」。")
            self.assertEqual((entry2["balance_before"], entry2["balance_after"]),
                             (83, 33), "K1L RED: 新目标兑换前后余额错。")
            st = pts.read_state(state)
            self.assertEqual(st["balance"], 33, "K1L RED: 兑换后余额非 33。")
            _assert_chain_contiguous(self, st["entries"])


# =========================================================================== #
# K1L-A5 — 传输幂等（command_id 重放）与 D-10 形状面                                #
# =========================================================================== #


class K1lTransportIdempotencyAcceptance(unittest.TestCase):
    """K1L-A5: 同 command_id 重放返首次结果、零新增、revision 不变；权威字段不入参。"""

    maxDiff = None

    # coverage: K1L-A5 / arch §6.5-1 + D-10 + K1L-E2
    def test_k1l_a5_transport_idempotency_command_id_replay(self):
        pts = _points_module()
        # ---- D-10 形状面：执行器入参不暴露任何权威字段 ------------------------
        for fn_name in ("add_points", "redeem_points"):
            sig = inspect.signature(getattr(pts, fn_name))
            for forbidden in ("balance", "revision", "delta", "entry_id",
                              "cursor", "target_id"):
                self.assertNotIn(
                    forbidden, sig.parameters,
                    f"K1L RED: {fn_name} 入参暴露权威字段 {forbidden!r} —— 模型可"
                    "产权威值 = D-10 双写者面。")
            self.assertIn(
                "command_id", sig.parameters,
                f"K1L RED: {fn_name} 无 command_id 形参 —— 传输幂等载体缺席"
                "（arch §6.5-1）。")
            self.assertIs(
                sig.parameters["command_id"].kind, inspect.Parameter.KEYWORD_ONLY,
                f"K1L RED: {fn_name} 的 command_id 非 keyword-only（K0 位置参调用"
                "零偏移纪律）。")
            self.assertIsNone(
                sig.parameters["command_id"].default,
                f"K1L RED: {fn_name} 的 command_id 缺省非 None —— K0 兼容要求缺省"
                "无传输幂等语义。")
        sig_del = inspect.signature(_points_attr("delete_entry"))
        for forbidden in ("balance", "revision"):
            self.assertNotIn(forbidden, sig_del.parameters,
                             f"K1L RED: delete_entry 入参暴露权威字段 {forbidden!r}。")

        with tempfile.TemporaryDirectory(prefix="k1l_a5_add_") as tmp:
            state = str(Path(tmp) / "state.json")
            first = pts.add_points(state, 5, "今天扫地", now=_t(0),
                                   command_id="cmd-add-1")
            # ---- 同 command_id 重放（更晚时刻、跨「重启」——state 每次落盘重读）--
            replay = pts.add_points(state, 5, "今天扫地", now=_t(30),
                                    command_id="cmd-add-1")
            self.assertEqual(replay.get("id"), first["id"],
                             "K1L RED: 同 command_id 重放未返首次结果（arch §6.5-1"
                             "：查询并保存命令回执，重放返首次）。")
            self.assertEqual(replay.get("delta"), first["delta"],
                             "K1L RED: 重放回执 delta 与首次不符。")
            st = pts.read_state(state)
            self.assertEqual(len(st["entries"]), 1,
                             "K1L RED: 重放新增了分录 —— 传输幂等被破（同一逻辑命令"
                             "的重试不得重复扣/记）。")
            self.assertEqual(st["revision"], 1,
                             "K1L RED: 重放递增了 revision（arch §6.5-1 明令禁止）。")
            # ---- 回执跨重启持久：删内存态后重放仍命中（state 每调用重读已覆盖，
            #      显式断言防实现把回执只存进程内 dict）。
            self.assertEqual(
                pts.add_points(state, 5, "今天扫地", now=_t(300),
                               command_id="cmd-add-1").get("id"),
                first["id"],
                "K1L RED: 跨调用（模拟进程重启后的重放）未命中持久回执 —— 回执必须"
                "与状态同临界区原子持久，不得只存进程内存。")

        with tempfile.TemporaryDirectory(prefix="k1l_a5_redeem_") as tmp:
            state = str(Path(tmp) / "state.json")
            pts.add_points(state, 50, "充值", now=_t(0), command_id="cmd-top")
            r1 = pts.redeem_points(state, 40, now=_t(60), command_id="cmd-red-1")
            r2 = pts.redeem_points(state, 40, now=_t(90), command_id="cmd-red-1")
            self.assertEqual(r2.get("id"), r1["id"],
                             "K1L RED: 兑换同 command_id 重放未返首次结果 —— 重复"
                             "扣款面（同一次兑换绝不重复扣）。")
            st = pts.read_state(state)
            self.assertEqual(len(st["entries"]), 2, "K1L RED: 兑换重放新增分录。")
            self.assertEqual(st["revision"], 2, "K1L RED: 兑换重放递增 revision。")
            self.assertEqual(st["balance"], 10, "K1L RED: 兑换重放动了余额。")


# =========================================================================== #
# K1L-A6 — 短窗语义去重（注入时钟、起步 10 分钟、窗口外合法新命令）                  #
# =========================================================================== #


class K1lShortWindowDedupAcceptance(unittest.TestCase):
    """K1L-A6: 注入时钟 599s/601s 边界、保守归一化、已删除不参与、传输幂等先行。"""

    maxDiff = None

    # coverage: K1L-A6 / D-04 + S-F6.3-AC5 + arch §6.5-2 + K1L-E2
    def test_k1l_a6_short_window_semantic_dedup_injected_clock(self):
        pts = _points_module()
        # 起步值冻结（S-F6.3-AC5：起步 10 分钟；调优定数后续包回填）。
        self.assertEqual(
            getattr(pts, "DEDUP_WINDOW_SECONDS", None), DEDUP_WINDOW_SECONDS_FROZEN,
            "K1L RED: DEDUP_WINDOW_SECONDS 缺失或 != 600 —— 短窗起步 10 分钟未冻结。")

        def _dup_ok(receipt):
            return (isinstance(receipt, dict)
                    and receipt.get("duplicate") is True
                    and isinstance(receipt.get("balance"), int)
                    and not isinstance(receipt.get("balance"), bool))

        with tempfile.TemporaryDirectory(prefix="k1l_a6_add_") as tmp:
            state = str(Path(tmp) / "state.json")
            e1 = pts.add_points(state, 5, "今天扫地", now=_t(0), command_id="c1")
            # ---- 窗口内同分值同事由（标点/空白归一化）-> 去重回执 ----------------
            d1 = pts.add_points(state, 5, "今天扫地。", now=_t(60), command_id="c2")
            self.assertTrue(
                _dup_ok(d1),
                f"K1L RED: 窗口内重复表达未返去重回执: {d1!r} —— 短窗语义去重缺席"
                "（S-F6.3-AC5：不记第二条、回「这条刚才记过啦」+ 权威余额）。")
            self.assertEqual(d1["balance"], 5,
                             "K1L RED: 去重回执未携带当前权威余额（arch §6.5-2）。")
            d2 = pts.add_points(state, 5, " 今天  扫地 ！！ ", now=_t(120))
            self.assertTrue(_dup_ok(d2),
                            "K1L RED: 空白/标点差异未按保守归一化命中去重。")
            st = pts.read_state(state)
            self.assertEqual(len(st["entries"]), 1,
                             "K1L RED: 去重命中仍新增分录 —— 重复记账双记面。")
            self.assertEqual(st["revision"], 1,
                             "K1L RED: 去重命中递增了 revision（无事实变化不得计数）。")
            # ---- 全半角统一归一化 ------------------------------------------------
            e2 = pts.add_points(state, 5, "ABC扫地", now=_t(180))
            d3 = pts.add_points(state, 5, "ＡＢＣ扫地", now=_t(240))
            self.assertTrue(_dup_ok(d3),
                            "K1L RED: 全角/半角差异未统一归一化（arch §6.5-2）。")
            # ---- 不同事由 / 不同分值 = 合法新命令 --------------------------------
            e3 = pts.add_points(state, 5, "帮忙洗碗", now=_t(300))
            e4 = pts.add_points(state, 3, "今天扫地", now=_t(360))
            self.assertTrue(
                _entry_keys_ok(e3) and _entry_keys_ok(e4)
                and e3["id"] != e1["id"] and e4["id"] != e1["id"],
                "K1L RED: 不同事由或不同分值被误去重 —— 匹配必须同时要求相同正负"
                "分值与归一化事由相等（arch §6.5-2），不得扩大窗口。")
            # ---- 窗口外同文案 = 合法新命令（601s > 600s）--------------------------
            e5 = pts.add_points(state, 5, "今天扫地", now=_t(601))
            self.assertTrue(_entry_keys_ok(e5),
                            "K1L RED: 窗口外（601s）同文案被拒/去重 —— 短窗去重不得"
                            "变成永久去重（窗口外同文案合法新命令）。")
            # ---- 窗口边缘（对 e3 年龄 599s）仍去重 --------------------------------
            d4 = pts.add_points(state, 5, "帮忙洗碗", now=_t(899))
            self.assertTrue(_dup_ok(d4), "K1L RED: 窗口内边缘（599s）未去重。")
            st = pts.read_state(state)
            self.assertEqual([e["id"] for e in st["entries"]],
                             [e5["id"], e4["id"], e3["id"], e2["id"], e1["id"]],
                             "K1L RED: 分录集合与时间倒序不符（去重/合法新增有误）。")
            # ---- 传输幂等先于语义去重：旧 command_id 重放返首次结果 ---------------
            replay = pts.add_points(state, 5, "今天扫地", now=_t(400),
                                    command_id="c1")
            self.assertEqual(
                replay.get("id"), e1["id"],
                "K1L RED: 同 command_id 重放被语义去重劫持 —— 传输幂等检查必须先于"
                "短窗去重（arch §6.5：重放返首次结果，不是「已记过」）。")
            self.assertFalse(
                bool(replay.get("duplicate")),
                "K1L RED: 传输重放回执带 duplicate 标记 —— 与首次结果形状不符。")
            # ---- 已删除分录不参与匹配（D-03 冻结解释，见文件头）-------------------
            pts_delete = _points_attr("delete_entry")
            pts_delete(state, e3["id"])   # 删掉「帮忙洗碗」
            e6 = pts.add_points(state, 5, "帮忙洗碗", now=_t(320))
            self.assertTrue(
                _entry_keys_ok(e6),
                "K1L RED: 已删除分录仍参与短窗匹配 —— 分录为事实（D-03）：删除后"
                "同文案是合法新记账，不得借去重日志阴魂不散。")

        with tempfile.TemporaryDirectory(prefix="k1l_a6_redeem_") as tmp:
            state = str(Path(tmp) / "state.json")
            # ---- 兑换侧同规则：同一次兑换不重复扣分（S-F6.3-AC5）-----------------
            pts.add_points(state, 50, "充值", now=_t(0))
            r1 = pts.redeem_points(state, 40, now=_t(10), command_id="r1")
            r2 = pts.redeem_points(state, 40, now=_t(20), command_id="r2")
            self.assertTrue(
                _dup_ok(r2),
                f"K1L RED: 窗口内重复兑换未去重: {r2!r} —— 同一次兑换不重复扣分"
                "（S-F6.3-AC5 兑换侧同此）。")
            st = pts.read_state(state)
            self.assertEqual(len(st["entries"]), 2,
                             "K1L RED: 兑换去重命中仍新增负向分录（重复扣款）。")
            self.assertEqual(st["balance"], 10, "K1L RED: 兑换去重动了余额。")
            self.assertEqual(st["revision"], 2, "K1L RED: 兑换去重递增 revision。")
            # ---- 兑换窗口外（601s）合法再兑：充值后再兑一次 ----------------------
            pts.add_points(state, 50, "再充值", now=_t(30))
            r3 = pts.redeem_points(state, 40, now=_t(700), command_id="r3")
            self.assertTrue(
                _entry_keys_ok(r3),
                "K1L RED: 兑换窗口外（601s）再兑被误去重 —— 短窗不是永久锁。")
            st2 = pts.read_state(state)
            self.assertEqual(st2["balance"], 20,
                             "K1L RED: 兑换窗口外再兑后余额非 20。")
            _assert_chain_contiguous(self, st2["entries"])


# =========================================================================== #
# K1L-A7 — keyset 分页 producer（>16 全量、revision 重置、禁 offset、空账）         #
# =========================================================================== #


class K1lKeysetPaginationAcceptance(unittest.TestCase):
    """K1L-A7: 20 条全量可及、默认 limit 6、插入/删除后重置回首页、非法游标、空账。"""

    maxDiff = None

    # coverage: K1L-A7 / D-08 + S-F6.2-AC1（全量可及数据半）+ 契约 §四 + K1L-E3
    def test_k1l_a7_keyset_pagination_full_traversal_and_revision_reset(self):
        pts = _points_module()
        # 禁 offset（机制级锚；keyset 唯一定位法，arch §6.5-3 / ADR-018）。
        sig = inspect.signature(pts.build_points_ledger_page)
        self.assertNotIn(
            "offset", sig.parameters,
            "K1L RED: build_points_ledger_page 暴露 offset 形参 —— keyset 分页禁 "
            "offset（ADR-018）。")
        self.assertEqual(
            getattr(pts, "PAGE_LIMIT_DEFAULT", None), POINTS_PAGE_LIMIT_DEFAULT,
            "K1L RED: 分页默认 limit 非 6（契约 §四：缺省 6、允许 1–50）。")

        with tempfile.TemporaryDirectory(prefix="k1l_a7_full_") as tmp:
            state = str(Path(tmp) / "state.json")
            # ---- 20 条（> story_list 16 条窗口）全量可及 -------------------------
            for i in range(20):
                pts.add_points(state, 1, f"full-{i:02d}", now=_t(i * 3600))
            rev = pts.read_state(state)["revision"]
            collected = []
            cursor = None
            page_sizes = []
            while True:
                frame = pts.build_points_ledger_page(
                    state, cursor=cursor, known_revision=rev)
                data = frame["data"]
                page = data["page"]
                page_sizes.append(len(data["items"]))
                collected.extend(e["reason"] for e in data["items"])
                self.assertEqual(page["total"], 20,
                                 "K1L RED: page.total 非全量分录数（不得用 items."
                                 "length 代替 total，契约 §1.3⑧）。")
                if not page["has_more"]:
                    self.assertIsNone(
                        page["next_cursor"],
                        "K1L RED: 已到最早页 next_cursor 非空（契约 §四：null 且 "
                        "has_more=false = 已到最早一页）。")
                    break
                self.assertIsInstance(page["next_cursor"], str,
                                      "K1L RED: 有续页却 next_cursor 非字符串。")
                cursor = page["next_cursor"]
            self.assertEqual(
                page_sizes, [6, 6, 6, 2],
                f"K1L RED: 默认 limit 分页大小 {page_sizes} != [6,6,6,2] —— 默认"
                "窗口/limit 语义偏移。")
            self.assertEqual(
                len(set(collected)), len(collected),
                "K1L RED: keyset 遍历出现重复分录 —— 游标定位重复给项。")
            self.assertEqual(
                len(collected), 20,
                "K1L RED: 20 条全量遍历有丢失 —— keyset 游标漏项。")
            self.assertTrue(
                all(f"full-{i:02d}" in collected for i in range(20)),
                "K1L RED: 20 条未全部可及 —— story_list 16 条窗口被复用进账本"
                "（K1L-E3 明令禁止）或遍历提前终止。")

        with tempfile.TemporaryDirectory(prefix="k1l_a7_reset_") as tmp:
            state = str(Path(tmp) / "state.json")
            for i in range(6):
                pts.add_points(state, 1, f"seed-{i}", now=_t(i * 60))
            rev6 = pts.read_state(state)["revision"]
            p1 = pts.build_points_ledger_page(state, known_revision=rev6, limit=2)
            c1 = p1["data"]["page"]["next_cursor"]
            # ---- 插入（加分）致 revision 变化：旧游标续页 -> 重置回最新首页 -------
            pts.add_points(state, 1, "mid-insert", now=_t(600))
            rev7 = pts.read_state(state)["revision"]
            stale = pts.build_points_ledger_page(
                state, cursor=c1, known_revision=rev6, limit=2)
            self.assertTrue(
                stale["data"]["page"]["reset"],
                "K1L RED: 插入致 revision 变化后续页未重置 —— 设备会跨版本拼接缓存"
                "（arch §6.5-3 / 契约 §四明令禁止）。")
            self.assertIn(
                "mid-insert",
                " ".join(e["reason"] for e in stale["data"]["items"]),
                "K1L RED: 重置响应不是最新首页（缺最新分录）。")
            fresh = pts.build_points_ledger_page(
                state, cursor=c1, known_revision=rev7, limit=2)
            self.assertFalse(
                fresh["data"]["page"]["reset"],
                "K1L RED: revision 一致的正常续页被误重置。")
            p1_ids = [e["id"] for e in p1["data"]["items"]]
            fresh_ids = [e["id"] for e in fresh["data"]["items"]]
            self.assertFalse(
                set(p1_ids) & set(fresh_ids),
                "K1L RED: 正常续页与首页重叠 —— keyset 定位重复给项。")
            # ---- 删除致 revision 变化：同样重置回首页 ----------------------------
            plus = pts.add_points(state, 1, "to-delete", now=_t(700))
            rev8 = pts.read_state(state)["revision"]
            p2 = pts.build_points_ledger_page(
                state, cursor=c1, known_revision=rev8, limit=2)
            c2 = p2["data"]["page"]["next_cursor"]
            _points_attr("delete_entry")(state, plus["id"])
            stale_after_delete = pts.build_points_ledger_page(
                state, cursor=c2, known_revision=rev8, limit=2)
            self.assertTrue(
                stale_after_delete["data"]["page"]["reset"],
                "K1L RED: 删除致 revision 变化后续页未重置（D-08 删除重推对账面）。")
            # ---- 非法游标 -> 最新首页 reset --------------------------------------
            bad = pts.build_points_ledger_page(state, cursor="@@bogus@@", limit=2)
            self.assertTrue(bad["data"]["page"]["reset"],
                            "K1L RED: 非法游标未回最新首页 reset=True。")
            # ---- 空账：权威 items=[]，不伪造分录 ----------------------------------
            empty = pts.build_points_ledger_page(str(Path(tmp) / "none.json"))
            data = empty["data"]
            self.assertEqual(data["items"], [], "K1L RED: 空账 items 非权威空数组。")
            self.assertEqual(data["page"]["total"], 0, "K1L RED: 空账 total 非 0。")
            self.assertEqual(data["balance"], 0, "K1L RED: 空账 balance 非 0。")
            self.assertTrue(data["page"]["reset"], "K1L RED: 空账首页 reset 非 True。")
            self.assertIsNone(data["page"]["next_cursor"],
                              "K1L RED: 空账 next_cursor 非空。")
            self.assertFalse(data["page"]["has_more"], "K1L RED: 空账 has_more 非 False。")


# =========================================================================== #
# K1L-A8 — 目标配置存储（四预置、自定义边界、禁删守卫、切目标、历史快照）            #
# =========================================================================== #


class K1lTargetConfigAcceptance(unittest.TestCase):
    """K1L-A8: 四预置/默认游乐场 40、5–200 边界、预置不可删、激活禁删、快照不改写。"""

    maxDiff = None

    def _four_keys(self, target):
        return {k: target[k] for k in ("id", "name", "points", "asset")}

    # coverage: K1L-A8 / S-F6.5-AC1/AC2/AC4（存储半）+ OQ-2/OQ-3 + K1L-E4
    def test_k1l_a8_target_config_storage_presets_custom_bounds_and_guards(self):
        pts = _points_module()
        get_active = _points_attr("get_active_target")
        list_targets = _points_attr("list_targets")
        add_custom = _points_attr("add_custom_target")
        set_active = _points_attr("set_active_target")
        delete_target = _points_attr("delete_target")

        with tempfile.TemporaryDirectory(prefix="k1l_a8_") as tmp:
            state = str(Path(tmp) / "state.json")
            # ---- 四预置 + 出厂默认激活游乐场 40 ----------------------------------
            presets = list_targets(state)
            got = {self._four_keys(t) and (t["id"], t["name"], t["points"], t["asset"])
                   for t in presets}
            expected = {(t["id"], t["name"], t["points"], t["asset"])
                        for t in PRESET_TARGETS_FROZEN}
            self.assertTrue(
                expected <= got,
                f"K1L RED: 四预置目标缺失或值漂移: {sorted(got)} != {sorted(expected)}"
                "（S-F6.5-AC1：巧克力 20 / 看电影 30 / 游乐场 40 / 礼物 50）。")
            self.assertEqual(
                self._four_keys(get_active(state)),
                self._four_keys(DEFAULT_TARGET_FROZEN),
                "K1L RED: 出厂激活目标非游乐场 40（S-F6.5-AC1：出厂默认激活，任何"
                "路径不出现「暂无目标」页态）。")
            # ---- 自定义越界/畸形拒绝且零落账（OQ-3：5–200 正整数）----------------
            rev0 = pts.read_state(state)["revision"]
            for bad_points in (CUSTOM_POINTS_MIN - 1, CUSTOM_POINTS_MAX + 1, 0, -5,
                               True, 2.5, "25"):
                with self.assertRaises(
                        ValueError,
                        msg=f"K1L RED: 自定义分值 {bad_points!r} 未拒绝（5–200 正"
                            "整数，bool/float/str 皆拒，OQ-3）。"):
                    add_custom(state, "测试目标", bad_points, now=_T0)
            for bad_name in ("", "   ", None):
                with self.assertRaises(
                        ValueError,
                        msg=f"K1L RED: 自定义名 {bad_name!r} 未拒绝（非空 str）。"):
                    add_custom(state, bad_name, 25, now=_T0)
            self.assertEqual(pts.read_state(state)["revision"], rev0,
                             "K1L RED: 拒绝路径动了 revision（拒绝零落账）。")
            self.assertEqual(
                len([t for t in list_targets(state) if t["id"] not in PRESET_IDS_FROZEN]),
                0, "K1L RED: 被拒的自定义目标残留存储。")
            # ---- 边界值 5 / 200 收；自定义可删（非激活）--------------------------
            small = add_custom(state, "小橡皮", CUSTOM_POINTS_MIN, now=_t(0))
            big = add_custom(state, "自行车", CUSTOM_POINTS_MAX, now=_t(60))
            for created in (small, big):
                self.assertIsInstance(
                    created.get("id"), str,
                    "K1L RED: 自定义目标 id 非字符串（id 由执行器生成，模型不产）。")
                self.assertTrue(
                    bool(created.get("id")),
                    "K1L RED: 自定义目标 id 为空 —— 执行器必须生成非空唯一 id。")
                self.assertEqual(created.get("asset"), "sc-flag",
                                 "K1L RED: 自定义目标 asset 非 sc-flag（通用标记图，"
                                 "契约 §1.3⑦ asset 闭集）。")
            ids_after_add = {t["id"] for t in list_targets(state)}
            self.assertIn(small["id"], ids_after_add, "K1L RED: 自定义 5 未持久化。")
            self.assertIn(big["id"], ids_after_add, "K1L RED: 自定义 200 未持久化。")
            delete_target(state, small["id"], now=_t(120))
            delete_target(state, big["id"], now=_t(180))
            ids_after_del = {t["id"] for t in list_targets(state)}
            self.assertNotIn(small["id"], ids_after_del,
                             "K1L RED: 非激活自定义删除后仍在库。")
            self.assertNotIn(big["id"], ids_after_del,
                             "K1L RED: 非激活自定义删除后仍在库。")
            # ---- 真实兑换快照 + 切目标不改写历史名目 ------------------------------
            set_active(state, "preset-playground", now=_t(240))
            pts.add_points(state, 63, "期中进步奖", now=_t(300))
            redemption = pts.redeem_points(state, 40, now=_t(360))
            self.assertEqual(redemption["reason"], "兑换游乐场",
                             "K1L RED: 兑换事由快照非「兑换游乐场」。")
            snapshot_before = dict(redemption)
            rev_pre_switch = pts.read_state(state)["revision"]
            new_active = set_active(state, "preset-gift", now=_t(420))
            self.assertEqual(self._four_keys(new_active),
                             self._four_keys(PRESET_TARGETS_FROZEN[3]),
                             "K1L RED: 切目标后激活目标非礼物 50。")
            st_after_switch = pts.read_state(state)
            self.assertGreater(
                st_after_switch["revision"], rev_pre_switch,
                "K1L RED: 切目标未递增 revision —— QQ 写入的观察推送（GAP-1）依赖 "
                "revision 变化。")
            self.assertEqual(
                len(st_after_switch["entries"]), 2,
                "K1L RED: 切目标产生了分录 —— 切目标不自动扣分（arch 场景⑥E），"
                "分录只能由 add/redeem 产生。")
            self.assertEqual(st_after_switch["balance"], 23,
                             "K1L RED: 切目标动了余额。")
            old_entry = [e for e in st_after_switch["entries"]
                         if e["id"] == redemption["id"]][0]
            self.assertEqual(
                (old_entry["reason"], old_entry["delta"],
                 old_entry["balance_before"], old_entry["balance_after"]),
                (snapshot_before["reason"], snapshot_before["delta"],
                 snapshot_before["balance_before"], snapshot_before["balance_after"]),
                "K1L RED: 切目标改写了历史兑换名目/前后余额 —— 历史名目保持原快照"
                "（arch 场景⑥E / K1L-E4）。")
            # ---- 兑换跟随新激活目标分值 ------------------------------------------
            pts.add_points(state, 60, "期末进步奖", now=_t(480))   # 23+60=83
            with self.assertRaises(ValueError,
                                   msg="K1L RED: 激活 50 后兑 40 未拒绝。"):
                pts.redeem_points(state, 40, now=_t(540))
            entry_gift = pts.redeem_points(state, 50, now=_t(600))
            self.assertEqual(entry_gift["reason"], "兑换礼物",
                             "K1L RED: 新目标兑换事由错。")
            # ---- 预置不可删 + 激活中禁删（OQ-2）----------------------------------
            for preset_id in PRESET_IDS_FROZEN:
                with self.assertRaises(
                        ValueError,
                        msg=f"K1L RED: 预置目标 {preset_id} 可删 —— 预置场景不可删"
                            "（S-F6.5-AC4 / arch §5.3.2 实现内不可删常量）。"):
                    delete_target(state, preset_id, now=_t(660))
            # ---- 激活中自定义禁删：先切换再删 -------------------------------------
            ice = add_custom(state, "冰淇淋", 25, now=_t(720))
            set_active(state, ice["id"], now=_t(780))
            with self.assertRaises(
                    ValueError,
                    msg="K1L RED: 激活中自定义目标被直接删除 —— OQ-2（先切换到其他"
                        "目标再删，任何路径不出现「暂无目标」页态）。"):
                delete_target(state, ice["id"], now=_t(840))
            self.assertIn(ice["id"], {t["id"] for t in list_targets(state)},
                          "K1L RED: 激活中禁删拒绝却删掉了存储。")
            set_active(state, "preset-movie", now=_t(900))
            delete_target(state, ice["id"], now=_t(960))
            self.assertNotIn(
                ice["id"], {t["id"] for t in list_targets(state)},
                "K1L RED: 切换后删除激活过的自定义目标失败。")
            self.assertEqual(
                self._four_keys(get_active(state)),
                self._four_keys(PRESET_TARGETS_FROZEN[1]),
                "K1L RED: 删除非激活自定义动了激活目标。")
            # ---- 未知 id 拒绝 -----------------------------------------------------
            with self.assertRaises(ValueError, msg="K1L RED: 未知目标 id 切换未拒。"):
                set_active(state, "no-such-target", now=_t(1020))
            with self.assertRaises(ValueError, msg="K1L RED: 未知目标 id 删除未拒。"):
                delete_target(state, "no-such-target", now=_t(1080))
            # ---- build_points_home 的 target = 当前激活目标（front 权威帧一致性）--
            home = pts.build_points_home(state)
            self.assertEqual(
                self._four_keys(home["data"]["target"]),
                self._four_keys(PRESET_TARGETS_FROZEN[1]),
                "K1L RED: build_points_home target 未跟随激活目标（K0 出厂语义在"
                "动态目标下必须延续）。")


# =========================================================================== #
# K1L-A9 — GAP-1 producer 侧：front 观察缝 restore 推送                             #
# =========================================================================== #


class K1lGap1ProducerRestoreAcceptance(unittest.TestCase):
    """K1L-A9: 外部写入致 revision 变化 -> front 权威 restore 帧；删除另推 ledger 首页。"""

    maxDiff = None

    # coverage: K1L-A9 / GAP-1 + arch §5.1.2 front 行 / §5.2 场景⑥B + S-F6.4-AC5
    #           （数据半）+ K1L-E5
    def test_k1l_a9_gap1_producer_side_restore_observation(self):
        pts = _points_module()
        front = _front()
        observe = getattr(front, "_observe_points_revision", None)
        self.assertTrue(
            callable(observe),
            "K1L RED: front_app.py 无 _observe_points_revision —— GAP-1 producer 侧"
            "观察缝未落地（K0 §六.1 留根：QQ 落账后设备缓存同步推送缺位）。")
        self.assertTrue(
            asyncio.iscoroutinefunction(observe),
            "K1L RED: _observe_points_revision 非 async —— 推送沿 _push_display_"
            "payload 下发须在事件循环内。")

        with tempfile.TemporaryDirectory(prefix="k1l_a9_") as tmp:
            with _front_with_points(tmp) as f:
                sp = f.POINTS_STATE_PATH
                # ---- 新连接：只建立基线，推 0 帧 --------------------------------
                dev = _FakeDeviceSocket()
                r = asyncio.run(f._observe_points_revision(dev))
                self.assertFalse(r, "K1L RED: 首次观察（建立基线）返回 True。")
                self.assertEqual(_points_home_frames(dev.sent_text), [],
                                 "K1L RED: 基线观察推送了帧（无变化不得推）。")
                # ---- 外部写入（QQ CLI 同一执行器路径）-> 恰一帧 restore home -----
                entry = pts.add_points(sp, 5, "QQ 加分", now=_T0)
                r = asyncio.run(f._observe_points_revision(dev))
                self.assertTrue(r, "K1L RED: revision 变化观察未报告推送（返回 False）。")
                homes = _points_home_frames(dev.sent_text)
                self.assertEqual(len(homes), 1,
                                 f"K1L RED: 外部写入后推送 {len(homes)} 帧 home（应恰"
                                 "一帧）。")
                data = homes[0].get("data") or {}
                self.assertIs(data.get("restore"), True,
                              "K1L RED: QQ 写入推送 restore 非 True —— 会抢前台"
                              "（S-F6.4-AC5 / 契约 §1.3⑦：QQ 写入恒 restore:true）。")
                self.assertEqual(data.get("balance"), 5,
                                 "K1L RED: 推送余额非权威值（读自 points.py 单源）。")
                self.assertEqual(data.get("revision"), 1,
                                 "K1L RED: 推送 revision 非权威值。")
                # ---- 无变化：零帧 ------------------------------------------------
                r = asyncio.run(f._observe_points_revision(dev))
                self.assertFalse(r, "K1L RED: 无变化观察返回 True。")
                self.assertEqual(len(_points_home_frames(dev.sent_text)), 1,
                                 "K1L RED: 无变化观察重复推送。")
                # ---- 外部删除 -> home 之外另推 ledger 首页 restore+reset ---------
                pts_delete = _points_attr("delete_entry")
                pts_delete(sp, entry["id"])
                r = asyncio.run(f._observe_points_revision(dev))
                self.assertTrue(r, "K1L RED: 删除致 revision 变化未报告推送。")
                homes = _points_home_frames(dev.sent_text)
                self.assertEqual(len(homes), 2, "K1L RED: 删除后未另推 home。")
                home_data = homes[1].get("data") or {}
                self.assertIs(home_data.get("restore"), True,
                              "K1L RED: 删除重推 home restore 非 True。")
                self.assertEqual(home_data.get("balance"), 0,
                                 "K1L RED: 删除重推余额非重算权威值 0。")
                ledgers = _points_ledger_frames(dev.sent_text)
                self.assertEqual(len(ledgers), 1,
                                 f"K1L RED: 删除后 points_ledger 首页推送 {len(ledgers)}"
                                 " 帧（应恰一帧——契约 §四：删除后重推对账）。")
                ldata = ledgers[0].get("data") or {}
                self.assertIs(ldata.get("restore"), True,
                              "K1L RED: 删除重推 ledger restore 非 True。")
                self.assertTrue(
                    (ldata.get("page") or {}).get("reset"),
                    "K1L RED: 删除重推 ledger page.reset 非 True —— 设备必须丢弃旧"
                    "分页缓存（契约 §四）。")
                self.assertNotIn(
                    "target", ldata,
                    "K1L RED: points_ledger 载荷带 target 字段 —— K0 ruling：ledger "
                    "载荷无 target 字段，仅 points_home 强制 target 四键。")
                self.assertEqual(ldata.get("items"), [],
                                 "K1L RED: 删除唯一分录后 ledger items 非权威空。")
                # ---- 每连接独立基线：新 websocket 不吃旧连接的观察态 --------------
                dev2 = _FakeDeviceSocket()
                r = asyncio.run(f._observe_points_revision(dev2))
                self.assertFalse(r, "K1L RED: 新连接首次观察返回 True。")
                self.assertEqual(_points_home_frames(dev2.sent_text), [],
                                 "K1L RED: 新连接基线观察推了帧。")
                # ---- state 损坏：不抛、不推、不改文件（失败可见、不伪造）----------
                dev3 = _FakeDeviceSocket()
                asyncio.run(f._observe_points_revision(dev3))   # 建立基线
                good_bytes = Path(sp).read_bytes()
                Path(sp).write_bytes(b"{not-json")
                corrupt_bytes = Path(sp).read_bytes()
                try:
                    r = asyncio.run(f._observe_points_revision(dev3))
                except Exception as e:  # noqa: BLE001
                    self.fail(
                        f"K1L RED: state 损坏时观察缝抛异常 {e!r} —— 失败必须可见于"
                        "返回值（False），不得炸连接、不得伪造。")
                self.assertFalse(r, "K1L RED: state 损坏观察返回 True（伪造成功）。")
                self.assertEqual(_points_home_frames(dev3.sent_text), [],
                                 "K1L RED: state 损坏仍推送帧（伪造权威视图）。")
                self.assertEqual(
                    Path(sp).read_bytes(), corrupt_bytes,
                    "K1L RED: 观察缝改写了（污染的）state 文件 —— 失败路径不得本地"
                    "补写/修复/重建（NFR-OFF-1 数据面半；恢复由外部显式动作完成）。")
                # ---- 恢复后：无陈旧补写、按当前权威状态观察 ------------------------
                Path(sp).write_bytes(good_bytes)
                r = asyncio.run(f._observe_points_revision(dev3))
                self.assertFalse(
                    r, "K1L RED: 恢复后（无变化）观察推帧 —— 失败期间不得排队任何"
                       "陈旧载荷（无本地写队列，D-06）。")
                pts.add_points(sp, 2, "恢复后写入", now=_t(60))
                r = asyncio.run(f._observe_points_revision(dev3))
                self.assertTrue(r, "K1L RED: 恢复后外部写入未推送。")
                homes3 = _points_home_frames(dev3.sent_text)
                self.assertEqual(len(homes3), 1, "K1L RED: 恢复后推送帧数 != 1。")
                self.assertEqual(
                    (homes3[0].get("data") or {}).get("balance"), 2,
                    "K1L RED: 恢复后推送余额非当前权威值 2（删除后 0 + 加 2）—— "
                    "推送必须读当前真实状态，不消费失败期间的陈旧数据。")


# =========================================================================== #
# K1L-A10 — 失败路径与离线边界（不推成功视图、零写入、无补写队列）                   #
# =========================================================================== #


class K1lFailurePathOfflineBoundaryAcceptance(unittest.TestCase):
    """K1L-A10: 执行器失败 front 不推成功视图；失败零写入；恢复后无幽灵补写。"""

    maxDiff = None

    # coverage: K1L-A10 / NFR-OFF-1 + D-06 + S-F6.3-AC6/S-F6.2-AC5（数据面半）
    #           + K1L-E5
    def test_k1l_a10_failure_paths_offline_boundary_no_local_queue(self):
        pts = _points_module()
        with tempfile.TemporaryDirectory(prefix="k1l_a10_") as tmp:
            with _front_with_points(tmp) as f:
                sp = f.POINTS_STATE_PATH
                # ---- 执行器硬失败（模拟持久化层故障）-> 不推成功视图 -------------
                dev = _FakeDeviceSocket()
                orig_add = pts.add_points

                def _boom(*args, **kwargs):
                    raise RuntimeError("simulated persist layer failure")

                pts.add_points = _boom
                try:
                    try:
                        asyncio.run(f._consume_points_intent(
                            dev, {"view": "control",
                                  "data": {"action": "add_points", "points": 5,
                                           "reason": "失败场景"}}, restore=True))
                    except Exception:  # noqa: BLE001 — 失败以异常表达也算消费失败
                        pass
                finally:
                    pts.add_points = orig_add
                self.assertEqual(
                    _points_home_frames(dev.sent_text), [],
                    "K1L RED: 执行器持久化失败仍推送 points_home —— 持久化失败不得"
                    "推送「成功」视图（arch 场景⑥尾段），设备会看到没有发生的记账。")
                self.assertFalse(
                    Path(sp).exists(),
                    "K1L RED: 执行器失败路径在磁盘留下 state 文件 —— 失败零写入。")
                # ---- 恢复后：只落真实一笔，无幽灵补写（无离线写队列）--------------
                dev2 = _FakeDeviceSocket()
                asyncio.run(f._consume_points_intent(
                    dev2, {"view": "control",
                           "data": {"action": "add_points", "points": 5,
                                    "reason": "恢复后真实写入"}}, restore=True))
                st = pts.read_state(sp)
                self.assertEqual(
                    len(st["entries"]), 1,
                    "K1L RED: 失败恢复后出现多笔分录 —— 失败尝试不得遗留待补写队列"
                    "（NFR-OFF-1：无离线写队列，写操作失败即失败、由用户重试）。")
                self.assertEqual(st["balance"], 5, "K1L RED: 恢复后余额非 5。")
                self.assertEqual(
                    len(_points_home_frames(dev2.sent_text)), 1,
                    "K1L RED: 恢复后成功写入未推权威帧。")
                # ---- state 损坏时执行器拒绝、front 拒绝不推（不伪造、不当空账）----
                Path(sp).write_bytes(b"{corrupt")
                dev3 = _FakeDeviceSocket()
                try:
                    asyncio.run(f._consume_points_intent(
                        dev3, {"view": "control",
                               "data": {"action": "add_points", "points": 3,
                                        "reason": "损坏场景"}}, restore=True))
                except Exception:  # noqa: BLE001 — ValueError 上抛或被消费缝吞掉均可
                    pass
                self.assertEqual(
                    _points_home_frames(dev3.sent_text), [],
                    "K1L RED: state 损坏时仍推 points_home —— 损坏账本必须显式失败，"
                    "绝不静默当空账覆盖真账本（数据丢失面）。")


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
