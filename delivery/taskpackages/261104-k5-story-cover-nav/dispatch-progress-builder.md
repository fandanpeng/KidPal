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
