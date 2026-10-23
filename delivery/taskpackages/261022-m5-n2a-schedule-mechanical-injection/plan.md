---
changeId: 261022-m5-n2a-schedule-mechanical-injection
title: 课表机械注入——front schedule.py 单源派生课表摘要，经内部通道注入 P1 动态 context 块，让零工具调用也能答对、断绝编造长尾
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect（workstream PROPOSAL FROZEN v1.0 + arch PROPOSAL FROZEN v1.1；门 1 待 human 派 architect）
date: 2026-10-22
gateProfile: ["evidence-integrity", "test-quality"]
dependsOn: ["261022-m5-k1-prompt-carrier-migration"]
mergeOrder: 51
---

# 背景摘录（锚「问题与根因（整改型）」，PROPOSAL §2）

- **课表编造长尾**：探针复现模型语音称「我先看一下课表哈」但**零工具调用**，直接编造「周四 = 语文/数学/英语/科学 + 16:00-17:00 美术课带画笔」；真实 `workspace/kidpal/schedule.md` 周四 = 语文/数学/道法/体育/英语/劳动/足球 + 18:30-20:00 编程（TECH-261018 在案，debt 证据 4）。v11 已把识别特征扩列到极致仍不能保证执行——纯 prompt 纪律把「是否读文件」留给模型自由裁量，是不可靠层。
- **架构裁定**（arch PROPOSAL T5）：候选甲采纳——front `schedule.py` 派生课表文本 → 会话建立时经既有内部注入通道（`kidpal_speak` / 记录语境信号同型，ADR-003 / ADR-013 先例）送入引擎 → 引擎拼进每轮动态 context 块。乙（引擎绑单源）为主案否决，丙（引擎自解析 MD）直接否——造第二解析器，违反契约 §1.5 单源红线。
- **注入语义**：逐轮在场（不按意图门控）、有界（今日 + 近日，建议 ≤3 日 / ≤2KB，本包定稿）、会话建立时刷新、窗口外日期仍走读文件或诚实说查不到、禁凭印象。
- **消费面**：本包落地即满足 `TECH-261018-schedule-read-compliance-residual` 第 3 层机械注入修复。

# 可行性判据

- host 面可闭环：`parse_schedule_md` / `items_for_date` 纯函数可离线对照，注入内容 == 解析器输出可机械反证；受控 ≥20 轮提问可在 loopback 探针执行。
- P1 已建立动态 context 块与有界模板——本包只填注入通道与内容，不重建装配机制（dependsOn 硬边）。
- 契约单源红线已有 spec 承载（契约 §1.5 :184-186、arch §5.1.2 课表解析行），无需新 spec。
- live 面：真机课表提问与人工核对归 P5 收口轮（真机配额默认核心包）。

# 跨包事实与共享冻结面约束（只有 RTE 看全局的部分）

- **动态 context 块由 P1 建立**：本包必须在 P1 模板的占位/装配面内注入，不得另建第二动态块或把静态纪律塞回动态块。若 P1 未合并则不得启动。
- **`engine-patch/connection.py` 与 P2 共享**：merge order 上 P5（51）先于 P2（52）；P2 rebase 时须保留本包的动态块装配段，本包不得改动 P2 的 payload 历史/user 字段语义。
- **`schedule.py` 单源只读复用**：`server/gateway/schedule.py` 的 `parse_schedule_md` / `items_for_date` / `group_upcoming` 是唯一解析器；本包不得复制解析逻辑到引擎或 front 其它文件；若必须改 schedule.py，严格最小且不得改变既有输出契约（test_m3k1 / test_m3k3 冻结验收断言）。
- **会话建立时注入的时序**：front 在会话建立时把摘要经内部通道送入引擎；课表变更 / 跨天最迟下一会话生效，会话内变更不追（记录为边界）。
- **B1 课表 MD 结构与 display E1 零改动**：注入只改「谁把内容喂给模型」，不改文件格式、不新增端云帧。
- **非本包**：静态语义承载（P1）、持久会话（P2）、记忆画像（P3/P4）。

# 铁律约束

- 契约 §1.5 单源红线：引擎不解析 MD、不建第二解析器；注入内容必须可追溯到 `schedule.py` 输出。
- 不按意图门控（是否课表回合不留给模型裁量）；窗口外诚实说查不到，禁凭印象。
- 契约面只读；B1 / display E1 / §三提醒 / §四私有消息帧零改动。
- debt 结项（TECH-261018 status: closed + consumed-by）由 integrator 收口路径落，Pod 内不手改账本。
- 本 Pod 只做本包 changeManifest 内文件；扩单 → 停、报 planner 走 architect 评估。

# demo 证据形态（与 roadmap 一致）

- host runner 输出：注入内容 == `parse_schedule_md` / `items_for_date` 同日派生逐项一致（受控日期集）；窗口 / 有界断言。
- loopback 探针：≥20 轮课表提问零工具调用也不编造，答案与 `kidpal/schedule.md` 逐日一致。
- 真机轮：本包为 M5 收口轮 owner（与 P4 计数报告对账同轮），human 持板抽问今天 / 明天 / 后天课表。
