# retro · 260914-arch-review-kidpal-session-ctx-contract（Track B 全环：契约载体定义一条消息，三轮评审两轮打回后 GREEN→PASS）

> 主线程第一人称热记忆 · 2026-09-15 · 本轮为 2026-09-14/15 连续治理批的延续乘客 · harness 形态 = architect 判官与 landing-verifier 均为同会话子 agent（SendMessage 续轮保持判官上下文连贯，R1/R2/R3 同一实例）。

**【硬项 R1】** verdict PASS 后即写，第一人称，未隔场。

**【硬项 R2 · 计量诚实】** 无逐相位 token 精确计量；下表为 TaskOutput 回报的子 agent 汇总用量（判官 R1 实测 59,235 / verifier R1 实测 38,084，R2/R3 与 verifier 复核轮未单独回报，按同量级估 ±30%）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | plan 文件 → PROPOSAL DRAFT v1.0（证据抽验后落笔） | 2026-09-14 | 2026-09-14 | ~30min | ~40k（估） |
| 评审 R1 | BLOCKED（9 项：5 MAJOR + 4 MINOR） | 2026-09-14 | 2026-09-14 | ~8min | 59,235（实测） |
| 修订 + OQ2 | 逐条吸收 + human AskUserQuestion 裁三值 | 2026-09-14 | 2026-09-14 | ~15min | ~10k（估） |
| 评审 R2 | BLOCKED（3 项：1 MAJOR 机械闸破坏点 + 2 MINOR） | 2026-09-15 | 2026-09-15 | ~10min | ~50k（估 ±30%） |
| 修订 | 三项吸收（含 CDQ 行逐字钉死） | 2026-09-15 | 2026-09-15 | ~10min | ~8k（估） |
| 评审 R3 | **GREEN** | 2026-09-15 | 2026-09-15 | ~8min | ~50k（估 ±30%） |
| FROZEN + 前置 | 标 FROZEN；v1.52 批先行独立 commit 0a303d9 | 2026-09-15 | 2026-09-15 | ~5min | ~3k |
| landing | T1–T4 packet-bound 落地 + gate-evidence 台账 | 2026-09-15 | 2026-09-15 | ~15min | ~10k |
| 核验 | verifier R1 **FAIL**（缺 architect-landing-summary）→ 补件 → 复核 **PASS** | 2026-09-15 | 2026-09-15 | ~2×4min | 38,084 + 复核（估） |
| **合计** | | 2026-09-14 | 2026-09-15 | ~1.5h 有效工作 | ~270k ±30% |

对照：先例 260902 契约重写批为多日大包；本批单条消息定义、两日收口，量级合理。

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 修订 + 落地） | ~71k（估） | ~26% | 证据抽验与钉死文本撰写是大头 |
| 判官（R1–R3） | ~159k（R1 实测 + R2/R3 估） | ~59% | 三轮评审为最大花费 |
| 核验官（两轮） | ~57k（R1 实测 + 复核估） | ~15% | 第一轮因缺件白跑一次语义核对 |

花费最大处 = 判官三轮。可压缩分析：R1 的 MAJOR-3（CDQ/evidence 缺件）与 MAJOR-5（工作区纠缠前置）属「起草期没读够先例与模板 Required Artifacts」——起草前若先读 `contract-change-evidence.md` + 一份近期 CDQ 行 + 跑一次 git status 盘点工作区，R1 打回可减 3–4 项；R2 的 MAJOR-1（锚清单 vs manifest 对账）则属判官读脚本发现的真实机械闸盲区，主线程起草时无法预知，属结构门兜底价值。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 落地时才发现缺 architect-landing-summary**：现象 = verifier 结构门早退 FAIL → 根因 = 主线程把「制品 4 件」读成 landing 后才产出，而结构门要求 summary 在 verifier 跑之前在场 → 证据锚 = `landing-fidelity-check.mjs` 报「architect-landing-summary.md is required」+ verifier verdict R1 节。
- **F2 · 起草期漏契约门伴生制品**：现象 = R1 MAJOR-3 打回无 CDQ 登记 / 无 evidence 台账计划 → 根因 = 主线程只盯 PROPOSAL 模板硬项、没读 `contract-change-evidence.md` 的 Required Artifacts → 证据锚 = architect-review.md R1 MAJOR-3。
- **F3 · 工作区纠缠未预先声明**：现象 = R1 MAJOR-5 打回（v1.52 未提交改动同文件 + T3.3 已提前应用）→ 根因 = 起草时把同会话早前的直改批当作背景而非「落地前置依赖」声明 → 证据锚 = architect-review.md R1 MAJOR-5 + git status 实测。
- **near-miss（最险一刻）**：T4 首版把 `architecture-gate-evidence.md` 写进「目标文件」行而 manifest 无此行——若直接 FROZEN 落地，`landing-fidelity-check.mjs` 的 manifest↔锚集对账会 fail-loud RED（判官 R2 实读脚本 `governance-gate-shared.mjs:345-419` 证实），升级闸拦下白跑一轮 landing。教训 = 「目标文件锚清单」是机械对象不是叙事清单，新增制品想不进 manifest 必须显式声明豁免理由。

## 4. 优化建议（按 ROI 排序）

1. **起草期 checklist 增「契约伴生件三问」**（是否 CDQ 登记？是否 evidence 台账计划？工作区是否有同文件未提交批？）：治 F2/F3 · 载体 = ARQ 收件箱（候选固化进 `docs/templates/PROPOSAL.template.md` §2 前言一句）· 预期收益 = 契约门批 R1 打回项 -3~4。
2. **verifier 前置自检一行**（跑结构门前先 `ls` 四件制品齐不齐）：治 F1 · 载体 = 登记不硬治（主线程纪律）· 预期收益 = 免一次 verifier 白跑。
3. **评审循环同实例续轮**（SendMessage 保持判官上下文）效果好，R2/R3 无需重验 R1 已核面：留批次（现状已自然形成，无需动）。

## 5. 一句总结

成本 ~270k tokens / ~1.5h / human 触点 2 次（流程指示 + OQ2 裁定）/ 制品 6 件（PROPOSAL + architect-review + landing-summary + gate-evidence + landing-verdict + retro），对照 260902 大包属轻量批；最险一刻 = gate-evidence 差点以锚清单形态冻结进机械闸盲区，被判官读脚本拦下；**保险丝判定：两方独立结构接住了作者偏差**（判官三轮拦下 12 项打回含 1 项机械闸破坏点，verifier 拦下缺件白跑——作者（主线程）自认「文本级小事」的两处均被独立方证实为真问题）；最值得带走的一条 = **钉死文本的纪律值三轮评审：凡写「逐字钉死」处，判官逐字核对全过、零争议，所有打回都落在没钉死的地方**。
