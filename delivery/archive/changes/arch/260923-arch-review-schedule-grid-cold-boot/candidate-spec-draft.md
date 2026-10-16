# 候选 spec 修订草案 · P2 课表页周网格冷启动恢复时机（schedule grid cold-boot restore）

> **性质声明**：本文件是 `/arch-change`（Track A spec profile）对抗 review 的**起草候选**，作者 = architect（起草角色），非 canonical。任何 `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**` 的改动须经 challenger↔defender 对抗 + adjudicator 裁决 + 契约门（触契约部分）+ human 批准后由主线程 packet-bound 落地。本草案不自评、不写冻结 charter。
> **输入契约核验**：本草案基于 review-moderator 派发 prompt 携带的已查证事实清单（triage 门 + 前序 review 证据）+ 本人独立复核 canonical 锚点（requirements S-F1.2 L260-273、arch §5.2/§6.2/§6.4/§10 决策 24/26、契约 §1.3②/§1.3⑥/§1.5/§2.4（L226 兜底刷新句）、ADR-004、代码五处锚点）。非裸 prompt 起草。
> **版本**：v3（2026-09-23）——对抗 review 收口后按 adjudicator 修复清单 R1-R11 就地回灌 + F-C5 human 裁定（ADR-005 显式记残余风险、不单独设闸，采 defender 方案）：requirements/arch 追加句统一三触发口径并去 R-S0.2 标签（R1/R2）、部署时序按仓内代码定案改写为确定性接管单分支（R3/R4，C1 条件保留）、§3.5 判据对齐决策 24 掉电边界 + NFR-OFF-1 两侧注记入修订总览（R5/R6）、契约句改「缺省或显式 false」（R7）、ADR-005 理由段分层记触发必要性 + K1 验收锚 + 泛化立场（R8/R9）、关联段在屏判定括注（R10）、ADR-005 增残余风险记录段 + §6.1 撤高估表述（R11）。
> **版本史**：v2 = architect-reviewer 契约预审 9 修正 + 2 增补（变更类型记 modifying 分量、L85 逐字改写、字段钉死 `data.restore`、生产纪律去三触发枚举、契约变更记录行、降级双面注记、CCP 落点新目录、C1 closeout condition、out-of-scope 清单；§2.4 佐证、目录名同步）。
> **目录命名注记**：spec-lane 惯例 = `<YYMMDD>-arch-review-<slug>`（如 `260922-arch-review-spec-v5-ui-align`）；moderator 已于起草后裁量改名对齐惯例：本目录现名 `260923-arch-review-schedule-grid-cold-boot`（原 `arch-review-260923-schedule-grid-cold-boot`），命中 scope-map 写槽 glob，机械放行。
> **挂账关联**：m3-schedule 遗留③（`delivery/workstreams/m3-schedule/progress.md:260`「症状 B 课表页冷缓存独立设计入口」）；K6（260923-m3-k6-boot-auto-connect）症状 A 已修（锚① PASS），症状 B 即本 review 对象。

---

## 1. 问题定式（requirements ↔ architecture 漂移）

三份 canonical 的合取在「冷启动后翻到 P2」这一路径上必然产生空态，而 requirements 承诺非空：

1. **需求面承诺**（requirements.md S-F1.2，L262/L265）：「作为孩子，我希望**翻到课表页就看到**这一周周一到周日的课都排在哪」；AC1 =「Given **进课表页**，When 看屏，Then 一周总览网格……」——验收锚定在「进页即见」，无任何「需先语音查询」前置条件。§2.4（L184）与 §6 阶段三（L628）把 P2 一周总览网格钉为 Phase 1 交付项。
2. **架构面无 grid 触发**（architecture.md §5.2 场景① L207 + §10 决策 26 L416）：「接下来」刷新三触发 = WS 连接建立 + 每日 0:00 跨天 + 新建日程后即时推——**三触发只覆盖 home upcoming，schedule view 的推送出口只有语音意图薄载荷一条**（「帮我显示课表」）。设备冷启动建连后，P2 周网格缓存 `s_grid_n`（kidpal_render_node.cc:728-748）没有任何填充路径。
3. **持久化被禁**（architecture.md §6.2 + §10 决策 24 L414）：板端非易失介质白名单 = 凭证 + 预栅格化资产/字体 + 固件本体；一切业务状态只驻 RAM、掉电即失——**NVS 持久化课表被排除**，重启后 RAM 网格缓存必为空。
4. **现状行为**：空缓存 → 友好空态「还没有课表数据，问我『我的课表』吧」（kidpal_render_node.cc:739-750，2026-09-21 有意取舍）——冷启动后孩子 BOOT 翻到 P2 看到的是这句引导语，S-F1.2 AC1「进课表页即见网格」不成立。

**漂移定性**：决策 26 定稿于 2026-09-02（第三轮 grilling），当时 P2 尚为旧列表形契约、无周网格、无 BOOT 翻页读缓存路径；2026-09-14 课表大改（v1.19 周网格）→ 2026-09-21 ADR-004（网格收归 front 机械计算）→ M3-K2（板端周网格渲染 + BOOT 翻页读缓存）逐步落地后，「三触发只刷 upcoming」的旧表述与「P2 = 常驻可翻页 + 翻到即见」的新现实之间出现结构性缺口——**属决策滞后（疏漏补齐），非有意取舍**（时序证据：决策 26 早于周网格设计 12 天；若为有意取舍，M3-K2 不会实现 BOOT 翻页读缓存路径；**契约侧佐证** = 契约 §2.4 L226 副通道句「或家长直接编辑此文件（副通道，无即时推，靠下次连接/每日跨天兜底刷新）」——契约自身已把「下次连接/每日跨天」当作课表数据的兜底刷新时机、未限定只刷 upcoming，grid 恢复挂同一时机与契约既有口径同向）。K6 真机第 1 轮（2026-09-23）冷启动日志实证：建连推送 `display_dispatch view=home upcoming_apply ok` 8.6s 到达、P1 恢复正常，同一时刻 P2 仍空——症状 B 即此缺口。

**约束边界（不可谈判项）**：决策 24 介质禁令不动（恢复机制不得触碰非易失介质）；M3-K3 语音「我的课表」→ schedule 路由前台接管是在役冻结行为，不得回归；alert 轮询起轮（`_register_alert_legs`）不回归；BOOT 翻页缓存读路径不回归。

---

## 2. 候选方案全集

### Shape A · 建连附带推 grid（恢复性推送，restore 语义）

**机制**：front 在三触发的推送点（建连 ① / 每日 0:00 ② / 录入 marker ③，均为现役 piggyback 通道）附带下发一条 schedule view 完整周网格载荷，携带 data 内新可选布尔 `data.restore: true`（与 §1.3⑥ `data.upcoming` additive 先例同位——**字段位置钉死在 data 内，不扩顶层信封概念**）；设备收到 restore 载荷 → 解析、**只填 RAM 网格缓存、不推 SERVER_PAGE 层、不接管前台**，若 P2 恰在屏则仅重绘 P2 本页；`data.restore` 缺省（薄载荷展开的主动展示路径）维持现行为无条件接管前台。

**关键先例（本案的可行性支点）**：home view 已在役实现完全同构的语义——`mcp_server.cc:540-568`：`data.upcoming` 注入 seam 后「P1 不在屏 -> seam 已更新、下次首页落屏自然带出（不抢前台）；P1 在屏 -> 重渲染首页日程主体」。restore 是把同一模式（数据面更新 vs 前台接管分离）从 home 复制到 schedule，data 内只差一个显式布尔字段（home 靠 data 缺席/在场区分新旧语义，schedule 的 data 在场已是「完整网格 + 接管」语义、无法复用缺席区分，故须显式字段——位置同 `data.upcoming` 先例钉死在 data 内）。

**spec 改动点**：① requirements S-F1.2 补充规则追加冷启动恢复一句（§3.3 给逐字文本）；② architecture §5.2 场景① 追加 grid 恢复触发表述（§3.2 给逐字替换文本）；③ 新增 ADR-005（§3.4 给全文草案，五节格式对齐 ADR.md 头部约定；§10 决策历史档冻结不动，修订以 ADR 承载）；④ arch §6.2 澄清半句 + requirements NFR-OFF-1 行与 arch §4 追踪表行注记（R5/R6 已裁三处同 PR：「不缓存」判据对齐决策 24 掉电边界，§3.5/§3.1）。

**契约影响**：**触 §1.3②**——additive 可选字段 `data.restore` + **modifying 既有板子行为条款**（§1.3② L85 现文「板子行为：切到 P2 课表页渲染……」把接管前台写成无条件，与 `data.restore: true` 直接冲突，须逐字改写带 restore 例外，**不得只追加 bullet**）+ 新增 front 生产纪律条款。四分类预判 = **CONTRACT_CHANGE_REQUIRED（随 spec 改动携带、同 PR 原子落地）**；变更类型如实记「additive 字段分量 + modifying 条款分量」，**不标纯 additive**；CCP 骨架见 §4。

**firmware 改动面**（估几十行）：`mcp_server.cc` schedule 路由读 `data.restore` 布尔——true 走新分支：`kidpal_schedule_grid_parse` → 成功则 `kidpal_render_node_cache_schedule_grid` + 若 P2 在屏则重绘 P2（需仿 `kidpal_render_node_home_on_screen` 先例补一个 on-screen 查询/重绘小缝）；**不调 `kidpal_render_node_begin_page`**。降级路径（parse 失败）= **板端入板后静默降级**——记日志、缓存零变化，绝不走现 `SetChatMessage("课表数据有点问题")`（后台推送不得打扰前台）；front 校验面 V27 对脏块的拒绝/降级在入板前独立生效（降级双面注记，见 §4）。`kidpal_render_node.cc` BOOT 翻页读路径零改动。

**server 改动面**（估几十行）：`schedule.py` 零改动（`week_grid_payload` 单源已存在，L375）；`schedule_push.py` 仿 `connect_push_payload`（L73-81）单源委托 `week_grid_payload` 增一个 grid 变体（带 restore 置位）；`front_app.py` 建连推送点（L762-773）在 home upcoming 之后追加一条 grid restore relay（同 model 快照、同 try/except 不阻断建连），每日/ marker piggyback 路径同型追加。

**风险**：① 契约语义碎片化——schedule view 出现双设备行为（接管 vs 恢复），纪律「薄载荷展开恒不置 restore」需验收齿钉死；② 部署时序——旧固件收到 restore 载荷 = **确定性接管前台顶到 P2**（旧固件无 restore 分支：`schedule_page.c:607-613` 对 data 内未知键取忽略语义、`mcp_server.cc:620-644` schedule 路由无条件 begin_page 接管）——每次建连被顶到课表页，不可接受；已按裁定记 **gate evidence C1 类 closeout condition**：builder 实测复核前 ECS 推送开关保持关（§7）；③ 建连双推（upcoming + grid ~3-4KB）帧序与失败面（一条成功一条失败的不一致窗口）。

**成本**：小。两端各几十行，无新通道、无新消息类型、无持久化、RAM 零新增（`s_grid_courses` 84×60B≈5KB 缓存 M3-K2 已建）。

### Shape B · 设备按需拉取（BOOT 翻到 P2 冷缓存时主动向 server 请求 grid）

**机制**：设备翻页发现缓存空 → 经 WS 发一条 device→cloud 请求消息（小智协议私有扩展，对偶 `kidpal_session_ctx` 形态）→ front 应答 grid 载荷。

**spec 改动点**：arch §5.2 新增拉取场景 + §6.4 语义扩写；requirements S-F1.2 补充规则改写数据口径。

**契约影响**：**触 §四**（端云协议剖面新增 device→cloud 请求/应答消息对，含关联 id、超时、重试语义）——比 Shape A 的一个布尔字段重得多；且首例「设备主动拉业务数据」消息，开创后每类冷缓存数据都会要求同型扩展（P3 故事列表同病）。

**firmware/server 改动面**：新消息类型解析 + 请求-应答关联机制（小智协议现无 device 发起 RPC 的先例，`kidpal_session_ctx` 是单向通知、无应答）+ 翻页路径挂异步等待 + 等待期 UI 态定义。中等偏大。

**风险**：① 哑终端纯洁性侵蚀——NFR-OFF-1「设备零轮询」虽非字面禁止单次拉取，但「设备知道业务数据缺什么并主动要」是职责倒挂的第一步（ADR-002 放弃原因④同型论证）；② 翻页体验引入网络往返延迟（弱网下翻页卡住或闪空态再跳内容，儿童产品体验劣化）；③ 请求-应答关联是全新失败面（超时/乱序/重连中断）。

**成本**：中高。**放弃**（推荐位：否决，但作为「契约面最小化」的反衬保留在案）。

### Shape C · 需求面砍（改写 S-F1.2，接受空态为正式口径）

**机制**：S-F1.2 措辞改写为「语音查询后翻页可见」——把 AC1 的「进课表页即见」弱化为「本开机周期内收到过课表推送后可见」，空态文案升格为正式行为。

**spec 改动点**：仅 requirements S-F1.2 重写（正文 + AC1 + 补充规则）；architecture 零改动（现状即合规）。

**契约影响**：零（契约不描述推送时机）。

**风险**：① 背叛 v1.19 课表大改的产品意图——「翻到即见整周谱」是该改版的核心用户价值（human 亲自指示的改版），砍它等于回退 human 决策，须 human 显式拍板；② 产品不一致——同一次冷启动 P1「接下来」8.6 秒自动恢复（K6 锚① PASS、human 目测确认「有内容了」）、P2 却要求孩子先开口说话，同屏两页两种恢复哲学，孩子无法理解；③ 空态文案「问我『我的课表』吧」把翻页手势变成死路，BOOT 翻页路径（M3-K2 专门实现）价值减半。

**成本**：零实现，纯文档。**放弃**（推荐位：否决；仅当契约门拒批 restore 字段时作为 fallback 升 human）。

### Shape D（补充候选）· 行为启发式：无新字段，设备按「建连后 N 秒内收到的 schedule 载荷 = 恢复、语音轮中收到 = 展示」自判

**机制**：wire 零改动，设备用时间窗/会话态启发式区分恢复性推送与主动展示。

**风险**：脆弱——语音会话在建连后立即发起（孩子开机就喊「帮我显示课表」）时启发式误判为恢复、拒绝接管 = 冻结行为回归；时间窗取值无 principled 依据。**否决**：dispatch 已预判「行为启发式（脆弱）」，独立复核确认——把 wire 语义藏进时间推断是 drift 温床（对偶 ADR-001 教训：断言耦合实现细节而非结构化指向）。

### 推荐位汇总

| Shape | 推荐位 | 一句话 |
|---|---|---|
| **A（建连/触发附带推 grid，restore 语义）** | **推荐** | 同构先例在役（home upcoming）、契约增量最小（一个 additive 布尔 + 一处既有条款 modifying）、哑终端与介质禁令零触碰 |
| B（设备按需拉取） | 否决 | 新协议面 + 职责倒挂 + 翻页引入网络延迟 |
| C（需求面砍） | 否决（fallback） | 背叛 v1.19 human 改版意图 + 与 P1 恢复行为不一致 |
| D（时间窗启发式） | 否决 | 脆弱、回归风险 |

---

## 3. 推荐 Shape A：修订条文草案（逐字精度，可直接搬进 canonical）

### 3.1 修订总览

| 文件 | 位置 | 动作 |
|---|---|---|
| docs/requirements.md | S-F1.2 补充规则（L271） | 追加一句冷启动恢复口径 |
| docs/requirements.md | NFR-OFF-1 行（L594） | 追加半句「RAM 渲染缓冲不禁、掉电即失」注记（R6，与 §3.5 同 PR） |
| docs/architecture.md | §5.2 场景①（L207） | 段末追加 grid 恢复触发表述 |
| docs/architecture.md | §4 追踪表 NFR-OFF-1 行（L149） | 追加半句「RAM 渲染缓冲不禁、掉电即失」注记（R6，与 §3.5 同 PR） |
| docs/architecture.md | §6.2（L315） | 追加 RAM 渲染缓冲澄清半句（R5 判据已对齐决策 24 掉电边界，与 NFR-OFF-1 两侧注记同 PR） |
| docs/ADR.md | 新条目 | append ADR-005（§10 决策历史档冻结不动，修订以 ADR 承载——ADR.md 头部约定） |
| docs/contracts/interface-contract.md | §1.3② | additive 可选字段 `data.restore` + **modifying 既有板子行为句（L85 逐字改写）** + 生产纪律句 + 契约变更记录行（**走契约门，同 PR 原子落地**） |

requirements S-F1.2 的正文、AC1-AC4、锚、可行性行**零改动**——Shape A 使既有验收标准原样成立，这正是选它的判据之一。

### 3.2 architecture.md §5.2 场景① 追加文本（接 L207 段末）

> P2 周网格缓存恢复 = 三触发（与「接下来」同源同机）的附带推送：WS 建连、每日 0:00 跨天与录入 marker 时，front 在推「接下来」后附带重推周网格**恢复性载荷**（契约 §1.3② `data.restore: true` 语义：设备只填 RAM 网格缓存、不接管前台，P2 在屏时仅重绘本页）。决策 26 三触发表述只覆盖 upcoming，网格恢复面由 ADR-005 补齐；语音「帮我显示课表」意图薄载荷展开仍为主动展示、无条件接管前台（M3-K3 冻结行为）。断网或推送未达期间翻到 P2 = 友好空态（决策 24 离线边界不变，网格缓存属 RAM 渲染缓冲、§5.4 板端状态行）。

### 3.3 requirements.md S-F1.2 补充规则追加句（接 L271 段末）

> 冷启动恢复 = WS 建连与每日 0:00 跨天时云端附带重推周网格（恢复性推送，只填缓存不抢前台，P2 在屏时刷新本页；录入 marker 同型附带，触发时机权威承载见 arch §5.2 / ADR-005）；断网或未收到推送时翻到本页显示友好空态提示（引导孩子用语音问课表），联网恢复后下次建连自动补齐。

### 3.4 ADR-005 全文草案（append 进 docs/ADR.md，五节格式）

> ## ADR-005 · P2 周网格冷启动恢复采「恢复性推送」：三触发附带重推 grid（`data.restore` 语义，只填缓存不接管前台）
>
> - **日期**：2026-09-23（260923-arch-review-schedule-grid-cold-boot 对抗 review；K6 症状 B 独立设计入口，m3-schedule 遗留③）
> - **决策**：P2 周网格的冷启动恢复不新增触发、不新增消息类型、不触碰决策 24 介质禁令——在「接下来」三触发（决策 26：WS 建连 + 每日 0:00 跨天 + 新建日程即时，producer = front 单源 schedule.py）的推送点**附带重推周网格恢复性载荷**：契约 §1.3② 增 data 内可选布尔 `data.restore`（additive 字段分量；既有「板子行为」条款同步 modifying——无条件接管句逐字改写为带 restore 例外），设备收到 `data.restore: true` 载荷只填 RAM 网格缓存（M3-K2 既有 `s_grid_courses` 渲染缓冲）、不推 SERVER_PAGE 层、不接管前台，P2 在屏时仅重绘本页（同构先例 = home view `data.upcoming` 注入 seam 的「不抢前台」语义，mcp_server.cc home 路由）；`data.restore` 缺省 = 现行为不变（薄载荷展开的语音主动展示无条件接管前台，M3-K3 冻结行为）。恢复性载荷板端入板后解析失败降级 = 静默记日志，不弹提示、不动前台（区别于主动展示路径的 SetChatMessage 降级；front 校验面 V27 降级在入板前独立生效）。断网/未达期间 P2 = 友好空态（空态提示引导语音问课表），离线边界与决策 24 零变化。**部署时序** = 固件先行；旧固件对 `data.restore` 载荷的行为已由仓内代码定案 = **确定性接管前台顶到 P2**（旧固件无 restore 分支，`schedule_page.c:607-613` 对 data 内未知键取忽略语义 + `mcp_server.cc:620-644` schedule 路由无条件 begin_page 为证）；builder 实测复核确认前 ECS 推送开关保持默认关，烧板验证后人工开闸（gate evidence C1 类 closeout condition）。
> - **理由**：决策 26 定稿（2026-09-02）时 P2 尚为旧列表形、无周网格与 BOOT 翻页读缓存路径，三触发只列 upcoming 属滞后疏漏而非取舍（课表大改 v1.19 在 12 天后）；K6 真机实证同一次冷启动 P1 8.6s 自动恢复、P2 恒空，S-F1.2 AC1「进课表页即见」不成立；K1 验收锚 M3K1-A5（`delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/taskpackage.json` structural）已写「真机联网/零点/录入三时点首页与课表页实际刷新目击」（deferred 归 M3-K3 设备轮）——三时点课表页刷新在建包期已是预期面。恢复机制的最小正确形状 = 把「数据面更新」与「前台接管」在 schedule view 上分离——home 已用 data.upcoming 在役验证了该分离，restore 只是把同一语义显式化为一个 additive 字段；设备保持纯接收端（决策 24 哑终端零侵蚀），云端保持唯一 producer（ADR-004 单源纪律零变化）。**触发必要性分层如实记**：触发②（每日跨天）有独立必要性——在线挂 P2 过零点需刷新 ※ 过期项与星标位（bit0/bit1 仅云端可算），缺触发②则 stale grid 呈现；触发③（录入 marker）无独立必要性——其附带属**不变式维护**（三触发同源同附带，比「两附带一不附带」好守）与**产品直觉一致性**（家长刚录入、孩子翻页立见新内容），不与触发②并列同等必要。**泛化立场**：本机制 = 「数据面更新 vs 前台接管分离」模式的第二实例（home `data.upcoming` 为首个）；后续同类冷缓存需求逐案走各自 review，不自动套用、不预扩字段。
> - **备选与放弃原因**：① **设备按需拉取**（BOOT 翻到冷缓存时 device→cloud 请求 grid）——放弃：需在 §四端云协议剖面新增首例「设备发起请求-应答」消息对（现无 device RPC 先例、`kidpal_session_ctx` 是单向通知），契约面远重于一个布尔字段；且设备知晓业务数据缺口并主动索取 = 职责倒挂（ADR-002 放弃原因④同型），翻页体验引入网络往返延迟；② **需求面砍**（S-F1.2 改写为「语音查询后可见」）——放弃：背叛 v1.19 课表大改的 human 意图（「翻到即见整周谱」是改版核心价值），且与 P1 自动恢复形成同屏双哲学的产品不一致；仅当契约门拒批 restore 字段时升 human 作 fallback；③ **时间窗启发式**（无新字段，设备按建连后 N 秒自判恢复/展示）——放弃：开机即语音「帮我显示课表」时误判拒绝接管 = 冻结行为回归，时间窗取值无原则性依据，wire 语义藏进时间推断是 drift 温床；④ **NVS 持久化网格**——直接违反决策 24，不进入备选。
> - **残余风险记录（F-C5，human 裁定 2026-09-23，采 defender 方案）**：LLM 绕过薄载荷约束直产带 `data.restore` 的完整网格理论上可过 shape floor 达设备、静默填缓存——与既存的 LLM 直产完整网格直达路径〔危害更高：接管前台〕同类且更低；根因缓解 = system prompt 薄载荷约束 + ADR-004 单源；不单独为 restore 键设运行时闸（与 §1.3② 既有 producer 纪律的执行地位对称；单独设闸对更重的既存洞不设防）。如未来整体收紧 LLM 直产 display 块面（provenance 旁路 + floor 收紧），restore 拒键随该收口一并做。
> - **关联**：spec requirements S-F1.2（补充规则冷启动恢复句）+ arch §5.2 场景①；契约 §1.3②（additive 字段 `data.restore` + 既有板子行为条款 modifying，CONTRACT_CHANGE_REQUIRED 随 spec 同 PR）；决策 24 / 26 / ADR-004；证据 = `delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/`（本 review）+ K6 真机冷启动日志（progress.md 2026-09-23 段，锚① 8.6s upcoming 恢复实证）；实现锚 = mcp_server.cc home 路由不抢前台先例 + kidpal_render_node.cc 网格缓存（在屏判定沿 home on_screen 先例口径；SERVER_PAGE 接管态下 restore 仅填缓存、不重绘）。

### 3.5 architecture.md §6.2 追加澄清半句（R5 已裁：判据对齐决策 24 掉电边界）

§6.2 现文「……不缓存任何事项。」后追加：

> （「不缓存」禁的是落非易失介质的持久层；板端 RAM 渲染缓冲——P2 网格缓存、P1 seam——掉电即失，不在禁列，见 §5.4 板端状态行。）

**动机**：NFR-OFF-1 / §6.2 的「不缓存事项/课表」字面与 M3-K2 起在役的 RAM 网格缓存存在字面张力。对抗 review R5/R6 已裁定：澄清句判据对齐决策 24 掉电边界（「不缓存」禁的是落非易失介质的持久层；RAM 渲染缓冲掉电即失、不禁），且 requirements NFR-OFF-1 行（L594）与 arch §4 追踪表行（L149）同 PR 各补半句注记（§3.1 两新行）——三处一体消解，不留半拉子。

---

## 4. 契约影响预判 + ContractChangePacket 骨架

**四分类预判：CONTRACT_CHANGE_REQUIRED（随 spec 改动携带、同 PR 原子落地）**。推送**时机**仍不进契约（2026-09-21 契约门裁定不变，权威承载 = arch §5.2 + ADR-005）；但**设备对载荷的行为语义**（接管前台 vs 只填缓存）与 **front 生产纪律**（谁可置 restore）是 §1.3② view 定义本体，additive 字段同 `data.upcoming` 先例（§1.3⑥，2026-09-21 契约门批准的 additive 可选字段）。

**契约改动草案**（proposedContractDiff，§1.3② 内四件，逐字）：

**① 既有板子行为句 modifying**（L85，旧句→新句，不得只追加 bullet）：

- 旧句：「板子行为：切到 P2 课表页渲染一周总览网格（学校课逐节、课外课逐门小字竖排、※/星标按 flags 渲染、今天列整列高亮）；薄载荷不直达设备（front 截获展开）；畸形/旧形状 data 由 front 机械校验拒绝并降级提示，不入板盲渲染（V27）。」
- 新句：「板子行为：切到 P2 课表页渲染一周总览网格（学校课逐节、课外课逐门小字竖排、※/星标按 flags 渲染、今天列整列高亮）；`data.restore: true` 时除外——只填网格缓存、不接管前台，P2 在屏时仅重绘本页；薄载荷不直达设备（front 截获展开）；畸形/旧形状 data 由 front 机械校验拒绝并降级提示，不入板盲渲染（V27）。」

**② 新增 `data.restore` bullet**（对齐 §1.3② 既有 bullet 风格）：

> - **`data.restore`**（可选布尔，additive——缺省（absent）或显式 `false` 时设备行为完全不变）：`true` = 恢复性载荷，只填网格缓存、不接管前台（P2 在屏时仅重绘本页）。降级双面注记：restore 载荷**板端入板后**解析失败 = 板端静默降级（记日志、缓存零变化、不弹提示、不动前台）；**front 校验面 V27**（脏块拒绝/降级提示、不入板盲渲染）在入板前独立生效——两层降级互不替代。

**③ 生产纪律句**（不枚举触发——触发时机不进契约，2026-09-21 裁定口径延续）：

> - **生产纪律（谁可置 `data.restore`）**：仅 front 后台附带推送路径可置 `restore`（触发时机属行为策略，权威承载 = arch §5.2 / ADR-005，不进契约）；意图薄载荷 `{view:schedule,data:{}}` 的展开（`expand_schedule_intent_payload`）恒不置——展开产物 = 主动展示、接管前台（M3-K3 冻结行为）。

**④ 契约变更记录行**（对齐 L87/L149 先例格式，契约门批准后随批落）：

> - （契约变更记录：2026-09-23 经契约门批准新增可选 `data.restore` 恢复性推送语义——additive 字段 + 既有板子行为条款 modifying；随 <实现包 id> 同 PR 落地，见 [CONTRACT-CHANGE-PACKET.md](../../delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md)。）

**字段位置与校验缝**：`data.restore` 钉死在 **data 内**（与 §1.3⑥ `data.upcoming` additive 先例同位，不扩顶层信封概念）；front 侧放行/校验落 §1.5 既有 data shape 校验缝（对 restore 键的机械放行属实现细节，§1.5 条款本身不改——见下 out-of-scope）。

**CCP 骨架**（`CONTRACT-CHANGE-PACKET.md`，落 `delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/`，待 architect-reviewer 预审 + human 契约门）：

- **Tier / 面**：Tier-1，E1 display 契约 §1.3②（跨系统直接消费者：front 生产 / 设备路由消费）。
- **变更类型**：additive 可选字段（`data.restore`）+ **modifying 既有板子行为条款**（L85 无条件接管句逐字改写带例外）+ 新增生产纪律条款。对不置 restore 的既有发送方向向后兼容，但既有条款文本被修改——**CCP 变更类型栏如实记 modifying 分量，不标纯 additive**。
- **Gate Scope（确认不改面 / out-of-scope 清单）**：本 gate 只动 §1.3②（L85 板子行为句 + restore bullet + 生产纪律句 + 契约变更记录行）。确认不改 = §1.3⑥ home（`data.upcoming` 语义零变化）、§1.5 网关职责（restore 载荷经既有 mcp display 通道与既有 data shape 校验缝，无新职责条目）、§1.6 system prompt（restore 与 LLM 无关——薄载荷展开恒不置，prompt 零改动）、§2.3 B1 解析规则、§四端云协议剖面与提醒面（无新消息类型、提醒语义零变化）。
- **动机**：spec 修订（S-F1.2 冷启动恢复）随携带；ADR-005。
- **风险**：① 双行为语义复杂度；② 部署时序（旧固件对 restore 载荷 = 确定性接管前台顶到 P2，仓内代码已定案——closeout C1 兜底，见下）；③ 生产纪律靠验收齿钉（§5）。
- **Closeout Conditions（C1 类）**：**C1 = builder 实测复核确认旧固件对 `data.restore` 载荷的确定性接管行为（仓内代码已定案：`schedule_page.c:607-613` 未知键忽略 + `mcp_server.cc:620-644` 无条件接管——复核确认而非定案）之前，ECS 推送开关保持默认关**；owner = 实现包 builder / integrator；required evidence = 旧固件行为复核实测记录 + 开关状态留痕 + 烧板验证后开闸记录。
- **验证**：契约 diff 逐字人工核对（本仓无 schema 校验器，contracts rule 注记口径）+ gate evidence 按 `.claude/references/contract-change-evidence.md` 十节骨架（Gate Scope / Classification / Human Decision / Applied Diff Inventory〔additive vs modifying 分列〕/ Closeout Conditions / Validation / Docs Sync / Verdict）。
- **裁决请求**：human 契约门批准「additive 字段 + modifying 条款 + 生产纪律句」；可 CONDITIONAL-APPROVE（C1 closeout condition 挂账），批准后与 spec 改动同 PR 原子落地。

---

## 5. 验收锚方向（host 可机判点，供 acceptance-author 起包时取材）

1. **server 推送帧序断言**（host pytest，front 面）：模拟两腿建连 → 断言下发序 = [home upcoming 载荷, schedule restore 载荷]（home 先、grid 后；同 model 快照、`data.restore: true`）；schedule.md 缺失/坏 → 两条均跳过或降级、建连不阻断（现 warning 语义保持）。
2. **薄载荷展开恒不置 restore**（回归守卫，M3-K3 冻结行为）：`expand_schedule_intent_payload` 输出断言不含 `data.restore` 键（或恒 falsy）——生产纪律的机械齿。
3. **firmware 缓存-only 不接管前台**（host acceptance，C 面）：注入 restore 载荷于三种前台态（P1 在屏 / P2 在屏 / 语音答案页在屏）→ 断言层栈快照**不新增 KIDPAL_LAYER_SERVER_PAGE**（takeover 路径的 begin_page 侧效应缺席）、`s_local_page` 不变；P2 在屏态断言网格重绘发生（缓存更新 + 本页刷新）；P1 态断言下次 BOOT 翻到 P2 读到新缓存（③ BOOT 翻页读路径回归）。
4. **语音接管回归守卫**：薄载荷展开产物（restore 缺省）注入 → 断言走 begin_page、SERVER_PAGE 入栈（现行为零偏移）。
5. **restore 降级静默（板端面）**：畸形 restore 载荷入板后 → 缓存零变化 + 日志 warn，**不触发 SetChatMessage**（区别于主动展示路径的降级弹提示；front 校验面 V27 降级独立生效，两层各有齿）。
6. **alert 轮询起轮不回归**：建连推送扩为两条后，`_register_alert_legs` / `_ensure_alert_loop` 时序与注册行为零偏移（回归面①）。
7. **每日/录入触发附带**：piggyback 路径断言跨天 due 时同时推 upcoming + grid restore；**不新增第二套进程内日账**（F3：`_daily_push_local_date` 形态勿复制——grid 附带沿用同一 due 判定点，front 重启导致的同日重推由载荷幂等性〔缓存覆盖写〕天然无害）。

---

## 6. 给 challenger 的预告关注点（自列）

> （对抗 review 已收口：以下为 review 前预告记录，R1-R11 修复与 F-C5 裁定已回灌正文；本节仅 §6.1/§6.2 按裁定更新措辞，其余留作预告存档、不再更新。）

1. **双行为语义的契约复杂度与纪律强度**：schedule view 从此有两个设备行为分支，「薄载荷展开恒不置 `data.restore`」目前只有 prose + 一条验收齿——challenger 可攻「生产纪律无机械闸，front 未来新调用方误置 restore 无门拦」。**收口记录（F-C5 human 裁定 2026-09-23，撤 review 期多层拦截的高估表述、换诚实记录）**：防线如实记 = system prompt 薄载荷约束 + ADR-004 单源（根因缓解）+ 验收齿 2（回归守卫）；LLM 绕过直产带 `data.restore` 完整网格理论上可过 shape floor 静默填缓存，属既存「LLM 直产完整网格直达（危害更高：接管前台）」的同族低位变体——**不单独为 restore 键设运行时闸**（与 §1.3② 既有 producer 纪律的执行地位对称；单独设闸对更重的既存洞不设防），残余风险显式记入 ADR-005；未来整体收紧 LLM 直产 display 块面（provenance 旁路 + floor 收紧）时，restore 拒键随该收口一并做。
2. **部署时序回归窗口（R3 已按仓内代码定案收口）**：旧固件收到 restore 载荷 = **确定性接管前台顶到 P2**——旧固件无 restore 分支，`schedule_page.c:607-613` 对 data 内未知键取忽略语义（仅畸形值返回 -1）、`mcp_server.cc:620-644` schedule 路由无条件 begin_page 接管；即每次建连被顶到课表页的回归级事故。处置已记 gate evidence C1 类 closeout condition：builder 实测复核前 ECS 推送开关保持关，固件先行、烧板验证后开闸（真机烧录 human 在场，常例）。**仍留观察的轴**（review 未裁、进实现包 closeout 定）：C1 若长期不闭合（设备不烧新固件），ECS 推送开关是永续配置项还是闭合后拆除的一次性闸——开关生命周期语义。
3. **两载荷非原子 / 失败面**：upcoming 推送成功、grid 推送失败（或反之）的一过性不一致窗口；重连风暴下重复双推的成本。答辩方向：两推各自独立降级不阻断建连（现状语义），grid 失败的下一次建连/跨天自然补齐，无重试语义引入必要。
4. **触发②③ 附带的必要性**：§6.4 已知「设备闲置约 3 分钟休眠断 WS」——0:00 设备多离线，跨天重推实由次日重连兜底，challenger 可攻「触发②附带冗余、应只做触发①最小面」。答辩方向：在线挂 P2 过零点的设备需刷新 ※ 过期项与星标位（bit0/bit1 云端算），缺触发②则 stale grid 呈现；且「三触发同源同附带」是不变式，比「两附带一不附带」好守。此点真实存在取舍空间，供 adjudicator 裁。
5. **「疏漏补齐」定性的历史证据强度**：本草案断言决策 26 漏 grid 属疏漏非取舍，主要证据是时序（决策 26 早于周网格设计 12 天）+ M3-K2 实现了 BOOT 翻页读缓存（若有意让 P2 只能语音进入，翻页读缓存路径无意义）。challenger 可要求更强考据或主张「当时的隐含设计 = 课表页语音入口唯一」——若成立，Shape C 需重估（升 human）。
6. **NFR-OFF-1「不缓存课表」字面张力**：RAM 网格缓存在役（M3-K2 起）与「设备不缓存事项/课表」的字面冲突——§3.5 澄清句是否足够，还是 requirements NFR-OFF-1 行也须同步注记（本草案只改 arch 侧，requirements 侧 NFR 行零改动，可能被攻半拉子）。

---

## 7. 部署与落地注意（供后续 handoff / 实现包取材，非本草案义务面）

- **升级顺序（gate evidence C1 类 closeout condition，非可先假设的前提）**：固件先行（新固件对不置 restore 的旧 server 载荷 = 现行为零偏移）；旧固件对 restore 载荷 = **确定性接管前台顶到 P2**（仓内代码定案：`schedule_page.c:607-613` 未知键忽略 + `mcp_server.cc:620-644` 无条件接管），**builder 实测复核确认前 ECS 推送开关保持默认关**，烧板验证 restore 链路后人工开闸。真机烧录 human 在场（常例）。
- **实现路由**：confirmed 后产 `dev-loop-handoff.json` 进 Dev Loop OS（`/triage-change` 分档——双面小改，预计档②单包）；本草案 §5 验收锚即 acceptance-author 输入。
- **落地通道**：canonical landing 恒 = 主线程 packet-bound；契约改动待 human 契约门签字后与 spec 同 PR 原子落地。
