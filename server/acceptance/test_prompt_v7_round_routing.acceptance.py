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

────────────────────────────────────────────────────────────────────────────
261012-kidpal-realtime-facts 增批（additive 补齿，tighten-only 窗口——既有齿零触碰、
既有断言语义不动；TaskPackage = delivery/taskpackages/261012-kidpal-realtime-facts，
源头 = docs/crq/tech/TECH-261012-weather-fabrication + requirements v1.62
NFR-SEC-2 ④（实时事实回合出网记账，行为口径权威）+ NFR-SAFE-1（联网转述边界，
行为口径权威）+ intake spine anchors.contract = NO_CONTRACT_CHANGE）：

冻结范围（RF-A1..A5）：
  ⑪ v10 落盘非空 + 首行 `## 角色` 无前言结构齿（RF-A1，PN-A1 同形态）；
  ⑫ 实时事实回合行纪律锚 + 对话行通用反编造锚 + 底线节第三不变量（RF-A2）：v9 反编造
     纪律逐域枚举（课表/查字/故事）漏通用实时事实域，天气问题落对话回合 = 无数据源又
     禁工具，结构性逼出编造（TECH-261012 现场三源对账）——v10 三处补死；
  ⑬ v9 历史版本 sha256 零回改齿（RF-A3，PN-A3 形态复刻）；
  ⑭ 三面接线一致 + 接线版承载 ⑫ 全部锚（RF-A4，PN-A4 同构红→绿半同构形态）；
  ⑮ RF-A5 = live 观测项（非自动断言，非确定性诚实申报，见
     TestPromptV10RealtimeFacts 类 docstring）。

强度申报（acceptance-evidence「契约条目 ↔ 齿 的强度申报」体例）：
  - RF-A1 → 落盘/结构齿 → 机制级（文件存在性 + 首行字面）；
  - RF-A2 → 源锚代理（v10 的产出物就是措辞本身，「源锚」即工件面）：并集口径不钉具体
    措辞——提示半/真查半/依据结果半/降级诚实半/禁编造半/禁虚称半/转述边界半七组半同义
    锚，措辞换写不误红、纪律语义被删才红；行级定位按回合类型名「实时事实回合」取表行
    （内容驱动、不按行号、按「|」过滤引言句回指）；「prompt 写明 ≠ LLM 遵循」行为等价
    假设显式化——LLM 实际遵循度 offline 不可机判，归 RF-A5 真机轮目击（M3K3/PN-A5
    同款申报）；
  - RF-A3 → 机制级（raw bytes sha256；v9 为 LF 落盘，raw == 换行归一双口径同值，
    2026-10-12 RF 批冻结实测核验）；
  - RF-A4 → 机制级（三面结构化指向解析 + 一致性 + 接线版语义锚在场）；三面解析器复用
    本文件 PN 批 _wired_prompt_faces_pn_guard（同文件内复用，零新增副本）；世系最小锚
    抽检（严禁凭印象 / story_list / 问答框）承载「v10 = v9 全量 + 实时事实扩行」继承面，
    防 bump 丢段；
  - 契约零涉及（spine anchors.contract = NO_CONTRACT_CHANGE，architect-reviewer 四分类
    2026-10-12：契约 §1.6 逐字引文未枚举回合路由/工具政策，v10 additive 逐面零冲突）：
    本批齿全部为 deploy 制品面锚，无 DB/事件断言（TaskPackage notes 地雷前置申报在案）。
  - 断言设计透镜（fixture 自含性声明）：全部齿直读 repo 内 deploy 制品，无外部资产 /
    时间真值 / 环境前提依赖；行为 seam（prompt 纪律 → LLM 行为）的驱动方 = 真机轮
    RF-A5（human 在场），offline 侧无驱动方 = 已申报盲区（非自动断言诚实申报，不伪装
    自动覆盖）。

RED 基线预期（冻结时点树，v10 未建、接线未改）：RF-A1 两齿 + RF-A2 三齿 + RF-A4 =
设计态红，原因单一（v10 文件缺失 / 现接线 v9 无实时事实回合行 = 真实缺失行为）；
RF-A3 = must-stay-green（v9 未被回改即绿，回改才红——负锚形态同 PN-A3）；既有齿
零触碰、既有绿不降。四个版本无关守卫（m3k3 A1 / MCD / M4K3-A8 / PN-A4）四面同批
翻即绿、漏翻必红（must-stay-green，不在本批改动面）。
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


# --------------------------------------------------------------------------- #
# RF 批辅助（261012-kidpal-realtime-facts，additive）
# --------------------------------------------------------------------------- #

V10_NAME = "openclaw-system-prompt.v10.md"

# 冻结时点 v9 内容 hash（raw bytes 口径；v9 为 LF 落盘，raw == 换行归一双口径同值，
# 2026-10-12 RF 批冻结实测）。v10 bump 与一切后续包不得改 v9 本体（V7 ⑤ / PN-A3
# 形态复刻）。计算式 = sha256(read_bytes())。
V9_SHA256 = "0b051e14b7f3f725327fb165c250bdf36abcd39ca03780c1db75f3878631de2c"

# 实时事实回合行纪律锚组（NFR-SEC-2 ④ 行为口径 + NFR-SAFE-1 转述边界的并集容换写——
# 措辞换写不误红、纪律语义被删才红；provider 名 Tavily 不入锚组，防供应商换写假红）。
# 先播提示半（human 260910-12 定调原话「可以提示，我去查一下，然后开始查」）。
_REALTIME_HINT_ANCHORS = ("我去查", "查一下", "先播", "先说", "提示")
# 真查半（平台联网搜索；不钉 provider 名）。
_REALTIME_QUERY_ANCHORS = ("查询", "搜索", "联网", "真查")
# 只依据查询结果作答 = 依据连接词半 × 结果名词半（双半同现才绿，防单词假绿）。
_REALTIME_BASIS_HALF = ("依据", "根据", "基于")
_REALTIME_RESULT_HALF = ("查询结果", "搜索结果", "检索结果")
# 降级诚实半 × 引导半（查不到/失败/超时 → 诚实说不知道并引导，NFR-SEC-2 ④）。
_REALTIME_DEGRADE_ANCHORS = ("没查到", "查不到", "不知道", "查询失败", "失败", "超时")
_REALTIME_HONESTY_ANCHORS = ("诚实", "如实", "引导", "换个问法", "换个话题")
# 禁编造半（TECH-261012：严禁编造数字）——与既有 _PROHIBITION_ANCHORS 连用。
_REALTIME_FABRICATION_HALF = ("编造", "凭印象", "凭空", "瞎编")
# 禁虚称已查半（TECH-261012：「我去查一下」承诺本身也是虚构）。
_FALSE_CLAIM_ANCHORS = ("虚称", "谎称", "假称", "冒称", "佯称", "假装")
# NFR-SAFE-1 转述边界半 × 不逐字照搬半（外部内容儿童化改写后转述，双半同现才绿）。
_PARAPHRASE_ADAPT_HALF = ("儿童化", "改写", "听得懂", "转述")
_PARAPHRASE_COPY_HALF = ("照搬", "逐字", "原样")


def _routing_rows(body: str, round_name: str) -> list[str]:
    """回合路由表行提取（内容驱动：按回合类型名定位表行，不按行号）。

    只取含回合类型名**且含「|」的表行**——引言句（「判不清时按『孩子对话回合』
    处理」）里的回合名回指不得充当表行（PN 批 _schedule_qa_rows 同纪律扩用）。"""
    window = _section_window(body, ("回合类型",), ("回合",))
    return [l for l in window.splitlines() if round_name in l and "|" in l]


def _baseline_paragraph(body: str) -> str:
    """底线节段落提取（内容驱动：定位含「不变的底线」的行至该段段尾，不按行号）。"""
    lines = body.splitlines()
    for i, l in enumerate(lines):
        if "不变的底线" in l:
            j = i
            while j < len(lines) and lines[j].strip():
                j += 1
            return "\n".join(lines[i:j])
    return ""


def _assert_realtime_row_anchors(testcase, body: str, label: str):
    """RF-A2 实时事实回合行锚组（内容驱动选行、并集口径不钉措辞）：
    ① 先播提示半（不静默干等）；② 真查半；③ 依据×结果双半（不无依据抢答）；
    ④ 降级诚实×引导；⑤ 禁编造（×禁止语义）；⑥ 禁虚称已查；⑦ NFR-SAFE-1 转述边界
    （儿童化改写 × 不逐字照搬）。"""
    rows = _routing_rows(body, "实时事实回合")
    testcase.assertTrue(
        rows,
        f"RF-A2 RED({label}): 回合路由面无「实时事实回合」表行——TECH-261012 根治"
        "（实时事实回合路由扩行）未落地，或回合类型路由表结构被破坏（V7-A1 既有锚面"
        "回退）。")
    row = "\n".join(rows)
    testcase.assertTrue(
        any(w in row for w in _REALTIME_HINT_ANCHORS),
        f"RF-A2 RED({label}): 实时事实回合行无先播提示锚（我去查/查一下/先播/先说/"
        "提示任一）——「不静默干等」半缺失（NFR-SEC-2 ④ 行为口径）。")
    testcase.assertTrue(
        any(w in row for w in _REALTIME_QUERY_ANCHORS),
        f"RF-A2 RED({label}): 实时事实回合行无真查锚（查询/搜索/联网/真查任一）——"
        "「然后开始查」半缺失（NFR-SEC-2 ④：真查而非编造）。")
    testcase.assertTrue(
        any(w in row for w in _REALTIME_BASIS_HALF)
        and any(w in row for w in _REALTIME_RESULT_HALF),
        f"RF-A2 RED({label}): 实时事实回合行无「只依据查询结果作答」口径（依据/根据/"
        "基于 × 查询结果/搜索结果/检索结果 双半同现才绿）——不无依据抢答半缺失。")
    testcase.assertTrue(
        any(w in row for w in _REALTIME_DEGRADE_ANCHORS),
        f"RF-A2 RED({label}): 实时事实回合行无降级触发锚（没查到/查不到/不知道/查询"
        "失败/失败/超时任一）——降级路径无载体。")
    testcase.assertTrue(
        any(w in row for w in _REALTIME_HONESTY_ANCHORS),
        f"RF-A2 RED({label}): 实时事实回合行无诚实/引导锚（诚实/如实/引导/换个问法/"
        "换个话题任一）——「诚实说不知道并引导」半缺失（NFR-SEC-2 ④）。")
    testcase.assertTrue(
        any(w in row for w in _REALTIME_FABRICATION_HALF)
        and any(w in row for w in _PROHIBITION_ANCHORS),
        f"RF-A2 RED({label}): 实时事实回合行无禁编造条款（编造/凭印象/凭空/瞎编 × "
        "禁止语义任一双半同现）——TECH-261012 编造根治锚缺失。")
    testcase.assertTrue(
        any(w in row for w in _FALSE_CLAIM_ANCHORS),
        f"RF-A2 RED({label}): 实时事实回合行无禁虚称已查锚（虚称/谎称/假称/冒称/佯称/"
        "假装任一）——「我去查一下」式虚构承诺封堵锚缺失（TECH-261012 现场第二幻觉面）。")
    testcase.assertTrue(
        any(w in row for w in _PARAPHRASE_ADAPT_HALF)
        and any(w in row for w in _PARAPHRASE_COPY_HALF),
        f"RF-A2 RED({label}): 实时事实回合行无联网转述边界（儿童化/改写/听得懂/转述 "
        "× 照搬/逐字/原样 双半同现才绿）——外部内容儿童化改写后转述、不逐字照搬"
        "（NFR-SAFE-1 2026-10-12 增）缺失。")


def _assert_dialog_row_generic_antifabrication(testcase, body: str, label: str):
    """RF-A2 对话行通用反编造锚（v9 逐域枚举漏的通用实时事实域补死）：对话回合行
    提及实时信息域，且（行内通用反编造条款 或 实时事实回合转路由锚）二者居一。"""
    rows = _routing_rows(body, "孩子对话回合")
    testcase.assertTrue(
        rows,
        f"RF-A2 RED({label}): 回合路由面无「孩子对话回合」表行——回合类型路由表"
        "结构被破坏（V7-A1 既有锚面回退）。")
    row = "\n".join(rows)
    testcase.assertIn(
        "实时", row,
        f"RF-A2 RED({label}): 对话回合行未提及实时信息域——v9 逐域枚举反编造漏的"
        "通用实时事实域未在对话行补死（TECH-261012 根因：天气问题落对话回合 = 无数据源"
        "又禁工具，结构性逼出编造）。")
    testcase.assertTrue(
        any(w in row for w in _REALTIME_FABRICATION_HALF) or "实时事实回合" in row,
        f"RF-A2 RED({label}): 对话回合行既无通用反编造条款（编造/凭印象/凭空/瞎编任一）"
        "也无实时事实回合转路由锚——对话行的实时信息反编造缺口未闭合。")


def _assert_baseline_third_invariant(testcase, body: str, label: str):
    """RF-A2 底线节第三不变量锚：底线节在「课表问题永不编造」「查字回合绝对禁工具」
    之后扩第三条——实时信息无查询支撑永不编造、永不虚称已查询。"""
    para = _baseline_paragraph(body)
    testcase.assertTrue(
        para,
        f"RF-A2 RED({label}): 底线节（「不变的底线」句）缺失——超越回合分类的底线"
        "结构被破坏（v9 既有两条不变量锚面回退）。")
    testcase.assertIn(
        "实时信息", para,
        f"RF-A2 RED({label}): 底线节无第三不变量（实时信息域未入底线）——plan.md"
        "「底线节扩第三不变量：实时信息无查询支撑永不编造、永不虚称已查询」未落地。")
    testcase.assertTrue(
        any(w in para for w in _REALTIME_FABRICATION_HALF),
        f"RF-A2 RED({label}): 底线节第三不变量无禁编造半（编造/凭印象/凭空/瞎编任一）。")
    testcase.assertTrue(
        any(w in para for w in _FALSE_CLAIM_ANCHORS) or "已查询" in para,
        f"RF-A2 RED({label}): 底线节第三不变量无禁虚称已查半（虚称/谎称/假称/冒称/"
        "佯称/假装/已查询任一）。")


class TestPromptV10RealtimeFacts(unittest.TestCase):
    """261012-kidpal-realtime-facts（offline，静态锚齿，additive 补齿）。

    RF-A5（live 观测项——**非自动断言**，非确定性诚实申报，本类不落测试）：
      ECS 部署 v10 + overlay 重跑（重跑后核 model_name=openclaw/kidpal 与决策 21 两键
      未被冲）后，真机问「今天西安的天气怎么样」全链路——预期：先播「我去查一下」式
      提示 → journalctl（OpenClaw 网关侧）+ docker logs（引擎 TTS 面）双侧对账真出
      web search（Tavily）调用 → 只依据查询结果作答（TECH-261012 现场模型自纠后的正确
      形态固化为纪律后的遵循验证）；再加测一次降级路径（断 Tavily 或问查不到的事实）
      ——诚实说没查到 + 引导，零编造、零虚称已查。非确定性来源：模型输出与工具调用
      行为无法 offline 机判——「prompt 写明 ≠ LLM 遵循」（M3K3 / PN-A5 同款行为等价
      假设，offline 侧无驱动方 = 已申报盲区）。观测 = 双侧逐句对账留痕 + human 真机轮
      在场目击判定；延迟口径先观测后钉阈值（spine openQ q3 human 裁：提示 ~4-5s 内
      首句语音 / 完整答复目标 ≤15s 上限 20s）。不造假绿、不计入 pytest 通过面。
    """

    maxDiff = None

    @classmethod
    def setUpClass(cls):
        cls.v10_path = DEPLOY_DIR / V10_NAME
        cls.v10 = _read_text(cls.v10_path)
        cls.v9_path = DEPLOY_DIR / V9_NAME

    # -- RF-A1 落盘 + 无前言结构齿（PN-A1 同形态） -----------------------------

    def test_rf_a1_v10_prompt_file_lands(self):
        """coverage: RF-A1 — v10 落盘非空（版本化演进，历史版本共存可 diff）。"""
        self.assertTrue(
            self.v10_path.is_file(),
            f"RF-A1 RED: {V10_NAME} 未落盘——v10 版本化 bump 未发生，实时事实回合"
            "路由（TECH-261012 根治）无载体（E4 族）。",
        )
        self.assertGreater(
            len(self.v10.strip()), 0,
            "RF-A1 RED: v10 文件为空。",
        )

    def test_rf_a1_v10_no_version_preamble(self):
        """coverage: RF-A1 — 制品本体无版本沿革前言：首行即 `## 角色`（PN-A1 同形态）。
        沿革只允许住 OPENCLAW_SYSTEM_PROMPT_CARRIER（260925 起的版本化纪律，bump
        不得回潮）。"""
        self.assertTrue(
            self.v10_path.is_file(), "RF-A1 RED: v10 未落盘（前置）。")
        first = self.v10.splitlines()[0].strip() if self.v10.splitlines() else ""
        self.assertEqual(
            first, "## 角色",
            "RF-A1 RED: v10 首行不是 `## 角色`——版本沿革前言回潮或角色节被移位"
            "（沿革只允许住 OPENCLAW_SYSTEM_PROMPT_CARRIER）。",
        )
        self.assertFalse(
            self.v10.startswith("#") and not self.v10.startswith("## "),
            "RF-A1 RED: v10 顶层出现 H1 前言标题（前言回潮）。",
        )

    # -- RF-A2 实时事实回合行 + 对话行通用反编造 + 底线节第三不变量 -------------

    def test_rf_a2_v10_realtime_facts_row_discipline(self):
        """coverage: RF-A2 — 实时事实回合表行含完整纪律锚组（先播提示 → 真查 →
        只依据查询结果作答 → 降级诚实引导；禁编造、禁虚称已查；NFR-SAFE-1 转述边界）。"""
        self.assertTrue(
            self.v10_path.is_file(), "RF-A2 RED: v10 未落盘（前置）。")
        _assert_realtime_row_anchors(self, self.v10, f"{V10_NAME} 文件")

    def test_rf_a2_v10_dialog_row_generic_antifabrication(self):
        """coverage: RF-A2 — 对话回合行通用反编造锚（v9 逐域枚举漏的通用实时事实域
        在对话行补死，或显式转路由到实时事实回合）。"""
        self.assertTrue(
            self.v10_path.is_file(), "RF-A2 RED: v10 未落盘（前置）。")
        _assert_dialog_row_generic_antifabrication(self, self.v10, f"{V10_NAME} 文件")

    def test_rf_a2_v10_baseline_third_invariant(self):
        """coverage: RF-A2 — 底线节第三不变量（实时信息无查询支撑永不编造、永不虚称
        已查询）与既有两条不变量并列在场。"""
        self.assertTrue(
            self.v10_path.is_file(), "RF-A2 RED: v10 未落盘（前置）。")
        _assert_baseline_third_invariant(self, self.v10, f"{V10_NAME} 文件")

    # -- RF-A3 v9 历史版本零回改（PN-A3 形态复刻，must-stay-green 负锚） --------

    def test_rf_a3_v9_prompt_file_immutable(self):
        """coverage: RF-A3 — v9 本体零回改（E2 铁律同形态；v1..v8 由既有各包齿钉）。
        v9 冻结时点 sha256 = 0b051e14…（2026-10-12 RF 批冻结，raw bytes 口径）。"""
        self.assertTrue(
            self.v9_path.is_file(),
            "RF-A3 RED: v9 文件被删除——历史版本共存可 diff 铁律被破坏。",
        )
        self.assertEqual(
            _sha256_raw(self.v9_path), V9_SHA256,
            "RF-A3 RED: v9 本体被回改——历史版本一律不改（版本化落盘惯例；如确需改动"
            "属治理裁决，须 human ruling 后更新本钉）。",
        )

    # -- RF-A4 三面接线一致 + 接线版承载实时回合锚（版本无关形态） --------------

    def test_rf_a4_wired_three_faces_consistent_and_carries_realtime_anchors(self):
        """coverage: RF-A4 — 三面接线一致且接线版承载 RF-A2 全部锚。

        版本无关形态（ARQ-260908 / 本文件头注②同纪律，接线类断言零版本字面）：
        「接线 = v10」的语义由组合承载——① 三面解析一致（渲染面
        apply-xiaozhi-overlay.sh PROMPT_SRC/PROMPT_DST == 声明面 provider.config.yaml
        三键 == 载体面 OPENCLAW_SYSTEM_PROMPT_CARRIER 声明行，must-stay-green 半）；
        ② 接线版文件落盘非空；③ 接线版正文携带 RF-A2 全部锚（红→绿半：现树接线 v9
        无实时事实回合行 = 设计态红；bump v10 + 三面同批后转绿；未来 bump 丢锚同样
        红，防丢内容）。④ v10 世系最小锚抽检（严禁凭印象 / story_list / 问答框）
        承载「v10 = v9 全量 + 实时事实扩行」的继承面。"""
        rendered, declared, carried = _wired_prompt_faces_pn_guard()
        self.assertTrue(
            rendered and declared and carried,
            f"RF-A4 RED: 接线面解析失败（rendered={rendered} declared={declared} "
            f"carried={carried}）——prompt 落盘即孤儿（PROMPT_SRC/PROMPT_DST、"
            "provider.config.yaml system_prompt 三键、OPENCLAW_SYSTEM_PROMPT_CARRIER "
            "声明行须指向 openclaw-system-prompt.vN.md）。",
        )
        self.assertEqual(
            set(rendered), set(declared),
            f"RF-A4 RED: 渲染面 {sorted(rendered)} 与声明面 {sorted(declared)} 指向"
            "不同版本——「验收所测 = 生产所用」断裂（ADR-001 接线不变式，bump 三面"
            "须同批）。",
        )
        self.assertEqual(
            set(rendered), set(carried),
            f"RF-A4 RED: 载体面（OPENCLAW_SYSTEM_PROMPT_CARRIER）声明 {sorted(carried)} "
            f"与渲染/声明两面 {sorted(rendered)} 不一致——carrier 声明未随 bump 同批。",
        )
        name = sorted(set(rendered))[0]
        path = DEPLOY_DIR / name
        self.assertTrue(
            path.is_file(), f"RF-A4 RED: 被接线的 {name} 未落盘（孤儿声明）。")
        body = _read_text(path)
        self.assertTrue(
            body.strip(), f"RF-A4 RED: 被接线的 {name} 是空文件。")
        # 红→绿半：接线版必须已携带实时事实回合全部锚（现接线 v9 无 = 设计态红）。
        _assert_realtime_row_anchors(self, body, f"接线版 {name}")
        _assert_dialog_row_generic_antifabrication(self, body, f"接线版 {name}")
        _assert_baseline_third_invariant(self, body, f"接线版 {name}")
        # 世系最小锚抽检（v10 = v9 全量继承；防 bump 丢段）。
        for needle, why in (
            ("严禁凭印象", "课表反编造覆盖句（9/25 live 编造事故根治锚）"),
            ("story_list", "禁产 story JSON 锚（2026-09-24 契约门 Q7）"),
            ("问答框", "课表问答口径锚（M3K3-A1）"),
        ):
            self.assertIn(
                needle, body,
                f"RF-A4 RED: 接线版 {name} 丢失世系锚「{needle}」——{why}；"
                "版本化 bump 只许扩行不许丢段（plan.md：v10 = v9 全量 + 实时事实扩行）。",
            )


if __name__ == "__main__":
    unittest.main(verbosity=2)
