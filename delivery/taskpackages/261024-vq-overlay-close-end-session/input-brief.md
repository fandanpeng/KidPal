# input-brief · 261024-vq-overlay-close-end-session

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-24T11:20:29.036Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261024-vq-overlay-close-end-session/taskpackage.json` · subsystem: firmware/main/application（会话结束入口 + 主循环事件位）+ firmware/main/boards/waveshare-s3-rlcd-4.2（GLOBAL_ACT_OVERLAY_CLOSE 级联收窄）+ firmware/main/test（既有验收断言重钉与结构断言） · rigor: full
> **改动指令源 = plan.md**：`delivery/taskpackages/261024-vq-overlay-close-end-session/plan.md`（bug 类包——本 brief ①b 为指针形态，真实改动指令在此）。

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261024-overlay-close-speaking-no-session-end.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261024-vq-overlay-close-end-session/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `§2.3` | 2.3 按键手势（3 键，无触摸；本节为权威键位定义） | 130 | ok |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 纯板端会话收尾语义修复：abort / close 均为既有 WS 消息，不新增 wire 语义、不改帧类型与字段类型；server/** 零 diff；设备端无跨端数据类变更。triage architect 逃生门预查 = NO_CONTRACT_CHANGE。

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

