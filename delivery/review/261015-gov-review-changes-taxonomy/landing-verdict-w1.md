# landing-verdict-w1 · 261015-gov-review-changes-taxonomy（landing-verifier 独立核验）

verdict: PASS + 3 ADVISORY

**核验范围 = 波段一 7 件 + §5 V1–V3，全量 manifest 对账留波段二终态**——本 verdict 为波段一范围限定核验（FROZEN §0 两波程序变体），不冒充全量保真；其余 75 件未落地属波段设计态。

- packet-id 核对：architect-landing-summary-w1.md packet-id 行 = FROZEN PROPOSAL v0.4 §2 T1 改法 1–4 + §5 V1–V3 波段子集 · R4 GREEN · change-id 261015-gov-review-changes-taxonomy——与 FROZEN 头部 packet-binding 行、§7 R4 GREEN 一致。
- 结构门（波段一适配）：`landing-fidelity-check.mjs` 全量对账本轮不跑（FROZEN §0 明文，波段一期间 75 件未落地属波段设计态）；`--preflight` 已跑作锚形态参考（非判定面）：anchorCount = 66，§2 锚清单形态可解析、advisories 空、findings 空，与 FROZEN T6 第 5 条现态口径（§2 并集 66 / manifest 82 / 缺口 16 新树锚）吻合。

## 六项核验结论

1. **逐文件 diff 对照 — ok（7/7）**
   - 4 件 taskpackages 制品（review-packet.md / supervisor-verdict.json / pod-retro.md / docs-delta.md）：`git diff --cached -M` 实测 0 insertions / 0 deletions = 纯 rename、零内容改动，目标路径与 FROZEN manifest :17–20 逐字一致；源 `delivery/changes/dev/` 全树不在场（`test ! -d` 实证）。
   - `.claude/scope-map.json`：`universal.denyCreate` 增 `delivery/changes/dev/**` + `_doc` 废止位注记（含 261014 批撤销语义 + 「任何写入即违规」）——T1 改法 2 落到位（denyCreate 单机制承载，与改法原文机制注记一致）。
   - `.claude/scripts/pre-merge-escalation-gate.mjs`：新增 `RETIRED_MERGE_PATHS` + `retiredPathProbe`（复用 `git merge-tree --write-tree` 调用面 + `ls-tree`，execFn 可注入，探测异常 fail-closed）+ `evaluate` 废止路径 BLOCK 分支（消息点名残件路径 + 归位 taskpackages 指引）+ `buildCtx` 接线（source vs main）+ 头注例外清单条目——T1 改法 3 落到位；self-check 增 retired-path-merge-block / retired-path-probe-fail-closed-block / retired-path-clean-pass / retired-probe-fn-clean / retired-probe-fn-dirty 五臂，改法原文「两臂（残件 BLOCK / 干净 PASS）+ execFn 注入两态」全部在册且为加法强化。
   - `.claude/scripts/brief-inputs.mjs`：:759 夹具 planRef `delivery/changes/dev/x/PROPOSAL.md` → `delivery/changes/arch/x/PROPOSAL.md` + 行尾注记——T1 改法 4 落到位（现行合法形态；波段二随 T6 定稿 review 路径，与改法原文括号注记一致）。
   - 超出 manifest 的 canonical 改动逐一质询：`delivery/changes/arch/INDEX.md` 增一行 frozen 行 = 状态转移 `--upsert` 既有程序制品（AGENTS.md 治理路由节义务），非本批 packet 语义改动；`.changes-index-state.json` numstat = 0 行（upsert 重写致 EOL churn，无内容变化，merged 缓存行按 `upsertArchRow` :391 只在 merged=yes 时写、frozen 态无 state 行属设计）；`.codebase-memory/` 两件 = 工具自刷新派生面（治理规则不追交）。**无私货**。
2. **packet-binding 一致 — ok**：summary 围栏块 7 行与 FROZEN manifest :14–20 逐行逐序一致；summary 骨架 = `docs/templates/architect-landing-summary.template.md` 五件组件齐（packet-id 行 / §1 对应表 / §2 验证证据 / §3 诚实注记 / manifest 围栏块）；landing 实际范围 ⊆ 被批 packet（T1 改法 1–4 全部落位，无 T2–T6 提前落地物——git status 全量核对在案）。
3. **验证集复跑 — ok（全部亲跑，输出与 summary 声称一致）**：
   - V3 ①：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → `{"selfCheck":"pre-merge-escalation-gate","pass":48,"fail":0,"total":48,"ok":true}` + changeid-prefix 4/4（基线 43 + 新增 5 臂，算术吻合）。
   - V3 ②（真实分支）：`node .claude/scripts/pre-merge-escalation-gate.mjs 261015-voice-rightcol-label-overflow --source-ref dev/pod-m2` → exit=1，BLOCK 单因 `retired-path-merge-block：合并结果树含废止路径残件 1 条（040000 tree eb121c95…	delivery/changes/dev）——旧布局分支带入；归位 delivery/taskpackages/ 同 id 目录后重跑…`——与 summary 引文逐字一致；对照跑 `--source-ref main` 亦 BLOCK（HEAD 尚含旧路径残件，commit 前现态，预期一致）。
   - V2：verifier 会话亲写探针 `delivery/changes/dev/probe.md` → PreToolUse:Write 钩子拦截，原文逐字 `scope(universal): 禁止写 delivery/changes/dev/probe.md（.claude/scope-map.json universal.denyCreate——影子目录或受保护区）。`；探针未落盘（`ls delivery/changes/dev` → No such file or directory）。
   - V1（working tree 形态）：四件 `test -f` 无 MISSING；`test ! -d delivery/changes/dev` 成立。**V1 判据时点 = post-commit**，working tree 预跑通过、post-commit 复证留待 summary §2 append（summary 诚实注记 4 已声明）。
   - 附带：`brief-inputs.mjs --self-check` → 43/43 passed SELF-CHECK GREEN；`settings-lint.mjs` → 3 files, 0 RED, 0 WARN。
4. **结构门 — 波段一范围限定、全量门留波段二**：按 FROZEN §0 适配点处理，不因全量门未跑而 FAIL；`--preflight` 锚形态参考通过（见头部）。清单第 4 项如约记「波段一范围限定、全量门留波段二」。
5. **真空绿规约 — ok**：重放类先断言输入——gate 夹具 dirty 臂 ls-tree 输出非空（含 `delivery/changes/dev/` 字面路径）、clean 臂显式空，反转实现（忽略 ls 输出）必翻 dirty 臂红，夹具能判别；接线类验证标准满足——真实分支 BLOCK 输出含新增自检名 `retired-path-merge-block`，`buildCtx → retiredPathProbe(execFn, source, 'main') → evaluate(ctx.retiredProbe)` 调用顺序经源码实读在册（buildCtx :446 区、evaluate :344 区、mkCtx 默认干净 retiredProbe 保 43 旧臂语义不变）；环境红基线——FROZEN §5 基线 43/43 在册，48−43=5 全为新增臂，无把环境红误归因改动的情形。
6. **诚实披露核对 — ok（4 条全部与实际一致，无该记未记）**：
   - 注记 1（V2 措辞差距）：亲跑拦截原文含 `universal.denyCreate`、不含「废止注记」字样（enforce-scope 不读 `_doc` 入消息）——与披露一致；废止注记本身在 scope-map `_doc` 在册，机制（拦截写入）经探针亲证无损。
   - 注记 2（占位件）：`landing-verdict.md` 实读确认**不含任何 verdict token 行**（无行首 `verdict:` 行），changes-index `deriveLanded` 读不到 PASS token，无 premature landed 面；占位件与 -w1 命名为 human 2026-10-16 已裁适配点，不属偏离。
   - 注记 3（planRef 中间态）：diff 实际值 `delivery/changes/arch/x/PROPOSAL.md`，与披露一致，且为 T1 改法 4 原文括号注记的照录执行。
   - 注记 4（V1 时点）：summary V1 为 working tree 预跑、post-commit 复证 append——与本轮亲核一致，时点如实。

## ADVISORY 清单（非阻断观察）

- **A1 · scope-map `_doc` 波段二预告行超出 T1 改法 2 字面**：`_doc` 第二新增行（「denyCreate 条目 delivery/changes/**（伞级扩展）随本批…波段二（T6 齿一）落地——伞解散后真家见 delivery/archive/README.md 旧→新映射」）不在 T1 改法 2 授权文本内。定性：doc-only（enforce-scope 不消费 `_doc`，零强制行为）、summary §1 已如实披露（「+ 波段二伞级扩展预告行」）、内容与同一 FROZEN packet 的 T6 第 6 条（波段二齿一扩展 + 注记更新）一致——判 ADVISORY 不判 FAIL；建议波段二落地时随 T6 注记更新一并改写，避免中间态文本残留。
- **A2 · summary V3 ① 「输出含全部新增自检名」措辞不准**：`--self-check` pass 态输出为纯计数 JSON（`failures` 字段仅 fail 态出现，pre-merge-escalation-gate.mjs :678–681 实读）；新增自检名的实际在场证据 = 真实分支 BLOCK 输出点名 `retired-path-merge-block` + 48−43=5 算术 + 源码级五臂断言实读。实质满足真空绿规约，仅措辞建议波段二 summary 收敛为「pass 态计数 + 失败态点名 + 真实分支 BLOCK 点名」三层表述。
- **A3 · staging 不对称（瞬态）**：4 件 rename 已 staged、3 件 `.claude` 修改未 staged（git status 实测）。非保真缺口——提交前主线程按 Hard Rules `git add` + `git diff --cached --stat` 复核即可。

## 亲核留痕（命令 + 关键输出）

| 命令 | 关键输出 |
|---|---|
| `git diff --cached -M --stat` | 4 件 rename，0 insertions / 0 deletions |
| `git diff -- .claude/scope-map.json` `.claude/scripts/pre-merge-escalation-gate.mjs` `.claude/scripts/brief-inputs.mjs` | 三件内容修改，逐行比对 FROZEN §2 T1 改法 2–4（见第 1 项） |
| `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | `{"pass":48,"fail":0,"total":48,"ok":true}` + changeid-prefix 4/4 |
| `node .claude/scripts/pre-merge-escalation-gate.mjs 261015-voice-rightcol-label-overflow --source-ref dev/pod-m2` | exit=1，BLOCK 单因 retired-path-merge-block（引文逐字吻合） |
| Write 探针 `delivery/changes/dev/probe.md` | PreToolUse:Write 拦截，原文逐字吻合 summary 引文；探针未落盘 |
| `for f in …; do test -f …; done` + `test ! -d delivery/changes/dev` | 无 MISSING；ABSENT |
| `node .claude/scripts/brief-inputs.mjs --self-check` / `node .claude/scripts/settings-lint.mjs` | 43/43 GREEN / 3 files 0 RED 0 WARN |
| `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/261015-gov-review-changes-taxonomy/PROPOSAL.md` | ok:true，anchorCount=66，advisories []，findings [] |
| `git diff` INDEX/state-cache + `upsertArchRow` 源码实读 | INDEX 单行 frozen upsert；state cache 0 行内容变化（EOL churn） |

## 结论

波段一 7 件实际落地 == FROZEN PROPOSAL v0.4 §2 T1 改法 1–4，验证集 V1–V3 机械证据亲跑复现，诚实注记 4 条与实际一致，无私货、无与批定矛盾、敏感面（scope-map 齿一 / 闸齿二）改动逐字核对通过。**放行建议：PASS——可进 commit + 前置升级闸 + 合并本地 main（合并归主线程程序，本 verdict 不替代 human merge 门）**；3 条 ADVISORY 随件流转，A1/A2 由波段二 landing 消化。
