# retro · 261023-arch-review-memory-wiring-verified-behavior（Track B 全环：spec 口径修正「已验证行为取代未验证设计」）

> 主线程第一人称热记忆 · 2026-10-23 · 乘客 #<按 review INDEX 顺序现推> · harness 形态 = 判官 / 核验官为同会话零上下文子 agent（architect-reviewer 两轮同实例续跑；landing-verifier 独立实例）· 无跨乘客同类坑对照（本环特殊点：与批 B Lite 包同 worktree 并行在途，全程零混提）。

**【硬项 R1 · 环后趁热、第一人称】** 落地与核验当轮即写，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent tokens = harness usage 通知的累计值（含上下文重载，非净增量）；主线程 tokens 未单独计量（本会话多批次共享），按落地+编排动作估占比，误差 ±30%。时长 = 通知 duration_ms。

**【硬项 R3 · 摩擦带根因 + 证据锚】** 见 §3。

**【硬项 R4 · 建议必标载体】** 见 §4。

**【硬项 R5 · 保险丝判定】** 见 §5。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | architect 起草 v0.1（preflight 首跑抓 T7 围栏截断、锚行前置修正） | 2026-10-23 | 同日 | ~6.4 min | ≈190k |
| 评审 R1 | 判官四查 + 判档主路径定谳，verdict BLOCK(F1–F5) | 同日 | 同日 | ~4.8 min | ≈69k |
| 折返修订 | author F1–F5 修订 → v0.2，两门复跑绿 | 同日 | 同日 | ~2.4 min | ≈228k |
| 评审 R2 | 判官复核 → GREEN | 同日 | 同日 | ~1.7 min | ≈92k |
| FROZEN | human 立项 ack（AskUserQuestion，OQ-1=(a)/OQ-2=(a)） | 同日 | 同日 | 主线程触点 | — |
| landing | 主线程 packet-bound 落 T1–T8（三文件） | 同日 | 同日 | ~10 min | 未单独计量 |
| 核验 | landing-verifier 独立核验 → PASS | 同日 | 同日 | ~5.8 min | ≈70k |
| **合计** | | | | **~31 min**（不含批 B 并行在途） | **≈65 万**（子 agent 合计，±10%） |

对照：261022 架构批（PROPOSAL v1.1）为多日多轮大体量批；本批为单日单主题轻量主路径批，评审两轮收敛（R1 五条全为补全项、零方向动摇），速率健康。

## 2. Token 账

计量方式诚实声明：子 agent 侧 = harness usage 通知累计（含续跑上下文重载，±10%）；主线程侧未单独计量（多批次共享会话），占比如下为估计（±30%）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（编排 + FROZEN + 落地 T1–T8 + 验证集） | 估 ≈15 万（±30%） | ~19% | 落地为 Edit 逐字搬运，无推导性生成 |
| architect（起草 + 折返，两轮） | ≈41.8 万 | ~64% | 最大头；折返轮含 PROPOSAL 全文重读（上下文重载） |
| architect-reviewer（R1 + R2，同实例续跑） | ≈16.1 万 | ~25%? | R2 聚焦复核、成本骤降（92k vs 69k 但时长 1/3） |
| landing-verifier | ≈7.0 万 | ~11%* | 独立字节级比对脚本自含 |

（占比合计 >100% 因分母口径为子 agent 合计 ≈65 万；如实声明，不凑数。）

花费最大处：author 折返轮（PROPOSAL 全文重读）。可压缩分析：R1 findings 若按「只发 finding 清单 + 定位行」而非全节上下文回派，author 无需重读全文——但同实例续跑本就带全上下文，重载主因是 agent 生命周期跨通知重启；真实压缩手段 = 判官与 author 的轮次在同一 agent 生命周期内完成接力（本环已部分达成）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 机械门不跳围栏，`## ADR-016` 标题行截断 §2**：现象 = preflight 首跑报 T7 缺锚（exit 1）→ 根因 = `parseSection2AnchorLists` 的 §2 节域按行首 `## ` 截断且不跳代码围栏（PROPOSAL 附录一 :346 留痕）→ 处置 = T7/T8 锚行物理前置 + 件内节序说明。教训：PROPOSAL 模板对「围栏内含标题形态文本」的改法（ADR 追加类）天然踩此坑——**「锚行前置」应进 PROPOSAL 模板的 ADR 类改法指引**。
- **F2 · 判档预期错位（轻 vs 主）**：任务书预期轻路径，author 按判据自评主路径（残余申报 = 新增语义记载）→ 判官复裁一致。成本 ≈0（author 已备瘦制品结构件、裁主不改写），但主线程排期时按轻预估的时长低估了约一档。根因 = 主线程派发时对「残余申报算不算新语义」预判不足。
- **F3 · 评审抓出 sweep 漏项（glossary 投影句 + census pattern 命不中自己的退役语面）**：author 初版判 glossary「指向仍成立」= 误判，R1-F1 驳回后新增 T8 并如实记录初版 census 的完备性缺口（PROPOSAL :265 勘误在案）。证明判官独立性有效（见 §5），也说明「投影句 sweep」缺一份核对清单。
- **near-miss（最险一刻）**：批 B 在同 worktree 在途期间落地 canonical 三文件——若 author/判官/verifier 任一方把批 B 在途件误认作本批 diff 面（或反向混提），双批证据链都会被污染。实际未出事：批 B integrator 全程零触碰申报（integrator stat 核对）+ landing-verifier 以 `git status --porcelain docs/` 限定 diff 面 + 派遣时明示「只核 docs/ 三文件」。教训 = 并行批同 worktree 时，各方 diff 面的**明示边界声明**是防污染的关键动作，本次三处声明全部在场。

## 4. 优化建议（按 ROI 排序）

1. **PROPOSAL 模板补「ADR 类改法的锚行前置指引」**：治 F1 · 载体 = ARQ 收件箱（gov 域，随下轮模板修订落地） · 预期收益 = ADR 追加类批免一次 preflight 假红往返。
2. **「投影句 sweep 清单」固化**：治 F3 · 载体 = ARQ 收件箱（把「词条/映射表/行内括注三类投影句 + census pattern 自反验证」列成 author 起草 checklist） · 预期收益 = R1 findings 数下降（本环 5 条中 2 条属此族）。
3. **主路径/轻路径判档预判口诀**：治 F2 · 载体 = 登记不硬治（triage 派发任务书里加一句「残余申报 = 新增语义记载 → 预期主路径」的经验注记即可） · 预期收益 = 排期预期对齐。

## 5. 一句总结

成本 ≈65 万 tokens（子 agent 合计）/ ~31 分钟纯相位时长 / human 触点 2 次（三项裁决一次 + FROZEN 确认一次）/ 制品 8 件（PROPOSAL + verdict ×2 + landing-summary + verdict.json + retro + 评审记录 §7 两轮）· 最险一刻 = 并行批同 worktree 的 diff 面污染风险（三处明示边界声明接住）· **保险丝判定：两方独立结构接住作者偏差**——判官 R1 抓出 author 自判「指向仍成立」的误判（F1）与三处清单漏项（F2/F3/F4），landing-verifier 以字节级比对独立证伪机械门的 4 项行内片段假阳 advisory，两层都实测生效 · 最值得带走的一条 = **「验证过的接线」要靠查实物证据而不是文档转述**——本批全部改法的可信度来自判官对 ECS 探针配置的亲读，文字记载（debt 证据 7 的缩略键形）本身就是错源。

## 6. 本环遗留

- **需求面同步批（OQ-1=(a)）**：/requirement-change 改 AC5（:598）/ 补充规则（:601）/ 可行性行（:603）/ NFR-PRIV-1 ①（:654，含「不被复述」半句），**须与 REQ-261023-family-arrangement-scope-clarification 合并起草**——待本批合并后主线程排期。
- **批 B 部署轮实测项**：`noisePatterns` 拦截行为（OQ-4）、QQ `per-sender` 与 `honcho_*` 可检索性（OQ-3）——实测结果回填 §6.7 与需求批。
- **debt 证据 7 键形勘误 append**：挂批 B 收口（planner 执行中）。
- **verdict ADVISORY 三条**（行内片段假阳 ×4 / :487→:490 行移 / 需求面张力窗口在册）：均非阻断、已记录于 landing-verdict.md，无需动作。
