# Review Packet · 261028-m6-k3-p3-story-typography

> integrator 收口（2026-11-02）。test-scope 清单落盘路径：`delivery/taskpackages/261028-m6-k3-p3-story-typography/test-scope-final.txt`。包级 diff ref = `a1e763f7`（pod-started baseRef，pod-events.jsonl 在案）。

## Change Size
- Files changed: 13（代码 5：story_page.c/.h、story_typography.acceptance.c 新件 1332 行、story_page_v2.acceptance.c revision 5、font_assets_runtime.acceptance.c revision 3；接线 1：host_app/main/CMakeLists.txt step-0 六增量；治理制品 7：taskpackage / pod-events / verifier-evidence r1-r3 / verify-runs / acceptance-wiring-patch / 本包 docs-delta + review-packet / current/ 投影）
- Net diff（tracked，vs a1e763f7）：+509 / −264；story_typography.acceptance.c 新件另计 1332 行
- Scope: 实现面 ⊆ changeManifest（story_page.c/.h + 两冻结件修订 + 新冻结件 + CMakeLists 接线 = acceptance-wiring-patch.md 逐字授权）；**mcp_server.cc 零 diff 实证**（常量 4→3 改 story_page.h 后两个消费点 :691/:1885 自动同步，crossEnd 零 producer diff 申报兑现：server/gateway/story.py、front/front_app.py 均零 diff）
- Split recommendation: 不拆——单一显示组件消费改造 + 其验收镜像 + M6 终端真机 owner 集于本包，同包原子交付

## Verification Evidence
- **final verify r3（bearing，scope 圈定）**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file .../test-scope-final.txt --change-id 261028-m6-k3-p3-story-typography --round 3 --invocation-id 8f2c1a4e-5b6d-4e7f-9a0b-3c2d1e0f9a8b` → **observedGateExit=0，fact ok:true**；10 闸逐闸：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译，xiaozhi.bin 0x324db0，app 分区余 9%）/ contract-lint / acceptance-coverage（STY-A1..A7 三冻结件在册）/ mutation-check（7 条全 sensitive）/ docs-sync（false 理由在案）/ security（false）全绿；unit 闸 = scope 交集空诚实绿跳过（清单为验收件族、无 unit 文件，非静默跳过）；acceptance 闸 = oracle-pass（固件 on-target 归 P3T-E4 真机轮）。fact = `verifier-evidence/rounds/r3.json`，`verifier-evidence-check.mjs` 复核 PASS（exit 0）
- **范围圈定**：detect_changes（per-Pod 专属图索引 `C-AI-KidPal-pod-m2`，since a1e763f7，impacted_total=23 全落 firmware/main 测试面、未截断）+ impactedTestsSeed 合成 test-scope-final.txt（10 件 = 本包三冻结件 ∪ blast radius 五件 ∪ seed 回归两件；archive happy_flow 按 seed archive-reference 注记排除）；三分法第 1 档「正常圈定」，无降级
- **Acceptance（host 通道① integrator 收口实测复跑，2026-11-02）**：**265 Tests / 3 Failures / 0 Ignored**，3 红全为 p1_aur_style AB-A1/A2/A4（auxbar 存量族，launch 基线锚在册、与本包无关）；STY-A1..A7 全绿；总 258→265 = 本包七 TEST 编入（接线后）预期增量；义务红名单（冻结件头注预申报：TEST 1 tooth 1/3、TEST 2 tooth 1、TEST 3 tooth 1/2/3 前半、TEST 5 tooth 2、TEST 7 五键 + auxbar×3）到绿名单收敛，零其它新增红、零段错误
- **红基线**：acceptance-author 冻结实跑 + planner 复核复跑一致（258/5 = 存量 3 + 设计态红 2：SPV2-B2、FAS-A8），逐条判读按名单（P2 retro F4 教训延续）
- **Mutation evidence**：7 条变异全 turnedRed（M-STY1 列表标题回 14px / M-STY2 阅读正文回 14px / M-STY3 常量 3→4 / M-STY4 摘折行 / M-STY5 标题回 14px 左对齐 / M-STY6 未变更页态误切 body / M-STY7 mcp 消费点回字面 3）；**M-STY2/M-STY6 首测 MASKED（冻结件夹具自遮，P2 F3 同型）→ acceptance-author revision 6（R12 夹具修 / R13 带顶 y0 150→152）后复测转正**，终态哈希在案（md5 4ca0d0a47ffb40355d2f7ee12fc0ba92 = 实现好态），证据已回填 mutationEvidence
- **Device build**：xiaozhi.bin 0x324db0 bytes（app 分区余 9%），firmware-build 闸真编译绿（r1 全量编译 + r2/r3 增量）
- **Server**：零 diff；395 passed 基线锚（launch-card @ 6ce77123，diff --quiet 复核零漂移），本包未触 server 面
- **Docs sync**：docsSyncRequired=false，零改动声明见 `docs-delta.md`；spec/契约面零漂移检出
- **零变异残留**：grep RT-M-STY / MUT-M-STY / DIAG-TEMP 零命中（firmware 全树）

## Design Review（uiux.designReviewRequired=true，如实申报）
- **状态 = host 半已验、真机半未决**：本包按 V8 两帧（`docs/ui/V8/canvas-p3.png` / `p3-reader.png`）完成 host 侧布局对照——列表行 16px 标题 + 定宽 276 LONG_DOT（右缘 x=316，分离带 [316..319] 零墨、日期列 14px 完整可见 = canvas-p3 行结构）、阅读标题 20px 全宽居中单行 LONG_DOT（墨 max_y≤42 与正文 y44 分离 = p3-reader 帧头区）、正文 16px WRAP 折行不越 x[12..396]（STY-A1/A2/A4/A5 墨迹断言承载）。
- **对照终判挂 P3T-E4 终端真机轮**（P0/P1/P2 已 merged 后一次目击四页并集，human 终判后才宣布 M6 收口）：字号观感、疏密与翻页手感（F-4 真机终判）、长标题真机省略效果，均以真机目击为准；本节不伪装通过。缺 assets 14px 降级真机观测已由 human 2026-10-29 裁豁免（P0 批裁定，launch-card 在案）。
- 仓库视觉资产：`repoVisualArtifactRequired=false`（bootstrap 期不入 screenshot/HTML，只留文本摘要与帧引用）。

## 事故与修复（如实记录）
- 本包无提交事故、无跨包冲突。两处冻结件夹具缺陷（STY-A2 多行带几何自遮、STY-A6 STAMP 带与印章圈相切）经 acceptance-author revision 6 修掉后变异复测转正（MASKED→turnedRed），REVISION LOG 留痕于冻结件文件尾；零负锚削弱。

## 集成执行回报
- 本包无跨包冲突消解，merge-chain 执行节不触发。
- closeout 时点 pod-closed 事件：由 `pod-events-append.mjs` 追加（actor/source = integrator），见 pod-events.jsonl。

## Simplification Debt
| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （diff 内零 `minimal:` 标记） | — | — | no-trigger |

## Findings
| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING / SHOULD） | — | — | 本包收口零 blocking/should finding | — | — |
| NIT | judgement-call | 治理流程 | M-STY2/M-STY6 变异首测 MASKED 由冻结件夹具缺陷自遮——与 P2 retro F3 同型缺陷在连续第二个包复现 | changed-code hunk：story_typography.acceptance.c 夹具带几何（R12 修）/ STAMP 带顶 y0（R13 修）；启发式 tag：冻结件可满足性实测未进冻结轮惯例 | 已由 revision 6 修复闭环；「冻结前实测校准预判窗」惯例建议随主线程 retro 裁决（P2 pod-retro §4.1 已在册，无需本包另立） |

## Five-Axis Review
- **正确性/契约**：story_list / story_text wire 形状零变（§1.3③/§1.3④，mcp_server.cc 零 diff + server 零 diff 双实证）；20px 缺字回退走 P0 [K4] fallback 链（title()->fallback==body() 消费点守卫 STY-A5 承载），不另造回退分支（crossPackageBehaviors 第 1 条）；kidpal_story_reader_slice 屏间分隔符 '\n' 不并入本屏切片（STY-A3 tooth 3 恰 3 自然行/2 换行承载），分屏数与切片不封顶的 notes 边角⑧退役注记不回退。
- **测试判别力**：7/7 变异 turnedRed（含 2 条 MASKED 复测转正）；14px 判别负控严格不等式咬住 M-STY1/M-STY2（两态墨高 13=13 不可判别即红）；include 直读常量使 M-STY3 同时打红 A3+A7；mcp 符号引用守卫（FAS story_mcp_lines_sync ≥2）使 M-STY7 可判别且 must-stay-green 方向有锚。
- **安全**：securityReviewRequired=false；security 闸绿；无输入解析面变化（只动渲染层字号/截断/居中与本地分屏切片粒度，云端载荷与会话生命周期零触碰）。
- **可运维性**：device build 0x324db0（app 分区余 9%，较 P2 0x324d10 增 ~160 bytes，无分区压力）；降级路径继承 P0 统一句柄（缺 assets 恒 14px 兜底，恒非 NULL，不新增 NULL 回落分支）；ksy_label_font 防御性 NULL 回落 14px 单点在案。
- **简洁性**：story_page.c 净 +56/−10 为消费点直改（helper 提取 ksy_label_font + 两处消费块 + 切片 1 行边界修正）；story_page.h 净 +5/−4 为常量 4→3 + 注释重基线；无抽象层新增、无跨层调用、无新依赖、无循环依赖（kidpal_font.h 为 P0 既有 seam）；FAS 八新键由 CMakeLists 六处增量逐字落线（acceptance-wiring-patch.md 对账）。

## Residual Risk
- Known limitation: 16px 态 CJK 实测墨高 13（P0 同源资产；P2 实测 14 为「课/口」字差，本包证据域 [13..16] 已按实测校准）——真机观感仍待 P3T-E4 目击。
- Known limitation: M-STY1 变异轮暴露「16px 态墨高 13 不大于 14px 态 13」的紧判别边界——当前靠严格不等式 + 定宽/省略几何断言联合判别，字号再往 15px 档走时该负控需重新校准。
- Follow-up recommended: **P3T-E4 终端真机轮**（四页并集一次目击 + 阅读翻页手感 F-4 终判 + design review 对照终判）——本包收口后由主线程/human 执行，human 终判后 M6 才翻账。
- Human decision needed: 远程发布（主线程 `release-pr.mjs --publish` 后 human 合 PR）；M6 收口终判归 human。
- decisionCoverage 扫描：`landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目 = **0 条**（9 条 decision 全部落 exitCriteria 或关联包）。

## 回滚路径
- 单包回滚 = `git revert a1e763f7..<HEAD>`（dev/pod-m2 上本包 closeout commit 区间）；实现面仅 story_page.c/.h + 三冻结件 + CMakeLists，revert 后 host 回 258/3 基线锚、设备回 14px 全 label 形态；story_page.h 常量回 4 与 story_page_v2/font_assets_runtime 旧锚同步回滚，无跨包悬挂（mcp_server/server 零 diff，无对端耦合）。

## 遗留项（不阻断本包收口）
1. **P3T-E4 终端真机轮 + human 终判**——M6 收口标准（PROPOSAL §5），收口后主线程/human 执行；M6 里程碑保持 in_delivery。
2. 远程发布——待 human。
3. 存量 p1_aur_style 三红（AB-A1/A2/A4）跨包在册，非本包义务。
4. 冻结件夹具缺陷同型二连（本包 NIT finding）——「冻结前实测校准预判窗」惯例裁决归主线程 retro（P2 pod-retro §4.1 在册）。
