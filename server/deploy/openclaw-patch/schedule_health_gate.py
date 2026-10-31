#!/usr/bin/env python3
"""KidPal 261031-openclaw-cron-trigger-gating — 判定器 B（课表解析健康巡检静默门）。

纯只读、无网络、不写盘；stdout 恒为**严格单行 JSON** `{fire: bool, message?, state?}`。
正常 exit 0；内部异常（含数值型 env 传非数字）exit≠0 且不输出任何半截判决 JSON。

消费方 = `schedule-health.trigger.js` 薄壳（OpenClaw cron `trigger.script`），每 15 分钟
无模型跑一次：无告警件、或同一坏指纹已告知 → fire:false（不起 agent 回合）；新坏指纹 /
需恢复动作 → fire:true（payload 按 kidpal-schedule-health skill 四分支执行）。

输入（env，均带 ECS 默认值）：
  KIDPAL_PARSE_ERROR_JSON        front 写的告警件（{checked_at, error, detail}）
  KIDPAL_PARSE_HEALTH_JSON       ack 状态件（{last_notified_fingerprint, last_notified_at}）
  KIDPAL_HEALTH_STALE_SEC        新鲜阈值（默认 180s）
  KIDPAL_HEALTH_FIRE_COOLDOWN_SEC 防刷屏冷却窗（默认 3600s）
  KIDPAL_TRIGGER_STATE_JSON      可选：上一轮冻结的 trigger.state 快照（JSON 串）

指纹 fingerprint = error + "|" + detail（与 skill 逐字一致）。
新鲜 = checked_at 距 now <= stale_sec；checked_at 不可解析 → 退回文件 mtime。

真值（四分支，按序命中即止）：
  1. 告警件不在场：ack 不在 → false；ack 在场 → true（恢复动作，payload 清 ack）；
  2. 告警件在场但不新鲜：同 1；
  3. 新鲜且指纹 == ack 指纹 → false；
  4. 新鲜且指纹不同（或 ack 缺失/损坏）→ true（message 自包含：指纹 + detail + 指明分支 4）；
  5. 告警件在场但不可解析 → true（宁可让 agent 看一眼也不静默）。

防刷屏保险丝：state.lastFire={fingerprint, atMs}；同指纹且在冷却窗内 → false。
**state 只作反刷屏与观测，不是去重权威**——去重权威仍是 ack 件（payload 单写）；
state 绝不携带 ack 去重键（last_notified_fingerprint / last_notified_at）。

python 3.6 兼容（ECS 有 3.6 解释器先例；本脚本用 venv 3.11 跑，但保持 3.6-safe 写法）。
"""

import json
import os
import sys
from datetime import datetime, timezone

DEFAULT_ERROR_JSON = "/opt/kidpal/alerts/schedule-parse-error.json"
DEFAULT_HEALTH_JSON = "/opt/kidpal/alerts/schedule-parse-health.json"
DEFAULT_STALE_SEC = 180
DEFAULT_COOLDOWN_SEC = 3600


def _env_int(name, default):
    """读数值型 env；显式传入非数字 → ValueError（fail-loud，禁静默回落默认值）。"""
    raw = os.environ.get(name)
    if raw is None or not str(raw).strip():
        return default
    return int(str(raw).strip())  # 非数字在此抛 ValueError


def _parse_iso(value):
    """ISO8601 → aware UTC datetime；支持 `Z` 后缀与 `+00:00` 偏移；不可解析 → None。"""
    if not isinstance(value, str) or not value.strip():
        return None
    text = value.strip()
    if text.endswith("Z"):
        text = text[:-1] + "+00:00"
    dt = None
    # datetime.fromisoformat 是 3.7+；3.6 上退回 strptime 形态（保持 3.6-safe）
    if hasattr(datetime, "fromisoformat"):
        try:
            dt = datetime.fromisoformat(text)
        except ValueError:
            dt = None
    if dt is None:
        for fmt in ("%Y-%m-%dT%H:%M:%S%z", "%Y-%m-%dT%H:%M:%S.%f%z",
                    "%Y-%m-%d %H:%M:%S%z"):
            try:
                dt = datetime.strptime(text, fmt)
                break
            except ValueError:
                dt = None
    if dt is None:
        return None
    if dt.tzinfo is None:
        dt = dt.replace(tzinfo=timezone.utc)
    return dt.astimezone(timezone.utc)


def _load_json(path):
    """读 JSON；缺失/损坏 → None（调用方按在场性另行判定）。"""
    try:
        with open(path, "r", encoding="utf-8") as fh:
            return json.load(fh)
    except (OSError, ValueError):
        return None


def _ack_fingerprint(health_path):
    if not os.path.exists(health_path):
        return None
    doc = _load_json(health_path)
    if not isinstance(doc, dict):
        return None
    fp = doc.get("last_notified_fingerprint")
    return fp if isinstance(fp, str) else None


def _last_fire(state):
    if isinstance(state, dict):
        lf = state.get("lastFire")
        if isinstance(lf, dict):
            return lf
    return None


def _recovery_message(reason):
    return (
        "课表解析告警件%s，但 ack 状态件仍在：请按 kidpal-schedule-health skill "
        "恢复分支清理 schedule-parse-health.json，不打扰家长。" % reason
    )


def compute(error_path, health_path, stale_sec, cooldown_sec, state=None, now=None):
    """纯函数真值表（单测直接调；env 读取在 main）。返回 {fire, message?, state?}。"""
    now = now or datetime.now(timezone.utc)
    if now.tzinfo is None:
        now = now.replace(tzinfo=timezone.utc)
    now = now.astimezone(timezone.utc)
    now_ms = int(now.timestamp() * 1000)
    ack_fp = _ack_fingerprint(health_path)

    # 分支 1：告警件不在场
    if not os.path.exists(error_path):
        if ack_fp is not None:
            return {"fire": True, "message": _recovery_message("已不在场（解析已恢复）")}
        return {"fire": False}

    # 分支 5：告警件在场但不可解析
    doc = _load_json(error_path)
    if not isinstance(doc, dict):
        return {
            "fire": True,
            "message": (
                "课表解析告警件在场但无法解析（invalid JSON）：请按 kidpal-schedule-health "
                "skill 边界处理、先核对告警件形态，不打扰家长。"
            ),
        }

    checked_at = _parse_iso(doc.get("checked_at"))
    if checked_at is None:
        try:
            checked_at = datetime.fromtimestamp(os.path.getmtime(error_path), timezone.utc)
        except OSError:
            checked_at = None
    fresh = checked_at is not None and (now - checked_at).total_seconds() <= stale_sec

    # 分支 2：告警件在场但不新鲜（已恢复等价）
    if not fresh:
        if ack_fp is not None:
            return {"fire": True, "message": _recovery_message("已不新鲜（解析已恢复）")}
        return {"fire": False}

    error_text = doc.get("error") if isinstance(doc.get("error"), str) else ""
    detail_text = doc.get("detail") if isinstance(doc.get("detail"), str) else ""
    fp = error_text + "|" + detail_text

    # 分支 3：新鲜且指纹 == ack 指纹（去重权威 = ack 件）
    if ack_fp is not None and ack_fp == fp:
        return {"fire": False}

    # 防刷屏保险丝：同指纹在冷却窗内且上轮 fire 成功 → 抑制（非去重权威）
    last = _last_fire(state)
    if last is not None and last.get("fingerprint") == fp:
        at_ms = last.get("atMs")
        if isinstance(at_ms, (int, float)) and not isinstance(at_ms, bool):
            if now_ms - at_ms < cooldown_sec * 1000:
                return {
                    "fire": False,
                    "state": {"lastFire": {"fingerprint": fp, "atMs": int(at_ms)}},
                }

    # 分支 4：新鲜且指纹不同（或 ack 缺失/损坏）→ fire
    message = (
        "课表解析失败出现新坏指纹：%s —— 详情：%s。"
        "请按 kidpal-schedule-health skill 分支 4 经 QQ 告知家长坏 token 与正确格式，"
        "并在告知后写 ack 状态件。" % (fp, detail_text)
    )
    return {
        "fire": True,
        "message": message,
        "state": {"lastFire": {"fingerprint": fp, "atMs": now_ms}},
    }


def _parse_state_env():
    raw = os.environ.get("KIDPAL_TRIGGER_STATE_JSON")
    if raw is None or not str(raw).strip():
        return None
    try:
        doc = json.loads(raw)
    except ValueError:
        return None  # 快照损坏只影响反刷屏，不影响正确性 → best-effort 忽略
    return doc if isinstance(doc, dict) else None


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
        error_path = os.environ.get("KIDPAL_PARSE_ERROR_JSON", DEFAULT_ERROR_JSON)
        health_path = os.environ.get("KIDPAL_PARSE_HEALTH_JSON", DEFAULT_HEALTH_JSON)
        stale_sec = _env_int("KIDPAL_HEALTH_STALE_SEC", DEFAULT_STALE_SEC)
        cooldown_sec = _env_int("KIDPAL_HEALTH_FIRE_COOLDOWN_SEC", DEFAULT_COOLDOWN_SEC)
        state = _parse_state_env()
        # argv 覆写（单测自由度；acceptance 只用 env）：error health
        if len(argv) >= 2:
            error_path, health_path = argv[0], argv[1]
        result = compute(error_path, health_path, stale_sec, cooldown_sec, state)
    except Exception as exc:  # noqa: BLE001 — fail-loud：异常 → exit≠0、无判决 JSON
        sys.stderr.write("schedule_health_gate: internal error: %r\n" % (exc,))
        return 2
    sys.stdout.write(json.dumps(result, ensure_ascii=False, separators=(",", ":")))
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
