#!/usr/bin/env python3
"""One-step rollback for the PERF-K1 model/thinking declaration apply.

冻结契约（FROZEN OFFLINE CONTRACT §3）：CLI = --config / --backup（默认取 config
旁最新 .bak-perf-k1-*）/ --dry-run（只打印意图、exit 0、不写）。非 dry-run 时，
把 backup 内容还原到 config，exit 0。
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

DEFAULT_CONFIG = "/root/.openclaw/openclaw.json"
BACKUP_SUFFIX = ".bak-perf-k1-"


def latest_backup(config_path: Path):
    """取 config 旁最新的 .bak-perf-k1-* 备份（按文件名排序，时间戳同格式单调）。"""
    pattern = config_path.name + BACKUP_SUFFIX + "*"
    candidates = sorted(config_path.parent.glob(pattern))
    return candidates[-1] if candidates else None


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(
        description="One-step rollback of the PERF-K1 declaration apply.")
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="openclaw.json 路径（默认 %(default)s）")
    ap.add_argument("--backup", default=None,
                    help="备份文件路径（默认 = config 旁最新 %s*）" % BACKUP_SUFFIX)
    ap.add_argument("--dry-run", action="store_true",
                    help="只打印意图，不写")
    args = ap.parse_args(argv)

    config_path = Path(args.config)
    backup_path = Path(args.backup) if args.backup else latest_backup(config_path)

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