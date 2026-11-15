# KidPal 系统延迟专项分析（2026-11-14）

> 主线程延迟排查轮产出。证据 = 当日 K2 复验真机轮（14:23~15:05）四源日志（设备 KPLOG029 / front journalctl / 引擎 docker logs / OpenClaw gateway journalctl+文件日志）+ 历史专项与债务账（260924-perf-k1、TECH-261022 prep 税、NFR-PERF-4 观察）。本文是分析记录，不是契约面；优化项落地各自走既有 loop。

## 1. 结论先行

1. **正常轮的端到端延迟已从 9 月基线（真机 13.6~17.2s）降到 5~6s**（当日多数轮实测「识别文本→发送第一段语音」5.0~6.0s），perf-k1 的 P0 重启 + P1 thinking-off 收益仍在。
2. **但「延迟大」的体感来自三类最差情形**，都不是常态轮：
   - **网关事件循环 ~31s 级阻塞**（当日 2 次，真机 1 次受损：14:23 轮 31s 无响应）——新登记 [TECH-261114-openclaw-gateway-eventloop-stalls](../debt/tech/TECH-261114-openclaw-gateway-eventloop-stalls.md)；
   - **一句话被 VAD/ASR 切成两段 → 两轮串行**（同轮实证，第二轮 lane wait 4.5s，总时延翻倍）；
   - **每次按键开会话要重建 WS 通道**（按键→可说话 1.57s，含 TLS 握手 0.53s；会话结束即断连）。
3. **每轮固定成本仍偏高但在可接受区**：OpenClaw agent run 2.5~4s（1~2 次 deepseek 调用，上游 API 本身只要 150~551ms 响应头）+ EdgeTTS 首句 1~2s + 链路胶水 ~0.5s。上下文已从 9 月 ~25k tokens 瘦到首轮 ~7.8k（M5 批落地），但 kidpal workspace AGENTS.md 又长回 20.6KB，Honcho 每轮再注入 ~9KB。
4. **主机面已不是主瓶颈**：ECS 已升 3.5GB RAM、load 0.15、swap 474MB（可控）、网关 RSS 377MB；但 honcho-deriver 仍在消化积压（单条 summary 57s + 上游超时），与网关共置抢资源。

## 2. 当日实测解剖（2026-11-14 下午真机轮）

### 2.1 正常轮分段（15:04:37「我现在一共有多少分？」）

| 段 | 实测 | 依据 |
|---|---|---|
| ASR 收尾→引擎收到文本 | ≤0.1s | Qwen3ASRFlash 流式，15:04:37 识别文本与大模型收到同秒 |
| OpenClaw agent run | ~2.7s | honcho-diag 15:04:38.395 → 两次 deepseek POST（250/231ms 响应头）→ run ended 15:04:41.084 |
| 引擎 TTS 首句（EdgeTTS） | ~1s | 发送第一段语音 15:04:42 → 语音生成成功 15:04:43 |
| 网络+设备解码起播 | ~0.5s | 设备 `<<` 首句 15:04:42（uptime 64512ms） |
| **设备体感（STT 上屏→首句播出）** | **5.15s** | KPLOG029 uptime 59362→64512ms |

同形态轮当日共 4 次：引擎侧 5.0/5.0/6.0/5.0s（14:26:06、14:26:30、14:27:31、15:04:37 起）。

### 2.2 最差轮解剖（14:23，31s）

时间线：14:23:14 设备连接（新 session）→ 14:23:21/23 一句话被切成两段 ASR 文本分两轮送入 → 网关自 ~14:23:14 起事件循环阻塞（`liveness heartbeat delayed elapsed=31438ms`），期间 kidpal agent sqlite 同步写 1.4~5.2s/次 ×3、session 写失败 ×2、maintenance cleanup 失败 ×3（155 个积压 session 分批列删未遂）→ 14:23:50 循环恢复，第一轮才开始跑（deepseek 551ms 即回）→ 14:23:53 第二轮 lane wait 4462ms → 14:23:54/56 两段首句语音。**孩子体感：说完话 33s 才听到回应。**

第二次阻塞在 11:01:13（elapsed=31690ms），与 `remote model catalog updated`（47 providers/1350 models）同秒结束——catalog 刷新每 6 小时一次（7 天 25 次），每次都是潜在 30s 级阻塞窗口，撞上语音轮即复现。

### 2.3 会话建立段（每次按键说话的固定前置）

KPLOG029：按键 key_route_dispatch(session_toggle) → WS Connecting → TLS 证书验证 0.53s → Session ID → listening，合计 **1.57s**（uptime 53012→54582ms）。会话结束（overlay_close）即断 WS 并回 WiFi LOW_POWER 省电模式，下次说话全部重来。与既有债务族 TECH-261014-wakeword-connecting-voice-layer / session-end-channels-unwired / autoconnect-gives-up-silently 同根。

## 3. 延迟构成总表（现状 + 历史锚）

| 层 | 现状 | 备注/历史锚 |
|---|---|---|
| ASR（Qwen3ASRFlash 流式） | ≤0.1s 收尾 | 9 月即证非瓶颈 |
| OpenClaw agent run（kidpal agent） | 2.5~4s | 上游 deepseek API 本身 150~551ms 响应头；大头在网关内编排 + 每轮上下文 prefill（~7.8k tokens 首轮）+ Honcho 注入 ~9KB + 工具轮多一次调用 |
| 流式形态 | ttfc==total | OpenClaw agent run 完成后才一次性吐 content deltas（2026-09-24 实证 28 样本；TECH-261018 再证 deepseek-flash id 单块形态）——引擎拿不到可提前 TTS 的半句 |
| TTS（EdgeTTS 首句） | 1~2s | 另有 403 DRM 单点风险两条债务在账（TECH-261012/261023） |
| front | ~ms 级 | 纯代理+回合路由，当日无异常 |
| 设备会话建立 | 1.57s/次 | 每按键一次付一次 |
| embedded run prep 税（整理/引导回合） | 10 月实测 7.05s（bootstrap-context 6.5s） | 4GB 升级后**未复测**（K3-E7 待办仍挂） |
| 网关事件循环阻塞 | 0~31s（当日 2 次） | 见 §2.2；7 天内仅当日出现 |
| 多轮工具路径（天气 web 搜索） | 9 月 15~28s | 现 {{weather_info}} 已机械注入 prompt（261012 real-time facts），常态天气题应不再走 web 工具轮；未复测 |

## 4. 优化空间（按优先级）

### P0 纯运维（低风险，可先行）
1. **session store 治理**：kidpal agent sqlite 17.8MB / main agent 92.7MB，cleanup 失败循环使积压只增不减——按 perf-k1 步⑦先例做 archive/cleanup（先 `openclaw backup create`），清后观察 slow write 是否消失。**直接针对 31s 阻塞触发源甲。**
2. **remote catalog 刷新降频/禁用**：查 OpenClaw 配置面能否关（1350 models 无用武之地），不能则并入升级评估。**针对触发源乙。**
3. **deriver 积压核查**：确认 5052+ 条 summary 消化是否 K4-E6 backfill 预期在途；若异常（57s/条 + 超时刷屏）则限速或暂停，还 ECS 安静。

### P1 小改（配置/小批实现）
4. **切句合并**：一句话被切成两轮是 31s 轮的放大因子，也独立造成 +5s 级串行等待。方向：引擎 VAD 静音窗调整 / 短间隔连续 ASR 段合并再送 LLM（需过 /triage-change 分流）。
5. **会话通道保持**：会话结束不即断 WS（或懒断+快速重连），省每次按键 1.5~2s——与 TECH-261014 债务族同批处置。
6. **kidpal workspace AGENTS.md 瘦身复查**：20.6KB（9 月瘦身后为 7.9KB，又长回去了），每轮 prefill 大头；顺带复查 Honcho 注入 ~9KB 的信噪比。

### P2 结构性（需立项）
7. **流式贯通（ttfc→真首句）**：调研 OpenClaw 流式出口（agent loop 缓冲点 / blockStreamingChunk sentence 模式），若能让引擎边收边 TTS，体感可再省 1~3s。9 月 k2 结构性调研项，至今未做。
8. **TTS 首句提速/备份**：EdgeTTS 首句 1~2s + DRM 403 单点（两条债务），可评估备选 TTS 或首句预合成。
9. **NFR-PERF-4 定阈**：本次分析给出常态 5~6s、最差 31s+ 的分布；建议按 perf-k1 方法学（3 题 × N≥3 中位数 + 三源时间戳）复测后钉阈值进需求面，让延迟有验收判据。
10. **OpenClaw 2.0.4 升级**：过 runbook §0 漂移闸后评估（cleanup 失败循环与 catalog 阻塞可能已有上游修复）。

## 5. 测量缺口（需要真机轮补的）

- embedded run prep 税在 4GB RAM 下的现值（K3-E7 待办）。
- 常态天气题现值（{{weather_info}} 注入后是否已不走 web 工具轮）。
- P0 运维动作后的对照复测（session 清理前后 slow write 计数、31s 阻塞是否绝迹）。
- 协议沿用 perf-k1：human 就绪信号开窗、单一长捕获、3 题（闲聊/课表/天气）× 每题 N≥3、串口+引擎+网关三源对齐；单次数字不作门证据。

## 6. 证据索引

- 当日引擎轮次：`docker logs xiaozhi-esp32-server`（14:23~15:05 识别文本/大模型收到/发送第一段语音/语音生成成功 四缝）。
- 网关阻塞明细：`/tmp/openclaw/openclaw-2026-11-14.log` 行 372~383（slow SQLite/session write failed/cleanup failed/heartbeat delayed JSON 原件）、行 277~278（catalog 刷新与第二次阻塞同秒）。
- 设备侧：KPLOG029 tail（本仓外 `c:/tmp/kplog-tail-261114.log`，含儿童语音转写，不入仓）。
- 历史锚：`delivery/taskpackages/260924-perf-k1-roundtrip-latency/evidence/latency-baseline-260924.md`（9 月基线与 P0/P1 全程）、`docs/debt/tech/TECH-261022-openclaw-prep-tax-memory-pressure.md`（prep 税）、`delivery/taskpackages/261104-k2-points-device-ui/review-packet.md`（NFR-PERF-4 观察）、`delivery/workstreams/device-memory-overhaul/progress.md:88`（上下文 22116→7761 tokens）。
- 现值快照（2026-11-14 16:23 CST）：ECS 3563MB RAM/swap 474MB/load 0.15；gateway RSS 377MB（15:02:51 重启后）；thinkingDefault=off、model=deepseek/deepseek-v4-flash 维持 P1 形态；honcho 5 容器 Up 2-3 weeks。

## 7. 补充核实：OpenClaw session 机制（2026-11-14 晚，human 质询触发联网+本机库核查）

- **机制**：openai-compat HTTP 端点按请求 `user` 字段派生 session——不带 user → 每请求新建匿名会话（`agent:kidpal:openai:<uuid>`）；带 user → 稳定会话键跨请求复用（官方文档 docs.openclaw.ai/gateway/openai-http-api，直连被网络策略拦，经检索摘要+本机实证交叉核验）。
- **现况**：主路径已带 user（261024 批：engine patch 送设备 MAC、front `_openclaw_chat_once` user 透传）。kidpal 库 177 会话 = 1 个 openai-user 稳定会话 + ~160 个历史匿名 + 7 diag-probe；main 库同构。**live 语音轮已不再新增匿名会话**——「每次一个 session」是 261024 前的历史积累。
- **残留问题**：自动 maintenance 清理历史匿名会话持续失败（当日三批 155 id、error 空对象），死重拖慢同步写 → 31s 阻塞放大因子。
- **抓手**（2026.8.1 CLI 实测在册）：`openclaw sessions cleanup --enforce` / `archive` / `compact`；`session.maintenance` 配置有效但未设置（运行默认）；清后 doctor `--session-sqlite compact`（VACUUM）真实缩文件。cleanup 仍失败 → 升级 2.0.4 或报上游（openclaw#57177）。

## 8. 当晚运维执行（2026-11-14 17:14~18:50 CST，human 授权）

P0 三项当晚执行完毕（过程与读数全量记在 [TECH-261114 债务条目](../debt/tech/TECH-261114-openclaw-gateway-eventloop-stalls.md) 的两段执行记录，此处只记结论）：kidpal 会话库 177→1；`models.catalogRefresh.enabled=false` 关断触发源乙；`session.maintenance` 七个键持久落地（enforce/14d/30d/300/40mb/30mb/14d）；离线 compact 一次；5322 个墓碑归档（58MB）由内建维护限速驱逐中。执行期间抓到**第三次 31.5s 阻塞**（config 热加载的预算评估同步卡循环）——当日三次 31438/31690/31524ms 高度一致，坐实「重维护评估撞事件循环 = 30s 级冻结」的内部时box形态。**live 配置改动已回流仓内声明批** `server/deploy/261114-openclaw-session-maintenance/`（重部署重放清单在声明 `_note` 里；该批为主线程直写实现面的记账批，正式 apply/rollback 脚本归下个部署批补齐）。
