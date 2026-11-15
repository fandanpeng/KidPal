# dispatch-progress · acceptance-author · 261104-k5-story-cover-nav

> 执行体：acceptance-author（Pod m2 worktree，分支 dev/pod-m2，包 base ref `03630b65`）
> 日期：2026-11-15
> 任务：为「故事封皮导航」包冻结可执行验收（新件两枚 + 既有冻结件最小重锚），跑红基线。

## 1. 做了什么

1. 消费封闭输入：taskpackage.json（sourceRefs / exitCriteria K5S-E1..E6 / impactedTestsSeed /
   changeManifest / decisionCoverage 全查）、plan.md 技术方案节（A–H）、input-brief.md；自定位
   `docs/contracts/interface-contract.md` §1.3③（:92）/§1.5（:250）/§四 kidpal_session_ctx（:347）。
2. 反查 decisionCoverage 六条落点：D-01→K5S-E1、D-07-story→K5S-E2、D-08-story→K5S-E2、
   D-09-cover→K5S-E3、目的8(TECH-261028)→K5S-E6、§5 收口→K5S-E5。D-01/D-07/D-08/D-09 的验收
   承载分别落在本批新件与重锚件（见 §2/§3）；D-09 资产面离线半 = KCN-C9，真机可见半挂 K5S-E3
   设备轮；§5 跨包联合收口按 plan 明确交主线程（不在本 Pod 自证，见 §6 未覆盖面）。
3. 真读代码实况：story_page.c/.h（view_init / page_input / render / list_gate / ctx_source_page /
   KSTORY_ART_CACHE_SLOTS=7）、keybar.c/.h（页槽表 P4 行 / story_slots 表）、kidpal_render_node.cc
   （KP_PAGE_STORY 分支 / s_story_data_received / story_substate）、mcp_server.cc（VRT_ROUTE_STORY_LIST
   区 / StoryDispatchAct）、application.cc:Listening 分支 ctx 发送、front_app.py（_push_story_list
   与五调用点 :1023/:1047/:1728/:1739/:2453 / _consume_kidpal_control）、display_schema.py
   （story_list floor）。跨端对端解析器（§②）已对照对端实际接受形状钉断言（story_list floor
   现要求 items 为数组、restore 在场须布尔、未知 source_page 记 warning）。
4. 落冻结件两枚（见 §2）、既有件最小重锚五枚 + 登记（见 §3）、红基线实跑（见 §4）。
5. 偏离与边界（见 §5），未覆盖面与风险（见 §6）。

## 2. 冻结清单（新件）

### A. `firmware/main/test/story_cover_nav.acceptance.c`（host runner）
- KCN-C1 封皮四件 + 四点第4实心 + 键帽 —— S-F2.6-AC1（book 形态/标题/计数/徽章真帧探针 +
  keybar dots/slots）
- KCN-C2 封皮计数两态 —— S-F2.6-AC1 / S-F2.1-AC5 / S-F2.1-AC6（×0 诚实 / 数据未达不显 + render_node
  结构锚）
- KCN-C3 手势矩阵 COVER/LIST/READER —— S-F2.6-AC2 / S-F2.6-AC3 / S-F2.1-AC2（含 COVER 不进
  300ms 消歧）
- KCN-C4 页态逐层返回序列 —— S-F2.6-AC3 / S-F2.6-AC1（含板级 BOOT 入口 reset 结构锚 StoryEnterCover）
- KCN-C5 restore 三态 + list_load 缓存装载 —— S-F2.6-AC1 / S-F2.6-AC4 / S-F2.1-AC6（含 mcp story 路由
  结构锚：list_load + page=COVER + 零 view_init）
- KCN-C9 ic_coin 资产 + 产线白名单 + 缓存槽扩容 —— S-F2.6 / D-09（KAP1 头 52x52/stride7/380B +
  gen_art_spiffs / gen_assets / 模拟器 assets.c 三处登记 + KSTORY_ART_CACHE_SLOTS>=8）
- KCN-C10 回归锚 + host 登记序 —— S-F2.1 / S-F2.2-AC1 / S-F2.3-AC1/AC2 / S-F2.4-AC2（counter 只信
  total / 删除同步减 / K0a 阅读键 / 记录语境 / CMake 登记序）

Seam contract 冻结（home = 实现落点，详见文件头）：KSTORY_PAGE_COVER=7；KSPV2_ACT_COVER_OPEN=13 /
COVER_BACK=14；`kidpal_story_list_load(v,list)`（只填缓存、page 零重置、NULL→-1）；封皮渲染
`list==NULL`=数据未达不画计数且 rc=0；COVER KEY 短→LIST / LIST KEY 长→COVER；板级
`StoryEnterCover`。

### B. `server/acceptance/test_story_cover_nav.acceptance.py`
- C6 display_schema 放行 `{"view":"story_list","data":{}}` + 畸形仍拒 + 白名单零扩 + front 截获展开 ——
  S-F2.6（契约 §1.3③/§1.5）
- C7 五调用点 restore 判定 —— S-F2.6-AC1 / S-F2.6-AC4（:1023/:1047/:1739 翻 true 行为断言；:2453
  保持 true 结构锚；:1728 归档失败离页帧保持 False 行为断言，注明「离页导航帧例外」human 2026-11-15
  裁定）
- C8 语境三分 —— S-F2.6-AC4（story_cover 普通问答不 acc.start/不推列表/不注记录信号；story 列表仍
  记录语境；未知值拒绝并 warning）

真实断言：每 TEST_CASE 至少一处闸认形态（C 件 `assert(...)` OOM 守卫 / Python `self.assertX`）。

## 3. 重锚清单（既有冻结件，最小改动）

| 文件 | 改动 | 为什么 |
|---|---|---|
| `firmware/main/test/story_page_v2.acceptance.c` | SPV2-A1 tooth 3：LIST KEY 长按由 `GLOBAL` 翻正为 `COVER_BACK(14)` 且 page→COVER；新增 `KSPV2_PAGE_COVER=7`/`KSPV2_ACT_COVER_BACK=14` 镜像；头注 LIST 行决策表 + [SL1] restore 说明同步 | S-F2.6-AC3 逐层返回「列表 → 封皮」；`:976` view_init page==LIST 语义**保持不动** |
| `firmware/main/test/keybar_three_slot.acceptance.c` | P4 页槽行 `(说话·双击看, 下移)`→`(打开, 下一页)`；新增 LIST 子态键帽断言 `kidpal_keybar_story_slots(LIST)==(说话·双击看, 下移)`；新增该缝 weak 哨兵 + 头注 | S-F2.6-AC1 封皮键帽；列表键帽行迁入 story_slots（防云端列表窗闪成封皮行） |
| `firmware/main/test/record_session_live.acceptance.c` | 新增 TEST 11 / RSL-A10：Listening 分支 `"story_cover"` 字面 + KP_PAGE_STORY + 封皮子态判据结构断言；[S1] 签名断言不动（`ctx_source_page(STORY)=="story"` 作回归参照） | 契约 §四 source_page 闭集扩 story_cover（S-F2.6-AC4） |
| `firmware/main/test/chrome_ghost.acceptance.c` | CG-A2：LIST 加入过渡窗在途集（新增「过渡窗 + LIST == story_slots(LIST)」断言；LIST 移出「非在途」回落集；头注/GAP-4/coverage 行同步） | 列表下沉封皮子页后云端列表渲染窗键帽不得闪成封皮行 |
| `server/acceptance/test_m4k1_story_cloud_link.acceptance.py` | A3 时点②（归档成功）/ A4 时点③（删除后）`assertFalse(restore)`→`assertTrue(restore is True)` | 契约 §1.3③ LANDED：后台恢复/对账推送恒 restore:true（D-07） |
| `server/acceptance/test_m4k3_record_session_live.acceptance.py` | A7 首帧 ctx(story) 恰一帧语义不变，补 `restore is True` 断言 | ctx 建立恢复推送（:1023）翻 true |
| `firmware/main/test/host_app/main/CMakeLists.txt` | **未能登记（scope-map 拒绝，见 §5 偏离 D1）**；已备好登记行待 builder 落 | 新件进 host runner（smoke 之前） |

## 4. 红基线（如实）

### host 面（`cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./espidf_wsl.sh build`
### + `wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf`）

- **编译**：绿（error 零）。新件因 CMake 登记被 scope 拒（偏离 D1），红基线跑法：临时把
  `#include "story_cover_nav.acceptance.c"` 追加进 `host_infra_smoke.acceptance.c` 编入 build
  验证一次，跑完**已还原**（工作树无残留）。另以 build/compile_commands.json 原 flags 对
  `story_cover_nav.acceptance.c` 单独 `-fsyntax-only` 实编（独立 TU，exit 0）——§4「C 类冻结验收
  实编：验收文件本身可编译」证据。
- **临时线束跑（含新件）**：`300 Tests 15 Failures`。
  - 新件 7 条全红（KCN-C1/C2/C3/C4/C5/C9/C10 各 1），红因逐条 = 实现缺席（COVER 渲染分支/
    list_load/新动作码/资产/登记缺失）。
  - 重锚 4 条红：SPV2-A1（LIST KEY 长仍 GLOBAL）、KB-A3（P4 槽仍列表行）、CG-A2（过渡窗 LIST 仍
    回落）、RSL-A10（Listening 无 story_cover）。
  - 另 4 条与本包无关：p1_auxbar_style AB-A1/A2/A4（**存量红**，本包未触碰）；host_infra_smoke
    boundary 1 条 = 临时线束把渲染测试排在 smoke 前导致的探针「已渲染」假阳（**线束产物，非真实
    红**，还原线束后消失）。
- **还原线束后正式跑（builder 当前可见态，不含未登记新件）**：`293 Tests 7 Failures`。
  - K5 设计态红 4 条：SPV2-A1 / KB-A3 / CG-A2 / RSL-A10（红因全为实现缺席）。
  - 存量红 3 条：p1_auxbar_style AB-A1/A2/A4。
  - 无环境红。

### server 面（`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`）

- 全量：`9 failed, 448 passed, 9 deselected, 33 subtests passed`。
  - 9 红 = 本批设计态红，无其它存量红：新件 6（C6×2 + C7 ctx/delete/archive-success + C8 story 列表
    restore）、M4K1 2（时点②/③ restore 翻正）、M4K3 1（ctx restore）。
  - 分件：`test_story_cover_nav` 6 failed / 4 passed（4 绿 = C7 archive-failure 离页帧 False、C7
    建连锁结构、C8 story_cover 普通问答、C8 未知值 warning）；`test_m4k1` 2 failed / 34 passed；
    `test_m4k3` 1 failed / 9 passed。
- 红因全为实现缺席（display_schema 未放行 thin / front 无展开 / 三处推送缺 restore）。

## 5. 偏离与理由

- **D1（blocker，交回 planner/builder）**：派发单要求我登记
  `firmware/main/test/host_app/main/CMakeLists.txt`，但 scope-map 强制拒绝（acceptance-author 写面
  仅 `firmware/**/*.acceptance.c` / `*/acceptance/**` / `tools/**` / `dispatch-progress`）。同因
  `firmware/main/test/chrome_ghost.test.c` 也拒写。→ 请 builder 在 CMakeLists 的
  `HOST_ACCEPTANCE_SRCS` 中、`host_infra_smoke.acceptance.c` 之前追加
  `"../../story_cover_nav.acceptance.c"`（KCN-C10 tooth 5 会校验该登记与顺序）；并在
  `chrome_ghost.test.c` 补「过渡窗 + LIST」白盒用例（acceptance 侧 chrome_ghost.acceptance.c 已覆盖
  行为，test.c 仅补白盒分支）。
- **D2**：封皮渲染落 `story_page.c`（plan A 技术裁定），本件按此冻结 seam；若 builder 选择落
  `kidpal_render_node.cc`（设备专属、host 不可编），KCN-C1/C2 的 host 真帧断言不可满足 → 停手升级。
- **D3**：C9 资产 offline 半只证「生成件 + 产线登记 + 缓存槽」，设备可见（真机目击）归 K5S-E3
  设备轮。
- **D4**：未加 `device-outcome` / `host-uncompilable-path` 标记——本批验收以 host 行为面 + 结构
  源锚为主，真机项（BOOT 四页并集 / 逐层返回手感 / 语音端到端 / restore 不抢前台时序）归 K5S-E4，
  不在这两件上打设备标记（避免与 launch-card 申报面错位）。

## 6. 未覆盖面 / 风险

- **真机面（K5S-E4）**：BOOT 四页并集与逐层返回手感、封皮观感（uiuxLevel 4 design review）、
  语音「打开故事」端到端、restore 不抢前台的真实时序、删除/盖章计数与记录会话并集——host 不可证，
  挂设备轮。
- **跨包联合收口（§5）**：全项真机 + 真实 QQ 轮由主线程另组织，本 Pod 只收导航并集（K5S-E5）。
- **结构源锚代理的强度**：mcp story 路由「list_load + page=COVER + 零 view_init」、application
  Listening「story_cover + 封皮子态门」、板级「StoryEnterCover」均为源码切片断言（设备 TU host 不可
  链），真机行为本体归设备轮；已按 acceptance-evidence 申报为源锚代理。
- **`:1728` 离页帧例外**：以行为断言钉 `restore` 保持 False 并注明 human 裁定；契约措辞排除建议已要求
  review-packet 留痕（本件不能改契约）。
- **资产缓存槽**：仅断言 `KSTORY_ART_CACHE_SLOTS >= 8`（内部常量，源码解析）；若设计 review 再增
  封面资产导致槽数不够，需重估。

## 7. input-brief §④ 清单外阅读 + 原因（原样回传，供 planner 记 over-scan）

| 清单外文件 | 原因（一句话） |
|---|---|
| `firmware/main/display/story_page.c` | 预评给的行号实读封皮落点/render/input/list_gate/资产槽，钉行为断言 |
| `firmware/main/display/keybar.c` / `keybar.h` | 钉 P4 页槽行与 story_slots 表现况 + 声明 |
| `firmware/main/display/kidpal_render_node.cc` / `.h` | KP_PAGE_STORY 分支 / s_story_data_received / substate 实况 |
| `firmware/main/mcp_server.cc` | VRT_ROUTE_STORY_LIST 区 restore/view_init 现状（C5 结构锚） |
| `firmware/main/application.cc` | Listening 分支 ctx 发送点（RSL-A10 结构锚） |
| `firmware/main/display/kidpal_layer.h` | KIDPAL_LAYER_NONE 冻结值 |
| `firmware/main/key_gesture_parser.h` | PAGE_COUNT/页面枚举/手势枚举 |
| `firmware/main/display/kidpal_font.h` | host 字体 mount seam（封皮标题 CJK 真帧） |
| `firmware/main/test/story_typography.acceptance.c` | 帧探针/mount 先例形态（非清单内，作形态参照） |
| `firmware/main/test/host_app/main/host_infra_smoke.acceptance.c` / `CMakeLists.txt` | 登记序与线束形态 |
| `server/gateway/front/front_app.py`（多段） | 五调用点 + ctx/delete/expand 实况（§② 必读，部分在清单外行号） |
| `tools/asset-pipeline/gen_art_spiffs.mjs` | KAP1 头格式与 WHITELIST 结构（C9 断言形状） |
| `docs/ui/V9/story-cover.png` | 封皮覆盖带坐标（页级 PNG = 布局权威） |

## 8. 冻结件缺陷最小修（2026-11-15，builder 轮后回派）

- **缺陷**：`firmware/main/test/story_page_v2.acceptance.c` SPV2-A1 跨用例状态残留——
  tooth 3（:1009）`kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS)` 按本包
  新冻结语义把 `v.page` 置为 COVER(7)；tooth 4（:1017）未复位页态，同一 `v` 上发 BOOT 短按
  落在 COVER 行 → GLOBAL(-2)，断言期望 CURSOR_NEXT(3)。此断言与 KCN-C3 冻结的「COVER BOOT
  族 = GLOBAL」+「LIST KEY 长 → COVER_BACK 且 page=COVER」互斥，任何实现都不可能同时满足
  （K4 pod-retro 教训①同族：跨用例一致性缺陷）。红基线期未暴露——当时 tooth 3 先红、测试
  中止，从未跑到 tooth 4。
- **修正（最小、纯 additive）**：tooth 4 之前加一行 `v.page = KSPV2_PAGE_LIST;` + 一行逐字
  留痕注释。其余断言一字未改、未删 tooth、未弱化任何断言。
- **复跑（host 全量）**：`espidf_wsl.sh build` 编译绿（error 零）→ host_infra_test.elf
  `301 Tests 3 Failures`，3 红全为 p1_auxbar_style AB-A1/A2/A4 存量族（本包未触碰）；
  SPV2-A1 转 PASS，K5 本批验收面全绿。
- **同族隐患扫描（只报告不改）**：story_page_v2 内 SPV2-A4（tooth 循环逐次 view_init +
  显式页态）、A6（显式 screen 复位 + open_reader 重调）、A7（逐段 view_init + 显式页态）、
  A8（view_init 重调）、B1/SLP-SLA5（渲染前显式页态）均无残留；story_cover_nav 的
  KCN-C3/C4/C5/C10 每条 tooth 前均 `kcn_view_cover` / `view_init` / 显式 `v.page` 复位，
  KCN-C4 的逐层序列是刻意状态推进（每步断言 page 期望值）；chrome_ghost CG-A2 与
  keybar KB-A3 迁入断言均为纯函数参数驱动、无共享页态；RSL-A10 为源码切片区断（无状态）。
  未发现其它同族「tooth 间状态残留」隐患。

## 9. 验收覆盖标记补齐（2026-11-15，acceptance-coverage 闸红回派）

- **触发**：`verify.mjs --phase=scoped` 的 `acceptance-coverage` 闸红，`missingCoverage` =
  KCN-C1…C5、KCN-C9、KCN-C10、C6、C7、C8。根因 = 闸判据 `hasCoverageMarker()` 只认紧随
  `coverage:` / `covers:` / `@coverage` 之后的**字面 id**（`(?:coverage|covers|@coverage)\s*:?\s*<id>\b`）；
  原文件仅有 `coverage: S-F2.6-AC1 -> KCN-C1` 指向式写法与 TEST_CASE 名里的 `(KCN-C1)`，均不匹配。
- **动作（纯追加注释，断言语义/实现零改）**：
  - `firmware/main/test/story_cover_nav.acceptance.c`：在 7 个对应 TEST_CASE 上方注释块内各加一行——
    `/* coverage: KCN-C1 */`、`/* coverage: KCN-C2 */`、`/* coverage: KCN-C3 */`、`/* coverage: KCN-C4 */`、
    `/* coverage: KCN-C5 */`、`/* coverage: KCN-C9 */`、`/* coverage: KCN-C10 */`。
  - `server/acceptance/test_story_cover_nav.acceptance.py`：三个测试类上方各加一行
    `# coverage: C6` / `# coverage: C7` / `# coverage: C8`（Python 语法合法）。
- **复跑**：
  - `node .claude/scripts/acceptance-coverage-check.mjs delivery/taskpackages/261104-k5-story-cover-nav/taskpackage.json`
    → `ok: true`（exit 0，10 个 id 全绿，structural/suiteGates 均空）；无其它缺项。
  - host 全量：`espidf_wsl.sh build` 编译绿（error 零，仅既有 truncation warning）→
    `host_infra_test.elf` `301 Tests 3 Failures 0 Ignored`，3 红全为 p1_auxbar 存量族。
  - server：`pytest -q --import-mode=importlib -m "not live" server/acceptance` →
    `457 passed, 9 deselected, 33 subtests passed`。
- 未提交（planner 代提）。

## 10. 真机轮回归断言补钉（2026-11-15，acceptance-author 回派）

- **缺口**：真机实证（22:11:39）front 已下发非 restore 完整帧
  `story_list_dispatch items=16 counter=19 degrade=false`，板端却打
  `story_list_gate defer_front page=7 voice_front=1 restore=0`——VOICE 前台判据把显式导航帧
  吞成「只装缓存」, P4 永不切换。既有断言只查符号存在, 若有人把 `story_voice_front` 重新喂回
  gate, 现有用例仍会全绿（无判别力）。
- **补钉（折进 KCN-C5, 不新开 id）**：`firmware/main/test/story_cover_nav.acceptance.c` 的
  KCN-C5 新增 tooth 5（:1029-1067）, 用与既有结构齿同款 `kcn_region_between` 切片,
  对 `firmware/main/mcp_server.cc` 的 `case VRT_ROUTE_STORY_LIST:`→`case VRT_ROUTE_STORY_TEXT:`
  区断言四件：
  1. 负锚（先判）`kidpal_story_list_gate(story_voice_front` = 0 处（禁回喂; 回喂即红）；
  2. 正锚 `kidpal_story_list_gate(0` >= 1 处（导航帧不看 voice_front）；
  3. `story_nav_takeover` 在场；
  4. `KSTORY_PAGE_RECORDING` 同区共现（录音演出页保护仍在; 到达时页态驱动, gate(0,RECORDING)==1）。
  失败消息锚定「导航帧被 voice_front 吞 = 2026-11-15 真机形态复潮」。coverage 标记仍沿用既有
  `/* coverage: KCN-C5 */`（无新 id, 不扩 mutation-check 证据面）。
- **顺带注释意图收窄**：SLP-SLA4 tooth 1 的 `KIDPAL_LAYER_VOICE` 结构判据原注释意图
  「VOICE 前台即 defer」已按真机证据收窄——只改注释意图说明, 判定/断言条件/测试逻辑一字未改
  （文件 = `story_page_v2.acceptance.c`；派发单给的 `record_session_live.acceptance.c` 无 SLP-SLA4,
  该 id 实落 story_page_v2, 见偏离 D5）。
- **正向 PASS 证据**：host 全量 `301 Tests 3 Failures 0 Ignored`, KCN-C5 `:PASS`（注释意图改后
  SLP-SLA4 亦 `:PASS`）, 3 红全为 p1_auxbar_style AB-A1/A2/A4 存量族（本包未触碰）。
- **反向变红证据**：shade-root 复演——把 `kidpal_story_list_gate(0` 改成
  `kidpal_story_list_gate(story_voice_front` 的 `mcp_server.cc` 副本置入临时仓根后跑同一 ELF,
  KCN-C5 `:FAIL`（:1052）「story 路由区仍把 story_voice_front 回喂 gate（1 处）——导航帧被
  voice_front 吞 = 2026-11-15 真机形态复潮…」。**未触碰实现文件**：仓库 `mcp_server.cc`
  sha256 前后恒 `a69ef4bf…`, `git diff` 为空（=`git checkout` 还原等价且零残留风险; 另存
  `c:/tmp/k5_mcp_fixed.cc` 作双保险）。
- **coverage 闸**：`acceptance-coverage-check.mjs` → `ok: true`（10 个 id 仍全绿, KCN-C5 在列）。
- **偏离 D5**：派发单写「record_session_live.acceptance.c 的 SLP-SLA4 tooth 1」, 实查 SLP-SLA4
  位于 `story_page_v2.acceptance.c:2218`（record_session_live 对应的是 RSL-A7, 其 gate 决策表断言
  `gate(1,LIST)==1` 属函数本体, 本次修复 gate 本体零改, 无需动）。按「SLP-SLA4 tooth 1 +
  KIDPAL_LAYER_VOICE 结构判据」唯一可定位, 改在 story_page_v2 的注释。
- 未提交（planner 代提）。
