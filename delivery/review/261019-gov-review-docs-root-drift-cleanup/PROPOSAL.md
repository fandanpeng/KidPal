# PROPOSAL · 261019-gov-review-docs-root-drift-cleanup

> **状态：FROZEN v1.0 · 2026-10-19 · 立项 ack（human 前端对齐四条原话在案）+ architect-reviewer R2 GREEN（R1 BLOCK(F1) 否回修订到位）**（起草 = architect，主线程派发；输入 = human 原话四条 + 主线程勘查产出〔事实由 architect 起草期逐条复核，见 §2 各 T 证据段〕）
> **承载 ARQ**：无——本批为 human 直接发起的文档漂移清理，非 ARQ 消费。关联背景批：`261015-gov-review-face-loop-unify`（契约门现行口径之源：无人签门、机械凭据闸）、`261019-gov-review-docs-root-ledger-rehome`（ADR 迁 `docs/decisions/` 与 contract 域立账之源，本批 T2/T4 是该批的引用改指连带）。
> **立项凭据**（human 原话，逐字）：①「特别是一些不正规的，比如在正文里边又加一行什么"日期改了改了"之类的注释类东西，再查一下这个事。」②「第七和 9 到 10 条，我们现在就拍呗。我觉得这些叠了某某日期的这种不正规的写法，就把它清理掉喽。」③「选（a），按你的建议改吧」——(a) = 剥掉内嵌注记、历史决策档条目回到原文形态；现状口径由既有权威面（正文现行节 / requirements.md §2.3 等）承载，不回头改写历史；决策 14/15 的「原文 + 单行 ⚠️ 历史口径指针」是同节已有先例形态。④「你还是走一个 gov change 的正规流程吧」——本批走 `/gov-change` Track B。
> **review 轨**：Track B 常驻轻路径（SSOT：`.claude/references/gov-change.md`）。**判档提示（起草自评）**：本批触 `docs/DEV-GOVERNANCE.md`（G1）+ `docs/architecture.md`——按 gov-change.md「轻路径判据」第③条，触 `docs/DEV-GOVERNANCE.md` 即命中恒 Track B（哪怕只改一个字符），不命中轻路径。正式判档归 architect-reviewer（只升不降）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T9）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（纯文档散文/死链/历史档编辑政策复原；零机器面触点、零行为语义变化、零门语义变化；全量可 git revert 回滚）。
> **review profile**：G1 + spec 面（均显式声明）。G1 触点 = `docs/DEV-GOVERNANCE.md` 三处散文漂移修正（T7/T8/T9，零门语义、零权限语义变化）；spec 面触点 = `docs/architecture.md` 五处（T1–T6，治理投影漂移修正 + 历史档编辑政策复原，零架构语义变化）；G2 / G3 / 契约面零触（实证见 §3 与 §5 第 6 条 diff 范围核对）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/architecture.md
docs/DEV-GOVERNANCE.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：`docs/architecture.md` 与 `docs/DEV-GOVERNANCE.md` 的散文面回到「与既定现实一致」——契约门写现行口径（261015 批起无人签门）、ADR 链接指向现址（261019 批已迁 `docs/decisions/ADR.md`）、术语表与结构 SSOT 指向现文件、团队表写的是存在的角色、契约未结项状态归 `docs/debt/contract/` 单一权威，且 architecture.md §10 历史决策档回到其文首自宣的「历史档正文保持原文」编辑政策（human 已裁 (a) 方案：剥内嵌注记、现状口径由既有权威面承载、不回头改写历史）。

**现状差距**：主线程勘查 + architect 起草期复核（2026-10-19）确认漂移两文件——human 勘查口径 8 处（按主题归并），落为 9 个改法条目：architecture.md 侧 T1–T6、DEV-GOVERNANCE.md 侧 T7–T9。主题数与条目数差异两笔：「§10 两处内嵌注记清理」拆为 T5（决策 12）/ T6（决策 20）两条（处置形态不同：裸删 vs 先例整形）；「`:253` 缺口条目 + `:56` human 行」拆为——`:253` 与 `:54-55` 团队表并为 T8（同根因：写的是不存在的角色）、`:56` 独立 T9（同族但根因是 261015 批漏改孤岛）。逐条 before→after 精确锚见 §2。

**为什么改（漂移的危害）**：这两个文件是仓的单一事实源（SSOT）——架构 SSOT 与开发域治理 G1 根。SSOT 说谎的代价不是脏，是读者按死口径行动：看到「契约门 = architect-reviewer 四分类 + human 签字」会以为契约变更还要等人签（261015 批已撤）；点 `[ADR.md](ADR.md)` 死链找不到决策档；读团队表会等一个不存在的 `uiux-designer` 接系统级 UI brief；两条相邻同名「契约门在办」bullet 内容互相矛盾，读者无法判断哪条是真；历史决策档里「⚠️ 2026-09-13 → 删除线 → ⚠️ 2026-09-14 再翻转」三层叠注已不可读，且违反该档文首自宣的「正文保持原文」政策。

**非目标（防 scope creep）**：① 零架构语义、零治理语义、零门语义变化——本批只让文字回到现实，不改任何现实；② 不动 §10 其余决策的 ⚠️ 注记（决策 4/6/7/8/17/19/21/22/24 等，见 §6 留根 3）；③ 不处理 `docs/debt/contract/` 域 7 条 deferred 条目本身；④ 不改 DEV-GOVERNANCE.md :47「合并走本地可回滚路径（--no-ff）」（已核 = Pod lane 现行口径，非漂移，见 §6 留根 1）；⑤ architecture.md 各处「← 收编自旧 §x」收编注本体保留（同文件 §2/§5 多处同款，不在本批范围），仅删 T3 点名的元注记尾巴；⑥ 「音量调节」未入 debt 账本的登记缺口不擅自补登（登记动作归主线程，见 §6 留根 4）。

**面归属提示（请 architect-reviewer 四查专核）**：architecture.md 五处均为**治理投影的漂移修正**（契约门口径、契约账指针、ADR 迁址连带）与**历史档编辑政策复原**，零架构语义变化，故随本批走 `/gov-change`；若 reviewer 判其中任何一处实为架构语义变更、应拆去 `/arch-change`，按否回处理（打回本 PROPOSAL 拆分）。

## 2. 问题、根因与改法（T1–T9）

### T1 · architecture.md :343 契约门口径写成已撤销的人签门

**问题**：§7 节首行括注写「契约门：architect-reviewer 四分类 + human 签字」。261015-gov-review-face-loop-unify 批已撤销人类契约签字门（契约变更 = 架构变更的一种，走 `/arch-change` loop，合并过机械凭据闸 `pre-merge-escalation-gate.mjs` 三件联立），本行是撤门前的旧口径，与同仓 `.claude/rules/contracts.md`、DEV-GOVERNANCE.md §4/§5（:88-89、:178）的现行口径直接矛盾——读者会以为契约变更还要等 human 签字。

**证据**：`docs/architecture.md:343`（2026-10-19 Read 逐字）：

```
数据格式与端云接口的单一事实源 = [docs/contracts/interface-contract.md](contracts/interface-contract.md)（契约门：architect-reviewer 四分类 + human 签字）：
```

现行口径权威面：`.claude/rules/contracts.md`「契约变更 = 架构变更的一种（261015-gov-review-face-loop-unify）：走 `/arch-change` loop，无人类签字门」；DEV-GOVERNANCE.md :88「**无人类签字门**（261015 批撤销——human 只在立项 ack 与例外升级出场）」。

**根因**：261015 批撤人签门时改了 rules / references / DEV-GOVERNANCE 的契约节与门表，architecture.md §7 这处行内括注漏改——投影面改指不全的典型残留。

**改法**：括注改写为现行口径。替换后完整行：

```
数据格式与端云接口的单一事实源 = [docs/contracts/interface-contract.md](contracts/interface-contract.md)（契约变更走 /arch-change loop，合并过机械凭据闸 pre-merge-escalation-gate.mjs——261015 批起无人签门）：
```

目标文件：`docs/architecture.md`（改）

### T2 · architecture.md :348-349 两条相邻同名「契约门在办」bullet 互相矛盾

**问题**：§7 末尾有两条相邻的、同以「契约门在办」起头的 bullet，内容互相矛盾：:349 是旧版（2026-09-14 对照 review 四件全「随下次契约门裁」），:348 是新版（「已销三留一」）。同一段落两条同名状态行各说各话，读者无法判断哪条是真。且这种「在 architecture.md 内嵌逐条契约状态」的写法本身就是腐化源——状态的真家已在 261019-gov-review-docs-root-ledger-rehome 批立为 `docs/debt/contract/` 域（一条目一文件 + 生成件 INDEX）。

**证据**：`docs/architecture.md:348-349`（2026-10-19 Read 逐字；两条 bullet 全文）：

```
- **契约门在办**：CDQ-260913-spec-split-fallout（spec.md 归档断链 + 验收基线重建，HUMAN_APPROVED 在办）；原登记四件（2026-09-14 对照 review）**已销三留一**——① E1 schedule 视图承载一周总览网格数据（requirements v1.19）✅ 已落地（M3-K1 2026-09-21 落契约 §1.3② 周网格；行内原标「在办」为**遗留漂移**，门 1 F3 注记「本笔顺带收」）；② 「故事删除」动作（requirements v1.47/v1.48，§5.4 数据视图）✅ 已落地（契约 §四 `kidpal_story_delete`，2026-09-24 契约门，M4-K1）；④ `kidpal_session_ctx` 端→网关消息（2026-09-14 已定义）✅ 已落地（CDQ-260914 LANDED；front 拦截半 = M4-K1，板端发送半归 M4-K3）；③ 「音量调节」动作（requirements v1.44，§5.1.1 MCP 工具面）**仍在办**。
- **契约门在办**：CDQ-260913-spec-split-fallout（spec.md 归档断链 + 验收基线重建，HUMAN_APPROVED 在办）；另登记四件随下次契约门裁（2026-09-14 对照 review）：① E1 schedule 视图承载一周总览网格数据（requirements v1.19）；② 「故事删除」动作（requirements v1.47/v1.48，§5.4 数据视图）；③ 「音量调节」动作（requirements v1.44，§5.1.1 MCP 工具面）；④ `kidpal_session_ctx` 端→网关消息（2026-09-14 已定义，随本批契约门落）。
```

已核事实（2026-10-19 实测）：① `docs/debt/contract/` 域现存 7 条目（CDQ-260913-spec-split-fallout / CDQ-260914-kidpal-session-ctx / CDQ-260921-m3-k1-schedule-grid / CDQ-260924-m4-k1-story-delete / CDQ-260924-m4-k1-storylist-semantics / CDQ-260924-m4-k1-storytext-seq / CDQ-261017-p3-story-body-sd-cache），frontmatter `status` 全为 `deferred`（逐件 head 实测）；条目的落地记录与剩余条件由条目正文 append-only 承载（例：CDQ-260924-m4-k1-story-delete 正文存「LANDED」原文行 + 剩余条件，2026-10-19 Read 核实）——账本确能承接状态唯一权威之位。② 「音量调节」control 动作（requirements v1.44）在 `docs/debt/` 全域 grep 零命中（2026-10-19 实测）——它是唯一只在 architecture.md 内嵌登记的在办项，新写法必须保留它的可见性，否则丢账。

**根因**：:349 是 2026-09-14 写入的原始状态行，:348 是后续对照 review 的更新行——更新时新起一行而未删旧行，同名双行并存；更深处是「契约状态内嵌在架构文档」这一放置方式本身与账本单一权威原则冲突（CDQ 单文件队列时代养成的习惯，账本已迁 `docs/debt/contract/`）。

**改法**：两条 bullet 删除，并为一条干净口径——状态唯一权威改指 `docs/debt/contract/`，architecture.md 不再内嵌逐条状态（防再腐）；「音量调节」以一行可见性锚保留。状态快照类日期戳一律不写进新 bullet（与「不再内嵌逐条状态」同句自冲突，且属本批要清的叠日期写法同族——R1-F1 否回修订）；逐条现状以账本 INDEX 现查为准。替换后完整 bullet：

```
- **契约决策账**：契约相关未结项的状态唯一权威 = [docs/debt/contract/](debt/contract/)（一条目一文件 + 生成件 [INDEX.md](debt/contract/INDEX.md)）；本档不再内嵌逐条状态，防内嵌清单与账本再漂移。唯一例外 = 「音量调节」control 动作（requirements v1.44，§5.1.1 MCP 工具面）**仍在办**——debt 账本全域查无此条目（2026-10-19 grep 实测），本档保留此一行作可见性锚防丢账，随下次契约门处置后连本行一并删。
```

目标文件：`docs/architecture.md`（改）

### T3 · architecture.md :341 收编注尾巴「，『在办』状态更新」= 元注记噪声

**问题**：§7 文首收编注以「，『在办』状态更新」收尾——这是当年更新状态时留下的元注记（注记的注记），不承载任何现行信息，正是 human 点名的「叠了某某日期/状态的不正规写法」同族噪声。

**证据**：`docs/architecture.md:341`（2026-10-19 Read 逐字）：

```
> ← 收编自旧 §4；CDQ-260902 已 CLOSED（2026-09-03 human 批四题，契约重写原子落地），「在办」状态更新。
```

**根因**：状态更新时代顺手在收编注尾巴追加的元注记，事后无人回收。

**改法**：删尾巴「，「在办」状态更新」，收编注本体保留（同文件 §2/§5 多处同款收编注不在本批范围，见 §0 非目标⑤）。替换后完整行：

```
> ← 收编自旧 §4；CDQ-260902 已 CLOSED（2026-09-03 human 批四题，契约重写原子落地）。
```

目标文件：`docs/architecture.md`（改）

### T4 · architecture.md :5 与 :397 两处 `[ADR.md](ADR.md)` 死链

**问题**：ADR 已随 261019-gov-review-docs-root-ledger-rehome 批迁 `docs/decisions/ADR.md`（迁址记在该文件文首 :5，2026-10-19 Read 核实），architecture.md 两处相对链接 `[ADR.md](ADR.md)` 仍指 docs 根旧址，点击即死链。全仓普查（git grep，2026-10-19）确认 `](ADR.md)` 链接形残留仅此两处。

**证据**：

- `docs/architecture.md:5`（2026-10-19 Read 逐字，摘录）：「架构决策本体 = [ADR.md](ADR.md)（本章 §10 只留索引 + MVP 时代决策历史档）」。
- `docs/architecture.md:397`（2026-10-19 Read 逐字，摘录）：「**2026-09-14 起，新架构决策一律登记 [ADR.md](ADR.md)（append-only，五件格式）」。
- 普查命令与结果：`git grep -n "](ADR.md)" -- ":!delivery" ":!firmware"` → 仅上两行命中（2026-10-19）。

**根因**：261019 迁址批的引用改指清单覆盖了 rules / references / agents / skills / DEV-GOVERNANCE / glossary 等面（该批 T6），architecture.md 这两处行内链接漏改——同批已改 :373 一处（gov INDEX 改指），这两处漏网。

**改法**：两处链接目标改指 `decisions/ADR.md`（显示文字「ADR.md」不动）。替换后完整片段：

- :5：「架构决策本体 = [ADR.md](decisions/ADR.md)（本章 §10 只留索引 + MVP 时代决策历史档）」。
- :397：「**2026-09-14 起，新架构决策一律登记 [ADR.md](decisions/ADR.md)（append-only，五件格式）」。

目标文件：`docs/architecture.md`（改）

### T5 · architecture.md :410 决策 12 内嵌注记三层叠加，讲的还是另一个需求

**问题**：决策 12 的本体（D3 笔顺格式定稿：makemeahanzi，strokes SVG path + medians 骨架点）至今未变；但条目尾巴叠了两层注记（2026-09-13 ⚠️ 新增注 → 2026-09-14 删除线「注记已消解」→ 再补一句「笔顺数据格式仍不变」），讲的都是「每日一字」——那是**另一个需求**，其生死已在权威面留痕：requirements.md §7 开放问题 5 已关闭（:678，2026-10-19 grep 核实「已消解（2026-09-14 v1.36）」）+ §8 已砍清单留痕（:698）+ §9 变更记录 v1.36 行（:744）。历史决策档无需为另一个已砍需求背负两层注记；且 §10 文首（:397）自宣「历史档正文保持原文」，本清理是让违规内嵌注记回到该政策。

**证据**：`docs/architecture.md:410`（2026-10-19 Read 逐字，全文）：

```
12. **汉字笔顺**：✅ D3 格式定稿——makemeahanzi 数据源（已实测跑通），OpenClaw 组 JSON（strokes SVG path + medians 骨架点），板子只渲染（LVGL 绘制 + 逐笔动画）。⚠️ 2026-09-13 需求侧新增「每日一字」（P4 空态由设备主动出一个字教学，requirements.md S-F3.4）：选字来源与下发链路未定（挂 requirements.md §7 开放问题 5），笔顺数据格式不变（D3 沿用）。~~注记已消解（2026-09-14 requirements v1.36）~~：撤 P4 常驻页、查字改会话内专门窗口（`char` view 覆盖窗），每日一字随之砍除、开放问题 5 关闭——笔顺数据格式仍不变。
```

**根因**：2026-09-13 需求侧加「每日一字」时顺手在架构决策 12 尾巴追加 ⚠️ 注记（requirements §9 v1.16 行 :764 如实记了这笔「连带 arch 决策 12 追加 ⚠️ 注记」）；2026-09-14 该需求砍除时又用删除线 + 再注的方式叠第二层——两次都是「在原文上叠注」而非「回原文」。

**改法**：采 human 裁定 (a)——整条注记尾巴删除，条目回原文形态（决策本体未变，无需任何 ⚠️ 历史口径指针；「每日一字」的生死留痕由 requirements.md §7/§8/§9 承载）。替换后完整行：

```
12. **汉字笔顺**：✅ D3 格式定稿——makemeahanzi 数据源（已实测跑通），OpenClaw 组 JSON（strokes SVG path + medians 骨架点），板子只渲染（LVGL 绘制 + 逐笔动画）。
```

目标文件：`docs/architecture.md`（改）

### T6 · architecture.md :418 决策 20 三层勘误叠加 → 按决策 14/15 先例整形

**问题**：决策 20 有**真实语义翻转**（原口径「BOOT 关弹窗同时照常翻页」→ requirements v1.39 翻转「覆盖层内 BOOT = 关弹窗回原页、不翻页」），现状权威 = requirements.md §2.3（:130 自宣「本节为权威键位定义」；v1.39 隔离口径在案 :44/:68，§9 变更记录 v1.39 行 :741——均 2026-10-19 grep 核实）。但历史档条目叠了三层：① 2026-09-13 删除线层（~~BOOT 同时照常翻页——措辞对齐~~）；② v1.39 翻转 ⚠️ 层；③ 「本句为 v1.13 旧口径，v1.43 勘误漏及本条，随 2026-09-14 架构对照 review 补注」元注层——三层咬合后已不可读，且删除线 + 元注正是 human 点名的不正规写法。与 T5 不同，本条有真翻转，不能裸删注记——按 human 裁定 (a)，采同节决策 14/15 已有先例形态（「原文 + 单行 ⚠️ 历史口径指针」；先例原文见 :412-413，2026-10-19 Read 核实）。

**证据**：`docs/architecture.md:418`（2026-10-19 Read 逐字，相关片段——外层括注内「渲染即确认出队」段）：

```
**渲染即确认出队**（KEY/BOOT 关弹窗即出队；~~BOOT 同时照常翻页——2026-09-13 措辞对齐 requirements.md §2.3 键位权威，原「只关弹窗」易误读为 BOOT 不翻页~~ ⚠️ 2026-09-14 requirements v1.39 翻转：覆盖层内 BOOT = 关弹窗回原页、**不翻页**（本句为 v1.13 旧口径，v1.43 勘误同步 §2.9/§3.1 时漏及本条，随 2026-09-14 架构对照 review 补注）；弹窗无超时驻留、多提醒排队逐个弹（录音态例外 = 排队延后，见 §2.9）；设备易失 id 集合幂等去重）
```

先例形态（决策 14，:412，逐字摘录）：「⚠️ 2026-09-14 requirements v1.36：撤 P4 查字常驻页，页循环 4→3（首页/课表/星星记录本）——本条为历史口径，现状见 §5.1.1（渲染节点层）。」

**根因**：两次措辞对齐与一次翻转都用「在原文上叠注/删除线」落地，三层叠加；根因同 T5，区别仅在本次翻转有实时语义、注记不能裸删。

**改法**：恢复被删除线划掉的原文「BOOT 同时照常翻页」，三层勘误换成单行 ⚠️ 历史口径指针（human 裁定 (a) 指定文）。替换后完整片段：

```
**渲染即确认出队**（KEY/BOOT 关弹窗即出队；BOOT 同时照常翻页。⚠️ 2026-09-14 requirements v1.39 翻转：覆盖层内 BOOT = 关弹窗回原页、不翻页——本条为历史口径，现状见 requirements.md §2.3。弹窗无超时驻留、多提醒排队逐个弹（录音态例外 = 排队延后，见 §2.9）；设备易失 id 集合幂等去重）
```

目标文件：`docs/architecture.md`（改）

### T7 · DEV-GOVERNANCE.md :3 文首迁移注记两处死指针

**问题**：文首迁移注记有两处指向不存在/已过时对象的指针：①「结构参考 = `.claude/rules/repo-structure.net-template.md`（原仓模板，含本仓未建面）」——该文件在本仓不存在（`.claude/rules/` 仅 authoring-agents-skills / contracts / prompt-authoring / repo-structure 四份，2026-10-19 ls 实测）；本仓结构 SSOT 是 `repo-structure.md`（其文首自宣「结构的单一事实源是本文件」）。②「术语表暂缺，建立时补」——`docs/glossary.md` 已建，且在 docs 根白名单内（repo-structure.md 白名单行写「`glossary.md`（术语表，已建）」）。全仓普查（git grep，2026-10-19）确认 `repo-structure.net-template` 与「术语表暂缺」字样均仅此一处。

**证据**：`docs/DEV-GOVERNANCE.md:3`（2026-10-19 Read 逐字，相关两段摘录）：「结构参考 = `.claude/rules/repo-structure.net-template.md`（原仓模板，含本仓未建面）。流程正文在各 operational 文档；契约 SSOT 永远是 [docs/contracts/**](contracts/)；术语表暂缺，建立时补。」

**根因**：迁移注记写于 2026-08-27/29 迁入期，指向原仓模板与当时的缺口状态；本仓后来立了自己的结构 SSOT 与术语表，注记未随现实更新——「暂缺，建立时补」类暂态句无人回收的典型。

**改法**：两处改指现状。替换后完整片段：「结构参考 = [repo-structure.md](../.claude/rules/repo-structure.md)（本仓结构 SSOT）。流程正文在各 operational 文档；契约 SSOT 永远是 [docs/contracts/**](contracts/)；术语表 = [glossary.md](glossary.md)（已建）。」

目标文件：`docs/DEV-GOVERNANCE.md`（改）

### T8 · DEV-GOVERNANCE.md :54-55 + :253 团队表与缺口登记写的是不存在的角色

**问题**：§2 团队表有两行写 `uiux-designer`（设计师，系统级 UI brief 候选）与 `uiux-author`（UI 翻译，TaskPackage 级），§8 已知缺口还有一条「uiux-designer 系统级 brief 的 moderator dispatch 接线」。已核现实（2026-10-19）：① `.claude/agents/` 无 `uiux-designer` 定义（ls 实测 17 份 def 无此件）；② `uiux-author` 现行职责已变——其 def（`.claude/agents/uiux-author.md`，Read 核实）= requirement-change 档 2 触 UI 批次按需求修订生成/修订系统级 UI（档 2 流程 4b，生成模式；出图 od_* 归主线程；dev-loop 包内不派，消费走 planner 挂图 + brief-inputs 透传），不再是「把冻结 brief 翻译成 TaskPackage uiux block」；③ 全仓普查（git grep，排除 delivery/ 与实现面，2026-10-19）：`uiux-designer` 在 canonical 面仅 :54 与 :253 两处命中（另两件命中在 `docs/debt/gov/_archive/2026/` 归档条目与归档 INDEX，append-only 历史、合法保留）。缺口条目所述「dispatch 尚未接线」的前提（角色存在、待接线）已不成立——系统级 UI 已由档 2 流程 4b（uiux-author 生成模式、主线程 dispatch）承载，按 §8 自宣规则「对应流程成文 landing 后从本节移除，不滞留已结项」应删。

**证据**：

- `docs/DEV-GOVERNANCE.md:54-55`（2026-10-19 Read 逐字）：

```
| 设计师 | `uiux-designer` | 系统级 UI brief 候选 |
| UI 翻译（TaskPackage 级） | `uiux-author` | 把冻结的系统 UI brief 翻译成 TaskPackage `uiux` block |
```

- `docs/DEV-GOVERNANCE.md:253`（2026-10-19 Read 逐字）：「- **uiux-designer 系统级 brief 的 moderator dispatch 接线**：起草者已定（`uiux-designer`），dispatch 尚未接线。」
- 对照现实：`.claude/agents/uiux-author.md` description 逐字（摘录）：「requirement-change 档 2 触 UI 批次……主线程在 architect 可行性消解后、requirement-reviewer 审稿前派你按需求修订生成/修订系统级 UI（档 2 流程 4b，生成模式）。……dev-loop 包内不派本 agent（消费走 planner 挂图 + brief-inputs ①c 透传）。」

**根因**：团队表与缺口条目写于 uiux-author 还是「TaskPackage 级翻译者」定位的时代；其后 uiux-author 职责改为档 2 生成模式（260929-gov-review-agent-def-alignment 批起，def 现行文为证）、`uiux-designer` 始终未建——DEV-GOVERNANCE.md 的编制投影未跟随。

**改法**：① 团队表两行并一行，按 uiux-author 现行口径写；② :253 缺口条目整行删除。替换后完整行（团队表）：

```
| 系统级 UI 设计执行 | `uiux-author` | requirement-change 档 2 触 UI 批次按需求修订生成/修订系统级 UI（档 2 流程 4b 生成模式：brief 升版 + 设计 HTML 迭代 + 出图规格单；出图 od_* 归主线程；dev-loop 包内不派——消费走 planner 挂图 + brief-inputs 透传） |
```

目标文件：`docs/DEV-GOVERNANCE.md`（改）

### T9 · DEV-GOVERNANCE.md :56 human 职责行仍写「契约门」

**问题**：§2 团队表 human 行写「契约门、高影响拍板、治理批准」——「契约门」与 261015 批撤销人签门矛盾；同文件 §4 契约节（:88「无人类签字门……human 只在立项 ack 与例外升级出场」）与 §5 工程门表（:178「261015 批替代人签门」）均已更新，此行是漏改的孤岛（与 T1 同族，G1 侧残留）。

**证据**：`docs/DEV-GOVERNANCE.md:56`（2026-10-19 Read 逐字）：「| 最终负责人 | human | 契约门、高影响拍板、治理批准 |」。

**根因**：261015 批改 DEV-GOVERNANCE.md 时更新了契约专节与门表，§2 团队表 human 行漏改。

**改法**：改写为现行出场点口径。替换后完整行：

```
| 最终负责人 | human | 立项 ack、例外升级拍板、高影响拍板、治理批准 |
```

目标文件：`docs/DEV-GOVERNANCE.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| spec 面（显式声明） | `docs/architecture.md` | 未触机器面 | T1–T6：治理投影漂移修正 + 历史档编辑政策复原，零架构语义变化；面归属请 reviewer 专核（§0 面归属提示） |
| G1（显式声明） | `docs/DEV-GOVERNANCE.md` | G1 | T7–T9：散文漂移修正，零门语义、零权限语义、零编制实况变化（写的是已存在的现实） |
| G2 / G3 / 契约面 / 实现面 | — | 未触 | 实证 = §5 第 6 条 diff 范围核对（landing 后 `git diff --stat` 恰两文件） |

**sweep 透镜说明**：本批不落地新门/新断言，无判定 pattern 需全仓预跑。**复扫模式完备性**（退役语面残留清扫）：本批清理的退役语 = `契约门在办` / `human 签字` / `](ADR.md)` / `注记已消解` / `措辞对齐 requirements.md` / `repo-structure.net-template` / `术语表暂缺` / `uiux-designer` / `契约门、高影响拍板` 九个 token，词形变体评估——「契约门」三字本体保留（T1 新文仍用「契约变更」表述、rules/contracts.md 等权威面同款，非退役词）；`uiux-designer` 无前缀/后缀变体（agent 名定长）；`](ADR.md)` 为链接形完整形态（bare「ADR.md」分工句不在清理范围，见 T4 改法）；余者均为定长短语无变体。普查命令与逐条命中记录见各 T 证据段与 §5 第 1/7 条。**仓内残留补记（R1-F3）**：`契约门在办` token 在 `docs/notes/arch-restructure-skeleton.md:27` 有一处残留（2026-09-14 架构重构骨架工作稿的章节映射表行，逐字：「纯导航指针（contracts/ 四件 + 契约门在办）」，2026-10-19 grep 核实）——notes 为非 canonical 经验/工作稿面，该行是历史工作记录、保留不改；canonical 面该 token 仅 architecture.md :348/:349 两处（本批 T2 清除），§5 第 1 条探针口径不变。

## 4. human 取舍点（OQ）

无新增待裁 OQ。本批唯一真实分叉——§10 历史决策档两处内嵌注记的处置方案——human 已于立项时当面裁定 (a)（立项凭据③：剥内嵌注记回原文形态、现状口径由权威面承载、决策 14/15 为先例形态；T5 按「注记涉另一已砍需求、裸删回原文」、T6 按「有真翻转、原文 + 单行 ⚠️ 历史口径指针」分别落实，两处均为 (a) 的机械适用）。其余八处（T1–T4、T7–T9）均为「文字回到已核实现实」的漂移修正，无方向性取舍。若 reviewer 评审中出现方向级分叉（如面归属判定翻转、T2 状态口径写法争议），按升级判据升 human。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 时点标注：除注明 post-merge 者外均为 **pre-commit**（landing worktree 上、commit 前实跑）。before 态探针 = 起草期 2026-10-19 实测留证（证明每条断言能判别：落地前命中、落地后翻零/翻一）。

1. **退役语残留清零**（pre-commit，九探针）：`grep -n "契约门在办" docs/architecture.md` → 零（before = 2 行，:348/:349）；`grep -n "human 签字" docs/architecture.md` → 零（before = :343）；`grep -n "](ADR.md)" docs/architecture.md` → 零（before = :5/:397 两处）；`grep -n "注记已消解\|每日一字" docs/architecture.md` → 零（before = :410）；`grep -n "措辞对齐 requirements.md" docs/architecture.md` → 零（before = :418）；`grep -n "repo-structure.net-template\|术语表暂缺\|契约门、高影响拍板" docs/DEV-GOVERNANCE.md` → 零（before = :3/:3/:56 各一）；`grep -n "uiux-designer" docs/DEV-GOVERNANCE.md` → 零（before = :54/:253）。
2. **新文在场断言**（pre-commit，逐条 grep -F 逐字）：① `grep -F "（契约变更走 /arch-change loop，合并过机械凭据闸 pre-merge-escalation-gate.mjs——261015 批起无人签门）" docs/architecture.md` → 1 行；② `grep -F "契约相关未结项的状态唯一权威 = [docs/debt/contract/](debt/contract/)" docs/architecture.md` → 1 行；③ `grep -c "decisions/ADR.md" docs/architecture.md` → 2；④ `grep -F "⚠️ 2026-09-14 requirements v1.39 翻转：覆盖层内 BOOT = 关弹窗回原页、不翻页——本条为历史口径，现状见 requirements.md §2.3" docs/architecture.md` → 1 行；⑤ `grep -F "结构参考 = [repo-structure.md](../.claude/rules/repo-structure.md)" docs/DEV-GOVERNANCE.md` → 1 行；⑥ `grep -F "术语表 = [glossary.md](glossary.md)（已建）" docs/DEV-GOVERNANCE.md` → 1 行；⑦ `grep -F "立项 ack、例外升级拍板、高影响拍板、治理批准" docs/DEV-GOVERNANCE.md` → 1 行；⑧ `grep -F "系统级 UI 设计执行" docs/DEV-GOVERNANCE.md` → 1 行。
3. **链接目标存在**（pre-commit）：`test -f docs/decisions/ADR.md && test -f docs/glossary.md && test -f .claude/rules/repo-structure.md && echo ALL-EXIST` → 输出 ALL-EXIST（起草期 2026-10-19 已实测三件均在）。
4. **锚机检回零**（pre-commit）：`node .claude/scripts/spec-anchor-check.mjs` → 输出 `"ok":true`、`errors:[]`（本批不改 architecture.md 任何小节编号；起草期基线 2026-10-19 实测 `"ok":true`、`errors:[]`、`archSectionCount:47`——落地后 archSectionCount 不变）。
5. **uiux-designer 全仓残留 ≡ 白名单**（pre-commit）：`git grep -n "uiux-designer" -- ":!delivery" ":!firmware" ":!server" ":!tools" ":!ESP32-S3-RLCD-4.2-Demo"` → 命中集合 ≡ 白名单两件：`docs/debt/gov/_archive/2026/ARQ-260923-planner-roster-reference-drift.md`（已结条目 append-only 历史原文）与 `docs/debt/gov/_archive/2026/INDEX.md`（该域归档 INDEX 薄行，生成件历史）——白名单外任何命中 = 落地遗漏，退回补改（起草期 before 态实测 = 白名单两件 + DEV-GOVERNANCE.md :54/:253 两处）。
6. **diff 范围 ≡ manifest**（pre-commit）：`git diff --stat` → 恰 `docs/architecture.md` 与 `docs/DEV-GOVERNANCE.md` 两文件；逐文件人读 diff 逐字核对 = §2 各 T 的 before→after（landing-verifier 核验本体）。
7. **结构门干跑**（起草期，随 FROZEN 申请附结果）：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261019-gov-review-docs-root-drift-cleanup/PROPOSAL.md` → 零形态错、anchorUnion ≡ {`docs/architecture.md`, `docs/DEV-GOVERNANCE.md`}（两文件九锚行，并集两成员）。
8. **post-merge 簿记**（合并本地 main 后，主线程）：main 上复跑第 4 条（锚机检）+ `node .claude/scripts/changes-index.mjs --upsert 261019-gov-review-docs-root-drift-cleanup`（INDEX 簿记惯例，upsert 跑在落地 commit 之前）。

## 6. 留根清单

1. **DEV-GOVERNANCE.md :47「合并走本地可回滚路径（--no-ff）」已核排除**：经查 = Pod lane 现行口径（`.claude/references/pod-coordination.md:101` merge-driver 本地 --no-ff 可回滚路径），与 merge-to-main skill 的 ff-only（gov/feature→main lane）并存，两 lane 两口径、非漂移——本批不改。
2. **收编注本体保留政策**：architecture.md §2/§5/§6/§7 多处「← 收编自旧 §x」注是 2026-09-14 重构的溯源记录，保留；本批仅删 T3 点名的元注记尾巴一处。
3. **§10 其余决策的 ⚠️ 注记未动**：决策 4/6/7/8/17/19/21/22/24 等亦带日期叠注形态（部分为实质内容注记，如决策 21 配置级修正、决策 24 史实更正与窄例外——非纯噪声），未在本批勘查清单内、未动；是否按同政策续整（哪些算「不正规叠注」、哪些是实质历史内容）留 human 后续裁定，需要时另立批。
4. **「音量调节」未入 debt 账本的登记缺口**：T2 以 architecture.md 一行可见性锚防丢账；是否补登记进 `docs/debt/contract/` 域，登记动作归主线程（human-approved），本批不擅自补登。
5. **contract 域 7 条 deferred 条目的处置**（含 CDQ-260913 剩余条件「① 契约 :4 改指（下次契约门顺带）」——T1 落笔后该条件即部分满足，条目追注与否归主线程记账纪律）不属本批。
6. **交日落复盘项**：本批与同日前批 261019-gov-review-docs-root-ledger-rehome 的引用改指漏网（T4 两处）同族——「迁移批的引用普查何以漏掉同文件两处行内链接」值得 loop-retro 时对照两批普查方法看一眼。
7. **architecture.md :4 文首裸「（契约门）」括注未对齐（R1-F4 登记）**：文首读者行「数据格式权威见 [contracts/interface-contract.md](contracts/interface-contract.md)（契约门）」——裸括注无具体门口径、现行口径下不算假话，本批不动（普查视野补记于此，2026-10-19 Read 核实原文）；后续批顺手对齐可改写为「契约变更走 /arch-change loop」或直接删括注。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-19 - architect

> 轮次：R1 全面审（四查 + 判档 + 面归属专核 + (a) 方案保真核 + 验证集可执行性 + 结构字段在场核对）。判官独立实例，未参与起草。

verdict: BLOCK(F1)

**判档结论（只升不降）**：**恒 Track B 主路径，不命中轻路径**——轻路径判据（gov-change.md「轻路径判据」，原文核）第③条命中：landing 清单含 `docs/DEV-GOVERNANCE.md`（命中任一即恒 Track B，哪怕只改一个字符）；且第①条亦不满足（`docs/architecture.md` 不在轻路径白名单 `.claude/references/**` / `.claude/skills/**` / `docs/notes/**` / `docs/templates/**` 内）。起草自评（Status 头「判档提示」）与判档一致。不升 Track A 段：方向已收敛（human 立项时已裁 (a) 方案，立项凭据③在案）、无跨系统结构性变更、riskTier low 成立（纯文档散文/死链/历史档编辑政策复原，逐 T 核实无行为语义）。

**面归属专核（§0 面归属提示，reviewer 专核义务）**：**T1–T6 零架构语义变化的主张成立**，不拆 `/arch-change`。逐条核据：T1/T2 为契约门口径与状态记账归位（§7 本就是「指针，不抄细节」节，:339 节题亲核）；T3 为元注记删除；T4 为迁址死链改指；T5/T6 为 §10 历史档编辑政策复原（决策本体零改，现状权威恒在 requirements.md，与 :397「历史档正文保持原文」自宣一致）；无小节编号变化——`node .claude/scripts/spec-anchor-check.mjs` 判官实测 `"ok": true`、`errors: []`、`archSectionCount: 47`（与 §5 第 4 条基线声明一致）；无机制/拓扑/边界/阶段语义改动。`docs/contracts/**` 零触（全 manifest 仅两文件，亲核）。

**(a) 方案保真核**：成立。T5 = 注记尾巴裸删、条目回原文（决策 12 本体「D3 格式定稿」至今为真，:410 before 逐字核）；T6 = 恢复删除线原文「BOOT 同时照常翻页」+ 单行 ⚠️ 历史口径指针，形态与同节决策 14（:412）/决策 15（:413）先例逐字同构（先例原文亲核：「⚠️ 2026-09-14 requirements v1.36：撤 P4 查字常驻页……本条为历史口径，现状见 §5.1.1（渲染节点层）」）。两处均未把历史档改写成现状档——T6 新文保留「本条为历史口径」自宣，现状外指 requirements.md §2.3（:130「本节为权威键位定义」亲核）。无越界。

**四查**：① 意图保真——human 四条原话与 T 集对应成立：点名「叠日期的不正规写法」→ T3/T5/T6；走 `/gov-change` 正规流程 → 本批载体；(a) 方案 → T5/T6 机械适用（见上）。② 纪律合规——路由表 / 写权边界 / 门语义零文本漂移改动：T1/T9 新文与 `.claude/rules/contracts.md`「无人类签字门」及 DEV-GOVERNANCE.md :88/:178 现行口径逐字一致方向；T2 与 repo-structure.md `docs/debt/contract/` 行（一条目一文件 + 生成件 INDEX、禁内嵌状态）对齐。③ 撞车复核——未发现与既有 agent 定义（`.claude/agents/` 17 份亲核 ls）、流程 SSOT、门语义的冲突；T8 新团队表行与 `uiux-author.md` def description 逐字同向（「档 2 流程 4b，生成模式」「od_* 归主线程」「dev-loop 包内不派」亲核）。④ 牵连面——引用普查亲核复跑：`git grep "](ADR.md)"`（排除 delivery/firmware）恰 :5/:397 两处；`repo-structure.net-template` 与「术语表暂缺」全仓恰 DEV-GOVERNANCE.md :3 一处；`.claude/rules/` 确无 net-template 件（ls 实测四份）；`uiux-designer` canonical 面恰 :54/:253 两处 + `_archive` 白名单两件（与 §5 第 5 条白名单吻合）；「每日一字/音量调节」在 `docs/debt/` 全域 grep 零命中亲核（exit 1）。

**findings**：

- **F1（SHOULD，阻塞冻结）· T2 新 bullet 内嵌日期戳状态快照，与自身「防再漂移」目标自相矛盾**。证据：§2 T2 改法文本「（2026-10-19 快照：域内 7 条全 deferred，含 CDQ-260913-spec-split-fallout〔spec 归档断链 + 验收 canary 基线重建挂账〕，逐条剩余条件见各条目）」——同句前半自宣「本档不再内嵌逐条状态，防内嵌清单与账本再漂移」，后半却内嵌了条目计数 + 状态断言（「7 条全 deferred」）+ 具体条目名。判官亲核 `docs/debt/contract/` 7 条目 frontmatter `status: deferred` 现核属实，但任一 CDQ 翻状态/新增条目（如「音量调节」补登记，留根 4 在案）后本行即成假——正是 human 点名要清的「叠了某某日期的不正规写法」同族，且是本批 T2 自己要根治的「内嵌清单与账本漂移」病灶复发。建议修法：删「（2026-10-19 快照：……逐条剩余条件见各条目）」整个括注，保留「状态唯一权威 = docs/debt/contract/ + 本档不再内嵌逐条状态 + 音量调节例外行」主体；快照语境已由「逐条剩余条件见各条目与生成件 INDEX」承担，无需计数。若起草方坚持保留，按 ACCEPT-AS-RISK 记账留痕（不推荐——修法成本一行）。
- **F2（NIT，不阻塞）· §0 漂移计数不自洽**：「确认 8 处漂移……architecture.md 5 处（T1–T6）」——T1–T6 为 6 个独立改点（T4 虽含 :5/:397 两行仍是一个 T），「5 处（T1–T6）」无论按 T 计（6）还是按行计（:5/:341/:343/:348-349/:397/:410/:418 ≥ 7）都对不上。建议改为实际口径（如「architecture.md 6 处（T1–T6）」+ 总数相应调整）。
- **F3（NIT，不阻塞）· 复扫完备性有一处仓内残留未记录**：「契约门在办」token 在 `docs/notes/arch-restructure-skeleton.md:27`（「纯导航指针（contracts/ 四件 + 契约门在办）」）有一处命中，§3 复扫段未记录（探针第 1 条只扫 architecture.md 本文件）。notes 非canonical、可作历史注记保留，但既然 §3 自宣「复扫模式完备性」，建议在 §3 或留根补记一笔（或探针补 `git grep -l "契约门在办"` 白名单断言）。
- **F4（NIT，不阻塞）· architecture.md :4 裸「（契约门）」括注未入普查视野**：文首分工句「数据格式权威见 [contracts/interface-contract.md]（契约门）」——§3 sweep 声明「『契约门』三字本体保留」指的是 T1 新文「契约变更」表述，未覆盖 :4 此括注。现口径下「契约门」可解作机械凭据闸（DEV-GOVERNANCE.md :178「契约机械凭据闸」为现行工程门表行），不算假话、不阻塞；但与 T1 同节语义已换轨，建议留根记一笔或后续批顺手对齐。

**验证集可执行性**：第 1 条九探针 before 态逐条亲核复跑（命中行与声明一致：契约门在办 :348/:349、human 签字 :343、](ADR.md) :5/:397、注记已消解/每日一字 :410、措辞对齐 :418、DEV-GOVERNANCE :3×2/:56、uiux-designer :54/:253）；第 2 条八断言的 -F 逐字串与新文对照可命中；第 3 条三文件存在亲核（test 通过）；第 4 条实测 `"ok": true` / `errors: []` / `archSectionCount: 47`；第 5 条白名单集合与全仓 grep 实测吻合；第 7 条 `--preflight` 旗标存在（landing-fidelity-check.mjs :50-56 亲核）。每条断言均「落地前命中、落地后翻零/翻一」可判别——无真空绿。真空绿反照：第 1 条 `grep "human 签字"` 若 T1 只删不改会假绿，但第 2 条①断言新文在场补位，双探针互锁成立。

**结构字段在场核对**：landing-manifest 围栏块在场（Status 头部下方固定位，恰两行 = manifest 两成员）；packet-id 行在场（「packet-binding（FROZEN 生效）：packet-id = 本 PROPOSAL.md §2 改法集（T1–T9）+ §5 验证集」）。两文件九 T 的目标文件集合 ≡ manifest 并集，无 scope creep。

**结论**：改法方向全部正确、九 T 改法与根因一一对应、(a) 裁定忠实、面归属主张成立；唯一须回改项 = F1（一行删改）。修订到位后 R2 聚焦复核 F1 与 F2/F3/F4 处置即可，无需全量重核。无升级 human 项（F1 为 SHOULD 级、有明确修法，非方向级分叉）。

**亲核留痕**（判官实测，2026-10-19）：Read architecture.md :1-10/:335-404/:396-430、DEV-GOVERNANCE.md :1-60/:82-96/:174-181/:248-257、gov-change.md 全文、arch-review-protocol.md 全文、uiux-author.md 头部、ADR.md :1-10、pod-coordination.md :95-110、debt/contract 条目 head 与 status 逐件；Bash 实测 = ls debt/contract（7 条目 + INDEX + _archive）、ls .claude/rules（4 份）、ls .claude/agents | wc -l（17）、git grep 五组 token、spec-anchor-check.mjs（ok:true / archSectionCount:47）、grep 出图规格单/设计 HTML in uiux-author.md（3 命中）、requirements.md :44/:68/:130/:678/:698/:741/:744/:764 与 v1.44 行（:736 音量语音调节段亲核——T2 新文引「requirements v1.44」可对上 v1.44 ② §2.1 音量语音调节段，「§5.1.1 MCP 工具面」为架构侧承载位、沿承旧 bullet 原文非新引入）、ledger-rehome 批 PROPOSAL（architecture.md 单点 :368 改指在案，佐证 T4 根因「同批已改 :373」）。

### R2 - 2026-10-19 - architect

> 轮次：R2 聚焦复核（轮次纪律：仅核 R1 findings 修订到位性 + 修订是否引入新矛盾，不做全量重核；判档/面归属/(a) 保真 R1 已裁、无重复事由不变）。

verdict: GREEN

**F1 修订到位性（R1 唯一阻塞项）**：**到位**。T2 改法新文（§2 T2 替换后 bullet）的「2026-10-19 快照：域内 7 条全 deferred（含 CDQ-260913…）」括注已整段删除；起草方自报「该字样全文件仅存 §7 R1 F1 证据引文内」经判官实测证实——`grep -n "2026-10-19 快照"` 全文件唯一命中 = :271（R1 评审记录 F1 证据引文，append-only 区、合法残留）。新 bullet 与「状态唯一权威 = docs/debt/contract/ + 本档不再内嵌逐条状态 + 音量调节例外行」主体一致，且改法说明段新增了「状态快照类日期戳一律不写进新 bullet」的修订理由句——与 R1 F1 修法方向逐点吻合。验证集兼容性复核：§5 第 2 条②断言的 -F 逐字串（「契约相关未结项的状态唯一权威 = [docs/debt/contract/](debt/contract/)」）在新 bullet 中仍完整在场，第 1 条九探针口径不受修订影响——修订未使任何探针失准。

**F1 附加判断（主线程转交起草方之问）：音量调节例外行内「（2026-10-19 grep 实测）」单日期戳——裁保留，不再剥**。裁据三条：① 性质不同——F1 打掉的是「状态快照」（会随账本翻状态/新增而静默变假的存量断言）；此日期戳是「debt 账本全域查无此条目」这一**负存在断言的举证时点**，剥掉它该断言就裸奔无据。② 该行自带终止条款（「随下次契约门处置后连本行一并删」）——过期即整行删，不存在「日期戳烂在永久正文里」的腐化路径，与 F1 病灶（无终止机制的内嵌状态）不同类。③ dated 单行指针正是 human 裁定 (a) 指定的先例形态——决策 14/15 的 ⚠️ 指针（:412-413）本身就带「2026-09-14 requirements v1.36」日期；human 点名要清的是「叠了某某日期的**不正规写法**」（叠注/删除线/矛盾双行），不是单行平直注上的事实锚日期。一行可去但不该去。

**F2 处置复核**：**到位，无新矛盾**。§0「现状差距」改为「human 勘查口径 8 处（按主题归并），落为 9 个改法条目」并写明两笔拆并映射（§10 两处注记 → T5/T6 两条；团队表/:253/:56 → T8/T9），原「architecture.md 5 处（T1–T6）」的可证伪算术矛盾消除；review profile 行（Status 头）保留的「五处（T1–T6）」在「主题 5 / 条目 6（T5、T6 同主题拆分）」新口径下自洽，与 §0 叙事一致。8 处为 human 勘查外部清单的归并口径，仓内不可证伪、不计 findings。

**F3 处置复核**：**到位**。§3 新增「仓内残留补记（R1-F3）」段，逐字引 `docs/notes/arch-restructure-skeleton.md:27`（「纯导航指针（contracts/ 四件 + 契约门在办）」）——判官 R1 已实测该行原文，补记与地面真值一致；「notes 非canonical 历史工作稿、保留不改」的处置与 R1 建议吻合，探针口径不变。

**F4 处置复核**：**到位**。新增留根 7 登记 architecture.md :4 裸「（契约门）」括注，引文与 :4 实文一致（「数据格式权威见 [contracts/interface-contract.md](contracts/interface-contract.md)（契约门）」），本批不动 + 后续批顺手对齐的处置与 R1 建议吻合。

**修订引入新矛盾扫描**：T2 改法说明句、§3 补记、留根 7 均为评审过程注记或登记性文本，未触路由表/写权边界/门语义；§0 新叙事与 Status 头 profile 行口径已对齐；landing-manifest 围栏块与 packet-id 行仍在场、内容未变（结构字段硬条款持续满足）。无新引入矛盾。

**结论**：R1 四项 findings 全部修订到位（F1 修法照建议落、F2/F3/F4 处置与建议方向一致），无升级 human 项。本 PROPOSAL 达 GREEN——主线程可凭「立项 ack + 本 GREEN」标 FROZEN（FROZEN 前按 §0 结构字段硬条款再人工复核围栏块与 packet-id 行）。R2 判官亲核留痕：`grep -n "2026-10-19 快照"`（唯一命中 :271）、R2 全文 Read 修订段（§0 :24、T2 :71/:74、§3 :222、留根 7 :249）、notes :27 复核（与 R1 实测一致）、§5 第 2 条②断言串对新 bullet 在场复核。
