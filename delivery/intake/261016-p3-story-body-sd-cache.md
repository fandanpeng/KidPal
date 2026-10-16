---
changeId: 261016-p3-story-body-sd-cache
sources: 2026-10-16 human 四轮对话原话逐字——① 「查一下 P3 story 双击没法查看是什么问题」；② 「我们现在有TF存储卡了，这个问题可以 统一考虑怎么解决了」；③ 「业务数据不落盘 是因为之前没用TF卡，ram根本存不下 无奈之举」；④ 「我觉得是不是可以到 story 列表的时候去检查它本地有没有？没有的话，这个时候就去远端 fetch。人在这个时候去选哪一个 story 会有一个操作动作嘛，等他选到这篇文章的时候，有可能这个就已经 fetch 下来了」+「进列表批量预取 + 双击兜底， 就是这个意思，批量 就拉当前页显示的，本地没有的」；⑤ 路线裁定「a」（选完整闭环，不切零契约止血小包）。关联引用 = TECH-261013-p3-reader-entry-no-text-silent-noop（debt · tech · deferred——本批消费对象）、261016-sd-card-mount（TF 卡能力就绪，本批的物理前提）、261010-p3-storylist-cold-start（先例：同族冷启动，data.restore 通道）、ADR-005 / ADR-010（其否决理由被本批推翻，见 design 稿 T1）、决策 24（介质禁令，本批 carve-out 对象）、NFR-OFF-1（离线边界，明确不动）
faceMap: 需求面动（S-F2.1 AC2「KEY 双击 → 进阅读态」不可兑现 → 兑现；补离线/降级 AC 与正文缓存来源的补充规则；requirements.md:140 P3 行、:598 NFR-OFF-1 邻位、S-F2.3）；架构面动（决策 24 介质禁令加 story 内容缓存 carve-out + 按 human 史实更正重述「哑终端纯洁性」为「当时的存储成本约束，现已由 TF 卡解除」；arch :232 存储行 / :266-267 板端状态表 / :317 离线边界节镜像改措辞；ADR-005:47 与 ADR-010:90 的「有意不变式」措辞被取代需处置）；契约面动（§四新增 kidpal_story_fetch 上行私有帧，同型既有 kidpal_story_delete〔protocol.cc:81-87〕；§1.3④ story_text 帧形状零改动、消费语义扩展）；实现面动（新增 /sdcard story 正文缓存层 + 预取器两部件；mcp_server.cc 消费侧 / story_page.c id 校验 / story_page.h 文案单源 / boards 侧 SD 消费 / front_app.py 收帧回帧）；governance 不动
facePlan: **`/triage-change` = 承载整条跨面链的编排会话（容器，非链上的一环）**——它四面探测并落本 spine（step 1），**在本会话内**按下列顺序推进各面（step 6 逐字：「轻面注明内联执行（本会话继续派发），重面给出 headless 拉起命令」），最后做自己的实现面五档分档并落 plan.md。**注意：triage 不是"第三个面"**——它是把需求、架构、开发合在一起的那一层；「实现面留本门分档」指的是分档是它自己的收尾动作，不是它等于实现面。面推进顺序：需求面先行（/requirement-change loop·**档 2 结构性**〔主线程 2026-10-16 判：AC 增删改 + 补充规则 = 档 1 语义修改，但 OQ-5 裁「调整键帽条提示」命中触 UI，按升档规则自然落档 2；升档须 human 确认，随立项 ack 一并〕：story-author 起草 P3 正文缓存措辞 + S-F2.1 AC2 兑现 + 离线降级 AC；档 2 三件套 = story-author 牵连面清单 → architect 可行性三件（必派）→ uiux-author 生成模式出图 → requirement-reviewer 四查 + 触 UI 第五查 → human 终拍看图）；其次 架构 + 契约面同批（/arch-change loop：决策 24 carve-out + 史实重述 + 契约包三节〔kidpal_story_fetch 帧定义 + story_text 消费语义〕随批原子落，无人签门、过机械凭据闸）；末了 实现面分档（**本门收尾动作，非"第三个面"**：五档 → 预期 ③拆包 或 ②单包 Full，承载包 = 缓存层 + 预取器 + 双击兜底 + id 修复，规模待 architect 比例化）；launch 前置 = 上游各面 anchors 回填（spine 源条款：先于实现面的 anchors 未回填 = 上游未冻结 → 承载包不得 launch）。设计输入 = delivery/intake/261016-p3-story-body-sd-cache.design.md（companion 稿，免冻、不进任何门）
anchors: {req: "（待回填）", arch: "（待回填）", contract: "（待回填）", imp: "（待回填）"}
openQ: OQ-1 零契约止血小包是否先切——human 2026-10-16 选 (a) 完整闭环，**未取**（留痕，不另立）。OQ-2 kidpal_story_fetch 载荷形态——**human 2026-10-16 裁：ids 批量**。OQ-3 隐私面记账落点——**human 2026-10-16 裁：本批只落盘、不加防护；将来给页面记录加口令 + 存储加混淆，先记 tech item 后处理**（已登记 docs/debt/tech/TECH-261016-sd-story-confidentiality.md，status: deferred）。OQ-4 缓存文件命名路线——**human 2026-10-16 裁：开 LFN，用 `<id>.txt`**（architect 仍须评估 sdkconfig 内存/codepage 代价，若有硬阻即刻回问，不自行改判）。OQ-5 是否触 UI——**human 2026-10-16 裁：调整**（键帽条「说话·双击看」提示措辞随语义改）→ 触发 requirement-change **升档 2** + 触 UI 批次扩五查 + uiux-author 生成模式出图。OQ-6 id 错配缺陷补登 debt 形态——**human 2026-10-16 同意**：已 append 进 docs/debt/tech/TECH-261013-p3-reader-entry-no-text-silent-noop.md 正文（同族同条目，随本批消费结项）。
status: in-flight
---

# spine · P3 story 正文本地缓存（261016-p3-story-body-sd-cache）

> 触面 ≥2 跨面变更的路由账本（[change-intake.md](../../.claude/references/change-intake.md) 程序；spine 免冻——各面冻结语义由各面制品承载，anchors 记录冻结事实）。设计输入 = [261016-p3-story-body-sd-cache.design.md](261016-p3-story-body-sd-cache.design.md)。

## 建档前查重（change-intake.md §3）

- **既有 spine 排查**：`delivery/intake/` 在册 8 份（26 份含 INDEX），逐份按意图与触面比对——最接近为 `261010-p3-storylist-cold-start.md`（同 P3 面、同「冷启动后数据不可达」族），但其对象是 **story_list 列表冷启动**、载体是 `data.restore` 标记、且已 `status: closed`；本次对象是 **story 正文**、载体是 TF 卡本地缓存 + 新增上行帧。**不同意图，不重复建档**。
- **debt 三域排查**：命中 `docs/debt/tech/TECH-261013-p3-reader-entry-no-text-silent-noop.md`（`status: deferred`）——**正是本批要消费的条目**，故不另立新条目，按其走 §3a 晋升路径：`sources.refs` 引其 id（在册），立项收口时翻 `closed` + 正文结项行带 `consumed-by: 261016-p3-story-body-sd-cache`。

## 立项 ack（change-intake.md §4 · 唯一必停人类触点）

四面对齐呈 human（「为什么改 / 往哪改 / 改动多大」）→ **待 ack**。升级判据随 ack 一并裁定路由：本变更**触契约面**（新增 §四私有上行帧），按 §8「升级判据 ≠ 触契约处置」——升级判据是路由门、触契约是内容处置，两者独立可达；契约内容走 `/arch-change` 无人签门通道。

## 触面事实（查出来的，非申报）

| 面 | 动不动 | 动哪 | 依据 |
|---|---|---|---|
| 需求 | ✅ | S-F2.1 AC2 兑现 + 离线/降级 AC + 补充规则；S-F2.3 阅读态可达性 | requirements.md:140（P3 行）/ :598（NFR-OFF-1 邻位）/ :163（键位对账③「双击接线已移除」需随批复查是否 stale） |
| 架构 | ✅ | 决策 24 介质禁令 carve-out + 史实重述；§5.4 板端状态表 / §5.3.1 存储行镜像 | architecture.md:101（五条禁令）/ :232（存储行）/ :266-267（板端状态表）/ :317（离线边界节）；archive/spec.md 决策 24 条文 |
| 契约 | ✅ | §四新增 `kidpal_story_fetch`；§1.3④ `story_text` 消费语义注记 | protocol.cc:81-87（同型先例 `kidpal_story_delete`）/ front_app.py:256 / mcp_server.cc:1154 |
| 实现 | ✅ | 新增缓存层 + 预取器；mcp_server.cc / story_page.c/.h / boards / front_app.py | mcp_server.cc:305-317、:1148-1151；story_page.c:1467-1475、:1707-1732、:1216-1237；waveshare-s3-rlcd-4.2.cc:747-754、:832-875 |
| governance | ❌ | 零命中 | 不触 `.claude/**`、不触 gate 登记 |

## 关联不动的面（本批显式排除，防 scope 蔓延）

- **NFR-OFF-1 / S-F1.4 离线边界**：「断网无提醒」**不变**——提醒 / 课表 / 日志一律不落卡，本批只给 story 正文开窄例外。
- **ADR-005 / ADR-010 的 `data.restore` 补丁**：与之并存，本批不摘除（摘除 = 另立变更，双机制债已记 design 稿 §5）。
- **光标预取、缓存淘汰、热插拔检测、离线产出**：均列 design 稿「非目标」。

## 档位裁定与编排（主线程 2026-10-16）

- **编排形态 = `/triage-change` 是承载整条跨面链的会话（容器）**。命令定义逐字：「你现在是 **主线程编排者**（人类主线程）」。它 step 1 四面探测并落本 spine，随后**在本会话内**按 facePlan 顺序推进各面（step 6 逐字：「轻面注明**内联执行（本会话继续派发）**，重面给出 headless 拉起命令」），最后做自己的实现面五档分档。**triage 不等于「实现面那一环」**——260928 那批的触发原话即「现在 triage 只关注到实现侧的拆包大小，**这个不够**」；改造后它把需求、架构、开发合在一起编。
- **分面路由去向**：需求面 → `/requirement-change`；架构/契约面 → `/arch-change`；治理面 → `/gov-change`；**实现面分档 = 本门收尾动作** → `/build-feature`。四面可同时存在于一个变更，各面门照旧各归各裁（统一 intake ≠ 统一门）。
- **面推进顺序由 facePlan 定，且不给 triage 本身编号**：本变更 = 需求面（先行）→ 架构 + 契约面 → 实现面分档。
- **起草期更正留痕（同一件事，错了两次）**：① 主线程曾答「不是 triage 先行、是 requirement-change 先行」——**错**，把「面执行顺序」与「编排入口」混为一谈；② 更正后又称 triage 为「③ 实现面」——**仍错**，把容器当成了链上的一环。两次均由 human 2026-10-16 当场指出。根因 = AGENTS.md 摘要行「实现面变更 → `/triage-change` 五档分流」与 change-triage.md §0「实现面**留本门**分档」都把 triage 描述成实现面那一环，而点明它是容器的 step 6「**本会话继续派发**」未被读出。已记 `docs/debt/gov/ARQ-261016-intake-entry-identity-unstated.md`（建议改动项应据此加固）。
- **本 spine 与该入口的关系**：本文件即该入口 step 1 的产物（主线程在起草会话中人工前置完成）——下一步应以 `/triage-change 261016-p3-story-body-sd-cache` 正式续跑，其 step 1 按 change-intake.md §3 建档前查重会命中本 spine 并**消费既有件、不重复建档**，随后按 facePlan 分面推进。
- **需求面档位 = 档 2 结构性**。判据链：AC 增删改（S-F2.1 AC2 兑现 + 新增离线/降级 AC）+ 补充规则口径 = 档 1 语义修改；但 OQ-5 裁「调整键帽条提示」命中**触 UI**（改交互、需出图），按 [requirement-change.md](../../.claude/references/requirement-change.md) 升档规则自然落**档 2**。升档裁定归主线程提呈、human 确认。
