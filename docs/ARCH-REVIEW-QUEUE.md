# ARCH-REVIEW-QUEUE · 架构 review 未结项队列

> 自 AInativecompany 迁移的净版队列（2026-08-27）。原仓历史条目不入本仓——原仓全量见 `c:\AI\AInativecompany\docs\ARCH-REVIEW-QUEUE.md`。
> 性质：工作队列（收件箱），只跟踪 `/review-arch`、`/review-governance`、`/arch-change` 产出的未结 spec / governance soundness 项：open findings、待拍板 deadlocks、follow-up、accepted-as-risk watchlist。已结项移入 `docs/archive/ARCH-REVIEW-QUEUE.closed.md`。
> 详细裁决、证据、debate log 归 `changes/arch/**/review-packet.md`、`decision-packet.md`；本文件只保留当前未结项。

## 0. AI 维护规则

| 规则 | 写法 |
|---|---|
| 新增 ARQ | 只有 unresolved / future-trigger / accepted-risk 仍需可见的项才进入 `1. 当前工作队列`。 |
| ARQ ID 格式 | 新条目用 `ARQ-<YYMMDD>-<english-kebab-slug>`：YYMMDD=创建日；slug=短英文 kebab（ASCII `[a-z0-9-]`、≤~5 词、不收尾 `-`）；人读中文标题留 title 列。子项引用 `ARQ-<YYMMDD>-<slug>-<子项>`。 |
| 已结 ARQ | human 批准且 landing 完成后即已结：从当前队列移除，在 `2. 已结索引` 加一行（ID / title / evidence 链接）。 |
| 计数口径 | 不手写计数：header 不带「（X 项）」后缀。 |
| 状态轨迹 | 治理落地状态不在本文件——由 `node .claude/scripts/changes-index.mjs --upsert <id>` 单行钉进 changes/INDEX.md arch 段。 |
| 写法约定 | 队列行散文写通顺完整的句子；内部标识符首次出现带一句实质说明；表格列结构与证据引用保持精确（见 AGENTS.md「输出标准」节）。 |

---

## 1. 当前工作队列

### 1.1 Blocker / 待拍板

| ID | 严重度 | title | 未结原因 | 处理时机 | 来源 |
|---|---|---|---|---|---|

### 1.2 Deferred / forcing-function

| ID | 严重度 | title | 未结原因 | 处理时机 | 来源 |
|---|---|---|---|---|---|
| ARQ-260827-tier1-boot-review | SHOULD | KidPal spec v0.1 + interface-contract 走首次对抗 review（spec 冻结前置门） | 项目现处 spec 草稿期，`docs/spec.md` 与 `docs/contracts/interface-contract.md` 均未经对抗 review；冻结前须过一轮（档 1 主用途即此）。review 时顺带裁定三个契约草案标记（E1 display 块、B1 课表 MD）是否达冻结级。 | human 说开始审 spec 时（建议入口 /arch-change 或 /review-arch） | 治理迁移（2026-08-27）首个登记项 |
| ARQ-260831-p4-stroke-dasharray-visual-check | COULD | P4 笔顺「未写笔画的虚线点链」dasharray 观感与契约 D3 canvas 折线渲染需视觉对齐核验 | 设计稿用 dasharray 描边实现虚线点链、允许「自然淡网格」观感；契约 D3 定的是 canvas 折线逼近贝塞尔，未定虚线渲染细则。两者之间需一次目测对齐；若确认需要契约字段，单独走契约门（不预批）。 | P4 dev-loop 包验收阶段 | 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3） |

### 1.3 Accepted-risk watchlist

| ID | 严重度 | title | 保留原因 | 复议触发 | 来源 |
|---|---|---|---|---|---|
| ARQ-260827-contract-mech-gate-gap | SHOULD | pass-oracle 第 4 关（spec↔contract 机械门）暂无脚本兜底 | 原仓 `arch-spec-contract-check.mjs` 深绑多文件 Prisma contracts 结构，不适用单文件 MD 契约面，未迁。pass-oracle 第 4 关在本仓降级为人工核对，须如实记录「机械门缺失」，不得虚报已机检。将来契约面复杂化（出现第二份契约文件或字段级 drift 高发）时写一个针对 markdown 契约表的一致性检查脚本补位。 | 契约面扩到 ≥2 文件 / 或出现首次 spec↔contract 漂移实例 | 治理迁移决策记录（2026-08-27，MIGRATION-NOTES §已知缺口） |
| ARQ-260831-simulator-lvgl-version-drift | SHOULD | 模拟器 LVGL 版本与固件错位（9.5 vs 9.3）是常驻风险，且 9.5 渲染行为存在实测差异 | 模拟器工程是独立拷贝（LVGL 9.5.0），固件锁 `~9.3.0`，未对齐。实战已证实差异不止 API 层面：9.5 软件渲染的 I1 索引图像路径绕过调色板（`lv_draw_sw_blend_to_argb8888.c` 的 `i1_image_blend` 把位图当 alpha/灰度直接混色，调色板仅经 bin-decoder 对文件源走 `decode_indexed_line` 生效）——「模拟器结论不直接当真机结论」必须作为纪律执行，双端同资产验证不可省。每次固件升 LVGL 组件锁，模拟器须同步；建议在未来 simulator 相关包落地时把版本对齐做成构建期校验。 | simulator 相关 dev-loop 包立项 / 固件 LVGL 组件锁变更 | 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3 + §8.2-V1） |
| ARQ-260831-simulator-truecolor-vs-1bpp | COULD | 模拟器（真彩灰度）与真机（1bpp 阈值化）的显色差异是第二类失真 | 同一张 UI 在模拟器上细线/小字可读（FreeType 灰度抗锯齿），阈值化后可能糊掉（实测设计稿 12–16px 文字阈值化后明显糊边）。建议 simulator 包顺手做「画布快照阈值化 PNG」一键自查，验收图一律以阈值化版本为准；真机字体子集化时 16px 以下 CJK 的字号下限需实测。 | simulator 相关 dev-loop 包立项 / 真机字体子集化任务 | 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3 + §8.2-V6） |
| ARQ-260831-cross-surface-check-missing | SHOULD | landing-fidelity-check 的 cross-surface 子门（GAR-001）子脚本未随迁出，结构门末段不可用 | `.claude/scripts/landing-fidelity-check.mjs:324` 调用 `.claude/scripts/cross-surface-check.mjs`，该文件在治理迁移时未迁入本仓，凡 diff 面含 `.claude/scope-map.json`/agents/settings/scripts/hooks 的 landing 都会以 MODULE_NOT_FOUND 红掉末段（前置断言不受影响）。要么从原仓补迁该脚本，要么裁剪/降级该触发；迁前 landing-verifier 须人肉覆盖该子门判面。 | 下一个含 .claude 面改动的治理批之前 | 260830-arch-review-ui-impl-strategy（landing-verdict §7 + retro F5） |
| ARQ-260831-docs-top-level-registry-gap | SHOULD | docs 平铺后 scope-map docsTopLevel 白名单未系统性对账，repo-structure.md 骨架缺失 | 260830 批 landing 时发现 `docs/spec.md`/`docs/notes/`/`docs/ui` 已在 git 中存在却未登记进 universal.docsTopLevel（白名单拦截 landing 写入）；当批只做最小补登。残留：①其余 docs 面路径未全面对账；②enforce-scope 报错文案引用的 `.claude/rules/repo-structure.md` 骨架文档不存在，登记流程缺第二落点。建议立治理包做全面对账 + 建册。 | 下一个 docs 面新增类别出现之前 / 或治理维护窗口 | 260830-arch-review-ui-impl-strategy（landing-summary §2 + retro F4） |

---

## 2. 已结索引

> 已结项按统一格式追加在此（ID / title / evidence），证据链接指向 `changes/arch/**`。
