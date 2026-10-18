#!/usr/bin/env python3
"""One-step rollback for the 261017 kidpal-agent skills allowlist apply.

冻结契约（server/acceptance/test_261017_openclaw_kidpal_skills.acceptance.py 头注
FROZEN OFFLINE CONTRACT §3）：CLI = --config / --backup（默认取 config 旁最新
声明 backup.pattern 匹配件）/ --dry-run（只打印意图、exit 0、不写）。非 dry-run
时，把 backup 内容还原到 config，exit 0。
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

DEFAULT_CONFIG = "/root/.openclaw/openclaw.json"
DECL_NAME = "kidpal-agent-skills-declaration.json"
FALLBACK_PATTERN = "openclaw.json.bak-kidpal-skills-*"


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
        description="One-step rollback of the kidpal-agent skills allowlist apply.")
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
