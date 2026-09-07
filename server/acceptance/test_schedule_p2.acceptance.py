"""Frozen acceptance tests for TaskPackage 260829-schedule-p2 (subsystem: server).

Scope of record (taskpackage acceptance/exitCriteria + intake hard red lines):
P2-A2 cloud face (B1 课表 MD 解析 + 按今天星期几/当前时间筛选 + 今天/明天列表产出),
决策 26 producer「接下来」分组计算 (包 6 = producer, 包 5 home-p1 只消费 — 断言钉
「producer 产出的分组结果符合契约 §1.3 ② schedule view 形状」, 不钉 home-p1 渲染内部),
P2-A6 录入→写入 schedule.md→查询/渲染正确消费链路, P2-E4 remind_before_min 废止
不被消费.  课表提醒触发逻辑不在本包 (归包 7, P2-E4) — nothing here schedules alerts.

FROZEN OFFLINE CONTRACT (builder MUST implement, pure python, no fastapi/gateway
imports at import time, offline-importable anywhere; recommended home:
server/gateway/schedule.py — same pure-module precedent as gateway/display_schema.py):

  parse_schedule_md(md_text: str) -> dict
      Parse contract §二 B1 schedule.md.  Blocks located by heading text
      (「每周固定课表」/「一次性安排」/「学校课程表」; a missing block -> empty
      list — 学校课程表 is optional per B1).  Returns:
        {
          "child": <frontmatter child, "" if absent>,
          "timezone": <frontmatter timezone, "" if absent>,
          "weekly":  [ {"weekday": <int 1..7, 周一=1..周日=7, 周天 accepted as 7>,
                        "start": "HH:MM", "end": "HH:MM"|None,
                        "name": <课程>, "location": <地点>, "note": <备注>} ],
          "onetime": [ {"date": "YYYY-MM-DD", "start","end","name","location","note"} ],
          "school":  [ {"weekday": <int>, "periods": [<course names, 第1节..第N节>]} ],
        }
      Frontmatter: child/timezone parsed; ALL other frontmatter keys (including
      the DEPRECATED remind_before_min) are ignored and NEVER surface in the
      model (P2-E4).  Time cell: "HH:MM-HH:MM" or "HH:MM" (end=None).  Table
      mechanics: header row then a dashes separator (skipped) then data rows; a
      data row whose cell count != header cell count raises ValueError; invalid
      weekday/date/time tokens raise ValueError (dirty rows are REJECTED loudly,
      never silently dropped).

  items_for_date(model, date_iso: str) -> list[dict]
      All items that calendar date, each EXACTLY {"time": <start "HH:MM">,
      "name", "location"} (contract §1.3 ② item shape), sorted by time ascending
      (stable: weekly block order before onetime block order at equal time).
      weekly rows match weekday(date_iso); onetime rows match date == date_iso —
      hence a one-off dated BEFORE the queried date (过期一次性安排) NEVER
      appears (契约 §2.3 过期自动忽略).

  items_after(model, date_iso: str, now_hhmm: str) -> list[dict]
      items_for_date filtered to start >= now_hhmm (「今天下午有什么课」-style
      current-time filter, P2-A2).

  schedule_view_payload(title: str, items: list[dict]) -> dict
      {"view": "schedule", "data": {"title": title, "items": items}} — the
      contract §1.3 ② producer form.  items must already be the exact item
      shape; no extra keys are invented (no new view, no new field, no 反白态 —
      契约零变更, intake hard red line).  MUST pass
      gateway.display_schema.validate_display (asserted below against the REAL
      frozen validator).

  group_upcoming(model, date_iso: str, now_hhmm: str, school_start: str|None = None) -> list[dict]
      决策 26 producer: P1「接下来」分组计算 (spec S-F1.1 / PROPOSAL §2 包 6).
      - 周中 (Mon..Fri) 且学校课程表有该星期行 且 school_start 给定 -> ONE merged
        item {"time": school_start, "name": "上学", "location": ""} (时刻取学校
        课程表首节; B1 学校课程表无时刻列, 首节时刻 = producer 配置入参 — B1 表体
        零变更).  周六/周日 never merges (周中白天 scope).
      - 培训班 (weekly) + 一次性安排 listed individually, same item shape.
      - Upcoming rule as of now_hhmm: an item with a known end is kept while
        end > now (in-progress counts as upcoming); a start-only item (and the
        merged 上学 item) is kept while start >= now.  Already-ended items are
        dropped.
      - Output sorted by time ascending (merged 上学 participates in the sort).
      Every grouped item is the exact contract item shape so the consumer
      (home-p1) can wrap it via schedule_view_payload and pass the gateway
      schema gate (asserted below).

DEPRECATED FIELD (P2-E4): remind_before_min is 已废止 (契约 §2.3, 2026-09-03
契约门裁定).  Asserted BOTH behaviorally (parse output invariant under changing
its value; never surfaces in the model) and as the intake red line behind the
reminder trigger logic being out of package scope (归包 7).

Live face (@pytest.mark.live, NOT auto-run — conftest mechanical skip gate,
ARQ-260901-g3): P2-A6 真录一轮的证据断言, evidence lands under
changes/dev/260829-schedule-p2/ (operator-produced md).  Three facts:
  ① QQ 录入指令事实 (家长经 QQ 对 OpenClaw 说录入内容);
  ② schedule.md 改写事实 (OpenClaw 自改写写入课表 MD, 即时触发重算;
     三档阶梯本身归包 7 — 证据只须证明改写与触发发生);
  ③ 查询/渲染消费事实 (孩子问课, OpenClaw 读改写后的 MD 正确答复 / 下发
     schedule view).

Coverage map (acceptanceRef / exitCriteria / decisionCoverage -> test):
  P2-A2  -> test_a2_parse_b1_three_blocks
          -> test_a2_parse_rejects_malformed_rows
          -> test_a2_filter_today_tomorrow_weekday_and_expired_ignored
          -> test_a2_time_filter_items_after
  P2-A3 (producer shape) / contract-§1.3② -> test_payload_shape_conforms_contract
  P2-A5 (producer 对接口径: 决策 26 分组结果符合契约形状) / spec-决策26
         -> test_decision26_grouping_school_merged_and_individual
         -> test_decision26_grouping_upcoming_rules
  P2-A6  -> test_a6_entry_write_consume_chain
          -> test_a6_prompt_carrier_v2_landed_and_wired (structural)
          -> test_a6_live_qq_entry_evidence (live, operator evidence)
  P2-E4  -> test_e4_remind_before_min_not_consumed
  boundary (契约零变更: 不发明 view / 不加字段) -> test_boundary_no_view_invention
  P2-E2 (两面证据: 本文件 + firmware/main/test/schedule_p2.acceptance.c;
         既有验收回归守卫) -> test_e1_existing_directionb_acceptance_green

Red baseline (freeze state, 2026-09-07 acceptance-author 实测): server/gateway
has NO schedule module — every offline tooth fails for exactly the missing
behaviour of record (frozen offline contract not implemented / producer carrier
v2 not landed); collection is error-free (冻结即冒烟).  See
firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md
for both faces.

decisionCoverage landing (taskpackage 260829-schedule-p2):
  PROPOSAL-§2-包6       -> P2-A2 teeth + 决策 26 producer teeth (this file)
  contract-§二-B1       -> P2-A2 parse/filter teeth (this file)
  spec-§4-砍当前课反白  -> boundary tooth (no extra fields / no 反白态, this file
                           + C 侧 schedule_p2.acceptance.c)
  contract-待确认点2-已实证解除 -> P2-E1 structural: B1 可解析由 parse teeth 承载

Run protocol:
  cd server && python -m pytest acceptance/test_schedule_p2.acceptance.py -q --import-mode=importlib
  (two-dot filename needs --import-mode=importlib, G 包先例; live class skipped
  unless RUN_LIVE=1 / KSP2_RUN_LIVE=1 or -m live explicitly requested)
  standalone: python acceptance/test_schedule_p2.acceptance.py
"""

from __future__ import annotations

import importlib.util
import json
import os
import subprocess
import sys
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; the standalone runner also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
SCHEDULE_MODULE = GATEWAY_DIR / "schedule.py"
DEPLOY_DIR = SERVER / "deploy"
PROMPT_CARRIER_V2 = DEPLOY_DIR / "openclaw-system-prompt.v2.md"
OVERLAY_WIRING = DEPLOY_DIR / "apply-xiaozhi-overlay.sh"
LIVE_EVIDENCE_DIR = REPO_ROOT / "changes" / "dev" / "260829-schedule-p2"

# --------------------------------------------------------------------------- #
# Frozen B1 sample (contract §2.2 verbatim structure) + fixed weekday dates    #
# (2026-09: 05=Sat, 07=Mon, 08=Tue, 09=Wed, 12=Sat — date.weekday() cross-     #
# checked 2026-09-07).                                                        #
# --------------------------------------------------------------------------- #

B1_CANONICAL = """---
child: 小明
timezone: Asia/Shanghai
remind_before_min: 15
---

# 小明的课表

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周三 | 19:00-20:00 | 英语培训 | 新东方 | |
| 周六 | 10:00-11:30 | 美术班 | 少年宫 | 带画笔 |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-08-30 | 14:00-15:00 | 牙医复诊 | 口腔医院 | |
| 2026-09-05 | 09:00 | 开学报到 | 学校 | 带暑假作业 |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 | 第4节 | 第5节 | 第6节 |
|------|-------|-------|-------|-------|-------|-------|
| 周一 | 语文 | 数学 | 英语 | 体育 | 美术 | 自习 |
"""

B1_SCHOOL_MON_WED = """---
child: 小明
timezone: Asia/Shanghai
---

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周三 | 19:00-20:00 | 英语培训 | 新东方 | |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 |
|------|-------|-------|-------|
| 周一 | 语文 | 数学 | 英语 |
| 周三 | 数学 | 语文 | 体育 |
"""

MON = "2026-09-07"
TUE = "2026-09-08"
WED = "2026-09-09"
SAT = "2026-09-12"

# --------------------------------------------------------------------------- #
# Offline loading of the frozen schedule module (deterministic red on missing) #
# --------------------------------------------------------------------------- #

_schedule_module_cache = None


def _load_schedule_module():
    """Import server/gateway/schedule.py by path, offline.

    Returns (module|None, err).  A missing/unimportable module is a RED for
    missing behaviour (self.fail), never a collection crash.
    """
    global _schedule_module_cache
    if _schedule_module_cache is not None:
        return _schedule_module_cache
    if not SCHEDULE_MODULE.is_file():
        return None, ("server/gateway/schedule.py missing — the B1 课表 MD 解析/筛选/"
                      "决策 26 producer 分组 pure module is not implemented (frozen "
                      "offline contract).")
    try:
        spec = importlib.util.spec_from_file_location("kidpal_gateway_schedule", SCHEDULE_MODULE)
        if spec is None or spec.loader is None:
            return None, "server/gateway/schedule.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _schedule_module_cache = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        return None, (f"server/gateway/schedule.py failed to import: {e!r}. The frozen "
                      "schedule module MUST be pure python (no fastapi/gateway imports "
                      "at import time) so this acceptance runs offline anywhere.")


def _schedule_callable(name: str):
    """Fetch one frozen-contract callable; (callable|None, err)."""
    mod, err = _load_schedule_module()
    if mod is None:
        return None, err
    fn = getattr(mod, name, None)
    if not callable(fn):
        return None, (f"schedule.py has no module-level callable {name}() — frozen "
                      "offline contract not implemented.")
    return fn, None


def _display_schema():
    """The REAL frozen gateway display validator (V27) — shape gate of record."""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415 — offline import, cached by python
    return ds


# --------------------------------------------------------------------------- #
# Offline structural / behavioral acceptance — the red baseline teeth          #
# --------------------------------------------------------------------------- #

class ScheduleP2OfflineAcceptance(unittest.TestCase):
    """Offline teeth: B1 parse/filter, producer payload shape, 决策 26 grouping,
    entry consumption chain, remind_before_min 废止, regression guards."""

    maxDiff = None

    # --- P2-A2 ---------------------------------------------------------------
    def test_a2_parse_b1_three_blocks(self):
        """coverage: P2-A2 / contract-§二-B1 — B1 课表 MD 三类区块解析, 逐字段保真."""
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("P2-A2 RED: " + err)
        model = parse(B1_CANONICAL)

        self.assertIsInstance(model, dict, "P2-A2 RED: parse_schedule_md must return a dict model.")
        self.assertEqual(model.get("child"), "小明", "P2-A2 RED: frontmatter child not parsed.")
        self.assertEqual(model.get("timezone"), "Asia/Shanghai",
                         "P2-A2 RED: frontmatter timezone not parsed.")

        weekly = model.get("weekly")
        self.assertEqual(
            len(weekly), 3,
            f"P2-A2 RED: 每周固定课表 must parse 3 rows, got {weekly!r}.")
        self.assertEqual(
            weekly[0],
            {"weekday": 1, "start": "16:30", "end": "17:30", "name": "钢琴课",
             "location": "琴行", "note": "带琴谱"},
            f"P2-A2 RED: weekly row 0 wrong: {weekly[0]!r} — 星期/时间(HH:MM-HH:MM)/"
            "课程/地点/备注 逐字段保真 (契约 §2.2).")
        self.assertEqual(weekly[1]["weekday"], 3, "P2-A2 RED: 周三 must map to weekday 3.")
        self.assertEqual(weekly[1]["name"], "英语培训", "P2-A2 RED: weekly row 1 name wrong.")
        self.assertEqual(weekly[2]["weekday"], 6,
                         "P2-A2 RED: 周六 must map to weekday 6 (周一=1..周日=7).")

        onetime = model.get("onetime")
        self.assertEqual(
            len(onetime), 2,
            f"P2-A2 RED: 一次性安排 must parse 2 rows, got {onetime!r}.")
        self.assertEqual(
            onetime[0],
            {"date": "2026-08-30", "start": "14:00", "end": "15:00", "name": "牙医复诊",
             "location": "口腔医院", "note": ""},
            f"P2-A2 RED: onetime row 0 wrong: {onetime[0]!r} (日期 YYYY-MM-DD 契约 §2.3).")
        self.assertEqual(onetime[1]["end"], None,
                         "P2-A2 RED: '09:00' start-only time must parse with end=None.")

        school = model.get("school")
        self.assertEqual(
            len(school), 1, f"P2-A2 RED: 学校课程表 must parse 1 row, got {school!r}.")
        self.assertEqual(school[0]["weekday"], 1, "P2-A2 RED: school row weekday wrong.")
        self.assertEqual(
            school[0]["periods"], ["语文", "数学", "英语", "体育", "美术", "自习"],
            f"P2-A2 RED: school periods wrong: {school[0]['periods']!r} (第1节..第6节).")

        # 学校课程表 optional: a B1 doc without the block parses with school == [].
        model_wo = parse(B1_CANONICAL.split("## 学校课程表")[0])
        self.assertEqual(model_wo.get("school"), [],
                         "P2-A2 RED: missing optional 学校课程表 block must parse as [].")

    def test_a2_parse_rejects_malformed_rows(self):
        """coverage: P2-A2 — 脏行大声拒绝 (ValueError), 绝不静默丢行."""
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("P2-A2 RED: " + err)
        bad_column = B1_CANONICAL.replace(
            "| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |", "| 周一 | 16:30-17:30 | 钢琴课 |")
        with self.assertRaises(ValueError,
                               msg="P2-A2 RED: a column-count-mismatched row must raise "
                                   "ValueError (脏行拒绝, 不静默丢)."):
            parse(bad_column)
        bad_weekday = B1_CANONICAL.replace("| 周三 | 19:00", "| 周九 | 19:00")
        with self.assertRaises(ValueError,
                               msg="P2-A2 RED: an invalid weekday token (周九) must raise "
                                   "ValueError (契约 §2.3 星期取 周一~周日)."):
            parse(bad_weekday)
        bad_date = B1_CANONICAL.replace("2026-08-30", "2026/08/30")
        with self.assertRaises(ValueError,
                               msg="P2-A2 RED: a non-YYYY-MM-DD date must raise ValueError "
                                   "(契约 §2.3 日期格式)."):
            parse(bad_date)

    def test_a2_filter_today_tomorrow_weekday_and_expired_ignored(self):
        """coverage: P2-A2 — 按今天星期几筛选 + 过期一次性安排自动忽略 + 今天/明天产出."""
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("P2-A2 RED: " + err)
        for_date, err2 = _schedule_callable("items_for_date")
        if for_date is None:
            self.fail("P2-A2 RED: " + err2)
        model = parse(B1_CANONICAL)

        # Monday 2026-09-07: weekly 钢琴课 + today-dated one-off (added variant) —
        # the canonical doc's today list is 钢琴课 only.
        monday = for_date(model, MON)
        self.assertEqual(
            monday, [{"time": "16:30", "name": "钢琴课", "location": "琴行"}],
            f"P2-A2 RED: Monday list wrong: {monday!r} — 每周固定按星期匹配, item 形状 = "
            "契约 §1.3 ② {time,name,location} (时间=开始时刻).")

        # Tuesday: nothing scheduled.
        self.assertEqual(for_date(model, TUE), [],
                         "P2-A2 RED: Tuesday must be an empty list (无课日空表).")

        # Wednesday: 英语培训 19:00.
        wed = for_date(model, WED)
        self.assertEqual(
            wed, [{"time": "19:00", "name": "英语培训", "location": "新东方"}],
            f"P2-A2 RED: Wednesday list wrong: {wed!r}.")

        # 过期一次性安排自动忽略 (契约 §2.3): 开学报到 (2026-09-05, Sat) and 牙医复诊
        # (2026-08-30) NEVER appear in later-day queries.
        for late in (MON, TUE, WED, SAT):
            names = [i["name"] for i in for_date(model, late)]
            self.assertNotIn("开学报到", names,
                             f"P2-A2 RED: expired one-off 开学报到 (2026-09-05) leaked into "
                             f"{late} — 过期自动忽略缺失 (契约 §2.3).")
            self.assertNotIn("牙医复诊", names,
                             f"P2-A2 RED: expired one-off 牙医复诊 (2026-08-30) leaked into "
                             f"{late}.")
        # Saturday query: 美术班 (weekly 周六) still there; the PAST one-off on the same
        # calendar weekday does not resurrect.
        sat = for_date(model, SAT)
        self.assertEqual(
            sat, [{"time": "10:00", "name": "美术班", "location": "少年宫"}],
            f"P2-A2 RED: Saturday list wrong: {sat!r} — 一次性安排只在 date 精确匹配当日.")

    def test_a2_time_filter_items_after(self):
        """coverage: P2-A2 — 按当前时间筛选 (「今天下午有什么课」语义 = start >= now)."""
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("P2-A2 RED: " + err)
        after, err2 = _schedule_callable("items_after")
        if after is None:
            self.fail("P2-A2 RED: " + err2)
        model = parse(B1_CANONICAL)

        afternoon = after(model, MON, "15:00")
        self.assertEqual(
            afternoon, [{"time": "16:30", "name": "钢琴课", "location": "琴行"}],
            f"P2-A2 RED: items_after(Mon, 15:00) wrong: {afternoon!r} — 14:00 前的项被"
            "过滤、16:30 待上保留 (按当前时间筛选 P2-A2).")
        self.assertEqual(after(model, MON, "17:00"), [],
                         "P2-A2 RED: after the day's last start there must be nothing left.")
        everything = after(model, MON, "00:00")
        self.assertEqual(everything, [{"time": "16:30", "name": "钢琴课", "location": "琴行"}],
                         "P2-A2 RED: now=00:00 must keep the whole day (filter semantics).")

    # --- P2-A3 producer shape / 契约 §1.3 ② -----------------------------------
    def test_payload_shape_conforms_contract(self):
        """coverage: P2-A3/契约 §1.3 ② — producer 载荷形状 = {view:schedule, data:{title,items}},
        逐项 keys 恰为 {time,name,location}, 且过真实 display_schema.validate_display 门."""
        payload_fn, err = _schedule_callable("schedule_view_payload")
        if payload_fn is None:
            self.fail("P2-A2 RED: " + err)
        items = [{"time": "14:00", "name": "钢琴课", "location": "琴行"},
                 {"time": "16:30", "name": "英语培训", "location": "新东方"}]
        payload = payload_fn("今天下午", items)
        self.assertEqual(
            payload,
            {"view": "schedule",
             "data": {"title": "今天下午", "items": [{"time": "14:00", "name": "钢琴课",
                                                      "location": "琴行"},
                                                     {"time": "16:30", "name": "英语培训",
                                                      "location": "新东方"}]}},
            f"P2-A3 RED: payload not the contract §1.3 ② form: {payload!r} — "
            "{view:schedule, data:{title, items:[{time,name,location}]}} 零增减.")
        for item in payload["data"]["items"]:
            self.assertEqual(
                set(item), {"time", "name", "location"},
                f"P2-A3 RED: item keys {sorted(item)} != {{time,name,location}} — 契约零变更, "
                "不得新增字段 (反白态/星标态等 UI 语义不得进 data 形状).")
        ds = _display_schema()
        ok, reason = ds.validate_display(payload)
        self.assertTrue(ok, f"P2-A3 RED: producer payload failed the REAL gateway schema "
                            f"gate ({reason}) — producer 输出必须能原样过机械校验下发.")
        # empty items stays a valid page
        empty = payload_fn("今天", [])
        ok2, reason2 = ds.validate_display(empty)
        self.assertTrue(ok2, f"P2-A3 RED: empty-schedule payload failed schema ({reason2}).")

    # --- 决策 26 producer grouping -------------------------------------------
    def test_decision26_grouping_school_merged_and_individual(self):
        """coverage: spec-决策26 / PROPOSAL-§2-包6 — 周中白天学校课合并「上学」取首节时刻,
        培训班/一次性逐项列出; 输出符合契约 item 形状并可整包过 schema 门."""
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("P2-A2 RED: " + err)
        group, err2 = _schedule_callable("group_upcoming")
        if group is None:
            self.fail("P2-A2 RED: " + err2)
        model = parse(B1_SCHOOL_MON_WED)

        # Monday morning: merged 上学 (school first period 08:00) + 钢琴课 individual.
        monday = group(model, MON, "12:00", school_start="08:00")
        self.assertEqual(
            monday, [{"time": "08:00", "name": "上学", "location": ""},
                     {"time": "16:30", "name": "钢琴课", "location": "琴行"}],
            f"决策 26 RED: Monday grouping wrong: {monday!r} — 周中白天学校课合并为 "
            "「08:00 上学」(首节时刻), 培训班逐项列出 (spec S-F1.1 示例口径).")

        # 周六 school row exists in B1_CANONICAL but weekends NEVER merge (周中 scope).
        model_full = parse(B1_CANONICAL)
        saturday = group(model_full, SAT, "08:00", school_start="08:00")
        self.assertEqual(
            [g["name"] for g in saturday], ["美术班"],
            f"决策 26 RED: Saturday grouping wrong: {saturday!r} — 周末不合并上学项 "
            "(周中白天的学校课才合并).")

        # school_start=None -> no merged item even on a school weekday.
        no_school = group(model, MON, "12:00", school_start=None)
        self.assertEqual(
            no_school, [{"time": "16:30", "name": "钢琴课", "location": "琴行"}],
            f"决策 26 RED: school_start=None must omit the merged 上学 item: {no_school!r}.")

        # Producer output conforms to the contract shape AND passes the schema gate
        # (intake red line: 钉「分组结果符合契约 schedule view 形状」即可).
        payload_fn, err3 = _schedule_callable("schedule_view_payload")
        if payload_fn is None:
            self.fail("P2-A2 RED: " + err3)
        payload = payload_fn("今天", monday)
        for item in payload["data"]["items"]:
            self.assertEqual(set(item), {"time", "name", "location"},
                             f"决策 26 RED: grouped item keys {sorted(item)} — 分组结果必须"
                             "保持契约 item 形状, 不钉 home-p1 渲染内部.")
        ds = _display_schema()
        ok, reason = ds.validate_display(payload)
        self.assertTrue(ok, f"决策 26 RED: grouped producer payload failed the schema gate "
                            f"({reason}).")

    def test_decision26_grouping_upcoming_rules(self):
        """coverage: spec-决策26 — upcoming 规则: 进行中保留、已结束剔除、start-only 按
        start 判、合并项同样受 upcoming 约束."""
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("P2-A2 RED: " + err)
        group, err2 = _schedule_callable("group_upcoming")
        if group is None:
            self.fail("P2-A2 RED: " + err2)
        model = parse(B1_SCHOOL_MON_WED)

        # 17:00 Monday: 钢琴课 16:30-17:30 in progress -> kept; 上学 (08:00, no end,
        # start < now) -> dropped.
        at_1700 = group(model, MON, "17:00", school_start="08:00")
        self.assertEqual(
            at_1700, [{"time": "16:30", "name": "钢琴课", "location": "琴行"}],
            f"决策 26 RED: in-progress item must stay upcoming, ended/no-end-start items "
            f"dropped: {at_1700!r}.")

        # 18:00 Monday: 钢琴课 ended -> nothing left.
        self.assertEqual(group(model, MON, "18:00", school_start="08:00"), [],
                         "决策 26 RED: after everything ended the upcoming list must be "
                         "empty (接下来不回看已结束项).")

        # 07:30 Monday: both the merged 上学 (08:00 >= 07:30) and 钢琴课 upcoming.
        early = group(model, MON, "07:30", school_start="08:00")
        self.assertEqual(
            early, [{"time": "08:00", "name": "上学", "location": ""},
                    {"time": "16:30", "name": "钢琴课", "location": "琴行"}],
            f"决策 26 RED: early-morning grouping wrong: {early!r}.")

    # --- P2-A6 ---------------------------------------------------------------
    def test_a6_entry_write_consume_chain(self):
        """coverage: P2-A6 — 录入→写入 schedule.md→查询/渲染正确消费: 真 tmp 文件落盘
        (真实持久化副作用), 再读回解析消费, producer 载荷过真实 schema 门."""
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("P2-A2 RED: " + err)
        for_date, err2 = _schedule_callable("items_for_date")
        if for_date is None:
            self.fail("P2-A2 RED: " + err2)
        payload_fn, err3 = _schedule_callable("schedule_view_payload")
        if payload_fn is None:
            self.fail("P2-A2 RED: " + err3)
        group, err4 = _schedule_callable("group_upcoming")
        if group is None:
            self.fail("P2-A2 RED: " + err4)

        md_path = Path(self.enterContext(_TmpDir())) / "schedule.md"
        before = B1_CANONICAL.split("## 一次性安排")[0]  # 周三英语培训尚未录入的课表
        md_path.write_text(before, encoding="utf-8")

        # 家长 QQ 录入「每周三 16:30 英语培训」→ OpenClaw 自改写 → schedule.md 落盘
        # (B1 表体零变更: 新行按 §2.2 每周固定课表列式写入).  本测试钉消费端: 写入的
        # MD 必须能被 parse/filter/payload 链路正确消费.
        entry_row = "| 周三 | 16:30-17:30 | 英语培训 | 新东方 | |\n"
        text = md_path.read_text(encoding="utf-8")
        lines = text.splitlines(keepends=True)
        idx = max(i for i, ln in enumerate(lines) if ln.startswith("| 周六"))
        lines.insert(idx + 1, entry_row)
        md_path.write_text("".join(lines), encoding="utf-8")  # REAL file write

        self.assertTrue(md_path.is_file() and md_path.stat().st_size > 0,
                        "P2-A6 RED: schedule.md not persisted to disk.")
        model = parse(md_path.read_text(encoding="utf-8"))
        wed = for_date(model, WED)
        self.assertIn(
            {"time": "16:30", "name": "英语培训", "location": "新东方"}, wed,
            f"P2-A6 RED: the entered course (每周三 16:30 英语培训) is not consumable from "
            f"the rewritten schedule.md: {wed!r} — 录入→写入→查询消费链路断裂 (S-F1.5).")
        payload = payload_fn("周三", wed)
        ds = _display_schema()
        ok, reason = ds.validate_display(payload)
        self.assertTrue(ok, f"P2-A6 RED: consumed payload failed the schema gate ({reason}).")
        grouped = group(model, WED, "12:00", school_start="08:00")
        self.assertIn(
            {"time": "16:30", "name": "英语培训", "location": "新东方"}, grouped,
            f"P2-A6 RED: the entered course missing from 决策 26 grouping: {grouped!r} — "
            "QQ 录入与「接下来」同源 (决策 26 producer, V22).")

    def test_a6_prompt_carrier_v2_landed_and_wired(self):
        """coverage: P2-A6 structural — producer prompt 载体按版本化落盘惯例以 v2 新文件
        落盘并接 overlay 渲染 (内容不锁 — 验收锁行为, 行为由上面的 pure-logic 齿承载)."""
        self.assertTrue(
            PROMPT_CARRIER_V2.is_file(),
            "P2-A6 RED: server/deploy/openclaw-system-prompt.v2.md missing — 课表 producer"
            "(决策 26 分组 + QQ 录入改写约定) 的 prompt 载体未按版本化惯例落盘 (v2 新文件).")
        self.assertTrue(
            PROMPT_CARRIER_V2.read_text(encoding="utf-8", errors="replace").strip(),
            "P2-A6 RED: openclaw-system-prompt.v2.md is empty.")
        wiring = "\n".join(
            p.read_text(encoding="utf-8", errors="replace")
            for p in sorted(DEPLOY_DIR.rglob("*")) if p.is_file())
        self.assertIn(
            "openclaw-system-prompt.v2.md", wiring,
            "P2-A6 RED: server/deploy wiring never references openclaw-system-prompt.v2.md — "
            "overlay 渲染未切换到 v2 (「验收所测 = 生产所用」断裂).")

    # --- P2-E4 ---------------------------------------------------------------
    def test_e4_remind_before_min_not_consumed(self):
        """coverage: P2-E4 — remind_before_min 已废止 (契约 §2.3): 行为不变式 + 模型
        不外露; 提醒触发逻辑不进本包 (归包 7)."""
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("P2-A2 RED: " + err)
        base = parse(B1_CANONICAL)
        mutated = parse(B1_CANONICAL.replace("remind_before_min: 15", "remind_before_min: 9999"))
        self.assertEqual(
            base, mutated,
            "P2-E4 RED: changing remind_before_min changed the parse model — 该字段已废止、"
            "不被消费 (2026-09-03 契约门裁定); 消费即双源漂移.")
        for key in base:
            self.assertNotIn("remind_before_min", key,
                             "P2-E4 RED: remind_before_min surfaced in the model keys.")
        self.assertNotIn(
            "remind_before_min", json.dumps(base, ensure_ascii=False),
            "P2-E4 RED: remind_before_min value leaked into the serialised model.")

    # --- boundary -------------------------------------------------------------
    def test_boundary_no_view_invention(self):
        """coverage: boundary (intake 红线: 契约零变更) — view 白名单不变、本包产出只
        发明 schedule 形状内的东西; V27 校验器冻结面在世."""
        ds = _display_schema()
        self.assertEqual(
            ds.VALID_VIEWS,
            {"char", "schedule", "story_list", "story_text", "text", "home", "control"},
            f"boundary RED: view whitelist drifted: {ds.VALID_VIEWS!r} — 契约零变更红线 "
            "(不发明新 view).")
        self.assertEqual(
            ds.MAX_DATA_BYTES, 65536,
            "boundary RED: MAX_DATA_BYTES drifted — V27 机械校验冻结面被动.")

    # --- P2-E2 / regression guard ---------------------------------------------
    def test_e1_existing_directionb_acceptance_green(self):
        """coverage: P2-E2 — 既有冻结验收回归守卫: gateway-directionB offline 套件独立
        复跑 exit 0 (本包新文件不得破坏并行验收面).  守卫目标 = directionB 套件
        (GRF-E1 同款); voice_roundtrip_b 套件的 VRT-A4 证据齿在本包冻结时点存在
        与本包无关的先行红 (adherence record 处置条目), 不选它作守卫目标以免守卫
        恒红 — 如实记录于红基线摘要."""
        target = SERVER / "acceptance" / "test_gateway_directionB.acceptance.py"
        proc = subprocess.run(
            [sys.executable, str(target)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=600, cwd=str(SERVER))
        self.assertEqual(
            proc.returncode, 0,
            "P2-E2 RED: the existing 260903-gateway-directionB acceptance regressed.\n"
            f"stdout tail: {proc.stdout[-1500:]!r}\nstderr tail: {proc.stderr[-800:]!r}")


class _TmpDir:
    """Minimal context-manager temp dir (keeps the file usable without tempfile
    import noise in the frozen file)."""

    def __enter__(self):
        import tempfile  # noqa: PLC0415
        self._path = tempfile.mkdtemp(prefix="ksp2_a6_")
        return Path(self._path)

    def __exit__(self, *exc):
        import shutil  # noqa: PLC0415
        shutil.rmtree(self._path, ignore_errors=True)
        return False


# --------------------------------------------------------------------------- #
# P2-A6 live face — QQ 录入真录一轮证据（不 auto-run, ARQ-260901-g3）            #
# --------------------------------------------------------------------------- #

class ScheduleP2LiveAcceptance(unittest.TestCase):
    """live evidence teeth: read the real QQ-entry smoke evidence under
    changes/dev/260829-schedule-p2/ (operator-produced md; probe ≠ exercise 纪律 —
    必须真走一轮 QQ 录入→改写→孩子问课, 只探测 OpenClaw 可达不算)."""

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _evidence_corpus(self):
        if not LIVE_EVIDENCE_DIR.is_dir():
            return None
        parts = []
        for p in sorted(LIVE_EVIDENCE_DIR.rglob("*")):
            if p.is_file() and p.suffix == ".md":
                try:
                    parts.append(p.read_text(encoding="utf-8", errors="replace"))
                except OSError:
                    continue
        return "\n".join(parts) if parts else None

    def test_a6_live_qq_entry_evidence(self):
        """coverage: P2-A6 live — 三事实: QQ 录入指令 / schedule.md 改写 / 查询渲染消费."""
        corpus = self._evidence_corpus()
        self.assertTrue(
            corpus,
            "P2-A6 live RED: no live evidence 落盘 under changes/dev/260829-schedule-p2/ — "
            "QQ 录入真录一轮（录入→改写→问课消费）的证据必须先落盘, 三事实齿据此核销 "
            "(probe ≠ exercise).")
        entry_markers = ("每周三", "录入", "QQ", "qq")
        self.assertTrue(
            any(m in corpus for m in entry_markers),
            "P2-A6 live RED: evidence does not record the QQ entry instruction fact "
            "(① 录入指令事实缺失).")
        rewrite_markers = ("schedule.md", "改写", "写入", "重算")
        self.assertTrue(
            any(m in corpus for m in rewrite_markers),
            "P2-A6 live RED: evidence does not record the schedule.md rewrite fact "
            "(② OpenClaw 自改写写入事实缺失).")
        consume_markers = ("查询", "答复", "播报", "view", "schedule")
        self.assertTrue(
            any(m in corpus for m in consume_markers),
            "P2-A6 live RED: evidence does not record the query/consume fact "
            "(③ 孩子问课被改写后课表正确答复/下发事实缺失).")
        failure_markers = ("改写失败", "写入失败", "查询不到", "消费失败")
        for marker in failure_markers:
            self.assertNotIn(
                marker, corpus,
                f"P2-A6 live RED: evidence records a FAILURE outcome ({marker!r}) — "
                "录入→消费链路存在真实缺陷, 不得按通过交付.")


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(ScheduleP2OfflineAcceptance)
    if os.environ.get("KSP2_RUN_LIVE") == "1" or os.environ.get("RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(ScheduleP2LiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)
