# PROPOSAL · 260924-gov-review-settings-deny-contract-merge

> **状态：FROZEN v0.4 · 2026-09-24 · 内容冻结，凭据齐备——主线程凭「立项 ack + GREEN」自冻，授权记本行**。**凭据段**：① human 立项 ack = 2026-09-24 本会话（前端对齐「为什么改/往哪改/改动多大」三段呈报后裁定「立项 ack，进评审循环」）；② OQ 裁定（同场，AskUserQuestion 回执）：**OQ-1 = (a) 删两行**（裁定项原文含「删除属机械强制层变更，本裁定原文即批准凭据」——本行即 settings 族两行删除的 human 批准凭据，classifier 拦截先例的批准闭环）；**OQ-2 = (a) 静态 lint+步骤接线**；**OQ-3 = (a) ②runbook 改道**——三项均按推荐位裁定，圈定范围与 v0.1 manifest 一致、零变化；③ architect GREEN = §7 R3（2026-09-24，同判官三轮收敛 R1 BLOCK(F1–F6) → R2 BLOCK(R2-F1) → R3 GREEN，findings 零新增、未触熔断、升级 human 零项；verdict 行首机锚 `verdict: GREEN` 在 §7，Track B 资格判官独立裁成立）。**B6 FROZEN 时点实录（兑现 v0.3 承诺措辞）**：`git status --porcelain` = 3 个 M（`delivery/changes/arch/INDEX.md`〔upsert 过程制品〕+ 两条 ARQ 追注）+ `.codebase-memory` 常驻脏 2 件 + 本批目录 untracked；`.claude/**` 零 diff 维持（HEAD `77000d9`）。**packet-bound landing 基准 = 本文件 §2 改法集 T1–T4 + §5 验证集（landing-verifier 比对基准 = v0.4 §2；T3-iii 两围栏块内容行即磁盘 byte 基准——block0 == merge-driver:1276 已机证、block1 为合法 JS 插入行照落零改动）。**
> **版本沿革**：v0.1 起草（零脚手架、§5 基线全量实跑、preflight 绿）→ v0.2 立项 ack + OQ 裁定记录（manifest 零变化；各 T 内「OQ 裁 (b)/(c)/(d) 时」条件分支自此失效、仅作选项空间记录供判官核）→ v0.3 = R1 findings 修订（F1 T3-iii 锚文与插入文本改围栏代码块承载、去反斜杠+反引号伪影〔`\n` 序列未连带改动〕· F2 merge-driver 触发条件收紧为通用 1 reason 前缀逐字、通用 4 不触发论证入文 · F3 §5 B6 补认第 3 个 M〔changes/arch/INDEX.md，upsert 过程制品〕· F4 settings-lint `--self-check` 输出契约钉死〔夹具 bad-/good- 前缀 + 尾行单行 JSON〕+ V2 改固定 token 机械断言 · F5 V1 增 review-loop `_doc` 机械齿 · F6 交互式断言两处校准至实证面〔T1 问题段 + T2-ii 插入文本；OQ-1(b) 代价段为已裁分支历史记录、按判官「随修与否主线程裁」口径保留原文不修〕· N1 §10e 步骤 2/3 增 K7 实走偏差限定句 · N2 T1-ii review-loop 括注澄清「句尾不指 lint」· N3 V3 四步关键词补全〔基线五词均 0 亲测〕；N4/N5 记录级不修，判官已注 landing-verifier trim 口径与观察项）→ **v0.4 = R2 finding 修订**（R2-F1：§10e 步骤 2 括注「K7 实走未落独立批准事件」为假——K7 pod-events 实测在场 human-decision 批准事件 `c6464758-eac0-4719-aa40-531ac4526961`（decision=approve-escalated-merge），merge-completed 代记 payload 逐字引其 human-approval-ref〔主线程亲核 grep 三命中〕；按判官示例句替换为「K7 实走系复用 approve-escalated-merge 事件作批准锚 + 本节首立契约包人合专用 decision 值、系成文收敛」。单点修订，未波及 §10e 其余文本与 V3 五词断言）。全部修订落 PROPOSAL 文本面，零脚手架态不变（`.claude/**` 零 diff 维持）。
> **承载 ARQ**：`ARQ-260924-settings-write-deny-rule-cli-drift`（T1+T2 全承载；条目内 human 终裁「不删……留个 gov 去处理」的移交裁量面 = 本批 OQ-1/OQ-2）· `ARQ-260923-contract-merge-deadpath`（T3 全承载；条目二选一处方 = 本批 OQ-3，其 trigger 行「与 dispatch-prompt-hard-terms 并批」线索已过时——该条已由批 A T3 结项，起草期追注在案）。两条目起草期实测追注均已 append（2026-09-24，工作树未 commit）。
> **review 轨**：Track B 常驻轻路径（SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。**轨道自评（写实，不自我美化）**：两件均为执行实测出的 bug（ARQ source 在案：260924-perf-k1 launch 实测 / loop-retro-2026-09-23 K7 实证）= Track B 快道资格（不论 G 几）；不触 `docs/contracts/**` 内容（T3 改的是闸对契约面 diff 的**处置输出**，契约文本零触碰）、非跨系统结构性变更 ⇒ 不升 Track A。**gate 事实显式标注交判官独立裁**：T3 触 merge 门载体脚本本体（`pre-merge-escalation-gate.mjs` / `merge-driver.mjs`，命中 Track A Eligibility「改 merge gate」机检硬排除的形态描述）——改法形态 = BLOCK 输出文本加 runbook 指针，判定语义零变更（BLOCK 仍恒 BLOCK，向严不减；self-check 33 例基线全绿、landing 后复跑仍全绿为机械证明），且 OQ-3 裁定为 human 批准凭据；存疑向严，若判官认为须升 Track A 即升。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（证成：T3 判定语义零变更 + T1 语义零损失〔Edit 行承载冻结语义、scope-map/钩子双拦不变〕+ 全批可整批回滚；不取 low 因触 merge 门脚本本体与 settings 机械强制层两个 G3 敏感面，且 T2 新增一个脚本）。
> **review profile**：G2+G3（G3 触面枚举：`.claude/dev-loop.settings.json` · `.claude/review-loop.settings.json`〔settings 族〕· `.claude/scripts/pre-merge-escalation-gate.mjs` · `.claude/scripts/merge-driver.mjs`〔门/driver 脚本〕· `.claude/scripts/settings-lint.mjs`〔新增机检脚本〕；G2 触面枚举：`.claude/skills/kidpal-pod-launch-prep/SKILL.md` · `.claude/references/pod-coordination.md`）+ 账本写回面（`docs/crq/gov/**`，非 G 层分类面）。**G1 触点为零、契约面内容触点为零、hooks 零触碰、scope-map.json 零触碰、verify-gates.json 零触碰（新 lint 不入闸族）、agent defs 零触碰、frontmatter 零变更**。OQ-1/OQ-2/OQ-3 已全裁 (a)（v0.2 凭据段），触面自此确定、无待裁分支。
>
> **零脚手架声明（诚实声明；与批 A 脚手架形态相反，系纪律要求）**：human 终裁附带纪律「处置拍板前主线程与 Pod 对 `.claude/**`（settings 族）零触碰」+ 机械强制层不得 agent 自批 ⇒ 本批起草期对工作树 **`.claude/**` 零写入**，T1/T2/T3 全部改动只在 OQ 裁定（= 批准凭据）+ architect GREEN + FROZEN 之后的 landing 相位落工作树。起草期实测全部为只读（含各脚本 `--self-check` 只读跑）。§5 起草期实跑记录如实标注「基线态（零脚手架）」。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。**manifest 形态记录**：v0.1 即推荐位形态（11 行）；OQ 三项均裁 (a)（= 推荐位），v0.2 manifest **零变化**，各 T 内「裁 (b)/(c)/(d) 时」条件分支自此失效、仅作选项空间记录供判官核对 OQ 给全性。

```landing-manifest
.claude/dev-loop.settings.json
.claude/review-loop.settings.json
.claude/scripts/settings-lint.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/references/pod-coordination.md
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/merge-driver.mjs
docs/crq/gov/_archive/2026/ARQ-260924-settings-write-deny-rule-cli-drift.md
docs/crq/gov/_archive/2026/ARQ-260923-contract-merge-deadpath.md
docs/crq/gov/INDEX.md
docs/crq/gov/_archive/2026/INDEX.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：机械强制层的两处「执行实测出的死件」就地消解——① settings 族里对现行 CLI 零执行力、却能把 headless launch 面炸成 exit 4 的两行无效 deny 规则按其处置裁定归位（删除或留档+机检兜底），launch 面不再潜伏版本漂移雷；② 契约面 diff 的合并死路径（升级闸恒 BLOCK、无任何成文通道、每个契约包都要现场即兴一次人合）获得成文处置（runbook 改道或证据接入），下一个契约包的合并者读 BLOCK 输出即得完整路径，不再靠翻 retro 与临场发明。

**现状差距**：① 两行 `Write(./docs/contracts/**)` 无效规则仍在两 settings 文件（:25 / :14），headless 形态已实测硬阻塞（exit 4，260924 01:28 在案），human 终裁移交本批处置但处置本身未落；② `hasContractGateEvidence` 仍硬编码 `false`（闸脚本自注「后续接入」），K7（260923）实走的人合操作（pre-merge 预检 + ff-only + pod-events 代记簿记）零成文，merge-driver 与升级闸的 BLOCK 输出均无任何指引。

**非目标（本批不做，防 scope creep）**：
- 不触 `docs/contracts/**` 内容一字；不触 G1 指令根；不回头改批 A 已落的 planner 编排文面。
- 不动升级闸对契约面 diff 的**判定语义**——「契约面人签字制不动、恒 BLOCK 向严」在两 OQ-3 选项下均保持（(a) 只加输出指针与 runbook；(b) 接证据也是「证据在场才放行」的向严形态，且 (b) 未被裁定前不预设计）。
- 不恢复 headless launch（human 终裁「launch 一律交互式」在本批落地后是否放宽归 human 另裁，不在本批）。
- 不新增 verify-gates 闸、不动 hooks、不动 scope-map；T2 的 lint 是 launch-prep 步骤面机检，不入合并闸族。
- 不处置 `ARQ-260924-landing-summary-template-pointer-gap`（批 A 收口新立，另批承载；本批 landing-summary 依纪律复制模板即天然规避其病灶）。

## 2. 问题、根因与改法（T1…T4）

**【硬项 · 起草期实证义务】** 本节断言纪律照模板执行；本批全部机械事实断言已于 2026-09-24 起草期只读实测（命令 + 关键输出逐字记录于各 T 证据段与 §5），无实测的写「未实测」。**行号坐标系**：全文 file:line 引用一律以 2026-09-24 起草期坐标系为准（HEAD `77000d9` + 工作树两条 ARQ 追注；`.claude/**` 零脚手架故脚本/settings 行号 == HEAD 行号）；landing 后行号随插入漂移，后续读者按文本字节内容定位。**换行形态事实**：六个 `.claude/**` 目标文件实测均为 CRLF（`grep -qU $'\r'` 逐件），landing 改动保持 CRLF 不混入 LF 行。

### T1 · settings 族两行无效 `Write(./docs/contracts/**)` deny 规则处置（OQ-1 裁定承载）

**问题**：两 settings 文件的 `permissions.deny` 各含一行 `Write(./docs/contracts/**)`，现行 CLI（2.1.268）判定其为无效形态（文件权限检查只认 `Edit(path)` 规则，Edit 规则已覆盖所有文件编辑工具）——该行**零执行力**，却使 headless（`-p` + `--dangerously-skip-permissions`）launch 硬失败 exit 4（260924-perf-k1 实测在案）；且 CLI auto-update 常开（doctor 实测），「交互式容忍」是版本行为不是保证，无效行留着即 launch 面潜伏雷。发生频率：headless（`-p` + skip-permissions）形态每次必炸（exit 4，原始证据在案）；报错打印发生在 settings 加载期——`-p` 无旗标与非 TTY 两形态当日冒烟实证打印，真交互 TTY 形态未测（两可如实记，R1-F6 校准，见 §6 留根 1）。

**证据**（起草期只读实测，2026-09-24，全量记录见两条目追注与 §5）：
- 病灶在场：`grep` 全仓 `"Write(` 恰好命中 2 行——dev-loop.settings.json:25 + review-loop.settings.json:14（第三面无命中；`.claude/settings.local.json` 不存在）。
- CLI 报错逐字（当日两次冒烟复现打印）：「Permission deny rule (.claude\dev-loop.settings.json): Write(./docs/contracts/**) is not matched by file permission checks — only Edit(path) rules are. Use Edit(./docs/contracts/**) instead (Edit rules cover all file-editing tools).」
- exit 4 事实以 2026-09-24 01:28 原始证据为准（C:/tmp/pod-perf-k1-launch.log，235B，ARQ 行内逐字）；当日复现该形态被 auto-mode classifier 拦截（嵌套 skip-permissions 禁跑，拦截正当、不绕行）。当日 `-p` 无旗标形态：同款报错打印后挂起 ≥150s（timeout 杀，exit 124）——**报错发生在 settings 加载期、打印与形态无关；硬失败目前只在带 skip-permissions 的 -p 形态有实证**。
- 入仓时点：`git log -S "Write(./docs/contracts"` → `9d62625`（2026-08-27 档 1 迁移）/ `4e9d776`（2026-08-29 档 2/3 迁移），长期潜伏坐实。
- 活机检消费者为零：scripts/hooks/scope-map/verify-gates/route-regression 均无断言该两行在场的活门（起草期 grep 实测；260908 批 §5 V4 的 `grep -cF 'Write(./docs/contracts/**)'=1` 系历史验证集断言、非活门、不回跑——删除后该历史断言按字面为假，属历史制品常态，如实记录不处置）。
- 双拦保全事实：契约冻结的机械承载 = deny 段 `Edit(./docs/contracts/**)` 行（CLI 口径覆盖所有文件编辑工具）+ scope-map `universal.bashDenyWrite` + enforce-scope 钩子 per-agent deny——T1 只动 Write 行，三件承载零触碰。

**根因**：规则形态自源仓迁移时即带（8/27–8/29），当时 CLI 或容忍或未校验；CLI 校验口径变化后该形态成无效规则，但无任何机检面能在 launch 前发现它（doctor 不检出，起草期实测「No installation issues found」）——无效规则静默潜伏至首次 headless launch 才爆。

**改法**（**OQ-1=(a) 删除形态，推荐位；裁 (b)/(c) 时本 T 改为零改动/白名单形态，v0.2 重写**）：两文件各做两处改动——

**T1-i deny 段删无效行**。承重 landing 指令——两文件的 deny 块现文完全同形（dev-loop :23–26 / review-loop :12–15，逐字机械提取）：

```json
    "deny": [
      "Edit(./docs/contracts/**)",
      "Write(./docs/contracts/**)"
    ]
```

各替换为（删 Write 行 + 去 Edit 行尾逗号；JSON 语法合法性入 §5 V1 机判）：

```json
    "deny": [
      "Edit(./docs/contracts/**)"
    ]
```

**T1-ii `_doc` 防再加注记**。dev-loop.settings.json `_doc` 数组内锚定行（逐字，现文件 :9）：

> "deny 用 settings.deny 硬冻（Claude Code 内核层，钩子盖不过）+ enforce-scope per-agent deny 双保险。",

在该行之后插入新 `_doc` 行（数组元素形态，含尾逗号）：

> "契约冻结的 settings.deny 承载只用 Edit(path) 形态——Write(path) 形态被 CLI 判无效且硬阻塞 headless launch（exit 4，ARQ-260924-settings-write-deny-rule-cli-drift 实证，260924-gov-review-settings-deny-contract-merge T1 删除）；勿再加回，settings-lint.mjs 对该形态报 RED。",

review-loop.settings.json `_doc` 数组内锚定行（逐字，现文件 :6）：

> "deny 把契约面 docs/contracts/** 冻结为只读（触契约走 architect-reviewer + 人类契约门）；",

在该行之后插入新 `_doc` 行（同上形态；该文件无 settings-lint 消费步骤注记需求，句尾**不**指 lint——以下方给出的插入行文本为准，括注与文本歧义已消，R1-N2）：

> "契约冻结 deny 只用 Edit(path) 形态——Write(path) 被 CLI 判无效（ARQ-260924 实证，260924-gov-review-settings-deny-contract-merge T1 删除），勿再加回；",

**为什么这样改**：对根因「无效形态潜伏 + 无发现面」直接移除病灶（全 launch 形态归零）+ 文件内注记防好心再加回（与 T2 lint 互为表里）；删除的语义零损失由三件承载保全（Edit 行 + bashDenyWrite + enforce-scope），human OQ-1 裁定原文记入 Status 行即机械强制层变更的批准凭据。**OQ-1=(b) 时替代形态**：两 settings 零改动，T2 lint 增 byte-exact 白名单恰放行该两行、新 `Write(` 规则仍 RED；**OQ-1=(c) 时**：两 settings 零改动、无白名单（lint 对该两行报 WARN 台账态，v0.2 按裁定细化）。

目标文件：`.claude/dev-loop.settings.json`（改，OQ-1=(a) 前提；裁 (b)/(c) 时零改动、本行从 manifest 裁剪）· `.claude/review-loop.settings.json`（改，同前）

### T2 · settings 合法性 dry-run 机检（OQ-2 裁定承载；ARQ 建议② 勘误后措辞的落地）

**问题**：settings 族的规则形态合法性在 launch 前无任何机械发现面——CLI 自带 doctor 不检出（起草期实测），live 冒烟形态脆（当日实测：`-p` 无旗标挂起 ≥150s；带 skip-permissions 形态 agent 侧被 classifier 禁跑）；CLI auto-update 常开使「今天合法的形态」可随版本静默失效。下一次同类漂移仍会潜伏至 launch 现场才爆。

**证据**：ARQ-260924 建议②（勘误后措辞「headless launch 面 settings 合法性 dry-run 机检」）；doctor 输出逐字「No installation issues found」（本 worktree 现跑，同机 settings 病灶在场）；`claude doctor` 帮助文本自述「Reads settings files in the current directory」但不校验 permissions 规则形态合法性（实测行为）；live 冒烟三形态实测记录（§2 T1 证据段）。

**根因**：settings 合法性校验只存在于 CLI launch 时刻（且各 launch 形态行为不一），仓内无独立的、launch 前可跑的静态校验件。

**改法**（**OQ-2=(a) 静态 lint 形态，推荐位；裁 (b)/(c)/(d) 时 v0.2 重写**）：

**T2-i 新脚本 `.claude/scripts/settings-lint.mjs`**（设计规格，landing 相位按此实现）：
- 扫描面 = settings 族机械枚举：`.claude/settings.json` + `.claude/settings.local.json`（在场才扫）+ `.claude/*.settings.json`（glob）；不扫 worktree 副本（launch-prep 在主 checkout 跑，Pod worktree 的 settings 随分支内容同源）。
- 判定：`permissions.{allow,ask,deny}` 数组内规则串以 `Write(` 起头 → **RED**（已实证无效形态，exit 1）；以 `Read(` 起头 → **WARN**（同族推断、未单独实证，advisory 不改退出码）；JSON 解析失败 → RED。其余规则形态不判（不发明 CLI 未声明的口径）。
- 白名单（仅 OQ-1=(b) 裁定时启用）：byte-exact 匹配 `"Write(./docs/contracts/**)"` 于两已知文件 → 降为 WARN 台账行；任何第三处或变形 → 仍 RED。
- `--self-check`：内嵌夹具，**判别性义务**（fixtures-must-discriminate）——含 `Write(` 规则的坏夹具必须被检出为 RED、纯 `Edit(` 好夹具必须放行；断言先验证夹具非空（真空绿规约）；反转实现（删检出逻辑）必使 self-check 红。**输出契约（R1-F4 钉，与 gate self-check 同形）**：夹具命名钉 `bad-` / `good-` 前缀；逐例输出行含夹具名 + ok 标记；尾行单行 JSON `{"selfCheck":"settings-lint","pass":<n>,"fail":<m>,"total":<n+m>,"ok":<bool>}`；任一例红 exit 1。
- 输出：逐文件逐命中行（file:line + 规则串逐字 + 判级），尾行汇总；exit 0=clean / 1=RED / 2=用法错。

**T2-ii launch-prep 步骤接线**。承重 landing 指令——锚定 `.claude/skills/kidpal-pod-launch-prep/SKILL.md` 步骤 2 内原文（逐字，现文件 :46）：

> blocking 非空即停——回 portfolio-planner 补 commit，不绕过。

在该行之后插入新段（同步骤 2 内、既有「上游已知红」段之前）：

> **settings 合法性机检（260924-gov-review-settings-deny-contract-merge T2）**：`node .claude/scripts/settings-lint.mjs`——扫 settings 族 permissions 规则的已知无效形态（`Write(path)` 文件权限规则被现行 CLI 判非法：零执行力且硬阻塞 headless launch exit 4，ARQ-260924 实证）。RED 即停、先修 settings 再走后续步骤（判停口径与 check-launch blocking 同）；CLI auto-update 常开，本机检是版本漂移的 launch 面前置防线。本机检在 launch 前静态校验 settings 文件内容，不依赖 CLI 各 launch 形态的行为差异，启动卡命令行①②两形态均覆盖（CLI 侧报错打印发生在 settings 加载期——2026-09-24 冒烟实证于 `-p` 无旗标与非 TTY 两形态；真交互 TTY 形态未测，见 ARQ-260924 追注）。

**为什么这样改**：静态 lint 对根因补「launch 前独立发现面」，确定性、秒级、零 API、agent 可代跑（live 冒烟三形态实测皆不满足此三件）；只编码已实证口径（Write→RED）+ 同族推断降级（Read→WARN），不发明 CLI 未声明的校验、防 lint 自身成为假红源。接线落 launch-prep 步骤 2（机械门步骤）使每包 launch 面天然过检，不新增闸族。

目标文件：`.claude/scripts/settings-lint.mjs`（新增，OQ-2=(a)/(c) 前提；裁 (b)/(d) 时不建、本行从 manifest 裁剪）· `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改，OQ-2=(a)/(b)/(c) 前提；裁 (d) 时零改动、本行裁剪）

### T3 · 契约面 merge 死路径消解（OQ-3 裁定承载；ARQ 建议② runbook 改道形态）

**问题**：升级闸对触 `docs/contracts/**` 的包恒 BLOCK（`hasContractGateEvidence` 硬编码 `false`，源码自注「后续接入」）——BLOCK 本身是设计（契约面人签字制、向严），但 BLOCK 之后**无任何成文通道**：merge-driver Step 1b 只 warn + setPod blocked，闸输出只有「升级 human，不得自动 merge」，人合的固定步骤（预检 / 批准事件落账 / ff 选定 / 簿记等价代记）零成文。每个契约包合并时都要现场重新发明一次（K7 260923 首例即兴实走，簿记靠自觉代记）；且绕开 merge-driver 的 Step 5b 机械簿记链，漏记无闸可拦。发生频率：每契约包一次（低频、每次都是即兴操作）。

**证据**（起草期只读实测，2026-09-24）：
- 硬编码两处 + 消费点：`pre-merge-escalation-gate.mjs:366`（live buildCtx，逐字 `hasContractGateEvidence: false, // 契约门证据（contract-change-packet）后续接入；现 false → 契约面 diff 恒 BLOCK（向严）`）+ `:393`（self-check mkCtx 夹具默认）；消费点 `:263`（通用 1 `ctx.contractsDiff && !ctx.hasContractGateEvidence` → BLOCK reason `:264`）。
- merge-driver 现行为：Step 1b（`merge-driver.mjs:1250–1282`）gate BLOCK → `warn`（:1276，输出闸尾 12 行）→ `setPod blocked` → `return false`；全文 grep 无任何契约包人合指引。
- K7 实走自记（ARQ 条目引 pod-events merge-completed 事件）：「非 merge-driver：升级闸对契约面 diff 恒 BLOCK（hasContractGateEvidence 占位未接入），human 直接指示合并」。
- 孪生先例：`ARQ-260902-escalate-merge-no-human-path`（closed 2026-09-05）——ESCALATE 人批曾同为死路径，260904 批建 `--human-approval-ref` 五要素机械证据口消解；本条是契约面侧的孪生缺口（ESCALATE 有通道、契约 BLOCK 无通道）。
- 事件形态先例：pod-events `eventType` 为机械闭集（`pod-events-append.mjs:32` 含 `human-decision`），`payload.decision` 为自由串有先例（`anchor3-deviation-ruling`，pod-coordination §11）——runbook 的批准事件不扩闭集。

**根因**：升级闸 v1 把契约门证据口留作占位（向严恒 BLOCK 正确），但「BLOCK 之后人类怎么走」的 runbook 面从未立——机械路径缺位时人合路径也无条文，处置知识只活在 K7 当场的会话记忆里。

**改法**（**OQ-3=(a) runbook 改道形态，推荐位；裁 (b) 证据接入时本 T 整体替换、v0.2 重写**）：三处改动，判定语义零变更——

**T3-i runbook 条文**。承重 landing 指令——锚定 `.claude/references/pod-coordination.md` 原文（逐字，现文件 :196）：

> ## 11. 锚 3 · demo 对账与 roadmap 达成状态（260912-gov-review-portfolio-decomposition-chain T4，FROZEN v1.0）

在该行之前插入新节（全文如下；节号 §10e 与既有 §10a–§10d 同族）：

> ## 10e. 契约包人合 runbook（260924-gov-review-settings-deny-contract-merge T3；K7 260923 即兴实走的成文化）
>
> 适用面：diff 触 `docs/contracts/**` 的任何包（dev Pod 档与 Track B 治理档同）。升级闸通用 1 对此类包**恒 BLOCK 且无机械放行口**（`hasContractGateEvidence` 证据位未接入，向严设计、非故障）——契约面人签字制不动，人合 = 唯一合法通道，固定四步、任一步缺失即不合并：
>
> 1. **pre-merge 预检**：merge-to-main skill 步骤 1–2 同形——`git diff --stat <target>...<source>` 列全改动面 + 相交面核对（零重叠，或重叠仅 §10c 登记表形态）；确认契约面改动带 human 契约门凭据（ContractChangePacket / CDQ 对应行 / 该批 PROPOSAL 的契约门批准记录，任一在案）。
> 2. **human 批准事件落账**：dev Pod 档 = pod-events append `human-decision`（`payload.decision="approve-contract-merge-manual"`、actor=human，五要素与 `approve-escalated-merge` 同形；eventType 闭集不扩，decision 值沿 `anchor3-deviation-ruling` 自由串先例——该 decision 值为本节首立口径：K7 实走系复用 `approve-escalated-merge` human 批准事件作批准锚（eventId `c6464758`，merge-completed 代记 payload 引其 human-approval-ref），本节为契约包人合首立专用 decision 值，系成文收敛，R1-N1/R2-F1）；Track B 治理档（无 pod-events）= human 批准原文记入该批 PROPOSAL Status 凭据段 + architect-landing-summary（OQ 裁定凭据同口径）。
> 3. **ff 与 no-ff 选定**：Track B 治理批恒 `git merge --ff-only`（arch-review-protocol「Track B 七步流程」步骤 7 同口径，非 ff 即停交 human）；dev Pod 通道 `--no-ff` 双亲合并以证合入事实（merge-driver Step 5 同形），由主线程代 merge-driver 执行；remote push 恒 human。（K7 实走的 dev Pod 包用了 `--ff-only`，系成文前的即兴偏差——本节按既有 canonical 口径收敛：`--no-ff` 双亲合并仅用于 dev Pod 通道、ff-only 仅限 Track B 治理批〔protocol 步骤 7 逐字口径〕，R1-N1 限定句。）
> 4. **Step 5b 簿记等价代记**（merge-driver 机械簿记的手动等价，三件缺一不可）：pod-events append `merge-completed`（payload 注记「非 merge-driver：契约包人合（§10e），批准锚 <eventId>」）+ `node .claude/scripts/changes-index.mjs --upsert <changeId>` + merge commit message 注明人合与批准事件锚。
>
> **边界与留根**：本节不改任何闸判定——升级闸对契约面 diff 仍恒 BLOCK（向严不减）；机械放行口（升级闸接契约门证据，与 `--human-approval-ref` 同构）为留根项（ARQ-260923-contract-merge-deadpath 建议①），触发 = 契约包频率上升或本 runbook 实走摩擦 ≥1 次实测，届时另批立项。

**T3-ii 升级闸 BLOCK reason 加指针**。承重 landing 指令——锚定 `pre-merge-escalation-gate.mjs` 原文（逐字，现文件 :264）：

> `    push('docs/contracts/** diff 且同批无契约门证据（契约面人签字制不动）');`

整行替换为（判定触发条件与 BLOCK 结果零变更，只扩 reason 文本；既有 self-check `contracts-block` 例断言 `reason.includes('docs/contracts')`，替换后仍命中）：

> `    push('docs/contracts/** diff 且同批无契约门证据（契约面人签字制不动）——契约包人合 runbook：.claude/references/pod-coordination.md §10e（预检/批准事件落账/ff 选定/簿记等价代记四步）');`

**T3-iii merge-driver Step 1b 条件指针**。承重 landing 指令——锚定 `merge-driver.mjs` 原文（逐字，现文件 :1276；**围栏代码块承载**——内嵌反引号须 byte 保真，blockquote+反斜杠转义形态会产伪影，R1-F1 修法）：

```js
      warn(`${changeId}: escalation gate BLOCK:\n${out.split('\n').slice(-12).join('\n')}`);
```

在该行之后插入一行（**触发条件 R1-F2 收紧** = 通用 1 reason 前缀逐字 `docs/contracts/** diff 且同批无契约门证据`——T3-ii 替换后该前缀保留〔替换文本以此前缀起句，兼容自证〕；:275 通用 4 contract-findings-unaccounted 的 reason 字面列表虽含 `docs/contracts/**|docs/spec.md|…` 子串但不含该前缀句式，不触发本指针——该 BLOCK 自有 CDQ 落账/human defer 锚的机械核销口，不受误导；「非通用 1 的 BLOCK 零噪声」按字面成立）。插入行全文（围栏承载，同 F1 口径）：

```js
      if (out.includes('docs/contracts/** diff 且同批无契约门证据')) warn(`${changeId}: 契约面 BLOCK——无机械放行口，人合走 .claude/references/pod-coordination.md §10e 契约包人合 runbook（四步：预检/批准事件落账/ff 选定/簿记等价代记）`);
```

**为什么这样改**：对根因「BLOCK 后无成文通道」把 K7 已验证走通的人合操作固化成四步条文（runbook 宿主 = pod-coordination §10 族，与 §10a 冲突消解 runbook 同族同位）+ 两处 BLOCK 输出机械携带指针（发现面：下一个合并者读闸输出即得路径，不靠翻 retro）；判定语义零变更故不触「向严」基线，证据接入（建议①）留根并给出量化触发器。

目标文件：`.claude/references/pod-coordination.md`（改，§10e 新节插入）· `.claude/scripts/pre-merge-escalation-gate.mjs`（改，reason 行整行替换）· `.claude/scripts/merge-driver.mjs`（改，Step 1b 插一行）

### T4 · 账本写回与状态转移（landing 相位执行，不在起草期做）

**问题**：本批承载 2 条 ARQ，landing 后账本须如实反映处置结果，否则账实漂移（账本 SSOT 纪律：改状态去改条目文件再重跑渲染）。

**证据**：`docs/crq/gov/INDEX.md:10` 现摘要行逐字（起草期现跑）「**摘要**：未结 18（待拍板 0 / 待触发 16 / 已接受风险 2）；已结 68（归档索引见下）。」；两条目 frontmatter `status: deferred` 在场；crq-doctor 基线 exit 0「条目 104 条：gov=86 tech=17 requirement=1 / advisory 142 条（不阻断）/ RED 零命中 —— ok」。

**根因**：（簿记义务，无病灶。）

**改法**（landing 相位、landing-verifier 核验前完成）：
1. **2 条全承载结项**：各条目正文 append 处置段（本批 changeId + T 编号 + OQ 裁定原文摘录 + landing 凭据），frontmatter `status: closed` + `closed: 2026-09-24`（实际 landing 日）+ `closedBy: 260924-gov-review-settings-deny-contract-merge T1+T2（OQ-1/OQ-2 裁定 <原文摘录>）` / `…T3（OQ-3 裁定 <原文摘录>）`；由 `crq-render.mjs` 自动归位 `_archive/2026/`（人不搬文件）。ARQ-260923 条目 trigger 行的过时并批线索已由起草期追注澄清，结项段不重复展开。
2. 重跑 `node .claude/scripts/crq-render.mjs`（INDEX 再生 + 归档归位）；`node .claude/scripts/crq-doctor.mjs` 复核 RED 零命中。
3. 状态转移各跑 `node .claude/scripts/changes-index.mjs --upsert 260924-gov-review-settings-deny-contract-merge` 钉行 `delivery/changes/arch/INDEX.md`（过程制品，不入 manifest）；合并 main 后再跑一次翻 mergedInMain（memory 纪律）。

**为什么这样改**：账本写回是 Track B 制品链义务；结项判定对照 §2 承载面不虚报（OQ-3=(a) 时 ARQ-260923 的建议①以留根形态记入处置段与 §6，条目仍结——处置完成 = 二选一已裁且已落，留根另有量化触发器）。

目标文件：`docs/crq/gov/_archive/2026/ARQ-260924-settings-write-deny-rule-cli-drift.md`（改后归档迁入）· `docs/crq/gov/_archive/2026/ARQ-260923-contract-merge-deadpath.md`（改后归档迁入）· `docs/crq/gov/INDEX.md`（渲染再生）· `docs/crq/gov/_archive/2026/INDEX.md`（渲染再生）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| settings 族 | `.claude/dev-loop.settings.json` | G3 改（条件：OQ-1=(a)） | T1 deny 段 + _doc 注记；消费方 = dev-loop Pod launch（CLI settings 加载）、enforce-scope 不读本文件 deny（读 scope-map） |
| settings 族 | `.claude/review-loop.settings.json` | G3 改（条件：OQ-1=(a)） | T1 同形；消费方 = review loop 会话 launch |
| settings 族 | `.claude/settings.json` | 未触 | 常驻设置无 deny 段、无 Write 形态规则（起草期 census 命中 0） |
| merge 门脚本 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 改（条件：OQ-3=(a)） | T3-ii 仅 reason 文本；判定语义零变更（self-check 33 例前后全绿为机械证明）；消费方 = merge-driver Step 1b + Track B 步骤 7 主线程直跑 |
| merge 编排脚本 | `.claude/scripts/merge-driver.mjs` | G3 改（条件：OQ-3=(a)） | T3-iii 仅条件 warn 一行；控制流零变更；消费方 = Pod 合并流 |
| 新机检脚本 | `.claude/scripts/settings-lint.mjs` | G3 新增（条件：OQ-2=(a)/(c)） | T2-i；只读扫描、不入 verify-gates 闸族、不进任何既有闸链；消费方 = launch-prep 步骤 2 + 手动 |
| launch 手册 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | G2 改（条件：OQ-2≠(d)） | T2-ii 步骤 2 内插一段；frontmatter description 零变更；消费方 = launch-prep 执行者（主线程） |
| 协调 reference | `.claude/references/pod-coordination.md` | G2 改（条件：OQ-3=(a)） | T3-i §10e 新节；消费方 = 契约包合并者（主线程/planner）、闸输出指针的落点 |
| 账本 | `docs/crq/gov/**`（2 迁 + 2 INDEX 再生） | 账本写回面 | T4；消费方 = crq-render / crq-doctor / 全仓 ARQ 引用 |
| lib 单源 | `.claude/scripts/lib/governance-gate-shared.mjs` | 未触 | OQ-3=(a) 不触证据核验函数；仅裁 (b) 时入 v0.2 触面 |
| G1 指令根 | AGENTS.md / CLAUDE.md / DEV-GOVERNANCE.md / README.md | 未触 | 零改动 |
| 契约面 | `docs/contracts/**` | 未触 | 零改动（恒只读） |
| hooks / scope-map / verify-gates / agent defs / templates | 各面 | 未触 | 零改动；SKILL.md 启动卡五要素文本不动（T2 是步骤面不是卡面） |
| 过程制品 | `delivery/changes/arch/260924-gov-review-settings-deny-contract-merge/**` | 新增 | 本批四件套，不入 landing-manifest（过程面非 canonical 改法集） |

**【硬项 · sweep 透镜义务】**：本批落地一个新机检断言面（T2 settings-lint，OQ-2=(a)/(c) 前提）——其判定 pattern 的全仓预跑已于起草期完成：`grep -rn '"Write(' `（全仓）→ 恰 2 命中（两 settings 文件 :25/:14，即 T1 处置对象本身）；`grep -rn '"Read(' .claude/*.json` → **0 命中**（WARN 级现面无存量）；settings 族 permissions 规则条数 census（`grep -c` 五形态前缀）→ settings.json=18 / dev-loop=4 / review-loop=2。原始输出与命令见 §5 起草期实跑记录。判官可 spot-check 复跑。

**【硬项 · 复扫模式完备性先验】**：本批涉残留清扫面 = settings 族无效规则形态。模式集显式枚举：`"Write(` × `"Read(`（带引号规则串前缀形态）× 文件面 { settings.json, settings.local.json（不存在，机械 ls 证）, *.settings.json（glob 恰 2 件）}——完备性论证：CLI 校验对象 = permissions 数组内规则串，其文本形态必以工具名前缀起头；文件权限类工具中 Edit 为合法形态、Write/Read 为待检形态，Bash/mcp 前缀不属文件权限检查口径（报错文本自证「only Edit(path) rules」限文件权限族）。全仓 `"Write(` 复扫另覆盖 settings 族之外的误放形态（命中仅账本/制品文本引用，非规则位）。

## 4. human 取舍点（OQ）

> **裁定记录（2026-09-24，human 立项 ack 同场，AskUserQuestion 回执）**：OQ-1 = **(a) 删两行**；OQ-2 = **(a) 静态 lint+步骤接线**；OQ-3 = **(a) ②runbook 改道**。三项均按推荐位裁定，圈定范围与 v0.1 草案一致，manifest 零变化；各 OQ 的「代价」段自此为裁定知情面。OQ-1 裁定原文含批准凭据性质（机械强制层变更不得 agent 自批——本裁定即 human 批准，记 Status 行凭据段）。

### OQ-1 · settings 族两行无效 `Write(./docs/contracts/**)` deny 规则删与否

**背景**：human 终裁（2026-09-24 晨）「不删，我们不走 headless 不就行了，这个留个 gov 去处理」——两行保留至今、逐字 patch 撤回，裁量面（删与否）显式移交本批。起草期只读实测补充的新事实：① 报错发生在 settings 加载期，`-p` 与非 `-p` 非 TTY 形态当日均打印同款报错（交互式真 TTY 形态在本环境不可测，K7 是否带 `--settings` 无记录、两可维持）；② 硬失败 exit 4 目前只在 `-p` + skip-permissions 形态有实证；③ `claude doctor` 不检出该病灶；④ CLI auto-update 常开（doctor 实测 enabled / channel latest）——「交互式容忍」是版本行为，未来版本可静默变化；⑤ 该两行在现行 CLI 口径下零执行力（报错文本自证），契约冻结语义全部由 Edit 行 + scope-map bashDenyWrite + enforce-scope 钩子承载；⑥ 活机检消费者为零（260908 批历史验证集断言除外，非活门）。
**代价与为什么该 human 定**：删除属机械强制层变更——classifier 拦截先例确立「移除 deny 规则须 human 本人提出」，agent 不得自批；且此决定细化/调整 human 前一终裁的方向，裁量权只能归 human。本 OQ 裁定原文将记入 Status 行凭据段作为批准凭据。
- **(a) 删两行（Recommended）**——理由：病灶从所有 launch 形态中根除，语义零损失（三件承载保全已论证），文件内 _doc 注记 + T2 lint 双防再加回。代价：若未来 CLI 恢复匹配 Write 形态规则，该行已不在（但 Edit 行仍覆盖同路径，防御面无实际减损）；260908 批历史验证集 V4 断言按字面变假（历史制品不回跑，记录即可）。
- **(b) 保留不删 + lint 白名单兜底**——尊重前终裁「不删」方向；T2 lint 对 byte-exact 的该两行降级 WARN 台账、新 Write 规则仍 RED。代价：headless 需求若恢复仍被硬阻塞（须届时再裁）；每次 launch 打印报错横幅（噪声侵蚀「报错=真问题」的信任）；交互式容忍度未被实证、auto-update 漂移风险持续存在。
- **(c) 保留且不设白名单**——最省事（settings 零改动）；但 T2 lint 若立则恒 RED 两行、机检失去判别力，实际与 (b) 相比省一行白名单配置而废掉整个机检，不建议组合出现（若裁 (c) 建议同时 OQ-2=(d)）。

### OQ-2 · settings 合法性 dry-run 机检入不入 launch-prep、什么形态

**背景**：ARQ-260924 建议②（勘误后措辞「headless launch 面 settings 合法性 dry-run 机检」）。起草期实测了候选载体：`claude doctor` 不校验 permissions 规则形态（实测「No installation issues found」与病灶共存）；live 冒烟三形态各有硬伤（`-p` 无旗标挂起 ≥150s、带 skip-permissions 形态 agent 侧被 classifier 禁跑且属「真起会话」重形态、非 TTY 形态被 stdin 校验污染）；静态 lint 确定性、秒级、零 API、可进 launch-prep 步骤链。
**代价与为什么该 human 定**：新增机检 = launch-prep 步骤面挂载扩张 + 一个新 G3 脚本（机械强制层邻接面）；「机检管多宽」（只 Write / 扩 Read / 未来随 CLI 口径扩）是治理投资节奏判断。
- **(a) 静态 lint 脚本 + launch-prep 步骤 2 接线（Recommended）**——理由：对已实证病灶类确定性设防，每包 launch 面天然过检；只编码已实证口径不发明校验，假红风险最低。代价：未知的新校验口径（未来 CLI 版本对其它规则形态的新判）检不出——该残余由留根观察项兜（launch 现场报错仍可见、交互式形态容忍）。
- **(b) live 冒烟为步骤**——启动卡②命令行的最小 `-p` 形态由 human 自跑一次、以无报错行为过检判据。理由：能检未知校验口径变化。代价：API 调用 + 形态脆（挂起实测）+ agent 不可代跑（步骤链断在 human 到场）+ 「过检判据」依赖人读输出，非机械可判。
- **(c) a+b 并行**——lint 为常步、冒烟为 headless 形态采用前的 human 预检（双保险）。代价：(b) 的全部代价照付，且当前终裁「launch 一律交互式」下 (b) 半件无消费场景。
- **(d) 不设机检**——若 OQ-1=(a) 病灶已除，机检防的是「未来再加回/新漂移」；裁 (d) 则该防线缺位，靠 _doc 注记与 launch 现场报错兜。注意组合风险：OQ-1=(b)/(c) + OQ-2=(d) = 病灶保留且零兜底。

### OQ-3 · 契约面 merge 死路径：① 接证据 vs ② runbook 改道

**背景**：升级闸 `hasContractGateEvidence` 占位未接入 → 契约包恒 BLOCK 且无成文通道（K7 首例即兴人合）。孪生先例 ARQ-260902（ESCALATE 人批无机械路径）由 260904 批建 `--human-approval-ref` 五要素证据口消解——选项①即其同构复刻；选项②把 K7 实走验证过的人合操作固化成 §10e 四步条文 + BLOCK 输出携带指针。契约包频率实测：K7 为迄今唯一实例（低频）。
**代价与为什么该 human 定**：①=为最敏感面（契约）新开机械放行口——证据锚的形态设计（什么算「human 契约门签字」的机读凭据：ContractChangePacket 落点 / CDQ 行 / 事件枚举）是新机制裁定，设计错 = 契约面开旁路洞，不可逆性高；②=把「绕开 merge-driver 的例外通道」成文化——例外通道固定化本身是治理姿态判断（多久一次可接受即兴 vs 条文）。向严节奏归 human。
- **(a) ② 最小改道 runbook（Recommended）**——理由：半径小（判定语义零变更、self-check 全绿可机械证明）、把已验证走通的 K7 操作成文化、发现面机械化（BLOCK 输出带指针，下一个合并者不再现场发明）；①以留根形态保留并给量化触发器（契约包频率上升或 runbook 实走摩擦 ≥1 次实测）。代价：契约包永走人合，merge-driver 的 rebase/quick-check/机械簿记链须手动等价代记（runbook 第 4 步承载；K7 已实证可行，靠条文不靠自觉）；若契约包频率上升，人合摩擦随之上升（触发器届时兜住）。
- **(b) ① 接入契约门证据**——升级闸读 human 契约门批准凭据（与 `--human-approval-ref` 同构、五要素核验复用 lib），证据在场即放行，merge-driver 全链保全。理由：孪生先例同构、一劳永逸。代价：须先定义契约门证据的 canonical 机读锚（现状无此形态——ContractChangePacket 的落点/命名/签字字段均无机读约定），设计面 + 脚本面 + self-check 扩 + merge-driver 透传旗标，本批半径约翻倍；且证据形态一旦放行过宽即契约面旁路洞（向严接证据的设计评审本身值一个独立批）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**起草期实跑记录（2026-09-24，基线态〔零脚手架〕；全部命令已实跑，关键输出逐字）**：
- **B1 病灶 census**：`grep -rn '"Write(' .claude/*.json` → 恰 2 命中（dev-loop.settings.json:25 / review-loop.settings.json:14）；`grep -rn '"Read(' .claude/*.json` → 0 命中；permissions 规则条数（`grep -c '"Edit(\|"Write(\|"Read(\|"Bash(\|"mcp__'`）→ settings.json=18 / dev-loop=4 / review-loop=2。
- **B2 闸 self-check 基线**：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → exit 0，逐字 `{"selfCheck":"pre-merge-escalation-gate","pass":33,"fail":0,"total":33,"ok":true}`（前置另有 changeid-prefix self-check 行 `pass:4 fail:0`，系 lib 连带输出、非本闸计数）。
- **B3 crq 基线**：`node .claude/scripts/crq-doctor.mjs` → exit 0「条目 104 条：gov=86 tech=17 requirement=1 / advisory 142 条（不阻断）/ RED 零命中 —— ok」；INDEX 摘要行（docs/crq/gov/INDEX.md:10）逐字见 §2 T4 证据段。
- **B4 CLI 冒烟三形态**：记录见 §2 T1 证据段与两条目追注（报错逐字 / exit 124 挂起 / exit 1 stdin 污染 / doctor 不检出 / classifier 拦截带旗标形态）；原始输出文件 C:/tmp/smoke-headless.log、C:/tmp/smoke-interactive.log、C:/tmp/smoke-headless3.log（会话外临时面，引用以本 PROPOSAL 与 ARQ 追注的逐字记录为准）。
- **B5 锚文机械提取**：T1/T2/T3 全部承重锚文以 `sed -n '<行号>p'` 单行提取后复制入 §2（2026-09-24），CRLF 形态逐件核验（六件全 CRLF）。
- **B6 工作树实况**：`git status --porcelain` =（除 .codebase-memory 常驻脏 2 件）**3 个 M** + 本批新增目录（untracked）——2 个 M 为两条 ARQ 追注（起草期登记纪律产物）；第 3 个 M = `delivery/changes/arch/INDEX.md`（+1 行 = 本批 draft 状态行，`changes-index.mjs --upsert` 过程制品、AGENTS.md 状态转移钉行纪律的正当产物，不入 manifest——B6 初记时点在 upsert 之前故漏，R1-F3 补认，批 A R1-F4 同型第二实例；FROZEN 时点复跑记录随 v0.3 申报）；**`.claude/**` 零 diff**（零脚手架声明的机械证明：`git diff --name-only -- .claude/` 空输出，R1 判官亲核成立）。
- **V0 preflight（起草期已实跑，2026-09-24）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260924-gov-review-settings-deny-contract-merge/PROPOSAL.md` → 逐字关键字段 `"ok": true, "gate": "landing-preflight", "section2BodyFound": true, "anchorCount": 11, "findings": []`；`anchorUnion` 11 成员与 landing-manifest 11 行**逐 token 对上**（settings ×2 / settings-lint / SKILL / pod-coordination / gate / merge-driver / 归档迁入条目 ×2 / INDEX ×2），注记零路径 token 夹带（读法注第 3 条核过）。OQ 裁定后 v0.2 裁剪 manifest 时**复跑并重新对账**。

**landing 前机械前置**：
- **V0**（FROZEN 前义务）：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260924-gov-review-settings-deny-contract-merge/PROPOSAL.md` → 预期：形态错 0 条；`anchorUnion` 成员名单 == landing-manifest 逐行对上（v0.1 推荐位形态 = 11 行；v0.2 按 OQ 裁定裁剪后复跑仍须对上；注记零路径 token 夹带）。
- **V0b**（FROZEN 前义务，§5 实跑）：本验证集各命令在基线态可跑性预证——B2/B3 已实跑（上方记录即证）；V1–V3 的目标态断言在 landing 后跑，基线态反向断言（如「Write 行 =1」）已由 B1 承载。

**landing 后核验（逐条可判；评估时点标注：〔工作树〕= landing 落盘后 commit 前，〔commit 后〕= 分支 commit 后）**：
- **V1（T1，OQ-1=(a) 前提）**〔工作树〕：`grep -cF 'Write(./docs/contracts/**)' .claude/dev-loop.settings.json` → **0**；同命令对 review-loop.settings.json → **0**；`grep -cF 'Edit(./docs/contracts/**)' ` 两文件各 → **=1**（冻结语义保留锚，260908 批 V4 正向钉法同构）；`node -e "JSON.parse(require('fs').readFileSync('<file>','utf8'));console.log('ok')"` 两文件各 → `ok`（JSON 语法合法性）；`grep -c '勿再加回' .claude/dev-loop.settings.json` → **≥1** 且 `grep -c '勿再加回' .claude/review-loop.settings.json` → **≥1**（两 `_doc` 注记各有机械齿——T1-ii 两条 landing 指令均被 pin，漏落任一条即红，R1-F5 补）。**OQ-1=(b)/(c) 前提时 V1 替换为**（分支已随 v0.2 裁定失效，仅存选项空间记录）：两文件 `git diff --name-only -- .claude/dev-loop.settings.json .claude/review-loop.settings.json` → 空输出（settings 零改动断言，保留面 = OQ 裁定）。
- **V2（T2，OQ-2=(a)/(c) 前提）**〔工作树〕：`node .claude/scripts/settings-lint.mjs --self-check` → exit 0 且输出含固定 token（R1-F4 契约钉，机械 grep 可判）：尾行含 `"selfCheck":"settings-lint"` 与 `"fail":0`，且输出含 `bad-` 前缀例名行 ≥1（判别性证明 = 输出契约本身：反转检出逻辑必使 bad- 例 fail>0 → exit 1 → 本断言红）；`node .claude/scripts/settings-lint.mjs` → exit 0（OQ-1=(a) 后全 clean；OQ-1=(b) 时 exit 0 且白名单 WARN 恰 2 行）；`grep -c 'settings-lint' .claude/skills/kidpal-pod-launch-prep/SKILL.md` → ≥1。**OQ-2=(b) 前提时**（分支已失效，仅存记录）：SKILL.md 冒烟步骤在场 grep 断言；**(d) 时**：SKILL.md 与 scripts/ 新增面零 diff 断言。
- **V3（T3，OQ-3=(a) 前提）**〔工作树〕：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → exit 0 且 total ≥33、fail=0（判定语义不变的机械证明——reason 文本改动不破坏既有 `contracts-block` 例的 `includes('docs/contracts')` 断言）；`grep -cF 'pod-coordination.md §10e' .claude/scripts/pre-merge-escalation-gate.mjs` → **=1**；`grep -cF '§10e' .claude/scripts/merge-driver.mjs` → **=1**；`grep -c '10e. 契约包人合 runbook' .claude/references/pod-coordination.md` → **=1**（节标题在场）；runbook 四步关键词逐词 `grep -c` → 各 **≥1**（`pre-merge 预检` / `批准事件落账` / `ff 与 no-ff 选定` / `簿记等价代记`——起草期基线五词均 0〔含节标题词〕，landing 后各 ≥1，判别性成立，R1-N3 补全四步覆盖）。**OQ-3=(b) 前提时 V3 整体替换**（分支已失效，仅存记录；v0.2 重写口径见 R1 §5 自洽性核）。
- **V4（T4）**〔工作树 + commit 后复证〕：`node .claude/scripts/crq-doctor.mjs` → RED 零命中；`node .claude/scripts/crq-render.mjs` 复跑后 `git diff --exit-code -- docs/crq/` → exit 0（幂等）；`docs/crq/gov/_archive/2026/` 下出现 2 个本批结项条目文件、`docs/crq/gov/` 顶层不再有同名文件；`docs/crq/gov/INDEX.md` 摘要行含精确子串（`grep -F -c` → 1）`未结 16（待拍板 0 / 待触发 14 / 已接受风险 2）；已结 70（归档索引见下）。`（算术对账基线 = 起草期 B3/INDEX 现值 18/16/2/68，−2 结项：18−2=16、16−2=14、68+2=70；**若 landing 前其它批动账本，以 landing 时点复测值为基线重算并在此注明**）。
- **V5**〔commit 后〕：`node .claude/scripts/changes-index.mjs --upsert 260924-gov-review-settings-deny-contract-merge` → `delivery/changes/arch/INDEX.md` 该 changeId 单行在场且状态列 == 当前相位；合并 main 后复跑翻 mergedInMain（memory 纪律，非 landing-verifier 核验面）。
- **V6（回归面）**〔工作树〕：`node .claude/scripts/route-regression-check.mjs` → exit 0（基线尾行「28 ok, 0 fail」不变——本批不动 agent descriptions）；`node .claude/scripts/lint-descriptions.mjs` → exit 0（SKILL.md frontmatter description 零变更：`git diff -U0 -- .claude/skills/kidpal-pod-launch-prep/SKILL.md | grep -cE '^[+-](description|name):'` → **0**）。

**真空绿规约遵守声明**：V1–V3 均为「具体字符串 =/≥ 具体数」或「self-check 逐例结果」形态，反转实现（漏落任一 T）必红；条件替换断言（OQ 裁定分支）各自写明前提与保留面对账（V1 的 (b)/(c) 分支保留面 = OQ 裁定原文、V2 的 (d) 分支保留面 = manifest 裁剪记录）；「零命中/空输出」断言共 3 处（V1-(b) settings 零 diff、V2-(d) 零 diff、V6 frontmatter 零命中），保留面逐一标注如上，无裸「全/均/无」断言。V4 算术带基线时点与重算口径（ARQ-260914 ③ 连带项教训：评估时点显式）。

## 6. 留根清单

1. **真交互 TTY 带 `--settings` 的形态证据**（观察项）：本环境无 pty 工具、嵌套 TUI = 真起会话，该形态不可测——下一次 human 在场交互式 launch 时观察 settings 报错横幅是否出现及是否阻断，落 pod-events 或 progress.md 一行即闭合「两可」；不构成门。
2. **headless-260911 日志张力点**（记录不考古）：`.git/devloop/launch/headless-260911-screen-unification.log`（EXIT=0、无 settings 报错）与勘误「260924 为首次 headless launch」存在张力，该次是否带 `--settings` 不可判定——已在 ARQ-260924 追注留痕，不做进一步考古。
3. **升级闸接契约门证据（ARQ-260923 建议①）**（OQ-3=(a) 前提）：留根 + 量化触发器（契约包频率上升或 §10e runbook 实走摩擦 ≥1 次实测）；届时独立批承载（证据锚形态设计本身值一个完整评审面）。
4. **settings-lint 的 `Read(` WARN 级**（OQ-2 含 lint 前提）：同族推断未单独实证——若未来实测 CLI 对 `Read(` 形态的判定，按实证升 RED 或移除，脚本内注释已标「未实证推断」。
5. **VERSIONED_ARTIFACT_RE_LOCAL 双源留根**（既有，本批不动）：gate :190 与 merge-driver 同形正则的 lib 收编留根注记维持原状。
6. **headless launch 恢复使用的前置**（自律件）：human 终裁「处置拍板前 launch 一律交互式」——本批 landing 完成即「处置已拍板」，是否恢复 headless 形态归 human 另裁；若恢复，OQ-1=(a) 下无障碍、OQ-1=(b)/(c) 下须先重裁删除。
7. **260908 批历史验证集 V4 断言失效注**（OQ-1=(a) 前提，自律件）：该批 §5 V4 的 `grep -cF 'Write(./docs/contracts/**)'=1` 断言在删除后按字面为假——历史验证集性质为一次性落地核验、不回跑；未来批次引用 260908 制品时不得把该断言当活门消费（起草期已实证无活门消费者）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-24 - architect

**评审对象**：DRAFT v0.2（立项 ack + OQ 三项全裁 (a) 后的推荐位形态；零脚手架批——T1/T2/T3 目标文件当前 == HEAD 原文，§2 承重锚文即 landing-verifier 的唯一 byte 比对基准，故本轮对全部锚文做机械 byte 级核对）。判官 = harness 降级形态（零上下文 coder 子 agent 承判官）；只读全仓 canonical，唯一写入 = 本 §7 段。

**亲核方法与边界声明**：以下全部命令/脚本本轮亲跑（2026-09-24，gov worktree，HEAD `77000d9` + 工作树 ARQ 追注），非采信 PROPOSAL 转述。**不可亲核面（如实标注，以 ARQ 追注为证据基线）**：① `claude doctor` 完整 TUI 输出「No installation issues found」与 auto-update enabled/channel latest——以 ARQ-260924 追注③⑤为准（`claude doctor --help` 帮助文本自述「Reads settings files in the current directory without a trust prompt」本轮亲跑核实）；② exit 4 当日复现（classifier 拦截 nested skip-permissions，拦截正当不绕行）——以 2026-09-24 01:28 原始日志为准（`C:/tmp/pod-perf-k1-launch.log` 在场亲核，235B，内容逐字 == §2 T1 引文 == ARQ 行内引文）；③ OQ 三项 AskUserQuestion 回执本体（会话内事件，仓内不可核）——本轮核的是凭据链**文面一致性**（Status 凭据段①②③ ↔ §4 裁定记录 ↔ 各 T 改法前提，三处互恰无矛盾）+ classifier 拦截先例在案（ARQ-260924 正文「移除 deny 规则须 human 本人提出……拦截正确」逐字在场）。

**亲跑复核清单与关键输出**（逐字）：
- **B1 census 复核**：`grep -rn '"Write(' .claude/*.json` → 恰 2 命中（dev-loop.settings.json:25 / review-loop.settings.json:14）；`grep -rn '"Read(' .claude/*.json` → 0 命中（exit 1）；规则条数 `grep -c` → settings.json=18 / dev-loop=4 / review-loop=2；`.claude/settings.local.json` 不存在（ls 亲证）；`*.settings.json` glob 恰 2 件——**与 §3 硬项段、§5 B1 全数吻合**。全仓 `"Write(` 复扫（排除 .git/node_modules/build/managed_components/参考树/.codebase-memory）：规则位命中仅两 settings 文件，其余 4 件为文本引用（本 PROPOSAL 自身、ARQ-260924 条目、260908 批 findings-r1/defense-r1）——§3「命中仅账本/制品文本引用，非规则位」属实。
- **B2 复核**：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → exit 0，逐字 `{"selfCheck":"pre-merge-escalation-gate","pass":33,"fail":0,"total":33,"ok":true}`（前置 changeid-prefix 行 `pass:4 fail:0` 同在）——33 例基线全绿属实。
- **B3 复核**：`node .claude/scripts/crq-doctor.mjs` → exit 0「条目 104 条：gov=86 tech=17 requirement=1 / advisory 142 条（不阻断）/ RED 零命中 —— ok」；`docs/crq/gov/INDEX.md:10` 整行逐字 == §2 T4 引文「**摘要**：未结 18（待拍板 0 / 待触发 16 / 已接受风险 2）；已结 68（归档索引见下）。」；两条承载 ARQ frontmatter `status: deferred` 亲核、且均在 INDEX 待触发表在册（:29/:35）→ **V4 算术 18−2=16、16−2=14、68+2=70 成立**（基线时点 = 本轮现值，含批 A retro 新立条目；「landing 前他批动账本则以 landing 时点复测重算」口径已在 V4 明记，ARQ-260914 ③ 教训已应用）。
- **B4 冒烟日志在场性**：`C:/tmp/pod-perf-k1-launch.log`（235B，Sep 24 01:28）、`smoke-headless.log`（235B，18:59）、`smoke-interactive.log`（329B，19:00）、`smoke-headless3.log`（235B，19:07）四件在场；内容逐字核对——报错行 == §2 T1 引文（「Permission deny rule (.claude\dev-loop.settings.json): Write(./docs/contracts/**) is not matched by file permission checks — only Edit(path) rules are. Use Edit(./docs/contracts/**) instead (Edit rules cover all file-editing tools).」），smoke-interactive.log 另含「Error: Input must be provided either through stdin or as a prompt argument when using --print」== 追注「exit 1 由 stdin/--print 要求所致」的物证。
- **B5 CRLF 复核**：六个 `.claude/**` 目标文件 `grep -qU $'\r'` 逐件 → 全 CRLF，属实。
- **B6 工作树复核**：`git diff --name-only -- .claude/` → 空输出（**零脚手架声明的机械证明成立**）；但 `git status --porcelain` 实况与 B6 申报有一处不符——见 F3。
- **V0 复核**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` → `"ok": true, "gate": "landing-preflight", "section2BodyFound": true, "anchorCount": 11, "findings": []`；anchorUnion 11 成员与 landing-manifest 11 行逐 token 对上（独立比对），注记零路径 token 夹带。
- **V6 基线复核**：`node .claude/scripts/route-regression-check.mjs` → exit 0 尾行「route-regression-check: 28 ok, 0 fail」；`node .claude/scripts/lint-descriptions.mjs` → exit 0。CLI `claude --version` → `2.1.268 (Claude Code)` == §2 T1「现行 CLI（2.1.268）」。
- **T1 证据面**：`git log --oneline -S 'Write(./docs/contracts' -- .claude/` → `4e9d776` + `9d62625`，`git show -s` 日期 2026-08-27 / 2026-08-29 == 申报；260908 批 §5 V4 断言原文在场（`delivery/changes/arch/260908-gov-review-merge-chain-hardening/PROPOSAL.md:182`，`grep -cF 'Write(./docs/contracts/**)' … = 1` 形态）且全仓无活门消费者（该串脚本面命中仅 gate 自身 reason 与本批制品）——「历史断言非活门、不回跑」定性成立，§6 留根 7 处置得当。**双拦保全亲核**：两文件 `Edit(./docs/contracts/**)` 行在位；`.claude/scope-map.json:25` `"bashDenyWrite": ["docs/contracts/**", ".claude/**", "delivery/taskpackages/**"]`；enforce-scope.mjs 读 scope-map（:133）非读 settings deny；scope-map per-agent deny 含 `docs/contracts/**`（:56/:60/:64 抽查）——T1 删除后契约冻结三件承载零减损属实。
- **T3 证据面**：gate :263（`if (ctx.contractsDiff && !ctx.hasContractGateEvidence) {`）/:264（reason 行）/:366（live `hasContractGateEvidence: false` + 自注）/:393（self-check mkCtx 夹具）四处逐字 == 申报；self-check `contracts-block` 例（:413）`expect('contracts-block', { contractsDiff: true }, true, 'docs/contracts')` 在场，T3-ii 替换文本保留 `docs/contracts` 子串 → 断言仍命中属实；reason 串「契约面人签字制不动」全仓脚本消费者 = gate 自身唯一（grep 亲证）；gate BLOCK 输出段（:593-596）reasons 逐行写 stderr + 尾行「→ 升级 human，不得自动 merge。」== 申报；merge-driver Step 1b（:1250-1282）BLOCK → warn（:1276，闸尾 12 行）→ setPod blocked（:1277）→ return false（:1278）== 申报；pod-coordination §10a(:136)/§10b(:156)/§10c(:162)/§10d(:183) 族在场、`## 11.` 锚（:196）逐字 == 申报、全文件无既有 `10e`（撞号零）；pod-events-append.mjs `VALID_EVENT_TYPES` 闭集含 `human-decision`（:28-35，其中 :32 即该值）== 申报；`anchor3-deviation-ruling` 自由串先例（pod-coordination :204）、`approve-escalated-merge` 五要素先例（gate :74、brief-inputs.mjs :213）在场；`ARQ-260902-escalate-merge-no-human-path` 归档件 frontmatter `status: closed` + `closed: 2026-09-05` 亲核；K7 实走事件逐字（`C:/AI/KidPal/delivery/taskpackages/260923-m3-k7-schedule-grid-restore/pod-events.jsonl` merge-completed 事件 payload）：「主线程人工路径簿记（非 merge-driver）：升级闸对契约面 diff 恒 BLOCK（hasContractGateEvidence 占位未接入），human 直接指示合并 → git merge --ff-only 落本地 main；……merge-driver --no-ff 与 Step 5b 均未跑，本行即代记」——§2 引文忠实；merge-to-main skill 步骤 1（查本地分支）/步骤 2（`git diff --stat <target>...<source>` 预览 overlap + 标记双侧都改文件）在场 == §10e 步骤 1「同形」申报；「恒 BLOCK 且无机械放行口」亲证：gate :241 自注「永不豁免：provenance 缺失、off-main、**契约面**、verdict=BLOCK」。
- **T4 证据面**：`docs/crq/gov/_archive/2026/INDEX.md` 在场；批 A T7 写回形态（closedBy 格式 / crq-render 自动归位「人不搬文件」/ changes-index upsert）先例吻合。
- **留根 2 证据亲核**：`.git/devloop/launch/headless-260911-screen-unification.log` 在场（git common dir 内，9105B，mtime 2026-09-11 19:59），尾行 `EXIT=0`，全文 grep settings/permission deny 零命中——「张力点记录不考古」的证据基础属实。

**锚文 byte 级机械比对**（node 脚本，PROPOSAL 引文机械提取〔剥 blockquote 前缀/code-span 定界符〕vs 磁盘现行文；脚本输出 JSON.stringify 留痕）——6 处承重锚：**5 PASS / 1 FAIL**：
- PASS：T1 deny 块（PROPOSAL :67-70 vs dev-loop :23-26 与 review-loop :12-15，四行 byte 级含 CRLF）；T1-ii dev-loop `_doc` 锚（:83 vs 文件 :9，剥缩进后 byte 级、文件内唯一）；T1-ii review-loop `_doc` 锚（:91 vs 文件 :6，同上）；T2-ii SKILL 锚（:120 vs :46 全行）；T3-i pod-coordination 锚（:147 vs :196 全行）；T3-ii gate 锚（:164 vs :264 含缩进 raw byte 级）。
- **FAIL：T3-iii merge-driver 锚（:172 vs :1276）**——见 F1。全部锚串在各自文件内唯一性另行 grep 亲证（8/8 count=1），定位无歧义。

**Track B eligibility 独立裁（作者自评不作数）**：
1. **直接 Track A 判据（协议「双轨路由」节）零命中**：① 契约面——landing-manifest 11 行无任何 `docs/contracts/**` 路径（preflight anchorUnion 机证），T3 改的是闸对契约面 diff 的**处置输出文本**与 runbook 条文，契约文本零触碰（亲核 manifest + §3 表）；② 跨系统结构性变更——纯治理面（G2+G3+账本），固件/服务端语义零涉及。**不升 Track A。**
2. **「改 merge gate」机检硬排除的形态描述命中面独立裁**：该排除属「Track A 触发时适用」节（新 idea 轨），且协议 :96-97 两行明文「**执行测出的 bug 走 Track B 快路径（不论 G 几）**」——本批两件 ARQ source 均为执行实测（260924-perf-k1 launch 实测 exit 4 / loop-retro-2026-09-23 K7 实证），在案亲核；「governance 类变更不再强制升级」（同节）。T3 判定语义零变更有可机验证明面：self-check 33 例基线本轮亲跑全绿（V3 要求 landing 后复跑仍全绿）+ reason 串唯一脚本消费者 = gate 自身 + `contracts-block` 例 `includes('docs/contracts')` 断言在替换文本下仍命中——三点均亲核成立。**裁定：不构成须升 Track A 的 gate 变更；Track B 资格成立。**此点协议文本清晰（非「存疑」情形），不触发存疑升 human。
3. **T2 settings-lint 不构成新增工程门**：不入 verify-gates.json 闸族（本轮亲核该文件无 settings-lint 且 manifest 零触碰）、不进任何既有闸链，是 launch-prep 步骤面机检（该步骤面既有 check-launch 机械门与已知红判停同族）；OQ-2=(a) human 裁定在案。
4. **G3 变更批准凭据链（classifier 拦截先例纪律）**：ARQ-260924 正文记录两次拦截（Self-Modification：「移除 deny 规则须 human 本人提出……拦截正确」）→ OQ-1=(a) human 裁定原文（含「本裁定原文即批准凭据」性质声明）记 Status 凭据段 + §4 裁定记录——文面链条闭合（回执本体的核verify边界见上方声明③）。riskTier low-medium 与 review profile G2+G3 分类亲核成立（G3 触面五件枚举 == manifest 子集；G1/契约面/hooks/scope-map/verify-gates/agent defs/frontmatter 零触点亲核）。

**§5 条件分支存废自洽性（OQ 三项全裁 (a) 后）**：Status 行与 :13「manifest 形态记录」两处显式声明「各 T 内『裁 (b)/(c)/(d) 时』条件分支自此失效、仅作选项空间记录供判官核对」——V1-(b)/(c)、V2-(b)/(d)、V3-(b) 的留存有明示法律地位，生效断言 = 各条 (a) 前提主干，landing-verifier 消费面无歧义。**自洽。** OQ 给全性核对：ARQ-260924 建议①②③ → OQ-1(a)/OQ-2/§3 复扫先验+B1 census 全承接；ARQ-260923 二选一处方 → OQ-3 两选项即原处方①②——**该 human 裁的（机械强制层删除批准、机检投资节奏、向严节奏）均已给全且已裁。**

**T2-i 设计规格充分性（landing-verifier 可机械核验性）**：扫描面枚举 ✓（与磁盘实况吻合）、判定规则 ✓（Write→RED/Read→WARN/解析失败→RED，只编码已实证口径）、退出码 ✓（0/1/2）、普通跑输出形态 ✓（file:line + 规则串逐字 + 判级 + 尾行汇总）、「不入闸族」与 verify-gates.json 零触碰一致 ✓；**缺口 = `--self-check` 输出契约未钉**——见 F4。

**Findings**（F1 须修方可 FROZEN；F2–F6 须修；N1–N5 记录级，可随修顺带或如实保留）：

- **F1（BLOCKING）T3-iii 锚文与插入文本的反引号转义伪影——锚文失 byte 级逐字**：PROPOSAL :172 锚文引文与 :176 插入文本共 4 处含 `\`+反引号（0x5C 0x60 两字符）形态（精确扫描：全 PROPOSAL 恰此两行命中该伪影）；磁盘 `merge-driver.mjs:1276` 对应位置为裸反引号（0x60）。机械比对（本轮 node 脚本，JSON.stringify 留痕）：prop 含 `\\`+反引号、disk 含裸反引号，差集恰为两处反斜杠——**锚文非逐字**。三重后果：① 本批零脚手架，§2 引文 = landing-verifier 唯一 byte 比对基准，失逐字即核验必红（批 A R1-F1/F2 同类先例 = BLOCKING）；② :176 是**将插入 merge-driver 的 JS 源码行**——若 landing 按 byte 照抄，表达式位 `\` 前缀反引号 = SyntaxError，merge-driver 直接损坏；若执行者临场「手工反转义」，重新引入 §2↔磁盘歧义（批 A F3 教训）；③ CommonMark code span 内反斜杠转义不生效，:172 的 code span 实际在首个内嵌反引号处截断，渲染面同坏。注意差集**仅**反引号前的反斜杠——引文内 `\n` 序列（0x5C 0x6E）与磁盘 byte 一致，修复时勿连带改动。**修法**：:172/:176 两处去掉反引号前的反斜杠，改用双反引号 code span（`` ``…`` ``）或围栏代码块承载内嵌反引号（推荐围栏块，byte 保真且渲染稳）；修复后复跑锚文机械比对（本轮脚本可复用）+ preflight 复跑。不影响目标效果，无须升级 human。
- **F2（SHOULD）T3-iii 触发条件过宽，「非契约 BLOCK 零噪声」断言按字面为假**：条件 `out.includes('docs/contracts/')` 还会命中 gate 既有第二条含该子串的 reason——:275 `contract-findings-unaccounted`（reason 文本含字面列表 `docs/contracts/**|docs/spec.md|docs/requirements.md|docs/architecture.md`，亲核在场）。场景：包 Basis 引 `docs/requirements.md` 的 hard-violation 未入账、diff 不触契约面 → 通用 4 单独 BLOCK → merge-driver 亦打印「契约面 BLOCK——无机械放行口，人合走 §10e」——而该 BLOCK **有**机械放行口（CDQ 落行或 human defer/accept-risk 锚 → 复跑 gate，:273 自注 + :275 reason 文本即证），指针文本对其构成误导，§2「非契约 BLOCK 零噪声」失实。**修法**：条件子串收紧为通用 1 reason 前缀（如 `out.includes('docs/contracts/** diff 且同批无契约门证据')`——T3-ii 替换后该前缀保留，兼容自证），或如实降格 §2 措辞并注记通用 4 边界。不影响判定语义（warn 非控制流），不须升级 human。
- **F3（SHOULD）§5 B6「工作树实况」枚举与实况不符（批 A R1-F4 同类复发）**：B6 申报「（除 .codebase-memory 常驻脏 2 件）2 个 M（两条 ARQ 追注）+ 本批新增目录」；本轮实测 `git status --porcelain` 另有一个 M：`delivery/changes/arch/INDEX.md`（+1 行 = 本批 draft 状态行「| [260924-gov-review-settings-deny-contract-merge](…) | 2026-09-24 | draft | — |」，`changes-index.mjs --upsert` 过程制品，AGENTS.md 步骤 7 钉行纪律的正当产物、非越界，但申报未如实带出）。零脚手架核心断言（`.claude/**` 零 diff）本轮亲核仍真。**修法**：B6 补认该 M 及其过程制品性质（不入 manifest），并注明 FROZEN 时点复跑记录。批 A 同型 finding 第二实例，主线程宜对照 memory「别把看到的一处当全部」纪律自查申报生成方式。
- **F4（SHOULD）T2-i 规格未钉 `--self-check` 输出契约，V2 判别性断言不可机械 grep**：规格钉了普通跑输出形态与退出码，但 self-check 的逐例输出形状/夹具命名/尾行汇总格式无一字约定；V2 断言「输出含坏夹具检出例与好夹具放行例的逐例结果」+「bad-fixture 例的检出计数 ≥1」缺固定 token，landing-verifier 只能人读输出判——「反转实现必红」无法机械复核，与同批 B2 先例（gate self-check 尾行 JSON `{"selfCheck":"pre-merge-escalation-gate","pass":33,"fail":0,"total":33,"ok":true}`）的成熟形态未复用。**修法**：T2-i 规格增一条 self-check 输出契约（建议与 gate 同形：逐例行含夹具名 + ok，尾行 JSON `{"selfCheck":"settings-lint","pass":<n>,"fail":0,"total":<n>,"ok":true}`，坏夹具命名钉 bad- 前缀），V2 相应改为固定 token grep 断言（尾行 JSON 在场 + fail=0 + bad- 例检出 ≥1）。
- **F5（SHOULD）V1 未钉 review-loop 侧 `_doc` 注记——T1-ii 第二条 landing 指令无机械齿**：V1 断言集（Write=0 ×2 / Edit=1 ×2 / JSON ok ×2 / `勿再加回` ≥1 **仅 dev-loop**）对 review-loop 新 `_doc` 行（PROPOSAL :95）零 pin——若 landing 漏落该条，V1 仍全绿，反转实现不变红，违 fixtures-must-discriminate 纪律（landing-verifier 人读 diff 可兜，但机械齿缺位即兜底单层化）。**修法**：V1 增 `grep -c '勿再加回' .claude/review-loop.settings.json → ≥1`（:95 新行文本含该子串，零新造词），或以该行唯一子串「契约冻结 deny 只用 Edit(path) 形态」=1 钉。
- **F6（SHOULD）交互式形态断言超出实证面，且与本批自身「两可」纪律内相矛盾**：§2 T1 问题段「发生频率：……**交互式形态每次 launch 打印报错横幅**（容忍但噪声）」把未实证形态写成已实证——当日冒烟实证面 = `-p` 无旗标与非 TTY 两形态打印（亲核日志）+ 01:28 原始 exit 4（`-p`+skip-permissions）；真交互 TTY 带 `--settings` 不可测、K7 交互 launch 是否带 `--settings` 无记录——同 PROPOSAL 的 OQ-1 背景①「两可维持」与 §6 留根 1 正是此口径，问题段独越。同型第二处：T2-ii 插入文本（将落 SKILL.md canonical）「启动卡命令行①②两形态均受保护（校验发生在 settings 加载期，与 launch 形态无关——2026-09-24 冒烟实测）」——①（真交互 TTY）无冒烟实测，括注引证超出实证面（「校验发生在 settings 加载期」在两已测形态实证，向①的推广是推断）；OQ-1(b) 代价段「每次 launch 打印报错横幅」同型（已裁选项的历史记录，随修与否主线程裁）。**修法**：两处（+可选第三处）统一校准为「实证面 = `-p` 无旗标 / 非 TTY 打印 + `-p`+skip-permissions exit 4（原始证据）；真交互 TTY = 两可（§6 留根 1）」；T2-ii 括注改「报错发生在 settings 加载期、打印与形态无关（2026-09-24 冒烟实测：`-p` 无旗标/非 TTY 形态；真交互 TTY 见本批留根 1）」。断言纪律问题（§2 硬项「无实测的写未实测」），不改任何改法方向，不须升级 human。
- **N1（NIT）§10e 步骤 3 与 K7 实走存在半步之差，「成文化」宣称宜加半句限定**：K7 事件逐字记录 dev Pod 包实际用 `git merge --ff-only`（且引 human-approval-ref = `approve-escalated-merge` 事件），§10e 步骤 3 为 dev Pod 通道规定 `--no-ff`（merge-driver Step 5 同形 :1418 亲核、arch-review-protocol 步骤 7「`--no-ff` 双亲合并仅用于 merge-driver 的 dev Pod 通道」同口径——**规定本身正确且向严**）；步骤 2 的新 decision 值 `approve-contract-merge-manual` 亦是 K7 未走过的新口径（自由串先例支撑，合法）。节标题「K7 260923 即兴实走的成文化」对步骤 2/3 实为「按 canonical 口径收敛 K7 即兴偏差」。建议步骤 3 或 T3「为什么这样改」加半句（K7 实走 ff-only 系即兴偏差，条文按 protocol/driver 既有口径收敛），防后人对照事件与条文误判矛盾。
- **N2（NIT）T1-ii review-loop 插入指令括注歧义**：:93「（同上形态；该文件无 settings-lint 消费步骤注记需求，**句尾同指 lint**）」与 :95 给出的新行文本句尾「勿再加回；」并不指 lint——括注与文本二者有一不实。修法：括注改「句尾不指 lint（该文件无消费步骤）」或文本补 lint 指针，二选一澄清，防 landing 执行者在两者间猜。
- **N3（NIT）V3 对 §10e 四步的关键词 pin 仅覆盖第 4 步**：节标题 + `簿记等价代记` ≥1 能判别整节漏落，判不出步骤 1–3 被截落（landing-verifier 人读兜底）。建议增「pre-merge 预检」「批准事件落账」「ff 与 no-ff 选定」三个 grep ≥1（文本已含，零新造词）。
- **N4（NIT·记录级，不要求修）**：T1-ii 两处 `_doc` 锚文引文省略行首 4 空格缩进（blockquote 承载形态所致）——本轮剥缩进后 byte 级一致 + 文件内唯一，定位无歧义，不构成失逐字；记录供 landing-verifier 比对时按 trim 口径执行（同本轮脚本）。
- **N5（NIT·观察，不要求本批修）**：T2-i 扫描面 = 主 checkout settings 族，不扫 worktree 副本——分支期未合并漂移窗口（gov 侧先落、main 未合期间在 Pod 分支上新引入的无效规则）是已声明的接受残余（「Pod worktree 的 settings 随分支内容同源」+ OQ-2(a) 代价段「launch 现场报错仍可见」兜底），如实记录不扩圈。

**结构字段核对（随结论义务）**：landing-manifest 围栏块在场（11 行）✓、packet-id 行在场（:7）✓、preflight anchorUnion ≡ manifest 机证 ✓；§5 真空绿规约遵守声明与实际断言形态相符（「零命中/空输出」3 处均有保留面标注——其中 V1-(b)/V2-(d) 分支已随 OQ 全裁 (a) 失效，生效断言无裸「全/均/无」）。

**结论**：**打回（BLOCK）**——F1（BLOCKING：T3-iii 锚文/插入文本反引号伪影，byte 保真与 JS 语法双重风险）+ F2–F6（SHOULD：触发条件过宽断言失实 / B6 申报枚举缺一件 / self-check 输出契约缺位致 V2 不可机械判 / V1 缺 review-loop `_doc` 齿 / 交互式断言超实证面）须在标 FROZEN 前修复；六项均为文本面/规格面机械修复，修法方向已逐条给出，**零项影响目标效果、零项改法方向性错误、零漏根因类、riskTier 与 profile 分类成立、Track B 资格独立裁成立——无须升级 human，无须转 Track A**。N1–N5 可随 F 项修订顺带处置或如实保留。R2 复核范围 = F1–F6 修复点（重点：:172/:176 byte 级重比 + F2 条件子串与 T3-ii 替换文本的兼容性 + V1/V2/V3 新断言的判别性反照）+ 修复未引入新矛盾的回归确认（锚文全套重比 + preflight 复跑 + B6 复跑）。随行提醒（非 finding）：① 标 FROZEN 时 Status 行机锚形态须 `FROZEN v0.3` 样式（`FROZEN v<大>.<小>`，勿写「FROZEN（v0.3）」——升级闸 isFrozen 判读形态，先例教训在案）；② 修订后版本沿革行补 v0.3 条目；③ F1 修复涉及 :172/:176 两行重写，修复后本记录「锚文 byte 级机械比对」段的 FAIL 项以 R2 重比结果为准。

verdict: BLOCK(F1,F2,F3,F4,F5,F6)

### R2 - 2026-09-24 - architect

**复核对象**：DRAFT v0.3（R1 findings F1–F6 + N1–N3 修订，申报全部落 PROPOSAL 文本面、零脚手架态不变）。同判官 resume；轮次纪律 = 只核前轮 findings 修订到位性 + 修订是否引入新矛盾，不做全量重核（R1 全套亲核留痕仍为本记录的地面真值基础，未变动面不重跑）。

**亲跑复核清单**（全部本轮独立实测，非采信主线程申报）：
- **F1 修复点（重点，围栏块 byte 级重比）**：v0.3 全 PROPOSAL 恰 2 个 ```js 围栏块（@:172 / @:178，机械提取）——**block0（T3-iii 锚文）单行与磁盘 `merge-driver.mjs:1276` byte 级 `===` PASS**；block0 内 `\n` 序列（0x5C 0x6E）计数 3 == 磁盘 3（未连带改动，R1 叮嘱遵守）；**block1（插入行）**结构四查全 PASS：无反斜杠+反引号伪影、裸反引号 `warn(`…`)` 形态、6 空格缩进 `if (out.includes(` 起、条件子串 == 通用 1 前缀逐字——为可直接落盘的合法 JS 源码行。伪影复扫（0x5C 0x60 精确 pattern）：**正文区（§7 前）命中 0**；§7 记录区命中 1（:329 = R1-F1 finding 对伪影的描述性引文，append-only 历史记录天然保留、非制品缺陷）。主线程申报「全 PROPOSAL artifact count=0」按字面与实测有 §7 一处之差（其扫描或先于 R1 append、或排除记录区），修复实质（锚文/插入文本区伪影清零）成立，如实记录不计 finding。
- **F2 修复点（条件收紧 + 兼容自证）**：gate 现文 `grep -cF 'docs/contracts/** diff 且同批无契约门证据'` → **恰 1**（:264，通用 1 reason）；:275（通用 4 字面列表）与 :12（头注「diff 触 docs/contracts/** 且同批无契约门证据」语序不同）均不含该前缀（各 grep → 0）；merge-driver 现文基线 → 0；T3-ii 替换文本（v0.3 :168）以该前缀起句 → 替换后条件仍命中，兼容自证成立；「非通用 1 的 BLOCK 零噪声」按字面成立（全 gate 含该前缀者唯一）。:176 括注论证与上述实测全数吻合。
- **F3 修复点（B6 补认）**：`git status --porcelain` 复跑 == v0.3 B6 新申报——3 个 M（`delivery/changes/arch/INDEX.md`〔+1 行本批 draft 状态行，upsert 过程制品、不入 manifest〕+ 两条 ARQ 追注）+ .codebase-memory 常驻脏 2 件 + 本批目录 untracked；`git diff --name-only -- .claude/` → 空输出（零脚手架维持）。补认措辞含批 A R1-F4 同型实例标注与 FROZEN 时点复跑承诺 ✓。
- **F4 修复点（self-check 输出契约）**：T2-i :115 已钉四件——夹具 `bad-`/`good-` 前缀命名、逐例行含夹具名+ok 标记、尾行单行 JSON `{"selfCheck":"settings-lint","pass":<n>,"fail":<m>,"total":<n+m>,"ok":<bool>}`、任一例红 exit 1（与 gate self-check 同形，B2 先例复用）；V2 :271 改固定 token 断言（尾行 `"selfCheck":"settings-lint"` + `"fail":0` + `bad-` 例名行 ≥1），机械 grep 可判 ✓；判别性论证链复核成立：反转检出逻辑 → bad- 夹具不被检出 →「坏夹具必须检出为 RED」断言例 fail → fail>0 → exit 1 → V2 红（反转实现必红）。
- **F5 修复点（V1 双侧齿）**：V1 :270 已增 review-loop 侧 `grep -c '勿再加回'` ≥1；判别性基线亲证：两 settings 文件现文该词各 → **0**（T1-ii 两插入行文本各含「勿再加回」，landing 后各 ≥1，漏落任一条即红）。
- **F6 修复点（实证面校准）**：:50 问题段频率句已改「headless（`-p` + skip-permissions）形态每次必炸（exit 4，原始证据在案）；报错打印发生在 settings 加载期——`-p` 无旗标与非 TTY 两形态当日冒烟实证打印，真交互 TTY 形态未测（两可如实记……见 §6 留根 1）」——与证据段 :55、OQ-1 背景①、留根 1 四处口径一致；:124 T2-ii 插入文本（将落 SKILL canonical）已改「本机检在 launch 前静态校验 settings 文件内容，不依赖 CLI 各 launch 形态的行为差异，启动卡命令行①②两形态均覆盖（CLI 侧报错打印发生在 settings 加载期——2026-09-24 冒烟实证于 `-p` 无旗标与非 TTY 两形态；真交互 TTY 形态未测，见 ARQ-260924 追注）」——「均覆盖」的论证基础从「CLI 行为实测」换为「lint 静态校验性质」（与 launch 形态无关，实证），引证不再超实证面 ✓。OQ-1(b) 代价段保留原文不修 = R1「随修与否主线程裁」口径，判断已记版本沿革行 ✓。
- **N1 修复点**：步骤 3 限定句 ✓（K7 实走 `--ff-only` 系成文前即兴偏差 + 按 protocol 步骤 7 / merge-driver Step 5 既有口径收敛——与 K7 事件逐字及 protocol 原文相符）；**步骤 2 括注引入新事实误差 → 见 R2-F1**。
- **N2 修复点**：:93 括注已改「句尾**不**指 lint——以下方给出的插入行文本为准，括注与文本歧义已消」，与 :95 插入行文本（句尾「勿再加回；」）一致 ✓。
- **N3 修复点**：V3 :272 四步关键词已补全为逐词 `grep -c` 各 ≥1（`pre-merge 预检` / `批准事件落账` / `ff 与 no-ff 选定` / `簿记等价代记`）；判别性基线亲证：五词（含节标题词 `10e. 契约包人合 runbook`）在现 `pod-coordination.md` 各 → **0**；§10e 文本（:151-158）五词逐一在场（步骤 1–4 标题内）→ landing 后各 ≥1 成立。
- **N4/N5**：记录级不修，与 R1 处置口径一致 ✓。
- **回归确认**：全套锚文重比（v0.3 行号）——T1 deny 块 :67-70 vs dev-loop :23-26 与 review-loop :12-15（四行 byte 级）、T1-ii `_doc` 双锚 :83/:91（trim 口径，R1-N4 已注）、T2-ii SKILL :120 vs :46、T3-i pod-coordination :147 vs :196、T3-ii gate :164 vs :264（raw 含缩进）、T3-iii merge-driver 围栏 block0 vs :1276——**7/7 PASS**（v0.3 修订未波及任何承重锚）；preflight 复跑 → `"ok": true, "section2BodyFound": true, "anchorCount": 11, "findings": []`（anchorUnion ≡ manifest 11 行维持）；Status 行 DRAFT v0.3 + 版本沿革 v0.3 条目（逐 finding 修订对照，与文本实际变动一致）在场 ✓；R1 记录 append-only 保全（:293-345 未被改动）✓。
- **v0.3 内部一致性扫描**：V2↔T2-i 契约 token 一致；V1↔T1-ii 两插入行文本一致；V3↔§10e 文本一致；:168 T3-ii 替换文本↔:176/:179 条件子串兼容；:50↔:55↔OQ-1 背景①↔留根 1 口径一致——除 R2-F1 外未发现修订引入的新矛盾。

**Findings**：

- **R2-F1（SHOULD）N1 修复在 §10e 步骤 2 括注引入与地面真值相矛盾的历史陈述**：v0.3 :156「该 decision 值为本节首立口径，**K7 实走未落独立批准事件**、成文补立」——实测 K7 `pod-events.jsonl`（`C:/AI/KidPal/delivery/taskpackages/260923-m3-k7-schedule-grid-restore/`）human-decision 批准事件**在场**：eventId `c6464758-eac0-4719-aa40-531ac4526961`（actor=human、source=planner-relay、decision=`approve-escalated-merge`），且 merge-completed 代记事件 payload 逐字引「human-approval-ref=c6464758-eac0-4719-aa40-531ac4526961（pod-events human-decision approve-escalated-merge，actor=human）」——R1-N1 原文亦明确点出该事实。按字面读（无「契约包人合专用」限定词），该括注陈述为假；按限定读亦有歧义，且与本 record R1-N1 引用的证据表面冲突。该文本将落 `pod-coordination.md` canonical，冻结+落地后 PROPOSAL 不可回改、canonical 修正须另批——须在 FROZEN 前修。**修法**（一句替换，示例）：「该 decision 值为本节首立口径——K7 实走系复用 `approve-escalated-merge` human 批准事件作批准锚（eventId `c6464758`，merge-completed 代记 payload 引其 human-approval-ref）；本节为契约包人合首立专用 decision 值，系成文收敛」。不影响目标效果（runbook 四步机制与两处指针均无关此句），不须升级 human。

**结构字段核对（随结论义务）**：landing-manifest 围栏块在场（11 行，v0.3 零变化）✓、packet-id 行在场（:7）✓、preflight anchorUnion ≡ manifest 机证复跑 ✓。

**结论**：**打回（BLOCK）**——R1 六项 F 与三项 N 修订**全部到位且机械复核通过**（F1 围栏块 byte 级 ===、F2 前缀唯一性恰 1、F3 B6 与实况相符、F4/F5/N3 判别性基线亲证全 0、F6 四处口径一致、锚回归 7/7 PASS、preflight 复跑绿），但 N1 修复在 §10e 步骤 2 括注引入一处与 K7 pod-events 地面真值相矛盾的历史陈述（R2-F1，SHOULD，canonical-bound）——须在标 FROZEN 前修复。单点一句话替换，**零项影响目标效果、无须升级 human、无须转 Track A**。R3 复核范围 = R2-F1 单点（括注新文本与 K7 事件逐字一致性）+ 该修订未波及 §10e 其余文本与 V3 五词断言的回归确认（五词 baseline 不受该句影响，机械重跑即可）。随行提醒（非 finding）：① FROZEN 机锚形态 `FROZEN v0.3`（或修订后实际版本号 `FROZEN v<major>.<minor>`）样式，勿写「FROZEN（v0.3）」；② B6 的 FROZEN 时点复跑记录建议以一行 git status 输出实录进 Status 凭据段或 landing-summary，兑现 v0.3 申报的承诺措辞；③ R2-F1 修复后版本沿革行补记（v0.3.1 或 v0.4 由主线程定，机锚形态随之）。

verdict: BLOCK(R2-F1)

### R3 - 2026-09-24 - architect

**复核对象**：DRAFT v0.4（R2-F1 单点修订；版本沿革行含 v0.4 修订对照条目）。同判官 resume；复核范围 = R2 结论随行指定的单点 + 轻量回归，不做全量重核（R1/R2 全套亲核留痕仍为地面真值基础）。

**R2-F1 修复点复核（括注新文本 ↔ K7 事件逐字一致性）**：
- v0.4 :156 括注现文：「该 decision 值为本节首立口径：K7 实走系复用 `approve-escalated-merge` human 批准事件作批准锚（eventId `c6464758`，merge-completed 代记 payload 引其 human-approval-ref），本节为契约包人合首立专用 decision 值，系成文收敛，R1-N1/R2-F1」——与 R2 记录示例句语义全等（标点微调 + 尾注 finding 引用，引用规范），R2-F1 指出的假陈述「K7 实走未落独立批准事件」已消除。
- **地面真值本轮亲跑复证**（`C:/AI/KidPal/delivery/taskpackages/260923-m3-k7-schedule-grid-restore/pod-events.jsonl`）：eventId `c6464758-eac0-4719-aa40-531ac4526961` 命中 2 行（其一 `eventType:"human-decision"` 事件本体，actor=human）；该事件 `"decision":"approve-escalated-merge"` 逐字命中；merge-completed 代记事件 payload 含 `human-approval-ref=c6464758-eac0-4719-aa40-531ac4526961` 逐字命中——括注三处事实要素（复用 approve-escalated-merge 事件 / eventId 短形式所指唯一 / 代记 payload 引其 human-approval-ref）**逐一有据**。「本节为契约包人合首立专用 decision 值」佐证：全仓 `approve-contract-merge-manual` grep 命中面 = 仅本批 PROPOSAL（canonical 零先例）——「首立」为真。**R2-F1 结案（CONFIRMED→FIX-SPEC→已修→复核绿）。**

**轻量回归（申报波及面独立复核）**：
- §10e 其余文本未波及：节标题（:151）、适用面（:153）、步骤 1（:155）、步骤 3（:157，R1-N1 限定句原样）、步骤 4（:158）、边界与留根（:160）与 v0.3 逐句一致（本判官 R2 上下文比对），唯一变动 = :156 步骤 2 括注——「单点修订」申报属实。
- V3 五词断言基线机械重跑：`10e. 契约包人合 runbook` / `pre-merge 预检` / `批准事件落账` / `ff 与 no-ff 选定` / `簿记等价代记` 在现 `pod-coordination.md` 各 → **0**（判别性维持）；§10e v0.4 文本五词逐一在场（步骤 2 标题「human 批准事件落账」含「批准事件落账」子串）→ landing 后各 ≥1 成立，括注修订不触及任何关键词。
- `git diff --name-only -- .claude/` → 空输出（零脚手架态维持）；preflight 复跑 → `"ok": true, "section2BodyFound": true, "anchorCount": 11, "findings": []`（anchorUnion ≡ manifest 11 行维持）。
- Status 行 DRAFT v0.4 + 版本沿革 v0.4 条目（含主线程亲核记录与波及面申报）在场；R1/R2 记录 append-only 保全（:293-373 未被改动）。

**Findings**：无（零新增）。

**结构字段核对（GREEN 结论随行义务）**：landing-manifest 围栏块在场（11 行，v0.4 零变化）✓、packet-id 行在场（:7）✓、preflight anchorUnion ≡ manifest 机证复跑 ✓。

**结论**：**GREEN**——R2-F1 单点修复落实且括注新文本与 K7 pod-events 地面真值逐字一致，回归零波及，三轮（R1 BLOCK(F1–F6) → R2 BLOCK(R2-F1) → R3 GREEN）收敛、未触熔断。**无任何项须升级 human；Track B 资格独立裁维持（R1 记录）；riskTier low-medium 与 review profile G2+G3 分类维持。** 主线程可凭「立项 ack（2026-09-24）+ 本 GREEN」标 FROZEN（arch-review-protocol「Track B 七步流程」第 3 步）。随行提醒（非 finding）：① FROZEN 机锚形态须 `**状态：FROZEN v0.4`（`FROZEN v<大>.<小>` 样式，勿写「FROZEN（v0.4）」——升级闸 isFrozen 判读形态，先例教训在案）；② 标 FROZEN 时兑现 B6 承诺：git status 一行实录进 Status 凭据段或 landing-summary；③ landing 相位按 §5 V1–V6 + T4 顺序执行，landing-verifier 比对基准 = 本 v0.4 §2——T3-iii 两围栏块（:172/:178）内容行即磁盘 byte 基准（block0 == merge-driver:1276 已机证；block1 为合法 JS 插入行、照落零改动）；④ landing 后 `changes-index.mjs --upsert` 钉行，合并 main 后复跑翻 mergedInMain（memory 纪律）。

verdict: GREEN
