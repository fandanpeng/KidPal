# 轻量设计 PROPOSAL · 261029-gov-review-p9-script-hygiene-cluster

> **状态：FROZEN v0.1**（2026-10-29 主线程凭「human 立项 ack + R1 设计审 GREEN」标；冻意图、边界、关键方案与验收标准，不冻未来字节。R1 findings S1/N1/N2/N3 作为实施义务随批携带，不阻冻结）
> **gov-flow**: direct-implementation
> **承载 ARQ**：`ARQ-261025-brief-inputs-anchor-noise`（T1）· `ARQ-261014-pod-events-booking-telemetry-fidelity`（T2，仅 human 已拍范围）。
> **riskTier**：low。两项都只改 advisory / 遥测读侧与落账约定，不改任何 blocking 判据。
> **实施通道**：首个 scripts author-implementation 新通道试点；普通脚本与 planner 非 review-loop agent 定义由 architect 实施，独立 reviewer 审设计与真实 diff。

## 1. 目标与非目标

### 目标

- T1：让 `brief-inputs.mjs` 区分符号引用、整文件文档引用和真实路径 / 锚点腐坏，终端不再逐条刷低价值 advisory，同时不丢失可核对的明细。
- T2：让 escalation 能被 `human-decision` 的显式引用闭合，兼容存量 `resolves` 写法；统一新事件的 actor 与关联字段约定，并为事件类型和 payload 缺陷提供 fail-open 告警。
- 用有判别力的自检覆盖新旧行为，保持历史事件只读、所有现有门判据不变。

### 非目标

- 不实现 MODEL-TIER 声明与实际模型对账，不排查 `mainThreadUsage` 快照刷新；这两项明确留待后续批。
- 不改 `verify.mjs`、supervisor、前置升级闸、merge、契约、spec、scope-map、settings 或任何 blocking 行为。
- 不改写、补记或修复历史 `pod-events.jsonl`；不从 summary 自然语言、决定类型、actor 或时间先后猜测 escalation 已闭合。
- 不新增运行时子命令，不建立新门或新账本；`--self-check` 只作为脚本验证入口。
- 不在本批结项或移动 ARQ 条目，账本状态由主线程按收口流程处理。

## 2. 现状复证

- `brief-inputs.mjs:216-225` 先查盘再处理裸 ref，导致 `NFR-PRIV-1` / `TECH-*` 进入「文件不在磁盘」分支；存在的整文件 ref 每条各产生一个 advisory。`:588-593` 的自检仍绑定旧 bare / missing 口径，必须与实现同步改。
- `pod-events-aggregate.mjs:217-243` 只收集 `agent-escalation-resolved.payload.refEventId`，没有 human-decision 闭合、`resolves` 兼容或短 eventId 前缀解析；`resolved` 目前还是 resolution 事件条数，不是实际闭合的 escalation 数。
- `pod-events-append.mjs` 已有事件类型闭集，但未导出供读侧复用；聚合器对未知事件类型和非对象 payload 没有 schema 告警。planner 现有条文要求记录 human decision/correction，但未钉死 actor 归属和闭合引用字段。

有效历史基线：`260928-char-window-key-triggered-play` 的 escalation `570eb95d…` 被 `agent-escalation-resolved.payload.resolves="570eb95d"` 引用，当前仍列入 unresolved；`261011-p1-home-v5-rework` 的两条 human-decision 只在 summary 中提到升级事件，没有结构化 `refEventId`，当前对应两条 escalation 仍列入 unresolved。后者在本批后仍应保持未闭合，证明实现没有猜测历史散文。

## 3. T1 · brief 锚解析分类与 advisory 降噪

### 3.1 引用分类

在现有 story 特判之后、查盘之前增加非路径符号引用分类：

- 无路径分隔符、无 `#` 锚且不带文件扩展名的裸 token 归为 `symbolic`，在锚点表如实标为「非路径 ref，跳过锚点解析」，产生 info 级记录，不再报缺失文件。
- `README.md`、含 `/` / `\` 的路径、带锚 ref 仍按路径处理；真实缺失路径和显式锚未定位保持 warning 级可见性，不因降噪被吞掉。
- 已存在的 Markdown 文档整文件引用保留 `bare` 表格状态，但降为 info，不再建议每份文档都必须补锚；不自动猜章节。非 Markdown 的裸路径不纳入这条文档豁免。

### 3.2 输出形态

- 将 advisory 内部表示带上 level / kind / message，避免靠中文文案反推类别。既有 crossEnd `warnings` 通道和 fail-loud `errors` 不变。
- CLI 对所有 advisory 只输出一行汇总，至少给出 info / warning 总数及各类 kind 计数，不再逐条 `ADV` 刷屏。
- 在生成的 `input-brief.md` 末尾增加非阻断 advisory 明细节，保留逐条 message 与必要 ref；终端降噪不以丢失真告警定位为代价。正常输出 JSON 的 advisory 计数继续反映实际条数。
- 既有自检中「bare 必须产生警告」的旧断言改为新分类断言，并新增裸 token、真实 missing / not-found、单行汇总和 brief 明细的判别 case。

## 4. T2 · pod-events 闭合、schema 收敛与告警

### 4.1 显式引用闭合

- 闭合来源扩为 `agent-escalation-resolved` 与 `human-decision`；两者均以 `payload.refEventId` 为新写法的唯一关联字段。
- 读侧把存量 `payload.resolves` 作为 `refEventId` 的弃用别名兼容，不要求迁移历史行。新事件不再写 `resolves`。
- 引用支持完整 eventId；为兼容已有 `resolves="570eb95d"`，也接受能唯一匹配本包 escalation 的短前缀。无匹配、匹配到非 escalation 或前缀不唯一时只告警、不闭合。
- `escalations.resolved` 改为有至少一条有效闭合引用的 distinct escalation 数，`unresolved` 是其补集；重复闭合事件不重复计数。human-decision 没有引用时只是普通决定，不产生闭合，也不报缺关联字段。

### 4.2 写侧 schema 约定

在 `.claude/agents/planner.md` 的 pod-events 实时记账和人批路径补齐同一约定：

- `human-decision` / `human-correction` 的 `actor` 恒为 `human`；planner 是记录者，不把决定归属写成自己。`source` 继续按真实记录通道及既有 backfill 约定填写。
- human 处置某个 escalation 时，在该 `human-decision.payload.refEventId` 写原 escalation 的完整 eventId；`agent-escalation-resolved` 同样使用完整 `refEventId`。关联字段与既有 decision / summary 等 payload 内容并列，不新增事件类型。
- 只明确「引用哪个事件」的机械格式，不要求 planner 从既有 summary 反推或补造历史关联。

### 4.3 读侧 fail-open schema 告警

- 从 `pod-events-append.mjs` 导出既有 `VALID_EVENT_TYPES`，聚合器复用同一闭集，不复制第二份枚举。未知事件类型计入事件总数并产生 `unknown-event-type` 告警，但不进入各专项聚合。
- 每条已解析事件都必须带非 null、非数组的对象 payload；缺失或类型错误产生 `invalid-payload` 告警。空对象不一律判错，例如既有 `pod-closed` 可为空；friction 的空对象与缺字段继续由 `malformedFrictions` 按现行更严格规则报告。
- `agent-escalation-resolved` 缺少 `refEventId` / 兼容 `resolves` 时产生 `missing-closure-ref` 告警；human decision/correction 的 actor 非 human 时产生归属告警。无效闭合引用也进入告警列。
- 聚合结果新增 `schemaWarnings` 数组及计数，元素带 eventId / eventType / code / message 等定位信息，并向 stderr 发诊断。`ok:true`、退出码 0、malformed 行跳过和所有现有门行为保持不变。
- 为聚合器增加 `--self-check`，以内存事件夹具覆盖 canonical 闭合、human 闭合、legacy 短前缀、无引用 human-decision、重复闭合、未知类型、无效 payload 和错误 actor；不改变正式 CLI 的既有参数。

## 5. 保护边界与牵连面

- 本批只触及两件普通遥测 / brief 脚本、事件 append 脚本的枚举导出，以及 planner 普通 agent 定义；四件均在 architect 现有 allow 内，无保护面落地。
- `VALID_EVENT_TYPES` 的唯一事实源仍是 `pod-events-append.mjs`；`dev-loop-operating-system.md` 的现有枚举投影不变，本批不新增事件类型，故无需另改 reference。
- planner 的 description、tools 和既有合并门条文不改；只补 pod-events 写字段约定，无需改 scope-map 或路由夹具。
- 不消费或改写历史账本事证。兼容只发生在聚合读侧；无法结构化确认的旧 escalation 保持 unresolved，宁可显式暴露也不假闭合。
- 两项均为既有脚本 fix 级修正及配套自检，无 feature 级新增子命令；实际 diff 出 manifest 必须回 reviewer。

## 6. 验收标准与自测计划

1. `node .claude/scripts/brief-inputs.mjs --self-check` 全绿；夹具证明裸 token 不再落 missing、整文件 Markdown 降 info、真实缺失路径 / 锚点仍为 warning，且终端汇总器只产一行而 brief 保留明细。反转任一分类或恢复逐条输出应使对应断言变红。
2. `node .claude/scripts/pod-events-aggregate.mjs --self-check` 全绿；夹具证明 human / resolved 两类显式引用均可闭合，`resolves` 短前缀可唯一解析，普通 human-decision 不误闭合，重复引用不虚增 resolved，所有 schema 缺陷只告警不阻断。
3. 三件脚本均通过 `node --check`；brief 既有缺失文件、story、crossEnd、uiux 等自检 case 不回归。
4. 历史只读复验：`260928-char-window-key-triggered-play` 应变为 `total=1 / resolved=1 / unresolved=[]`；`261011-p1-home-v5-rework` 应保持两条无结构化引用的 escalation 未闭合。任一结果相反都要查明，不得靠 summary 猜引用凑绿。
5. 对含空 friction payload 或旧事件类型的真实包运行聚合器，确认 `ok:true`、退出码 0、告警可定位，原有 `malformedFrictions` 仍非空。
6. reviewer 另核实际 diff ⊆ manifest、无 blocking 判据变化、无 manifest 外文件；本设计不自评通过。

### 设计前现状基线（2026-10-29）

- `brief-inputs.mjs --self-check`：**43/43 passed，SELF-CHECK GREEN**。这证明旧行为自检当前为绿，也证明旧 bare 断言必须在 T1 同步修订，不能当作新口径验收。
- 聚合器现无自检入口；真实包基线已跑通且退出码 0：`260928-char-window-key-triggered-play` 为 `total=1 / resolved=1 / unresolved=1`，`261011-p1-home-v5-rework` 为 `total=3 / resolved=1 / unresolved=2`。两处 unresolved 与条目所述缺口一致。

## 7. 轻量 landing-manifest

```landing-manifest
.claude/scripts/brief-inputs.mjs | architect | ordinary | T1 分类、汇总、明细与同步自检
.claude/scripts/pod-events-aggregate.mjs | architect | ordinary | T2 闭合、schema 告警与自检
.claude/scripts/pod-events-append.mjs | architect | ordinary | 仅导出既有事件类型闭集供读侧复用
.claude/agents/planner.md | architect | ordinary | actor 与 refEventId 写侧约定，非 review-loop 定义
delivery/workstreams/gov-debt-reduction/task_plan.md | main-thread | ordinary | 批记账：workstream 波次记账
docs/debt/gov/ARQ-261014-pod-events-booking-telemetry-fidelity.md | main-thread | ordinary | 批记账：ARQ 范围裁定追注
```

## 8. 设计评审记录（architect-reviewer · 2026-10-29，append-only）

### 四查结论

**意图保真**：T1 与 human ack「三条建议改法全取」对齐——ARQ-261025 建议①②③分别对应 §3.1 symbolic 分类、整文件 Markdown 降 info（条目建议②的「降为 info 或按章节锚解析」二选一取前者，§3.1 明写不自动猜章节）、§3.2 一行汇总；brief 明细节为设计新增，与条目「真 ref 腐坏报警可信度不被稀释」意图一致。T2 与条目 ARQ-261014 范围裁定追注（正文 2026-10-29 段）逐项对齐：核心①（human-decision 引用闭合）= §4.1、核心②（refEventId 单一写法 + resolves 兼容映射 + actor 归属）= §4.1-4.2、追加校验（事件类型枚举告警 + payload 必填校验，fail-open）= §4.3；③④明确出非目标，与追注「不在本批」一致。历史只读、不从 summary 猜闭合的边界（§1 非目标 + 验收 4）与条目「机器聚合不得假闭合」意图一致。

**纪律合规**：`> **gov-flow**: direct-implementation` 恰一行（PROPOSAL:4），形状过 `DIRECT_FLOW_RE`（governance-gate-shared.mjs:148）。manifest 四行各四列，解析器取前三列、第四列理由被忽略（governance-gate-shared.mjs:194-205），格式合法。归属核对亲验 scope-map：architect allow 含 `.claude/scripts/**`（scope-map.json:176）与 `.claude/agents/**`（:171），deny 列（:179-197）不含 planner.md——manifest 四件全在 allow，无保护面落地；planner 非 review-loop agent 定义（review-loop 保护集 = architect-reviewer/review-moderator/scenario-author/challenger/defender/adjudicator/landing-verifier，均不涉本批），architect 实施归属如实。轻设计六要素齐（目标/非目标/关键方案/保护边界/验收标准/manifest），未写逐字节改法，符合 SSOT 第 1 步口径。riskTier low 与实况一致：两脚本均为 advisory/读侧，不触 verify.mjs、闸、merge、契约任何 blocking 判据。

**撞车复核**：设计「现状复证」三条逐项亲核为实——brief-inputs.mjs:216-225 missing/bare advisory 逐条 push（:220/:225）、自检 :588-593 绑定旧 bare/missing 断言；pod-events-aggregate.mjs:217-243 仅收 `agent-escalation-resolved` 的 `payload.refEventId`（:219-220）、`resolved` = resolutions 条数（:240）、无 resolves 映射无短前缀；pod-events-append.mjs:29 `VALID_EVENT_TYPES` 未导出（同文件 :47 `FIXER_ROOT_CAUSES` 已有单源导出先例，导出方案无先例障碍）。planner.md 现状核实：ESCALATION-BRIDGE（:98）已钉 agent-escalation-resolved 的 refEventId 写法，human-decision 落点在 :44/:127/:129 但确未钉 actor 归属与闭合引用字段——设计「补齐同一约定」与现状吻合、无重复条文冲突。历史基线亲验：260928-char-window-key-triggered-play/pod-events.jsonl:5 escalation `570eb95d-ceba-4f44-92f1-c12b121720c1`、:7 resolved 事件 payload `resolves:"570eb95d"`（8 位短前缀）；261011-p1-home-v5-rework 三条 escalation（fe6c3f46/851eb481/930257e5）仅 851eb481 被 d650d56e 的 refEventId 结构化闭合，两条 human-decision（b7e2d94a/d41f0b6e）确只在 summary 散文提级——§2/验收 4 的前后预期与磁盘事实一致。dev-loop-operating-system.md §5d:130 事件闭集投影不含新类型，设计「本批不新增事件类型故不动 reference」成立。

**牵连面完整**：聚合器消费方圈定为 loop-retro（loop-retro.md:27 new-format 读取 + dev-loop.md:92 文档行）；`escalations.resolved` 无机械消费方（retro-efficiency.mjs 不读 escalations），语义修正形状兼容。brief 的 stderr ADV 行（brief-inputs.mjs:790-791）与 JSON advisory 计数（:806）无机械解析方，input-brief 消费方 = acceptance-author/builder（acceptance-author.md:24 / builder.md:24），新增明细节不破封闭输入面语义。`journal-append.mjs:60` 的同名闭集属 pod-journal 另一账本，不涉。聚合器 import pod-events-append 无副作用风险（pod-events-append.mjs:133-136 有 CLI 导入护栏）。

### Findings

- **S1 · SHOULD · T1 自检适配范围未列全**：§2 与 §3.2 只点名 :588-593 旧 bare/missing 断言须同步改，但 advisories 改为带 level/kind/message 的结构表示后，既有依赖字符串方法的断言会机械变红——brief-inputs.mjs:620-621（uiux 悬空 ref `advisories.some((s) => s.includes(...))`）、:635（②③档越界 ADV）、:745-746（story 未定位到节）。验收 3 已要求这些 case 不回归；实施时须把这些断言适配到新表示（或保留字符串视图访问器），不得因结构化而削弱断言判别力。设计完备性问题，作者实施时落，不升级 human。
- **N1 · NIT**：§3.1 symbolic 判据「不带文件扩展名」需机器可判谓词（如末段 `\.\w+$` 白名单或启发式）——点分 token（如 `ESP32-S3-RLCD-4.2-Demo` 型）不产生歧义归类；建议自检加一条判别 case。
- **N2 · NIT**：`escalations.resolved` 由「resolution 事件条数」改为「有效闭合 distinct escalation 数」是既有输出键语义修正（形状兼容、无机械消费方）；建议实施提交在批记录注明该语义变化，避免 loop-retro 跨周期数字对比时误读。
- **N3 · NIT**：actor/refEventId 写侧约定只落 planner.md；pod-events schema 语义 SSOT 在 dev-loop-operating-system.md §5d（:141 仅说 planner 代 append、未钉归属），当前无冲突。两处今后各自演化有漂移风险，留待后续触 §5d 的批顺手收拢，本批最小 diff 不强求。

### 判档

常规流程，无条件加强。逐项依据：landing-verifier——核心落点清单（前置升级闸/共享闸库/merge-driver/deny-main-push/enforce-scope/INDEX landed 推导）全不涉，改动面 advisory/读侧可回滚；新鲜 reviewer——无评审争议；完整普查——消费方聚焦检索已圈住（见牵连面）。注记：本批为首个 scripts author-implementation 试点，通道属新但评审义务不变，试点风险显现在实现评审面（diff ⊆ manifest、自检判别力），届时按 SSOT 第 5 步从严核，不预设加强。

verdict: GREEN

## 9. 实施记录（architect · 2026-10-29，append-only）

实现 commit：`9534d71f8c587976f54767696a037a7b0d3f1e0d`（8 件：四件实现文件、两件主线程已落盘的 manifest 补记账文件，以及本批 PROPOSAL / events.jsonl）。

- `brief-inputs.mjs`：advisory 改为 `{ level, kind, message, ref }`；在 story 特判之后、查盘之前加入 symbolic 分类，Markdown 整文件引用降 info，真实 missing / anchor-not-found 保持 warning；终端改为一行分类计数汇总，brief 末尾保留逐条明细。S1 点名的 uiux、②③档越界、story 未定位三处自检均改用结构化字段并保留原负向断言；N1 采用末段 `\.[A-Za-z0-9_]+$` 扩展名启发式，点分 token 与带扩展名裸文件名各有判别夹具。
- `pod-events-aggregate.mjs`：复用 append 脚本导出的事件类型闭集；`agent-escalation-resolved` / `human-decision` 以完整 eventId 或唯一短前缀闭合，兼容存量 `resolves`；新增 fail-open `schemaWarnings` 与 12 项内存自检。**N2 语义注记**：`escalations.resolved` 已由「resolution 事件条数」改为「至少一条有效闭合引用的 distinct escalation 数」，跨周期 retro 数字不可按旧口径直接比较。
- `pod-events-append.mjs`：仅导出既有 `VALID_EVENT_TYPES`，未增删事件类型；`planner.md` 仅补 human actor 与完整 `refEventId` 写侧约定。N3 按裁定不触 operating-system.md。manifest 已补两件主线程记账路径，本 agent 未改其内容。

自测结果：

1. `node .claude/scripts/brief-inputs.mjs --self-check`：49/49 passed，SELF-CHECK GREEN。
2. `node .claude/scripts/pod-events-aggregate.mjs --self-check`：12/12 passed，SELF-CHECK GREEN；三件脚本 `node --check` 均通过。
3. 真实包 `260928-char-window-key-triggered-play`：预期短前缀闭合，实际 `total=1 / resolved=1 / unresolved=[]`，`schemaWarningsTotal=0`。
4. 真实包 `261011-p1-home-v5-rework`：预期两条散文提级不误闭合，实际 `total=3 / resolved=1 / unresolved=2`；空 friction payload 仍在 `malformedFrictions`，旧 `merge-completed` 产生一条可定位 `unknown-event-type` 告警，`ok:true`、退出码 0。
5. 真实包 brief dry-run：stderr 只有一行 ADV 汇总（info=4 / warning=2），不再逐条刷屏。
6. 临时副本反转验证：关闭 symbolic 分类后对应 brief 夹具如预期变红；移除 legacy `resolves` 映射后闭合夹具如预期变红。临时件均已删除，未改仓内历史事件。
7. `lint-descriptions.mjs` 无 flagged；`route-regression-check.mjs --self-check` 为 104 ok / 0 fail。

偏差：无 manifest 外出作者改动；`.codebase-memory/` 两件与既有 INDEX 工作树改动未 stage、未提交。

## 10. 实现评审记录（architect-reviewer · 2026-10-29，append-only）

### 范围核对

`git diff --name-only main...9534d71f` = 8 件：非证据 6 件逐一对照 manifest 六行（brief-inputs.mjs / pod-events-aggregate.mjs / pod-events-append.mjs / planner.md / task_plan.md / ARQ-261014 追注），件件在册、无 manifest 外文件；PROPOSAL 与 events.jsonl 为批证据件。其后 `fed74d1c` 仅追加本批 PROPOSAL（+20 行）与 events.jsonl（+1 行）两件证据，无实现面变化。manifest 补两行主线程记账件的修正本身已回审：task_plan.md 追加的复证 / 立项 ack / 通道说明四行与 ARQ-261014 的「范围裁定追注」内容与 §8 设计审所引 human 两拍一致，归属标 main-thread、保护标 ordinary 如实——修正成立。

### 逐项核验（R1 义务 + 冻结方案）

- **S1（三处自检适配）**：亲核 diff——uiux 悬空 ref 断言改用 `kind === 'uiux-missing-ref'` + message 包含 + ①c 节段内负向排除 + 明细保留断言（brief-inputs.mjs:684-686）；②③档越界改 `kind === 'counterpart-out-of-scope'` + message 包含 + fail-loud 负向臂保留（:700-701）；story 未定位改 `kind === 'story-not-found'` + message 包含 + errors 为空断言保留（:810-812）。三处均保留原负向 / 定位判别力，未因结构化削弱。
- **N1（symbolic 谓词 + 判别夹具）**：`isSymbolicRef` = 无 `\/#` 分隔符且末段不匹配 `\.[A-Za-z0-9_]+$`（brief-inputs.mjs:78-81），机器可判；夹具四件——`NFR-PRIV-1` + `ESP32-S3-RLCD-4.2-Demo`（点分 token 不误判）→ symbolic、`definitely-missing-P9.md` → 路径 missing、`firmware/main/mcp_server.cc` → bare-non-markdown warning、显式锚未定位 → anchor-not-found warning（:636-651）。反转抽验见下。
- **N2（resolved 语义注记）**：已在实施记录（§9 pod-events-aggregate 条目）明写「由 resolution 事件条数改为至少一条有效闭合引用的 distinct escalation 数，跨周期 retro 数字不可按旧口径直接比较」。
- **N3**：`dev-loop-operating-system.md` 不在 diff 文件清单，未触碰。
- **T1 §3.1/§3.2**：symbolic 分类插在 story 特判后、查盘前（:238-241）；Markdown 整文件降 info（:252-254）、非 Markdown 裸路径保持 warning（:255-257）；真实 missing / 锚未定位保持 warning（:244/:269-271）；终端只打一行汇总（:855），brief 末尾⑥节保留逐条 level/kind/ref/message（:596-604）；JSON `advisories` 计数不变（:870）。
- **T2 §4.1-4.3**：闭合来源 = `agent-escalation-resolved` + `human-decision`，`resolves` 作弃用别名，完整 eventId / 唯一短前缀均闭合，无匹配 / 非 escalation / 前缀不唯一只告警不闭合（pod-events-aggregate.mjs:259-281，reason 三值 no-match / not-an-escalation / ambiguous-prefix）；`resolved` = distinct Set 计数（:299）；缺引用告警只对 resolved 事件、human-decision 无引用不告警（:90-94）；`VALID_EVENT_TYPES` 从 append 导出复用（:17 + append :29），未知类型计入 eventsTotal 不进专项聚合，payload 必须非 null 非数组对象，空 `pod-closed` 合法、空 friction 仍归 `malformedFrictions`（:70-97）；`schemaWarnings` 带 eventId/eventType/code/message + stderr 诊断（:433-435）；`ok:true`、exit 0、CLI 既有参数不变。planner.md 约定块一条，钉 actor=human、完整 refEventId、不再写 resolves、不反推历史，覆盖实时记账与人批路径。

### 自测亲跑

1. `brief-inputs.mjs --self-check`：49/49 passed，SELF-CHECK GREEN（exit 0）。
2. `pod-events-aggregate.mjs --self-check`：12/12 passed，SELF-CHECK GREEN（exit 0）。
3. 三件脚本 `node --check` 全通过。
4. 真实包 `260928-char-window-key-triggered-play`：`total=1 / resolved=1 / unresolved=[]`，`schemaWarningsTotal=0`——与验收 4 前半一致。
5. 真实包 `261011-p1-home-v5-rework`：`total=3 / resolved=1 / unresolved=[fe6c3f46, 930257e5]`——两条散文提级保持未闭合，与验收 4 后半一致；`malformedFrictions=1`、旧 `merge-completed` 产生一条可定位 `unknown-event-type` 告警、`ok:true`、exit 0——与验收 5 一致。
6. 真实包 brief dry-run（261011）：stderr 仅一行 `ADV summary — info=2 warning=4 kinds=missing-ref=4, whole-document=2`，markdown 含「⑥ 非阻断 advisory 明细」且逐条带 ref——终端降噪不丢明细。

### 判别力反转抽验（仓外副本 C:/tmp/p9rev，未触仓内文件，验后已删）

- 反转①：`isSymbolicRef` 恒 false 后自检变红——`① 裸符号与点分 token → symbolic，不落 missing` FAIL（48/49，SELF-CHECK RED），证明该夹具确实咬住分类逻辑。
- 反转②：移除存量 `resolves` 回退映射后自检变红——闭合相关三条 FAIL（9/12，SELF-CHECK RED），证明 legacy 兼容路径与 distinct resolved 计数断言有判别力。

### 消费方抽查

- brief stderr ADV 行 / JSON 计数：全 `.claude` 面检索无任何 hook / 门 / 脚本解析 `brief-inputs: ADV` 行或该 JSON 的 advisories 计数；input-brief 消费方仍为 acceptance-author / builder 人读封闭输入面，新增⑥节不破封闭语义。
- `escalations.resolved`：无机械消费方（retro-efficiency.mjs 不读 escalations），语义修正形状兼容；loop-retro 为人读消费，N2 注记已防跨周期误读。
- `VALID_EVENT_TYPES` 导出：唯一新消费方 = 聚合器 import；append 的 CLI 导入护栏在（:134-136），import 无副作用；`journal-append.mjs` 同名闭集属 pod-journal 另一账本，不涉。
- blocking 面：verify.mjs、前置升级闸、merge 链路均不消费本批三脚本的输出；diff 未触任何 blocking 判据文件。

### 判档与 verdict

维持常规流程，无条件加强——试点风险显现在实现评审面已核毕（diff ⊆ manifest、自检判别力、消费方不破），无评审争议、无核心合并 / 权限自改，不触发 landing-verifier 第二核验。实施记录与真实 diff 一致，无虚报。

verdict: GREEN

reviewedCommit: fed74d1c99f292daba807407abe8f9518f07e645
reviewedTree: 60dbe01fc1d3899c4193a2eca2507f5c87c1536f
