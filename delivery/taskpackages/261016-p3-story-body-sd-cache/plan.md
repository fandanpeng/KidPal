---
changeId: 261016-p3-story-body-sd-cache
rigor: full
riskTier: high
sourceType: proposal
reviewed-by: architect
date: 2026-10-17
---

# 建什么 + 方案（P3 story 正文本地缓存：SD 只读镜像 + 预取 + 双击兜底 + id 校验修复）

> **档位**：②单包 Full（human 触点①改判，自 ③拆包 降为单包；riskTier 仍 high，单向守卫不可降）。
> **changeId 复用说明**：沿用 `261016-p3-story-body-sd-cache`——本变更是单一跨面变更，需求面 §9 版本行与架构面 §12 changeId 行均已钉本 id 作「定稿标记」；`delivery/intake/<id>.md`（spine）与 `delivery/taskpackages/<id>/`（本包）是不同树、不同 INDEX lane，无路径/记账冲突；形状 `^\d{6}-[a-z0-9-]+$` 合法；日期前缀 261016 = 变更立案日（task-intake-and-slicing.md 明载日期前缀是「近似主张、非精确建包时戳」），复用保「一变一 id」跨面可追，另铸新 id 会割断需求/架构锚与本包的连线。

## 建什么

孩子在 P3 星星记录本里选中任意一条记录双击，**都能进阅读态看到正文**——本机录的、重启前录的、云端恢复回来的老故事一视同仁；断网时看过的正文仍能看；断网又没缓存的篇给出孩子看得懂的可见提示；**双击打开的一定是光标选中的那一篇**（修 id 错配）。

板端 TF 卡（`/sdcard`）成为 story 正文的**只读镜像**缓存介质——云端 story 归档仍是唯一真相源，板端只读、一切写入仍走云（决策 24 carve-out + 契约 §四/§1.3④，均已在本批上游落地）。

**来源**：跨面 spine `delivery/intake/261016-p3-story-body-sd-cache.md` + 消费 debt `docs/debt/tech/TECH-261013-p3-reader-entry-no-text-silent-noop.md`（含 2026-10-16 追加的 id 错配缺陷段）。上游三重 anchors 已齐：需求 v1.64 frozen（S-F2.1 AC2 兑现 + AC7 + 正文缓存边界 + WF-F2-06）、架构已落（决策 24 carve-out + §5.3.1/§5.4/§6.2 + ADR-011）、契约已 LANDED（CDQ-261017：§四 `kidpal_story_fetch` + §1.3④ story_text 消费语义）。

## 方案

### 实施序列（单包内三块，顺序推进，不拆 Pod）

单包内按依赖顺序三段实施：**块 1（固件本地能力）→ 块 2（云端 front 收帧回帧，可与块 1 交错/并行编码但同一 Pod）→ 块 3（显示面 id 校验修复 + 双击三分支）**。三块同属一个 changeId、一个 fixBudget、一个收口轮；不再有包间 blocking edge，但 `mcp_server.cc` 的区域分工纪律仍保留（见「关键接线」）。

#### 块 1 · 固件本地能力（缓存层 + 预取器 + LFN + 协议帧 + story_text 写卡）

- 新增 `firmware/main/display/story_cache.c/.h`：纯逻辑模块，承载 RAM 索引（卡上有哪些 id）、「当屏本地缺失项」判定、读回 id 校验、预取队列决策——**零 FATFS/SD 依赖，host_app linux target 同源可测**（对齐 reminder_page.c / story_page.c 既有纯逻辑缝范式）。
- 新增 SD I/O 薄适配（chip 侧，建议 `firmware/main/boards/waveshare-s3-rlcd-4.2/story_sd_io.cc`）：`/sdcard/story/<id>.txt` 读写，消费已就绪的 `s_sd_mounted`（waveshare-s3-rlcd-4.2.cc:274）；卡不在位/坏卡 → 整层失效、预取跳过、双击走「拉进 RAM 显示、不落盘」路径。
- `protocols/protocol.cc` 新增 `SendKidpalStoryFetch(ids)`（照 `SendKidpalStoryDelete` :81-89 同型，载荷 `{"session_id":...,"type":"kidpal_story_fetch","ids":[...]}`）。
- `mcp_server.cc` 的 `VRT_ROUTE_STORY_TEXT` 分支（:1134-1154）扩展：story_text 到达后写卡 + 更新索引 + 触发预取（若在列表态）。
- `CMakeLists.txt` 接入新模块；`sdkconfig` 开 LFN。

#### 块 2 · 云端 front 收帧回帧

`server/gateway/front/front_app.py` 新增 `kidpal_story_fetch` 消费分支（照 `kidpal_story_delete` :256-259 同族缝）：逐 id 校验存在于 story 账本（哨兵头 + `<stories_dir>/<id>.md` 存在）→ 读 MD → 以既有 `story_text` 帧逐篇应答（**复用** `gw_story.story_text()` + `_push_display_payload`，见 :315/:531/:607，零新字段）；非法/不存在 id 拒绝 + 审计 + 不回帧；空 ids = no-op。**契约复用核查 PASS**：契约 §四已逐字钉死此行为语义，实现面不产生新契约触点、无漏定字段。

#### 块 3 · 显示面 id 校验修复 + 双击三分支 + 文案单源

- `display/story_page.c` `kidpal_story_open_reader`（:1707-1732）签名加 id 校验（校验「被打开正文 id == 光标所选行 id」，不符拒绝）。
- `mcp_server.cc` `KSPV2_ACT_OPEN_READER` 分支（:305-317）三分支重写：命中索引 → 读卡进阅读（零网络）；未命中 → 插队发该篇 `kidpal_story_fetch`（优先级高于后台预取）→ 到达后进阅读；离线/超时/卡不在位且拉取失败 → 可见降级提示（WF-F2-06 整屏覆盖层，10s 无操作自动消失）。
- `display/story_page.h` 文案单源（降级提示话术，守既有「话术单源 = story_page.h」纪律）。

### 关键接线决策

- **`mcp_server.cc` 区域分工（单包内纪律）**：块 1 只动 `VRT_ROUTE_STORY_TEXT` 分支（写卡 + 预取触发），块 3 只动 `KSPV2_ACT_OPEN_READER` 分支（三分支 + id 校验消费侧）。同一文件两个函数、同一 builder 顺序实现，避免同函数互踩——单包内无需 blocking edge，但此分工仍是必须（两分支语义独立、不得交叉污染）。
- **预取器不挤占语音主链路（可验收约束）**：把「该不该发下一帧」做成纯逻辑函数（显式注入 `page_state / voice_front / ws_busy / elapsed_ms`），host_app 单测断言 + 真机观察双层。可验收齿：① 仅在 `page_state == KSTORY_PAGE_LIST` 且 `voice_front == 0` 发 `kidpal_story_fetch`，录音态/会话态/阅读态立即挂起并清在途队列；② 逐篇串行 + 篇间间隔（≥ 阈值 ms）+ 发前查 WebSocket 空闲；③ 离开 P3（页态迁移）即停。**正确性不对称钉死**：「预取是优化、双击插队 fetch 是正确性保证」——预取挂掉功能照常，插队兜底保证正确性，两者共用同一条帧、契约不因多触发点加东西。
- **LFN 裁决结论（OQ-4，硬结论保留）**：开 LFN、用 `<id>.txt`，无硬阻。代价 = `CONFIG_FATFS_LFN_NONE` → `CONFIG_FATFS_LFN_HEAP`，内存 ~1-2KB heap（估读，builder 真机编译烧板后以 heap 水位实测确认）；codepage 非问题——story id 纯 ASCII（`_slugify`/`_hash_slug` 产 ASCII slug / `s`+8hex，中文不进文件名），codepage 437/850 即可。**8.3 路线不可行的硬论据**：`story_list` items 不携带 seq（契约 §1.3③ items={id,title,date}，seq 只在 story_text 载荷 §1.3④），故 `S<seq:04d>.TXT` 在列表时点拿不到 seq，要么重新开契约、要么退化为 id-hash 8.3 映射（多一层映射 + 碰撞 + 读回校验），均更差——开 LFN 是唯一干净路线。
- **id 错配修复验法**：① host_app 单测（`kidpal_story_open_reader` 加 id 参数：id 相等 → 进阅读；id 不等 → 拒绝 + 消费侧走降级）；② 真机回归位：双击「非本机录制的那一篇」→ 显示选中的那篇、不是缓存残留篇（TECH-261013 追加段「显示错内容比无反应更危险」的直接回归齿）。
- **riskTier=high 的下游含义**：supervisor 合并门对 high 包升级 human（非机械自动放行），真机 live 轮 + human 目检是预期收口路径。

## 技术方案（planner+architect 预评）

> **2026-10-17 · architect 前置技术预评（planner Step 1g ARCH-ADVISORY 档；只读 advisory，不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据）**。以下为**采纳要点**，全文见本包 pod-events 同日 dispatch 记录。

- **id 校验落点修正（不改 `kidpal_story_open_reader` 签名）**：该函数是冻结 seam，被 `story_page_v2.acceptance.c:551` 与环境调用点（mcp_server.cc:311/:1198）钉死；加参数会破冻结齿。改为**新增 additive 纯逻辑 seam** `kidpal_story_reader_id_match(st, list, cursor)`（落 `story_cache.c/.h`），在 `mcp_server.cc` OPEN_READER 消费侧调用 `open_reader` **之前**校验（正文 id == 光标所选行 id），不符拒绝并走降级。**行为不变、冻结件零触碰**。
- **共享状态单元（单包最大工程风险点）**：预取队列 / 双击插队 / pending-fetch 等待态 / SD 索引是块 1 与块 3 **共用的同一套状态**——按一个单元一次设计、一次实现，两分支只消费其入口；不得把块 1、块 3 做成两次互不相干的改动（否则互踩同一队列）。
- **`SdCardMounted()` 前置阻断项**：现为 waveshare 板私有成员（:967），基类 `Board` 无此 virtual → mcp_server 拿不到挂载标志。须在 `boards/common/board.h` 加 `virtual bool SdCardMounted() { return false; }`（默认 false），waveshare 板侧改 public + override。
- **SD I/O 适配边界**：`sd_mounted` 以显式 bool 入参注入（不直接碰板级私有态）；无卡/坏卡短路、绝不 fopen；**先写卡成功才登记 RAM 索引**，写失败只清该篇、正文照常 RAM 显示（不整层清空）。
- **front 复用零新字段**：新增 `kidpal_story_fetch` 消费分支只调 `gw_story.story_text()` + `_push_display_payload`（不拼新 key）；建议在 `server/gateway/story.py` 抽 `fetch_story()` helper 与 `delete_story` 共享 id 安全（`_ID_UNSAFE_RE` / `..` / resolve 逃逸 / 哨兵头），避免复制路径穿越防御；非法 id 记 warning 审计、空 ids no-op。
- **盖章侧效应保证**：fetch 路径不置 `archive_pend`、不调 `kidpal_story_on_saved`——`fresh_save` 判据（archive_pend>0 && id≠last_saved_id）在 fetch 应答时恒假，天然不进盖章（契约 §1.3④）。
- **LFN 具体键**：`firmware/sdkconfig` 的 `CONFIG_FATFS_LFN_NONE=y` → `CONFIG_FATFS_LFN_HEAP=y`；codepage 维持 437（story id 纯 ASCII）；LFN 工作缓冲走 PSRAM（`CONFIG_FATFS_ALLOC_PREFER_EXTRAM=y`），heap 观察位 = 挂载后 / 写卡后 internal + SPIRAM 读数。
- **WF-F2-06 覆盖层归属**：走 `story_page.c` 新增页态（与 WF-F2-05 删除确认同族：页态值 additive + render + 手势 + 10s idle 步进 seam），不动 `kidpal_render_node.cc`。
- **预取竞态收口**：决策与发帧合并同一 tick，发帧前 `protocol.IsConnected()` 实时复查；页态迁移 / `voice_front` 置位时立即清空在途队列。
- **id 路径安全**：固件写卡前二次校验 id 只含 `[a-zA-Z0-9._-]` 且不含 `..` / `/` / `\`（不信任云端、卡可换）；front 侧复用 delete 的 id 安全面。

## 非目标

光标预取 / 缓存淘汰（LRU）/ 热插拔检测 / 离线产出（写入仍须联网）/ 摘除 data.restore 补丁（另立变更评估）/ 隐私加密防护（TECH-261016 已登记 deferred，本批只落盘不加防护）。

## 留根清单

TECH-261016（隐私加密防护 deferred）· data.restore 补丁摘除评估 · 缓存淘汰策略（真实数据后另裁）。

## 附录 · 结构化种子（planner intake 输入，非 plan 正文）

### changeManifest 种子（7 改 + 2 新增）

- 新增 `firmware/main/display/story_cache.c` + `story_cache.h`（纯逻辑，host_app 同源）
- 新增 SD I/O 薄适配 `firmware/main/boards/waveshare-s3-rlcd-4.2/story_sd_io.cc`（chip 侧）
- 改 `firmware/main/CMakeLists.txt`（接入新模块）
- 改 `firmware/main/protocols/protocol.cc`（`SendKidpalStoryFetch`）
- 改 `firmware/main/mcp_server.cc`（`VRT_ROUTE_STORY_TEXT` 写卡 + `KSPV2_ACT_OPEN_READER` 三分支，两分支区域分工）
- 改 `firmware/main/display/story_page.c`（open_reader id 校验签名）
- 改 `firmware/main/display/story_page.h`（文案单源）
- 改 `firmware/sdkconfig`（开 LFN）
- 改 `server/gateway/front/front_app.py`（收帧回帧）

### impactedTests 种子（三面）

- **firmware host_app**：story_cache 索引 / 缺失判定 / 读回 id 校验 / 预取队列决策单测；open_reader id 校验（相等进阅读 / 不等拒绝）；OPEN_READER 三分支决策单测。
- **server pytest**：front `kidpal_story_fetch` 收帧回帧——合法 id 逐篇回 story_text、非法/不存在 id 拒绝不回帧、空 ids no-op、多 id 逐篇顺序。
- **firmware 真机 live-manual（收口轮）**：SD 读回 roundtrip；双击三类记录进阅读（本机录/重启前/云端恢复）；离线双击降级提示；双击非本机录制篇显示选中篇（id 错配回归位）；预取期语音无卡顿。

### sourceRefs / origin / gateProfile / crossEnd / decisionCoverage 种子

- `sourceRefs`：`S-F2.1`（AC2、AC7）、`S-F2.3`、`docs/requirements.md:S-F2.1`、`delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md`、`docs/debt/tech/TECH-261013-p3-reader-entry-no-text-silent-noop.md`、`delivery/intake/261016-p3-story-body-sd-cache.md`。
- `origin`：`upstreamType: proposal`、`upstreamRef: delivery/review/261017-arch-review-p3-story-body-sd-cache/PROPOSAL.md`（上游冻结决策权威 = 决策 24 carve-out + 契约包）、`upstreamFrozen` 由 planner 冻结时按标准 pin 回灌。
- `gateProfile`：`["state-machine", "mcp-tool-contract", "contract-drift", "security", "docs-sync"]`。
- `crossEnd`：跨端申报——`counterparts` 至少含 `{path: server/gateway/front/front_app.py, symbol: kidpal_story_fetch 消费分支}` 与 `{path: firmware/main/protocols/protocol.cc, symbol: SendKidpalStoryFetch}`；不填 exempt（跨端性明确）。
- `decisionCoverage`（sourceType=proposal → 必填）：至少映射——决策 24 carve-out（落地到 SD 写卡 acceptance）→ `acceptance:`；契约 §四 `kidpal_story_fetch` ids 批量消费（落地到 front pytest）→ `acceptance:`；§1.3④ fetch 应答「卡不在位不留存、盖章不触发」（落地到固件消费 acceptance）→ `acceptance:`；AC2「双击一视同仁进阅读」→ `acceptance:`；AC7「可见降级提示 + 正文须与所选篇目一致」→ `acceptance:`；预取不挤占语音主链路 → `acceptance:`（纯逻辑缝）+ 真机观察 exitCriteria；id 错配修复 → `acceptance:`；隐私只落盘不加防护 → `dropped`（reason 引 TECH-261016 deferred）。
