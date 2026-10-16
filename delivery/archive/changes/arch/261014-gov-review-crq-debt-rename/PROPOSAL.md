# PROPOSAL · 261014-gov-review-crq-debt-rename

> **状态：FROZEN v0.3 · 2026-10-14 · 凭据：human 立项 ack（2026-10-14 原话「commit merge to main」——OQ1/OQ2 未单独异议、按推荐位 (a)/(a) 执行，OQ 裁定注记见 §4；R1 打回两 BLOCK 属 OQ1(a) 已裁方向覆盖补全，human 追认随合并报告）+ architect R1→R2 打回、R3 GREEN（§7 评审记录，3 轮预算内熔断未触发）**。
> **承载 ARQ**：无（human 2026-10-14 直接发起：目录名 `crq` 不自明、实为 debt；讨论轮已裁三方向——不拆 bug/tech 域、不搬 `delivery/`、只改名，本 PROPOSAL 即根）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。governance 类变更不强制升级；升级判据零命中（不触 `docs/contracts/**`、非跨系统结构性变更）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（触点全在治理机制面 G2+G3，无实现面/契约面；全部为路径平移 + 引用批改、行为保持；但波及账本机制单源库与全部生成件，且活引用残留漏网会断治理面读写——260914 批曾为同类路径批改付四轮修复，故向严一档）。
> **review profile**：G2+G3（G2：`.claude/` agents/commands/references/rules/skills；G3：`.claude/` scripts/scope-map；`.claude/route-fixtures/` 实况不在 G1–G4 任何正则内，§3 显式归类、§6 留根。G1 与契约面零触点——`AGENTS.md` / `CLAUDE.md` / `README.md` / `docs/contracts/**` 经 2026-10-14 census 实测零命中）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

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

---

## 0. 想实现的效果与差距

目标效果：未结决策项账本的目录名与其实义对齐——`docs/crq/` → `docs/debt/`（欠账台账，名即义），仓内全部活引用一次跟改到位，机检面（doctor / render / scope-map↔repo-structure 双登记）同步绿。

现状差距：目录名 `crq` 是仓内缩写、不自明——全仓唯一展开处是 `.claude/rules/repo-structure.md` 表行注「CRQ = change request 账」（2026-10-14 核实：`docs/glossary.md:35` 词条亦只写「CRQ 账本」未展开）。human 2026-10-14 判「叫这个名字很变扭，其实就是 debt」。

非目标（本批不做什么，防 scope creep）：不拆 bug/tech 域（human 2026-10-14 讨论轮已裁）；不搬 `delivery/`（同轮已裁，账本留 `docs/`——理由：`delivery/` 为单次交付工件面且已被裁出 CodebaseMemory 主图，账本是 agent 高频读写的活状态面）；不改条目 ID 前缀（ARQ / TECH / REQ 全保留，`ID_RE` 零变更）；不追改历史制品（`delivery/**`、冻结验收件注释、`docs/ADR.md`、`docs/CONTRACT-DECISION-QUEUE.md` 行文、`docs/showme/` 快照、账本条目 append-only 正文内的旧路径引文——「引用当时位置」为既成惯例，验收件注释引用已归档搬家的条目路径即先例）；不重构账本字段、域配置与归档机制（`DOMAINS` / `STATUSES` / `SEVERITIES` 等闭集零变更）。

## 2. 问题、根因与改法（T1–T5）

**【硬项 · 起草期实证义务】** 本节全部断言附 2026-10-14 起草期实测（命令 + 关键输出 verbatim + 日期）；census 一律机械枚举生成（命令见 §3）。

**【硬项 · 目标文件锚清单】** 每 T 末尾显式列目标文件；改法标题一律 `### T<n>` 形态。迁移裁腿说明（136 条目件不入锚）：先例 = 260914 批 landing 期结构修正，「迁移锚只记迁入路径（四目录根）」（`delivery/changes/arch/260914-gov-review-delivery-restructure/PROPOSAL.md:152`）+「目录迁移整裁腿——四目录根为迁入锚、git diff 不产目录级变更不可机判」（同件 :222，历史面件零改动不产锚）。本批同裁：136 个条目文件为**纯 rename、零内容变更**，不入 manifest 锚，守恒与纯 rename 核验归 §5 V3 机检断言；6 份生成件 INDEX 因内容随渲染更新，**入锚**。

### T1 · 账本目录改名与机制脚本路径单源化
**问题**：账本目录名 `docs/crq` 不自明（缩写债）；机制脚本内路径表示双源——`LEDGER_ROOT` 分段引号形与 doctor 硬编码斜杠形并存，改名后两源须同步、漏一处即机检断链。
**证据**：`.claude/scripts/crq-ledger-shared.mjs:19` verbatim：`export const LEDGER_ROOT = ['docs', 'crq'];`；`.claude/scripts/crq-doctor.mjs:43` verbatim：`` const at = `docs/crq/${DOMAINS[domain].dir}/${rel}`; ``（硬编码前缀，未复用 LEDGER_ROOT）。2026-10-14 八形态扫描（模式与命令见 §3）：斜杠形活面 49 件（枚举 = §3 census）、分段引号形恰 1 处（即 LEDGER_ROOT）、相对链接/裸段形 4 处（ARCH-REVIEW-QUEUE.md 指路表 3 + repo-structure.md:40 白名单段 1）、反斜杠形 / 转义形 / 点号粘连形 / C 双引号实参形均 0 命中。
**根因**：260923 批（`260923-gov-review-arq-ssot-restructure`）自单文件 ARCH-REVIEW-QUEUE 重构为账本时沿用旧队列字母缩写起名，未过 Instruction Budget 三问的「数月后仍立」检验；doctor 起草期展示路径硬编码、未从 LEDGER_ROOT 派生。
**改法**：① `git mv docs/crq docs/debt`（142 件整体平移——136 条目 + 6 生成件 INDEX，条目正文与 frontmatter 零改动）；② `LEDGER_ROOT` 改 `['docs', 'debt']`；③ 脚本内一切硬编码 `docs/crq` 字面量（doctor:43 起）改由 LEDGER_ROOT 派生，消双源；**同时脚本内一切旧脚本名自指串一并改指新名**（R1 BLOCK-1 正形）——含 `crq-ledger-shared.mjs:204` 的 `GENERATED_MARKER`、`crq-render.mjs:48-50` indexHeader 模板串与 :227 注释串、doctor / render 的 self-check 输出串与互 import 行（doctor:13-14 / render:24，R1 注记 5 声明覆盖关系）——T5 重渲后 6 份 INDEX 头注不得再写死已不存在的旧脚本名；复扫命令见 §5 V1（含「旧名文件引用形」），命中须 0；④ 脚本重命名（OQ1(a) 推荐案）：`crq-ledger-shared.mjs` → `debt-ledger-shared.mjs`、`crq-render.mjs` → `debt-render.mjs`、`crq-doctor.mjs` → `debt-doctor.mjs`；`crq-migrate.mjs` **保留原名不删不改名**（一次性已完成工具，头注补 retired 注 + import 行随迁改指 debt-ledger-shared.mjs；**retired 注不复述旧路径与旧脚本名字面**——写「261014 批随账本改名，路径与依赖已迁 `docs/debt` / `debt-ledger-shared.mjs`」，免与 V1 斜杠形 / 旧名引用形互锁〔R1 QUESTION-2 正形〕）。OQ1 裁 (b) 时本条收缩为：脚本文件名保留 `crq-*`，仅内容路径跟改，manifest 锚名随之再透明小修。
目标文件：`.claude/scripts/debt-ledger-shared.mjs`（迁移+改） `.claude/scripts/debt-render.mjs`（迁移+改） `.claude/scripts/debt-doctor.mjs`（迁移+改） `.claude/scripts/crq-migrate.mjs`（改）

### T2 · 结构双登记同步（scope-map ↔ repo-structure ↔ route-fixtures）
**问题**：`docs/` 根准入是双登记结构——`.claude/rules/repo-structure.md` 白名单与 `.claude/scope-map.json` 的 `universal.docsTopLevel` 必须一致（enforce-scope 钩子按 scope-map 机判，repo-structure 是结构 SSOT）；另有三个写槽 token 与一份对齐 fixture 引用旧目录。
**证据**（2026-10-14 机械枚举，`python -c` 遍历 scope-map 全键，输出 verbatim）：`/universal/docsTopLevel/allowDirs = docs/crq`、`/agents/integrator/allow = docs/crq/**`、`/agents/adjudicator/allow = docs/crq/**` 三键；`.claude/route-fixtures/surface-alignment.json` adjudicator 槽 allow 含 `"docs/crq/**"` 且 rationale 注记「docs/crq/** added by 260923-gov-review-arq-ssot-restructure」；repo-structure.md 共 6 处 `docs/crq`（表行 / 白名单行 / 三同名物消歧表 / 语义说明段）+ 1 处相对段形（:40 `` `crq/` ``）。
**根因**：260923 批铺账本时的登记面即此四处，目录名变则四处联动；双登记一致性是 repo-structure.md 自身钉死的义务（「两处必须一致」）。
**改法**：三键同步 `docs/crq` → `docs/debt`；route-fixtures adjudicator 槽 allow 与 rationale 注记跟改；repo-structure 表行 / 白名单 / 消歧表 / 语义说明段同步（行注「CRQ = change request 账」随改名一并消亡，见 T4 glossary 沿革注）。
目标文件：`.claude/scope-map.json` `.claude/route-fixtures/surface-alignment.json` `.claude/rules/repo-structure.md`

### T3 · 治理面定义与参考件跟改（agents / commands / references / rules / skills）
**问题**：账本路径被治理面机制文件直接引用（ adjudicator 的按域更新职责、gov-arq 的登记落点、loop-retro 的 ARQ 产出面、评审协议的账本检查步骤等），改名后不同步即指向断链、按旧路径写入会被 enforce-scope 拦。
**证据**（2026-10-14 census，`grep -rln "docs/crq"` 活面）：`.claude/agents/adjudicator.md`（4 处）、`.claude/agents/review-moderator.md`（1 处）、`.claude/commands/` 4 件（arch-change / loop-retro / review-arch / review-governance）、`.claude/references/` 6 件（arch-review-protocol / architect-operating-model / contract-change-evidence / dev-loop-operating-system / dev-loop / requirement-change）、`.claude/rules/contracts.md`（ARQ-260827 账本条目指针）、`.claude/skills/gov-arq/SKILL.md`（6 处）、`.claude/skills/merge-to-main/SKILL.md`（1 处）。另有术语件 2 件只含「CRQ」字样不含路径（OQ1(a) 跟改）：`.claude/commands/triage-change.md`、`.claude/references/change-intake.md`。
**根因**：账本是治理面的高频读写对象，机制文件必然钉它的路径；本批是改名批改的自然联动面。
**改法**：上述 17 件路径引用批改 + **bare「CRQ」术语全量清扫**（OQ1(a)，R1 BLOCK-2 正形）：判据 = 活面 `grep -rn "CRQ"` → 0 命中（见 §5 V1 bare-CRQ 条），存量显式点名——`requirement-change.md:60`「REQ- 域 CRQ 闭环纪律」、`glossary.md:37`（CDQ 词条）「CRQ tech 域」、`docs/templates/crq-item.template.md:1`「CRQ 条目文件骨架」；另 `glossary.md:36`（ARQ 词条）「已重构为 CRQ 账本」句并入沿革注一并改写；沿革注 / retired 注一律**不复述旧路径与旧脚本名字面**（免 V1 互锁）。**旧脚本名与旧模板名引用跟改**（OQ1(a)，R1 BLOCK-1 正形）：17 件内 `crq-render` / `crq-doctor` / `crq-ledger-shared` / `crq-item` 四名引用（判官逐行核得：`arch-change.md:42`、`review-arch.md:31`、`review-governance.md:33`、`arch-review-protocol.md:170`、`dev-loop-operating-system.md:158`、`dev-loop.md:337`、`requirement-change.md:66`、`gov-arq SKILL.md:40,42,58,71,78`、`merge-to-main SKILL.md:18`、`repo-structure.md:22,47`、`adjudicator.md` 与 `loop-retro.md`〔模板名〕、`gov-arq SKILL.md:27,51`〔模板名，R2 QUESTION-1 补正——判官 R2 实测模板名引用在 :27/:51，:40,42,58,71,78 为脚本名引用〕）全部跟改新名。agent 正文批改遵守 prompt-authoring 卫生：权限边界句只改路径 token、不动语义（Pruning 硬边界 HQ-1）。
目标文件：`.claude/agents/adjudicator.md` `.claude/agents/review-moderator.md` `.claude/commands/arch-change.md` `.claude/commands/loop-retro.md` `.claude/commands/review-arch.md` `.claude/commands/review-governance.md` `.claude/commands/triage-change.md` `.claude/references/arch-review-protocol.md` `.claude/references/architect-operating-model.md` `.claude/references/contract-change-evidence.md` `.claude/references/dev-loop-operating-system.md` `.claude/references/dev-loop.md` `.claude/references/requirement-change.md` `.claude/references/change-intake.md` `.claude/rules/contracts.md` `.claude/skills/gov-arq/SKILL.md` `.claude/skills/merge-to-main/SKILL.md`

### T4 · 术语与文档面收口（glossary 沿革注 / stub 指路表 / 条目模板 / gitattributes）
**问题**：术语权威与指路件须随改名收口，否则读者循旧词找不到新家。
**证据**：`docs/glossary.md:35-37` 三词条（CRQ 账本 / ARQ / CDQ）均含 `docs/crq` 路径；`docs/ARCH-REVIEW-QUEUE.md`（指路 stub）指路表 3 行为相对链接形（verbatim 例：`` [`docs/crq/gov/INDEX.md`](crq/gov/INDEX.md) ``）+ 登记命令行含 `crq-render`；`docs/templates/crq-item.template.md` 为条目骨架模板（gov-arq 登记时引用）；`.gitattributes:28-29` verbatim：`docs/crq/*/INDEX.md merge=union`、`docs/crq/*/_archive/*/INDEX.md merge=union`（生成件 union 合并属性，指向旧路径即失效）。
**根因**：同 T2/T3——改名联动面；glossary 是术语 SSOT、stub 是断引用保活件、模板是登记入口、gitattributes 是合并机检，四者都钉目录名。
**改法**：glossary 三词条路径跟改，「CRQ 账本」词条改名「debt 账本」并**记沿革**（沿革注写「原名 `crq`、261014 批改名」——**不复述完整旧路径**，免 V1 斜杠形互锁〔R1 QUESTION-2 正形〕；OQ2(a) 推荐案下此沿革注即旧路径的轻量指路替代）；stub（`docs/ARCH-REVIEW-QUEUE.md`）指路表、登记命令行与全文两处脚本名（:19 `crq-doctor.mjs` 提及 + `crq-render` 登记，R1 BLOCK-1 正形）跟改；条目模板改名 `debt-item.template.md` 并跟改内文（:1「CRQ 条目文件骨架」题行与内文路径，R1 BLOCK-2 正形）；`.gitattributes` 两条 pattern 改指 `docs/debt/`、注释块跟改。
目标文件：`docs/glossary.md` `docs/ARCH-REVIEW-QUEUE.md` `docs/templates/debt-item.template.md`（迁移） `.gitattributes`

### T5 · 生成件重渲染
**问题**：6 份 INDEX 是生成件，头注含「引用本文件必须带目录（`docs/crq/…`）」等由渲染模板拼出的路径行，目录改名后须重渲才算归位（render 自带归档归位 + 渲染，盘上旧内容即 drift）。
**证据**：`.claude/scripts/crq-render.mjs:48-51` 头注模板 verbatim 含 `docs/crq/${DOMAINS[domain].dir}/${rel}` 拼接；2026-10-14 基线实测：`find docs/crq -name 'INDEX.md' | wc -l` = 6（gov / tech / requirement 三域 + 三域 `_archive/2026/`）。
**根因**：生成件内容是 LEDGER_ROOT 与域配置的纯函数，机制改对后重跑即自愈——归 V5 不归手改（生成件勿手改纪律）。
**改法**：落地末跑 `node .claude/scripts/debt-render.mjs`（归档归位 + 全 INDEX 重渲），随后 `--check` 验零漂移（§5 V2）。
目标文件：`docs/debt/gov/INDEX.md` `docs/debt/tech/INDEX.md` `docs/debt/requirement/INDEX.md` `docs/debt/gov/_archive/2026/INDEX.md` `docs/debt/tech/_archive/2026/INDEX.md` `docs/debt/requirement/_archive/2026/INDEX.md`

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

**census 命令与日期**（2026-10-14，gov worktree）：

- 全仓总量：`grep -rln "docs/crq" .`（排除 `.git` / 参考树 / `node_modules`）→ **688 处 / 168 文件**（含 `delivery/**` 历史制品大头）。
- 活引用面（全仓减 `delivery/`、减 `.codebase-memory/`——**含**下表历史豁免行所列 showme 3 件、验收件 6 件、ADR / CDQ 与条目旧引文子面，QUESTION-R1-1 口径修正）→ **49 件**，机械枚举清单 = 下表「改面」与「历史豁免」各行合计；断链风险全部集中在改面 34 锚（sweep 表对 49 件逐行分类无遗漏——R1 复核确认）。另记：census 时点早于本 PROPOSAL 落盘，判官复扫全仓 710 处 / 169 件，差值恰为本 PROPOSAL 自身 22 处 / 1 件（良性自增，R1 注记 1）。
- 八形态扫描命令逐类记录于 §2 T1 证据段与 §5 V1（模式与验收判据同一条，path-form-family §四 教训）。

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G3 机制脚本 | `.claude/scripts/crq-ledger-shared.mjs` · `crq-render.mjs` · `crq-doctor.mjs` · `crq-migrate.mjs` | G3（改） | LEDGER_ROOT + doctor:43 硬编码 + 头注；migrate 保留名、retired 注 |
| G3 结构登记 | `.claude/scope-map.json` | G3（改） | 三键：docsTopLevel + integrator/adjudicator allow |
| G3 合并属性 | `.gitattributes` | G3（改；实测不在 GATE_LAYER_RULES G1–G4 正则内，治理面近似归类——R1 注记 3） | union 两 pattern 行 + 注释块 |
| G2 治理面定义 | `.claude/agents/` 2 件 · `commands/` 5 件 · `references/` 7 件 · `rules/` 2 件 · `skills/` 2 件 | G2（改） | 18 件路径 + 术语批改；清单 = §2 T2/T3 锚 |
| 配套 fixture | `.claude/route-fixtures/surface-alignment.json` | G2 邻配套件（改） | **不在 G1–G4 任何正则内**（GATE_LAYER_RULES 实测核对），显式归类；盲区 §6 留根 |
| 结构 SSOT | `.claude/rules/repo-structure.md` | G2（改） | 表行 + 白名单 + 消歧表 + 语义说明段 |
| 术语/指路/模板 | `docs/glossary.md` · `docs/ARCH-REVIEW-QUEUE.md` · `docs/templates/crq-item.template.md` | docs 根（改） | 词条改写含沿革注；stub 指路表；模板迁名 |
| 生成件 | `docs/crq/**/INDEX.md` 6 份 | 生成件（改，随 T5 重渲） | 由 LEDGER_ROOT 派生，重跑 render 自愈 |
| 迁移件（裁腿） | `docs/crq/**` 条目 136 件 | 迁移（零内容变更，不产锚） | 纯 rename；守恒 = §5 V3；先例 260914 批 :152/:222 |
| 历史制品豁免 | `delivery/**`（review-packet / docs-delta / taskpackage / verify-runs 等）· `firmware/main/test/story_page_v2.acceptance.c:257` 等 `server/acceptance/` 5 件 · `docs/ADR.md` · `docs/CONTRACT-DECISION-QUEUE.md` · `docs/showme/` 3 件 · 条目正文旧路径引文 5 件 | 未触（历史豁免） | append-only 纪律；「引当时位置」惯例（验收件注释引已归档条目路径即先例）；CDQ/ADR 命中经核全为已结行历史行（260914 批同裁先例 :152） |
| 结构门脚本 | `.claude/scripts/landing-fidelity-check.mjs` | 未触 | 两处命中均为 :244 历史实录注释 + :340-341 合成输入自检夹具，非活引用 |
| 生成簿记 | `delivery/changes/arch/INDEX.md` · `.codebase-memory/` 两件 | 未触（生成件自更） | changes-index `--upsert` 自更；codebase-memory 工具自刷新面，不追交 |

**【硬项 · 复扫模式完备性先验】** 本批属残留清扫类，且波及**两个 token 族**（R1 BLOCK-1 补记）：① 目录 token `docs/crq`——path-form-family 八类逐类子检（§5 V1 前六条 + bare-CRQ 条）；② 改名文件名 token `crq-(render|doctor|ledger-shared|item)`——其带路径与裸文件名两种表示由 §5 V1 第七条单模式一并覆盖。形态本身未出新类（① 属八类已知形态、② 属斜杠形 / 裸段形对另一 token 的应用）——按族谱表 §6 用法**无需 append 新行**，本段即补记，供 R2 复核。`crq` 为仓内专名：无大小写变体（2026-10-14 `grep -rni "docs/crq"` 活面 0 命中佐证）、无反斜杠形 / 转义形 / 点号粘连形（各 0 命中）、C 实参形命中全在豁免面注释内。

## 4. human 取舍点（OQ）

**OQ1 · 改名深度：全套一致 or 只改目录？**（human 裁定 2026-10-14：(a)——立项 ack 原话「commit merge to main」未单独异议，按推荐位执行）
- (a) **全套一致（推荐）**：目录 + 三件机制脚本重命名（`debt-ledger-shared` / `debt-render` / `debt-doctor`）+ 条目模板改名 + prose 术语「CRQ 账本」→「debt 账本」。推荐理由：`crq` 这个不自明的缩写正是本批要除的债——留一半（脚本名/术语保留）会让每个新起草的文档继续繁殖旧缩写；全套一次清，引用面 grep 可枚举、一次跟改。
- (b) 最小面：只改目录与指向它的路径引用，脚本文件名与「CRQ」术语保留。代价：churn 略小，但缩写债永续，且「目录叫 debt、脚本叫 crq-render」的不一致本身成为新债。
- `crq-migrate.mjs` 两案下均保留原名（一次性已完成工具，历史件），不设选项。
- 附带背景：脚本重命名会触发 `KidPal-claude` 图索引（`.claude/scripts` 入图面）自刷新，属工具常态自愈、无人工动作。

**OQ2 · 旧路径处置：留不留指路 stub？**（human 裁定 2026-10-14：(a)——同上，按推荐位执行）
- (a) **不留 stub（推荐）**：旧路径沿革记入 glossary「debt 账本」词条 + git rename 历史可查；`docsTopLevel` 白名单不增行。推荐理由：历史引用按「引当时位置」惯例本就不要求可达（ARCH-REVIEW-QUEUE stub 的存在理由是断**活跃**引用，本批活跃引用全部跟改、只剩历史引用）；留 stub 则 `docs/` 根永挂一个双行白名单。
- (b) 留 `docs/crq/` 目录 stub（比照 `docs/ARCH-REVIEW-QUEUE.md` 文件级 stub 先例）：代价 = `docsTopLevel` 与 repo-structure 白名单双行并存、`docs/` 边界说明再动一次（「docs 子目录数再增两个」复议触发计数 +1）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**pre 基线（2026-10-14 起草期已实跑，输出 verbatim 留档）**：
- `node .claude/scripts/crq-doctor.mjs --self-check` → `crq-doctor --self-check: ok (13/13)`
- `node .claude/scripts/crq-render.mjs --check` → exit 0、无输出（零漂移静默即绿）
- `node .claude/scripts/crq-doctor.mjs`（真实仓）→ `条目 136 条：gov=98 tech=36 requirement=2`、`advisory 172 条（不阻断）`、`RED 零命中 —— ok`
- `git ls-files docs/crq | wc -l` → 142；`find docs/crq -name 'INDEX.md' | wc -l` → 6

**V1 · 残留清扫（pre-commit；post-commit 复跑一遍）**——模式与 §3 census 同一条（path-form-family §四：清扫与判据同源），活面 = 改面 34 锚所在文件集 + `AGENTS.md`/`CLAUDE.md`/`README.md`：
- 斜杠形：`grep -rn "docs/crq" <活面文件集>` → 0
- 分段引号形：`grep -rnE "['\"]docs['\"]\s*,\s*['\"]crq['\"]" .claude docs` → 0
- 相对链接/裸段形：`grep -rnE "[^a-zA-Z0-9_/-]crq/" .claude docs .gitattributes` → 0（OQ1(b) 案下需补第二段过滤排除 `crq-migrate` 文件名——按裁定调）
- 转义形：`grep -rn 'crq\\' .claude docs` → 0
- 点号粘连形：`grep -rnE '\.[a-zA-Z]*crq' .claude docs` → 0（排除 crq- 前缀文件名形态）
- 旧名文件引用形（R1 BLOCK-1 新增，第二个 token 族）：`grep -rnE "crq-(render|doctor|ledger-shared|item)" <活面文件集>` → 0（判官起草期预扫：`crq-migrate.mjs` 自身零命中该模式；其 retired 注按 §2 T1 ④ 正形不复述旧名，免互锁）
- bare「CRQ」术语形（R1 BLOCK-2 新增；R2 BLOCK-1 缩圈）：`grep -rn "CRQ" <活面文件集减 6 份生成件 INDEX>` → 0。生成件 INDEX 豁免理由：已结条目 frontmatter `closedBy` 的历史记名（「2026-10-12 tech 域 CRQ 盘点」）经重渲原样带入 `tech/_archive/2026/INDEX.md:19–:26` 共 7 处（起草期亲核 2026-10-14：`grep -cn "CRQ"` 六份 INDEX 仅 tech 归档年 7、余五份 0），属 append-only 历史记名不可回改、V3 similarity 断言钉其纯 rename。其余活面白名单为空——沿革注 / retired 注一律不复述旧名旧路径字面；showme 与历史制品不在活面集
- 幂等复跑（风险注记①）：V1 各命令复跑一次，命中仍 0（非 0 即叠加损伤，回退单轮重做）

**V2 · 机检面（pre-commit；post-commit 复跑）**：
- `node --check` 全部改名后脚本 + `crq-migrate.mjs` → 各 exit 0
- `node .claude/scripts/debt-doctor.mjs --self-check` → `debt-doctor --self-check: ok (13/13)`（自检名随脚本头注更新）
- `node .claude/scripts/debt-doctor.mjs`（真实仓）→ `条目 136 条：gov=98 tech=36 requirement=2` 守恒 + `RED 零命中 —— ok`（防真空：doctor 内建 0 条目 FATAL，`crq-doctor.mjs:121` verbatim `FATAL docs/crq/** 下 0 条目——真空输入，拒绝判绿`——路径字面随改名更新，规约不变）
- `node .claude/scripts/debt-render.mjs --check` → exit 0、无输出（零漂移）

**V3 · 守恒与纯 rename 核验（pre-commit）**：
- `git ls-files docs/crq | wc -l` = 0；`git ls-files docs/debt | wc -l` = 142（基线同上）
- `git diff -M --cached --summary`：136 条目文件全为 `rename` 行、similarity 100%（零内容变更的人读级断言）；6 INDEX 除外（内容随 T5 重渲更新）

**V4 · 双登记一致性（pre-commit；post-commit 复跑）**：scope-map `docsTopLevel.allowDirs` 与 repo-structure 白名单行逐 token 相等且只含 `docs/debt`（OQ2(a) 案不含 `docs/crq`）——人读核对 + `grep -n "docs/crq" .claude/scope-map.json .claude/rules/repo-structure.md` = 0 命中辅助。

**V5 · 结构门基线（pre-commit；post-commit 复跑）**：`node .claude/scripts/landing-fidelity-check.mjs --self-check` → ok（本批零接触该脚本，基线保持）。

**post-commit（landing-verifier 侧）**：V1 / V2 / V4 复跑 + 逐文件人读 diff（34 锚逐文件核对实际落地 == frozen 清单；136 条目按 V3 rename 汇总核对）。

## 6. 留根清单

- **route-fixtures G 层归属盲区**：`.claude/route-fixtures/` 不在 GATE_LAYER_RULES G1–G4 任何正则内（2026-10-14 实测核对），本批显式归类为 G2 邻配套件；盲区本身登记不硬治，交 cadence 复盘。
- **旧路径无 stub（OQ2(a) 案）**：glossary「debt 账本」词条沿革注 = 唯一改名记录点；若未来出现旧路径高频迷路再议 stub。
- **「CRQ = change request」展开定义随本批消亡**：repo-structure 行注是全仓唯一展开处，改名后不再有该缩写的权威定义；未来若需要 change request 语义，另起新名不复用。
- **条目 append-only 正文旧路径引文保留**（5 件，§3 豁免行）：读者凭 glossary 沿革注可解；不回改历史散文。
- **八形态族谱表本批无新形态**，无需 append。
- **用户级 memory（仓外自维护面）**：主 AI 的 auto-memory 里有 `docs/crq` 路径引用，landing 后自更，不入本批 manifest。
- **自律件**：落地时 scope-map 先改（enforce-scope 钩子按 scope-map 机判，先改它再 `git mv` 才不拦新路径写入）；单 commit 原子落（mv + 全部跟改 + 重渲同 commit stage）；`git add` 后照 Hard Rules 跑 `git diff --cached --stat` 核对再提交。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-10-14 · architect

**结论：打回（2 项 BLOCK、2 项 QUESTION）**——T1–T5 改法方向、全部承重引文、census 断链面分类、136 条目裁腿先例、pre 基线四命令、manifest 对账均经判官独立实测复核成立；两项 BLOCK 同根（OQ1(a) 全套改名下，「旧脚本名 / 旧模板名 / bare『CRQ』术语」三类旧名残留面既未被 §2 改法覆盖面点名、又无 §5 V1 任何形态模式可机检——V1 全绿不等于旧名清零），按正形修订后 R2 复核，预期 GREEN。两 BLOCK 均触 §0 目标效果句「仓内全部活引用一次跟改到位 / 名即义」，按 canonical「影响目标效果的打回项升级 human」记**升级 human**；正形不新增设计、不扩 landing-manifest，human 追认修订即可。

**逐项复核（判官自证，2026-10-14 本 worktree 亲跑）**：

- **T1/T2/T4 承重引文逐字复核**：`crq-ledger-shared.mjs:19` = `export const LEDGER_ROOT = ['docs', 'crq'];` ✓；`crq-doctor.mjs:43` 硬编码模板串 verbatim ✓；`crq-doctor.mjs:121` FATAL 串（防真空绿）verbatim ✓；`crq-render.mjs:48-51` 头注模板含 `docs/crq/` 拼接 ✓；`.gitattributes:28-29` 两条 union pattern verbatim ✓；`docs/glossary.md:35-37` 三词条均含 `docs/crq` 路径 ✓；scope-map 三键（python 全树遍历，输出 = `/universal/docsTopLevel/allowDirs[5] = docs/crq`、`/agents/integrator/allow[4] = docs/crq/**`、`/agents/adjudicator/allow[3] = docs/crq/**`）恰三键、无第四处 ✓；route-fixtures adjudicator 槽 allow 含 `"docs/crq/**"` 且 rationale 注记 260923 批字样 verbatim ✓；repo-structure.md `docs/crq` 恰 6 处 + :40 相对段形 `` `crq/` `` ✓；T3 逐文件计数抽查 adjudicator=4 / review-moderator=1 / gov-arq=6 / merge-to-main=1 / contracts.md=1 全符 ✓；术语件 `triage-change.md` / `change-intake.md` 实测确只含 bare「CRQ」不含 `docs/crq` 路径 ✓。
- **基线四命令逐字复现**：`node .claude/scripts/crq-doctor.mjs --self-check` → `crq-doctor --self-check: ok (13/13)`；`crq-render.mjs --check` → exit 0、无输出；doctor 真实仓 → `条目 136 条：gov=98 tech=36 requirement=2` + `advisory 172 条（不阻断）` + `RED 零命中 —— ok`；`git ls-files docs/crq | wc -l` = 142、`find docs/crq -name 'INDEX.md' | wc -l` = 6——与 §5 pre 基线四条逐字全同 ✓。
- **census 总量与活面复核（判官补扫，含 §3 枚举命令未覆盖的死角）**：判官实测全仓 710 处 / 169 件（排除 `.git` / `.codebase-memory`；本 worktree 无参考树与 node_modules 命中），与 §3 记 688 / 168 之差恰为本 PROPOSAL 自身 22 处 / 1 件（census 后落盘，良性，见注记 1）；活面口径可复算：`.claude` 23 件 + `.gitattributes` + docs 根 4 件（glossary / ARCH-REVIEW-QUEUE / 模板 / ADR+CDQ 计 docs 侧）+ `docs/crq/**` 内 11 件（6 INDEX + 5 条目旧引文）+ showme 3 件 + 验收件 6 件 = 49 件 ✓；判官无扩展名过滤 `crq` token 补扫 `.claude` 与 `docs`，唯一 census 外命中 = `.claude/skills/archify/package-lock.json:59` sha512 integrity base64 串内巧合子串（`QcrqEN1`），非引用、无动作面；大小写变体（`grep -rni "docs/crq"` 活面）0、反斜杠形 0、点号粘连形（除 crq- 前缀文件名）0 ✓；`landing-fidelity-check.mjs` 两处命中（:244 历史实录注释 / :340-341 self-check 合成输入夹具）定性核实为非活引用 ✓。
- **manifest 对账**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` → `ok=true`、anchorCount=34、anchorUnion 与 landing-manifest 围栏块 34 行逐一相等（判官逐行比对，两集合全等）✓；`--self-check` 全绿 ✓。
- **136 裁腿不产锚先例引文真实性**：`260914-gov-review-delivery-restructure/PROPOSAL.md:152`「迁移锚只记迁入路径（四目录根）」与 `:222`「目录迁移整裁腿——四目录根为迁入锚、git diff 不产目录级变更不可机判」两处 verbatim 在案；:152 同段并载 CDQ「census 命中经核全为已结行历史行」同裁记录，§3 历史豁免行引其为例属实 ✓。
- **route-fixtures G 层断言实测**：`crq-ledger-shared.mjs` `GATE_LAYER_RULES` 四正则逐条比对——`.claude/route-fixtures/` 不命中 G1–G4 任何一条 ✓（§8 review profile 行与 §3 配套 fixture 行断言成立）；顺带核得 `.gitattributes` 亦不在四正则内（见注记 3）。
- **§0 非目标 vs human 讨论轮三裁**：不拆 bug/tech 域、不搬 `delivery/`、只改名三边界一致；ARQ / TECH / REQ 前缀与 `ID_RE` 零变更、`DOMAINS` / `STATUSES` / `SEVERITIES` 闭集零变更声明与 shared 实况相符 ✓；136 条目纯 rename + 6 INDEX 入锚 + 模板/脚本迁名计入锚的裁腿边界与先例形态一致 ✓。
- **OQ 裁定凭据（判官显式裁定，非默采）**：**裁定成立、采信**。理由：① 两问均以「推荐位 + 反选代价」形态在 PROPOSAL 正文在案，ack 原话「commit merge to main」是对整批（含已标推荐位的取舍点）的放行语；② OQ1(a) 与 human 自述动机「叫这个名字很变扭，其实就是 debt」同向——留一半缩写与该动机相斥，判官不认为存在「human 想要 (b) 但被代理私取 (a)」的实质分叉；③ 两问任一事后反选均为机械可逆小修（脚本名回改 / 补 stub 皆可枚举跟改），不构成不可逆损伤。注记留档：该 ack 为「整体放行」型凭据而非逐问裁定，仓内无证据可核验 ack 时两问是否已随前端对齐出示；若 human 事后反选，回改操作即本批 V1 各命令之逆，枚举可行。

**打回项**：

- **BLOCK-R1-1 · OQ1(a) 全套改名下，旧脚本名 / 旧模板名引用面无改法点名、无 V1 形态覆盖——机械断链 + 生成件死指针自我复制**。T1 ③ 只覆盖 `docs/crq` 字面量（「doctor:43 起」+ §5 V1 复扫），T3 改法只说「路径引用批改 + 术语两短语」，而 OQ1(a) 把三件机制脚本与条目模板改名后，以下旧名引用全部悬空：① 指令面 11 件（均已在 34 锚内、判官逐行亲核）：`commands/arch-change.md:42`、`commands/review-arch.md:31`、`commands/review-governance.md:33`（各含 `node .claude/scripts/crq-render.mjs`）、`references/arch-review-protocol.md:170`、`references/dev-loop-operating-system.md:158`、`references/dev-loop.md:337`（crq-render + crq-doctor）、`references/requirement-change.md:66`、`skills/gov-arq/SKILL.md:40,42,58,71,78`（crq-doctor / crq-ledger-shared / crq-render）、`skills/merge-to-main/SKILL.md:18`、`rules/repo-structure.md:22,47`、`docs/ARCH-REVIEW-QUEUE.md:19`（`crq-doctor.mjs` 提及未点名；`crq-render` 提及虽被 T4「登记命令行」点名，同 bullet 未全覆盖）；② 生成链自复制死指针：`crq-ledger-shared.mjs:204` `GENERATED_MARKER` 与 `crq-render.mjs:48-50`（indexHeader 模板）、`:227` 注释串均含 `crq-render.mjs` / `crq-doctor.mjs` 字样——T1 ③「一切 `docs/crq` 前缀」不覆盖它们，T5 重渲后 6 份 INDEX 头部将继续写死已不存在的旧脚本名，且每次渲染逐字复制，与「名即义」目标相反；③ 模板名 `crq-item.template.md` 被 `adjudicator.md` / `loop-retro.md` / `gov-arq SKILL.md` / `ARCH-REVIEW-QUEUE.md` 四件引用，T4 改名模板但引用跟改未点名；④ V1 六条模式全为路径形态（斜杠 / 分段引号 / 裸段 / 转义 / 点号粘连 / 幂等复跑），点号粘连形还显式排除 crq- 前缀文件名形态——**落地后 V1 全绿而上述旧名引用全数残留**，恰复刻本批 riskTier 自评点名的病灶「活引用残留漏网会断治理面读写」（gov-arq 登记步骤、三条 command 收尾步骤、merge-to-main 合并步骤届时都指向不存在的脚本）。**定性**：影响目标效果（机检面可用 + 全部活引用跟改），升级 human。**正形**：T1 ③ 扩为「脚本内一切 `docs/crq` 字面量 + 一切旧脚本名自指串（含 `GENERATED_MARKER`、indexHeader 模板串、self-check 输出串）一并改指新名」；T3 改法补一句「17 件内旧脚本名与旧模板名引用（crq-render / crq-doctor / crq-ledger-shared / crq-item）随 OQ1(a) 全部跟改新名」；T4 的 ARCH-REVIEW-QUEUE 改法由「登记命令行」扩为「stub 全文两处脚本名」；§5 V1 增一条「旧名文件引用形」：`grep -rnE "crq-(render|doctor|ledger-shared|item)" <活面文件集>` → 0（实测预扫 crq-migrate.mjs 自身零命中该模式，白名单只需覆盖 retired 注——见 QUESTION-R1-2）；该新形态建议在 §3 模式完备性段补记一行（是否 append path-form-family 族谱表由主线程按其 SSOT 惯例处置）。不扩 landing-manifest（全部命中件已在 34 锚内）。
- **BLOCK-R1-2 · bare「CRQ」术语收口不完整，与 OQ1(a)「全套一次清」及 §6 留根声明直接矛盾**。T3 改法只枚举「CRQ 账本 / CRQ 条目」两短语，判官全活面 bare「CRQ」补扫（剔除 showme 豁免面与已点名两短语后）实得三处残留：`references/requirement-change.md:60`「REQ- 域 CRQ 闭环纪律」、`docs/glossary.md:37`（CDQ 词条）「契约类条目在 CRQ tech 域以 `cdq:` 指针挂接」、`docs/templates/crq-item.template.md:1`「CRQ 条目文件骨架」（T4 只注「跟改内文路径」）。其中 glossary 是术语 SSOT，其 CDQ 词条若仍留「CRQ tech 域」，与 §6 留根「『CRQ = change request』展开定义随本批消亡、未来另起新名不复用」直接矛盾——术语权威自身继续繁殖旧缩写，正是 OQ1(a) 推荐理由点名要除的债。V1 无 bare-CRQ 形态检查，残留不可机见。**定性**：影响目标效果（名即义），升级 human。**正形**：T3/T4 术语句由两短语枚举改为活面 bare「CRQ」全量清扫，判据命令 `grep -rn "CRQ" <活面文件集>` → 0 命中（白名单 = glossary「debt 账本」词条沿革注等历史记名），或显式增列上述三处；glossary ARQ 词条「已重构为 CRQ 账本」句并入沿革注一并改写。不扩 landing-manifest（三件均已在锚内）。

**QUESTION（不阻塞 GREEN 判定，FROZEN 前处置）**：

- **QUESTION-R1-1 · §3 census「49 件」口径与括注减除清单不自洽**。判官复算：49 = 全仓减 `delivery/`、减 `.codebase-memory/` 后的命中件数（含 showme 3 + 验收件 6 + ADR + CDQ）；而 §3 bullet 括注写「全仓减 `delivery/`、`docs/showme/`、验收件、`.codebase-memory/`」，按该减除清单应为 40 件。sweep 表对 49 件逐行分类无遗漏（历史豁免行覆盖 ADR / CDQ / showme / 验收件），断链风险结论不受影响；但 census 属硬项（机械枚举 + 口径留档），建议 FROZEN 前把括注修一致（改减除清单，或 49 改 40 并把豁免面单列）。
- **QUESTION-R1-2 · V1 斜杠形 × crq-migrate retired 注互锁**。crq-migrate.mjs 是 34 锚之一（活面），V1 斜杠形要求其 `docs/crq` 命中为 0；T1 ④ 同时要求「头注补 retired 注」——若 retired 注复述旧路径字面（如「原 `docs/crq`，261014 批改名」），V1 即红。正形：retired 注不复述旧路径与旧脚本名字面（写「261014 批随账本改名，路径与依赖已迁 `docs/debt` / `debt-ledger-shared.mjs`」即可），或在 V1 活面定义中显式豁免 migrate 沿革行。先写清免得落地时返工。

**注记（不阻塞，供 FROZEN 前顺手处置或 retro 回收）**：

1. census 总量 688 / 168 与判官实测 710 / 169 之差恰为本 PROPOSAL 自身（22 处 / 1 件，census 后落盘）——良性；建议 §3 留档补一句「PROPOSAL 落盘后自增 N 处」以免后批复核困惑。
2. `.claude/skills/archify/package-lock.json:59` 的 `crq` 命中为第三方 integrity 哈希内巧合子串，非引用、无动作面；判官补扫据此确认 census 无扩展名死角。
3. sweep 表 G 层标签为近似（`.gitattributes` 标 G3、route-fixtures 标「G2 邻配套件」，实测二者都不在 GATE_LAYER_RULES G1–G4 任何正则内）——与 261013 批 R1 注记 2 同型，无流程差；route-fixtures 显式归类 + §6 留根处置得当。
4. T2 触 `/agents/integrator/allow`、`/agents/adjudicator/allow` 写槽 token 平移（G3 + review-loop 邻接面）——按协议「双轨路由」节，Track B 治理批（含 review-loop def 邻接面）不强制升级，语义零变更、三键已在 human 立项 ack 的 PROPOSAL 内点名；T3 改法已声明 prompt-authoring HQ-1 边界（权限边界句只改 token 不动语义），R2 复核时一并照此核对落 diff。
5. doctor/render 自身互 import（doctor:13-14 / render:24）随 ④ 改名必改——V2 实跑（`node debt-doctor.mjs --self-check`）天然机检 import 断链，无需新增验证项，注记在此声明其覆盖关系。

**R2 入口**：仅 BLOCK-R1-1 / BLOCK-R1-2 两项（QUESTION 两项 FROZEN 前处置即可）。主线程按正形修订 §2 T1③ / T3 / T4 与 §5 V1 后本判官 R2 复核，预期 GREEN；R2 复核面 = 修订段 diff + V1 新形态两条命令（旧名文件引用形 + bare-CRQ 形）+ landing-manifest 不变确认。

### R2 · 2026-10-14 · architect

**结论：打回（1 项 BLOCK〔新〕、1 项 QUESTION〔新〕）**——R1 两 BLOCK 的 v0.2 修订经判官逐条实测复核**全部成立**，两 QUESTION 处置到位、注记 3 已顺手修正、manifest 34 行 byte 级不变（preflight 亲跑 ok=true、anchorCount=34、manifest↔anchorUnion 集合全等）、R1 记录 append-only 完好；族谱表不 append 的主线程判断**采信**（论证成立 + 判官补强理由，见下）。新 BLOCK 系 R1 BLOCK-2 正形落地后经判官 R2 干跑暴露的**验证集可达性缺陷**（bare-CRQ 判据按现活面口径恒红、无法通过）——窄修一处即 GREEN，无设计变更、不扩 manifest。

**R1 修订逐条验收（判官自证，2026-10-14 本 worktree 亲跑）**：

- **BLOCK-R1-1 修订 ✓（四腿全落）**：① T1 ③ 扩句已含 `GENERATED_MARKER`（:204）、indexHeader 模板串（:48-50）与 :227、self-check 输出串、互 import 行（doctor:13-14 / render:24）并引 R1 注记 5 覆盖关系；② T3 改法补「旧脚本名与旧模板名引用跟改」句 + 逐文件:行点名；③ T4 stub 改法扩为「指路表、登记命令行与全文两处脚本名（:17 `crq-render` 登记 / :19 `crq-doctor.mjs` 提及——判官逐行亲核）」；④ V1 第七条 + §3 模式完备性段两 token 族补记在案。**判别力干跑（判官亲跑）**：`grep -rnE "crq-(render|doctor|ledger-shared|item)"` 置于 34 锚 + AGENTS/CLAUDE/README 活面文件集现跑，命中恰为 25 件 = 四机制脚本 + repo-structure + adjudicator + 四 commands（arch-change/review-arch/review-governance/loop-retro）+ 四 references（arch-review-protocol / dev-loop-operating-system / dev-loop / requirement-change）+ gov-arq + merge-to-main + stub + 模板 + `.gitattributes` + 6 INDEX——与 T1③/T3/T4/T5 改法归属逐一对应，无第三类误报（review-moderator / glossary / 术语件 2 件零命中佐证）；大小写变体 `grep -rniE` 活面零额外命中。模式可判别性成立。
- **BLOCK-R1-2 修订 ✓**：T3 改法术语句已改为活面 bare「CRQ」全量清扫（V1 第八条、白名单为空）+ 三处存量显式点名（requirement-change.md:60 / glossary.md:37 / 模板 :1）+ glossary.md:36 ARQ 词条句并入沿革注——与 R1 正形一致；§6「CRQ 定义随本批消亡」矛盾消除。
- **QUESTION-R1-1 ✓**：§3 括注已修为「全仓减 `delivery/`、减 `.codebase-memory/`（含豁免子面）→ 49 件」并补记 710/169 自增差——与判官 R1 复算口径一致。
- **QUESTION-R1-2 ✓**：T1 ④ retired 注、T4 沿革注「不复述旧路径与旧脚本名字面」互锁条款两处落文；沿革注措辞「原名 `crq`」（小写、无路径前缀）实测不触发 V1 斜杠形 / 旧名引用形 / bare-CRQ 形三条模式，三模式对现拟措辞零误伤——互锁成立。
- **注记 3 处置 ✓**：§3 表 `.gitattributes` 行已补「实测不在 GATE_LAYER_RULES G1–G4 正则内，治理面近似归类」。
- **族谱表不 append 判断（主线程留判官复核项）：采信**。① 其自述论证成立——`crq-(render|doctor|ledger-shared|item)` 命中均为已知形态类（斜杠形 / 裸段形）对第二 token 的应用、非新形态，判官大小写变体与分段引号形补扫（均零）佐证无隐藏新类；② 判官补强一条更硬理由：path-form-family 族谱表 SSOT 实住在**已归档条目** `docs/crq/gov/_archive/2026/ARQ-260915-path-form-family-reference.md`（append-only 件、本批 136 纯 rename 裁腿面内）——本批 V3 纯 rename 断言（similarity 100%）不允许本批 append，知识保全已由 §3 模式完备性段补记 + V1 第七条单模式承担，未来批如需入表走条目追加证据注记。不要求改回 append。
- **「human 追认随合并报告」兑付形态：判官采信**（Status 行 v0.2 声明）。理由：两 BLOCK 修订均零新设计、零 manifest 变更、全部囿于 human 立项 ack 已含的 OQ1(a) 方向；Track B 前端对齐模型的 human 触点 = 立项 ack + 例外升级，本批 riskTier low-medium 不触发升级闸重算面；human 于合并报告处可见全量制品（R1/R2 记录 + landing summary + retro）且有 revert 窗口（本地 main auto-merge 可回滚）。本裁定记档供 human 合并报告时显式看到；新 BLOCK-R2-1 同属覆盖补全类、同此兑付。

**打回项**：

- **BLOCK-R2-1 · V1 第八条（bare「CRQ」→ 0）按现活面口径不可通过——tech 归档 INDEX 有 7 处「CRQ」经已结条目 `closedBy` 字段渲染带入，源在纯 rename 裁腿面内不可清**。判官对 V1 第八条做判别力干跑（现跑即应枚举全部存量）：活面 34 锚 + AGENTS/CLAUDE/README 中，除 17 件治理面件与 2 术语件、模板、glossary 的已知存量（全部有改法归属、逐件验收 ✓）外，`docs/crq/tech/_archive/2026/INDEX.md` 实测 **7 处** bare「CRQ」（:19–:26，已结薄行「依据」列）——其源 = 7 个已结 tech 条目 frontmatter `closedBy` 字段「主线程（2026-10-12 tech 域 CRQ 盘点，human 指示…）」，系 append-only 历史记名、属 136 条目纯 rename 零内容变更面，本批不可回改；INDEX 是条目 frontmatter 的纯函数（T5 重渲原样带入），故 V1 第八条对 INDEX 恒红、pre-commit 与 post-commit 两次均无法通过——冻结一个机械恒红的验证项，落地时只能靠 off-spec 变通或假绿，正是验证集要防的事。该缺口系 R1 BLOCK-2 正形（判官本人所拟）未预见 `closedBy` 渲染带入，R2 干跑实测抓出——判官与主线程同责，非起草侧退步。**正形**：§5 V1 第八条活面缩圈 = 34 锚**减 6 份生成件 INDEX** + AGENTS/CLAUDE/README，并附注「INDEX 内 bare『CRQ』= 已结条目 `closedBy` 历史记名的纯函数渲染（现值 7 处、源件 tech 域归档 7 条目），append-only + 纯 rename 纪律保留、不入清扫面」；其余各条活面维持原样（判官已逐条干跑验证可达零：斜杠形对 INDEX 仅头注 :8 模板行、重渲后归零；旧名引用形对 INDEX 经头注模板改名后归零；裸段形 / 转义形 / 点号粘连形对 `docs/crq/**` 条目正文现跑零命中——见复核实录）。机械窄修、不扩 manifest、不改设计。
- **QUESTION-R2-1 · T3 改法括注「判官逐行核得」清单与判官实测有出入（引注保真，非覆盖缺口）**。v0.2 括注列「gov-arq SKILL.md:40,42,58,71,78」+「adjudicator.md 与 loop-retro.md〔模板名〕」，但 gov-arq 的 `crq-item` 模板引用实在 **:27 与 :51**（「见 `docs/templates/crq-item.template.md` 的域表」/「复制 `docs/templates/crq-item.template.md` 为条目文件」），不在所列五行内、也不在模板名两件列内；ARCH-REVIEW-QUEUE 的模板链接虽由 T4 指路表句实质覆盖，括注未提。机械面无漏改风险——「17 件内四名引用全部跟改新名」为全量句、V1 第七条对活面机检兜底（gov-arq :27/:51 均在模式命中面内、判官干跑确认）——但「判官逐行核得」的引注应与判官实测逐字一致（memory 纪律：承重引注落笔逐词核）。FROZEN 前把括注补全（gov-arq 补「:27,:51〔模板名〕」或并入模板名列举、stub 模板链接注明归 T4），prose-only、不扩 manifest。

**注记（不阻塞，FROZEN 前顺手处置）**：

1. §2 T1 目标文件行行尾杂散反引号（「（改）`」，v0.1 即有的排版残留）——preflight 解析不受影响，FROZEN 前顺手删。
2. V1 各条对活面 / 全 `.claude`+`docs` 两种作用域混用是有意的（斜杠形 / 旧名形 / bare 形按锚集精扫、裸段 / 转义 / 点号形按目录面宽扫），判官逐条干跑确认两种口径下均可达零（除 BLOCK-R2-1 所指一处）；该分工 v0.2 行文已可辨读，无需改。

**R3 入口**：仅 BLOCK-R2-1 一项（QUESTION-R2-1 与注记 1 FROZEN 前顺手处置即可）。主线程按正形缩 V1 第八条活面并附注源件豁免理由后，本判官 R3 窄复核（复核面 = 该条修订 diff + V1 第八条对缩圈后活面干跑零异常确认），预期 GREEN。轮次状态：R1 打回 → R2 打回（1 项，系 R1 正形落地的验证集可达性收尾）——按协议「3 轮未 GREEN → human 三选一」，R3 为同判官 resume 预算末轮。

### R3 · 2026-10-14 · architect

**结论：GREEN（无打回项）**——BLOCK-R2-1 缩圈修订经窄复核成立；v0.3 全部修订（V1 第八条活面缩圈 + 豁免附注、T3 括注补正、杂散反引号删除）与 R2 正形逐字一致，无新 BLOCK。熔断未触发（R1 打回 → R2 打回 → R3 GREEN，3 轮预算内收口）。

**R3 窄复核实录（2026-10-14 本 worktree 亲跑）**：

- **BLOCK-R2-1 修复核验**：① §5 V1 bare-CRQ 条已改为 `grep -rn "CRQ" <活面文件集减 6 份生成件 INDEX>` → 0，附注源件豁免理由（closedBy 历史记名经重渲带入 `tech/_archive/2026/INDEX.md:19–:26` 共 7 处、append-only 不可回改、V3 similarity 钉纯 rename）——与 R2 正形逐要素相符；② **缩圈后活面干跑（判官亲跑）**：28 件（34 锚减 6 INDEX + AGENTS/CLAUDE/README）现跑命中 22 件、逐件计数与判官 R2 干跑**全同、无新增无回归**，且每件命中均有改法归属（脚本头注 → T1、repo-structure 行注 → T2、17 件治理面件与术语件 → T3、glossary 与模板 → T4；AGENTS / CLAUDE / README = 0）——落地后可达零，判据通过性成立；③ **INDEX 豁免数字独立复算**：六份 INDEX `grep -c "CRQ"` 仅 tech 归档年 = 7、余五份 = 0，与附注所记逐字一致。
- **QUESTION-R2-1 补正核验**：T3 括注已列 `gov-arq SKILL.md:27,51`〔模板名〕并注「:40,42,58,71,78 为脚本名引用」——与判官 R2 实测一致，引注保真恢复。
- **杂散反引号**：§2 T1 目标文件行行尾已删，token 不受影响（preflight anchorCount=34 复核佐证）。
- **GREEN 前提 · 结构字段核对**：landing-manifest 围栏块（34 行）在场、packet-binding（FROZEN 生效）行在场；`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` 亲跑 → `ok=true`、anchorCount=34、manifest ↔ anchorUnion 集合全等（python 逐行比对 setequal=True）。
- **append-only 完好性**：R1 记录（:185–:218）与 R2 记录（:220–:244 区段内）未被触碰，本次仅追加本节。

**注记（不阻塞，nit 级、不处置亦不影响）**：V1 第八条附注所引命令写作 `grep -cn`——`-c` 计数模式下 `-n` 无效果，属措辞 nit；判官已用同 grep 独立复算数字为真，落地执行时以 `-c`（或 `-rn`）为准即可。

**R3 后状态**：human 立项 ack（在案，含 OQ 推荐位裁定采信——R1 记录）+ architect GREEN（本节）→ 主线程可标 FROZEN 并走 packet-bound landing 序列（landing-manifest 绑定 + architect-landing-summary + landing-verifier 独立核验 + retro + 前置升级闸）；「human 追认随合并报告」兑付形态经 R2 采信在档。评审循环至 GREEN 收口。
