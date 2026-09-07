# PROPOSAL · 260907-gov-review-arq-governance-batch-2

> **状态：DRAFT v0.3 · 2026-09-07 · R1 NEEDS-REVISION（F1–F5 修订级 + F6–F11 NIT）已修订 · human 已裁 OQ1=(a) / OQ2=(a) / OQ3=human 自裁 ADR 落 docs/ADR.md 单文件日志 · 待 R2 复验**
> **承载 ARQ**：`ARQ-260906-lfc-canonical-anchor-gap`、`ARQ-260906-lfc-verbatim-crlf-citation-gap`、`ARQ-260907-renormalize-clears-skip-worktree`、`ARQ-260907-frozen-acceptance-compile-baseline`、`ARQ-260907-taskpackage-format-expectations-frontload`、`ARQ-260907-friction-event-payload-schema`、`ARQ-260907-integrator-adr-canonical-location`（七项一批清结，前例 `260906-gov-review-arq-governance-batch`；另折入该批 retro §4 建议 1 的模板钉死项）。不关的根：五项非 governance ARQ（multiturn-continuity / p4-stroke-dasharray / v23-asr-swap / protocol-version-pinned / nvs-csv-wifi）留队列不动，见 §6。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T8）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（四件闸/聚合脚本改动均有自检或真数据重放兜底；成文两件行为保持；agent def 只加引用行）。
> **review profile**：G2+G3（触面 = `.claude/scripts/**` 四件 G3 闸/聚合脚本 + `docs/DEV-GOVERNANCE.md` / `docs/templates/**` / `docs/ADR.md` G2 成文 + scope-map/repo-structure 结构边界（T7）+ 四 agent def 引用行（OQ2=(a) 已裁））；G1/契约面为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/pod-events-aggregate.mjs
docs/DEV-GOVERNANCE.md
docs/templates/PROPOSAL.template.md
docs/ADR.md
.claude/scope-map.json
.claude/rules/repo-structure.md
.claude/agents/acceptance-author.md
.claude/agents/planner.md
.claude/agents/portfolio-planner.md
.claude/agents/builder.md
```

> manifest 注：OQ 已全裁——OQ1=(a) 八项全收、OQ2=(a) 四件 agent def 引用行批准、OQ3=human 自裁 ADR 落 `docs/ADR.md`（docs 根、与 spec 同级）。T7 三件（docs/ADR.md 新增 + scope-map + repo-structure）由条件锚转常设锚，围栏块为满配形态，FROZEN 前人工复核与 §2 并集一致。

## 0. 想实现的效果与差距

**目标效果**：把 2026-09-06/07 两轮实测与 loop-retro-2026-09-07 登记的七项 governance ARQ 一批清结，使 landing-fidelity 机械门对仓根点文件 / `changes/**` / CRLF 工作树 / 裸文件名引用四类形态恢复真判，merge-driver 对活图内容漂移恢复自动放行（260829-voice-roundtrip 合并实测 T6 不足），friction 记账与起包格式期望前置成文——下一个治理批 / dev-loop 包不再重付同型人工宽免与回派。

**现状差距**：§1.2 队列现存十二行，其中七行 governance 相关（两行 lfc 机械门缺口 + 四行 260907 loop-retro 批 + 一行 260907 renormalize 补证行）；三行机械门缺口在本批（260906-gov-review-arq-governance-batch）landing 时以「整裁腿 + 人读 diff 担纲」宽免过一次，每批重现；merge-driver 内容漂移病灶在包 2 合并实测打出（三次 rebase 拒绝 + 手动续 rebase 坑）；friction 空 payload 已构成实际证据丢失（55a43813 机器侧不可恢复）。

**非目标**（本批不做什么）：① 五项非 governance ARQ 不动（处理时机各有所属：部署首验 / P4 验收 / 配额重置补验 / dev-loop 固件包 / dev-loop 配网包）；② landing-fidelity 闸不新增命令行参数面（T1 走常量拓宽、不加 CLI 白名单选项）；③ 不动 verify-gates.json（260905 批已对齐）；④ 不改 `docs/contracts/**`（本批零契约触点）；⑤ 不动 dev-loop 内循环角色职责结构（agent def 只加 References/引用行，OQ2 裁 (a) 时）。

## 1. 问题与根因（地面真值）

### 问题类 A：landing-fidelity 机械门四类形态盲区（T1/T2，ARQ-260906-lfc-canonical-anchor-gap + ARQ-260906-lfc-verbatim-crlf-citation-gap）

- **锚形态盲区**（实测 2026-09-06，本批 landing）：`.claude/scripts/lib/governance-gate-shared.mjs:34` `export const CANONICAL_TEST = /^(?:\.claude\/|docs\/|\.planning\/|README\.md|AGENTS\.md|CLAUDE\.md)/;`——只认四目录 + 仓根三件套。仓根点文件（`.gitattributes`，260906 批首见）与 `changes/**` 路径（TP-A review-packet 复核注）无法机锚，锚行只能整裁腿登记、人读 diff 担纲，锚集 ≢ manifest 机检失配靠 landing-verifier 按 260905 判例宽免。**根因** = 锚提取正则先于落点形态演化；**实证次数** = 260906 批一次（两文件），凡同类落点批必复发。
- **verbatim 字节级对账 CRLF 假失配**（实测 2026-09-06，260906-gov-review-index-status-truth-source 批）：`.claude/scripts/landing-fidelity-check.mjs:154` `if (content.includes(block))` 逐字节比对（R1-F6 行号核准）——仓内 .mjs 工作树全量 CRLF（`git ls-files --eol` = i/lf w/crlf）、PROPOSAL.md 经 Write 落盘 LF，`includes` 必假；2 条 advisory 经字节级对账止于行尾差异（实质零漂移）。**根因** = 比对未做行尾归一。同闸 `checkCitationAnchors`（`landing-fidelity-check.mjs:71`）对 §6 裸文件名引用（无 `.claude/scripts/` 前缀）报 file-not-exist，三文件实存。**根因** = 引据解析无前缀补全。
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

`checkVerbatimQuotes`（`landing-fidelity-check.mjs:135`）比对前双侧行尾归一：`content.includes(block)` 改为 `content.replace(/\r\n/g, '\n').includes(block.replace(/\r\n/g, '\n'))`。`runVerbatimQuotesSelfCheck` 补 CRLF 用例（LF 围栏块 × CRLF 目标文件 → 命中，即 missing 不新增）。`checkCitationAnchors`（:71）对 file-not-exist 结论追加前缀重解析：候选前缀 `.claude/scripts/`、`.claude/scripts/lib/`、`.claude/hooks/` 依次试探（R1-F9 补 lib 子目录——共享库落 lib/，裸文件名引用两候选会试不中），命中即消警并在 problems 输出附 `resolvedByPrefix` 注记（不静默——消警要有痕）。为什么：两类假失配都属「实质零漂移/实存文件被判失踪」，归一与补全都比对账纪律更便宜；消警留痕防真失踪被前缀试探掩盖。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`。

### T3 · merge-driver 活图守护升内容级：ensureGraphCacheSynced

`ensureSkipWorktree`（`merge-driver.mjs:705`）升级为 `ensureGraphCacheSynced`，对 `CODEBASE_MEMORY_DIRTY_PATHS` 两件执行幂等三拍：① `git ls-files -v` 查旗标，S 在册者先 `git update-index --no-skip-worktree`；② `git checkout HEAD -- <path>` 把内容还原到 HEAD 快照（**index 与工作树双还原**——裸 `git checkout --` 只还原 worktree ← index，覆盖不了「renormalize 已把两件内容带入 staging」的实况（ARQ 证据一），rebase 对 staged 改动照样 fail-closed，R1-F2）；③ `git update-index --skip-worktree` 复旗标。stderr 留痕一行（`N content-restored, M flag-only`），失败 advisory-only 永不阻塞合并（承 T6 既有语义）。rebase 步（:778）与 merge 步两调用点不变。头注释补 `.gitattributes` 三步序交互注记：「renormalize 会清 skip-worktree 旗标（ARQ-260907-renormalize-clears-skip-worktree 证据一），凡跑 renormalize 后依赖本函数幂等复位；本函数已升内容级，旗标与内容（含 staging）双兜底」。`--self-check-worktree-topology` 补断言：临时夹具内对被跟踪文件改内容 + 设旗标，跑 `ensureGraphCacheSynced` 后断言内容与 HEAD 一致且旗标复位；**另补 staged 漂移用例**（改内容并 `git add` 入 staging，断言还原后 staging 与 HEAD 一致，R1-F2）。rebase 中断堆积重复 pick 的次生坑以头注释成文（清 todo 指引），不改 rebase 逻辑本体。为什么：包 2 实证旗标级守护不足（ARQ 补证「候选②不成立」），内容级还原是实测验证过的修复，且还原语义须覆盖 staging 漂移形态。

目标文件：`.claude/scripts/merge-driver.mjs`。

### T4 · friction 事件最小 schema：聚合器显式告警 + 成文

`pod-events-aggregate.mjs` `aggregateFriction`（:135 起）增加残缺事件显式列 `malformedFrictions`：payload 空对象 / 缺 `frictionType` / 缺 `summary` 三判据，命中即入列（含 changeId + eventType 侧定位信息），不静默归 unknown；既有 `unknown` 兜底保留（非标键仍归 unknown，但残缺件同时出现在 malformedFrictions 列——告警与归档双轨）。无 self-check 入口（该脚本现无自检），验证走真数据重放：对 `.git/devloop/pods/` 现存 pod-events 跑聚合，断言 260829-voice-roundtrip 的 fb2c70c9 / 55a43813 与 260903-device-ws-switch 非标键两条全部入列（§5 V4）。schema 成文进 `docs/DEV-GOVERNANCE.md`（「friction 事件最小 schema」小节：`frictionType` + `summary` 必填、附加细节允许扩展键、空 payload = 落账缺陷）。planner def 引用行（OQ2 条件锚）。

目标文件：`.claude/scripts/pod-events-aggregate.mjs`、`docs/DEV-GOVERNANCE.md`、`.claude/agents/planner.md`（引用行，OQ2=(a) 已裁）。

### T5 · 验收纪律 §4：C 类冻结验收实编

`docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节新增 §4「C 类冻结验收实编」：C 类（固件/结构面）冻结验收落盘时，acceptance-author 当场实编一次该验收文件（error 为零），红基线明示「验收文件本身可编译」判据；与 §3 冻结即冒烟并列（§3 管执行通道，§4 管编译通道）。`acceptance-author.md` References 节加一行引用（「（T5，260907 批）C 类验收落盘前必读 §4」形态，承 260906 批 T8 同款 +1 引用行先例）。为什么：host-test-infra 实证「红基线从未实编过验收文件」，§3 通道清单不含编译。

目标文件：`docs/DEV-GOVERNANCE.md`、`.claude/agents/acceptance-author.md`（引用行，OQ2=(a) 已裁）。

### T6 · 起包格式期望前置清单

`docs/DEV-GOVERNANCE.md` 新节「起包格式期望前置」（验收测试纪律旁），五子项一一对应 ARQ 行五证据：① Draft TaskPackage 带 mutationEvidence 骨架（空骨架也行，字段必须在场）；② acceptance 断言格式期望（机器可识别断言形态 + coverage 标记随测试名）写进 acceptance-author 派发默认；③ 验收 ID 禁括号后缀（`(B#)` 形态破坏 `\b` 匹配）；④ 资产/规划类包 mutation/docs-sync 降级声明在 Draft 阶段写字段；⑤ 新约定包 dispatch 时同步查相关机械闸判据。planner / portfolio-planner / builder / acceptance-author 四 agent def 各加一行 References 引用（OQ2 条件锚）。为什么：格式期望只活在闸代码里、起草侧零成文，4 包 5 子项同型红是结构性回派税。

目标文件：`docs/DEV-GOVERNANCE.md`、`.claude/agents/planner.md`、`.claude/agents/portfolio-planner.md`、`.claude/agents/builder.md`、`.claude/agents/acceptance-author.md`（后四件引用行，OQ2=(a) 已裁；planner 在 T4/T6 各 +1 共 +2，acceptance-author 在 T5/T6 各 +1 共 +2）。

### T7 · ADR canonical 落点：docs 根单文件决策日志（human OQ3 自裁）

human 2026-09-07 自裁「放 docs 根目录、和 spec 同级」→ 落 **`docs/ADR.md` 单文件决策日志**（与 `spec.md` / `DEV-GOVERNANCE.md` / `ARCH-REVIEW-QUEUE.md` 同级的平铺注册表形态，承本仓单文件 markdown 惯例；不建 `docs/adr/` 子目录）。落地四件：

1. **新增 `docs/ADR.md`**：头节声明性质（架构决策记录 = 「选了什么 / 为什么 / 放弃了哪些备选」的长期留档，区别于 `changes/**` 过程证据与 `docs/notes/` 经验记录）+ 条目格式约定（每条 = 编号 ADR-NNN / 日期 / 决策一句话 / 理由 / 备选与放弃原因 / 关联 change 或制品锚；append-only，不覆盖既有条目）。骨架入库时不预填任何决策条目（存量散落的决策不做考古搬迁，自新决策起生效——留根见 §6）。
2. **`.claude/scope-map.json`**：`universal.docsTopLevel` 白名单加 `docs/ADR.md`（enforce-scope 按全路径判定，R1-F10）；integrator 角色 allow 面加 `docs/ADR.md` 写槽（dev-loop 收口时落新增条目）。
3. **`.claude/rules/repo-structure.md`**：docs 根白名单文件列表加 `ADR.md`（与 scope-map 双向锚定，enforce-scope 机判一致）；顶层地图 `docs/` 根行说明同步。
4. **`docs/DEV-GOVERNANCE.md`** 入口速查或角色编制节补一句指针（integrator 收口产 ADR 条目 → `docs/ADR.md`）。

为什么交 human 且采纳自裁结果：scope-map 角色编制与 docs 根白名单都是结构边界动作（repo-structure.md「有意识动作」条款）；human 选 docs 根平铺而非子目录，与本仓契约/队列注册表的单文件惯例一致。

目标文件：`docs/ADR.md`（新增）、`.claude/scope-map.json`、`.claude/rules/repo-structure.md`、`docs/DEV-GOVERNANCE.md`。

### T8 · PROPOSAL 模板钉死机械门可辨形态

`docs/templates/PROPOSAL.template.md` 硬项 2a 段末追加两句：「改法标题一律 `### T<n> · <标题>` 标题形态——机械门 `parseSection2AnchorLists` 只认 `### T<n>`（260906 批 FROZEN 后格式返修实证，粗体段形态不被机辨）；landing-manifest 围栏块一行一个裸路径，不带括注（`extractManifestBlock` 逐字取行）。」为什么：机械门契约是 drafting 相位输入约束，模板是起草侧唯一强制阅读面，零脚本改动消一整类返修。

目标文件：`docs/templates/PROPOSAL.template.md`。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 闸共享库 | `.claude/scripts/lib/governance-gate-shared.mjs` | G3 | T1 新增导出；消费方 = landing-fidelity-check 两处（:386 锚提取切换、:367 changedNotInProposal 面不动）；全仓 CANONICAL_TEST 仅此两消费点，proposal-consolidation-check 不消费（R1-F8 全消费点核对） |
| 结构闸 | `.claude/scripts/landing-fidelity-check.mjs` | G3 | T1/T2：锚过滤切换、verbatim 归一、citation 前缀补全、自检三处扩展；advisory 列结构不变（problems 字段追加字段，消费方 = landing-verifier 人读） |
| 合并通道 | `.claude/scripts/merge-driver.mjs` | G3 | T3：ensureGraphCacheSynced 替换 ensureSkipWorktree（两调用点语义升级，还原覆盖 worktree+staging 双面）；自检扩展；既有五自检不回退 |
| 聚合器 | `.claude/scripts/pod-events-aggregate.mjs` | G3 | T4：aggregateFriction 加 malformedFrictions 列；消费方 = loop-retro / 主线程人读，纯增量字段 |
| 治理成文 | `docs/DEV-GOVERNANCE.md` | G2 | T4/T5/T6/T7 四节新增/扩写；行为保持（成文面） |
| 模板 | `docs/templates/PROPOSAL.template.md` | G2 | T8 硬项 2a 段末追加两句 |
| agent def | `.claude/agents/{acceptance-author,planner,portfolio-planner,builder}.md` | agent def | T4/T5/T6 引用行（合计 +6：acceptance-author +2 / planner +2 / portfolio-planner +1 / builder +1；职责结构零改动，OQ2=(a) 已批） |
| 结构边界 | `.claude/scope-map.json`、`.claude/rules/repo-structure.md` | G3/G2 | T7：scope-map（G3）docsTopLevel 加 `docs/ADR.md` + integrator allow 写槽；repo-structure（G2）白名单行——两件常设锚（OQ3 已裁） |
| ADR 日志 | `docs/ADR.md` | G2 | T7 新增骨架（性质声明 + 条目格式约定，零决策条目预填）；writer = integrator（收口）+ 主线程（治理批 landing）——architect 不入 allow 面，白名单语义下不落即不写，治理批 ADR 条目由主线程 landing 代落（R1-F5） |
| 未触 | `docs/contracts/**`、`docs/spec.md`、`firmware/**`、`server/**`、`.claude/settings*.json`、`verify-gates.json` | 未触 | 本批零触点（§0 非目标） |

**【硬项 1b census · R1 判官代跑 2026-09-07】** ANCHORABLE_TEST 判定 pattern 全仓预跑（`git ls-files` 过新正则）：仓根点文件命中 `.cbmignore` / `.gitattributes` / `.gitignore` 三件、`changes/` 前缀 126 条，与 T1 设计预期一致（判官 R1 亲验在册，原始输出归 landing-summary）；T2 归一 pattern 全仓无扫描面（运行时双侧归一）；T4 告警 pattern 预跑 = §5 V4 真数据重放。

## 4. human 取舍点（OQ）

- **OQ1 · 批次范围**：(a) T1–T8 八项全收（推荐——七 ARQ + 模板钉死同根同批，前例 260906 批十项一批先例，分批徒增评审开销）；(b) 收 T1–T6 + T8、T7（ADR 落点）留队列（若想隔离结构边界变更）；(c) 只收机械件 T1–T4（闸侧自洽批，成文面留下批）。**裁断：human 2026-09-07 裁 (a)。**
- **OQ2 · agent def 引用行授权**：T4/T5/T6 涉四件 agent def 各 +1 行 References 引用（承 260906 批 OQ3=(b)「agent def 只加引用、职责/工具面结构零改动」human 已批先例）：(a) 批准（推荐——起草 agent 不经 planner 中转直接可见期望，回派税的根在 agent 侧视野）；(b) 只成文 DEV-GOVERNANCE、不动 agent def（期望可见性降级，回派风险残留）。**裁断：human 2026-09-07 裁 (a)。**
- **OQ3 · ADR canonical 落点**：(a) 新 `docs/adr/` 目录；(b) 维持 `changes/dev/` 落 + 主线程晋升迁移惯例成文；(c) 本批不裁。**裁断：human 2026-09-07 经「ADR 是什么」名词澄清后自裁第三案——「放 docs 根目录、和 spec 同级」→ 落 `docs/ADR.md` 单文件决策日志（T7 按此改写，见 §2 T7）。**

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1** `node .claude/scripts/landing-fidelity-check.mjs --self-check` → `ok:true` 且自检名清单含新增（CRLF verbatim 用例、ANCHORABLE_TEST 锚用例各一）。
- **V2** T1 预跑 census：`git ls-files | node -e "<ANCHORABLE_TEST 过滤>"` → 仓根点文件 + `changes/**` 命中清单在册（landing-summary 附命令与输出）。
- **V3** T3 自检：`node .claude/scripts/merge-driver.mjs --self-check-worktree-topology` → `pass:7 fail:0 ok:true`（原 5 断言 + 内容级还原 + staged 漂移新断言）；既有四自检（retro-advisory / quick-checks / detect-changes / rebase-target）全 ok 不回退。
- **V4** T4 真数据重放：`node .claude/scripts/pod-events-aggregate.mjs --change-id <id>` 逐包跑（脚本从 `.claude/taskpackages/<id>/pod-events.jsonl` 读、单包一跑，R1-F3 入口核准）：260829-voice-roundtrip → fb2c70c9 / 55a43813 两条命中 malformedFrictions；260903-device-ws-switch → 非标键两条（b2d0d32c / 05957d1c）命中；260831-ui-art-assets / 260829-session-logic 跑通不崩（其失真形态 = 缺事件，ledger=0/retro=N 型，机器侧不可判、留人读对照）。重放先断言输入文件存在非空（真空绿规约）。
- **V5** T4/T5/T6/T7 成文核验：DEV-GOVERNANCE 新节标题 grep 在场；`docs/ADR.md` 骨架在场（性质声明 + 条目格式约定）；`scope-map.json` `docsTopLevel` 含 `ADR.md`；`repo-structure.md` 白名单行在场。
- **V6** T7 机检一致性：enforce-scope 对 `docs/ADR.md` 的 integrator 写槽放行冒烟（或以 scope-map JSON 有效性 + 双向锚定 grep 核验代替，如实记录所用口径）。
- **V7** `node --check` 对四件 .mjs 全过。
- **V8** landing 后结构门 `node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-arq-governance-batch-2` → 锚集 ≡ manifest（本批落点全部 canonical/ANCHORABLE 可机辨——T1 自证）；verbatim advisory 零 missing（CRLF 归一自证——§2 围栏块如有）。
- **V9** agent def 引用行核验（OQ2=(a) 已裁）：四件 def diff 行数 = acceptance-author.md +2（T5 纪律引用 + T6 起包清单引用）、planner.md +2（T4 schema 引用 + T6 起包清单引用，R1-F4 核准）、portfolio-planner.md +1、builder.md +1，合计 +6；职责/工具面结构零改动。

## 6. 留根清单

- **不关的根**：五项非 governance ARQ 留队列（multiturn-continuity / p4-stroke-dasharray-visual-check / v23-asr-swap-quota-recheck / protocol-version-pinned-constant / nvs-csv-wifi-injection），处理时机各在 ARQ 行（部署首验 / P4 验收 / 配额重置补验 / dev-loop 固件包 / dev-loop 配网包），本批不代处理。
- **交日项（承 260906 批 retro §6）**：「包 2 是否踩 T6/T7/T8」的复盘结论随本批 landing-summary 记账——T6 踩（内容漂移实证，T3 承接修复）、T7 部分踩（pod-retro 仍录 `tr -d '\r'` 绕行，疑 pod worktree 先于 T7 合并创建，checkout 代次问题成文备注）、T8 遵循度（包 2 acceptance-author 是否引用 DEV-GOVERNANCE 纪律）核对后一并记录。
- **登记不硬治项**：rebase 中断重复 pick 清 todo 指引以 merge-driver 头注释成文（T3 内），不做 rebase 状态自动清理（场景过窄）。
- **自律件**：本批 PROPOSAL 自身即按 T8 钉死形态起草（`### T<n>` 标题 + 裸路径 manifest），起草期自证模板句可执行。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1（2026-09-07 · architect 判官 · Track B · verdict: NEEDS-REVISION）

**判官身份声明**：本 R1 由 architect 子 agent 以纯判官身份执行——只读 canonical 与本批真值源（七个被改脚本源码、scope-map、repo-structure、上一批先例制品、ARQ 队列七行原文），只写本节 append；未落地任何改法、未跑任何会变更工作树的命令（merge-driver 仅读源码，未执行任何自检入口）。**路由判断**：本批零契约触点（`docs/contracts/**` 不在 §2/manifest 任何改法内）、非跨系统结构性变更（固件协议与服务端语义均不动）→ Track B 常驻轻路径路由正确，不命中升级判据。

**结构硬条款核对**：landing-manifest 围栏块在场（13 行裸路径、一行一件、无括注——恰 T8 钉死形态）；packet-id 行在场；§2 八个 T 均有「目标文件：」锚清单且 backtick 包裹齐备；承重 landing 指令原文钉死（T1 的 `CANONICAL_TEST.test(...)` → `ANCHORABLE_TEST.test(...)` 切换、T2 的 `content.includes(block)` → 双侧归一改写、T3 三拍命令序列均给了原文级替换式，无「那行」类两读指称）。**但机械对账层面锚并集 ≢ manifest（F1）**，见下。

**硬项 1a 实证纪律抽验**：行号引用抽验 12 处，11 处精确命中（`governance-gate-shared.mjs:34` CANONICAL_TEST 原文、`landing-fidelity-check.mjs:71/:135/:186/:367/:386`、`merge-driver.mjs:705/:778/:873`、`pod-events-aggregate.mjs:139`、`DEV-GOVERNANCE.md:263/:280`、`PROPOSAL.template.md:49`），仅 1 处漂移（F6）；「比照先例」四处均读原文核对一致（260906 批 retro §4 建议 1 与 F1 的标题/manifest 形态记述、该批 T6 整裁腿形态、T8 引用行先例、ARQ-260907-renormalize 行补证段）；friction 重放证据亲验在册（`pod-events.jsonl` 中 fb2c70c9/55a43813 payload 实为 `{}`、260903-device-ws-switch 两条非标键原文比对）。总体实证纪律良好。

**Findings**：

- **F1 · NEEDS-REVISION · 条件锚注记与已裁 manifest 满配自相矛盾，V8 结构门必 RED**。OQ2 已裁 (a)（agent def 引用行批准），manifest 为 13 件满配形态，但 T4/T5/T6 的 agent def 锚行仍带「（OQ2 条件锚）」注记。`parseSection2AnchorLists`（`.claude/scripts/lib/governance-gate-shared.mjs:410`）对路径后注记含「条件」者裁出不入并集——机械解析下 `.claude/agents/acceptance-author.md` 在 T5 与 T6 两处均被裁出（T6 的「后四件 OQ2 条件锚」注记在机器上只裁掉最后一件：planner/portfolio-planner/builder 因「、」分隔注记不含「条件」而保留），锚并集（12 件）≢ manifest（13 件），`compareAnchorSets` 将报仅 manifest 有 `acceptance-author.md` → V8「锚集 ≡ manifest」必假红。这正是本批 T8 要消灭的 landing 相位格式返修类，§6 自律件「起草期自证模板句可执行」被此实失证伪。且 T7 已按 OQ3 裁断把条件锚转常设锚、T4/T5/T6 未同步——同批两种处理不一致。**建议改法**：OQ2 既已裁 (a)，把 T4/T5/T6 锚行注记改为无条件形态（如「（OQ2=(a) 已裁）」——注记不含「条件」「或」「迁出」字样），使锚并集 = manifest = 13 件；§3 表 agent def 行的「OQ2 条件锚」字样同步改「OQ2=(a) 已裁」。
- **F2 · NEEDS-REVISION · T3 三拍的 `git checkout -- <path>` 对「renormalize 残留已入 staging」场景不成立**。`git checkout -- <path>` 只把 worktree 还原成 **index** 内容；而 ARQ-260907-renormalize 证据一的实况是旗标被清（H）+ 两件挂脏**且被带入 staging**——此时三拍走下来：①查旗标见 H 不清、②checkout -- 把 worktree 还原成 index 里的脏内容、③复旗标，index 相对 HEAD 仍有 staged 改动，rebase 照样拒绝。T3 头注释成文却宣称「凡跑 renormalize 后依赖本函数幂等复位……本函数已升内容级，旗标与内容双兜底」——宣称覆盖的恰是这个场景。**建议改法**：第②拍改 `git checkout HEAD -- <path>`（或 `git restore --source=HEAD --staged --worktree -- <path>`，同时还原 index 与 worktree）；`--self-check-worktree-topology` 新断言补一个 staged-drift 用例（改内容 + `git add` 后跑三拍，断言 index 与 worktree 均 == HEAD 且旗标复位）。「还原到 HEAD 快照」的措辞也应照此改准（plain checkout -- 的语义是「还原到 index」）。
- **F3 · NEEDS-REVISION · V4 重放方法与脚本实际入口不符**。`pod-events-aggregate.mjs:157` 读的是 `.claude/taskpackages/<changeId>/pod-events.jsonl`（经 `--change-id <id>` 单包一跑），不读 `.git/devloop/pods/`——该目录实测只放 pod JSON 注册表（`260829-voice-roundtrip.json` 等 16 件），事件本体在 taskpackages 侧。T4 正文与 V4 均写成「对 `.git/devloop/pods/` 现存 pod-events 跑聚合」，照写跑不起来；且两包断言需两跑。硬项 1a 要求引用脚本入口先亲核。**建议改法**：V4 改为两跑 `node .claude/scripts/pod-events-aggregate.mjs --change-id 260829-voice-roundtrip` 与 `--change-id 260903-device-ws-switch`，输入面如实写 `.claude/taskpackages/<id>/pod-events.jsonl`；断言不变（四条残缺事件的 payload 原文判官已亲验在册，可判）。
- **F4 · NEEDS-REVISION · planner.md 加两行但 V9 断言「各恰 +1 行」**。T4 与 T6 各给 planner.md 加一条 References 引用行（friction schema + 起包期望），按 §2 改法 planner 应 +2 行；V9 却断言「四件 def diff 各恰 +1 行」，照跑必假红或逼 verifier 现场改判据。§3 表 agent def 行「各 +1 行 References」同病。**建议改法**：V9 改为「每件 def 的 References 新增行数恰等于 §2 涉它的 T 数（planner = 2；acceptance-author / portfolio-planner / builder 各 = 1）、职责/工具面结构零改动」，§3 表同步。
- **F5 · NEEDS-REVISION · docs/ADR.md 的 writer 声明与 architect scope-map allow 面矛盾**。§3 表 ADR 行写「writer = integrator（收口）+ architect（治理批）」，但 T7 的 scope-map 改法只加 `universal.docsTopLevel` 与 integrator allow，不动 architect 条目；architect 是 allow 白名单语义（`.claude/scope-map.json:82-96`，allow 非空且未命中即拦截），`docs/ADR.md` 不在 architect allow → enforce-scope 必拦 architect 子 agent 写它——恰是本 ARQ 原病灶（integrator 被 enforce-scope 拦）在 architect 侧的复刻。**建议改法**（二选一，交主线程/human 定）：① T7 scope-map 改法补一条 architect allow 加 `docs/ADR.md`；② §3 表 writer 收窄为「integrator（收口）+ 主线程（治理批 packet-bound landing）」——按「landing 恒主线程」常态规则，②与现行治理模型自洽性更好，倾向 ②。
- **F6 · NIT · 引用行号漂移一处**：§1 类 A 引 `landing-fidelity-check.mjs:152` 的 `content.includes(block)`，实际在 :154（:152 是 `let content;`）。修订时顺手改准即可（citation 锚为 advisory 面，不阻塞）。
- **F7 · NIT · §3 表把 `.claude/scope-map.json` 标为 G2**：按 `docs/DEV-GOVERNANCE.md` §6 治理面四分类，scope-map 属 G3 强制执行层；头部 review profile 已如实写 G2+G3，仅表内类别一格标错，修订时改 G3。
- **F8 · NIT · proposal-consolidation-check 非 CANONICAL_TEST 消费方**：全仓 grep `CANONICAL_TEST` 仅两消费点（`landing-fidelity-check.mjs:367` changedNotInProposal + `governance-gate-shared.mjs:386` 锚过滤），proposal-consolidation-check 不消费该常量（它是共享库其它导出的消费方）——§3 表「消费方 = … proposal-consolidation-check（不受影响）」结论对、归因错。T1 的核心设计声明（锚提取向宽用 ANCHORABLE_TEST、`changedNotInProposal` 检测面维持 CANONICAL_TEST 不动、防 changes/** 常规写入误报 changed-not-in-proposal）经全消费点核对**成立**，两职责分离判断正确。
- **F9 · NIT · T2 citation 前缀候选缺 `.claude/scripts/lib/`**：本仓共享库已在 `lib/` 子目录（governance-gate-shared.mjs / changeid-shared.mjs），裸文件名 `governance-gate-shared.mjs:34` 这类引用用 `.claude/scripts/` / `.claude/hooks/` 两前缀都试不中。建议候选前缀补第三个 `.claude/scripts/lib/`。
- **F10 · NIT · T7 scope-map 写法形态不精确**：`universal.docsTopLevel.allowFiles` 现行条目是全路径文件名形态（`docs/spec.md` 等），`enforce-scope.mjs:60` 按 rel 全路径 includes 判定——T7 措辞「白名单加 `ADR.md`」应写「加 `docs/ADR.md`」防落地时写成裸名形态。repo-structure.md 侧白名单是裸名形态（`spec.md` 等），加 `ADR.md` 即可——两处形态本不同向，建议 T7 措辞分别点明照各自现行形态落。
- **F11 · NIT · 硬项 1b census 证据后置**：模板硬项 1b 要求判定 pattern 全仓预跑的原始输出（或摘要 + 命令 + 日期）进 §3 证据段，本稿把输出时点后置到 landing-summary。判官已代跑核可行性（只读）：`git ls-files` 过滤无斜杠行 = `.cbmignore` / `.gitattributes` / `.gitignore` 三件仓根点文件，`changes/` 前缀 126 条——与 §3 预期一致、无意外形态（`.claude` 系多段路径不会被单段点文件正则误中）。建议修订时把此 census 结果（命令 + 计数 + 日期）落进 §3 证据段，满足硬项原文。

**影响面 sweep 完备性**：§3 表 UNCLASSIFIED=0 成立；manifest 13 件全部入表且有对应 T；非目标声明（契约面 / spec / verify-gates / 实现面零触点）与 §2 实际改法一致，核对无违。

**OQ 裁断落稿一致性**：OQ1=(a) 八项全收 → §2 T1–T8 齐；OQ3 → T7 docs/ADR.md 单文件日志四件落法齐；OQ2=(a) → manifest 满配含四件 agent def，但锚行注记未随裁断更新（F1）——裁断与 manifest 一致、与 §2 锚行形态不一致。

**verdict：NEEDS-REVISION（打回修订）**——F1–F5 五条 NEEDS-REVISION 级 finding 在册（F1 为最重：机械门自证必假红；F2 为 T3 技术方案对宣称覆盖场景不成立）。修订后请回 R2，R1 只复验 F1–F5 改正与 F6–F11 顺手项，不重开全量评审。
