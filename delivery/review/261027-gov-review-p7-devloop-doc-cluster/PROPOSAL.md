# PROPOSAL · 261027-gov-review-p7-devloop-doc-cluster

> **状态：FROZEN v0.1 · 2026-10-27 · R1 GREEN（verdict 见 §7 R1；判档 Track B 主路径，只升不降，改法集不变）；凭「立项 ack（task_plan P7 节，human 2026-10-27 两拍）+ R1 GREEN」标冻**
> **承载 ARQ**：`ARQ-261025-adjacent-frozen-anchor-sweep`（T1）· `ARQ-261014-rename-batch-second-token-census`（T2）· `ARQ-261025-debt-archive-ref-atomic`（T3，只落修法 ①+③）· `ARQ-260908-known-red-count-no-mechanical-source` 子项③（T4）。立项 = workstream `gov-debt-reduction` P7 节（`delivery/workstreams/gov-debt-reduction/task_plan.md:102-108`，human 2026-10-27 两拍 ack：六条全部立项、修法照各条目建议改法；批形态乙 = 轻路径收 ①②③⑥ 四条）；四条 ARQ 的结项动作归主线程（见 §6 留根 1）。
> **review 轨**：轻路径（起草期自评，判档归 architect-reviewer、**只升不降**）。判据逐条见 §1。Track A 段判据不命中：方向已由四条 ARQ 的既有建议改动 + P7 立项 ack 收敛，无声明红队。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4，共六处编辑）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（一句话证成：五件文档面的起包期 / 起草期约定与示例，不新增门、不改任何闸判定、不触脚本与 settings）。
> **review profile**：G2（`.claude/references/**` 四件 + `docs/templates/**` 一件）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段，GREEN 结论前必须在场；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/task-intake-and-slicing.md
.claude/references/dev-loop.md
.claude/references/path-form-family.md
docs/templates/PROPOSAL.template.md
.claude/references/acceptance-evidence.md
```

---

## 0. 想实现的效果与差距

**目标效果**（P7 四条，逐条对应一个起包期 / 起草期的盲区）：

1. **相邻冻结锚扫查**（T1）：本包要改的实现点若已被别的包冻结断言钉过同一字面，起包期就把它扫出来并处置，不再等 builder 首轮或 verifier 终审才暴露。
2. **改名批两 token 族**（T2）：重命名类治理批的 census 同时枚举旧路径 token 与旧文件名 stem token，堵住「八类形态全绿而旧名引用全残」。
3. **debt 归档归位与引用同批**（T3）：结项条目的 `docRefs` / `sourceRefs` 预留 `_archive/<年>/` 形态、锚句随 acceptance id 定稿回填，不再靠 final 轮 `docs-sync` 红来发现。
4. **已知红现值口径单源**（T4）：host runner 报告头写明执行目录，现值数字一律取文档命令位置，同一实现不再出两份红名单。

**现状差距**（主线程 2026-10-27 实测 + 起草期亲核，证据逐条落在 §2 各 T）：

- T1：三条候选落点均无该义务文面；最近的邻接物是 planner 的「拆包快照保鲜核验」（`planner.md:57`），它只核引用存在性，不做目标符号级的跨包扫查。
- T2：`path-form-family.md` 全篇零「token 族 / stem」承载（现共六节，末节「相关」）；PROPOSAL 模板的 sweep 透镜义务（`PROPOSAL.template.md:77`）只覆盖「新门 / 新断言判定 pattern 预跑」。
- T3：`debt-render.mjs` 的归位机制在场（位置 = status 纯函数），但起包面没有「结项条目的引用须预留归档位」的自查句。
- T4：`acceptance-evidence.md` 只有子项②的 root 注入节，子项③（报告头执行目录 + 现值口径）零承载。

**非目标（本批不做，防 scope creep）**：

- **不触 planner.md / integrator.md**（agents 面属轻路径排除集）：T1 的承载面收在 task-intake + dev-loop，T3 只走起包侧文面、integrator 侧句省略。若评审认为必须改 agent 定义才成立，按只升不降挪出另批（本批未挪出任何条目，见 §1 末）。
- **不写新门、不写脚本、不改 settings / scope-map / verify-gates**：四条全部是文档义务，无机械消费方新增。
- **不动 runner 代码**（T4）：报告头字段的机械化属 host 测试基建面，维持 260920 批留根④ 的处置边界。
- **不立 debt 归档重定向 stub**（T3）：条目已裁倾向 ①+③ 纯文面，stub 会造第二事实源。
- **不改 spec 两文件、不触契约面、不动固件 / shim**：零 diff。
- **不引入新机制**：四条均为既有流程的约定增补，按 [change-triage.md](../../.claude/references/change-triage.md)「新机制批判定」不命中。

## 1. 判档依据（轻路径）

**逐文件落点**（全部在轻路径允许集 `.claude/references/**` 与 `docs/templates/**`）：

| 文件 | T | 改动性质 |
|---|---|---|
| `.claude/references/task-intake-and-slicing.md` | T1 / T3 | 插入两个小节（起包自查文面） |
| `.claude/references/dev-loop.md` | T1 | changeManifest 条目 append 一句 |
| `.claude/references/path-form-family.md` | T2 | 新增第六节 + 「相关」节顺延为第七节 |
| `docs/templates/PROPOSAL.template.md` | T2 | 起草期自检句 ② 内 append 一句 |
| `.claude/references/acceptance-evidence.md` | T4 | 追加一段（约定文面） |

**排除集逐项核查**（轻路径判据③）：`.claude/commands/**` / `.claude/agents/**` / `.claude/hooks/**` / `.claude/scripts/**` / `.claude/scope-map.json` / `*.settings.json` / `.claude/verify-gates.json` / `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` —— 十项零 diff（§5 V4 机械断言）。

**性质申明（向严写清）**：四条新增的是**文档义务**（起包检查项与起草义务），不是错字或排版；但它们**不改任何门 / 闸的判定语义、不新增机器闸、不新增脚本消费方**——义务的执行方是人（起草者 / 起包方）按命令自查。按 gov-change 轻路径判据②「零行为语义变化」的字面口径，判官可以取更严的读法；本批自评取轻，是因为改动全部收在文档面、无机器判定链变化，且立项 ack 已按 P7「轻路径四条」立项。若判官判②不满足 → 只升不降转 Track B 主路径（四查、核验、升级闸仍全套在），**改法集不变、不重做**。

**落点约束履约（立项 ack 附带）**：① T1 收 task-intake + dev-loop，未触 planner.md；② T2 两处并落（path-form-family 承载义务本体、模板句只给触发点并指回该表，不复述为第二 SSOT）；③ T3 收 task-intake 起包自查，integrator.md 侧句省略；④ T4 只落 acceptance-evidence。**挪出申报：无**——四条全部收在允许集内，没有条目因触排除集而需挪出。

## 2. 改法清单（T1…T4）

| T | ARQ | 落点 | 编辑数 |
|---|---|---|---|
| T1 | adjacent-frozen-anchor-sweep（建议改动 `…/ARQ-261025-adjacent-frozen-anchor-sweep.md:20`） | task-intake + dev-loop | 2 |
| T2 | rename-batch-second-token-census（建议改动 `…/ARQ-261014-rename-batch-second-token-census.md:18`） | path-form-family + PROPOSAL.template | 2 |
| T3 | debt-archive-ref-atomic（建议改动 `…/ARQ-261025-debt-archive-ref-atomic.md:20`，取 ①+③） | task-intake | 1 |
| T4 | known-red-count 子项③（条目 `…/ARQ-260908-known-red-count-no-mechanical-source.md:8`） | acceptance-evidence | 1 |

**装配与自证**：六处编辑以旧串锚定装配（`node .claude/scripts/assemble-edits.mjs`，清单 = 本 §2 各「旧串 / 新文」段；仓外副本 `C:/tmp/p7-assembly`），装配后 6/6 旧串在现盘恰命中 1 次、6/6 新文在副本恰命中 1 次（§5 基线 B1–B3）。本 §2 的十二个引文块与清单逐条对账（块对账脚本按「原文标签块 = `oldStr`、改后标签块 = `newStr`」归一比对，6/6 配对、未配对 0），非手抄转录。

### T1 · 相邻冻结锚扫查（intake / 1g 档）

**问题**：本包要改的实现点，若同一字面已被别的包冻结断言钉过，冲突在起包期完全不可见——要等 builder 首轮或 verifier 终审才撞上，一次至少烧一轮返工；跨包重锚还要人类出场批准。

**证据**：① `261018-sd-log-persist`——intake 只核了本包 changeManifest 与 scope-map，未扫既有冻结件里相邻字面锚，撞上 `261016-sd-card-mount` 冻结断言钉死的 `max_files=5`（本包要 8），builder 首轮 host 全量才暴露；② `261022-m5-k4-history-corpus-backfill`——起包逐字段对磁盘核验才发现 ADR-016 已改 K4-E2 措辞；③ 正向对照 `261018-sd-log-usb-read` 起包前扫过相邻锚，零 fixer 轮（证据源 = 条目 `ARQ-261025-adjacent-frozen-anchor-sweep.md:16`）；④ 最近邻接物现文（`planner.md:57`）：「**逐字段对磁盘核验**四问：① `acceptance[]`/`acceptanceTestsFile[]` 引用的文件是否存在；② 冻结件/红基线引用的对象是否仍在跑（未进归档区）…」——逐字段核引用存在性，无目标符号级扫查；⑤ 主线程复证（`task_plan.md:106`）：「三落点均无字面锚扫查义务文面」。

**根因**：起包检查项里缺一条「以目标符号为输入、以既有冻结件为语料」的动作。既有保鲜核验以本包 refs 为输入（查引用是否还在），方向相反；没有任何一步把「别处已钉死的同一字面」带进视野。

**改法**（两处）：

一、task-intake 新增小节，插在「### story 锚点引用与转写」之前（锚串 = 该标题行，现盘恰 1 次命中）。

原文（`task-intake-and-slicing.md:278`，逐字节）：

```text
### story 锚点引用与转写
```

改后（逐字节）：

```text
### 相邻冻结锚扫查（起包自查 · changeManifest 定稿后）

> ARQ-261025-adjacent-frozen-anchor-sweep 立（261027-gov-review-p7-devloop-doc-cluster T1）。纯起包检查项：零新门、零闸判据改动。

**做什么**：起包方（planner）在 changeManifest 定稿（architect 前置技术评估 1g 档产出、planner 回填）之后、finalize 之前，用本包 changeManifest 的目标符号与常量（函数名、宏名、文件名、路径字面、数值阈值）对既有冻结件做一次字面扫查。扫查面 = 仓内既有验收测试文件（`**/*.acceptance.*`）与其它已冻结断言文件的并集。命中即把「相邻冻结锚清单 + 冲突处置（沿用 / 重锚 / human 裁）」写进 plan.md 的「跨包事实与共享冻结面约束」节；无 plan.md 的包写进 taskpackage `notes`。**扫查结果为空也要显式记一句「零命中」**——「没写」与「没有」不能混同。

**为什么**：本包要改的实现点若已被别的包钉过同一字面（先例：某包冻结断言钉死 `max_files=5`，后继包要 8），冲突会推迟到 builder 首轮或 verifier 终审才暴露，一次至少烧一轮返工；而改既有冻结断言超出 tighten-only 窗口，还须该包 owner 与 human 窄口径批准重锚。起包期一次字面扫查把暴露时点提到最便宜的位置。扫查只产清单与处置建议，**不预批任何既有冻结件的改动**——改它仍须该包 owner 与 human 逐次授权。

**与 planner「拆包快照保鲜核验」的分界（防双头检查项）**：该步核的是**本包元数据引用的对象是否仍在盘上、仍在跑**（引用存在性，输入 = 本包 refs 与验收文件路径）；本扫查核的是**本包目标符号是否已被别处的冻结件钉过**（跨包字面冲突，输入 = changeManifest 目标符号）。输入不同、结论不同，互补不重叠，不并成一步、也不互代。

### story 锚点引用与转写
```

二、dev-loop 的 `changeManifest[]` 条目末尾 append 一句（锚串 = 该条目末句，现盘恰 1 次命中）。

原文（`dev-loop.md:194`，逐字节）：

```text
禁先改后补。
```

改后（逐字节）：

```text
禁先改后补。**起包自查（相邻冻结锚扫查）**：changeManifest 定稿后，起包方须用其目标符号与常量对既有验收测试文件与冻结断言做字面扫查，命中即记相邻冻结锚清单与冲突处置、空也显式记「零命中」；义务、落点与 planner 拆包快照保鲜核验（Step 1f）的分界见 [task-intake-and-slicing.md](task-intake-and-slicing.md)「相邻冻结锚扫查」节（261027 批 T1）。
```

**咬合裁定（并列新步，不并入 1f）**：两条路都试过——① 并入 planner 的 1f 当第五问，会把义务塞进 agents 面（轻路径排除集），且 1f 的输入是「拆包交接产物」，与本扫查「本包目标符号」不同源，硬并会造成双头检查项；② 在 task-intake 立独立小节、由 dev-loop 条目做入口指引，义务的消费方（起包方）天然读得到，且与 1f 的分界句写在同一节里，判官与执行者都能对账。取 ②。

**为什么这样改**：把「扫查动作 + 落点 + 空结果也记录」三件钉死，且与既有 1f 划清输入与结论的分界——这是本条最容易被读成重复动作的地方，分界句固定在同一节内，防下一批再发明一个同义检查。

目标文件：`.claude/references/task-intake-and-slicing.md`（改） · `.claude/references/dev-loop.md`（改）

### T2 · 改名批两 token 族（census 并枚举旧路径与旧文件名 stem）

**问题**：重命名类治理批只枚举旧路径 token，旧文件名 stem 的引用（自指串、命令行 file:line、裸文件名）全部漏网，落地后断链，只能靠判官打回兜底——每批多付一轮评审。

**证据**：`261014-gov-review-crq-debt-rename` 批 R1 实测——起草期只覆盖目录 token `docs/crq` 的字面量，旧 stem（`crq-render` / `crq-doctor` / `crq-ledger-shared` / `crq-item`）以三种形态散布，V1 六条路径形态模式无一可命中（条目 `ARQ-261014-rename-batch-second-token-census.md:14`）；主线程复证（`task_plan.md:106`）：「path-form-family.md 零「token 族/stem」承载」；条目 risk 行在案裁定：「stem 引用属第二 token 族、非新形态类」（同文件 `:20`）。

**根因**：形态族谱表按「同一段路径的文本写法」分轴，八类模式都以路径分隔符或目录 token 为锚；stem 是**文件名**层的 token，没有分隔符可锚，天然落在八类之外。族谱的 append 义务（第五节用法第 4 条）只定义了「遇新形态 append」，没有定义「第二个对象」该如何承载，于是 stem 变成没人认领的面。

**改法**（两处；并落理由：义务本体放族谱表，模板句只给动笔时的触发点并指回该表，不复述为第二 SSOT）：

一、path-form-family 新增第六节（承载义务本体），原「相关」节顺延为第七节（锚串 = 该标题行，现盘恰 1 次命中）。

原文（`path-form-family.md:62`，逐字节）：

```text
## 六、相关
```

改后（逐字节）：

```text
## 六、第二 token 族：文件名 stem（改名批专用）

> 261027-gov-review-p7-devloop-doc-cluster T2 立（承 `ARQ-261014-rename-batch-second-token-census`）。**分界裁定（261014 批 R2 先例在案）**：旧文件名 stem 的引用属**第二 token 族、非新形态类**——它不 append 进第一节那张形态表；第五节用法第 4 条「遇新形态 append」的义务只对**形态类**生效，不对 token 族生效。

**两个 token 族**：第一节八类管的是「同一段目录 / 路径的文本写法」；改名批（重命名文件 / 脚本 / 模板 / 命令）还有第二个对象——**旧文件名 stem**。261014-gov-review-crq-debt-rename 批 R1 实测：起草期只枚举了目录 token `docs/crq` 的字面量，旧 stem（`crq-render` / `crq-doctor` / `crq-ledger-shared` / `crq-item`）仍散在三种形态里——自指串（`GENERATED_MARKER`、indexHeader 模板、self-check 输出串）、指令面命令行（11 件 file:line 引用）、裸文件名引用。第一节的路径形态模式无一能命中 stem，于是出现「八类全绿而旧名引用全残」，落地即断链。

**起草义务**：凡改名批，census 必须**同时枚举两个 token 族**——① 旧路径 token（按第一节八类逐类清扫）；② 旧文件名 stem token（按本节子检清扫）。两族各配一条可机械执行的子检，两条子检都写进 PROPOSAL §5 验证集；只做第 ① 族不算完成。

**stem 族清扫子检模式**：把旧 stem 的公共前缀与其全部后缀编成交替组，匹配所有出现形态（下例是 261014 批实形，前缀与后缀按当批对象替换）：

`crq-(render|doctor|ledger-shared|item)`

迁移写法 = `<旧 stem 前缀>-(<全部后缀>)`，后缀按长优先排列防前缀互吃（`ledger-shared` 排在 `ledger` 之前）。**清理与验收判据用同一条模式**（同一条模式两次使用，理由见第四节）——命中数须为 0，非 0 即引用面尚未清完，回补而不豁免。

**与形态表的关系**：两族子检都跑完后，第五节用法第 3 步（验收期用同一条模式）照常适用；只有出现**新的文本形态**时才 append 第一节的表，第二 token 族本身不构成那次 append。

## 七、相关
```

二、PROPOSAL 模板的起草期自检句 ② 内 append 一句（锚串 = 该句末括注，现盘恰 1 次命中；不新增第 ⑥ 句、表头「实为五句」计数不动）。

原文（`PROPOSAL.template.md:69` 行内片段，逐字节）：

```text
（260914 批因路径批改付了四轮评审 + 落地四轮修复，其中最隐蔽的一类四轮机检全漏）。
```

改后（逐字节）：

```text
（260914 批因路径批改付了四轮评审 + 落地四轮修复，其中最隐蔽的一类四轮机检全漏）。 **改名批（重命名文件 / 脚本 / 模板 / 命令）另须枚举旧文件名 stem 第二 token 族**——stem 引用属第二 token 族、非新形态类，义务与子检形态见该表第六节（261014 批 R1 打回实证；261027 批 T2）。
```

**为什么这样改**：义务本体放族谱表（它是路径类起草的 SSOT，改法类变更的读者必到）；模板句只承担可发现性——起草者写 §2 自检时就被点回该表，且明写「见该表第六节」，不复述义务内容。第二 token 族与「遇新形态 append」的分界在同一节开头裁定，防止下批把 stem 当第九形态 append 进第一张表。

目标文件：`.claude/references/path-form-family.md`（改） · `docs/templates/PROPOSAL.template.md`（改）

### T3 · debt 结项归档归位与引用同批

**问题**：包内结项 debt 条目后，渲染器按状态把条目搬进 `_archive/<年>/`，而 `docRefs` / `sourceRefs` 仍指顶层旧路径，引用静默陈旧，要到 final 轮 `docs-sync` 才红——本波三包各多烧一轮或一次权内修正。

**证据**：① `261013-lang-config-thinking-fix`——integrator 收口把条目归档，planner 的 `sourceRefs` / `docRefs` 立刻陈旧，final r4 `docs-sync` 红才发现；② `261017-prompt-v11-schedule-recall`——同型，`debt-render.mjs` 自动归位、TaskPackage 的 `docRefs` 仍指旧顶层路径，final 红在 missingDocs；③ `261022-p3-record-chrome-ghost-fix`——`docRefs` 锚句起包时未回填，r2 `docs-sync` 红（证据源 = 条目 `ARQ-261025-debt-archive-ref-atomic.md:16`）；④ 归位机制现文（`repo-structure.md` docs/debt 行）：「**已结条目由渲染脚本自动归位 `_archive/<结项年>/`**…位置 = 状态的纯函数，**人不搬文件**」。

**根因**：归档搬迁是渲染器按 status 产生的副作用，引用更新是起包时手写的字段事实，两者之间没有同批绑定；起包面也没有「本包要结项哪条、它的引用写终态还是旧位」的自查句。

**改法**（一处：task-intake 新增小节，插在「### deliveryImpact」之前；锚串 = 该标题行，现盘恰 1 次命中）：

原文（`task-intake-and-slicing.md:130`，逐字节）：

```text
### deliveryImpact
```

改后（逐字节）：

```text
### docRefs / sourceRefs 指向 debt 条目时的归档形态（起包自查）

> ARQ-261025-debt-archive-ref-atomic 立（261027-gov-review-p7-devloop-doc-cluster T3）。纯起包自查文面：零新门。

- **归位是渲染器的副作用，人不搬文件**：debt 条目结项后，`debt-render.mjs` 按「位置 = 状态纯函数」把条目从 `docs/debt/<域>/` 自动归位到 `docs/debt/<域>/_archive/<结项年>/`。因此引用 debt 条目的路径有两种合法形态——未结顶层位与归档位，切换时点由状态翻转决定。
- **起包自查**：本包要结项 debt 条目（或在包内翻转其 `status`）时，`docRefs[]` / `sourceRefs[]` 若指向该条目，须预留 `_archive/<结项年>/` 归位形态——按包内结项时点直接写终态路径，或在条目结项后同批回填；不得留顶层旧路径，否则 `docs-sync` 要到 final 轮才红（三包实证：261013-lang-config-thinking-fix / 261017-prompt-v11-schedule-recall / 261022-p3-record-chrome-ghost-fix）。
- **锚句即时回填**：`docRefs` 的锚句在 acceptance id 定稿时即回填——起包时先写 plan、acceptance id 后定的形态会留下空锚，同样拖到 verify 红。
- **不立重定向 stub**：在顶层留 `docs/debt/<域>/<ID>.md` 指向归档位的 stub，会让同一事实有两个落点，与「位置 = 状态纯函数」的归位机制冲突，本批不立。

### deliveryImpact
```

**为什么这样改**：把「引用写哪个位」绑到「本包结不结项这条」这个起包时就能回答的问题上；同时写清机制归渲染器、人不搬文件，避免下一批误把手搬归档当处置。修法 ②（重定向 stub）按条目既裁不做，理由写在同一节内，评议时可直接核。

目标文件：`.claude/references/task-intake-and-slicing.md`（改）

### T4 · host runner 报告头记执行目录 + 现值口径单源

**问题**：「上游已知红」一类现值数字在 launch 面是手写快照，且同一 ELF 在文档命令位置与仓库根跑出两个数字；口径不写，红基线判读与既有红记账全靠人肉对账，判读错还会把既有红误记成本包回归。

**证据**：① 实测反例现文（`acceptance-evidence.md:49`）：「同一 ELF 在**文档命令位置**…跑出 **3** 条既有红，在**仓库根**跑出 **2** 条——同一实现两份数字」；② 条目子项③ 现文（`ARQ-260908-known-red-count-no-mechanical-source.md:8`）：「③ runner 报告头未做——**③ 留下次动 acceptance-evidence 约定的治理批**」；③ 260920 批处置边界（`delivery/archive/changes/arch/260920-gov-review-bearing-strength-align/PROPOSAL.md:104`）：「子项③（host runner 报告头写执行目录）**不并入**（属 host 测试基建面，见 §6 留根④）」；④ 主线程复证（`task_plan.md:106`）：「③ 报告头执行目录现值口径零承载」。

**根因**：子项② 落地时把「在断言内显式取仓根」写成了撰写约定，但「报告头记执行目录 + 现值数字取哪个位置的输出」是**读侧**约定，两件事同源不同面；读侧不写，跑测者与判读者就没有同一坐标。

**改法**（一处：acceptance-evidence 的 root 注入节末追加一段，锚串 = 该节收束段，现盘恰 1 次命中）：

原文（`acceptance-evidence.md:51`，逐字节）：

```text
**为什么重要**：evidence 类齿判定不稳定 = 「同一实现两次跑出不同红名单」，与「证据可重现」的要求直接相悖；手写快照随 main 前进必然腐化、无门报警。
```

改后（逐字节；目标文件工作树为 CRLF、索引为 LF——逐字装配清单内两段以 CRLF 相连，本块以 LF 承载，`--cite-check` 双侧行尾归一口径下逐字命中）：

```text
**为什么重要**：evidence 类齿判定不稳定 = 「同一实现两次跑出不同红名单」，与「证据可重现」的要求直接相悖；手写快照随 main 前进必然腐化、无门报警。

**host runner 报告头记执行目录 + 现值口径单源（ARQ-260908 子项③，261027-gov-review-p7-devloop-doc-cluster T4）**：报告头（跑 host runner 的产出首部）须写明本次执行目录；「上游已知红」一类现值数字**一律取文档命令位置**（`firmware/main/test/host_app/`，即 `firmware/main/test/HOST_TEST_CONVENTIONS.md` [C4.4] 规定的运行命令所在位）下的输出。报告、红基线与 launch 面引用同一个数字时须同址同源，不得一半取仓根跑出的结果——同一实现两份数字（见本节实测反例）会把既有红误记成本包回归。**兑现方与降级**：现行 host runner 没有机械写报告头的产线，跑测者手写该行即可（约定级兑现，不新增工具）；报告头字段的机械化属 host 测试基建面改动，按需另走 dev-loop，不在本约定内预批（260920 批留根④ 的处置边界维持）。
```

**为什么这样改**：把「执行目录」与「现值口径」写成一对——报告头给出坐标、口径规定数字的取位，读侧才有单一坐标；同时写清兑现方（跑测者手写）与机械化边界（不预批、另走 dev-loop），避免留下没有兑现方的能力声明，并与 260920 批的既有裁定一致。

目标文件：`.claude/references/acceptance-evidence.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 流程定义 G2 | `.claude/references/task-intake-and-slicing.md` | 改（T1 / T3） | 新增两节起包自查；消费方 = planner 起包、判官与核验官读义务 |
| 流程定义 G2 | `.claude/references/dev-loop.md` | 改（T1） | changeManifest 条目 append 起包自查入口句；消费方 = planner intake |
| 流程定义 G2 | `.claude/references/path-form-family.md` | 改（T2） | 新增第六节（第二 token 族）+ 相关节顺延为第七节；消费方 = 改名批起草者、判官 |
| 治理制品骨架 G2 | `docs/templates/PROPOSAL.template.md` | 改（T2） | 起草期自检句 ② 内 append 一句；消费方 = 后续全部 PROPOSAL 起草者 |
| 流程定义 G2 | `.claude/references/acceptance-evidence.md` | 改（T4） | 追加报告头与现值口径约定段；消费方 = acceptance-author、跑测者、判官 |
| 生成件（不在 manifest） | `delivery/review/INDEX.md` | 再生 | `changes-index.mjs --upsert` 单行钉入（主线程，禁手改） |
| 指令根 G1 | `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` | 未触 | 零 diff |
| agents / commands / hooks / scripts / settings / scope-map / verify-gates | `.claude/agents/**` · `.claude/commands/**` · `.claude/hooks/**` · `.claude/scripts/**` · `.claude/scope-map.json` · `*.settings.json` · `.claude/verify-gates.json` | 未触 | 零 diff；无新增门、无门判定链变化 |
| 契约面 / 规范 spec | `docs/contracts/**` · `docs/requirements.md` · `docs/architecture.md` | 未触 | 零 diff |
| 实现面 | `firmware/**` / `server/**` / `tools/**` | 未触 | 零 diff |
| 账本 | `docs/debt/gov/ARQ-*.md` 四条 | 未触（本批起草） | 结项翻转归主线程（§6 留根 1） |

**【硬项 · sweep 透镜义务】**：本批**不落地新门、不落地新断言 pattern**（四条均为文档义务，无脚本读取新文面），故无「新判定 pattern 全仓预跑」项。新字面碰撞 census（起草期 2026-10-27 实跑，语料 = 全仓 `.md` / `.json` / `.mjs`，排除本批目录）：

- `相邻冻结锚扫查`：**0 文件**（全仓零承载，本批为首钉）。
- `第二 token 族`：**2 文件**——`docs/debt/gov/ARQ-261014-rename-batch-second-token-census.md`（条目原文，立项材料）与 `delivery/archive/changes/arch/261014-gov-review-crq-debt-rename/retro.md`（归档历史证据，append-only）。两者都是处方源文 / 历史证据，不构成碰撞语义。
- `host runner 报告头`：**1 文件**——`delivery/archive/changes/arch/260920-gov-review-bearing-strength-align/PROPOSAL.md:104`（当年裁定「不并入」的历史证据）。本批不与它冲突：它裁定的是「机械化不做」，本批只立读侧约定，并在 T4 正文里复述该边界。
- `指向 debt 条目时的归档形态`：**0 文件**。
- 附：T2 示例模式 `crq-(render|doctor|ledger-shared|item)` 全仓现跑 **57 文件**（其中 29 在 `delivery/archive/changes`、多个历史 taskpackage）——它是 261014 批的历史实形，**不是**本批引入的现行判据；新文面已注明「前缀与后缀按当批对象替换」，本批不新增任何以它为判据的门。

**【硬项 · 复扫模式完备性先验】**：本批唯一的退役语面 = 标题重编号（`path-form-family.md` 的「相关」节由六变七）。模式集显式枚举全形态：`## 六、相关`（行首形态）/ `六、相关`（裸串）/ `第六节`（节号引用形态）。实跑（`grep -rn`，语料全仓 `.md`）：`## 六、相关` 现行承载 = **1 处**（`path-form-family.md:62`，本批改为 `## 七、相关`）；其余两形态 **0 处**——即除该标题外无任何对「第六节」的既有引用需要随之改号。另：其余四处编辑全为「插入型」（新文本挂在锚行前 / 后），不产生退役语面。

## 4. human 取舍点（OQ）

**本批无待 human 拍板项**：修法照四条 ARQ 的既有建议改动，落点照 P7 立项 ack 附带的落点约束，条目内已裁的倾向（debt 归档取 ①+③、不立 stub）照办。以下为实现层技术取舍，判官可 challenge，任一被推翻都有明确退路：

- **取舍 A · T1 立独立小节，不并入 planner 的保鲜核验**：理由见 T1「咬合裁定」；退路：判官若坚持并入，该条自动触 agents 面 → 挪出另批（本批改法集相应减一 T），或由判官判「分界句已足够」维持。
- **取舍 B · T2 两处并落（族谱表 + 模板句），模板句走第 ② 句内 append**：不新增第 ⑥ 句、不动「实为五句」计数——新增句块会牵动表头计数与既有批的引用口径；退路：判官若要求独立成句，改回独立段落即可，判据不变。
- **取舍 C · T3 只走起包侧文面，不留 integrator 侧句**：归档由渲染器自动完成，integrator 无手搬动作，收口侧再写一遍会变成同一义务的两处承载；退路：若判官认为收口侧需一句「归档后核引用」，那属 agents 面，挪出另批。
- **取舍 D · T4 兑现方写「跑测者手写」，不建工具**：维持 260920 批的边界；退路：机械化若被立项，另走 dev-loop（本批不预批）。
- **取舍 E · 判档取轻（文档义务、零门语义）**：最严读法可判②不满足；退路：只升不降转 Track B 主路径，改法集不变（§1 末）。

## 5. 验证集

> **断言纪律**：所有预期值 = 起草期实跑输出回填（命令 + 关键输出见基线与留痕），不推算；落地期实跑若与本表有字节差异，以落地实跑为准覆写本表并在 §7 留痕。
> **装配**：六处编辑由共享锚文装配器装配（`node .claude/scripts/assemble-edits.mjs C:/tmp/p7-work/edits.json C:/tmp/p7-assembly`，仓外副本）；清单 `oldStr` = 本 §2 各段旧串、`newStr` = 新文全文。
> **时点**：pre-commit = 落地文件在盘、未 commit 即可跑；post-commit = 须落地提交后才成立。

**基线（起草期 2026-10-27 实跑）**：

| # | 命令 | 实跑输出（逐字 / 摘要） |
|---|---|---|
| B1 | 清单预检（`node C:/tmp/p7-work/build.mjs`，逐条 `split(oldStr).length - 1`） | 6/6 条 `hits: 1`（旧串在现盘各恰 1 次） |
| B2 | 装配（`node .claude/scripts/assemble-edits.mjs C:/tmp/p7-work/edits.json C:/tmp/p7-assembly`） | `ok:true`、5 件；sha256（前 → 后）：task-intake `dee17a25687fb8f2…→8ee2586a2f82fe7b…`（afterLines 483）· dev-loop `666d5b97b7e24764…→8f68480a10926e17…`（360）· path-form-family `4ddfb48cd7eb76e9…→724605a93f6a0ffb…`（82）· PROPOSAL.template `658bb4d43f279f65…→77d63493acae7867…`（103）· acceptance-evidence `51576a596096b98f…→c1a244baa40a1d80…`（74） |
| B3 | 装配副本对账（`node C:/tmp/p7-work/verify.mjs`，双侧行尾归一） | 6/6 条：旧串在现盘 = 1、新文在副本 = 1、新文在现盘 = 0（未落地）、`bad: 0`；另：新增文本中出现的「齿」字 = **0**（输出标准）、U+FFFD = 0 |
| B4 | 逐文件 diff 规模（`git diff --no-index --numstat`，仓内 ↔ 副本） | task-intake **19+/0−** · dev-loop **1+/1−** · path-form-family **17+/1−** · PROPOSAL.template **1+/1−** · acceptance-evidence **2+/0−** |
| B5 | 行尾一致性（副本读回计数） | 四件 LF 文件 `bareLf = lines`（无 CRLF）；acceptance-evidence **73 行 / CRLF 73 / 裸 LF 0**——插入段随目标文件保持均匀 CRLF（`git ls-files --eol` 现文 = `i/lf w/crlf`，落地提交后索引仍为纯 LF） |
| B6 | 结构抽查（副本） | path-form-family 节序 `一…五 / 六（新）/ 七、相关`；task-intake 新节在「plan.md 全量义务」与「story 锚点引用与转写」之间、T3 新节在「sourceRefs」与「deliveryImpact」之间；`## 六、相关` 在副本 = 0、`## 七、相关` = 1 |
| B7 | 普查 census（§3） | `相邻冻结锚扫查` 0 文件 · `第二 token 族` 2 文件 · `host runner 报告头` 1 文件 · `指向 debt 条目时的归档形态` 0 文件 · `crq-(render|doctor|ledger-shared|item)` 57 文件（历史实形，非现行判据） |
| B8 | 退役语面 census（§3） | `## 六、相关` = 1（`path-form-family.md:62`）· `六、相关`（裸串）同 1 处 · `第六节` = 0 |
| B9 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261027-gov-review-p7-devloop-doc-cluster/PROPOSAL.md` | `ok:true`、`section2BodyFound:true`、`anchorCount:5`、anchorUnion = manifest 五件逐 token 一致、`findings:[]`、`advisories:[]`、exit 0 |
| B10 | `node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261027-gov-review-p7-devloop-doc-cluster/PROPOSAL.md --phase draft` | `ok:true`、`phase:draft`、`frozen:false`、`targetCount:5`、**checkedCount 6 / missing 0 / notChecked 6（六块新文）/ unverified 0**；六条 checked 各 `matched` 指向对应目标文件 |
| B11 | 落地相位沙盘：装配副本五件 + 本稿复本落 `C:/tmp/p7-landsim`（同仓根相对路径），`CLAUDE_PROJECT_DIR=C:/tmp/p7-landsim node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261027-gov-review-p7-devloop-doc-cluster/PROPOSAL.md --phase landing` | `ok:true`、`phase:landing`、`targetCount:5`、**checkedCount 6 / missing [] / notChecked 6（六块旧串）/ unverified []**；六条 matched 逐条指向对应目标文件——V2 的终态预演 |
| B12 | 本稿结构自检 | 341 行、LF（CRLF 0）、U+FFFD 0、`### T<n>` 标题 4、目标文件锚行 4、围栏定界行 26（偶数）、manifest 块 5 行 |

**落地验证集**：

| # | 时点 | 命令 | 预期（实跑回填 / 正推） | 判别力 / 配对 |
|---|---|---|---|---|
| V1 | pre-commit | ① 落地前复跑装配到全新输出根：`node .claude/scripts/assemble-edits.mjs C:/tmp/p7-work/edits.json C:/tmp/p7-prelaunch`；② 落地后对仓内五件跑 sha256（Node `createHash('sha256').update(content,'utf8')`） | ① `ok:true`、5 件、逐件 `afterSha256` 与 B2 逐值相同；② 五件仓内 sha256 与 B2 `afterSha256` 逐值相同（task-intake `8ee2586a2f82fe7b…` · dev-loop `8f68480a10926e17…` · path-form-family `724605a93f6a0ffb…` · PROPOSAL.template `77d63493acae7867…` · acceptance-evidence `c1a244baa40a1d80…`） | ① 反向：任一锚漂移即 exit 1（0 命中）或 count≠1（多命中）且零写盘；② 负向（无字节差）× 正向（五件集合 = manifest 集合） |
| V2 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261027-gov-review-p7-devloop-doc-cluster/PROPOSAL.md`（FROZEN 后自动 phase=landing） | `ok:true`、`phase:landing`、`targetCount:5`、**checkedCount 6 / missing [] / notChecked 6（旧串块）/ unverified []**；起草期沙盘预演 = B11（逐值相同） | 负向（missing 为空）× 正向（checked 非空且逐条 `matched != null`） |
| V3 | pre-commit | 字面正推（`grep -cF` 逐项，仓内落地文件） | task-intake：`### 相邻冻结锚扫查（起包自查 · changeManifest 定稿后）` 0→1、`### docRefs / sourceRefs 指向 debt 条目时的归档形态（起包自查）` 0→1；dev-loop：`**起包自查（相邻冻结锚扫查）**` 0→1；path-form-family：`## 六、第二 token 族：文件名 stem（改名批专用）` 0→1、`## 六、相关` 1→0、`## 七、相关` 0→1；模板：`另须枚举旧文件名 stem 第二 token 族` 0→1；acceptance-evidence：`**host runner 报告头记执行目录 + 现值口径单源` 0→1 | 每条新字面配「旧字面 = 0」或「未误改哨」：`### story 锚点引用与转写` 1→1、`### deliveryImpact` 1→1、`实为五句` 1→1、`禁先改后补。` 1→1（作为新句前缀仍恰 1 次） |
| V4 | pre-commit | `git diff --name-only -- docs/contracts/ docs/requirements.md docs/architecture.md server/ firmware/ tools/ .claude/agents/ .claude/commands/ .claude/hooks/ .claude/scripts/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json AGENTS.md CLAUDE.md docs/DEV-GOVERNANCE.md` | 空输出 | 正向配对：`git diff --name-only`（生成件之前）恰 = manifest 五件（本批 PROPOSAL 为未跟踪新件、不进 `git diff`）；并发改动（如 `task_plan.md` / INDEX 在途）逐件核性质并记入 landing-summary |
| V5 | pre-commit | `git status --porcelain` 中本批目录内的 `_scratch` / 临时件 | 无（本批全部起草期产物在仓外 `C:/tmp/p7-work` 与 `C:/tmp/p7-assembly`，零入仓） | 负向（仓内无残留）× 正向（V4 的「diff 恰等于 manifest」） |
| V6 | post-commit | `node .claude/scripts/changes-index.mjs --upsert 261027-gov-review-p7-devloop-doc-cluster` | `delivery/review/INDEX.md` 出现该 id 行；须在落地 commit 之前跑（主线程 chore） | 生成件，禁手改 |
| V7 | post-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs 261027-gov-review-p7-devloop-doc-cluster --source-ref HEAD` | 预期 `PASS (…, kind=gov)`、exit 0 | **未实测**（依赖 FROZEN + landing-verdict）；本批无契约 diff |

**起草期自检留痕（模板五句的落地记录，2026-10-27）**：

1. **preflight 实跑**：见 B9；通过后才给判官。
2. **路径形态族谱**：本批无搬家、无存量路径改落；T2 新增的是义务文面（族谱表本身）而非路径替换。新增字面已按 §3 census 预跑（B7）。
3. **承重引文 byte 对账**：§2 十二个引文块与清单逐条对账（块对账脚本，6/6 配对、未配对 0，双侧行尾归一），零手抄长输出；`--cite-check --phase draft` 见 B10。
4. **长文落地自检**：本稿分块写入、写后机检——341 行、LF（CRLF 0）、U+FFFD 0、围栏定界 26（偶数）、`### T<n>` 标题 4、目标文件锚行 4、manifest 块 5 行（B12）。
5. **脚本批拆分自查**：本批 `.claude/scripts/**` 零 diff（V4），拆分纪律不适用。

## 6. 留根清单

1. **四条 ARQ 结项归主线程**：`ARQ-261025-adjacent-frozen-anchor-sweep` / `ARQ-261014-rename-batch-second-token-census` / `ARQ-261025-debt-archive-ref-atomic` / `ARQ-260908-known-red-count-no-mechanical-source`——状态翻转与文末 append 属账本登记动作（主线程），architect 不触 `docs/debt/**`；T1–T4 落地即四条修法全部消化，条目可按主线程登记口径结项。
2. **T1 无机械闸**：扫查执行方是人，靠命令与「零命中也要记」的纪律；复议触发 = 出现「未扫、冲突又在 builder 首轮暴露」的新实例（此时再议机检，不预设）。
3. **T2 stem 子检是模式模板**：示例前缀与后缀按当批对象替换；复议触发 = 出现第三种 stem 引用形态（如配置键内嵌 stem）需要在该节 append 新子形。
4. **T4 报告头机械化未做**：维持 260920 批留根④ 的边界（host 测试基建面）；复议触发 = 需要机器解析报告头字段（如快照脚本消费）。
5. **未触 agents 面**：planner.md / integrator.md 零 diff；若判官认为 T1 的分界句不足以防双头、或 T3 需收口侧一句，按只升不降挪出另批（本批不改）。
6. **生成件**：`delivery/review/INDEX.md` 由 `changes-index.mjs --upsert` 再生，不进 manifest。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · architect-reviewer 评审记录（2026-10-27）

verdict: GREEN

**四查结论**（全部亲核实测，未采信转述）：

1. **意图保真：过。** T1–T4 与四条 ARQ 建议改动逐条对得上：T1 = ARQ-261025-adjacent-frozen-anchor-sweep.md:20 全要素（目标符号/常量输入、`**/*.acceptance.*` + 既有冻结件语料、命中清单 + 沿用/重锚/human 裁三档处置、空也显式记「零命中」、零新门零闸改）；T2 = ARQ-261014:18 双 token 族 census 义务 + 子检模式迁移 + 261014 批 R2「第二 token 族、非新形态类」分界先例照录；T3 = ARQ-261025-debt-archive-ref-atomic.md:20 取 ①+③、② stub 拒绝理由（第二事实源）成文同节；T4 = ARQ-260908:8 子项③ 报告头执行目录 + 现值口径单源（文档命令位），与 260920 批 PROPOSAL.md:104「子项③不并入（属 host 测试基建面）」既有裁定咬合（该引文逐字核对无误）。立项 ack 三条落点约束（task_plan.md:107）逐条履约：T1 未触 planner.md、T3 integrator 侧句省略、T4 未触 runner 代码；复证记录「起草期须写咬合防双头」→ T1 分界段在场且与 planner.md:57（1f）实文吻合（1f 逐字段核验四问引文逐字核对一致）。
2. **纪律合规：判据①过、判据③过，判据②不满足（见判档）。** diff 全落允许集（references 四件 + templates 一件，manifest 一致）；十项排除集零触、V4 机械断言兜底；不触 spec / 契约 / 实现；`.claude/scripts/**` 零 diff、拆分纪律不适用。prompt-authoring 卫生：新文面每句带操作语义与可检查终点（「零命中也要显式记」），权限边界句（不预批既有冻结件改动、机械化不预批）属 HQ-1 保全类、在场合规。
3. **撞车复核：过。** ① T1 与 planner 1f：输入不同（本包目标符号 vs 本包 refs 引用存在性）、结论不同，分界段与 1f 实文互证，不构成双头；② T3 与 repo-structure.md docs/debt 行「位置 = 状态的纯函数，人不搬文件」引文逐字吻合、同向不冲突；③ T2 与 path-form-family 既有结构：第五节用法第 4 条 / 第四节「同一条模式两次使用」/ 第五节第 3 步三处回引节号与实文一致，新节裁定「append 义务只对形态类生效」是对既有义务的边界澄清（先例在案）、非改写；④ 新字面 census 亲核：「相邻冻结锚扫查」「指向 debt 条目时的归档形态」全仓 0 承载（首钉）；「第二 token 族」2 文件、「host runner 报告头」1 文件，均为处方源文 / 历史证据件，非碰撞。
4. **牵连面完整：过。** 退役语面（「六、相关」→「七、相关」）模式枚举齐（行首 / 裸串 / 节号引用三形态），`.md` 语料实测仅 path-form-family.md:62 一处（本批目标）；各锚串逐文件恰 1 次命中（task-intake 两标题、dev-loop.md:194「禁先改后补。」、template:69 括注、acceptance-evidence.md:51 收束段均亲核唯一）；六处编辑均为插入 / 行内 append，B4 行数账（19+/0−、1+/1−、17+/1−、1+/1−、2+/0−）与新文形状自洽；§5 验证集判别力足——V1 五件 sha256 全文件钉死（任一实现反转必红）、V2 missing×checked 配对、V3 逐条 0→1 配四哨 1→1、V4 排除集空 × diff 恰 = manifest 正向配对，无真空绿项。

**判档结论：Track B 主路径（只升不降）。** 轻路径判据②枚举「措辞/格式/链接/勘误」不涵盖本批性质：四条改动全部是**新增规范性义务文面**（起包扫查动作、双族 census 义务、归档引用形态自查、报告头与现值口径约定），执行方（planner / 起草者 / 跑测者）按新文面的行为将实质改变——这是流程语义增补，不是措辞级勘误；AGENTS.md 对轻路径的定性「文档面措辞级批」同此口径。「零机器闸 / 零脚本消费方」只说明 Track A 段判据（收敛度）不命中，不构成判据②的满足。起草者 §1 已向严申报此张力并预置退路（取舍 E），按「只升不降 + 存疑向严」裁：**转 Track B 主路径，改法集不变、不重做**——本 PROPOSAL 形态已具 Track B 全件（§0–§7 齐备、验证集已实跑回填），主线程按 Track B 七步落地即可：landing-summary 用全件骨架、retro 不豁免，其余不变。

**findings**（无 BLOCKING / SHOULD；三条 NIT 记录在案、均无落地前必改项）：

- **N1（NIT · census 口径自洽性）**：§3/B8 退役语面 census 声明语料 = 全仓 `.md`、报 `第六节` = 0；按 B7 同款全语料（.md/.json/.mjs）实测另有 1 处——`delivery/taskpackages/260924-perf-k1-roundtrip-latency/evidence/probe-batches/post-p1-b-low.json:22`（历史探针证据件，非对 path-form-family 节号的活引用，不构成撞车）。记录在案，防下一批照抄「全语料 0」表述。
- **N2（NIT · T1 落位）**：T1 新节落在 task-intake「拆包链」节内部（「plan.md 全量义务」与「story 锚点引用与转写」之间），而义务适用面是一切起包、不限于拆包链。发现性由 dev-loop.md 入口句 + 节名自描述兜底，且紧邻其引用的「跨包事实与共享冻结面约束」定义位，取舍可接受、不打回。
- **N3（NIT · 落地注意）**：V1① 依赖仓外副本 `C:/tmp/p7-work/edits.json` 在落地期仍在且未漂移；该件若丢失，§2 十二个引文块即等价编辑清单可重建。landing-summary 记一句副本路径即可。

**落地补充指令**：§1「review 轨：轻路径」为起草期自评，判档以本记录为准（Track B）；取舍 E 退路已被触发（转 Track B），取舍 A–D 维持；状态转移与 FROZEN 标注按 Track B 批口径执行。
