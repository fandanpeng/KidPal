# architect-landing-summary · 261027-gov-review-p75-closeout-shoot-cluster

packet-id：FROZEN PROPOSAL（v0.2）§2 改法集（T1–T6，共 17 处编辑，旧串锚定逐字节承诺）+ §5 验证集 · 评审轮次 R1 BLOCK(F1,F2,F3)+2NIT → v0.2 文本级修订 → R2 GREEN（判档 Track B 主路径维持）· change-id gov-review-261027-gov-review-p75-closeout-shoot-cluster

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 收口义务→承载写点映射表（修法①） | `docs/DEV-GOVERNANCE.md` | §6 新增映射表（7 行：账本登记 / 设计稿母版 / 出图 / spine 回填 / spec 挂法句 / docs-notes 经验记录 / taskpackage 元数据）+ §7 分工句防双表漂移 |
| T2 收口写点核对（修法②） | `.claude/agents/planner.md` | 新增 Step 8c「收口写点核对」：finalize 前按映射表逐行核 + 持会话 agent 粒度核写权（Pod 主会话 = planner，deny docs/**）；惯例非门 |
| T3 出图管道节（改进项①） | `.claude/skills/opendesign-connect/SKILL.md` | 新增「出图管道」节：`node tools/asset-pipeline/shoot_design.mjs <母版> <输出目录>`；版本语义纯指针指向 `docs/ui/UI-DESIGN-VERSIONS.md`（P5 声明面），本 skill 不复述 |
| T4 出图通道口径 + 单写者约定（改进项② + trigger ③） | `.claude/agents/uiux-author.md` | 五处：通道口径四处（description / Role / Step 3 / Boundaries）+ Step 2 末尾母版/增量页单写者约定（Write 语义判据——本 agent 工具面无 Edit；盘上含非本 agent 产出内容 → 停、交回主线程合并） |
| T5 消费者对齐（真因清除） | `.claude/references/requirement-change.md`；`.claude/commands/requirement-change.md` | 4b / 4.5 两处「按规格单跑 OpenDesign MCP（od_*）回填 PNG」改为 shoot_design.mjs 管道口径；角色表 `od_* 归主线程` 同步；OD 失联只挡设计生成 |
| T6 结项记账 | `docs/debt/gov/ARQ-261019-*.md`（已归位 `_archive/2026/`）；`docs/debt/gov/ARQ-261014-*.md` | ARQ-261019 status→closed + 结项追注（与条目编号一一对应，单写者约定标 trigger ③）→ debt-render 归位；ARQ-261014 维持 deferred + 文末追注（2610-25 三子项：②③ 收编、① 半收编留根） |

## 2. 验证证据

- **V1（装配可重现 + sha256 对账）**：① 落地前复跑 `node .claude/scripts/assemble-edits.mjs <批目录>/_scratch/edits.json C:/tmp/p75-prelaunch` → `ok:true`、8 件，逐件 afterSha256 与 FROZEN v0.2 §5 B2 逐值相同；② 落地用同清单同断言语义（`node C:/tmp/p7-work/apply-lands.mjs C:/AI/KidPal-gov <edits.json>`，逐条 oldStr 恰 1 命中、失败零写盘）→ `ok:true`，仓内 8 件 sha256 与 B2 逐值相同（DEV-GOVERNANCE `cc2960c6bade0dce…`、planner `372d17f780022ef7…`、SKILL `6b54eec333a99052…`、reference `50f8dd67bd8bb3e6…`、command `8edfb351af5a260c…`、uiux-author `2bc3d2033ebc43a6…`、ARQ-261019 `048c306239810c03…`、ARQ-261014 `522e9b1ba946f834…`）。绿。
- **V2（cite-check 落地相位）**：`--cite-check` → `ok:true`、`phase:landing`、`targetCount:8`、checked 17 / missing 0 / notChecked 17 / unverified 0；15 条 matched 直指对应目标文件，ARQ-261019 两块按归档位 `docs/debt/gov/_archive/2026/ARQ-261019-….md` 命中（debt-render 归位后 checker 归档兜底生效，P5 批先例）。绿。
- **V3–V5（字面正推 / 排除集 / 残留）**：起草期 B7/B8 census 与判官 R1/R2 两轮复跑一致；主线程落地后复核——G3 面（scope-map / settings / verify-gates / hooks / scripts）+ 契约面 + spec 两文件 + `docs/ui/**` + 实现面 `git diff --name-only` 空输出；`git status --porcelain` 本批改动恰 = manifest 8 件（7 M + ARQ-261019 D + 归档位 ??）+ 预期在途件（INDEX 生成件、task_plan、.codebase-memory 常驻脏）；`_scratch/` 已于落地 commit 前删除（V5 / §6-6）。绿。
- **真空绿规约**：V1① 重放断言（edits.json 在场）；V2 负向（missing 空）× 正向（checked 17 逐条 matched 非空）；sha256 全文件钉死 = 任一实现反转必红。无真空绿项。

## 3. 诚实注记

- **判官两轮记录**：R1 三 SHOULD（F1 追注编号错位 / F2 Edit 不可执行 / F3 B10 摘要失实）+ 2 NIT 全部修订到位（R2 逐条核 + 机检复跑），无遗留 BLOCKING/SHOULD。
- **cite-check 归属歧义 1 条**（F3 修订后口径）：ARQ-261019 frontmatter 翻转块 oldStr（`status: deferred`）在两件 ARQ 条目各恰 1 命中，checker 归属到 ARQ-261014——歧义非未命中，由装配器「恰 1 命中」断言与 V3 哨行兜住；落地相位下新文唯一命中 ARQ-261019，无实际风险。
- **在途件时点口径**：本 summary 写盘时点的工作树在途件 = INDEX 生成件、task_plan.md、.codebase-memory 两件（常驻自刷新）；其后主线程账本 / 生成件动作以 landing-verifier 实跑为最终口径（ARQ-261027-landing-summary-inflight-snapshot 处方精神，条目待批）。
- **留根 7 条**见 FROZEN §6（Pod per-session cast uiux-author 可用性、账本机械通道 OQ-1 已拍 (a) 留观察、SKILL description 死指针、Step 8c 无机械闸、生成件/归档位移、_scratch 清理〔已执行〕、P5 留根交接）。
- **ARQ-261014 不结项**（部分闭合：2610-19 CDQ 子项 + 本批 ②③ 收编、① 半收编），维持 deferred；ARQ-261019 整条结项归档。

```landing-manifest
docs/DEV-GOVERNANCE.md
.claude/agents/planner.md
.claude/skills/opendesign-connect/SKILL.md
.claude/agents/uiux-author.md
.claude/references/requirement-change.md
.claude/commands/requirement-change.md
docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md
docs/debt/gov/ARQ-261019-opendesign-shoot-script-precedent.md
```
