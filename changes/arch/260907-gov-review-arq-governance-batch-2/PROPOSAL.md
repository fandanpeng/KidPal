# PROPOSAL · 260907-gov-review-arq-governance-batch-2

> **状态：DRAFT v0.1 · 2026-09-07 · 待 human 立项 ack + OQ 三题裁断**
> **承载 ARQ**：`ARQ-260906-lfc-canonical-anchor-gap`、`ARQ-260906-lfc-verbatim-crlf-citation-gap`、`ARQ-260907-renormalize-clears-skip-worktree`、`ARQ-260907-frozen-acceptance-compile-baseline`、`ARQ-260907-taskpackage-format-expectations-frontload`、`ARQ-260907-friction-event-payload-schema`、`ARQ-260907-integrator-adr-canonical-location`（七项一批清结，前例 `260906-gov-review-arq-governance-batch`；另折入该批 retro §4 建议 1 的模板钉死项）。不关的根：五项非 governance ARQ（multiturn-continuity / p4-stroke-dasharray / v23-asr-swap / protocol-version-pinned / nvs-csv-wifi）留队列不动，见 §6。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T8）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（四件闸/聚合脚本改动均有自检或真数据重放兜底；成文两件行为保持；agent def 只加引用行）。
> **review profile**：G2+G3（触面 = `.claude/scripts/**` 四件 G3 闸/聚合脚本 + `docs/DEV-GOVERNANCE.md` / `docs/templates/**` G2 成文 + 四 agent def 引用行（human OQ2 前置）+ scope-map/repo-structure（OQ3 条件锚））；G1/契约面为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/pod-events-aggregate.mjs
docs/DEV-GOVERNANCE.md
docs/templates/PROPOSAL.template.md
.claude/agents/acceptance-author.md
.claude/agents/planner.md
.claude/agents/portfolio-planner.md
.claude/agents/builder.md
```

> manifest 注：T7（ADR canonical 落点）为 OQ3 条件锚——裁 (a) 时追加 `.claude/scope-map.json` + `.claude/rules/repo-structure.md` 两行，裁 (b) 时改落 `docs/DEV-GOVERNANCE.md`（已在册），裁 (c) 时 T7 整体撤出。T4/T5/T6 的 agent def 引用行为 OQ2 条件锚——裁 (b) 时四件 agent def 行撤出。DRAFT 期围栏块按 OQ 全裁 (a) 的满配形态预填，FROZEN 前按裁断裁剪。

## 0. 想实现的效果与差距

**目标效果**：把 2026-09-06/07 两轮实测与 loop-retro-2026-09-07 登记的七项 governance ARQ 一批清结，使 landing-fidelity 机械门对仓根点文件 / `changes/**` / CRLF 工作树 / 裸文件名引用四类形态恢复真判，merge-driver 对活图内容漂移恢复自动放行（260829-voice-roundtrip 合并实测 T6 不足），friction 记账与起包格式期望前置成文——下一个治理批 / dev-loop 包不再重付同型人工宽免与回派。

**现状差距**：§1.2 队列现存十二行，其中七行 governance 相关（两行 lfc 机械门缺口 + 四行 260907 loop-retro 批 + 一行 260907 renormalize 补证行）；三行机械门缺口在本批（260906-gov-review-arq-governance-batch）landing 时以「整裁腿 + 人读 diff 担纲」宽免过一次，每批重现；merge-driver 内容漂移病灶在包 2 合并实测打出（三次 rebase 拒绝 + 手动续 rebase 坑）；friction 空 payload 已构成实际证据丢失（55a43813 机器侧不可恢复）。

**非目标**（本批不做什么）：① 五项非 governance ARQ 不动（处理时机各有所属：部署首验 / P4 验收 / 配额重置补验 / dev-loop 固件包 / dev-loop 配网包）；② landing-fidelity 闸不新增命令行参数面（T1 走常量拓宽、不加 CLI 白名单选项）；③ 不动 verify-gates.json（260905 批已对齐）；④ 不改 `docs/contracts/**`（本批零契约触点）；⑤ 不动 dev-loop 内循环角色职责结构（agent def 只加 References/引用行，OQ2 裁 (a) 时）。

## 1. 问题与根因（地面真值）

### 问题类 A：landing-fidelity 机械门四类形态盲区（T1/T2，ARQ-260906-lfc-canonical-anchor-gap + ARQ-260906-lfc-verbatim-crlf-citation-gap）

- **锚形态盲区**（实测 2026-09-06，本批 landing）：`.claude/scripts/lib/governance-gate-shared.mjs:34` `export const CANONICAL_TEST = /^(?:\.claude\/|docs\/|\.planning\/|README\.md|AGENTS\.md|CLAUDE\.md)/;`——只认四目录 + 仓根三件套。仓根点文件（`.gitattributes`，260906 批首见）与 `changes/**` 路径（TP-A review-packet 复核注）无法机锚，锚行只能整裁腿登记、人读 diff 担纲，锚集 ≢ manifest 机检失配靠 landing-verifier 按 260905 判例宽免。**根因** = 锚提取正则先于落点形态演化；**实证次数** = 260906 批一次（两文件），凡同类落点批必复发。
- **verbatim 字节级对账 CRLF 假失配**（实测 2026-09-06，260906-gov-review-index-status-truth-source 批）：`.claude/scripts/landing-fidelity-check.mjs:152` `if (content.includes(block))` 逐字节比对——仓内 .mjs 工作树全量 CRLF（`git ls-files --eol` = i/lf w/crlf）、PROPOSAL.md 经 Write 落盘 LF，`includes` 必假；2 条 advisory 经字节级对账止于行尾差异（实质零漂移）。**根因** = 比对未做行尾归一。同闸 `checkCitationAnchors`（`landing-fidelity-check.mjs:71`）对 §6 裸文件名引用（无 `.claude/scripts/` 前缀）报 file-not-exist，三文件实存。**根因** = 引据解析无前缀补全。
- **影响面**：凡 §2 用围栏代码块逐字钉死改法的治理批于 Windows 工作树必复现 verbatim 假失配；引用裸文件名必复现 citation 假失踪——advisory 观察列信噪比持续走低，每批都需人工定性。

### 问题类 B：merge-driver 活图守护对「内容漂移」不足（T3，ARQ-260907-renormalize-clears-skip-worktree）

- **证据一（源仓回流，2026-09-07）**：`.gitattributes` 三步序步② `git add --renormalize .` 把 skip-worktree 标记清掉（`git ls-files -v` 由 `S` 翻回 `H`）、两件重新挂脏且被带入 staging（commit 前核对铁律抓住）。
- **证据二（包 2 合并实测补证，2026-09-07，ARQ 行原文）**：「skip-worktree 标记**完好**（S 在、merge-driver 日志 `0 set, 2 already flagged`）仍被挡：MCP 图索引自动刷新使两件内容与 HEAD 真实漂移，rebase 内部 checkout 对目标分支该路径内容不同的场景按真实字节比对拒绝（"local changes would be overwritten"），skip-worktree 对此不豁免——幂等重设 flag 防不住内容漂移，候选②不成立。实证修复 = rebase 前把两件**内容级**还原到 HEAD 快照（`update-index --no-skip-worktree` → `checkout --` → 复 flag），无副作用（图缓存自刷新）」；另 merge-driver rebase 中断会在 rebase-merge/done 堆积重复 pick，手动续 rebase 须清 todo。证据锚 = worktree `C:/AI/KidPal-pod-voice-roundtrip` 三次 rebase 拒绝日志 + pod-events friction 链 + 本仓 commit af2219d（merge-driver human-approval-ref=ec528da7）。
- **根因**：本批 T6 落地的 `ensureSkipWorktree`（`.claude/scripts/merge-driver.mjs:705`，rebase 步 :778 / merge 步两调用点）只保证旗标在位，不处理「旗标在位但内容漂移」——git rebase/checkout 对 skip-worktree 文件的内容比对不豁免。**这是上次 retro 交日项「包 2 是否踩 T6 病灶」的实证答案：踩了，且暴露的是 T6 设计层级缺口（旗标级 vs 内容级）。**

### 问题类 C：friction 记账机器侧失真（T4，ARQ-260907-friction-event-payload-schema）

- **证据**（loop-retro GC3 覆盖比对，7 个 new-format 包，ARQ 行原文）：① 260829-voice-roundtrip 2 条 friction 事件 payload 为空对象 `{}`（fb2c70c9、55a43813），其中 55a43813 内容机器侧不可恢复（实际证据丢失）；② 260903-device-ws-switch 2 条用非标键，聚合按 frictionType 归 unknown（`.claude/scripts/pod-events-aggregate.mjs:139` `const ft = (f.payload && f.payload.frictionType) || 'unknown';` 静默兜底）；③ 260831-ui-art-assets pod-retro 列 4 卡点而 friction 事件 = 0（ledger=0/retro=N 型）；④ 另三包各有 1–2 散文主题无对应 friction 事件。**根因** = friction 落账无最小 schema 约束、聚合器对残缺事件静默降级 unknown，无显式告警面。**实证次数** = 7 包中 4 包失真。

### 问题类 D：验收纪律缺「C 类冻结验收实编」判据（T5，ARQ-260907-frozen-acceptance-compile-baseline）

- **证据**（260829-host-test-infra，friction f0720d11 + pod-retro「流程债」节，ARQ 行原文）：冻结验收 `host_infra_smoke.acceptance.c` line99 宏参编译错误在 builder 首实编才暴露，红基线从未实编过该文件；退回 acceptance-author 修（具名数组）+ 实编确认后过关。与已结 ARQ-260902-frozen-acceptance-never-run（本仓 `docs/DEV-GOVERNANCE.md:263`「## 验收测试纪律」§3 冻结即冒烟）相关但不重合：§3 冒烟判据写的是 live/线束/pytest 通道（`DEV-GOVERNANCE.md:280`「### 3. 冻结即冒烟（线束通道 error 为零）」），未明示 **C 冻结验收文件本身的可编译性**。**根因** = 冒烟通道清单缺 C 类实编项。

### 问题类 E：起包格式期望不前置，闸红反推回派（T6，ARQ-260907-taskpackage-format-expectations-frontload）

- **证据**（跨 Pod，loop-retro 归纳，ARQ 行原文五子项）：① portfolio 起草的 taskpackage 不带 mutationEvidence 骨架（260903-gateway-directionB mutation-check 红一轮；260829-session-logic 分工模糊、planner 补位）；② acceptance-author 按前包模板写 markdown prose 被 acceptance-coverage 判「无可识别断言」，回派补 31 条机器断言（260831-ui-art-assets 卡点2）；③ 验收 ID 带 `(B#)` 破坏 `\b` 匹配（同包卡点3）；④ 资产/规划类包对 mutation/docs-sync 的降级声明到 verifier 红才补（卡点4）；⑤ 建新约定的包未同步查相关机械闸（260829-host-test-infra，vacuous-green human-decision afd31ac 直改收口）。**根因** = 格式期望只存在于闸的判定代码里，起草侧（planner/portfolio-planner/acceptance-author/builder）无成文期望清单，靠闸红反推。**实证次数** = 4 包 5 子项。

### 问题类 F：ADR canonical 落点未裁（T7，ARQ-260907-integrator-adr-canonical-location，COULD）

- **证据**（260829-smoke-e2e pod-retro「钩子/规则帮倒忙与帮忙」节，ARQ 行原文）：enforce-scope 拦 integrator 写 `docs/notes/` 是照章办事，但 ADR 的 canonical 落点未给 integrator allow 面明确答案，现状只能落 `changes/dev/`。**根因** = scope-map 角色编制与 ADR 制品归属未对齐。**性质** = 需要一次落点裁定（OQ3），非纯机械修复。

### 问题类 G：PROPOSAL 模板未钉死机械门可辨形态（T8，260906 批 retro §4 建议 1 折入）

- **证据**（260906 批 landing 实测，本仓 retro F1）：改法标题 `**T<n> · …**` 粗体段形态被 `parseSection2AnchorLists` 判「§2 无任何 T 条目」、manifest 括注行被 `extractManifestBlock` 逐字取入——FROZEN 后格式返修一轮 + verifier 额外负担。模板硬项 2a（`docs/templates/PROPOSAL.template.md:49`）钉了锚清单义务但未钉标题/manifest 行形态。**根因** = 机械门契约（标题形态 / 锚正则）是 drafting 相位输入约束，模板未承载。

## 2. 改法（T1–T8）

### T1 · landing-fidelity 锚形态拓宽：新增 ANCHORABLE_TEST 与 CANONICAL_TEST 分离

`governance-gate-shared.mjs` 新增导出常量 `ANCHORABLE_TEST`（锚提取专用）：形态 = 既有 `CANONICAL_TEST` ∪ 仓根点文件（`.` 开头单段文件名，如 `.gitattributes`）∪ `changes/` 前缀路径。`.claude/scripts/landing-fidelity-check.mjs:386` 锚提取过滤处 `CANONICAL_TEST.test(...)` 改用 `ANCHORABLE_TEST.test(...)`。**`CANONICAL_TEST` 本体与 `changedNotInProposal` 检测面（`landing-fidelity-check.mjs:367`）维持不动**——防把治理 landing 期 `changes/**` 制品常规写入误报成 changed-not-in-proposal（检测面向严、锚提取向宽，两职责分离）。为什么这样改：T7/T9 类落点（仓根点文件、changes 制品复核注）是合法治理落点，锚提取必须能机辨；而 changed-not-in-proposal 是「未申报改动」检测，扩面会引入新假阳，不在本批诉求内。`runSection2AnchorSelfCheck` 补两形态用例（仓根点文件锚、changes/** 锚各一正例）。

目标文件：`.claude/scripts/lib/governance-gate-shared.mjs`（ANCHORABLE_TEST 新增）、`.claude/scripts/landing-fidelity-check.mjs`（:386 过滤切换 + 自检用例）。

### T2 · verbatim 对账 CRLF 归一 + citation 前缀补全

`checkVerbatimQuotes`（`landing-fidelity-check.mjs:135`）比对前双侧行尾归一：`content.includes(block)` 改为 `content.replace(/\r\n/g, '\n').includes(block.replace(/\r\n/g, '\n'))`。`runVerbatimQuotesSelfCheck` 补 CRLF 用例（LF 围栏块 × CRLF 目标文件 → 命中，即 missing 不新增）。`checkCitationAnchors`（:71）对 file-not-exist 结论追加前缀重解析：候选前缀 `.claude/scripts/`、`.claude/hooks/` 依次试探，命中即消警并在 problems 输出附 `resolvedByPrefix` 注记（不静默——消警要有痕）。为什么：两类假失配都属「实质零漂移/实存文件被判失踪」，归一与补全都比对账纪律更便宜；消警留痕防真失踪被前缀试探掩盖。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`。

### T3 · merge-driver 活图守护升内容级：ensureGraphCacheSynced

`ensureSkipWorktree`（`merge-driver.mjs:705`）升级为 `ensureGraphCacheSynced`，对 `CODEBASE_MEMORY_DIRTY_PATHS` 两件执行幂等三拍：① `git ls-files -v` 查旗标，S 在册者先 `git update-index --no-skip-worktree`；② `git checkout -- <path>` 把内容还原到 HEAD 快照（图缓存自刷新，无数据丢失面——ARQ 补证实证修复原样固化）；③ `git update-index --skip-worktree` 复旗标。stderr 留痕一行（`N content-restored, M flag-only`），失败 advisory-only 永不阻塞合并（承 T6 既有语义）。rebase 步（:778）与 merge 步两调用点不变。头注释补 `.gitattributes` 三步序交互注记：「renormalize 会清 skip-worktree 旗标（ARQ-260907-renormalize-clears-skip-worktree 证据一），凡跑 renormalize 后依赖本函数幂等复位；本函数已升内容级，旗标与内容双兜底」。`--self-check-worktree-topology` 补断言：临时夹具内对被跟踪文件改内容 + 设旗标，跑 `ensureGraphCacheSynced` 后断言内容与 HEAD 一致且旗标复位。rebase 中断堆积重复 pick 的次生坑以头注释成文（清 todo 指引），不改 rebase 逻辑本体。为什么：包 2 实证旗标级守护不足（ARQ 补证「候选②不成立」），内容级还原是实测验证过的修复。

目标文件：`.claude/scripts/merge-driver.mjs`。

### T4 · friction 事件最小 schema：聚合器显式告警 + 成文

`pod-events-aggregate.mjs` `aggregateFriction`（:135 起）增加残缺事件显式列 `malformedFrictions`：payload 空对象 / 缺 `frictionType` / 缺 `summary` 三判据，命中即入列（含 changeId + eventType 侧定位信息），不静默归 unknown；既有 `unknown` 兜底保留（非标键仍归 unknown，但残缺件同时出现在 malformedFrictions 列——告警与归档双轨）。无 self-check 入口（该脚本现无自检），验证走真数据重放：对 `.git/devloop/pods/` 现存 pod-events 跑聚合，断言 260829-voice-roundtrip 的 fb2c70c9 / 55a43813 与 260903-device-ws-switch 非标键两条全部入列（§5 V4）。schema 成文进 `docs/DEV-GOVERNANCE.md`（「friction 事件最小 schema」小节：`frictionType` + `summary` 必填、附加细节允许扩展键、空 payload = 落账缺陷）。planner def 引用行（OQ2 条件锚）。

目标文件：`.claude/scripts/pod-events-aggregate.mjs`、`docs/DEV-GOVERNANCE.md`、`.claude/agents/planner.md`（OQ2 条件锚）。

### T5 · 验收纪律 §4：C 类冻结验收实编

`docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节新增 §4「C 类冻结验收实编」：C 类（固件/结构面）冻结验收落盘时，acceptance-author 当场实编一次该验收文件（error 为零），红基线明示「验收文件本身可编译」判据；与 §3 冻结即冒烟并列（§3 管执行通道，§4 管编译通道）。`acceptance-author.md` References 节加一行引用（「（T5，260907 批）C 类验收落盘前必读 §4」形态，承 260906 批 T8 同款 +1 引用行先例）。为什么：host-test-infra 实证「红基线从未实编过验收文件」，§3 通道清单不含编译。

目标文件：`docs/DEV-GOVERNANCE.md`、`.claude/agents/acceptance-author.md`（OQ2 条件锚）。

### T6 · 起包格式期望前置清单

`docs/DEV-GOVERNANCE.md` 新节「起包格式期望前置」（验收测试纪律旁），五子项一一对应 ARQ 行五证据：① Draft TaskPackage 带 mutationEvidence 骨架（空骨架也行，字段必须在场）；② acceptance 断言格式期望（机器可识别断言形态 + coverage 标记随测试名）写进 acceptance-author 派发默认；③ 验收 ID 禁括号后缀（`(B#)` 形态破坏 `\b` 匹配）；④ 资产/规划类包 mutation/docs-sync 降级声明在 Draft 阶段写字段；⑤ 新约定包 dispatch 时同步查相关机械闸判据。planner / portfolio-planner / builder / acceptance-author 四 agent def 各加一行 References 引用（OQ2 条件锚）。为什么：格式期望只活在闸代码里、起草侧零成文，4 包 5 子项同型红是结构性回派税。

目标文件：`docs/DEV-GOVERNANCE.md`、`.claude/agents/planner.md`、`.claude/agents/portfolio-planner.md`、`.claude/agents/builder.md`、`.claude/agents/acceptance-author.md`（后四件 OQ2 条件锚）。

### T7 · ADR canonical 落点（OQ3 条件项）

OQ3 裁 (a)（新 `docs/adr/` 目录）：`scope-map.json` `universal.docsTopLevel` 加 `adr/`、architect/integrator 角色 allow 面加 `docs/adr/**` 写槽；`repo-structure.md` 顶层地图加行（`docs/adr/` = ADR/rationale canonical，architect 产出、integrator 可写）；DEV-GOVERNANCE 入口速查补一句。裁 (b)（维持现状）：DEV-GOVERNANCE 成文惯例——integrator 落 `changes/dev/<changeId>/`，主线程收口晋升时迁 `docs/notes/`，`docs/notes/` 写权限维持主线程。裁 (c)：T7 整体撤出，ARQ 留队列。为什么交 human：scope-map 角色编制变更是结构边界动作（repo-structure.md「有意识动作」条款），且落点选择是 discoverability vs 结构克制的取舍。

目标文件（条件）：`.claude/scope-map.json`、`.claude/rules/repo-structure.md`、`docs/DEV-GOVERNANCE.md`（裁 (a) 前两件 + 后一件；裁 (b) 仅后一件）。

### T8 · PROPOSAL 模板钉死机械门可辨形态

`docs/templates/PROPOSAL.template.md` 硬项 2a 段末追加两句：「改法标题一律 `### T<n> · <标题>` 标题形态——机械门 `parseSection2AnchorLists` 只认 `### T<n>`（260906 批 FROZEN 后格式返修实证，粗体段形态不被机辨）；landing-manifest 围栏块一行一个裸路径，不带括注（`extractManifestBlock` 逐字取行）。」为什么：机械门契约是 drafting 相位输入约束，模板是起草侧唯一强制阅读面，零脚本改动消一整类返修。

目标文件：`docs/templates/PROPOSAL.template.md`。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 闸共享库 | `.claude/scripts/lib/governance-gate-shared.mjs` | G3 | T1 新增导出；消费方 = landing-fidelity-check（锚提取切换）+ proposal-consolidation-check（不受影响，CANONICAL_TEST 本体不动） |
| 结构闸 | `.claude/scripts/landing-fidelity-check.mjs` | G3 | T1/T2：锚过滤切换、verbatim 归一、citation 前缀补全、自检三处扩展；advisory 列结构不变（problems 字段追加字段，消费方 = landing-verifier 人读） |
| 合并通道 | `.claude/scripts/merge-driver.mjs` | G3 | T3：ensureGraphCacheSynced 替换 ensureSkipWorktree（两调用点语义升级）；自检扩展；既有五自检不回退 |
| 聚合器 | `.claude/scripts/pod-events-aggregate.mjs` | G3 | T4：aggregateFriction 加 malformedFrictions 列；消费方 = loop-retro / 主线程人读，纯增量字段 |
| 治理成文 | `docs/DEV-GOVERNANCE.md` | G2 | T4/T5/T6/T7 四节新增/扩写；行为保持（成文面） |
| 模板 | `docs/templates/PROPOSAL.template.md` | G2 | T8 硬项 2a 段末追加两句 |
| agent def | `.claude/agents/{acceptance-author,planner,portfolio-planner,builder}.md` | agent def | T4/T5/T6 引用行（各 +1 行 References，OQ2 条件锚；human 批准前置） |
| 结构边界（条件） | `.claude/scope-map.json`、`.claude/rules/repo-structure.md` | G2 | 仅 OQ3 裁 (a) 时触发 |
| 未触 | `docs/contracts/**`、`docs/spec.md`、`firmware/**`、`server/**`、`.claude/settings*.json`、`verify-gates.json` | 未触 | 本批零触点（§0 非目标） |

**【硬项 1b 预跑】** T1 判定 pattern（ANCHORABLE_TEST）全仓预跑：以 `git ls-files` 全列表过新正则，census 仓根点文件（预期 `.gitattributes`、`.claude` 例外形态核对）与 `changes/**` 命中量，输出进 landing-summary。T2 归一 pattern 全仓无面（运行时双侧归一，无扫描面）。T4 告警 pattern 预跑 = §5 V4 真数据重放。

## 4. human 取舍点（OQ）

- **OQ1 · 批次范围**：(a) T1–T8 八项全收（推荐——七 ARQ + 模板钉死同根同批，前例 260906 批十项一批先例，分批徒增评审开销）；(b) 收 T1–T6 + T8、T7（ADR 落点）留队列（若想隔离结构边界变更）；(c) 只收机械件 T1–T4（闸侧自洽批，成文面留下批）。
- **OQ2 · agent def 引用行授权**：T4/T5/T6 涉四件 agent def 各 +1 行 References 引用（承 260906 批 OQ3=(b)「agent def 只加引用、职责/工具面结构零改动」human 已批先例）：(a) 批准（推荐——起草 agent 不经 planner 中转直接可见期望，回派税的根在 agent 侧视野）；(b) 只成文 DEV-GOVERNANCE、不动 agent def（期望可见性降级，回派风险残留）。
- **OQ3 · ADR canonical 落点**：(a) 新 `docs/adr/` 目录（推荐——ADR/rationale 是跨包复用的架构决策面，散落 `changes/dev/` 发现性差；scope-map + repo-structure 双登记一次到位）；(b) 维持 `changes/dev/` 落 + 主线程晋升迁移惯例成文（零结构变更，发现性债保留）；(c) 本批不裁、T7 撤出。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1** `node .claude/scripts/landing-fidelity-check.mjs --self-check` → `ok:true` 且自检名清单含新增（CRLF verbatim 用例、ANCHORABLE_TEST 锚用例各一）。
- **V2** T1 预跑 census：`git ls-files | node -e "<ANCHORABLE_TEST 过滤>"` → 仓根点文件 + `changes/**` 命中清单在册（landing-summary 附命令与输出）。
- **V3** T3 自检：`node .claude/scripts/merge-driver.mjs --self-check-worktree-topology` → `pass:6 fail:0 ok:true`（原 5 断言 + 内容级还原新断言）；既有四自检（retro-advisory / quick-checks / detect-changes / rebase-target）全 ok 不回退。
- **V4** T4 真数据重放：对 `.git/devloop/pods/` 现存事件跑 `pod-events-aggregate.mjs` → 260829-voice-roundtrip fb2c70c9 / 55a43813 与 260903-device-ws-switch 非标键两条全部命中 malformedFrictions（重放先断言输入非空——真空绿规约）。
- **V5** T5/T6/T7 成文核验：DEV-GOVERNANCE 新节标题 grep 在场；T7 裁断对应落点文件 grep 在场。
- **V6** `node --check` 对四件 .mjs 全过。
- **V7** landing 后结构门 `node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-arq-governance-batch-2` → 锚集 ≡ manifest（本批落点全部 canonical/ANCHORABLE 可机辨——T1 自证）；verbatim advisory 零 missing（CRLF 归一自证——§2 围栏块如有）。
- **V8** agent def 引用行核验（OQ2 裁 a 时）：四件 def diff 各恰 +1 行、职责/工具面结构零改动。

## 6. 留根清单

- **不关的根**：五项非 governance ARQ 留队列（multiturn-continuity / p4-stroke-dasharray-visual-check / v23-asr-swap-quota-recheck / protocol-version-pinned-constant / nvs-csv-wifi-injection），处理时机各在 ARQ 行（部署首验 / P4 验收 / 配额重置补验 / dev-loop 固件包 / dev-loop 配网包），本批不代处理。
- **交日项（承 260906 批 retro §6）**：「包 2 是否踩 T6/T7/T8」的复盘结论随本批 landing-summary 记账——T6 踩（内容漂移实证，T3 承接修复）、T7 部分踩（pod-retro 仍录 `tr -d '\r'` 绕行，疑 pod worktree 先于 T7 合并创建，checkout 代次问题成文备注）、T8 遵循度（包 2 acceptance-author 是否引用 DEV-GOVERNANCE 纪律）核对后一并记录。
- **登记不硬治项**：rebase 中断重复 pick 清 todo 指引以 merge-driver 头注释成文（T3 内），不做 rebase 状态自动清理（场景过窄）。
- **自律件**：本批 PROPOSAL 自身即按 T8 钉死形态起草（`### T<n>` 标题 + 裸路径 manifest），起草期自证模板句可执行。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
