# PROPOSAL · 261019-gov-review-docs-root-ledger-rehome

> **状态：FROZEN v1.0 · 2026-10-19 · human 立项 ack（方案甲 + OQ1–OQ4 全裁 (a)，含 OQ3 五件 agent def 改动显式批准）+ architect-reviewer R2 GREEN（§7）**
> **承载 ARQ**：`ARQ-261014-closeout-obligation-write-authority-vacuum`（部分缓解、不结项——本批把 CDQ 行写点迁入 integrator/adjudicator 既有 `docs/debt/**` 写槽覆盖面，该条目其余义务面〔设计稿母版 / spine 回填 / 挂法句写点映射〕未治，落地时正文追注、status 留 deferred）。
> **立项凭据**：human 原话「docs 根目录下 ADR, ARQ, CDQ，这几个放着有点冗余」「他把这些东西放在 docs 的根目录下，搞得有点不太合适」；2026-10-19 AskUserQuestion 当面裁定 = 方案甲（docs 根瘦回四份真顶层文档 `requirements.md` / `architecture.md` / `DEV-GOVERNANCE.md` / `glossary.md` + 既有子目录；CDQ 拆条目并入 `docs/debt/` 新第四域 `contract/`；`docs/ADR.md` 迁 `docs/decisions/ADR.md`；`docs/ARCH-REVIEW-QUEUE.md` stub 指路内容并入新建 `docs/debt/README.md` 后从根删除）+ 通道 = `/gov-change` 机器面恒 Track B 主路径。
> **review 轨**：Track B 常驻轻路径（SSOT：`.claude/references/gov-change.md`；机器面〔scripts / scope-map / settings / G1〕恒 Track B——本批触 `.claude/scripts/**`、`.claude/scope-map.json`、`docs/DEV-GOVERNANCE.md`，不命中轻路径判据）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（升级闸读点换址——换址方向向严：新址读不到条目即 BLOCK、不误放行；闸自检夹具同步改写护航；其余为文本迁移与改指；触点面宽〔48 锚〕但行为面收敛于一个闸函数 + 账本域配置；全量可 git revert 回滚）。
> **review profile**：G1+G2+G3（G1 触点 = `docs/DEV-GOVERNANCE.md` 五处引用改指，零门语义变化——显式声明；G2 = rules / references / agents / skills / templates；G3 = scripts ×4 + scope-map + route-fixtures；契约面零触——实证见 §3 零接触声明；spec 面单点 = `docs/architecture.md:368` 指路改指，显式声明）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/DEV-GOVERNANCE.md
docs/architecture.md
docs/glossary.md
docs/decisions/ADR.md
docs/CONTRACT-DECISION-QUEUE.md
docs/ARCH-REVIEW-QUEUE.md
docs/debt/README.md
docs/debt/contract/CDQ-260913-spec-split-fallout.md
docs/debt/contract/CDQ-260914-kidpal-session-ctx.md
docs/debt/contract/CDQ-260921-m3-k1-schedule-grid.md
docs/debt/contract/CDQ-260924-m4-k1-storylist-semantics.md
docs/debt/contract/CDQ-260924-m4-k1-storytext-seq.md
docs/debt/contract/CDQ-260924-m4-k1-story-delete.md
docs/debt/contract/CDQ-261017-p3-story-body-sd-cache.md
docs/debt/contract/_archive/2026/CDQ-260829-storylist-total.md
docs/debt/contract/_archive/2026/CDQ-260902-shim-api-field-envelope.md
docs/debt/contract/_archive/2026/CDQ-261011-p1-home-v5-rework.md
docs/debt/contract/_archive/2026/CDQ-261010-p3-storylist-cold-start-restore.md
docs/debt/contract/_archive/2026/CDQ-261017-p3-story-body-sd-cache-devloop-accounting.md
docs/debt/contract/INDEX.md
docs/debt/contract/_archive/2026/INDEX.md
docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md
docs/debt/tech/TECH-261011-spec-v5-reconcile-note-drift.md
docs/debt/tech/TECH-261019-impl-prose-ledger-path-drift.md
docs/debt/tech/INDEX.md
docs/templates/debt-item.template.md
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/debt-ledger-shared.mjs
.claude/scripts/debt-render.mjs
.claude/scripts/debt-doctor.mjs
.claude/scope-map.json
.claude/route-fixtures/surface-alignment.json
.claude/rules/repo-structure.md
.claude/rules/contracts.md
.claude/references/arch-review-protocol.md
.claude/references/architect-operating-model.md
.claude/references/contract-change-evidence.md
.claude/references/escalation-playbook.md
.claude/references/taskpackage-authoring.md
.claude/references/change-intake.md
.claude/agents/adjudicator.md
.claude/agents/architect.md
.claude/agents/architect-reviewer.md
.claude/agents/scenario-author.md
.claude/agents/review-moderator.md
.claude/skills/kidpal-idf-env/SKILL.md
.claude/skills/kidpal-nvs-ops/SKILL.md
.claude/skills/merge-to-main/SKILL.md
```

> manifest 说明行（不进门栏）：`docs/debt/tech/TECH-261019-impl-prose-ledger-path-drift.md` 与 `docs/debt/tech/INDEX.md` 为**条件锚**——仅当 §4 OQ2 裁甲案（挂 tech 账一条）时落。**FROZEN 裁断执行记录（2026-10-19）**：OQ2 裁甲案 → manifest 保留这两行，§2 T6 锚行两条「条件锚」注记已按机械义务摘除（解析器按「条件」注记把条件锚排除在锚并集之外，`.claude/scripts/lib/governance-gate-shared.mjs:443`；摘除后锚并集 48 = manifest 48，结构门双向差闭合）。`docs/CONTRACT-DECISION-QUEUE.md` 与 `docs/ARCH-REVIEW-QUEUE.md` 为**删除锚**（纯删除，非迁移——landing-verifier 须核两件在结果树中不存在）。`docs/ADR.md` 为迁移锚，按模板规约只记迁入路径 `docs/decisions/ADR.md`。两份 `INDEX.md` 为生成件锚（`debt-render.mjs` 产物，落地时跑渲染生成、不手写）。

---

## 0. 想实现的效果与差距

**目标效果一句话**：docs 根瘦回四份真顶层文档（`requirements.md` / `architecture.md` / `DEV-GOVERNANCE.md` / `glossary.md`）+ 既有子目录，三本账各归其位——ADR 进 `docs/decisions/`、ARQ stub 撤销（指路归并 `docs/debt/README.md`）、CDQ 拆 12 条目并入 debt 账本新第四域 `contract/`（白拿状态机 / 渲染 INDEX / 自动归档 / doctor 机检），引用面同步改指，升级闸读点语义等价换址。

**现状差距**：docs 根现摊七份文件，其中三件（`ADR.md` / `ARCH-REVIEW-QUEUE.md` / `CONTRACT-DECISION-QUEUE.md`）是台账与队列，human 判「放着有点冗余」「搞得有点不太合适」。三件的来历各有裁定：`ADR.md` 是 260907 批 human 裁「放 docs 根目录、和 spec 同级」（该裁定由本批裁定 supersede）；`ARCH-REVIEW-QUEUE.md` 自 260923 批账本化后已降为手写指路 stub、不再承接登记；`CONTRACT-DECISION-QUEUE.md` 是 12 行单文件表格，状态词自成一套（DRAFT / PRECHECKED / LANDED / CLOSED·已结），与 debt 账本机制（状态机 + 生成 INDEX + 自动归档 + doctor）平行重复，且其写点无登记角色可写（ARQ-261014 实证：integrator 与 planner 代落双双被 enforce-scope 拦，裁主线程直落）。

**非目标（本批不做）**：不改任何契约 / 需求 / 架构语义（`docs/contracts/**`、`docs/requirements.md` 零触）；不改实现面（`firmware/`、`server/`、`tools/` 一字不动）；不结 ARQ-261014（仅追注）；不改任何历史制品（`delivery/**`、`docs/debt/<域>/_archive/<年>/`、`docs/archive/**` append-only）；不改 `server/deploy/openclaw-system-prompt.v3–v6`（v5/v6 受 sha256 冻结验收断言钉死、机械不可改〔`.gitattributes:32` 注记实钉 raw 集 = v6–v9〕；v3/v4 无任何 hash 验收断言、受 E2「历史版本零回改」铁律纪律冻结；四件均永久豁免不改指）；不为 CDQ 新域新立状态词（归一 debt 闭集）；不预扩 `docs/decisions/` 的承接范围（只住 ADR.md 一件，未来是否收其它决策类单文件 = 另批有意识动作）。

## 2. 问题、根因与改法（T1…T6）

**【硬项 · 起草期实证义务】**本批全部行为断言均附起草期实测（2026-10-19，工作树 = gov worktree `C:/AI/KidPal-gov`）：命令 + 关键输出逐字随各 T 证据段或 §3 census 段留痕；凡与派发简报（主线程普查）出入处，以实测为准并显式标注「普查修正」。量化声明的覆盖集合 = §3 census 表逐件枚举，判官 spot-check 即可。

### T1 · CDQ 拆账：12 行 → `docs/debt/contract/` 12 条目文件，根文件删除

**问题**：`docs/CONTRACT-DECISION-QUEUE.md` 是 docs 根第三份台账——12 行单文件表格承载契约决策状态与剩余条件，却自造一套平行机制：状态词（DRAFT / PRECHECKED / LANDED / CLOSED·已结 + 已退役 HUMAN_APPROVED）与 debt 账本闭集（blocker / deferred / accepted-risk / closed）各说各话；无生成 INDEX、无自动归档、无 doctor 机检；写点无登记角色可写（scope-map 无任何 agent allow 该文件，收口落行只能主线程直落——ARQ-261014 在册实证）。

**证据**：
- 数据行实测 12 行（**普查修正**：派发简报称「11 行表格」——`grep -c '^| CDQ-' docs/CONTRACT-DECISION-QUEUE.md` = 13 含表头，数据行 = 12，逐 ID 枚举：CDQ-260829-storylist-total / CDQ-260902-shim-api-field-envelope / CDQ-260913-spec-split-fallout / CDQ-260914-kidpal-session-ctx / CDQ-260921-m3-k1-schedule-grid / CDQ-260924-m4-k1-storylist-semantics / CDQ-260924-m4-k1-storytext-seq / CDQ-260924-m4-k1-story-delete / CDQ-261011-p1-home-v5-rework / CDQ-261010-p3-storylist-cold-start-restore / CDQ-261017-p3-story-body-sd-cache / CDQ-261017-p3-story-body-sd-cache-devloop-accounting）。
- 状态分布实测：CLOSED·已结 3 行（260829 / 260902 / 261017-devloop-accounting）；HUMAN_APPROVED 1 行（CDQ-260913，退役词、未结、带剩余条件）；LANDED 8 行（其中 2 行剩余条件栏为「无——…非契约门条件」：261011 / 261010）。
- 写权真空证据：`docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md:16` verbatim「CDQ 行无任何登记角色可写 `docs/CONTRACT-DECISION-QUEUE.md`，integrator 与 planner 代落双双被拦 → `d41f0b6e` 裁主线程直落」。
- 唯一未结行 CDQ-260913 剩余条件栏 verbatim（**本批硬要求：原文原样过账进新条目文件，不得丢**）：「① 契约 :4 改指（下次契约门顺带）；②③ phase 2 验收基线重建：统一重钉新 canary（requirements.md / architecture.md）+ 旧断言改指或退役，一个动作处置」。

**根因**：CDQ 单文件队列立于 debt 账本机制（260923 批）之前；账本化浪潮收了 ARQ（260923）却没收 CDQ——契约面有独立门与防线，队列随迁的窗口一直没开。根文件与账本并存 = 两套状态词、两个查法、两个写点纪律，docs 根因此多出一份「既不是正式顶层文档也不是子目录账本」的东西。

**改法**：
1. **拆**：12 行 → 12 个条目文件，骨架 `docs/templates/debt-item.template.md`（含 T2 扩的 contract 域字段），文件名 = `docs/debt/contract/<CDQ-id>.md`。每行映射规则：
   - frontmatter：`id` / `title`（取变更面短名）/ `domain: contract` / `opened`（= 行内日期列）/ `status`（按下表逐行钉）/ `severity`（closed 行按 schema 豁免不填；deferred 行见下表）/ `owner` / `trigger` / `paths: [docs/contracts/interface-contract.md]` + `hardExclusions: true`（契约路径命中机检硬排除，显式申报免 advisory——`debt-doctor.mjs:32-36` 规则）/ `source`（= 关联 change 的**现行仓根相对路径**，归一规则：`../changes/arch/<id>/` → `delivery/archive/changes/arch/<id>/`〔261015 批 T6 归档后真家〕，裸 changeId → `delivery/taskpackages/<id>/` 或 `delivery/review/<id>/`，逐行按下表）/ `fourClass`（= 行内四分类裸枚举值，括注进正文）/ `loopEvidence`（= 行内 loop 凭据列摘要）。
   - 正文：`**CDQ 行原文**`段 = 该行整行 verbatim（机械提取 + byte 对账，含变更面 / 四分类括注 / loop 凭据全文）；`**剩余条件**`段 = 未结行逐字过账；`**结项**`段 = closed 行一行（依据 = closedBy）。
   - **状态映射表（逐行钉；规则本身交 §4 OQ1 批核）**：

   | CDQ-id | CDQ 状态 | → status | severity | closed / closedBy | 落位 |
   |---|---|---|---|---|---|
   | CDQ-260829-storylist-total | CLOSED·已结 | closed | （豁免） | 2026-08-29 / `delivery/archive/changes/arch/260829-arch-review-kidpal-spec-stories/` | `_archive/2026/` |
   | CDQ-260902-shim-api-field-envelope | CLOSED·已结（行载 2026-09-03 原子落地） | closed | （豁免） | 2026-09-03 / `delivery/archive/changes/arch/260902-arch-review-shim-api-field-envelope/` | `_archive/2026/` |
   | CDQ-260913-spec-split-fallout | HUMAN_APPROVED（退役词，未结） | deferred | MAJOR | — | 顶层 |
   | CDQ-260914-kidpal-session-ctx | LANDED（剩余：实现归 dev-loop 后续包） | deferred | SHOULD | — | 顶层 |
   | CDQ-260921-m3-k1-schedule-grid | LANDED（剩余：A2–A4 归 K1、prompt v6 影子契约归 K3） | deferred | SHOULD | — | 顶层 |
   | CDQ-260924-m4-k1-storylist-semantics | LANDED（剩余：K1 / K2） | deferred | SHOULD | — | 顶层 |
   | CDQ-260924-m4-k1-storytext-seq | LANDED（剩余：K1 / K2） | deferred | SHOULD | — | 顶层 |
   | CDQ-260924-m4-k1-story-delete | LANDED（剩余：K1 / K2 / K3） | deferred | SHOULD | — | 顶层 |
   | CDQ-261011-p1-home-v5-rework | LANDED（剩余：无——非契约门条件） | closed | （豁免） | 2026-10-12 / `delivery/taskpackages/261011-p1-home-v5-rework/` | `_archive/2026/` |
   | CDQ-261010-p3-storylist-cold-start-restore | LANDED（剩余：无——非契约门条件） | closed | （豁免） | 2026-10-12 / `delivery/taskpackages/261010-p3-storylist-cold-start-restore/` | `_archive/2026/` |
   | CDQ-261017-p3-story-body-sd-cache | LANDED（剩余：实现面承载包） | deferred | SHOULD | — | 顶层 |
   | CDQ-261017-p3-story-body-sd-cache-devloop-accounting | CLOSED·已结 | closed | （豁免） | 2026-10-17 / `delivery/taskpackages/261016-p3-story-body-sd-cache/` | `_archive/2026/` |

   - 映射规则一句话：`CLOSED·已结 → closed`；`HUMAN_APPROVED`（未结，仅 1 行）→ deferred；`LANDED` → 剩余条件栏为「无」（含「无——…非契约门条件」形态）者 closed，否则 deferred（**LANDED ≠ debt closed**——剩余条件原样过账进 trigger 与正文）。closed 日期取行内结项日；行内无另载者取批准日（CDQ-260829 = 2026-08-29）/ LANDED 日（261011、261010 = 2026-10-12）。
   - **落位即归档**：closed 行直接落在 `docs/debt/contract/_archive/2026/`（位置 = 状态的纯函数，`expectedRel` 从出生即满足，doctor `mislocated` 红不可能触发）；deferred 行落顶层。跑 `debt-render.mjs` 后应零移动。
   - **changeId 可查性 parity（升级闸语义前提）**：每条目的 `source` 或正文必含其关联 changeId 字面（上表 closedBy/source 路径内嵌 changeId，天然满足）——闸的核销语义「changeId 子串可查」在 12 条目间逐条成立（T3 的闸改法依赖此性质）。
2. **删**：`git rm docs/CONTRACT-DECISION-QUEUE.md`。文件头（状态词 legend / 四分类权威指针）的承载去向：状态词归一 debt 闭集（旧词对照表落 `contract-change-evidence.md`，T6）；四分类权威不动（仍在 `.claude/rules/contracts.md`，T6 改其 CDQ 指路行）。
3. **副作用声明**：本批落地后，新契约变更批的 CDQ 登记动作 = 在 `docs/debt/contract/` 写条目文件 + 跑渲染——该路径在 integrator 与 adjudicator 的 `docs/debt/**` 既有写槽内，ARQ-261014 的「CDQ 行无写点」真空就此消除（该条目不结，追注见 T6）。

目标文件：`docs/debt/contract/CDQ-260913-spec-split-fallout.md`（新增）· `docs/debt/contract/CDQ-260914-kidpal-session-ctx.md`（新增）· `docs/debt/contract/CDQ-260921-m3-k1-schedule-grid.md`（新增）· `docs/debt/contract/CDQ-260924-m4-k1-storylist-semantics.md`（新增）· `docs/debt/contract/CDQ-260924-m4-k1-storytext-seq.md`（新增）· `docs/debt/contract/CDQ-260924-m4-k1-story-delete.md`（新增）· `docs/debt/contract/CDQ-261017-p3-story-body-sd-cache.md`（新增）· `docs/debt/contract/_archive/2026/CDQ-260829-storylist-total.md`（新增·直接落归档位）· `docs/debt/contract/_archive/2026/CDQ-260902-shim-api-field-envelope.md`（新增·直接落归档位）· `docs/debt/contract/_archive/2026/CDQ-261011-p1-home-v5-rework.md`（新增·直接落归档位）· `docs/debt/contract/_archive/2026/CDQ-261010-p3-storylist-cold-start-restore.md`（新增·直接落归档位）· `docs/debt/contract/_archive/2026/CDQ-261017-p3-story-body-sd-cache-devloop-accounting.md`（新增·直接落归档位）· `docs/debt/contract/INDEX.md`（新增·生成件）· `docs/debt/contract/_archive/2026/INDEX.md`（新增·生成件）· `docs/CONTRACT-DECISION-QUEUE.md`（删除）

### T2 · 账本机制扩第四域 `contract`（debt-ledger-shared / debt-render / debt-doctor / 条目模板）

**问题**：debt 账本机制（域配置 / ID 闭集 / 前缀路由 / 字段序 / 自检夹具 / 条目模板）当前只认三域——`debt-ledger-shared.mjs:13-17` 的 `DOMAINS` 只有 gov / tech / requirement，`ID_RE`（:35）只放行 `ARQ|TECH|REQ` 前缀，`PREFIX_TO_DOMAIN`（:39）无 CDQ 路由。T1 的 12 条目落进去即被 doctor 判 `bad-id` / `unknown-prefix` 红。

**证据**：
- `debt-ledger-shared.mjs:13-17` verbatim：`gov: { dir: 'gov', prefix: 'ARQ', extra: [] }` 等三行，无 contract。
- `debt-ledger-shared.mjs:35` verbatim：`export const ID_RE = /^(ARQ|TECH|REQ)-\d{6}-[a-z0-9-]+$/;`——`CDQ-260913-spec-split-fallout` 形态合 `\d{6}-slug` 段、只差前缀白名单。
- `debt-doctor.mjs` 实测基线（2026-10-19 `node .claude/scripts/debt-doctor.mjs`）：「条目 163 条：gov=113 tech=48 requirement=2 / RED 零命中 / advisory 195 条（no-paths=108 severity-off-rubric=22 legacy-prefix-off-domain=19 tech-no-handoff=33 hard-exclusion-undeclared=13）」——渲染与检查全部由 `DOMAIN_KEYS` 数据驱动，扩域 = 改配置 + 补夹具，无新控制流（起草期通读 `debt-render.mjs` / `debt-doctor.mjs` 实证：`renderIndex` / `expectedIndexes` / `readItems` / doctor 漂移检测均按 `DOMAIN_KEYS` 迭代）。
- `DOMAINS.extra` 无机械消费方（起草期实测 `grep -n "\.extra\b" .claude/scripts/*.mjs` 仅命中 doctor 自检夹具的同名局部变量，与域配置无关）——extra 是文档位；字段确定性序列化由 `FM_ORDER`（`debt-ledger-shared.mjs:115`）承载。

**根因**：域是配置不是代码（260923 批设计如此——扩域本该便宜）；CDQ 没进来只是因为从来没人开这个域。

**改法**（克制新立机检——全是配置行与夹具，无新脚本）：
1. `debt-ledger-shared.mjs`：① `DOMAINS` 增一行 `contract: { dir: 'contract', prefix: 'CDQ', extra: ['fourClass', 'loopEvidence'] }`（置 requirement 后）；② `ID_RE` 扩为 `/^(ARQ|TECH|REQ|CDQ)-\d{6}-[a-z0-9-]+$/`；③ `PREFIX_TO_DOMAIN` 增 `CDQ: 'contract'`；④ `FM_ORDER` 尾部 `landedIn` 后追加 `'fourClass', 'loopEvidence'`；⑤ `checkItems` 增一条 advisory（对偶 `tech-no-handoff` 形态）：`it.domain === 'contract' && !fm.fourClass` → `contract-no-fourclass`（advisory 不阻断）；⑥ 头注与 `PREFIX_TO_DOMAIN` 注释更新（CDQ = contract 域本域前缀，非祖辈前缀；`GRANDFATHER_PREFIX = 'ARQ'` 不动）。
2. `debt-render.mjs`：行为零改（`DOMAIN_KEYS` 驱动自动覆盖新域）；自检夹具补两例——contract 域渲染产出含 CDQ 条目（真空绿规约：夹具输入非空断言先行）、域过滤（CDQ 条目不进 gov index，对偶既有 tech-过滤夹具 `domain-filter-broken`）。
3. `debt-doctor.mjs`：行为零改；自检夹具补四例——CDQ 前缀落 contract 域不红（正控）、CDQ 前缀落 gov 域报 `prefix-domain-mismatch` 红、contract 条目缺 fourClass 报 `contract-no-fourclass` advisory、contract 条目带 fourClass 零报（正控）。
4. `docs/templates/debt-item.template.md`：① 「域怎么选」表题「三值」改「四值」并增第四行——`contract` / `contract/` / `CDQ-` /「契约面变更的决策状态与剩余条件（原 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列，261019 批并入）。登记 = integrator 收口落条目 / 主线程治理批落条目；四分类五值权威 = `.claude/rules/contracts.md`」；② tech 行描述「技术相关，**含 contract**」改为「技术相关：固件 / 工具 / 部署运维 / spec 文档（契约**决策账**已独立成 `contract` 域；tech 域契约类条目以 `cdq:` 指针挂接）」；③ tech 域 `cdq` 字段注（现文「契约的门与批准状态仍以 CDQ 为唯一权威」）改为「契约的门与批准状态以 `contract` 域条目（`docs/debt/contract/`）为唯一权威」；④ 增 contract 域字段说明段——`fourClass`（五值闭集，权威 `.claude/rules/contracts.md`；缺了 doctor 出 advisory）、`loopEvidence`（loop 凭据 = changeId + landing-verdict 结果摘要）。
5. **零配置实证声明**：新域两份 INDEX 的合并驱动已由 `.gitattributes` 既有 glob 覆盖——起草期实测 `git check-attr merge docs/debt/gov/INDEX.md` → `merge: union`，规则行 = `docs/debt/*/INDEX.md merge=union` 与 `docs/debt/*/_archive/*/INDEX.md merge=union`，`docs/debt/contract/INDEX.md` 同形命中（§5 验证集对不存在路径复跑 `git check-attr` 钉死）。

目标文件：`.claude/scripts/debt-ledger-shared.mjs`（改）· `.claude/scripts/debt-render.mjs`（改·自检夹具）· `.claude/scripts/debt-doctor.mjs`（改·自检夹具）· `docs/templates/debt-item.template.md`（改）

### T3 · 升级闸 CDQ 读点换址：单文件读 → `docs/debt/contract/` 目录扫描

**问题**：`pre-merge-escalation-gate.mjs` 的契约类 hard-violation 核销检查（T5，260908 批立）逐字读 `docs/CONTRACT-DECISION-QUEUE.md` 做 changeId 子串匹配（:198-199）；T1 删除该文件后，读点返回 null → `cdqHasRow` 恒 false → 任何在案契约类 finding 恒 BLOCK（fail-closed 方向正确但永久误拦），且 BLOCK 文案（:363）指向一个不存在的文件。闸与 CDQ 删除必须同批原子换址。

**证据**：
- `pre-merge-escalation-gate.mjs:198-199` verbatim：`const cdqText = readText(join(dir, 'docs', 'CONTRACT-DECISION-QUEUE.md'));` / `const cdqHasRow = cdqText !== null && cdqText.includes(changeId);`
- 同文件 :169 核销条款（a）verbatim：「a) docs/CONTRACT-DECISION-QUEUE.md 含本 changeId 字面（对应 CDQ 行）」。
- 同文件 :363 BLOCK 文案 verbatim（摘录）：「…但 CONTRACT-DECISION-QUEUE 无对应行且无 human defer/accept-risk 锚（260908 批 T5；字面集 260913 批扩展）」。
- 自检夹具 :600-619（T5 字面集扩展 4 例）经 `mkReadText` 按路径注入，:603 以 `p.includes('CONTRACT-DECISION-QUEUE')` 为注入针——同型耦合，须随读点改写。
- 调用形态实证（:444）：`contractFindingsAccounted(projectDir, changeId, { podsRecordPath })`——读**工作树**，故换址后核销语义 = 「合并结果工作树的 `docs/debt/contract/` 下有条目含本 changeId」；老分支合并场景 parity 成立（其 packet 指向的 changeId 已随 T1 条目化迁入新址）。
- 闸自检基线实测（2026-10-19 `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check`）：`{"selfCheck":"pre-merge-escalation-gate","pass":49,"fail":0,"total":49,"ok":true}`，exit=0。

**根因**：闸按 260908 批时的单文件现实硬编码了路径；文件迁址 = 读点必须随迁——这是 260923 批 ARQ 账本化时未波及 CDQ 的同一笔旧账的另一半。

**改法**（保持「changeId 可查」语义与 fail-closed 方向；注入点同构扩展，self-check 可测性不变）：
1. **读点换实现**（:198-199 一带）：单文件读 → 目录扫描。默认列举器 = 递归读 `join(dir, 'docs', 'debt', 'contract')`（含 `_archive/<年>/`——已结条目被渲染器归位后**仍须可查**，否则已结批的后续复核包会被误拦）下全部 `.md`，**排除 `INDEX.md`**（生成件；凭据本体 = 条目文件，INDEX 内容派生自条目、不独立作证）；目录不在场 → 返回空集（`cdqHasRow` = false → BLOCK，向严）。判定句改为「任一条目文件文本含 changeId 字面」。注入面扩展：`deps.listFiles` 可注入（默认 = 真实递归列举；`deps.readText` 注入形态不变）。
2. **注释与文案**：:163-172 函数 doc 的核销条款（a）改写为「a) `docs/debt/contract/`（含 `_archive/<年>/`）任一条目文件含本 changeId 字面（对应 CDQ 条目；261019 批起 CDQ 单文件队列并入 debt 账本 contract 域）」；:363 BLOCK 文案改写为「…但 `docs/debt/contract/` 无对应条目且无 human defer/accept-risk 锚（260908 批 T5；字面集 260913 批扩展；CDQ 家 261019 批迁 docs/debt/contract/）——核销 = 在 docs/debt/contract/ 立条目（骨架 docs/templates/debt-item.template.md）」。
3. **自检夹具改写**（真空绿规约不变：夹具非空断言先行 + 逐 case 预期）：:592-598 的 T5 三例注入的是预制 `contractFindings` 对象、不经读点——**零改**；:600-619 的 4 例（字面集扩展）从 `mkReadText` 单文件注入改写为 `listFiles + readText` 目录注入（正例 = 条目含 changeId → accounted；负例 = 条目不含 → unaccounted）；**新增两例**——⑤ changeId 只在 `_archive/2026/` 的已结条目里 → accounted（归位后可查 parity 断言）；⑥ 目录在场但无任何条目含 changeId → unaccounted（BLOCK 点名）。自检总数 49 → 51 起步，全绿为门。
4. **不动的部分**：`CONTRACT_SPEC_LITERALS`（:177）四件字面集不变（与本批无关）；`hasArchLoopContractEvidence`（契约合并凭据三件联立）不读本批任何被迁文件，零改。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

### T4 · docs 根瘦身另两件：ADR 迁址 + ARQ stub 撤销，指路归并 `docs/debt/README.md`

**问题**：docs 根另两件台账的处置——`docs/ADR.md`（11 条架构决策记录，append-only 单文件日志）按 260907 批 human 裁定住 docs 根；`docs/ARCH-REVIEW-QUEUE.md` 自 260923 批账本化后只是手写指路 stub（20 行），不再承接登记、不再生成、不再被机械消费，却占着 docs 根白名单一席与两处 per-agent 写槽。

**证据**：
- `docs/ADR.md:4` 落点凭据 verbatim：「落点凭据：human 2026-09-07 裁『放 docs 根目录、和 spec 同级』（OQ3，260907-gov-review-arq-governance-batch-2 T7）」——本批 2026-10-19 human 新裁定（方案甲）supersede 之。
- `docs/ARCH-REVIEW-QUEUE.md:3` 自述 verbatim：「它是**手写 stub、不是生成件、不再承接任何登记**」；其「去哪里找」表五行指路（gov / tech / requirement 三域 INDEX + 条目原文 + CDQ 原址）——指路内容真实有用，文件本身已无存在必要（**普查修正**：派发简报称 stub 20 行，实测 `wc -l` = 20 ✓ 无出入；其指路对象之一 `docs/CONTRACT-DECISION-QUEUE.md` 随 T1 消失，指路表必须随本批重写）。
- stub 迁移史来源行（:20）：「本批改动来源：`delivery/changes/arch/260923-gov-review-arq-ssot-restructure/`」。
- ADR.md 头部为 append-only 纪律面（:5「append-only：不覆盖、不改写既有条目」）——迁移只 git mv + 头部追一行迁址记，既有条目与落点凭据历史行一字不改。

**根因**：两件都是「单文件 markdown 注册表惯例」（260907 批原文）时代的产物；docs 根白名单当时既收正式文档也收台账。账本机制成熟 + human 对根面整洁的新裁定使旧惯例到期。

**改法**：
1. **ADR 迁址**：`git mv docs/ADR.md docs/decisions/ADR.md`（新子目录，本批唯一新住客）；文件头部落点凭据历史行**不动**，在其下追加一行迁址记：「> 迁址记：2026-10-19 human 裁 docs 根瘦身（261019-gov-review-docs-root-ledger-rehome 方案甲），本文件自 `docs/ADR.md` 迁 `docs/decisions/ADR.md`；落点凭据行（2026-09-07 裁）由本裁定 supersede；条目编号与内容零改。」integrator 落 ADR 条目的既有职责不变（写槽改指见 T5）。
2. **ARQ stub 撤销**：`git rm docs/ARCH-REVIEW-QUEUE.md`；其指路内容归并进新建的 `docs/debt/README.md`（见下），不留重定向文件（白名单收回即 enforce-scope 机械防回归，stub 本身已成多余一层）。
3. **新建 `docs/debt/README.md`**（账本门面，内容四节）：
   - **账本是什么 + 四域表**（gov / tech / requirement / contract：目录、ID 前缀、裁定线——与 T2 条目模板第四行同口径）；
   - **去哪里找 / 怎么登记 / 怎么结项**（吸收 stub 既有三节实质：三域 INDEX 指路 → 四域；登记 = 写条目文件 + 跑 `debt-render.mjs`；结项 = 原地翻 status + 重跑渲染、已结自动归位 `_archive/<结项年>/`、人不搬文件；三份→四份主 INDEX 是生成件勿手改、`debt-doctor.mjs` 报漏跑渲染）；
   - **迁址记（261019 批）**：`docs/ARCH-REVIEW-QUEUE.md`（指路 stub）已删除——条目按 ID grep 即得（`grep -rl "<ARQ-id>" docs/debt/`）；`docs/CONTRACT-DECISION-QUEUE.md` 已删除——12 行迁为 contract 域 12 条目（5 条已结在 `_archive/2026/`）；`docs/ADR.md` 已迁 `docs/decisions/ADR.md`。**历史制品（`delivery/**`、`docs/archive/**`、`docs/debt/<域>/_archive/<年>/`）与实现面散文里的旧路径引用悬空 = 已知代价（§3 豁免面），以本节为指路兜底**；
   - **生成件纪律**：INDEX 勿手改、`merge=union` + 渲染自愈（doctor `index-stale` 红 → 重跑渲染归位）。
4. **零改实证声明**：`.gitattributes` 对新域 INDEX 零加行（T2 改法 5）；`docs/decisions/` 无既有内容冲突（起草期 `ls docs/` 实证无该目录）。

目标文件：`docs/decisions/ADR.md`（迁移·迁入锚）· `docs/ARCH-REVIEW-QUEUE.md`（删除）· `docs/debt/README.md`（新增）

### T5 · scope-map 与 route-fixtures 同步：白名单三撤一增，写槽改指

**问题**：结构红线的机械面（`scope-map.json`）与跨面对齐 fixture 仍登记三件旧路径：universal 白名单放行三件将删/将迁文件、缺 `docs/decisions` 子目录；两处 per-agent 写槽指向将删文件。规则面（`repo-structure.md` 白名单）与机械面必须同批一致（该文件自述「两处必须一致——enforce-scope 钩子按 scope-map 机判」）。

**证据**（**普查修正**：派发简报称「per-agent allow 有 3 处 docs/ARCH-REVIEW-QUEUE.md（:38/:151/:246 附近）」——起草期逐行实测：per-agent 命中实为 **2 处**（integrator :150、adjudicator :245）；简报的 :38 属 universal.docsTopLevel.allowFiles 而非 per-agent，:151 实为 `docs/debt/**`、:246 实为 `docs/debt/**`。逐行留痕如下）：
- `scope-map.json:36-44` universal.docsTopLevel.allowFiles 七行含 `docs/ARCH-REVIEW-QUEUE.md`（:38）、`docs/CONTRACT-DECISION-QUEUE.md`（:39）、`docs/ADR.md`（:43）；allowDirs（:45-52）无 `docs/decisions`。
- `scope-map.json:145-155` integrator.allow 含 `docs/ARCH-REVIEW-QUEUE.md`（:150）、`docs/debt/**`（:151）、`docs/ADR.md`（:152）。
- `scope-map.json:241-247` adjudicator.allow 含 `docs/ARCH-REVIEW-QUEUE.md`（:245）、`docs/debt/**`（:246）。
- `route-fixtures/surface-alignment.json:16` adjudicator 行 verbatim（摘录）：`"allow": [..., "docs/ARCH-REVIEW-QUEUE.md", "docs/debt/**"]`，rationale 尾注「ARCH-REVIEW-QUEUE.md kept as stub path」。该 fixture 只登记 review-loop + architect（其 `_doc` 自述「只登记档1 已迁 agent」），integrator 不在册、无需同步（起草期通读全件实证：integrator 字样零命中）。
- 无其它机械面引用：`grep -rn` 对 `.claude/hooks/`、`.claude/commands/`、`verify-gates.json`、各 `settings.json` 三件旧路径零命中（2026-10-19 实测）。

**根因**：scope-map 白名单与写槽按「文件在 docs 根」登记；文件迁址/删除后登记即成悬空允许（对白名单语义无安全害处——allow 一个不存在的路径不放行任何真实写，但结构 SSOT 与机械面漂移本身就是违规种子）。

**改法**：
1. `scope-map.json` universal.docsTopLevel：allowFiles 撤 `docs/ARCH-REVIEW-QUEUE.md`、`docs/CONTRACT-DECISION-QUEUE.md`、`docs/ADR.md` 三条（余四条 = 目标布局四份真顶层文档）；allowDirs 增 `docs/decisions`（置字母序位）。
2. `scope-map.json` integrator.allow：撤 `docs/ARCH-REVIEW-QUEUE.md`；`docs/ADR.md` 改指 `docs/decisions/ADR.md`（ADR 新增条目落点职责不变——`docs/DEV-GOVERNANCE.md:240` 登记 integrator 落条目）；`docs/debt/**` 不动（已覆盖 CDQ 新域——ARQ-261014 的 CDQ 写点真空由此闭合，见 T1 改法 3）。
3. `scope-map.json` adjudicator.allow：撤 `docs/ARCH-REVIEW-QUEUE.md`（`docs/debt/**` 已覆盖）。
4. `route-fixtures/surface-alignment.json` adjudicator 行：allow 撤 `docs/ARCH-REVIEW-QUEUE.md`；rationale 尾注改写（stub 已删除，260923  stub 化 → 261019 删除；旧字面 `docs/ARCH-REVIEW-QUEUE.md` 以撤销注形态保留，§5 验证集第 7 条白名单 ⑯ 登记）；`lastReviewed` 翻为落地日。
5. **架构面零触声明**：architect 段（:161-193）allow/deny 与本批无关零改（其 allow 本就不含三件旧路径——CDQ 从未在 scope-map 授予 architect，与 ARQ-261014 实证互洽）。

目标文件：`.claude/scope-map.json`（改）· `.claude/route-fixtures/surface-alignment.json`（改）

### T6 · 引用改指全清单：规则 / 流程 / agent / skill / 顶层文档 / 活跃条目追注

**问题**：三件旧路径（及「三域」枚举）在 canonical 文面有 23 处活跃引用（豁免面之外，逐件枚举见下）。文件删除/迁址后这些引用全部悬空；其中 5 件是 review-loop / self agent 定义（阀门面），2 处是史上已半悬空指针（ARQ-077 / ARQ-260813——账本无此二条目，见证据）。

**证据**（逐件逐行，2026-10-19 grep + Read 实测；**普查修正**两处——派发简报称 references 6 件含 `arch-scenarios.md`、agents 3 件：实测 `arch-scenarios.md` 对三件旧路径零命中〔仅 :43 一处 `delivery/review/gov-review-defender-write-scope/ADR-001-...md` 编号引用，与本批无关〕，而 `taskpackage-authoring.md:4` 与 `change-intake.md:28/:30` 命中简报未列；agents 实为 5 件——简报 3 件外增 `architect-reviewer.md:28`（`docs/ADR.md` 路径形）与 `review-moderator.md:36`（三域 INDEX 枚举））：

*规则面（2 件）*：
- `.claude/rules/repo-structure.md:21` docs/debt/ 行「三域文件夹 `gov/` `tech/` `requirement/`」；:61 白名单文件行（含三件旧路径原文）；:62 子目录行（无 `decisions/`）；:70 消歧表 `docs/debt/{gov,tech,requirement}/INDEX.md` 行；:77 语义说明段「账本原本就住 `docs/`（`ARCH-REVIEW-QUEUE.md` / `CONTRACT-DECISION-QUEUE.md` 都是 docs 根文件）…原地铺开」历史叙述。
- `.claude/rules/contracts.md:26` verbatim（摘录）：「CDQ（`docs/CONTRACT-DECISION-QUEUE.md`）保留为跨时间账本，列义『loop 凭据』」。

*流程面 references（6 件改 + 1 件实证零改）*：
- `arch-review-protocol.md:130` SPAWN-NEW-CONCERN 行 `docs/debt/{gov,tech,requirement}/` 枚举；:144 verbatim（摘录）「从仓库决策日志/CONTRACT-DECISION-QUEUE/既往 grilling 提炼」。
- `architect-operating-model.md:29` verbatim：「5. `docs/CONTRACT-DECISION-QUEUE.md` and the debt ledger (`docs/debt/{gov,tech,requirement}/INDEX.md` - …)」；:40 G4 行 verbatim（摘录）：「**Evidence / queues (G4)**: `delivery/review/**`, `docs/CONTRACT-DECISION-QUEUE.md`. Registration of debt ledger entries…」。:48/:87/:166-171 为 ARQ/CDQ 概念词（无路径）——其中 :169-171「CDQ is the contract decision backlog…」段追加半句新址注，余不动。
- `contract-change-evidence.md:28` PRECHECKED 行「（CDQ 行登记）」；:32 verbatim（摘录）「CDQ 历史行保留不改」；:44 verbatim（摘录）「CDQ 登记：每契约变更批一行（四分类五值 + loop 凭据列…）」；状态词表（:23-32）整体转历史对照（261019 批起新条目用 debt 闭集）。
- `escalation-playbook.md:20` verbatim：「- 要改契约 → 进 CONTRACT-DECISION-QUEUE 异步批，切别的 worktree 不空等」。
- `taskpackage-authoring.md:4` verbatim（摘录）：「来源：ARQ-260813-taskpackage-friction-fields-template（`docs/ARCH-REVIEW-QUEUE.md`，5 包独立实证）」——**该 ID 在现行账本无条目**（`find docs/debt -name "*260813*"` 零命中，2026-10-19 实测），改指时如实注「该 ID 未迁入现行账本」。
- `change-intake.md:28`「debt 未结账三域（gov / tech / requirement）」；:30「查重范围扩 docs/debt 三域未结条目」。
- `arch-change.md` **实证零改**：:20 仅 bare「ADR.md」分工句两处（「决策本体归 ADR.md」「与既有节 / ADR.md 既有条目」）——文件名未变、分工句不钉路径，继续成立（对偶 `arch-doc-restructure` 同形态，见下）。

*agent 定义面（5 件；阀门声明见 §4 OQ3）*：
- `adjudicator.md:39` 产物行 `docs/debt/{gov,tech,requirement}/<ID>.md` 枚举；:50 verbatim（摘录）：「`docs/ARCH-REVIEW-QUEUE.md` 已降为指路 stub、不承接写入」——文件删除后此句直接失效，改写为「原单文件队列已撤销（260923 批降 stub、261019 批删除），登记 = 写 `docs/debt/<域>/<ID>.md` 条目」。:3 description 的「原 ARCH-REVIEW-QUEUE」历史注**保留不动**（准确、属 routing signal 稳定面；列入 §5 白名单）。
- `architect.md:52` G4 句——旧文把 `docs/CONTRACT-DECISION-QUEUE.md` 列入 architect G4 可写面，而 scope-map 从未授予（ARQ-261014 :16 实证 integrator/planner 代落被拦后主线程直落）——改写为：「**证据与队列（G4）**：`delivery/review/**`、`delivery/workstreams/README.md`。debt 账本条目（`docs/debt/<域>/<ID>.md`，四域含 contract〔原 `docs/CONTRACT-DECISION-QUEUE.md`，261019 批并入〕）与 ARQ 的登记动作归**主线程**（human-approved，见 OS §6「登记动作 = 主线程」+ 查重纪律）：architect 拥有 ARQ/CDQ 处理权（经面 loop `/arch-change`、`/gov-change` 分流/landing），不做登记；scope-map allow 相应不含 `docs/debt/**`。」（同句 `delivery/workstreams/README.md` 在 scope-map architect 段同样无授予——**史上既有漂移，本批不顺手治**，登记 §6 留根 6。）
- `architect-reviewer.md:28` verbatim（摘录）：「与既有节 / `docs/ADR.md` 既有条目 / 决策历史档冲突」→ `docs/decisions/ADR.md`（:27 分工句 bare「决策本体归 ADR.md」不动）。
- `scenario-author.md:24` verbatim（摘录）：「仓库决策日志 / CONTRACT-DECISION-QUEUE / 既往 grilling —— 提炼 regression-canaries 的来源」→ 契约决策账（`docs/debt/contract/`）。
- `review-moderator.md:36` verbatim（摘录）：「debt 账本现存条目（`docs/debt/{gov,tech,requirement}/INDEX.md`）」→ 四域枚举。

*skill 面（3 件改 + 1 件实证零改）*：
- `kidpal-idf-env/SKILL.md:56` verbatim（摘录）：「see `docs/ARCH-REVIEW-QUEUE.md` ARQ-260906-shell-scripts-lf-gitattributes」→ 该条目现居 `docs/debt/gov/_archive/2026/ARQ-260906-shell-scripts-lf-gitattributes.md`（已结归档位，Glob 实测）。
- `kidpal-nvs-ops/SKILL.md:30` verbatim（摘录）：「tracked in `docs/ARCH-REVIEW-QUEUE.md` ARQ-260906-protocol-version-pinned-constant」→ 该条目现居 `docs/debt/tech/ARQ-260906-protocol-version-pinned-constant.md`（活跃，Glob 实测）。
- `merge-to-main/SKILL.md:18` verbatim（摘录）：「旧热点 `docs/ARCH-REVIEW-QUEUE.md` 已降为冷 stub」→ 改写为过去时撤销注（「旧单文件热点 `docs/ARCH-REVIEW-QUEUE.md` 已撤销〔261019 批〕」），同句 debt INDEX `merge=union` + 渲染自愈实质不变。
- `arch-doc-restructure/SKILL.md:15` **实证零改**：「决策本体 → ADR.md」为 bare 分工句，文件名未变继续成立。

*顶层文档（3 件）*：
- `docs/DEV-GOVERNANCE.md`（G1 显式声明触点）五处：:90 证据行 `[CONTRACT-DECISION-QUEUE.md](CONTRACT-DECISION-QUEUE.md)（列义 = loop 凭据）` → 契约决策账 `docs/debt/contract/`（一条目一文件、列义不变）；:109 证据行 `[ARCH-REVIEW-QUEUE.md](ARCH-REVIEW-QUEUE.md)` → debt 账本 `docs/debt/`（指路 = `docs/debt/README.md`）；:206 G4 表行两旧路径 → `docs/debt/**`（四域账本）；:240 ADR 行 `docs/ADR.md` +「human 裁 docs 根平铺」注 → `docs/decisions/ADR.md` + 迁址注（2026-10-19 裁）；:252 `ARCH-REVIEW-QUEUE.md ARQ-077` → **ARQ-077 在现行账本无条目**（`find docs/debt -name "ARQ-077*"` 零命中，2026-10-19 实测）——改写为「剩余子项与消费方链原见 ARQ-077（原单文件队列 ID，未迁入现行 debt 账本，队列文件 261019 批删除——以本节记述为准）」。:30 / :223 的 ARQ/CDQ 概念词不动。
- `docs/glossary.md` 四处：:35 debt 行「三域 `gov/` `tech/` `requirement/`」→ 四域；:36 ARQ 行「`docs/ARCH-REVIEW-QUEUE.md` 存指路 stub」→「指路 stub 已于 261019 批删除」；:37 CDQ 行全句重写（家 = `docs/debt/contract/` 域、一条目一文件、ID 前缀 CDQ-、tech 域 `cdq:` 指针挂接不变、**门与批准状态以 contract 域条目为唯一权威**）；:38 ADR 行 `ADR.md` → `docs/decisions/ADR.md`。
- `docs/architecture.md:368`（spec 面单点，显式声明）verbatim：「> 治理类 ARQ 全景（~30 条）见 [ARCH-REVIEW-QUEUE.md](ARCH-REVIEW-QUEUE.md)，本章只记架构相关面。」→「> 治理类未结项全景见 [docs/debt/gov/INDEX.md](debt/gov/INDEX.md)（debt 账本 gov 域；原 ARCH-REVIEW-QUEUE.md 已撤销，261019 批），本章只记架构相关面。」（「~30 条」陈旧计数顺势去数字化——gov 域现 113 条目。）

*活跃 debt 条目追注（2 件；append-only 正文不改写、只尾部追加）*：
- `docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md`：:16 引 `docs/CONTRACT-DECISION-QUEUE.md`（全文仅此一处路径引用；:20 仅「CDQ 行」概念词）——追注一段：本批 CDQ 并入 `docs/debt/contract/`（写点在 integrator/adjudicator `docs/debt/**` 写槽内），CDQ 行写权真空一项闭合；条目不结（其余义务面未治），status 留 deferred。
- `docs/debt/tech/TECH-261011-spec-v5-reconcile-note-drift.md`：:21 引 `docs/CONTRACT-DECISION-QUEUE.md`——追注一行：gate authority 所指 CDQ-261011-p1-home-v5-rework 已迁 `docs/debt/contract/_archive/2026/`（closed）；`cdq:` 指针（ID 形）不受影响。

**根因**：引用面按文件旧家成文；本批是文件三次迁移（删两件、迁一件）的必然连带——引用改指必须与迁移同批原子落，否则 canonical 文面集体悬空。

**改法**：按上列逐件逐行改指（承重行均附 verbatim 旧文；替换后完整句以上行为准、landing 逐字核）。统一口径三条：① 「三域」枚举一律扩为四域（`gov / tech / requirement / contract`）；② CDQ 概念词保留（账还在、只是换了家与载体），路径形引用一律改指 `docs/debt/contract/`；③ bare「ADR.md」分工句（arch-change:20、arch-doc-restructure:15、architect-reviewer:27）一律不动（文件名未变），路径形 `docs/ADR.md` 一律改指 `docs/decisions/ADR.md`——**保留 bare 形态清单在此钉死，供 §5 残留扫描豁免核对**。

目标文件：`.claude/rules/repo-structure.md`（改）· `.claude/rules/contracts.md`（改）· `.claude/references/arch-review-protocol.md`（改）· `.claude/references/architect-operating-model.md`（改）· `.claude/references/contract-change-evidence.md`（改）· `.claude/references/escalation-playbook.md`（改）· `.claude/references/taskpackage-authoring.md`（改）· `.claude/references/change-intake.md`（改）· `.claude/agents/adjudicator.md`（改）· `.claude/agents/architect.md`（改·自改递归阀，OQ3）· `.claude/agents/architect-reviewer.md`（改·第五阀，OQ3）· `.claude/agents/scenario-author.md`（改·第五阀，OQ3）· `.claude/agents/review-moderator.md`（改·第五阀，OQ3）· `.claude/skills/kidpal-idf-env/SKILL.md`（改）· `.claude/skills/kidpal-nvs-ops/SKILL.md`（改）· `.claude/skills/merge-to-main/SKILL.md`（改）· `docs/DEV-GOVERNANCE.md`（改·G1 显式声明）· `docs/glossary.md`（改）· `docs/architecture.md`（改·spec 面单点显式声明）· `docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md`（追注）· `docs/debt/tech/TECH-261011-spec-v5-reconcile-note-drift.md`（追注）· `docs/debt/tech/TECH-261019-impl-prose-ledger-path-drift.md`（新增：主线程随批登记）· `docs/debt/tech/INDEX.md`（生成件：随前条目落地重渲染）

## 3. 影响面 sweep 分类（UNCLASSIFIED=0）

**census 机械枚举取证（2026-10-19，gov worktree 工作树）**：
- `grep -rl "CONTRACT-DECISION-QUEUE" .` → 75 文件；`grep -rl "ARCH-REVIEW-QUEUE" .` → 153 文件；`grep -rl "docs/ADR\.md" .` → 105 文件。三个全集的绝大多数落在豁免面（`delivery/**` 历史制品 / `docs/debt/<域>/_archive/<年>/` / `docs/archive/**` / 实现面散文）；**活跃面 = 下表逐件枚举**（豁免面之外的每一文件均已打开核对命中行形态，无截断省略——命中行均短行，唯 `repo-structure.md:77` 与 `architect-operating-model.md:40` 两处长行已定点 Read 展开取证）。
- 词形变体负实测：`grep -rl "CONTRACT_DECISION_QUEUE\|ARCH_REVIEW_QUEUE" .` → 0 命中；反斜杠路径形 `grep -rl "docs\\\\ADR\.md\|docs\\\\ARCH-REVIEW-QUEUE\|docs\\\\CONTRACT-DECISION-QUEUE" .` → 0 命中。
- `.gitattributes`：`git check-attr merge docs/CONTRACT-DECISION-QUEUE.md docs/ARCH-REVIEW-QUEUE.md docs/ADR.md` → 三件均 `unspecified`（无合并驱动连带）；`docs/debt/*/INDEX.md` 与 `docs/debt/*/_archive/*/INDEX.md` 已配 `merge=union`。

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G1 | `docs/DEV-GOVERNANCE.md` | 改（G1 显式声明） | 五处引用改指（:90/:109/:206/:240/:252），零门语义变化 |
| spec 面 | `docs/architecture.md` | 改（spec 单点显式声明） | :368 单行指路改指 + 陈旧计数去数字化 |
| spec 面 | `docs/requirements.md` | 未触 | 实证：三件旧路径零引用（grep 全集中无此文件） |
| G1 | `AGENTS.md` / `CLAUDE.md` | 未触 | 实证：零引用（`.claude` 全目录 grep 无命中；两文件全文起草期在卷核对） |
| 契约面 | `docs/contracts/**` | 未触 | 实证：`interface-contract.md` 仅 6 处 `ADR-0xx` 编号引用（`grep -c "ADR-0"` = 6），无路径引用；编号不变；本批契约零写 |
| docs 根 | `docs/ADR.md` → `docs/decisions/ADR.md` | 迁移 | git mv + 头部迁址记一行（append-only 纪律保持） |
| docs 根 | `docs/CONTRACT-DECISION-QUEUE.md` | 删除 | 内容拆 12 条目入 contract 域（T1） |
| docs 根 | `docs/ARCH-REVIEW-QUEUE.md` | 删除 | 指路内容归并 `docs/debt/README.md`（T4） |
| G4 账本 | `docs/debt/contract/**`（12 条目 + 2 生成 INDEX） | 新增 | closed 5 条直接落 `_archive/2026/`；deferred 7 条落顶层 |
| G4 账本 | `docs/debt/README.md` | 新增 | 账本门面 + 迁址记（stub 指路归并） |
| G4 账本 | `docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md` | 追注 | append-only 尾部追加；status 不动 |
| G4 账本 | `docs/debt/tech/TECH-261011-spec-v5-reconcile-note-drift.md` | 追注 | 同上 |
| G4 账本 | `docs/debt/tech/TECH-261019-impl-prose-ledger-path-drift.md` + `docs/debt/tech/INDEX.md` | 新增（条件） | 仅 OQ2 裁甲案时落（主线程登记动作） |
| G3 | `.claude/scripts/pre-merge-escalation-gate.mjs` | 改 | T3：读点换址 + 文案 + 自检夹具（49 → 51+ 全绿为门） |
| G3 | `.claude/scripts/debt-ledger-shared.mjs` / `debt-render.mjs` / `debt-doctor.mjs` | 改 | T2：域配置 + 字段序 + 自检夹具；无新控制流 |
| G3 | `.claude/scope-map.json` | 改 | T5：白名单三撤一增 + integrator/adjudicator 写槽改指 |
| G3 | `.claude/route-fixtures/surface-alignment.json` | 改 | T5：adjudicator 行同步 |
| G3 | `.claude/hooks/**` / `.claude/verify-gates.json` / 各 settings | 未触 | 实证零引用 |
| G2 rules | `.claude/rules/repo-structure.md` / `contracts.md` | 改 | T6 |
| G2 references | `arch-review-protocol` / `architect-operating-model` / `contract-change-evidence` / `escalation-playbook` / `taskpackage-authoring` / `change-intake` | 改 | T6；`arch-change.md` 与 `arch-scenarios.md` 实证零改（bare 分工句 / 零命中） |
| G2 agents | `adjudicator` / `architect` / `architect-reviewer` / `scenario-author` / `review-moderator` | 改（阀门面，OQ3） | T6；全是引用改指与枚举扩域，零职责/权限语义变化 |
| G2 skills | `kidpal-idf-env` / `kidpal-nvs-ops` / `merge-to-main` | 改 | T6；`arch-doc-restructure` 实证零改 |
| G2 templates | `docs/templates/debt-item.template.md` | 改 | T2：域表四值化 + contract 域字段 |
| G2 commands | `.claude/commands/**` | 未触 | 实证零引用（`loop-retro.md:36` 仅 ARQ/CDQ 概念词，无路径） |
| 实现面 | `server/deploy/` 6 件 7 处（prompt v3/v4/v5/v6 各 1 + `OPENCLAW_SYSTEM_PROMPT_CARRIER:50` + `README.md:51/:81`）、`server/acceptance/` 2 件、`tools/asset-pipeline/tests/test_art_assets.acceptance.md:517` | 未触 | 全是散文/docstring 提及，无机械耦合（`grep -E "existsSync\|Path(\|open(\|read_text\|readFile"` 配 ADR/QUEUE 零命中实证）；处置交 §4 OQ2。v5/v6 两件受 sha256 冻结验收断言钉死、机械不可改指（`.gitattributes:32` 注记实钉 raw 集 = v6–v9；v5 = 归一 sha256 齿、v6 = 归一+raw 齿），v3/v4 无任何 hash 验收断言、受 E2「历史版本零回改」铁律约束（纪律冻结）——四件本批均不改指，豁免与理由写进处置条目 |
| 实现面 | `firmware/**` | 未触 | 实证零命中 |
| 历史制品 | `delivery/**`、`docs/debt/<域>/_archive/<年>/`、`docs/archive/**` | 未触 | append-only 纪律；旧路径引用悬空 = 已知代价，`docs/debt/README.md` 迁址记指路 + 按 ID grep 兜底 |
| 历史快照 | `notes/showme/show-me-gov-mechanism-growth.html:191` | 未触 | 一次性视觉解释稿（时间线 chip「ARCH-REVIEW-QUEUE 入仓」= 历史事实陈述）；处置交 §4 OQ2 |
| vendor | `ESP32-S3-RLCD-4.2-Demo/**` | 未触 | 只读参考面 |

**【硬项 · sweep 透镜义务】**本批新立/改动的机械断言 pattern 两处：① 闸 `contractFindingsAccounted` 新读点（目录扫描）——landing 前目录不存在无法全仓预跑，其预演载体 = 闸自检 6 例目录注入夹具（含 `_archive` 可查 parity 例与零匹配 BLOCK 例），起草期以现行 49 例全绿为基线（实测留痕见 T3 证据）；② doctor 新 advisory `contract-no-fourclass`——以其判定 pattern 预跑 = T2 夹具四例（正控/反控成对）。census 清单全部机械枚举生成（命令与输出如上），判官 spot-check 即可。

**【硬项 · 复扫模式完备性先验】**退役语面模式集显式枚举：**A** `CONTRACT-DECISION-QUEUE`、**B** `ARCH-REVIEW-QUEUE`、**C** `docs/ADR\.md`（仅 pathed 形态）。完备性论证：三 token 均为复合大写连字符专名，无自然语言词形变化；substring 匹配天然覆盖带/不带 `docs/` 前缀、带/不带 `.md` 后缀、带/不带 backtick 与链接括号等装饰差异（装饰字符不参与匹配）；下划线变体与反斜杠路径变体已实测零命中（命令如上）；bare `ADR.md` / bare `CDQ` / bare `ARQ` 概念词**不属于**退役面（文件名未变 / 账还在 / 前缀还在），其合法留存清单在 T6 改法末尾钉死（bare 分工句三件 + 各处历史注），§5 残留扫描以「命中 ≡ 白名单」收口而非「零命中」。

## 4. human 取舍点（OQ）

**OQ1 · CDQ 状态映射规则**（12 行 → debt status 闭集；逐行结果 = §2 T1 映射表）
背景：CDQ 旧词 LANDED 与 debt `closed` 不等价——8 行 LANDED 中 6 行剩余条件栏有实质待办（全是「实现面归 dev-loop 包 X」形态），2 行为「无——…非契约门条件」。逐条核那些 dev-loop 包是否已兑现超出本批范围，且未核即结 = 替历史行宣布无事。
- (a) **按 T1 映射表**【推荐】：`CLOSED·已结 → closed`（3 行）；`HUMAN_APPROVED`（未结 1 行）→ deferred；`LANDED` 剩余条件为「无」者 → closed（2 行），否则 → deferred（6 行）。closed 日期取行内结项日 / 批准日。open 区 7 行承载真实剩余条件，已结 5 行白拿 `_archive/2026/` 自动归位。
- (b) LANDED 一律 closed（剩余条件只留正文档案）——账更干净，但 6 行实现侧待办失去状态位跟踪，与「LANDED ≠ closed」事实相悖。
- (c) LANDED 一律 deferred——连「剩余 = 无」的 2 行也挂 open，向严过度、制造噪音行。
- 推荐理由一句话：(a) 是唯一让「状态 = 事实」成立的映射，且每行的 closed/deferred 判据机械可复（剩余条件栏是否为空）。

**OQ2 · 实现面与历史快照的旧路径散文处置**（`server/deploy` 6 件 7 处 + `server/acceptance` 2 件 + `tools` 1 件 + `notes/showme` 1 件）
背景：这些文件在 gov loop 主线程写权之外（Hard Rule：实现动作走 dev-loop），且全是散文/docstring 提及、无机械耦合（实测无文件读取断言）；其中 prompt v5/v6 两件被 sha256 冻结验收断言钉死（v5 归一 sha256 齿、v6 归一+raw 齿；`.gitattributes:32` 注记实钉 raw 集 = v6–v9），改一个字节即破断言——机械上不可改指；v3/v4 无任何 hash 验收断言，受 E2「历史版本零回改」铁律约束——纪律冻结、改指违反铁律。不改的代价 = 读者按旧路径扑空，需 `docs/debt/README.md` 迁址记 + 按 ID grep 兜底。
- (a) **不动 + 挂 tech 账一条**【推荐】：主线程随批登记 `TECH-261019-impl-prose-ledger-path-drift`（trigger = 下个触该文件的 dev-loop 包顺手改指；条目内写明 v3–v6 四件永久豁免（v5/v6 机械冻结、v3/v4 E2 纪律冻结）、showme 快照豁免）；showme HTML 不动（一次性历史快照，改词 = 改写历史陈述）。
- (b) 不动 + 仅 §6 留根——省一个条目，但 9 件文件的已知漂移只活在本文里，半年后无人记得。
- (c) 立 dev-loop 包本期改指——散文改指不值一个包的开销，且 v3–v6 依然动不了，包只能做半截。
- 推荐理由一句话：(a) 用账本既有机制把「已知漂移」钉成可触发条目，成本一行登记，无 Hard Rule 越界。

**OQ3 · agent 定义五件的阀门确认**（`architect.md` 自改递归阀 + `architect-reviewer.md` / `adjudicator.md` / `scenario-author.md` / `review-moderator.md` 第五阀）
背景：五件 def 的改动全是引用改指与「三域→四域」枚举扩展，零职责/权限语义变化；但 `architect.md` 改动属自改递归（须显式 human-approved、不得自批），四件 review-loop def 属第五阀面。其中 `adjudicator.md:50` 一句在 stub 删除后直接失效（指向不存在文件的现行纪律句），拆批会让 def 与真实结构继续漂移。
- (a) **本批立项 ack 显式覆盖五件 def 改动**【推荐】——阀门要求的是「显式 human 批」，立项 ack 点名覆盖即满足；同源同批原子收口。
- (b) def 五件拆出另立专批——本批先落其余；代价是 stub 删除后 `adjudicator.md:50` 等处处于失效态直到专批落地。
- 推荐理由一句话：阀门要的是显式批准而非分批，(a) 满足阀门且消漂移窗口。

**OQ4 · contract 域两个机读字段（`fourClass` / `loopEvidence`）**
背景：CDQ 表的「四分类」「loop 凭据」两列是契约决策的语义核心；debt schema 的 `DOMAINS.extra` 机制现成（tech 域 `handoff`/`cdq` 先例），扩的是数据不是代码，doctor 只加一条 advisory。
- (a) **设立两字段**【推荐】——四分类保机读形态（后续契约面统计/机检有抓手），成本 = 配置行 + `FM_ORDER` 两键 + 一条 advisory。
- (b) 不设，全进正文——schema 零扩；代价 = 四分类退回纯散文，未来想机判时得回扫 12+ 条正文。
- 推荐理由一句话：CDQ 表的特色列值得以机读形态存活，机制现成、边际成本趋零。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 时点标注：除注明 post-merge 者外均为 **pre-commit**（landing worktree 上、commit 前实跑）。自检/重放类遵守真空绿规约。

1. **闸自检全绿**（pre-commit）：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → 输出 `"ok":true` 且 `total >= 51`、`fail:0`（T3 改写 4 例 + 新增 2 例〔`_archive` 可查 parity、零匹配 BLOCK〕在场；既有 45 例不回退）。基线留痕：起草期 49/49 ok（2026-10-19）。
2. **debt 机制自检全绿**（pre-commit）：`node .claude/scripts/debt-render.mjs --self-check` 与 `node .claude/scripts/debt-doctor.mjs --self-check` → 各输出 ok、零 fail；T2 新增夹具（contract 渲染 + 域过滤 + CDQ 前缀四例）落地后自检总数应 > 基线。入口与基线起草期亲核实测（2026-10-19）：`debt-render --self-check: ok (8/8)`、`debt-doctor --self-check: ok (13/13)`。
3. **debt-doctor 实仓零红**（pre-commit）：`node .claude/scripts/debt-doctor.mjs` → `RED 零命中`；advisory 计数 ≤ 基线 195（12 新条目 `hardExclusions: true` 已标、paths 已填，不新增 advisory；基线留痕：2026-10-19 实测 195 条分项计数）。
4. **渲染归位幂等**（pre-commit）：`node .claude/scripts/debt-render.mjs` 连跑两次 → 首跑 stdout 无「归档归位」移动行（closed 5 条出生即在 `_archive/2026/`）、次跑后 `git diff --stat` 对该目录为零（渲染确定性）。
5. **contract/INDEX.md 内容断言**（pre-commit）：`grep -n "CDQ-260913-spec-split-fallout" docs/debt/contract/INDEX.md` → 命中且位于「待触发」节（open 区 7 行：CDQ-260913 / 260914 / 260921 / 260924×3 / 261017）；`grep -c "CDQ-" docs/debt/contract/_archive/2026/INDEX.md` → ≥5（已结薄行 5 条）；主 INDEX 已结区只留年度指针行。
6. **CDQ-260913 剩余条件原文过账**（pre-commit，硬要求）：`grep -F "① 契约 :4 改指（下次契约门顺带）；②③ phase 2 验收基线重建：统一重钉新 canary（requirements.md / architecture.md）+ 旧断言改指或退役，一个动作处置" docs/debt/contract/CDQ-260913-spec-split-fallout.md` → 命中（逐字，含全角标点）。
7. **旧路径残留扫描 ≡ 白名单**（pre-commit）：`git grep -n "CONTRACT-DECISION-QUEUE\|ARCH-REVIEW-QUEUE\|docs/ADR\.md" -- ":!delivery" ":!docs/debt" ":!docs/archive" ":!server" ":!tools" ":!firmware" ":!notes/showme"` → 命中集合逐行人读核对 ≡ 下列白名单（文件 + 内容特征钉死，不钉行号）。**排除面钉文**：debt 面整体豁免（`:!docs/debt`）——真实归档位为 `docs/debt/<域>/_archive/<年>/`（根级无 `_archive`，原写法 `:!docs/debt/_archive` 不匹配任何实际路径，现状即有 6 份 append-only 归档条目会被误扫）；且活跃条目按 append-only 纪律追注不改写原文，`ARQ-261014-closeout-obligation-write-authority-vacuum.md:16` 与 `TECH-261011-spec-v5-reconcile-note-drift.md:21` 两处原文旧路径命中属追注场景合法保留、由排除面吸收；debt 面指路本体 = 白名单 ③ 迁址记（位于排除面内、不出现命中，登记备查）。白名单：① `.claude/scripts/crq-migrate.mjs` 头注四处（retained 历史件的迁移源记述）；② `.claude/rules/repo-structure.md` 语义说明段改写后的历史叙述（两队列文件曾居 docs 根）；③ `docs/debt/README.md` 迁址记节（本批新建，指路本体）；④ `docs/glossary.md` ARQ 行历史注（「原义 ARCH-REVIEW-QUEUE 单文件队列」）；⑤ `.claude/agents/adjudicator.md:3` description 历史注（「原 ARCH-REVIEW-QUEUE」）；⑥ `.claude/agents/adjudicator.md` 改写后的撤销注（「原单文件队列已撤销」）；⑦ `.claude/references/taskpackage-authoring.md` 改写后的历史注；⑧ `docs/DEV-GOVERNANCE.md:252` 改写后的 ARQ-077 历史注；⑨ `docs/decisions/ADR.md` 头部 260907 落点凭据历史行的「对偶 ARCH-REVIEW-QUEUE.md」字样（append-only 不改写）+ 迁址记新行；⑩ `.claude/references/contract-change-evidence.md` 与 `architect-operating-model.md` 改写后的迁址历史注；⑪ `.claude/rules/contracts.md` 改写后的迁址历史注；⑫ `docs/decisions/ADR.md` 条目正文历史行（ADR-001 关联行含 `docs/ARCH-REVIEW-QUEUE.md` 字面——append-only 不改写；⑨ 只覆盖头部落点凭据行与迁址记行，本条覆盖条目正文）；⑬ `.claude/agents/architect.md` 改写后新文自带的迁址注（「原 `docs/CONTRACT-DECISION-QUEUE.md`，261019 批并入」——T6 改法文本自含旧字面）；⑭ `.claude/skills/merge-to-main/SKILL.md` 改写后新文自带的撤销注（「旧单文件热点 `docs/ARCH-REVIEW-QUEUE.md` 已撤销〔261019 批〕」——T6 改法文本自含旧字面）；⑮ `docs/architecture.md` 改写后新文自带的撤销注（「原 ARCH-REVIEW-QUEUE.md 已撤销，261019 批」——T6 改法文本自含旧字面）；⑯ `.claude/route-fixtures/surface-alignment.json` adjudicator 行 rationale 尾注改写后的撤销注（旧字面 `docs/ARCH-REVIEW-QUEUE.md` 以历史注形态保留——T5 改法 4 钉文）。**白名单外任何命中 = 落地遗漏，退回补改**。
8. **bare 形态保留核对**（pre-commit）：`git grep -n "ADR\.md" -- .claude/references/arch-change.md .claude/skills/arch-doc-restructure/SKILL.md .claude/agents/architect-reviewer.md` → 仅 bare 分工句命中（`arch-change:20` 两处、`arch-doc-restructure:15` 一处、`architect-reviewer:27` 一处）；同三文件无 `docs/ADR.md` 路径形残留（`architect-reviewer:28` 已改指 `docs/decisions/ADR.md`）。
9. **合并驱动零配置实证**（pre-commit）：`git check-attr merge docs/debt/contract/INDEX.md docs/debt/contract/_archive/2026/INDEX.md` → 两件均 `merge: union`（既有 glob 覆盖，`.gitattributes` 零改成立）。
10. **docs 根终态核对**（pre-commit）：`ls docs/` → 顶层文件恰四件（`requirements.md` / `architecture.md` / `DEV-GOVERNANCE.md` / `glossary.md`）+ 子目录含 `decisions/` 与 `debt/`；`test -f docs/CONTRACT-DECISION-QUEUE.md; echo $?` → 1，`test -f docs/ARCH-REVIEW-QUEUE.md; echo $?` → 1，`test -f docs/decisions/ADR.md; echo $?` → 0。
11. **ADR 内容零改核对**（pre-commit）：`git diff --no-renames HEAD -- docs/ADR.md docs/decisions/ADR.md`（或等价 rename 检测命令）→ 除新增迁址记一行外字节一致（`git mv` + 单行 append 的形态核验）。
12. **post-merge 复核**（合并本地 main 后，主线程簿记）：main 上复跑第 1、3 条（闸自检 + doctor 零红）+ `node .claude/scripts/changes-index.mjs --upsert 261019-gov-review-docs-root-ledger-rehome`（INDEX 簿记惯例）。

> **landing-fidelity 结构门**：本 PROPOSAL 的 `--preflight` 干跑结果随 FROZEN 申请附上（起草期已跑，结果见 §7 区上方主线程注或 landing 包附件；锚并集与 manifest 一致性以门输出为准）。

## 6. 留根清单

1. **ARQ-261014 不结项**：本批只闭合其「CDQ 行写权真空」一項（新址 `docs/debt/contract/**` 落在 integrator/adjudicator 既有 `docs/debt/**` 写槽内）；设计稿母版 / spine 回填 / 挂法句写点映射未治——条目留 deferred，正文追注本批变化（T6）。
2. **实现面散文旧路径**：OQ2 裁定后按裁断执行——甲案 = 主线程随批登记 `TECH-261019-impl-prose-ledger-path-drift`（trigger = 下个触该文件的 dev-loop 包顺手改指；v3–v6 prompt 四件**永久豁免**（v5/v6 受 sha256 验收断言机械冻结、v3/v4 受 E2 铁律纪律冻结）、showme 快照豁免，写进条目正文）；乙案 = 仅以本条留根。
3. **闸新读点首次 live 验证**：下一个真契约变更批是 `contractFindingsAccounted` 目录扫描形态的首次实战（观察项：BLOCK 文案的指引是否足以让 integrator 一次立对条目）；若出现误拦/误放，转 ARQ 登记。
4. **repo-structure 复议触发额度**：该文件语义说明段的复议触发 = 「docs 子目录数再增两个，或出现第二个记录库」——本批新增 `docs/decisions/` 消费一个额度；再增一个子目录即达触发线，后续批注意。
5. **CDQ 旧状态词停用**：DRAFT / PRECHECKED / LANDED 自本批起不再产生（新条目直接用 debt 闭集）；`contract-change-evidence.md` 状态词表转历史对照（T6 承载）。既有 `cdq:` 指针（tech 域条目的 ID 形引用）零影响。
6. **观测到的既有漂移（本批不顺手治）**：`architect.md:52` G4 句的 `delivery/workstreams/README.md` 在 scope-map architect 段无授予（与 CDQ 同款 def↔scope-map 漂移，ARQ-261014 同族）——本句本批只做最小改写（撤两件旧路径引用），README 悬案登记在此、交后续 agent-def 对齐批。
7. **自律件（不进模板的环内纪律）**：CDQ 行原文与剩余条件的搬运 = 承重引文——机械提取（`grep -o` / 单行 Read 复制）+ 落笔前后 byte 比对，禁目视转录；长文落地分块追加 + 写后即机检自身产物（行数核对 + 抽读首尾 + grep 噪音特征）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-19 - architect-reviewer

**轮次**：R1 全面审。**亲核方式**：全仓 Read/Grep/Bash 实测（引述逐字、计数复跑、脚本实跑、preflight 干跑），不采信 PROPOSAL 转述；地面真值留痕随各 finding 与文末「亲核留痕」。

**四查结论**：① 意图保真——通过（方案甲四件〔CDQ→`docs/debt/contract/` 第四域 / ADR→`docs/decisions/` / ARQ stub 并入 `docs/debt/README.md` 后删除 / docs 根瘦回四份〕原样承接，非目标清单明确、无扩面）。② 纪律合规——通过（契约面零触实证：残留扫描 `docs/contracts/**` 零命中、interface-contract.md 仅 6 处 `ADR-0` 编号引用〔实测 `grep -c` = 6〕、manifest 无契约路径；闸改法 fail-closed 方向保持〔目录不在场 → 空集 → BLOCK〕；写权边界零新增授予）。③ 撞车复核——通过（`adjudicator.md:50` 失效句改写、`contract-change-evidence.md` 状态词表转历史对照、`repo-structure.md` 复议触发额度在 §6 留根 4 记账，均无冲突）。④ 牵连面完整——主体通过（census 计数 75/153/105 逐字复现〔除本批 PROPOSAL 自身〕；词形变体负实测成立——唯一命中 = 本批 PROPOSAL 自引；实现面 sweep 6+2+1+1 实测复现；hooks/commands/settings/verify-gates 零命中实测），**但 §5 验证集第 7 条白名单未闭合 → F1**。

**判档复核**：Track B 主路径成立——本批触 `.claude/scripts/**`、`.claude/scope-map.json`、`docs/DEV-GOVERNANCE.md`，gov-change.md 轻路径判据③「命中任一 → 恒 Track B」逐条命中，判据引用成立，无降档空间。

**OQ 完备性复核**：OQ1–OQ4 设置完备、两臂均可行，不替 human 裁。OQ1 判据「剩余条件栏是否为空」机械可判（12 行剩余条件栏逐行亲核：261011/261010 以「无——」起、其余 6 行 LANDED 有实质待办）；OQ3 阀门要求「显式 human 批」而非分批，(a) 臂论证成立。**OQ2 背景材料含事实错误 → F2（human 决策材料须先正再裁）**。

**结构字段在场核对**：manifest 围栏块在场（48 行目标）、packet-id 行在场；`landing-fidelity-check.mjs --preflight` 亲跑通过（`findings: []`），锚并集实测 46 = 48 − 2 条件锚，与 manifest 说明行声明一致。

---

verdict: BLOCK(F1,F2,F3,F4)

**F1 · BLOCKING · §5 验证集第 7 条按现文不可闭合（排除面 glob 错形态 + 白名单缺口），落地后必假红**
- **(1a) 排除面路径形态错**：`:!docs/debt/_archive` 不匹配实际归档位 `docs/debt/<域>/_archive/<年>/`。亲核：`ls docs/debt/` = gov / requirement / tech 三目录（无根级 `_archive`）；`find docs/debt -maxdepth 2 -name _archive` = 3 个域内归档目录。现状即有 6 份 append-only 归档条目命中扫描：`docs/debt/gov/_archive/2026/ARQ-260907-integrator-adr-canonical-location.md:16`（docs/ADR.md）、`ARQ-260915-changes-index-lane-split.md:16`、`ARQ-260923-schedule-grid-restore-gate.md:8 与 :11`、`ARQ-260929-agent-def-provenance-noise.md:23`、`docs/debt/gov/_archive/2026/INDEX.md:77`、`docs/debt/tech/_archive/2026/ARQ-261001-openclaw-model-name-overlay-drift.md:32`。第 7 条命令 landing 后必命中这 6 处 → 按其自设规则「白名单外任何命中 = 落地遗漏，退回补改」假红；而这些条目恰是本批非目标明言不改的 append-only 历史面（非目标与 §3 census 写的 `docs/debt/_archive/**` 同为错误路径形态——manifest 里 contract 域落位写法 `docs/debt/contract/_archive/2026/` 才是对的）。
- **(1b) 白名单 ①–⑪ 之外、按 T3/T5/T6 改法落地后仍会命中的行**（逐一亲核）：① `docs/debt/gov/ARQ-261014-...md:16` 与 `docs/debt/tech/TECH-261011-...md:21`——T6 明文「append-only 正文不改写、只尾部追加」，原文旧路径留在卷；② `docs/decisions/ADR.md:16`（ADR-001 关联行，append-only 不改写，含 `` `docs/ARCH-REVIEW-QUEUE.md` `` 字面）——白名单 ⑨ 只覆盖头部落点凭据行 + 迁址记行，覆盖不到条目正文；③ `.claude/agents/architect.md:52` 改写后新文自带「原 \`docs/CONTRACT-DECISION-QUEUE.md\`，261019 批并入」（T6 改法文本自含旧字面）；④ `.claude/skills/merge-to-main/SKILL.md:18` 改写后新文自带「旧单文件热点 \`docs/ARCH-REVIEW-QUEUE.md\` 已撤销〔261019 批〕」；⑤ `docs/architecture.md:368` 改写后新文自带「原 ARCH-REVIEW-QUEUE.md 已撤销，261019 批」；⑥ `.claude/route-fixtures/surface-alignment.json:16` rationale 改写文本未钉（保留旧字面则命中；该文件既不在白名单也不在排除面）。
- **建议修法方向**：第 7 条排除面改 `:!docs/debt`（debt 面整体豁免、以白名单 ③ 迁址记为 debt 面指路本体）或改 `:!docs/debt/*/_archive` 并把上列 (1b) 六行逐一补进白名单（文件 + 内容特征钉死，与既有 ①–⑪ 同形）；同步修正非目标与 §3 的 `docs/debt/_archive/**` 路径形态。

**F2 · SHOULD · 「prompt v3–v6 受 raw sha256 冻结验收断言钉死」断言与实测不符（OQ2 human 决策材料事实错）**
- 亲核：`.gitattributes:32` 注记 verbatim「prompt v6–v9 被四条 raw sha256 冻结齿（sha256(read_bytes()) 口径）钉历史版本零回改」——被钉集 = **v6–v9**，非 v3–v6。验收齿实测：v5 = 归一 sha256（`test_m3k3_voice_schedule_entry.acceptance.py:198`/`:517`）、v6 = 归一 + raw（`test_perf_k1_roundtrip_latency.acceptance.py:212` / `test_prompt_v7_round_routing.acceptance.py:182`）、v7/v8/v9 = raw（`test_m4k3_record_session_live.acceptance.py:189` / `test_prompt_v7_round_routing.acceptance.py:187`/`:563`）；**v3、v4 无任何 hash 齿**（acceptance 全目录按版本文件名 + sha256 组合检索零命中）。
- 影响：非目标、§3 census、OQ2 背景、§6 留根 2 四处的「v3–v6 四件物理上不可改指 / 改一个字节即破断言」过强——v3/v4 是纪律冻结（E2「历史版本零回改」铁律）而非机械冻结。OQ2 推荐臂 (a) 方向不变（散文改指不值包；v5/v6 仍机械冻结；v3/v4 改指违反 E2 铁律），但 human 决策材料事实错须先正。
- **建议修法方向**：四处统一改为「v5/v6 受 sha256 齿钉死（机械不可改）；v3/v4 受 E2 铁律约束（纪律冻结、无机检齿）；四件本批均不改指，豁免与理由写进 TECH-261019 条目」。不升级 human（方向不变、纯证据修正）。

**F3 · NIT · T6 证据行称 ARQ-261014「:16/:20 引 docs/CONTRACT-DECISION-QUEUE.md」——:20 无路径引用**
- 亲核：该文件全文仅 :16 一处路径引用；:20 只出现「CDQ 行」概念词。追注方案不受影响，落地以 :16 为准。

**F4 · NIT · manifest 说明行引 `governance-gate-shared.mjs:443` 未带 `lib/` 目录**
- 亲核：`.claude/scripts/` 直下无此文件，实际在 `.claude/scripts/lib/governance-gate-shared.mjs`；行号 :443 内容正确（条件锚裁剪 `/触发条件|条件/`）。按仓内引用纪律（路径必须带目录）补 `lib/` 即可。

**亲核留痕（关键项）**：闸 :198-199/:169/:363/:603/:444/:177 逐字命中，`--self-check` 49/49 ok exit 0；doctor 全量基线「条目 163 条：gov=113 tech=48 requirement=2 / advisory 195（分项 108/22/19/33/13）/ RED 零命中」逐字复现，render 8/8、doctor 13/13；T2 配置行 `debt-ledger-shared.mjs:13-17`/`:35`/`:39`/`:115`（尾 landedIn）逐字命中，`hardExclusionsOf` contracts 命中规则 :68 在场，DOMAIN_KEYS 数据驱动实证（doctor :40/:123、render :214）；closed 行 severity 豁免有 doctor 夹具正控（:90-92）与归档 closed 条目实例佐证；scope-map :36-44/:150-152/:245-246 逐字命中、architect 段 allow 无三件旧路径与 `docs/debt/**`（:161-193 亲读）；CDQ 12 数据行、状态分布 3 CLOSED / 1 HUMAN_APPROVED / 8 LANDED（2 行「无——」）、CDQ-260913 剩余条件逐字、closed 5 / deferred 7 与 manifest 与 §5.5 三方一致；`ARQ-260813`/`ARQ-077` 账本零命中；两 ARQ-260906 条目落位（gov 归档 / tech 活跃）实测；T6 引用清单逐件抽查全中（含 bare 形态三件与「普查修正」三处）；`git check-attr` 新域两 INDEX 出生即 `merge: union`、旧三件 `unspecified`；`docs/decisions/` 不存在；preflight 干跑 `findings: []`、锚并集 46。

### R2 - 2026-10-19 - architect-reviewer

**轮次**：R2 聚焦审——F1–F4 修订到位性 + 修订引入的新矛盾扫描；不做全量重核（R1 已亲核面不重复）。

**F1 修订到位（BLOCKING 已消）**：§5 第 7 条排除面改 `:!docs/debt` 并附排除面钉文（成因记述「真实归档位为 `docs/debt/<域>/_archive/<年>/`、根级无 `_archive`、原写法不匹配任何实际路径」与 R1 亲核一致）；白名单扩 ⑫（ADR 条目正文历史行）–⑯（surface-alignment rationale 撤销注），与 R1 (1b)②–⑥ 一一对应；ARQ-261014:16 / TECH-261011:21 由排除面吸收且钉文说明追注场景合法保留；③ 迁址记落排除面内「登记备查」的处置自洽。非目标 / §3 census 豁免面 / §3 历史制品行 / T4.3 迁址记四处路径形态均修正为 `docs/debt/<域>/_archive/<年>/`。**闭合性复核**：12 条新条目 verbatim 行不含三个扫描 pattern（亲核原表——CDQ-261011/261010 行内路径为 `docs/crq/tech/...` 形，不匹配扫描集），排除 debt 面不漏任何承重命中；扫描区内 landing 后预期命中集 ≡ 白名单 ①②④–⑯，第 7 条按修订文可执行、可闭合。

**F2 修订到位（SHOULD 已消）**：四处统一改正并与 R1 实测逐项一致——非目标（v5/v6 sha256 机械不可改 + `.gitattributes:32` 实钉 raw 集 = v6–v9 注记、v3/v4 无 hash 断言受 E2 铁律纪律冻结、四件永久豁免不改指）；§3 实现面行（补 v5 归一齿 / v6 归一+raw 齿的齿型区分）；OQ2 背景与甲案（「改一个字节即破断言」收窄到 v5/v6、v3/v4 改为「纪律冻结、改指违反铁律」）；留根 2（同口径）。OQ2 三臂方向与推荐理由不受影响。

**F3 修订到位（NIT 已消）**：T6 ARQ-261014 行改「:16 引（全文仅此一处路径引用；:20 仅『CDQ 行』概念词）」——与 R1 亲核一致。

**F4 修订到位（NIT 已消）**：manifest 说明行补 `.claude/scripts/lib/governance-gate-shared.mjs:443` 全路径（行号与内容 R1 已核正确）。

**修订引入的新矛盾扫描**：T5 改法 4 新增钉文与白名单 ⑯ 互指一致；第 7 条排除面钉文与 §3 census 豁免面口径一致；未发现修订引入的其它矛盾。**新发现一处 R1 未抓的小计数漂移（NIT，不阻 GREEN）**：头部 riskTier 行「触点面宽〔47 锚〕」与 preflight 实测锚并集 46（manifest 说明行同载 46 vs 48）不符，差 1、陈旧计数——纯散文 justification 位、非机器消费面，建议 FROZEN 前顺手改为 46。

**结构字段复核**：manifest 围栏块与 packet-id 行在场未动；条件锚两条与说明行裁剪义务未动。

verdict: GREEN

**R2 结论**：F1–F4 全部修订到位、无修订引入矛盾；残留一项 NIT（riskTier 行 47→46 锚计数）不阻 GREEN，随 FROZEN 裁剪顺手消化即可。
