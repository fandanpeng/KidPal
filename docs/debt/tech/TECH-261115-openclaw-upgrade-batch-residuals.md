---
id: TECH-261115-openclaw-upgrade-batch-residuals
title: OpenClaw 2.0.4 升级批待办集——main 库 63 个 archived 僵尸行清理全免疫（疑缺陷）/ 重维护评估同步卡循环机制（触发源丙）仍在 / skill-collection-review cron 连续失败
domain: tech
status: deferred
severity: SHOULD
owner: 主线程（OpenClaw 网关运维面）
trigger: 下次 OpenClaw 升级/运维批（update-checker 报 2.0.4 可用，须过 runbook §0 版本漂移闸）；或 archived 僵尸行再次引发 slow session write；或重维护评估再撞出 30s 级冻结
opened: 2026-11-15
closed: ""
closedBy: ""
paths: []
source: 承接 TECH-261114-openclaw-gateway-eventloop-stalls（2026-11-15 结项）的残项 + 当日全天验证观察；证据链见该归档条目
handoff: 与 TECH-261022-openclaw-prep-tax-memory-pressure 同批（升级/内存复测）；升级前先备份（openclaw backup create，注意 legacy audit migration lease 瞬态超时形态，文件级 cp 兜底）
---

**残项一：main 库 ~63 个 archived 僵尸行对清理机制全免疫（疑似 OpenClaw 缺陷）**。2026-09-24 perf-k1 步⑦ archive 的历史会话：`archived_at` 已设、`created_at` 为 null、transcript 文件已失。实测 `pruneAfter`（14d）跳过、`maxEntries` 预算驱逐跳过、`--fix-missing` 不适用（archived 行按设计豁免常规驱逐）——惰性元数据，不阻塞主路径，但每次 store 维护扫描都要翻过它们。处置候选：升级 2.0.4 后重试 `openclaw sessions cleanup --enforce`；仍无效则 `openclaw doctor --session-sqlite recover --github-issue` 报上游（cleanup 免疫 + `created_at` null 跳过均可能在 2.0.4 修复）。

**残项二：重维护评估同步卡循环机制仍在（触发源丙）**。2026-11-14 三次 31438/31690/31524ms 高度一致的 30s 级事件循环冻结，分别发生在 remote catalog 刷新、session store 维护、session.maintenance config 热加载评估窗口——疑 OpenClaw 内部 ~30s 同步维护预算。已关断可控触发源（`models.catalogRefresh.enabled=false`、session 库已清瘦、maintenance 策略已钉 14d/30d/300/40mb），11-15 全天零复发（0 liveness heartbeat delayed）；但**机制本身未消除**——未来任何重维护（大批量清理、config reload、catalog 手动重开）仍可能在服务窗口撞出一次 30s 冻结。升级批核对 changelog / 上游 issue。

**残项三：skill-collection-review cron 连续失败（error 4x，每 7d）**。kidpal agent 的 skill workshop 背景作业，与语音主链无关、与本次延迟专项同批观察到的独立小项（2026-11-14 `openclaw cron list` 见 `skill-collection-review … error (4x)`，至今在册）。升级批顺带查其错误原因，或确认可安全禁用。

**风险**：riskTier: low——三条均为运维/升级批范围，不动契约与回合路由；升级动作本身按既有 runbook 与漂移闸纪律执行。