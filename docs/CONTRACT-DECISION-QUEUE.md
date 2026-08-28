# CONTRACT-DECISION-QUEUE · 契约决策队列

> 记录契约面（`docs/contracts/**`）变更的决策状态与剩余条件。骨架依据 `.claude/references/contract-change-evidence.md`。
> 状态词：DRAFT / PRECHECKED / HUMAN_APPROVED / CONDITIONAL-APPROVE / LANDED / CLOSED·已结。

| CDQ-id | 日期 | 变更面 | 四分类 | human 门 | 状态 | 关联 change | 剩余条件 |
|---|---|---|---|---|---|---|---|
| CDQ-260829-storylist-total | 2026-08-29 | `interface-contract.md` §1.3 ③ story_list.data + `total`(additive 可选整数,故事果计数) | CONTRACT_CHANGE_REQUIRED | 批准 2026-08-29 | **CLOSED·已结** | [260829-arch-review-kidpal-spec-stories](../changes/arch/260829-arch-review-kidpal-spec-stories/) | 无（evidence 台账见该 change 包 architecture-gate-evidence.md） |
