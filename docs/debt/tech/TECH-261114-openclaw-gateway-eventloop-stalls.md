---
id: TECH-261114-openclaw-gateway-eventloop-stalls
title: OpenClaw 网关事件循环 ~31s 级阻塞（两个实证触发源：kidpal session store 同步写+cleanup 失败循环、remote model catalog 刷新）——真机语音轮最差 33s 无响应
domain: tech
status: deferred
severity: MAJOR
owner: 主线程（OpenClaw 网关运维面）
trigger: 下次 OpenClaw 运维/升级批；或 journalctl 再现 liveness heartbeat delayed / slow SQLite transaction hold；或 NFR-PERF-4 延迟定阈批
opened: 2026-11-14
closed: ""
closedBy: ""
paths: []
source: 2026-11-14 延迟专项分析（当日 K2 复验真机轮三源日志）——journalctl -u openclaw-gateway、/tmp/openclaw/openclaw-2026-11-14.log（JSON 明细）、引擎 docker logs、设备 KPLOG029
handoff: 与 TECH-261022-openclaw-prep-tax-memory-pressure 同批处置（OpenClaw 升级 / ECS 运维面）；NFR-PERF-4 定阈时作为最差情形来源引用
---

**证据（2026-11-14，当日两次 ~31s 事件循环阻塞，7 天内仅此日出现）**：

1. **触发源甲：session store 同步写 + maintenance cleanup 失败循环（14:23 轮，真机受损）**。设备 14:23:14 连接（新 session），引擎 14:23:21/23 送两轮 ASR 文本（一句话被切成「今天收拾。」「碗筷加三分。」），网关文件日志逐字：`slow SQLite transaction hold`（`async:false, database:/root/.openclaw/agents/kidpal/agent/openclaw-agent.sqlite, operation:agent.write`）elapsedMs=**1383/4803/1368**；`slow SQLite session write` elapsedMs=**1414/5214**；`SQLite session write failed` ×2；`SQLite session maintenance cleanup failed` ×3（三批共 **155 个 session id** 列删未遂）。`[diagnostic] liveness heartbeat delayed: overdue=1438ms elapsed=31438ms`——事件循环 14:23:14→14:23:45 整体阻塞。该轮设备端「识别文本→发送第一段语音」= **31s**（14:23:23→14:23:54），孩子体感说完话 33s 无响应。store 现状：kidpal agent sqlite **17.8MB**、main agent sqlite **92.7MB**、共享 state sqlite **63MB**；churn 来源 = OpenClaw HTTP chatCompletions 每请求新建 session（2026-09-24 perf-k1 实证 510→511），cleanup 失败导致只增不减。
2. **触发源乙：remote model catalog 刷新（11:01，空闲时段未伤及真机轮）**。`[gateway] remote model catalog updated; restart the Gateway to apply it`（`providers:47, models:1350`）与 `liveness heartbeat delayed: elapsed=31690ms` 同秒落盘（11:01:13.845/.857），刷新窗口内事件循环阻塞 ~31.7s。该刷新 **每 6 小时一次**（05:01/11:01/17:00/23:00，7 天 journalctl 计 25 次）——每次都是一个 30s 级阻塞窗口，撞上语音轮即复现 14:23 形态。
3. **背景放大因子（同日在档）**：honcho-deriver 仍在消化积压（`PERFORMANCE summary_kidpal_5052 | SHORT_summary_creation=57017ms`，48h 内 UpstreamLLMError timeout ×4），与网关共置 2C/3.5GB ECS；`slow SQLite transaction hold` 7 天计 9 次。

**影响面**：正常轮「识别→首段语音」5~6s（当日多数轮实测），但阻塞窗口内的轮次直接 +30s 级；阻塞期间 qqbot、提醒 cron、整理回合全部排队（单进程单循环）。NFR-PERF-4「首轮延迟偏大」观察（261104-k2 复验轮）的最差情形来源即此。

**建议改动（候选，交 review 不自批）**：① kidpal/main agent session store 归档清理（`openclaw sessions` archive/cleanup 协议，2026-09-24 perf-k1 步⑦先例），清后观察 slow write/cleanup failed 是否消失；② 查 OpenClaw 配置能否禁用或降频 remote model catalog 刷新（1350 models 对单机 overkill），不能则并入升级评估（2.0.4 可升，需过 runbook §0 漂移闸）；③ cleanup 失败循环本身疑为 OpenClaw 缺陷，升级批内查 changelog/上游 issue；④ deriver 积压确认是否 K4-E6 backfill 预期在途，异常则限速。

**风险**：riskTier: low——①②为运维面软操作（有 backup create 先例可回滚），③④为调查项；均不动契约与回合路由。

**补充核实（2026-11-14 晚，session 机制专项；append 不改上文）**：

- **机制确认（官方文档 + 本机库实证）**：OpenClaw 的 openai-compat HTTP 端点按请求里的 `user` 字段派生 session key——**不带 `user` → 每请求新建匿名会话**（key 形如 `agent:kidpal:openai:<uuid>`）；**带 `user` → 稳定会话键、跨请求复用**。
- **我方主路径已经带 user**：261024 批起 engine patch 送 user=设备 MAC（connection.py 补丁探针核 `llm.response` 命名参数；当日 lane key `session:agent:kidpal:openai-user:58:e6:c5:72:0c:bc` 实证），front `_openclaw_chat_once` 同样 user 透传（`server/gateway/provider.py:99` `openclaw_stream_request(messages, user=None)`）。kidpal 库现况：`openai-user` 稳定会话仅 **1** 个；177 个会话中 ~160 个为 261024 前时代的历史匿名 uuid 会话、7 个 diag-probe（10-25）。main 库同构（qqbot 3 + cron/reminder 2 + 大量匿名）。
- **结论收窄**：「每请求新建 session」的 churn 属历史积累与不带 user 的调用方；live 语音路径**已不再新增匿名会话**。残留问题 = **自动 maintenance 对历史匿名会话持续清理失败**（当日三批 155 个 id 列删未遂、error 空对象 `{}`），死重拖慢同步写，放大 31s 阻塞。
- **处置抓手核实（2026.8.1 本机 CLI 实测）**：`openclaw sessions` 具 `archive` / `cleanup`（Run session-store maintenance now，`--enforce` 立即执行上限）/ `compact` 子命令；`session.maintenance` 配置路径有效但**未设置**（运行默认值）。修复序：①手动 `sessions cleanup --enforce`（仍失败 → 升 2.0.4 或报上游，参 openclaw#57177）；②显式设 retention/preserveRecent；③清后 doctor `--session-sqlite compact`（WAL checkpoint + VACUUM）真实缩文件——旧库 auto_vacuum=NONE，删行不还页。
- **来源**（直连被本机网络策略拦，经检索摘要 + 本机 CLI/DB 交叉核验）：docs.openclaw.ai/gateway/openai-http-api · docs.openclaw.ai/reference/session-management-compaction/maintenance · docs.openclaw.ai/cli/sessions · docs.openclaw.ai/cli/doctor/sqlite-maintenance · github.com/openclaw/openclaw/issues/57177。

**P0① 执行记录（2026-11-14 17:14~17:25 CST，human 授权「先手动清」，append）**：

- **dry-run 揭穿「cleanup 失败」表象**：默认策略下 `sessions cleanup --all-agents --dry-run` = **177 kept / 0 pruned**——`session.maintenance` 未设置时 retention 无界，自动维护从不真删；当日 14:23 日志里的「cleanup failed + 155 id 批」是处理列表空转+写路径劣化，不是删除动作失败。
- **执行序**：file-level cp 回滚锚（`/root/kidpal-agent.sqlite.bak-261114-pre-cleanup` 17.8MB、`/root/main-agent.sqlite.bak-261114-pre-cleanup` 92.7MB）→ `config set session.maintenance`（先 warn 试跑再翻 enforce；最终持久值 `{mode:enforce, pruneAfter:"14d", preserveRecent:"7d", maxEntries:300}`，热加载免重启）→ `sessions cleanup --all-agents --enforce`（gateway 日志 `sessions.cleanup 12557ms ✓`）→ `--agent main --enforce --fix-missing`。注：`openclaw backup create` 本次**失败**（`timed out waiting for legacy audit migration lease`，瞬态租约竞争，`state_leases` 表核查为空）——回滚锚以文件级 cp 为准。
- **结果**：kidpal session_nodes **177→1**（保留的正是设备稳定会话 `agent:kidpal:openai-user:58:e6:c5:72:0c:bc`），transcript_events 1588→340；main **71→67**。doctor `--session-sqlite inspect --session-sqlite-all-agents` = **0 issue**；清理后网关 error/failed/slow 计数 **0**。
- **残留（未清）**：① main 库 ~60 个 2026-09-24 时代匿名 `openai:<uuid>` 僵尸行对内置维护**全免疫**（created_at=null；pruneAfter、maxEntries=10 预算驱逐 dry-run 均跳过；`--fix-missing` 只多收 1 个）——main 已不在儿童语音路径（qqbot/cron 用），属惰性死重，处置候选 = 升级 2.0.4 或 `doctor --session-sqlite recover --github-issue` 报上游；② **sqlite 文件未缩**（kidpal 17.8MB 仅 1 会话——删行不还页），真实收缩需 `doctor --session-sqlite compact`（offline 模式，须停 gateway = 短暂停机，待 human 定窗口）；③ 触发源乙（catalog 刷新阻塞）本次未动，仍开放。
- **待验证**：下一次真机语音轮首轮无 31s 卡顿、`slow SQLite transaction hold` 不再出现（复测锚 = 本条目当日基线）。

**P0②③ 执行记录（2026-11-14 18:12~18:50 CST，human 授权「三件残留的现在就可以处理，停 gateway 也没问题」，append）**：

- **第三次 31.5s 阻塞（新证据，触发源丙）**：18:12 对 session.maintenance（带 maxDiskBytes 预算）+ models.catalogRefresh 做 config 热加载，reload 评估同步卡循环 `elapsed=31524ms`。当日三次阻塞 31438/31690/31524ms 高度一致——疑 OpenClaw 内部 30s 同步维护预算；重维护评估（catalog 刷新 / session 维护 / config reload）撞上事件循环即 30s 级卡顿。
- **main/sessions 58MB 真身**：5322 个 `*.jsonl.deleted.<ts>.<hash>.zst` 墓碑归档（9/27~10/30，261024 持久会话补丁前的匿名会话遗骸）——`resetArchiveRetention` 未设时默认「保留到 10gb 磁盘预算驱逐」= 实际永不删。63 个 main 僵尸**行**即 2026-09-24 perf-k1 步⑦ archive 的会话（archived_at 已设，transcript 文件已失，故 pruneAfter/maxEntries/fix-missing 全不适用——archived 行受常规驱逐豁免属设计行为）。
- **已执行**：①`models.catalogRefresh.enabled=false`（触发源乙关断，重部署需重放——声明批已回流）；②`session.maintenance` 持久落 `{mode:enforce, pruneAfter:"14d", preserveRecent:"30d", maxEntries:300, maxDiskBytes:"40mb", highWaterBytes:"30mb", resetArchiveRetention:"14d"}`（preserveRecent=30d 护 qqbot/cron 会话不被预算误伤）；③停机窗口 `doctor --session-sqlite compact --session-sqlite-all-agents`（main 91.0→89.1MB、kidpal 13.3→13.1MB——僵尸行属活数据，真缩需行删除后二次 compact，收益评估后暂缓）；④墓碑归档驱逐由内建维护限速执行中（~1-3 文件/s，5322→4301 @18:47，ETA 半小时级，探针实证驱逐期网关可服务）。
- **重启核验全绿**：qqbot dad/mom READY、Honcho ready、18789 锁 loopback（V28）、cron kidpal-reminder-plan ok、loopback 探针 HTTP 200 / 5.0s / 小伴人设正常（认得孩子名字，Honcho 注入工作）。附带观察：cron `skill-collection-review` error(4x) 在册未处置（与本条无关）；`openclaw backup create` 因 legacy audit migration lease 瞬态超时失败一次（state_leases 表空，未复现）。
- **回流记账（human 指示「openclaw 侧配置记录在仓里」）**：声明批 `server/deploy/261114-openclaw-session-maintenance/session-maintenance-declaration.json` 已落仓——含两键目标值、drift_guard、当日 live 人工应用记录、重部署重放顺序；**如实记：主线程直写 server/deploy 实现面，跳过 dev-loop 的一次性记账操作**，正式双形态 apply/rollback 脚本 + acceptance 归下个部署批按 261025 骨架补齐，补齐前 deploy.sh 不得把本批当可执行批消费。
- **残留待办**：①驱逐完成后复核 main/sessions < 预算 + doctor inspect；②63 个 archived 僵尸行留待升级 2.0.4 批（或上游 issue）处置，行本体为惰性元数据；③23:00 整点核验 `remote model catalog updated` 日志行绝迹（catalogRefresh=false 生效判据）。
- **P0③ deriver 核查收口（18:40 CST）**：无积压、无需限速——honcho-redis keyspace 空（零待处理键）、deriver 自 07:05:54 后 11h 零处理零错误、CPU 0%；今晨 57s 摘要（summary_kidpal_5052）是 280 消息大会话的偶发单条摘要，非持续 backfill；记忆册所记 K4-E6 全量 backfill 运维轮仍属「未启动」而非「在途」。deriver 大摘要时占上游配额与单核，但在独立容器内，不直接阻塞网关事件循环。
