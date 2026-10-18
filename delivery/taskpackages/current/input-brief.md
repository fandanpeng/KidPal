# input-brief · 261017-prompt-v11-schedule-recall

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-18T02:05:53.243Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> regenerated-because: integrator 收口：两条 debt 结项后由 debt-render 归位 _archive/2026，sourceRefs/docRefs/changeManifest 同步改指归档现址（引用精度修正，非语义变更）（覆盖上一份快照，旧 generated-at 见 git diff 上一版本）
> 来源 TaskPackage: `delivery/taskpackages/261017-prompt-v11-schedule-recall/taskpackage.json` · subsystem: server/deploy（prompt v11 四面接线 + KidPal 极简模板与 overlay 两键 + manifest 单一事实源 + openclaw.json kidpal skills 定点 patch）+ server/acceptance（additive v11 齿 / 新键锚扩展 / skills 声明离线契约） · rigor: lite
> **改动指令源 = plan.md**：`delivery/taskpackages/261017-prompt-v11-schedule-recall/plan.md`（bug 类包——本 brief ①b 为指针形态，真实改动指令在此）。

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `S-F1.3` | S-F1.3 · 语音问课表 | 277 | ok |
| `docs/architecture.md` | `5.5.5` | 5.5.5 OpenClaw system prompt 版本化 | 297 | ok |
| `docs/debt/tech/_archive/2026/TECH-261017-schedule-answer-skip-read-hallucination.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/_archive/2026/TECH-261017-xiaozhi-template-residue-deploy-ssot-gap.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261017-prompt-v11-schedule-recall/plan.md` | — | 无锚点（整文件） | — | bare |
| `delivery/intake/261017-prompt-v11-schedule-recall.md` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 纯部署面 prompt/配置/声明工件 + 验收齿变更：零 wire 形状改动（帧类型 / 字段类型与承载 / 回执语义 / 序列化形态全不动），设备侧与 server 网关协议零 diff；prompt 是 OpenClaw 运行时注入文本、非跨端帧；kidpal skills 是 OpenClaw 运行时 config 白名单、非仓内 agent 定义。契约面 NO_CONTRACT_CHANGE（display E1 / schedule view / B1 课表 MD 全不变）。

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

