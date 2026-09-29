---
description: 需求面变更 loop 入口（对偶 /build-feature）。驱动一个需求变更走 requirement-change.md 三档流程：档位判定 → story-author 盘上 updating 起草 → requirement-reviewer 四查否回循环 →（档 2）architect 可行性三件 + human 终拍 → 主线程翻 frozen + §9 记账。
argument-hint: <human 需求意图原话，例如 "查字窗演示改 KEY 起播，播报保持现状">
---

# /requirement-change

你现在是**主线程编排者**。按 `.claude/references/requirement-change.md`（流程 SSOT）推进本次需求变更；这是需求面正式变更的默认入口，档 0 勘误也在此入口收束。本次 human 意图输入：

> $ARGUMENTS

逐步执行，并在每一步显式说明「派给谁、为什么、期望产物」：

1. **档位判定**：按 requirement-change.md 三档判据判 0/1/2（0 勘误/措辞 / 1 语义修改 / 2 结构性）；存疑向严升档，升档裁定主线程提呈、human 确认。意图原话缺失或含糊 → fail-closed 用 AskUserQuestion 回问 human，不脑补语义。
2. **档 0 收束**：主线程直改（挂 write-story 完稿自检 + `node .claude/scripts/spec-anchor-check.mjs` 须绿）→ §9 追加变更行标「勘误」→ 结束。档 0 不走状态机；若 requirements.md 处于 updating 在途，不并行执行——按 SSOT 边界场景裁等 frozen 或随批落。
3. **起草（档 1/2）**：派 `story-author`——盘上起草（直接编辑 `docs/requirements.md`，头部状态行置 updating；architecture.md 镜像句同批），return 交回自检结果 + 撞车报告 + 已改锚行清单 + 改前/改后句级对照 +（档 2）牵连面清单。起草前核状态行必须为 frozen（已在途 = 先按 SSOT 边界场景处置）。
4. **可行性（仅档 2，默认必派）**：派 `architect`——产出固定三件：可行性三态（✅/🟡/🔴）+ 逐 story/AC 技术锚 + 影响面概貌。免派唯一例外：human 明示可行性已有在先评估承载（免派事由记 §9 该行依据列）。🔴/取舍点升回 human，不带病下行（author 按可行性修订后重走第 3 步自检）。
5. **审稿（档 1/2）**：派 `requirement-reviewer` 四查——dispatch prompt 输入构成显式含「human 意图原话 + author 改前/改后对照」，审稿对象 = 盘上 updating 态文件；任一查不过 → 逐条否回项打回 story-author 改盘上文件，循环至 PASS 或升级 human。
6. **落稿**：档 1 = PASS 后、档 2 = human 终拍后（architect 结论 + reviewer 结论一并呈），主线程翻状态行 frozen + §9 追加版本行（版本号即 changeId）+ 依 SSOT 分工表核 reviewer/author 结论留痕进依据列。
7. **机检与消费保护**：每次翻转/落稿跑 `node .claude/scripts/spec-anchor-check.mjs` 须绿；updating 态期间本文件不得进入下游消费（triage / taskpackage / launchPreconditions 只认 frozen——机械判据 = §9 版本行在位）。
8. **记账收尾**：跨面变更（触面 ≥2）先按 `change-intake.md` 建/更新 spine 并回填 anchors.req；单面变更直接收束。human 明确说跳过流程的一次性操作允许直改，§9 记账须写明「跳过 story-author/reviewer」。

你不改 reviewer 判决、不跳否回循环、不代笔修订；触 `docs/contracts/**` 的发现一律路由 architect-reviewer 四分类 + 人类契约门，本流程无权处置。
