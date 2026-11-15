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

## 技术方案（planner+architect 预评）

> 2026-11-15 · architect ARCH-ADVISORY（dev-loop Step 1g 技术预评档，14 次工具调用）。只出意见、不豁免任何闸、不构成通过判据。planner 已核预评未决项：v9 母版五个符号 `sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag` 均在位（`docs/ui/kidpal-ui-design-v9.html` grep 各 1 命中）→ `docs/ui/**` 不进 manifest；渲染缓存取**方案 A**（3 槽 + 按名 strcmp 重载）；子命令取**扁平动词**。

### A. 渲染落点（硬结论）

- **必须动 `firmware/main/display/kidpal_render_node.cc`（host 不可编）**：`points_page.c` 只做载荷解析与决策表（`:485-536` 已解析 asset，`:948-959` 已按 target_points 算 overshoot），真正画图元的是 render_node.cc 的 P3 积分本渲染区（≈`:888-1000`）；场景槽 `RN_POINTS_ART_SLOTS 3` 的命中判据是 `slot->name == asset` 指针相等，只认编译期字面量 `sc_wheel_42_42`。
- **压缩 device-only delta**：五键 asset→bin 名映射与刻度几何 helper 下沉到 host 可编的 `points_page.c/.h`（如 `kidpal_points_scene_asset(const char *target_asset)`，未知标识返回 NULL → 不画，绝不猜图）；`kidpal_render_node.cc` 只改两点：把 `sc_wheel_42_42` 字面量换成映射结果；场景槽命中判据改 `strcmp` 并允许名字变化时重载。
- **合并门申报**：manifest 必须新增 `kidpal_render_node.cc`；launch-card deviceOutcome 必须 `userVisible:true`、`hostUncompilablePaths:["firmware/main/display/kidpal_render_node.cc"]`、acceptanceRefs 非空（本包设备轮 id），并在合并前落 `device-round-evidence.md`。该路径已在 `verify-gates.deviceOutcomeGate.hostUncompilablePaths` 登记集内，无需新登记。
- **缓存方案 A 的代价**：切目标时旧场景槽重载（RAM 零增长），打破注释里「零逐出」的字面口径；配套 acceptance 断言「切目标后旧场景标不再出现在帧里」，RAM 资源预算写进真机轮证据。

### B. 资产管线（只新增 4 枚）

- **`sc_wheel_42_42.bin` 是 K2 已交付件**（`gen_assets.mjs:451`、`gen_art_spiffs.mjs:77` WHITELIST、磁盘实存），本包不新增、不改名、不进 manifest。
- 新增 `sc_choco_42_42` / `sc_movie_42_42` / `sc_gift_42_42` / `sc_flag_42_42`（42×42、rotate 0、kind 'svg'），步骤：① `gen_assets.mjs` sc 段追加 4 job（`svgUse('<id>')`）；② 跑 `node tools/asset-pipeline/gen_assets.mjs` 重生成 `assets.c/h`；③ `gen_art_spiffs.mjs` WHITELIST 纯追加 4 名 + 头部计数 78→82；④ 跑 `node tools/asset-pipeline/gen_art_spiffs.mjs` 落 bin，`WROTE <n> KAP1 assets ...` 即管线日志。
- **命名律**（`gen_art_spiffs.mjs:37-39`）：条目名 = assets.h 符号名 = `<name>.bin` 根名。契约里的 `sc-choco` 是逻辑标识，落盘是 `sc_choco_42_42.bin`。
- **KAP1 机检**：无独立校验脚本 → 在新 host acceptance 直读 spiffs_root 断言：magic `"KAP1"`、format=1、w=h=42、stride=6、文件长 268、墨位计数 >0 且 <全墨。四枚新件逐枚断言，`sc_wheel_42_42.bin` 只读断言。
- **既有资产零回归**：提交前 `git status --porcelain firmware/main/assets/spiffs_root` 只允许 4 个 `A`、0 个 `M`；`assets.c/h` diff 纯追加（删除行 0）。任一既有 bin 变动 = 回归，停手升级，不合理解释。

### C. CLI / skill 扩展（扁平动词）

- `target-add --name TEXT --points N --command-id ID`
- `target-switch --target-id ID --command-id ID`
- `target-delete --target-id ID --command-id ID`
- 列举复用既有只读 `targets`（不新增 `target list`）。
- 回执单行 `key=value`，**target 回执禁带 `delta` / `entry_id`**（防误读为记账）；`target-switch` 回显 `balance=...`（= 没扣分的可对账事实）。
- 校验分工：`--points` 复用 `_parse_cli_int`；**5–200 范围不重复实现**，交给 `add_custom_target` 单源；未知/重复/缺值旗标 → `_UsageError` exit 2；预置禁删、激活自定义禁删、未知 target id → exit 1（`points.py:872-890` 已实现）。
- **SKILL.md 必改**：§8 末条「目标的新增/切换/删除不在本 skill 面」是**硬阻断句**，不改则 QQ 侧必然拒答，必须改写为目标配置纪律；frontmatter description 扩到覆盖目标管理；§2 表加三行；§5 一次性报错表加「删预置 / 删激活中自定义（先切换）/ 目标名空 / 分值出 5–200」；新增「切目标前先用 `targets` 拿 id，不得自编 target id」。

### D. 数据不变量：已成立 vs 要补

- **已成立（K4 只补测试）**：`set_active_target` 只写 active_target + revision+1、零分录；历史 reason 快照不变；`redeem_points` 只认激活分值；`add_custom_target` 的三关校验在 `_critical` 之前（越界零写入）；唯一激活由标量 active_target 承载；预置常量 + 禁删激活；`targets`/`active_target` 非出厂态才落盘（wire additive）。
- **要补**：`target-*` 三子命令与回执/usage；SKILL.md 改写；asset→bin 映射；render_node 按名重载；**切目标前后 `balance` 与 `len(entries)` 不变、`revision` +1** 必须双端断言（server + host），这是「切目标不扣分」的唯一可判形态。
- **builder 第一步**：读 `front_app.py::_push_display_payload` 的触发条件——若按 revision 变化推，则 target 动词天然生效（front_app.py 可能零 diff，manifest 保留即可）；若按命令白名单推，必须扩。

### E. CMakeLists 纪律

新 host acceptance 注册到 `firmware/main/test/host_app/main/CMakeLists.txt` 时**必须置于 host smoke acceptance 之前**（smoke 恒末位）；`points_page.c` 已登记，核而不重复加。

### F. 明确禁改

- `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`docs/ui/**`（v9 母版符号已齐，不许改母版）。
- K5 面：`ic-coin` 封皮资产与故事导航；本包不得改 K5 的生成器/清单范围。
- 既有 `sc_wheel_42_42.bin` 与其它 78 枚既有资产：只读。
