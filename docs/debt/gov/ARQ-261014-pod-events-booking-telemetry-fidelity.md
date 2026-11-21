---
id: ARQ-261014-pod-events-booking-telemetry-fidelity
title: pod-events 簿记一致性与遥测保真度——escalation 闭合链缺、actor 归属误落、同决定双记、关联字段失效、模型档位与用量数据失真
domain: gov
status: deferred
severity: SHOULD
owner: architect（schema 约定）+ dev-loop（脚本落地）
trigger: 下次治理批 / 下个动 pod-events schema 约定或 pod-events-aggregate.mjs 的批
opened: 2026-10-14
closed: ""
closedBy: ""
paths: [".claude/scripts/pod-events-aggregate.mjs", ".claude/agents/planner.md"]
source: loop-retro-2026-10-14 18 包复盘（`delivery/changes/retro/2026-10-14-loop-retro.md`）
---

**证据（六类实据，loop-retro-2026-10-14）**：① **escalation-resolved 缺失**：261011-home-v5 `fe6c3f46` / `930257e5`、261012-realtime-facts `81b32bfb` 均由 human-decision 实际闭合但未补 agent-escalation-resolved 事件，聚合器把它们计为 unresolved（同族先例：loop-retro-2026-09-23 已录 k3-char-window / card-fix / display-fix 三例）——机器聚合永久高估未结数；② **actor 归属误落**：261001-m4-k3 `04f83250`（actor 误落 planner → `91bb853b` 归属修正重落，批内唯一簿记纠偏型事件）、260924-perf-k1 `13c53d7b`（actor=planner、source=human，与 K3 同病）；③ **同决定双记**：261001-openclaw `a5c3b27a`（actor=planner）/ `c3d96131`（actor=human）为同一批准的双记；④ **关联字段失效**：260928-char-window-key `4368504c` 用 `resolves` 字段而非 `refEventId` → 聚合器把 `570eb95d` 判 unresolved（实义已经 human `84c4e953` 消解）；⑤ **时间戳倒挂**：261012-realtime-facts `9d4c1f68`（06:45Z）晚于引用它作 approval-ref 的 merge-completed `7ed9f75d`（06:36Z），事后补记痕迹；⑥ **模型/用量数据失真**：260924-m4-k1 两次显式 [MODEL-TIER] opus 升档实际模型均 deepseek-v4.1-flash（byModel 五 dispatch 全 deepseek，retro 只留痕不裁决）、260924-perf-k1 漏升一次 + dispatch-stamp 传 0 充数（`054f8ed8` / `f56287b2` 同 dispatchId 双记、前者 durationMs/toolUses 填 0）、261013-tomorrow-row 全部 10 条 dispatch 的 mainThreadUsage 逐字段相同（cumulativeDelta 全 0，主线程 checkpoint 不可信）。

**影响面**：机器聚合（escalation 未结数 / token 覆盖 / mainThread 用量 / 模型分布）系统性失真，loop-retro 每轮 GC3 需人工补正；MODEL-TIER 升档链路「声明 ≠ 实际」使分层出场成本核算失去数据基础。

**修法候选**（交 review，不自批）：① escalation 事件支持被 human-decision 引用自动闭合（loop-retro-2026-09-23 已提，至今未落地——聚合器按 refEventId / approval-ref 关联即可）；② schema 校验收敛 actor 归属与关联字段（refEventId 单一写法，`resolves` 弃用或聚合器映射兼容）；③ MODEL-TIER 声明与 transcript 实际模型对账进 dispatch-stamp（不一致即 warn）；④ mainThreadUsage 快照刷新机制排查（261013 实证）。**riskTier: low**（纯记账 / 遥测面，不动任何门判据）。

**追加证据（loop-retro-2026-10-25，25 包复盘）**：① 模型/用量失真继续——261019-p3-record-dialogue-guide 记 dispatch-stamp 报的实际生效模型与派发指定不符（cc-switch 代理改写）；261022-m5-k3 记 transcript 派生模型显示与当前会话不同。② GC3 覆盖偏差系统性复发（25 包）：261020-p3-rec-frame-visual ledger friction=0 而 retro 记三主题；261022-m5-k3 ledger friction=1 而 retro 记四条；261013-lang-config 4 vs 六条卡点；261016-p3-story-body 3 vs 五条；261023-agents-md 4 条 friction payload 全空（malformed=4）语义不可恢复；反向 261018-sd-log-usb-read ledger friction=0 而 retro 明记两条过程教训。整轮 58 条 friction 中 over-scan 30 / unknown 7 / malformed 7，语义承载率偏低。建议本条修法优先级提升：事件类型枚举告警 + payload 必填校验（fail-open 告警）可与 261024-gov-review-retro-efficiency-instrumentation 的 T7 同批。证据：delivery/retro/2026-10-25-loop-retro.md §一/§四。

**范围裁定追注（2026-10-29，P9 立项 human 拍）**：P9（261029-gov-review-p9-script-hygiene-cluster）取核心 ① + ② + 追加校验（事件类型枚举告警 + payload 必填校验，fail-open）；**候选 ③（MODEL-TIER 声明与 transcript 实际模型对账）与 ④（mainThreadUsage 快照刷新机制排查）明确不在本批、排队后续批**——两者为排查型（涉 cc-switch 代理改写、checkpoint 机制等外部因素），与本卫生批量级不符；①② + 追加校验随 P9 落地结项后，本条目残余仅 ③④，届时按残余重评或拆条。

**追加证据（loop-retro-2026-11-17，19 包复盘）——枚举告警已生效，闭集本身没扩；并新增「零 dispatch 事件」断层**：① **schema 告警 138 条 / 19 包**，几乎全是 `unknown-event-type`（`merge-completed`、`human-merge-gate-approved`、`planner-ruling`、`verify-red`、`live-deploy`、`device-flash`、`acceptance-frozen`、`builder-complete` 等；单包最高 261104-k2 32 条 / 19 种）。P9 的「事件类型枚举告警」在工作，证据 = 告警被逐条亮出；缺的是闭集维护——写侧大量手写闭集外类型，`pod-events-append.mjs:29-42` 的 9 值闭集与真实词表脱节，聚合器只能 fail-open 跳过专项聚合。② **9 包零 `dispatch-completed`**（261028-m6-k1 / m6-k2a / m6-k2b、261031-cron、261104-k0 / k1 / k2 / k4 / k5）：遥测不是「token 字段缺」而是**整条 dispatch 事件没写**，效率表对这些包的 token / agent 分钟 / 模型全列空，过半新包的过程长度不可测。③ **escalation 缺闭合与缺字段**：261024-vq-overlay `29530b79` 与 261104-k2 `66447168` 均 unresolved——后者连 `category` 字段都没有（聚合为 unknown），且实际已由 A 案 + round 4 绿 + human 批合并消解，属 ① 的复发实例。④ **GC3 覆盖偏差 16/17**：17 个有 pod-retro 的包中 16 包「ledger 摩擦主题数 < retro 散文主题数」，其中 8 包为 `ledger=0 / retro=3..6`（详见 `delivery/retro/2026-11-17-loop-retro.md` §四）。**建议**：除既有 ①② 外，追加一条「`dispatch-completed` 缺失本身可见」——效率表对零 dispatch 包显式打印 `no-dispatch-events` 而非空行，避免把「没记录」误读成「没成本」。证据：delivery/retro/2026-11-17-loop-retro.md §一 / §四。
