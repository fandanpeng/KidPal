"""Frozen acceptance tests — TaskPackage 261013-qq-schedule-write-closure
(subsystem: server/deploy〔OpenClaw main workspace skill 面〕+ server/gateway〔front
schedule 解析告警面〕；rigor lite；riskTier medium；sourceType regression；
gateProfile ["test-quality", "docs-sync"]).

需求锚 / story 语义权威 = docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md
（含 2026-10-13 补记：建议②③归 261013-gov-review-deploy-single-source-loop，dev 批只对账
建议①④）。变更计划权威 = delivery/taskpackages/261013-qq-schedule-write-closure/plan.md
（§3 B1 格式权威摘录 / §4 architect 1g 预评全文 / §5 验收面草案）。B1 格式权威 =
docs/contracts/interface-contract.md §2.2/§2.3/§2.4（只读消费，本包零契约改动）。

背景：家长经 QQ 让 OpenClaw main agent 改写 kidpal/schedule.md 是课表主录入通道（契约
§2.4）。main workspace 无写课表 skill（无 B1 格式纪律 / 无写后机检 / 无写后即时重算）——
一个脏日期格（如 '2026-10-12 周一'）令整份 schedule.md 在 front 单源解析器（P2-A2 脏行大
声拒绝）整包拒 → 三触发全静默停摆、家长零感知（2026-10-12 live 实录，TECH 证据 1）。

===========================================================================
Scope of record（taskpackage acceptance QSW-A1..A5）：

  QSW-A1 新增 server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md
         —— 钉死 B1 写入纪律（每周固定区块「星期」列周一~周日 / 一次性安排区块「日期」列
         纯 YYYY-MM-DD 严禁星期后缀 / 时间 HH:MM 或 HH:MM-HH:MM / 五列形状 / 表头逐字范本
         「日期|时间|事项|地点|备注」与「星期|时间|课程|地点|备注」）+ 写后机检步骤（front
         单源 gateway.schedule 的 parse_schedule_md + group_upcoming 实际解析并确认新行在列）
         + 通过后即时重算落 marker（/opt/kidpal/venv/bin/python3 跑 reminder_schedule_lib.py
         + refresh marker）+ 失败处置（还原本次编辑、QQ 回正确格式范本）。
        承载齿：
          · test_a1_write_skill_exists_and_pins_format_headers（机制级）
          · test_a1_write_skill_pins_machine_check_and_recalc（机制级）
          · test_a1_write_skill_pins_date_no_weekday_rule（机制级）
  QSW-A2 server/deploy/manifest.yaml 增 kidpal-schedule-write 与 kidpal-schedule-health 两条
         skill 条目（dst /root/.openclaw/workspace/skills/<name>/SKILL.md）+ cron_jobs 增
         kidpal-schedule-health 核验条目；新增条目字段完整（src/dst/owner/verify/restart）。
        承载齿：
          · test_a2_manifest_registers_two_skill_entries（机制级）
          · test_a2_manifest_registers_health_cron_entry（机制级）
          · test_a2_referenced_skill_srcs_exist_on_disk（机制级；architect 4.7-3 廉加固）
  QSW-A3 front schedule 解析失败 surfaced 告警——server/gateway 新增告警写函数（alerts 目录
         写 schedule-parse-error.json，字段含 checked_at/error/detail，与 deploy-drift.json
         同构），front_app.py 既有 schedule_md_err 日志位（:687 catch 块）挂接调用、日志行为
         不回改；脏日期格（带星期后缀）经 front 单源解析触发告警件写入且字段可判，合法文件
         不写告警件。承载齿：
          · test_a3_dirty_date_cell_writes_alert_with_judgeable_fields（行为级）
          · test_a3_legal_file_writes_no_alert（行为级；守卫）
          · test_a3_alert_fires_through_poll_entry（行为级）
          · test_a3_alert_write_failure_is_swallowed（错误路径，行为级）
  QSW-A4 新增 server/deploy/openclaw-skills/workspace-main/kidpal-schedule-health/SKILL.md
         —— main agent 经轻量 cron 检测 /opt/kidpal/alerts/schedule-parse-error.json，命中
         未确认的解析失败时 QQ 主动告知家长（含坏 token 详情与正确格式引导话术），确认后置
         ack 语义防重复打扰。承载齿：
          · test_a4_health_skill_exists_and_pins_detection_ack_qq（机制级）
  QSW-A5 = live-manual structural 殿后项（真机轮，post-deploy 后由 human 触发）——**不落
         offline runner**（taskpackage acceptance[4] kind=structural + notes【真机门】）。
         依赖：ECS bootstrap（261013-gov-review-deploy-single-source-loop 的 ecs-bootstrap.md）
         + openclaw cron 注册 + QQ 路由目标配置（家长 openid，TECH-261012 证据 5 既有缺口）。
         本件不写可执行齿，证据入 closeout 制品。

================================= 断言设计（标的 / 判别力）=================================

技术路线（QSW-A3）= **行为级**（M4K1 / tech260928-school-start 先例：fastapi stub +
front_app.py 离线装载）。理由：告警写函数落 front_app.py（plan §4.2 定案——schedule.py 是
冻结纯解析模块、加 I/O 破纯净性），其私有 helper 名未冻结；但 story 与 architect 均把挂接点
钉死在 `_load_schedule_model()` 的 `except ValueError` 块（现 :687 日志行旁），故本件以
`_load_schedule_model()` 为冻结观测缝：脏输入真跑 → 断言 **alerts 目录出现
schedule-parse-error.json 且字段可判**（设备/运维可见的副产物），比 grep 源码强一档（写了
helper 但没在 catch 块挂接 / 挂接在别处不生效都逃不掉）。合法性判据 = 文件在场且
checked_at/error/detail 三字段语义可判。

QSW-A1/A2/A4 技术路线 = **结构性文本/落盘齿**（test_261001-openclaw-model-name 先例）。
标的 = 仓内 skill 文本（OpenClaw 消费的护栏）与 manifest 登记（verify-deploy.sh 消费），
判别力 = 钉住 story 逐字指定的契约字形（表头范本、日期/时间格式 token）与冻结机器面
（parse_schedule_md / group_upcoming / 单源 parser 绝对路径 / 钉死解释器 / 告警件与 ack 件
文件名）在场、日期无星期后缀纪律在场。纯散文措辞不逐字钉（防脆），但语义 load-bearing 的
token 精确钉。

==================== 契约条目 ↔ 齿 的强度申报（acceptance-evidence T2 体例）====================

  映射（story 条目 → 承载齿 → 性质档）：
   · QSW-A1 写入纪律字形（表头逐字范本 + 日期/时间格式）→ a1_format_headers 齿 → 机制级
   · QSW-A1 写后机检 + 即时重算（parse_schedule_md / group_upcoming / 单源绝对路径 /
     钉死解释器 / reminder_schedule_lib.py / refresh marker）→ a1_machine_check 齿 → 机制级
   · QSW-A1 日期无星期后缀纪律 → a1_date_rule 齿 → 机制级（宽松正则，见下）
   · QSW-A2 两 skill 条目字段完整 + dst 落点 → a2_skill_entries 齿 → 机制级
   · QSW-A2 cron 条目（主体 main + 调度 every 15m）→ a2_cron_entry 齿 → 机制级
   · QSW-A2 src 在仓内真实存在 → a2_srcs_exist 齿 → 机制级
   · QSW-A3 脏格触发告警件 + 字段可判 → a3_dirty_alert 齿 → 行为级
   · QSW-A3 合法文件不写告警件 → a3_legal_no_alert 齿 → 行为级（守卫）
   · QSW-A3 告警经真实轮询入口触发 → a3_poll_entry 齿 → 行为级
   · QSW-A3 写告警件失败静默吞（不炸轮询循环）→ a3_swallow 齿 → 行为级（错误路径）
   · QSW-A4 检测/ack/QQ 三语义 token → a4_health_skill 齿 → 机制级

  缺口（承载齿强度低于 story 所需处，逐条显式化）：
   · **QSW-A1/A4 是 skill 文本护栏，offline 只证「护栏文本在场且含 load-bearing token」，
     不证「OpenClaw main agent 读到后真的按此改写」**——后者需 live 真机（QSW-A5 殿后）。
     本件是「护栏已写对」而非「护栏已生效」，等价假设显式化。
   · **QSW-A2 manifest 的 yaml 结构合法性 + 条目完整性扫描 canonical 归属 = ECS 侧
     verify-deploy.sh**（plan §4.7-2：server 测试面零 `import yaml` 纪律）。本件只用
     stdlib 做「src/dst/cron 键在场 + 字段在场 + src 文件在仓内存在」的文本/路径断言，
     **不做 yaml.safe_load**——yaml 解析通过与否归 ECS 侧口径（QSW-A2 原文「yaml.safe_load
     解析通过」不在 host 面承载）。
   · **QSW-A3 「日志行为不回改」不在本件断言**——本件只钉新增告警副作用；既有
     `log.warning("schedule_md_err …")` 行是否保留归 builder 自由（story 要求不回改，
     但日志文本属内部实现、非设备/运维可见契约面，不钉逐字）。
   · **a1_date_rule / a4_ack 用宽松正则**（builder 散文措辞自由）：命中任一常见表述即过，
     非逐字钉——防脆；同时保留 a1_format_headers 的逐字表头钉作为主判别力。
   · **checked_at 精确格式不逐字钉**——plan §4.2 定「ISO8601 UTC、对齐 deploy-drift.json
     同名字段口径」；本件只钉「存在且形如 `YYYY-MM-DDTHH:MM…`」（deploy-drift.json 的
     producer 在 verify-deploy.sh，本件不复刻其时区精度契约）。

  行为等价假设（显式化，不写即默认不成立）：
   · 「`_load_schedule_model()` 脏输入 → 告警件在场」⇔「front 每分钟轮询命中 schedule_md_err
     时 surfaced 告警」——假设轮询循环每 tick 调 `_load_schedule_model()`（front_app.py
     :828 + reminder_poll_loop 已核）；本件另加 a3_poll_entry 齿经真实 `run_schedule_poll_once`
     入口复证该等价，不单靠直调 helper。

Run protocol（文件名两点须 importlib 模式；root 由 __file__ 显式推导，不依赖调用点 cwd——
acceptance-evidence root 注入纪律）:
  cd server && PYTHONIOENCODING=utf-8 python -m pytest -q --import-mode=importlib \
      -m "not live" acceptance/test_261013_qq_schedule_write_closure.acceptance.py
  (standalone: python 本文件)
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import json
import os
import re
import sys
import tempfile
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

try:
    import pytest  # noqa: F401  (markers only; standalone unittest runner also works)
    _HAS_PYTEST = True
except ImportError:  # pragma: no cover
    _HAS_PYTEST = False

# --------------------------------------------------------------------------- #
# Paths（仓根由 __file__ 显式推导）
# --------------------------------------------------------------------------- #

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY = SERVER / "deploy"
FRONT_APP = SERVER / "gateway" / "front" / "front_app.py"

SKILLS_DIR = DEPLOY / "openclaw-skills" / "workspace-main"
WRITE_SKILL = SKILLS_DIR / "kidpal-schedule-write" / "SKILL.md"
HEALTH_SKILL = SKILLS_DIR / "kidpal-schedule-health" / "SKILL.md"
MANIFEST = DEPLOY / "manifest.yaml"

# --------------------------------------------------------------------------- #
# 契约 §2.2/§2.3 逐字字形与冻结机器面常量
# --------------------------------------------------------------------------- #

# 表头逐字范本（contract §2.2；2026-10-12 事故源 = 表头曾漂成「星期/日期」「课程/事项」）
HDR_WEEKLY = "| 星期 | 时间 | 课程 | 地点 | 备注 |"
HDR_ONETIME = "| 日期 | 时间 | 事项 | 地点 | 备注 |"

# 冻结机器面 token（story QSW-A1 逐字指定）
SINGLE_SOURCE_PARSER = "/opt/kidpal/front/gateway/schedule.py"  # front 单源解析器 canonical 位
PINNED_INTERPRETER = "/opt/kidpal/venv/bin/python3"             # 钉死绝对路径（3.6.8 坑）
RECALC_LIB = "reminder_schedule_lib.py"                          # cron 同款即时重算脚本
REFRESH_MARKER = "schedule.refresh"                             # 触发③ 即时重算 marker

# 告警件 / ack 件文件名（plan §4.2/§4.3 + taskpackage notes 定稿）
ALERT_FILENAME = "schedule-parse-error.json"
ACK_FILENAME = "schedule-parse-health.json"

# 脏日期格（2026-10-12 live 事故 token：日期带星期后缀）
DIRTY_TOKEN = "2026-10-12 周一"

# manifest dst 落点（story QSW-A2：live 落点 /root/.openclaw/workspace/skills/<name>/SKILL.md）
WRITE_SKILL_SRC = "server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md"
HEALTH_SKILL_SRC = "server/deploy/openclaw-skills/workspace-main/kidpal-schedule-health/SKILL.md"
WRITE_SKILL_DST = "/root/.openclaw/workspace/skills/kidpal-schedule-write/SKILL.md"
HEALTH_SKILL_DST = "/root/.openclaw/workspace/skills/kidpal-schedule-health/SKILL.md"
HEALTH_CRON = "kidpal-schedule-health"

# 宽松正则（散文措辞 builder 自由；命中任一常见表述即过，防脆）
_A1_DATE_RULE_PATTERNS = [
    r"(禁|勿|不要|不得|别|切勿)[^\n]{0,14}星期",     # 严禁星期后缀 / 不要带星期
    r"星期[^\n]{0,14}(后缀|不要|禁|勿|别|错|多余)",   # 星期后缀 / 星期…不要写
    r"日期[^\n]{0,24}YYYY-MM-DD",                    # 日期…YYYY-MM-DD
]
_A4_ACK_RULE_PATTERNS = [
    r"(ack|ACK|已告知|已通知|已提醒)",
    r"(防|避免|不)[^\n]{0,8}重复",
    r"(去重|指纹|fingerprint)",
]
# B1 合法 / 脏 样本（脏 = 一次性安排区块日期格带星期后缀）
LEGAL_B1 = """---
child: 测试娃
timezone: Asia/Shanghai
---

# 测试娃的课表

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-08-30 | 14:00-15:00 | 牙医复诊 | 口腔医院 | |
"""
DIRTY_B1 = LEGAL_B1.replace("2026-08-30", DIRTY_TOKEN)


# --------------------------------------------------------------------------- #
# 读取工具
# --------------------------------------------------------------------------- #

def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _missing_needles(text: str, pairs) -> list:
    """收集缺席的 (label, needle) —— 用 offenders 列表形态一次报全，方法级明确 FAILED。"""
    return [f"{label}（缺 {needle!r}）" for label, needle in pairs if needle not in text]


# --------------------------------------------------------------------------- #
# front_app.py 离线装载（M4K1 / tech260928 先例：fastapi stub + spec_from_file_location）
# --------------------------------------------------------------------------- #

_ensure_fastapi_stub_done = False
_front_module_cache = None


def _ensure_fastapi_stub() -> None:
    """front_app.py 离线装载的 fastapi stub（g-relay-fix / M4K1 / tech260928 同款先例）。"""
    global _ensure_fastapi_stub_done
    if _ensure_fastapi_stub_done:
        return
    try:
        import fastapi  # noqa: F401
        _ensure_fastapi_stub_done = True
        return  # real fastapi present — even better, use it
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    if "fastapi" not in sys.modules:
        stub = _PermissiveModule("fastapi")
        stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
        stub.HTTPException = type("HTTPException", (Exception,), {})
        sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _PermissiveModule("fastapi.responses")
    _ensure_fastapi_stub_done = True


def _load_front_module():
    """Import server/gateway/front/front_app.py by path, offline. (mod|None, err).

    装载坑（architect 4.5）：SCHEDULE_MD / ALERTS_DIR 是 front_app 模块级常量、**import 时**
    从 env 读（front_app.py :668-669）。故装载前先把两 env 钉到临时默认位（绝不触真实
    /opt/kidpal），装载后立即还原 env（模块常量已捕获安全值，逐齿再 patch 模块属性）。
    """
    global _front_module_cache
    if _front_module_cache is not None:
        return _front_module_cache
    if not FRONT_APP.is_file():
        return None, "server/gateway/front/front_app.py missing — 网关解析告警面不在世。"
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))

    safe_md = Path(tempfile.gettempdir()) / "kidpal_absent_schedule.md"
    safe_alerts = Path(tempfile.gettempdir()) / "kidpal_absent_alerts"
    keys = ("KIDPAL_SCHEDULE_MD", "KIDPAL_ALERTS_DIR")
    saved = {k: os.environ.get(k) for k in keys}
    os.environ["KIDPAL_SCHEDULE_MD"] = str(safe_md)
    os.environ["KIDPAL_ALERTS_DIR"] = str(safe_alerts)
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_qsw", FRONT_APP)
        if spec is None or spec.loader is None:
            return None, "front_app.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
    except Exception as e:  # noqa: BLE001 — import error here is a RED, not a crash
        return None, (f"front_app.py failed to import: {e!r}. 冻结口径：该模块必须离线可装载"
                      "（import 期零网络 I/O；fastapi 缺席走本地 stub，M4K1 先例）。")
    finally:
        for k, v in saved.items():
            if v is None:
                os.environ.pop(k, None)
            else:
                os.environ[k] = v
    _front_module_cache = (mod, None)
    return mod, None


def _front():
    mod, err = _load_front_module()
    if mod is None:
        raise AssertionError("QSW RED: " + err)
    return mod


@contextlib.contextmanager
def _front_paths(front, schedule_md: Path, alerts_dir: Path):
    """钉死 SCHEDULE_MD / ALERTS_DIR 到临时位：同时 patch 模块属性与 env（对两种实现都生效，
    逐齿 try/finally 还原，不污染同进程其它测试）。"""
    keys = ("KIDPAL_SCHEDULE_MD", "KIDPAL_ALERTS_DIR")
    saved_env = {k: os.environ.get(k) for k in keys}
    saved_attr = (front.SCHEDULE_MD, front.ALERTS_DIR)
    front.SCHEDULE_MD = str(schedule_md)
    front.ALERTS_DIR = str(alerts_dir)
    os.environ["KIDPAL_SCHEDULE_MD"] = str(schedule_md)
    os.environ["KIDPAL_ALERTS_DIR"] = str(alerts_dir)
    try:
        yield
    finally:
        front.SCHEDULE_MD, front.ALERTS_DIR = saved_attr
        for k, v in saved_env.items():
            if v is None:
                os.environ.pop(k, None)
            else:
                os.environ[k] = v


def _manifest_flow_block(text: str, needle: str):
    """取 manifest 里含 needle 的 flow-map 条目 `{...}`（flow 无嵌套花括号，DOTALL 捕获）。"""
    m = re.search(r"\{[^{}]*?" + re.escape(needle) + r"[^{}]*?\}", text, re.DOTALL)
    return m.group(0) if m else None


def _manifest_list_item_block(text: str, name: str):
    """取 cron_jobs 里 `- name: <name>` 起、到下一 `- name:`（或结尾）的块。"""
    m = re.search(r"^\s*-\s*name:\s*" + re.escape(name) + r"\s*$", text, re.M)
    if not m:
        return None
    rest = text[m.end():]
    nxt = re.search(r"^\s*-\s*name:", rest, re.M)
    end = m.end() + (nxt.start() if nxt else len(rest))
    return text[m.start():end]


# =========================================================================== #
# QSW-A1 — kidpal-schedule-write skill 文本齿                                    #
# =========================================================================== #

class QswA1WriteSkill(unittest.TestCase):
    """QSW-A1: main workspace 写课表 skill 钉死 B1 写入纪律 + 写后机检 + 即时重算 + 失败处置。"""

    maxDiff = None

    def setUp(self):
        self.assertTrue(
            WRITE_SKILL.is_file(),
            f"QSW-A1 RED: 写课表 skill 不在位 —— 要求文件 {WRITE_SKILL}（main workspace "
            "写课表护栏；缺它 = 家长 QQ 录入无 B1 格式纪律，脏格致整份 schedule.md 解析失败）。")
        self.text = _read_text(WRITE_SKILL)
        self.assertTrue(self.text.strip(), f"QSW-A1 RED: {WRITE_SKILL} 为空。")

    def test_a1_write_skill_exists_and_pins_format_headers(self):
        """coverage: QSW-A1 — skill 钉死 B1 表头逐字范本 + 日期/时间/五列格式（机制级）。

        逐字钉两区块表头（契约 §2.2）——2026-10-12 事故源正是表头漂成「星期/日期」「课程/
        事项」；逐字表头在场 = 护栏具备防止该漂移的字形。
        """
        missing = _missing_needles(self.text, (
            ("每周固定区块表头（星期|时间|课程|地点|备注）", HDR_WEEKLY),
            ("一次性安排区块表头（日期|时间|事项|地点|备注）", HDR_ONETIME),
            ("一次性安排日期格式 token YYYY-MM-DD", "YYYY-MM-DD"),
            ("时间格式 token HH:MM", "HH:MM"),
        ))
        self.assertEqual(
            missing, [],
            f"QSW-A1 RED: skill 缺 B1 表头逐字范本 / 日期时间格式字形 —— 缺项 = {missing}。"
            "缺表头范本 = 家长录入无护栏、脏格可再漂进 schedule.md（2026-10-12 事故源）。")

    def test_a1_write_skill_pins_machine_check_and_recalc(self):
        """coverage: QSW-A1 — 写后机检（parse_schedule_md + group_upcoming，front 单源绝对
        路径 + 钉死解释器）+ 通过后即时重算（reminder_schedule_lib.py + refresh marker）
        （机制级）。"""
        missing = _missing_needles(self.text, (
            ("front 单源解析函数 parse_schedule_md", "parse_schedule_md"),
            ("当日「接下来」计算函数 group_upcoming", "group_upcoming"),
            ("front 单源 parser 绝对路径", SINGLE_SOURCE_PARSER),
            ("钉死解释器 /opt/kidpal/venv/bin/python3", PINNED_INTERPRETER),
            ("即时重算脚本 reminder_schedule_lib.py", RECALC_LIB),
            ("触发③ 即时重算 marker schedule.refresh", REFRESH_MARKER),
        ))
        self.assertEqual(
            missing, [],
            f"QSW-A1 RED: skill 缺写后机检 / 即时重算步骤 —— 缺项 = {missing}。缺写后机检 = "
            "agent 改写后 front 不校验、不重算，脏文件可静默停摆（触发③ 病灶）。")

    def test_a1_write_skill_pins_date_no_weekday_rule(self):
        """coverage: QSW-A1 — 日期列无星期后缀纪律（宽松正则：命中任一常见表述即过；机制级）。

        判别力：2026-10-12 live 事故 = 日期格 '2026-10-12 周一' 被写进一次性安排区块。护栏
        必须明文「日期纯 YYYY-MM-DD、不带星期」。措辞 builder 自由，故用正则族。
        """
        self.assertTrue(
            any(re.search(p, self.text) for p in _A1_DATE_RULE_PATTERNS),
            "QSW-A1 RED: skill 未明文「一次性安排日期不带星期后缀」纪律 —— 正则族 "
            f"{_A1_DATE_RULE_PATTERNS!r} 均未命中 {WRITE_SKILL}。该纪律是本包要消除的病灶"
            "根因（脏日期格致整份解析失败），必须入护栏文本。")


# =========================================================================== #
# QSW-A2 — manifest 两 skill 条目 + cron 条目                                    #
# =========================================================================== #

class QswA2Manifest(unittest.TestCase):
    """QSW-A2: manifest.yaml 登记两 skill 条目（字段完整 + dst 落点）与 health cron 条目。

    stdlib-only（plan §4.7-2：server 测试面零 `import yaml` 纪律）——yaml.safe_load 解析
    通过属 ECS 侧 verify-deploy.sh 口径，本件不承载；只做文本/路径在场断言。
    """

    maxDiff = None

    def setUp(self):
        self.assertTrue(MANIFEST.is_file(), f"QSW-A2 RED: {MANIFEST} 不在位。")
        self.text = _read_text(MANIFEST)

    def test_a2_manifest_registers_two_skill_entries(self):
        """coverage: QSW-A2 — 两 skill 条目在场且字段完整（src/dst/owner/verify/restart）（机制级）。

        与既有 5 条 skill 条目同款（architect 4.4）：owner: deploy.sh / verify: md5 /
        restart: none。src 必须与 SKILL.md 同批原子落（否则 verify-deploy.sh 报源缺失）。
        """
        offenders = []
        for label, src, dst in (
            ("kidpal-schedule-write", WRITE_SKILL_SRC, WRITE_SKILL_DST),
            ("kidpal-schedule-health", HEALTH_SKILL_SRC, HEALTH_SKILL_DST),
        ):
            block = _manifest_flow_block(self.text, src)
            if block is None:
                offenders.append(f"{label}: 无 flow 条目（src {src!r} 未登记）")
                continue
            if dst not in block:
                offenders.append(f"{label}: dst 非 {dst!r}（块 = {block!r}）")
            for field in ("owner: deploy.sh", "verify: md5", "restart: none"):
                if field not in block:
                    offenders.append(f"{label}: 缺字段 {field!r}（块 = {block!r}）")
        self.assertEqual(
            offenders, [],
            f"QSW-A2 RED: manifest skill 条目缺失/字段不完整 —— {offenders}。缺条目 = "
            "verify-deploy.sh 不铺放该 skill、live 不生效；字段不完整 = 核验不完整。")

    def test_a2_manifest_registers_health_cron_entry(self):
        """coverage: QSW-A2 — cron_jobs 增 kidpal-schedule-health 条目（主体 main + 调度
        every 15m）（机制级）。

        频率恒 15m：verify-deploy.sh 把 expect_schedule 硬编成 every_min == 15（:123-127），
        非 15m 会假报漂移（architect 4.3）。
        """
        block = _manifest_list_item_block(self.text, HEALTH_CRON)
        self.assertIsNotNone(
            block,
            f"QSW-A2 RED: manifest cron_jobs 无 {HEALTH_CRON} 条目 —— health skill 无调度"
            "唤起面、解析失败不会被 QQ 转告。")
        self.assertIn(
            "expect_agent: main", block,
            f"QSW-A2 RED: {HEALTH_CRON} cron 未钉 expect_agent: main —— 条目块 = {block!r}。")
        self.assertIn(
            'expect_schedule: "every 15m"', block,
            f"QSW-A2 RED: {HEALTH_CRON} cron 未钉 expect_schedule: \"every 15m\" —— "
            f"条目块 = {block!r}。频率须恒 15m（verify-deploy.sh 硬核验 every_min==15）。")

    def test_a2_referenced_skill_srcs_exist_on_disk(self):
        """coverage: QSW-A2 — manifest 引用的两新 SKILL.md src 在仓内真实存在（机制级；
        architect 4.7-3 廉加固）。专抓 src 拼写错 / 忘建文件（verify: md5 的 src 缺失会让
        verify-deploy.sh 报「仓内源缺失」）。"""
        missing = [src for src in (WRITE_SKILL_SRC, HEALTH_SKILL_SRC)
                   if not (REPO_ROOT / src).is_file()]
        self.assertEqual(
            missing, [],
            f"QSW-A2 RED: manifest 登记的 src 在仓内不存在 —— {missing}。src 拼写错或 "
            "SKILL.md 未建；verify: md5 的源缺失会让 verify-deploy.sh 报「仓内源缺失」。")


# =========================================================================== #
# QSW-A3 — front schedule 解析失败 surfaced 告警（行为级）                        #
# =========================================================================== #

class QswA3ParseAlert(unittest.TestCase):
    """QSW-A3: 脏日期格经 front 单源解析触发 alerts/schedule-parse-error.json 写入且字段可判；
    合法文件不写；写失败静默吞。"""

    maxDiff = None

    def _tmp(self):
        d = tempfile.mkdtemp(prefix="qsw_a3_")
        self.addCleanup(lambda: __import__("shutil").rmtree(d, ignore_errors=True))
        return Path(d)

    def test_a3_dirty_date_cell_writes_alert_with_judgeable_fields(self):
        """coverage: QSW-A3 — 脏日期格（带星期后缀）经 front 单源解析失败 → 告警件写入且
        checked_at/error/detail 字段可判（行为级）。"""
        front = _front()
        tmp = self._tmp()
        schedule_md = tmp / "schedule.md"
        alerts = tmp / "alerts"
        schedule_md.write_text(DIRTY_B1, encoding="utf-8")

        with _front_paths(front, schedule_md, alerts):
            model = front._load_schedule_model()

        self.assertIsNone(
            model,
            "QSW-A3 前置条件破坏：脏日期格未致解析失败（front 单源 parser 行为变了吗？）—— "
            f"token {DIRTY_TOKEN!r} 应触发 ValueError、_load_schedule_model 返回 None。")
        alert_path = alerts / ALERT_FILENAME
        self.assertTrue(
            alert_path.is_file(),
            f"QSW-A3 RED: 解析失败未落告警件 {alert_path} —— front 对 schedule_md_err 仍静默"
            "（家长零感知病灶未消）。要求 alerts 目录写 schedule-parse-error.json。")
        report = json.loads(alert_path.read_text(encoding="utf-8"))
        self.assertIsInstance(report, dict, f"QSW-A3 RED: 告警件非 JSON 对象：{report!r}")
        for field in ("checked_at", "error", "detail"):
            self.assertIn(
                field, report,
                f"QSW-A3 RED: 告警件缺字段 {field!r}（在场 = {sorted(report)}）—— 要求"
                " {checked_at, error, detail} 三字段（与 deploy-drift.json 同构）。")
        self.assertEqual(
            report["error"], "ValueError",
            f"QSW-A3 RED: 告警件 error = {report['error']!r} != 'ValueError' —— 脏日期格经"
            " _parse_date_token 应抛 ValueError（type(e).__name__）。")
        self.assertIn(
            DIRTY_TOKEN, str(report["detail"]),
            f"QSW-A3 RED: 告警件 detail 未含坏 token {DIRTY_TOKEN!r} —— detail 应取自"
            f" str(e)[:120]（与现日志同源）。实得 = {report['detail']!r}。")
        self.assertRegex(
            str(report["checked_at"]), r"^\d{4}-\d{2}-\d{2}T",
            f"QSW-A3 RED: 告警件 checked_at 非 ISO8601 形态 —— 实得 = {report['checked_at']!r}。")

    def test_a3_legal_file_writes_no_alert(self):
        """coverage: QSW-A3 — 合法 B1 文件解析成功 → 不写告警件（行为级；守卫齿，现状即
        must-stay-green，防 builder 把告警写挂到成功路径）。"""
        front = _front()
        tmp = self._tmp()
        schedule_md = tmp / "schedule.md"
        alerts = tmp / "alerts"
        schedule_md.write_text(LEGAL_B1, encoding="utf-8")

        with _front_paths(front, schedule_md, alerts):
            model = front._load_schedule_model()

        self.assertIsNotNone(
            model, "QSW-A3 前置条件破坏：合法 B1 解析失败（样本错了吗？）。")
        self.assertFalse(
            (alerts / ALERT_FILENAME).exists(),
            f"QSW-A3 RED: 合法文件也写了告警件 {alerts / ALERT_FILENAME} —— 告警写误挂"
            "成功路径 = 假告警（家长被无谓打扰）。")

    def test_a3_alert_fires_through_poll_entry(self):
        """coverage: QSW-A3 — 告警经真实轮询入口 run_schedule_poll_once 触发（行为级）。

        证「每分钟轮询命中 schedule_md_err 时 surfaced 告警」（行为等价假设经真实入口复证，
        不单靠直调 _load_schedule_model）。
        """
        front = _front()
        tmp = self._tmp()
        schedule_md = tmp / "schedule.md"
        alerts = tmp / "alerts"
        schedule_md.write_text(DIRTY_B1, encoding="utf-8")

        with _front_paths(front, schedule_md, alerts):
            report = asyncio.run(front.run_schedule_poll_once())

        self.assertEqual(
            report.get("reason"), "no_model",
            f"QSW-A3 RED: poll 入口脏输入未走 no_model 分支 —— report = {report!r}。")
        self.assertTrue(
            (alerts / ALERT_FILENAME).is_file(),
            f"QSW-A3 RED: 经 run_schedule_poll_once 入口未落告警件 {alerts / ALERT_FILENAME}"
            " —— 轮询命中 schedule_md_err 时告警未 surfaced。")

    def test_a3_alert_write_failure_is_swallowed(self):
        """coverage: QSW-A3 — 告警写失败静默吞（错误路径，行为级）。

        plan §4.2：告警写必须自身 try/except 全吞，不得让写告警件失败反向上抛——
        _load_schedule_model 契约是「返回 None、不炸轮询循环」。构造 ALERTS_DIR 落在常规
        文件之下（写必失败），断言仍返回 None 且不抛。
        """
        front = _front()
        tmp = self._tmp()
        schedule_md = tmp / "schedule.md"
        schedule_md.write_text(DIRTY_B1, encoding="utf-8")
        blocker = tmp / "blocker_file"          # 常规文件
        blocker.write_text("x", encoding="utf-8")
        bad_alerts = blocker / "nested_alerts"  # 落在文件之下 -> mkdir/open 必失败

        with _front_paths(front, schedule_md, bad_alerts):
            try:
                model = front._load_schedule_model()
            except Exception as e:  # noqa: BLE001
                self.fail(
                    f"QSW-A3 RED: 写告警件失败反向上抛 {e!r} —— 告警写必须自身全吞，"
                    "_load_schedule_model 契约是「返回 None、不炸轮询循环」（plan §4.2）。")
        self.assertIsNone(
            model,
            "QSW-A3 RED: 告警写失败时 _load_schedule_model 未返回 None。")


# =========================================================================== #
# QSW-A4 — kidpal-schedule-health skill 文本齿                                   #
# =========================================================================== #

class QswA4HealthSkill(unittest.TestCase):
    """QSW-A4: main agent health skill 检测告警件 → QQ 主动告知 → ack 防重复打扰。"""

    maxDiff = None

    def setUp(self):
        self.assertTrue(
            HEALTH_SKILL.is_file(),
            f"QSW-A4 RED: health skill 不在位 —— 要求文件 {HEALTH_SKILL}（main agent 解析"
            "失败 QQ 转告护栏；缺它 = 告警件落了也无人转告家长）。")
        self.text = _read_text(HEALTH_SKILL)
        self.assertTrue(self.text.strip(), f"QSW-A4 RED: {HEALTH_SKILL} 为空。")

    def test_a4_health_skill_exists_and_pins_detection_ack_qq(self):
        """coverage: QSW-A4 — skill 钉死检测面（schedule-parse-error.json）+ ack 状态面
        （schedule-parse-health.json）+ QQ 转告语义（机制级）。"""
        missing = _missing_needles(self.text, (
            ("front 告警件 schedule-parse-error.json", ALERT_FILENAME),
            ("ack 独立状态件 schedule-parse-health.json", ACK_FILENAME),
            ("QQ 转告语义", "QQ"),
        ))
        self.assertEqual(
            missing, [],
            f"QSW-A4 RED: health skill 缺检测/ack/QQ 面 —— 缺项 = {missing}。缺检测/ack/QQ 面 "
            "= 解析失败不被主动告知家长（告警件落了也无人转告）。")
        self.assertTrue(
            any(re.search(p, self.text) for p in _A4_ACK_RULE_PATTERNS),
            "QSW-A4 RED: health skill 未明文 ack 防重复打扰语义 —— 正则族 "
            f"{_A4_ACK_RULE_PATTERNS!r} 均未命中 {HEALTH_SKILL}。")


# =========================================================================== #
# QSW-A5 — live-manual structural（不落 offline runner，仅头注申报）              #
# =========================================================================== #
# QSW-A5 无离线可执行齿：依赖 ECS bootstrap + openclaw cron 注册 + QQ 路由目标配置
# （家长 openid，TECH-261012 证据 5 既有缺口），post-deploy 由 human 触发、证据入
# closeout 制品（taskpackage acceptance[4] + notes【真机门】）。见本件头注 Scope of record。


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
