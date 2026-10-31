#!/usr/bin/env python3
"""KidPal 261031-openclaw-cron-trigger-gating — 判定器 A（提醒语义回合静默门）。

纯只读、无网络、不写盘；stdout 恒为**严格单行 JSON** `{fire: bool, message?, state?}`。
正常 exit 0；内部异常（含数值型 env 传非数字）exit≠0 且不输出任何半截判决 JSON。

消费方 = `reminder-semantic.trigger.js` 薄壳（OpenClaw cron `trigger.script`），每 15 分钟
无模型跑一次：内容未变 → fire:false（不起 agent 回合、零 token）；任一输入变化 → fire:true
（薄壳把 message 当该轮上下文，payload 按 kidpal-reminder-semantic skill 重算 semantic.json）。

fire:true 条件（命任一；schedule.md 缺失为终局守卫 → 恒 fire:false）：
  ① semantic.json 缺失；
  ② semantic.json 非法 JSON；
  ③ mtime(semantic) - mtime(schedule) <= KIDPAL_SEMANTIC_GATE_MARGIN_SEC（默认 60s，
     读写竞态余量；边界含等于）；
  ④ mtime(semantic) <= mtime(skill) | mtime(gateway/schedule.py) | mtime(reminder_schedule_lib.py)
     （margin 0；边界含等于——与下游同 mtime 视为落后）。

明确不覆盖：AGENTS.md / 系统 prompt 变更不触发重算（semantic.json 内容 = schedule × skill ×
parser × lib 的函数）；出现实证再扩输入集。

python 3.6 兼容（ECS 有 3.6 解释器先例；本脚本用 venv 3.11 跑，但保持 3.6-safe 写法）。
"""

import json
import os
import sys

DEFAULT_SCHEDULE_MD = "/root/.openclaw/workspace/kidpal/schedule.md"
DEFAULT_SEMANTIC_JSON = "/opt/kidpal/alerts/semantic.json"
DEFAULT_SKILL = "/root/.openclaw/workspace/skills/kidpal-reminder-semantic/SKILL.md"
DEFAULT_PARSER = "/opt/kidpal/front/gateway/schedule.py"
DEFAULT_LIB = "/opt/kidpal/openclaw-patch/reminder_schedule_lib.py"
DEFAULT_MARGIN_SEC = 60


def _env_int(name, default):
    """读数值型 env；显式传入非数字 → ValueError（fail-loud，禁静默回落默认值）。"""
    raw = os.environ.get(name)
    if raw is None or not str(raw).strip():
        return default
    return int(str(raw).strip())  # 非数字在此抛 ValueError


def _mtime(path):
    try:
        return os.path.getmtime(path)
    except OSError:
        return None


def compute(schedule_md, semantic_json, skill_md, parser_py, lib_py, margin_sec):
    """纯函数真值表（单测直接调；env 读取在 main）。返回 {fire, message?}。"""
    # 终局守卫：无课表 = 不产语义产物（skill 约定不写、保留旧产物）——即便下游变新也不 fire。
    if _mtime(schedule_md) is None:
        return {"fire": False}

    if _mtime(semantic_json) is None:
        return {
            "fire": True,
            "message": "semantic.json 缺失：提醒语义回合尚未产出语义产物，请按 skill 重算。",
        }

    try:
        with open(semantic_json, "r", encoding="utf-8") as fh:
            json.load(fh)
    except (OSError, ValueError):
        return {
            "fire": True,
            "message": "semantic.json 非法 JSON：语义产物损坏，请按 skill 重算。",
        }

    sem_m = os.path.getmtime(semantic_json)
    sched_m = os.path.getmtime(schedule_md)
    reasons = []

    # ③ 读写竞态余量：mtime(semantic) - mtime(schedule) <= margin（边界含等于）
    if sem_m - sched_m <= margin_sec:
        reasons.append(
            "schedule.md 比 semantic.json 新（课表 mtime 差 %.0fs <= %ss 竞态余量），"
            "课表可能在 agent 读取后又被修改，需补跑一轮。"
            % (sem_m - sched_m, margin_sec)
        )

    # ④ 下游输入比 semantic 新（margin 0；同 mtime 视为落后）
    downstream = (
        ("提醒语义 skill", "skill", skill_md),
        ("课表解析器 gateway/schedule.py", "parser", parser_py),
        ("提醒排程库 reminder_schedule_lib.py", "lib", lib_py),
    )
    for label, _kind, path in downstream:
        m = _mtime(path)
        if m is not None and sem_m <= m:
            reasons.append("%s 比 semantic.json 新（或同 mtime），语义产物已过期，需重算。" % label)

    if reasons:
        return {
            "fire": True,
            "message": "；".join(reasons) + " 请运行 kidpal-reminder-semantic 回合重算 semantic.json。",
        }
    return {"fire": False}


def _force_utf8_stdio():
    """控制台/重定向面强制 UTF-8（Windows cp1252 会因中文 message 编码失败）；3.7+ reconfigure。"""
    for stream in (sys.stdout, sys.stderr):
        reconf = getattr(stream, "reconfigure", None)
        if reconf is not None:
            try:
                reconf(encoding="utf-8", errors="replace")
            except Exception:  # noqa: BLE001
                pass


def main(argv=None):
    _force_utf8_stdio()
    argv = list(sys.argv[1:] if argv is None else argv)
    try:
        schedule_md = os.environ.get("KIDPAL_SCHEDULE_MD", DEFAULT_SCHEDULE_MD)
        semantic_json = os.environ.get("KIDPAL_SEMANTIC_JSON", DEFAULT_SEMANTIC_JSON)
        skill_md = os.environ.get("KIDPAL_REMINDER_SEMANTIC_SKILL", DEFAULT_SKILL)
        parser_py = os.environ.get("KIDPAL_SCHEDULE_PARSER", DEFAULT_PARSER)
        lib_py = os.environ.get("KIDPAL_REMINDER_LIB", DEFAULT_LIB)
        margin_sec = _env_int("KIDPAL_SEMANTIC_GATE_MARGIN_SEC", DEFAULT_MARGIN_SEC)
        # argv 覆写（单测自由度；acceptance 只用 env）：schedule semantic skill parser lib
        if len(argv) >= 5:
            schedule_md, semantic_json, skill_md, parser_py, lib_py = argv[:5]
        result = compute(schedule_md, semantic_json, skill_md, parser_py, lib_py, margin_sec)
    except Exception as exc:  # noqa: BLE001 — fail-loud：异常 → exit≠0、无判决 JSON
        sys.stderr.write("reminder_semantic_gate: internal error: %r\n" % (exc,))
        return 2
    sys.stdout.write(json.dumps(result, ensure_ascii=False, separators=(",", ":")))
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
