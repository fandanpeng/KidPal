# PROPOSAL · 261026-gov-review-final-phase-dispatch-contract

> **状态：FROZEN v0.2 · 2026-10-26 · 立项 ack + R2 GREEN 双凭据齐，主线程标冻（评审史：DRAFT v0.1 → R1 `BLOCK(F1,F2)`+NIT → v0.2 修订 → R2 聚焦复核 GREEN；human 立项 ack 2026-10-26：三条 ARQ 并批、候选修法原样交评审；human 同日裁 OQ1=(a)）。v0.2 修订点：OQ1 补裁定行并解除三处条件注记（F1）、T1 契约块 ⓐ 括注对齐现源（F2）、取舍 D 变体计数订正（NIT）**
> **承载 ARQ**：`ARQ-261026-final-phase-dispatch-contract-unwritten`（主条目，MAJOR）、`ARQ-260923-codemem-mode-doc-drift`（SHOULD，同批并）、`ARQ-261025-interrupted-dispatch-progress-note`（COULD，同批并）。关系注记：主条目 = `ARQ-261014-merge-gate-tooling-hotfix-batch` ④ 的拆出载体——261014 已随 P1 批（`261026-gov-review-merge-gate-cluster-fix`）结项归档，本批不双头记账。
> **review 轨**：**预判 Track B 主路径（非轻路径），最终判档归 architect-reviewer**。预判依据：本批 diff 触及 `.claude/agents/**`、`.claude/hooks/**`、`.claude/scope-map.json`、`AGENTS.md`——按 `.claude/references/gov-change.md`「轻路径判据」③，命中任一即恒 Track B，哪怕只改一个字符。A 段升级判据两条均不命中（非跨系统结构性变更；三条修法方向已由 ARQ 预消化 + human 立项 ack 收敛）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（证成：行为面只有两处——`verify.mjs` 一条 fail-closed 文案补指引（退出码与判定零变）、`scope-map.json` 给三个 dev 角色各加一条窄写权（只 `dispatch-progress-*.md` 一种文件形）；其余全为文面。不按 low 报：scope-map 是每次写入必经的强制层，写权面扩张须人眼过，故取 low-medium）。
> **review profile**：G1+G2+G3（`AGENTS.md` / `agents`+`references`+`skills` / `hooks`+`scope-map`）；**契约面零触碰**、**agent 定义只改 planner 一个文面文件且非自改、非 review-loop 定义**、**无新增门**。

```landing-manifest
AGENTS.md
.claude/agents/planner.md
.claude/hooks/verify.mjs
.claude/references/dev-loop.md
.claude/scope-map.json
.claude/skills/codebase-memory/cli/SKILL.md
.claude/skills/kidpal-pod-launch-prep/SKILL.md
```

---

## 0. 想实现的效果与差距

**目标效果**：

1. **final 相位三个契约一次成文**（谁跑 final / scope-file 谁产 / round 序数怎么算），零上下文 planner 按派发书就能一次派对；并把 `planner.md` 与 `integrator.md` 两处**相冲突**的 final 命令消成一处真源。
2. **CodebaseMemory 建图模式的文档与实况一致**：不再教人用 moderate 把图静默削薄约 44%，mode 的生效语义首次有文档承载。
3. **429 / 配额中断后可读一份进度快照恢复**，不再纯靠 `git status` 考古，也不再容易把「已落盘」误判成「未完成」而重做。

**现状差距（全部为 2026-10-26 起草期实测或亲核，见 §2 各 T 证据）**：

- `planner.md` 全文 `scope-file` 出现 **0** 次、`next completed ordinal` 出现 **0** 次；而 `integrator.md` Step 4 已完整写明 final 由 integrator 跑并携 scope-file——两处并存，且 planner 那处**缺 `--scope-file`**，正是「照派单模板跑必然撞 fail-closed」的直接来源（261022-m5-k1 pod-retro 原话在案）。
- round 序数语义只活在 `verifier-evidence-check.mjs` 的报错句里（`round X is not next completed ordinal Y`），先例包各自吃一次。
- `AGENTS.md`、`kidpal-pod-launch-prep/SKILL.md` 仍写 moderate；实测存量索引是 full 量级（17742 / 15756 / 20782 nodes），moderate 探针约 1 万级。
- 中断恢复无任何约定（本波 2 包命中，紧随的治理批自身也中了一次 429）。

**非目标（本批不做，防 scope creep）**：

- **不改 `integrator.md`**——其 Step 4 已是正确形态（谁跑、scope-file 谁产写全）；本批只把 planner 侧对齐，避免造出第三个双源。
- **不新增门、不改任何闸判据、不改 `verify.mjs` 退出码**；progress 注记不接任何机械检查。
- **不追溯改历史散文**：`delivery/workstreams/**` 的 progress、既有 launch-card、`delivery/archive/**` 归档批里的 moderate 字样一律不动（append-only 纪律）。
- **不动 launch card 六要素**（final 契约 SSOT 落 `planner.md`，理由见 §4 技术取舍 A）。
- **不扩 scope-map 之外的写权**；不给子 agent 新增派发要求。

## 2. 问题、根因与改法（T1…T3）

### 2.0 三条 ARQ 复证结论（起草期 2026-10-26；ARQ 行是待验证假设）

> 纪律：以下每条结论都对应一条实跑命令或亲读文件；无实测的推断显式标「未实测」。ARQ 原文里的路径与行号按「搬运引据视同新断言复证」纪律逐条核过，漂移处在本节如实更正。

**复证 1 · `ARQ-261026-final-phase-dispatch-contract-unwritten` —— 处方有效，且实况比 ARQ 更硬：不是「三个契约都没写」，而是「写了两处、彼此冲突」。**

- **paths 勘误（重要）**：ARQ 写的 `.claude/references/planner.md` **不存在**（`ls .claude/references/` 无此文件）；派单模板真身 = **`.claude/agents/planner.md`**（53,595 字节，亲读）。本批按真身落点，ARQ 条目正文的 paths 待主线程在结项时订正。
- **冲突实况（逐字对照）**：
  - `planner.md:74`（Step 10 `[SCOPED/FULL]`）：「终审 merge-readiness 以 full bearing 模式跑一次：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --change-id <changeId> --round <round> --invocation-id <uuid>`（跑全部闸）」——**无 `--scope-file`**，且位置在 verifier↔fixer 内循环里。
  - `integrator.md:38`（Step 4）：「② 使用 planner/TaskPackage 提供的 final completed-only `round` 与 `invocationId` 跑 final bearing verification：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file <test-scope 清单> --change-id <changeId> --round <round> --invocation-id <uuid>`（**scope-file 缺失 = 红闸举手**，verify.mjs fail-closed）」；同节 ① 明确 scope-file 由 integrator 跑 detect_changes + impactedTests 种子**合成并落盘**。
- **八包复现**（`delivery/retro/2026-10-25-loop-retro.md:19-21` 逐字在案：261013 / 261014 / 261018 / 261019 / 261020 / 261022-m5-k1 / 261022-chrome-ghost / 261023，另有 261014-voice-round 同根先例）。本批抽三包原话直引：
  - `261022-m5-k1-prompt-carrier-migration/pod-retro.md:9`：「**final 轮第一次 invoke 因缺 `--scope-file` 被 verify.mjs fail-closed 举手**——我没先读 integrator Step 4 的「终验两步走」就把 final 轮排在自己头上。时序应该是：scoped 轮归 planner、final 轮归 integrator（携圈定清单）。轮号回收复用（r2 未产证据）没有留下脏状态，fail-closed 红闸这次帮了忙。」
  - `261022-p3-record-chrome-ghost-fix/pod-retro.md:11`：「**final round 序数不是「final 的第 1 轮」**：派单写了 `--round 1`，机械闸拒绝——round 序数跨相位全局单调，本包 scoped 已烧掉 r1/r2/r3，final 必须是 r4（「round 1 is not next completed ordinal 4」）。…以后派单统一写「final round = next completed ordinal」。」
  - `261023-agents-md/pod-retro.md:8`：「final verify 轮序号误派 round=3（沿用了上一包 M5-K3 的 final r3 先例，没核本包 own 序列——本包只跑过 scoped r1，机器 nextRound=2）。…**round 序列是 per-TaskPackage 的，派发前先看 `verifier-evidence/rounds/` 盘面，别背上一包的号。**」
- **ordinal 语义源码亲核**（逐字）：`.claude/scripts/verifier-evidence-check.mjs:405` `nextRound: finals.size + 1`；`:382` `errors.push(\`gap before round ${index + 1}\`)`（不许跳号）；`:412` `if (expected.round !== state.nextRound) throw new Error(\`round ${expected.round} is not next completed ordinal ${state.nextRound}\`)`。**未发布 fact 的失败不占序**：`verify.mjs:238-241` 缺 scope-file 在 `preflightProducer` / `publishFact` 之前 `process.exit(2)`（本批实跑 exit 2、无 fact）；`publishFact`（`:357-368`）以 `linkSync` 落到既有 final = EEXIST → `fact publication failed`。**已发布 fact 占序**：`:421` `ok: child.status === 0`——`ok:false` 的红轮也发布 fact、也占序。
- **两条同根先例亲核**：
  - required-check 参数形状（`260929-char-window-v5-trim/pod-retro.md:12` 逐字）：「`taskpackage-required-check.mjs` 带 `--change-id` 或位置 changeId 都不对（位置参数被当路径解析），正确姿势是无参走 current/ 指针。脚本 usage 不直观…」。源码印证：`taskpackage-required-check.mjs` 的 `taskPackagePath()` 有显式 `process.argv[2]` → `resolveExplicitTaskPackage`，无参 → `resolveCurrentTaskPackage`。
  - 补轮删旧 verdict（`261014-voice-round-entry-exit-fixes/pod-retro.md:17` 逐字）：「human … 单点提醒「重跑 supervisor 闸前必须先删旧 verdict」。**「删旧 write-once verdict」这条如果没人提醒，我大概率会撞墙**——write-once 闸撞旧件时的报错不算自解释。」源码核对（R1-F2 后订正）：`supervisor-verdict-gate.mjs:455-461` 的 EEXIST → failLoud 文案现含自解释处置指引——「Remove the stale verdict explicitly only if a new round warrants it.」，并已解释 write-once 语义与「新 verdict 须新 bearing round」；261014 retro 的体感针对的是当时无中文 SOP、规则埋在英文长句里。故本批只把规则以中文 SOP 成文进派发书，不改闸文案。
- **现状计数（起草期实跑）**：`grep -c "scope-file" .claude/agents/planner.md` → **0**；`grep -c "next completed ordinal"` → **0**；`grep -c "正确序列" .claude/hooks/verify.mjs` → **0**。
- **final 命令 census（排除归档）**：全仓 `--phase=final` 的 invoke 命令只有两处文档落点——`.claude/agents/integrator.md:38`（正确，带 scope-file）与 `.claude/agents/planner.md:74`（缺陷，缺 scope-file）；其余命中均为 `verify.mjs` 自检与 `unit-run-check` / `acceptance-run-check` 的参数解析 case。**无第三处**，故只改这两处中的 planner 侧即可收口。
- **处置**：候选四件全收（① 三契约成文 ② verify.mjs 报错带指引 ③ 补轮删旧 write-once verdict 成文 ④ required-check 参数形状成文），文面整合进同一段派发书块 + 一处 Step 10 对齐（T1 改法 1/2/3）。

**复证 2 · `ARQ-260923-codemem-mode-doc-drift` —— 三处病灶仍有效，行号有漂。**

- **行号勘误**：`AGENTS.md` 的原 `:39` 现为 **`:46`**（`grep -n "moderate" AGENTS.md` → 唯一命中 :46）；`kidpal-pod-launch-prep/SKILL.md` 的原 `:93` 现为 **`:18`**（产物清单第 5 条「（moderate + persistence）」）与 **`:104`**（`index_repository(repo_path=<worktree>, mode=moderate, persistence=true)`）**两处**（`grep -c "moderate"` → 2）；`codebase-memory/cli/SKILL.md:42` 的 `delete_project '{"project":"C-AI-KidPal"}'` **行号未漂**（亲核）。
- **实况复测（2026-10-26，`codebase-memory-mcp cli index_status`）**：`C-AI-KidPal-pod-char-stroke` = 17742 nodes / 42184 edges（status ready）、`C-AI-KidPal-pod-host-test-infra` = 15756 nodes、主索引 `C-AI-KidPal` = 20782 nodes——均 full 量级，与 ARQ 记录逐值一致。
- **mode 空转（「只在从零解析时生效」）= 2026-09-23 ARQ 实测在案（探针 A/B 三份 logfile + 存量三例），本批未复跑**——复跑需从零解析两个探针项目，成本高且证据已在案；文档只陈述可观察事实并注实测日期，不写未复证的因果。
- **波及面 census**（`grep -rn "moderate"`，排除参考工程目录）：canonical 活面共 4 行 = 上述三处 + `cli/SKILL.md:20` 的模式定义行（保留，见 T2 边界）；`delivery/taskpackages/**` 与 `delivery/workstreams/**` 的历史记录多行（launch 卡 / progress / launch-prep-record）、`delivery/archive/**` 归档批注记——**append-only 不追改**。
- **处置**：候选三件全收；另加「mode 语义」一行落工具文档（`cli/SKILL.md`），理由见 §4 技术取舍 C。

**复证 3 · `ARQ-261025-interrupted-dispatch-progress-note` —— 证据成立，且候选修法与写权实况冲突（本批唯一真实分叉，human 已裁 (a)；见 §4 OQ1）。**

- **证据亲核**：`261022-m5-k3-honcho-memory-production/pod-retro.md:7` 逐字（429 拦腰、只靠 `git status` 考古、「若派发模板约定『中断前先落进度注记』会更省」）、`:25` 改进项「派发模板加一句「被中断时把已落盘进度写进包目录 progress-note」」；另紧随的治理批自身亦 429 中招（`delivery/review/261026-gov-review-retro-efficiency-fidelity/retro.md:46` F3，同根不新立、`:64` 指向本条）。
- **写权实况（起草期实跑 `enforce-scope.mjs` stdin 探针，cwd = 本 gov worktree）**：builder / fixer / acceptance-author 写 `delivery/taskpackages/<id>/dispatch-progress-builder.md` → **exit 2**（逐字「scope: agent "builder" 只能写 firmware/**, server/**, tools/**, delivery/workstreams/**；…超出边界。」）；planner → exit 0；integrator → exit 0（其 allow 含 `delivery/taskpackages/**`）。另 `universal.bashDenyWrite` 含 `delivery/taskpackages/**`——**任何角色**用 Bash 重定向写包目录恒拦。
- **冲突说明**：候选修法「往包目录写一行进度注记」对主受害角色（acceptance-author / builder / fixer）**当前不可执行**；照抄会让每个子 agent 撞一次 enforce-scope，形成返工债（违反 `planner.md` 派发书②「scope 写权前置核对」的既有纪律）。
- **候选第二形态（「pod-events friction 带 progress 字段」）亦不选**：`frictionType` 是闭集 `stall|scope-drift|scan-miss|degraded|over-scan`（`dev-loop-operating-system.md:122`），progress 不是摩擦，塞进去会污染 `byType` 统计、并可能与 `malformedFrictions` 告警面纠缠；且 pod-events 写入同属包目录，遭遇同一条写权障碍。理由记 §4 技术取舍 B。
- **处置**：T3 取「包目录窄写权 + 自包含进度快照」形态；写权扩张经 human 2026-10-26 裁定取 (a)（窄写权，见 §4 OQ1），T3 据此无条件落地。

### T1 · final 相位派发契约成文 + planner 侧 final 命令与 integrator 对齐

**问题**：final 相位的三个契约（谁跑、scope-file 谁产、round 序数怎么算）没有任何一处完整成文；更糟的是 `planner.md` 自带一条**与 `integrator.md` 冲突**的 final 命令（缺 scope-file），零上下文 planner 照它派单必然撞 fail-closed 举手。八包复现，每包至少烧半轮到一轮。

**证据**：见 §2.0 复证 1（两处逐字对照、三包 pod-retro 原话、`verifier-evidence-check.mjs:405/:412`、`verify.mjs:238-241`、两条同根先例原话、三项 grep 计数）。

**根因**：派发契约的成文家被拆在两处且方向相反——integrator 侧写全了（但 planner 会话不加载 integrator 定义），planner 侧写了半截（且那半截缺 scope-file）。ordinal 语义则只在机器报错里，先例靠试错传递。这是「契约只活在闸行为里」的典型形态：闸 fail-closed 确实兜住了错，但每次兜住都要现场回推一次正确形态。

**改法**：

1. **`planner.md` 新增一段派发书契约块**。插入点：紧接「派发书三条硬约定」blockquote 之后、`> **分层出场口径…**` blockquote 之前。锚句（原文末句逐字）：`三条均为派单模板条目：零新门、不改任何闸判据。` 其后插入（围栏内按字节落地）：

```markdown
> **派发书 final 相位三条契约（261026 批 T1，八包复现成文；本段 = SSOT，命令真身 = integrator.md Step 4）**：
> ① **谁跑 final**：final 相位（merge-readiness 终审）恒由 **integrator Step 4** 实跑；planner 不在 verifier↔fixer 内循环里替跑 final，其职责 = 派 integrator 前分配 final 的 `round`（见 ③）与 `invocationId`，并写进派单。
> ② **scope-file 谁产**：`--scope-file` 恒由 integrator Step 4 ① 的范围圈定产出（`detect_changes` + 本包 `impactedTests` 种子合成，落 `delivery/taskpackages/<changeId>/test-scope-final.txt`，补轮变体 `test-scope-final-r<N>.txt`）；planner Step 1g 只产种子、不产清单。缺清单 / 圈定失败 = 升级 human，不默认全量、不缩到只剩本包档。
> ③ **round 序数怎么算**：序数 = 本包**已发布** verifier-round fact 个数 + 1（判据 = `verifier-evidence-check.mjs` 的 `next completed ordinal`；scoped / final 共用一条序列，编号不许跳号）。**未发布 fact 的失败不占序**——缺 scope-file、bearing preflight 失败都在开跑前 exit 2、不落 fact，重试仍用同一序数；**已发布 fact（`ok:true` 或 `ok:false`）占序**——同号重跑报 `fact publication failed`，补轮一律取下一个序数。派单写 `<next completed ordinal>`、不写死数字；派前先列 `verifier-evidence/rounds/` 盘面，别背上一包的号。
> **正确序列**：scoped 轮全绿 → integrator 圈定范围 → final。
> **两条同根先例**：ⓐ 补轮重跑 supervisor 闸前**先显式删旧 `supervisor-verdict.json`**（write-once；EEXIST 报错为英文长句、逐字含「Remove the stale verdict explicitly only if a new round warrants it」= 新 round 才可显式删旧，本行给中文 SOP）。ⓑ `taskpackage-required-check.mjs` **无参调用**（走 `current/` 指针）；位置参数 / `--change-id` 会被当路径解析而红。
```

2. **`planner.md` Step 10 `[SCOPED/FULL]` 段两处替换**（消掉与 integrator 冲突的 final 命令）。

   原文一（逐字，原文行内代码用反引号包裹，此处整段按字节引在围栏内）：

```markdown
终审 merge-readiness 以 full bearing 模式跑一次：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --change-id <changeId> --round <round> --invocation-id <uuid>`（跑全部闸）。
```

   替换后完整句（围栏内按字节落地）：

```markdown
终审 merge-readiness **由 integrator 在 Step 4 实跑**（每包唯一一次）：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file <integrator 圈定的 test-scope-final.txt> --change-id <changeId> --round <round> --invocation-id <uuid>`（跑全部闸；缺 `--scope-file` = exit 2 红闸举手、不发布 fact、不消耗 round 序数）。正确序列 = scoped 轮全绿 → integrator 圈定 → final；planner 只分配 final 的 completed-only round 与 invocationId（算法见「派发书 final 相位三条契约」③）。
```

   原文二（逐字）：

```markdown
在 `verify.mjs` invoke 参数中声明 phase=scoped、round、invocationId（scoped 轮）、phase=final、round、invocationId（终审）。
```

   替换后完整句（围栏内按字节落地）：

```markdown
在 `verify.mjs` invoke 参数中声明 phase=scoped、round、invocationId（scoped 轮）；phase=final、round、invocationId 由 planner 分配、随派单交 integrator 在 Step 4 携带实跑（终审）。
```

3. **`verify.mjs` fail-closed 报错补「正确序列 + 不占序」提示**（候选 ②；fail-loud 带指引，退出码与判据零变）。原文（逐字，`:239`）：

```js
    process.stderr.write(`[verify.mjs] final 相位缺 --scope-file（红闸举手，260928 批 T9 fail-closed）：范围 = 影响面圈定清单（detect_changes + impactedTests 种子合成，integrator Step 4）；全量回归 = 人工触发位。缺清单/圈定失败 → 升级 human，不默认全量\n`);
```

   替换后（逐字）：

```js
    process.stderr.write(`[verify.mjs] final 相位缺 --scope-file（红闸举手，260928 批 T9 fail-closed）：范围 = 影响面圈定清单（detect_changes + impactedTests 种子合成，integrator Step 4）；全量回归 = 人工触发位。缺清单/圈定失败 → 升级 human，不默认全量。正确序列 = scoped 绿 → integrator 圈定 → final；本次在开跑前退出，未发布 fact、round 序数不消耗\n`);
```

   必须保留 `缺 --scope-file` 字面——既有自检 case `final-missing-scope-file-stderr-names`（`verify.mjs:478`）依赖它；同时保留 `260928 批 T9` 出处注记（本批是加指引，不是另立判据）。

4. **不新增门、不新增自检 case**：本 T 的行为面只有一条 stderr 文案，无新判据；回归证据 = 既有 16 条自检全绿（§5 V1）。

**为什么这样改**：对根因「两处成文、方向相反」——改法 1 把完整契约放回 planner 唯一必读面，改法 2 把冲突面收敛到同一方向（planner 分配、integrator 实跑），改法 3 让下次再撞 fail-closed 的现场自带正确形态（对齐本仓「失败要可归因」取向，不新增判据）。

目标文件：`.claude/agents/planner.md`（改） · `.claude/hooks/verify.mjs`（改）

### T2 · CodebaseMemory 建图模式文档勘误（moderate → full，并首次成文 mode 生效语义）

**问题**：三处 canonical 文面教人用 moderate 建图，与实况（存量 full、moderate 静默削薄约 44% 节点）相反；mode 的生效语义（只在从零解析时生效）无任何文档承载。后果：重建图的人照抄会把图静默削薄，而 `index_status` 仍报 ready（静默缺项，最难发现的一类）；launch-prep 的 `mode=moderate` 注记既不生效、又与存量不符，属误导。

**证据**：见 §2.0 复证 2（三处行号、`index_status` 节点数复测 2026-10-26、2026-09-23 探针实测、moderate 全仓 census）。

**根因**：模式参数是工具侧「只在首次解析生效」的开关，文档把它当成「可随时声明的属性」写；且本仓早期建图用 moderate、后来实际全用 full，文档没跟着走。同类失实早已在同族批次被登记留后（`delivery/archive/changes/arch/260923-arch-review-graph-index-name-errata/architect-review.md:67` 逐字「**L39「moderate 模式」失实**：AGENTS.md L39 与 full 实测（符号削薄约 44%）不符——留待下批」），本批收口。

**改法**：

1. `AGENTS.md:46` 首句替换。原文（逐字）：`本仓自有代码及各参考工程均已建 CodebaseMemory 图索引（moderate 模式）。`

   替换后（逐字，围栏内按字节落地）：

```markdown
本仓自有代码及各参考工程均已建 CodebaseMemory 图索引（建图统一用 full 模式；2026-09-23 实测 moderate 比 full 少约 44% 节点、削减几乎全在 Variable 面，graph 会被静默削薄而 index_status 仍报 ready；mode 只在从零解析时生效，改档须删条目重建）。
```

2. `kidpal-pod-launch-prep/SKILL.md` 三处：
   - 产物清单第 5 条（`:18`）：原文片段（逐字）`（moderate + persistence）` → 替换为 `（full + persistence）`（同句其余字面不动）。
   - 步骤 5 命令块（`:104`）：原文（逐字）`index_repository(repo_path=<worktree>, mode=moderate, persistence=true)` → 替换为 `index_repository(repo_path=<worktree>, mode=full, persistence=true)`。
   - 步骤 5 末条（`:109` 的「返回值 `nodes/edges == expected_nodes/expected_edges` 才算建完…」）之后追加一条（围栏内按字节落地）：

```markdown
- **不要传 `moderate`**（261026 批 T2）：实测 moderate 比 full 少约 44% 节点（削减几乎全在 Variable 面），且 mode 只在从零解析时生效——对已建项目换 mode 重跑是空转；改档须先 `delete_project` 再按 full 重建。
```

3. `codebase-memory/cli/SKILL.md` 两处：
   - Modes 块（`:18-21`，末条为 `- \`fast\` — filtered files, no similarity/semantic`）之后追加一条（围栏内按字节落地）：

```markdown
- **mode 只在从零解析时生效**：对已建项目换 mode 重跑不改变图（2026-09-23 实测 nodes 逐值不变）；改档须先 `delete_project` 再按新 mode 重建。本仓建图一律用 `full`。
```

   - `:42` 示例替换。原文（逐字）：`codebase-memory-mcp cli delete_project '{"project":"C-AI-KidPal"}'` → 替换后（逐字）：`codebase-memory-mcp cli delete_project '{"project":"<项目名>"}'`；并在该代码块下方追加一句（围栏内按字节落地）：

```markdown
删除可再生（重建即恢复），但照抄前先 `list_projects` 核对项目名——别把本仓主索引当占位符删掉。
```

**不做的边界**：`cli/SKILL.md:19-20` 的 `full` / `moderate` 模式定义行保留（工具确有这些模式，删了反而失义——它们是「工具能力说明」，不是「本仓建图指令」）；历史散文（launch 卡 / progress / 归档批）不追改（append-only）。

**为什么这样改**：对根因「文档把一次性开关写成持久属性」——三处 canonical 面统一到 full 并给出实测依据；mode 语义放到唯一讲工具参数的文件（`cli/SKILL.md`），G1 只留一句依据，控制常驻指令预算。

目标文件：`AGENTS.md`（改） · `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改） · `.claude/skills/codebase-memory/cli/SKILL.md`（改）

### T3 · 中断派发进度留痕（窄写权 + 自包含快照）

**问题**：子 agent 被 429 / 配额中断不留任何回执，恢复只能 `git status` 考古盘点「什么已落、什么没落」，既费心思又容易把「已落盘」误判成「未完成」而重做。本波 2 包命中，紧随的治理批自己也中了一次。

**证据**：见 §2.0 复证 3（pod-retro 原话两处、同批 retro F3、`enforce-scope` 探针四臂逐字、`scope-map` 写权表亲核）。

**根因**：派发契约只覆盖正常返回；中断路径的恢复知识既没有落点（包目录对三个主要受害角色不可写），也没有约定（没人写「中断前先留痕」）。唯一已可写的 `delivery/workstreams/**` 语义不对——那里是 workstream 长周期记忆，不是包内进度。

**改法**：

1. `planner.md` 派发书新增一段进度留痕契约（紧接 T1 契约块之后；围栏内按字节落地）：

```markdown
> **派发书进度留痕契约（261026 批 T3；429 / 配额中断的恢复契约）**：长任务子 agent（acceptance-author / builder / fixer / integrator）每完成一个可交付段落（一组验收件、一组实现、一轮修复、一件收口制品），把**自包含进度快照**写进 `delivery/taskpackages/<changeId>/dispatch-progress-<agent>.md`（Write 覆盖语义：已完成段落清单 + 已落盘文件路径 + 下一步；**不载 tokens / gates / rounds / verdict**——那些以机器件为权威）。被中断时最后一份快照即断点；**planner 恢复时先读全部 `dispatch-progress-*.md`，再 `git status` 考古**。写权 = scope-map 对 acceptance-author / builder / fixer 的窄 allow（只 `delivery/taskpackages/*/dispatch-progress-*.md` 一种文件形；integrator 已有包目录写权）；写法 = Write/Edit（包目录 Bash 重定向恒拦）。**惯例非门**：写不进去只降级为无注记，不阻断任务；缺注记不判红。
```

2. `dev-loop.md` 文件地图新增一行（插在 `input-brief.md` 条目之后、`pod-events.jsonl` 条目之前；围栏内按字节落地）：

```markdown
- `dispatch-progress-<agent>.md`（261026 批 T3）：子 agent 的中断恢复进度快照——自包含、Write 覆盖（不载 tokens/gates/rounds/verdict；机器权威在 verifier-evidence / pod-events）。**生产者** = acceptance-author / builder / fixer / integrator（每完成一个可交付段落落一份）；**消费者** = planner（恢复时先读、再 `git status` 考古）。写权 = scope-map 三角色窄 allow（`delivery/taskpackages/*/dispatch-progress-*.md`），包目录 Bash 重定向恒拦。惯例非门：缺注记不判红。
```

3. `scope-map.json` 三处 `allow` 各追加一项（**OQ1 已裁 (a)，无条件落地**）：`acceptance-author` / `builder` / `fixer` 的 `allow` 数组末尾各追加 `"delivery/taskpackages/*/dispatch-progress-*.md"`；数组其余项与 `deny` 一律不动。该 glob 由钩子的极简 `globToRegExp` 解释（`enforce-scope.mjs`：`*` = 单段任意、`**` = 跨目录），故只匹配「包目录下恰一层、文件名以 `dispatch-progress-` 开头的 md」。

**为什么这样改**：候选修法的收益前提是「中断那一刻文件已在盘」——只有窄写权能让子 agent 自己落盘；写权窄到单文件形，包内其他制品（taskpackage / review-packet / verdict / 证据）仍被 allow 边界拦住，`deny` 面零改动。惯例不接机械检查，保「不新增门」。

目标文件：`.claude/agents/planner.md`（改） · `.claude/references/dev-loop.md`（改） · `.claude/scope-map.json`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 指令根 G1 | `AGENTS.md` | 改（T2） | CodebaseMemory 段首句改 full + 实测依据；消费方 = 全部会话（`CLAUDE.md` 经 `@AGENTS.md` 导入传导，不另改） |
| 角色定义 G2 | `.claude/agents/planner.md` | 改（T1/T3） | 派发书新增两段契约块 + Step 10 `[SCOPED/FULL]` 两处对齐；消费方 = 每个 Pod 的 planner 会话（agent 定义随 `--agent planner` 必加载） |
| 流程参考 G2 | `.claude/references/dev-loop.md` | 改（T3） | 文件地图新增 `dispatch-progress-<agent>.md` 一行；消费方 = 全部 dev-loop 角色与人工核对 |
| 技能 G2 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | 改（T2） | 产物清单/命令两处 moderate→full + 一条「不要传 moderate」注记；消费方 = launch 面执行者（主线程） |
| 技能 G2 | `.claude/skills/codebase-memory/cli/SKILL.md` | 改（T2） | 新增 mode 生效语义一条 + `delete_project` 示例改正无害占位；消费方 = 任何建图 / 删条目操作者 |
| 强制执行层 G3 | `.claude/hooks/verify.mjs` | 改（T1） | 一条 fail-closed stderr 文案补指引；判定与退出码零变，消费方按退出码分流、不解析文案 |
| 强制执行层 G3 | `.claude/scope-map.json` | 改（T3；OQ1 已裁 (a)，无条件落地） | `acceptance-author` / `builder` / `fixer` 的 `allow` 各追加一条窄 glob；消费方 = `enforce-scope.mjs`（注册于 `settings.json` / `dev-loop.settings.json` / `review-loop.settings.json` 三处 PreToolUse） |
| 契约面 | `docs/contracts/**` | 未触 | 零 diff（§5 V8 断言） |
| spec / 需求面 | `docs/requirements.md` / `docs/architecture.md` | 未触 | — |
| review-loop / architect 定义 | `.claude/agents/{architect,architect-reviewer,review-moderator,scenario-author,challenger,defender,adjudicator,landing-verifier}.md` | 未触 | 无自改、无第五阀问题 |
| 其余机器面 | `.claude/scripts/**`、`.claude/verify-gates.json`、`*.settings.json` | 未触 | 本批零脚本改动、零新门、零 settings 变更 |

**【硬项 · sweep 透镜义务】新判据 / 新语义全仓预跑（起草期 2026-10-26，命令 + 结果）**：

- **T1 判据词基线**：`grep -c "scope-file" .claude/agents/planner.md` → `0`；`grep -c "next completed ordinal" .claude/agents/planner.md` → `0`；`grep -c "正确序列" .claude/hooks/verify.mjs` → `0`。落地后三条均 ≥1（§5 V3）。
- **T3 新命名面**：`grep -rn "dispatch-progress" --include="*.md" --include="*.json" --include="*.mjs" .` → 命中 7 行、全为叙述面（`delivery/retro/2026-10-25-loop-retro.md` 2 行、`delivery/review/261026-gov-review-retro-efficiency-fidelity/retro.md` 2 行、`delivery/workstreams/gov-debt-reduction/task_plan.md` 1 行、`docs/debt/gov/` 的 ARQ 条目 1 行 + 生成件 `INDEX.md` 1 行），**零既有制品文件**——新命名不撞历史件。
- **T3 写权探针（反例先行）**：同一 payload 形态对 builder / fixer / acceptance-author 均 `exit 2`、对 planner / integrator 均 `exit 0`（逐字见 §2.0 复证 3）；落地后前三人翻 `exit 0`、其余不变（§5 V5）。
- **T2 moderate census**：`grep -rn "moderate"`（排除参考工程与 `docs/archive`）→ canonical 活面 4 行（`AGENTS.md:46`、`launch-prep/SKILL.md:18`、`launch-prep/SKILL.md:104`、`cli/SKILL.md:20`）；历史面 = `delivery/taskpackages/**`（launch 卡、launch-prep-record）与 `delivery/workstreams/**`（progress）、`delivery/archive/**`——全部 append-only 不追改。
- **消费方核对**：`verify.mjs` 的 stderr 无程序解析方（调用方一律按退出码 0/1/2 分流，唯 `--self-check` 的文案断言依赖 `缺 --scope-file` 字面，本批保留）；`scope-map.json` 的 allow 由钩子按路径匹配消费，无第二读取方；`planner.md` / 两个 SKILL / `dev-loop.md` 均为 prose 消费。

**【硬项 · 复扫模式完备性先验】**：本批无退役语面、无重命名、无目录搬家。唯一「模式集」类改动 = T2 的字面清扫，形态已逐类枚举（moderate 三形态 `（moderate 模式）` / `mode=moderate` / `moderate + persistence`，外加 `delete_project` 示例里的主索引字面 `C-AI-KidPal`），且 `cli/SKILL.md:19-20` 的模式定义行是**有意保留**，不属残留。

## 4. human 取舍点（OQ）

**OQ1 · 进度注记的写权落法（T3 的真分叉；`ARQ-261025` 候选修法与写权实况冲突处）**

- **(a) 窄写权 + 自包含快照（本批建议）**：`scope-map.json` 给 `acceptance-author` / `builder` / `fixer` 各加一条 `delivery/taskpackages/*/dispatch-progress-*.md` 的 allow，子 agent 自己落盘。代价 = 三个 dev 角色的写权面各开一个单文件形的窄口（须人眼过）；收益 = 候选修法真的可执行，中断恢复从「考古」降到「读一份快照」。
- **(b) 不动写权，planner 侧代落**：planner 在每次派发 / 返回时写「已派发 / 已返回」两行。代价 = 中断的子 agent 无回执可代落，真正想知道的「哪些段落已落盘」仍然缺；收益 = 零权限变化。
- **(c) 改派发形态（长任务拆成多次短派发）**：代价 = 每次重启子 agent 的上下文成本远高于一行注记，且与既有派发模型冲突；收益 = 零写权变化。
- **推荐 (a)**。理由：候选修法的收益前提就是「中断那一刻文件已在盘」，只有 (a) 满足；口子窄到单文件形（包内 taskpackage / review-packet / verdict / 证据面全在 allow 外），`deny` 面零改动，且有 §5 V5 的正反双探针把「只开了这一个口」钉死。
- **human 已裁（2026-10-26）：取 (a)**——三个 dev 角色各加一条单文件形窄 allow，`deny` 零改动。T3 改法 3、§3 表 scope-map 行、§5 V6 与 landing-manifest 据此**无条件落地**（条件注记已解除，R1-F1）。
- **裁定后对照（备查，不执行）**：若取 (b) 或 (c)，T3 改法 3 与 §5 V5 / V6 删除，landing-manifest 去掉 `.claude/scope-map.json`（七行变六行）；本批已裁 (a)，该分支不执行。

**非 OQ 但提请评审注意的技术取舍（已给理由，judge 可 challenge）**：

- **取舍 A · T1 落 `planner.md` 单源、启动卡不加**：ARQ 给的是「派单模板（或 launch card 文面）」二选一。选派单模板，因为启动卡是每包人写（`kidpal-pod-launch-prep` 步骤 7），两处都写 = 双源漂移；而 `planner.md` 随 `--agent planner` 必然加载，零上下文 planner 一定读得到。
- **取舍 B · T3 不走 `pod-events` friction 的 `progress` 字段**：`frictionType` 是闭集（`stall|scope-drift|scan-miss|degraded|over-scan`），progress 不是摩擦；硬塞会污染 `byType` 统计并与 `malformedFrictions` 告警面纠缠，且 pod-events 写入同属包目录、遭遇同一条写权障碍。改为独立快照文件，语义各归各位。
- **取舍 C · mode 生效语义落 `cli/SKILL.md` 而非 `AGENTS.md`**：AGENTS 是常驻指令根（Instruction Budget 三问），工具参数语义放工具文档更合适；G1 只留一句实测依据 + 结论（full）。
- **取舍 D · scope-file 规范名定为 `test-scope-final.txt`**：存量主形态 20 份，补轮变体 `test-scope-final-r<N>.txt` 共 **12 份**（r2=6 / r3=2 / r4=3 / r6=1），另有 `test-scope.txt` / `final-scope.txt` / `verifier-scope.txt` 等散形态——本批只把主形态 + 补轮变体写成规范名，存量不追溯改名（逐项实测：`test-scope-final.txt` 20 / `test-scope-final-r2.txt` 6 / `test-scope.txt` 3 / `test-scope-final-r4.txt` 3 / `verifier-scope.txt` 2 / `test-scope-final-r3.txt` 2 / `final-scope.txt` 2 / `test-scope-final-r6.txt` 1 / `scope-list.txt` 1）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> **断言纪律**：每条 = 命令 + 预期 + 评估时点；基线值全部为起草期实跑（2026-10-26，本 gov worktree）。文案类预期（「落地后含某句」）依 §2 替换文本直接可判，不依赖 scratch 原型（本批无原型件）。
> **时点定义**：pre-commit = 落地文件在盘、未 commit 即可跑；post-commit = 须落地提交后才成立。

**基线（HEAD，pre-commit，起草期实跑）**：

| # | 命令 | 实跑输出（逐字 / 摘要） |
|---|---|---|
| B1 | `node .claude/hooks/verify.mjs --self-check` | `{"selfCheck":"verify.mjs","pass":16,"fail":0,"total":16,"ok":true}`，exit 0 |
| B2 | `node .claude/hooks/enforce-scope.mjs --self-check` | `"ok": true`，8 臂（`fixture-nonempty` / `fixture-real-topology` / `sibling-worktree-workfile-allowed` / `sibling-worktree-redline-blocked` / `outside-path-agent-blocked-legacy` / `outside-path-mainthread-allowed-legacy` / `home-tree-behavior-unchanged` / `sibling-legacy-file-topology-allowed`），exit 0 |
| B3 | `node .claude/hooks/verify.mjs --gate-child --phase=final` | exit **2**；stderr 逐字含 `缺 --scope-file`，**不含**「正确序列」 |
| B4 | `enforce-scope` stdin 探针（四条，命令形见 V5） | builder / fixer / acceptance-author 写 `dispatch-progress-*.md` → 均 **exit 2**；planner / integrator → **exit 0** |
| B5 | 三条 grep 基线 | `planner.md` 的 `scope-file` = 0、`next completed ordinal` = 0；`verify.mjs` 的 `正确序列` = 0；`AGENTS.md` 的 `（moderate 模式）` = 1；`launch-prep` 的 `mode=moderate` = 1、`moderate + persistence` = 1；`cli/SKILL.md` 的 `delete_project '{"project":"C-AI-KidPal"}'` = 1 |

**落地验证集**：

| # | 时点 | 命令 | 预期 | 判别力 |
|---|---|---|---|---|
| V1 | pre-commit | `node .claude/hooks/verify.mjs --self-check` | `pass:16, fail:0, ok:true`，exit 0（文案改动不增不减 case） | 若文案误删 `缺 --scope-file` 字面 → `final-missing-scope-file-stderr-names` 红 |
| V2 | pre-commit | `node .claude/hooks/verify.mjs --gate-child --phase=final 2>&1 \| grep -c "正确序列"`；`echo ${PIPESTATUS[0]}` | 第一行输出 `1`，exit = `2` | 基线 B3 = 0 / exit 2；改动后仍 exit 2、字面出现 |
| V3 | pre-commit | `grep -c "scope-file" .claude/agents/planner.md`；`grep -c "next completed ordinal" .claude/agents/planner.md`；`grep -c "dispatch-progress-" .claude/agents/planner.md`；`grep -c "supervisor-verdict.json" .claude/agents/planner.md`；`grep -c "taskpackage-required-check.mjs" .claude/agents/planner.md` | 五项均 ≥1（基线前两项 = 0） | 基线 0 → 落地 ≥1 |
| V4 | pre-commit | `grep -c "（moderate 模式）" AGENTS.md`；`grep -c "mode=moderate" .claude/skills/kidpal-pod-launch-prep/SKILL.md`；`grep -c "moderate + persistence" .claude/skills/kidpal-pod-launch-prep/SKILL.md`；`grep -c "delete_project '{\"project\":\"C-AI-KidPal\"}'" .claude/skills/codebase-memory/cli/SKILL.md` | 前两项 = 0；第三项 = 0；第四项 = 0（基线各项分别 1 / 1 / 1 / 1） | 逐字清零即判别；`cli/SKILL.md` 模式定义行的 `moderate` 有意保留、不计入 |
| V5 | pre-commit | 四条 stdin 探针（`echo '<payload>' \| node .claude/hooks/enforce-scope.mjs`，`agent_type` 依次 builder / fixer / acceptance-author 写 `delivery/taskpackages/261026-fx/dispatch-progress-builder.md`）→ 均 exit 0；护栏两条：builder 写 `delivery/taskpackages/261026-fx/review-packet.md` → exit 2；builder 写 `delivery/taskpackages/261026-fx/taskpackage.json` → exit 2；planner 写同名 progress 文件 → exit 0 | 前四条 exit 0、两条护栏 exit 2、planner exit 0 | 基线 B4 前三人 exit 2 → 落地 0（正例翻转）；护栏保持 2（反例不破） |
| V6 | pre-commit | `node -e "const fs=require('fs');const m=JSON.parse(fs.readFileSync('.claude/scope-map.json','utf8'));const w='delivery/taskpackages/*/dispatch-progress-*.md';for(const a of ['builder','fixer','acceptance-author'])console.log(a, m.agents[a].allow.includes(w), JSON.stringify(m.agents[a].deny));"` | 三行均 `true`，`deny` 与基线逐字一致 | OQ1 已裁 (a)，无条件适用；若三行任一 `false` = 改法未落 |
| V7 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-final-phase-dispatch-contract/PROPOSAL.md` | `ok:true`、`anchorCount:7`、`anchorUnion` = manifest 七行逐条一致、`advisories` 不含路径夹带 | 结果见本节末「preflight 实跑」 |
| V8 | pre-commit | `git diff --name-only -- docs/contracts/` | 空输出 | 契约面零触碰 |
| V9 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight` 复跑（与 V7 同） + 逐文件 `git diff` 人读 | 七文件 diff 与 §2 各 T 替换文本逐字一致 | landing-verifier 期执行 |
| V10 | post-commit | 复跑 V1–V8（in-repo） | 全部符合上表 | 落地后由主线程 / landing-verifier 执行；**未实测**（依赖落地） |
| V11 | post-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs 261026-gov-review-final-phase-dispatch-contract --source-ref HEAD` | `PASS (…, kind=gov)`，exit 0 | **未实测**（依赖 FROZEN + landing-verdict）；本批无契约 diff |

**preflight 实跑**：见文件末尾「起草期自检留痕」段。

## 6. 留根清单

**本批结项（landing 后由主线程执行登记 / 状态变更，architect 不做）**：`ARQ-261026-final-phase-dispatch-contract-unwritten`（主条目，结项时须顺带订正其 `paths`：`.claude/references/planner.md` → `.claude/agents/planner.md`）、`ARQ-260923-codemem-mode-doc-drift`、`ARQ-261025-interrupted-dispatch-progress-note`。

**留根与复议触发**：

1. **launch card 不加 final 三条契约**（取舍 A）：SSOT 恒在 `planner.md`。复议触发 = 观察到一次零上下文 planner 明明加载了定义仍没按契约派单。
2. **scope-file 存量命名变体不追溯**：只规范新包。复议触发 = 机械侧需要按名取件（届时另立批统一改名 + 迁移）。
3. **progress 注记是惯例非门**：不接机械检查、缺注记不判红。复议触发 = 中断恢复仍出现两次以上「纯考古且误判重做」的实例。
4. **moderate 历史散文不追改**（`delivery/workstreams/**`、旧 launch 卡、归档批）：append-only。复议触发 = 出现新会话照抄旧 progress 行在 canonical 面写回 moderate。
5. **`KidPal-claude` 项目的建图模式未单列核实**：AGENTS 新句不再逐项目声明模式，只给「建图统一用 full」+ 实测依据。复议触发 = 需要按项目声明模式时（例如引入不同用途的索引）。
6. **`verify.mjs` 的 bearing preflight 失败文案未含「正确序列」**：该路径的报错来自 `verifier-evidence-check.mjs:412`（ordinal 不匹配），本批只改缺 scope-file 这一条；两者同族但不同判据，未一并扩。复议触发 = 再次出现序数误派且现场未从报错自解释。
7. **OQ1 已裁 (a)（2026-10-26）**：T3 改法 3、§3 表与 §5 V5 / V6 无条件落地，原「若取 (b)/(c)」条件分支已解除。复议触发 = 观察到窄写权被越界使用（子 agent 试图借该 allow 写包内其他文件、被 allow 边界拦下）时，评估收窄或加注。

**环内自律**：本批无 scratch 原型件（唯一行为面是一条 stderr 文案与 scope-map 数据改动），§5 预期值全部来自起草期实跑基线或替换文本直判；落地实现若与本 PROPOSAL 文本有字节差异，以落地实跑为准覆写 §5 预期并在 §7 留痕，不把起草期读数当既有事实转引。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

## 起草期自检留痕（四句自检的落地记录，2026-10-26）

1. **preflight 实跑**（只读干跑，V7）：

```
$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-final-phase-dispatch-contract/PROPOSAL.md
{"ok":true,"gate":"landing-preflight","proposal":"delivery/review/261026-gov-review-final-phase-dispatch-contract/PROPOSAL.md",
 "section2BodyFound":true,"anchorCount":7,
 "anchorUnion":[".claude/agents/planner.md",".claude/hooks/verify.mjs",".claude/references/dev-loop.md",
   ".claude/scope-map.json",".claude/skills/codebase-memory/cli/SKILL.md",
   ".claude/skills/kidpal-pod-launch-prep/SKILL.md","AGENTS.md"],
 "advisories":[],"findings":[]}
exit=0
```

   `anchorUnion`（7 项）与头部 landing-manifest 围栏块逐条一致；`advisories` 空 = 锚行未夹带路径记号。

2. **路径形态族谱**（`path-form-family.md`）：本批不改任何路径的落点与文件名（无搬家、无改名），只新引一个文件名（`dispatch-progress-<agent>.md`）与一个既有件的规范名（`test-scope-final.txt`），两者都只出现在本批自己改写的文面里，无存量残留需要清扫。

3. **承重引文 byte 对账**（逐条 `grep -cF` 实跑，各 = 1）：`planner.md` 两条被替换原文 + 「派发书三条硬约定」锚句、`verify.mjs` 报错原文、`AGENTS.md:46` 原句、`launch-prep` 两处原文、`cli/SKILL.md` 原示例——八条全部逐字命中，无目视转录。

4. **长文落地自检**：本文件分块写入，写后机检 = `wc -l`（v0.2 = 369 行）、mojibake 扫描（U+FFFD 替换字符零命中）、`### T<n>` 标题计数（3）、`目标文件：` 锚行计数（3）与 preflight 一致。

### R1 - 2026-10-26 - architect-reviewer

**四查结论概要**：

1. **意图保真**：三条 ARQ 候选修法方向零改向。T1 四件候选（①三契约成文 ②verify.mjs 报错带指引 ③补轮删旧 verdict 成文 ④required-check 参数形状成文）全收且落点在 ARQ 给定的二选一内（派单模板侧，取舍 A 成立——launch card 每包人写、双源风险论据成立）；T2 候选三件全收，「mode 语义成文」在 ARQ-260923 原文自带（「mode 语义……当前无任何文档承载」）范围内；T3 对候选修法的改形（包目录窄写权 + 自包含快照）有写权实况支撑且已按 human 已裁 OQ1=(a) 评审。§2.0 五条事实修正逐条亲核成立：paths 勘误（`.claude/references/planner.md` 实测不存在、真身 `.claude/agents/planner.md`）✓；冲突实况（planner.md:74 缺 scope-file 与 integrator.md:38 带全的并存冲突，两处原文 grep -cF 各 = 1）✓；T2 行号漂移（AGENTS.md :39→:46、launch-prep :93→:18+:104 双处、cli/SKILL.md:42 未漂）✓；ordinal 语义源码复核（见下）✓；T3 写权冲突（五臂探针亲跑复现基线）✓。无借修正之名改向。
2. **纪律合规**：manifest 7 文件 == §2 三 T 目标文件并集（V7 preflight 独立复跑 `ok:true`、`anchorCount:7`、`advisories:[]`、exit 0）；零契约面触碰、零 spec 面、零 settings、无新增门（唯一行为面 = verify.mjs 一条 stderr 文案，`:238-241` 判定流亲核零变）；agent 定义只动 planner（非自改、非 review-loop 定义、非 architect）成立。**scope-map 窄写权亲核为真窄**：`globToRegExp`（enforce-scope.mjs:29-48）`*` = `[^/]*` 单段、`**` = `.*` 跨目录、整式 `^…$` 锚定——`delivery/taskpackages/*/dispatch-progress-*.md` 只匹配包目录下恰一层该名形文件；三角色 `deny` 数组逐字零动、deny 优先序（:259-265 deny 先于 allow 判）不受影响；五臂 stdin 探针亲跑（builder/fixer/acceptance-author = exit 2、planner/integrator = exit 0）与 §2.0 复证 3 逐字吻合。T3 契约「惯例非门」、verify.mjs 报错消费方 census（仅 `--self-check` 文案断言依赖 `缺 --scope-file` 字面，:478 亲核在案、改法已保留）均成立。
3. **撞车复核**：`--phase=final` invoke 命令 canonical 活面亲核仅两处（integrator.md:38 正确 / planner.md:74 缺陷；`.claude/` 全域 grep，其余命中均为 verify.mjs 自检与 unit-run-check / acceptance-run-check 参数解析 case）——改 planner 侧收口方向正确、无第三消费方；`dev-loop-operating-system.md:38/:55` 的「终审 full（按 scope-file…）」为阶段描述、与新契约同向不冲突。planner.md 既有 `supervisor-verdict.json`（Step 12 区，3 处）与 `taskpackage-required-check.mjs`（Step 5/8b，2 处）表述均不含 ⓐ/ⓑ 两先例内容，契约块新增非重复。T3 命名 `dispatch-progress` 全仓 grep（排除本 PROPOSAL 自身）= 7 行全叙述面、零既有制品——新命名不撞。取舍 B 亲核成立：`frictionType` 闭集 `stall|scope-drift|scan-miss|degraded|over-scan`（dev-loop-operating-system.md:122 逐字在案）。T2 moderate census（canonical 活面 4 行）与「有意保留 cli/SKILL.md 模式定义行」边界合理。
4. **牵连面**：ordinal 语义与契约块 ③ 逐句对源核过——`collectEvidenceState` 单 rounds 目录收 `r<N>.json`（scoped/final 共用序列、无 phase 维度）、`nextRound = finals.size + 1`（:405 区段）、跳号报 `gap before round`（:382 区段）、`ok: child.status === 0`（ok:false 红轮也落 fact 占序）、`fact publication failed`（:433）、缺 scope-file 在 `preflightProducer`/`publishFact` 之前 exit 2——③ 的「未发布失败不占序 / 已发布 fact（ok:true|ok:false）占序 / 同号重跑报 fact publication failed」表述与源一致。八包清单与 retro 原话（2026-10-25-loop-retro.md:19-21、三包 pod-retro 抽查）逐字吻合；两条同根先例源码印证（required-check `process.argv[2]` 显式解析 :28/:306、supervisor-verdict-gate EEXIST failLoud :455-461）。B1（verify 16/16）、B2（enforce-scope ok:true 8 臂）、B3（gate-child final exit 2、`正确序列` = 0）、B4（五臂探针）、B5（grep 计数）基线全部亲跑复现；主索引 `index_status` = 20782 nodes / ready（复证 2 数值吻合）。

**V10 / V11 起草者自标「未实测」的判官表态**：**不构成否回项**。两项均为结构性 post-commit 检查（V10 依赖落地提交、V11 依赖 FROZEN + landing-verdict），先例批（261026-gov-review-merge-gate-cluster-fix V14/V15）同形态未实测放行；前置项 V1–V9 的基线与命令形态我已逐条亲核可执行、可判别（V2 的 `PIPESTATUS` 取 exit 2 形态亲跑成立、V5 stdin 契约 `tool_name/tool_input.file_path/agent_type/cwd` 亲核对源）。

**findings**：

- **F1 · SHOULD（解后方可冻）· §4 OQ1 缺「human 已裁」行，条件化注记未随裁定解除**。证据：human 已于 2026-10-26 裁 OQ1 = (a)（窄写权），但 §4 OQ1 现文止于「推荐 (a)」、无裁定行——与同日先例批（261026-gov-review-merge-gate-cluster-fix §4 三个 OQ 均带「**human 已裁（2026-10-26）**」行）形态不一致；FROZEN 制品是裁定记录载体，缺行使「OQ1 是否已裁」在下游（landing-verifier / 升级闸 / 结项追溯）不可判。连带：T3 改法 3 的「**仅 OQ1 取 (a) 时落地**」、§3 表 scope-map 行「（T3，**仅 OQ1=(a) 时落地**）」、V6 的「仅 OQ1=(a) 时适用」三处条件化注记与 manifest 无条件含 `.claude/scope-map.json` 之间，在裁定未回填前存在解读分叉。修法方向（主线程/起草者 append-only 或定稿改）：§4 OQ1 末补「**human 已裁（2026-10-26）：取 (a)**」一行；上述三处条件注记同步改为「已裁 (a)，无条件落地」。
- **F2 · SHOULD（解后方可冻）· T1 契约块 ⓐ 的「EEXIST 报错不自解释」与现源不符**。证据：`.claude/hooks/supervisor-verdict-gate.mjs` 的 EEXIST failLoud 文案（:455-461 逐字在案）现含自解释处置指引——「Remove the stale verdict explicitly only if a new round warrants it.」，且已解释 write-once 语义与「新 verdict 须新 bearing round」；PROPOSAL §2.0 自注「（英文长句，无中文指引）」字面为真，但契约块 ⓐ 括注「（write-once；EEXIST 报错不自解释）」是把 261014-voice-round retro 的当时体感升格为对现源的断言，落地即钉进 planner canonical。修法方向：括注改为与现源一致的表述（如「write-once；报错为英文长句、指明须显式删旧」），或删去该断言只留动作规则（ⓐ 的规则本体「先显式删旧再重跑」不受影响、无需动）。
- **NIT（随批顺手，不单独设 verdict 障碍）· 取舍 D 变体计数 off-by-one**：`test-scope-final-r<N>.txt` 全仓实测 **12** 份（r2=6 / r3=2 / r4=3 / r6=1，find 亲跑），§4 取舍 D 写「共 13 份」；其余计数（test-scope-final.txt 20 / test-scope.txt 3 / verifier-scope.txt 2 / final-scope.txt 2）逐一吻合。不影响「主形态优势」结论，落地时顺手订正即可。

**观察注记（不计 finding）**：复证 1 引用 `261023-agents-md/pod-retro.md` 为缩写名，真身 `261026` 前目录 = `delivery/taskpackages/261023-agents-md-static-semantics-alignment/`；与 retro 源（2026-10-25-loop-retro.md:19）的短名一致、:8 引文逐字属实，不要求改。T1 改法 2 替换句在 planner.md 保留一条带 scope-file 的完整 final 命令，与 integrator.md:38 形成两处同形命令——契约块已显式自降（「本段 = SSOT，命令真身 = integrator.md Step 4」）且带指针，属带归属的投影非竞争双源，放行；复议触发与留根 1 同域。

**升级 human 项**：无——两项 SHOULD 修法均为记录回填与括注改写，无设计分叉、不触发「影响目标效果」升级线；判档 Track B 主路径成立（见下），riskTier low-medium 证成合理（scope-map 为每次写入必经强制层，写权扩张须人眼过——OQ1 人眼已过）。

**判档结论**：**Track B 主路径**（维持预判，不升不降）。轻路径判据（gov-change.md「轻路径判据」③）命中多条——diff 触 `.claude/agents/**`（planner.md）、`.claude/hooks/**`（verify.mjs）、`.claude/scope-map.json`、`AGENTS.md`，任一即恒 Track B；条目一新增义务条款亦非纯措辞。Track A 段两条判据均不命中：非跨系统结构性变更（纯治理面）；方向已收敛（三条 ARQ 预消化 + human 立项 ack + OQ1 已裁，本轮 findings 无方向级分叉）。

verdict: BLOCK(F1,F2)

### R2 - 2026-10-26 - architect-reviewer

> 轮次纪律：聚焦 R1 两项 SHOULD + NIT 的修订到位性 + 修订是否引入新矛盾，不做全量重核。逐条亲核如下。

**① F1 到位性（亲核）**：

- **OQ1 裁定行已补**（`:260`）：「**human 已裁（2026-10-26）：取 (a)**——……据此**无条件落地**（条件注记已解除，R1-F1）」，与先例批裁定行形态一致。
- **三处条件注记全部解除**：T3 改法 3（`:220`「**OQ1 已裁 (a)，无条件落地**」）、§3 表 scope-map 行（`:236`「改（T3；OQ1 已裁 (a)，无条件落地）」）、V6 判别力列（`:294`「OQ1 已裁 (a)，无条件适用」）。全文 body 残留扫描：原「仅 OQ1 取 (a) 时落地」形态零命中；两处假设分支已改写为不执行的备查/留根——`:261`「裁定后对照（备查，不执行）……本批已裁 (a)，该分支不执行」、§6 留根 7（`:315`）改写为「OQ1 已裁 (a)……原『若取 (b)/(c)』条件分支已解除」并另立窄写权越界复议触发——与 manifest 无条件含 `.claude/scope-map.json`（`:15`）自洽，解读分叉消除。

**② F2 到位性（亲核）**：

- **§2.0 复证 1 已订正**（`:64`）：现文逐字引现源 failLoud 文案「Remove the stale verdict explicitly only if a new round warrants it.」（与 `.claude/hooks/supervisor-verdict-gate.mjs:455-461` 亲读一致），并如实区分「261014 retro 的体感针对的是当时无中文 SOP、规则埋在英文长句里」——体感不再升格为现源断言，且「本批只把规则以中文 SOP 成文、不改闸文案」的边界句准确。
- **T1 契约块 ⓐ 括注已对齐现源**（`:103`）：「（write-once；EEXIST 报错为英文长句、逐字含「Remove the stale verdict explicitly only if a new round warrants it」= 新 round 才可显式删旧，本行给中文 SOP）」——引文逐字属实、不再含「不自解释」失实断言；规则本体「补轮重跑 supervisor 闸前**先显式删旧 `supervisor-verdict.json`**」原样未动。

**③ NIT 到位性**：取舍 D（`:268`）改为「共 **12 份**（r2=6 / r3=2 / r4=3 / r6=1）」并补 `test-scope-final-r6.txt` 1 / `scope-list.txt` 1 两个「其余各 1」的实名——与 R1 亲跑 find 计数逐项吻合，清单自洽。

**修订是否引入新矛盾**：无——三处修订均为记录回填与括注改写，不触任何替换文本、命令、预期值；v0.2 preflight 复跑 `ok:true`、`anchorCount:7`、`advisories:[]`（369 行，与 Status 行自述一致）；判档不变（Track B 主路径）。

verdict: GREEN
