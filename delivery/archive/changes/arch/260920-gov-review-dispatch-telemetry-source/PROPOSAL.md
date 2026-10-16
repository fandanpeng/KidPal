# PROPOSAL · 260920-gov-review-dispatch-telemetry-source

> **状态：FROZEN v1.0 · 2026-09-20 · 可进 landing**（凭据：**human 2026-09-20 立项 ack + 三问全裁**〔范围 a+b+c 一并 / 通道采 transcript 作权威源 / 轨级改判全批 Track B——见 §4 裁决表〕+ **architect R2 GREEN**〔§7 `### R2`，`结论：GREEN`，2 条 advisory 不阻；R1 `打回`〔3 SHOULD + 3 COULD + 2 NIT〕→ v0.2 全折入 → R2 逐条判为**真修复**（回病根判，非「看起来改了」）〕+ **标冻前折入 R2-N3 / R2-N4**〔§7 `### v0.2 标冻前折入`；两条均为 advisory、不阻 GREEN，折入理由 = 二者皆防**静默错值**而非措辞：N3 防「harness 单值」与四路旗标被读成一回事，N4 防 `message.type` 谓词静默得 0〕。**标冻前折入项属标冻前修订**，故不适用「FROZEN 后改 packet 内制品须记 Status 行」那一条——本行即其记录〔承批③ / 批④ 同款先例〕。**R1 → R2 两轮均无升级 human 项**〔含判官对 human 轨级改判的表态：**可接受、不升级**，见 §7 `### R1`〕。
>
> **FROZEN 后 packet 内修订 2 处**：（一）**2026-09-20，landing 期**：`payload.mainThreadUsage` 的在场条件由「无条件带键」明确为**「仅命中派发 transcript 时带该键」**——若无条件带，老调用序列会凭空多出一个恒 `unavailable` 段，**使 §5 #4「逐字段一致」字面不成立**（该矛盾由验证集 V4 自己抓出）。**属取本件既有约定的同一形态**（`--model` 注释原文「不传则 **不带该键**（向后兼容…）」），**不扩不改任何改法内容、六条目标路径一字未变**；详见 `architect-landing-summary.md` §3 落地期发现 1。
>
> （二）**2026-09-21，合 main 后缺陷修正**：T1 的 `dedupeByMessageId` **去重方向取错**（first-wins，应为 **last-wins**）——transcript 按流式分块逐条写且 `usage` 渐进更新，**真值只在末块**，取首块会把 `output` 与 `total` **系统性低报且静默**（实测判官 `output` 0 → 62,043、`total` 144,986 → 207,029，**input 两次相同**）。**这是本件唯一一处实现层缺陷**，与 §2 T1「按 `message.id` 去重」的目标一致、只是方向取反，故**不属改法变更**；修法 = 改 last-wins + 夹具由「等值块」改「渐进块」（等值块下两种取法等价、对方向恒真）+ 两条方向断言 + 反向控制。**由 human 授权走一次性快速路径**（未走完整 Track B 环，按 AGENTS.md 如实记「跳过流程」，见 `retro.md` §6）。六条目标路径仍一字未变。
> **承载 ARQ**：`ARQ-260909-dispatch-stamp-telemetry-gaps`（三条子项 a / b / c **全收**——取件复证发现三者由同一个机制解决，见 §0）。不关的根写 §6 留根。
> **review 轨**：**Track B 常驻轻路径**（SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。**轨级由 human 立项时改判**：ARQ 原文对本项子项 c 标注「触及时转 `full-adversarial + human`」，human 2026-09-20 立项裁定**全批走 Track B**，理由原话口径为「即使从 harness 那边拿不到，也只是没有 token 计数；我们现在本来很多都没有 token 计数，没有太大损失」。判官的升级判据据此**不适用**本批；若判官认为该改判本身构成风险，按既有条款打回升级。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium。证成：全部改动为**只读派生 + 只增字段**，不新增门、不改任何 gate 判定；既有消费方按 `status` 取值、与 `source` 正交，向后兼容；唯一新增风险面是引入一条**仓外、未文档化**的读取通道，已由「取不到即降级 `unavailable`、绝不禁用派发、绝不当门」三条兜底（§2 T1 改法、§5 验证集第 3 条）。
> **review profile**：G3 + G2。触面枚举：G3 = `.claude/scripts/dispatch-stamp.mjs`、`.claude/scripts/lib/transcript-telemetry.mjs`（新增）、`.claude/scripts/pod-events-aggregate.mjs`；G2 = `.claude/agents/planner.md`（条款投影，**不动 frontmatter**）、`.claude/references/dev-loop-operating-system.md`、`.claude/references/dev-loop.md`。**G1 / 契约面为零**：不触 `docs/contracts/**`、不触 `docs/requirements.md` / `docs/architecture.md`、非 gate 变更、不动 scope-map · settings · hooks。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/lib/transcript-telemetry.mjs
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/pod-events-aggregate.mjs
.claude/agents/planner.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：cadence 复盘的 **token 账**变得可归因——每次派发带上四路 token 分项与**实际生效模型**，主线程自身的 **token** 消耗也不再是零观测。

> **口径界定（R1-S3 折入）**：ARQ 行里的「84% 黑箱」是**挂钟**口径（原句「13 包 134.7h 挂钟仅 21.6h 可归因」），**本批不解决挂钟归因**——`mainThreadUsage` 只提供主线程的 token 四路分项，「某段挂钟可不可归因」是另一个量。本批**不得**被读作已闭合挂钟黑箱；§5 第 9 条的判定基准据此收窄为 token 账。

**现状差距**：`dispatch-completed` 事件只记一个 harness 回传的单值（口径未文档化、与逐消息账对不上），主线程自身消耗一个字都没有。2026-09-20 现跑清点（命令与输出见 §3 census 段）：去重后 199 条 `dispatch-completed` 中，**57 条压根没有 `tokenUsage` 键**、11 条显式 `unavailable`、127 条只有单值 `total`、**仅 4 条有分项（且只有 `input` 一路）**、**带 `model` 的 0 条**。

**取件复证改写了这个 ARQ 的构成**（这是本批最重要的一条，判官请优先核）：

| ARQ 子项 | ARQ 原建议 | 复证实况 |
|---|---|---|
| a · 实际生效模型 | 「补记实际生效模型入 stamp」 | **半成品**：`260920-gov-review-reader-scripts-align` 已落 `--model` 旗标，但该旗标只能喂进「**请求的**模型」（槽位名），喂不了「**生效的**模型」；且全量 199 条事件中 `model` 字段实际使用 **0 次** |
| b · token 分项 | 「token input/output/cache-read/cache-write 分项入 payload」 | **按此建议改是改不出来的**：旗标面（`--token-input/--token-output/--token-cache-read/--token-cache-write/--token-total`）、`planner.md` 调用行、脚本校验逻辑**三处都已在位**。缺口**不在脚本侧，在上游**——harness 只回单值，调用方永远拿不到分项。脚本侧再改也补不出分项 |
| c · 主线程自身观测 | 「含真设计分叉（harness 无回传，**估算 vs 不采** 待定）」 | **两个选项之外存在第三条路**：主会话 transcript 逐消息落 usage，主线程消耗**本来就能直接读**，既不用估算也不用放弃 |

**⇒ 三条子项塌缩成一个机制**：读会话 transcript。这也是 human 立项时裁「a+b+c 一并做」的依据。

**非目标（本批不做什么）**：

- **不改 harness**，不追查 harness `subagent_tokens` 单值的口径根因——本批只**把它记下来供交叉核对**，不解释它。
- **不追溯回填**历史上那 51 条自造 `dispatchId`（`acceptance-author-r1` 之类）的事件。这些 id 不是 agentId，与 transcript 无连接键；据时间戳反推属于猜测，本批不做（§6 留根）。
- **不把 transcript 派生数据当门**：新增字段全部是记账 / advisory 面，不进任何 gate 判定、不改 merge 门、不改 verify 闸。
- 不动 `docs/contracts/**`、不动 scope-map / settings / hooks、不动任何 agent frontmatter。

---

## 2. 问题、根因与改法（T1…T5）

**【硬项 · 起草期实证义务】** 本节的断言纪律：

- 凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；无实测不写断言，写「未实测」。
- 凡声称「对偶 / 比照 <先例>」→ 先读先例文件，引 file:line verbatim 确认形态再落笔。
- 凡「全 / 均 / 无」量化声明 → 每次改写其覆盖集合时，对新入集成员重新取证并留痕。
- 凡引用 commit SHA → 先 `git rev-parse <sha>` 核实存在，输出留痕。
- 凡数字引用（计数 / 行号 / 版本号等）→ 起草期现跑实测（命令 + 输出留痕）或核对权威原处；不得转引陈旧制品数字，确需转引时显式注出处与复核结果。
- 凡引用脚本 / 闸 / CLI 命令入口（如 `--self-check`、闸调用命令）→ 起草期亲核入口存在（实跑一次，或至少验证该入口在脚本帮助/源码中），不得凭印象构造。
- 搬运 / 改写既有文本中的引据（file:line / 编号 / 数据点）视同新断言复证——版本迭代搬运期的旧引据漂移不得免检。

**本批已完成的起草期实证汇总**（命令与逐字输出见各 T 的「证据」段；全部为 2026-09-20 现跑）：

| 编号 | 实测对象 | 结论 |
|---|---|---|
| E1 | 两发探针（前台 / 后台各一发，`general-purpose` / `model: haiku`） | harness 回传仅 `<usage>subagent_tokens / tool_uses / duration_ms</usage>` 三项，**无分项、无模型**；transcript 侧逐消息 `message.usage` 含 `input_tokens / output_tokens / cache_read_input_tokens / cache_creation_input_tokens` 四路 |
| E2 | 同一次派发的两个数对账 | 前台：harness 36,560 vs transcript 逐消息合计 36,829（input+output）；后台：harness 31,824 vs transcript 合计 26,961。**偏差方向不一致**，对不上任何可推导公式 ⇒ ARQ「口径不明、不可用作账」成立 |
| E3 | transcript 去重陷阱 | 同一次 API 响应的 usage 按 content block **落两遍**（`apiBlockIndex` 0/1，数值相同）；按 `message.id` 去重后前台从 6 条记录收敛到 3 条消息。**不去重即虚增一倍** |
| E4 | 连接键 | `dispatch-stamp.mjs` 现有的 `--dispatch-id` **就是 harness 回传的 agentId**：从 pod-events 取 3 个真实 dispatchId，**3/3** 命中磁盘上的 `agent-<id>.jsonl` |
| E5 | 生效模型可观测 | 探针请求 `model: haiku`，transcript 记 `message.model = deepseek-v4-flash`——**正好复现 ARQ 说的槽位映射失真**。**请求值另有一份结构化载体**：同一派发的 `agent-<id>.meta.json` 逐字为 `{"agentType":"general-purpose","description":"Harness usage probe","toolUseId":"call_00_tuNw8SPwgMJwkn5kkVkH5411","spawnDepth":1,"requestShape":"foreground","requestNonInteractive":true,"model":"haiku"}`（`model` 键**仅当调用方显式传了 model 覆盖时在场**，未传则整个键缺失）。**R1-N1 判本载体名不实，主线程举证反驳——见 §7 v0.2 修订记录 N1 条** |
| E6 | 覆盖面 | 全机 `agent-*.jsonl` 共 **397** 份（**时点快照，2026-09-20 起草期现跑**；R1 判官复跑得 **398**，差额来自起草与复跑之间新增的子 agent transcript，含判官自身那一次——**该数随每次派发单调增长，不得当稳定量引用**，R1-N2）；在册 10 种 typed agent（architect 17 / landing-verifier 15 / challenger / defender / adjudicator / scenario-author / architect-reviewer / Plan / Explore / general-purpose）全部有档；前台与后台派发都落盘 |
| E7 | 引据漂移 | ARQ 原文写 `planner.md:57`，现测 MODEL-TIER 条在 **`planner.md:59`**（漂移 2 行）；DISPATCH-STAMP 条在 `:60` |
| E8 | 工作树池复用导致跨目录 | 某包 pod-events 里的 dispatchId，其 transcript 落在**另一个** worktree 的会话目录下（worktree 池复用所致）⇒ 查找**必须跨全部项目目录**，不能假设同目录 |

### T1 · 新增 transcript 读取库

**问题**：仓库里没有任何代码能读到会话 transcript，三条子项全部无处落地；且该通道是仓外、未文档化的 harness 内部产物，若无一层统一封装，读法会散落到各调用点、各自处理失败。

**证据**：全仓现跑 `grep -rn "homedir\|\.claude/projects\|os\.homedir" .claude/scripts/ .claude/hooks/ .claude/agents/ .claude/references/` → **零命中**（2026-09-20）。即：本仓从未读过该目录，不存在可复用的读取器。

**根因**：遥测自建立起只有一条数据源——harness 回传给主线程的 `<usage>` 块；`dispatch-stamp.mjs` 的注释首行即写「把 harness 回传值落盘」，数据模型与调用契约都围着这一个源长出来。上游不给分项时，脚本侧没有任何第二来源可退回。

**改法**：新增 `.claude/scripts/lib/transcript-telemetry.mjs`，导出两个只读函数：

- `readDispatchTelemetry(agentId)` —— 按 agentId 跨**全部**项目目录定位 `<projects>/*/*/subagents/agent-<agentId>.jsonl`（E8：不可假设同目录），读出：四路分项（`input / output / cacheRead / cacheWrite`，`total = input + output`）、**实际生效模型**（`message.model`）、`sessionId`、`messageCount`。**按 `message.id` 去重**（E3 实测的 2× 陷阱）。
- `readMainThreadUsage(sessionId, projectDirName)` —— 读 `<projects>/<slug>/<sessionId>.jsonl`，过滤 `isSidechain !== true` 的 assistant 消息，同样按 `message.id` 去重后**累计**四路分项与生效模型。`<slug>` 与 `<sessionId>` 由上一函数的返回值自洽给出（agent transcript 的目录名即 `<slug>/<sessionId>`，记录内亦有 `sessionId` 字段，E4 实测），**不使用「最新 mtime」这类启发式**。

三条硬约束写进实现：

1. **失败即降级，绝不抛出**：文件不存在 / 不可解析 / 字段缺失 / 权限不足 → 返回 `{ found: false, reason }`，由调用方如实记 `unavailable`。**绝不禁用派发、绝不阻断 stamp**。
2. **路径可覆盖**：项目根走 `KIDPAL_CLAUDE_PROJECTS_DIR` 环境变量覆盖（默认 `os.homedir() + '/.claude/projects'`），供夹具测试与异常环境使用。
3. **不做模型名白名单**：E5 实测同一次派发两次观测到 `deepseek-flash` 与 `deepseek-v4-flash` 两个写法（代理层命名），实现只透传字符串、不校验、不映射。
4. **`assistant` 谓词钉死为顶层 `o.type === 'assistant'`**（R2-N4 折入）。transcript 里三个字段并存且**值不同名**——顶层 `o.type`、`message.role`、`message.type`，其中 **`message.type` 的值是 `"message"`、不是 `"assistant"`**。2026-09-20 现跑实测（同一探针文件）：按 `message.type === 'assistant'` 计数 = **0**，按 `o.type === 'assistant'` 计数 = **6**。取错字段即**静默得 0**——不抛错、不报红、不降级，直接产出「主线程零消耗」这个看似合理的错误结论。实现须用 `o.type === 'assistant'`（等价于 `message.role === 'assistant'`），**绝不可取 `message.type`**。

自带 `--self-check`（本仓既有形态：`acceptance-run-check.mjs` / `landing-fidelity-check.mjs` / `merge-driver.mjs` 等 10+ 脚本均有该入口），夹具至少覆盖：正常命中 / 去重后不虚增 / 文件缺失降级 / 自造 id 不命中 / `sessionId` 反推主会话路径。

目标文件：`.claude/scripts/lib/transcript-telemetry.mjs`（新增）

### T2 · dispatch-stamp 接入该库

**问题**：`dispatch-stamp.mjs` 落盘的是 harness 单值，账不可用；主线程自身消耗零观测。

**证据**：2026-09-20 census，199 条去重 `dispatch-completed` 的 `tokenUsage` 键形状分布现跑为 `{"source,status,total":127, "reason,status":11, "input,source,status":4}`，另有 57 条无该键；`model` 字段 0 条。

**根因**：`buildTokenUsage()` 只有两条输入路径——结构化旗标（调用方喂）与 legacy `--tokens`；两条都以「调用方拿得到值」为前提。而 harness 回传本身不给分项，调用方无值可喂，于是 127 条退化成单值、11 条退化成 `unavailable`、57 条连键都没写。主线程消耗则更彻底：它不在任何回传里，没有任何代码路径去取。

**改法**：把 T1 的库接进 `run()`，**只作用于 post-cutoff 分支**（pre-cutoff 走 legacy `dispatch-log.jsonl`，数据模型不同，保持零变更以压风险面）：

- **取值优先级（钉死）**：transcript 命中 → 旗标值 → `unavailable`。命中 transcript 时**必须同时置 `tokenUsage.status = 'exact'`**——`status` 是既有消费方的**唯一分桶键**（`loop-metrics.mjs:242` 与 `pod-events-aggregate.mjs:76` 都只判 `status`、不读 `source`，R1-S1 现读核实）。**只写 `source: 'transcript'` 而不置 `status: 'exact'`，新源会被落进 `unavailable` 桶**，「tokenExact 覆盖度提升」静默落空，且无任何门报红（与本批 §2 T4 自陈的「这类矛盾不会让任何门变红」同族）。此词是本批「目标效果能否被既有消费方计数」的承重前提（R1-S1）。
- 命中 transcript → `tokenUsage` 用四路分项 + `status: 'exact'` + `source: 'transcript'`；该源在**仓内 token 归因**意义上是可用的最忠实记录（模型 API 回传的逐消息 usage），**但不是计费真值**——provider 计费可因缓存折扣等与之不同，计费对账不在本批范围（R1-C1 折入）。未命中 → **原样保留既有行为**（旗标值或 `unavailable` 或省略），既有调用方零变更。
- `payload.model` 取**生效模型**（transcript 优先，`--model` 旗标降为回退），并新增 `payload.modelSource`（`'transcript' | 'flag'`）标明该值从哪来——这正是 ARQ 子项 a 与 `planner.md:59` MODEL-TIER 条「dispatch-stamp 若可得须补记实际生效模型」的欠账落点。
- 新增 `payload.mainThreadUsage`：主线程**截至本次 stamp 的累计**四路分项 + 生效模型（子项 c）。取累计值而非「本次派发窗口内增量」，是因为窗口边界需要一次额外的「派发开始」stamp 才能划，属真设计分叉；累计值在每个 stamp 处形成检查点，retro 取同一包首尾两次 stamp 做差即得该包期间主线程消耗，**无需新增调用点**。
- 保留 `tokenUsage.harnessReported`（可选）：调用方若仍传了 harness 单值，一并记下供交叉核对（E2 那类对账的证据基础）。**该「单值」的点名（R2-N3 折入）**：post-cutoff 面 = `--token-total <n>`，legacy 面 = `--tokens <n>`；**它不是四路旗标**（`--token-input/--token-output/--token-cache-read/--token-cache-write` 是调用方手喂分项的通道、与 harness 回传无关），落地行文须写清这一层映射，防后来者把两者读成一回事。
- `--dispatch-id` 未命中 transcript 且**不像** harness agentId 时，stderr 打一条 advisory（fail-soft、不改 exit code）：这正是历史上 51 条自造 id 的可观测化。

**向后兼容**：所有既有旗标语义不变；不传新参数的老调用序列产出与改前**逐字段一致**（§5 验证集第 4 条用既有事件重放核对）。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改）

### T3 · planner.md 调用方纪律

**问题**：通道的产出率取决于调用方是否把**真 agentId** 传进 `--dispatch-id`，而这条约定在本仓只是**自发形成的习惯**、未成文、无机检。2026-09-20 现跑的分包解析表显示：越靠近当下越好（`260915-m1-k3` 16/16、`260915-m1-n1b` 16/16、`260917-m1-k4-rma8` 6/6、`260917-m2-k3` 8/8、两个 `260920` 包 3/3 与 4/4 全绿），但历史面 51/142 是自造标签（`acceptance-author-r1` / `verify-r2` / `integrator-closeout`，以及形如 `7f3a91c4-...-builder01a` 的伪 UUID），通道对它们完全取不到。同时 `--model` 旗标虽已落地，实际使用 **0 次**。

**证据**：分表 TOTAL 行为 `199 条 / 57 无 id / 91 命中 / 51 不命中`（命令：见 §3 census 段）。`--model` 使用数：`with model 0`。

**根因**：`--dispatch-id` 的**语义从未钉死**——早期把它当「人读的派发标签」用（`builder-r1` 这种名字显然是为读日志方便），后来 harness 回传 agentId 后才逐渐改传真值，但文档没跟上，于是两代习惯并存。`--model` 同理：MODEL-TIER 条说的是「补记**实际生效**模型」，而旗标只能喂**请求**模型，语义错位导致它无处可填、无人愿填。

**改法**：改 `planner.md` 的 `[DISPATCH-STAMP]` 条（现 `:60`）：

- 钉死一句：**`--dispatch-id` 必须传 harness 回传的 agentId**（它是 transcript 的连接键，自造标签会让该次派发在遥测里不可归因）。
- 说明分项与生效模型现由脚本**自动派生**，调用方不再需要（也**不应**）手喂 `--token-input` 系列；`--model` 降为「transcript 取不到时的回退值」。
- 保留现文中「**绝不传 0 充数**」原句不动（§4 的 human 裁定亦要求保留）。

同步改 `[MODEL-TIER]` 条（现 `:59`）末句「dispatch-stamp 若可得须补记实际生效模型」——由「人工补记」改指**自动派生**，否则该句与本批落地后的事实自相矛盾。

**注意**：本 T 是**条款文本投影**，不动任何 agent 的 frontmatter（`model:` 键），也不改派发期的 model 覆盖授权面。

目标文件：`.claude/agents/planner.md`（改）

### T4 · token 政策单源改写

**问题**：`dev-loop-operating-system.md:132` 是 token 字段的**政策单一事实源**，现文把数据源硬钉在 harness 上。若不同步，本批落地即制造「脚本写 `source: transcript`、政策明文只接受 harness」的**同仓自相矛盾**——且这类矛盾不会让任何门变红。

**证据**：现文逐字为——

> `- `dispatch-completed.tokenUsage` 只接受 harness exact 值或显式 `unavailable`。`total = input + output`（两者都有时），cache 字段不重复计入；unknown 绝不写 0，harness genuine zero 必须带 `note: harness-reported-zero`。`

（`.claude/references/dev-loop-operating-system.md:132`，2026-09-20 现读。）

**根因**：该行成文时仓内只有 harness 一个数据源，政策与事实同义；本批引入第二源后政策落后于事实。

**改法**：把该行改为**按来源分档**的表述——`source: 'transcript'`（仓内 token 归因最忠实记录、四路齐全；**非计费真值**）与 `source: 'harness'`（回退 / 交叉核对，单值口径未文档化）并列；**原句的四条实质约束一条不删**（`total = input + output`、**cache 字段不重复计入**、unknown 绝不写 0、genuine zero 带 `note: harness-reported-zero`）——四条为 R1-C2 现读核定数（v0.1 曾误写「三条」而括注列四条，算术不自洽）；新增一句降级纪律：**transcript 取不到时按既有路径降级为 `unavailable`，不得因遥测缺失而阻断或延迟派发**。其中「cache 不重复计入」在 transcript 源下**更承重**——transcript 同时给出 `cache_read` / `cache_creation` 两路，而 `total` 只取 `input + output`，该句正是「cache 两路不并入 total」的政策锚（R1-C2）。

目标文件：`.claude/references/dev-loop-operating-system.md`（改）

### T5 · 消费侧接线与目录行同步

**问题**：新字段若无人消费，账在黑箱里换了个格式，仍然看不见——ARQ 的「便宜验证」指名要 `pod-events-aggregate.mjs` 的覆盖度能反映出来；另有一份脚本文档行描述本脚本能力，不同步即制造陈旧引据。

**证据**：`pod-events-aggregate.mjs:59-103` 的 `aggregateTokens()` 现只输出 `dispatchesTotal / dispatchesWithTokenData / dispatchesUnavailable / tokenTotals{input,output,cacheRead,cacheWrite,total}`，**无模型覆盖度、无主线程消耗**；`.claude/references/dev-loop.md:88` 现文逐字为——

> `| `scripts/dispatch-stamp.mjs` | `--token-status exact|unavailable` + 可选 input/output/cache/total/source/reason；post-cutoff 只写 pod-events `dispatch-completed`，pre-cutoff 保留 legacy `--tokens` + dispatch-log。 |`

**根因**：消费侧与文档行都是围绕「单值 + harness」的旧形状长出来的。

**改法**：

- `pod-events-aggregate.mjs`：**只增键、不改既有键**（该脚本的既有兼容纪律，见其 `:238` 注释「T7 新列（只增键，既有消费方不破）」）——新增 `models`（生效模型分布 + 有值条数）与 `mainThread`（各 stamp 检查点序列 + 末次累计）两组聚合。
- `.claude/references/dev-loop.md:88` 目录行：补上 transcript 派生与生效模型两项能力，保持一句话行格式。

`loop-metrics.mjs:240` 经查按 `tu.status === 'exact'` 取值、与 `source` **正交**，本批**不需改**（归 §3「未触」并注明理由）。

目标文件：`.claude/scripts/pod-events-aggregate.mjs`（改）· `.claude/references/dev-loop.md`（改）

---

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

**census 清点**（2026-09-20 现跑；口径：遍历 `delivery/**` 下全部 `pod-events.jsonl`，按 `eventId` 去重，只取 `eventType === 'dispatch-completed'`）：

```
命令：node <inline 脚本> — 见本文末「census 脚本」段（机械枚举，非人眼清点）
文件数 32 · raw 行 207 · 按 eventId 去重丢弃 8（legacy 重复副本）· unique 199
exact 131 · unavailable 11 · 无 tokenUsage 键 57 · 带 model 0
键形状 {"source,status,total":127, "reason,status":11, "input,source,status":4}
按 actor：planner 38(缺 durationMs 36) · acceptance-author 49(缺 5) · builder 47(缺 6)
         integrator 35(缺 2) · fixer 21(缺 3) · verifier 4(缺 4) · architect-reviewer 3(缺 1) · architect 2(缺 0)
```

> **census 口径注记（防误读）**：`delivery/changes/dev/` 下存有 1 份 legacy 部分副本（`260829-char-stroke`），与 `delivery/taskpackages/` 同名件**内容不同**（11 行 vs 16 行），故清点**必须按 eventId 去重**；不去重会得 207 而非 199。

**dispatchId ↔ transcript 解析率**（同口径，2026-09-20 现跑）：

```
磁盘上 agent transcript 总数 397（时点快照；R1 复跑 398，该数随每次派发增长，非稳定量）
unique dispatch-completed 199 → 无 dispatchId 字段 57 / 有 id 142
  有 id 者中：命中 transcript 91（64.1%）/ 未命中 51
未命中成因两类：(1) 人读自造标签（acceptance-author-r1 / verify-r2 / integrator-closeout…）
               (2) 伪 UUID（7f3a91c4-5b2e-4e8a-9d1f-builder01a / aef7e029-builder-mut-campaign…）
近期包已近全绿：260915-m1-k3 16/16 · 260915-m1-n1b 16/16 · 260917-m1-k4-rma8 6/6
              · 260917-m2-k3 8/8 · 260920-m2-charwindow-card-fix 3/3 · 260920-m2-charwindow-display-fix 4/4
```

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G3 派发遥测脚本 | `.claude/scripts/dispatch-stamp.mjs` | G3（改） | 本批主触点，T2 |
| G3 新增读取库 | `.claude/scripts/lib/transcript-telemetry.mjs` | 新增（G3） | T1，只读派生、无副作用 |
| G3 聚合消费方 | `.claude/scripts/pod-events-aggregate.mjs` | G3（改） | T5，只增键 |
| G3 聚合消费方 | `.claude/scripts/loop-metrics.mjs` | **未触** | 按 `tu.status === 'exact'` 取值（`:240-244` 现读），与 `source` 正交 ⇒ 新源自动被正确计入，无需改动 |
| G3 legacy 分支 | `.claude/scripts/dispatch-stamp.mjs`（pre-cutoff 路径） | **未触** | T2 明确只改 post-cutoff 分支；pre-cutoff 走 `dispatch-log.jsonl`，数据模型不同 |
| G2 调用方条款 | `.claude/agents/planner.md` | G2（改） | T3，`[DISPATCH-STAMP]`（`:60`）+ `[MODEL-TIER]`（`:59`）；**不动 frontmatter** |
| G2 token 政策 | `.claude/references/dev-loop-operating-system.md` | G2（改） | T4，`:132` 单源改写 |
| G2 脚本目录行 | `.claude/references/dev-loop.md` | G2（改） | T5，`:88` |
| G2 派发/过程态描述 | `.claude/references/dev-loop-operating-system.md:110` | **未触** | 该行描述 `dispatch-log.jsonl`（pre-cutoff legacy）字段，本批不改 legacy 面 |
| 契约面 | `docs/contracts/**` | **未触** | 零触点 |
| spec 面 | `docs/requirements.md` / `docs/architecture.md` | **未触** | 零触点 |
| 强制层 | `.claude/scope-map.json` / `.claude/settings*.json` / `.claude/hooks/**` | **未触** | 零触点（本批不新增门、不改闸） |
| 外仓只读依赖 | `~/.claude/projects/**`（harness 会话 transcript） | 新增只读依赖 | **本批唯一新增风险面**，见 §0 非目标与 T1 三条硬约束 |

**【硬项 · sweep 透镜义务】** 本批**不新增门、不新增断言**，故无「新判定 pattern 全仓预跑」义务。上行 census 与解析率表均由脚本机械枚举生成（命令形态见本节 census 段），判官可任取若干行 spot-check。

**【硬项 · 复扫模式完备性先验】** 本批不改退役语面、不做残留清扫，故无模式集完备性论证义务。但 **T4 触及一句「单一事实源」的政策文本**，故按同族纪律完成消费点穷尽扫：现跑 `grep -rn "tokenUsage\|token-status\|tokenStatus\|subagent_tokens\|--token-input\|--token-total\|token-source" --include=*.mjs --include=*.md --include=*.json .claude/ docs/` → 命中 6 个仓内文件（`planner.md:60`、`dev-loop-operating-system.md:132`、`dev-loop.md:88`、`dispatch-stamp.mjs`、`loop-metrics.mjs:240`、`pod-events-aggregate.mjs:71`）+ ARQ 队列行本身。**除 `loop-metrics.mjs` 判为未触（理据见上表）外，其余五个消费点全部落到 T1–T5**，与 landing-manifest 六路径一一对应（`docs/ARCH-REVIEW-QUEUE.md` 为簿记面，落地后随 ARQ 结移改写，不入 packet）。

---

## 4. human 取舍点（OQ）

**本批三问已于 2026-09-20 立项期由 human 一次性裁定完毕，无遗留未决 OQ。** 逐条登记如下（裁定即为本批的立项 ack）：

| 问 | human 裁定 | 背景与理由（human 口径） |
|---|---|---|
| 批的范围 | **(a) a+b+c 一并做** | 依据是取件复证：三条子项实测由同一个机制解决，拆开会让同一段源语义改两遍 |
| 观测通道 | **(a) 采 transcript 作权威源** | transcript 分项写入 pod-events 作主账并标注来源；harness 单值保留作交叉参考 |
| review 轨 | **全批走 Track B**（**改判**） | human 原话口径：「即使从 harness 那边拿不到，也只是没有 token 计数；我们现在本来很多都没有 token 计数，没有太大损失，所以我觉得走 Track B」。**这是对 ARQ 原文标注「子项 c 触及时转 `full-adversarial + human`」的显式覆盖**——判官若认为该覆盖本身构成风险面，按既有条款打回升级，本 PROPOSAL 不预埋结论 |

**评审期若浮现新取舍点**，由判官记入 §7，主线程升级 human 后回灌本表。

**不变项（human 未授权改动，本批不得触）**：禁 0 充数政策、契约面冻结、契约门、landing-verifier 独立核验、机械强制层。

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **库自检非空绿**：`node .claude/scripts/lib/transcript-telemetry.mjs --self-check` → 输出含全部新增自检名、逐项 PASS、退出码 0。夹具须覆盖：
   - **派发侧（`readDispatchTelemetry`）**：正常命中 / **按 `message.id` 去重后数值不虚增**（E3 陷阱的正控——夹具须含同一 `message.id` 的多块记录，断言合计等于单份而非倍数）/ 文件缺失降级 `found:false` / 自造 id 不命中 / 由 `sessionId` 反推主会话路径。
   - **主线程侧（`readMainThreadUsage`）——R1-S2 折入，v0.1 零覆盖**：① **`isSidechain !== true` 过滤正确**（夹具须**混入** `isSidechain: true` 的记录，断言其 usage **未被计入**——只测「全 false 夹具」等于没测过滤）；② **`message.id` 去重后累计正确**；③ **累计求和正确**（含 cache 两路与 `total = input + output` 的关系）；④ 空 principal / 文件缺失降级。
   - **真空绿规约**：自检须先断言夹具输入非空，空夹具判 FAIL 而非 PASS；每条夹具须同时断言「该被计入的计入了」与「不该被计入的没被计入」。
2. **端到端命中**：对一次**真实**派发的 agentId 跑 `dispatch-stamp.mjs`（写入临时 `--task-package-id` 或测试包，避免污染在册包）→ 产出的 `dispatch-completed` 行**必须含** `tokenUsage.status === 'exact'`（**R1-S1 的承重词，缺此项即本批目标静默落空**）+ `tokenUsage.source === 'transcript'` + `tokenUsage.input/output/cacheRead/cacheWrite` 四路 + `payload.model` + `payload.modelSource === 'transcript'` + `payload.mainThreadUsage`。**人读核对（两条正交，缺一不可）**：
   - ① **派发侧**：该行四路数值 == 对**同一 agentId** 直接现算 transcript 的值（同一命令、同一时刻，防「看起来对」）。
   - ② **主线程侧（R1-S2 折入，v0.1 只核了①）**：该行 `mainThreadUsage` 四路数值 == 对**同一 sessionId** 的主会话文件按同口径（`isSidechain !== true` 过滤 + `message.id` 去重 + 累计）直接现算的值；并核其**单调不减**（后一次 stamp 的累计 ≥ 前一次）——累计语义的唯一可判性质。
3. **降级不阻断（本批风险面正控）**：用历史自造 id（如 `builder-r1`）跑同一命令 → 该行 `tokenUsage.status === 'unavailable'` 且带 reason；**退出码仍为 0**；stderr 出现 advisory。**不得因 transcript 缺失而失败**。
4. **向后兼容重放**：取一条既有 `dispatch-completed` 事件的原始调用序列（新参数全不传）重放 → 产出字段集与改前**逐字段一致**（键集比对，非「看着像」）。**前置**：重放输入须非空断言。
5. **聚合消费方兼容**：`node .claude/scripts/pod-events-aggregate.mjs --change-id 260920-m2-charwindow-card-fix` → 新增 `models` / `mainThread` 键在场，且 `tokens` / `actors` / `escalations` / `friction` **四个既有键一个不少、形状不变**。
6. **政策↔实现一致**：落地后 `dev-loop-operating-system.md:132` 已含 `transcript` 源表述，且**逐字保留原句四条实质约束**——`total = input + output`、**cache 字段不重复计入**、unknown 绝不写 0、genuine zero 带 `note: harness-reported-zero`（R1-C2 折入：v0.1 只核了其中两句）；`planner.md:59/:60` 已含 agentId 纪律句、**frontmatter 零 diff**（`git diff --numstat` 核对 `planner.md` 变更只落在正文条款行）。
7. **形态门干跑**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-dispatch-telemetry-source/PROPOSAL.md` → 无形态错（无加粗锚行、无缺锚清单）；并逐 token 核 `anchorUnion` 成员名单，确认六条锚行注记内**未夹带任何路径 token**（260920-gov-review-citation-anchor-residue 读法注第 3 条）。
8. **无门污染**：`git diff --stat` 确认改动集合 == landing-manifest 六路径，**无 scope-map / settings / hooks / contracts 任何改动**。
9. **真机可读性**（landing 后一次）：跑一次真实派发并用 `--change-id` 看 aggregate 输出，确认 retro 硬项 R2 所需四路分项 + 生效模型 + 主线程累计**在一行里可取齐**——这是本批「目标效果」的直接判定。**判定边界（R1-S3 折入）**：本项只判定 **token 账**；**不含挂钟归因**，不得据本项判定本批已闭合 ARQ 的「84% 黑箱」（那 84% 是挂钟口径，见 §0 口径界定）。

---

## 6. 留根清单

- **51 条历史自造 `dispatchId` 不回填**：`acceptance-author-r1` / `verify-r2` / `integrator-closeout` 及伪 UUID 类（`...-builder01a`）与 transcript 无连接键。按时间戳反推属猜测，本批不做。不关的根，登记留待日落复盘；若日后需要，只能靠「同期唯一未匹配 transcript」的启发式，需单独论证。
- **harness `subagent_tokens` 口径根因不查**：本批只把它记进 `harnessReported` 供交叉核对，不解释其算法。E2 的两组对账数据（36,560 vs 36,829；31,824 vs 26,961）作为该问题的现存证据沉淀，供后续专项。
- **verifier 4 条「无 durationMs」的口径误置**：其事件自带的 `reason` 写明「`verify.mjs` 由 Bash 直跑，harness 不回报该子进程 token」——verify 根本不是 agent 派发，本就无 harness 回传可报。把它计为「遥测缺口」是分类错误。本批不治，只如实登记；若日后要观测 verify 自身消耗，需另设机制（那是 Bash 子进程，不在 transcript 面内）。
- **`mainThreadUsage` 的读法交实践检验**：本批落**累计检查点**，retro 取差值得区间消耗。若实践发现语义歧义（如跨 session 重开、多 worktree 并发），按既有路径登记 ARQ 再议。
- **transcript 通道的版本耦合**：本机 Claude Code 2.1.276 实测。上游布局若变，表现为 `found:false` 降级——**这是设计上的预期行为而非故障**。**失效信号须有机械观察点（R1-C3 折入）**：cadence 复盘读 T5 新增的 `models` 聚合里的**「有值条数」**（及 `dispatchesWithTokenData` 中 `source === 'transcript'` 的占比），**命中率归零 = 该值连续多个包恒 0**，而非靠人眼「留意」。v0.1 只写了「留意」，无机制，属本批自陈「这类矛盾不会让任何门变红」的同一盲区。
- **登记不硬治项**：`ARQ-260914-legacy-sxay-story-id-batch`（跨 10 文件含 3 处 agent def 的老口径换例句）仍可搭本批顺风车——**本批不并**，留原批。

---

## 7. 评审记录

**R1（architect 判官，2026-09-20）**

**独立复证摘要**（全部现跑，命令与关键输出 verbatim，不照单采信作者自报）：

- 段一 census（按 eventId 去重）现跑得 `raw 207 / dup 8 / unique 199 / exact 131 / unavail 11 / 无 tokenUsage 键 57 / 带 model 0 / shapes {"source,status,total":127,"reason,status":11,"input,source,status":4}`——与 §0/§3 逐数吻合。8 条 dup 的 legacy 副本口径（`delivery/changes/dev/` vs `delivery/taskpackages/` 同名件内容不同）成立：不去重得 207 而非 199。
- 段二解析率现跑得 `无 dispatchId 字段 57 / 有 id 142 / 命中 91 / 未命中 51 / 命中率 64.1%`——逐数吻合。未命中清单与 §3 两类成因吻合（人读自造标签 `builder-r1`/`acceptance-author-r1`/`verify-r2`/`integrator-closeout`… + 伪 UUID `7f3a91c4-5b2e-4e8a-9d1f-builder01a` 等）。
- 磁盘 `agent-*.jsonl` 总数现跑得 **398**（作者 397，+1 属 live 漂移：作者 census 后 14:14–14:56 间新增若干 gov worktree 子 agent transcript）。非计错，是时点问题，作者已标「现跑 + 给脚本」，处理可接受（见 N2）。
- actor 分布 8 项（planner 38 缺 36 / acceptance-author 49 缺 5 / builder 47 缺 6 / integrator 35 缺 2 / fixer 21 缺 3 / verifier 4 缺 4 / architect-reviewer 3 缺 1 / architect 2 缺 0）现跑逐数吻合，合计 199。
- `grep -rn "\.claude/projects\|os\.homedir\|homedir"` 于 `.claude/{scripts,hooks,agents,references}/` 零命中——T1「本仓从未读过该目录」成立。
- 消费点 grep（tokenUsage 家族）现跑命中 6 文件 + ARQ 行，与 §3 复扫穷尽表一致；`loop-metrics.mjs:240-244` 只判 `tu.status === 'exact'/'unavailable'`、不读 `source`，与 §3「未触」判据一致。
- transcript 结构实证（抽 `agent-a80025b139bddfabd.jsonl`，70 行）：`message.usage` 含 `input_tokens/output_tokens/cache_read_input_tokens/cache_creation_input_tokens` 四路 + `message.model` + 顶层 `isSidechain`/`sessionId`；主会话 `<slug>/<sessionId>.jsonl` 存在（439 行、136 条 `isSidechain!==true` 的 assistant usage）——`readMainThreadUsage` 结构可行。**E3 去重陷阱实证**：该文件 28 条 usage 条目仅 9 个 distinct `message.id`（一 id 最多出现 7 次），不去重即虚增数倍，「按 message.id 去重」是承重正解。
- `--preflight` 干跑：`anchorCount 6 / findings []`，anchorUnion 六路径 == landing-manifest 六路径，锚行无加粗、注记无路径 token。四硬项（起草期实证 / 目标文件锚清单 / sweep 透镜 / 复扫完备性）逐条核过，均满足。
- **子项 b 自报证伪独立复核成立**：`dispatch-stamp.mjs:104-109` 已具 `--token-input/--token-output/--token-cache-read/--token-cache-write/--token-total`、`:101-156` 已具校验逻辑、`planner.md:60` 已具调用行——三处确已就位，缺口真在上游（harness 只回单值）。§0「按此建议改是改不出来的」成立，不是照单采信。

**finding（SHOULD = 打回必须闭合；COULD/NIT = 建议，不阻 GREEN 前可先留根）**：

- **S1（SHOULD）· T2 未钉死 transcript 命中时 `tokenUsage.status`**。§3 消费点表断言「按 `tu.status === 'exact'` 取值（`:240-244` 现读），与 `source` 正交 ⇒ 新源自动被正确计入，无需改动」，但该断言成立前提是 transcript 命中时 tokenUsage 带 `status:'exact'`。§2 T2 改法只写「命中 transcript → `tokenUsage` 用四路分项 + `source: 'transcript'`」，未写 `status`。若实现不设 `status:'exact'`，`loop-metrics.mjs:242` 与 `pod-events-aggregate.mjs:76` 会把新源落进 unavailable 桶，「tokenExact 覆盖度提升」这一目标静默落空，且无任何门报红（正是 §2 T4 自陈的「这类矛盾不会让任何门变红」同族静默面）。**理由**：这是本批「目标效果」能否被既有消费方计数的承重前提，缺一个词即整条新源不可归因。
- **S2（SHOULD）· mainThreadUsage 累计值零验证**。§5 #1 自检夹具清单（正常命中/去重不虚增/文件缺失降级/自造 id 不命中/反推主会话路径）无一条断言 `readMainThreadUsage` 的「`isSidechain !== true` 过滤 + `message.id` 去重 + 累计求和」核心计算正确；§5 #2 人读核对只对子 agent 四路值（「== 对同一 agentId 直接现算 transcript 的值」），不对 `mainThreadUsage`。子项 c（主线程自身观测）的三分项之一，其累计值若算错（sidechain 过滤误伤 / 去重遗漏 / 求和错）无门兜底——正是「真空绿规约」要防的「看起来对」。**理由**：子项 c 目标效果直接依赖该计算，且当前验证集对它零覆盖，把新数据源唯一正确性押在实现期自觉。
- **S3（SHOULD）· §0「84% 黑箱」目标句把挂钟口径数字嫁接给 token 账**。ARQ 行内「84% 黑箱」实测口径是**挂钟**（「13 包 134.7h 挂钟仅 21.6h 可归因」），本批 `mainThreadUsage` 只解决主线程 **token** 消耗，不解挂钟归因。§0 目标句「token 账从「84% 黑箱」变成可归因」把挂钟数字套在 token 账上，读者会误判本批闭合了挂钟黑箱；§5 #9 又把该句当「本批目标效果的直接判定」。**理由**：目标效果的判定基准被错标，属测量命题与基线口径错位，会让 retro 对本批「是否达成目标」做错误判定。
- **C1（COULD）·「权威源」措辞过度声明**。T4 称 transcript 逐消息 usage 为「权威分项源」、T2 称「行为权威源」，未声明其 ≠ 计费真值（provider 计费可因缓存折扣等与 `message.usage` 不一致）。§6 留根「harness `subagent_tokens` 口径根因不查」已部分覆盖「不解释算法」，但「权威」一词仍易被读者当计费真值。建议补一句界定：「权威 = 仓内 token 归因最忠实记录（模型 API 回传的逐消息 usage），非计费真值；计费对账不在本批范围」。
- **C2（COULD）· T4「三条实质约束」与 §5 #6「两句」算术不一致**。T4 改法写「原句的三条实质约束一条不删」但括注列了**四条**（`total = input + output`、cache 不重复计入、unknown 绝不写 0、genuine zero 带 note）；§5 #6 只核「绝不写 0」与 `total = input + output` **两句**。原句（`dev-loop-operating-system.md:132`）实为四条。建议统一为「四条」，§5 #6 核四条（尤其「cache 不重复计入」在 transcript 源下更承重，且是 transcript 四路里 cache 两路不并入 total 的政策锚）。
- **C3（COULD）· 外部依赖失效信号无机械观察点**。兜底（降级 `unavailable` / 不禁用派发 / 不当门）安全面足够；但「命中率突然归零」§6 只写「留意」（人眼），无机制。T5 新增的 `models` 聚合「有值条数」就是现成机械观察点——建议 §6 留根点名它（cadence 复盘读 `models` 有值条数，而非纯人眼留意命中率）。
- **N1（NIT）· E5「`meta.json` 另记请求值 `haiku`」载体名不实**。本机实测 `.claude/projects/**` 下 meta.json / `meta*` 文件 **0 命中**；「haiku」102 处命中均在 `attachment.prompt_snapshot.systemPrompt` 文本（agent prompt 的 tiering 文案），非「请求模型」结构化记录。E5 核心结论（槽位映射失真、请求值 ≠ 生效值）成立，但「meta.json」载体描述需更正为实际落点。
- **N2（NIT）· 398 vs 397**（见复证摘要；非计错，时点漂移。若担心定稿后继续漂，可把该数标「时点快照」）。

**对 human 轨级改判的表态**：**可接受，不升级**。原「子项 c 触及时转 `full-adversarial + human`」的触发前提是「c 含真设计分叉（估算 vs 不采待定）」；取件复证实证出的第三条路（直读主会话 transcript 逐消息 usage）把「估算 vs 放弃」这个分叉本身溶解——既非估算也非放弃，是直读。本批全部改动为只读派生 + 只增字段、零新门、fail-open，爆炸半径与 ARQ 标 Track A 的动机（分叉未决）不再成立；human 亦已出场立项 ack（「human」半腿保全）。故轨级不升级。但 S1/S2/S3 三项 SHOULD 是「影响目标效果」的闭合义务，须在 Track B 内闭合后再 GREEN。

**结论：打回**（SHOULD 集 = S1、S2、S3；C1–C3、N1–N2 建议随 R2 一并处理或留根）

### v0.2 修订记录 · 2026-09-20 · 主线程（折入 R1 的 S1/S2/S3 + C1–C3；N1 举证反驳、N2 折入）

> 本节由**主线程**追加，**不触碰上方 R1 判官文本任何一字**（append-only 共存，承批④ 先例）。

**折入（全部 6 条 S/C 项；N 项分列于后）**

| R1 项 | 级别 | 折入动作（落点） |
|---|---|---|
| S1 · T2 未钉死 `status` | SHOULD | §2 T2 新增**取值优先级**一条并把 **`status:'exact'` 钉死**：「命中 transcript 时**必须同时置 `tokenUsage.status = 'exact'`**」，并写明理由（`status` 是 `loop-metrics.mjs:242` / `pod-events-aggregate.mjs:76` 的**唯一分桶键**，只写 `source` 会让新源落进 `unavailable` 桶且无门报红）。§5 #2 相应把 `status === 'exact'` 列为**必含项**并标「缺此项即本批目标静默落空」。**判官此条为主线程作实漏项**——v0.1 §3 的「与 `source` 正交」断言本身就以 `status:'exact'` 为前提，却在改法里没写出来，正是「散文侧同步没有 grep 索引」的同族 |
| S2 · `mainThreadUsage` 零验证 | SHOULD | §5 #1 按**两侧**重写夹具清单——「派发侧」保留原五条，「**主线程侧**」新增四条（`isSidechain !== true` 过滤 / `message.id` 去重 / 累计求和 / 降级），并明写**过滤夹具必须混入 `isSidechain: true` 记录**（「只测全 false 夹具等于没测过滤」）；§5 #2 人读核对拆成**两条正交**（派发侧 ① / 主线程侧 ②）+ 单调不减断言 |
| S3 · 「84% 黑箱」口径错位 | SHOULD | §0 目标句**去挂钟数字**并新增「口径界定」块（明写 84% 是**挂钟**口径、本批**不解决挂钟归因**、不得被读作已闭合挂钟黑箱）；§5 #9 补「判定边界」句收窄为 token 账 |
| C1 ·「权威源」过度声明 | COULD | T2 / T4 两处措辞改为「**仓内 token 归因最忠实记录（模型 API 回传的逐消息 usage），非计费真值**」，并明写「provider 计费可因缓存折扣等与之不同，计费对账不在本批范围」 |
| C2 · 「三条」/「两句」算术不一致 | COULD | T4 改「**四条**实质约束一条不删」并注明 v0.1 误写；§5 #6 由核两句改为**核四条**（补齐 `cache 字段不重复计入`、`note: harness-reported-zero`），并写明「cache 不重复计入」在 transcript 源下更承重（transcript 给 cache 两路而 `total` 只取 `input + output`） |
| C3 · 失效信号无机械观察点 | COULD | §6 留根改指 T5 的 `models` 聚合**「有值条数」**为机械观察点，明写「命中率归零 = 该值连续多个包恒 0」，并自陈 v0.1 只写「留意」属同一盲区 |

**N1 · 举证反驳（不折入，请 R2 复裁）**

判官判「E5 的 `meta.json` 载体名不实：本机 `.claude/projects/**` 下 meta.json / `meta*` 文件 **0 命中**」。**主线程实测该判据未命中，因为搜索 pattern 与文件实际命名不匹配**——该文件的 basename 是 `agent-<agentId>.meta.json`（以 `agent-` 起头），`meta*` 作为**前缀** glob 永远匹配不到它。现跑留痕（2026-09-20）：

```
$ find . -maxdepth 4 -name "meta*"          # 判官 pattern
（无输出）

$ find . -maxdepth 4 -name "*.meta.json" | wc -l
398

$ cat ./c--AI-KidPal-gov/63ffe407-030f-4de3-9301-51e57a7a95e8/subagents/agent-aaecbb47025725868.meta.json
{"agentType":"general-purpose","description":"Harness usage probe","toolUseId":"call_00_tuNw8SPwgMJwkn5kkVkH5411","spawnDepth":1,"requestShape":"foreground","requestNonInteractive":true,"model":"haiku"}
```

即：`*.meta.json` 共 **398** 份（与 transcript 1:1，同判官 N2 现跑的 398 一致），逐字含 `"model":"haiku"`。**E5 的载体描述成立**，且该键是**结构化字段**（非判官所指「只存在于 `prompt_snapshot` 文本」）。**一处限定**（R2 请一并核）：该 `model` 键**仅当调用方显式传了 model 覆盖时在场**，未传时**整个键缺失**（同机多数 `meta.json` 无此键）——故它是「**请求的覆盖值**」而非「请求的生效档」，与 `message.model`（生效值）语义不同、不可互换。E5 据此已在 v0.2 补全逐字载体与在场条件。

> **主线程自陈**：v0.1 该处只写「`meta.json` 另记请求值 `haiku`」而未给逐字输出与在场条件，**引据密度不足**正是判官起疑的合理诱因；本条的结论虽是反驳，但作者侧的引据欠密度属实。

**N2 · 折入（不构成计错）**

`397` 已在 §2 E6 与 §3 census 段两处标为**时点快照**并写明「该数随每次派发单调增长、不得当稳定量引用」，同时记录 R1 复跑的 `398` 与差额来源。

**自查（折入时按既有纪律过的两遍）**

1. **「改单源判据时须 grep 其全部消费点逐一过」**：本次改动的核心措辞有四组（`status:'exact'` / 「非计费真值」/ 「四条」/ `models` 有值条数），已全文 grep 复核散落处是否同步：`四条实质约束` 在 T4 与 §5 #6 **两处一致**（v0.1 的病根正是只改一处）；`权威源` 一词 T2 / T4 两处**均已替换**；`84% 黑箱` 在 §0 与 §5 #9 **两处均带口径界定**。
2. **位置副词复核**：本批改法含「**必须同时置**」（S1）、「**唯一**分桶键」（S1）、「**不得**因遥测缺失而阻断」（T4）三处承重副词，已逐句确认落文位置与作用域无歧义。

### R2（architect 判官，2026-09-20，同一实例续审）

**结论：GREEN**（S1/S2/S3 三条 SHOULD 均真闭合、非「看起来改了」；C1–C3 折入属实；N1 判据经复跑证为本人错误、已如实更正。剩余 N3/N4 两条 advisory 不阻 GREEN，可随 landing 时一并收或留根。）

**一、S1/S2/S3 逐条复核（回病根判）**

- **S1 — 真修复**。§2 T2 现写「**取值优先级（钉死）**：transcript 命中 → 旗标值 → `unavailable`。命中 transcript 时**必须同时置 `tokenUsage.status = 'exact'`**」，并给出病根理由「`status` 是 `loop-metrics.mjs:242` 与 `pod-events-aggregate.mjs:76` 的**唯一分桶键**」；§5 #2 必含项清单现列 `tokenUsage.status === 'exact'` 并标「缺此项即本批目标静默落空」。改法与验证集**一致**，v0.1 的病根「改法写了、验证没写」已消除。对「取值优先级是否引入新歧义」：以谁为准 = transcript（明确）；harnessReported 交叉核对已在 §2 T2 第 5 条覆盖（「调用方若仍传了 harness 单值，一并记下」）。残留 N3。
- **S2 — 真修复**。§5 #1 新增「主线程侧」四条夹具：① `isSidechain !== true` 过滤（**夹具须混入 `isSidechain: true` 记录**，明写「只测全 false 夹具等于没测过滤」）② `message.id` 去重后累计正确 ③ 累计求和正确（含 cache 两路与 `total = input + output` 关系）④ 空 principal / 文件缺失降级；并挂「真空绿规约」双断言。§5 #2 拆两条正交人读核对（派发侧 ① / 主线程侧 ②）+ 单调不减断言。**对「混入 isSidechain:true 是否够」**：够。缺「同一 message.id 同时出现在 sidechain 与 principal」交叉夹具**不是缺口**——message.id 在 harness 数据模型里逐消息唯一、不会同一 id 横跨两态；E3 去重陷阱是「同一 id 在同侧重复出现」，已由夹具 ② 覆盖。故不开新 finding。残留 N4。
- **S3 — 真修复**。§0 目标句已去「84% 黑箱」四字、改为「token 账变得可归因」，并新增「**口径界定（R1-S3 折入）**」块逐字写「84% 黑箱」是**挂钟**口径、本批**不解决挂钟归因**、不得被读作已闭合；§5 #9 补「判定边界」句收窄为 token 账、明写「不含挂钟归因」。token 与挂钟两个量在 §0 与 §5 #9 两处均已分开，无残留挂钟数字被当本批目标。

**二、N1 复裁（判据更正，如实留痕）**

主线程反驳**成立，本人 R1 判据有误**。复跑留痕：

```
$ find . -maxdepth 4 -name "meta*"          # 本人 R1 判据（前缀 glob）
（无输出）

$ find . -maxdepth 4 -name "*.meta.json" | wc -l
398

$ cat ./c--AI-KidPal-gov/63ffe407-030f-4de3-9301-51e57a7a95e8/subagents/agent-aaecbb47025725868.meta.json
{"agentType":"general-purpose","description":"Harness usage probe","toolUseId":"call_00_tuNw8SPwgMJwkn5kkVkH5411","spawnDepth":1,"requestShape":"foreground","requestNonInteractive":true,"model":"haiku"}
```

根因：本人 R1 用 `meta*`（`/^meta/i` 前缀）搜载体，而文件 basename 是 `agent-<id>.meta.json`（以 `agent-` 起头），前缀 glob 永远不命中——**是搜索 pattern 错，非载体不存在**。`*.meta.json` 现跑 398 份（与 transcript 1:1）；逐字含 `"model":"haiku"` 的探针载体确在。另复核「`model` 键仅当显式传覆盖时在场」限定：398 份中仅 **36** 份含 `model` 键，与主线程限定吻合。**E5 载体描述（`agent-<id>.meta.json` 结构化字段）复原为真**。此更正按纪律记入 R2，不静默撤回。

**三、C2 算术复核（四条判定）**

`dev-loop-operating-system.md:132` 原文实质约束**应为「四条」**，v0.2 判定成立：

1. `total = input + output`（两者都有时）——**独立一条**（定义 total 算法，不并入任何其它条）。
2. cache 字段不重复计入——独立一条。
3. unknown 绝不写 0——独立一条。
4. harness genuine zero 必须带 `note: harness-reported-zero`——**独立一条，不与「绝不写 0」同源**：两者是「零」的互补两面——「绝不写 0」管「unknown 不得用 0 充数」（禁假 0），「genuine zero 带 note」管「真 0 必须写 0 但标 note」（标真 0），语义相反，**不可合并**。

故 T4「**四条**实质约束一条不删」与 §5 #6「逐字保留原句四条实质约束」判定无误，无需再改。

**四、剩余 advisory（不阻 GREEN）**

- **N3（NIT）·「harness 单值」的旗标映射未明写**。§2 T2 第 5 条「调用方若仍传了 harness 单值」未点名「harness 单值」是哪个旗标。从现行为（`buildTokenUsage()` 的 `--token-total` 路径产出 127 条 `{source,status,total}`）可知 = `--token-total`（legacy 面 = `--tokens`）。建议落地时补一句映射，防实现期把「harness 单值」误读为四路旗标之一。**理由**：E2 对账（36,560 vs 36,829）的证据基础就是这条单值，映射不清会让 harnessReported 交叉核对落空——但意图已明、自然映射可达，故 NIT 不阻。
- **N4（COULD）·「assistant 消息」谓词未钉字段**。§2 T1 写「过滤 `isSidechain !== true` 的 assistant 消息」，但 transcript 记录同时存在三个字段：顶层 `o.type === 'assistant'`、`o.message.role === 'assistant'`、`o.message.type === 'message'`（本机现读实证，190 条 usage 条目全为 `o.type==='assistant'` 且 `message.role==='assistant'`、`message.type==='message'`）。「assistant 消息」若被实现误读为 `message.type === 'assistant'` 会**静默取 0**（真值是 `'message'`）。自然读法（`o.type` 或 `message.role`）均正确，但三字段并存正是「改法文本读两遍、抓条件副词」要防的歧义。**理由**：读法歧义落在无机器判据覆盖处，落错即静默黑箱（与本批主题同族）；但两自然读法皆对，故 COULD 不阻 GREEN。建议落地时把谓词钉为 `o.type === 'assistant'`（等价 `message.role === 'assistant'`，绝不可取 `message.type`）。

### v0.2 标冻前折入 · 2026-09-20 · 主线程（折入 R2 的 N3 / N4，标冻前）

> 主线程追加，**不触碰 R1 / R2 判官文本任何一字**。两条均为 advisory、均不阻 GREEN；**折入理由 = 二者防的都是「静默错值」而非措辞**，与本批主题（遥测静默失真）同族，故不留根、就地闭合。标冻前折入属标冻前修订，见 Status 行。

| R2 项 | 级别 | 折入动作（落点） |
|---|---|---|
| N3 · 「harness 单值」旗标映射未明写 | NIT | §2 T2 第 5 条（`harnessReported`）补一句映射：**post-cutoff = `--token-total`，legacy = `--tokens`**，并明写**它不是四路旗标**（那四路是调用方手喂分项的通道、与 harness 回传无关），防后来者读成一回事 |
| N4 · 「assistant 消息」谓词未钉字段 | COULD | §2 T1 硬约束由三条扩为**四条**，新增第 4 条：谓词**钉死为顶层 `o.type === 'assistant'`**，并附 2026-09-20 现跑实测——按 `message.type === 'assistant'` 计数 = **0**、按 `o.type === 'assistant'` 计数 = **6**（同一探针文件）。**该折入的实测由主线程独立复跑**（未照抄判官给的 190 条口径），结论一致 |

**主线程独立复跑留痕（N4 折入前自测，未照单采信）**：

```
$ node -e "… 统计同一 transcript 两种谓词的计数 …"
top-level o.type = "assistant" | message.role = "assistant" | message.type = "message" | has usage = true
按 message.type===assistant 计数 = 0   |   按 o.type===assistant 计数 = 6
```

⇒ 判官 N4 的「静默取 0」判断**成立且可复现**：误取 `message.type` 产出的不是报错、而是「主线程零消耗」这个**看似合理**的错误结论——正是本批要根治的那类失真，故不留在根。

---

## 附 · census 脚本（供判官复跑）

本节两段 census（199 条清点 / 397 份 transcript 解析率）均由下列一次性内联 Node 脚本产出，非人眼清点。判官复跑时**须注意**：`delivery/changes/dev/` 下存在与 `delivery/taskpackages/` 同名的 legacy 部分副本且**内容不同**，故必须按 `eventId` 去重，否则得 207 而非 199。

```js
// 段一：dispatch-completed 清点（按 eventId 去重）
const fs=require('fs'),path=require('path');
function walk(d,out=[]){for(const e of fs.readdirSync(d,{withFileTypes:true})){const p=path.join(d,e.name);
  if(e.isDirectory())walk(p,out);else if(e.name==='pod-events.jsonl')out.push(p);}return out;}
const seen=new Set();let raw=0,dup=0,exact=0,unavail=0,noTu=0,withModel=0;const shapes={};
for(const f of walk('delivery'))for(const l of fs.readFileSync(f,'utf8').split(/\r?\n/)){
  if(!l.trim())continue;let e;try{e=JSON.parse(l)}catch{continue}
  if(e.eventType!=='dispatch-completed')continue;raw++;
  if(e.eventId){if(seen.has(e.eventId)){dup++;continue}seen.add(e.eventId);}
  if(e.payload&&e.payload.model)withModel++;
  const tu=e.payload&&e.payload.tokenUsage;
  if(!tu){noTu++;continue}
  const k=Object.keys(tu).sort().join(',');shapes[k]=(shapes[k]||0)+1;
  if(tu.status==='exact')exact++;else unavail++;}
console.log({raw,dup,unique:raw-dup,exact,unavail,noTu,withModel,shapes});
```

```js
// 段二：dispatchId → transcript 解析率
const PROJ=require('os').homedir()+'/.claude/projects';
const idx=new Set();
for(const d of fs.readdirSync(PROJ,{withFileTypes:true})){if(!d.isDirectory())continue;
  const sd=path.join(PROJ,d.name);
  for(const s of fs.readdirSync(sd,{withFileTypes:true})){if(!s.isDirectory())continue;
    const sp=path.join(sd,s.name,'subagents');if(!fs.existsSync(sp))continue;
    for(const f of fs.readdirSync(sp)){const m=/^agent-(.+)\.jsonl$/.exec(f);if(m)idx.add(m[1]);}}}
// 再对去重后的 dispatch-completed 逐条判 idx.has(payload.dispatchId)
```

---

## 附 · 起草期制度教训（本批自曝，供判官核）

1. **`planner.md:57` 是陈旧行号**：ARQ 行与临时计划文件均写 `planner.md:57`，2026-09-20 现读实为 `:59`（MODEL-TIER）、`:60`（DISPATCH-STAMP）。**漂移 2 行**。本 PROPOSAL 引用处一律给现行行号，并在 §0 记该漂移。这正是「搬运旧引据视同新断言复证」纪律的一次实用。
2. **ARQ 的「建议改动」第三次被证伪**：子项 b 的建议（「分项入 payload」）指向脚本侧，而实测三处（旗标 / 文档 / 校验）**早已在位**，真缺口在上游数据源。若照 ARQ 字面落地，本批会做一堆空改动并声称完成。
3. **「已实测」不等于「测对了」的又一变体**：我最初写的一次性 census 漏了路径层级（`subagents/` 在 `<projects>/<slug>/<sessionId>/` 下，不在 `<slug>/` 下），输出「磁盘上 transcript 总数 0」——一个看起来像结论的**错误**数字。修正层级后为 397。**教训**：零值 / 极端值出现在 census 里时，先怀疑测量工具本身。
