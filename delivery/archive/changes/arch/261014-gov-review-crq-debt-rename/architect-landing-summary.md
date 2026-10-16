# architect-landing-summary · 261014-gov-review-crq-debt-rename

packet-id：FROZEN PROPOSAL v0.3 §2 改法集（T1–T5）+ §5 验证集 · 评审轮次 R1 打回 → R2 打回 → R3 GREEN（2026-10-14，§7）· change-id 261014-gov-review-crq-debt-rename

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 目录改名 + 机制单源化 | `git mv docs/crq docs/debt`（142 件）；`.claude/scripts/{debt-ledger-shared,debt-render,debt-doctor}.mjs`（迁名 + LEDGER_ROOT 改 `['docs','debt']` + 硬编码字面量全部改经 `LEDGER_ROOT.join('/')` 派生〔doctor:43 / render:51、:201、:220 + 旧脚本名自指串 GENERATED_MARKER / indexHeader / self-check 输出串 / 互 import〕）；`crq-migrate.mjs` 保留原名 + retired 注（不复述旧字面） | 完成；`node --check` 四件全过；doctor 自检 13/13 |
| T2 结构双登记 | `.claude/scope-map.json` 三键（docsTopLevel.allowDirs + integrator.allow + adjudicator.allow）；`.claude/route-fixtures/surface-alignment.json` adjudicator 槽（allow + rationale，lastReviewed 更新 2026-10-14）；`.claude/rules/repo-structure.md` 表行 / 白名单 / 消歧表 / 引用纪律 / 语义说明段 | 完成；scope-map 与 repo-structure 各 3 处 debt、0 处 crq |
| T3 治理面 17 件 | agents 2 + commands 5 + references 7 + rules/contracts.md + skills 2：路径引用 + bare「CRQ」术语全量清扫 + 旧脚本名/模板名引用（crq-render / crq-doctor / crq-ledger-shared / crq-item 四名，含 gov-arq :27,51 模板名〔R2 QUESTION-1 补正〕）全部跟改新名 | 完成；route-regression-check 106 ok（--self-check）/ 53 ok（真实扫描）；lint-descriptions 干净 |
| T4 术语与文档面 | `docs/glossary.md` 三词条改写（debt 账本词条记沿革「原名 `crq`、261014 批改名」，不复述完整旧路径）；`docs/ARCH-REVIEW-QUEUE.md` stub 指路表 / 登记命令 / 两处脚本名；`docs/templates/debt-item.template.md`（迁名 + 内文）；`.gitattributes` 两条 union pattern 改指 `docs/debt/` + 注释块 | 完成 |
| T5 生成件重渲 | 6 份 INDEX（3 域 + 3 归档年）：删除旧生成件后 `node .claude/scripts/debt-render.mjs` 重渲（渲染器防冲掉闸拦住旧标记头文件——生成件删掉重渲是合法再生路径，无手改内容可丢） | 完成；条目 136 守恒（gov=98 tech=36 requirement=2）、advisory 172 同基线、RED 零命中、`--check` exit 0 |

## 2. 验证证据

- **pre 基线（2026-10-14 起草期，§5 留档）**：crq-doctor --self-check `ok (13/13)`；crq-render --check exit 0；doctor 真实仓 `条目 136 条：gov=98 tech=36 requirement=2` + `RED 零命中`；`git ls-files docs/crq | wc -l` = 142、INDEX = 6。
- **V1 残留清扫（八条 + 幂等复跑，落地后实跑 2026-10-14）**：活面 = 30 锚文件 + AGENTS.md / CLAUDE.md（README.md 仓内不存在）——斜杠形 / 分段引号形 / 裸段形 / 转义形 / 点号粘连形 / 大小写变体 / 旧名文件引用形（`crq-(render|doctor|ledger-shared|item)`）/ bare-CRQ 形（减 6 生成件）**全 0 命中**；幂等复跑二跑仍 0（无叠加损伤）。
- **V2 机检面**：`node --check` 四脚本 exit 0；`debt-doctor --self-check` → `debt-doctor --self-check: ok (13/13)`；`debt-doctor` 真实仓 → 条目 136 守恒 + RED 零命中；`debt-render --check` → exit 0 零漂移（真空绿规约：doctor 内建 0 条目 FATAL 防真空 + V3 计数守恒双闸）。
- **V3 守恒**：`git ls-files docs/crq | wc -l` = 0；`git ls-files docs/debt | wc -l` = 142；`git diff -M --cached --summary` = 146 个 rename，其中 `docs/debt` 非 INDEX 行 136 = 条目件全 rename（staged stat 逐件 `| 0` 零内容变更）；6 INDEX 除外（内容随 T5 重渲更新，属预期）。
- **V4 双登记一致**：`grep -c "docs/debt"` scope-map = 3、repo-structure = 3；`grep -c "docs/crq"` 两件 = 0 / 0。
- **V5 结构门基线**：`landing-fidelity-check.mjs --self-check` 全组 ok（含 isFrozen 19/19；advisory「conflicting-status-lines」为其夹具自身 first-wins 提示，非本批红）。
- **配套机检（authoring rule 义务）**：`route-regression-check.mjs --self-check` 106 ok / 真实扫描 53 ok；`lint-descriptions.mjs` scanned 16、flagged 0。
- **preflight**：`landing-fidelity-check.mjs --preflight` ok=true、anchorCount=34、anchorUnion ≡ manifest 34 行（R3 复核 + landing 后复跑同值）。

## 3. 诚实注记

- **渲染器防冲掉闸触发一次（已按其指引处置）**：旧 INDEX 携旧生成标记头，新标记不认 → 先 `rm` 6 份生成件再重渲；无手工内容损失（生成件 = 条目文件纯函数，doctor 136 守恒佐证）。
- **落地期新修一处（FROZEN 文本未预见）**：repo-structure 语义说明段起草时写「时名 `crq/`」带斜杠，V1 裸段形当场抓红——改为「时名 `crq`」后全绿。属落地自检抓出、与 FROZEN 改法语义一致（沿革注不复述旧路径形态），如实记档。
- **LF→CRLF add 警告 14 条**：仓内常态 autocrlf 行为（脚本文件工作区 LF、检出配置 CRLF），与 261013 批 LF-EOL 齿钉结论一致，非本批引入。
- **ADVISORY 留根**：route-fixtures 与 `.gitattributes` 实测均不在 GATE_LAYER_RULES G1–G4 正则内（§3 近似归类 + §6 留根，R1 注记 3）；census 判官复扫 710/169 与起草期 688/168 之差 = 本 PROPOSAL 自身（R1 注记 1）。
- **merge 未随本批执行**（human 2026-10-14 指示「landing 好了后先不要 merge，main 那边还有其他改动」）——升级闸与 ff 合并留待 human 通知；本批制品在 gov 分支冻结待合。

```landing-manifest
.claude/scripts/debt-ledger-shared.mjs
.claude/scripts/debt-render.mjs
.claude/scripts/debt-doctor.mjs
.claude/scripts/crq-migrate.mjs
.claude/scope-map.json
.claude/route-fixtures/surface-alignment.json
.claude/rules/repo-structure.md
.claude/rules/contracts.md
.claude/agents/adjudicator.md
.claude/agents/review-moderator.md
.claude/commands/arch-change.md
.claude/commands/loop-retro.md
.claude/commands/review-arch.md
.claude/commands/review-governance.md
.claude/commands/triage-change.md
.claude/references/arch-review-protocol.md
.claude/references/architect-operating-model.md
.claude/references/contract-change-evidence.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/requirement-change.md
.claude/references/change-intake.md
.claude/skills/gov-arq/SKILL.md
.claude/skills/merge-to-main/SKILL.md
docs/glossary.md
docs/ARCH-REVIEW-QUEUE.md
docs/templates/debt-item.template.md
.gitattributes
docs/debt/gov/INDEX.md
docs/debt/tech/INDEX.md
docs/debt/requirement/INDEX.md
docs/debt/gov/_archive/2026/INDEX.md
docs/debt/tech/_archive/2026/INDEX.md
docs/debt/requirement/_archive/2026/INDEX.md
```
