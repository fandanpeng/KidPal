# PROPOSAL · 261017-arch-review-p3-story-body-sd-cache

> **状态：FROZEN v1.0 · 2026-10-17 · 主线程凭据自冻**——human 立项 ack（2026-10-17 本会话，human 原话「立项」）+ architect-reviewer **R2 GREEN**（判档：主路径；契约四分类：`ACCOMPANIES_SPEC_CHANGE`）；R1 BLOCK(F1,F2) → 修订 → R2 靶向复核 GREEN + 三条 FROZEN 前 token 收口。（本面 = 架构 + 契约面，触契约分支；FROZEN 须含 human 立项 ack + reviewer GREEN + 判档结论。）
> **承载 ARQ**：无独立 ARQ（来源 = 跨面 intake spine `delivery/intake/261016-p3-story-body-sd-cache.md`；消费 debt `TECH-261013-p3-reader-entry-no-text-silent-noop`）。上游需求面 = requirements v1.64（已 frozen）。
> **review 轨**：Track B **主路径**（非轻路径——触 `docs/contracts/**` + 改 §2.4 原则 / §10 决策 24 + 新增 ADR + 首个请求-应答环 = 非零语义、触承重节）。SSOT：`.claude/references/arch-change.md`。
> **packet-binding（FROZEN 生效）**：packet-id = 本 PROPOSAL §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触契约面 + 跨 ADR-005「无 device RPC 先例」边界 + 决策 24 史实重述；但边界窄、只读镜像、板端零写入面，不触云端 producer 单源）。
> **review profile**：架构面 + 契约面（触契约分支，显式标注）；G1/G2/G3 治理面 = 零（不触 `.claude/**`）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是结构字段，GREEN 结论前必须在场；判官随 GREEN 结论核对、主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/architecture.md
docs/ADR.md
docs/contracts/interface-contract.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：孩子在 P3 星星记录本里选中任意一条记录双击，都能进阅读态看到正文——本机录的、重启前录的、云端恢复回来的老故事一视同仁；断网时看过的那几篇仍然能看。板端 TF 卡（`/sdcard`）成为 story 正文的本地缓存介质，云端 story 归档仍是唯一真相源，板端只读镜像、一切写入照旧走云。

**现状差距**（需求面 v1.64 已冻结的判据，本面不重述产品侧，只给架构侧缺口）：
1. 决策 24（architecture.md §10 :417）介质禁令禁止一切业务数据落非易失介质（含 SD 卡），且把理由写成「『哑终端不存数据』纯洁性保持」；ADR-010 备选③更把「板端 RAM 缓存重启即失」写成「有意不变式」。这些句子与需求 v1.64 的「story 正文只读镜像缓存」直接冲突。
2. 契约 §四私有消息族只有单向帧（`kidpal_story_delete` / `kidpal_session_ctx`），无「设备发起 request → 网关 response」的请求-应答环；设备侧无取正文的通路。
3. 架构面除已随需求镜像的三处（§5.3.1 存储行 / §5.4 数据视图 / §6.2 离线边界）外，还有四处「禁令重述点」仍是未收窄原文。

**非目标（本批不做）**：提醒 / 课表 / 日志一律不落卡（NFR-OFF-1「断网无提醒」语义不变）；不做光标预取 / 缓存淘汰 / 热插拔检测 / 离线产出；不摘除 ADR-005/010 已落地的 `data.restore` 补丁（并存，摘除另立变更）；不夹带 `session_id` 契约显示漂移勘误（留将来契约门，见 §6）。

---

## 2. 问题、根因与改法（T1…T6 + 契约包）

### T1 · 决策 24 介质禁令开 story 正文窄例外 + 史实重述

**问题**：决策 24 的介质禁令原文禁止一切业务数据落非易失介质（含 SD 卡），并把理由写成「『哑终端不存数据』纯洁性保持」；ADR-010 备选③把「板端 RAM 缓存重启即失」写成「有意不变式」。这与需求 v1.64 的 story 正文只读镜像缓存直接冲突——reviewer 四查读的正是这些句子，不显式改写会被照旧拿「违反决策 24」打回。

**证据**：
- architecture.md:417 决策 24 原文：「离线边界（2026-09-02 grilling 定）：设备不缓存事项/课表——断网无提醒……「哑终端不存数据」纯洁性保持，离线兜底验收撤回。介质禁令（V4 定）：板端非易失介质（16MB Flash / SD / NVS）仅允许设备凭证与服务器地址（url/token/version）、预栅格化 1bpp 美术资产与字体、固件本体；一切业务/身份/事项状态……只能驻留 RAM、掉电即失、禁落任何非易失介质。」
- architecture.md:101 §2.4 原则 5：「介质禁令：板端非易失介质仅存设备凭证、预栅格化资产与字体、固件本体；业务/身份/事项状态只驻 RAM（决策 24）。」
- ADR.md:90 ADR-010 备选③：「NVS 持久化列表缓存——放弃：违反决策 24（板端 RAM 缓存重启即失是有意不变式），不进入备选。」
- spine sources human 2026-10-16 原话逐字：「业务数据不落盘 是因为之前没用TF卡，ram根本存不下 无奈之举」。

**根因**：介质禁令的原始动因是**当时的存储成本约束**（未用 TF 卡、RAM 存不下业务数据），并非产品原则；但账面被升格为「哑终端纯洁性」「有意不变式」。TF 卡能力已于 2026-10-16 就绪（`261016-sd-card-mount`，SDMMC 1-bit 挂载 `/sdcard`，无卡/坏卡降级不崩），该约束被解除。

**改法**：
- §10 决策 24（:417）原文不动，句尾 append 一段「史实更正 + 窄例外」注记（同决策 19「出网路径④…原文①–③不动」、决策 21「2026-09-14 查证 + 配置级修正」的补注先例）：把「纯洁性保持」降回「存储成本约束」、白名单增列「story 正文只读镜像」、边界不变（提醒/课表/日志仍不落板、断网无提醒不变）。逐字插入段见下方围栏。
- §2.4 原则 5（:101）句尾 append 窄例外（只给 story 正文、不扩及任意业务数据）。逐字插入段见下方围栏。

```text
【插入 1 · §10 决策 24 条目句尾 append】

**史实更正 + 窄例外（`261016-p3-story-body-sd-cache`，2026-10-17）**：「『哑终端不存数据』纯洁性保持」句的历史更正——介质禁令的原始动因是**当时的存储成本约束**（未用 TF 卡、RAM 存不下业务数据），并非产品原则；human 2026-10-16 原话逐字「业务数据不落盘 是因为之前没用TF卡，ram根本存不下 无奈之举」。TF 卡能力 2026-10-16 就绪（`261016-sd-card-mount`）后该约束解除：介质禁令**白名单增列「story 正文只读镜像」**（云端 story 归档仍为唯一真相源，板端不承载任何写入；断网时已留存正文照常回看）。**边界不变**：提醒 / 课表 / 日志等其余业务 / 身份 / 事项状态仍只驻 RAM、禁落非易失介质；「离线边界（断网无提醒）」部分语义不变。

【插入 2 · §2.4 原则 5 句尾 append】

**窄例外（`261016-p3-story-body-sd-cache`，2026-10-17）**：SD 卡可为 **story 正文**留**只读镜像**（云端 story 归档仍为唯一真相源、板端不承载任何写入，见决策 24 注记 / §6.2）；提醒 / 课表 / 日志等其余业务数据仍不落板——此例外**只给 story 正文**、不扩及任意业务数据。
```

目标文件：`docs/architecture.md`（改）

### T2 · 禁令其余重述点镜像（第二梯队，防牵连面否回）

**问题**：除原则 5 与决策 24 外，architecture.md 还有四处「禁令重述点」仍是未收窄原文，与 carve-out 有文本张力——留原文必触发 reviewer 四查④牵连面否回。

**证据**：
- architecture.md:62 §2.1 顶层定位：「设备只负责收音/放音/显示/按键，不理解业务、不存数据、不做智能；所有数据、逻辑、记忆在云端。」
- architecture.md:97 §2.4 原则 1：「哑终端：设备零业务、零存储、零智能（决策 1/24）；」
- architecture.md:115 §3.1 离线边界：「离线边界：断网仅本地时间/温湿度/电量，不缓存任何事项（NFR-OFF-1）；」
- architecture.md:150 §4 NFR-OFF-1 映射行：「NFR-OFF-1 离线边界 | 设备不缓存事项/课表 + 介质禁令（白名单制）+ 断网仅本地时间/温湿度/电量 +RAM 渲染缓冲不禁（掉电即失，§6.2 澄清） | §6.2 / §5.4」

**根因**：需求面批只镜像了三处（§5.3.1 存储行 / §5.4 数据视图 / §6.2 离线边界），但「禁令重述点」不止这三处；上列四处是对「不存数据 / 零存储 / 不缓存事项」的概括复述，未随 carve-out 收窄。

**改法**：四处轻改，措辞从「零存储 / 不缓存任何事项」收窄为「零真相源存储 / 窄例外」；逐字替换对见下方围栏（每对 = 原文句 → 替换后句）。

```text
【替换 1 · §2.1 :62】
原：设备只负责收音/放音/显示/按键，不理解业务、不存数据、不做智能；所有数据、逻辑、记忆在云端。
新：设备只负责收音/放音/显示/按键，不理解业务、不做智能、不承载业务真相源（story 正文仅作板端只读镜像缓存，见 §6.2）；所有数据、逻辑、记忆在云端。

【替换 2 · §2.4 原则 1 :97】
原：哑终端：设备零业务、零存储、零智能（决策 1/24）；
新：哑终端：设备零业务、零智能、业务数据零真相源存储（story 正文只读镜像为窄例外，见原则 5）；

【替换 3 · §3.1 :115】
原：离线边界：断网仅本地时间/温湿度/电量，不缓存任何事项（NFR-OFF-1）；
新：离线边界：断网仅本地时间/温湿度/电量，不缓存任何事项（story 正文只读镜像为窄例外，NFR-OFF-1 注记）；

【替换 4 · §4 :150 映射行对策格】
原：设备不缓存事项/课表 + 介质禁令（白名单制）+ 断网仅本地时间/温湿度/电量 +RAM 渲染缓冲不禁（掉电即失，§6.2 澄清）
新：设备不缓存事项/课表 + 介质禁令（白名单制）+ 断网仅本地时间/温湿度/电量 +RAM 渲染缓冲不禁（掉电即失，§6.2 澄清；story 正文只读镜像窄例外，§6.2）
```

目标文件：`docs/architecture.md`（改）

### T3 · ADR-005/010 否决理由处置 + 新增 ADR-011

**问题**：ADR-005 备选①、ADR-010 备选②③ 的否决理由以「device RPC 首例」「有意不变式」为由否决了持久化 / 按需拉取方案；本批已推翻这些否决理由，需记账处置，否则 reviewer 撞车复核（四查③）会照旧拿它们打回。

**证据**：
- ADR.md:47 ADR-005 备选①：「设备按需拉取……放弃：需在 §四端云协议剖面新增首例「设备发起请求-应答」消息对（现无 device RPC 先例、kidpal_session_ctx 是单向通知）」
- ADR.md:90 ADR-010 备选②：「设备按需拉取（device RPC）——放弃：ADR-005 :47 备选①同型否决（需在 §四新增首例设备发起请求-应答消息对……）」
- ADR.md:90 ADR-010 备选③：「NVS 持久化列表缓存——放弃：违反决策 24（板端 RAM 缓存重启即失是有意不变式），不进入备选。」

**根因**：这些否决理由在裁决当时成立（成本判断正确），但「有意不变式」实为「存储成本约束」的误升华；本批为 story 正文**有意支付**「device RPC 首例」成本、解除介质禁令对 story 正文的约束，故否决理由被取代。注意：被取代的是**备选否决理由**，不是 ADR-005/010 的**决策本体**（`data.restore` additive 字段仍在役）。

**改法**：新增 ADR-011（append 到 docs/ADR.md 尾），**不改 ADR-005/010 原文**——ADR.md 头部 append-only 纪律逐字「append-only：不覆盖、不改写既有条目；推翻旧决策 = 新增一条并注明 supersedes ADR-XXX」。ADR-011 逐字草案见 §6「ADR-011 落地附件」——草案含 `## ADR-011` 标题行，置于 §2 之外以规避节界检测对围栏内 `##` 行的误截断（解析器节界检测不跳围栏）。

目标文件：`docs/ADR.md`（改）

### T4 · 契约面 §四新增 kidpal_story_fetch 上行私有帧

**问题**：设备侧无取 story 正文的通路；需求 v1.64 要求「双击即开 + 断网回看」，需新增设备发起的取正文请求。

**证据**：
- interface-contract.md:271 `kidpal_story_delete` 定义（同型先例，单向通知、无应答）。
- firmware/main/protocols/protocol.cc:82-88 `SendKidpalStoryDelete` 逐字形态：`{"session_id":"...","type":"kidpal_story_delete","id":"<story-id>"}`。
- spine OQ-2 human 2026-10-16 裁：载荷 = ids 批量（触发场景天然「当前屏这一批」）。
- spine OQ-9 起草期纠偏：`kidpal_story_delete` 是「设备→网关**方向**」先例（单向），**不是**「请求-应答环」先例；「请求-应答环」本批之前不存在，`kidpal_story_fetch` + `story_text` 应答是**首个**。

**根因**：契约 §四私有消息族仅有单向帧（delete / session_ctx），无「设备发起 request → 网关 response」的请求-应答环；story 正文取回需要此环。

**改法**：§四新增 `kidpal_story_fetch` 上行私有帧（插在 `kidpal_story_delete` 定义与 `mcp` 工具调用之间），逐字仿 delete 形态（session_id 同源、ids 数组载荷），显式声明其为首个请求-应答环、首次跨过 ADR-005 边界；应答复用既有 story_text 帧、下行零新形状。逐字定义见下方围栏。

```text
【插入 · §四新增帧（interface-contract.md :271 与 :272 之间）】

- **`kidpal_story_fetch` 消息**（端→网关，KidPal 私有扩展消息；2026-10-17 新增，`261016-p3-story-body-sd-cache`）：设备请求取回 story 正文时发送 `{"session_id":"...","type":"kidpal_story_fetch","ids":["<story-id>", ...]}`——`ids` = story_list items 下发过的 id 数组（闭集来源，设备不造 id）；触发 = 进 P3 列表 / 翻屏时的当屏批量预取 + 双击兜底插队单篇（human 2026-10-16 裁载荷 = ids 批量，触发场景天然「当前屏这一批」，一帧一往返优于逐篇发帧）。**网关消费并拦截、不下透引擎**：front 机械执行（逐 id 校验存在于 story 账本〔哨兵头 + `<stories_dir>/<id>.md` 存在〕→ 读 MD → 以既有 **`story_text` 帧（§1.3④）逐篇应答**，每篇一次）；非法 / 不存在 id 拒绝 + 审计留痕、不回帧；空 ids 数组 = no-op。**这是契约面首个「设备发起 request → 网关 response」的请求-应答环**——此前 §四私有消息（`kidpal_story_delete` / `kidpal_session_ctx`）均单向通知、无应答（ADR-005 边界保持）；本帧 + `story_text` 应答**首次跨过 ADR-005 立下的「现无 device RPC 先例」边界**（据实裁定、不沿用「delete 是首例请求-应答」类不精确引据，见 spine OQ-9 / ADR-011）。下行**零新形状**——应答复用既有 `story_text` 帧。省略整条消息 = 不取回，向后兼容。
```

目标文件：`docs/contracts/interface-contract.md`（改）

### T5 · 契约面 §1.3④ story_text 消费语义注记扩展

**问题**：story_text 帧此前仅云端主动推送（归档后 / 恢复推送），本批它第一次被「设备主动要」触发（作为 fetch 应答），需补消费语义注记。

**证据**：interface-contract.md:115-130 §1.3④ story_text 现状形状 `view:"story_text"` + `data:{id,title,text,seq}`；板子行为句含「fresh_save 场景触发盖章演出」。

**根因**：帧形状零改动，但消费触发面新增一种（fetch 应答），且盖章演出语义须与 fresh_save 路径分叉——fetch 应答不触发盖章。

**改法**：§1.3④ 在 `seq` bullet 与既有契约变更记录之间 append 一条消费语义注记（形状零改动 + 盖章不随 fetch 触发）。逐字见下方围栏。

```text
【插入 · §1.3④ 消费语义注记（interface-contract.md :129 与 :130 之间）】

- **消费语义扩展（2026-10-17，`261016-p3-story-body-sd-cache`）**：本帧新增第二个消费触发——作为 `kidpal_story_fetch`（§四）的**应答帧**逐篇下发（设备主动索取、网关按 id 回）；**帧形状零改动**（仍 `view:"story_text"` + `data:{id,title,text,seq}`）。板端消费：fetch 应答帧入板后写 SD 本地副本（§6.2 只读镜像；**卡不在位 / 坏卡时不留存、当场显示**）+ 若当前正等待该篇则进阅读态；**盖章演出不随 fetch 应答触发**（盖章只随 `fresh_save` 归档路径触发）。这是本帧第一次被「设备主动要」触发——此前仅云端主动推送（归档成功后 / WS 建连恢复）。
```

目标文件：`docs/contracts/interface-contract.md`（改）

### T6 · architecture.md §12 变更记录 changeId 行 append（定稿标记）

**问题**：arch 面语义性改动（决策 24 carve-out）的定稿标记 = architecture.md §12「变更记录」changeId 行（arch-change.md:37）；不落则该行不在场、下游实现面承载包 launch 前置「arch 已冻结」不满足（arch-change.md:38）。

**证据**：architecture.md:433 §12 头部「本文件每次改动都在下表追加一行（新行在上，倒序）——changeId、日期、改了哪些节、依据」；arch-change.md:37-38（changeId 行 = 定稿标记 + launchPreconditions 消费判据）；先例 = architecture.md:437 的 `261015-gov-review-changes-taxonomy` 行。

**根因**：packet-id = §2 改法集 + §5 验证集；不显式列 §12 append 动作，packet-bound landing 不会落它，§12 顶层仍停在 261015-gov-review-changes-taxonomy。

**改法**：§12 变更记录表顶部（261015-gov-review-changes-taxonomy 行之上）追加一行。changeId 取 **spine changeId `261016-p3-story-body-sd-cache`**（本批为跨面 intake，下游实现面承载包按 spine 锚「arch 已冻结」、非 arch-review 目录 token）；改动面涵盖本批改动（§2.1 / §2.4 原则 1·5 / §3.1 / §4 / §10 决策 24）与需求批已写但未 §12 记账的三处镜像（§5.3.1 / §5.4 / §6.2）。依据列**不含 FROZEN 版本 token**（避免「FROZEN vX.Y」字面落进 canonical §12 留模板 token、与 landed 对账断）。逐字见下方围栏。

```text
【插入 · §12 变更记录表首行（新行在上，倒序）】

| 261016-p3-story-body-sd-cache | 2026-10-17 | §2.1 / §2.4 原则 1·5 / §3.1 / §4 禁令重述点收窄 + §10 决策 24 carve-out 史实更正注记（「哑终端纯洁性」降回「存储成本约束」）+ §5.3.1 / §5.4 / §6.2 需求批镜像 | delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md（契约面同批原子落） |
```

目标文件：`docs/architecture.md`（改）

### 契约包 · 端云协议

**proposedContractDiff**（逐面最小 diff）：
- **shimApi / §四端云协议剖面**：新增上行私有帧 `kidpal_story_fetch`（`{"session_id":"...","type":"kidpal_story_fetch","ids":["<story-id>", ...]}`，逐字仿 `kidpal_story_delete` 形态、ids 批量载荷），声明为契约面首个「设备发起 request → 网关 response」请求-应答环、首次跨过 ADR-005「现无 device RPC 先例」边界；应答复用既有 `story_text` 帧、下行零新形状。逐字定义见 §2 T4。
- **displayBlock / §1.3④ story_text**：消费语义注记扩展（形状零改动、fetch 应答不触发盖章、卡不在位/坏卡时不留存当场显示）。逐字见 §2 T5。

**impact**：固件（`mcp_server.cc` / `story_page.c` / boards，归实现面承载包）、front（`front_app.py` 收帧 → 校验 → 逐篇回 `story_text`，归实现面承载包）、引擎 / OpenClaw 零影响（fetch 拦截不下透、`story_text` 复用不改形状）；ADR-005 边界本批首次跨过（首个请求-应答环），已 ADR-011 记账；不破坏已跑通实测链路（`kidpal_story_delete` / `kidpal_session_ctx` 单向语义不变）。详见 §3 sweep 表。

**validationPlan**：逐字 diff 对账（T4/T5 插入段 byte 对账）+ 边界守持核 + `spec-anchor-check.mjs` + `pre-merge-escalation-gate.mjs` 机械凭据闸三件联立。详见 §5。

---

## 3. 影响面 sweep 分类

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 架构 canonical | `docs/architecture.md` | 改 | 决策 24 carve-out + 史实重述（T1）+ 四处镜像句（T2）；§5.3.1/§5.4/§6.2 已随需求镜像改、本批不重复 |
| ADR | `docs/ADR.md` | 改 | 新增 ADR-011（T3）；ADR-005/010 原文不动 |
| 契约 canonical | `docs/contracts/interface-contract.md` | 改 | 新增 kidpal_story_fetch（T4）+ story_text 消费语义（T5） |
| 契约消费·固件 | `firmware/main/mcp_server.cc` / `story_page.c` / `story_page.h` / `boards/**` / `protocol.cc` | 未触（本批） | 消费侧与接线归实现面承载包（五档分档，非本面落地） |
| 契约消费·front | `server/gateway/front/front_app.py` | 未触（本批） | 收帧 → 逐 id 校验 → 逐篇回 story_text，归实现面承载包 |
| 契约消费·引擎/OpenClaw | — | 未触 | fetch 网关拦截不下透（同 delete/session_ctx 先例）；story_text 复用不改形状，引擎侧 story_text 已被 relay 收紧拒绝（§1.5 第 3 条 Q7 单源） |
| ADR-005 边界 | — | 跨边界 | 本批首次跨过「无 device RPC 先例」边界（首个请求-应答环），ADDR-011 记账 + 四分类据实裁定 |

> sweep 说明：本面落地只改三个 canonical 文件（见 landing-manifest）；实现面（固件 / front）的消费与接线归 dev-loop 承载包，不属本面落地范围。契约单源纪律不变——fetch 应答同属 front 机械推送单源，不下透引擎。

---

## 4. human 取舍点（OQ）

本批 human 取舍点已全部在 spine `openQ` 裁定（OQ-1~9），本面不重提，只列与本面直接相关的已裁项与剩余建议项：

- **OQ-2（已裁）**：`kidpal_story_fetch` 载荷 = ids 批量（非单 id）——T4 已落。
- **OQ-9（已裁，起草期纠偏）**：`kidpal_story_delete` 是「设备→网关方向」先例（单向），非「请求-应答环」先例；本批 `kidpal_story_fetch` + `story_text` 应答是首个请求-应答环、首次跨过 ADR-005 边界——T4 已据实落，不沿用不精确引据。
- **A5 并入本批（主线程 2026-10-17 裁）**：§2.1 / §2.4 原则 1 / §3.1 / §4 四处镜像句并入本批，landing-manifest 覆盖（T2）。
- **D3 不夹带（主线程 2026-10-17 裁）**：`session_id` 契约显示漂移不夹带本批，留将来契约门（§6 留根）。
- **无新增 human 取舍点**；下列为 architect 建议 + reviewer 裁定项：判档（建议主路径，reviewer 只升不降）、ADR 处置方式（建议新增承接、不改原文）、四分类（建议 `ACCOMPANIES_SPEC_CHANGE`，reviewer 裁定）。

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **锚完整性**：`node .claude/scripts/spec-anchor-check.mjs` → 悬空锚归零（architecture.md 改动后 + requirements.md 既有锚）。
2. **PROPOSAL 结构门（pre-commit 干跑）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md` → anchorUnion 成员 = {docs/architecture.md, docs/ADR.md, docs/contracts/interface-contract.md}，与 landing-manifest 并集一致、无夹带路径 token。
3. **契约机械凭据闸（post-commit，合并前）**：`pre-merge-escalation-gate.mjs` 对 `docs/contracts/interface-contract.md` diff 三件联立（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖契约路径）→ 缺一 BLOCK。
4. **逐字 diff 对账（人读，post-commit）**：landing-verifier 逐文件 diff 核对实际落地 == 本 PROPOSAL §2 各 T 的插入/替换段（承重引文 byte 对账）——T1 两段 insert、T2 四段 replace、T3 一段 ADR-011（§6 附件）、T4 一段帧定义、T5 一段注记、T6 §12 changeId 行 append。
5. **边界守持核（人读）**：carve-out 措辞只给 story 正文、显式排除任意业务数据；提醒/课表/日志不落卡；断网无提醒（NFR-OFF-1）语义不变；首个请求-应答环声明在场、不沿用不精确引据。
6. **ADR-011 记账在场（人读）**：supersedes 注记指 ADR-005 备选① / ADR-010 备选②③，且注明「决策本体仍在役」。

---

## 6. 留根清单

- **OQ-3 隐私面（deferred）**：story 正文落可拔卡 = 数据落点扩张；本批只落盘不加防护，口令 + 存储混淆记 `docs/debt/tech/TECH-261016-sd-story-confidentiality.md`（status: deferred），将来处理。
- **双机制债**：`data.restore` 补丁（ADR-005/010）与新缓存层并存，摘除另立变更评估（非目标）。
- **D3 `session_id` 契约显示漂移**：契约 §四 delete/session_ctx 显示省略 `session_id`，而 `protocol.cc:86-98` 实现含 `session_id`；本批 fetch 帧按实现写含 `session_id`（与代码一致），是否顺带补 delete/session_ctx 的契约显示 = 留将来契约门勘误，不夹带本批。
- **OQ-4 LFN 代价评估（实现面）**：缓存文件命名走 LFN（`<id>.txt`），sdkconfig 内存 / codepage 代价归实现面承载包的 architect 技术预评（若有硬阻即刻回问，不自行改判）。

**ADR-011 落地附件**（packet-bound landing 逐字 append 到 `docs/ADR.md` 尾）：

```text
## ADR-011 · story 正文板端只读镜像缓存 = 决策 24 介质禁令窄例外 + 契约首个设备发起请求-应答环（kidpal_story_fetch + 复用 story_text 应答）

- **日期**：2026-10-17（261016-p3-story-body-sd-cache 架构+契约面）
- **决策**：① 决策 24 介质禁令开**窄例外**——SD 卡可为 **story 正文**留**只读镜像**（云端 story 归档仍为唯一真相源、板端不承载任何写入、断网时已留存正文照常回看）；提醒 / 课表 / 日志等其余业务数据仍不落板、断网无提醒语义不变。② 契约 §四新增上行私有帧 `kidpal_story_fetch`（`{"session_id","type","ids":[...]}`，ids 批量 = 当屏预取 + 双击兜底），应答复用既有 `story_text` 帧、下行零新形状——这是契约面**首个「设备发起 request → 网关 response」的请求-应答环**，首次跨过 ADR-005 立下的「现无 device RPC 先例」边界。③ **边界守持**：板端仍**只读镜像**（ADR-004 云端唯一 producer 纪律零变化、一切写入仍走云端）；读回只认云端 story_list 存在的篇目（卡可拔可换、卡上数据不可信）；不触 ADR-005/010 已落地的 `data.restore` 补丁（并存、摘除另立变更）；隐私面数据落点扩张（正文落可拔卡）已记 TECH-261016-sd-story-confidentiality（deferred）。
- **理由**：介质禁令原始动因 = **当时的存储成本约束**（未用 TF 卡、RAM 存不下业务数据），非产品原则（human 2026-10-16 原话逐字「业务数据不落盘 是因为之前没用TF卡，ram根本存不下 无奈之举」）；TF 卡能力 2026-10-16 就绪（261016-sd-card-mount）后约束解除。story 正文与提醒 / 课表不同——正文体积大、且「双击必开 + 断网回看」要求本地留存，无 `data.restore` 布尔字段的便宜替代；故以「只读镜像」窄例外 + 「请求-应答环」换取「双击即开 + 离线回看」，一次性成本 = 一条上行帧 + 一条消费语义注记。
- **备选与放弃原因（supersedes 注记）**：本批**取代** ADR-005 备选①（:47「需新增首例设备发起请求-应答消息对」）与 ADR-010 备选②（:90「device RPC 首例」）、备选③（:90「板端 RAM 缓存重启即失是有意不变式」）的**否决理由**——「device RPC 首例」成本被本批有意支付（story 正文无便宜替代），「有意不变式」实为「存储成本约束」的误升华；ADR-005/010 的**决策本体**（`data.restore` additive 字段）不受影响、仍在役。
- **关联**：决策 24；ADR-004（云端单源）/ ADR-005（data.restore 第二实例 + 无 device RPC 先例出处）/ ADR-007（kidpal_story_delete 单向先例）/ ADR-010（data.restore 第三实例）；requirements v1.64（S-F2.1 AC2/AC7 + 补充规则正文缓存边界 + NFR-OFF-1 邻位注记）；契约 §四 kidpal_story_fetch + §1.3④ story_text；spine = delivery/intake/261016-p3-story-body-sd-cache.md。
```

**CDQ 登记（主线程账本动作，landing 时 append 到 `docs/CONTRACT-DECISION-QUEUE.md`，非 §2 改法 manifest 目标——CDQ 是跨时间账本登记、非被 diff 的契约路径；状态 PRECHECKED→LANDED 随 landing-verdict 翻）**：行草案如下，供主线程 landing 时逐字用：

```text
| CDQ-261017-p3-story-body-sd-cache | 2026-10-17 | interface-contract.md 两触点：① §四新增 kidpal_story_fetch（端→网关，{"session_id","type","ids":[...]} ids 批量，首个设备发起请求-应答环，应答复用 story_text、下行零新形状）；② §1.3④ story_text 消费语义注记扩展（形状零改动、fetch 应答不触发盖章） | ACCOMPANIES_SPEC_CHANGE（随需求 v1.64 + 决策 24 carve-out 携带，同批原子落） | 261017-arch-review-p3-story-body-sd-cache PROPOSAL（reviewer GREEN + landing-verdict PASS） | PRECHECKED | 261017-arch-review-p3-story-body-sd-cache | 实现面承载包（五档分档）；ADR-011 随批落 |
```

---

## 7. 评审记录

> 判官 append-only · R1（architect-reviewer，2026-10-17）· 审稿对象 = 本 PROPOSAL DRAFT v1.0 · 只读 canonical 亲核，唯一写面 = 本节。

verdict: BLOCK(F1, F2)

### R1 · 四查结论

**① 意图保真 — 过（无夹带）**
忠实承载 human 2026-10-16 五项原话 + spine OQ-1~9 已裁项；范围 = 「决策 24 给 story 正文开窄例外 + 契约加一条取回帧」。T2 第二梯队四处镜像句（§2.1 :62 / §2.4 原则 1 :97 / §3.1 :115 / §4 :150）系主线程 2026-10-17 裁并入本批（PROPOSAL §4 A5），非夹带；T5 story_text 消费语义注记是 T4 复用帧的直接后果、同一契约变更内，非夹带。非目标显式排除（提醒/课表/日志不落卡、不摘 data.restore、不夹带 session_id 漂移勘误）。§0/T1 史实更正忠实承载 human 原话逐字。未见超范围动作。

**② 纪律合规 — 过（除 F1 / F2）**
- Track B 主路径体例在场：Status（:3）、packet-binding（:6）、riskTier/review profile（:7-8）、landing-manifest 围栏块（:12-16）、结构字段硬条款（:10）。
- append-only 合规：决策 24 走句尾 append 注记（保原文，合决策 19/21 补注先例）；ADR-005/010 原文不动、新增 ADR-011（合 ADR.md:5 append-only + supersedes 纪律）。
- 契约 diff 只出文本未落盘：T4/T5 逐字段在围栏内（:126-130 / :144-148），亲核 `git status docs/contracts/interface-contract.md docs/architecture.md docs/ADR.md` 洁净——落盘归 packet-bound landing（主线程），合规。
- 锚/结构门形态就绪：§2 各 T「目标文件：」为裸行（非加粗），并集 = {docs/architecture.md, docs/ADR.md, docs/contracts/interface-contract.md} ≡ manifest。
- **F1**：`docs/architecture.md` §12「变更记录」changeId 行未列（见下）。
- **F2**：契约包三节未以 SSOT 规定的字段形态在场（见下）。

**③ 撞车复核 — 过**
- ADR-004（云端唯一 producer）：板端只读镜像、零写入面，零冲突。
- ADR-005/010：ADR-011 精确取代「备选否决理由」而非决策本体（`data.restore` additive 字段仍在役，草案 :209 明写）——零冲突。
- frozen requirements v1.64 + 已落 arch 三处镜像（§5.3.1 :232 / §5.4 :268 / §6.2 :318）：口径一致（唯一真相源 / 只读镜像 / 提醒课表日志不落板 / 断网无提醒不变），无口径不一。
- 契约 §四既有消息族：新帧载荷 `{"session_id","type","ids":[...]}` 与 delete（`... "id":"..."` 单 id）不同形、无形态撞车；应答复用 `story_text`、下行零新形状。
- 禁令重述点 census 完备（起草期实跑 `grep -n "不存数据|零存储|不缓存|禁落|介质禁令|哑终端|不承载" docs/architecture.md`）：需收窄者 = :62/:97/:101/:115/:150 五处（T1 两处 + T2 四处），:266（凭证白名单，合法存续）/ :267（板端「其余」业务状态仍 RAM-only，不与 carve-out 冲突）/ :429（术语指针）不需改——T2「四处」枚举完备。

**④ 牵连面完整 — 过（除 F1）**
- landing-manifest 三文件覆盖 T1–T5 全部改法；T2 四处镜像句全在 `docs/architecture.md` 内、在 manifest 覆盖内。
- 反向锚：requirements S-F2.1 支撑锚 = arch §5.5.4 / §5.1（requirements.md:443），均不重编号；本批不改小节号、锚稳定规则不触、无悬空锚。
- **F1**：arch 文末 §12 changeId 行为本批定稿标记，未列。

### 否回项

**F1 · `docs/architecture.md` §12「变更记录」changeId 行未列**
- 位置：PROPOSAL §2（T1/T2 目标文件行）、§5 验证集第 4 条、landing-manifest。
- 问题：本批对 `docs/architecture.md` 的改动不止需求批的逐字镜像，还含决策 24 carve-out（语义性内容变更，T1）。本批「定稿标记」按 `.claude/references/arch-change.md` :37 = `docs/architecture.md` 文末「变更记录」节的 changeId 行；同文件 §12 头部（`docs/architecture.md`:433）逐字「本文件每次改动都在下表追加一行（新行在上，倒序）」。现改法集 / §5 byte 对账清单（「T1 两段 insert、T2 四段 replace」）/ landing-manifest 均未列该行 → packet-bound landing 不会落该行（packet-id = §2 改法集 + §5 验证集），§12 顶层仍停在 `261015-gov-review-changes-taxonomy`。
- 依据：arch-change.md :37-38（changeId 行 = 定稿标记 + launchPreconditions「arch 已冻结」消费判据）；architecture.md:433（每改动 append 一行）；261015-gov-review-face-loop-unify PROPOSAL 自身即列其 §12 行（先例）。下游 261016-p3-story-body-sd-cache 实现面承载包的 launch 前置（上游 arch 已冻结 = changeId 行在场）会因缺行不可满足。
- 建议：在 §2（T1 或 T2）补一条落地动作、在 §5 第 4 条 byte 对账清单加列——append 一行 `| 261016-p3-story-body-sd-cache | 2026-10-17 | §2.1 / §2.4 原则1·5 / §3.1 / §4 禁令重述点收窄 + §10 决策 24 carve-out 史实更正注记（+ §5.3.1 / §5.4 / §6.2 需求批镜像） | delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md |`（列义与 261015 行同形）。若仓内既定实践是主线程 landing 期机械补该行，则在 §2 或 manifest 注一行显式说明，使 byte 对账面有据。

**F2 · 契约包三节未以 SSOT 规定字段形态在场**
- 位置：PROPOSAL §2 T4/T5、§3、§5。
- 问题：`.claude/references/contract-change-evidence.md`（261015 T17 收窄版）:7「契约变更批（/arch-change）的 PROPOSAL 改法集**必须含契约包三节**」（`proposedContractDiff` / `impact` / `validationPlan`，:34-42 给「### 契约包 · <面名>」骨架、字段名沿 architect-reviewer 输出格式）。现 PROPOSAL 无「契约包」节、三字段名未在场——diff 内容在 T4/T5、impact 内容散在 §3、validationPlan 内容散在 §5，均未按 SSOT 字段形态标注（亲核全文零「契约包」/「proposedContractDiff」/「validationPlan」字样）。
- 依据：contract-change-evidence.md :7-11 / :34-42；rules/contracts.md :8/:18；arch-change.md :33。`pre-merge-escalation-gate.mjs` 契约分支只查「FROZEN + PASS verdict + manifest 覆盖」三件（:132-159），不解析三节——故此非机闸拦、是 SSOT 文面义务；然 `ACCOMPANIES_SPEC_CHANGE` 的处置栏逐字即「契约包三节并入该批 PROPOSAL」（rules/contracts.md :18），本批正是该分类的首个 post-261015 实例。
- 建议：补一节「### 契约包 · 端云协议 §四/§1.3④」，按骨架显式写 `**proposedContractDiff**`（= T4 新帧 + T5 注记的最小 diff，或直接指 T4/T5）`**impact**`（= §3 契约消费面：固件 / front）`**validationPlan**`（= §5 命令 + 退出码 + 预期）。内容已具备，属轻改。

### 非阻断观察（NIT）

- NIT-1：ADR-011 草案在固定五小节（日期/决策/理由/备选与放弃原因/关联）外增设「边界守持」小节（附件 :208）。内容承重，但 ADR.md:6 定「固定小节」——建议并入「决策」或「理由」，或视为允许扩展并留由（不阻断）。
- NIT-2：T5 §1.3④ 注记（:147）写「fetch 应答帧入板后写 SD 本地副本」，未提「卡不在位时不留存、当场显示」分支（requirements v1.64 补充规则已覆盖）。契约注记不必复述全部固件行为，可选对齐。
- NIT-3：contract-change-evidence.md :44 有「每契约变更批一行」的 CDQ 登记义务（四分类五值 + changeId + landing-verdict 结果）。PROPOSAL 未提该行落点——建议确认（若为主线程 landing 期账本动作则 §2/manifest 注一行；若入批则该路径须进 manifest）。

### 判档（独立）

**主路径**。轻路径判据 ① 零语义变化（否：决策 24 carve-out + 新增契约帧 = 语义）、② 改动性质限错字/排版/澄清/指针（否：新增 ADR + 契约帧 + carve-out）、③ 不触 `docs/contracts/**`（否：触）——三否，主路径。architect 建议主路径，独立复核同意、无降档。Track A 段：方向已收敛（human 2026-10-16 选路线 (a) + OQ-1~9 已裁），无方向级缺口，不升 Track A 段。

### 契约四分类（独立）

**ACCOMPANIES_SPEC_CHANGE**。契约变更（§四新增 `kidpal_story_fetch` 上行帧 + §1.3④ `story_text` 消费语义注记）随已 frozen 需求 v1.64 与决策 24 carve-out 同批原子落地——属「随 spec/架构变更携带」（rules/contracts.md :18）。非 SPEC_CONTRACT_DRIFT（requirements 与契约无「须裁改哪边」的冲突，是契约补能力而非消解矛盾）、非 CONTRACT_CHANGE_REQUIRED（非脱离 spec 的独立契约变更，需求 v1.64 即其 spec 依据）。architect 建议一致。

### 两件裁决

- **决策 24 处置形态（append 注记 vs 原地改写）**：append 注记**合纪律、不升档**。依据：ADR.md:5 append-only 精神 + arch §10 头部（:392）「本节以下为 MVP 时代决策历史档，冻结不再新增」+ 决策 19/21 的句尾补注先例（PROPOSAL T1 已引）。注记（插入 1）与决策 24 条目同址、显式点名被更正句「『哑终端不存数据』纯洁性保持」，误读窗口已闭；canonical 侧另有 requirements v1.64 + §6.2 镜像兜底。不改原文属正确选择，无须升档。

- **ADR-011 supersedes 精度**：**精确**。草案 supersedes 注记（附件 :209）逐字取代「ADR-005 备选①（:47）与 ADR-010 备选②（:90）、备选③（:90）的**否决理由**」，并明写「ADR-005/010 的**决策本体**（`data.restore` additive 字段）不受影响、仍在役」——指到备选否决理由、未误伤决策本体。

### 首请求-应答环事实独立核验

**成立，PROPOSAL 据实表述正确、已无初版不精确引据。**
- 契约 §四 `kidpal_story_delete` 逐字（`docs/contracts/interface-contract.md`:271）：「**单向通知、无应答消息**……不新增请求-应答消息对，ADR-005 边界保持」——亲核吻合。
- `kidpal_session_ctx`（同文件 :270）：单向通知（网关消费拦截、无应答）——同为单向。
- 故「设备→网关**方向**先例存在（delete / session_ctx 两条单向），但**请求-应答环**本批之前不存在」成立。PROPOSAL T4 逐字（:129）「这是契约面首个『设备发起 request → 网关 response』的请求-应答环——此前 §四私有消息（`kidpal_story_delete` / `kidpal_session_ctx`）均单向通知、无应答」表述精确（限定「§四私有消息」，未误扩及 hello 等小智原生握手）；PROPOSAL 全文已无「delete 是首例请求-应答」类不精确引据（spine OQ-9 纠偏已落）。ADR-011 草案（:206）同款据实。

### 结论

BLOCK(F1, F2)。两项均为文面/SSOT 合规缺口（非语义冲突），修订后靶向复核即可。

---

> 判官 append-only · R2（architect-reviewer，2026-10-17）· 靶向复核（R1 否回 + NIT 闭合 + 新问题扫描）· 只读亲核。

verdict: GREEN

### R2 · 靶向复核结论

**F1 闭合 — 是。**
- T6（:152）逐字 append 行围栏（:165）；§5 第 4 条 byte 对账清单已加列 T6（:215）；§2 T6 目标行 `docs/architecture.md`（:168）在 manifest 内。独立性亲核：arch §12 表头 4 列（changeId | 日期 | 改动面 | 依据，architecture.md:435），T6 行同为 4 列、格式吻合。
- **changeId 取值裁决（应询）**：取 spine changeId `261016-p3-story-body-sd-cache` **正确**。理由：① 本批是跨面 intake，spine changeId = 本变更全链身份；需求面 §9 的 v1.64 行（requirements.md:716）即用同一 spine changeId 记账——arch §12 沿用同 identity 才与需求面对偶，下游承载包按 spine anchors.arch「arch 已冻结」消费时指向一致；② 261015 用 `261015-gov-review-changes-taxonomy` 因它是纯 gov 批、changeId 即 review-dir token、无独立 spine，非跨面；③「依据」列已指到实际 review 目录（delivery/review/261017-arch-review-…），identity（changeId）与证据路径（依据）两分——干净。
- **一处 FROZEN 前须处置（零语义）**：:165 逐字围栏行含「（FROZEN vX.Y；契约面同批原子落）」占位——该行是要 byte 落进 architecture.md §12 的承重文本，字面落「FROZEN vX.Y」= canonical 留模板 token；若主线程回填实际版则 landed ≠ packet 围栏块（byte 对账面断）。建议二选一：① 从围栏行删「FROZEN vX.Y；」只留「契约面同批原子落」+ 依据路径；或 ② 现填实际冻结版使 byte 对账干净。

**F2 闭合 — 是。**
新增「### 契约包 · 端云协议」（:170）按 contract-change-evidence.md :34-42 骨架显式三字段：`proposedContractDiff`（:172）/ `impact`（:176）/ `validationPlan`（:178），各指回 T4/T5 · §3 · §5。子字段名核（应询）：`shimApi` / `displayBlock` 是 contract-change-evidence.md:9 四字段（shimApi / displayBlock / strokeFormat / scheduleMD）中的两项、配对正确——§四端云协议剖面 = 端云接口面（CDQ-260902 即把「新增端云协议剖面节」记在 shim-api-field-envelope 面，先例印证）；§1.3④ story_text = displayBlock；strokeFormat / scheduleMD 本批未触、正确省略。与 SSOT 骨架吻合。

**NIT 处置 — 均可接受。**
- NIT-1（ADR-011 固定小节）：已闭——「边界守持」并入决策为 ③（:234），回归 ADR.md:6 五小节（日期/决策/理由/备选与放弃原因/关联）。
- NIT-2（§1.3④ 卡不在位分支）：已闭——注记补「卡不在位 / 坏卡时不留存、当场显示」（:147）。
- NIT-3（CDQ 归属）：**认可**。CDQ 是跨时间账本（docs/CONTRACT-DECISION-QUEUE.md），非被 diff 的契约路径（在 docs/contracts/** 之外）、pre-merge 闸 contract-path 判据只认 docs/contracts/**、其 loop 凭据列依赖 landing-verdict——归主线程 landing 期 append、不进 manifest，成立。§6（:240-244）逐字行草案亲核列数吻合 CDQ 表头 8 列（CDQ-id | 日期 | 变更面 | 四分类 | loop 凭据 | 状态 | 关联 change | 剩余条件）；状态 PRECHECKED→LANDED 随 landing-verdict 翻、与 contract-change-evidence.md:28 词表一致。小注（不阻断）：草案「关联 change」列写纯 changeId，历史行多用相对链接——主线程 append 时可对齐列风格（旧链接现指归档树，非必循形状）。

**新引入问题 — 无实义夹带；两处 stale token 建议 FROZEN 前顺修。**
- 亲核 preflight（实跑 §5:213 命令）→ `ok:true` / `findings:[]` / anchorUnion = {docs/ADR.md, docs/architecture.md, docs/contracts/interface-contract.md} ≡ manifest 三文件；「契约包」非 T 节、T6 目标行归 docs/architecture.md，未新增锚 token。
- **stale-1**：packet-binding 行（:6）「§2 改法集（T1–T5）」→ 现有 T6，应改「T1–T6」（packet-id 是结构字段，失准会少计 T6；机闸不解析该文本，但按「结构字段 GREEN 前核对」宜正）。
- **stale-2**：§2 标题（:33）「（T1…T5）」→ 建议「T1…T6 + 契约包」。
- 二者均一 token 级、零语义、不影响闸与 byte 对账，判为 FROZEN 前必改（与上 §12 占位一并由主线程标 FROZEN 时收口），不构成再次否回。
- 余：+105 行全部可追到 F1/F2/NIT 闭合，无越界 canonical、无新契约触点、manifest 未变——无夹带。

**四查遗留面（R1 已过，R2 抽核未翻案）**：意图保真 / 纪律合规 / 撞车复核 / 牵连面四查结论维持 R1；首请求-应答环事实（delete 单向、本批首个环）维持成立；决策 24 append 处置、ADR-011 supersedes 精度维持 R1 裁决。契约 diff 仍未落盘（亲核 `git status` 三 canonical + CDQ 全洁净）。

### 判档（最终）：主路径

与 R1 同——轻路径三否，主路径；方向已收敛（human 选路线 (a) + OQ-1~9 已裁），不升 Track A 段。

### 契约四分类（最终）：ACCOMPANIES_SPEC_CHANGE

与 R1 同——随已 frozen 需求 v1.64 + 决策 24 carve-out 同批原子落；非 drift、非独立契约变更。

### 结论

GREEN。R1 两否回（F1 / F2）实质闭合、三 NIT 处置可接受、无实义夹带；两处 stale token（packet-id :6 / §2 标题 :33）与 §12 行 FROZEN 占位（:165）为 **FROZEN 前必改**项（零语义、一 token 级），由主线程标 FROZEN 时一并收口。
