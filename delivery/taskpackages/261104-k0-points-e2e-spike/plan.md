---
changeId: 261104-k0-points-e2e-spike
title: 端到端最小 spike——真实语音与 QQ 记账、权威 points_home、设备消费与首尾分页 canary
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
  workstream: 261103-star-points-story-cover
reviewed-by: architect（/triage-change 分流 review 2026-11-03；门 1 待主线程另派）
date: 2026-11-04
gateProfile: ["evidence-integrity", "test-quality", "mcp-tool-contract", "state-machine"]
dependsOn: []
mergeOrder: 71
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- 当前没有积分账本数据域、`points.py`、积分 display producer 或设备积分 view。既有可复用通路是 story_list 的 front 自推、display_schema 机械校验、OpenClaw 薄意图与设备 view 路由。
- 本变更新建“设备语音/QQ 记账生产者 → 云端持久账本 → front 权威推送 → 设备积分页”的端到端业务数据通路，因此 spike-first 命中。最高风险不是单个函数，而是真实生产者、线格式、持久状态和消费端能否一次串通。
- 本包只做最小闭环：一条真实设备完整语音记账句、首选两笔同形真实 QQ canary（第一笔设备在其他页、第二笔积分页在屏）、设备实际看到权威余额和原因，以及首尾两条经 `kidpal_points_page` 可及。完整账本规则、正式 UI 和正式 QQ skill 均不在本包提前扩张。

# 可行性判据

- 图索引已核实 front `_push_story_list` 到 `_push_display_payload` 的自推链、`validate_display` 白名单与控制动作门禁、设备 `kidpal_display_route` 的扩展点；`points.py` 无既有节点，新建不会覆盖旧 writer。
- K0a 与本包除唯一共享构建清单外文件面可解耦：K0a 留 P3 接管缝并先落 `firmware/main/test/host_app/main/CMakeLists.txt`，本包并行开发服务端和新增 points 文件，在 K0a merged、重取基线后追加该清单。因新页枚举与共享清单均来自 K0a，host build、device build 与真机终验必须在 K0a 合并后完成。
- 门 1 已裁定真实 QQ 最小 producer：真实 QQ 一次性明确指令 → 既有 main agent → K0 最小 add CLI，不建临时 skill；以 canary token 关联 QQ、front、points 与设备日志。禁止直编 state 文件、禁止终端调 CLI 冒充 QQ、禁止本地 OpenClaw 注入冒充、禁止提前建立 K3 skill。该裁定只证明路径可行，真实执行结果仍由本包 spike 证据判定。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **K0a 先合，K0 后合。** 两包可并行开发，但 K0 的 host build、device build、真机终验和 ready-to-merge 必须基于 K0a 合并后的代码；K0 此时追加唯一共享构建清单 `firmware/main/test/host_app/main/CMakeLists.txt`，不回改 K0a 的 render_node/keybar/story_page。
- **K1 在同一 `points.py` 上正式化单 writer。** 本包不得另建状态文件、第二套余额算法或长期 Spike 旁路；K1 负责锁、原子写、余额重算、两类幂等、删除和完整分页。
- **K2/K3 边界**：本包最小渲染不等于积分主页完成；QQ canary 不等于正式 QQ 通道完成。完整 UI/删除归 K2，完整加/兑/查/删与稳定定位归 K3。
- **相邻冻结锚拆包期预扫非零命中**：display_schema 既有 `start_organizing` 自推/relay 分叉、session_ctx 既有 home/schedule/story 行为和设备 `start_organizing` 路由均为保留面。新 view/动作只按契约 additive 扩展，既有归档精确七 view 断言不重开、不当作现役验收。
- front/AGENTS.md 与 K5 后续故事导航改动共享，但本列车串行合并；本包只增加积分薄意图，不改故事列表的 P4 导航语义。

# 铁律约束

- 通过证据的第一位是设备真实看到权威余额和原因。state 文件、CLI 成功、pytest 读回或 front 日志都只能作辅证，缺一真实消费端目击不得宣布 spike 成功。
- QQ 首选两笔同形真实 canary：第一笔在设备其他页发送并验证不抢前台，第二笔在积分页在屏时发送并验证原地刷新。若只能执行一笔，另一状态只能作为离线/宿主证据，不得宣称两种状态都由真实 QQ 链路验证。禁止人工直编 state 文件、终端调 CLI 冒充 QQ、本地 OpenClaw 注入冒充或提前建立 K3 skill。
- 模型只产 `add_points`/`redeem_points` 薄意图，不生成余额、分录 id、前后余额、revision、游标、资产标识或成功回执；front 依据真实执行结果答复与推送。
- 不完整句不落账。本包不实现正式兑换、删除、短窗去重或自定义目标，不用 spike 的名义放宽这些 K1/K2/K3/K4 的验收义务。
- 不改 `docs/contracts/**`、requirements 或 architecture；发现契约形状不足、必须新增写权或必须提前实现 K3 完整 skill 时立即停并升级。

# demo 证据形态（与 roadmap 一致）

- server 离线验收与 host 结构/行为证据：薄意图截获、完整 points_home 自推、restore 前台语义、分页首尾 canary 和设备路由。
- `spike-evidence.md`：真实设备语音与首选两笔同形真实 QQ 的状态化时间线，关联 canary token、front 命令、推送、设备路由/消费日志及看屏照片；若仅一笔，明确另一状态仅为离线/宿主证据。
- K0a 合并后的 host build、device build 与一轮真机终验；记录实际反馈时延但不替 NFR-PERF-4 拍数。
