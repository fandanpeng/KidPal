# scope-final — 261022-p3-record-front-polish-fix 影响面圈定终验（scope-file）

> 本文件 = final 相位 verify.mjs `--scope-file` 消费的影响面圈定清单（TaskPackage 目录 sibling，Step 4 范围圈定制品）。**圈定失败须举手，不得默认全量、也不得缩到只剩本包档**——本包圈定走三分法①（per-Pod 专属图索引可用，正常圈定，无降级）。

## 1. 圈定可用性（三分法判定）

- per-Pod 专属图索引 `C-AI-KidPal-pod-m2` **可用**（detect_changes 实测命中）→ **正常圈定**，无需降级 git 抽查路径。
- detect_changes 包内独立实测：base = 包首提交父 `80bc2027`，changed_files **11**（实现面 3：front_app.py / story.py / prompts/guide-round.v1.md + 验收件 1 + taskpackage 投影与 verify 证据 7），seed_symbols **77**，**impacted_total 106（精确，无截断）**——全部落 server 三模块（server/acceptance 55 / server/gateway 18 / server/tests 33），**零 firmware 跨面**（crossEnd 豁免声明与实测一致）。
- 另跑 base=main 共享分支 diff（changed 60，含两并联包）复核：firmware/main impacted 簇全部来自并联固件包（chrome-ghost-fix / diag-log-content），与本包零交集——server 侧 impacted 集在两次 base 下逐符号一致，确认 106 全部由本包三个实现文件传导。

## 2. 影响面对账（detect_changes → 预期缝）

| blast radius 集群 | 代表符号 | 对应实现缝 | 判定 |
|---|---|---|---|
| front_app 引导/收尾族（18 中 14） | _story_auto_finish / _story_finish_on_phrase / _story_finish_session / _organize_and_archive_offloop / _consume_kidpal_control / _push_story_list | [FP-C]① 三常量单源 + _story_timeout_sweep 阈值叠加应答窗 + 应答窗收尾通路 + 素材窗过滤 + 篇幅截断 + 标题兜底 | 预期内 |
| story.py 账本/列表族 | StoryAccumulator._ensure / feed / mark_pending + list_stories 消费面 | [FP-C]② list_stories 同日 seq tiebreak + _SessionState.guide_broadcast_since 锚 | 预期内 |
| server/acceptance 记录会话族（55 符号） | test_record_front_polish（本包验收件）+ test_record_dialogue_guide（RdgStuckTimer 五边界）+ test_record_cancel_lifecycle（Rcl 全族）+ test_m4k1_story_cloud_link（M4K1FinishTimeout / Ledger / StoryListPush / SAOArchiveOrder）+ test_m4k3_record_session_live + test_story_fetch + archive/test_story_p3（list ordering / archive roundtrip） | FP-A6 点名三族 + stuck timer / sweep / seq 账本 / story list push 各 seam 的行为级消费面 | 预期内 |
| server/tests 单测面（33 符号） | test_story_units（StoryListWindowUnits / SeqLedgerUnits / StuckGuideDecisionUnits / PendingStopUnits）+ test_story_cancel_lifecycle_units（AutoFinishLegScope / GuideRoundGuard） | list_stories 排序键、seq 账本、stuck/guide 决策、auto_finish leg scope 的直接单测消费方 | 预期内（unit 层由此承载，见 §3.4） |
| test_261013_qq_schedule_write_closure（hop 1，_manifest_list_item_block） | manifest list-item block helper 共享 seam | 安全余量收录 | 预期内 |

零超出预期符号集的 impact；无跨层调用、无循环依赖、零新增依赖（三常量 + 启发式纯函数均 front_app.py 模块内单源，无新 import 面）；wire 形状零改动（story_list / story_text / kidpal_speak / start_organizing 既有帧形状零 diff，tiebreak 只变同日到达序）与 blast radius 全落 server 侧互证。

## 3. test-scope 清单（final verify 消费）

范围 = 本包 `acceptanceTestsFile` ∪ blast radius 映射测试 ∪ 共享 seam 相关测试 ∪ 安全余量。落定如下：

1. **本包验收件**：`server/acceptance/test_record_front_polish.acceptance.py`（FP-A1..A5 行为级 8 齿；FP-A6 = suiteGate 套件级回归门、FP-A7 = live-manual structural，均不入 runner 通过面）。
2. **冻结回归网（FP-A6 点名三族，floor）**：
   - `server/acceptance/test_record_dialogue_guide.acceptance.py`（stuck timer 五边界 + sweep guide round——本包动了 _story_timeout_sweep 阈值叠加与第 3 轮引导守卫，必须全量在跑）；
   - `server/acceptance/test_record_cancel_lifecycle.acceptance.py`（cancel/teardown 生命周期 seam——_story_finish_session 消费方）；
   - `server/acceptance/test_m4k1_story_cloud_link.acceptance.py`（seq 账本 + story list push + SAO archive order——list_stories tiebreak 改动的承载族）。
3. **blast radius 映射测试**（detect_changes 集群 → 测试件）：
   - `server/acceptance/test_m4k3_record_session_live.acceptance.py`（story list push gating——_push_story_list 消费面）；
   - `server/acceptance/test_story_fetch.py`（story fetch 消费侧——archive/seq 语义不受 stamp/fetch 扰动的负锚面）；
   - `server/acceptance/archive/test_story_p3.acceptance.py`（list ordering / item shape / archive roundtrip——list_stories 排序语义先存断言，安全余量）；
   - `server/acceptance/test_261013_qq_schedule_write_closure.acceptance.py`（hop 1 共享 list-item block helper seam，安全余量）。
4. **unit 层承载（blast radius 映射单测）**：planner 裁定 = 行为级 8 齿即承载 requiredLayers 的 unit（窗口分支 / 缺 seq 回退 / 截断边界 / 兜底判定全为行为断言，Lite 口径不为边际价值扩 changeManifest 至 server/tests/**）；本节把 detect_changes 映射到的直接单测消费方**圈入 scope 真跑**，unit 闸由 blast radius 而非 manifest 扩面承载——比裁定更严，非矛盾：
   - `server/tests/test_story_units.py`（StoryListWindowUnits 排序窗 / SeqLedgerUnits seq 账本 / StuckGuideDecisionUnits stuck+guide 决策 / PendingStopUnits pending 边界）；
   - `server/tests/test_story_cancel_lifecycle_units.py`（AutoFinishLegScopeUnits leg scope / GuideRoundGuardUnits 引导守卫）。
5. **安全余量**：FP-A6 套件级回归门（server 全量 246 = 238 基线 + 8 新齿，零新增红）在 r2/r3 轮已全量实跑留痕；final 相位 acceptance/unit 闸按本 scope 清单求交真跑（非全量——全量回归 = 人工触发位，260928 批 T10），suiteGate 语义由既有全量证据 + scope 交集真跑共同承载。
6. **设备编译闸**：firmware-build final 相位恒跑（lite 包 scoped 后置 ≠ 跳过）；本包零 firmware diff，固件编译 = 分支健康锚（分支含两并联固件包改动，一编双证）。

## 4. final bearing verification 执行位

- 命令形（scope-file 缺失 = 红闸，本文件即清单；round 序数跨相位全局单调——scoped r1-r3 已落账，final = round 4）：
  `node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261022-p3-record-front-polish-fix/scope-final.md --change-id 261022-p3-record-front-polish-fix --round 4 --invocation-id 3b056af4-5b44-464a-b43d-b75f85fd7156`
- 随后 `node .claude/scripts/verifier-evidence-check.mjs --change-id 261022-p3-record-front-polish-fix --phase final --round 4 --invocation-id 3b056af4-5b44-464a-b43d-b75f85fd7156`；checker PASS + fact `ok:true` 方进 closeout。
- **test-scope 清单落盘路径**（一行，final verifier fact / review-packet results 元数据必含）：`delivery/taskpackages/261022-p3-record-front-polish-fix/scope-final.md`。

## 5. 机读清单（acceptance-run-check / unit-run-check 的 scopeFilter 逐行消费；一行一条 repo-relative 路径，# 注释；散文节仅供人读）

server/acceptance/test_record_front_polish.acceptance.py
server/acceptance/test_record_dialogue_guide.acceptance.py
server/acceptance/test_record_cancel_lifecycle.acceptance.py
server/acceptance/test_m4k1_story_cloud_link.acceptance.py
server/acceptance/test_m4k3_record_session_live.acceptance.py
server/acceptance/test_story_fetch.py
server/acceptance/archive/test_story_p3.acceptance.py
server/acceptance/test_261013_qq_schedule_write_closure.acceptance.py
server/tests/test_story_units.py
server/tests/test_story_cancel_lifecycle_units.py
