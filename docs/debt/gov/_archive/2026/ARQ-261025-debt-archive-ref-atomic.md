---
id: ARQ-261025-debt-archive-ref-atomic
title: debt 结项归档归位与引用更新不同批——docRefs / sourceRefs 静默陈旧，final 轮 docs-sync 才红
domain: gov
status: closed
severity: SHOULD
owner: architect（起包自查文面）+ dev-loop（debt-render 行为）
trigger: 下一治理批（动 `debt-render.mjs` / `task-intake-and-slicing.md` / integrator 收口文面时并批）；或再出现一次归档后引用陈旧
opened: 2026-10-25
closed: 2026-10-27
closedBy: 261027-gov-review-p7-devloop-doc-cluster
paths: [".claude/scripts/debt-render.mjs", ".claude/references/task-intake-and-slicing.md", ".claude/agents/integrator.md"]
source: [delivery/retro/2026-10-25-loop-retro.md, delivery/taskpackages/261013-lang-config-thinking-fix/pod-retro.md, delivery/taskpackages/261017-prompt-v11-schedule-recall/pod-retro.md, delivery/taskpackages/261022-p3-record-chrome-ghost-fix/pod-retro.md]
---

**证据**：① `261013-lang-config-thinking-fix`：integrator 收口把 TECH 条目 `crq-render` 归档进 `_archive/2026/`，planner 的 `sourceRefs` / `docRefs` 立刻陈旧，final r4 docs-sync 红才暴露（1f 权内修正）。② `261017-prompt-v11-schedule-recall`：同型——integrator 先翻 closed，`debt-render.mjs` 按「位置 = 状态纯函数」自动归位，TaskPackage 的 docRefs 仍指旧顶层路径，final 红在 missingDocs。③ `261022-p3-record-chrome-ghost-fix`：docRefs 锚句起包时未回填（acceptance id 定稿前 plan 已写），r2 docs-sync 红。

**影响面**：凡本包要结项 debt 条目、或 docRefs 指向 debt 条目的包。归档搬迁是渲染器的自动副作用，引用更新是起包时的手写事实，两者没有同批绑定；每次都要用一轮 final 复验来发现。本波 3 包命中，前两包各多烧一轮。

**建议改动**（交 review、不自批）：① 起包自查加一条「本包要结项 debt 时，docRefs / sourceRefs 预留 `_archive/<年>/` 归位形态」；② 或让 `debt-render.mjs` 在归档时保留稳定指针（如 `docs/debt/<域>/<ID>.md` 重定向 stub），使引用不必随位置变化；③ docRefs 锚句在 acceptance id 定稿时即回填，不等 verify 红。倾向 ①+③（纯文面），②需评估 stub 是否造第二事实源。

**风险**：riskTier: low——①③为起包/收口文面；②若做只影响渲染器产物路径，不改门判据。

**结项追注（2026-10-27，261027-gov-review-p7-devloop-doc-cluster T3）**：修法 ①+③ 已落地——task-intake-and-slicing.md 新增「docRefs / sourceRefs 指向 debt 条目时的归档形态（起包自查）」节（结项条目引用预留 `_archive/<年>/` 归位形态、锚句随 acceptance id 定稿回填、不立重定向 stub）；② stub 按条目既裁不做。integrator.md 侧句省略（归位是渲染器副作用、integrator 无手搬动作，ack 落点约束）。R1 GREEN、落地五件 sha256 对账一致。
