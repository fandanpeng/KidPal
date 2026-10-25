#!/usr/bin/env python3
"""One-step rollback for the 261025 memory-core dreaming-disable apply.

冻结契约（server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py
头注 FROZEN OFFLINE CONTRACT §3）：CLI = --config / --backup（默认取 config 旁
最新声明 backup.pattern 匹配件）/ --dry-run（只打印意图、exit 0、不写、不重启、
零外部命令）/ 注入桩（--systemctl-bin，或与 apply 同形的 --openclaw-bin 单桩兼收
openclaw 与 systemctl 两类 argv）。

非 dry-run 序（validate 在前、restart 在后）：
  ① 整档还原 backup 内容到 config（内容 == apply 前的时间戳快照）；
  ② `openclaw config validate` —— 还原后先校验；
  ③ 通过才 `systemctl restart openclaw-gateway`；validate 失败则不重启并报错
     （exit 非 0；网关仍跑内存态是其接受形态）。

**语义区分（K3 崩因教训，务必读）**：本脚本做的是「还原 apply 之前的时间戳
备份快照」——那份快照是 apply 之前被网关正常加载过的**已知合法快照**，整档还原
只是把文件退回一个**合法快照**，不构造新配置内容。与之相对，**禁脚本直写 /
拼装新目标态**：脚本自己拼装或改写配置内容去「绕过 zod 校验」是 K3 崩因
（2026-09 实测：直写绕过 zod strictObject 校验 → 网关 exit 78 崩循环）。本脚本
零这种路径——apply 的配置改动只经 OpenClaw 自己的写入口 config patch。

openclaw.json 含 gateway 凭据，整文件不入仓；纯 stdlib（json），不依赖 jq。
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

DEFAULT_CONFIG = "/root/.openclaw/openclaw.json"
DECL_NAME = "dreaming-disable-declaration.json"
FALLBACK_PATTERN = "openclaw.json.bak-kidpal-dreaming-*"
GATEWAY_UNIT = "openclaw-gateway"


class StepError(RuntimeError):
    """回滚某一步失败 —— 任一步失败即停。"""


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


def _run_ext(argv, stub_bin):
    """执行一次外部命令。argv[0] 恒为 "openclaw" 或 "systemctl"；.py 桩经
    sys.executable 调起（跨平台）。未注入时回落 PATH 上的真实命令。"""
    if stub_bin:
        stub = str(stub_bin)
        if stub.lower().endswith(".py"):
            cmd = [sys.executable, stub, *argv]
        else:
            cmd = [stub, *argv]
    else:
        cmd = list(argv)
    try:
        return subprocess.run(
            cmd, capture_output=True, text=True, encoding="utf-8",
            errors="replace", timeout=300)
    except OSError as e:
        raise StepError(f"无法执行 {' '.join(argv)}: {e!r}") from e


def _step(label, argv, stub_bin):
    print(f"[step] {label}: {' '.join(argv)}")
    proc = _run_ext(argv, stub_bin)
    for line in (proc.stdout or "").splitlines():
        print(f"    {line}")
    if proc.returncode != 0:
        for line in (proc.stderr or "").splitlines():
            print(f"    ! {line}", file=sys.stderr)
        raise StepError(f"{label} 失败（rc={proc.returncode}）—— 失败即停。")
    return proc


def _force_utf8_stdio() -> None:
    """跨平台输出稳定：被捕获/重定向时也按 UTF-8 写字。"""
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except (AttributeError, ValueError):
            pass


def main(argv=None) -> int:
    _force_utf8_stdio()
    ap = argparse.ArgumentParser(
        description="One-step rollback of the 261025 memory-core "
                    "dreaming-disable apply（整档还原 time-stamped backup + "
                    "config validate 通过才 restart）。")
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="openclaw.json 路径（默认 %(default)s）")
    ap.add_argument("--backup", default=None,
                    help="备份文件路径（默认 = config 旁最新声明 pattern 匹配件）")
    ap.add_argument("--dry-run", action="store_true",
                    help="只打印意图，不写、不重启、零外部命令")
    ap.add_argument("--systemctl-bin", default=None, dest="stub_bin",
                    help="外部命令注入桩（restart 与 config validate 全经它；"
                         ".py 桩经 sys.executable 调起）")
    ap.add_argument("--openclaw-bin", default=None, dest="stub_bin",
                    help="同 --systemctl-bin（姊妹 apply 同形单桩兼收两类 argv）")
    args = ap.parse_args(argv)

    config_path = Path(args.config)
    backup_path = Path(args.backup) if args.backup else latest_backup(
        config_path, _decl_pattern())

    if backup_path is None or not backup_path.is_file():
        print(f"ERR: 找不到备份文件（config={config_path}, backup={backup_path}）",
              file=sys.stderr)
        return 2

    if args.dry_run:
        print(f"[dry-run] would restore {backup_path} -> {config_path} "
              "(no write, no restart, no external command).")
        return 0

    # ① 整档还原已知合法快照（apply 前的时间戳备份）
    try:
        config_path.write_text(backup_path.read_text(encoding="utf-8"),
                               encoding="utf-8")
    except OSError as e:
        print(f"FAILED: 还原备份失败（{backup_path} -> {config_path}）: {e!r}",
              file=sys.stderr)
        return 1
    print(f"RESTORED {config_path} <- {backup_path}")

    try:
        # ② 还原后先校验（不起网关；validate 失败则不重启）
        _step("config validate", ["openclaw", "config", "validate"], args.stub_bin)
        # ③ 校验通过才重启网关
        _step("systemctl restart openclaw-gateway",
              ["systemctl", "restart", GATEWAY_UNIT], args.stub_bin)
    except StepError as e:
        print(f"FAILED: {e}", file=sys.stderr)
        print("  网关未重启（config validate 未过则不 restart —— 网关仍跑内存态）。",
              file=sys.stderr)
        return 1

    print(f"ROLLED BACK -> {config_path}（已知合法快照还原 + config validate 通过"
          f" + systemctl restart {GATEWAY_UNIT}）")
    return 0


if __name__ == "__main__":
    sys.exit(main())
