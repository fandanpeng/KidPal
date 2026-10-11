"""260925-prompt-v7-round-routing — v7 回合路由制 prompt 齿（offline）。

冻结范围（human 260925 拍板批）：
  ① v7 落盘锚齿 + 「制品本体无版本沿革前言」结构齿（首行即 `## 角色`）；
  ② 回合类型路由表锚（孩子对话 / 查字 / 课表问答 / 提醒 cron 四行 + 兜底句 + 只读边界 +
     课表反编造覆盖句——2026-09-25 live 编造事故的生成端根治锚）；
  ③ 禁产 story JSON 锚（2026-09-24 契约门 Q7 对齐：relay 机械拒绝引擎侧
     story_list/story_text，prompt 不得再教模型产这两块）；
  ④ 课表段 m3k3 同组语义锚（问答框 / 不切页 / 备注读回 / 意图薄载荷 / 网格不由 LLM 生成）；
  ⑤ v6 历史版本 sha256 零回改齿（E2 铁律同形态；v1..v5 由既有各包齿钉）。

强度申报：
  1) 内容驱动选节（并集窗口，不钉标题措辞；措辞换写不误红、语义被删才红——m3k3 ② 同族）；
  2) 本齿**不**断言「当前接线 = v7」版本字面（ADR-001 / ARQ-260908 纪律：三面接线一致性由
     test_m3k3_voice_schedule_entry.acceptance.py::test_a1_prompt_wired_three_faces_version_agnostic
     与 test_perf_k1_roundtrip_latency.acceptance.py 的版本无关形态承载，bump 不假红）；
  3) v7 本体零回改 = 后继 bump 时新增 `.vN` sha256 齿（同本文件 ⑤ 形态），不改本齿。

────────────────────────────────────────────────────────────────────────────
261010-prompt-v9-schedule-anti-narration 增批（additive 补齿，tighten-only 窗口——
既有齿零触碰、既有断言语义不动；TaskPackage = delivery/taskpackages/
261010-prompt-v9-schedule-anti-narration，源头 = docs/crq/tech/TECH-261010-schedule-
round-narration-leak）：

冻结范围（PN-A1..A5）：
  ⑥ v9 落盘非空 + 首行 `## 角色` 无前言结构齿（V7-A1 同形态，PN-A1）；
  ⑦ 课表问答回合行对称反叙述纪律锚（PN-A2）：v7 修对话行时漏了课表行的对称条款
     （TECH-261010：真机 TTS 把「这是课表问答回合」「需要先读 kidpal/schedule.md」等
     模型内部推理当语音播出）——内容驱动选节 + 并集窗口，不钉具体措辞：
     「先读 schedule.md」既有锚保持 + 反叙述纪律入行；
  ⑧ v8 历史版本 sha256 零回改齿（PN-A3，V7 ⑤ 形态复刻）；
  ⑨ 三面接线一致 + 接线版承载 ⑦ 锚（PN-A4）：版本无关形态（ARQ-260908 / 头注②
     同纪律，接线类断言零版本字面）——「接线 = v9」的语义由组合承载：三面解析一致
     （渲染面 == 声明面 == 载体面，must-stay-green 半）+ 接线版纪律锚在场（红→绿半，
     现树接线 v8 无该锚 = 红）；未来 bump 若丢纪律同样红（防丢内容）；
  ⑩ PN-A5 = live 观测项（非自动断言，非确定性诚实申报，见
     TestPromptV9ScheduleAntiNarration 类 docstring）。

强度申报（acceptance-evidence「契约条目 ↔ 齿 的强度申报」体例）：
  - PN-A1 → 落盘/措辞齿 → 源锚代理（v9 的产出物就是措辞本身，故「源锚」即工件面；
    「prompt 写明 ≠ LLM 遵循」行为等价假设显式化——LLM 实际遵循度 offline 不可机判，
    归 PN-A5 真机轮目击）；
  - PN-A2 并集口径（不钉具体措辞的实现方式）：泄漏类别词 6 选 ≥2（回合判定/文件路径/
    流程步骤/内部动作/内部推理/内部独白）+ 语音半同义组（语音/播报/念出/念给/TTS）
    + 禁止半同义组（不得/禁止/严禁/违规/不许/不进入）——措辞换写不误红、纪律语义被删
    才红；行级定位按回合类型名「课表问答回合」取行（内容驱动、不按行号），**不取
    并集全文窗口**——对话行既有「内部动作/违规」词若漏进窗口会造成假绿（TECH-261010
    根因正是对话行已有该词）；
  - PN-A3 → 机制级（raw bytes sha256；v8 为 LF 落盘，raw == 换行归一双口径同值）；
  - PN-A4 → 机制级（三面结构化指向解析 + 一致性 + 接线版语义锚在场）；三面解析器为
    m3k3 冻结件 _wired_prompt_faces 同口径自含副本（跨冻结件 import 会把两份冻结件
    焊死，故复制不引用）；另附 v9 世系最小锚抽检（严禁凭印象 / story_list / 问答框）
    承载「v9 = v8 全量 + 课表行扩行」的继承面（plan.md 修复方案），防 bump 丢段；
  - 契约零涉及：契约 §1.6 system prompt 摘要不承载语音反叙述纪律（PN-E2 零改预期），
    本批齿全部为 deploy 制品面锚，无 DB/事件断言（TaskPackage notes 地雷前置申报在案）。

RED 基线预期（冻结时点树，v9 未建、接线未改）：PN-A1 两齿 + PN-A2 + PN-A4 = 设计态红
（v9 文件缺失 / 现接线 v8 无课表行纪律锚 = 真实缺失行为）；PN-A3 = must-stay-green
（v8 未被回改即绿，回改才红——负锚形态同 V7 ⑤）；既有齿零触碰、既有绿不降。
"""

from __future__ import annotations

import hashlib
import re
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DEPLOY_DIR = REPO_ROOT / "server" / "deploy"

V7_NAME = "openclaw-system-prompt.v7.md"
V6_NAME = "openclaw-system-prompt.v6.md"
V8_NAME = "openclaw-system-prompt.v8.md"
V9_NAME = "openclaw-system-prompt.v9.md"

# 冻结时点 v6 内容 hash（raw bytes 口径，v6 为 LF 落盘；v7 bump 与一切后续包不得改 v6 本体）。
# 计算式 = sha256(read_bytes())。
V6_SHA256 = "bf5c6b4c11fd155241a0c5e51adbe2cbcce82768426e12e1a135dfa573f6197b"

# 冻结时点 v8 内容 hash（raw bytes 口径；v8 为 LF 落盘，raw == 换行归一双口径同值，
# 2026-10-11 PN 批冻结实测）。v9 bump 与一切后续包不得改 v8 本体（V7 ⑤ 形态复刻）。
# 计算式 = sha256(read_bytes())。
V8_SHA256 = "91f4a0032a74d5a317f3a7a90c057e767feb6fed2b30c876068048db57edd6f4"


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _sha256_raw(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _section_window(body: str, title_keywords: tuple[str, ...],
                    extra_line_keywords: tuple[str, ...] = ()) -> str:
    """内容驱动选节：标题含任一关键词的 `## ` 节全文 + 其它节内含 extra 行的并集。"""
    lines = body.splitlines()
    heads = [i for i, l in enumerate(lines) if l.startswith("## ")]
    out: list[str] = []
    for k, i in enumerate(heads):
        j = heads[k + 1] if k + 1 < len(heads) else len(lines)
        if any(w in lines[i] for w in title_keywords):
            out.extend(lines[i:j])
        else:
            out.extend(l for l in lines[i:j]
                       if any(w in l for w in extra_line_keywords))
    if not heads:  # 无节结构（异常文件）——退化为全文
        out = lines
    return "\n".join(out)


# --------------------------------------------------------------------------- #
# PN 批辅助（261010-prompt-v9-schedule-anti-narration，additive）
# --------------------------------------------------------------------------- #

# 反叙述纪律泄漏类别词（TECH-261010 泄漏面：回合判定 / 文件路径 / 流程步骤 / 内部动作；
# 内部推理 / 内部独白为 TECH 标题与泄漏通道同义措辞——并集容换写）。
_ANTI_NARRATION_CATEGORIES = (
    "回合判定", "文件路径", "流程步骤", "内部动作", "内部推理", "内部独白")
# 语音半同义组（纪律钉在「不得进入语音部分」）。
_VOICE_HALF_ANCHORS = ("语音", "播报", "念出", "念给", "TTS")
# 禁止语义同义组。
_PROHIBITION_ANCHORS = ("不得", "禁止", "严禁", "违规", "不许", "不进入")

_PROMPT_FILE_RE = re.compile(r"openclaw-system-prompt\.v\d+\.md")


def _schedule_qa_rows(body: str) -> list[str]:
    """课表问答回合行提取（内容驱动：按回合类型名定位表行，不按行号）。

    只取含「课表问答回合」的行、**不取并集全文窗口**——对话行既有「内部动作/违规」
    词若漏进窗口，PN-A2 会在 v8/v9 未补课表行纪律时就假绿（TECH-261010 根因即
    对话行已有反叙述纪律、课表行没有）。"""
    window = _section_window(body, ("回合类型",), ("回合", "课表", "schedule"))
    return [l for l in window.splitlines() if "课表问答回合" in l]


def _assert_schedule_row_anti_narration(testcase, body: str, label: str):
    """PN-A2 锚组（内容驱动选节、并集窗口，不钉具体措辞）：
    ① 既有锚保持——课表问答回合行仍钉「先读 schedule.md 再作答」；
    ② 对称反叙述纪律入行——泄漏类别词 ≥2/6 + 语音半 + 禁止半（v7 对话行「任何
    『先查一下 / 先确认一下』的内部动作都是违规」句式的课表行对称扩行）。"""
    rows = _schedule_qa_rows(body)
    testcase.assertTrue(
        rows,
        f"PN-A2 RED({label}): 回合路由面无「课表问答回合」行——回合类型路由表"
        "结构被破坏（V7-A1 既有锚面回退）。")
    row = "\n".join(rows)
    # ① 既有锚保持（bump 不得丢课表行既有纪律）。
    testcase.assertIn(
        "先读", row,
        f"PN-A2 RED({label}): 课表问答回合行无「先读」锚——先读真课表再作答的"
        "既有纪律被删（工具/文件动作照做半缺失）。")
    testcase.assertIn(
        "schedule.md", row,
        f"PN-A2 RED({label}): 课表问答回合行无 schedule.md 路径锚——真实课表来源"
        "声明缺失（反编造链路的读文件半）。")
    # ② 对称反叙述纪律入行（并集口径：换写不误红、纪律语义被删才红）。
    hits = [w for w in _ANTI_NARRATION_CATEGORIES if w in row]
    testcase.assertGreaterEqual(
        len(hits), 2,
        f"PN-A2 RED({label}): 课表问答回合行缺对称反叙述纪律——泄漏类别词仅命中 "
        f"{hits}（须 ≥2/6：回合判定/文件路径/流程步骤/内部动作/内部推理/内部独白）。"
        "v7 修对话行时漏了课表行的对称条款（TECH-261010），此处防回潮。")
    testcase.assertTrue(
        any(w in row for w in _VOICE_HALF_ANCHORS),
        f"PN-A2 RED({label}): 课表问答回合行反叙述纪律无语音半锚（语音/播报/念出/"
        "念给/TTS 任一）——纪律未钉到「不得进入语音部分」。")
    testcase.assertTrue(
        any(w in row for w in _PROHIBITION_ANCHORS),
        f"PN-A2 RED({label}): 课表问答回合行反叙述纪律无禁止语义锚（不得/禁止/"
        "严禁/违规/不许/不进入任一）。")


def _wired_prompt_faces_pn_guard():
    """三面解析当前被接线的版本化 prompt 文件名（版本无关，ARQ-260908 纪律）。

    m3k3 冻结件 _wired_prompt_faces 同口径自含副本——跨冻结件 import 会把两份冻结件
    焊死，故复制不引用（解析正则逐字对齐 m3k3 侧，防两套口径漂移）。
    返回 (rendered, declared, carried)。"""
    render_src = _read_text(DEPLOY_DIR / "apply-xiaozhi-overlay.sh")
    rendered = re.findall(
        r"PROMPT_(?:SRC|DST)\s*=.*?(openclaw-system-prompt\.v\d+\.md)", render_src)
    decl_src = _read_text(DEPLOY_DIR / "provider.config.yaml")
    declared = re.findall(
        r"(?:artifact|source|deploy_target)\s*:\s*\"[^\"]*"
        r"(openclaw-system-prompt\.v\d+\.md)", decl_src)
    carried = []
    carrier = _read_text(DEPLOY_DIR / "OPENCLAW_SYSTEM_PROMPT_CARRIER")
    for line in carrier.splitlines():
        if "artifact（生成唯一来源）" in line or (
                line.strip().startswith("- 渲染面") and "openclaw-system-prompt" in line):
            m = _PROMPT_FILE_RE.search(line)
            if m:
                carried.append(m.group(0))
    return rendered, declared, carried


class TestPromptV7RoundRouting(unittest.TestCase):
    """260925-prompt-v7-round-routing（offline，静态锚齿）。"""

    @classmethod
    def setUpClass(cls):
        cls.v7_path = DEPLOY_DIR / V7_NAME
        cls.v7 = _read_text(cls.v7_path)
        cls.v6_path = DEPLOY_DIR / V6_NAME

    # -- ① 落盘 + 无前言结构齿 ------------------------------------------------

    def test_a1_v7_prompt_file_lands(self):
        """coverage: V7-A1 — v7 落盘非空（版本化演进，历史版本共存可 diff）。"""
        self.assertTrue(
            self.v7_path.is_file(),
            f"V7-A1 RED: {V7_NAME} 未落盘——制品缺失即接线悬空（E4 族）。",
        )
        self.assertGreater(
            len(self.v7.strip()), 0,
            "V7-A1 RED: v7 文件为空。",
        )

    def test_a1_v7_no_version_preamble(self):
        """coverage: V7-A1 — 制品本体无版本沿革前言：首行即 `## ` 节标题。

        v6 及以前的前言（20.8% / 对模型零操作语义 + 免检区行为语义载体）随
        260925-prompt-v7-round-routing 外置 OPENCLAW_SYSTEM_PROMPT_CARRIER；本齿防回潮。"""
        first = self.v7.splitlines()[0].strip() if self.v7.splitlines() else ""
        self.assertEqual(
            first, "## 角色",
            "V7-A1 RED: v7 首行不是 `## 角色`——版本沿革前言回潮或角色节被移位"
            "（沿革只允许住 OPENCLAW_SYSTEM_PROMPT_CARRIER）。",
        )
        self.assertFalse(
            self.v7.startswith("#") and not self.v7.startswith("## "),
            "V7-A1 RED: v7 顶层出现 H1 前言标题（前言回潮）。",
        )

    # -- ② 回合路由表锚 --------------------------------------------------------

    def test_a1_v7_round_routing_anchors(self):
        """coverage: V7-A1 — 回合类型路由表：三类回合行 + 查字绝对禁工具 + 兜底句 +
        只读边界 + 课表反编造覆盖句。P0-1 根治语义必须整体在场。"""
        window = _section_window(self.v7, ("回合类型",), ("回合", "课表", "schedule"))

        for kw, why in (
            ("孩子对话回合", "对话回合（一回合作答禁工具）行缺失"),
            ("查字回合", "查字回合（绝对禁工具）行缺失——human 260925 复审补行要求"),
            ("课表问答回合", "课表问答回合（先读真课表）行缺失"),
            ("提醒 cron 回合", "提醒 cron 回合（语义 JSON）行缺失"),
            ("判不清", "兜底句（判不清按对话回合处理）缺失"),
            ("严禁凭印象", "课表反编造覆盖句缺失——9/25 live 编造事故的生成端封堵锚"),
            ("只读", "kidpal/、stories/ 只读边界声明缺失（QQ 录入归家庭侧 agent 的拓扑声明）"),
            ("kidpal-schedule-answer", "课表问答回合未指 skill 流程（prompt↔skill 许可结构）"),
        ):
            self.assertIn(kw, window, f"V7-A1 RED: 回合路由面无「{kw}」——{why}。")
        # 一回合作答禁令强度（对百科回合的延迟保护，NFR-PERF-1）。
        self.assertTrue(
            any(w in window for w in ("不调用任何工具", "不读取任何文件")),
            "V7-A1 RED: 对话回合「一回合作答」禁令强度被稀释——禁工具/禁读文件表述缺失。",
        )

    # -- ③ 禁产 story JSON 锚 --------------------------------------------------

    def test_a1_v7_story_json_ban_anchor(self):
        """coverage: V7-A1 — 星星记录本禁产 story_list/story_text display 块
        （2026-09-24 契约门 Q7：relay 机械拒绝引擎侧来源）。"""
        window = _section_window(self.v7, ("故事",), ("story",))
        self.assertIn("story_list", window, "V7-A1 RED: story 面无 story_list 措辞。")
        self.assertIn("story_text", window, "V7-A1 RED: story 面无 story_text 措辞。")
        self.assertTrue(
            any(w in window for w in ("严禁生成", "不要生成", "禁生成", "不生成", "你不生成")),
            "V7-A1 RED: story 面无「禁产 story JSON」禁令——模型被教产出 relay 机械拒绝的块。",
        )
        self.assertTrue(
            any(w in window for w in ("文字答复", "只用文字", "text view")),
            "V7-A1 RED: story 面无「只用文字答复」正路口径。",
        )

    # -- ④ 课表段 m3k3 同组语义锚 ----------------------------------------------

    def test_a1_v7_schedule_anchors(self):
        """coverage: V7-A1 — 课表段语义锚（m3k3 锚组并集：问答框 / 不切页 / 备注读回 /
        意图薄载荷 / 网格不由 LLM 生成）。课表段跨批不得丢语义。"""
        window = _section_window(self.v7, ("课表",), ("课表", "schedule"))
        self.assertIn("问答框", window, "V7-A1 RED: 课表段无「问答框」锚。")
        self.assertTrue(
            any(w in window for w in ("不切", "不滚动", "不定位")),
            "V7-A1 RED: 课表段无「不切页/不定位」锚。")
        self.assertIn("备注", window, "V7-A1 RED: 课表段无「备注」锚。")
        self.assertTrue(
            any(w in window for w in ("答复", "读回", "念出")),
            "V7-A1 RED: 课表段无「答复/读回」锚。")
        self.assertTrue(
            any(w in window for w in ("帮我显示课表", "打开课表")),
            "V7-A1 RED: 课表段无显示意图锚。")
        self.assertTrue(
            "薄载荷" in window or "{}" in window,
            "V7-A1 RED: 课表段无薄载荷 data:{} 锚。")
        self.assertTrue(
            any(w in window for w in ("不生成", "不由 LLM", "不产", "机械计算", "front", "网关")),
            "V7-A1 RED: 课表段无「网格数据不由 LLM 生成」锚。")

    # -- ⑤ v6 历史版本零回改 ----------------------------------------------------

    def test_a1_v6_prompt_file_immutable(self):
        """coverage: V7-A1 — v6 本体零回改（E2 铁律同形态；v1..v5 由既有各包齿钉）。
        v6 冻结时点 sha256 = bf5c6b4c…（260925-prompt-v7-round-routing 批冻结）。"""
        self.assertTrue(
            self.v6_path.is_file(),
            "V7-A1 RED: v6 文件被删除——历史版本共存可 diff 铁律被破坏。",
        )
        self.assertEqual(
            _sha256_raw(self.v6_path), V6_SHA256,
            "V7-A1 RED: v6 本体被回改——历史版本一律不改（版本化落盘惯例）。",
        )


class TestPromptV9ScheduleAntiNarration(unittest.TestCase):
    """261010-prompt-v9-schedule-anti-narration（offline，静态锚齿，additive 补齿）。

    PN-A5（live 观测项——**非自动断言**，非确定性诚实申报，本类不落测试）：
      ECS 部署 v9 + overlay 重跑后，真机问「明天有什么课」——TTS 不得播出回合判定 /
      文件路径 / 流程步骤类前导叙述（TECH-261010 泄漏通道 = 工具调用轮次间前导文本经
      OpenClaw /v1 流式端点直推引擎逐句 TTS）。非确定性来源：模型输出风格漂移无法
      offline 断言根治——「prompt 写明 ≠ LLM 遵循」（M3K3 同款行为等价假设，LLM 遵循度
      offline 不可机判）。观测方式 = journalctl（OpenClaw 网关侧）+ docker logs（引擎
      TTS 面）双侧逐句对账留痕，human 真机轮目击判定，不造假绿。
    """

    maxDiff = None

    @classmethod
    def setUpClass(cls):
        cls.v9_path = DEPLOY_DIR / V9_NAME
        cls.v9 = _read_text(cls.v9_path)
        cls.v8_path = DEPLOY_DIR / V8_NAME

    # -- PN-A1 落盘 + 无前言结构齿（V7-A1 同形态） -----------------------------

    def test_pn_a1_v9_prompt_file_lands(self):
        """coverage: PN-A1 — v9 落盘非空（版本化演进，历史版本共存可 diff）。"""
        self.assertTrue(
            self.v9_path.is_file(),
            f"PN-A1 RED: {V9_NAME} 未落盘——v9 版本化 bump 未发生，课表问答回合"
            "反叙述纪律（TECH-261010 根治）无载体（E4 族）。",
        )
        self.assertGreater(
            len(self.v9.strip()), 0,
            "PN-A1 RED: v9 文件为空。",
        )

    def test_pn_a1_v9_no_version_preamble(self):
        """coverage: PN-A1 — 制品本体无版本沿革前言：首行即 `## 角色`（V7-A1 同形态）。
        沿革只允许住 OPENCLAW_SYSTEM_PROMPT_CARRIER（260925-prompt-v7-round-routing
        起的版本化纪律，bump 不得回潮）。"""
        self.assertTrue(
            self.v9_path.is_file(), "PN-A1 RED: v9 未落盘（前置）。")
        first = self.v9.splitlines()[0].strip() if self.v9.splitlines() else ""
        self.assertEqual(
            first, "## 角色",
            "PN-A1 RED: v9 首行不是 `## 角色`——版本沿革前言回潮或角色节被移位"
            "（沿革只允许住 OPENCLAW_SYSTEM_PROMPT_CARRIER）。",
        )
        self.assertFalse(
            self.v9.startswith("#") and not self.v9.startswith("## "),
            "PN-A1 RED: v9 顶层出现 H1 前言标题（前言回潮）。",
        )

    # -- PN-A2 课表问答回合行对称反叙述纪律锚 -----------------------------------

    def test_pn_a2_v9_schedule_row_anti_narration_symmetry(self):
        """coverage: PN-A2 — 课表问答回合行含对称反叙述纪律（内容驱动选节、并集
        窗口不钉措辞）：「先读 schedule.md」既有锚保持 + 反叙述纪律入行。"""
        self.assertTrue(
            self.v9_path.is_file(), "PN-A2 RED: v9 未落盘（前置）。")
        _assert_schedule_row_anti_narration(self, self.v9, f"{V9_NAME} 文件")

    # -- PN-A3 v8 历史版本零回改（V7 ⑤ 形态复刻，must-stay-green 负锚） --------

    def test_pn_a3_v8_prompt_file_immutable(self):
        """coverage: PN-A3 — v8 本体零回改（E2 铁律同形态；v1..v7 由既有各包齿钉）。
        v8 冻结时点 sha256 = 91f4a003…（2026-10-11 PN 批冻结，raw bytes 口径）。"""
        self.assertTrue(
            self.v8_path.is_file(),
            "PN-A3 RED: v8 文件被删除——历史版本共存可 diff 铁律被破坏。",
        )
        self.assertEqual(
            _sha256_raw(self.v8_path), V8_SHA256,
            "PN-A3 RED: v8 本体被回改——历史版本一律不改（版本化落盘惯例；如确需改动"
            "属治理裁决，须 human ruling 后更新本钉）。",
        )

    # -- PN-A4 三面接线一致 + 接线版承载 v9 纪律锚（版本无关形态） --------------

    def test_pn_a4_wired_three_faces_consistent_and_carries_v9_discipline(self):
        """coverage: PN-A4 — 三面接线一致且接线版承载课表行反叙述纪律。

        版本无关形态（ARQ-260908 / 本文件头注②同纪律，接线类断言零版本字面）：
        「接线 = v9」的语义由组合承载——① 三面解析一致（渲染面 apply-xiaozhi-overlay.sh
        PROMPT_SRC/PROMPT_DST == 声明面 provider.config.yaml 三键 == 载体面
        OPENCLAW_SYSTEM_PROMPT_CARRIER 声明行，must-stay-green 半）；② 接线版文件
        落盘非空；③ 接线版正文携带 PN-A2 纪律锚（红→绿半：现树接线 v8 无课表行
        纪律 = 红；bump v9 + 三面同步后转绿；未来 bump 丢纪律同样红，防丢内容）。
        ④ v9 世系最小锚抽检（严禁凭印象 / story_list / 问答框）承载「v9 = v8 全量 +
        课表行扩行」的继承面。"""
        rendered, declared, carried = _wired_prompt_faces_pn_guard()
        self.assertTrue(
            rendered and declared and carried,
            f"PN-A4 RED: 接线面解析失败（rendered={rendered} declared={declared} "
            f"carried={carried}）——prompt 落盘即孤儿（PROMPT_SRC/PROMPT_DST、"
            "provider.config.yaml system_prompt 三键、OPENCLAW_SYSTEM_PROMPT_CARRIER "
            "声明行须指向 openclaw-system-prompt.vN.md）。",
        )
        self.assertEqual(
            set(rendered), set(declared),
            f"PN-A4 RED: 渲染面 {sorted(rendered)} 与声明面 {sorted(declared)} 指向"
            "不同版本——「验收所测 = 生产所用」断裂（ADR-001 接线不变式，bump 三面"
            "须同批）。",
        )
        self.assertEqual(
            set(rendered), set(carried),
            f"PN-A4 RED: 载体面（OPENCLAW_SYSTEM_PROMPT_CARRIER）声明 {sorted(carried)} "
            f"与渲染/声明两面 {sorted(rendered)} 不一致——carrier 声明未随 bump 同批。",
        )
        name = sorted(set(rendered))[0]
        path = DEPLOY_DIR / name
        self.assertTrue(
            path.is_file(), f"PN-A4 RED: 被接线的 {name} 未落盘（孤儿声明）。")
        body = _read_text(path)
        self.assertTrue(
            body.strip(), f"PN-A4 RED: 被接线的 {name} 是空文件。")
        # 红→绿半：接线版必须已携带课表行反叙述纪律（现接线 v8 无 = 设计态红）。
        _assert_schedule_row_anti_narration(self, body, f"接线版 {name}")
        # 世系最小锚抽检（v9 = v8 全量继承；防 bump 丢段）。
        for needle, why in (
            ("严禁凭印象", "课表反编造覆盖句（9/25 live 编造事故根治锚）"),
            ("story_list", "禁产 story JSON 锚（2026-09-24 契约门 Q7）"),
            ("问答框", "课表问答口径锚（M3K3-A1）"),
        ):
            self.assertIn(
                needle, body,
                f"PN-A4 RED: 接线版 {name} 丢失世系锚「{needle}」——{why}；"
                "版本化 bump 只许扩行不许丢段（plan.md：v9 = v8 全量 + 课表行扩行）。",
            )


if __name__ == "__main__":
    unittest.main(verbosity=2)
