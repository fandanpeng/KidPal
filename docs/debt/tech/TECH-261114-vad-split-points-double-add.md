---
id: TECH-261114-vad-split-points-double-add
title: 引擎 Silero VAD 缺省 1000ms 切句致句中停顿 >1s 分两轮——23 天基线真切句 n=2、全部命中积分句式（事由…停顿…加X分）；11-14 叠加网关阻塞时单事双记账（+6 分）。样本不足先观察，不立包
domain: tech
status: deferred
severity: SHOULD
owner: 主线程（观察采集）→ dev 批（判据命中后立包）
trigger: 观察判据命中（再累计 ≥2 次真切句事件，或双记账再发生一次 → 升 MAJOR 并走 /triage-change 立包）；或 NFR-PERF-4 延迟定阈批 / 下一个引擎配置面包顺带重估
opened: 2026-11-14
closed: ""
closedBy: ""
paths: ["server/deploy/provider.config.yaml"]
source: 2026-11-14 延迟专项 P1-④ 调查——引擎容器源码（core/providers/vad/silero.py）+ 引擎 docker 日志 8 天全量扫描（201 条识别文本终句）+ /opt/kidpal/points/state.json 账本互证
handoff: 立包时候选修法三件见正文；观察采集零改动、走既有日志通道（kidpal-ecs-gateway skill）
---

**机制定位（引擎容器 0.9.6 源码实读）**：`core/providers/vad/silero.py` 读三个配置键——`threshold`（人声概率阈，默认 **0.5，是概率不是秒数，易误读**）、`threshold_low`（静音滞回，默认 0.2）、`min_silence_duration_ms`（**切句静音时长，默认 1000ms**）。live `.config.yaml`（渲染源 = `server/deploy/provider.config.yaml`）**无 VAD 节 = 全跑默认**。孩子句中停顿 >1s 即被切成两段 ASR 终句、引擎分两轮送 LLM，OpenClaw 同 lane 串行（第二轮实测 lane wait 4462ms）。

**基线扫描（2026-10-23 → 11-14，201 条终句）**：相邻 ≤4s 的配对 68 对，逐条定性后**真「一句切两段」仅 2 次**，其余为录音会话连续叙述（10-28 / 11-11，分段进素材属设计行为）与连发独立问题（11-14 15:04）。两次真切句**全部命中积分句式**：

| 时间 | 切片 | gap | 后果 |
|---|---|---|---|
| 11-06 09:12 | 「我今天帮妈妈收玩具。」+「加三分。」 | 1s | **未双记**——碎片 1 缺分值被「不完整句不落账」纪律兜住，碎片 2 合历史单记一笔（账本 01:12:50 UTC 恰一条 +3） |
| 11-14 14:23 | 「今天收拾。」+「碗筷加三分。」 | 2s | **双记账 +6 分**（账本 06:23:54 + 06:23:56 两条 +3，reason「帮妈妈收拾碗筷」/「今天收拾碗筷」）；front points_dedup 按 reason 文本判重、两文案不同未拦 |

**双记账条件假设（待更多样本证实）**：11-14 双记叠加了当日 31s 网关事件循环阻塞（TECH-261114-openclaw-gateway-eventloop-stalls）——两轮被压进同一执行窗、各自带合并后语境发出 add_points；11-06 常态时序下纪律兜住未双记。即「切句 → 双记」可能需要同窗巧合，非必然。

**观察方案（零改动）**：定期扫引擎日志，判据 =「相邻两条识别文本 ≤4s 且两碎片合成同一意图（人工定性）」→ 命中即在本条目 append 一行（时间 / 切片 / gap / 账本后果）；顺带核对 `/opt/kidpal/points/state.json` 短窗同 delta 双分录。频率基线 = 本条 source 的 23 天 n=2。

**修法候选（立包时裁，不自批）**：A 配置面 `min_silence_duration_ms` 1000→1300/1400（部署管线现成、每轮收尾 +0.3~0.4s 全员税）；B engine-patch 短句合并窗口（碎片短且 2s 内续段则合并再送 LLM，零税但代码面，参照 display-strip 补丁先例）；C front points dedup 增「短窗同 delta」维度兜底双记账（误杀真·连续同分记录的取舍需需求面裁）。A 为第一刀候选，B 为 A 不够时的二期，C 独立可并行。

**风险**：riskTier: low——观察本身零改动；候选修法各自带回归面（A 全员延迟税 / B 引擎补丁面 / C 记账语义），立包时按 triage 判档配真机回归。
