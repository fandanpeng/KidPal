# PROPOSAL · 260915-gov-review-worktree-pool-reuse

> **状态：FROZEN v1.0 · 2026-09-15 · 可进 landing**（凭据：human 立项 ack 2026-09-15「立项」+ OQ1 撤销 / OQ2 已拍 (a) / OQ3 已拍串行+复用 / OQ4 撤销；architect R1 需修订〔review-r1-architect.md，F1–F3 BLOCKING + F4–F6 SHOULD〕→ 主线程回填 v0.5 → R2 GREEN〔review-r2-architect.md，N3/N4 NIT 已在冻结前收进 T1②/V8〕）。
> **承载 ARQ**：无——human 2026-09-15 会话直接触发（W1 launch 后对「每包一树」成本结构与基线重跑成本的复盘讨论）；收口时按惯例补登记。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（一句话证成：纯 launch 侧流程与 reference 成文改动，merge-driver / reconcile / board schema 零触碰，行为面收敛为「少建树、少重跑」；T3 对真值纪律是精确化不是放松）。
> **review profile**：G2（触面 = `.claude/skills/kidpal-pod-launch-prep/SKILL.md` + `.claude/references/pod-coordination.md` + `.claude/agents/integrator.md` + `.claude/skills/kidpal-idf-env/SKILL.md` + `.claude/references/dev-loop-operating-system.md`）+ **spec 面一行连带**（`docs/architecture.md` §5.5.6 开发拓扑记账，纯增记账无行为变更——architect 评审连带核；契约面/G3 脚本面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：GREEN 结论前必须在场（最终期限 FROZEN；判官随 GREEN 结论核对、缺项打回）。

```landing-manifest
.claude/agents/integrator.md
.claude/references/dev-loop-operating-system.md
.claude/references/pod-coordination.md
.claude/skills/kidpal-idf-env/SKILL.md
.claude/skills/kidpal-pod-launch-prep/SKILL.md
docs/architecture.md
```

---

## 0. 想实现的效果与差距

目标效果一句话：Pod worktree 从「每个 TaskPackage 一棵 + 每包一分支」改为「**树池复用 + 一树一常驻分支 + 按需扩容**」——串行包在既有空闲树上、树的常驻分支上续跑（host 冷编译税从每包一次降为每树一生一次，分支仪式归零），是否建新树做并行由 human 在 launch 时按值不值得评估决定；**上游已知红基线从「每次现跑」改为「锚点 + 差异判停」**——拿到的版本与上次量测零差异时引用既有实测，不重跑。

现状差距：launch-prep skill 把「每包 add 新 worktree」固化成无条件步骤，串行波次也被迫付全额环境成本（worktree + 图索引 + **host 冷编译**）；而包与树本无结构性绑定（board 的 worktreePath 是 launch 侧预填的运行时属性）。

非目标（本批不做什么）：不改 merge-driver / portfolio-reconcile / board schema（worktreePath 语义原样）；不引入自动调度或树池管理脚本（建树决策恒 human）；不碰快照流向纪律与 ARQ-260905 图索引规则（每 worktree 专属索引原样成立）；不删不建任何现有 worktree（W1 两棵树按 OQ3 处置）。

## 1. 问题与根因（地面真值）

**问题 1：每包一树 → 每包付一次 host 冷编译，串行波次被迫全额付。**
证据（实测，2026-09-15）：`.gitignore:2` 含 `build/`——`firmware/main/test/host_app/build/` 是 gitignored 编译产物，不随 worktree 走；主 checkout 有上批遗留热 build（`ls firmware/main/test/host_app/build/host_infra_test.elf` 命中，launch-prep 基线轮蹭此热缓存），而新建 Pod worktree 结构性全冷。冷编译耗时 = human 会话实测体感约 30 分钟量级（2026-09-15 launch-prep 后台构建轮观察；未精确计时，如实标注）。根因：build 缓存以 checkout 为单位存在，「树数 = 包数」即「冷编译次数 = 包数」。

**问题 2：launch-prep skill 把「每包一树」写成无条件硬步骤。**
证据（verbatim）：`.claude/skills/kidpal-pod-launch-prep/SKILL.md:56` = `git worktree add C:/AI/KidPal-pod-<suffix> -b dev/<changeId> main`；`:59` = 「`<suffix>` = changeId 去日期前缀（`260829-voice-roundtrip` → `voice-roundtrip`）」；产物清单 `:14` = 「每个包：worktree `C:/AI/KidPal-pod-<suffix>` + branch `dev/<changeId>`」。根因：该 skill 成文于 kidpal-mvp 15 包 WIP=3 高并行年代，把当时实践固化，未区分「隔离需要」（worktree 的本质价值）与「数量策略」（每包一棵只是当时的默认）。

**问题 3（连带，同文件过时项）：skill 的 base ref 措辞与 F-05 裁定漂移（三处，R1-F2 补全）。**
证据（verbatim）：`SKILL.md:56` 尾参 = `main`、`:14` 括注 = 「base = 当前 main」、`:38` = `node .claude/scripts/portfolio-reconcile.mjs --check-launch <changeId> --base-ref main`（launch 机械门实际参数，行为承重）；而现行治理 = 拆包产物 commit 到**专用 base 分支**、worktree 自该 ref 建出（`.claude/agents/portfolio-planner.md` F-05/Step 6 ②；本 workstream 实作 = `base/m1-home-qa`，260915 实跑）。根因：F-05 落地时未回写 skill。

**问题 4：上游已知红基线「每次现跑」的口径缺增量豁免，零差异也重跑。**
证据（verbatim）：`.claude/skills/kidpal-pod-launch-prep/SKILL.md:43` = 「机械来源 = 现跑命令，禁手写散文（存量反例：`260829-story-p3/taskpackage.json` 的 knownRedFloor 手写段）」（260909-gov-review-gate-truthfulness T5）。该纪律只定义了「现跑」与「手写散文」两态，未定义第三态——**带锚引用既有实测**；而 human 2026-09-12 已在 260911-render-pages launch prompt 裁过一次先例（「红基线判读……不重跑，引 PKG-4 合并收口实测 @ main 0f9895d；M1 零 firmware diff、server 面 final r5 唯一红同源，口径引已提交 inventory——KR retro 教训 #1」）——一次性裁定未成文，后续批次无据可依。根因：T5 落地时基线量测成本尚不敏感（热树分钟级），冷树/多波次放大后成为每次 launch 与 Pod intake 的重复成本，且「引用既有实测」与「手写散文」在现行措辞下不可区分。

**支撑事实（复用模型本就成立，只是未成文）**：TaskPackage 与 worktree 无结构绑定——`.claude/agents/portfolio-planner.md:50`：「worktree 路径记录由 **launch 侧预填**：`portfolio-reconcile.mjs --set-pod <changeId> --status queued --worktree-path <abspath>`（脚本参数面；launch 方是唯一知道 worktree 绝对路径的实体）」；merge-driver 经 `pods/<change-id>.json` 的 worktreePath 定位（`.claude/references/pod-coordination.md:86`），只要 board 里路径属实，树是新建还是复用对全部下游机械透明。

**问题 5：每包一分支是纯惯例、非机械依赖，惯例带来分支散播与换包仪式成本。**
证据（verbatim，2026-09-15 亲核）：`.claude/scripts/merge-driver.mjs:1411-1412` = 「// Get the current branch name, then merge it into main」「`const branch = execSync('git rev-parse --abbrev-ref HEAD', { cwd: wt, ... })`」——merge-driver 读 Pod worktree **当前所在分支**并入 main，不按 `dev/<changeId>` 名字定位（`:1413` 仅在「已在 main/master 上」时跳过）；`pre-merge-escalation-gate.mjs` / `portfolio-reconcile.mjs` grep 零命中 `dev/` 分支名（升级闸按 commit 范围判 off-main，分支名无关）。分支惯例的 prose 承载仅两处：`.claude/agents/integrator.md:64` = 「worktree 当前分支亲核为 `dev/<changeId>`……不在本 Pod 分支上 = 停」、`:68` = 「仅在 Pod 分支 `dev/<changeId>` 上执行 `git rebase` / `git merge`」；`.claude/agents/planner.md` 中 `dev/<changeId>` 命中全部是 `delivery/changes/dev/` **路径**（grep 亲核，非分支语义）。根因：`dev/<changeId>` 是「每包一树」年代的配套惯例，树池模型下它的唯一实际功能（隔离上一包残留/废弃半成品）可由更轻的复位规则承担。

**问题 6：launch prompt 承载错位——环境事实与教训留根被逐包搬运，而非住进耐久的家。**
证据（2026-09-15 亲核，K1a prompt 实物拆段）：launch prompt 的段落构成 = 身份/探测声明（planner.md:35 本已支持 changeId 裸起步自探测三处）+ 包事实（taskpackage.json / plan.md / decision-coverage 自带）+ 环境事实（IDF 通路 / WSL 命令 / COM5 / ECS——与 skills `kidpal-idf-env` / `kidpal-serial-monitor` / `kidpal-ecs-gateway` 大面积重复，且 architecture.md:300 已有「WSL CRLF 坑见 kidpal-idf-env skill」的指针先例）+ 基线数字（T3 后家 = progress.md 带锚记录）+ 上批教训留根（家 = 上一包 pod-retro.md）。根因：prompt 成文于全新 workspace 冷启动年代，兼了「耐久知识搬运工」职能；树池模型下 workspace 常驻成熟，搬运纯属重复，且逐包抄写引入过期传染风险（skill「坑」节在案的图索引漏建事故即抄旧文根因）。真正只有 launch 侧知道的 = board worktreePath 重指 + 基线差异判停结果——prompt 应收缩为启动卡（changeId + 基线锚指针 + 上包留根指针）。

**问题 7：架构文档开发视图缺开发拓扑记账。**
证据（verbatim 亲核）：`docs/architecture.md` §5.5.6（`298-302` 行）只记构建工具链、测试四面、日志关联三样；worktree 开发拓扑与固件环境等价三件套（sdkconfig/dependencies.lock 镜像、managed_components 共享软链、worktree-bootstrap）**零承载**，全住 launch-prep skill 操作层。根因：§5.5.6 成文时单 checkout 开发，多 worktree 拓扑后未回写——「怎么造」的 dev 视图缺了「在哪种树上造」这一级事实。

## 2. 改法（T1…T6）

### T1 · kidpal-pod-launch-prep skill 重写 launch 步骤（复用 ∨ 按需新建 + 换包协议）

改 `.claude/skills/kidpal-pod-launch-prep/SKILL.md`：① 产物清单第 3 条与「步骤 3」从「每个包：worktree + branch `dev/<changeId>`」改为「每个包：**运行时 worktree（复用空闲树 ∨ 按需新建）**」；② 新增换包协议四条（与 T2 的 reference 小节同文概要）：树空闲判读（**会话在场即占用**——worktree 由人打开会话占用，一目录一会话，并行同树在操作层不可达；board 的 worktreePath 是记录不是锁）→ **一树一常驻分支**（树建出时定一个常驻分支〔如 `dev/pool-1`，命名自由非门〕，包与包之间不换不删；上一包 **merged** → 直接续跑，上一包 **abandoned** → 先 main→base 同步（**显式前置一步**，R2-N3）再 `git reset --hard <同步后的专用 base 分支>` 复位清半成品再续——复位目标**非 main**：main..base 含未 launch 包的拆包制品，字面复位 main 会把它们从树里抹掉、下一包 intake 即 fail-closed〔R1-F1〕；原「不复用旧包分支」的隔离职能由复位规则承担）→ 交接卫生（tracked 净 + untracked 清、**白名单保留 `build/` 与 `managed_components` 软链**——保热编译与组件缓存）→ 按需扩容（建新树决策归 human，评估并行收益 vs 一棵树的冷编译成本；每树各自常驻分支，git 本禁同分支两树、天然分流）；③ base ref 措辞对齐 F-05——**三处**（R1-F2 补全）：`:14`「base = 当前 main」、`:56` 尾参 `main`、`:38` `--check-launch <changeId> --base-ref main`（行为承重——launch 机械门的实际参数）→ 全部改专用 base 分支；④ 「坑」节补一条：复用树上残留 untracked 的交接卫生教训位；⑤ **launch prompt 收缩为启动卡**——skill §7 重写：prompt 终态 = 数行启动卡，**四要素**（包 changeId + 基线锚指针〔progress.md 带 SHA 行〕+ 上一包 pod-retro 留根指针 + **固件严格模式声明行**〔`VERIFY_FW_BUILD_REQUIRED=1` 默认开、无关闭留痕即开——firmware-build-check.mjs 读进程环境变量、未设即降级〔R1-F3，实测 :108〕，声明载体随 prompt 收缩后归启动卡，不得静默丢〕），身份探测/包事实/环境事实/教训全文一律不再搬运（planner changeId 裸起步自探测〔planner.md:35〕、环境事实归 skills、基线归 progress.md、留根归 pod-retro）；launch 侧动作收敛为三件 = board worktreePath 重指 + 基线差异判停（T3）+ 写启动卡；⑥ **dev-loop-operating-system.md:50 同族措辞对齐**——「每个 ready TaskPackage: git worktree add」改为「复用 ∨ 按需新建」口径（R1-F4，与 SKILL.md 同族的每包一树固化残留）。为什么：直击根因 2/3/5/6；不设独占校验门——占用在操作层自证（human 2026-09-15 裁定，OQ4 撤销）；分支零仪式——merge-driver 读当前分支（merge-driver.mjs:1412），常驻分支零脚本改动。
目标文件：`.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）+ `.claude/references/dev-loop-operating-system.md`（改，⑥ 一行）

### T2 · pod-coordination reference 增「worktree 池复用」小节

改 `.claude/references/pod-coordination.md`：在 Pod worktree 协调面（§1 附近）新增小节「worktree 池复用（换包协议）」：① worktree 与 TaskPackage 无结构绑定（引 portfolio-planner.md:50 launch 侧预填语义）；② 换包协议成文（四条，与 T1 同义）：空闲判读（会话在场即占用、board worktreePath 是记录不是锁）、一树一常驻分支（merged 续跑 / abandoned 复位到**同步后的专用 base 分支**，非 main——R1-F1）、交接卫生白名单、按需扩容决策归 human（评估并行收益 vs 一棵树的冷编译成本）；③ 与既有条款的相容声明：merge-driver 经 pods json worktreePath 定位 + 读当前分支（不认 `dev/<changeId>` 名，问题 5 证据）不受影响、ARQ-260905 每 worktree 专属图索引随树复用自动增量、快照流向不涉 Pod 树不变。为什么：复用协议需要单一事实源，skill 是操作手册、reference 是协调语义 SSOT，两处各管一面不重复正文。
目标文件：`.claude/references/pod-coordination.md`（改）

### T3 · 上游已知红基线的锚点 + 差异判停协议（成文 human 2026-09-12 一次性裁定）

改 `.claude/skills/kidpal-pod-launch-prep/SKILL.md` §2「上游已知红计数」段：① **producer 记锚**——基线记录行（progress.md append 行）在既有「计数 + 命令 + 日期」三要素外增两要素：**量测时 HEAD commit SHA** + **面域**（host 面 = `firmware`、server 面 = `server`，两面分别记锚——一面变另一面可继续引用）；② **consumer 差异判停**——Pod intake / 后续 launch 拿到基线记录后先跑 `git diff --quiet <anchor-sha> HEAD -- <面域>`：零差异 → 引用该 progress.md 行（SHA + 落盘锚 verbatim），**不重跑**；有差异 → 重跑该面（命令同记录）；③ **真值纪律精确化**——「现跑命令，禁手写散文」增补第三态定义：带锚引用 =（量测 SHA + 命令 + 落盘锚）三件齐，缺一即视同手写散文（纪律不放松，只把「重跑」与「带锚引用」并列合法）；④ launch prompt 模板要素（skill §7）同步——**启动卡基线锚行**附 anchor SHA 与面域，Pod 按差异判停（措辞与 T1 ⑤ 启动卡形态统一，R1-N2）。**边界（防误伤）**：本协议只管**上游已知红基线**；本包验收红基线（acceptance-author 步骤 6 的新冻结齿红基线）恒现跑——新测试无缓存可言。为什么：直击根因 4，且把 human 已裁先例成文化，与 T1/T2 同文件同批不扩触面。
目标文件：`.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改，与 T1 同文件）

### T4 · integrator.md 分支亲核措辞对齐（常驻分支口径）

改 `.claude/agents/integrator.md:64/68`：分支亲核从「当前分支亲核为 `dev/<changeId>`」改为「当前分支亲核为**本树常驻分支**（`git rev-parse --abbrev-ref HEAD`，非 main/master 即过——merge-driver 同口径读当前分支）」；`:68` 的「仅在 Pod 分支 `dev/<changeId>` 上执行 `git rebase` / `git merge`」同步改「仅在本树常驻分支上执行」，**「禁在 Pod 分支引入 merge commit」纪律原文保留**（驱动契约不变）。为什么：分支惯例仅此一处 prose 承载行为语义（问题 5 证据），不对齐则 integrator 按旧惯例停在新形态的合法树上。
目标文件：`.claude/agents/integrator.md`（改）

### T5 · kidpal-idf-env skill 补两坑（环境事实归位耐久的家）

改 `.claude/skills/kidpal-idf-env/SKILL.md`：补 2026-09-15 launch-prep 实测两坑——① **`espidf_wsl.sh` 头注释的 `-T main` 示例已失效**（IDF v5.5.1 idf.py 无 `-T` 选项，报 `Error: No such option: -T`；正确用法 = `espidf_wsl.sh build`）；② **git-bash 调 WSL 须 `MSYS_NO_PATHCONV=1`**（否则 `/mnt/c/...` 被 MSYS 改写为 `C:/Program Files/Git/mnt/c/...`，报 No such file or directory）。两坑命令与输出 verbatim 落本 change 目录耐久制品 [evidence-wsl-pitfalls.md](evidence-wsl-pitfalls.md)（R1-F6：证据不再只引会话记录）。为什么：launch prompt 收缩后（T1 ⑤）环境事实的唯一耐久家 = skill，两坑今日实踩、证据在案，不补则下个包重踩。
目标文件：`.claude/skills/kidpal-idf-env/SKILL.md`（改）

### T6 · architecture §5.5.6 开发拓扑记账（一行，spec 面连带）

改 `docs/architecture.md` §5.5.6「构建与测试基建」：构建条目后补一行开发拓扑事实——「开发拓扑：Pod 在本地 git worktree 树池中开发（复用 ∨ 按需扩容，一树一常驻分支）；固件环境等价三件套 = sdkconfig/dependencies.lock 镜像 + managed_components 软链共享主 checkout + worktree-bootstrap；host build 热缓存随树（`build/` gitignored），换包增量编译（操作协议见 kidpal-pod-launch-prep skill）」。为什么：直击根因 7——「怎么造」的 dev 视图补上「在哪种树上造」这一级事实，本批拓扑变更同步记账防文档漂移；纯增记账、无行为变更。
目标文件：`docs/architecture.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| launch 操作手册 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | G2 | 消费方 = human/主线程（launch 执行者）+ Pod intake（T3 基线差异判停）；skill 外的 Pod 行为零改（prompt 载体不变） |
| integrator 分支亲核 | `.claude/agents/integrator.md` | G2 | 消费方 = integrator（closeout 分支核对）；驱动契约（禁 merge commit）原文保留 |
| IDF 环境知识 | `.claude/skills/kidpal-idf-env/SKILL.md` | G2 | 消费方 = 所有跑 WSL 构建的角色（Pod builder/verifier、主线程）；纯增两坑条目 |
| dev-loop 操作系统 | `.claude/references/dev-loop-operating-system.md` | G2 | `:50` 每包 worktree add 同族措辞一行对齐（R1-F4）；消费方 = 治理读者 |
| 架构开发视图 | `docs/architecture.md` | **spec 面连带** | §5.5.6 增一行开发拓扑记账；消费方 = 架构读者；无行为变更（architect 评审连带核） |
| Pod 协调语义 | `.claude/references/pod-coordination.md` | G2 | 消费方 = planner/merge-driver 语义读者；新节纯增、既有节零改 |
| merge-driver / portfolio-reconcile / board schema | — | 未触 | worktreePath 语义原样（引用支撑事实见 §1） |
| portfolio-planner agent def | — | 未触 | 「不 launch、树归 launch 侧」边界原样，复用模型反而更贴其原文 |
| ARQ-260905 图索引规则 | — | 未触 | 规则按 worktree 粒度，复用树的原索引继续增量刷新 |

**【硬项 1b · sweep 透镜】** 本批不落地新门/新断言（换包协议为流程纪律 + 人读核对，非机械门），无判定 pattern 需全仓预跑；UNCLASSIFIED = 0。

## 4. human 取舍点（OQ）

**OQ1 · 复用树命名——已撤销（human 2026-09-15 裁：无绑定关系）**：树名是纯标识符，运行时绑定唯一 = board worktreePath；不做命名规定、既有树名一律不动（中途改名会孤儿化路径派生的图索引项目）、新树命名自由（建议中性，非门）。
**OQ2 · ccache——已拍 (a) 不落（human 2026-09-15 裁「不做」）**：本批不落，留根登记（触发 = 新树冷编译再次成为感知瓶颈或池扩容频次升高）。
**OQ3 · W1 两棵既有树处置——已拍（human 2026-09-15 裁）：W1 即串行 + 复用**：K1a 跑完后，N1b 直接在 K1a 的 worktree 换包跑（首个实战换包即本批新规则的活体试验）；第二棵已备环境的树（KidPal-pod-m1-n1b-p1-home-battery）转为池备件，留待按需扩容用，不拆不改。
**OQ4 · 独占校验——已撤销（human 2026-09-15 裁：并行同树操作层不可达）**：worktree 由人打开会话占用、一目录一会话，「两包指进同一棵树」在操作层不存在触发路径；board worktreePath 定位为**记录不是锁**（merge-driver 定位用），不设任何校验门——与「每包一树」时代同口径（彼时同样无检查、零事故）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. `grep -n 'worktree add' .claude/skills/kidpal-pod-launch-prep/SKILL.md` → 出现处均带「按需新建」条件语义，无条件每包一树形态残留。
2. `grep -nE '当前 main|--base-ref main|-b dev/<changeId> main' .claude/skills/kidpal-pod-launch-prep/SKILL.md` → 0 命中（base ref 措辞三处对齐 F-05 专用 base 分支，含行为承重的 `--base-ref` 参数——R1-F2 加严）。
3. `grep -n 'worktree 池' .claude/references/pod-coordination.md` → 新节在位且含四条协议关键词（空闲判读/常驻分支/交接卫生/按需扩容，R1-F5 词形对齐 v0.3 协议正文）。
4. `grep -n 'anchor-sha\|量测时 HEAD\|差异判停' .claude/skills/kidpal-pod-launch-prep/SKILL.md` → 基线锚点 + 差异判停协议在位；`grep -n '三件齐' 同文件` → 第三态（带锚引用）定义在位。
5. `grep -n 'dev/<changeId>' .claude/agents/integrator.md` → 0 命中（分支亲核改常驻分支口径）；`grep -n '禁在.*分支引入 merge commit' 同文件` → 驱动契约句在位。
6. `grep -n 'MSYS_NO_PATHCONV\|No such option' .claude/skills/kidpal-idf-env/SKILL.md` → 两坑条目在位；`grep -n '\-T main' 同文件` → 仅以「已失效」警示形态出现（非有效示例）。
7. `grep -n '开发拓扑' docs/architecture.md` → §5.5.6 记账行在位且含「树池/环境等价/热缓存随树」关键词。
8. `grep -n '每个 ready TaskPackage' .claude/references/dev-loop-operating-system.md` → 0 命中（:50 同族措辞已被「复用 ∨ 按需」口径替换，R2-N4）。
9. landing 后即时试验（human 2026-09-15 已排期，OQ3）：K1a 跑完合并后，N1b 在 K1a worktree 换包 launch——按新协议真跑一次复用换包 + 常驻分支续跑 + 基线差异判停（预期：host 面 `git diff <K1a-合并后锚>..HEAD -- firmware` 非零 → 重跑；server 面 K1a 零触碰 → 带锚引用不重跑），记录（命令 + 树内状态 + 判停结果）进 `delivery/workstreams/m1-home-qa/progress.md`——冷编译税与基线重跑税消减的实证锚。

## 6. 留根清单

- ccache（若 OQ2=(a)）：登 ARQ 观察——触发 = 新树冷编译再次成为感知瓶颈或池扩容频次升高。
- 独占校验机械化（若 OQ4=(a)）：登 ARQ 留根——触发 = 人读核对出现实际漏检。
- 本 PROPOSAL 无承载 ARQ（human 会话直接触发）——收口 retro 时按惯例评估是否补登记。
- 自律件：树池无管理脚本，池态 = `git worktree list` + board worktreePath 两个地面真值；不做第三份台账。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
