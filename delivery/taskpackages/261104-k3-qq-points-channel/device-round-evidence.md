# device-round-evidence · 261104-k3-qq-points-channel

> K3 真实 QQ live 轮（2026-11-15）。本包 `deviceOutcome.userVisible=false`，前置升级闸的
> 机械 deviceOutcomeGate 不消费本文件；此处如实留痕，供 K3Q-E2/E3/E4 判读与 human 复核。
> 三源 = 真实 QQ 会话（OpenClaw `chat-qqbot-main`）/ front journalctl / `points.py` state receipts。

## 一、真实 QQ 已通过

| 场景 | 证据锚 | 结论 |
|---|---|---|
| 加分 | `qq:20261115113811:add3hw` → +3（26→29），front 观察缝推 restore=true | PASS |
| 删最近那条 | `delete:qq:20261115113840:del3hw` 删刚加那笔（29→26） | PASS |
| 查账 / 最近记录 | human 确认回复正确 | PASS |
| skill 路由 | main agent `kidpal-points-write` ✓ ready；command_id 形态符合 §3 | PASS |
| 目标配置（与 K4 共用 skill） | 全部 target 动词 receipts 正常 | PASS（K4 面） |

## 二、未通过 / 未闭环（如实申报）

- **歧义删除未闭环**：2026-11-15 三次「删掉昨天/今天那条」在候选 ≥2 时被模型猜着执行
  （`delete:...113920:delyesterday`、`...115710:delyesterday2`、`...115855:deltoday`），
  误删 3 条真实分录（其中 2 条发生在 SKILL.md §5.1 部署后）。
  已部署两刀：SKILL.md §5.1 相对时间定位纪律 + `workspace-main/AGENTS.md` 常驻硬纪律
  （均 additive，K3Q-A5 静态守卫 21 passed）。**运行层是否遵守未复测通过**；
  human 2026-11-15 裁定「删了这种情况再观察一下」，记为本包已知未闭环项，不宣称已修复。
  根因观察：main agent 模型 = `deepseek/deepseek-v4-flash`，可读 skill（command_id 形态正确）
  但跳过 `recent` 前置；是否需要更强模型或 CLI 层机械确认，留待观察后决策。
- **重复输入（QQ 直发同句）**：未单独复测；设备语音侧换措辞不会触发短窗去重（按归一化事由判定）。
- **兑换正向（真实兑现后扣）**：未测；仅有离线与设备侧负向。
- **NFR-PERF-4**：观测到 QQ→设备刷新为秒级，未单独记录精确秒数（不写死阈值）。

## 三、机器消费行

本包 `deviceOutcome.userVisible=false`，无机械设备门消费；不落 `device-round-evidence（JSON）：` 行。
