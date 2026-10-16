# architect-landing-summary · 260924-gov-review-settings-deny-contract-merge

packet-id：FROZEN PROPOSAL v0.4 §2 改法集（T1–T4）+ §5 验证集 · 评审轮次 R1 BLOCK(F1–F6) → R2 BLOCK(R2-F1) → **R3 GREEN**（同判官三轮收敛，未触熔断，升级 human 零项）· change-id 260924-gov-review-settings-deny-contract-merge

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1-i deny 段删无效 `Write(./docs/contracts/**)` 行（OQ-1=(a) human 批准凭据在 PROPOSAL Status 行） | `.claude/dev-loop.settings.json` · `.claude/review-loop.settings.json` | 两文件 deny 段现各仅剩 `Edit(./docs/contracts/**)` 行；JSON 解析合法；契约冻结三件承载（Edit 行 + scope-map bashDenyWrite + enforce-scope）零触碰 |
| T1-ii `_doc` 防再加回注记 | 同上两文件 | 各插入一行「勿再加回」注记（dev-loop 版含 settings-lint RED 指针；review-loop 版句尾不指 lint，按 R1-N2 澄清后的括注落地） |
| T2-i 新建 settings 合法性静态 lint（OQ-2=(a)） | `.claude/scripts/settings-lint.mjs`（新增） | 按 FROZEN 规格实现：扫描面机械枚举 / `Write(`→RED / `Read(`→WARN / 解析失败→RED / 退出码 0-1-2 / self-check 输出契约四件（bad-、good- 前缀夹具 + 逐例行含名与 ok + 尾行单行 JSON + 任一例红 exit 1）；不入 verify-gates 闸族 |
| T2-ii launch-prep 步骤 2 接线 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | 「settings 合法性机检」段插入步骤 2 内（check-launch 判停句之后、「上游已知红」段之前）；文本 == FROZEN §2 T2-ii 插入段（F6 校准版：实证面口径）；frontmatter 零变更 |
| T3-i §10e 契约包人合 runbook（OQ-3=(a)） | `.claude/references/pod-coordination.md` | 新节插入 `## 11. 锚 3 ·` 锚行之前；四步全文 == FROZEN §2 T3-i（含 R1-N1 步骤 3 限定句与 R2-F1 修订后的步骤 2 括注）+ 边界与留根段 |
| T3-ii 升级闸通用 1 reason 行加 §10e 指针 | `.claude/scripts/pre-merge-escalation-gate.mjs` | :264 整行替换 == FROZEN §2 T3-ii 替换文本；判定触发条件与 BLOCK 结果零变更（self-check 33 例前后全绿） |
| T3-iii merge-driver Step 1b 条件指针行 | `.claude/scripts/merge-driver.mjs` | :1276 之后插入一行 == FROZEN §2 T3-iii 围栏 block1 逐字（条件子串 = 通用 1 前缀 `docs/contracts/** diff 且同批无契约门证据`，:275 通用 4 不误触发） |
| T4 账本写回 | `docs/crq/gov/_archive/2026/ARQ-260924-settings-write-deny-rule-cli-drift.md` · `docs/crq/gov/_archive/2026/ARQ-260923-contract-merge-deadpath.md` · `docs/crq/gov/INDEX.md` · `docs/crq/gov/_archive/2026/INDEX.md` | 两条目各 append 处置结项段（追注段原文保全）+ frontmatter `status: closed` / `closed: 2026-09-24` / `closedBy` 含 OQ 裁定；crq-render 自动归位 `_archive/2026/` + INDEX ×2 再生；crq-doctor RED 零命中 |

## 2. 验证证据

（评估时点：以下均为〔工作树〕landing 落盘后、commit 前实跑；命令逐字与关键输出逐字记录。V5 为〔commit 后〕项，见 §3。）

- **V1（T1）→ 绿**：`grep -cF 'Write(./docs/contracts/**)'` 两文件各 → **0**；`grep -cF 'Edit(./docs/contracts/**)'` 两文件各 → **1**；`grep -c '勿再加回'` 两文件各 → **1**（F5 双侧齿均命中）；`node -e "JSON.parse(...)"` 两文件各 → `json ok`。
- **V2（T2）→ 绿**：`node .claude/scripts/settings-lint.mjs --self-check` → exit 0，尾行逐字 `{"selfCheck":"settings-lint","pass":6,"fail":0,"total":6,"ok":true}`（固定 token `"selfCheck":"settings-lint"` 与 `"fail":0` 在场；bad- 前缀例 3 条全 ok = 判别性成立，反转检出逻辑必红）；`node .claude/scripts/settings-lint.mjs` → exit 0，尾行 `settings-lint: 3 files, 0 RED, 0 WARN`（T1 落地后全 clean）；`grep -c 'settings-lint' .claude/skills/kidpal-pod-launch-prep/SKILL.md` → **1**。
- **V3（T3）→ 绿**：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → exit 0，尾行逐字 `{"selfCheck":"pre-merge-escalation-gate","pass":33,"fail":0,"total":33,"ok":true}`（== 起草期 B2 基线，判定语义零变更的机械证明）；`grep -cF 'pod-coordination.md §10e' .claude/scripts/pre-merge-escalation-gate.mjs` → **1**；`grep -cF '§10e' .claude/scripts/merge-driver.mjs` → **1**；`grep -c '10e. 契约包人合 runbook' .claude/references/pod-coordination.md` → **1**；四步关键词逐词 `grep -c` → `pre-merge 预检`=1 / `批准事件落账`=1 / `ff 与 no-ff 选定`=1 / `簿记等价代记`=1（基线均 0，判别性成立）。
- **V4（T4）→ 绿**：`node .claude/scripts/crq-doctor.mjs` → 尾两行 `advisory 142 条（不阻断）：…` / `RED 零命中 —— ok`；`docs/crq/gov/_archive/2026/` 两条目文件在场、`docs/crq/gov/` 顶层零命中（ls+grep 实测）；`docs/crq/gov/INDEX.md:10` 摘要行逐字 `**摘要**：未结 16（待拍板 0 / 待触发 14 / 已接受风险 2）；已结 70（归档索引见下）。`——算术与 FROZEN V4 预期（18−2=16 / 16−2=14 / 68+2=70）一致，landing 时点无他批动账本。
- **V6（回归面）→ 绿**：`node .claude/scripts/route-regression-check.mjs` → 尾行逐字 `route-regression-check: 28 ok, 0 fail`；`node .claude/scripts/lint-descriptions.mjs` → exit 0（flag 1 = landing-verifier.md over-length，基线既有、非本批触面）；`git diff -U0 -- .claude/skills/kidpal-pod-launch-prep/SKILL.md | grep -cE '^[+-](description|name):'` → **0**（frontmatter 零命中）。
- **diff 最小性**：`git diff --stat -- .claude/` → 6 files changed, 19 insertions(+), 5 deletions(-)（settings ×2 各 2+/2−、pod-coordination 11+、merge-driver 1+、gate 1+/1−、SKILL 2+）+ 新增 settings-lint.mjs（untracked，不入 diff stat）——无 manifest 外触面。
- **V0 preflight**（FROZEN 前已跑，v0.4 定稿态）：`landing-fidelity-check.mjs --preflight` → `"ok": true, anchorCount: 11, findings: []`；anchorUnion ≡ manifest 11 行。

## 3. 诚实注记

1. **换行形态（修订版注记，替换初版错误披露——landing-verifier A1 采信）**：六个既有 CRLF 目标文件在 landing 全程**保持纯 CRLF、未混入 LF 行**（机械证明：`git ls-files --eol` 六件均 `i/lf w/crlf` + 字节级 CR 计数 == LF 计数〔dev-loop 75/75、SKILL 138/138〕）。初版本节曾披露「Edit 工具将文件规范化为统一 LF」——该披露**不实**，根因 = 主线程用 msys awk（textmode 剥 `\r`）测得假 LF 读数、且未交叉第二信号源即归因写入（ARQ-260914「工具负结果先交叉第二信号源」教训复发，retro 自认入账）。FROZEN §2「landing 改动保持 CRLF 不混入 LF 行」承诺在工作树层与 canonical 层**均直接成立**；新增文件 settings-lint.mjs 为 LF 形态（新文件无既有形态可保持，autocrlf=true 下 checkout 后转 CRLF）。
2. **landing-verifier F1 轮（如实记）**：首轮核验 FAIL(F1)——T2-ii 承重插入段落盘 SKILL.md 时漏抄 1 字符（「非 TTY **两**形态」脱「两」，FROZEN 490 vs 落地 489 codepoints，verifier 字符级 diff 机证）；本 summary §1 该行原申报「文本 == FROZEN 插入段」当时不实。修复 = 按 FROZEN PROPOSAL T2-ii 原文机械回贴，复证 node 字符级比对 `byte-equal: true`（490==490）+ V2 `grep -c 'settings-lint'`=1 + V6 frontmatter 零命中复跑绿，随后重派同核验官复核。教训入 retro（承重插入段落地后未即时 byte 对账 FROZEN 原文——批 A retro 第 4 条「锚文机械提取 + 落笔前 byte 对账」纪律在插入段的对偶面失守）。
3. **settings-lint 夹具实现期修正（如实记）**：self-check 首跑 6 例中 1 例红——初版夹具 `good-no-permissions`（`'{"hooks":{}}'`）不含 `permissions` 字样、与真空绿前置断言（夹具须含 permissions 段）冲突，被 self-check 当场检出（判别性义务的自证）；修正为 `good-permissions-no-arrays`（`'{"permissions":{},"hooks":{}}'`）后 6/6 绿。FROZEN 契约四件（bad-/good- 前缀命名、逐例行、尾行 JSON、例红 exit 1）未变，夹具具体名不在 FROZEN 钉死面（契约只钉前缀）。**A3 披露**：实现另含 `warn-` 前缀夹具（`warn-read-deny`）——FROZEN 钉 bad-/good- 两前缀，`warn-` 为无害扩展（WARN 级判定路径的判别夹具），契约四件不受损，verifier 判非阻断。
4. **FROZEN §5 V4 幂等子句的评估时点（A2 处置）**：V4 的「`crq-render.mjs` 复跑后 `git diff --exit-code -- docs/crq/` → exit 0」子句在 **pre-commit 时点恒 exit 1**（本批 landing 改动尚未 commit，工作树对 index 必有 diff——评估时点歧义，verifier 实测证实）；底层幂等性成立（verifier 复跑 crq-render 后 git status 与初始快照零变化）。处置：该子句按 FROZEN V4 标注的〔工作树 + commit 后复证〕双时点口径，以 **commit 后复跑**为准，输出记入 retro；不改 FROZEN 文本（冻结后不可回改），时点歧义教训入 retro（§5 断言标评估时点纪律的第三形态：双时点标注的子句须逐子句拆明哪个子句属哪个时点）。
5. **lint-descriptions 基线 flag**：`landing-verifier.md over-length (363 chars > 320)` 为基线既有（批 A V3 同记录在案），非本批引入、不处置。
6. **gate self-check 前置 `changeid-prefix` 行**（pass:4）为 lib 连带输出，B2 基线与 V3 的 33 例计数口径均不含它（起草期已声明）。
7. **V5 为〔commit 后〕项**：`changes-index.mjs --upsert` 于 commit 后跑（status → landed）；合并 main 后再跑一次翻 mergedInMain（memory 纪律，随下批 ff 或本批收口 commit 承载）。
8. **残留 / 降级 / 未落项**：无——T1–T4 全落、manifest 11 行与实际触面一致；留根项（真交互 TTY 观察、`Read(` WARN 级复证、建议① 证据接入触发器、headless 恢复前置）均在 FROZEN §6 与 §10e 边界段有去向，非本批未落项。

```landing-manifest
.claude/dev-loop.settings.json
.claude/review-loop.settings.json
.claude/scripts/settings-lint.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/references/pod-coordination.md
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/merge-driver.mjs
docs/crq/gov/_archive/2026/ARQ-260924-settings-write-deny-rule-cli-drift.md
docs/crq/gov/_archive/2026/ARQ-260923-contract-merge-deadpath.md
docs/crq/gov/INDEX.md
docs/crq/gov/_archive/2026/INDEX.md
```
