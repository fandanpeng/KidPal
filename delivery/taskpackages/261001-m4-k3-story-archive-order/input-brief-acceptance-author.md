# input-brief · acceptance-author · 261001-m4-k3-story-archive-order

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-01T04:38:48.571Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/261001-m4-k3-story-archive-order/taskpackage.json` · subsystem: server/gateway/front · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.4` | 1.4 控制类指令（OpenClaw 判意图后下发） | 160 | ok |
| `docs/requirements.md` | `S-F2.2` | S-F2.2 · 开录（双入口）（原「一键开录」「一键录故事」） | 418 | ok |
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 203 | ok |
| `delivery/changes/intake/261001-m4-k3-record-session-live-repair.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261001-m4-k3-story-archive-order/plan.md` | — | 无锚点（整文件） | — | bare |
| `delivery/changes/dev/261001-m4-k3-record-session-live-repair/pod-retro.md` | — | 无锚点（整文件） | — | bare |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |
| `server/gateway/story.py` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F2.2-AC2 · 源 = `docs/requirements.md` 节首行 :418 · 本包声明 AC 粒度回引：`S-F2.2-AC2`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.2 · 开录（双入口）（原「一键开录」「一键录故事」）

> 作为孩子，我希望在星星记录本按一下 KEY 就能开始记、或者说「我要讲故事」「帮我记下来」就开始录、知道正在录，以便把想记下来的东西留住。

**验收标准**
- AC1：Given 在星星记录本按 KEY（无触发词），When 会话建立，Then 屏显记录语境引导「需要我帮你记点什么？」+ 聆听标识，孩子开口第一句即进录音态（REC 闪点 + 大时长 + 声波跳动 + 「跟着感觉说，我在记！」）；本会话**一刀切纯记录**——内容全部累积，不做逐句意图切换。
- AC2：Given 录音中，When 孩子说「我说完了」/「讲完啦」，Then 停止录音并保存，新的一篇出现在列表；录音中 **AI 不逐句回复**——切句只用于话语回显，收尾信号前设备闭嘴等待。
- AC3：Given 保存成功那一瞬，When 触发，Then 成就盖章演出（见 S-F2.4）。
- AC4：Given 录音中，When 再按 KEY（兜底收尾，与全局会话结束手势一致），Then 停录并保存；KEY 长按**同短按（保存）**——防长按误触毁稿（v1.47）；When 按 BOOT（短按/长按/双击同义）或说「讲错啦」，Then 取消录音、已录内容丢弃退回列表——**不设重录恢复**（v1.49 human 裁先不加，见 §8）。

**线框**：WF-F2-02（录音态）；记录语境引导态复用 WF-B-01 布局、气泡换「需要我帮你记点什么？」。
**补充规则**：录音不自动开始——翻到 P3 只见列表，BOOT 翻页路过不误触；P3 同时承担回看。双入口口径 = P3 按 KEY（**记录语境**，无需触发词）+ 任意页触发词（「我要讲故事」/「帮我记下来」，判意图后开录）。一刀切 = P3 按 KEY 的会话里**提问也被记录**（2026-09-14 human 裁，代价已知悉——提问请去其他页按 KEY）。技术信号 = 设备须把「会话发起自 P3」上下文告知云端、网关即切累积态（arch §5.1 故事归档组件；信号载体 = 契约 §四 `kidpal_session_ctx`，source_page=story，front 拦截切累积态）。误判挽回口径 = 被误判为闲聊的真实故事，孩子可经「刚才那段是故事」挽回。埋点 = 无。
**锚**：UI = `p3-recording` ｜ 支撑 = arch §5.1（故事归档组件）、§5.2 场景④（录音与归档链路，板+云）
**可行性**：✅（manual 聆听 + 网关累积态已备；P3 会话上下文信号 = 新增，随 arch §5.1 联动）
```

### S-F2.2-AC4 · 源 = `docs/requirements.md` 节首行 :418 · 本包声明 AC 粒度回引：`S-F2.2-AC4`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.2 · 开录（双入口）（原「一键开录」「一键录故事」）

> 作为孩子，我希望在星星记录本按一下 KEY 就能开始记、或者说「我要讲故事」「帮我记下来」就开始录、知道正在录，以便把想记下来的东西留住。

**验收标准**
- AC1：Given 在星星记录本按 KEY（无触发词），When 会话建立，Then 屏显记录语境引导「需要我帮你记点什么？」+ 聆听标识，孩子开口第一句即进录音态（REC 闪点 + 大时长 + 声波跳动 + 「跟着感觉说，我在记！」）；本会话**一刀切纯记录**——内容全部累积，不做逐句意图切换。
- AC2：Given 录音中，When 孩子说「我说完了」/「讲完啦」，Then 停止录音并保存，新的一篇出现在列表；录音中 **AI 不逐句回复**——切句只用于话语回显，收尾信号前设备闭嘴等待。
- AC3：Given 保存成功那一瞬，When 触发，Then 成就盖章演出（见 S-F2.4）。
- AC4：Given 录音中，When 再按 KEY（兜底收尾，与全局会话结束手势一致），Then 停录并保存；KEY 长按**同短按（保存）**——防长按误触毁稿（v1.47）；When 按 BOOT（短按/长按/双击同义）或说「讲错啦」，Then 取消录音、已录内容丢弃退回列表——**不设重录恢复**（v1.49 human 裁先不加，见 §8）。

**线框**：WF-F2-02（录音态）；记录语境引导态复用 WF-B-01 布局、气泡换「需要我帮你记点什么？」。
**补充规则**：录音不自动开始——翻到 P3 只见列表，BOOT 翻页路过不误触；P3 同时承担回看。双入口口径 = P3 按 KEY（**记录语境**，无需触发词）+ 任意页触发词（「我要讲故事」/「帮我记下来」，判意图后开录）。一刀切 = P3 按 KEY 的会话里**提问也被记录**（2026-09-14 human 裁，代价已知悉——提问请去其他页按 KEY）。技术信号 = 设备须把「会话发起自 P3」上下文告知云端、网关即切累积态（arch §5.1 故事归档组件；信号载体 = 契约 §四 `kidpal_session_ctx`，source_page=story，front 拦截切累积态）。误判挽回口径 = 被误判为闲聊的真实故事，孩子可经「刚才那段是故事」挽回。埋点 = 无。
**锚**：UI = `p3-recording` ｜ 支撑 = arch §5.1（故事归档组件）、§5.2 场景④（录音与归档链路，板+云）
**可行性**：✅（manual 聆听 + 网关累积态已备；P3 会话上下文信号 = 新增，随 arch §5.1 联动）
```


## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 纯 front（server 面）实现修复：不改任何 wire 消息、不触固件与契约/spec 面；聆听形态变更与引擎侧抑制逐句回复随 Package B（D3-甲）另立，不在本包

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

