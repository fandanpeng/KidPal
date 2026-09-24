# 交付完工度 · STATUS（自动生成，勿手改）

> 由 [.claude/scripts/delivery-status.mjs](../.claude/scripts/delivery-status.mjs) 从 [milestones.jsonl](milestones.jsonl) 生成。
> 单一事实源是 `milestones.jsonl`（里程碑基线，镜像 `core.Milestone` + `delivery.MilestoneDeliveryState`）；改状态去改它再重跑：`node .claude/scripts/delivery-status.mjs`。
> 里程碑度量**交付（实现验收）**、非设计；完工比例只读派生、不手填、不数 `delivery/workstreams/` todo。

## 完工比例

- **已验收**：0%（0 / 4 里程碑 accepted 及以后）
- **在途预计**：30%（按 state 阶段加权：planning 0 · in_delivery .4 · qa .7 · ready .9 · accepted 1）

## 里程碑基线

### M1

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 阶段一·首页 + 百科问答（对话主路径体验） | `in_delivery` | green | 全局 UI 基建先行（键帽条三槽/覆盖层 BOOT 隔离/KEY 长按返回上一层，§2.3 键位对账）；P1 新版式「接下来」+ 电量 ADC（S-F1.1）；百科问答演出补齐（思考页/聆听回显/问答框滚动/打断手感，S-F1.3）+ happy-flow-host-capture 收尾；提醒弹窗四态统一（S-F1.4） | [ref](../docs/requirements.md) |

### M2

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 阶段二·查字窗（live 链路修复 + 页→窗改造） | `in_delivery` | green | live 查字链路修复（NFR-PERF-2 黄灯转绿前提，host 面已验、live 端到端待实证）；撤 P4 常驻页、页循环 4→3、锚 canvas-p4→char-window 对账改名（F3 全节） | [ref](../docs/requirements.md) |

### M3

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 阶段三·课表（一周网格 + 家长录入 + 数据接首页） | `in_delivery` | green | K5 设备时间进行中（SNTP 校时 + PCF85063 写回 + P1 周期 re-inject，260922-m3-k5-sntp-time-sync）；P2 一周总览网格 + 语音问课表口径（S-F1.2/S-F1.3）；家长录入 B4 课表数据源头（S-F1.5）；与阶段一合流——课表数据接首页「接下来」（决策 26 链路） | [ref](../docs/requirements.md) |

### M4

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 阶段四·星星记录本（P3 四态 + F2 改名对账） | `planning` | green | 拆包已落（2026-09-24，三包 mergeOrder 38–40，workstream m4-star-notebook）：K1 story-cloud-link（契约三触点门【story_list 语义拆分/story_text seq/删除载体 kidpal_story_delete】+ front session_ctx 拦截/story_list 推送/删除执行/seq 账本/收尾词+120s）→ K2 p3-star-notebook-ui（P3 键路由补全+300ms 消歧 + 列表/删除覆盖层/阅读/盖章 + CJK 字体接线 + F2 改名对账四项）→ K3 record-session-live（板端 session_ctx 发送 + 引导/录音态演出 + prompt v7 双指令 + live 真机轮一次性收口）；契约门 HUMAN_APPROVED + 门 1 过目 = K1 launch 前置（LP1/LP2） | [ref](../docs/requirements.md) |

