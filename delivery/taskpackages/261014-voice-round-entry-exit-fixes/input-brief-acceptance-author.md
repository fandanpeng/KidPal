# input-brief · acceptance-author · 261014-voice-round-entry-exit-fixes

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-14T05:45:10.870Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/261014-voice-round-entry-exit-fixes/taskpackage.json` · subsystem: firmware/main 会话状态分发面（application.cc：唤醒词入口直推 / tts stop 出口守卫 / sentence_start 轮来源采样）+ server/deploy/engine-patch（connection.py _kidpal_try_speak 误 await 修正） · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/crq/tech/TECH-261014-wakeword-connecting-voice-layer.md` | — | 无锚点（整文件） | — | bare |
| `docs/crq/tech/TECH-261014-post-speak-listening-lingering.md` | — | 无锚点（整文件） | — | bare |
| `docs/crq/tech/TECH-261014-kidpal-speak-await-nonetype.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261014-voice-round-entry-exit-fixes/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `S-F1.4` | S-F1.4 · 日程提醒 | 290 | ok |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F1.4-AC1 · 源 = `docs/requirements.md` 节首行 :290 · 本包声明 AC 粒度回引：`S-F1.4-AC1`

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

### S-F1.4-AC6 · 源 = `docs/requirements.md` 节首行 :290 · 本包声明 AC 粒度回引：`S-F1.4-AC6`

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


## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `firmware/main/application.cc` | `Application::InitializeProtocol` | :620-829 | — |
| `server/deploy/engine-patch/connection.py` | `_kidpal_try_speak` | :363-366 | 418 |

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

