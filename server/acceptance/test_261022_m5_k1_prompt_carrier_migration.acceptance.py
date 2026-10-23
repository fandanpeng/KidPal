"""Frozen acceptance tests — 261022-m5-k1-prompt-carrier-migration（K1-E1..E6 host 面）
(subsystem: server/deploy prompt 承载反转 + kidpal agent 工具面收窄；TaskPackage =
delivery/taskpackages/261022-m5-k1-prompt-carrier-migration/taskpackage.json；
上游 = delivery/workstreams/device-memory-overhaul/PROPOSAL.md §2/§3.1 FROZEN v1.0 +
delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md T3/T7
FROZEN v1.1，ADR-001 承载不变式重定义已随 arch 批 0411e957 落稿)。

背景：v11 静态语义作为引擎 `prompt:` 键经 extraSystemPrompt 挂在 OpenClaw 缓存边界
之下（22,116 prompt tokens 中 cache miss 14,948，TECH-261022 证据 2）。本包把静态
语义反转进 workspace AGENTS.md 稳定前缀（缓存边界之上），引擎只发有界动态 context
小块；kidpal agent 工具面收窄（K1-E4 落
test_261022_openclaw_kidpal_tools.acceptance.py，分工件）。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；offline 可跑 / 导入期零网络 I/O）

  K1-E1 · AGENTS.md 静态语义迁移（test_k1_e1_*）：
    - workspace-kidpal/AGENTS.md 承载 v11 全量静态语义（内容驱动并集锚、容换写）：
      回合路由五行（孩子对话 / 实时事实 / 查字 / 课表问答 / 提醒 cron）+ 各域纪律
      （回复格式 / 课表 / 查字 / 故事 / 提醒）+ 底线句吸收（v11 三条不变底线并入
      既有底线节）。**行级定位判法**：回合路由纪律须落在「含回合类型名的行（或
      行段）」上——措辞换写不误红、纪律语义被删才红；builder 以 v11 的表格形态
      迁入最稳（v11 行即单行表格行）。
    - AGENTS.md 保有「静态语义单一事实源」自声明（261017 arch 批已落 = 本批
      must-stay-green 半，回退才红）。
    - v11 历史文件原地冻结零 diff：sha256（LF 归一口径 = canonical blob 口径，
      检出环境无关；v11 为 LF 落盘，raw == LF 归一同值）=
      7698fe49ca1c8bc2087ffa3c316c3037c443d4f97b6ea60d6c106c5a16359168。
      计算式 = sha256(read_text.replace("\r\n","\n"))。

  K1-E2 · 动态 context 模板单件（test_k1_e2_*）：
    - 新增 server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt，在场非空，
      **有界**：含本回合时间 / 日期锚 + 「课表摘要」占位锚；**不含静态纪律**
      （静态纪律半同义负锚组零命中）。
    - 二选一定形判法（冻结）：两个候选（kidpal-dynamic-context.v1.txt /
      kidpal-agent-prompt.txt）中，充当「.config.yaml prompt: 块内容源」的候选
      **恰一个**（离线渲染产物反解，见 K1-E3 渲染齿；两候选同时作为内容源 =
      红即「两个动态模板件并存」）。prompt_template（引擎装配壳）与动态块内容源
      是两个角色——装配壳（{{base_prompt}} 包裹层）不构成第二动态模板件。
      TODO-BUILDER：采用哪种定形（新件取代 / 原地改写）由 builder 落；推荐
      「新件取代」（arch T3 OQ-8 单一事实源原则）；若取「原地改写」，
      kidpal-dynamic-context.v1.txt 只作指针壳、不得复载动态内容。

  K1-E3 · 承载三面接线（ADR-001 重定义，test_k1_e3_*）：
    - 静态面集合相等：渲染面（live workspace AGENTS.md，manifest 登记铺放）==
      声明面（OPENCLAW_SYSTEM_PROMPT_CARRIER 静态承载声明 + provider.config.yaml
      静态承载键，键形归 P1 实现——本齿以「键值行引用 AGENTS.md 路径」的形态
      判，不钉具体键名）== 被测面（探针直读同一 AGENTS.md，归 live 观测）。
    - 动态面集合相等：渲染面（.config.yaml prompt: 值 == 动态模板内容，**离线
      真跑 apply-xiaozhi-overlay.sh 验证**）== 声明面（provider.config.yaml 动态
      模板键引用内容源模板文件）== 被测面（探针请求同源，归 live 观测）。
    - manifest.yaml 条目同步（撤 v11 专属锚「实时事实回合」、增动态块锚；内容源
      模板文件入清单）。
    - .config.yaml marker 锚集 ↔ 渲染产物一致性：**每个锚都必须命中离线渲染
      产物**（行为级），且锚集不再含 v11 静态内容锚「实时事实回合」（负向）。
    - verify-deploy.sh 三时机语义随改 = 脚本恒 manifest 驱动（md5 / marker 两核验
      路径在场），语义随 manifest 同批翻——不钉脚本内具体锚值。

  K1-E5 · supersede 留痕存在性（test_k1_e5_*）：六件既有冻结验收的语义换代
    痕迹在场（零静默改写的机判面）——v7 / 261010 / m3k3 / m4k3 / perf-k1 五件
    SUPERSEDED-BY（m3k3 / m4k3 / perf-k1 为 planner 2026-10-23 授权扩面，同纪律）
    + 261017 件 SUPERSEDED-REVIEW 复核记录（零断言改动）。**扩单申报**：
    perf-k1 探针 resolver 产品面 server/deploy/perf-k1/probe_gate.py::
    resolve_prompt_artifact 的 vN 解析须由 builder 同批改指 AGENTS.md
    （新拓扑探针被测面契约见 test_a1_probe_prompt_resolution_version_agnostic
    superseded docstring；perf-k1/ 不在本包 changeManifest，须 planner 扩单）。

  K1-E6 · 已知窗口（test_k1_e6_*）：AGENTS.md 仓内镜像改动后、live 铺放前
    verify-deploy.sh 报 md5 漂移属预期已知窗口（launch-card 判红豁免在案），
    本齿只钉「md5 核验路径在场且未被本包弱化」（无 AGENTS.md 特例跳过），
    不判 live md5 绿红（本包不单独铺放该文件——护栏真空禁令）。

  K1-E7 / K1-E8 = live 观测项（**非自动断言**，live-manual 层诚实申报，本类不落
    测试）：K1-E7 = human 部署后复测单轮 DeepSeek prompt_tokens ≤11,000（基线
    22,116）+ cache hit 覆盖稳定前缀 + 工具面 audit 可见工具 = 4 项、57KB schema
    消失；K1-E8 = SOUL/USER live 清理（human 2026-10-22 门 1 终裁 A1，launch
    card 人工步骤，随 deploy.sh 铺放批次处理）。非确定性来源：token 体积与 cache
    命中是部署后运行时观测量、LLM 遵循度 offline 不可机判；host 面转绿后停门等
    human（awaiting-human:live-deploy），Pod 不自行上 ECS。

===========================================================================
强度申报（契约条目 → 承载断言 → 性质档）：

  · E1 迁移内容锚                        → test_k1_e1_agents_md_static_semantics_migration → 源锚代理
    （AGENTS.md 的产出物就是措辞本身；并集半同义组容换写；「prompt 写明 ≠ LLM
    遵循」行为等价假设显式化——遵循度归 K1-E7 真机/部署轮目击）
  · E1 自声明在场                        → test_k1_e1_agents_md_self_declaration_single_source → 机制级
  · E1 v11 冻结零 diff                   → test_k1_e1_v11_frozen_zero_diff → 机制级（sha256）
  · E2 动态件在场 / 有界 / 无纪律        → test_k1_e2_dynamic_context_template_present_and_bounded → 源锚代理
  · E2 单件接线（离线渲染反解）          → test_k1_e2_single_dynamic_template_carrier_wired → 行为级（真跑）
  · E3 静态面声明接线                    → test_k1_e3_static_face_declaration_wiring → 机制级
  · E3 动态面声明接线                    → test_k1_e3_dynamic_face_declaration_wiring → 机制级
  · E3 渲染产物 = 有界动态块             → test_k1_e3_offline_render_prompt_is_bounded_dynamic_block → 行为级（真跑）
  · E3 锚集 ↔ 渲染产物一致               → test_k1_e3_marker_anchors_match_rendered_product → 行为级（真跑）
  · E5 supersede 留痕                    → test_k1_e5_supersede_traces_present → 机制级
  · E6 已知窗口核验路径                  → test_k1_e6_agents_md_md5_drift_path_known_window → 机制级

缺口（强度低于契约要求处，逐条）：
  · offline 只证仓内镜像与渲染函数行为；live AGENTS.md 铺放、探针被测面同源、
    prompt_tokens/cache 命中、工具列表 = 4 项归 K1-E7 live 轮（human 部署，停门
    停等，本 Pod 不自行上 ECS）。
  · E3 静态面「键形归 P1 实现」——本齿以路径引用形态判（不钉键名）；键名定形后
    可由 tighten-only 窗口补强为键名精确断言。
  · 行为等价假设（显式化）：① 离线渲染（临时 XIAOZHI_ROOT + 假 token）⇔ ECS
    真机渲染同源同逻辑（渲染函数无环境分支——现脚本仅 secrets 读取与环境变量
    路径差异）；② prompt: 块内容 == 注入源文件内容（现行渲染函数 sed 缩进 2 空
    格注入，本齿按 2 空格反解；builder 改注入形态须同批调整本判法）；③ AGENTS.md
    承载 = OpenClaw stable project context 组装（参考实现 system-prompt.ts 证据链
    在 arch T3，offline 不可证运行时组装序）。
  · fixture 自含性声明：全部断言直读 repo 内制品 + 临时目录渲染产物；无外部资产
    / 时间真值依赖；唯一环境前提 = bash（离线渲染）与 PyYAML（manifest 解析，
    与 verify-deploy.sh 同一解析器），缺任一 = infra 红、不静默跳过。行为 seam
    （静态语义 → LLM 行为、承载反转 → token 体积）的驱动方 = K1-E7 部署轮
    （human 在场），offline 侧无驱动方 = 已申报盲区。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/
  （standalone: python 本文件）

RED 基线（冻结时点树，2026-10-23，builder 未动工）：本件 E1 迁移内容齿 / E2 全部 /
E3 接线与渲染齿 = 设计态红（AGENTS.md 未迁入 v11 语义 / 动态模板件缺失 / carrier
与 provider 未指向新不变式 / 渲染产物仍是 v11 全量 / 锚集仍含「实时事实回合」=
真实缺失行为）；E1 自声明齿、v11 冻结齿、E5 留痕齿（supersede 由 acceptance-author
同批落）、E6 机制齿 = must-stay-green（回退才红）。既有套件红基线（冻结前实跑）=
246 passed / 4 deselected / 33 subtests passed，全绿。
"""

from __future__ import annotations

import hashlib
import os
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

try:
    import yaml  # noqa: F401  (lazy import in helpers; 环境前提缺失 = infra 红)
    _HAS_YAML = True
except ImportError:
    _HAS_YAML = False

REPO_ROOT = Path(__file__).resolve().parents[2]
DEPLOY_DIR = REPO_ROOT / "server" / "deploy"
ACCEPTANCE_DIR = REPO_ROOT / "server" / "acceptance"

K1_AGENTS_MD = DEPLOY_DIR / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md"
K1_DYNAMIC_CTX = DEPLOY_DIR / "openclaw-agents" / "kidpal-dynamic-context.v1.txt"
K1_AGENT_PROMPT = DEPLOY_DIR / "kidpal-agent-prompt.txt"
K1_CARRIER = DEPLOY_DIR / "OPENCLAW_SYSTEM_PROMPT_CARRIER"
K1_PROVIDER = DEPLOY_DIR / "provider.config.yaml"
K1_OVERLAY_SH = DEPLOY_DIR / "apply-xiaozhi-overlay.sh"
K1_MANIFEST = DEPLOY_DIR / "manifest.yaml"
K1_VERIFY_SH = DEPLOY_DIR / "verify-deploy.sh"
K1_V11 = DEPLOY_DIR / "openclaw-system-prompt.v11.md"

# v11 冻结时点内容 hash（LF 归一口径 = canonical blob 口径；v11 为 LF 落盘，raw 同值）。
# 计算式 = sha256(read_text.replace("\r\n", "\n"))。2026-10-23 K1 批冻结实测。
K1_V11_SHA256_LF = "7698fe49ca1c8bc2087ffa3c316c3037c443d4f97b6ea60d6c106c5a16359168"

# 两个动态模板候选（K1-E2 二选一定形判法的论域）。
K1_DYNAMIC_CANDIDATES = ("kidpal-dynamic-context.v1.txt", "kidpal-agent-prompt.txt")

SUPERSEDE_TAG = "SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E"
SUPERSEDE_REVIEW_TAG = "SUPERSEDED-REVIEW 261022-m5-k1-prompt-carrier-migration"


# --------------------------------------------------------------------------- #
# 读盘 / 渲染 helpers（仓根由 __file__ 显式推导，不依赖调用点 cwd）
# --------------------------------------------------------------------------- #

def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _sha256_lf(path: Path) -> str:
    txt = _read_text(path)
    return hashlib.sha256(txt.replace("\r\n", "\n").encode("utf-8")).hexdigest()


def _require_yaml(testcase, tag: str):
    if not _HAS_YAML:
        testcase.fail(
            f"{tag} RED(infra): PyYAML 不在位——manifest 静态解析前置缺失"
            "（verify-deploy.sh 同一依赖）: ImportError")


def _k1_manifest_entries(testcase, tag: str) -> list:
    _require_yaml(testcase, tag)
    if not K1_MANIFEST.is_file():
        testcase.fail(f"{tag} RED: {K1_MANIFEST} 不在场。")
    try:
        doc = yaml.safe_load(K1_MANIFEST.read_text(encoding="utf-8"))
    except Exception as e:  # noqa: BLE001 — 解析错误是 RED，不是 crash
        testcase.fail(f"{tag} RED: manifest.yaml 解析失败: {e!r}")
    entries = doc.get("entries") if isinstance(doc, dict) else None
    testcase.assertTrue(
        isinstance(entries, list) and entries,
        f"{tag} RED: manifest.yaml 缺 entries 列表（单一事实源被破坏）。")
    return entries


def _k1_config_marker_anchors(testcase, tag: str) -> list[str]:
    """manifest.yaml 中 .config.yaml（verify=marker）条目的 anchors 并集。"""
    _require_yaml(testcase, tag)
    try:
        doc = yaml.safe_load(K1_MANIFEST.read_text(encoding="utf-8"))
    except Exception as e:  # noqa: BLE001
        testcase.fail(f"{tag} RED: manifest.yaml 解析失败: {e!r}")
    anchors: list[str] = []
    for entry in (doc or {}).get("entries", []) or []:
        if isinstance(entry, dict) \
                and str(entry.get("dst") or "").endswith(".config.yaml") \
                and entry.get("verify") == "marker":
            anchors.extend(str(a) for a in (entry.get("anchors") or []))
    testcase.assertTrue(
        anchors, f"{tag} RED: manifest 无 .config.yaml marker 条目/锚集。")
    return anchors


# --------------------------------------------------------------------------- #
# 离线渲染（K1-E2/E3 共用，进程内缓存一次）
# --------------------------------------------------------------------------- #

_RENDER_CACHE: dict = {}


def _offline_render(testcase, tag: str) -> dict:
    """离线真跑 apply-xiaozhi-overlay.sh（行为级）：临时 XIAOZHI_ROOT + 假 token
    （≥22 字符），零网络、零 ECS 依赖、零 secret 读取（secrets 指向不存在文件）。
    返回 {rc, stdout, stderr, rendered, prompt_block}。"""
    if "result" in _RENDER_CACHE:
        return _RENDER_CACHE["result"]
    testcase.assertTrue(
        K1_OVERLAY_SH.is_file(),
        f"{tag} RED: 渲染脚本不在场 —— 要求 {K1_OVERLAY_SH}。")
    with tempfile.TemporaryDirectory() as td:
        root = Path(td) / "render-root"
        env = dict(os.environ)
        env["XIAOZHI_ROOT"] = root.as_posix()
        env["XIAOZHI_DEVICE_TOKEN"] = "K1-FROZEN-OFFLINE-RENDER-TOKEN-0123456789"
        env["XIAOZHI_SECRETS"] = (root / "absent-secrets.env").as_posix()
        env.pop("OPENCLAW_TOKEN", None)
        env.pop("DASHSCOPE_API_KEY", None)
        try:
            proc = subprocess.run(
                ["bash", K1_OVERLAY_SH.as_posix()],
                capture_output=True, text=True, encoding="utf-8",
                errors="replace", env=env, timeout=120)
            rc, out, err = proc.returncode, proc.stdout, proc.stderr
        except FileNotFoundError as e:
            testcase.fail(
                f"{tag} RED(infra): bash 不在位——离线渲染环境前提缺失: {e!r}")
        rendered = ""
        cfg = root / ".config.yaml"
        if cfg.is_file():
            rendered = cfg.read_text(encoding="utf-8", errors="replace")
    result = {
        "rc": rc, "stdout": out, "stderr": err, "rendered": rendered,
        "prompt_block": _prompt_block(rendered),
    }
    _RENDER_CACHE["result"] = result
    return result


def _prompt_block(rendered: str) -> str:
    """渲染产物 .config.yaml 的顶层 `prompt: |` literal block 反解（dedent 2 空格，
    现行渲染函数 sed 's/^/  /' 注入口径）。"""
    lines = rendered.splitlines()
    start = None
    for i, l in enumerate(lines):
        if l.rstrip() == "prompt: |":
            start = i + 1
            break
    if start is None:
        return ""
    out: list[str] = []
    for l in lines[start:]:
        if l.strip() and not l.startswith("  "):
            break
        out.append(l[2:] if l.startswith("  ") else "")
    return "\n".join(out)


# --------------------------------------------------------------------------- #
# K1-E1 静态语义迁移锚组（口径对齐 v7/PN/RF/PV 各批冻结锚，自含副本——跨冻结件
# import 会把冻结件焊死，故复制不引用）。并集半同义组：措辞换写不误红、纪律语义
# 被删才红。行级定位 = 含回合类型名的行（builder 以 v11 表格形态迁入最稳）。
# --------------------------------------------------------------------------- #

# 实时事实回合行纪律半锚组（RF-A2 口径）。
_K1_RT_HINT = ("我去查", "查一下", "先播", "先说", "提示")
_K1_RT_QUERY = ("查询", "搜索", "联网", "真查")
_K1_RT_BASIS = ("依据", "根据", "基于")
_K1_RT_RESULT = ("查询结果", "搜索结果", "检索结果")
_K1_RT_DEGRADE = ("没查到", "查不到", "不知道", "查询失败", "失败", "超时")
_K1_RT_HONESTY = ("诚实", "如实", "引导", "换个问法", "换个话题")
_K1_RT_FABRICATION = ("编造", "凭印象", "凭空", "瞎编")
_K1_FALSE_CLAIM = ("虚称", "谎称", "假称", "冒称", "佯称", "假装", "已查询")
_K1_PARAPHRASE_ADAPT = ("儿童化", "改写", "听得懂", "转述")
_K1_PARAPHRASE_COPY = ("照搬", "逐字", "原样")
_K1_PROHIBITION = ("不得", "禁止", "严禁", "违规", "不许", "不进入")
# 反叙述泄漏类别词（PN-A2 口径）。
_K1_NARRATION_CATEGORIES = (
    "回合判定", "文件路径", "流程步骤", "内部动作", "内部推理", "内部独白")
_K1_VOICE_HALF = ("语音", "播报", "念出", "念给", "TTS")
# 课表识别特征扩列（PV-A1 口径）。
_K1_UNIVERSAL = ("凡提到", "只要提到", "任何提到", "凡是提到", "凡涉及", "只要涉及",
                 "凡是涉及")
_K1_WEEKDAY_RE = re.compile(r"周[一二三四五六日天1-7Xx×]|星期[一二三四五六日天1-7Xx×]")
_K1_SCHEDULE_SUBJECT = ("课表", "课程", "上课", "上什么课", "几点上课", "要带什么")

# 回合路由五行：回合类型名 → 必须命中的锚组列表（每组至少一词，全组须齐）。
K1_ROUTING_ROWS = {
    "孩子对话回合": [
        ("一回合作答", "同一回合", "直接作答", "当回合"),
        ("不调用任何工具", "禁工具", "禁用工具", "不读取任何文件", "禁读文件"),
    ],
    "实时事实回合": [
        _K1_RT_HINT, _K1_RT_QUERY,
        ("依据结果双半",),  # 占位：双半同现由专门断言承载（见 test 体）
    ],
    "查字回合": [
        ("绝对禁工具", "不调用任何工具", "禁用工具", "一律不调工具"),
        ("最小意图块",),
        ("meaning", "中文释义"),
        ("坐标",),
        ("静态字库", "不生成", "严禁", "禁自生成", "不由", "禁复述"),
    ],
    "课表问答回合": [
        ("先读",),
        ("schedule.md",),
        ("读不到", "查不到"),
        ("诚实", "如实", "不知道"),
        _K1_UNIVERSAL,
        ("后天",),
        ("大后天",),
        ("备注",),
    ],
    "提醒 cron 回合": [
        ("备注",),
        ("语义",),
        ("entry_key",),
        ("不算时刻", "不生成 id", "不写 pending", "不生成 id、"),
    ],
}


def _k1_rows(body: str, round_name: str) -> list[str]:
    return [l for l in body.splitlines() if round_name in l]


def _k1_baseline_window(body: str) -> str:
    """底线节窗口（内容驱动：含「底线」的标题行至下一标题；找不到则退化全文）。"""
    lines = body.splitlines()
    heads = [i for i, l in enumerate(lines) if l.startswith("#")]
    start = next((i for i in heads if "底线" in lines[i]), None)
    if start is None:
        return body
    end = next((i for i in heads if i > start), len(lines))
    return "\n".join(lines[start:end])


class K1PromptCarrierMigrationAcceptance(unittest.TestCase):

    maxDiff = None

    # ------------------------------------------------------------------ #
    # K1-E1 · AGENTS.md 静态语义迁移
    # ------------------------------------------------------------------ #

    def _agents_body(self, tag: str) -> str:
        self.assertTrue(
            K1_AGENTS_MD.is_file(),
            f"{tag} RED: {K1_AGENTS_MD} 不在场——静态语义主承载镜像缺失（K1-E1）。")
        body = _read_text(K1_AGENTS_MD)
        self.assertTrue(body.strip(), f"{tag} RED: AGENTS.md 为空。")
        return body

    def test_k1_e1_agents_md_static_semantics_migration(self):
        """coverage: K1-E1 — AGENTS.md 承载 v11 全量静态语义：回合路由五行 +
        各域纪律 + 底线句吸收（内容驱动并集锚、行级定位、容换写）。"""
        tag = "K1-E1"
        body = self._agents_body(tag)
        # ① 回合路由五行，逐行定位并核纪律锚组。
        for round_name, groups in K1_ROUTING_ROWS.items():
            rows = _k1_rows(body, round_name)
            self.assertTrue(
                rows,
                f"{tag} RED: AGENTS.md 无含「{round_name}」的行——回合路由未迁入"
                "（v11 回合类型与纪律路由表缺行；行级定位要求：纪律落在含回合类型名"
                "的行上，表格形态最稳）。")
            row = "\n".join(rows)
            for group in groups:
                if group == ("依据结果双半",):
                    continue  # 双半同现专门断言
                self.assertTrue(
                    any(w in row for w in group),
                    f"{tag} RED: 「{round_name}」行缺纪律锚组 {group}——v11 该回合"
                    "纪律未迁入或被稀释（措辞换写可、语义删除不可）。")
        # ② 实时事实回合行的双半同现组（依据×结果 / 禁编造×禁止 / 改写×不照搬 /
        #    降级×诚实——RF-A2 口径，单单词不判绿）。
        rt_row = "\n".join(_k1_rows(body, "实时事实回合"))
        self.assertTrue(
            any(w in rt_row for w in _K1_RT_BASIS)
            and any(w in rt_row for w in _K1_RT_RESULT),
            f"{tag} RED: 实时事实回合行无「只依据查询结果作答」双半口径。")
        self.assertTrue(
            any(w in rt_row for w in _K1_RT_FABRICATION)
            and any(w in rt_row for w in _K1_PROHIBITION),
            f"{tag} RED: 实时事实回合行无禁编造×禁止语义双半（TECH-261012 根治锚）。")
        self.assertTrue(
            any(w in rt_row for w in _K1_FALSE_CLAIM),
            f"{tag} RED: 实时事实回合行无禁虚称已查锚。")
        self.assertTrue(
            any(w in rt_row for w in _K1_PARAPHRASE_ADAPT)
            and any(w in rt_row for w in _K1_PARAPHRASE_COPY),
            f"{tag} RED: 实时事实回合行无联网转述边界双半（NFR-SAFE-1）。")
        # ③ 课表问答回合行：时间面覆盖 ≥3/6 + 主题面 ≥5/6 + 反叙述对称纪律
        #   （PV-A1 / PN-A2 口径）。
        sched_row = "\n".join(_k1_rows(body, "课表问答回合"))
        time_hits = [w for w in ("今天", "明天", "周几", "这周", "下周", "号")
                     if w in sched_row]
        self.assertGreaterEqual(
            len(time_hits), 3,
            f"{tag} RED: 课表问答回合行时间面覆盖不足（命中 {time_hits}，须 ≥3/6）"
            "——TECH-261017 识别特征扩列未迁入。")
        self.assertTrue(
            _K1_WEEKDAY_RE.search(sched_row),
            f"{tag} RED: 课表问答回合行缺「周X / 星期X」模式锚。")
        subject_hits = [w for w in _K1_SCHEDULE_SUBJECT if w in sched_row]
        self.assertGreaterEqual(
            len(subject_hits), 5,
            f"{tag} RED: 课表问答回合行主题面覆盖不足（命中 {subject_hits}，"
            "须 ≥5/6）。")
        narration_hits = [w for w in _K1_NARRATION_CATEGORIES if w in sched_row]
        self.assertGreaterEqual(
            len(narration_hits), 2,
            f"{tag} RED: 课表问答回合行缺对称反叙述纪律（泄漏类别词仅命中 "
            f"{narration_hits}，须 ≥2/6）。")
        self.assertTrue(
            any(w in sched_row for w in _K1_VOICE_HALF)
            and any(w in sched_row for w in _K1_PROHIBITION),
            f"{tag} RED: 课表问答回合行反叙述纪律缺语音半或禁止半"
            "（TECH-261010 根治锚）。")
        # ④ 各域纪律（正文级并集）。
        for needle, why in (
            ("display", "回复格式：display 块承载缺失"),
            ("白名单", "view 白名单纪律缺失"),
            ("story_list", "故事域：禁产 story_list 锚缺失（契约门 Q7）"),
            ("story_text", "故事域：禁产 story_text 锚缺失"),
            ("帮我记下来", "故事域：双指令意图边界缺失（S-F2.2）"),
            ("问答框", "课表域：问答框口径缺失（M3K3-A1）"),
            ("薄载荷", "课表域：显示意图薄载荷口径缺失"),
            ("机械计算", "课表域：网格不由 LLM 生成口径缺失"),
        ):
            self.assertIn(
                needle, body,
                f"{tag} RED: AGENTS.md 缺「{needle}」——{why}（v11 各域纪律迁移面）。")
        self.assertTrue(
            any(w in body for w in ("严禁生成", "不要生成", "禁生成", "不生成",
                                    "严禁产", "禁产")),
            f"{tag} RED: 故事域无「禁产 story JSON」禁令语义。")
        self.assertTrue(
            any(w in body for w in ("HTML", "附录", "details")),
            f"{tag} RED: 回复格式无「禁附录/HTML 结构」锚。")
        # ⑤ 底线句吸收：v11 三条不变底线并入底线节（课表永不编造 / 查字绝对禁
        #   工具 / 实时信息第三不变量）。
        bl = _k1_baseline_window(body)
        self.assertTrue(
            ("课表" in bl and any(w in bl for w in ("永不编造", "严禁凭印象"))),
            f"{tag} RED: 底线节无「课表问题永不编造」不变量——v11 底线句未吸收。")
        self.assertTrue(
            ("查字" in bl and any(w in bl for w in ("绝对禁工具", "禁工具",
                                                    "不调用任何工具"))),
            f"{tag} RED: 底线节无「查字回合绝对禁工具」不变量——v11 底线句未吸收。")
        self.assertTrue(
            ("实时信息" in bl and any(w in bl for w in _K1_RT_FABRICATION)
             and any(w in bl for w in _K1_FALSE_CLAIM)),
            f"{tag} RED: 底线节无第三不变量（实时信息无查询支撑永不编造、永不虚称"
            "已查询）——v11 底线句未吸收。")

    def test_k1_e1_agents_md_self_declaration_single_source(self):
        """coverage: K1-E1 — AGENTS.md 保有「静态语义单一事实源」自声明（arch 批
        已落 = must-stay-green；回退才红）。"""
        tag = "K1-E1"
        body = self._agents_body(tag)
        self.assertIn(
            "静态语义", body,
            f"{tag} RED: AGENTS.md 自声明丢失「静态语义」——arch 批 T3 第 2 条"
            "自声明反转被回退。")
        self.assertIn(
            "单一事实源", body,
            f"{tag} RED: AGENTS.md 自声明丢失「单一事实源」——主承载地位声明被回退。")
        self.assertTrue(
            any(w in body for w in ("以本文件为准", "以本镜像为准", "以 AGENTS.md 为准")),
            f"{tag} RED: AGENTS.md 自声明丢失「冲突以本文件为准」语义——"
            "主承载裁定句被回退。")

    def test_k1_e1_v11_frozen_zero_diff(self):
        """coverage: K1-E1 — v11 历史文件原地冻结零 diff（sha256 LF 归一口径 =
        canonical blob 口径；v11 = 引擎注入承载的终点版，承载反转后不再 bump）。"""
        tag = "K1-E1"
        self.assertTrue(
            K1_V11.is_file(),
            f"{tag} RED: v11 文件被删除——历史版本共存可 diff 铁律被破坏（OQ-8："
            "v11 原地冻结，唯一静态件 = AGENTS.md，删除 ≠ 冻结）。")
        self.assertEqual(
            _sha256_lf(K1_V11), K1_V11_SHA256_LF,
            f"{tag} RED: v11 本体被回改——历史版本一律不改（版本化落盘惯例；"
            "如确需改动属治理裁决，须 human ruling 后更新本钉）。")

    # ------------------------------------------------------------------ #
    # K1-E2 · 动态 context 模板单件
    # ------------------------------------------------------------------ #

    def test_k1_e2_dynamic_context_template_present_and_bounded(self):
        """coverage: K1-E2 — kidpal-dynamic-context.v1.txt 在场非空、有界（时间/
        日期 + 课表摘要占位）、不含静态纪律（负锚组零命中）。"""
        tag = "K1-E2"
        self.assertTrue(
            K1_DYNAMIC_CTX.is_file(),
            f"{tag} RED: {K1_DYNAMIC_CTX} 未落盘——动态 context 模板单件缺失"
            "（K1-E2 / arch T3 §5.5.5 动态件）。")
        body = _read_text(K1_DYNAMIC_CTX)
        self.assertTrue(body.strip(), f"{tag} RED: 动态模板件为空。")
        # 有界：本回合时间 / 日期锚。
        self.assertTrue(
            any(w in body for w in ("时间", "日期")),
            f"{tag} RED: 动态块缺时间/日期锚——回合内动态信息（本回合时间/日期）"
            "无承载。")
        self.assertTrue(
            any(w in body for w in ("今天", "星期", "周几", "weekday")),
            f"{tag} RED: 动态块缺今天/星期事实位——课表「后天星期几」事实来源缺失。")
        # 有界：课表摘要占位（R8：课表机械注入位置 = 本包建立的动态 context 块；
        # 注入内容归 P5，本包只建占位面）。
        self.assertTrue(
            any(w in body for w in ("课表摘要", "课表占位", "schedule_summary",
                                    "schedule summary")),
            f"{tag} RED: 动态块缺「课表摘要」占位锚——R8 课表机械注入的落位面缺失"
            "（TODO-BUILDER：占位措辞建议用「课表摘要」；另择名须经 planner 调整"
            "本锚组）。")
        # 有界（负向）：不含静态纪律（静态纪律只进 AGENTS.md）。
        for token in ("回合类型", "严禁凭印象", "绝对禁工具", "story_list",
                      "story_text", "最小意图块", "帮我记下来", "绝不称呼"):
            self.assertNotIn(
                token, body,
                f"{tag} RED: 动态块含静态纪律锚「{token}」——静态语义进动态块 = "
                "缓存边界之下重复承载（K1-E2 有界禁令 / T3「不含静态纪律」）。")

    def test_k1_e2_single_dynamic_template_carrier_wired(self):
        """coverage: K1-E2 — 二选一定形机判：两候选中充当 .config.yaml prompt:
        块内容源的恰一个（离线渲染产物反解；两候选同被渲染进 prompt 块 = 两个
        动态模板件并存 = 红）。装配壳（{{base_prompt}} 包裹层）不构成第二动态
        模板件。"""
        tag = "K1-E2"
        render = _offline_render(self, tag)
        self.assertEqual(
            render["rc"], 0,
            f"{tag} RED: apply-xiaozhi-overlay.sh 离线渲染失败 rc={render['rc']}——"
            f"渲染面不可用。stdout={render['stdout']!r} stderr={render['stderr']!r}")
        block = render["prompt_block"]
        self.assertTrue(
            block.strip(),
            f"{tag} RED: 渲染产物无 `prompt: |` 块（或注入形态偏离现行 2 空格"
            "缩进判法）——动态面渲染面无内容源。")
        block_norm = block.replace("\r\n", "\n").strip()
        sources = []
        for name in K1_DYNAMIC_CANDIDATES:
            body = _read_text(DEPLOY_DIR / name)
            if body.strip() and body.replace("\r\n", "\n").strip() \
                    and body.replace("\r\n", "\n").strip() in block_norm:
                sources.append(name)
        self.assertEqual(
            len(sources), 1,
            f"{tag} RED: prompt: 块内容源候选命中 {sources}（须恰一）——"
            + ("两个动态模板件并存（K1-E2 二选一禁令）。" if len(sources) > 1
               else "渲染产物既非 kidpal-dynamic-context.v1.txt 也非改写后的 "
                    "kidpal-agent-prompt.txt（现树 = v11 全量直注，承载反转未落）。"))

    # ------------------------------------------------------------------ #
    # K1-E3 · 承载三面接线（ADR-001 重定义）
    # ------------------------------------------------------------------ #

    def test_k1_e3_static_face_declaration_wiring(self):
        """coverage: K1-E3 — 静态面：carrier 静态承载声明 + provider.config.yaml
        静态承载键（键值行引用 AGENTS.md，键形归 P1）+ manifest AGENTS.md 登记
        条目（渲染面铺放 owner=deploy.sh、verify=md5）。"""
        tag = "K1-E3"
        carrier = _read_text(K1_CARRIER)
        self.assertTrue(carrier.strip(), f"{tag} RED: carrier 不在场/为空。")
        self.assertIn(
            "AGENTS.md", carrier,
            f"{tag} RED: carrier（OPENCLAW_SYSTEM_PROMPT_CARRIER）未声明 AGENTS.md "
            "静态承载——静态面声明面缺位（ADR-001 重定义：静态面 = AGENTS.md + "
            "carrier + provider 静态键）。")
        self.assertTrue(
            any(w in carrier for w in ("静态语义", "主承载", "静态承载")),
            f"{tag} RED: carrier 未声明 AGENTS.md 为静态语义主承载——承载反转后 "
            "carrier 口径未随批翻。")
        self.assertIn(
            "openclaw-system-prompt.v11.md", carrier,
            f"{tag} RED: carrier 丢失 v11 声明——版本演进史唯一全量副本应保留 v11 "
            "终点版声明（历史文件冻结可 diff）。")
        provider = _read_text(K1_PROVIDER)
        self.assertTrue(provider.strip(), f"{tag} RED: provider.config.yaml 缺失。")
        self.assertIsNotNone(
            re.search(r"(?m)^\s*[A-Za-z_][\w.]*\s*:[^\n]*AGENTS\.md", provider),
            f"{tag} RED: provider.config.yaml 无引用 AGENTS.md 的键值行——静态承载"
            "声明面缺位（键形归 P1；判法 = 存在 key: value 行且值含 AGENTS.md 路径）。")
        entries = _k1_manifest_entries(self, tag)
        agents_entries = [
            e for e in entries if isinstance(e, dict)
            and "workspace-kidpal/AGENTS.md" in str(e.get("src") or "")]
        self.assertTrue(
            agents_entries,
            f"{tag} RED: manifest 缺 workspace-kidpal/AGENTS.md 条目——静态面渲染面"
            "（live /root/.openclaw/workspace-kidpal/AGENTS.md）无铺放登记。")
        entry = agents_entries[0]
        self.assertEqual(
            str(entry.get("dst") or ""),
            "/root/.openclaw/workspace-kidpal/AGENTS.md",
            f"{tag} RED: AGENTS.md 条目 dst 漂移（实得 {entry.get('dst')!r}）。")
        self.assertEqual(
            entry.get("verify"), "md5",
            f"{tag} RED: AGENTS.md 条目 verify ≠ md5——每日核验面缺失（K1-E6 前置）。")
        self.assertEqual(
            entry.get("owner"), "deploy.sh",
            f"{tag} RED: AGENTS.md 条目 owner ≠ deploy.sh——铺放单一写者被破坏。")

    def test_k1_e3_dynamic_face_declaration_wiring(self):
        """coverage: K1-E3 — 动态面：provider.config.yaml 动态模板键引用内容源
        模板文件（键形归 P1）+ manifest 登记内容源模板条目。"""
        tag = "K1-E3"
        provider = _read_text(K1_PROVIDER)
        pattern = r"(?m)^\s*[A-Za-z_][\w.]*\s*:[^\n]*(kidpal-dynamic-context\.v1\.txt|kidpal-agent-prompt\.txt)"
        m = re.search(pattern, provider)
        self.assertIsNotNone(
            m,
            f"{tag} RED: provider.config.yaml 无引用动态模板候选的键值行——动态面"
            "声明面缺位（ADR-001 重定义：动态面 = .config.yaml prompt + 动态模板键）。")
        entries = _k1_manifest_entries(self, tag)
        render = _offline_render(self, tag)
        block = render["prompt_block"]
        block_norm = block.replace("\r\n", "\n").strip()
        sources = [
            name for name in K1_DYNAMIC_CANDIDATES
            if (_read_text(DEPLOY_DIR / name).replace("\r\n", "\n").strip()
                and _read_text(DEPLOY_DIR / name).replace("\r\n", "\n").strip()
                in block_norm)]
        self.assertEqual(
            len(sources), 1,
            f"{tag} RED: 动态内容源候选命中 {sources}（须恰一，K1-E2 同判法）。")
        wired = sources[0]
        self.assertIn(
            wired, m.group(0),
            f"{tag} RED: provider 动态模板键引用 {m.group(0).strip()!r} ≠ 渲染产物"
            f"内容源 {wired}——声明面与渲染面分叉（ADR-001 动态面相等破坏）。")
        template_entries = [
            e for e in entries if isinstance(e, dict) and wired in (
                str(e.get("src") or "") + str(e.get("dst") or ""))]
        self.assertTrue(
            template_entries,
            f"{tag} RED: manifest 缺内容源模板 {wired} 的条目——部署单一事实源"
            "未登记（261013 纪律）。")

    def test_k1_e3_offline_render_prompt_is_bounded_dynamic_block(self):
        """coverage: K1-E3 — 离线真跑渲染：.config.yaml prompt: 块 = 有界动态块
        （时间/日期 + 课表摘要锚；零静态纪律；零 v11 静态内容标记）。"""
        tag = "K1-E3"
        render = _offline_render(self, tag)
        self.assertEqual(
            render["rc"], 0,
            f"{tag} RED: 渲染脚本离线跑失败 rc={render['rc']}（渲染面须保持可用）。"
            f"stderr={render['stderr']!r}")
        block = render["prompt_block"]
        self.assertTrue(
            block.strip(),
            f"{tag} RED: 渲染产物无 prompt: 块——引擎动态注入面缺失。")
        self.assertTrue(
            any(w in block for w in ("时间", "日期"))
            and any(w in block for w in ("今天", "星期", "周几", "weekday")),
            f"{tag} RED: 渲染产物 prompt 块缺时间/日期动态锚——现树仍注入 v11 "
            "全量静态语义（承载反转未落）。")
        self.assertTrue(
            any(w in block for w in ("课表摘要", "课表占位", "schedule_summary",
                                     "schedule summary")),
            f"{tag} RED: 渲染产物 prompt 块缺「课表摘要」占位——R8 动态块落位面缺失。")
        for token in ("回合类型", "严禁凭印象", "绝对禁工具", "story_list",
                      "story_text", "最小意图块", "帮我记下来"):
            self.assertNotIn(
                token, block,
                f"{tag} RED: 渲染产物 prompt 块含静态纪律锚「{token}」——静态语义"
                "仍在引擎注入面（缓存边界之下），承载反转未完成。")
        self.assertNotIn(
            "实时事实回合", block,
            f"{tag} RED: 渲染产物 prompt 块含 v11 静态内容锚「实时事实回合」——"
            "v11 全量仍被注入（承载反转未完成）。")

    def test_k1_e3_marker_anchors_match_rendered_product(self):
        """coverage: K1-E3 — .config.yaml marker 锚集同步：撤 v11 专属锚
        「实时事实回合」（负向）+ 每个锚都命中离线渲染产物（锚集 ↔ 渲染产物
        一致性，防「锚改漏/渲染漏改」单向漂移）。"""
        tag = "K1-E3"
        anchors = _k1_config_marker_anchors(self, tag)
        self.assertNotIn(
            "实时事实回合", anchors,
            f"{tag} RED: manifest .config.yaml 锚集仍含 v11 静态内容锚「实时事实"
            "回合」——该锚是 v11 静态内容锚，静态语义移出引擎 prompt 后必失效"
            "（plan.md 可行性判据末条：必须同批撤换为动态块锚）。")
        render = _offline_render(self, tag)
        self.assertEqual(
            render["rc"], 0, f"{tag} RED: 渲染脚本离线跑失败 rc={render['rc']}（前置）。")
        for anchor in anchors:
            self.assertIn(
                anchor, render["rendered"],
                f"{tag} RED: marker 锚 {anchor!r} 未命中离线渲染产物——锚集与渲染"
                "面脱钩（verify-deploy.sh 对真实 live 渲染件必报漂移）。")

    # ------------------------------------------------------------------ #
    # K1-E5 · supersede 留痕存在性
    # ------------------------------------------------------------------ #

    def test_k1_e5_supersede_traces_present(self):
        """coverage: K1-E5 — 既有冻结验收语义换代留痕在场（零静默改写机判面）：
        v7 / 261010 件含 SUPERSEDED-BY 标记；261017 件含 SUPERSEDED-REVIEW 复核
        记录（本批复核其断言零承载拓扑语义、零改写）；m3k3 / m4k3 / perf-k1 三件
        为 planner 2026-10-23 授权扩面 supersede（同纪律：留痕 + 只动承载拓扑
        相关断言）。"""
        tag = "K1-E5"
        expectations = {
            "test_prompt_v7_round_routing.acceptance.py": SUPERSEDE_TAG,
            "test_261010_engine_decision21_overlay_persist.acceptance.py": SUPERSEDE_TAG,
            "test_261017_openclaw_kidpal_skills.acceptance.py": SUPERSEDE_REVIEW_TAG,
            "test_m3k3_voice_schedule_entry.acceptance.py": SUPERSEDE_TAG,
            "test_m4k3_record_session_live.acceptance.py": SUPERSEDE_TAG,
            "test_perf_k1_roundtrip_latency.acceptance.py": SUPERSEDE_TAG,
        }
        for fname, marker in expectations.items():
            text = _read_text(ACCEPTANCE_DIR / fname)
            self.assertIn(
                marker, text,
                f"{tag} RED: {fname} 无 supersede 留痕（{marker!r}）——冻结验收"
                "语义换代必须留痕，零静默改写（K1-E5）。")

    def test_k1_e3_deploy_readme_pointer_section_follows_new_carrier(self):
        """coverage: K1-E3 — server/deploy/README.md prompt 指针节随承载反转换代：
        不再叙述 v11 为「现行/当前被接线」（承载反转后 v11 = 冻结终点版、引擎注入
        = 动态块），指针节指向新承载（AGENTS.md / 动态承载）。

        TODO-BUILDER：本节散文改写归 builder（planner 2026-10-23 裁定扩
        changeManifest 纳入 server/deploy/README.md）；本齿只判「旧口径撤净 +
        新承载指向在场」，不钉改写后具体措辞。"""
        tag = "K1-E3"
        readme = _read_text(DEPLOY_DIR / "README.md")
        self.assertTrue(readme.strip(), f"{tag} RED: server/deploy/README.md 不在场。")
        lines = readme.splitlines()
        start = next((i for i, l in enumerate(lines)
                      if l.startswith("## ") and "system prompt" in l.lower()), None)
        self.assertIsNotNone(
            start,
            f"{tag} RED: README 无 prompt 指针节（标题含「system prompt」的 `## ` 节"
            "缺失）——运维面指针入口被删而非改写。")
        end = next((i for i in range(start + 1, len(lines))
                    if lines[i].startswith("## ")), len(lines))
        section = "\n".join(lines[start:end])
        stale = re.findall(
            r"(?:当前被接线|现状\s*=|当前\s*=)[^\n]{0,40}?openclaw-system-prompt\.v\d+\.md",
            section)
        self.assertEqual(
            stale, [],
            f"{tag} RED: README 指针节仍以「当前被接线/现状 =」口径叙述版本化 prompt "
            f"vN（{stale[:1]!r}）——承载反转后 v11 是冻结终点版、引擎注入 = 动态块，"
            "旧现行接线口径 = 散文漂移（261022-m5-k1 K1-E3）。")
        self.assertTrue(
            "AGENTS.md" in section or "动态" in section,
            f"{tag} RED: README 指针节未指向新承载（AGENTS.md / 动态 context 块）"
            "——承载反转后运维指针仍无新落点（TODO-BUILDER：builder 改写本节）。")

    # ------------------------------------------------------------------ #
    # K1-E6 · 已知窗口（AGENTS.md md5 漂移判红豁免的核验路径）
    # ------------------------------------------------------------------ #

    def test_k1_e6_agents_md_md5_drift_path_known_window(self):
        """coverage: K1-E6 — AGENTS.md md5 核验路径在场且未被弱化：manifest 条目
        verify=md5（test_k1_e3 已钉）+ verify-deploy.sh 保持通用 md5/marker 双路径
        （三时机共用）；负向：脚本无 AGENTS.md 特例跳过（漂移检测不被本包静默
        减免）。已知窗口本身（镜像改后、铺放前 md5 报漂移）按 launch-card 判红
        豁免执行，本齿不判 live md5 绿红。"""
        tag = "K1-E6"
        script = _read_text(K1_VERIFY_SH)
        self.assertTrue(script.strip(), f"{tag} RED: verify-deploy.sh 不在场/为空。")
        self.assertIn(
            "manifest.yaml", script,
            f"{tag} RED: verify-deploy.sh 不再 manifest 驱动——三时机核验单一事实源"
            "被破坏（261013 纪律）。")
        self.assertIn(
            "md5", script,
            f"{tag} RED: verify-deploy.sh 丢失 md5 核验路径——AGENTS.md 每日漂移"
            "核验面缺失（K1-E6 前置）。")
        self.assertTrue(
            "anchors" in script or "marker" in script,
            f"{tag} RED: verify-deploy.sh 丢失 marker 锚核验路径——.config.yaml "
            "渲染件核验面缺失。")
        self.assertNotIn(
            "AGENTS.md", script,
            f"{tag} RED: verify-deploy.sh 出现 AGENTS.md 特例——已知窗口豁免是"
            "loop 判红纪律（launch-card），不是核验脚本对单文件静默减免；脚本"
            "须保持对全部 manifest 条目一视同仁的 md5 核验。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
