# architect-landing-summary · 261015-gov-review-changes-taxonomy

packet-id：FROZEN PROPOSAL v0.4 §2 改法集（T1–T6）+ §5 验证集 · 评审轮次 R4 GREEN（熔断后新判官实例）· change-id 261015-gov-review-changes-taxonomy

**终态声明（全量口径）**：本件为波段二终态 summary——核验范围 = 波段二落地 + 全 manifest 终态（83 件——landing 期 FAIL(F1) 补列 debt-ledger-shared.mjs 后）；波段一中间件（architect-landing-summary-w1.md / landing-verdict-w1.md）核验范围 = 波段一 7 件 + V1–V3，见在案历史件。本批落地序列 = 波段一（commit a8ee823，已合 main）+ 波段二（本批提交序列）。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1（波段一，a8ee823） | taskpackages 归位 4 件 + scope-map 齿一 + gate 齿二 + brief-inputs 夹具 | 已落地并独立核验（landing-verdict-w1.md PASS+3A），本表不再重复 |
| T2 · 生命周期三层语义 | `.claude/rules/repo-structure.md` | 顶层地图后新增「变更制品生命周期三层语义」节（诉求/证据/复盘三层 + 触发源判据条文 + idea 记录链四行纵向地图 + 比例化原则），全部按新树路径书写 |
| T3 · debt→intake 晋升 | `.claude/references/change-intake.md` + `docs/templates/debt-item.template.md` | §3a 晋升节（触面≥2 建 spine 引 debt id / 触面=1 直进各面入口 / 主动跳 debt / 结项 consumed-by 约定）+ §3 建档查重扩 debt 未结条目 + 模板结项行补 consumed-by 句 |
| T4 · companion 设计稿规范 | `.claude/references/change-intake.md` + `delivery/intake/261014-single-tier-bringlist-home.design.md`（改名平移）+ 同名 spine | §7a companion 节（需要判据/命名/免冻不设独立评审批）+ 261014 设计稿 .PROPOSAL.md→.design.md 改名 + spine 两处引用同步 + spine faceMap.contract/facePlan 契约行/顺序行刷现行口径（无人签门） |
| T5 · intake INDEX + 收敛 + 收口齿 | `.claude/scripts/changes-index.mjs`（intake lane + 漂移检测 + review lane 迁移）+ 齿五处（arch-change/gov-change/requirement-change 命令与流程 SSOT、integrator.md、build-feature.md）+ loop-retro.md 漂移核查行 + 261010/261001 spine 整改 | INDEX 投影 8 spine 全解析；漂移标 3 件如实（⚠未闭环 ×3）+ 2 件值域外标已随整改消除；齿语义 =「回写 spine anchors + status；写者恒 = 主线程」 |
| T6 · 伞解散切换 | 整树 git mv（arch 前移 1 批 → delivery/review/；intake 9 / retro 3 前移；changes 余树 → delivery/archive/changes/）+ 活引用改写（.claude/** + G1 + templates + notes + showme + spec 面 2 处档 0 勘误）+ archive/README.md 路标 + INDEX 切换（lane 键 arch→review、STATE_CACHE_PATH 迁址、归档指针行）+ 双齿扩展（scope-map denyCreate 伞级、gate RETIRED_MERGE_PATHS 伞级 + 自检臂）+ 锚白名单扩根（governance-gate-shared ANCHORABLE_TEST 四新根 + isProposalMdPath review 分支）+ 契约凭据候选正则 review 形态 | delivery/ 顶层现 = archive/intake/milestones/review/retro/taskpackages/workstreams 七目；changes/ 伞不在场 |

## 2. 验证证据

- **V2 · deny 双态（波段二态）**：Write 探针 `delivery/changes/probe.md` → PreToolUse 拦截，原文逐字：`scope(universal): 禁止写 delivery/changes/probe.md（.claude/scope-map.json universal.denyCreate——影子目录或受保护区）。` 探针未落盘。
- **V3 · 合并闸（波段二追加臂）**：gate --self-check 49/49（含 retired-path-umbrella-block 伞级 BLOCK 臂）；RETIRED_MERGE_PATHS = ['delivery/changes'] 在册。
- **V4 · 活树零残留（七类子检）**：P2 分段形 0、P4 裸 token 形 0；P3 转义形命中 = 归档路径/保留位/历史注记 + debt-ledger-shared.mjs :58 G4 谓词（活面漏改，verifier FAIL(F1) 裁——已按修法扩根落地并补列 packet，初版「全为归档/保留位」披露失准、本行随修）；P1 斜杠形 union-external = **0**——52 行命中全数归类：本批自引（PROPOSAL 31 + landing-verdict-w1 12 + summary-w1 8 = 51 行，证据件取证原文）+ gate 保留位 4（齿二注释与自检夹具，设计态）+ 豁免类（spine 历史叙述/refs、ADR/ARQ/CDQ/contracts、firmware red-baseline 2、server/evidence 1、§9 既有行、notes/showme 归档改指行）+ 历史件投影（taskpackages/INDEX.md origin.ref 列 9 行——由包内历史 taskpackage.json origin 字段再生，属包内历史件豁免的投影延伸）。V4 判据三分类的「本批自身引用」按证据件自引扩项执行（与 FROZEN 判据的偏差见 §3 注 2）。
- **V5 · intake INDEX 一致**：changes-index --self-check 41/41（基线 31 + intake lane 10 臂：parseSpine 双形态 / faceMapHits 结构化+inline / anchorsProgress 待回填 / 漂移①② / 值域外 / closed 无标）；INDEX 数据行 8 == spine 文件 8（`ls delivery/intake/*.md` = 9 含 1 件 companion design.md，非 spine、解析器按 T4 命名规约排除——V5 判据按 spine 计数执行）；抽验 2 份：261012（faceMap 四面命中 + anchors 4/4 + in-flight ↔ INDEX 行一致）、261001（awaiting-upstream + 4/4 + ⚠未闭环 ↔ 整改后实态一致）。
- **V6 · 机检绿**：settings-lint 0 RED；spec-anchor ok:true errors:[]；landing-fidelity --preflight anchorCount = **82**，findings [] / advisories []——**anchorUnion ≡ manifest 全等**（FROZEN T6 第 5 条扩根 + R3-F1 七件补列两路归零兑现）。
- **V7 · INDEX 切换对账**：`test -f delivery/archive/changes/arch/INDEX.md` ✓（旧 INDEX 随归档冻结）；`delivery/review/INDEX.md` 数据行 = 1（前移名单 = 本批自身，落地窗口 backfill 实测唯一「仍要向 main 交付制品」批）+ 头部指针行指向归档旧 INDEX ✓；changes-index 连跑两次 `git diff --stat` 零变化（幂等）✓。
- **V8 · 时机前置**：2026-10-16 实测——dev/260915-m1-k1a-key-layer-dispatch 已在 main；dev/pod-m2 内容全数进 main（261015-p1-auxbar-style-fix 经 Pod 自己的 dev-loop 流落 main，分支由活 Pod worktree 持有、不影响前置）；261013 簿记已补齐合并 main（INDEX frozen/yes）；260926 目录在 main 在册（draft 态，随归档冻结、T6 第 1 条核注见 §3 注 4）。
- **V9 · 契约零写**：本批 diff `git diff --name-only main -- docs/contracts/` = 空（OQ-A 裁 (b)，断链改向归另起轻批）。
- **附带机械检查**：gate --self-check 49/49；changes-index --self-check 41/41 + changeid-prefix 4/4；settings-lint 3 files 0 RED 0 WARN；spec-anchor ok；brief-inputs --self-check 43/43。

## 3. 诚实注记

1. **两件 spine 格式修复（V5 使然）**：261013-gov-review-lf-eol-teeth-pin.md 与 260928-char-window-key-triggered-play.md 的 frontmatter 无闭合 `---` 围栏（历史手写缺陷、非本批引入）——intake lane 投影需可解析 frontmatter，EOF 各补一行闭合围栏（零内容改动、零字段改动）。超出 FROZEN 整改枚举（261010/261001）的机械格式修复，如实登记。
2. **V4 判据「本批自身引用」扩项**：本批证据件（landing-verdict-w1 / architect-landing-summary-w1 / 本件与终态 verdict）内的旧路径取证原文与 spec 面两条勘误行内的旧字面量，按「本批自引」类并入命中集——与 FROZEN §5 V4 字面（仅 PROPOSAL 自身引用）的偏差为证据件性质同源扩项，如实登记。
3. **taskpackages/INDEX.md origin.ref 列旧路径**（9 行）：由包内历史 taskpackage.json origin 字段经 closeout-check 再生的投影——源字段属包内历史件豁免；若逐行改投影列则须改历史源（违反豁免），故按投影随源豁免处理，留根观察。
4. **260926-arch-review-char-window-key-triggered-play**（draft/—）：目录已在 main 在册（PROPOSAL 起草时「制品不在 main」的实测已过时）——随归档冻结；日后重启 = 新立 changeId 引旧批（frozen/yes 停滞行同款处置）。
5. **frozen/yes 停滞异常行 5 行**（260922/260923-graph-index/260928-requirement-face/260928-requirement-loop/260929-agent-def）：随归档冻结，落地窗口逐行核注 = 本注记（状态机完整闭环或人工 adjudication 在案，重启 = 新立 changeId 引旧批）。
6. **enforce-scope 拦截消息不含废止注记字样**（波段一已记，waves 间未变）：denyCreate 消息为通用形态，废止注记在 scope-map `_doc`（V2 措辞差距承袭 landing-verdict-w1 ADVISORY 口径）。
7. **261001 整改后预期漂移标**：⚠未闭环（anchors 4/4 实况回填但未 closed）——⚠滞留Nd 按末提交日代理在落地提交后归零不出现（verifier A2 预告失准已核，检测机制本身合 FROZEN）；如实呈现待下轮 loop-retro 复议（T5 复议触发承载点）。
8. **landing 期 FAIL(F1) 补列（verifier 2026-10-16 裁）**：debt-ledger-shared.mjs :58 G4 证据根枚举缺新树四根（P3 转义形活面、debt-doctor 消费链潜伏缺口）——按核验官修法扩根落地（changes 根保留解析历史 debt 路径）+ §2 T6 目标文件行与 manifest 补列（82→83）+ Status 行凭据补记；debt-doctor/render self-check 13/13、8/8 绿。

```landing-manifest
.claude/scope-map.json
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/brief-inputs.mjs
delivery/taskpackages/261015-voice-rightcol-label-overflow/review-packet.md
delivery/taskpackages/261015-voice-rightcol-label-overflow/supervisor-verdict.json
delivery/taskpackages/261015-voice-rightcol-label-overflow/pod-retro.md
delivery/taskpackages/261015-voice-rightcol-label-overflow/docs-delta.md
.claude/rules/repo-structure.md
.claude/references/change-intake.md
docs/templates/debt-item.template.md
.claude/scripts/changes-index.mjs
.claude/agents/integrator.md
.claude/commands/build-feature.md
.claude/commands/arch-change.md
.claude/commands/gov-change.md
.claude/commands/requirement-change.md
.claude/commands/loop-retro.md
.claude/references/requirement-change.md
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/portfolio-reconcile.mjs
.claude/scripts/proposal-consolidation-check.mjs
.claude/scripts/settings-lint.mjs
.claude/scripts/debt-ledger-shared.mjs
.claude/scripts/arch-review-handoff-check.mjs
.claude/scripts/devloop-artifact-check.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/hooks/commit-safety.mjs
.claude/hooks/enforce-scope.mjs
.claude/hooks/suggest-codebase-memory.mjs
.claude/hooks/supervisor-verdict-gate.mjs
.claude/route-fixtures/surface-alignment.json
.claude/agents/adjudicator.md
.claude/agents/architect.md
.claude/agents/challenger.md
.claude/agents/defender.md
.claude/agents/landing-verifier.md
.claude/agents/planner.md
.claude/agents/review-moderator.md
.claude/agents/scenario-author.md
.claude/references/arch-review-protocol.md
.claude/references/arch-scenarios.md
.claude/references/architect-operating-model.md
.claude/references/change-triage.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/plain-file-handoff.md
.claude/references/path-form-family.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/references/taskpackage-authoring.md
.claude/references/track-b-verifier-brief.md
.claude/skills/brainstorming/SKILL.md
.claude/skills/gov-arq/SKILL.md
.claude/skills/kidpal-idf-env/SKILL.md
.claude/skills/merge-to-main/SKILL.md
.claude/skills/opendesign-connect/SKILL.md
AGENTS.md
docs/DEV-GOVERNANCE.md
docs/requirements.md
docs/architecture.md
docs/notes/ui-implementation-strategy.md
docs/showme/README.md
docs/templates/PROPOSAL.template.md
docs/templates/architect-landing-summary.template.md
docs/templates/retro.template.md
docs/templates/GOVERNANCE-MIGRATION-GUIDE.md
delivery/intake/260928-char-window-key-triggered-play.md
delivery/intake/260929-char-window-v5-trim.md
delivery/intake/261001-m4-k3-record-session-live-repair.md
delivery/intake/261010-p3-storylist-cold-start.md
delivery/intake/261011-p1-home-v5-rework.md
delivery/intake/261012-kidpal-realtime-facts.md
delivery/intake/261013-gov-review-lf-eol-teeth-pin.md
delivery/intake/261014-single-tier-bringlist-home.md
delivery/intake/261014-single-tier-bringlist-home.design.md
delivery/intake/INDEX.md
delivery/retro/2026-09-13-loop-retro-efficiency.html
delivery/retro/2026-09-23-loop-retro.md
delivery/retro/2026-10-14-loop-retro.md
delivery/review/INDEX.md
delivery/review/.changes-index-state.json
delivery/archive/README.md
```
