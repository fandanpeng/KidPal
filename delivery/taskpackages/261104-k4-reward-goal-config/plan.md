---
changeId: 261104-k4-reward-goal-config
title: 奖励场景与激活目标——四预置美术、QQ 自定义/切换/删除、唯一激活与刻度/兑换联动
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
  workstream: 261103-star-points-story-cover
reviewed-by: architect（/triage-change 分流 review 2026-11-03；门 1 待主线程另派）
date: 2026-11-04
gateProfile: ["evidence-integrity", "test-quality", "state-machine", "mcp-tool-contract"]
dependsOn: ["261104-k1-points-ledger-core", "261104-k2-points-device-ui", "261104-k3-qq-points-channel"]
mergeOrder: 75
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- K1 已有奖励目标的合法存储与校验，K2 已能显示默认游乐场 40 的进度线，K3 已有正式 QQ skill；但四预置场景美术、自定义通用旗标、QQ 目标管理和切目标后的完整联动尚未交付。
- 奖励目标不是单纯配置项：它决定兑换金额、进度线刻度和场景标，并且切换不能自动生成兑换分录。删除激活目标会破坏“恒有目标”的单页进行中口径，必须执行 OQ-2 的先切换再删。
- 本包在这三包已合并的基线上完成场景与目标闭环。书封 `ic-coin` 和故事导航不在本包，归 K5 使用同一资产管线后序承接。

# 可行性判据

- 架构 §5.5.2 与产线实读确认可沿 `gen_assets.mjs`→`gen_art_spiffs.mjs`→`spiffs_root` 产出 KAP1 资产；契约已冻结五个资产标识，V9 已定稿摩天轮 `sc-wheel`，不存在资产选型待裁。
- K1/K2/K3 已分别提供配置存储、渲染缝和 QQ 通道，本包可对自定义边界、切换、删除与跨端联动做端到端验证，无需重开账本或通道设计。
- server 可验目标规则和历史不变量，host 可验刻度/资产，真实 QQ 与真机可完成家长操作及孩子看屏终裁。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **K1→K4**：目标配置仍由 points.py 单 writer 执行；本包利用 K1 存储和校验，不复制出第二套目标库。切目标只改激活配置，历史兑换名目保持原快照。
- **K3→K4**：目标命令扩展同一 `kidpal-points-write` skill，沿用一次性可判、普通错误回复和禁 ask_user；不另建 QQ 配置助手。
- **K2→K4**：restore 更新不抢前台，积分页原地刷新。K2 的四态、负余额和默认游乐场 40 是回归基线，自定义/切换只能 additive 扩展。
- **K4→K5 资产串行**：本包产 `sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag`，K5 后序补 `ic-coin` 和书封；两包不得并行改生成器、资产清单或生成件。
- **相邻冻结锚拆包期预扫非零命中**：K1 目标不变量、K2 默认目标与页面、K3 QQ 行为、KAP1 格式及既有资产闭集均为保留面。V9 已明确 `pt-cat` 复用且不新增字体/分区；未发现同一资产标识被钉到互斥语义。

# 铁律约束

- 预置清单恒为巧克力 20、看电影 30、游乐场 40 默认、礼物 50；预置不可删，自定义只接受 5–200 正整数，激活自定义必须先切换再删。
- 同刻只能有一个激活目标，不出现“暂无目标”页；设备端不设目标，家长配置只走 QQ。
- 切目标不自动扣分，之后兑换只认新目标分值；旧目标分值拒答且不写账。目标配置命令仍共享 K1 的幂等与原子写。
- 资产标识只取 `sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag`，自定义恒用 `sc-flag`；不得根据奖励名猜图、为每个自定义目标生成新美术或回用已淘汰的 `sc-castle`。
- 不新增字体、分区或设备设置页，不修改生成后的二进制文件；不改 requirements、architecture、`docs/contracts/**`，需要新资产标识或线格式时立即停并升级。

# demo 证据形态（与 roadmap 一致）

- 资产管线日志与 KAP1 机检：四预置和通用旗标从 v9 母版产出，host/设备装载一致，既有资产零回归。
- server pytest + host runner：5/200 边界、唯一激活、禁删、切换不扣分、历史快照、刻度伸缩与新分值兑换的 RED→GREEN 和变异证据。
- 真实 QQ + 一轮真机：自定义/切换/删除全过程、其他页不抢前台、积分页场景标和刻度同步；V9 design review 与资源预算记录在场。切目标刷新顺带记录与 NFR-PERF-4 相同的“QQ 消息→积分页原地刷新”实测时延字段，不写死阈值。
