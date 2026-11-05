# Review Packet — 261104-k0a-four-page-skeleton

- 日期：2026-11-05 · 编制：integrator（Pod 池树 C:/AI/KidPal-pod-m2，分支 dev/pod-m2）
- base ref：22b30654（workstream 拆包锚，不用 main 口径）· mergeOrder 70（K0a 先合，K0=71 等本包）
- riskTier：high（改全局页面状态机/键路由/会话来源映射，误切积分页→记录语境的根因面）

## Change Size

- Files changed：28（vs 22b30654）= changeManifest 21 路径（20 跟踪修改 + 1 新增 `firmware/main/test/four_page_skeleton.acceptance.c`）+ 7 件 delivery/** 制品。**零出界**（E5 对照：实际 diff ⊆ changeManifest ✓）。
- Net diff：+1324 / −551（含 delivery 制品）。
- Scope：实现 11 文件（四页枚举/循环、keybar 四点页槽、P3 静态占位 + K0 接管缝、source_page 枚举驱动、TECH-261028 窄修、log_semantics/lcd_display/record_session_repair.test.c 机械重锚）+ 9 冻结验收件 + 共享 CMakeLists 登记行。
- Split recommendation：不拆。四页骨架是单一原子语义变更（枚举 3→4 连锁九件验收重锚），拆开会留中间红态。

## Verification Evidence

- **红基线**（acceptance-author 冻结实跑 2026-11-04）：host build error=0（-Werror 全绿）；273 Tests / 21 Failures / 0 Ignored——存量族 3（p1_auxbar AB-A1/A2/A4，基线锚 265/3 @ 2f2c5871 非本包）+ 本批义务红 18，按名判读零错位。
- **终态 host**：273 Tests / **3 Failures**（仅存量 AB 族，与本包无关）；scoped r3 全绿（`verifier-evidence/rounds/r3.json`，round 3，含 firmware-build 真编译）。
- **final verify（唯一一次，round 4）**：`verifier-evidence/rounds/r4.json` —— ok:true、phase=final、round=4、invocationId=273daeed-f856-4a3b-a983-0df47129b1ca、observedGateExit=0；**十闸全绿**：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译，xiaozhi.bin 0x324f80 / app 分区余 9%）/ contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance。checker：`verifier-evidence-check.mjs` PASS（factOk:true）。
- **test-scope 清单落盘路径一行（E5 留根锚）**：`delivery/taskpackages/261104-k0a-four-page-skeleton/test-scope-final.txt`（10 条 = 本包 9 件验收 + record_session_repair.test.c 单测面；detect_changes 0 种子命中降级留痕 + trace_path 抽查可用，见下「机器辅证记账」）。**注**：本版 verify.mjs 未把 scope-file 路径写进 fact results 元数据（见 Findings NIT-1），该行由本 packet 与 docs-delta.md 承载。
- **device build**：firmware-build 闸真编译通过（r4.json 内 build-pass，ESP-IDF v5.5.1 工具链，bootloader/app/spiffs 分区尺寸检查全过）。
- **变异证据**：builder 变异取证轮（2026-11-05）13 条变异（M-01..M-13）/ 13 轮 build+run / **20 条 functional id 全部 turnedRed 且逐条 md5 还原确认零残留**；终态回到 273/3。2 条 docs canary（M2K2-E1 需求锚 / M2K2-E2 契约行）= **structural 豁免裁定**（planner 2026-11-05 裁定 A：唯一红路径 = 改 requirements/契约文本，铁律零 docs 触碰、不存在合法源变异；kind:'structural' = 闸通道①既定语义）。K0A-C1 保持 functional（M-11 换序变异实证不靠摘行造假）。正副产品：M-13/M-10 连带打红非本包冻结齿（char_window_overlay/voice_round_ui），证变异为真实语义回归。
- **Security gate**：securityReviewRequired = false（如实申报）；security 闸绿（r4）。无新增外部依赖、无网络/存储新面、无凭据处理；变更全部在既有固件页面状态机与键路由内部。
- **Docs sync**：docsSyncRequired = false（理由 = taskpackage docsSyncReason：requirements v1.71 / 架构 / ADR-018 已冻结，本包只落实现与验收镜像）；docs-sync 闸绿 + docs-delta.md 落盘（零 docs 变更义务 + prose 存量清扫移交）。

### 机器辅证记账（Step 9b advisory，范围判定归 Step 4）

- detect_changes（C-AI-KidPal @ 22b30654）：changed_files 只命中 delivery/** 与 .codebase-memory 再生面、**seed_symbols=0 / impacted_total=0**——图索引只反映已提交态，工作树未提交的 firmware 改动不进 seed。按三分法走降级：trace_path 抽查可用——`kidpal_render_node_cycle_next` inbound 命中 boards 层（InitializeButtons / keya_dispatch_global_action，与既有验收映射一致）；`kidpal_story_ctx_source_page` inbound=0 = taskpackage 已记档的 C 调用解析缺口（application.cc:1313-1320 实读补录）。降级圈定合法、如实留痕；re-index 需求交主线程/human（不虚报核对）。

## exitCriteria 逐条对照（K0A-E1..E5）

| # | 判据 | 状态 | 证据 |
|---|---|---|---|
| E1 | Pod 内 acceptance-author 产出并冻结四页循环/四点页点/逐页 source_page/故事三 story 零回归/阅读键缺陷的验收证据；红基线、变异证据、收紧型修订留痕齐全 | **绿** | 红基线冻结实跑在案（taskpackage redBaselineResult，21 红按名）；9 冻结验收件（含 3 件 planner 扩单相邻锚）；变异 20 条敏感回填 + 2 条 structural 豁免；REVISION LOG 收紧型重锚申报在 acceptance[] |
| E2 | host 全量验收与 device build 证据在场；P3 静态占位与 P4 故事原地迁移可编译；K0 接管缝在位 | **绿** | host 273/3（存量 3）；firmware-build 真编译 r4 绿；接管缝三处见下「K0 交接」 |
| E3 | 一轮真机证据覆盖 BOOT 四页循环、四点及实心位、逐页 source_page、阅读 KEY 短按零响应、长按回列表、阅读键帽；串口与看屏互证 | **pending（live-manual 相位，申报）** | 本包 host/scoped/final 全绿 = 静态与 host seam 面；真机轮未跑。真机核验三点：①BOOT 四页循环 + 四点页点当前实心位看屏；②逐页 KEY 开会话后串口 audit source_page 值（points_home 不误报 story——D-08 真机终判）；③阅读 KEY 短按零响应 + 长按回列表 + 键帽「长按退出/翻屏」——TECH-261028 窄修（kidpal_render_node_story_substate SERVER_PAGE+STORY+READER→READER 支）真机终判 |
| E4 | V9 相关帧 design review；review-packet 含 demo 对账行与未覆盖面申报；只宣称四页骨架与阅读键修复 | **pending（design review 人工相位）** | demo 对账行留待 K0A-E4（占位行见下「UI design-review 证据」节）；本包宣称面 = 四页骨架 + 阅读键修复，**不**宣称故事封皮（K5）或积分正式业务（K2）已完成 |
| E5 | 实际 diff 限于 changeManifest；TECH-261028 消费关系与 supersede 授权链写入收口制品；不吸收 TECH-261022 独立承接范围 | **绿** | diff ⊆ manifest + delivery/** + CMakeLists（Change Size 节）；TECH-261028 消费关系 = planner 2026-11-05 裁定 B 窄修（manifest 文件内 additive 支），其条目更新与条件加强候选不归本包；TECH-261022 共享 keybar/故事页串行对账由 crossPackageBehaviors 承载、独立承接包未并入 |

## 未覆盖面申报

1. **K0A-E3 真机轮**（BOOT 四页循环、四点、逐页 source_page、阅读 KEY 短按零响应 + 键帽 + 长按回列表——串口与看屏互证）。
2. **K0A-E4 V9 帧 design review**（demo 对账行待真机/人工相位）。
3. **TECH-261028 真机断链窄修终判**（SERVER_PAGE+STORY+READER→READER 支）随 K0A-E3。
4. **prose 存量漂移**：render_node.cc / story_page.c / lcd_display.cc 旧「P3 故事页」注释——不在齿面，移交后续触碰包顺手清（docs-delta.md 移交节；lcd_display.cc:1372 邻接锚为机械锚不得误清）。
5. **全量回归**（scope 外其余 host 验收件全集）：人工触发位，不进 final scope；存量 3 红（p1_auxbar AB 族）非本包责任面。

## K0 接管缝交接说明（给 261104-k0-points-e2e-spike）

K0 等 K0a merged、重取基线后追加，三处缝已留：

1. **话术单源宏**：`firmware/main/display/kidpal_render_node.h:56` `KP_POINTS_NODATA_HINT`（「积分数据还在路上，等一等再来看」）——K0 正式积分页消费同一宏，不得复制字面量（K0A-A4 负锚守卫：零 %d / 零 lv_bar / 零 KSTORY_EMPTY_HINT 借用）。
2. **渲染 case 缝**：`kidpal_render_node.cc` case `KP_PAGE_POINTS_HOME` 块 = 静态占位落点；K0 正式页替换该 case 内容、保持枚举与页名闭集不动。
3. **共享 CMakeLists**：`firmware/main/test/host_app/main/CMakeLists.txt` 是两包唯一共享构建清单——K0 的 points_page 登记行必须等 K0a merged 后追加，不得提前改（本包 K0A-C1 注册序守卫已锚 mine < smoke，K0 追加时不破坏该序）。

## 集成执行回报

无（本包未触发 merge-chain 冲突消解路径）。

## 设备轮证据

none（deviceOutcome 未触发；E3 真机轮 pending，见 exitCriteria 表与未覆盖面申报）。

## UI design-review 证据（bootstrap 期纯文本，Step 6b）

- **viewport coverage summary**：目标设备 RLCD 400x300（host seam 同分辨率，story_page.c:1012 minimal 注在案）；本包覆盖 V9 五帧骨架位——canvas-p1（P1 首页，页点四点化）、canvas-p2（P2 课表，页点同源）、canvas-p3（P3 积分占位，第 3 实心 + NODATA 话术 + 键帽「问一问/下一页」）、p3-reader（P4 阅读态，键帽「长按退出/翻屏」+ TECH-261028 短按零响应）、points-home-nodata（P3 数据未达态）。KB-A3 槽对与页点同源闭集 host 验收绿。
- **a11y summary**：嵌入式无屏读/焦点面；可达性 = 键位语义——四点页点与 key_page_next 回绕、KEY 长按回列表、覆盖层 KEY 短按语义零改动（KD-A1/A2/A6 全绿实证）；防御列越界页返回 -1（K0A-A3 变异实证）。
- **design-drift summary（brief↔实现）**：骨架级无漂移——四页序/四点页点/占位话术/键帽文案与 V9 brief 一致（host 验收逐字锚）。已知分层后置：故事封皮（K5）、积分正式业务与账本（K2）、D-02 完整 points_home 均不在本包，V9 帧的完整观感对账留 K0A-E4。
- **external artifact reference**：`docs/ui/V9/canvas-p1.png` / `canvas-p2.png` / `canvas-p3.png` / `p3-reader.png` / `points-home-nodata.png`；brief = `docs/ui/UIUX-DESIGN-BRIEF.md`；复合稿 = `docs/ui/kidpal-ui-design-v9.html`（仓内既有资产，本包未新增重资产）。
- **residual risk**：观感终判（1bpp 笔画、页点间距、话术排版）留 K0A-E4 人工相位；**demo 对账行：待 K0A-E4 真机/人工相位回填**（占位——E4 申报 pending）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包 diff 零新增 `minimal:` 标记） | — | — | no-trigger |

存量 `minimal:` 标记（story_page.c:1012/1464/1647/1706、log_semantics.c:65）为前批遗留、本包未触碰语义，不计入本包台账。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| 无 BLOCKING | — | — | — | — | — |
| NIT | judgement-call | dev-loop 工具链 | final verify fact（r4.json）results 元数据不含 --scope-file 落盘路径行，交接单「results 元数据必含行」由 review-packet/docs-delta 手工承载 | r4.json 实读（全文无 test-scope-final 引用）+ taskpackage 交接单要求行 | 交 planner pod-retro 沉淀：verify.mjs final 相位把 scope-file 路径写进 fact 元数据（不改闸判据，纯投影增列） |
| SHOULD | judgement-call | 构建卫生 | 工作树存在未跟踪构建邻产物 `firmware/main/test/host_app/sdkconfig.pre-launch-bak-20261104`（launch 期 sdkconfig 可逆改名备份），不入 commit；后续包若同法改名应统一备份命名或改为不入树 | git status untracked 条目 + .gitignore 口径核对（该文件不在忽略清单，靠 add 白名单纪律拦截） | 交主线程/后续 launch 包：确认 sdkconfig 备份文件入不入树口径，避免逐包积累 |
| SHOULD | judgement-call | 测试面 | 全量 host 回归（scope 外其余验收件）为人工触发位，本次 final 未跑全集；存量 3 红（p1_auxbar AB 族）基线锚在册非本包责任 | test-scope-final.txt 圈定口径 + r4.json acceptance 闸 surfaces.firmwareAcceptanceFiles=9 | merge 后 K1 launch 前由 planner 决定是否触发一次全量 host 回归作列车基线复核 |

结构闸（verify.mjs 十闸）PASS/FAIL 未复述为 finding；gate 全绿仍完成本五轴语义 review。

## Five-Axis Review

1. **安全**：无新增攻击面/外部输入处理变更；窄修 additive 支只对 READER 前台层放开 story_substate，LIST/RECORDING 仍 -1 不碰 CG-A2 过渡窗；键路由覆盖层语义零改动有 KD 族负锚实证。绿。
2. **契约**：session_ctx 六值闭集零扩（source_page 仍 home/schedule/story 三值闭集 + 未知/负值/COUNT→home 防御）；契约文件零触碰，contract-lint 绿；K2（points_ledger）/K5（story_cover）扩值不在本包。绿。
3. **边界**：页枚举 COUNT=4 与页点/槽对/页名表三处同源（keybar 四点、DLG-A2 页名闭集、M3K2-A6 同源判别）；越界防御（dots 越界 -1、source_page 兜底 home）变异实证。绿。
4. **可运维性**：log_semantics 页名表 3→4 项同步（DLG-A2），P3 日志不再伪名 story；KPLOG 语义缝无变更。绿。
5. **依赖方向**：新增 case 分支均在既有状态机内（render_node 页 case、story_page 归属支、keybar 槽查表），无新模块、无新外部依赖、无跨层倒挂、无循环依赖（detect_changes 降级辅证 + diff 实读复述）；implementations 仍单向依赖 contracts 枚举（KP_PAGE_* 头内单源）。

## Residual Risk

- Known limitation：真机面（E3）与观感终判（E4）pending——host seam 与静态证据绿不替代真机互证；TECH-261028 窄修的真机命中待 K0A-E3 终判（builder 静态路径分析已实证修法甲真机不命中，窄修为 planner 裁定 B）。
- Follow-up recommended：prose 存量清扫（移交节）；K0 merged 后共享 CMakeLists 串行登记；全量 host 回归作 K1 基线复核（Findings SHOULD-3）。
- Human decision needed：无阻塞项；remote push 与 release 仍归 human。
- **decisionCoverage 风险接受项（landsIn=dropped && reason=risk-accepted\*）**：无（本包六条 decision 全部 landsIn exitCriteria 或下游 package，零 dropped/risk-accepted 条目）。

## 可回滚路径

- **整包回退 = 单 commit revert**：本包全部实现/验收/制品收口为 dev/pod-m2 单 commit，revert 该 commit 即完整回退（无跨 commit 语义耦合；milestones M7 state 转移与 INDEX 投影随 revert 回 planning 语义，STATUS.md 可重生成）。
- **构建漂移兜底**：sdkconfig 如遇构建漂移，`git checkout -- firmware/main/test/host_app/sdkconfig`（launch 期改名备份文件 sdkconfig.pre-launch-bak-20261104 留树外快照，不入版本控制）。
- **部分回退注意**：若只回退验收重锚不回退实现，会留假绿——验收与实现必须同 commit 同退。

## 机器产线件与投影

- 承重 fact：`verifier-evidence/rounds/r4.json`（final，ok:true，checker PASS）——closeout 判断来源。
- 投影：`verdict.json`（本包目录晋升件，= current/verdict.json r4 投影，program fs-write 产出）；`verification.json` 本版 verify.mjs 不产出（closeout-check advisory-only，缺失不红）。
- 不产 session-digest.md / loop-health.md / loop-metrics.json / pod-journal（post-cutoff ≥ 250725）；pod-events.jsonl 在案（advisory 检查通过）。
