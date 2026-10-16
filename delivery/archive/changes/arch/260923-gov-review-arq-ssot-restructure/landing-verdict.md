# landing-verdict · 260923-gov-review-arq-ssot-restructure

verdict: PASS

**结论：PASS + 5 ADVISORY**（非阻断观察，交 human merge 门一并过目；无 FAIL、无升级 human 项）

- change-id：`260923-gov-review-arq-ssot-restructure`
- packet：FROZEN PROPOSAL（Status 行 FROZEN v1.0，§7 含 R1 BLOCK→R2 BLOCK→R3 GREEN 三轮）+ 双侧 landing-manifest 围栏块（各 124 行：25 件机制/文面 + 99 件 `docs/crq/**` 成员枚举）+ `architect-landing-summary.md`
- 核验方式：零上下文重推——机械门亲跑 + 逐文件读 diff 对照 FROZEN §2 改法 + 迁移条目人读抽核 + scratch 独立重演；**不采信 summary 自述**
- 轨：Track B（无 decision-packet，`proposal-consolidation-check.mjs` = N/A；「frozen ↔ 批定一致」前提由 §7-R3 GREEN + Status FROZEN 承载）

## 1. 机械门输出

**结构门**（第 1 门）：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260923-gov-review-arq-ssot-restructure` → exit 0，`ok:true`、`frozen:true`、`packetBinding:true`、`gitDegraded:false`；`unauthorized:[]`、`claimedNotLanded:[]`、`plannedNotLanded:[]`、`proposalTargets`(=124) == `summaryTargets`(=124)。
- `advisory.changedCanonicalNotInProposal` = `["docs/crq/gov/ARQ-260924-landing-gate-dir-anchor-gap.md", "delivery/workstreams/m3-schedule/progress.md"]`——两条均在 summary §3 声明内，逐项见 §7-ADVISORY。
- `lintDescriptionsAdvisory`：exit 0，仅既有 warn（`landing-verifier.md` 363>320，非本批文件）。

**设计侧门**（第 2 门）：Track B ⇒ N/A（无 `decision-packet.md`，不跑 `proposal-consolidation-check.mjs`）。

**preflight（V6）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <PROPOSAL>` → `ok:true`、`findings:[]`、`anchorCount:124`（== manifest 124 行，与目录简写机械展开后一致）。**注**：summary §2-V6 写「anchorCount:26」为展开前旧值，实况 124（展开动作见 summary §3-9，已核）。

## 2. packet-binding 核对

- summary `packet-id` 行 = FROZEN PROPOSAL §2 改法集（T1–T6）+ §5 验证集 + 轮次 R1(RED)→R2(RED)→R3(GREEN) —— 与 FROZEN 头注一致。
- manifest == §2 各 T「目标文件锚清单」并集：25 件机制/文面 + 99 件 `docs/crq/**` 枚举，与结构门 `proposalTargets` == `summaryTargets` 吻合；条件锚（`.gitattributes`、`docs/crq/**`）按 §2 逐件展开裁剪。
- **落在（工作树改动）⊆ packet**：唯一越界 = dogfood 条目（ADVISORY-1，已声明）；其余全部在册。

## 3. 逐文件语义核对（25 件机制/文面，读实际 diff 对照 §2 改法）

| 文件 | 实际改动 | 对照 §2 | 判 |
|---|---|---|---|
| `.claude/scope-map.json` | `docsTopLevel.allowDirs` +`docs/crq`；integrator allow +`docs/crq/**`；adjudicator allow +`docs/crq/**` | T6「allowDirs 增 crq + integrator/adjudicator 增写槽」 | ok（无越权扩张） |
| `.claude/rules/repo-structure.md` | docs 根白名单 +`crq/`；`docs/crq/` 表行（谁写列含 integrator）；三种同名物消歧表 + 引用纪律；`docs/` 语义扩张说明；archive 行举例句去「.closed.md」 | T6「白名单 + crq 行 + 语义扩张 + 三种同名物消歧」；OQ-2「repo-structure:20 举例句须改」 | ok |
| `.claude/rules/contracts.md` | :15 指针改 `docs/crq/gov/ARQ-260827-contract-mech-gate-gap.md` | T6 | ok |
| `.claude/skills/gov-arq/SKILL.md` | 全篇：登记从「填表格行」→「写条目文件 + 跑 `crq-render`」；增「定域」步（三值判据）、`paths[]` 机推步、七值 severity 并集说明、四段正文、`crq-render`/`crq-doctor` 自检 | T6 + T3（域判据/前缀规则） | ok |
| `.claude/skills/merge-to-main/SKILL.md` | :18 热点清单：旧 ARQ 热点句删除，改 CRQ index `merge=union` + 渲染自愈 + stub 冷文件 | T6 | ok |
| `.claude/route-fixtures/surface-alignment.json` | adjudicator allow +`docs/crq/**`，lastReviewed→2026-09-24，rationale 更新 | T6（与 scope-map 对齐件） | ok |
| `.claude/references/dev-loop-operating-system.md` | :158 登记纪律段 + :173 出口指针 改 CRQ 口径（两处） | T6（两处） | ok |
| `.claude/references/dev-loop.md` | :338 表格列结构约定行整条换为 CRQ index 生成件 + doctor 判据 | T6 | ok |
| `.claude/references/arch-review-protocol.md` | :170 SPAWN-NEW-CONCERN 登记行改 CRQ 按域 | T6 | ok |
| `.claude/references/architect-operating-model.md` | :29 决策账本清单 + :40 队列注册段改 CRQ（两处） | T6 | ok |
| `.claude/references/contract-change-evidence.md` | :17 Queue update 行改 CDQ 权威 + CRQ tech 域 `cdq:` 指针 | T6 | ok |
| `.claude/agents/adjudicator.md` | description（保留「queue」路由信号 + 加 CRQ）/ Step7 产物 / Boundaries / 制品定义 四处改写 | T6「description/:39/:50/:77 四处」 | ok（写面按域落文件夹） |
| `.claude/agents/review-moderator.md` | :61 读面改 `docs/crq/{gov,tech,requirement}/INDEX.md` | T6 | ok |
| `.claude/commands/loop-retro.md` | :40 ARQ candidate 登记改 `docs/crq/gov/` + 查重 `grep -l` | T6 | ok |
| `.claude/commands/arch-change.md` | :42 收口核验改「按域写 CRQ 条目 + 跑 render」 | T6 | ok |
| `.claude/commands/review-arch.md` | :31 同款 | T6 | ok |
| `.claude/commands/review-governance.md` | :33 同款 | T6 | ok |
| `docs/ARCH-REVIEW-QUEUE.md` | 153 行 → 21 行指路 stub（-141/+14）：标题 + 三域去向表 + 登记/结项指引 + 来源指针；非生成件、无计数固化 | T2 改法 5 + R3-N1（实物形态） | ok |
| `docs/glossary.md` | ARQ 词条改 CRQ 口径 + 新增 CRQ 词条 + TECH-/REQ- 说明 + CDQ 词条补 `cdq:` 指针 | T6 | ok |
| `docs/templates/crq-item.template.md` | 新建：域表 + 骨架（含 `title` 必填）+ 三条纪律 | T1 改法 6 + R1-F1（补 title） | ok |
| `.gitattributes` | +两行 glob：`docs/crq/*/INDEX.md` + `docs/crq/*/_archive/*/INDEX.md` merge=union | T2 改法 3 件 2（两行 glob 主+归档） | ok（`git check-attr` 逐件命中 union） |
| `.claude/scripts/crq-ledger-shared.mjs`（新） | 域配置/闭集/`REQUIRED_FIELDS`(含 title)/`GATE_LAYER_RULES`/`hardExclusionsOf`/解析序列化/排序（升序）/归档纯函数/`checkItems` | T2 改法 1 + T5 两档 | ok |
| `.claude/scripts/crq-render.mjs`（新） | 渲染 5 份 INDEX + 归档归位 + R6 写守卫 + R7 CRLF 归一 + `--check`/`--self-check` | T2 改法 2/6 + R5/R6/R7 | ok |
| `.claude/scripts/crq-doctor.mjs`（新） | 红档 5 + advisory 6 + 件 3 漂移检测 + `--self-check` 13 夹具 | T5 | ok |
| `.claude/scripts/crq-migrate.mjs`（新） | 95→94 迁移 + `--src`/`--dry-run`/`--verify` + E9 真空守卫 | T1 改法 4/5 + E9 | ok |

**FROZEN 共享闸件**：`.claude/scripts/lib/governance-gate-shared.mjs` **零 diff**（`git diff HEAD` 空、`git status` 无）——与 PROPOSAL「不改任何 FROZEN 共享闸件、T7 已删」逐字吻合。
**无越界 G1/契约**：`git status` 无对 `AGENTS.md`/`CLAUDE.md`/`docs/DEV-GOVERNANCE.md`/`docs/contracts/**`/`settings`/`hooks`/`verify-gates.json` 的改动（不新增门）。

## 4. 敏感面逐字核（自升级高发面）

- **G3 写权（scope-map）**：仅 +`docs/crq`（allowDir）/ integrator / adjudicator 写槽——与 T6 表逐字一致；**未扩张 architect 写权**（architect 仍无 `docs/crq/**`，与 T3/architect-operating-model「登记 = 主线程动作」一致）。无借 landing 夹带。
- **G3 机制脚本**：4 件 new 脚本审查——无 hook 注册、无写 `verify-gates.json`/`settings`；doctor/render 只读账本 + 写 `docs/crq/**`；`crq-doctor` 明定为告警级（main 无 exit≠0 于真仓绿态）。
- **self-def / review-loop agent 定义**：`adjudicator.md`（review-loop 定义）改动 == 被批 T6 四处（写面按域），未改 `tools`/`name`/`model`/authority 语义；`architect.md`（self-def）**零触点**。
- **G1 指令根**：零触点。

## 5. 迁移保真（V8 全量人读抽核承接）

**机器对账**（亲跑 `crq-migrate.mjs --verify --src /tmp/orig-queue.md`）：ID 集合全等 ok；逐条对账（源→意图）仅 1 条差 = `cert-lifecycle-deadlock +177`；逐条对账（意图→落盘）2 条差 = `schedule-grid-restore-gate +316` + `landing-gate-dir-anchor-gap +1750`（dogfood 新条目，非迁移面，见 ADVISORY-1）。**两处已声明语义差**（PROPOSAL §0-E2/T1 5-1/T3 例外条款）**逐字核过**：
- `docs/crq/tech/ARQ-260902-cert-lifecycle-deadlock.md`：§1.3 watchlist 正文 **verbatim 保留** + §2 决策闭环 **原文整段保留** + 新增「双重落位裁决（迁移）」段（DUAL_PLACEMENT→accepted-risk）；正文无删改、仅追加。忠实。
- `docs/crq/gov/_archive/2026/ARQ-260923-schedule-grid-restore-gate.md`：源 §1.1 正文 **verbatim 保留** + 追加「结项（2026-09-23，按 T3 例外条款）」段（置 closed/closedBy）；源「处理时机」格（已写明已结）并入 `trigger` 原文留痕。忠实（状态改写有 T3 例外条款为据）。

**随机 8 条人读抽核**（覆盖 未结/已结 × 三域，逐条比对源行 cell 与条目 frontmatter+正文）：

| 条目 | 域/状态 | 源节 | 结论 |
|---|---|---|---|
| `gov/ARQ-260918-pod-session-settings-identity-unobservable` | gov/deferred | §1.2 | 正文 verbatim；title=源 title cell；owner 尾部「+」为 §3-4 声明的 best-effort 截断 |
| `gov/ARQ-260827-contract-mech-gate-gap` | gov/accepted-risk | §1.3 | 正文 verbatim；域=OVERRIDES gov（留根 12 成文判据） |
| `gov/_archive/2026/ARQ-260907-gate-lib-script-separation` | gov/closed | §2 | 正文 verbatim；severity 空（已结豁免）；归位 `_archive/2026/` |
| `tech/ARQ-260831-simulator-lvgl-version-drift` | tech/accepted-risk | §1.3 | 正文 verbatim；域 override |
| `tech/ARQ-260918-spec-face-3page-reconcile` | tech/deferred | §1.2 | 正文 verbatim；域 override |
| `requirement/ARQ-260916-home-date-format-english` | requirement/accepted-risk | §1.3 | 正文 verbatim；保留 `ARQ-` 祖辈前缀（T4 改法 5） |
| `gov/ARQ-260914-portfolio-reconcile-launchpreconditions` | gov/deferred | §1.2（7 列裸竖线行） | 锚首三列+锚末两列解析正确，正文内裸 `\|` 逐字保留为 `\|` |
| `tech/_archive/2026/ARQ-260902-shim-401-gate-bypass` | tech/closed | §2 | 正文 verbatim；域 override；归位归档 |

**落域/计数**（亲核）：gov 顶层 13（12 迁移 + 1 dogfood）、`_archive/2026/` 64；tech 顶层 15、归档 2；requirement 顶层 1；`docs/crq` 共 100 文件 = 99 manifest + 1 dogfood；5 份 INDEX。与 summary §1/T3 表（迁移时点 28 未结）一致（+dogfood ⇒ 现 29 未结）。

## 6. 验证集复跑（§5 V1–V14，与 summary §2 声称比对）

| # | 结果（亲跑） | 与 summary 比对 |
|---|---|---|
| V1 | `--dry-run --src` = 95→94、ID 全等 yes、gov76/tech17/req1、竖线 58、⚠列数=7 仅 1 条 | 一致 |
| V2 | `crq-render --self-check: ok (8/8)` | 一致 |
| V3 | `crq-doctor --self-check: ok (13/13)`（判别力：夹具含反转正负控） | 一致 |
| V4 | 真仓 `条目 95 条：gov=77 tech=17 requirement=1`；`advisory 134`（no-paths94/severity-off-rubric5/legacy-prefix18/tech-no-handoff17）；`RED 零命中` | 一致（gov77=76+dogfood；advisory 134 同） |
| V5 | `crq-render --check` exit 0；写跑 5 份「未变」；`docs/crq` 零 diff（untracked）；stub diff -141/+14 | 一致（含 §3-5 口径注） |
| V6 | preflight `anchorCount:124`/`findings:[]` | **实况 124**（summary §2-V6 写 26 = 展开前） |
| V7 | `route-regression-check` 28 ok 0 fail；`lint-descriptions` 仅既有 warn | 一致 |
| V8 | 人读抽核（本件 §5） | 承接完成 |
| V9 | grep `ARCH-REVIEW-QUEUE` render/doctor/lib = 零命中（exit 1）；migrate 正当读历史源为声明内例外 | 一致 |
| V10 | render 连跑两次 md5 零差异 | 一致 |
| V11 | scratch 重演「两分支各加一条」：merge exit 0 **零人工解冲突**；两 row 俱在（零丢失零重复）；摘要行平凡取一（陈旧，未结 3 vs 真值 4）；doctor 当场 `RED 1 [index-stale]`；重渲染归位后 `RED 零命中` | 一致（E6 论断成立，**本核验官独立重演**） |
| V12 | scratch 删生成标记头 → `FATAL 目标不是生成件…拒写` exit 1 | 一致 |
| V13 | gov INDEX 摘要「未结 13（待拍板 0 / 待触发 11 / 已接受风险 2）；已结 64」+ 已结区年度指针；ID 同目录相对链接；归档薄行 | 见 ADVISORY-2（现 13 非 summary 记的 12） |
| V14 | 本批改动/新增文面中 `INDEX.md` 零裸指涉（泛指集合豁免）；5 份 INDEX 均带生成标记头 | 一致 |

**E9 真空守卫**（亲跑）：`crq-migrate.mjs` 对 stub 源 → `FATAL 源解析出 0 行…拒绝继续` exit 1。**R6 守卫**：scratch 实测 FATAL exit 1。**真空绿规约**：render/doctor 0 条目 FATAL（脚本内建）、V11 两分支各自渲染成功已断言。

## 7. ADVISORY 清单（非阻断观察）

1. **dogfood 条目越 manifest**：`docs/crq/gov/ARQ-260924-landing-gate-dir-anchor-gap.md`（gov/deferred，含 paths/gateLayers/hardExclusions）不在 frozen manifest，属 FROZEN 后账本日常登记（summary §3-10 声明）。经核：为真治理 bug（landing 门 vs preflight 锚口径差）+ 新机制首次登记（dogfood），**无自升级、无权限/写域扩张**；结构门亦分类为 advisory（非 unauthorized）。**交 human merge 门一并过目**。
2. **gov INDEX 摘要「未结 13」vs summary §2-V13 记「12」**：差值 = ADVISORY-1 的 dogfood 条目（12 迁移 + 1 dogfood），属 §3-10 已声明的后续动作；summary 为快照型记述，未构成漏记。
3. **summary §1-T2「63 条入 `_archive`」**：实况 64（E10 已述 63→（schedule-grid 置 closed）→64 的两段口径），单句数字未随扫——landing-summary 内小瑕，不影响落地。
4. **`gov-arq/SKILL.md` 尾换行丢失**：HEAD 版以 LF 收尾，工作树版末字节非换行（`\ No newline at end of file`）——非 §2 改法项的无意空白变更（无功能影响）。
5. **`crq-render.mjs --self-check` 标签「ok (8/8)」**：实为编号夹具 1–7 组（组内多断言），标签为标称计数——随打印字符串固定，非判据缺口（doctor 的 13/13 同型标称）。

## 8. 破坏性自检状态

- 每个 summary 声称的改动均已在真实 diff 找到对应（25 件文面逐件、4 脚本、5 INDEX、94 条目 + dogfood）；**未跳读敏感面或大 diff**（scope-map / adjudicator / repo-structure / 全篇 gov-arq skill / 迁移条目均逐件读 actual diff）。
- **未采信 summary 自述**：机械门亲跑退出码为准，迁移对账亲跑，union 合并、R6 守卫、真空守卫均 scratch 独立重演（不依赖主线程留痕）。
- 唯一「越界」= dogfood 条目（ADVISORY-1，已声明、非自升级）。

## 9. 升级线

无「落地与 FROZEN 语义偏离」「私货」「FROZEN 方向性问题」——**不升级 human**。LANDING 保真判 PASS；merge 仍归 human（Track B 自动合并门 + human 晨间追认/否决窗口在场，summary §3-1 声明）。