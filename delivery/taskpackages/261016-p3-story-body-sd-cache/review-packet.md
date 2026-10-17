# Review Packet · 261016-p3-story-body-sd-cache

> integrator · 2026-10-17（round-6 复收口）· rigor=full / riskTier=high / sourceType=proposal · 分支 `dev/pod-m2` · **上一轮 1 条 BLOCKING（WF-F2-06 覆盖层可见 chrome 面）已由 fixer 修复（commit `8f80394a`）并经 final r6 全绿复验 → 本包已无阻断 finding，可交 planner 跑 supervisor 机械闸；integrator 不 push、不 merge、不自行判定通过**。

## Change Size

- **Files changed（实现面 21 件）**：`firmware/main/display/story_cache.c`（新 +310）、`story_cache.h`（新 +143）、`boards/waveshare-s3-rlcd-4.2/story_sd_io.cc`（新 +167）、`boards/common/board.h`（+5）、`boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+7/-3）、`firmware/main/CMakeLists.txt`（+6）、`protocols/protocol.cc`（+45）、`protocols/protocol.h`（+22/-1）、`mcp_server.cc`（+435/-1）、`display/story_page.c`（+73）、`display/story_page.h`（+39/-1）、`display/keybar.c`（+13/-1，WF-F2-06 新槽 case）、`display/lcd_display.cc`（+25/-2，dots_overlay 判据 + 5 号态接线）、`firmware/sdkconfig`（+12/-12 面）、`test/host_app/main/CMakeLists.txt`（+11）、`test/story_cache.acceptance.c`（冻结件 +1695，13 TEST_CASE）、`test/acceptance/red-baseline/sc_red_wrapper.sh`（新 +23）、`test/acceptance/red-baseline/unity.h`（+10 additive）、`server/gateway/front/front_app.py`（+25）、`server/gateway/story.py`（+83/-17）、`server/acceptance/test_story_fetch.py`（冻结件 +623）。
- **Net diff**：实现面 **+3723/-49（21 文件）**；全分支（含 delivery / docs 簿记与 closeout 制品）**47 文件 +5891/-212**（`git diff main...HEAD --shortstat` 实测于本轮收口 commit 前一刻；含本 packet 自身，故为近似口径，实现面数字为稳定口径）。基准 = `main...HEAD`（merge_base `b6cc8945`）。
- **Scope**：P3 story 正文 SD 只读镜像（`/sdcard/story/<id>.txt`）+ 列表当屏预取 + 双击三分支兜底 + id 错配修复 + WF-F2-06 降级覆盖层（正文 + 底栏 chrome 单源）+ 云端 `kidpal_story_fetch` 收帧回帧。**契约零 diff、零新增依赖、零新增出网面、零跨层调用**。
- **Split recommendation**：**不拆**（human 触点①已定 ②单包 Full；本包单 changeId 单收口轮）。

## Verification Evidence

- **Structural gates（final r6）**：`firmware-build` **真编译 build-pass**（`xiaozhi.bin 0x3217f0` = 3282928 字节，最小 app 分区余 9%；较 r3 少 256 字节，与删死绘制代码方向一致）；`contract-lint` PASS（唯一 warning = 上游断链 `../../delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md`，非本包引入）。
- **Final 相位全闸（round 6 / invocationId `475c0184-1a11-47d5-b6f5-4e6cf21fd0f7`）**：`{"ok":true,"phase":"final","round":6,"evidenceRef":"…/verifier-evidence/rounds/r6.json","observedGateExit":0}`；**10 闸全跑**（8 scoped + unit + acceptance）；`verifier-evidence-check --change-id … --phase final --round 6 --invocation-id …` → `{"ok":true,"gate":"verifier-evidence","factOk":true,"observedGateExit":0,"phase":"final","round":6}`（exit 0）。
- **Scoped 复验（round 5 / `eb7f15d0-f703-45c1-9e6e-1c74afa8a267`）**：fix 后 8 闸全绿 `ok:true`；round 4 唯一红点 = `docs-sync`（docRefs 仍指 debt-render 归位前的 TECH-261013 路径），已由 planner commit `da8055df` 改指归档路径消除。
- **Acceptance / unit（按 test-scope 圈定真跑）**：`unit` 闸 `executor-run`（scope 内 4 件 pytest）**86 passed + 3 subtests**；`acceptance` 闸 `oracle-pass`（固件冻结验收 15 件 / server 3 件）→ `executor-run` **51 passed + 3 subtests**。
- **全量交叉核对（integrator 独立复跑，非闸内）**：firmware host ELF **195 Tests / 3 Failures / 0 Ignored**（新增 13 齿全 PASS；3 红逐条实核 = 上游已知红 `p1_auxbar_style` AB-A4 / AB-A2 / AB-A1，新增红 **0**）；`server/acceptance/`（`--import-mode=importlib -m "not live"`）**181 passed / 4 deselected / 33 subtests**（与基线逐字一致）；`server/tests/` **91 passed**。
- **Mutation check**：`mutation-check` PASS；`perAcceptance` SC-A1..SC-A8 全条 `turnedRed=true` + 还原复绿。SC-A5 的变异面本轮扩为 4 条受影响齿（含新增 chrome 三条）；**integrator 独立抽验 1 条**：把 `lcd_display.cc` 的 `kidpal_keybar_dots(page, dots_overlay, &d)` 回退为 `overlay_top` → `story_cache.acceptance.c:1631` 立即转红（错误文案逐字为「页点可见性判据未纳入 KSTORY_PAGE_BODY_UNAVAILABLE…」），`git checkout` 还原后复绿、工作树干净。SC-A9 = structural / live-manual，`not-applicable-structural`。
- **Security gate**：`securityReviewRequired=true`、两轴复核（见「五轴 · Operability/security」）；`security` 闸 PASS。
- **Docs sync**：r6 `docs-sync` 闸 PASS；TECH-261013 已结项并归档（见 `docs-delta.md` §1）；`docs/notes` 盘点行已由主线程代落（§2）；修复轮无新增 docs-sync 需求（§5）；`docs/ui/UIUX-DESIGN-BRIEF.md` 在途警示行已于 intake commit `c0f0297e` 撤除、核对一致。
- **机器件口径**：承重 = `verifier-evidence/rounds/r5.json`（scoped，fix 后 8 闸绿）+ `r6.json`（final，10 闸绿；`authority: bearing`、`producedBy: verify.mjs`，immutable fact）；`current/verdict.json` = verify.mjs 脚本 fs-write 的 non-bearing projection，只随 commit 收口、不据以判 closeout；`verdict.json` / `verification.json` 按 closeout 制品 SSOT 为 advisory-only、**永不晋升包目录**，故本包不产这两件。

## test-scope 清单（final 相位范围圈定）

- **落盘路径**：`delivery/taskpackages/261016-p3-story-body-sd-cache/test-scope-final.txt`（`verify.mjs --phase=final --scope-file` 消费，**round 6**）。**本行即「清单路径记入 results 元数据必含行」的落点**——r6 fact 的 `results[]` 由 verify.mjs 上游 schema 固定（`id/gate/kind/ok/exit/exitCode/ms/timing/stdout/stderr/notExecuted/summary`），无 scopeFile 字段；scope 路径在该 schema 下只能落在本 review-packet 与各闸 stdout。
- **圈定方式 = 正常路径（非降级）**：`detect_changes`（per-Pod 专属图索引 `C-AI-KidPal-pod-m2`，base=main，merge_base `b6cc8945`，inbound depth=2）round-3 为 **155 seed / 17 impacted**；round-6 复跑（fix 新增 keybar.c / lcd_display.cc 面）为 **167 seed / 26 impacted**，impacted_modules 与 round-3 同集合（`firmware/main` + `server/{gateway,tests,acceptance}`），**零预期外模块**；hop 面含 `lcd_display` 内部消费符号（`SetupUI / KidPalKeybarSync / KidPalVoiceChromeFront / kidpal_voice_apply_look` 等）。round-3 的 `trace_path` 抽查（`CustomBoard::SdCardMounted` callers_total=7、`kidpal_story_page_input` 4 命中）结论不外推，图新鲜度本轮由 `detect_changes` 读到新改文件内符号佐证。
- **合成**：`acceptanceTestsFile`（2 件）∪ blast radius 映射测试 ∪ 共享 seam 相关测试。**round-6 增量**：树扫类测试不在图边（源级文本消费，非符号调用），按 grep 双确认补入两件——`keybar_three_slot.acceptance.c`（keybar.c 字面闭集红线 + 三槽结构齿）、`voice_rightcol_label.acceptance.c`（lcd_display.cc 全文行级负向齿）；其余 keybar.c / lcd_display.cc 消费者（char_window_overlay / retract_p4_three_page / schedule_grid / record_session_live / reminder_overlay_unify / voice_round_ui / story_page_v2）已在 round-3 清单内。**floor** = 本包 acceptanceTestsFile ∪ blast radius ∪ 共享 seam（本包无对端包）。**圈定降级 = 无**。
- **例外申报**：`firmware/main/test/acceptance/archive/happy_flow_capture.test.c` 虽被 detect_changes 命中 hop 1，但属已归档退役面（`SKIP_DIRS` 含 `archive`、不入 host 构建），不入清单；`char_stroke.acceptance.c` 树扫对象 = `custom_lcd_display.cc` / `char_page.cc`（grep 双确认对 `display/lcd_display.cc` 零命中），同样不入。

## demo 对账行（demo 证据 ⟷ 本包效果段）

- **demo 证据形态** = host runner 全量（195T/3F，本包 13 齿全绿含 chrome 三条）+ final r6 10 闸绿 + 设备真编（build-pass）+ 设计稿 `docs/ui/V6/p3-body-unavailable.png`；**真机 live-manual 日志未产生**（SC-A9 未跑，如实挂账）。
- **本包效果段**（plan §建什么）= 双击任意一条记录都能进阅读（本机录 / 重启前 / 云端恢复一视同仁）；断网已留存照常打开；取不回给可见提示；**双击打开的一定是光标选中那一篇**。
- **一致性结论 = 自动化半一致、真机半未跑**：行为齿（预取决策 / 三分支 / id 校验 / 写卡门控 / 覆盖层话术与 10s 关闭）全绿且变异可杀；**上一轮查出的覆盖层可见 chrome 面偏离已修复并转机判**（底栏 `知道了 / 关闭` 单源、页点隐藏、死绘制删除，SC-A5 三条新结构齿钉住）；真机半仍挂 SC-A9；标题「大字」未达标、显式 deferred（见 Findings / 未覆盖面 ⑩）。

## UI design-review 文本证据（bootstrap：纯文本、零 screenshot 入仓）

- **viewport coverage**：RLCD 单一物理屏 400×300（归一屏模型，无响应式断点）；WF-F2-06 覆盖层按 `docs/ui/V6/p3-body-unavailable.png` 布局坐标落地——标题 y=64（**本轮起水平居中**）、副句 y=96、选中条目框 y=130（x=40 / w=320 / h=40，星星 x=60 · 标题 x=90 · 日期 x=300）；底栏键帽 `[知道了]`/`[关闭]` 由 keybar.c 三槽 chrome 落屏（y 带 = 底条内），页点在该态隐藏。
- **a11y summary**：话术单源（`KSTORY_BODY_UNAVAILABLE_HINT`/`_SUB` = 设计稿逐字；键帽文案单源 = `KSTORY_BODY_UNAVAILABLE_KEYCAP_KEY/_BOOT` 宏，keybar.c 零字面重抄、守 KB-E2 闭集红线）；14px 普惠体、`KSY_COLOR_INK #333333` 于 RLCD 反射白底（对比度充足）；纯告知态无待确认内容、无倒计时数字（对齐删除确认静默口径）；KEY／BOOT 任一手势均可关闭（无单键依赖）；小伴不出场。无屏幕阅读器面（本产品无此能力）。
- **design-drift summary（brief v5 ↔ 实现）**：① 覆盖层正文布局 = 无漂移；② **底栏键帽条 = 漂移已消除**（brief:101「覆盖层……不显示页点」+ 设计稿底栏 `[知道了]`/`[关闭]`：5 号态键帽槽经 `kidpal_keybar_story_slots` 单源出值，页点经 `dots_overlay` 判据隐藏；SC-A5 三条结构齿机判，变异可杀）；③ 标题字号 = **漂移残留、显式 deferred**（brief「大字」/设计稿 26px vs 实现 14px；居中已修；大字号需求 host sdkconfig 开 `LV_FONT_FMT_TXT_LARGE` 不在本包 changeManifest，且设备内置 30px 字体缺字——记为后续视觉批候选，**不得记为已达标**）；④ 覆盖层自画键帽死绘制 = **已删除**（键帽单源 = keybar.c，SC-A5 TEST 13 钉唯一画法源）。
- **external artifact reference**：无（`repoVisualArtifactRequired=false`；设计稿 PNG 在仓内，截图/视频留仓外）。
- **residual risk**：真机目检未做（SC-A9）；新齿为源锚代理（符号链 ≠ 运行时像素），覆盖层真帧表现（字重、底条遮盖关系、页点消失视觉）须在 SC-A9 目检。

## 未覆盖面申报（逐条一行，零缺口明写 none）

- ① **隐私加密防护未做** —— 现状 = 明文落卡（human 2026-10-16 OQ-3 裁定「本批只落盘、不加防护」）；去向 = `TECH-261016-sd-story-confidentiality`（`status: deferred`）跟踪，**不表述为「已防护」**。
- ② **缓存淘汰（LRU）** —— 非目标；现状 = 索引容量 16（与 `story_list` 单页窗口同值）+ 卡满即写失败、只清该篇不整层清空；旧正文文件不自动清理（容量风险留后续批）。
- ③ **光标预取** —— 非目标；现状 = 仅「进列表 / 翻屏」当屏批量 + 双击插队单篇，光标移动不触发预取。
- ④ **热插拔检测** —— 非目标；现状 = RAM 索引首次使用时惰性重建一次（`g_story_cache_scanned` 单次）；运行期拔卡 → 读失败只移出该篇；运行期插卡本开机不复扫。
- ⑤ **SC-A9 真机 live-manual 轮未跑** —— human 持板硬前置（离线双击已留存 / 三类记录进阅读 / 降级覆盖层 10s 与双键关闭 / **覆盖层真帧目检（底栏键帽 + 页点消失 + 标题观感）** / id 错配回归位 / 预取期语音不卡顿 / SD 读回 roundtrip / heap 水位）；**本报告不代表真机已验证**。
- ⑥ **设备真编 ≠ 烧板** —— 设备侧只由 `firmware-build` 闸承载（真编译 + 分区余量），本相位未烧板。
- ⑦ **P3 覆盖层「可见 chrome 面」的机判强度 = 源锚代理** —— SC-A5 三条新结构齿钉「槽文案单源 / 页点判据纳入 5 号态 / 无自画键帽」，但非像素级断言；运行时真帧观察仍归 ⑤ 的 SC-A9。
- ⑧ **契约 wire 断言面** —— SC-A7/A8 逐字对照 `docs/contracts/interface-contract.md` §四 `kidpal_story_fetch` 与 §1.3④ `story_text` 消费语义；**契约文件本包零 diff**（`git diff main...HEAD -- docs/contracts/` 为空），属「上游已 LANDED、本包只消费」，非未覆盖。
- ⑨ **其余 acceptance（SC-A1..A8）** —— 全部有断言且绿（host 13 齿 + server 7 用例 + 结构齿）。
- ⑩ **WF-F2-06 标题「大字」未落地（显式 deferred，非达标）** —— 现状 = `font_puhui_14_1` + 水平居中；阻塞原因 = 大字字体 `font_puhui_20_4` 需 host runner sdkconfig 开 `LV_FONT_FMT_TXT_LARGE`（该文件不在本包 changeManifest，扩单成本 > 本批收益），且设备内置 30px 字体缺字；去向 = **后续视觉批候选**（立项时评估 sdkconfig 扩单 / 字体资源补齐），或 SC-A9 真机目检后另立观感快修。
- （零缺口项明写）**none** —— 除上述 ①–⑩ 外无其它未覆盖条目。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包 diff 与新件扫描**零新增 `minimal:` 标记**） | — | — | no-trigger |

> 邻域注记（非本包债务）：`firmware/main/display/story_page.c` 既有四处 `minimal:` 标记均在本包 diff hunk **之外**（上游 K2/RP-A2 既有），不入本表。本包修复轮**净减少**一处重复实现（覆盖层自画键帽死绘制已删，键帽单源 = keybar.c）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| ~~BLOCKING~~ **→ RESOLVED** | **hard-violation**（原判） | P3 WF-F2-06 覆盖层 · 底栏键帽条 / 页点（story_page.c + lcd_display.cc + keybar.c） | **原 finding**：覆盖层出现时底栏仍是 P3 列表基层形态（`说话·双击看`/`下移` 与两键真实行为不符＝状态误导）、3 页点常显、`story_page.c` 自画键帽被不透明底条遮成死绘制（双画法源）。**现状（复验）= 已修复**：`keybar.c` 增 `KSTORY_PAGE_BODY_UNAVAILABLE` 槽（文案经 `story_page.h` 宏单源：`知道了`/`关闭`）；`lcd_display.cc` 以 `dots_overlay`（layer 栈归一值 OR 5 号态）驱动页点隐藏并把 5 号态纳入键帽槽裁决策；`story_page.c` 删除自画键帽（键帽单源 = keybar.c） | 原 Basis：`docs/ui/UIUX-DESIGN-BRIEF.md:101`、`docs/ui/UIUX-DESIGN-BRIEF.md:32`、`docs/requirements.md:382` + 设计稿 + changed-code hunks（`lcd_display.cc:1378-1393 / :1410`、`kidpal_render_node.cc:953-961 / :1173-1187`、`story_page.c:1281-1290`）。**复验证据**：fix commit `8f80394a`；scoped r5（invocationId `eb7f15d0-f703-45c1-9e6e-1c74afa8a267`，`ok:true`）；final r6（`475c0184-1a11-47d5-b6f5-4e6cf21fd0f7`，`factOk:true`）；新增 3 条冻结齿实跑 PASS（`story_cache.acceptance.c:1549` 槽文案单源 / `:1598` 页点判据 / `:1648` 唯一画法源）；变异可杀（taskpackage `mutationEvidence` SC-A5 四齿 `turnedRed=true` + integrator 独立抽验 `dots_overlay→overlay_top` 在 `:1631` 复现红、还原复绿） | **已解除阻断，无进一步动作**。残余观察 = 真帧像素级表现归 SC-A9（见未覆盖面 ⑤⑦） |
| SHOULD | judgement-call | UI · WF-F2-06 标题字号 | 标题「这篇暂时打不开」仍用 `font_puhui_14_1`（与正文同号），brief 写「**大字**」、设计稿标题显著大于正文。**居中部分已修**（`lv_obj_align(LV_ALIGN_TOP_MID)`，原 x=118 左偏已消）；字号层级未兑现 | changed-code hunk `story_page.c:1236-1247` + brief:101「大字」+ 设计稿；heuristic：告知型覆盖层的字号层级是可读性信号。**fixer 显式 back-out 理由**：大字字体 `font_puhui_20_4` 需 host runner sdkconfig 开 `LV_FONT_FMT_TXT_LARGE`（`firmware/main/test/host_app/sdkconfig.defaults` 不在本包 changeManifest，扩 manifest 收益 < 成本）；设备内置 30px 字体缺字 | **deferred（不得记为已达标）**：去向 = 后续视觉批候选（立项评估 sdkconfig 扩单 / 字体资源）或 SC-A9 真机目检后另立观感快修。不阻塞合并（任务可完成、话术/形态/键语义已正确） |
| NIT | judgement-call | firmware/main/mcp_server.cc `StorySendFetchBatch` | 发帧前未过 `kidpal_story_id_safe`：id 来源 = 云端 `story_list` items（`parse` 只做 safe-truncate），若含引号/反斜杠会拼出畸形 JSON 帧 → 无应答 → 10s 超时降级（无路径注入面：写卡/读卡有 [C9] 双门，front 侧 `_safe_story_path` 亦拒绝） | changed-code hunk `mcp_server.cc:353-376`；heuristic：输入校验不对称（同一 id 在落盘面双门、在出帧面无门），但当前 id 唯一来源是云端受控列表 | **accepted-with-rationale**（本批不改）：畸形帧的最坏后果 = 超时后走 WF-F2-06 可见降级，无注入、无崩溃；升级触发 = story id 来源扩面（如语音查询通路）时在 `StorySendFetchBatch` 前加 `kidpal_story_id_safe` 过滤 |
| NIT | judgement-call | docs/notes | `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md:62` 原「业务数据不落板、决策 24 介质禁令不变」stale（story 正文按窄例外落卡） | hard-rule：scope-map 无 agent allow 覆盖 `docs/notes/**`（integrator allow 实证不含该路径） | **resolved**：主线程已按 `docs-delta.md` §2 ready-to-apply 文本代落（工作树 2026-10-17 18:52，仅第 62 行变动、其余行与 EOL 未动，`git diff` 实核）；integrator 随本轮 closeout commit 收口、未改一字。无残余动作 |
| FYI | judgement-call | firmware/main/test/p1_auxbar_style.acceptance.c | 既有 host 套件 3 红（AB-A1/A2/A4）在**上游包 261015-p1-auxbar-style-fix** 冻结件与落地态漂移上，**非本包引入** | `taskpackage.json.redBaselineResult` 锚 + integrator 独立复跑逐条 FAIL 行实核（AB-A4/AB-A2/AB-A1 三条与基线逐字同） | 归上游包 owner 的 tighten-only 窗口；**勿记本包头上**（本包 13 齿全绿） |

> 无 hard-violation 之外的阻断项；结构闸（firmware-build / contract-lint）PASS 不复述为 finding（归 verifier evidence）。**本包无未决 BLOCKING ⇒ 可进入 supervisor 机械闸。**

## Five-Axis Review

1. **Correctness**：对照 `S-F2.1-AC2 / AC7`（+ `S-F2.3-AC1/AC2` 不变）逐条——AC2「双击一视同仁进阅读」由三分支兑现（命中本地镜像读卡零网络 / 未命中插队 fetch / 取不回降级），正文一律按**光标所选 id** 取（卡分支按 id 读文件、fetch 分支按 id 等应答并以 `kidpal_story_reader_id_match` 复核），旧「盲开上一轮缓存」净根因已消；AC7「可见提示 + 正文须与所选一致」由覆盖层 + id 校验兑现，**且覆盖层 chrome 面本轮已达 brief 口径**（底栏槽 `知道了/关闭` 单源、页点隐藏、无死绘制）。边界：卡不在位 / 坏卡短路（不 fopen、不崩溃、正文仍 RAM 显示）；写卡失败只清该篇（不整层清空）；索引 16 满拒绝；`..`/分隔符/超长 id 拒绝；fetch 在途 15s 有界收口防预取永停；双击等待 10s 超时；`found→card` 分支读失败时移出索引转在线取回。**语义判断留痕**：`ws_busy` 以 `Application::GetDeviceState() != kDeviceStateIdle` 代理（无公开连接查询面）——预取是优化、失败自然降级，发帧前另有 `SendText` 通道失败兜底；`dots_overlay` 是 `kidpal_keybar_sync_locked` 内**只读消费点**的并入（不动层栈裁决 owner，守 KB-E3），未改变任何层 push/pop 语义。
2. **Test quality**：冻结件现 **13 TEST_CASE**（fixer 轮 10 → 13，tighten-only 只增不改）；新增三条均为结构齿但带语义自检（首条断言 `kidpal_keybar_story_slots(KSTORY_PAGE_BODY_UNAVAILABLE)` 返回 0 且左右槽逐字 = `知道了`/`关闭`、负向断言不是基层文案、NULL 防御 -1；第二条三路接受合法落法并拒绝「判据未纳入」；第三条先自检截到的分支含话术宏、再断言无自画键帽字面且 keybar.c 承载该槽——防「删整段代码把齿伪装成绿」）。server 7 用例覆盖「合法逐篇应答 / 非法与不存在拒绝+审计+零帧 / 空 ids no-op / 多 id 帧序 / 畸形载荷不崩不下透 / 无盖章与归档侧效应 / seq 缺省三键形状」。变异取证：SC-A1..SC-A8 全条 `turnedRed` + 还原复绿，chrome 面 4 条受影响齿在案；integrator 独立抽验页点判据变异确凿转红。**残余缺口**：结构齿是源锚代理，真帧像素表现仍需 SC-A9 目检（已如实申报）。
3. **Contract/boundary safety**：`docs/contracts/**` 零 diff；wire 逐字对照 §四（`{"session_id","type":"kidpal_story_fetch","ids":[...]}`）+ §1.3④（应答复用 `story_text` 形状、零新字段、不触发盖章）；设备端不造 id（闭集来源 = `story_list` items）、front 逐 id 校验账本存在 + 哨兵头；云端归档仍是唯一真相源（设备零写云动作、只读镜像），决策 24 窄例外面（提醒/课表/日志）零触碰。**修复轮边界纪律**：新文案宏落 `story_page.h`（话术单源同址）而非 keybar.c，使 keybar.c 保持对语音闭集键帽字面的零重抄（KB-E2 冻结齿约束）；`keybar.c` / `lcd_display.cc` 已按 planner 处置扩入 changeManifest（`taskpackage.json` 实核 21 条）。无新依赖、无跨层调用、无绕过 contracts 的本地类型。
4. **Operability/security**：**轴 1 data-at-rest**——落盘内容仅 story 正文（与云端归档同源文本），不含 token/凭据/个人信息；明文可读属 human OQ-3 已裁接受风险，由 TECH-261016（deferred）跟踪，本节未以「已评审」冒充「已防护」。**轴 2 路径注入**——固件 `kidpal_story_id_safe`（长度 <64、`..`、字符集 `[A-Za-z0-9._-]`）在写卡 `story_sd_io.cc:61` / 读卡 `:98` / 启动扫描 `:157` / 索引 commit `story_cache.c:151` **四处**前置；云端 `story.py::_safe_story_path` 为 delete/fetch **共享单源**（类型 + `..` + 分隔符 + `resolve` 逃逸 + 哨兵头），违规 warning 审计。可运维：`kdp-` corr_id 进日志、拒绝/失败均有 WARN 不静默；预取/等待 tick 三驱动全空即自删（不常驻）；`story_sd_write` 半截文件落盘即 `remove` 清场。**零新增出网面、零权限提升**。修复轮新代码为零新增权限面（keybar 槽纯值表 + 页点判据布尔合并 + 删绘图代码）。
5. **Maintainability/simplicity**：纯逻辑 / 芯片侧分层清晰（`story_cache.c` 零 FATFS/LVGL，`story_sd_io.cc` 薄适配，`sd_mounted` 显式入参注入）；状态单元按单包内「一次设计一次实现」纪律落地；front 复用 `gw_story.story_text()` 零新字段、`story.py` 抽共享 id 安全单源（deletion test：删 `_safe_story_path` 则复杂度回流两处 → 深模组，留）。**上一轮唯一的「双画法源」浅模组已被消除**：覆盖层键帽单源 = keybar.c，`story_page.c` 渲染分支注释显式记录取舍（键帽不在此画、字号 deferred 原因），后续维护者无需猜。无过早抽象、无未消费 config。

## Residual Risk

- **Known limitation**：
  - ① **隐私防护未做**（明文落卡，human OQ-3 裁定接受）——见下方 decisionCoverage 段。
  - ② 缓存淘汰 / 光标预取 / 热插拔检测属非目标；卡满后新篇不缓存（仍可在线看）。
  - ③ **SC-A9 真机 live-manual 未跑**（含覆盖层真帧目检、SD 读回 roundtrip、三类记录、离线降级、id 错配回归位、预取期语音不卡顿、LFN heap 水位）。
  - ④ **WF-F2-06 标题「大字」deferred**（未达标、显式挂账）：需 host sdkconfig 扩单 + 字体资源，去向 = 后续视觉批候选。
  - ⑤ 上游 `p1_auxbar_style` 3 红（AB-A1/A2/A4）非本包引入。
- **Follow-up recommended**：真机轮（SC-A9）按未覆盖面 ⑤ 清单执行；视觉批（大字 + `LV_FONT_FMT_TXT_LARGE` 扩单评估 / 字体补齐）；`StorySendFetchBatch` id 过滤（NIT，来源扩面时触发）。
- **Human decision needed**：① 本包 `riskTier=high` ⇒ supervisor 合并门升级 human（预期路径，非缺陷；all-mock 前置申报亦在案）；② 真机 live-manual 轮需 human 持板；③ 视觉批是否立项（含字体/sdkconfig 扩单取舍）。
- **decisionCoverage 裁决遗留**：本包 `decisionCoverage` 共 8 条，其中 **`landsIn: "dropped"` 仅 1 条**——「隐私面：本批只落盘、不加防护」（`sourceRefs` = `delivery/intake/261016-p3-story-body-sd-cache.md#openQ`，`reason` = 「human 2026-10-16 OQ-3 裁定本批范围只落盘不加防护；防护面另以 TECH-261016-sd-story-confidentiality（deferred）跟踪」）。**如实说明**：该条 `reason` **不以机械前缀 `risk-accepted` 开头**（故按 `reason.startsWith('risk-accepted')` 的机筛不会命中），但其实质是 **human 已裁定的接受风险**，来源 = human 2026-10-16 OQ-3 原话「将来给页面记录一个口令，存储加一个混淆，记一个 tech item，将来再处理」。按风险实质列示：
    - `D1: 隐私面（story 正文明文落卡、本批不加防护）— landsIn: dropped — reason: human 2026-10-16 OQ-3 裁定（只落盘不加防护）— 跟踪件 TECH-261016-sd-story-confidentiality（deferred）。`

## 集成执行回报

- **范围圈定（round-6 复跑）**：`detect_changes`（per-Pod 图索引 `C-AI-KidPal-pod-m2` 可用、未降级）167 seed / 26 impacted，新增改动面 = `display/keybar.c` + `display/lcd_display.cc`，impacted_modules 与 round-3 同集合、**零空降模块**；树扫消费者按 grep 双确认补入 `keybar_three_slot.acceptance.c` / `voice_rightcol_label.acceptance.c`，清单落盘 `test-scope-final.txt`（round 6）。
- **终验**：final r6 10 闸全绿（`ok:true` / `observedGateExit=0`），`verifier-evidence-check` → `factOk:true`；host 全量独立复跑 195T/3F（3 红 = 上游 AB-A1/A2/A4）；server 两套件 181/4 deselected/33 subtests + 91 passed。
- **变异抽验**：`dots_overlay→overlay_top` 单点回退 → SC-A5 页点判据齿在 `story_cache.acceptance.c:1631` 转红，还原后复绿、工作树干净。
- 本树**无跨包冲突、无 rebase、无 Merge-Chain 触发**（单包、无对端包、常驻分支 `dev/pod-m2`）；无 `supervisor-verdict.json` 等机械闸 fs-write 产物待收口（supervisor 门由 planner 侧机械闸跑）。
- 里程碑：`deliveryImpact=none`，`delivery/milestones/milestones.jsonl` **无阶段跳变**（本包不触状态转移；未改 STATUS.md、未跑 delivery-status 翻账）。
- `.codebase-memory/artifact.json` / `graph.db.zst` 常驻脏不入 commit（AGENTS.md 纪律）。
- **结论**：闸面全绿、变异闭环、无未决 BLOCKING ⇒ **本包可交 planner 跑 `supervisor-verdict-gate.mjs` 机械闸**（riskTier=high 预期升级 human）；integrator 未 push、未 merge。
