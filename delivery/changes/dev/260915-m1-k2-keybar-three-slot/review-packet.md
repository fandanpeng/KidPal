# Review Packet · 260915-m1-k2-keybar-three-slot（键帽条三槽）

> integrator 收口，面向 supervisor 合并门。verifier final round fact = delivery/taskpackages/260915-m1-k2-keybar-three-slot/verify-runs.jsonl 第 8 行（2026-09-16T09:11:38Z，9 闸 8 绿 1 红）；`delivery/taskpackages/current/verdict.json` = non-bearing compatibility projection（本包 unit 闸红判读见下）。Pod worktree 常驻分支 dev/260915-m1-k1a-key-layer-dispatch（树池复用，K1a/N1b 同支）。

## Change Size

- Files changed: 22（git diff --cached --stat 终值见「commit 交付」：firmware 7 件 + delivery 治理面 12 件 + closeout 制品 4 件 + .claude/skills 新件 1 件 + taskpackages/current 指针面，另含 verify-runs.jsonl）
- Net diff: firmware 代码面 +388/-143（含治理面；固件实现本体 = keybar.{h,c} 新增 140 行 + lcd_display.cc 三槽改造 +147 + kidpal_render_node.cc 桥 +21 + 两个 CMakeLists 接线 +16）；冻结验收件 keybar_three_slot.acceptance.c 新增 935 行（冻结即基线，未动）
- Scope: firmware display 键帽条 chrome 改造——三槽（左 KEY 提示 · 中页点 · 右 BOOT 提示）+ 页点机制（layer_top 同源显隐）+ 随态文案（kidpal_voice_states 冻结闭集单源）；server/ 零触碰、docs/contracts/** 零触碰
- Split recommendation: 不拆——单包单面，mergeOrder 23，dependsOn k1a（已 merged）

## Verification Evidence

- Structural gates: firmware-build 真编译绿（xiaozhi.bin 0x2c8340 / app 分区 0x370000，19% 余量）；contract-lint 绿；acceptance-coverage 绿（KB-A1~A6 全覆盖，structural = [KB-A5]）
- Acceptance/unit: host runner **18 Tests / 0 Failures / 0 Ignored**（13 基线 + 5 新齿，相对冻结基线 red-baseline-freeze.md 13/0 零新增红，KB-A6 达标）；final 相位 acceptance 闸绿（oracle-pass，firmwareAcceptanceFiles=17）
- **Final 相位红判读（unit 闸 1 红）**：红项全部为 `server/acceptance/archive/` 归档件收集 artifact——sweep 77b6b0c 后归档件引用已退役 server 面产物（front_app.py / engine-patch / display_schema.py / v1 prompt）被 verify.mjs final unit 闸 pytest 递归收集报红；本包 server/ 零触碰（git status 实证）。**不修、不代修、不伪造**；豁免件 = [existing-red-inventory.md](existing-red-inventory.md)（合并门剩余红证据 ESCALATE 豁免即该清单），口径缺口登记 ARQ 交独立治理批（挂账 d）
- KB-A5 真机确认轮：human 四项全 PASS（2026-09-16），全过程 = [device-round-record.md](device-round-record.md)
- Security gate: 绿（securityReviewRequired=false）
- Mutation check: 6 变异 / 5 acceptanceId 全 sensitive（见下）
- Docs sync: 绿（docsSyncRequired=false——三槽规格权威 = requirements §2.2 + brand-spec + 设计稿 v4 §03 comp-keybar，均在文冻结；实现注记随代码注释与 closeout 制品承载）
- 影响面核对（worktree 抽查）：git diff --stat 落在预期闭集；trace_path 抽查 `kidpal_lcd_keybar_sync` 入度 = 3（rn_keybar_sync_locked ← rn_render_home_locked / rn_render_kidpal_page_locked），与设计接线一致、无计划外调用方；graph 项目 C-AI-KidPal-pod-m1-k1a-key-layer-dispatch 随树增量刷新

### Mutation 摘要（taskpackage mutationEvidence.perAcceptance）

| acceptanceId | 变异 | 结果 |
|---|---|---|
| KB-A1 | M-CAPLABEL-RESURRECT（lcd_display.cc:382 复活 g_kidpal_keycap_label） | failed as expected（tooth4 @ :733） |
| KB-A2 | M-DOTS-VIS-ALWAYS（keybar.c:26 overlay 分支恒真） | failed as expected（:491） |
| KB-A3 | M-SLOTS-P1-DRIFT（keybar.c:44 左槽 说话→上页）+ M-SLOTS-OVERLAY-LEFT-FIXED（keybar.c:67 左槽恒「固定」） | failed as expected（:529 / :574） |
| KB-A4 | M-RENDER-OVERLAY-GATE-ZERO（lcd_display.cc:1061+1051 front_is_overlay 恒 0） | failed as expected（:753） |
| KB-A6 | M-CMAKE-ORDER-SWAP（host CMakeLists 注册序移 smoke 之后） | failed as expected（:839；smoke 边界齿未级联红，如实记录） |

## 验收对照表（KB-A1~A6 / KB-E1~E3）

| 条目 | 状态 | 证据 |
|---|---|---|
| KB-A1 三槽结构 | PASS | host 齿 test4（keybar_three_slot.acceptance.c:733 区）+ 真机第 3 轮 human PASS（device-round-record.md） |
| KB-A2 页点机制 | PASS | host 齿 test1（:491 区，4 点 deviation 对账锚）+ 真机 K2DIAG `dots vis=1/0` 摘录 + human PASS |
| KB-A3 随态文案闭集 | PASS | host 齿 test2（:529/:574 区）+ K2DIAG `slots rc=0 left=说完啦/打断 right=结束` 摘录 + human PASS |
| KB-A4 layer_top 同源 | PASS | host 齿 test4 tooth2-4/6（:753 区，零 front_is_overlay 红线）+ mutation M-RENDER-OVERLAY-GATE-ZERO sensitive |
| KB-A5 真机确认轮（structural） | PASS | **human 四项全 PASS（2026-09-16）** = 最终裁定；device-round-record.md 全程 |
| KB-A6 host 终跑 | PASS | 18/0 vs 冻结基线 13/0 零新增红；red-baseline-freeze.md 五要素锚 |
| KB-E1 范围纪律 | PASS | 页点 4 点 deviation 已登记（launchPreconditions + decisionCoverage，human 2026-09-16 拍板）；spec 面零修订（git status 实证 docs/ 零触碰） |
| KB-E2 闭集零偏移 | PASS | keybar.{h,c} 无键帽文案字面（值域只经 kidpal_voice_states look().keycap 单源）；齿 test3 行为复核 |
| KB-E3 零裁决权 | PASS | keybar.{h,c} 无任何 layer push/pop；显隐只读 overlay_top 入参（= kidpal_render_node_front_is_overlay，键路由同源） |

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/lcd_display.cc（键帽 14px 全量字体） | 全量 font_puhui_14_1 顶替 30px 稀疏子集 | 专属子集字库（键帽字符集 + 30px） | M2/后续轮字库批（挂账 b，no-trigger 本包） |
| firmware/main/display/keybar.c（P4 撤页过渡兜底槽） | P4 槽只钉存在性不钉值 | P4 值钉定须 human 裁定（ADV-1） | M2 撤 P4 包 4→3 收回时一并了结（KB-E1） |

（仓库无 `minimal:` 标记惯例命中；上两行来自验收ADV与真机 tradeoff 挂账，均 no-trigger 本包。）

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| 无 BLOCKING | — | — | — | — | — |
| SHOULD | judgement-call | governance | verify.mjs final unit 闸 pytest 递归收集扫入 server/acceptance/archive/ 归档件，对已退役 server 面产物报 6 失败块——量测口径缺口（本包红判读的根因） | changed-code 无涉（本包 server 零触碰）；heuristic：收集面 artifact 判读（VRT-A2 先例同根）+ existing-red-inventory.md R1 | 登记治理 ARQ（unit 闸 / 基线量测命令对 archive/ 排除口径）；owner = 独立治理批，不塞本包 |
| NIT | judgement-call | firmware | 键帽 14px 字号 vs 设计稿 30px 姿态的视觉 tradeoff（30px 全量字体超 ota 分区 gate） | changed-code hunk（lcd_display.cc LV_FONT_DECLARE 处根因注释）+ heuristic：`shrink`（资源约束下的降级交付） | 专属子集字库归后续轮字库批（挂账 b） |
| NIT | judgement-call | firmware | BOOT 翻页序 P1→P4→P2→P3 非直觉序（KP_PAGE_* 枚举序 + cycle_next 线性遍历） | changed-code 面在 k1a（kidpal_render_node.cc 页枚举），非本包 hunk；heuristic：deletion test 判 owner | owner = 260915-m1-k1a-key-layer-dispatch 复核（挂账 a，KB-E3 边界本包未修） |

## Five-Axis Review

1. **Correctness**：三槽机制值全部出自 keybar 纯逻辑单源（page/overlay/scene 入参 → 槽位值），设备侧零重算；页点显隐经 `kidpal_render_node_front_is_overlay` 与键路由同源、不立第二真相源；KP_PAGE_* → PAGE_P1..P4 语义页映射集中在 kidpal_keybar_sync_locked 一处 switch（default 兜底回 P1，越界防御在 keybar 内 -1）。host 5 新齿 + mutation 6/6 sensitive + 真机四项 PASS 背书。
2. **Test quality**：冻结验收件 5 齿覆盖决策表（页点显隐/左右槽值/结构替换/同源红线/注册序守卫）+ MUST-STAY-GREEN 锚（PAGE_COUNT=4 deviation 对账、canary）；weak 哨兵 KKB_UNIMPLEMENTED 保证未接线即红；mutation 全 sensitive 且红齿落点逐条登记。
3. **Contract/boundary safety**：docs/contracts/** 零触碰；kidpal_voice_states 冻结闭集零偏移（模块头显式纪律「不出现键帽文案字面」+ 齿复核）；spec 面（requirements/architecture）零修订。
4. **Operability/security**：sync 内部递归取 lvgl 锁、self 守卫 + 锁失败静默返回零副作用；boot 期 layer 栈空路径有首刷兜底；无新依赖、无 secret/网络面。固件构建 gate 资源余量 19%。
5. **Maintainability/simplicity**：keybar.{h,c} 140 行纯 C 无芯片依赖、host/device 两侧同源（照 kidpal_voice_states.c 先例）；deletion test：删掉 keybar 模块则页点/槽位裁决复杂度泄漏回 lcd_display.cc 多调用点 = 深模组信号（留）；无重复实现、无过早抽象。

## Residual Risk

- Known limitation: 键帽 14px vs 设计 30px 字号 tradeoff（30px 全量字体超 ota 分区；专属子集字库归后续轮）；页点 4 点 vs spec 3 点 deviation（M2 撤 P4 收回，human 已拍板）；P4 页槽值未钉（spec 无权威行，human 裁定后 tighten-only）。
- Follow-up recommended: ① 挂账 a——BOOT 翻页序 owner = k1a 复核；② 挂账 b——30px 专属子集字库批；③ 挂账 c——真机轮一次计划外重启（未复现，挂观察）；④ 挂账 d——治理 ARQ（unit 闸归档收集口径缺口）。
- Human decision needed: 无新增——四项挂账均有 owner 或已登记，不阻断合并门。

## 回滚路径

- 分支 = dev/260915-m1-k1a-key-layer-dispatch（Pod worktree 常驻分支）；本包 commit = 单 commit，revert 该 commit 即整体回滚（keybar 新件为新增文件，revert 无残留；lcd_display.cc 三槽段随 revert 回单行文案形态）。
- 设备侧回滚 = 重烧上一稳定固件（烧录纪律见 .claude/skills/kidpal-flash-device/SKILL.md，app 偏移 0x20000）。
- 依赖序：mergeOrder 23，dependsOn k1a 已 merged；revert 本包不影响 k1a 制品。

## 待 supervisor 裁断

verifier final fact 8 绿 1 红（unit 红 = existing-red-inventory.md 豁免件）；host 18/0 + mutation 全 sensitive + KB-A5 human 四项 PASS + docs-sync/security 绿 + 五轴 review 无 BLOCKING。integrator 判定：可进合并门；unit 闸红豁免与 SHOULD 级治理 ARQ 提请 supervisor 一并裁断。
