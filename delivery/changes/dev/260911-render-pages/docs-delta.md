# docs-delta — 260911-render-pages（批次③ 渲染观感 + V3 整屏还原双轨）

> **追记（2026-09-12 收口刷新，真机轮 RP-A5 四轮 fixer + human PASS 后）**：本窗口 docs 面增量**仅一条** = `docs/ARCH-REVIEW-QUEUE.md` +2 行（**ARQ-260912-host-werror-comment-acceptance-gap** 登记，SHOULD；fixer 轮披露、非 integrator 落笔）。`docs/contracts/**` / `docs/spec.md` 维持零触碰（r6 contract-lint PASS + `git diff --name-only` 空输出实测）；docs/notes 无新锚点需求（真机轮收口未引入新 docRef）。前窗清单（§A-§D）全部维持有效、不重述。

> integrator docs-sync 结果清单 · 2026-09-12 · 分支 `dev/260911-render-pages`（base = main，HEAD = `83a6e66`）。
> 性质：固件观感/还原双轨包（subsystem=firmware，rigor=full，riskTier=high，gateProfile=host-teeth/device-round/frozen-acceptance-revision，mergeOrder 21）。
> `docsSyncRequired=true` + `docsOwner=integrator`。r5 docs-sync 闸 `ok:true`。**核心结论：canonical 面（`docs/contracts/**`、`docs/spec.md`）与 docs/notes 零 integrator 改动**——docs/notes 锚点由主线程先行落盘（本相位无 docs/notes 写权，enforce-scope 机判拦截、未绕过）；本包 docs 增量 = 主线程锚点 1 处 + 证据件清单（changes/dev/ 同目录）。

## A. docs 面增量清单（本包实际落盘的 docs 变化）

| 文件 | 变更 | 作者相位 | 内容 |
|---|---|---|---|
| `docs/notes/ui-implementation-strategy.md` | +2 行（append 注记，不覆盖既有行） | **主线程**（intake 相位，先于 integrator） | 「执行承载注记（260911-render-pages 包补）」：§2 转写纪律节钉明 = 本包 RP-A8（P2/P3/P4 页面换皮，坐标转写禁手抄）与 RP-A4（V3 字体体系）的执行依据；管线落点 `tools/asset-pipeline/`。该注记同时是 docs-sync 闸 r3 红的消解件（docRef 锚点补 acceptance id 关联） |
| `changes/dev/260911-render-pages/font-decision-s01.md` | 新增 | planner（S0.1 决策转写） | human 字体方案决策记录（触点②，DECIDED）：正文自产 GB2312 全集 6763 字子集 / 标题站酷+数字 Baloo 走 SPIFFS 非 KAP1 独立落地+分型新齿 / 预渲染位图路线废除。即 RP-E1「字体决策记录在案」evidence 主体 |
| `changes/dev/260911-render-pages/intake-survey-s02-s03.md` | 新增 | planner（intake 勘察） | S0.2 课表数据链勘察（根因 = 固件缓存时机，云侧 producer 无罪，F6 解除确认）+ S0.3 五态落点清单（PKG-2 合并后 rebase 校正版） |
| `changes/dev/260911-render-pages/red-baseline-freeze.md` | 新增 | acceptance-author（主线程转写） | 红基线冻结 FROZEN-BASELINE：在册红五条引证 + P2-A1 缺陷形态复核 + v3_restore 6 齿红基线 + U 闭集连带红 + F2/F5 新齿落点 + 终态追记（host 全量 124/3F） |
| 本目录 closeout 制品（review-packet / docs-delta / existing-red-inventory / verdict / verification） | 新增 | integrator（本相位） | closeout/audit 制品，非 docs 消费面 |

**实现侧注记承载处（未改 docs 面，注记随实现代码注释）**：`firmware/main/CMakeLists.txt`（kidpal_voice_states.c 接线注记：纯逻辑模块 / host_app linux target 同源 / 层底座零触碰）；`firmware/main/display/story_page.c`（`kidpal_story_reader_slice` REVISION LOG 头注：切片单源 / 指针不复制 / 4096B 栈纪律）；`firmware/main/display/kidpal_voice_states.{c,h}`（五态内容单源头注）；`firmware/main/display/kidpal_render_node.cc`（#11 空态注释：R-S0.2 根因引用）；`firmware/main/mcp_server.cc`（行高单源替换注记）；`firmware/main/test/{schedule_p2,story_p3,ui_art_integration}.acceptance.c` REVISION LOG（冻结件留痕面）。

## B. 明确没动的面（及为什么）

1. **`docs/spec.md` 零改动**：本包缺陷轨 = 兑现冻结契约既述行为（taskpackage origin 明示：#9「本项=兑现冻结契约既述行为非语义变更」），还原轨 = 观感换皮（LVGL 骨架与动态文本语义保留）——零 spec 语义面变化。实现中若发现与 spec 冲突走逃生门，本包全程零触发（S0.2 勘察明示「不触契约，逃生门未触发」）。
2. **`docs/contracts/**` 零改动（只读铁律）**：display 统一响应块 / 笔顺 JSON / 课表 MD 结构 / shim API 四面零变化——换皮消费的仍是既有 `kidpal/display` 契约载荷，字体/资产为设备本地渲染面、不触 wire 形状。`git diff --name-only -- docs/` 实测空输出（见 §D）。
3. **docs/notes 其余文件零改动**：integrator 无 docs/notes 写权（enforce-scope 机判）；唯一需要的锚点已由主线程落盘（§A 首行）。
4. **docRefs 锚点解析现状（如实记录，不代修）**：input-brief-builder 快照中部分锚点 not-found（`PROGRESS-SUMMARY-260910.md#§四A/#§四D`、`review-packet.md#F5`、`kidpal-ui-design-v3.html#sec-pages/#sec-states/#sec-rules`）——锚点语法与文档实况的定位偏差，正文 SSOT 均可整文件消费，非内容缺口；不阻塞 docs-sync 闸（r4/r5 ok:true）。

## C. docs-sync 闸收敛轨迹（r3 红 → r4/r5 绿）

- **r3 红**（inv `3753750d-…`）：`docsWithoutAcceptance:["docs/notes/ui-implementation-strategy.md"]`——docRef 面缺 acceptance id 关联。
- **消解**：主线程在 docs/notes 落 RP-A8/RP-A4 执行承载注记（§A 首行）。
- **r4/r5 绿**：docs-sync `ok:true`（r5 276 ms），`docsSyncRequired:true` 满足、docRefs 与 acceptance 全关联。

## D. 机器核据（integrator 本相位实跑）

```
git diff --name-only -- docs/                 → 仅 docs/notes/ui-implementation-strategy.md（主线程锚点 +2 行，非 integrator 改动）
git diff --name-only -- docs/contracts/       → 空输出（契约面零触碰，实测）
git diff --name-only -- docs/spec.md          → 空输出（spec 零触碰，实测）
git diff --name-only -- server/               → 空输出（server 面零触碰，S0.2 裁定）
git diff --name-only -- managed_components/   → 空输出（RP-A6 冻结面零越界）
contract-lint 闸 r5 ok:true                   → 闸对象 docs/contracts/interface-contract.md
docs-sync 闸 r5 ok:true                       → docsSyncRequired=true + docRefs/acceptance 全关联
node .claude/scripts/closeout-check.mjs       → 见 review-packet §Closeout（本相位实跑）
```

> 本包 docs-delta 结论 = **canonical 零改动 + docs/notes 锚点主线程承载 + 证据件齐**，与 `docsSyncRequired=true` 满足态一致，无遗留同步项。
