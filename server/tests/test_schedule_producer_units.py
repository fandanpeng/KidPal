"""KidPal 260921-m3-k1-cloud-schedule-producer 服务端单元测试 — 周网格 / home upcoming /
三触发决策面实现自带测（分支、边界与错误路径；不只 happy path）。

与冻结验收（server/acceptance/test_m3k1_schedule_producer.acceptance.py）的分工：冻结件
是**规格**（17 齿，全绿）；本文件是实现自带测，覆盖规格未逐字段钉的边界/错误路径——
上午/下午/晚上分界时刻留痕、脏 timezone 大声拒绝、marker 竞态幂等、薄载荷之下的
validate_display schedule 收紧分支、（borderline）周外一次性安排过滤。

纯 stdlib + 按路径加载，零网络、零 ECS 依赖，任何 python3 可跑：
  cd server && python -m pytest -q tests/test_schedule_producer_units.py
"""

from __future__ import annotations

import importlib.util
import json
import os
import sys
import tempfile
import unittest
from datetime import datetime, timezone as dt_timezone
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


schedule = _load_by_path("kidpal_gateway_schedule_m3k1_ut", GATEWAY_DIR / "schedule.py")
push = _load_by_path("kidpal_gateway_schedule_push_m3k1_ut", GATEWAY_DIR / "schedule_push.py")
ds = _load_by_path("kidpal_gateway_display_schema_m3k1_ut", GATEWAY_DIR / "display_schema.py")

# 2026-09-21 = 周一（day 0 基）；09-23 周三；09-27 周日。
MON = "2026-09-21"
WED = "2026-09-23"
SUN = "2026-09-27"

GRID = """---
child: 小明
timezone: Asia/Shanghai
---

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周二 | 09:30-10:30 | 书法 | 墨斋 | |
| 周三 | 13:00-14:00 | 英语培训 | 新东方 | |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-09-24 | 20:00 | 晚会 | 礼堂 | |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 | 第4节 | 第5节 | 第6节 |
|------|-------|-------|-------|-------|-------|-------|
| 周三 | 数学 | 语文 | 体育 | 科学 | 音乐 | 劳动 |
"""

UTC = dt_timezone.utc


class WeekGridProducerBoundaries(unittest.TestCase):
    def setUp(self):
        self.model = schedule.parse_schedule_md(GRID)

    def test_slot_boundary_pins_left_and_night(self):
        """分界常量留痕（契约显式缺口③）：09:30 上午0、13:00 下午1、20:00 晚上2。"""
        courses = schedule.week_grid_payload(self.model, MON)["data"]["week"]["courses"]
        by_name = {c["name"]: c for c in courses}
        self.assertEqual(by_name["书法"]["slot"], 0)     # 09:30 -> 上午
        self.assertEqual(by_name["英语培训"]["slot"], 1)  # 13:00 -> 下午
        self.assertEqual(by_name["晚会"]["slot"], 2)      # 20:00 -> 晚上

    def test_school_slots_4_plus_2(self):
        courses = schedule.week_grid_payload(self.model, MON)["data"]["week"]["courses"]
        school = [c for c in courses
                  if c["name"] in ("数学", "语文", "体育", "科学", "音乐", "劳动") and c["day"] == 2]
        am = [c["name"] for c in school if c["slot"] == 0]
        pm = [c["name"] for c in school if c["slot"] == 1]
        self.assertEqual(am, ["数学", "语文", "体育", "科学"])  # 第1..4节
        self.assertEqual(pm, ["音乐", "劳动"])                  # 第5..6节

    def test_onetime_outside_week_filtered(self):
        out_of_week = schedule.parse_schedule_md(
            "## 每周固定课表\n| 星期 | 时间 | 课程 | 地点 | 备注 |\n"
            "|---|---|---|---|---|\n| 周一 | 10:00-11:00 | 书画 | 墨 | |\n"
            "## 一次性安排\n| 日期 | 时间 | 事项 | 地点 | 备注 |\n"
            "|---|---|---|---|---|\n| 2026-09-14 | 20:00 | 上周事 | 旧 | |\n"
            "| 2026-09-30 | 20:00 | 下周事 | 新 | |\n")
        names = {c["name"] for c in
                 schedule.week_grid_payload(out_of_week, MON)["data"]["week"]["courses"]}
        self.assertEqual(names, {"书画"})
        self.assertNotIn("上周事", names)
        self.assertNotIn("下周事", names)

    def test_week_boundary_accepts_any_day_in_week(self):
        """week_date_iso 传周内任意日都落同一周一..周日周（09-23 -> 09-21..09-27）。"""
        g1 = schedule.week_grid_payload(self.model, MON)["data"]["week"]["courses"]
        g2 = schedule.week_grid_payload(self.model, WED)["data"]["week"]["courses"]
        self.assertEqual(sorted(g1, key=lambda c: (c["name"], c["day"])),
                         sorted(g2, key=lambda c: (c["name"], c["day"])))


class HomeUpcomingProducerBoundaries(unittest.TestCase):
    def test_preview_time_is_start_hhmm_not_label(self):
        """preview.time = 明日首项开始 HH:MM（板端 hp1_item_t.time 缓冲仅容 HH:MM）。"""
        model = schedule.parse_schedule_md(GRID)
        payload = schedule.home_upcoming_payload(model, MON, "12:00", WED, "08:00")
        preview = payload["data"]["upcoming"]["preview"]
        # 周三首项：合并上学(明天? 不, 周三 wd=3) + 学校周三行 合并「上学」08:00
        self.assertIsNotNone(preview)
        self.assertRegex(preview["time"], r"\d{2}:\d{2}")


class DailyPushDueBoundaries(unittest.TestCase):
    def test_unknown_timezone_rejected_loud(self):
        with self.assertRaises(ValueError):
            push.daily_push_due("America/New_York", None, datetime(2026, 9, 21, tzinfo=UTC))

    def test_single_source_json_equality(self):
        model = schedule.parse_schedule_md(GRID)
        home = schedule.home_upcoming_payload(model, MON, "12:00", SUN, "08:00")
        connect = push.connect_push_payload(model, MON, "12:00", SUN, "08:00")
        self.assertEqual(json.loads(json.dumps(connect, ensure_ascii=False)),
                         json.loads(json.dumps(home, ensure_ascii=False)))


class RefreshMarkerBoundaries(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="k1_marker_ut_"))
        self.addCleanup(__import__("shutil").rmtree, self.dir, True)

    def test_consume_marker_race_idempotent(self):
        marker = self.dir / push.MARKER_FILENAME
        marker.write_text("", encoding="utf-8")
        self.assertTrue(push.consume_refresh_marker(self.dir))
        self.assertFalse(push.consume_refresh_marker(self.dir))  # 竞态/已消费 -> 幂等 False
        self.assertFalse(marker.exists())

    def test_secondary_channel_no_marker(self):
        (self.dir / "schedule.md").write_text("直改不看 marker", encoding="utf-8")
        self.assertFalse(push.refresh_requested(self.dir))
        self.assertFalse(push.consume_refresh_marker(self.dir))


class ValidateDisplayScheduleTighten(unittest.TestCase):
    def test_thin_and_empty_week_pass(self):
        ok, reason = ds.validate_display({"view": "schedule", "data": {}})
        self.assertTrue(ok, reason)
        ok2, reason2 = ds.validate_display(
            {"view": "schedule", "data": {"week": {"courses": []}}})
        self.assertTrue(ok2, reason2)

    def test_legacy_and_malformed_rejected(self):
        for bad in ({"view": "schedule", "data": {"title": "x", "items": []}},
                    {"view": "schedule", "data": {"week": {}}},
                    {"view": "schedule", "data": {"week": {"courses": "nope"}}},
                    {"view": "schedule", "data": {"week": {"courses": [{"day": 0}]}}}):
            ok, reason = ds.validate_display(bad)
            self.assertFalse(ok, f"schedule 收紧漏放行: {bad!r}")
            self.assertTrue(isinstance(reason, str) and reason)


class ExpandScheduleIntentBoundaries(unittest.TestCase):
    """260921-m3-k3 意图薄载荷截获展开决策面（M3K3-A2 实现自带测）。

    规格 = server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py 的 A2 四齿；
    本文件补规格未逐字段钉的边界/错误路径：恰薄载荷才展开、单源同算、其余原样放行、
    脏 B1 大声 ValueError（P2-A2）、展开产物过 display_schema 机械门。"""

    def setUp(self):
        self.model = schedule.parse_schedule_md(GRID)

    def _expand(self, payload, model=None, week=MON, now=None, school=None):
        return schedule.expand_schedule_intent_payload(
            payload, self.model if model is None else model, week, now, school)

    def test_thin_intent_expands_single_source(self):
        expanded = self._expand({"view": "schedule", "data": {}}, now="17:00",
                                school="08:00")
        expected = schedule.week_grid_payload(
            self.model, MON, "17:00", "08:00")
        self.assertEqual(expanded, expected)          # 单源同算，不另起网格
        ok, reason = ds.validate_display(expanded)
        self.assertTrue(ok, reason)                   # 展开产物过真实机械门

    def test_thin_without_now_school_matches_grid(self):
        expanded = self._expand({"view": "schedule", "data": {}})
        self.assertEqual(
            expanded, schedule.week_grid_payload(self.model, MON))

    def test_only_exact_thin_payload_intercepts(self):
        full = schedule.week_grid_payload(self.model, MON)
        cases = [
            ("full week grid", full),
            ("legacy shape", {"view": "schedule",
                              "data": {"title": "今天", "items": []}}),
            ("missing data", {"view": "schedule"}),
            ("data not empty", {"view": "schedule",
                                "data": {"week": {"courses": []}}}),
            ("data not dict", {"view": "schedule", "data": "[]"}),
            ("non-schedule view", {"view": "text", "data": {"text": "hi"}}),
            ("not a dict", "```display\n{view:schedule}\n```"),
        ]
        for label, payload in cases:
            self.assertIsNone(self._expand(payload),
                              f"非薄载荷被误截获: {label}")

    def test_data_emptied_equivalent(self):
        """data == {} 的等价空 dict（A2 冻结契约按 == 判）也能展开。"""
        self.assertIsNotNone(self._expand({"view": "schedule", "data": dict()}))

    def test_dirty_b1_valueerror_loud(self):
        """脏 B1（坏 week_date_iso / 坏 model 触发 producer 校验）→ 沿 week_grid_payload
        的 ValueError 大声传播，绝不静默产空网格（P2-A2）。"""
        dirty = GRID.replace("| 周一 | 16:30", "| 周九 | 16:30")
        with self.assertRaises(ValueError):
            schedule.parse_schedule_md(dirty)   # 脏星期在 parse 层即大声拒绝
        # expand 链自身：坏 week_date_iso 触发 week_grid_payload 校验 -> ValueError 上抛，
        # 不是返回 None（不静默降级产空网格）。
        with self.assertRaises(ValueError):
            self._expand({"view": "schedule", "data": {}}, week="not-a-date")

    def test_wire_roundtrip_preserved(self):
        expanded = self._expand({"view": "schedule", "data": {}})
        wire = json.dumps(expanded["data"], ensure_ascii=False)
        self.assertEqual(json.loads(wire), expanded["data"])


if __name__ == "__main__":
    unittest.main(verbosity=2)