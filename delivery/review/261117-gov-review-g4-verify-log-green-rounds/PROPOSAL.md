# 轻量设计 PROPOSAL · 261117-gov-review-g4-verify-log-green-rounds

> **状态：FROZEN v2 · 2026-11-17**（FROZEN 凭据 = human 2026-11-17 立项裁「现在修 G4（选项 A）」+ 设计审 R1 BLOCK(F1) → v2 回修 → R2 GREEN（F1 CLOSED / F2、F3 接受 / 不要故障注入臂 / 常规档、不启 landing-verifier）；主线程标冻，冻结意图、边界、关键方案与验收标准，不冻字节；作者≠判官由 R1/R2 独立审链承载。）
> **gov-flow**: direct-implementation
> **治理面：G3 强制执行层（`.claude/hooks/**`）；本批记录属 G4 证据面。**
> **一句话范围：只把 gate log 的产出从「仅红闸」改为「每闸每轮」，不改任何闸的判定逻辑、不改 gate 集合、不改 supervisor 门语义。**
> **立项依据：human 2026-11-17 裁「现在修 G4（推荐，选项 A）」——治理面工具缺口修复，不是新方向。诊断件 = `C:/AI/KidPal-pod-channel/delivery/taskpackages/261116-channel-resident-core/gate-tooling-gap-g4.md`（尚在 pod-channel worktree，未进 main；本文引文按该副本逐字核对）。首例实测受阻包 = `261116-channel-resident-core`（P1）。**

---

## 1. 目标与非目标

### 目标

1. 让「从第一轮起全绿」的 Pod 能正常收尾：`verify.mjs` 每闸每轮无条件落一份 gate log，使 G4（`verify-logs-existence`）与 G6（`provenance.verifyLogRefs`）恢复**可满足**。
2. 让 gate log 的产出语义与其消费方声称的语义一致：log = 「本 worktree 每轮每闸真的跑过」的轨迹证据，而不是「红过」的证据。G4 的文案本来就写 `gates were not actually run`，缺的是产线，不是判据。
3. 失败路径行为零变化：红轮的 ErrorEnvelope（`logFile` / `logTail` / `fingerprint` / `failingStep` / `category` / `cmd`）与 exit code 逐字保持。
4. 交付 hermetic 且有判别力的自测：「绿轮次落 log」与「正常红轮 envelope 形状回归」两件事可机判、可回归，不靠真机 Pod 才能证明（红轮**写失败**语义零变化不由自测案承载，由 §5 B2 的 diff 逐字节核读承载——分工见 §3.7）。

### 非目标

- 不重写 `verify.mjs` 的判定逻辑：gate 选择（`gateActive`）、`defer` 判定、`phase` 解析、`ok = exitCode === 0`、round fact 发布（`publishFact`）、exit code 语义全部零改。
- **不改 `writeGateLog` 的签名、目录、文件名规则、返回值形状与 log 内容头部**（R1 F1 回修后新增的本条：原 §3.3 拟加的 `ok=` / `exit=` / `changeId=` 三行头部已撤回，归 §7.4 后续批）。红分支调用站点 `verify.mjs:289` 逐字不动，本批只在绿分支新增一次 best-effort 调用。
- 不改 `.claude/verify-gates.json`：gate 集合、`timing`、`defer` 配置零改（该文件属保护面，见 §4）。
- 不改 `.claude/hooks/supervisor-gate-check.mjs`：G4 判据、`verifyLogsHaveContent`、STEPS 注册表、T4 behavioral battery 零改。
- 不改 `.claude/hooks/supervisor-verdict-gate.mjs`：`collectVerifyLogs`、`evidenceComplete`、`decide`、`buildProvenance`、`writeVerdictOnce` 零改。
- 不给 G4 加第二证据源（诊断件「建议修法 2」）；不引入聚合器、不新增门、不新增 advisory。理由见 §3.5。
- 不做 `.claude/verify-logs/` 的清理 / 轮转 / 配额。理由见 §3.6。
- 不补 G4/G6 的 Pod 归属与新鲜度判据（既有缺口，本批不动；见 §7.1）。
- 不触契约面、scope-map、settings、指令根、任何 agent 定义。
- 不代 `261116-channel-resident-core` 做恢复动作（其 verdict 处置属该 Pod 与主线程，见 §5 D2）。

---

## 2. 现状、根因与牵连面（证据）

### 2.1 根因（代码实证，逐字引用）

- `verify.mjs:191-203` 定义 `writeGateLog(gate, output)`：目录 `.claude/verify-logs`（`mkdirSync recursive`）、文件名 `${Date.now()}-${safeId}.log`、内容头部四行 `gate=` / `kind=` / `cmd=` / `phase=` + 空行 + 闸输出。
- 全仓**唯一**调用点在 `!ok` 分支内 —— `verify.mjs:288-289`：

  ```js
      if (!ok) {
        const logFile = writeGateLog(gate, out);
  ```

- `supervisor-gate-check.mjs:48`：`{ id: 'G4', name: 'verify-logs-existence', optional: false }`；判点 `:717-724` 只调 `verifyLogsHaveContent(verifyLogsDir)`（`:176-182`：目录存在 ∧ ≥1 个 `.endsWith('.log')` ∧ 至少一个 `size > 0`），**无豁免分支、无 env 旁路**。
- 推论：全绿轮次不产生任何 log → 目录从未创建 → G4 恒红。这与诊断件的根因判断一致。

### 2.2 牵连面比诊断件更宽：三处下游，不止 G4

诊断件只点了 G4（Stop 门）。实地核代码后，「无 log」同时打穿三处，其中两处属 supervisor 合并门本体：

| # | 位置 | 判据 | 全绿 Pod 的后果 |
|---|---|---|---|
| ① | `supervisor-gate-check.mjs:719`（G4） | `!verifyLogsHaveContent(dir)` → `block('G4 FAILED: no verify-logs found with content. ...')` | Stop 门 BLOCK（已实测，首例 = 261116） |
| ② | `supervisor-verdict-gate.mjs:698` | `const evidenceComplete = bearing.present && bearing.ok === true && closeoutPresent && verifyLogs.length > 0;` → `decide()` `:549-555` | `!evidenceComplete` → `verdict = 'BLOCK'`（reason `adjudicate:low/med∧evidence-incomplete→BLOCK`）；`completedRounds >= 2` → 熔断 `ESCALATE`（FIX-12）。即**低/中风险全绿 Pod 连 auto-merge 也拿不到**，并被迫升级 human |
| ③ | `supervisor-gate-check.mjs:324` | `validateRefArray(provenance.verifyLogRefs, 'provenance.verifyLogRefs', projectDir, { prefix: '.claude/verify-logs/' })`；`validateRefArray:211-213` 对空数组即 `block('... must be a non-empty array.')` | G6 BLOCK。`collectVerifyLogs`（`supervisor-verdict-gate.mjs:629-635`）在目录不存在时返回 `[]` → 即使 G4 被放宽，G6 仍红 |

②是本批新发现、诊断件未记的一项：缺口不只是「收尾被拦」，而是「全绿 Pod 结构性拿不到 APPROVE」。这进一步强化了「修 producer 而非修 consumer」的取舍（§3.5①）。

### 2.3 为什么长期没被发现

有红轮的 Pod 因失败分支已写过 log，①②③ 三处全部满足 → 缺口只在「从第一轮起全绿」的 Pod 显形。诊断件原文：「有红轮的 Pod 因失败分支已写过 log，不受影响——这也是该缺口长期未被发现的原因。」

### 2.4 消费面普查（机械可核，用于确认改动不伤别人）

- 全仓 `verify-logs` 的**机读**消费方恰三处：`verify.mjs`（产）、`supervisor-gate-check.mjs`（G4 目录检查 + G6 `prefix` 校验）、`supervisor-verdict-gate.mjs`（`collectVerifyLogs` → `evidenceComplete` + `verifyLogRefs`）。无 metrics / loop-health / 仪表盘消费方（`verifyLogsPresent` 仅写入 provenance 作 informational 字段，全仓无读取方）。
- log **内容**无机读消费方：只有 `size > 0`（两处）与 `.endsWith('.log')`（两处）两个判据。人读方 = fixer 按 ErrorEnvelope `logFile` 读原件（先例记于 `delivery/taskpackages/260915-m1-k3-voice-round-ui/pod-retro.md:31`：「verify.mjs 的 logTail 截断曾误导一轮判读——logFile 原件比 logTail 可信」）。
- 文件名形状无解析方：G6 只校 `prefix: '.claude/verify-logs/'`；battery fixture 自造 `.claude/verify-logs/fixture.log`（`supervisor-gate-check.mjs:418`），不依赖 `verify.mjs` 行为。
- 目录不入仓、不入图索引：`.gitignore:45` `.claude/verify-logs/`、`.claude/.cbmignore:14` `verify-logs/`。
- 规范文档无「log 只在失败时写」的成文口径：`.claude/references/**`、`.claude/agents/**`、`.claude/rules/**`、`.claude/skills/**`、`docs/DEV-GOVERNANCE.md` 对 `verify-logs` 的检索为零命中（`verifyLogRefs` 仅出现在 `planner.md:125-126` 与 `references/dev-loop.md:215` 的手工可核清单里，均不涉及产出时机）→ 本批无强制文档同步义务。§6 manifest 第二行的 `dev-loop.md` 补充属**自愿防回退小补，非必要**：设计审 R1（§9.2 第 8 条 / N5）已判「非必要、保留或未改皆合规」，**该文件最终未改不影响本批合规**，落地时不得被当作必改项（`gov-change.md`「允许计划内文件最终未改」）。

---

## 3. 关键方案

### 3.1 主改法：绿轮次也落 gate log（producer 侧单点修）

- **改哪**：`verify.mjs` `runGateChild()` 闸循环内 `:288` 一带。
- **改成什么（意图级，不写逐字节）**：绿闸（`ok === true`）也落一份 gate log；`writeGateLog` 的签名、目录、命名规则、返回路径形状与 log 内容头部**沿用现有实现、逐字不动**（R1 F1 后收敛，见 §3.3）。绿分支的形状 = 在 `if (!ok) {…}` 之外另起一次 best-effort 调用（`try { writeGateLog(gate, out) } catch { stderr 点名 }`，语义见 §3.2），不复用、不改写红分支的调用语句。
- **形状取舍**：诊断件的参考 patch 是把 `const logFile = writeGateLog(gate, out);` 整行移出 `if (!ok)`（**单调用点**）。本设计不取单调用点，取**两分支各自调用**：绿分支走 best-effort 包装（§3.2），红分支保持原样调用原函数、且调用仍先于 error 对象构造。
  - 理由：单调用点若要同时满足 §3.2 的「绿轮写失败不致命」，就必然把写失败改为非致命，于是红轮在写失败时的行为也跟着变（今天 = `writeGateLog` 抛错 → child 未捕获异常退出、stdout 无 payload → 父 `parseChildPayload:334-344` 解析失败 → bearing 路径 `:402-408` `process.exit(2)`；改后 = `error.logFile` 变 `null` 继续走红轮）。那是**失败路径的语义变化**，与验收标准 B1/B2 冲突。两分支形状下红分支字节不动，「失败轮次零变化」可由 diff 核读直接证明。
  - 行锚精度注（R1 F3 回修）：此处失败语义是「child 崩溃 → 父 parse 失败 → exit 2」，**不经** `runBearing:398-401` 的 `![0,1].includes(child.status)` 基础设施分支（Node 未捕获异常默认 exit 1，`1 ∈ [0,1]` 不命中该分支）。outcome（写失败 = 父 exit 2）不变。
- **为什么修 producer 而不修 consumer**：见 §3.5。

### 3.2 绿轮次 log 写失败的处理：best-effort + 点名（不静默、不致命）

- **判据**：绿轮的 log 写失败**不得**把一轮绿改判为红 / infra 失败；但**不得静默** —— 写失败必须在 stderr 点名（含 `gate.id` 与失败原因），让操作者当场看见证据缺口，而不是拖到收尾门看到一句莫名其妙的 G4 BLOCK。
- **依据（仓内既有原则，逐字）**：`verify.mjs:447-449`

  ```js
    } catch {
      // Non-bearing projection/observability cannot change published 0/1.
    }
  ```

  以及 `persistVerifyRuns:186-188` `// fail-open: persistence failure never blocks verify`。gate log 与 `verify-runs.jsonl` / `verdict.json` 同属非承重投影；承重件是 round fact，其发布路径（`publishFact` + `validateFactObject` fail-loud）零改。
- **备选（写失败即致命）不取的理由**：会让可观测性故障升格为验证故障，与上述既有原则相悖。其成本本身可控（父在 `parseChildPayload` 失败后 `process.exit(2)`，位置先于 `publishFact`，故 fact 不发布、round 序数不消耗），但方向不对，且会把「Windows 杀软/EBUSY 抖一下」变成一轮白跑。
- **诚实记账**：写失败若真发生，G4/G6 仍会红（= 今天的既有形态），差别只是 verify 当场点了名。不引入新的静默假绿。

### 3.3 log 头部自描述 —— **R1 F1 已撤回，不在本批**

原设计拟在 log 头部（`verify.mjs:199` 的四行 `gate=` / `kind=` / `cmd=` / `phase=`）之外加 `ok=` / `exit=` / `changeId=` 三行，理由是改动后目录里绿 log 与红 log 混装、跨 Pod 累积，需要归属与结果标注。**本项已按设计审 R1 的阻断项 F1 撤回**，事实链如下（我复核后同意）：

- `ok` 与 `exitCode` 是 `runGateChild` 闸循环内的局部量（`verify.mjs:271-272`），不在 `writeGateLog(gate, output)`（`:191-203`，签名只收两参）的可见域；`changeId` 若按原设计「每轮在闸循环外解析一次、不在每闸重复读盘」，同样只能经调用参数下传。
- 三者叠加 ⇒ 红分支调用站点 `const logFile = writeGateLog(gate, out);`（`:289`）的调用参数必改 ⇒ 与 §3.1「红分支字节不动」和 §5 B2「`if (!ok) {…}` 分支体除位置外无字符变化」直接互斥，且恰好打掉本设计最强的保证（红分支逐字不变 ⇒ 失败路径零变化）。
- 同时它把后续批的能力（归属 / 新鲜度收紧）前塞进「一行修正」，超出 human 立项原意。

**本批收敛后的形状**：`writeGateLog` 的签名、目录、文件名规则（`${Date.now()}-${safeId}.log`）、返回值形状与 log 内容头部**全部逐字不动**；红分支调用站点不动；只在绿分支新增一次 best-effort 调用（§3.1 / §3.2）。`ok=` / `exit=` / `changeId=` 头部字段连同「混装归属 / 新鲜度收紧」一并归入 §7.4 后续候选项，并在那里写清重评触发条件。

保留本小节编号占位（不重排 §3.4–§3.7），以免批记录内的既有交叉引用与 reviewer §9 的行内引用错位。

### 3.4 判②：去重与限界

- **去重：不需要。** G4 只要求 ≥1 个非空 `.log`；每轮每闸一条正是「轨迹」语义的正确形状。去重会把多轮证据压成一条，反而削弱 `verifyLogRefs` 与人读价值。同毫秒同名覆盖在实践上不可达：文件名 = `Date.now()` + 净化后的 `gate.id`，同轮内同闸不重跑，异闸 id 不同名。
- **单文件限界：已有，无需新增。** 写入内容 = `boundedText(stdout, LIMITS.perResultStdoutBytes) + boundedText(stderr, LIMITS.perResultStderrBytes)`（`verify.mjs:268-270`，两常量均 `16_384`，见 `verifier-evidence-check.mjs:25-26`）→ 单 log 上界约 32 KiB。
- **数量限界：本批不加配额。** 增长量级（机械可算）：`.claude/verify-gates.json` gates 共 10 条，其中 `timing=full` 2 条（`unit` / `acceptance`）→ scoped 相位激活 8 闸、final 相位 10 闸。一个 3–5 轮 Pod ≈ 30–50 个文件；典型每个 < 1 KiB（最坏 32 KiB）→ 每 Pod 典型几十 KiB、最坏约 1.5 MiB。目录 gitignored → 不入仓、不污染 diff；`.cbmignore` → 不入图索引。真实成本只是本地磁盘，随 worktree 生命周期线性累积。
- **唯一需要记下的量变后果（诚实记账）**：`collectVerifyLogs` 返回全目录 → 入仓的 `supervisor-verdict.json` 里 `verifyLogRefs` 数组会随之变长。既有实况已有 50+ 条（`delivery/taskpackages/261104-k5-story-cover-nav/supervisor-verdict.json:62-109`），修后同一长寿命 worktree 会增长到数百条量级，`references/dev-loop.md:215` 要求的「`verifyLogRefs` 手工比对」实际可核性进一步下降（该可核性今天已经很低）。本批**不改** `collectVerifyLogs`：改它 = 改 supervisor 门的 provenance 内容 = 越出本批边界并牵动 G6；改为记入 §7.2 后续候选（该候选与 §7.4 的 log 归属标注互为前置，需同批考虑）。

### 3.5 判③：不给 G4 加第二证据源（诊断件「建议修法 2」不取）

三条理由，按权重排：

1. **它不解决问题。** 选项 2 只放宽 G4，但 §2.2 的 ② `evidenceComplete` 与 ③ G6 `verifyLogRefs` 非空两处仍红：全绿 Pod 照样拿不到 APPROVE、照样在 Stop 门被拦。要让它真通，必须同时改 `supervisor-verdict-gate.mjs`（合并门裁决本体）与 `supervisor-gate-check.mjs`（Stop 门本体）两处核心合并保护逻辑 —— 改动面与风险都远大于修 producer 一处分支，而收益为负。
2. **它造第二口径。** 同一件事实（「闸跑过」）出现两个机械判据源 = 口径漂移风险。仓内既有教训逐字可引：`supervisor-gate-check.mjs:692-694` 撤除 G2pre 的理由是「同一制品两处检查 = 口径漂移（ARQ-077 活例）」。
3. **它把本批抬进更重的档。** 动 Stop 门 / 合并门本体属「改核心合并保护逻辑」（`references/gov-change.md` 条件加强节），会触发第二独立核验；而 §3.1 的 producer 侧修不触这两份文件。

补充结论：producer 修好后，G4 的判据本身**不需要**变——它的语义（「闸跑过」）本来正确，只是此前不可满足。修产线 ≠ 放宽门。

### 3.6 判④：不做清理 / 轮转

- **不取「按时间或数量删旧 log」的机械依据**：G6 对 `verifyLogRefs` 逐条做存在性校验 —— `resolveRepoRelativeRef:206` `if (!existsSync(abs)) block(\`${fieldName} references missing file: ${ref}\`);` —— 而 `supervisor-verdict.json` 是 write-once 且入仓（`writeVerdictOnce` 遇 `EEXIST` fail-loud：「supervisor-verdict.json already exists (write-once bearing observation point, FIX-3/FIX-5)」，`supervisor-verdict-gate.mjs:602-610`）。一个 ESCALATE 后停等 human 数日的 Pod，其 verdict 里钉的 log 一旦被轮转删掉，恢复会话时 G6 直接 BLOCK「references missing file」。即：删除会把「证据变多」的新问题换成「合并门更脆」的老问题。
- **现状成本可接受**（§3.4 量化：gitignored、纯本地、每 Pod 几十 KiB 量级），且今天没有任何自动清理机制，本批不引入新机制。
- **若日后确需清理**，正确形状 = 「Pod 起手清本 worktree 上一 Pod 的遗留 + 保护当前 Pod 已钉 refs」，需同时动 dev-loop 起手步骤与归属判据 → 另批（§7.3）。

### 3.7 判别力测试（hermetic，可机判）

- **载体**：`verify.mjs --self-check`（既有 `selfCheck():456-496`，argv 触发、不真跑仓内闸；已有 hermetic 先例 = defer 接缝臂 `:486-493`，用 `spawnSync` + 独立 env 断言）。
- **新增两案**（案名逐条输出，沿用仓内「无案名 = 未执行」的真空绿规约）：
  - **案 A `green-round-writes-gate-log`**：tmpdir 沙箱造最小工程（`.claude/verify-gates.json` 只含一条恒绿闸，例如 `node -e "process.exit(0)"`），以 `CLAUDE_PROJECT_DIR=<沙箱>` 跑 `--gate-child --phase=scoped`，断言 child `status === 0` **且** 沙箱 `.claude/verify-logs/` 下 ≥1 个 `.log` 且 `size > 0`。判别力：删掉 §3.1 的绿分支写 → 本案红。
  - **案 B `red-round-envelope-unchanged`**：同沙箱换一条恒红闸（例如 `node -e "process.exit(3)"`），断言 child `status === 1`、payload `ok:false`、`error.logFile` 指向**存在且非空**的 log、`error.logTail` 含该闸输出、`error.failingStep` / `category` / `fingerprint` / `cmd` 齐备。**判别力范围（R1 F2 回修后收窄）**：本案走的是「红轮写 log 成功」的正常路径，因此它只证**正常红轮的 ErrorEnvelope 六字段形状回归**——它能抓住的是字段改名 / 丢失 / 赋值来源改错、以及红轮不再落 log 这类回归；它**不能**证「红轮写失败时的语义零变化」（把红分支改成 best-effort 后，只要写成功本案仍绿）。判别力的正确表述：挪动 / 改名 `logFile` 等字段、或删掉红分支的 log 写 → 本案红。
- **写失败语义零变化由谁承载（R1 F2 回修，取 reviewer 三选一中的 ①+③）**：由 §5 **B2 的 `git diff` 逐字节核读**承载——红分支字节不变 ⇒ 写失败语义不变，这是机械可核的强保证；案 B 不重复承担、也不声称承担。**不取 ②（故障注入臂）的理由**：预置同名文件 / 只读目录使 `writeGateLog` 抛错，依赖平台特定的错误形态（Windows 下 `mkdirSync` / `writeFileSync` 对同名文件与只读属性的 errno 与 POSIX 不同），会把一个 hermetic 案变成平台敏感案；而它要证的事情已由 B2 以更强形式（字节级）覆盖，收益不抵脆性。
- **不新增测试框架、不进 `verify-gates.json`**（self-check 不是闸，符合「不改 gate 集合」非目标）；`--self-check` 非自动触发，由作者实施时跑并把命令与 stdout 记入批记录，reviewer 核判别力（可要求作者当场演示 A2）。
- **既有回归**：`node .claude/hooks/supervisor-gate-check.mjs --self-check`（含 T4 behavioral battery，fixture 自造 `fixture.log`，不依赖 `verify.mjs`）须仍 exit 0，作为「合并门零变化」的机械旁证。
- **实施风险提示（Windows）**：闸命令经 `spawnSync(cmd, { shell: true })` 执行（`verify.mjs:267`），沙箱里的 `node -e "…"` 在 cmd.exe 下的引号处理需实测；若引号不稳，改用沙箱内落一个 `.mjs` 小脚本作闸命令（同样 hermetic，且不引新依赖）。

---

## 4. 保护边界与归属判定

### 4.1 `.claude/hooks/verify.mjs` 判 **ordinary**，实施归属 **architect**

三条依据（逐条核过，不凭记忆）：

1. **scope-map 机械面**：`agents.architect.allow` 含 `.claude/hooks/**`（`.claude/scope-map.json:177`）；deny 段（`:179-197`）逐条核过，不含 `verify.mjs`、不含 `.claude/hooks/**`。deny 中的 G3 项只有 `.claude/scope-map.json`、`.claude/*.settings.json`、`.claude/settings.json`。→ 物理写权在 architect。
2. **DEV-GOVERNANCE §6 authority 表 G3 行**（`docs/DEV-GOVERNANCE.md:204`，逐字）：「**强制执行层（G3）** | `.claude/scope-map.json`、`.claude/*.settings.json`、`.claude/verify-gates.json`、`.claude/{hooks,scripts}/**` | architect / 按 scope 分工（权限与设置归主线程） | **human**」。→ hooks 的 Owner/Writer = architect；划归主线程的是「权限与设置」，`verify.mjs` 既非权限文件也非设置文件。
3. **architect.md「Write Scope」保护面清单**点名的是 `verify-gates`（= `.claude/verify-gates.json`），不是 hooks 目录；同节明文把「普通 scripts/hooks」列入 architect 可实施的 ordinary 面。本批**不触** `verify-gates.json`（§1 非目标第 2 条）。

### 4.2 本批不触的保护面（逐条）

指令根 `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md`；`.claude/scope-map.json`；`.claude/settings.json` 与 `.claude/*.settings.json`；`.claude/verify-gates.json`；`.claude/agents/architect.md` 与全部 review-loop agent 定义；独立评审/验证凭据（`implementation-review.json` / `implementation-verification.json`）；本地 main、remote 分支与 PR。

### 4.3 G3 的 Approver = human 如何满足

DEV-GOVERNANCE:204 的 G3 Approver 是 human。本批由 **human 2026-11-17 的立项裁定**（「现在修 G4，选项 A」）承载；本批不新增权限、不开例外、不动保护面、不放宽任何门，故不另求人签。合并仍过机械前置升级闸，作者不自批、不合并、不发布。

### 4.4 其它边界

- **契约面零触**：本批不写 `docs/contracts/**`，无需契约包三节；前置升级闸的契约凭据检查对本批应不触发，若闸另有判定按闸的口径处理，不绕。
- **author ≠ judge 不破**：设计由 architect 起草，设计审与真实 diff 审归 architect-reviewer，FROZEN 归主线程，合并归主线程。
- **实施期的自伤风险（必须写进实施纪律）**：`verify.mjs` 一跑就会写**入仓**文件 —— `writeVerdictJson:147-163` 覆写 `delivery/taskpackages/current/verdict.json`（实测该文件被 git 跟踪），`persistVerifyRuns:168-189` 向 `delivery/taskpackages/<current changeId>/verify-runs.jsonl` append（同样被跟踪；gov worktree 的 current 指针现指向 `261104-k5-story-cover-nav`）。因此 §5 A3 那类**真实工程端到端跑**必须在一次性临时 worktree / 临时 clone 里做，**不得**在 gov worktree 里跑；否则会产生 manifest 外的非证据 diff，触发出 scope。hermetic 的案 A/案 B 用 tmpdir 沙箱，天然无此问题。

### 4.5 判档建议（供 reviewer 双向判档，不自决）

- **建议常规档。** 事实依据：改动局限在一个文件的一处分支 + self-check 两案；不改任何 `ok` / exit code 计算；不改合并门与 Stop 门代码；单 commit 可整体 revert 回旧行为；有 hermetic 判别力测试。
- **需要 reviewer 特别核的真实语义风险 = 「G4 可满足性变宽」是否等于「合并门变弱」。** 本设计的回答是**否**：log 只在真实 `spawnSync` 跑完一个闸之后写，没有新增任何旁路或豁免；`touch` 伪造的风险在今天与修后完全同形（G4 从来不抗伪造，抗伪造的是 write-once bearing round fact + G6 的 `validateVerifierEvidenceRefs`：`exactCount: 1` + `changeId` / `phase: 'final'` / `round` / `ok === true` 四重校验，`supervisor-gate-check.mjs:229-281`）。
- **若 reviewer 认为「证据产线变更 = 核心合并保护逻辑」**，则按 `references/gov-change.md` 条件加强节启用 landing-verifier 第二独立核验。需要说明：该节的核心落点清单（前置升级闸、共享闸库、merge-driver、deny-main-push、enforce-scope、INDEX landed 推导）**不含** `verify.mjs`，故本设计不主张加强，但不排斥 reviewer 升档（双向判档归 reviewer）。

---

## 5. 验收标准（可机判）

### A 组 · 修复生效（合并前）

- **A1**：`node .claude/hooks/verify.mjs --self-check` exit 0，stdout JSON `ok:true`，且逐条输出的案名含 `green-round-writes-gate-log` 与 `red-round-envelope-unchanged`（缺案名 = 未执行 = 不通过）。
- **A2（判别力，reviewer 可要求当场演示）**：临时删掉绿分支的 log 写 → 案 A 转红（`ok:false`，failures 含 `green-round-writes-gate-log`）；恢复后转绿。证明案 A 非恒真。
- **A3（真实工程端到端，一次性临时 worktree 内跑，禁在 gov worktree 跑——§4.4）**：`rm -rf .claude/verify-logs` 后跑一次 `node .claude/hooks/verify.mjs --phase=scoped`；若该轮全绿 → `.claude/verify-logs/` 存在且 `.log` 数 ≥ 该相位激活闸数（当前配置 scoped = 8）；若中途有红 → 红闸之前每闸各一份 + 红闸一份，且 scoped 相位「遇红即停」行为不变。命令、exit code、`ls` 结果记入批记录。
- **A4**：`node .claude/hooks/supervisor-gate-check.mjs --self-check` 仍 exit 0（G4/G6 与 T4 battery 零回归）。

### B 组 · 失败路径零变化（合并前）

- **B1**：案 B 全绿 —— **正常红轮**（写 log 成功路径）的 ErrorEnvelope `logFile` / `logTail` / `fingerprint` / `failingStep` / `category` / `cmd` 六字段齐备，`logFile` 指向存在且非空的文件。红轮**写失败**时的语义零变化不由本案承载，由 B2 承载（§3.7 已明写分工）。
- **B2（「失败轮次零变化」的承重判据）**：`git diff main...HEAD -- .claude/hooks/verify.mjs` 逐字节核读并把核读结论记入批记录，覆盖范围明写为**三处**：
  1. `if (!ok) {…}` **整段分支体**（编辑前 `verify.mjs:288-309`）除位置外无字符变化 —— 不只是 error 对象构造段（`:290-299`），还须含 final 相位的 `failures.push(error); continue;`（`:300-306`）与 scoped 相位的 `console.log(...)` + `process.exit(1)`（`:307-308`）；`writeGateLog(gate, out)` 调用仍在该分支内、仍先于 error 对象构造。
  2. 循环后的全红清单块（编辑前 `:312-318`，`if (failures.length > 0) {…}` 含 `errors[]` 投影与 `summarizeNotExecuted`）无字符变化。
  3. `writeGateLog` 函数体（编辑前 `:191-203`）无字符变化 —— 签名、目录、文件名规则 `${Date.now()}-${safeId}.log`、内容头部四行、返回值形状全部不动（R1 F1 回修后本条成为可字面成立的断言，§3.3 已撤回头部加行）。
  - 允许的 diff 只有：绿分支新增的 best-effort 调用（含其 `try/catch` 与 stderr 点名）、§3.7 的 self-check 两案、以及必要的注释。任何落在上述三处的字符变化 = B2 不通过。
- **B3**：final 相位「遇红不停、跑完全闸产全红清单」行为零改 —— **由 B2 承载**（R1 F2 回修后不再「择一」）：B2 第 1 项覆盖 `failures.push(error); continue;`，第 2 项覆盖全红清单收集与 `errors[]` 投影，二者合起来即机械证明 final 相位红信号完备性零改。批记录里写明「B3 by B2 第 1+2 项」即可，不再要求 final 相位 self-check 变体（若作者自愿加，属额外证据，不作为通过条件）。

### C 组 · 不被绕过的负向断言（合并前）

- **C1**：不得以 `touch` / 手写空 log 过闸 —— `verifyLogsHaveContent` 的 `size > 0` 判据零改（`supervisor-gate-check.mjs:176-182` 不出现在 diff 内）。
- **C2**：不得新增任何 env 旁路或豁免分支 —— `supervisor-gate-check.mjs` 与 `supervisor-verdict-gate.mjs` 均不出现在 diff 内（manifest 未列；出 scope 即回 reviewer，不自扩自过）。
- **C3**：既有消费方零破坏 —— `persistVerifyRuns` 段（含 `OWNER: metrics-ledger (FROZEN v4). C1 does NOT modify this section.` 注释）不出现在 diff 内；round fact 形状与 `validateFactObject` 白名单零改（绿轮 `error: null` 不变，`logFile` 只出现在红轮 envelope 内）；`verify-runs.jsonl` 与 `verifier-evidence/**` 的既有读方零改。

### D 组 · 合并后回归确认（主线程 / 原 Pod，非合并门）

- **D1**：在 `261116-channel-resident-core` 所在 Pod worktree 复跑诊断件「复现」序列（`rm -rf .claude/verify-logs` → 全绿 verify → `node .claude/hooks/supervisor-gate-check.mjs`），确认 G4 不再 BLOCK。
- **D2**：该 Pod 若已落 write-once 的 `supervisor-verdict.json` 且 `verifyLogRefs` 为空 / `verdict` 为 BLOCK-ESCALATE，须按 `writeVerdictOnce` 的 fail-loud 指引处置（显式移除旧 verdict 后以**新** bearing round / invocationId 重产），不得静默覆写。本批只修产线，不代该 Pod 做恢复动作。

---

## 6. 轻量 `landing-manifest`

```landing-manifest
.claude/hooks/verify.mjs | architect | ordinary | 主改点：绿分支新增 best-effort gate log 写（writeGateLog 签名与红分支调用逐字不动）+ self-check 两案
.claude/references/dev-loop.md | architect | ordinary | 自愿防回退小补（file-map 行补一句 gate log 语义），非必要；最终未改不影响本批合规
```

说明：

- 新流程 manifest 语义 = **预计 scope**，实际非证据 diff 必须 ⊆ 上列路径；允许计划内文件最终未改（`references/gov-change.md`「轻量 scope 与机械凭据」节）。
- 本批记录 `delivery/review/261117-gov-review-g4-verify-log-green-rounds/**` 属证据尾，由 `isEvidenceTailPath`（`governance-gate-shared.mjs:121-126`）识别、不入 scope 对账，故不列入。
- 出 scope 时先补理由并回 reviewer 回审，作者不自扩自过。

---

## 7. 遗留观察与建议（不属本批，交主线程裁是否登记）

1. **G4/G6 对 verify-logs 无 Pod 归属与新鲜度判据**（既有缺口，本批不改）：任一 worktree 里上一个 Pod 遗留的非空 log 即可满足 G4；`verifyLogRefs` 也只校 `prefix` + `existsSync` —— `validateRefArray:222-224` 的 block 文案写的是 `contains cross-Pod or stale ref outside ${options.prefix}`，但实际判据只有前缀，没有 changeId 归属、没有 mtime 新鲜度。若要收紧，形状 = 让 log 可归属（带 changeId，见 §7.4）+ G4/G6 增当前 Pod 归属与新鲜度判据；动的是 Stop 门本体 → 另批 + 条件加强。
   - **附记（设计审 R1 N4 记账）**：本批修后，`defer` 命中的闸（自报 `executor-skipped`、exit 0）也落绿 log，故 G4 可被「全 defer 的 scoped 轮」满足。这是 `defer` 既有语义的延伸而非新洞（defer 是 261015/261016 批的设计特性，「没跑成」已由 `liftNotExecuted` 上收为 round fact 的机器可判字段 `notExecuted`，且 `final` 相位永不 defer、恒跑兜底），本批不改；若日后要收紧 G4 的「跑过」语义，应与本项的归属/新鲜度判据同批考虑。
2. **`verifyLogRefs` 随 log 数线性增长且入仓**（§3.4）：候选形状 = `collectVerifyLogs` 按当前 changeId 裁剪或取最近 N 条；动的是 `supervisor-verdict-gate.mjs` 的 provenance 内容 → 另批。同时建议复评 `references/dev-loop.md:215` 把 `verifyLogRefs` 列为「手工可核」面是否还成立。
3. **`.claude/verify-logs/` 无清理策略**（§3.6）：候选形状 = Pod 起手清上一 Pod 遗留 + 保护当前 Pod 已钉 refs；需同时动 dev-loop 起手步骤 → 另批。
4. **gate log 的自描述与归属标注（R1 F1 撤回项在此留根）**：本批撤回的 `ok=` / `exit=` / `changeId=` 头部三行（§3.3）与「混装归属」收紧合并为一个后续候选项。
   - **候选形状**：`writeGateLog` 扩参（或改为接收一个已组装的 header 对象），log 头增 `ok=` / `exit=` / `changeId=`（changeId 由 current 指针 fail-open 解析，读法与 `readCurrentPackageRigor:130-139` 同源）；可选再增 `round=`（需同时改 `runGateChildProcess:324-332` 的 child 透传参数面，因 round 只在父进程 `runBearing` 手里）。文件名规则若要带 changeId，属命名迁移，需同批核 G6 `prefix` 校验与 battery fixture。
   - **它与本批的边界为何必须分开**：扩参必然改动红分支调用站点 `:289`，而本批的最强保证正是「红分支逐字不动 ⇒ 失败路径零变化」（§5 B2）。两者同批做会互相抵消，故必须分批。
   - **重评触发（满足任一即起批，否则不做）**：① §7.1 的 G4/G6 归属或新鲜度收紧被立项（那时 changeId 标注成为前置件）；② §7.2 的 `verifyLogRefs` 裁剪被立项（需按 changeId 过滤）；③ 出现一次真实的排障事故，其根因是「无法判定某个 log 属哪个 Pod / 哪轮 / 绿还是红」（例如 fixer 或 human 读错 log 归因）；④ 单个 worktree 的 `.claude/verify-logs/` 文件数或体积造成实际运维困扰（§7.3）。单纯「看起来更有用」不构成触发。
5. **批记录落点不一致（协调项，非设计项）**：本批 `events.jsonl` 现只存在于 `C:/AI/KidPal-pod-m2/delivery/review/261117-gov-review-g4-verify-log-green-rounds/events.jsonl`（一行 intake 事件），而 gov 工作树 `C:/AI/KidPal-gov-g4` 的同名目录在本设计落笔前不存在。请主线程把事件流与本批记录收敛到**将要合并的同一分支**（gov-g4），避免批制品跨 worktree 分家导致前置升级闸找不到 FROZEN / verdict / 钉点。
6. **诊断件位置与引文时效**：`gate-tooling-gap-g4.md` 现在 pod-channel worktree，未进 main。本文引文按该副本逐字核对；若该 Pod 合并时文件内容变化，以合并后的 main 版本为准并回核引文。
   - **回灌建议（交主线程裁）**：诊断件只记了 G4 一处下游，本批实地核出另有 `evidenceComplete`（→ BLOCK / 熔断 ESCALATE）与 G6 `verifyLogRefs` 两处（§2.2，R1 §9.2 第 1 条已亲核同意「无第四处」）。建议由主线程把这一增补同步给 pod-channel planner / 原诊断件（append 一行即可，不改其既有结论），以免该 Pod 的收尾记录仍按「只有 G4」的口径理解阻塞原因。本批不代写他人 Pod 的制品。

---

## 8. 需要 reviewer 重点核的点

> R1 回修后本节按「R1 已结 / 仍待核」重标，便于 R2 只核未结项（R1 §9.6 已限定复审范围 = F1 闭合 + F2/F3 + 新矛盾）。

**R1 已结（保留为记录，不再作为待核问题）**

1. ~~§3.1 形状：两分支各自调用 vs 单调用点上移~~ → R1 §9.2 第 2 条「方法方向同意」（行锚已按 F3 修正）。
2. ~~§3.2 绿轮写失败 best-effort + 点名~~ → R1 §9.2 第 3 条「同意，与 fail-loud 口径一致」。
3. ~~§3.3 头部加 `ok=` / `exit=` / `changeId=`~~ → R1 F1 判**阻断**、不同意保留；本批已按推荐处置**删除**（§3.3 留撤回说明，能力归 §7.4 后续候选并写清重评触发）。
4. ~~§3.5 / §4.5 判档~~ → R1 N3 判**常规档、不启 landing-verifier**，并同意「G4 可满足性变宽 ≠ 合并门变弱」。
5. ~~§3.5 不给 G4 加第二证据源~~ / ~~§3.6 不做清理轮转~~ / ~~§4.1 ordinary 归属~~ / ~~§2.2 三处牵连面~~ → R1 §9.2 第 5 / 6 / 7 / 1 条均同意（第 1 条并亲核确认「无第四处」）。
6. ~~§6 manifest 第二行 dev-loop.md 是否必要~~ → R1 §9.2 第 8 条 / N5 判「非必要、保留或未改皆合规」；§2.4 与 §6 已据此明写「最终未改不影响本批合规」。

**仍待 R2 核（本次回修触及面）**

7. **F1 是否真闭合**：§3.3 已删、`writeGateLog` 签名与红分支调用站点声明为逐字不动、绿分支另起 `try/catch`——§1 非目标第 2 条 / §3.1 / §3.3 / §5 B2 第 3 项四处口径是否已自洽、无残留互斥。
8. **F2 处置是否接受**：取 reviewer 三选一的 ①+③（案 B 措辞收窄为「仅证正常红轮 envelope 形状回归」+ 明写「写失败语义零变化由 B2 逐字节核读承载」），并给出**不取 ②（故障注入臂）**的理由（平台敏感、且被 B2 以更强形式覆盖）。若你仍要求 ②，请明示，我按 ② 补案。
9. **F3 是否闭合**：§3.1 已改述为「child 崩溃 → 父 `parseChildPayload` 失败 → `:402-408` exit 2」并加行锚精度注，不再引 `:398-401`；§3.2 的「备选不取」理由同步改口径（不再说 infra 分支）。
10. **B2 覆盖范围收紧是否足够**：B2 现明写覆盖 `if (!ok) {…}` **整段分支体**（`:288-309`，含 `failures.push(error); continue;` 与 scoped 的 `console.log` + `exit(1)`）、循环后全红清单块（`:312-318`）、`writeGateLog` 函数体（`:191-203`）三处，并列出「允许的 diff 只有绿分支新增 + self-check 两案 + 必要注释」。B3 改为由 B2 第 1+2 项承载。
11. **§7.4 留根的写法**：撤回项的候选形状、与本批的边界理由、四条重评触发是否足够明确（避免变成无期限的口头承诺）；以及 N4（全 defer 的 scoped 轮也满足 G4）记入 §7.1 附记是否位置得当。

---

## 9. architect-reviewer 设计审（R1，2026-11-17）

verdict: BLOCK(F1)

> 本节由 architect-reviewer 独立追加，append-only。所有 file:line、计数、命令行为均亲核本批 gov worktree（`C:/AI/KidPal-gov-g4`，分支 `gov/g4-verify-log-fix` @ `390a7297`），不采信 PROPOSAL 与作者报告的二手转述。

### 9.1 四查结论

- **意图保真：主体成立、一处回修。** human 原话「直接实施 `verify.mjs` 的一行修正（每闸每轮无条件落 log）」——本设计的主改（§3.1 绿闸也落 log）与「修 producer 不修 consumer」完全对得上诊断件（`gate-tooling-gap-g4.md:33-48` 参考 patch 为「单调用点上移」）。但 §3.3 的 `ok=`/`exit=`/`changeId=` 头部加三行既超出「一行修正」，又与 §5 B2 自相矛盾（见 F1），须回修；删去 §3.3 后完全保真。
- **纪律合规：一处纪律缺陷。** `gov-flow: direct-implementation` 头部✓；manifest 三列形状✓；非目标齐备✓；验收以案名逐条输出（仓内「无案名 = 未执行」规约）✓；保护面零触——`verify.mjs` 非保护面（§9.4 N3）。**缺陷**：§3.3 与 B2 对「红分支是否逐字节不变」给出互斥要求，冻结前须收敛（F1）。
- **撞车复核：无冲突。** 与 `supervisor-gate-check.mjs:691-694` 撤除 G2pre 的「同一制品两处检查 = 口径漂移（ARQ-077 活例）」先例不冲突——本设计不新增第二证据源（§3.5），正是遵循该先例。与 `verify.mjs:186-188`（`persistVerifyRuns` fail-open）、`:447-449`（非承重投影不改 0/1）的既有 fail-open 原则一致，§3.2 绿轮 best-effort 有据。ARQ 账本无撞车：`docs/debt` 下 `verify-logs` 相关命中仅 `ARQ-260924-split-order-granularity-gate-gap`（已 closed、拆包粒度题、无关）与一条 CDQ 噪声，无在册活条目。
- **牵连面完整：成立。** 机读消费方经独立普查确为三文件（`verify.mjs` 产 / `supervisor-gate-check.mjs` G4 `:719` + G6 `:324` / `supervisor-verdict-gate.mjs` `collectVerifyLogs:629-635` → `evidenceComplete:698` → `decide:549-555` + `verifyLogRefs`）。`enforce-scope.mjs:88` 仅在注释里把 `.claude/verify-logs` 列为「有意不覆盖」的残余面，非消费方（N1）。log 内容无机读方（仅 `size > 0` / `.endsWith('.log')` / `prefix`）。入仓后果（`verifyLogRefs` 随 log 数增长）作者已在 §3.4/§7.2 诚实记账并归后续批，本批不改，合理。

### 9.2 对作者 8 条判断的逐条意见

1. **「缺口打穿三处（G4 / evidenceComplete / G6 verifyLogRefs），修 verify.mjs 一处即同时解开，无第四处」——同意。** 三处判据亲核：`supervisor-gate-check.mjs:719`（G4）、`supervisor-verdict-gate.mjs:698` 的定义式 `bearing.present && bearing.ok === true && closeoutPresent && verifyLogs.length > 0`、`supervisor-gate-check.mjs:324` + `validateRefArray:211-213`（空数组即 block）。三者都只依赖「`.claude/verify-logs/*.log` 存在且非空」，产线补上即同时可满足。第四处：`enforce-scope.mjs:88` 只是注释，非消费方；全仓无 metrics/仪表盘/loop-health 读取方（`verifyLogsPresent` 仅 `supervisor-verdict-gate.mjs:663` 写入，无读取方）。
2. **「两分支各自调用 > 单调用点上移」——方法方向同意，但机制行锚需修正（F3）。** 结论（单点 + best-effort 会改红轮写失败语义，两分支保红分支不动）正确。但 §3.1:82 写「今天 = 抛错 → child 崩溃 → 父 `runBearing:398-401` 判 `![0,1].includes(child.status)` → exit 2」不精确：红轮 `writeGateLog` 抛错时 child 作为 `--gate-child` 未捕获异常退出（Node 默认 exit 1），`1 ∈ [0,1]` 不命中 `:398-401`；实际是空 stdout 使 `parseChildPayload` 失败、在 `:402-408` exit 2。**outcome（写失败 = 父 exit 2 基础设施失败）不变**，仅行锚错，改述为「child 崩溃 → 父 parse 失败 exit 2」即可。
3. **「绿轮写失败 = best-effort + stderr 点名」——同意，与 fail-loud 口径一致。** gate log 属非承重投影（承重件是 round fact，`publishFact` + `validateFactObject` fail-loud 路径零改）；写失败仍让 G4/G6 变红（既有形态），只是当场点名，不引入静默假绿。既有 `:186-188` / `:447-449` 即同款惯例。无反向掩盖风险。
4. **「§3.3 头部加字段，不加也能修好 G4，可直接删」——不同意保留，本批应删（F1，阻断项）。** 见 F1：`ok=`/`exit=`/`changeId=` 要与 §5 B2「红分支逐字节不变」共存不可能；且它以「为 §7.1/§7.2 留钩子」为由把后续批能力前塞，超出「一行修正」。删去即同时解冲突、回到 human 原意。
5. **「不给 G4 加第二证据源（诊断件选项 2 不取）」——同意，三条理由均成立。** ① 不解决问题（② `evidenceComplete`、③ G6 两处仍红）；② 造第二口径（`supervisor-gate-check.mjs:691-694` G2pre 撤除先例逐字可引）；③ 抬进更重档（动 Stop 门/合并门本体）。
6. **「不做清理 / 轮转」——同意。** `resolveRepoRelativeRef:206` 对 `verifyLogRefs` 逐条 `existsSync`，且 `supervisor-verdict.json` write-once 入仓（`writeVerdictOnce:602-610` EEXIST fail-loud）；删 log 会让停等数日的 Pod 恢复即 G6 BLOCK。取舍成立。
7. **「`.claude/hooks/verify.mjs` 判 ordinary / architect 实施」——同意。** 亲核：`scope-map.json:177` `agents.architect.allow` 含 `.claude/hooks/**`；deny 段（`:179-197`）逐条不含 `verify.mjs` / `.claude/hooks/**`（仅 agents 定义、scope-map、settings、指令根、独立评审凭据）；`docs/DEV-GOVERNANCE.md:204` G3 行 Writer = architect（权限与设置归主线程，`verify.mjs` 二者皆非）；`architect.md:47`「普通 scripts/hooks」列 architect ordinary，`:49-54` 保护面清单点名的是 `verify-gates`（= `verify-gates.json`）而非 hooks 目录，本批不触 `verify-gates.json`。
8. **manifest 第二行 `.claude/references/dev-loop.md`（自愿同步）——非必要。** §2.4 自认「无强制文档同步义务」成立；`dev-loop.md:215` 的「`verifyLogRefs` 手工比对」不因本修失效。保留则属防回退小补、未改亦合规（`gov-change.md`「允许计划内文件最终未改」）。不反对，不强制。

### 9.3 Findings

**F1（阻断 · 冻结前须收敛）：§3.3 头部加 `ok=`/`exit=`/`changeId=` 与 §5 B2「红分支逐字节不变」互斥。**

- 事实链：`writeGateLog(gate, output)` 现签名只收 `gate` 与 `output`（`verify.mjs:191-203`）；`ok`、`exitCode` 是 `runGateChild` 闸循环内的局部量（`:271-272`），不在 `writeGateLog` 可见域。要把 `ok=`/`exit=` 写进 log 头，唯一干净路径是把这两个运行期值经调用参数传入 → 红分支 `const logFile = writeGateLog(gate, out);`（`:289`）的调用参数必须改变。§3.3 又规定 `changeId`「每轮在闸循环外解析一次…不在每闸重复读盘」→ 同样须经参数下传。三者叠加，红分支调用站点必改。
- 但这正与 §3.1「两分支形状下红分支字节不动，『失败轮次零变化』可由 diff 核读直接证明」（PROPOSAL:82）以及 §5 B2「`if (!ok) {` 分支体内**除位置外无字符变化**（`writeGateLog(gate, out)` 调用仍在该分支内…）」（PROPOSAL:186-187）冲突——B2 同一句又写「`writeGateLog` 的目录、文件名、返回值未改（§3.3 只加内容头部行）」，等于一边允许函数体加头行、一边要求调用站点逐字节不变，二者不能同时成立。
- 因此冻结会让「关键方案 §3.3」与「验收标准 B2」互相矛盾，且恰好打掉作者在 §8.1 主张的最强保证（红分支字节不变 ⇒ 失败路径零变化）。
- **处置（二选一，倾向前者）**：
  - （推荐）本批删去 §3.3 三行头部：`writeGateLog` 签名与红分支调用逐字不动，绿分支另起 `try { writeGateLog(gate, out); } catch { stderr 点名 }`。把 `ok=`/`exit=`/`changeId=` 连同归属/新鲜度收紧一并归入 §7.1/§7.2 后续候选。这同时消解冲突、回到 human「一行修正」原意。
  - （次选）保留 §3.3，但须改写 B2 与 §3.1 的措辞为「**行为**零变化（ErrorEnvelope 六字段由 `gate` + `out` 计算、不经 log 头），红分支调用参数允许追加 `ok/exit/changeId`、log 头增三行」，并明示不再主张「红分支逐字节不变」。此路弱化最强保证，不推荐。

**F2（非阻断 · 建议同轮修）：§3.7 案 B 的判别力声明过宽。**

- §3.7:136 声称案 B「把红分支改成 best-effort…→ 本案红」。实际案 B 走「红轮写 log 成功」的正常路径；若把红分支改成 best-effort，只要写成功，案 B 仍绿、不会变红。该断言只在注入写失败时才成立。
- 真正承重「失败轮次零变化」的是 §5 B2 的 `git diff` 逐字节核读（红分支字节不变 ⇒ 行为不变）；案 B 只证「正常红轮 envelope 六字段形状回归」。
- 补强（三选一）：① 修正 §3.7 案 B 措辞为「仅证正常红轮 envelope 形状回归，失败写语义由 B2 承载」；② 给案 B 加故障注入臂（沙箱把 `.claude/verify-logs` 预置为同名文件或只读，使 `writeGateLog` 抛错，断言红轮仍 fail-loud、不产出 `logFile=null` 的 envelope）；③ 明写 B3 由 B2 diff 核读承载。

**F3（非阻断 · 行锚精度）：§3.1:82 的失败语义机制行锚不精确。**

- 红轮写失败当前经「child 未捕获异常 → 空 stdout → 父 `parseChildPayload` 失败 → `:402-408` exit 2」，而非「父 `runBearing:398-401` 判 `![0,1]` exit 2」。结论不变，仅需改述。

### 9.4 非阻断观察（记录，供实施与后续批）

- **N1**：`enforce-scope.mjs:88` 把 `.claude/verify-logs` 列入「有意不覆盖（残余面）」注释，非消费方；§2.4「机读消费方恰三处」经独立普查成立。
- **N2**：`verifyLogsPresent` 仅 `supervisor-verdict-gate.mjs:663` 写入，全仓无读取方，成立。
- **N3 判档**：`verify.mjs` 不是 `gov-change.md`「核心落点」清单（前置升级闸 / 共享闸库 / merge-driver / deny-main-push / enforce-scope / INDEX landed 推导）成员；`architect.md:47` 明许作者实施核心合并逻辑、仅「改判定语义」时才启第二独立核验。本设计判定语义零改（`ok = exitCode === 0`、gate 选择、exit code、ErrorEnvelope 字段全不变），故 **常规档、不启 landing-verifier**，作者 §4.5 判档成立。§4.5 所提「G4 可满足性变宽 ≠ 合并门变弱」我同意：无新增旁路，log 只在真实 `spawnSync` 之后写，抗伪造仍靠 write-once bearing round fact + G6 `validateVerifierEvidenceRefs`（`:229-281` 四重校验，零改）。
- **N4**：`defer` 闸（executor-skipped、exit 0）现在也落绿 log，故 G4 可被「全 defer 的 scoped 轮」满足。这是 defer 既有语义延伸（defer 是设计特性、`notExecuted` 已入 round fact），非本设计新洞，可在 §7 记一句。
- **N5**：manifest 第二行 dev-loop.md 自愿同步非必要（见 9.2 第 8 条）。
- **N6**：`.claude/verify-logs/` 不入仓、不入图索引（`.gitignore:45`、`.claude/.cbmignore:14`）亲核成立；battery fixture 自造 `fixture.log`（`supervisor-gate-check.mjs:418`）不依赖 `verify.mjs`，A4 旁证成立。

### 9.5 特别核：验收是否足以证明「失败轮次零变化」

**以 B2 为承重即足够，但前提是删去 §3.3（F1）。** 分述：

- 案 A 证明「绿轮落 log」；案 B 证明「正常红轮 envelope 六字段形状回归」；**案 B 本身不足以**证明「写失败语义零变化」（F2）。
- 「失败轮次零变化」的真正机械保证 = §5 B2 的 `git diff main...HEAD -- .claude/hooks/verify.mjs` 逐字节核读：红分支字节不变 ⇒ 行为不变。故必须删 §3.3 才能让 B2 字面成立。
- **补强点**：① 删 §3.3（F1），使红分支调用与函数体逐字不动；② B3 由 B2 承载时，B2 须覆盖 `if (!ok) {…}` **整段分支体**（含 `failures.push(error); continue;` 与循环后 `failures.length > 0` 全红清单块，`verify.mjs:300-318`），不仅 error 对象构造段，方能证 final 相位「遇红不停」零改；③ 修正 §3.7 案 B 判别力措辞或加故障注入臂（F2）。

### 9.6 复审范围

回修后 R2 只核：F1 是否闭合（§3.3 已删或 B2/§3.1 已按次选改写且自洽）、F2/F3 是否顺手修正、以及修订是否引入新矛盾；不重复全量四查。方向层面无争议，不升级 human，不进 Track A。

---

## 10. 作者 R1 回修记录（architect，2026-11-17；§9 判官区未改一字，本节为 append）

**处置总述**：F1 采 reviewer 推荐的第一处置（删 §3.3 三行头部），**不走**次选（不弱化 B2）；F2 采三选一的 ①+③；F3 按改述修正；两项补充收紧（B2 覆盖整段分支体、dev-loop.md 标注非必要）均已落文。R1 N4 已记入 §7.1 附记。

| 发现 | 处置 | 落点（本文小节） |
|---|---|---|
| **F1（阻断）** §3.3 头部三行与 B2 互斥 | 采推荐处置：删去 `ok=` / `exit=` / `changeId=` 三行头部；`writeGateLog` 签名与红分支调用站点 `:289` 声明为逐字不动；绿分支另起 `try/catch`。撤回项转为后续候选并写清四条重评触发 | §3.3 改为撤回说明（保留编号占位，不重排 §3.4–§3.7）；§1 非目标新增第 2 条；§3.1「改成什么」重写；§5 B2 第 3 项；§7.4 新增留根项 |
| **F2（非阻断）** 案 B 判别力声明过宽 | 采 ①+③：案 B 措辞收窄为「仅证正常红轮 envelope 六字段形状回归」，并明写「红轮写失败语义零变化由 B2 逐字节核读承载」；B3 由 B2 第 1+2 项承载、不再「择一」。不取 ②（故障注入臂）并给出理由（平台敏感 + 被 B2 以更强形式覆盖） | §3.7 案 B 与其后新增的分工段；§5 B1 / B3 |
| **F3（非阻断）** §3.1:82 行锚不精确 | 改述为「child 未捕获异常退出、stdout 无 payload → 父 `parseChildPayload:334-344` 解析失败 → bearing 路径 `:402-408` `process.exit(2)`」，并加一行「不经 `:398-401` 基础设施分支（Node 未捕获异常默认 exit 1，`1 ∈ [0,1]` 不命中）」的精度注；连带修 §3.2「备选不取」理由里同源的 infra 措辞 | §3.1 理由项 + 新增行锚精度注；§3.2 第 3 项 |
| **补充收紧 1** B2 须覆盖整段分支体 | B2 重写为三处明写覆盖：`if (!ok) {…}` 整段（`:288-309`，含 `failures.push(error); continue;` `:300-306` 与 scoped 的 `console.log` + `exit(1)` `:307-308`）、循环后全红清单块（`:312-318`）、`writeGateLog` 函数体（`:191-203`）；并列出「允许的 diff 只有绿分支新增 + self-check 两案 + 必要注释」，落在三处内的任何字符变化 = B2 不通过 | §5 B2 |
| **补充收紧 2** dev-loop.md 标注非必要 | 明写「自愿防回退小补、非必要；**最终未改不影响本批合规**，落地时不得被当作必改项」，并引 R1 §9.2 第 8 条 / N5 与 `gov-change.md`「允许计划内文件最终未改」 | §2.4 末项；§6 manifest 第二行理由列 |
| **N4（记录）** 全 defer 的 scoped 轮也满足 G4 | 记入 §7.1 附记：属 `defer` 既有语义延伸（`notExecuted` 已入 round fact、`final` 相位永不 defer 兜底），本批不改；若日后收紧 G4「跑过」语义应与归属/新鲜度判据同批 | §7.1 附记 |
| **N1 / N2 / N3 / N5 / N6** | 均为 R1 对作者判断的亲核确认，无需回修；N3（常规档、不启 landing-verifier）已在 §8 标为「R1 已结」 | §8 |

**连带一致性核对（防新矛盾，供 R2 直接核）**：撤回 §3.3 后，全文对 `changeId=` 头部、「归属钩子」的引用已逐处清理 —— §3.4 末项改为指向 §7.2 + §7.4 互为前置；§7.1 改为「让 log 可归属（带 changeId，见 §7.4）」；§5 B2 第 3 项删去「§3.3 只加内容头部行」括注；§6 manifest 第一行理由删去「log 头部自描述」。§3.4–§3.7 编号未变，故 §1 非目标（引 §3.5 / §3.6 / §7.1）、§2.2、§3.6（引 §3.4）、§4.5、§5 A1（引 §3.7）等既有交叉引用与 §9 判官区的行内引用全部仍然有效。

**结构 / 机械锚复跑结果（回修后以 `.claude/scripts/lib/governance-gate-shared.mjs` 实跑，非声称）**：

- `parseDirectImplementationProposal(PROPOSAL 全文)` → `direct=true`、`flowCount=1`（gov-flow 标记恰一条，回修未新增标记）、`scope=[{.claude/hooks/verify.mjs | architect | ordinary}, {.claude/references/dev-loop.md | architect | ordinary}]`、`errors=[]`、`landed=false`（未冻结、无 verdict GREEN、无钉点，符合当前相位）。
- `isFrozen=false`：状态行改为「DRAFT v2（R1 回修后，待 architect-reviewer R2 复审 → 主线程标 FROZEN）」，按 `isFrozen` 的「值部先出现者主导」规则由 DRAFT 主导，不会因句尾提到 FROZEN 而误判冻结。
- verdict 锚全文恰一处 = 第 270 行 `verdict: BLOCK(F1)`（判官 R1 所写），解析结果 `verdict="BLOCK"`。§10 未引入任何 `VERDICT_STATUS_RE` / `VERDICT_HEADING_RE` 可命中行 → 作者没有、也不可能用本节覆盖判官判决（后出者胜规则下 BLOCK 仍为当前终态）。
- `reviewedCommit` / `reviewedTree` 解析均为 `null`，全文无 `REVIEWED_PIN_RE` 可命中行 → 无伪钉点。
- scope 谓词复核：`isDirectImplementationPathInScope` 对 `.claude/hooks/verify.mjs` = true、`.claude/references/dev-loop.md` = true、`.claude/hooks/supervisor-gate-check.mjs` = **false** → §5 C2「合并门与 Stop 门不得入 diff」是机械可强制的，不靠自觉。
- §9 判官区完整性：本次回修唯一触及 §9 的操作 = 以 §9.6 末行为锚在其后 append §10，锚行逐字保留；`verdict: BLOCK(F1)` 行与 §9.6 正文均在原位原样（机械核：该行仍以独立行存在）。
- 工作树洁净度：回修只改本 PROPOSAL 一个文件，`git status --porcelain` 仍只有 `?? delivery/review/261117-gov-review-g4-verify-log-green-rounds/`（证据尾），无任何代码或 canonical 改动。

---

## 11. architect-reviewer 设计审 R2（2026-11-17）

verdict: GREEN

> R2 只核 R1 §9.6 限定范围（F1 闭合 / F2·F3 处置 / 新矛盾），不重跑全量四查。源文件与工作树亲核：`gov/g4-verify-log-fix` @ `390a7297`，`git status --porcelain` 仅 `?? delivery/review/261117-gov-review-g4-verify-log-green-rounds/`（证据尾），无 tracked diff；`.claude/hooks/verify.mjs` 本次回修未动（`git hash-object` = `516b8da00119f447a5676b8149aca80e26c47327`）。§9 判官区经复核未被改动（`verdict: BLOCK(F1)` 仍在 §9 首行、§9.6 正文原位）。本节为设计审 GREEN：冻结意图 / 边界 / 关键方案 / 验收标准。**非**真实 diff 审，不写 `reviewedCommit` / `reviewedTree` 钉点（钉点属后续实现评审）。

### 11.1 F1 闭合判定：**CLOSED**

四处口径已收敛且与源头一致，无残留互斥：

- **§1 非目标第 2 条（`PROPOSAL:23`）**：明写「不改 `writeGateLog` 的签名、目录、文件名规则、返回值形状与 log 内容头部…红分支调用站点 `verify.mjs:289` 逐字不动，本批只在绿分支新增一次 best-effort 调用」。
- **§3.1 改成什么（`:81`）**：`writeGateLog` 的签名 / 目录 / 命名 / 返回路径 / 头部「沿用现有实现、逐字不动」，绿分支「在 `if (!ok) {…}` 之外另起一次 best-effort 调用」。
- **§3.3（`:102-112`）**：改为撤回说明，保留编号占位（不重排 §3.4–§3.7）；原三行头部已删，能力归 §7.4。
- **§5 B2 第 3 项（`:197`）**：`writeGateLog` 函数体（编辑前 `:191-203`）「无字符变化——签名、目录、文件名规则、内容头部四行、返回值形状全部不动」，并删去了 R1 所引「§3.3 只加内容头部行」的旧括注。

源头对证：`verify.mjs:289` 现为 `const logFile = writeGateLog(gate, out);`（两参调用），`writeGateLog`（`:191-203`）现签名 `(gate, output)`；只要实现按「绿分支另起一次调用、不动签名与红分支」，B2 第 1/2/3 项的三处「无字符变化」即字面成立。R1 F1 的互斥根因（头部加字段⇒红分支必改参⇒与字节不变冲突）已消除。**CLOSED。**

全量检索确认无残留「本批加头部 / 归属钩子」旧笔：`ok=`/`exit=`/`changeId=` 在正文仅出现于撤回说明（§1:23、§3.3:104/110）、后续候选（§7.4:235-236）与 R1/§10 历史记录；`§3.4:119` 已改指「§7.2 + §7.4 互为前置」、`§7.1:231` 已改指「见 §7.4」、`§6 manifest` 第一行已去「log 头部自描述」措辞。

### 11.2 F2 / F3 判定：均接受

- **F2——接受（①+③）。** §3.7 案 B（`:142`）已把判别力收窄为「只证**正常红轮** ErrorEnvelope 六字段形状回归」，并明写「不能证红轮写失败语义零变化」；新增分工段（`:143`）把「写失败语义零变化」明指由 §5 B2 逐字节核读承载。B1（`:193`）与 B3（`:199`）均已据此改口径（B3 改由 B2 第 1+2 项承载、不再「择一」）。措辞与 R1 F2 的补强 ①+③ 逐条对应。
- **F3——接受。** §3.1 理由项（`:83`）已改述为「`writeGateLog` 抛错 → child 未捕获异常退出、stdout 无 payload → 父 `parseChildPayload:334-344` 解析失败 → bearing 路径 `:402-408` `process.exit(2)`」，并新增行锚精度注（`:84`）明写「不经 `runBearing:398-401` 的 `![0,1]` 分支（Node 未捕获异常默认 exit 1，`1 ∈ [0,1]` 不命中）」；§3.2 备选理由同源同步。三处行锚（`:334-344`、`:398-401`、`:402-408`）亲核与现状一致。

### 11.3 「要不要故障注入臂 ②」——**不要**

明确表态：**不要求 ②（故障注入臂）。** 理由：

1. **更强覆盖已被 B2 占据。** 「失败轮次零变化」的机械保证是 §5 B2 对 `if (!ok) {…}` 整段分支体（`:288-309`）、全红清单块（`:312-318`）、`writeGateLog` 函数体（`:191-203`）的逐字节核读——红分支字节不变 ⇒ 写失败语义不变。这是字节级、覆盖全部分支的强保证。故障注入臂只采样「写失败」单一路径，覆盖**弱于**字节核读。
2. **② 引入平台脆性而收益不抵。** 要触发 `writeGateLog` 抛错（把 `.claude/verify-logs` 预置为同名文件 / 只读目录），`mkdirSync({recursive:true})` / `writeFileSync` 在 Windows 与 POSIX 的错误码与触达方式不一致，会把 hermetic 案变成平台敏感案；其欲证之事已由 B2 以更强形式覆盖。
3. **B2 是 diff 核读而非机测不构成缺口。** 该保证本就是真实 diff 审的固有动作（本轮 R2 后的实现评审即会执行 B2），把「写失败语义」交给它符合「diff 逐字节 ⇔ 行为不变」的强对偶，不需要再用一个弱测试重复声明。

结论：作者 ①+③ 的取舍成立，**不补 ②**。

### 11.4 新矛盾复核：无阻断项；提一条实现防呆

回修未引入新矛盾。§3.4–§3.7 编号未重排，既有交叉引用（§1 引 §3.5/§3.6/§7.1、§3.6 引 §3.4、§5 A1 引 §3.7 等）均仍有效；§7 新增留根项编为 §7.4、原 4/5 顺延 5/6，与 §3.4:119 / §7.1:231 的「§7.2 / §7.4」指向自洽。

一条实现防呆（非阻断，供作者实施时避开，否则会当场触 B2）：**绿分支务必「另起独立块」，不得写成 `} else {` 接在 `:309` 的 `}` 上。** 若用 `else`，`:309` 由 `    }` 变为 `    } else {`，即落在 B2 第 1 项「`:288-309` 除位置外无字符变化」的字符变化内、B2 直接不通过。建议形状：在 `if (!ok) {…}` 之后、`for` 循环体内另起 `if (ok) { try { writeGateLog(gate, out); } catch (e) { /* stderr 点名 */ } }`——final 相位红闸的 `continue`（`:305`）与 scoped 红闸的 `process.exit(1)`（`:308`）都在 `if (!ok)` 内，红色路径不会落到绿块，故无重复写；绿闸正常落到绿块写一份。此形状下 `:288-309`、`:312-318`、`:191-203` 三处字节不动，B2 可字面成立。

另核：§3.1:81 的「在 `if (!ok) {…}` 之外另起一次 best-effort 调用」已蕴含「另起」，与上述防呆一致；实施时不得为省事改用 `else`。

### 11.5 判档复核

维持 R1 §9.4 N3 的结论：**常规流程档、不启 landing-verifier**。判定语义零改（`ok = exitCode === 0`、gate 选择、exit code、ErrorEnvelope 字段全不变），`verify.mjs` 不在 `gov-change.md`「核心落点」清单，`architect.md:47`「改判定语义」才启第二独立核验的条件未触发。回修未改变这一判断的依据。

### 11.6 冻结放行

设计审 GREEN。主线程可标 FROZEN（冻结意图 / 边界 / 关键方案 / 验收标准）；作者随后实施，实施后由本 reviewer 做真实 diff 审（B2 逐字节核读为其中一环），届时以 `reviewedCommit` / `reviewedTree` 各一行钉点。本 R2 GREEN 不是真实 diff 审、不产生钉点。

---

## 12. 作者实施相位记录（architect，2026-11-17；FROZEN v2 后第一次实施尝试）

**状态：代码未落地（`.claude/hooks/verify.mjs` 零改动），实施被机械边界 fail-closed 拦下；改动已在仓外 harness 全链验证完毕并交接。**

### 12.1 阻塞事实（逐字凭据，未绕过）

作者本会话的树根是 `c:/AI/KidPal-pod-m2`（Pod worktree，分支 `dev/pod-m2`），而 FROZEN 指定的实施树是 `C:/AI/KidPal-gov-g4`（分支 `gov/g4-verify-log-fix`）。对 gov 树 `.claude/hooks/verify.mjs` 发起 Edit 时被 PreToolUse 钩子拦截：

```
PreToolUse:Edit hook error: [node "${CLAUDE_PROJECT_DIR}/.claude/hooks/enforce-scope.mjs"]:
scope(architect): scripts/hooks 作者实现只允许当前 gov worktree，禁止跨树或不可识别树写入。
```

判据：`enforce-scope.mjs:301-304`（architect 写 `^\.claude\/(?:scripts|hooks)\//` 时调 `architectNonMainViolation`）+ `:168-173`（目标树 root 必须与会话 `CLAUDE_PROJECT_DIR` 的树 root `samePath`，否则 fail-closed）；同一约束经 `:243-249` 覆盖 Bash（`git -C/--git-dir/--work-tree` 跨树 `add`/`commit` 直接 block）。

作者**没有**绕过：未用 Bash 重定向写 `.claude/**`（`universal.bashDenyWrite` 恒拦）、未用 `git -C` 跨树提交、未改 settings / scope-map / 钩子、未请求放宽任何权限。

### 12.2 已完成的工作（仓外 harness，非仓内落地）

在 `/tmp` 一次性 harness（gov 树 `.claude/scripts` + `verify-gates.json` + `verify.mjs` 副本，blob 与工作树一致 = `516b8da00119f447a5676b8149aca80e26c47327`）里落笔并跑通全部合并前验收项，实测输出见交接件 §6：

- **A1**：`--self-check` exit 0，`{"pass":18,"fail":0,"total":18,"ok":true}`（原 16 案 + 新 2 案），案名 `green-round-writes-gate-log` / `red-round-envelope-unchanged` 逐条输出 `[PASS]`。
- **A2 判别力**：移除绿分支写 → 案 A `[FAIL] (exit=0 ok=true logs=0)`、案 B 仍 PASS、exit 1；另做案 B 判别力补充演示（把红分支 `logFile,` 改 `logFile: null,`）→ 案 B `[FAIL]`、案 A 仍 PASS、exit 1。两次演示后均还原并以 `git hash-object` 复核 blob 一致。
- **B2 逐字节**：三处保护区机械等值核全 `IDENTICAL`（`writeGateLog` 函数体 sha `05a37f6e49e4fb94` / `if (!ok) {…}` 整段 sha `c632d016b5c7e9d8` / 循环后全红清单块 sha `7cc94d8965b451bf`，改前改后同值），区间与 FROZEN 指定的 `:191-203` / `:288-309` / `:312-318` 一一对应。
- **删除行普查**：`113 insertions(+), 1 deletion(-)`，唯一删除行 = `import { dirname, resolve } from 'node:path';`（改为含 `join` 的版本并新增 `node:os` 的 `tmpdir` 导入，均为 self-check 两案所需）；六个 hunk 分别落在 import 块 ×3、绿分支插入点、battery 函数、selfCheck 接线，**三处保护区零 hunk**。
- **B3**：by B2 第 1+2 项（按 FROZEN §5 B3）。
- **A3 等价端到端**（仓外多闸沙箱、legacy 父模式；未在 gov 树跑，遵守 §4.4 与 FROZEN 禁令）：全绿 scoped → exit 0 / 3 个非空 log；中间红 scoped → exit 1 / 2 个 log 且第三闸未跑（遇红即停不变）；final 相位含 `timing=full` 闸 → exit 0 / 2 个 log；同沙箱 scoped 清目录后 → exit 0 / 1 个 log（full 闸被跳过 = gate 选择不变）。绿 log 与红 log 内容形状一致、头部仍四行。四形态均无「gate log 写失败」点名。
- **形状纪律**：绿分支按 R2 §11.4 防呆**另起独立 `if (ok) {…}` 块**，未用 `} else {`，故 `:309` 收尾行零字符变化。

### 12.3 交接与下一步

- 交接件：同目录 `implementation-handoff.md`（作者预备实施输入，**非字节合同**）——含四处编辑的旧锚点与新文本、实施会话必须自跑的 8 项验证清单、可复用的 `b2check.mjs`、以及可选的 `dev-loop.md` 一行建议文本。
- 待主线程处置：在**树根为 `C:/AI/KidPal-gov-g4`** 的会话里重派 architect 实施（本文件属 ordinary、作者实施为宜），实施会话须自己落笔、自己重跑 §5 清单（不得引用本会话的 harness 结果作为通过凭据）、自提真实实现 commit，并在本 PROPOSAL append 自己的实施证据段。
- manifest 第二行 `.claude/references/dev-loop.md`（可选、非必要）：作者倾向改（防回退价值），因同一机械阻塞未落地，一并交实施会话裁；不改亦合规（§2.4 / R1 N5）。
- 本会话结束时的 gov 树状态（诚实记账）：`.claude/hooks/verify.mjs` blob 仍为 `516b8da00119f447a5676b8149aca80e26c47327`（未改）；无任何 commit；`git status --porcelain` 仅 `?? delivery/review/261117-gov-review-g4-verify-log-green-rounds/`（本 PROPOSAL + 交接件，均属证据尾）。
- 作者不自评通过、不写钉点、不合并。真实 diff 审与 `reviewedCommit` / `reviewedTree` 钉点归 architect-reviewer。

## 13. 实施记录 · 主线程代落地（2026-11-17）

> **实施主体 = 主线程**。依据：`gov-change.md`「条件加强 · 主线程代落地：保护面、写权不足或 human 明确选择」——architect 会话的树根是 `C:/AI/KidPal-pod-m2`，`enforce-scope.mjs` 对 `.claude/hooks/**` 的跨树写入 fail-closed，作者无法在本批实施（凭据见 §12 与 `implementation-handoff.md` §1）。作者≠判官不变：真实 diff 审仍由 architect-reviewer 独立执行并钉点。

**改动面**（按 FROZEN v2 与交接件 §3）：
- `.claude/hooks/verify.mjs`：四处编辑（import 增 `mkdtempSync/readdirSync/rmSync/statSync/join/tmpdir`；绿分支另起独立 `if (ok) { try { writeGateLog(gate, out); } catch { stderr 点名 } }`；`runGateLogBattery` 两案；`selfCheck` 接线）。三处保护区（`writeGateLog` 函数体、`if (!ok){…}` 整段、循环后全红清单块）零字符变化。
- `.claude/references/dev-loop.md`：`hooks/verify.mjs` 行追加「每闸每轮落 gate log」一句（manifest 第二行，可选，已改）。

**实跑验证（本树，2026-11-17）**：

| 项 | 命令 | 实跑结果 |
|---|---|---|
| A1 | `node .claude/hooks/verify.mjs --self-check` | `pass:18 fail:0 total:18 ok:true`；`[PASS] green-round-writes-gate-log` / `[PASS] red-round-envelope-unchanged` 逐条输出 |
| A2 判别力 | 临时移除绿分支 `writeGateLog(gate, out);` 后重跑 | `[FAIL] green-round-writes-gate-log (exit=0 ok=true logs=0)`，案 B 仍 PASS；随即还原并复跑回 18/18 |
| B2 逐字节 | `git diff -U0 -- .claude/hooks/verify.mjs` | `113 insertions(+), 1 deletion(-)`；唯一删除行 = `import { dirname, resolve } from 'node:path';`；六 hunk = import ×3 / 绿分支插入 `@@ -309,0 +315,22 @@` / battery `@@ -455,0 +483,82 @@` / selfCheck 接线 `@@ -493,0 +603,3 @@`；三处保护区零 hunk |
| A3 端到端等价 | 仓外沙箱（`C:/tmp/g4e2e`，CLAUDE_PROJECT_DIR 指向沙箱，单绿闸） | `{"ok":true,"phase":"scoped","round":1,"observedGateExit":0}`；`.claude/verify-logs/1794928789700-probe-green.log` 非空、头部四行（`gate=` / `kind=` / `cmd=` / `phase=`） |
| A4 | `node .claude/hooks/supervisor-gate-check.mjs --self-check` | `Battery: 10/10 pass` / `OK` |
| C1–C3 | `git diff --stat` | 仅 `.claude/hooks/verify.mjs` 与本批证据尾；`supervisor-gate-check.mjs` / `supervisor-verdict-gate.mjs` / `verify-gates.json` 零 diff |

**诚实记账**：① 基线与逐字节核读使用**工作树副本**（CRLF），未用 `git show HEAD:`（LF，会假失败——交接件 §2 的坑）。② A3 等价跑在仓外沙箱，未在 gov 树跑（避免写 tracked 的 `delivery/taskpackages/current/verdict.json` 与 `verify-runs.jsonl` 出 scope）。③ 本记录不自签 verdict、不写钉点；钉点归 architect-reviewer 真实 diff 审。
