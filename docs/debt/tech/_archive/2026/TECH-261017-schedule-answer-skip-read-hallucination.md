---
id: TECH-261017-schedule-answer-skip-read-hallucination
title: 课表问答幻觉——「后天」未命中课表回合识别特征（v10 只枚举今天/明天/这周），kidpal agent 未读 schedule.md 直接编造（真机报「第三节英语/下午美术」等不存在课程）
domain: tech
status: closed
severity: MAJOR
owner: dev 批（server/deploy prompt 面）
trigger: 下一次动 server/deploy prompt 面的批（bump v11 补识别特征），与 TECH-261017-xiaozhi-template-residue-deploy-ssot-gap 同批
opened: 2026-10-17
closed: 2026-10-18
closedBy: 261017-prompt-v11-schedule-recall
paths: ["server/deploy/openclaw-system-prompt.v10.md"]
source: 2026-10-17 真机 smoke 轮（引擎日志 22:57:33 识别文本「后天都有什么课程？」→ 22:58:30 语音「第一节语文…第三节英语…下午还有一节美术」）+ OpenClaw audit（run chatcmpl_48ec37ef 仅 agent_run 零 tool_action；kidpal agent 自 2026-10-14 起 audit 无 read 记录）+ loopback 复现 9/9 读文件即答对
handoff: 与 TECH-261017-xiaozhi-template-residue-deploy-ssot-gap 合批走部署面 loop；bump 过版本无关守卫 + acceptance 锚齿
---

**证据（真机 + audit + 复现三源对账）**：2026-10-17 22:57 真机问「后天都有什么课程？」（当天 10-17 是星期六，后天 = 10-19 星期一）。引擎日志逐字：22:57:33「识别文本: 后天都有什么课程？」→ 22:57:54 语音「后天是星期一」+「我去翻翻课表哈」→ 22:58:30 语音「第一节是语文，第二节是数学，第三节是英语，第四节是体育，下午还有一节美术」。实际 `kidpal/schedule.md` 周一那一行 = 语文/数学/足球/科学/班会/语文/足球，**无英语、无美术**——整段凭印象编造。OpenClaw audit 该回合（run `chatcmpl_48ec37ef-0461-480e-9cba-e3fa01aefc5f`）只有 `agent.run.started/finished`、零 `tool_action`；kidpal agent 最后一次 `read` 工具动作停在 2026-10-14。同型 loopback 复现（同 system prompt、同问题、同 `openclaw/kidpal` 路径，含流式与非流式、含引擎真实拼接的完整 prompt）9 次**全部调用 read 并答对周一课表**——证明 prompt 本身可走通、skill 与工具均可用，故障是概率性的「该读未读」。

**根因**：`server/deploy/openclaw-system-prompt.v10.md` 回合路由表「课表问答回合」的识别特征只枚举「今天/明天/这周有什么课、几点上课、要带什么」，**「后天」及大后天/周X/星期X/下周/几号等表述不在其中**；判不清时按同表兜底落入「孩子对话回合」，而该行纪律明文「不调用任何工具、不读取任何文件」（v10 :11）——模型于是只剩凭印象编造一条路。这与 TECH-261012-weather-fabrication 是同一「枚举式 prompt 脆弱性」的第二个域（天气靠 v10 新增「实时事实回合」已根治；课表是识别特征枚举窄、非漏域）。

**影响面**：孩子得到错误的课表信息，直接违背「课表问题永不编造」底线不变量。所有未枚举的日期/时段表述（后天、大后天、周几、星期几、下周、某月某号）都会漏读、凭记忆作答，且答错形态对 8-12 岁孩子无自愈能力（孩子只会戳穿「没有美术」）。无「课程」二字的问题（「后天要带什么」「周一几点放学」）同理漏判。

**建议改动（human 已裁方向 = 只要提到课表就读、不排除）**：v10 → v11，仅改路由表「课表问答回合」识别特征列——从「今天/明天/这周…」扩为「凡提到课表、课程、上课、上什么课、几点上课、要带什么，以及任何『今天/明天/后天/大后天/周X/星期X/周几/这周/下周/（某月某）号 + 课』组合，一律判课表问答回合：先读 `kidpal/schedule.md` 再作答，读不到诚实说查不到」。8-12 岁语境「课」基本即学校课表，误伤极低且读文件只会更准。四面接线同批翻 v11（OPENCLAW_SYSTEM_PROMPT_CARRIER / provider.config.yaml system_prompt 三键 / apply-xiaozhi-overlay.sh PROMPT_SRC/DST / manifest.yaml 两条 v10 条目 + .config.yaml marker 锚）。交 review 不自批。

**风险**：riskTier: medium——改 prompt 单一事实源（版本化 artifact），需 bump 版本 + 三处接线 + 机检锚同步，漏一处即「验收所测≠生产所用」；但识别特征扩列是增量、不动既有回合纪律，爆炸半径 = 课表问答回合的召回面。

consumed-by: 261017-prompt-v11-schedule-recall（2026-10-18 收口：prompt v11 落盘 `server/deploy/openclaw-system-prompt.v11.md`，课表问答回合识别特征扩列 + 先读 `kidpal/schedule.md` 纪律句；四面接线同批翻 v11〔OPENCLAW_SYSTEM_PROMPT_CARRIER / provider.config.yaml 三键 / apply-xiaozhi-overlay.sh PROMPT_SRC·DST / manifest.yaml 两条目 + .config.yaml marker 锚集〕，v10 sha256 零回改；host 面验收 PV-A1/A2/A3 绿，真机轮留 PV-A8 human 持板门〔本 Pod 零部署/零烧录〕）
