"""Frozen acceptance tests — 261023-agents-md-static-semantics-alignment（A1/A2/A3 host 面）
(subsystem: server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md 静态语义单一事实源
+ server/deploy/openclaw-skills/workspace-kidpal/kidpal-schedule-answer/SKILL.md（B1 同口径）；
TaskPackage = delivery/taskpackages/261023-agents-md-static-semantics-alignment/taskpackage.json；
技术权威 = 同目录 plan.md（断言种子 §4 / 红基线判法 §10 / 冻结锚保护 §3 / 禁入 §11）；
上游 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/
agents-md-advisory-20261023.md 批 A+B（B1/B2/B3/S1/S3/S4/S5/S6/N1–N6）+ human 2026-10-23
立项 ack + architect-reviewer 四分类 verdict = NO_CONTRACT_CHANGE（S3 甲案，A3 第④半在场）)。

背景：workspace-kidpal/AGENTS.md 是 v11 的忠实迁移，但此后三件事没回改对应句子——
10-19/10-20 记录会话改对话引导态（标题/正文改由 front 整理回合产出）、10-22 P5 课表
机械注入（课表摘要经内部帧进动态 context）、10-23 K3 记忆上线（v12 守则）。当前不是
「少了什么」，而是三处仍在强制已不存在的旧行为（课表强制先读文件 / 故事旧整理管道 /
提醒整节以本工作区为执行主体），且 v12 与两条底线的接缝未处理。本包把静态 prompt 拉
回已冻结的 spec（arch §5.2 / requirements S-F2.2 AC4 / S-F1.3 AC4），零代码、零契约、
零 manifest、零动态 context 模板、零 tools 声明。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O；断言逐条对应 plan.md §4 断言种子 A1/A2/A3）

  A1 · B1 课表问答「摘要优先 + 降级读文件」新口径，两文件同口径（test_a1_…）：
    a) AGENTS.md 回合路由表：含「课表问答回合」的行同时含「课表摘要」与
       「不必再读文件」（识别特征列逐字不动 = K1/PV-A1 冻结锚零触，本件不重钉）。
    b) 同一行含占位降级词「（本次会话未提供）」与「截断」（三触发降级句的两半：
       窗口外由「摘要没覆盖的日期」承载；占位与截断必须同在，否则截断日会被当
       「没有安排」——plan §2.1(3)）。
    c) AGENTS.md 全文零「课表答案必须来自本回合实际读到的内容」（旧唯一来源句
       撤净——摘要与文件同源派生、等同本回合读到的事实）。
    d) 底线节窗口含「课表摘要」（第一条不变量同步：内容来自摘要或本回合读到的
       schedule.md，plan §2.1(14)）。
    e) SKILL.md 同口径：含「课表摘要」+「覆盖的日期直接用摘要作答」+
       「本次会话未提供」。
    f) SKILL.md 零「先读文件再作答，绝不凭印象」（旧口径撤净，反编造覆盖句换
       「摘要等同事实」新措辞——plan §2.2(3)；prompt↔skill 双件同批是铁律 2）。

  A2 · B2 故事节旧整理管道撤净（test_a2_…）：
    故事节窗口（标题含「故事」的 `## ` 节 ∪ 含 `story` 的行）内：
    a) 零命中旧管道四锚：「下一个回合」「重录一次」「刚才那段是故事」
       「逐字来自整段转写」（261019 前管道；现行链路 = front 一次性整理回合，
       arch §5.2④ + S-F2.2 AC4 同口径）。
    b) 在场新口径四锚：「你不产标题」「整理」「不是逐字转写」「12 字」（最后者
       必须是阿拉伯数字 + 空格字面——M4K3 冻结锚「12 字」依赖，禁写「十二字」，
       plan §3 M4K3 行）。

  A3 · S1 提醒单源指针 + B3/S6 记忆负向 + S3 对象锁定澄清 + S5 版本残留
       （test_a3_…）：
    a) 提醒节窗口（标题含「提醒」的 `## ` 节）含 `kidpal-reminder-semantic`
       （字段级纪律单一事实源指向 main agent skill——plan §2.1(11)）；窗口内零
       「≤60字」「≤90字」（字段级细节已外移，双源漂移消除）；全文零
       `KIDPAL_SEMANTIC_JSON`（env 绑定在 main 侧 cron 命令串，本 agent 进程
       未必有——S8 随之消解）。
    b) 记忆节窗口含「不需要写任何文件」「不调用任何工具」「记不清」「绝不编造」
       （B3：记忆平台自动承载 + 无画像时诚实说记不清 + 编造记忆负向——plan
       §2.1(17)；§6.7 降级语义下模型不再被要求「答得上来」）。
    c) 全文零「（v12）」零「旧守则」（S5：版本号与 rationale 是仓维注记，对模型
       零操作语义——plan §2.1(16)(18)；K3 冻结 heading 正则 ^##\\s+[^\\n]*记忆 与
       节内 needles（孩子世界的事实/宠物/同学/家长隐私/家庭安排）由
       test_261022_honcho_memory.acceptance.py 继续承载，本件零触不重钉）。
    d) 含「对象锁定」的行同时含 K3 冻结锚串逐字「不引用、不复述任何家长隐私
       （姓名、联系方式、家庭安排细节）」（含右括号，test_261022_honcho_memory
       :229 子串断言依赖；锚串本体零改、只在其后用「——」追加）+「孩子自己的
       日程信息」+「不算复述家长隐私」（S3 甲案：接送/携带/老师称呼类备注按
       S-F1.3 AC4 原词读回，划出「家长隐私」范畴；plan §2.1(13) + §6 甲案裁决）。

===========================================================================
强度申报（契约条目 / 决策 → 承载断言 → 性质档）：

  · B1（arch §5.2 摘要优先口径）      → test_a1_schedule_summary_first_with_fallback_same_wording → 源锚代理
    （本包交付物就是 prompt 措辞本身；「prompt 写明 ≠ LLM 遵循」行为等价假设
    显式化——遵循度归 human 部署轮真机/live seeded 探针目击，非本件自动断言对象）
  · B2（arch §5.2④ + S-F2.2 AC4）    → test_a2_story_organize_pipeline_no_legacy_residue → 源锚代理
  · S1 单源指针（manifest cron 归 main）→ test_a3_reminder_single_source_and_memory_negatives ① → 源锚代理
  · B3/S6 记忆负向（§6.7 降级语义）   → 同上 ② → 源锚代理
  · S5 版本残留（仓维注记零操作语义） → 同上 ③ → 源锚代理
  · S3 甲案（四分类 NO_CONTRACT_CHANGE）→ 同上 ④ → 源锚代理

缺口（强度低于 spec 要求处，逐条）：
  · LLM 行为遵循度（摘要直答 / 不编造记忆 / 备注原词读回）offline 不可机判，
    归 human 部署轮真机轮与 live seeded 探针（plan §6 附带风险注记随行）——
    offline 侧无驱动方 = 已申报盲区，行为 seam（静态语义 → LLM 行为）的驱动方
    = human live 轮。
  · 「课表答案必须来自本回合实际读到的内容」负锚只钉 plan §4 指定全句；若
    builder 以同义换写残留旧语义（非全句），本件不红——由 A1 ①/④ 正锚 +
    live 探针兜底。

fixture 自含性声明：全部断言直读 repo 内两份文本制品（路径由 __file__ 显式推导
仓根，不依赖调用点 cwd——root 注入纪律）；无外部资产 / 时间真值 / 环境前提依赖
（纯 str 断言，零第三方库）；行为 seam 驱动方缺口见上。依赖 AGENTS.md 与
SKILL.md 在场非空 = infra 红、不静默跳过。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" acceptance/test_261023_agents_md_static_semantics_alignment.acceptance.py
  （standalone: python 本文件）

RED 基线（冻结时点树，2026-10-23，builder 未动工，实跑实测）：本件 3/3 设计态红
——A1 红：路由行仍「先读 schedule.md」旧口径（无「课表摘要」「不必再读文件」）、
SKILL.md 旧「先读文件再作答」在场；A2 红：旧管道句「下一个回合」「重录一次」
「刚才那段是故事」「逐字来自整段转写」在场、「你不产标题」缺位；A3 红：提醒节
无 kidpal-reminder-semantic 单源指针、KIDPAL_SEMANTIC_JSON / ≤60字 / ≤90字 在场、
「（v12）」「旧守则」在场、对象锁定行无 S3 澄清追加。唯一允许的红 = 本件自身
3 条（builder 落 plan §2 文本后转绿）；既有冻结件零新增红判法 = plan §10 三条
（六件读数件 101 passed / 4 deselected + 全量 313 passed / 6 deselected / 33
subtests 零 failure + git diff 不触 server/acceptance/ 既有件与 deploy 四承载件）。
"""

from __future__ import annotations

import re
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]

AGENTS_MD = REPO_ROOT / "server" / "deploy" / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md"
SKILL_MD = (
    REPO_ROOT / "server" / "deploy" / "openclaw-skills" / "workspace-kidpal"
    / "kidpal-schedule-answer" / "SKILL.md"
)

# A1 · B1 课表问答锚（plan §4 A1 ①②③④⑤⑥ 逐字）。
A1_ROUND_ROW_KEY = "课表问答回合"          # 回合路由表行定位键（识别特征列零触）
A1_ROW_POSITIVES = ("课表摘要", "不必再读文件")          # ① 摘要优先
A1_ROW_DEGRADES = ("（本次会话未提供）", "截断")          # ② 降级三触发之二
A1_OLD_SOURCE_SENTENCE = "课表答案必须来自本回合实际读到的内容"  # ③ 旧唯一来源句
A1_BASELINE_HEADING = "底线"                            # ④ 底线节窗口定位
A1_SKILL_POSITIVES = ("课表摘要", "覆盖的日期直接用摘要作答", "本次会话未提供")  # ⑤
A1_SKILL_NEGATIVE = "先读文件再作答，绝不凭印象"          # ⑥ 旧 skill 口径

# A2 · B2 故事节旧管道撤净（plan §4 A2 逐字）。
A2_SECTION_HEADING = "故事"
A2_NEGATIVES = ("下一个回合", "重录一次", "刚才那段是故事", "逐字来自整段转写")
A2_POSITIVES = ("你不产标题", "整理", "不是逐字转写", "12 字")

# A3 · S1 单源 + B3/S6 记忆负向 + S5 残留 + S3 澄清（plan §4 A3 ①②③④ 逐字）。
A3_REMINDER_HEADING = "提醒"
A3_REMINDER_SOURCE = "kidpal-reminder-semantic"
A3_REMINDER_FIELD_DETAIL_NEGATIVES = ("≤60字", "≤90字")
A3_ENV_NEGATIVE = "KIDPAL_SEMANTIC_JSON"
A3_MEMORY_HEADING = "记忆"
A3_MEMORY_POSITIVES = ("不需要写任何文件", "不调用任何工具", "记不清", "绝不编造")
A3_VERSION_NEGATIVES = ("（v12）", "旧守则")
A3_OBJECT_LOCK_KEY = "对象锁定"
# K3 冻结锚串逐字（含右括号；test_261022_honcho_memory K3_P1_AGENTS_ANCHORS 依赖
# 其逐字在场——本件自含副本，跨冻结件 import 会把冻结件焊死，故复制不引用）。
A3_K3_PRIVACY_ANCHOR = "不引用、不复述任何家长隐私（姓名、联系方式、家庭安排细节）"
A3_S3_CLARIFIES = ("孩子自己的日程信息", "不算复述家长隐私")


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _section_text(testcase: unittest.TestCase, src: str, heading_needle: str, tag: str) -> str:
    """取「标题含 heading_needle 的 `## ` 节」窗口：自该标题行起、至下一个 `## `
    标题行或 EOF。节不存在 = 设计态红（不是 skip）。"""
    lines = src.splitlines()
    start = None
    for i, line in enumerate(lines):
        if line.startswith("## ") and heading_needle in line:
            start = i
            break
    if start is None:
        testcase.fail(
            f"{tag} RED: AGENTS.md 无标题含 {heading_needle!r} 的 `## ` 节 —— "
            "节窗口定位失败（节被删/改名 = builder 越界，plan §3 冻结锚保护）。")
    rest = lines[start + 1:]
    end = len(rest)
    for j, line in enumerate(rest):
        if line.startswith("## "):
            end = j
            break
    return "\n".join(rest[:end])


class AgentsMdStaticSemanticsAlignmentAcceptance(unittest.TestCase):
    """261023-agents-md-static-semantics-alignment · A1/A2/A3（host 面，offline）。"""

    maxDiff = None

    def _agents_md(self, tag: str) -> str:
        src = _read_text(AGENTS_MD)
        self.assertTrue(
            src.strip(),
            f"{tag} RED: {AGENTS_MD} 不在场/为空 —— 静态语义单一事实源镜像缺失"
            "（infra 红前置，K1 承载反转基座）。")
        return src

    def _skill_md(self, tag: str) -> str:
        src = _read_text(SKILL_MD)
        self.assertTrue(
            src.strip(),
            f"{tag} RED: {SKILL_MD} 不在场/为空 —— B1 双件同批的 skill 侧缺失"
            "（铁律 2：只改一边 = 制造新双源矛盾）。")
        return src

    # ----------------------------------------------------------------- #
    # A1 · B1 课表问答：摘要优先 + 降级读文件，两文件同口径
    # ----------------------------------------------------------------- #

    def test_a1_schedule_summary_first_with_fallback_same_wording(self):
        """coverage: A1, B1, S4 — 课表问答「摘要优先、窗口外/占位/截断降级读文件」
        新口径落 AGENTS.md 回合路由行与底线不变量，SKILL.md 同口径；旧唯一来源句
        与旧 skill 口径撤净。guard-class: wording（prompt 措辞 = 本包交付物）。"""
        tag = "A1"
        agents_src = self._agents_md(tag)
        skill_src = self._skill_md(tag)

        # ① ② 含「课表问答回合」的行同时含「课表摘要」「不必再读文件」+
        #    占位降级词「（本次会话未提供）」与「截断」（识别特征列不在本件断言
        #    论域——K1/PV-A1 冻结锚继续承载）。
        row_lines = [l for l in agents_src.splitlines() if A1_ROUND_ROW_KEY in l]
        self.assertTrue(
            row_lines,
            f"{tag} RED: AGENTS.md 回合路由表无「{A1_ROUND_ROW_KEY}」行 —— B1 "
            "新口径无落点（plan §2.1(3)）。")
        for line in row_lines:
            for needle in A1_ROW_POSITIVES + A1_ROW_DEGRADES:
                self.assertIn(
                    needle, line,
                    f"{tag} RED: 「{A1_ROUND_ROW_KEY}」路由行缺锚 {needle!r} "
                    "—— 摘要优先/降级三触发未落（摘要没覆盖或截断日会被当"
                    "「没有安排」，plan §2.1(3)）。")

        # ③ 全文零旧唯一来源句（摘要与文件同源派生后，旧句与新口径结构性冲突）。
        self.assertNotIn(
            A1_OLD_SOURCE_SENTENCE, agents_src,
            f"{tag} RED: AGENTS.md 仍含旧句 {A1_OLD_SOURCE_SENTENCE!r} —— 旧"
            "「唯一来源」口径未撤，与摘要优先新口径结构性冲突（plan §4 A1③）。")

        # ④ 底线节窗口含「课表摘要」（第一条不变量同步）。
        baseline_section = _section_text(self, agents_src, A1_BASELINE_HEADING, tag)
        self.assertIn(
            "课表摘要", baseline_section,
            f"{tag} RED: 底线节窗口缺「课表摘要」—— 第一不变量仍只认本回合读到"
            "的 schedule.md，未与 P5 摘要注入对齐（plan §2.1(14)）。")

        # ⑤ SKILL.md 同口径三锚。
        for needle in A1_SKILL_POSITIVES:
            self.assertIn(
                needle, skill_src,
                f"{tag} RED: SKILL.md 缺锚 {needle!r} —— B1 双件同批口径未"
                "落 skill 侧（铁律 2）。")

        # ⑥ SKILL.md 零旧口径句。
        self.assertNotIn(
            A1_SKILL_NEGATIVE, skill_src,
            f"{tag} RED: SKILL.md 仍含旧口径 {A1_SKILL_NEGATIVE!r} —— skill 的"
            "「先读文件再作答」会与 AGENTS.md 摘要优先打架（新双源矛盾）。")

    # ----------------------------------------------------------------- #
    # A2 · B2 故事节：旧整理管道撤净
    # ----------------------------------------------------------------- #

    def test_a2_story_organize_pipeline_no_legacy_residue(self):
        """coverage: A2, B2 — 故事节窗口内旧管道四锚零命中、新口径四锚在场：
        标题/正文/id 全由网关整理回合产出（arch §5.2④ + S-F2.2 AC4），「你不产
        标题」防与 front 整理指令竞争输出形状。guard-class: wording。"""
        tag = "A2"
        agents_src = self._agents_md(tag)

        # 窗口 = 标题含「故事」的 `## ` 节 ∪ 含 `story` 的行（plan §4 A2 定义）。
        section = _section_text(self, agents_src, A2_SECTION_HEADING, tag)
        story_lines = [l for l in agents_src.splitlines() if "story" in l]
        window = section + "\n" + "\n".join(story_lines)

        for negative in A2_NEGATIVES:
            self.assertNotIn(
                negative, window,
                f"{tag} RED: 故事节窗口仍含旧管道锚 {negative!r} —— 261019 "
                "前「下一回合起标题 / 逐字转写 / 重录 / 挽回」旧管道未撤净，"
                "会与 front 整理回合竞争输出形状（plan §2.1(9)(10)）。")

        for positive in A2_POSITIVES:
            self.assertIn(
                positive, window,
                f"{tag} RED: 故事节窗口缺新口径锚 {positive!r} —— 整理管道"
                "归属网关的新口径未落（plan §2.1(9)；「12 字」须为阿拉伯数字"
                " + 空格字面，M4K3 冻结锚依赖）。")

    # ----------------------------------------------------------------- #
    # A3 · S1 提醒单源 + B3/S6 记忆负向 + S5 残留 + S3 对象锁定澄清
    # ----------------------------------------------------------------- #

    def test_a3_reminder_single_source_and_memory_negatives(self):
        """coverage: A3, S1, B3, S5, S6, S3 — 提醒节字段级纪律单源外移（指向
        main agent 的 kidpal-reminder-semantic skill）；记忆节平台自动承载 +
        编造记忆负向；版本号/rationale 仓维残留撤净；对象锁定行 K3 冻结锚串
        逐字保留且其后追加 S3 澄清（甲案，四分类 NO_CONTRACT_CHANGE）。
        guard-class: wording。"""
        tag = "A3"
        agents_src = self._agents_md(tag)

        # ① 提醒节窗口：单源指针在场；字段级细节外移；env 名全文零命中。
        reminder_section = _section_text(self, agents_src, A3_REMINDER_HEADING, tag)
        self.assertIn(
            A3_REMINDER_SOURCE, reminder_section,
            f"{tag} RED: 提醒节缺 {A3_REMINDER_SOURCE!r} 单源指针 —— 字段级纪律"
            "仍在本文件双源漂移（cron 实际归 main agent，plan §2.1(4)(11)）。")
        for negative in A3_REMINDER_FIELD_DETAIL_NEGATIVES:
            self.assertNotIn(
                negative, reminder_section,
                f"{tag} RED: 提醒节仍含字段级细节 {negative!r} —— 单源外移"
                "未净（双源漂移源头，plan §2.1(11)）。")
        self.assertNotIn(
            A3_ENV_NEGATIVE, agents_src,
            f"{tag} RED: AGENTS.md 仍含 {A3_ENV_NEGATIVE!r} —— env 绑定在 main "
            "侧 cron 命令串，本 agent 进程未必有（plan §2.1(11)，S8 随之消解）。")

        # ② 记忆节窗口：平台自动承载 + 负向四锚。
        memory_section = _section_text(self, agents_src, A3_MEMORY_HEADING, tag)
        for positive in A3_MEMORY_POSITIVES:
            self.assertIn(
                positive, memory_section,
                f"{tag} RED: 记忆节缺锚 {positive!r} —— 「记忆由平台自动承载"
                "/ 想不起来诚实说记不清 / 绝不编造孩子说过的话」未落（§6.7 "
                "降级语义下无画像注入时模型不得编造，plan §2.1(17)）。")

        # ③ 全文零版本号与 rationale 仓维残留。
        for negative in A3_VERSION_NEGATIVES:
            self.assertNotIn(
                negative, agents_src,
                f"{tag} RED: AGENTS.md 仍含 {negative!r} —— 仓维注记对模型"
                "零操作语义，S5 残留未撤（plan §2.1(16)(18)）。")

        # ④ 对象锁定行：K3 冻结锚串逐字 + S3 澄清两半（甲案裁决在场）。
        lock_lines = [l for l in agents_src.splitlines() if A3_OBJECT_LOCK_KEY in l]
        self.assertTrue(
            lock_lines,
            f"{tag} RED: AGENTS.md 无「{A3_OBJECT_LOCK_KEY}」行 —— 底线对象锁定"
            "条缺位（K3 冻结锚承载面被删，plan §3）。")
        for line in lock_lines:
            self.assertIn(
                A3_K3_PRIVACY_ANCHOR, line,
                f"{tag} RED: 对象锁定行 K3 冻结锚串不逐字在场（含右括号）—— "
                "单向追加红线被破（test_261022_honcho_memory :229 子串断言依赖，"
                "plan §2.1(13) builder 边界①）。")
            for clarify in A3_S3_CLARIFIES:
                self.assertIn(
                    clarify, line,
                    f"{tag} RED: 对象锁定行缺 S3 澄清 {clarify!r} —— 备注读回"
                    "（S-F1.3 AC4）与家长隐私禁止的接缝未处理（plan §2.1(13)，"
                    "甲案）。")


if __name__ == "__main__":
    unittest.main()
