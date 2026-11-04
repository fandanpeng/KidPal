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
