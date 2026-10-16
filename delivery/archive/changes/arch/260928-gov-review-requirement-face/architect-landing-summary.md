# architect-landing-summary · 260928-gov-review-requirement-face

> 主线程 packet-bound 落地记录。packet = FROZEN PROPOSAL v1.0 + 本 summary + review.md（R1 BLOCK→R2 GREEN）。

## 落地动作（T1–T6 ↔ manifest 七路径逐一）

| T | 落地 | 目标文件 |
|---|---|---|
| T1 | 新增需求面起草角色 agent（只起草不裁定、无 Write、同会话 return） | `.claude/agents/story-author.md`（新增） |
| T2 | 新增需求面审稿角色 agent（四查 + 否回权 + 不代笔、无 Write） | `.claude/agents/requirement-reviewer.md`（新增） |
| T3 | 新增需求面变更流程 SSOT（三档判据 + 升档规则 + 两档流程 + 跳过例外 + 定稿载体 + REQ- CRQ 闭环纪律 + 角色分工） | `.claude/references/requirement-change.md`（新增） |
| T4 | §9 纪律引言追加定稿载体 + 跳过标注 + 流程指针（追加不覆盖，零 story 语义改动） | `docs/requirements.md`（改） |
| T5 | `git mv` 改名 + frontmatter id 同步 + crq-render 再生 + crq-doctor 绿 | `docs/crq/requirement/REQ-260916-home-date-format-english.md`（迁入）· `docs/crq/requirement/INDEX.md`（再生） |
| T6 | ARQ-260928 正文末 append「建议 2 已由本批承载」注记（只追加不改既有行） | `docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md`（append） |

## 验证集执行（§5，2026-09-28 pre-commit）

| # | 结果 | 关键输出 |
|---|---|---|
| V1 preflight | ✅ ok | anchorCount 7 ≡ manifest 七行，零 advisory |
| V2 lint-descriptions | ✅ exit 0 | scanned 15 / flagged 1 = landing-verifier.md 超长（**存量既有**，非本批引入；本批两新文件未 flag） |
| V3 route-regression-check | ✅ 28 ok 0 fail | 既有断言不退化（真空绿注记见 PROPOSAL V3 行） |
| V4 spec-anchor-check | ✅ ok:true | forward / v5 设计稿 / 13 story / 31 锚行 / errors [] |
| V5 crq-render + doctor | ✅ | requirement/INDEX.md 已渲染（REQ- 条目入册）；108 条目；RED 零命中 |
| V6 全 ID 残留 Grep | ✅ 销项达成 | 三归类清单见下 |
| V7 git status | landing 后 commit 时核 | — |

## V6 三归类清单（全 ID `ARQ-260916-home-date-format-english`）

- **① 更新类**：零命中（与 T5 执行注记「预计≈空」一致；requirement/INDEX.md 已随再生换 REQ-）。
- **② append-only 历史类（不改写，留痕）**：`delivery/changes/arch/260923-gov-review-arq-ssot-restructure/`（architect-landing-summary.md ×2、landing-verdict.md ×1、PROPOSAL.md ×7）+ `delivery/changes/dev/260915-m1-n1b-p1-home-battery/`（device-round-record.md ×1、docs-delta.md ×1、review-packet.md ×4）。
- **③ G3 注记类（登记不硬治）**：`.claude/scripts/crq-migrate.mjs:32` 迁移映射注记（脚本属强制层、本批未触面）。
- **销项 = 三归类清单之外零命中：达成。**

## 裁定与凭据链

human 立项 ack（2026-09-28）→ architect R1 BLOCK(F1,F2) → 主线程修订 v0.2（F1 全 ID 三归类 / F2 采 (a) 版本号即 changeId / F3–F10 全处置）→ R2 GREEN（4 MINOR 修毕后冻）→ human 裁 OQ-5 = (a) + FROZEN 确认 → packet-bound landing（本件）。
