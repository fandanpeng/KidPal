# retro · 260915-gov-review-worktree-pool-reuse（Track B 全环：树池复用 + 常驻分支 + 基线差异判停 + 启动卡收缩）

> 主线程第一人称热记忆 · 2026-09-15 · harness 形态：判官 = architect 全量 agent 子代理、核验官 = landing-verifier 全量 agent 子代理（非降级形态）· 跨乘客对照：锚行加粗形态撞结构门 = ARQ-260913 同款坑**第二次发生**（跨乘客复踩在案）。

**【硬项 R1 · 环后趁热、第一人称】** verdict 复核相位仍在进行中动笔起草本 retro（语义面已判全绿、结构门返修已绿，仅差 verdict 文件更新为 PASS）——特此声明；verdict PASS 落盘后本 retro 即为终态，无隔场补记。

**【硬项 R2 · 计量诚实】** 子代理有精确计量（usage 字段）；主线程无 token 计量——估计法 = 会话轮次 × 每轮上下文规模量级，误差界 ±50%。

**【硬项 R3/R4/R5】** 见 §3/§4/§5。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 + human 对齐 | PROPOSAL v0.1→v1.0（7 轮 human 方案讨论：W 弃字母/K-N 判据/分支/启动卡/架构记账） | 2026-09-15 上午 | 下午 | ~3h（会话内，与 K1a launch 并行） | ~600k ±50%（无计量） |
| 评审 R1 | architect 需修订（F1–F3 BLOCKING + F4–F6 SHOULD + N1/N2） | — | — | 5.5 min | 70,199 |
| 回填 + R2 | v0.5 回填 → R2 GREEN（N3/N4 冻结前收进） | — | — | 2 min（上下文复用） | 262 |
| FROZEN | 凭据齐（ack + R2 GREEN + OQ 全收口） | — | — | 分钟级 | — |
| landing | T1–T6 六文件 + 制品四件（commit 7bba2ac + 返修 5ae0874） | — | — | ~50 min | ~400k ±50%（无计量） |
| 核验 | landing-verifier：R1 FAIL（锚行格式单点）→ 返修 → 复核 PASS | — | — | 5.6 min + 复核轮 | 42,341 + 复核 |
| **合计** | | | | ~4h 墙钟（含与 K1a 开发并行等待） | ~1.1M ±50% |

对照：比 260914-delivery-restructure 批（大得多）轻；与 260908 系列小治理批同量级——但 human 对齐轮次偏多（7 轮），因本批方向是会话里现场共创（用户逐条挑战：命名绑定/独占校验/prompt 价值/分支必要性），非预设方案评审。

## 2. Token 账

计量方式：子代理 = usage 精确值；主线程 = 估计（±50%）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 落地 + 返修） | ~1.0M ±50% | ~88% | 七轮 human 对齐是最大头（每轮重读全文） |
| 判官（R1 + R2） | 70,461 | ~6% | R1 全量亲核 17 项承重断言；R2 上下文复用极廉 |
| 核验官（R1 + 复核） | 42,341 + 复核轮 | ~4% | 逐文件人读 diff + 结构门 |

花费最大处：human 对齐轮次的主线程上下文重读。可压缩分析：方案共创类讨论天然多轮，但 v0.2–v0.4 三次增量（基线/分支/prompt）本可在起草前一次性想全——根因是问题 4/5/6/7 都由 human 在 ack 前逐个抛出，主线程未在 v0.1 主动 sweep「launch 面还有哪些承载错位」。教训：**human 抛出第一个成本痛点时，主线程应主动同族扫描**（本批问题 5/6 全是「每包一树」同族，本可并进 v0.1）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · PROPOSAL 锚行加粗形态撞结构门（第二次）**：现象 = landing-fidelity-check exit 1 报六锚缺失 → 根因 = `parseSection2AnchorLists` 只认行首裸「目标文件：」，起草者（我）凭手感写了 `**…**`，而 PROPOSAL.template 硬项 2a 段只钉了 `### T<n>` 标题形态、**未钉锚行裸形态** → 证据锚 = verifier verdict + `governance-gate-shared.mjs:345` + 260913 批同款先例注记（ARQ-260913-lfc-anchorable-delivery-gap 附带发现）。
- **F2 · 大段 Edit 替换吞段**：现象 = T2 落地时 old_string 含 pod-coordination「门归属」整段而 new_string 未带回，段落被误删 → 根因 = 手工构造大替换块时 old/new 不对称 → 证据锚 = 落地会话（当场发现立即恢复，verifier 未受影响）。
- **F3 · V8 验证命令字面不达标返工**：现象 = 替换行保留「每个 ready TaskPackage」前缀，冻结 grep 模式仍命中 → 根因 = 落地措辞未对着验证命令写 → 证据锚 = landing-summary §3.1。
- **F4 · 结构门参数形态**：现象 = 首跑传裸 change-id 报「review directory does not exist」→ 根因 = usage 要目录路径、README 未提示 → 证据锚 = landing-fidelity-check.mjs:268。
- **near-miss（最险一刻）**：F2 的误删若未当场发现，会静默丢掉「门归属（非第 7 工程门）」整段治理语义——侥幸因素是我对被替换区域有新鲜记忆。教训：大段 Edit 前先 `grep -c` 新旧块行数对称性，或分两次小编辑。

## 4. 优化建议（按 ROI 排序）

1. **PROPOSAL.template 硬项 2a 补一句锚行形态**（「目标文件：行首裸形态、禁加粗/反引号包裹整行」）：治 F1 · 载体 = 固化候选 → ARQ 收件箱（ARQ-260913-lfc-anchorable-delivery-gap 追加证据注记，处理时机已到）· 预期收益 = 根治第二次复踩。
2. **主线程「同族扫描」纪律**：治 §2 可压缩分析 · 载体 = 留批次（下个成本痛点类 PROPOSAL 起草时执行，暂不立机制）· 预期收益 = human 对齐轮次 −2~3。
3. **landing-fidelity-check README 行补参数示例**：治 F4 · 载体 = 登记不硬治（下次触该脚本的批次顺手）· 预期收益 = 省一次首跑失败。
4. **大段 Edit 对称性自检**：治 F2 · 载体 = 主线程自律件（不进模板——工具行为非治理面）· 预期收益 = 杜绝吞段。

## 5. 一句总结

成本 ~1.1M tokens / ~4h 墙钟 · human 触点 8 次（7 轮共创 + 1 次立项）· 制品 4 件 + 证据件 1 份 · 对照同量级批 human 轮次偏多但换来了方向零返工 · 最险一刻 = Edit 误删门归属段（当场恢复）· **保险丝判定：两方独立结构接住作者偏差**——判官独立接住三处实质错（F1 复位目标自破 F-05 / base-ref 残留逃逸验证集 / 严格模式载体静默丢失），核验官独立接住结构格式错（锚行形态），作者偏差无一漏网 · 最值得带走的一条：**human 抛成本痛点时主动同族扫描**——本批一半的问题（分支、prompt、架构记账）本可在 v0.1 一次想全。

## 6. 本环遗留

- V9（N1b 在 K1a worktree 首例换包 + 基线差异判停）：按 OQ3 排期，K1a 合并后执行，证据落 workstream progress.md——新协议的活体试验，未到期非欠账。
- ccache（OQ2 已拍不落）：留根 ARQ 观察（触发 = 新树冷编译再成瓶颈）。
- lint-descriptions 既有 warn（landing-verifier.md 超长）：非本批引入，下个触该文件的批次顺手收。
- citationAnchorAdvisory（SKILL.md:56 裸文件名）：ARQ-260906-lfc-verbatim-crlf-citation-gap 在册族，advisory 级维持。
