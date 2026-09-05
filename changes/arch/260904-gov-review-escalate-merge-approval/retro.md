# retro · 260904-gov-review-escalate-merge-approval（Track B 全环：升级闸 ESCALATE 人批放行路径）

> 主线程第一人称热记忆 · 2026-09-05 · 乘客 #6（治理档）· harness 形态：判官 / 核验官 = 本会话派生的零上下文子 agent（architect / landing-verifier），主线程起草 + 落地 + 编排 · 跨乘客对照：本仓治理档第 6 次全环（对偶基线 260830-arch-review-ui-impl-strategy）。

**【硬项 R1】** verdict 绿后即写，热记忆，未隔场。

**【硬项 R2】** 无逐相位 token 计量表；§2 用子 agent usage 实测值 + 主线程用量为估计值（误差 ±30%），如实声明。

**【硬项 R3】** §3 每条 F 带现象 → 根因 → 证据锚。

**【硬项 R4】** §4 每条建议标载体。

**【硬项 R5】** §5 含保险丝判定。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | PROPOSAL v0.1（起草时点在 2026-09-04，含 human 立项 ack + OQ 三裁，先于本会话） | 09-04 | 09-04 | — | — |
| 评审 R1 | architect 全证据实核（16 tool_uses）→「需修订」F1/F2 | 09-05 | 09-05 | ~6 min | 52,022（实测） |
| 修订 | 主线程按 F1/F2 改 PROPOSAL（T4+§3+manifest） | 09-05 | 09-05 | ~5 min | ~15k（估） |
| 评审 R2 | architect 复核 → GREEN | 09-05 | 09-05 | ~1 min | 86（实测） |
| FROZEN | 状态行 + INDEX 钉行 | 09-05 | 09-05 | ~1 min | ~3k（估） |
| landing | 主线程 T1–T4 全落地（9 文件） | 09-05 | 09-05 | ~25 min | ~60k（估） |
| 核验 | landing-verifier（29 tool_uses）→ GREEN | 09-05 | 09-05 | ~9 min | 57,818（实测） |
| 收尾 | T4.6 ARQ 移行 + retro + 合并门 | 09-05 | 09-05 | ~10 min | ~20k（估） |
| **合计** | | | | **~57 min（本会话段）** | **~208k ±30%** |

对照：260830 批（spec/文档档）全环约一个下午；本批是 G3 脚本档、评审与核验均一次过修订即绿，节奏更快。

## 2. Token 账

计量方式诚实声明：判官与核验官为子 agent usage 实测；主线程起草/落地/收尾为估计（±30%）；PROPOSAL 起草段在 09-04 场次、未计入本表。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（修订 + FROZEN + landing + 收尾） | ~98k（估） | ~47% | 落地编辑占大头 |
| 判官（R1+R2） | 52,108（实测） | ~25% | R1 逐证据实核是大头，R2 极轻 |
| 核验官 | 57,818（实测） | ~28% | 结构门 + 逐文件 diff + V1–V4 复跑 |

花费最大处：主线程 landing 编辑（9 文件 × 逐处精准改）。可压缩分析：R1 的证据实核（PROPOSAL §1 证据密度高）物有所值——它实测出「全部逐字命中」，让 R2 只花 86 tokens；本批无需压缩。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · V4 重放实证出升级闸 live 路径存量缺陷（off-main 恒 fail-closed）**：现象 = V4 双向重放里 off-main 理由恒为「无 main ref 无法判」而非预期「source-on-main」→ 根因 = `evaluate` 的 off-main 判定依赖 `ctx.execFn`，但 live 入口 `buildCtx` 返回的 ctx 从未带该字段（undefined → revParse 抛错被吞 → fail-closed 分支）；self-check 能过是因为 mkCtx 手工注入了 execFn → 证据锚 = `pre-merge-escalation-gate.mjs` buildCtx 返回对象（无 execFn 键）+ `evaluate` 内 `sourceReachableFromMain(ctx.execFn, ...)` + V4 实测输出（本 summary §2）。**未修**——不在 FROZEN manifest 半径，改之破 packet fidelity；方向 fail-closed 无错误放行，留 ARQ。
- **F2 · V3 存量失败混入验证集**：现象 = `merge-driver --self-check-quick-checks` FATAL（verify-gates.json 缺 `types` 门定义）→ 根因 = 存量环境缺口，与本批零交集 → 证据锚 = `git stash` 后 HEAD 复跑同报错（本 summary §3.2）。教训：治理批验证集里引用「既有自检回归」时，应先记录这些自检在 HEAD 的基线状态，否则 landing 期无法区分存量/新增。
- **F3 · Agent 派发通道中断（会话开头）**：现象 = auto mode 安全分类器不可用，architect 派发连续 3 次被拦 → 根因 = 平台侧分类器依赖同模型实例暂时不可达 → 证据锚 = 本会话记录（claude-sonnet-4-6[1M] unavailable）。处置 = 等待窗口内主线程先用只读工具预核 §1 证据（副产品：预核结果供判官交叉对照），human 切模型后恢复。教训：派发被拦时用只读预核填充等待窗口，不空转。
- **near-miss（最险一刻）**：V4 对照组跑出「off-main 无 main ref 无法判」时，一度想顺手在升级闸里补 `ctx.execFn` 一个字段了事——该改动一行、看起来无害，但它不在 FROZEN manifest 里，落了就是 silent scope creep、landing-verifier 必打回。没出事是因为 packet-binding 纪律先于「顺手修」冲动：记进 summary 插曲、留 ARQ。教训：FROZEN 后发现的一切存量缺陷，无论多小，只记录不修。

## 4. 优化建议（按 ROI 排序）

1. **登记升级闸 execFn 存量缺陷 ARQ**：治 F1 · 载体 = ARQ 收件箱（gov-arq 快路径，修复面小：buildCtx 补一字段 + live 冒烟断言）· 预期收益 = 升级闸 off-main 判定从「恒 fail-closed 不可判」恢复真判，且给「self-check 绿 ≠ live 绿」补一个实例锚。
2. **登记 verify-gates.json 缺 types 门 ARQ**：治 F2 · 载体 = ARQ 收件箱 · 预期收益 = V3 三项自检恢复全绿基线。
3. **验证集基线前置**：治 F2 复发 · 载体 = 登记不硬治（下次治理批起草时把「引用的既有自检在 HEAD 的基线状态」写进 PROPOSAL §5）· 预期收益 = landing 期存量/新增判定零歧义。
4. **PROPOSAL 证据密度分级**：观察项（非本批问题）· 载体 = 留批次 · R1 逐字实核成本高但本批抓到 0 缺陷；若后续 PROPOSAL 证据持续全命中，可考虑判官抽核比例化（本批已按 medium tier 抽核一半，未见过度）。

## 5. 一句总结

成本 ≈ 208k tokens / ~57 min（本会话段，起草段在 09-04）/ human 触点 2 次（立项 ack+OQ 三裁、模型切换救场）/ 制品 5 件（PROPOSAL + architect-landing-summary + landing-verdict + retro + INDEX/ARQ 钉行）· 最险一刻 = V4 揪出存量缺陷后的「顺手修」冲动被 packet-binding 纪律拦住 · **保险丝判定：两方独立结构接住作者偏差**——判官 R1 在主线程已预核一半证据的前提下仍独立抓出 census 两处漏面（F1 升类 code-review-gate.md 是主线程完全没看到的），核验官独立复跑 V1–V4 并对存量缺陷做构造性实证，两方都没有采信主线程自述 · 最值得带走的一条：**FROZEN 后发现的存量缺陷只记录不修，是保真度纪律的试金石**。

## 6. 本环遗留

- 升级闸 live 路径 `ctx.execFn` 缺失（off-main 恒 fail-closed）→ 去 ARQ 收件箱（§4.1）。
- verify-gates.json 缺 `types` 门定义（V3 之一存量红）→ 去 ARQ 收件箱（§4.2）。
- R1 观察项（五要素不含事件时序绑定，重放类残差）→ 已被 PROPOSAL §6 留根第 1 条覆盖，不再单列。
- 方向 B 留根 10 销账 → 待 G/WS 两包 ESCALATE 合并时实证（PROPOSAL §6 第 4 条）。
