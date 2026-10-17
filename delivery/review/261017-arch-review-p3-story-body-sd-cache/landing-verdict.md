# landing-verdict · 261017-arch-review-p3-story-body-sd-cache

> 判官 = landing-verifier（只读 codebase；唯一写面 = 本文件）· 2026-10-17
> 核验对象 = landing commit **b3f50ebd**（base = 父提交 **472068ea**，当前 HEAD = b3f50ebd，工作树洁净）
> 依据 = FROZEN PROPOSAL `delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md`（状态行 `FROZEN v1.0 · 2026-10-17 · 主线程凭据自冻`；§7 = R1 BLOCK(F1,F2) → R2 GREEN）。landing-summary 只作对照、不作依据。

## 判定

verdict: PASS + 6 ADVISORY

**ok: true — PASS（放行建议；merge 仍归 human / 合并机械闸）**

```json
{
  "ok": true,
  "reviewDir": "delivery/review/261017-arch-review-p3-story-body-sd-cache",
  "landingCommit": "b3f50ebd",
  "baseCommit": "472068ea",
  "track": "B",
  "packetBinding": "ok",
  "structuralGate": "ok",
  "semanticDiffMatch": "ok",
  "outOfScopeCanonicalChanges": 0,
  "gitDegraded": false
}
```

## 1 · 结构门（机器，完整模式）

命令：`node .claude/scripts/landing-fidelity-check.mjs delivery/review/261017-arch-review-p3-story-body-sd-cache`（非 `--preflight`），exit 0。

```json
{
  "ok": true, "gate": "landing-fidelity",
  "frozen": true, "packetBinding": true, "gitDegraded": false,
  "proposalTargets": ["docs/architecture.md", "docs/ADR.md", "docs/contracts/interface-contract.md"],
  "summaryTargets":  ["docs/architecture.md", "docs/ADR.md", "docs/contracts/interface-contract.md"],
  "unauthorized": [], "claimedNotLanded": [], "plannedNotLanded": [],
  "advisory": { "changedCanonicalNotInProposal": ["docs/CONTRACT-DECISION-QUEUE.md"] }
}
```

三集合核对（手工复核在同一 commit 上）：

- FROZEN §2 各 T「目标文件」并集 = {`docs/architecture.md`（T1/T2/T6）, `docs/ADR.md`（T3）, `docs/contracts/interface-contract.md`（T4/T5）}
- `landing-manifest` 围栏块（PROPOSAL :12-16）= 同三文件
- 实际 diff 涉及文件（`git show --numstat b3f50ebd`）= 上述三文件 + 三件非 canonical：`docs/CONTRACT-DECISION-QUEUE.md`（CDQ 行，R2 NIT-3 明确认可为 landing 期账本动作、不入 manifest）、`delivery/review/…/PROPOSAL.md`（标 FROZEN + §7 R1/R2 记录）、`delivery/review/…/architect-landing-summary.md`（loop 必需品，AGENTS.md 制品 4 件之一）。**无第四只 canonical 文件被改。**

numstat（逐文件精确行数，用于反推「无夹带改动」）：

```
364  0  delivery/review/…/PROPOSAL.md
 39  0  delivery/review/…/architect-landing-summary.md
  8  0  docs/ADR.md
  1  0  docs/CONTRACT-DECISION-QUEUE.md
  7  6  docs/architecture.md
  2  0  docs/contracts/interface-contract.md
```

`docs/architecture.md` 的 7 增 6 删与改法集自洽：T1 两段句尾 append（2 行改 = 2 删 2 增）+ T2 四处 replace（4 删 4 增）+ T6 一行 append（1 增）= **7 增 6 删**，恰好吃干，无第 4 处多余改动。契约 2 增 0 删 = T4 + T5；ADR 8 增 0 删 = ADR-011（空行 + 标题 + 5 bullet）；CDQ 1 增 0 删。

**设计侧门（`proposal-consolidation-check.mjs`）：Track B 下 N/A**——本批证据集无 `decision-packet.md`，「frozen PROPOSAL ↔ 批定一致性」前提由 §7 两轮评审记录（R1 BLOCK → R2 GREEN）+ Status 行 FROZEN 承载。

**锚完整性**：`node .claude/scripts/spec-anchor-check.mjs` → `"ok": true`、`"errors": []`、`designIdCount: 61`、exit 0。

## 2 · 逐文件、逐 T 逐字核对（人读 + 程序化 byte 比对）

核对方法：把 FROZEN PROPOSAL 的每个 `text` 围栏块剥掉 proposal 侧注记号（`【…】` 头行、`原：`/`新：` 前缀）后，与落地文件做 `includes` 全等比对并计次数；T1 另做「剥掉插入段后必须等于 base 行」的 append-only 证明；T3 做「base 全文是 landed 全文的逐字前缀」的 append-only 证明。

| 改法 | 文件 | structuralTouched | semanticMatch | expected（FROZEN PROPOSAL） | actual（b3f50ebd diff） |
|---|---|---|---|---|---|
| T1-a | `docs/architecture.md` §10 决策 24 | 是 | **ok** | :54「史实更正 + 窄例外」段逐字句尾 append，原文一字不删 | 逐字命中 ×1；剥去插入段后与 base 决策 24 行**逐字相等**（纯 append） |
| T1-b | `docs/architecture.md` §2.4 原则 5 | 是 | **ok** | :58「窄例外」段逐字句尾 append | 逐字命中 ×1；剥去插入段后与 base 原则 5 行**逐字相等**（纯 append） |
| T2-1 | `docs/architecture.md` §2.1 :62 | 是 | **ok** | :80 新句 | base 存原句、landed 存新句各 ×1，原句已不在 landed |
| T2-2 | `docs/architecture.md` §2.4 原则 1 :97 | 是 | **ok** | :84 新句 | 句内内容逐字 ×1；落地行保留 `1. **哑终端**：` 的列表序号与粗体包裹（围栏给的是「句子级」原文/新文，markdown 包裹属文档骨架，见 ADVISORY A3） |
| T2-3 | `docs/architecture.md` §3.1 :115 | 是 | **ok** | :88 新句 | 同上——句内内容逐字 ×1，落地行保留 `- **离线边界**：` 包裹 |
| T2-4 | `docs/architecture.md` §4 :150 映射行 | 是 | **ok** | :92 新对策格 | base 存原格、landed 存新格各 ×1，原格已不在 landed |
| T3 | `docs/ADR.md` 文末 ADR-011 | 是 | **ok** | §6 附件围栏全文（含 `## ADR-011` 标题行）逐字 append | 逐字命中 ×1；**base ADR.md 全文（24634 chars）是 landed（26237 chars）的逐字前缀**，尾部仅 `\n` + ADR-011 块 → ADR-005 / ADR-010 原文一字未动（append-only 成立） |
| T4 | `docs/contracts/interface-contract.md` §四 | 是 | **ok** | §2 T4 围栏帧定义逐字插入于 `kidpal_story_delete` 与 `mcp` 之间 | 逐字命中 ×1；行定位 delete=:272 / **fetch=:273** / mcp=:274——与 PROPOSAL 引的 :271/:272 相差 +1，系 T5 在本文件 :130 先插入一行导致全文下移；邻位关系（session_ctx → delete → fetch → mcp）完全正确；base 中不存在该块 |
| T5 | `docs/contracts/interface-contract.md` §1.3④ | 是 | **ok** | §2 T5 围栏注记逐字插入于 `seq` bullet 与既有契约变更记录之间 | 逐字命中 ×1；行定位 seq=:129 / **note=:130** / 契约变更记录=:131，与 PROPOSAL 的 :129/:130 一致；base 中不存在该块 |
| T6 | `docs/architecture.md` §12 变更记录 | 是 | **ok** | :165 changeId 行逐字 append 于 `261015-gov-review-changes-taxonomy` 之上 | 逐字命中 ×1，行 436，261015 行在其正下一行 437；4 列；changeId = `261016-p3-story-body-sd-cache`（spine identity，R2 已裁）；**依据列 = `delivery/review/261017-…/PROPOSAL.md（契约面同批原子落）`，不含任何 FROZEN 版本 token** |

FROZEN 版本 token 落面复核：`docs/architecture.md` 中 `FROZEN v<数字>` 仅出现在 :3（文件头，base 既有）与 :438（261015 既有行，base 既有）——**本批新增行零命中**；契约与 ADR 新增行零命中；PROPOSAL 中该字样仅出现在状态行（`FROZEN v1.0`，合法）与 §2 :160 / §7 :335 两处**谈论**该占位符的说明文字，T6 承重围栏行本身已按 R2 建议删净占位。

## 3 · 超范围改动检查

`git diff 472068ea b3f50ebd` 六个文件全部有据：

1. 三件 canonical（manifest 内）；
2. `docs/CONTRACT-DECISION-QUEUE.md`（CDQ 行）——PROPOSAL §6 :240 明确列为「主线程账本动作、非 §2 改法 manifest 目标」，R2 NIT-3 认可；append-only（base 是该文件 landed 的逐字前缀），8 数据列齐全，状态 `PRECHECKED`；
3. PROPOSAL.md（标 FROZEN + §7 R1/R2，review 制品）；
4. architect-landing-summary.md（review 制品）。

**canonical 面超出 §2 的改动 = 0**；`.claude/**`、`docs/requirements.md`、固件 / front 代码均未触（无 G1/G3/self-def/review-loop 定义面改动，本批亦无此授权需求）。

## 4 · 边界守持核（人读）

- **carve-out 只给 story 正文**：`只给 story 正文` ×1、`不扩及任意业务数据` ×1（原则 5 注记）；`白名单增列「story 正文只读镜像」` 在场，同段并列「提醒 / 课表 / 日志等其余业务 / 身份 / 事项状态仍只驻 RAM、禁落非易失介质」、「『离线边界（断网无提醒）』部分语义不变」——**NFR-OFF-1「断网无提醒」语义未被改写**（§3.1 与 §4 仅在括号内注明窄例外，未动「不缓存任何事项」主干与 NFR 编号）。
- **首个请求-应答环声明在场**：契约 T4 逐字「这是契约面首个「设备发起 request → 网关 response」的请求-应答环——此前 §四私有消息（`kidpal_story_delete` / `kidpal_session_ctx`）均单向通知、无应答」；ADR-011 决策②同款表述。
- **无不精确引据**：落地文本中唯一出现 `delete 是首例请求-应答` 字样的地方是 T4 的否定式括注「**不沿用**「delete 是首例请求-应答」类不精确引据」，该括注是 PROPOSAL 逐字要求的内容；无任何一处作出「delete 是首例请求-应答环」的肯定断言。
- **ADR-011 supersedes 精确**：逐字含「取代 ADR-005 备选① / ADR-010 备选②、备选③ 的**否决理由**」+「ADR-005/010 的**决策本体**（`data.restore` additive 字段）不受影响、仍在役」——只取代否决理由、明写决策本体在役。
- **只读镜像 / 单源**：`` 只读镜像 ``、`唯一真相源`、`不承载任何写入` 在三件 canonical 中一致出现，未出现「板端可写 story」类表述；`data.restore` 补丁未被摘除（非目标守住）。

## 5 · 破坏性自检

- landing-summary 声称的每一项，都在真实 diff 中找到了对应行（§1 表逐 T 对应）；summary 第 3、4 条自述「未执行 / 归 verifier」，本 verdict 已独立补做，未采信其结论。
- 未跳面：三个 canonical 文件全部逐段核过，包括改法面最大的 T2 与契约两触点；敏感面（契约 §四协议剖面、ADR supersedes 语义、决策 24 史实重述）逐字核。
- 未凭脚本单一信号放行：结构门的 `verbatimQuoteAdvisory` 报了 5 个 missing，我逐块查证为脚本整块匹配的假阳性（见 A1），改以自己剥头后的逐段比对为准。
- 未采信 landing-summary 的「无降级、无允差、无未落项」自述。

## 6 · ADVISORY（均不阻断）

- **A1 · 结构门 `verbatimQuoteAdvisory` 5 条 missing 为假阳性**。脚本实现（`.claude/scripts/landing-fidelity-check.mjs:463-477`）取整个 fenced block 原文做 `includes` 整块匹配；本 PROPOSAL 的改法围栏块均带 proposal 侧注记号（`【插入 …】` / `【替换 …】` 头行与 `原：`/`新：` 前缀），这些记号本就不该落进 canonical，故整块必然「not-found」。剔除注记号后逐段比对：T1 两段、T2 四段、T4、T5、T6 全部逐字命中、各 ×1。建议（非本批义务）：给该 advisor 加「剥头行 + 按 `原：/新：` 分侧」的解析，或改为按围栏内非标记行段匹配，减少后续批的误报噪音。
- **A2 · CDQ 行与 PROPOSAL §6 草案非逐字**。PROPOSAL :240 措辞是「行草案如下，供主线程 landing 时逐字用」，实际落地行在草案基础上加了反引号 / 粗体、把「loop 凭据」格填成 `architect-reviewer R2 GREEN（判档 = 主路径；四分类 = ACCOMPANIES_SPEC_CHANGE）；human 立项 ack 2026-10-17`、「关联 change」格改为相对链接、末格补 `landing-verdict PASS 后翻 LANDED`。语义等价且所增信息全部为真，R2 NIT-3 亦已明示「主线程 append 时可对齐列风格」；CDQ 是账本、不在 manifest、非 canonical 契约路径。判为可接受，但「逐字用」的措辞与实况有落差，供后续批起草时注意用词。
- **A3 · T2 #2 / #3 的 markdown 包裹**。围栏给的是句子级「原文句 → 替换后句」（不含 `1. ` 列表序号与 `**标签**` 粗体），落地时保留了这两层包裹，句内文字逐字一致。属文档骨架与句子内容的层级差异，非偏差；因脚本整块比对已假阳性，特此显式登记以免后续复核误判。
- **A4 · CDQ 状态仍为 `PRECHECKED`**。PROPOSAL :240 规定「PRECHECKED→LANDED 随 landing-verdict 翻」。本 verdict 为 PASS，**主线程需在合并前把该行翻为 `LANDED`**（我按边界未改 CDQ）。
- **A5 · 合并机械闸本批未跑**：`pre-merge-escalation-gate.mjs` 归合并门前置，不在 landing-verifier 职责内；本 verdict 只给放行建议。注意 landing-summary §3 第 3 条已记载本批制品为 intake spine + review PROPOSAL 双落点、可能撞在册 `ARQ-261017-merge-gate-scope-unstated` 的 lane 白名单 BLOCK——若撞，按该 ARQ 账面实况处理并留痕，不私改闸。
- **A6 · `architect-landing-summary.md` 本身是本批新增文件**：属 AGENTS.md 制品 4 件之一，落在 review 证据目录（非 canonical），不计入 scope creep；结构门亦未把它列入 `changedCanonicalNotInProposal`。

## 7 · packet-binding 结论

- landing-summary 引的 packet-id = 「FROZEN PROPOSAL §2 改法集（T1–T6 + 契约包）+ §5 验证集」，与 review id `261017-arch-review-p3-story-body-sd-cache` 对应的 PROPOSAL 一致；PROPOSAL :6 的 packet-binding 行已按 R2 stale-1 收口为 `T1–T6`，§2 标题已按 stale-2 收口为 `T1…T6 + 契约包`。
- landing 实际范围 ⊆ 被批 packet：三 canonical 文件的每一行改动都追溯到 T1–T6 的具体围栏段；无 packet 外的 canonical 私货（`unauthorized: []` 机检 + 我的 diff 抽查一致）。
- 契约包三节（`proposedContractDiff` / `impact` / `validationPlan`）在场于 PROPOSAL §2「契约包 · 端云协议」（:170-178），其内容与 T4/T5 落地一致。放行建议：可进入合并机械凭据闸（FROZEN PROPOSAL + 本 PASS verdict + manifest 覆盖契约路径三件联立）。
