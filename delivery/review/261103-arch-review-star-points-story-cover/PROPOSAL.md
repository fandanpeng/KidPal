# PROPOSAL · 261103-arch-review-star-points-story-cover

> **状态：FROZEN v1.0 · 2026-11-04**（FROZEN 凭据 = spine human 触点①裁定 + R1 四查 BLOCK(F1)→v0.2 定点修订 + R2 靶向复核 GREEN；判档主路径 Track B、契约四分类 ACCOMPANIES_SPEC_CHANGE 沿 R1 裁定生效；作者≠判官由 R1/R2 独立审链承载，契约无人签门、合并过机械凭据闸。立项依据 = spine `delivery/intake/261103-star-points-story-cover.md` facePlan 顺序 2、human 触点①与需求面 v1.71 frozen。）
> **修订注记（R1 F1）**：v0.2 仅把短窗阈值改为 requirements S-F6.3 AC5 回引并同步 §6 留根指针，其余内容不变。
> **承载 ARQ**：`ARQ-260918-spec-face-3page-reconcile` 由本批改写 architecture.md 撤查字页前旧四页叙述而消化，实际结项由主线程在落地后按 consumed-by 处理。根来源 = 上述 spine、workstream review R-01~R-13 与 frozen requirements v1.71。
> **review 轨**：Track B；architect 预判**主路径**——触契约、改 §5.x 组件/场景/数据机制并新增 ADR，轻路径判据①②③④均不满足；最终判档归 architect-reviewer，只升不降。Track A 不触发：方向与三项开放问题均已由 human 裁定。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T10）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **landing-lane**：`packet-bound`。canonical 落地归主线程；本起草只写本 PROPOSAL，不直改 architecture 或 contracts。
> **riskTier**：medium（新增端云持久数据域、两类 display view、控制动作与三条私有消息，并重排 P3/P4 导航；本批本身仅三份 canonical/决策文档，不碰实现和 live 数据。）
> **review profile**：架构面 + 契约面（触契约分支，显式标注）+ ADR；G1/G2/G3 治理面 = 零触。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/architecture.md
docs/decisions/ADR.md
docs/contracts/interface-contract.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 frozen requirements v1.71 的星星积分本与故事封皮落成可实现的架构和端云契约——云端账本与目标配置只有一个确定性写者，设备语音和 QQ 共用同一命令执行器，front 以权威推送驱动 P3 积分主页和可全量分页的详细账本，故事模块则以 P4 封皮为入口并复用 `story_list.total` 作为封皮计数。

**现状差距**：

1. architecture 仍是三页设备组件和七个 display view，front 只有 story/schedule 两类机械 producer；没有积分账本、目标配置、QQ 积分通道或完整账本分页机制。
2. 契约没有积分下行形状、记账/兑换薄意图、设备删除和分页上行消息；`kidpal_session_ctx` 也不能区分积分主页、详细账本和故事封皮。
3. `story_list` 仍被写成“切到 P3 列表”，不能同时支撑 P4 封皮计数、列表缓存和“打开故事只到封皮”。`char` 与 `story_text` 的现役板子行为句也仍引用旧页位。
4. `story_page.c:2285-2297` 以数值 `case 2` 映射故事语境；插入 P3 积分后会把积分页误报为故事记录语境，必须改为页枚举加子页状态驱动。
5. architecture §5.1.1 仍有撤查字页前旧四页叙述，§9 仍把“三页循环”列为未来目标，头部仍写 v1.71 updating；这些与本批 frozen 需求不一致。

**非目标（本批不做）**：

- 不改 `docs/requirements.md`、glossary、UI brief、v9 母版或 V9 PNG；行为语义与视觉权威已经 frozen。
- 不改固件、server、OpenClaw skill、部署配置或测试实现；K0–K5 全部由 dev-loop 承载。
- 不引入新外部依赖、多设备/多孩子身份、本地离线写队列、设备语音删除、惩罚性扣分或越目标庆祝演出。
- 不重开引擎 Intent，不扩 kidpal 工作区写权，不让 LLM 直接写账本或生成余额/目标/分页权威载荷。
- 不借本批结项 TECH-261030、TECH-261023、ARQ-260907 或 TECH-261022-p3-record-session-state-churn。

**spec 关系总说明**：原 workstream D-01/D-02/D-08 的“需修订”均已被 requirements v1.71 满足，本批没有遗留的“需修订”裁定，也不新增 launchPreconditions。下述 wire 字段名、存储位置、游标和组件接线标为 `spec-silent`；行为语义均标 `within-spec`。若 reviewer 发现必须改变 story 语义，须退回需求面并把新前置条件登记到 spine，不在本批暗改。

---

## 2. 问题、根因与改法（T1…T10 + 契约包）

### T1 · 设备组件改为四页循环、P3 积分子页与 P4 故事模块，语境映射改枚举驱动

**问题**：architecture §5.1.1 只有首页/课表/故事三类页面，view 路由只有七个 view，按键和 MCP 工具面也没有积分或封皮。更严重的是现役 `kidpal_story_ctx_source_page` 用数值 `case 2` 判断故事页，插入积分页后会产生错误的记录语境。

**证据**：

- `docs/architecture.md:177-185` 仍写旧四页顺序、三页修复项和 7 view；requirements v1.71 §2.3（`docs/requirements.md:137-161`）已冻结 P3 积分主页/详细账本、P4 封皮/列表/阅读全矩阵。
- `firmware/main/display/kidpal_render_node.h:32-37` 当前为 HOME/SCHEDULE/STORY/COUNT=3；`firmware/main/display/story_page.c:2285-2297` 逐字以 `case 2` 返回 `story`。
- `firmware/main/application.cc:1313-1320` 是每次会话发送 `kidpal_session_ctx` 的单点；S-F6.3（`docs/requirements.md:725`）只钉积分语境意图，载体归本批。

**根因**：旧模型把“顶层页序号”与“故事模块身份”当作同一个数值。新模型有主页面和模块子页两层，必须由统一页枚举加子页状态共同决定键归属、页点和会话语境，不能继续平移数字。

**改法**：保留 §5.1.1 标题，替换渲染节点、页面组件、view 路由、按键路由和 MCP 工具面五行，新增积分页与故事封皮职责，并在表后追加语境映射约束。`spec 关系 = within-spec`（页结构）+ `spec-silent`（组件拆分与映射实现）。

拟落地文本：

```text
| **渲染节点层** | `display/kidpal_render_node.cc` | 归一屏收口（唯一顶层屏）；主页面枚举统一为 `KP_PAGE_HOME → KP_PAGE_SCHEDULE → KP_PAGE_POINTS → KP_PAGE_STORY_COVER`，`KP_PAGE_COUNT=4`，`cycle_next()` 仅在这四个主页面间循环。故事列表/阅读/记录与积分详细账本是模块子页，不进入主页面取模序列；查字窗仍为覆盖窗。 |
| 页面组件 | `home_p1.c` / `schedule_page.c` / `points_page.c` / `story_page.c` / `story_cover.c` / `char_page.cc` / `reminder_page.c` | `points_page` 承载 P3 积分主页、详细账本、删除确认、分页游标与 RAM 缓存；`story_cover` 承载 P4 封皮；`story_page` 承载 P4 模块内列表/阅读/记录/整理/盖章及逐层返回；reminder 与 char 的覆盖层语义不变。 |
| view 路由 | `display/view_router.c` | 9 view 白名单路由（text/home/control/char/schedule/story_list/story_text/points_home/points_ledger），UNKNOWN 拒绝；`story_list` 是 P4 模块共享数据帧，不再等同“切到列表”。 |
| 按键路由 | `key_gesture_parser.cc` + 板装配层 + `keybar.c` | 以统一 `kidpal_page_id_t` 和子页状态分流，不再另维护一组平行数值。P3 主页 KEY 短按开会话、双击进详细，详细账本 BOOT 浏览/长按删除；P4 封皮 KEY 进列表，列表/阅读逐层返回。keybar 中槽恒为 4 点，P3 子页保持第 3 实心、P4 子页保持第 4 实心，覆盖层不显示页点。 |
| MCP 工具面 | `mcp_server.cc` | `kidpal/display` 工具说明与路由扩至 9 view；积分权威帧只更新 RAM 缓存并按 `restore` 决定是否接管前台；删除账本分录经 §四 `kidpal_points_delete` 上行，分页经 `kidpal_points_page` 上行，均不走 MCP display 反向调用。 |

会话语境映射单点仍由 application.cc 会话发起入口消费，但改由 `KP_PAGE_*` 符号与当前子页状态计算：HOME→`home`，SCHEDULE→`schedule`，POINTS 主页→`points_home`，POINTS 详细账本→`points_ledger`，STORY_COVER 封皮→`story_cover`，P4 故事列表→`story`。落地时把 `story_page.c:2285-2297` 的 `kidpal_story_ctx_source_page(int page)` 改为中性的 `kidpal_session_ctx_source_page(page, substate)`（可迁入中性模块），application.cc 单点同步传入两个维度；禁止只把 `case 2` 改成 `case 3`，也禁止以其它数字字面映射。未知或越界输入回退 `home` 并记日志，绝不能误回 `story`。`story` 只从 P4 故事列表产生记录语境，`points_*` 与 `story_cover` 均为普通问答语境，其中 points 由 front 注入账本摘要。P4 阅读、删除确认、整理过渡等需求未开放会话入口的状态不得发送 `kidpal_session_ctx`，不能借新映射额外开入口。
```

`TECH-261022-p3-record-session-state-churn` 与本批共享 keybar/story_page 文件，K5 必须在其后串行对账，本批不重复承接该条目的会话状态改造。

目标文件：`docs/architecture.md`（改）

### T2 · 服务端以 points.py 确定性单 writer 统一语音与 QQ，front 负责校验、执行接线与权威推送

**问题**：requirements 要求设备语音与 QQ 同构记账，但现有 front 和 QQ skill 没有账本组件。若两条通道分别解析、写文件或算余额，会形成双写者、并发覆盖和重复扣款；若让 LLM 直接写状态，又会违反 kidpal 数据只读与网关恒薄边界。

**证据**：

- workstream review R-05/R-07 要求 D-10 单 writer、短窗去重与传输幂等分离，QQ 不依赖 `ask_user`。
- `server/gateway/display_schema.py:26-48` 只有 7 view 和既有控制动作；`server/gateway/front/front_app.py:568-626` 已有 front 自推和 restore 先例。
- `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md:58-115` 已有“QQ→确定性机检→即时刷新”的工程形态，但课表的直写文件规则不能照搬到积分。
- `TECH-261023-openclaw-tool-face-prompt-only-readonly` 要求 kidpal 数据目录只读；requirements S-F6.3/S-F6.4（`docs/requirements.md:717-743`）只允许薄意图后形成真实分录。

**根因**：账本命令的语义抽取与确定性执行是两种职责。前者可由模型完成，后者必须在同一实现内串行完成校验、重算、持久化和回执，任何第二份“便捷实现”都会成为新的真相源。

**改法**：新增 `server/gateway/points.py` 作为账本与目标配置的唯一命令执行器和状态写者；设备语音由 front 调其 Python API，QQ 由新 `kidpal-points-write` skill 调同一模块的 CLI。front_app、QQ skill 与 LLM 均不得另写余额或直接改状态文件。`spec 关系 = within-spec`（共享规则与只读边界）+ `spec-silent`（模块、文件与 CLI 形态）。

在 §5.1.2 新增并修订以下组件行：

```text
| **积分账本与目标配置** | `server/gateway/points.py` | 唯一确定性 writer。以进程间锁串行执行命令，在同一临界区内读取最新状态、校验、重算、原子替换并保存命令回执；余额与每条分录的变动前后余额全部由剩余分录派生。设备语音经 front API、QQ 经同模块 CLI 进入，两个入口共享全部不变量，不各写一套。 |
| **QQ 积分 skill** | `server/deploy/openclaw-skills/workspace-main/kidpal-points-write/SKILL.md` | main agent 把家长自然语言整理成一次性可判命令后只调 points.py CLI，使用真实执行结果回复；不得直接编辑 `/opt/kidpal/points/`，不得写 schedule.md 或 workspace-kidpal 数据，不调用 `ask_user`。歧义命令用普通 QQ 文本报错并给可重述格式，删除以稳定记录 id 或最近列表序号定位。 |
| display 校验 | `gateway/display_schema.py` | 9 view 白名单 + 积分 shape floor + 来源门禁。points_home/points_ledger 的完整载荷仅 front 自推可下发；引擎 display 流只能携带 `{}` 导航薄载荷。`add_points` / `redeem_points` 由 front 截获执行，引擎来源的同名列举不进入设备控制路由；front 自推的权威 points 帧走既有 self_push 通路。 |
```

在 front 组件行末追加以下职责，不改写既有 story/schedule 段：

```text
**积分面**：front 校验语音薄意图并为每次逻辑命令建立稳定 `command_id`，调用 points.py 执行后按真实结果答复与推送；QQ 侧由 front 观察 points.py 状态 `revision` 变化并推送同一权威视图。WS 建连、QQ 写入、删除与目标切换的积分推送均为恢复性数据更新，只填缓存不抢前台；设备主动语音记账/兑换成功或短窗去重命中时例外，主动展示积分主页。front 不做第二份余额算法，所有成功/拒绝均以 points.py 回执为准。
```

§2.4 原则 3 追加同型切分注记：积分语义理解在脑，命令校验、余额重算、目标配置不变量和原子持久化是 gateway 可机判执行层，与 ADR-002/ADR-004 的混合分层同型，不解释为允许 front 或 QQ 各自发明业务规则。§5.1.3 增加 `front / QQ skill → points.py（同一执行器）→ /opt/kidpal/points/state.json` 的单向依赖，§5.3.2 交接目录增加该状态文件及其锁文件；OpenClaw 主 agent 不直接持有状态写路径。

**新机制 spike 申报**：本变更首次建立“语音/QQ 薄意图 → 云端持久账本 → front 权威推送 → 设备积分页与删除消费”的端到端数据通路，命中 spike-first。K0 必须位于 K1–K5 最前：真实设备语音与真实 QQ 各写一笔，设备实际看到余额和事由；QQ 在其他页只更新数据不抢前台；最早/最新两条 canary 证明完整账本消费不止最近一条；证据落 K0 `spike-evidence.md`。本架构文档 GREEN 不替代该 spike。

目标文件：`docs/architecture.md`（改）

### T3 · 运行视图新增积分端到端场景，并把故事场景④改为 P4 模块

**问题**：现有五个运行场景没有语音/QQ 记账、兑换、删除、目标切换与建连恢复时序；场景④仍假设 P3 故事列表是顶层页，并称归档/删除后的 `story_list` 为主动接管前台。这与 v1.71 的 P4 封皮、逐层返回和“打开故事只到封皮”不一致。

**证据**：`docs/architecture.md:211-225` 的场景④仍写 P3 上下文、P3 在屏重绘和归档/删除主动推送；requirements S-F2.6（`docs/requirements.md:516-529`）、S-F6.1~S-F6.5（`:679-758`）已给出新行为权威。`docs/contracts/interface-contract.md:105-110` 也有同样的旧消费假设。

**根因**：旧场景把数据帧和页面跳转绑定在一起。新导航要求先把 story/points 数据更新与前台接管分开，再由显式打开意图决定落到封皮、积分主页或详细账本。

**改法**：场景④只改 P3→P4 及 story_list 生产纪律，保留记录语境、整理、归档、删除与丢弃的既有机制；新增场景⑥覆盖积分全链。`spec 关系 = within-spec`。

场景④的 story_list 段替换为：

```text
**story_list = P4 模块共享数据帧**。front 的所有后台恢复/对账推送（含 WS 建连、记录语境建立、归档成功后和删除执行后）均以 `data.restore:true` 推送，只更新封皮与列表共用的 RAM 缓存、不接管前台；封皮与列表都以 `total` 渲染现存篇数，不另造故事计数源。孩子明确说「打开故事」时，OpenClaw 只发 `story_list` 空 data 导航薄载荷，front 展开完整载荷且不带 restore，设备据此进入 P4 封皮而非跳过封皮进列表。归档后的盖章与返回列表由 story_text/本地模块状态承接，不再靠 story_list 主动抢前台。`items` 仍是最新 ≤16 条窗口，`total` 仍是全量现存篇数；窗口外旧条目的既有边界不因本批扩张。front 原“relay 收紧 story_* 块”句相应收窄：story_list 仅空 data 导航薄载荷可由 front 截获展开，非空 story_list 与任意 story_text 的引擎来源仍拒转。
```

新增运行场景⑥：

```text
**⑥ 积分记账、查账与更正**：
A. 设备语音：设备在积分主页/详细账本发起会话时先送 `kidpal_session_ctx`（points_home / points_ledger，详细账本可带 selected_id）→ front 从 points.py 取当前余额、激活目标与被选分录，经既有内部注入通道形成有界账本摘要 → ASR/OpenClaw 只产 add_points / redeem_points 薄意图 → front 校验并生成稳定 command_id → points.py 串行执行 → front 按真实回执播报结果并推送 points_home。成功或短窗去重命中时该主动语音路径展示 P3 积分主页；不完整句不产动作、只走普通对话。
B. QQ：main agent 经 kidpal-points-write 把每条家长消息整理为一次性可判命令并调 points.py CLI → 同一执行器返回成功或普通文本可转述的错误 → front 发现 revision 变化后推送 points_home（以及删除时的 points_ledger 首页）restore 载荷。设备在其他页不抢前台，积分页在屏时原地刷新。任何路径不得调用 ask_user。
C. 冷启动与完整账本：WS 建连时 front 推送 points_home 和 points_ledger 首页 restore 载荷。KEY 双击或「打开详细账本」时设备发 kidpal_points_page 首页请求，front 回首页帧并进入 P3 详细账本；后续翻屏发 next_cursor，front 以 keyset 游标回下一页 restore 载荷。revision 不一致或游标失效时回最新首页并令设备清空旧分页缓存，不静默拼接不同版本。
D. 删除：设备整屏确认后发送 kidpal_points_delete(id) → points.py 删除该分录、重算全部余额与变动前后余额 → front 重推 points_home 与 points_ledger 首页 restore 对账。重复/已不存在 id 幂等 no-op 仍重推；非法 id 拒绝并审计。QQ 删除走同一执行器。
E. 目标配置：QQ 新增/切换/删除自定义目标均走 points.py。预置场景不可删，激活中的自定义目标不可删，必须先切换；自定义分值只接受 5–200 正整数。切目标只改激活配置，不自动产生兑换分录，历史兑换名目保持原快照。
```

失败边界作为场景⑥末段逐字落：断网或云端不可达时设备写操作给可见失败且不本地排队；QQ 消息无法到达云端时同样失败，不由模型假报成功。句式不完整不执行；兑换分值不等于当前激活目标或余额不足时拒绝且不写账；同一 `command_id` 重放返回首次结果、不新增分录；短窗语义去重命中不新增分录但按 S-F6.3 AC5 回应并跳积分页；持久化失败不得推送“成功”视图。QQ 歧义只普通报错，绝不挂起等多轮澄清。

目标文件：`docs/architecture.md`（改）

### T4 · 数据视图登记账本/目标/缓存，短窗语义去重与传输幂等分离

**问题**：现有数据视图没有云端积分状态位置与 owner，§6.5 只覆盖提醒幂等。完整账本还涉及删除后的历史余额重算、跨进程互斥、稳定分页和断网边界，不能只写“存一个积分文件”。

**证据**：`docs/architecture.md:269-284` 无积分行，`:341-351` 只允许 RAM 渲染缓冲和提醒幂等；requirements S-F6.2 AC4（`docs/requirements.md:704`）要求删除后重算并允许仅该路径出现负余额，S-F6.3 AC5（`:721`）要求短窗去重，NFR-OFF-1（`:817-823`）禁止本地补记。R-04 明确禁止照搬 story_list 的 16 条窗口。

**根因**：账本既有长期状态又有可重算派生值，必须把“命令重试”“用户重复说”“删除后重算”分成三个机制，否则一个去重布尔会同时吞掉合法重试与合法新分录。

**改法**：§5.4 新增三行，§6.2 补积分边界，§6.5 新增积分段。`spec 关系 = within-spec`（余额、负余额、离线）+ `spec-silent`（文件、游标、命令标识）。

§5.4 新增：

```text
| 积分账本与目标配置 `points/state.json` | `/opt/kidpal/points/` | 仅 `points.py`（front API 与 QQ CLI 共用） | 云端持久；保存分录、激活目标、自定义目标、revision 与命令回执；预置四场景为实现内不可删常量，出厂激活游乐场 40 | 云端唯一真相源 | requirements F6；端云显示形状 = 契约 §1.3⑦⑧ |
| 积分写入互斥 `.lock` | `/opt/kidpal/points/` | `points.py` 进程间锁 | 仅执行期同步原语，不是业务事实源 | 不作为余额或配置来源 | §6.5 积分段 |
| 积分渲染缓存 | 设备 RAM | 固件 | 缓存 points_home、已取账本页、revision、游标与当前选中 id；掉电即失，禁止写 NVS/SD | 重启后靠恢复推送和按需分页重建 | 契约 §1.3⑦⑧ / §四 |
```

`state.json` 的内部存储结构由 points.py 单源拥有，不在端云契约复制一份。逻辑事实包括：稳定分录 id、带正负号的 delta、事由、发生时间、变动前后余额、产生通道与 command_id；激活目标 id、自定义目标、revision 和命令回执。余额恒为剩余分录 delta 之和；每次删除后按时间顺序重算所有后续分录的 balance_before/balance_after。预置目标删除拒绝，激活自定义目标删除拒绝，目标切换不改历史分录。

§6.2 追加：积分账本、目标配置、分录副本和离线写队列均不得落设备非易失介质；RAM 中的权威帧渲染缓存不属于持久业务副本。设备写失败不本地暂存，恢复后只从云端重取，不做设备侧冲突合并。

§6.5 新增积分段：

```text
**积分链路的两类去重分离**：
1. **传输幂等**针对同一逻辑命令的重试。channel adapter 为命令提供稳定 `command_id`；QQ 取平台消息 id 加命令序号，语音由 front 以连接/会话和相关回合序号建立。points.py 在同一锁临界区查询并保存命令回执，同 id 重放返回首次结果，不新增分录、不递增 revision。新的用户回合即使文案相同也必须使用新的 command_id，不能借传输幂等扩大去重窗口。
2. **短窗语义去重**针对 S-F6.3 AC5 的重复表达。points.py 以可注入时钟判断短窗（起步值与调优定数的权威 = requirements S-F6.3 AC5，调优后回填），以相同正负分值和保守归一化后的事由匹配；归一化只去空白/标点、统一全半角和不改变事由的时间前缀，不引入模型相似分或模糊阈值。命中后返回“已记过”回执和当前权威余额，不新增分录；它与 command_id 无关，窗口外同文案仍是合法新命令。若真机验收证明「事由相近」需要更宽的模糊语义，回 /arch-change 评估，不在 Pod 内自调相似度。
3. **分页一致性**采用 `(occurred_at, id)` 时间倒序 keyset 游标，不用 offset。游标携带最后一条排序键与 revision；插入或删除使 revision 变化时回最新首页并要求设备重置分页缓存，防止跨版本重复/漏项。每页 total 始终为当前全量分录数，items.length 不顶替 total。
```

§4 NFR-OFF-1 对策行补“积分只驻云端、设备无离线写队列”；新增 NFR-PERF-4 行，指向 §5.1.2 积分组件、§5.2 场景⑥和 §6.6 日志，阈值仍只回引 requirements、不在架构写数。

目标文件：`docs/architecture.md`（改）

### T5 · 资产、UI 态与 prompt 静态语义登记

**问题**：§5.5 尚未登记书封、四预置场景、自定义通用标记、积分主页/账本/封皮 UI 态和积分 prompt 纪律。若不写清静态语义承载，K2/K3 容易分别发明 view 白名单和“模型直接报余额”的第二通道。

**证据**：`docs/architecture.md:295-323` 只有既有资产与 prompt；V9 已有 `points-home` 四态、`points-ledger`、`points-delete-confirm`、`story-cover`，以及 `sc-choco` / `sc-movie` / `sc-wheel` / `sc-gift` / `sc-flag` / `ic-coin` 符号。requirements S-F6.5 AC1/AC2（`docs/requirements.md:749-756`）冻结预置和自定义资产语义。

**根因**：UI 和 prompt 是两条独立消费线，必须分别指到 v9 视觉权威和 workspace 静态语义单源，不能把像素或 prompt 全文复制进 architecture。

**改法**：§5.5.2 追加资产登记，§5.5.4 追加 UI 态，§5.5.5 追加积分静态语义接线。`spec 关系 = within-spec`（视觉和生成纪律）+ `spec-silent`（prompt 接线）。

拟追加文本：

```text
**积分与封皮资产登记（261103-star-points-story-cover）**：书封与大星徽 `ic-coin`、四预置场景 `sc-choco`（巧克力 20）/ `sc-movie`（看电影 30）/ `sc-wheel`（游乐场 40）/ `sc-gift`（礼物 50）和自定义通用旗标 `sc-flag` 均复用 §5.5.2 预栅格化管线；`pt-cat` 复用母版头部资产，不另造吉祥物。目标配置只存上述稳定资产标识，设备不根据奖励名猜图。字体现有 16px 正文、20px 标题与 14px 键帽档足够，本批不新增字体或分区。
```

```text
**积分与封皮 UI 态**：`points-home` 含正常、空账、数据未达、负余额四态；`points-ledger` 为全量可及的分屏列表，`points-delete-confirm` 复用故事删除确认同族覆盖层；`story-cover` 为 P4 模块入口。布局坐标与 1bpp 形态只指 UI-DESIGN-VERSIONS current 对应的 v9 / V9 帧，不在本文件复制像素。数据未达不是“空账”：前者没有权威帧、不得显示余额或进度线，后者已收到 `balance=0, recent=[]` 的权威帧并照常画目标。
```

```text
**积分静态语义**：workspace-kidpal/AGENTS.md 的 view 白名单由七选一改九选一，新增积分回合与 points_home / points_ledger 导航薄载荷、add_points / redeem_points 两类薄意图。模型不得生成余额、目标分值权威值、分录 id、变动前后余额、revision、游标、资产标识或成功回执；只说中性承接语，实际结果由 front 播报。完整句式缺少事由/动作/分值时不产动作，兑换不产目标 id。main workspace 的新 QQ skill 同样只调确定性 CLI，并显式禁止 ask_user 与直接文件写入。上述内容仍由 §5.5.5 的静态单源和部署三面相等承载，不放入动态 context，不新建 prompt 家。
```

目标文件：`docs/architecture.md`（改）

### T6 · 契约 §1.3 新增 points_home / points_ledger，story_list 改为 P4 模块共享数据帧

**问题**：契约七个 view 无法表达积分余额、激活目标、最近三条、全量账本分页与空态；story_list/story_text/char 的现役板子行为句还停在旧 P3/P4 页位。若只加 view 名而不写 producer 纪律，LLM 仍可能直接生成积分载荷。

**证据**：`docs/contracts/interface-contract.md:44-163` 无积分形状，`:92-113` 把 story_list 绑定 P3 列表，`:63` 仍写“切到 P4 查字页”；requirements S-F6.1 AC4（`docs/requirements.md:687`）区分空账与数据未达，S-F6.2 AC1（`:701`）要求全量可及，S-F2.6 AC4（`:524`）要求打开故事只到封皮。

**根因**：现有契约把“数据类型”“页面身份”“前台行为”混在一个 view 句里。新形状必须分别定义权威数据、导航薄载荷和 restore 消费，才能复用同一帧服务封皮、列表和后台恢复。

**改法**：新增⑦⑧两节并修订①③④的现役行为句。`spec 关系 = within-spec`（显示与导航行为）+ `spec-silent`（字段名与 JSON 形状）。

① `char` 的板子行为句完整替换为：

```text
板子行为：在当前宿主页上弹出会话内查字覆盖窗，显示静态完整示范字、拼音、笔画与释义；KEY 短按起播/重播笔顺动画（沿 medians），长按或 BOOT 关窗回原页且不翻页。查字窗不占主页面页位，也不显示页点；详细交互权威 = requirements §2.2/§2.3 与 F3。
```

③ `story_list` 的板子行为、`total`、`items`、`restore` 与生产纪律四段替换为：

```text
板子行为：本 view 是 **P4 故事模块共享数据帧**，同时服务封皮计数与故事列表，不再等同“切到故事列表”。`data.restore:true` 时只更新封皮/列表共用 RAM 缓存、不接管前台；封皮或列表在屏时各自原地重绘。不带 restore 的完整帧只用于 front 展开「打开故事」导航薄载荷，设备据此进入 **P4 故事封皮**，不得跳过封皮直进列表。

- **`total`**（可选，整数）：现存篇数 = 星星计数，由云端计算下发；P4 封皮和 P4 故事列表共用此值，禁止另造封皮计数源。缺省时板子保留向后兼容的 `items.length` 回退，front 机械推送恒带 total。
- **`items`**：仍为最新 ≤16 条窗口、日期倒序，服务故事列表既有消费；`total` 不受窗口限制。此 16 条窗口只属 story_list，不得复用为积分“完整账本”的边界。
- **`data.restore`**（可选布尔）：缺省/false = 显式打开故事模块，进入 P4 封皮；true = 后台数据更新，只填缓存不抢前台。入板解析失败仍按既有静默降级与 front V27 双层防线处理。
- **生产纪律**：完整 story_list 只能由 front 产生。所有后台恢复/对账推送（含 WS 建连、记录语境建立、归档成功后、删除执行后）恒带 `restore:true`；只有 front 展开「打开故事」的 `{view:"story_list",data:{}}` 薄载荷时不带 restore。OpenClaw 仅可为该显式导航发送空 data 薄载荷，禁止生成 total/items/restore 或任何完整故事列表。归档后的盖章与返回列表由 story_text 和本地模块状态承接，不靠 story_list 抢前台。§1.5 的 relay 规则同步收窄：空 data story_list 由 front 截获展开，非空 story_list 与任意 story_text 的引擎来源仍拒绝。
```

④ `story_text` 板子行为首句的“P3 进阅读态”改为“在 P4 故事模块内进阅读态”，其余 seq、fresh_save 与 fetch 语义不变；§四 `kidpal_story_fetch` 的触发语“进 P3 列表”同步改“进 P4 故事列表”。

新增⑦：

```text
#### ⑦ `points_home` —— P3 星星积分本主页

{
  "view": "points_home",
  "data": {
    "revision": 12,
    "balance": 23,
    "target": {
      "id": "preset-playground",
      "name": "游乐场",
      "points": 40,
      "asset": "sc-wheel"
    },
    "recent": [
      {
        "id": "pt_01J...",
        "delta": -40,
        "reason": "兑换游乐场",
        "occurred_at": "2026-11-03T20:00:00+08:00",
        "balance_before": 63,
        "balance_after": 23
      }
    ],
    "restore": true
  }
}
```

上例在 canonical 中放入 `json` 围栏。字段约束：`revision` 为单调递增整数；`balance`、`balance_before`、`balance_after` 为可正可负整数；`delta` 正数表示加分、负数表示兑换；`target` 恒为当前唯一激活目标，`points` 为正整数，`asset` 只能取 `sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag`；`recent` 为时间倒序最多三条、复用统一分录形状，`occurred_at` 为带时区 ISO8601，设备按本地日期生成“今天/昨天/MM-DD”。

- **空账与未达**：已收到权威帧且 `balance=0,recent=[]` 是确认空账，设备显示 0 与空账话术并照常画目标；“数据未达”不是载荷枚举值，而是设备尚未收到任何权威 points_home 帧，此时不得显示余额数字或进度线。不得用 `balance:null` 或伪造 0 表示未达。
- **restore**：缺省/false = 主动展示 P3 积分主页；true = 只填 RAM 缓存不抢前台，P3 主页在屏时原地重绘。设备主动语音记账/兑换成功或短窗去重命中由 front 发不带 restore 的帧；WS 建连、QQ 写入、删除与目标切换均发 restore:true。
- **生产纪律**：完整载荷只能由 front 依据 points.py 当前状态产生。OpenClaw 只在「打开积分本」时发 `{"view":"points_home","data":{}}` 导航薄载荷，front 展开为完整帧且不带 restore；模型不得生成上述任何权威字段。负余额只能来自删除重算后的 points.py 状态，front 不另做钳位。
```

新增⑧：

```text
#### ⑧ `points_ledger` —— P3 详细账本分页

{
  "view": "points_ledger",
  "data": {
    "revision": 12,
    "balance": 23,
    "items": [
      {
        "id": "pt_01J...",
        "delta": -40,
        "reason": "兑换游乐场",
        "occurred_at": "2026-11-03T20:00:00+08:00",
        "balance_before": 63,
        "balance_after": 23
      }
    ],
    "page": {
      "total": 25,
      "next_cursor": "opaque-cursor-or-null",
      "has_more": true,
      "reset": true
    },
    "restore": true
  }
}
```

上例同样以 `json` 围栏落地。`items` 使用与 points_home.recent 相同的分录形状、时间倒序；`page.total` 是当前全量分录数，不能用 items.length 代替。`next_cursor` 为 front 生成的不透明 keyset 游标，结束时空且 `has_more=false`；`reset=true` 表示设备先丢弃已缓存分页再装入本页，首页与版本重置响应均为 true，正常续页为 false。

- **空账与未达**：权威空账帧为 `items:[],page.total=0,balance=0`；数据未达仍是没有收到任何权威 points_ledger 帧，不发 `balance:null`。
- **restore**：首次显式打开详细账本的应答不带 restore，设备进入 P3 详细账本；续页、WS 建连恢复、删除后重推均带 restore:true，只更新缓存/当前账本页、不抢其他页面前台。设备主动语音「打开详细账本」由 OpenClaw 发 `{"view":"points_ledger","data":{}}`，front 以首页完整帧展开。
- **生产纪律**：完整载荷只能由 front 产生，模型不得生成 items/balance/revision/page/restore。分页请求—应答、游标失效与版本重置的 wire 规则见 §四 `kidpal_points_page`；本 view 不继承 story_list 的 16 条窗口。
```

目标文件：`docs/contracts/interface-contract.md`（改）

### T7 · 契约 §1.4/§1.5/§1.6 定义记账薄意图、front 机械执行与生成纪律

**问题**：现有控制动作只有会话和故事控制，无法表达“加 5 分”或“兑换当前目标”。front 白名单和 system prompt 也没有积分来源门禁，模型可能虚报写入成功或直接生成余额。

**证据**：`docs/contracts/interface-contract.md:165-194` 无积分动作与 prompt；`start_organizing` 已提供“网关决定类、引擎 display 流拒转”的先例。requirements S-F6.3 AC1~AC5（`docs/requirements.md:717-722`）冻结句式、金额与去重边界。

**根因**：记账不是渲染命令，而是需要 front 执行的薄意图。若把它当普通 control 透传到板子，就会绕过唯一 writer；若让模型说“已经记好了”，执行失败时又无法撤回。

**改法**：新增两个网关决定类动作，front 拦截后调 points.py，不把动作帧下发设备；权威结果以 points_home/points_ledger 表达。`spec 关系 = within-spec`（行为）+ `spec-silent`（动作名与字段）。

§1.4 表新增：

```text
| `add_points` | 完整加分句的薄意图：`data={"action":"add_points","points":5,"reason":"今天扫地"}`；points 为正整数，reason 为非空事由 | **不直接下发设备**。front 截获、建立 command_id 并交 points.py 执行；按真实结果播报并推送 points_home。缺事由/分值、points 非正整数或句式不完整时拒绝执行，引擎 display 流中的完整 points_home 不得替代此执行链。 |
| `redeem_points` | 完整兑换句的薄意图：`data={"action":"redeem_points","points":40}`；points 必须等于当前激活目标分值 | **不直接下发设备**。front 交 points.py 校验当前目标与余额，成功后由执行器生成负向分录，事由统一取“兑换<当前目标名>”；金额不匹配或余额不足拒绝且不写账。模型不提供 target id，不直接提供负 delta，不宣称已兑换。 |
```

两动作与 `end_session/start_organizing` 同属网关决定类：引擎 display 流可携带其薄意图供 front 截获，但不得经 relay 进入板端控制路由；front 自推通路也不把这两个 action 发给设备。设备只消费执行后的权威积分视图。

§1.5 第 3 条白名单改为 `char/schedule/story_list/story_text/text/home/control/points_home/points_ledger`，并追加：points 两个 view 的完整形状 floor 校验（整数余额与前后余额、target 四键、分录六键、page 四键、restore 可选布尔）；完整 points 帧仅 front 自推，引擎侧完整 points 帧拒转，空 data 导航薄载荷由 front 展开。story_list 的 shape floor 增认 `data:{}` 导航薄载荷，非空完整帧仍按 total/items/restore 校验；其 relay 规则同步收窄为空 data 可截获展开，非空 story_list 与任意 story_text 的引擎来源仍拒转。`add_points/redeem_points` 不走板端 relay。

§1.5 新增第 7 条：

```text
7. 积分面机械职责：front 把 add_points / redeem_points 薄意图交给 points.py 同一确定性执行器；QQ skill 也仅调该执行器 CLI。front 不直接编辑状态文件、不另算余额，负责 command_id 接线、结果答复、权威 points_home/points_ledger 推送、删除重算后的重推对账，以及 WS 建连恢复。命令校验、余额重算、目标配置不变量和原子写是机械执行，不是 LLM 业务生成，零自研第二套业务逻辑红线不变。
```

§1.6 的 system prompt 引文完整替换为：

```text
你是儿童学习伙伴。每次回复先用口语化、适合 8–12 岁孩子的话回答，再在末尾附一个 display 代码块。view 取 char/schedule/story_list/story_text/text/home/control/points_home/points_ledger 之一。涉及课表时只发 schedule 空 data 导航薄载荷，完整网格由 front 展开。涉及星星记录本时不得生成完整 story_list/story_text；只有孩子明确说「打开故事」时才可发 story_list 空 data 薄载荷，由 front 展开并进入 P4 封皮。涉及积分时，「打开积分本/打开详细账本」只发 points_home/points_ledger 空 data 薄载荷；完整加分句只发 control/add_points {points,reason}，完整兑换句只发 control/redeem_points {points}。缺少事由、动作词或分值时不产记账动作，只说普通答复。不得生成余额、激活目标、最近记录、分录 id、前后余额、revision、游标、资产或 restore，不得宣称已经记账/兑换成功；这些权威数据与执行结果只由 front 依据 points.py 推送和播报。
```

目标文件：`docs/contracts/interface-contract.md`（改）

### T8 · 契约 §四扩会话语境、设备删除上行与完整账本分页请求—应答

**问题**：`kidpal_session_ctx` 只有 story/home/schedule 三值，无法区分积分主页、详细账本与故事封皮；设备删除账本分录没有上行载体；完整账本也不能靠 story_list 的固定窗口获得。

**证据**：`docs/contracts/interface-contract.md:268-287` 现只有 story 私有帧族；`kidpal_story_delete` 提供单向幂等删除与隐式 ack 先例，`kidpal_story_fetch` 提供设备请求—网关卡应答先例。requirements S-F6.2 AC1（`docs/requirements.md:701`）要求任意历史分录可及，S-F6.3 补充规则（`:725`）把语境载体留给本批。

**根因**：积分需要“会话上下文 + 写删除 + 读分页”三类不同通道，不能全塞进 display view 或复用故事 id。分页读取可沿用 ADR-011 已建立的 device request→gateway response 边界，删除则沿用 ADR-007 单向隐式 ack。

**改法**：扩展现有 session_ctx，新增 `kidpal_points_delete` 与 `kidpal_points_page`。`spec 关系 = within-spec`（语境、删除、全量可及）+ `spec-silent`（消息名、游标与载荷）。

`kidpal_session_ctx` 整条定义替换为：

```text
- **`kidpal_session_ctx` 消息**（端→网关，KidPal 私有扩展消息）：设备在每次发起会话时、首帧音频前发送 `{"type":"kidpal_session_ctx","source_page":"home"}`；wss 常驻连接复用时同样发送，重复发送安全。`source_page` 闭集扩为 `home / schedule / points_home / points_ledger / story_cover / story`。P4 故事列表产生 `story` 并切记录语境；P4 封皮产生 `story_cover`，P3 积分主页/详细账本分别产生 `points_home/points_ledger`，三者均为普通问答语境，不得误切记录语境。详细账本可附加 `"selected_id":"<points-entry-id>"`，其值只能来自最近收到的 points_ledger items，供 front 注入被问分录摘要；它不是删除或写入指令。
  无论取值，整条消息一律由 front 消费并拦截、不下透引擎。`story` 的记录语境抑制沿 ADR-013 既有内部注入；points 两值由 front 从 points.py 取余额、激活目标、最近记录及可选 selected_id 分录，经既有内部通道注入有界摘要，模型不直接读状态文件。story_cover/home/schedule 保持普通会话。省略整条消息 = 普通会话，向后兼容；未知值拒绝并记日志，不得回退成 story。
```

新增删除消息：

```text
- **`kidpal_points_delete` 消息**（端→网关，KidPal 私有扩展消息；2026-11-04 新增，`261103-star-points-story-cover`）：设备在账本删除确认子态 KEY 确认后发送 `{"session_id":"...","type":"kidpal_points_delete","id":"<points-entry-id>"}`；id 只能取 points_ledger items 下发过的值，设备不造 id。front 整条消费并拦截、不下透引擎，交 points.py 删除该分录并按剩余分录重算余额及所有后续前后余额。删除后重推 points_home 与 points_ledger 首页，二者均带 `restore:true` 且 ledger `page.reset=true`，作为隐式 ack 对账。重复/已不存在 id = 幂等 no-op 仍重推；非法 id 拒绝并审计。单向通知、无独立应答消息；设备超时未收到不含该 id 的对账帧可重发。省略整条消息 = 无删除，向后兼容。
```

新增分页消息：

```text
- **`kidpal_points_page` 消息**（端→网关，KidPal 私有扩展消息；2026-11-04 新增，`261103-star-points-story-cover`）：设备请求详细账本分页时发送 `{"session_id":"...","type":"kidpal_points_page","cursor":null,"known_revision":12,"limit":6}`。`cursor` 缺省/null = 请求最新首页；续页时原样回传上一帧 `page.next_cursor`；`known_revision` 与 `limit` 可选，limit 缺省 6、允许 1–50，超出即拒绝并记日志。front 整条消费并拦截、不下透引擎，以 points.py 当前状态生成 §1.3⑧ points_ledger 应答。
  首页显式打开应答不带 restore 且 `page.reset=true`；续页应答带 `restore:true`、正常时 `reset=false`。游标按 `(occurred_at,id)` 时间倒序 keyset 定位，不用 offset。游标非法、过期或 known_revision 与当前 revision 不一致时，front 返回最新首页、`page.reset=true`；续页触发的重置仍带 restore:true，设备必须丢弃旧分页缓存再装入，不得跨版本追加。`next_cursor=null` 且 `has_more=false` 表示已到最早一页。front 对每一请求至多回一帧；空结果以权威 `items=[]` 返回，不伪造分录。省略整条消息 = 不取页，向后兼容。
```

§四末尾追加本批契约变更记录，列明 session_ctx 六值、points_delete、points_page、两类积分 view 与 story_list P4 消费语义均随 `261103-star-points-story-cover` 同批原子落地，并指向本 PROPOSAL。

目标文件：`docs/contracts/interface-contract.md`（改）

### 契约包 · displayBlock + shimApi

**proposedContractDiff**：

- **displayBlock**：§1.3 新增 `points_home`（余额、唯一激活目标、最近三条、空账/未达、负余额、restore）与 `points_ledger`（统一分录、前后余额、全量 total、keyset 分页、reset、restore）；§1.4 新增 `add_points/redeem_points` 网关决定类薄意图；§1.5/§1.6 同步白名单、front 单源与 prompt 纪律。`story_list` 改作 P4 模块共享数据帧，封皮计数复用 total；`char/story_text` 现役页位句对账。逐字 diff 见 T6/T7。
- **shimApi**：§四 `kidpal_session_ctx` 扩为六值并支持 points_ledger 的可选 selected_id；新增 `kidpal_points_delete` 单向幂等删除与 `kidpal_points_page` 请求—应答。逐字 diff 见 T8。
- **strokeFormat / scheduleMD**：未触，不携 diff。

**impact**：固件 view_router、mcp_server、points/story 页面、keybar 与 application.cc 会话入口需消费新形状；front/display_schema 需新增来源门禁、执行接线、恢复推送与私有帧消费；新增 points.py 和 QQ 积分 skill；引擎 display 流只允许薄意图，不重开 Intent。旧 story/home/schedule 的 wire 形状不变，但 story_list 的后台推送统一改 restore:true，K5 必须回归归档、删除和冷启动。部署顺序 = 新固件先行；旧固件没有 points 路由且把非 restore story_list 当列表帧，front 的积分推送与封皮导航须在对应真机验收后才可对生产设备开放。全部实现归 K0–K5，本批不落代码。

**validationPlan**：canonical 落地做 JSON 示例解析、三文件 byte 对账、preflight/cite-check、spec-anchor-check 与契约机械凭据闸；实现期以 K0 真实端到端 spike、K1 单 writer/幂等/删除重算、K2 设备空态/分页/删除、K3 真实 QQ、K4 目标边界、K5 P4 导航与原三值 session_ctx 回归验证。详细命令与正反判据见 §5。

**CDQ 登记（主线程账本动作，非本批 manifest 目标）**：reviewer 正式分类后，主线程按 `.claude/references/contract-change-evidence.md` 在 `docs/debt/contract/` 登记 `CDQ-261103-star-points-story-cover`，机读 `fourClass` 取正式裁定值，`loopEvidence` 指本 changeId 与 landing-verdict 结果摘要；起草 agent 不写 debt 账本，状态迁移由主线程按实际凭据处理。

目标文件：`docs/contracts/interface-contract.md`（改）

### T9 · 新增 ADR-018 记录单 writer、分页与 P4 数据帧取舍

**问题**：单 writer、传输幂等与语义去重分离、keyset 分页、story_list 复用为 P4 共享数据帧，都是实现期容易重新争论的长期决策。只改组件和契约不能保存“为什么不双写、为什么不用 offset、为什么不新增 story_cover view”的理由。

**证据**：`docs/decisions/ADR.md` 现行末条为 ADR-017，ADR-004/007/010/011 分别提供确定性 producer、隐式 ack、restore 与 device request→response 先例，但没有积分决策。workstream D-10、R-04/R-05/R-09 已收敛方向，无需重开 Track A。

**根因**：这些选择跨越数据一致性、wire 形状和导航消费，需要在 ADR 中显式记录备选与放弃原因，避免实现包用局部便利推翻系统边界。

**改法**：在 `docs/decisions/ADR.md` 文件尾 append ADR-018，既有 ADR-001~017 零改；完整五小节文本见 §6。`spec 关系 = within-spec`，技术承载为 spec-silent。

目标文件：`docs/decisions/ADR.md`（改，文件尾 append）

### T10 · 头部、§9 演进与 §12 定稿记账同步，消化 ARQ-260918

**问题**：architecture 头部仍写 v1.71 updating，§9 to-be 仍把“三页循环”当目标，§12 没有本批 changeId 行；§5.1.1 的旧四页叙述若只改组件行而不记演进，也会留下自相矛盾。

**证据**：当前工作树头部差异仅把 v1.49+ 改为“v1.70 为上一 frozen；v1.71 updating”，requirements 已在其后 frozen v1.71；`docs/architecture.md:456-464` 的 to-be④仍写“撤 P4 常驻页 → 3 页循环”。`ARQ-260918-spec-face-3page-reconcile` 要求随本变更消解旧页叙述。

**根因**：需求面冻结先于架构面落地，架构状态指针、演进路线和定稿账需要在同一 packet 内收尾，不能只改功能段。

**改法**：三处同步。`spec 关系 = within-spec`；没有新增需求修订。

1. 头部定名行替换为：

```text
> **定名（2026-09-13）**：**MVP** = 已开发完成、经真机验证的交付基线；**Phase 1** = 即将开发的下一版（需求 = requirements.md，v1.71 frozen，2026-11-04）。「二期」= Phase 1 之后。
```

2. §9 引言“Phase 1 四阶段”改为“Phase 1 五阶段（阶段五 = 星星积分本 + 故事封皮导航）”，to-be 整段替换为：

```text
**to-be（Phase 1 结构变更）**：① 键路由按 layer_top 与子页状态分流，KEY 长按逐层返回，单双击消歧覆盖 P4 故事列表与 P3 积分主页；② 60s 问答会话 timer 生产者接线，记录会话继续沿 §5.2 场景④；③ 思考态渲染接线；④ 查字保持会话内覆盖窗，不占主页面页位；⑤ display 周网格与首页数据保持 front 单源；⑥ 在既有故事控制与会话语境之上新增积分薄意图、points 两个 view、删除与分页私有帧；⑦ L4 渲染回执与查字 live 修复继续按既有登记推进；⑧ 主页面改为首页→课表→P3 积分本→P4 故事封皮四页循环，积分账本由 points.py 单 writer 承载，故事列表/阅读下沉 P4 子页并复用 story_list.total 作封皮计数。实现顺序以 requirements §6 阶段五和 K0→K5 串行切片为准。
```

3. §7 display 统一响应块指针由“7 view 白名单”改为“9 view 白名单”，并补“points_home/points_ledger 完整载荷由 front 单源产生，add_points/redeem_points 为 front 截获执行的薄意图”；该节仍只放指针，具体形状不复制进 architecture。

4. §12 表头分隔行后插入新行（新行在上）：

```text
| 261103-star-points-story-cover | 2026-11-04 | 头部需求 frozen 指针 / §2.4 原则 3 同型注记 / §4 NFR-OFF-1 与 NFR-PERF-4 映射 / §5.1.1 四页设备组件、枚举驱动语境与 9 view / §5.1.2 points.py 单 writer、QQ skill、front 积分职责 / §5.1.3 依赖 / §5.2 场景④ P4 化与新增场景⑥ / §5.3.2 积分状态位置 / §5.4 账本、目标与 RAM 缓存 / §5.5.2 资产、§5.5.4 UI 态、§5.5.5 prompt / §6.2 积分介质边界、§6.5 两类去重与分页一致性 / §7 display 指针 9 view / §9 五阶段与演进 / §12 本行 | delivery/review/261103-arch-review-star-points-story-cover/PROPOSAL.md（requirements v1.71 frozen 为行为权威；契约 §1.3/§1.4/§1.5/§1.6/§四同批原子落；ADR-018 同批落；ARQ-260918 随旧页叙述改写按 consumed-by 结项） |
```

目标文件：`docs/architecture.md`（改）

---

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 架构 canonical | `docs/architecture.md` | 改 | T1–T5、T10；不重编号，既有 arch 支撑锚保持 |
| 决策记录 | `docs/decisions/ADR.md` | 改（append） | T9 新增 ADR-018；既有条目零改 |
| 契约 canonical | `docs/contracts/interface-contract.md` | 改 | T6–T8；displayBlock + shimApi，同批原子落 |
| 需求 canonical | `docs/requirements.md` | 未触 | v1.71 frozen 是唯一行为权威；无逐字镜像需重审 |
| UI 设计面 | `docs/ui/**` | 未触（只读取证） | current=v9、brief v8、V9 11 帧已 frozen，可供 K0–K5 消费 |
| 实现面 | `firmware/**`、`server/**` | 未触（只读取证） | K0–K5 归 dev-loop；本批不实现、不部署、不写 live 状态 |
| 治理面 | `.claude/**`、G1 roots | 未触 | 无新 agent/gate/scope-map/settings；若实现期需要则回 intake 加面 |
| debt 账本 | `docs/debt/**` | 未触 | ARQ-260918 的 consumed-by 结项由主线程落地后处理；其余 debt 不借本批核销 |
| CDQ-260914 | `kidpal_session_ctx` 扩展关系 | 未核销 | 只有 K5/相关包验收同时覆盖原 story/home/schedule 发送与 front 拦截后，主线程才可结项 |

**契约四分类建议 = `ACCOMPANIES_SPEC_CHANGE`**。所有契约改动都直接支撑 frozen requirements v1.71 的 F6/S-F2.6，并随架构同批原子落地；不是脱离 spec 的独立契约变更，也不是要求裁改 spec 与契约哪一边的 drift。正式分类归 architect-reviewer。

**复扫模式与完备性**：本批退役的现役语面按词形 × 有/无反引号 × 所在节复扫，包括 `切到 P3 星星记录本`、`P3 右上角星星计数器`、`P3 进阅读态`、`进 P3 列表`、`切到 P4 查字页`、`HOME→CHAR→SCHEDULE→STORY`、`撤 P4 后改 3 页`、`撤 P4 常驻页 → 3 页循环`。architecture/contract 的 §12 与契约变更记录、§9 as-is 历史基线、ADR、requirements §9 属历史留痕，不按现役语面清扫；§10 决策 14 同样是历史档。T1/T3/T6/T10 覆盖上述现役命中，不扩大到其它含“P3/P4”的合法新文。

本批不新增治理门或机械断言，无新门 pattern 的全仓预跑义务；§5 的正向新形状断言与负向旧句残留断言成对给出。

---

## 4. human 取舍点（OQ）

**无新增 human 取舍点。** 以下均直接消费已裁结论：

- OQ-1：仅删除重算允许负余额；兑换动作仍要求余额不低于激活目标分值（spine openQ / S-F6.2 AC4、S-F6.3 AC3）。
- OQ-2：预置不可删，激活中的自定义目标不可删，须先切换（S-F6.5 AC4）。
- OQ-3：自定义目标只接受 5–200 正整数（S-F6.5 AC2）。
- 分流触点①：③拆包、K0 spike-first、WIP=1；本批不改为单包或 nano。
- 键位终审：P3 积分主页 KEY 短按语音、双击详细；P4 封皮 KEY 进列表；故事列表/阅读逐层返回。

留给 reviewer 而非 human 的裁定：主路径判档、契约四分类、points.py“单一实现 writer”是否充分满足 D-10、story_list 后台推送统一 restore 的兼容性、keyset 游标与 revision 重置是否足以防止跨版本漏项、ADR-018 的归并粒度。

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

### 起草期实跑基线（2026-11-04）

**基线 A · spec 锚与 UI-ready**：

```text
$ node .claude/scripts/spec-anchor-check.mjs
{
  "specAnchorCheck": "forward",
  "design": "kidpal-ui-design-v9.html",
  "designIdCount": 78,
  "archSectionCount": 48,
  "storyCount": 20,
  "anchorLines": 44,
  "errors": [],
  "ok": true
}
```

**基线 B · 需求文件保护哈希**：`git hash-object docs/requirements.md` 实测 `e775822a3e6c1138d8103de6ac5c281a902f0f0a`。landing 前后必须一致，防止借本批回改 frozen 需求。

**基线 C · ADR 编号**：对 `docs/decisions/ADR.md` 复扫 `^## ADR-`，现行末条为 ADR-017，ADR-018 零命中；因此本批新号为 ADR-018。

**基线 D · 契约 JSON 示例现状**：2026-11-04 对现行契约 10 个 `json` 围栏逐一 `JSON.parse`，既有 char 两个示例因省略号/占位不是严格 JSON。本批不宣称“全仓契约示例原本全绿”，落地后只对新增 points_home/points_ledger 两个严格示例做解析和字段断言；既有两个非严格示例不在本批扩改范围。

### landing 前（pre-commit）

1. **PROPOSAL 结构门**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261103-arch-review-star-points-story-cover/PROPOSAL.md` → 预期 `ok:true`、`findings:[]`，`anchorUnion` 恰为 {`docs/architecture.md`, `docs/decisions/ADR.md`, `docs/contracts/interface-contract.md`}，与 landing-manifest 集合相等。2026-11-04 起草期已实跑，实际 `ok:true`、`anchorCount:3`、`anchorUnion` 与上述集合一致、`advisories:[]`、`findings:[]`。
2. **承重引文对账**：`node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261103-arch-review-star-points-story-cover/PROPOSAL.md` → 起草期核对现文锚与磁盘现文；落地后核对改后文本。2026-11-04 起草期首跑实际 `ok:true`、`phase:draft`、`missing:[]`、`unverified:[]`；本稿以新文插入/完整段替换为主，`checkedCount:0`，不得把该结果解释为旧串 byte 已验，装配时仍须逐锚命中。发现锚文零命中或多义命中即停止，不以目视转录替代。
3. **共享锚文装配器**：主线程以 §2 的替换/插入点生成编辑清单，运行 `node .claude/scripts/assemble-edits.mjs <编辑清单.json> <仓外输出根>` → 每个 oldStr 恰命中一次，全部命中后才写仓外副本，输出仅含 manifest 三文件。本项在 FROZEN 前实跑，未实跑不得标 FROZEN。
4. **spec 锚全量复跑**：重跑基线 A → 预期仍为 `design=kidpal-ui-design-v9.html`、`designIdCount=78`、`archSectionCount=48`、`storyCount=20`、`anchorLines=44`、`errors=[]`、`ok=true`；本批不改 architecture 小节编号、requirements story 或 v9 锚。
5. **正向/负向现役句配对**：正向断言新文包含 `KP_PAGE_POINTS`、`points_home`、`points_ledger`、`kidpal_points_page`、`kidpal_points_delete`、`ADR-018`；负向断言 T1/T3/T6/T10 所列现役旧句在目标节为零。历史变更记录、ADR 与 requirements §9 不纳入清扫，避免把历史留痕误报为残留。
6. **需求零触**：landing 前后分别运行 `git hash-object docs/requirements.md`，两次均须等于基线 B。正向落点由第 3 项三文件装配结果证明，避免只查禁改面形成真空绿。

### landing 后、commit 前与 post-commit

7. **真实 diff 范围**：在 packet 落地 worktree 对落地前 base 运行 `git diff --name-only` → 非证据改动恰为 manifest 三文件；`git diff --numstat -- docs/decisions/ADR.md` 删除列为 0、新增列大于 0，且 `^## ADR-018 ·` 恰一次、`^## ADR-019 ·` 零次。
8. **新增契约示例严格解析与 shape 正向断言**：提取 `json` 围栏中 view 为 points_home/points_ledger 的两个示例，预期解析失败集合为空且示例数恰为 2；断言 points_home 含 revision/balance/target/recent，points_ledger 含 revision/balance/items/page，两个负余额字段类型为整数，page.total 不等于用 items.length 代替的伪计数。既有 char 两个非严格示例按基线 D 排除，不借排除扩到其它新示例。
9. **story_list 消费语义对账**：人读+定点检索确认所有后台恢复/对账推送（含 WS 建连、记录语境建立、归档、删除）均写 `restore:true`，显式打开故事才是不带 restore 的封皮导航；`total` 同时指封皮和列表计数，16 条窗口仅留 story_list，不出现在 points_ledger。正向断言 P4 封皮语义在场，负向断言现役“切到 P3 星星记录本”句为零。
10. **契约机械凭据闸**：合并前由主线程运行现行 `pre-merge-escalation-gate.mjs`，本批契约 diff 须同时满足 FROZEN PROPOSAL、PASS landing-verdict、manifest 覆盖 `docs/contracts/interface-contract.md`；缺一即 BLOCK，不以文本评审替代。
11. **落地后全量复跑**：第 1、2、4、5、6 项不得窄化；再由 landing-verifier 逐文件人读 diff，核对 §2 各 T 与 §6 ADR 附件，无夹带、无 requirements/UI/governance 改动。

### 下游实现契约 validationPlan（不是本文档批的假绿灯）

12. **K0 spike**：真实设备语音和真实 QQ 各写一笔，设备实际显示权威余额与事由；QQ 在其他页不抢前台；最早/最新 canary 均可经分页看到。文件写入或 pytest 读回不能单独作为通过依据。
13. **K1 账本核心**：依次“加 50 → 兑 40 → 删加 50”得余额 −40，随后兑换必须拒绝；同 command_id 重放只产生一次分录，窗口外同文案新 command_id 可再记，窗口内归一化重复只回“已记过”；并发语音/QQ 由同一锁串行，崩溃重读后 revision 与回执不丢。
14. **K2 分页与设备**：以超过两页的数据验证首页、续页、末页、最早一条、空账、未达和负余额；续页中插入/删除触发 revision 重置，无跨版本重复或漏项；设备删除重发幂等，断网不写本地队列；P3/P4 每一页开会话后的 source_page 逐值核对，特别防止积分页误发 story。
15. **K3/K4 QQ 与目标**：歧义命令普通文字报错且不调用 ask_user；5 与 200 接受、4 与 201 拒绝；预置删除拒绝，激活自定义删除拒绝，切换后可删；切目标不自动扣分，兑换只认新目标分值，真实 QQ 与真机联合取证。
16. **K5 故事回归**：四页 BOOT 循环、封皮计数复用 total、列表/阅读逐层返回、打开故事只到封皮；归档/删除/冷启动的 story_list restore 不抢前台，原 story/home/schedule 三值 session_ctx 发送和 front 拦截义务不回归。满足此项后主线程才评估 CDQ-260914 核销。

---

## 6. 留根清单

- **长期增长与备份**：state.json、命令回执和分录首版不自动淘汰，容量与备份只观察；出现真实容量压力另立变更，不在 K1 静默加窗口或删除历史。
- **短窗归一化**：窗口起步值与调优定数的权威 = requirements S-F6.3 AC5，调优后回填；保守文本归一化是技术初值；真机若证明“事由相近”需要模糊匹配，必须回架构评审，不能由 Pod 自造相似度阈值。
- **NFR-PERF-4**：软时延数值仍待真机调优，本文档批不填阈值；K0 先证明设备真实可见刷新。
- **debt 关系**：ARQ-260918 由主线程按实际落地 consumed-by 结项；TECH-261028 由 K5 消化；TECH-261022-p3-record-session-state-churn 只串行对账；TECH-261030/TECH-261023/ARQ-260907 不借本批结项；CDQ-260914 需原三值回归后才可核销。
- **QQ 与只读防线**：新 skill 调 CLI 不等于开放直接文件写权；任何改成 LLM 直写 state.json、front/QQ 各算一份余额或依赖 ask_user 的实现都必须回本批重审。
- **旧 story_list 窗口**：本批只把封皮计数与后台 restore 语义改成 P4 模块共享，未扩展故事列表 16 条窗口；若以后要故事全量可及，另立需求与架构变更。

**ADR-018 落地附件**（packet-bound landing 逐字 append 到 `docs/decisions/ADR.md` 尾）：

```text
## ADR-018 · 积分账本由确定性单 writer 承载；完整账本 keyset 分页；story_list 复用为 P4 共享数据帧

- **日期**：2026-11-04（`261103-star-points-story-cover` 架构+契约面 Track B；行为输入 = requirements v1.71 frozen）
- **决策**：① `server/gateway/points.py` 是积分账本与目标配置的唯一确定性 writer。设备语音由 front 调其 API，QQ skill 调其 CLI；两者共用校验、锁、余额重算、原子持久化和命令回执，LLM/front_app/QQ 不另写状态或另算余额。② 同一逻辑命令以 command_id 做传输幂等，用户短窗重复表达以可注入时钟和保守事由归一化做语义去重，二者不得共用一个开关；删除后重算允许负余额，其它路径不得制造负余额。③ 详细账本以 `(occurred_at,id)` keyset 游标分页，revision 变化时回最新首页并重建设备分页缓存，不用 story_list 的 16 条窗口或 offset。④ story_list 作为 P4 封皮与列表共享数据帧，total 是两者唯一现存篇数来源；后台推送统一 restore:true，显式“打开故事”才由 front 展开不带 restore 的封皮导航帧，不新增 story_cover view 或第二故事计数源。⑤ 会话语境由统一页枚举加子页状态产生，禁止数字 case 映射；points_home/points_ledger/story_cover 均为普通问答语境，story 只从 P4 列表产生记录语境。
- **理由**：双通道分别写状态会形成并发覆盖、重复扣款和余额双源；让 LLM 写余额则破坏只读与可机判边界。命令重试和用户重复说是不同问题，混用会把合法新分录吞掉或让网络重试重复扣款。完整账本在持续插入/删除下使用 offset 会重复或漏项，keyset 加 revision 重置给出可判一致性。封皮与列表的计数本来就是同一个 total，新增 view/计数只会造双源；后台数据更新与显式导航分开后，归档、删除和 QQ 推送才不会抢前台。页位数值在插入 P3 后必然漂移，枚举加子页状态才能保住语境。
- **备选与放弃原因**：① front 与 QQ 各写一份账本逻辑——放弃：双写者和规则漂移。② LLM 直接编辑状态文件或输出完整积分载荷——放弃：违反 kidpal 只读、front 单源与确定性验收。③ 只靠短窗去重保证传输幂等——放弃：窗口外重试会重复写，窗口内合法命令又难以区分。④ offset 或 story_list 16 条窗口——放弃：并发插入/删除下分页不稳定，且不满足全量可及。⑤ 新增 story_cover view 与独立 cover_total——放弃：同一数据双源；story_list 加明确 restore/导航纪律已足够。⑥ 保留数值 source_page 映射——放弃：插入页位后会把积分页误切为故事记录语境。⑦ 设备离线写队列或 SD 账本副本——放弃：requirements NFR-OFF-1 明确不本地补记，介质白名单不扩。
- **关联**：requirements v1.71 §2.2–§2.4、S-F2.6、S-F6.1–S-F6.5、NFR-OFF-1、NFR-PERF-4；architecture §2.4、§5.1、§5.2 场景④⑥、§5.4、§6.2、§6.5；契约 §1.3⑦⑧、§1.4、§1.5、§1.6、§四；ADR-002/004（确定性分层）、ADR-005/010（restore）、ADR-007（删除隐式 ack）、ADR-011（设备请求—网关卡应答）；spine = `delivery/intake/261103-star-points-story-cover.md`；实现 = K0 spike → K1–K5 串行。
```

---

## 7. 评审记录

（判官 append-only 区；待 architect-reviewer 四查、主路径判档与契约四分类。主线程不预判结论。）

---

### R1 · 2026-11-04 · architect-reviewer（独立审稿；只读 canonical，唯一写入 = 本节）

**verdict: BLOCK(F1)**

**查一 · 意图保真：PASS。** spine human 原话五条逐一对上：故事封皮 + BOOT 四页循环（T1 渲染节点行、S-F2.6）；目标做活 + QQ 渠道设置（T2 QQ skill 行、场景⑥E、S-F6.5）；语音触发须完整记账句式（T7 add_points/redeem_points 拒绝条件、S-F6.3 AC1）；「允许负余额」仅删除重算（T4 逻辑事实段、契约⑦「负余额只能来自删除重算…front 不另做钳位」、K1 用例）；「禁止删，先切换」「5–200 正整数」「KEY短按=语音」（T4/场景⑥E、T1 按键路由行）。非目标清单与 ideas 已批示不做三项（庆祝演出/惩罚性扣分/加分护栏）一一对应，无夹带语义。子页页点（P3/P4 子页保持模块身份实心点）以 S-F2.6 补充规则 incorporation 的 ideas 连带决定（human 2026-11-03 拍板）为准，与 requirements §2.2「仅主页面显示」的表面张力经核对为该句与覆盖层对比的语境差异、不构成 story 语义变更，不退需求面。

**查二 · 纪律合规：F1（一处阈值双源），其余 PASS。** 分工句三条核对：决策本体归 ADR-018（T9，五小节齐）；数据格式归 contracts（T4 明写 state.json 内部结构不在契约复制）；「需求与阈值归 requirements.md」在 T4 自身对 NFR-PERF-4 已写「阈值仍只回引 requirements、不在架构写数」——但同在 T4 的 §6.5 拟落地文本把未定阈值数字写死进架构（见 F1）。既有先例 = arch §5.2 场景④卡壳阈值只写「阈值权威 = requirements NFR-PERF-3〔待定〕」、不复述 8 秒。arc42 骨架与锚稳定规则：T1–T10 零改小节编号；spec-anchor-check 本审独立实跑逐字吻合基线 A（78/48/20/44、ok:true）。

**查三 · 撞车复核：PASS。** ① ADR：本审独立复扫 `^## ADR-` 末条 = ADR-017、ADR-018 零命中；ADR-002/004/005/007/010/011/013 引据逐条到 ADR.md 核实在位且语义相符（ADR-011 device request→response 先例、ADR-007 隐式 ack、ADR-013 记录语境抑制均真实）。② 契约现行：`:63`「切到 P4 查字页」、`:105-110` story_list「归档/删除执行后恒不置 restore（缺省=主动展示）」、`:184` relay 收紧句、`:192` prompt 句均与 T6/T7 替换申报相符——story_list 后台推送统一 restore:true 是对既有生产纪律段的 modifying，走本 loop 合法、非暗改。③ architecture 现行：`:177` 旧四页叙述、`:180` 7 view、`:194` 7 view 校验、`:462` to-be「撤 P4 常驻页 → 3 页循环」全部命中 T1/T3/T10 覆盖清单；§9 as-is、§10 决策 14、双方变更记录按 §3 复扫模式豁免，豁免理由成立（历史留痕）。④ ARQ-260918 承接（T10 头部+§9）与 debt 消费表（spine §debt）一致，TECH-261022 串行对账、TECH-261023 只读防线均如实不借批结项。

**查四 · 牵连面完整：PASS（附两条非阻断注记）。** ① 消费方：固件侧 view_router/mcp_server/keybar/key_gesture_parser/application.cc 逐一有 T 行与 impact 落点；本审亲核承重锚——`firmware/main/display/kidpal_render_node.h:32-37`（COUNT=3）、`story_page.c:2285-2298`（case 2 数值映射，含「M2K2-A2 冻结等值」防护注释）、`application.cc:1313-1320`（session_ctx 单点）、`server/gateway/display_schema.py:27-48`（7 view + 网关决定类分表）、`front_app.py:568-626`（self_push + restore 先例）、`kidpal-schedule-write/SKILL.md:58-115`（QQ→机检→marker 先例）——与 §2 引用逐字相符。② 反向锚：零改小节编号，requirements story 支撑锚（§5.1/§5.2/§5.4/§5.5.4/§6.2/§6.5）不受扰动；§12 changeId 行（launchPreconditions 消费判据）由 T10.4 承载。③ 结构字段：landing-manifest 围栏与 packet-id 行在场；preflight 本审独立复跑 `ok:true`、anchorUnion 恰为 manifest 三文件、findings/advisories 空。④ 基线 B/C/D 本审独立复跑吻合（requirements 哈希 `e775822a…f0a` 一致；ADR-017 末条 + ADR-018 零命中；契约 10 个 json 围栏计数本审复核一致、char 两示例含省略号非严格）。⑤ 部署次序与旧固件保护：impact「新固件先行 + 积分推送/封皮导航真机验收后才对生产设备开放」正确封住「旧固件把非 restore story_list 当列表帧抢前台」（260928 旧缺陷的复现路径）与「旧固件无 points 路由」两个面。

**起草方五项自报重点核查——逐项独立核，均成立：**

1. **单 writer 与网关恒薄同时成立；front 推送非第二真相源。** points.py 落 gateway 与既有形态同型（story.py 归档/硬删/seq 账本、schedule.py 唯一解析器、reminder_schedule_lib 均为 gateway 内确定性执行器先例）；T2 对 §2.4 原则 3 追加同型注记并挂 ADR-002/004，语义切分正确（语义理解在脑、校验/重算/原子写在执行器）。front 只依据 points.py 状态序列化权威帧、不另算余额（「所有成功/拒绝均以 points.py 回执为准」钉死）；余额/前后余额由 points.py 从剩余分录派生并持久，front 的 revision 观察 = 推送触发量、非独立计算源。QQ skill 与语音 API 共用同一模块与不变量，进程间锁补齐跨进程互斥（§5.4 `.lock` 行明确「不是业务事实源」）。
2. **双轨去重支撑 S-F6.3 AC5 且不过度吞单；归一化边界守住。** 传输幂等（同 id 重放返首次结果、不新增分录、不递增 revision）与语义去重（窗口内命中回「已记过」+权威余额+跳页）机制分离、判据互不借用；「新的用户回合即使文案相同也必须使用新的 command_id」钉死传输幂等不被扩为去重窗口，窗口外同文案 = 合法新命令——吞单边界即 human AC5 裁定的 10 分钟短窗本身，无越权扩张。归一化白名单（空白/标点/全半角/不改变时间前缀）+「更宽模糊语义回 /arch-change、不在 Pod 内自调相似度」= 阈值与相似度调参权收回架构面，成立。
3. **story_list P4 化兼容性、部署次序、旧固件保护、front 单源均守住。** 修改的是现契约生产纪律段（modifying，随本批原子落）；后台推送统一 restore:true 后，归档/删除不再抢前台与 S-F2.6 P4 模块、逐层返回、盖章由 story_text 承接的 frozen 语义自洽；「打开故事」空 data 薄载荷 + front 展开不带 restore → 进封皮不跳列表，满足 S-F2.6 AC4。front 单源未削弱：非空 story_list/story_text 引擎来源仍拒转，OpenClaw 新增的唯一出口是空 data 薄载荷（schedule 薄载荷先例同型），relay 收窄句在 T3/T6/T7 三处一致。旧固件保护见查四⑤。
4. **keyset 分页 + revision 重置与删除不变量两层一致无漏。** `(occurred_at,id)` keyset + 游标携带 revision + 变更即回首页 reset，闭环防跨版本重复/漏项；OQ-1 在架构（删除后按时间序重算前后余额、余额=剩余分录 delta 之和、其它路径不造负余额）与契约（balance 系可正可负整数、front 不钳位）两层同口径，K1「加 50→兑 40→删加 50 得 −40、随后兑换必须拒绝」同时钉负余额仅删除路径与兑换余额门槛（S-F6.2 AC4/S-F6.3 AC3）；OQ-2/OQ-3 落 points.py 不变量与场景⑥E；目标规则不入 wire（targets 走 QQ CLI，非端云形状）故契约层无漏。删除消息沿 ADR-007 隐式 ack 先例，重发/幂等 no-op 仍重推语义与 `kidpal_story_delete` 同构。
5. **source_page 映射完整且不误切记录语境；CDQ-260914 前置守住。** 六值闭集覆盖 P3 主页/详细与 P4 封皮/列表；记录语境仅 `story`（P4 列表），`points_*`/`story_cover` 均普通问答；未开放会话入口的状态（阅读/删除确认/整理过渡）禁发；「未知或越界回退 home 绝不回 story」（设备侧）与「未知值拒绝并记日志、不得回退成 story」（front 侧）双层一致。CDQ-260914 在 §3 明记未核销、核销前置 = K5 原三值发送与 front 拦截回归（§5.16），未被本批静默消化。

**F1（否回项 · 纪律/阈值双源）**：T4 §6.5 积分段第 2 条拟落地文本「points.py 以可注入时钟判断起步 10 分钟窗口」把 requirements S-F6.3 AC5 明标「起步 10 分钟、**真机调优定数**」的未定阈值数字写死进架构，且句内无权威回引——与本稿自身对 NFR-PERF-4 的「阈值仍只回引 requirements、不在架构写数」及场景④卡壳阈值先例（不复述 8 秒）不一致；真机调优改数后此处必成第二源漂移。**修改指令**：该句改为「points.py 以可注入时钟判断语义去重窗口（起步值与真机调优定数的权威 = requirements S-F6.3 AC5，调优后回填）」，不在架构复述分钟数；§6 留根「短窗归一化」行的「起步 10 分钟沿需求」同步补「定数归 requirements」显式指针或改为同一回引句式。§5 验证集第 13 项未写具体分钟数，无需动。此为唯一否回项；修回后本审仅核 F1 及修订引入的新矛盾。

**非阻断注记（两条，不构成否回）**：(1) 现役固件 `story_page.c:2287-2288` 注释自证「M2K2-A2 冻结等值」验收齿钉在数值映射上——T1 改枚举+子页驱动后该冻结验收必红并须随 K2/K5 重写，§5.14 已覆盖逐值核对，但建议包级 brief 显式点名 M2K2-A2 夹具改写义务，防实现包只改函数不换齿。(2) T2 QQ skill 行未显式写 S-F6.4 AC2 查账（家长问余额/目标/最近记录）的只读路径——按课表「读 MD 答询」先例，读 points.py 状态（CLI status 输出或只读 state.json）即可满足且不破只读防线，建议起草方顺手在 skill 行补一句，语义零变化。

**判档：主路径（Track B）——确认起草方预判。** 轻路径判据逐条：① 零语义变化——不满足（新增数据域/页结构/契约形状）；② 改动性质——不满足（非错字/排版/指针刷新级）；③ 不触 `docs/contracts/**`——不满足（T6–T8 触契约）；④ 不触 §5.x——不满足（§5.1/§5.2/§5.4/§5.5/§6.5 改写）。判据须全部满足 → **主路径**。Track A 段不触发：判据只看收敛度——三项 OQ（负余额/禁删激活/5–200）均已 human 裁定、spike-first 与 ③拆包已裁、方向无未裁根本分叉、无红队声明；触承重面（§2.3 端云职责、契约形状）不自动升级。

**契约四分类：`ACCOMPANIES_SPEC_CHANGE`（正式裁定，与起草方建议一致）。** 依据：全部契约改动（两类 display view、两控制动作、白名单/prompt、三条私有消息）逐条直接支撑 frozen requirements v1.71 的 F6/S-F2.6/§2.3，并与架构改动随批原子落地——非独立契约变更（有架构配套），非 spec↔contract 冲突裁改（drift 不成立）。反证过五问：无需 adapter/mapper（现契约形状确实无法表达分页/删除/积分载荷）；无现有扩展字段可放（§1.3 无积分 view、§四无删除/分页载体）；无本地命名撞 canonical（points_home/points_ledger/add_points 全新、与既有 view/action 闭集不撞）；未前塞二期能力（多设备/语音删除/惩罚性扣分均显式排除）；无既有形状可复用只差载荷（story_list 16 条窗口被 R-04 显式排除做账本边界）。CDQ-261103-star-points-story-cover 由主线程按本裁定值登记。

**结构字段核对：** landing-manifest 围栏（三文件）与 packet-id 行在场；preflight 本审独立复跑绿（anchorUnion = manifest 集合相等）。结构字段 GREEN 前义务已满足，最终期限 FROZEN。

**结论：BLOCK(F1)，单一否回项。** architect 按 F1 修改指令修订 T4/§6 留根后，本审复核该条及修订引入的新矛盾即可转 GREEN；判档与四分类裁定不受 F1 影响、随本记录生效。主线程在本批转 GREEN 前不得标 FROZEN。

---

### R2 · 2026-11-04 · architect-reviewer（F1 靶向复核；只核 F1 消解与越界扩散，R1 其余结论不重复审）

**verdict: GREEN**

**F1 复核：已消解。** 依据 commit `5f377f0819ecb6e80ffc1d8b153cf5c3686c4851`（DRAFT v0.2）真实 diff 独立核对（`git show 5f377f08`）：恰三处、仅本 PROPOSAL 一文件（4+/3−），与自报范围一致——① 头部版本行升 v0.2 + 修订注记行（纯记账，零语义）；② T4 §6.5 积分段第 2 条删「起步 10 分钟窗口」写数，改为「以可注入时钟判断短窗（起步值与调优定数的权威 = requirements S-F6.3 AC5，调优后回填）」——与 R1 修改指令句式一致，机制（可注入时钟/归一化白名单/窗口外合法新命令）逐字保留；③ §6 留根「短窗归一化」行改为同一回引句式。全文残留「10 分钟」检索仅命中 §7 本评审记录自身（R1 历史记述，append-only 豁免），§2 改法文本与 §6 留根零残留；requirements 侧权威句（S-F6.3 AC5「起步 10 分钟、真机调优定数」）经本审在 `docs/requirements.md:721` 复核在位，回引指向真实。

**越界扩散：无。** diff 未触 §2 其它 T、§5 验证集、landing-manifest、ADR-018 附件与 §7 R1 原文；结构性连带核对——preflight 本审在 v0.2 复跑 `ok:true`、`anchorCount:3`、anchorUnion 恰为 manifest 三文件、findings/advisories 空——与 v0.1 逐字一致。修订未引入新矛盾：回引句不改变去重机制语义，K1/场景⑥失败边界等引用 S-F6.3 AC5 的段落不受影响。

**结论：GREEN + 判档 = 主路径（Track B）+ 契约四分类 = `ACCOMPANIES_SPEC_CHANGE`（均沿 R1 裁定生效）。** 无未闭合否回项。R1 两条非阻断注记（M2K2-A2 夹具改写义务点名 / QQ skill 查账只读路径一句）维持非阻断、由 architect 落地编辑时酌情采纳，不拦 FROZEN。主线程可凭本 GREEN 标 FROZEN 并进入 packet-bound 落地；落地期实跑与 §5 基线如有字节差异，按 ARQ-261021 以落地实跑覆写并在本节留痕。
