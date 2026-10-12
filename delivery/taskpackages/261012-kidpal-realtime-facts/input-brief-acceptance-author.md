# input-brief · acceptance-author · 261012-kidpal-realtime-facts

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-12T04:30:02.226Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/261012-kidpal-realtime-facts/taskpackage.json` · subsystem: server/deploy + ECS OpenClaw 工作区 · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/requirements.md` | `NFR-SEC-2` | NFR-SEC-2 · 儿童数据出网记账 | 588 | ok |
| `docs/requirements.md` | `NFR-SAFE-1` | NFR-SAFE-1 · 内容安全 | 600 | ok |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | — | 无锚点（整文件） | — | bare |
| `docs/crq/tech/TECH-261012-weather-fabrication.md` | — | 无锚点（整文件） | — | bare |
| `requirements v1.62 NFR-SEC-2-4` | — | （文件不存在） | — | missing |
| `requirements v1.62 NFR-SAFE-1` | — | （文件不存在） | — | missing |
| `delivery/changes/intake/261012-kidpal-realtime-facts.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261012-kidpal-realtime-facts/plan.md` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 单面实现变更（server/deploy prompt 族 + 验收 additive 齿 + arch 指针 docs-sync）：不改 wire、固件零改动、契约 NO_CONTRACT_CHANGE 四分类留痕；同端三个 sync 面（声明/渲染/carrier）由 changeManifest + 四个版本无关守卫承载；部署面 = ECS overlay 重跑（v9 同款先例 exempt 形态，261010 决策 21 键同款上传前核远端==基线）

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 261010-prompt-v9-schedule-anti-narration | `server/acceptance/test_prompt_v7_round_routing.acceptance.py` · `server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py` |
| 261001-openclaw-model-name-overlay-drift | `server/acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py` · `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

