"""KidPal 261031-openclaw-cron-trigger-gating — cron_triggers.py 行为单测。

覆盖 2026-10-31 ECS live 实测暴露的三个缺陷（静态锚抓不到）：

1. `apply` 曾把 trigger **文件内容**传给 `openclaw cron edit --trigger-script`，但 CLI 的
   该参数收的是**文件路径**（自己读取后内联进 job），真实 live 报 ENOENT。本测断言 argv
   里该值等于盘上触发文件路径，且 argv 不含 JS 源码片段。
2. `status` 子命令曾未定义 `--job`，`_targets()` 读 `args.job` 直接 AttributeError 崩。
   本测覆盖「全量 targets」与「指定 --job」两种用法。
3. `cron edit` 只认 automation id（名字 → `Automation not found`）。本测断言 `apply` /
   `clear` 先经 `cron list --json` 解析 name→id，且 edit/clear 的第一个位置参数是 **id**
   而不是 name；名字找不到时非零退出并报出该名字。

驱动方式：在临时目录造一个假 `openclaw` 可执行文件（记录 argv、对 `cron show` 回灌
trigger JSON、对 `cron list` 回灌含 id/name 的顶层数组），PATH 前置后**子进程真跑**
cron_triggers.py —— 断言的是真实 argv 形态，不是源码字面量。纯只读、不碰 /opt、不联网。

Windows 注：CreateProcess 解析裸命令名只认 PE，故假 CLI 用 `sys.executable` 的副本
命名为 `openclaw.exe`，脚本体落在同名目录的 `cron` 文件里（python 把 argv[1] 当脚本名）。

  python -m pytest -q server/tests/test_cron_triggers_units.py
"""

from __future__ import annotations

import importlib.util
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
CRON_TRIGGERS = SERVER / "deploy" / "openclaw-patch" / "cron_triggers.py"

JOBS = (
    ("kidpal-reminder-semantic", "reminder-semantic.trigger.js"),
    ("kidpal-schedule-health", "schedule-health.trigger.js"),
)

# 假 openclaw 回灌的 automation id（形态取自 ECS `cron list --json` 实测）
IDS = {
    "kidpal-reminder-semantic": "26c6c771-c83e-4883-9b44-3a742023e481",
    "kidpal-schedule-health": "5b1d2e3f-4a6b-4c7d-8e9f-0123456789ab",
}

BODY_A = (
    "const res = await exec({ command: \"/opt/kidpal/venv/bin/python3 gate_a.py\" });\n"
    "const raw = String(res?.aggregated ?? res?.text ?? \"\");\n"
    "json({ fire: true });\n"
)
BODY_B = (
    "const res = await exec({ command: \"/opt/kidpal/venv/bin/python3 gate_b.py\" });\n"
    "json({ fire: false });\n"
)

# 假 openclaw：记录每次调用的 argv；`cron show` 回灌 trigger JSON、`cron list` 回灌 job 数组。
_SHIM = '''import json
import os
import sys


def _main(args):
    log = os.environ.get("FAKE_OPENCLAW_LOG")
    if log:
        with open(log, "a", encoding="utf-8") as fh:
            fh.write(json.dumps(sys.argv) + "\\n")
    if args and args[0] == "list":
        try:
            ids = json.loads(os.environ.get("FAKE_OPENCLAW_IDS") or "{}")
        except ValueError:
            ids = {}
        jobs = [{"id": jid, "name": name, "agentId": "main"} for name, jid in ids.items()]
        sys.stdout.write(json.dumps(jobs))
    elif args and args[0] == "show" and len(args) > 1:
        try:
            mapping = json.loads(os.environ.get("FAKE_OPENCLAW_MAP") or "{}")
        except ValueError:
            mapping = {}
        script = ""
        path = mapping.get(args[1])
        if path and os.path.isfile(path):
            with open(path, encoding="utf-8") as fh:
                script = fh.read()
        sys.stdout.write(json.dumps({"trigger": {"script": script, "once": False}}))
    return 0


if __name__ == "__main__":
    _args = sys.argv[1:]
    if _args and _args[0] == "cron":
        _args = _args[1:]
    sys.exit(_main(_args))
'''


class CronTriggersUnits(unittest.TestCase):

    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp(prefix="kpt-cron-triggers-"))
        self.trig_dir = self.tmp / "triggers"
        self.trig_dir.mkdir()
        for (_, fname), body in zip(JOBS, (BODY_A, BODY_B)):
            (self.trig_dir / fname).write_text(body, encoding="utf-8", newline="\n")
        self.log = self.tmp / "openclaw-argv.log"
        self._install_fake_openclaw()
        self._old_path = os.environ.get("PATH", "")
        os.environ["PATH"] = str(self.tmp) + os.pathsep + self._old_path

    def tearDown(self):
        os.environ["PATH"] = self._old_path
        # Windows：刚跑过的假 CLI（python.exe 副本）偶发被杀软短暂占用，重试几拍再放弃。
        for _ in range(10):
            try:
                shutil.rmtree(str(self.tmp))
                return
            except FileNotFoundError:
                return
            except PermissionError:
                time.sleep(0.2)
        shutil.rmtree(str(self.tmp), ignore_errors=True)

    # ── 假 CLI 安装 ────────────────────────────────────────────────────── #
    def _install_fake_openclaw(self):
        if sys.platform == "win32":
            self.assertTrue(sys.executable, "sys.executable 为空，无法造假 CLI")
            shutil.copyfile(sys.executable, str(self.tmp / "openclaw.exe"))
            # python 把 argv[1] 当脚本名：cron_triggers 调的是 `openclaw cron ...`
            (self.tmp / "cron").write_text(_SHIM, encoding="utf-8", newline="\n")
        else:
            shim = self.tmp / "shim.py"
            shim.write_text(_SHIM, encoding="utf-8", newline="\n")
            wrapper = self.tmp / "openclaw"
            wrapper.write_text(
                '#!/bin/sh\nexec "%s" "%s" "$@"\n' % (sys.executable, shim),
                encoding="utf-8", newline="\n")
            wrapper.chmod(0o755)

    # ── 驱动真脚本 ─────────────────────────────────────────────────────── #
    def _run(self, *args, ids=None):
        env = dict(os.environ)
        env["FAKE_OPENCLAW_LOG"] = str(self.log)
        env["FAKE_OPENCLAW_MAP"] = json.dumps(
            {name: str(self.trig_dir / fname) for name, fname in JOBS})
        env["FAKE_OPENCLAW_IDS"] = json.dumps(IDS if ids is None else ids)
        return subprocess.run(
            [sys.executable, str(CRON_TRIGGERS),
             "--trigger-dir", str(self.trig_dir), *args],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            cwd=str(self.tmp), env=env, timeout=180,
        )

    def _calls(self):
        if not self.log.is_file():
            return []
        calls = []
        for line in self.log.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if not line:
                continue
            argv = json.loads(line)
            # POSIX: [openclaw 路径, "cron", ...]；Windows: ["cron", ...]（脚本名）
            try:
                calls.append(argv[argv.index("cron"):])
            except ValueError:
                calls.append(argv)
        return calls

    def _edits(self):
        return [c for c in self._calls() if len(c) > 1 and c[1] == "edit"]

    # ── 缺陷 1：apply 传路径不传内容；缺陷 3：edit 目标必须是 id ───────── #
    def test_apply_passes_trigger_file_path_and_automation_id(self):
        proc = self._run("apply")
        self.assertEqual(proc.returncode, 0, proc.stderr)
        calls = self._calls()
        self.assertEqual(calls[0], ["cron", "list", "--json"], calls)
        edits = self._edits()
        self.assertEqual(len(edits), 2, edits)
        for call, (name, fname) in zip(edits, JOBS):
            self.assertEqual(call[:3], ["cron", "edit", IDS[name]], call)
            self.assertNotEqual(call[2], name, "edit 目标必须是 automation id，不能是 name")
            self.assertEqual(call[3], "--trigger-script", call)
            self.assertEqual(call[4], str(self.trig_dir / fname), call)
            self.assertTrue(os.path.isfile(call[4]), call[4])
            joined = " ".join(call)
            self.assertNotIn("await exec", joined)
            self.assertNotIn("res?.aggregated", joined)

    def test_apply_dry_run_previews_id_and_path_without_writes(self):
        proc = self._run("apply", "--dry-run")
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertEqual(self._edits(), [], "apply --dry-run 不得调用 cron edit")
        self.assertTrue(
            all(c[1] == "list" for c in self._calls()),
            "apply --dry-run 只允许只读 cron list：%r" % (self._calls(),))
        self.assertIn("[dry-run]", proc.stdout)
        for name, fname in JOBS:
            self.assertIn(IDS[name], proc.stdout, proc.stdout)
            self.assertIn(fname, proc.stdout, proc.stdout)
        self.assertNotIn("await exec", proc.stdout)
        self.assertNotIn("res?.aggregated", proc.stdout)

    # ── 缺陷 2：status 两种用法都不崩 ──────────────────────────────────── #
    def test_status_all_targets_ok(self):
        proc = self._run("status")
        self.assertEqual(proc.returncode, 0, proc.stderr + proc.stdout)
        self.assertIn("OK kidpal-reminder-semantic", proc.stdout)
        self.assertIn("OK kidpal-schedule-health", proc.stdout)
        self.assertNotIn("Traceback", proc.stderr)
        self.assertNotIn("AttributeError", proc.stderr)

    def test_status_single_job_ok(self):
        proc = self._run("status", "--job", "kidpal-schedule-health")
        self.assertEqual(proc.returncode, 0, proc.stderr + proc.stdout)
        self.assertIn("OK kidpal-schedule-health", proc.stdout)
        self.assertNotIn("kidpal-reminder-semantic", proc.stdout)
        self.assertNotIn("Traceback", proc.stderr)
        self.assertNotIn("AttributeError", proc.stderr)

    # ── clear 走 --clear-trigger，目标同样是 id ────────────────────────── #
    def test_clear_uses_clear_trigger_flag_and_automation_id(self):
        proc = self._run("clear")
        self.assertEqual(proc.returncode, 0, proc.stderr)
        edits = self._edits()
        self.assertEqual(len(edits), 2, edits)
        for call, (name, _) in zip(edits, JOBS):
            self.assertEqual(call[:3], ["cron", "edit", IDS[name]], call)
            self.assertEqual(call[-1], "--clear-trigger", call)

    # ── 缺陷 3 补：名字找不到必须 fail-loud ────────────────────────────── #
    def test_missing_job_name_fails_loud(self):
        only_health = {"kidpal-schedule-health": IDS["kidpal-schedule-health"]}
        proc = self._run("apply", ids=only_health)
        self.assertNotEqual(proc.returncode, 0, proc.stdout)
        self.assertIn("kidpal-reminder-semantic", proc.stderr)
        self.assertEqual(self._edits(), [], "名字未找到时不得对任何 job 执行 edit")

    def test_resolve_target_passes_uuid_through(self):
        spec = importlib.util.spec_from_file_location(
            "kidpal_cron_triggers_ut", CRON_TRIGGERS)
        self.assertIsNotNone(spec)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        uuid = IDS["kidpal-reminder-semantic"]
        listing = [(jid, name) for name, jid in IDS.items()]
        self.assertEqual(module._resolve_target(uuid, []), uuid)
        self.assertEqual(
            module._resolve_target("kidpal-schedule-health", listing),
            IDS["kidpal-schedule-health"])
        self.assertIsNone(module._resolve_target("no-such-job", listing))


if __name__ == "__main__":
    unittest.main()
