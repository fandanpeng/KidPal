# RM-A8 真机确认轮 · human 裁决落账（2026-09-17，合并链前）

> 本文件为 260917-m1-k4-rma8-fixbatch 合并链前最后一个制品，逐字记录主线程 2026-09-17 转达的 human 裁决与真机目击证据。裁决已锚入 pod-events human-decision `a4f89467-8038-4878-98f3-1ec4c2979aa7`（2026-09-17）。
> 取证形态沿用 [device-round-1.md](./device-round-1.md) 的三方互证口径：设备侧串口 + ECS 网关日志 + human 目击。

## 1. 裁决总览

1. **合并批准**：human 批准升裁合并（approve-escalated-merge）——机械 supervisor verdict 为 ESCALATE（riskTier=high），human 目击真机实证后放行。
2. **F1 实证通过**（渲染回执接线）——FX-A2 真机齿由 deferred 转**真机实证通过**。
3. **F2 实证通过**（弹窗驻留守卫）——FX-A3 真机靶由 deferred 转**真机实证通过**。
4. **弹窗字号 14px 裁定：接受**（详见第 4 节全文）——不另开批。
5. **剩余项**（BOOT 键位 / 录音排队例外 / 模拟器截图）随合并后重跑轮补测，证据落本目录。

## 2. F1 实证证据（三方互证，human 目击确认）

- **设备侧**：`alert_receipt ack=true dequeued=true`——渲染回执按「画成即武装」语义正常发出并被网关接收，device-round-1 F1 根因（mark_rendered 零调用点 → 回执永不发出）已消解。
- **网关侧**：`alert_ack attempts=2`——网关收到回执后 ack 链路收敛；60s 重推链消失，**无 60s 重推**（device-round-1 现象「重推 ×3 后静默作废」不再复现）。
- **human 目击**：TTS **单次播放**——重推消失连带 speech 腿重播消失（device-round-1「用户实际听到两遍」不再复现）。

**判定**：F1 修复（mcp_server.cc:366 mark_rendered 一行接线）真机实证通过。

## 3. F2 实证证据（串口 + human 目击，human 目击确认）

- **串口**：`voice_look_skip reason=reminder_persist`——F2 守卫（`kidpal_voice_apply_look` 入口条件守卫，lcd_display.cc）在 speaking 态实际触发，拦截 TALK/LISTEN 场景刷回，弹窗不再被语音轮视觉件覆盖。
- **串口 + human 目击**：弹窗**全程 top=5 驻留**（REMINDER 层层级值，与 device-round-1 layer 栈方向记录一致）——弹窗自 alert_render 起持续在屏，不即起即灭。
- **human 目击**：KEY 关闭**回当前页**——RM-A6 KEY 语义正确执行，与 device-round-1 第 120.4s 观测口径一致。

**判定**：F2 修复（apply_look 入口条件守卫）真机实证通过，弹窗驻留 + 最高层覆盖两项此前不成立的缺陷均消解。

## 4. 弹窗字号 14px 裁定（全文）

弹窗「大字事项」字号缺口经主线程核验：弹窗与对话同为 `font_puhui_14_1` **14px**（无更大字号档）。

**human 裁定：14px 接受**。约束理由：受字体资产限制——全量 30px 字体超出分区容量，稀疏子集则缺字形，当前 14px 为资产约束下的可行解；观感由排版承载（版式层级而非字号承载「大字」语义）。**不另开批**。

## 5. FX-A2 / FX-A3 状态转移（deferred → 真机实证通过）

| 条目 | 原状态（review-packet 验收对照表） | 现状态 | 证据 |
|---|---|---|---|
| FX-A2 F1 真机齿 | PENDING RM-A8（structural deferred） | **真机实证通过** | 第 2 节三方互证（ack=true dequeued=true / attempts=2 / 单次 TTS / 无 60s 重推） |
| FX-A3 F2 真机靶 | PENDING RM-A8（structural deferred） | **真机实证通过** | 第 3 节串口 + 目击（voice_look_skip / top=5 驻留 / KEY 回当前页） |

## 6. 剩余项清单与去向（合并后重跑轮）

| 剩余项 | 说明 | 去向 |
|---|---|---|
| BOOT 键位 | RM-A8 清单未测项 | 合并后重跑轮补测 |
| 录音排队例外 | FX-A5 机制面已绿（TEST 1 tooth2 + TEST 7 + MUT-FX-A5），真机例外行为未目击 | 合并后重跑轮补测 |
| 模拟器截图 | FX-A7 内 planner 裁定项（K4 收口遗留缺口，不拆独立批） | 合并后重跑轮顺带补 |

补测证据统一落本目录（`delivery/changes/dev/260917-m1-k4-rma8-fixbatch/`），append 纪律不覆盖既有制品行。
