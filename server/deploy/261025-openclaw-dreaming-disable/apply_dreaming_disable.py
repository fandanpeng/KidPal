#!/usr/bin/env python3
"""Safe patch generator / applier for the 261025 memory-core dreaming-disable
declaration.

冻结契约（server/acceptance/test_261025_openclaw_dreaming_disable.acceptance.py
头注 FROZEN OFFLINE CONTRACT §2）：**双形态**，骨架承 261023 批 B
apply_honcho_plugin.py；本包差异 = memory-core 为 OpenClaw **bundled** 插件，
故**零 plugins install / inspect / doctor 三步**（那三步只服务 npm 第三方插件
安装与加载核对，architect 1g 裁决 ③）。

  形态① patch 生成器：`--decl <decl> --gen-patch <out>` —— 从声明工件生成
  `openclaw config patch` 消费的 patch 文件（嵌套树 / JSON 子集，stdlib json
  可解析；展平叶键集恰 1 个 = 声明 change 逐字）。**不读 config、不写 config、
  零外部命令、零备份**——config 路径不存在也照常生成。

  形态② 安全应用编排（SKIP 先于 drift，architect 1g 裁决 ②；照
  apply_honcho_plugin.py；与 apply_agent_tools.py 的「联合幂等」**有意不同**，
  禁当不一致修掉——本包写目标仅 dreaming 一叶，工具面锚是前置基线而非写目标）：
    ⓪ 幂等预检：config 现状已达声明目标（dreaming 已 false）→ 打印 SKIP、
       exit 0，零写零备份零外部命令。SKIP 分支若工具面基线漂移，把
       drift_problems 逐条打印为 **WARN**（stdout，exit 0 不变）——避免「dreaming
       已关 + 工具面回退」完全静默，运维仍能看到组合态。
    ⓪' drift-guard：逐条核对 drift_guard，任一不中 → 打印 STOP、exit 3，零写
       零备份**零外部命令**。判定面复制 apply_agent_tools.py 的 drift_problems
       （`_MISSING` / `_MALFORMED` 双哨兵 + expect / expect_any 双形；父键在场
       但非对象 = 真漂移，不得按「键缺席」放行）。布尔叶另加严格判：候选为
       false 时只有现状 `is False` 才算命中（Python 下 `0 == False`，`==` 比较
       会放过 0 / 畸形值，architect 1g 裁决 ①）。
    ① 写前时间戳备份：config 同目录、basename + ".bak-kidpal-dreaming-" + UTC
       yyyymmddTHHMMSS、内容 = 原始 config（整档快照，供一步回滚）。
    ② patch 生成 → `openclaw config patch --file <p> --dry-run --json`
       （**首个外部命令**；写前 schema 校验，回执须 checks.schema === true）；
    ③ 正式 `openclaw config patch --file <p>`（**无 --dry-run**；OpenClaw 自有
       写入口 = 唯一允许的配置改动路径）；
    ④ `openclaw config validate`；
    ⑤ `systemctl restart openclaw-gateway`；
    ⑥ `openclaw health` → 其后附 `systemctl show -p NRestarts openclaw-gateway`
       崩循环核对（NRestarts > 0 = 重启后进程又起（崩循环），K3 exit 78 教训）。
    任一步失败 → 非零退出、立即停、其后步骤零调用、config 全程不变。

**网关安全铁律（K3 崩因教训）**：openclaw.json 只经 OpenClaw 自己的写入口写
（`config patch --dry-run` 校验后 `--file` 落）；**本脚本零「直写 openclaw.json」
路径**——除时间戳备份副本与临时 patch 文件外不产生任何配置写入（K3 崩因 =
脚本直写绕过 zod strictObject 校验 → 网关 exit 78 崩循环）。

注入通道（离线桩真跑）：`--openclaw-bin <path>` —— **全部**外部副作用命令
（openclaw CLI 与 systemctl）经该注入桩执行，桩收到完整 argv（argv[0] =
"openclaw" 或 "systemctl"）；.py 结尾的桩经 sys.executable 调起（跨平台）。
未注入时回落到 PATH 上的真实 `openclaw` / `systemctl`（ECS 部署轮形态）。

CLI:
    python apply_dreaming_disable.py --config <openclaw.json> \\
        --decl <decl.json> [--openclaw-bin <path>] [--settle-seconds N]
    python apply_dreaming_disable.py --decl <decl.json> --gen-patch <out.patch.json>
默认 --config = /root/.openclaw/openclaw.json；默认 --decl = 本脚本目录下的
dreaming-disable-declaration.json（声明面 == 消费面）。

退出码：0 = 已应用 / 已达标 SKIP / patch 已生成；1 = 编排某一步失败（失败即停）；
2 = CLI 缺件或非法 JSON；3 = drift-guard 不中 STOP。

openclaw.json 含 gateway 凭据，整文件不入仓；纯 stdlib（json / re / shutil），
零额外依赖、不依赖 jq；离线可用 sys.executable 对临时 fixture 真跑。
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
DECL_NAME = "dreaming-disable-declaration.json"
BACKUP_SUFFIX = ".bak-kidpal-dreaming-"
GATEWAY_UNIT = "openclaw-gateway"
PATCH_NAME = "dreaming-disable.patch.json"

_MISSING = object()    # 键缺席（unset）
_MALFORMED = object()  # 父键在场但非对象 —— 与「键缺席」严格区分（fail-closed）


# --------------------------------------------------------------------------- #
# 点分路径读写 / 展平（openclaw.json 形状；只读 config / 构造 patch，不写 config）
# --------------------------------------------------------------------------- #

def _json_get(obj, dotted: str):
    """点分读。返回叶值 / _MISSING（键缺席）/ _MALFORMED（父键在场但非对象）。

    两个哨兵**不得参与任何 `==` 相等判**——调用方须先按 `is` 判定，否则
    「父键非对象」会被错当「键缺席」按 unset 放行并静默覆盖畸形配置。"""
    cur = obj
    for part in dotted.split("."):
        if not isinstance(cur, dict):
            return _MALFORMED
        if part not in cur:
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
    """声明 value 深拷贝（apply 不得共享声明对象引用）。"""
    return json.loads(json.dumps(value, ensure_ascii=False))


def build_patch_tree(changes: list) -> dict:
    """声明 changes → `openclaw config patch` 消费的嵌套 patch 树。

    展平后的叶键集 = 声明点分键逐字（恰 1 个叶键，零额外键）。"""
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


# --------------------------------------------------------------------------- #
# drift-guard（expect | expect_any 双形，恰含其一；复制 apply_agent_tools.py）
# --------------------------------------------------------------------------- #

def _alt_hits(cur, alt) -> bool:
    """单个基线候选是否命中：'unset' 判**键缺席**（_MALFORMED 不算缺席——父键
    在场但非对象是真漂移，须 fail-closed）；候选为布尔 false 时只认现状
    `is False`（防 `0 == False` 放行畸形值）；其余（标量 / list）判值相等。
    键在场但形状 / 值不符 → 不中。"""
    if alt == "unset":
        return cur is _MISSING
    if alt is False:
        return cur is False
    return cur is not _MISSING and cur is not _MALFORMED and cur == alt


def _describe_alts(alts: list) -> str:
    return " 或 ".join(repr(a) for a in alts)


def drift_problems(config: dict, drift_guard: list) -> list:
    """逐条核对 drift_guard；返回不中的条目描述（空 = 全部命中）。

    每项须恰含 expect 或 expect_any 之一：
      · expect     : 单值候选（"unset" 判键缺席，否则判值相等）。
      · expect_any : 非空候选列表，任一命中即通过（升级基准：fresh 缺席 /
                     目标态并存，取一即合法起点）。"""
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
        if cur is _MISSING:
            got = "键缺席"
        elif cur is _MALFORMED:
            got = "父键在场但非对象（malformed，非键缺席）"
        else:
            got = repr(cur)
        problems.append(
            f"drift_guard[{i}] {path}: 期望 {_describe_alts(alts)}，实得 {got}")
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
            "步；config 只经 OpenClaw 写入口改，失败序零写。")
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

    with tempfile.TemporaryDirectory(prefix="kidpal-dreaming-patch-") as td:
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

            # ③ 正式 patch（OpenClaw 自己的写入口 —— 本脚本唯一允许的配置改动路径）
            _step(
                "config patch（正式写入）",
                ["openclaw", "config", "patch", "--file", str(patch_path)],
                openclaw_bin)

            # ④ 配置校验（不起网关，按 schema 校验当前配置）
            _step("config validate", ["openclaw", "config", "validate"],
                  openclaw_bin)

            # ⑤ 重启 + ⑥ 健康 / NRestarts 核对（5 秒级重启 = 崩循环报警）
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
                    "在重启（崩循环）；回滚（rollback_dreaming_disable.py）。")
            if not m:
                print("    note: NRestarts 值不可解析（回执形态差异）—— 以网关日志"
                      "与 health 回执为准。")
        except StepError as e:
            print(f"FAILED: {e}", file=sys.stderr)
            print(f"  backup kept: {backup_path}", file=sys.stderr)
            return 1

    print(f"APPLIED -> {config_path}（经 OpenClaw 写入口 config patch；备份 "
          f"{backup_path.name}）")
    print("  部署轮核对：patch 前后 openclaw config get plugins.entries.memory-core "
          "比对，差异只允许 dreaming.enabled（合并语义仓内无取证）。")
    return 0


# --------------------------------------------------------------------------- #
# CLI
# --------------------------------------------------------------------------- #

def _validate_changes(decl: dict) -> tuple:
    """声明写入面校验：返回 (changes, None) 或 (None, 错误文案)。"""
    changes = decl.get("changes")
    if not isinstance(changes, list) or not changes:
        return None, ("声明 changes 缺 path/value —— 写入面须为 changes "
                      "[{path, value}]（恰一个 dreaming 叶键）。")
    for i, change in enumerate(changes):
        if (not isinstance(change, dict)
                or not isinstance(change.get("path"), str)
                or not change["path"].strip()
                or "value" not in change):
            return None, (f"声明 changes[{i}] 缺 path/value —— 每条须为 "
                          "{path, value}（点分叶路径 + 目标值）。")
    return changes, None


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
        description="Safe patch generator / applier for the 261025 memory-core "
                    "dreaming-disable declaration（openclaw.json 只经 OpenClaw "
                    "写入口 config patch 写）。")
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="openclaw.json 路径（默认 %(default)s；--gen-patch 不读它）")
    ap.add_argument("--decl", default=None,
                    help="声明工件路径（默认 = 本脚本目录下 %s）" % DECL_NAME)
    ap.add_argument("--gen-patch", default=None, dest="gen_patch",
                    help="形态①：只从声明生成 config patch 文件后退出（不读 config、"
                         "不写 config、零备份）")
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
    changes, verr = _validate_changes(decl)
    if verr is not None:
        print(f"ERR: {verr}", file=sys.stderr)
        return 2
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
            print(f"  + {change['path']} = {change['value']}")
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

    # ⓪ 幂等预检（先于任何外部命令 / 备份 / 写；SKIP 先于 drift，architect 1g ②）
    if target_state(config, changes) == config:
        print("SKIP: config already at declared target state (idempotent, "
              "no write, no backup, no external command).")
        problems = drift_problems(config, drift_guard)
        for p in problems:
            print(f"WARN: drift baseline not clean -> {p}")
        if problems:
            print("WARN: dreaming 已关闭但工具面基线漂移 —— 组合态非静默，请核对 "
                  "261025 工具面（exit 0 不变：本包写目标仅 dreaming 一叶，工具面锚"
                  "是前置基线而非写目标）。")
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
