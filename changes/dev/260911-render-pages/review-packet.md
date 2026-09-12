# Review Packet — 260911-render-pages（批次③ 渲染观感 + V3 整屏还原双轨）

> integrator 产出 · 2026-09-12 · 分支 `dev/260911-render-pages`（base = main，HEAD = `83a6e66` 260911-display-first-m1 合并收口后）。
> 供 supervisor 合并门复核。本包 `riskTier=high`（`gateProfile=[host-teeth, device-round, frozen-acceptance-revision]`），照常触发 supervisor 升级人类。
> 交付本体：缺陷轨四条（#9 故事页排版 READER 按 screens 切片只画当前屏 + #10 字体缺字「猬」GB2312 全集自产子集 + #11 课表页数据链固件缓存空态友好页 + #12 P2-A1 恒红齿修齿方案 B）+ 还原轨（P2/P3/P4 页面换皮 V3 框族/网点/字体 + P4 笔顺页观感 + 语音轮五态演出换皮 `kidpal_voice_states` 纯逻辑单源 + V3 字体体系并包 fonts_root 三字体 SPIFFS 非 KAP1 落地 + 唯一新增 KAP1 = `bubble_tail_24.bin` 走 U 闭集修订 47→48）。
> **合并门必读**：本包无 BLOCKING finding。承重 final fact `r5` **非全绿**（`ok:false`），唯一红 = `unit`/`acceptance` 两闸同源的 `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4）——**在册他人红**（owner=260829-smoke-e2e，见同目录 `existing-red-inventory.md` R1；与先例 PKG-4 合并夜同一红、同一处置通道），与本包零相关、零新增红。host runner（WSL ELF 全量）3 红（WSA-A6:830 / WSA-A4:663 / VRT-A2·E2:878）均为在册既存红，在册第 4 条红 P2-A1 已由 RP-A1 修齿消解出失败清单。故合并门需**按红证据 ESCALATE 通道**提 human 裁定「留红如案、红转交各自 owner」；本包自身面（scoped 7 闸 + 新齿红基线红→绿 + 冻结面零越界）全绿。真机整屏 V3 对照轮（RP-A5 等 4 项）= pending，owner=human，先例同 PKG-4（DF-A3/A5/A6 closeout 时 pending、human 收口轮裁 PASS）。

## Change Size

- Files changed（`git diff --numstat -- firmware/ tools/` 实测：**14 files changed, +838/−53**；另有新增文件与 closeout 制品）：
  - **实现面（firmware/display + mcp_server + CMakeLists，10 文件 +233/−18）**：
    - `firmware/main/display/kidpal_voice_states.c`（新，65 行）+ `kidpal_voice_states.h`（新，51 行）：语音轮五态内容选择单源（`kidpal_voice_state_look(int scene, kidpal_voice_look_t *out)` 纯逻辑，场景 → 姿态/气泡文案/键帽条，无芯片依赖，host_app linux target 同源——冻结契约件，RP-A9/F3 承载）。
    - `firmware/main/display/story_page.c`（+92/−3）+ `story_page.h`（+14，additive-only）：`kidpal_story_reader_slice(st, screen, lines_per_screen, out_len)` READER 渲染切片单源（指针/偏移进 story->text，零大栈缓冲，4096B 栈纪律）；READER 分支按 screens 只画当前屏；标题/正文标签设宽 + WRAP 消横向裁切；`KSTORY_READER_LINES_PER_SCREEN`（=4）行高单源（RP-A2/F2/F5）。
    - `firmware/main/display/kidpal_render_node.cc`（+21/−8）：KP_PAGE_SCHEDULE 分支 `s_sched_n<=0` 时改画友好空态（「还没有课表数据，问我『我的课表』吧」居中提示，V3 换皮纪律），不再渲染空壳课表页；语音查询路径（mcp case 4）不动（RP-A3，S0.2 勘察根因 = 固件缓存时机、云侧 producer 无罪，逃生门未触发、未扩 server 面）。
    - `firmware/main/display/lcd_display.cc`（+136）+ `lcd_display.h`（+5/−1）：五态驱动落点改写（SetChatMessage/SetStatus/SetEmotion → `kidpal_voice_state_look` 内容驱动 + 姿态/气泡/键帽条消费），layer 底座与优先级仲裁零触碰（RP-A9）。
    - `firmware/main/display/reminder_page.c`（+59/−2）：提醒弹窗压暗 LV_OPA_50 → 既有 d50 网点资产平铺合成（schedule_page 先例机制，F4；零新增 KAP1）。
    - `firmware/main/display/schedule_page.c`（+65）/ `char_stroke.c`（+3）：P2 换皮（框族/网点/涂鸦白名单）+ P4 笔顺页观感（米字格三态笔道/进度珠，B8 打包策略：笔形 3 态叠加 + 米字格单份背景）（RP-A8/A10）。
    - `firmware/main/mcp_server.cc`（+5/−3）：阅读态分页行高硬编码 4 → `KSTORY_READER_LINES_PER_SCREEN` 单源。
    - `firmware/main/CMakeLists.txt`（+4）+ `test/host_app/main/CMakeLists.txt`（+17/−1）：`kidpal_voice_states.c` 接线 + `v3_restore.acceptance.c` 注册（host_infra_smoke 之前）。
  - **资产管线（tools/asset-pipeline，新增 2 件）**：`fonts/gen_fonts.sh`（59 行，lv_font_conv 三字体自产子集管线：正文 GB2312 全集 6763 字 / 标题站酷 / 数字 Baloo 2）+ `gen_bubble_tail.mjs`（50 行，气泡尾巴 24×24 KAP1 生成）——坐标/资产转写管线化（禁手抄纪律，RP-A8/F9 承接）。
  - **资产（4 件 bin，均需 `git add -f` 入库——`firmware/.gitignore:16 *.bin` 吞 fonts_root；spiffs_root 已有否定规则）**：`firmware/main/assets/fonts_root/{font_body.bin(248KB), font_title_zcool.bin, font_num_baloo.bin}`（SPIFFS 非 KAP1 独立落地，S0.1 决策 `changes/dev/260911-render-pages/font-decision-s01.md` DECIDED）+ `firmware/main/assets/spiffs_root/bubble_tail_24.bin`（唯一新增 KAP1，U 闭集 47→48）。
  - **冻结验收件修订 3 文件 +417/−35（仅 acceptance-author 畏痕修订）**：`schedule_p2.acceptance.c`（+144/−20，RP-A1 齿 6/齿 5 定义签名定位修法，tightly-only + REVISION LOG）、`story_p3.acceptance.c`（+237，F2 当前屏正文机制齿 + F5 切片契约齿，REVISION LOG 2026-09-12 条目）、`ui_art_integration.acceptance.c`（+36/−15，B8 闭集 47→48 + 字体分型连带修订，REVISION LOG）。新增冻结件 `firmware/main/test/v3_restore.acceptance.c`（906 行，RP-A11 新产 6 齿：五态内容/接线/姿态键帽气泡资产/d50 压暗/换皮框族/字体分型闭集，红基线 = 接线后首跑全红 + 守卫绿）。
  - **closeout 制品 5 件**（本次积分收拢落 `changes/dev/260911-render-pages/`）：`review-packet.md` / `docs-delta.md` / `existing-red-inventory.md` / `verdict.json` / `verification.json`（同目录已有 `intake-survey-s02-s03.md` / `font-decision-s01.md` / `red-baseline-freeze.md` 三件先行制品）。
  - dev-loop 执行 ledger/evidence（`.claude/taskpackages/260911-render-pages/`）：`taskpackage.json`（活文件 + acceptance 冻结补记）、`plan.md`、`input-brief-{acceptance-author,builder}.md`、`pod-events.jsonl`、`verifier-evidence/rounds/r{1..5}.json`、`verify-runs.jsonl`、`arch-review{,-expansion}.md`。
- Scope: `subsystem=firmware`；`docs/contracts/**` 与 `docs/spec.md` **零改动**（contract-lint 闸 PASS）；`server/**` 零改动（S0.2 勘察裁定根因在固件侧，F6 解除确认）。
- Split recommendation: **不拆**——缺陷轨与还原轨同批扩包裁定（human-20260911 对话内三问钉死「整屏一次还原」），字体是换皮的依赖（「拆出去就没皮」），四缺陷与还原面共享 story_page/lcd_display 同文件面，拆开任一轨都断「整屏一次还原」裁定语义。

## Verification Evidence

> **承重 fact 声明（诚实口径）**：final completed-only round = **r5**（`phase:"final"`，identity = changeId+phase+round+invocationId），
> `verifier-evidence/rounds/r5.json`：`ok:false`、`observedGateExit:1`、`authority:"bearing"`、`producedBy:"verify.mjs"`、
> `invocationId 7f3c2a41-9b6e-4d58-a1c2-84e0d7b91f30`、`startedAt 2026-09-12T09:36:00.937Z / completedAt 09:36:38.645Z`。
> `ok:false` 的**唯一**成因 = `unit`/`acceptance` 两 full 闸同源红 `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4，在册他人红 R1）；scoped 7 闸**全绿**。此为本包「留红如案」的 machine 记录，非本包自身面红（见 §Findings / §Residual Risk）。
> `current/verdict.json` 为同一 `producedBy:"verify.mjs (script fs-write)"` 的投影（non-bearing compatibility projection），closeout 判断以 r5 fact 为准；本相位 `verifier-evidence-check.mjs` 已核验 fact 有效（exit 0，`factOk:false` 但 evidence 合法——与先例 PKG-4 合并夜同形态）。

- **verifier 逐轮收敛轨迹（r1→r5）**：
  - **r1**（inv `63cb93d9-…`，scoped，`ok:false`）：唯一红 = `acceptance-coverage`——`missingCoverage:["RP-A1","RP-A3","RP-A6","RP-A7","RP-A11"]`。根因 = 扩包后 acceptance 清单与冻结验收件覆盖标记尚未同步（RP-A5/A10 已标 structural，其余五条未对齐）。acceptance-author 冻结轮交付后（v3_restore 新产 6 齿 + 齿修复 + 补记落 taskpackage）消解。
  - **r2**（inv `e8935cf8-…`，scoped，`ok:false`）：唯一红 = `mutation-check`——`mutationRequired=false must include mutationSkipReason/mutationReason`。根因 = finalize 尚未补 `mutationSkipReason` 字段，planner 补记（skip 理由：本包齿以真实渲染帧/资产闭集/结构扫描为断言对象，无 vacuity 自证面）后消解。
  - **r3**（inv `3753750d-…`，scoped，`ok:false`）：唯一红 = `docs-sync`——`docsWithoutAcceptance:["docs/notes/ui-implementation-strategy.md"]`。根因 = docRef 面缺 acceptance id 锚点；主线程在 docs/notes 落 RP-A8/RP-A4 执行承载注记（integrator 无 docs/notes 写权，enforce-scope 拦截，未绕过）后消解。
  - **r4**（inv `9bf6aeb8-…`，scoped，`ok:true`，2026-09-12T09:33）：**七闸全绿**（`observedGateExit:0`），firmware-build 真编译（`build-pass`，xiaozhi.bin 0x2c35b0、app 分区余 20%）。
  - **r5**（inv `7f3c2a41-…`，final，planner 分配 round 5）：scoped 7 闸全绿 + `unit`/`acceptance` 两 full 闸红（唯一 SMK-A4）。`unit` 计数 `1 failed / 168 passed / 6 skipped / 45 deselected`、`acceptance` 计数 `1 failed / 131 passed / 6 skipped / 45 deselected`，与 launch 申报在册上游基线同名单，零新增红。
- **r5 逐闸（实测 ms）**：taskpackage-required 192 / **firmware-build 9876（`build-pass` 真编译非降级——ninja 全跑、`xiaozhi.bin 0x2c35b0`、`0xaca50 bytes (20%) free`、spiffsgen art.bin 含 bubble_tail_24 新资产）** / contract-lint 230 / acceptance-coverage 249（RP-A1..A11 全在册、`structural:["RP-A5","RP-A10"]`、`suiteGates:["RP-A7"]`、files 五件冻结验收件）/ mutation-check 216（skipped 有因）/ docs-sync 276 / security 415 / unit 13538 / acceptance 12441。
- **本包齿终态（red-baseline-freeze.md §七，builder 实现后 host runner 全量实测）**：**124 Tests / 3 Failures / 0 Ignored**，3 红逐条比对均为在册非本包红（VRT-A2·E2:878 / WSA-A6:830 / WSA-A4:663）；本包齿 v3_restore 6/6、story_p3 10/10（含 F2/F5 新齿）、schedule_p2 7/7（RP-A1 修齿后）、ui_art_integration 11/11（闭集 48）、story_page_stack 3/3（栈纪律）；KEYA/SU 不回归。在册 5 红中 P2-A1 已消解出失败清单。
- **新齿红基线先行（RP-A11）**：v3_restore 6 齿红基线 = 接线后首跑 test 1-6 全红 + 守卫绿（逐齿红因逐字核实落 `red-baseline-freeze.md` §三）；F2 机制齿判别式对行数参数不敏感双判别式布防、F5 含禁词扫描（`[512]/[1024]/[2048]/[4096]` 大栈缓冲）+ 当前文件最大栈缓冲 64B 安全核实。
- **Security gate**：r5 `ok:true`（`securityReviewRequired:false`）。本包无新公网入口、无新 secrets、无新依赖；资产管线为离线生成脚本（lv_font_conv / node mjs），不进设备信任面。
- **Docs sync**：r5 `ok:true`（`docsSyncRequired:true` + docsOwner=integrator）。判定与逐条理由 = 同目录 `docs-delta.md`（docs/notes 锚点主线程已落 + canonical/spec/契约零改动 + 证据件清单）。**不改 docs/contracts/**、不改 docs/notes（无写权）。
- **Closeout**：本 phase 跑 `node .claude/scripts/closeout-check.mjs`（默认只读，不传 `--update-index`）→ 见 §Closeout。
- **影响面核对（step 11b）**：Pod worktree 内 diff 对索引不可见 → `detect_changes` 如实标注 N/A (worktree)，以 `git diff --numstat -- firmware/ tools/`（14 文件 +838/−53）＋关键共享符号抽查替代：① `kidpal_voice_state_look` 仅 lcd_display.cc 消费（kidpal_voice_states.h 声明、CMakeLists 接线，host_app 同源）；② `kidpal_story_reader_slice` 仅 story_page.c READER 分支消费；③ `KSTORY_READER_LINES_PER_SCREEN` 消费点 = story_page.c + mcp_server.cc（硬编码 4 替换）；④ lcd_display.cc 五态改写只动内容面、layer 底座（KidPalReparentChatToLayer/chat_top[]）零触碰；⑤ 改动只影响 taskpackage crossEnd counterparts 登记的预期符号面。

## 逐 Acceptance 状态映射（RP-A1~A11）

| 项 | 状态 | 承载证据 |
|---|---|---|
| RP-A1 P2-A1 齿修复（方案 B） | **host 绿** | 齿 6/齿 5 定义签名定位修法（tightly-only + REVISION LOG），修后当前树即绿，schedule_p2 7/7；方案 A（render_node 补哨兵）禁令遵守 |
| RP-A2 故事页排版 | **host 绿** | `kidpal_story_reader_slice` 切片单源 + WRAP + `KSTORY_READER_LINES_PER_SCREEN` 单源；story_p3 10/10（F2/F5 新齿在内）+ story_page_stack 3/3 |
| RP-A3 课表页数据链 | **host 绿（机制面）** | S0.2 勘察根因 = 固件缓存时机（`changes/dev/260911-render-pages/intake-survey-s02-s03.md` 证据链），cycle_next 空态友好页实现，schedule_p2 7/7；真机见真实条目 → 归 RP-A5 真机轮核验（pending human） |
| RP-A4 V3 字体体系并包 | **host 绿（资产/齿面）** | S0.1 决策 DECIDED（`font-decision-s01.md`，正文自产 GB2312 全集 6763 字、标题/数字 SPIFFS 非 KAP1、预渲染位图路线废除）；fonts_root 三字体入库 + `gen_fonts.sh` 管线；字体分型闭集齿（v3_restore test 6，fonts_root 独立闭集与 B8 分型）；唯一新增 KAP1 = bubble_tail_24.bin（闭集 47→48 + gen_bubble_tail.mjs）；「猬」抽样渲染可见证据 → RP-A5 真机轮顺带核验（pending human） |
| RP-A5 真机整屏 V3 对照轮 | **PENDING（owner=human）** | human 持板四页 + 五态逐一对照 V3 稿 + 两句检验题（「这是什么」「哪里可爱」）孩子实测 + HWV-A7 顺带；证据落本目录 |
| RP-A6 冻结面零越界 | **host 绿** | contract-lint PASS + `git diff --name-only -- docs/` 空（contracts/spec/spec.md 零触碰）；`*.acceptance.*` 仅 acceptance-author 留痕修订 3+1 文件、REVISION LOG 齐、断言语义不放松；story_page.h 冻结头 additive-only（+14 纯新增声明） |
| RP-A7 host runner 终跑全绿 | **host 绿（零新增回归）** | 124 Tests / 3 Failures——3 红逐条 = 在册他人红（VRT-A2·E2:878 / WSA-A6:830 / WSA-A4:663），在册 P2-A1 已消解；suiteGate 形态在案 |
| RP-A8 P2/P3/P4 页面换皮 | **host 齿绿 / 真机证据 PENDING（owner=human）** | v3_restore test 5 框族齿（frm_ + 网点引用断言）绿；布局由设计稿转写纪律（docs/notes 注记 + tools 管线）；涂鸦观感/倾斜 ≤3 处按红基线裁定「不进脆断言、留真机轮/设计 review」（`red-baseline-freeze.md` §三 test 5 行） |
| RP-A9 五态演出换皮 | **host 齿绿 / 真机证据 PENDING（owner=human）** | v3_restore test 1（五态内容逐字断言「我在听！/说完啦/不等了/打断/知道了/叫醒它」）+ test 2（活侧驱动接线、双分支陷阱布防）+ test 3（姿态/键帽/气泡尾巴资产）绿；`kidpal_voice_states` 纯逻辑冻结件 + layer 底座零触碰；PKG-1 路由语义回归复验 = KEYA/SU 不回归 |
| RP-A10 P4 笔顺页观感还原 | **host 齿绿 / 真机证据 PENDING（owner=human）** | char_stroke.c 改写 + B8 打包策略（笔形 3 态叠加 + 米字格单份背景）遵守；char_stroke 冻结齿复跑绿 |
| RP-A11 新增验收齿 crossEnd 登记 | **DONE** | v3_restore.acceptance.c 新产 6 齿（红基线先行），counterparts 11 符号落 taskpackage crossEnd 字段；路径敏感既有齿（story_p3/ui_art_integration）修订走 acceptance-author 留痕 |

## Simplification Debt

`grep -r "minimal:" firmware/main/display/ firmware/main/test/ tools/asset-pipeline/` 本包新增命中 0 处（mcp_server.cc 原 `minimal:` 注释被本包 RP-A2 单源化替换消除——旧债销账而非新债）。无新抽象债务：`kidpal_voice_states` 是五态内容的单源收敛（PKG-2 迁出聊天 layer 的内容驱动入口），`kidpal_story_reader_slice` 是排版切片单源，均为 plan 预定承载件。本包无 `no-trigger` 简化债务条目；「过期课目过滤 producer 语义优化」为 intake 明示不强制可选项、未做（intake-survey §S0.2，不裁契约、不阻塞）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | final 闸红（留红如案，跨包） | **SMK-A4 final 闸红（不伪造不代修）**：r5 `unit`/`acceptance` 两闸唯一红 = `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（`server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据）。与本包零相关（本包 `git diff -- server/` 零改动；先例 PKG-4 合并夜同一红同名单） | Basis: final r5 fact `ok:false` stdout（`AssertionError: SMK-A4 RED: …screen.md missing`，`test_smoke_e2e.acceptance.py:422`）+ `existing-red-inventory.md` R1（owner=260829-smoke-e2e，出处 red-baseline-freeze §一）；heuristic: `[evidence]` 族红非本包固件面，不越权代采 | 留红如案、提 human 按红证据 ESCALATE 通道裁（红转交 owner=260829-smoke-e2e）。本包不代采不代修 |
| FYI | judgement-call | host runner 基线红（留红如案，跨包） | **host 基线 3 红留案 + P2-A1 已消解**：WSA-A6:830 / WSA-A4:663（owner=260903-device-ws-switch）+ VRT-A2·E2:878（owner=260829-voice-roundtrip，cwd 敏感判读钉明 cwd）——在册他人红，本包不代修；在册第 4 条红 P2-A1:571（本包 RP-A1 修复对象）已由修齿消解出失败清单，host 全量 124/3F 复核确认 | Basis: `red-baseline-freeze.md` §一（human 指示 2026-09-12：不重跑、引在册）+ §七终态追记实测计数；heuristic: 3 红均为证据齿类、本包零相关面 | 留红如案，基线消解走独立批；P2-A1 消解作为 RP-A1 兑现证据留痕 |
| SHOULD | judgement-call | 真机 pending（structural） | **RP-A5 / RP-A8 / RP-A9 / RP-A10 真机证据 pending（owner=human）**：真机整屏 V3 对照轮（四页 + 五态 + 两句检验题 + HWV-A7 顺带 + 「猬」渲染可见 + 课表真实条目）未执行——bootstrap 期设备动作恒 human 持板，本包不出真机动作 | Basis: taskpackage `structural:["RP-A5","RP-A10"]` 判定 + exitCriteria RP-E2/RP-E4 + 先例 PKG-4（DF-A3/A5/A6 closeout 时 pending、human 收口轮裁 PASS 成案） | 不阻塞合并门 diff 复核；真机轮由 human 收口（先例同 PKG-4），结果落本目录 evidence |

**Kind 说明**：三行均 `judgement-call`（无一条是本包 repo diff 引入的 hard-violation；三行均为在册留红/结构性 pending）。无 BLOCKING → 不停合并门；但 final fact `ok:false` 触发红证据 ESCALATE（见 §合并门须知）。

## Five-Axis Review

1. **Correctness（对 taskpackage acceptance + V3 设计稿 + S0 决策记录）**：缺陷轨四条逐条兑现——#9 排版 = READER 切片只画当前屏（`kidpal_story_reader_slice` 指针/偏移不复制、`KSTORY_READER_LINES_PER_SCREEN` 单源、F2 机制齿双判别式实帧断言「屏 0 含首行不含末行、翻页后首行不可见」）；#10 缺字 = 正文 GB2312 全集 6763 字自产子集（「猬猾狸」犭旁整族实测入表，font-decision-s01 实测方法交叉吻合真机实证）；#11 课表 = 缓存时机根因修复（S0.2 三级证据链：引擎日志回合 → schedule.md 数据源 → cycle_next 缓存路径逐字定位）+ 空态友好页；#12 = 方案 B 修齿（定义签名定位，非补哨兵）。还原轨 = 五态内容逐字断言（v3_restore test 1）+ 活侧驱动接线（test 2，双分支陷阱布防）+ 资产闭集（test 3/6）+ 框族（test 5）+ d50 压暗（test 4）。host 齿实测为本包齿 37 齿全绿（6+10+7+11+3），非「跑了没报错」。
2. **Test quality**：新齿全部红基线先行（RP-A11 明示义务）——v3_restore 6 齿接线后首跑全红 + 守卫绿，逐齿红因逐字核实（`red-baseline-freeze.md` §三）；F2 判别式对行数参数不敏感（防实现换参数绕齿）；F5 禁词扫描布防大栈缓冲回归（4096B 栈纪律）；test 2 活侧定位写明「最后一个 SetupUI 定义点之后、死分支区间零命中」防 lcd_display.cc #if 双分支误钉。mutation `mutationRequired=false` 有因（齿以真实渲染帧/资产闭集/结构扫描为断言对象，无 vacuity 自证面，launch 与 acceptance-author 双判一致）。
3. **Contract / boundary safety**：`docs/contracts/**` 与 `docs/spec.md` 零触碰（contract-lint PASS + `git diff --name-only -- docs/` 空）。#9 为「兑现冻结契约既述行为非语义变更」（taskpackage origin 明示）；`*.acceptance.*` 仅 acceptance-author 留痕修订（P2-A1 齿 + U 闭集扩容 + F2/F5 新齿），REVISION LOG 齐、断言语义不放松；story_page.h 冻结头 additive-only；managed_components 零触碰。U 闭集修订走明示通道（唯一新增 KAP1 = bubble_tail_24，压暗复用既有 d50 零新增）。
4. **Operability / security**：无新公网入口/凭据/依赖；五态改写只动内容面（layer 底座与优先级仲裁零触碰，chat_top[] 成员边界外零改动）；七不变式布防清单遵守（零残留五字符串、自建屏恰 1 处、lv_screen_load 实参恒 s_kidpal_scr、CMakeLists 不动既有条目）；字体 SPIFFS 落地 = 改字体不重烧固件（运维面正向）；资产管线脚本离线、不进设备信任面。
5. **Maintainability / simplicity**：单源化三处（`KSTORY_READER_LINES_PER_SCREEN` 行高、`kidpal_voice_state_look` 五态内容、`kidpal_story_reader_slice` 切片），mcp_server 硬编码 4 替换销一处 `minimal:` 旧债；坐标/资产转写管线化（gen_fonts.sh / gen_bubble_tail.mjs，禁手抄纪律）；换皮零触碰动态文本语义与 LVGL 骨架（V3 §07 红线执行纪律）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired` / `s2ExploratoryGoals` ⇒ 本相位**未执行** S2 official exploratory scan，无 runId、无 structured `runResult`、无 `knowledgeSuggestion` 待决项。required-by-risk 侧注记：`riskTier=high` 但 planner 未开该面；本包风险面（资产闭集越界 / 冻结面越界 / 栈纪律回归 / 双分支误钉）均已有机械齿或结构闸承载（v3_restore 6 齿 + ui_art B8 齿 + story_page_stack 3 齿 + contract-lint），非探索式攻击面。本 packet 如实记录「未声明 ⇒ 未执行」，不以 builder debug run 冒充 official evidence。

## UI design-review 证据（bootstrap 期纯文本口径）

本包为 UIUX 重包（换皮 + V3 整屏还原），bootstrap 期证据只入文本、不入 screenshot/video/HTML：

- **Viewport coverage**：设备固定 400×300 1bpp RLCD 单一 viewport，无多断点面；P2/P3/P4 + 语音轮五态 + P4 笔顺页全页面在改（`kidpal_schedule_page_render` / `kidpal_story_page_render` / `kidpal_char_card_build` / 五态落点 / `kidpal_reminder_page_render`）。
- **A11y summary**：嵌入式设备无 WCAG 轴；等价面 = 1bpp 对比度（V3 黑白稿原生满足）+ 字号阶梯落位（V3 阶梯，标题站酷/正文自产/数字 Baloo）+ HWV-A7 字号下限回写顺带核验（归 RP-A5 真机轮）。
- **Design-drift summary（V3 稿 ↔ 实现）**：结构面 drift 由 v3_restore test 5（框族 frm/frm-sm/frm-flat + 三档网点引用）+ test 3（7 姿态资产中五态消费 + bubble_tail_24）+ test 6（字体分型闭集）机判钉住；观感面 drift（倾斜 ≤3 处/屏、涂鸦 ≤2 处/屏四类白名单、印章虚线/蝴蝶结/24px 角片三项 V3 定稿裁定）不进脆断言（red-baseline-freeze §三 test 5 行裁定），归 RP-A5 human 持板对照轮逐屏核——**该项为 design-drift 的收口判据，pending**。
- **External artifact reference**：V3 设计稿 `docs/ui/kidpal-ui-design-v3.html`（§01 检验题 / §06 五态 / §07 红线清单）+ 定稿裁定快照 `tools/asset-pipeline/report/snapshots/`（pose_daily_bow_3x.png 等）。
- **Residual risk**：观感面最终验收未收口（RP-A5 pending human）——host 齿担保结构/内容/资产/接线面，不担保最终观感；不新增 UI gate（复用 code-review-gate 既有轴，本包无 UI BLOCKING finding）。

## Residual Risk

- Known limitation / 留红如案（均非本包引入、owner 已登记）：
  1. **SMK-A4 final 闸红**（existing-red R1，owner=260829-smoke-e2e）：`screen.md` 证据件缺口，本包不代采、不伪造，提 human ESCALATE 裁。
  2. **host 基线 3 红**（WSA-A6:830 / WSA-A4:663 / VRT-A2·E2:878，existing-red R2-R4）：在册既存跨包红，owner 各归原包，本包不代修，基线消解走独立批。
- Pending（owner 恒 human，本包不出真机/部署动作；先例同 PKG-4）：
  - **RP-A5（真机整屏 V3 对照轮，structural）**：四页 + 五态逐一对照 V3 稿 + 故事正文换行完整可见（含「猬」）+ BOOT 翻页逐屏正文 + 课表页真实内容 + 两句检验题孩子实测 + HWV-A7 顺带——human 持板。
  - **RP-A8 / RP-A9 / RP-A10 真机证据**：换皮观感 / 五态演出 / 笔顺页观感的真机证据件——human 持板（RP-A5 同轮收口，结果落本目录）。
- Human decision needed：
  - **红证据 ESCALATE**：final fact r5 `ok:false`（SMK-A4 留红如案），提 human 裁「接受本包合并（红转交 owner=260829-smoke-e2e / 独立批基线消解）或先修 R1 再合并」——由 supervisor-verdict-gate 机械闸 + planner 前置升级闸（`pre-merge-escalation-gate.mjs`）承接，本 integrator 不自判。
  - **risk-accepted 台账**：`taskpackage.decisionCoverage` 九条 decision 的 `landsIn` 全部指向 `acceptance:RP-*` / `exitCriteria:RP-E1`，**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头条目** → 本包无 risk-accepted 待追认项。
- 里程碑面：`deliveryImpact=milestone-affecting` 但无里程碑阶段跳变（M4 UI 重做批次③推进，非新阶段开启）——`.delivery/milestones.jsonl` 本相位不改；`.planning/kidpal-mvp/progress.md` 收口段归主线程/workstream owner（integrator 不代写他人撰写行），如需补记交回 planner。

## Closeout

`node .claude/scripts/closeout-check.mjs`（本 phase 实跑，默认只读、**不传 `--update-index`**）：制品补齐后 `requiredFiles:["docs-delta.md","review-packet.md"]` 两件 blocking 制品在本目录在场；`existing-red-inventory.md` 为非 blocking 但必交（红证据 ESCALATE 豁免件，supervisor 机械闸按形状检查）。
advisories（advisory-only，不阻塞按 SSOT）：① `verdict.json`（verify.mjs script fs-write 投影晋升）/ `verification.json`（integrator 投影）= non-bearing，承重 = r5 fact；② `pod-retro.md` 为 planner 收尾相位产物（非本相位职责，Q5=warn）；③ `pod-events.jsonl` 在场（`.claude/taskpackages/260911-render-pages/pod-events.jsonl`，advisory 满足：human-decision S0.1 字体方案 `0cf82868` + friction over-scan/scope-drift 各 1 条）。
post-cutoff（>= 250725）不产 session-digest / loop-health / loop-metrics / pod-journal。

## Commit 冻结（合并门复核的对象）

- 功能提交 = builder/acceptance-author 已落工作树（固件实现面 + 资产 + 冻结验收件修订 + 管线脚本），积分收拢提交（本次）把 closeout 制品 5 件 + dev-loop ledger（pod-events / verify-runs / r1-r5 facts / input-briefs / taskpackage 活文件 / plan / arch-review）收口。commit 前按 AGENTS.md Hard Rule 用 `git diff --cached --stat` 核对；**`git add -f` 四件 bin**（font_body.bin / font_title_zcool.bin / font_num_baloo.bin / bubble_tail_24.bin——`firmware/.gitignore:16 *.bin` 吞 fonts_root，先例同 U 包 47 枚 spiffs 资产）；**不 stage `.codebase-memory/**`**（机器再生面常驻脏，Pod 分支禁止私带快照 commit，merge-driver 处理）；不 stage 其它包文件。
- 可回滚路径：全部交付物为 feature 分支 `dev/260911-render-pages` 上的本地 commit（**未 push、未动 main**）；回滚 = 分支级 revert（实现面单层 additive 为主 + 新文件删除即还原；冻结验收件修订经 REVISION LOG 逆向可溯）。main 零暴露。
- 合并相位：supervisor 机械闸（`supervisor-verdict-gate.mjs`）裁断 → planner 前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行后 `merge-driver --pod` 本地合并（本包 mergeOrder 21，dependsOn 260911-screen-unification 已合并）。本 integrator 不自行判定通过、不执行 merge、不 push、不动 merge-driver。

## 合并门须知（短清单）

1. 无 BLOCKING finding；三行 FYI/SHOULD 均为留红如案或结构性 pending，无一阻断本包自身面。
2. 承重 fact = **r5**（phase final，scoped 7 闸绿 + unit/acceptance red = SMK-A4 在册 R1，`ok:false`）；supervisor 复核读 `verifier-evidence/rounds/r5.json`。
3. 本包齿 37 齿全绿（v3_restore 6 + story_p3 10 + schedule_p2 7 + ui_art 11 + stack 3），新齿红基线先行在案；host 全量 124/3F 零新增红，在册 P2-A1 已消解。
4. 冻结面（`docs/contracts/**`、`docs/spec.md`、`managed_components/**`）git 零触碰（contract-lint PASS + `git diff --name-only -- docs/` 空）；`*.acceptance.*` 修订仅 acceptance-author 留痕、REVISION LOG 齐。
5. **红证据 ESCALATE**：final fact `ok:false` 触发人类裁决通道（风险面 = SMK-A4 + host 3 红留红如案，owner 均已登记），由 supervisor 机械闸 + planner 前置升级闸承接，riskTier=high 照常升级。
6. human 待办（真机收口轮）：RP-A5 整屏对照 + RP-A8/A9/A10 真机证据（同轮收口）；SMK-A4 留红如案裁定（owner 260829-smoke-e2e）；host 基线 3 红走独立批消解。均不阻塞本包 merge。
