#!/usr/bin/env python3
"""KidPal 包 7（260829-schedule-reminder）生产侧确定性排程脚本 — 提醒三档阶梯算术 +
幂等 id + alerts/pending.json 原子写。

语义权威 = 冻结验收 server/acceptance/test_schedule_reminder.acceptance.py 文件头
（FROZEN OFFLINE CONTRACT module 1），不得偏移。裁定一（eventId
acaef302-9ddc-4508-b5d3-94023e050f43 = 生产侧**混合分层**）把本文件限定为「确定性
python」那一半：三档时刻算术（时间串解析、跨天映射、事前 N 分钟减法、已过档跳过）+
幂等 id 确定性散列 + pending.json 原子写 + 机械 schema 校验 + 坏输入降级。语义判断
（备注里哪部分算要带的）与给孩子的文案由 OpenClaw agent 回合按 system prompt v3 产出，
写成本文件消费的**语义中间产物**（load_semantic），本文件不预测、不假装能离线验大模型
输出。

纯 stdlib、导入期零网络/零文件 I/O（同目录先例 warm_char_cache.py）；OpenClaw cron
以 --command 载荷执行本文件（见 DEPLOY-RUNBOOK 的提醒段）。课表 B1 解析**复用包 6**
server/gateway/schedule.py 的 parse_schedule_md（RMD-E3：消费包 6 数据链路，不重复
造数据），本文件不自建第二套 B1 解析器。

三档时刻 = 纯服务端配置（DEFAULT_LADDER_CONFIG + server/deploy/openclaw-reminder.
config.yaml），不进契约 B1、永不烧板子（设备侧零引用由 C 孪生档 RMD-A7 齿负向钉死）。
frontmatter 的 remind_before_min 已废止（契约 §2.3），本文件永不读它——提前量只从
config 的 before_min 来。
"""

from __future__ import annotations

import hashlib
import json
import os
import re
import sys
import tempfile
from datetime import date, datetime, timedelta, timezone

# --------------------------------------------------------------------------- #
# FROZEN OFFLINE CONTRACT — 档位常量与配置默认值
# --------------------------------------------------------------------------- #

LADDER_NIGHT = "night"        # 前一天晚上：提醒次日要带的东西，提前准备
LADDER_MORNING = "morning"    # 当天早上：今天什么时间干什么
LADDER_SOON = "soon"          # 事项开始前 N 分钟：临期提醒

DEFAULT_LADDER_CONFIG = {"night_at": "20:30", "morning_at": "07:45", "before_min": 15}

# 提醒条目 id 上界 = 设备侧 kidpal_alert_t.id[40]（跨档一致性，网关按同一上界拒超额）
ID_MAX_LEN = 40
_ID_CHARSET_RE = re.compile(r"[0-9A-Za-z._-]{1,40}")
_HHMM_RE = re.compile(r"^([01]\d|2[0-3]):[0-5]\d$")

# 本包生效时区固定 +08:00（契约 §三 fire_at = ISO8601+08:00；课表 frontmatter timezone
# 是 OpenClaw 侧展示信息，不参与档位算术）。
_TZ_OFFSET = "+08:00"
_TZ8 = timezone(timedelta(hours=8))


# --------------------------------------------------------------------------- #
# 包 6 解析器复用面（RMD-E3）
# --------------------------------------------------------------------------- #

def _schedule_module():
    """取包 6 的 B1 解析模块（单一事实源 server/gateway/schedule.py）。

    部署实态：本脚本随 openclaw-patch 上 ECS 的 OpenClaw workspace，而 gateway 在
    /opt/kidpal/front（kidpal-front.service 的 WorkingDirectory），故路径经
    KIDPAL_SERVER_DIR 可配、仓内相对路径兜底；两条都不可用时由调用方降级报错（不
    自建第二套 B1 解析 = RMD-E3 红线）。
    """
    try:
        from gateway import schedule as sc  # 已随 server/gateway 上 ECS 的场合
        return sc
    except ImportError:
        pass
    import importlib.util  # noqa: PLC0415

    candidates = []
    env_dir = os.environ.get("KIDPAL_SERVER_DIR", "").strip()
    if env_dir:
        candidates.append(env_dir)
    here = os.path.dirname(os.path.abspath(__file__))
    candidates.append(os.path.abspath(os.path.join(here, "..", "..")))  # repo: server/
    for base in candidates:
        path = os.path.join(base, "gateway", "schedule.py")
        if os.path.isfile(path):
            spec = importlib.util.spec_from_file_location("kidpal_gateway_schedule", path)
            if spec is not None and spec.loader is not None:
                mod = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(mod)
                return mod
    raise RuntimeError(
        "package-6 schedule parser not reachable (set KIDPAL_SERVER_DIR) — 提醒排程必须"
        "复用包 6 的 B1 解析器，不在此另起一套 (RMD-E3)")


def parse_schedule_md(md_text: str) -> dict:
    """契约 §二 B1 课表 MD 解析——转调包 6 实现（本文件零第二套解析器）。"""
    return _schedule_module().parse_schedule_md(md_text)


# --------------------------------------------------------------------------- #
# 配置载体（三档时刻的家，纯服务端配置）
# --------------------------------------------------------------------------- #


def load_ladder_config(path) -> dict:
    """读部署提醒配置载体（YAML-lite `key: value` 行）。

    '#' 注释与未知键忽略（配置面失控红线：未知键不得进生效配置）；缺文件/坏文件 ->
    DEFAULT_LADDER_CONFIG 的副本（排程不得因缺配置而停摆）；单项缺失或值不合法 -> 该
    项回默认，不瞎猜其它值。归一：night_at/morning_at -> "HH:MM" str、before_min -> int。
    """
    cfg = dict(DEFAULT_LADDER_CONFIG)
    try:
        with open(str(path), "r", encoding="utf-8") as f:
            text = f.read()
    except OSError:
        return cfg
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#") or ":" not in stripped:
            continue  # 非 key: value 行 = 忽略（不报错、不猜）
        key, _, value = stripped.partition(":")
        key = key.strip()
        value = value.split("#", 1)[0].strip().strip('"').strip("'").strip()
        if key == "before_min":
            try:
                minutes = int(value)
            except ValueError:
                continue  # 非法值 -> 保留默认（降级，不停摆）
            if 0 <= minutes <= 24 * 60:
                cfg["before_min"] = minutes
            continue
        if key in ("night_at", "morning_at"):
            if _HHMM_RE.match(value):
                cfg[key] = value
            continue
        # 未知键：忽略
    return cfg


# --------------------------------------------------------------------------- #
# 条目身份与幂等 id（V15）
# --------------------------------------------------------------------------- #


def _row_identity(row: dict, date_or_weekday: str) -> str:
    start = (row.get("start") or "").strip()
    end = (row.get("end") or "").strip()
    span = f"{start}-{end}" if end else start
    return "|".join([
        date_or_weekday,
        span,
        (row.get("name") or "").strip(),
        (row.get("location") or "").strip(),
    ])


def entry_key(section: str, row: dict) -> str:
    """课表行的稳定身份：weekly|<weekday>|<start>-<end>|<name>|<location> 或
    onetime|<date>|<start>-<end>|<name>|<location>（end 缺省时省略）。

    学校课程表区块没有提醒计划，故不在此表内（决策 20 按区块判范围）。"""
    if section == "weekly":
        return f"weekly|{_row_identity(row, str(row.get('weekday', '')))}"
    if section == "onetime":
        return f"onetime|{_row_identity(row, str(row.get('date', '')))}"
    raise ValueError(f"unknown reminder section: {section!r} (weekly|onetime)")


def reminder_id(key: str, ladder: str, event_date: str) -> str:
    """幂等 id = 确定性散列(条目 + 档位 + 事件日期)，"rmd-" 前缀 + sha256 派生 hex。

    跨重推/重算不变：输入**只有** (key, ladder, event_date) —— 不掺 now、随机数、进程
    状态、pending.json 既有内容（V15）；改配置时刻只改 fire_at、不改本 id（时刻不是散列
    输入）。长度 <= 40 且仅 [0-9A-Za-z._-] = 设备侧 id[40] 可承载 token。"""
    payload = f"{key}␟{ladder}␟{event_date}".encode("utf-8")
    return "rmd-" + hashlib.sha256(payload).hexdigest()[:24]


# --------------------------------------------------------------------------- #
# 语义中间产物（备注里哪部分算要带的 + 给孩子的文案，OpenClaw agent 回合产出）
# --------------------------------------------------------------------------- #


def _clean_str_list(value) -> list:
    if not isinstance(value, list):
        return []
    return [item.strip() for item in value if isinstance(item, str) and item.strip()]


def load_semantic(path):
    """读语义中间产物 {"<entry_key>": {"title"?, "speech"?, "bring"?: [str]}}。

    缺文件 / 坏 JSON / 顶层非对象 -> (None, reason)：脚本必须能降级继续排程（缺语义 =
    走机械兜底文案，不是不出提醒）。个别条目形状不合法 -> 只丢该条目、不连坐整档。
    """
    try:
        with open(str(path), "r", encoding="utf-8") as f:
            text = f.read()
    except OSError as e:
        return None, f"semantic artifact unreadable: {e.__class__.__name__}"
    try:
        raw = json.loads(text)
    except (ValueError, TypeError) as e:
        return None, f"semantic artifact is not valid JSON: {e.__class__.__name__}"
    if not isinstance(raw, dict):
        return None, "semantic artifact root must be a JSON object keyed by entry_key"

    out = {}
    for key, value in raw.items():
        if not isinstance(key, str) or not isinstance(value, dict):
            continue  # 坏邻居条目单独丢（降级粒度 = 单条目）
        cleaned = {}
        for field in ("title", "speech"):
            got = value.get(field)
            if isinstance(got, str) and got.strip():
                cleaned[field] = got.strip()
        if "bring" in value:
            bring = _clean_str_list(value.get("bring"))
            if not isinstance(value.get("bring"), list):
                continue  # bring 存在但非数组 = 该条目整体不合法
            if bring:
                cleaned["bring"] = bring
        if cleaned:
            out[key] = cleaned
    return out, None


def bring_items(note: str, semantic_entry) -> list:
    """携带内容（RMD-A8）：语义产物给了 bring 就用它（原样、去空串）；否则机械兜底 =
    备注非空时整条备注作单项，备注空 -> []。绝不凭空发明要带物。"""
    if isinstance(semantic_entry, dict):
        bring = _clean_str_list(semantic_entry.get("bring"))
        if bring:
            return bring
    text = (note or "").strip()
    return [text] if text else []


# --------------------------------------------------------------------------- #
# 三档排程（决策 20：按区块范围 + 三段阶梯 + 已过去档位跳过）
# --------------------------------------------------------------------------- #


def _parse_now(now_iso: str) -> datetime:
    text = (now_iso or "").strip()
    if text.endswith("Z") or text.endswith("z"):
        text = text[:-1] + "+00:00"
    try:
        stamp = datetime.fromisoformat(text)
    except ValueError as e:
        raise ValueError(f"now must be ISO8601 with offset: {now_iso!r} ({e})")
    if stamp.tzinfo is None:
        raise ValueError(f"now must carry a UTC offset (契约 §三 +08:00): {now_iso!r}")
    return stamp


def _hhmm(minutes_of_day: int) -> str:
    total = minutes_of_day % (24 * 60)
    return f"{total // 60:02d}:{total % 60:02d}"


def _ladder_fire_at(event_day: date, ladder: str, start_hhmm: str, cfg: dict) -> datetime:
    """三档时刻算术：night = 事件日 -1 天的 night_at（跨天映射）；morning = 事件日当天
    morning_at；soon = 事件开始时刻 - before_min（跨小时减法，落到前一天也算）。"""
    start_min = int(start_hhmm[:2]) * 60 + int(start_hhmm[3:5])
    if ladder == LADDER_NIGHT:
        at = datetime.combine(event_day - timedelta(days=1), datetime.min.time())
        return at + timedelta(hours=int(cfg["night_at"][:2]), minutes=int(cfg["night_at"][3:5]))
    if ladder == LADDER_MORNING:
        at = datetime.combine(event_day, datetime.min.time())
        return at + timedelta(hours=int(cfg["morning_at"][:2]), minutes=int(cfg["morning_at"][3:5]))
    if ladder == LADDER_SOON:
        at = datetime.combine(event_day, datetime.min.time()) + timedelta(minutes=start_min)
        return at - timedelta(minutes=int(cfg["before_min"]))
    raise ValueError(f"unknown ladder: {ladder!r}")


def _view_text(start: str, name: str, location: str, bring: list) -> str:
    text = f"{start} {name}"
    if location:
        text += f" @{location}"
    if bring:
        text += " · " + " · ".join(bring)
    return text


def _fallback_text(ladder: str, start: str, name: str, bring: list) -> tuple:
    """机械兜底文案（无语义产物时用）。night 档必须把备注原文带上 —— 这是「携带内容由
    备注列承载」的可机判落点（RMD-A8）；带物只来自备注/语义输入，绝不凭空发明。"""
    items = "、".join(bring)
    if ladder == LADDER_NIGHT:
        title = f"明天 {start} {name}"
        speech = f"明天{start}有{name}"
        if items:
            speech += f"，记得带上：{items}"
        return title, speech + "。"
    if ladder == LADDER_MORNING:
        title = f"今天 {start} {name}"
        speech = f"今天{start}有{name}"
        if items:
            speech += f"，记得带上：{items}"
        return title, speech + "。"
    title = f"{start} {name} 开始前"
    speech = f"{start} 有{name}，该出发了。"
    if items:
        speech = f"{start} 有{name}，该出发啦，记得带上：{items}。"
    return title, speech


def _rows_in_scope(model: dict, first_day: date, last_day: date):
    """按区块判范围的待排事项生成器：yield (section, row, event_date)。

    weekly = 事件日在窗口内按星期重复生成（周末培训班同样每周重复——决策 20：周末班打破
    平环节奏最容易漏）；onetime = 只在 date 精确匹配窗口内日期时生成（过期一次性安排因此
    永不出现，契约 §2.3）；school（学校课程表区块）**永不生成任何档位**（在校例行孩子
    自己看课表）。"""
    for day_no in range((last_day - first_day).days + 1):
        day = first_day + timedelta(days=day_no)
        weekday = day.isoweekday()   # 契约 §2.3 周一=1..周日=7（与包 6 行模型同）
        for row in (model.get("weekly") or []):
            if row.get("weekday") == weekday:
                yield "weekly", row, day
        for row in (model.get("onetime") or []):
            if str(row.get("date")) == day.isoformat():
                yield "onetime", row, day


def plan_reminders(model: dict, now_iso: str, config=None, semantic=None,
                   horizon_days: int = 7) -> list:
    """契约 §三 提醒条目列表（按 id 升序）。规则全部可离线机判（冻结件 docstring）。"""
    now = _parse_now(now_iso)
    now_local = now.astimezone(_TZ8).replace(tzinfo=None)  # 统一到 +08:00 挂钟再比
    cfg = dict(DEFAULT_LADDER_CONFIG)
    cfg.update({k: v for k, v in (config or {}).items() if k in DEFAULT_LADDER_CONFIG})
    for key in ("night_at", "morning_at"):
        if not _HHMM_RE.match(str(cfg.get(key) or "")):
            cfg[key] = DEFAULT_LADDER_CONFIG[key]
    try:
        cfg["before_min"] = int(cfg.get("before_min", DEFAULT_LADDER_CONFIG["before_min"]))
    except (TypeError, ValueError):
        cfg["before_min"] = DEFAULT_LADDER_CONFIG["before_min"]

    days = int(horizon_days) if horizon_days else 7
    if days < 1:
        days = 1
    first_day = now_local.date()                  # 今天起（今天还没到的事项照样可提醒）
    last_day = first_day + timedelta(days=days - 1)

    entries = []
    seen = set()
    for section, row, event_day in _rows_in_scope(model, first_day, last_day):
        start = (row.get("start") or "").strip()
        if not _HHMM_RE.match(start):
            continue  # 无开始时刻的行不排提醒（脏行由包 6 解析层大声拒绝）
        key = entry_key(section, row)
        sem = (semantic or {}).get(key) if isinstance(semantic, dict) else None
        bring = bring_items((row.get("note") or ""), sem)
        event_date = event_day.isoformat()
        for ladder in (LADDER_NIGHT, LADDER_MORNING, LADDER_SOON):
            fire = _ladder_fire_at(event_day, ladder, start, cfg)
            fire_at = fire.strftime("%Y-%m-%dT%H:%M:%S") + _TZ_OFFSET
            if fire <= now_local:
                continue  # 已过去的档位自动跳过（当晚才录入「明天」的事，前晚档不发）
            rid = reminder_id(key, ladder, event_date)
            if rid in seen:
                continue
            seen.add(rid)
            title, speech = _fallback_text(ladder, start, row.get("name") or "", bring)
            if isinstance(sem, dict):
                title = sem.get("title") or title
                speech = sem.get("speech") or speech
            entries.append({
                "id": rid,
                "fire_at": fire_at,
                "title": title[:60],
                "speech": speech[:90],
                # 内嵌 display 块只取契约 §1.3 白名单（text）—— 不发明提醒专用 view
                "view": {"view": "text", "data": {"text": _view_text(
                    start, row.get("name") or "", (row.get("location") or "").strip(), bring)}},
            })
    entries.sort(key=lambda e: e["id"])
    return entries


# --------------------------------------------------------------------------- #
# pending.json 原子写（同机交接文件，交网关 push helper）
# --------------------------------------------------------------------------- #


def write_pending_atomic(path, entries: list) -> None:
    """原子写交接文件：容器 {"version":1,"generated_at":<iso>,"alerts":[...]}（alerts 按
    id 升序）。临时文件 + os.replace，完成后目录内不残留 .tmp；同一 plan 两次写入的
    alerts 数组逐字节相同（generated_at 可不同且不被消费）。"""
    target = str(path)
    ordered = sorted(entries, key=lambda e: str(e.get("id", "")))
    body = json.dumps({
        "version": 1,
        "generated_at": datetime.now().astimezone().isoformat(timespec="seconds"),
        "alerts": ordered,
    }, ensure_ascii=False, sort_keys=True, separators=(",", ":"))

    folder = os.path.dirname(os.path.abspath(target)) or "."
    os.makedirs(folder, exist_ok=True)
    fd, tmp = tempfile.mkstemp(prefix=".pending-", suffix=".tmp", dir=folder)
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as f:
            f.write(body)
            f.flush()
            os.fsync(f.fileno())
        os.replace(tmp, target)   # 原子换入：网关读不到半成品
    except BaseException:
        try:
            if os.path.exists(tmp):
                os.unlink(tmp)    # 失败路径也必须清掉 .tmp（不留半成品）
        except OSError:
            pass
        raise


# --------------------------------------------------------------------------- #
# cron 载荷入口（OpenClaw cron --command 执行；live 相位人工批准，本包只落载体）
# --------------------------------------------------------------------------- #


def main(argv=None) -> int:
    """重算三档并原子写 pending.json。

    用法（ECS，python3 纯 stdlib）:
      python3 reminder_schedule_lib.py [schedule.md] [pending.json] [semantic.json]
    路径全部 env 可配（KIDPAL_SCHEDULE_MD / KIDPAL_ALERTS_DIR / KIDPAL_PENDING_JSON /
    KIDPAL_SEMANTIC_JSON / KIDPAL_REMINDER_CONFIG）—— 禁止硬编 /root/.openclaw/**。
    缺语义产物 -> 降级继续排程（机械兜底），不抛异常。
    """
    argv = list(sys.argv[1:] if argv is None else argv)
    schedule_md = argv[0] if argv else os.environ.get(
        "KIDPAL_SCHEDULE_MD", "")
    alerts_dir = os.environ.get("KIDPAL_ALERTS_DIR", "")
    pending = argv[1] if len(argv) > 1 else os.environ.get("KIDPAL_PENDING_JSON", "") or (
        os.path.join(alerts_dir, "pending.json") if alerts_dir else "")
    semantic_path = argv[2] if len(argv) > 2 else os.environ.get("KIDPAL_SEMANTIC_JSON", "")
    config_path = os.environ.get(
        "KIDPAL_REMINDER_CONFIG",
        os.path.join(os.path.dirname(os.path.abspath(__file__)), "..",
                     "openclaw-reminder.config.yaml"))
    if not schedule_md or not pending:
        print("usage: reminder_schedule_lib.py <schedule.md> <pending.json> "
              "[semantic.json]  (or set KIDPAL_SCHEDULE_MD/KIDPAL_PENDING_JSON)",
              file=sys.stderr)
        return 2
    try:
        with open(schedule_md, "r", encoding="utf-8") as f:
            model = parse_schedule_md(f.read())
    except Exception as e:  # noqa: BLE001 — cron 侧大声失败留日志，不写半成品
        print(f"schedule parse FAILED: {e!r}", file=sys.stderr)
        return 1
    semantic, sem_err = (None, None)
    if semantic_path:
        semantic, sem_err = load_semantic(semantic_path)
        if sem_err:
            print(f"semantic degrade (mechanical fallback): {sem_err}", file=sys.stderr)
    cfg = load_ladder_config(config_path)
    plan = plan_reminders(model, datetime.now().astimezone().isoformat(timespec="seconds"),
                          config=cfg, semantic=semantic)
    write_pending_atomic(pending, plan)
    print(f"planned {len(plan)} alert slots -> {pending} "
          f"(night_at={cfg['night_at']} morning_at={cfg['morning_at']} "
          f"before_min={cfg['before_min']} semantic={'fallback' if semantic is None else 'loaded'})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
