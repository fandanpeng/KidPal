# architect-landing-summary · 260907-gov-review-arq-governance-batch-2

packet-id：FROZEN PROPOSAL §2 改法集（T1–T8）+ §5 验证集 · 评审轮次 R1 NEEDS-REVISION → 主线程修订 → R2 GREEN（N1–N3 限定折入）· change-id 260907-gov-review-arq-governance-batch-2

> 落地人：主线程（Track B 步骤 4，packet-bound）· 日期：2026-09-07
> packet = FROZEN PROPOSAL + landing-manifest + 本 summary（architect-landing-summary）
> 凭据 = human 立项 ack（「开始review吧」2026-09-07）+ OQ 三题裁断（OQ1=(a) / OQ2=(a) / OQ3=human 自裁 docs/ADR.md 单文件日志）+ architect R2 GREEN

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

> manifest 注：13 件满配形态（OQ 全裁后无条件锚）；landing 后机械门以 ANCHORABLE_TEST（T1 新增）机辨仓根点文件与 changes/ 形态——本 summary 围栏块即 T1 的首个真实消费实例。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 ANCHORABLE_TEST 锚形态拓宽 | `.claude/scripts/lib/governance-gate-shared.mjs`（ANCHORABLE_TEST 新增 + :386 过滤切换）+ `.claude/scripts/landing-fidelity-check.mjs`（section2Anchors 自检 +2 用例） | ✅ 已落：仓根点文件（`.` 单段）与 `changes/` 前缀入锚集；changedNotInProposal 检测面（:367）CANONICAL_TEST 不动 |
| T2 verbatim CRLF 归一 + citation 前缀重解析 | `.claude/scripts/landing-fidelity-check.mjs`（checkVerbatimQuotes 归一 + checkCitationAnchors 前缀试探 + 两自检扩展） | ✅ 已落：resolvedByPrefix 独立观察列（不混 checked）；前缀候选 `.claude/scripts/` / `.claude/scripts/lib/` / `.claude/hooks/` |
| T3 ensureGraphCacheSynced 内容级还原 | `.claude/scripts/merge-driver.mjs`（ensureSkipWorktree → ensureGraphCacheSynced，两调用点 :800/:895 同步更新；拓扑自检 +4 断言；头注释补 renormalize 操作序注记 + rebase 重复 pick 清 todo 指引） | ✅ 已落：三拍 = no-skip-worktree → `git checkout HEAD --`（index+worktree 双还原）→ 复旗标；失败 advisory-only |
| T4 friction 最小 schema | `.claude/scripts/pod-events-aggregate.mjs`（malformedFrictions 列）+ `docs/DEV-GOVERNANCE.md`（「## friction 事件最小 schema」节）+ `.claude/agents/planner.md`（+1 引用行） | ✅ 已落：空 payload / 缺 frictionType / 缺 summary 三判据显式点名 |
| T5 验收纪律 §4 C 类实编 | `docs/DEV-GOVERNANCE.md`（「### 4. C 类冻结验收实编」）+ `.claude/agents/acceptance-author.md`（+1 引用行） | ✅ 已落：编译通道 error 为零判据，与 §3 并列不互替 |
| T6 起包格式期望前置 | `docs/DEV-GOVERNANCE.md`（「## 起包格式期望前置」五条清单）+ 四件 agent def（planner/portfolio-planner/builder/acceptance-author 引用行） | ✅ 已落：五子项逐条对 ARQ 行五证据 |
| T7 ADR canonical 落点 | `docs/ADR.md`（新增骨架）+ `.claude/scope-map.json`（docsTopLevel + integrator allow 槽）+ `.claude/rules/repo-structure.md`（docs 根白名单行）+ `docs/DEV-GOVERNANCE.md`（§7 入口速查 ADR 行） | ✅ 已落：单文件日志形态，条目格式 append-only 约定在文件头，零决策条目预填 |
| T8 模板钉死 | `docs/templates/PROPOSAL.template.md`（硬项 2a 段末两句） | ✅ 已落：`### T<n>` 标题形态 + manifest 裸路径形态成文 |

manifest 外零落地：契约面（`docs/contracts/**`）、`docs/spec.md`、自有实现面（firmware/server）、verify-gates.json 均未触碰（FROZEN §0 非目标）；四件 .mjs 语法全过（V7）。

## 2. 验证证据（§5 重放，2026-09-07）

landing 中（执行侧自检）：

- **V1** `node .claude/scripts/landing-fidelity-check.mjs --self-check` → section2Anchors `pass:19 fail:0`（原 17 + 仓根点文件锚 / changes 制品锚 2 新用例）、citation-anchors `pass:5 fail:0`（原 2 + 前缀重解析 3 断言）、verbatim-quotes `pass:6 fail:0`（原 4 + CRLF 用例 2 断言），全 ok ✅
- **V2** ANCHORABLE_TEST census（`git ls-files` 过形态）：`changes/` 前缀 126 条、仓根点文件 `.cbmignore` / `.gitattributes` / `.gitignore` 三件——与 R1 判官代跑 census 一致 ✅
- **V3** merge-driver 五自检：`--self-check-worktree-topology` → `pass:9 fail:0 ok:true`（原 5 + 形态一 content-drift 2 断言 + 形态二 staged-drift 2 断言）；retro-advisory 3/3、quick-checks ok、detect-changes 16/16、rebase-target 7/7 全绿零回退 ✅
- **V4** T4 真数据重放（`--change-id` 逐包）：260829-voice-roundtrip → fb2c70c9 / 55a43813 命中 `empty-payload`；260903-device-ws-switch → b2d0d32c / 05957d1c 命中 `missing-frictionType`+`missing-summary`；260831-ui-art-assets 跑通不崩（friction 0，缺事件形态留人读）；260829-session-logic 跑通不崩（1 条 friction 完好不入列）✅
- **V5** 成文核验：DEV-GOVERNANCE 三新节标题 grep 计 3 命中；`docs/ADR.md` 骨架在场；`scope-map.json` `docs/ADR.md` 计 2 命中（allowFiles + integrator allow）；repo-structure 白名单行在场 ✅
- **V6** scope-map JSON parse 有效（`JSON.parse` 实测）✅
- **V7** `node --check` 四件 .mjs 全过 ✅

landing 后（核验侧）：

- **V8** 结构门：`node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-arq-governance-batch-2` —— 锚集 ≡ manifest 断言首次可机判（T1 自证：13 件全部 ANCHORABLE 可辨）；verbatim advisory 零 missing（T2 自证）→ 由 landing-verifier 独立复跑确认
- **V9** agent def 引用行核验：diff stat = acceptance-author +2 / builder +1 / planner +2 / portfolio-planner +1，合计恰 +6（初版 acceptance-author 误合成 1 行已当场拆开对齐 FROZEN 计数）✅

## 3. 诚实注记

1. **V3 断言计数超 FROZEN 预估**：FROZEN V3 写「pass:7」（原 5 + 两形态各 1 断言），实际落地为 pass:9——内容还原与旗标复位在两形态下各自独立断言（每形态 2 条：还原生效 + 旗标复位）。断言集合完整覆盖 FROZEN 声明的两形态，计数差异 = FROZEN 起草期对断言粒度的预估低估，非设计变更；如实登记请 verifier 复核。
2. **`mergedInMain=yes` 提前钉入的实况**：changes-index upsert（FROZEN 钉行时）现推 `gitContainedInMain` 得 yes——本批 PROPOSAL 草稿 commits（d305c43/c4ef232）已随并发会话的 `260907-architect-dispatch-model-override` landing ff-merge 进入 main 历史（该会话在我治理分支上落了 1c46aa1 并 ff main，topology 插曲见注 3）。脚本语义下「change 目录史已入 main」为真、非误账；本批 landing commits 的合并仍在 Track B 步骤 7。
3. **会话拓扑插曲（如实登记）**：R1 评审期间并发会话在同一 checkout 上切 main 落了 `260907-architect-dispatch-model-override`（其 landing commit 1c46aa1 骑在本批治理分支上、ff 进 main），本批 v0.3 修订 commit 89ca380 误落 main——已修复：89ca380 cherry-pick 回治理分支（55abc92）、main reset 回 9e84869，分支拓扑复原后再续 R2。
4. **V9 当场修正**：acceptance-author 引用行初版合成 1 行（FROZEN 钉 T5/T6 各 1 行），验证时发现即拆开，终态与 FROZEN 计数逐件一致。
5. **交日项结论（260906 批 retro §6 挂账）**：包 2（260829-voice-roundtrip）launch/合并实测**踩了 T6**（skip-worktree 旗标完好仍被内容漂移挡 rebase 三次，升级为本批 T3 内容级根治）+ **T7 部分残余**（pod-retro 仍录 `tr -d '\r'` 绕行；定性 = 包 2 pod worktree 创建先于 T7 入 main，worktree checkout 代次问题——.gitattributes 只对后续 checkout 生效，存量 worktree 需 rm+checkout 重置，非 T7 规则本身缺陷）；T8 遵循度 = 包 2 acceptance-author 已引用 DEV-GOVERNANCE 验收纪律（pod-retro WSA 验收引用在册）。病灶未在本批前暴露的原因：T6 内容级缺口与 worktree checkout 代次均为真实合并/构建才触发的环境形态，设计评审不可达。
6. **friction 重放计数口径**：V4 四包断言仅覆盖「逐事件可机判」形态（空 payload / 缺键）；「缺事件」形态（ui-art-assets ledger=0/retro=N 等）机器侧不可判，按 FROZEN 口径留人读对照，不虚报机判覆盖。

## 4. 状态转移记录

- 2026-09-07 human 立项 ack（「开始review吧」）+ OQ 三题裁断（OQ1=(a) / OQ2=(a) / OQ3=human 自裁 docs/ADR.md 单文件日志）。
- 2026-09-07 architect R1 NEEDS-REVISION（F1–F5 修订级 + F6–F11 NIT）→ 主线程修订（v0.3，commit 55abc92）→ R2 GREEN（N1–N3 NIT 限定折入）→ 主线程凭「立项 ack + GREEN」标 **FROZEN v1.0**（commit e81c600）；`changes-index.mjs --upsert` 钉 frozen。
- 2026-09-07 主线程 packet-bound 落地 T1–T8（§1 清单，13 文件）→ V1–V9 全绿 → 本 summary 写就，交 landing-verifier 独立核验（Track B 步骤 5）。
- 待办（verifier 绿后）：ARQ 七项移 §2 已结 → retro（步骤 6）→ 前置升级闸 + 自动合并本地 main（步骤 7；remote push 留 human）。
