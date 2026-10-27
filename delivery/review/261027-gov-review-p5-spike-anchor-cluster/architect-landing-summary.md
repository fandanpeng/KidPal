# architect-landing-summary · 261027-gov-review-p5-spike-anchor-cluster

packet-id：FROZEN PROPOSAL §2 改法集（T1–T3）+ §5 验证集 · 评审轮次 R1 BLOCK(F1) → R2 BLOCK(F2,F3) → R3 BLOCK(F4) → R4 GREEN（熔断于 R3 触发、human 2026-10-27 拍换新判官实例再战 1 轮，R4 解除） · change-id gov-review-261027-gov-review-p5-spike-anchor-cluster

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1①（判档必答） | `.claude/references/change-triage.md` | 「新机制批判定（spike-first 前置问）」节插入判档清单前（锚 = 「小 feature 与小 bug 合流为同一档」行），命中/未命中/性质三 bullet 齐 |
| T1②（spike 相位节） | `.claude/references/dev-loop.md` | 「新机制批 spike 相位」节插入「变更制品（Change Artifact）」节之前；spike 定义三条 spine + 数据迁移四问 + Gate E 引用如实承载 |
| T1③（制品表新条） | `.claude/references/dev-loop.md` | 变更制品表追加 `spike-evidence.md` 条目（惯例非门口径） |
| T1④（intake 纪律步） | `.claude/agents/planner.md` | Step 1i 插入既有 Step 2 之前（锚 = 「对任何疑似契约变更先派…」行）；spike 包 / 正式包 / 未命中三分支齐 |
| T1⑤（模板硬项） | `docs/templates/PROPOSAL.template.md` | 「【硬项 · 新机制 spike 申报】」块插入「条目形态（五段固定式…）」行之前；三选一 + 缺席即判官 BLOCK |
| T2.1（声明面新文件） | `docs/ui/UI-DESIGN-VERSIONS.md`（新建） | 机读 `current: kidpal-ui-design-v7.html`（行首恰 1）+ v2–v7 逐版备注（git 考据回填，v5 起点 `ec0804bd` 2026-09-22）+ 现行指针依据记录（四证合一）+「活图夹与冻结快照」语义节（OQ2 丙裁成文，含 F3 过渡限定） |
| T2.2（脚本六编辑 A0/A/B/C/D/E） | `.claude/scripts/spec-anchor-check.mjs` | 现行设计稿改由声明面 `current:` 指针决定；声明缺失 / 指针失效 exit 2；完整性自查兜底（前序 ≥10 且缺失 ≥50% exit 2、归因点名）+ 逃逸旗 `--allow-master-shrink`；退出码行同步。197 行 CRLF → 265 行 CRLF |
| T2.3（纪律文面四处五条目） | `.claude/agents/uiux-author.md` · `.claude/skills/write-story/SKILL.md` · `docs/ui/UIUX-DESIGN-BRIEF.md` · `.claude/references/requirement-change.md` | uiux Process 第 2 步（命名族 = 完整母版 + 声明面同次落盘）与第 3 步出图落点句（单一活图夹 + 升版冻结 + 引用走声明面，C-1b 含过渡限定）整句替换；write-story 锚行规则第 4 条改指声明面（旧 v3 字面归零）；brief 版本行尾部改指声明面（旧 v6 字面归零，brief 自身版本行保留）；requirement-change 4b 补声明面指句 |
| T2.4（存量迁移） | `docs/ui/UI-DESIGN-VERSIONS.md` 内 | v2–v7 逐版备注 + 两族编号错位说明（brief v5↔设计 v6、brief v6↔设计 v7）随声明面落地；图夹与 `designRefs` 存量形态零触碰（OQ2 丙裁边界） |
| T2.5 / T3（ARQ 记账） | `docs/debt/gov/ARQ-261024-new-mechanism-spike-first.md` · `ARQ-261017-anchor-check-master-unvalidated.md` · `ARQ-260908-versioned-guard-repoint-execution.md` | 三条目 frontmatter `status: deferred→closed`、`closed: 2026-10-27`、`closedBy: 261027-gov-review-p5-spike-anchor-cluster` + 正文 append 结项注记各一段（260908 注记载四件逐件授权记录与 teeth-sweep 消化复证，明标「纯记账、未执行任何重指」）；`debt-render.mjs` 渲染后三条目自动归位 `docs/debt/gov/_archive/2026/`（状态纯函数，非人工搬移） |

## 2. 验证证据

- **V1** · 落地后脚本 + 落地后声明面，真仓正跑 → `{"specAnchorCheck":"forward","design":"kidpal-ui-design-v7.html","designIdCount":64,"archSectionCount":48,"storyCount":14,"anchorLines":36,"errors":[],"ok":true}`，exit 0——与 B17 / B1 逐字相同。**绿**。
- **V2** · 夹具 A–H4 全谱（`C:/tmp/p5-assembly/fixtures/`，`CLAUDE_PROJECT_DIR` 逐夹具）→ A/D/E/H4 exit 0；B exit 2 且 stderr 与 B5 逐字同（母版完整性报文含两版锚数与 83% 占比）；C exit 1（单点收缩不误判母版）；F/G/H1/H2/H3 exit 2 且归因各自形态（声明缺席 / 指针不存在 / 0 行 / 2 行 / 非母版命名）；`--allow-master-shrink`（夹具 B）exit 0；`--reverse`（E）结构视图正常 /（F）FATAL 设计稿版本声明 exit 2；`--diff-ids`（F，正确传参 `docs/ui/kidpal-ui-design-v1.html v2.html`）→ `oldCount:12 newCount:12`、`removed:["d-10"]`、`added:["d-11"]`、exit 0，与 B16 逐字同。与 B2（现行脚本同夹具假绿 exit 0）/ B3（现行脚本夹具 E exit 1）对拍判别成立。**绿**。
- **V3** · 文档字面正推（真仓落地件 `grep -cF`）→ planner `1i. **新机制批 spike 前置核` = 1；模板「新机制 spike 申报」= 1；uiux「母版命名族与版本声明纪律」= 1、「单一活图夹」= 1、过渡限定语 = 1；write-story 新指针 = 1、旧 `kidpal-ui-design-v3.html` = **0**；brief 旧 `](kidpal-ui-design-v6.html)` = **0**；requirement-change「设计稿版本以声明面」= 1；声明面「单一活图夹」= 1、过渡限定语 = 1、行首 `current:`（正则 `^current: `）= **1**、正文 **31 行**、指针文件 `kidpal-ui-design-v7.html` 在盘。逐条 = B23 列值。**绿**。
- **V4** · 条目 frontmatter 正反配对（归档位 `_archive/2026/` 三件）→ 每件：`status: deferred` = **0**、`status: closed` = 1、`closed: ""` = **0**、`closed: 2026-10-27` = 1、`closedBy: 261027-gov-review-p5-spike-anchor-cluster` = 1；结项 append 注记各 1 段。**绿**。
- **V5→V8** · `debt-render.mjs`（渲染已执行）→ `--check` exit 0（盘上 = 应渲染，V5 中间态已被渲染动作收口）、`--self-check: ok (10/10)`；三条目在 `docs/debt/gov/_archive/2026/`、主 INDEX 与归档 INDEX 已重渲。**绿**。
- **V6** · `git diff --name-only -- docs/contracts/ docs/requirements.md docs/architecture.md server/ firmware/ tools/ .claude/hooks/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json .claude/scripts/pre-merge-escalation-gate.mjs AGENTS.md CLAUDE.md docs/DEV-GOVERNANCE.md delivery/archive/` → **空输出**；`docs/ui/` 内 diff 恰 = `UIUX-DESIGN-BRIEF.md`（+ 新建 untracked `UI-DESIGN-VERSIONS.md` 在盘）。**绿**。
- **V7** · `landing-fidelity-check.mjs --preflight <PROPOSAL>` → `ok:true`、`anchorCount` 13、`anchorUnion` 与 manifest 十三行逐条一致、`findings:[]`、`advisories:[]`。**绿**。
- **V11** · `changes-index.mjs --upsert 261027-gov-review-p5-spike-anchor-cluster` 已在 FROZEN 标记后、落地 commit 前执行（draft / frozen 两次 upsert 均落）。**绿**。
- **真空绿规约**：负向断言（旧字面 0 残留 ×2、`status: deferred` 0 ×3、禁触面空 diff）均与正向配对在场；夹具判别力经 B2/B3 对拍（同夹具两版脚本结论相反）实证。

## 3. 诚实注记

1. **执行顺序注记（V5/V8 时点）**：实际落地序列 = 文件落盘 → debt-render 渲染 → 验证集复跑 → 本 summary；即 V5 描述的「翻转后、渲染前」中间态未单独观测（渲染紧随翻转执行），V8 的收口断言（`--check` exit 0 + 归位三件 + self-check 10/10）已实测成立。语义与 §5 预期一致，时点合并留痕。
2. **`--diff-ids` 传参形态**：`--diff-ids` 实参 = 相对 root 路径（用法见脚本 `:25`），非裸版本名；首跑误传 `v1 v2` 得 FATAL「设计稿不存在」（exit 2）——属调用方传参错误、非落地偏差（R2 判官与 B16 均以路径形态实测 exit 0）。留痕防后人重踩。
3. **夹具留存**：夹具配方脚本（`_scratch/make-fixtures.mjs`）已随 `_scratch/` 按环内自律删除，夹具本体在仓外 `C:/tmp/p5-assembly/fixtures/`（A–H4 十一形态）可复跑；配方重建路径见 FROZEN PROPOSAL §5「夹具配方」节。
4. **V9 / V10 未实测**：V9（post-commit 复跑 V1–V4）与 V10（升级闸，依赖 FROZEN + landing-verdict 凭据）按 §5 时点定义由落地提交后执行，不在本 summary 范围。
5. **伴生件**：落地 commit 另含本批制品（PROPOSAL / 本 summary / 后续 verdict / retro）、`delivery/review/INDEX.md`（upsert 生成）、`docs/debt/gov/{INDEX.md,_archive/2026/INDEX.md}`（debt-render 生成）与三条目归位移动（D + 新路径）、`delivery/workstreams/{INDEX.md,index.jsonl,task_plan.md}`（workstream 记账，hook 生成 + 主线程记账）；`.codebase-memory/` 两件常驻脏不 stage。

```landing-manifest
.claude/agents/planner.md
.claude/agents/uiux-author.md
.claude/references/change-triage.md
.claude/references/dev-loop.md
.claude/references/requirement-change.md
.claude/scripts/spec-anchor-check.mjs
.claude/skills/write-story/SKILL.md
docs/debt/gov/ARQ-260908-versioned-guard-repoint-execution.md
docs/debt/gov/ARQ-261017-anchor-check-master-unvalidated.md
docs/debt/gov/ARQ-261024-new-mechanism-spike-first.md
docs/templates/PROPOSAL.template.md
docs/ui/UI-DESIGN-VERSIONS.md
docs/ui/UIUX-DESIGN-BRIEF.md
```
