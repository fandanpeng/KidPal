---
id: TECH-261030-qqbot-ask-user-block
title: QQ 渠道 `ask_user` 提问送不出去——run 卡死 900 秒、后续消息全排队，家长侧只看到「不回」
domain: tech
status: deferred
severity: MAJOR
owner: 主线程（OpenClaw 插件运维面）；修复面可能需向插件上游报缺陷
trigger: QQ 侧再现「发了没回」；或升级 openclaw-qqbot / OpenClaw 主体时
opened: 2026-10-30
closed: ""
closedBy: ""
paths: ["server/deploy/openclaw-skills/workspace-main/qqbot-reply-diagnosis/SKILL.md", "server/deploy/openclaw-skills/workspace-main/AGENTS.md"]
source: 2026-09-21 QQ 会话实录（转存于 2026-10-30 main 会话 compact 摘要）+ 仓内 qqbot-reply-diagnosis SKILL.md 第 6 条 + 2026-10-30 live 复核
handoff: 插件升级 / 替换批；或向 @tencent-connect/openclaw-qqbot 上游报缺陷
---

**证据**：

1. 2026-09-21 实录：会话 `agent:main:qqbot:direct:7fc1c647fd88af785487ebb35ac0d99c`（sessionId `78f7f8ef-6cde-4e9c-9aa4-0a59d5e2b38c`）当时 `state=processing reason=blocked_tool_call activeTool=ask_user`，卡到 900 秒超时，期间家长后续消息全部排队、无任何回复；日志里 ask_user 的提问没有发送记录，而正常回复都有 `Sent markdown chunk`。
2. 仓内已有诊断条目把该现象写成排障知识：`server/deploy/openclaw-skills/workspace-main/qqbot-reply-diagnosis/SKILL.md:24`（现象判据 + 绕行法：从 webchat 面拿到答案、`sessions_send` 带「不要再调 ask_user」回灌、或重启网关清卡死 run）。
3. 当时已建任务 `task_946b172d-df48-4440-bb4c-2d5500e545e2`「修复 QQ 渠道 ask_user 卡死会话」，未见修复动作。
4. 2026-10-30 复核：插件仍是 `openclaw-qqbot 2.0.3`（enabled，旧 `qqbot 2026.7.1` 已 disabled，配置未变）；家长 direct 会话最后更新 2026-10-15、`status=done`；近两日日志无 `blocked_tool_call` 复现（OpenClaw 日志只留 2 天）。即：机制未修，只是这段时间没踩到。

**影响面**：家长在 QQ 上问到含糊问题，agent 一旦走 `ask_user` 澄清就静默卡 15 分钟，后面的消息全排队，家长看到的是「发了没回」，也没有任何错误提示；只能靠人从 webchat 面把答案带过去。QQ 是家长主用通道，静默失败比报错更难发现。

**建议改动**（候选，交 review、不自批）：① 至少在 workspace-main `AGENTS.md` 与相关 skill 里写死「QQ 渠道不要调用 ask_user，含糊就先按最可能理解执行并说明」——prompt 层兜底，落地最快；② 插件侧做短超时降级（blocked 超过 30s 自动改发普通消息），需动第三方插件或包一层；③ 向上游 `@tencent-connect/openclaw-qqbot` 报缺陷（提问通道未接线）；④ 加监控：日志出现 `blocked_tool_call` 即告警——可与「DeepSeek 欠费导致 cron 静默禁用」合并成同一条 OpenClaw 告警出口。

**风险**：riskTier: medium——①是 prompt 面纪律、不是机械门，只能降概率；②要动第三方插件或包一层，需真机/真 QQ 复证；全程不触 `docs/contracts/**`。

**关联**：`qqbot-reply-diagnosis` skill 第 6 条已是本缺陷的运维兜底（现象判据 + 绕行法），本条记的是缺陷本身与修复候选，两者不互相替代。
