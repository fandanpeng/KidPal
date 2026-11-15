---
changeId: 261104-k5-story-cover-nav
title: 故事封皮导航——P4 封皮、列表/阅读下沉、逐层返回与 story_list 共享帧消费
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
  workstream: 261103-star-points-story-cover
reviewed-by: architect（/triage-change 分流 review 2026-11-03；门 1 待主线程另派）
date: 2026-11-04
gateProfile: ["evidence-integrity", "test-quality", "state-machine", "mcp-tool-contract"]
dependsOn: ["261104-k2-points-device-ui", "261104-k4-reward-goal-config"]
mergeOrder: 76
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- K0a 已把故事原地迁到 P4，但那只是四页骨架的过渡态：故事列表仍是平级页，封皮、模块内层级和逐层返回尚未实现。front 与设备也还把 story_list 当成“切到列表”的页面帧，而 landed 契约已把它改成 P4 模块共享数据帧。
- 封皮需要书封、标题、现存星星数和“打开/下一页”键帽。列表和阅读下沉后，KEY 长按必须阅读→列表→封皮→首页逐层返回；语音“打开故事”只到封皮，不能跳过模块入口。
- 本包最后落共享导航面，并承担导航并集收口。TECH-261028 已在 K0a 修复，本包只做回归；PROPOSAL §5 的跨包账本与真实 QQ 联合收口由主线程另组织。

# 可行性判据

- K0a 已提供四页枚举、四点 keybar、枚举驱动语境和阅读键修复；K2 已落定 P3 积分页。K5 可以在稳定基线上集中处理 P4 层级，不必再与积分实现并行抢共享文件。
- ADR-018 已明确 story_list.total 是封皮与列表共同计数源，否决新 story_cover view 和 cover_total。front `_push_story_list` 的六个调用者、display_schema floor 与 OpenClaw 静态纪律均已核实，可按契约逐点对齐。
- V9 story-cover 与既有列表/阅读帧齐全，KAP1 管线在 K4 后串行可用；host/server 可验导航与帧语义，真机可完成四页和故事层级并集。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **K0a→K5**：K0a 的 P4 平级故事是合法过渡，不是最终 spec 完成态。本包不得回退四页枚举或阅读键修复，只在其上新增封皮和子页层级。
- **story_list 只有一份计数**：封皮与列表都信 `total`，后台恢复/归档/删除推送恒 restore:true；只有显式“打开故事”的空 data 薄载荷进入封皮。不得新增 story_cover view、cover_total 或独立计数 API。
- **语境分工**：K0a 建枚举映射，K2 扩 points_ledger，本包扩 story_cover。封皮发起的是普通问答，只有 P4 故事列表产生 `story` 并进入记录语境；路过封皮不得自动开录。
- **相邻冻结锚拆包期预扫非零命中**：story_page_v2 的列表返回、record_session_live 的语境、keybar_three_slot 的页槽、chrome_ghost 的转场，以及 server M4K1/M4K3 的 story_list 推送断言均需保留或收紧。旧实现/测试若与“后台 restore:true”冲突，由 acceptance-author 留痕重锚，不能改契约或静默保留旧行为。
- **K4→K5 资产串行**：K4 已产场景资产，本包在同一管线补 `ic-coin` 与书封；生成器、资产清单和生成件不与 K4 并行修改。

# 铁律约束

- P4 封皮是故事模块唯一主入口；KEY 短按才进列表，BOOT 从封皮 wrap 回首页，KEY 长按逐层返回，每按一次只上一层。列表/阅读的 BOOT 仍用于页内浏览和翻屏，不恢复全局翻页。
- 封皮计数等于现存篇数，数据未达不显计数、确认空账可显 ×0；故事星星与积分余额不得合并。列表、录音、盖章、阅读、删除确认等未变帧沿既有 V8/V9 口径，不借下沉重画。S-F2.1 AC7 纳入回归：正文未留存且当刻取不回时给可见提示，不黑屏、不显示别篇；由 Pod 内 acceptance-author 覆盖。
- 语音“打开故事”只到封皮；OpenClaw 只能发 story_list 空 data 薄载荷，完整列表仍只能由 front 产生。任何后台推送不抢前台。
- 阅读 KEY 短按零响应和正确键帽以 K0a 为基线，不得再落全局 SESSION_TOGGLE；本包不重复承接 TECH-261028，也不吸收 TECH-261022 的独立修复范围。
- 不改 requirements、architecture、`docs/contracts/**`，不新增 story_cover view/cover_total；发现需要新线格式、计数源或全局键位新语义时立即停并升级。

# demo 证据形态（与 roadmap 一致）

- host runner + server pytest：封皮/子页导航、四点与覆盖层、story_list total/restore/薄载荷、story_cover/story 语境及故事回归的 RED→GREEN 和变异证据；S-F2.1 AC7 覆盖正文未留存且取不回时的可见提示与不显示别篇。
- 资产管线与 device build：ic-coin/书封产出与装载，V9 design review；既有故事子态和 K4 场景资产零回归。
- 一轮真机导航并集：BOOT 四页、封皮→列表→阅读、逐层返回、语音只到封皮、记录会话、删除/盖章计数和阅读键；导航之外的跨包联合收口项明确交主线程。

# 技术方案（planner+architect 预评）

> 2026-11-15 · architect **Step 1g ARCH-ADVISORY 档**前置技术预评回执 + planner 裁定的落盘版。效力声明：预评不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据；本节为 planner 只读消费 plan.md 的唯一例外（task-intake-and-slicing.md 例外条款）。回执全文见本批 pod-events 与被引 path:line。

## A. 封皮落点（Q1）

- **封皮落 `story_page.c` 新增子页态 + render 分支**，不建独立 `story_cover.c`，也不落 `kidpal_render_node.cc` 的 P4 渲染分支。理由：① host 可编性——`firmware/main/test/host_app/main/CMakeLists.txt` 已编入 `display/story_page.c`，封皮四件/计数/键帽可用 pixel-probe 先例做真帧断言；render_node.cc 设备专属只能 tree-scan + 真机目击。② 状态机单一：故事七子态全在 `story_page.h`，render_node 只做页分派。③ 冻结面影响可控。
- `changeManifest` 相应**删去** `firmware/main/display/story_cover.c/.h`（无新源文件；`firmware/main/CMakeLists.txt` 若无其它新增源亦删除）。

## B. 子页层级模型（Q2）

- `KSTORY_PAGE_COVER = 7` 在 `story_page.h` 子页枚举尾部追加（只增不改）；封皮是 `KP_PAGE_STORY` 的一个子态，`s_local_page` 全程 `KP_PAGE_STORY`，四点页点第 4 实心与「页循环在模块内断开」自然成立，`KP_PAGE_*` 枚举零改。
- 新增 host 纯函数 `kidpal_story_list_load(v, list)`：只装 count/counter/screens/影子 id + cursor 钳制，**不动 page**；`kidpal_story_view_init` 保持 page=LIST 语义（SPV2-A1 钉死）。
- 出场帧（front 展开「打开故事」= 非 restore 完整帧）= `list_load + page=COVER`；逐层返回：READER KEY 长 = 既有 READER_EXIT 零改；LIST KEY 长 = 新码 COVER_BACK 并置 page=COVER；COVER KEY 长 = GLOBAL（基层 = HOME）。
- **入口 reset（human 2026-11-15 裁定 = 板级 BOOT 钩子）**：`firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` 的 PAGE_NEXT 分支判当前页 = P3 时调 McpServer 新缝 `StoryEnterCover()`（经 `ScheduleStoryRender()` 落主任务），保证冷启动/异常退出后首翻 P4 也见封皮（S-F2.6-AC1）。不动 `[S1]` 签名与 `view_init` 语义。

## C. 按键与键帽（Q3）

- 不复用 `KSPV2_ACT_GLOBAL` 表达「回封皮」：act 闭集尾部追加 `KSPV2_ACT_COVER_BACK`、`KSPV2_ACT_COVER_OPEN`；COVER case 全表（KEY 短=COVER_OPEN 进列表、BOOT 短/双/长=GLOBAL、KEY 长=GLOBAL、KEY 双=NONE）。`key_route_layer` 全局表零触碰；`kidpal_story_key_owns` 的 K0a 阅读键支零触碰。
- 键帽：`keybar.c` P4 页槽行改「打开 / 下一页」；新 `kidpal_keybar_story_slots` 增 LIST 行「说话·双击看 / 下移」（迁自页槽表）；`lcd_display.cc` 子态集合补 LIST；chrome_ghost 过渡窗集合补 LIST（否则云端列表渲染窗键帽闪成封皮行 → 该件重锚）。
- COVER 单击不得进 300ms 双击消歧（`kidpal_story_debounce_gate` 冻结只认 LIST）。

## D. story_list 共享帧（Q4）

- **五调用点判定**：`:2453` WS 建连已 true（保持）；`:1023` ctx 建立 → 翻 true；`:1047` 删除后 → 翻 true（设备 restore 分支须显式 `was_delete → dismiss_overlay + page=LIST`，否则幽灵弹窗）；`:1739` 归档成功 → 翻 true；**`:1728` 归档失败离页帧 = 保持 restore=False（human 2026-11-15 裁定）**，按「离页导航帧」例外处理，`[R2]`/M4K1 SAO-A1 零改，review-packet 留痕 + ARQ 建议契约措辞排除离页帧。
- **设备侧 restore 必须从「view_init 重装」改为「只装缓存不导航」**：restore → 只更新 `g_story_list` + counter + 影子 id + cursor 钳制；封皮在屏重绘封皮、列表在屏重绘列表、READER/STAMP/DELETE/RECORDING 在屏零渲染零页态改动（现实现无条件 `view_init` 会把活动子态重置成 LIST，restore 常态化后会放大 TECH-261022 族 churn）。非 restore → `list_load + page=COVER`。
- 薄载荷 `{"view":"story_list","data":{}}`：**设备永不收**（front 截获展开）；`display_schema.py` 增 `data == {}` 早退（对齐 points 薄载荷先例）；`front_app.py` 展开链新增 `_expand_story_nav_intent`（恰薄载荷才展开，展开帧不带 restore），并把 Q7 降级改为 `and not front_expanded`，否则自展帧被自己降级。契约 §1.5 已落「增认 `data:{}` 导航薄载荷」，**无需改契约文本**。

## E. 语境（Q5）

- 不动 `kidpal_story_ctx_source_page` 签名（`[S1]` 冻结）；在 `application.cc` 发送点按 K2 `points_ledger` 先例扩一支：`page == KP_PAGE_STORY && 会话页态 == COVER → source_page="story_cover"`。判据用会话页态镜像的只读 getter（不能用 VOICE 前台时恒 -1 的 substate）。COVER = 普通问答（front 已白名单 `story_cover`，零行为）；LIST 仍 `story` 记录语境。

## F. 资产（Q6）

- **不需要「书封位图」job**：封皮几何 = rbox + 书脊线 + 标题 + 徽章 + 小伴，原生绘制。必须新增 = `ic_coin_52_52`（V9 52px 口径）。可选（design review 判定）：`pose_daily_78`、`ic_sparkle_20`、`ic_cloud_34`；复用现有尺寸不合的资产须在 design review 记为偏差项。
- 追加方式：`gen_assets.mjs` 尾部 job（job name = 产物符号名）、`gen_art_spiffs.mjs` WHITELIST 尾部追加；产物名 = `<name>.bin`；生成后**纯追加 + 其余还原**（K4 先例）。
- **风险点**：`story_page.c` 的 `KSTORY_ART_CACHE_SLOTS = 7` 已被 7 枚占满，封皮再加资产会静默不可用——须扩容（纯逻辑常量，host 可测）。

## G. 验收面（Q7）

- 新冻结一件 `firmware/main/test/story_cover_nav.acceptance.c`（host 为主 + 结构齿）：C1 封皮四件/四点第 4 实心/键帽；C2 计数两态；C3 手势矩阵；C4 页态序列；C5 restore 三态（缓存态不导航 / 出场帧进封皮 / 畸形降级 + 活动子态零重置）；C6 front 薄载荷口；C7 五调用点 restore 判定（含 :1728 例外留痕）；C8 语境三分；C9 资产；C10 回归（S-F2.1 AC1–AC6+AC7、S-F2.2 AC1、S-F2.3 AC1/2、S-F2.4 AC2、K0a 阅读键、chrome_ghost）。
- host 可闭环：C1–C6、C8 结构半、C9、C10 host 半。**只能真机**：BOOT 四页并集与逐层返回手感、封皮观感（uiuxLevel 4 design review）、语音「打开故事」端到端、restore 不抢前台的真实时序、删除/盖章计数与记录会话并集（K5S-E4）。
- 必须重锚：story_page_v2（`:976` 保持 + LIST KEY 长按翻正 + 矩阵注释）、keybar_three_slot（P4 页槽行 + 新增 LIST 行）、chrome_ghost（过渡窗集合含 LIST）、record_session_live（`[S1]` 不动、补 cover 结构齿）、server M4K1（restore 断言翻正，`:1728` 帧除外）、server M4K3（恰一帧语义不变、补 restore=True 断言）。four_page_skeleton 零重锚。

## H. 风险与停手点（Q9）

1. **归档失败离页帧** → 已由 human 2026-11-15 裁定「保持原样 + 留痕」，不再是停点；若实现中发现必须翻 true 才能满足冻结件，停。
2. **入口 reset** → 已由 human 裁定「板级 BOOT 钩子」；若被迫改 `[S1]` 签名或 `view_init` 语义（会同时波及两件冻结验收），停并升级。
3. **restore 常态化的页态保护边界**：若评审要求 restore 期间保留活动子态而牵出 `[R2]`/view_init/TECH-261022 churn 语义重定义，或要求新线格式/新计数源，立即停。
