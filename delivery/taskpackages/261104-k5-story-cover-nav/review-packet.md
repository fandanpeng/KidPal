# Review Packet · 261104-k5-story-cover-nav

> 包：故事封皮导航——P4 封皮、列表/阅读下沉、逐层返回与 story_list 共享帧消费（S-F2.6 AC1–AC4 + S-F2.1 AC1–AC7 回归 + S-F2.2 AC1 + S-F2.3 AC1/AC2 + S-F2.4 AC2 现存计数半）
> base ref：`03630b65`（包首提交父，包级 diff 恒用此 ref）｜集成 HEAD：`ca5fa9e7`（含真机轮 K5S-E4 修复三 commit `54dd605e` / `a467bd39` / `ca5fa9e7`）｜worktree `C:/AI/KidPal-pod-m2`，分支 `dev/pod-m2`
> 本 Packet 供 supervisor 机械闸 + 人工复核；不自评通过。

## Change Size

- **Files changed**：28 个实现/工具文件（`firmware/` 19、`server/` 5、`tools/` 4；`git diff --numstat 03630b65..HEAD -- firmware server tools`），另 delivery 簿记件若干不计入实现面。
  - 新资产 1 枚：`firmware/main/assets/spiffs_root/ic_coin_52_52.bin`（KAP1，52×52，stride 7，380 B）
  - 生成件：`tools/simulator/LvglWindowsSimulator/assets/assets.c` +459 / `assets.h` +1（管线自动生成，纯追加非手写）
  - 新冻结验收件 2 件：`firmware/main/test/story_cover_nav.acceptance.c`（+1208）、`server/acceptance/test_story_cover_nav.acceptance.py`（+609）
- **Net diff**：**+3005 / −88**（firmware +1856/−66、server +680/−22、tools +469/−0）；不含 `delivery/`。较集成首收（`8de12f5a`，+2846/−70）增量 = 真机轮修复三 commit（`story_page.c` 封皮几何 + `mcp_server.cc` 导航帧接管 + 回归断言）。
- **设备 TU（`hostUncompilablePaths`，不进 host 构建，靠 tree-scan 源锚代理）**：`application.cc` +10/−0、`mcp_server.cc` +70/−18、`kidpal_render_node.cc` +18/−1、`lcd_display.cc` +7/−2、板级 `waveshare-s3-rlcd-4.2.cc` +8/−1（合计 5 TU +113/−22）。**deviceOutcome 申报路径实际触达**：本包设备 diff 实际含上述五件（`kidpal_render_node.cc` / `mcp_server.cc` / `application.cc` / `lcd_display.cc` / 板级 `.cc`），与 launch-card `deviceOutcome.hostUncompilablePaths` 申报集逐条一致。
- **Scope**：固件 P4 封皮子态（`KSTORY_PAGE_COVER`）+ COVER 渲染/输入表 + `kidpal_story_list_load` 缓存装载；keybar P4 页槽改封皮行 + story_slots/过渡窗补 LIST；board BOOT 钩子 `StoryEnterCover`；front `_expand_story_nav_intent` 薄载荷展开 + 三时点 restore:true（含 :1728 例外）；display_schema story_list 薄载荷放行；资产管线尾部追加 `ic_coin_52_52` job + 白名单。
- **Split recommendation**：**不拆**。封皮子态、列表/阅读下沉、逐层返回与 story_list 共享帧消费必须同批落地（拆开会留下「封皮入口存在但 restore 语义未对齐 → 后台推送抢前台」或「薄载荷被拒 → 语音打开故事失效」的中间态）；`ic-coin` 与书封几何依赖同批 KEY 页槽行。

## Verification Evidence

- **final 轮承重 fact（真机轮修复后重跑，当前承重）**：`delivery/taskpackages/261104-k5-story-cover-nav/verifier-evidence/rounds/r5.json`
  - phase=`final`，round=`5`，invocationId=`d74c065d-c675-44ca-bbae-740c5eb7b786`，observedGateExit=0，`ok:true`（10 闸全绿零 skip）
  - **轮序说明（如实留痕）**：rounds/ 目录承重事实序列 = r1（scoped，首轮红 ok:false）→ r2（scoped，8/8 绿）→ r3（final，真机轮前置终验）→ **r4（scoped，真机轮三缺陷当轮修复后重跑，8/8 绿）→ r5（final，真机轮修复后终验重跑）**。`verifier-evidence-check.mjs` 的 `preflightProducer` 强制 `round === 已完成数 + 1`（连续序），故本次 final 重跑取连续下一序 5。r3 为真机轮前 final，已被 r5 取代为当前承重 final，r3 留作历史。
  - 机械复核：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261104-k5-story-cover-nav --phase final --round 5 --invocation-id d74c065d-c675-44ca-bbae-740c5eb7b786` → exit 0，`factOk:true`，observedGateExit=0。
- **test-scope 清单落盘路径**：`delivery/taskpackages/261104-k5-story-cover-nav/test-scope-final.txt`（25 条测试文件：本包 2 冻结件 + impactedTestsSeed 7 + 共享 seam 12 + 安全余量 4；含非测试件登记与排除项留痕）。**test-scope 复核（真机轮修复后）= 无变化**：重跑 `detect_changes`（since=`03630b65`，direction=both，depth=2）仍为 275 seed 符号 / 297 impacted 节点（模块分布同前）；新增三 commit 仅改 `story_page.c` / `mcp_server.cc` / 既有冻结件 `story_cover_nav.acceptance.c` / `story_page_v2.acceptance.c`（后两者已在清单），无新增受影响测试文件，25 条清单继续覆盖。
- **范围圈定依据**：`detect_changes`（project `C-AI-KidPal-pod-m2`，since=`03630b65`，merge_base=`03630b65`，direction=both，depth=2）→ 275 seed 符号 / 297 impacted 节点（firmware/main 181、server/gateway 75、server/acceptance 28、server/tests 2、builtins 10、firmware/scripts 1）× taskpackage `impactedTestsSeed` 7 条 × crossEnd seam 合成；per-Pod 专属图索引可用 → 正常圈定（未走降级路径）。
- **final r5 逐闸（全跑零 skip）**：taskpackage-required PASS / change-manifest PASS（pre-cutoff 豁免声明在场）/ **firmware-build PASS**（真编译，`xiaozhi.bin` 0x329170，8% 余）/ contract-lint PASS（仅一条既有断链 warning）/ acceptance-coverage PASS（10 acceptance id：KCN-C1..C5、C9、C10、C6、C7、C8）/ mutation-check PASS（10 条 perAcceptance）/ docs-sync PASS / security PASS（`securityReviewRequired=false`）/ **unit PASS**（timing=full，182 passed + 22 subtests，liveDeselected=0）/ **acceptance PASS**（timing=full，server 面 90 passed + 3 subtests，liveDeselected=0；固件面 firmwareAcceptanceFiles=12 / serverAcceptanceFiles=6 结构核对）。**零 notExecuted 跳过**。结构闸结果不复述为 finding。
- **host runner 全量（integrator 独立实跑，K0 锚命令 `wsl -d Ubuntu-24.04 -- ./espidf_wsl.sh build && ./build/host_infra_test.elf`，@HEAD `ca5fa9e7` 重建+复跑）**：
  - 结果 = **301 Tests / 3 Failures / 0 Ignored**（与真机轮前一致；真机轮修复零新增用例、零新增红）。
  - **3 红全属 `p1_auxbar_style.acceptance.c` 存量族**：AB-A1（`:522` 电池图标源）、AB-A2（`:608` 温度图标源）、AB-A4（`:754` wifi 资产重出守约）——与本包 launch 基线 **292 Tests / 3 Failures（同三条 @03630b65）** 同族同项。
  - 301 − 292 = 9 = 本包新增测试件；**7 条 KCN host case 全 PASS**（KCN-C1/C2/C3/C4/C5/C9/C10；C5 含真机轮新增 tooth 5「导航帧不得回喂 voice_front」）；另重锚 **SPV2-A1 / KB-A3 / CG-A2 / RSL-A10 全转 PASS**、builder `chrome_ghost.test.c` TEST 3b PASS。⇒ **K5 零新增红**，3 红为既有 infra 债（不归本包修，见 Residual Risk）。
- **server 面（@HEAD `ca5fa9e7` 复跑）**：acceptance 闸 scope 内 6 件 = 90 passed / 3 subtests，0 live deselected；`server/acceptance/` 全量 = **457 passed / 0 failed / 9 deselected / 33 subtests**（沿用 r3 口径，本轮 server 面零改动）；`server/` 全量（tests + acceptance 等）= 819 passed / 0 failed / 9 deselected / 60 subtests。零红。
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

K5S-E1（Pod 内冻结验收全量 + 红基线/变异齐全）离线面达成；K5S-E2（story_list 共享帧只信 total + 三推送 restore:true + 薄载荷只进封皮）离线达成；K5S-E3（ic-coin/书封经产线 + V9 design review）离线达成、真机观感由 K5S-E4 看屏逐项确认；**K5S-E4 真机轮达成**（human-visual PASS S-F2.6-AC1..AC4，见下「真机轮（K5S-E4）」与 `device-round-evidence.md`）；K5S-E5（导航并集 owner + review-packet 对账 PROPOSAL §5）本 Packet §对账 达成、跨包联合收口归主线程；K5S-E6（TECH-261028 只回归不重修，diff ⊆ manifest）达成。

## 真机轮（K5S-E4，2026-11-15 晚，四轮看屏 + 三源互证）

> 证据 SSOT = `delivery/taskpackages/261104-k5-story-cover-nav/device-round-evidence.md`（机器消费行 `device-round-evidence（JSON）` 供前置升级闸 `deviceOutcome` 判据）。设备固件 = source `ca5fa9e7`（含真机轮全部修复），app `0x20000` + art `0x780000` 双分区烧录、双 hash 校验通过、NVS 保留；三源 = 设备 SD 日志（KPLOG）/ ECS front journalctl / human 看屏。最终固件 binary sha256 = `7f495a2862f7d7e02606024f36e0645bcbc0e7442feeb774763f0c8331cccbc6`。

- **最终轮 PASS 台账（human 看屏 + KPLOG 互证）**：封皮四件（书封 + 「星星记录本」+ ★×19 + 键帽「打开 / 下一页」）；四点页点第 4 实心 + 覆盖层页点隐藏；BOOT 四页并集 P1→P2→P3→P4（落 P4 即封皮）；封皮 KEY 短按进列表 / BOOT 回 P1 / KEY 长按回首页；逐层返回（阅读→列表→封皮→首页，每按一次只上一层）；语音「打开故事」只到封皮不跳列表；后台 restore 帧不抢前台；封皮计数 = 现存篇数、删除同步减；封皮观感 vs V9 母版 human 第四轮确认「都 OK 了」。
- **本轮抓到 3 个缺陷并当轮修复重烧**（下表只记最终固件上的通过项）：
  1. **front 未部署**：首轮语音「打开故事」不切页——ECS `/opt/kidpal/front/front/front_app.py` 为旧版（无 `_expand_story_nav_intent`），薄载荷被旧门拒。处置：部署 K5 front + display_schema 并重启 `kidpal-front`（先 `.bak-20261115-k5` 备份）。此缺陷在部署面，不产生代码 commit。
  2. **导航帧被 VOICE 前台吞**（部署后仍不切页）：K3「VOICE 前台即 defer」判据把显式导航帧连坐吞掉。修复 `mcp_server.cc`：非 restore 完整帧 = 显式导航帧，按到达时页态判 `story_nav_takeover`，不再看 voice_front；录音态页保护保留（commit `54dd605e`）+ 回归断言折进 KCN-C5 tooth 5（commit `a467bd39`）。
  3. **封皮视觉与 V9 母版不一致**（三轮看屏逐步收口：缺小伴/星徽比例错/缺涂鸦 → 书封几何偏大偏左/书脊位置长度错/★过小/云压书封 → ★与数字叠角/标题偏低偏左）：按 `docs/ui/V9/story-cover.png` 逐像素测量重落位（commit `54dd605e` + `ca5fa9e7`）。
- **deviceOutcome 申报路径实际触达**：本包设备 diff 实际含 `kidpal_render_node.cc` / `mcp_server.cc` / `application.cc` / `lcd_display.cc` / 板级 `.cc` 五件，与 launch-card `deviceOutcome.hostUncompilablePaths` 申报逐条一致（见 Change Size 节）。四个申报 acceptanceRef（S-F2.6-AC1..AC4）各有 human-visual PASS 目击条目。
- **残余与观察项**：风险 A（离页帧落封皮——本轮机读只钉 restore 位，未构造整理失败场景）、风险 B（restore 驻留重绘闪帧——本轮未构造该时序、未量化）；风险 C（封皮缺小伴）已闭环（已补 pose_daily_96 等比缩放落位，human 看屏确认）。跨包联合收口（K5S-E5：整列车真机 + 真实 QQ 端到端）不属本包，归主线程另组织。

## 红基线 → 绿证据链

1. **红基线 @03630b65**（acceptance-author 冻结实跑 + planner 独立复跑，两次数值一致）：host = 293 Tests / 7 Failures（4 设计态红 SPV2-A1 / KB-A3 / CG-A2 / RSL-A10 + 3 存量 p1_auxbar AB-A1/A2/A4）；server = 9 failed / 448 passed / 9 deselected / 33 subtests（9 红全本批设计态红，零存量污染）。
2. **builder 实现后**：server 457 passed / 0 failed / 9 deselected（9 设计态红全绿、零新增红）；host 301 Tests / 4 Failures（3 存量 + SPV2-A1 冻结件自身 tooth 序缺陷）。
3. **SPV2-A1 冻结件缺陷修复**（acceptance-author 最小修 e2cf90e0，不改实现）：tooth 3 把 `v.page` 置 COVER 后 tooth 4 未复位；修 = tooth 4 前 `kidpal_story_view_init` 复位到 LIST。修后 host 301 / 3（仅存量 p1_auxbar）。
4. **coverage 标记形态修**（406c1953）：acceptance-coverage 首轮红因 coverage 标记形态不匹配（非断言缺陷）→ 补精确标记。
5. **变异证据 10/10**（8de12f5a）：每条 acceptance 变异→转红→还原复跑绿。
6. **integrator final 承重轮 r3**（真机轮前）：10 闸全绿零 skip；host 全量独立复跑 301/3（3 红全存量）；server scope 内 90 passed。零新增红。
7. **真机轮 K5S-E4 三缺陷当轮修复后**：`r4`（scoped）8/8 绿；`r5`（final）10 闸全绿零 skip；host 全量 @HEAD 复跑 301/3（同三条存量）；server acceptance 全量 457 passed / 0 failed。零新增红。修复 commit = `54dd605e`（封皮视觉 + 导航帧接管）/ `a467bd39`（KCN-C5 tooth 5 回归断言）/ `ca5fa9e7`（封皮几何逐像素对齐）。

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

- **风险 A · 归档失败离页帧落点 = 封皮而非列表**：`front_app.py` `_story_finish_session` 失败补帧走「离页导航帧例外」`restore=False`（human 2026-11-15 裁定），到设备走**非 restore 完整帧** = 进场帧 → 落**封皮**（S-F2.6-AC1 非 restore → COVER），而非 front 原注释语义「回 P3 列表」。**结论**：语义变更如实记录——设备新语义下「离页导航帧」到设备即按非 restore 完整帧进封皮（该帧目的是把设备从困死的整理态带回故事模块，进封皮而非列表是可接受的模块入口行为，且不触发 restore 抢前台之外的副作用）。front 原 docstring「回 P3 列表」措辞已随 `:1771` 一带注释更新（`_push_story_list` 说明该例外是导航帧、翻 true 会因只填缓存不接管前台导致困死复现）。**处置**：转真机轮 K5S-E4 观察项。离线无法证真实时序。**真机轮现状（如实保留，未闭环）**：本轮未构造整理失败场景，机读断言只钉 `restore` 位；「离页帧落封皮」的行为观感归后续观察（设备新语义进封皮是可接受模块入口，非困死/黑屏）。
- **风险 B · restore 在 READER/RECORDING/STAMP 驻留时多一次基层重绘（闪帧观察项）**：restore 帧设计为「只填缓存不导航、活动子态零重置」（KCN-C5 钉死 page 不改），但在 READER/RECORDING/STAMP 子态驻留时，缓存更新后渲染层可能触发一次基层重绘。**结论**：页态零重置成立（KCN-C5 机检通过），「多一次基层重绘」属视觉副作用而非状态错误。**真机轮现状（如实保留，未量化）**：本轮未构造 READER/RECORDING/STAMP 驻留 + restore 的时序，闪帧与否未量化；若出现可见闪帧则归后续 churn 优化。
- **风险 C · 封皮渲染未含「小伴」**：plan §F 曾列「封皮几何 = rbox + 书脊线 + 标题 + 徽章 + 小伴」，本包实现含书封 rbox + 书脊线 + 原生双圈星徽 + `ic_coin_52_52` + 「星星记录本」标题 + `★ ×N` 计数，**未绘小伴**。设计权威 = `docs/ui/V9/story-cover.png` + `docs/ui/kidpal-ui-design-v9.html`。**结论**：归 **uiuxLevel 4 design review 偏差项**——真机 design review 时以 V9 story-cover 帧逐项比对。**真机轮现状（已闭环）**：真机轮首轮发现封皮缺小伴，当轮补 `pose_daily_96` 等比缩放落位，human 第四轮看屏确认「都 OK 了」，与 V9 母版逐项对照一致，偏差闭环（见 `device-round-evidence.md` 风险 C）。

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
| SHOULD | judgement-call | firmware · `mcp_server.cc#StoryEnterCover` | `StoryEnterCover` 依赖「同批 main_tasks_ FIFO 先 cache 再 cycle」的调度顺序成立（非显式同步屏障）；当前板级 PAGE_NEXT 调用点紧接 cycle_next 入队，顺序成立，但属隐式时序耦合。 | changed-code hunk `StoryEnterCover` + 板级 PAGE_NEXT 分支；heuristic = 单生产者主任务队列 FIFO 语义，非跨线程并发 | 无本批动作；真机 K5S-E4 已确认首翻 P4 落封皮稳定（BOOT 四页并集 P1→P4 落封皮、无跳页）；如将来板级增并发入队点，改显式同步 |
| NIT | judgement-call | server · `front_app.py#_expand_story_nav_intent` | 展开帧用 `gw_story.list_stories` 现算，异常吞为 `log.warning` 后返回 None（原样交 display_schema 降级）；读列表失败的可见降级由下游机械门承载，非本缝硬失败。 | changed-code hunk `_expand_story_nav_intent`；heuristic = 中继缝不炸连接、失败转机械门降级（既有 `_consume_points_intent` 同款） | 无本批动作 |
| NIT | judgement-call | tools · 资产管线 | 整文件重生成 `assets.c/h` 会顺带漂 `p1_chip_mingzao.bin` 等既有资产（K2/K4 pod-retro 同族 tech-debt）；本包以「纯追加 + 还原」守住 spiffs_root 1 新增 / 0 修改，但生成器缺陷本身未修。 | changed-code hunk `gen_assets.mjs` / `gen_art_spiffs.mjs`；deletion test = 去掉追加段后漂移仍在，说明缺陷在生成器而非本包改动 | 不新建债务条目（K2/K4 已记同族）；见 Residual Risk |

无 BLOCKING finding。UI finding 按 `code-review-gate.md` 既有严重度解读，无 BLOCKING（见 Design-Review Evidence；风险 C 真机轮已闭环，见架构复核风险节）。

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
- **design-drift 摘要**：1bpp 无灰度是 RLCD 硬约束下的既定设计红线；形状/位置/尺寸与 V9 对齐。**已知偏差**：封皮未绘小伴（plan §F 曾列，见风险 C）——真机轮已补齐并 human 看屏确认闭环。封皮几何经真机轮三轮看屏、按 `docs/ui/V9/story-cover.png` 逐像素测量重落位（commit `ca5fa9e7`），human 第四轮确认「都 OK 了」。真机目视已确认。
- **viewport / a11y 摘要**：单照 400×300、1bpp、无滚动；封皮标题、星星数、键帽均以文本承载核心信息，星徽/书封属装饰性强化；四点页点第 4 实心与覆盖层隐藏沿用既有形态。
- **external artifact reference**：`docs/ui/V9/story-cover.png`、`docs/ui/kidpal-ui-design-v9.html`（真机看屏已确认于 K5S-E4；仓外真机截图/录屏不落盘，bootstrap 期只留引用）。
- **residual risk**：真机目检已完成（书封/大星徽辨识度、标题与计数版式、小伴在场均 human 确认）；残余 = 风险 A/B 观察项（未构造时序、未量化，见架构复核风险节）。

## PROPOSAL §5 导航项对账 + 跨包端到端余项（K5S-E5）

PROPOSAL §5「workstream 收口标准」列真机与真实 QQ 联合收口项；本包作为**导航并集 owner** 收导航项，余下跨包端到端项交主线程另组织。

| §5 收口项 | 本包状态 | 归属 |
|---|---|---|
| BOOT 四页循环（首页→课表→积分本→故事封皮） | 离线结构锚 C4/KCN-C10 + 板级 BOOT hook；**真机轮 K5S-E4 PASS**（KPLOG P1→P2→P3→P4，落 P4 即封皮） | 导航并集本包 owner；真机轮 K5S-E4 ✅ |
| 阅读 → 列表 → 封皮 → 首页逐层返回 | 离线 KCN-C3/C4 手势矩阵 + 页态序列；**真机轮 K5S-E4 PASS**（human 看屏 + KPLOG `STORY_RENDER page=1→0→7` 序） | 导航并集本包 owner；真机轮 K5S-E4 ✅ |
| 故事计数变化不影响积分 | 离线 C2/C10（计数只信 total、与积分分离）；真机轮确认封皮计数 = 现存篇数、删除同步减；与积分分离仍离线机检 | 本包 + 真机轮（计数面 ✅）+ 跨包联合轮 |
| 孩子完整句式加分 / 随口不记账 / 短窗去重 / 兑现负向分录 / 删除重算 / 负余额兑换 | 本包不涉积分账本实现（K2/K3/K4 面） | **跨包联合收口轮（主线程）** |
| QQ 其他页到账不抢前台 / 切目标同步 | K4/K3 面 | **跨包联合收口轮（主线程）** |
| 断网可见失败无本地补记 | 既有 NFR-OFF-1，本包不涉 | **跨包联合收口轮（主线程）** |
| 真实 QQ 端到端 | 本包不涉 | **跨包联合收口轮（主线程）** |

本 Packet 显式声明：**K5 合并不自动宣称 workstream 完成**——整列车真机 + 真实 QQ 联合收口轮由主线程另组织（K5S-E5）。

## 真机轮覆盖结果（K5S-E4；原「未覆盖面」逐条闭环）

> 原 7 条未覆盖项已由 2026-11-15 晚真机轮（四轮看屏 + KPLOG + ECS front 三源互证）覆盖；证据 SSOT = `device-round-evidence.md`。以下逐条记结果：

1. **BOOT 四页并集**（首页→课表→积分本→故事封皮，含 P3→P4 首翻见封皮的板级 BOOT 钩子实测）—— **PASS**：KPLOG `23:04:43 / :44 / :48` 三段 `PAGE_SWITCH`（home→schedule→points_home→story），落 P4 即封皮；human 看屏确认。
2. **逐层返回手感**（阅读→列表→封皮→首页，每按一次只上一层）—— **PASS**：human 看屏（列表长按回封皮、阅读长按回列表）；KPLOG `STORY_RENDER page=1→0→7` 序互证。
3. **封皮观感 design review**（V9 story-cover 逐项比对，含风险 C 小伴偏差项）—— **PASS**：human 四轮看屏，第四轮「都 OK 了」；小伴已补（风险 C 闭环）。
4. **语音「打开故事」端到端**（OpenClaw 发空 data 薄载荷 → front 展开非 restore 完整帧 → 设备进封皮）—— **PASS**：KPLOG `23:05:37 story_list_dispatch items=16 degrade=false`（非 restore 导航帧）→ 同秒 `STORY_RENDER page=7` → `23:05:44 PAGE_SWITCH trigger=net`；human 看屏确认切到封皮。本轮同时抓到并修复 front 未部署 + 导航帧被 VOICE 吞两缺陷。
5. **restore 不抢前台真实时序**（归档/删除/记录语境建立三时点推送时设备在其他页不抢屏；含风险 B 闪帧观察）—— **PASS（不抢前台）**：KPLOG `23:05:50 story_list_gate defer_front page=7 voice_front=0 restore=1`（restore 帧被 defer，页面不跳）。**风险 B 闪帧**未构造该时序、未量化（如实保留）。
6. **删除/盖章计数**（真机删除同步减计数、归档盖章 N 计数）—— **PASS**：human 看屏（★×19 与现存篇数一致，删除同步减）；计数源 = `story_list.total`（KCN-C10 变异钉住）。
7. **设备轮证据** —— `device-round-evidence.md` **已落**（首行后恰一行 `device-round-evidence（JSON）` 机器消费行）：`sourceCommit` = `ca5fa9e7…`（= 合并 HEAD）、`binarySha256` = `7f495a28…`，四个申报 acceptanceRef（S-F2.6-AC1..AC4）各有 human-visual PASS 目击条目。

## Residual Risk

- **p1_auxbar 三条存量红（AB-A1/A2/A4）**：与本包 launch 基线 @03630b65 同族同项，属既有 infra 债（电池/温度/wifi 资产消费点或落位），本包零新增；修不修由既有债务裁决，不在本包范围。
- **真机面已覆盖（K5S-E4）**：原「真机面全空」已闭环——`device-round-evidence.md` 已落机器消费行，四个申报 acceptanceRef 各有人类看屏 PASS 目击；前置升级闸 `deviceOutcome` 判据可由该机器消费行满足（sourceCommit = 合并 HEAD `ca5fa9e7`，binarySha256 见证据文件）。
- **风险 A/B/C 观察项**：见架构复核三条风险；A（离页帧落封皮，机读只钉 restore 位，行为观感归后续观察）、B（restore 驻留重绘闪帧，未构造时序、未量化）为残余观察项，不阻塞合并门机械面；C（封皮无小伴）真机轮已闭环。
- **资产管线重生成漂移（不重复立账）**：整文件重跑 `gen_assets.mjs` 会顺带漂 `p1_chip_mingzao.bin` 等，与 K2/K4 pod-retro 同族；本包纯追加守住 1 新增 / 0 修改，生成器缺陷未修。
- **`decisionCoverage` 中 `landsIn === 'dropped'` 且 `reason` 以 `risk-accepted` 开头的条目**：**无**（本包 6 条 decisionCoverage 全为 exitCriteria 承接，无 risk-accepted 放弃项）。
- **叠跑前提**：K5 dependsOn K2/K4 均已 merged 本地 main；mergeOrder=76（列车最后一包）；合并序无前序未完成项。
- **回滚方式**：本包改动全在 `dev/pod-m2` 的非 main 分支 commit 内，`git revert 03630b65..ca5fa9e7`（实现/验收/资产/真机轮修复三 commit）+ 撤销收尾 commit 即可回退；无数据迁移、无分区表变更、无远端状态变更。

## 需 supervisor / human 裁点

1. **真机轮 K5S-E4 已完成**：human 看屏 + KPLOG + ECS front 三源互证，`device-round-evidence.md` 已落，S-F2.6-AC1..AC4 human-visual PASS（human 2026-11-15 原话「都OK了，commit merge吧」）。**残余观察项 = 风险 A/B**（离页帧落封皮行为观感、restore 驻留闪帧，均未构造时序/未量化），请 supervisor 知悉两条为观察项、非阻塞机械面。
2. **跨包联合收口轮（K5S-E5）**：整列车真机 + 真实 QQ 端到端由主线程另组织，K5 合并不自动宣称 workstream 完成。
3. **ARQ 建议（非本包动作）**：契约 §1.3③ 措辞未来显式排除「归档失败离页导航帧」；`acceptance-coverage-check.mjs` 补 `--change-id` 旗标对齐 mutation-check——均交主线程/后续治理批。
4. **final 承重轮序数 = 5 的说明**：见 Verification Evidence 轮序说明（rounds 1–4 已占序，机械规约取下一连续序 5）；供 supervisor 核对 `r5.json` 与 invocationId `d74c065d-c675-44ca-bbae-740c5eb7b786`。
