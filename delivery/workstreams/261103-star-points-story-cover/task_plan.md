---
workstream: 261103-star-points-story-cover
slug: 261103-star-points-story-cover
status: active
opened: 2026-11-03
closed:
summary: 星星积分本（金币账本：单页进行中、设备语音 + QQ 双通道记账、可配置奖励目标）+ 故事封皮与四页导航（主页面 3→4、故事模块下沉 P4 封皮子页）；契约 9 view / 薄意图 / 删除与 keyset 分页私有帧随批已落
owner: main-thread
area: systems
changes:
  - 261104-k0a-four-page-skeleton
  - 261104-k0-points-e2e-spike
  - 261104-k1-points-ledger-core
  - 261104-k2-points-device-ui
  - 261104-k3-qq-points-channel
  - 261104-k4-reward-goal-config
  - 261104-k5-story-cover-nav
---

# task_plan · 261103-star-points-story-cover（workstream 级分波交付计划）

> 本件 = workstream 拆包顺序链必备节（delivery/workstreams/README.md「拆包顺序链」纪律）。设计权威 = [PROPOSAL.md](PROPOSAL.md)（FROZEN v1.0，裁定 D-01..D-10 + OQ-1/2/3 human 裁定）；评审依据 = [review.md](review.md)；里程碑登记 = `delivery/milestones/milestones.jsonl` M7-star-points-cover（2026-11-04 补登，state=planning）；上游 anchors = 需求 v1.71 frozen + 架构 FROZEN v1.0（ADR-018）+ 契约五触点 LANDED（spine 3/4，imp 随包回填）。

```yaml
workstream: 261103-star-points-story-cover
phases:
  - id: K0a
    name: 四页骨架（P3 积分占位 + P4 故事迁移 + 枚举驱动语境映射）
    scope: 页枚举 3→4（P3=积分本占位页、P4=故事页原地迁移，故事暂仍平级页、封皮留 K5）；keybar 4 页点；BOOT 四页循环；story_page source_page 数值 case 2 改枚举驱动（KP_PAGE_*+子页状态，R-09 承重接线点）；TECH-261028 阅读键缺陷修复前移至此；P3 占位页 = S-F6.1 数据未达空态口径静态实现（无业务逻辑）
    changeManifest: firmware/main/display/kidpal_render_node.h、firmware/main/display/kidpal_render_node.cc、firmware/main/display/keybar.c、firmware/main/key_gesture_parser.h、firmware/main/display/story_page.c（语境映射面 + TECH-261028 阅读键位）、相关冻结验收更新
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: K0
    name: 端到端最小 spike（记账通路真值验证）
    scope: 服务端为主——points.py 最小写入路径、front 推送 points_home 帧、OpenClaw workspace-kidpal AGENTS.md 薄意图（add_points/redeem_points）、真实设备语音一句完整记账句 + 首选两笔同形真实 QQ canary（第一笔其他页、第二笔积分页在屏）、首尾两条过 kidpal_points_page 分页、消费端可观测日志；设备端 = 新建 points_page.c 消费 points_home 帧最小渲染；两包唯一共享构建清单 = firmware/main/test/host_app/main/CMakeLists.txt，K0a 先落、K0 在 K0a merged 后追加；证据落本包 spike-evidence.md，以设备真看到权威余额与原因为通过依据（不以文件写入/pytest 读回自证）
    changeManifest: server/gateway/points.py（最小路径）、server/gateway/display_schema.py、server/gateway/front/front_app.py（推送接线）、server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md（薄意图规则）、新增 firmware/main/display/points_page.c（最小渲染）、firmware/main/test/host_app/main/CMakeLists.txt（两包唯一共享构建清单，K0a 先落、K0 后追加）、新增 spike-evidence.md
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: K1
    name: 云端确定性账本核心（points.py 单 writer 正式化）
    scope: 原子追加分录、余额重算、兑换不变量（余额 ≥ 激活目标分值、真实兑现后才扣、同一次兑换不重复扣）、短窗语义去重（起步 10 分钟、验收注入时间）与传输幂等分离、删除规则（删除重算允许负余额 = OQ-1；兑换仍要求余额 ≥ 目标分值）、奖励场景配置存储（四预置 + 自定义 5–200 = OQ-3、禁删激活目标 = OQ-2）、keyset 分页 producer、断网不本地补写
    changeManifest: server/gateway/points.py、server/gateway/display_schema.py（shape floor 收紧）、server/tests 或 server/acceptance 新增账本验收、相关部署面
    riskTier: critical
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: K2
    name: 设备积分本（points_home + points_ledger 全页与语音闭环）
    scope: points_page.c 占位换正式——积分主页（余额/激活目标进度线/小伴位置/最近三条/负余额如实显示 = S-F6.1 AC5）+ 详细账本页（分录列表/keyset 取页/删除确认整屏覆盖层 = S-F6.2）、积分键位（KEY 短按积分语境语音/双击详情/长按回首页）、语音记账正反验收（完整句落账跳页、不完整句不落账 = S-F6.3）、目标分值不匹配温和拒答、冷启动恢复、设备删除上行 kidpal_points_delete
    changeManifest: firmware/main/display/points_page.c、firmware/main/display/points_page.h（新增）、firmware/main/mcp_server.cc（积分 view/删除上行消费）、firmware/main/application.cc（session_ctx 积分语境单点）、新增冻结验收
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: K3
    name: QQ 家长账本通道
    scope: workspace-main 新积分 skill（仿 kidpal-schedule-write 机检/marker 工程形态，账本用 points.py 单 writer、不碰 schedule.md）；QQ 加/兑/查/删与语音同规则（S-F6.4）；稳定记录 id/序号定位、不调用 ask_user（TECH-261030 硬约束）、普通 QQ 错误回复、其他页到账不抢前台；真实 QQ 验证含歧义输入不卡死
    changeManifest: server/deploy/openclaw-skills/workspace-main/（新增积分 skill）、server/gateway/points.py（QQ 入口校验位，若需）、相关部署面
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: K4
    name: 奖励场景与激活目标配置
    scope: 四预置场景（巧克力20/看电影30/游乐场40默认/礼物50）+ 场景 1bpp 美术出资产（走既有资产管线）、QQ 自定义/切换/删除（OQ-2/OQ-3 边界 = 禁删激活目标、5–200 正整数）、唯一激活目标、自定义通用标记图、分值刻度伸缩、切目标后兑换只认新分值（S-F6.5）
    changeManifest: server/gateway/points.py（目标配置面）、firmware 资产分区/装载（场景图与通用标记）、firmware/main/display/points_page.c（刻度伸缩渲染）、相关验收
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: K5
    name: 故事封皮导航（封皮 + 故事模块下沉 + 逐层返回）
    scope: 封皮页（书封美术/标题/星星计数复用 story_list total/键帽「打开/下一页」= S-F2.6）、故事列表/阅读下沉 P4 子页、逐层返回（阅读→列表→封皮→首页）、语音「打开故事」只到封皮（「打开详细账本」直进 P3 详细账本）、覆盖层页点消失口径；TECH-261028 已前移 K0a（登记消费关系，不重复修）；K5 兼导航并集收口 owner
    changeManifest: firmware/main/display/kidpal_render_node.h/cc（封皮层）、firmware/main/display/story_page.c/h（下沉与返回层级）、新增 story_cover 页面件、keybar（封皮键帽）、story_list 消费对齐契约 §1.3③、相关冻结验收更新
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
```

## 顺序与 blocking edges

- **K0a ∥ K0 并行（本列车唯一 WIP=2 对），merge order 钉死 K0a 先合**——K0 的固件件（points_page.c）引用 K0a 的页枚举新值，且 `firmware/main/test/host_app/main/CMakeLists.txt` 为两包唯一共享构建清单；K0a 先落，K0 在 K0a merged、重取基线后追加自己的登记。K0 仅服务端与新增 points 文件并行开发，其 host build、device build、真机终验和 ready-to-merge 均排在 K0a 合入之后。
- K0a/K0 合并后回 WIP=1：**K0 → K1 → K2 → K3 → K4 → K5** 串行（六包共享 front/prompt/render_node/keybar，R-13 互踩防线）。
- K1 → K2 / K1 → K3 为内容依赖；K3 排 K2 后（真实 QQ 验证与设备端联调顺序）；K4 依赖 K1–K3（自定义目标管理与 QQ 通道耦合）；K5 依赖 K2（积分页在 P3 落定后再动共享导航面）。
- 共享面串行对账：TECH-261022-p3-record-session-state-churn 有独立承接包，与本列车共享 keybar/故事页文件——与其 merge order 串行，不吸收其 scope。
- launchPreconditions：① 上游 phase 完成（按顺序链）② anchors.req（v1.71）+ anchors.arch（§12 行 + ADR-018）+ anchors.contract（CDQ-261103 五触点）已回填（现状满足）③ 触 UI 包 UI-ready = requirements §9 v1.71 行 + brief v8 对齐注记（均在位）④ K0 为 spike 包，其证据纪律见 PROPOSAL §4 / review.md D 节。
- 真机轮配额：K0a（四页循环）、K0（spike 终验）、K2（积分页）、K4（场景与刻度）、K5（导航并集 = workstream 收口轮，PROPOSAL §5）各留一轮；跨包端到端收口轮由主线程另组织。
