# architect-landing-summary-w1 · 261015-gov-review-changes-taxonomy

packet-id：FROZEN PROPOSAL v0.4 §2 改法集（T1，改法 1–4）+ §5 验证集（V1–V3 波段子集）· 评审轮次 R4 GREEN（熔断后新判官实例）· change-id 261015-gov-review-changes-taxonomy

**范围声明（不冒充全量保真）**：本件为波段一中间件——核验范围 = 波段一 7 件（下方 manifest 围栏块 = FROZEN manifest 82 行中的波段一子集）+ §5 V1–V3 机械证据。结构门 `landing-fidelity-check.mjs` 的全量 manifest 对账只在波段二终态跑（届时产 `architect-landing-summary.md` / `landing-verdict.md` 终态件，核验范围 = 波段二落地 + 全 manifest 终态）；波段一期间其余 75 件未落地属波段设计态（PROPOSAL §0 落地排序与依赖节）。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2 T1） | 落地文件 | 结果 |
|---|---|---|
| T1 改法 1 · 归位 | `delivery/taskpackages/261015-voice-rightcol-label-overflow/`（review-packet.md / supervisor-verdict.json / pod-retro.md / docs-delta.md） | 四件 `git mv` 归位，零内容改动；与既有包件零文件名冲突（落地前实测复核）；空树 `delivery/changes/dev/` 已 rmdir |
| T1 改法 2 · 齿一 | `.claude/scope-map.json` | `universal.denyCreate` 增 `delivery/changes/dev/**`；`_doc` 增废止位注记（dev lane 已由 261014 批撤销，任何写入即违规）+ 波段二伞级扩展预告行 |
| T1 改法 3 · 齿二 | `.claude/scripts/pre-merge-escalation-gate.mjs` | 新增 `RETIRED_MERGE_PATHS` + `retiredPathProbe`（复用 merge-tree --write-tree 结果树 + ls-tree，execFn 可注入，探测异常 fail-closed）+ `evaluate` 通用 5 检查（残件 BLOCK 消息带归位指引）+ `buildCtx` 接线（两档通用）+ 头注例外清单第 7 条 |
| T1 改法 4 · fixture 顺修 | `.claude/scripts/brief-inputs.mjs` | self-check 夹具 `planRef` 由 `delivery/changes/dev/x/PROPOSAL.md` 改 `delivery/changes/arch/x/PROPOSAL.md`（现行合法形态；随 T6 波段二定稿为 `delivery/review/x/PROPOSAL.md`，与改法原文一致） |

## 2. 验证证据

- **V2 · deny 双态实测（波段一态）**：落地会话 Write 探针 `delivery/changes/dev/probe.md` → PreToolUse 钩子拦截，拦截原文逐字：`scope(universal): 禁止写 delivery/changes/dev/probe.md（.claude/scope-map.json universal.denyCreate——影子目录或受保护区）。` 探针未落盘（`delivery/changes/dev` 全树不在场）。
- **V3 · 合并闸 BLOCK/PASS**：① `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → `{"selfCheck":"pre-merge-escalation-gate","pass":48,"fail":0,"total":48,"ok":true}`（基线 43 + 齿二新增 5 臂：retired-path-merge-block / retired-path-probe-fail-closed / retired-path-clean-pass / retired-probe-fn-clean / retired-probe-fn-dirty，输出含全部新增自检名）；② 真实旧布局分支实跑：`node .claude/scripts/pre-merge-escalation-gate.mjs 261015-voice-rightcol-label-overflow --source-ref dev/pod-m2` → `exit=1`，BLOCK 理由逐字：`retired-path-merge-block：合并结果树含废止路径残件 1 条（040000 tree eb121c95ab583c7b868534897678a9e29b5e708c	delivery/changes/dev）——旧布局分支带入；归位 delivery/taskpackages/ 同 id 目录后重跑（dev lane 已由 261014 批撤销，真家映射见 delivery/archive/README.md）`；off-main 未触发（单因证据，dev/pod-m2 与 main diverged，与 T1 证据「在途分支」一致）。
- **V1 · 搬移对账（working tree 预跑）**：`for f in review-packet.md supervisor-verdict.json pod-retro.md docs-delta.md; do test -f ... || echo MISSING; done` → 无 MISSING（四件全在位）；`test ! -d delivery/changes/dev` → 在场性否定成立。**post-commit 复证将 append 本节**（V1 判据时点 = post-commit）。
- **附带机械检查**：`brief-inputs.mjs --self-check` → 43/43 passed，SELF-CHECK GREEN；`settings-lint.mjs` → 3 files, 0 RED, 0 WARN（scope-map 改动后复跑）。

## 3. 诚实注记

1. **V2 措辞差距**：拦截信息含 `universal.denyCreate` 字样；「废止注记」字样落在 scope-map `_doc` 条目文本，denyCreate 拦截消息为通用形态（enforce-scope.mjs 不读 `_doc` 入消息）。PROPOSAL V2 预期「拦截信息含 universal.denyCreate 与废止注记字样」中后件由 _doc 在册承载、消息本体不含——机制（拦截写入）无损，措辞差距如实登记。
2. **landing-verdict.md 占位件（human 2026-10-16 裁「占位件方案」）**：FROZEN §0 制品清单未含波段一 `landing-verdict.md`，但闸 gov 档机锚 `readGovAnchors` 以 `existsSync(landing-verdict.md)` 判档 6，与「-w1 中间件」设计互斥（评审四轮未覆盖）。human 裁定：波段一另产 `landing-verdict.md` 机锚占位件——内容仅范围声明、**不含任何 verdict token 行**（changes-index deriveLanded 读不到 PASS token，无 premature landed 面）；波段二终态 verdict 重写本文件。本偏差随本注记留痕，FROZEN PROPOSAL 文本不改。
3. **brief-inputs planRef 中间态**：波段一定为 `delivery/changes/arch/x/PROPOSAL.md`（现行合法形态），波段二随 T6 改 `delivery/review/x/PROPOSAL.md`——T1 改法 4 原文「落地时随 T6 定稿」照录。
4. **V1 时点**：上表 V1 为 working tree 预跑；commit 后复证 append 本件后随 verdict-w1 交 landing-verifier 复核。

```landing-manifest
.claude/scope-map.json
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/brief-inputs.mjs
delivery/taskpackages/261015-voice-rightcol-label-overflow/review-packet.md
delivery/taskpackages/261015-voice-rightcol-label-overflow/supervisor-verdict.json
delivery/taskpackages/261015-voice-rightcol-label-overflow/pod-retro.md
delivery/taskpackages/261015-voice-rightcol-label-overflow/docs-delta.md
```
