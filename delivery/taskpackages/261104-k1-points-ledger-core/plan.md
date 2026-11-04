---
changeId: 261104-k1-points-ledger-core
title: 云端确定性账本核心——points.py 单 writer、原子写与重算、两类幂等、删除和目标存储、keyset 分页
rigor: full
riskTier: critical
sourceType: proposal
origin:
  ref: delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
  workstream: 261103-star-points-story-cover
reviewed-by: architect（/triage-change 分流 review 2026-11-03；门 1 待主线程另派）
date: 2026-11-04
gateProfile: ["evidence-integrity", "test-quality", "real-db", "state-machine", "mcp-tool-contract"]
dependsOn: ["261104-k0a-four-page-skeleton", "261104-k0-points-e2e-spike"]
mergeOrder: 72
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- K0 只证明最小端到端通路，尚未形成可长期承载双通道写入的账本核心。积分需要持久分录、余额与逐条前后余额、激活目标、revision 和命令回执，且删除、兑换和分页都有跨命令不变量。
- 现有 `story.py` 的 seq 账本只服务故事入册流水号，与积分余额是不同数据域，不能合并或改作积分 writer。front 和 QQ 若分别写文件或算余额，会形成双写者、并发覆盖和重复扣款。
- 本包把 K0 的 `points.py` 最小路径正式化为唯一确定性 writer，并补齐 front producer/CLI 共用面。完整设备 UI 归 K2，正式 QQ 交互归 K3，目标配置美术与切换演示归 K4。

# 可行性判据

- 架构已明确进程间锁、同一临界区读取最新状态→校验→重算→原子替换→保存回执；契约已明确删除隐式 ack、keyset 游标与 revision 重置，ADR-018 已排除双 writer、offset 和本地离线队列。
- 图索引确认当前没有 `points.py` 或既有积分 writer；front `_push_display_payload` 与设备 `kidpal_display_route` 是现成消费对端。新逻辑可用临时目录、注入时钟和真实 server pytest 做确定性验证。
- K0a/K0 均已合并是启动前置。K0 的真实消费证据提供通路基线，本包可集中验证账本不变量，不需要在本包重开 UI 或 QQ 全功能设计。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **唯一事实源与唯一 writer**：`points/state.json` 的内部结构只由 `points.py` 拥有；front API 与 K3 QQ CLI 都进入同一执行器，设备只显示权威结果，OpenClaw 不直接持写路径。
- **删除与兑换不是同一路径**：删除可令余额变负并重算后续前后余额；兑换仍要求当前余额不低于激活目标分值，且真实兑现和一次明确确认后才产生负向分录。不得用统一的“余额不得为负”钳位覆盖 OQ-1。
- **两类幂等分离**：稳定 command_id 处理传输重试；短窗语义去重处理用户重复表达，起步 10 分钟、注入时间验收。新的用户回合即使文案相同也不得复用旧 command_id。
- **分页不得复用故事窗口**：排序键为 `(occurred_at,id)` 时间倒序，契约默认 limit=6、允许 1–50；插入或删除改变 revision 时回最新首页并要求设备重置，不得跨版本拼接。
- **相邻冻结锚拆包期预扫**：`story.py`/`test_story_units.py` 的故事 seq 原子写是独立保留面；display_schema 与 record_dialogue_guide 的控制来源门禁不得因积分动作而放开设备盲转。未发现既有积分常量被其他包钉到互斥值，planner 定稿后仍须复扫。

# 铁律约束

- 余额与每条分录的变动前后余额全部由剩余分录派生；任何入口不得接收模型提交的权威余额、目标 id、分录 id、revision 或游标作为写入事实。
- 原子写必须覆盖命令回执与状态的一致性；同 command_id 重放返回首次结果，不新增分录、不递增 revision。任何失败不得留下半写状态。
- 四预置场景不可删，激活中的自定义目标不可直接删，自定义分值只接受 5–200 正整数；切目标不自动扣分，历史兑换名目保持原快照。
- 不实现设备本地账本、NVS/SD 积分副本或离线写队列；不把 story_list 16 条窗口、offset 或模型模糊相似分带入账本。
- 不改 requirements、architecture、`docs/contracts/**` 或故事数据域；发现需要改线格式、分页规则、短窗匹配语义或新增第二 writer 时立即停并升级。

# demo 证据形态（与 roadmap 一致）

- server pytest：原子写、余额重算、兑换/删除不变量、负余额、两类幂等、目标配置和失败路径的 RED→GREEN 与变异证据。
- keyset producer 的首页/续页/最早页/版本重置/非法游标/空账证据，并由真实消费端复核，不以生产者读回单独收口。
- K0 通路的回归证据与 front/QQ 共用 writer 的可观测日志；本包不占用独立真机 UI 配额，消费端复核结果交给 K2/K3 继续复证。
