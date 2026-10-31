---
description: 治理面变更 loop 入口。轻设计 → reviewer 设计审与双向判档 → architect 直接实施 → reviewer 真实 diff 审 → 机械检查与合并；既定 ARQ 一句启动，条件加强按实际风险启用。
argument-hint: <human 意图原话或 ARQ-id 清单，例如 "ARQ-260928-arch-change-retirement">
---

# /gov-change

你现在**主线程编排者**：派发、FROZEN 状态、保护面处理、实时事件、机械检查、合并与记账；不起草变更内容，不常规转录评审。按 `.claude/references/gov-change.md`（流程 SSOT）推进。

> $ARGUMENTS

1. **立项**：新方向由 human 裁意图；既定 ARQ 波次先读条目，只说一句启动确认，不重复展开旧修法。带 spine 时先消费 faceMap/facePlan。
2. **Track A 段（仅方向未收敛或声明红队）**：派 review-moderator 跑对抗机器，human 拍方向后流回常规实施；触及承重机器不自动升级。
3. **轻设计**：派 architect 写轻 PROPOSAL 与轻量 landing-manifest（路径、实施归属、保护标记），头部标 direct-implementation，不写逐字节改法。
4. **设计审**：派 architect-reviewer 四查并判常规或条件加强，可升可降；reviewer 自写评审段。GREEN 后由主线程标 FROZEN（冻意图与验收，不冻字节）。
5. **作者实施**：architect 在非 main 分支实施普通治理文件、自测并提交真实 commit；保护面由主线程处理。出 scope 必须回 reviewer。
6. **实现审**：reviewer 审真实 diff，最后 verdict 后出者胜，并以 reviewedCommit/reviewedTree 钉最后实现评审；后续实现变化须重审。
7. **合并收尾**：跑聚焦检查和前置升级闸，按 merge-to-main skill 合本地 main，落地 commit 前 upsert `delivery/review/INDEX.md`。普通批不派 landing-verifier、不写 landing-summary/verdict 或事后 retro。

主线程实时追加本批 events.jsonl，usage 只记 `{agent, wallMs, tokens, toolUses}`（各值可 null）；事件不进闸或 INDEX。架构+契约面转 `/arch-change`，实现面转 `/triage-change`。
