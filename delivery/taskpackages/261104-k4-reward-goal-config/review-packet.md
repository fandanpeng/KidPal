# Review Packet · 261104-k4-reward-goal-config

> 包：奖励场景与激活目标配置（S-F6.5 AC1–AC4 + S-F6.1 AC2/AC3 + S-F6.3 AC3 + S-F6.4 AC4/AC5）
> base ref：`cad07f2c`（K3 未合并 HEAD，包级 diff 恒用此 ref）｜集成 HEAD：`e87310de`｜worktree `C:/AI/KidPal-pod-m2`，分支 `dev/pod-m2`
> 本 Packet 供 supervisor 机械闸 + 人工复核；不自评通过。

## Change Size

- **Files changed**：18 个实现/工具文件（`firmware/` 8、`server/` 6、`tools/` 4；`git diff --numstat cad07f2c..HEAD -- firmware server tools`），另 delivery 簿记件若干不计入实现面。
  - 新资产 4 枚：`firmware/main/assets/spiffs_root/sc_{choco,movie,gift,flag}_42_42.bin`（各 268 B）
  - 生成件：`tools/simulator/LvglWindowsSimulator/assets/assets.c` +1207（管线自动生成，非手写）
- **Net diff**：**+3207 / −49**（firmware + server + tools）；不含 `delivery/`。
- **Scope**：云端目标配置三动词（target-add/switch/delete）与幂等回执、`read_state` 两键纯 additive、SKILL.md 目标管理段；固件五键 asset→bin 单源映射 + 刻度几何 helper + 渲染场景槽按名重载；资产管线两段纯追加 + 模拟器资产再生成；host 验收 5 个 TEST_CASE + server 验收 9 个 + CLI 单测。
- **Split recommendation**：**不拆**。资产产出与消费面（`kidpal_points_scene_asset` → render 槽）必须同批落地，否则设备上会出现「契约标识合法但 bin 不存在 → 场景标恒不画」的中间态；K5 的 `ic-coin`/封皮已由 `decisionCoverage` 明确出包（`package:261104-k5-story-cover-nav`），无越界。

## Verification Evidence

- **final 轮承重 fact**：`delivery/taskpackages/261104-k4-reward-goal-config/verifier-evidence/rounds/r1.json`
  - phase=`final`，round=`1`，invocationId=`7d3f9c1e-5b2a-4e8f-9a61-c2d4b8f0a7e3`，observedGateExit=0，`ok:true`
  - 机械复核：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261104-k4-reward-goal-config --phase final --round 1 --invocation-id 7d3f9c1e-5b2a-4e8f-9a61-c2d4b8f0a7e3` → exit 0，`factOk:true`
- **test-scope 清单落盘路径**：`delivery/taskpackages/261104-k4-reward-goal-config/test-scope-final.txt`（12 条测试文件 + 非测试件登记与排除项留痕）
- **范围圈定依据**：`detect_changes`（project `C-AI-KidPal-pod-m2`，since=`cad07f2c`，merge_base=`cad07f2c`，direction=both，depth=2）→ 31 changed files / 157 seed 符号 / 48 impacted 节点（server/gateway 27、firmware/main 13、server/tests 1、builtins 7）× taskpackage `impactedTestsSeed` 11 条合成；per-Pod 专属图索引可用 → 正常圈定（未走降级路径）。
- **Structural gates（final r1 逐闸）**：taskpackage-required PASS / change-manifest PASS（pre-cutoff 豁免声明在场）/ firmware-build PASS（真编译，`xiaozhi.bin` 生成）/ contract-lint PASS（仅一条既有断链 warning）/ acceptance-coverage PASS（14 id）/ mutation-check PASS（14 条 perAcceptance）/ docs-sync PASS / security PASS（`securityReviewRequired=false`）。结构闸结果不复述为 finding。
- **Acceptance / unit**：
  - unit 闸（scope 交集）：**110 passed**，1 warning（`asyncio.iscoroutinefunction` DeprecationWarning，既有）。
  - acceptance 闸（scope 交集）：**41 passed**，1 warning（同上）；oracle 面 `firmwareAcceptanceFiles=5`、`serverAcceptanceFiles=3`。
  - 本包 server 冻结件 11 passed（planner Step 10 复核 + builder 变异逐条复跑）；K0 spike unit 51 passed（扩单修正后）；K3/K0/K2 冻结回归 40 passed 零新增红。
- **host runner 全量（integrator 独立实跑，K0 锚命令 `wsl -d Ubuntu-24.04 … ./espidf_wsl.sh build && ./build/host_infra_test.elf`）**：
  - 结果 = **292 Tests / 3 Failures / 0 Ignored**（289 条 PASS 行）。
  - **3 红全属 `p1_auxbar_style.acceptance.c` 存量族**：AB-A1（`ic_batt_15` 消费点未接入，仍走 ic_batt_48 缩放旧路径）、AB-A2（`ic_thermo_12` 落位漂出授权域）、AB-A4（`ic_wifi_14` 消费点缺失）。与 K2 final 基线 **287 Tests / 3 Failures（同三条）** 同族同项；292 − 287 = 5 = 本包新增 K4G-H1..H5，**五条全 PASS**（日志逐条可见 `reward_goal_config.acceptance.c:265/313/386/436/487` PASS）。⇒ **K4 零新增红**，3 红为既有 infra 债（不归本包修，见 Residual Risk）。
- **Mutation check**：13 处真实变异覆盖 14 个 acceptance id，逐处还原复跑；A1/A7 复原后仍红暴露冻结件自身缺陷，已由 acceptance-author 最小修正并重冻（不改实现）。
- **Docs sync**：`docsSyncRequired=false`；人工复核成立，详见 `docs-delta.md`（requirements / architecture / contracts / docs/ui 四文件零 diff）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | server · `points.py#_store_target_receipt` | 病态撞键（调用方 command_id 字面量恰好以 `target:` 开头且同键已落分录回执）时，目标回执降级为「不持久」，下次重放会当普通执行（可能二次 bump revision）。已有 `log.warning` 且与既有 delete 同款处理。 | changed-code hunk `_store_target_receipt`；heuristic = 传输幂等（分录回执）优先于目标命令回执持久，降级方向保守不损账本 | 无本批动作；如未来 command_id 命名空间收敛，再统一前缀协议 |
| NIT | judgement-call | firmware · `kidpal_render_node.cc#rn_points_art_dsc` | 失败记忆 `slot->name = asset` 保存的是调用方指针而非副本；当前三处调用点均传编译期字面量（`pt_cat_34_28.bin` / `ic_coin_36_36.bin` / `kidpal_points_scene_asset()` 返回的静态串），无悬垂。 | changed-code hunk `rn_points_art_dsc`（`slot->name = asset` 两处）；heuristic = 生命周期依赖调用方约定，非所有权转移 | 无本批动作；若将来允许动态拼接资产名，改用具名枚举槽或拷贝 |
| NIT | judgement-call | tools · 资产管线 | 整文件重生成 `assets.c/h` 会顺带漂 `ic_wifi_14` 母版 map 与 `p1_chip_mingzao.bin`（K2 pod-retro 同族 tech-debt）；本包以「纯追加 + 还原」守住 spiffs_root 4A/0M，但生成器缺陷本身未修。 | changed-code hunk `gen_assets.mjs` / `gen_art_spiffs.mjs`；deletion test = 去掉追加段后漂移仍在，说明缺陷在生成器而非本包改动 | 不新建债务条目（K2 已记同族）；见 Residual Risk |

无 BLOCKING / SHOULD finding。UI finding 按 `code-review-gate.md` 既有严重度解读，无 BLOCKING（见 Design-Review Evidence）。

## Five-Axis Review

1. **Correctness**：S-F6.5 AC1–AC4 逐条落地——四预置常量与出厂激活游乐场 40（K4G-H4 回归）、自定义 5–200 边界与越界一次性报错（K4G-A1，越界 state 零写入由 A1 变异证明）、唯一激活标量单点（A3）、切目标不扣分且 `revision+1` 零新分录（A4）、切后只认新分值且历史 reason 快照不变（A5/A6）、预置禁删 / 激活中禁删先切换（A2）。刻度随激活目标伸缩与越目标延伸（H2 两档 5/200 + 非法入参 −1）。边界条件：负余额钳位只在决策表一处（render 侧不二次判断，单一真相源），未知标识返回 NULL 不猜图（H1）。
2. **Test quality**：14 个 acceptance id 全部有真实断言（server 直接读 state 文件核查零写入与 revision；host 直读 spiffs_root 逐枚校验 KAP1 魔数/长 268/42×42/stride 6/墨量区间）。13 处变异逐条使目标断言转红并复原（A1/A7 两处复原后仍红暴露冻结件缺陷 → 修正重冻），说明断言判别力真实；`test_missing_file_is_zero_state` 的投影 equality 扩单经 architect APPROVE、只动该测试体。
3. **Contract/boundary safety**：`docs/contracts/**` 零 diff；`read_state` 追加两键为纯 additive（K0 存量精确三键断言改为投影 equality，属扩单先例而非放松实现）；回执单行 `key=value`、target 三动词禁 `delta=`/`entry_id=` 由 A7 机检；未新增依赖、未跨层调用（CLI 只做解析与回执装配，范围校验单源在 `add_custom_target`）。
4. **Operability/security**：所有目标命令回执 / 重放 / 拒绝均带 `corr_id` 结构化日志；错误分界 exit 2（旗标解析/缺值）与 exit 1（语义拒绝 + stderr）由 A7 钉住；`command_id` 前缀命名空间隔离三族回执防串味；无 secret、无外部输入直入渲染（asset 标识走白名单映射，未知即不画）。
5. **Maintainability/simplicity**：改动局部——`points_page.c` 只追加两个 helper（零触碰 K0/K2 既有实现），`kidpal_render_node.cc` 只改槽命中与场景标取图两点；映射与刻度下沉到 host 可编层使 device-only delta 压缩到最小；未发现过早抽象（无单实现的接口层、无未用 config）。`simplicity` tag 未命中（无 `yagni`/`delete`/`shrink` 项）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| — | diff 内无 `minimal:` 标记 | — | no-trigger |

## Design-Review Evidence（纯文本；本包 `uiux.designReviewRequired=true`，仓内不存截图/HTML）

- **V9 design refs 覆盖**：`docs/ui/V9/points-home.png`（常态帧：积分本标题 / 金币余额 / 进度线 + 小伴在线上 + 目标位场景标 / 最近得到三行 / 底部四页点）、`points-home-empty.png`（空态）、`points-home-negative.png`（负余额）三帧均在 `docRefs`。本包改动集中在进度线中段——场景标取图与刻度几何，直接对应 points-home 常态帧；empty / negative 两帧为 K2 既有结构，本包以 K4G-H4 回归保证零偏移（不新增该两帧状态）。
- **美术来源与尺寸**：四枚场景标 `sc-choco`（巧克力 20）/`sc-movie`（看电影 30）/`sc-wheel`（游乐场 40，沿用 K2）/`sc-gift`（礼物 50）与自定义通用旗标 `sc-flag`，全部来自 `docs/ui/kidpal-ui-design-v9.html` 内嵌 SVG symbol（`svgUse('<id>')`，五符号在母版各 1 命中），经既有预栅格化 1bpp 管线产出 **42×42、stride 6、每枚 268 B** 的 KAP1 件；场景标悬空纪律 = 底边距基线 12 px、42 宽中心对目标刻度位，与 V9 一致。
- **资源预算**：新增 4×268 B = **1072 B** 进 SPIFFS art 分区（art 512 KB 分区内占比可忽略，不触 assets 8 MB 分区）；渲染 RAM **零增长**——方案 A 三槽固定分工（`pt_cat` / `ic_coin` / 场景标），场景标按名 `strcmp` 命中、换目标时释放旧像素缓冲后复用同一槽，峰值与 K2 持平（每槽 42×42×4 B ≈ 7 KB，仅在用时分配）。
- **design-drift 摘要**：1bpp 无灰度是 RLCD 硬约束下的既定设计红线，矢量母版 → 1bpp 量化差异属预期；形状、位置、尺寸、悬空纪律与 V9 对齐，未发现版式漂移。**未做像素级逐点比对**，也未在真机上做目视确认（见未覆盖面）。
- **viewport / a11y 摘要**：单目标视口 400×300、1bpp、无滚动；目标名与分值以文本 + 刻度标签（起点/10/20/30/40…）承载，场景标属装饰性强化，不构成唯一信息载体（a11y 可读性不依赖图标辨识）；正文 16px 档维持既有可读性下限。
- **external artifact reference**：`docs/ui/V9/points-home.png`、`docs/ui/V9/points-home-empty.png`、`docs/ui/V9/points-home-negative.png`（仓外真机截图待 K4G-L1，bootstrap 期只留引用不落盘）。
- **residual risk**：真机目检（四预置辨识度、`sc-flag` 旗面在大分值/越目标版面的观感）未跑。

## Demo 对账（roadmap `delivery/workstreams/261103-star-points-story-cover/roadmap.md#261104-k4-reward-goal-config`）

| roadmap 条目 | 内容 | 本包当前证据 | 状态 |
|---|---|---|---|
| 效果段 | 孩子能看到巧克力/看电影/游乐场/礼物四种目标美术；家长能从 QQ 新增带通用旗标的自定义奖励并切换激活；40 切到其他分值后刻度随之变化、之后兑换只认新分值；切换本身不扣分；激活中的自定义目标必须先切换才能删除 | 四枚资产落盘 + KAP1 机检（H3）；五键映射（H1）；刻度两档伸缩（H2）；自定义边界/唯一激活/禁删/先切换再删（A1–A3）；切目标不扣分与切后兑换（A4–A6）；出厂态回归（H4） | 离线/宿主面 **达成**；真机看屏面 **待 K4G-L1** |
| 证据形态 | 资产管线/KAP1 机检 + server pytest + host runner + 真实 QQ 配置全过程 + 一轮真机场景标/刻度/大分值/restore 看屏证据；切目标刷新顺带记录「QQ 消息→积分页原地刷新」实测时延字段，不写死阈值 | 前三项 **已具备**（H3 机检、41+110 pytest、host runner 292/3 全存量）；后两项 **未跑** | 部分达成，缺真机轮 |

## 未覆盖面申报（逐条；均为「未跑」，待 human 设备窗 K4G-L1；ECS 部署轮归主线程）

1. **真实 QQ 目标配置全过程** —— 未跑：新增自定义（5/200 边界内）、出界拒绝、切换唯一激活、非激活自定义删除、预置删除拒绝、激活自定义先切换再删的真实 QQ 会话。
2. **真机看屏（四预置 + sc-flag + 200 大分值/越目标）** —— 未跑：四枚预置场景标逐枚目视、通用旗标目视、大分值与越目标版面。
3. **切目标后旧场景标消失** —— 未跑：真机上切目标后旧场景标不得残留在帧里（离线由 host 结构断言覆盖，非看屏）。
4. **其他页不抢前台 / 积分页原地刷新** —— 未跑：切目标时设备在其他页不被抢屏、积分页在屏时原地刷新。
5. **NFR-PERF-4 同款时延字段** —— 未跑：「QQ 消息 → 积分页原地刷新」实测时延字段记录（不写死阈值）。
6. **RAM 资源预算实测** —— 未跑：真机运行时堆/PSRAM 峰值与三槽缓存占用实测（设计上是零增长，未实测）。
7. **V9 真机 design review** —— 未跑：以真机帧对 V9 design refs 做目视比对。
8. **设备轮证据** —— 未跑：`device-round-evidence.md` 未落。本包已申报 `deviceOutcome{userVisible:true, acceptanceRefs:[K4G-L1], hostUncompilablePaths:[firmware/main/display/kidpal_render_node.cc]}`，**前置升级闸将因此 BLOCK 至真机轮完成**；不伪造 PASS。

## Residual Risk

- **资产管线重生成漂移（不重复立账）**：整文件重跑 `gen_assets.mjs` → `assets.c/h` 会顺带改变 `ic_wifi_14` 母版 map 与 `p1_chip_mingzao.bin`，与 K2 pod-retro 记录的 tech-debt 同族；本包以纯追加 + 还原守住 spiffs_root **4 新增 / 0 修改**，但生成器缺陷未修。若后续包再跑整文件重生成，仍有踩中该漂移的风险。
- **p1_auxbar 三条存量红（AB-A1/A2/A4）**：与 K2 final 基线同族同项，属既有 infra 债（`ic_batt_15` / `ic_thermo_12` / `ic_wifi_14` 消费点或落位），本包零新增；修不修由既有债务裁决，不在本包范围。
- **真机面全空（K4G-L1）**：见未覆盖面 1–8；`pre-merge-escalation-gate` 的设备轮子判据会拦，这是预期行为，不是本包缺陷。
- **叠跑前提**：K4 开发在未合并的 K3 之上（human 2026-11-15 override），合并序仍钉死 **K3 → K4**；K4 的合并门前必须先完成 K3 合并。
- **decisionCoverage 中 `landsIn === 'dropped'` 且 `reason` 以 `risk-accepted` 开头的条目**：**无**（本包 7 条 decisionCoverage 分别为 exitCriteria / package 承接，无 risk-accepted 放弃项）。
- **回滚方式**：本包改动全在 `dev/pod-m2` 的非 main 分支 commit 内，逐 commit `git revert e87310de`（实现）+ 撤销 `verifier-evidence/rounds/r1.json` 之外的收尾 commit 即可回退；无数据迁移、无分区表变更、无远端状态变更。

## 需 supervisor / human 裁点

1. 真机轮 K4G-L1 尚未跑：合并门会因 `deviceOutcome` 拦停，需 human 设备窗（与 K3 live 轮合并）完成后补 `device-round-evidence.md` 再复走门。
2. K3 未合并：合并序须先 K3 后 K4，planner 主线程把关。
3. ECS 部署轮（skill/CLI 面）归主线程，不在本包收口范围内。