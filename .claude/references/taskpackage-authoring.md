# TaskPackage 撰写约定 · 字段约定单一入口（撰写侧）

> 定位：planner 起草 TaskPackage 时的字段级撰写约定单一入口。五个包独立实证、反复踩坑的 6 个字段坑照本清单一次填对，免去每包 1–3 轮返工。本文只补撰写侧约定：不新增任何机械闸、不改任何既有闸判据。
> 来源：ARQ-260813-taskpackage-friction-fields-template（`docs/ARCH-REVIEW-QUEUE.md`，5 包独立实证）；承载批 = [260824-gov-review-taskpackage-authoring-convention](../../delivery/review/260824-gov-review-taskpackage-authoring-convention/PROPOSAL.md)。

## 1. docRefs 只列承重引用

`docRefs[]` 只放本包实际改动、或承载 acceptance 提及的参考件；上下文参考件不入 `docRefs[]`。不改动文档时走 `docsNoChangeReason` 诚实路径，勿为通过 docs-sync 检查硬塞引用。

- 出处实证：docs-sync 参考件误伤 ×3（error-surface 三个 docRef 全为上下文引用；eval-tp1/tp2 的 pod-board.md / CLAUDE.md 通用参考件不承载 acceptance 提及而被闸报红）；闸判定逻辑见 `docs-sync-check.mjs:101-111`（无 acceptance 提及且无 `docsNoChangeReason` → 红）。

## 2. securityReviewNotes 覆盖 requiredAxes 全轴

securityReviewNotes 按 security-check 的全轴清单逐轴落笔（含 ai-cost、ai-output 等轴），漏轴必红。

- 出处实证：eval-tp1 只写 ai-cost 漏 ai-output 轴，闸报红后补齐；requiredAxes 枚举见 `security-check.mjs:55-65`。

## 3. mutationRequired=false 必配 mutationSkipReason 或 mutationReason

声明 `mutationRequired=false` 时必须配 `mutationSkipReason` 或 `mutationReason`（闸接受任一，`mutation-check.mjs:93`），说明本包为何无 mutation。

- 出处实证：i1 等 3 包草稿遗漏该字段（friction `b5818e2c`，retro-2026-07-30 F1）。

## 4. docsSyncRequired=false 必配 docsSyncReason 或 docsNoChangeReason

声明 `docsSyncRequired=false` 时必须配 `docsSyncReason` 或 `docsNoChangeReason`（闸接受任一，`docs-sync-check.mjs:74`）。「零写注记串」惯例：不改动的 contractRefs / docRefs 清单显式写空注记，不省略说明。

- 出处实证：`260820-s1-m02-orphan-debt` scoped r1–r5 连续五轮红全是字段口径（ARQ-260813 第 5 实证）。

## 5. mutationEvidence 用 acceptance 原样完整文本

mutationEvidence 必须原样使用 acceptance 完整文本——不转述、不缩写。

- 出处实证：eval-tp2 固化经验（ARQ-260813 原始实证第 4 条）。

## 6. coverage marker 每 id 独占一行

coverage 标记按每 id 独占一行书写，严禁 `coverage: A, B, C` 单行逗号写法——闸按逐 id 紧邻匹配（`acceptance-coverage-check.mjs:48`），逗号分隔单行只命中首个 id。

- 出处实证：`260820-s1-m02-orphan-debt` 的 `coverage: A, B, C, D` 只命中 A（ARQ-260813 第 5 实证新子形态）。

---

**与闸的关系**：本清单 6 条均为既有闸行为与固化经验的转述，不改任何判据；转述与闸源码不一致时以闸源码为准，并按 ARQ 流程登记（减摩擦非加门——承载批硬边界）。
