# device-round-record · 260911-display-first-m1（DF-A3 时延 + DF-A6 workstream 收口真机轮）

> 执行模式（PKG-1 KR-A5 / PKG-2 SU-A8 同型）：agent 部署 + 烧录 + 串口捕获，human 持板口述观察，**PASS 裁定权归 human**。
> PASS 凭据：human 口头裁定「pass 通过」（VS Code 交互会话，2026-09-12）。本件由 planner 主线程落盘。

## 1. 部署 + 烧录事实

- **引擎（live ECS 8.137.91.16）**：`server/deploy/engine-patch/{connection.py,kidpal_display_strip.py}` 覆盖部署（human 授权 event `authorize-engine-patch-direct-deploy`，旧版备份 `.bak-260912-pre-m1`）；上传字节核验 85991/7880 == 本地；`on_block_closed` 计数 connection=2 / strip=8；`docker restart` 后启动零报错（LLM=OpenClawOpenAI / ASR=Qwen3ASRFlash / Intent=nointent / VAD=SileroVAD 全初始化成功）；`docker inspect` Mounts 两位在位（`deploy/engine-patch/*.py -> core/*.py`）；每次设备建连打「`kidpal display-strip patch active`」标记行。
- **固件（COM5，ESP32-S3 MAC 58:e6:c5:72:0c:bc）**：`firmware/build/xiaozhi.bin`（0x2c2e00 级，PKG-1+PKG-2 合体）full flash（bootloader/app/分区表/assets/art 全写，hash verified，hard reset）。

## 2. 三日志源 correlationId 对齐（DF-A3 证据）

轮次「我的课表」（session_id `ad3d502e-2b25-4e52-b67d-00ee69816725`）：

| 源 | 时间 | 标记 | 内容 |
|---|---|---|---|
| 设备 | 160725ms | `Application: >>` | `我的课表`（ASR 上行） |
| 引擎 | 12:15:50 | `识别文本` | `我的课表。` |
| 引擎 | 12:15:50 | `大模型收到用户消息` | `我的课表。` |
| **设备** | **169745ms** | **`MCP: display_dispatch view=schedule`** | `items=2 is_tomorrow=1 degrade=false` |
| **设备** | **169745ms** | **`MCP: schedule_render`** | `items=2 drawn=2`（真画上屏） |
| 引擎 | 12:16:01 | `发送第一段语音` | `今天周六上午十点到十一点半是武术班` |
| 设备 | 171135ms | `Application: <<` | `今天周六上午十点到十一点半是武术班`（TTS 首段） |

**DF-A3 判读**：display 上屏（169745ms）**比 TTS 首段（171135ms）早 1390ms ≈ 1.39 秒**。
判据「display 页上屏不晚于 TTS 首段后 0.5s」＝**满足，且是早到（负向）非卡线**。

（注：引擎日志对 display 块发射自身不打 log 行——`on_block_closed→websocket.send` 无日志；display 块的到达证据在**设备侧**「`display_dispatch`/`schedule_render`」。首次读引擎日志因缺该标记误判「未发块」，已被设备串口证伪。）

## 3. DF-A6 workstream 收口五条并集对账

| 收口标准 | 本包轮实测 | 裁定 |
|---|---|---|
| 上屏时延（本包承载） | display 早 TTS 首段 1.39s，≤+0.5s | PASS（§2 证据） |
| 屏幕落点确定（PKG-2） | 语音轮结束后 `voice_end: top=2/4`；display 后页驻留；翻页 `cycle CHAR→SCHEDULE(n=1)→STORY→HOME` 四页闭环，无随机跳页 | PASS（PKG-2 已验项复核） |
| 两键行为与路线一致（PKG-1 语义 × PKG-2 回归，SU-A7 并后复跑） | `KEY_ID_KEY=0`→`action=1`（对话，6 次）；`KEY_ID_BOOT=1`→`action=3`（翻页，6 次）；与 spec §4.0/§5.3「KEY 主司对话、BOOT 主司翻页」一致 | PASS |
| 当日三崩溃同族 0 复位（PKG-2） | 全轮 grep `Panic\|abort\|rst:\|reset\|reboot\|boot:\|assert\|Backtrace` **零命中**；`free sram` 全程 52–75KB，无泄漏式下滑 | PASS |
| 两键聆听态一致性（SU-A7 重点观察：聆听态单击 BOOT=关闭会话 × KEY 语义一致性） | 日志未现聆听态不预期行为；`key_gesture_parser gesture=1` 全程单按手势无 ghost 双击（`gesture=2` 零命中） | PASS |

## 4. human PASS 凭据

- human 口头裁定「pass 通过」（2026-09-12 真机轮后，VS Code 交互会话）；PASS 裁定权恒归 human（SU-A8/KR-A5 先例同型）。
- 本包以 agent 身份只做烧录 + 串口捕获 + 三源对齐；上屏观感终裁与两键物理确认由 human 持板完成。

## 5. 在册红口径（非本包新增，留红如案）

- SMK-A4（server `test_smoke_e2e.acceptance.py:422` screen.md 缺失，owner=260829-smoke-e2e）——本包真机轮产生的上屏证据可**顺带**供 owner 补证，但本包不代补、不标本包红。
- host 4 在册红（WSA-A6/A4 / P2-A1 / VRT-A2·E2）——本包 diff 零 touch firmware，引用 PKG-2 existing-red-inventory，不重跑不代修。