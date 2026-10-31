#!/usr/bin/env python3
"""KidPal 261031-openclaw-cron-trigger-gating — cron trigger 幂等 apply / clear / status。

写入口只许 openclaw CLI（`openclaw cron edit --trigger-script` / `--clear-trigger`）；
本脚本**不直写 openclaw.json**（不 open 写模式、不 write_text 任何配置件）。trigger
来源 = 仓内部署后的 `/opt/kidpal/openclaw-patch/triggers/*.trigger.js`；`apply` 把
**盘上文件路径**交给 `--trigger-script`，由 CLI 自己读取并内联进 job（2026-10-31 ECS
实测：`--trigger-script` 收的是路径，内联文本会被当成路径打开而 ENOENT）。`status`
仍逐字读入同一份文件与 live 内联文本比对 —— 与 `verify-deploy.sh` 共用同一份源。

`openclaw cron show <name>` 认名字，但 `openclaw cron edit` **只认 automation id**（2026-10-31
ECS 实测：传名字报 `Automation not found`）。故 `apply` / `clear` 先跑 `openclaw cron list
--json`（顶层数组，元素含 `id` / `name`）解析 name→id；名字找不到即 fail-loud 退出，不静默跳过。

用法：
  python3 cron_triggers.py status                  # 逐 job 比对 live trigger 与盘上文件
  python3 cron_triggers.py apply [--dry-run]       # 幂等加/更新 trigger
  python3 cron_triggers.py apply --job kidpal-schedule-health
  python3 cron_triggers.py clear [--dry-run]       # 一步回滚：--clear-trigger
  python3 cron_triggers.py clear --job kidpal-reminder-semantic

退出码：0 = 全绿 / apply-clear 成功；1 = 有漂移或命令失败；
        2 = 用法或本地文件缺失 / `cron list` 无法解析目标 job。
"""

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

DEFAULT_TRIGGER_DIR = "/opt/kidpal/openclaw-patch/triggers"
_UUID_RE = re.compile(
    r"^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-"
    r"[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$")
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


def _cron_list():
    """openclaw cron list --json → [(id, name), ...]；失败返回 None。

    2026-10-31 ECS 实测形态：顶层数组，元素含 `id` / `name`（disabled 的 job 也列出）。
    """
    proc = subprocess.run(
        ["openclaw", "cron", "list", "--json"],
        capture_output=True, text=True, timeout=120,
    )
    if proc.returncode != 0:
        sys.stderr.write("openclaw cron list 失败: %s\n" % (proc.stderr or "").strip())
        return None
    try:
        doc = json.loads(proc.stdout)
    except ValueError:
        sys.stderr.write("openclaw cron list 输出非 JSON\n")
        return None
    if isinstance(doc, dict):
        # 容忍旧版包一层对象的形态
        for key in ("jobs", "items", "automations", "data"):
            if isinstance(doc.get(key), list):
                doc = doc[key]
                break
        else:
            doc = []
    if not isinstance(doc, list):
        sys.stderr.write("openclaw cron list 输出结构未知（期望顶层数组）\n")
        return None
    listing = []
    for item in doc:
        if not isinstance(item, dict):
            continue
        jid = item.get("id") or item.get("jobId") or item.get("job_id")
        jname = item.get("name")
        if isinstance(jid, str) and isinstance(jname, str):
            listing.append((jid, jname))
    return listing


def _resolve_target(name, listing):
    """name → automation id（cron edit 只认 id）；本身是 UUID 则原样返回；找不到返回 None。"""
    if _UUID_RE.match(name):
        return name
    for jid, jname in listing or ():
        if jname == name:
            return jid
    return None


def _resolve_targets(names):
    """names → [(name, id), ...]；任一名字在 cron list 里找不到即 fail-loud 返回 None。"""
    listing = _cron_list()
    if listing is None:
        return None
    resolved, missing = [], []
    for name in names:
        target = _resolve_target(name, listing)
        if target is None:
            missing.append(name)
        else:
            resolved.append((name, target))
    if missing:
        have = ", ".join(n for _, n in listing) or "（列表为空）"
        sys.stderr.write(
            "openclaw cron list 未找到 job：%s；现有 job：%s\n" % (", ".join(missing), have))
        return None
    return resolved


def _abbrev(arg, head=28, tail=28):
    """超长参数保留首尾（打印路径形态时尾部的文件名不会被截掉）。"""
    if len(arg) <= head + tail + 1:
        return arg
    return arg[:head] + "…" + arg[-tail:]


def _run(cmd, dry_run):
    display = " ".join(_abbrev(c) for c in cmd)
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
    names = _targets(args)
    # 先做本地前置检查：缺文件时不必触碰 openclaw（也让本地无 CLI 的 dry-run 报因清晰）。
    sources = {}
    for name in names:
        src = Path(args.trigger_dir) / JOBS[name]
        if not src.is_file():
            sys.stderr.write("本地 trigger 文件缺失：%s\n" % src)
            return 2
        sources[name] = src
    resolved = _resolve_targets(names)
    if resolved is None:
        return 2
    rc = 0
    for name, target in resolved:
        # --trigger-script 收的是文件路径（CLI 自己读取并内联进 job），不是脚本文本；
        # cron edit 的目标只认 automation id（name→id 已由 _resolve_targets 解析）。
        rc |= _run(
            ["openclaw", "cron", "edit", target, "--trigger-script", str(sources[name])],
            args.dry_run,
        )
    return 0 if rc == 0 else 1


def cmd_clear(args):
    resolved = _resolve_targets(_targets(args))
    if resolved is None:
        return 2
    rc = 0
    for _name, target in resolved:
        rc |= _run(["openclaw", "cron", "edit", target, "--clear-trigger"], args.dry_run)
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
                       help="只打印将执行的命令（仅只读 cron list 解析 id），不执行任何写操作")

    p_status = sub.add_parser("status", help="比对 live trigger 与盘上文件")
    # --job 与 apply/clear 对齐：status 缺它会让 _targets() 读 args.job 时 AttributeError
    # （2026-10-31 ECS 实测：`status` 直接崩在 Namespace 无 job 属性）。
    p_status.add_argument("--job", choices=sorted(JOBS), default=None,
                          help="只比对单个 job（缺省 = 两个都比对）")
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
