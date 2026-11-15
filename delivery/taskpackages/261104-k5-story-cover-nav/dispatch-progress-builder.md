# dispatch-progress · builder · 261104-k5-story-cover-nav

> 执行体：builder（Pod m2 worktree，分支 dev/pod-m2，包 base ref `03630b65`，起手 HEAD `ffc00bcc`）
> 日期：2026-11-15
> 任务：实现「故事封皮导航」包——P4 封皮子态 + 列表/阅读下沉 + 逐层返回 + story_list 共享帧消费 + 书封 ic-coin 资产。

## 1. 实现清单（文件 → 改了什么）

| 文件 | 改动 |
|---|---|
| `firmware/main/display/story_page.h` | 子页枚举尾部追加 `KSTORY_PAGE_COVER = 7`（只增不改）；act 闭集尾部追加 `KSPV2_ACT_COVER_OPEN = 13` / `KSPV2_ACT_COVER_BACK = 14`；新增 `int kidpal_story_list_load(...)` 声明。 |
| `firmware/main/display/story_page.c` | ① `KSTORY_ART_CACHE_SLOTS` 7→8；② 新增 `kidpal_story_list_load`（只填 count/counter/screens/影子 id+cursor 钳制，page/rec/盖章字段零触碰，NULL→-1）；③ 输入表：LIST KEY 长 `GLOBAL`→`COVER_BACK` 并置 page=COVER，新增 COVER 行（KEY 短 COVER_OPEN→LIST / KEY 长 GLOBAL / KEY 双 NONE / BOOT 全 GLOBAL）；④ 渲染分支新增 `case KSTORY_PAGE_COVER`（书封 rbox 边框 + 书脊线 + 原生双圈星徽 + `ic_coin_52_52` best-effort 覆盖 + 「星星记录本」标题 + list!=NULL 才画 `★ ×N`；list==NULL 不画计数、rc=0）。 |
| `firmware/main/display/keybar.c` | P4 页槽行 `说话·双击看/下移`→`打开/下一页`；`kidpal_keybar_story_slots` 新增 `KSTORY_PAGE_LIST` 行 `说话·双击看/下移`；`kidpal_keybar_story_transition_slots` 在途集补 `KSTORY_PAGE_LIST`。 |
| `firmware/main/display/keybar.h` | P4 页槽行/story_slots/transition 文档注释同步。 |
| `firmware/main/display/lcd_display.cc` | 故事子态键帽路由集补 `KSTORY_PAGE_LIST`（列表子态经 story_slots(LIST) 出列表行；COVER 不在集内回落 P4 页槽表「打开/下一页」）。 |
| `firmware/main/display/kidpal_render_node.cc` | `KP_PAGE_STORY` 分支新增 COVER 分派（`s_story_view.page==KSTORY_PAGE_COVER` → story_page.c 封皮渲染，数据未达传 NULL 不画计数，不被列表空态截走）；新增 `kidpal_render_node_story_session_substate()` getter（s_story_view.page 原值，不经前台层过滤）。 |
| `firmware/main/display/kidpal_render_node.h` | 声明上述 getter。 |
| `firmware/main/mcp_server.cc` | story_list 路由 load 块：`kidpal_story_view_init` → `kidpal_story_list_load`（restore 只填缓存；非 restore 完整帧置 page=COVER；was_delete 显式关层回 LIST；stamp_hold 保持 STAMP）；`StoryDispatchAct` 新增 COVER_OPEN/COVER_BACK → ScheduleStoryRender；新增 `McpServer::StoryEnterCover()`（置 COVER + 主任务 cache_story，list/text 传 nullptr 不改数据/送达标志）。 |
| `firmware/main/mcp_server.h` | 声明 `void StoryEnterCover();`。 |
| `firmware/main/application.cc` | Listening 分支新增 story_cover 扩点：`KP_PAGE_STORY && story_session_substate()==KSTORY_PAGE_COVER → source_page="story_cover"`（不改 `kidpal_story_ctx_source_page` 签名）。 |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` | `GLOBAL_ACT_PAGE_NEXT` 分支：当前页=P3 时调 `McpServer::GetInstance().StoryEnterCover()`（human 2026-11-15 裁定 = 板级 BOOT 钩子）。 |
| `firmware/main/test/host_app/main/CMakeLists.txt` | 在 `host_infra_smoke.acceptance.c` 之前登记 `../../story_cover_nav.acceptance.c`。 |
| `firmware/main/test/chrome_ghost.test.c` | 新增白盒 TEST 3b：过渡窗 + 在途 LIST 裁决命中、rec_sub 全值域无关、LIST 行逐字（说话·双击看/下移）。 |
| `server/gateway/display_schema.py` | story_list floor 增 `data == {}` 早退放行（对齐 points 薄载荷先例），其余畸形仍拒。 |
| `server/gateway/front/front_app.py` | 新增 `_expand_story_nav_intent`（仅 `{"view":"story_list","data":{}}` 展开为完整非 restore 帧、置 front_expanded）；Q7 降级判据改 `and not front_expanded`；`:1023` ctx / `:1047` 删除后 / `:1739` 归档成功三处 `_push_story_list` 传 `restore=True`；`:1728` 归档失败离页帧保持缺省 False（不动，human 裁定）。 |
| `tools/asset-pipeline/gen_assets.mjs` | 尾部追加 `ic_coin_52_52` job（母版 ic-coin 符号 52×52，命名律 = 产物符号名）。 |
| `tools/asset-pipeline/gen_art_spiffs.mjs` | WHITELIST 尾部追加 `'ic_coin_52_52'`。 |
| `tools/simulator/.../assets.c` / `.h` | 纯追加 ic_coin_52_52 map + descriptor + extern 符号（461 插入 0 删除）。 |
| `firmware/main/assets/spiffs_root/ic_coin_52_52.bin` | 新产出 KAP1 件（52×52 / stride 7 / 380B / magic KAP1）。 |

## 2. 自测数字（前后对比）

### server（`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`）
- 红基线：`9 failed / 448 passed / 9 deselected / 33 subtests passed`。
- 实现后：`457 passed / 0 failed / 9 deselected / 33 subtests passed`（**9 条设计态红全绿、零新增红**）。

### host（`espidf_wsl.sh build` → `build/host_infra_test.elf`）
- 红基线：`293 Tests / 7 Failures / 0 Ignored`（4 设计态红：SPV2-A1/KB-A3/CG-A2/RSL-A10；3 存量 p1_auxbar）。
- 实现后：`301 Tests / 4 Failures / 0 Ignored`。
  - 新增/重锚件全绿：KCN-C1/C2/C3/C4/C5/C9/C10、KB-A3、CG-A2、RSL-A10、chrome_ghost.test builder TEST 3b 全 PASS。
  - 4 Failures = 3 条存量 p1_auxbar（AB-A1/A2/A4，本包未触碰）+ **SPV2-A1（见 §4 blocker：冻结件 tooth 序缺陷，非实现缺陷）**。

## 3. 冻结验收逐条状态

| 验收 | 状态 | 说明 |
|---|---|---|
| KCN-C1..C10（host） | 绿 | 封皮四件/计数两态/手势矩阵/逐层返回/restore 三态/资产/回归+登记序全 PASS。 |
| C6/C7/C8（server） | 绿 | display_schema 薄载荷放行+畸形仍拒+白名单零扩；五调用点 restore（含 :1728 例外）；语境三分。 |
| KB-A3 / CG-A2 / RSL-A10（重锚） | 绿 | P4 封皮键帽+LIST 行；过渡窗补 LIST；story_cover 语境扩点。 |
| SPV2-A1（重锚） | **红（冻结件缺陷）** | tooth 3 已绿（LIST KEY 长→COVER_BACK+page=COVER），但 tooth 4 失败：见 §4。 |

## 4. BLOCKER —— SPV2-A1 冻结件 tooth 3→4 缺页面复位（非实现缺陷，须 acceptance-author 最小修）

**现象**：SPV2-A1 tooth 4「LIST 态 BOOT 短按 = CURSOR_NEXT(3)」得 `act=-2(GLOBAL)`。

**根因**：`firmware/main/test/story_page_v2.acceptance.c:1009` tooth 3 用同一个 `v` 调
`kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS)`，该调用（按 plan + 本批
KCN-C3 冻结值）把 `v.page` 置为 `KSTORY_PAGE_COVER(7)`；tooth 4（:1017）**未复位 `v.page`**
就直接 `kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK)`，此时 `v.page==COVER`
→ 按 COVER 输入行返回 `GLOBAL(-2)`。

**为何是冻结件缺陷而非实现缺陷**：KCN-C3（`story_cover_nav.acceptance.c:811-820`）冻结
「COVER BOOT 短/双/长=GLOBAL」；KCN-C3（:823-829）冻结「LIST KEY 长 → COVER_BACK 且
page→COVER」。SPV2-A1 tooth 4 在 tooth 3 留下的 COVER 态上要求 `CURSOR_NEXT`，与 KCN-C3
「COVER BOOT=GLOBAL」**直接矛盾**——两个冻结件对同一 `(COVER, BOOT, CLICK)` 输入要求相反
返回值，任何实现都不可能同时满足。

**建议最小修**（acceptance-author，冻结件写权属主）：tooth 4 前插入
`(void)kidpal_story_view_init(&v, &l);`（或 `v.page = KSPV2_PAGE_LIST;`）复位到 LIST；其余
断言零改。修后本包 host 应达 `301/3`（仅剩 3 条存量 p1_auxbar）。

**为何红基线未暴露**：红基线下 tooth 3 因实现缺席先红（LIST KEY 长仍 GLOBAL），测试在 tooth 3
即中止、从未到达 tooth 4；tooth 3 转绿后 tooth 4 的序列缺陷才浮现。

## 5. 偏离与理由

- **清单外文件（须 architect 扩单）：`firmware/main/mcp_server.h`**。plan 步骤 5 要求新增
  `McpServer::StoryEnterCover()`（板级钩子消费点，KCN-C4 冻结齿在 board + mcp_server.cc 两侧
  扫描 `StoryEnterCover` 字面），**McpServer 是类方法，声明必须落 mcp_server.h**——但
  `changeManifest[]` 只列了 `mcp_server.cc`、未列 `mcp_server.h`。此为 manifest 遗漏（架构侧），
  非我可自行扩清单。处置：本包已改 `mcp_server.h`（否则 board/`mcp_server.cc` 无法编译、
  冻结齿无法满足），**如实申报为清单外改动，请 planner 派 architect 评估扩单**（manifest
  append `firmware/main/mcp_server.h` + 评估依据留痕）。除此之外实际实现 diff 全部落在
  `changeManifest[]` 内。
- **StoryEnterCover 未字面用 `ScheduleStoryRender()`**：plan B 写「经 ScheduleStoryRender() 落主任务」，但 `ScheduleStoryRender` 会立即渲染并把 `s_local_page` 置 KP_PAGE_STORY；若板级随后再 `cycle_next`，cycle_next 读到 STORY 会算出 next=HOME（P4→P0 跳错页）。故 `StoryEnterCover` 只置 `g_story_view.page=COVER` + 主任务 `cache_story(view, nullptr, nullptr)`（list/text 传 nullptr 不误置送达标志），令板级随后的 `cycle_next`（FIFO 在同批 main_tasks_ 内先跑 cache、再 cycle）以封皮态渲染 P4。行为达成 S-F2.6-AC1「首翻 P4 见封皮」，且保住 cycle_next 既有清层/页切换日志。**建议 review-packet 留痕此偏离**。
- **资产产出方式**：全量重跑 `gen_assets.mjs` 到 `C:/tmp/k5assets`（隔离目录），因既有资产在当轮 sharp/librsvg 下与入库快照有渲染漂移（第 14492 行附近 ~484 行差异），故**未覆盖**仓库 `assets.c/.h`，改为从隔离产物抽取 `ic_coin_52_52` 单块纯追加（git diffstat：`assets.c +460 / assets.h +1`，0 删除）；`gen_art_spiffs.mjs` 全量重跑后仅 `ic_coin_52_52.bin` 为新增、`p1_chip_mingzao.bin` 漂移已 `git checkout` 还原（纯追加纪律达标）。
- **`lcd_display.cc` 子态集补 LIST**：plan C 要求；若不补，列表子态会回落 P4 页槽表「打开/下一页」（封皮行）而非列表行（S-F2.1 键帽零回归被破）。

## 6. 未覆盖面 / 风险

- 真机面（K5S-E4）：BOOT 四页并集与逐层返回手感、封皮观感（uiuxLevel 4 design review）、语音「打开故事」端到端、restore 不抢前台真实时序、删除/盖章计数与记录会话并集——host 不可证，挂设备轮。**未运行**。
- 设备 TU（kidpal_render_node.cc / mcp_server.cc / application.cc / 板级 .cc）不进 host 构建，其结构由 tree-scan 源锚代理承载（KCN-C2/C4/C5、RSL-A10、chrome_ghost CG-A2 接线锚），host 真帧不可达——行为本体归设备轮。
- SPV2-A1 冻结件缺陷（§4）：需 acceptance-author 最小修后本包 host 方达 301/3。

## 7. input-brief §④ 清单外阅读 + 原因（原样回传，供 planner 记 over-scan）

| 清单外文件 | 原因（一句话） |
|---|---|
| `firmware/main/display/story_page.c` | 既有实现（view_init/render/input/list_gate/资产槽）实读，钉封皮落点与 list_load 复用。 |
| `firmware/main/display/keybar.c` / `keybar.h` | P4 页槽行 / story_slots / transition 实况与声明。 |
| `firmware/main/display/kidpal_render_node.cc` / `.h` | KP_PAGE_STORY 分支 / s_story_view / story_substate getter 实况。 |
| `firmware/main/mcp_server.cc` / `.h` | story 路由 load 块 / StoryDispatchAct / ScheduleStoryRender / McpServer 成员声明。 |
| `firmware/main/application.cc` | Listening 分支 ctx 发送点（points_ledger 先例，story_cover 扩点落点）。 |
| `firmware/main/display/lcd_display.cc` | 键帽三槽 story 子态路由集（补 LIST 消费点）。 |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` | GLOBAL_ACT_PAGE_NEXT 分派与 cycle_next 调用点。 |
| `server/gateway/front/front_app.py`（多段） | `_push_story_list` 五调用点 / `_expand_points_nav_intent` 先例 / build_display_tool_calls Q7 / `_story_finish_session`。 |
| `firmware/main/test/chrome_ghost.acceptance.c` / `keybar_three_slot.acceptance.c` / `story_page_v2.acceptance.c` / `record_session_live.acceptance.c` | 重锚断言逐条核对（只读）。 |
| `tools/asset-pipeline/gen_assets.mjs` / `gen_art_spiffs.mjs` | 资产管线 job/白名单结构，产出纯追加。 |

## 8. 变异证据轮（builder · mutation evidence · append-only · 2026-11-15）

> 目的：证明 `taskpackage.acceptance[]` 每条功能验收对实现**敏感**——把实现临时改坏 → 目标冻结验收变红 → 精确还原。
> 方法：每条只施加一个最小**行为级**变异，单独 build/run（host 7 条）或 pytest（server 3 条），记录目标 case 的 `:FAIL` 原文；随后 `git checkout -- <file>` 精确还原。
> 基线（还原态）：host `301 Tests 3 Failures`（仅存量 p1_auxbar AB-A1/A2/A4）；server `10 passed`。
> 末轮 GOLDEN（全部还原后重跑）：host `301 Tests 3 Failures`（KCN-C1..C10 全 PASS）；server `10 passed`。合计 8 轮 host rebuild+run + 3 轮 server pytest + 4 次逐件复绿。

### 8.1 逐条结果

| 验收 | 变异（文件·改动） | 目标红原文（摘） | 连带红 | 还原后 |
|---|---|---|---|---|
| KCN-C1 | `keybar.c` PAGE_P4 页槽行「打开/下一页」→「说话·双击看/下移」 | `:676 ... KCN-C1 RED: P4 页槽键帽 (左=说话·双击看, 右=下移) 期望 (打开, 下一页)` | KB-A3（允许） | golden:597 PASS |
| KCN-C2 | `story_page.c` COVER 计数块 `if(list!=NULL)`→`if(1)`（NULL 仍画计数） | `:749 ... KCN-C2 RED: 数据未达（list==NULL）时封皮仍画计数（带墨 82）` | 无 | golden:701 PASS |
| KCN-C3 | `story_page.c` COVER KEY 短去 `v->page=LIST` | `:797 ... KCN-C3 RED: COVER KEY 短按 act=13 page=7, 期望 COVER_OPEN(=13) 且 page=LIST(0)` | 无 | golden:781 PASS |
| KCN-C4 | `story_page.c` COVER KEY 长加 `v->page=LIST`（返回 GLOBAL 但页态错） | `:893 ... KCN-C4 RED: 封皮长按 act=-2 page=0, 期望 GLOBAL 且页态由全局层接管` | 无 | golden:864 PASS |
| KCN-C5 | `story_page.c` `kidpal_story_list_load` NULL 检查后加 `v->page=LIST` | `:971 ... KCN-C5 RED: list_load 改动了 page（7 -> 0）` | 无 | golden:929 PASS |
| KCN-C9 | `story_page.c` `#define KSTORY_ART_CACHE_SLOTS` 8→7 | `:1094 ... KCN-C9 RED: KSTORY_ART_CACHE_SLOTS = 7 < 8` | 无 | golden:1033 PASS |
| KCN-C10 | `story_page.c` `kidpal_story_counter` 恒返回 `list->count`（items 顶替 total） | `:1148 ... KCN-C10 RED: counter 语义漂移（total=0->0 / 2->2 / 12(items=2)->2）` | C5 tooth1 + SPV2-A7 + SPV2-A8（同 counter-total 族，见 §8.2） | golden:1124 PASS |
| C6 | `display_schema.py` 删 story_list floor 的 `if data=={}` 早退 | `:356 ... C6 RED: story_list 薄载荷 ... 被真门拒绝 (story_list items must be an array)` | 无（front_expands 子测仍绿） | `-k c6` 2 passed |
| C7 | `front_app.py` 删除后重推 `restore=True`→缺省 False | `:450 ... C7 RED: 删除执行后重推（:1047）未带 restore:true` | 无 | `-k c7` 5 passed |
| C8 | `front_app.py` 记录语境分支 `source_page=="story"`→`in("story","story_cover")` | `:550 ... 'accumulate' != 'chat' ... C8 RED: story_cover 误切记录语境（routing != chat）` | 无（story/unknown 子测仍绿） | `-k c8` 3 passed |

### 8.2 偏差与说明

- **C8 派发单建议打 `application.cc`**：该建议打不中 server 冻结件——C8 的唯一冻结承载 = `server/acceptance/test_story_cover_nav.acceptance.py`（真跑 `pump_client_to_upstream`），`application.cc` 是设备 TU、不进 host/server 构建、无任何冻结断言读它。故 C8 变异落在真实 seam `front_app.py` 的 `session_ctx` 记录语境分支（把 story_cover 误纳入 story → routing 变 accumulate），行为级且只红 `test_c8_story_cover_plain_qa`。
- **C10 变异选择**：先试 `kidpal_story_counter` 恒返回 count（dispatch 建议口径），能打红 C10 tooth1，但连带 C5 tooth1 + SPV2-A7 + SPV2-A8。因 C10 是**回归锚**（其「封皮计数只信 story_list.total」与这些断言同源），该连带全落在 counter-total 语义族内，逐条如实申报。另试过 `CMakeLists` 登记序变异（C10「host 登记序」tooth5），只能红 C10 但会把 host_infra smoke boundary 一起打红（smoke 执行序副作用，属**出族**连带），故弃用、以 counter 变异为准。
- **C1 连带 KB-A3**：prompt 明示允许。
- **C6** 只红 `test_c6_story_list_thin_payload_floor`（薄载荷放行半）；`test_c6_front_expands_thin_payload_not_downgrade` 走 `_expand_story_nav_intent` 不经该早退，保持绿——两半同属 C6 族，已覆盖主断言。
- C2/C3/C4/C5/C9 五条变异各自**只红目标 case**（逐轮 `301 Tests 4 Failures` = 3 存量 + 目标）。

### 8.3 还原与工作树证据

每次变异后 `git checkout -- <file>`，`git diff --stat <file>` 为空。轮末（全部还原、golden 重跑后）：
```
$ git status --porcelain
 M delivery/taskpackages/261104-k5-story-cover-nav/pod-events.jsonl    # 非本 agent 产出（并行会话追加）
$ git diff --stat -- firmware/ server/ tools/                          # 空（实现面零改动）
$ git diff --stat -- '*acceptance*' 'server/acceptance/' docs/         # 空（验收/文档零改动）
```
实现面与验收/文档面净零改动成立。

### 8.4 机读 JSON 数组（与任务 package `acceptance[]` 逐字对齐）

```json
[
 {
  "acceptance": "KCN-C1: 封皮四件（书封形态两向长墨段 / 标题「星星记录本」在墨 / 星星计数在墨 / 大星徽在墨）+ 四点页点第 4 实心 + 覆盖层态页点隐藏 + 键帽（打开 / 下一页）。回引 S-F2.6-AC1、requirements §2.4。设计态红 = 封皮 COVER 渲染分支与键帽行未落",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "firmware/main/display/keybar.c PAGE_P4 页槽行由「打开 / 下一页」改回列表残留行「说话·双击看 / 下移」",
  "command": "wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf",
  "evidence": "story_cover_nav.acceptance.c:676: ...(KCN-C1):FAIL ... KCN-C1 RED: P4 页槽键帽 (左=说话·双击看, 右=下移) 期望 (打开, 下一页)——封皮键帽「打开 / 下一页」未落（S-F2.6-AC1; 现状 (说话·双击看, 下移) = 列表行残留） || 汇总 301 Tests 5 Failures",
  "restoredCheck": "golden:597 封皮四件+四点第4实心+键帽 (KCN-C1):PASS",
  "collateral": "KB-A3 同红（允许，prompt 明示）"
 },
 {
  "acceptance": "KCN-C2: 封皮计数两态（数据已送达如实显示含 ×0 / 数据未达 rc=0 且计数带零墨不误显）+ render_node P4 分支出现 KSTORY_PAGE_COVER 分派。回引 S-F2.6-AC1 补充规则、S-F2.1-AC5/AC6。设计态红 = COVER 分支缺席",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "firmware/main/display/story_page.c COVER 渲染分支计数块 `if (list != NULL)` 改为 `if (1)`（数据未达 list==NULL 时仍画计数）",
  "command": "wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf",
  "evidence": "story_cover_nav.acceptance.c:749: ...(KCN-C2):FAIL ... KCN-C2 RED: 数据未达（list==NULL）时封皮仍画计数（带墨 82）——篇数未知不能显 ×0 误导（S-F2.1-AC6 同型判据） || 汇总 301 Tests 4 Failures",
  "restoredCheck": "golden:701 封皮计数两态 (KCN-C2):PASS",
  "collateral": "无（仅 C2）"
 },
 {
  "acceptance": "KCN-C3: 手势矩阵（COVER KEY 短→COVER_OPEN 进列表 / BOOT→GLOBAL / KEY 长→GLOBAL / KEY 双=NONE；LIST KEY 长→COVER_BACK；READER KEY 长→LIST；COVER 不进 300ms 双击消歧）。回引 S-F2.6-AC2/AC3、S-F2.1-AC2、requirements §2.3。设计态红 = COVER case 与新动作码缺席",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "firmware/main/display/story_page.c COVER KEY 短按块去掉 `v->page = KSTORY_PAGE_LIST;`（返回 COVER_OPEN 但不进列表）",
  "command": "wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf",
  "evidence": "story_cover_nav.acceptance.c:797: ...(KCN-C3):FAIL ... KCN-C3 RED: COVER KEY 短按 act=13 page=7, 期望 COVER_OPEN(=13) 且 page=LIST(0)——封皮 KEY 短按不进列表（S-F2.6-AC2） || 汇总 301 Tests 4 Failures",
  "restoredCheck": "golden:781 手势矩阵 COVER/LIST/READER (KCN-C3):PASS",
  "collateral": "无（仅 C3）"
 },
 {
  "acceptance": "KCN-C4: 页态逐层返回序列（阅读→列表→封皮→首页，每按一次只上一层）+ 板级 BOOT 入口 reset 结构锚（StoryEnterCover 调用点存在）。回引 S-F2.6-AC3/AC1、PROPOSAL §5。设计态红 = 入口 reset seam 缺席",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "firmware/main/display/story_page.c COVER KEY 长按块加 `v->page = KSTORY_PAGE_LIST;`（action 仍 GLOBAL 但页态错留 LIST，逐层返回第 2 层破）",
  "command": "wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf",
  "evidence": "story_cover_nav.acceptance.c:893: ...(KCN-C4):FAIL ... KCN-C4 RED: 封皮长按 act=-2 page=0, 期望 GLOBAL 且页态由全局层接管（封皮->首页） || 汇总 301 Tests 4 Failures",
  "restoredCheck": "golden:864 页态逐层返回序列 (KCN-C4):PASS",
  "collateral": "无（仅 C4；C3 tooth2 只校 act 不校 page，保持绿）"
 },
 {
  "acceptance": "KCN-C5: restore 三态（restore 帧只填缓存不导航且活动子态零重置 / 非 restore 完整帧 = 进场帧进封皮 / 畸形帧降级）+ kidpal_story_list_load 不改 page + mcp 路由结构锚（restore 分支不调 view_init）。回引 S-F2.6-AC1/AC4、S-F2.1-AC6、D-07/D-08。设计态红 = list_load 缺席、mcp 仍 view_init",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "firmware/main/display/story_page.c kidpal_story_list_load 在 NULL 检查后加 `v->page = KSTORY_PAGE_LIST;`（restore 帧导航，破「只填缓存不导航」）",
  "command": "wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf",
  "evidence": "story_cover_nav.acceptance.c:971: ...(KCN-C5):FAIL ... KCN-C5 RED: list_load 改动了 page（7 -> 0）——restore 帧必须只填缓存不导航、活动子态零重置（plan D; TECH-261022 churn 防线） || 汇总 301 Tests 4 Failures",
  "restoredCheck": "golden:929 restore 三态与缓存装载 (KCN-C5):PASS",
  "collateral": "无（仅 C5）"
 },
 {
  "acceptance": "KCN-C9: ic_coin_52_52.bin（KAP1 头、52x52、stride 7）+ 产线三处白名单登记 + KSTORY_ART_CACHE_SLOTS>=8。回引 S-F2.6、D-09、K5S-E3 离线半。设计态红 = 资产未产出、槽未扩容",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "firmware/main/display/story_page.c `#define KSTORY_ART_CACHE_SLOTS` 8→7（资产缓存槽未扩容）",
  "command": "wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf",
  "evidence": "story_cover_nav.acceptance.c:1094: ...(KCN-C9):FAIL ... KCN-C9 RED: KSTORY_ART_CACHE_SLOTS = 7 < 8——7 枚已被既有资产占满, 封皮再加资产会静默不可用（plan F 风险点; 须扩容为纯逻辑常量） || 汇总 301 Tests 4 Failures",
  "restoredCheck": "golden:1033 ic_coin 资产与产线白名单 (KCN-C9):PASS",
  "collateral": "无（仅 C9；源扫描 tooth4）"
 },
 {
  "acceptance": "KCN-C10: 回归锚（封皮计数只信 story_list.total / 删除同步减 / K0a 阅读键零回归 / 记录语境入口）+ host 登记序。回引 S-F2.1-AC1..AC7、S-F2.2-AC1、S-F2.3-AC1/AC2、S-F2.4-AC2、D-01。设计态红 = 登记序与新 seam 缺席",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "firmware/main/display/story_page.c kidpal_story_counter 改为恒返回 list->count（以 items.length 顶替 total）",
  "command": "wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf",
  "evidence": "story_cover_nav.acceptance.c:1148: ...(KCN-C10):FAIL ... KCN-C10 RED: counter 语义漂移（total=0->0 / 2->2 / 12(items=2)->2）——星星计数须恒信 story_list.total（契约 §1.3③ / S-F2.4-AC2 现存篇数） || 汇总 301 Tests 7 Failures",
  "restoredCheck": "golden:1124 回归锚+登记序 (KCN-C10):PASS",
  "collateral": "连带 C5 tooth1 + SPV2-A7 + SPV2-A8（同一 counter-total 语义族；C10 为回归锚，其「计数只信 total」与这些断言同源，连带在族内）"
 },
 {
  "acceptance": "C6: display_schema 放行 {\"view\":\"story_list\",\"data\":{}} 薄载荷 + 其余畸形仍拒 + 白名单零扩（无 story_cover view）+ front _expand_story_nav_intent 展开为非 restore 完整帧且 Q7 不误杀自展帧。回引 S-F2.6（契约 §1.3③/§1.5）。设计态红 = 薄载荷口与展开 seam 缺席",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "server/gateway/display_schema.py story_list floor 删除 `if data == {}: return (True, \"ok\")` 早退（薄载荷不再放行）",
  "command": "python -m pytest -q --import-mode=importlib -m \"not live\" server/acceptance/test_story_cover_nav.acceptance.py -k c6",
  "evidence": "test_story_cover_nav.acceptance.py:356: AssertionError: False is not true : C6 RED: story_list 薄载荷 `{'view':'story_list','data':{}}` 被真门拒绝 (story_list items must be an array) —— 契约 §1.3③/§1.5「增认 data:{} 导航薄载荷」未落地 || 1 failed, 1 passed, 8 deselected",
  "restoredCheck": "还原后 -k c6: `2 passed, 8 deselected`",
  "collateral": "无（仅 test_c6_story_list_thin_payload_floor；front_expands 子测仍绿）"
 },
 {
  "acceptance": "C7: 五调用点 restore 判定（ctx 建立 / 删除后 / 归档成功翻 true；WS 建连保持 true；归档失败离页帧保持 False = human 2026-11-15 裁定「离页导航帧例外」）。回引 S-F2.6-AC1/AC4、D-07。设计态红 = 三处未翻 true",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "server/gateway/front/front_app.py 删除执行后重推（:1074）`_push_story_list(websocket, restore=True)` 改回缺省 `_push_story_list(websocket)`（restore=False）",
  "command": "python -m pytest -q --import-mode=importlib -m \"not live\" server/acceptance/test_story_cover_nav.acceptance.py -k c7",
  "evidence": "test_story_cover_nav.acceptance.py:450: AssertionError: False is not true : C7 RED: 删除执行后重推（:1047）未带 restore:true —— 对账重推只填缓存不抢前台被破。 || 1 failed, 4 passed, 5 deselected",
  "restoredCheck": "还原后 -k c7: `5 passed, 5 deselected`",
  "collateral": "无（仅 test_c7_delete_repush_restore_true）"
 },
 {
  "acceptance": "C8: 语境三分（story_cover 普通问答不 acc.start / 不推 story_list / 不注记录信号；story 列表仍记录语境；未知 source_page 拒绝并 warning）。回引 S-F2.6-AC4（契约 §四）。设计态红 = story_cover 语境未接线",
  "turnedRed": true, "sensitive": true, "restored": true,
  "mutation": "server/gateway/front/front_app.py session_ctx 记录语境分支 `source_page == \"story\"` 扩为 `source_page in (\"story\",\"story_cover\")`（story_cover 误切记录语境）",
  "command": "python -m pytest -q --import-mode=importlib -m \"not live\" server/acceptance/test_story_cover_nav.acceptance.py -k c8",
  "evidence": "test_story_cover_nav.acceptance.py:550: AssertionError: 'accumulate' != 'chat' ... C8 RED: story_cover 误切记录语境（routing != chat）—— 封皮发起普通问答, 不得 acc.start（S-F2.6-AC4 / 契约 §四「普通问答语境」）。 || 1 failed, 2 passed, 7 deselected",
  "restoredCheck": "还原后 -k c8: `3 passed, 7 deselected`",
  "collateral": "无（仅 test_c8_story_cover_plain_qa；story/unknown 子测仍绿）"
 }
]
```

