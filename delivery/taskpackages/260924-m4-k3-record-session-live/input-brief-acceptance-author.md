# input-brief · acceptance-author · 260924-m4-k3-record-session-live

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-30T10:06:44.062Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/260924-m4-k3-record-session-live/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§四` | 四、端云协议剖面（小智协议消息子集，2026-09-03 新增，方向 B） | 258 | ok |
| `docs/contracts/interface-contract.md` | `§1.4` | 1.4 控制类指令（OpenClaw 判意图后下发） | 160 | ok |
| `docs/requirements.md` | `S-F2.2` | S-F2.2 · 开录（双入口）（原「一键开录」「一键录故事」） | 418 | ok |
| `docs/requirements.md` | `§2.1` | 2.1 语音交互模型（已定：按一下 KEY / 喊唤醒词启动会话；KEY 打断停播） | 30 | ok |
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 203 | ok |
| `docs/architecture.md` | `§5.5.5` | 5.5.5 OpenClaw system prompt 版本化 | 295 | ok |
| `delivery/workstreams/m4-star-notebook/progress.md` | — | 无锚点（整文件） | — | bare |
| `delivery/changes/dev/260928-story-connect-push-defer/review-packet.md` | — | 无锚点（整文件） | — | bare |
| `firmware/main/application.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/protocols/websocket_protocol.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/protocols/protocol.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/mcp_server.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/story_page.c` | — | 无锚点（整文件） | — | bare |
| `firmware/main/display/kidpal_render_node.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/session_state_machine.h` | — | 无锚点（整文件） | — | bare |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |
| `server/gateway/story.py` | — | 无锚点（整文件） | — | bare |
| `server/deploy/openclaw-system-prompt.v6.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/openclaw-system-prompt.v7.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | — | 无锚点（整文件） | — | bare |
| `server/deploy/provider.config.yaml` | — | 无锚点（整文件） | — | bare |
| `server/deploy/apply-xiaozhi-overlay.sh` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）

### S-F2.2-AC1 · 源 = `docs/requirements.md` 节首行 :418 · 本包声明 AC 粒度回引：`S-F2.2-AC1`

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

### S-F2.2-AC3 · 源 = `docs/requirements.md` 节首行 :418 · 本包声明 AC 粒度回引：`S-F2.2-AC3`

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

### S-F2.1-AC1 · 源 = `docs/requirements.md` 节首行 :403 · 本包声明 AC 粒度回引：`S-F2.1-AC1`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.1 · 看「星星记录本」列表

> 作为孩子，我希望看到我记下来的所有内容排成列表、每篇都是一颗星星，以便有收集的成就感。

**验收标准**
- AC1：Given 进星星记录本，When 看列表，Then 星星计数（×N，右上角），每条 = 星星图标 + 标题 + 日期；**光标默认选中第一条**，选中行**带边框**（不反白——反白真机效果差，2026-09-14 human 裁）；**列表超一屏时底部显示「第 x 屏 · 共 y 屏」**（与阅读态分屏同心智，v1.28）。
- AC2：Given 列表中，When BOOT 短按，Then 选中项下移——光标到当前屏底再按翻下一屏并落该屏首项，列表到底 wrap 回首项（永不离页）；When BOOT 双击，Then 直接翻下一屏、光标落该屏首项（长列表快速跳屏；最后一屏再双击 wrap 回首屏）；When KEY 双击，Then 进入阅读态（见 S-F2.3）。
- AC3：Given 星星记录本任意时刻，When 孩子看屏，Then 常驻语音入口提示「说『我要讲故事』开录新的」。
- AC4：Given 列表选中某篇，When BOOT 长按，Then 弹出删除确认弹窗（**整屏覆盖层，WF-F2-05**，大字「删掉这篇？」+ 待删条目，页点消失同覆盖层口径；v1.48 human 裁全屏形态）：KEY 短按 = 确认删除（条目移除、星星计数 −1、云端同步）；BOOT 任一手势 = 取消回列表；10 秒无操作静默取消回列表；其余手势不响应。

**线框**：WF-F2-01（实存）+ WF-F2-05（删除确认弹窗，v1.48）。
**补充规则**：出路口径（v1.29 human 裁）= 列表态**无按键翻页出口**（BOOT 本页不翻页——多屏列表里「到底翻页」与「翻下一屏」直觉冲突），离开 = KEY 双击进阅读 / KEY 长按回首页（列表 = P3 顶层，长按一层即到家）/ 语音切页（「打开课表页」等）——页循环在本页断开，对话为主产品可接受。埋点 = 无。
**锚**：UI = `canvas-p3` ｜ 支撑 = arch §5.5.4（UI 演出·星星记录本渲染，板）、§5.1（服务端组件·故事数据链路，云）
**可行性**：✅
```

### S-F2.1-AC2 · 源 = `docs/requirements.md` 节首行 :403 · 本包声明 AC 粒度回引：`S-F2.1-AC2`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.1 · 看「星星记录本」列表

> 作为孩子，我希望看到我记下来的所有内容排成列表、每篇都是一颗星星，以便有收集的成就感。

**验收标准**
- AC1：Given 进星星记录本，When 看列表，Then 星星计数（×N，右上角），每条 = 星星图标 + 标题 + 日期；**光标默认选中第一条**，选中行**带边框**（不反白——反白真机效果差，2026-09-14 human 裁）；**列表超一屏时底部显示「第 x 屏 · 共 y 屏」**（与阅读态分屏同心智，v1.28）。
- AC2：Given 列表中，When BOOT 短按，Then 选中项下移——光标到当前屏底再按翻下一屏并落该屏首项，列表到底 wrap 回首项（永不离页）；When BOOT 双击，Then 直接翻下一屏、光标落该屏首项（长列表快速跳屏；最后一屏再双击 wrap 回首屏）；When KEY 双击，Then 进入阅读态（见 S-F2.3）。
- AC3：Given 星星记录本任意时刻，When 孩子看屏，Then 常驻语音入口提示「说『我要讲故事』开录新的」。
- AC4：Given 列表选中某篇，When BOOT 长按，Then 弹出删除确认弹窗（**整屏覆盖层，WF-F2-05**，大字「删掉这篇？」+ 待删条目，页点消失同覆盖层口径；v1.48 human 裁全屏形态）：KEY 短按 = 确认删除（条目移除、星星计数 −1、云端同步）；BOOT 任一手势 = 取消回列表；10 秒无操作静默取消回列表；其余手势不响应。

**线框**：WF-F2-01（实存）+ WF-F2-05（删除确认弹窗，v1.48）。
**补充规则**：出路口径（v1.29 human 裁）= 列表态**无按键翻页出口**（BOOT 本页不翻页——多屏列表里「到底翻页」与「翻下一屏」直觉冲突），离开 = KEY 双击进阅读 / KEY 长按回首页（列表 = P3 顶层，长按一层即到家）/ 语音切页（「打开课表页」等）——页循环在本页断开，对话为主产品可接受。埋点 = 无。
**锚**：UI = `canvas-p3` ｜ 支撑 = arch §5.5.4（UI 演出·星星记录本渲染，板）、§5.1（服务端组件·故事数据链路，云）
**可行性**：✅
```

### S-F2.1-AC3 · 源 = `docs/requirements.md` 节首行 :403 · 本包声明 AC 粒度回引：`S-F2.1-AC3`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.1 · 看「星星记录本」列表

> 作为孩子，我希望看到我记下来的所有内容排成列表、每篇都是一颗星星，以便有收集的成就感。

**验收标准**
- AC1：Given 进星星记录本，When 看列表，Then 星星计数（×N，右上角），每条 = 星星图标 + 标题 + 日期；**光标默认选中第一条**，选中行**带边框**（不反白——反白真机效果差，2026-09-14 human 裁）；**列表超一屏时底部显示「第 x 屏 · 共 y 屏」**（与阅读态分屏同心智，v1.28）。
- AC2：Given 列表中，When BOOT 短按，Then 选中项下移——光标到当前屏底再按翻下一屏并落该屏首项，列表到底 wrap 回首项（永不离页）；When BOOT 双击，Then 直接翻下一屏、光标落该屏首项（长列表快速跳屏；最后一屏再双击 wrap 回首屏）；When KEY 双击，Then 进入阅读态（见 S-F2.3）。
- AC3：Given 星星记录本任意时刻，When 孩子看屏，Then 常驻语音入口提示「说『我要讲故事』开录新的」。
- AC4：Given 列表选中某篇，When BOOT 长按，Then 弹出删除确认弹窗（**整屏覆盖层，WF-F2-05**，大字「删掉这篇？」+ 待删条目，页点消失同覆盖层口径；v1.48 human 裁全屏形态）：KEY 短按 = 确认删除（条目移除、星星计数 −1、云端同步）；BOOT 任一手势 = 取消回列表；10 秒无操作静默取消回列表；其余手势不响应。

**线框**：WF-F2-01（实存）+ WF-F2-05（删除确认弹窗，v1.48）。
**补充规则**：出路口径（v1.29 human 裁）= 列表态**无按键翻页出口**（BOOT 本页不翻页——多屏列表里「到底翻页」与「翻下一屏」直觉冲突），离开 = KEY 双击进阅读 / KEY 长按回首页（列表 = P3 顶层，长按一层即到家）/ 语音切页（「打开课表页」等）——页循环在本页断开，对话为主产品可接受。埋点 = 无。
**锚**：UI = `canvas-p3` ｜ 支撑 = arch §5.5.4（UI 演出·星星记录本渲染，板）、§5.1（服务端组件·故事数据链路，云）
**可行性**：✅
```

### S-F2.1-AC4 · 源 = `docs/requirements.md` 节首行 :403 · 本包声明 AC 粒度回引：`S-F2.1-AC4`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.1 · 看「星星记录本」列表

> 作为孩子，我希望看到我记下来的所有内容排成列表、每篇都是一颗星星，以便有收集的成就感。

**验收标准**
- AC1：Given 进星星记录本，When 看列表，Then 星星计数（×N，右上角），每条 = 星星图标 + 标题 + 日期；**光标默认选中第一条**，选中行**带边框**（不反白——反白真机效果差，2026-09-14 human 裁）；**列表超一屏时底部显示「第 x 屏 · 共 y 屏」**（与阅读态分屏同心智，v1.28）。
- AC2：Given 列表中，When BOOT 短按，Then 选中项下移——光标到当前屏底再按翻下一屏并落该屏首项，列表到底 wrap 回首项（永不离页）；When BOOT 双击，Then 直接翻下一屏、光标落该屏首项（长列表快速跳屏；最后一屏再双击 wrap 回首屏）；When KEY 双击，Then 进入阅读态（见 S-F2.3）。
- AC3：Given 星星记录本任意时刻，When 孩子看屏，Then 常驻语音入口提示「说『我要讲故事』开录新的」。
- AC4：Given 列表选中某篇，When BOOT 长按，Then 弹出删除确认弹窗（**整屏覆盖层，WF-F2-05**，大字「删掉这篇？」+ 待删条目，页点消失同覆盖层口径；v1.48 human 裁全屏形态）：KEY 短按 = 确认删除（条目移除、星星计数 −1、云端同步）；BOOT 任一手势 = 取消回列表；10 秒无操作静默取消回列表；其余手势不响应。

**线框**：WF-F2-01（实存）+ WF-F2-05（删除确认弹窗，v1.48）。
**补充规则**：出路口径（v1.29 human 裁）= 列表态**无按键翻页出口**（BOOT 本页不翻页——多屏列表里「到底翻页」与「翻下一屏」直觉冲突），离开 = KEY 双击进阅读 / KEY 长按回首页（列表 = P3 顶层，长按一层即到家）/ 语音切页（「打开课表页」等）——页循环在本页断开，对话为主产品可接受。埋点 = 无。
**锚**：UI = `canvas-p3` ｜ 支撑 = arch §5.5.4（UI 演出·星星记录本渲染，板）、§5.1（服务端组件·故事数据链路，云）
**可行性**：✅
```

### S-F2.3-AC1 · 源 = `docs/requirements.md` 节首行 :433 · 本包声明 AC 粒度回引：`S-F2.3-AC1`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.3 · 回看记录（原「回看记录与朗读」，v1.52 砍朗读）

> 作为孩子，我希望选一篇记录能看到它的文字，以便重温我记下来的东西。

**验收标准**
- AC1：Given 列表选中某篇，When KEY 双击，Then 进阅读态：标题 + 正文分页，底部「第 1 屏 · 共 3 屏」。
- AC2：Given 阅读中，When BOOT 短按（双击同短按），Then 翻下一屏——末屏再翻 wrap 回首屏、**永不离页**（离开 = KEY 长按退出阅读回列表（v1.36 返回上一层，再长按回首页）/ 语音切页）；When 按 KEY 短按，Then 不响应（朗读已砍，v1.52）。

**线框**：WF-F2-04（阅读态，纯阅读）。
**补充规则**：数据口径 = 正文排版 13.5px 起、行高 1.72（可读性口径，回引 NFR-READ-1）。阅读态口径 = **一义到底（纯阅读），无会话/问答入口**（2026-09-14 human 裁；v1.52 砍朗读，KEY 短按不响应）——问生字、问故事内容请先退出（KEY 长按逐层返回）再问，故事语境会话（按 KEY 带故事背景开会话）同案不取，本期不考虑故事内问答。已砍联动 = 「生字反白高亮」「生字跳查字页（原 AC3）」「朗读出音（v1.52）」（见已砍清单）。埋点 = 无。
**锚**：UI = `p3-reader` ｜ 支撑 = arch §5.5.4（UI 演出·阅读态渲染，板）、§5.1（服务端组件·故事数据链路，云）
**可行性**：✅（朗读砍除后纯板端渲染，host 已验）
```

### S-F2.3-AC2 · 源 = `docs/requirements.md` 节首行 :433 · 本包声明 AC 粒度回引：`S-F2.3-AC2`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.3 · 回看记录（原「回看记录与朗读」，v1.52 砍朗读）

> 作为孩子，我希望选一篇记录能看到它的文字，以便重温我记下来的东西。

**验收标准**
- AC1：Given 列表选中某篇，When KEY 双击，Then 进阅读态：标题 + 正文分页，底部「第 1 屏 · 共 3 屏」。
- AC2：Given 阅读中，When BOOT 短按（双击同短按），Then 翻下一屏——末屏再翻 wrap 回首屏、**永不离页**（离开 = KEY 长按退出阅读回列表（v1.36 返回上一层，再长按回首页）/ 语音切页）；When 按 KEY 短按，Then 不响应（朗读已砍，v1.52）。

**线框**：WF-F2-04（阅读态，纯阅读）。
**补充规则**：数据口径 = 正文排版 13.5px 起、行高 1.72（可读性口径，回引 NFR-READ-1）。阅读态口径 = **一义到底（纯阅读），无会话/问答入口**（2026-09-14 human 裁；v1.52 砍朗读，KEY 短按不响应）——问生字、问故事内容请先退出（KEY 长按逐层返回）再问，故事语境会话（按 KEY 带故事背景开会话）同案不取，本期不考虑故事内问答。已砍联动 = 「生字反白高亮」「生字跳查字页（原 AC3）」「朗读出音（v1.52）」（见已砍清单）。埋点 = 无。
**锚**：UI = `p3-reader` ｜ 支撑 = arch §5.5.4（UI 演出·阅读态渲染，板）、§5.1（服务端组件·故事数据链路，云）
**可行性**：✅（朗读砍除后纯板端渲染，host 已验）
```

### S-F2.4-AC1 · 源 = `docs/requirements.md` 节首行 :446 · 本包声明 AC 粒度回引：`S-F2.4-AC1`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.4 · 星星保存成就盖章（原「故事保存成就盖章」）

> 作为孩子，我希望每记下一篇就得到一颗盖章徽章，以便觉得"我做到了"。

**验收标准**
- AC1：Given 录音保存成功那一瞬，When 触发，Then 全屏只剩勋章：双圈印章 + 文案「星星徽章·第 N 颗已入册！」+ 小伴跳起，定格 0.8 秒。
- AC2：Given 盖章演出结束，When 回星星记录本，Then 星星计数 +1。**删除口径（v1.47）**：星星计数 = **现存篇数**（列表态确认删除即 −1）；盖章文案的 N = **入册流水号只增不减**（「第 N 颗已入册」是历史成就，不被删除改写）。

**线框**：WF-F2-03（盖章演出，全屏定格）。
**补充规则**：无。
**锚**：UI = `p3-stamp-frame` + `comp-stamp`（组件为验收对象，write-story 锚行规则 3 例外单锚） ｜ 支撑 = arch §5.5.4（UI 演出·盖章演出渲染，板）
**可行性**：✅
```

### S-F2.4-AC2 · 源 = `docs/requirements.md` 节首行 :446 · 本包声明 AC 粒度回引：`S-F2.4-AC2`

> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。

```markdown
##### S-F2.4 · 星星保存成就盖章（原「故事保存成就盖章」）

> 作为孩子，我希望每记下一篇就得到一颗盖章徽章，以便觉得"我做到了"。

**验收标准**
- AC1：Given 录音保存成功那一瞬，When 触发，Then 全屏只剩勋章：双圈印章 + 文案「星星徽章·第 N 颗已入册！」+ 小伴跳起，定格 0.8 秒。
- AC2：Given 盖章演出结束，When 回星星记录本，Then 星星计数 +1。**删除口径（v1.47）**：星星计数 = **现存篇数**（列表态确认删除即 −1）；盖章文案的 N = **入册流水号只增不减**（「第 N 颗已入册」是历史成就，不被删除改写）。

**线框**：WF-F2-03（盖章演出，全屏定格）。
**补充规则**：无。
**锚**：UI = `p3-stamp-frame` + `comp-stamp`（组件为验收对象，write-story 锚行规则 3 例外单锚） ｜ 支撑 = arch §5.5.4（UI 演出·盖章演出渲染，板）
**可行性**：✅
```


## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/front/front_app.py` | `_story_feed_from_stt` | :104-150 | 445 |
| `server/gateway/front/front_app.py` | `_story_route_display_payload` | :91-150 | 405 |
| `server/gateway/story.py` | `StoryAccumulator` | :455-495 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260924-m4-k2-p3-star-notebook-ui | `firmware/main/test/story_page_v2.acceptance.c` |
| 260923-m3-k6-boot-auto-connect | `firmware/main/test/boot_auto_connect.acceptance.c` |
| 260923-m3-k7-schedule-grid-restore | `server/acceptance/test_schedule_grid_restore.acceptance.py` · `firmware/main/test/schedule_grid_restore.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |
|---|---|---|---|---|
| `package:260924-m4-k1-story-cloud-link` | kidpal_session_ctx 信号双端实现（契约已 LANDED 零改动） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | front 拦截半 = K1（M4K1-A2）；板端发送半 = K3（M4K3-A1）；两端合并前互不阻塞（省略消息 = 普通会话向后兼容）。 |
| `package:260924-m4-k1-story-cloud-link` | K1/K2 live 真机半（structural deferred 承接） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | 单真机不两轮撞车——K1-A7/K2-A7 live 半全部挂 K3 设备轮一次性收口；K3 设备轮前 K1/K2 host 半须 merged。 |
| `package:260924-m4-k2-p3-star-notebook-ui` | mcp_server.cc story 路由面双写串行 | `260924-m4-k2-p3-star-notebook-ui` | `260924-m4-k3-record-session-live` | K2 改形 story_list/story_text 消费 + 删除发送接线；K3 在其上加 stt 回显 + 录音 control + 引导态——mergeOrder 39→40 串行，K3 intake rebase K2 冻结面。 |
| `package:260924-m4-k2-p3-star-notebook-ui` | K1/K2 live 真机半（structural deferred 承接） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | K2 真机目测半（列表/弹窗/阅读/盖章四面 1bpp 目测 + 300ms 消歧手感）挂 K3 设备轮；K3 设备轮前 K2 host 半须 merged。 |
| `package:260928-story-connect-push-defer` | story_list 三时点①推送时机语义变更（建连即推摘除） | `260928-story-connect-push-defer` | `260924-m4-k3-record-session-live` | ①的恢复时机（设备进 P3 语境推送 / RAM 缓存填充）归 K3 session_ctx 设备半落地后重裁——K3 planner/integrator 消费本包 docs-delta 与 K1 notes §5.2 行为等价假设②的实况教训（「建连」在真机拓扑 = 每会话）。K2（板端 P3）消费面不受影响：②③时点与 wire 形状不变，板端收到 story_list 即渲染/缓存的语义由 K2 设计承载。 |
| `workstream:m4-star-notebook` | kidpal_session_ctx 信号双端实现（契约已 LANDED 零改动） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | front 拦截半 = K1（M4K1-A2，上行泵解析处拦截不下透）；板端发送半 = K3（M4K3-A1，protocol 原语 + application.cc 会话发起入口读 layer_top）；两端合并前互不阻塞（省略消息 = 普通会话向后兼容，契约既有语义）。 |
| `workstream:m4-star-notebook` | K1/K2 live 真机半（structural deferred 承接） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | 单真机不两轮撞车（M2/M3 先例）——K1-A7/K2-A7 live 半全部挂 K3 设备轮一次性收口；K3 设备轮前 K1/K2 host 半须 merged。 |
| `workstream:m4-star-notebook` | mcp_server.cc story 路由面双写串行 | `260924-m4-k2-p3-star-notebook-ui` | `260924-m4-k3-record-session-live` | K2 改形 story_list/story_text 消费 + 删除发送接线；K3 在其上加 stt 回显消费 + 录音 control + 引导态——mergeOrder 39→40 串行，K3 intake rebase K2 冻结面、新齿对 K2 新缝写不改 K2 语义。 |
| `workstream:m4-star-notebook` | 删除发送原语复用（protocol 层 §四消息发送缝） | `260924-m4-k2-p3-star-notebook-ui` | `260924-m4-k3-record-session-live` | K2 落 kidpal_story_delete 发送原语；K3 session_ctx 发送同族扩展（同一缝、不改 K2 冻结断言）。门 1 F6 聚合完备性修复：包级登记为准、workstream 级同步收录。 |
| `workstream:m4-star-notebook` | story_list 推送触发重裁（建连即推摘除后①恢复语义） | `260924-m4-k1-story-cloud-link` | `260924-m4-k3-record-session-live` | defer 包 260928-story-connect-push-defer 实证摘除①建连时点（现役两时点 = 归档后 + 删除后）；恢复语义重裁归 K3（引导态会话建立同拍为候选缝），结论归 acceptance 冻齿；K2 消费面不受影响（列表渲染恒消费最新 story_list，不预钉建连时点）。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

