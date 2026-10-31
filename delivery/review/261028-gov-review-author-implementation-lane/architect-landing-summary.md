# architect-landing-summary · 261028-gov-review-author-implementation-lane

packet-id：FROZEN PROPOSAL（v1.0）§2 改法集（T1–T6，manifest 26 件）+ §5 验证集 · 评审轮次 R1–R5 设计评审 + R6 真实实现评审 GREEN · change-id gov-review-261028-gov-review-author-implementation-lane

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 双 landing lane | `AGENTS.md`、`docs/DEV-GOVERNANCE.md`、gov-change command/reference、arch-change、arch-review-protocol、architect-operating-model、review-moderator、adjudicator、merge-to-main、gov-arq | 缺省 packet-bound；纯 scripts/hooks 满足条件时 author-implementation；混合面及其余敏感面恒旧通道 |
| T2 最小写权与分支拦截 | architect def、scope-map、surface-alignment fixture、enforce-scope、release-pr 注释 | architect 仅新增 scripts/hooks；保留自改/评审面/G1/settings/scope-map deny 与两份独立凭据 deny；main/detached/跨树/直接 git 检查落地 |
| T3 模板兼容 | PROPOSAL 模板、landing-summary 模板、assemble-edits 注释 | 新 lane 冻设计与行为/测试约束，旧 lane 逐字与装配能力保留 |
| T4 独立评审与验证 | architect-reviewer、landing-verifier、track-b judge/verifier briefs | reviewer 审真实 diff 并钉 review JSON；verifier 独立同树/消费层验证并钉 verification JSON |
| T5 landing 结构门 | landing-fidelity-check、governance-gate-shared | lane/JSON fail-loud 单源；新 lane 核 Git tree、manifest、工作树漂移，旧 lane 原行为保留 |
| T6 合并闸树绑定 | pre-merge-escalation-gate | reviewed/verified commit/tree 重算、祖先关系、changedFiles==manifest、证据尾封闭、旧 SHA/rebase/越界 fail-closed |

## 2. 验证证据

- **语法与结构**：四个核心脚本 `node --check` 全过；scope-map 与 surface-alignment `JSON.parse` 全过；preflight `ok:true / anchorCount:26 / findings:[] / advisories:[]`；26/26 manifest 文件均有实际 diff；`git diff --check` 无冲突标记。
- **enforce-scope**：`--self-check` **19/19**（8 项既有 + 11 项新增）。新增 main/detached/跨树写、gov 非 main 放行、直接 git add/commit、git -C、两份凭据禁写等消费层 case。
- **pre-merge**：`--self-check` **96/96**。含 evaluate 消费层正反 case，以及真实临时 Git 仓中 pre-merge 与 landing-fidelity 双 CLI happy path、旧 SHA、越 manifest、错 tree。
- **landing-fidelity**：旧 12 组自检全绿，新增 `author-lane-structure` **11/11**（缺省/显式/未知/冲突 lane、证据尾边界、凭据 malformed/坏 SHA）。
- **独立 reviewer R6**：真实 diff GREEN；其自建黑盒夹具验证双门 happy、debt 尾 BLOCK、rebase 旧 SHA BLOCK，并做 enforce-scope 真树 14/14 探针。
- **回归**：assemble-edits **16/16**；route-regression **104/104**；lint-descriptions **0 flagged**；settings-lint **0 RED / 0 WARN**；release-pr **26/26**。
- **R6 后顺手修订**：F13 的 arch-review-protocol 定稿段三处绝对句与 F14 的 Track A 派遣句已同步为按 lane 分流；未改变实现判据。

## 3. 诚实注记

1. **本批实际执行方式**：R5 后 human 明确指示主线程直接实施，跳过继续起草循环；本批按 packet-bound 记录，scope-map 直接落稳定终态，没有执行此前临时十件写权/三态恢复方案。该一次性流程跳过已在 PROPOSAL §5.4/§7 留痕，独立 reviewer 与 verifier 不豁免。
2. **新通道尚未由本批自证日常使用**：本批实现并测试 author-implementation 机械面，但自身不消费新 lane；P9 才是首个 scripts 试点，首个 hooks 实证另行记录。
3. **F15 留根**：Bash 不是完整 shell 沙箱；`git -C<path>`、`--work-tree=<path>`、`cd <树> && git add` 等子命令形态仍有绕过同树检查的残余面。现有兜底 = Write/Edit 与直接 git 检查、角色禁令、合并闸 Git 重算与 landing-verifier；不宣称已封死所有进程内写入，后续可专项收紧。
4. **闸调用位置**：`collectAuthorImplementationGate` 沿 buildCtx 既有约定从仓根执行 Git 命令；不是新增限制，但新 lane CLI 使用须遵守同一约定。
5. **临时起草件**：本批 `_scratch/` 是此前起草期产物，落地 commit 前删除；新增防线均落在 manifest 内常驻 `--self-check`，不依赖 `_scratch` 才能通过。
6. **R6 非阻断项**：F13/F14 已修；F15 按上条留根，无未闭合 BLOCKING/SHOULD。

```landing-manifest
AGENTS.md
docs/DEV-GOVERNANCE.md
.claude/commands/gov-change.md
.claude/references/gov-change.md
.claude/references/arch-change.md
.claude/references/arch-review-protocol.md
.claude/references/architect-operating-model.md
.claude/agents/review-moderator.md
.claude/agents/adjudicator.md
.claude/skills/merge-to-main/SKILL.md
.claude/skills/gov-arq/SKILL.md
.claude/agents/architect.md
.claude/scope-map.json
.claude/route-fixtures/surface-alignment.json
.claude/hooks/enforce-scope.mjs
.claude/scripts/release-pr.mjs
docs/templates/PROPOSAL.template.md
docs/templates/architect-landing-summary.template.md
.claude/scripts/assemble-edits.mjs
.claude/agents/architect-reviewer.md
.claude/agents/landing-verifier.md
.claude/references/track-b-judge-brief.md
.claude/references/track-b-verifier-brief.md
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
```
