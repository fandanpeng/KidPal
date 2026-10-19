# retro · 261019-gov-review-local-memory-extraction（Track B 全环：一轮 GREEN、一次闸制品落盘被拦的合规处置）

> 主线程第一人称热记忆 · 2026-10-19 · 乘客 #N（Track B 常态化后第若干乘客）· harness 形态 = typed agent（architect / architect-reviewer / landing-verifier 均为仓内 typed 定义）· 跨乘客对照： enforce-scope worktree 误拦第二次发生（首次 = 2026-10-18 architect 子会话，ARQ-261018）。

**【硬项 R1】** verdict 绿后趁热即写（landing-verdict PASS 落盘、INDEX 翻 landed、gov 分支 commit 6be24084 之后、合并 main 之前）。
**【硬项 R2】** 子 agent token/时长有 harness 实测（下表）；主线程自身无计量，按制品体量估算并标误差界。
**【硬项 R3/R4/R5】** 见 §3/§4/§5。

## 1. 时间账

| 相位 | 内容 | 时长 | tokens |
|---|---|---|---|
| 起草 | architect PROPOSAL v1.0 一次成形（preflight 首跑 RED 一次〔门缺陷抓获〕→ 修形态转绿） | 19.2 min | 90,149 |
| 评审 R1 | architect-reviewer 四查 + 判档，GREEN 一轮过（2 NIT） | 6.7 min | 57,554 |
| FROZEN | 凭「立项 ack（含第五阀显式批准）+ R1 GREEN」 | —（主线程动作） | — |
| landing | 主线程 T1–T4 六处编辑 + V1–V5 验证 + landing-summary | ~25 min（估，±10） | 无计量 |
| 核验 | landing-verifier PASS + 2 ADVISORY（写盘被 hook 误拦 ×2，主线程机械转写） | 5.6 min | 47,945 |
| **合计** | 子 agent 三段实测 31.5 min + 主线程落地段 | ~55 min（估，±10） | 195,648（子 agent 实测） |

对照：一轮 GREEN + 零否回，评审成本是近期 gov 批的低位；起草成本含一次门缺陷规避返工（T3 形态重写）。

## 2. Token 账

计量方式诚实声明：子 agent 三段 = harness 任务通知实测值；主线程（编排 + 落地 + 本 retro）无计量，不算入合计、不装精确。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（编排 + 落地 + retro） | 无计量 | — | 制品 = landing 六处编辑 + summary 39 行 + 本件 |
| 判官（R1） | 57,554 | 29% | 一轮过 |
| 起草（architect） | 90,149 | 46% | 含起草期实证义务（逐文件 grep 基线 + 源码亲核） |
| 核验官 | 47,945 | 25% | 含两次 Write 被拦的无效消耗 |

花费最大处 = 起草（实证义务逐条跑基线是主要体积来源，属必要成本）。可压缩分析：verifier 的两次被拦 Write 是纯浪费（约数千 token 级），治它的就是 ARQ-261018 的修复，已在升格。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · enforce-scope 误拦闸制品落盘（二次发生）**：现象 = landing-verdict 两次 Write 被拦，路径被解析为 `../KidPal-gov/...` 判越界 → 根因 = ARQ-261018 已诊的 worktree gitdir 锚定 off-by-one（`enforce-scope.mjs:141-145` dirname^3 多剥一层）→ 证据锚 = verdict 文件「核验过程事件」段 + ARQ-261018 追注（2026-10-19）。本次处置链 = 核验者如实报告不绕行 + 主线程机械转写 + trigger 命中升格 MAJOR——合规形态，未重演首次的 Python 绕行。
- **F2 · FROZEN 验证集两处措辞瑕疵**：现象 = V2c 阈值 ≥5 与 `grep -ciE` 按行计数语义不匹配（实测 4 行恰为四要点行）+ 命令内 `\|` 表格转义（判官 R1-F1 先抓获）→ 根因 = 起草者按「关键词在场」直觉估阈值、未按命令计数语义校准；验证集写进 markdown 表格时未防管道符转义 → 证据锚 = landing-summary 诚实注记① + verdict ADVISORY-1 裁断（认可非落地缺陷）。两处同源：**FROZEN 验证集的命令必须起草期按最终形态实跑一遍并核对计数语义**。
- **F3 · 批制品起步落错 worktree**：现象 = ARQ 登记与 PROPOSAL 起草在主 worktree 产出，FROZEN 前才发现 off-main 校验会拦，整体迁移 gov worktree（diff -r 核一致后主 worktree 还原）→ 根因 = 主线程在 main worktree 会话里起手治理批，worktree 路由没有在批启动时先判 → 证据锚 = landing-summary 诚实注记③。迁移零损失但白付一轮搬运。
- **near-miss（最险一刻）**：landing-verifier 若像 2026-10-18 首次事件那样用 Python/Bash 绕行写 verdict，闸制品就会绕过 scope 护栏落盘、且「绕行」不会像这次一样被升格记录——没出是因为 verifier 守住了「不用 Bash 重定向代写」的红线、选了如实报告。教训：护栏误拦时的处置路径本身就是治理资产，本次已写进 ARQ-261018 追注作范式。

## 4. 优化建议（按 ROI 排序）

1. **修 enforce-scope worktree 锚定 off-by-one**：治 F1 · 载体 = ARQ-261018（已升格 MAJOR、trigger「下一治理批必带」）· 预期收益 = 消除所有 worktree 内子 agent 的合法写误拦 + 杜绝绕行诱因。
2. **FROZEN 验证集起草纪律补一句「命令按最终形态实跑 + 阈值按命令计数语义校准」**：治 F2 · 载体 = 登记不硬治（验证集措辞级校准候选，verdict ADVISORY-1 同款建议；后续批触碰模板自检块时顺手评估是否并入④）· 预期收益 = 验证集「冻结可判性」名实一致，免 verifier 裁断开销。
3. **治理批启动即判 worktree 路由**：治 F3 · 载体 = 留批次（gov-change SSOT 或主线程习惯层，下一 gov 批起步时验证「gov 批从 gov worktree 起手」是否应成文）· 预期收益 = 省一次迁移搬运 + 消除主 worktree 短暂污染窗口。
4. **门行为两件观察（§2 边界探测不跳围栏 / verbatim-quote 行级匹配假阴性）**：载体 = ARQ 收件箱（PROPOSAL §6 已立第一件 + verdict ADVISORY-2 第二件，登记动作归主线程后续补登）· 预期收益 = 门报错可信度修复，起草期少付规避返工。

## 5. 一句总结

成本（子 agent 195,648 tokens 实测 / 全环约 55 分钟估）· 触点（human 三次：前端两裁 + 立项 ack/第五阀一次，合并前无新增触点）· 制品 4 件齐 + ARQ 一登一升格 · 对照前乘客 = 一轮 GREEN 零否回的低位评审成本。最险一刻 = verifier 险些有理由绕行护栏（F1/near-miss）。**保险丝判定：两方独立结构接住作者偏差**——判官 R1 独立抓获 V2c 转义瑕疵（F1）与例证失准（F2）两条 NIT，verifier 独立裁断阈值瑕疵并对敏感面逐字核，两处作者侧偏差均被独立面接住、无带病入库。最值得带走的一条：**护栏误拦时「如实报告 + 机械转写 + 升格」是合规处置范式，绕行才是事故**。

## 6. 本环遗留

- ADVISORY-1（V2c 阈值措辞瑕疵）：裁断不阻断，留根 = 验证集措辞级校准候选（建议 2）。
- ADVISORY-2（verbatim-quote 行级匹配假阴性）：ARQ 候选，待主线程补登。
- PROPOSAL §6 留根五项：批外删 memory（verdict 绿后立即执行）、ARQ-261019 结项（合并 main 后按账本纪律原地翻字段）、门缺陷观察（建议 4）、description 不补（登记不硬治）、存量「齿」不专项清扫（防误立项声明）。

