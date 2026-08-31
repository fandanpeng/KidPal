# 交付完工度 · STATUS（自动生成，勿手改）

> 由 [.claude/scripts/delivery-status.mjs](../.claude/scripts/delivery-status.mjs) 从 [milestones.jsonl](milestones.jsonl) 生成。
> 单一事实源是 `milestones.jsonl`（里程碑基线，镜像 `core.Milestone` + `delivery.MilestoneDeliveryState`）；改状态去改它再重跑：`node .claude/scripts/delivery-status.mjs`。
> 里程碑度量**交付（实现验收）**、非设计；完工比例只读派生、不手填、不数 `.planning/` todo。

## 完工比例

- **已验收**：0%（0 / 7 里程碑 accepted 及以后）
- **在途预计**：11.4%（按 state 阶段加权：planning 0 · in_delivery .4 · qa .7 · ready .9 · accepted 1）

## 里程碑基线

### M0

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 跑通官方小智固件，体验交互手感 | `in_delivery` | green | 官方小智固件已跑通、硬件能力已实测盘点（spec §2）；声学/AEC 实盘证据收口（audio-bargein-hw / hardware-validation 包） | [ref](../docs/spec.md) |

### M1

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 薄桥 shim + 设备↔shim↔OpenClaw 联通 | `in_delivery` | green | shim 与云端链路验证批收口（cloud-link-bringup 8/9，display 12/12）；待设备端服务器地址指向 shim 的真机联通（阶段 C 殿后） | [ref](../docs/spec.md) |

### M2

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 全链路：语音→ASR→OpenClaw→回复→TTS→设备播放 | `planning` | green | whisper(ASR)+DeepSeek+TTS 全链路真机串通；char 冷启动 ~109s 延迟待流式/异步消解（M2） | [ref](../docs/spec.md) |

### M3

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 查生字（笔顺 JSON 下发 + LVGL 逐笔演示） | `planning` | green | makemeahanzi 笔顺 D3 JSON 下发 + lv_canvas 逐笔渲染（char-stroke 包）；D3 格式已定稿 | [ref](../docs/spec.md) |

### M4

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 4 页 UI 重做 + 课表 + RTC 提醒 | `planning` | green | 首页 P1/课表 P2/故事 P3/查字 P4 四页 1bpp RLCD 重做 + 课表 MD 消费 + RTC 提醒（home-p1 / schedule-p2 / schedule-reminder 包） | [ref](../docs/spec.md) |

### M5

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 故事记录 + 归档 + 语音喊停实测调优（AEC） | `planning` | green | 故事记录/列表/回放 + 归档 + AEC 喊停误触发/漏触发阈值实测（最大技术风险点） | [ref](../docs/spec.md) |

### M6

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 家长端录入 + 配网绑定 + 稳定性 + 功耗优化 | `planning` | green | 家长端形态（QQ/小程序/MD）待定；配网与设备绑定 + 按一下会话续航功耗实测（spec §9 待细化） | [ref](../docs/spec.md) |

