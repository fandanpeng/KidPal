# architect-notes · 260908-gov-review-merge-chain-hardening

> **起草位注记**：architect 评审循环作者阶段候选稿（review-moderator 派发）。只起草候选——不落 G1/G2/G3 canonical、不写冻结 charter、不自评可批准；判据与裁定归 scenario-author / challenger / defender / adjudicator，最终归 human。本文件不进 landing-manifest，是评审输入件。
> 状态：CANDIDATE v0.1 · 2026-09-08。全部行号按当日 main tip `fae886a` 亲核（PROPOSAL 提交本身不改实现面，故与 PROPOSAL v0.2 评审基线一致）。

---

## §T3 · integrator.md「merge-chain execution」节候选正文

### 落位建议

插在 `## Process / 步骤` 之后、`## References / 共享规则` 之前，独立成节（编号沿用现有结构，无 Process 子编号占用冲突）。理由：该节是**条件触发的子流程**（仅 merge-driver abort + blocked 且 planner 派单指名时生效），与 Process 的常规步骤并列放会稀释触发条件；与 Boundaries 并列又会把它变成被动边界而非主动流程。frontmatter（name / tools / model / description）零改动——self-def 申报范围收窄到「正文新节扩执行面」，与 PROPOSAL 头注存疑点①一致。

### 候选正文（整节原样可落）

```markdown
## Merge-Chain Execution / 集成链执行（Pod 分支冲突消解，条件触发）

> 触发条件：merge-driver 因冲突 abort + blocked（含版本化制品 add/add 特判 blocked）后、planner 派单指名你执行消解时本节才生效。本节把「集成期 Pod 分支上的 rebase / 冲突消解」从既有「不 merge」默认禁止中显式豁出并加前置；main 侧合并权不因此转移（铁律 3）。前置不满足时停下回报 planner / 升级 human，不自行补条件、不就地改写。

### 前置（缺一即停）

1. planner 派单制品内**逐文件预登记消解规则**——冲突集里出现无预登记口径的文件 = 停。
2. **human 授权事件 id 在场**（pod-events 内可锚定的 eventId，planner 派单引用之）——id 缺失或锚不到对应事件 = 停。
3. worktree 当前分支亲核为 `dev/<changeId>`（`git rev-parse --abbrev-ref HEAD`）——不在本 Pod 分支上 = 停。

### 边界铁律（逐字）

- 仅在 Pod 分支 `dev/<changeId>` 上执行 `git rebase` / `git merge`（消解重放）；**禁在 Pod 分支引入 merge commit**（驱动契约：merge-driver Step 3 rebase 与 merge commit 结构性不兼容）。
- **永不 push**。
- **永不 merge 进 main**；main 侧合并恒归 planner 主线程 merge-driver 路径（supervisor APPROVE + 前置升级闸放行后），不是你。
- 消解中**不改** `*.acceptance.*`（冻结验收件）、`docs/contracts/**`（恒只读）、`docs/spec.md`——冲突集含这些文件 = 停下升级，消解口径由 planner / human 裁，不就地改写。

### 消解执行（按 pod-coordination.md §10a runbook + §10c 版本化制品规则）

- current/ 投影冲突恒取被合并包侧，且物化后**归一 LF**（`core.autocrlf` 物化 CRLF 会让 sha256 逐字节比对承重闸由绿翻红）。
- 版本化制品（§10c 登记表所列 `*.vN` 槽位）add/add 冲突**禁 `--ours` / `--theirs` 单边消解**——单边会让被丢正文一侧的 needle 齿偶然命中而假绿；消解 = 取并集 + 复跑**双方** carrier 齿并留 exit code。
- graph 缓存冲突调 merge-driver 导出的 `ensureGraphCacheSynced`，不自行复刻三拍。

### 消解完成后（集成树复跑清单，实测数字回报入 review-packet「集成执行回报」）

双方冻结件（本包 + 对端包）+ 全档 pytest + host ELF（文档命令位）+ 设备真编 + 九闸脚本。只跑本包档不算完成——对端红漏检是实测陷阱（消解副作用只在对端冻结齿上显形）。

### fs-write 闸产物收口提交归属

`supervisor-verdict.json` 等机械闸 fs-write 产物，由你以 chore commit 收口进本 Pod 分支（对齐步骤 12 的 feature 分支 commit 职责），不留 unstaged 的 blocking 机器件——merge-driver Step 0 白名单收口是兜底，不是常态路径。收口后 `portfolio-reconcile.mjs --set-pod <id> --status ready-to-merge`，交 planner 重跑 merge-driver。
```

### 起草注记（不入正文）

- 「human 授权事件 id」未钉死五要素形态（`approve-escalated-merge` 那套是合并豁免专用，集成消解授权可能是 `dc9a5c15` 式一次性 grant 或 human-decision 事件）——若评审判需要统一形态，落点在 T4/T5 的机械核验面，不在 integrator 正文；正文只要求「可锚定的 eventId 在场」。
- 「不改 `*.acceptance.*`」的语义边界：冲突消解按预登记口径选边（git 层面取某一侧版本）不等于「改写」；铁律禁的是**编辑正文**（改断言、改 needle）。本注记供判官复核措辞是否需要在正文再显式一句。
- 前置 3 的亲核命令与 dev-loop.settings.json 现有 Bash hook 生态同构（deny-main-push.mjs:58-66 的 getCurrentBranch 先例），不新增工具面。

---

## §T4 · dev-loop.settings.json 条件化 git 条目：形态裁定候选（OQ3）

### 能力面事实（按仓内实况与 Claude Code settings 机制，逐条可核）

1. **settings 权限规则的表达维度只有「工具 + 参数模式」两个**。`permissions.allow / ask / deny` 数组里的 Bash 规则（如 `Bash(git merge:*)`）按**命令串前缀**匹配；规则无条件维度——不能按 cwd、按 worktree、按 agent、按外部状态（如 pod-events 里是否有授权事件）条件化。仓内无 `ask` 数组先例（三份 settings 均只用 `deny`，dev-loop.settings.json:16-21），`permissions.ask` 是 Claude Code settings 既有能力但本仓未用过——首次启用应带 V3 探针实证（PROPOSAL §5 已列）。
2. **命令级条件匹配的真实粒度在 PreToolUse hook 脚本里，不在 settings 层**。matcher 字段只匹配**工具名**（dev-loop.settings.json:24 `"matcher": "Bash"`）；`tool_input.command` 与 cwd 由 hook 从 stdin JSON 读取（enforce-scope.mjs:15-21、deny-main-push.mjs:497-504 先例），之后是任意程序逻辑——deny-main-push.mjs:59-66 甚至会跑 `git rev-parse --abbrev-ref HEAD` 亲核当前分支。即：「Pod worktree 内 + 授权事件在场 → 放行」这类复合条件，settings 层**完全表达不了**，唯一可表达通道 = 新写一个 PreToolUse hook 脚本。
3. **前缀匹配有已知绕道面**：复合命令 `cd <dir> && git merge …` 不命中 `git merge` 前缀。仓内先例的对策是 hook 内分段解析（deny-main-push.mjs:69-125 的 splitCommandSegments）；settings 层的 ask / deny 条目因此天然是 best-effort，不是密不透风层。
4. **merge-driver 自身不受影响**：它由 planner 经 Bash 跑 `node .claude/scripts/merge-driver.mjs …`，其内部 git 操作走 node child_process（merge-driver.mjs:35），不经 Bash 工具权限层——T4 条目不会拦到合并机械链本体。

### 三选一裁定候选

**推荐 (a) ask**，附两条诚实边界；理由与排除项如下。

- **(b) 条件 allow——不可表达（在 settings 层）**。「有预登记消解规则 + human 授权事件引用 → allow，无 → deny」的语义在权限规则层没有承载维度。它唯一可表达形态 = 新增 PreToolUse hook 脚本（读 command + cwd → 亲核分支名 → 读 pod-events 核授权事件 → 放行/拦截）。但那是**新增一道 G3 hook = 事实上的新闸**，超出本批「settings 条件化条目」的字面范围，且 hook 内核验授权事件要么复刻五要素逻辑（与 T6 刚提取的 lib 形成新耦合点）、要么 import 治理 lib（把 dev-loop 平台 hook 与治理 lib 钉死）。**不推荐本批做**；若 ask 摩擦实测不可接受，hook 形态另行立项走 full（等价新设闸，机械硬排除命中）。
- **(c) deny + runbook——可表达但自相矛盾**。deny 会拦死 pod-coordination §10a runbook 的合法消解路径（runbook 第 1 步就是在 Pod worktree 跑 `git rebase <target.ref>`，§10a:124）；integrator / planner 用 Bash 执行时被拦，合法路径被重新推回「human 明确说跳过流程的一次性操作」豁免或仓外操作——与 P1 病根（流程外知识 + 逐次人批）同形，方向倒退。
- **(a) ask——推荐**。`permissions.ask` 补 `Bash(git merge:*)` 与 `Bash(git rebase:*)`：每次消解现场命令必弹人类确认，把「靠分类器放行侥幸」变成「逐次有人在场的成文确认」，与 fail-closed 方向一致；摩擦 = 每次消解多 1-2 次人交互（§10a 重放 + rebase --continue 循环内若各成一条 Bash 命令则逐条弹）——这是真人类检查点，非浪费。

**两条诚实边界（建议原文进 landing-summary / PROPOSAL T4 留根）**：
① 条件语义（worktree 内 / 授权事件在场）机械层表达不了，由 T3 正文纪律承载，ask 只机械兜「每次必有人在场」；
② 前缀匹配可被复合命令绕过，airtight 版本需 hook（留根，另批）。

**候选条目形状**（评审期裁后落）：

```json
"ask": [
  "Bash(git merge:*)",
  "Bash(git rebase:*)"
]
```

---

## §T7 · 版本无关 green-guard 撰写约定：规约落点候选 + 正文

### 落点三选一候选：`.claude/references/acceptance-evidence.md`「必要证据」段

推荐理由（对照三候选）：

1. **消费者对位**：撰写约定的唯一消费者 = acceptance-author 写验收断言的当下。acceptance-evidence.md「必要证据」段已是同类撰写约定集合（「API 验收必须同时断言…」「AI 功能验收必须捕获…」，acceptance-evidence.md:5-11），新增一条「版本化制品接线类守卫必须版本无关」同构同位、同文件「反例」节（:31-39）正好可挂「硬写版本字面」反例闭环。
2. **task-intake-and-slicing.md 排除**：它是 planner 的拆包 / TaskPackage 字段 reference（intake fields / slicing matrix / rigor 字段），acceptance-author 不是其主消费者；断言格式权威落在 intake 文件，撰写者读不到，漂移风险大。
3. **DEV-GOVERNANCE.md 验收测试纪律节排除**：该节（docs/DEV-GOVERNANCE.md:264 起）承载的是**运行态**纪律（host/live 两态、pytest-not-installed 语义、闸执行器行为），与**撰写态**断言格式是两个职责；且 DEV-GOVERNANCE 是 G1，触面成本升级（G1 恒 full + human）无对应增益——G2 reference 足额承载撰写约定，与本仓「G1 只放跨角色指令根」的分层一致。

ARQ 建议改动②（architect-reviewer 预审 / verifier 增列「字面版本耦合」检查判据 = 工程门判据变更）：候选输入 = 先做**告警级 lint 夹具**（「字面版本名出现在新接线类 green-guard 断言里 → warn」，ARQ 便宜验证计划原文），先例 = lint-descriptions.mjs 的 RC-2 告警级模式；gate 化（判红）留根另裁，本批不设新门。是否设门归评审期裁（PROPOSAL T7 已申报）。

### 条款正文候选（整段原样可落入 acceptance-evidence.md「必要证据」段末尾）

```markdown
- 版本化制品（`*.vN` 落盘物，如 `server/deploy/openclaw-system-prompt.vN.md`、engine-patch、overlay 配置）的**接线类 green-guard 必须版本无关**，三步：① 解析当前被接线 / 渲染的那一版文件名——从结构化指向读（渲染面如 `apply-xiaozhi-overlay.sh` 的 `PROMPT_SRC`、声明面如 `provider.config.yaml` 的 `system_prompt.artifact|source|deploy_target`），不从目录猜；② 断言该文件确实落盘（防孤儿）；③ 断言其正文逐字携带本包语义锚点（防未来版丢内容）。**禁在接线类断言里硬写版本字面文件名**：前序包 forward-bump 后字面陈旧，守卫要么假红（断言字面陈旧非行为破损、需 human ruling 才放行），要么假绿（needle 在别的版本正文里偶然命中，丢正文侧静默报绿——schedule-reminder 实测先例）。**落盘 / 措辞类守卫钉死某版合理**（如断言某版基座结构、措辞收紧），撰写时以注释标注类别，与接线类区分。重指母版 = `test_e3_current_prompt_wired_and_carries_char_c1`（human 裁 `c54bbcde`）；清点范围 = 全仓 grep `openclaw-system-prompt.v[0-9]` 于验收断言字面处逐条判「接线守卫（该版本无关化）vs 落盘 / 措辞守卫（钉死某版合理）」。
```

### 既有守卫重指清单表（在册实例逐条，行号按 main tip `fae886a` 亲核）

| # | 实例 | 位置（亲核） | 类别 | 处置候选 |
|---|---|---|---|---|
| 1 | char-stroke CHR-E3 接线齿 `test_e3_current_prompt_wired_and_carries_char_c1` | `server/acceptance/test_char_stroke.acceptance.py:467`（解析 PROMPT_SRC / provider 声明两面得版本再断言，docstring :468-482 载改判缘由） | 接线守卫 | **已重指**（human 裁 `c54bbcde`）——版本无关形态母版，免动 |
| 2 | char-stroke CHR-E3 措辞齿 `test_e3_prompt_v2_landed_with_discipline` | 同文件 :423（`PROMPT_V2_PATH` 读点 :432-435，常量 :144） | 落盘 / 措辞守卫 | 免改——钉 v2 措辞面合理（ARQ 原文「姊妹齿保留」） |
| 3 | schedule_p2 P2-A6 wiring 齿 | `server/acceptance/test_schedule_p2.acceptance.py:584-587`（needle 字面 :585，常量 :144） | 接线守卫 | **现属假绿**——绿由 v2 文件自身 + config 历史句维持、与真接线（已 v3）无关；待重指，触他人冻结件须逐件 human 授权（本批只登记不预批） |
| 4 | voice_roundtrip VRT 落盘齿 `test_e4_system_prompt_versioned_landed` | `server/acceptance/test_voice_roundtrip_b.acceptance.py:378`（`SYSTEM_PROMPT_PATH` 常量 :116 钉 `.v1`） | 落盘 / 措辞守卫 | 免改——钉 v1 基座结构合理 |
| 5 | voice_roundtrip VRT 接线齿 `test_e4_system_prompt_wired_into_deploy_overlay` | 同文件 :410（needle = `SYSTEM_PROMPT_NAME` 裸名 :117，corpus 只读 deploy 无扩展名文件含 `OPENCLAW_SYSTEM_PROMPT_CARRIER`） | 接线守卫（天然版本无关） | **免改——反例母版**（ARQ 原文「裸名 + 无扩展名 carrier」即此齿） |
| 6 | voice_roundtrip VRT live 探针（A4 / A7） | 同文件 :509-512（读 `SYSTEM_PROMPT_PATH` = `.v1`；live 齿类 :423 起） | live 接线（所测 .v1 ≠ 生产 v3） | 待重指——carrier 声明文件已注记该断裂；无 live env 重指无法实证（ARQ 原文），随本 ARQ 落地时一并走版本无关重指 + 核 `PROMPT_V1_PATH`/`PROMPT_V2_PATH` 常量语义与 `OPENCLAW_SYSTEM_PROMPT_CARRIER` 口径统一 |
| 7 | char-stroke CHR-A8/A9 live 探针 | `server/acceptance/test_char_stroke.acceptance.py:683-685`（`test_a8_live_block_matches_makemehanzi`，def :678）与 :722-724（`test_a9_live_char_latency_5s_band`，def :718）；常量 :144 | live 接线（所测 .v2 ≠ 生产 v3） | 待重指——同 #6 语义（ARQ 原行号 :618/:657 为 story-p3 评审时点值，落地后行移，PROPOSAL 已随 §8 修订注记改） |
| 8 | schedule_p2 `PROMPT_CARRIER_V2` 常量 | `server/acceptance/test_schedule_p2.acceptance.py:144` | 常量（被 #3 消费） | 随 #3 重指一并处置 |

清单外防误伤：`test_schedule_reminder.acceptance.py:294` `PROMPT_CARRIER_V3` 与 :1416 的 RMD-A8 齿是本批 P2 假绿问题的**受害侧**（其 v3 字面合理——它是当前接线版的事实锚点，且已由同包新齿 `test_a8_prompt_carrier_v3_reminder_section_anchors` 补闭假绿，carrier 声明文件注记在案），不在重指清单内。

---

## §review-bg · 候选关注点 + canary 线索（供 scenario-author 起 charter 参考）

### 评审关注面（每条注明来源机制行号）

1. **T1-4「双源统一」的对照系失真**：ARQ 证据文本称「对照常规路径 Step 1b（:1137-1143）cwd 与参数同用 wt，故无此形状」——亲核不符：Step 1b 实际 = `execFileSync('node', gateArgs, { cwd: wt })` 且 gateArgs 仍传 `--project-dir projectDir`（merge-driver.mjs:1136-1142），与 `approvalRefUnlock`（:915-921）**同形**（cwd = worktree、参数 = 主仓）。真正的差异点只有一个：`approvalRefUnlock` 的 `findWorktree(tp, projectDir) || projectDir` 回退（:919）——worktree 解析失败时 cwd 落主仓，`--source-ref HEAD` 求值成 main 的 HEAD → off-main BLOCK（gate :154-156）——方向 fail-closed 无损但恒空转。评审应裁：T1-4 若「统一为同一解析主体」只改 approvalRefUnlock，会与常规路径形成两形；统一口径需先裁 gate 的「git 在 cwd（worktree）求值、文件读在 --project-dir（主仓）求值」哪些是有意语义（建议判据：source-ref 必须在 Pod worktree 求值、verdict/PROPOSAL 读主仓——现状两路径均已如此，缺口仅在回退分支）。
2. **T1-1 add/add 特判的 widen 方向**：特判（`*.v[0-9]+.md` + §10c 登记表）只应 widen **指引文案**、不得 widen **放行**——漏拦形态（登记表外的版本化物，如 engine-patch 目录无 `.vN` 后缀形态）须退回通用 blocked fail-closed。建议 charter 钉一条：特判命中集与放行集的补集关系不变式。
3. **T3 self-def 申报复核**（PROPOSAL 头注存疑点①）：integrator.md frontmatter 零改动、正文新节实质扩执行面（把 merge 禁令豁出一角）——是否触 lightweight 硬排除的「agent def 正文新节」判据，判官显式裁并归档。
4. **T4 ask 的绕道面诚实性**：settings 前缀规则可被复合命令（`cd x && git merge …`）绕过（§T4 事实 3）；PROPOSAL T4 的「形态若不可表达则降级」条款应随裁断值回填为两条边界注记（条件语义承载在 T3 正文、airtight 需 hook 留根），不得只在 landing-summary 落。
5. **T5 新核验行的解析面**：`Kind=hard-violation` + Basis 引 `docs/contracts/**` / `docs/spec.md`（或 severity ≥ SHOULD 的 contract/boundary 类）——Findings 六列是 review-packet prose 约定（integrator.md:42），非结构化字段；机械核验对自由文本 Basis 的匹配策略（前缀 / 正则）决定误报漏报面，charter 应钉「两态夹具 + 未匹配即放行的方向声明」（放行为默认、只拦可识别形态 = 向严方向的反例，须显式裁）。
6. **T6 行为等价的可证面**：gate 判定语义零变更声明靠 self-check 双路径复跑钉住；但 spawn→import 消除的「以 merge-driver argv 误判 changeId」语义耦合（ARQ-260907-gate-lib-script-separation 证据原文）本身无可证夹具（旧形态是 bug 非 spec 行为）——等价断言范围应明确排除该语义差。

### regression-canary 候选（绝不能变的既有行为）

- **merge-driver 五自检全绿**：`--self-check-retro-advisory / --self-check-quick-checks / --self-check-detect-changes / --self-check-rebase-target / --self-check-worktree-topology / --self-check-merge-friction`（入口 :1329-1349）——夹具名单与断言语义逐字不变，T1 新增四夹具只增不改。
- **投影特判 theirs 语义**：current/ 投影冲突恒取被合并包侧（:853 注释「rebase 语境 theirs = 被重放的 Pod 侧」）——T1-2 LF 归一只能加在 checkout 之后，不得改变取侧方向。
- **Step 0 fail-closed**：白名单外变更在场即 warn + 保持 fail（:813-815），绝不吞非治理面变更。
- **approvalRefUnlock 核验失败 → kept-blocked**（:926-929）不得变 unlocked；核验成功才 setPod ready-to-merge（:930）。
- **gate 例外清单六条判定语义零变**（pre-merge-escalation-gate.mjs 头注 :10-19 + evaluate :136-200）：T5 只新增一条核验行；尤其 `--human-approval-ref` 仅适用 devloop 档（:377-380）不得扩到 gov 档，`hasContractGateEvidence: false` 的向严默认（:249）不动。
- **gate self-check 既有 case 全数通过**（:257-341，含 T1 人批豁免 8 例）——case 名单只增不减。
- **settings 现有 deny 面不变**：dev-loop.settings.json `permissions.deny` 两条契约面 Edit/Write deny（:17-20）与五组 hook 挂点（:22-68）结构不动，T4 只增 `ask` 数组。
- **deny-main-push / enforce-scope 自检不变**（各自的 --self-check 输出零变化）——T4 不碰 hook 层。
