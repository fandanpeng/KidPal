"""Frozen acceptance tests — TaskPackage 261031-openclaw-cron-trigger-gating
（rigor=lite / riskTier=high / sourceType=human-ad-hoc；plan =
delivery/taskpackages/261031-openclaw-cron-trigger-gating/plan.md）

subsystem = OpenClaw cron 调度面：两条每 15 分钟 agentTurn cron（kidpal-reminder-
semantic / kidpal-schedule-health）改「变更驱动」——每次触发先跑一个**无模型、纯只读**
python 判定器，`fire:false` 时不起 agent 回合（内容没变则不花 token），`fire:true` 才跑
原 payload。承载 = 两个判定器 + 两个薄壳 JS + 幂等 apply/clear 工具 + 部署面（manifest /
verify-deploy / runbook / skills 镜像 / README）。

冻结信息
  changeId : 261031-openclaw-cron-trigger-gating
  冻结日期 : 2026-10-31（acceptance-author）
  覆盖 AC  : CRG-A1 / CRG-A2 / CRG-A3 / CRG-A4 / CRG-A5（CRG-A6 由红/绿基线对照体现，
             不在本件内重复跑全量——见末「Run protocol / 红基线」）
  写域     : 本件 = 本包唯一冻结验收写点；builder/fixer 只读，禁改断言语义（tighten-only
             见 docs/DEV-GOVERNANCE.md「验收测试纪律」§6）。

被测对象（builder 尚未创建 → 冻结时点全红 = 设计态红，逐条指向「本包实现未落地」）：
  · server/deploy/openclaw-patch/reminder_semantic_gate.py      （新，判定器 A）
  · server/deploy/openclaw-patch/schedule_health_gate.py        （新，判定器 B）
  · server/deploy/openclaw-patch/triggers/reminder-semantic.trigger.js （新，薄壳 A）
  · server/deploy/openclaw-patch/triggers/schedule-health.trigger.js   （新，薄壳 B）
  · server/deploy/openclaw-patch/cron_triggers.py               （新，幂等 apply/clear/status）
  · server/deploy/manifest.yaml                                 （既有：4 条 md5 + 2 条 expect_trigger_file）
  · server/deploy/verify-deploy.sh                              （既有：trigger 比对 + once + triggers.enabled 守卫）
  · server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md              （既有：§8.3d + --clear-trigger）
  · server/deploy/openclaw-skills/README.md                     （既有：两目 trigger 注记）
  · server/deploy/openclaw-skills/workspace-main/kidpal-reminder-semantic/SKILL.md （既有：trigger 段）
  · server/deploy/openclaw-skills/workspace-main/kidpal-schedule-health/SKILL.md   （既有：trigger 段）

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期零网络 I/O）
判据逐字承 plan.md「trigger 契约」节，本件把其钉成可执行断言。

共同输出契约（A、B 两判定器同形）
  · 独立可执行 python 脚本；**纯只读**（不写盘、不联网）。
  · stdout 恒为**严格单行** JSON object；键 = {fire: bool, message?: str, state?: obj}。
    fire 必须是真布尔（`type(x) is bool`；0/1 冒充 → 红）。
  · 正常 exit 0；**内部异常 exit≠0**（fail-loud：薄壳抛错 → 每轮 cron status=error，
    不允许静默降级成 fire:false 吞掉故障），且异常时不得输出「完整 fire JSON」
    （半截/陈旧结果被薄壳当判决 = 假绿）。
  · 输入全部经环境变量（均带 ECS 默认值）；本件只用 env 驱动（argv 覆写属 builder
    单测自由度，不在本件断言面）。

判定器 A：reminder_semantic_gate.py（CRG-A1 / CRG-A2）
  env：KIDPAL_SCHEDULE_MD / KIDPAL_SEMANTIC_JSON / KIDPAL_REMINDER_SEMANTIC_SKILL /
       KIDPAL_SCHEDULE_PARSER / KIDPAL_REMINDER_LIB / KIDPAL_SEMANTIC_GATE_MARGIN_SEC(60)
  fire:true 条件（命任一）：
    ① semantic.json 缺失；
    ② semantic.json 非法 JSON；
    ③ mtime(semantic) - mtime(schedule) <= margin(默认 60s)（读写竞态余量，边界含等于）；
    ④ mtime(semantic) <= mtime(skill) | mtime(gateway/schedule.py) | mtime(reminder_schedule_lib.py)
       （margin 0；边界**含等于**——semantic 与下游同 mtime 视为落后）；
  四输入全新鲜且 schedule 在场 → fire:false；
  schedule.md 缺失 → fire:false（**终局守卫**：skill 约定无课表不写、保留旧产物；即使
  下游输入变新也不 fire）。
  message 自包含：fire:true 时带触发原因（本件钉「message 含被牵连输入的语义 token」+ 不同
  原因 message 不恒等）。

判定器 B：schedule_health_gate.py（CRG-A3 / CRG-A4）
  env：KIDPAL_PARSE_ERROR_JSON / KIDPAL_PARSE_HEALTH_JSON / KIDPAL_HEALTH_STALE_SEC(180) /
       KIDPAL_HEALTH_FIRE_COOLDOWN_SEC(3600)
       + **KIDPAL_TRIGGER_STATE_JSON**（可选；上一轮冻结的 trigger.state 快照的 JSON 串 ——
         本件为 host 可测性钉定的输入通道，plan 未命名通道，见「缺口」第 3 条）。
  指纹 fingerprint = error + "|" + detail（**逐字**，与 kidpal-schedule-health skill 一致）。
  新鲜 = checked_at 距 now <= stale_sec(180)；checked_at 不可解析 → **退回文件 mtime**。
  checked_at 须支持两种 ISO 形态：带 `+00:00` 偏移 与 带 `Z` 后缀。
  真值（四分支，按序命中即止）：
    1. 告警件不在场：ack 不在 → fire:false；ack 在场 → fire:true（恢复动作，payload 清 ack）；
    2. 告警件在场但不新鲜：同 1（ack 在 → true 恢复；ack 不在 → false）；
    3. 新鲜且指纹 == ack 指纹 → fire:false；
    4. 新鲜且指纹不同（或 ack 缺失/损坏）→ fire:true（message 自包含：指纹 + detail +
       指明走 skill 分支 4）；
    5. 告警件在场但不可解析 → fire:true（宁可让 agent 看一眼也不静默；message 按 skill 边界处理）
  防刷屏保险丝：state.lastFire = {fingerprint, atMs}；同指纹且 atMs 落在
  fire_cooldown_sec 冷却窗内 → fire:false（仅抑制、非去重权威——去重权威仍是 ack 件）。
  state 仅承载反刷屏：含 lastFire.fingerprint + atMs；**不得**携带 ack 去重键
  （last_notified_fingerprint / last_notified_at）——门不得自造去重权威。

部署面（CRG-A5，静态锚 / 源锚代理）
  · manifest.yaml：4 条 md5 登记（2 判定器 + 2 薄壳 JS）；两条 cron_jobs 各加
    expect_trigger_file 指向对应 JS；原 expect_agent=main / expect_schedule=every 15m 不变。
  · verify-deploy.sh：消费 expect_trigger_file、含 cron.triggers.enabled 守卫、含 once 守卫。
  · DEPLOY-RUNBOOK.md：含 --clear-trigger 与 §8.3d 节。
  · 两份 SKILL.md + README：含 trigger 变更驱动段（reminder-semantic 另须落 60s 新鲜度合同）。
  · 两个薄壳 JS：exec 调**各自**判定器、返回 json({...fire...})、不串引用对方判定器。
  · cron_triggers.py：经 openclaw cron edit 加/去 trigger（--trigger-script / --clear-trigger /
    --dry-run / status），**不直写 openclaw.json**。

===========================================================================
强度申报（契约条目 → 承载断言 → 性质档）：
  · A 真值表（缺失/坏 JSON/竞态 60 边界两侧/落后 skill·parser·lib/全新鲜/schedule 缺失）
        → test_crg_a1_* → 行为级（真跑纯 python 判定器）
  · A 输出形态（单行/可 JSON.parse/fire 真布尔/message 含原因/内部异常 exit≠0 无半截 JSON）
        → test_crg_a2_* → 行为级（真跑）
  · B 真值表（四分支 + 恢复 + 指纹逐字 + 两种 ISO + 坏 checked_at 退 mtime + 冷却窗）
        → test_crg_a3_* → 行为级（真跑）
  · B 输出形态（单行/真布尔/message 含指纹+detail+分支4/state 仅反刷屏/内部异常 exit≠0）
        → test_crg_a4_* → 行为级（真跑）
  · 部署面 manifest/verify/runbook/SKILL/README/薄壳/cron_triggers 锚
        → test_crg_a5_* → 源锚代理（读源文本；非执行 bash / 非跑 openclaw CLI）
  · live L1-L7 证据齐 → test_live_* → 源锚代理（只读证据文件；真实 ECS 执行归主线程/human）

缺口（承载断言强度低于契约要求处，逐条；不猜、不静默）：
  1. verify-deploy.sh 的 trigger 比对/守卫是**源文本锚**，不执行 bash、不跑 openclaw CLI
     （离线无 ECS）；「盘上文本 vs job 内联文本逐字相等」的真行为归 live L2/L1。
  2. runbook 只钉 --clear-trigger 在场 + §8.3d 标题在场；§8.6 回滚段的具体位置与措辞不钉
     （语义/排版归 integrator 文档审）。
  3. **判定器 B 的「上一轮 state」输入通道 plan 未命名**（只说「读冻结的 trigger.state 只读
     快照」）。本件为 host 可测性钉定 `KIDPAL_TRIGGER_STATE_JSON`（JSON 串）——这是验收侧
     补的可测缝，非新增产品语义；builder 若用其它通道须与本件对齐后回 planner 说明。
  4. **内部异常触发面钉为「数值型 env 传入非数字」**（KIDPAL_SEMANTIC_GATE_MARGIN_SEC /
     KIDPAL_HEALTH_STALE_SEC = "not-a-number"）：契约只要求「内部异常 exit≠0」，本件选
     配置污染这一确定性触发点，禁静默回落默认值（fail-loud）。其余异常面（I/O 等）不逐一钉。
  5. **判定器 A「schedule 缺失 + semantic 也缺失」的组合不钉**：plan 只声明 schedule 缺失 →
     fire:false，未明示与条件①的先后；本件只钉「schedule 缺失时即便下游变新也不 fire」
     （可判别排序），不钉语义有歧义的叠加态。
  6. message 内容钉「含被牵连输入 token」+「不同原因不恒等」，不钉完整句式；分支 4 额外钉
     「含指纹 + detail + 提到分支 4」——措辞自由，语义不可删。
  7. 判定器 A 依赖的 mtime「读写竞态 60s 余量」精确边界用整数秒 mtime 钉（60 命中、61 不命中）；
     ECS 上真实 agent 读写时延的竞态是否被兜住归 live L3。
  8. 行为等价假设（显式化，不写即默认不成立）：
       - 「同 mtime」在 host fixture 用 os.utime 整数秒赋同一 epoch 实现；跨文件系统若
         亚秒精度被截断，等值比较仍成立（同值），不引入假红；
       - 「新鲜」以 checked_at/f mtime 相对**运行时刻 now** 判定，fixture 用 now 与 now-3600s
         两侧取值，不依赖墙钟对齐到秒；
       - 薄壳 JS 断言的是「引用正确判定器路径 + 调 exec + 返回 json(fire)」，不证 OpenClaw
         运行时真执行（黑盒，归 live L2）。

fixture 自含性声明：全部断言直读仓内制品 + 每测 tempfile 目录（文件内容与 mtime 内嵌本文件
生成）；判定器经 sys.executable 子进程真跑（强制 PYTHONUTF8=1 / PYTHONIOENCODING=utf-8 避免
Windows 控制台编码干扰）；零网络、零外部资产、零凭据。evidence 类扫描（manifest/verify/
runbook/SKILL/README/薄壳/live 证据）仓根由 __file__ 显式推导（REPO_ROOT / LIVE_DIR），不依赖
调用点 cwd。行为 seam 驱动方对账：判定器 ← 薄壳 exec（live）；薄壳 ← OpenClaw trigger 运行时
（黑盒，live）；manifest/verify ← 部署轮；SKILL ← cron payload 回合。host 只能证「判定器真值 +
静态接线」，真机 fire/exec 行为差集已逐条申报（上面缺口 1/3/8）。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" \
      "server/acceptance/test_261031_openclaw_cron_trigger_gating.acceptance.py"
  （standalone: python 本文件）

红基线（冻结时点 2026-10-31，builder 未动工；实测）：
  · 本件单跑：**25 failed / 0 passed / 2 deselected**（2 deselected = 本件两条 live，
    不 auto-run）。25 条 host 全设计态红，按缺失实现归组（逐条均指向「本包未落地」，
    无环境 / 测试自身错误）：
      - reminder_semantic_gate.py 不在位 → CRG-A1 ×6 / CRG-A2 ×2；
      - schedule_health_gate.py 不在位 → CRG-A3 ×10 / CRG-A4 ×1；
      - cron_triggers.py / 两个薄壳 JS 不在位 → CRG-A5 ×2；
      - manifest 未登记 4 件（reminder_semantic_gate.py not found）→ CRG-A5 ×1；
      - verify-deploy.sh 无 expect_trigger_file → CRG-A5 ×1；
      - DEPLOY-RUNBOOK 无 --clear-trigger → CRG-A5 ×1；
      - 两份 SKILL 无 trigger 段 → CRG-A5 ×1。
  · 全量 server/acceptance 对照上游基线（370 passed / 7 deselected / 33 subtests
    passed @ 93b92a6b）：**25 failed / 370 passed / 9 deselected / 33 subtests passed**
    —— 370 passed 恰等于基线零回退，25 条失败**全部**落在本件（CRG-A6 证据）。

REVISION LOG
  · R1（2026-10-31，planner 授权收紧型更正，据 builder 实测报告）：
    - 旧形态：CronTriggerGatingAcceptance._norm 用 `str(p).replace("\\","/").lstrip("./")`；
      `_norm(entry.get("dst"))` 与未归一化的 `/opt/kidpal/...` 直接比较。
    - 新形态：`_norm` 只剥字面前缀 `"./"`（`s[2:] if s.startswith("./") else s`），保留
      前导 `/`；归一化后与 `/opt/kidpal/...` 可比。
    - 理由：`.lstrip("./")` 是**字符集**语义（剥任意前导 '.'/'/'），会把 ECS 绝对路径的
      前导 '/' 一并剥掉（实测 `'/opt/x'.lstrip('./') == 'opt/x'`），任何合法 manifest dst
      恒不满足；`startswith("./")` 才是**字面前缀**语义。仅此一处更正，未放宽其它断言、
      未改实现。
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
import tempfile
import time
import unittest
from datetime import datetime, timedelta, timezone
from pathlib import Path

try:
    import yaml  # manifest 断言前置；缺席 = infra 红
    _HAS_YAML = True
except ImportError:  # pragma: no cover
    _HAS_YAML = False

try:
    import pytest  # live marker
    _HAS_PYTEST = True
except ImportError:  # pragma: no cover
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY = SERVER / "deploy"
PATCH_DIR = DEPLOY / "openclaw-patch"
TRIGGERS_DIR = PATCH_DIR / "triggers"

GATE_A = PATCH_DIR / "reminder_semantic_gate.py"
GATE_B = PATCH_DIR / "schedule_health_gate.py"
TRIGGER_A = TRIGGERS_DIR / "reminder-semantic.trigger.js"
TRIGGER_B = TRIGGERS_DIR / "schedule-health.trigger.js"
CRON_TRIGGERS = PATCH_DIR / "cron_triggers.py"

MANIFEST = DEPLOY / "manifest.yaml"
VERIFY = DEPLOY / "verify-deploy.sh"
RUNBOOK = PATCH_DIR / "DEPLOY-RUNBOOK.md"
SKILLS_README = DEPLOY / "openclaw-skills" / "README.md"
SKILL_A = DEPLOY / "openclaw-skills" / "workspace-main" / "kidpal-reminder-semantic" / "SKILL.md"
SKILL_B = DEPLOY / "openclaw-skills" / "workspace-main" / "kidpal-schedule-health" / "SKILL.md"

CHANGE_ID = "261031-openclaw-cron-trigger-gating"
LIVE_DIR = REPO_ROOT / "delivery" / "taskpackages" / CHANGE_ID

GATE_A_NAME = "reminder_semantic_gate.py"
GATE_B_NAME = "schedule_health_gate.py"
TRIGGER_A_NAME = "reminder-semantic.trigger.js"
TRIGGER_B_NAME = "schedule-health.trigger.js"

# 整数秒 mtime 基准（2023-11-14）；全部 mtime 以整数秒赋值，避免亚秒噪声污染边界
BASE_EPOCH = 1_700_000_000

_MISSING = object()


# --------------------------------------------------------------------------- #
# 读盘 / 文件 / 子进程 helpers（仓根由 __file__ 显式推导，不依赖调用点 cwd）
# --------------------------------------------------------------------------- #

def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _write(path: Path, text: str) -> Path:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")
    return path


def _touch(path: Path, epoch: float) -> Path:
    os.utime(str(path), (epoch, epoch))
    return path


def _mk(td: Path, name: str, text: str, epoch: float) -> Path:
    return _touch(_write(td / name, text), epoch)


def _run_gate(script: Path, overrides: dict):
    """子进程真跑判定器；overrides 为 env 覆写（值可为 Path/str/int）。"""
    if not script.is_file():
        return None
    env = dict(os.environ)
    env.update({k: str(v) for k, v in overrides.items()})
    env["PYTHONUTF8"] = "1"
    env["PYTHONIOENCODING"] = "utf-8"
    return subprocess.run(
        [sys.executable, str(script)],
        env=env, capture_output=True, text=True,
        encoding="utf-8", errors="replace", timeout=90,
    )


def _is_complete_fire(text: str) -> bool:
    """stdout 是否为「完整、带真布尔 fire 的判决 JSON」（用于内部异常不得输出半截结果）。"""
    try:
        obj = json.loads((text or "").strip())
    except ValueError:
        return False
    return isinstance(obj, dict) and type(obj.get("fire")) is bool


def _now() -> datetime:
    return datetime.now(timezone.utc)


def _fp(error: str, detail: str) -> str:
    return error + "|" + detail


def _mentions_branch4(msg: str) -> bool:
    low = msg.lower()
    return ("分支" in msg and "4" in msg) or ("branch" in low and "4" in low)


# ---- 判定器 A fixture ------------------------------------------------------ #

def _env_a(td: Path, margin=None) -> dict:
    e = {
        "KIDPAL_SCHEDULE_MD": td / "schedule.md",
        "KIDPAL_SEMANTIC_JSON": td / "semantic.json",
        "KIDPAL_REMINDER_SEMANTIC_SKILL": td / "SKILL.md",
        "KIDPAL_SCHEDULE_PARSER": td / "schedule.py",
        "KIDPAL_REMINDER_LIB": td / "reminder_schedule_lib.py",
    }
    if margin is not None:
        e["KIDPAL_SEMANTIC_GATE_MARGIN_SEC"] = margin
    return e


def _gate_a_fixture(
    td: Path, *, schedule_ok: bool = True, semantic="{}",
    semantic_mtime: float = BASE_EPOCH, schedule_mtime: float = BASE_EPOCH - 3600,
    skill_mtime: float = BASE_EPOCH - 1800, parser_mtime: float = BASE_EPOCH - 1800,
    lib_mtime: float = BASE_EPOCH - 1800,
) -> None:
    """铺判定器 A 的四输入文件 + mtime；semantic=None → 不建 semantic.json。"""
    if schedule_ok:
        _mk(td, "schedule.md", "# 课表\n", schedule_mtime)
    if semantic is not None:
        _mk(td, "semantic.json", semantic, semantic_mtime)
    _mk(td, "SKILL.md", "skill", skill_mtime)
    _mk(td, "schedule.py", "parser", parser_mtime)
    _mk(td, "reminder_schedule_lib.py", "lib", lib_mtime)


# ---- 判定器 B fixture ------------------------------------------------------ #

def _env_b(td: Path, *, stale=None, cooldown=None, state=None) -> dict:
    e = {
        "KIDPAL_PARSE_ERROR_JSON": td / "schedule-parse-error.json",
        "KIDPAL_PARSE_HEALTH_JSON": td / "schedule-parse-health.json",
    }
    if stale is not None:
        e["KIDPAL_HEALTH_STALE_SEC"] = stale
    if cooldown is not None:
        e["KIDPAL_HEALTH_FIRE_COOLDOWN_SEC"] = cooldown
    if state is not None:
        e["KIDPAL_TRIGGER_STATE_JSON"] = json.dumps(state, ensure_ascii=False)
    return e


def _error_file(
    td: Path, checked_at, *, error="ScheduleParseError",
    detail="日期列格式错误: 2026-10-32 周一", mtime=None, raw=None,
) -> Path:
    p = td / "schedule-parse-error.json"
    if raw is not None:
        _write(p, raw)
    else:
        _write(p, json.dumps(
            {"checked_at": checked_at, "error": error, "detail": detail},
            ensure_ascii=False))
    if mtime is not None:
        _touch(p, mtime)
    return p


def _ack_file(td: Path, fp: str, *, raw=None) -> Path:
    p = td / "schedule-parse-health.json"
    if raw is not None:
        _write(p, raw)
    else:
        _write(p, json.dumps(
            {"last_notified_fingerprint": fp,
             "last_notified_at": _now().isoformat()},
            ensure_ascii=False))
    return p


# --------------------------------------------------------------------------- #
# 主验收（host，offline）
# --------------------------------------------------------------------------- #

class CronTriggerGatingAcceptance(unittest.TestCase):

    maxDiff = None

    # -- 断言 helpers ----------------------------------------------------- #

    def _gate_json(self, script: Path, overrides: dict, tag: str) -> dict:
        """真跑判定器 → 断言正常形态（rc0 / 严格单行 / JSON object / fire 真布尔）→ 返回 obj。"""
        if not script.is_file():
            self.fail(
                f"{tag} RED: 判定器不在位 —— 冻结契约要求 {script}（builder 未实现，设计态红）。")
        proc = _run_gate(script, overrides)
        out = proc.stdout or ""
        self.assertEqual(
            proc.returncode, 0,
            f"{tag} RED: 判定器 rc={proc.returncode}（正常应 0）。"
            f"stdout={out!r} stderr={(proc.stderr or '')!r}")
        nonempty = [ln for ln in out.splitlines() if ln.strip()]
        self.assertEqual(
            len(nonempty), 1,
            f"{tag} RED: stdout 非严格单行（{len(nonempty)} 个非空行）：{out!r}")
        try:
            obj = json.loads(nonempty[0])
        except ValueError as e:  # noqa: BLE001
            self.fail(f"{tag} RED: stdout 非法 JSON：{nonempty[0]!r}（{e}）")
        self.assertIsInstance(obj, dict, f"{tag} RED: stdout JSON 非 object：{obj!r}")
        self.assertIn("fire", obj, f"{tag} RED: 输出缺 fire 键：{obj!r}")
        self.assertIs(
            type(obj["fire"]), bool,
            f"{tag} RED: fire 非真布尔（实得 {type(obj['fire']).__name__}={obj['fire']!r}）。")
        return obj

    def _msg(self, obj: dict, tag: str, tokens) -> str:
        self.assertIn("message", obj, f"{tag} RED: fire:true 但缺 message（自包含触发原因）。")
        msg = obj["message"]
        self.assertIsInstance(msg, str, f"{tag} RED: message 非字符串：{msg!r}")
        self.assertTrue(msg.strip(), f"{tag} RED: message 为空（须自包含触发原因）。")
        low = msg.lower()
        self.assertTrue(
            any(t.lower() in low for t in tokens),
            f"{tag} RED: message 未含触发原因 token {list(tokens)}；message={msg!r}")
        return msg

    def _assert_false(self, obj: dict, tag: str) -> None:
        self.assertIs(obj["fire"], False, f"{tag} RED: 期望 fire:false，实得 fire={obj['fire']!r}（{obj!r}）。")

    def _assert_true(self, obj: dict, tag: str) -> None:
        self.assertIs(obj["fire"], True, f"{tag} RED: 期望 fire:true，实得 fire={obj['fire']!r}（{obj!r}）。")

    # ================================================================== #
    # CRG-A1 · 判定器 A 真值表
    # ================================================================== #

    def test_crg_a1_semantic_missing_fires(self):
        """coverage: CRG-A1 — semantic.json 缺失 → fire:true；message 自包含（语义产物）。"""
        tag = "CRG-A1 semantic-missing"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(td, semantic=None)
            obj = self._gate_json(GATE_A, _env_a(td), tag)
            self._assert_true(obj, tag)
            self._msg(obj, tag, ["semantic", "语义"])

    def test_crg_a1_invalid_json_fires(self):
        """coverage: CRG-A1 — semantic.json 非法 JSON → fire:true。"""
        tag = "CRG-A1 invalid-json"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(td, semantic='{"broken": ', semantic_mtime=BASE_EPOCH)
            obj = self._gate_json(GATE_A, _env_a(td), tag)
            self._assert_true(obj, tag)
            self._msg(obj, tag, ["semantic", "语义", "json"])

    def test_crg_a1_schedule_race_boundary(self):
        """coverage: CRG-A1 — 读写竞态余量边界：mtime(semantic)-mtime(schedule)==60 → fire:true；
        61（其余输入全新鲜）→ fire:false（margin 含等于）。"""
        tag = "CRG-A1 race-boundary"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(
                td, semantic="{}", semantic_mtime=BASE_EPOCH,
                schedule_mtime=BASE_EPOCH - 60, skill_mtime=BASE_EPOCH - 1000,
                parser_mtime=BASE_EPOCH - 1000, lib_mtime=BASE_EPOCH - 1000)
            obj = self._gate_json(GATE_A, _env_a(td), tag)
            self._assert_true(obj, tag)  # 边界 60 命中
            self._msg(obj, tag, ["schedule", "课表"])
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(
                td, semantic="{}", semantic_mtime=BASE_EPOCH,
                schedule_mtime=BASE_EPOCH - 61, skill_mtime=BASE_EPOCH - 1000,
                parser_mtime=BASE_EPOCH - 1000, lib_mtime=BASE_EPOCH - 1000)
            obj = self._gate_json(GATE_A, _env_a(td), tag + " (61s)")
            self._assert_false(obj, tag + " (61s)")  # 61s 越界 → 全新鲜不 fire

    def test_crg_a1_downstream_newer_fires(self):
        """coverage: CRG-A1 — semantic 落后 skill / parser / lib 任一 → fire:true；
        skill 侧用「mtime 相等」钉 <= 边界（等值视为落后）。"""
        tag = "CRG-A1 downstream-newer"
        # skill 同 mtime（等值触发 <=）
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(
                td, semantic="{}", semantic_mtime=BASE_EPOCH,
                schedule_mtime=BASE_EPOCH - 3600, skill_mtime=BASE_EPOCH,
                parser_mtime=BASE_EPOCH - 1800, lib_mtime=BASE_EPOCH - 1800)
            obj = self._gate_json(GATE_A, _env_a(td), tag + " skill=")
            self._assert_true(obj, tag + " skill=")
            self._msg(obj, tag + " skill=", ["skill", "技能"])
        # parser 更新
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(
                td, semantic="{}", semantic_mtime=BASE_EPOCH,
                schedule_mtime=BASE_EPOCH - 3600, skill_mtime=BASE_EPOCH - 1800,
                parser_mtime=BASE_EPOCH + 30, lib_mtime=BASE_EPOCH - 1800)
            obj = self._gate_json(GATE_A, _env_a(td), tag + " parser")
            self._assert_true(obj, tag + " parser")
            self._msg(obj, tag + " parser", ["schedule.py", "parser", "解析"])
        # lib 更新
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(
                td, semantic="{}", semantic_mtime=BASE_EPOCH,
                schedule_mtime=BASE_EPOCH - 3600, skill_mtime=BASE_EPOCH - 1800,
                parser_mtime=BASE_EPOCH - 1800, lib_mtime=BASE_EPOCH + 30)
            obj = self._gate_json(GATE_A, _env_a(td), tag + " lib")
            self._assert_true(obj, tag + " lib")
            self._msg(obj, tag + " lib", ["lib", "reminder", "排程", "库"])

    def test_crg_a1_all_fresh_no_fire(self):
        """coverage: CRG-A1 — 四输入全新鲜（semantic 新于 schedule 超 margin 且新于下游）→ fire:false。"""
        tag = "CRG-A1 all-fresh"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(
                td, semantic="{}", semantic_mtime=BASE_EPOCH,
                schedule_mtime=BASE_EPOCH - 3600, skill_mtime=BASE_EPOCH - 1800,
                parser_mtime=BASE_EPOCH - 1800, lib_mtime=BASE_EPOCH - 1800)
            obj = self._gate_json(GATE_A, _env_a(td), tag)
            self._assert_false(obj, tag)

    def test_crg_a1_schedule_missing_no_fire(self):
        """coverage: CRG-A1 — schedule.md 缺失 → fire:false（终局守卫：即便下游 skill 变新也不 fire，
        无课表不产语义产物）。"""
        tag = "CRG-A1 schedule-missing"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            # skill 比 semantic 新——若无终局守卫，条件④会 fire；守卫须先判 schedule 缺失
            _gate_a_fixture(
                td, schedule_ok=False, semantic="{}", semantic_mtime=BASE_EPOCH,
                skill_mtime=BASE_EPOCH + 30, parser_mtime=BASE_EPOCH - 1800,
                lib_mtime=BASE_EPOCH - 1800)
            obj = self._gate_json(GATE_A, _env_a(td), tag)
            self._assert_false(obj, tag)

    # ================================================================== #
    # CRG-A2 · 判定器 A 输出形态 / 内部异常
    # ================================================================== #

    def test_crg_a2_output_shape_single_line_bool_message(self):
        """coverage: CRG-A2 — stdout 严格单行、可 JSON 解析、fire 真布尔、message 非空且
        不同触发原因的 message 不恒等（含触发原因）。"""
        tag = "CRG-A2 shape"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(
                td, semantic="{}", semantic_mtime=BASE_EPOCH,
                schedule_mtime=BASE_EPOCH - 60, skill_mtime=BASE_EPOCH - 1000,
                parser_mtime=BASE_EPOCH - 1000, lib_mtime=BASE_EPOCH - 1000)
            obj = self._gate_json(GATE_A, _env_a(td), tag)
            self.assertIs(type(obj["fire"]), bool, f"{tag} RED: fire 非真布尔。")
            msg_race = self._msg(obj, tag, ["schedule", "课表"])
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(
                td, semantic="{}", semantic_mtime=BASE_EPOCH,
                schedule_mtime=BASE_EPOCH - 3600, skill_mtime=BASE_EPOCH + 30,
                parser_mtime=BASE_EPOCH - 1800, lib_mtime=BASE_EPOCH - 1800)
            obj2 = self._gate_json(GATE_A, _env_a(td), tag + " skill")
            msg_skill = self._msg(obj2, tag + " skill", ["skill", "技能"])
        self.assertNotEqual(
            msg_race, msg_skill,
            "CRG-A2 RED: 竞态与 skill 变更两类原因的 message 恒等 —— 未含可辨触发原因，"
            "不符合 message 自包含。")

    def test_crg_a2_internal_error_exit_nonzero_no_half_json(self):
        """coverage: CRG-A2 — 内部异常（数值型 env 传入非数字）→ exit≠0 且不输出完整 fire JSON
        （fail-loud，禁静默回落吞故障）。"""
        tag = "CRG-A2 internal-error"
        if not GATE_A.is_file():
            self.fail(f"{tag} RED: 判定器不在位 —— 冻结契约要求 {GATE_A}（设计态红）。")
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _gate_a_fixture(td, semantic="{}", semantic_mtime=BASE_EPOCH,
                            schedule_mtime=BASE_EPOCH - 3600)
            proc = _run_gate(GATE_A, _env_a(td, margin="not-a-number"))
            out = proc.stdout or ""
            self.assertNotEqual(
                proc.returncode, 0,
                f"{tag} RED: 非法 KIDPAL_SEMANTIC_GATE_MARGIN_SEC 应内部异常 exit≠0（fail-loud），"
                f"实得 rc=0 stdout={out!r} —— 不得静默回落默认值吞掉配置故障。")
            self.assertFalse(
                _is_complete_fire(out),
                f"{tag} RED: 内部异常时仍输出了完整 fire 判决 JSON（薄壳会误当判决）：{out!r}")

    # ================================================================== #
    # CRG-A3 · 判定器 B 真值表
    # ================================================================== #

    def test_crg_a3_absent_alert_no_ack_no_fire(self):
        """coverage: CRG-A3 分支1 — 告警件不在场 + ack 不在 → fire:false。"""
        tag = "CRG-A3 absent/no-ack"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            obj = self._gate_json(GATE_B, _env_b(td), tag)
            self._assert_false(obj, tag)

    def test_crg_a3_absent_alert_with_ack_recovery_fires(self):
        """coverage: CRG-A3 分支1 — 告警件不在场 + ack 在场 → fire:true（恢复动作，payload 清 ack）。"""
        tag = "CRG-A3 absent/ack-recovery"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _ack_file(td, "OldError|old detail")
            obj = self._gate_json(GATE_B, _env_b(td), tag)
            self._assert_true(obj, tag)
            self.assertIn("message", obj, f"{tag} RED: 恢复动作 fire:true 缺 message。")
            self.assertTrue(str(obj["message"]).strip(),
                            f"{tag} RED: 恢复动作 message 为空。")

    def test_crg_a3_stale_alert_recovery_two_sided(self):
        """coverage: CRG-A3 分支2 — 告警件不新鲜：ack 在 → fire:true（恢复）；ack 不在 → fire:false。"""
        tag = "CRG-A3 stale"
        stale_ct = (_now() - timedelta(seconds=3600)).isoformat()
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, stale_ct)
            _ack_file(td, "OldError|old detail")
            obj = self._gate_json(GATE_B, _env_b(td), tag + "/ack")
            self._assert_true(obj, tag + "/ack")
            self.assertTrue(str(obj.get("message", "")).strip(),
                            f"{tag}/ack RED: 恢复动作缺 message。")
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, stale_ct)
            obj = self._gate_json(GATE_B, _env_b(td), tag + "/no-ack")
            self._assert_false(obj, tag + "/no-ack")

    def test_crg_a3_fresh_same_fingerprint_suppressed(self):
        """coverage: CRG-A3 分支3 — 新鲜告警且指纹 == ack 指纹 → fire:false。"""
        tag = "CRG-A3 fresh/same-fp"
        err, det = "ScheduleParseError", "日期列格式错误: 2026-10-32 周一"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, _now().isoformat(), error=err, detail=det)
            _ack_file(td, _fp(err, det))
            obj = self._gate_json(GATE_B, _env_b(td), tag)
            self._assert_false(obj, tag)

    def test_crg_a3_fresh_new_fingerprint_fires_with_message_and_state(self):
        """coverage: CRG-A3 分支4 — 新鲜且新指纹 → fire:true；指纹逐字 == error|detail；
        message 自包含（指纹 + detail + 分支4）；state.lastFire.fingerprint 逐字同指纹。"""
        tag = "CRG-A3 fresh/new-fp"
        err, det = "ScheduleParseError", "日期列格式错误: 2026-10-32 周一"
        fp = _fp(err, det)
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, _now().isoformat(), error=err, detail=det)
            _ack_file(td, "OtherError|different detail")
            obj = self._gate_json(GATE_B, _env_b(td), tag)
            self._assert_true(obj, tag)
            msg = self._msg(obj, tag, [fp])
            self.assertIn(det, msg, f"{tag} RED: message 未含 detail 原文（自包含失败）：{msg!r}")
            self.assertTrue(_mentions_branch4(msg),
                            f"{tag} RED: message 未指明走 skill 分支 4（自包含失败）：{msg!r}")
            state = obj.get("state")
            self.assertIsInstance(state, dict, f"{tag} RED: fire:true 缺 state 反刷屏快照：{obj!r}")
            last_fire = state.get("lastFire")
            self.assertIsInstance(last_fire, dict, f"{tag} RED: state 缺 lastFire：{state!r}")
            self.assertEqual(
                last_fire.get("fingerprint"), fp,
                f"{tag} RED: state.lastFire.fingerprint={last_fire.get('fingerprint')!r} "
                f"≠ 逐字指纹 {fp!r}。")

    def test_crg_a3_ack_missing_or_corrupt_fires(self):
        """coverage: CRG-A3 分支4 — 新鲜新指纹 + ack 缺失 / ack 损坏 → 均 fire:true。"""
        tag = "CRG-A3 ack"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, _now().isoformat())
            obj = self._gate_json(GATE_B, _env_b(td), tag + "/missing")
            self._assert_true(obj, tag + "/missing")
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, _now().isoformat())
            _ack_file(td, "x|y", raw="not json {")
            obj = self._gate_json(GATE_B, _env_b(td), tag + "/corrupt")
            self._assert_true(obj, tag + "/corrupt")

    def test_crg_a3_checked_at_two_iso_forms(self):
        """coverage: CRG-A3 — checked_at 两种 ISO 形态（+00:00 偏移 / Z 后缀）均按新鲜解析
        （新鲜 + 同指纹 → fire:false，反证解析正确而非误判过期）。"""
        tag = "CRG-A3 iso-forms"
        err, det = "ScheduleParseError", "坏 token 2026-10-32"
        fp = _fp(err, det)
        now = _now()
        forms = [now.isoformat(timespec="seconds"), now.strftime("%Y-%m-%dT%H:%M:%SZ")]
        for ct in forms:
            with tempfile.TemporaryDirectory() as d:
                td = Path(d)
                _error_file(td, ct, error=err, detail=det)
                _ack_file(td, fp)
                obj = self._gate_json(GATE_B, _env_b(td), f"{tag}[{ct}]")
                self._assert_false(obj, f"{tag}[{ct}]")

    def test_crg_a3_bad_checked_at_falls_back_to_mtime(self):
        """coverage: CRG-A3 — checked_at 不可解析 → 退回文件 mtime；mtime=now → 按新鲜处理
        （新鲜 + 同指纹 → fire:false，反证「退 mtime」而非误判过期或崩溃）。"""
        tag = "CRG-A3 bad-checked_at"
        err, det = "ScheduleParseError", "坏 token 2026-10-32"
        fp = _fp(err, det)
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, "not-a-timestamp", error=err, detail=det,
                        mtime=int(time.time()))
            _ack_file(td, fp)
            obj = self._gate_json(GATE_B, _env_b(td), tag)
            self._assert_false(obj, tag)

    def test_crg_a3_unparseable_alert_fires(self):
        """coverage: CRG-A3 分支5 — 告警件在场但不可解析 → fire:true（宁可让 agent 看一眼不静默）。"""
        tag = "CRG-A3 unparseable-alert"
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, None, raw="not json {")
            obj = self._gate_json(GATE_B, _env_b(td), tag)
            self._assert_true(obj, tag)
            self._msg(obj, tag, ["解析", "parse", "边界", "invalid", "无法", "不可"])

    def test_crg_a3_cooldown_window_suppresses_and_expires(self):
        """coverage: CRG-A3 — 防刷屏：同指纹在冷却窗内（state.lastFire.atMs 新）→ 抑制 fire:false；
        越出冷却窗（atMs 旧）→ fire:true。ack 用不同指纹以排除分支3去重。"""
        tag = "CRG-A3 cooldown"
        err, det = "ScheduleParseError", "坏 token 2026-10-32"
        fp = _fp(err, det)
        now_ms = int(time.time() * 1000)
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, _now().isoformat(), error=err, detail=det)
            _ack_file(td, "OtherError|different detail")
            recent = {"lastFire": {"fingerprint": fp, "atMs": now_ms - 60_000}}  # 60s 前
            obj = self._gate_json(
                GATE_B, _env_b(td, cooldown=3600, state=recent), tag + "/within")
            self._assert_false(obj, tag + "/within")
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, _now().isoformat(), error=err, detail=det)
            _ack_file(td, "OtherError|different detail")
            old = {"lastFire": {"fingerprint": fp, "atMs": now_ms - 2 * 3600 * 1000}}  # 2h 前
            obj = self._gate_json(
                GATE_B, _env_b(td, cooldown=3600, state=old), tag + "/expired")
            self._assert_true(obj, tag + "/expired")

    # ================================================================== #
    # CRG-A4 · 判定器 B 输出形态 / state 仅反刷屏 / 内部异常
    # ================================================================== #

    def test_crg_a4_output_shape_state_antispam_and_internal_error(self):
        """coverage: CRG-A4 — 输出单行 / fire 真布尔 / 分支4 message 含指纹+detail+分支4 /
        state 仅反刷屏（含 lastFire.fingerprint+atMs，不含 ack 去重键）/ 内部异常 exit≠0 无半截 JSON。"""
        tag = "CRG-A4 shape"
        err, det = "ScheduleParseError", "日期列格式错误: 2026-10-32 周一"
        fp = _fp(err, det)
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, _now().isoformat(), error=err, detail=det)
            obj = self._gate_json(GATE_B, _env_b(td), tag)
            self.assertIs(type(obj["fire"]), bool, f"{tag} RED: fire 非真布尔。")
            msg = self._msg(obj, tag, [fp])
            self.assertIn(det, msg, f"{tag} RED: message 缺 detail 原文。")
            self.assertTrue(_mentions_branch4(msg), f"{tag} RED: message 未提分支 4。")
            state = obj.get("state")
            self.assertIsInstance(state, dict, f"{tag} RED: 缺 state。")
            self.assertNotIn(
                "last_notified_fingerprint", state,
                f"{tag} RED: state 携带 ack 去重键 last_notified_fingerprint —— 门不得自造"
                "去重权威（去重权威 = ack 件）。")
            self.assertNotIn(
                "last_notified_at", state,
                f"{tag} RED: state 携带 ack 去重键 last_notified_at。")
            last_fire = state.get("lastFire")
            self.assertIsInstance(last_fire, dict, f"{tag} RED: state 缺 lastFire。")
            self.assertEqual(last_fire.get("fingerprint"), fp,
                             f"{tag} RED: lastFire.fingerprint 非逐字指纹。")
            at_ms = last_fire.get("atMs")
            self.assertIsInstance(at_ms, (int, float), f"{tag} RED: lastFire.atMs 非数值：{at_ms!r}")
            self.assertFalse(isinstance(at_ms, bool), f"{tag} RED: lastFire.atMs 为 bool。")
            self.assertGreater(at_ms, 0, f"{tag} RED: lastFire.atMs 非正数。")

        # 内部异常 exit≠0 且无完整 fire JSON
        if not GATE_B.is_file():
            self.fail(f"{tag} RED: 判定器不在位 —— 冻结契约要求 {GATE_B}（设计态红）。")
        with tempfile.TemporaryDirectory() as d:
            td = Path(d)
            _error_file(td, _now().isoformat(), error=err, detail=det)
            proc = _run_gate(GATE_B, _env_b(td, stale="not-a-number"))
            out = proc.stdout or ""
            self.assertNotEqual(
                proc.returncode, 0,
                f"{tag} RED: 非法 KIDPAL_HEALTH_STALE_SEC 应内部异常 exit≠0（fail-loud），"
                f"实得 rc=0 stdout={out!r}。")
            self.assertFalse(
                _is_complete_fire(out),
                f"{tag} RED: 内部异常时仍输出完整 fire 判决 JSON：{out!r}")

    # ================================================================== #
    # CRG-A5 · 部署面静态锚（源锚代理）
    # ================================================================== #

    def _manifest_doc(self, tag: str) -> dict:
        if not _HAS_YAML:
            self.fail(f"{tag} RED(infra): PyYAML 不在位 —— manifest 解析前置缺失。")
        self.assertTrue(MANIFEST.is_file(), f"{tag} RED: {MANIFEST} 不在场。")
        try:
            return yaml.safe_load(MANIFEST.read_text(encoding="utf-8")) or {}
        except Exception as e:  # noqa: BLE001
            self.fail(f"{tag} RED: manifest.yaml 解析失败: {e!r}")

    @staticmethod
    def _norm(p) -> str:
        # 只剥字面前缀 "./"，保留前导 "/"：`.lstrip("./")` 是字符集语义，会把 ECS 绝对
        # 路径的前导 "/" 一并剥掉（'/opt/x' -> 'opt/x'），令合法 manifest dst 恒不满足。
        s = str(p or "").replace("\\", "/")
        return s[2:] if s.startswith("./") else s

    def test_crg_a5_manifest_trigger_entries_and_cron_jobs(self):
        """coverage: CRG-A5 — manifest: 4 条 md5 登记（2 判定器 + 2 薄壳 JS）；两条 cron_jobs
        各加 expect_trigger_file 指向对应 JS，且原 agent=main / schedule=every 15m 不变。"""
        tag = "CRG-A5 manifest"
        doc = self._manifest_doc(tag)
        entries = [e for e in (doc.get("entries") or []) if isinstance(e, dict)]
        by_src = {self._norm(e.get("src")): e for e in entries}
        required = {
            "server/deploy/openclaw-patch/reminder_semantic_gate.py":
                "/opt/kidpal/openclaw-patch/reminder_semantic_gate.py",
            "server/deploy/openclaw-patch/schedule_health_gate.py":
                "/opt/kidpal/openclaw-patch/schedule_health_gate.py",
            "server/deploy/openclaw-patch/triggers/reminder-semantic.trigger.js":
                "/opt/kidpal/openclaw-patch/triggers/reminder-semantic.trigger.js",
            "server/deploy/openclaw-patch/triggers/schedule-health.trigger.js":
                "/opt/kidpal/openclaw-patch/triggers/schedule-health.trigger.js",
        }
        for src, dst in required.items():
            self.assertIn(
                src, by_src,
                f"{tag} RED: manifest 未登记 {src} —— 新增部署资产必须接入 verify-deploy"
                "（261013 单源纪律；本包「4 条 md5 条目」）。")
            entry = by_src[src]
            self.assertEqual(self._norm(entry.get("dst")), dst,
                             f"{tag} RED: {src} 的 dst={entry.get('dst')!r} 应为 {dst!r}。")
            self.assertEqual(str(entry.get("verify")), "md5",
                             f"{tag} RED: {src} verify 应 md5。")
            self.assertEqual(str(entry.get("owner")), "deploy.sh",
                             f"{tag} RED: {src} owner 应 deploy.sh。")
            self.assertTrue((REPO_ROOT / src).is_file(),
                            f"{tag} RED: 登记的仓内源缺失：{src}。")

        jobs = {j.get("name"): j for j in (doc.get("cron_jobs") or []) if isinstance(j, dict)}
        for name, trig_name in (
                ("kidpal-reminder-semantic", TRIGGER_A_NAME),
                ("kidpal-schedule-health", TRIGGER_B_NAME)):
            self.assertIn(name, jobs, f"{tag} RED: manifest cron_jobs 缺 {name}。")
            job = jobs[name]
            self.assertIn("expect_trigger_file", job,
                          f"{tag} RED: cron_jobs[{name}] 缺 expect_trigger_file —— verify 无"
                          "触发比对输入。")
            trig_path = str(job["expect_trigger_file"])
            self.assertEqual(
                Path(self._norm(trig_path)).name, trig_name,
                f"{tag} RED: {name} 的 expect_trigger_file={trig_path!r} 应指向 {trig_name}。")
            # 频率 / agent 不变（CRG-E1：只加 trigger，不动 every 15m / main）
            self.assertEqual(str(job.get("expect_agent")), "main",
                             f"{tag} RED: {name} expect_agent 变了（应为 main）。")
            self.assertEqual(str(job.get("expect_schedule")), "every 15m",
                             f"{tag} RED: {name} expect_schedule 变了（应为 every 15m）。")

    def test_crg_a5_verify_deploy_trigger_guards(self):
        """coverage: CRG-A5 — verify-deploy.sh 消费 expect_trigger_file（内联文本比对接缝）
        且含 cron.triggers.enabled 守卫与 once 守卫。"""
        tag = "CRG-A5 verify-deploy"
        src = _read_text(VERIFY)
        self.assertTrue(src.strip(), f"{tag} RED: {VERIFY} 不在场或为空。")
        for token in ("expect_trigger_file", "triggers.enabled", "once"):
            self.assertIn(
                token, src,
                f"{tag} RED: verify-deploy.sh 缺 {token!r} —— 冻结契约要求 cron 段增"
                "内联 trigger 文本比对 + once 非 true 守卫 + cron.triggers.enabled 守卫。")
        self.assertIn("trigger", src.lower(),
                      f"{tag} RED: verify-deploy.sh 无 trigger 相关核验字样。")

    def test_crg_a5_runbook_clear_trigger_and_8_3d(self):
        """coverage: CRG-A5 — DEPLOY-RUNBOOK 含 §8.3d 节 + --clear-trigger 一键回滚命令。"""
        tag = "CRG-A5 runbook"
        body = _read_text(RUNBOOK)
        self.assertTrue(body.strip(), f"{tag} RED: {RUNBOOK} 不在场或为空。")
        self.assertIn("--clear-trigger", body,
                      f"{tag} RED: runbook 无 --clear-trigger（一步回滚命令缺失）。")
        self.assertIsNotNone(
            re.search(r"(?m)^#+\s*8\.3d", body),
            f"{tag} RED: runbook 无 §8.3d 节标题（变更驱动说明 + 所有权表 + 观测/回滚命令）。")

    def test_crg_a5_skill_and_readme_trigger_sections(self):
        """coverage: CRG-A5 — 两份 SKILL.md 与 skills README 含 trigger 变更驱动段；
        reminder-semantic 另落 60s 新鲜度合同。"""
        tag = "CRG-A5 skills"
        a = _read_text(SKILL_A)
        self.assertTrue(a.strip(), f"{tag} RED: {SKILL_A} 不在场。")
        self.assertIn("trigger", a.lower(),
                      f"{tag} RED: kidpal-reminder-semantic SKILL 无 trigger 变更驱动段。")
        self.assertTrue(
            any(t in a.lower() for t in ("新鲜", "新于", "mtime", "竞态", "余量", "newer")),
            f"{tag} RED: reminder-semantic SKILL 未写 semantic.json 新鲜度合同。")
        self.assertTrue(
            any(t in a.lower() for t in ("60s", "60 秒", "60秒", "60 秒钟", "60-second", "60 sec")),
            f"{tag} RED: reminder-semantic SKILL 未写 60s 竞态余量。")

        b = _read_text(SKILL_B)
        self.assertTrue(b.strip(), f"{tag} RED: {SKILL_B} 不在场。")
        self.assertIn("trigger", b.lower(),
                      f"{tag} RED: kidpal-schedule-health SKILL 无 trigger 判定层段。")
        self.assertTrue(
            any(t in b for t in ("判定层", "是否要跑", "是否跑", "是否执行", "变更驱动"))
            or "gate" in b.lower(),
            f"{tag} RED: schedule-health SKILL 未写 trigger = 是否要跑的判定层口径。")

        readme = _read_text(SKILLS_README)
        self.assertTrue(readme.strip(), f"{tag} RED: {SKILLS_README} 不在场。")
        self.assertIn("trigger", readme.lower(),
                      f"{tag} RED: skills README 无两目 trigger 注记（docsSyncRequired）。")

    def test_crg_a5_trigger_shells_reference_correct_gate(self):
        """coverage: CRG-A5 — 两个薄壳 JS：exec 调各自判定器、返回 json({...fire...})、
        不串引用对方判定器。"""
        tag = "CRG-A5 shells"
        pairs = (
            (TRIGGER_A, GATE_A_NAME, GATE_B_NAME, TRIGGER_A_NAME),
            (TRIGGER_B, GATE_B_NAME, GATE_A_NAME, TRIGGER_B_NAME),
        )
        for trig, gate_name, other_gate, trig_name in pairs:
            self.assertTrue(trig.is_file(), f"{tag} RED: 薄壳 {trig} 不在位（设计态红）。")
            src = _read_text(trig)
            self.assertTrue(src.strip(), f"{tag} RED: {trig_name} 为空。")
            self.assertIn(gate_name, src,
                          f"{tag} RED: {trig_name} 未引用判定器 {gate_name}。")
            self.assertNotIn(other_gate, src,
                             f"{tag} RED: {trig_name} 串引了对方判定器 {other_gate}。")
            self.assertIn("exec", src, f"{tag} RED: {trig_name} 未用 exec 调判定器。")
            self.assertIn("json(", src, f"{tag} RED: {trig_name} 未返回 json({{...}})。")
            self.assertIn("fire", src, f"{tag} RED: {trig_name} 未透传 fire。")

    def test_crg_a5_cron_triggers_helper_static_anchors(self):
        """coverage: CRG-A5 — cron_triggers.py 经 openclaw cron edit 加/去 trigger
        （--trigger-script / --clear-trigger / --dry-run / status），不直写 openclaw.json。"""
        tag = "CRG-A5 cron_triggers"
        self.assertTrue(CRON_TRIGGERS.is_file(),
                        f"{tag} RED: {CRON_TRIGGERS} 不在位（设计态红）。")
        src = _read_text(CRON_TRIGGERS)
        self.assertTrue(src.strip(), f"{tag} RED: cron_triggers.py 为空。")
        for token in ("cron", "edit", "--trigger-script", "--clear-trigger", "--dry-run", "status"):
            self.assertIn(token, src,
                          f"{tag} RED: cron_triggers.py 缺 {token!r} —— 冻结契约要求经"
                          "openclaw cron edit 幂等 apply/clear/status 且支持 --dry-run。")
        bad = re.search(
            r"(?:config|cfg|openclaw_json|target|target_path)[A-Za-z_]*\s*\.\s*write_text\s*\(", src)
        self.assertIsNone(
            bad, f"{tag} RED: cron_triggers.py 有直写 config 的 write_text 路径 —— 禁直写"
                 " openclaw.json（写入口只许 openclaw CLI）。")
        bad2 = re.search(r"open\(\s*[^)\n]*openclaw\.json[^)\n]*,\s*['\"][wa]", src)
        self.assertIsNone(
            bad2, f"{tag} RED: cron_triggers.py 以写模式 open openclaw.json —— 禁直写。")


# =========================================================================== #
# LIVE —— L1..L7 证据读（@pytest.mark.live，不 auto-run；probe ≠ exercise）
# 真实 ECS 执行（openclaw cron edit --trigger-script / L1-L7 / 回滚演练）归主线程/human，
# 本类只对 live 相位**真实产出**的证据文件施加判定。证据缺位 = 设计态红（不 auto-run，
# ARQ-260901-g3；conftest 机械 skip）。
# =========================================================================== #

class CronTriggerGatingLiveAcceptance(unittest.TestCase):

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _evidence_text(self) -> str:
        for name in ("live-evidence.md", "live-handoff.md"):
            p = LIVE_DIR / name
            if p.is_file():
                return _read_text(p)
        return ""

    def _require(self, tag: str) -> str:
        body = self._evidence_text()
        self.assertTrue(
            body.strip(),
            f"{tag} live RED: 未见 live 证据（{LIVE_DIR} 下 live-evidence.md / live-handoff.md）"
            "—— L1-L7 归主线程/human ECS 轮，证据须先落盘。")
        return body

    def test_live_evidence_covers_l1_l7(self):
        """coverage: CRG-A5/CRG-E4 — live 证据覆盖 L1..L7 全段。"""
        tag = "CRG-LIVE"
        body = self._require(tag)
        missing = [l for l in ("L1", "L2", "L3", "L4", "L5", "L6", "L7") if l not in body]
        self.assertEqual(
            missing, [],
            f"{tag} live RED: live 证据缺段 {missing}（L1-L7 七事实须逐段落证据）。")

    def test_live_evidence_records_exec_shape_and_rollback(self):
        """coverage: CRG-E4 — L2 记录 exec 返回字段实形（aggregated / text）；L6 回滚演练含
        --clear-trigger。"""
        tag = "CRG-LIVE"
        body = self._require(tag)
        low = body.lower()
        self.assertTrue(
            ("aggregated" in low) or ("text" in low),
            f"{tag} live RED: L2 未记录 exec 返回字段实形（aggregated vs text）—— "
            "未跑 spike 的四条未知项须在此收口。")
        self.assertIn(
            "--clear-trigger", body,
            f"{tag} live RED: L6 回滚演练证据缺 --clear-trigger。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
