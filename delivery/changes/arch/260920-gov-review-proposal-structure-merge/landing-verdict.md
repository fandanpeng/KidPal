# landing-verdict · 260920-gov-review-proposal-structure-merge

**verdict：PASS + 4 ADVISORY**（核验官：landing-verifier，零上下文独立核验，2026-09-20。判定依据 = 亲跑机械门 + 五文件逐 diff 人读对照 FROZEN §2，不采信 landing-summary 自述。）

**结构门**：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260920-gov-review-proposal-structure-merge` → exit 0，`ok:true`，`frozen:true`，`packetBinding:true`，`proposalTargets ≡ summaryTargets`（5 路径一致），`unauthorized` / `claimedNotLanded` / `plannedNotLanded` 全空。

---

## 1. 逐文件 diff 对照（5/5 全核，未抽样）

| 文件 | structuralTouched | semanticMatch | 核对结论 |
|---|---|---|---|
| `docs/templates/PROPOSAL.template.md` | 是（git diff，35 行变更） | **ok** | ① 原 §1 节头删除、硬项 1a 的 7 条断言纪律**逐字保留**（diff 中 7 条 bullet 全为上下文行、零改动）；② §2 标题改 `## 2. 问题、根因与改法（T1…Tn）`、节号保 2；③ 五段固定式示范块在 :50–:57（`〔第五段·锚清单〕` 前缀避解析器、`### T<n>` 用非数字占位避幻影条目）、两处读法注在 :59–:64，与 FROZEN §2 T1 示范块逐字一致；④ 原 `:47`「对 §1 哪个根因」句已替换（见 ADVISORY-2 措辞微差）；⑤ 四标签去数字（:36/:46/:74/:76，均带「（原『硬项 …』）」溯源注）+ `:6` 形制来源句同步去数字 |
| `.claude/references/track-b-judge-brief.md` | 是 | **ok** | 透镜第 1 条 `§1 引述` → `§2 引述`；透镜第 2 条 `硬项 1b` → `硬项 · sweep 透镜义务`。仅 2 行改动，无其余触碰 |
| `.claude/references/residual-scan-patterns.md` | 是 | **ok** | 三处去数字到位：`:3`（1c → 硬项）、`:16`（1b → 硬项 · sweep 透镜义务）、`:18`（1c → 硬项 · 复扫模式完备性先验），与 FROZEN T2 目标文件注记「→ 硬项」相符 |
| `.claude/references/path-form-family.md` | 是（仅 1 行） | **ok** | `:64` `硬项 2a` → `硬项 · 目标文件锚清单`。该文件本体由前批 P4（`95bc44b`）落地，本批未夹带其它改动 |
| `.claude/skills/gov-arq/SKILL.md` | 是 | **ok** | `:12` 同句四标签逐处改（`硬项 · 起草期实证 / sweep 透镜 / 复扫先验 / 目标文件锚清单`，非整行替换）+ `:34`（`硬项 ·「目标文件锚清单」的种子`） |

**敏感面**：G1 / G3 / 契约面 / self-def / review-loop agent 定义**零触碰**——`git diff --stat` 实证脚本面零改动，机械门 `parseSection2AnchorLists` 定位三要素未受影响（V3 零差异佐证）。工作树内除 5 manifest 文件 + 本批 PROPOSAL.md（V4 行，见 ADVISORY-4）外，仅 `.codebase-memory/` 两件常驻自刷新脏（按 AGENTS.md 不追交、不判）。

## 2. packet-binding 一致

- summary packet-id 行 =「FROZEN PROPOSAL v1.0 §2 改法集（T1–T2）+ §5 验证集 · R1 BLOCK(F1..F3)+F4–F8 → R2 GREEN · change-id 一致」↔ PROPOSAL 头部 packet-binding 行逐项吻合。
- summary 内嵌 landing-manifest ≡ PROPOSAL landing-manifest ≡ §2 各 T 目标文件并集（T1：模板 + judge-brief；T2：并集 = 5 路径）——结构门对账 aligned（5 ≡ 5）。

## 3. 验证集复跑（V1–V7 全部亲跑）

| 项 | 结果 |
|---|---|
| V1 preflight | `ok:true` / `findings:[]` / `anchorCount:5` / exit 0 ✓ |
| V2 结构门 | `ok:true` / frozen + packetBinding 真 / 5 路径对账 aligned / 三空集 / exit 0 ✓ |
| V3 self-check | 全电池绿：`section2Anchors 19/19`（= R1 判官基线，**零差异**成立）、landing-preflight 9/9、isFrozen 19/19、citation-anchors 14/14 等 ✓ |
| V4 标签本体 | `grep -rnE "【硬项[[:space:]]*1[abc]\b\|【硬项[[:space:]]*2a\b" --include="*.md" .claude/ docs/` → **零命中**（exit 1）✓；**未加 `grep -v`** |
| V4b 对照（核验官加测） | 裸模式 `硬项 1a\|1b\|1c\|2a` 复跑 → 恰命中模板 ：36/:46/:74/:76 四处「原『硬项 …』」**留痕引用**——实证 V4 模式收窄必要、且收窄后待清项确实归零 |
| V5 | `§1 引述` = 0、`§2 引述` = 1（双向）✓ |
| V6 / V6b | `^## 1.` = 0、`^## 2.` = 1；`grep -n "§1"` 模板零命中 ✓ |
| V7 | route-regression `28 ok, 0 fail`；lint-descriptions exit 0，仅既有 `landing-verifier.md` 超长告警（本批未触该文件）✓ |

**真空绿规约**：V4/V5 均为双向断言（旧标识符零命中 + 新标识符在场——新形态已在各文件 diff 中逐处读到）；V3 为零差异型（19/19 对 R1 基线）；V1 自指型绿。重放输入非空（grep 扫描面非空、V4b 有命中证明模式有效）。

## 4. 结构门附加 advisory 的核验官裁定

结构门 `verbatimQuoteAdvisory` 报一条 `missing`（示范块变体 not-found-verbatim）：系机检对「首行去缩进、其余行保留缩进」混合形态的匹配局限。核验官人工比对 FROZEN §2 T1 示范块（PROPOSAL :65–:70）与模板落地块（:51–:56）：**六行逐字一致**。该 advisory 不构成偏差。

## 5. 诚实披露核对（summary §3 四条）

1. census 首版 `grep -v` 误计——与 FROZEN §2 T2「本批自证」段一致，属实。
2. V4 模式收窄——**属实且必要**（V4b 对照实验：裸模式恰命中 4 处留痕引用，落地后裸 V4 必假红）。但见 ADVISORY-4。
3. 模板 `:6` 漏改自查——落地件 `:6` 现已正确（`硬项 · 目标文件锚清单`），属实。
4. 五段式两处形态约束——均已写进模板正文（读法注 1/2），属实。

**该记未记排查**：五文件 diff 内未发现 summary 未申报的其它改动（模板尾部换行符移除除外，见 ADVISORY-3）。

## 6. ADVISORY 清单（非阻断）

- **ADVISORY-1 · 模板头部新增「结构沿革」行未获 FROZEN §2 枚举授权**。该行（模板 `:7`）不在 T1 五条改法清单内，属落地者自加；内容纯为本批变更的溯源注记、零行为语义、零机械门影响，且已在 summary §1 表申报。建议：retro 或 human 合并门知悉即可，无需返工。
- **ADVISORY-2 · 钉死句微差**。T1 连带二钉死的替换句为「每 T 一节：先写…」，模板 ：34 落地为「每 T 一节**，五段固定式**：先写…」——插入五字，其余逐字一致，语义增强非偏离。按「承重指令禁止转述」的字面标准属偏差；建议后续触模板批顺手对齐，或在 retro 记录。
- **ADVISORY-3 · 模板文件尾换行符被移除**（`\ No newline at end of file`）。纯格式噪声，无语义影响。
- **ADVISORY-4 · FROZEN 后对 PROPOSAL §5 V4 行的事后修订未入 FROZEN 状态行记录**。working tree diff 实证：PROPOSAL.md 在标 FROZEN（`72bfe61`）之后被改一行——V4 命令由裸模式改为标签本体模式（附 R1-F2 更正注）。修订本身方向正确、已如实申报于 summary §3.2、且不触 §2 改法集（落地范围不受影响）；但 FROZEN 状态行只记录了 F4/F7 补修、未记本次 §5 修订——**建议主线程 commit 时在该行补记（或 retro 记录），使 packet 的事后修订链完整**。

## 7. 结论

落地 diff 与 FROZEN PROPOSAL §2（T1–T2）语义保真成立：改法落全、无私货越权（ADVISORY-1 为已申报的良性溯源注记）、无敏感面触碰、机械门与验证集全绿且零差异。**放行建议：可进 human 合并门**；四条 ADVISORY 均不阻断，其中 ADVISORY-4 建议在 commit/retro 环节补记后闭合。
