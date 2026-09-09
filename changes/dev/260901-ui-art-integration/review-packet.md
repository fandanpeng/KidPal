# Review Packet · 260901-ui-art-integration（美术资产集成，包 U）

> integrator closeout · 2026-09-09 · 分支 `dev/260901-ui-art-integration`（worktree，commit 冻结、不 push 不 merge）
> **状态：closeout 未完，两项 pending（详见 §9）——本 packet 先行汇总全部证据，supervisor 合并门暂不 ready。**
> ① docs-sync 锚点 append 被 enforce-scope 机判拦截（integrator/planner 均无 `docs/notes/**` 写权），锚点文本已备好待主线程落盘（`docs-delta.md` §2）；② final bearing fact r1 = ok:false（红集 = 上述 docs-sync + 8 条上游既有 server pytest 红，非本包引入，判读见 §2/§3）；锚点落盘后须复跑 final（round 2）取得 ok:true 承重 fact 再进合并门。

## 1. 实现面清单（Change Size）

- 16 modified + 12 untracked，modified 面 +475/-382 行；untracked = art_assets 模块 2 文件 + 冻结验收 1 文件 + SPIFFS 落地面 47 枚 .bin + 转换脚本 1 枚 + taskpackage 面。
- Scope：`firmware/**` + `tools/asset-pipeline/gen_art_spiffs.mjs` + `.claude/taskpackages/**`；`docs/contracts/**` 与 `server/**` 零触碰（diff 无一文件）。
- Split recommendation：单包不可拆（资产层/落地面/换皮标记/回归齿同一冻结契约承载）。

### 固件侧（资产层 + 落地面 + 接线）

- `firmware/main/display/art_assets.h/.c`（新增）：冻结离线契约 9 函数（mount/load/pixel/glyph/glyph_advance + render_asset/draw_nine_slice/draw_dots/draw_time），C linkage 零芯片依赖、host 同源；KAP1 格式（magic 0x3150414B / format 1BPP|RGB565 / MSB-first / 阈值化 {0x0000,0xFFFF} 语义）+ 防御门（坏 magic/截断/中间灰/NULL）；weak 哨兵 -2 模式照 HP1 先例。
- `firmware/main/assets/spiffs_root/`（新增）：47 枚 KAP1 .bin 闭集（吉祥物 7 姿态 / 14 枚图标族 / 手绘框族 3 枚 / 网点 d50/d25/d12 / imgfont 11 枚 / stamp/thermo/time 等），闭集白名单 = 验收 `artt_expected[]` 表；`firmware/.gitignore` 补 `!main/assets/spiffs_root/*.bin` 白名单豁免（防全联通配 `*.bin` 吞掉入库件）。
- `tools/asset-pipeline/gen_art_spiffs.mjs`（新增）：单一母版（`tools/simulator/LvglWindowsSimulator/assets/assets.c`，V3 定稿重出图）→ KAP1 1bpp 转换器，非阈值化中间值拒绝静默。
- 接线：`firmware/main/CMakeLists.txt`（+display/art_assets.c；assets 分区 SPIFFS 镜像接线带互斥守卫——仅 `CONFIG_FLASH_DEFAULT_ASSETS` 与 `CONFIG_FLASH_CUSTOM_ASSETS` 均未启用时才接管，默认 sdkconfig `CONFIG_FLASH_DEFAULT_ASSETS=y` ⇒ **守卫默认休眠、SPIFFS 真机烧录殿后 9b**，并存冲突已上报 planner 未静默双写）；`firmware/main/test/host_app/main/CMakeLists.txt`（HOST_ACCEPTANCE_SRCS + 验收 + art_assets.c，**插在 smoke 之前、smoke 保持列表末位** = 包 0 boundary 齿注册序守卫）；`host_app/main/host_infra_seam.c` additive `art_render_blit_frame()`（资产组合整帧入可探针缝，不动包 0 冻结断言）。

### 换皮标记（ART-A6）

- 六页渲染面 additive 各加一行 `#include "art_assets.h"`：`view_router.c`（mount 接线点）、`char_stroke.c`、`home_p1.c`、`schedule_page.c`、`reminder_page.c`、`story_page.c`——即冻结契约 BUILDER DUTY 6 的「include 或 kidpal_art_* 调用」结构消费标记；页面其余代码零改动（ART-E3 不 supersede）。

### 验收/证据面

- `firmware/main/test/ui_art_integration.acceptance.c`（新增）：11 TEST_CASE 冻结验收（覆盖映射见文件头：A2 structural/behavioral/render → test 1/2/6；A3 → 5/9；A4 → 7/8；A5 landing → 3/4；A6 wiring → 10；A7 regression → 11；E2/E3/E4 留根与前置声明在头注）。
- `firmware/main/test/acceptance/red-baseline/red-baseline-summary.md` U 包段 append（84 行，上文 VRT/HP1 段零改动）。
- taskpackage：`mutationEvidence` 6 条已回填（全 turnedRed + sensitive）；`redBaselineResult` 含红形态勘误（见 §3）。

## 2. Verification Evidence

- **final bearing fact r1**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --change-id 260901-ui-art-integration --round 1 --invocation-id 816abf74-019f-47c0-828c-10c4ad2a5b10` → `.claude/taskpackages/260901-ui-art-integration/verifier-evidence/rounds/r1.json`，**ok:false / observedGateExit 1**。逐闸：taskpackage-required / firmware-build / contract-lint / acceptance-coverage / mutation-check / security 六闸 GREEN（firmware-build 本机真编译非降级：venv idf5.5 真跑 ninja，`xiaozhi.bin 0x2b8b30`，app 分区余 31%——**设备 build exit 0 证据在闸内**）；docs-sync RED（§9 pending ①）；unit / acceptance RED（timing=full 全档真跑，8 failed / 156+119 passed / 41 deselected，8 红全在上游文件，判读见 §3）。
- **host+SDL 冻结验收 11/11**（builder/verifier 实测，taskpackage mutationEvidence 6 条 notes 逐条载明 pristine 基线）：官方 runner 全档 **97 Tests / 2 Failures**——本包 11 TEST_CASE 全 PASS，2 红 = 上游 WSA-A6/A4 既有红非本包；每条变异还原后 pristine 复测均回 97/2。
- **红基线**：`red-baseline-summary.md` U 包段——**11 Tests 10 Failures，ELF 退出码 = 10**（stub runner 实跑，BUILD_CLEAN，红全在 weak 哨兵/装配/落地面缺失真实行为；1 条 A7 回归齿 stub 下按设计 skip）。上游基线 = server pytest 8 failed/119 passed、host ELF 86/3（VRT-A2/WSA-A6/WSA-A4），判读相对值。
- **Mutation check**：6/6 功能齿（A2~A7 各一）turnedRed + sensitive——invert-magic / advance 48→40 / 9-slice 钉 C 行 / 资产改名 / include 字面量全删 / 路由码劫持，还原后 sha256/行为与基线一致；两处首探不敏感（注释 include 残留字面量）已诚实注记。structural 齿 ART-A1 按 TaskPackage kind=structural 免 mutation。
- **Docs sync**：见 §9 pending ①（其余 docRefs PROPOSAL/spec 弱 token 已过）。
- verifier-evidence-check：对 r1（ok:false）未跑 PASS 判定——待 r2 ok:true 后按职责跑。

## 3. 上游 8 红判读（非本包引入的证据）

final 相位 unit/acceptance 两闸全档真跑（新闸真值口径下 server pytest 不再整档 skip），8 failed 逐条 = `test_smoke_e2e.acceptance.py` 7 条（SMK-A1/A2/A3×2/A4/E1/E2，设备 HTTP shim 协议/mp3 解码/证据落盘腿）+ `test_voice_roundtrip_b.acceptance.py` VRT-A4 1 条（遵循度复验记录 failure-disposition）。**本包 diff 零触 `server/**`**，8 红全部落在本包未触及文件上；红数与 launch 申报上游基线「server pytest 8 failed/119 passed」同数（119→156 passed 为并行包新增收集，零新增红）。已知红无机械基线源的问题在册 = `ARQ-260908-known-red-count-no-mechanical-source`；是否以此为由放行本包 final 判读 = **planner 裁定点**（§9）。

## 4. Five-Axis Review（gate 全绿之外完整执行）

- **实现面逐文件 diff 核对**：16 modified 全部落在 §1 清单内；六页改动均为单行 include additive，无布局/逻辑重排；`host_infra_seam.c` additive 强符号不触包 0 冻结断言。
- **契约边界**：契约面零触碰（diff 无 `docs/contracts/**`）；验收文件内契约 canary 在世；display 统一响应块 E1 未动。
- **类型/规范**：art_assets 纯 C 零芯片依赖（模块纯净性 guard 齿扫 mcp/websocket/esp_spiffs 等词，GREEN）。
- **安全**：security 闸 GREEN；资产加载防御门（坏 magic/截断/越界/中间灰）齐全，无路径穿越面（root 由 mount 固定、load 相对路径）。
- **可运维性/回滚**：回滚 = feature 分支整体弃合并即可，main 无污染；SPIFFS 镜像接线带互斥守卫、默认休眠，不影响既有烧录链。

## 5. Simplification Debt（`minimal:` 标记台账）

- 换皮接线为结构消费标记（include）而页面逐资产渲染调用未接入：`no-trigger`——冻结契约 BUILDER DUTY 6 即「include 或调用」二选一，页面逐像素真换皮随 9b 真机观感通道（ART-E2）统一落，注册于 taskpackage decisionCoverage（PROPOSAL-§2-包U → acceptance:ART-A2）。
- 见 §7 NIT-2（spiffs_root 47 枚二进制入库的仓库体积取舍）：`no-trigger`。

## 6. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING / SHOULD） | — | — | — | — | — |

NIT-1：`firmware/main/test/host_app/main/host_infra_seam.c` 文件末尾缺换行符（append 块后无 EOF newline）。judgement-call：diff hunk（新增块末行）+ 风格一致性；不影响编译与任何齿。
NIT-2：`firmware/main/assets/spiffs_root/` 47 枚二进制（约数百 KB 量级）入 git 库，仓库体积永久增量。judgement-call：验收 test 3/4 运行时读盘对账 + 设备 SPIFFS 烧录源要求其入库（`firmware/.gitignore` 白名单注释自证此为有意决策），非疏忽。
NIT-3：`view_router.c` 作为 mount 初始化接线点仅带 include 标记，`kidpal_art_mount` 运行时调用点尚不存在（验收对 mount 的运行时消费承载在验收 harness 内）。judgement-call：changed-code hunk + 验收头 BUILDER DUTY 6 的「mount 初始化接线点」措辞 vs 齿判据「include 或调用」二选一的宽口径——设备侧首次真取资（9b/后续包）前须补 mount 初始化，否则运行时资产加载无处锚定。

结构闸（verify final r1 / docs-sync / firmware-build）结果见 §2，不复述为 finding。

## 7. Residual Risk（decisionCoverage dropped + risk-accepted）

`taskpackage.json.decisionCoverage` 共 4 条，全部 `landsIn` 落 acceptance/exitCriteria，**`landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目为 0 条**——无 human risk-accepted 残留需列。

Known limitation / Follow-up（留根，非 dropped 项）：

1. ART-E2 殿后 9b：SPIFFS 真机文件源路径、1bpp 真机观感、16px 以下 CJK 真机可读性（HWV-A7，ARQ-260831-small-cjk-1bpp-readability）、前光确认——host+SDL 通道不覆盖。
2. ART-E4 前置已满足：V3 定稿（human 三观感裁定）+ 母版指针切 v3 + 重出图 47 枚机判绿；test 4 溯源齿为其在世守卫。注意 `tools/asset-pipeline/verify_art_assets.mjs` 的 HTML/ASSETS 常量为绝对路径指主 worktree（C:/AI/KidPal/...），跨 worktree 复跑机判需注意路径语义（red-baseline-summary U 段脚注）。
3. assets 分区互斥守卫默认休眠：SPIFFS 真机接管与既有 `CONFIG_FLASH_DEFAULT_ASSETS` 烧录链的切换归 9b（planner 已知并存冲突，未静默双写）。

## 8. Human decision needed

- §3 判读：8 条上游 server pytest 红是否按 launch 申报的「相对基线」口径放行本包 final（在册 ARQ 已指出 known-red 无机械源）。
- §9 pending ① 的 docs/notes 锚点 append 须由有写权方（主线程）执行。

## 9. Pending（closeout 未完清单）

1. **docs-sync 锚点 append**（唯一本包闸红）：目标 = `docs/notes/validation-batch-conclusion.md` 末尾 append「§8 美术资产集成（U 包 260901-ui-art-integration）验收锚点」一节，逐字文本备好在 `docs-delta.md` §2；integrator Edit 被 enforce-scope 拦（角色写边界不含 docs/notes，planner 同样 deny docs/**，未绕过机判），照 red-baseline 归位注记先例交主线程原样落盘。落盘后 `node .claude/scripts/docs-sync-check.mjs` 应转绿（gate 判据 = docRef 文件弱 token 命中 ART-A* id）。
2. **final 复跑（round 2）**：锚点落盘后 `verify.mjs --evidence-mode=round --phase=final --round 2 --invocation-id <新 uuid>`；unit/acceptance 8 上游红是否阻断 ok:true 按 §8 human/planner 裁定执行；绿后跑 `verifier-evidence-check.mjs --phase final --round 2` + `closeout-check.mjs`，并把 verdict.json/verification.json 投影与 r2 fact 晋升本目录。
3. commit 已落（见 §10）；锚点落盘与 r2 后的 docs 增量由后续 commit 补冻结。
