# Adjudications · 260908-gov-review-merge-chain-hardening

> adjudicator（中立判官）裁定记录 · 2026-09-08。逐条：CONFIRMED / DISMISSED / DEADLOCK + 一句证据 + outcome。全部行号按本会话当日亲核复核（main 实现面与 charter 基线 `fae886a` 等价）。A-3 事实裁与其余可机判关键证据（F1/F2/N1/N2/N3/N4/N5/F11）由 adjudicator 独立亲核源码确认，非采信双方自述。

## 一、R1 findings（F1–F12）

| # | 裁定 | 证据（一句） | outcome |
|---|---|---|---|
| F1 | CONFIRMED | adjudicator 亲核 `.claude/scripts/lib/governance-gate-shared.mjs` 已存在 518 行（lib 目录在册），PROPOSAL :99/:116 误标「新增」与 :97「既有先例同名 lib」同段自相矛盾——landing 即撞名 | FIX（批内回灌）：修向终裁 = **并入既有 lib**（追加命名导出、既有导出与函数体零改动，lib 头注 B5「computeFrozenPin 纯新增 + isFrozen 零改动」先例在案）；改名会造双治理 lib 双源、违反 GA-6 单源纪律，不采。PROPOSAL :97/:99/:116 措辞改「扩展既有 lib」；等价夹具名册按 N2 六改七 |
| F2 | CONFIRMED | adjudicator 亲跑 `grep -n "docs/contracts" .claude/dev-loop.settings.json` 命中 3 行（:7 _doc、:18-19 deny 条目字面），V4 负断言按字面永假 | FIX（批内回灌）：V4 改写采纳 defender 文案 + challenger R2 B-Ⅱ 转义修正——`grep -cF 'Edit(./docs/contracts/**)'` = 1 且 `grep -cF 'Write(./docs/contracts/**)'` = 1（deny 两行逐字在位）+ ask 数组逐字白名单两形（`Bash(git merge:*)` / `Bash(git rebase:*)` 各 -cF = 1）+ permissions 段无指向 docs/contracts 的 allow 条目。比「零命中」机证力更强且方向可过 |
| F3 | CONFIRMED | V5（PROPOSAL :140）明令 landing 时结移 `docs/ARCH-REVIEW-QUEUE.md` 而该文件不在 manifest 六路径（:12-19）；T7 目标文件行（:105）唯一 canonical 锚经 ANCHORABLE_TEST 入 §2 并集、compareAnchorSets 双向差必报失配（challenger R2 B-Ⅲ 形状亲核闭合） | FIX（批内回灌）：FROZEN 前 manifest 补 `docs/ARCH-REVIEW-QUEUE.md` 行；A-5 裁后 T7 落点路径一并补。不采「条件」注记粉饰（fail-loud 方向） |
| F4 | CONFIRMED | PROPOSAL :57 残留 `## 2. 改法（T1…T6）` 与 :6 packet-binding「T1–T7」两个口径 | FIX（批内回灌）：标题改 T1…T7，一处即净 |
| F5 | CONFIRMED | gate off-main 判定 :150-158 亲核在案：cwd 落主仓 → HEAD=main → `source-on-main` → off-main BLOCK 恒命中；PROPOSAL :64 候选自违 charter canary 10 | FIX（批内回灌）：删除「或两者恒用 projectDir」或原位标注已被 canary 10 + off-main 语义先验排除 |
| F6 | CONFIRMED | V1-③「cwd ≡ --project-dir 解析主体」预设 A-3 统一侧；A-3 事实层支持「维持两形、只改回退分支」侧，断言文字按裁断不可实现 | FIX（A-3 裁后回填）：V1-③ 条件化——按本批 A-3 裁断改写为「approvalRefUnlock 与 Step 1b 同形（source-ref 在 worktree cwd 求值、文件读走 --project-dir 主仓）+ 回退分支消除后 worktree 解析失败显式 errorExit 而非静默落主仓」 |
| F7 | CONFIRMED | T3 前置 1/2 的生产侧义务（planner 派单格式）manifest 与留根清单（:143-150）两处皆无落点，charter S-2 明文「哪怕落点在留根清单也要有」 | FIX（留根）：留根清单增行「planner 派单格式义务：派单制品必填字段（逐文件消解规则预登记格式 + human 授权 eventId 引用形态 + 停等超时收口人），归下一批 planner.md 修订或首批 T3 演练实测后承载」。defender 补辩的「停 + 回报响应与结构性 forcing 在场」（architect-notes :19「不自行补条件、不就地改写」）一并记档，不抵消生产侧义务缺口 |
| F8 | CONFIRMED | charter GA-10② 冻结判据要求等价断言显式排除 changeId 误判语义差，PROPOSAL V2（:137）未落；且 defender 修向文案被 N1 证伪 | FIX（批内回灌）：V2 补显式排除句 + 按 N1 修正后文案落入口不变量（import 探针主断言，见 N1） |
| F9 | CONFIRMED（清单本体）/ DISMISSED（严重性定性） | census 亲核属实：server/deploy 下正则不命中的版本化物 ≥5 类（engine-patch/、openclaw-patch/、OPENCLAW_SYSTEM_PROMPT_CARRIER、provider.config.yaml、apply-xiaozhi-overlay.sh）；PROPOSAL :71 同形态风险注记在册非无视 | FIX（批内回灌）：T2-1 初始清单扩实测超集（五类逐条入册）+ V1-① 补「engine-patch 形态 add/add → 通用 blocked（非特判、非放行）」反例夹具。「S-1 假绿链原样复发」定性 DISMISSED——engine-patch 形态落通用 blocked = fail-closed 与批前同水平不恶化，且 §10a + T3 已成文禁单边/取并集/复跑双方，残余风险低于批前 |
| F10 | CONFIRMED | PROPOSAL :72「补三条」实列 ①②③④ 四条且 T3（:79）按四条序引用 | FIX（批内回灌）：「补三条」改「补四条」，一处即净 |
| F11 | CONFIRMED（零影响） | adjudicator 亲核：`.claude/hooks/enforce-scope.mjs` 无 self-check 入口（grep 零命中）、`.claude/hooks/deny-main-push.mjs` 有——charter canary 11 措辞失配属实，对 PROPOSAL 零修向 | Charter erratum：交 moderator 注记归档（canary 11 按「文件零 diff」执行），不入 decision-packet 修向 |
| F12 | CONFIRMED | T7「撰写时注释标注类别」（:103）格式未钉，VRT-E4 误伤防线全押自由散文注释 | FIX（随 A-6 一并裁）：A-6 裁定时同钉最小标注格式 `<!-- guard-class: wiring | landing | wording -->`（正则可判、人类可读、不与 prose 冲突）；本批不做 lint 则格式仍先钉（撰写当下即用，防自由散文漂移） |

## 二、B 系断链判定（按 R2 撤回/维持终态）

| # | 终态裁定 | 证据（一句） | outcome |
|---|---|---|---|
| B-0 | CONFIRMED（半断链） | 三处引用锚在场但无防漂移对账点——「半句同文案」靠 FROZEN 时刻对齐，后续批改 runbook 不核 driver | FIX（批内回灌）：T2 §10c 条款加一行互指维护注记「本节为版本化制品消解口径单一权威；merge-driver 特判指引文案与 integrator.md 消解段为引用方，改本节口径须同批核两引用」 |
| B-1 | DISMISSED（「真断链」定性）——R2 A-1 撤回成立 | charter P-4 自身把留空点钉为「launch 面无**机械**消费方」（消费方声明 T2-1 在案：owner=planner / 触发=launch / 让位规则=vN+1 起编）；兜底链 fail-closed 可机证（撞槽 → T1-1 特判 blocked + 显式指引，失败态 board 机械可见）；§9 毕业先例逐字在案（pod-coordination.md:104-116） | DISMISS-WITH-GROUNDING + 附随改法：留根清单增行「versioned-artifact-claim 机械消费挂点（portfolio-reconcile `--check-claim` 形态候选）按 §9 毕业条件观测后批」——该行须真落留根清单，否则本 DISMISS 作废。挂点取舍本体入 human 拍板清单（GA-8 域） |
| B-2 | CONFIRMED | 「随批维护」四字未名 owner / 更新时机 / vN 回收，charter GA-2② 点名，引不出原文反驳 | FIX（批内回灌）：T2-1 补三短句——owner = 批主线程（触碰登记表所列形态的批 landing 时随批更新）；更新触发 = 批版本化制品路径集与登记表 diff 非空；vN 回收 = 不做机械回收、槽位由 forward-bump 语义由后版覆盖（实测需要再另行立项） |
| B-3 | CONFIRMED（= F7） | 同 F7 | FIX（留根）：同 F7 修向 |
| B-4 | DISMISSED（「真断链」定性）——R2 A-2 撤回成立 | charter A-8 原文自设闭合路径：「若裁放行，须同时裁『写作侧对齐约定』（B-4）作为补偿控制」——「裁定项依赖顺序」不是「成文缺口」 | DISMISS-WITH-GROUNDING + packet 义务：本 decision-packet A-8 项内同场裁写作侧对齐约定（已履约，见 A-8）；T5 落地时 gate 核验行注释钉 Basis 匹配形态 + V2 两态夹具对账 |
| B-5 | DISMISSED（主体）——R2 A-3 撤回成立 | 执行者身份三处在场：sweep 表 :112 消费方列「planner / integrator / human 消解现场」逐字在；pod-coordination.md:70 planner 主线程现场在案；integrator 分支触发条件 architect-notes :19 钉死；ask 弹窗接收方 = 交互会话 human 操作者（平台机制事实） | DISMISS-WITH-GROUNDING + 残留 CONFIRMED（N6，COULD）：ask 批准判据一行随 T4/A-4 回填进 dev-loop.settings.json `_doc` 数组：「ask 弹窗批准判据：核 human 授权 eventId 在场且可锚到 pod-events 对应事件」 |
| B-6 | CONFIRMED | defer eventId 锚格式未定义：gate 现仅核 human-decision / approve-escalated-merge 五要素，defer/accepted-risk decision 值形态缺位，T5 无从实现 | FIX（批内回灌）：T5 补最小形态声明——defer / accepted-risk 记录 = pod-events `human-decision` 事件，payload.decision ∈ {defer-contract-change, accept-risk}（枚举值随 human 批 A-8 时确认），核验复用 resolveHumanApproval 五要素、decision 值参数化为上列 |
| B-7 | CONFIRMED（半断链） | architect 已自我指出 hook↔lib 耦合（architect-notes :70），PROPOSAL 留根未登记 | FIX（随 A-4 回填）：A-4 回填时 hook 留根行内并入一句「另批 hook 若核授权事件，import governance-gate-shared.mjs（与 gate / merge-driver 同源）或复刻五要素——形态另批裁，耦合声明随该批 landing 归档」 |
| B-8 | CONFIRMED（队列化挂载部分）/ DISMISSED（「无主工作项」定性） | 重指流程先例完整在案：CHR-E3 首例重指 = human 裁 `c54bbcde`、记录落点 = pod-events human-decision + commit、发起 = human 逐件授权（charter P7 已钉前提）——「无主」定性不成立；触发时机与队列化未挂钩属实 | FIX（批内回灌）：T7③ 落地时（或 V5 结移动 ARCH-REVIEW-QUEUE 的同一动作里）增一条排队行引用重指清单，行内注明「发起 = 下一批触碰对应冻结件时（对齐 P7）或 human 专项批」——零新增文件 |

## 三、R2 N 系（级联派生，随裁）

| # | 裁定 | 证据（一句） | outcome |
|---|---|---|---|
| N1 | CONFIRMED | adjudicator 亲核 lib 五处 `process.exit`（:199/:231/:261/:334/:517，均在 run*SelfCheck 体内）、`process.argv` 全文件零命中——defender F8 修向「不 process.exit」按字面今天即假，与 F2 同病族 | FIX（批内回灌）：V2/F8 修向文案按 defender R2 终稿落——import 探针主断言（`node -e "import('<lib>').then(m => { if (typeof m.resolveHumanApproval !== 'function') process.exit(2); }).catch(() => process.exit(3))"` 须 exit 0，同时机证顶层零副作用 + 提取导出可调）+ `grep -c "process.argv" <lib>` = 0 辅断言；弃「不 process.exit」全称负断言（函数体归属需 AST、grep 不可判，写成断言即下一个 F2） |
| N2 | CONFIRMED | adjudicator 亲核 portfolio-reconcile.mjs:34 具名导入 isProposalMdPath / normalizeSourceRef / resolveOriginRef、:329 `--self-check` 入口在——消费方 7 非 6，双方 R1 grep 均漏计 | FIX（批内回灌）：F1 等价夹具名册六改七，portfolio-reconcile `--self-check` 全量复跑零回退列入 V2 配套 |
| N3 | CONFIRMED | adjudicator 亲核 pod-coordination.md:126 逐字 `GIT_EDITOR=true git rebase --continue`（defender 行号订正 :127→:126 成立）——非复合命令的单条逃逸，砍在消解主循环最高频位，ask 保护主张 day 1 在主循环位不成立 | FIX（A-4 裁后批内回灌）：归 A-4/GA-8 同裁——defender 调整后立场「条件保留 ask、首选修 runbook 命令形态」采纳为本批建议（详见 decision-packet A-4） |
| N4 | CONFIRMED | adjudicator 亲核：实际符号 `test_a8_live_block_matches_makemeahanzi`（test_char_stroke.acceptance.py:22/:172/:678 三处一致），PROPOSAL :160 与 architect-notes :117 均写作 `makemehanzi`——按名 grep 清点必漏命中 | FIX（批内回灌）：两处符号逐字订正为 makemeahanzi |
| N5 | CONFIRMED（分叉成立 + defender 选项 2 采纳） | adjudicator 亲核：approvalRefUnlock 调用点唯一在 merge-driver 入口 :1368-1370（`SINGLE_POD && HUMAN_APPROVAL_REF && !DRY_RUN` 守卫）、解锁后同 invocation 内 ready 集收集 → processPod（:1117）→ Step 1b（:1136-1150）**无条件再 spawn 全闸**（gateArgs 含 --human-approval-ref）任一违例 BLOCK + setPod blocked + return false——merge 级 fail-closed 由 Step 1b 背书，「fail-open」定性过重成立；但 PROPOSAL :97 未申报解锁核验集收窄、V2 夹具与 F8 排除项均不覆盖，分叉须显式裁 | FIX（N5 三选项排序裁断 = **选项 2：收窄 + Step 1b 背书补偿断言**）：① T6 条款补申报句「解锁预核收窄为五要素事件核验，全闸由 Step 1b 同 invocation 背书（merge-driver :1132-1150）」；② V2 增背书夹具「unlock 收窄形态下，off-main / provenance 违例 Pod 解锁后仍在同 invocation 被 Step 1b BLOCK 并回落 blocked」；③ canary 4 kept-blocked 字面与意图不动（:927 起 catch 块亲核在案）。弃选项 1（保留 spawn 则「消双跑」名不副实、T6 对 merge-driver 侧收益归零、argv 语义耦合 wart 原样）；弃选项 3（全闸六条 + 豁免全部共享化 = 提取面扩到 evaluate 全域 + ctx 装配，等价夹具负担 ×6、批内重构风险上升，成本收益倒挂）。ready-to-merge 瞬态自纠于 :1148 回落 blocked，board 状态属信息面，不构成拒收理由 |

## 四、A 系裁定项（charter §3 + 派生）

裁断正文见 decision-packet.md；此处只记裁定结论与归属。

- **A-1（存疑点① self-def 判据）**：裁「不触 lightweight 硬排除判据字面，但属 agent def 执行语义变更、恒 human-approved」——判据先例归档，入 human 拍板清单。
- **A-2（存疑点② T5 定性）**：裁「既有前置升级闸门内加项」（折进注范式），非新增 merge gate、非第 7 工程门；T4 ask 同理是 settings 权限层条目非新门。agent 层归档，随批改法吸收。
- **A-3（Step 1b 同形 + 修复口径）**：事实层亲核确认同形成立（ARQ 对照系失真确认）；设计层裁**维持两形、只改豁免路径回退分支**（回退改显式 errorExit）。建议即地板，入 human 拍板清单。
- **A-4（OQ3 + GA-8 + N3）**：裁建议 = ask 条件保留 + 先修 runbook 命令形态互洽。强制层 settings + GA-8 张力，human 拍板。
- **A-5（T7 落点三选一）**：复判采纳 architect 荐 acceptance-evidence.md「必要证据」段（:5-11）+ 反例节（:31-39）挂反例；被排除两落点**不需要**指回引用。入 human 拍板清单（落点选择确认）。
- **A-6（字面耦合检查是否设门）**：裁本批不设新门（与 GA-4/A-2 一致），告警级 lint 夹具 + gate 化留根另裁；guard-class 标注格式随裁钉死。入 human 拍板清单（闸判据域）。
- **A-7（双源关系）**：裁登记表为 authority + 正则为形态兜底；登记表初始清单 = 实测超集（F9 修向）；不一致时赢家 = 登记表，发现通道 = V1-① 反例夹具 + B-2 diff 触发。agent 层归档，随批改法吸收（authority 语义随 human 整包批确认）。
- **A-8（T5 未识别形态方向 + B-4 义务）**：裁**放行（只拦可识别形态）**为默认方向 + 同场裁写作侧对齐约定作为补偿控制（B-4 闭合义务本 packet 履约）。闸判据域，human 拍板。

## 五、deadlock 独立复核

双方声称无 deadlock，adjudicator 独立复核**确认无 deadlock**：R1 defender 13 条 findings 中 10 条痛快 CONCEDE 均附独立亲核实锤；F9 各半（清单 CONCEDE、定性 REBUT 有据）；F11 零影响 erratum。B 系三条 REBUT（B-1/B-4/B-5）经 challenger R2 定向复攻后以可检验证据撤回（charter 原文逐字闭合、探针矩阵、三处在场），非各说各话。N5 在双方共享 Step 1b 背书事实后收敛为三选项排序，N3 在 challenger 探针后 defender 调整立场并给出明确修向——收敛态均为「共享证据后的选项排序」，属 adjudicator 裁断域，不构成「满预算仍争、正反各有据僵住」。

## 六、计数

- F 系 12 条：CONFIRMED 11（其中 F9 拆分：清单 CONFIRMED / 定性 DISMISSED；F11 零影响）+ F9 定性部分 DISMISSED = 全部有终态。
- B 系 9 条：CONFIRMED 5（B-0/B-2/B-3/B-6/B-7）+ DISMISSED 3（B-1/B-4/B-5，均附条件改法或 packet 义务）+ B-8 拆分 CONFIRMED（队列化）/ DISMISSED（定性）。
- N 系 5 条 + N6 残留：CONFIRMED 6。
- A 系 8 项：全部裁定（结论见上节）。
- DEADLOCK：0。
