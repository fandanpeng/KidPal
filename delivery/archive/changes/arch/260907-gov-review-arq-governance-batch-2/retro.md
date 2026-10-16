# retro · 260907-gov-review-arq-governance-batch-2（Track B 全环：七 ARQ + 模板钉死八项清结）

> 主线程第一人称热记忆 · 2026-09-07 · 乘客 #9（治理档；当日继 260906 三批与并发会话的 model-override 批之后）· harness 形态：判官 / 核验官 = 本会话派生的零上下文子 agent（architect R1 24 tool_uses / landing-verifier 22 tool_uses），主线程起草 + 落地 + 编排 · 跨乘客对照：260906 批（#8，十项 19 文件）之后的第二个「ARQ 批量清结批」，量级略小（七项 + 折入 1 项、13 文件）但首吃「并发会话骑线合并」拓扑插曲。

**【硬项 R1】** 热记忆即写，未隔场。**【硬项 R2】** §2 计量诚实声明。**【硬项 R3】** §3 每条 F 带现象→根因→证据锚。**【硬项 R4】** §4 每条建议标载体。**【硬项 R5】** §5 保险丝判定。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|主线程估计 ±35%||
| 分析 + 起草 | 新增 ARQ 全文取证 + 闸/脚本落点摸底 + PROPOSAL v0.1（DRAFT，commit d305c43） | 09-07 | 09-07 | ~20 min | ~55k（估） |
| OQ 交互 | 三题 AskUserQuestion（ADR 名词澄清一轮）+ v0.2 修订（c4ef232） | 09-07 | 09-07 | ~8 min | ~10k（估） |
| 评审 R1 | architect 24 tool_uses → NEEDS-REVISION（F1–F5 修订级 + F6–F11 NIT；110,479 实测） | 09-07 | 09-07 | ~9 min | 110,479（实测） |
| 修订 + 拓扑修复 | v0.3 修订（89ca380 误落 main → cherry-pick 55abc92 + main reset） | 09-07 | 09-07 | ~12 min | ~15k（估） |
| 评审 R2 | 判官续实例复验 → GREEN（N1–N3 NIT 限定折入） | 09-07 | 09-07 | ~4 min | （续实例，未单独计量） |
| FROZEN | 状态行 + N1–N3 折入 + INDEX 钉行（e81c600） | 09-07 | 09-07 | ~5 min | ~6k（估） |
| landing | T1–T8 落地 13 文件 + V1–V9 验证 + summary（c099c1b） | 09-07 | 09-07 | ~35 min | ~85k（估） |
| 核验 | landing-verifier 22 tool_uses → GREEN 13/13（59,378 实测） | 09-07 | 09-07 | ~8 min | 59,378（实测） |
| 收尾 | ARQ 七项移 §2 + 新 ARQ 登记（a8db4d4）+ 本 retro | 09-07 | 09-07 | ~10 min | ~12k（估） |
| **合计** | | | | **~1 h 43 min** | **~353k ±35%** |

对照：260906 批（#8）~2 h 44 min / ~309k——量级略小、时长更短（OQ 一次过、判官 R1 一次修订即 R2 GREEN），节奏连续两批持平向好。

## 2. Token 账

计量方式：判官（R1）与核验官子 agent 实测（110,479 / 59,378）；主线程估计 ±35%。R2 为续实例（SendMessage 续 R1 判官），未单独计量——260906 批 F4 建议已部分采纳（子 agent 实测汇报入 prompt 要求，R1/verifier 已带 usage 返回；R2 续实例的 usage 不再单独结算，留观察）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + OQ + 修订 + FROZEN + landing + 收尾） | ~183k（估） | ~52% | landing 段最重（T3 三拍语义 + 自检夹具两形态） |
| 判官（R1 + R2 续实例） | 110,479 实测（R2 未单结） | ~31% | R1 五处 BLOCK 级全中靶心（含两处「本批病灶在 PROPOSAL 自身复现」） |
| 核验官 | 59,378（实测） | ~17% | 13/13 人读 diff + 结构门 + V1–V4 独立复跑 |

花费最大处：R1 判官的亲核实证（24 tool_uses 逐行号抽验 + friction 事件原文亲验）——成本高但直接换来了 F1/F2 两条「本批治理的病灶类型在 PROPOSAL 自身复现」级打回，是保险丝结构的正向收益。可压缩分析：v0.1 起草若直接按 260906 批 retro 建议 1（T8 已钉进模板）写 `### T<n>` + 裸路径，可省一轮格式类打回——本批 v0.1 已按此形态起草，R1 打回全为实质问题，模板钉死已见效。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 并发会话骑线合并拓扑插曲（最险）**：现象 = R1 评审期间另一会话在同一 checkout 上切 main、把其 landing commit（1c46aa1）骑在本批治理分支上并 ff main，本批 v0.3 修订 commit（89ca380）误落 main → 根因 = 共享 checkout 无会话间互斥，两个 Track B 批次同 checkout 并发时 branch 状态是共享可变态 → 证据锚 = git log 实证（main = 1c46aa1 + 9e84869 + 89ca380；分支 = c4ef232 + 1c46aa1）+ 修复链（cherry-pick 55abc92 + main reset 9e84869，verifier 独立复核实证自洽）。**near-miss**：若 v0.3 之后的 FROZEN/landing commits 继续落 main，off-main 校验与合并语义全乱。没出事的原因 = `git branch --show-current` 在每次 commit 前的习惯性核对 + 拓扑修复及时。教训：**Track B 批次期间每次 commit 前核对当前分支应为强制习惯**（本批起主线程自守）。
- **F2 · R1 打回两条「本批病灶在 PROPOSAL 自身复现」**：现象 = F1（T4/T5/T6 锚行「OQ2 条件锚」注记会被 parseSection2AnchorLists 裁出 → 锚并集 12 ≢ manifest 13，V8 必假红）+ F2（T3 `git checkout --` 只还原 worktree←index，覆盖不了 renormalize staging 实况）→ 根因 = 起草期未以机械门视角自审锚行注记 + git 语义想当然 → 证据锚 = R1 记录（governance-gate-shared.mjs:410 裁剪规则 + checkout 语义核对）。判官把「本批要消灭的返修类」在自家 PROPOSAL 上抓出来，是两方独立结构价值的最直接实证。
- **F3 · 自检夹具撞 git sparse 拒绝**：现象 = 形态二 staged 漂移用例 `git add` 撞 skip-worktree 文件被 git 拒（sparse hint）→ 根因 = git 对 skip-worktree 在册文件的 add 拒绝语义未在起草时预料 → 证据锚 = self-check 运行 stderr（updateSparsePath hint）→ 修复 = 形态二先撤两件旗标再 staging（也更贴合 renormalize 实况：旗标清 + 内容入 staging）。
- **F4 · V9 计数偏差当场修正**：现象 = acceptance-author 引用行初版把 T5+T6 合成 1 行（FROZEN 钉各 1 行）→ 根因 = 起草引用行时把两条纪律合成一条 bullet → 证据锚 = git diff --stat 实测 5 vs FROZEN +6 → 修正 = 拆两行，终态逐件对齐。
- **near-miss（另记）**：T4 入列字段（eventId+ts）与 FROZEN 字面（changeId+eventType）措辞异——verifier 定性定位意图达成、留 retro 对齐措辞（本行即对齐：落地字段 eventId+ts 是更精确的逐事件定位形态，FROZEN 措辞指定位信息应含 changeId/eventType 侧信息——顶层 changeId 已含、eventType 恒 friction，实质等价）。

## 4. 优化建议（按 ROI 排序）

1. **Track B 批次期间 commit 前强制核对分支**：治 F1 · 载体 = 固化候选 → 主线程自律件先行的 self-discipline（不建新 gate——共享 checkout 场景少，为它建机械门不成比例）；若再现第二次，升级 ARQ 评估轻提醒（merge-driver 或 closeout 输出一行「当前分支非治理分支」warn）。· 预期收益 = 消掉拓扑插曲整类。
2. **R2 续实例 usage 单独结算**：治 §2 R2 未单结 · 载体 = 登记不硬治（续实例省 token 但计量粗糙的取舍，两批实证可接受） · 预期收益 = 计量精度。
3. **FROZEN 阶段对自检断言计数预估标注「≥」语义**：治 §3 注 1（V3 pass:9 vs FROZEN 7 的粒度超集宽免）· 载体 = 固化候选 → 下批 PROPOSAL 起草自律（V 条目写「断言数 ≥ N」或直接不写死计数） · 预期收益 = 省 verifier 一轮定性。
4. **citation 前缀候选域补 docs/ 裸名**：治 verifier 观察 4② · 载体 = 已登记 ARQ-260907-lfc-citation-docs-prefix（下批动 landing-fidelity 的治理批顺手） · 预期收益 = advisory 信噪比归零。

## 5. 一句总结

~1 h 43 min / ~353k ±35% / human 触点 3 次（立项 ack + OQ 三题〔含 ADR 名词澄清〕+ 无追加）/ 制品 4 件齐 · 对照 #8 批时长缩短 37%、tokens 持平 · 最险一刻 = 并发会话骑线合并插曲（F1，branch 核对习惯抓住）· **保险丝判定：两方独立结构接住了作者偏差**——判官 R1 打回 F1（条件锚致 V8 必假红）与 F2（checkout 语义错）两条「本批病灶在 PROPOSAL 自身复现」级问题，核验官 13/13 人读 diff + 六条诚实注记逐条核成立 + V3 计数超集与 T4 措辞异两处宽免均留痕定性 · 最值得带走的一条 = **判官「亲核实证」模式（24 tool_uses 逐行号抽验）是保险丝结构的核心成本，也是核心价值——R1 五处修订级打回全部中靶**。

## 6. 本环遗留（批次型变更特有）

- **五项非 governance ARQ 留队列**（multiturn-continuity / p4-dasharray / v23-asr / protocol-version / nvs-csv-wifi）——处理时机各在 ARQ 行，本批不代处理。
- **ARQ-260907-lfc-citation-docs-prefix 新登记**（§1.2，a8db4d4）：下批动 landing-fidelity 的治理批顺手根治。
- **ARQ-260907-architect-dispatch-model-override**（并发会话登记，§1.2）：本批不代处理，owner 归其批。
- **model-override 批拓扑关系**：其 landing commit（1c46aa1）骑在本批治理分支上、已随其流程 ff 进 main——两批历史已交织（本批 PROPOSAL 草稿 commits 亦在其 main 中），合并时按 merge-driver 语义走即可，无冲突面（本批 landing 改动 13 文件与其 3 文件零交集）。
- **自律件**：主线程 Track B 批次期间每次 commit 前核对当前分支（F1 教训，建议 1 载体）；包 3+ 起包时四方按 DEV-GOVERNANCE「## 起包格式期望前置」五条自检。
- **docs/ADR.md 空转期**：零条目预填、存量决策不考古搬迁——首个条目将由下一个产出架构决策的批次按文件头格式登记（integrator 收口或主线程治理批）。
