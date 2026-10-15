# device-round-record · 260921-m3-k3-voice-schedule-entry-live（M3K3-A3 真机确认轮）

> M3K3-A3 = live 真机确认轮（human 持板）。本记录如实分列三源证据与 human 目击，**按 human 实际确认范围写、不扩写**：human 口述确认项标「确认」，日志可证的标「有证据」，human 未逐项口述的标「未逐项口述确认」，缺口如实标注去向。M3K3-A3 最终裁定 = **human 目击设备轮 OK**（2026-09-21 晚）。
> 三源：① 串口被动捕获 `c:/tmp/k3_round1.log`（仓外证据，先例 = K2 device-round-record 不拷贝进仓、只引路径；30min 完整落盘 1873 行，覆盖设备 uptime ~22s 起）；② front systemd journalctl（ECS 8.137.91.16，`kidpal-front.service`，integrator 收口时实拉，窗口 20:00–23:10）；③ 引擎 docker logs（`xiaozhi-esp32-server` 容器，同窗口）。文中「摘录」= 逐字引日志行；关联时点 = 三源会话 id / 门数 / 文本逐字比对锚定。

## 时间线（2026-09-21 晚，三源关联；设备 uptime → 墙钟锚 = 会话 id e82735d7：串口 277.415s ↔ front/engine 22:10:25，推得设备 boot ≈ 22:05:54）

### 第一轮语音会话（22:10:25–22:20:21，串口 + front + engine 三源同证）

| 设备 uptime | 墙钟 | 事件 | 证据源 |
|---|---|---|---|
| 270.9s | 22:10:25 | 按键发起第 1 次建连 `WS: Connecting to websocket server: wss://8.137.91.16:8000 with version: 1`，Session `e82735d7-…` | 串口 L288/291；front `WebSocket / [accepted]` 22:10:25；engine 会话同 id |
| 277.5s | 22:10:32 | `MCP: display_dispatch view=home corr_id=kdp-277535 upcoming_apply ok`——**三触发① 建连即推首页「接下来」到端实证** | 串口 L302 |
| 287.0s | 22:10:42 | `mbedtls_ssl_fetch_input error=76` → WS 断开（断开前有按键事件；归因未定位，见缺口节） | 串口 L307-310 |
| 307.9s | 22:10:57 | 第 2 次建连（Session `fd9bcdc0-…`），309.1s `view=home … upcoming_apply ok`（触发① 复证） | 串口 L337/340/347；front 22:10:57 |
| 314.0s | 22:11:02 | `>> 明天有什么课？` → 327.5s `<< 明天是星期二呀，白天在学校有语文、思维训练、音乐、体育`（两段答复完整播报；engine 22:11:14 `route:text` ack + 22:11:15/16/18 两段 TTS 同文）——**S-F1.3 AC1 问答框答复实证（不切课表页，route:text）+ 日期准确（9-22 确为周二）** | 串口 L348-351；engine 22:11:02–22:11:18 |
| 349.2s | 22:11:31 | `>> 帮我显示课表` → 354.2s `MCP: display_dispatch view=schedule corr_id=kdp-354205 courses=37 degrade=false`（**自动切 P2 周网格**）→ 355.5s `<< 好呀，课表这就给你打开`（engine 22:11:43 TTS 同文）——**S-F1.3 AC2「帮我显示课表」意图链完整目击：意图 → front 截获展开 → view=schedule 下发 → 设备切页** | 串口 L365-370；engine 22:11:37/42/43 |
| 368.7s | 22:11:56 | error=76 断开（同上归因未定位） | 串口 L381-384 |
| 687.8s | 22:17:16 | 第 3 次建连（Session `362fbacf-…`），688.9s `view=home … upcoming_apply ok`（触发① 三度复证） | 串口 L758/761/768；front 22:17:16；engine 22:17:17 |
| 691.9s | 22:17:22 | `>> 明天有什么课？` → 707.3s + 713.3s 两段完整答复（「明天是周二……体育和英语……放学后六点半去文化宫上书法班，上到七点半。明天就这些安排啦」；engine 22:17:35/38 同文）——**AC4 备注内容读回的间接实证（书法班时间/地点进了口头答复）** | 串口 L769-782；engine 22:17:20–22:17:38 |
| 851.5s | 22:19:59 | `MCP: display_dispatch view=home corr_id=kdp-851535 upcoming_apply ok`，853.0s `<< 时间过得真快啊……明天学校见了`（farewell 播报；engine 22:19:51 收结束对话指令 → 22:19:59 route:home ack → 22:20:01 farewell TTS）——**归因更正：该次 home 显示与 farewell 轮同拍（v6 口径「孩子说再见 → view=home 空 data」），早于 QQ 录入的设备侧实证时点，不作三触发③证据引用**（详见触发③节） | 串口 L801-802；engine 22:19:51–22:20:01 |
| 867.1s | 22:20:21 | error=76 断开，设备回 idle；串口捕获至 ~22:36 结束，无新业务事件 | 串口 L821-824 |

### 第二轮语音会话（22:43:42–22:47，front + engine 两源；串口捕获已结束）

| 墙钟 | 事件 | 证据源 |
|---|---|---|
| 22:43:42 / 22:43:55 | 建连 ×2（front `WebSocket / [accepted]`；engine `使用快速提示词: # KidPal OpenClaw 基础 System Prompt — v6（v5 版 + 260...`——**v6 已在生产引擎生效**） | front + engine 22:43:42/55 |
| 22:44:01 | `识别文本: 帮我显示课表页。` → 22:44:08 设备回 `{"route":"schedule","courses":40}` → 22:44:09 TTS「好嘞，课表这就给你打开」——**第二次意图链目击；门数 37→40（22:11:42 courses=37 → 22:44:08 courses=40）= QQ 录入 → OpenClaw 改写 schedule.md → producer → front → 设备网格的全通路实证** | engine 22:44:01–22:44:09 |
| 22:46:42 / 22:47:25 | 连接超时关闭（引擎侧超时检查任务正常退出） | engine |

### front journalctl 补充（零点档行，integrator 收口实拉）

摘录（journalctl -u kidpal-front.service，逐字）：

```
Sep 21 22:00:34 iZ2vca04vpdbx8x9ohy0y2Z uvicorn[1642194]: 2026-09-21 22:00:34,735 kidpal-front schedule_push trigger=midnight corr=0b5cfdf6ab0049d6a371967c37e4f42b local=2026-09-21 sent=1
```

- **如实注记**：该行 = front 于 21:56:44（prompt v6 部署窗口，`front_app.py.bak-260921-k3-v6` 备份 21:54 在 ECS 在案）重启后、设备 22:00:34 建连拉起轮询、`last_push=None → daily_push_due=True` 的**重启补推**（K1 review-packet finding #1 已申报的行为），非真零点 tick。通路端到端（每日档 due 判定 → home_upcoming_payload → 中继下发 → 设备 applied）为真；真零点点位（当日 00:00）未在本轮取证。当日 journalctl 共 4 条 `trigger=midnight`（20:07 / 20:18 / 20:28 / 22:00:34，pid 各异），均为部署重启后的补推——「每 local 日恰一次」在重启窗口不成立，K1 finding #1 行为真机复现在案。
- trigger①（建连即推）在 front 侧为静默成功路径（无成功日志行，仅失败 warning），设备侧 `upcoming_apply ok` ×3 即到端实证，front WS accepted 行作建连锚。

## 决策 26 三触发逐条判定

| 触发 | 判定 | 证据与缺口 |
|---|---|---|
| ① 联网建连 | **确认** | 串口 3 次建连均 `view=home … upcoming_apply ok`（277.5s / 309.1s / 688.9s）+ front WS accepted（22:10:25 / 22:10:57 / 22:17:16 / 22:43:42 / 22:43:55）+ engine route:home acks 逐次同拍 |
| ② 每日零点 | **有证据（通路级）** | journalctl `trigger=midnight … sent=1`（22:00:34）在案；性质 = 重启补推而非真零点 tick（如实注记见上节），真零点点位未目击 |
| ③ 录入后即时 | **部分证据（机器日志缺位）** | journalctl `trigger=immediate` **全量回溯 0 命中**（journal 保留期内从未出现）；`/opt/kidpal/alerts/` 现无 `schedule.refresh` marker（已消费与未落笔不可区分）。QQ 录入的设备侧实证 = 门数 37→40 + human 目击三处即时变化；「录入 → marker → front 轮询恰一次消费 → 即时推」的 journalctl 环节本轮缺位。串口 851.5s 的 home 显示经三源关联为 farewell 轮 view=home 显示（v6 口径），**不**作 ③ 证据 |

## human 口头目击（2026-09-21 晚，如实转写、不扩写）

- 网格有课、切页正常。
- 答复播报正常且日期准（「明天是星期二」与 2026-09-22 周二相符）。
- QQ 录入后课表页 / 问课表 / 首页三处即时变化。
- 设备轮 OK。

## K2 deferred 真机半（M3K2-A7）确认范围——如实标注

- human 报「切页显示正常、网格有课」——此两项**确认**。
- ※（临时标记）/ ★（星标）/「明早」标签 / 14px 目测细节 human **未逐项口述确认**——如实标注「未逐项口述确认」，不写已确认。

## K1 deferred 真机半（M3K1-A5）随轮收口判定

- 触发①：本轮三源实证（见上），**闭环**。
- 触发②：journalctl 行在案 + 通路级实证（重启补推性质如实注记），**通路闭环、点位目击缺**。
- 触发③：human 目击 + 门数 37→40 设备侧实证；journalctl `trigger=immediate` 缺位（见触发③节），**机器日志半未闭环**——不写全闭环，缺项已在 review-packet 未覆盖面申报挂账。

## 已知缺口如实记录（均非 K3 义务）

1. **设备时钟错误**：串口全程 `W (…) Display: System time is not set, tm_year: 70`（OTA 响应缺 server_time，K1 热修轮已登记独立缺口）→ 今天列高亮落错列（本轮目击落周五列，实为周一 9-21）。
2. **WS 会话中段断开**：本轮三次会话分别在 +9s / +59s / +178s 后 `mbedtls_ssl_fetch_input error=76` 静默断开（与 K1 热修轮登记的 ~36s 掉线同族缺口；断开前均有按键事件，归因——NAT/keepalive vs 主动 abort——未定位，如实记录不复演）。设备均自动重连成功，未阻断本轮清单执行。
3. **首页 upcoming CJK 课程名画不出**：挂账④ → 已立项 K4 包 `260921-m3-k4-p1-cjk-dynamic-text`（Lite，分支 commit `40ce8c7`）。

## M3K3-A3 确认清单逐项对账（对齐冻结验收清单族）

| 清单族 | 判定 | 证据锚 |
|---|---|---|
| S-F1.2 网格/周总览 | 确认 | human 目击 + `view=schedule courses=37/40 degrade=false`（串口 354.2s；engine 22:11:42 / 22:44:08） |
| S-F1.2 今天列 | 缺口（非本包义务） | 设备时钟 tm_year 70 → 高亮落周五列（实为周一）；去向 = K1 热修轮 server_time 缺口 |
| S-F1.2 ※ 临时标记 | 未逐项口述确认 | human 未口述 ※ 细节；网格整体目击含临时安排在格（37 门含 QQ 录入增量），逐字形目击缺 |
| S-F1.2 星标 ★ | 未逐项口述确认 | 同上，human 未逐项口述 |
| NFR-READ-1 14px 目测 | 未逐项口述确认 | human 未口述字号观感 |
| S-F1.3 问答框答复（AC1） | 确认 | `route:text` ack + 两段答复完整播报、不切课表页（22:11:02 / 22:17:20 两轮） |
| S-F1.3 帮我显示课表切页（AC2） | 确认 | 意图链完整目击 ×2（22:11:37 courses=37 / 22:44:01 courses=40，均自动切 P2） |
| S-F1.3 播报声波（AC3） | 确认 | human 目击播报正常 + engine TTS 链（发送第一段语音/语音生成成功）+ 设备 speaking 态（voice_look scene=2） |
| S-F1.3 备注读回（AC4） | 有证据（间接） | 答复含书法班时间/地点备注内容（22:17:38「放学后六点半去文化宫上书法班，上到七点半」）；备注逐字读回未逐项口述 |
| S-F1.5 QQ 录入三处即时（AC1） | 确认（human 目击）+ 机器侧部分实证 | human 目击三处即时变化；门数 37→40 全通路实证；`trigger=immediate` journalctl 缺位如实注记 |
| S-F1.5 直改文件/跨天兜底（AC2） | 未在本轮专门目击 | human 未口述直改文件副通道测试；兜底语义由 K1 offline 齿（A4 模拟齿）承载，真机半缺 |
| 决策 26 触发①联网/建连 | 确认 | 见三触发表 |
| 决策 26 触发②零点/每日 | 有证据（通路级） | 见三触发表 |
| 决策 26 触发③录入后/即时 | 部分证据 | 见三触发表 |
| K1 deferred M3K1-A5 | ①②在案、③机器日志半未闭环 | 见 K1 deferred 节 |
| K2 deferred M3K2-A7 | 部分确认（切页/网格）+ 未逐项口述项 | 见 K2 deferred 节 |
| 三源日志·串口 | 在案 | c:/tmp/k3_round1.log（1873 行，22:06–22:36 被动捕获） |
| 三源日志·front/systemd | 在案 | ECS journalctl kidpal-front.service（20:00–23:10 实拉） |
| 三源日志·引擎 docker logs | 在案 | ECS docker logs xiaozhi-esp32-server（21:55–22:47 实拉） |

## 收口判定

- **M3K3-A3 = human 目击设备轮 OK（2026-09-21 晚）**——清单主项（S-F1.2 网格 / S-F1.3 AC1–AC3 / S-F1.5 AC1 human 半 / 三触发①②）确认或有证据；未逐项口述项与机器日志缺位项如实挂账（见对账表与疑点），不写全绿。
- S-F1.2 真机半（K2 deferred M3K2-A7）与三触发 live 半（K1 deferred M3K1-A5）随本轮**按上表程度收口**，缺项显式化，不静默。
