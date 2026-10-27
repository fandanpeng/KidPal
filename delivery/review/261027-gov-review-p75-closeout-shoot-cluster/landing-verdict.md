# landing-verdict · 261027-gov-review-p75-closeout-shoot-cluster

verdict: PASS + 1 ADVISORY

- 核验人：landing-verifier（独立核验，只读 codebase，只写本 verdict）
- 核验时点：2026-10-27 · 落地在工作树未 commit（nothing staged）
- packet-binding：landing-summary packet-id 行引 `gov-review-261027-gov-review-p75-closeout-shoot-cluster` == 被批 review id；summary 内 landing-manifest 围栏块 8 件 == FROZEN PROPOSAL manifest 8 件（逐 token 一致）；落地实际范围 = manifest 8 件，⊆ 被批 packet。

## 1. 结构门（机器）

- `landing-fidelity-check.mjs --preflight`：**ok:true**、section2BodyFound:true、anchorCount 8、anchorUnion 八件与 manifest 逐 token 一致、findings []、advisories []、exit 0。
- `landing-fidelity-check.mjs --cite-check --phase landing`：**ok:true**、phase:landing、frozen:true、targetCount 8、**checkedCount 17 / missing [] / notChecked 17（十七块原文，落地相位预期）/ unverified []**。15 条 matched 直指对应目标文件；ARQ-261019 两块（line 381 / 395，frontmatter 翻转 + 结项追注）按归档位 `docs/debt/gov/_archive/2026/ARQ-261019-opendesign-shoot-script-precedent.md` 命中——debt-render 归位后 checker 归档兜底生效（P5 批先例）。B10 所述 1 条归属歧义（oldStr `status: deferred` 双条目命中）在 landing 相位不复现：新文唯一命中 ARQ-261019 归档位。
- Track B：decision-packet 不在证据集，`proposal-consolidation-check.mjs` N/A 不跑。

## 2. 逐文件人读 diff（8 件 / 17 处编辑，全对 FROZEN §2 T1–T6，无漏改、无多落）

| 文件 | structuralTouched | semanticMatch | 核对明细 |
|---|---|---|---|
| `docs/DEV-GOVERNANCE.md` | M | ok | 3 hunk = T5-1（§2 团队表「出图 od_* 归主线程」→「出图归主线程（`shoot_design.mjs` 管道）」）+ T1-1（§6 末尾 / §7 之前插映射表整节：说明段 + 三列 7 行表 + 为什么成文段，与 §2 T1 改后块逐字一致）+ T1-2（§7 首句 append 分工边界句）。改动恰在 §6/§7 之间，`## 7. 入口速查` 标题未动（哨 1→1）。 |
| `.claude/agents/planner.md` | M | ok | 1 added line = T2（8c 收口写点核对，插在 8b 行与 `9. 派 builder` 行之间，改后块逐字一致，含「按持会话 agent 粒度」「惯例非门」全句）；邻行零碰（哨 8b / Step 9 各 1→1）。 |
| `.claude/skills/opendesign-connect/SKILL.md` | M | ok | 1 hunk = T3（「出图管道」节插在「生效（需 human）」之后、「## 约束（must / never）」之前，命令 / 行为 / 输入与落点（纯指针、P5 NIT-1 修订口径）/ 只捡变更帧 / 先例五 bullet 与 §2 T3 改后块逐字一致）。 |
| `.claude/agents/uiux-author.md` | M | ok | 5 处 = T4-1…T4-5：description（`od_* 归主线程`→`出图归主线程`）、Role（od_* → shoot_design.mjs 管道句）、Step 2 段末三空格缩进追加单写者约定（F2 修订后 Write 语义判据含「本 agent 工具面无 Edit」）、Step 3 出图交付句、Boundaries 出图与失联口径（「出图 pending」→「生成 pending」）。P5 批文面（Step 2 命名族 / Step 3 版本语义）逐字未动。 |
| `.claude/references/requirement-change.md` | M | ok | 2 处 = T5-2（4b 出图执行句换管道口径含 SKILL 指针）+ T5-3（角色表「不执行出图（出图管道归主线程）」）。 |
| `.claude/commands/requirement-change.md` | M | ok | 1 行内两处 = T5-4（4.5 出图执行句）+ T5-5（失联句「设计生成走 od_* 时失联挂 human-decision 事件、产物标『生成 pending』」）。 |
| `docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md` | M | ok | 纯 append = T6-3（文末 2026-10-27 追注，与 §2 改后块逐字一致；status 维持 deferred，哨 1→1）。 |
| `docs/debt/gov/ARQ-261019-opendesign-shoot-script-precedent.md` | D + 归档位 ?? | ok | 原路径删除、经 debt-render 归位 `docs/debt/gov/_archive/2026/`。**归档位 vs HEAD 原文 blob（2715b493）逐字节 diff：恰两处**——T6-1（frontmatter `status: deferred` → `closed` + `closed: 2026-10-27` + `closedBy: 261027-gov-review-p75-closeout-shoot-cluster`）+ T6-2（文末结项追注，F1 修订后口径：①skill 节 ②定义四处 ③真因记录成文消解 + 单写者约定标 trigger ③），其余正文逐字节相同；CRLF 行尾保留（cat -A 全行 `^M$`，符合「保留文件原 CRLF」承诺）。 |

**编辑数对账**：T1=2 + T2=1 + T3=1 + T4=5 + T5=5 + T6=3 = **17/17 落齐，零多落、零夹带**（各文件 diff hunk 全部可归属到 §2 某 T，无 PROPOSAL 未授权改动）。

## 3. sha256 独立复算（对 §5 B2 after 值，V1②）

`sha256sum`（= Node createHash utf8 语义）逐件复算，**8/8 与 B2 afterSha256 前 16 位逐值相同**：

| 文件 | 复算值 | B2 预期 |
|---|---|---|
| DEV-GOVERNANCE.md | cc2960c6bade0dce | cc2960c6bade0dce ✓ |
| planner.md | 372d17f780022ef7 | 372d17f780022ef7 ✓ |
| SKILL.md | 6b54eec333a99052 | 6b54eec333a99052 ✓ |
| references/requirement-change.md | 50f8dd67bd8bb3e6 | 50f8dd67bd8bb3e6 ✓ |
| commands/requirement-change.md | 8edfb351af5a260c | 8edfb351af5a260c ✓ |
| uiux-author.md | 2bc3d2033ebc43a6 | 2bc3d2033ebc43a6 ✓ |
| ARQ-261019（归档位） | 048c306239810c03 | 048c306239810c03 ✓ |
| ARQ-261014 | 522e9b1ba946f834 | 522e9b1ba946f834 ✓ |

## 4. V3 字面正推 + 哨行（独立复跑）

23/23 断言全对：退役语面旧值全 0（DG `出图 od_* 归主线程`、RF/CM `按规格单跑 OpenDesign MCP（od_*）`、RF/UA `不执行出图（od_* 归主线程）`、CM `od_* 失联挂 human-decision`、A19 `status: deferred`）；新字面各 = 预期（DG 映射字面 2、PL `收口写点核对` 1、SK `## 出图管道` / `shoot_design` 各 1、UA 单写者约定 1、A19 `status: closed` / `结项追注（2026-10-27` 各 1、A14 追注 1）；哨行恒 1（`## 7. 入口速查` / PL `8b. **锚 2` 与行首 `9. 派 builder` / SK `## 约束（must / never）` / RF `**出图执行 = 主线程代执行**` / CM `4.5. **UI 生成` / UA `机检在现行稿相对前序版本塌缩过半` / A14 `status: deferred`）——邻行零误删。

## 5. 破坏性自检（manifest 外零改动）

- **排除集零 diff**（实跑 `git diff --name-only -- docs/contracts/ docs/requirements.md docs/architecture.md docs/ui/ firmware/ server/ tools/ .claude/hooks/ .claude/scripts/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json AGENTS.md CLAUDE.md`）：**空输出**。G3 面 / 契约面 / spec 两文件 / docs/ui 图夹 / 实现面 / 指令根全部零触碰，与 PROPOSAL §3 sweep 断言一致。
- **`git status --porcelain` 全量过、在途件逐件定性**：
  - manifest 8 件（7 M + ARQ-261019 D + 归档位 ??）——本批落地本体。
  - `delivery/review/261027-gov-review-p75-closeout-shoot-cluster/`（??）——仅 PROPOSAL.md + architect-landing-summary.md 两件；**`_scratch/` 已删**（V5 达成）。
  - `delivery/review/INDEX.md`（M）——生成件；工作树内变更 = 上一批 p7-devloop 行补钉（前批 chore），**本批 p75 行未入**（见 ADVISORY-1）。
  - `docs/debt/gov/INDEX.md` + `_archive/2026/INDEX.md`（M）——debt-render 再生：ARQ-261019 行移出待触发段（12→11）、归档 2026 年 123→124 并落薄行（已结日期 2026-10-27 / 依据本批 id）；ARQ-261014 行仍在未结段。禁手改件、正常。
  - `delivery/workstreams/gov-debt-reduction/task_plan.md`（M）——P7.5 立项 ack + DRAFT/R1/R2/FROZEN/落地进度记账，workstream 账本面，预期在途。
  - `.codebase-memory/artifact.json` + `graph.db.zst`（M）——常驻自刷新脏，按治理不追交。
- 敏感面复核：本批触 G1（DEV-GOVERNANCE）与 G2（planner / uiux-author 两 agent 定义 + SKILL + references + commands）——逐字核过（§2 表），无权限 / 写域扩张：planner 8c 是纯核对纪律步、无新门；uiux-author 单写者约定是收窄性写入纪律（禁整份覆盖），均 == 被批 packet 文本。

## 6. ADVISORY

1. **ADVISORY-1 · `changes-index --upsert` 尚未跑**：`delivery/review/INDEX.md` 无 `261027-gov-review-p75-closeout-shoot-cluster` 行（FROZEN 转移点 upsert 未执行）。V7 属主线程 chore 且要求**在落地 commit 之前**跑——跑晚会静默漏带 INDEX.md 与 state 两生成件（前两批先例）。非 landing 保真缺陷（INDEX 是生成件、不在 manifest），但**必须在落地 commit 前闭合**：`node .claude/scripts/changes-index.mjs --upsert 261027-gov-review-p75-closeout-shoot-cluster`。

## 7. 结论

PASS：实际落地 diff == FROZEN PROPOSAL（v0.2）§2 改法集 T1–T6 共 17 处编辑 + §5 验证集承诺，逐文件逐字核对成立；两机检全绿；sha256 八件复算逐值相符；manifest 外 canonical 零改动；`_scratch/` 已删。放行进入后续主线程动作（--upsert 补钉 → 落地 commit → 升级闸 → 合并）；merge 决定归 human / 机械闸，本 verdict 只核保真度。
