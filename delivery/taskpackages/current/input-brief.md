# input-brief · 261019-p3-record-dialogue-guide

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-20T05:38:48.445Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261019-p3-record-dialogue-guide/taskpackage.json` · subsystem: server/gateway (story.py 卡壳词表/轮次纯决策/整理输入装配 + front_app.py sweep 重写/引导与整理 LLM 回合/start_organizing 下发/自推通路分叉/Package B 等价信号注入 + display_schema.py 分表 additive + 新增两枚 front 侧版本化 prompt 制品 + server/deploy/engine-patch/connection.py 记录语境标志认领/chat 入口第三守卫/stop-truncation 豁免重绑) + firmware/main (application.cc 聆听形态切换与 kvr 场景守卫 + display/view_router.h/.c VRT_CTRL_START_ORGANIZING=3 additive + mcp_server.cc 控制 case 3 消费 + display/story_page.c 帧 A 修订/引导播报中子态/整理过渡态渲染与键位分叉/思考态接线) · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.4` | 1.4 控制类指令（OpenClaw 判意图或 front 检出后下发） | 165 | ok |
| `docs/contracts/interface-contract.md` | `§四` | 四、端云协议剖面（小智协议消息子集，2026-09-03 新增，方向 B） | 266 | ok |
| `docs/requirements.md` | `NFR-PERF-3` | NFR-PERF-3 · 记录会话卡壳引导时机（v1.65 新增，`261019-p3-record-dialogue-guide`） | 614 | ok |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/decisions/ADR.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/intake/261019-p3-record-dialogue-guide.md` | — | 无锚点（整文件） | — | bare |
| `delivery/intake/261019-p3-record-dialogue-guide.design.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261019-p3-record-dialogue-guide/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/contract/CDQ-261019-p3-record-dialogue-guide.md` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V7/p3-recording.png` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V7/p3-recording-guide.png` | — | 无锚点（整文件） | — | bare |
| `docs/ui/V7/state-organizing.png` | — | 无锚点（整文件） | — | bare |
| `NFR-PERF-3` | — | （文件不存在） | — | missing |
| `docs/requirements.md` | `S-F1.4` | S-F1.4 · 日程提醒 | 293 | ok |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F2.2 · 源 = `docs/requirements.md` 节首行 :453 · 本包声明 AC 粒度回引：`S-F2.2`、`S-F2.2-AC1`、`S-F2.2-AC2`、`S-F2.2-AC3`、`S-F2.2-AC4`、`S-F2.2-AC5`

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
**补充规则**：录音不自动开始——翻到 P3 只见列表，BOOT 翻页路过不误触；P3 同时承担回看。双入口口径 = P3 按 KEY（**记录语境**，无需触发词）+ 任意页触发词（「我要讲故事」/「帮我记下来」，判意图后开录），两入口进**同一条记录语境对话态**，引导策略随内容自适应；孩子说「别说话听我讲」时 AI 收声只听（对话语境行为，不加独立状态）；**收声不停摆**——收声只抑制引导播报，卡壳计时照常走、被抑制的引导轮照常计入轮次，第 2 轮计时到仍无回应即自动收尾（AC5；依据 = 设计稿决策 6「孩子真离开则归档无害」的既定意图，非新语义，留痕 spine openQ q6）。**翻案留痕（v1.65）**：本 story 废除 2026-09-14「一刀切纯记录、提问也被记录」裁定，并连带改写架构决策 25 第二条聆听语义（原「manual 聆听 + 网关累积」→「累积 + 卡壳轻引导 + 收尾整理」）——当初定的理由 = 聆听语义简单可靠、录音不被 AI 插话污染；现在变 = 孩子对一台沉默设备讲完完整故事实际很难（卡壳即放弃），叙事脚手架对 8–12 岁目标是真实价值，且对话态下问答天然被滤出不进素材、「提问被记录」的代价随之消解；设计权威 = [261019-p3-record-dialogue-guide.design.md](../delivery/intake/261019-p3-record-dialogue-guide.design.md)（human 2026-10-19 逐段批准）。技术信号 = 设备须把「会话发起自 P3」上下文告知云端、网关即切**记录语境对话态**（arch §5.1 故事归档组件；信号载体 = 契约 §四 `kidpal_session_ctx`，source_page=story）。误判挽回口径 = 被误判为闲聊的真实故事，孩子可经「刚才那段是故事」挽回（**闲聊语境、其他页保留**——记录语境对话态内不再需要）。**收尾词「说完了」并入（v1.66，human 2026-10-20 裁：「收尾词表并入『说完了』，与『我说完了』『讲完啦』并存」）**：真机轮 R2 实证孩子自然语「好，说完了」（ASR 识别「好说完了。」）不触发收尾；收尾词检测 = 子串扫描（实现侧 `is_finish_phrase`，server/gateway/story.py 既有机制，arch §5.1 故事归档组件行在案），并入「说完了」一词天然覆盖「好，说完了」「我说完了」等前缀变体；261001 q3 悬案（「『说完了』是否并入收尾词表」，spine `261001-m4-k3-record-session-live-repair` q3，deferred）就此裁定关闭。埋点 = 无。
**锚**：UI = `p3-recording` ｜ 支撑 = arch §5.1（故事归档组件）、§5.2 场景④（录音与归档链路，板+云）
**可行性**：🟡（累积与归档链路已实测；卡壳计时引导与收尾忠实整理步为新增机制、未真机实证——设计可行，引导手感与整理忠实性随实现包验收，验收种子见设计稿）
```

### S-F1.4 · 源 = `docs/requirements.md` 节首行 :293 · 本包声明 AC 粒度回引：`S-F1.4-AC6`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F1.4 · 日程提醒

> 作为孩子，我希望遇到非常规的事（要带的东西、临时安排）和容易忘的课（周末培训班这类）设备提前主动提醒我，以便不丢三落四；例行去上的学校课我自己看课表就行。

**验收标准**
- AC1：Given 录入了明天要带画具的美术课，When 今天 20:30 到点，Then 设备提示音 + 语音读出提醒文案 + 弹窗驻留（大字事项 + 时间），弹窗驻留到按键。
- AC2：Given 弹窗驻留中，When 按 KEY，Then 关闭回当前页；When 按 BOOT，Then 关闭回原页、不翻页（v1.39 隔离）。
- AC3：Given 弹窗期间另一条提醒到达，When 收到，Then 排队不叠屏；当前弹窗关闭后下一条自动弹出。
- AC4：Given 同一提醒已弹过，When 云端因重算/重推再次下发同一提醒，Then 设备不重复弹窗。
- AC5：Given 设备断网，When 到点，Then 无提醒——设备不缓存事项（回引 NFR-OFF-1）。
- AC6：Given 提醒到达时前台存在任意覆盖 / 会话状态（查字窗开启、语音会话播报中等），When 弹出，Then 提醒为最高层覆盖当前内容；置顶期间 KEY/BOOT 按 AC2 语义处理，不穿透底层窗口（§2.2 层级口径）。**录音态例外**：Given 录音中，When 提醒到达，Then 排队延后、录音会话结束立刻弹（防提醒 TTS 混进录音内容）。

**线框**：WF-B-04（实存，提醒弹窗 = 四态同骨架之 pose-jump 形态）。
**补充规则**：范围口径 = 「学校课程表」区块不提醒（在校例行）；「每周固定课表」（培训班，不论周中周末）+ 一次性安排走三段阶梯，每周固定项每周重复生成；携带内容走课表备注列。三档时刻 = 前一天 20:30 / 当天 07:45 / 事项开始前 15 分钟（全局常量，家长侧可调性后置）。过期口径 = 已过时刻的档位不补发，不追催。弹窗驻留口径 = 驻留到按键，**不设无操作自动消失**（2026-09-14 human 裁：提醒比常显信息重要——前一天 20:30 档提醒的本来就是第二天的事，过夜驻留无害）。权限 = 录入仅家长侧（见 S-F1.5），设备端只读。埋点 = 无。
**锚**：UI = `state-reminder` ｜ 支撑 = arch §5.2 场景⑤（提醒推送链路，云）、§6.5（幂等与重试）、§5.5.4（UI 演出·提醒弹窗，板）
**可行性**：✅
```


## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/display_schema.py` | `GATEWAY_DECISION_CONTROL_ACTIONS` | :86-93 | — |
| `server/gateway/display_schema.py` | `validate_display` | :6-7 | 58 |
| `firmware/main/display/view_router.c` | `kidpal_display_control_action` | :68-91 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_on_control` | :1907-1936 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

（无同 subsystem 的已 merged 先例包）

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

