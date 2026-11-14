---
changeId: 261104-k3-qq-points-channel
title: QQ 家长积分通道——正式 kidpal-points-write skill、加兑查删、稳定定位与一次性错误回复
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
  workstream: 261103-star-points-story-cover
reviewed-by: architect（/triage-change 分流 review 2026-11-03；门 1 待主线程另派）
date: 2026-11-04
gateProfile: ["evidence-integrity", "test-quality", "state-machine", "mcp-tool-contract"]
dependsOn: ["261104-k1-points-ledger-core", "261104-k2-points-device-ui"]
mergeOrder: 74
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- K0 只完成真实 QQ 最小 canary，家长还不能使用完整加/兑/查/删。现有 `kidpal-schedule-write` 提供“自然语言→一次性命令→确定性机检→marker/推送→真实结果回复”的工程先例，但其数据源和写入规则属于课表，不能复用为积分账本。
- QQ 还有两个已知硬边界：`ask_user` 会把会话卡死，直接写 kidpal 数据目录会破坏模型只读与单 writer 防线。积分删除必须能稳定定位，歧义输入必须普通文本报错，而不是靠多轮澄清。
- 本包新建正式 `kidpal-points-write` skill，复用 K1 CLI 与 K2 restore 消费缝。目标自定义、切换和删除配置在同 skill 上的扩展归 K4。

# 可行性判据

- K1 已提供 CLI、稳定 command_id/分录 id、兑换和删除不变量；K2 已提供 QQ restore 不抢前台与积分页原地刷新。K3 可以把验证重心放在真实家长输入、稳定定位和通道证据。
- 课表 skill 的机检、部署、真实结果回复形态已实读，可复用工程形态而不复制数据写入。points.py 的原子写替代课表“先备份再还原”的文件级保护，不需要发明第二套状态修补流程。
- 真实 QQ 可逐条发送独立命令并观察回复与设备，能直接验证歧义不卡死、删除定位和其他页不抢前台。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **K0→K3**：K0 的真实 QQ canary 只证明通路，本包不重复 spike，也不把临时最小接线当成正式 skill 已完成。
- **K1→K3→K4**：K1 拥有所有命令语义；K3 建立 skill 和加/兑/查/删；K4 在同一 skill 增加目标配置。任何一层都不得直接编辑 `/opt/kidpal/points/` 或 `schedule.md`。
- **K2→K3**：QQ 写入/删除的下行均 restore:true，其他页只更新数据；K3 真实 QQ 验收要同时观察 QQ 回复和 K2 的设备表现，不能只看 CLI。按 NFR-PERF-4 记录 QQ 消息到积分页原地刷新的实测时延，不写死阈值。
- **相邻冻结锚拆包期预扫非零命中**：课表 skill/验收、OpenClaw 部署三面、K1 points 核心验收均为保留面。`ask_user` 禁令是新增积分 skill 的硬约束，不要求也不允许借本包修 QQ 插件本体。
- **传输幂等**：QQ command_id 来自平台消息 id 加命令序号；新的家长消息即使文案相同也必须新建 command_id，再交给 K1 的短窗语义去重判断，不能由 skill 自行吞重。

# 铁律约束

- 所有加/兑/查/删只调 points.py CLI，并以真实回执回复；不得直接编辑状态文件、计算另一套余额、写 schedule.md 或把模型生成的分录 id 当事实。
- 不调用 `ask_user`，不依赖多轮澄清。无法唯一理解时普通 QQ 文本一次说明正确说法；删除只用稳定记录 id 或最近列表序号定位。
- 兑换只认激活目标分值且真实兑现后扣，同一次兑换不重复扣；不提供惩罚性扣分入口。查账回答来自真实状态，不凭对话记忆猜余额。
- QQ 推送永不抢前台；本包不新增设备设置页、不提前实现 K4 目标管理，不宣称修复 TECH-261030 插件缺陷或其他并行 debt。
- 不改 requirements、architecture、`docs/contracts/**` 或 workspace-kidpal 数据只读边界；需要扩大写权或直接文件写时立即停并升级。

# demo 证据形态（与 roadmap 一致）

- server 离线验收：skill 静态纪律、CLI 参数与回执、稳定定位、错误回复、command_id、部署三面和 K1/K2 回归。
- 真实 QQ 记录：加、兑、查、删各一笔，另含金额不匹配、重复兑换、歧义输入和定位不清；普通错误回复及时到达且无卡死。
- 设备看屏/串口 + front/points/QQ 日志：证明其他页不抢前台、积分页原地刷新，以及 QQ 回复与权威账本一致；按 NFR-PERF-4 记录真实 QQ 消息到积分页原地刷新的实测时延，不写死阈值。

## 技术方案（planner+architect 预评）

> 2026-11-14 · architect ARCH-ADVISORY（dev-loop Step 1g 技术预评档）：只出意见、不落地、不改任何文件；advisory 不豁免 verify.mjs 任何闸、不构成通过判据、不改 TaskPackage 事实。planner 已核预评未决项：`POINTS_STATE_ENV = "KIDPAL_POINTS_STATE"`（points.py:89）；`server/gateway/__init__.py` 在册，`cd /opt/kidpal/front && python -m gateway.points` 可导入；`deploy.sh` / `verify-deploy.sh` 均为 `manifest.yaml` 表驱动（改 manifest 即生效，两脚本预期零改动）。

### 写入下行通路

- **观察缝已接线，无需新造 marker**：front `points_observe`（front_app.py:843）/ `run_points_observe_once`（:887）已在轮询腿真实调用（:2363/:2371），比较 points.py 权威 revision 快照；变化则经 `_push_display_payload`（:597）恰推一帧 `points_home`（`data.restore=True`），条件成立时补一帧 `points_ledger` 首页（restore=true、reset=true）。基线维护三处齐全（建连 :958、语音消费后 :797、推送后 :868）。
- **通路**：QQ skill 调 `KIDPAL_POINTS_STATE=/opt/kidpal/points/state.json /opt/kidpal/venv/bin/python3 -m gateway.points ...`（先 `cd /opt/kidpal/front`）→ points.py 在 `_critical` 文件锁内原子替换 state、revision+1 → front 下一轮观察腿检出变化并推帧。CLI 与 front 共用同一把锁（points.py:236-279，docstring 明写覆盖 front API 与 QQ CLI 跨进程）。
- **两个真实边界（契约内，不改契约）**：① 删除幂等 no-op（id 形状合法但不存在）不写账、revision 不变 → 不触发推送，回执 `deleted=false` 必须与成功分开措辞，不得宣称「设备已刷新」；② 推送的 ledger 恒首页且 `page.reset=true`，设备在任何页都会被重置回首页——这是契约 §四防止跨版本拼接的既定语义，不算缺陷。
- 解释器与加载方式：沿用课表 skill 的钉死解释器纪律（裸 `python3` 在 ECS 解析到 3.6.8），但**不要照抄课表 skill 的 `spec_from_file_location` 段**——`python -m gateway.points` 需要包可导入，两种加载方式互斥。

### CLI 扩展（points.py）

保持既有单行 `key=value` 回执形态，扩字段不换格式（skill 要解析它）；回执行补 **revision** 与 **command_id**（现有 `_entry_receipt_line` docstring 声称有 revision，实际没输出）。

| 子命令 | 形状 | 说明 |
|---|---|---|
| `add` | `--points N --reason TEXT --command-id ID` | 兼容既有位置参数调用 |
| `redeem` | `--points N --command-id ID` | 事由由执行器取「兑换<激活目标名>」，模型不提供 |
| `delete` | `--entry-id pt_x` 或 `--index N`，另带 `--command-id ID` | 两者互斥；未命中 = 幂等 no-op、exit 0 |
| `show` | 无参 | balance / revision / 激活目标四键 / entries_total |
| `recent` | `--limit N`（缺省 5，上界 1–50） | 每条一行，**index 与 entry_id 同行**，由执行器输出 |
| `targets` | 无参 | 只读列举；目标新增/切换/删除归 K4，本包不开 CLI 口 |

- **序号→id 映射恒在 points.py 内、且在同一 `_critical` 临界区一次做完**：需拆出内部 `_delete_entry_locked(...)`、公开 `delete_entry` 转调；否则「先读序号再删」之间存在 TOCTOU 窗口，新写入会让序号静默漂移、删错账。排序权威 = `_entry_sort_key` / `_sorted_desc`，与设备 ledger 分页同一把尺。
- `recent` 把 index 与 entry_id 同行输出 = 把序号→id 的换算权从模型手里收走。

### command_id 与去重

- 首选透传 `qq:<msg_id>:<seq>`；平台消息 id 在 skill 运行环境的可得性未核，按不可靠设计（TECH-261023 工具面 prompt-only/只读）。
- 降级：skill 生成**公开可对账**的 `qq:<yyyymmddHHMMSS>:<reason 短哈希>`，并在 QQ 回复正文中把 command_id 念出来——重复因此成为可被发现的事实，而不是被静默吞掉。
- 去重权威不在 skill：command_id 传输幂等（同 id 重放返首次结果、不新增分录、不增 revision）先于短窗事由去重执行，两层都由 K1 已落地。**SKILL.md 必须写死：恒调 CLI，永不自行判断「像重复就不调」。**

### 删除定位边界（一次性普通文本报错）

必须一次性报错 + 给可重述格式（禁 `ask_user`、禁多轮澄清）：① `--entry-id` 与 `--index` 都缺或都给；② index 非正整数 / 越界 / 空账；③ entry-id 畸形；④ 只给自然语言描述、无可解析定位符；⑤ 试图按金额/事由/目标删除。**不属于错误**：entry-id 合法但不存在 = 幂等 no-op，回复「该记录已不存在（可能已被删除），账本未变」。

### 离线可机检 vs 只能真实 QQ 轮

- **离线（server pytest）**：新 CLI 子命令分派与 usage；回执行逐字段精确断言；拒绝 = exit 1 + stderr + **state 文件字节不变**且 stdout 无成功回执；command_id 透传幂等；序号删除的锁内性（中间条、越界、零写入）；SKILL.md 静态纪律 + 负向断言（不得出现 `ask_user`、不得指示直接编辑/写 state.json、不得残留 schedule.md 写路径与 front parser 机检段）；tmp state + 假 websocket 驱动 `run_points_observe_once` 断言 `_push_display_payload` 恰一次且 `data.restore is True`；部署三面（manifest 条目、deploy 表驱动、verify md5）。
- **只能真实 QQ 轮（human 在场）**：ask_user 不触发、无 900 秒卡死、无后续消息排队；NFR-PERF-4 实测时延（记值不写死）；其他页不抢前台；模型真实遵守 SKILL.md；command_id 实际可得性。

### front_app.py 改动区约束

K3 真实 delta 极小（缝已接线）。只准动 `# GAP-1 producer 侧` 观察块（约 :804-900）与 delete/page handler（约 :1049-1144）；**不得改** `_push_display_payload`（:597）与轮询循环结构（:2353+）；出区走唯一扩单通道（builder/fixer → planner 带根因假设 → architect 评估）。

### 明确禁改（负向约束）

- `server/deploy/261017-openclaw-kidpal-skills/kidpal-agent-skills-declaration.json`：它管**板侧 kidpal agent** 的 `modelVisible` 三项白名单，与 main 侧新 skill 无关；误改会扩大孩子 agent 可见技能集，且踩 K2 F1 修复轮碰过的冻结件（含 drift_guard 锚与 backup/rollback）。
- `261022-openclaw-kidpal-tools/kidpal-agent-tools-declaration.json`、`261022-openclaw-honcho-plugin/honcho-plugin-declaration.json`：同作用于运行态 `openclaw.json`，不触。
- `server/deploy/openclaw-skills/workspace-kidpal/**`（含只读 `kidpal-points-answer`）：板侧只读边界与零写权是 K2 F1 既定结论。
