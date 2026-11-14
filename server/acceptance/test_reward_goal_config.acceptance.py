"""Frozen acceptance tests — TaskPackage 261104-k4-reward-goal-config（server 面）。

承载 story / 语义权威 = docs/requirements.md：
  S-F6.5（奖励场景与激活目标配置；AC1 四预置 / AC2 自定义 5–200 + 通用标记图 + 入库可激活 /
         AC3 唯一激活 + 切换后兑换只认新分值 + 切片刻度数据面 / AC4 预置禁删 + 激活中自定义禁删 +
         非激活自定义可删）
  S-F6.4-AC4（QQ 命令一次性可判：每条命令独立可执行或独立报错，不依赖多轮澄清）
  S-F6.4-AC5（QQ 通道到账只更新数据、不抢前台——本件只证 target 回执可对账，设备演出半归 host/真机）
  S-F6.3-AC3（兑换只认当前激活目标分值）
  S-F6.1-AC3（兑换扣分后按新余额重画——本件证「切目标本身零分录」，余额变化只由兑换产生）
契约只读消费 = docs/contracts/interface-contract.md §1.3⑦（points_home.target 四键 /
asset 闭集 sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag）与 §四（state 路径单源 / 原子写）。
工程先例形态 = server/acceptance/test_qq_points_channel.acceptance.py（K3 冻结件）。

Scope of record（K4G-A1..A9 → 本文件测试方法）:

  K4G-A1 自定义目标 5–200 边界 + 入库可激活（S-F6.5-AC2）：
    · test_k4g_a1_custom_points_bounds_5_200          （行为级 / 边界）
  K4G-A2 删除规则：预置禁删逐个 / 激活中自定义禁删 / 非激活可删 / 未知 id 拒绝
    （S-F6.5-AC4）：
    · test_k4g_a2_delete_policy_presets_and_active    （行为级 / 错误路径）
    · test_k4g_a2_delete_nonactive_custom_succeeds    （行为级）
  K4G-A3 唯一激活 + build_points_home target 四键随切更新（S-F6.5-AC3）：
    · test_k4g_a3_unique_active_and_home_target_sync  （行为级）
  K4G-A4 切目标不扣分：balance / len(entries) 不变、revision +1、零新分录
    （S-F6.5-AC3 + S-F6.1-AC3）：
    · test_k4g_a4_switch_does_not_deduct             （行为级 / 不变量）
  K4G-A5 切后兑换只认新分值（S-F6.3-AC3）：
    · test_k4g_a5_post_switch_redeem_new_value_only  （行为级 / 错误路径）
  K4G-A6 历史兑换名目快照不变（S-F6.5-AC3）：
    · test_k4g_a6_history_reason_snapshot_kept       （行为级 / 不变量）
  K4G-A7 target 回执形状 + usage/语义拒绝分级（S-F6.4-AC4）：
    · test_k4g_a7_target_receipts_no_delta_entry_id  （行为级）
    · test_k4g_a7_usage_exit_2_and_semantic_exit_1   （行为级 / 错误路径）
  K4G-A8 command_id 幂等沿用 K3（S-F6.5-AC2 + S-F6.4-AC4）：
    · test_k4g_a8_target_command_id_idempotent       （行为级 / 幂等）
  K4G-A9 kidpal-points-write SKILL.md 目标配置纪律（S-F6.5 + S-F6.4-AC4）：
    · test_k4g_a9_skill_md_target_management         （机制级 / 静态）

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — 逐条对齐 plan.md「技术方案」C 节：

  1) CLI 新增三个扁平动词子命令（同一 `python -m gateway.points`，同一 state env）：
       target-add    --name TEXT --points N --command-id ID
       target-switch --target-id ID --command-id ID
       target-delete --target-id ID --command-id ID
     列举复用既有只读 `targets`（不新增 `target list`）。
  2) 回执恒为**单行 key=value**（空白分词），且 **target 回执禁带 delta= / entry_id=**
     （防模型误读为记账）：
       target-add    含 target_id=（执行器生成，形如 target-<hex>）、name=、points=、
                     revision=、command_id=
       target-switch 含 target_id=（切换到的 id）、active_target_id=、**balance=**
                     （= 没扣分的可对账事实）、revision=、command_id=
       target-delete 含 target_id=、deleted=true、revision=、command_id=
  3) 校验分工：--points 复用 `_parse_cli_int`；**5–200 范围不重复实现**，交给
     `add_custom_target` 单源（越界 -> ValueError -> exit 1，锁外零写入）。
  4) 旗标错误 -> exit 2（未知旗标 / 重复旗标 / 旗标缺值 / 缺必填旗标）；语义拒绝 ->
     exit 1：说明写 stderr、stdout 无成功回执、state 文件**字节不变**。
     语义拒绝集合 = 分值 4/201、奖励名空、未知 target id、删预置、删激活中自定义。
  5) 写路径全部走 K1 既有函数（add_custom_target / set_active_target / delete_target）
     与 K1 幂等回执表：同 command_id 重放返回首次结果、不新增目标、**不重复 bump
     revision**（target-add / target-switch 均须如此）。
  6) 切目标 = 只写 active_target + revision+1、零分录；既有分录的 reason 快照不改写。
  7) 兑换（redeem_points）继续只认 `_resolve_active()` 的分值——切后旧分值 exit 1 零写入。

==================== 契约条目 ↔ 验收断言 的强度申报 ====================

| 契约 / story 条目 | 承载断言 | 性质档 |
|---|---|---|
| S-F6.5-AC1 四预置清单/分值/默认游乐场 40 | 既有 K1 冻结面；本件 test_k4g_a2_delete_policy_presets_and_active 顺带复核四 pre id 恒在 | 行为级（回归半） |
| S-F6.5-AC2 自定义 5–200 + 入库可激活 + 通用标记图 | test_k4g_a1_custom_points_bounds_5_200（asset == sc-flag） | 行为级 |
| S-F6.5-AC3 唯一激活 + 切换联动 | test_k4g_a3_unique_active_and_home_target_sync / test_k4g_a4_switch_does_not_deduct | 行为级 |
| S-F6.5-AC3 切片刻度（设备端） | 设备半归 firmware/main/test/reward_goal_config.acceptance.c（K4G-H2） | 设备/host |
| S-F6.5-AC4 预置禁删 / 激活中禁删 / 非激活可删 | test_k4g_a2_delete_policy_presets_and_active / test_k4g_a2_delete_nonactive_custom_succeeds | 行为级 |
| S-F6.4-AC4 一次性可判（每条命令独立执行或独立报错） | test_k4g_a7_usage_exit_2_and_semantic_exit_1 | 行为级 |
| S-F6.4-AC5 到账不抢前台（下行帧半） | K3Q-A6 既有冻结面；K4 的 target 动作经同一观察缝（真机轮复证） | 行为级（回归半） |
| S-F6.3-AC3 兑换只认激活分值 | test_k4g_a5_post_switch_redeem_new_value_only | 行为级 |
| S-F6.1-AC3 兑换扣分后按新余额 | test_k4g_a5_post_switch_redeem_new_value_only（balance 变化只由兑换产生） | 行为级 |
| K1→K4 单向 writer / K3→K4 同 skill 扩展 | test_k4g_a9_skill_md_target_management / 本件全 CLI 面 | 机制级 + 行为级 |

弱于契约处（显式缺口，不得当已兑现）:
  GAP-K4G-1 「模型真实按新 §5 报错表一次性回复」离线不可证 → 真实 QQ 轮。
  GAP-K4G-2 S-F6.4-AC5「其他页不抢前台 / 积分页原地刷新」的设备演出半不落本件 → 真机轮
            （K4G-L1 live-manual）。
  GAP-K4G-3 S-F6.5-AC3「切片刻度随目标伸缩」的视觉像素半归 host K4G-H2 + 真机看屏。
  GAP-K4G-4 自定义目标名文案长度/字符集人机判定 → 真实 QQ 轮。

行为等价假设（显式）:
  EQ-1 `python -m gateway.points`（cwd=server）≡ ECS `cd /opt/kidpal/front && python -m
       gateway.points` 同一导入/执行语义（K3Q EQ-1 同口径）。
  EQ-2 tmp 目录 state.json ≡ /opt/kidpal/points/state.json 同一文件语义（K3Q EQ-3）。

Coverage map（acceptanceRef -> test）:
  K4G-A1 -> test_k4g_a1_custom_points_bounds_5_200
  K4G-A2 -> test_k4g_a2_delete_policy_presets_and_active / test_k4g_a2_delete_nonactive_custom_succeeds
  K4G-A3 -> test_k4g_a3_unique_active_and_home_target_sync
  K4G-A4 -> test_k4g_a4_switch_does_not_deduct
  K4G-A5 -> test_k4g_a5_post_switch_redeem_new_value_only
  K4G-A6 -> test_k4g_a6_history_reason_snapshot_kept
  K4G-A7 -> test_k4g_a7_target_receipts_no_delta_entry_id / test_k4g_a7_usage_exit_2_and_semantic_exit_1
  K4G-A8 -> test_k4g_a8_target_command_id_idempotent
  K4G-A9 -> test_k4g_a9_skill_md_target_management
  coverage: K4G-L1 — live-manual 真实 QQ 自定义/切换/删除全过程 + 不抢前台 + 刷新时延实测
            （载体 device-round-evidence.md），不落自动断言、不计入本文件通过面。

Run protocol（文件名两点须 importlib 模式；root 由 __file__ 显式推导，不依赖调用点 cwd）:
  python -m pytest -q --import-mode=importlib server/acceptance/test_reward_goal_config.acceptance.py

REVISION LOG
  2026-11-15 初版冻结（acceptance-author）。设计态红 = points.py 无 target-add /
             target-switch / target-delete 子命令（main 未知子命令 exit 2）与对应回执；
             SKILL.md §8 仍写「目标的新增 / 切换 / 删除不在本 skill 面」硬阻断句、
             frontmatter/§2/§5 未覆盖目标管理。K1 存储层（add_custom_target /
             set_active_target / delete_target / 兑换只认激活分值）已绿，本件对它们
             做行为复核（非空过：断言的是 K4 新增的 CLI 面与回执形状）。
"""

from __future__ import annotations

import os
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

# --------------------------------------------------------------------------- #
# Paths（仓根由 __file__ 显式推导）                                              #
# --------------------------------------------------------------------------- #

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
POINTS_PY = SERVER / "gateway" / "points.py"
POINTS_SKILL_PATH = (
    REPO_ROOT
    / "server/deploy/openclaw-skills/workspace-main/kidpal-points-write/SKILL.md"
)

POINTS_STATE_ENV = "KIDPAL_POINTS_STATE"


# --------------------------------------------------------------------------- #
# 工具                                                                          #
# --------------------------------------------------------------------------- #


def _new_state_path(tc) -> Path:
    d = tempfile.mkdtemp(prefix="k4g_")
    tc.addCleanup(lambda: __import__("shutil").rmtree(d, ignore_errors=True))
    return Path(d) / "state.json"


def _snapshot(state: Path):
    """state 文件字节快照；不存在返回 None（零写入判据同时覆盖「不创建」）。"""
    return state.read_bytes() if state.exists() else None


def _parse_kv(line: str) -> dict:
    out = {}
    for tok in line.split():
        if "=" in tok:
            key, val = tok.split("=", 1)
            out[key] = val
    out["_line"] = line.strip()
    return out


def _receipt_lines(proc) -> list:
    return [_parse_kv(ln) for ln in proc.stdout.splitlines() if ln.strip()]


def _receipt(proc) -> dict:
    lines = _receipt_lines(proc)
    return lines[0] if lines else {}


def _has_key(r: dict, suffix: str) -> bool:
    return any(k.endswith(suffix) for k in r if k != "_line")


def _cli_env(state_path):
    env = dict(os.environ)
    env.pop(POINTS_STATE_ENV, None)
    if state_path is not None:
        env[POINTS_STATE_ENV] = str(state_path)
    env["PYTHONIOENCODING"] = "utf-8"
    env["PYTHONUTF8"] = "1"
    return env


def _run_cli(args, state_path):
    return subprocess.run(
        [sys.executable, "-m", "gateway.points", *args],
        cwd=str(SERVER),
        env=_cli_env(state_path),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=120,
    )


_points_cache = None


def _points_module():
    global _points_cache
    if _points_cache is not None:
        return _points_cache
    if not POINTS_PY.is_file():
        raise AssertionError("K4G RED: server/gateway/points.py 不在世 —— K1 权威账本丢失。")
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.points as pts  # noqa: PLC0415

    _points_cache = pts
    return pts


def _add_seed(state: Path, points: int, reason: str, cid: str) -> None:
    proc = _run_cli(
        ["add", "--points", str(points), "--reason", reason, "--command-id", cid], state)
    assert proc.returncode == 0, (
        f"K4G 前置破坏：seed add 失败；stdout={proc.stdout!r} stderr={proc.stderr!r}")


def _target_add(state: Path, name: str, points, cid: str):
    return _run_cli(
        ["target-add", "--name", name, "--points", str(points), "--command-id", cid], state)


# =========================================================================== #
# K4G-A1 — 自定义目标 5–200 边界 + 入库可激活                                    #
# =========================================================================== #


class K4gA1CustomBounds(unittest.TestCase):
    maxDiff = None

    def test_k4g_a1_custom_points_bounds_5_200(self):
        """coverage: K4G-A1 / S-F6.5-AC2 —— 5 与 200 接受、4 与 201 拒绝且 state 零写入；入库可激活。"""
        pts = _points_module()
        for value in (5, 200):
            state = _new_state_path(self)
            before = _snapshot(state)
            proc = _target_add(state, f"奖励{value}", value, f"tadd-{value}")
            self.assertEqual(
                proc.returncode, 0,
                f"K4G-A1 RED: 分值 {value} 应接受；stdout={proc.stdout!r} stderr={proc.stderr!r}")
            r = _receipt(proc)
            self.assertEqual(r.get("points"), str(value),
                             f"K4G-A1 RED: 回执 points 非 {value}；receipt={r}")
            self.assertTrue(_has_key(r, "target_id"),
                            f"K4G-A1 RED: target-add 回执缺 target_id；receipt={r}")
            targets = pts.list_targets(str(state))
            self.assertEqual(len(targets), 5,
                             f"K4G-A1 RED: 入库后目标应 4 预置 + 1 自定义；实得 {targets}")
            custom = [t for t in targets if t["name"] == f"奖励{value}"]
            self.assertEqual(len(custom), 1, f"K4G-A1 RED: 自定义目标未入库；{targets}")
            self.assertEqual(custom[0]["asset"], "sc-flag",
                             f"K4G-A1 RED: 自定义目标须用通用标记图 sc-flag；{custom[0]}")
            # 入库即可激活（AC2）
            sw = _run_cli(
                ["target-switch", "--target-id", custom[0]["id"],
                 "--command-id", f"tsw-{value}"], state)
            self.assertEqual(sw.returncode, 0,
                             f"K4G-A1 RED: 新自定义目标应可激活；stderr={sw.stderr!r}")
            self.assertEqual(pts.get_active_target(str(state))["id"], custom[0]["id"])
            self.assertNotEqual(before, None)
        for value in (4, 201):
            state = _new_state_path(self)
            _add_seed(state, 10, "seed", f"s-{value}")
            before = state.read_bytes()
            proc = _target_add(state, f"越界{value}", value, f"tbad-{value}")
            self.assertEqual(
                proc.returncode, 1,
                f"K4G-A1 RED: 分值 {value} 应语义拒绝 exit 1；"
                f"stdout={proc.stdout!r} stderr={proc.stderr!r}")
            self.assertTrue(proc.stderr.strip(), f"K4G-A1 RED: {value} 拒绝说明应写 stderr")
            self.assertNotIn("revision=", proc.stdout,
                             f"K4G-A1 RED: {value} 拒绝却输出成功回执；stdout={proc.stdout!r}")
            self.assertEqual(state.read_bytes(), before,
                             f"K4G-A1 RED: {value} 越界必须零写入")
            self.assertEqual(len(pts.list_targets(str(state))), 4,
                             f"K4G-A1 RED: {value} 越界不得入库")


# =========================================================================== #
# K4G-A2 — 删除规则                                                             #
# =========================================================================== #


class K4gA2DeletePolicy(unittest.TestCase):
    maxDiff = None

    def test_k4g_a2_delete_policy_presets_and_active(self):
        """coverage: K4G-A2 / S-F6.5-AC4 —— 四预置禁删逐个、激活中自定义禁删、未知 id 拒绝，全零写入。"""
        pts = _points_module()
        state = _new_state_path(self)
        _add_seed(state, 10, "seed", "s0")
        # 预置禁删（逐个；AC1 四 pre id 恒在顺带复核）
        preset_ids = [t["id"] for t in pts.list_targets(str(state))]
        self.assertEqual(preset_ids, ["preset-choco", "preset-movie",
                                      "preset-playground", "preset-gift"],
                         f"K4G-A2 RED: 四预置 id 集漂移；{preset_ids}")
        for pid in preset_ids:
            before = state.read_bytes()
            proc = _run_cli(
                ["target-delete", "--target-id", pid, "--command-id", f"del-{pid}"], state)
            self.assertEqual(
                proc.returncode, 1,
                f"K4G-A2 RED: 删除预置 {pid} 应 exit 1；stdout={proc.stdout!r} stderr={proc.stderr!r}")
            self.assertTrue(proc.stderr.strip(), f"K4G-A2 RED: {pid} 拒绝说明应写 stderr")
            self.assertNotIn("deleted=true", proc.stdout,
                             f"K4G-A2 RED: {pid} 拒绝却回成功删除；stdout={proc.stdout!r}")
            self.assertEqual(state.read_bytes(), before, f"K4G-A2 RED: {pid} 零写入")
        # 激活中自定义禁删
        added = _target_add(state, "看海", 60, "tadd-sea")
        self.assertEqual(added.returncode, 0, f"K4G-A2 前置：target-add 失败 {added.stderr!r}")
        sea_id = _receipt(added).get("target_id")
        sw = _run_cli(["target-switch", "--target-id", sea_id, "--command-id", "tsw-sea"], state)
        self.assertEqual(sw.returncode, 0, f"K4G-A2 前置：切换失败 {sw.stderr!r}")
        before = state.read_bytes()
        proc = _run_cli(["target-delete", "--target-id", sea_id, "--command-id", "del-sea"], state)
        self.assertEqual(
            proc.returncode, 1,
            f"K4G-A2 RED: 删除激活中自定义应 exit 1（须先切换）；stderr={proc.stderr!r}")
        self.assertIn("切换", proc.stderr,
                      f"K4G-A2 RED: 拒绝说明须指向「先切换到其他目标再删」；stderr={proc.stderr!r}")
        self.assertEqual(state.read_bytes(), before, "K4G-A2 RED: 激活中禁删必须零写入")
        # 未知 id
        before = state.read_bytes()
        proc = _run_cli(
            ["target-delete", "--target-id", "target-deadbeef", "--command-id", "del-unk"], state)
        self.assertEqual(proc.returncode, 1,
                         f"K4G-A2 RED: 未知 target id 删除应 exit 1；stderr={proc.stderr!r}")
        self.assertEqual(state.read_bytes(), before, "K4G-A2 RED: 未知 id 零写入")

    def test_k4g_a2_delete_nonactive_custom_succeeds(self):
        """coverage: K4G-A2 / S-F6.5-AC4 —— 非激活自定义可删（deleted=true、revision +1、目标消失）。"""
        pts = _points_module()
        state = _new_state_path(self)
        _add_seed(state, 10, "seed", "s0")
        a = _target_add(state, "看电影二刷", 30, "add-a")
        b = _target_add(state, "礼物升级", 80, "add-b")
        self.assertEqual((a.returncode, b.returncode), (0, 0),
                         f"K4G-A2 前置：target-add 失败 {a.stderr!r} / {b.stderr!r}")
        aid, bid = _receipt(a).get("target_id"), _receipt(b).get("target_id")
        sw = _run_cli(["target-switch", "--target-id", aid, "--command-id", "sw-a"], state)
        self.assertEqual(sw.returncode, 0, f"K4G-A2 前置：切换失败 {sw.stderr!r}")
        st0 = pts.read_state(str(state))
        proc = _run_cli(["target-delete", "--target-id", bid, "--command-id", "del-b"], state)
        self.assertEqual(proc.returncode, 0,
                         f"K4G-A2 RED: 非激活自定义应可删；stdout={proc.stdout!r} stderr={proc.stderr!r}")
        r = _receipt(proc)
        self.assertEqual(r.get("deleted"), "true", f"K4G-A2 RED: 回执须 deleted=true；receipt={r}")
        self.assertEqual(r.get("target_id"), bid, f"K4G-A2 RED: 回执 target_id 失真；receipt={r}")
        st1 = pts.read_state(str(state))
        self.assertNotIn(bid, {t["id"] for t in st1["custom_targets"]},
                         "K4G-A2 RED: 被删自定义仍在 custom_targets")
        self.assertEqual(st1["revision"], st0["revision"] + 1,
                         "K4G-A2 RED: 删除应 revision +1")
        self.assertEqual(st1["active_target"], aid, "K4G-A2 RED: 删除不得改激活目标")
        self.assertEqual(len(st1["entries"]), len(st0["entries"]),
                         "K4G-A2 RED: 删除目标不得产生分录")


# =========================================================================== #
# K4G-A3 — 唯一激活 + build_points_home target 四键同步                          #
# =========================================================================== #


class K4gA3UniqueActive(unittest.TestCase):
    maxDiff = None

    def test_k4g_a3_unique_active_and_home_target_sync(self):
        """coverage: K4G-A3 / S-F6.5-AC3 —— switch 后标量 active_target 唯一、list 恰一命中、home 四键随切。"""
        pts = _points_module()
        state = _new_state_path(self)
        _add_seed(state, 20, "seed", "s0")
        custom = _target_add(state, "露营", 120, "add-camp")
        self.assertEqual(custom.returncode, 0, f"K4G-A3 前置：target-add 失败 {custom.stderr!r}")
        cid = _receipt(custom).get("target_id")
        expect_after_add = {
            "id": cid, "name": "露营", "points": 120, "asset": "sc-flag"}
        for target_id, expect in (
                (cid, expect_after_add),
                ("preset-movie",
                 {"id": "preset-movie", "name": "看电影", "points": 30, "asset": "sc-movie"})):
            proc = _run_cli(
                ["target-switch", "--target-id", target_id,
                 "--command-id", f"sw-{target_id}"], state)
            self.assertEqual(proc.returncode, 0,
                             f"K4G-A3 RED: 切换到 {target_id} 应 exit 0；stderr={proc.stderr!r}")
            st = pts.read_state(str(state))
            self.assertEqual(st["active_target"], target_id,
                             f"K4G-A3 RED: 标量 active_target 未随切更新；st={st.get('active_target')}")
            active = pts.get_active_target(str(state))
            self.assertEqual(active, expect,
                             f"K4G-A3 RED: 激活目标四键失真；实得 {active} 期望 {expect}")
            targets = pts.list_targets(str(state))
            hits = [t for t in targets if t["id"] == target_id]
            self.assertEqual(len(hits), 1,
                             f"K4G-A3 RED: list_targets 应恰一个命中；实得 {hits}")
            self.assertEqual(sum(1 for t in targets if t["id"] == st["active_target"]), 1,
                             "K4G-A3 RED: 同一时刻只能有一个激活目标（标量单点）")
            data = pts.build_points_home(str(state))["data"]
            self.assertEqual(data["target"], expect,
                             f"K4G-A3 RED: build_points_home target 未随切更新；{data.get('target')}")
            self.assertEqual(data["view"] if False else "points_home",
                             pts.build_points_home(str(state))["view"])


# =========================================================================== #
# K4G-A4 — 切目标不扣分                                                          #
# =========================================================================== #


class K4gA4NoDeduction(unittest.TestCase):
    maxDiff = None

    def test_k4g_a4_switch_does_not_deduct(self):
        """coverage: K4G-A4 / S-F6.5-AC3、S-F6.1-AC3 —— 切换前后 balance 与 len(entries) 不变、revision +1、零新分录。"""
        pts = _points_module()
        state = _new_state_path(self)
        _add_seed(state, 12, "扫地", "s1")
        _add_seed(state, 8, "洗碗", "s2")
        st0 = pts.read_state(str(state))
        self.assertEqual((st0["balance"], len(st0["entries"])), (20, 2),
                         "K4G-A4 前置破坏：seed 余额/分录数不符")
        proc = _run_cli(
            ["target-switch", "--target-id", "preset-choco", "--command-id", "sw-choco"], state)
        self.assertEqual(proc.returncode, 0,
                         f"K4G-A4 RED: 切换应 exit 0；stdout={proc.stdout!r} stderr={proc.stderr!r}")
        r = _receipt(proc)
        self.assertEqual(r.get("balance"), "20",
                         f"K4G-A4 RED: target-switch 回执须回显未变的 balance=20；receipt={r}")
        st1 = pts.read_state(str(state))
        self.assertEqual(st1["balance"], st0["balance"],
                         "K4G-A4 RED: 切目标不得扣分（balance 必须不变）")
        self.assertEqual(len(st1["entries"]), len(st0["entries"]),
                         "K4G-A4 RED: 切目标不得产生任何分录")
        self.assertEqual(st1["revision"], st0["revision"] + 1,
                         "K4G-A4 RED: 切目标须 bump revision（数据更新但非记账）")
        self.assertEqual(st1["active_target"], "preset-choco")


# =========================================================================== #
# K4G-A5 — 切后兑换只认新分值                                                    #
# =========================================================================== #


class K4gA5PostSwitchRedeem(unittest.TestCase):
    maxDiff = None

    def test_k4g_a5_post_switch_redeem_new_value_only(self):
        """coverage: K4G-A5 / S-F6.3-AC3 —— 旧分值 redeem 拒绝零写入；新分值 redeem 成功落负向分录。"""
        pts = _points_module()
        state = _new_state_path(self)
        _add_seed(state, 50, "攒分", "s0")
        sw = _run_cli(["target-switch", "--target-id", "preset-movie",
                       "--command-id", "sw-movie"], state)
        self.assertEqual(sw.returncode, 0, f"K4G-A5 前置：切换失败 {sw.stderr!r}")
        before = state.read_bytes()
        old = _run_cli(["redeem", "--points", "40", "--command-id", "rd-old"], state)
        self.assertEqual(
            old.returncode, 1,
            f"K4G-A5 RED: 切到 30 分目标后按旧 40 兑换应 exit 1；stderr={old.stderr!r}")
        self.assertTrue(old.stderr.strip(), "K4G-A5 RED: 拒绝说明应写 stderr")
        self.assertEqual(state.read_bytes(), before, "K4G-A5 RED: 旧分值拒绝必须零写入")
        ok = _run_cli(["redeem", "--points", "30", "--command-id", "rd-new"], state)
        self.assertEqual(ok.returncode, 0,
                         f"K4G-A5 RED: 新分值 30 兑换应 exit 0；stderr={ok.stderr!r}")
        r = _receipt(ok)
        self.assertEqual(r.get("delta"), "-30", f"K4G-A5 RED: 兑换分录非负向；receipt={r}")
        self.assertEqual(r.get("balance"), "20", f"K4G-A5 RED: 兑换后余额应 20；receipt={r}")
        self.assertIn("看电影", r.get("reason", ""),
                      f"K4G-A5 RED: 兑换事由应取新激活目标名；receipt={r}")
        st1 = pts.read_state(str(state))
        self.assertEqual(st1["balance"], 20, "K4G-A5 RED: 权威余额未按新兑换重算")
        self.assertEqual(len(st1["entries"]), 2, "K4G-A5 RED: 应恰新增一条负向分录")


# =========================================================================== #
# K4G-A6 — 历史兑换名目快照不变                                                  #
# =========================================================================== #


class K4gA6HistorySnapshot(unittest.TestCase):
    maxDiff = None

    def test_k4g_a6_history_reason_snapshot_kept(self):
        """coverage: K4G-A6 / S-F6.5-AC3 —— 切目标不改写既有兑换分录的 reason 快照。"""
        pts = _points_module()
        state = _new_state_path(self)
        _add_seed(state, 50, "攒分", "s0")
        rd = _run_cli(["redeem", "--points", "40", "--command-id", "rd-wheel"], state)
        self.assertEqual(rd.returncode, 0, f"K4G-A6 前置：默认目标兑换失败 {rd.stderr!r}")
        sw = _run_cli(["target-switch", "--target-id", "preset-gift",
                       "--command-id", "sw-gift"], state)
        self.assertEqual(sw.returncode, 0,
                         f"K4G-A6 RED: 切换应成功（否则本用例空过）；stderr={sw.stderr!r}")
        self.assertEqual(pts.get_active_target(str(state))["id"], "preset-gift",
                         "K4G-A6 前置破坏：切换未生效")
        negatives = [e for e in pts.read_state(str(state))["entries"] if e["delta"] < 0]
        self.assertEqual(len(negatives), 1, f"K4G-A6 前置破坏：负向分录数 {len(negatives)}")
        reason = negatives[0]["reason"]
        self.assertIn("游乐场", reason,
                      f"K4G-A6 RED: 历史兑换名目快照被改写；reason={reason!r}")
        self.assertNotIn("礼物", reason,
                         f"K4G-A6 RED: 切换不得重写历史 reason 为新目标名；reason={reason!r}")


# =========================================================================== #
# K4G-A7 — target 回执形状 + usage/语义拒绝分级                                  #
# =========================================================================== #


class K4gA7ReceiptsAndErrors(unittest.TestCase):
    maxDiff = None

    def _seeded(self):
        state = _new_state_path(self)
        _add_seed(state, 20, "seed", "s0")
        added = _target_add(state, "看海", 60, "tadd-sea")
        self.assertEqual(
            added.returncode, 0,
            f"K4G-A7 RED: 前置 target-add 应成功（缺失 = K4 子命令面未落地）；"
            f"stdout={added.stdout!r} stderr={added.stderr!r}")
        return state, _receipt(added).get("target_id")

    def test_k4g_a7_target_receipts_no_delta_entry_id(self):
        """coverage: K4G-A7 / S-F6.4-AC4 —— 三回执单行 key=value、禁带 delta/entry_id、switch 回显 balance。"""
        state, tid = self._seeded()
        cases = {}
        add = _target_add(state, "礼物升级", 80, "tadd-gift")
        cases["target-add"] = add
        sw = _run_cli(["target-switch", "--target-id", tid, "--command-id", "tsw-sea"], state)
        cases["target-switch"] = sw
        de = _run_cli(["target-delete", "--target-id", tid, "--command-id", "tdel-sea"], state)
        cases["target-delete"] = de
        for label, proc in cases.items():
            self.assertEqual(proc.returncode, 0,
                             f"K4G-A7 RED: {label} 应 exit 0；stdout={proc.stdout!r} stderr={proc.stderr!r}")
            lines = _receipt_lines(proc)
            self.assertEqual(len(lines), 1,
                             f"K4G-A7 RED: {label} 回执须恰一行 key=value；stdout={proc.stdout!r}")
            r = lines[0]
            for banned in ("delta", "entry_id"):
                self.assertNotIn(banned, r,
                                 f"K4G-A7 RED: {label} 回执禁带 {banned}=（防误读为记账）；receipt={r}")
            for required_suffix in ("revision", "command_id"):
                self.assertTrue(_has_key(r, required_suffix),
                                f"K4G-A7 RED: {label} 回执缺 {required_suffix}=；receipt={r}")
        self.assertTrue(_has_key(_receipt(cases["target-add"]), "target_id"))
        self.assertTrue(_has_key(_receipt(cases["target-switch"]), "target_id"))
        self.assertEqual(_receipt(cases["target-switch"]).get("balance"), "20",
                         "K4G-A7 RED: target-switch 须回显未变 balance=20（可对账事实）")
        self.assertEqual(_receipt(cases["target-delete"]).get("deleted"), "true",
                         "K4G-A7 RED: target-delete 回执须 deleted=true")

    def test_k4g_a7_usage_exit_2_and_semantic_exit_1(self):
        """coverage: K4G-A7 / S-F6.4-AC4 —— 未知/重复/缺值/缺必填旗标 exit 2；语义拒绝 exit 1 + stderr + 无成功回执。"""
        state, tid = self._seeded()
        usage_cases = [
            ["target-add", "--name", "x", "--points", "10", "--command-id", "c", "--bogus", "1"],
            ["target-add", "--name", "x", "--name", "y", "--points", "10", "--command-id", "c"],
            ["target-switch", "--target-id"],
            ["target-add", "--name", "x", "--command-id", "c"],
            ["target-delete", "--target-id", tid, "--index", "1", "--command-id", "c"],
        ]
        for args in usage_cases:
            before = _snapshot(state)
            proc = _run_cli(args, state)
            self.assertEqual(
                proc.returncode, 2,
                f"K4G-A7 RED: 用法错误 {args} 应 exit 2；实得 {proc.returncode} "
                f"stdout={proc.stdout!r} stderr={proc.stderr!r}")
            self.assertTrue(proc.stderr.strip(), f"K4G-A7 RED: {args} usage 说明应写 stderr")
            self.assertEqual(_snapshot(state), before, f"K4G-A7 RED: {args} 零写入")
        semantic_cases = [
            ["target-add", "--name", "   ", "--points", "10", "--command-id", "c-empty"],
            ["target-add", "--name", "x", "--points", "4", "--command-id", "c-low"],
            ["target-add", "--name", "x", "--points", "201", "--command-id", "c-high"],
            ["target-switch", "--target-id", "target-deadbeef", "--command-id", "c-unk"],
            ["target-delete", "--target-id", "preset-playground", "--command-id", "c-pre"],
        ]
        for args in semantic_cases:
            before = _snapshot(state)
            proc = _run_cli(args, state)
            self.assertEqual(
                proc.returncode, 1,
                f"K4G-A7 RED: 语义拒绝 {args} 应 exit 1；实得 {proc.returncode} "
                f"stdout={proc.stdout!r} stderr={proc.stderr!r}")
            self.assertTrue(proc.stderr.strip(), f"K4G-A7 RED: {args} 拒绝说明应写 stderr")
            self.assertNotIn("revision=", proc.stdout,
                             f"K4G-A7 RED: {args} 拒绝却输出成功回执；stdout={proc.stdout!r}")
            self.assertEqual(_snapshot(state), before, f"K4G-A7 RED: {args} 零写入")


# =========================================================================== #
# K4G-A8 — command_id 幂等                                                      #
# =========================================================================== #


class K4gA8Idempotency(unittest.TestCase):
    maxDiff = None

    def test_k4g_a8_target_command_id_idempotent(self):
        """coverage: K4G-A8 / S-F6.5-AC2、S-F6.4-AC4 —— 同 id 重放返首次结果、不新增目标、不重复 bump revision。"""
        pts = _points_module()
        state = _new_state_path(self)
        _add_seed(state, 10, "seed", "s0")
        cmd = ["target-add", "--name", "看海", "--points", "60", "--command-id", "tadd-idem"]
        p1 = _run_cli(cmd, state)
        self.assertEqual(p1.returncode, 0, f"K4G-A8 RED: 首次 target-add 应 exit 0；{p1.stderr!r}")
        tid = _receipt(p1).get("target_id")
        st1 = pts.read_state(str(state))
        p2 = _run_cli(cmd, state)
        self.assertEqual(p2.returncode, 0, f"K4G-A8 RED: 重放应 exit 0；{p2.stderr!r}")
        self.assertEqual(_receipt(p2).get("target_id"), tid,
                         "K4G-A8 RED: 重放须返回首次 target_id")
        st2 = pts.read_state(str(state))
        self.assertEqual(len(st2["custom_targets"]), len(st1["custom_targets"]),
                         "K4G-A8 RED: 重放不得新增自定义目标")
        self.assertEqual(st2["revision"], st1["revision"],
                         "K4G-A8 RED: 重放不得重复 bump revision")
        # switch 重放：revision 不再增长
        sw = ["target-switch", "--target-id", tid, "--command-id", "tsw-idem"]
        self.assertEqual(_run_cli(sw, state).returncode, 0, "K4G-A8 RED: 首次 switch 应成功")
        st3 = pts.read_state(str(state))
        replay = _run_cli(sw, state)
        self.assertEqual(replay.returncode, 0, f"K4G-A8 RED: switch 重放应 exit 0；{replay.stderr!r}")
        self.assertEqual(_receipt(replay).get("balance"), str(st3["balance"]),
                         "K4G-A8 RED: switch 重放回执 balance 应与首次一致")
        self.assertEqual(pts.read_state(str(state))["revision"], st3["revision"],
                         "K4G-A8 RED: switch 重放不得重复 bump revision")


# =========================================================================== #
# K4G-A9 — SKILL.md 目标配置纪律                                                 #
# =========================================================================== #


class K4gA9SkillTargetManagement(unittest.TestCase):
    maxDiff = None

    def setUp(self):
        self.assertTrue(
            POINTS_SKILL_PATH.is_file(),
            f"K4G-A9 RED: kidpal-points-write SKILL.md 不在位 —— 要求 {POINTS_SKILL_PATH}。")
        self.text = POINTS_SKILL_PATH.read_text(encoding="utf-8")
        self.assertTrue(self.text.strip(), f"K4G-A9 RED: {POINTS_SKILL_PATH} 为空。")

    def test_k4g_a9_skill_md_target_management(self):
        """coverage: K4G-A9 / S-F6.5-AC2~AC4、S-F6.4-AC4 —— 旧硬阻断句消失、frontmatter/§2/§5 覆盖目标管理。"""
        # 1) §8 旧硬阻断句必须不存在（存在 = QQ 侧必然拒答目标配置）
        for banned in ("不在本 skill 面", "归目标配置通道", "这轮没接"):
            self.assertNotIn(
                banned, self.text,
                f"K4G-A9 RED: SKILL.md 仍含旧硬阻断句 {banned!r} —— 模型会据此拒答目标配置。")
        # 2) frontmatter description 覆盖目标管理
        parts = self.text.split("---")
        self.assertGreaterEqual(len(parts), 3, "K4G-A9 RED: SKILL.md 缺 frontmatter")
        frontmatter = parts[1]
        self.assertIn("目标", frontmatter,
                      "K4G-A9 RED: frontmatter description 未覆盖目标管理")
        self.assertTrue(
            ("新增" in frontmatter and ("切换" in frontmatter or "删除" in frontmatter)),
            "K4G-A9 RED: frontmatter description 须覆盖目标的增/切/删（至少含新增与切换或删除）")
        # 3) §2 子命令表含三个 target 动词（扁平动词形态）
        for verb in ("target-add", "target-switch", "target-delete"):
            self.assertIn(verb, self.text,
                          f"K4G-A9 RED: SKILL.md 子命令表缺 {verb}（扁平动词形态）")
        # 4) §5 一次性报错表含四类目标错误
        error_facts = (
            ("删预置", r"预置[^\n]{0,60}(不可删|不能删|禁止删|删不)"),
            ("删激活中自定义（先切换）", r"激活[^\n]{0,80}(先切换|切换到|先切)"),
            ("分值出 5–200", r"5\s*[–\-~]\s*200"),
            ("目标名空", r"目标名[^\n]{0,40}(空|不空|不能为空|不得为空)"),
        )
        missing = [label for label, pat in error_facts if not re.search(pat, self.text)]
        self.assertEqual(
            missing, [],
            f"K4G-A9 RED: §5 报错表缺目标错误类 —— {missing}（须为四类各给一次性回复话术）")
        # 5) 「切目标前先用 targets 拿 id」纪律
        self.assertIn("targets", self.text,
                      "K4G-A9 RED: SKILL.md 未提只读 targets 列举")
        discipline = (
            re.search(r"(不要|不得|禁止)[^\n]{0,20}(自编|自己编|编造|猜)[^\n]{0,20}(target\s*id|目标\s*id)", self.text)
            or re.search(r"(切|target-switch)[^\n]{0,80}targets[^\n]{0,80}(拿|取|获)", self.text)
            or re.search(r"targets[^\n]{0,80}(拿|取)[^\n]{0,40}(target\s*id|目标\s*id)", self.text)
        )
        self.assertTrue(
            discipline,
            "K4G-A9 RED: SKILL.md 未写死「切目标前先用 targets 拿 id、不得自编 target id」纪律。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)