---
id: TECH-261026-oneshot-wrapper-prompt-derived-facts
title: 设备流程回合的「机器包装指令」随孩子原话一起以 owner 身份入库，被 deriver 抽成「孩子要求整理成文」的过程性结论
domain: tech
status: deferred
severity: SHOULD
owner: dev-loop（实现候选）+ 主线程（清理运维轮）+ architect（上游提报形态）
trigger: 下一次动记忆管线（抽取策略 / honcho 插件版本升级 / 需求面记忆口径再评）时一并裁；或运营面观察到该族结论再次挤占画像窗口时
opened: 2026-10-26
closed: ""
closedBy: ""
paths:
  - server/gateway/front/front_app.py
  - server/deploy/honcho/
  - server/deploy/261022-openclaw-honcho-plugin/
source: "需求面批 261025-spec-reconciliation-memory-writing-scope 的可行性核实（architect 2026-10-26 核实报告，逐条对实现与 live 数据）；live 实证 = Honcho documents 表 owner 侧 6 条过程性结论（说明见正文）；源消息形态 = front_app.py:409-410 与 :848-849 的恒定包装模板"
handoff: "机制修复 → dev-loop（候选路线见正文，需 human 选路）；上游提报 → architect 起草 issue 形态；运营清理 → 主线程运维轮"
---

**证据**（2026-10-26 live 实测 + 代码定位）：

1. **源消息形态**：front 的两条一次性回合把「机器指令」与「孩子原话」拼成**一条 user 消息**——整理回合 `f"孩子讲的内容：\n{material}\n\n请按指令把这段话忠实整理成一篇。"`（`server/gateway/front/front_app.py:409-410`）、引导回合 `f"孩子到目前为止讲的内容：\n{spoken}\n\n孩子停顿了，请按指令给一句轻声引导。"`（同文件 `:848-849`）。后半句是常量，机器/孩子边界在源头 100% 可分离。
2. **归属机制**：该消息以 `role=user`、`peer_name=owner` 入库（经 `provider.py` 的 `user=<device_id>` 透传落设备 session 键族），deriver 的观察者扇出是**按 role 而非 authorship**——owner 的 user 消息扇出到 `(owner, owner)` 与 `(agent-kidpal, owner)` 两个「关于孩子」的结论集合。
3. **live 实证**：owner 侧存在 6 条过程性结论，全部由包装指令派生：
   `owner asks agent-kidpal to faithfully organize one passage of the child's speech into an article.`（2026-10-24 11:30）
   `On 2026-10-22, owner instructed the agent to faithfully organize into an article the child's account of making rubber-clay crafts.` 等（同日 11:29/11:21，共 5 条）。
   按 created_at 倒序排名分别为第 **35 / 38 / 52 / 63 / 66 / 69** 位，**全部落在最近 100 条输入窗口内**（owner 侧结论总数 404 条）。
4. **不是偶发**：多数由 **backfill** 重新灌入 10-20 / 10-22 的整理回合后派生（documents.created_at 为 10-24，内容指向 10-20/10-22），说明历史语料同样携带该形态。
5. **三个环节都无处置通道**（2026-10-26 核查）：① 发送前不能删——R2 已实证 request 级 system 不被模型跟随后，指令必须留在 user 消息里，否则整理产出退化；② 采集时不能裁——honcho 插件 `noisePatterns` 只支持整条丢（当初整条丢导致孩子原话也没进记忆，即本次需求口径反转的起因）；③ 派生时不能标记——Honcho 的 `messages.metadata` 列存在但不参与派生决策，无 skip 标记，deriver 提示词在容器镜像 `/app/src/deriver/prompts.py`（改它 = 自管镜像补丁）。

**影响面**：孩子画像的输入窗口被「孩子要求把这段话整理成文章」这类从未说过的过程性陈述占用（当前 6 条），与 dreaming 污染同属「机器生成文本被署成 owner」的同一结构缝隙；本次 dreaming 那条已由全局关停 + 数据清理闭合，本条是**同一缝隙的另一形态**，且形成路径（模板包装）仍在生产新数据。不修则随每次故事整理/引导回合继续累积。另注：因 `honcho_search_conclusions` 已放行给记忆回忆回合，这类结论也可能被检索召回。

**建议改动**（候选，交 review / human 选路，不自批）：

- **A 改包装形态**（成本最低，成效不确定）：给机器部分加显式标记（如 `【设备指令】…`），寄望 deriver 不再当作孩子的事实；需实测抽检能否真的不再派生该类结论。
- **B deriver 抽取策略过滤**（确定性最高，代价最大）：改自托管 deriver 的提示词模板（镜像层），加「忽略整理/格式类指令，只抽孩子世界的事实」规则；须自管镜像补丁并承担升级漂移（对齐 `ARQ-260907-engine-deploy-patch-drift` 的同族教训）。
- **C 运营面定期清理**（现状）：按族删除过程性结论，无法根治，且两次清理之间仍会挤占窗口。
- **D 上游提报**（正解、周期不可控）：向 honcho 插件 / Honcho 提「消息级不参与派生标记」的 feature 请求，让发送侧能声明「这段是设备指令，不是用户话语」。

**风险**：riskTier **medium**——改动落在消息形态或派生策略上，不触契约面、不触设备端；主要风险 = 方案 A 无效却以为解决（须以 live 抽检结论为准）、方案 B 的镜像漂移、方案 C 的清理不及时。review profile = **Track B**（三要件：执行测出有实证 / 问题与修复路径清晰 / 爆炸半径已量化），path 不落 G1/G3/契约面。owner = dev-loop + 主线程 + architect。**形态注记**：若最终选定方案 B（改镜像）或 D（改插件），按约定升 full-adversarial + human。
