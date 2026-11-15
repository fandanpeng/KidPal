"""KidPal 261104-k4-reward-goal-config 服务端单元测试 — 目标配置 CLI 面增量
（分支、边界与错误路径；不只 happy path）。

与冻结验收（server/acceptance/test_reward_goal_config.acceptance.py）的分工：冻结件是
**规格**；本文件是实现自带测，覆盖规格未逐字段钉的实现内部边界——

  - target-add / target-switch / target-delete 三回执的逐字段形状（单行 key=value、
    禁带 delta=/entry_id=、target_id 生成形态、switch 回显 balance）；
  - 退出码分界：usage（未知/重复/缺值/缺必填旗标）= 2，语义拒绝（分值出界 / 目标名空 /
    未知 id / 删预置 / 删激活中自定义）= 1，且两条路径都零写入；
  - 传输幂等：add/switch 同 command_id 重放返首次结果、不新增、不重复 bump revision；
    delete 重放即使目标已不在也返首次 deleted=true；kind 不符不算同一命令的重放；
  - 目标命令回执的命名空间前缀（"target:"）与病态撞键守卫（不覆盖分录回执）；
  - 切目标 / 删目标对账本分录零触碰（余额、分录、历史 reason 快照都不动）；
  - read_state 的 K4 additive 两键（custom_targets / active_target）；
  - API 层（add_custom_target / set_active_target / delete_target）的 command_id 参数。

纯 stdlib + 按路径导入，零网络、零 ECS 依赖，任何 python3 可跑：
  cd server && python -m pytest -q tests/test_points_goal_units.py
"""

from __future__ import annotations

import io
import json
import os
import re
import sys
import tempfile
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
if str(SERVER) not in sys.path:
    sys.path.insert(0, str(SERVER))

import gateway.points as points  # noqa: E402

TARGET_ID_RE = re.compile(r"^target-[0-9a-f]+$")


def _parse_kv(line: str) -> dict:
    out = {}
    for tok in line.split():
        if "=" in tok:
            key, val = tok.split("=", 1)
            out[key] = val
    out["_line"] = line.strip()
    return out


class _StateDirMixin(unittest.TestCase):
    def setUp(self):
        self._dir = tempfile.TemporaryDirectory(prefix="k4g_unit_")
        self.addCleanup(self._dir.cleanup)
        self.state = os.path.join(self._dir.name, "state.json")

    # ---- CLI 助手（进程内走 points.main，env 注入与生产同一单源） ----
    def cli(self, args):
        old = os.environ.get(points.POINTS_STATE_ENV)
        os.environ[points.POINTS_STATE_ENV] = self.state
        out, err = io.StringIO(), io.StringIO()
        try:
            with redirect_stdout(out), redirect_stderr(err):
                rc = points.main(list(args))
        finally:
            if old is None:
                os.environ.pop(points.POINTS_STATE_ENV, None)
            else:
                os.environ[points.POINTS_STATE_ENV] = old
        return rc, out.getvalue(), err.getvalue()

    def receipt(self, args):
        rc, out, err = self.cli(args)
        self.assertEqual(rc, 0, f"{args} 应成功；stdout={out!r} stderr={err!r}")
        lines = [ln for ln in out.splitlines() if ln.strip()]
        self.assertEqual(len(lines), 1, f"回执须恰一行；stdout={out!r}")
        return _parse_kv(lines[0])

    def add_seed(self, value=10, reason="seed", cid="seed-0"):
        rc, out, err = self.cli(
            ["add", "--points", str(value), "--reason", reason, "--command-id", cid])
        self.assertEqual(rc, 0, f"seed add 失败；stdout={out!r} stderr={err!r}")

    def bytes_or_none(self):
        p = Path(self.state)
        return p.read_bytes() if p.exists() else None


# --------------------------------------------------------------------------- #
# 回执形状                                                                      #
# --------------------------------------------------------------------------- #


class TargetReceiptShapeTests(_StateDirMixin):
    def test_add_receipt_fields(self):
        r = self.receipt(
            ["target-add", "--name", "看海", "--points", "60", "--command-id", "add-1"])
        self.assertRegex(r["target_id"], TARGET_ID_RE)
        self.assertEqual(r["name"], "看海")
        self.assertEqual(r["points"], "60")
        self.assertEqual(r["revision"], "1")
        self.assertEqual(r["command_id"], "add-1")
        for banned in ("delta", "entry_id"):
            self.assertNotIn(banned, r, f"target 回执禁带 {banned}=（防误读为记账）")

    def test_switch_receipt_fields_and_balance(self):
        self.add_seed(20, "攒分", "s0")
        self.receipt(["target-add", "--name", "看海", "--points", "60", "--command-id", "a1"])
        custom = points.list_targets(self.state)[-1]
        r = self.receipt(
            ["target-switch", "--target-id", custom["id"], "--command-id", "sw-1"])
        self.assertEqual(r["target_id"], custom["id"])
        self.assertEqual(r["active_target_id"], custom["id"])
        self.assertEqual(r["balance"], "20", "switch 回执须回显未变 balance（切目标不扣分）")
        for banned in ("delta", "entry_id"):
            self.assertNotIn(banned, r)

    def test_delete_receipt_fields(self):
        a = self.receipt(["target-add", "--name", "甲", "--points", "20", "--command-id", "a1"])
        self.receipt(["target-add", "--name", "乙", "--points", "30", "--command-id", "a2"])
        self.receipt(["target-switch", "--target-id", a["target_id"], "--command-id", "sw"])
        b_id = [t["id"] for t in points.list_targets(self.state) if t["name"] == "乙"][0]
        r = self.receipt(["target-delete", "--target-id", b_id, "--command-id", "del-1"])
        self.assertEqual(r["target_id"], b_id)
        self.assertEqual(r["deleted"], "true")
        self.assertIn("revision", r)
        for banned in ("delta", "entry_id"):
            self.assertNotIn(banned, r)


# --------------------------------------------------------------------------- #
# 退出码分界（usage=2 / 语义=1，两路零写入）                                      #
# --------------------------------------------------------------------------- #


class TargetExitCodeTests(_StateDirMixin):
    def test_usage_exit_2_zero_write(self):
        self.add_seed()
        before = self.bytes_or_none()
        cases = [
            ["target-add", "--name", "x", "--points", "10", "--command-id", "c", "--bogus", "1"],
            ["target-add", "--name", "x", "--name", "y", "--points", "10", "--command-id", "c"],
            ["target-add", "--name", "x", "--command-id", "c"],          # 缺 --points
            ["target-switch", "--target-id"],                            # 旗标缺值
            ["target-delete", "--target-id", "x", "--index", "1", "--command-id", "c"],
            ["target-delete", "--command-id"],                           # 缺值
        ]
        for args in cases:
            rc, out, err = self.cli(args)
            self.assertEqual(rc, 2, f"{args} 应 exit 2；stdout={out!r} stderr={err!r}")
            self.assertTrue(err.strip(), f"{args} usage 说明应写 stderr")
            self.assertNotIn("revision=", out, f"{args} 用法错误不得输出成功回执")
            self.assertEqual(self.bytes_or_none(), before, f"{args} 零写入")

    def test_semantic_exit_1_zero_write(self):
        self.add_seed()
        before = self.bytes_or_none()
        cases = [
            ["target-add", "--name", "   ", "--points", "10", "--command-id", "c1"],
            ["target-add", "--name", "x", "--points", "4", "--command-id", "c2"],
            ["target-add", "--name", "x", "--points", "201", "--command-id", "c3"],
            ["target-add", "--name", "x", "--points", "0", "--command-id", "c4"],
            ["target-add", "--name", "x", "--points", "abc", "--command-id", "c5"],
            ["target-switch", "--target-id", "target-deadbeef", "--command-id", "c6"],
            ["target-delete", "--target-id", "preset-playground", "--command-id", "c7"],
        ]
        for args in cases:
            rc, out, err = self.cli(args)
            self.assertEqual(rc, 1, f"{args} 应语义拒绝 exit 1；stdout={out!r} stderr={err!r}")
            self.assertTrue(err.strip(), f"{args} 拒绝说明应写 stderr")
            self.assertNotIn("revision=", out, f"{args} 拒绝却输出成功回执")
            self.assertEqual(self.bytes_or_none(), before, f"{args} 零写入")

    def test_delete_active_target_refused_with_switch_hint(self):
        a = self.receipt(["target-add", "--name", "甲", "--points", "20", "--command-id", "a1"])
        self.receipt(["target-switch", "--target-id", a["target_id"], "--command-id", "sw"])
        before = self.bytes_or_none()
        rc, out, err = self.cli(
            ["target-delete", "--target-id", a["target_id"], "--command-id", "del"])
        self.assertEqual(rc, 1)
        self.assertIn("切换", err, "拒绝说明须指向「先切换到其他目标再删」")
        self.assertNotIn("deleted=true", out)
        self.assertEqual(self.bytes_or_none(), before)


# --------------------------------------------------------------------------- #
# 传输幂等                                                                      #
# --------------------------------------------------------------------------- #


class TargetIdempotencyTests(_StateDirMixin):
    def test_add_replay_returns_first_target(self):
        args = ["target-add", "--name", "看海", "--points", "60", "--command-id", "same"]
        first = self.receipt(args)
        st1 = points.read_state(self.state)
        replay = self.receipt(args)
        st2 = points.read_state(self.state)
        self.assertEqual(replay["target_id"], first["target_id"])
        self.assertEqual(replay["revision"], first["revision"])
        self.assertEqual(len(st2["custom_targets"]), len(st1["custom_targets"]))
        self.assertEqual(st2["revision"], st1["revision"], "重放不得重复 bump revision")

    def test_add_replay_with_different_payload_keeps_first(self):
        first = self.receipt(
            ["target-add", "--name", "看海", "--points", "60", "--command-id", "same"])
        replay = self.receipt(
            ["target-add", "--name", "别的名字", "--points", "80", "--command-id", "same"])
        self.assertEqual(replay["target_id"], first["target_id"])
        self.assertEqual(replay["name"], "看海")
        self.assertEqual(replay["points"], "60")

    def test_switch_replay_no_revision_bump(self):
        a = self.receipt(["target-add", "--name", "甲", "--points", "20", "--command-id", "a1"])
        args = ["target-switch", "--target-id", a["target_id"], "--command-id", "sw"]
        first = self.receipt(args)
        st1 = points.read_state(self.state)
        replay = self.receipt(args)
        st2 = points.read_state(self.state)
        self.assertEqual(replay["balance"], first["balance"])
        self.assertEqual(replay["revision"], first["revision"])
        self.assertEqual(st2["revision"], st1["revision"])

    def test_delete_replay_after_target_gone(self):
        self.receipt(["target-add", "--name", "甲", "--points", "20", "--command-id", "a1"])
        b = self.receipt(["target-add", "--name", "乙", "--points", "30", "--command-id", "a2"])
        first = self.receipt(
            ["target-delete", "--target-id", b["target_id"], "--command-id", "del"])
        st1 = points.read_state(self.state)
        replay = self.receipt(
            ["target-delete", "--target-id", b["target_id"], "--command-id", "del"])
        self.assertEqual(replay["deleted"], "true")
        self.assertEqual(replay["target_id"], b["target_id"])
        self.assertEqual(replay["revision"], first["revision"])
        self.assertEqual(points.read_state(self.state)["revision"], st1["revision"])

    def test_kind_mismatch_is_not_a_replay(self):
        # 同一 command_id 先当 target-add 用，再当 target-switch 用：kind 不符 = 新命令。
        self.receipt(["target-add", "--name", "甲", "--points", "20", "--command-id", "shared"])
        st0 = points.read_state(self.state)
        r = self.receipt(
            ["target-switch", "--target-id", "preset-movie", "--command-id", "shared"])
        self.assertEqual(r["target_id"], "preset-movie")
        self.assertEqual(points.read_state(self.state)["active_target"], "preset-movie")
        self.assertEqual(points.read_state(self.state)["revision"], st0["revision"] + 1)

    def test_target_receipt_does_not_overwrite_entry_receipt(self):
        # 病态撞键：裸 command_id 恰好以 "target:" 开头 → 目标回执的键与该分录回执同键，
        # 目标回执降级为不持久，但**绝不覆盖**分录回执（add 的传输幂等优先）。
        self.add_seed(10, "seed", "target:seed")
        raw = json.loads(Path(self.state).read_text(encoding="utf-8"))
        entry_receipt_id = raw["receipts"]["target:seed"]["id"]
        r = self.receipt(["target-add", "--name", "甲", "--points", "20", "--command-id", "seed"])
        raw = json.loads(Path(self.state).read_text(encoding="utf-8"))
        self.assertEqual(raw["receipts"]["target:seed"]["id"], entry_receipt_id,
                         "分录回执被目标回执覆盖了（传输幂等优先级反转）")
        self.assertNotIn("kind", raw["receipts"]["target:seed"])
        # 目标本身照常创建（降级的只是回执持久化，不是写入）。
        self.assertIn(r["target_id"], {t["id"] for t in raw["targets"]})


# --------------------------------------------------------------------------- #
# 零分录不变量 + read_state additive 键                                          #
# --------------------------------------------------------------------------- #


class TargetInvariantTests(_StateDirMixin):
    def test_target_ops_never_touch_entries(self):
        self.add_seed(50, "攒分", "s0")
        rc, _, _ = self.cli(["redeem", "--points", "40", "--command-id", "rd"])
        self.assertEqual(rc, 0)
        st0 = points.read_state(self.state)
        self.receipt(["target-add", "--name", "看海", "--points", "60", "--command-id", "a1"])
        self.receipt(["target-switch", "--target-id", "preset-choco", "--command-id", "sw"])
        st1 = points.read_state(self.state)
        self.assertEqual(st1["entries"], st0["entries"], "目标操作不得改任何分录")
        self.assertEqual(st1["balance"], st0["balance"], "目标操作不得改余额")
        self.assertEqual(
            [e["reason"] for e in st1["entries"] if e["delta"] < 0],
            ["兑换游乐场"], "历史兑换名目快照不得被切换改写")

    def test_read_state_exposes_additive_target_keys(self):
        st = points.read_state(self.state)
        self.assertEqual(st["custom_targets"], [])
        self.assertEqual(st["active_target"], points.DEFAULT_TARGET["id"])

    def test_targets_command_is_read_only(self):
        rc, out, err = self.cli(["targets"])
        self.assertEqual(rc, 0, err)
        self.assertEqual(len(out.splitlines()), len(points.PRESET_TARGETS))
        self.assertFalse(Path(self.state).exists(), "只读列举不得创建 state 文件")


# --------------------------------------------------------------------------- #
# API 层 command_id（写函数返回形状不变）                                          #
# --------------------------------------------------------------------------- #


class TargetApiIdempotencyTests(_StateDirMixin):
    def test_api_add_replay(self):
        a = points.add_custom_target(self.state, "看海", 60, command_id="api-add")
        b = points.add_custom_target(self.state, "看海", 60, command_id="api-add")
        self.assertEqual(a, b)
        self.assertEqual(len(points.read_state(self.state)["custom_targets"]), 1)

    def test_api_switch_replay_keeps_four_key_shape(self):
        points.add_custom_target(self.state, "看海", 60, command_id="api-add")
        tid = points.read_state(self.state)["custom_targets"][0]["id"]
        first = points.set_active_target(self.state, tid, command_id="api-sw")
        revision = points.read_state(self.state)["revision"]
        replay = points.set_active_target(self.state, tid, command_id="api-sw")
        self.assertEqual(first, replay)
        self.assertEqual(set(first), {"id", "name", "points", "asset"},
                         "set_active_target 返回形状须保持 K1 四键（additive revision 不上浮）")
        self.assertEqual(points.read_state(self.state)["revision"], revision)

    def test_api_delete_replay(self):
        points.add_custom_target(self.state, "甲", 20, command_id="api-a")
        points.add_custom_target(self.state, "乙", 30, command_id="api-b")
        tid = [t["id"] for t in points.read_state(self.state)["custom_targets"]
               if t["name"] == "乙"][0]
        first = points.delete_target(self.state, tid, command_id="api-del")
        replay = points.delete_target(self.state, tid, command_id="api-del")
        self.assertEqual(first["deleted"], True)
        self.assertEqual(replay["deleted"], True)
        self.assertEqual(replay["revision"], first["revision"])


if __name__ == "__main__":
    unittest.main(verbosity=2)