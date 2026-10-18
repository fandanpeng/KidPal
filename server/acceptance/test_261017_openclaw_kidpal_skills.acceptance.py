"""Frozen acceptance tests — 261017-prompt-v11-schedule-recall PV-A6
(subsystem: server/deploy/261017-openclaw-kidpal-skills；TaskPackage =
delivery/taskpackages/261017-prompt-v11-schedule-recall/taskpackage.json 的 PV-A6)。

背景：kidpal agent 的 openclaw.json 里 19 个 modelVisible skill 中 18 个是 OpenClaw
bundled/extra 噪音，挤占每次注入的 system prompt（TECH-261017-xiaozhi-template-
residue-deploy-ssot-gap 附带项③）。openclaw.json 含 gateway.token，**整文件不入仓**，
只入无 secret 的声明工件；本件钉该声明工件的离线契约（仿
server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py 的声明工件形态）。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期零网络 I/O）

落位目录 = server/deploy/261017-openclaw-kidpal-skills/

  1) kidpal-agent-skills-declaration.json —— 声明工件（**JSON**，json.loads 解析，
     零额外依赖）。必需顶层键：case / change / drift_guard / backup / rollback。
       · case : 非空字符串（本包只有一种收敛策略，取值不作枚举约束）。
       · change : {"path": "agents.entries.kidpal.skills",
                   "skills": ["kidpal-schedule-answer", "weather", "tavily"]}
           - path 必须恰为 "agents.entries.kidpal.skills"（apply / rollback /
             drift_guard 三者共用的唯一定位键）；
           - skills 必须**恰三项**、无重复、集合 = 上述白名单（顺序不作语义）。
       · drift_guard : 非空 list[{path, expect}]；须含
           {"path": "agents.entries.kidpal.skills", "expect": "unset"}
         （现状锚 = kidpal agent 未设 authored skills，bundled/extra 全量生效）。
         expect == "unset" 判键缺席，否则判值相等。
       · backup : {"dir": "<ECS 备份目录>", "pattern": "<含 'bak' 的命名模式>"}；
         pattern 是 config 同目录下备份文件的 glob（<config basename>.bak-...-*）。
       · rollback : {"script": "rollback_agent_skills.py"}（= 本目录下回滚脚本名）。

  2) apply_agent_skills.py —— 幂等 apply 脚本（**Python**；离线用
     `sys.executable <script> --config <fixture.json> --decl <decl.json>` 可真跑）。
     语义序（与 perf-k1 PERF-A1 同形制）：
       ① 幂等优先：config 现状已等于声明目标 → 打印 SKIP 并 exit 0（不写、不再备份）；
       ② drift-guard：逐条核对 drift_guard（expect=="unset" 判键缺席，否则判值相等），
          任一不中 → 打印 STOP 信息并 **exit 3**（不写、不备份）；
       ③ 备份：写前建时间戳备份，**落 config 同目录**，文件名 = config basename +
          ".bak-..." + UTC `yyyymmddTHHMMSS`（内容 = 原始 config；须匹配声明
          backup.pattern 的 glob）；
       ④ 应用：把 change.skills 写到 change.path；**只改该键**，其余键（含
          gateway.token、agents.entries 下其它 agent）原样保留。成功 exit 0。
      CLI/参数：--config（默认 /root/.openclaw/openclaw.json）、--decl（默认同目录
     kidpal-agent-skills-declaration.json）。用 stdlib json（**不依赖 jq**）。
     缺 config / 缺声明 / config 非法 JSON → exit 2。

  3) rollback_agent_skills.py —— 一步回滚（Python）。CLI：--config、--backup
     （默认取 config 旁最新声明 backup.pattern 匹配件）、--dry-run（只打印意图、
     exit 0、不写）。非 dry-run：把 backup 内容还原到 config，exit 0。

  4) openclaw.json 整文件不入仓（含 gateway.token）；声明工件内不得出现
     gateway.token 字样。

===========================================================================
强度申报（契约条目 → 承载齿 → 性质档）：

  · 声明在位 / JSON 可解析 / 顶层键齐        → test_pv_a6_declaration_present_and_schema → 机制级
  · allowlist 恰三项 / 无重复 / path 定位    → test_pv_a6_allowlist_exactly_three     → 机制级
  · apply 脚本消费声明（stdlib json）        → test_pv_a6_apply_script_consumes_declaration → 机制级
  · apply 幂等（二次零语义变更、零新备份）   → test_pv_a6_apply_idempotent             → 行为级（真跑）
  · drift_guard 现状不中拒绝（exit 3 零写）  → test_pv_a6_apply_drift_guard_exit3      → 行为级（真跑）
  · 写前时间戳备份（内容 = 原始 config）     → test_pv_a6_apply_timestamped_backup     → 行为级（真跑）
  · 只改 skills 键、gateway.token 零触碰     → test_pv_a6_apply_preserves_other_keys    → 行为级（真跑）
  · 回滚 dry-run / 一步还原                  → test_pv_a6_rollback_dry_run_no_write /
                                               test_pv_a6_rollback_restores_backup    → 行为级（真跑）
  · openclaw.json 零入仓 / 声明零 secret     → test_pv_a6_no_openclaw_json_in_repo     → 机制级

缺口（承载齿强度低于契约要求处，逐条）：
  · offline 只钉「脚本对临时 fixture 的行为」；ECS 上真实 openclaw.json 的 apply /
    回滚 / `openclaw skills list --agent kidpal` ready = 3 归 PV-A8 真机轮（human
    持板），offline 不证 live 生效。
  · drift_guard 的「现状锚 = 未设」offline 只证声明形态；ECS 实况是否真为未设由
    apply 时 STOP / APPLY 回执承载（真锚一致性只在 live 执行时成立，perf-k1 同款缺口）。
  · 行为等价假设（显式化，不写即默认不成立）：
      - `expect == "unset"` ⇔ openclaw.json 中 kidpal agent 无 authored skills 键
        （bundled/extra 全量生效的现状）；
      - backup.pattern 以 glob 在 config 同目录成立 ⇔ pattern 形如
        `<config basename>.bak-...-*`；
      - 「allowlist 生效」以 openclaw.json 文本状态为代理——OpenClaw 运行时是否
        真只注入三项归 PV-A8 live 轮。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/
  （standalone: python 本文件）

RED 基线（冻结时点树，2026-10-18，builder 尚未建目录）：PV-A6 全齿设计态红——
server/deploy/261017-openclaw-kidpal-skills/ 不存在（声明工件 / apply / rollback
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

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY_DIR = SERVER / "deploy"
SKILLS_DIR = DEPLOY_DIR / "261017-openclaw-kidpal-skills"
DECL_PATH = SKILLS_DIR / "kidpal-agent-skills-declaration.json"
APPLY_SCRIPT = SKILLS_DIR / "apply_agent_skills.py"
ROLLBACK_SCRIPT = SKILLS_DIR / "rollback_agent_skills.py"

TARGET_PATH = "agents.entries.kidpal.skills"
EXPECTED_SKILLS = ("kidpal-schedule-answer", "weather", "tavily")
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
# fixture（「现状 = 未设 skills」的 openclaw.json 最小形状）
# --------------------------------------------------------------------------- #

def _fixture_pre() -> dict:
    """变更前现状：kidpal agent 无 authored skills（drift_guard 锚 = unset）。"""
    return {
        "gateway": {"token": "FIXTURE-GATEWAY-TOKEN-0123456789"},
        "agents": {
            "defaults": {"model": {"primary": "openclaw/kidpal"}},
            "entries": {
                "main": {"skills": ["kidpal-schedule-write"]},
                "kidpal": {"prompt": "kidpal/base"},
            },
        },
        "modelPolicy": {"allow": ["openclaw/kidpal"]},
    }


def _fixture_mismatch() -> dict:
    """drift-guard 必不中的现状：skills 键已被设成别的白名单。"""
    state = _fixture_pre()
    _json_set(state, TARGET_PATH, ["bundled-noise"])
    return state


def _target_state(pre: dict, skills) -> dict:
    state = json.loads(json.dumps(pre))
    _json_set(state, TARGET_PATH, list(skills))
    return state


def _backups(td: Path, pattern: str) -> list[Path]:
    return sorted(td.glob(pattern))


class OpenclawKidpalSkillsAcceptance(unittest.TestCase):

    maxDiff = None

    def _decl(self) -> dict:
        self.assertTrue(
            DECL_PATH.is_file(),
            f"PV-A6 RED: 声明工件不在位 —— 冻结契约要求 {DECL_PATH}。"
            "openclaw.json 含 gateway.token 整文件不入仓，只入声明（TECH-261017 "
            "附带项③）。",
        )
        decl, err = _load_json(DECL_PATH)
        self.assertIsNotNone(
            decl, f"PV-A6 RED: 声明工件不可解析 —— {err}。")
        self.assertIsInstance(decl, dict, "PV-A6 RED: 声明工件顶层必须是 JSON object。")
        return decl

    def _backup_pattern(self, decl: dict) -> str:
        pattern = (decl.get("backup") or {}).get("pattern") if isinstance(
            decl.get("backup"), dict) else None
        self.assertTrue(
            isinstance(pattern, str) and pattern.strip(),
            "PV-A6 RED: 声明缺 backup.pattern（前置）。")
        self.assertNotIn(
            "/", pattern,
            f"PV-A6 RED: backup.pattern = {pattern!r} 含路径分隔符 —— 须为 config "
            "同目录下备份文件的相对 glob（<config basename>.bak-...-*）。")
        return pattern

    # ------------------------------------------------------------------ #
    # PV-A6 · 声明工件在位 / schema / allowlist 恰三项（机制级）
    # ------------------------------------------------------------------ #

    def test_pv_a6_declaration_present_and_schema(self):
        """coverage: PV-A6 — 声明工件在位 + JSON 可解析 + 五顶层键齐 + 各段结构合法。"""
        decl = self._decl()
        for key in ("case", "change", "drift_guard", "backup", "rollback"):
            self.assertIn(
                key, decl,
                f"PV-A6 RED: 声明工件缺必需顶层键 {key!r} —— 冻结契约结构 = "
                "case / change / drift_guard / backup / rollback（仿 perf-k1）。")
        self.assertIsInstance(decl.get("case"), str, "PV-A6 RED: case 必须是字符串。")
        self.assertTrue(str(decl.get("case", "")).strip(), "PV-A6 RED: case 不得为空。")

        change = decl.get("change")
        self.assertIsInstance(change, dict, "PV-A6 RED: change 必须是 object。")
        self.assertEqual(
            change.get("path"), TARGET_PATH,
            f"PV-A6 RED: change.path = {change.get('path')!r}，须恰为 "
            f"{TARGET_PATH!r}（apply / rollback / drift_guard 共用定位键）。")
        self.assertIsInstance(
            change.get("skills"), list,
            "PV-A6 RED: change.skills 必须是 list（目标白名单）。")

        dg = decl.get("drift_guard")
        self.assertIsInstance(dg, list, "PV-A6 RED: drift_guard 必须是 list。")
        self.assertTrue(dg, "PV-A6 RED: drift_guard 不得为空。")
        for i, entry in enumerate(dg):
            self.assertIsInstance(entry, dict, f"PV-A6 RED: drift_guard[{i}] 必须是 object。")
            self.assertTrue(
                isinstance(entry.get("path"), str) and entry.get("path", "").strip(),
                f"PV-A6 RED: drift_guard[{i}].path 必须是非空字符串。")
            self.assertTrue(
                isinstance(entry.get("expect"), str) and entry.get("expect", "").strip(),
                f"PV-A6 RED: drift_guard[{i}].expect 必须是非空字符串"
                "（'unset' 或具体值）。")
        self.assertTrue(
            any(e.get("path") == TARGET_PATH and e.get("expect") == "unset" for e in dg),
            "PV-A6 RED: drift_guard 未锚定 "
            f"{{'path': {TARGET_PATH!r}, 'expect': 'unset'}} —— 现状锚（kidpal agent "
            "未设 authored skills）缺失则 apply 无法拒绝漂移。")

        backup = decl.get("backup")
        self.assertIsInstance(backup, dict, "PV-A6 RED: backup 必须是 object。")
        self.assertTrue(
            isinstance(backup.get("dir"), str) and backup.get("dir", "").strip(),
            "PV-A6 RED: backup.dir 必须是非空字符串（ECS 备份目录）。")
        self.assertTrue(
            isinstance(backup.get("pattern"), str) and "bak" in backup.get("pattern", ""),
            "PV-A6 RED: backup.pattern 必须是含 'bak' 的字符串（config 同目录 glob）。")

        rollback = decl.get("rollback")
        self.assertIsInstance(rollback, dict, "PV-A6 RED: rollback 必须是 object。")
        script = rollback.get("script")
        self.assertTrue(
            isinstance(script, str) and script.strip(),
            "PV-A6 RED: rollback.script 必须是非空字符串。")
        self.assertTrue(
            (SKILLS_DIR / script).is_file(),
            f"PV-A6 RED: rollback.script 指向的 {script!r} 不在 {SKILLS_DIR} —— "
            "声明面与回滚面不一致（回滚不可跑）。")

    def test_pv_a6_allowlist_exactly_three(self):
        """coverage: PV-A6 — 目标白名单恰三项、无重复、集合 = kidpal-schedule-answer /
        weather / tavily（顺序不作语义）。"""
        decl = self._decl()
        skills = (decl.get("change") or {}).get("skills")
        self.assertIsInstance(skills, list, "PV-A6 RED: change.skills 不是 list（前置）。")
        self.assertEqual(
            len(skills), 3,
            f"PV-A6 RED: 白名单项数 = {len(skills)}（{skills}），须恰三项 —— "
            "kidpal agent 只保留 kidpal-schedule-answer / weather / tavily。")
        self.assertEqual(
            len(set(skills)), 3,
            f"PV-A6 RED: 白名单有重复项（{skills}）—— 重复项不构成额外能力。")
        self.assertEqual(
            sorted(skills), sorted(EXPECTED_SKILLS),
            f"PV-A6 RED: 白名单 {sorted(skills)} ≠ 契约三项 "
            f"{sorted(EXPECTED_SKILLS)} —— bundled/extra 噪音未收敛干净或误删"
            "kidpal-schedule-answer。")

    def test_pv_a6_apply_script_consumes_declaration(self):
        """coverage: PV-A6 — apply 脚本在位、消费声明工件名、用 stdlib json（不依赖 jq）。"""
        self.assertTrue(
            APPLY_SCRIPT.is_file(),
            f"PV-A6 RED: apply 脚本不在位 —— 冻结契约要求 {APPLY_SCRIPT}。")
        src = _read_text(APPLY_SCRIPT)
        self.assertIn(
            "kidpal-agent-skills-declaration.json", src,
            "PV-A6 RED: apply 脚本未引用声明工件名 —— 声明面与消费面不一致。")
        self.assertIsNotNone(
            re.search(r"(?m)^\s*(?:import\b[^\n]*\bjson\b|from\s+json\s+import\b)", src),
            "PV-A6 RED: apply 脚本未用 stdlib json（冻结契约：零额外依赖、不依赖 jq）。")

    # ------------------------------------------------------------------ #
    # PV-A6 · apply 真跑（幂等 / drift-guard / 备份 / 只改 skills 键）（行为级）
    # ------------------------------------------------------------------ #

    def test_pv_a6_apply_idempotent(self):
        """coverage: PV-A6 — apply 真跑①：现状锚匹配 → 声明目标态；②：再跑 →
        幂等 no-op（同值、零新备份、rc 0）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        skills = (decl.get("change") or {}).get("skills")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_pre()
            _write_json(cfg, pre)
            want = _target_state(pre, skills)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0,
                f"PV-A6 RED: apply 首次运行 rc={rc}（应 0）。stdout={out!r} stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"PV-A6 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "PV-A6 RED: apply 未产生声明目标态（kidpal skills = 声明白名单三项）。")
            backups_after_first = _backups(td, pattern)
            self.assertTrue(
                backups_after_first,
                "PV-A6 RED: 首次 apply 未建备份（前置，供下方幂等对比）。")

            rc2, out2, err2 = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            got2, _ = _load_json(cfg)
            self.assertEqual(
                rc2, 0,
                f"PV-A6 RED: apply 二次运行 rc={rc2}（幂等应 0）。"
                f"stdout={out2!r} stderr={err2!r}")
            self.assertEqual(
                got2, want, "PV-A6 RED: apply 二次运行改动了已达标态（非幂等）。")
            self.assertEqual(
                len(_backups(td, pattern)), len(backups_after_first),
                "PV-A6 RED: apply 二次运行又新建了备份（幂等应不重备份）。")

    def test_pv_a6_apply_drift_guard_exit3(self):
        """coverage: PV-A6 — drift_guard 现状锚不中 → 拒绝（退出码 3）且零写入、零备份。"""
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
                f"PV-A6 RED: drift-guard 现状锚不中应 exit 3（实得 rc={rc}）。"
                f"stdout={out!r} stderr={err!r}")
            after, _ = _load_json(cfg)
            self.assertEqual(
                after, drifted, "PV-A6 RED: drift-guard 拒绝后仍改写了 config。")
            self.assertEqual(
                _backups(td, pattern), [],
                "PV-A6 RED: drift-guard 拒绝后仍建了备份（拒绝 = 完全 no-op）。")

    def test_pv_a6_apply_timestamped_backup(self):
        """coverage: PV-A6 — 写前建时间戳备份（落 config 同目录、匹配声明 pattern、
        文件名含 yyyymmddTHHMMSS、内容 = 原始 config）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        pre = _fixture_pre()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"PV-A6 RED: apply rc={rc}。stderr={err!r}")
            backups = _backups(td, pattern)
            self.assertTrue(
                backups,
                "PV-A6 RED: apply 未建匹配 backup.pattern 的备份 —— openclaw.json "
                "定点 patch 回滚先决缺失。")
            self.assertRegex(
                backups[0].name, TIMESTAMP_RE.pattern,
                f"PV-A6 RED: 备份名 {backups[0].name!r} 不含 UTC 时间戳"
                "（yyyymmddTHHMMSS）—— 非时间戳备份无法确定先后。")
            saved, _ = _load_json(backups[0])
            self.assertEqual(
                saved, pre, "PV-A6 RED: 备份内容 ≠ 变更前 config（备份不及时/被覆盖）。")

    def test_pv_a6_apply_preserves_other_keys(self):
        """coverage: PV-A6 — apply 只改 skills 键：gateway.token 与其它 agent /
        同 agent 其它键原样保留（secret 零触碰、非目标字段零扰动）。"""
        self.assertTrue(APPLY_SCRIPT.is_file(), "PV-A6 RED: apply 脚本不在位（前置）。")
        pre = _fixture_pre()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"PV-A6 RED: apply rc={rc}。stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"PV-A6 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                _json_get(got, "gateway.token"), _json_get(pre, "gateway.token"),
                "PV-A6 RED: apply 改动了 gateway.token —— 定点 patch 越界触碰 secret"
                "（冻结契约：只改 agents.entries.kidpal.skills）。")
            self.assertEqual(
                _json_get(got, "agents.entries.main"), _json_get(pre, "agents.entries.main"),
                "PV-A6 RED: apply 改动了其它 agent 条目 —— 定点 patch 越界。")
            self.assertEqual(
                _json_get(got, "agents.entries.kidpal.prompt"),
                _json_get(pre, "agents.entries.kidpal.prompt"),
                "PV-A6 RED: apply 抹掉了 kidpal 条目的既有非 skills 键 —— "
                "改写而非定点合并。")
            self.assertEqual(
                _json_get(got, "modelPolicy"), _json_get(pre, "modelPolicy"),
                "PV-A6 RED: apply 改动了 modelPolicy —— 定点 patch 越界。")

    # ------------------------------------------------------------------ #
    # PV-A6 · 回滚（dry-run + 一步还原）（行为级）
    # ------------------------------------------------------------------ #

    def test_pv_a6_rollback_dry_run_no_write(self):
        """coverage: PV-A6 — 回滚脚本 --dry-run 可跑（exit 0）且零写入。"""
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"PV-A6 RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-kidpal-skills-20261018T000000"
            cur = {"applied": True}
            _write_json(cfg, cur)
            _write_json(bak, {"applied": False})

            rc, out, err = _run_py(
                ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak, "--dry-run"])
            self.assertEqual(
                rc, 0,
                f"PV-A6 RED: rollback --dry-run rc={rc}（应 0）。"
                f"stdout={out!r} stderr={err!r}")
            after, _ = _load_json(cfg)
            self.assertEqual(after, cur, "PV-A6 RED: --dry-run 改写了 config。")

    def test_pv_a6_rollback_restores_backup(self):
        """coverage: PV-A6 — 一步还原：backup 内容写回 config（exit 0）。"""
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"PV-A6 RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-kidpal-skills-20261018T000000"
            _write_json(cfg, {"applied": True})
            _write_json(bak, {"applied": False})

            rc, out, err = _run_py(ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak])
            self.assertEqual(
                rc, 0, f"PV-A6 RED: rollback rc={rc}。stdout={out!r} stderr={err!r}")
            after, _ = _load_json(cfg)
            self.assertEqual(
                after, {"applied": False}, "PV-A6 RED: 回滚未还原 backup 内容。")

    # ------------------------------------------------------------------ #
    # PV-A6 · openclaw.json 零入仓 / 声明零 secret（机制级）
    # ------------------------------------------------------------------ #

    def test_pv_a6_no_openclaw_json_in_repo(self):
        """coverage: PV-A6 — openclaw.json 整文件不入仓（含 gateway.token）；声明
        工件内不得出现 gateway.token 字样。"""
        leaked = [str(p) for p in SERVER.rglob("openclaw.json") if p.is_file()]
        self.assertEqual(
            leaked, [],
            "PV-A6 RED: 仓内出现 openclaw.json（含 gateway.token secret）—— "
            "只入无 secret 的声明工件，整文件零入仓。实得："
            f"{leaked}")
        decl_text = _read_text(DECL_PATH)
        self.assertTrue(
            decl_text.strip(),
            "PV-A6 RED: 声明工件不在位/为空（前置）。")
        self.assertNotIn(
            "gateway.token", decl_text,
            "PV-A6 RED: 声明工件出现 gateway.token 字样 —— 声明面不得承载 secret 键。")
        self.assertIsNone(
            re.search(r'"token"\s*:', decl_text),
            "PV-A6 RED: 声明工件出现 token 键 —— 声明面不得承载 secret。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
