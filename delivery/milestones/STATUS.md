# 交付完工度 · STATUS（自动生成，勿手改）

> 由 [.claude/scripts/delivery-status.mjs](../.claude/scripts/delivery-status.mjs) 从 [milestones.jsonl](milestones.jsonl) 生成。
> 单一事实源是 `milestones.jsonl`（里程碑基线，镜像 `core.Milestone` + `delivery.MilestoneDeliveryState`）；改状态去改它再重跑：`node .claude/scripts/delivery-status.mjs`。
> 里程碑度量**交付（实现验收）**、非设计；完工比例只读派生、不手填、不数 `delivery/workstreams/` todo。

## 完工比例

- **已验收**：0%（0 / 4 里程碑 accepted 及以后）
- **在途预计**：20%（按 state 阶段加权：planning 0 · in_delivery .4 · qa .7 · ready .9 · accepted 1）

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
| 阶段三·课表（一周网格 + 家长录入 + 数据接首页） | `planning` | green | P2 一周总览网格 + 语音问课表口径（S-F1.2/S-F1.3）；家长录入 B4 课表数据源头（S-F1.5）；与阶段一合流——课表数据接首页「接下来」（决策 26 链路） | [ref](../docs/requirements.md) |

### M4

| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |
|---|---|---|---|---|
| 阶段四·星星记录本（P3 四态 + F2 改名对账） | `planning` | green | P3 四态全家：列表/记录会话（录音+归档链路）/星星徽章盖章/阅读态（S-F2.1~2.4）；F2 改名对账四项（盖章文案/页名/提示缺第二指令/选中反白→边框） | [ref](../docs/requirements.md) |

