---
changeId: 261010-prompt-v9-schedule-anti-narration
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect（/triage-change 分流 review，2026-10-10）
date: 2026-10-10
---

# prompt v9 bump：课表问答回合补对称反叙述纪律（TECH-261010-schedule-round-narration-leak）

## 根因假设
- v8 回合路由表纪律不对称：对话行（openclaw-system-prompt.v8.md:11）有「任何『先查一下/先确认一下』的内部动作都是违规」反叙述纪律，课表行（:13）只钉「先读 schedule.md 再作答」——v7 修同类问题时只修了对话行，课表行漏对称条款。
- 触发面 = deepseek-v4-flash 输出风格漂移（把回合路由表词汇当工具调用前导文本念出）：10-10 16:49 轮引擎日志逐句对账 + 9-26 对照轮干净前导，链路（prompt v8/模型/OpenClaw 2026.8.1）零变更——非我方配置回归。
- 泄漏通道 = 工具调用轮次间前导文本经 OpenClaw /v1 流式端点原样推引擎逐句 TTS；最终 agent 输出干净（网关 journalctl 对账）。

## 修复方案（prompt v9 版本化 bump；槽位纪律 ARQ-260908——已核 server/deploy/ 无 .v9 在场）
- 新建 server/deploy/openclaw-system-prompt.v9.md = v8 全量 + 课表问答回合行补对称纪律：工具/文件动作照做（先读 schedule.md、反编造、备注读回全保持），但语音只播最终面向孩子的答复——任何回合判定、文件路径、流程步骤、内部动作叙述不得进入语音部分（v7 :11 句式扩行）；查字行既有「语音照常口语讲字义」正面要求保持。历史 v1..v8 零回改。
- 接线四面同步（版本化铁律：渲染面 == 声明面 == 被测面）：① OPENCLAW_SYSTEM_PROMPT_CARRIER 现值 v8 → v9 + 演进史 append；② provider.config.yaml system_prompt.artifact/source/deploy_target（:27/:31/:32）→ v9；③ apply-xiaozhi-overlay.sh PROMPT_SRC/PROMPT_DST（:102-103）→ v9；④ docs-sync：arch §5.5.5（:297）与 §5.1（:196）「现状 = vN」指针刷 v9，顺带收敛既有漂移（arch 现写 v7、实际接线 v8——v8 批漏刷，记入本包 docs-delta）；server/deploy/README.md prompt 指针节同核。
- 部署：v9 + overlay 重跑上 ECS（决策 21 键同款先例：ecs_put.py exec_command+stdin + MSYS_NO_PATHCONV=1，上传前核远端 == 基线）。

## 影响面
- 文件：server/deploy/{openclaw-system-prompt.v9.md（新）、OPENCLAW_SYSTEM_PROMPT_CARRIER、provider.config.yaml、apply-xiaozhi-overlay.sh}；server/acceptance/test_prompt_v7_round_routing.acceptance.py（additive 新齿）；docs/architecture.md §5.5.5/§5.1 指针；（核）server/deploy/README.md。固件零改动。
- 契约零改动（contract :184 prompt 意图摘要不涉语音纪律；display 块规则零变）；需求零改动（NFR-SAFE-1 within-spec）。
- 已知地雷：本包验收全为文件内容锚、无 DB/事件断言——若 supervisor 机械闸判 all-mock 触发 riskTier 重算 high → 需 human 放行（先例在案），plan 前置申报。

## acceptance 种子
impactedTests：server/acceptance/test_prompt_v7_round_routing.acceptance.py（既有冻结件，纯 additive 补齿优先走 tighten-only 窗口——不新建文件、既有断言语义不动；需改既有齿语义则正常改验收通道留痕）；test_m3k3_voice_schedule_entry.acceptance.py::test_a1_prompt_wired_three_faces_version_agnostic（版本无关三面守卫，随接线面自然转绿 v9、零改动消费）；server prompt/deploy 族相关面子集 + 机械闸全量不降。

验收齿草案：
- PN-A1：v9 落盘非空 + 首行 `## 角色`（无前言结构齿，V7-A1 同形态）。
- PN-A2：课表问答回合行含对称反叙述纪律锚（内容驱动选节、并集窗口不钉措辞：「先读 schedule.md」既有锚保持 + 反叙述纪律入窗）。
- PN-A3：v8 sha256 零回改齿（V7 ⑤ 形态复刻）。
- PN-A4：三面接线一致 = v9（CARRIER == provider.config 三键 == overlay PROMPT_SRC）。
- PN-A5（live 观测项，诚实申报为非确定性断言）：ECS 部署后真机问「明天有什么课」——TTS 无回合判定/文件路径/流程步骤类前导叙述（模型风格漂移无法离线断言根治；journalctl + docker logs 双侧逐句对账留痕）。

## 非目标
- 引擎侧 strip 前导文本兜底（engine-patch 面扩大）——留账；prompt 纪律失效复发时再评。
- 其它回合行复核（查字行已有正面要求兜着）。

## demo 证据形态
offline 锚齿红→绿全程留痕 + ECS 部署记录 + 真机课表问答 1 轮双侧日志逐句对账。
