"""Frozen acceptance tests — 261022-m5-k1-prompt-carrier-migration K1-E4
(subsystem: server/deploy/261022-openclaw-kidpal-tools；TaskPackage =
delivery/taskpackages/261022-m5-k1-prompt-carrier-migration/taskpackage.json；
上游 = delivery/workstreams/device-memory-overhaul/PROPOSAL.md §1/§2 工具面收窄
裁定 + decisionCoverage「工具面收窄：kidpal agent 可见工具 = read/write/exec/
web_search 四项」）。

背景：全局 `tools.profile="coding"` 对设备 agent 意味着 34 工具 57KB schema/轮；
设备只需 4 项（TECH-261022 证据 3）。声明式覆写走 261017-openclaw-kidpal-skills
先例（apply 幂等 + drift_guard + backup + rollback；openclaw.json 含 gateway 凭据
**整文件不入仓**，只入无 secret 声明）。与 261017 skills 定点 patch 作用于同一
运行态文件（openclaw.json 不入仓）：**本包先落**，P3 的 drift_guard 现状锚须以
本包已应用态为准（plan.md 跨包事实）；两包均不得整文件改写。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O；仿 server/acceptance/test_261017_openclaw_kidpal_skills.acceptance.py
头注 FROZEN OFFLINE CONTRACT 同形制）

落位目录 = server/deploy/261022-openclaw-kidpal-tools/

  1) kidpal-agent-tools-declaration.json —— 声明工件（**JSON**，json.loads 解析，
     零额外依赖）。必需顶层键：case / change / drift_guard / backup / rollback。
       · case : 非空字符串（本包只有一种收窄策略，取值不作枚举约束）。
       · change : {"path": "agents.entries.kidpal.tools.allow",
                   "allow": ["read", "write", "exec", "web_search"]}
           - path 必须恰为 "agents.entries.kidpal.tools.allow"（apply / rollback /
             drift_guard 定位键家族中 apply 的写点）；
           - allow 必须**恰四项**、无重复、集合 = {read, write, exec, web_search}
             （顺序不作语义）。
       · drift_guard : 非空 list[{path, expect}]；须含
           {"path": "agents.entries.kidpal.tools", "expect": "unset"}
         （现状锚 = kidpal agent 未设工具覆写，全局 tools.profile="coding" 生效）。
         expect == "unset" 判键缺席，否则判值相等。
       · backup : {"dir": "<ECS 备份目录>", "pattern": "<含 'bak' 的命名模式>"}；
         pattern 是 config 同目录下备份文件的 glob（<config basename>.bak-...-*）。
       · rollback : {"script": "rollback_agent_tools.py"}（= 本目录下回滚脚本名）。

  2) apply_agent_tools.py —— 幂等 apply 脚本（**Python**；离线用
     `sys.executable <script> --config <fixture.json> --decl <decl.json>` 可真跑）。
     语义序（261017 PV-A6 同形制）：
       ① 幂等优先：config 现状已等于声明目标 → 打印 SKIP 并 exit 0（不写、不再备份）；
       ② drift-guard：逐条核对 drift_guard（expect=="unset" 判键缺席，否则判值
          相等），任一不中 → 打印 STOP 信息并 **exit 3**（不写、不备份）；
       ③ 备份：写前建时间戳备份，**落 config 同目录**，文件名 = config basename +
          ".bak-..." + UTC `yyyymmddTHHMMSS`（内容 = 原始 config；须匹配声明
          backup.pattern 的 glob）；
       ④ 应用：把 change.allow 写到 change.path；**只改该键**，其余键（含
          gateway.token、agents.entries 下其它 agent、261017 skills 白名单键）
          原样保留。成功 exit 0。
     CLI/参数：--config（默认 /root/.openclaw/openclaw.json）、--decl（默认同目录
     kidpal-agent-tools-declaration.json）。用 stdlib json（**不依赖 jq**）。
     缺 config / 缺声明 / config 非法 JSON → exit 2。

  3) rollback_agent_tools.py —— 一步回滚（Python）。CLI：--config、--backup
     （默认取 config 旁最新声明 backup.pattern 匹配件）、--dry-run（只打印意图、
     exit 0、不写）。非 dry-run：把 backup 内容还原到 config，exit 0。

  4) openclaw.json 整文件不入仓（含 gateway.token）；声明工件内不得出现
     gateway.token 字样、不得出现 token 键。

  5) manifest.yaml 登记：261022-openclaw-kidpal-tools 声明 / apply / rollback
     三件至少各一条（dst = /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/
     下），部署单一事实源（261013 纪律）。

===========================================================================
强度申报（契约条目 → 承载断言 → 性质档）：

  · 声明在位 / schema / allow 恰四项     → test_k1_e4_declaration_present_and_schema /
                                              test_k1_e4_allowlist_exactly_four → 机制级
  · apply 脚本消费声明（stdlib json）    → test_k1_e4_apply_script_present_and_consumes_declaration → 机制级
  · apply 幂等（二次零语义变更、零新备份）→ test_k1_e4_apply_applies_then_idempotent → 行为级（真跑）
  · drift_guard 现状不中拒绝（exit 3 零写）→ test_k1_e4_apply_drift_guard_exit3 → 行为级（真跑）
  · 写前时间戳备份（内容 = 原始 config） → test_k1_e4_apply_timestamped_backup → 行为级（真跑）
  · 只改 tools.allow 键、261017 skills
    组合态与 gateway.token 零触碰        → test_k1_e4_apply_preserves_other_keys_and_composes_with_skills_patch → 行为级（真跑）
  · 回滚 dry-run / 一步还原              → test_k1_e4_rollback_dry_run_and_restore → 行为级（真跑）
  · openclaw.json 零入仓 / 声明零 secret → test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration → 机制级
  · manifest 三件登记                    → test_k1_e4_manifest_registers_the_patch_workbench → 机制级

缺口（承载断言强度低于契约要求处，逐条）：
  · offline 只钉「脚本对临时 fixture 的行为」；ECS 上真实 openclaw.json 的 apply /
    回滚 / `openclaw` 审计可见工具 = 4 项、57KB schema 消失 = K1-E7 live 观测项
    （human 部署轮，本 Pod 不自行上 ECS），offline 不证 live 生效。
  · drift_guard「现状锚 = 未设」offline 只证声明形态；ECS 实况是否真为未设由
    apply 时 STOP / APPLY 回执承载（261017 PV-A6 同款缺口）。
  · 行为等价假设（显式化，不写即默认不成立）：
      - change.path = "agents.entries.kidpal.tools.allow" ⇔ OpenClaw 运行时按
        agent 级 tools.allow 列表收窄可见工具（运行时键形以部署轮工具列表观测
        为准；若部署轮实跑发现运行时键形不符，属验收判法升级，走 tighten-only
        窗口经 planner 调整，不得静默改 openclaw.json 结构凑合）；
      - `expect == "unset"` ⇔ openclaw.json 中 kidpal agent 无 tools 覆写键
        （全局 tools.profile="coding" 生效的现状）；
      - backup.pattern 以 glob 在 config 同目录成立 ⇔ pattern 形如
        `<config basename>.bak-...-*`；
      - 「工具面收窄生效」以 openclaw.json 文本状态为代理——OpenClaw 运行时是否
        真只注入四项归 K1-E7 live 轮。
  · fixture 自含性声明：全部断言直读 repo 内制品 + tempfile fixture，无外部资产 /
    时间真值 / 环境前提依赖（PyYAML 仅 manifest 齿用，与 verify-deploy.sh 同一
    解析器）。行为 seam（声明 → OpenClaw 运行时工具注入）的驱动方 = K1-E7
    部署轮，offline 侧无驱动方 = 已申报盲区。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/
  （standalone: python 本文件）

RED 基线（冻结时点树，2026-10-23，builder 尚未建目录）：K1-E4 全齿设计态红——
server/deploy/261022-openclaw-kidpal-tools/ 不存在（声明工件 / apply / rollback
三件缺失 = 真实缺失行为），红名逐条指向缺口；本件不触碰任何既有绿。
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

try:
    import yaml  # noqa: F401  (lazy import in helper; 环境前提缺失 = infra 红)
    _HAS_YAML = True
except ImportError:
    _HAS_YAML = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY_DIR = SERVER / "deploy"
TOOLS_DIR = DEPLOY_DIR / "261022-openclaw-kidpal-tools"
DECL_PATH = TOOLS_DIR / "kidpal-agent-tools-declaration.json"
APPLY_SCRIPT = TOOLS_DIR / "apply_agent_tools.py"
ROLLBACK_SCRIPT = TOOLS_DIR / "rollback_agent_tools.py"
MANIFEST_YAML = DEPLOY_DIR / "manifest.yaml"

TARGET_PATH = "agents.entries.kidpal.tools.allow"
DRIFT_ANCHOR_PATH = "agents.entries.kidpal.tools"
EXPECTED_TOOLS = ("read", "write", "exec", "web_search")
MANIFEST_DIR_TOKEN = "261022-openclaw-kidpal-tools"
TIMESTAMP_RE = re.compile(r"\d{8}T\d{6}")

_MISSING = object()


# --------------------------------------------------------------------------- #
# 读盘 / 进程 helpers（仓根由 __file__ 显式推导，不依赖调用点 cwd）
# --------------------------------------------------------------------------- #

def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _load_json(path: Path):
    txt = _read_text(path)
    if not txt:
        return None, f"{path} 缺失或为空"
    try:
        return json.loads(txt), None
    except ValueError as e:  # noqa: BLE001
        return None, f"{path} 非法 JSON: {e!r}"


def _write_json(path: Path, obj) -> None:
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2), encoding="utf-8")


def _run_py(script: Path, args, timeout: int = 120):
    """用当前解释器跑 python 脚本（离线可跑，不经 bash/ssh）；脚本缺失 → rc 127。"""
    if not script.is_file():
        return 127, "", f"{script} 不在位（冻结契约工件缺失）"
    proc = subprocess.run(
        [sys.executable, str(script), *[str(a) for a in args]],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
        timeout=timeout,
    )
    return proc.returncode, proc.stdout, proc.stderr


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


# --------------------------------------------------------------------------- #
# fixture（组合态：261017 skills 白名单已应用 + kidpal 无 tools 覆写 = drift 锚现状）
# --------------------------------------------------------------------------- #

def _fixture_pre() -> dict:
    """变更前现状：kidpal agent 无 tools 覆写（drift_guard 锚 = unset）；
    skills 白名单 = 261017 已应用组合态（plan.md：本包先落、P3 drift 锚以本包
    已应用态为准——组合共存是声明式 patch 的设计前提，非顺带）。"""
    return {
        "gateway": {"token": "FIXTURE-GATEWAY-TOKEN-0123456789"},
        "tools": {"profile": "coding"},
        "agents": {
            "defaults": {"model": {"primary": "openclaw/kidpal"}},
            "entries": {
                "main": {"skills": ["kidpal-schedule-write"]},
                "kidpal": {
                    "prompt": "kidpal/base",
                    "skills": ["kidpal-schedule-answer", "weather", "tavily"],
                },
            },
        },
        "modelPolicy": {"allow": ["openclaw/kidpal"]},
    }


def _fixture_mismatch() -> dict:
    """drift-guard 必不中的现状：tools.allow 已被设成别的白名单。"""
    state = _fixture_pre()
    _json_set(state, TARGET_PATH, ["bash", "browser"])
    return state


def _target_state(pre: dict, allow) -> dict:
    state = json.loads(json.dumps(pre))
    _json_set(state, TARGET_PATH, list(allow))
    return state


def _backups(td: Path, pattern: str) -> list[Path]:
    return sorted(td.glob(pattern))


class OpenclawKidpalToolsAcceptance(unittest.TestCase):
    """261022-m5-k1-prompt-carrier-migration K1-E4（offline，四态验收）。

    live-manual 观测项（**非自动断言**，structural 申报，本类不落测试）：
      human 部署轮在 ECS 核验——`openclaw` 工具面 audit 可见工具 = read / write /
      exec / web_search 恰四项、57KB schema 消失（K1-E7 的一部分）；本包 apply 在
      真实 openclaw.json 上的 STOP / APPLY 回执与 261017 skills patch 组合共存。
      非确定性来源：运行时键形与工具注入是部署后运行时观测量，offline 不可机判
      （行为等价假设见头注缺口节）。host 面转绿后停门等 human
      （awaiting-human:live-deploy），Pod 不自行上 ECS。"""

    maxDiff = None

    def _decl(self) -> dict:
        self.assertTrue(
            DECL_PATH.is_file(),
            f"K1-E4 RED: 声明工件不在位 —— 冻结契约要求 {DECL_PATH}。"
            "openclaw.json 含 gateway.token 整文件不入仓，只入声明（K1-E4 铁律）。")
        decl, err = _load_json(DECL_PATH)
        self.assertIsNotNone(decl, f"K1-E4 RED: 声明工件不可解析 —— {err}。")
        self.assertIsInstance(decl, dict, "K1-E4 RED: 声明工件顶层必须是 JSON object。")
        return decl

    def _backup_pattern(self, decl: dict) -> str:
        pattern = (decl.get("backup") or {}).get("pattern") if isinstance(
            decl.get("backup"), dict) else None
        self.assertTrue(
            isinstance(pattern, str) and pattern.strip(),
            "K1-E4 RED: 声明缺 backup.pattern（前置）。")
        self.assertNotIn(
            "/", pattern,
            f"K1-E4 RED: backup.pattern = {pattern!r} 含路径分隔符 —— 须为 config "
            "同目录下备份文件的相对 glob（<config basename>.bak-...-*）。")
        return pattern

    # ------------------------------------------------------------------ #
    # K1-E4 · 声明工件在位 / schema / allow 恰四项（机制级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_declaration_present_and_schema(self):
        """coverage: K1-E4 — 声明工件在位 + JSON 可解析 + 五顶层键齐 + 各段结构
        合法 + drift_guard 现状锚（tools 键 unset）。"""
        decl = self._decl()
        for key in ("case", "change", "drift_guard", "backup", "rollback"):
            self.assertIn(
                key, decl,
                f"K1-E4 RED: 声明工件缺必需顶层键 {key!r} —— 冻结契约结构 = "
                "case / change / drift_guard / backup / rollback（仿 261017 PV-A6）。")
        self.assertIsInstance(decl.get("case"), str, "K1-E4 RED: case 必须是字符串。")
        self.assertTrue(str(decl.get("case", "")).strip(), "K1-E4 RED: case 不得为空。")

        change = decl.get("change")
        self.assertIsInstance(change, dict, "K1-E4 RED: change 必须是 object。")
        self.assertEqual(
            change.get("path"), TARGET_PATH,
            f"K1-E4 RED: change.path = {change.get('path')!r}，须恰为 "
            f"{TARGET_PATH!r}（apply 写点）。")

        dg = decl.get("drift_guard")
        self.assertIsInstance(dg, list, "K1-E4 RED: drift_guard 必须是 list。")
        self.assertTrue(dg, "K1-E4 RED: drift_guard 不得为空。")
        for i, entry in enumerate(dg):
            self.assertIsInstance(entry, dict,
                                  f"K1-E4 RED: drift_guard[{i}] 必须是 object。")
            self.assertTrue(
                isinstance(entry.get("path"), str) and entry.get("path", "").strip(),
                f"K1-E4 RED: drift_guard[{i}].path 必须是非空字符串。")
            self.assertTrue(
                isinstance(entry.get("expect"), str) and entry.get("expect", "").strip(),
                f"K1-E4 RED: drift_guard[{i}].expect 必须是非空字符串。")
        self.assertTrue(
            any(e.get("path") == DRIFT_ANCHOR_PATH and e.get("expect") == "unset"
                for e in dg),
            f"K1-E4 RED: drift_guard 未锚定 {{'path': {DRIFT_ANCHOR_PATH!r}, "
            "'expect': 'unset'}} —— 现状锚（kidpal agent 未设工具覆写，全局 "
            'tools.profile="coding" 生效）缺失则 apply 无法拒绝漂移。')

        backup = decl.get("backup")
        self.assertIsInstance(backup, dict, "K1-E4 RED: backup 必须是 object。")
        self.assertTrue(
            isinstance(backup.get("dir"), str) and backup.get("dir", "").strip(),
            "K1-E4 RED: backup.dir 必须是非空字符串（ECS 备份目录）。")
        self.assertTrue(
            isinstance(backup.get("pattern"), str) and "bak" in backup.get("pattern", ""),
            "K1-E4 RED: backup.pattern 必须是含 'bak' 的字符串（config 同目录 glob）。")

        rollback = decl.get("rollback")
        self.assertIsInstance(rollback, dict, "K1-E4 RED: rollback 必须是 object。")
        script = rollback.get("script")
        self.assertTrue(
            isinstance(script, str) and script.strip(),
            "K1-E4 RED: rollback.script 必须是非空字符串。")
        self.assertTrue(
            (TOOLS_DIR / script).is_file(),
            f"K1-E4 RED: rollback.script 指向的 {script!r} 不在 {TOOLS_DIR} —— "
            "声明面与回滚面不一致（回滚不可跑）。")

    def test_k1_e4_allowlist_exactly_four(self):
        """coverage: K1-E4 — 工具白名单恰四项、无重复、集合 = read / write / exec /
        web_search（顺序不作语义）。"""
        decl = self._decl()
        allow = (decl.get("change") or {}).get("allow")
        self.assertIsInstance(allow, list,
                              "K1-E4 RED: change.allow 不是 list（目标白名单）。")
        self.assertEqual(
            len(allow), 4,
            f"K1-E4 RED: 白名单项数 = {len(allow)}（{allow}），须恰四项 —— 设备"
            "agent 只需 read / write / exec / web_search（TECH-261022 证据 3）。")
        self.assertEqual(
            len(set(allow)), 4,
            f"K1-E4 RED: 白名单有重复项（{allow}）—— 重复项不构成额外能力。")
        self.assertEqual(
            sorted(allow), sorted(EXPECTED_TOOLS),
            f"K1-E4 RED: 白名单 {sorted(allow)} ≠ 契约四项 {sorted(EXPECTED_TOOLS)} "
            "—— 34 工具噪音未收敛干净或误收多余能力。")

    def test_k1_e4_apply_script_present_and_consumes_declaration(self):
        """coverage: K1-E4 — apply 脚本在位、消费声明工件名、用 stdlib json
        （不依赖 jq）。"""
        self.assertTrue(
            APPLY_SCRIPT.is_file(),
            f"K1-E4 RED: apply 脚本不在位 —— 冻结契约要求 {APPLY_SCRIPT}。")
        src = _read_text(APPLY_SCRIPT)
        self.assertIn(
            "kidpal-agent-tools-declaration.json", src,
            "K1-E4 RED: apply 脚本未引用声明工件名 —— 声明面与消费面不一致。")
        self.assertIsNotNone(
            re.search(r"(?m)^\s*(?:import\b[^\n]*\bjson\b|from\s+json\s+import\b)", src),
            "K1-E4 RED: apply 脚本未用 stdlib json（冻结契约：零额外依赖、不依赖 jq）。")

    # ------------------------------------------------------------------ #
    # K1-E4 · apply 真跑（幂等 / drift-guard / 备份 / 只改目标键）（行为级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_apply_applies_then_idempotent(self):
        """coverage: K1-E4 — apply 真跑①：现状锚匹配 → 声明目标态；②：再跑 →
        幂等 no-op（同值、零新备份、rc 0）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        allow = (decl.get("change") or {}).get("allow")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_pre()
            _write_json(cfg, pre)
            want = _target_state(pre, allow)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: apply 首次运行 rc={rc}（应 0）。stdout={out!r} stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "K1-E4 RED: apply 未产生声明目标态（kidpal tools.allow = 契约四项）。")
            backups_after_first = _backups(td, pattern)
            self.assertTrue(
                backups_after_first,
                "K1-E4 RED: 首次 apply 未建备份（前置，供下方幂等对比）。")

            rc2, out2, err2 = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            got2, _ = _load_json(cfg)
            self.assertEqual(
                rc2, 0,
                f"K1-E4 RED: apply 二次运行 rc={rc2}（幂等应 0）。"
                f"stdout={out2!r} stderr={err2!r}")
            self.assertEqual(
                got2, want, "K1-E4 RED: apply 二次运行改动了已达标态（非幂等）。")
            self.assertEqual(
                len(_backups(td, pattern)), len(backups_after_first),
                "K1-E4 RED: apply 二次运行又新建了备份（幂等应不重备份）。")

    def test_k1_e4_apply_drift_guard_exit3(self):
        """coverage: K1-E4 — drift_guard 现状锚不中 → 拒绝（退出码 3）且零写入、
        零备份。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            drifted = _fixture_mismatch()
            _write_json(cfg, drifted)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 3,
                f"K1-E4 RED: drift-guard 现状锚不中应 exit 3（实得 rc={rc}）。"
                f"stdout={out!r} stderr={err!r}")
            after, _ = _load_json(cfg)
            self.assertEqual(after, drifted, "K1-E4 RED: drift-guard 拒绝后仍改写了 config。")
            self.assertEqual(
                _backups(td, pattern), [],
                "K1-E4 RED: drift-guard 拒绝后仍建了备份（拒绝 = 完全 no-op）。")

    def test_k1_e4_apply_timestamped_backup(self):
        """coverage: K1-E4 — 写前建时间戳备份（落 config 同目录、匹配声明 pattern、
        文件名含 yyyymmddTHHMMSS、内容 = 原始 config）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        pre = _fixture_pre()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"K1-E4 RED: apply rc={rc}。stderr={err!r}")
            backups = _backups(td, pattern)
            self.assertTrue(
                backups,
                "K1-E4 RED: apply 未建匹配 backup.pattern 的备份 —— openclaw.json "
                "定点 patch 回滚先决缺失。")
            self.assertRegex(
                backups[0].name, TIMESTAMP_RE.pattern,
                f"K1-E4 RED: 备份名 {backups[0].name!r} 不含 UTC 时间戳"
                "（yyyymmddTHHMMSS）—— 非时间戳备份无法确定先后。")
            saved, _ = _load_json(backups[0])
            self.assertEqual(
                saved, pre, "K1-E4 RED: 备份内容 ≠ 变更前 config（备份不及时/被覆盖）。")

    def test_k1_e4_apply_preserves_other_keys_and_composes_with_skills_patch(self):
        """coverage: K1-E4 — apply 只改 tools.allow 键：gateway.token、261017
        skills 白名单组合态、其它 agent、modelPolicy 原样保留（secret 零触碰、
        非目标字段零扰动、声明式 patch 组合共存）。"""
        self.assertTrue(APPLY_SCRIPT.is_file(), "K1-E4 RED: apply 脚本不在位（前置）。")
        pre = _fixture_pre()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"K1-E4 RED: apply rc={rc}。stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                _json_get(got, "gateway.token"), _json_get(pre, "gateway.token"),
                "K1-E4 RED: apply 改动了 gateway.token —— 定点 patch 越界触碰 secret"
                "（冻结契约：只改 agents.entries.kidpal.tools.allow）。")
            self.assertEqual(
                _json_get(got, "agents.entries.kidpal.skills"),
                _json_get(pre, "agents.entries.kidpal.skills"),
                "K1-E4 RED: apply 抹掉了 261017 skills 白名单组合态 —— 声明式 patch "
                "组合共存被破坏（plan.md 跨包事实：本包与 261017/P3 作用于同一文件，"
                "均不得整文件改写）。")
            self.assertEqual(
                _json_get(got, "agents.entries.main"), _json_get(pre, "agents.entries.main"),
                "K1-E4 RED: apply 改动了其它 agent 条目 —— 定点 patch 越界。")
            self.assertEqual(
                _json_get(got, "tools"), _json_get(pre, "tools"),
                "K1-E4 RED: apply 改动了全局 tools 块 —— 定点 patch 越界（只收窄 "
                "kidpal agent，不动全局 profile）。")
            self.assertEqual(
                _json_get(got, "modelPolicy"), _json_get(pre, "modelPolicy"),
                "K1-E4 RED: apply 改动了 modelPolicy —— 定点 patch 越界。")

    # ------------------------------------------------------------------ #
    # K1-E4 · 回滚（dry-run + 一步还原）（行为级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_rollback_dry_run_and_restore(self):
        """coverage: K1-E4 — 回滚脚本 --dry-run 可跑（exit 0）且零写入；非
        dry-run 一步还原 backup 内容（exit 0）。"""
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"K1-E4 RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-kidpal-tools-20261023T000000"
            cur = {"applied": True}
            _write_json(cfg, cur)
            _write_json(bak, {"applied": False})

            rc, out, err = _run_py(
                ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak, "--dry-run"])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: rollback --dry-run rc={rc}（应 0）。"
                f"stdout={out!r} stderr={err!r}")
            after, _ = _load_json(cfg)
            self.assertEqual(after, cur, "K1-E4 RED: --dry-run 改写了 config。")

            rc2, out2, err2 = _run_py(ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak])
            self.assertEqual(
                rc2, 0, f"K1-E4 RED: rollback rc={rc2}。stdout={out2!r} stderr={err2!r}")
            after2, _ = _load_json(cfg)
            self.assertEqual(
                after2, {"applied": False}, "K1-E4 RED: 回滚未还原 backup 内容。")

    # ------------------------------------------------------------------ #
    # K1-E4 · openclaw.json 零入仓 / 声明零 secret（机制级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration(self):
        """coverage: K1-E4 — openclaw.json 整文件不入仓（含 gateway.token）；声明
        工件内不得出现 gateway.token 字样、不得出现 token 键。"""
        leaked = [str(p) for p in SERVER.rglob("openclaw.json") if p.is_file()]
        self.assertEqual(
            leaked, [],
            "K1-E4 RED: 仓内出现 openclaw.json（含 gateway.token secret）—— "
            "只入无 secret 的声明工件，整文件零入仓。实得："
            f"{leaked}")
        decl_text = _read_text(DECL_PATH)
        self.assertTrue(decl_text.strip(), "K1-E4 RED: 声明工件不在位/为空（前置）。")
        self.assertNotIn(
            "gateway.token", decl_text,
            "K1-E4 RED: 声明工件出现 gateway.token 字样 —— 声明面不得承载 secret 键。")
        self.assertIsNone(
            re.search(r'"token"\s*:', decl_text),
            "K1-E4 RED: 声明工件出现 token 键 —— 声明面不得承载 secret。")

    # ------------------------------------------------------------------ #
    # K1-E4 · manifest 登记（部署单一事实源，261013 纪律）
    # ------------------------------------------------------------------ #

    def test_k1_e4_manifest_registers_the_patch_workbench(self):
        """coverage: K1-E4 — manifest 至少三条 261022-openclaw-kidpal-tools 条目
        （声明 / apply / rollback），src 在仓内存在、dst 在
        /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/ 消费位。"""
        self.assertTrue(_HAS_YAML or self.fail(
            "K1-E4 RED(infra): PyYAML 不在位——manifest 解析前置缺失。"))
        self.assertTrue(
            MANIFEST_YAML.is_file(), "K1-E4 RED: manifest.yaml 不在场。")
        try:
            doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
        except Exception as e:  # noqa: BLE001 — 解析错误是 RED，不是 crash
            self.fail(f"K1-E4 RED: manifest.yaml 解析失败: {e!r}")
        entries = [
            e for e in (doc or {}).get("entries", []) or []
            if isinstance(e, dict) and MANIFEST_DIR_TOKEN in (
                str(e.get("src") or "") + str(e.get("dst") or ""))]
        self.assertGreaterEqual(
            len(entries), 3,
            f"K1-E4 RED: manifest 对 {MANIFEST_DIR_TOKEN} 的登记条目 "
            f"{len(entries)} < 3 —— 声明 / apply / rollback 三件须各入部署单一"
            "事实源（261013 纪律：新资产必须登记并接入 verify-deploy.sh）。")
        srcs = [str(e.get("src") or "") for e in entries]
        missing_src = [s for s in srcs if s and not (REPO_ROOT / s).is_file()]
        self.assertEqual(
            missing_src, [],
            f"K1-E4 RED: manifest 登记的仓内源缺失: {missing_src}。")
        bad_dst = [str(e.get("dst") or "") for e in entries
                   if not str(e.get("dst") or "").startswith(
                       "/opt/kidpal/openclaw-patch/" + MANIFEST_DIR_TOKEN)]
        self.assertEqual(
            bad_dst, [],
            f"K1-E4 RED: 登记条目 dst 越出 openclaw-patch 消费位: {bad_dst}。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
