#!/usr/bin/env python3
"""Safe patch generator / applier for the 261023 kidpal Honcho plugin declaration.

冻结契约（server/acceptance/test_261022_memory_isolation.acceptance.py 头注
FROZEN OFFLINE CONTRACT §B2）：**双形态安全化**，「点分写入器直写 openclaw.json」
路径已撤销。

  形态① patch 生成器：`--gen-patch <out> [--decl <p>]` —— 从声明工件生成
  `openclaw config patch` 消费的 patch 文件（JSON 子集形态，stdlib json 可解析；
  展平叶键集恰 = 声明 changes 的全部点分键）。离线可跑、零外部命令、不读 config。

  形态② 安全应用编排（advisory §9 序，任一步失败即停、失败序零写）：
    ⓪ 幂等预检：config 只读直判现状已达标 → 打印 SKIP、exit 0（零外部命令、
       零备份、零写）。
    ⓪' drift-guard：逐条核对 drift_guard，任一不中 → 打印 STOP、exit 3（零外部
       命令、零备份、零写）。现状锚 = P1 已应用态（tools.allow 五项在位；
       2026-10-25 随 261025-kidpal-tools-profile-fix 显式换代，drift_problems
       代码零改）。
    ① 写前时间戳备份：config 同目录、basename + ".bak-kidpal-honcho-" + UTC
       yyyymmddTHHMMSS、内容 = 原始 config；
    ② patch 生成 → `openclaw config patch --file <p> --dry-run --json`，必须
       checks.schema == true；
    ③ `openclaw plugins install @honcho-ai/openclaw-honcho --pin`（安装记录 pin
       版本；本次已验证版本 = 1.7.0）；
    ④ `openclaw plugins inspect openclaw-honcho --runtime`（Status: loaded +
       Version: 1.7.0 + Policy: allowConversationAccess: true）+
       `openclaw plugins doctor`；
    ⑤ 正式 `openclaw config patch --file <p>`（OpenClaw 自己的写入口，自带校验）；
    ⑥ `openclaw config validate`；
    ⑦ `systemctl restart openclaw-gateway`，静置后健康核对：`openclaw health`
       + `systemctl show -p NRestarts openclaw-gateway`（确认进程没在 5 秒级
       重启 = 崩循环报警）。任一步失败 → 非零退出、立即停。

**网关安全铁律（advisory §9）**：openclaw.json 只经 OpenClaw 自己的写入口写
（`config patch`），写前必 `--dry-run` schema 校验；**本脚本零「直写
openclaw.json」路径**——除备份副本与 patch 文件外不产生任何配置写入（K3 崩因 =
脚本直写绕过 zod strictObject 校验 → 网关 exit 78 崩循环）。

注入通道（离线桩真跑）：`--openclaw-bin <path>` —— **全部**外部副作用命令
（openclaw CLI 与 systemctl）经该注入桩执行，桩收到完整 argv（argv[0] =
"openclaw" 或 "systemctl"）；.py 结尾的桩经 sys.executable 调起（跨平台）。
未注入时回落到 PATH 上的真实 `openclaw` / `systemctl`（ECS 部署轮形态）。

CLI:
    python apply_honcho_plugin.py --config <openclaw.json> --decl <decl.json> \\
        [--openclaw-bin <path>] [--settle-seconds N]
    python apply_honcho_plugin.py --decl <decl.json> --gen-patch <out.patch.json>
默认 --config = /root/.openclaw/openclaw.json；默认 --decl = 本脚本目录下的
honcho-plugin-declaration.json（声明面 == 消费面）。

退出码：0 = 已应用 / 已达标 SKIP / patch 已生成；1 = 编排某一步失败（失败即停）；
2 = CLI 缺件或非法 JSON；3 = drift-guard 不中 STOP。

openclaw.json 含 gateway 凭据，整文件不入仓；纯 stdlib json，零额外依赖、
不依赖 jq；离线可用 sys.executable 对临时 fixture 真跑。
"""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
import tempfile
import time
from datetime import datetime, timezone
from pathlib import Path

DEFAULT_CONFIG = "/root/.openclaw/openclaw.json"
DECL_NAME = "honcho-plugin-declaration.json"
BACKUP_SUFFIX = ".bak-kidpal-honcho-"
PLUGIN_ID = "openclaw-honcho"
PLUGIN_PACKAGE = "@honcho-ai/openclaw-honcho"
PLUGIN_VERSION_PIN = "1.7.0"
PLUGIN_LOAD_LOG_LINE = "Honcho memory plugin loaded"
PLUGIN_POLICY_LINE = "Policy: allowConversationAccess: true"
GATEWAY_UNIT = "openclaw-gateway"
PATCH_NAME = "honcho-plugin.patch.json"

_MISSING = object()


# --------------------------------------------------------------------------- #
# 点分路径读写（声明消费面；只读 config / 构造 patch，不写 config）
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


def build_patch_tree(changes: list) -> dict:
    """声明 changes → `openclaw config patch` 消费的嵌套 patch 树。

    展平后的叶键集 = 声明点分键逐字（逐键单写，零额外键）。"""
    tree: dict = {}
    for change in changes:
        _json_set(tree, change["path"], _deep_copy(change["value"]))
    return tree


def target_state(config: dict, changes: list) -> dict:
    """config 现状叠加声明目标（只读判幂等用，绝不写回）。"""
    state = _deep_copy(config)
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


# --------------------------------------------------------------------------- #
# 外部命令唯一出口（openclaw CLI 与 systemctl 全经此；--openclaw-bin 注入桩）
# --------------------------------------------------------------------------- #

class StepError(RuntimeError):
    """编排某一步失败 —— 任一步失败即停（失败序零写）。"""


def _run_ext(argv, openclaw_bin):
    """执行一次外部副作用命令。argv[0] 恒为 "openclaw" 或 "systemctl"。"""
    if openclaw_bin:
        stub = str(openclaw_bin)
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


def _step(label, argv, openclaw_bin):
    print(f"[step] {label}: {' '.join(argv)}")
    proc = _run_ext(argv, openclaw_bin)
    for line in (proc.stdout or "").splitlines():
        print(f"    {line}")
    if proc.returncode != 0:
        for line in (proc.stderr or "").splitlines():
            print(f"    ! {line}", file=sys.stderr)
        raise StepError(
            f"{label} 失败（rc={proc.returncode}）—— 任一步失败即停，不继续下一"
            "步（advisory §9）。")
    return proc


def _schema_ok(stdout: str) -> bool:
    """config patch --dry-run --json 回执核 schema:true（可能是多行输出）。"""
    text = (stdout or "").strip()
    docs = []
    if text:
        try:
            docs.append(json.loads(text))
        except ValueError:
            for line in text.splitlines():
                try:
                    docs.append(json.loads(line))
                except ValueError:
                    continue
    for doc in docs:
        if isinstance(doc, dict):
            checks = doc.get("checks")
            if isinstance(checks, dict) and checks.get("schema") is True:
                return True
    return False


def _require_text(label, text, needle):
    if needle.lower() not in (text or "").lower():
        raise StepError(
            f"{label} 输出未含 {needle!r} —— 插件运行时证据不成立（接线未生效或"
            "版本 pin 不符）。")


# --------------------------------------------------------------------------- #
# 形态② 安全应用编排
# --------------------------------------------------------------------------- #

def orchestrate(config_path: Path, changes: list,
                openclaw_bin, settle_seconds: int) -> int:
    # ① 写前时间戳备份（config 同目录、内容 = 原始 config；不触 config 本体）
    ts = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S")
    backup_path = config_path.parent / (config_path.name + BACKUP_SUFFIX + ts)
    try:
        shutil.copyfile(config_path, backup_path)
    except OSError as e:
        print(f"FAILED: 写前备份失败（{backup_path}）: {e!r} —— 不进入编排。",
              file=sys.stderr)
        return 1
    print(f"BACKUP: {backup_path}")

    with tempfile.TemporaryDirectory(prefix="kidpal-honcho-patch-") as td:
        patch_path = Path(td) / PATCH_NAME
        patch_path.write_text(
            json.dumps(build_patch_tree(changes), ensure_ascii=False, indent=2)
            + "\n", encoding="utf-8")
        print(f"PATCH: {patch_path}")

        try:
            # ② dry-run：写前 schema 校验（openclaw.json 零写）
            dry = _step(
                "config patch --dry-run（schema 校验）",
                ["openclaw", "config", "patch", "--file", str(patch_path),
                 "--dry-run", "--json"],
                openclaw_bin)
            if not _schema_ok(dry.stdout):
                raise StepError(
                    "config patch --dry-run 未回 checks.schema = true —— 声明形状"
                    "被 schema 拒收风险（K3 崩因 ①），拒绝继续。")

            # ③ 先装插件（配置有条目而插件未装时 validate 会给 stale 警告）
            _step(
                "plugins install @honcho-ai/openclaw-honcho --pin",
                ["openclaw", "plugins", "install", PLUGIN_PACKAGE, "--pin"],
                openclaw_bin)

            # ④ 运行时体检：loaded + 版本 pin + 插件条目内 hooks 授权
            inspect = _step(
                "plugins inspect openclaw-honcho --runtime",
                ["openclaw", "plugins", "inspect", PLUGIN_ID, "--runtime"],
                openclaw_bin)
            _require_text("plugins inspect --runtime", inspect.stdout, "Status: loaded")
            _require_text("plugins inspect --runtime", inspect.stdout,
                          f"Version: {PLUGIN_VERSION_PIN}")
            _require_text("plugins inspect --runtime", inspect.stdout,
                          PLUGIN_POLICY_LINE)
            _step("plugins doctor", ["openclaw", "plugins", "doctor"], openclaw_bin)

            # ⑤ 正式 patch（OpenClaw 自己的写入口 —— 本脚本唯一允许的配置改动路径）
            _step(
                "config patch（正式写入）",
                ["openclaw", "config", "patch", "--file", str(patch_path)],
                openclaw_bin)

            # ⑥ 配置校验（不起网关，按 schema 校验当前配置）
            _step("config validate", ["openclaw", "config", "validate"],
                  openclaw_bin)

            # ⑦ 重启 + 健康/NRestarts 核对（5 秒级重启 = 崩循环报警）
            _step("systemctl restart openclaw-gateway",
                  ["systemctl", "restart", GATEWAY_UNIT], openclaw_bin)
            if settle_seconds and settle_seconds > 0:
                time.sleep(settle_seconds)
            _step("openclaw health（运行时隔离/回退）", ["openclaw", "health"],
                  openclaw_bin)
            restarts = _step(
                "systemctl show -p NRestarts（崩循环核对）",
                ["systemctl", "show", "-p", "NRestarts", GATEWAY_UNIT],
                openclaw_bin)
            m = re.search(r"NRestarts=(\d+)", restarts.stdout or "")
            if m and int(m.group(1)) > 0:
                raise StepError(
                    f"openclaw-gateway NRestarts={m.group(1)} > 0 —— 重启后进程又"
                    "在重启（崩循环）；按 advisory §9 回退。")
            if not m:
                print("    note: NRestarts 值不可解析（回执形态差异）—— 以网关日志"
                      "与 health 回执为准。")
        except StepError as e:
            print(f"FAILED: {e}", file=sys.stderr)
            print(f"  backup kept: {backup_path}", file=sys.stderr)
            return 1

    print(f"APPLIED -> {config_path}（经 OpenClaw 写入口 config patch；备份 "
          f"{backup_path.name}）")
    print(f"  部署轮核对：journalctl -u {GATEWAY_UNIT} 中应出现日志行 "
          f"'{PLUGIN_LOAD_LOG_LINE}'（上游插件加载证据）；三证采集见验收件 D 节。")
    return 0


# --------------------------------------------------------------------------- #
# CLI
# --------------------------------------------------------------------------- #

def _load_decl(decl_path: Path):
    if not decl_path.is_file():
        print(f"ERR: 声明工件不存在: {decl_path}", file=sys.stderr)
        return None
    try:
        decl = json.loads(decl_path.read_text(encoding="utf-8"))
    except ValueError as e:
        print(f"ERR: 声明工件非合法 JSON: {e!r}", file=sys.stderr)
        return None
    if not isinstance(decl, dict):
        print("ERR: 声明工件顶层须为 object。", file=sys.stderr)
        return None
    changes = decl.get("changes")
    if not isinstance(changes, list) or not changes or not all(
            isinstance(c, dict) and isinstance(c.get("path"), str)
            and c["path"].strip() and "value" in c for c in changes):
        print("ERR: 声明 changes 缺 path/value —— 无法定位目标键。", file=sys.stderr)
        return None
    return decl


def _force_utf8_stdio() -> None:
    """跨平台输出稳定：被捕获/重定向时也按 UTF-8 写字（Windows 默认 cp1252
    会因中文标签抛 UnicodeEncodeError；ECS UTF-8 环境下等价 no-op）。"""
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except (AttributeError, ValueError):
            pass


def main(argv=None) -> int:
    _force_utf8_stdio()
    ap = argparse.ArgumentParser(
        description="Safe patch generator / applier for the kidpal Honcho plugin "
                    "declaration（openclaw.json 只经 OpenClaw 写入口 config patch 写）。")
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="openclaw.json 路径（默认 %(default)s）")
    ap.add_argument("--decl", default=None,
                    help="声明工件路径（默认 = 本脚本目录下 %s）" % DECL_NAME)
    ap.add_argument("--gen-patch", default=None, dest="gen_patch",
                    help="形态①：只从声明生成 config patch 文件后退出（不读 config）")
    ap.add_argument("--openclaw-bin", default=None, dest="openclaw_bin",
                    help="外部副作用命令注入桩（openclaw CLI 与 systemctl 全经它；"
                         ".py 桩经 sys.executable 调起）")
    ap.add_argument("--settle-seconds", type=int, default=5,
                    dest="settle_seconds",
                    help="restart 后静置秒数再健康核对（默认 %(default)s）")
    args = ap.parse_args(argv)

    decl_path = Path(args.decl) if args.decl else (
        Path(__file__).resolve().parent / DECL_NAME)
    decl = _load_decl(decl_path)
    if decl is None:
        return 2
    changes = decl["changes"]
    drift_guard = decl.get("drift_guard") or []

    # 形态①：patch 生成器（零 config 依赖、零外部命令、零备份）
    if args.gen_patch:
        out = Path(args.gen_patch)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(
            json.dumps(build_patch_tree(changes), ensure_ascii=False, indent=2)
            + "\n", encoding="utf-8")
        print(f"PATCH: {out}")
        for change in changes:
            print(f"  + {change['path']}")
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

    # ⓪ 幂等预检（先于任何外部命令 / 备份 / 写）
    if target_state(config, changes) == config:
        print("SKIP: config already at declared target state (idempotent, "
              "no write, no backup, no external command).")
        return 0

    # ⓪' drift-guard（先于任何外部命令 / 备份 / 写）
    problems = drift_problems(config, drift_guard)
    if problems:
        print("STOP: drift-guard mismatch — refusing to apply "
              "(STOP-on-mismatch; no write).", file=sys.stderr)
        for p in problems:
            print(f"  - {p}", file=sys.stderr)
        print(f"  config: {config_path}", file=sys.stderr)
        return 3

    return orchestrate(config_path, changes, args.openclaw_bin,
                       args.settle_seconds)


if __name__ == "__main__":
    sys.exit(main())
