# architect-landing-summary · 261019-gov-review-docs-root-ledger-rehome

packet-id：FROZEN PROPOSAL §2 改法集（T1–T6）+ §5 验证集 · 评审轮次 R1 BLOCK→R2 GREEN · change-id 261019-gov-review-docs-root-ledger-rehome

> 落地 = 主线程 packet-bound（FROZEN v1.0 PROPOSAL §2 T1–T6 + §5 验证集）。落地日 2026-10-19，gov worktree `C:/AI/KidPal-gov`。

## 1. 落地点 ↔ 改法对应（按 manifest 48 锚逐件）

| 锚 | 处置 | 状态 |
|---|---|---|
| `docs/CONTRACT-DECISION-QUEUE.md` | **删除锚**：`git rm`（12 行已迁 contract 域条目） | ✅ |
| `docs/ARCH-REVIEW-QUEUE.md` | **删除锚**：`git rm`（指路内容并入 `docs/debt/README.md`） | ✅ |
| `docs/decisions/ADR.md` | **迁移锚**：`git mv` + 头部追加迁址记一行（落点凭据历史行与 11 条目零改——`git diff --cached` 纯 rename，工作区 diff = +1 行迁址记） | ✅ |
| `docs/debt/README.md` | 新增：四节（四域表 / 找·登记·结项 / 迁址记 / 生成件纪律） | ✅ |
| `docs/debt/contract/` 12 条目 | 新增：一次性迁移脚本机械提取（`c:\tmp\gen-cdq-entries.mjs`，落盘前 byte 对账——原行与未结行剩余条件逐字包含），7 deferred 落顶层、5 closed 直接落 `_archive/2026/`；状态映射 = OQ1 (a) 臂逐行钉 | ✅ |
| `docs/debt/contract/INDEX.md` + `_archive/2026/INDEX.md` | **生成件锚**：`debt-render.mjs` 生成（首跑渲染、次跑零 diff 幂等） | ✅ |
| `.claude/scripts/debt-ledger-shared.mjs` | T2：DOMAINS +contract 行、ID_RE 扩 CDQ、PREFIX_TO_DOMAIN +CDQ、FM_ORDER +两键、checkItems +contract-no-fourclass advisory、头注更新 | ✅ |
| `.claude/scripts/debt-render.mjs` | T2：行为零改；自检夹具 +2（contract 渲染 / CDQ 域过滤），8/8→10/10 | ✅ |
| `.claude/scripts/debt-doctor.mjs` | T2：行为零改；自检夹具 +4（CDQ 前缀四例），13/13→17/17 | ✅ |
| `.claude/scripts/pre-merge-escalation-gate.mjs` | T3：CDQ 读点单文件读 → `docs/debt/contract/` 递归目录扫描（排除 INDEX.md；目录不在场 → 空集向严）；`deps.listFiles` 注入面扩展；doc 条款 (a) 与 BLOCK 文案改指；自检 4 例改写为目录注入 + 新增 2 例（`_archive` 可查 parity / 零匹配 BLOCK），49→51 | ✅ |
| `docs/templates/debt-item.template.md` | T2 改法 4 四件 + 头注「三份 INDEX」→「各域 INDEX」（同文件计数漂移顺手修，防止模板自相矛盾） | ✅ |
| `.claude/scope-map.json` | T5：allowFiles 三撤（余四份真顶层文档）；allowDirs +`docs/decisions`；integrator.allow 撤 stub + `docs/ADR.md`→`docs/decisions/ADR.md`；adjudicator.allow 撤 stub | ✅ |
| `.claude/route-fixtures/surface-alignment.json` | T5：adjudicator 行 allow 撤 stub、rationale 尾注改写（旧字面以撤销注形态保留）、lastReviewed→2026-10-19 | ✅ |
| `.claude/rules/repo-structure.md` | T6：:21 四域、:61 白名单三撤「仅此四份」、:62 子目录 +`decisions/`、:70 消歧表四域、:77 语义段历史叙述改写 | ✅ |
| `.claude/rules/contracts.md` | T6：CDQ 指路行改指 `docs/debt/contract/`（列义不变 + 机读字段注） | ✅ |
| references ×6 | arch-review-protocol / architect-operating-model / contract-change-evidence（状态词表转历史对照）/ escalation-playbook / taskpackage-authoring（ARQ-260813 未迁入注）/ change-intake | ✅ |
| agents ×5（OQ3 立项 ack 显式覆盖） | adjudicator（:39 四域枚举、:50 撤销句改写、:3 description 历史注保留）/ architect（:52 G4 句按处方重写）/ architect-reviewer（:28 改指）/ scenario-author（:24 改指）/ review-moderator（:36 四域枚举） | ✅ |
| skills ×3 | kidpal-idf-env:56 / kidpal-nvs-ops:30（各指条目现居路径 + 撤销注）/ merge-to-main:18（撤销注） | ✅ |
| `docs/DEV-GOVERNANCE.md` | G1 显式声明触点五处（:90/:109/:206/:240/:252，ARQ-077 未迁入注） | ✅ |
| `docs/glossary.md` | 四处（debt 行四域、ARQ 行 stub 删除注、CDQ 行全句重写、ADR 行改指） | ✅ |
| `docs/architecture.md:368` | spec 面单点显式声明：指路改指 gov 域 INDEX + 去陈旧计数 | ✅ |
| `docs/debt/gov/ARQ-261014-*.md` | 追注一段（CDQ 写权真空一项闭合；条目不结，status 留 deferred） | ✅ |
| `docs/debt/tech/TECH-261011-*.md` | 追注一段（gate authority 所指条目已迁 `_archive/2026/`；`cdq:` 指针不受影响） | ✅ |
| `docs/debt/tech/TECH-261019-impl-prose-ledger-path-drift.md` | **OQ2 甲案新增**：实现面/快照 10 处散文漂移登记（trigger = 下个触该文件的 dev-loop 包顺手改指；v5/v6 sha256 机械冻结 + v3/v4 E2 纪律冻结四件与 showme 快照永久豁免写入正文） | ✅ |
| `docs/debt/tech/INDEX.md` | 生成件：随 TECH-261019 落条目重渲染 | ✅ |

未触面（按 PROPOSAL 声明）：契约面 `docs/contracts/**` 零写；实现面 `server/` `tools/` `firmware/` 散文按 OQ2 甲案不动（已挂 TECH-261019）；历史制品 `delivery/**`、`docs/archive/**`、`docs/debt/<域>/_archive/` 既有件零改（append-only）。

## 2. 验证证据（§5 pre-commit 1–11 执行记录）

| 项 | 结果 |
|---|---|
| 1 闸自检 | ✅ `{"pass":51,"fail":0,"total":51,"ok":true}`（T3 改写 4 例 + 新增 2 例在场；基线 49 无回退） |
| 2 debt 机制自检 | ✅ render 10/10、doctor 17/17（基线 8/8、13/13，新增夹具全在场） |
| 3 doctor 实仓 | ✅ RED 零命中；**advisory 196（基线 195，+1）**——如实说明：+1 = `severity-off-rubric` 22→23，来源 = CDQ-260913 沿用原队列 `MAJOR` 档（MAJOR 在并集闭集内、非 rubric 五档，advisory 级不阻断；迁移不造数据、不静默改档，与账本既有「实况并集」纪律一致） |
| 4 渲染幂等 | ✅ 首跑渲染 contract 两件 + tech/INDEX，次跑全「未变」零移动（closed 5 条出生即在 `_archive/2026/`，无归位移动行） |
| 5 INDEX 内容 | ✅ `CDQ-260913` 命中于「待触发（7）」节（open 7 行 = 260913/260914/260921/260924×3/261017）；`_archive/2026/INDEX.md` CDQ 行 = 5；主 INDEX 已结区仅年度指针行 |
| 6 剩余条件逐字 | ✅ `grep -F` 全角原文命中（trigger 字段与正文 CDQ 行原文两处） |
| 7 残留扫描 | ⚠️ **见下方「验证集偏差登记」**——命中 27 行：18 行 ≡ 白名单（①–⑯ 覆盖），9 行为白名单欠列的同类迁址注 |
| 8 bare 形态 | ✅ `arch-change:20`（两处）、`arch-doc-restructure:15`、`architect-reviewer:27` 仅 bare 分工句；`architect-reviewer:28` 已改指 `docs/decisions/ADR.md`；三文件无 `docs/ADR.md` 路径形残留 |
| 9 合并驱动 | ✅ 两件新 INDEX `merge: union`（既有 glob 覆盖，`.gitattributes` 零改） |
| 10 docs 根终态 | ✅ 顶层文件恰四件 + 子目录含 `decisions/` `debt/`；两队列文件不存在、`docs/decisions/ADR.md` 在场 |
| 11 ADR 零改 | ✅ staged diff = 纯 rename（0 增 0 删）；工作区 diff = +1 行迁址记 |
| 12 post-merge | 待合并后执行（闸自检 + doctor 零红复跑 + `--upsert` 簿记） |

## 验证集偏差登记（项 7，交 landing-verifier 独立裁定）

§5 第 7 条白名单 ①–⑯ 按「文件 + 内容特征」钉死。落地实测命中 27 行中 9 行在白名单之外，**全部与 ⑬–⑯ 同类 = PROPOSAL 处方新文自带的迁址注**（改写后的新文必须点名旧文件才能说清它去了哪；处方文本自身即含旧路径字面，⑬ architect.md 同型先例）：

| # | 命中 | 内容特征 |
|---|---|---|
| ⑰ | `repo-structure.md:21` | 四域行 contract 域来源注（「261019 批自 `docs/CONTRACT-DECISION-QUEUE.md` 并入」） |
| ⑱ | `repo-structure.md:61` | 白名单文件行瘦身注（「仅此四份……已删除」） |
| ⑲ | `debt-ledger-shared.mjs:40` | PREFIX_TO_DOMAIN 注释（T2 改法 1⑥「注释更新」的落地形态） |
| ⑳ | `kidpal-idf-env/SKILL.md:56` | 改指后新文自带撤销注 |
| ㉑ | `kidpal-nvs-ops/SKILL.md:30` | 同上 |
| ㉒ | `DEV-GOVERNANCE.md:90` | 证据行改指注（⑧ 只覆盖 :252，本行同型） |
| ㉓ | `DEV-GOVERNANCE.md:109` | 同上 |
| ㉔ | `glossary.md:37` | CDQ 行全句重写自带并入注（④ 只覆盖 :36 ARQ 行，本行同型） |
| ㉕ | `templates/debt-item.template.md:15` | T2 改法 4① 处方文本自身即含旧路径（「原 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列，261019 批并入」——处方 verbatim） |

主线程判定依据：9 行无一为「漏改的活引用」——每行语义都是「旧文件已删/已迁 + 新址在哪」，删除旧字面即破坏注记本身；且模板行 ㉕ 的处方文本（PROPOSAL §2 T2 改法 4①）逐字含旧路径，落地与处方一致即必然命中。**结论候选 = 验证集白名单欠列（PROPOSAL 侧瑕疵），非落地遗漏**；是否接受此裁定（或以 R3 增补白名单）归 landing-verifier。

## 3. 诚实注记

1. 落地过程一次自伤已修：TECH-261011 追注首次写入时脚本中断留下 `**追注**` 残片，当即按 append-only 原则修齐全段（残片未进入任何 commit）。
2. 迁移用一次性脚本 `c:\tmp\gen-cdq-entries.mjs` 在仓外（附加工作目录），不入仓、不进 manifest；12 条目内容 = 机械提取 + byte 对账产物。
3. `docs/debt/README.md` 四域表与「迁址记」节按 T4 改法 3 处方撰写；其中四域表第三列「什么时候选它」与模板第四行同口径。

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
