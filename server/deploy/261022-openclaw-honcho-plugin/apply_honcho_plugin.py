#!/usr/bin/env python3
"""Idempotent applier for the 261022 kidpal Honcho plugin declaration.

冻结契约（server/acceptance/test_261022_memory_isolation.acceptance.py 头注
FROZEN OFFLINE CONTRACT §B）：语义序（261017 PV-A6 / K1-E4 同形制）——
  ① 幂等优先：config 现状已等于全部声明目标 → 打印 SKIP、exit 0（不写、不备份）；
  ② drift-guard：逐条核对 drift_guard，任一不中 → 打印 STOP、exit 3（不写、不备份）
    —— 本包 drift_guard 现状锚 = P1 已应用态（tools.allow 四项在位），P1 漂移
    时必须 STOP 拒绝叠加，而非整文件改写；
  ③ 写前时间戳备份：config 同目录、basename + ".bak-..." + UTC yyyymmddTHHMMSS、
    内容 = 原始 config；
  ④ 应用：逐条把 value 写到 path 点分键（每条只改自己的键），成功 exit 0。

CLI:
    python apply_honcho_plugin.py --config <openclaw.json> --decl <decl.json>
默认 --config = /root/.openclaw/openclaw.json；默认 --decl = 本脚本目录下的
honcho-plugin-declaration.json（声明面 == 消费面）。

openclaw.json 含 gateway 凭据，整文件不入仓；本脚本只改声明列出的点分键
（hooks.allowConversationAccess + agents.entries.kidpal.plugins.honcho），其余键
（gateway / P1 tools.allow 四项 / 261017 skills 白名单 / 其它 agent 条目）原样
保留——双声明式 patch 组合（P1 先、P3 后）、互不覆盖。纯 stdlib json，零额外
依赖、不依赖 jq；离线可用 sys.executable 对临时 fixture 真跑。
"""

from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime, timezone
from pathlib import Path

DEFAULT_CONFIG = "/root/.openclaw/openclaw.json"
DECL_NAME = "honcho-plugin-declaration.json"
BACKUP_SUFFIX = ".bak-kidpal-honcho-"

_MISSING = object()


# --------------------------------------------------------------------------- #
# 点分路径读写（openclaw.json 形状）
# --------------------------------------------------------------------------- #

def _json_get(obj, dotted: str):
    cur = obj
    for part in dotted.split("."):
        if not isinstance(cur, dict) or part not in cur:
            return _MISSING
        cur = cur[part]
    return cur


def _json_set(obj: dict, dotted: str, value) -> None:
    parts = dotted.split(".")
    cur = obj
    for p in parts[:-1]:
        nxt = cur.get(p)
        if not isinstance(nxt, dict):
            nxt = {}
            cur[p] = nxt
        cur = nxt
    cur[parts[-1]] = value


def _deep_copy(value):
    return json.loads(json.dumps(value, ensure_ascii=False))


def apply_changes(state: dict, changes: list) -> dict:
    """逐条把 change.value 写到 change.path 点分键（每条只改自己的键）。"""
    for change in changes:
        _json_set(state, change["path"], _deep_copy(change["value"]))
    return state


def drift_problems(config: dict, drift_guard: list) -> list:
    """逐条核对 drift_guard；返回不中的条目描述（空 = 全部命中）。

    expect == "unset" 判键缺席，否则 json 相等判。"""
    problems: list = []
    for i, entry in enumerate(drift_guard):
        if not isinstance(entry, dict):
            problems.append(f"drift_guard[{i}] 不是 object")
            continue
        path = entry.get("path", "")
        expect = entry.get("expect", "")
        cur = _json_get(config, path)
        if expect == "unset":
            if cur is not _MISSING:
                problems.append(
                    f"drift_guard[{i}] {path}: 期望键缺席（'unset'），实得 {cur!r}")
        else:
            if cur is _MISSING:
                problems.append(
                    f"drift_guard[{i}] {path}: 期望 {expect!r}，实得键缺席")
            elif cur != expect:
                problems.append(
                    f"drift_guard[{i}] {path}: 期望 {expect!r}，实得 {cur!r}")
    return problems


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(
        description="Idempotent apply of the kidpal Honcho plugin declaration.")
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="openclaw.json 路径（默认 %(default)s）")
    ap.add_argument("--decl", default=None,
                    help="声明工件路径（默认 = 本脚本目录下 %s）" % DECL_NAME)
    args = ap.parse_args(argv)

    config_path = Path(args.config)
    decl_path = Path(args.decl) if args.decl else (Path(__file__).resolve().parent / DECL_NAME)

    if not config_path.is_file():
        print(f"ERR: config 不存在: {config_path}", file=sys.stderr)
        return 2
    if not decl_path.is_file():
        print(f"ERR: 声明工件不存在: {decl_path}", file=sys.stderr)
        return 2
    try:
        config = json.loads(config_path.read_text(encoding="utf-8"))
    except ValueError as e:
        print(f"ERR: config 非合法 JSON: {e!r}", file=sys.stderr)
        return 2
    try:
        decl = json.loads(decl_path.read_text(encoding="utf-8"))
    except ValueError as e:
        print(f"ERR: 声明工件非合法 JSON: {e!r}", file=sys.stderr)
        return 2

    changes = decl.get("changes") or []
    drift_guard = decl.get("drift_guard") or []
    if not isinstance(changes, list) or not changes or not all(
            isinstance(c, dict) and isinstance(c.get("path"), str) and c["path"].strip()
            and "value" in c for c in changes):
        print("ERR: 声明 changes 缺 path/value —— 无法定位目标键。", file=sys.stderr)
        return 2

    # ① 幂等优先：现状已等于全部声明目标 → SKIP（不写、不备份）
    target = apply_changes(_deep_copy(config), changes)
    if target == config:
        print("SKIP: already at declared target state (idempotent).")
        return 0

    # ② drift-guard：任一不中 → STOP，exit 3，不写、不备份
    #   （现状锚 = P1 已应用态：tools.allow 四项在位；P1 漂移即拒绝叠加。）
    problems = drift_problems(config, drift_guard)
    if problems:
        print("STOP: drift-guard mismatch — refusing to apply "
              "(STOP-on-mismatch; no write).", file=sys.stderr)
        for p in problems:
            print(f"  - {p}", file=sys.stderr)
        print(f"  config: {config_path}", file=sys.stderr)
        return 3

    # ③ 写前时间戳备份（落 config 同目录；内容 = 原始 config）
    ts = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S")
    backup_path = config_path.parent / (config_path.name + BACKUP_SUFFIX + ts)
    backup_path.write_text(config_path.read_text(encoding="utf-8"), encoding="utf-8")

    # ④ 应用（逐条定点写自己的键，其余键原样保留）
    apply_changes(config, changes)
    config_path.write_text(json.dumps(config, ensure_ascii=False, indent=2),
                           encoding="utf-8")
    print(f"APPLIED case={decl.get('case')!r} -> {config_path}")
    for change in changes:
        print(f"  set {change['path']}")
    print(f"  backup: {backup_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
