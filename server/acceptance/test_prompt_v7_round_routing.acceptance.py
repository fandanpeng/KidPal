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

────────────────────────────────────────────────────────────────────────────
261017-prompt-v11-schedule-recall 增批（additive 补齿，tighten-only 窗口——既有齿
零触碰、既有断言语义不动；TaskPackage = delivery/taskpackages/
261017-prompt-v11-schedule-recall，源头 = docs/debt/tech/TECH-261017-schedule-
answer-skip-read-hallucination（MAJOR，真机问「后天都有什么课程？」零工具调用凭
印象编造）+ TECH-261017-xiaozhi-template-residue-deploy-ssot-gap（SHOULD））：

冻结范围（PV-A1/A2/A3/A4/A7 落本文件；PV-A5 落
test_261010_engine_decision21_overlay_persist；PV-A6 落
test_261017_openclaw_kidpal_skills（新建）；PV-A8 = structural live-manual 观测项，
非自动断言、不落测试）：
  ⑯ v11 落盘非空 + 首行 `## 角色` 无前言 + 课表问答回合行识别特征扩列锚（PV-A1）：
     时间面（今天/明天/后天/大后天/周X/星期X/周几/这周/下周/号）与主题面
     （课表/课程/上课/上什么课/几点上课/要带什么）并集容换写，且纪律仍落在
     「先读 schedule.md 再作答 / 读不到诚实说查不到」同一表行内；
  ⑰ v11 相对 v10 的差异只落在课表问答回合表行（PV-A1，diff-confinement；同
     M3K3-A1 v5/v6 逐字比对齿——「其余段落相对 v10 零偏移」的机判形态）；
  ⑱ v10 历史版本零回改（PV-A2，RF-A3 形态复刻，must-stay-green 负锚）；
  ⑲ 四面接线一致（渲染面 PROMPT_SRC/DST / 声明面 provider.config.yaml 三键 /
     载体面 OPENCLAW_SYSTEM_PROMPT_CARRIER 声明行 / manifest.yaml prompt 条目）+
     接线版承载 ⑯ 扩列锚 + carrier 沿革 append 且历史行不删（PV-A3，版本无关形态：
     从结构化指向解析当前版，禁 v11 字面当作唯一判据）；
  ⑳ KidPal 极简模板 `kidpal-agent-prompt.txt` 三段位与残留禁令（PV-A4）+
     manifest 条目齐 / entries 静态可解析 / README prompt 指针节跟当前接线版
     （PV-A7）。

强度申报（acceptance-evidence「契约条目 ↔ 齿 的强度申报」体例）：
  - PV-A1 → 落盘/结构齿（机制级：文件存在性 + 首行字面）+ 源锚代理（v11 的产出物
    就是措辞本身；扩列锚采用并集半同义组，措辞换写不误红、识别面被删才红）；
    diff-confinement 为机制级（逐行比对，断言「只许扩课表识别特征列」）；
    「prompt 写明 ≠ LLM 遵循」行为等价假设显式化——LLM 实际遵循度 offline 不可
    机判，归 PV-A8 真机轮目击；
  - PV-A2 → 机制级（LF 归一 sha256；v10 仓内 blob 为 LF——本树 Windows 检出侧
    autocrlf 会把工作树转 CRLF，故按 LF 归一口径钉，与 canonical blob 同值、
    检出环境无关）；
  - PV-A3 → 机制级（四面结构化指向解析 + 一致性 + 接线版语义锚在场 + carrier
    沿革对既存历史版本文件全覆盖）；解析器为本文件自含实现（跨冻结件 import 会
    把冻结件焊死，PN 批同纪律）；
  - PV-A4 → 源锚代理（模板产出物即文本；三段位 + 残留词表为机制级文本锚）；
  - PV-A7 → 机制级（manifest YAML 静态解析 + verify-deploy.sh 语义等价校验 +
    README 指针对结构化解析出的当前接线版）；
  - fixture 自含性声明：全部齿直读 repo 内 deploy 制品，无外部资产 / 时间真值
    依赖；唯一环境前提 = PyYAML（与 verify-deploy.sh 同一解析器，缺库时判 infra
    红、不静默跳过）；行为 seam（prompt 纪律 → LLM 行为）的驱动方 = PV-A8
    真机轮（human 在场），offline 侧无驱动方 = 已申报盲区；
  - 契约零涉及（crossEnd exempt：零 wire 形状改动），本批齿全部为 deploy 制品面
    锚，无 DB/事件断言（TaskPackage notes 地雷前置申报在案）。

RED 基线预期（冻结时点树，v11 未建、接线未改）：PV-A1 三齿 + PV-A3 接线版扩列
锚 = 设计态红（v11 文件缺失 / 现接线 v10 无后天与大后天识别特征 = 真实缺失行为）；
PV-A2 = must-stay-green（v10 未被回改即绿）；PV-A3 四面一致与 carrier 沿革齿、
PV-A7 静态可解析与 README 指针齿 = 现树绿、bump 漏翻才红（must-stay-green 半：
bump 后 README 仍指 v10 / manifest 漏改即红）；PV-A4 + PV-A7 manifest 条目齐 =
设计态红（模板与 skills 条目未建）。既有 V7/PN/RF 齿零触碰、既有绿不降。
"""

from __future__ import annotations

import hashlib
import re
import unittest
from itertools import zip_longest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DEPLOY_DIR = REPO_ROOT / "server" / "deploy"

V7_NAME = "openclaw-system-prompt.v7.md"
V6_NAME = "openclaw-system-prompt.v6.md"
V8_NAME = "openclaw-system-prompt.v8.md"
V9_NAME = "openclaw-system-prompt.v9.md"

# 冻结时点 v6 内容 hash（raw bytes 口径 = canonical blob 口径，eol=lf 钉后检出即 LF；
# v7 bump 与一切后续包不得改 v6 本体）。
# 计算式 = sha256(read_bytes())。261013-gov-review-lf-eol-teeth-pin 批重算：原冻结值
# bf5c6b4c… 为 Windows CRLF 检出伪影（census 实测），重算对齐 canonical blob 425d293d…。
V6_SHA256 = "425d293d35b106b581785a78055f5718068531b240d52db763591eed68b0cc62"

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
        v6 冻结时点 sha256 = 425d293d…（260925-prompt-v7-round-routing 批冻结；
        261013-gov-review-lf-eol-teeth-pin 批重算对齐 canonical blob——原值 bf5c6b4c… 为 Windows CRLF 检出伪影）。"""
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


# --------------------------------------------------------------------------- #
# PV 批辅助（261017-prompt-v11-schedule-recall，additive）
# --------------------------------------------------------------------------- #

PV11_NAME = "openclaw-system-prompt.v11.md"
PV11_TEMPLATE_NAME = "kidpal-agent-prompt.txt"
PV11_SKILLS_DIRNAME = "261017-openclaw-kidpal-skills"
PV11_MANIFEST = DEPLOY_DIR / "manifest.yaml"
PV11_README = DEPLOY_DIR / "README.md"

# 冻结时点 v10 内容 hash（LF 归一口径 = canonical blob 口径）。v11 bump 与一切后续
# 包不得改 v10 本体（V7 ⑤ / PN-A3 / RF-A3 形态复刻）。
# 口径说明（2026-10-18 PV 批冻结实测）：仓内 blob 为 LF（`git show HEAD:…` 实测
# sha256 = 本值）；本树 Windows 检出侧 autocrlf 把工作树文件转成 CRLF，raw bytes
# sha256 因而与 blob 不同——故统一按 LF 归一后计算（read_text + \r\n→\n），
# canonical blob 与任何检出环境同值。
PV11_V10_SHA256_LF = "963b2b79bf2331dd76528f282aca620c39f764aef8b8888c2b96af7ae90b9b23"

# 课表问答回合识别特征扩列锚（plan.md §A 原文并集半同义组：措辞换写不误红、
# 识别面被删才红）。时间面含「后天 / 大后天」——TECH-261017 真机触发词（原 v10 只
# 枚举今天/明天/这周，「后天」落对话回合兜底 = 结构性逼出编造）。
PV11_SCHEDULE_SUBJECT_ANCHORS = (
    "课表", "课程", "上课", "上什么课", "几点上课", "要带什么")
PV11_SCHEDULE_TIME_ANCHORS = ("今天", "明天", "周几", "这周", "下周", "号")
PV11_WEEKDAY_RE = re.compile(r"周[一二三四五六日天1-7Xx×]")
PV11_XINGQI_RE = re.compile(r"星期[一二三四五六日天1-7Xx×]")
# 「只要提到就一律判课表回合」的全称量词半（human 2026-10-17 裁「只要提到课表就去
# 读」；不收「一律」单词——v10 课表行已有「一律违规」，单词会假绿）。
PV11_UNIVERSAL_ANCHORS = (
    "凡提到", "只要提到", "任何提到", "凡是提到", "凡涉及", "只要涉及", "凡是涉及")

PV11_PROMPT_FILE_RE = re.compile(r"openclaw-system-prompt\.v(\d+)\.md")


def _sha256_lf(path: Path) -> str:
    """LF 归一 sha256（canonical blob 口径；检出 eol 无关）。"""
    try:
        txt = path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""
    return hashlib.sha256(txt.replace("\r\n", "\n").encode("utf-8")).hexdigest()


def _pv_manifest_entries(testcase) -> list[dict]:
    """解析 manifest.yaml entries（与 verify-deploy.sh 同一解析器 yaml.safe_load）。

    PyYAML 是唯一环境前提（verify-deploy.sh 自身也依赖它）——缺库 / 解析失败 = infra
    红，不静默跳过（冻结即冒烟纪律：收集通道 error 为零，断言面不缩）。"""
    try:
        import yaml  # noqa: PLC0415  (lazy: 不让既有 V7/PN/RF 齿被可选依赖连坐)
    except ImportError as e:  # pragma: no cover - 环境前提缺失
        testcase.fail(
            f"PV manifest RED(infra): PyYAML 不在位——manifest 静态解析前置缺失"
            f"（verify-deploy.sh 同一依赖）: {e!r}")
    if not PV11_MANIFEST.is_file():
        testcase.fail(f"PV manifest RED: {PV11_MANIFEST} 不在场。")
    try:
        doc = yaml.safe_load(PV11_MANIFEST.read_text(encoding="utf-8"))
    except Exception as e:  # noqa: BLE001 — 解析错误是 RED，不是 crash
        testcase.fail(
            f"PV manifest RED: manifest.yaml 解析失败（verify-deploy.sh 会 exit 2）: {e!r}")
    entries = doc.get("entries") if isinstance(doc, dict) else None
    testcase.assertTrue(
        isinstance(entries, list) and entries,
        "PV manifest RED: manifest.yaml 缺 entries 列表（单一事实源被破坏）。")
    return entries


def _pv_parse_version(name: str) -> int:
    m = PV11_PROMPT_FILE_RE.search(name or "")
    return int(m.group(1)) if m else -1


def _pv_wired_faces_four(testcase):
    """四面结构化解析当前被接线的版本化 prompt 文件名（版本无关，ARQ-260908 纪律）。

    渲染面 = apply-xiaozhi-overlay.sh PROMPT_SRC/PROMPT_DST；声明面 =
    provider.config.yaml system_prompt 三键；载体面 = OPENCLAW_SYSTEM_PROMPT_CARRIER
    声明行；铺放面 = manifest.yaml prompt 条目（deploy 输入副本 + data 渲染副本）。
    返回 (rendered, declared, carried, manifested)，各为该面解析出的文件名列表。"""
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
            m = PV11_PROMPT_FILE_RE.search(line)
            if m:
                carried.append(m.group(0))
    manifested = []
    for entry in _pv_manifest_entries(testcase):
        if not isinstance(entry, dict):
            continue
        for field in ("src", "dst"):
            m = PV11_PROMPT_FILE_RE.search(str(entry.get(field) or ""))
            if m:
                manifested.append(m.group(0))
    return rendered, declared, carried, manifested


def _pv_carrier_history_region(carrier: str) -> str:
    """carrier 沿革段提取（内容驱动：含「历史版本」的行 → 下一条 `- 声明面` 行为止）。"""
    lines = carrier.splitlines()
    start = next((i for i, l in enumerate(lines) if "历史版本" in l), None)
    if start is None:
        return ""
    end = len(lines)
    for j in range(start + 1, len(lines)):
        if lines[j].startswith("- 声明面"):
            end = j
            break
    return "\n".join(lines[start:end])


def _pv_readme_prompt_section(readme: str) -> str:
    """README prompt 指针节提取（内容驱动：标题含 `system prompt` 的 `## ` 节全文）。"""
    lines = readme.splitlines()
    start = next((i for i, l in enumerate(lines)
                  if l.startswith("## ") and "system prompt" in l), None)
    if start is None:
        return ""
    end = len(lines)
    for j in range(start + 1, len(lines)):
        if lines[j].startswith("## "):
            end = j
            break
    return "\n".join(lines[start:end])


class TestPromptV11ScheduleRecall(unittest.TestCase):
    """261017-prompt-v11-schedule-recall（offline，静态锚齿，additive 补齿）。

    PV-A8（live-manual 观测项——**非自动断言**，structural 豁免，本类不落测试）：
      ECS 部署由 human 在场执行后，human 持板验收——① 问「后天都有什么课程？」→
      OpenClaw 审计出现 read 工具动作、答复与 kidpal/schedule.md 周一一行一致
      （语文/数学/足球/科学/班会/语文/足球），无英语/美术；② loopback 探针同口径
      复验；③ `openclaw skills list --agent kidpal` ready = 3
      （kidpal-schedule-answer / weather / tavily）；④ 人为触发一次引擎出错兜底，
      确认播报不含「小智」。非确定性来源：模型「该读未读」是概率性的、工具调用与
      TTS 播报 offline 不可机判（「prompt 写明 ≠ LLM 遵循」M3K3 / PN-A5 / RF-A5
      同款行为等价假设）。观测 = OpenClaw 审计 + 引擎日志双侧留痕，human 真机轮
      目击判定；本 Pod 不自行部署。"""

    maxDiff = None

    @classmethod
    def setUpClass(cls):
        cls.v11_path = DEPLOY_DIR / PV11_NAME
        cls.v11 = _read_text(cls.v11_path)
        cls.v10_path = DEPLOY_DIR / V10_NAME
        cls.v10 = _read_text(cls.v10_path)

    # -- PV-A1 落盘 + 无前言 + 课表行扩列 + 差分收口 ---------------------------

    def test_pv_a1_v11_prompt_file_lands(self):
        """coverage: PV-A1 — v11 落盘非空（版本化演进，历史版本共存可 diff）。"""
        self.assertTrue(
            self.v11_path.is_file(),
            f"PV-A1 RED: {PV11_NAME} 未落盘——v11 版本化 bump 未发生，课表问答回合"
            "识别特征扩列（TECH-261017 后天课表零工具调用根因）无载体（E4 族）。",
        )
        self.assertGreater(
            len(self.v11.strip()), 0,
            "PV-A1 RED: v11 文件为空。",
        )

    def test_pv_a1_v11_no_version_preamble(self):
        """coverage: PV-A1 — 制品本体无版本沿革前言：首行即 `## 角色`（RF-A1 同形态）。
        沿革只允许住 OPENCLAW_SYSTEM_PROMPT_CARRIER（260925 起的版本化纪律，bump
        不得回潮）。"""
        self.assertTrue(
            self.v11_path.is_file(), "PV-A1 RED: v11 未落盘（前置）。")
        first = self.v11.splitlines()[0].strip() if self.v11.splitlines() else ""
        self.assertEqual(
            first, "## 角色",
            "PV-A1 RED: v11 首行不是 `## 角色`——版本沿革前言回潮或角色节被移位"
            "（沿革只允许住 OPENCLAW_SYSTEM_PROMPT_CARRIER）。",
        )
        self.assertFalse(
            self.v11.startswith("#") and not self.v11.startswith("## "),
            "PV-A1 RED: v11 顶层出现 H1 前言标题（前言回潮）。",
        )

    def _assert_schedule_row_expansion(self, body: str, label: str):
        """PV-A1 扩列锚组（内容驱动选行、并集半同义，不钉具体措辞）：
        ① 既有读文件纪律保持（先读 / schedule.md / 读不到诚实说查不到）；
        ② 「只要提到 → 一律判课表问答回合」全称量词；
        ③ 时间面扩列（后天 + 大后天 + 周X/星期X + 覆盖 ≥3/6）；
        ④ 主题面扩列（课程 + 覆盖 ≥5/6）。"""
        rows = _schedule_qa_rows(body)
        self.assertTrue(
            rows,
            f"PV-A1 RED({label}): 回合路由面无「课表问答回合」表行——回合类型路由表"
            "结构被破坏（V7-A1 既有锚面回退）。",
        )
        row = "\n".join(rows)
        # ① 既有纪律保持（扩列不得替换成「直接凭记忆答」）。
        self.assertIn(
            "先读", row,
            f"PV-A1 RED({label}): 课表问答回合行无「先读」锚——先读真课表再作答的"
            "既有纪律被删。")
        self.assertIn(
            "schedule.md", row,
            f"PV-A1 RED({label}): 课表问答回合行无 schedule.md 路径锚——真实课表来源"
            "声明缺失。")
        self.assertTrue(
            "读不到" in row or "查不到" in row,
            f"PV-A1 RED({label}): 课表问答回合行无「读不到/查不到」降级路径锚——"
            "读不到诚实说查不到的纪律半缺失。")
        self.assertTrue(
            any(w in row for w in ("诚实", "如实", "不知道")),
            f"PV-A1 RED({label}): 课表问答回合行降级路径无诚实语义锚（诚实/如实/"
            "不知道任一）。")
        # ② 全称量词半（human 2026-10-17 裁「只要提到课表就去读」）。
        self.assertTrue(
            any(w in row for w in PV11_UNIVERSAL_ANCHORS),
            f"PV-A1 RED({label}): 课表问答回合行无「凡提到 / 只要提到」类全称量词——"
            "识别特征仍是枚举式（TECH-261017 根因：枚举漏词即落对话回合兜底）。")
        # ③ 时间面扩列（后天 / 大后天 = 真机触发词；周X / 星期X = 任意具体星期）。
        self.assertIn(
            "后天", row,
            f"PV-A1 RED({label}): 课表问答回合行识别特征缺「后天」——TECH-261017 "
            "真机触发词未命中即落对话回合兜底（零工具调用 + 凭印象编造）。")
        self.assertIn(
            "大后天", row,
            f"PV-A1 RED({label}): 课表问答回合行识别特征缺「大后天」——相对日表达"
            "只扩到后天不完整（plan.md §A 扩列清单）。")
        self.assertTrue(
            PV11_WEEKDAY_RE.search(row) or PV11_XINGQI_RE.search(row),
            f"PV-A1 RED({label}): 课表问答回合行识别特征缺「周X / 星期X」模式——"
            "任意具体星期表达未覆盖（plan.md §A 扩列清单）。")
        time_hits = [w for w in PV11_SCHEDULE_TIME_ANCHORS if w in row]
        self.assertGreaterEqual(
            len(time_hits), 3,
            f"PV-A1 RED({label}): 课表问答回合行时间面覆盖不足——命中 {time_hits}"
            "（须 ≥3/6：今天/明天/周几/这周/下周/号）。")
        # ④ 主题面扩列。
        subject_hits = [w for w in PV11_SCHEDULE_SUBJECT_ANCHORS if w in row]
        self.assertIn(
            "课程", subject_hits,
            f"PV-A1 RED({label}): 课表问答回合行识别特征缺「课程」——主题面扩列未落"
            "（plan.md §A：凡提到课表、课程、上课…）。")
        self.assertGreaterEqual(
            len(subject_hits), 5,
            f"PV-A1 RED({label}): 课表问答回合行主题面覆盖不足——命中 {subject_hits}"
            "（须 ≥5/6：课表/课程/上课/上什么课/几点上课/要带什么）。")

    def test_pv_a1_v11_schedule_row_expanded_recognition(self):
        """coverage: PV-A1 — v11 课表问答回合行识别特征扩列锚齐（并集容换写），
        且纪律句仍在同表行内。"""
        self.assertTrue(
            self.v11_path.is_file(), "PV-A1 RED: v11 未落盘（前置）。")
        self._assert_schedule_row_expansion(self.v11, f"{PV11_NAME} 文件")

    def test_pv_a1_v11_diff_confined_to_schedule_row(self):
        """coverage: PV-A1 — v11 = v10 全文仅「课表问答回合」识别特征列扩列：
        两版逐行比对，差异只允许落在课表问答回合表行（其余段落零偏移）。"""
        self.assertTrue(
            self.v11_path.is_file(), "PV-A1 RED: v11 未落盘（前置）。")
        self.assertTrue(
            self.v10.strip(), "PV-A1 RED(infra): v10 不在场/为空——差分基线缺失。")
        offenders = []
        for lineno, (old, new) in enumerate(
                zip_longest(self.v10.splitlines(), self.v11.splitlines(),
                            fillvalue=None), start=1):
            if old == new:
                continue
            if "课表问答回合" in (old or "") and "课表问答回合" in (new or ""):
                continue
            offenders.append((lineno, old, new))
        self.assertEqual(
            offenders, [],
            "PV-A1 RED: v11 相对 v10 的差异越出「课表问答回合」表行——版本化 bump "
            "只许扩课表识别特征列、其余段落零偏移（plan.md §A）。首三处："
            f"{offenders[:3]!r}",
        )

    # -- PV-A2 v10 历史版本零回改（RF-A3 形态复刻，must-stay-green 负锚） ------

    def test_pv_a2_v10_prompt_file_immutable(self):
        """coverage: PV-A2 — v10 本体零回改（历史不可变铁律；v1..v9 由既有各包齿钉）。
        v10 冻结时点 sha256 = 963b2b79…（2026-10-18 PV 批冻结，LF 归一口径 =
        canonical blob 口径）。"""
        self.assertTrue(
            self.v10_path.is_file(),
            "PV-A2 RED: v10 文件被删除——历史版本共存可 diff 铁律被破坏。",
        )
        self.assertEqual(
            _sha256_lf(self.v10_path), PV11_V10_SHA256_LF,
            "PV-A2 RED: v10 本体被回改——bump 必须新落 .v11 文件、历史版本一律不改"
            "（版本化落盘惯例；如确需改动属治理裁决，须 human ruling 后更新本钉）。",
        )

    # -- PV-A3 四面接线一致 + 接线版承载扩列锚 + carrier 沿革（版本无关形态） ----

    def test_pv_a3_four_faces_wired_consistent(self):
        """coverage: PV-A3 — 四面接线一致且 manifest prompt 条目齐（deploy 输入
        副本 + data 渲染副本）。

        版本无关形态（ARQ-260908 纪律，接线类断言零版本字面）：从渲染面 /
        声明面 / 载体面 / manifest 四面各自解析当前版，断言四面集合相等且该版
        落盘非空——「验收所测 = 生产所用」；bump 同批翻即绿、漏翻必红。"""
        rendered, declared, carried, manifested = _pv_wired_faces_four(self)
        self.assertTrue(
            rendered and declared and carried and manifested,
            f"PV-A3 RED: 接线面解析失败（rendered={rendered} declared={declared} "
            f"carried={carried} manifested={manifested}）——prompt 落盘即孤儿"
            "（PROMPT_SRC/PROMPT_DST、provider.config.yaml system_prompt 三键、"
            "OPENCLAW_SYSTEM_PROMPT_CARRIER 声明行、manifest.yaml prompt 条目须"
            "指向 openclaw-system-prompt.vN.md）。",
        )
        self.assertEqual(
            set(rendered), set(declared),
            f"PV-A3 RED: 渲染面 {sorted(rendered)} 与声明面 {sorted(declared)} 指向"
            "不同版本——「验收所测 = 生产所用」断裂（ADR-001 接线不变式，bump "
            "须同批）。",
        )
        self.assertEqual(
            set(rendered), set(carried),
            f"PV-A3 RED: 载体面（OPENCLAW_SYSTEM_PROMPT_CARRIER）声明 "
            f"{sorted(carried)} 与渲染/声明两面 {sorted(rendered)} 不一致——"
            "carrier 声明未随 bump 同批。",
        )
        self.assertEqual(
            set(rendered), set(manifested),
            f"PV-A3 RED: manifest.yaml prompt 条目 {sorted(manifested)} 与渲染/声明"
            f"/载体三面 {sorted(rendered)} 不一致——清单仍指旧版，verify-deploy.sh "
            "会对账失败（261013 单一事实源纪律）。",
        )
        name = sorted(set(rendered))[0]
        path = DEPLOY_DIR / name
        self.assertTrue(
            path.is_file(), f"PV-A3 RED: 被接线的 {name} 未落盘（孤儿声明）。")
        self.assertTrue(
            _read_text(path).strip(), f"PV-A3 RED: 被接线的 {name} 是空文件。")
        # manifest 两条 prompt 条目：deploy 输入副本 + data 渲染副本。
        entries = _pv_manifest_entries(self)
        prompt_entries = [
            e for e in entries
            if isinstance(e, dict) and any(
                PV11_PROMPT_FILE_RE.search(str(e.get(f) or "")) for f in ("src", "dst"))]
        self.assertGreaterEqual(
            len(prompt_entries), 2,
            "PV-A3 RED: manifest prompt 条目不足两条——须含 deploy 输入副本与 data "
            f"渲染副本（实得 {len(prompt_entries)} 条）。")
        dsts = [str(e.get("dst") or "") for e in prompt_entries]
        self.assertTrue(
            any("/xiaozhi-esp32-server/deploy/" in d for d in dsts),
            f"PV-A3 RED: manifest 无 deploy 输入副本条目（dst 列表 {dsts}）——"
            "overlay 渲染源的铺放面缺失。")
        self.assertTrue(
            any("xiaozhi-docker-data" in d for d in dsts),
            f"PV-A3 RED: manifest 无 data 渲染副本条目（dst 列表 {dsts}）——"
            "overlay 渲染产物无核验面（渲染即失对账）。")

    def test_pv_a3_carrier_history_appends_all_prior_versions(self):
        """coverage: PV-A3 — carrier 沿革 append 一行且历史行不删：既存历史版本
        `.vK.md` 文件（K < 当前接线版）须全部在「历史版本」沿革段被点名。"""
        rendered, _declared, _carried, _manifested = _pv_wired_faces_four(self)
        self.assertTrue(rendered, "PV-A3 RED: 无接线 prompt 版本可解析。")
        current = _pv_parse_version(sorted(set(rendered))[0])
        self.assertGreater(current, 0, "PV-A3 RED: 接线 prompt 版本号解析失败。")
        carrier = _read_text(DEPLOY_DIR / "OPENCLAW_SYSTEM_PROMPT_CARRIER")
        self.assertTrue(
            carrier.strip(),
            "PV-A3 RED: OPENCLAW_SYSTEM_PROMPT_CARRIER 不在场——沿革唯一全量副本缺失。")
        history = _pv_carrier_history_region(carrier)
        self.assertTrue(
            history,
            "PV-A3 RED: carrier 无「历史版本」沿革段——沿革唯一全量副本结构被破坏。")
        missing = [
            f"openclaw-system-prompt.v{k}.md" for k in range(1, current)
            if (DEPLOY_DIR / f"openclaw-system-prompt.v{k}.md").is_file()
            and f"openclaw-system-prompt.v{k}.md" not in history
        ]
        self.assertEqual(
            missing, [],
            "PV-A3 RED: carrier 沿革漏记既存历史版本 "
            f"{missing}——bump 须 append 一行（历史行不删），沿革是版本演进史唯一"
            "全量副本（260925 起的版本化纪律）。",
        )

    def test_pv_a3_wired_version_carries_expanded_recognition(self):
        """coverage: PV-A3 — 接线版正文携带 PV-A1 扩列锚（红→绿半：现接线 v10 无
        后天/大后天 = 设计态红；bump v11 + 四面同批后转绿；未来 bump 丢锚同样红，
        防丢内容）。"""
        rendered, _declared, _carried, _manifested = _pv_wired_faces_four(self)
        self.assertTrue(rendered, "PV-A3 RED: 无接线 prompt 版本可解析。")
        name = sorted(set(rendered))[0]
        body = _read_text(DEPLOY_DIR / name)
        self.assertTrue(body.strip(), f"PV-A3 RED: 被接线的 {name} 不在场/为空。")
        self._assert_schedule_row_expansion(body, f"接线版 {name}")

    # -- PV-A4 KidPal 极简模板（三段位 + 残留禁令） -----------------------------

    def _template_body(self) -> str:
        path = DEPLOY_DIR / PV11_TEMPLATE_NAME
        self.assertTrue(
            path.is_file(),
            f"PV-A4 RED: {PV11_TEMPLATE_NAME} 未落盘——通用小智模板与 v11 对冲"
            "（工具不匹配就直答 / 禁 ``` 段）无替换件（TECH-261017 第二根因）。")
        body = _read_text(path)
        self.assertGreater(len(body.strip()), 0, "PV-A4 RED: 模板文件为空。")
        return body

    def test_pv_a4_kidpal_template_identity_and_context(self):
        """coverage: PV-A4 — 模板含 `<identity>` 位（承接 build_enhanced_prompt 以
        config["prompt"] 注入的 base_prompt）与 `<context>` 位（日期/星期/农历/
        设备位置/天气事实注入位——课表「后天星期几」的事实来源必须保留）+
        中文输出约束一句。"""
        body = self._template_body()
        self.assertIn(
            "<identity>", body,
            "PV-A4 RED: 模板缺 <identity> 位——v10 入口（config[\"prompt\"] 作 "
            "base_prompt）无处承接，prompt 与 v11 脱钩。")
        self.assertIn(
            "</identity>", body, "PV-A4 RED: 模板 <identity> 位未闭合。")
        self.assertIn(
            "{{base_prompt}}", body,
            "PV-A4 RED: 模板 <identity> 未以 {{base_prompt}} 承接引擎注入的 "
            "base_prompt（plan.md §B：<identity>{{base_prompt}}</identity>）。")
        self.assertIn("<context>", body, "PV-A4 RED: 模板缺 <context> 位。")
        self.assertIn("</context>", body, "PV-A4 RED: 模板 <context> 位未闭合。")
        # 事实注入位（并集容换写：英文占位名或中文表述任一）。
        fact_groups = {
            "今天日期": ("today_date", "今天日期", "日期"),
            "星期": ("today_weekday", "星期", "weekday"),
            "农历": ("农历", "lunar"),
            "设备位置": ("位置", "location", "城市"),
            "天气": ("天气", "weather"),
        }
        missing = [name for name, anchors in fact_groups.items()
                   if not any(a in body for a in anchors)]
        self.assertEqual(
            missing, [],
            "PV-A4 RED: 模板 <context> 缺事实注入位 "
            f"{missing}——课表「后天星期几」与实时事实回合的事实来源必须在位"
            "（plan.md §B：<context> 今天日期、星期、农历、设备位置、天气必须留）。")
        self.assertIn(
            "中文", body,
            "PV-A4 RED: 模板缺中文输出约束（plan.md §B 三段位之三）。")
        self.assertTrue(
            any(w in body for w in ("回答", "输出", "回复", "作答", "说话")),
            "PV-A4 RED: 模板「中文输出约束」缺输出动词半（回答/输出/回复/作答/"
            "说话任一）——约束句不成立。")

    def test_pv_a4_kidpal_template_free_of_xiaozhi_residue(self):
        """coverage: PV-A4 — 模板不含通用小智模板残留（play_music / hass /
        handle_exit_intent / speaker_recognition / anti_ai_smell / 空 <memory> /
        禁 ``` 段），且不写 `{{current_time}}`（引擎传字面量，写了会留字面串）。"""
        body = self._template_body()
        lower = body.lower()
        for token in ("play_music", "handle_exit_intent", "speaker_recognition",
                      "anti_ai_smell", "tts_format_constraints"):
            self.assertNotIn(
                token, lower,
                f"PV-A4 RED: 模板残留通用小智模板段「{token}」——与 v11 对冲"
                "（plan.md §B 删除清单）。")
        self.assertIsNone(
            re.search(r"\bhass\b", lower),
            "PV-A4 RED: 模板残留通用小智模板 hass 智能家居语义（plan.md §B 删除清单）。")
        self.assertNotIn(
            "<memory", lower,
            "PV-A4 RED: 模板残留 <memory> 段（含空 <memory>）——plan.md §B 删除清单。")
        self.assertNotIn(
            "{{current_time}}", body,
            "PV-A4 RED: 模板写了 {{current_time}}——build_enhanced_prompt 传字面量，"
            "会留字面串在 prompt 里（plan.md §B 已知引擎缺陷规避）。")
        fence_offenders = [
            line for line in body.splitlines()
            if "```" in line and (
                "代码块" in line or "code block" in line.lower()
                or any(w in line for w in ("禁止", "严禁", "不得", "不要", "不能", "不可")))
        ]
        self.assertEqual(
            fence_offenders, [],
            "PV-A4 RED: 模板残留「禁 ``` 段」（tts_format_constraints 条款）——"
            "v11 的末尾 ```display 块纪律会被这条对冲。实得行："
            f"{fence_offenders[:2]!r}")

    # -- PV-A7 manifest 条目齐 + 静态可解析 + README 指针 -----------------------

    def test_pv_a7_manifest_entries_cover_template_and_skills(self):
        """coverage: PV-A7 — manifest 条目齐：kidpal-agent-prompt.txt 一条 +
        261017-openclaw-kidpal-skills 声明工件条目一条（prompt v11 两条见 PV-A3）。"""
        entries = _pv_manifest_entries(self)

        def _mentions(needle: str) -> list[dict]:
            return [
                e for e in entries
                if isinstance(e, dict) and needle in (
                    str(e.get("src") or "") + "\n" + str(e.get("dst") or ""))]

        self.assertTrue(
            _mentions(PV11_TEMPLATE_NAME),
            f"PV-A7 RED: manifest 缺 {PV11_TEMPLATE_NAME} 条目——KidPal 极简模板"
            "未入单一事实源（live 铺放/核验无面）。")
        self.assertTrue(
            _mentions(PV11_SKILLS_DIRNAME),
            f"PV-A7 RED: manifest 缺 {PV11_SKILLS_DIRNAME} 声明工件条目——skills "
            "收敛声明未入单一事实源。")

    def test_pv_a7_manifest_entries_statically_parseable(self):
        """coverage: PV-A7 — entries 可被 verify-deploy.sh 静态解析（等价校验：
        每条 dst 在场；md5 条 src 在仓内存在；marker 条 anchors 非空）。"""
        entries = _pv_manifest_entries(self)
        problems = []
        for i, entry in enumerate(entries):
            if not isinstance(entry, dict):
                problems.append(f"entries[{i}] 不是 object")
                continue
            dst = entry.get("dst")
            if not dst:
                problems.append(f"entries[{i}] 缺 dst")
            expect = entry.get("verify")
            if expect == "marker":
                if not entry.get("anchors"):
                    problems.append(f"entries[{i}] verify=marker 但 anchors 空（dst={dst}）")
            else:
                src = entry.get("src")
                if not src:
                    problems.append(f"entries[{i}] verify={expect!r} 但缺 src（dst={dst}）")
                elif not (REPO_ROOT / str(src)).is_file():
                    problems.append(f"entries[{i}] 仓内源缺失: {src}")
        self.assertEqual(
            problems, [],
            "PV-A7 RED: manifest entries 静态解析问题（verify-deploy.sh 会判漂移/"
            "配置错误）：\n  - " + "\n  - ".join(problems))

    def test_pv_a7_readme_pointer_follows_wired_version(self):
        """coverage: PV-A7 — server/deploy/README.md prompt 指针节刷当前接线版，
        且不复述沿革（沿革唯一全量副本 = carrier）。"""
        rendered, _declared, _carried, _manifested = _pv_wired_faces_four(self)
        self.assertTrue(rendered, "PV-A7 RED: 无接线 prompt 版本可解析（指针目标缺失）。")
        current = sorted(set(rendered))[0]
        readme = _read_text(PV11_README)
        self.assertTrue(readme.strip(), "PV-A7 RED: server/deploy/README.md 不在场。")
        section = _pv_readme_prompt_section(readme)
        self.assertTrue(
            section,
            "PV-A7 RED: README 无 prompt 指针节（标题含「system prompt」的 `## ` 节"
            "缺失）——指针节是运维面唯一分散入口。")
        self.assertIn(
            current, section,
            f"PV-A7 RED: README prompt 指针节未指向当前接线版 {current}——"
            "bump 后仍指旧版 = 散文漂移（arch §5.5.5 :360 先例债）。")
        stale = [
            name for name in re.findall(
                r"(?:当前|现状)[^\n]{0,20}?(openclaw-system-prompt\.v\d+\.md)", section)
            if name != current
        ]
        self.assertEqual(
            stale, [],
            f"PV-A7 RED: README 指针节仍以「当前/现状」口径写旧版 {stale}——"
            f"当前接线版为 {current}。")
        lineage = [
            line for line in section.splitlines()
            if len(set(re.findall(r"openclaw-system-prompt\.v\d+\.md", line))) >= 2
        ]
        self.assertEqual(
            lineage, [],
            "PV-A7 RED: README 指针节复述沿革（单行并列多个版本文件）——沿革只允许"
            "住 OPENCLAW_SYSTEM_PROMPT_CARRIER。实得行："
            f"{lineage[:2]!r}")


if __name__ == "__main__":
    unittest.main(verbosity=2)
