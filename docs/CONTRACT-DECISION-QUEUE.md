# CONTRACT-DECISION-QUEUE · 契约决策队列

> 记录契约面（`docs/contracts/**`）变更的决策状态与剩余条件。骨架依据 `.claude/references/contract-change-evidence.md`。
> 状态词：DRAFT / PRECHECKED / HUMAN_APPROVED / CONDITIONAL-APPROVE / LANDED / CLOSED·已结。

| CDQ-id | 日期 | 变更面 | 四分类 | human 门 | 状态 | 关联 change | 剩余条件 |
|---|---|---|---|---|---|---|---|
| CDQ-260829-storylist-total | 2026-08-29 | `interface-contract.md` §1.3 ③ story_list.data + `total`(additive 可选整数,故事果计数) | CONTRACT_CHANGE_REQUIRED | 批准 2026-08-29 | **CLOSED·已结** | [260829-arch-review-kidpal-spec-stories](../changes/arch/260829-arch-review-kidpal-spec-stories/) | 无（evidence 台账见该 change 包 architecture-gate-evidence.md） |
| CDQ-260902-shim-api-field-envelope | 2026-09-02 | `interface-contract.md` 方向 B 重写：§三改推送语义（mcp kidpal/alert + 渲染回执出队）+ 新增「端云协议剖面」节（建议 §四）+ §1.5 收敛 + E1 control 扩 start_recording + B1 frontmatter `remind_before_min` 处置（packet v2 已落，旧 HTTP 信封版保留为 superseded） | ACCOMPANIES_SPEC_CHANGE（① 随 spec 改动携带；原判② 独立，前提被方向 B 作废） | 四题已裁（2026-09-03）：①批准方向 B 重写②确认 WS+Opus③确认强制鉴权+渲染回执出队+speech④remind_before_min 标废止 | **CLOSED·已结**（2026-09-03 原子落地） | [260902-arch-review-shim-api-field-envelope](../changes/arch/260902-arch-review-shim-api-field-envelope/) | 无——契约重写与 spec v0.2 FROZEN 同 PR 原子落地（human 一次性放行，记录见该目录 architecture-gate-evidence.md §12） |
