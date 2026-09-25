# Contract Change Packet · M4 story_list 语义拆分 + story_text seq 承载 + 删除载体新增

> Change ID: 260924-m4-k1-story-cloud-link（随包契约变更）
> Date: 2026-09-24 ｜ Status: **HUMAN_APPROVED**（2026-09-24 human 契约门裁十问，裁定记录见 §5；四分类预审 NEEDS_REVISION → v2 修订并入同日）
> 关联: `delivery/workstreams/m4-star-notebook/` ｜ 依据骨架 = `.claude/references/contract-change-evidence.md` ｜ 预审记录 = 主线程会话 2026-09-24 architect-reviewer 报告（裁定表见 §0，漏问点 1–7 已并入 Scope/Human Question）

## 0. 四分类预审裁定（architect-reviewer 2026-09-24，moderator 复判不受影响）

| 触点 | 分类 | 依据摘要 |
|---|---|---|
| ① §1.3③ story_list total 语义拆分 + 行为句改写 | **spec↔contract 漂移**（收敛方向唯一 = contract 追已冻结 spec；CDQ 行沿用 M3-K1 先例标签 `ACCOMPANIES_SPEC_CHANGE` 保队列一致） | 契约现文 total=「故事果计数」兼盖章 N 源 + 行为句「KEY 选择」 ↔ requirements v1.47（S-F2.4 AC2 计数/流水号拆分）/v1.26/v1.27 |
| ②a §1.3④ story_text 增 `data.seq`（additive） | **独立契约变更**（`CONTRACT_CHANGE_REQUIRED`） | spec v1.47 只钉 N 语义、不钉载体——载体是契约层新造（先例 = §1.3③ total CDQ-260829、§1.3⑥ upcoming CDQ-260921） |
| ②b §1.3④ 朗读句清理 | **spec↔contract 漂移** | 契约行为句「或仅靠 TTS 朗读」↔ requirements v1.52 砍朗读（§8 已砍清单） |
| ③ §四 `kidpal_story_delete` 新增 | **独立契约变更**（`CONTRACT_CHANGE_REQUIRED`） | arch §5.4【链路载体待契约】+ §7 契约门在办②；判据一致先例 = CDQ-260914 kidpal_session_ctx |
| 连带 §1.5 职责句 / §1.6 story 段（Q9）/ display_schema story 闸（Q8） | 随触点②③连带 | M3-K1 §1.5/§1.6 连带改写先例 |
| §1.4 control 表 | **无需改**（`NO_CONTRACT_CHANGE`） | 三 action 语义不动；删除为端→云方向、control 为云→端，丙案否决成立 |

## 1. Scope（要改什么）

`docs/contracts/interface-contract.md` 三触点 + 连带，及随包钉行收口面：

### 触点① E1 §1.3③ `story_list` —— total 语义拆分 + 板子行为句改写（spec↔contract 漂移对齐）

现状（2026-08-29 版）：`data.total` 现文 = 「孩子已录故事总数 = **故事果计数**……板子据此渲染 P3 右上角故事果计数器（×N）**与盖章帧「第 N 颗」**」——一个字段兼作两义；板子行为句 = 「切到 P3 故事页，显示标题列表，**KEY 选择**」（v1.27 前旧交互）；节标题「故事列表」为 v1.26 前旧术语。

拟改（requirements v1.47/v1.26/v1.27 冻结口径）：

```json
{ "view": "story_list",
  "data": { "total": 5,
    "items": [ { "id": "2026-09-20-xiaotuzi", "title": "我和小兔子的冒险", "date": "09-20" } ] } }
```

- **`total` 语义收窄 = 现存篇数**（星星计数 ×N，S-F2.1 AC1；**确认删除即 −1**，S-F2.1 AC4/S-F2.4 AC2 v1.47 拆分口径）——不再兼作盖章 N 源；缺省回退 `items.length` 条款保留；「items 分页/截断时不得以 items.length 顶替 total」警示句**保留且更 load-bearing**（预审核实：Q2 上限口径裁定后此句为板端计数正确性的唯一防线）。
- **盖章 N（入册流水号）迁出 total** → 承载见触点②（story_text.data.seq）。
- **板子行为句改写**：「切到 P3 星星记录本，渲染列表（星星图标+标题+日期、选中行带边框、分屏指示；手势族权威 = requirements §2.3 矩阵——契约不复制交互细节，只钉数据消费与降级：畸形/缺字段拒绝并友好降级，不盲渲染 V27）」。旧「KEY 选择」句废。
- **术语对账**：正文「故事果计数」→「星星计数」（v1.26 改名）；节标题「③ story_list —— 故事列表」→「星星记录本列表」是否一并改，随 Q1 裁（wire 字段名 `story_list`/`total` 不变，Q10）。
- **items 上限口径（预审漏问点 1，新纳入）**：板端 `KSTORY_LIST_MAX=16`（story_page.h:40）且 `kidpal_story_list_parse` 超限**整单 -1 降级不静默截断**（story_page.c:556-564）——「全量推送」与 16-cap 硬冲突（>16 篇即整列表被拒）。三案见 Q2；裁定结果落契约「items 条数上限与超限行为」条款 + K1 推送 producer/K2 cap 值同批对齐。

### 触点② E1 §1.3④ `story_text` —— 增可选 `data.seq`（additive，独立契约变更）+ 朗读句清理（漂移）

现状：三键 `{id,title,text}`（story.py:211 钉行「data keys 恰为 {id,title,text}」）；板子行为句 = 「P3 故事页显示标题+正文（滚动），**或仅靠 TTS 朗读**」；节标题「故事正文（回放/刚存好）」含旧术语「回放」（v1.44 措辞对账面）。

拟改：

```json
{ "view": "story_text",
  "data": { "id": "2026-09-24-maoxian", "title": "我的大冒险", "text": "……", "seq": 6 } }
```

- **`seq`（可选整数，additive——缺省时设备行为不变）** = 入册流水号，**只增不减**（S-F2.4 AC2 v1.47：「第 N 颗已入册」是历史成就、不被删除改写）；producer = front（story.py 持久账本，删除不回退）；板端盖章帧 N 唯一权威源（易失 `stamp_index = counter + 1` 本地现算退役——story_page.c:1071）。
- **行为句清理**：「或仅靠 TTS 朗读」删除（v1.52 human 裁砍朗读）；改「P3 进阅读态显示标题+正文（分屏翻页，手势权威 = requirements §2.3 矩阵）；`fresh_save` 场景（刚存好）触发盖章演出（文案『星星徽章·第 {seq} 颗已入册！』、seq 缺省时降级演出形态随 Q4 裁）」。
- 节标题「回放」措辞随 Q1 术语对账一并裁。

### 触点③ §四 端云协议剖面 —— 新增 `kidpal_story_delete` 消息（端→网关，独立契约变更）

现状：删除云端同步无载体（arch §5.4 明写【链路载体待契约】；§7 契约门在办②）；设备发起的云端**写**操作在契约无先例（kidpal_session_ctx 为单向**通知**先例；ADR-005 备选①放弃理由钉「现无 device RPC 先例」边界）。

拟增（§四，kidpal_session_ctx 条目之后）：

- **`kidpal_story_delete` 消息**（端→网关，KidPal 私有扩展消息）：设备在删除确认子态 KEY 确认后发送 `{"type":"kidpal_story_delete","id":"<story-id>"}`——id 取值 = story_list items 下发过的 id（闭集来源，设备不造 id）。**网关消费并拦截、不下透引擎**：front 机械执行（哨兵头校验 + `<stories_dir>/<id>.md` 删除 + 流水号账本不回退）→ 删除后 story_list 重推对账（total −1 可见）。**单向通知、无应答消息**——可靠性强化形态随 Q5 裁（拟：story_list 重推 = 隐式 ack，设备未收到重推超时重发 delete、front 幂等；不新增请求-应答消息对，ADR-005 边界保持）。不存在 id = 幂等 no-op（front 仍重推列表对账）；非法 id（路径穿越/无哨兵头）拒绝 + 审计日志。省略整条消息 = 无删除，向后兼容。删除的产品语义（硬删 vs 软删/回收站）与连带边界随 Q6 裁。
- **§1.5 网关职责连带**：增写 story 面机械职责句（story_list 推送 producer + kidpal_story_delete 机械执行——「机械计算与校验，非自研业务逻辑，零自研红线不变」注记同 schedule 先例形态）；白名单注记无改（7 view 不动——kidpal_story_delete 为 §四消息非 display view）。
- **display_schema.py 校验面（预审 2.1-4 失真修正 + Q8）**：现状 story_list/story_text **无任何形状 floor**（全文仅 schedule 一条 shape floor，display_schema.py:87-105；story_* 仅受白名单+data 存在性+大小上限约束）——旧版 CCP「收紧」表述失真，实为**新增校验面或维持零校验**的二选一，随 Q8 裁。
- **§1.6 system prompt 约束模板 story 段（预审 2.1-2 + Q9）**：现模板仅有课表段（2026-09-21 契约门改写）；story_list/story_text 收归 front 机械生产后，模板应对称增 story 段（「涉及星星记录本时只用文字答复，不产 story_list/story_text JSON」）——随本 diff 改写或 defer 归 K3 v7，随 Q9 裁。
- **§1.4 control 表零改动**（start_recording/end_session/stop_speaking 既有语义不动；删除不走 control——方向语义不符，§2 丙案否决）。

### 随包钉行收口面（预审 2.1-1/2.3 补，pass-oracle 第 4 关人工 diff 逐字核对项）

| 钉行处 | 现文（旧语义） | 收口动作 |
|---|---|---|
| `server/gateway/story.py:12-13` 模块 docstring | 「契约零改动红线：不新增 view、不给 story_list / story_text 加字段」 | 随契约门同 PR 更新（seq 即加字段——钉行由契约门裁定取代，M3 schedule.py 钉行先例） |
| `server/gateway/story.py:181-183` list_stories docstring | 「板子右上角 ×N 与盖章『第 N 颗』据此渲染」（total 兼两义） | 改 total 单义（现存篇数）；items 上限口径随 Q2 落 |
| `server/gateway/story.py:211` story_text docstring | 「data keys 恰为 {id,title,text}」 | 改四键（seq 可选） |
| `server/deploy/openclaw-system-prompt.v6.md:75-76/:153-155/:271-273` | view 表 story 形状 + 「total=全集总数…盖章『第 N 颗』据此渲染」+「故事果计数取 story_list.total（全集）」 | **v7 逐条收口**（归 K3，Q7 裁定双源窗口处置后 v6 产块指令在 front 闸口事实失效；v6 文件本身不回改——版本化演进纪律） |

## 2. Alternatives（考虑过什么）

- 删除载体乙案 = HTTP `POST /command` 复用：**否**——设备侧无 HTTP 客户端面（WS 常驻唯一通道，§四传输层口径），为删除新开 HTTP 通道是更重的设备面。
- 删除载体丙案 = display `control` 块反向复用：**否**——control 是云→端方向（OpenClaw 判意图后下发），端→云删除塞进 control 语义错位；且 control 经 LLM/display 校验链，删除是纯机械端侧动作不该过 LLM 面。
- 删除走 RPC 应答（设备等确认再移除条目）：**否**——契约无 device RPC 先例（ADR-005 备选①同款放弃理由逐字对齐：「现无 device RPC 先例、kidpal_session_ctx 是单向通知」）；儿童产品删除体验以本地即时 + 异步对账为宜。**可靠性缺口正视（预审漏问点 5 并入）**：单向无应答下 delete 帧丢失 = front 不删 → 下次重推该条**复活**（儿童视角「删掉的故事又回来了」）；删除是持久破坏性写、容错要求高于 restore 布尔字段——强化形态三选一见 Q5（裸甲案接受罕见复活 / 隐式 ack + 超时重发 / 新造应答消息对【否——破 ADR-005 边界】）。
- seq 放 story_list items（每条带 seq）：**否**——盖章触发点 = fresh_save（story_text 回传时），单点携带最省；列表条目无 seq 消费场景（计数用 total）；items 形状零改动最小化消费面。
- seq 新造 view（story_stamp）：**否**——盖章演出判定（fresh_save）与 story_text 消费同点，新 view 徒增白名单与设备路由面（M3 schedule_grid 备选①同款放弃理由）。
- total 拆两个字段（total + count）：**否**——total 语义收窄为单义（现存篇数）+ seq 迁出即可，双字段冗余。
- items 上限：见 Q2 三案（front 分页 / 板端抬 cap / 最新窗口截断）——各自代价（device RPC 面 / RAM 与 64KB 载荷上限 / 长列表浏览语义缺损）在 Q2 内列明。

## 3. Impact（影响谁）

| 消费方 | 影响 |
|---|---|
| 板端 `story_page.c` / `mcp_server.cc` story 路由（M4-K2 重构中） | 消费新形状（total 单义 + seq）；旧冻结输入表/易失 stamp_index 随 K2 重写退役，无并存期；`KSTORY_LIST_MAX` 值随 Q2 裁定同批对齐 |
| 板端删除发送（M4-K2 新接线） | `kidpal_story_delete` 发送原语（protocol 层，与 K3 session_ctx 发送同族新增）；隐式 ack 重发形态随 Q5 |
| `gateway/front_app.py` / `gateway/story.py`（M4-K1 本包） | 拦截/推送/删除执行/seq 账本接线 + 三处钉行更新（§1 钉行表）；StoryAccumulator 既有语义零偏移 |
| `gateway/display_schema.py` | view 白名单**零扩**（kidpal_story_delete 为 §四消息非 display view）；story 形状 floor 新增与否随 Q8 |
| OpenClaw prompt（M4-K3 bump v7） | v6 三处 story 钉行（§1 钉行表）逐条收口；**双源窗口正视（预审漏问点 6——撤旧版「无双源漂移」断言）**：v6 仍指示 LLM 产 story_list 块且 OpenClaw 无 story 工具、一回合不读写文件 ⇒ LLM 产块 = 模型臆造、与 front 机械推送构成**真双源**；窗口处置随 Q7 裁（拟 front relay 收紧 story_* = 机械推送单源） |
| 过渡窗口期（板端旧冻结面） | K1 契约落地 → K2/K3 合并之间设备仍旧故事页：total 数值语义变化对旧板端渲染无破坏（`kidpal_story_counter` story_page.c:615-626 优先 total，同为 ×N 数字）；seq additive 旧板端忽略（story_text_parse :707-712 跳过未知键）；**盖章面例外注（预审补）**：旧板端盖章 N = `stamp_index = counter + 1` 本地现算（:1071）——窗口内若发生删除（total < seq），旧板端盖章 N 与新语义分叉；因旧板端无删除触发面（删除 UI 归 K2）且窗口短，实际风险低，如实登记不遮蔽 |
| 既有冻结验收 | 板/云 story_p3 旧件均已 archive 退役，无 supersede 面；`story_page.h` FROZEN OFFLINE CONTRACT 头注随 K2 声明退役口径 |

**随包收口义务（gate evidence，先例 = M3-K1）**：① `architecture-gate-evidence.md` 十节 ledger（§3 收编 §0 预审裁定表）；② `evidence/` 原始验证输出（validate_display 样本：story_list 新语义 / story_text 带 seq 与缺 seq / kidpal_story_delete 非 display 面负样本）；③ CDQ 行登记（触点①=漂移收口行、②a seq 与 ③ delete 各 `CONTRACT_CHANGE_REQUIRED` 行，`docs/CONTRACT-DECISION-QUEUE.md`）；④ ADR 追加（删除载体选型 + seq 流水号语义 + Q2 上限口径 = durable semantics）+ 契约正文变更记录行（三触点各一行，先例 = §1.3③ total 2026-08-29 记录行）；⑤ pass-oracle 第 4 关无脚本 gatekeeper（ARQ-260827-contract-mech-gate-gap 在册）——ledger §5/§8 如实记「人工 diff 逐字核对」降级，**核对清单 = §1 钉行表四处 + Q2/Q5/Q7/Q8/Q9 裁定落文处 + 变更记录行**（预审修订清单第 7 项）。

## 4. Human Question（请契约门裁定，十问一次问全——预审漏问点 1–7 已并入）

1. **触点① story_list**：`total` 语义收窄 = 现存篇数（删除即 −1）+ 盖章 N 迁出 + 板子行为句改写（交互细节引 requirements §2.3 矩阵、契约只钉数据消费与降级）+ 术语对账（正文「故事果→星星计数」；节标题「故事列表→星星记录本列表」、story_text 节标题「回放」措辞是否一并改）+「items 分页/截断不得以 items.length 顶替 total」警示句保留——是否认可？——拟认可（items 形状 {id,title,date} 零改动；wire 字段名 story_list/total 不变）。
2. **items 上限口径（漏问点 1，与板端 16-cap 硬冲突）**：甲案 = front 推 ≤16 条页 + 板端分页导航（取次页须 device→cloud 请求 = 触 ADR-005「无 device RPC 先例」边界，须同批裁）；乙案 = K2 抬 `KSTORY_LIST_MAX`（拟 64~256，RAM 量级 ~100B/条可忽略；载荷受 `MAX_DATA_BYTES=65536` 自然上限 ≈ 数百条）+ front 推全量、超载荷上限时截最新 N 条 + total 全量（警示句兜底计数正确）；丙案 = front 恒推最新 16 条 + total 全量（板端只见窗口——**损 S-F2.1 AC2 长列表跳屏语义**，v1.30 changelog「30 屏 × 5-6 条/屏」预期不成立）。——拟乙案（spec 长列表语义保真、零新 RPC 面；截断兜底与警示句配套）。
3. **触点② story_text**：可选 `data.seq`（additive，入册流水号只增不减，producer=front 持久账本）+ 朗读句清理（v1.52）+ fresh_save 盖章行为句——是否认可？（分类 = ②a 独立契约变更 / ②b 漂移，§0 裁定表）——拟认可。
4. **seq 缺省降级演出（漏问点 2）**：producer=front 持久账本恒能产 seq，缺省 = 纯防御路径——降级演出形态（不带 N 的文案变体）spec S-F2.4 AC1 逐字文案未定义：接受「板端自定降级形态、留痕即可」，还是需 human 过目定义降级文案？——拟板端自定（防御路径不占 spec 面；真机常态不可达）。
5. **触点③ 删除载体与可靠性（漏问点 5）**：甲案 `kidpal_story_delete` §四端→网关单向私有消息（front 机械执行 + story_list 重推对账、幂等 no-op、非法 id 拒绝审计）——是否认可？可靠性强化：裸甲案（接受 delete 帧丢失 → 重推复活的罕见不一致）vs **隐式 ack + 超时重发**（设备发出 delete 后未收到「不含该 id 的 story_list 重推」即超时重发、front 幂等——不新增应答消息对、ADR-005 边界保持）——拟甲案 + 隐式 ack 重发。
6. **删除产品语义与连带边界（漏问点 3+4）**：硬删（MD 原子删、无恢复）vs 云端软删/回收站/短时可恢复？审计日志留存范围（谁可查）？删除语义边界 = 仅 story MD，还是连带 OpenClaw 侧对话记忆/日志（§8「家长知情权由 OpenClaw 侧完整日志兜底」）？家长侧删除通道（QQ 对 OpenClaw 删故事）本期是否建？——拟：硬删 + front 审计日志（误删防护已由确认子态 + 10s 静默取消承担，requirements v1.48 已裁）；边界 = 仅 story MD（OpenClaw 日志面不动——知情权兜底面恰需保留）；家长删除通道本期不建、挂账登记（§8 小程序已砍、QQ 面录入侧无删除入口现状维持）。
7. **双源窗口处置（漏问点 6）**：K1 落地 → K3 v7 bump 之间，v6 仍指示 LLM 产 story_list/story_text 块（模型臆造，OpenClaw 无 story 工具）——front relay 对 story_* 收紧（**拒绝引擎侧来源的 story_list/story_text display 块**，story 数据只走 front 机械推送单源；机械实现 = relay 路径按 view 过滤，front 自推不经 relay 天然豁免）vs 放行接受臆造双源 vs v7 收口提前为 K1 前置——拟 front relay 收紧（K1 即收口双源；v6 产块指令被闸口事实失效，v7 文面收口仍归 K3 不提前）。
8. **display_schema story 形状闸（预审 2.1-4）**：新增 story_list/story_text 形状 floor（total 整数 ≥0 / items 三键 / seq 可选整数——schedule floor 先例对称、V27 机械执行面补齐）vs 维持零校验（仅白名单+大小上限）——拟新增轻量 floor（与 Q7 收紧同点落地，脏块拒绝并降级提示）。
9. **§1.6 模板 story 段（预审 2.1-2）**：随本 diff 对称增 story 段（「涉及星星记录本时只用文字答复，不产 story_list/story_text JSON」——课表段 2026-09-21 先例形态）vs defer 归 K3 v7——拟随本 diff 一并改写（契约模板与部署制品同口径收口，防模板-prompt 分叉）。
10. **推送触发与冷启动分界（原 Q4）**：story_list 三时点推送（WS 建连 + 归档成功后 + 删除执行后）= 行为策略归 arch §5.2④ 扩展承载、**不进契约**（M3 upcoming 三触发同款分界）；板端「进页即见列表」= 推送 + RAM 缓存（决策 24 介质禁令不变）、**不设 restore 类冷启动保障**（story_list 全量载荷建连即推已覆盖——Q2 乙案下成立）——是否认可此分界？——拟认可。

## 5. Human Ruling（2026-09-24 契约门裁定记录）

> 裁定形态 = AskUserQuestion 四问打包（切法过目 + Q2 单裁 + Q5/Q6 合并裁 + 其余七问一批），human 选择项逐字落账；与拟答的偏差仅 Q2 一处。

- **Q1 认可（拟答）**：story_list `total` 语义收窄 = 现存篇数（删除即 −1）+ 盖章 N 迁出 + 板子行为句改写（交互细节引 requirements §2.3 矩阵、契约只钉数据消费与降级）+ 术语对账（正文「故事果→星星计数」；节标题「故事列表→星星记录本列表」、story_text 节标题「回放」措辞一并改）+「items 分页/截断不得以 items.length 顶替 total」警示句保留；items 形状 {id,title,date} 零改动；wire 字段名 story_list/total 不变。
- **Q2 裁丙案（≠拟答乙案，human 改判）**：story_list items = **front 恒推最新 16 条窗口 + `total` 恒全量**——板端 `KSTORY_LIST_MAX=16` 维持现值不抬、零解析面改动；契约落文钉「items = 最新 ≤16 条窗口（日期倒序）、total = 全量现存篇数、警示句 load-bearing」。**知情注记（如实留痕）**：① S-F2.1 AC2 长列表跳屏语义在 16 条窗口内成立（v1.30「30 屏×5-6 条/屏」动机面收窄，human 过目丙案代价说明后选定）；② 连带面 = 设备端浏览/删除窗口限于最新 16 条，更早条目云端仍在（total 计入、阅读不可达设备端）——旧条目设备端删除可达性随之收窄，本注记为裁定附带后果的首次显式登记，如 human 后续认为需旧条目可达面（分页/查询类），属新需求另立（触 device RPC 边界 = Q2 甲案族，届时再裁）。
- **Q3 认可（拟答）**：story_text 增可选 `data.seq`（additive，入册流水号只增不减，producer=front 持久账本）+ 朗读句清理（v1.52）+ fresh_save 盖章行为句；分类拆标（②a 独立契约变更 / ②b 漂移）在案。
- **Q4 认可（拟答）**：seq 缺省 = 纯防御路径，降级演出形态板端自定（不带 N 文案变体）、不占 spec 面，留痕即可。
- **Q5 认可（拟答组合）**：删除载体 = 甲案 §四 `kidpal_story_delete` 单向私有消息 + **隐式 ack + 超时重发**（设备发 delete 后未收到「不含该 id 的 story_list 重推」即超时重发、front 幂等承接；不新增请求-应答消息对，ADR-005 边界保持）；设备侧重发接线归 K2、front 幂等归 K1。
- **Q6 认可（拟答组合）**：**硬删**（MD 原子删、无恢复）+ front 审计日志（误删防护已由确认子态 + 10s 静默取消承担，requirements v1.48 已裁）；删除语义边界 = **仅 story MD**（不触 OpenClaw 侧对话记忆/日志——§8 家长知情权兜底面恰需保留）；家长侧删除通道（QQ 面）本期不建、挂账登记。
- **Q7 认可（拟答）**：双源窗口处置 = **front relay 收紧**——拒绝引擎侧来源的 story_list/story_text display 块（relay 路径按 view 过滤；front 机械推送不经 relay 天然豁免），story 数据只走机械推送单源；K1 即收口，v6 产块指令被闸口事实失效（v7 文面收口仍归 K3 不提前）。
- **Q8 认可（拟答）**：display_schema 新增 story 轻量形状 floor（total 整数 ≥0 / items 三键 / seq 可选整数——schedule floor 先例对称、V27 机械执行面补齐），与 Q7 收紧同点落地。
- **Q9 认可（拟答）**：契约 §1.6 模板随本 diff 对称增 story 段（「涉及星星记录本时只用文字答复，不产 story_list/story_text JSON」——课表段 2026-09-21 先例形态）。
- **Q10 认可（拟答）**：story_list 三时点推送（WS 建连 + 归档成功后 + 删除执行后）= 行为策略归 arch §5.2④ 扩展承载、不进契约；板端「进页即见列表」= 推送 + RAM 缓存（决策 24 介质禁令不变）；不设 restore 类冷启动保障（丙案下载荷有界 ≤16 条，建连即推已覆盖）。

**同批过目**：门 1 切法（human 认可三包拆法，review.md 门 1 节 GREEN 在案）。
**落地通道**：主线程起草逐字 patch → human 过目确认 → `git apply`（M3 乙通道先例 + CCP 落地通道纪律：CRLF 陷阱自查、收口义务同批、诚实留痕）；收口义务 = §3 ①–⑤（gate evidence 十节 / evidence 样本 / CDQ 三行 / ADR 追加 + 变更记录行 / 人工 diff 核对清单）。
