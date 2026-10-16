# Landing Summary · 260905-gov-review-verify-gates-align

> 落地人：主线程（Track B 步骤 4，packet-bound）
> 日期：2026-09-05
> packet-id = FROZEN PROPOSAL §2 改法集（T1–T5）+ §5 验证集；packet = FROZEN PROPOSAL + landing-manifest + 本 summary（architect-landing-summary）
> 被批 review id = 260905-gov-review-verify-gates-align（本目录；R1 需修订 → R2 GREEN，评审记录在 §7）

```landing-manifest
.claude/scripts/merge-driver.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/verify-gates.json
.claude/references/pod-coordination.md
docs/ARCH-REVIEW-QUEUE.md
changes/arch/260905-gov-review-verify-gates-align/PROPOSAL.md
```

> manifest 注：`docs/ARCH-REVIEW-QUEUE.md` 的 T3（ARQ 移已结 + 索引钉行）按 PROPOSAL 约定留待 landing-verifier 绿后执行，属预期状态不算缺口。

## 1. landing 清单执行对照

| manifest 路径 | 动作 | 落地内容 | 状态 |
|---|---|---|---|
| `.claude/scripts/merge-driver.mjs` | 改 | T1：`:403` `QUICK_CHECK_IDS` 源仓 4 门残留 → 本仓 2 门 `['contract-lint','acceptance']` + 头注释改写（firmware-build 有意不入冒烟的三条理由）；`:405-406` oracle 注释补 260905 对齐句（oracle 本体 2 条未动）；T1.5：`:687-688` 豁免例外注释与 `:704` warn 文案「其余 types/schema/contract-lint」→「其余 contract-lint」（含「260905 对齐两门案」注） | ✅ 已落 |
| `.claude/scripts/pre-merge-escalation-gate.mjs` | 改 | T4（OQ2=(a)）：`readGovAnchors` frozen 正则 `/^\*\*状态：FROZEN v\d+\.\d+/m` → `/^[>\s]*\*\*状态：FROZEN v\d+\.\d+/m`（容忍 `> ` 引用前缀，双形态命中；行内注记出处） | ✅ 已落 |
| `.claude/verify-gates.json` | 改 | T2：`_doc` 数组追加 3 行注（quick-check 冒烟子集两门案决策 + firmware-build 不入冒烟理由 + types 缺口照旧）；gates 数组零改动 | ✅ 已落 |
| `.claude/references/pod-coordination.md` | 改 | T5：`:72` quick-check 三门集名 → 两门案表述（含 firmware-build 不入冒烟理由括注） | ✅ 已落 |
| `docs/ARCH-REVIEW-QUEUE.md` | 改 | T3：ARQ-260905-verify-gates-types-missing 移已结 + 索引钉行——**landing-verifier 绿后执行**（PROPOSAL 约定时序） | ⏳ 待 verifier 绿 |
| `changes/arch/260905-.../PROPOSAL.md` | 改 | R1 修订（F1/F2+NIT×2：T5 增设、T1.5 折入、manifest 6 路径）+ 状态行 FROZEN v1.0 | ✅ 已落 |

manifest 外零落地：verify.mjs、merge-driver 其余合并链语义（rebase/重试/exemption/token）、DEV-GOVERNANCE、契约面均未触碰（PROPOSAL §0 非目标）。

## 2. 验证集重放（PROPOSAL §5，landing 后自检，2026-09-05）

- **V1 quick-check 自检转绿**：`{"selfCheck":"merge-driver-quick-checks","ok":true,"expectedCmds":["node .claude/scripts/contract-lint.mjs","node .claude/scripts/acceptance-run-check.mjs"],"gotCmds":[同],"expectedIds":["contract-lint","acceptance"],"gotIds":[同]}` ✅（got==expected 双断言全过）
- **V2 语法闸**：`node --check` 两脚本零输出通过 + verify-gates.json `JSON.parse` 合法 ✅
- **V3 既有自检回归**：`--self-check-detect-changes` 16/16 ✅、`--self-check-rebase-target` 7/7 ✅（本批后 merge-driver 自检三件套**首次 3/3 全绿**——V1 此前恒 FATAL）
- **V4 运行时通道 live 冒烟**：`merge-driver --dry-run` 与 `--dry-run --pod 260829-smoke-e2e` 均输出 `DRY RUN → reconciling board → board: 15 pods … → no ready-to-merge pods; nothing to do`——**FATAL `verify-gates.json missing` 零命中**，`:807` 取门通过、运行时恢复实证 ✅
- **V5 FROZEN 锚双形态断言**：`node -e` 三输入 → `> ` 前缀 true / 行首 true / DRAFT false ✅；升级闸 self-check 18/18 回归 ✅

## 3. landing 过程插曲（如实记录）

1. **无拦截、无返工**：本批 landing 一次通过全部验证集，无 scope-map/enforce-scope 拦截、无验证红。260904 批建立的两个前置（升级闸 execFn 热修 + FROZEN 行首锚格式）在本批合并门直接受益——预期本批 gov 合并可全机械走通（留待步骤 7 实证）。
2. **T5 原为 R1-F1 打回项**：census 初版把 pod-coordination.md:72 误判「不点门名」，判官实核证伪（该行 pre-existing 陈旧点源仓三门集名），修订升类入 T5 并入 manifest——census 断言「不点门名」类表述须逐一 grep 实证，不可凭印象。

## 4. 状态转移记录

- 2026-09-05 human 立项 ack + OQ1=(a)/OQ2=(a) 同场裁断。
- 2026-09-05 architect R1「需修订」（F1/F2 SHOULD + 2 NIT）→ 主线程修订（T5 增设、T1.5 折入、manifest 6 路径、packet-id 行 T1–T5）→ R2 **GREEN** → 主线程凭「立项 ack + GREEN」标 **FROZEN v1.0**；`changes-index.mjs --upsert` 钉 frozen。
- 2026-09-05 主线程 packet-bound 落地 T1/T1.5/T2/T4/T5（本 summary §1 清单）→ 验证集 V1–V5 全绿（§2）→ 本 summary 写就，交 landing-verifier 独立核验（Track B 步骤 5）。
- 待办（verifier 绿后）：T3 ARQ 移已结 + 索引钉行 → retro（步骤 6，含 §6 留根的「迁移意图表述分歧」澄清义务）→ 前置升级闸（gov 档机锚）→ 自动合并本地 main（步骤 7；remote push 留 human）。
