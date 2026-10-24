#!/usr/bin/env python3
"""Applier / patch generator for the 261022 kidpal-agent tools allowlist declaration.

冻结契约（server/acceptance/test_261022_openclaw_kidpal_tools.acceptance.py 头注
FROZEN OFFLINE CONTRACT §2）：**双形态**。

  形态① 直写应用（语义序 = ①幂等优先 ②drift-guard（不中 → exit 3）③写前时间戳
  备份 ④应用（只改声明的那一个点分路径键））。幂等 SKIP 先于 drift-guard：目标态
  命中即 no-op，不经基线判定（升级基准只管「从哪来」，不管「到哪去」）。

  形态② patch 生成器 `--gen-patch <out>`（261023 批 B apply_honcho_plugin.py 先例）：
  从声明生成 `openclaw config patch` 消费的 patch 文件（嵌套树形态，stdlib json
  可解析；展平叶键集恰 = 一个叶键 {change.path: change.allow}）。**不读 config、
  不写 config、零外部命令、零备份**——config 路径不存在也照常生成。

**网关安全铁律（K3 崩因教训）**：openclaw.json 只经 OpenClaw 自己的写入口写
（`openclaw config patch --dry-run` 校验后 `--file` 落）；脚本直写会绕过 zod
strictObject 校验 → 网关 exit 78 崩循环（261022 K3 实测崩因）。live 应用首选
形态②；形态① 保留为离线核对与既有回退路径。

CLI:
    python apply_agent_tools.py --config <openclaw.json> --decl <decl.json>
    python apply_agent_tools.py [--decl <decl.json>] --gen-patch <out.patch.json>
默认 --config = /root/.openclaw/openclaw.json；默认 --decl = 本脚本目录下的
kidpal-agent-tools-declaration.json（声明面 == 消费面）。

退出码：0 = 已应用 / 已达标 SKIP / patch 已生成；2 = 缺件（直写形态缺 config、
两形态缺声明）或非法 JSON；3 = drift-guard 不中 STOP（零写零备份）。

openclaw.json 含 gateway 凭据，整文件不入仓；本脚本只读改写
agents.entries.kidpal.tools.allow 一个键，其余键（含 gateway / 其它 agent 条目 /
261017 skills 白名单）原样保留。纯 stdlib（json），不依赖 jq；离线可用
sys.executable 对临时 fixture 真跑。
"""

from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime, timezone
from pathlib import Path

DEFAULT_CONFIG = "/root/.openclaw/openclaw.json"
DECL_NAME = "kidpal-agent-tools-declaration.json"
BACKUP_SUFFIX = ".bak-kidpal-tools-"

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


def apply_change(state: dict, change: dict) -> dict:
    """按 change 声明施加变更，就地改写 state 并返回它（只动 change.path 一个键）。"""
    _json_set(state, change["path"], list(change["allow"]))
    return state


def build_patch_tree(change: dict) -> dict:
    """声明 change → `openclaw config patch` 消费的嵌套 patch 树。

    展平后的叶键集 = 声明点分键逐字（恰一个叶键，零额外键）。"""
    tree: dict = {}
    _json_set(tree, change["path"], list(change["allow"]))
    return tree


# --------------------------------------------------------------------------- #
# drift-guard（expect | expect_any 双形，恰含其一）
# --------------------------------------------------------------------------- #

def _alt_hits(cur, alt) -> bool:
    """单个基线候选是否命中：'unset' 判键缺席，其余（object / list / 标量）判值
    相等。键在场但值不等 → 不中（fail-closed，不按缺席放行）。"""
    if alt == "unset":
        return cur is _MISSING
    return cur is not _MISSING and cur == alt


def _describe_alts(alts: list) -> str:
    return " 或 ".join(repr(a) for a in alts)


def drift_problems(config: dict, drift_guard: list) -> list:
    """逐条核对 drift_guard；返回不中的条目描述（空 = 全部命中）。

    每项须恰含 expect 或 expect_any 之一：
      · expect     : 单值候选（"unset" 判键缺席，否则判值相等）。
      · expect_any : 非空候选列表，任一命中即通过（升级基准：新机态 unset 与
                     live 既有态覆写并存，二者取一即合法起点）。"""
    problems: list = []
    for i, entry in enumerate(drift_guard):
        if not isinstance(entry, dict):
            problems.append(f"drift_guard[{i}] 不是 object")
            continue
        path = entry.get("path", "")
        if not isinstance(path, str) or not path.strip():
            problems.append(f"drift_guard[{i}] 缺非空点分 path")
            continue
        has_expect = "expect" in entry
        has_any = "expect_any" in entry
        if has_expect == has_any:
            problems.append(
                f"drift_guard[{i}] {path}: 须恰含 expect 或 expect_any 之一"
                f"（实得 expect={has_expect} / expect_any={has_any}）")
            continue
        if has_expect:
            alts = [entry.get("expect")]
        else:
            alts = entry.get("expect_any")
            if not isinstance(alts, list) or not alts:
                problems.append(
                    f"drift_guard[{i}] {path}: expect_any 须为非空候选列表，"
                    f"实得 {alts!r}")
                continue
        cur = _json_get(config, path)
        if any(_alt_hits(cur, alt) for alt in alts):
            continue
        got = "键缺席" if cur is _MISSING else repr(cur)
        problems.append(
            f"drift_guard[{i}] {path}: 期望 {_describe_alts(alts)}，实得 {got}")
    return problems


def _force_utf8_stdio() -> None:
    """跨平台输出稳定：被捕获/重定向时也按 UTF-8 写字（Windows 默认 cp1252
    会因中文标签抛 UnicodeEncodeError；ECS UTF-8 环境下等价 no-op）。"""
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except (AttributeError, ValueError):
            pass


# --------------------------------------------------------------------------- #
# CLI
# --------------------------------------------------------------------------- #

def main(argv=None) -> int:
    _force_utf8_stdio()
    ap = argparse.ArgumentParser(
        description="Applier / patch generator for the kidpal-agent tools allowlist "
                    "declaration.")
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="openclaw.json 路径（默认 %(default)s；--gen-patch 不读它）")
    ap.add_argument("--decl", default=None,
                    help="声明工件路径（默认 = 本脚本目录下 %s）" % DECL_NAME)
    ap.add_argument("--gen-patch", default=None, dest="gen_patch",
                    help="形态②：只从声明生成 config patch 文件后退出（不读 config、"
                         "不写 config、零备份）")
    args = ap.parse_args(argv)

    decl_path = Path(args.decl) if args.decl else (
        Path(__file__).resolve().parent / DECL_NAME)
    if not decl_path.is_file():
        print(f"ERR: 声明工件不存在: {decl_path}", file=sys.stderr)
        return 2
    try:
        decl = json.loads(decl_path.read_text(encoding="utf-8"))
    except ValueError as e:
        print(f"ERR: 声明工件非合法 JSON: {e!r}", file=sys.stderr)
        return 2
    if not isinstance(decl, dict):
        print("ERR: 声明工件顶层须为 object。", file=sys.stderr)
        return 2

    change = decl.get("change") or {}
    drift_guard = decl.get("drift_guard") or []
    if not isinstance(change, dict) or not change.get("path") \
            or not isinstance(change.get("allow"), list):
        print("ERR: 声明 change 缺 path/allow —— 无法定位目标键。", file=sys.stderr)
        return 2

    # 形态② patch 生成器（零 config 依赖、零外部命令、零备份）
    if args.gen_patch:
        out = Path(args.gen_patch)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(
            json.dumps(build_patch_tree(change), ensure_ascii=False, indent=2) + "\n",
            encoding="utf-8")
        print(f"PATCH: {out}")
        print(f"  + {change['path']} = {change['allow']}")
        print("  live 应用：openclaw config patch --file <patch> --dry-run --json "
              "→ 校验通过后去掉 --dry-run 正式落盘。")
        return 0

    config_path = Path(args.config)
    if not config_path.is_file():
        print(f"ERR: config 不存在: {config_path}", file=sys.stderr)
        return 2
    try:
        config = json.loads(config_path.read_text(encoding="utf-8"))
    except ValueError as e:
        print(f"ERR: config 非合法 JSON: {e!r}", file=sys.stderr)
        return 2

    # ① 幂等优先：现状已等于声明目标 → SKIP（不写、不备份、不经 drift-guard）
    target = apply_change(json.loads(json.dumps(config)), change)
    if target == config:
        print("SKIP: already at declared target state (idempotent).")
        return 0

    # ② drift-guard：任一不中 → STOP，exit 3，不写、不备份
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

    # ④ 应用（只改 change.path 一个键）
    apply_change(config, change)
    config_path.write_text(json.dumps(config, ensure_ascii=False, indent=2),
                           encoding="utf-8")
    print(f"APPLIED case={decl.get('case')!r} -> {config_path}")
    print(f"  allow: {change['allow']}")
    print(f"  backup: {backup_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
