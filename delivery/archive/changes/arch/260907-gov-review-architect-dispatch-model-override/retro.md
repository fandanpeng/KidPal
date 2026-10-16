# retro · 260907-gov-review-architect-dispatch-model-override（Track B 全环：起草→R1 GREEN→FROZEN→landing→verifier APPROVE 一日内闭环）

> 主线程第一人称热记忆 · 2026-09-07 · 乘客 #N（当日第 5 个 Track B 批次，同日继 260906 三批与 loop-retro 批之后）· harness 形态：判官 / 核验官 = 零上下文子 agent 全权形态（architect R1 37 tool_uses 亲核实证；landing-verifier 24 tool_uses 亲跑 V1–V4）· 跨乘客对照：锚行粗体病灶第二例（260906 批首例成文于其 §1）。

**【硬项 R1】** verdict 绿后即写，本环相位全部发生于本会话内，第一人称无隔场。

**【硬项 R2】** 计量来源 = Agent 工具返回的 usage 字段实测（判官 69100 tokens / 520859ms；核验官 54161 tokens / 545736ms）；主线程侧无逐相位计量，按会话体感估计 ±30%。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | 追查（ARQ 登记）→ PROPOSAL v0.1 → OQ 裁断回灌 | ~14:4x | ~15:0x | ~30min（会话内连续） | 估 60k ±30% |
| 评审 R1 | architect 判官，0 BLOCK / 3 NIT，GREEN | 15:1x | 15:1x | 8.7min | 69100（实测） |
| FROZEN | v1.1：F1/F2/F3 回灌 + 状态行 | 紧随 R1 | 紧随 R1 | ~5min | 估 10k ±30% |
| landing | T1 hook+settings、T2 planner.md、V1–V4 全绿 | 紧随 | 紧随 | ~10min | 估 25k ±30% |
| 核验 | landing-verifier 首轮 REJECT（F1 结构门）→ 修锚行 → 复跑 GREEN → APPROVE | 15:2x | 15:4x | ~18min | 54161（实测）+ 主线程修格式 |
| **合计** | 起草到可合并 | | | ~70min | ~220k ±（判官+核验官实测 123k） |

对照：与前几乘客同量级；本轮判官实测背书（预 landing 实跑钉死源码）显著降低了返修轮次（R1 一轮 GREEN）。

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（追查+起草+落地+修复） | 估 ~95k ±30% | ~43% | 追查阶段（transcript/proxy 取证）占比过半——ARQ provenance 义务的固定成本 |
| 判官（R1） | 69100 | ~31% | 实测；37 tool_uses 含预 landing 实跑 |
| 核验官 | 54161 | ~25% | 实测；24 tool_uses，两轮（首核+收口） |

花费最大处：主线程追查（harness transcript + cc-switch SQLite 取证）。可压缩分析：同类「模型档位没生效」追查若有一份「档位解析优先级 + 代理映射」速查 reference，可省过半；候选见 §4-3。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 锚行粗体病灶第二例**：现象 = landing-verifier 结构门 RED「§2 缺锚清单」，而锚清单语义在场 → 根因 = 主线程起草时凭语感写成 `**目标文件**：`，机械门 `parseSection2AnchorLists` 只认行首裸 `目标文件：`（`.claude/scripts/lib/governance-gate-shared.mjs:368`）→ 证据锚 = landing-verdict.md F1 + 本 PROPOSAL :201/:218 修订注记。260906 批已在模板钉了标题/manifest 形态，但**锚行形态漏钉**——同根第三处待钉（见 §4-1）。
- **F2 · summary 初稿「零告警」失实**：现象 = 主线程 lint 输出重定向 /dev/null 只看 exit 0，写了「零告警」→ 根因 = 告警级工具 exit 0 与「无告警」不等价，验证转述未看输出体 → 证据锚 = landing-verifier 复核「1 条既有告警（landing-verifier.md 超长）」+ summary §2 V3 修正行。教训：advisory 级工具的转述必须基于输出体非 exit code。
- **F3 · 结构门首跑时点晚于 FROZEN**：现象 = 机械门兼容性到 landing 核验期才首次暴露 → 根因 = 起草期未跑结构门预检（它是 landing 期工具，但 draft 形态即可空跑）→ 证据锚 = 本环 verifier 首轮 REJECT 唯一阻断项即格式项。
- **near-miss（最险一刻）**：判官 R1 若未做「从围栏逐字节抽出源码实跑」的预 landing 实证，hook 源码若有笔误将带病 FROZEN、到 V5 在线探针才爆——届时逃生门与 deny 路径已在产线。没出事的原因 = judge brief + 判官自觉的实测纪律；制度化建议见 §4-2。

## 4. 优化建议（按 ROI 排序）

1. **PROPOSAL 模板补锚行形态硬条款**：治 F1 · 载体 = ARQ 收件箱（新 ARQ：模板硬项 2a 处补一句「锚行一律行首裸 `目标文件：`，不加粗体」——与 260906 批已钉的标题/manifest 形态同节）· 预期收益 = 消「结构门 RED→格式返修→verifier 二轮」整条返修链，本环实测浪费 ~18min 核验轮。
2. **判官 brief 钉「预 landing 实跑」义务**：治 near-miss · 载体 = ARQ 收件箱（候选：track-b-judge-brief 增「PROPOSAL 内钉死源码/配置须逐字节抽出实跑」条款——本环判官自发做了，收益巨大但靠自觉）· 预期收益 = 带病 FROZEN 风险前移拦截。
3. **「模型档位解析优先级」速查 reference**：治主线程追查成本 · 载体 = 登记不硬治（用户环境面文档，`.claude/references/` 是否收纳待 architect 裁）· 预期收益 = 同类追查省过半。
4. **advisory 工具转述纪律**：治 F2 · 载体 = 自律件（主线程：warning 级工具的验证转述须引输出体关键行，不引 exit code）· 预期收益 = 防 summary 失实类返工。

## 5. 一句总结

成本 ~70min / ~220k tokens（实测子 agent 123k）· 触点 human 4 次（OQ 裁断×2 轮、立项 ack、豁免讨论）· 制品 4 件齐 · 最险一刻 = 钉死源码若未判官预实跑将带病 FROZEN · **保险丝判定：两方独立结构接住作者偏差**——判官独立枚举纠正了主线程 census 计数（F1→下界化），verifier 独立结构门 + 亲跑 V1–V4 接住了锚行格式缺陷与 summary 失实（F1/F2），两处偏差均未流入合并 · 最值得带走的一条 = **锚行/标题/manifest 的机械门形态约束应在起草期空跑结构门一次，不留给 landing 期首曝**。

## 6. 本环遗留

- **V5 在线探针未闭合**（本会话 hook 快照于启动时，注册后派发不可靠触发）：去向 = 新会话首跑补验，指令在 landing-verdict.md §5；探针红即打回。
- **OQ3 留根**：AInativecompany 仓 architect.md `model: opus` 漂移不动（human 裁留根），去向 = 那仓独立治理批。
- **ADVISORY**：CRLF verbatim 假失配 2 条（归 ARQ-260906-lfc-verbatim-crlf-citation-gap 既有病灶）；`docs/ARCH-REVIEW-QUEUE.md` changedCanonicalNotInProposal 系本 change 自身 ARQ 登记，非私货。
