"""KidPal 261104-k3-qq-points-channel 服务端单元测试 — QQ 家长通道 CLI 面增量
（回执逐字段精确 / 拒绝零写入 / 稳定定位删除的锁内性 / 两类幂等）。

与冻结验收（server/acceptance/test_qq_points_channel.acceptance.py）的分工：冻结件是
**规格**；本文件是实现自带测，覆盖规格未逐字段钉死的地方——
  ① 回执**键集完整相等**（多一个键、少一个键、改名都红），不是「包含某字段」级；
  ② 拒绝路径四联断言：exit 1 + stderr 说明 + stdout 无成功回执 + state 文件**字节不变**；
  ③ 序号→id 换算与删除**共用同一次 state 读且在文件锁内**（TOCTOU 删错账的行为级证明，
     冻结件只做源锚代理）；
  ④ 事由含空白时回执仍是**单行单 token**（skill 的解析口径依赖它）；
  ⑤ recent 的 limit 边界与默认截断、show/targets 的只读不建文件。

纯 stdlib + 包导入，零网络、零 ECS 依赖：
  cd server && python -m pytest -q tests/test_points_cli_units.py
"""

from __future__ import annotations

import contextlib
import inspect
import io
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

SERVER = Path(__file__).resolve().parents[1]
if str(SERVER) not in sys.path:
    sys.path.insert(0, str(SERVER))

import gateway.points as pts  # noqa: E402

STATE_ENV = pts.POINTS_STATE_ENV


# --------------------------------------------------------------------------- #
# 工具                                                                          #
# --------------------------------------------------------------------------- #


def _parse_kv(line: str) -> dict:
    """回执行 -> {key: value}（与 skill / 冻结验收同一解析口径：空白分词 + 首个 '=' 切）。"""
    out = {}
    for token in line.split():
        if "=" in token:
            key, value = token.split("=", 1)
            out[key] = value
    return out


class _CliCase(unittest.TestCase):
    """一次 CLI 调用 = 一个进程内 main()（env 注入 + stdout/stderr 捕获）。"""

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory(prefix="k3q_unit_")
        self.addCleanup(self._tmp.cleanup)
        self.state_path = str(Path(self._tmp.name) / "state.json")

    # -- CLI --------------------------------------------------------------- #
    def cli(self, argv, state_path="__default__"):
        target = self.state_path if state_path == "__default__" else state_path
        old = os.environ.get(STATE_ENV)
        if target is None:
            os.environ.pop(STATE_ENV, None)
        else:
            os.environ[STATE_ENV] = str(target)
        out, err = io.StringIO(), io.StringIO()
        try:
            with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
                rc = pts.main(argv)
        finally:
            if old is None:
                os.environ.pop(STATE_ENV, None)
            else:
                os.environ[STATE_ENV] = old
        return rc, out.getvalue(), err.getvalue()

    # -- 断言助手 ----------------------------------------------------------- #
    def ok(self, argv):
        rc, out, err = self.cli(argv)
        self.assertEqual(rc, 0, f"{argv} 应 exit 0；stdout={out!r} stderr={err!r}")
        return out, err

    def lines(self, out: str) -> list:
        return [_parse_kv(line) for line in out.splitlines() if line.strip()]

    def first(self, out: str) -> dict:
        rows = self.lines(out)
        self.assertTrue(rows, f"回执为空：{out!r}")
        return rows[0]

    def seed(self, points, reason, *, now=None):
        return pts.add_points(self.state_path, points, reason, now=now)

    def bytes_now(self) -> bytes:
        path = Path(self.state_path)
        return path.read_bytes() if path.exists() else b""


# --------------------------------------------------------------------------- #
# ① 回执逐字段精确                                                              #
# --------------------------------------------------------------------------- #


class ReceiptFieldTests(_CliCase):
    def test_add_receipt_exact_key_set_and_values(self):
        out, _ = self.ok(["add", "--points", "3", "--reason", "老师表扬",
                          "--command-id", "qq:20261114T101500:abc123"])
        rows = self.lines(out)
        self.assertEqual(len(rows), 1, f"add 回执应恰一行；实得 {rows}")
        receipt = rows[0]
        self.assertEqual(
            set(receipt),
            {"entry_id", "delta", "balance", "reason", "revision", "command_id"},
            f"add 回执键集不符（多/少/改名都算破坏 skill 解析口径）；实得 {receipt}")
        self.assertRegex(receipt["entry_id"], r"^pt_[0-9a-f]{32}$")
        self.assertEqual(receipt["delta"], "+3")
        self.assertEqual(receipt["balance"], "3")
        self.assertEqual(receipt["reason"], "老师表扬")
        self.assertEqual(receipt["revision"], "1")
        self.assertEqual(receipt["command_id"], "qq:20261114T101500:abc123")

    def test_add_receipt_reason_with_space_stays_single_token(self):
        out, _ = self.ok(["add", "--points", "2", "--reason", "老师 表扬",
                          "--command-id", "c-space"])
        rows = self.lines(out)
        self.assertEqual(len(rows), 1, f"含空格事由不得把回执拆成多行：{out!r}")
        self.assertEqual(len(out.strip().splitlines()), 1, out)
        self.assertEqual(rows[0]["reason"], "老师%20表扬",
                         "空白位须编码成 %XX（值本身不改写），否则回执会被拆 token")
        self.assertEqual(rows[0]["balance"], "2")

    def test_redeem_receipt_exact_key_set_and_negative_delta(self):
        self.seed(50, "攒分")
        out, _ = self.ok(["redeem", "--points", "40", "--command-id", "rd-1"])
        receipt = self.first(out)
        self.assertEqual(
            set(receipt),
            {"entry_id", "delta", "balance", "reason", "revision", "command_id"},
            f"redeem 回执键集不符；实得 {receipt}")
        self.assertEqual(receipt["delta"], "-40")
        self.assertEqual(receipt["balance"], "10")
        self.assertEqual(receipt["revision"], "2")
        self.assertEqual(receipt["reason"], "兑换游乐场")
        self.assertEqual(receipt["command_id"], "rd-1")

    def test_delete_receipt_exact_key_set_both_outcomes(self):
        newest = self.seed(1, "alpha")
        self.seed(2, "beta")
        out, _ = self.ok(["delete", "--entry-id", newest["id"], "--command-id", "d1"])
        receipt = self.first(out)
        self.assertEqual(
            set(receipt),
            {"deleted", "entry_id", "revision", "balance", "command_id"},
            f"delete 回执键集不符；实得 {receipt}")
        self.assertEqual(receipt["deleted"], "true")
        self.assertEqual(receipt["entry_id"], newest["id"])
        self.assertEqual(receipt["revision"], "3")
        self.assertEqual(receipt["balance"], "2")
        out, _ = self.ok(["delete", "--entry-id", "pt_" + "d" * 32,
                          "--command-id", "d2"])
        receipt = self.first(out)
        self.assertEqual(receipt["deleted"], "false")
        self.assertEqual(receipt["revision"], "3", "no-op 不得递增 revision")

    def test_show_receipt_exact_key_set(self):
        self.seed(6, "seed")
        out, _ = self.ok(["show"])
        receipt = self.first(out)
        self.assertEqual(
            set(receipt),
            {"balance", "revision", "entries_total",
             "active_target_id", "active_target_name", "active_target_points"},
            f"show 回执键集不符；实得 {receipt}")
        self.assertEqual(receipt["balance"], "6")
        self.assertEqual(receipt["revision"], "1")
        self.assertEqual(receipt["entries_total"], "1")
        self.assertEqual(receipt["active_target_id"], "preset-playground")
        self.assertEqual(receipt["active_target_name"], "游乐场")
        self.assertEqual(receipt["active_target_points"], "40")

    def test_targets_receipt_exact_key_set_and_presets(self):
        out, _ = self.ok(["targets"])
        rows = self.lines(out)
        self.assertEqual(len(rows), 4, f"出厂四项全在；实得 {rows}")
        for row in rows:
            self.assertEqual(set(row), {"id", "name", "points", "asset"},
                             f"target 行键集不符；实得 {row}")
        self.assertEqual([row["name"] for row in rows],
                         ["巧克力", "看电影", "游乐场", "礼物"])
        self.assertEqual([row["points"] for row in rows], ["20", "30", "40", "50"])

    def test_recent_receipt_exact_key_set_and_ordering(self):
        self.seed(1, "alpha")
        self.seed(2, "beta")
        out, _ = self.ok(["recent", "--limit", "2"])
        rows = self.lines(out)
        self.assertEqual(len(rows), 2, f"recent --limit 2 应回 2 行；实得 {rows}")
        for row in rows:
            self.assertEqual(
                set(row),
                {"index", "entry_id", "delta", "balance_before", "balance_after",
                 "reason", "occurred_at"},
                f"recent 行键集不符（index 与 entry_id 必须同行）；实得 {row}")
        self.assertEqual(rows[0]["reason"], "beta", "index 1 = 时间倒序最新")
        self.assertEqual(rows[0]["index"], "1")
        self.assertEqual(rows[0]["delta"], "+2")
        self.assertEqual((rows[0]["balance_before"], rows[0]["balance_after"]), ("1", "3"))
        self.assertEqual(rows[1]["index"], "2")
        self.assertEqual(rows[1]["reason"], "alpha")

    def test_recent_default_limit_truncates_to_five(self):
        for position in range(7):
            self.seed(1, f"r{position}")
        out, _ = self.ok(["recent"])
        self.assertEqual(len(self.lines(out)), pts.RECENT_LIMIT_DEFAULT,
                         "recent 缺省应只回最近 5 条")

    def test_recent_empty_ledger_outputs_nothing(self):
        out, err = self.ok(["recent"])
        self.assertEqual(out.strip(), "", f"空账 recent 不伪造分录；stdout={out!r}")
        self.assertEqual(err, "")


# --------------------------------------------------------------------------- #
# ② 拒绝路径：exit 1 + stderr + 无成功回执 + state 字节不变                        #
# --------------------------------------------------------------------------- #


class RejectPathTests(_CliCase):
    #: 成功回执的两种指纹（add/redeem/delete 的成功行各带其一）。
    _SUCCESS_MARKERS = ("ok entry_id=", "deleted=true", "duplicate=true")

    def assert_rejected(self, argv, *, before: bytes, note: str = ""):
        rc, out, err = self.cli(argv)
        self.assertEqual(rc, 1, f"{note} {argv} 应 exit 1；stdout={out!r} stderr={err!r}")
        self.assertTrue(err.strip(), f"{note} {argv} 拒绝说明应写 stderr")
        for marker in self._SUCCESS_MARKERS:
            self.assertNotIn(marker, out,
                             f"{note} {argv} 拒绝却输出成功回执；stdout={out!r}")
        self.assertEqual(self.bytes_now(), before,
                         f"{note} {argv} 拒绝不得改写 state（零写入）")
        return err

    def test_invalid_add_values_zero_write(self):
        cases = (
            ["add", "--points", "0", "--reason", "x", "--command-id", "c0"],
            ["add", "--points", "-3", "--reason", "x", "--command-id", "cneg"],
            ["add", "--points", "1.5", "--reason", "x", "--command-id", "cfloat"],
            ["add", "--points", "abc", "--reason", "x", "--command-id", "cabc"],
            ["add", "--points", "3", "--reason", "   ", "--command-id", "cblank"],
        )
        for argv in cases:
            self.seed(5, "seed")
            before = self.bytes_now()
            self.assert_rejected(argv, before=before)

    def test_valid_but_absent_command_id_is_usage_error(self):
        """旗标形 add 缺 --command-id = 用法错误 exit 2（不是语义拒绝）。"""
        before = self.bytes_now()
        rc, out, err = self.cli(["add", "--points", "3", "--reason", "x"])
        self.assertEqual(rc, 2, f"缺 --command-id 应 exit 2；out={out!r} err={err!r}")
        self.assertIn("usage", err)
        self.assertEqual(self.bytes_now(), before)

    def test_usage_errors_exit_2_and_zero_write(self):
        self.seed(5, "seed")
        before = self.bytes_now()
        cases = (
            [],                                                        # 无子命令
            ["frobnicate"],                                            # 未知子命令
            ["show", "extra"],                                         # 无参命令多余参数
            ["targets", "--limit", "3"],
            ["add", "--points", "3", "--points", "4", "--reason", "x",
             "--command-id", "dup"],                                   # 重复旗标
            ["add", "3", "r", "extra"],                                # 位置形多余参数
            ["redeem"],                                                # 旗标形缺值
            ["delete", "--amount", "5", "--command-id", "d7"],          # 未登记选项
        )
        for argv in cases:
            rc, out, err = self.cli(argv)
            self.assertEqual(rc, 2, f"{argv} 应 exit 2；stdout={out!r} stderr={err!r}")
            self.assertIn("usage", err, f"{argv} 用法错误须回 usage")
            self.assertEqual(self.bytes_now(), before, f"{argv} 用法错误零写入")

    def test_missing_env_exit_2_and_no_state_file(self):
        rc, out, err = self.cli(["show"], state_path=None)
        self.assertEqual(rc, 2, f"缺 {STATE_ENV} 应 exit 2；err={err!r}")
        self.assertIn(STATE_ENV, err)
        self.assertFalse(Path(self.state_path).exists(), "缺 env 不得创建账本文件")

    def test_delete_locator_ambiguity_zero_write(self):
        self.seed(5, "seed")
        before = self.bytes_now()
        cases = (
            ["delete", "--command-id", "d0"],                                  # 都缺
            ["delete", "--entry-id", "pt_" + "d" * 32, "--index", "1",
             "--command-id", "d1"],                                            # 都给
            ["delete", "--index", "0", "--command-id", "d2"],                  # 非正整数
            ["delete", "--index", "abc", "--command-id", "d3"],                # 非整数
            ["delete", "--index", "9", "--command-id", "d4"],                  # 越界
            ["delete", "--entry-id", "   ", "--command-id", "d5"],             # 空白 id
            ["delete", "--entry-id", "hello", "--command-id", "d6"],           # 畸形 id
            ["delete", "--entry-id", "PT_" + "d" * 32, "--command-id", "d8"],  # 大小写
        )
        for argv in cases:
            self.assert_rejected(argv, before=before)

    def test_delete_shape_reject_leaves_no_lock_side_effect(self):
        """定位符形状拒绝在进锁前挡下：拒绝路径连 `.lock` 都不该新造。"""
        with tempfile.TemporaryDirectory(prefix="k3q_nolock_") as tmp:
            state = str(Path(tmp) / "state.json")
            for argv in (["delete", "--command-id", "d0"],
                         ["delete", "--entry-id", "hello", "--command-id", "d6"]):
                rc, _out, err = self.cli(argv, state_path=state)
                self.assertEqual(rc, 1, err)
            self.assertEqual(sorted(p.name for p in Path(tmp).iterdir()), [],
                             "形状拒绝不得产生 .lock / state.json 等文件副作用")

    def test_redeem_gate_rejects_zero_write(self):
        self.seed(10, "零钱")
        before = self.bytes_now()
        self.assert_rejected(["redeem", "--points", "40", "--command-id", "rd-short"],
                             before=before, note="余额不足")
        self.seed(30, "补足")
        before = self.bytes_now()
        self.assert_rejected(["redeem", "--points", "20", "--command-id", "rd-wrong"],
                             before=before, note="金额不符激活目标")
        self.assert_rejected(["redeem", "--points", "0", "--command-id", "rd-zero"],
                             before=before, note="非正整数")

    def test_recent_limit_bounds_rejected(self):
        self.seed(1, "seed")
        before = self.bytes_now()
        for raw in ("0", "51", "abc", "-1"):
            self.assert_rejected(["recent", "--limit", raw], before=before)


# --------------------------------------------------------------------------- #
# ③ 稳定定位删除：序号→id 换算与删除同一临界区（TOCTOU 的行为级证明）              #
# --------------------------------------------------------------------------- #


class DeleteLocatorTests(_CliCase):
    def _three(self):
        return [self.seed(1, "alpha"), self.seed(2, "beta"), self.seed(3, "gamma")]

    def test_index_resolution_and_delete_share_one_in_locked_read(self):
        self._three()
        real_file_lock = pts._file_lock
        real_load = pts._load_full
        flags = {"inside_lock": False}
        loads = []

        @contextlib.contextmanager
        def spy_file_lock(path):
            with real_file_lock(path):
                flags["inside_lock"] = True
                try:
                    yield
                finally:
                    flags["inside_lock"] = False

        def spy_load(path):
            loads.append(flags["inside_lock"])
            return real_load(path)

        with mock.patch.object(pts, "_file_lock", spy_file_lock), \
                mock.patch.object(pts, "_load_full", spy_load):
            receipt = pts.delete_entry(self.state_path, index=2)

        self.assertTrue(receipt["deleted"])
        self.assertEqual(
            len(loads), 1,
            f"删除应只读一次 state（序号→id 换算与删除共用同一次读，杜绝两次读之间"
            f"漂移）；实得 {len(loads)} 次")
        self.assertTrue(all(loads), "那次读必须发生在 `.lock` 文件锁内")

    def test_delete_entry_delegates_to_locked_helper(self):
        source = inspect.getsource(pts.delete_entry)
        self.assertIn("_delete_entry_locked", source,
                      "公开 delete_entry 必须在 _critical 内转调 _delete_entry_locked")
        self.assertIn("_critical", source)

    def test_index_targets_recent_order_exactly(self):
        entries = self._three()
        rows = self.lines(self.ok(["recent", "--limit", "3"])[0])
        # index 1 = 最新（gamma），index 3 = 最早（alpha）——与 recent 回执逐条对齐。
        mapping = {row["index"]: row["entry_id"] for row in rows}
        self.assertEqual(mapping["1"], entries[2]["id"])
        self.assertEqual(mapping["2"], entries[1]["id"])
        self.assertEqual(mapping["3"], entries[0]["id"])
        for index, expected in (("3", entries[0]["id"]), ("1", entries[2]["id"])):
            receipt = self.first(self.ok(
                ["delete", "--index", index, "--command-id", f"del-{index}"])[0])
            self.assertEqual(receipt["entry_id"], expected,
                             f"--index {index} 删错分录")

    def test_index_out_of_range_after_deletions_zero_write(self):
        self._three()
        self.ok(["delete", "--index", "3", "--command-id", "d3"])   # 只剩 2 条
        before = self.bytes_now()
        rc, out, err = self.cli(["delete", "--index", "3", "--command-id", "d3b"])
        self.assertEqual(rc, 1, f"越界应 exit 1；out={out!r} err={err!r}")
        self.assertNotIn("deleted=true", out)
        self.assertEqual(self.bytes_now(), before)

    def test_entry_id_noop_is_idempotent_and_byte_identical(self):
        self._three()
        before = self.bytes_now()
        receipt = self.first(self.ok(
            ["delete", "--entry-id", "pt_" + "d" * 32, "--command-id", "noop"])[0])
        self.assertEqual(receipt["deleted"], "false")
        self.assertEqual(self.bytes_now(), before, "幂等 no-op 必须零写入")
        # 同一 id 连续删两次：第二次仍是 no-op（不报错、不再动账本）。
        first = self.seed(4, "delta")
        self.ok(["delete", "--entry-id", first["id"], "--command-id", "x1"])
        before2 = self.bytes_now()
        receipt2 = self.first(self.ok(
            ["delete", "--entry-id", first["id"], "--command-id", "x2"])[0])
        self.assertEqual(receipt2["deleted"], "false")
        self.assertEqual(self.bytes_now(), before2)


# --------------------------------------------------------------------------- #
# ④ 两类幂等（传输 command_id 重放 / 短窗语义去重）                               #
# --------------------------------------------------------------------------- #


class IdempotencyTests(_CliCase):
    def test_command_id_replay_returns_first_result(self):
        argv = ["add", "--points", "4", "--reason", "洗碗", "--command-id", "qq:a:1"]
        first = self.first(self.ok(argv)[0])
        second = self.first(self.ok(argv)[0])
        self.assertEqual(second["entry_id"], first["entry_id"])
        self.assertEqual(second["revision"], first["revision"])
        self.assertEqual(second["balance"], first["balance"])
        state = pts.read_state(self.state_path)
        self.assertEqual((len(state["entries"]), state["revision"]), (1, 1),
                         "重放不得新增分录、不得递增 revision")
        self.assertNotIn("duplicate", second,
                         "传输幂等重放 ≠ 短窗去重（不得标 duplicate）")

    def test_command_id_replay_after_later_write_still_first_entry(self):
        argv = ["add", "--points", "4", "--reason", "洗碗", "--command-id", "qq:a:1"]
        first = self.first(self.ok(argv)[0])
        self.ok(["add", "--points", "2", "--reason", "别的", "--command-id", "qq:a:2"])
        replay = self.first(self.ok(argv)[0])
        self.assertEqual(replay["entry_id"], first["entry_id"])
        self.assertEqual(replay["revision"], "2", "revision 取当前权威值")
        self.assertEqual(len(pts.read_state(self.state_path)["entries"]), 2)

    def test_short_window_duplicate_is_receipt_not_error(self):
        self.ok(["add", "--points", "3", "--reason", "扫地", "--command-id", "qq:b:1"])
        out, _ = self.ok(["add", "--points", "3", "--reason", "扫地",
                          "--command-id", "qq:b:2"])
        receipt = self.first(out)
        self.assertEqual(receipt["duplicate"], "true")
        self.assertEqual(receipt["revision"], "1")
        state = pts.read_state(self.state_path)
        self.assertEqual((len(state["entries"]), state["revision"]), (1, 1))

    def test_duplicate_receipt_carries_command_id(self):
        self.ok(["add", "--points", "3", "--reason", "扫地", "--command-id", "qq:c:1"])
        receipt = self.first(self.ok(
            ["add", "--points", "3", "--reason", "扫地", "--command-id", "qq:c:2"])[0])
        self.assertEqual(receipt["command_id"], "qq:c:2",
                         "去重回执也要把这次的 command_id 念出来（可对账）")

    def test_redeem_replay_does_not_double_charge(self):
        self.seed(50, "攒分")
        argv = ["redeem", "--points", "40", "--command-id", "qq:rd:1"]
        first = self.first(self.ok(argv)[0])
        second = self.first(self.ok(argv)[0])
        self.assertEqual(second["entry_id"], first["entry_id"])
        self.assertEqual(pts.read_state(self.state_path)["balance"], 10,
                         "同一次兑换重放不得重复扣分")


# --------------------------------------------------------------------------- #
# ④b 删除的 command_id 重放保护（R1：`--index` 重放不得删到漂移后的另一条）        #
# --------------------------------------------------------------------------- #


class DeleteReplayTests(_CliCase):
    def raw_state(self) -> dict:
        return json.loads(Path(self.state_path).read_text(encoding="utf-8"))

    def _three(self):
        return [self.seed(1, "alpha"), self.seed(2, "beta"), self.seed(3, "gamma")]

    def test_delete_index_replay_returns_first_result_and_deletes_once(self):
        entries = self._three()
        argv = ["delete", "--index", "2", "--command-id", "qq:del:2"]
        first = self.first(self.ok(argv)[0])
        self.assertEqual(first["deleted"], "true")
        self.assertEqual(first["entry_id"], entries[1]["id"], "index 2 = beta")
        self.assertEqual((first["revision"], first["balance"]), ("4", "4"))
        after_first = self.bytes_now()
        state_first = pts.read_state(self.state_path)
        self.assertEqual(len(state_first["entries"]), 2)
        # 同 command_id 重放：序号此时已漂移（index 2 变成 alpha），必须返首次结果。
        second = self.first(self.ok(argv)[0])
        self.assertEqual(second, first, "重放须原样返首次结果（entry_id/revision/balance）")
        self.assertEqual(self.bytes_now(), after_first, "重放不得改写 state（零写入）")
        state_after = pts.read_state(self.state_path)
        self.assertEqual(len(state_after["entries"]), 2,
                         "重放不得删第二条（首删后 index 2 已指向另一条分录）")
        self.assertEqual(state_after["revision"], state_first["revision"],
                         "重放不得递增 revision")
        remaining = {entry["id"] for entry in state_after["entries"]}
        self.assertEqual(remaining, {entries[0]["id"], entries[2]["id"]})

    def test_delete_entry_id_replay_is_guarded_too(self):
        entries = self._three()
        argv = ["delete", "--entry-id", entries[0]["id"], "--command-id", "qq:del:id"]
        first = self.first(self.ok(argv)[0])
        before = self.bytes_now()
        second = self.first(self.ok(argv)[0])
        self.assertEqual(second, first)
        self.assertEqual(self.bytes_now(), before)

    def test_delete_noop_stays_zero_write_and_records_no_receipt(self):
        self.seed(5, "seed")
        before = self.bytes_now()
        receipt = self.first(self.ok(
            ["delete", "--entry-id", "pt_" + "d" * 32, "--command-id", "qq:noop"])[0])
        self.assertEqual(receipt["deleted"], "false")
        self.assertEqual(self.bytes_now(), before, "no-op 恒零写入")
        self.assertNotIn("receipts", self.raw_state(),
                         "no-op 不得留下删除回执（否则零写入契约被破）")

    def test_delete_receipt_never_leaks_into_add_replay(self):
        """删除回执住 `delete:` 命名空间：同一字面串做 add 的 command_id 仍照常落账。"""
        seed = self.seed(5, "seed")
        self.ok(["delete", "--entry-id", seed["id"], "--command-id", "qq:shared:1"])
        add_argv = ["add", "--points", "2", "--reason", "新事", "--command-id", "qq:shared:1"]
        added = self.first(self.ok(add_argv)[0])
        self.assertNotIn("deleted", added, "add 不得把删除回执当自己的重放结果")
        self.assertRegex(added["entry_id"], r"^pt_[0-9a-f]{32}$")
        replay = self.first(self.ok(add_argv)[0])
        self.assertEqual(replay["entry_id"], added["entry_id"], "add 的重放语义不变")
        self.assertEqual(len(pts.read_state(self.state_path)["entries"]), 1)

    def test_delete_receipt_never_overwrites_add_receipt(self):
        """病态撞键：add 的裸 command_id 恰为 `delete:<x>` 时，删除不得覆盖那条回执。"""
        add_argv = ["add", "--points", "2", "--reason", "旧事", "--command-id", "delete:zz"]
        added = self.first(self.ok(add_argv)[0])
        self.seed(3, "另一笔")
        receipt = self.first(self.ok(
            ["delete", "--index", "1", "--command-id", "zz"])[0])
        self.assertEqual(receipt["deleted"], "true", "撞键只降级删除回执持久化，删除照常执行")
        replay = self.first(self.ok(add_argv)[0])
        self.assertEqual(replay["entry_id"], added["entry_id"],
                         "add 的传输幂等回执不得被删除回执顶掉")

    def test_delete_replay_key_is_isolated_in_receipts(self):
        entries = self._three()
        self.ok(["delete", "--index", "2", "--command-id", "qq:del:iso"])
        receipts = self.raw_state()["receipts"]
        self.assertEqual(list(receipts), ["delete:qq:del:iso"],
                         "删除回执只许住命名空间键，不得占用裸 command_id 位")
        self.assertEqual(
            set(receipts["delete:qq:del:iso"]),
            {"id", "deleted", "revision", "balance"})
        self.assertEqual(receipts["delete:qq:del:iso"]["id"], entries[1]["id"])


# --------------------------------------------------------------------------- #
# ⑤ 只读命令不建账本                                                             #
# --------------------------------------------------------------------------- #


class ReadOnlyTests(_CliCase):
    def test_show_and_targets_do_not_create_state(self):
        self.ok(["show"])
        self.ok(["targets"])
        self.assertFalse(Path(self.state_path).exists(),
                         "show/targets 只读：不得创建 state 文件")
        self.assertEqual(sorted(p.name for p in Path(self._tmp.name).iterdir()), [],
                         "只读命令不得留下 .lock 等副作用文件")

    def test_show_reports_zero_state_without_file(self):
        receipt = self.first(self.ok(["show"])[0])
        self.assertEqual(
            (receipt["balance"], receipt["revision"], receipt["entries_total"]),
            ("0", "0", "0"), "缺席 state = 出厂零态（不改写、不伪造）")


if __name__ == "__main__":
    unittest.main(verbosity=2)