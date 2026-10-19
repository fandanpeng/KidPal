# landing-verdict · 261018-arch-review-sd-log-persist

> verdict: GREEN

## 1. 核验范围与凭据

- **changeId**：`261018-arch-review-sd-log-persist`（跨面 spine = `261018-sd-log-persist`；本面 = 架构面）。
- **评审目录**：`delivery/review/261018-arch-review-sd-log-persist/`。
- **FROZEN PROPOSAL**：同目录 `PROPOSAL.md`，Status 行逐字「**状态：FROZEN v1.0 · 2026-10-18 · 主线程凭据自冻**」——human 立项 ack（2026-10-18，spine `delivery/intake/261018-sd-log-persist.md:55`）+ architect-reviewer R2 GREEN（判档 = 主路径；契约四分类 = `NO_CONTRACT_CHANGE`）；R1 BLOCK(F1–F3) → 修订 v0.2 → R2 靶向复核 GREEN，评审记录在 §7。
- **landing-summary**：同目录 `architect-landing-summary.md`（主线程写；packet-id = FROZEN §2 改法集 T1–T8 + §5 验证集）。
- **landing-manifest**：`docs/architecture.md` + `docs/ADR.md`（PROPOSAL 围栏块与 landing-summary 围栏块一致）。
- **核验基线**：工作树 `C:/AI/KidPal-pod-m2`，分支 `dev/pod-m2`，landing **未 commit**（「已落地」= 工作树改动）。`git status --porcelain` 显示 canonical 仅 `M docs/architecture.md`、`M docs/ADR.md`。
- **Track 判定**：Track B 常驻轻路径（无 decision-packet 制品），故本角色定义中「设计侧门」`proposal-consolidation-check.mjs` 按定义 N/A；frozen↔批定一致性由 §7 R2 GREEN + Status FROZEN 行承载。

## 2. 结构门（机械）

命令：`node .claude/scripts/landing-fidelity-check.mjs delivery/review/261018-arch-review-sd-log-persist` → exit 0。关键字段：

```json
{
  "ok": true,
  "gate": "landing-fidelity",
  "frozen": true,
  "packetBinding": true,
  "proposalTargets": ["docs/architecture.md", "docs/ADR.md"],
  "summaryTargets": ["docs/architecture.md", "docs/ADR.md"],
  "unauthorized": [],
  "claimedNotLanded": [],
  "plannedNotLanded": [],
  "gitDegraded": false
}
```

结构门 **绿**：未冻结 / 无 packet-binding / scope creep / claimed-but-not-landed / planned-but-not-landed 五项均无命中。

**两处 advisory 的独立判定（均非阻断）**：

1. `verbatimQuoteAdvisory.missing` 列出 7 条 not-found —— 属**脚本口径**，非落地问题。原因：脚本把围栏块整块（含首行【插入 · …】/【替换 · …】说明标签行）拿去目标文件找逐字命中，而标签行本就不该落地。我已用独立脚本剥掉标签行（T3 另只取「新：」之后内容）做 byte 级对账，八段全部命中（见 §3）。**判 : advisory 预期为红、实质为绿**。
2. `advisory.changedCanonicalNotInProposal` 列出 8 个 `docs/debt/**` 路径 —— 属 **baseRef 口径**，非本批工作树改动。实测 `git diff --name-only HEAD~1..HEAD` 的清单与这 8 个路径逐项一致（来源 = 上一提交 `67b33675 chore(261017-prompt-v11-schedule-recall): 收口……`），而 `git status --porcelain -- docs/debt/` 为**空**、`git diff --stat -- docs/debt/` 为**空**。按工作树状态判定：`docs/debt/**` 本批零改动。

## 3. 逐 T 逐字对账（T1–T8）

**方法**：用 Node 脚本从 `PROPOSAL.md` 提取全部 8 个 ```text 围栏块，剥掉首行说明标签（【插入 …】/【替换 …】）与首尾空行；T3 只取「新：」之后内容；再对目标文件做 `indexOf` 精确子串匹配。**换行归一化说明**：Windows autocrlf 检出使工作树两文件全程 CRLF，而 `git show HEAD:<file>` 的 blob 全程 LF（实测 ADR worktree CRLF=108/LF-only=0，HEAD CRLF=0/LF-only=100；architecture 同形）——这是检出态而非内容差异，故比对前做 CRLF→LF 归一（与提交侧归一化一致）。归一后**八段全部命中**（`blockCount: 8, allOk: true`）。

| T | 落地位置（工作树行号） | 形态 | 围栏字节 / 行数 | 逐字对账 |
|---|---|---|---|---|
| T1 | `docs/architecture.md:422`（§10 决策 24 句尾） | insert（同一列表项内句尾 append） | 1323 B / 1 行 | **吻合**（`indexOf` 命中）；原「边界不变」句完整保留（删除行是新行的子串） |
| T2 | `docs/architecture.md:101`（§2.4 原则 5 句尾） | insert | 406 B / 1 行 | **吻合**；原句完整保留（删除行是新行子串） |
| T3 | `docs/architecture.md:232`（§5.3.1 存储行内片段） | replace | 344 B / 1 行 | **吻合**；`oldRow.replace(oldFrag, newFrag) === newRow` 精确成立，旧片段在行内仅出现 1 次、替换后归零，存储行其余部分未动 |
| T4 | `docs/architecture.md:269`（story 行 268 之后、1bpp 行 270 之前） | insert | 545 B / 1 行 | **吻合**；新行 6 列（与表头六列对齐），位置正确 |
| T5 | `docs/architecture.md:319`（§6.2 节尾） | insert | 587 B / 1 行 | **吻合**；原句完整保留（删除行是新行子串） |
| T6 | `docs/architecture.md:331-335`（§6.6） | insert（新起一段） | 612 B / 1 行 | **吻合**；原「三日志源」长句被拆为「原句前半 + 新段 + 原句后半」，`add[0] + add[4] === 删除行`（257 字符全等），三日志源计数句仍在、未改成四源 |
| T7 | `docs/ADR.md:102-108`（文末 append） | insert（标题 + 五小节） | 3759 B / 7 行 | **吻合**；位于 ADR-011「关联」行之后、以一个空行分隔；ADR-011 及既有条目原文一字未动 |
| T8 | `docs/architecture.md:442`（§12 表首行） | insert | 452 B / 1 行 | **吻合**；4 列对齐，changeId = `261018-sd-log-persist`，依据列 = `delivery/review/261018-arch-review-sd-log-persist/PROPOSAL.md（ADR-012 同批落）`，**不含 FROZEN 版本 token** |

**T7 的首轮误报已排除**：首轮脚本对 ADR-012 报 not-found，逐字符定位后确认差异**全部是 `\r`**（工作树 CRLF vs 围栏 LF），且工作树 ADR 全文 108/108 行为 CRLF（非仅新块），证明是检出态而非落地偏差；归一后逐字命中。

## 4. 范围与 append-only

- `git diff --numstat -- docs/ADR.md` → `8  0  docs/ADR.md`：**删除列 = 0**，纯 append，符合 FROZEN §5 第 3 条。
- `git diff --numstat -- docs/architecture.md` → `11  5  docs/architecture.md`：5 条删除全部为 T1 / T2 / T3 / T5 / T6 的整行重写（T1/T2/T5 删除行是新行子串；T3 为片段替换且精确可逆验证通过；T6 拆段后 `add[0]+add[4]` 与被删行 257 字符全等），**无任何实质内容被删**。
- **新增行数精确对账（证明无夹带）**：architecture 11 条新增 = T1(1) + T2(1) + T3(1) + T5(1) + T6(5：前半行 / 空行 / 新段 / 空行 / 后半行) + T4(1) + T8(1) = 11；ADR 8 条新增 = 一个空行分隔 + ADR-012 围栏 7 行 = 8。**新增集合 ≡ 围栏文本，无超出 packet 的私货**。
- **文件范围**：`git status --porcelain` 全量为——`M delivery/intake/INDEX.md`、`M delivery/taskpackages/INDEX.md`（本批 spine/承载包在 INDEX 的登记行，`changes-index.mjs` 生成，非 canonical）、`M docs/ADR.md`、`M docs/architecture.md`（landing-manifest 两件）、未跟踪的 `delivery/intake/261018-sd-log-persist.md`、`delivery/review/261018-arch-review-sd-log-persist/`、`delivery/taskpackages/261018-sd-log-persist/`。**landing-manifest 之外零 canonical 落地**。
- `docs/requirements.md`、`docs/contracts/**`、`docs/debt/**`、`.claude/**` 在**工作树**零改动（`git status` / `git diff` 均为空）；契约四分类 `NO_CONTRACT_CHANGE` 与实测一致。
- **packet-binding**：landing 范围（docs 两文件、8 个 T）⊆ 被批 packet（FROZEN §2 T1–T8 + §5），结构门 `unauthorized: []` 与我的 diff 抽查同向，无写域或权限扩张。

## 5. 验证集复跑

1. `node .claude/scripts/spec-anchor-check.mjs` → `ok: true`、`errors: []`，`archSectionCount: 47`、`anchorLines: 31`、`storyCount: 13`、`designIdCount: 61`，与起草期基线同形。
2. `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261018-arch-review-sd-log-persist/PROPOSAL.md` → `ok: true`、`findings: []`、`advisories: []`、`section2BodyFound: true`、`anchorCount: 2`、`anchorUnion: ["docs/ADR.md","docs/architecture.md"]` ≡ landing-manifest。
3. 隐私注记计数（实跑）：
   - `grep -c "对话原文"` → `docs/architecture.md:3`（T1 `:422` / T4 `:269` / T6 `:333`）、`docs/ADR.md:2`（ADR-012 标题 `:102` + 决策④ `:105`）。
   - `grep -c "TECH-261016-sd-story-confidentiality"` → `docs/architecture.md:1`（T1 `:422`）、`docs/ADR.md:3`（ADR-011 既有 `:97` + ADR-012 决策④ `:105` + 关联 `:108`）。

**§5 第 6 条 ADR 预期计数差异的独立判定**：FROZEN 写 ADR 预期「对话原文 1 / TECH-261016 2」，实测「2 / 3」。我判为**验证集预期公式失准（D-finding，非阻断）**，理由是三条实证：① 落地 ADR-012 文本与 §6 附件围栏逐字一致（3759 B / 7 行全部命中），未多一字未少一字；② 多出的两次命中**就在被批围栏文本之内**——标题行「…可删除、含对话原文）」含「对话原文」，关联小节「隐私记账 = docs/debt/tech/TECH-261016-sd-story-confidentiality.md」含 TECH id，故实测计数是被冻结文本的**必然结果**，与落地者动作无关；③ architecture 侧 3 / 1 与预期完全相符，说明偏差只出现在 ADR 的「每条目估算 1 次」公式上。故**不构成 landing 偏差**，落地保真结论不受影响；建议作为 D-finding 留给 retro / 下一批校准计数公式（PROPOSAL 已 FROZEN，本角色不改写它）。

## 6. 边界守持核（人读）

| 守持项 | 实测 | 结论 |
|---|---|---|
| carve-out 只给「设备诊断日志」 | T1/T4/T5/T6/ADR-012 的增列对象均为「设备诊断日志（固件 `esp_log` 输出流）」，形态固定为「常开 + 有界轮转、目录总量 16MB 封顶（512KB × 32）、可删除」；白名单未增列任何业务 / 身份 / 事项数据 | **守持** |
| 业务日志语义不变 | 「业务日志 = 家长知情权 = OpenClaw 侧完整日志」在 T1 / T2 / T3 / T5 / T6 / ADR-012 决策②中一致复述，且均明写「业务日志仍不落板、语义不变」 | **守持** |
| 提醒 / 课表仍不落板 | T1「提醒 / 课表等其余业务 / 身份 / 事项状态仍只驻 RAM」、T3「提醒 / 课表 / 业务日志等其余业务数据仍不落板」、T5「提醒 / 课表仍不落板」 | **守持** |
| 断网无提醒不变 | `断网无提醒` 在 architecture 命中 2 处（`:101` 原句 + T1）、ADR 2 处（ADR-011 原句 + ADR-012 决策③），语义均为「不变」 | **守持** |
| 诊断日志不提供离线功能 | T1「诊断日志不提供任何离线功能」、T2「不提供离线功能」、T5「不承载任何离线功能」、ADR-012 决策①「不提供任何离线功能」 | **守持** |
| 「三日志源」不新增第四源 | §5.5.6 `:307` 与 §6.6 `:331` 两处「三日志源」计数句仍在、未被改成四源；T6 明写「这是设备串口同一 `esp_log` 流的留存副本，**不新增第四个源**」；全文无「四日志源」表述 | **守持** |
| requirements / contracts 零改动 | 工作树 `git status` / `git diff` 对两处均为空 | **守持** |

## 7. 破坏性自检（临时副本，已丢弃）

在 `C:/tmp/lv-261018-sd-log-persist/` 生成 canonical 的临时副本并注入单字符偏差，用与 §3 完全相同的匹配器复跑：

- **探针 1**：`architecture.mutated.md`——把 T6 段内 `logs/KPLOGnnn.TXT` 删一个字符为 `logs/KPLOGnn.TXT` → `mutatedMatches: false`（干净副本为 true）。**匹配器报红，符合预期**。
- **探针 2**：`ADR.mutated.md`——把 ADR-012 决策④ 的「有意识接受」改一字为「有意识接授」→ `mutatedMatches: false`（干净副本为 true）。**匹配器报红，符合预期**。
- 探针副本运行后即删除（`temp copies removed: true`），**工作树两个 canonical 文件全程未被写入**（探针所有变更都在临时副本 / 内存中）。

结论：本 verdict 的比对方法对单字符级偏差敏感，§3 的「全绿」不是匹配器失效导致的假绿。

## 8. Findings 分级

**B（阻断）：无阻断项。**

**D（非阻断观察）**

- **D-1 · FROZEN §5 第 6 条 ADR 侧隐私计数公式失准**：预期 `对话原文:1 / TECH-261016:2`，实测 `2 / 3`。判定依据见 §5——落地文本与围栏逐字一致，多出的命中就在被批围栏文本内（ADR-012 标题含「含对话原文」、关联小节含 TECH id）。建议下一批或 retro 校准公式为「标题 + 决策④ = 2」「ADR-011 + 决策④ + 关联 = 3」，本批不阻断、不改已完成 landing。

**NIT（技艺性观察，不影响放行）**

- **NIT-1 · 行内衔接符**：T1 / T2 / T5 围栏内以空行分隔的段落，落地时以单个空格衔接在原句之后（`。 **白名单增列…`），以保持列表项 / 段落不被空行截断；**段落文本本身 byte 级一致**，且与 2026-10-17 story 注记的「句尾补注在同一列表项内」先例同型。判为形制而非偏差。
- **NIT-2 · 结构门围栏比对口径**：`verbatimQuoteAdvisory` 恒红（标签行不可落地），建议脚本后续识别【插入 / 替换】标签行与「原：/新：」形态，使该项对合规 landing 也能报绿，减少每批评审的解释成本。
- **NIT-3 · 结构门 baseRef 口径**：`advisory.changedCanonicalNotInProposal` 把 `HEAD~1` 带入的上一条提交（261017 收口，含 `docs/debt/**`）算作本批改动；建议按纯工作树口径或显式排除非本批提交，避免每批都要人工排除假阳性。

## 9. 最终结论

- **结构门**：绿（ok / frozen / packetBinding 全 true，unauthorized / claimedNotLanded / plannedNotLanded 全空）。
- **语义保真**：T1–T8 八段围栏**逐字落地、位置正确、原文零删改、无夹带**，新增行集合 ≡ 围栏文本。
- **范围**：landing ⊆ 被批 packet，manifest 之外零 canonical 落地；ADR append-only（8/0）成立。
- **验证集**：锚完整性绿、preflight 绿；隐私计数唯一差异已判定为验证集预期公式失准（D-1），非落地偏差。
- **边界守持**：七项全部守持。

**结论：GREEN。放行主线程 commit（分支 `dev/pod-m2`）。** 后续 merge 仍归主线程升级闸 / human manual-merge 门，本 verdict 不替代 merge 判定。

---

> 核验者：landing-verifier（独立核验，只读 codebase）· 2026-10-18 · 唯一写面 = 本文件。
> 凭据来源 = 脚本实跑输出 + `git diff` 逐 hunk 人读 + 围栏 byte 级对账，未采用 landing-summary 自述作为通过依据。
