#!/usr/bin/env python3
"""KidPal 261031-openclaw-cron-trigger-gating — cron trigger 幂等 apply / clear / status。

写入口只许 openclaw CLI（`openclaw cron edit --trigger-script` / `--clear-trigger`）；
本脚本**不直写 openclaw.json**（不 open 写模式、不 write_text 任何配置件）。trigger
内容来源 = 仓内部署后的 `/opt/kidpal/openclaw-patch/triggers/*.trigger.js`，逐字读入
后交给 CLI —— 与 `verify-deploy.sh` 的内联文本比对共用同一份源。

用法：
  python3 cron_triggers.py status                  # 逐 job 比对 live trigger 与盘上文件
  python3 cron_triggers.py apply [--dry-run]       # 幂等加/更新 trigger
  python3 cron_triggers.py apply --job kidpal-schedule-health
  python3 cron_triggers.py clear [--dry-run]       # 一步回滚：--clear-trigger
  python3 cron_triggers.py clear --job kidpal-reminder-semantic

退出码：0 = 全绿 / apply-clear 成功；1 = 有漂移或命令失败；2 = 用法或本地文件缺失。
"""

import argparse
import json
import subprocess
import sys
from pathlib import Path

DEFAULT_TRIGGER_DIR = "/opt/kidpal/openclaw-patch/triggers"
# job 名 → trigger 文件名（仓内/ECS 落点同构）
JOBS = {
    "kidpal-reminder-semantic": "reminder-semantic.trigger.js",
    "kidpal-schedule-health": "schedule-health.trigger.js",
}


def _read_trigger(path):
    return Path(path).read_text(encoding="utf-8")


def _norm(text):
    return (text or "").replace("\r\n", "\n").replace("\r", "\n").strip()


def _show(name):
    """openclaw cron show <name> --json → dict；失败返回 None。"""
    proc = subprocess.run(
        ["openclaw", "cron", "show", name, "--json"],
        capture_output=True, text=True, timeout=120,
    )
    if proc.returncode != 0:
        sys.stderr.write("openclaw cron show 失败: %s\n" % (proc.stderr or "").strip())
        return None
    try:
        return json.loads(proc.stdout)
    except ValueError:
        sys.stderr.write("openclaw cron show 输出非 JSON\n")
        return None


def _inline_script(payload):
    trig = payload.get("trigger") if isinstance(payload, dict) else None
    if isinstance(trig, dict):
        script = trig.get("script")
        if isinstance(script, str):
            return script
    return ""


def _run(cmd, dry_run):
    display = " ".join(
        (c if len(c) <= 40 else c[:40] + "…") for c in cmd)
    print(("[dry-run] " if dry_run else "+ ") + display)
    if dry_run:
        return 0
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=180)
    if proc.stdout.strip():
        print(proc.stdout.strip())
    if proc.returncode != 0:
        sys.stderr.write("命令失败 rc=%d: %s\n" % (proc.returncode, (proc.stderr or "").strip()))
    return proc.returncode


def _targets(args):
    if args.job:
        if args.job not in JOBS:
            sys.stderr.write("未知 job：%s（可选：%s）\n" % (args.job, ", ".join(JOBS)))
            raise SystemExit(2)
        return [args.job]
    return list(JOBS)


def cmd_status(args):
    drift = 0
    for name in _targets(args):
        src = Path(args.trigger_dir) / JOBS[name]
        payload = _show(name)
        if payload is None:
            drift += 1
            continue
        live = _inline_script(payload)
        if not src.is_file():
            print("DRIFT %s: 盘上 trigger 文件缺失 %s" % (name, src))
            drift += 1
            continue
        want = _norm(_read_trigger(str(src)))
        got = _norm(live)
        once = (payload.get("trigger") or {}).get("once") if isinstance(payload.get("trigger"), dict) else None
        if not got:
            print("DRIFT %s: live 无 trigger.script（需 cron edit --trigger-script）" % name)
            drift += 1
        elif got != want:
            print("DRIFT %s: live trigger 内联文本与盘上 %s 不一致" % (name, src))
            drift += 1
        elif once is True:
            print("DRIFT %s: trigger.once=true（必须 false）" % name)
            drift += 1
        else:
            print("OK %s: trigger 内联文本与 %s 一致（once=%r）" % (name, src, once))
    return 1 if drift else 0


def cmd_apply(args):
    rc = 0
    for name in _targets(args):
        src = Path(args.trigger_dir) / JOBS[name]
        if not src.is_file():
            sys.stderr.write("本地 trigger 文件缺失：%s\n" % src)
            return 2
        script = _read_trigger(str(src))
        rc |= _run(
            ["openclaw", "cron", "edit", name, "--trigger-script", script],
            args.dry_run,
        )
    return 0 if rc == 0 else 1


def cmd_clear(args):
    rc = 0
    for name in _targets(args):
        rc |= _run(["openclaw", "cron", "edit", name, "--clear-trigger"], args.dry_run)
    return 0 if rc == 0 else 1


def build_parser():
    parser = argparse.ArgumentParser(
        description="KidPal cron trigger 幂等 apply/clear/status（写入口 = openclaw CLI）")
    parser.add_argument("--trigger-dir", default=DEFAULT_TRIGGER_DIR,
                        help="trigger 文件目录（默认 %s）" % DEFAULT_TRIGGER_DIR)
    sub = parser.add_subparsers(dest="command")

    def _common(p):
        p.add_argument("--job", choices=sorted(JOBS), default=None,
                       help="只操作单个 job（缺省 = 两个都处理）")
        p.add_argument("--dry-run", action="store_true",
                       help="只打印将执行的命令，不真正调用 openclaw")

    p_status = sub.add_parser("status", help="比对 live trigger 与盘上文件")
    p_status.set_defaults(func=cmd_status)

    p_apply = sub.add_parser("apply", help="幂等加/更新 trigger（--trigger-script）")
    _common(p_apply)
    p_apply.set_defaults(func=cmd_apply)

    p_clear = sub.add_parser("clear", help="一步回滚（--clear-trigger）")
    _common(p_clear)
    p_clear.set_defaults(func=cmd_clear)
    return parser


def _force_utf8_stdio():
    """控制台/重定向面强制 UTF-8（Windows cp1252 会因中文 help/输出编码失败）；3.7+ reconfigure。"""
    for stream in (sys.stdout, sys.stderr):
        reconf = getattr(stream, "reconfigure", None)
        if reconf is not None:
            try:
                reconf(encoding="utf-8", errors="replace")
            except Exception:  # noqa: BLE001
                pass


def main(argv=None):
    _force_utf8_stdio()
    parser = build_parser()
    args = parser.parse_args(argv)
    if not getattr(args, "func", None):
        parser.print_help()
        return 2
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
