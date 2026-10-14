---
id: TECH-261014-kidpal-speak-await-nonetype
title: engine-patch 对同步 speak_txt 误 await——每轮提醒语音注入必报 ERROR「播报注入失败」但主链仍通（语音在报错后照常合成下发），2026-09-11 live 轮已测出、记「归后续修复」后未修未登记，本条补登
domain: tech
status: deferred
severity: SHOULD
owner: dev 批（server/deploy 引擎部署面）
trigger: 下一次动 server/deploy 引擎部署面的批顺带（与 TECH-261012-edgetts-403-hotpatch 耐久化同批的先例模式）；修复一行级，不值得单开包
opened: 2026-10-14
closed: ""
closedBy: ""
paths: ["server/deploy/engine-patch/connection.py"]
source: 2026-10-14 ECS 引擎容器 24h 日志三轮提醒 ERROR 逐字（20:15 / 20:30 / 07:45）+ 260911-ecs-deploy-convergence live 轮原始发现（delivery/taskpackages/260911-ecs-deploy-convergence/live-findings-and-closeout-notes.md）+ 代码锚 connection.py:383-390
handoff: 下一个动 server/deploy 的 dev 批；复验与 TECH-261014-post-speak-listening-lingering 的提醒复验轮天然同轮
---

**现象**：每轮提醒到点，引擎日志必出一条 ERROR——`kidpal_speak 播报注入失败: object NoneType can't be used in 'await' expression (提醒降级：弹窗+提示音仍必达)`。2026-10-14 检查 ECS 引擎容器 24h 日志，三轮提醒（261013 20:15 / 20:30、261014 07:45）逐条命中，无一例外。

**根因（代码锚 + 日志时序双证）**：[connection.py:383-390](../../../server/deploy/engine-patch/connection.py) `_kidpal_try_speak` 内 `await speak_txt(self, text.strip())`——`speak_txt`（引擎原生 `core/handle/intentHandler.py`）是**同步函数**：调用先行完成（语音发送任务已排队）、返回 None，随后对 None 求 await 即抛 TypeError。日志时序为证（07:45 轮）：ERROR 在 07:45:00，`发送第一段语音` 在 07:45:01、「语音生成成功」在 07:45:02——报错发生在语音照常下发**之前**，主链不受影响。260911-ecs-deploy-convergence live 轮已定性「报了错但主链仍通（两段音频真实送出），属真 bug，归后续修复」——其后无批承接、无条目登记，本条补登闭账。

**为什么要修（不只是日志洁癖）**：ERROR 文案自称「播报注入失败」与实际「注入成功、语音已下发」相反——2026-10-14 诊断「提醒没播报」目击报告时，该行第一时间把归因带偏到服务端（实际引擎侧三轮均正常下发）；不修则每次运维对账提醒链路都会被它误导一次。

**修法**：`await speak_txt(...)` 改为同步调用 `speak_txt(...)`（一行级；或按返回值是否 coroutine 判别以兼容引擎未来改异步）。注意保留外层 try/except 降级语义（V20「弹窗+提示音必达，语音为可选增强」）不变。

**复验**：提醒轮引擎日志无该 ERROR 且语音仍出（`语音生成成功`/`发送第一段语音` 照常）——与 TECH-261014-post-speak-listening-lingering 的提醒复验轮天然同轮，顺带闭账。

**风险**：riskTier: low——一行改动、降级路径不动、ECS 侧重启容器即生效；Track B 三要件齐（执行测出三轮逐字 / 修法明确无分叉 / 爆炸半径 = 引擎日志噪音与诊断误导，已揭示）。
