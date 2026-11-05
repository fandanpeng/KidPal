# input-brief · 261104-k0a-four-page-skeleton

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-11-04T13:21:11.471Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261104-k0a-four-page-skeleton/taskpackage.json` · subsystem: firmware 主页面枚举、BOOT 循环、keybar、故事页按键与会话来源映射 · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§四-kidpal_session_ctx` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/261103-star-points-story-cover/task_plan.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261104-k0a-four-page-skeleton/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/decisions/ADR.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261028-p3-reader-key-falls-to-session-toggle.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261022-p3-record-session-state-churn.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/kidpal-ui-design-v9.html` | — | 无锚点（整文件） | — | bare |
| `docs/ui/UIUX-DESIGN-BRIEF.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/canvas-p1.png` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/canvas-p2.png` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/canvas-p3.png` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/p3-reader.png` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V9/points-home-nodata.png` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `§2.3` | 2.3 按键手势（3 键，无触摸；本节为权威键位定义） | 131 | ok |
| `docs/requirements.md` | `§2.4` | 2.4 页面结构（4 页循环，本批 3→4） | 176 | ok |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | `§3.1-D-01` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md` | `§3.1-D-08` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261103-star-points-story-cover/task_plan.md` | `K0a` | （未定位到标题节） | — | not-found |
| `docs/decisions/ADR.md` | `ADR-018` | ADR-018 · 积分账本由确定性单 writer 承载；完整账本 keyset 分页；story_list 复用为 P4 共享数据帧 | 153 | ok |
| `firmware/main/display/story_page.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/kidpal_render_node.h` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/kidpal_render_node.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/keybar.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/key_gesture_parser.h` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F6.1 · 源 = `docs/requirements.md` 节首行 :679 · 本包声明 AC 粒度回引：`S-F6.1-AC4`

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

### S-F2.1 · 源 = `docs/requirements.md` 节首行 :456 · 本包声明 AC 粒度回引：`S-F2.1-AC1`、`S-F2.1-AC2`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.1 · 看「星星记录本」列表

> 作为孩子，我希望看到我记下来的所有内容排成列表、每篇都是一颗星星，以便有收集的成就感。

**验收标准**
- AC1：Given 进星星记录本且列表数据已送达（正常接收与冷启动恢复推送均算——冷启动恢复口径见补充规则），When 看列表，Then 星星计数（×N，右上角），每条 = 星星图标 + 标题 + 日期（标题超宽时尾部省略号截断、不挤占日期列——日期完整可见）；**光标默认选中第一条**，选中行**带边框**（不反白——反白真机效果差，2026-09-14 human 裁）；**列表超一屏时底部显示「第 x 屏 · 共 y 屏」**（与阅读态分屏同心智，v1.28）。
- AC2：Given 列表中，When BOOT 短按，Then 选中项下移——光标到当前屏底再按翻下一屏并落该屏首项，列表到底 wrap 回首项（永不离页）；When BOOT 双击，Then 直接翻下一屏、光标落该屏首项（长列表快速跳屏；最后一屏再双击 wrap 回首屏）；When KEY 双击，Then 进入阅读态看选中那一篇的正文（见 S-F2.3）——本机录的、重启前录的、云端恢复回来的老故事**一视同仁**：正文取自设备本地留存的那一篇，本地尚无该篇时设备即时从云端取回后再进阅读态（进入本列表、或本页翻屏时已先行为当屏各篇取回本地缺失的正文，通常双击即开）。
- AC3：Given 星星记录本任意时刻，When 孩子看屏，Then 常驻语音入口提示「说『我要讲故事』开录新的」。
- AC4：Given 列表选中某篇，When BOOT 长按，Then 弹出删除确认弹窗（**整屏覆盖层，WF-F2-05**，大字「删掉这篇？」+ 待删条目，页点消失同覆盖层口径；v1.48 human 裁全屏形态）：KEY 短按 = 确认删除（条目移除、星星计数 −1、云端同步）；BOOT 任一手势 = 取消回列表；10 秒无操作静默取消回列表；其余手势不响应。
- AC5（无内容空态，v1.63）：Given 列表数据已送达且现存篇数为零（新设备或全部删除后），When 孩子看列表，Then 星星计数显示 **×0**（诚实计数、页身份保持），列表体区显示「本子空空的，讲个故事点亮第一颗星吧」；AC3 常驻语音入口提示**照常保留**——空态话术与入口提示两句话术并存、不互相顶替。
- AC6（数据未达空态，v1.63）：Given 未收到列表数据、现存篇数未知（典型 = 设备重启后恢复推送未到或失败），When 孩子看列表，Then **不显示星星计数**（篇数未知，×0 会误导——明明有故事却显 0 颗），屏显「本子还没翻开，等等再来看看吧」；联网恢复、恢复推送到达后本页刷新为正常列表（按 AC1）。两态判据落在「有无列表数据」而非「断网与否」，与无内容空态（AC5）显式分叉（形态对齐 S-F1.1 AC5/AC6/AC7 先例）。
- AC7（离线 / 降级，v1.64 本批新增）：Given 列表某篇的正文既未在设备本地留存、当刻又取不回来（典型 = 断网），When 孩子双击该篇，Then 设备给出孩子看得懂的**可见提示**（**整屏覆盖层，WF-F2-06**；示例话术「这篇暂时打不开，等会儿再试试吧」——最终措辞归文案单源 / 设计稿、守 §2.2 错误兜底的儿童化口径，本 AC 只钉「**必须有可见反馈**」），不静默、不黑屏、也不显示出别篇的内容——**正文须与所选篇目一致**；已本地留存的篇目断网时照常打开（本批新增的离线可看边界，见补充规则）。

**线框**：WF-F2-01（实存）+ WF-F2-05（删除确认弹窗，v1.48）+ WF-F2-06（正文取不回提示，v1.64）。
**补充规则**：出路口径（v1.29 human 裁；本批随封皮下沉平移）= 列表态**无按键翻页出口**（BOOT 本页不翻页——多屏列表里「到底翻页」与「翻下一屏」直觉冲突），离开 = KEY 双击进阅读 / KEY 长按回封皮（v1.29「长按一层即到家」随下沉改为逐层返回：列表 → 封皮 → 首页）/ 语音切页（「打开课表页」等）——页循环在 P4 故事模块子页断开，对话为主产品可接受。冷启动恢复（v1.63，复刻 S-F1.2 周网格先例口径）= 板端列表数据属内存缓存、重启即失，WS 建连时云端附带重推星星列表（恢复性推送，只填缓存不抢前台，P3 在屏时刷新本页；触发时机权威承载见 arch §5.2 场景④）；数据未达（恢复推送未到/失败）时翻到本页 = 数据未达空态（AC6），恢复推送到达后自动补齐。空态口径（v1.63）= 两态显式分叉（形态对齐 S-F1.1 AC5/AC6/AC7 先例）：**无内容空态**（数据已送达、确无篇目——新设备或全删后）计数显 ×0 + 话术「本子空空的，讲个故事点亮第一颗星吧」（AC5），AC3 常驻提示照常并存；**数据未达空态**（未收到列表数据、篇数未知）不显计数 + 话术「本子还没翻开，等等再来看看吧」（AC6）——判据落在「有无列表数据」而非「断网与否」。正文缓存边界（v1.64）= 设备为 story 正文在本地留一份副本、**只留正文**（提醒 / 课表 / 日志等其余业务数据照旧不落本地——回引 NFR-OFF-1「断网无提醒」，语义不变）；**云端的故事归档是唯一真相源，本地副本只是它的只读镜像**——设备不从本地副本改任何东西，录音归档 / 删除等一切写入照旧走云端；双击进阅读时**只认云端列表里存在的篇目**（本地副本与云端列表对不上的一律不采信），命中判据 = 该篇正文已在本地留存。本地留存不可用时设备仍可在线取回并当场显示（只不留存）；此时若同时离线、该篇又未留存，按 AC7 走可见提示。埋点 = 无。
**锚**：UI = `canvas-p3` ｜ 支撑 = arch §5.5.4（UI 演出·星星记录本渲染，板）、§5.1（服务端组件·故事数据链路，云）
**可行性**：✅
```

### S-F2.2 · 源 = `docs/requirements.md` 节首行 :474 · 本包声明 AC 粒度回引：`S-F2.2-AC1`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.2 · 对话式引导记录（双入口，v1.65 改名，原「开录（双入口）」）

> 作为孩子，我希望在星星记录本按一下 KEY、或者说「我要讲故事」「帮我记下来」就开始记，讲不下去的时候小伴轻轻提我一句，讲完了帮我把刚才说的话整理成一篇收进本子，以便把我想记的东西完完整整地留住。

**验收标准**
- AC1：Given 在星星记录本按 KEY（或任意页说「我要讲故事」/「帮我记下来」），When 会话建立，Then 屏显记录语境引导「需要我帮你记点什么？」+ 聆听标识，孩子开口即进记录会话（REC 闪点 + 大时长 + 声波跳动 + 话语回显）；孩子讲着时 **AI 默认闭嘴**——切句只用于话语回显，不逐句插话。
- AC2：Given 记录会话中孩子停顿超卡壳阈值（起步约 8 秒、**待定**——回引 NFR-PERF-3，真机调优后立项拍数）或明示卡住（如「卡住了」「想不起来」），When 触发，Then AI 基于已讲内容轻声引导一句（讲故事推情节「那后来呢？」，日记/碎碎念轻提「还有呢」——随内容自适应），只提问推动、不提供情节选项、不代编情节；引导播报不跳页（停留记录会话屏、气泡换引导文字、回显区常驻）；引导播报中 KEY 短按 = 打断引导播报、继续聆听（长按同；同 §2.1 打断停播语义），BOOT 无操作（§2.3 录音态子态按键口径）；**引导语、孩子中途的提问与 AI 的回答都不进故事素材**。
- AC3：Given 保存成功那一瞬，When 触发，Then 成就盖章演出（见 S-F2.4）。
- AC4：Given 记录会话中，When 孩子说「我说完了」/「讲完啦」/「说完了」（「说完了」v1.66 并入——真机 R2 实证孩子自然语「好，说完了」不收尾，收尾词检测为子串扫描、并入一词即覆盖前缀变体；261001 q3 关闭留痕见补充规则）或再按 KEY（兜底收尾，与全局会话结束手势一致；KEY 长按**同短按**——防长按误触毁稿，v1.47），Then 设备把全程孩子说的内容**忠实整理**成一篇（只用孩子原话与事实，去口水词、断句、分段、起标题，不换表达、不补情节），整理期间以思考态（或等效过渡演出，下行触发机制归架构裁定）盖住延迟——整理中 KEY/BOOT 均不响应（非交互过渡态，§2.3 录音态子态按键口径），完成后新的一篇出现在列表；When 按 BOOT（短按/长按/双击同义）或说「讲错啦」，Then 取消、已录内容丢弃退回列表——**不设重录恢复**（v1.49 human 裁先不加，见 §8）。
- AC5（自动收尾，v1.65 新增）：Given 连续 2 轮引导孩子仍无回应，When 第 2 轮计时到仍无回应，Then 视为讲完、自动收尾走忠实整理 → 归档 → 盖章（取代原「120 秒无语音自动收尾」，§7 开放问题 7 撤注；录音期间提醒排队延后规则不变，见 §2.2）。引导轮**含 AI 收声只听等被抑制而不播报的轮次**——收声不停摆，防孩子收声后离开、会话挂死不归档。

**线框**：WF-F2-02（对话式增补注记见线框框后注记，v1.65）；记录语境引导态复用 WF-B-01 布局、气泡换「需要我帮你记点什么？」。
**补充规则**：录音不自动开始——BOOT 翻页只到 P4 封皮、不进列表，路过不误触；星星记录本列表同时承担回看。双入口口径 = 在星星记录本列表按 KEY（**记录语境**，无需触发词）+ 任意页触发词（「我要讲故事」/「帮我记下来」，判意图后开录），两入口进**同一条记录语境对话态**，引导策略随内容自适应；孩子说「别说话听我讲」时 AI 收声只听（对话语境行为，不加独立状态）；**收声不停摆**——收声只抑制引导播报，卡壳计时照常走、被抑制的引导轮照常计入轮次，第 2 轮计时到仍无回应即自动收尾（AC5；依据 = 设计稿决策 6「孩子真离开则归档无害」的既定意图，非新语义，留痕 spine openQ q6）。**翻案留痕（v1.65）**：本 story 废除 2026-09-14「一刀切纯记录、提问也被记录」裁定，并连带改写架构决策 25 第二条聆听语义（原「manual 聆听 + 网关累积」→「累积 + 卡壳轻引导 + 收尾整理」）——当初定的理由 = 聆听语义简单可靠、录音不被 AI 插话污染；现在变 = 孩子对一台沉默设备讲完完整故事实际很难（卡壳即放弃），叙事脚手架对 8–12 岁目标是真实价值，且对话态下问答天然被滤出不进素材、「提问被记录」的代价随之消解；设计权威 = [261019-p3-record-dialogue-guide.design.md](../delivery/intake/261019-p3-record-dialogue-guide.design.md)（human 2026-10-19 逐段批准）。技术信号 = 设备须把「会话发起自星星记录本列表」上下文告知云端、网关即切**记录语境对话态**（arch §5.1 故事归档组件；信号载体 = 契约 §四 `kidpal_session_ctx`，source_page=story）。误判挽回口径 = 被误判为闲聊的真实故事，孩子可经「刚才那段是故事」挽回（**闲聊语境、其他页保留**——记录语境对话态内不再需要）。**收尾词「说完了」并入（v1.66，human 2026-10-20 裁：「收尾词表并入『说完了』，与『我说完了』『讲完啦』并存」）**：真机轮 R2 实证孩子自然语「好，说完了」（ASR 识别「好说完了。」）不触发收尾；收尾词检测 = 子串扫描（实现侧 `is_finish_phrase`，server/gateway/story.py 既有机制，arch §5.1 故事归档组件行在案），并入「说完了」一词天然覆盖「好，说完了」「我说完了」等前缀变体；261001 q3 悬案（「『说完了』是否并入收尾词表」，spine `261001-m4-k3-record-session-live-repair` q3，deferred）就此裁定关闭。埋点 = 无。
**锚**：UI = `p3-recording` ｜ 支撑 = arch §5.1（故事归档组件）、§5.2 场景④（录音与归档链路，板+云）
**可行性**：🟡（累积与归档链路已实测；卡壳计时引导与收尾忠实整理步为新增机制、未真机实证——设计可行，引导手感与整理忠实性随实现包验收，验收种子见设计稿）
```

### S-F2.3 · 源 = `docs/requirements.md` 节首行 :490 · 本包声明 AC 粒度回引：`S-F2.3-AC1`、`S-F2.3-AC2`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.3 · 回看记录（原「回看记录与朗读」，v1.52 砍朗读）

> 作为孩子，我希望选一篇记录能看到它的文字，以便重温我记下来的东西。

**验收标准**
- AC1：Given 列表选中某篇，When KEY 双击，Then 进阅读态：标题 + 正文分页，底部屏指示「第 x 屏 · 共 y 屏」（屏数随篇长与排版而定，不钉常数）。
- AC2：Given 阅读中，When BOOT 短按（双击同短按），Then 翻下一屏——末屏再翻 wrap 回首屏、**永不离页**（离开 = KEY 长按退出阅读回列表（v1.36 返回上一层；再长按回封皮、封皮再长按回首页——离页路径经封皮）/ 语音切页）；When 按 KEY 短按，Then 不响应（朗读已砍，v1.52）。

**线框**：WF-F2-04（阅读态，纯阅读）。
**补充规则**：数据口径（2026-10-28 批改写，`261028-font-typography-scaleup`）= 阅读排版：**标题 20px、水平居中、单行**——超宽时尾部省略号截断（400px 屏宽下约 19 个汉字可见）；**正文 16px**（可读性口径回引 NFR-READ-1）。正文分屏以自然行为单位，每屏自然行数随正文排版在实现侧定、不钉死于本文件（字号上调后每屏行数相应减少，防长自然行折行超屏）；翻页手感（屏内疏密与翻页成本的取舍）真机轮终判，本文件不锁死。阅读态口径 = **一义到底（纯阅读），无会话/问答入口**（2026-09-14 human 裁；v1.52 砍朗读，KEY 短按不响应）——问生字、问故事内容请先退出（KEY 长按逐层返回）再问，故事语境会话（按 KEY 带故事背景开会话）同案不取，本期不考虑故事内问答。已砍联动 = 「生字反白高亮」「生字跳查字页（原 AC3）」「朗读出音（v1.52）」（见已砍清单）。埋点 = 无。
**锚**：UI = `p3-reader` ｜ 支撑 = arch §5.5.4（UI 演出·阅读态渲染，板）、§5.1（服务端组件·故事数据链路，云）
**可行性**：✅（朗读砍除后纯板端渲染，host 已验）
```


## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

| 类别 | 路径 |
|---|---|
| designRefs | `docs/ui/V9/canvas-p1.png` |
| designRefs | `docs/ui/V9/canvas-p2.png` |
| designRefs | `docs/ui/V9/canvas-p3.png` |
| designRefs | `docs/ui/V9/p3-reader.png` |
| designRefs | `docs/ui/V9/points-home-nodata.png` |
| sharedUiRefs | `docs/ui/kidpal-ui-design-v9.html` |
| sharedUiRefs | `docs/ui/UIUX-DESIGN-BRIEF.md` |

页级 PNG（400×300）= **布局坐标定位权威**：layout 常量按图定，文字线框仅口径补充。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/front/front_app.py` | `_consume_kidpal_control` | :629-676 | 1326 |

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
| `workstream:261103-star-points-story-cover` | TECH-261028 前移与 K5 回归 | `261104-k0a-four-page-skeleton` | `261104-k0a-four-page-skeleton` | K0a 唯一承接修复，K5 只验证封皮改造零回归，不重复结项。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

## ⑥ 非阻断 advisory 明细（终端只打汇总；此处保留定位）

- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§四-kidpal_session_ctx`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§四-kidpal_session_ctx（起包期核对锚点或目标文档是否已变更）
- **info · whole-document** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
- **info · whole-document** · ref=`delivery/workstreams/261103-star-points-story-cover/task_plan.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261103-star-points-story-cover/task_plan.md
- **info · whole-document** · ref=`delivery/taskpackages/261104-k0a-four-page-skeleton/plan.md`：Markdown 整文件引用（不自动猜章节）：delivery/taskpackages/261104-k0a-four-page-skeleton/plan.md
- **info · whole-document** · ref=`docs/requirements.md`：Markdown 整文件引用（不自动猜章节）：docs/requirements.md
- **info · whole-document** · ref=`docs/architecture.md`：Markdown 整文件引用（不自动猜章节）：docs/architecture.md
- **info · whole-document** · ref=`docs/contracts/interface-contract.md`：Markdown 整文件引用（不自动猜章节）：docs/contracts/interface-contract.md
- **info · whole-document** · ref=`docs/decisions/ADR.md`：Markdown 整文件引用（不自动猜章节）：docs/decisions/ADR.md
- **info · whole-document** · ref=`docs/debt/tech/TECH-261028-p3-reader-key-falls-to-session-toggle.md`：Markdown 整文件引用（不自动猜章节）：docs/debt/tech/TECH-261028-p3-reader-key-falls-to-session-toggle.md
- **info · whole-document** · ref=`docs/debt/tech/TECH-261022-p3-record-session-state-churn.md`：Markdown 整文件引用（不自动猜章节）：docs/debt/tech/TECH-261022-p3-record-session-state-churn.md
- **warning · bare-non-markdown** · ref=`docs/ui/kidpal-ui-design-v9.html`：ref 无锚点，清单退化为整文件引用：docs/ui/kidpal-ui-design-v9.html（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **info · whole-document** · ref=`docs/ui/UIUX-DESIGN-BRIEF.md`：Markdown 整文件引用（不自动猜章节）：docs/ui/UIUX-DESIGN-BRIEF.md
- **warning · bare-non-markdown** · ref=`docs/ui/V9/canvas-p1.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/canvas-p1.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`docs/ui/V9/canvas-p2.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/canvas-p2.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`docs/ui/V9/canvas-p3.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/canvas-p3.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`docs/ui/V9/p3-reader.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/p3-reader.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`docs/ui/V9/points-home-nodata.png`：ref 无锚点，清单退化为整文件引用：docs/ui/V9/points-home-nodata.png（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-01`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-01（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-08`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md#§3.1-D-08（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`delivery/workstreams/261103-star-points-story-cover/task_plan.md#K0a`：ref 的显式锚点未定位到标题节：delivery/workstreams/261103-star-points-story-cover/task_plan.md#K0a（起包期核对锚点或目标文档是否已变更）
- **warning · bare-non-markdown** · ref=`firmware/main/display/story_page.c`：ref 无锚点，清单退化为整文件引用：firmware/main/display/story_page.c（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`firmware/main/display/kidpal_render_node.h`：ref 无锚点，清单退化为整文件引用：firmware/main/display/kidpal_render_node.h（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`firmware/main/display/kidpal_render_node.cc`：ref 无锚点，清单退化为整文件引用：firmware/main/display/kidpal_render_node.cc（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`firmware/main/display/keybar.c`：ref 无锚点，清单退化为整文件引用：firmware/main/display/keybar.c（非 Markdown 路径不适用文档豁免，起包期核对引用形态）
- **warning · bare-non-markdown** · ref=`firmware/main/key_gesture_parser.h`：ref 无锚点，清单退化为整文件引用：firmware/main/key_gesture_parser.h（非 Markdown 路径不适用文档豁免，起包期核对引用形态）

