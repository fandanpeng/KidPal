"""Frozen acceptance tests — TaskPackage 260924-perf-k1-roundtrip-latency
(subsystem: server; rigor full; riskTier medium; sourceType human-ad-hoc;
gateProfile ["test-quality"]).

需求锚：docs/requirements.md NFR-PERF-1（思考响应不冷场）+ NFR-PERF-2（查字端到端 5 秒档）。
技术方案权威 = delivery/taskpackages/260924-perf-k1-roundtrip-latency/plan.md（FROZEN v1.0
「验收口径」节）+ taskpackage.json acceptance[]/exitCriteria[]。**A/B 主次以
launch-prep-record.md「human 五项业务裁定」B1 行为准**：P1 主案 = B 案（v4-flash 关/降
thinking 档，不换模型），A 案（换非推理模型）降为 fallback。plan.md 冻结文本不回改。

Scope of record (taskpackage acceptance PERF-A1..A4 + exitCriteria PERF-E1..E3):

  PERF-A1 探针门（live 证据型，两层门）+ 其机械接线面（offline）——
    · 声明工件在位 / schema / case 语义（B：thinkingDefault∈{off,low}；A：非推理模型
      且 ∈ allowlist）；
    · apply 脚本幂等 + drift-guard（现状锚点核对不中 → 拒绝，形制照抄
      server/deploy/openclaw-patch/apply-openclaw-patch.sh 的 STOP-on-mismatch，退出码 3）
      + 时间戳备份；
    · 探针脚本 v6 注入（system prompt = 生产所用，逐字）+ hash 断言（渲染面==被测面，
      ADR-001 精神，评审 D1）；
    · 两层门判定器（保底 6/9/14s、目标 4/6/10s）**逻辑本身** offline 齿（喂 fixture 证据
      → 断言判定结果）；**时延数字本身不入 CI 齿**（timing 齿天然 flaky，评审 §3）。
  PERF-A2 质量守卫判定器（display 块出现率 + schema 合法率 100%、课表备注读回、天气须含
    真实检索数据）——offline 只钉判定器逻辑；真实批次全量判定归 live。
  PERF-A3 晨间真机轮门（闲聊≤8s/课表≤10s 保底；闲聊≤5s/课表≤7s 目标）——offline 钉同一
    判定器在 a3 层的阈值行为；真机轮 human 持板，不在本件 auto-run 面。
  PERF-A4 回滚与核验——回滚脚本 dry-run 可跑 + 一步还原行为 + P0 五项核验证据（live 面）。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期零网络 I/O）

落位目录 = server/deploy/perf-k1/（与 openclaw-patch 同族，声明 + 幂等 apply + 回滚 + 探针）：

  1) openclaw-model-declaration.json —— 模型/thinking 变更声明工件（**JSON**，用 json.loads
     解析，零额外依赖）。必需顶层键：case / change / drift_guard / backup / rollback。
       · case : "B" | "A"。（B = 主案：改 thinking 档，不换模型；A = fallback：换非推理模型。）
       · change（**一晚只动一个变量**，B/A 两轴互斥，plan 归因卫生）：
           case=B  → {"thinkingDefault": "off"|"low"}（且不得含 model 键）
           case=A  → {"model": {"primary": "<id>", "reasoning": false,
                                "allow": ["<id>", ...]}}（primary ∈ allow；且不得含
                     thinkingDefault 键）
       · drift_guard : 非空 list[{path, expect}]。path = openclaw.json 的点分路径；expect =
         变更前的现状锚（字符串 "unset" = 该键必须缺席；否则为具体值）。B 案的 gal 锚 =
         `agents.defaults.thinkingDefault` == "unset"（证据 §8.1 CLI 地面真值）；A 案的锚 =
         `agents.defaults.model.primary` == 现状型号。
       · backup : {"dir": "<ECS 备份目录>", "pattern": "<含 'bak' 的命名模式>"}。
       · rollback : {"script": "rollback_model_declaration.py"}。

  2) apply_model_declaration.py —— 幂等 apply 脚本（**Python**；离线用
     `sys.executable <script> --config <fixture.json> --decl <decl.json>` 可真跑）。语义序：
       ① 幂等优先：若 config 现状已等于声明目标 → 打印 SKIP 并 exit 0（不写、不再备份）；
       ② drift-guard：逐条核对 drift_guard（expect=="unset" 判键缺席，否则判值相等），
          任一不中 → 打印 STOP 信息并 **exit 3**（不写）；
       ③ 备份：写前建时间戳备份 `config + ".bak-perf-k1-<UTC yyyymmddTHHMMSS>"`（内容 =
          原始 config）；
       ④ 应用：B 写 `agents.defaults.thinkingDefault`；A 写 `agents.defaults.model.primary`
          并把 primary 并入 `modelPolicy.allow`。成功 exit 0。
      CLI/参数：`--config`（默认 /root/.openclaw/openclaw.json）、`--decl`（默认同目录
      openclaw-model-declaration.json）。用 stdlib json（**不依赖 jq**）。

  3) rollback_model_declaration.py —— 一步回滚（Python）。CLI：`--config`、`--backup`
     （默认取 config 旁最新 `.bak-perf-k1-*`）、`--dry-run`（只打印意图、exit 0、不写）。
     非 dry-run：把 backup 内容还原到 config，exit 0。

  4) probe_gate.py —— **纯判定器 / 纯 helper 模块**（无网络、无 fastapi，离线可 import）：
       · resolve_prompt_artifact(repo_root) -> Path
           从**结构化指向**解析当前被接线 / 渲染的生产 system prompt 文件（读
           server/deploy/provider.config.yaml 的 system_prompt.source|artifact 与
           apply-xiaozhi-overlay.sh 的 PROMPT_SRC/DST —— **版本无关**，禁硬写 vN 字面，
           ARQ-260908 纪律）。
       · prompt_sha256(path) -> str
           返回该文件的 sha256（**LF 归一后**：read_text().replace("\\r\\n","\\n") 再 encode
           utf-8），与生产渲染件的 hash 口径一致（证据 §7 实证）。
       · build_probe_messages(prompt_text, question) -> list[dict]
           返回 [{"role":"system","content":prompt_text}, {"role":"user","content":question}]
           （评审 D1：探针必带生产 prompt，裸探针不测生产形态）。
       · median(samples) -> float（N 样本中位数）。
       · extract_display_blocks(text) -> list[str]（取 ```display fenced JSON 块原文）。
       · evaluate_batch(batch) -> dict（两层门 + 质量判定，纯逻辑）。
           batch = {"segment": str, "layer": "a1"|"a3",
                    "samples": {topic -> [ {total_s: float, reply: str,
                                            retrieval?: bool, note_readback?: bool}, ... ]},
                    "host": {...}, "sessions": {"before": int, "after": int}}
           topic ∈ {"chat","schedule","weather"}；层阈值常量：
                    BOTTOM_A1 = {"chat":6.0,"schedule":9.0,"weather":14.0}
                    TARGET_A1 = {"chat":4.0,"schedule":6.0,"weather":10.0}
                    BOTTOM_A3 = {"chat":8.0,"schedule":10.0}
                    TARGET_A3 = {"chat":5.0,"schedule":7.0}
           返回键：segment, layer, n_min, n_ok（各题样本 ≥3）, medians{topic:float},
                    bottom{topic:bool}, bottom_pass, target{topic:bool}, target_pass,
                    quality{n,display_rate,schema_rate,quality_pass,
                            weather_retrieval_ok,note_readback_ok},
                    verdict ∈ {"PASS","BOTTOM_ONLY","FAIL"}。
           判定规则：n_ok 假 → verdict "FAIL"；bottom 任一假 → "FAIL"；bottom 全真且 target
                    全真 → "PASS"；否则 "BOTTOM_ONLY"。天气题：**全部** weather 样本
                    retrieval 真才计 bottom 真（否则计时再好也判红）。质量：display 块
                    出现率与 schema 合法率（schema 用 server/gateway/display_schema.
                    validate_display 真门）均须 1.0，且 weather_retrieval_ok、
                    note_readback_ok 皆真 → quality_pass。

  5) probe_batch.py —— live 探针批执行器（发 HTTP、N≥3、计时、主机快照、session 计数；
     须 `--check-prompt` 模式：离线解析生产 prompt + 打印其路径与归一 sha256，exit 0，零网络）。

live 证据（PERF-A1/A2/A4 判定器的输入，builder/verifier live 相位产出）：
  结构化批次 JSON 落 `probe-batches/<segment>.json`（schema 同 evaluate_batch 的 batch）,
  位于 delivery/changes/dev/260924-perf-k1-roundtrip-latency/ 或
  delivery/taskpackages/260924-perf-k1-roundtrip-latency/evidence/ 之下。

===========================================================================
强度申报（acceptance-evidence「契约条目 ↔ 齿 的强度申报」T2；映射 / 缺口 / 等价假设）

  映射（契约/设计条目 → 承载齿 → 性质档）：
   · PERF-A1 声明工件在位/schema/case 语义        → decl_* 齿        → 机制级
   · PERF-A1 drift-guard 现状锚（§8.1 unset）      → decl_drift 齿    → 机制级
   · PERF-A1 apply 幂等                            → apply 齿（真跑两次）→ 行为级
   · PERF-A1 drift-guard 拒绝（exit 3）            → apply_mismatch 齿 → 行为级
   · PERF-A1 时间戳备份                            → apply_backup 齿  → 行为级
   · PERF-A4 回滚 dry-run / 一步还原               → rollback 两齿    → 行为级
   · PERF-A1 探针 v6 注入 + hash（D1）             → probe_prompt 齿  → 机制级 / 源锚代理
   · PERF-A1 两层门判定器                          → gate_* 齿        → 行为级（fixture 逻辑）
   · PERF-A2 质量守卫判定器                        → quality_* 齿     → 行为级
   · PERF-A3 引擎日志门判定（a3 层）               → gate_a3 齿       → 行为级
   · PERF-E1 must-stay-green（provider/overlay/v6） → e1_* 齿          → 机制级
   · PERF-A4 P0 五项核验 / A2 全量 / QQ 回归       → live 证据读齿     → 源锚代理

  缺口（承载齿强度低于契约要求处，逐条）：
   · 「探针批执行器真发请求」不在 offline 齿——offline 只钉其 prompt 接线（--check-prompt）
     与纯判定逻辑；真实发 HTTP / N≥3 采样 / 主机快照 / session 计数归 live 相位（builder
     夜间执行 + live 证据结构化落盘）。**offline 不证明执行器真发请求**。
   · 「时延门」数字判定 = live 证据 + 冻结阈值；offline 齿只证判定器**逻辑**正确，不证真实
     时延达标（时延数字不入 CI 齿，评审 §3）。
   · A2 的「提醒 semantic.json 形状冒烟」「QQ 录入回归」= 晨间 human 目击 / live 证据记录，
     不 offline 钉（live 齿只核验证据记录在场）。
   · 声明工件与 ECS 实况的**真锚**一致性只在 live 执行时成立——offline 只钉锚点**声明形态**。

  行为等价假设（显式化，不写即默认不成立）：
   · `expect=="unset"` ⇔ 「openclaw.json 无 authored thinkingDefault」（依据证据 §8.1 CLI
     回执逐字：「The runtime default applies until you set an authored value…」）。
   · 「非推理」⇔ 声明面 `model.reasoning == false`——假设 catalog reasoning 标志由声明工件
     承载（offline 无法读 ECS catalog）；A 案 live 须以 `openclaw models list --json` 复核
     （builder 步⑤纪律）。
   · 「渲染面 == 被测面」以 sha256 相等为代理——假设 ECS 渲染件与仓内工件逐字相同（证据 §7
     实测 LF 归一后相等；ARQ/ADR-001 精神）。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/
  live 面（@pytest.mark.live）不 auto-run（conftest 机械 skip，ARQ-260901-g3）。
"""

from __future__ import annotations

import hashlib
import importlib.util
import json
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

# --------------------------------------------------------------------------- #
# Paths（仓根由 __file__ 显式推导，不依赖调用点 cwd——acceptance-evidence root 注入纪律）
# --------------------------------------------------------------------------- #

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY_DIR = SERVER / "deploy"
GATEWAY_DIR = SERVER / "gateway"

PERF_K1_DIR = DEPLOY_DIR / "perf-k1"
DECL_PATH = PERF_K1_DIR / "openclaw-model-declaration.json"
APPLY_SCRIPT = PERF_K1_DIR / "apply_model_declaration.py"
ROLLBACK_SCRIPT = PERF_K1_DIR / "rollback_model_declaration.py"
PROBE_GATE_MODULE = PERF_K1_DIR / "probe_gate.py"
PROBE_BATCH_MODULE = PERF_K1_DIR / "probe_batch.py"

PROVIDER_DECL_YAML = DEPLOY_DIR / "provider.config.yaml"
OVERLAY_SH = DEPLOY_DIR / "apply-xiaozhi-overlay.sh"
PROMPT_V6 = DEPLOY_DIR / "openclaw-system-prompt.v6.md"

CHANGE_ID = "260924-perf-k1-roundtrip-latency"
LIVE_EVIDENCE_DIRS = [
    REPO_ROOT / "delivery" / "changes" / "dev" / CHANGE_ID,
    REPO_ROOT / "delivery" / "taskpackages" / CHANGE_ID / "evidence",
]
PROBE_BATCH_DIRNAME = "probe-batches"

# 落盘 / 措辞类守卫：v6 是「本包不动 prompt」的冻结基座，钉死其归一 sha256 合理。
# guard-class: landing —— v6 相对零回改（PERF-E1/E2 铁律）。计算式 = sha256(
# read_text.replace("\r\n","\n") 后 utf-8)；实测 == ECS 渲染件 hash（证据 §7）。
V6_SHA256_NORM = "425d293d35b106b581785a78055f5718068531b240d52db763591eed68b0cc62"
V6_BYTES_NORM = 22464

_MISSING = object()


# --------------------------------------------------------------------------- #
# 读盘 helpers
# --------------------------------------------------------------------------- #

def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _sha256_norm(path: Path) -> str:
    try:
        txt = path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""
    return hashlib.sha256(txt.replace("\r\n", "\n").encode("utf-8")).hexdigest()


def _load_json(path: Path):
    txt = _read_text(path)
    if not txt:
        return None, f"{path} missing or empty"
    try:
        return json.loads(txt), None
    except ValueError as e:  # noqa: BLE001
        return None, f"{path} is not valid JSON: {e!r}"


def _load_module_from(path: Path, modname: str):
    """importlib 载入任意路径的纯 python 模块；缺文件 / 导入错 → (None, err) 不 crash。"""
    if not path.is_file():
        return None, f"{path} missing — 冻结契约模块不在位。"
    try:
        spec = importlib.util.spec_from_file_location(modname, path)
        if spec is None or spec.loader is None:
            return None, f"{path} not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod, None
    except Exception as e:  # noqa: BLE001 — import error is a RED, not a crash
        return None, (f"{path} failed to import: {e!r}（冻结契约模块 MUST 纯 python / "
                      "导入期零网络 I/O）。")


def _load_probe_gate():
    return _load_module_from(PROBE_GATE_MODULE, "kidpal_probe_gate_perf_k1")


def _run_py(script: Path, args, cwd: Path | None = None, timeout: int = 120):
    """用当前解释器跑 python 脚本；离线可跑（不经 bash/ssh）。"""
    proc = subprocess.run(
        [sys.executable, str(script), *[str(a) for a in args]],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
        timeout=timeout, cwd=str(cwd) if cwd else None,
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
# 声明工件校验（纯函数——teeth 与 mutation 复现共用；返回 violation 字符串列表）
# --------------------------------------------------------------------------- #

def validate_declaration(decl) -> list[str]:
    """校验声明工件对象，返回违规列表（空 = 合法）。纯逻辑、无 IO。"""
    v: list[str] = []
    if not isinstance(decl, dict):
        return ["declaration 顶层必须是 JSON object"]

    case = decl.get("case")
    if case not in ("B", "A"):
        v.append(f"case 必须是 'B' 或 'A'，实得 {case!r}")

    change = decl.get("change")
    if not isinstance(change, dict):
        v.append("change 必须是 object")
        change = {}

    dg = decl.get("drift_guard")
    if not isinstance(dg, list) or not dg:
        v.append("drift_guard 必须是非空 list")
        dg = []
    else:
        for i, e in enumerate(dg):
            if not isinstance(e, dict):
                v.append(f"drift_guard[{i}] 必须是 object")
                continue
            if not isinstance(e.get("path"), str) or not e["path"].strip():
                v.append(f"drift_guard[{i}].path 必须是非空字符串")
            if not isinstance(e.get("expect"), str) or not e["expect"].strip():
                v.append(f"drift_guard[{i}].expect 必须是非空字符串（'unset' 或具体值）")

    backup = decl.get("backup")
    if not isinstance(backup, dict) or not isinstance(backup.get("dir"), str) \
            or not isinstance(backup.get("pattern"), str) or "bak" not in backup.get("pattern", ""):
        v.append("backup 必须是 {dir:str, pattern:str(含 'bak')}")

    rollback = decl.get("rollback")
    if not isinstance(rollback, dict) or not isinstance(rollback.get("script"), str) \
            or not rollback.get("script", "").strip():
        v.append("rollback 必须是 {script:str}")

    dg_paths = [e.get("path", "") for e in dg if isinstance(e, dict)]

    if case == "B":
        if "model" in change:
            v.append("case=B 不得同时声明 model 变更（一晚只动一个变量）")
        td = change.get("thinkingDefault")
        if td not in ("off", "low"):
            v.append(f"case=B 要求 change.thinkingDefault ∈ {{'off','low'}}，实得 {td!r}")
        if not any("thinkingDefault" in p for p in dg_paths):
            v.append("case=B 的 drift_guard 须锚定 thinkingDefault 路径（现状 = unset，§8.1）")
    elif case == "A":
        if "thinkingDefault" in change:
            v.append("case=A 不得同时声明 thinking 变更（一晚只动一个变量）")
        model = change.get("model")
        if not isinstance(model, dict):
            v.append("case=A 要求 change.model 为 object")
        else:
            primary = model.get("primary")
            allow = model.get("allow")
            if not isinstance(primary, str) or not primary.strip():
                v.append("case=A 要求 change.model.primary 为非空字符串")
            if model.get("reasoning") is not False:
                v.append("case=A 要求 change.model.reasoning 显式为 false（非推理模型）")
            if not isinstance(allow, list) or not allow:
                v.append("case=A 要求 change.model.allow 为非空 list（allowlist 增补）")
            elif isinstance(primary, str) and primary not in allow:
                v.append(f"case=A 要求 primary {primary!r} ∈ model.allow（删 allow 条目即违规）")
        if not any("model" in p for p in dg_paths):
            v.append("case=A 的 drift_guard 须锚定 model 路径（现状 = 当前型号）")
    return v


# --------------------------------------------------------------------------- #
# 接线指针解析（版本无关；不硬写 vN 字面，ARQ-260908 纪律）
# --------------------------------------------------------------------------- #

_PROMPT_FILE_RE = re.compile(r"openclaw-system-prompt\.v\d+\.md")


def _wired_prompt_artifact() -> str:
    """从声明面 provider.config.yaml 解析当前被接线的 prompt 文件名（结构化指向）。"""
    m = re.search(r"artifact\s*:\s*\"[^\"]*?(openclaw-system-prompt\.v\d+\.md)\"",
                  _read_text(PROVIDER_DECL_YAML))
    return m.group(1) if m else ""


def _overlay_prompt_files() -> list[str]:
    return re.findall(r"PROMPT_(?:SRC|DST)\s*=.*?(openclaw-system-prompt\.v\d+\.md)",
                      _read_text(OVERLAY_SH))


# --------------------------------------------------------------------------- #
# 声明 fixture / apply 期望态（供 apply 齿离线 exercise）
# --------------------------------------------------------------------------- #

_BASE_OPENCLAW = {
    "agents": {"defaults": {"model": {"primary": "deepseek/deepseek-v4-flash"}}},
    "modelPolicy": {"allow": ["deepseek/deepseek-v4-flash"]},
}


def _fixture_pre(decl: dict) -> dict:
    """按 drift_guard 的现状锚构造「变更前」config fixture。"""
    state = json.loads(json.dumps(_BASE_OPENCLAW))
    for entry in decl.get("drift_guard", []):
        path, expect = entry.get("path", ""), entry.get("expect", "")
        if expect and expect != "unset":
            _json_set(state, path, expect)
    return state


def _fixture_mismatch(decl: dict) -> dict:
    """构造 drift-guard 必不中的 fixture（现状被改成第三个值）。"""
    state = _fixture_pre(decl)
    for entry in decl.get("drift_guard", []):
        path, expect = entry.get("path", ""), entry.get("expect", "")
        if expect == "unset":
            _json_set(state, path, "__drifted__")
        else:
            _json_set(state, path, "__drifted__")
        break
    return state


def _apply_change_to_state(state: dict, decl: dict) -> dict:
    """按 change 声明施加变更（apply 后应有的目标态）。"""
    ch = decl.get("change", {})
    if "thinkingDefault" in ch:
        _json_set(state, "agents.defaults.thinkingDefault", ch["thinkingDefault"])
    if isinstance(ch.get("model"), dict):
        primary = ch["model"]["primary"]
        _json_set(state, "agents.defaults.model.primary", primary)
        allow = _json_get(state, "modelPolicy.allow")
        if not isinstance(allow, list):
            allow = []
            _json_set(state, "modelPolicy.allow", allow)
        if primary not in allow:
            allow.append(primary)
    return state


def _write_json(path: Path, obj) -> None:
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2), encoding="utf-8")


# =========================================================================== #
# OFFLINE（CI）齿 —— PERF-A1/A2/A3/E1 机械面（timing 数字不入 CI）
# =========================================================================== #

class PerfK1OfflineAcceptance(unittest.TestCase):

    maxDiff = None

    # ---- 共享：声明工件载入 -------------------------------------------------
    def _decl(self) -> dict:
        decl, err = _load_json(DECL_PATH)
        self.assertIsNotNone(
            decl,
            f"PERF-A1 RED: 声明工件不在位/不可解析 —— {err}。"
            "冻结契约要求 server/deploy/perf-k1/openclaw-model-declaration.json。",
        )
        return decl

    def _gate(self):
        mod, err = _load_probe_gate()
        self.assertIsNotNone(
            mod,
            f"PERF-A1 RED: 判定器模块不在位 —— {err}。"
            "冻结契约要求 server/deploy/perf-k1/probe_gate.py（纯逻辑、离线可 import）。",
        )
        return mod

    # ------------------------------------------------------------------ #
    # PERF-A1 · 声明工件在位 / schema / case 语义（机制级）
    # ------------------------------------------------------------------ #

    def test_a1_declaration_file_present_and_schema(self):
        """coverage: PERF-A1 — 声明工件在位 + JSON 可解析 + 必需键齐 + case 合法。"""
        decl = self._decl()
        for key in ("case", "change", "drift_guard", "backup", "rollback"):
            self.assertIn(
                key, decl,
                f"PERF-A1 RED: 声明工件缺必需顶层键 {key!r} —— "
                "B1 修正后对象 = thinking 档位声明（B 案）/ 非推理模型声明（A 案）。",
            )
        violations = validate_declaration(decl)
        self.assertEqual(
            violations, [],
            "PERF-A1 RED: 声明工件 schema 违规：\n  - " + "\n  - ".join(violations),
        )

    def test_a1_declaration_case_semantics_b_and_a_validators(self):
        """coverage: PERF-A1 — 校验器对 B 主案与 A fallback 两形态皆可判（机制级）。

        本齿钉「判定器逻辑对两形态都成立」：合法的 B/A fixture 过、越界值判红——
        这样 B 不达标转 A 时齿不会因错误理由翻红（任务书 §齿形态 1 防返工理由）。
        """
        good_b = {
            "case": "B",
            "change": {"thinkingDefault": "off"},
            "drift_guard": [{"path": "agents.defaults.thinkingDefault", "expect": "unset"}],
            "backup": {"dir": "/root/.openclaw", "pattern": "openclaw.json.bak-perf-k1-*"},
            "rollback": {"script": "rollback_model_declaration.py"},
        }
        good_a = {
            "case": "A",
            "change": {"model": {"primary": "deepseek/deepseek-chat", "reasoning": False,
                                 "allow": ["deepseek/deepseek-v4-flash", "deepseek/deepseek-chat"]}},
            "drift_guard": [{"path": "agents.defaults.model.primary",
                             "expect": "deepseek/deepseek-v4-flash"}],
            "backup": {"dir": "/root/.openclaw", "pattern": "openclaw.json.bak-perf-k1-*"},
            "rollback": {"script": "rollback_model_declaration.py"},
        }
        self.assertEqual(validate_declaration(good_b), [], "合法 B 形态被误判")
        self.assertEqual(validate_declaration(good_a), [], "合法 A 形态被误判")

        # 越界 / 边界：thinking 值非法、model primary 不在 allow、reasoning 非 false、
        # 两轴混声明 —— 均须判红（mkdir 面：一审一变量 + allowlist 自洽）。
        bad_b = json.loads(json.dumps(good_b))
        bad_b["change"]["thinkingDefault"] = "high"
        self.assertTrue(validate_declaration(bad_b), "thinkingDefault='high' 未判红")

        bad_a_allow = json.loads(json.dumps(good_a))
        bad_a_allow["change"]["model"]["allow"] = ["deepseek/deepseek-v4-flash"]
        self.assertTrue(validate_declaration(bad_a_allow), "A 案 primary 不在 allow 未判红")

        bad_a_reason = json.loads(json.dumps(good_a))
        bad_a_reason["change"]["model"]["reasoning"] = True
        self.assertTrue(validate_declaration(bad_a_reason), "A 案 reasoning=True 未判红")

        mixed = json.loads(json.dumps(good_b))
        mixed["change"]["model"] = {"primary": "x", "reasoning": False, "allow": ["x"]}
        self.assertTrue(validate_declaration(mixed), "B/A 两轴混声明未判红")

    def test_a1_declaration_drift_guard_anchors_case_field(self):
        """coverage: PERF-A1 — drift_guard 现状锚指向本 case 变更的配置族（机制级 / §8.1）。"""
        decl = self._decl()
        case = decl.get("case")
        dg = [e for e in decl.get("drift_guard", []) if isinstance(e, dict)]
        if case == "B":
            hits = [e for e in dg if "thinkingDefault" in e.get("path", "")]
            self.assertTrue(
                hits,
                "PERF-A1 RED: B 案 drift_guard 未锚定 agents.defaults.thinkingDefault —— "
                "B1 抓手坐实于「unset 现状」（证据 §8.1），锚点缺失则 apply 无法拒绝漂移。",
            )
            self.assertEqual(
                hits[0].get("expect"), "unset",
                "PERF-A1 RED: B 案 thinkingDefault 现状锚必须是 'unset'（证据 §8.1 CLI 地面真值："
                "openclaw config get agents.defaults.thinkingDefault → unset）。",
            )
        elif case == "A":
            hits = [e for e in dg if "model" in e.get("path", "")]
            self.assertTrue(
                hits,
                "PERF-A1 RED: A 案 drift_guard 未锚定 model 路径（现状 = 当前型号）。",
            )
            self.assertTrue(
                hits[0].get("expect", "").strip(),
                "PERF-A1 RED: A 案 model 现状锚缺 expect 值。",
            )
        else:
            self.fail(f"PERF-A1 RED: 未识别 case={case!r}")

    # ------------------------------------------------------------------ #
    # PERF-A1 · apply 脚本（幂等 / drift-guard / 备份）—— 真跑离线 fixture（行为级）
    # ------------------------------------------------------------------ #

    def _run_apply(self, config: Path, decl: Path):
        return _run_py(APPLY_SCRIPT, ["--config", config, "--decl", decl])

    def test_a1_apply_script_present_and_consumes_declaration(self):
        """coverage: PERF-A1 — apply 脚本在位且消费声明工件（机制级）。"""
        self.assertTrue(
            APPLY_SCRIPT.is_file(),
            f"PERF-A1 RED: apply 脚本不在位 —— 冻结契约要求 {APPLY_SCRIPT}。",
        )
        src = _read_text(APPLY_SCRIPT)
        self.assertIn(
            "openclaw-model-declaration.json", src,
            "PERF-A1 RED: apply 脚本未引用声明工件名 —— 声明面与消费面不一致"
            "（openclaw.json 模型选择现状不在仓内跟踪，声明化是正解，评审 §4 P1）。",
        )

    def test_a1_apply_offline_applies_then_idempotent(self):
        """coverage: PERF-A1 — apply 真跑①：现状锚匹配 → 变到目标态；②：再跑 → 幂等 no-op（行为级）。"""
        decl = self._decl()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, _fixture_pre(decl))
            want = _apply_change_to_state(json.loads(json.dumps(_fixture_pre(decl))), decl)

            rc, out, err = self._run_apply(cfg, DECL_PATH)
            self.assertEqual(
                rc, 0,
                f"PERF-A1 RED: apply 首次运行 rc={rc}（应 0）。stdout={out!r} stderr={err!r}",
            )
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "PERF-A1 RED: apply 未产生声明目标态（B 写 thinkingDefault / "
                "A 写 model.primary+allow）。",
            )

            backups_after_first = sorted(td.glob("openclaw.json.bak-perf-k1-*"))
            rc2, out2, _ = self._run_apply(cfg, DECL_PATH)
            got2, _ = _load_json(cfg)
            self.assertEqual(rc2, 0, f"PERF-A1 RED: apply 二次运行 rc={rc2}（幂等应 0）。out={out2!r}")
            self.assertEqual(got2, want, "PERF-A1 RED: apply 二次运行改动了已达标态（非幂等）。")
            self.assertEqual(
                len(sorted(td.glob("openclaw.json.bak-perf-k1-*"))), len(backups_after_first),
                "PERF-A1 RED: apply 二次运行又新建了备份（幂等应不重备份）。",
            )

    def test_a1_apply_driftguard_exit3_on_mismatch(self):
        """coverage: PERF-A1 — drift-guard：现状锚不中 → 拒绝（退出码 3）且零写入（行为级）。

        形制照抄 server/deploy/openclaw-patch/apply-openclaw-patch.sh 的 STOP-on-mismatch
        （版本漂移 → 停止，不产属性绑定 diff）。"""
        decl = self._decl()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            drifted = _fixture_mismatch(decl)
            _write_json(cfg, drifted)

            rc, out, err = self._run_apply(cfg, DECL_PATH)
            self.assertEqual(
                rc, 3,
                f"PERF-A1 RED: drift-guard 现状锚不中应 exit 3（实得 rc={rc}）。"
                f"stdout={out!r} stderr={err!r}",
            )
            after, _ = _load_json(cfg)
            self.assertEqual(after, drifted, "PERF-A1 RED: drift-guard 拒绝后仍改写了 config。")

    def test_a1_apply_timestamped_backup_before_write(self):
        """coverage: PERF-A1 — 写前建时间戳备份，内容 = 原始 config（行为级）。"""
        decl = self._decl()
        pre = _fixture_pre(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = self._run_apply(cfg, DECL_PATH)
            self.assertEqual(rc, 0, f"PERF-A1 RED: apply rc={rc}。stderr={err!r}")
            backups = sorted(td.glob("openclaw.json.bak-perf-k1-*"))
            self.assertTrue(
                backups,
                "PERF-A1 RED: apply 未建时间戳备份 —— A4「openclaw.json 时间戳备份」缺失"
                "（回滚先决）。",
            )
            saved, _ = _load_json(backups[0])
            self.assertEqual(saved, pre, "PERF-A1 RED: 备份内容 ≠ 变更前 config。")

    # ------------------------------------------------------------------ #
    # PERF-A4 · 回滚（dry-run + 一步还原）（行为级）
    # ------------------------------------------------------------------ #

    def test_a4_rollback_dry_run_no_write(self):
        """coverage: PERF-A4 — 回滚脚本 --dry-run 可跑（exit 0）且零写入。"""
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"PERF-A4 RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。",
        )
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-perf-k1-20260924T000000"
            _write_json(cfg, {"applied": True})
            _write_json(bak, {"applied": False})

            rc, out, err = _run_py(ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak, "--dry-run"])
            self.assertEqual(
                rc, 0,
                f"PERF-A4 RED: rollback --dry-run rc={rc}（应 0）。stdout={out!r} stderr={err!r}",
            )
            after, _ = _load_json(cfg)
            self.assertEqual(after, {"applied": True}, "PERF-A4 RED: --dry-run 改写了 config。")

    def test_a4_rollback_restores_backup(self):
        """coverage: PERF-A4 — 一步还原：backup 内容写回 config（行为级）。"""
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-perf-k1-20260924T000000"
            _write_json(cfg, {"applied": True})
            _write_json(bak, {"applied": False})

            rc, out, err = _run_py(ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak])
            self.assertEqual(rc, 0, f"PERF-A4 RED: rollback rc={rc}。stderr={err!r}")
            after, _ = _load_json(cfg)
            self.assertEqual(after, {"applied": False}, "PERF-A4 RED: 回滚未还原 backup 内容。")

    # ------------------------------------------------------------------ #
    # PERF-A1 · 探针接线（v6 注入 + hash 断言，D1）（机制级 / 源锚代理）
    # ------------------------------------------------------------------ #

    def test_a1_probe_prompt_resolution_version_agnostic(self):
        """coverage: PERF-A1 — 探针 prompt 解析走结构化指向（渲染面==被测面，D1）（机制级）。

        版本无关（ARQ-260908 纪律）：钉「探针解析出的 prompt == 声明面接线的 prompt」，
        不硬写 vN 字面——前序包 forward-bump 后字面陈旧会假红。"""
        gate = self._gate()
        resolver = getattr(gate, "resolve_prompt_artifact", None)
        self.assertTrue(
            callable(resolver),
            "PERF-A1 RED: probe_gate 缺 resolve_prompt_artifact(repo_root)（冻结契约 API）。",
        )
        resolved = Path(resolver(REPO_ROOT))
        declared = _wired_prompt_artifact()
        self.assertTrue(
            declared,
            f"PERF-A1 RED: {PROVIDER_DECL_YAML} system_prompt.artifact 解析不到 prompt 文件名"
            "（声明面破坏）。",
        )
        self.assertEqual(
            resolved.name, declared,
            f"PERF-A1 RED: 探针解析出的 prompt {resolved.name!r} ≠ 声明面接线 {declared!r} —— "
            "渲染面 == 被测面（ADR-001 / 评审 D1）被破坏。",
        )
        self.assertTrue(
            resolved.is_file() and resolved.stat().st_size > 0,
            f"PERF-A1 RED: 探针解析出的 prompt {resolved} 不在场或为空（孤儿接线）。",
        )
        # hash 口径 = LF 归一（与生产渲染件一致，证据 §7）。
        sha = getattr(gate, "prompt_sha256", None)
        self.assertTrue(callable(sha), "PERF-A1 RED: probe_gate 缺 prompt_sha256(path)。")
        self.assertEqual(
            sha(resolved), _sha256_norm(resolved),
            "PERF-A1 RED: prompt_sha256 未按 LF 归一口径计算（渲染面 hash 不可比）。",
        )

    def test_a1_probe_messages_include_production_prompt(self):
        """coverage: PERF-A1 — 探针 messages 必带 system(被接线版逐字) + user(题)（机制级 / D1）。

        裸探针不带生产 prompt 时测的不是生产形态（评审推论甲 / §0.2-9 实证：无 display 块、
        唤 David）——A1/A2 判定探针必须带仓内被接线 prompt 逐字。260925-prompt-v7-round-routing
        起改解析声明面得出版本（原硬编 v6 字面 = ARQ-260908-versioned-guard-version-literal-coupling
        族类，bump 后静默测旧版假绿）。"""
        gate = self._gate()
        build = getattr(gate, "build_probe_messages", None)
        self.assertTrue(callable(build), "PERF-A1 RED: probe_gate 缺 build_probe_messages().")
        declared = _wired_prompt_artifact()
        self.assertTrue(
            declared,
            f"PERF-A1 RED: {PROVIDER_DECL_YAML} system_prompt.artifact 解析不到 prompt 文件名。",
        )
        prompt = _read_text(DEPLOY_DIR / declared)
        msgs = build(prompt, "明天有什么课程?")
        self.assertEqual(len(msgs), 2, "PERF-A1 RED: 探针 messages 不是 [system, user] 两元素。")
        self.assertEqual(msgs[0].get("role"), "system", "PERF-A1 RED: 首元素非 system 角色。")
        self.assertEqual(
            msgs[0].get("content"), prompt,
            "PERF-A1 RED: system 内容 ≠ 仓内被接线 prompt 逐字 —— 探针未注入生产 prompt（D1 缺陷）。",
        )
        self.assertEqual(msgs[1].get("role"), "user", "PERF-A1 RED: 次元素非 user 角色。")
        self.assertEqual(msgs[1].get("content"), "明天有什么课程?", "PERF-A1 RED: user 题面不符。")

    def test_a1_probe_script_check_prompt_runs(self):
        """coverage: PERF-A1 — 探针脚本 --check-prompt 离线可跑且解析出生产 prompt + 归一 hash（源锚代理）。

        hash 断言版本无关（260925-prompt-v7-round-routing 起；原钉 v6 哈希字面 =
        ARQ-260908-versioned-guard-version-literal-coupling 族类复发实例）——哈希取
        「探针解析出 == 声明面接线」的那份文件（test_a1_probe_prompt_resolution_version_agnostic
        已证解析一致性），bump 不再假红。"""
        self.assertTrue(
            PROBE_BATCH_MODULE.is_file(),
            f"PERF-A1 RED: 探针执行器不在位 —— 冻结契约要求 {PROBE_BATCH_MODULE}。",
        )
        rc, out, err = _run_py(PROBE_BATCH_MODULE, ["--check-prompt"])
        self.assertEqual(
            rc, 0,
            f"PERF-A1 RED: probe_batch --check-prompt rc={rc}（应 0）。stdout={out!r} stderr={err!r}",
        )
        declared = _wired_prompt_artifact()
        self.assertIn(
            declared, out,
            f"PERF-A1 RED: --check-prompt 未打印解析出的生产 prompt {declared!r}（渲染面==被测面）。",
        )
        wired = _wired_prompt_artifact()
        self.assertTrue(
            wired,
            f"PERF-A1 RED: {PROVIDER_DECL_YAML} system_prompt.artifact 解析不到 prompt 文件名。",
        )
        expected_sha = _sha256_norm(DEPLOY_DIR / wired)
        self.assertIn(
            expected_sha, out.lower(),
            "PERF-A1 RED: --check-prompt 未打印生产 prompt 的归一 sha256（hash 断言缺失）。",
        )

    # ------------------------------------------------------------------ #
    # PERF-A1/A2/A3 · 判定器逻辑 offline 齿（fixture 证据 → 断言判定结果；行为级）
    # ------------------------------------------------------------------ #

    @staticmethod
    def _batch(layer: str, chat_s, sched_s, weather_s=None) -> dict:
        samples = {
            "chat": [{"total_s": s, "reply": "好呀，我们一起看吧。\n```display\n"
                      + json.dumps({"view": "text", "data": {"text": "x"}}) + "\n```\n"}
                     for s in chat_s],
            "schedule": [{"total_s": s, "reply": "明天下课后有钢琴课，备注是带琴谱。\n```display\n"
                          + json.dumps({"view": "text", "data": {"text": "x"}}) + "\n```\n",
                          "note_readback": True} for s in sched_s],
        }
        if weather_s is not None:
            samples["weather"] = [{"total_s": s, "reply": "今天西安 17~25℃，午后有霾。\n```display\n"
                                   + json.dumps({"view": "text", "data": {"text": "x"}}) + "\n```\n",
                                   "retrieval": True} for s in weather_s]
        return {"segment": "fixture", "layer": layer, "samples": samples,
                "host": {"mem_available_mb": 800}, "sessions": {"before": 511, "after": 520}}

    def _eval(self, batch: dict) -> dict:
        gate = self._gate()
        fn = getattr(gate, "evaluate_batch", None)
        self.assertTrue(callable(fn), "PERF-A1 RED: probe_gate 缺 evaluate_batch(batch)。")
        return fn(batch)

    def _median(self, samples) -> float:
        gate = self._gate()
        fn = getattr(gate, "median", None)
        self.assertTrue(callable(fn), "PERF-A1 RED: probe_gate 缺 median(samples)。")
        return fn(list(samples))

    def test_a1_gate_decider_median(self):
        """coverage: PERF-A1 — median：奇数/偶数样本中位数正确（行为级）。"""
        self.assertEqual(self._median([1.0, 2.0, 3.0]), 2.0)
        self.assertAlmostEqual(self._median([1.0, 2.0, 3.0, 4.0]), 2.5)
        self.assertAlmostEqual(self._median([6.64, 4.38, 9.29]), 6.64)

    def test_a1_gate_decider_two_layer_a1(self):
        """coverage: PERF-A1 — a1 两层门：目标全达 → PASS；仅保底 → BOTTOM_ONLY；保底破 → FAIL（行为级）。"""
        dec = self._eval(self._batch("a1", [3.9, 3.8, 4.0], [5.5, 5.9, 6.0], [9.0, 9.5, 10.0]))
        self.assertTrue(dec["bottom_pass"], f"目标态应过保底门: {dec}")
        self.assertTrue(dec["target_pass"], f"闲聊3.9/课表5.9/天气9.5 应过目标门: {dec}")
        self.assertEqual(dec["verdict"], "PASS")

        dec2 = self._eval(self._batch("a1", [5.5, 5.9, 6.0], [8.0, 8.5, 9.0], [13.0, 13.5, 14.0]))
        self.assertTrue(dec2["bottom_pass"], f"保底态应过保底门: {dec2}")
        self.assertFalse(dec2["target_pass"], "保底态不应过目标门")
        self.assertEqual(dec2["verdict"], "BOTTOM_ONLY", f"仅保底应 BOTTOM_ONLY: {dec2}")

        dec3 = self._eval(self._batch("a1", [7.0, 7.5, 8.0], [8.0, 8.5, 9.0], [13.0, 13.5, 14.0]))
        self.assertFalse(dec3["bottom_pass"], "闲聊 7.5s > 保底 6s 应破门")
        self.assertEqual(dec3["verdict"], "FAIL")

    def test_a1_gate_decider_requires_n_ge_3(self):
        """coverage: PERF-A1 — 门需各题 N≥3；样本不足 → n_ok 假且 verdict FAIL（行为级）。"""
        dec = self._eval(self._batch("a1", [3.0, 3.1], [5.0, 5.1], [9.0, 9.1]))
        self.assertFalse(dec["n_ok"], f"N=2 应判 n_ok 假: {dec}")
        self.assertEqual(dec["verdict"], "FAIL", f"样本不足不得判 PASS: {dec}")

    def test_a1_gate_decider_weather_requires_real_retrieval(self):
        """coverage: PERF-A1 — 天气题附加条件：无真实检索数据 → 计时再好也判红（行为级）。"""
        batch = self._batch("a1", [3.0, 3.1, 3.2], [5.0, 5.1, 5.2], [3.0, 3.1, 3.2])
        for s in batch["samples"]["weather"]:
            s["retrieval"] = False  # 快，但无 tavily 检索证据
        dec = self._eval(batch)
        self.assertFalse(
            dec["quality"]["weather_retrieval_ok"],
            f"天气无检索证据应判 weather_retrieval_ok 假: {dec}",
        )
        self.assertFalse(dec["bottom_pass"], f"天气无检索证据应使保底门红: {dec}")

    def test_a2_quality_decider_display_and_schema_rates(self):
        """coverage: PERF-A2 — A2 质量守卫：display 块出现率 + schema 合法率 100%（行为级）。"""
        good = self._batch("a1", [3.0, 3.1, 3.2], [5.0, 5.1, 5.2], [9.0, 9.1, 9.2])
        dec = self._eval(good)
        self.assertEqual(dec["quality"]["display_rate"], 1.0, f"全带块应 1.0: {dec}")
        self.assertEqual(dec["quality"]["schema_rate"], 1.0, f"全合法应 1.0: {dec}")
        self.assertTrue(dec["quality"]["quality_pass"], f"全绿批次应 quality_pass: {dec}")

        # 缺块：一条回复无 ```display（快样本 David 人设的形态，证据 §7.4）
        missing = self._batch("a1", [3.0, 3.1, 3.2], [5.0, 5.1, 5.2], [9.0, 9.1, 9.2])
        missing["samples"]["chat"][0]["reply"] = "你好呀，David。"
        dec_m = self._eval(missing)
        self.assertLess(dec_m["quality"]["display_rate"], 1.0, f"缺块应 <1.0: {dec_m}")
        self.assertFalse(dec_m["quality"]["quality_pass"], f"缺块应 quality_pass 假: {dec_m}")

        # schema 非法：view 不在白名单（真门 validate_display 判红）
        dirty = self._batch("a1", [3.0, 3.1, 3.2], [5.0, 5.1, 5.2], [9.0, 9.1, 9.2])
        dirty["samples"]["chat"][1]["reply"] = (
            "错块。\n```display\n" + json.dumps({"view": "bogus", "data": {}}) + "\n```\n")
        dec_d = self._eval(dirty)
        self.assertLess(dec_d["quality"]["schema_rate"], 1.0, f"非法块应 <1.0: {dec_d}")
        self.assertFalse(dec_d["quality"]["quality_pass"], f"非法块应 quality_pass 假: {dec_d}")

    def test_a2_quality_decider_note_readback(self):
        """coverage: PERF-A2 — 课表题须含备注读回（S-F1.3 AC1/AC4 不回退）（行为级）。"""
        bad = self._batch("a1", [3.0, 3.1, 3.2], [5.0, 5.1, 5.2], [9.0, 9.1, 9.2])
        bad["samples"]["schedule"][0]["note_readback"] = False
        dec = self._eval(bad)
        self.assertFalse(dec["quality"]["note_readback_ok"], f"课表缺备注读回应判假: {dec}")
        self.assertFalse(dec["quality"]["quality_pass"], f"课表缺备注读回应 quality_pass 假: {dec}")

    def test_a3_gate_decider_layer_a3_thresholds(self):
        """coverage: PERF-A3 — a3 层阈值（保底 8/10s、目标 5/7s）行为正确（行为级）。"""
        dec = self._eval(self._batch("a3", [4.5, 4.8, 5.0], [6.5, 6.8, 7.0]))
        self.assertTrue(dec["target_pass"], f"闲聊4.8/课表6.8 应过 a3 目标门: {dec}")
        self.assertEqual(dec["verdict"], "PASS")

        dec2 = self._eval(self._batch("a3", [7.0, 7.5, 8.0], [9.5, 9.8, 10.0]))
        self.assertTrue(dec2["bottom_pass"], f"闲聊7.5/课表9.8 应过 a3 保底门: {dec2}")
        self.assertFalse(dec2["target_pass"], "a3 保底态不应过目标门")
        self.assertEqual(dec2["verdict"], "BOTTOM_ONLY")

        dec3 = self._eval(self._batch("a3", [9.0, 9.5, 10.0], [9.5, 9.8, 10.0]))
        self.assertFalse(dec3["bottom_pass"], "闲聊 9.5s > a3 保底 8s 应破门")
        self.assertEqual(dec3["verdict"], "FAIL")

    # ------------------------------------------------------------------ #
    # PERF-E1/E2 · must-stay-green（引擎 LLM 段声明面 + v6 零回改）（机制级）
    # ------------------------------------------------------------------ #

    def test_e1_provider_llm_segment_unchanged(self):
        """coverage: PERF-E1 — provider.config.yaml LLM 段声明面零改动（机制级 / 契约 §四 V9）。"""
        src = _read_text(PROVIDER_DECL_YAML)
        self.assertTrue(src, f"PERF-E1 RED: {PROVIDER_DECL_YAML} 不在场。")
        self.assertIn(
            'base_url: "http://127.0.0.1:18789/v1"', src,
            "PERF-E1 RED: LLM base_url 声明被改 —— 契约 §四要求 /v1（live 260906：缺 /v1 引擎 404）。",
        )
        self.assertIn(
            'model: "openclaw/default"', src,
            "PERF-E1 RED: LLM model 声明被改 —— 契约 §四钉 openclaw/default。",
        )
        self.assertRegex(src, r"stream:\s*true", "PERF-E1 RED: LLM stream 声明被改。")
        self.assertRegex(
            src, r"forbid_max_tokens:\s*true",
            "PERF-E1 RED: forbid_max_tokens 声明被改 —— 契约 §四 / V9 流式禁 max_tokens。",
        )

    def test_e1_overlay_llm_segment_unchanged(self):
        """coverage: PERF-E1
        coverage: PERF-E2 —— apply-xiaozhi-overlay.sh 引擎 LLM 段渲染面零回改（机制级）。

        E2 铁律：不改引擎 .config.yaml LLM 段（base_url /v1、model_name openclaw/default、
        流式禁 max_tokens）；本齿钉其仓内渲染源面。"""
        src = _read_text(OVERLAY_SH)
        self.assertTrue(src, f"PERF-E1 RED: {OVERLAY_SH} 不在场。")
        self.assertIn(
            "base_url: http://127.0.0.1:18789/v1", src,
            "PERF-E1 RED: overlay 引擎 LLM base_url 被改（契约 §四 / V28 loopback）。",
        )
        self.assertIn(
            "model_name: openclaw/default", src,
            "PERF-E1 RED: overlay 引擎 LLM model_name 被改（契约 §四）。",
        )
        self.assertIn(
            "max_tokens", src,
            "PERF-E1 RED: overlay 的「流式禁 max_tokens」注记消失（契约 §四 / V9）。",
        )

    def test_e1_prompt_v6_byte_hash_unchanged(self):
        """coverage: PERF-E1
        coverage: PERF-E2 —— v6 prompt 归一 sha256 零回改（本包不动 prompt）（落盘守卫）。"""
        self.assertTrue(PROMPT_V6.is_file(), f"PERF-E1 RED: {PROMPT_V6} 不在场。")
        self.assertEqual(
            _sha256_norm(PROMPT_V6), V6_SHA256_NORM,
            "PERF-E1/E2 RED: v6 prompt 内容被回改 —— 本包铁律「prompt v6 零回改、本包不动 prompt」。",
        )

    def test_e1_prompt_wiring_faces_consistent(self):
        """coverage: PERF-E1 — prompt 接线面（声明面 + 渲染面）同指一在场文件（机制级 / 版本无关）。"""
        declared = _wired_prompt_artifact()
        rendered = _overlay_prompt_files()
        self.assertTrue(declared, "PERF-E1 RED: provider.config.yaml 未解析出 prompt 文件名。")
        self.assertTrue(rendered, "PERF-E1 RED: apply-xiaozhi-overlay.sh 未解析出 PROMPT_SRC/DST。")
        self.assertTrue(
            set(rendered) <= {declared},
            f"PERF-E1 RED: 渲染面 {rendered} 与声明面 {declared} 不同指（接线不变式 ADR-001）。",
        )
        self.assertTrue(
            (DEPLOY_DIR / declared).is_file(),
            f"PERF-E1 RED: 接线的 prompt {declared} 不在场（孤儿接线）。",
        )


# =========================================================================== #
# LIVE 齿 —— PERF-A1/A2/A4 证据读（@pytest.mark.live，不 auto-run）
# =========================================================================== #

def _live_json_batches() -> list[Path]:
    found: list[Path] = []
    for root in LIVE_EVIDENCE_DIRS:
        base = root / PROBE_BATCH_DIRNAME
        if base.is_dir():
            found.extend(sorted(base.glob("*.json")))
    return found


def _live_md_corpus() -> str:
    parts: list[str] = []
    for root in LIVE_EVIDENCE_DIRS:
        if not root.is_dir():
            continue
        for p in sorted(root.rglob("*.md")):
            if p.is_file():
                parts.append(_read_text(p))
    return "\n".join(parts)


class PerfK1LiveAcceptance(unittest.TestCase):
    """live 证据读齿：PERF-A1 探针批（标准 3 题 ×N≥3 中位数 + 主机快照 + session 计数）+
    门判定器对证据文件的判定 + PERF-A2 质量证据 + PERF-A4 P0 五项核验。

    probe ≠ exercise 注记（DEV-GOVERNANCE 验收测试纪律 §2）：本类只对 live 相位**真实产出**
    的结构化证据施加判定；真实发 HTTP 的探针批执行器归 builder/verifier live 相位。证据缺位
    = 设计态红（不 auto-run，ARQ-260901-g3；conftest 机械 skip）。
    """

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _require_batches(self) -> list[Path]:
        batches = _live_json_batches()
        self.assertTrue(
            batches,
            "PERF-A1/E3 live RED: no probe-batch evidence 落盘 —— 探针批（三段归因：当晚基线"
            f"重测 / P0 后 / P1 后）须先把结构化批次落 'probe-batches/*.json'（{LIVE_EVIDENCE_DIRS}）。",
        )
        return batches

    def test_live_a1_probe_batches_structured_three_segments(self):
        """coverage: PERF-A1
        coverage: PERF-E3 —— 三段归因批次齐、各题 N≥3、主机快照 + session 计数在场（源锚代理）。"""
        batches = self._require_batches()
        segments = []
        for p in batches:
            obj, err = _load_json(p)
            self.assertIsNotNone(obj, f"PERF-A1 live RED: {p} 不可解析: {err}")
            self.assertIn("segment", obj, f"PERF-A1 live RED: {p} 缺 segment 字段。")
            self.assertIn("samples", obj, f"PERF-A1 live RED: {p} 缺 samples。")
            n_by_topic = {t: len(v) for t, v in obj["samples"].items() if isinstance(v, list)}
            self.assertTrue(n_by_topic, f"PERF-A1 live RED: {p} samples 空。")
            for t, n in n_by_topic.items():
                self.assertGreaterEqual(
                    n, 3,
                    f"PERF-A1 live RED: {p} 题 {t!r} 样本 N={n} < 3 —— 门 = N≥3 中位数（评审 D3）。",
                )
            self.assertIn("host", obj, f"PERF-A1 live RED: {p} 缺主机状态快照（free -m 口径）。")
            self.assertIn("sessions", obj, f"PERF-A1 live RED: {p} 缺 session store 计数。")
            segments.append(obj["segment"])
        self.assertGreaterEqual(
            len(set(segments)), 3,
            f"PERF-A1/E3 live RED: 段数 < 3（实得 {segments}）—— 三段归因：当晚基线重测 / "
            "P0 重启后 / P1 切模型后，分开记不混算。",
        )

    def test_live_a1_gate_decider_applies_to_evidence(self):
        """coverage: PERF-A1 — 判定器真跑 live 证据（两层门），逐段产出可核验 verdict（源锚代理）。"""
        gate = self._gate()
        fn = getattr(gate, "evaluate_batch", None)
        self.assertTrue(callable(fn), "PERF-A1 live RED: probe_gate 缺 evaluate_batch()。")
        batches = self._require_batches()
        verdicts = []
        for p in batches:
            obj, _ = _load_json(p)
            dec = fn(obj)  # 不得崩溃
            self.assertIn(dec.get("verdict"), ("PASS", "BOTTOM_ONLY", "FAIL"),
                          f"PERF-A1 live RED: {p} 判定器未产出合法 verdict: {dec}")
            self.assertIn("bottom_pass", dec, f"PERF-A1 live RED: {p} 判定结果缺 bottom_pass。")
            verdicts.append((obj.get("segment"), dec["verdict"]))
        self.assertTrue(verdicts, "PERF-A1 live RED: 无段可判。")

    def test_live_a2_quality_evidence_recorded(self):
        """coverage: PERF-A2 — 质量守卫证据在场（display 块出现率/schema 合法率 + 天气检索 + 备注读回）（源锚代理）。"""
        gate = self._gate()
        fn = getattr(gate, "evaluate_batch", None)
        self.assertTrue(callable(fn), "PERF-A2 live RED: probe_gate 缺 evaluate_batch()。")
        corpus = _live_md_corpus()
        for needle in ("display", "备注"):
            self.assertIn(
                needle, corpus,
                f"PERF-A2 live RED: live 证据未记 {needle!r} 相关判定 —— A2 全量判定"
                "（display 块 100% / 备注读回 / 天气真实检索）须留痕。",
            )

    def test_live_a4_post_restart_five_point_checklist(self):
        """coverage: PERF-A4 — P0 重启后五项核验证据齐（cron/qqbot/loopback/dist marker/引擎可达）（源锚代理）。"""
        corpus = _live_md_corpus()
        self.assertTrue(corpus, "PERF-A4 live RED: 无 live 证据 md 落盘。")
        needles = {
            "①cron kidpal-reminder-plan": "kidpal-reminder-plan",
            "②qqbot 通道": "qqbot",
            "③18789 loopback": "18789",
            "④char-stroke dist marker": "KidPal C1",
            "⑤引擎容器可达": "可达",
        }
        for label, needle in needles.items():
            self.assertIn(
                needle, corpus,
                f"PERF-A4 live RED: 五项核验证据缺 {label}（marker={needle!r}）—— 重启后五项缺一"
                "即处置并记录。",
            )


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)