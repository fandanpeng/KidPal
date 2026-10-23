# Review Packet · 261020-p3-rec-frame-visual

- 日期：2026-10-20 · 作者：integrator（dev-loop 收口相位）
- changeId：261020-p3-rec-frame-visual · rigor=nano · riskTier=low · sourceType=human-ad-hoc
- 行为权威 = reviewed plan（`plan.md` 意图定义五件，human 触点① 2026-10-20 确认）
- 预期合并门：nano/low → 常规合并链（supervisor 复核 + merge-driver；本包到真机目检门即停等 `awaiting-human:real-device`，human 终判后再走合并）

## Change Size

- Files changed：`firmware/main/display/story_page.c` 单文件，+23 / −10 行（`git diff --numstat`）——全部在 changeManifest（单条目）内，NRF-X2 过。
- Scope：`kidpal_story_page_render` 内 case KSTORY_PAGE_RECORDING 分支的位式常量与绘制坐标 + 分支头注释；签名与函数本体零触、server 零触、其它页态分支零触。
- Split recommendation：不拆（nano 单意图包）。

## 实现内容摘要（builder 交付，integrator 实地核对 diff）

1. **「在记」一行右移合并**：REC 闪点 @(180,18)→@(200,18)、「REC」文案 @(198,17)→@(218,17)、大时长由第二行 @(180,38) 并入同行 @(254,17)——起点 200 = 帧 B 引导气泡右缘 192 + 8px 安全距，消除与左侧对话气泡的视觉重叠；元素集零变化。
2. **声波缩绘挪猫下方**：rec_wave 资产 206x34 由右侧中部 @(180,94) 改缩绘 212/256（≈0.83，`lv_image_set_scale` + `lv_image_set_pivot(0,0)` 左上锚）落位 @(8,191)——x=8..179（猫中线 x=93 居中）/ y=191..219，不侵入右回显列 x≥180；程序化兜底跳动条（资产缺失分支）同位式挪移（x=32..154、中心 y=205 = 缩绘后声波中线）。
3. **分支头注释同步改写**：位式注释承载改动依据（安全距、缩绘落位、兜底条位式），旧 @(180,94)/两行时长表述退役。

## Verification Evidence

### verifier 轮证据（事实件 = `verify-runs.jsonl` 6 条；verdict.json 投影同步）

| 轮 | 相位 | 结果 | 备注 |
|---|---|---|---|
| r1 | scoped | 红 | `taskpackage-required`：crossEnd 骨架缺位——planner 修 taskpackage 元数据闭环，零实现红。 |
| r2–r3 | scoped | 红 | `mutation-check`：mutationRequired=false 缺 mutationSkipReason——planner 补字段闭环，零实现红。 |
| r4 | scoped | 红 | 同 r2（重复窗口），r4 内补齐后该轮仍带旧判——r5 复核全绿。 |
| r5 | scoped | **全绿** | 8/8 闸 PASS（firmware-build 按 nano defer 自报 executor-skipped，非跳过——final 兜底）。 |
| r6 | **final** | **全绿** | **10/10 闸 PASS、零 notExecuted**：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译 54.7s，defer 兜底兑现）/ contract-lint / acceptance-coverage / mutation-check（skip 有据）/ docs-sync / security / unit / acceptance。ts=2026-10-20T14:40:34Z。 |

- **test-scope 清单落盘路径**：`delivery/taskpackages/261020-p3-rec-frame-visual/scope-final.md`（final 相位消费，verify.mjs --scope-file 传入；合成来源 = detect_changes + impactedTestsSeed + 安全余量，留根锚）。
- **unit 闸**（timing=full，scope 求交）：server pytest 真跑 **333 passed / 4 deselected（live）零红**；固件面结构判 oracle-pass。
- **acceptance 闸**（timing=full，scope 求交）：server acceptance pytest 真跑 **229 passed / 4 deselected 零红**（与基线锚 229P0F 同值）；固件 24 件冻结验收文件 runner 证据在册（on-target 实跑归真机轮，闸担保不静默跳过）。
- **host 全量套件本机复跑**（NRF-X1 兑现，integrator 独立于 builder 实跑）：espidf_wsl.sh build 绿（ELF 晚于 story_page.c mtime）+ `host_infra_test.elf` 实跑 → **220 Tests 3 Failures 0 Ignored**——3 红逐条核对全为 p1_auxbar AB-A1（电 batteries 图标源锚）/ AB-A2（温度图标源锚）/ AB-A4（wifi 资产重出锚）**先存基线红**（与 2026-10-20 基线锚 220T3F 同批同数，launch-card 在册）；story/record 区既有齿（story_page_v2 / record_dialogue_guide / record_dialogue_v7_fix 等）全绿，**零新增红**。
- **已知红申报**：上述 3 条 p1_auxbar 红为既有基线（261018/261019 包同批留根、auxbar 存量红在上一包 pod-retro 遗留在册），与本包正交——按「已知红≠本包红」申报，不造假、不判绿；修复归后续 auxbar 包。

### 白盒复核（NRF-X3，nano 行为验收判分位）

**方法**：detect_changes（project=C-AI-KidPal-pod-m2，base=HEAD 4ca3df2d，工作树 diff；depth=3，direction=inbound）+ trace_path 抽查 + grep 调用点核对（grep 为 ground truth）。

| 维度 | plan 五件⑤预估 | 实测 | 差值判读 |
|---|---|---|---|
| diff 文件数 | 1 个实现面文件 ≤ 2 | 1（story_page.c） | 一致 |
| 将触符号 | kidpal_story_page_render | 同 | 一致 |
| 调用作用域 | 4（mcp_server.cc 紧耦合簇）≤ 5 | **5** | **+1**：kidpal_render_node.cc:952 `rn_render_kidpal_page_locked`（KP_PAGE_STORY 分支渲染入口）plan 预估漏计——按调用作用域 5 ≤ 5 阈值**仍达标**，不构成 nano 判档翻案 |
| 调用点 | 6（mcp_server.cc :108/:142/:518/:1557/:1642/:1697） | **7**（+kidpal_render_node.cc:952） | 略超的量数再 +1，性质与 plan 已申报的「6 略超阈值」相同：全部为对同一符号的调用，diff 不触签名/本体，调用方零适配 |
| 图收录 | — | detect_changes/trace_path 只报 archive test caller（happy_flow_capture.test） | **降级圈定如实留痕**：mcp_server.cc / kidpal_render_node.cc 的调用边图缺——圈定与爆炸半径以 grep 逐点核对为准（上两行即 grep 实测）；per-Pod 图索引可用但调用边不全，属三分法「索引可用、trace 抽查降级」档，合法圈定 |

**语义影响面核对**：diff 改动全部落在 RECORDING 分支的绘制调用实参（坐标/scale/pivot）与注释内；`rec_substate==1` 判定、回显常驻锚（[G5] echo_reset 语义）、键位分叉、元素集均零触——与 plan 五件③ before/after 预期行为差一致，无超出意图的行为变化。

**结论**：实际爆炸半径 = 7 调用点 / 5 调用作用域 / 1 文件，与 plan 预估（6/4/1）同量级、边界量数在已申报方向上再 +1 且不越阈；**无 BLOCKING、无出界**，NRF-X3 通过。

### 结构性闸豁免清单（suiteGates / structural，dev-loop SSOT 审计消费节）

- `NRF-A1`（suiteGate 纯形态）：既有 host 全量套件零新增红——本机复跑兑现（见上）。
- `firmware-build`：结构性编译门（scoped defer → final 真编译 54.7s PASS）。
- `change-manifest` pre-cutoff 存量放行：gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28（闸输出原文 verify-runs.jsonl 在册）。
- `mutation-check` skip：nano 无验收语义净增（mutationSkipReason 在册——目标区域零坐标断言齿，无新冻结断言可变异）。

### Security gate

PASS（r5 + final 复跑）。`securityReviewRequired=false`（设备端本地绘制坐标微调，无安全轴）。

### Docs sync

PASS。零 canonical 改动据实申报，见同目录 [docs-delta.md](docs-delta.md)；v7 设计稿回同步 = 后补账（真机终判后直修快环），不阻塞。

## Five-axis semantic review（author 五轴，nano 比例化）

| 轴 | 结论 |
|---|---|
| 安全 | 无安全面触碰：纯本地绘制坐标，无输入处理、无内存布局变化（lv_image_set_scale/pivot 为 LVGL 既有 API 常规用法，无越界绘制面——缩绘落位 x=8..179 / y=191..219 在 400x300 版面内）。 |
| 契约 | 零契约触碰：display 统一响应块（wire schema）零触，NO_CONTRACT_CHANGE 自查成立；`docs/contracts/**` 本包零读改。 |
| 边界 | 分支内自洽：改动全部限于 RECORDING 帧共用块（帧 A/B 同生效，一次编辑两帧同拍——与 v7「在记信号三帧常驻」语义一致）；帧 C（整理态）零触；无跨层调用、无循环依赖、无新增依赖、无绕过 contracts 的本地类型。 |
| 可运维性 | 注释承载完整设计依据（安全距、缩绘落位、兜底条位式），后续维护者可从分支头注释直接复现坐标推理；日志行零变。 |
| 简化 | 实现与 plan 意图一一对应、无投机抽象；`minimal:` 标记 grep 计数 = **0**，简化债务台账为空。 |

### Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| LOW | judgement-call | plan 估算 | plan 五件⑤爆炸半径预估漏计 kidpal_render_node.cc:952（rn_render_kidpal_page_locked）调用点——实测 7 调用点/5 作用域 vs 预估 6/4；按作用域阈值仍达标，不翻档 | changed-code 上下文 + grep 调用点核对（firmware/main/display/kidpal_render_node.cc:952）；本包 human-ad-hoc 直提、triage grep 漏一点属量级内误差 | 记录留根即可；后续 plan 做调用点普查时补 grep 全点核对口径（本包无返工位） |
| INFO | judgement-call | 观感 | 缩绘 212/256（≈0.83）使声波资产较 v7 设计稿原尺寸缩小 ~17%，与设计稿存在可见差；206px 资产 vs 左区 ~170px 的张力由缩绘化解，是否可接受 = human 真机观感终判 | story_page.c 缩绘调用 hunk（lv_image_set_scale(wi, 212)）+ plan 五件③「真机目检为准」 | 真机目检（NRF-X4，本 Pod 到门停等） |

结构闸（verify.mjs final 10/10）PASS/FAIL 状态不复述为 finding；gate 全绿不豁免上述五轴语义 review，均已逐轴执行。

### Residual Risk

- 真机观感未经实证（host seam 400x300 与真实 RLCD 屏存在渲染差）——本包全部风险集中于 NRF-X4 真机目检 human 终判，status=awaiting-human:real-device；目检若打回，走 fixer（fixBudget=2 余量满）。
- v7 设计稿 / WF-F2-02 线框与实现漂移在真机终判前持续存在——后补账通道在册（plan「后补账提示」），非本包阻塞。
- p1_auxbar 三条先存红持续在册（非本包引入，修复归后续 auxbar 包）。
- `decisionCoverage` 中 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目：**无**（本 taskpackage 无 decisionCoverage 字段，无被丢决策）。

## UI design-review 证据（纯文本，bootstrap 期口径）

- viewport coverage：设备固定分辨率 400x300 版面，单帧内位式——不适用响应式 viewport 矩阵；落位核对以坐标算术承载（缩绘 x=8..179 不侵入回显列 x≥180、REC 行起点 200 距气泡右缘 8px）。
- a11y summary：不适用（1bpp 无灰阶设备屏，无对比度/焦点面变化；文案与元素集零变，读屏语义不变）。
- design-drift summary：本包即一次**受控 design-drift**——ask①（波浪线挪猫下方）偏离 WF-F2-02 线框画法与 v7 帧现位式，ask②（REC+时长同行）反而更贴线框「● REC 02:35」一行画法；architect 逃生门自查判实现面观感修（human 触点①确认），v7 设计稿回同步钉为真机终判后补账。
- external artifact reference：human 截图直提（会话内，未入仓）；`docs/ui/kidpal-ui-design-v7.html` :945–979 为回同步对象。
- residual risk：真机观感终判前，缩绘观感（finding #2）与 REC 行右端裕量（~300 行宽）未经实屏确认。

## 集成执行回报

无 merge-driver 冲突消解（本包单文件 diff、无对端包，跨包消解清单不适用）；无 spine 回写（本包无 spine）；无跨面回写。

## Simplification Debt

`minimal:` 标记 grep = 0 条；本包无新增简化债务。

## Closeout 记账

- 制品：`plan.md` / `launch-card.md` / `taskpackage.json` / `scope-final.md` / `docs-delta.md` / `review-packet.md` / `verify-runs.jsonl`（6 轮）；`verdict.json`（current 投影，final r6 全绿）。
- closeout-check --update-index 于 commit 前执行（结果见最终回报）；post-cutoff 不产 session-digest / loop-health / loop-metrics / pod-journal；pod-events advisory 检查见最终回报。
- status 交接：closeout 后 = `awaiting-human:real-device`（NRF-X4 真机目检门，本 Pod 不自行烧录）；human 终判后归 planner 走 supervisor 合并门。

## 回滚方式

单文件单 commit——`git revert <commit-sha>` 即全量还原（story_page.c 无其它在途改动）；无数据/契约/配置迁移面。
