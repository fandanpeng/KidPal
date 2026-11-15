# input-brief · 261104-k4-reward-goal-config

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-11-14T16:45:07.288Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261104-k4-reward-goal-config/taskpackage.json` · subsystem: points 目标配置 + QQ skill 扩展 + 1bpp 场景资产 + firmware 进度线刻度 · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.3⑦` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§1.4` | 1.4 控制类指令（OpenClaw 判意图或 front 检出后下发） | 231 | ok |
| `docs/contracts/interface-contract.md` | `§1.5` | 1.5 网关的职责（薄，零自研业务逻辑红线不变） | 250 | ok |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/261103-star-points-story-cover/review.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/261103-star-points-story-cover/task_plan.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261104-k4-reward-goal-config/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/decisions/ADR.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/kidpal-ui-design-v9.html` | — | 无锚点（整文件） | — | bare |
| `docs/ui/UIUX-DESIGN-BRIEF.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/points-home.png` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/points-home-empty.png` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/points-home-negative.png` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | `§3.1-D-09` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | `§3.2-OQ-2` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | `§3.2-OQ-3` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/task_plan.md` | `K4` | （未定位到标题节） | — | not-found |
| `docs/architecture.md` | `§5.5.2` | 5.5.2 资产管线（预栅格化 1bpp） | 315 | ok |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F6.5 · 源 = `docs/requirements.md` 节首行 :745 · 本包声明 AC 粒度回引：`S-F6.5-AC1`、`S-F6.5-AC2`、`S-F6.5-AC3`、`S-F6.5-AC4`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F6.5 · 奖励场景与激活目标配置（本批新增）

> 作为家长，我希望从常用奖励场景里挑一个设为当前目标、也能自定义新奖励，以便目标跟着孩子的情况走。

**验收标准**
- AC1（预置场景库，human 2026-11-03 拍板第一版）：Given 出厂状态，When 看场景库，Then 预置四项 = 巧克力 20 分 / 看电影 30 分 / **游乐场 40 分（出厂默认激活）** / 礼物 50 分，每场景 = 奖励名 + 目标分值 + 场景美术；出厂默认激活保证单页进行中恒有目标、不存在「暂无目标」页面态。
- AC2（自定义奖励，OQ-3）：Given 家长经 QQ 新增自定义奖励（奖励名 + 目标分值），When 分值为 5–200 的正整数，Then 入库、可激活；自定义场景在进度线上用**通用标记图**（不做逐个定制美术）；When 分值出界，Then 按 S-F6.4 AC4 一次性报错说明允许范围。
- AC3（切换，唯一激活）：Given 家长经 QQ 设置/切换激活目标，When 生效，Then 同一时刻只有一个激活目标，设备端进度线场景标与刻度随之更新——分值刻度随激活目标伸缩（激活 60 分目标时刻度画到 60）；**设备端不设目标**（目标设置只走 QQ 家长通道）。
- AC4（删除，OQ-2）：Given 家长要求删除某场景，When 它是预置场景，Then 不可删除；When 它是自定义奖励且非当前激活目标，Then 删除生效；When 它正在激活中，Then **禁止直接删除**、回复须先切换到其他目标再删。

**线框**：无（设置走 QQ 家长通道，无板端设置页；场景标与刻度显示归 WF-F6-01 进度线）。
**补充规则**：预置清单与分值 = human 2026-11-03 拍板第一版；自定义分值区间 5–200 正整数（OQ-3 裁定）；预置场景美术出厂自带、自定义用通用标记图（资产登记归架构面）。埋点 = 无。
**锚**：UI = 无（板端无设置 UI；场景标显示归 WF-F6-01） ｜ 支撑 = arch §5.1（服务端组件，云）
**可行性**：🟡（云端目标配置与既有资产管线可承载，预置/自定义/唯一激活规则须确定性校验及切目标联动验收）

---
```

### S-F6.1 · 源 = `docs/requirements.md` 节首行 :679 · 本包声明 AC 粒度回引：`S-F6.1-AC2`、`S-F6.1-AC3`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F6.1 · 看积分进度与最近记录（本批新增）

> 作为孩子，我希望翻开积分本一眼看到我的余额、激活目标在哪、最近几笔得分，以便知道离奖励还有多远。

**验收标准**
- AC1：Given 翻到 P3 星星积分本，When 看屏，Then 顶部 = 页名 + 当前积分（金币内嵌五角星符号 + 数字），下部 = 最近三条积分记录（每条 = 分值 + 原因 + 日期）+ 「详细」入口（KEY 双击进详细账本，屏上提示承载双击口令）；页点 4 点、第 3 实心。本页**单页进行中**——始终是同一个账本页，不设「已达成待兑现」「暂无目标自由积累」独立状态页（human 裁收敛）。
- AC2：Given 激活目标场景（出厂默认 = 游乐场 40 分），When 看中部进度线，Then 起点、分值刻度与激活目标的场景标（画在激活目标的分值位）一并显示，小伴标志放在线上代表当前余额位置，已走过的部分为实线、未走到为虚线；超激活目标时实线与小伴越过场景标继续延伸、页面结构不变；不放「再得 N 分就能去游乐场」一类解释文案。分值刻度随激活目标伸缩（见 S-F6.5 AC3）。
- AC3：Given 兑换扣分发生，When 回到本页，Then 按新余额重画进度线（如 63 − 40 = 23，小伴回到 23 的位置）——「够不够兑换」只是小伴与场景标在线上的位置关系，不是页面状态。
- AC4（空态两态，S-F1.1/S-F2.1 先例同型）：Given 账本数据已送达且确无分录（新账本），When 看本页，Then 余额如实显示 0、最近记录区显示空态话术；Given 账本数据未达（未收到数据），When 看本页，Then 余额与记录区**不显示数字**（未知显数会误导）、屏显「积分数据还在路上，等一等再来看」类话术——**纯等待口径、不暗示故障或打不开**（human 2026-11-03 终拍裁定），数据到达后本页刷新为正常显示——两态判据落在「有无账本数据」而非「断网与否」；话术逐字归文案单源/设计稿、与无内容空态话术保持可区分。
- AC5（负余额显示，OQ-1）：Given 删除重算后余额为负（仅删除重算路径可为负，见 S-F6.2 AC4），When 看本页，Then 余额数字如实显示负值、不藏数；负余额下进度线的画法（小伴位置、实线虚线走向）留设计稿/线框迭代，本 AC 只钉「负数可见」。
- AC6：Given 本页任意时刻，When 孩子看屏，Then 常驻语音入口提示照常在位（S-F2.1 AC3 先例同型，如「说『加5分』就能记账」类，措辞归文案单源/设计稿）；键帽「问一问 / 下一页」按 §2.3 本页行。Given 孩子说「打开积分本」，When 设备收到，Then 切到本页。

**线框**：WF-F6-01（实存；视觉权威 = showme 线框 + 本批 uiux-author 在途产物）。
**补充规则**：数据口径 = 余额是全部账本分录的计算结果，由云端算好、设备只显示权威值（账本云端为唯一真相源，回引 NFR-OFF-1）；积分页数据在设备启动后经恢复性推送补齐（S-F1.2/S-F2.1 冷启动恢复先例同型：只填数据不抢前台、本页在屏时原地刷新——机制归架构面，本 story 钉「重启后翻到积分页能看到权威余额与记录」）。金币符号与故事星星的视觉区分原则见 §4。埋点 = 无。
**锚**：UI = `points-home`（新增，随批 261103-star-points-story-cover 在途） ｜ 支撑 = arch §5.5.4（UI 演出，板）、§5.1（front 组件，云）
**可行性**：🟡（积分主页可复用 display、恢复推送与 1bpp 资产管线，新账本 view 及空态/负余额形状须随契约和 K0/K2 实证）
```

### S-F6.3 · 源 = `docs/requirements.md` 节首行 :712 · 本包声明 AC 粒度回引：`S-F6.3-AC3`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F6.3 · 设备语音记账与兑换（本批新增）

> 作为孩子，我希望直接跟小伴说「我今天扫地了，加5分」就能记账、攒够了说一声就能兑换，以便不等家长操作也能自己攒积分兑奖励。

**验收标准**
- AC1（记账句式门槛，human 2026-11-03 拍板）：Given 孩子对设备说话，When 语句是**明确完整的记账句式**——事由 + 动作词（加/扣）+ 分值（如「我今天扫地了，加5分」「我们今天去游乐场了，扣40分」），Then 触发记账；When 只提事件、不带分值动作（如随口说「我们去游乐场了」），Then **不触发**记账、走普通对话、不落任何账。
- AC2（加分）：Given 完整加分句，When 说出，Then 立即记账（加分任意金额 + 事由，自报即到账——设备不区分说话的是家长还是孩子），并自动跳转积分本页展示新余额。
- AC3（兑换资格与金额校验，human 拍板「只认目标分值」）：Given 语音兑换句，When 扣的分值 = 当前激活目标分值且余额 ≥ 该分值，Then 兑换成立（按 AC4 落账）；When 金额不匹配（如目标 40 说「扣20分」、目标已换 50 说「扣40分」），Then 温和拒答并说明当前目标要扣多少、不落账；When 余额不足激活目标分值，Then 不能兑换、温和说明还差多少。
- AC4（真实兑现后才扣）：Given 已具备兑换资格（余额 ≥ 激活目标分值 = 资格、**不自动扣分**），When 奖励实际兑现并经一次明确的兑换确认，Then 产生一条负向分录（分录形状同 S-F6.2 AC2 单行口径：兑换名目住原因位、分值 + 兑换前后余额 + 日期）；**绝不提前扣**——定好了明天去游乐场余额也不动，临时没去成什么都不用处理（从未扣分、余额保持原样，改天真去了再扣）。
- AC5（短窗去重，human 拍板）：Given 短时间内（起步 10 分钟、真机调优定数——验收经注入时间覆盖）分值相同且事由相近的重复记账句，When 再次说出，Then 不记第二条、友好回应「这条刚才记过啦」并跳积分本页；兑换侧「同一次兑换不重复扣分」同此。
- AC6（断网）：Given 断网，When 语音记账/兑换，Then 失败给可见安抚（「现在联系不上账本，等会儿再试」类，措辞归文案单源/设计稿），**不本地暂存补记**（回引 NFR-OFF-1）。

**线框**：无专属页——语音记账/兑换 = 会话态复用共享底座（WF-B-01/02/03）；记账成功跳转落点 = WF-F6-01。
**补充规则**：在积分本页（主页/详细账本）发起的会话带**积分语境**（浏览账本时问「我为什么得了3分」最顺手）——语境信号的技术载体与取值归架构/契约面，本 story 只钉意图。记账句式门槛以 human 2026-11-03 拍板为准（讨论前文「我们去完游乐场啦」类不完整句按门槛不落账，正负两类都进验收）。惩罚性扣分、语音删除、加分护栏（单笔分值上限）第一版不做（human 批示）——删除只走设备按键（S-F6.2）与 QQ（S-F6.4）。埋点 = 无。
**锚**：UI = 共享底座（语音轮三态） ｜ 支撑 = arch §5.1（front 组件，云）、§5.2（运行场景，云）
**可行性**：🟡（语音链路已有，薄意图→确定性账本→权威推送路径可行，句式门槛、短窗去重与真实兑现确认须 K0/真机验收）
```

### S-F6.4 · 源 = `docs/requirements.md` 节首行 :729 · 本包声明 AC 粒度回引：`S-F6.4-AC4`、`S-F6.4-AC5`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F6.4 · QQ 家长记账与更正（本批新增）

> 作为家长，我希望在 QQ 里就能给孩子加扣积分、查账、删除误记和兑换，以便随时掌握和更正孩子的账本。

**验收标准**
- AC1（记账同构）：Given 家长在 QQ 说（如「给小伴加 3 分，今天老师表扬了」），When 消息收到，Then 解析后记账——账面规则与设备语音完全同构（human 拍板「与语音同规则」）：加任意分 + 事由；兑换同 S-F6.3 规则（只认激活目标分值、不符回说明、真实兑现后才扣、同一次兑换不重复扣）。
- AC2（查账）：Given 家长询问当前余额、激活目标或最近记录，When 发问，Then 回答账本真实状态。
- AC3（删除）：Given 家长要求删除某条记录，When 定位明确（如「删掉刚才那条加 3 分」类稳定指认），Then 该分录删除、余额重算；When 定位含糊或指认不上，Then 用普通文字回复说明可用的定位方式、不进入追问卡死。
- AC4（一次性可判，硬约束）：Given 家长消息含糊或格式错误，When 无法唯一理解，Then 用普通文字回复说明正确说法——每条消息独立可执行或独立报错，**QQ 命令不依赖多轮澄清问答**（措辞面归实现）。
- AC5（推送不抢前台）：Given QQ 通道到账（加分/兑换/切目标/删除），When 设备正在其他页，Then 只更新数据、不抢前台；When 积分本页在屏，Then 原地刷新余额与进度线。

**线框**：无（QQ 侧无设备 UI；设备侧显示落点 = WF-F6-01 原地刷新）。
**补充规则**：通道 = 家长主通道 QQ（S-F1.5 先例同型）；惩罚性扣分的门对所有通道关闭；「一次性可判 + 普通文字错误回复」为硬设计约束（QQ 侧歧义命令宁可报错重来、不挂起等补充——多轮澄清不可依赖），具体回复格式归实现。埋点 = 无。
**锚**：UI = 无（QQ 侧无板端 UI） ｜ 支撑 = arch §5.1（服务端组件·家长通道，云）
**可行性**：🟡（QQ 与 front 在线推送先例可复用，须新 skill 共用单一 writer 并实证一次性报错和不抢前台）
```


## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

| 类别 | 路径 |
|---|---|
| designRefs | `docs/ui/V9/points-home.png` |
| designRefs | `docs/ui/V9/points-home-empty.png` |
| designRefs | `docs/ui/V9/points-home-negative.png` |
| sharedUiRefs | `docs/ui/kidpal-ui-design-v9.html` |
| sharedUiRefs | `docs/ui/UIUX-DESIGN-BRIEF.md` |

页级 PNG（400×300）= **布局坐标定位权威**：layout 常量按图定，文字线框仅口径补充。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/front/front_app.py` | `_push_display_payload` | :597-606 | 655, 795, 874, 878, 952, 1070, 1075, 1101, 1144 |
| `firmware/main/display/view_router.c` | `kidpal_display_route` | :33-74 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

（无同 subsystem 的已 merged 先例包）

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |
|---|---|---|---|---|
| `package:261104-k1-points-ledger-core` | 目标配置存储到 QQ 管理与设备刻度联动 | `261104-k1-points-ledger-core` | `261104-k4-reward-goal-config` | K1 先建立合法存储和校验，K4 完成 QQ 自定义/切换/删除、美术与切目标后兑换/刻度联动。 |
| `package:261104-k3-qq-points-channel` | QQ 目标配置命令的后续扩展 | `261104-k4-reward-goal-config` | `261104-k4-reward-goal-config` | K3 建立 skill 与普通错误回复纪律，K4 在同一 skill 上新增自定义/切换/删除目标，不另建通道。 |
| `workstream:261103-star-points-story-cover` | K3 QQ skill 到 K4 目标配置扩展 | `261104-k3-qq-points-channel` | `261104-k4-reward-goal-config` | 同一 skill 增加自定义/切换/删除目标，不另建 QQ 通道或 ask_user 流程。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

## ⑥ 非阻断 advisory 明细（终端只打汇总；此处保留定位）

- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§1.3⑦`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§1.3⑦（起包期核对锚点或目标文档是否已变更）
- **info · whole-document** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
- **info · whole-document** · ref=`delivery/workstreams/261103-star-points-story-cover/review.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261103-star-points-story-cover/review.md
- **info · whole-document** · ref=`delivery/workstreams/261103-star-points-story-cover/task_plan.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261103-star-points-story-cover/task_plan.md
- **info · whole-document** · ref=`delivery/taskpackages/261104-k4-reward-goal-config/plan.md`：Markdown 整文件引用（不自动猜章节）：delivery/taskpackages/261104-k4-reward-goal-config/plan.md
- **info · whole-document** · ref=`docs/requirements.md`：Markdown 整文件引用（不自动猜章节）：docs/requirements.md
- **info · whole-document** · ref=`docs/architecture.md`：Markdown 整文件引用（不自动猜章节）：docs/architecture.md
- **info · whole-document** · ref=`docs/contracts/interface-contract.md`：Markdown 整文件引用（不自动猜章节）：docs/contracts/interface-contract.md
- **info · whole-document** · ref=`docs/decisions/ADR.md`：Markdown 整文件引用（不自动猜章节）：docs/decisions/ADR.md
- **warning · bare-non-markdown** · ref=`docs/ui/kidpal-ui-design-v9.html`：ref 无锚点，清单退化为整文件引用：docs/ui/kidpal-ui-design-v9.html（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **info · whole-document** · ref=`docs/ui/UIUX-DESIGN-BRIEF.md`：Markdown 整文件引用（不自动猜章节）：docs/ui/UIUX-DESIGN-BRIEF.md
- **warning · bare-non-markdown** · ref=`docs/ui/V9/points-home.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/points-home.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`docs/ui/V9/points-home-empty.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/points-home-empty.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`docs/ui/V9/points-home-negative.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/points-home-negative.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-09`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-09（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.2-OQ-2`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.2-OQ-2（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.2-OQ-3`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.2-OQ-3（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/task_plan.md#K4`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/task_plan.md#K4（起包期核对锚点或目标文档是否已变更）

