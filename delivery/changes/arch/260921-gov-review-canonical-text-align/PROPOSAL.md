# PROPOSAL · 260921-gov-review-canonical-text-align

> **状态：FROZEN（v0.4.1）· 2026-09-21 · 凭据齐 = human 立项 ack + OQ-1 重拍 (a) + R3 GREEN + `--preflight` 绿**；评审史 R1 BLOCK(F1–F5) → R2 BLOCK(F7–F9) → R3 GREEN（无升级 human 项）。**v0.4.1 = FROZEN 前微订**：承 R3 两条 NIT——证据段 ④ 的 `:255` verbatim 落点归位、§3 census① 用词收窄；**未触任何 T 的承重指令 / 目标文件 / landing-manifest / 验证集**（R3 已预先声明两条为「非阻断、可 landing 期顺手订正」）。（凭据：**human 2026-09-21 立项 ack**（前端对齐：方向 = T1 + T2 / 量级 = 3–4 文件、零契约零 G3 零新增门）；**OQ-1 两次裁定**——先裁 **(b)**（跨双轨统一入口），R1 判官 **F1** 实测出「`docs/DEV-GOVERNANCE.md`（G1 指令根）已以 **(a)** 口径写就多处、且既不在 manifest 也不在 review profile 内」后，主线程**携该新信息回请 human 重拍**，**human 2026-09-21 重拍 = (a)**（理由：G1 现文本就全是 (a)，改 (a) 只需动 G2、**零 G1 触面**）。**该重拍使 F1 的前提消失，但 F1 指出的 census 缺陷仍须修**——见 §3 与 §2 T1 证据段）。
> **来源**：本批是本日首位乘客 `260921-gov-review-launch-pipeline` 的**收口期溢出项**——该批报告向 human 列了三条待决项，human 裁「#1 登记 ARQ（已办：作为既行 `ARQ-260914-landing-preflight-anchor-parse` 的「③ 连带项」第二实例追加，非新立）、#2/#3 快速修复」。
> **本批定位**：两条都是**canonical 文本「说的」与「事实 / 彼此」不符**——T1 是多处 canonical 文面共用一个 **从未被定义过**的词（「统一入口」），T2 是一份 canonical 简报引用了一个本仓从未存在过的脚本。**共同根因**：文本在各自的演进里被局部改过，**没有任何一次改动负责让它们互相对账**——与首位乘客同一族病（那条链也是四段各自演进、缝不起来）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。**轨道自评**：不触 `docs/contracts/**`、非跨系统结构性变更 ⇒ 默认 Track B；human 本日已在首位乘客的分类环节被告知该判据并选定 Track B，本批同轨、不声明红队。**OQ-1 重拍 (a) 后**：**G1 零触面**（`AGENTS.md` 与 `docs/DEV-GOVERNANCE.md` 经核**已与 (a) 一致、不改**），profile 收敛为纯 G2。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（**T1 / T2**）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**low**（**v0.3 回落**：v0.2 曾因 OQ-1 裁 (b) 把 `AGENTS.md`（G1）纳入落地而上抬为 low-medium；**重拍 (a) 后 G1 触面归零**，理由回到 v0.1 原判——T1 是**给一个术语补定义**、T2 是**删除一处悬空引用**，两条都不改任何闸的判定语义、不动 write scope / authority、零新增门、契约面零改动。）
> **review profile**：**G2**（`.claude/references/arch-review-protocol.md` + `.claude/commands/arch-change.md` + `.claude/references/track-b-verifier-brief.md`）。**G1 / G3 / 契约面 = 零**；**工程门 = 零新增**。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/commands/arch-change.md
.claude/references/arch-review-protocol.md
.claude/references/track-b-verifier-brief.md
```

（**v0.3 变更**：v0.2 曾把 `AGENTS.md` 加入围栏块（OQ-1 裁 (b) 时）；**重拍 (a) 已移除** ⇒ 本清单回到 **3 路径**、与 T1/T2 的「目标文件」并集恒等。**`AGENTS.md` / `docs/DEV-GOVERNANCE.md` / `CLAUDE.md` 三文件经核为「已一致、不改」**——三者的逐处核验见 §2 T1 证据段的 census。）
---

## 0. 想实现的效果与差距

**目标效果一句话**：给「统一入口」这个词**补上一句定义**（写死它的所指），让 verifier 简报**不再引用不存在的东西**。

**现状差距**：

- **T1**：多处 canonical 文本把 `/arch-change` 称作「**统一入口**」，但**「统一」的所指从未在任何一处写出过定义**。涉及 ① 命令 `description`（`arch-change.md:2`）② 协议路由 SSOT（`:15`）③ `AGENTS.md:24` ④ `docs/DEV-GOVERNANCE.md` **八处**（`:101` / `:117` / `:151` / `:157` / `:243` / `:245` / `:254` / `:255`；`:118` 是命令文件引用、不计）。（**v0.3 重定性**：v0.1/v0.2 曾作「四处互相矛盾」——经 **R1-F1** 实测证伪一半，v0.1 的 census 还**整个漏了** `docs/DEV-GOVERNANCE.md`；详见 §2 T1。）
- **T2**：`.claude/references/track-b-verifier-brief.md` 的核验清单第 4 项要求核验官跑 `node .claude/scripts/cross-surface-check.mjs`——**该脚本在本仓从未存在过**（`git log --all -- .claude/scripts/cross-surface-check.mjs` 空输出）。它是自源仓迁移时带进来的文本，**迁移时没有做过引用对账**。

**影响面（T1，本批起草期实测自撞）**：首位乘客的分类环节，主线程**在「统一入口」的所指上自己填了一个答案（填成了"跨双轨"）**，据此在给 human 的选项里把 `/arch-change` 描述成「按 Track B 七步闭环起草」，**human 据此误选一次**，随后由主线程发现并重拍。即：**该词的定义空白已经真实地造成过一次误路由**（不是理论风险）——**定义没写下来，读者只能自己填，填错即误路由**。这正是本 T 改法只补一句定义、不动任何定位的原因。

**影响面（T2）**：影响面小但形态恶劣——核验官到场会**卡在一个不存在的门上**。本批首位乘客实撞：主线程派遣时不得不额外声明「该项在本仓无此件、按无此门记录」，核验官据此记 `ADVISORY A1`。**若主线程当时没主动声明，核验官可能据简报判 FAIL 或自行发明判据。**

**非目标（本批不做什么，防 scope creep）**：

- **不动 `/arch-change` 的流程正文**（步骤 1–7），也**不动任何一处的定位表述**。它们描述的是 Track A 机器、**写的是对的**；本批只**补一句定义**（把「统一入口」的所指写死）。
- **不改任何 Track A / Track B 的机制**——本批是**措辞与引用**面的对齐，零机制变更。
- **不新增对账门**。T1/T2 的病根都是「面间没对账」，但**本批不立机械齿**——一次同类实例（本批另附 ARQ 记录）不足以支撑新增门；若复发再议（见 §6 留根①）。
- **不扫其它 canonical 文件的悬空引用**。T2 只修本批实测点名的那一处；全量扫描另登留根②。

## 2. 问题、根因与改法（T1…T2）

**【硬项 · 起草期实证义务】** 本节的断言纪律：凡 regex / git / 文件存在性断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）。**本批全部实测均为 2026-09-21 于本 worktree（`C:/AI/KidPal-gov`，branch `gov/kidpal`）现跑。**

**【硬项 · 目标文件锚清单】** 每 T 末尾显式列「目标文件：<仓根相对路径清单>」；**缺锚清单 = 门 RED 点名缺锚**。改法标题一律 `### T<n> · <标题>` 标题形态；landing-manifest 围栏块一行一个裸路径。

**条目形态（五段固定式）**：问题 / 证据 / 根因 / 改法 / 〔第五段·锚清单〕。

**【起草期自检两句】** ① FROZEN 标注前跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>` 并通过 §5 验证集命令实跑；② 本批涉**术语/引用统一**，故 §3 的【硬项 · 复扫模式完备性先验】已履行（模式集见该节）。

### T1 · 「统一入口」这个词在多处 canonical 文面里都没有被定义过（v0.3 重定性）

> **⚠ v0.3 重定性（承 R1-F1 + human OQ-1 重拍）**：v0.1/v0.2 把本条写成「四处文面**互相矛盾**」——**该定性经 R1-F1 证伪了一半**。判官独立实测出 `docs/DEV-GOVERNANCE.md`（G1 指令根，**v0.1 的 census 完全漏了它**）在 `:101` / `:117` / `:151` / `:157` / `:243` / `:245` / `:254` / `:255` **八处**以 **(a) 口径**写就（**v0.4 更正**：v0.3 作「五处」、漏 `:157` / `:245` / `:255`，承 **R2-F7**），其中 `:254`（verbatim）「**统一入口**：治理 / spec 变更**默认 = Track B**（主线程起草 PROPOSAL 走起，见 §4 治理变更）；命中升级判据或声明红队 → `/arch-change <plan>`（Track A：…）」是最明确的一处。⇒ **在 (a) 口径下，这十一处（`arch-change.md:2` / 协议 `:15` / `AGENTS.md:24` 三处 + G1 的八处）各自自洽、并不冲突**：命令头「Track A 全套对抗机器入口」是**对的**，`AGENTS.md` 把 `/arch-change` 列在升级路径下也是**对的**。**真正的缺陷因此收窄为「缺定义」而非「相冲突」**——**修法是补一句定义，不是改写这些定位**。（该重定性直接呼应 human 2026-09-21 把 OQ-1 从 (b) 重拍回 (a)：G1 现文本就全是 (a)，故 (a) 下零 G1 触面。）

**问题**：`/arch-change` 被多处 canonical 文本称作 / 暗示为「**统一入口**」，但**「统一」的所指从未在任一处写出过定义**——它统一的是「Track A 内 spec / governance 两个 profile」，还是「Track A 与 Track B 两条轨」？两种读法在字面上都通顺。该空白在本次会话造成了一次**真实的误路由**（见证据末条）。影响面 = 每一个读到这些表述、要判断「我该不该走这个命令」的 agent 或 human；发生频率 = 每轮选轨都可能撞。

**证据**（全部 2026-09-21 现跑 verbatim）：

- **① 命令 `description`（本 T 要补的那处空白）**：`.claude/commands/arch-change.md:2`（frontmatter `description`）作「架构/治理变更**统一入口**（对偶 /build-feature）。把 plan mode 产出的想法交给对抗 review loop 自动跑：分类（spec/governance/mixed + 是否触契约）→ …」——**「统一入口」四字在此无任何限定**。同文件 `:8`（正文首行）作「> **本命令 = Track A 全套对抗机器入口**。默认路由 = Track B 常驻轻路径（`AGENTS.md`「治理路由」节）；启动本命令前确认命中升级判据…或 human / architect 已声明红队。」——**在 (a) 口径下这行是对的**；v0.1 曾判其"与 `description` 矛盾"，**该判断 v0.3 撤销**（行号亦由 v0.1 的 `:9` 更正为 `:8`，承 R1-F3）。
- **② 协议（路由 SSOT）—— 定义句将落在这一处**：`.claude/references/arch-review-protocol.md:15`：「**统一入口 `/arch-change`**(对偶 dev-loop `/build-feature`):把一个 plan/想法丢给 moderator,先 phase-0 分类再路由进**上面某个 profile**;`/review-arch`、`/review-governance` 是 profile 已知时的直达入口。」——**关键在「上面某个 profile」的所指**：该文同段上文只列了两个 profile（spec profile / governance profile），**二者都是 Track A 机器**（其父节标题即「##### 两个 profile（共同机制 + 各自 oracle）」）。故若按字面读，「统一入口」= **Track A 内两 profile 的统一入口**，而非跨双轨。
- **AGENTS.md 的用法**：`AGENTS.md:24`（verbatim 起首）：「**升级判据（命中任一 → Track A full-adversarial，入口 `/arch-change`）**：① 触契约面 `docs/contracts/**`（直走契约门）；② 跨系统结构性变更…」——把 `/arch-change` 列在**升级路径**名下，**在 (a) 口径下与命令头一致、不改**。
- **④ `docs/DEV-GOVERNANCE.md`（G1 指令根）——「核过、不改」（v0.3 新增；R1-F1 的 census 缺陷修复；v0.4 承 R2-F7 补全计数）**：该文**八处**以 (a) 口径写就 `/arch-change` 的定位，逐处 verbatim：`:101`（对抗 review 流程图节点）`IN["/arch-change 统一入口分类"]`；`:117`（散文）「**怎么做**：`/arch-change` 统一入口分类 → architect 起草候选 → 评审会（scenario-author 冻判据 → challenger ↔ defender 对抗 → adjudicator 裁决）→ 过 pass-oracle → 落地分流…」；`:151`（治理变更流程图边）`D1 -.->|"命中升级判据:<br/>触contracts/跨系统结构性/声明红队"| TA["Track A → /arch-change<br/>(对抗 review)"]`；`:157`（治理变更七步第 1 步的括注）「（Track A 触发时：入口 `/arch-change` 自动分类进 governance profile 并派 architect 起草；proposal 已备时 `/review-governance` 直达。）」；`:243`（治理面表行）「系统 spec（KidPal：…）| 治理路由（Track B 默认）；跨系统结构性变更 / 声明红队 → `/arch-change`（Track A spec profile …）」；`:245`（治理面表行）「系统级 UI 蓝图 / UIUX·OpenDesign 治理 | `/arch-change` governance profile 携带 UIUX 轴集（非第三 profile、非新工程门；…） | human…」；`:254`（最明确的一处）「**统一入口**：治理 / spec 变更**默认 = Track B**（主线程起草 PROPOSAL 走起，见 §4 治理变更）；命中升级判据或声明红队 → `/arch-change <plan>`（Track A：…）」；`:255`（同一注块末句）「… `/triage-change` 发现疑似事实源 → 先 `architect-reviewer`，再升级 `/arch-change` 或契约门。」。**⇒ 在 (a) 口径下八处全部自洽、本批零改动**；**v0.1 的 census 漏了整个文件**（只在 T1 目标文件里找、未做全仓扫描）——**如实记录为 R1-F1 的实质缺陷**。v0.2 曾因裁 (b) 而拟改其中数处，**重拍 (a) 后该拟改全部撤销**。
- **⑤ `CLAUDE.md`——「核过、不改」**：实测 `grep -n "arch-change" CLAUDE.md` → **零命中**（它对路由只有 `@AGENTS.md` 导入行、无独立表述），故不需同步。
- **Track B 的入口在别处另有明文**：`AGENTS.md`「治理路由」节（verbatim 起首）：「治理 / spec 变更默认走 **Track B 常驻轻路径**——七步闭环：1. 主线程起草 PROPOSAL…」——**Track B 的入口 = 主线程直接起草，无命令**。
- **真实误路由（本批同一会话实测）**：首位乘客 `260921-gov-review-launch-pipeline` 的分类环节，主线程据命令头把 `/arch-change` 理解为「Track B 默认路由的入口」，在 AskUserQuestion 里把该选项描述为「直接按 Track B 七步闭环起草 PROPOSAL」——**human 据此选了 `/arch-change`，随后由主线程发现描述有误并请 human 重拍**（该批 Track B 裁定的实际来源即重拍后的一次）。**证据锚与口径诚实声明（经 R1-F5 指出后收窄）**：发生事实（一次轨道选择的重拍）有该批 `retro.md` §5「human 触点 5 次（**轨道选择更正重拍 1 次** + …）」承载；但**「选项文案写的是『直接按 Track B 七步闭环起草』」这一处机制细节，源出主线程第一人称自述、该 retro 里没有逐字记录**。**本 PROPOSAL 如实标注该分界：机制细节 = 主线程自述，非制品级证据**（仓内无 AskUserQuestion 往返留痕）。**T1 的成立不依赖该细节**——「多处文本共用一个未定义的词」这一事实（上面 ①②③④⑤ 五条）独立充分。

**根因**：这十一处文本**各自都是自洽的**，缺的**不是一致性，而是一句定义**——「统一入口」的「统一」到底统的是什么，**没有任何一处写过**。写 `description` 的人想的是"分类后再路由"；写正文头的人想的是"本命令承载 Track A 机器"；写协议的人想的是"Track A 内 spec/governance 两 profile"；写 `AGENTS.md` 的人想的是"升级路径的入口"。**四个所指彼此相容，只是没有一个被写下**——于是读者（本批主线程即一例）只能自己填，填错了就是一次误路由。这是**同首位乘客一族**的病：局部自洽、缺那"把隐含前提写出来的一句"。

**改法（human 2026-09-21 重拍 OQ-1 = (a)：`/arch-change` 是 Track A 的入口）**：**不在任何一处的定位表述上做改动**（这十一处全部自洽、全部正确），**只补一句定义**——把「统一」的所指写死在一处 canonical 位置（路由 SSOT），使读者不必自己填。**这是本 T 的改法本体**；v0.2 的 (b) 路径（改写命令头 + `AGENTS.md` + 协议口径）**已随重拍整体撤销**，存档于本节末。

**承重 landing 指令（逐处原文引用 + 替换后完整句）**：

**① `.claude/references/arch-review-protocol.md:15`（路由 SSOT —— 定义句落在此处）** —— 现句（原文引用）：
> **统一入口 `/arch-change`**(对偶 dev-loop `/build-feature`):把一个 plan/想法丢给 moderator,先 phase-0 分类再路由进上面某个 profile;`/review-arch`、`/review-governance` 是 profile 已知时的直达入口。

替换为：
> **统一入口 `/arch-change`**(对偶 dev-loop `/build-feature`):把一个 plan/想法丢给 moderator,先 phase-0 分类再路由进上面某个 profile;`/review-arch`、`/review-governance` 是 profile 已知时的直达入口。**⚠「统一」的所指（本句即「`/arch-change` = 统一入口」这一定位的定义句）**：统一的**不是** Track A 与 Track B 两条轨，而是 **Track A 内部的 spec / governance 两个 profile**——`/arch-change` **是 Track A 的入口**；**Track B 常驻轻路径没有命令入口**，其入口 = 主线程直接起草 PROPOSAL（见「双轨路由」节）。故「命中升级判据 / 声明红队 → `/arch-change`」与「默认 Track B → 主线程起草」两句各指其一、并不冲突。**（边界 · 承 R2-F9）**「统一入口」在本仓是一个**通用短语**而非本命令的专名：`docs/DEV-GOVERNANCE.md` §7 的「**统一入口**」段标题指的是**治理 / spec 变更的入口总览**（同一段同时讲两条轨），`.claude/agents/portfolio-planner.md:25` 的「repair-intent 统一入口」指的是**修复意图的收口处**——二者各由本处上下文定义，**不在本句的覆盖范围内**；本句只定义「用作 `/arch-change` 标签时」的所指。

**② `.claude/commands/arch-change.md:2`（`description`）—— 配套半句，本次取「改」**：现作「架构/治理变更**统一入口**（对偶 /build-feature）。…」。「统一入口」四字无限定正是本 T 认定的空白；**改法 = 在「统一入口」后加最小限定词**，使描述与 ① 的定义句同指。**承重 landing 指令**：`架构/治理变更**统一入口**（对偶 /build-feature）。` → `架构/治理变更**统一入口（Track A）**（对偶 /build-feature）。`。**边界与纪律**：`description` 是**路由信号**且受 `lint-descriptions.mjs` 字数预算约束——改动**只动 `description`、逐字段隔离**（不碰 `name` / `tools` / `model`），且为**纯加限定词、不重写**（起草期实测：lint 现仅告警 `landing-verifier.md`、本命令 description 未超阈，加 10 字符仍在阈内）。

**③ `AGENTS.md` / `docs/DEV-GOVERNANCE.md` / `CLAUDE.md` —— 「核过、不改」**：`AGENTS.md:24`（原句「**升级判据（命中任一 → Track A full-adversarial，入口 `/arch-change`）**：…」，与命令头一致）与治理路由节、`docs/DEV-GOVERNANCE.md` 八处（`:101` / `:117` / `:151` / `:157` / `:243` / `:245` / `:254` / `:255`，逐处 verbatim 见证据段 ④）**在 (a) 口径下全部自洽**，本批**零改动**；`CLAUDE.md` 经核零命中、不改。⇒ **本 T 的 G1 触面 = 零**。

**【存档 · 未采纳 · v0.2 的 (b) 路径】** v0.2 曾裁 (b)（`/arch-change` = 跨双轨统一入口），拟动三处：`arch-change.md:8` 正文头改写为「统一入口…分类后路由」、`AGENTS.md:24` 删「，入口 `/arch-change`」+ 治理路由节补「命令入口（统一）」句、协议 `:15` 改写为「默认 Track B…」。**human 2026-09-21 重拍回 (a)，该路径整体撤销**——理由：R1-F1 实测出 G1（`docs/DEV-GOVERNANCE.md`）现文已全是 (a) 口径，(b) 须连带重写 G1、成本与收益不匹配。

目标文件：`.claude/references/arch-review-protocol.md`（改 · 承重指令 ①）· `.claude/commands/arch-change.md`（改 · 承重指令 ②，`description` 加限定词）

### T2 · verifier 简报引用了一个本仓从未存在的脚本

**问题**：`.claude/references/track-b-verifier-brief.md` 的核验清单第 4 项「结构门复跑」要求核验官跑两个脚本，其中 `cross-surface-check.mjs` **在本仓不存在**。核验官到场会卡在一个不存在的门上；本批首位乘客实撞，靠主线程派遣时主动声明才没走偏。

**证据**（2026-09-21 现跑）：

- 清单第 4 项 verbatim：「4. **结构门复跑**：`node .claude/scripts/cross-surface-check.mjs` + `node .claude/scripts/landing-fidelity-check.mjs <change-dir>` 亲跑看退出码。」
- **本仓无此件**：`ls .claude/scripts/cross-surface-check.mjs` → 不存在；同目录 `landing-fidelity-check.mjs` **在**。
- **从未进过版本控制**：`git log --oneline --all -- .claude/scripts/cross-surface-check.mjs` → **空输出**（非"被删过"，是**本仓从未有过**）。
- 该简报**其余引用全部在场**（同一脚本核）：`changes-index.mjs` → `.claude/scripts/changes-index.mjs` 存在；`landing-verdict.md` 为产出物名（非路径引用，不计）。**故本批只此一处悬空。**
- 实撞留痕：首位乘客 `260921-gov-review-launch-pipeline` 的 `landing-verdict.md` **ADVISORY A1**（「canonical 简报点名的 `cross-surface-check.mjs` 本仓不存在（迁移残余）——按「无此门」记录、不判 FAIL」）。

**根因**：该简报是自源仓迁移的文本（其头注自述「Kimi CLI 无 typed landing-verifier agent」——本仓有 typed agent），**迁移时只搬了文本、没有做过引用对账**。断言：迁移对账是一次性动作，**做完就没人再回来核**，于是悬空引用长期留在 canonical 里、每次派遣都靠人临时补一句。

**改法**：**承重 landing 指令**——`.claude/references/track-b-verifier-brief.md` 清单第 4 项，从两脚本并列改为只留在场者，并把缺失事实就地钉死（防后来者再以为是漏跑）：

原文引用：
> 4. **结构门复跑**：`node .claude/scripts/cross-surface-check.mjs` + `node .claude/scripts/landing-fidelity-check.mjs <change-dir>` 亲跑看退出码。

替换为：
> 4. **结构门复跑**：`node .claude/scripts/landing-fidelity-check.mjs <change-dir>` 亲跑看退出码。**（本仓无 `cross-surface-check.mjs`**——该件为自源仓迁移带入的悬空引用，本仓从未存在；若你到场的清单里仍见到它，即为本文修订未生效，记 ADVISORY 并按「无此门」处理。260921-canonical-text-align T2 删除。）

并在该简报头注的「harness 降级标准声明」句后补一句同旨注记（落点措辞由 architect 定），使"本仓有 typed agent、无该脚本"这两件事实在同一处可见。

目标文件：`.claude/references/track-b-verifier-brief.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 角色与流程定义 | `.claude/commands/arch-change.md` | **G2** | T1（承重指令 ②：**仅 `description`（`:2`）加限定词「（Track A）」**；**正文 `:8` 不改**——v0.2 的 (b) 路径已随 OQ-1 重拍整体撤销，承 R2-F8） |
| 角色与流程定义 | `.claude/references/arch-review-protocol.md` | **G2** | T1（取 (a) 时改 `:15` 措辞 + 补边界句；取 (b) 时该处不动） |
| 角色与流程定义 | `.claude/references/track-b-verifier-brief.md` | **G2** | T2（删悬空引用 + 补就地说明 + 头注补注记） |
| 入口与指令根 | `AGENTS.md` / `docs/DEV-GOVERNANCE.md` / `CLAUDE.md` | **未触**（**v0.3**） | OQ-1 重拍 (a) 后三者**经核与 (a) 一致、零改动**（逐处 verbatim 见 §2 T1 证据段 ③④⑤）；v0.2 拟改 `AGENTS.md` 的 (b) 路径**已随重拍撤销** |
| 强制执行层 / 契约面 / 工程门 | — | **零** | 三条路径均不触；**零新增工程门** |

**【硬项 · sweep 透镜义务】** 本批**不引入新门、不引入新判定 pattern**（T1 是文面表述对齐、T2 是删引用），故无 pattern 全仓预跑义务。**已履行的 census（现跑留痕）**：① `/arch-change` 定位表述的**目标面枚举 = 十一处**（**v0.4.1 收窄，承 R3 NIT-2**：该数只覆盖 `/arch-change` 的定位表述、非全仓 pattern 级枚举；后者见本节复扫表）（`arch-change.md:2` / `arch-review-protocol.md:15` / `AGENTS.md:24` 三处 + `docs/DEV-GOVERNANCE.md` 八处 `:101` / `:117` / `:151` / `:157` / `:243` / `:245` / `:254` / `:255`；`CLAUDE.md` 零命中），各附 verbatim（**承 R2-F8 更正**——v0.3 原记「四处…`:9`」是 stale 残留）；② verifier 简报**全部路径引用**的存在性核（`cross-surface-check.mjs` MISS / `changes-index.mjs` OK / `landing-verdict.md` 为产出物名不计）；③ 该悬空件的版本控制史核（`git log --all` 空输出）。

**【硬项 · 复扫模式完备性先验】** 本批涉**符号/引用**面，模式集显式枚举如下（2026-09-21 现跑，范围 = `.claude/**` + `docs/**` + `AGENTS.md`）：

| 模式 | 命中 | 归宿 |
|---|---|---|
| `cross-surface-check` | **9 行 / 4 文件**（**经 R1-F2 独立复扫更正**；v0.1 原记「仅 `track-b-verifier-brief.md` 一处」是**事实错误**）：`track-b-verifier-brief.md:23`（**唯一悬空引用** = T2 触面）· `landing-fidelity-check.mjs:636/638/639/649/658`（5 行，**非悬空**——该脚本以 `crossSurfaceScript` resolve + `existsSync` 守卫 + `subGate` 名承载「本仓无此件」这一实况，是**有意的缺件守卫**）· `governance-gate-shared.mjs:268/284`（2 行注释引据）· `ARCH-REVIEW-QUEUE.md:85`（`ARQ-260831-cross-surface-check-missing`，**已结项**） | **仅 `track-b-verifier-brief.md:23` 改**；其余三文件是「文档化缺件 / 机械守卫 / 已结 ARQ」，**不该清、不属 T2 触面** |
| `统一入口` | **12 行 / 7 文件**（**经 R2-F8 更正**；v0.3 原记「四处 / 2 文件」为漏计）。按所指分三类：**义 A · 用作 `/arch-change` 的标签 = 10 行**（`review-moderator.md:3/23/52`、`arch-change.md:2`、`review-arch.md:12`、`review-governance.md:13`、`arch-review-protocol.md:15/31`、`DEV-GOVERNANCE.md:101/117`）· **义 B · 入口总览 = 1 行**（`DEV-GOVERNANCE.md:254`）· **义 C · repair-intent 的收口处 = 1 行**（`portfolio-planner.md:25`） | **仅 `arch-change.md:2` 改**（加限定词）；义 A 其余 9 行在 (a) 口径下自洽、**零改动**；义 B / 义 C 是**通用短语的另两个填充语**（非同一术语），**不改**——边界句见 §2 T1 承重指令 ① 末（承 R2-F9；判官原记「11 + 1」两义，实测 `portfolio-planner.md:25` 为**第三义**，本行按实测写） |
| `本命令 = Track A` | **3**（`arch-change.md:8` / `review-arch.md:8` / `review-governance.md:8`；经 R1-F4 更正，v0.2 原记「1」为漏计） | **仅 `arch-change.md:8`** 属 T1 触面；后两处是 **Track A 直达命令**（跳过分类），其自述 **正确、不改** |

**完备性论证（v0.2 重写；v0.4 承 R2-F8/F9 补全）**：`统一入口`（**12 行 / 7 文件**，已按 A / B / C 三义逐行归类）与 `本命令 = Track A`（3 行 / 3 文件）两模式已逐处枚举并给归宿。`cross-surface-check` 的完备性改由**全仓 grep（`.claude/` + `docs/` + `AGENTS.md`）**取证，得 **9 行 / 4 文件**、已逐行归类（见上表）——**v0.1 的「仅一处」是漏枚举**，其错误根源 = **只在 T2 的目标文件里找、没做全仓扫描**（**这正是本批 T1/T2 要治的「面间不对账」病，我本人在起草期又犯了一次**——如实记录，不自掩）。**T2 的实质由此收窄**：该缺件是**已知实况**——有**已结 ARQ**（`ARQ-260831-cross-surface-check-missing`）+ 有**机械守卫**（`landing-fidelity-check.mjs` 的 `existsSync` 跳过、`subGate` 记账）；**缺陷只在 verifier 简报仍按「应跑该门」写**，使核验官与主线程每轮须临时补口径（首位乘客 ADVISORY A1 即此）。

## 4. human 取舍点（OQ）

**OQ-1 · `/arch-change` 是 Track A 的入口，还是跨双轨的统一入口？** —— **human 2026-09-21 两次裁定，终裁 = (a) Track A 的入口**。过程：首次裁 **(b)**；R1 判官 **F1** 实测出 `docs/DEV-GOVERNANCE.md`（G1 指令根）**已以 (a) 口径写就多处**、且该文件既不在 manifest 也不在 review profile 内 ⇒ (b) 须**连带重写 G1**、文件数从 human ack 的「3–4」扩到 5；主线程**携该新信息回请重拍**，human **重拍 = (a)**（G1 现文本就全是 (a)，故 (a) 零 G1 触面）。**(b) 的完整选项文本与本批为其拟定的三处改法已存档于 §2 T1 末「【存档 · 未采纳】」段**。

- **(a) Track A 的入口**（**推荐**）：缺陷在协议 `:15` 的「统一入口」措辞可被读宽。**推荐理由**：① 协议同段上文的「上面某个 profile」按字面只可能指 spec / governance 两个 **Track A** profile；② `AGENTS.md` 把 Track B 的入口明文写为「主线程起草 PROPOSAL」（无命令），两条并列即已自洽；③ 命令头「Track A 全套对抗机器入口」与之一致；④ **本路只改 1 个文件、零 G1 触面**。（**v0.4 补注**：承重指令 ② 的 `description` 限定词使 T1 实际触 **2** 个 G2 文件；「1 个文件」是 OQ 呈报 human 时的原始表述，保留不改——**G1 触面为零**这一结论不受影响。）**代价**：`description` 里的「统一入口」一词仍留白，须由 architect 判是否加限定词（改 `description` 是路由信号面，须逐字段隔离）。
- **(b) 跨双轨统一入口**：缺陷在命令头。**代价**：须同批改 `AGENTS.md:24`（**触 G1**）→ riskTier 上抬、G1 触面须在 FROZEN 前重估；且须论证"分类后判为 Track B 时如何交回主线程起草"（命令的步骤 3–7 全是对抗机器），**这条本身带一点设计分叉**。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · 结构门（landing 前）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260921-gov-review-canonical-text-align/PROPOSAL.md` → 预期 `{"ok":true,...}`。**起草期现跑（v0.3 复跑）= `ok:true, anchorCount:3, findings:[]`**，`anchorUnion` = `arch-change.md` / `arch-review-protocol.md` / `track-b-verifier-brief.md` —— 与 landing-manifest 围栏块**恒等 3 路径**。**v0.2 说明**：v0.1 曾因 `AGENTS.md` 为条件锚而预置了「FROZEN 前裁剪」机制；**OQ-1 裁 (b) 后该条件消失、无裁剪动作**（试金石：若你跑到 `anchorCount ≠ 3` 或 `findings` 非空，即本 PROPOSAL 在 v0.2 的某处改动把锚面改坏了，**停、回主线程**，勿自行修）。
- **V2 · T1 定义句在场 + 定位表述未被改动（landing 后）**：**该条为语义断言，判官 / 核验官须人读判读，不得只数命中数**。命令（分别取证、输出留痕）：
  - `grep -n "统一" .claude/references/arch-review-protocol.md` → 预期 `:15` 行**含定义句**（可判据：「统一的**不是** Track A 与 Track B 两条轨，而是 Track A 内部的 spec / governance 两个 profile」，或同义措辞），且该句与同行前半「统一入口 `/arch-change`…」**共处一句、不矛盾**；**并须含边界句**（承 R2-F9）——即明说 `DEV-GOVERNANCE.md` §7 的「统一入口」段标题（**入口总览**义）与 `portfolio-planner.md:25` 的「repair-intent 统一入口」（**收口处**义）**不在本句定义范围内**。**缺边界句 = 本 T 未按 frozen 改法落地。**
  - `grep -n "统一入口（Track A）" .claude/commands/arch-change.md` → 预期命中 `:2` 的 `description`（承重指令 ② 加的限定词在场）。
  - **「未改动」正向断言（防越界）**：`git diff --stat` 对 `.claude/commands/arch-change.md` **应只显示 `description` 一行变化**；`AGENTS.md` / `docs/DEV-GOVERNANCE.md` / `CLAUDE.md` **应零变化**（本 T 声明的 G1 零触面）。
  - **合取判据（人读）**：读 ①（协议定义句）与 ②（命令 description 限定词）应得出**同一条规则**——「`/arch-change` = **Track A** 的统一入口（统一的是 Track A 内 spec/governance 两 profile）；Track B 无命令入口，由主线程直接起草」。任一处与此规则不符，即本 T 未达效果。
- **V3 · T2 悬空引用收窄（landing 后）**：`grep -n "cross-surface-check" .claude/references/track-b-verifier-brief.md` → 预期命中行**含「本仓无此件」的说明**，不再是裸的「跑它」指令。**范围收窄声明（v0.2 更正，承 R1-F2）**：本断言**只覆盖 verifier 简报**——v0.1 写「canonical 面零命中」是**错的**：`landing-fidelity-check.mjs`（5 行，含 `existsSync` 守卫）/ `governance-gate-shared.mjs`（2 行引据）/ `ARCH-REVIEW-QUEUE.md:85`（已结 ARQ）**各有应当保留的引据，落地后必仍命中 ⇒ 原断言必假红**。**这三处不改、不属本批触面。**
- **V4 · verifier 简报引用存在性核（landing 后，真空绿规约）**：对 `track-b-verifier-brief.md` 重跑本批 §3 的引用存在性脚本 → 预期**零 MISS**（先断言扫描到的引用数 > 0，防空输入真空绿）。
- **V5 · 路由回归（landing 后）**：`node .claude/scripts/route-regression-check.mjs` → 预期 ok 数不减、fail = 0（本批动了 `commands/` 与 `references/`）。**另跑** `node .claude/scripts/lint-descriptions.mjs` → 预期**仅既有** `landing-verifier.md` 超长告警、无新增（**取 (a) 且 architect 判需动 `arch-change.md` 的 `description` 时，本行须重跑并逐条核**）。
- **V6 · 落地保真（landing 后，主线程派 landing-verifier 执行）**：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260921-gov-review-canonical-text-align` → 预期 ok；逐文件人读 diff 核对 manifest（**3 路径**）与 §2 各 T 目标文件并集恒等。

## 6. 留根清单

1. **「面间对账」不立机械齿（承首位乘客 §4 建议 2/4）**——T1/T2 的病根都是「文本各自演进、无人对账」，但本批**只修实例、不建门**。**触发条件** = 同类实例再现（第三例）；届时与既行 `ARQ-260914-landing-preflight-anchor-parse` 的「③ 连带项」（其非机械化半已有本日第二实例在案）一并评估是否值得机械化。**交接动作**：交 `/loop-retro` 观察。
2. **canonical 文件的悬空引用全量扫描未做**——本批只修实测点名的那一处；`track-b-judge-brief.md` 经同法核**引用全部在场**，但其余 canonical 文件（agents / references / commands / skills）**未扫**。**交接动作**：值得一次全量只读 census（脚本化：抽文件内路径型 token → 存在性核），并登 ARQ；本批不夹带。
3. **`track-b-verifier-brief.md` / `track-b-judge-brief.md` 的「Kimi 降级头注」在本仓不适用**——本仓有 typed `architect` / `landing-verifier` agent，两份简报的头注（自述「Kimi CLI 无 typed … agent，由零上下文 coder 子 agent 承担」）与本仓实况不符。本批首位乘客的两次派遣均由主线程显式略去该头注。**本批未处理**（T2 只动第 4 项与头注一句注记，**未删头注**——删它属"harness 形态声明"面，须单独判）。**交接动作**：与留根②的全量 census 合批处置。
4. **本批的自律件**：起草期全部 census 与存在性核均为 2026-09-21 现跑；T1 的 OQ-1 为**真分叉**（两条改法的落点、G 层、riskTier 都不同），故**未自裁**、全文交 human。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-21 - architect

verdict: BLOCK(F1,F2,F3,F4,F5)

判官亲核（全部 2026-09-21 于本 worktree `C:/AI/KidPal-gov` 现跑）：承重指令 ①–④ 与 T2 的「原文引用」与磁盘逐字吻合（arch-change.md:8 / AGENTS.md:24 / arch-review-protocol.md:15 / track-b-verifier-brief.md:23 均 grep 取证比对一致）；`git log --oneline --all -- .claude/scripts/cross-surface-check.mjs` 空输出、`ls` 该件不存在，故 T2「本仓从未存在」成立；`--preflight` = `ok:true, anchorCount:4, findings:[]` 与 V1 一致；packet-id 行（§0 Status 块第 6 行）与 landing-manifest 围栏块（4 路径）在场。OQ-1 裁 (b) 后「触 G1 是否构成升级判据」复核：**PROPOSAL 判断成立**——SSOT（AGENTS.md:24 + arch-review-protocol.md「双轨路由」）的直接 Track A 判据仅两条（触 `docs/contracts/**` / 跨系统结构性变更），且「governance 类变更不再强制升级」明文覆盖 G1，故触 `AGENTS.md` 不构成升级判据。但 §3 完备性论证**不成立**，两条目标符号的枚举各有缺口（F1/F2/F4）。

- **F1 · BLOCKING · 升级 human —— T1 census 漏了 `docs/DEV-GOVERNANCE.md`（G1），其对 `/arch-change` 的定位与 OQ-1 裁 (b) 直接冲突**。§3「复扫模式完备性先验」与 §0「非目标」均只枚举四处（arch-change.md / AGENTS.md / arch-review-protocol.md），实测 `docs/DEV-GOVERNANCE.md` 至少四处以 (a) 口径定位 `/arch-change`：`:254`「> **统一入口**：治理 / spec 变更**默认 = Track B**（主线程起草 PROPOSAL 走起，见 §4 治理变更）；命中升级判据或声明红队 → `/arch-change <plan>`（Track A：先分类…）」——与 (b)「`/arch-change` = 跨双轨统一入口、默认 Track B」直接相悖；`:151`「命中升级判据…→ Track A → `/arch-change`（对抗 review）」；`:157`「（Track A 触发时：入口 `/arch-change` 自动分类…）」；`:243`「跨系统结构性变更 / 声明红队 → `/arch-change`（Track A spec profile…）」。后果：① 本批目标效果（「让 `/arch-change` 在 canonical 文本里只有一个说法」）**达不到**——落地后 DEV-GOVERNANCE.md 仍保留 (a) 口径、与已改的 arch-change.md:8 新句互相矛盾；② 这是又一个 G1 文件，不在 landing-manifest、不在 review profile 枚举里。**升级 human 理由**：承重指令 ③ 现句位「治理路由节补一句」只落 AGENTS.md，但 DEV-GOVERNANCE.md §7「统一入口」注（`:254`）是同一词、同一定位，若不动则矛盾残留、若动则 human 立项 ack 的「量级 = 3–4 文件」被扩到 5 文件——这是 ack 范围之外的面，须 human 重拍。改法方向：补全 census 枚举 DEV-GOVERNANCE.md `:151/:157/:243/:254`（+ review-moderator.md description/`:23`/`:52` 的「统一入口」表述、review-arch.md / review-governance.md 的「本命令 = Track A」与「统一入口」），按 (b) 决定是否同批对齐并扩 manifest / review profile；或显式 re-scope 排除并写理由（但「只有一个说法」的目标表述须随之收窄）。

- **F2 · BLOCKING —— §3「`cross-surface-check` 仅 track-b-verifier-brief.md 一处」是事实错误，且 V3「canonical 面零命中」落地后必假红**。独立复扫 `grep -rn "cross-surface-check" .claude/ docs/ AGENTS.md` 命中 9 行、分布 4 文件：`track-b-verifier-brief.md:23`（T2 目标）+ `.claude/scripts/landing-fidelity-check.mjs:636/638/639/649/658`（`:649` `const crossSurfaceScript = resolve(projectDir, '.claude/scripts/cross-surface-check.mjs');`，其余为「未迁入」实况注记 + `subGate: 'cross-surface-check'`）+ `.claude/scripts/lib/governance-gate-shared.mjs:268/284`（防回退注记）+ `docs/ARCH-REVIEW-QUEUE.md:85`（`ARQ-260831-cross-surface-check-missing` 已结行）。后三文件**不在 landing-manifest**，落地后仍命中；故 V3 的 `grep -rn "cross-surface-check" .claude/ docs/ AGENTS.md` → 预期「零命中」**不可达**（同首位乘客 retro F1「V6 零命中不可达」一族病）。且这些命中是「文档化缺件 + existsSync 守卫 + 已结 ARQ」，**非悬空引用**、不该清。改法方向：① census 修正为「`track-b-verifier-brief.md:23` = 唯一真悬空引用（待 T2 删）；其余 8 行为『缺件文档化/守卫/ARQ』，非悬空、不动」；② V3 断言收窄为「`track-b-verifier-brief.md` 内 `cross-surface-check` 零命中」（或显式白名单后三文件的合法命中）；③ 完备性论证句「不存在别处引用同名脚本的未枚举形态」删除或改写。

- **F3 · SHOULD —— 承重指令 ① 的行号锚 :9 应为 :8**。实测 `grep -n "本命令 = Track A 全套对抗机器入口" .claude/commands/arch-change.md` → **`8:`**（frontmatter 占 1–4、`# /arch-change` 在 `:6`、空行 `:7`，正文首行在 `:8`，`:9` 为空行）。T1 证据段「`:9`（正文首行）」、承重指令 ① 标题「`.claude/commands/arch-change.md:9`」、§3 sweep「`本命令 = Track A` | 1（`arch-change.md:9`）」三处均写 `:9`。原文引用本身逐字吻合（quote-based 落地不会改错），但行号锚偏移一位，落地期若按行号定位会撞空行。改法：三处 `:9` 统一改 `:8`。

- **F4 · SHOULD —— §3 sweep「`本命令 = Track A` | 1」少计，实为 3 文件**。实测该句在 `.claude/commands/arch-change.md:8`、`.claude/commands/review-arch.md:8`、`.claude/commands/review-governance.md:8` 三处。后两处是 spec/governance profile **直达命令**，其「本命令 = Track A 全套对抗机器入口」是正确表述、**不改**；但 census 计数「1」是事实错误，且未说明「为何另两处不动」。改法：计数改 3 并注明「仅 arch-change.md 需改（承重指令 ①），review-arch.md / review-governance.md 为 Track A 直达命令、表述正确、不改」。

- **F5 · SHOULD —— T1 证据第 5 条「真实误路由」的详细叙事无 verbatim 支撑，证据锚弱**。该条断言「主线程据命令头把 `/arch-change` 理解为…在 AskUserQuestion 里把该选项描述为『直接按 Track B 七步闭环起草 PROPOSAL』——human 据此选了 `/arch-change`，随后…重拍」，引「retro.md §1/§5」。实测：retro §1 是时间账表、**不含**此叙事；retro §5 仅一句「human 触点 5 次（**轨道选择更正重拍 1 次** + …）」，未载具体选项文案；首位乘客 PROPOSAL Status 块 `:11` 仅记「分类环节被显式询问『Track A vs Track B』并选定 Track B」。故「真实误路由」的**发生事实**（一次轨道重拍）有支撑，但**机制细节**（命令头误读 + 选项文案）是主线程自述、未落 cited 源。改法方向：把证据锚改指向真正承载该叙事的落盘处，或把断言收窄为「曾发生一次轨道选择更正重拍（retro §5）」。（此条不承改法正确性——(b) 是 human 独立裁。）

- **F6 · NIT —— V2 对 protocol「统一入口」的 grep 口径不精确**。V2 断言「`grep -n "统一入口" .claude/references/arch-review-protocol.md` → 预期命中行**含**『默认 Track B』」，但该文件 `:15` 与 `:31`（「## Intake + 分类（`/arch-change` 统一入口）」标题）都命中；落地后仅 `:15` 含「默认 Track B」，`:31` 不含。改法方向：口径改为「`:15` 行含『默认 Track B』」，或注明 `:31` 为标题、语义不冲突。另 V2 合取判据只 cover 4 文件、不含 DEV-GOVERNANCE.md（根因见 F1，随 F1 一并收口）。

复核结论：结构字段（packet-id 行 + 4 路径 landing-manifest 围栏块）在场、preflight 绿；但 §3 两条目标符号的完备性论证各有一处实质缺口（F1/F2/F4），其中 F1 触及 human 立项 ack 的「量级」边界并令目标效果不可达，须升级 human；F2 令 V3 假红。建议主线程就 F1 补全 census 并回 human 重拍 scope，就 F2/F3/F4/F5 逐条修订后进入 R2 聚焦复核。

### R2 - 2026-09-21 - architect

verdict: BLOCK(F7,F8,F9)

聚焦复核（只核 R1 修订到位性 + 是否引入新矛盾，不做全量重核）。R1 六条修订到位性：**F2 全到位**（§3 census 改为 9 行/4 文件并逐行归类、完备性论证重写、V3 收窄为只覆盖 verifier 简报——实测 `grep -rn "cross-surface-check" .claude/ docs/ AGENTS.md` = 9 行/4 文件，与 F2 逐字吻合）；**F4 全到位**（`本命令 = Track A` 改 3）；**F5 全到位**（误路由叙事收窄并显式标注「机制细节 = 主线程自述、非制品级证据」）；**F6 全到位**（V2 改为 `grep -n "统一"` 并 scope 到 `:15` 行）；**F1/F3 部分到位**（F1 的 census 补齐了 `docs/DEV-GOVERNANCE.md` 与 `CLAUDE.md`，但计数仍漏三处，见 F7；F3 在证据段 ① 已改 `:8`，但 §3 sweep 表两行仍残留 `:9`，见 F8）。**重定性本身成立、非逃避**：human 携 R1-F1 实测把 OQ-1 从 (b) 重拍回 (a)，(a) 口径下四处（连同 G1）确为各自自洽，「缺定义」而非「互相矛盾」的定性准确，改法收窄为「补一句定义 + description 加限定词」与 (a) 决策一致、承重指令 ① 替换句自洽。但本轮仍有三处未收口：

- **F7 · SHOULD —— `docs/DEV-GOVERNANCE.md` 的 census 计数仍错：v0.3 证据段 ④ 写「五处」，实测为八处**。`grep -n "arch-change" docs/DEV-GOVERNANCE.md` 的 `/arch-change` 定位表述共 8 行：`:101` / `:117` / `:151` / `:157` / `:243` / `:245` / `:254` / `:255`（`:118` 是命令文件引用 `arch-change.md`、非定位表述，不计）。v0.3 列了 `:101/:117/:151/:243/:254` 五处，**漏 `:157`**（「（Track A 触发时：入口 `/arch-change` 自动分类进 governance profile 并派 architect 起草…）」）、**`:245`**（「系统级 UI 蓝图…`/arch-change` governance profile 携带 UIUX 轴集」）、**`:255`**（「先 architect-reviewer，再升级 `/arch-change` 或契约门」）三处——三者同为 (a) 口径，结论「零改动」**不受影响**，但「五处…逐处 verbatim」的枚举是事实错误，且这是 R1-F1 之后**第二次** census 不完整。

- **F8 · SHOULD —— §3 sweep 表与 census① 段有 stale v0.2 残留未随 v0.3 清理**（同源：v0.3 改了 §2 证据段与完备性论证，但 §3 多处未同步）：① sweep 表 `arch-change.md` 行（`:118`）仍写「**仅 OQ-1 取 (b) 时**触正文 `:9`；取 (a) 时仅 description 微调」——(b) 已重拍、`:9` 应为 `:8`；② census① 段（`:124`）仍写「`/arch-change` 定位表述的**四处枚举**（`arch-change.md:2` / `:9`、`arch-review-protocol.md:15`、`AGENTS.md:24`…）」——v0.3 已扩为 8 处且含 DEV-GOVERNANCE/CLAUDE，`:9` 应为 `:8`，此句整个未更新；③ sweep 表 `统一入口` 行（`:131`）仍写「**待判官 spot-check**（…**四处**中用到该词的：`arch-change.md:2`、`arch-review-protocol.md:15`）」——与完备性论证「已逐处枚举并给归宿」自相矛盾，且计数错（实测 `统一入口` 在 `.claude/**`+`docs/**`+`AGENTS.md` 共 **12 行**，非「四处…2 文件」）。这三处是 v0.3 自身的「面与面对不上」，须同批清理。

- **F9 · SHOULD —— (a) 定义句「别处照此读」对 `docs/DEV-GOVERNANCE.md:254` 的「统一入口」标题过载，该词的跨双轨义仍残留**。定义句（承重指令 ①）写「统一的**不是** Track A 与 Track B 两条轨，而是 Track A 内部的 spec / governance 两个 profile」，并令「别处照此读」；但 `DEV-GOVERNANCE.md:254` 的标题「**统一入口**：治理 / spec 变更**默认 = Track B**（主线程起草…）；命中升级判据或声明红队 → `/arch-change <plan>`（Track A…）」**正文正是跨双轨的路由总览**（同时描述 Track B 与 Track A）——即定义句明言「不是」的那个义。实测 `统一入口` 全仓 12 行里，11 行是「`/arch-change` 的标签」义（与 (a) 定义一致），**唯 `:254` 标题是「路由总览」义**。后果：落地后该词仍有两义，读者照「别处照此读」去读 `:254` 标题会读不通；PROPOSAL 却把 `:254` 判为「在 (a) 口径下全部自洽、零改动」——这只对「`/arch-change` 定位」成立，对「`统一入口` 词的义项」不成立。改法方向：census 显式区分两义——`DEV-GOVERNANCE.md:254` 的「统一入口」标题改为无歧义词（如「入口总览」），或定义句收窄为「`/arch-change` 的『统一入口』标签 = Track A 内两 profile」，并明说 `:254` 标题是另一义、不在「别处照此读」覆盖内。二者择一即可，不引新门、不须再升 human。

复核通过项：`--preflight` = `ok:true, anchorCount:3, findings:[]`（与 V1 一致）；`lint-descriptions.mjs` 仅告警 `landing-verifier.md`（363 > 320），`arch-change.md` description 未超阈（承重指令 ② 的 lint 断言成立）；landing-manifest 围栏块（3 路径，`:13-17`）与 packet-id 行（`:7`）在场。承重指令 ① 替换句自洽（「命中升级判据 → `/arch-change`」与「默认 Track B → 主线程起草」两句各指其一、不冲突）；承重指令 ② 与 ① 同指。F7/F8/F9 均为 SHOULD、agent 可修、不须再升 human；建议主线程修订后进 R3 聚焦复核。

### R3 - 2026-09-21 - architect

verdict: GREEN

聚焦复核（只核 F7/F8/F9 修订到位性 + 是否引入新矛盾，不做全量重核）。三条 findings 修订到位性逐条亲核磁盘：

- **F7 到位**：全线改「八处」并补齐 `:157` / `:245` / `:255`——§0 `:28`、重定性 blockquote `:54`（含 `:255` verbatim）、改法 ③ `:82` 三处均列齐 `:101/:117/:151/:157/:243/:245/:254/:255` 八字，`:118` 已显式声明「命令文件引用、不计」。实测 `grep -n "arch-change" docs/DEV-GOVERNANCE.md` 的定位表述恰为这 8 行，吻合。
- **F8 到位**：三处点名残留已清——sweep 表 `arch-change.md` 行（`:118`，改「仅 description(:2) 加限定词、正文 :8 不改」）、census① 段（`:124`，改「全仓枚举 = 十一处」并列八处行号）、`统一入口` 行（`:131`，改「12 行 / 7 文件」并分 A/B/C 三义）、完备性论证（`:134`）。**主线程自查的 5 处同族残留（§0:5、T1 标题 :52、:54 末「改写这些定位」、:56「读到这些表述」、:70「这十一处」）也已全部改净，我未再发现 live 区有其它陈旧「四处 / 五处」计数或面与面对不上**。
- **F9 到位（方向二正确，方向一不必）**：承重指令 ① 的替换句删去「别处照此读」、改「本句即『`/arch-change` = 统一入口』这一定位的定义句」，并追加**边界句**——明说「统一入口」是通用短语非本命令专名、`DEV-GOVERNANCE.md` §7 标题指**入口总览**、`portfolio-planner.md:25` 指**修复意图的收口处**、二者不在本句覆盖范围内。**独立裁断三点**：① **边界句确实关掉了 F9 的过载**——它把「统一 = Track A 内两 profile」显式限定为「用作 `/arch-change` 标签时」，并预先把 `:254` 标题（入口总览义）与 `:25`（repair-intent 义）点名排除，落地后读者照定义句去读 `:254` 不再误读（该句同时说清 `/arch-change` 的「统一」不是两条轨、而 `:254` 标题恰恰是两条轨的入口总览，两义各归其位）；② **10 / 1 / 1 是实测正解**——`portfolio-planner.md:25`「repair-intent 统一入口在这里，不直接进 planner」既非 `/arch-change` 标签义、也非路由总览义，是**第三义（修复意图收口处）**，我 R2 记「11 + 1」是把该行误并入标签义，**R2-F9 原文计数有误，以 v0.4 的 10 / 1 / 1 为准**；③ **方向二足矣，不须方向一**——改 `:254` 标题会把 G1 拉回 manifest（3→4 路径），与 human 重拍 (a) 的「零 G1 触面」前提相悖，边界句在 G2 面内就解除了两义共存，是代价最小的正解。§5 V2 新增的「定义句须含边界句，缺则该 T 未按 frozen 落地」断言可执行、且与承重指令 ① 一致。

两处 NIT（非阻断，可留待 landing 期顺手订正，不挡 GREEN）：
- **NIT-1**：证据段 ④（`:63`）写「八处…逐处 verbatim」但该行的 verbatim 枚举只列 `:101/:117/:151/:157/:243/:245/:254` 七处，`/255` 的 verbatim 落在重定性 `:54` 而非 `:63`；改法 ③（`:82`）写「逐处 verbatim 见证据段 ④」指到 `:63`，实缺一处。计数本身正确（八处），只是 `:255` 的 verbatim 位置与指向不一致，建议在 `:63` 补一句 `:255` verbatim 或把 `:82` 的指向改为「见 `:54` 与 `:63`」。
- **NIT-2**：census①（`:124`）「`/arch-change` 定位表述的**全仓枚举 = 十一处**」的「全仓」用词偏宽——「十一处」实为 T1 证据段 ①②③④⑤ 的目标面子集；完整 pattern 级枚举是 sweep 表「`统一入口` 12 行 + `本命令 = Track A` 3 文件」（`/131`/`/132`）。建议把「全仓枚举」改为「T1 证据段目标面枚举」以免与 sweep 表的「全仓」语义打架。

GREEN 结构核（均已亲核）：landing-manifest 围栏块 = 3 路径（`:13-17`）在场；packet-id 行（`:7`）在场；§2 各 T「目标文件」并集 = T1 `{arch-review-protocol.md, arch-change.md}` ∪ T2 `{track-b-verifier-brief.md}` = **3 路径，与 manifest 恒等**；`--preflight` = `ok:true, anchorCount:3, findings:[]`（本判官 R3 复跑）。三个目标文件（`arch-change.md` / `arch-review-protocol.md` / `track-b-verifier-brief.md`）与 `AGENTS.md` / `docs/DEV-GOVERNANCE.md` 均干净未落地。承重指令 ①（协议 `:15` 加定义句 + 边界句）与 ②（`arch-change.md:2` description 加「（Track A）」）原文引用逐字吻合、替换句自洽且同指。**结论：可标 FROZEN，进入 packet-bound landing + landing-verifier 核验。**
