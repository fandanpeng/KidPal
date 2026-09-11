# 阶段 2/3 live 实测发现与收口判定 · 260911-ecs-deploy-convergence

> 部署窗口 2026-09-11 10:33–12:10，human 全程在场。本条为 planner 第一人称收口记录，供 integrator 进 Review Packet。

## 交付达成（绿）
- **ECS-A1**：修复后 overlay 渲染面 vs live 快照结构 diff = 仅 prompt v1→v4 一条（严格零意外 delta）✓（D1-D6 全对齐，D5 经 human 纠偏回 false）
- **ECS-A3**：live config 四 provider 全对（Qwen3ASRFlash/OpenClawOpenAI/EdgeTTS/nointent/nomem）+ prompt v4 + 一轮真实语音端到端 0 断链（10:55 引擎日志 exercise：listen→ASR 识别→TTS 生成→音频下发）✓
- **ECS-A4**：kidpal-reminder-plan cron 在册（862ce6ab，venv 解释器载荷）+ `state.lastRunStatus=ok`（12:00 后确认）+ kidpal-front.service env 含 KIDPAL_ALERTS_DIR/KIDPAL_SERVER_DIR + `alert_poll` 心跳在场（11:34-11:45）✓
- **ECS-A5 L1/L2/L3**：L1 cron 摘录、L2 pending.json 20 档位逐字段对照契约 §三全合、L3 真实推送（`alert_deliver delivered=True device_frames=1 speech=True`，语音下行证据 `发送音频 FIRST "@/LAST"`）✓
- **ECS-A6（WSA-A4 归位 + WSA-A6）**：flash.log 归位 `server/acceptance/_artifacts/device-ws-switch/`，188 处烧录标记精确匹配；ecs-teardown-evidence.md 当日真只读探针（8080 `TcpTestSucceeded: False`、8000 `True` 对照）；host runner WSA-A6 绿。

## 实测发现（记 finding，不塞绿）
1. **L4 渲染回执 live 未达**：4 次投递均只收到设备 `ack:false` 受理帧（`state:queued`），无一次 `ack:true` 渲染回执 → 档位不出队、走重推上限。**human 现场实测症状**：板子只显示「该出发了」等 speech 文字（普通文字上屏），**未出现 state-reminder 富弹窗**——提醒弹窗消费面（`reminder_page`）live 未渲染，掉回普通显示路径，故无渲染回执。固件侧 `mcp_server.cc` 有 `kidpal_reminder_may_emit_receipt` 判定门，live 未触发。疑闪录固件的提醒弹窗面未随 schedule-reminder 生效。归 schedule-reminder 固件面后续排查，本包越权不改冻结固件。
2. **engine-patch `_kidpal_try_speak` await 报错**：每次提醒语音注入伴随 `ERROR-kidpal_speak 播报注入失败: object NoneType can't be used in 'await' expression`，但 edge-tts 语音仍照常合成下发（两段音频真实送出），属「报了错但主链仍通」——真 bug，在 own code `server/deploy/engine-patch/connection.py`，归后续修复。
3. **设备休眠即断 WS → 推送只落在线时刻**（human 现场指出的产品级问题）：`alert_skip reason=no_online_device` 多次出现；设备闲置约 3 分钟即「再见」休眠断开。三档提醒的价值前提（前晚/早档在家时段在线）与当前休眠/连接行为不匹配。**产品决策项**（候选项：常亮/充电时段保连、重连即重推重审、档位时刻对齐唤醒时段、晨档唤醒时补投）——记 human 待裁，不属本包实现面。
4. **WSA-A4 host 齿仍红（留红如案，不伪造）**：除 flash.log 外的三制品（provision.log / smoke-roundtrip.log / screen*.jpg）是 260903-device-ws-switch 包 closeout 时散失的证据遗物，部分真源在 `C:\tmp`（screen1.jpg、device_ws_nvs.csv、engine_*.log）但往返证据 session-id 不配对（d6f4e629 vs 40024357），**无法诚实拼装成一次匹配会话的往返**，故留红、不拼凑过齿。flash.log 归位（本包 #13 交付项）已完成。

## 回滚就绪（ECS-A7）
- live `.config.yaml` 时间戳备份多份在 ECS：`.config.yaml.bak-260911-planner-s01`（260907 态）+ 既有 `.bak-260905/.bak2-260905/.bak-260906-nointent/.bak-260907-vrt-prompt`；本地脱敏快照 `s0.1-live-config-snapshot-260911.redacted.yaml`。
- `openclaw cron remove 88fceff0`（首版重复条目）已实际演练成功（`"ok": true, "removed": true`）——演练路径 = `openclaw cron remove <id>`，ECS-A7 回滚演练记录据此成立。
- 代码面：引擎补丁 / gateway 代码 ECS 侧均留 `.bak-260911-*` 备份可还原；schedule.md 留 `schedule.md.bak-260911-l3l4`。
- 冻结面（docs/contracts/**、*.acceptance.*、managed_components/**）本窗口 git 零触碰。

## human 待办（交班）
- 安全组 8080 规则状态在 ECS 管理面确认并回填（agent 权限面外，先例 shim-archive-record.md:22）。
- L4 回执面 / kidpal_speak bug / 休眠连接设计三项 finding 的产品与技术裁量。
- 测试条目「部署核验测试提醒」已无用，下次编辑课表时可删（schedule.md「一次性安排」行）。