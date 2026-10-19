# input-brief · 261018-sd-log-persist

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-18T14:11:37.075Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261018-sd-log-persist/taskpackage.json` · subsystem: firmware/main/diagnostics（sd_log 纯逻辑 host 同源 seam + sd_log_fs 芯片面）+ firmware/main（app_main 最早期装 esp_log 钩子）+ firmware/main/boards/waveshare-s3-rlcd-4.2（挂载成功分支 attach + max_files 5→8）+ firmware/main/CMakeLists.txt 与 firmware/main/test/host_app/main/CMakeLists.txt 两处显式登记 + firmware/main/test（host 验收件） · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261018-arch-review-sd-log-persist/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/intake/261018-sd-log-persist.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261018-sd-log-persist/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | `§6.6` | 6.6 可观测性 | 330 | ok |
| `docs/architecture.md` | `§10` | 10. 决策记录 | 395 | ok |
| `docs/ADR.md` | `ADR-012` | ADR-012 · 设备诊断日志落 TF 卡 = 决策 24 介质禁令白名单增列（常开 + 有界轮转、可删除、含对话原文） | 102 | ok |
| `docs/debt/tech/TECH-261016-sd-story-confidentiality.md` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 纯板端本地诊断日志采集与落卡：零 wire 形状改动（帧类型 / 字段类型与承载 / 回执语义 / 序列化形态全不动），不新增端云消息、不做远程开关；esp_log 钩子只消费设备本地日志流，server/** 零 diff（PROPOSAL 契约四分类 = NO_CONTRACT_CHANGE）。

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

