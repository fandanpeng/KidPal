---
id: ARQ-261019-opendesign-shoot-script-precedent
title: "设计稿出图先例修正：出图走仓内 shoot_design.mjs，不依赖 Open Design 应用"
domain: gov
status: closed
closed: 2026-10-27
closedBy: 261027-gov-review-p75-closeout-shoot-cluster
severity: SHOULD
owner: gov
trigger: "① opendesign-connect skill 增补「出图不走 OD」先例段（od_* 失联/应用未跑时出图管道 = tools/asset-pipeline/shoot_design.mjs <html> <outdir>，playwright 无头 Edge 逐画布精确截图、400×300 阈值化 1bpp，每批只捡变更帧落 docs/ui/V<n>/）；② uiux-author agent 定义增补同款指引；③ 261019 批实测补充：uiux-author 全文件 Write 曾两次覆盖主线程合并的完整母版（R1 回归），skill 增补「母版/增量页单写者约定」条目"
opened: 2026-10-19
paths: [.claude/skills/opendesign-connect/SKILL.md, .claude/agents/uiux-author.md]
hardExclusions: true
source: [delivery/intake/261019-p3-record-dialogue-guide.md]
fourClass: NO_CONTRACT_CHANGE
loopEvidence: "261019-p3-record-dialogue-guide spine q8（human 指点「出图应该不用 Open Design，HTML 改好之后就可以调他那个脚本去出图」，主线程实测 shoot_design.mjs 全链成功 + 目检通过）"
---

# ARQ-261019-opendesign-shoot-script-precedent · 设计稿出图先例修正：出图走仓内 shoot_design.mjs

## 背景

261019-p3-record-dialogue-guide 批 4.5 步出图时 OD daemon 失联（Open Design 应用未跑、会话内拉起失败），按 procedure 4.5 本应挂 human-decision；human 当场指点「出图应该不用 Open Design，HTML 改好之后就可以调他那个脚本去出图」——主线程实测 `tools/asset-pipeline/shoot_design.mjs` 全链成功（15 画布精确截图、阈值化 1bpp、三帧捡进 docs/ui/V7/），并与 uiux-author 立母版单写者约定（其全文件 Write 曾两次把主线程合并的完整 v7 母版覆盖回 3 帧增量页）。

## 改进项

1. **opendesign-connect skill 增补**：「出图不走 OD」先例段——od_* 失联 ≠ 出图堵死；出图管道 = `node tools/asset-pipeline/shoot_design.mjs <html> <outdir>`，每批只捡变更帧（261016 先例）。
2. **uiux-author 定义增补**：出图与母版写入分工——uiux-author 交增量页/补丁说明，完整母版与出图归主线程；防整文件覆盖回归。
3. **真因记录**：OD daemon 端口动态 + 应用拉起失败是常态场景，「出图依赖 OD 应用」是历史误解，skill 既有文本会误导后续批挂等 OD。

**结项追注（2026-10-27，261027-gov-review-p75-closeout-shoot-cluster 批；R1 F1 修订）**：三条改进项全部落地——① `opendesign-connect/SKILL.md` 增补「出图管道」节（命令 / 行为 / 落点 / 只捡变更帧 / 先例）；② `uiux-author` 定义增补出图与母版写入分工（description / Role / Step 3 / Boundaries 四处通道句同步）；③ 真因记录随 ① ② 成文消解——skill「出图管道」节与定义四处均明写「OD 失联只挡设计生成」，条目所述「skill 既有文本会误导后续批挂等 OD」的口径已清除。另落条目 **trigger ③**（正文改进项编号之外的 261019 实测追加项）：母版 / 增量页单写者约定，挂 `uiux-author` Step 2 末尾。连带对齐出图消费者四处（`DEV-GOVERNANCE.md` §2 团队表、`references/requirement-change.md` 4b 与角色表、`commands/requirement-change.md` 4.5）——出图一律走 `shoot_design.mjs` 管道，od_* 只承载设计生成。

## 优先级

SHOULD——不影响本批已闭环；下次触 UI 批次（uiux-author 重派）前落地即可。
