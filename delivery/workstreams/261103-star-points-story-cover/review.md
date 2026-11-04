# 261103-star-points-story-cover · 分流 review（architect）

- reviewer：architect（/triage-change 派发；只读 + 同会话 return，零写入）
- 日期：2026-11-03
- 输入：delivery/ideas/261103-story-cover-navigation.md、delivery/ideas/261103-star-points-book.md、docs/notes/showme/261103-star-points-home-wireframe.html、requirements/architecture/interface-contract 全文、docs/debt 四域、governance 面
- 结论：**③拆包 GREEN；spike-first 命中必须保留**。BLOCKING findings 均已转化为需求 OQ / 架构契约任务 / 包级 launch 前置，无留给实现 Pod 自裁的悬空项。本 sign-off 只表示 reviewed plan 的方向与拆包依据成立，不代表需求、架构、契约已冻结，也不授权任何包 launch；PROPOSAL 的 FROZEN 归 human。

## A. faceMap

### 1. 需求面：动，档 2 结构性变更并触 UI

- 主页面与页点：`docs/requirements.md:70` 仍定义中槽 3 页点且仅 P1–P3 显示；`171-185` 仍是首页、课表、星星记录本三页循环；`161` 仍写三页核心逻辑。须改为首页→课表→积分本→故事封皮，并明确故事列表和阅读均属 P4 模块、显示第 4 个实心页点。
- 权威按键矩阵：`docs/requirements.md:136-149` 的 P3 列表、删除确认、阅读等行须重排为 P3 积分主页、P3 详细账本、P4 封皮、P4 故事列表、P4 阅读等状态；`151-161` 的"页循环在 P3 断开""双击消歧仅 P3 列表""3 页循环"均须改写。
- 产品概述：`docs/requirements.md:19-22` 目前只列问课表、帮我记录、查生字三项专项能力，需补积分账本；`57` 记录语境与 `docs/contracts/interface-contract.md:276` 会话上下文同步考虑积分语境。
- F2 故事面：`docs/requirements.md:323-327`、`333-350`、`437-452`、`455-469`、`471-482` 以故事列表为 P3 顶层，须下沉为 P4 封皮子页并逐层返回；建议新增 stable story 锚 `S-F2.6 · 故事封皮与分层导航`（不复用已砍的 S-F2.5）。
- 新积分 feature：`docs/requirements.md:586-604` F5 后新增 F6，建议拆为积分主页、详细账本与删除、设备语音记账与兑换、QQ 家长通道、目标配置五条 story；积分与故事星星互相独立的口径须写进 F2/F6 边界。
- 离线与视觉：`docs/requirements.md:662-668` NFR-OFF-1 补积分断网不可写、不本地暂存、未达与空账两态；`612-616` 视觉原则区分钟点金币与故事星星，受 14px 可读性下限约束。
- 路线图与定稿账：`docs/requirements.md:686-710` 登记新能力推进位置；`754-760` §9 追加版本行。程序依据 = `.claude/references/requirement-change.md:39-58`（档 2 触 UI 五查 + human 看图终拍）。

### 2. 架构面：动

- 设备组件：`docs/architecture.md:170-186` 新增积分主页、详细账本、故事封皮组件，扩展渲染页枚举、view 路由、按键归属与 MCP 删除上行面；`177` 仍写撤查字页前四页旧序（本次随批消化的陈旧 spec）。
- 服务端组件：`docs/architecture.md:188-205` 增加账本与目标配置确定性 owner、front 记账命令校验/执行/推送职责、QQ skill 与 prompt 接线；`194` display schema 目前 7 view。
- 运行场景：`docs/architecture.md:211-225` 新增"设备语音/QQ → 确定性账本 → front 推送 → 设备积分页"场景，修订场景④ P3 顶层假设；语音记账、兑换、QQ 写入、删除、目标切换、建连恢复均有时序与失败边界。
- 数据视图：`docs/architecture.md:265-284` 新增积分账本分录、激活目标与奖励场景库、设备 RAM 渲染缓存等行；`252` 交接目录登记新数据位置。
- 横切机制：`docs/architecture.md:341-342` 维持业务数据不落板、补积分无离线写队列；`350-351` 幂等机制目前只覆盖提醒，须为账本另写短窗去重与请求幂等。
- 开发与资产：`docs/architecture.md:295-305` 既有资产/字体管线可复用，登记书封、四预置场景与自定义通用标记；`307-323` 补新 UI 态与 prompt 静态语义接线。
- 演进与定稿：`docs/architecture.md:456-464` to-be 更新；`509-515` §12 追加 changeId 行；新决策本体进 ADR。
- 外部依赖不加：`docs/architecture.md:32-45` 已覆盖本变更所需依赖。

### 3. 契约面：动，预期四分类 ACCOMPANIES_SPEC_CHANGE（最终归 architect-reviewer）

- `docs/contracts/interface-contract.md:44-163` view 集合无积分主页/详细账本载荷；`180-187` front 白名单与 shape floor 无积分视图。至少需要积分主页和账本列表两类权威下行形状（余额、激活目标、最近记录、兑换前后余额、空态/未达、restore、分页语义）。
- `docs/contracts/interface-contract.md:92-113` `story_list` 板子行为仍写"切到 P3 星星记录本"，须随封皮分层改 P4 模块语义；封皮计数复用 `total`，不另造故事计数源。
- `docs/contracts/interface-contract.md:165-178` 需评估语音加分/兑换薄意图控制动作：模型只产结构化意图，front 校验并执行确定性账本命令，模型不直接提交余额或宣称写入成功；动作名与形状由 /arch-change 定。
- `docs/contracts/interface-contract.md:189-194` system prompt view 列表与生成纪律同步，明确积分权威数据只由 front 推送。
- `docs/contracts/interface-contract.md:268-287` §四：① `kidpal_session_ctx.source_page` 扩积分主页/详细账本/故事封皮语境；② 设备删除账本分录的上行私有消息及重推对账；③ "完整账本"若按需取页需分页请求—应答形状。不能照搬 story_list 16 条窗口后宣称完整。

### 4. 实现面：动

固件事实锚：
- `firmware/main/display/kidpal_render_node.h:28-37` 页枚举 HOME/SCHEDULE/STORY/COUNT=3；`134-139` 三页循环声明；`firmware/main/display/kidpal_render_node.cc:1080-1101` 以 KP_PAGE_COUNT 取模翻页。
- `firmware/main/key_gesture_parser.h:44-49` 只有 P1/P2/P3；`firmware/main/display/keybar.c:18-29` 页点总数取 PAGE_COUNT，`40-52` P3 槽仍是故事键帽。
- `firmware/main/display/story_page.c:2285-2297` 会话来源映射用数值 `case 2` 表示故事——插入 P3 积分后故事变数值 3，不改则积分页按 KEY 被误报成故事记录语境；**承重接线点，须改枚举驱动并补验收**。
- `firmware/main/display/story_page.c:2414-2436` 故事按键归属、`firmware/main/display/view_router.c:33-65` 7 view 路由、`firmware/main/mcp_server.cc:1368-1375` display 工具说明需扩展。
- `firmware/main/application.cc:1313-1320` 是每次会话发送 kidpal_session_ctx 的实际入口，积分/封皮语境从此单点消费新映射。

server/OpenClaw 事实锚：
- `server/gateway/display_schema.py:26-35` 只有 7 view，`41-48` 只有既有控制动作，`58-160` 是新增积分 shape floor 与来源门禁的执行点。
- `server/gateway/front/front_app.py:610-626` 已有 story_list 恢复推送与不抢前台先例；`629-689` 私有帧消费与删除重推对账先例（复用接线形态、不复用业务规则）。
- `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md:9-18` 回合路由、`27-36` view 白名单、`50-56` 故事纪律、`62-83` 硬约束需加入积分薄意图规则；`78` 数据只读边界保留。
- `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md:58-115` 提供"QQ→确定性机检→marker→front 推送"先例；积分复用工程形态，账本用自己的单一 writer 与机检，不写进 schedule.md。

## B. governance 探测结论

**不触 governance 面，不需要 /gov-change。** 新测试落现有 firmware/main/test/、server/acceptance/、server/tests/ 体系；不需要新 agent、scope-map 修改、gate 或工程门。跨面启动与 anchors 前置消费 `.claude/references/change-intake.md:74-79`；契约包消费 `.claude/references/arch-change.md:46-54`。若实施发现必须改 .claude/** 或新增机械闸，停止并回 intake 加面，由 human 裁定。

## C. 查重与 debt 消费关系

delivery/intake/INDEX.md:10-29 的 16 条 spine 经积分/封皮/奖励关键词复核无相近意图，可新建本 spine。

| debt 条目 | 关系 | 处置 |
|---|---|---|
| ARQ-260918-spec-face-3page-reconcile | 被本变更消化 | spec 重写页循环时消解 requirements.md:165 与 architecture.md:177 等陈旧叙述；spec 定稿后主线程按 consumed-by 结项 |
| TECH-261012-qq-schedule-write-format-gap | 并行，只复用先例 | 新 QQ 积分 skill 参考机检/marker 形态；验收不得宣称修复该条目 |
| TECH-261030-qqbot-ask-user-block | 并行，硬设计约束 | 积分 QQ 命令不依赖 ask_user；歧义用普通回复可执行格式；删除用稳定记录 id/序号 |
| TECH-261022-device-memory-prompt-overhaul | 并行底座 | 复用持久 session 与静态 prompt 承载；积分回合做记忆归属回归，不并入遗留项 |
| TECH-261023-openclaw-tool-face-prompt-only-readonly | 并行防线 | 保持 kidpal 数据目录只读、模型发薄意图、front 确定性执行；若改为直接写账本须先正面处理本条并重审 |
| ARQ-260907-openclaw-tools-rejection-rootcause | 不阻塞不消费 | 继续依赖 front relay、私有帧与 OpenClaw skill，不重开引擎 Intent |
| TECH-261028-p3-reader-key-falls-to-session-toggle | 被 K5 消化 | 否则下沉故事子页会把"阅读 KEY 短按误开会话"与键帽缺位带入新层级 |
| TECH-261022-p3-record-session-state-churn | 有独立承接包 | 共享 keybar/故事页文件须串行对账，不重复承接、不代替结项 |
| CDQ-260914-kidpal-session-ctx | 本变更扩展之 | 仅当同批验收覆盖原 story/home/schedule 发送与 front 拦截义务时方可由主线程核销 |

## D. spike-first 判定（ARQ-261024）

**命中。** 本变更首次建立"设备语音/QQ 记账生产者 → 云端持久账本与目标配置 → front 权威推送 → 设备积分页与删除消费"的端到端业务数据通路。QQ、OpenClaw、WS、文件存储均为既有依赖，只排除"新外部依赖集成"，排除不了"新建一条端到端数据通路"；无存量数据迁移不影响命中。

首个包 = 独立 **②Full spike**（不判 nano），在正式账本、完整 UI、数据回灌之前以最小真实数据验证最高风险路径：

1. 设备说完整"事由+加+分值"，经真实 OpenClaw/front 路径形成云端分录，设备积分页看到权威余额与原因；不以文件写入或 pytest 读回作通过依据。
2. QQ 写一条首尾可识别记录，验证设备在其他页只更新数据不抢前台、积分页在屏时原地刷新。
3. 按完整账本的窗口/分页设计放置最早与最新两条 canary，验证消费端看到的不只是最近一条。
4. 留 front 记账命令、推送、设备 view 路由与页面消费的可观测日志，证据落 spike 包 spike-evidence.md。

## E. 五档裁决

**③拆包。** 本变更同时包含新云端持久数据域、两条写入通道、端云新契约、设备两个新页面族、故事模块层级重排、全局按键与页点改造、OpenClaw 两类工作区 prompt/skill、真实 QQ 与真机验收；跨多个可独立演示的垂直切片，无法单包一轮诚实收口，不满足 Lite"无悬挂契约决策"与 nano"无验收语义净增"。workstream 总体 riskTier: high，包级由 portfolio-planner 逐包派生；spike、账本核心、设备积分、契约相关包不预降 Lite。

## F. findings（实质单轮 review）

| finding | 级别 | 发现 | 处置 |
|---|---|---|---|
| R-01 | BLOCKING | ideas 和线框不是 frozen spec，当前无可引用的 F6 story，现有契约也无法表达积分 | 转为 facePlan 的需求→架构/契约硬前置；anchors 未回填不得 launch |
| R-02 | BLOCKING | 删除早期加分可能使已兑换后的余额变负，和"余额不为负"冲突 | 列 OQ-1；human 2026-11-03 裁「允许负余额」（仅删除重算路径），PROPOSAL D-03 已按裁定回灌；验收含"加 50→兑 40→删加 50"用例 |
| R-03 | BLOCKING | 激活自定义目标可删与"恒有激活目标"冲突，自定义范围未拍数 | 列 OQ-2/OQ-3；human 2026-11-03 裁「禁止删激活目标，先切换」「5–200 正整数」 |
| R-04 | BLOCKING | "完整账本"不能照搬 story_list 最多 16 条窗口 | K1/K2 设计分页或按需取页及首尾 canary；契约包含分页请求/应答 |
| R-05 | BLOCKING | 两通道分别写文件/算余额会产生双写者、并发与重复扣款 | D-10 确定性单 writer；K1 统一原子写、余额重算、短窗语义去重与传输幂等分离 |
| R-06 | BLOCKING | 契约缺口不止新 view，还包括控制意图、prompt、会话语境、设备删除与分页 | 契约包按 §1.3/1.4/1.5/1.6/§四完整起草，与架构同批原子落 |
| R-07 | MAJOR | QQ 删除和配置需要澄清，而 ask_user 会卡死 | QQ 命令一次性可判格式 + 普通文字错误回复；稳定 id/最近列表序号定位；真实 QQ 验收含歧义输入不卡死 |
| R-08 | MAJOR | 冷启动、数据未达、确无分录、断网写失败、其他页推送是不同状态 | K2 分别设验收断言；RAM 缓存可恢复，持久账本只在云端；设备主动语音成功后才跳页，QQ 永不抢前台 |
| R-09 | MAJOR | 插入 P3 后 kidpal_story_ctx_source_page(case 2) 会把积分页误报为故事语境；阅读页另有已登记 KEY 路由缺陷 | 改枚举驱动映射并更新既有冻结验收；K5 消费 TECH-261028；逐页验证 KEY 开会话后的 source_page |
| R-10 | MAJOR | ideas 前文"我们去完游乐场啦"与后文"必须事由+动作+分值"表面冲突 | 以后段 2026-11-03 终审门槛为准；不完整句只走普通对话不落账，纳入正负两类验收 |
| R-11 | MAJOR | 400×300、1bpp、14px 下限、长原因文本、超目标刻度与新增美术容量可能破坏版面 | 需求档 2 出 frozen PNG；K2/K4 做文本截断、大分值、越目标与资源预算验收，真机看屏终裁 |
| R-12 | MAJOR | 既有 QQ/WS/OpenClaw 易被误判为非新机制而跳过验证 | spike-first 命中明确，K0 先于所有正式实现并以设备消费结果为证据 |
| R-13 | MAJOR | 六包共享 front、prompt、render_node、keybar，按层并行互踩 | K0→K5 串行主干；跨包行为写入 decision-coverage；K5 负责导航并集 + 整列车收口轮 |

## architect sign-off

分流 review 结论为 **③拆包可提交 human 触点①确认，spike-first 必须保留**。上述 BLOCKING 项均已转化为需求 OQ、架构/契约任务或包级 launch 前置，没有留给实现 Pod 自裁的悬空项。本 sign-off 只表示 reviewed plan 的方向与拆包依据成立，不代表需求、架构、契约已经冻结，也不授权任何包 launch；PROPOSAL 的 FROZEN 仍归 human。

（签署：architect 分流 review 实例，2026-11-03；主线程搬运落盘，OQ 裁定回灌见 PROPOSAL §3.2 与 spine openQ。）

## 门 1（拆包四件套 checklist review，2026-11-04）

- reviewer：architect（主线程另派，两轮 = 全量 checklist + 修正后聚焦复检）；制品 = roadmap.md + decision-coverage.json（13 LP）+ 7× delivery/taskpackages/261104-k*（taskpackage.json + plan.md）+ board 初始化（mergeOrder 70–76，K0a∥K0 并行对、其余 WIP=1 串行）。
- **第一轮 verdict：BLOCK**（轻量修正型，七包方向与依赖主干成立，不放行 commit）。findings：F-01（BLOCKING）K0a/K0 双认领 host_app CMakeLists、「文件零重叠」声明失真；F-02（BLOCKING）K5 漏 S-F2.1 AC7；F-03（BLOCKING）K3 漏挂 NFR-PERF-4；F-04（MAJOR）K0 双状态证据一笔 QQ 消息不闭合；F-05（MAJOR）task_plan 风险字段漂移（K1 应 critical、K3/K4 应 high、medium-high 非法枚举）。
- **LP7 裁定（第一轮内闭环）**：K0 QQ producer = 真实 QQ 一次性明确指令（带唯一 canary token）→ 既有 main agent → K0 暴露的最小 add CLI；**不建临时 skill**。四条禁令：禁直编 points/state.json、禁终端调 CLI 冒充 QQ、禁本地 OpenClaw 注入冒充、禁提前建 K3 正式 skill。LP7 verified 只覆盖预评可达性；真实执行由 K0 spike 证据判定、不预先标绿。
- **planner 修正轮**：六项全部落盘——F-01 改「唯一共享构建清单 + K0a 先落、K0 重取基线后追加」（K0 的 host/device build、真机终验、ready-to-merge 后置到 K0a merged 后）；F-02 K5 回归集 AC1–AC7 + S-F2.1-AC7 sourceRef；F-03 K3 挂 NFR-PERF-4（sourceRefs/acceptanceRefs/K3Q-E4 实测时延不写死阈值）+ K4 顺带同时延字段；F-04 K0S-E3 首选两笔同形真实 QQ canary（其他页 / 积分页在屏各一笔）+ 单笔时诚实降级口径；F-05 task_plan 风险值对齐；LP7 evidence 回填本节。
- **聚焦复检 verdict：GREEN**——五 findings 逐项闭环确认（K0a/K0 changeManifest 交集机械重算恰为 firmware/main/test/host_app/main/CMakeLists.txt 一处；AC1–AC7、NFR-PERF-4、两笔 canary、风险值、LP7 预评/执行边界均在位，无新增阻塞项）；K0a/K0/K3/K4/K5 五个改动包 devloop-artifact-check.mjs 全部 ok:true、advisories 空。
- **sign-off：门 1 放行「human 过目切法 → commit」**；launchPreconditions 中 LP5（门 1 GREEN + human 过目）随本节落盘 + human 过目后闭环，LP6（commit 专用 base ref）随 commit 闭环。
