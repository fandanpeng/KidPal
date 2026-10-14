# docs-delta · 260901-ui-art-integration（包 U）

## 1. canonical 面总口径

- **契约面零变更**：`docs/contracts/**` 一字未动（diff 无该树文件；contract-lint GREEN；验收文件内契约 canary 在世）。无需走 architect-reviewer 四分类。
- **spec.md 零变更**：本包行为全部落在 spec §5.6（教学字体红线，资产侧确认标准字体与可爱体共存）/ §5.7（美术资产管线，决策 17：离线位图 + imgfont 只留动态时间）既有冻结口径内，无漂移、无新增措辞需求。
- **PROPOSAL 面**：`.planning/kidpal-mvp/PROPOSAL.md` 零变更；U 包四条 decisionCoverage 已在 TaskPackage 登记并与 PROPOSAL §2/§4 对齐。

## 2. docs-sync 闸红项：validation-batch-conclusion.md 锚点（PENDING，待有写权方落盘）

`node .claude/scripts/docs-sync-check.mjs` 红项唯一：docRef `docs/notes/validation-batch-conclusion.md` 未提及任何 ART-A* 验收 id（其余 docRefs PROPOSAL.md / spec.md 已含弱 token 过闸）。

**补法**：在该文件**末尾 append-only** 追加下节（历史记录类文件不覆盖既有行）。integrator 的 Edit 已被 enforce-scope 机判拦截（integrator 角色写边界 = firmware/server/docs/systems/docs/templates/ARQ/ADR/changes/dev/.claude/taskpackages/.planning，不含 docs/notes；planner 同样 deny docs/**）——照 red-baseline-summary 归位注记先例，逐字交主线程落盘：

```markdown
## 8. 美术资产集成（U 包 260901-ui-art-integration）验收锚点

> 2026-09-09 integrator closeout 按 append 纪律追加，上文各节零改动。

本 notes 的 TP-B UI 可行性结论（§2：双层方案成立、无 🔴 级项、预算不超）与 §6 真机留根项，对应美术资产集成 U 包的冻结验收（`firmware/main/test/ui_art_integration.acceptance.c`，docRef 锚点）逐项落位如下：

- **ART-A5 资产出图落地**：§2「全部走资产管线出图并机判转绿」的资产本体经 V3 设计稿定稿后落地为 `firmware/main/assets/spiffs_root/` 47 枚闭集（吉祥物 7 姿态/14 枚图标族/手绘框族/网点/站酷快乐体 + Baloo 2）。
- **ART-A2~A4 固件侧管线**：§2 双层方案中「预栅格化位图资产扛美术」的固件承载半——SPIFFS assets 分区 + 1bpp 位图加载（ART-A2）、imgfont 动态时间数字（ART-A3，对应 §4 第 2 条「imgfont 只留给动态时间数字」取舍）、9-slice 手绘框族 + 抖动网点 d50/d25/d12（ART-A4）。ART-A1 为装配结构齿。
- **ART-A6 六页换皮**：各 UI 渲染面统一接入共享资产层（消费 §4 第 2 条「倾斜/手绘框/网点全部烘进离线资产，运行时零成本」的落地面）。
- **ART-A7 host 回归**：换皮后各页面 host 回归（布局/逻辑不破），独立于真机观感通道。
- **殿后项对应**：§6 第 1/2 条真机留根（I1 真机文件源路径、16px 以下 CJK 1bpp 真机可读性）+ SPIFFS 真机文件源路径与真机观感，归 9b（ART-E2 显式殿后，本包 host+SDL 通道不覆盖）。
```

落盘后复跑 `node .claude/scripts/docs-sync-check.mjs` 应转绿（判据 = docRef 文件弱 token 命中 ART-A* id；gate 为文件级 + 弱 token 命中，非语义覆盖，CH-14 已注记该弱点）。

## 3. progress.md（.planning/kidpal-mvp/）

integrator 已按 append 纪律补记本包收口段（append-only，见该文件 2026-09-09 U 包段）。

## 4. 其余 docs 面

无。docs/ 根、docs/ui/（V3 设计稿为前置输入，本包不回写）、docs/templates/ 零变更。
