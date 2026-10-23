#!/usr/bin/env python3
"""One-step rollback for the 261023 kidpal Honcho plugin apply.

冻结契约（server/acceptance/test_261022_memory_isolation.acceptance.py 头注
FROZEN OFFLINE CONTRACT §B3）：CLI = --config / --backup（默认取 config 旁最新
声明 backup.pattern 匹配件）/ --dry-run（只打印意图、exit 0、不写）。非 dry-run
时，把 backup 内容还原到 config，exit 0。

三级回滚序申报（advisory §8「回滚方式」，由低危到彻底；任一级可用）：

  ① `openclaw plugins disable openclaw-honcho` + `systemctl restart
     openclaw-gateway` —— **低危首选**：配置留着、插件不加载，可随时再
     `plugins enable` 恢复；先做这一级确认崩循环是否真由插件引起。
  ② 还原时间戳备份 openclaw.json.bak-kidpal-honcho-*（**本脚本承载**）：把
     配置退回 apply 之前的状态（回滚声明键，保留 backup 副本供复查）。
  ③ `openclaw plugins uninstall openclaw-honcho` —— 连 npm project 一起清
     （彻底卸载；npm 项目目录随之删除，重装 = 重新 `plugins install --pin`）。

本脚本只做第 ② 级（一步还原备份）；第 ①/③ 级经上面 OpenClaw CLI 原生命令手工
执行（它们不经脚本直写 openclaw.json——openclaw.json 只经 OpenClaw 自己的写入
口改，advisory §9 铁律）。
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

DEFAULT_CONFIG = "/root/.openclaw/openclaw.json"
DECL_NAME = "honcho-plugin-declaration.json"
FALLBACK_PATTERN = "openclaw.json.bak-kidpal-honcho-*"


def _decl_pattern() -> str:
    """读本脚本目录声明工件的 backup.pattern；缺失/非法时回落内置同形 pattern。"""
    decl_path = Path(__file__).resolve().parent / DECL_NAME
    try:
        decl = json.loads(decl_path.read_text(encoding="utf-8"))
        pattern = (decl.get("backup") or {}).get("pattern")
        if isinstance(pattern, str) and pattern.strip():
            return pattern
    except (OSError, ValueError):
        pass
    return FALLBACK_PATTERN


def latest_backup(config_path: Path, pattern: str):
    """取 config 旁最新的声明 pattern 匹配备份（文件名含 UTC 时间戳，字典序单调）。"""
    candidates = sorted(config_path.parent.glob(pattern))
    return candidates[-1] if candidates else None


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(
        description="One-step rollback of the kidpal Honcho plugin apply.")
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="openclaw.json 路径（默认 %(default)s）")
    ap.add_argument("--backup", default=None,
                    help="备份文件路径（默认 = config 旁最新声明 pattern 匹配件）")
    ap.add_argument("--dry-run", action="store_true",
                    help="只打印意图，不写")
    args = ap.parse_args(argv)

    config_path = Path(args.config)
    backup_path = Path(args.backup) if args.backup else latest_backup(
        config_path, _decl_pattern())

    if backup_path is None or not backup_path.is_file():
        print(f"ERR: 找不到备份文件（config={config_path}, backup={backup_path}）",
              file=sys.stderr)
        return 2

    if args.dry_run:
        print(f"[dry-run] would restore {backup_path} -> {config_path} (no write).")
        return 0

    config_path.write_text(backup_path.read_text(encoding="utf-8"), encoding="utf-8")
    print(f"RESTORED {config_path} <- {backup_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
