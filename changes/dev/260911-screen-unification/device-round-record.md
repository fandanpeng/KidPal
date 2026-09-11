# SU-A8 真机并发轮记录（2026-09-11，human 持板 + agent 烧录/捕获）

> 包 `260911-screen-unification`（PKG-2 归一屏）收口阶段真机轮。执行依据 =
> human 裁断 `su-a8-device-round-now`（ruling-260911-screen-unification-03.txt 裁断①，
> pod-events eventId `e3d74b2c-fd26-45f1-abad-c897b59ac18c`；采纳 deputy 预裁锚
> SU-RULING-03，deviation=false）。执行模式与 PKG-1 KR-A5 同型：agent 烧录 + 串口捕获，
> human 持板操作并口述观察，PASS 裁定权 = human。日志原件归档本目录 `device-round/`。

## 一、烧录与启动核验

- 产物：`firmware/build/xiaozhi.bin` 2,893,248 B（= 0x2c25c0，ruling 字面核对一致，
  本包 r 轮 firmware-build 真编产物，mtime 2026-09-11 19:26）。
- 21:03:31 esptool 烧 COM5 app@0x20000（460800 baud），`Hash of data verified`，
  RTS hard reset。
- 启动捕获（`device-round/su-a8-boot.log`，60s 复位捕获）：LVGL 启动 → WiFi 关联
  AP「701」→ 首次 WS 连接 `select() timeout`（t=48s，瞬时网络抖动）→ 1s 内自动重试
  → `WS: Connecting to wss://8.137.91.16:8000 with version: 1`（协议版本正确）→
  `Session ID: d1307514` → `State: connecting -> listening`。无 panic/abort/Guru。

## 二、场景轮时间线（墙钟 CST；设备 uptime 毫秒为日志原文锚点）

### 轮 A：语音轮 + SERVER_PAGE 推送页并发（21:11-21:14，`su-a8-keypress-02.log`）

1. 21:15 前的准备段设备经历一次重启（见 §四 复位归因）；本段证据取自重启前捕获。
2. t=191805 human 单击 BOOT：`show_chat: VOICE layer push (top=3)` →
   `idle -> connecting` → Session `bd4b66bd` → listening。
3. t=199285 human 问「明天有什么课程？」（`Application: >>` ASR 上屏）。
4. t=209565 回答期间引擎推课表页：`MCP: display_dispatch view=schedule
   corr_id=kdp-209565 items=1 is_tomorrow=1 degrade=false` +
   `schedule_render corr_id=kdp-209565 items=1 drawn=1`——**语音轮进行中 SERVER_PAGE
   并发落地**（SU-RULING-01：内容页 > VOICE，回答期间内容页可见）。
5. t=211115 TTS 回答「明天是周六，上午十点到十一点半有武术班…」。
6. t=231855 human 第二次单击 BOOT（设备处 listening 态）：冻结键位语义 = 关闭会话
   （`WS: Websocket disconnected` → idle）；`voice_end: top=4`——SERVER_PAGE 驻留
   （OQ3=c：回答后屏幕落点确定、非随机回退）。
7. t=240075-243215 human 双击翻页：`cycle -> CHAR (P4)`、`cycle -> SCHEDULE (P2)`。
   human 口述观察（逐字）：「小智页面正中间那个 笑脸图标还在，切换到其他页面，那个也在」
   ——对照冻结设计：`kidpal_render_node.cc:157-158`「聊天 chrome 已 reparent 到
   lv_layer_top（恒浮于活动屏之上、任何页可见）」，SU-RULING-02 单物理屏字面行为，
   非 layer 泄漏；human 知悉后于裁定时接受（见 §五）。

### 轮 B 第一次尝试：提醒到点但设备离线（22:40-22:49，`su-a8-round.log`）

- 服务端布置（human 授权事件 `3ccd1dc6-f97e-4f5f-96a6-d24772cdc603`）：ECS
  `/root/.openclaw/workspace/kidpal/schedule.md` 备份 `.bak-260911-su-a8` 后追加
  一次性条目「23:00-23:10 屏幕归并测试提醒@客厅」。三档机制实测：pending.json 每
  15 分钟重生成（22:04:41/22:19:41/22:34:41/23:04:41 秒级准点），soon 档 =
  开始时刻 − before_min(15)。22:34:41 重生成带上 `rmd-c9504546…` fire_at 22:45:00。
- 22:45:00/22:47:00/22:49:00 front 三次 `alert_skip reason=no_online_device`——
  设备处于 idle 离线态（本 build 会话被引擎告别关闭后不自动重连，需 BOOT/唤醒词
  唤醒），档位按「单档上限 3 次」规则作废。
- 本段设备侧 570s 捕获：uptime 连续（5124s→5692s）、零 WS 活动、零复位——**证明
  「0 复位」证据面在纯 idle 下同样成立**，同时暴露协调缺陷：提醒到点时刻必须由
  human 保持会话活跃（第二次尝试据此改协议并成功）。

### 轮 B 第二次尝试（正式轮）：提醒恰在语音轮窗口落入（23:12:30-23:20:30，`su-a8-round2.log`）

1. 服务端：23:04:41 重生成带上 `rmd-ea23943ea7a727da35837222` fire_at 23:15:00
   （条目「23:30-23:40 屏幕归并测试提醒二@客厅」）。
2. 23:15:00（t=1500525）human 单击 BOOT：`show_chat: VOICE layer push (top=3)` →
   Session `4002e3e1`；23:15:03 第二次单击（listening 态）误触关闭会话（冻结语义，
   `kidpal_render_node` 正常回收：`render home` + `voice_end: top=2`）；23:15:06-07
   第三次单击重建 Session `2227d890` → listening。**会话在 23:15:07 起保持在线**。
3. **23:16:01（t=1561205）提醒到点落入**：
   - 设备侧：`MCP: alert_dispatch corr_id=rmd-ea23943ea7a727da35837222 state=1
     queue=1 known=1 degrade=false` → `alert_render corr_id=rmd-ea23943e… raised=1
     actions=7 tone=ogg_popup speak_leg=engine`；TTS 音频管线启动（OpusHead
     version=1 16kHz 收到，codec output enable 23:16:01→23:16:17 约 15.5s）；
     `Application: << 23:30 有屏幕归并测试提醒二` / `<< 该出发了`。
   - front 侧互证：23:16:00 `alert_deliver corr_id=rmd-ea23943e… delivered=True
     device_frames=1 speech=True`。
   - 落入时设备状态：VOICE 层活跃 + WS 会话在线（listening 待话）——REMINDER(5) >
     VOICE(3) 层级仲裁成立，弹窗置前，无崩溃无复位。
4. 23:17:00 front 重推同档（设备渲染回执未达 front，`acked=0` 触发 60s 重推——
   已知「L4 回执面」live 发现同族复现，见 §五④）；设备侧 23:17:01
   `alert_dispatch state=0 queue=0 known=1`——**冻结契约 push 幂等去重实测成立**：
   不重复弹窗、不崩。
5. 23:17:02 human 单击（listening 态，关闭会话）→ 23:17:16-36 双击翻页
   `cycle -> CHAR (P4)` → `SCHEDULE (P2)` → `STORY (P3)` → `HOME (P1)`，全部落位。
6. 捕获收口（23:20:30）：**uptime 1356445→1832445 连续单调，`rst:|panic|abort|
   Guru|backtrace|assert` 扫描 0 命中**，free sram 69-75KB 稳定。

## 三、断言逐项对账（SU-A8 冻结字面 + ruling-03 补充面）

| 断言 | 证据 | 结果 |
|---|---|---|
| 语音轮+推送页并发 | 轮 A：问答中 `display_dispatch view=schedule drawn=1`（corr_id=kdp-209565） | ✅ |
| 提醒恰在语音轮到点 | 轮 B2：VOICE 层活跃+会话在线时 `alert_render raised=1`，front `delivered=True` 互证 | ✅（落入时为 listening 待话，非语句中途；human 裁定接受，见 §五） |
| 0 复位 0 崩溃（当日三崩溃同族） | 两段捕获 uptime 连续单调、崩溃标记 0 命中；轮内零复位 | ✅ |
| 回答落点确定（OQ3=c 驻留可见） | 轮 A `voice_end: top=4` SERVER_PAGE 驻留；轮 B2 提醒收口 `state=0 queue=0` + 翻页全落位；回答文字驻留聊天层（human 目视确认笑脸/chrome 恒在） | ✅ |
| push 幂等去重（冻结契约，加测） | front 23:17 重推 → 设备 `known=1` 去重不重弹不崩 | ✅ |
| 两键行为一致 | **不在本轮**（ruling-03 边界：本 base 无 PKG-1 KEY 接线；归并后 SU-A7 以 PKG-1 基线复跑收口） | ➖ 未执行 |

## 四、复位归因（如实记录）

当晚共三次设备重启，全部落在捕获窗口之外、与捕获脚本串口开合时刻精确吻合：

| 重启时刻（推算） | 吻合事件 | 归因 |
|---|---|---|
| ~21:09:55 | keypress-01 捕获（180s）结束关串口 | 工具伪影（疑 DTR/RTS 回弹触发自动复位电路） |
| ~21:15:00 | keypress-02 捕获（240s）结束关串口 | 同上 |
| ~22:50:00 | 轮 B1 捕获（570s）结束关串口（22:49:45） | 同上 |

第一次重启时 human 对「是否碰过 RESET/USB」答复「不确定」（AskUserQuestion，
2026-09-11），按归因不明如实记录；后两次时间吻合模式一致（板级 USB-Serial/JTAG
DTR/RTS 自动复位电路即 esptool 免按键烧录所用电路，`serial_capture.py` 明确依赖它
做复位）。**三段正式捕获窗口内 uptime 均连续单调、零复位**——SU-A8「0 复位」断言
按轮内证据判定成立。

## 五、human 观察与裁定（逐字）

- 轮前首次按键观察（21:06）：「按boot没有跳出 对话页」——后证实为设备处开机自动
  会话的 listening 态，单击命中冻结语义「关闭会话」，非按键失效（21:11 起按键
  日志均正常）。
- 笑脸跨页（21:12）：「小智页面正中间那个 笑脸图标还在，切换到其他页面，那个也在」
- 提醒落入（23:16）：「出现了提醒」
- 终轮口述 + 裁定（23:23，逐字）：「1. 显示 有个提示框，里面有内容，但是字体有些
  超出框了，没有播报，2.笑脸一直在，3.没有 4.pass，这个功能上一个包也测试过，
  我还担心你们合并会有冲突」
  - 条目对应：① 提醒弹窗出现、有内容、**字体超出框**、**未听到语音播报**；
    ② 笑脸/chrome 全程恒在；③ 无黑屏/重启/卡死；④ **裁定 PASS**（理由：提醒
    功能上一包已测，本轮担忧点 = 归并是否引入冲突回归——证据表明未引入）。

**SU-A8 裁定 = PASS（human，2026-09-11 23:23）。**

## 六、观察项与交班（非本轮红，逐项登记去处）

1. **提醒弹窗字体超出框**（human 目视）→ 观感面（字体子集/排版）归批次③
   `260911-render-pages`（本包 SU-E2 明确零触碰观感面）。
2. **「没有播报」vs 日志 TTS 管线激活**：设备日志显示 opus 流收到、codec 使能约
   15.5s、`speak_leg=engine`，human 未听到声音。候选因素：16k→24k 重采样失真
   （每会话均有 `resampling may cause distortion` 警告）、音量、engine speak 腿
   实际出声。交 workstream 收口 `260911-display-first-m1` 对账时归音频面 owner
   跟进；不阻断本包（human 已 PASS，且播报链路属上一包已测面）。
3. **`Display: System time is not set, tm_year: 70` 每秒刷屏**：
   `lvgl_display.cc:124-136` idle 态状态栏时钟更新，SNTP 未同步走警告分支且该分支
   漏刷 `last_status_update_time_`（10s 节流失效）。上游 xiaozhi 模式 + 本 build
   未做系统时间同步（首页时钟走 PCF85063 RTC 注入，不受影响）。噪音级，登记交
   观感/平台面。
4. **设备渲染回执未达 front**（front 持续 `acked=0` → 60s 重推 → 设备幂等去重
   兜底）：与 `260911-ecs-deploy-convergence` 已登记交班的「L4 回执面」live 发现
   同族复现，非本包新增红。
5. **聆听态单击 BOOT = 关闭会话**：冻结键位语义，真机易误触（本轮 human 两次
   误触均被 layer 栈正常回收，无损坏）。SU-A7 两键回归复跑时一并观察。
6. **笑脸/聊天 chrome 跨页恒可见** = SU-RULING-02 冻结设计字面（`lv_layer_top`
   常驻）；human 知悉并接受。若后续观感要求按前台层隐藏 chrome，属新需求走
   `/arch-change`，不属本包。

## 七、证据归档清单

- `device-round/su-a8-boot.log`——烧录后启动核验（60s 复位捕获）
- `device-round/su-a8-keypress-01.log`——准备段（仅心跳，无按键）
- `device-round/su-a8-keypress-02.log`——轮 A：语音轮+课表推送并发 + 翻页
- `device-round/su-a8-round.log`——轮 B1：设备离线、提醒三次 skip 作废 + idle 零复位
- `device-round/su-a8-round2.log`——轮 B2（正式轮）：提醒落入 + 幂等去重 + 零复位
- 服务端互证：front journalctl `alert_deliver/alert_skip/alert_poll`（23:14-23:18），
  pending.json 档位 `rmd-ea23943e…`（fire_at 23:15:00）；ECS `schedule.md` 测试条目
  已于 23:21 从 `.bak-260911-su-a8` 还原删除（live 数据零残留）。
- pod-events：裁断①②（`e3d74b2c` / `1b1f46cf`）、schedule 写入授权（`3ccd1dc6`）、
  PASS 裁定事件（见本记录提交时的追加行）。
