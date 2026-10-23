"""Frozen acceptance tests — TECH-261010-engine-decision21-keys-not-persistent
(subsystem: server/deploy; anchor: docs/crq/tech/TECH-261010-engine-decision21-keys-not-persistent.md).

human 直接授权直修（2026-10-10 会话在案；跳过 dev-loop、如实记录）：真机使用反馈
两条 bug（静置 2 分钟告别语复出 / 重启后 WS 推不下通知）取证定根因 = 引擎决策 21 两键
（close_connection_no_voice_time: 86400 + end_prompt.enable: false）从未进入 overlay
渲染链——260914 的 live 修正只落在活配置，任何整体再生（最近一次 1001 overlay 重跑）
即丢失，引擎回落源码默认值（120s 无语音告别语 + 180s 硬断，receiveAudioHandle.py:114/123）
→ 决策 21「设备常驻在线（提醒推送可达）」被击穿。

本件 = 仓内根治齿（对齐 261001-openclaw-model-name-overlay-drift 先例形态）：
钉「决策 21 两键有持久渲染面」，防下次重部署复现。

===========================================================================
齿清单（三条，均机制级文本正钉）：

  D21-A1 渲染面正钉：apply-xiaozhi-overlay.sh 的 .config.yaml heredoc 含两键
        （close_connection_no_voice_time: 86400 与 end_prompt: enable: false）。
        渲染面是 live .config.yaml 的唯一真源——heredoc 缺键 = 再生即丢（本次病根）。
  D21-A2 声明面正钉：provider.config.yaml（渲染源的机器可读声明）含同两键
        （engine_timeout 声明块）。
  D21-A3 声明面↔渲染面一致性：两键的值在声明面与渲染面逐字一致
        （86400 / enable: false）。

缺口（强度申报，逐条）：
  · offline 齿不证引擎 live 真的以 24h 兜底跑——live 行为由部署演练承载
    （主线程 261010 在 ECS 重跑 overlay 渲染 + 引擎日志核验，授权 = 同一 human 指示）。
  · 齿只钉渲染/声明两面文本，不覆盖 .config.yaml 全量再生语义（heredoc 重写全文）——
    与 261001 MNF-A1 行为等价假设同构：heredoc 直写为唯一 live 真源。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_261010_engine_decision21_overlay_persist.acceptance.py

────────────────────────────────────────────────────────────────────────────
261017-prompt-v11-schedule-recall 增批（additive 扩展，tighten-only 窗口——既有
D21-A1/A2/A3 三齿零触碰、既有断言语义不动；TaskPackage =
delivery/taskpackages/261017-prompt-v11-schedule-recall 的 PV-A5）：

PV-A5 冻结范围：overlay 渲染两键 —— apply-xiaozhi-overlay.sh 的 .config.yaml
heredoc 含 `prompt_template: /opt/xiaozhi-esp32-server/data/kidpal-agent-prompt.txt`
与 `system_error_response`（KidPal 口径兜底语，禁「小智」字样）；KidPal 极简模板
源经 overlay 拷入 data/（bind-mount）随之生效；provider.config.yaml 声明面同步
两键（同值）；.config.yaml 的 manifest marker 锚集覆盖两锚。

强度申报（契约条目 → 承载齿 → 性质档）：
  · 渲染面两键在场 / prompt_template 容器路径 → test_pv_a5_overlay_renders_template_and_error_response_keys → 机制级
  · 声明面两键在场                          → test_pv_a5_declaration_carries_two_keys → 机制级
  · 两面值逐字一致                          → test_pv_a5_render_and_declaration_values_agree → 机制级
  · 兜底语禁「小智」                        → test_pv_a5_error_response_forbids_xiaozhi_identity → 机制级
  · 模板经 overlay 拷入 $DATA_DIR           → test_pv_a5_overlay_copies_template_into_data_dir → 行为级（脚本结构解析）
  · .config.yaml marker 锚集覆盖两键        → test_pv_a5_config_marker_anchors_cover_two_keys → 机制级

缺口（逐条）：
  · offline 只证仓内渲染源与声明面；live .config.yaml 的真实再生由 human 部署后
    verify-deploy.sh 三时机核验承载（.config.yaml 是生成件，改错引擎起不来——
    TaskPackage notes 地雷③）。
  · 「模板经 bind-mount 生效」以「overlay 把模板拷入 $DATA_DIR」的脚本行为为代理
    （行为等价假设：data/ == 容器内 /opt/xiaozhi-esp32-server/data，与
    prompt_template 容器路径同源）。
  · 兜底语真实播报不含「小智」归 PV-A8 真机轮（human 人为触发一次引擎出错兜底）。

RED 基线预期（冻结时点树，两键未落）：PV-A5 六齿全红（渲染/声明面均缺键 =
真实缺失行为）；D21-A1/A2/A3 三齿本批零触碰、继续绿。
"""

from __future__ import annotations

import re
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY = SERVER / "deploy"

PROVIDER_DECL_YAML = DEPLOY / "provider.config.yaml"
OVERLAY_SH = DEPLOY / "apply-xiaozhi-overlay.sh"

# 决策 21 两键（arch §6.4）：值 = 决策原文，不得「顺手调优」。
D21_NO_VOICE_KEY = "close_connection_no_voice_time: 86400"
D21_END_PROMPT_RENDER = "end_prompt:"
D21_END_PROMPT_ENABLE = "  enable: false"


class D21OverlayPersistAcceptance(unittest.TestCase):

    maxDiff = None

    # ------------------------------------------------------------------ #
    # D21-A1 · 渲染面正钉（heredoc 是 live .config.yaml 唯一真源）
    # ------------------------------------------------------------------ #

    def test_d21_a1_overlay_renders_decision21_keys(self):
        """coverage: D21-A1 — apply-xiaozhi-overlay.sh 渲染面含决策 21 两键。"""
        self.assertTrue(
            OVERLAY_SH.is_file(),
            f"D21-A1 RED: 渲染面不在场 —— 要求 {OVERLAY_SH}。",
        )
        src = OVERLAY_SH.read_text(encoding="utf-8", errors="replace")
        self.assertIn(
            D21_NO_VOICE_KEY, src,
            "D21-A1 RED: 渲染面缺 close_connection_no_voice_time: 86400 —— heredoc 不写此键，"
            "下次 overlay 再生会把引擎冲回源码默认 120s 无语音断连（决策 21 复现病根）。",
        )
        self.assertIn(
            D21_END_PROMPT_ENABLE, src,
            "D21-A1 RED: 渲染面缺 end_prompt.enable: false —— 缺键即引擎默认告别语复活"
            "（receiveAudioHandle.py:123 enable 缺省 True）。",
        )

    # ------------------------------------------------------------------ #
    # D21-A2 · 声明面正钉（provider.config.yaml 机器可读声明）
    # ------------------------------------------------------------------ #

    def test_d21_a2_declaration_carries_decision21_keys(self):
        """coverage: D21-A2 — provider.config.yaml 声明面含决策 21 两键（engine_timeout 块）。"""
        self.assertTrue(
            PROVIDER_DECL_YAML.is_file(),
            f"D21-A2 RED: 声明面不在场 —— 要求 {PROVIDER_DECL_YAML}。",
        )
        src = PROVIDER_DECL_YAML.read_text(encoding="utf-8", errors="replace")
        self.assertIn(
            "close_connection_no_voice_time: 86400", src,
            "D21-A2 RED: 声明面缺 close_connection_no_voice_time: 86400 —— 渲染源声明不完整。",
        )
        self.assertIn(
            "enable: false", src,
            "D21-A2 RED: 声明面缺 end_prompt enable: false —— 渲染源声明不完整。",
        )

    # ------------------------------------------------------------------ #
    # D21-A3 · 声明面 ↔ 渲染面一致性（两键值逐字同）
    # ------------------------------------------------------------------ #

    def test_d21_a3_declaration_render_values_agree(self):
        """coverage: D21-A3 — 两键值在声明面与渲染面逐字一致（防半改）。"""
        decl = PROVIDER_DECL_YAML.read_text(encoding="utf-8", errors="replace")
        rend = OVERLAY_SH.read_text(encoding="utf-8", errors="replace")
        for needle in ("close_connection_no_voice_time: 86400", "enable: false"):
            in_decl = needle in decl
            in_rend = needle in rend
            self.assertEqual(
                in_decl, in_rend,
                f"D21-A3 RED: 键 {needle!r} 在声明面/渲染面仅单侧在场（decl={in_decl}, "
                f"render={in_rend}）——半改状态，两面须同步落。",
            )


# =========================================================================== #
# 261017-prompt-v11-schedule-recall 增批（additive —— D21-A1/A2/A3 三齿零触碰）
#
# PV-A5（TaskPackage acceptance[5]）：overlay 渲染两键 ——
#   apply-xiaozhi-overlay.sh 的 .config.yaml heredoc 含
#     `prompt_template: /opt/xiaozhi-esp32-server/data/kidpal-agent-prompt.txt`
#     与 `system_error_response`（KidPal 口径兜底语，禁「小智」字样）；
#   KidPal 极简模板源经 overlay 拷入 data/（bind-mount）随之生效；
#   provider.config.yaml 声明面同步两键（同值）；
#   .config.yaml 的 manifest marker 锚集覆盖 prompt_template / system_error_response。
#
# 强度申报：机制级（渲染面 heredoc 文本 + 声明面文本 + 两面值逐字一致 + 铺放行
# 结构解析）。缺口（逐条）：offline 只证仓内渲染源与声明面；live .config.yaml 的
# 真实再生由 human 部署后 verify-deploy.sh 三时机核验承载（.config.yaml 是生成件，
# 改错引擎起不来——TaskPackage notes 地雷③）。「模板经 bind-mount 生效」以「overlay
# 把模板拷入 $DATA_DIR」的脚本行为为代理（行为等价假设：data/ == 容器内
# /opt/xiaozhi-esp32-server/data，与 prompt_template 容器路径同源，260913 部署面实测）。
#
# 设计态红（冻结时点树）：两键在两面均缺 = 红；兜底语含「小智」= 红；marker 锚集
# 未覆盖两键 = 红。D21-A1/A2/A3 三齿本批零触碰。
# =========================================================================== #

PV11_TEMPLATE_NAME = "kidpal-agent-prompt.txt"
PV11_TEMPLATE_CONTAINER_PATH = "/opt/xiaozhi-esp32-server/data/kidpal-agent-prompt.txt"
PV11_PROMPT_TEMPLATE_KEY = "prompt_template"
PV11_ERROR_RESPONSE_KEY = "system_error_response"
MANIFEST_YAML = DEPLOY / "manifest.yaml"


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _overlay_heredoc(src: str) -> str:
    """渲染面 .config.yaml heredoc 正文提取（内容驱动：`cat > … <<EOF` → 首个 EOF 行）。"""
    lines = src.splitlines()
    start = None
    for i, line in enumerate(lines):
        if re.match(r"^\s*cat\s*>\s*\S*OVERLAY\S*\s*<<", line):
            start = i + 1
            break
    if start is None:
        return ""
    out = []
    for line in lines[start:]:
        if line.strip() == "EOF":
            break
        out.append(line)
    return "\n".join(out)


def _yaml_scalar(text: str, key: str):
    """取 `key: value` 标量值（去首尾引号）；缺键返回 None。"""
    m = re.search(rf"^\s*{re.escape(key)}\s*:\s*(.+?)\s*$", text, re.MULTILINE)
    if not m:
        return None
    val = m.group(1).strip()
    if len(val) >= 2 and val[0] == val[-1] and val[0] in "\"'":
        val = val[1:-1]
    return val


def _expanded_copy_lines(src: str, needle: str, target_dir_token: str) -> list[str]:
    """找「把 needle 拷入 target_dir_token 目录」的 cp/install 行（一层变量展开）。

    形制容忍两种写法：字面路径同行 cp，或 PROMPT_TEMPLATE_SRC/DST 变量分派后
    `cp "$SRC" "$DST"`（变量值先展开再判）。"""
    assigns = {}
    for line in src.splitlines():
        m = re.match(r"\s*([A-Za-z_][A-Za-z0-9_]*)=(.*)$", line)
        if m:
            assigns[m.group(1)] = m.group(2)
    hits = []
    for line in src.splitlines():
        if not re.search(r"\b(cp|install)\b", line):
            continue
        expanded = line
        for var in assigns:
            expanded = expanded.replace("${" + var + "}", assigns[var])
            expanded = expanded.replace("$" + var, assigns[var])
        if needle in expanded and target_dir_token in expanded:
            hits.append(line.strip())
    return hits


def _config_yaml_marker_anchors(testcase) -> list[str]:
    """manifest.yaml 中 .config.yaml（verify=marker）条目的 anchors 并集。"""
    try:
        import yaml  # noqa: PLC0415  (lazy: 不让可选依赖连坐本文件既有三齿)
    except ImportError as e:  # pragma: no cover - 环境前提缺失
        testcase.fail(
            f"PV-A5 RED(infra): PyYAML 不在位——manifest marker 锚集解析前置缺失"
            f"（verify-deploy.sh 同一依赖）: {e!r}")
    if not MANIFEST_YAML.is_file():
        testcase.fail(f"PV-A5 RED: {MANIFEST_YAML} 不在场。")
    try:
        doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
    except Exception as e:  # noqa: BLE001 — 解析错误是 RED，不是 crash
        testcase.fail(f"PV-A5 RED: manifest.yaml 解析失败: {e!r}")
    anchors: list[str] = []
    for entry in (doc or {}).get("entries", []) or []:
        if not isinstance(entry, dict):
            continue
        if str(entry.get("dst") or "").endswith(".config.yaml") \
                and entry.get("verify") == "marker":
            anchors.extend([str(a) for a in (entry.get("anchors") or [])])
    testcase.assertTrue(
        anchors, "PV-A5 RED: manifest 无 .config.yaml marker 条目/锚集——渲染件无核验面。")
    return anchors


class KidpalTemplateTwoKeyAcceptance(unittest.TestCase):
    """261017-prompt-v11-schedule-recall PV-A5（offline，静态锚齿，additive 扩展）。

    承载：KidPal 极简模板两键（prompt_template / system_error_response）在渲染面
    heredoc 与声明面 provider.config.yaml 同值在场，兜底语禁「小智」。"""

    maxDiff = None

    @classmethod
    def setUpClass(cls):
        cls.overlay_src = _read_text(OVERLAY_SH)
        cls.heredoc = _overlay_heredoc(cls.overlay_src)
        cls.decl_src = _read_text(PROVIDER_DECL_YAML)

    def test_pv_a5_overlay_renders_template_and_error_response_keys(self):
        """coverage: PV-A5 — 渲染面 heredoc 含 prompt_template（容器路径指向引擎
        模板候选之一）与 system_error_response 两键。

        SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E2：prompt_template
        容器路径判法从「恒 = .../kidpal-agent-prompt.txt」换为「两个模板候选
        （kidpal-agent-prompt.txt / kidpal-dynamic-context.v1.txt）之一」（二选一
        定形：新件取代后 prompt_template 可改指动态模板件；模板零指向 = 引擎回落
        镜像内置通用小智模板，仍红）。system_error_response 断言零变化。"""
        self.assertTrue(
            OVERLAY_SH.is_file(),
            f"PV-A5 RED: 渲染面不在场 —— 要求 {OVERLAY_SH}。")
        self.assertTrue(
            self.heredoc,
            "PV-A5 RED: apply-xiaozhi-overlay.sh 的 .config.yaml heredoc 解析失败"
            "（`cat > … <<EOF` → `EOF` 结构被破坏）—— 渲染面无从核验。")
        prompt_val = _yaml_scalar(self.heredoc, PV11_PROMPT_TEMPLATE_KEY)
        self.assertIsNotNone(
            prompt_val,
            "PV-A5 RED: 渲染面 heredoc 缺 prompt_template 键 —— 引擎仍读镜像内置"
            "通用小智模板（TECH-261017 第二根因），模板承载不生效。")
        prompt_base = str(prompt_val).rsplit("/", 1)[-1]
        self.assertIn(
            prompt_base, (PV11_TEMPLATE_NAME, "kidpal-dynamic-context.v1.txt"),
            f"PV-A5 RED(superseded): prompt_template = {prompt_val!r}，容器路径"
            f"basename {prompt_base!r} 不属引擎模板候选集 "
            f"({PV11_TEMPLATE_NAME} / kidpal-dynamic-context.v1.txt) —— 261022-m5-k1 "
            "K1-E2 二选一定形未落或指错文件。")
        error_val = _yaml_scalar(self.heredoc, PV11_ERROR_RESPONSE_KEY)
        self.assertIsNotNone(
            error_val,
            "PV-A5 RED: 渲染面 heredoc 缺 system_error_response 键 —— 引擎出错兜底"
            "语仍是「主人，小智现在有点忙」（TECH-261017 附带项②）。")
        self.assertTrue(
            str(error_val).strip(),
            "PV-A5 RED: system_error_response 值为空 —— 兜底语缺位。")

    def test_pv_a5_declaration_carries_two_keys(self):
        """coverage: PV-A5 — 声明面 provider.config.yaml 同步两键（渲染源的机器
        可读声明）。"""
        self.assertTrue(
            PROVIDER_DECL_YAML.is_file(),
            f"PV-A5 RED: 声明面不在场 —— 要求 {PROVIDER_DECL_YAML}。")
        prompt_val = _yaml_scalar(self.decl_src, PV11_PROMPT_TEMPLATE_KEY)
        self.assertIsNotNone(
            prompt_val,
            "PV-A5 RED: 声明面缺 prompt_template —— 渲染源声明不完整"
            "（单侧在场 = 下次 overlay 再生即丢，D21 病史同型）。")
        error_val = _yaml_scalar(self.decl_src, PV11_ERROR_RESPONSE_KEY)
        self.assertIsNotNone(
            error_val,
            "PV-A5 RED: 声明面缺 system_error_response —— 渲染源声明不完整。")

    def test_pv_a5_render_and_declaration_values_agree(self):
        """coverage: PV-A5 — 两键值在声明面与渲染面逐字一致（防半改）。"""
        for key in (PV11_PROMPT_TEMPLATE_KEY, PV11_ERROR_RESPONSE_KEY):
            in_render = _yaml_scalar(self.heredoc, key)
            in_decl = _yaml_scalar(self.decl_src, key)
            self.assertIsNotNone(
                in_render, f"PV-A5 RED: 渲染面缺 {key}（前置）。")
            self.assertIsNotNone(
                in_decl, f"PV-A5 RED: 声明面缺 {key}（前置）。")
            self.assertEqual(
                in_decl, in_render,
                f"PV-A5 RED: 键 {key} 在声明面/渲染面值不一致（decl={in_decl!r}, "
                f"render={in_render!r}）——半改状态，两面须同步落。")

    def test_pv_a5_error_response_forbids_xiaozhi_identity(self):
        """coverage: PV-A5 — 兜底语 KidPal 口径：两面均不含「小智」字样。"""
        for face, text in (("渲染面 heredoc", self.heredoc),
                           ("声明面 provider.config.yaml", self.decl_src)):
            val = _yaml_scalar(text, PV11_ERROR_RESPONSE_KEY)
            self.assertIsNotNone(
                val, f"PV-A5 RED({face}): 缺 {PV11_ERROR_RESPONSE_KEY}（前置）。")
            self.assertNotIn(
                "小智", str(val),
                f"PV-A5 RED({face}): 兜底语含「小智」—— 播报仍自称小智"
                "（TECH-261017 附带项②/去小智残留）。")

    def test_pv_a5_overlay_copies_template_into_data_dir(self):
        """coverage: PV-A5 — 模板源经 overlay 拷入 data/（bind-mount）随之生效。

        SUPERSEDED-BY 261022-m5-k1-prompt-carrier-migration K1-E2：拷贝对象判法
        从「kidpal-agent-prompt.txt 单件」换为「两个模板候选至少一者被拷入
        $DATA_DIR」（二选一定形；零候选拷入 = prompt_template 指向空气，仍红）。"""
        self.assertTrue(self.overlay_src.strip(), f"PV-A5 RED: {OVERLAY_SH} 不在场。")
        hits = _expanded_copy_lines(
            self.overlay_src, PV11_TEMPLATE_NAME, "DATA_DIR")
        hits_dynamic = _expanded_copy_lines(
            self.overlay_src, "kidpal-dynamic-context.v1.txt", "DATA_DIR")
        self.assertTrue(
            hits or hits_dynamic,
            "PV-A5 RED(superseded): overlay 无把引擎模板候选（kidpal-agent-prompt."
            "txt / kidpal-dynamic-context.v1.txt）拷入 $DATA_DIR 的 cp/install 行 —— "
            "模板源不上 ECS data/，prompt_template 指向空气（孤儿声明；"
            "261022-m5-k1 K1-E2 二选一定形）。")

    def test_pv_a5_config_marker_anchors_cover_two_keys(self):
        """coverage: PV-A5 — manifest .config.yaml marker 锚集覆盖 prompt_template /
        system_error_response 两锚（渲染件核验面同步）。"""
        anchors = _config_yaml_marker_anchors(self)
        joined = "\n".join(anchors)
        for key in (PV11_PROMPT_TEMPLATE_KEY, PV11_ERROR_RESPONSE_KEY):
            self.assertIn(
                key, joined,
                f"PV-A5 RED: manifest .config.yaml marker 锚集缺 {key}（现锚集 "
                f"{anchors}）—— 渲染件漂移时 verify-deploy.sh 无该键核验锚"
                "（D21-A1 同型病史：渲染产物无对账面）。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)