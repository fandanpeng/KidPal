# PROPOSAL · 261016-gov-review-triage-entry-identity

> **状态：FROZEN v0.3 · 2026-10-17 · 主线程凭据自冻**（凭据三件：human 立项 ack 2026-10-16「同意立项」+ human 裁 OQ-1~4〔OQ-4=(a) 显式批准破自改递归边界〕+ architect R1 GREEN / R2 GREEN 维持、无 BLOCKING、无升级 human〔§7 `:323` / `:352`〕）
> **承载 ARQ**：`ARQ-261016-intake-entry-identity-unstated`（本批 = 该 ARQ 的主体批：承载建议 1/2/3/5 + 该条目自身缺陷 ①②；建议 4〔check 齿〕已由 human OQ-3=(b) 裁 convention-only 留根）。
> **review 轨**：Track B（触 G1 = `AGENTS.md`，gov-change 轻路径判据 ③ 命中指令根、恒 Track B——哪怕只改一个字符；本批不是轻路径，按 Track B 主路径起草完整 PROPOSAL。SSOT：`.claude/references/gov-change.md`「轻路径判据」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（纯文档措辞显式化 + 表述消歧；不触流程 / 门权 / 契约面 / hooks / settings / scope-map / verify-gates；触 G1 指令根 + architect.md self-def〔OQ-4=(a) human 显式批准破界〕，但全部改法为措辞级、零权限 / 工具 / 写域 / 门语义变化，风险归 low）。
> **review profile**：G1+G2+G4（G1 = `AGENTS.md` ×1；G2 = references ×3〔change-triage / change-intake / architect-operating-model〕+ command ×1〔triage-change〕+ agent ×1〔architect，self-def〕；G4 = debt 账本 ×1 + 生成件 INDEX ×1；G3 / 契约面为零。**触 selfDef 硬排除**——architect.md 自身定义正文，已由 human OQ-4=(a) 显式批准破界；不触 agentFrontmatter〔只改正文 `:26`、不动 frontmatter description/tools/name〕、不触 reviewLoopAgentDef〔architect 非 review-loop agent〕、不触 newGate / contracts / specContractDrift。判档归 architect-reviewer，selfDef 触面在此显式标注）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
AGENTS.md
.claude/references/change-triage.md
.claude/references/change-intake.md
.claude/commands/triage-change.md
.claude/agents/architect.md
.claude/references/architect-operating-model.md
docs/debt/gov/ARQ-261016-intake-entry-identity-unstated.md
docs/debt/gov/INDEX.md
```

---

## 0. 想实现的效果与差距

**目标效果**：把一句已存在于 260928 FROZEN PROPOSAL §目标效果、却从未进入常驻指令面的定义**正面写进 canonical**——`/triage-change` 是**跨面变更的编排入口（容器）**：human 说一次意图，它把需求 / 架构+契约 / 治理 / 实现四面合在一次陈述里做探测、落 spine，再按 facePlan 派发（需求 / 架构+契约 / 治理三面由它派发到各自 loop，实现面五档分档是它对该面的收尾动作；轻面本会话内联、重面 headless 拉起下游编排会话）。读者（含编排者本人）不再从「四个平等面入口 + 一个独立跨面账本」的错误心智模型出发。

**本批不是勘误而是定义显式化**（决定 Track B 恒定与 riskTier 归 low 的实质理由之一）：`grep -rn "编排入口" .claude AGENTS.md CLAUDE.md` 实测 = **零命中**——「`/triage-change` 是跨面编排入口」这句**从未以正面陈述形式存在于任何 canonical**，只以从句寄生在「关于 dev 门的段落」里（`change-triage.md` §0「实现面留本门分档，不再整单踢走」+ `commands/triage-change.md` step 6「轻面注明内联执行（本会话继续派发）」）。所以本批是把一句「已存在于 FROZEN PROPOSAL、却从未进入常驻指令面」的定义补写为正面陈述，而非纠正一句写错的旧句。

**现状差距**（成因机理三条件，ARQ 追加③按权重排序）：

1. **最高权威面每次会话重新灌入旧定义**：`AGENTS.md` 是常驻指令（会话启动即加载、经 `CLAUDE.md` `@AGENTS.md` 传导），其「四面入口」节把 `/triage-change` 写成「实现面变更」的并列项、且「跨面 intake」另起一段、一字未提 triage 就是编排入口。新定义被读到一次，旧定义每次会话重新灌入一次——读一次压不住灌多次。
2. **新定义全文无正面陈述，仅以从句寄生在旧段落里**：全部载体 = `change-triage.md` §0「实现面留本门分档，不再整单踢走」+ `commands/triage-change.md` step 6「轻面注明内联执行（本会话继续派发）」——两句都是「关于 dev 门的段落」里的补充从句，读起来像脚注。
3. **先入 schema 的追加式吸收**：首次接收的 `AGENTS.md` + `change-intake.md` §8（「triage 实现面」）形成 schema；后读到的「四面探测（intake）」被编码为「triage 另外还兼做 intake」，而非「triage 就是跨面全流程本身」——新定义未覆盖旧 schema，只叠加上去。

**两次独立误读实证**（同一读者 = 主线程编排者本人、同一文本、两种不同错法）：证据 1 是「没读定义就下结论」（把 triage 说成「只是实现面的分流前置门，不编排需求面与架构/契约面」）；append ① 是「**读完了定义仍然读错**」（把 triage 编排为「③ 实现面」、即把容器当链上一环），human 当场第二次指出：「triage 现在就是要把需求、架构跟开发都合在一起，它并不是只看开发的，所以你把它放到第三位是不对的」。第二次发生在完整阅读之后，排除了「读者偷懒」的解释，成因锁在文本本身。

**非目标**（防 scope creep）：

1. **零门权 / 零流程语义变更**：`change-triage.md` §2 判定表、§6 逃生门、五档本体（含 ⓪档 0 nano 判据节）、`change-intake.md` 的程序条文（§2 spine 形态 / §3 探测程序 / §5 发起机制 / §7 facePlan 修订权 / §7a companion）一律不动。本批只做「定义显式化 + 表述消歧」。
2. **不改谁裁 / 何时停 / 哪个门**：任何改法若被读成改了门语义，均已是纯措辞形态或列为 OQ。
3. **不立新门 / 新机检齿**（ARQ 建议 4 已由 human OQ-3=(b) 裁 convention-only 留根，见 §6）。
4. **不触 `docs/contracts/**`、hooks、settings、scope-map、verify-gates、scripts（G3 零触）**。
5. `change-triage.md` §2 判定表与五档本体一字不动（本批不碰任何档位判据）。
6. 不动 `CLAUDE.md` 正文（经 `@AGENTS.md` 导入传导、不另改——与 260928 T5 同一纪律）。
7. **不动 `change-triage.md` §6 逃生门**（`:113`「dev 分流门无权批准事实源变更」是逃生门承重段，属零门权纪律的显式豁免，本批不碰）。
8. **不动实现面单面路径的功能性描述**（`dev-loop.md:26/:32/:129`、`task-intake-and-slicing.md:247` 的「改 `firmware/**` 行为 → 过 `/triage-change` 分流门」是对单面实现变更的正确功能性描述、非入口身份断言）。

## 2. 问题、根因与改法（T1–T7）

**【硬项 · 起草期实证义务】**本节断言逐条已实测（2026-10-16）：

- **commit SHA 先核后引**（`git rev-parse` 输出 verbatim）：`2e43fc59b8ec5e9d2e1a3e0052d6973326e4c942`、`68ab0579297cacbf02b0b33c1fc0fe97b780d298`、`39646e88d66f349ef57fbf509b49976bbc237ae1` 三支均在册。
- **正面陈述零命中实测**：`grep -rn "编排入口" .claude AGENTS.md CLAUDE.md` = 零命中（无输出）；而「编排」作为他词（「主线程编排者」「编排循环」「编排者」）分散存在于 `triage-change.md:8`、`change-triage.md:15/:67`、`change-intake.md:47/:48`——唯独没有「编排入口」这一正面身份短语。
- **行号锚全部现跑核实**（`grep -n`，非转引）：`AGENTS.md:26/27/28/29/30/31`、`change-triage.md:1/:10`、`change-intake.md:4/:77`、`triage-change.md:2/:9`、`architect.md:26`、`architect-operating-model.md:67` 逐条命中，verbatim 引于各 T。
- **前序 commit 实际 diff 复证**（`git show <sha> -- AGENTS.md`，纠正 ARQ 证据 5 的失实）：
  - `2e43fc59`（260928-gov-review-cross-face-intake-spine）**改了 AGENTS.md**——`git show` 实测 `+` 两行：「需求面变更」行与「跨面 intake」行（含「CLAUDE.md 经 `@AGENTS.md` 导入传导」尾句）。即 **AGENTS.md 在那批是被同步过的**，ARQ 证据 5「前身批改了 triage 身份却没同步 AGENTS.md、`paths` 未含 AGENTS.md 即直接实证」失实。
  - `68ab0579`（261015-gov-review-face-loop-unify）**新立**「**四面入口（每面一个 loop，互为对偶）**」框架——`git show` 实测 `+` 行含「四面入口」节头、「架构+契约面变更」「治理面变更」「实现面变更（四档分流）」；「需求面变更」「跨面 intake」两行是 260928 已落的 context 行、被该批**收编**进新框架。即成因精确表述 = **后批框架把前批身份陈述重新收编成并列项**，不是「漏同步」。
  - `39646e88`（261016-gov-review-nano-tier，**2026-10-16 20:59 当天落地**）动的正是 `AGENTS.md:30` 这一行——`git show` 实测 `-四档分流` `+五档分流`，仍没看出身份问题。**本批的直接前序批踩过同一行**，此证据比 ARQ 引的 260928 证据强得多。
- **旧框架语面 census（三种词形 + §8 并列括注，复扫完备性先验的输入）**：`grep -rn "dev 侧变更分流前置门"` = 4 处（`change-triage.md:1`、`triage-change.md:2`、`triage-change.md:9`、`ARQ-261016-…:96`）；`grep -rn "dev 变更分流前置门"` = 1 处（`architect.md:26`）；`grep -rn "dev 分流门"` = 2 处（`architect-operating-model.md:67`、`change-triage.md:113`）；`grep -rn "triage 实现面" .claude AGENTS.md docs` = 3 处（`change-intake.md:77`、`ARQ-261016-…:33`、`ARQ-261016-…:48`）。全量展开取证与销项见 §3 复扫硬项。
- **脚本入口存在性核**：`ls -la .claude/scripts/landing-fidelity-check.mjs .claude/scripts/debt-render.mjs .claude/scripts/route-regression-check.mjs` 三件均在册；`landing-fidelity-check.mjs` `--preflight` 标志、`debt-render.mjs` `--check`/`--self-check` 标志、`route-regression-check.mjs` `--self-check` 标志均已在源码/头部注释亲核（见 §5）。
- **debt INDEX title 投影核**：`docs/debt/gov/INDEX.md:37` 的 title 列 verbatim = ARQ frontmatter `title` 的一字不差投影（T7 依据）。

**【硬项 · 目标文件锚清单】**每 T 末尾显式列「目标文件：」；landing-manifest ≡ 各 T 并集（T1–T6 各一个 + T7 两个 = 八个，逐一对应 §0 上方 manifest 八行）。

**条目形态（五段固定式，照此书写）**：

### T1 · AGENTS.md 入口节显式化编排入口身份 + 容器属性

**问题**：`AGENTS.md` 是常驻指令（每次会话加载、经 `CLAUDE.md` `@AGENTS.md` 导入），其「四面入口」节把 `/triage-change` 写成「实现面变更」的并列项，且「跨面 intake」另起一段——一字未提 `/triage-change` 就是跨面编排入口。读者（含编排者本人）从「四个平等面入口 + 一个独立跨面账本」的心智模型出发，这是两次误读的最高权威面成因（成因机理 1）。影响面 = 所有跨面变更（触面 ≥2）的第一次路由决策；代价形状 = 静默错路由（错认入口不报错、只会从错误的门进入、后续 anchors 顺错路径推进）。

**证据**：

- `AGENTS.md:26` verbatim：`**四面入口（每面一个 loop，互为对偶）**：`
- `AGENTS.md:30` verbatim：`**实现面变更**：走 `/triage-change` 五档分流 → `/build-feature`（dev loop 不变）。`
- `AGENTS.md:31` verbatim：`**跨面 intake**：触面 ≥2 的跨面 idea 先过统一 intake 建 spine（`delivery/intake/<changeId>.md`；程序 SSOT `.claude/references/change-intake.md`，260928-gov-review-cross-face-intake-spine 立）——统一 intake ≠ 统一门，各面门照旧各归各裁；spine 免冻，anchors 记录各面自己的冻结态。CLAUDE.md 经 `@AGENTS.md` 导入传导，不另改。`
- 权威形态原文（260928 FROZEN PROPOSAL §目标效果，`delivery/archive/changes/arch/260928-gov-review-cross-face-intake-spine/PROPOSAL.md` 节引〔省略 spine 落点括注，以 … 标示〕）：「`/triage-change` **升格为跨面 intake**：human 说一次意图，agent 对照四面 canonical 文本 + CRQ 未结账做触面探测，落一份 spine 制品 …，分面计划驱动各面门照旧各归各裁（统一 intake ≠ 统一门），各面定稿逐面回填 anchors 使跨面顺序可查；面间接力由主线程 headless 拉起下游编排会话完成，human 只在各面既有的人类触点出场。」
- 直接前序批实证：`39646e88`（261016-gov-review-nano-tier，2026-10-16 20:59）改动正是 `AGENTS.md:30` 这一行（四档→五档），没看出身份问题（起草期 `git show` 实测，见硬项节）。

**根因**：`2e43fc59`（260928）落地时 AGENTS.md 只加了「需求面变更」与「跨面 intake」两行，没把「`/triage-change` 升格为跨面 intake」这个身份**正面写进入口节**；`68ab0579`（261015）新立「四面入口（每面一个 loop，互为对偶）」框架时，把 260928 已落的「跨面 intake」段收编成第 4 面之后的第 5 个并列条目、并把实现面写成「只是一个面」。后批框架把前批身份陈述重新收编成并列项（不是漏同步）。

**改法**：三处替换，全部为措辞显式化，零门权 / 零流程语义变化（「dev loop 不变」「统一 intake ≠ 统一门，各面门照旧各归各裁」「spine 免冻」「anchors 记录各面自己的冻结态」「CLAUDE.md 经导入传导」等既有语义全部保留）：

1. 节头（`AGENTS.md:26`）替换后完整句：

```text
**四面入口（每面一个 loop，互为对偶；`/triage-change` 兼跨面编排入口）**：
```

2. 实现面行（`AGENTS.md:30`）替换后完整句：

```text
**实现面变更**：单面实现变更走 `/triage-change` 五档分流 → `/build-feature`（dev loop 不变；五档分流是 `/triage-change` 对实现面的收尾动作，不等于它只属实现面）。
```

3. 跨面 intake 行（`AGENTS.md:31`）替换后完整句：

```text
**跨面 intake**：触面 ≥2 的跨面 idea 的编排入口 = `/triage-change`——它把需求 / 架构+契约 / 治理 / 实现四面合在一次陈述里做探测、落 spine，再按 facePlan 派发（需求 / 架构+契约 / 治理三面由它派发到各自 loop，实现面五档分流由它收尾；轻面本会话内联、重面 headless 拉起下游编排会话）；程序 SSOT `.claude/references/change-intake.md`（260928-gov-review-cross-face-intake-spine 立）——统一 intake ≠ 统一门，各面门照旧各归各裁；spine 免冻，anchors 记录各面自己的冻结态。CLAUDE.md 经 `@AGENTS.md` 导入传导，不另改。
```

目标文件：`AGENTS.md`（改）

### T2 · change-triage.md 标题去「dev 侧」限 + §0 补正面身份陈述

**问题**：`change-triage.md` 是 `/triage-change` 入口下 architect 遵循的程序 SSOT，但标题「dev 侧变更分流前置门程序」把身份限死在「dev 侧」，且 §0「实现面留本门分档，不再整单踢走」把正面身份藏成「关于 dev 门的段落」里的从句——全文无一句形如「`/triage-change` 是跨面变更的编排入口」的正面陈述（成因机理 2）。这是「读完定义仍读错」的文本主犯之一：读者读到「实现面留本门分档」，只能推出「triage = 实现面那一环」。

**证据**：

- `change-triage.md:1` verbatim：`# Change Triage（dev 侧变更分流前置门程序）`
- `change-triage.md:10` verbatim：`- **四面探测（intake）**：变更进来先按 `change-intake.md` 做跨面探测——触面事实记 spine faceMap（触面 ≥2 建档）；命中触面事实（触 `docs/contracts/**` / systems spec / 跨系统结构性 / 存疑向严）→ 按 facePlan 路由（架构/契约面走 `/arch-change` loop），实现面留本门分档，不再整单踢走。governance 面（`.claude/agents` 定义 / gate）命中 → 路由 `/gov-change`。`

**根因**：标题是立门时（gov-review-architect-change-triage，FROZEN 2026-07-22）按纯实现侧分流设计取的；260928 批升格 triage 为跨面 intake 时只改了 §0 的第一刀措辞（「不再整单踢走」），没同步标题、也没补正面身份陈述。`change-triage.md:58`「dev 侧为何单轮而非对抗」是合法用法（解释实现面 review 单轮的理据，与 spec/governance 对抗 review 对举），本批不动。

**改法**：两处，措辞显式化（§2 判定表、§6 逃生门、五档本体一字不动）：

1. 标题（`change-triage.md:1`）替换后完整句：

```text
# Change Triage（变更分流前置门程序——/triage-change 兼跨面编排入口与实现面五档分档）
```

2. §0 新增「入口身份」bullet（插在「铁律」bullet 之前，作为 §0 首行）+ 四面探测行补限定语：

```text
- **入口身份**：`/triage-change` 是跨面变更的编排入口（容器）——它承载整条跨面链：先做四面探测落 spine，再按 facePlan 派发各面（轻面本会话内联、重面 headless 拉起）；实现面五档分档是它对该面的收尾动作，不是它等于实现面。
```

四面探测行（`change-triage.md:10`）中「实现面留本门分档，不再整单踢走」→「实现面留本门分档（分档是它的收尾动作，不是它等于实现面），不再整单踢走」（只加括注限定，其余不动）。

目标文件：`.claude/references/change-triage.md`（改）

### T3 · change-intake.md 自声明入口 + §8 消歧

**问题**：intake 程序 SSOT 全文没有「谁是本程序的入口」这句话——`:4` 抬头只说「消费关系」，从「可被消费的程序」视角写、不绑定入口；§8「开场即消费」把 triage 与其余四个并列成「各面流程入口」且标注「triage 实现面」，字面读法即「五个平等入口」，恰好覆盖 `:4` 抬头句的权威（反向加固，ARQ 证据 4）。误读者的推理链即由此形成（读 §8 → 认定五入口并列 → 认定 triage 只是一个面）。

**证据**：

- `change-intake.md:4` verbatim：`> 消费关系：`/triage-change` 与 [change-triage.md](change-triage.md) 消费本文件做四面探测与分面推进；[requirement-change.md](requirement-change.md) 的三档判据被本文件消费（不复制）；各面门照旧各归各裁——统一 intake ≠ 统一门。`
- `change-intake.md:77` verbatim：`- **开场即消费**：带 changeId 调用的各面流程入口（triage 实现面 / requirement-change 各档 / /arch-change / /gov-change / build-feature）先读 spine 的 faceMap/facePlan，不得要求 human 重述意图。`

**根因**：intake 程序从「可被消费」视角写（消费方 = triage），从不从「谁是入口」视角写；§8 括注是写「开场即消费」纪律时顺手把五个调用方并列枚举，没按「编排入口 + 被派发面」的层级写。

**改法**：两处，措辞显式化（程序条文 §2/§3/§5/§7/§7a 一字不动）：

1. §1（触发与比例化）新增「入口」bullet（插在「触发」bullet 之前，作为 §1 首行）：

```text
- **入口**：本程序的入口 = `/triage-change` 的四面探测步（step 1）——跨面 idea 从 `/triage-change` 进来，先做四面探测、落 spine，再按 facePlan 派发各面（含实现面五档分档）。
```

2. §8 开场即消费行（`change-intake.md:77`）替换后完整句：

```text
- **开场即消费**：带 changeId 调用的各面流程入口（triage 为编排入口、其余为其按 facePlan 派发面：requirement-change 各档 / /arch-change / /gov-change / build-feature）先读 spine 的 faceMap/facePlan，不得要求 human 重述意图。
```

目标文件：`.claude/references/change-intake.md`（改）

### T4 · triage-change.md description 与正文开篇补正面身份陈述

**问题**：`commands/triage-change.md` 的 description（`:2`）与正文开篇（`:9`）都是「dev 侧变更分流前置门」的旧框架——正确内容（step 1 四面探测 / step 6 本会话继续派发）被包在旧框架里，无正面身份陈述。description 是每次考虑调用本命令都会载入的路由信号，是「每次会话重新灌入旧定义」的第二个常驻面（成因机理 2/3）。不纳入则命令入口自身仍在灌旧定义，直接削弱本批目标。

**证据**：

- `triage-change.md:2` verbatim：`description: dev 侧变更分流前置门（对偶 /arch-change、/build-feature）。改 firmware/** 行为的变更开工前先过 architect 一次比例化 review：判五档（③拆包 / ②单包Full / ②单包Lite / ⓪档 0 nano / ①直接修）→ 人类确认/改判路由 → reviewed plan 按成熟度就近落地（大 feature 落 delivery/workstreams/<ws>/PROPOSAL.md，小 feature/bug 落 delivery/taskpackages/<id>/plan.md）→ 报告下一步命令。`
- `triage-change.md:9` verbatim：`这是 **dev 侧变更分流前置门**，对偶事实源侧的 `/arch-change` 与实现侧的 `/build-feature`：改 `firmware/**` 行为的变更（大 feature / 小 feature / bug 不分入口）开工前先过 architect 一次**比例化 review**，判它走哪档、把 reviewed plan 落到它的家。`

**根因**：description 与开篇框架都是立门时（2026-07-22）按纯实现侧分流写的路由信号；260928 批升格只改了正文 step 1/6，没改 description 与开篇框架。

**改法**：两处，措辞显式化（step 1–6 流程正文、人类触点①–③、convention-only 不变量一字不动；description 遵守 `.claude/rules/authoring-agents-skills.md`「description 只写 when-to-use + 职责边界 + 路由信号、不概括 workflow 正文」——跨面身份以「跨面编排入口」**身份信号**承载，流程链〔step 1 探测 / step 6 派发 / 五档〕留在正文、不进 description；保留全部路由承重信号：对偶 `/arch-change`、`/build-feature`、五档枚举、dev loop 落地家）：

1. description（`triage-change.md:2`）替换后完整句：

```text
description: 变更分流 + 跨面编排入口（对偶 /arch-change、/build-feature）。改 firmware/** 行为的单面实现变更开工前先过 architect 一次比例化 review：判五档（③拆包 / ②单包Full / ②单包Lite / ⓪档 0 nano / ①直接修）→ 人类确认/改判路由 → reviewed plan 按成熟度就近落地（大 feature 落 delivery/workstreams/<ws>/PROPOSAL.md，小 feature/bug 落 delivery/taskpackages/<id>/plan.md）→ 报告下一步命令。
```

2. 正文开篇（`triage-change.md:9`）替换后完整句：

```text
这是 **变更分流与跨面编排入口**（对偶事实源侧的 `/arch-change` 与实现侧的 `/build-feature`）：跨面变更（触面 ≥2）先做四面探测落 spine、再按 facePlan 派发各面；改 `firmware/**` 行为的单面实现变更（大 feature / 小 feature / bug 不分入口）开工前先过 architect 一次**比例化 review**，判它走哪档、把 reviewed plan 落到它的家。
```

目标文件：`.claude/commands/triage-change.md`（改）

### T5 · architect.md 自身定义去「dev 变更分流前置门」限（self-def，OQ-4=(a) 破界）

**问题**：`.claude/agents/architect.md:26` 是 architect 自身定义（每次被派发都载入），其「dev 变更分流前置门（`/triage-change` 入口）」把 architect 在 triage 里的职责限死在「对改自有实现面行为的变更判五档」，同源旧框架的第三个词形承载点（成因机理 2/3）。architect 自己也因此读不出「triage 兼跨面编排入口、其 intake 探测亦派 architect」。这是复扫 census 发现的旧框架第 4 个 `.claude` 面承载点，human OQ-4=(a) 显式批准破自改递归边界、随本批顺带改。

**证据**：`architect.md:26` verbatim：`- **dev 变更分流前置门**（`/triage-change` 入口）：对改自有实现面（固件业务 + 云端 shim）行为的变更判五档（③拆包 / ②单包Full / ②单包Lite / ⓪档 0 nano / ①直接修）、比例化 review 深度、落 reviewed plan——程序按 `.claude/references/change-triage.md` 执行（不内联）。`

**根因**：同 T2/T4——立门时（2026-07-22）按纯实现侧分流写；260928 升格后未同步 architect 自身定义里的这处身份框架。

**改法**：一处替换，措辞显式化、零门语义变化。本 T 属 architect **自改递归**（`docs/DEV-GOVERNANCE.md:212` §6 硬条款 3 逐字：「**自改递归**：修改自身定义、权限边界、工具、write scope，或影响自身权限的强制执行层文件 → 必须升级为 human-approved governance change；不得自批，不得仅凭同一 agent 的判断落地」）——已由 human OQ-4=(a) 明批破界。改法只加身份陈述、不删任何权限边界 / 角色约束 / 五档职责语义——「对改自有实现面行为的变更判五档」「比例化 review 深度」「落 reviewed plan」「程序按 change-triage.md 执行」全部保留。替换后完整句：

```text
- **变更分流前置门（`/triage-change` 入口，兼跨面编排入口）**：对改自有实现面（固件业务 + 云端 shim）行为的变更判五档（③拆包 / ②单包Full / ②单包Lite / ⓪档 0 nano / ①直接修）、比例化 review 深度、落 reviewed plan——程序按 `.claude/references/change-triage.md` 执行（不内联）；`/triage-change` 同时是跨面变更的编排入口（触面 ≥2 时先做四面探测落 spine、再按 facePlan 派发各面，实现面五档分档是它对该面的收尾动作）。
```

目标文件：`.claude/agents/architect.md`（改）

### T6 · architect-operating-model.md 路由节去「dev 分流门」限（镜像转述）

**问题**：牵连面复核（OQ-4 破界后对 architect.md:26 旧框架的镜像/转述枚举）发现 `.claude/references/architect-operating-model.md:67` 在「变更入口（261015 批元结构）」节把 `/triage-change` 写成「dev 分流门」——与 AGENTS.md 入口节同构的旧框架镜像（「不碰事实源 → 进 dev 分流门」+ 尾随一句「跨面编排归 triage 统一 intake」），把编排入口身份降成尾随从句。这是 architect 自身 operating model 的路由节，同为身份承载点。

**证据**：`architect-operating-model.md:67` verbatim：`- **不碰事实源**（纯实现面 / docs 行为变更）→ 进 `/triage-change` dev 分流门：architect 做比例化 review、返回五档裁决（③拆包 / ②单包Full / ②单包Lite / ⓪档 0 nano / ①直接修），程序见 [change-triage.md](change-triage.md)。`/triage-change` 是 **Part-2 工程门 / bootstrap 临时信任门**，非业务门；跨面编排（触面 ≥2）归 triage 统一 intake（change-intake.md spine）。`

**根因**：同 T1/T5——261015 批立「变更入口」元结构时把 triage 写成「dev 分流门」、跨面编排只作尾随从句（追加式吸收的又一落点）。

**改法**：一处替换，措辞显式化（只把「dev 分流门」去限 + 兼入口身份；尾随句「跨面编排归 triage 统一 intake」已含跨面指针、保留不动；「Part-2 工程门 / bootstrap 临时信任门，非业务门」门性质句保留不动）。替换后完整句：

```text
- **不碰事实源**（纯实现面 / docs 行为变更）→ 进 `/triage-change` 分流门（兼跨面编排入口）：architect 做比例化 review、返回五档裁决（③拆包 / ②单包Full / ②单包Lite / ⓪档 0 nano / ①直接修），程序见 [change-triage.md](change-triage.md)。`/triage-change` 是 **Part-2 工程门 / bootstrap 临时信任门**，非业务门；跨面编排（触面 ≥2）归 triage 统一 intake（change-intake.md spine）。
```

目标文件：`.claude/references/architect-operating-model.md`（改）

### T7 · ARQ 条目 title 更正 + paths 增列 + 承载注记

**问题**：本批承载 ARQ 的自身两处缺陷（ARQ ①②）须在本批一并处置——① `title` 写「未在任何 canonical 声明」，与其证据 2 自述「入口身份只存在于 triage 侧两个文件 + change-intake.md 一句抬头」直接矛盾；② `paths` 现列 `[AGENTS.md, .claude/references/change-intake.md]`，按本批改法集还须增列 `change-triage.md`、`triage-change.md`、`architect.md`、`architect-operating-model.md`（前三者是「被旧框架包着」、末者是「镜像转述」，均非「缺内容」）。

**证据**：

- ARQ frontmatter `title` verbatim：`/triage-change 的跨面 intake 入口身份未在任何 canonical 声明——AGENTS.md 入口摘要行未随 260928 批同步、change-intake.md 未自声明入口，读者会得出「triage 只是四面并列入口之一」的错误心智模型`
- ARQ frontmatter `paths` verbatim：`paths: [AGENTS.md, .claude/references/change-intake.md]`
- ARQ 追加② verbatim：「准确表述应为：**它在 canonical 里，只是不在最高权威那一路（AGENTS.md）**……`/gov-change` 处置时须一并更正 `title`——注意 `title` 是 `docs/debt/gov/INDEX.md` 的 title 列来源，改后须重跑 `node .claude/scripts/debt-render.mjs`。」
- ARQ 追加③ verbatim：「`.claude/references/change-triage.md` —— 其文件标题仍为「Change Triage（dev 侧变更分流前置门程序）」……`.claude/commands/triage-change.md` —— 正确内容在此……但被「dev 侧变更分流前置门」的框架包裹，无正面身份陈述。两者不是「缺内容」而是「内容被旧框架包着」。」
- `docs/debt/gov/INDEX.md:37` title 列 verbatim（= `title` 字段的一字不差投影，起草期已核）。

**根因**：`title` 是在证据 2 定稿前写的、未随正文修正；`paths` 是登记时的保守列举、未覆盖「被旧框架包着」与「镜像转述」的文件。两者都是登记期残件。

**改法**：三处（debt 账本 append-only 纪律：只改 frontmatter 机读字段 + append 承载注记，不改既有正文段）：

1. `title` 更正（frontmatter，替换后完整行）：

```text
title: /triage-change 的跨面编排入口身份未进最高权威面 AGENTS.md——入口身份只存在于 triage 侧两个文件与 change-intake.md 一句抬头，AGENTS.md「四面入口」节把它收编成并列的「实现面变更」，读者会得出「triage 只是四面并列入口之一」的错误心智模型
```

2. `paths` 增列（frontmatter，替换后完整行）：

```text
paths: [AGENTS.md, .claude/references/change-intake.md, .claude/references/change-triage.md, .claude/commands/triage-change.md, .claude/agents/architect.md, .claude/references/architect-operating-model.md]
```

3. 正文末尾 append 承载注记（append-only，不改既有行）：

```text
**2026-10-16 注记（本条目承载批）**：建议 1/2/3/5（AGENTS.md 入口节显式化编排入口身份 / change-intake.md 自声明入口 + §8 消歧 / change-triage.md 标题与 §0 补正面陈述 / commands description 与正文补正面陈述）已由 261016-gov-review-triage-entry-identity 批承载，并随 OQ-4=(a) 增列 architect.md（self-def、human 显式批准破界）与 architect-operating-model.md（镜像转述）两处同源框架。本条目自身两处缺陷随本批更正：title 措辞由「未在任何 canonical 声明」改为「未进最高权威面 AGENTS.md」（对齐证据 2 自述）；paths 增列 change-triage.md + triage-change.md + architect.md + architect-operating-model.md。建议 4（check 齿）按 human 裁 OQ-3=(b) convention-only 留根。
```

**改后必须重跑 debt-render**：`title` 是 `docs/debt/gov/INDEX.md` 的 title 列来源，改后跑 `node .claude/scripts/debt-render.mjs` 再生 INDEX（INDEX 是生成件、勿手改）。

目标文件：`docs/debt/gov/ARQ-261016-intake-entry-identity-unstated.md`（改）· `docs/debt/gov/INDEX.md`（改·机器再生）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 指令根 | `AGENTS.md` | G1 | T1 入口节三处替换；Hard Rules / CodebaseMemory 节 / 其余行零改动；CLAUDE.md 经导入传导不另改 |
| reference | `.claude/references/change-triage.md` | G2 | T2 标题 + §0 补正面陈述；§2 判定表 / §6 逃生门 / 五档本体（含 ⓪档 0 nano）不动 |
| reference | `.claude/references/change-intake.md` | G2 | T3 §1 补入口 + §8 消歧；程序条文 §2/§3/§5/§7/§7a 不动 |
| command | `.claude/commands/triage-change.md` | G2 | T4 description + 开篇补正面陈述；step 1–6 / 人类触点①–③ / convention-only 不动；description 保留全部路由信号 |
| agent | `.claude/agents/architect.md` | G2（self-def） | T5 `:26` 去「dev 变更分流前置门」限 + 补跨面身份；frontmatter / tools / write scope 零改动；自改递归（DEV-GOVERNANCE §6.3）human OQ-4=(a) 明批破界 |
| reference | `.claude/references/architect-operating-model.md` | G2 | T6 `:67` 去「dev 分流门」限；「Part-2 工程门 / bootstrap 临时信任门」门性质句不动 |
| debt 账本 | `docs/debt/gov/ARQ-261016-intake-entry-identity-unstated.md` | G4 | T7 frontmatter title/paths 更正 + append 承载注记（append-only，不改既有正文段） |
| 生成件 | `docs/debt/gov/INDEX.md` | G4（生成件） | T7 重跑 debt-render.mjs 再生（title 列投影；生成件勿手改） |
| 契约面 | `docs/contracts/**` | 未触 | 零触 |
| 强制层 | hooks / settings / scope-map / verify-gates / scripts | 未触 | 零新门 / 零机检齿（OQ-3=(b) convention-only 留根，本批不含） |
| 实现面功能描述 | `dev-loop.md` / `task-intake-and-slicing.md` | 未触 | 「改 firmware/** 行为 → 过 /triage-change 分流门」是单面实现路径的正确功能性描述、非入口身份断言 |

**【硬项 · sweep 透镜义务】**本批**不落地新门 / 新断言**（全部 T 都是措辞显式化，无判定 pattern、无脚本），无「以其判定 pattern 全仓预跑」义务。

**【硬项 · 复扫模式完备性先验】**本批退役语面 = 身份性「dev 分流门 / 前置门」旧框架 + §8 的「triage 实现面」并列括注。模式集显式枚举四词形：`dev 侧变更分流前置门`、`dev 变更分流前置门`（architect.md 无「侧」变体）、`dev 分流门`（architect-operating-model.md + change-triage.md §6 变体）、`triage 实现面`（change-intake.md §8 括注）。census 全量（2026-10-16 复跑，命令 + 展开取证如下）：

- `grep -rn "dev 侧变更分流前置门" .claude AGENTS.md docs` = **4 处**：`change-triage.md:1`（T2）、`triage-change.md:2`（T4）、`triage-change.md:9`（T4）、`docs/debt/gov/ARQ-261016-…:96`（append 历史行、豁免）。
- `grep -rn "dev 变更分流前置门" .claude AGENTS.md docs` = **1 处**：`architect.md:26`（T5）。
- `grep -rn "dev 分流门" .claude AGENTS.md docs` = **2 处**：`architect-operating-model.md:67`（T6）、`change-triage.md:113`（§6 逃生门、豁免）。
- `grep -rn "triage 实现面" .claude AGENTS.md docs` = **3 处**：`change-intake.md:77`（T3）、`ARQ-261016-…:33`（append 证据 4、描述性引用、豁免）、`ARQ-261016-…:48`（append 建议 3、描述性引用、豁免）。

**销项** = ① 目标内改写 **6 处**（`change-triage.md:1`、`triage-change.md:2/:9`、`change-intake.md:77`、`architect.md:26`、`architect-operating-model.md:67`）；② 显式豁免 **4 处**——`ARQ-261016-…:96` + `:33` + `:48`（三条 append 历史行、描述问题/证据/建议本身）+ `change-triage.md:113`（§6 逃生门承重段、零门权纪律不动）。

**完备性论证**：退役语面四词形已全量列、无第五类形态；「dev 侧」其余命中（`change-triage.md:58`、`pod-coordination.md:170`、debt「dev 侧在途」、showme HTML）为合法非身份性用法，「分流门」其余命中（`dev-loop.md:26/:32/:129`、`task-intake-and-slicing.md:247`）为单面实现路径的功能性描述——均不属退役语面、不动（已列 §0 非目标 7/8）。

## 4. human 取舍点（OQ）

| OQ | 题 | 裁定（human 2026-10-16 亲裁） |
|---|---|---|
| OQ-1 | AGENTS.md 入口节表述幅度 | **已裁 (b) 节头显式化**——T1 现状形态，不改 |
| OQ-2 | commands/triage-change.md 纳入本批 | **已裁 (a) 纳入**——T4 现状形态，不改 |
| OQ-3 | check 齿（ARQ 建议 4） | **已裁 (b) convention-only 留根**——不立机检齿；T7 append 已回填「OQ-3=(b) convention-only 留根」；§6 留根保留 |
| OQ-4 | architect.md:26 同源框架 | **已裁 (a) 本批顺带改**——human 原话逐字「本批顺带改」；提问背景逐字「需你此刻显式批准破自改递归边界」（architect.md = 自改递归 + 第五阀 + prompt-authoring 硬排除）；新增 T5；牵连面复核新增 T6（architect-operating-model.md:67 镜像转述） |

> 注：OQ-4 原推荐位为 (b)，human 改裁 (a)——本批据此新增 T5/T6 并重算 review profile（触 selfDef 硬排除，已标注）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**起草期自检实跑记录（2026-10-17，v0.3）**：V1 已实跑（增列 architect.md + architect-operating-model.md 后复跑），输出 `ok: true`、`anchorCount: 8`、`findings: []`、`advisories: []`；anchorUnion 八路径与 manifest 逐 token 一致（无注记夹带路径 token、无目录 token advisory）——预检绿，非门判定。F2 配套三命令实测：`route-regression-check.mjs` = `52 ok, 0 fail`；`route-regression-check.mjs --self-check` = `104 ok, 0 fail [--self-check]`（✓ all required signals/whitelists falsifiable）；`lint-descriptions.mjs` = `scanned 16, exempt 0, excluded 1 (architect.md), flagged 0`（clean）。

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261016-gov-review-triage-entry-identity/PROPOSAL.md` | 形态零错；anchorUnion = 8 路径与 manifest 逐一吻合、无注记夹带路径 token、无目录 token advisory |
| V2 | pre-commit | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/route-regression-check.mjs --self-check` + `node .claude/scripts/lint-descriptions.mjs` | 绿——实测 `52 ok, 0 fail` / `104 ok, 0 fail [--self-check]`（必留信号可证伪）/ `scanned 16, flagged 0`（lint 干净）。T4 description 改动是 command description、route-regression 只断言 agent description 与「你现在是…」路由（triage-change 不在 fixtures 内），故不破坏；去 workflow 摘要句后 lint 面只减不增 |
| V3 | post-commit | `node .claude/scripts/debt-render.mjs --check` | 绿（T7 title/paths 更正并重跑 debt-render 后，`docs/debt/gov/INDEX.md` 盘上 == 应渲染） |
| V4 | post-commit | `grep -n "编排入口" AGENTS.md .claude/references/change-triage.md .claude/references/change-intake.md .claude/commands/triage-change.md .claude/agents/architect.md .claude/references/architect-operating-model.md` | 六个目标文件各 ≥1 命中（正面陈述落地） |
| V5 | post-commit | `grep -rnE "dev 侧变更分流前置门|dev 变更分流前置门" .claude/ AGENTS.md` | 零命中（T2/T4/T5 改写后三个「前置门」承载点全清零；docs 里 ARQ:96 为历史描述性引用、集外豁免） |
| V6 | post-commit | `grep -rn "dev 分流门" .claude/ AGENTS.md` | 仅 `.claude/references/change-triage.md:113` 命中（§6 逃生门、零门权纪律显式豁免；architect-operating-model.md:67 → T6 改写后不再命中） |

## 6. 留根清单

- **check 齿（ARQ 建议 4）**：表述缺位类缺陷的机检自检（如「编排入口」字样断言）→ OQ-3=(b) convention-only 留根，待将来有机制需求再立（届时走 `/gov-change`，触 G3 恒 Track B）。
- **「dev 侧」合法用法复查**：`change-triage.md:58`「dev 侧为何单轮而非对抗」为合法解释性用法，本批不动；若将来「实现面分档」被再改语义，届时一并对该节复查。
- **ARQ-261016 结项**：本批 landing + landing-verifier 绿 + retro 齐后，评估建议 1/2/3/5 全落地 + 建议 4 留根清晰 + title/paths 已更正，由主线程裁是否结项。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-17 - architect

verdict: GREEN

**亲核基线（全实测，2026-10-17）**

- `git rev-parse` 三支 SHA 均在册（`2e43fc59`／`68ab0579`／`39646e88`）；`git show <sha> -- AGENTS.md` 逐一亲核。
- **ARQ 更正 ①（`2e43fc59` 是否同步了 AGENTS.md）＝ PROPOSAL 判断成立**：`git show 2e43fc59 -- AGENTS.md` 实测 `+` 两行（「需求面变更」+「跨面 intake」含「CLAUDE.md 经 `@AGENTS.md` 导入传导」尾句）——AGENTS.md 确被该批同步。ARQ 证据 5「改了身份却没同步 AGENTS.md」失实，更正无误。（旁证：`docs/debt/gov/_archive/2026/ARQ-260928-triage-cross-face-intake-gap.md:8` 的 `paths` 确不含 AGENTS.md——paths 列举遗漏属实，但不能推「未同步」，更正 ① 立。）
- **ARQ 更正 ②（漂移点＝`68ab0579`）＝ PROPOSAL 判断成立**：`git show 68ab0579 -- AGENTS.md` 实测新立节头「**四面入口（每面一个 loop，互为对偶）**」+ 新增「架构+契约面变更」「治理面变更」「实现面变更」三行；「需求面变更」「跨面 intake」两行为 260928 已落 context 行、被该批**收编**。成因＝后批框架收编，非漏同步——表述成立。
- **直接前序批实证成立**：`git show 39646e88 -- AGENTS.md` 实测仅改 `:30` 一行（`-四档分流` `+五档分流`）——同一行刚被上一批踩过，证据强于 ARQ 引的 260928 证据。
- **正面陈述零命中**：`grep -rn "编排入口" .claude AGENTS.md CLAUDE.md` = 无输出（零命中）——「定义从未进入常驻指令面」前提成立。
- **行号锚逐条 verbatim 命中**：`AGENTS.md:26/27/28/29/30/31`、`change-triage.md:1`／`:10`／`:113`、`change-intake.md:4`／`:77`、`triage-change.md:2`／`:9`、`architect.md:26`、`architect-operating-model.md:67` 全部与 PROPOSAL 引文一字不差。
- **census 复核**：`grep -rn "<形>" .claude AGENTS.md docs` 实测 `dev 侧变更分流前置门`=4、`dev 变更分流前置门`=1、`dev 分流门`=2、`triage 实现面`=3——与 §3 逐条吻合，销项 6 目标改写 + 4 豁免 = 10 处算术自洽。
- **机械预检**：`landing-fidelity-check.mjs --preflight` 实测 `ok:true`、`anchorCount:8`、`anchorUnion` 八路径与 landing-manifest 逐 token 一致、`findings:[]`/`advisories:[]`。`route-regression-check.mjs` 实测 `52 ok, 0 fail`；`.claude/route-fixtures/routing.json` 无 triage 条目（`commandEntry:[]`、`agentRequiredSignals` 无 triage-change、无 architect 键）⇒ T4/T5 不触路由断言。`debt-render.mjs --check` 实测 `exit=0`（基线绿）。`docs/debt/gov/INDEX.md:37` title 列 = ARQ frontmatter title 一字不差。
- **判档复核（判决权在本判官）＝ 维持**：本批触 `AGENTS.md`（且触 `.claude/commands/**`、`.claude/agents/**`）⇒ `gov-change.md:18` 轻路径判据 ③ 命中 ⇒ **恒 Track B**，architect 自评成立。review profile = G1（AGENTS.md）+ G2（references×3 + command×1 + agent×1）+ G4（debt×1 + 生成件 INDEX×1）；G3/契约面零触；**selfDef 硬排除命中**（architect.md 自身定义正文），PROPOSAL 已显式披露且由 human OQ-4=(a) 批准破界——§4 留痕在场、与裁断一致（摘要形态，见 F4）。
- **牵连面独立复扫**：本判官另跑裸 `分流前置门`／`分流门` 全仓扫——未发现新增「身份性」第五词形。`docs/showme/show-me-gov-triage.html:90`（裸「分流前置门」，语义为新定义「分面路由器」，非旧框架身份承重）与 `dev-loop.md:26/:32/:129`、`task-intake-and-slicing.md:247`（功能性单面指路，§0 非目标 8 已豁免）均属合法用法；`arch-change.md:22`／`gov-change.md:21`「实现面变更转 `/triage-change`」为单向功能指针、非身份断言。repo-wide（含 `*.mjs`／`*.json`）无脚本/夹具依赖退役串——V5/V6 清零不会被机械面反噬。
- **结构字段在场**：landing-manifest 围栏块（八行目标路径）与 packet-id 行（§头部「packet-binding」块）均在 §1 前在场。
- **意图保真**：T1–T7 相对 human 原话（「triage 现在就是要把需求、架构跟开发都合在一起」）与 260928 FROZEN PROPOSAL §目标效果（`delivery/archive/changes/arch/260928-gov-review-cross-face-intake-spine/PROPOSAL.md:29` 逐字亲核）忠实——无过度（未新增门权/流程语义）亦无不足（五处身份承载点全收）。

**findings（无 BLOCKING；GREEN 结论可与下列并存）**

- F1（SHOULD · 事实准确性·内部矛盾）§2（`:61`）census 谓 `grep -rn "triage 实现面"` = **1 处**（仅 `change-intake.md:77`），与 §3（`:281`）同形 `= 3 处`（`change-intake.md:77` + `ARQ-…:33` + `ARQ-…:48`）**自相矛盾**，且实测为 **3**——§3 正确、§2 失实。§2 另三形计数与 §3 一致（含 docs 内 `ARQ-…:96`），故非 scope 差异、系 §2 漏数。不影响改法集与验证集（§3 为权威复扫段）。建议修法：§2 该形改为 **3 处**并补列两处 ARQ 描述性豁免，消内生矛盾（免 landing-verifier 撞见）。
- F2（NIT · 纪律合规）T4 新 description（含「跨面变更（触面 ≥2）从本命令进统一 intake：先四面探测落 spine、再按 facePlan 派发各面」）新添 workflow 摘要句，与 `authoring-agents-skills.md`「description 只写 when-to-use + 职责边界 + 路由信号、**不概括 workflow 正文**」相抵触（该规则对命令文件适用；lint 为告警级）。原 description 已含 workflow 链（既存状态），T4 使其更甚。建议：以「编排入口」身份信号替代 workflow 叙述（保 `/arch-change`／`/build-feature`／五档枚举／落地家路由信号即可），或至少复核 T4 后 `lint-descriptions.mjs` 告警面。
- F3（NIT · 引用精度）T5 称守「**HQ-1 硬边界守则**（prompt-authoring.md）」，但 `prompt-authoring.md` 首行硬排除明写本规则**不适用于 `architect.md`**；对 architect.md 的承重约束实为 `docs/DEV-GOVERNANCE.md:212` §6「自改递归」条（保留语义的原则仍成立、T5 改法本身不违）。建议：承重依据改引 DEV-GOVERNANCE §6.3，HQ-1 至多作同构参照。
- F4（NIT · 留痕强度）OQ-4 的破自改递归批准（human「本批顺带改」当且仅当「显式批准破界」）在 §4 以**摘要**形态在场、与裁断一致（无「缺失/不符」故不升 human）；但破自改递归属治理最高敏感动作，建议补记 human 原话逐字（对齐 spine `sources`／讨论记录耐久源纪律），使 §4 留痕可被日后复核。
- F5（NIT · 引文精度）T1 证据段（`:78`）标「260928 FROZEN PROPOSAL §目标效果**逐字**」，实文省略了括注 `（delivery/changes/intake/<changeId>.md）`（该路径随后批迁为 `delivery/intake/`）；省略合理，但「逐字」标签宜加省略号或改「节引」。

**结项**：无影响目标效果级问题、无方向级缺口 ⇒ 不升级 human；判档维持 Track B（非轻路径）、review profile 维持 G1+G2+G4。F1 建议 FROZEN 前随手修正（§2 数字），F2–F5 为可选打磨。主线程可凭本条 GREEN 标 FROZEN。

### R2 - 2026-10-17 - architect

verdict: GREEN

**轮次范围**：聚焦 R1 findings（F1–F5）修订到位性 + 修订是否引入新矛盾；不做全量重核。全部断言亲核实测（2026-10-17）。

**前轮 findings 到位性（逐条亲核）**

- **F1（SHOULD · §2 census 数与 §3 矛盾）＝ 到位**：§2 `:61` 现写 `grep -rn "triage 实现面" .claude AGENTS.md docs` = **3 处**（`change-intake.md:77`、`ARQ-…:33`、`ARQ-…:48`），与 §3 `:281` 一致。本判官复跑四形（`grep -rn "<形>" .claude AGENTS.md docs`）实测 **4 / 1 / 2 / 3**——§2 与 §3 两两一致、与地面真值一致，原内生矛盾消除。
- **F2（NIT · T4 description workflow 摘要句）＝ 到位**：v0.3 新 description（`:177`）= `变更分流 + 跨面编排入口（对偶 /arch-change、/build-feature）。改 firmware/** 行为的单面实现变更开工前先过 architect 一次比例化 review：判五档（…）→ 人类确认/改判路由 → reviewed plan 按成熟度就近落地（…）→ 报告下一步命令。`——v0.2 新增的 workflow 摘要句「跨面变更（触面 ≥2）从本命令进统一 intake：先四面探测落 spine、再按 facePlan 派发各面；」**已删**，跨面身份改以「跨面编排入口」**信号短语**承载；改法前言（`:172`）补引 `authoring-agents-skills.md` 纪律。**路由承重信号零丢失亲核**：对偶 `/arch-change` ✓、`/build-feature` ✓、五档枚举（③拆包 / ②单包Full / ②单包Lite / ⓪档 0 nano / ①直接修）✓、落地家（`delivery/workstreams/<ws>/PROPOSAL.md` + `delivery/taskpackages/<id>/plan.md`）✓ 全在。三命令复跑与 R1 基线对照：`route-regression-check.mjs` = `52 ok, 0 fail`；`--self-check` = `104 ok, 0 fail [--self-check]`（✓ all required signals/whitelists falsifiable）；`lint-descriptions.mjs` = `scanned 16, exempt 0, excluded 1 (architect.md), flagged 0`（clean）——三者均绿，无信号丢失现形（注：`triage-change` 不在 `routing.json` fixtures 内、route-regression 本不断言其 description，故 V2 主要用于 confirm 无回归 + lint 证 workflow 摘要删除未触发告警）。
- **F3（NIT · HQ-1 误引）＝ 到位**：T5 `:196` 改引 `docs/DEV-GOVERNANCE.md:212` §6 硬条款 3——本判官亲核该行 verbatim：「**自改递归**：修改自身定义、权限边界、工具、write scope，或影响自身权限的强制执行层文件 → 必须升级为 human-approved governance change；不得自批，不得仅凭同一 agent 的判断落地」——引文与 `:212` 首句一字不差（`## 6. 治理的治理（meta）` 起于 `:197`，本条为 §6 下第 3 项，「§6.3」定位成立）。§3 `:266` 同步改引。R1 指出的「HQ-1 对 architect.md 不适用」问题消除。
- **F4（NIT · 留痕强度）＝ 到位**：§4 `:294` OQ-4 现载 human 原话逐字「本批顺带改」+ 提问背景逐字「需你此刻显式批准破自改递归边界」，与派单所述 human 原话一致；破自改递归批准留痕由摘要升为逐字，可复核性达标。
- **F5（NIT · T1 引文「逐字」名实不符）＝ 到位**：T1 `:78` 现标「（…PROPOSAL.md **节引**〔省略 spine 落点括注，以 … 标示〕）」，引文内以「…」标省略位、不再自称逐字——名实相符。

**新矛盾扫描**

- 相对 v0.2 仅动：Status 行（→ v0.3，述「R1 GREEN 无 BLOCKING、F1–F5 已逐条处置」，与 R1 记录一致）；§2 `:61`（数字 1→3）；T1 `:78`（逐字→节引 + …）；T4 `:172`/`:177`（引规则 + 删 workflow 摘要句）；T5 `:196`（改引 DEV-GOVERNANCE）；§3 `:266`（同步改引）；§4 `:294`（补 human 逐字）；§5 `:300`/`:305`（自检记录 + V2 行补实测值）。**T1–T7 改法本体（替换后完整句）/ landing-manifest / 门语义均未被动**：manifest（`:13–:20`）仍八路径；V1 复跑实测 `ok:true`、`anchorCount:8`、`anchorUnion` 八路径与 manifest 逐 token 一致、`findings:[]`/`advisories:[]`。未发现 F1–F5 修订引入新内部矛盾或新事实错误。
- **可选打磨（R2-N1，NIT，不阻塞）**：§2 `:61` 四形中仅 `triage 实现面` 一行显式标了 scope（`.claude AGENTS.md docs`），另三形未标；此正是 F1 的成因土壤（scope 未明 → 计数口径漂移）。建议四形统一标 scope，杜绝复发。非必需。

**GREEN 保持性**：F1–F5 到位、无新矛盾、manifest/结构字段/门语义零漂移 ⇒ 维持 GREEN。无影响目标效果级问题、无方向级缺口 ⇒ **不升级 human**。主线程可凭本条 GREEN 标 FROZEN。
