"""KidPal 261104-k1-points-ledger-core 服务端单元测试 — gateway/points.py 正式化增量面
（分支、边界与错误路径；不只 happy path）。

与冻结验收（server/acceptance/test_points_ledger_core.acceptance.py）的分工：冻结件是
**规格**；本文件是实现自带测，覆盖规格未逐字段钉的实现内部边界——

  - 短窗去重的保守归一化（全半角 / 空白 / 标点 / 时间前缀保留 / 不模糊匹配）与窗口
    边界（599s / 601s / 未来时间 / 脏时间戳跳过）；
  - keyset 游标编解码（合法往返 / 非 base64 / 非 JSON / 非对象 / 缺键）；
  - 校验器（limit 类型与边界、目标四键形状、自定义分值 bool/float/str/出界、command_id）；
  - 余额与逐条前后余额派生（乱序写入 / 删除重算 / 负余额 / 同刻 (occurred_at,id) 定序）；
  - 目标配置存储（预置副本隔离、自定义 id 生成与唯一、激活切换、禁删守卫、未知 id）；
  - 传输回执（重放返首次 / 回执与分录同批持久 / 删除后回执仍可读）；
  - state 可选键往返与损坏拒绝（targets / active_target / receipts 形状违约）；
  - 进程间锁（`.lock` 与 state 同目录、常驻不删、跨进程子进程真跑零丢账）。

纯 stdlib + 按路径导入，零网络、零 ECS 依赖，任何 python3 可跑：
  cd server && python -m pytest -q tests/test_points_ledger_units.py
"""

from __future__ import annotations

import importlib.util
import json
import os
import subprocess
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


points = _load_by_path("kidpal_gateway_points_k1_ut", GATEWAY_DIR / "points.py")

_TZ_CN = dt_timezone(timedelta(hours=8))
_T0 = datetime(2026, 11, 5, 10, 0, 0, tzinfo=_TZ_CN)


def _t(offset_seconds: int) -> datetime:
    return _T0 + timedelta(seconds=offset_seconds)


class _StateDirMixin(unittest.TestCase):
    """每个用例一条隔离的 state 文件（绝不触部署面）。"""

    def setUp(self) -> None:  # noqa: D401 — unittest hook
        self._tmp = tempfile.TemporaryDirectory(prefix="k1l_ut_")
        self.addCleanup(self._tmp.cleanup)
        self.dir = Path(self._tmp.name)
        self.state = str(self.dir / "state.json")


# --------------------------------------------------------------------------- #
# 归一化（短窗去重的保守口径）                                                   #
# --------------------------------------------------------------------------- #


class NormalizeReasonTests(unittest.TestCase):
    def test_whitespace_is_dropped(self):
        self.assertEqual(points._normalize_reason(" 今天  扫地 "), "今天扫地")

    def test_ascii_and_cjk_punctuation_are_dropped(self):
        self.assertEqual(points._normalize_reason("今天扫地。"), points._normalize_reason("今天扫地"))
        self.assertEqual(points._normalize_reason("扫地，加五分！"), points._normalize_reason("扫地加五分"))

    def test_fullwidth_forms_unify_to_ascii(self):
        self.assertEqual(points._normalize_reason("ＡＢＣ扫地"), points._normalize_reason("ABC扫地"))
        self.assertEqual(points._normalize_reason("１２３"), points._normalize_reason("123"))

    def test_time_prefix_is_preserved_not_stripped(self):
        self.assertNotEqual(
            points._normalize_reason("今天扫地"), points._normalize_reason("昨天扫地"))

    def test_no_fuzzy_matching(self):
        self.assertNotEqual(
            points._normalize_reason("扫地"), points._normalize_reason("扫地很干净"))

    def test_empty_punctuation_only_normalises_to_empty(self):
        self.assertEqual(points._normalize_reason("。。。 ！"), "")


# --------------------------------------------------------------------------- #
# 游标编解码                                                                     #
# --------------------------------------------------------------------------- #


class CursorCodecTests(unittest.TestCase):
    def test_roundtrip(self):
        entry = {"occurred_at": _t(0).isoformat(), "id": "pt_x"}
        decoded = points._decode_cursor(points._encode_cursor(entry))
        self.assertEqual(decoded, {"occurred_at": entry["occurred_at"], "id": "pt_x"})

    def test_invalid_cursors_decode_to_none(self):
        for bad in (None, "", 123, "!!!!", "e30", "W10", "eyJ4IjogMX0"):
            self.assertIsNone(points._decode_cursor(bad), bad)

    def test_non_ascii_cursor_decodes_to_none(self):
        self.assertIsNone(points._decode_cursor("中文游标"))


# --------------------------------------------------------------------------- #
# 校验器                                                                         #
# --------------------------------------------------------------------------- #


class ValidatorTests(unittest.TestCase):
    def test_limit_rejects_non_int_and_out_of_range(self):
        for bad in (True, False, "6", 6.0, 0, -1, points.PAGE_LIMIT_MAX + 1, None):
            with self.assertRaises(ValueError):
                points._validate_limit(bad)

    def test_limit_accepts_inclusive_bounds(self):
        self.assertEqual(points._validate_limit(points.PAGE_LIMIT_MIN), 1)
        self.assertEqual(points._validate_limit(points.PAGE_LIMIT_MAX), 50)

    def test_entry_shape_rejects_extra_and_missing_keys(self):
        good = {"id": "pt_a", "delta": 1, "reason": "r",
                "occurred_at": _t(0).isoformat(),
                "balance_before": 0, "balance_after": 1}
        self.assertIsNone(points._entry_shape_error(good))
        self.assertIsNotNone(points._entry_shape_error({**good, "extra": 1}))
        missing = dict(good)
        del missing["reason"]
        self.assertIsNotNone(points._entry_shape_error(missing))
        self.assertIsNotNone(points._entry_shape_error({**good, "delta": True}))

    def test_target_shape(self):
        good = {"id": "target-1", "name": "小汽车", "points": 30, "asset": "sc-flag"}
        self.assertIsNone(points._target_shape_error(good))
        self.assertIsNotNone(points._target_shape_error({**good, "points": 0}))
        self.assertIsNotNone(points._target_shape_error({**good, "name": "  "}))
        self.assertIsNotNone(points._target_shape_error({**good, "asset": 1}))
        self.assertIsNotNone(points._target_shape_error({k: v for k, v in good.items() if k != "asset"}))

    def test_command_id_semantics(self):
        self.assertIsNone(points._coerce_command_id(None))
        self.assertEqual(points._coerce_command_id("cmd-1"), "cmd-1")
        for bad in ("", "   ", 123, True):
            with self.assertRaises(ValueError):
                points._coerce_command_id(bad)

    def test_now_rejects_non_datetime_and_normalises_naive(self):
        with self.assertRaises(ValueError):
            points._coerce_now("2026-11-05T10:00:00")
        self.assertEqual(
            points._coerce_now(datetime(2026, 11, 5, 10, 0, 0)).tzinfo, dt_timezone.utc)


# --------------------------------------------------------------------------- #
# 余额链重算                                                                     #
# --------------------------------------------------------------------------- #


class RecomputeChainTests(unittest.TestCase):
    def test_chain_follows_time_order_not_insertion_order(self):
        entries = [
            {"id": "pt_b", "delta": 2, "reason": "晚", "occurred_at": _t(60).isoformat(),
             "balance_before": 99, "balance_after": 99},
            {"id": "pt_a", "delta": 3, "reason": "早", "occurred_at": _t(0).isoformat(),
             "balance_before": 99, "balance_after": 99},
        ]
        out = points._recompute_chain(entries)
        self.assertEqual([e["id"] for e in out], ["pt_b", "pt_a"])
        self.assertEqual([(e["balance_before"], e["balance_after"]) for e in out], [(3, 5), (0, 3)])

    def test_negative_running_balance_is_not_clamped(self):
        entries = [
            {"id": "pt_a", "delta": 5, "reason": "加分", "occurred_at": _t(0).isoformat(),
             "balance_before": 0, "balance_after": 5},
            {"id": "pt_b", "delta": -40, "reason": "兑换", "occurred_at": _t(60).isoformat(),
             "balance_before": 5, "balance_after": -35},
        ]
        out = points._recompute_chain(entries)
        # 时间倒序返回：先 -40（5 -> -35）再 +5（0 -> 5），负余额不钳位
        self.assertEqual(
            [(e["balance_before"], e["balance_after"]) for e in out],
            [(5, -35), (0, 5)])

    def test_same_timestamp_orders_by_id(self):
        entries = [
            {"id": "pt_a", "delta": 1, "reason": "a", "occurred_at": _t(0).isoformat(),
             "balance_before": 0, "balance_after": 0},
            {"id": "pt_b", "delta": 2, "reason": "b", "occurred_at": _t(0).isoformat(),
             "balance_before": 0, "balance_after": 0},
        ]
        out = points._recompute_chain(entries)
        self.assertEqual([e["id"] for e in out], ["pt_b", "pt_a"])
        self.assertEqual((out[1]["balance_before"], out[1]["balance_after"]), (0, 1))
        self.assertEqual((out[0]["balance_before"], out[0]["balance_after"]), (1, 3))


# --------------------------------------------------------------------------- #
# 短窗语义去重（注入时钟）                                                       #
# --------------------------------------------------------------------------- #


class ShortWindowDedupTests(_StateDirMixin):
    def test_window_boundary_599_vs_601(self):
        points.add_points(self.state, 5, "今天扫地", now=_t(0))
        inside = points.add_points(self.state, 5, "今天扫地", now=_t(599))
        self.assertIs(inside.get("duplicate"), True)
        outside = points.add_points(self.state, 5, "今天扫地", now=_t(601))
        self.assertIn("id", outside)

    def test_deleted_entries_do_not_match(self):
        entry = points.add_points(self.state, 5, "帮忙洗碗", now=_t(0))
        points.delete_entry(self.state, entry["id"])
        fresh = points.add_points(self.state, 5, "帮忙洗碗", now=_t(60))
        self.assertIn("id", fresh)
        self.assertNotIn("duplicate", fresh)

    def test_different_sign_or_points_never_dedup(self):
        points.add_points(self.state, 5, "今天扫地", now=_t(0))
        for delta in (3, 6):
            self.assertIn("id", points.add_points(self.state, delta, "今天扫地", now=_t(60)))

    def test_clean_timestamp_skips_match_without_crash(self):
        points.add_points(self.state, 1, "坏时间", now=_t(0))
        raw = json.loads(Path(self.state).read_text(encoding="utf-8"))
        raw["entries"][0]["occurred_at"] = "not-a-timestamp"
        Path(self.state).write_text(json.dumps(raw, ensure_ascii=False), encoding="utf-8")
        # 脏时间参与排序退化，但不得炸；同文案仍按「无法判定窗口」保守记新账。
        fresh = points.add_points(self.state, 1, "坏时间", now=_t(60))
        self.assertIn("id", fresh)

    def test_transport_idempotency_checked_before_dedup(self):
        first = points.add_points(self.state, 5, "今天扫地", now=_t(0), command_id="c1")
        replay = points.add_points(self.state, 5, "今天扫地", now=_t(60), command_id="c1")
        self.assertEqual(replay["id"], first["id"])
        self.assertNotIn("duplicate", replay)

    def test_dedup_receipt_carries_authoritative_balance(self):
        points.add_points(self.state, 5, "今天扫地", now=_t(0))
        receipt = points.add_points(self.state, 5, "今天扫地", now=_t(30))
        self.assertEqual(receipt["balance"], 5)
        self.assertEqual(points.read_state(self.state)["revision"], 1)


# --------------------------------------------------------------------------- #
# 删除重算                                                                       #
# --------------------------------------------------------------------------- #


class DeleteEntryTests(_StateDirMixin):
    def test_delete_recomputes_running_balances_and_keeps_others(self):
        plus = points.add_points(self.state, 50, "误记", now=_t(0))
        points.redeem_points(self.state, 40, now=_t(60))
        receipt = points.delete_entry(self.state, plus["id"])
        self.assertIs(receipt["deleted"], True)
        self.assertEqual(receipt["balance"], -40)
        self.assertEqual(receipt["revision"], 3)
        state = points.read_state(self.state)
        self.assertEqual(len(state["entries"]), 1)
        self.assertEqual(
            (state["entries"][0]["balance_before"], state["entries"][0]["balance_after"]),
            (0, -40))

    def test_missing_id_is_noop_without_writing(self):
        points.add_points(self.state, 1, "a", now=_t(0))
        before_bytes = Path(self.state).read_bytes()
        receipt = points.delete_entry(self.state, "pt_never_existed")
        self.assertIs(receipt["deleted"], False)
        self.assertEqual(Path(self.state).read_bytes(), before_bytes)

    def test_illegal_ids_rejected_before_lock(self):
        points.add_points(self.state, 1, "a", now=_t(0))
        before = points.read_state(self.state)
        for bad in ("", "   ", None, 123, [], True):
            with self.assertRaises(ValueError):
                points.delete_entry(self.state, bad)
        self.assertEqual(points.read_state(self.state), before)


# --------------------------------------------------------------------------- #
# 目标配置存储                                                                   #
# --------------------------------------------------------------------------- #


class TargetConfigTests(_StateDirMixin):
    def test_factory_defaults_without_state_file(self):
        self.assertEqual(points.get_active_target(self.state), dict(points.DEFAULT_TARGET))
        self.assertEqual(len(points.list_targets(self.state)), len(points.PRESET_TARGETS))
        self.assertFalse(Path(self.state).exists(), "只读目标查询不得落账")

    def test_preset_list_is_isolated_copy(self):
        targets = points.list_targets(self.state)
        targets[0]["points"] = 999
        self.assertEqual(points.PRESET_TARGETS[0]["points"], 20)

    def test_custom_bounds_and_type_rejections_do_not_write(self):
        for bad in (4, 201, 0, -5, True, 2.5, "25", None):
            with self.assertRaises(ValueError):
                points.add_custom_target(self.state, "目标", bad, now=_T0)
        self.assertFalse(Path(self.state).exists())

    def test_custom_name_rejections(self):
        for bad in ("", "   ", None, 12):
            with self.assertRaises(ValueError):
                points.add_custom_target(self.state, bad, 25, now=_T0)

    def test_custom_target_ids_are_unique_and_generated(self):
        a = points.add_custom_target(self.state, "小橡皮", 5, now=_t(0))
        b = points.add_custom_target(self.state, "自行车", 200, now=_t(60))
        self.assertEqual(a["asset"], points.CUSTOM_TARGET_ASSET)
        self.assertTrue(a["id"] and b["id"] and a["id"] != b["id"])
        self.assertNotIn(a["id"], {t["id"] for t in points.PRESET_TARGETS})

    def test_switch_does_not_touch_entries_or_snapshots(self):
        points.add_points(self.state, 63, "进步奖", now=_t(0))
        redemption = points.redeem_points(self.state, 40, now=_t(60))
        before = points.read_state(self.state)
        active = points.set_active_target(self.state, "preset-gift", now=_t(120))
        self.assertEqual(active, dict(points.PRESET_TARGETS[3]))
        after = points.read_state(self.state)
        self.assertEqual(after["entries"], before["entries"])
        self.assertEqual(after["balance"], before["balance"])
        self.assertEqual(after["revision"], before["revision"] + 1)
        self.assertEqual(
            [e for e in after["entries"] if e["id"] == redemption["id"]][0], redemption)

    def test_delete_guards(self):
        for preset in {t["id"] for t in points.PRESET_TARGETS}:
            with self.assertRaises(ValueError):
                points.delete_target(self.state, preset, now=_t(0))
        custom = points.add_custom_target(self.state, "冰淇淋", 25, now=_t(60))
        points.set_active_target(self.state, custom["id"], now=_t(120))
        with self.assertRaises(ValueError):
            points.delete_target(self.state, custom["id"], now=_t(180))
        points.set_active_target(self.state, "preset-movie", now=_t(240))
        receipt = points.delete_target(self.state, custom["id"], now=_t(300))
        self.assertEqual(receipt["deleted"], True)
        self.assertNotIn(custom["id"], {t["id"] for t in points.list_targets(self.state)})
        with self.assertRaises(ValueError):
            points.delete_target(self.state, "no-such-target", now=_t(360))

    def test_unknown_switch_rejected_without_write(self):
        points.add_points(self.state, 5, "a", now=_t(0))
        before = Path(self.state).read_bytes()
        with self.assertRaises(ValueError):
            points.set_active_target(self.state, "no-such-target", now=_t(60))
        self.assertEqual(Path(self.state).read_bytes(), before)


# --------------------------------------------------------------------------- #
# 权威载荷与分页                                                                 #
# --------------------------------------------------------------------------- #


class PayloadTests(_StateDirMixin):
    def test_home_target_follows_active_target(self):
        points.set_active_target(self.state, "preset-choco", now=_t(0))
        home = points.build_points_home(self.state)
        self.assertEqual(home["data"]["target"], dict(points.PRESET_TARGETS[0]))
        self.assertNotIn("restore", home["data"])

    def test_ledger_payload_never_carries_target(self):
        points.add_points(self.state, 1, "a", now=_t(0))
        page = points.build_points_ledger_page(self.state)
        self.assertNotIn("target", page["data"])
        self.assertEqual(page["data"]["page"]["total"], 1)

    def test_empty_ledger_is_authoritative_zero(self):
        data = points.build_points_ledger_page(self.state)["data"]
        self.assertEqual(data["items"], [])
        self.assertEqual(data["balance"], 0)
        self.assertEqual(data["page"], {"total": 0, "next_cursor": None,
                                        "has_more": False, "reset": True})

    def test_thin_and_full_pages_share_wire_shape(self):
        for i in range(3):
            points.add_points(self.state, 1, f"第{i}笔", now=_t(i * 60))
        page = points.build_points_ledger_page(self.state, limit=2)["data"]
        self.assertEqual(set(page["page"]), {"total", "next_cursor", "has_more", "reset"})


# --------------------------------------------------------------------------- #
# state 可选键与损坏拒绝                                                          #
# --------------------------------------------------------------------------- #


class StateFileTests(_StateDirMixin):
    def test_defaults_stay_out_of_file_and_come_back_as_defaults(self):
        points.add_points(self.state, 1, "a", now=_t(0))
        raw = json.loads(Path(self.state).read_text(encoding="utf-8"))
        self.assertNotIn("targets", raw)
        self.assertNotIn("active_target", raw)
        self.assertEqual(points.get_active_target(self.state), dict(points.DEFAULT_TARGET))

    def test_receipts_survive_roundtrip(self):
        first = points.add_points(self.state, 5, "a", now=_t(0), command_id="cmd-a")
        raw = json.loads(Path(self.state).read_text(encoding="utf-8"))
        self.assertEqual(raw["receipts"]["cmd-a"]["id"], first["id"])
        self.assertEqual(
            points.add_points(self.state, 5, "a", now=_t(600), command_id="cmd-a")["id"],
            first["id"])

    def test_corrupt_targets_and_receipts_rejected(self):
        points.add_custom_target(self.state, "目标", 25, now=_t(0))
        raw = json.loads(Path(self.state).read_text(encoding="utf-8"))
        bad_targets = dict(raw, targets=[{"id": "x"}])
        Path(self.state).write_text(json.dumps(bad_targets, ensure_ascii=False), encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)
        bad_receipts = dict(raw, receipts={"cmd": "not-an-entry"})
        Path(self.state).write_text(json.dumps(bad_receipts, ensure_ascii=False), encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)

    def test_reserved_or_duplicate_target_ids_rejected(self):
        points.add_custom_target(self.state, "目标", 25, now=_t(0))
        raw = json.loads(Path(self.state).read_text(encoding="utf-8"))
        raw["targets"].append(dict(raw["targets"][0]))
        Path(self.state).write_text(json.dumps(raw, ensure_ascii=False), encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)

    def test_unknown_active_target_rejected(self):
        Path(self.state).write_text(
            json.dumps({"revision": 1, "balance": 0, "entries": [],
                        "active_target": "ghost"}), encoding="utf-8")
        with self.assertRaises(ValueError):
            points.read_state(self.state)


# --------------------------------------------------------------------------- #
# 进程间锁 / 原子写                                                              #
# --------------------------------------------------------------------------- #


class LockAndAtomicityTests(_StateDirMixin):
    def test_lock_file_lives_next_to_state_and_persists(self):
        points.add_points(self.state, 1, "a", now=_t(0))
        self.assertTrue((self.dir / ".lock").is_file())
        self.assertFalse((self.dir / ".lock").is_dir())

    def test_no_tmp_residue_after_write(self):
        points.add_points(self.state, 1, "a", now=_t(0))
        residue = [p.name for p in self.dir.iterdir() if ".tmp-" in p.name]
        self.assertEqual(residue, [])

    def test_failed_replace_keeps_old_state_and_raises(self):
        points.add_points(self.state, 5, "首笔", now=_t(0))
        before = Path(self.state).read_bytes()
        real_replace = os.replace

        def _boom(*args, **kwargs):
            raise OSError("simulated persist failure")

        os.replace = _boom
        try:
            with self.assertRaises(OSError):
                points.add_points(self.state, 9, "boom", now=_t(60))
        finally:
            os.replace = real_replace
        self.assertEqual(Path(self.state).read_bytes(), before)
        self.assertEqual(points.read_state(self.state)["balance"], 5)

    def test_cross_process_writes_are_serialised_by_file_lock(self):
        """真子进程并发（与冻结件不同形态：这里只验「零丢账 + .lock 在位」）。"""
        code = (
            "import sys\n"
            "from gateway.points import add_points\n"
            "add_points(sys.argv[1], 1, 'p' + sys.argv[2])\n")
        env = dict(os.environ)
        env["PYTHONPATH"] = str(SERVER)
        procs = [
            subprocess.Popen([sys.executable, "-c", code, self.state, str(i)],
                             cwd=str(SERVER), env=env,
                             stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
            for i in range(4)]
        for proc in procs:
            _, err = proc.communicate(timeout=120)
            self.assertEqual(proc.returncode, 0, err)
        state = points.read_state(self.state)
        self.assertEqual(state["revision"], 4)
        self.assertEqual(state["balance"], 4)
        self.assertEqual(len(state["entries"]), 4)
        self.assertTrue((self.dir / ".lock").is_file())


if __name__ == "__main__":  # pragma: no cover — standalone unittest runner
    unittest.main()
