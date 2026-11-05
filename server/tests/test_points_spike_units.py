"""KidPal 261104-k0-points-e2e-spike 服务端单元测试 — gateway/points.py 增量面
+ display_schema 积分 floor 边界（分支、边界与错误路径；不只 happy path）。

与冻结验收（server/acceptance/test_points_e2e_spike.acceptance.py）的分工：冻结件是
**规格**（6 条全绿）；本文件是实现自带测，覆盖规格未逐字段钉的边界/错误路径——
state 损坏（非缺失）拒绝分支、limit 类型与边界（含 bool/字符串/缺省）、keyset 多页
走查与同刻 (occurred_at,id) 次序、recent 截断与返回结构隔离（改返回值不得污染
DEFAULT_TARGET / 账本）、CLI 参数面与「失败零写入 / 单 state 文件」、display_schema
points_ledger floor 分支（冻结件只钉 points_home 侧）。

261104-k1-points-ledger-core 正式化对齐（manifest 扩单，planner ruling）：`points.py`
在同一文件上补齐跨进程 `.lock`、短窗语义去重与删除/目标存储后，本文件两条 K0 期断言
按冻结契约更新——① state 同目录恒有常驻 `.lock`（K1L-A1 的进程间锁机制锚，不是第二
账本）；② 同额二次兑换在 10 分钟窗口内命中语义去重（CLI 返 0 + duplicate=true，
S-F6.3-AC5「同一次兑换不重复扣分」），兑换资格门与余额门分别改由「错额兑换」与
「窗口外兑换」钉住（不再靠时间巧合）。

纯 stdlib + 按路径/包导入，零网络、零 ECS 依赖，任何 python3 可跑：
  cd server && python -m pytest -q tests/test_points_spike_units.py
"""

from __future__ import annotations

import contextlib
import importlib.util
import io
import json
import os
import sys
import tempfile
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
if str(SERVER) not in sys.path:
    sys.path.insert(0, str(SERVER))

GATEWAY_DIR = SERVER / "gateway"


def _load_by_path(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None, f"{path} not loadable"
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


points = _load_by_path("kidpal_gateway_points_k0pts_ut", GATEWAY_DIR / "points.py")
ds = _load_by_path("kidpal_gateway_display_schema_k0pts_ut", GATEWAY_DIR / "display_schema.py")

_TZ_CN = dt_timezone(timedelta(hours=8))


def _t(day: int, hour: int = 9) -> datetime:
    return datetime(2026, 11, day, hour, 0, 0, tzinfo=_TZ_CN)


class _StateDirMixin(unittest.TestCase):
    """每个用例一条隔离的 state 文件（绝不触部署面）。"""

    def setUp(self) -> None:  # noqa: D401 — unittest hook
        self._tmp = tempfile.TemporaryDirectory(prefix="k0pts_ut_")
        self.addCleanup(self._tmp.cleanup)
        self.state = str(Path(self._tmp.name) / "state.json")


# --------------------------------------------------------------------------- #
# read_state / 持久化                                                          #
# --------------------------------------------------------------------------- #


class PointsStateTests(_StateDirMixin):
    def test_missing_file_is_zero_state(self):
        self.assertEqual(
            points.read_state(self.state),
            {"revision": 0, "balance": 0, "entries": []})

    def test_corrupt_json_raises_not_blank_ledger(self):
        Path(self.state).write_text("{not json", encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)

    def test_non_object_root_raises(self):
        Path(self.state).write_text("[]", encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)

    def test_entries_must_be_list(self):
        Path(self.state).write_text(
            json.dumps({"revision": 1, "balance": 0, "entries": {}}), encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)

    def test_corrupt_entry_shape_raises(self):
        bad = {"id": "x", "delta": True, "reason": "r",
               "occurred_at": "2026-11-03T10:00:00+08:00",
               "balance_before": 0, "balance_after": 1}
        Path(self.state).write_text(
            json.dumps({"revision": 1, "balance": 1, "entries": [bad]}), encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)

    def test_negative_revision_raises(self):
        Path(self.state).write_text(
            json.dumps({"revision": -1, "balance": 0, "entries": []}), encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)

    def test_balance_recomputed_from_entries_not_trusted_copy(self):
        entry = {"id": "pt_a", "delta": 7, "reason": "r",
                 "occurred_at": "2026-11-03T10:00:00+08:00",
                 "balance_before": 0, "balance_after": 7}
        Path(self.state).write_text(
            json.dumps({"revision": 3, "balance": 999, "entries": [entry]}),
            encoding="utf-8")
        self.assertEqual(points.read_state(self.state)["balance"], 7)

    def test_empty_state_path_rejected(self):
        for bad in (None, "", 0):
            with self.assertRaises(ValueError):
                points.read_state(bad)

    def test_entries_are_time_descending_on_read(self):
        points.add_points(self.state, 1, "早", now=_t(1))
        points.add_points(self.state, 2, "晚", now=_t(2))
        reasons = [e["reason"] for e in points.read_state(self.state)["entries"]]
        self.assertEqual(reasons, ["晚", "早"])


# --------------------------------------------------------------------------- #
# add_points                                                                   #
# --------------------------------------------------------------------------- #


class AddPointsTests(_StateDirMixin):
    def test_chain_of_balances_and_revision(self):
        first = points.add_points(self.state, 5, "今天扫地", now=_t(1))
        second = points.add_points(self.state, 3, "帮忙洗碗", now=_t(2))
        self.assertEqual((first["balance_before"], first["balance_after"]), (0, 5))
        self.assertEqual((second["balance_before"], second["balance_after"]), (5, 8))
        state = points.read_state(self.state)
        self.assertEqual(state["revision"], 2)
        self.assertEqual(state["balance"], 8)
        self.assertNotEqual(first["id"], second["id"])

    def test_rejects_non_positive_integers(self):
        for bad in (0, -3, -1):
            with self.assertRaises(ValueError):
                points.add_points(self.state, bad, "r", now=_t(1))
        self.assertFalse(Path(self.state).exists())

    def test_rejects_bool_float_string_none(self):
        for bad in (True, False, 1.0, 2.5, "5", None):
            with self.assertRaises(ValueError):
                points.add_points(self.state, bad, "r", now=_t(1))
        self.assertFalse(Path(self.state).exists())

    def test_rejects_missing_or_blank_reason(self):
        for bad in (None, "", "   ", "\t\n", 123):
            with self.assertRaises(ValueError):
                points.add_points(self.state, 5, bad, now=_t(1))
        self.assertFalse(Path(self.state).exists())

    def test_rejected_attempt_leaves_ledger_untouched(self):
        points.add_points(self.state, 5, "ok", now=_t(1))
        before = points.read_state(self.state)
        with self.assertRaises(ValueError):
            points.add_points(self.state, 0, "bad", now=_t(2))
        self.assertEqual(points.read_state(self.state), before)

    def test_naive_now_gets_utc_offset(self):
        entry = points.add_points(
            self.state, 1, "无时区", now=datetime(2026, 11, 4, 9, 0, 0))
        self.assertTrue(entry["occurred_at"].endswith("+00:00"))

    def test_invalid_now_type_rejected_without_write(self):
        with self.assertRaises(ValueError):
            points.add_points(self.state, 1, "r", now="2026-11-04T09:00:00")
        self.assertFalse(Path(self.state).exists())

    def test_entry_has_exactly_six_frozen_keys(self):
        entry = points.add_points(self.state, 1, "r", now=_t(1))
        self.assertEqual(set(entry.keys()), set(points.ENTRY_KEYS))


# --------------------------------------------------------------------------- #
# redeem_points                                                                #
# --------------------------------------------------------------------------- #


class RedeemPointsTests(_StateDirMixin):
    def _fund(self, amount=100):
        points.add_points(self.state, amount, "攒分", now=_t(1))

    def test_rejects_any_value_but_active_target(self):
        self._fund()
        before = points.read_state(self.state)
        for bad in (1, 39, 41, points.DEFAULT_TARGET["points"] * 2):
            with self.assertRaises(ValueError):
                points.redeem_points(self.state, bad, now=_t(2))
        self.assertEqual(points.read_state(self.state), before)

    def test_rejects_when_balance_insufficient(self):
        points.add_points(self.state, points.DEFAULT_TARGET["points"] - 1, "差一分", now=_t(1))
        before = points.read_state(self.state)
        with self.assertRaises(ValueError):
            points.redeem_points(self.state, points.DEFAULT_TARGET["points"], now=_t(2))
        self.assertEqual(points.read_state(self.state), before)

    def test_exact_balance_is_insufficient_edge_allowed(self):
        target = points.DEFAULT_TARGET["points"]
        points.add_points(self.state, target, "刚好", now=_t(1))
        entry = points.redeem_points(self.state, target, now=_t(2))
        self.assertEqual(entry["balance_after"], 0)
        self.assertEqual(entry["delta"], -target)
        self.assertEqual(entry["reason"], f"兑换{points.DEFAULT_TARGET['name']}")

    def test_rejects_bool_and_non_integer(self):
        self._fund()
        for bad in (True, points.DEFAULT_TARGET["points"] + 0.0, "40", None):
            with self.assertRaises(ValueError):
                points.redeem_points(self.state, bad, now=_t(2))

    def test_second_redeem_blocked_by_insufficient_balance(self):
        """窗口外二次兑换钉**余额门**（本用例的判别对象不是去重）。

        两次兑换显式拉开到 DEDUP_WINDOW_SECONDS 之外：窗口内的同额二次兑换会返
        duplicate 回执而非拒绝（S-F6.3-AC5 预期行为，见
        test_redeem_requires_matching_target_and_balance），只有落在窗口外时才
        真正走到「余额 < 激活目标分值」这道门。"""
        target = points.DEFAULT_TARGET["points"]
        self._fund(target)
        first_at = _t(2)
        second_at = first_at + timedelta(seconds=points.DEDUP_WINDOW_SECONDS + 1)
        points.redeem_points(self.state, target, now=first_at)
        before = points.read_state(self.state)
        self.assertEqual(before["balance"], 0)
        with self.assertRaises(ValueError):
            points.redeem_points(self.state, target, now=second_at)
        self.assertEqual(points.read_state(self.state), before)


# --------------------------------------------------------------------------- #
# build_points_home                                                            #
# --------------------------------------------------------------------------- #


class PointsHomePayloadTests(_StateDirMixin):
    def test_recent_is_capped_and_time_descending(self):
        for day in range(1, 6):
            points.add_points(self.state, day, f"第{day}笔", now=_t(day))
        home = points.build_points_home(self.state)
        self.assertEqual(home["view"], "points_home")
        recent = home["data"]["recent"]
        self.assertEqual(len(recent), points.HOME_RECENT_MAX)
        self.assertEqual([e["reason"] for e in recent], ["第5笔", "第4笔", "第3笔"])

    def test_empty_ledger_is_authoritative_zero(self):
        home = points.build_points_home(self.state)
        self.assertEqual(
            home["data"],
            {"revision": 0, "balance": 0, "target": points.DEFAULT_TARGET, "recent": []})

    def test_no_restore_key_on_producer_payload(self):
        points.add_points(self.state, 1, "r", now=_t(1))
        self.assertNotIn("restore", points.build_points_home(self.state)["data"])

    def test_returned_structures_are_isolated_copies(self):
        points.add_points(self.state, 1, "r", now=_t(1))
        home = points.build_points_home(self.state)
        home["data"]["target"]["points"] = 999
        home["data"]["recent"][0]["reason"] = "tampered"
        self.assertEqual(points.DEFAULT_TARGET["points"], 40)
        self.assertEqual(points.read_state(self.state)["entries"][0]["reason"], "r")


# --------------------------------------------------------------------------- #
# build_points_ledger_page                                                     #
# --------------------------------------------------------------------------- #


class PointsLedgerPageTests(_StateDirMixin):
    def _seed(self, n=5):
        base = _t(1)
        for i in range(1, n + 1):
            points.add_points(self.state, i, f"第{i}笔", now=base + timedelta(minutes=i))

    def test_default_limit_is_six_and_first_page_resets(self):
        self._seed(8)
        page = points.build_points_ledger_page(self.state)["data"]
        self.assertEqual(len(page["items"]), points.PAGE_LIMIT_DEFAULT)
        self.assertTrue(page["page"]["reset"])
        self.assertTrue(page["page"]["has_more"])
        self.assertEqual(page["page"]["total"], 8)
        self.assertNotIn("restore", page)

    def test_limit_bounds_are_inclusive(self):
        self._seed(60)
        self.assertEqual(
            len(points.build_points_ledger_page(self.state, limit=1)["data"]["items"]), 1)
        self.assertEqual(
            len(points.build_points_ledger_page(self.state, limit=50)["data"]["items"]), 50)

    def test_limit_out_of_range_or_wrong_type_raises(self):
        self._seed(2)
        for bad in (0, -1, 51, True, False, "6", 6.0, []):
            with self.assertRaises(ValueError):
                points.build_points_ledger_page(self.state, limit=bad)

    def test_keyset_walk_reaches_every_entry_exactly_once(self):
        self._seed(5)
        seen, cursor, guard = [], None, 0
        while True:
            guard += 1
            self.assertLess(guard, 10, "keyset 走查不收敛（游标环）")
            page = points.build_points_ledger_page(
                self.state, cursor=cursor, limit=2)["data"]
            seen.extend(e["reason"] for e in page["items"])
            cursor = page["page"]["next_cursor"]
            if not page["page"]["has_more"]:
                self.assertIsNone(cursor)
                break
        self.assertEqual(seen, [f"第{d}笔" for d in range(5, 0, -1)])

    def test_invalid_cursors_fall_back_to_latest_first_page(self):
        self._seed(3)
        # 非 base64 / 空串 / 非 JSON / 非对象 / 非预期键形状。
        for bad in ("bogus", "", "!!!!", "e30", "W10", "eyJ4IjogMX0"):
            page = points.build_points_ledger_page(self.state, cursor=bad)["data"]
            self.assertTrue(page["page"]["reset"], bad)
            self.assertEqual(page["items"][0]["reason"], "第3笔", bad)

    def test_known_revision_mismatch_resets(self):
        self._seed(3)
        first = points.build_points_ledger_page(self.state, limit=1)["data"]
        cursor = first["page"]["next_cursor"]
        stale = points.build_points_ledger_page(
            self.state, cursor=cursor, known_revision=999, limit=1)["data"]
        self.assertTrue(stale["page"]["reset"])
        self.assertEqual(stale["items"][0]["reason"], "第3笔")

    def test_known_revision_absent_continues(self):
        self._seed(3)
        first = points.build_points_ledger_page(self.state, limit=1)["data"]
        second = points.build_points_ledger_page(
            self.state, cursor=first["page"]["next_cursor"], limit=1)["data"]
        self.assertFalse(second["page"]["reset"])
        self.assertEqual(second["items"][0]["reason"], "第2笔")

    def test_total_is_full_count_not_page_length(self):
        self._seed(7)
        page = points.build_points_ledger_page(self.state, limit=3)["data"]
        self.assertEqual(page["page"]["total"], 7)
        self.assertEqual(len(page["items"]), 3)

    def test_cursor_older_than_every_entry_yields_empty_authoritative_page(self):
        self._seed(2)
        # 构造一个「早于最早一条」的合法 keyset 游标（生产侧不会产出，属过期/越界面）。
        beyond_cursor = points._encode_cursor(
            {"occurred_at": "2000-01-01T00:00:00+08:00", "id": "zzz"})
        page = points.build_points_ledger_page(
            self.state, cursor=beyond_cursor, limit=2)["data"]
        self.assertEqual(page["items"], [])
        self.assertEqual(page["page"]["total"], 2)
        self.assertFalse(page["page"]["has_more"])
        self.assertIsNone(page["page"]["next_cursor"])
        self.assertFalse(page["page"]["reset"])

    def test_same_timestamp_orders_by_id_descending(self):
        points.add_points(self.state, 1, "同刻一", now=_t(1))
        points.add_points(self.state, 2, "同刻二", now=_t(1))
        ids = [e["id"] for e in points.read_state(self.state)["entries"]]
        self.assertEqual(ids, sorted(ids, reverse=True))

    def test_ordering_is_deterministic_across_reads(self):
        points.add_points(self.state, 1, "a", now=_t(1))
        points.add_points(self.state, 2, "b", now=_t(1))
        one = [e["id"] for e in points.read_state(self.state)["entries"]]
        two = [e["id"] for e in points.read_state(self.state)["entries"]]
        self.assertEqual(one, two)
        self.assertEqual(len(set(one)), 2)


# --------------------------------------------------------------------------- #
# CLI（main）                                                                  #
# --------------------------------------------------------------------------- #


class PointsCliTests(_StateDirMixin):
    def _main(self, argv):
        old = os.environ.get(points.POINTS_STATE_ENV)
        os.environ[points.POINTS_STATE_ENV] = self.state
        try:
            return points.main(argv)
        finally:
            if old is None:
                os.environ.pop(points.POINTS_STATE_ENV, None)
            else:
                os.environ[points.POINTS_STATE_ENV] = old

    def test_add_success_writes_single_ledger(self):
        self.assertEqual(self._main(["add", "3", "canary"]), 0)
        self.assertEqual(points.read_state(self.state)["balance"], 3)
        # 单源：同目录不产生第二**账本**文件；`.lock` 是 K1 冻结契约的进程间锁机制锚
        # （K1L-A1：state 同目录 `.lock` 文件在位，常驻不删——unlink-while-held 是经典
        # 竞态），不属于第二账本，故按新布局直接钉住两条。
        siblings = sorted(p.name for p in Path(self._tmp.name).iterdir())
        self.assertEqual(siblings, [".lock", "state.json"])

    def test_redeem_requires_matching_target_and_balance(self):
        self._main(["add", "50", "fund"])
        self.assertEqual(self._main(["redeem", str(points.DEFAULT_TARGET["points"])]), 0)
        self.assertEqual(points.read_state(self.state)["balance"], 10)
        # 资格门（只认激活目标分值）：40 下兑 30 = 错额，必须非零退出且零落账。
        before = points.read_state(self.state)
        self.assertNotEqual(self._main(["redeem", "30"]), 0)
        self.assertEqual(points.read_state(self.state), before)
        # 短窗语义去重（K1L-A6 / S-F6.3-AC5）：同额二次兑换落在 10 分钟窗口内 ->
        # 命中语义去重，CLI 返 0 且回执标 duplicate=true（**不重复扣分**，这是 K1
        # 冻结语义的正确行为；去重检查先于余额门，窗口外的余额门由
        # test_second_redeem_blocked_by_insufficient_balance 钉）。
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            rc_again = self._main(["redeem", str(points.DEFAULT_TARGET["points"])])
        self.assertEqual(rc_again, 0, out.getvalue())
        self.assertIn("duplicate=true", out.getvalue())
        self.assertEqual(points.read_state(self.state)["balance"], 10)

    def test_missing_env_refuses_and_writes_nothing(self):
        old = os.environ.pop(points.POINTS_STATE_ENV, None)
        try:
            self.assertEqual(points.main(["add", "3", "r"]), 2)
        finally:
            if old is not None:
                os.environ[points.POINTS_STATE_ENV] = old
        self.assertFalse(Path(self.state).exists())

    def test_bad_subcommand_and_arg_count(self):
        self.assertEqual(self._main([]), 2)
        self.assertEqual(self._main(["nope"]), 2)
        self.assertEqual(self._main(["add", "3"]), 2)
        self.assertEqual(self._main(["add", "3", "r", "extra"]), 2)
        self.assertEqual(self._main(["redeem"]), 2)
        self.assertEqual(self._main(["redeem", "40", "extra"]), 2)
        self.assertFalse(Path(self.state).exists())

    def test_non_integer_and_zero_points_rejected_without_write(self):
        for argv in (["add", "abc", "r"], ["add", "0", "r"], ["add", "1.5", "r"],
                     ["add", "-3", "r"], ["redeem", "abc"]):
            self.assertNotEqual(self._main(argv), 0)
        self.assertFalse(Path(self.state).exists())

    def test_blank_reason_rejected_without_write(self):
        self.assertNotEqual(self._main(["add", "3", "   "]), 0)
        self.assertFalse(Path(self.state).exists())


# --------------------------------------------------------------------------- #
# display_schema 积分 floor（points_ledger 侧 + 双路径）                        #
# --------------------------------------------------------------------------- #


class PointsSchemaFloorTests(unittest.TestCase):
    def _ledger(self, **data_overrides):
        data = {"revision": 1, "balance": 5, "items": [],
                "page": {"total": 0, "next_cursor": None,
                         "has_more": False, "reset": True}}
        data.update(data_overrides)
        return {"view": "points_ledger", "data": data}

    def test_ledger_complete_frame_gated_on_relay_but_allowed_self_push(self):
        frame = self._ledger()
        self.assertFalse(ds.validate_display(frame)[0])
        self.assertTrue(ds.validate_display(frame, self_push=True)[0])

    def test_ledger_page_missing_key_rejected(self):
        for missing in ("total", "next_cursor", "has_more", "reset"):
            data = {"revision": 1, "balance": 0, "items": [],
                    "page": {"total": 0, "next_cursor": None,
                             "has_more": False, "reset": True}}
            del data["page"][missing]
            frame = {"view": "points_ledger", "data": data}
            self.assertFalse(ds.validate_display(frame, self_push=True)[0],
                             f"page 缺 {missing!r} 未拒")

    def test_ledger_items_and_page_type_floor(self):
        self.assertFalse(ds.validate_display(
            self._ledger(items={"o": 1}), self_push=True)[0])
        self.assertFalse(ds.validate_display(
            self._ledger(page=[]), self_push=True)[0])

    def test_revision_and_balance_booleans_rejected(self):
        self.assertFalse(ds.validate_display(
            self._ledger(revision=True), self_push=True)[0])
        self.assertFalse(ds.validate_display(
            self._ledger(balance=False), self_push=True)[0])

    def test_thin_nav_payloads_pass_both_paths(self):
        for view in ("points_home", "points_ledger"):
            payload = {"view": view, "data": {}}
            self.assertTrue(ds.validate_display(payload)[0], view)
            self.assertTrue(ds.validate_display(payload, self_push=True)[0], view)

    def test_restore_optional_bool_only(self):
        self.assertTrue(ds.validate_display(
            self._ledger(restore=True), self_push=True)[0])
        self.assertFalse(ds.validate_display(
            self._ledger(restore="true"), self_push=True)[0])

    def test_whitelist_is_additive_nine(self):
        self.assertEqual(len(ds.VALID_VIEWS), 9)
        self.assertTrue({"points_home", "points_ledger"} <= set(ds.VALID_VIEWS))

    def test_points_actions_are_decision_class_not_forwardable(self):
        self.assertTrue({"add_points", "redeem_points"}
                        <= set(ds.GATEWAY_DECISION_CONTROL_ACTIONS))
        self.assertFalse({"add_points", "redeem_points"}
                         & set(ds.FORWARDABLE_CONTROL_ACTIONS))


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
