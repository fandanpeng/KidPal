"""KidPal B1 课表 MD 纯解析 / 筛选 / 决策 26 producer 分组模块 (TaskPackage
260829-schedule-p2, 方向 B 课表消费链路).

语义权威 = 冻结验收 server/acceptance/test_schedule_p2.acceptance.py 文件头
(frozen offline contract: parse_schedule_md / items_for_date / items_after /
schedule_view_payload / group_upcoming)，本模块不得偏移；行为契约另有：

- 契约 §二 B1 课表 MD 结构（表体零变更）：每周固定课表 + 一次性安排两类必带,
  学校课程表为可选区块（缺块 -> 空表）。
- 契约 §1.3 ② schedule view data 形状（2026-09-21 契约门重定义，随 M3-K1 同 PR，
  packet = delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/）：
  完整网格 {week:{courses:[{day,slot,name,flags}]}}——day 0 基（周一=0）、slot
  0 上午 / 1 下午 / 2 晚上、flags bit0=※ 临时安排 bit1=星标呼应；意图薄载荷
  data:{}（front 截获展开，不直达设备）；旧 {title, items} 列表形即废（front
  机械校验拒绝并降级）。备注不进网格载荷；旧「不进反白/星标等 UI 语义态」
  钉行由 flags 位承载取代（契约 §1.3② 变更记录行）。
- P2-E4：frontmatter ``remind_before_min`` 已废止（契约 §2.3, 2026-09-03 契约门
  裁定）——解析忽略、永不进入模型；提醒触发逻辑不在本模块（归包 7）。
- P2-A2：脏行大声拒绝（ValueError），绝不静默丢行。

纯 python、import 时不依赖 fastapi/gateway —— offline 任何位置可导入（同
gateway/display_schema.py 纯模块先例）；载荷可原样过 gateway.display_schema
.validate_display 机械门（frozen 验收在用）。

决策 26 producer 分组口径（group_upcoming）的 upcoming 规则实现注记：冻结验收
对合并「上学」项的 12:00 断言要求其在校时段计为 upcoming——本实现把合并项视为
「自 school_start 起至当日下一事项开始前处于进行中」（end := 当日下一事项的
start，无下一事项时回退 start-only 规则 start >= now），与「进行中保留、
已结束剔除、start-only 按 start 判」的全部冻结断言一致。
"""

from __future__ import annotations

import re
from datetime import datetime, timedelta

# 契约 §2.3：星期取 周一~周日（周一=1..周日=7；「周天」同义接受）。
_WEEKDAY_TOKENS = {
    "周一": 1,
    "周二": 2,
    "周三": 3,
    "周四": 4,
    "周五": 5,
    "周六": 6,
    "周日": 7,
    "周天": 7,
}

_TIME_RE = re.compile(r"^([01]\d|2[0-3]):[0-5]\d$")
_DATE_RE = re.compile(r"^\d{4}-\d{2}-\d{2}$")

# B1 区块标题文本（按 heading 行文本定位；学校课程表为可选区块）。
_BLOCK_WEEKLY = "每周固定课表"
_BLOCK_ONETIME = "一次性安排"
_BLOCK_SCHOOL = "学校课程表"

_SEP_ROW_RE = re.compile(r"^:?-{3,}:?$")


# --------------------------------------------------------------------------- #
# 校验小工具（脏值大声拒绝，绝不静默吞）
# --------------------------------------------------------------------------- #


def _validate_hhmm(token: str) -> str:
    if not isinstance(token, str) or not _TIME_RE.match(token.strip()):
        raise ValueError(f"invalid HH:MM time token: {token!r} (契约 §2.3 时间格式)")
    return token.strip()


def _parse_time_cell(cell: str) -> tuple[str, str | None]:
    """时间格 "HH:MM-HH:MM" 或 "HH:MM"（end=None）。非法 -> ValueError。"""
    text = cell.strip()
    if "-" in text:
        start, _, end = text.partition("-")
        return _validate_hhmm(start), _validate_hhmm(end)
    return _validate_hhmm(text), None


def _parse_weekday_token(token: str) -> int:
    text = token.strip()
    if text not in _WEEKDAY_TOKENS:
        raise ValueError(
            f"invalid weekday token: {token!r} (契约 §2.3 星期取 周一~周日)")
    return _WEEKDAY_TOKENS[text]


def _parse_date_token(token: str) -> str:
    text = token.strip()
    if not _DATE_RE.match(text):
        raise ValueError(
            f"invalid date token: {token!r} (契约 §2.3 日期格式 YYYY-MM-DD)")
    datetime.strptime(text, "%Y-%m-%d")  # 日历合法性（如 2026-02-30 -> ValueError）
    return text


def _weekday_of_date(date_iso: str) -> int:
    """date_iso (YYYY-MM-DD) -> 星期几（周一=1..周日=7）。非法 -> ValueError。"""
    text = _parse_date_token(date_iso)
    # Monday=0..Sunday=6 (date.weekday()) -> 契约口径 +1
    return datetime.strptime(text, "%Y-%m-%d").date().weekday() + 1


def _split_row(line: str) -> list[str]:
    """把一行 markdown 表格行切成去空白 cell 列表（首尾竖线容忍缺失）。"""
    text = line.strip()
    if text.startswith("|"):
        text = text[1:]
    if text.endswith("|"):
        text = text[:-1]
    return [c.strip() for c in text.split("|")]


# --------------------------------------------------------------------------- #
# 区块定位与表格行抽取
# --------------------------------------------------------------------------- #


def _block_data_rows(md_text: str, block_name: str) -> list[list[str]]:
    """定位含 block_name 的 heading，返回其下表格的数据行（cell 列表）。

    区块缺失 -> []（学校课程表 optional per B1）。表机制：header 行 -> 破折号
    分隔行（跳过）-> 数据行；数据行 cell 数 != header cell 数 -> ValueError
    （脏行大声拒绝，契约 P2-A2）。
    """
    lines = md_text.splitlines()
    start = None
    for i, line in enumerate(lines):
        stripped = line.strip()
        if stripped.startswith("#") and block_name in stripped:
            start = i + 1
            break
    if start is None:
        return []

    # header 行
    header_idx = None
    for i in range(start, len(lines)):
        if lines[i].strip().startswith("|"):
            header_idx = i
            break
        if lines[i].strip().startswith("#"):
            return []  # 区块下无表格：视为空表
    if header_idx is None:
        return []
    header = _split_row(lines[header_idx])

    # 破折号分隔行（紧随 header；缺失 = 畸形表，大声拒绝）
    if header_idx + 1 >= len(lines):
        raise ValueError(
            f"B1 block {block_name!r}: table separator row missing (脏表大声拒绝)")
    sep_cells = _split_row(lines[header_idx + 1])
    if not sep_cells or not all(_SEP_ROW_RE.match(c) for c in sep_cells):
        raise ValueError(
            f"B1 block {block_name!r}: table separator row missing (脏表大声拒绝)")

    rows: list[list[str]] = []
    for line in lines[header_idx + 2:]:
        stripped = line.strip()
        if not stripped.startswith("|"):
            break  # 表格结束（空行/下一 heading/散文）
        cells = _split_row(line)
        if len(cells) != len(header):
            raise ValueError(
                f"B1 block {block_name!r}: row cell count {len(cells)} != header "
                f"cell count {len(header)}: {line!r} (脏行拒绝, 不静默丢)")
        rows.append(cells)
    return rows


# --------------------------------------------------------------------------- #
# 三类区块行解析（逐字段保真）
# --------------------------------------------------------------------------- #


def _parse_weekly_row(cells: list[str]) -> dict:
    # 列式：星期 | 时间 | 课程 | 地点 | 备注
    start, end = _parse_time_cell(cells[1])
    return {
        "weekday": _parse_weekday_token(cells[0]),
        "start": start,
        "end": end,
        "name": cells[2],
        "location": cells[3],
        "note": cells[4],
    }


def _parse_onetime_row(cells: list[str]) -> dict:
    # 列式：日期 | 时间 | 事项 | 地点 | 备注
    start, end = _parse_time_cell(cells[1])
    return {
        "date": _parse_date_token(cells[0]),
        "start": start,
        "end": end,
        "name": cells[2],
        "location": cells[3],
        "note": cells[4],
    }


def _parse_school_row(cells: list[str]) -> dict:
    # 列式：星期 | 第1节 | 第2节 | ... | 第N节（无时刻列，契约 B1 表体零变更;
    # 首节时刻由 producer 显式入参 school_start 提供，见 group_upcoming）
    return {
        "weekday": _parse_weekday_token(cells[0]),
        "periods": list(cells[1:]),
    }


def _parse_frontmatter(md_text: str) -> tuple[str, str]:
    """frontmatter 只取 child/timezone；其余键（含已废止的 remind_before_min）
    一律忽略、永不进入模型 (P2-E4)。无 frontmatter -> ("", "")。"""
    lines = md_text.splitlines()
    if not lines or lines[0].strip() != "---":
        return "", ""
    child = ""
    timezone = ""
    for line in lines[1:]:
        if line.strip() == "---" or line.strip() == "...":
            break
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        key, _, value = stripped.partition(":")
        key = key.strip()
        value = value.split("#", 1)[0].strip().strip('"').strip("'")
        if key == "child":
            child = value
        elif key == "timezone":
            timezone = value
        # 其余 frontmatter 键（remind_before_min 等）显式忽略——废止不消费
    return child, timezone


# --------------------------------------------------------------------------- #
# FROZEN OFFLINE CONTRACT
# --------------------------------------------------------------------------- #


def parse_schedule_md(md_text: str) -> dict:
    """解析契约 §二 B1 课表 MD，返回 {child, timezone, weekly, onetime, school}。"""
    if not isinstance(md_text, str):
        raise ValueError("schedule md must be a string")
    child, timezone = _parse_frontmatter(md_text)
    return {
        "child": child,
        "timezone": timezone,
        "weekly": [_parse_weekly_row(c) for c in _block_data_rows(md_text, _BLOCK_WEEKLY)],
        "onetime": [_parse_onetime_row(c) for c in _block_data_rows(md_text, _BLOCK_ONETIME)],
        "school": [_parse_school_row(c) for c in _block_data_rows(md_text, _BLOCK_SCHOOL)],
    }


def items_for_date(model: dict, date_iso: str) -> list[dict]:
    """某日历日全部事项，逐项恰为契约 §1.3 ② item 形状 {time,name,location}
    （time = 开始时刻），按 time 升序稳定排序（同刻每周固定在一次性安排前）。

    每周固定行按 weekday(date_iso) 匹配；一次性安排行按 date 精确匹配 date_iso
    —— 过期一次性安排因此永不出现（契约 §2.3 过期自动忽略）。"""
    wd = _weekday_of_date(date_iso)
    out: list[dict] = []
    for row in model.get("weekly", []):
        if row["weekday"] == wd:
            out.append({"time": row["start"], "name": row["name"],
                        "location": row["location"]})
    for row in model.get("onetime", []):
        if row["date"] == date_iso:
            out.append({"time": row["start"], "name": row["name"],
                        "location": row["location"]})
    out.sort(key=lambda it: it["time"])  # 稳定排序：块序保持
    return out


def items_after(model: dict, date_iso: str, now_hhmm: str) -> list[dict]:
    """items_for_date 过滤 start >= now_hhmm（「今天下午有什么课」当前时间筛选）。"""
    _validate_hhmm(now_hhmm)
    return [it for it in items_for_date(model, date_iso) if it["time"] >= now_hhmm]


def schedule_view_payload(title: str, items: list[dict]) -> dict:
    """契约 §1.3 ② producer 载荷 {view:schedule, data:{title,items}}。

    items 必须已是精确 item 形状；不发明额外键（零增减——不新增 view/字段/
    反白态等 UI 语义，契约零变更）。整包可原样过 gateway.display_schema 门。"""
    return {"view": "schedule", "data": {"title": title, "items": list(items)}}


def group_upcoming(model: dict, date_iso: str, now_hhmm: str,
                   school_start: str | None = None) -> list[dict]:
    """决策 26 producer：P1「接下来」分组计算（spec S-F1.1 / PROPOSAL §2 包 6）。

    - 周中（Mon..Fri）且学校课程表有该星期行且 school_start 给定 -> 合并一项
      {"time": school_start, "name": "上学", "location": ""}（B1 学校课程表无
      时刻列，首节时刻 = producer 配置显式入参 school_start——B1 表体零变更，
      载体 server/deploy/openclaw-schedule.config.yaml）。周六/周日永不合并
      （周中白天 scope）。
    - 培训班（每周固定）+ 一次性安排逐项列出，同 item 形状。
    - upcoming 规则：有 end 的项 end > now 保留（进行中=upcoming）；start-only
      项 start >= now 保留；已结束剔除。合并「上学」项在校时段（至当日下一事项
      开始前）计为进行中（见模块 docstring 实现注记）。
    - 输出按 time 升序（合并项参与排序），逐项恰为契约 item 形状。
    """
    _validate_hhmm(now_hhmm)
    if school_start is not None:
        _validate_hhmm(school_start)
    wd = _weekday_of_date(date_iso)

    # 候选项（携带 end 供 upcoming 判定；合并上学项 end 见实现注记）
    candidates: list[dict] = []
    if wd <= 5 and school_start is not None:
        if any(r["weekday"] == wd for r in model.get("school", [])):
            candidates.append({"time": school_start, "name": "上学",
                               "location": "", "end": None, "merged": True})
    for row in model.get("weekly", []):
        if row["weekday"] == wd:
            candidates.append({"time": row["start"], "name": row["name"],
                               "location": row["location"], "end": row["end"],
                               "merged": False})
    for row in model.get("onetime", []):
        if row["date"] == date_iso:
            candidates.append({"time": row["start"], "name": row["name"],
                               "location": row["location"], "end": row["end"],
                               "merged": False})

    # 合并「上学」项：进行中窗口至当日下一事项开始（无下一事项 -> start-only 规则）
    for it in candidates:
        if it["merged"] and it["end"] is None:
            next_starts = [o["time"] for o in candidates
                           if not o["merged"] and o["time"] > it["time"]]
            if next_starts:
                it["end"] = min(next_starts)

    kept = []
    for it in candidates:
        if it["end"] is not None:
            if it["end"] > now_hhmm:  # 进行中保留（end > now），已结束剔除
                kept.append(it)
        elif it["time"] >= now_hhmm:  # start-only 按 start 判
            kept.append(it)

    kept.sort(key=lambda it: it["time"])  # 合并项参与排序
    return [{"time": it["time"], "name": it["name"], "location": it["location"]}
            for it in kept]


# --------------------------------------------------------------------------- #
# 周网格 / home upcoming producer（契约门重定义，M3-K1）
# --------------------------------------------------------------------------- #
# 分界常量（契约 §1.3② 未钉明确时刻分界，accepted 显式缺口③——builder 实现处留痕）：
#   上午 slot0 = 00:00 起、下午 slot1 = 12:00 起、晚上 slot2 = 18:00 起（含）。
# 只影响无歧义样本之外的分界时刻（10:00→0、16:30→1、18:30/19:00→2 均稳）。
_PM_START = "12:00"       # slot 1 下午起点（含）
_EVENING_START = "18:00"  # slot 2 晚上起点（含）
# 学校课程表第 1..4 节 -> 上午(slot0)、第 5..6 节 -> 下午(slot1)（spec S-F1.2 AC1 4+2）。
_SCHOOL_AM_PERIODS = 4


def _slot_of(hhmm: str) -> int:
    """开始时刻 -> slot（0 上午 / 1 下午 / 2 晚上）。契约未钉分界、取无歧义留痕常量。"""
    if hhmm >= _EVENING_START:
        return 2
    if hhmm >= _PM_START:
        return 1
    return 0


def _week_bounds(week_date_iso: str) -> tuple[str, str]:
    """week_date_iso（周内任意日历日）所在周一..周日周，返回 (周一_iso, 周日_iso)。"""
    d = datetime.strptime(_parse_date_token(week_date_iso), "%Y-%m-%d").date()
    monday = d - timedelta(days=d.weekday())
    return monday.isoformat(), (monday + timedelta(days=6)).isoformat()


def week_grid_payload(model: dict, week_date_iso: str,
                      now_hhmm: str | None = None,
                      school_start: str | None = None) -> dict:
    """契约 §1.3② 周网格 producer 载荷 {view:schedule, data:{week:{courses}}}。

    - 学校课（「学校课程表」可选区块）逐节：第 1..4 节 -> slot0 上午、第 5..6 节 ->
      slot1 下午，逐节各一条，flags 基 0；空白节跳过。
    - 固定课外课（「每周固定课表」）逐门：day = row weekday-1（周一=0 基），slot 按
      开始时刻落上午/下午/晚上，flags 基 0。
    - 临时安排（「一次性安排」）逐门：当周日期落格（同日映射），flags |= 1(※)；
      周外日期（过期/未到）不出现。
    - 星标（now_hhmm 给定时）：group_upcoming(model, week_date_iso, now_hhmm,
      school_start) 首项按 name + day(week_date_iso 星期-1) + slot(其开始时刻) 匹配
      网格条目 -> 该条目 flags |= 2；合并「上学」项无同名课程 -> 不匹配不炸无星标；
      无首项（当日无 upcoming）-> 全网格无 bit1。

    输出 {view, data.week.courses} 键集精确；courses 条目键集 {day,slot,name,flags}；
    「今天」列不在载荷（设备本地日期推导）；备注不进载荷。"""
    if now_hhmm is not None:
        _validate_hhmm(now_hhmm)
    start_iso, end_iso = _week_bounds(week_date_iso)

    courses: list[dict] = []
    # 学校课逐节（optional 区块；空白节跳过不占格）
    for row in model.get("school", []):
        for idx, name in enumerate(row["periods"]):
            if not name or not name.strip():
                continue
            slot = 0 if idx < _SCHOOL_AM_PERIODS else 1
            courses.append({"day": row["weekday"] - 1, "slot": slot,
                            "name": name.strip(), "flags": 0})
    # 固定课外课逐门（按开始时刻落槽）
    for row in model.get("weekly", []):
        courses.append({"day": row["weekday"] - 1, "slot": _slot_of(row["start"]),
                        "name": row["name"], "flags": 0})
    # 临时安排逐门：仅当周日期落格且带 ※(bit0)；周外过期/未到过滤
    for row in model.get("onetime", []):
        if start_iso <= row["date"] <= end_iso:
            courses.append({"day": _weekday_of_date(row["date"]) - 1,
                            "slot": _slot_of(row["start"]),
                            "name": row["name"], "flags": 1})

    # 星标呼应（bit1）：当前「接下来」首项匹配网格条目则叠加
    if now_hhmm is not None:
        upcoming = group_upcoming(model, week_date_iso, now_hhmm, school_start)
        if upcoming:
            first = upcoming[0]
            day = _weekday_of_date(week_date_iso) - 1
            slot = _slot_of(first["time"])
            for c in courses:
                if (c["name"] == first["name"] and c["day"] == day
                        and c["slot"] == slot):
                    c["flags"] |= 2
                    break  # 一格多门逐门列出时同名同格只星标其一（当前项）

    return {"view": "schedule", "data": {"week": {"courses": courses}}}


def home_upcoming_payload(model: dict, today_iso: str, now_hhmm: str,
                          tomorrow_iso: str,
                          school_start: str | None = None) -> dict:
    """契约 §1.3⑥ home 可选 data.upcoming producer 载荷。

    items = group_upcoming(model, today_iso, now_hhmm, school_start) 逐项剥 location
    后的 {time,name}（分组口径零偏移复用，含决策 26 合并「上学」项 12:00 在校时段计
    upcoming 语义）；preview = group_upcoming(model, tomorrow_iso, "00:00",
    school_start) 首项的 {time,name}（明日预告首项开始时刻）；明日无项 -> preview None。
    day_key 恒为「今天」。payload 即 home_p1 seam 注入参数的 JSON 化。"""
    items = [{"time": it["time"], "name": it["name"]}
             for it in group_upcoming(model, today_iso, now_hhmm, school_start)]
    nxt = group_upcoming(model, tomorrow_iso, "00:00", school_start)
    preview = ({"time": nxt[0]["time"], "name": nxt[0]["name"]} if nxt else None)
    return {"view": "home", "data": {"upcoming": {
        "day_key": "今天", "items": items, "preview": preview}}}


def expand_schedule_intent_payload(payload, model: dict, week_date_iso: str,
                                   now_hhmm: str | None = None,
                                   school_start: str | None = None) -> "dict | None":
    """「帮我显示课表」意图薄载荷 -> front 截获展开的决策面（契约 §1.3② 甲案，
    TaskPackage 260921-m3-k3-voice-schedule-entry-live M3K3-A2 冻结契约）。

    - payload 恰为意图薄载荷：dict 且 view == "schedule" 且 data == {}（空 dict）
      -> 返回 week_grid_payload(model, week_date_iso, now_hhmm, school_start)
      （**单源同算**——直接复用 K1 周网格 producer，绝不另起一套网格计算）。
    - 其余一切（非 dict / view 非 schedule / data 缺失 / data 非空 dict / data
      非 dict）-> 返回 None = 不截获，front 原样走既有 relay 链（薄载荷之外形状由
      display_schema.validate_display 机械门裁决：新周网格放行、旧 {title,items}
      拒绝降级——Q5，K1 已冻结，本件不重复）。
    - 脏 B1（model 解析自坏 MD）-> 沿 week_grid_payload 的 ValueError 大声传播，
      绝不静默产空网格（P2-A2 先例）。
    """
    if not isinstance(payload, dict):
        return None
    if payload.get("view") != "schedule":
        return None
    data = payload.get("data")
    if not isinstance(data, dict) or data:
        return None
    return week_grid_payload(model, week_date_iso, now_hhmm, school_start)
