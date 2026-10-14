---
id: TECH-261010-schedule-round-narration-leak
title: 课表问答回合把模型内部推理当语音播报（deepseek-v4-flash 把 prompt 回合表词汇当工具调用前导文本念出）
domain: tech
status: closed
severity: SHOULD
owner: dev-loop（prompt v9 bump 或引擎侧 strip）
trigger: 下个动 openclaw-system-prompt / 引擎 TTS 出口的包；再现率高（课表是高频问句）可提前
opened: 2026-10-10
closed: 2026-10-12
closedBy: 主线程（2026-10-12 tech 域 CRQ 盘点，human 指示；证据见各条目收口节）
paths: ["server/deploy/openclaw-system-prompt.v8.md", "server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER"]
source: 2026-10-10 真机使用反馈（问「明天有什么课」，播报把思考过程念出来）+ 引擎/OpenClaw 网关日志逐句对账
---

**定性：真 bug（用户可感缺陷），根子是「prompt 纪律不对称 + 模型输出风格漂移」合击，非 requirements.md 层面的设计缺口**——需求意图明确（语音部分 = 念给孩子听的话），但 v7 修同类问题时只给「孩子对话回合」补了反叙述纪律，「课表问答回合」行漏了对称条款，纪律面存在实现层盲区。

**证据（2026-10-10 16:49 轮，引擎日志逐句）**：ASR 识别「明天有什么课？」后，TTS 依次播出「这是课表问答回合」「需要先读 kidpal/schedule.md 拿真实课表」「先按 skill 流程读文件」「明天是 2026-10-11 星期日」「周日没有学校课程表内容，查每周固定课表和一次性安排」「課表里周日无任何行 → 明天（周日）没有课」，最后才是真答复「明天是星期日，不用去学校……」。OpenClaw 网关侧同窗口（journalctl）显示：16:49:08→16:49:11 三次模型调用（工具调用夹层），**最终 agent 输出干净**（只有答复 + display 块）——泄漏的是工具调用轮次间的**前导文本**，经 OpenClaw /v1 流式端点原样推给引擎、引擎逐句送 TTS。对照 2026-09-26 15:06/17:48 两轮课表问答：模型同样发前导文本但内容是孩子友好的「我帮你看看星期一有什么课」（干净）；9-26 → 10-10 之间本链路（prompt v8、模型 `deepseek/deepseek-v4-flash`、OpenClaw 2026.8.1）零变更——即触发面是模型输出风格漂移（把 prompt 回合路由表词汇当独白），非我方配置回归。引擎侧 `docker logs` 全量回溯（容器 09-10 起未重启）确认该叙述句式仅 10-10 出现过。

**影响面**：孩子听到机器内部独白（「这是课表问答回合」「需要先读 kidpal/schedule.md」），高频课表问句下随时可再现；查字回合同理暴露（v8 反叙述纪律只在对话行，查字行靠「语音照常口语讲字义」正面要求兜着）。

**修法候选**：① prompt v9（首选，dev-loop 走版本化 bump 槽位规则）：课表问答回合行补对称纪律——工具/文件动作照做，但语音只播最终面向孩子的答复，任何回合判定、文件路径、流程步骤不得进入语音部分（v7「任何『先查一下/先确认一下』的内部动作都是违规」句式扩行）。② 引擎侧兜底（engine-patch 面扩大，成本高）：strip 工具调用轮次间的前导文本段，只 TTS 末轮输出。**riskTier: medium**。

---

## 收口（2026-10-12 主线程盘点）

修法候选 ① 已落地：261010-prompt-v9-schedule-anti-narration 批 bump v9，课表问答回合行补对称反叙述纪律——「任何『这是课表问答回合』『需要先读 schedule.md』式的回合判定、文件路径、流程步骤、内部动作叙述都不得进入语音部分」（v10 第 14 行在世实证）；v10（261012-kidpal-realtime-facts）零偏移继承，carrier `OPENCLAW_SYSTEM_PROMPT_CARRIER` 指向 v10 且 261012 真机轮已按 v10 验证（RF-A5 双路径实证）。引擎侧兜底 ② 未启用（prompt 侧根治后无需要）。关闭。
