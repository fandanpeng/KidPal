# Review Packet · 261104-k5-story-cover-nav

> 包：故事封皮导航——P4 封皮、列表/阅读下沉、逐层返回与 story_list 共享帧消费（S-F2.6 AC1–AC4 + S-F2.1 AC1–AC7 回归 + S-F2.2 AC1 + S-F2.3 AC1/AC2 + S-F2.4 AC2 现存计数半）
> base ref：`03630b65`（包首提交父，包级 diff 恒用此 ref）｜集成 HEAD：`8de12f5a`｜worktree `C:/AI/KidPal-pod-m2`，分支 `dev/pod-m2`
> 本 Packet 供 supervisor 机械闸 + 人工复核；不自评通过。

## Change Size

- **Files changed**：28 个实现/工具文件（`firmware/` 19、`server/` 5、`tools/` 4；`git diff --numstat 03630b65..HEAD -- firmware server tools`），另 delivery 簿记件若干不计入实现面。
  - 新资产 1 枚：`firmware/main/assets/spiffs_root/ic_coin_52_52.bin`（KAP1，52×52，stride 7，380 B）
  - 生成件：`tools/simulator/LvglWindowsSimulator/assets/assets.c` +459 / `assets.h` +1（管线自动生成，纯追加非手写）
  - 新冻结验收件 2 件：`firmware/main/test/story_cover_nav.acceptance.c`（+1208）、`server/acceptance/test_story_cover_nav.acceptance.py`（+609）
- **Net diff**：**+2846 / −70**（firmware +1697/−48、server +680/−22、tools +469/−0）；不含 `delivery/`。
- **设备 TU（`hostUncompilablePaths`，不进 host 构建，靠 tree-scan 源锚代理）**：`application.cc` +10、`mcp_server.cc` +37/−?、`kidpal_render_node.cc` +19、`lcd_display.cc` +9、板级 `waveshare-s3-rlcd-4.2.cc` +9/−?（合计 5 TU +77/−7）。
- **Scope**：固件 P4 封皮子态（`KSTORY_PAGE_COVER`）+ COVER 渲染/输入表 + `kidpal_story_list_load` 缓存装载；keybar P4 页槽改封皮行 + story_slots/过渡窗补 LIST；board BOOT 钩子 `StoryEnterCover`；front `_expand_story_nav_intent` 薄载荷展开 + 三时点 restore:true（含 :1728 例外）；display_schema story_list 薄载荷放行；资产管线尾部追加 `ic_coin_52_52` job + 白名单。
- **Split recommendation**：**不拆**。封皮子态、列表/阅读下沉、逐层返回与 story_list 共享帧消费必须同批落地（拆开会留下「封皮入口存在但 restore 语义未对齐 → 后台推送抢前台」或「薄载荷被拒 → 语音打开故事失效」的中间态）；`ic-coin` 与书封几何依赖同批 KEY 页槽行。

## Verification Evidence

- **final 轮承重 fact**：`delivery/taskpackages/261104-k5-story-cover-nav/verifier-evidence/rounds/r3.json`
  - phase=`final`，round=`3`，invocationId=`8a33b02c-611b-4097-9d06-cb151bd8b7f0`，observedGateExit=0，`ok:true`
  - **轮序说明（如实留痕）**：本包 scoped 相位已把 round 1（`r1.json`，ok:false 首轮红）与 round 2（`r2.json`，8/8 绿）落为承重 fact，占用共享 round 序数 1–2；`verifier-evidence-check.mjs` 的 `preflightProducer` 强制 `round === 已完成数 + 1`（rounds/ 目录连续序），故 **final 承重轮序数 = 3**（写 `r3.json`），非「从 1 起」——「从 1 起」是 K4 先例（K4 scoped 未落承重 fact），本包机制上不可行，按机械规约取 3。
  - 机械复核：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261104-k5-story-cover-nav --phase final --round 3 --invocation-id 8a33b02c-611b-4097-9d06-cb151bd8b7f0` → exit 0，`factOk:true`，observedGateExit=0。
- **test-scope 清单落盘路径**：`delivery/taskpackages/261104-k5-story-cover-nav/test-scope-final.txt`（25 条测试文件：本包 2 冻结件 + impactedTestsSeed 7 + 共享 seam 12 + 安全余量 4；含非测试件登记与排除项留痕）。
- **范围圈定依据**：`detect_changes`（project `C-AI-KidPal-pod-m2`，since=`03630b65`，merge_base=`03630b65`，direction=both，depth=2）→ 42 changed files / 275 seed 符号 / 297 impacted 节点（firmware/main 181、server/gateway 75、server/acceptance 28、server/tests 2、builtins 10、firmware/scripts 1）× taskpackage `impactedTestsSeed` 7 条 × crossEnd seam 合成；per-Pod 专属图索引可用 → 正常圈定（未走降级路径）。
- **final r3 逐闸（全跑零 skip）**：taskpackage-required PASS / change-manifest PASS（pre-cutoff 豁免声明在场）/ **firmware-build PASS**（真编译，`xiaozhi.bin` 0x328ff0，8% 余）/ contract-lint PASS（仅一条既有断链 warning）/ acceptance-coverage PASS（10 acceptance id：KCN-C1..C5、C9、C10、C6、C7、C8）/ mutation-check PASS（10 条 perAcceptance）/ docs-sync PASS / security PASS（`securityReviewRequired=false`）/ **unit PASS**（timing=full，182 passed + 22 subtests，liveDeselected=0）/ **acceptance PASS**（timing=full，server 面 90 passed + 3 subtests，liveDeselected=0；固件面 firmwareAcceptanceFiles=12 / serverAcceptanceFiles=6 结构核对）。**零 notExecuted 跳过**。结构闸结果不复述为 finding。
- **host runner 全量（integrator 独立实跑，K0 锚命令 `wsl -d Ubuntu-24.04 -- ./espidf_wsl.sh build && ./build/host_infra_test.elf`）**：
  - 结果 = **301 Tests / 3 Failures / 0 Ignored**。
  - **3 红全属 `p1_auxbar_style.acceptance.c` 存量族**：AB-A1（`ic_batt_15` 消费点未接入，仍走 ic_batt_48 缩放旧路径）、AB-A2（`ic_thermo_12` 落位漂出授权域）、AB-A4（`ic_wifi_14` 消费点缺失）——与本包 launch 基线 **292 Tests / 3 Failures（同三条 @03630b65）** 同族同项。
  - 301 − 292 = 9 = 本包新增测试件；**7 条 KCN host case 全 PASS**（KCN-C1/C2/C3/C4/C5/C9/C10），另重锚 **SPV2-A1 / KB-A3 / CG-A2 / RSL-A10 全转 PASS**、builder `chrome_ghost.test.c` TEST 3b PASS。⇒ **K5 零新增红**，3 红为既有 infra 债（不归本包修，见 Residual Risk）。
- **server 面（final r3 acceptance 闸实跑 scope 内 6 件）**：90 passed / 3 subtests，0 live deselected。服务端红基线（launch @03630b65）447 passed + 9 设计态红；builder 实现后 457 passed / 0 failed；final scope 内 6 件 90 passed 零红。
- **Mutation check**：10/10 逐条变异→目标冻结断言转红→精确还原复跑绿；细节见下「变异证据」节（详表在 `taskpackage.mutationEvidence.perAcceptance` 与 `dispatch-progress-builder.md §8`）。
- **Docs sync**：`docsSyncRequired=false`；人工复核成立，详见 `docs-delta.md`（requirements / architecture / contracts / docs/ui 四文件零 diff）。

## 冻结验收状态（10 acceptance id + 6 重锚件；K5S-E1..E6）

| 验收 / 重锚 | 状态 | 载体 / 证据 |
|---|---|---|
| KCN-C1 封皮四件 + 四点第 4 实心 + 覆盖层隐藏 + 键帽 | 绿 | host `story_cover_nav.acceptance.c:597` PASS（301/3 全量内） |
| KCN-C2 封皮计数两态 + render_node COVER 分派 | 绿 | host `:701` PASS |
| KCN-C3 手势矩阵（COVER 短/BOOT/长/双；LIST 长=COVER_BACK；READER 长=LIST；不进 300ms 消歧） | 绿 | host `:781` PASS |
| KCN-C4 页态逐层返回 + 板级 BOOT 入口 reset 结构锚 | 绿 | host `:864` PASS（board + mcp_server.cc 双侧扫 `StoryEnterCover` 字面） |
| KCN-C5 restore 三态 + list_load 不改 page + mcp 路由结构锚 | 绿 | host `:929` PASS |
| KCN-C9 ic_coin KAP1 + 产线三处白名单 + ART_CACHE_SLOTS≥8 | 绿 | host `:1033` PASS |
| KCN-C10 回归锚（计数只信 total / 删除同步减 / 阅读键零回归 / 记录语境入口）+ host 登记序 | 绿 | host `:1124` PASS |
| C6 display_schema 薄载荷放行 + 畸形仍拒 + 白名单零扩 + front 展开 + Q7 不误杀 | 绿 | server `test_story_cover_nav.acceptance.py -k c6` 2 passed |
| C7 五调用点 restore 判定（含 :1728 离页帧例外） | 绿 | server `-k c7` 5 passed |
| C8 语境三分（story_cover 普通问答 / story 记录 / 未知拒） | 绿 | server `-k c8` 3 passed |
| **重锚** SPV2-A1（story_page_v2） | 绿 | host PASS（tooth 3→4 缺页面复位由 acceptance-author 最小修后重冻） |
| **重锚** KB-A3（keybar_three_slot，P4 封皮行 + LIST 行） | 绿 | host PASS |
| **重锚** CG-A2（chrome_ghost，过渡窗补 LIST） | 绿 | host PASS |
| **重锚** RSL-A10（record_session_live，story_cover 语境扩点） | 绿 | host PASS |
| **重锚** M4K1（server，restore 断言翻正 + :1728 例外） | 绿 | server acceptance PASS |
| **重锚** M4K3（server，恰一帧 + 补 restore=True） | 绿 | server acceptance PASS |

K5S-E1（Pod 内冻结验收全量 + 红基线/变异齐全）离线面达成；K5S-E2（story_list 共享帧只信 total + 三推送 restore:true + 薄载荷只进封皮）离线达成；K5S-E3（ic-coin/书封经产线 + V9 design review）离线半达成、真机观感待 K5S-E4；K5S-E4 真机轮**未跑**（见未覆盖面）；K5S-E5（导航并集 owner + review-packet 对账 PROPOSAL §5）本 Packet §对账 达成、跨包联合收口归主线程；K5S-E6（TECH-261028 只回归不重修，diff ⊆ manifest）达成。

## 红基线 → 绿证据链

1. **红基线 @03630b65**（acceptance-author 冻结实跑 + planner 独立复跑，两次数值一致）：host = 293 Tests / 7 Failures（4 设计态红 SPV2-A1 / KB-A3 / CG-A2 / RSL-A10 + 3 存量 p1_auxbar AB-A1/A2/A4）；server = 9 failed / 448 passed / 9 deselected / 33 subtests（9 红全本批设计态红，零存量污染）。
2. **builder 实现后**：server 457 passed / 0 failed / 9 deselected（9 设计态红全绿、零新增红）；host 301 Tests / 4 Failures（3 存量 + SPV2-A1 冻结件自身 tooth 序缺陷）。
3. **SPV2-A1 冻结件缺陷修复**（acceptance-author 最小修 e2cf90e0，不改实现）：tooth 3 把 `v.page` 置 COVER 后 tooth 4 未复位；修 = tooth 4 前 `kidpal_story_view_init` 复位到 LIST。修后 host 301 / 3（仅存量 p1_auxbar）。
4. **coverage 标记形态修**（406c1953）：acceptance-coverage 首轮红因 coverage 标记形态不匹配（非断言缺陷）→ 补精确标记。
5. **变异证据 10/10**（8de12f5a）：每条 acceptance 变异→转红→还原复跑绿。
6. **integrator final 承重轮 r3**：10 闸全绿零 skip；host 全量独立复跑 301/3（3 红全存量）；server scope 内 90 passed。零新增红。

## 变异证据（10/10 摘要；详表见 `taskpackage.mutationEvidence.perAcceptance` 与 `dispatch-progress-builder.md §8`）

| 验收 | 变异点（文件·改动） | 目标转红 | 还原复绿 |
|---|---|---|---|
| KCN-C1 | keybar.c P4 页槽行改回列表残留行 | `:676 KCN-C1 RED` 键帽期望(打开/下一页) | golden:597 PASS |
| KCN-C2 | story_page.c COVER 计数块 `if(list!=NULL)`→`if(1)` | `:749 RED` 数据未达仍画计数 | golden:701 PASS |
| KCN-C3 | story_page.c COVER KEY 短去 `page=LIST` | `:797 RED` act=13 page=7 | golden:781 PASS |
| KCN-C4 | story_page.c COVER KEY 长加 `page=LIST` | `:893 RED` 长按页态错留 LIST | golden:864 PASS |
| KCN-C5 | story_page.c list_load 加 `page=LIST` | `:971 RED` restore 帧导航 | golden:929 PASS |
| KCN-C9 | story_page.c ART_CACHE_SLOTS 8→7 | `:1094 RED` 槽未扩容 | golden:1033 PASS |
| KCN-C10 | story_page.c counter 恒返回 count | `:1148 RED` counter 语义漂移 | golden:1124 PASS |
| C6 | display_schema.py 删 `if data=={}` 早退 | `:356 RED` 薄载荷被拒 | `-k c6` 2 passed |
| C7 | front_app.py 删除后重推 restore=True→缺省 | `:450 RED` 未带 restore:true | `-k c7` 5 passed |
| C8 | front_app.py 记录语境分支误纳 story_cover | `:550 RED` routing≠chat | `-k c8` 3 passed |

连带红如实申报：C1 连带 KB-A3（prompt 允许）；C10 连带 C5 tooth1 + SPV2-A7/A8（同 counter-total 语义族，C10 为回归锚同源）；其余 7 条各自只红目标 case。

## 架构复核提出的三条留痕风险（逐条结论）

- **风险 A · 归档失败离页帧落点 = 封皮而非列表**：`front_app.py` `_story_finish_session` 失败补帧走「离页导航帧例外」`restore=False`（human 2026-11-15 裁定），到设备走**非 restore 完整帧** = 进场帧 → 落**封皮**（S-F2.6-AC1 非 restore → COVER），而非 front 原注释语义「回 P3 列表」。**结论**：语义变更如实记录——设备新语义下「离页导航帧」到设备即按非 restore 完整帧进封皮（该帧目的是把设备从困死的整理态带回故事模块，进封皮而非列表是可接受的模块入口行为，且不触发 restore 抢前台之外的副作用）。front 原 docstring「回 P3 列表」措辞已随 `:1771` 一带注释更新（`_push_story_list` 说明该例外是导航帧、翻 true 会因只填缓存不接管前台导致困死复现）。**处置**：转真机轮 K5S-E4 观察项——真机需确认失败补帧后设备确实落到封皮（可见、可继续操作），非困死或黑屏。离线无法证真实时序。
- **风险 B · restore 在 READER/RECORDING/STAMP 驻留时多一次基层重绘（闪帧观察项）**：restore 帧设计为「只填缓存不导航、活动子态零重置」（KCN-C5 钉死 page 不改），但在 READER/RECORDING/STAMP 子态驻留时，缓存更新后渲染层可能触发一次基层重绘。**结论**：页态零重置成立（KCN-C5 机检通过），「多一次基层重绘」属视觉副作用而非状态错误；**转真机轮 K5S-E4 闪帧观察项**——真机目检 restore 常态化后活动子态是否有可见闪帧；若无可见闪帧则闭环，若有则归后续 churn 优化。
- **风险 C · 封皮渲染未含「小伴」**：plan §F 曾列「封皮几何 = rbox + 书脊线 + 标题 + 徽章 + 小伴」，本包实现含书封 rbox + 书脊线 + 原生双圈星徽 + `ic_coin_52_52` + 「星星记录本」标题 + `★ ×N` 计数，**未绘小伴**。设计权威 = `docs/ui/V9/story-cover.png` + `docs/ui/kidpal-ui-design-v9.html`。**结论**：归 **uiuxLevel 4 design review 偏差项**——真机 design review 时以 V9 story-cover 帧逐项比对，若 V9 母版封皮确实要求小伴在场，则记为设计偏差交后续观感修复；本包不自行认定实现缺陷（设计稿 V9 封皮以小伴为装饰非唯一信息载体，不构成 a11y blocker）。**转真机轮 K5S-E4 design review 偏差项**。

## builder 偏离申报

- **`StoryEnterCover` 未字面用 `ScheduleStoryRender()`（architect 复核成立且必要）**：plan §B 写「经 ScheduleStoryRender() 落主任务」，但 `ScheduleStoryRender` 会立即渲染并把 `s_local_page` 置 `KP_PAGE_STORY`；若板级随后再 `cycle_next`，cycle_next 读到 STORY 会算出 next=HOME（P4→P0 跳错页）。builder 改 `StoryEnterCover` 只置 `g_story_view.page=COVER` + 主任务 `cache_story(view, nullptr, nullptr)`（list/text 传 nullptr 不误置送达标志），令随后的 `cycle_next`（同批 main_tasks_ FIFO 先 cache 再 cycle）以封皮态渲染 P4，保住 cycle_next 既有清层/页切换日志与四页循环。行为达成 S-F2.6-AC1「首翻 P4 见封皮」，K0a 冻结面（四页枚举/layer/日志）零改。**architect 复核 APPROVE，偏离成立且必要。**
- **资产纯追加（全量重跑漂移已 checkout 还原）**：全量重跑 `gen_assets.mjs` 到隔离目录 `C:/tmp/k5assets`，因既有资产在当轮 sharp/librsvg 下与入库快照有渲染漂移（第 14492 行附近 ~484 行差异），故**未覆盖**仓库 `assets.c/.h`，改从隔离产物抽取 `ic_coin_52_52` 单块纯追加（git diffstat：`assets.c +459 / assets.h +1`，0 删除）；`gen_art_spiffs.mjs` 全量重跑后仅 `ic_coin_52_52.bin` 新增、`p1_chip_mingzao.bin` 漂移已 `git checkout` 还原（纯追加纪律达标，spiffs_root 1 新增 / 0 修改）。
- **`lcd_display.cc` 子态集补 LIST**：plan §C 要求；不补则列表子态回落 P4 页槽表「打开/下一页」（封皮行）而非列表行，S-F2.1 键帽零回归被破。

## `mcp_server.h` 扩单留痕（architect APPROVE）

- `changeManifest[]` 原只列 `mcp_server.cc`，未列 `mcp_server.h`。`StoryEnterCover()` 为板级入口 reset 所需新增**公有类方法**，声明必须落 `mcp_server.h`（`waveshare-s3-rlcd-4.2.cc` PAGE_NEXT 分支 → `mcp_server.cc` 定义成对）；1g 粗 glob 未展开类声明头。
- 处置：builder 如实申报清单外改动，planner 派 architect 评估 → **architect APPROVE**，`changeManifest append firmware/main/mcp_server.h`（pod-events `6dccc13d`）。除此之外实际实现 diff 全部落在 `changeManifest[]` 内。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | firmware · `mcp_server.cc#StoryEnterCover` | `StoryEnterCover` 依赖「同批 main_tasks_ FIFO 先 cache 再 cycle」的调度顺序成立（非显式同步屏障）；当前板级 PAGE_NEXT 调用点紧接 cycle_next 入队，顺序成立，但属隐式时序耦合。 | changed-code hunk `StoryEnterCover` + 板级 PAGE_NEXT 分支；heuristic = 单生产者主任务队列 FIFO 语义，非跨线程并发 | 无本批动作；真机 K5S-E4 观察首翻 P4 落封皮稳定（无跳页）；如将来板级增并发入队点，改显式同步 |
| NIT | judgement-call | server · `front_app.py#_expand_story_nav_intent` | 展开帧用 `gw_story.list_stories` 现算，异常吞为 `log.warning` 后返回 None（原样交 display_schema 降级）；读列表失败的可见降级由下游机械门承载，非本缝硬失败。 | changed-code hunk `_expand_story_nav_intent`；heuristic = 中继缝不炸连接、失败转机械门降级（既有 `_consume_points_intent` 同款） | 无本批动作 |
| NIT | judgement-call | tools · 资产管线 | 整文件重生成 `assets.c/h` 会顺带漂 `p1_chip_mingzao.bin` 等既有资产（K2/K4 pod-retro 同族 tech-debt）；本包以「纯追加 + 还原」守住 spiffs_root 1 新增 / 0 修改，但生成器缺陷本身未修。 | changed-code hunk `gen_assets.mjs` / `gen_art_spiffs.mjs`；deletion test = 去掉追加段后漂移仍在，说明缺陷在生成器而非本包改动 | 不新建债务条目（K2/K4 已记同族）；见 Residual Risk |

无 BLOCKING finding。UI finding 按 `code-review-gate.md` 既有严重度解读，无 BLOCKING（见 Design-Review Evidence；风险 C 为偏差项观察，非 blocker）。

## Five-Axis Review

1. **Correctness**：S-F2.6 AC1–AC4 逐条落地——封皮四件与页点第 4 实心（C1）、计数两态只信 `story_list.total`（C2/C10）、手势矩阵含逐层返回与不进 300ms 消歧（C3/C4）、restore 三态与活动子态零重置（C5）、薄载荷展开进封皮（C6）、五调用点 restore 判定含离页帧例外（C7）、语境三分（C8）、资产与产线白名单（C9）。回归锚 K0a 阅读键、记录语境入口、删除同步减、四点基线零回归（C10 + SPV2-A1/KB-A3/CG-A2/RSL-A10 重锚全绿）。边界：数据未达不画计数、确认空账显 ×0；未知 source_page 拒绝。
2. **Test quality**：10 个 acceptance id 全部有真实断言（host pixel-probe 真帧 + 结构源锚 tree-scan + server 真跑 pump）；10 处变异逐条使目标断言转红并精确还原（C1 连带 KB-A3 允许、C10 连带同族已申报），说明判别力真实；SPV2-A1 冻结件 tooth 序缺陷暴露后由 acceptance-author 最小修（不改实现）而非放松断言。
3. **Contract/boundary safety**：`docs/contracts/**` 零 diff；front 三时点 restore:true 与 display_schema 薄载荷放行均落已 LANDED 契约（§1.3③/§1.5）；未新增 story_cover view/cover_total（ADR-018）；`kidpal_story_ctx_source_page` 签名未动（`[S1]` 冻结）、`view_init` 语义未动（SPV2-A1 钉死）；未新增依赖、未跨层调用（薄载荷展开 = front 机械单源）。
4. **Operability/security**：restore 分流避免后台推送抢前台（记录会话/删除/归档三时点）；离页导航帧例外避免困死复现；展开异常带 `corr_id` 结构化日志；无 secret、无外部输入直入渲染（资产走白名单映射，未知即不画，缺图 best-effort）。安全闸 `securityReviewRequired=false`。
5. **Maintainability/simplicity**：封皮落 `story_page.c` 子态（无新源文件，状态机单一、host 可编）；`kidpal_story_list_load` 与 `view_init` 职责分离（只填缓存 vs 导航）；`StoryEnterCover` 复用主任务队列不与 cycle_next 抢调度；未发现过早抽象。`simplicity` tag 未命中（无 `minimal:` 标记）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| — | diff 内无 `minimal:` 标记 | — | no-trigger |

## Design-Review Evidence（纯文本；本包 `uiux.uiuxLevel=4`、`designReviewRequired=true`，仓内不存截图/HTML）

- **V9 design refs 覆盖**：封皮直接对应 `docs/ui/V9/story-cover.png`（书封 + 标题「星星记录本」+ 星星数 + 大星徽 + 键帽）；故事列表/阅读沿用 `canvas-p1/p2/p3`、`p3-reader`、`points-home` 既有帧（本包下沉不改未变帧口径）。均已在 `uiux.designRefs`。
- **美术来源与尺寸**：`ic_coin_52_52`（52×52 KAP1，来自 `docs/ui/kidpal-ui-design-v9.html` 内嵌 ic-coin 符号，经既有预栅格化管线）；书封几何原生绘制（rbox + 书脊线 + 双圈星徽）。资产纯追加，渲染 RAM 零结构性增长（ART_CACHE_SLOTS 7→8 一枚纯逻辑常量扩容）。
- **design-drift 摘要**：1bpp 无灰度是 RLCD 硬约束下的既定设计红线；形状/位置/尺寸与 V9 对齐。**已知偏差（非 blocker）**：封皮未绘小伴（plan §F 曾列，见风险 C），归 design review 偏差项待真机比对 V9 母版。**未做像素级逐点比对**，也未真机目视确认（见未覆盖面）。
- **viewport / a11y 摘要**：单照 400×300、1bpp、无滚动；封皮标题、星星数、键帽均以文本承载核心信息，星徽/书封属装饰性强化；四点页点第 4 实心与覆盖层隐藏沿用既有形态。
- **external artifact reference**：`docs/ui/V9/story-cover.png`、`docs/ui/kidpal-ui-design-v9.html`（仓外真机截图/录屏待 K5S-E4，bootstrap 期只留引用不落盘）。
- **residual risk**：真机目检（书封/大星徽辨识度、标题与计数版式、小伴在场与否）未跑。

## PROPOSAL §5 导航项对账 + 跨包端到端余项（K5S-E5）

PROPOSAL §5「workstream 收口标准」列真机与真实 QQ 联合收口项；本包作为**导航并集 owner** 收导航项，余下跨包端到端项交主线程另组织。

| §5 收口项 | 本包状态 | 归属 |
|---|---|---|
| BOOT 四页循环（首页→课表→积分本→故事封皮） | 离线结构锚 C4/KCN-C10 + 板级 BOOT hook；**真机并集未跑** | 导航并集本包 owner；真机轮 K5S-E4 |
| 阅读 → 列表 → 封皮 → 首页逐层返回 | 离线 KCN-C3/C4 手势矩阵 + 页态序列；**真机手感未跑** | 导航并集本包 owner；真机轮 K5S-E4 |
| 故事计数变化不影响积分 | 离线 C2/C10（计数只信 total、与积分分离）；真机未跑 | 本包 + 真机轮 |
| 孩子完整句式加分 / 随口不记账 / 短窗去重 / 兑现负向分录 / 删除重算 / 负余额兑换 | 本包不涉积分账本实现（K2/K3/K4 面） | **跨包联合收口轮（主线程）** |
| QQ 其他页到账不抢前台 / 切目标同步 | K4/K3 面 | **跨包联合收口轮（主线程）** |
| 断网可见失败无本地补记 | 既有 NFR-OFF-1，本包不涉 | **跨包联合收口轮（主线程）** |
| 真实 QQ 端到端 | 本包不涉 | **跨包联合收口轮（主线程）** |

本 Packet 显式声明：**K5 合并不自动宣称 workstream 完成**——整列车真机 + 真实 QQ 联合收口轮由主线程另组织（K5S-E5）。

## 未覆盖面申报（逐条；均为「未跑」，待 human 设备窗 K5S-E4）

1. **BOOT 四页并集**（首页→课表→积分本→故事封皮，含 P3→P4 首翻见封皮的板级 BOOT 钩子实测）—— 未跑：离线仅结构锚 C4，真实板级 cycle_next + StoryEnterCover 时序未真机证。
2. **逐层返回手感**（阅读→列表→封皮→首页，每按一次只上一层）—— 未跑：离线 KCN-C3/C4 手势矩阵通过，真机手感与手感阈值未证。
3. **封皮观感 design review**（V9 story-cover 逐项比对，含风险 C 小伴偏差项）—— 未跑。
4. **语音「打开故事」端到端**（OpenClaw 发空 data 薄载荷 → front 展开非 restore 完整帧 → 设备进封皮）—— 未跑：离线 C6/C8 结构通过，真实语音链路未跑。
5. **restore 不抢前台真实时序**（归档/删除/记录语境建立三时点推送时设备在其他页不抢屏；含风险 B 闪帧观察）—— 未跑。
6. **删除/盖章计数**（真机删除同步减计数、归档盖章 N 计数）—— 未跑：离线 C10 计数语义通过，真机计数变化未证。
7. **设备轮证据** —— `device-round-evidence.md` **未落**。本包已在 launch-card 申报 `deviceOutcome{userVisible:true, acceptanceRefs:[S-F2.6-AC1..AC4], hostUncompilablePaths:[kidpal_render_node.cc / mcp_server.cc / application.cc / lcd_display.cc / 板级 .cc]}`，设备轮子判据会拦至真机轮完成；**不伪造 PASS**。

## Residual Risk

- **p1_auxbar 三条存量红（AB-A1/A2/A4）**：与本包 launch 基线 @03630b65 同族同项，属既有 infra 债（电池/温度/wifi 资产消费点或落位），本包零新增；修不修由既有债务裁决，不在本包范围。
- **真机面全空（K5S-E4）**：见未覆盖面 1–7；`pre-merge-escalation-gate` 的设备轮子判据会拦，这是预期行为，不是本包缺陷。
- **风险 A/B/C 观察项**：见架构复核三条风险；A（离页帧落封皮）/B（restore 多一次重绘闪帧）/C（封皮无小伴）均转真机轮观察，不阻塞合并门机械面，但 human 设备窗需逐项确认。
- **资产管线重生成漂移（不重复立账）**：整文件重跑 `gen_assets.mjs` 会顺带漂 `p1_chip_mingzao.bin` 等，与 K2/K4 pod-retro 同族；本包纯追加守住 1 新增 / 0 修改，生成器缺陷未修。
- **`decisionCoverage` 中 `landsIn === 'dropped'` 且 `reason` 以 `risk-accepted` 开头的条目**：**无**（本包 6 条 decisionCoverage 全为 exitCriteria 承接，无 risk-accepted 放弃项）。
- **叠跑前提**：K5 dependsOn K2/K4 均已 merged 本地 main；mergeOrder=76（列车最后一包）；合并序无前序未完成项。
- **回滚方式**：本包改动全在 `dev/pod-m2` 的非 main 分支 commit 内，`git revert 8de12f5a..a6306e22`（实现/验收/资产）+ 撤销收尾 commit 即可回退；无数据迁移、无分区表变更、无远端状态变更。

## 需 supervisor / human 裁点

1. **真机轮 K5S-E4 尚未跑**：合并门会因 `deviceOutcome` 拦停，需 human 设备窗完成真机导航并集（六项）并补 `device-round-evidence.md` 再复走门；风险 A/B/C 在真机轮逐项确认。
2. **跨包联合收口轮（K5S-E5）**：整列车真机 + 真实 QQ 端到端由主线程另组织，K5 合并不自动宣称 workstream 完成。
3. **ARQ 建议（非本包动作）**：契约 §1.3③ 措辞未来显式排除「归档失败离页导航帧」；`acceptance-coverage-check.mjs` 补 `--change-id` 旗标对齐 mutation-check——均交主线程/后续治理批。
4. **final 承重轮序数 = 3 的说明**：见 Verification Evidence 轮序说明（scoped round 1–2 已占序，机械规约取下一连续序 3）；供 supervisor 核对 `r3.json` 与 invocationId。
