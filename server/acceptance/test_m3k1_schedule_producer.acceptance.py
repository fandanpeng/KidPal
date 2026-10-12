"""Frozen acceptance tests — TaskPackage 260921-m3-k1-cloud-schedule-producer
(subsystem: server; workstream m3-schedule; mergeOrder 33; rigor full; riskTier medium).

Scope of record (taskpackage acceptance M3K1-A1..A4 + CONTRACT-CHANGE-PACKET §5
human ruling, 2026-09-21 HUMAN_APPROVED):
M3K1-A1 契约门齿 (validate_display 三样本回归 + Q5 旧形状收紧拒绝),
M3K1-A2 B1→周网格计算 (学校课逐节 4+2 / 固定课外课逐门 / 临时安排 ※ / 星标呼应),
M3K1-A3 首页 upcoming payload (group_upcoming 零偏移复用 + 决策 26 合并「上学」
12:00 回归), M3K1-A4 三触发模拟 (建连 / 每日零点 / 录入即时 marker；副通道不即时)。
M3K1-A5 live 三触发复证 = structural deferred 挂 M3-K3 设备轮 —— 本文件**无 live 面**。

Shape authority = CONTRACT-CHANGE-PACKET.md §1/§5 (E1 §1.3② 周网格重定义 +
§1.3⑥ home 可选 data):
  schedule: {"view":"schedule","data":{"week":{"courses":[
              {"day":<0..6 周一=0,即 B1「周一」的 0 基数组下标序>,
               "slot":<0 上午/1 下午/2 晚上>,"name":<课程名>,"flags":<位掩码>}]}}}
    flags: bit0(1)=※ 临时安排(一次性安排区块)、bit1(2)=星标呼应(云端判定的当前
    「接下来」项)。「今天」列不在载荷(设备本地日期推导)；**备注不进载荷**
    (S-F1.2 线框口径：备注露面处 = 语音答复/提醒 speech)。一格多门 = 同
    (day,slot) 多条逐门列出。
  home: {"view":"home","data":{"upcoming":{"day_key":"今天",
          "items":[{"time","name"}],
          "preview":[{"time","name"},…] | [] }}}
    (additive，缺 data 行为不变; 2026-10-11 契约门多项数组化——CCP
    261011-p1-home-v5-rework 触点① HUMAN_APPROVED + requirements v1.61
    S-F1.1 AC2/AC6/AC7: preview = 明日全量 {time,name} 数组按开始时间升序（学校课
    合并「XX:XX 上学」与课外班、临时事件同列, AC2）; `[]` = 云端确认明日无项（AC6
    「明天没有什么课喽」话术的显式 wire 载体——producer 明日无项改发 [] 不再 None,
    plan R1）; producer 恒发数组、不发 null（null/键缺省 = 未提供, 板端数据未达态
    AC7——板端对旧单对象形状保留一个过渡版的解析容忍, producer 不发该形状）。

FROZEN OFFLINE CONTRACT (builder MUST implement; pure python, no fastapi/gateway
imports at import time, offline-importable anywhere):

  server/gateway/schedule.py — 增量两个 producer 函数(既有 parse_schedule_md /
  items_for_date / items_after / schedule_view_payload / group_upcoming 语义零偏移):

    week_grid_payload(model: dict, week_date_iso: str,
                      now_hhmm: str | None = None,
                      school_start: str | None = None) -> dict
      周网格 producer 载荷(上形)。week_date_iso 定所在周一..周日周:
      - 学校课(「学校课程表」区块)逐节: 第 1..4 节 -> slot 0(上午)、第 5..6 节 ->
        slot 1(下午)，逐节各一条，flags 基 0(spec S-F1.2 AC1 上午 4 节/下午 2 节)。
      - 固定课外课(「每周固定课表」)逐门: day = 行 weekday-1(B1 周一=1..周日=7 ->
        载荷 0 基)，slot 按开始时刻落上午/下午/晚上(flags 基 0)。
      - 临时安排(「一次性安排」)逐门: 当周日期落格(同上映射)且 flags |= 1(※)；
        周外日期(已过期/未到)不出现。
      - 星标: now_hhmm 给定时，group_upcoming(model, week_date_iso, now_hhmm,
        school_start) 首项(决策 26 口径)按 name + day(=week_date_iso 的星期-1) +
        slot(其 start 时刻) 匹配网格条目 -> 该条目 flags |= 2；合并「上学」项无
        同名课程、不匹配任何条目(不炸、无星标)；无首项(now=None/当日无 upcoming)
        -> 全网格无 bit1。
      - 输出 {view,data.week.courses} 键集精确；courses 条目键集恰为
        {day,slot,name,flags}；备注不出现。条目顺序不钉(比较用多重集)。

    home_upcoming_payload(model: dict, today_iso: str, now_hhmm: str,
                          tomorrow_iso: str, school_start: str | None = None) -> dict
      home producer 载荷(上形)。items = group_upcoming(model, today_iso, now_hhmm,
      school_start) 逐项剥 location 后的 {time,name}(分组口径**零偏移复用**，
      含决策 26 合并「上学」项 12:00 在校时段计 upcoming 语义)；preview =
      group_upcoming(model, tomorrow_iso, "00:00", school_start) 全量逐项剥 location
      后的 {time,name} 数组(2026-10-11 多项化——明日日程全量进区, 按开始时间升序,
      学校课合并「XX:XX 上学」项在列; **明日无项 -> preview == [] 不再 None**,
      plan R1: None 会使 AC6 话术永不触发)。day_key 恒为 "今天"(payload 即
      home_p1 seam 注入参数 JSON 化)。

  server/gateway/schedule_push.py — 新纯模块(三触发 producer 决策面; 触发调度/
  接线属实现细节，本面只钉「模拟条件下到达且不重复」的可观测决策):

    MARKER_FILENAME = "schedule.refresh"   # 落既有 alerts 交接目录(pending.json 同目录)
                                           # 的跨组件协议常量: OpenClaw 主通道改写
                                           # schedule.md 后落此 marker，front 轮询
                                           # piggyback 消费(取舍点 C 甲案)

    connect_push_payload(model, today_iso, now_hhmm, tomorrow_iso,
                         school_start=None) -> dict
      触发① WS 建连即推的推送内容: 必须 == home_upcoming_payload(同参)
      (三触发单源，不得每触发各算一套)。

    daily_push_due(timezone: str, last_push_local_date: str | None,
                   now_utc: datetime) -> bool
      触发② 每日零点判定。timezone 取 B1 frontmatter(model["timezone"])；语义 =
      now_utc 在该时区的本地日历日 != last_push_local_date 即 due(本地零点后第一
      tick 触发; 调用方记下已推本地日 -> 每 local 日恰一次)。
      last_push_local_date=None -> True(尚未推过)。"Asia/Shanghai" = UTC+8
      (house 先例 story.py _CN_TZ / reminder_schedule_lib._TZ8，B1 唯一在役
      时区)；timezone 为空 -> 默认按 Asia/Shanghai。

    refresh_requested(alerts_dir) -> bool
      触发③ 主通道即时请求是否 pending(alerts_dir 下 MARKER_FILENAME 在场)。
      副通道直改 schedule.md 不落 marker -> 恒 False(S-F1.5 AC2: 副通道无即时
      推送，靠①②兜底；计划否决 mtime 方案正是因其抹平主/副通道判别)。

    consume_refresh_marker(alerts_dir) -> bool
      消费一个 pending 请求: True + marker 移除(恰一次)；无 marker -> False。

FROZEN CONTRACT DECLARATION (M3K1-A1, packet §3 收口义务): Q5 收紧落在 §1.5
「机械 schema 校验」执行点 = gateway.display_schema.validate_display ——
schedule view data 只放行 {} 薄载荷(取舍点 A 甲案: LLM 发薄载荷、front 截获展开)
与新周网格形状；旧 {title,items} 形状拒绝(降级提示，V27 兜底)。

契约条目 ↔ 齿 强度申报 (acceptance-evidence「契约条目 ↔ 齿 的强度申报」体例):
  契约 §1.3② 周网格形状 → A2 齿 → 行为级(逐字段精确断言计算结果)
  契约 §1.3⑥ home data 承载 → A3 齿 → 行为级
  packet Q5 收紧裁定 → A1 legacy-reject 齿 → 行为级(validate_display 真门判定)
  packet Q2 甲案薄载荷 → A1 thin-payload 齿 → 行为级
  arch §5.2 场景①/决策 26 三触发 → A4 齿 → 行为级(**限声明模拟面**)
  弱于契约处(显式缺口): ① 触发①②的 front 真实接线(真 ws 建连帧到达/真零点
  调度)offline 齿钉不到 —— 行为等价假设「front 接线每触发恰调一次声明面」
  **显式化: offline 不可验**，live 复证归 M3-K3(M3K1-A5)；② timezone 换算只钉
  Asia/Shanghai(在役唯一值)，其它 zone 行为不钉；③ 上午/下午/晚上的分界时刻
  契约未定义 —— 齿只钉无歧义样本(10:00→上午、16:30→下午、18:30/19:00→晚上)，
  分界常量由 builder 实现处留痕；④ 学校 >6 节的 slot 归属不钉(spec 口径 4+2)；
  ⑤ 机械门收紧深度钉到「形状 floor」(legacy/缺 courses 拒绝)，逐字段类型校验
  深度不钉(builder 可更深)。

Coverage map (acceptanceRef -> test):
  M3K1-A1 -> test_a1_new_week_grid_shape_passes_schema
           -> test_a1_thin_payload_passes_schema
           -> test_a1_home_with_and_without_data_passes_schema
           -> test_a1_legacy_schedule_shape_rejected_q5
           -> test_a1_malformed_week_shape_rejected
  M3K1-A2 -> test_a2_week_grid_frozen_shape_and_day0_mapping
           -> test_a2_week_grid_onetime_flag_and_out_of_week_filtered
           -> test_a2_week_grid_star_marks_current_upcoming
           -> test_a2_week_grid_flags_compose_bit0_or_bit1
           -> test_a2_week_grid_empty_week_and_dirty_input_boundary
  M3K1-A3 -> test_a3_home_upcoming_payload_frozen_shape
           -> test_a3_decision26_merged_school_1200_regression
           -> test_a3_preview_tomorrow_and_empty_cases
           -> test_a3_preview_multi_item_ascending_school_merged (2026-10-11)
  HV5-AC2 (preview 明日全量进区/升序/「上学」合并项在列, requirements v1.61 AC2)
           -> test_a3_preview_multi_item_ascending_school_merged
  HV5-AC6 (producer 半: 上学日明天区必然非空且含「上学」项——空态话术仅非上学日
           可达的 wire 侧闭环, 防 vacuous) -> test_a3_preview_multi_item_ascending_
           school_merged（上学日场景）+ test_a3_preview_tomorrow_and_empty_cases
           （非上学日无项场景 -> preview == [] = 板端话术态唯一可达 wire 值）
  HV5-AC7 (producer 半: producer 恒发数组不发 None——null 语义退役于 producer,
           「未提供」只可能来自未推送) -> test_a3_preview_tomorrow_and_empty_cases
  M3K1-A4 -> test_a4_connect_push_payload_single_source
           -> test_a4_daily_push_due_local_midnight_b1_timezone
           -> test_a4_refresh_marker_main_channel_only_and_exactly_once
  M3K1-E1 零偏移守卫 -> test_e1_existing_schedule_semantics_zero_offset

Red baseline (现状树, acceptance-author 2026-09-21 实跑，命令见 taskpackage
redBaselineCommand / 交付报告): A2/A3/A4 齿在未实现树上 RED(schedule.py 无
week_grid_payload/home_upcoming_payload、无 schedule_push.py = 真实缺失行为)；
A1 中 legacy-reject / malformed-week 两齿 RED(现行 validate_display 放行旧形状 =
Q5 收紧未实现)，A1 三样本 pass 齿现状绿(must-stay-green —— builder 收紧时不得
打破)；零偏移守卫齿现状绿。收集零错误(冻结即冒烟，DEV-GOVERNANCE §3)。

REVISION LOG (acceptance-author only; 冻结件修订授权链):
  2026-09-21  initial freeze (acceptance-author, TaskPackage 260921-m3-k1-cloud-schedule-producer).
  2026-10-11  冻结件修订（TaskPackage 261011-p1-home-v5-rework; 授权链双引用 =
              「CCP id 261011-p1-home-v5-rework（HUMAN_APPROVED 2026-10-11）+
              docs/requirements.md §9 v1.61 版本行（2026-10-11）」, plan R3）:
              ① M3K1-A3 frozen shape 齿更新——preview 单对象 → 多项数组（AC2 全量
              进区/升序/「上学」合并项在列）+ 明日无项改发 [] 不再 None（plan R1,
              AC6 话术的 wire 载体）; ② 新增 test_a3_preview_multi_item_ascending_
              school_merged（HV5-AC2/AC6 producer 半）; ③ A1 schema 样本随 CCP
              触点①/② 顺修（数组形状 + HH:MM, 「明早」旧漂移收回）; ④ 头注
              FROZEN OFFLINE CONTRACT preview 文本同步。零触碰面：M3K1-A1/A2/A4/
              E1 既有齿断言零偏移（A1 样本形状更新不触 validate_display 判据——
              home view 无 upcoming shape floor, 样本过门性质不变）。

Run protocol:
  python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance
  (两点文件名须 importlib 模式，G 包先例; standalone: python 本文件)
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

try:
    import pytest  # noqa: F401  (markers only; standalone runner also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
SCHEDULE_MODULE = GATEWAY_DIR / "schedule.py"
PUSH_MODULE = GATEWAY_DIR / "schedule_push.py"

# --------------------------------------------------------------------------- #
# Frozen B1 fixtures.  2026-09-21 = Monday (date.weekday()==0, cross-checked
# 2026-09-21 against datetime) -> week = 09-21(Mon)..09-27(Sun); 09-23/09-16/
# 09-30 = Wednesday; 09-27 = Sunday.
# --------------------------------------------------------------------------- #

B1_GRID = """---
child: 小明
timezone: Asia/Shanghai
---

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周三 | 19:00-20:00 | 英语培训 | 新东方 | |
| 周日 | 10:00-11:00 | 美术班 | 少年宫 | 带画笔 |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-09-23 | 18:30-19:30 | 口语课 | 线上 | 提前调试设备 |
| 2026-09-16 | 10:00 | 已过期事项 | 旧周 | |
| 2026-09-30 | 14:00 | 未来事项 | 下周 | |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 | 第4节 | 第5节 | 第6节 |
|------|-------|-------|-------|-------|-------|-------|
| 周一 | 语文 | 数学 | 英语 | 体育 | 美术 | 自习 |
| 周三 | 数学 | 语文 | 体育 | 科学 | 音乐 | 劳动 |
"""

B1_HOME = """---
child: 小明
timezone: Asia/Shanghai
---

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周日 | 10:00-11:00 | 美术班 | 少年宫 | 带画笔 |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-09-21 | 14:00-15:00 | 牙医复诊 | 口腔医院 | |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 |
|------|-------|-------|-------|
| 周一 | 语文 | 数学 | 英语 |
"""

B1_EMPTY = """---
child: 小明
timezone: Asia/Shanghai
---

# 小明的课表
"""

MON = "2026-09-21"   # 周一（day 0）
TUE = "2026-09-22"
WED = "2026-09-23"   # 周三（day 2）
SUN = "2026-09-27"   # 周日（day 6）

UTC = dt_timezone.utc
# Asia/Shanghai = UTC+8: 16:05Z -> 09-22 00:05 本地（零点已过）；15:55Z -> 09-21 23:55。
UTC_LOCAL_CROSSED = datetime(2026, 9, 21, 16, 5, tzinfo=UTC)
UTC_LOCAL_BEFORE_MIDNIGHT = datetime(2026, 9, 21, 15, 55, tzinfo=UTC)

MARKER_NAME = "schedule.refresh"   # 声明面跨组件协议常量（packet 取舍点 C 甲案）


# --------------------------------------------------------------------------- #
# Offline module loading (missing/unimportable = RED for missing behaviour)   #
# --------------------------------------------------------------------------- #

_module_cache: dict = {}


def _load_module(path: Path, modname: str, missing_msg: str):
    cached = _module_cache.get(modname)
    if cached is not None:
        return cached
    if not path.is_file():
        return None, missing_msg
    try:
        spec = importlib.util.spec_from_file_location(modname, path)
        if spec is None or spec.loader is None:
            return None, f"{path} not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _module_cache[modname] = (mod, None)
        return mod, None
    except Exception as e:  # noqa: BLE001 — import error is a RED, not a crash
        return None, (f"{path} failed to import: {e!r} (frozen contract module MUST "
                      "be pure python — no fastapi/gateway imports at import time).")


def _schedule_callable(name: str):
    mod, err = _load_module(
        SCHEDULE_MODULE, "kidpal_gateway_schedule_m3k1",
        "server/gateway/schedule.py missing — B1 解析/producer 纯模块不在位。")
    if mod is None:
        return None, err
    fn = getattr(mod, name, None)
    if not callable(fn):
        return None, (f"schedule.py has no module-level callable {name}() — frozen "
                      "M3K1 offline contract (week grid / home upcoming producer) "
                      "not implemented.")
    return fn, None


def _push_module():
    return _load_module(
        PUSH_MODULE, "kidpal_gateway_schedule_push_m3k1",
        "server/gateway/schedule_push.py missing — 三触发 producer 决策面(触发①②③"
        "模拟契约)未实现。")


def _push_callable(name: str):
    mod, err = _push_module()
    if mod is None:
        return None, err
    fn = getattr(mod, name, None)
    if not callable(fn):
        return None, (f"schedule_push.py has no module-level callable {name}() — "
                      "frozen M3K1 trigger-simulation contract not implemented.")
    return fn, None


def _display_schema():
    """The REAL frozen gateway display validator (V27) — shape gate of record."""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415 — offline import, cached
    return ds


def _parse(md: str):
    parse, err = _schedule_callable("parse_schedule_md")
    if parse is None:
        raise AssertionError("M3K1 RED: " + err)
    return parse(md)


def _grid(model, week_date_iso, now_hhmm=None, school_start=None):
    fn, err = _schedule_callable("week_grid_payload")
    if fn is None:
        raise AssertionError("M3K1-A2 RED: " + err)
    return fn(model, week_date_iso, now_hhmm, school_start)


def _home(model, today_iso, now_hhmm, tomorrow_iso, school_start=None):
    fn, err = _schedule_callable("home_upcoming_payload")
    if fn is None:
        raise AssertionError("M3K1-A3 RED: " + err)
    return fn(model, today_iso, now_hhmm, tomorrow_iso, school_start)


def _courses_multiset(payload) -> set:
    """courses 条目的 (day,slot,name,flags) 多重集（条目顺序不钉）。"""
    courses = payload["data"]["week"]["courses"]
    return {(c["day"], c["slot"], c["name"], c["flags"]) for c in courses}


def _expected_grid_multiset(piano_flags: int = 0, oral_flags: int = 1) -> set:
    """B1_GRID 当周(09-21..09-27)期望网格多重集（见文件头 FROZEN OFFLINE CONTRACT）。

    学校课逐节 4+2、固定课外课逐门按时刻落槽、口语课为当周三一次性安排(※=bit0)。
    piano_flags/oral_flags 供星标用例覆写（bit1 叠加）。"""
    mon_am = [(0, 0, n, 0) for n in ("语文", "数学", "英语", "体育")]
    mon_pm = [(0, 1, n, 0) for n in ("美术", "自习")]
    wed_am = [(2, 0, n, 0) for n in ("数学", "语文", "体育", "科学")]
    wed_pm = [(2, 1, n, 0) for n in ("音乐", "劳动")]
    return (
        set(mon_am) | set(mon_pm) | set(wed_am) | set(wed_pm)
        | {(0, 1, "钢琴课", piano_flags)}      # 周一 16:30 -> 下午
        | {(2, 2, "英语培训", 0)}              # 周三 19:00 -> 晚上
        | {(6, 0, "美术班", 0)}                # 周日 10:00 -> 上午, day=6(周一=0 基)
        | {(2, 2, "口语课", oral_flags)}       # 当周三一次性安排 18:30 -> 晚上, ※
    )


# --------------------------------------------------------------------------- #
# M3K1-A1 — 契约门齿（validate_display 真门; 样本 = packet §1/§5 冻结形状 literal）
# --------------------------------------------------------------------------- #

class M3K1ContractGateAcceptance(unittest.TestCase):
    """M3K1-A1: Q5 收紧 + 甲案薄载荷 + home additive 三样本回归（真实机械门）。"""

    maxDiff = None

    def test_a1_new_week_grid_shape_passes_schema(self):
        """coverage: M3K1-A1 / packet §1 触点① — 冻结周网格样本过真实 validate_display。"""
        sample = {"view": "schedule", "data": {"week": {"courses": [
            {"day": 0, "slot": 0, "name": "语文", "flags": 0},
            {"day": 0, "slot": 2, "name": "口语", "flags": 1},
        ]}}}
        ok, reason = _display_schema().validate_display(sample)
        self.assertTrue(
            ok, f"M3K1-A1 RED: frozen week-grid sample rejected by the real schema "
                f"gate ({reason}) — Q5 收紧不得误杀新形状 (packet §1/§5).")

    def test_a1_thin_payload_passes_schema(self):
        """coverage: M3K1-A1 / packet Q2 甲案 — data:{} 薄载荷必须过门(front 截获展开)。"""
        ok, reason = _display_schema().validate_display({"view": "schedule", "data": {}})
        self.assertTrue(
            ok, f"M3K1-A1 RED: thin payload data={{}} rejected ({reason}) — 甲案「LLM 发"
                "薄载荷、front 截获展开」要求薄载荷过机械门 (packet Q2).")

    def test_a1_home_with_and_without_data_passes_schema(self):
        """coverage: M3K1-A1 / packet §1 触点② Q3 — home 带 data 与缺 data 均过门。"""
        ds = _display_schema()
        with_data = {"view": "home", "data": {"upcoming": {
            "day_key": "今天",
            "items": [{"time": "14:00", "name": "钢琴课"}],
            "preview": [{"time": "08:00", "name": "上学"}],
        }}}
        ok, reason = ds.validate_display(with_data)
        self.assertTrue(ok, f"M3K1-A1 RED: home+data rejected ({reason}) — §1.3⑥ "
                            "additive 承载必须过机械门 (packet Q3).")
        ok2, reason2 = ds.validate_display({"view": "home"})
        self.assertTrue(ok2, f"M3K1-A1 RED: bare home rejected ({reason2}) — 缺 data "
                             "行为不变 (additive 红线)。")

    def test_a1_legacy_schedule_shape_rejected_q5(self):
        """coverage: M3K1-A1 / packet Q5 — 旧 {title,items} 形状收紧拒绝(降级提示)。"""
        legacy = {"view": "schedule", "data": {
            "title": "今天下午",
            "items": [{"time": "14:00", "name": "钢琴课", "location": "琴行"}]}}
        ok, reason = _display_schema().validate_display(legacy)
        self.assertFalse(
            ok, f"M3K1-A1 RED: legacy schedule shape {legacy!r} still passes the gate "
                "— Q5 收紧(旧形状拒绝并降级提示)未实现 (packet §5).")
        self.assertTrue(isinstance(reason, str) and reason,
                        "M3K1-A1 RED: 拒绝必须带 reason(降级文案依据)，不得裸 False。")
        ok2, _ = _display_schema().validate_display({"view": "schedule", "data": {
            "title": "今天下午", "items": "not-even-a-list"}})
        self.assertFalse(ok2, "M3K1-A1 RED: 旧形状脏体(title/items 键 + 非 list items)"
                              "必须一并拒绝 — 收紧后 data 只放行 {} 或新周网格形状。")

    def test_a1_malformed_week_shape_rejected(self):
        """coverage: M3K1-A1 — 新形状 floor: week 缺 courses 的非新形状必须拒绝。"""
        ok, reason = _display_schema().validate_display(
            {"view": "schedule", "data": {"week": {}}})
        self.assertFalse(
            ok, f"M3K1-A1 RED: {{'week': {{}}}} (courses 缺失) passed the gate ({reason})"
                " — 收紧到新形状 = 非 {}/周网格形状一律拒绝 (packet Q5).")


# --------------------------------------------------------------------------- #
# M3K1-A2 — B1→周网格计算齿
# --------------------------------------------------------------------------- #

class M3K1WeekGridAcceptance(unittest.TestCase):
    """M3K1-A2: 学校课逐节 4+2 / 固定课外课逐门 / 临时安排 ※ / 星标呼应 / day 0 基。"""

    maxDiff = None

    def test_a2_week_grid_frozen_shape_and_day0_mapping(self):
        """coverage: M3K1-A2 / contract §1.3② — 冻结形状逐字段 + day 0 基映射 +
        「今天」列不在载荷 + 备注不进载荷。"""
        payload = _grid(_parse(B1_GRID), MON)
        self.assertEqual(
            payload,
            {"view": "schedule",
             "data": {"week": {"courses": payload["data"]["week"]["courses"]}}},
            "M3K1-A2 RED: payload envelope must be exactly {view:schedule, "
            "data:{week:{courses}}} — 「今天」列不在载荷(设备本地日期推导)，无其它键 "
            "(packet §1/§5)。")
        self.assertEqual(
            set(payload["data"]), {"week"},
            "M3K1-A2 RED: data keys != {week} — 「今天」列/备注等不得进载荷。")
        self.assertEqual(
            set(payload["data"]["week"]), {"courses"},
            "M3K1-A2 RED: week keys != {courses}。")
        for c in payload["data"]["week"]["courses"]:
            self.assertEqual(
                set(c), {"day", "slot", "name", "flags"},
                f"M3K1-A2 RED: course keys {sorted(c)} != {{day,slot,name,flags}} — "
                "备注不进网格载荷 (human 契约门 Q1 裁定)、无今日标记键。")
            self.assertIsInstance(c["day"], int, "M3K1-A2 RED: day 必须是整数 0..6。")
            self.assertIsInstance(c["slot"], int, "M3K1-A2 RED: slot 必须是整数 0..2。")
            self.assertIsInstance(c["flags"], int, "M3K1-A2 RED: flags 必须是整数位掩码。")
        self.assertEqual(
            _courses_multiset(payload), _expected_grid_multiset(),
            f"M3K1-A2 RED: grid multiset wrong:\n{sorted(_courses_multiset(payload))}"
            f"\nexpected:\n{sorted(_expected_grid_multiset())} — 学校课逐节 4+2、"
            "课外课逐门、day 0 基(周一=0/周三=2/周日=6, 即 B1「周一..周日」数组下标序)、"
            "16:30→下午/19:00→晚上/10:00→上午。")
        # day 0 基映射显式钉（防 K1/K2 各自脑补差一偏移 — packet §1 注记）
        days = {c["name"]: c["day"] for c in payload["data"]["week"]["courses"]
                if c["name"] in ("钢琴课", "英语培训", "美术班")}
        self.assertEqual(days, {"钢琴课": 0, "英语培训": 2, "美术班": 6},
                         "M3K1-A2 RED: day 0 基映射错误 — B1 周一=1..周日=7 必须"
                         "映射为载荷 day=weekday-1（周一=0..周日=6）。")

    def test_a2_week_grid_onetime_flag_and_out_of_week_filtered(self):
        """coverage: M3K1-A2 — 当周一次性安排落格带 ※(bit0)，周外(过期/未到)不出现。"""
        payload = _grid(_parse(B1_GRID), MON)
        oral = [c for c in payload["data"]["week"]["courses"] if c["name"] == "口语课"]
        self.assertEqual(
            oral, [{"day": 2, "slot": 2, "name": "口语课", "flags": 1}],
            "M3K1-A2 RED: 当周一次性安排口语课(2026-09-23 18:30)必须落 (day=2, slot=2) "
            "且 flags bit0=1(※) — 「一次性安排按当周日期落格」(S-F1.2 AC3)。")
        names = {c["name"] for c in payload["data"]["week"]["courses"]}
        self.assertNotIn("已过期事项", names,
                         "M3K1-A2 RED: 周外过期一次性安排(09-16)漏进当周网格 — 过期过滤缺失。")
        self.assertNotIn("未来事项", names,
                         "M3K1-A2 RED: 周外未来一次性安排(09-30)漏进当周网格。")

    def test_a2_week_grid_star_marks_current_upcoming(self):
        """coverage: M3K1-A2 / S-F1.2 AC4 — group_upcoming 首项呼应星标(bit1)；
        合并「上学」项不匹配任何课程(不炸、无星标)；无 upcoming 无星标。"""
        # 周一 17:00: 上学(08:00, 至 16:30)已过 -> 首项 = 钢琴课 16:30(进行中) -> 星标。
        payload = _grid(_parse(B1_GRID), MON, now_hhmm="17:00", school_start="08:00")
        self.assertEqual(
            _courses_multiset(payload), _expected_grid_multiset(piano_flags=2),
            "M3K1-A2 RED: 当前「接下来」项(钢琴课)未带 bit1 星标 — S-F1.2 AC4 星标呼应"
            "缺失（flags 位掩码 bit1=2，与基值叠加）。")
        # 周一 12:00: 首项 = 合并「上学」-> 无同名课程 -> 无星标且不炸。
        payload_merged = _grid(_parse(B1_GRID), MON, now_hhmm="12:00", school_start="08:00")
        self.assertEqual(
            _courses_multiset(payload_merged), _expected_grid_multiset(),
            "M3K1-A2 RED: 首项为合并「上学」时星标必须不落在任何课程上(无同名课程)且"
            "网格其余不变。")
        # 23:00: 当日无 upcoming -> 全网格无 bit1。
        payload_none = _grid(_parse(B1_GRID), MON, now_hhmm="23:00", school_start="08:00")
        self.assertEqual(
            _courses_multiset(payload_none), _expected_grid_multiset(),
            "M3K1-A2 RED: 当日无 upcoming 时不得有星标残留。")

    def test_a2_week_grid_flags_compose_bit0_or_bit1(self):
        """coverage: M3K1-A2 — 一次性安排同时是当前「接下来」项 -> flags = bit0|bit1 = 3。"""
        # 周三 15:00, school_start=None: 无合并项 -> 首项 = 口语课 18:30(当周一次性)。
        payload = _grid(_parse(B1_GRID), WED, now_hhmm="15:00", school_start=None)
        self.assertEqual(
            _courses_multiset(payload), _expected_grid_multiset(oral_flags=3),
            "M3K1-A2 RED: 口语课(一次性安排 + 当前接下来项)flags 应为 1|2=3 — 位掩码"
            "两 bit 独立叠加，不得互斥覆盖。")

    def test_a2_week_grid_empty_week_and_dirty_input_boundary(self):
        """coverage: M3K1-A2 边界 — 全空课表 -> 空网格过真门；脏输入大声拒绝不静默。"""
        payload = _grid(_parse(B1_EMPTY), MON)
        self.assertEqual(
            payload["data"]["week"]["courses"], [],
            "M3K1-A2 RED: 全空课表(三区块皆缺/空)必须产空 courses —— 学校课程表 optional。")
        ok, reason = _display_schema().validate_display(payload)
        self.assertTrue(ok, f"M3K1-A2 RED: empty-week payload failed the real gate ({reason}).")
        dirty = B1_GRID.replace("| 周三 | 19:00", "| 周九 | 19:00")
        with self.assertRaises(
                ValueError,
                msg="M3K1-A2 RED: 脏 B1(周九)经网格链路必须 ValueError 大声拒绝，"
                    "绝不静默产网格 (P2-A2 先例)。"):
            _grid(_parse(dirty), MON)


# --------------------------------------------------------------------------- #
# M3K1-A3 — 首页 upcoming payload 齿
# --------------------------------------------------------------------------- #

class M3K1HomeUpcomingAcceptance(unittest.TestCase):
    """M3K1-A3: group_upcoming 零偏移复用 + 决策 26 合并「上学」12:00 回归 + preview。"""

    maxDiff = None

    def test_a3_home_upcoming_payload_frozen_shape(self):
        """coverage: M3K1-A3 / contract §1.3⑥（2026-10-11 多项数组化）— 冻结 home
        data 形状逐字段。preview = 明日全量 {time,name} 数组（周日单项）。"""
        model = _parse(B1_HOME)
        payload = _home(model, MON, "12:00", SUN, school_start="08:00")
        self.assertEqual(
            payload,
            {"view": "home", "data": {"upcoming": {
                "day_key": "今天",
                "items": [
                    {"time": "08:00", "name": "上学"},
                    {"time": "14:00", "name": "牙医复诊"},
                    {"time": "16:30", "name": "钢琴课"},
                ],
                "preview": [{"time": "10:00", "name": "美术班"}],
            }}},
            "M3K1-A3 RED: home payload 形状/内容错误 — items = group_upcoming 零偏移"
            "结果剥 location({time,name} 恰两键, hp1_item_t seam)，day_key=今天，"
            "preview = 明日(周日)全量 {time,name} 数组（2026-10-11 多项化, CCP 触点①）。")
        ok, reason = _display_schema().validate_display(payload)
        self.assertTrue(ok, f"M3K1-A3 RED: producer home payload failed the real gate "
                            f"({reason}) — 三触发下发必须原样过机械门。")

    def test_a3_decision26_merged_school_1200_regression(self):
        """coverage: M3K1-A3 / arch §10 决策 26 — 12:00 在校时段合并「上学」计 upcoming
        （既有 group_upcoming 语义零偏移回归，经 home payload 断言）。"""
        model = _parse(B1_HOME)
        payload = _home(model, MON, "12:00", SUN, school_start="08:00")
        items = payload["data"]["upcoming"]["items"]
        self.assertIn(
            {"time": "08:00", "name": "上学"}, items,
            f"M3K1-A3 RED: 12:00 在校时段合并「上学」项缺失: {items!r} — 决策 26 "
            "「在校时段计 upcoming」语义偏移 (arch §10 决策 26 / 冻结先例)。")
        for it in items:
            self.assertEqual(
                set(it), {"time", "name"},
                f"M3K1-A3 RED: home item keys {sorted(it)} != {{time,name}} — "
                "hp1_item_t seam 形状 (location 不进 home 载荷)。")

    def test_a3_preview_tomorrow_and_empty_cases(self):
        """coverage: M3K1-A3 / HV5-AC7 — 明日无项 -> preview == []（不再 None, plan
        R1：不改则 AC6「明天没有什么课喽」话术永不触发）；非上学日（周二无学校行、
        无课外班与临时事件）= 板端话术态唯一可达 wire 值的场景构造（防 vacuous）。"""
        model = _parse(B1_HOME)
        empty_prev = _home(model, MON, "12:00", TUE, school_start="08:00")
        self.assertIsInstance(
            empty_prev["data"]["upcoming"]["preview"], list,
            "HV5-AC6 RED: preview 必须恒为数组（三态语义: 数组/[]/null 只在前两者 "
            "出现于 producer wire）——单项对象/None 均为旧形状残留。")
        self.assertEqual(
            empty_prev["data"]["upcoming"]["preview"], [],
            "M3K1-A3 RED: 明日(周二)无项时 preview 必须为 []（云端确认明日无项, "
            "AC6 话术的显式 wire 载体）——旧 None 会使板端空态话术永不触发 (plan R1)。")
        self.assertEqual(
            empty_prev["data"]["upcoming"]["day_key"], "今天",
            "M3K1-A3 RED: day_key 恒为「今天」(payload 即 home_p1 seam 注入参数)。")
        # school_start=None: 无合并项，周一 items = 牙医复诊 + 钢琴课（口径零偏移）。
        nos = _home(model, MON, "12:00", SUN, school_start=None)
        self.assertEqual(
            nos["data"]["upcoming"]["items"],
            [{"time": "14:00", "name": "牙医复诊"}, {"time": "16:30", "name": "钢琴课"}],
            "M3K1-A3 RED: school_start=None 时 items 必须与 group_upcoming 同口径"
            "(无合并项) — 单源复用不得另算一套。")

    def test_a3_preview_multi_item_ascending_school_merged(self):
        """coverage: HV5-AC2/HV5-AC6 — 明日全量进区：学校课合并「XX:XX 上学」与
        课外班、临时事件同列，按开始时间升序（requirements v1.61 AC2, 2026-10-11
        human 改判翻转 v1.57「仅课外班与临时事件」OQ2 口径）；上学日明天区必然
        非空（AC6 话术仅非上学日可达的 wire 侧闭环, 防 vacuous）。样本 = B1_GRID
        明日周三：学校周三行（合并「上学」08:00）+ 周三英语培训 19:00 + 周三一次性
        口语课 18:30 → 三项升序。"""
        model = _parse(B1_GRID)
        payload = _home(model, MON, "12:00", WED, school_start="08:00")
        preview = payload["data"]["upcoming"]["preview"]
        self.assertEqual(
            preview,
            [
                {"time": "08:00", "name": "上学"},
                {"time": "18:30", "name": "口语课"},
                {"time": "19:00", "name": "英语培训"},
            ],
            "HV5-AC2 RED: preview 必须为明日全量 {time,name} 数组、按开始时间升序、"
            "学校课合并「上学」项与课外班、临时事件同列 — "
            f"actual={preview!r} (requirements v1.61 AC2 / CCP 触点①)。")
        for it in preview:
            self.assertEqual(
                set(it), {"time", "name"},
                f"HV5-AC2 RED: preview item keys {sorted(it)} != {{time,name}} — "
                "hp1_item_t seam 形状（location 不进 home 载荷, 与 items 同形）。")
        self.assertRegex(
            preview[0]["time"], r"\d{2}:\d{2}",
            "HV5-AC2 RED: preview.time 必须钉 HH:MM 字面（K1 裁决: 自然语言标签不入 "
            "wire——「明早」旧漂移已随 CCP 触点② 收回）。")


# --------------------------------------------------------------------------- #
# M3K1-A4 — 三触发模拟齿
# --------------------------------------------------------------------------- #

class M3K1TriggerSimulationAcceptance(unittest.TestCase):
    """M3K1-A4: 建连 / 每日零点 / 录入即时 marker 三触发模拟——各自到达、互不重复。

    触发调度/接线属实现细节，本类只钉声明模拟面（文件头 FROZEN OFFLINE CONTRACT）
    的可观测决策；真机三时点复证归 M3-K3（M3K1-A5 deferred）。"""

    maxDiff = None

    def test_a4_connect_push_payload_single_source(self):
        """coverage: M3K1-A4 触发① — 建连推送内容 == home_upcoming_payload(单源)。"""
        home_fn, err = _schedule_callable("home_upcoming_payload")
        if home_fn is None:
            self.fail("M3K1-A3 RED: " + err)
        connect, err2 = _push_callable("connect_push_payload")
        if connect is None:
            self.fail("M3K1-A4 RED: " + err2)
        model = _parse(B1_HOME)
        args = (model, MON, "12:00", SUN, "08:00")
        payload = connect(*args)
        self.assertEqual(
            payload, home_fn(*args),
            "M3K1-A4 RED: 建连推送内容与 home_upcoming_payload 不一致 — 三触发必须"
            "单源同算，不得每触发各产一套 (单源防漂移)。")
        self.assertEqual(payload.get("view"), "home",
                         "M3K1-A4 RED: 建连推送必须是 view=home 载荷。")

    def test_a4_daily_push_due_local_midnight_b1_timezone(self):
        """coverage: M3K1-A4 触发② — 本地(Asia/Shanghai)零点恰一次；B1 frontmatter
        timezone 驱动判定；空 timezone 默认 Asia/Shanghai。"""
        due, err = _push_callable("daily_push_due")
        if due is None:
            self.fail("M3K1-A4 RED: " + err)
        # 16:05Z = 上海 09-22 00:05（本地零点已过）-> 新本地日 -> due。
        self.assertTrue(
            due("Asia/Shanghai", "2026-09-21", UTC_LOCAL_CROSSED),
            "M3K1-A4 RED: 本地零点后第一 tick 未判 due — 每日零点触发缺失 "
            "(timezone=Asia/Shanghai = UTC+8, house 先例 _CN_TZ)。")
        # 同一时刻，若当天已推过(本地日 09-22) -> 恰一次语义 -> 不重复 due。
        self.assertFalse(
            due("Asia/Shanghai", "2026-09-22", UTC_LOCAL_CROSSED),
            "M3K1-A4 RED: 同一本地日重复判 due — 恰一次语义缺失（此例同时区分"
            "「按 UTC 日误算」的实现：UTC 日 09-21 != 09-22 会假 due）。")
        # 15:55Z = 上海 09-21 23:55（本地零点未到）-> 同一本地日 -> 不 due。
        self.assertFalse(
            due("Asia/Shanghai", "2026-09-21", UTC_LOCAL_BEFORE_MIDNIGHT),
            "M3K1-A4 RED: 本地零点前误判 due — 零点边界按 B1 时区而非 UTC。")
        # 尚未推过 -> due（首连/首日兜底）。
        self.assertTrue(
            due("Asia/Shanghai", None, UTC_LOCAL_BEFORE_MIDNIGHT),
            "M3K1-A4 RED: last_push=None 必须判 due（尚未推过）。")
        # 空 timezone -> 默认 Asia/Shanghai（B1 唯一在役时区的缺省兜底）。
        self.assertEqual(
            due("", "2026-09-21", UTC_LOCAL_CROSSED),
            due("Asia/Shanghai", "2026-09-21", UTC_LOCAL_CROSSED),
            "M3K1-A4 RED: 空 timezone 行为必须 == Asia/Shanghai 缺省（house 先例）。")

    def test_a4_refresh_marker_main_channel_only_and_exactly_once(self):
        """coverage: M3K1-A4 触发③ / S-F1.5 AC1+AC2 — 主通道 marker 即时请求恰一次；
        副通道直改 MD 无 marker 不产生即时请求；两轮模拟轮询恰一次推送。"""
        requested, err = _push_callable("refresh_requested")
        if requested is None:
            self.fail("M3K1-A4 RED: " + err)
        consume, err2 = _push_callable("consume_refresh_marker")
        if consume is None:
            self.fail("M3K1-A4 RED: " + err2)
        connect, err3 = _push_callable("connect_push_payload")
        if connect is None:
            self.fail("M3K1-A4 RED: " + err3)
        mod, err4 = _push_module()
        if mod is None:
            self.fail("M3K1-A4 RED: " + err4)
        self.assertEqual(
            getattr(mod, "MARKER_FILENAME", None), MARKER_NAME,
            f"M3K1-A4 RED: MARKER_FILENAME != {MARKER_NAME!r} — 跨组件协议常量"
            "(OpenClaw 主通道写 / front 读)必须钉住。")

        with tempfile.TemporaryDirectory(prefix="m3k1_a4_") as tmp:
            alerts_dir = Path(tmp)
            workspace = Path(tmp) / "workspace"   # 副通道直改的 schedule.md 所在面
            workspace.mkdir()
            model = _parse(B1_HOME)

            # 空交接目录：无请求。
            self.assertFalse(
                requested(alerts_dir),
                "M3K1-A4 RED: 空交接目录误报即时请求。")
            # 副通道：直改 schedule.md（不落 marker）-> 无即时请求（S-F1.5 AC2）。
            (workspace / "schedule.md").write_text(B1_HOME + "\n", encoding="utf-8")
            self.assertFalse(
                requested(alerts_dir),
                "M3K1-A4 RED: 副通道直改 MD(无 marker)触发了即时请求 — 主/副通道"
                "判别被抹平（S-F1.5 AC2: 副通道无即时推送，靠①②兜底）。")
            # 主通道：OpenClaw 改写后落 marker -> 请求 pending。
            (alerts_dir / MARKER_NAME).write_text("", encoding="utf-8")
            self.assertTrue(
                requested(alerts_dir), "M3K1-A4 RED: marker 在场未报即时请求。")
            # 模拟 front 轮询 piggyback 两轮：第一轮消费 -> 恰一次推送；第二轮无。
            leg: list = []   # 模拟设备腿：记录到达的推送载荷
            if consume(alerts_dir):
                leg.append(connect(model, MON, "12:00", SUN, "08:00"))
            if consume(alerts_dir):
                leg.append(connect(model, MON, "12:00", SUN, "08:00"))
            self.assertEqual(
                len(leg), 1,
                f"M3K1-A4 RED: 一个 marker 模拟两轮轮询产出 {len(leg)} 次推送 — "
                "「到达且互不重复」被破坏（消费必须恰一次）。")
            self.assertFalse(
                requested(alerts_dir),
                "M3K1-A4 RED: 消费后请求仍 pending — marker 未移除。")
            self.assertFalse(
                (alerts_dir / MARKER_NAME).exists(),
                "M3K1-A4 RED: marker 文件未被移除 — 下一轮会重复推送。")
            self.assertEqual(
                [p.get("view") for p in leg], ["home"],
                "M3K1-A4 RED: 即时推送到达载荷必须是 view=home upcoming 载荷。")


# --------------------------------------------------------------------------- #
# M3K1-E1 — 既有 schedule.py 冻结语义零偏移守卫（must-stay-green）
# --------------------------------------------------------------------------- #

class M3K1ZeroOffsetGuard(unittest.TestCase):
    """coverage: M3K1-E1 — 既有 parse_schedule_md / items_for_date / group_upcoming
    冻结语义回归守卫（builder 增量扩展不得偏移既有行为；先例 = 已归档 p2 冻结件）。"""

    maxDiff = None

    def test_e1_existing_schedule_semantics_zero_offset(self):
        parse, err = _schedule_callable("parse_schedule_md")
        if parse is None:
            self.fail("M3K1-E1 RED: " + err)
        for_date, err2 = _schedule_callable("items_for_date")
        if for_date is None:
            self.fail("M3K1-E1 RED: " + err2)
        group, err3 = _schedule_callable("group_upcoming")
        if group is None:
            self.fail("M3K1-E1 RED: " + err3)
        model = parse(B1_GRID)
        self.assertEqual(model["child"], "小明", "M3K1-E1 RED: frontmatter child 偏移。")
        self.assertEqual(model["timezone"], "Asia/Shanghai",
                         "M3K1-E1 RED: frontmatter timezone 偏移。")
        self.assertEqual(
            model["weekly"][0],
            {"weekday": 1, "start": "16:30", "end": "17:30", "name": "钢琴课",
             "location": "琴行", "note": "带琴谱"},
            "M3K1-E1 RED: weekly 行逐字段保真偏移（weekday 1 基 + note 保留）。")
        # 周三当日列表: weekly 英语培训 19:00 + 当周一次性口语课 18:30，按 time 升序。
        self.assertEqual(
            for_date(model, WED),
            [{"time": "18:30", "name": "口语课", "location": "线上"},
             {"time": "19:00", "name": "英语培训", "location": "新东方"}],
            "M3K1-E1 RED: items_for_date 周三列表偏移（逐项 {time,name,location}、"
            "time 升序稳定）。")
        # 周六(09-26, day5)无课 -> 空表。
        self.assertEqual(for_date(model, "2026-09-26"), [],
                         "M3K1-E1 RED: 无课日应空表。")
        # 决策 26: 周一 17:00 进行中钢琴课保留、已结束合并上学项剔除。
        self.assertEqual(
            group(model, MON, "17:00", school_start="08:00"),
            [{"time": "16:30", "name": "钢琴课", "location": "琴行"}],
            "M3K1-E1 RED: group_upcoming upcoming 规则偏移（进行中保留/已结束剔除）。")
        # 周日永不合并上学项。
        self.assertEqual(
            [g["name"] for g in group(model, SUN, "09:00", school_start="08:00")],
            ["美术班"],
            "M3K1-E1 RED: 周末不得合并「上学」项（周中白天 scope）。")


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    for cls in (M3K1ContractGateAcceptance, M3K1WeekGridAcceptance,
                M3K1HomeUpcomingAcceptance, M3K1TriggerSimulationAcceptance,
                M3K1ZeroOffsetGuard):
        suite.addTests(loader.loadTestsFromTestCase(cls))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)
