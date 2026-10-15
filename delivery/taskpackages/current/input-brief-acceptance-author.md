# input-brief · acceptance-author · 261015-voice-rightcol-label-overflow

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-15T03:57:19.399Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/261015-voice-rightcol-label-overflow/taskpackage.json` · subsystem: firmware/main/display（lcd_display.cc 语音轮右列三 label 宽度 264→196） · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `S-F1.3` | S-F1.3 · 语音问课表 | 275 | ok |
| `delivery/taskpackages/261015-voice-rightcol-label-overflow/plan.md` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F1.3-AC1 · 源 = `docs/requirements.md` 节首行 :275 · 本包声明 AC 粒度回引：`S-F1.3-AC1`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F1.3 · 语音问课表

> 作为孩子，我希望直接问"我下节课在哪""明天有什么课"，设备告诉我，以便不用自己翻。

**验收标准**
- AC1：Given 任意页，When 按 KEY 说「明天有什么课」，Then 设备播报答复，答复文字同步上屏——**与百科问答同一问答框（共享底座播报态），不切课表页、不滚动定位**（2026-09-14 human 裁：问答不嵌课表页）。
- AC2：Given 孩子明确说「帮我显示课表/打开课表」，When 设备收到，Then 屏幕切到课表页（P2 周视图网格）。
- AC3：Given 答案较长，When 播报中，Then 声波条跳动 + 文字同步上屏——就是共享底座播报态本身（§2.2 播报中行，线框 WF-B-03）。
- AC4：Given 日程条目带备注（如携带画具），When 语音问课表，Then 答复包含备注内容。

**线框**：WF-B-01/02/03（共享底座语音轮三态；答复展示面 = WF-B-03 问答框）。
**补充规则**：数据口径 = 备注列参与语音答复（带物可语音读回）。原「屏幕切到课表页并滚动定位到相关课程行」作废（见 §8 已砍清单）。埋点 = 无。
**锚**：UI = 共享底座（聆听→思考→播报） ｜ 支撑 = arch §5.2 场景②（语音问答链路，云）、§5.1（front 组件·display 校验与下发，板）
**可行性**：✅（human 2026-09-13 实测定案）
```

### S-F1.3-AC3 · 源 = `docs/requirements.md` 节首行 :275 · 本包声明 AC 粒度回引：`S-F1.3-AC3`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F1.3 · 语音问课表

> 作为孩子，我希望直接问"我下节课在哪""明天有什么课"，设备告诉我，以便不用自己翻。

**验收标准**
- AC1：Given 任意页，When 按 KEY 说「明天有什么课」，Then 设备播报答复，答复文字同步上屏——**与百科问答同一问答框（共享底座播报态），不切课表页、不滚动定位**（2026-09-14 human 裁：问答不嵌课表页）。
- AC2：Given 孩子明确说「帮我显示课表/打开课表」，When 设备收到，Then 屏幕切到课表页（P2 周视图网格）。
- AC3：Given 答案较长，When 播报中，Then 声波条跳动 + 文字同步上屏——就是共享底座播报态本身（§2.2 播报中行，线框 WF-B-03）。
- AC4：Given 日程条目带备注（如携带画具），When 语音问课表，Then 答复包含备注内容。

**线框**：WF-B-01/02/03（共享底座语音轮三态；答复展示面 = WF-B-03 问答框）。
**补充规则**：数据口径 = 备注列参与语音答复（带物可语音读回）。原「屏幕切到课表页并滚动定位到相关课程行」作废（见 §8 已砍清单）。埋点 = 无。
**锚**：UI = 共享底座（聆听→思考→播报） ｜ 支撑 = arch §5.2 场景②（语音问答链路，云）、§5.1（front 组件·display 校验与下发，板）
**可行性**：✅（human 2026-09-13 实测定案）
```


## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 纯板端 LVGL 显示版式修复——零 wire 面：不触碰任何跨端 wire 形状（帧类型/字段类型与承载/回执语义/序列化形态），语音回显/答复数据链路与对端面零涉，无对端面

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

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

