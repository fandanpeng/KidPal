---
id: ARQ-260924-landing-gate-dir-anchor-gap
title: landing-fidelity 门的落地探测是逐文件精确匹配、不消费目录锚——而 preflight 锚提取接受目录 token，两门口径不一致；迁移类批次写目录简写必在收口期炸
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下次动 landing-fidelity-check / preflight / governance-gate-shared 解析面的治理批 → **2026-09-25 部分承载（批 C）**：处方①③ 已落（preflight 输出增 `advisories` 目录 token 点名 + PROPOSAL.template manifest 说明行补「一行一个文件路径」纪律句）；**② 维持不预批**——触承重闸判定逻辑（landing 门目录前缀展开），须独立批完整评审；下批该面承接者按此二分
opened: 2026-09-24
closed: ""
closedBy: ""
paths: [.claude/scripts/landing-fidelity-check.mjs, .claude/scripts/lib/governance-gate-shared.mjs]
gateLayers: G3
hardExclusions: true
source: [delivery/changes/arch/260923-gov-review-arq-ssot-restructure/（FROZEN 后首跑结构门实录 + architect-landing-summary §3 格式例外注）]
---

**证据**（260923-gov-review-arq-ssot-restructure landing 期实录，2026-09-24）：该批 PROPOSAL manifest 末行写目录简写 `docs/crq/`（v0.3 起草期 preflight 干跑曾实测该 token **入锚并集**——`anchorUnion` 含 `docs/crq/`，当时判为可用形态）。FROZEN 后首跑 `node .claude/scripts/landing-fidelity-check.mjs <change-dir>` 即 RED 两条：「claimed-but-not-landed：docs/crq/」+「planned-but-not-landed：docs/crq/」——根因经读源码核实：`gitChangedFiles()` 以 `git status --porcelain -uall` **逐文件**枚举（注释自述「-uall：untracked 文件逐个列出（默认把未跟踪目录折叠成目录条目 → 新目录内新增文件会被误判 not-landed；本批 dogfood 发现并修）」——该修复恰证明此面踩过同族坑），而 manifest↔改动集比对是 `changed.has(t)` **精确集合成员判定**（`landing-fidelity-check.mjs:613-615` 一带），目录路径永不命中任何文件行。即：**preflight（起草期）接受目录锚、landing 门（收口期）不接受**——错误反馈延迟到 FROZEN 之后才出现，此时修 manifest 已构成格式例外（本批按 260913-arch-review-spec-split 先例走 human 追认 + 机械展开 99 行，见该批 PROPOSAL manifest 下注）。

**影响面**：凡迁移/新建目录类批次（本批 99 件、260914-gov-review-delivery-restructure 先例 139 行全枚举）都会在两个门之间遇到口径差：起草期干跑绿 → 收口期结构门红 → 走格式例外追认或返工。频率 = 每个迁移类批次一次；成本 = 一次 FROZEN 后例外流程 + 判官/核验官额外核对一轮。

**建议改动**（候选，交 review，不自批）：① 最小修 = **preflight 增一条 advisory**：锚并集中出现「以 `/` 结尾的目录 token」即提示「landing 门不消费目录锚，manifest 须逐件枚举（迁移批先例：260914 批 139 行）」——把反馈提前回起草期，不动承重门判定；② 或 landing 门对目录锚做前缀展开（改 `landing-fidelity-check.mjs` 匹配逻辑——**触承重闸，须独立治理批 + 完整评审**，本条不预批该档）；③ 或仅在 `PROPOSAL.template.md` 的 manifest 说明句加「一行一个**文件**路径（目录须展开为成员枚举）」纪律句（最便宜，纯模板面）。倾向 ①+③ 组合：反馈前移 + 纪律成文，均不触门判定语义。

**风险**：riskTier: low（判据口径差，非行为缺陷；现行为 fail-loud 不假绿）。review profile：Track B 三要件 = ① 执行测出 yes（本条即实录）；② 修复明确 **部分**（①③ 明确、② 触承重闸有真分叉）；③ 半径已被执行揭示 yes（迁移类批次收口期必现）。G 层（由 paths 机推）：G3（`.claude/scripts/`）；候选 ③ 另触 `docs/templates/`（G2 邻接）。机检硬排除：不触 contracts；①③ 不改任何门判定语义，② 若做即「改工程门」→ 须标 full。验证计划：preflight 对含目录锚的样例 PROPOSAL 出 advisory（新夹具反转即红）+ 模板句落地后 `landing-fidelity-check --preflight` 全绿回归。owner=architect。

**批 C 起草期实测复证（2026-09-24，260924-gov-review-gate-parse-teeth 起草前）**：处方落点仍在 + 现行为干跑实证——① preflight 对含目录锚样例（仓外临时夹具，锚行含 `docs/crq/` 与 `.claude/scripts/landing-fidelity-check.mjs` 两 token）现行为 = `anchorUnion` 收目录 token、`ok:true`、`findings:[]`、exit 0：目录 token **静默通过**起草期；机制面 = `ANCHORABLE_TEST`（governance-gate-shared.mjs:40）为前缀正则、`docs/crq/` 必命中。② landing 门精确匹配核实现状：landing-fidelity-check.mjs:613（claimed-but-not-landed）/:615（planned-but-not-landed）均 `changed.has(t)` 集合成员判定，目录路径永不命中 `git status --porcelain -uall` 的逐文件行——两门口径差维持原状。③ 处方落点细化：advisory 齿落 `preflightFindings`/`runPreflight`（:227-266）；**设计要点**——现 `ok = findings.length === 0` 且 exit code 联动，目录 token 齿若入 `findings` 会把 preflight 变红（向严、超「advisory 不拦门」处方），倾向独立 `advisories[]` 输出键（不动 ok/exit）——具体形态交评审裁；夹具判别性要求 = 含目录 token 样例 advisory 非空、纯文件 token 样例 advisory 为空（反转实现必须变红）。④ 处方 ③（模板 manifest 纪律句）落点 PROPOSAL.template.md 与批 B retro §4-2/§4-5 载体候选（围栏承载条款 / §5 逐子句时点，均已追注入 ARQ-260914）**同文件面**，收进同批时合并设计一次改齐。② 档（landing 门前缀展开）触承重闸判定逻辑，维持本条自注「不预批、若做须独立完整评审」。

**部分承载（2026-09-25，260924-gov-review-gate-parse-teeth T9，本条不结）**：处方①③ 落地——preflight 输出增 `advisories` 键（`dirTokenAdvisories` 纯函数：锚并集中以 `/` 结尾的目录 token 逐个点名，文案含「landing 门逐文件精确匹配 changed.has、manifest 须逐件枚举成员」与 260914 批先例）；**ok/exit 零变更实证**：目录样例 ok:true / exit 0 / advisories 1，纯文件样例 advisories 0（判别性双向），landing-fidelity `--self-check` 19/19（含新增 2 判别 case）。PROPOSAL.template.md manifest 说明行补纪律句「一行一个**文件**路径——目录须展开为成员枚举（landing 门不消费目录锚，目录简写收口期必红）」（与批 B §4-2 围栏承载条款 / §4-5 逐子句时点 / ARQ-260914 追注 (B) 第三句同批合并设计、同一文件面一次改齐）。**处方② 维持不预批**（触承重闸判定逻辑，须独立批完整评审）。证据：本批 PROPOSAL §2 T9 + §5 V9 + landing-summary。
