# review · 260928-gov-review-cross-face-intake-spine

> 判官：architect（Track B 常驻轻路径评审循环，R1）。
> 审对象：本目录 `PROPOSAL.md`（DRAFT v1.0，human 立项 ack 已获得，OQ-1~4 已裁钉 §4）。
> 评审方法：按 `docs/templates/PROPOSAL.template.md` 四硬项逐核 + 方案实质审查（T1–T7 接缝、headless 三铁律、T6 先例形态）；全部引据 file:line 亲读核对，全部可现跑命令实跑（留痕见文末「实测留痕」节）。

## R1 - 2026-09-28 - architect

verdict: **BLOCK**

### 通过面（已核绿，无须再证）

- **V1 preflight 实跑绿**：`ok: true`、`anchorCount: 10`、`advisories: []`、`findings: []`；anchorUnion 十路径与 landing-manifest 十路径逐一吻合（逐 token 人工比对，集合相等）；全部七条 `### T<n>` 标题形态合规，`目标文件：` 行均不加粗、注记均不夹带路径 token。
- **起草期实证义务大头为真**：agents 目录实跑枚举 16 个定义（14 既有 + story-author + requirement-reviewer，`ls` 亲核）；五脚本均在册；routing.json `_doc` 尾句 verbatim「档2 迁入 dev-loop 时补 build-feature/planner 行」核实；`agentRequiredSignals` 六条目核实；build-feature.md:16「四件」预检原文、task-intake-and-slicing.md:272-285 canonical 定义节（:285 存量迁移条目）、AGENTS.md:24/:26、requirement-change.md:3 留根句、triage-change.md:13/:21/:32-35、change-triage.md:10——全部 verbatim 对原文核实无误。
- **T6 先例形态与信号逐条为真**：scope-map `architect-reviewer` 现条目 = `allow: [], deny: ["**"]`（scope-map.json:121-124；verifier/supervisor 同形），T6 两条目拟采同形，成立；enforce-scope.mjs 亲读确认「per-agent allow/deny 只拦 Write/Edit/MultiEdit，Bash 分支只守 universal.bashDenyWrite」——「管写不管读，return 型交付不受影响」断言准确（两 agent 跑 `spec-anchor-check.mjs` 不受影响）；routing.json 拟补 8 条信号逐条是两 agent description（story-author.md:3 / requirement-reviewer.md:3）的真实子串，亲核全中；「两 agent 无 `tools:Agent()` 白名单」核实。
- **V2 现跑绿**（28 ok, 0 fail）；「本批补条使前批 V3 真空绿缺口闭合，非真空绿」表述准确——补条后绿即证明两新 agent 信号在位。
- **V3 现跑绿**（输出 `scope-map ok`）。
- **T4 fail 语义同口径**：⑤「未回填 → 不进 intake，报 `awaiting-upstream`，先完成上游面」与既有「任一缺 → 不进 planner intake」同判（都拦在 Step 1 intake 之外）；「与 ③ 路径同判、粒度同为包」与 task-intake-and-slicing.md:284 现文一致；插入位（④ 与句号间）机械自洽。
- **T5 插入位与引文核实**：AGENTS.md:24 段尾句、:26 段首句 verbatim 无误，两段之间插入可行；两段插入文本已逐字给出。
- **T7 现状核实**：ARQ 条目末注记现恰一条（:35），append-only 纪律对齐；V6 现跑计数 = 0，append 后 = 1 可判。
- **T3 证据核实**：repo-structure.md:32-33 两行 verbatim 无误；`delivery/changes/retro/` 目录实存两制品（2026-09-13 / 2026-09-23），漂移披露为真。
- **「settings 零改动」主张与 T1/T6 改法一致**：headless 复用在册 settings（OQ-2 裁 (b)），manifest 无 settings 文件，§3「强制层其余 hooks / settings 未触」行成立。
- **riskTier medium + G2+G3 证成成立**（触 G3 scope-map + 指令根 + 新 headless 机制，但契约/hooks/门体系零触、只加 intake 前置）；OQ-1 human 改判原话完整留痕于 §4 与 Status 头（但见 F7）。

### Findings

**F1 · BLOCKING · 「第一刀」census 断言失实 ×2，且直接导致 V4 不可判**

§2 实证段断言「「第一刀」全仓 census = 恰 3 处（triage-change.md:13、triage-change.md:21、change-triage.md:10，grep 实跑 2026-09-28，三处全在 T2 目标集内、无集外残留）」；§3 复扫完备性先验收句「完备性论证 = 该词为本批引入前仅存于 triage 两文件的门内术语，AGENTS.md / references 其余 / docs / workstreams 全域 grep 零命中」。

实跑（2026-09-28，命令与全输出见文末实测留痕）：

```
grep -rn "第一刀" .claude/ AGENTS.md docs/ delivery/workstreams/
```

命中 **9 处**，非 3 处：

- 流程面 3 处（与断言一致）：`.claude/commands/triage-change.md:13`、`triage-change.md:21`、`.claude/references/change-triage.md:10`；
- 集外残留 6 处（断言称零）：`docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md:13`（frontmatter source 行）、`:19`、`:20`（证据段）；`docs/showme/README.md:13`；`docs/showme/show-me-gov-triage.html:108`、`:122`。

「docs 全域 grep 零命中」与「无集外残留」均为失实断言（模板硬项：凡「全 / 均 / 无」量化声明须实测留痕）。

后果链：V4（pre-commit，`grep -rn "第一刀" .claude/ AGENTS.md docs/ delivery/workstreams/` 预期「零命中」）**landing 后必红**——ARQ 条目既有行 append-only 不可改（AGENTS.md Hard Rules「历史记录类文件一律 append」+ T7 自述「只追加、不改既有行」），showme 两文件为本批非目标⑥「保持讨论稿定位」不动。6 处描述性引用将永久命中 V4 的 grep，验证集按现文不可判。

修法（三处联动）：

1. V4 命令 scope 收窄为 canonical 流程面：`grep -rn "第一刀" .claude/ AGENTS.md delivery/workstreams/` → 预期零命中（该 scope 现跑仅 .claude 3 处，全在 T2 目标集内，landing 后清零可判）；
2. §2 census 改全量如实：9 处 = 流程面 3 处（T2 全数改写）+ 历史账本描述性引用 3 处（ARQ append-only 不动）+ 讨论稿描述性引用 3 处（非目标⑥不动）；
3. §3 复扫完备性论证改写：门内术语用法仅存 triage 两文件（3 处，T2 全改）；docs/crq 与 docs/showme 的 6 处为对既有流程的描述性引用，分属 append-only 账本与非目标讨论稿，显式豁免并一句证成（「描述性引用非门内规则语使用，不构成语面残留」）。

**F2 · BLOCKING · §3 sweep 表漏 classify 目标文件 `.claude/rules/repo-structure.md`，「UNCLASSIFIED=0」断言失实；review profile 枚举同步漏项**

landing-manifest 第 4 行 / T3 目标文件 `.claude/rules/repo-structure.md` 在 §3 影响面 sweep 表中**无行**（表 12 行逐行核对：change-intake / change-triage / task-intake / triage-change / build-feature / AGENTS.md / scope-map / routing.json / ARQ / 契约面 / hooks+settings / showme——缺 repo-structure.md）。被触且进 manifest 的文件未分类 ⇒ 节标题「UNCLASSIFIED=0」不成立。

连带：Status 头 review profile 行「G2 = references ×3 + commands ×2 + 指令根 ×1 + CRQ append ×1」计 7 件，实际 G2 触面 8 件（漏 rules ×1）。

修法：§3 表在 command 行与指令根行之间补一行——面 = rules、文件 = `.claude/rules/repo-structure.md`、类别 = G2、说明 = T3 两行结构登记（intake 新行 + retro 漂移修复）+ 消费方（全仓结构认知）；review profile 行补「rules ×1」。

**F3 · MINOR · T2 触点③ 替换块（triage-change.md:32-35）只钉死首句，:33-:35 三行去留两读**

T2 改法给出「triage-change.md:32-35 替换后完整句」但只含步骤 6 首句；:33-34 两 bullet（②单包 → planner `/build-feature`；③大 feature → portfolio-planner）与 :35「planner/portfolio 必须是**自己的主线程会话**——子 agent 不能再派子 agent，**本门拉不起那个会话**；dev loop 本就该人类在场。」三行的去留未逐行钉死。若 landing 时保留 :35，则与 T1 第 5 块 headless 拉起机制、T2 新句「重面给出 headless 拉起命令」直接语义矛盾（「本门拉不起那个会话」vs 本门打出 headless 拉起命令）。按模板硬项「承重 landing 指令必须原文引用钉死并给出替换后完整句，禁止转述」，此处须消两读。修法：T2 显式写明「:33-35 三行随块整体替换删除」（新句「按 facePlan 顺序打出各面执行命令」已覆盖两 bullet 的路由职能；:35 的「子 agent 不能再派子 agent」约束由 T1 禁递归铁律承接），或逐行给出保留/改写后全文。

**F4 · MINOR · 四面探测面不含 governance 面，原第一刀的该分支路由在新文本无显式落点**

原第一刀分区对象（triage-change.md:13）= 「`docs/contracts/**` / kernel·systems spec / `.claude/agents` 定义 / gate」四类；T2 替换句与 T1 第 3 块探测语料（requirements + architecture + contracts + CRQ 三域）均不含 `.claude/agents` 定义 / gate / G1-G3 治理面，升级判据也只列「触 `docs/contracts/**` 或跨系统结构性」。纯 governance 变更或含 governance 面的混合变更在新文本无显式路由分支——只能靠保留的 change-triage §6 逃生门（原文「疑似触契约/spec/agent 定义/gate」未动）事后兜底，兜底存在但由事前探测降为 review 中发现。修法：T1 四面探测程序或 T2 替换句补一句「触 `.claude/agents` 定义 / gate / 治理面 → 路由 `/arch-change`（governance 变更通道），不判四档」，把原第一刀的第四类分区对象显式安放。

**F5 · MINOR · 双入口并存期无去重纪律，spine changeId 本身不提供去重**

非目标①保留 /arch-change 本体、退役安置留根独立批，则并存期内同一跨面 idea 可经两入口分别 intake（human 手开 /arch-change 处理架构面，与 triage 建的 spine 互不知情）。T1 第 8 块「带 changeId 调用的各面流程入口先读 spine」只约束带 changeId 的调用，/arch-change 开场无「先查既有 spine」条款；spine 亦无「建档前查重」纪律。留根清单第一项已含「与统一 intake 的入口关系定稿」，终态有归口，缺的是并存期过渡纪律。修法：T1 第 3 块补一句「建档前查 `delivery/changes/intake/` 既有 spine，同 idea 已建档则续用不另建」即可，成本一行。

**F6 · MINOR · spine anchors 回填写者未定 + 并发写无防护；openQ 答复循环终止条件未写死**

frontmatter `anchors` 为 per-face 回填槽，但回填写者（主线程还是下游 headless 子会话）未指定。若归下游子会话：headless 会话带 `--agent` 即受 per-agent scope-map 约束（如 review-moderator allow 不含 `delivery/changes/intake/**`，实读 scope-map.json:101-103 核实），物理写不进 spine；若多面并行，同一 spine 文件并发 Edit 亦无「单写者」规则。另 L1「主线程答疑」只写「能答的写回 openQ 续跑」，主线程也答不了时的去向（应落 L2 呈人）未成文。修法：T1 第 2/5 块钉死三条——「spine 唯一写者 = 主线程；下游会话只 return anchor 信息，回填归主线程」「facePlan 执行顺序外不并行拉起子会话」「L1 主线程不能答的疑问即升 L2，不得悬置」。

**F7 · MINOR · OQ-1「Status 行已同步」与 ARQ 实况不符**

§4 OQ-1 裁定栏写「ARQ 自评 Track A 被 human 裁定覆盖，Status 行已同步」。实读 `docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` 全文：frontmatter（status: deferred，:5）与正文均无任何 Track B 改判留痕；风险节（:33）仍原文写「**review profile = full-adversarial + human（Track A，/arch-change 入口）**，不美化快路径」。「已同步」为失实表述（改判留痕目前只在 PROPOSAL）。修法二选一：把改判记录并入 T7 append 注记文本（如「review 轨经 human 2026-09-28 裁定 Track B（原话见承载批 §4 OQ-1），本条目风险节 Track A 自评作废」），或删「已同步」改为「改判留痕于本 PROPOSAL §4」。推荐前者——ARQ 是这条路由事实的账本，改判应进账。

**F8 · MINOR · T3 两新表格行未按模板钉死逐字行文**

模板硬项要求承重 landing 文本「原文引用钉死并给出替换后完整句」。T5/T7 的插入文本已逐字给出，T2/T4 的替换句亦然；唯 T3 的 intake 行与 retro 行只给了四列语义的 prose 转述（owner / 放什么 / 永不允许），表格行逐字文本由落地者自行生成，landing-verifier 逐文件人读 diff 时无 frozen 原文可比。修法：landing 前把两行完整表格行文（含第一列路径 token 形态，建议 `` `delivery/changes/intake/**` `` / `` `delivery/changes/retro/**` `` 对齐 :32-33 既有形态）逐字写进 T3 改法或 landing-summary，供核验官对 diff。

### Verdict

**BLOCK**。BLOCKING 项：F1（census 断言失实 ×2 + V4 按现文不可判）、F2（§3 sweep 漏 classify manifest 目标文件，UNCLASSIFIED=0 失实 + profile 枚举漏项）。两项均为文面修订级（V4 收 scope + census 如实改写；§3 补一行 + profile 补一枚举），不动 T1–T7 改法本体与 OQ 裁定，修订后可直进 R2。

MINOR 六条（F3–F8）建议随 R2 修订一并顺手修：F3（T2 替换块 :33-35 去留钉死）与 F7（OQ-1「已同步」失实表述）虽标 MINOR，但都属「落地时会产生两读/失实留痕」类，建议必改；F4/F5/F6 各补一句即可；F8 可延至 landing-summary。方案本体（spine 制品形态、四面探测、headless 三铁律、launchPreconditions 推广②、T6 先例同形登记）实质审查通过，未见 gate 绕过或语义矛盾。

### 实测留痕（2026-09-28，均于 C:/AI/KidPal 仓根实跑）

1. `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-cross-face-intake-spine/PROPOSAL.md`
   → `{"ok":true,"gate":"landing-preflight","section2BodyFound":true,"anchorCount":10,...,"advisories":[],"findings":[]}`，EXIT=0。anchorUnion 十路径与 manifest 十路径逐 token 比对相等。
2. `node .claude/scripts/route-regression-check.mjs` → `route-regression-check: 28 ok, 0 fail`，EXIT=0。
3. `node -e "JSON.parse(require('fs').readFileSync('.claude/scope-map.json','utf8'));console.log('scope-map ok')"` → `scope-map ok`。
4. `grep -rn "第一刀" .claude/ AGENTS.md docs/ delivery/workstreams/` → 9 处命中（.claude 3 + docs/crq 3 + docs/showme 3），全输出已引于 F1；AGENTS.md 与 delivery/workstreams/ 零命中。
5. `grep -c "260928-gov-review-cross-face-intake-spine" docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` → `0`（V6 现状基线，append 后预期 = 1 可判）。
6. `grep -n "delivery/changes/arch\|delivery/changes/dev" .claude/rules/repo-structure.md` → :32/:33 两行，与 T3 证据 verbatim 一致；`ls delivery/changes/retro/` → 两制品在册。
7. `ls .claude/agents/` → 16 个定义文件（14 既有 + story-author + requirement-reviewer）；`ls .claude/scripts/` → 五脚本（landing-fidelity-check / lint-descriptions / route-regression-check / changes-index / taskpackage-required-check）均在册。

## R2 - 2026-09-28 - architect

verdict: **GREEN**

审对象：本目录 `PROPOSAL.md` DRAFT v1.1（R1 BLOCK(F1/F2 + MINOR F3–F8) 修订版）。逐 finding 复核 + 修订面新矛盾扫描，方法同 R1（引据亲读、命令实跑，留痕见本段末）。

### F1–F8 处置核验（逐条）

- **F1 真解**：§2 实证段 census 改全量 9 处如实（流程面 3 + 集外 6 描述性引用，逐处 file:line 列明）——与 R1 实测输出逐处比对一致（ARQ :13/:19/:20、showme README:13、html:108/:122，无一漂移）；§3 复扫硬项同口径重写，六处显式豁免附证成（append-only 历史行 + 讨论稿非目标⑥），完备性论证改「census 全量已列、无第三类形态」——成立（9 处确只有门内术语定义与转述两种用法）；V4 命令收窄为 `grep -rn "第一刀" .claude/ AGENTS.md delivery/workstreams/`、预期改「流程面 census 清零」。**V4 可判性复证**：收窄 scope 现跑恰 3 处（triage-change.md:13/:21、change-triage.md:10），全在 T2 目标集内，T2 landing 后清零——预期与改法对得上。
- **F2 真解**：§3 表补 rules 行（`.claude/rules/repo-structure.md`，G2，说明含 T3 两行登记与消费方）；header review profile 改「references ×3 + commands ×2 + rules ×1 + 指令根 ×1 + CRQ append ×1，共 8 件」——逐件对 manifest 重数：G2 8 件 + G3 2 件 = 10 = manifest 全集，UNCLASSIFIED=0 现成立。
- **F3 真解**：T2 改法 3 改「triage-change.md:32-35 **整块替换**」，替换句补「单包/拆包的下一步命令映射（②单包 → planner `/build-feature <id>`；③大 feature → portfolio-planner `<ws>`）保留为 runbook 条目」（原 :33-34 语义保全）；显式写明原 :35「本门拉不起那个会话」随块删除 + 语义去向（人类在场由三铁律「禁跨门」承接、「子 agent 不能再派子 agent」约束不变因 headless 是完整顶层会话）——两读消除，与 T1 第 5 块无矛盾。
- **F4 真解**：T1 第 3 块补「语料另含 governance 面（`.claude/agents/**` 定义 / gate 登记）——命中记入 faceMap 并路由 /arch-change 治理 profile，不建实现面档」——原第一刀第四类分区对象（`.claude/agents` 定义 / gate）重新有显式路由落点。残留一处措辞不同步，见 R2-M1。
- **F5 真解**：T1 第 3 块补建档前查重（查 `delivery/changes/intake/` 既有 spine、命中消费不重建）+ 并存期 /arch-change 侧同纪律单侧承载（其文本改动归退役安置批，诚实披露而非纸面宣称）；§6 退役安置留根项补「其开场查重条款落文本」。
- **F6 真解**：T1 第 5 块补「anchors 回填写者恒 = 主线程（headless 子会话以退出报告交回）+ 单写者规则（同一 spine 同一时刻仅一个主线程会话持写权、跨会话接力经 openQ / 退出报告交接、禁并行双写）」；「review-moderator 现形态即拦」断言复证为真（scope-map.json:101-103，allow 不含 `delivery/changes/intake/**`）；第 6 块补 L1 答不了去向（openQ 标 `escalated` + status 写 `awaiting-human:<openQ>`、呈人、答后续跑、不得悬置）——R1 三子点（写者 / 并发 / 终止条件）全落。
- **F7 真解**：T7 append 注记补改判留痕句（「本条目风险节自评 review profile = Track A，经 human 2026-09-28 裁 OQ-1=(b) 改判 Track B……本批按 Track B 执行」）——改判进 ARQ 账本，为真留痕；OQ-1 行补「改判留痕随 T7 回写 ARQ 条目」。v1.1 语境下「Status 行已同步」读作本 PROPOSAL Status 行（:3 已载 OQ-1~4 全裁 Track B）成立，ARQ 侧由 T7 承载，两读均有着落。
- **F8 真解**：T3 两新表格行逐字行文以围栏块钉死（四列形态与 :32-33 既有行同构，intake 行含永不允许双条款、retro 行齐四列），注明 landing-summary 逐字承载；围栏内路径 token 不入锚集（preflight anchorUnion 十路径不变复证）。V7 可判性顺带复证：两新行分别含 `delivery/changes/intake/` 与 `delivery/changes/retro/**`，grep pattern 各 ≥1 命中。

### 新矛盾扫描（修订面）

T1 第 3/5/6 块新增文本与既有块（第 2 块字段定义、第 8 块接缝）、T2 替换句、T4/T5/T6/T7 改法、§0 非目标、§3 sweep、§5 验证集、§6 留根逐处交叉读——无语义矛盾、无 manifest 漂移、无 OQ 裁定变更、无新门/新断言引入。T7 补句不破坏 V6（append 段内 batch id 恰现一次，V6 基线 0 复跑在案）。

### 残留 MINOR（两条，措辞同步级，不阻断；landing 前可顺手修）

- **R2-M1**：T2 替换句（triage-change.md:13 新文）的语料枚举「requirements / architecture / contracts + CRQ 未结账」未随 F4 修订同步含 governance 面——语义靠句首「程序 SSOT 见 `change-intake.md`」指针承接（T1 第 3 块已含），不构成漏洞；建议顺手在枚举尾补「（语料枚举以 SSOT 为准）」或补「+ governance 面」一词，免两文枚举漂移。
- **R2-M2**：T1 第 2 块 faceMap 字段定义仍写「四面触面事实」，而第 3 块 F4 修订后 governance 命中亦「记入 faceMap」——faceMap 实际可载第五类行。建议第 2 块补一句「含 governance 探测命中行（路由 /arch-change 治理 profile，不建实现面档）」使字段定义与探测程序同步。

### Verdict

**GREEN**。R1 两项 BLOCKING（F1 census 失实致 V4 不可判、F2 sweep 漏 classify）均真解且复证实跑通过；MINOR 六条（F3–F8）处置到位；修订未引入新矛盾。残留 R2-M1/M2 两条措辞同步级 MINOR 供 landing 前顺手修，不阻断——若修，属 T1/T2 文本内一词一句级补写，不改 manifest、不改 OQ、不构成 packet 漂移，主线程可径直带入 FROZEN 前定稿；若不修，随首个 spine 实例 retro 收口亦无碍。

### 实测留痕（2026-09-28 R2 复跑，均于 C:/AI/KidPal 仓根）

1. `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` → `ok: true`、`anchorCount: 10`、`advisories: 0`、`findings: 0`；anchorUnion 十路径与 manifest 逐 token 相等（较 v1.0 无漂移）。
2. `grep -rn "第一刀" .claude/ AGENTS.md delivery/workstreams/`（V4 收窄后 scope）→ 恰 3 处（triage-change.md:13 / :21、change-triage.md:10），全在 T2 目标集内；AGENTS.md、delivery/workstreams/ 零命中。
3. `grep -c "260928-gov-review-cross-face-intake-spine" docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` → `0`（V6 基线不变，T7 append 后预期 = 1）。
