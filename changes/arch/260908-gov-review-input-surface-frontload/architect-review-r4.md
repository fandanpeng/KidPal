# architect-review-r4 · 260908-gov-review-input-surface-frontload

> **判官身份**：Track B 熔断条款（`arch-review-protocol.md`「双轨路由」节「Track B 内熔断升级」第 ③ 项）下 human 裁定的**零上下文新判官实例**，与 R1–R3 判官不同人、无其上下文承继。本轮效力：GREEN → 主线程标 FROZEN 进 packet-bound landing；不 GREEN → 按规则自动转 Track A。
> **评审对象**：`changes/arch/260908-gov-review-input-surface-frontload/PROPOSAL.md`（DRAFT v0.4）。
> **输入**：`AGENTS.md`「治理路由」节、路由 SSOT「双轨路由」节、`track-b-judge-brief.md`（逐条照办）、PROPOSAL v0.4 全文、`architect-review-r1.md`（R1 全文 + R2 节 + R3 节）与 PROPOSAL §7 三条 verdict、§8-A/B/C/D/E。
> **纪律**：只读全仓 canonical；唯一可写面 = 本文件 + PROPOSAL §7 append。一切事实断言 Read/Grep/Bash 实测，**不凭 PROPOSAL 或前轮记录转述采信**——本轮焦点涉及的数字全部独立重算。

---

## Verdict：GREEN（零 findings）

R3 预告的两项复核面全部通过：R3-F1 的 verbatim 替换句已 byte 级落位、改正后的新短语本身经独立取证为真、§6 与 §4 的同事件归属矛盾消解、census 三处数字经本轮独立重算逐数吻合；v0.2/v0.3/v0.4 三轮改动的交叉自洽五处扫描全部闭合，未发现前轮漏审的同类残留（新实例独立扫描面见焦点二各条与「范围外观察」）。结构字段在场。**无升级 human 项。**

profile（G1+G2+G3）、G1 归类、Track B 降档、riskTier low-medium：本轮亲核**未发现反证**，按派遣纪律不重开，维持 R1 起三轮复核的稳定结论。

---

## 焦点一 · R3-F1 落点到位性（四项逐条）

### ① 替换句落位与旧短语残留 —— ✓ 过

- **byte 级比对**：python 断言 `expect in L211` → **True**。§4 OQ3(b)（`:211`，§4 节界 = `:205`–`:212`）现文括注与 R3 判官 verbatim 替换句逐字一致，含 `` `subsystem=server` `` 反引号形态：「gateway-directionB（引擎方向 B 定向包，refs 引 §四端云协议剖面——`subsystem=server` 单面标签、业务实跨端）」。
- **旧短语全文件扫描**：`OpenClaw 拒 tools 数组那次` 命中仅 `:281`（§7 R3 verdict 历史记录，引 finding 原文）与 `:348`（§8-E 回灌表 old→new 记录）——两处均 append-only 历史区，不改属正当。**§4 规范文本零残留** ✓。

### ② 改正后的新短语本身是否为真 —— ✓ 三要素独立亲核，全部为真

**证据源 = `.claude/taskpackages/260903-gateway-directionB/taskpackage.json`（本轮 Read 全文）+ 契约标题 + 契约节结构，不默认前轮已证：**

- **「refs 引 §四端云协议剖面」**：该包 `docRefs` 含 `docs/contracts/interface-contract.md#§四`（taskpackage.json `:62`）、`sourceRefs` 同锚（`:67`）；契约 `:223` 标题实测 verbatim「`## 四、端云协议剖面（小智协议消息子集，2026-09-03 新增，方向 B）`」——引 §四 ∧ §四 即「端云协议剖面」，成立 ✓。
- **「`subsystem=server` 单面标签」**：taskpackage.json `:12` 实测 `"subsystem": "server"` ✓。
- **「业务实跨端」（独立找证，第三点按派遣要求不采信前轮）**：该包 acceptance 自身三条跨端证据——GWA-A2「xiaozhi-esp32-server 在 ECS 部署并运行，**wss 端口设备侧可达**」、GWA-A6「kidpal/display 与 kidpal/alert 两种载荷经 JSON-RPC tools/call 下发、**设备回执回传闭环**」、GWA-A7「…**设备侧消费验收归包 2**」；story 首句「**方向 B 流式网关 bring-up**（tracer-bullet 服务端片）」。spec.md「方向 B」实测 13 处，定义处 verbatim「端云通信由『MVP 同步 HTTP』转为 **WebSocket + Opus 流式（方向 B）**」——「引擎方向 B 定向包」与「单面标签、业务实跨端」两个描述语均有据 ✓。

### ③ §6 留根 (c) 档与 §4 现文矛盾消解 —— ✓ 过

- **g-relay-fix notes 亲核**：`.claude/taskpackages/260907-g-relay-fix/taskpackage.json:127` notes 实测 verbatim 含「live 引擎 Intent:nointent（OpenClaw 拒 tools 数组，WS 包 SHOULD #2）」——§6 (c) 档「taskpackage notes 即记」的归属为真 ✓（§6 引文把原文括号渲染为逗号，见「范围外观察 (4)」微注，零事实影响）。
- **「WS 包 SHOULD #2」落点亲核**：`changes/dev/260903-device-ws-switch/review-packet.md` Findings 表实测三条 SHOULD（`:75` ASR 凭据 / `:76` OpenClaw 拒 tools 数组 / `:77` docs-sync），`:76` 恰为第二条，verbatim「引擎 `Intent: nointent`（工具调用关闭）——OpenClaw 拒绝 tools 数组（`invalid tool configuration`），tracer-bullet 为串通首链先关意图/工具」✓。
- **反向排除**：`changes/dev/260903-gateway-directionB/` 全目录 grep「tools 数组|nointent」**零命中**（本轮复跑确认）——该事件确不归属 gateway-directionB ✓。
- §4 现文括注已不含任何 OpenClaw 事件归属，与 §6 不再存在同事件异归属 → **矛盾消解** ✓。

### ④ census 三处独立重算 —— ✓ 逐数吻合（第三次重算，不客气）

本轮以 T2 同源谓词（锚点绑定 `docs/contracts/interface-contract.md` 文件名 ∧ 节 ∈ {§1.5, §三, §四}，字段面 = sourceRefs/contractRefs/docRefs/acceptanceTestsFile 递归取串，排除 `current/` 指针目录）python 全量重跑，实测输出：

```
raw glob count (incl current): 20
packages (excl current/): 19
dual (firmware+server): 8 / 19
dual names: ['260829-char-stroke', '260829-hardware-validation', '260829-schedule-p2',
             '260829-schedule-reminder', '260829-smoke-e2e', '260829-story-p3',
             '260829-voice-roundtrip', '260903-device-ws-switch']
refIC (any ref to interface-contract.md): 19 / 19
wire hits: 5 / 19
  260829-schedule-reminder   firmware+server  ['§三']          dual=True
  260829-voice-roundtrip     firmware+server  ['§四']          dual=True
  260903-device-ws-switch    firmware+server  ['§四']          dual=True
  260903-gateway-directionB  server           ['§四']          dual=False
  260907-g-relay-fix         server           ['§1.5', '§三']  dual=False
wire-dual: 3   wire-non-dual: ['260903-gateway-directionB', '260907-g-relay-fix']
```

**与文内三处逐数对照**：

| 断言处 | 文内数字 | 本轮实测 | 判定 |
|---|---|---|---|
| §1 P2 末条（`:101`） | 引 wire 节 5/19；成员五包（节归属与双面标注）；双面 3；非双面 2；净增覆盖 2/19 | 5/19；五成员、节归属、双面标注逐一相同；3；2；2/19 | ✓ |
| §3 硬项 1b（`:201`） | 双面 8/19、wire 5/19、非双面 2/19 = g-relay-fix + gateway-directionB | 8/19（八包名单与 §1 P2 `:98` 名单逐一相同）、5/19、2/19 同成员 | ✓ |
| §4 OQ3(b)（`:211`） | wire 5/19、(b) 只抓双面 3、(a) 净增 2/19 = g-relay-fix + gateway-directionB | 同上 | ✓ |

**附带 spot-check（gateway-directionB / g-relay-fix 相关的全部规范面数据点）**：g-relay-fix `subsystem=server`、`rigor="lite"`、全文件含 "firmware" 字符串扫描 = **False（零命中）** ✓（§1 P2 `:99`）；§1 P1 表唯一 gateway-directionB 遥测归属 `e4b70402` 实测 = dispatch-completed / builder / 260903-gateway-directionB / **phase=token-plan-probe / 42.2min / 97 tools** ✓（与表行逐字吻合——R3-F1 同类「事件归属错误」在 P1 表无残留）；契约节标题 §1.5「网关的职责」`:138`、§三「提醒推送与出队语义」`:205`、§二「B1 课表 MD 结构」`:153`（T2 实现期评估清单的 §二 定性一致）✓。

---

## 焦点二 · 无新矛盾扫描（v0.2/v0.3/v0.4 交叉自洽，五处）

### ① brief 生成时点六处表述 —— ✓ 闭合，规范面零「Step 9 生成」读法

六处逐一实测现文：T2 头（`:132`「生成时点 = planner Step 7 派发前一次产双份…不依赖任何 finalize-only 字段」）、T2 advisory WARN 宿主（`:139`「WARN 唯一宿主时点 = Step 7」）、T3.3 括注（`:147`「唯一定义生成点 = planner Step 7 派发前一并产出…Step 9 只引用」）、T4 planner 第一条（`:157`「Step 7 前置『一次产双 brief』…Step 9…只引用已在场的 input-brief-builder.md 路径（不再生成）」）、T6 注册文（`:174`「生成时点 = Step 7 派发前一次产双份」）、§3 表新脚本行（`:193`「Step 7 前置一次产双份、Step 9 只引用」）——**全部同一口径**。全文件「Step 9」共 9 处：规范面 4 处（`:139/:147/:157/:193`，均为只引用/只在 Step 7 生成语义）+ 历史面 5 处（`:242/:267/:279` §7、`:298/:339` §8，append-only 引述旧 finding，正当）。**WARN 唯一宿主 = Step 7 的逻辑独立复核为真命题**：WARN 前提 =（引 wire 节 ∧ counterparts 空 ∧ 无 exempt），Step 8 硬前置强制二选一后，即便 Step 9 条件重跑因 refs 改动**新增** wire 节引用，前提的合取支「counterparts 空 ∧ 无 exempt」恒假 → WARN 不可再触发 ✓。

### ② V5 六臂 ↔ T1/T4 互为可满足 —— ✓ 闭合，F7 型互斥未复现

- **V5①**：DEV-GOVERNANCE 实测「五条前置清单」恰 **1 处**（`:291`，头注末句，T1 承重替换锚 verbatim 吻合「…跨 4 包 5 子项实证）。五条前置清单：」）、「六条前置清单」**0 处** → T1 单点替换后两齿（≥1 / ==0）同时满足 ✓。
- **V5②**：T1 第六条要素 1 含 `input-brief` 字面、要素 3 含 `crossEnd` 字面 → 两内容锚由冻结文本必然产出 ✓。
- **V5③**：`.claude/agents/` 实测 pattern `五条前置清单|五条中的` 命中恰 **3 处**（planner.md`:109`「起包五条前置清单」、portfolio-planner.md`:58`「等五条前置清单」、acceptance-author.md`:46`「（起包清单五条中的②③）」），**全部在 T4 改写集内**（三处分别指定「五条→六条」计数词替换）；builder.md`:46` 实测不含「五条」字样（现文「消费 Draft TaskPackage 的 mutationEvidence 骨架与验收 ID 形态…」）与 T4 声明一致 → landing 后零命中可达 ✓。
- **V5④**：T4 冻结文本为四 def 各注入 `input-brief` 字面——planner（「input-brief-builder.md」路径 + Step 8 前置「两份 input-brief 在场」）、portfolio-planner（R2-F2 最小句形「`crossEnd` 骨架与 `input-brief` 同源…」）、acceptance-author（Inputs 改「`input-brief-acceptance-author.md`…」）、builder（Inputs 改 `input-brief-builder.md`）→ `grep -l … | wc -l == 4` 必然产出，无执行者自由发挥位 ✓。
- **V5⑤**：`:122` 实测现文枚举 = `{frictionType: stall|scope-drift|scan-miss|degraded, summary}`，T7 第一条扩 `over-scan` → 齿可满足 ✓。**V5⑥**：T6 注册 input-brief 入 dev-loop.md 文件地图（`:72`「## 2. 文件地图」实测在场）✓。
- 六臂与 T1/T4/T6/T7 全部互为可满足，无一处「按 T 字面落即 V 红」✓。

### ③ V3 正态 fixture 清单 ↔ 闸实际必填 —— ✓ 逐项吻合（多列漏列皆无）

`taskpackage-required-check.mjs` 本轮 Read 全文，**无条件必填面**实测枚举：origin.type∨sourceType 至少一（`:161`）+ 八标量 nonEmptyString（`:163-164`：changeId/subsystem/story/redBaselineCommand/redBaselineResult/deliveryImpact/planningImpact/riskTier）+ 七数组 nonEmptyArray（`:167-168`：acceptance/acceptanceTestsFile/exitCriteria/contractRefs/docRefs/sourceRefs/gateProfile）+ iterationBudget/fixBudget 数值（`:247-248`）+ docsOwner=integrator（`:250-251`，docsSyncRequired=false 时豁免——V3② 列 docsOwner=integrator 为安全侧，非矛盾）。**V3② 清单与此逐项对得上，无条件必填零漏列、零多列**。条件红项核对：枚举值有效性（`:192-196`）、并存一致性（`:199-205`）、planningRefs 联动（`:232-245`）、decisionCoverage/proposal back-check（`:254-347`，非 proposal 来源且不带 DC 数组则不触发）、changeId 前缀（`:208-230`，V3② 已显式钉「带合法 YYMMDD- 前缀避 `:208-230` 前缀红」——行号范围实测吻合）、canonical↔current 一致性（`:131-151`，**仅裸跑触发**；V3 显式 argv → `explicitInvocation=true` 跳过，且 `:23-24` `resolve(projectDir, explicit)` 实测支持仓外绝对路径——V3① 落点规约与闸行为一致）。「照抄 planner.md TaskPackage 格式示例」覆盖枚举值合法性与 planningImpact=none 联动。**T3.2 引锚复核**：`:167` 七数组循环含 acceptanceTestsFile（「本就无条件要求非空」为真）；`:292` 实测 `entry.reason.trim().length < 12`（消息文本续 `:293`，引 if 起始行为判据位，成立）✓。

### ④ §8-B1/B2 五处口径 —— ✓ 同一口径，无一处写死另一处写活

- **B1（counterpart fail-loud）三处**：T2②（`:135`「path 不存在或 symbol 零命中 → 脚本非零退出并点名条目，绝不静默产出空锚」）、V1①（`:215`「path 不存在 / symbol 零命中 → 非零退出且 stderr 点名条目」）、§8-B1（`:315`「path 不存在 / symbol zero 命中 → 脚本非零退出点名条目，不静默产空锚」）——同条件（两触发支）同行为（非零退出 + 点名），全为硬规约措辞 ✓。
- **B2（exempt ≥12 字符）四处**：T3.2（`:146`「长度地板 ≥ 12 字符，对齐本闸既有同型判据 `:292`」+ verbatim 引文）、T5（`:168`「`exempt` 理由长度地板 ≥12 字符（对齐闸 `:292` 同型判据，见 T3.2 与 §8-B2）」+ R2-F5 理由句）、V3（`:217` 正态「`exempt: "<≥12 字理由>"`」+ ④ 负例「`exempt: "—"`（< 12 字）→ 仍 RED」）、§8-B2（`:316`）——同一地板值、同一数源（闸 `:292` 实测 `< 12` 判据在场）、起草侧（T5）与闸侧（T3.2）与验证侧（V3④）三投影齐 ✓。

### ⑤ T7 over-scan ↔ T1 激励措辞 ↔ `:122` 现文格式 —— ✓ 接得上

- **枚举形态**：`:122` 实测为关键词联合 `stall|scope-drift|scan-miss|degraded` → 扩 `over-scan` 零迁移成本（OQ1(a) 论据「枚举是关键词形态」为真）✓。
- **触发注记同型**：`:122` 现文 stall 型已有启发式触发注记（「`stall` 由 dispatch-log 的 durationMs/toolUses 比启发式触发」）——T7「触发定义钉进同一条目…对照 `:122` 现文 stall 型已有启发式触发注记，本条同型」与现文格式接得上 ✓。生产者三件齐（触发条件 = 申报段非空、生产者 = planner、summary 形状 = 引申报行数与代表文件），死 canon 风险消除。
- **DP-D4 未破**：`:122` 现文 verbatim「**纯 advisory、绝不当门**（DP-D4）」在场，T7 明文「维持 `:122` 既有定性…over-scan 只记账不拦截」——定性零改动 ✓。
- **激励措辞逐字同**：T1 第 2 条（`:123`）「**超读是常态动作，不申报才是问题**」与 T7（`:180`）所引逐字一致，申报→记账→仪表盘闭环两端口径同一 ✓。

---

## 结构字段核对（随 GREEN 结论执行）

- **landing-manifest 围栏块在场**（`:12-24`）：**11 行**，与 T1–T7 目标文件精确闭合（T1:1 + T2:1 + T3:1 + T4:4 + T5:1 + T6:1 + T7:2 = 11，无孤儿、无缺项），v0.1→v0.4 零改动；
- **packet-id 行在场**（`:6`：「packet-id = 本 PROPOSAL.md §2 改法集（T1–T7）+ §5 验证集」）✓。

## Findings 表

**无**（0 findings）。两项复核面全过，独立扫描未发现前轮漏审的同类残留或新引入矛盾。

## 范围外观察（非打回权内，不升 severity）

1. **pre-merge GREEN 机锚处置判定 = 恰当（另登 ARQ，不并入本批）**。本轮亲核 `pre-merge-escalation-gate.mjs:83-84`：注释意图「宽松匹配『GREEN』且『verdict』同段（拒绝仅 history 提及 GREEN）」，实装 `/verdict[：:]\s*GREEN|结论[：:]\s*GREEN|GREEN/gi` 含裸 `GREEN` 分支——本批 PROPOSAL 自 DRAFT 起正文即含 "GREEN" 字样（`:10` 结构条款句、§7「三轮未 GREEN」等），该关机锚在本批恒真，**自证其害属实**。主线程处置（§6 留根 + 另登 ARQ + 修法方向含 self-check 反例 fixture）恰当：本批 manifest 零触该脚本，并入即混两件不相干变更同 packet = scope creep；留根纪律（同 §6 frictionType environment 漂移先例）适用。**§7「NEEDS-WORK/GREEN」字样对本轮 verdict 机锚可读性的影响判定**：裸分支使机锚**无法区分**本轮 GREEN 与任何历史 NEEDS-WORK（恒真），但该失真先于本轮存在、非本轮 verdict 造成；且本轮 verdict 行「verdict：GREEN」恰同时命中**窄分支** `verdict[：:]\s*GREEN`——ARQ 修法方向（收窄到 verdict 行/段形态）落地后本轮 verdict 仍可被正确机读，无需本批任何额外动作。merge 安全面由 FROZEN 行 + landing-verdict 在场 + green 三锚共同把守，支持 ARQ 尽早修（R1 曾提可并入 260908 merge-chain 邻批，ARQ 登记保留该裁量空间）。
2. **T4 Step 9 重跑触发词的窄松动（非矛盾，供 retro）**。重跑触发条件钉为「若 Step 8 之后 **refs** 有改动则重跑覆盖」，未含「crossEnd 在 Step 8 才被补填/改动」分支：若 planner 违背设计时点（T4 Step 1 intake 跨端判断 + portfolio 预填 + Step 7 WARN 复核均前置于 Step 8）把 counterparts 迟填到 Step 8 且 refs 未动，Step 9 引用的 brief 第②路为空骨架快照，机械存在性（T3.3）不拦。**不构成 finding**：各冻结条款可同时满足（非 F7/R2-F2 型），R2 已定性「新鲜度是 Step 9 纪律」且该定性在 R3 WARN 宿主论证中自洽成立；暴露面 = planner 无视 WARN 的纪律偏差窄路径，T2②(b) 消费期重 grep 语义与超读申报提供可见性。**不建议改冻结文本**（landing 执行者扩写触发词即偏离 frozen PROPOSAL）；建议 retro 记录，pilot 期凭 T7 仪表盘观察是否实际发生，再议是否扩为「refs 或 crossEnd 改动」。
3. **熔断记录链建议（非 finding）**。§8-E 明言「human 裁断在 §7 之后另行登记，本表不预判」——human 三选一 = ③ 的裁断目前载体 = 派遣 prompt 与本轮 R4 标题注「熔断后新判官实例」。建议主线程标 FROZEN 时在 Status 行补一句 human 三选一裁断（③，2026-09-09）登记，闭合熔断条款的记录链（属 FROZEN 授权动作，非正文改动）。
4. **引文标点微注（零级，不要求改）**。§6 (c) 档引 g-relay-fix notes 时把原文括号「Intent:nointent**（**OpenClaw 拒 tools 数组，WS 包 SHOULD #2**）**」渲染为逗号连接（引号内内容词逐字在场，仅两处标点归一）。零事实影响、三前轮均经手未标，本轮如实记录、**不构成任何级 finding，不要求修改**。

## Coverage · 未审面诚实列

1. **§1 P1 遥测全表未重跑**（91/13/47 与八 actor 行）：R2 曾以复现命令原文逐字重跑逐行一致；本轮按派遣范围（两项复核面，非全量重核）只抽核与焦点相关的 `e4b70402` 单点归属与 census 涉及的全部字段面，全表未三度重跑。
2. **§5 V4/V6 基线数字未再验**（story-p3 aa 四条 113/8/18/19、三 eventId、fixer 13 包 3 次）：R1/R2 双轮已验，不在本轮焦点。
3. **§1 P3 裸跑 eventsTotal=33 / current 指针指向**未重跑（R1 已验，V4「显式带 --change-id」纪律条文自洽性本轮文面核过）。
4. `brief-inputs.mjs` 为新增脚本，无可审实现——只审接口 spec 与四路展开语义的冻结文本自洽（承 R1 coverage）。
5. T7 聚合列实现未写，`:122` 现文与现输出形状中本轮只再验 `:122`；`pod-events-aggregate.mjs` 现输出四块形状未重跑（R1 实跑过）。
6. 四 agent def 除 T4 引用行外的其余正文、`scope-map.json` 全量、`.claude/scripts/lib/` 未全量重核（承 R1 coverage 声明；本批声称零触碰面与 manifest 一致性本轮文面核过）。
7. §8-A/B/C/D 历史回灌表未逐条重核（append-only 历史，R2/R3 已对账）；本轮核 §8-E ↔ v0.4 正文一致（替换记录与实文吻合 ✓）+ §8-B1/B2 ↔ 正文五处口径（焦点二④ ✓）。
8. V6 pilot 回看为交日落项，不预审。

## 亲核留痕（本轮实测命令与输出摘要）

- python byte 比对：替换句 `in` L211 → True；替换句全文件出现 2 次（`:211` + §7 R3 记录）；旧短语命中 `:281/:348`（皆历史区）；§4 节界 `:205-212` 由 `## ` 标题扫描确定。
- Read 全文：gateway-directionB taskpackage.json（subsystem `:12`、docRefs `:62`、sourceRefs `:67`、story `:13`、GWA-A2/A6/A7）；taskpackage-required-check.mjs 全文（必填面 `:161/:163-164/:167-168/:247-251/:250-251`、条件红 `:192-205/:232-245/:254-347`、前缀 `:208-230`、current 校验 `:131-151`、argv 解析 `:23-24`、`:292-293`）；device-ws-switch review-packet `:65-89`（SHOULD 三行 `:75/:76/:77`）；pre-merge-escalation-gate `:72-91`（`:83-84`）。
- python census 重跑：20 glob → 19 包；dual 8/19 名单；refIC 19/19；wire 5/19 五元组（输出块见焦点一④）。
- python 单点：e4b70402 → gateway-directionB / builder / token-plan-probe / 42.2min / 97tools；g-relay-fix rigor=lite、全文件 "firmware" → False；dev-loop.md `:229` 含「承重字段/decisionCoverage 校验由 closeout 相位程序化调用」+「F-04」→ True；spec.md「方向 B」13 处（定义 = WebSocket + Opus 流式）。
- grep：契约标题结构（`:138` §1.5 / `:153` §二 / `:205` §三 / `:223` §四）；PROPOSAL「Step 9」9 处分类；DEV-GOVERNANCE「五条前置清单」恰 1（`:291`）/「六条」0；agents/ pattern 恰 3 处；gateway-directionB changes/dev「tools 数组|nointent」零命中；g-relay-fix notes `:127` verbatim。
- Bash 行打印：planner `:54/:55/:56/:57/:109`、portfolio-planner `:58`、acceptance-author `:24/:29/:46`、builder `:24/:46`、dev-loop-operating-system `:122`、dev-loop `:72/:125/:229`、task-intake-and-slicing `:211/:217`、DEV-GOVERNANCE `:212/:289/:291/:293/:296/:297`——T1/T3/T4/T5/T6/T7 全部行号锚逐一命中现文。
