# Docs Delta · 260924-m4-k1-story-cloud-link

> integrator 收拢制品（2026-09-24）。本件回答「本包文档面义务是否已全清」，逐面核对与事实锚如下，并承载 C4（arch 面收口）的 **patch 形态 delta**（integrator 无 `docs/architecture.md` 写权，delta 在此成文、合并后由 human spec 批落地）。
> TaskPackage `docsSyncRequired=true`；`docRefs = [docs/requirements.md#F2, docs/architecture.md#§5.2, docs/architecture.md#§5.4]`；`contractRefs = [interface-contract.md#§1.3③, #§1.3④, #§四]`；`docsOwner = integrator`。

## 1. 结论

- **契约面：本批零触碰**（契约 diff 已由主线程经 human 契约门通道于本包 launch 前落地 = landing commit `5e862d0` / 常驻分支同义 commit `3a9ff24`）。本包只读消费 `docs/contracts/interface-contract.md`（§1.3③ / §1.3④ / §四）；`contract-lint` 闸绿。
- **需求面（`docs/requirements.md`）：零改**。`docRefs#F2`（S-F2.1/S-F2.2/S-F2.4 + §2.1 记录会话段）为行为权威、口径已冻结在前（v1.26–v1.52），本包**消费**不改。
- **架构面（`docs/architecture.md`）：本批不可写 → C4 义务以 patch 形态 delta 成文于本件 §4，合并后由 human spec 批落地**。落地前 gate evidence §6 C4 **保持 pending**（已追加处置注记）。delta 恰好覆盖 taskpackage `docsSyncReason` 声明的四处：§5.2 场景④扩展 / §5.4 故事行载体收口 / §5.1.2 front 故事接线现状行 / §7 在办①②④销账。
- **ADR 面：零改**。决策已由契约落地批登记 `ADR-007`（`docs/ADR.md:60`：盖章 N = front 持久 seq 账本 / story_list 最新 16 条窗口 + total 全量 / 删除 = 端→网关单向消息 + 隐式 ack 对账）；本包实现与之一致，**无新 ADR 实义**（不重复登记）。
- **workstream 面：`delivery/workstreams/m4-star-notebook/progress.md` append 收口节**（append-only，不改既有行）。

## 2. 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` | **零触碰**（landing 批已落，本包只读消费；铁律 E2） | 只读 |
| spec 面·需求 | `docs/requirements.md`（`docRefs#F2`） | 零触碰（既有冻结需求，本包消费） | 只读 |
| spec 面·架构 | `docs/architecture.md` | **本批不可写 → patch 形态 delta 成文于本件 §4**（合并后 human spec 批落地） | delta（待批） |
| ADR | `docs/ADR.md` | 零改（ADR-007 已在；本包实现与之一致） | 只读 |
| workstream | `delivery/workstreams/m4-star-notebook/progress.md` | append 收口节 | docs-sync 落点 |
| closeout 制品 | `delivery/changes/dev/260924-m4-k1-story-cloud-link/`（本件 + `review-packet.md`） | integrator 收拢制品落盘 | closeout |

## 3. docs-sync 机械判据落实

- 判据来源 = scoped/final 相位的 `docs-sync` 闸：TaskPackage 有 `docRefs` 且 `docsSyncRequired=true` → 走 `docsSyncReason` 分支（闸 stdout 逐字引 `docsNoChangeReason` = 本包 `docsSyncReason`）。**scoped 轮 `docs-sync` 闸 exit 0**（见 `verifier-evidence/rounds/r1.json`）。
- 语义层核对：`docsSyncReason` 声明的四处 arch 收口动作 = 本件 §4 delta 的四处 hunk，一一对应，**无声明未做的项、无做而未声明的项**。

## 4. C4 arch 收口 · patch 形态 delta（本批不可写 `docs/architecture.md`；合并后由 human spec 批落地）

> **结构性事实前置（如实）**：integrator 的 scope-map allow 列表为 `["firmware/**","server/**","docs/systems/**","docs/templates/**","docs/ARCH-REVIEW-QUEUE.md","docs/crq/**","docs/ADR.md","delivery/changes/dev/**","delivery/taskpackages/**","delivery/workstreams/**"]`——**含已废弃的 `docs/systems/**`（AGENTS.md 明载 docs 已平铺、不再套 `systems/<sys>/` 层）却无 `docs/architecture.md`**，故 C4 类义务对本包角色结构性不可直达。同族在案 = `docs/crq/tech/ARQ-260918-spec-face-3page-reconcile.md`（deferred，owner=architect + 主线程 spec 批，「dev-loop 角色对 `docs/**` 无写权、不代改」）。**本 delta 即 ARQ-260918 建议改动同族的第五、六、七、八处，可与之同批搭载。**
>
> **基线**：`docs/architecture.md` 现行文本（本 worktree 工作树态，2026-09-24）。文件行结尾为 LF；若工作树物化为 CRLF，`git apply` 前须以工作树实态生成（CCP 落地通道纪律先例）。
> **落地后动作**：`architecture-gate-evidence.md §6` C4 由 pending 翻 done（落地 commit 号回填）；本包 review-packet 已先行申报该未覆盖面。

### 4.1 delta 汇总表

| # | 位置（现行行号） | 动作 | 归属 |
|---|---|---|---|
| H1 | §5.2 场景④ `:213` | 重写该段（推送触发扩展 + 收尾三式 front 机械 + 删除/账本链路口径） | docsSyncReason「§5.2 场景④扩展」 |
| H2 | §5.1.2 front 网关行 `:185` | 重写该行末「故事接线」短语为「故事机械面」全项 | docsSyncReason「§5.1.2 front 故事接线现状行」 |
| H3 | §5.1.2 故事归档行 `:192` | 重写该行（**连带**：seq/窗口/delete/超时决策落地后该行现状描述失真，留之即造新漂移） | §5.1.2 连带现状行 |
| H4 | §5.4 故事行 `:261` | 重写该行 + 新增 `.seq.json` 账本行（载体收口） | docsSyncReason「§5.4 故事行载体收口」 |
| H5 | §7 契约门在办 `:341` | 重写该行（①②④销账、③留办） | docsSyncReason「§7 在办①②④销账（含①遗留漂移同笔核销）」 |

### 4.2 H1 · §5.2 场景④（`:213`）

上下文：`:212` 空行；`:214` 空行；`:215` = `**⑤ 提醒推送**：…`（本 hunk 仅替换 `:213` 单行）。

```diff
@@ -213,1 +213,1 @@  §5.2 运行视图 · 场景 ④ 记录归档
- **④ 记录归档**：触发词（「我要讲故事」/「帮我记下来」）或 P3 会话上下文信号（设备告知「会话发起自 P3」，网关会话建立即切累积态——requirements v1.32 扩展；信号载体 = 契约 §四 `kidpal_session_ctx`（front 拦截消费、引擎不感知））→ 引擎 manual 聆听 + front 旁路累积（stt 帧喂入，AI 不逐句回复）→ 收尾（「我说完了/讲完啦」/再按 KEY / 120s 超时自动收尾归档）→ `story.py` 归档 + 起标题（模型一回合只起 ≤12 字 title）→ `story_text` 下发渲染 + 盖章演出（N = 入册流水号只增不减）。误判兜底见 §10 决策 25 与 requirements v1.47/v1.49 修订。
+ **④ 记录归档**：触发词（「我要讲故事」/「帮我记下来」）或 P3 会话上下文信号（设备告知「会话发起自 P3」，网关会话建立即切累积态——requirements v1.32 扩展；信号载体 = 契约 §四 `kidpal_session_ctx`——**front 上行泵整条拦截消费、无论取值、引擎不感知**，TLS 之后不下透）→ 引擎 manual 聆听 + front 旁路累积（stt 帧喂入，AI 不逐句回复）→ **收尾三式**（front 机械扫描 stt 文本命中收尾词「我说完了/讲完啦」→ stop 归档 / 再按 KEY（listen-stop）→ 归档 + 下发 / **120s 无语音超时** front 侧计时 sweep 自动收尾；三式皆走正常归档、不丢弃）→ `story.py` 归档（**持久流水号账本 `<stories_dir>/.seq.json`** 分配 `seq=max+1`、只增不减——同 id 重写不变、删除不回退、重启存活、账本缺失/损坏重建不回退 + 告警）+ 起标题（模型一回合只起 ≤12 字 title）→ `story_text`（**携带 `data.seq`**）下发渲染 + 盖章演出（N = 入册流水号只增不减）。**story_list = front 机械 producer 三时点推送**（WS 建连 / 归档成功后 / 删除执行后；`items` = 最新 ≤16 条窗口、`total` = 全量现存篇数，二者独立、`items.length` 不顶替 `total`）——推送触发属行为策略（不进契约）；**引擎侧来源的 `story_*` display 块经 front relay 收紧降级**（story 数据单源 = front 机械推送）。**删除** = 契约 §四 `kidpal_story_delete`（端→网关单向）→ front `story.py::delete_story` **硬删** MD（哨兵头校验 + 路径穿越/无哨兵头拒绝 + 审计日志留痕）→ 重推 `story_list` 对账（`total` −1 可见 = 隐式 ack 载体；seq 不回退）。误判兜底见 §10 决策 25 与 requirements v1.47/v1.49 修订。
```

### 4.3 H2 · §5.1.2 front 网关行（`:185`）

上下文：`:184` = 表头分隔行；`:186` = `| 鉴权 | …`。

```diff
@@ -185,1 +185,1 @@  §5.1.2 服务端组件 · front 网关
- | **front 网关** | `server/gateway/front/front_app.py`（+ `server/gateway/` 冻结模块） | 唯一公网入口：TLS 终结（uvicorn ssl，自签 20 年证书）、HTTP upgrade 层强制鉴权（accept 之前，失败 403）、路由（GET /healthz、POST /command、POST /adaptation/dryrun、WS）、**ws 双泵**（上行透传；下行截获 `kidpal_display_blocks` 侧通道帧 → 机械校验 → 逐块 mcp 帧下发设备）、故事接线（stt 帧喂累积 / listen-stop 归档下发）、提醒轮询循环 |
+ | **front 网关** | `server/gateway/front/front_app.py`（+ `server/gateway/` 冻结模块） | 唯一公网入口：TLS 终结（uvicorn ssl，自签 20 年证书）、HTTP upgrade 层强制鉴权（accept 之前，失败 403）、路由（GET /healthz、POST /command、POST /adaptation/dryrun、WS）、**ws 双泵**（上行透传；下行截获 `kidpal_display_blocks` 侧通道帧 → 机械校验 → 逐块 mcp 帧下发设备）、**故事机械面**（上行泵拦截 `kidpal_session_ctx`/`kidpal_story_delete` 整条消费不下透引擎；`source_page=story` 切累积态；stt 帧喂累积 + 累积态命中收尾词 stop 归档；listen-stop 归档 + 下发 `story_text(seq)`；`story_list` 三时点自推〔建连/归档后/删除后〕；relay 收紧引擎侧 `story_*` 块；120s 超时 sweep）、提醒轮询循环 |
```

### 4.4 H3 · §5.1.2 故事归档行（`:192`，连带现状行）

上下文：`:191` = 课表解析行；`:193` = 提醒投递行。

```diff
@@ -192,1 +192,1 @@  §5.1.2 服务端组件 · 故事归档
- | 故事归档 | `gateway/story.py` | `archive_story` 落盘 `<stories_dir>/<id>.md`（哨兵头 + kidpal-id/date/title）；story_text 最小三键下发 |
+ | 故事归档 | `gateway/story.py` | `archive_story` 落盘 `<stories_dir>/<id>.md`（哨兵头 + kidpal-id/date/title）+ 持久流水号账本 `.seq.json`（`seq=max+1`、只增不减）；`list_stories` = 最新 ≤16 条窗口 + `total` 全量现存篇数；`story_text` = 三键 + 可选 `seq`（additive）；`delete_story` 硬删（哨兵头校验 + 越权/穿越拒绝 + 审计）；`is_finish_phrase` 收尾词检测 + `StoryAccumulator.timed_out_sessions` 120s 纯决策 |
```

### 4.5 H4 · §5.4 故事行（`:261`）+ 新增账本行

上下文：`:260` = 课表行；`:262` = 提醒行。`-` 行为原文，`+` 行 = 替换故事行并**紧随插入** `.seq.json` 账本行。

```diff
@@ -261,1 +261,2 @@  §5.4 数据视图 · 故事
- | 故事 `stories/<id>.md` | `/opt/kidpal/stories/` | front `story.py` 归档 | 持久；删除 = 云端同步删 MD（C5，requirements v1.47）【链路载体待契约】 | 云端持久 | 契约 story_text |
+ | 故事 `stories/<id>.md` | `/opt/kidpal/stories/` | front `story.py` 归档 | 持久；删除 = 云端同步**硬删** MD（requirements v1.47；**链路载体 = 契约 §四 `kidpal_story_delete`**，2026-09-24 已 LANDED） | 云端持久 | 契约 §1.3③ story_list / §1.3④ story_text |
+ | 入册流水号账本 `.seq.json` | `/opt/kidpal/stories/.seq.json` | front `story.py`（原子写、单写者） | 持久；**只增不减**（删除不回退、同 id 重写不变、重启存活；缺失/损坏则从现存文件重建 + 告警、不倒退） | 云端持久 | 契约 §1.3④（`seq` 语义）/ ADR-007 |
```

### 4.6 H5 · §7 契约门在办（`:341`）

上下文：`:340` = 端云协议剖面项；`:342` 空行；`:343` = `---`。

```diff
@@ -341,1 +341,1 @@  §7 接口与契约 · 契约门在办
- - **契约门在办**：CDQ-260913-spec-split-fallout（spec.md 归档断链 + 验收基线重建，HUMAN_APPROVED 在办）；另登记四件随下次契约门裁（2026-09-14 对照 review）：① E1 schedule 视图承载一周总览网格数据（requirements v1.19）；② 「故事删除」动作（requirements v1.47/v1.48，§5.4 数据视图）；③ 「音量调节」动作（requirements v1.44，§5.1.1 MCP 工具面）；④ `kidpal_session_ctx` 端→网关消息（2026-09-14 已定义，随本批契约门落）。
+ - **契约门在办**：CDQ-260913-spec-split-fallout（spec.md 归档断链 + 验收基线重建，HUMAN_APPROVED 在办）；原登记四件（2026-09-14 对照 review）**已销三留一**——① E1 schedule 视图承载一周总览网格数据（requirements v1.19）✅ 已落地（M3-K1 2026-09-21 落契约 §1.3② 周网格；行内原标「在办」为**遗留漂移**，门 1 F3 注记「本笔顺带收」）；② 「故事删除」动作（requirements v1.47/v1.48，§5.4 数据视图）✅ 已落地（契约 §四 `kidpal_story_delete`，2026-09-24 契约门，M4-K1）；④ `kidpal_session_ctx` 端→网关消息（2026-09-14 已定义）✅ 已落地（CDQ-260914 LANDED；front 拦截半 = M4-K1，板端发送半归 M4-K3）；③ 「音量调节」动作（requirements v1.44，§5.1.1 MCP 工具面）**仍在办**。
```

### 4.7 落地方式与边界（如实）

- **本批不落地**：integrator 对 `docs/architecture.md` 无写权（见 §4 头注结构性事实）。delta 以 patch 形态留此，合并后由 **human spec 批**（与 `ARQ-260918` 四处同批搭载）逐字过目 → `git apply` → 回填 `architecture-gate-evidence.md §6` C4 = done。
- **不改 `docs/requirements.md` / `docs/contracts/**`**：前者行为权威已冻结在前、本批零改；后者恒只读（治理批通道专属）。
- **delta 可 apply 性自检**：五行 hunk 的 `-` 原文逐字取自本 worktree `docs/architecture.md` 现行文本（`:185` / `:192` / `:213` / `:261` / `:341`，读取时点 2026-09-24），单命中无歧义（每行全行唯一）；H4 为「替换 + 紧随插入」，与其余四 hunk 互不重叠。

## 5. 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git status --porcelain docs/contracts/` 零输出；`contract-lint` 闸绿（scoped 轮 exit 0） |
| 需求面零写入 | `git status --porcelain docs/requirements.md` 零输出 |
| 架构面未写（如实） | `git status --porcelain docs/architecture.md` 零输出；C4 delta 成文于本件 §4（待 human spec 批） |
| ADR 零改 | `git status --porcelain docs/ADR.md` 零输出；ADR-007 已在（`docs/ADR.md:60`），本包实现与之一致 |
| C4 未覆盖面已申报 | 本件 §4 + `review-packet.md`「未覆盖面申报」节 + `architecture-gate-evidence.md §6` C4 处置注记 |
| workstream append-only | progress.md 仅文件尾追加收口节，既有行逐字未动 |