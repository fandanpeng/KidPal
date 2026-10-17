# Review Packet · 261016-p3-story-body-sd-cache

> integrator · 2026-10-17 · rigor=full / riskTier=high / sourceType=proposal · 分支 `dev/pod-m2` · **含 1 条 BLOCKING finding → 停在本包合并门前，交回 planner 派 fixer；不 push、不 merge、不自行判定通过**。

## Change Size

- **Files changed（实现面 19 件）**：`firmware/main/display/story_cache.c`（新 +310）、`story_cache.h`（新 +143）、`boards/waveshare-s3-rlcd-4.2/story_sd_io.cc`（新 +167）、`boards/common/board.h`（+5）、`boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+7/-3）、`firmware/main/CMakeLists.txt`（+6）、`protocols/protocol.cc`（+45）、`protocols/protocol.h`（+22/-1）、`mcp_server.cc`（+435/-1）、`display/story_page.c`（+88）、`display/story_page.h`（+31/-1）、`firmware/sdkconfig`（+4/-4 面）、`test/host_app/main/CMakeLists.txt`（+11）、`test/story_cache.acceptance.c`（冻结件 +1254）、`test/acceptance/red-baseline/sc_red_wrapper.sh`（新 +23）、`test/acceptance/red-baseline/unity.h`（+10 additive）、`server/gateway/front/front_app.py`（+25）、`server/gateway/story.py`（+83/-17）、`server/acceptance/test_story_fetch.py`（冻结件 +623）。
- **Net diff**：实现面 **+3252/-40（19 文件）**；全分支（含 delivery / docs 簿记与 closeout 制品）29 文件 **+4180/-158**。基准 = `main...HEAD`（merge_base `b6cc8945`）。
- **Scope**：P3 story 正文 SD 只读镜像（`/sdcard/story/<id>.txt`）+ 列表当屏预取 + 双击三分支兜底 + id 错配修复 + WF-F2-06 降级覆盖层 + 云端 `kidpal_story_fetch` 收帧回帧。**契约零 diff、零新增依赖、零新增出网面、零跨层调用**。
- **Split recommendation**：**不拆**（human 触点①已定 ②单包 Full；本包单 changeId 单收口轮）。

## Verification Evidence

- **Structural gates**：final r3 `firmware-build` **真编译 build-pass**（`xiaozhi.bin 0x3218f0` = 3283184 字节，最小 app 分区余 9%）；`contract-lint` PASS（唯一 warning = 上游断链 `../../delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md`，非本包引入）。
- **Final 相位全闸**：r3 `ok:true` / `observedGateExit=0` / **10 闸全跑**（8 scoped + unit + acceptance），`verifier-evidence-check --phase final --round 3 --invocation-id fb3d31f8-…` → `factOk:true` / exit 0。
- **Acceptance / unit（按 test-scope 圈定真跑）**：`unit` 闸 `executor-run`（scope 内 4 件 pytest）**86 passed + 3 subtests**；`acceptance` 闸 `oracle-pass`（固件冻结验收 13 件 / server 3 件）→ `executor-run` **51 passed + 3 subtests**。
- **全量交叉核对（integrator 独立复跑，非闸内）**：firmware host ELF **192 Tests / 3 Failures / 0 Ignored**（3 红 = 上游已知红 `p1_auxbar_style` AB-A1/A2/A4，新增红 **0**，逐条 FAIL 行实核）；server `acceptance/` **181 passed / 4 deselected / 33 subtests**（与基线逐字一致）；`server/tests/` **91 passed**。
- **Security gate**：`securityReviewRequired=true`、两轴复核（见「五轴 · Operability/security」）；`security` 闸 PASS。
- **Mutation check**：`mutation-check` PASS——`perAcceptance` 9 条：SC-A1..SC-A8 全条 `turnedRed=true` + 还原复绿（实测靶位行号与 `taskpackage.json.mutationEvidence.reason` 逐字一致）；SC-A9 = structural / live-manual，`not-applicable-structural`。
- **Docs sync**：`docs-sync` 闸 PASS；TECH-261013 已结项并归档（见 `docs-delta.md` §1）；`docs/notes` 待主线程落笔（§2）；`docs/ui/UIUX-DESIGN-BRIEF.md` 在途警示行已于 intake commit `c0f0297e` 撤除、核对一致。
- **机器件口径**：承重 = `verifier-evidence/rounds/r3.json`（`authority: bearing`、`producedBy: verify.mjs`，final immutable fact）；`current/verdict.json` = verify.mjs 脚本 fs-write 的 non-bearing projection，只随 commit 收口、不据以判 closeout；`verdict.json` / `verification.json` 按 closeout 制品 SSOT 为 advisory-only、**永不晋升包目录**（`closeout-check.mjs:14-15` / `changes-index.mjs:158-160` 实证），故本包不产这两件。

## test-scope 清单（final 相位范围圈定）

- **落盘路径**：`delivery/taskpackages/261016-p3-story-body-sd-cache/test-scope-final.txt`（`verify.mjs --phase=final --scope-file` 消费，round 3）。**本行即「清单路径记入 results 元数据必含行」的落点**——r3 fact 的 `results[]` 由 verify.mjs 上游 schema 固定（`id/gate/kind/ok/exit/exitCode/ms/timing/stdout/stderr/notExecuted/summary`），无 scopeFile 字段；scope 路径在该 schema 下只能落在本 review-packet 与各闸 stdout。
- **圈定方式 = 正常路径（非降级）**：`detect_changes`（per-Pod 专属图索引 `C-AI-KidPal-pod-m2`，base=main，merge_base `b6cc8945`，inbound depth=2）= **155 seed symbols / 17 impacted**，impacted_modules = `firmware/main`(8) + `server/acceptance`(2) + `server/gateway`(2) + `server/tests`(5)，**零预期外模块**；`trace_path` 抽查 `CustomBoard::SdCardMounted`（callers_total=7，含本批新增 `mcp_server::StoryCacheEnsureLoaded / StoryDispatchAct / StoryPrefetchRefresh / StoryTickTimerCb / HandleStoryKey`）与 `kidpal_story_page_input`（4 命中，含本批新件）——图索引已收进本批新符号、新鲜。
- **合成**：`acceptanceTestsFile`（2 件）∪ blast radius 映射测试 ∪ 共享 seam 相关测试（story_page.c/.h 页态 5 与 idle 缝 → story_page_v2 / record_session_live / record_session_repair(.test)；mcp_server.cc 树扫族 → char_window_overlay / reminder_overlay_unify / retract_p4_three_page / schedule_grid / schedule_grid_restore；protocol.cc 树扫族 → boot_auto_connect / voice_round_ui；board.h/.cc → sd_card_mount / p1_home_battery；server story.py/front_app.py → test_story_units / test_m4k1 / test_m4k3）∪ 安全余量（host 单 ELF 记账）。**floor** = 本包 acceptanceTestsFile ∪ blast radius ∪ 共享 seam（本包无对端包）。**圈定降级 = 无**。
- **例外申报**：`firmware/main/test/acceptance/archive/happy_flow_capture.test.c` 虽被 detect_changes 命中 hop 1，但属已归档退役面（`SKIP_DIRS` 含 `archive`、不入 host 构建），**不入清单**。

## demo 对账行（demo 证据 ⟷ 本包效果段）

- **demo 证据形态** = host runner 全量（192T/3F，冻结件 10 齿全绿）+ final 相位 10 闸绿 + 设备真编（build-pass）+ 设计稿 `docs/ui/V6/p3-body-unavailable.png`；**真机 live-manual 日志未产生**（SC-A9 未跑，如实挂账）。
- **本包效果段**（plan §建什么）= 双击任意一条记录都能进阅读（本机录 / 重启前 / 云端恢复一视同仁）；断网已留存照常打开；取不回给可见提示；**双击打开的一定是光标选中那一篇**。
- **一致性结论 = 自动化半一致、真机半未跑、且覆盖层可见面存在设计偏离**：行为齿（预取决策 / 三分支 / id 校验 / 写卡门控 / 覆盖层骨架 + 跨端接线）全绿且变异可杀；真机半挂 SC-A9；**评审另查出覆盖层出现时底栏键帽槽与页点仍是列表基层形态**（Finding ①）——该面不在冻结齿覆盖内，**demo 对账不闭环**。

## UI design-review 文本证据（bootstrap：纯文本、零 screenshot 入仓）

- **viewport coverage**：RLCD 单一物理屏 400×300（归一屏模型，无响应式断点）；WF-F2-06 覆盖层按 `docs/ui/V6/p3-body-unavailable.png` 布局坐标落地——标题 y=64、副句 y=96、选中条目框 y=130（x=40 / w=320 / h=40，星星 x=60 · 标题 x=90 · 日期 x=300）、底部键帽 y=266。**覆盖层本体布局与设计稿一致**。
- **a11y summary**：话术单源（`KSTORY_BODY_UNAVAILABLE_HINT`/`_SUB` = 设计稿逐字）、14px 普惠体、`KSY_COLOR_INK #333333` 于 RLCD 反射白底（对比度充足）；纯告知态无待确认内容、无倒计时数字（对齐删除确认静默口径）；KEY／BOOT 任一手势均可关闭（无单键依赖）；小伴不出场（不与语音姿态同框）。无屏幕阅读器面（本产品无此能力）。
- **design-drift summary（brief v5 ↔ 实现）**：① 覆盖层正文布局 = 无漂移；② **底栏键帽条 = 漂移**（brief:101「整屏覆盖层……**不显示页点**」+ 设计稿底栏 `[知道了]`/`[关闭]`；实现仍是 P3 列表基层 `说话·双击看`/`下移` + 3 页点常显）→ Finding ①；③ 标题字号 = 漂移（brief「**大字**」/设计稿标题显著大于正文；实现用与正文同号 14px 且 x=118 左偏约 30px）→ Finding ②；④ 覆盖层自画的 `[知道了]`/`[关闭]` 键帽位于不透明 top-layer 底条（y 262-300）之下 = 死绘制（同一问题的一部分，见 Finding ①）。
- **external artifact reference**：无（`repoVisualArtifactRequired=false`；设计稿 PNG 在仓内，截图/视频留仓外）。
- **residual risk**：真机目检未做（SC-A9）；finding ① 修复后需重新目检底栏形态。

## 未覆盖面申报（逐条一行，零缺口明写 none）

- ① **隐私加密防护未做** —— 现状 = 明文落卡（human 2026-10-16 OQ-3 裁定「本批只落盘、不加防护」）；去向 = `TECH-261016-sd-story-confidentiality`（`status: deferred`）跟踪，**不表述为「已防护」**。
- ② **缓存淘汰（LRU）** —— 非目标；现状 = 索引容量 16（与 `story_list` 单页窗口同值）+ 卡满即写失败、只清该篇不整层清空；旧正文文件不自动清理（容量风险留后续批）。
- ③ **光标预取** —— 非目标；现状 = 仅「进列表 / 翻屏」当屏批量 + 双击插队单篇，光标移动不触发预取。
- ④ **热插拔检测** —— 非目标；现状 = RAM 索引首次使用时惰性重建一次（`g_story_cache_scanned` 单次）；运行期拔卡 → 读失败只移出该篇；运行期插卡本开机不复扫。
- ⑤ **SC-A9 真机 live-manual 轮未跑** —— human 持板硬前置（离线双击已留存 / 三类记录进阅读 / 降级覆盖层 10s 与双键关闭 / id 错配回归位 / 预取期语音不卡顿 / SD 读回 roundtrip / heap 水位）；**本报告不代表真机已验证**。
- ⑥ **设备真编 ≠ 烧板** —— 设备侧只由 `firmware-build` 闸承载（真编译 + 分区余量），本相位未烧板。
- ⑦ **P3 覆盖层「可见 chrome 面」（底栏槽文案 + 页点）无自动断言** —— 冻结件 SC-A5 只钉「话术单源 / 10s / 任一手势关闭」，键帽条与页点属跨模块 chrome、不在冻结面；本次语义 review 在该缺口查出 **BLOCKING 缺陷**（Finding ①），转 fixer 补齿/修复。
- ⑧ **契约 wire 断言面** —— SC-A7/A8 逐字对照 `docs/contracts/interface-contract.md` §四 `kidpal_story_fetch` 与 §1.3④ `story_text` 消费语义；**契约文件本包零 diff**（`git diff main...HEAD -- docs/contracts/` 为空），属「上游已 LANDED、本包只消费」，非未覆盖。
- ⑨ **其余 acceptance（SC-A1..A8）** —— 全部有断言且绿（host 10 齿 + server 7 用例 + 结构齿）。
- （零缺口项明写）**none** —— 除上述 ①–⑦ 外无其它未覆盖条目。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包 diff 与新件扫描**零新增 `minimal:` 标记**） | — | — | no-trigger |

> 邻域注记（非本包债务）：`firmware/main/display/story_page.c:1001 / :1324 / :1406 / :1450` 四处既有 `minimal:` 标记均在本包 diff hunk **之外**（上游 K2/RP-A2 既有），不入本表。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| **BLOCKING** | **hard-violation** | P3 WF-F2-06 覆盖层 · 底栏键帽条 / 页点（firmware/main/display/story_page.c + lcd_display.cc + keybar.c + kidpal_render_node.cc） | **正文取不回覆盖层出现时，孩子看到的底栏仍是 P3 列表基层形态**：左右槽 `说话·双击看` / `下移`（两键此时真实行为都是「关闭回列表」，提示与实际动作不符＝状态误导），且 3 个页点常显（设计/brief 要求覆盖层不显示页点）。同时 `story_page.c` 在覆盖层内自画的 `[知道了]` / `[关闭]` 键帽位于不透明 top-layer 底条（y 262-300）之下，**是被完全遮住的死绘制**——同一语义存在两个画法源，其中一个不可见 | **hard-violation**：`docs/ui/UIUX-DESIGN-BRIEF.md:101`（WF-F2-06「**形态 = 整屏覆盖层**（与删除确认 WF-F2-05 / 提醒弹窗同族——覆盖列表、**不显示页点**、小伴不出场）」）+ `docs/ui/UIUX-DESIGN-BRIEF.md:32`（「覆盖层（提醒弹窗/查字窗/语音轮演出/**正文取不回提示**）内 = 关掉顶层回原页」）+ `docs/requirements.md:382`（WF-F2-06「页点消失 = 覆盖层口径」）+ 设计稿 `docs/ui/V6/p3-body-unavailable.png`（底栏 `[知道了]`/`[关闭]`、无页点）。changed-code hunks：`lcd_display.cc:1378-1393`（story_sub 只分支 DELETE/READER/RECORDING，5 号态落 `kidpal_keybar_slots(P3, overlay_top=0)` → 基层槽表）；`lcd_display.cc:1410`（`overlay_top=0` → `kidpal_keybar_dots` 返回 `visible=1`）；`kidpal_render_node.cc:953-961 / :1173-1187`（无该态覆盖层 → `front_is_overlay()=0`、substate=5 原样透出）；`story_page.c:1281-1290`（自画键帽 y=266..290 被 opaque `bottom_bar_`（`lcd_display.cc:1015-1044` + `KidPalReparentChatToLayer` :2457-2469 上提 lv_layer_top）遮住） | **交回 planner 派 fixer（不得进入合并门）**。建议最小修：① `keybar.c::kidpal_keybar_story_slots` 增 `case KSTORY_PAGE_BODY_UNAVAILABLE` = `知道了` / `关闭`（沿用值域单源），`lcd_display.cc` 把 `story_sub == 5` 纳入 `overlay_top` 的有效判据使页点隐藏；② 删除 `story_page.c` 覆盖层内自画键帽（去第二画法源）；③ 补一条可机判齿（槽文案 + 页点可见性）或在冻结件 SC-A5 增结构齿。**注意**：`keybar.c` / `lcd_display.cc` / `kidpal_render_node.cc` **不在本包 changeManifest** → planner 须先扩 manifest；且须核 `keybar_three_slot.acceptance.c` 是否有对该值域的冻结负向齿/闭集计数齿 |
| SHOULD | judgement-call | UI · WF-F2-06 标题字号与对齐 | 标题「这篇暂时打不开」用 `font_puhui_14_1`（与正文同号）且左端钉 x=118（7 字 ×14px → 视觉中心 ≈167，屏心 200，**左偏约 33px**）；brief 写「**大字**」、设计稿标题显著大于正文并居中——层级与对齐均未兑现 | changed-code hunk `story_page.c:1236-1247`（`lv_label_set_text(q, KSTORY_BODY_UNAVAILABLE_HINT)` + `lv_obj_set_pos(q, 118, hint_y)` + `font_puhui_14_1`）；对照 `docs/ui/UIUX-DESIGN-BRIEF.md:101`「大字」+ 设计稿 `docs/ui/V6/p3-body-unavailable.png`；heuristic：屏幕可见层级（大字 vs 正文）是告知型覆盖层的核心可读性信号 | 建议随 Finding ① 的修复轮一并处理（最小 = 标题挂 `font_puhui_20_4`（`managed_components/78__xiaozhi-fonts` 在位，需 CMake 接线）+ 水平居中）；**不单独阻塞合并**（任务可完成、话术/形态/键语义正确）。若 fixer 成本过高，转真机视觉轮（SC-A9）目检后另立观感快修 |
| NIT | judgement-call | firmware/main/mcp_server.cc `StorySendFetchBatch` | 发帧前未过 `kidpal_story_id_safe`：id 来源 = 云端 `story_list` items（`parse` 只做 safe-truncate），若含引号/反斜杠会拼出畸形 JSON 帧 → 无应答 → 10s 超时降级（无路径注入面：写卡/读卡有 [C9] 双门，front 侧 `_safe_story_path` 亦拒绝） | changed-code hunk `mcp_server.cc:353-376`（只拷 id + `StoryMarkFetchIssued`，零字符集门）；heuristic：输入校验不对称（同一 id 在落盘面双门、在出帧面无门） | 本批无需动作（front 侧拒绝 + 超时降级兜底）；后续若 story id 来源扩面（如语音查询通路），在 `StorySendFetchBatch` 前加 `kidpal_story_id_safe` 过滤 |
| NIT | judgement-call | docs/notes | `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md:62` 现文「业务数据不落板、决策 24 介质禁令不变」已 stale（story 正文按窄例外落卡），待主线程落笔 | hard-rule：scope-map 无 agent allow 覆盖 `docs/notes/**`（integrator allow 实证不含该路径）；改动为事实态盘点同步、非 spec 面变更 | 主线程（human 主会话）按 `docs-delta.md` §2 ready-to-apply 逐字文本落笔（含 CRLF 保持） |
| FYI | judgement-call | firmware/main/test/p1_auxbar_style.acceptance.c | 既有 host 套件 3 红（AB-A1/A2/A4）在**上游包 261015-p1-auxbar-style-fix** 冻结件与落地态漂移上，**非本包引入** | `taskpackage.json.redBaselineResult` 锚 + integrator 独立复跑逐条 FAIL 行实核：3 红名单与基线逐字同 | 归上游包 owner 的 tighten-only 窗口；**勿记本包头上**（本包 story_cache 10 齿全绿） |

> 无 hard-violation 之外的阻断项；结构闸（firmware-build / contract-lint）PASS 不复述为 finding（归 verifier evidence）。**Finding ① 为 BLOCKING ⇒ 本包不进入合并门。**

## Five-Axis Review

1. **Correctness**：对照 `S-F2.1-AC2 / AC7`（+ `S-F2.3-AC1/AC2` 不变）逐条——AC2「双击一视同仁进阅读」由三分支兑现（命中本地镜像读卡零网络 / 未命中插队 fetch / 取不回降级），正文一律按**光标所选 id** 取（卡分支按 id 读文件、fetch 分支按 id 等应答并以 `kidpal_story_reader_id_match` 复核），旧「盲开上一轮缓存」净根因已消；AC7「可见提示 + 正文须与所选一致」由覆盖层 + id 校验兑现（**可见 chrome 面见 Finding ①**）。边界：卡不在位 / 坏卡短路（不 fopen、不崩溃、正文仍 RAM 显示）；写卡失败只清该篇（不整层清空）；索引 16 满拒绝；`..`/分隔符/超长 id 拒绝；fetch 在途 15s 有界收口防预取永停；双击等待 10s 超时；`found→card` 分支读失败时移出索引转在线取回。**一处语义判断留痕**：`ws_busy` 以 `Application::GetDeviceState() != kDeviceStateIdle` 代理（无公开连接查询面）——预取是优化、失败自然降级，且发帧前另有 `SendText` 通道自身失败兜底。
2. **Test quality**：冻结件 10 齿断言真实行为（返回码/决策码级，非注释假绿）+ 结构齿扫真实工作树；server 7 用例覆盖「合法逐篇应答 / 非法与不存在拒绝+审计+零帧 / 空 ids no-op / 多 id 帧序 / 畸形载荷不崩不下透 / 无盖章与归档侧效应（stories 目录字节快照）/ seq 缺省三键形状」；变异取证 **8/8 功能条 turnedRed + 还原复绿**（SC-A9 结构豁免）。**缺口**：覆盖层「可见 chrome 面」（底栏槽文案 / 页点）无自动断言，且正是 Finding ① 所在——fixer 轮应补齿（否则同类回归无防线）。
3. **Contract/boundary safety**：`docs/contracts/**` 零 diff；wire 逐字对照 §四（`{"session_id","type":"kidpal_story_fetch","ids":[...]}`）+ §1.3④（应答复用 `story_text` 形状、零新字段、不触发盖章）；设备端不造 id（闭集来源 = `story_list` items）、front 逐 id 校验账本存在 + 哨兵头；**云端归档仍是唯一真相源**（设备零写云动作、只读镜像），决策 24 窄例外面（提醒/课表/日志）零触碰。无新依赖、无跨层调用、无绕过 contracts 的本地类型（纯逻辑层消费 `story_page.h` 在世类型）。
4. **Operability/security**：**轴 1 data-at-rest**——落盘内容仅 story 正文（与云端归档同源文本），不含 token/凭据/个人信息；明文可读属 human OQ-3 已裁接受风险，由 TECH-261016（deferred）跟踪，本节未以「已评审」冒充「已防护」。**轴 2 路径注入**——固件 `kidpal_story_id_safe`（长度 <64、`..`、字符集 `[A-Za-z0-9._-]`）在写卡 `story_sd_io.cc:61` / 读卡 `:98` / 启动扫描 `:157` / 索引 commit `story_cache.c:151` **四处**前置；云端 `story.py::_safe_story_path` 为 delete/fetch **共享单源**（类型 + `..` + 分隔符 + `resolve` 逃逸 + 哨兵头），违规 warning 审计。可运维：`kdp-` corr_id 进日志、拒绝/失败均有 WARN 不静默；预取/等待 tick 三驱动全空即自删（不常驻）；`story_sd_write` 半截文件落盘即 `remove` 清场。**零新增出网面、零权限提升**。
5. **Maintainability/simplicity**：纯逻辑 / 芯片侧分层清晰（`story_cache.c` 零 FATFS/LVGL，`story_sd_io.cc` 薄适配，`sd_mounted` 显式入参注入、不碰板级私有态）；状态单元（预取队列 / 插队 / pending / 索引）按单包内「一次设计一次实现」纪律落地，两分支只消费入口；front 复用 `gw_story.story_text()` 零新字段、`story.py` 抽共享 id 安全单源（deletion test：删 `_safe_story_path` 则复杂度回流两处调用方 → 深模组，留）。**反面**：`story_page.c` 覆盖层自画键帽与 `keybar` chrome 形成**双画法源**且其中一个不可见（Finding ①）——这是本包唯一的浅模组/重复实现信号，应随修复删除。无过早抽象、无未消费 config。

## Residual Risk

- **Known limitation**：
  - ① **Finding ①（BLOCKING）未修前，覆盖层底栏与页点在真机可见面上不符合 brief/设计稿**（槽文案误导 + 页点未隐藏 + 死绘制）。
  - ② **隐私防护未做**（明文落卡，human OQ-3 裁定接受）——见下方 decisionCoverage 段。
  - ③ 缓存淘汰 / 光标预取 / 热插拔检测属非目标；卡满后新篇不缓存（仍可在线看）。
  - ④ SC-A9 真机 live-manual 未跑（含 SD 读回 roundtrip、三类记录、离线降级、id 错配回归位、预取期语音不卡顿、LFN heap 水位）。
  - ⑤ `docs/notes:62` 待主线程落笔（exitCriteria 未闭项之一）。
- **Follow-up recommended**：fixer 修 Finding ① +②（顺带删死绘制、补齿）；上游 `p1_auxbar_style` 3 红归其 owner 的 tighten-only 窗口；`StorySendFetchBatch` id 过滤（NIT）留观察。
- **Human decision needed**：① 本包 `riskTier=high` ⇒ supervisor 合并门升级 human（预期路径，非缺陷；all-mock 前置申报亦在案）；② 真机 live-manual 轮需 human 持板；③ Finding ① 的修复口径（是否接受「扩 changeManifest + 改 keybar/lcd_display」）由 planner/human 裁。
- **decisionCoverage 裁决遗留**：本包 `decisionCoverage` 共 8 条，其中 **`landsIn: "dropped"` 仅 1 条**——「隐私面：本批只落盘、不加防护」（`sourceRefs` = `delivery/intake/261016-p3-story-body-sd-cache.md#openQ`，`reason` = 「human 2026-10-16 OQ-3 裁定本批范围只落盘不加防护；防护面另以 TECH-261016-sd-story-confidentiality（deferred）跟踪」）。**如实说明**：该条 `reason` **不以机械前缀 `risk-accepted` 开头**（故按 `reason.startsWith('risk-accepted')` 的机筛不会命中），但其实质是 **human 已裁定的接受风险**，来源 = human 2026-10-16 OQ-3 原话「将来给页面记录一个口令，存储加一个混淆，记一个 tech item，将来再处理」。按风险实质列示：
    - `D1: 隐私面（story 正文明文落卡、本批不加防护）— landsIn: dropped — reason: human 2026-10-16 OQ-3 裁定（只落盘不加防护）— 跟踪件 TECH-261016-sd-story-confidentiality（deferred）。`

## 集成执行回报

- `detect_changes` 已跑（per-Pod 专属图索引 `C-AI-KidPal-pod-m2` 可用、未降级）；`trace_path` 抽查同结论；改动只影响预期符号（story 面 + SD 面 + front/ story 面），**零空降模块**。
- 本树**无跨包冲突、无 rebase、无 Merge-Chain 触发**（单包、无对端包、常驻分支 `dev/pod-m2`）；无 `supervisor-verdict.json` 等机械闸 fs-write 产物待收口（supervisor 门由 planner 侧机械闸跑）。
- 里程碑：`deliveryImpact=none`，`delivery/milestones/milestones.jsonl` **无阶段跳变**（本包不触状态转移；未改 STATUS.md、未跑 delivery-status 翻账）。
- `.codebase-memory/artifact.json` / `graph.db.zst` 常驻脏不入 commit（AGENTS.md 纪律）。
- **结论**：闸面全绿、变异闭环，但 **Finding ① 为 BLOCKING ⇒ 本包停在合并门前**，等待 planner 派 fixer（建议同轮把 Finding ② 与补齿一并处理）；修复后重跑 verifier scoped + 本轮 final 圈定复验，再入 supervisor 机械闸。
