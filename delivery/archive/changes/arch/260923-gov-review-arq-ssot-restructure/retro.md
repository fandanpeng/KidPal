# retro · 260923-gov-review-arq-ssot-restructure（Track B 全环：起草 v0.1→v0.5 → R1 BLOCK(10) → R2 BLOCK(4) → R3 GREEN → FROZEN v1.0 → landing → verifier PASS+5 ADVISORY）

> 主线程第一人称热记忆 · 2026-09-24 · 乘客编号未核（如实标：本仓 Track B 常驻批，未查既往乘客序号）· harness 形态：**typed `architect` 判官（三轮同实例 resume）+ typed `landing-verifier` 核验官**（本仓非零上下文 coder 降级形态）· 跨乘客对照：账本形态重构类首批（前无同型）。

**【硬项 R1 · 环后趁热、第一人称】** verdict PASS 后即写，合并前完成 ✓。

## 1. 时间账

**【硬项 R2 · 计量诚实】** 时长以子 agent harness 报告为准（精确）；主线程相位无独立计时，以会话跨度估计并标 ±；日期跨 09-23（方案讨论）与 09-24（执行），中间有 human 会话间断，「时长」列只计机器活跃段。

| 相位 | 内容 | 时长 | 备注 |
|---|---|---|---|
| 方案讨论 | human 四轮定方向（三域 / 档C / docs/crq / archive）| 跨 2 日、4 次 human 触点 | v0.1→v0.4 每次改向都由 human 输入触发 |
| 起草+脚手架 | PROPOSAL v0.1–v0.5 + 4 脚本 + 94 条迁移 + 实演 | 主线程估计 2–3 h（±50%，无计时） | 含 E1–E10 全部实测 |
| 评审 R1 | typed architect，60 tool uses | 53.8 min（harness 报告 3 228 859 ms） | BLOCK(F1..F10)，1 BLOCKING |
| 修订 v0.5 + R2 | 同实例 resume，16 tool uses | 10.5 min（627 824 ms） | BLOCK(G1..G4)，零 BLOCKING |
| 修订 + R3 | 同实例 resume，12 tool uses | 10.5 min（627 8xx ms 段报） | **GREEN** + 1 NIT |
| FROZEN + landing 收口 | 格式例外展开 + dogfood 登记 + summary | 主线程估计 <1 h（±50%） | 结构门首跑红→展开后绿 |
| 核验 | typed landing-verifier，69 tool uses | 13.4 min（801 386 ms） | **PASS + 5 ADVISORY** |

对照：评审三轮收敛（熔断线 = 3 轮未 GREEN，本批压线通过）；R1 53.8 min 为最大单段——全面审 + 独立重演合并场景的代价，物有所值（10 findings 全部成立）。

## 2. Token 账

计量方式诚实声明：子 agent 为 harness 逐段报告；**resume 段报告疑为累计口径**（R2 段 16 tool uses 报 270 314、R3 段 12 报 293 044——若为逐段独立值则与 tool 数不成比例），故判官总量取**末段累计 293k 为上界、202k+68k+23k 逐段差值为下界估计**，误差界 ±40%。主线程无独立计量，不装精确。

| 载体 | tokens | 备注 |
|---|---|---|
| 判官（R1–R3，同实例） | ≈293k（上界口径） | R1 占绝对大头（全面审 + scratch 重演） |
| 核验官 | 179 447（精确） | 含 V11 独立重演 + 10 条逐字抽核 |
| 主线程（起草 + 落地 + 自查） | 无计量 | 会话跨两日、含大量读盘 |

花费最大处：R1 全面审。可压缩分析：若起草期就按「同族数字复扫」纪律自查（F2 教训），R1-F6/R2-G1 两轮共 16 处数字残留 findings 及其修订往返可省——估计省判官侧 15–20%。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 两门目录锚口径差（最痛：FROZEN 后才炸）**：现象 = PROPOSAL manifest 写目录简写 `docs/crq/`，preflight 干跑**接受**（入锚并集），landing 结构门**拒绝**（逐文件精确匹配，claimed/planned-but-not-landed 双红）→ FROZEN 后被迫走格式例外、机械展开 99 行 + human 晨间追认。根因 = `landing-fidelity-check.mjs` 的 `gitChangedFiles()`/集合成员判定只认文件路径，而 `parseSection2AnchorLists` 的 token 提取接受目录形态——两门同一 token 口径不一致，且反馈延迟到收口期。证据锚 = 结构门首跑输出（summary §3-9）+ `landing-fidelity-check.mjs:613-615` 一带 + 已登记 `docs/crq/gov/ARQ-260924-landing-gate-dir-anchor-gap.md`。
- **F2 · 同族数字残留三连发**：现象 = R1-F6 六处（「23 项」「5/5」「14/14」「顺延」「5 份」…）→ v0.5 修 → R2-G1 又十处（「52 处」「16 处」「两份/三处」…）→ verdict 后 summary 又三处（26/12/63）。根因 = 同一事实在文档内多处引用（E 表、T 文、V 表、OQ、留根互相转引），改一处不扫同族即漂。证据锚 = §7 R1-F6 / R2-G1 逐处行号 + verifier ADVISORY-2/3。处置 = 起草期声明⑥复扫纪律 + 留根 18（升格模板候补）。
- **F3 · title 整字段漏迁、机械对账全绿掩护**：现象 = 首迁 94 件 ID 集合全等、长度对账可解释差额——全绿；渲染后人读发现 INDEX title 列**全空**（migrate 的 fm 漏了 title 键）。根因 = 对账基线取自「意图」而非「渲染形态」，字段整体缺失在两侧同时缺失 ⇒ 长度差不显。证据锚 = §0-E8。
- **F4 · 迁移源被自己的落地动作覆盖**：现象 = stub 落盘后复跑 migrate → 源 0 行。根因 = 单文件既是迁移源又是落地目标，顺序耦合；真空绿守卫 FATAL 拦住（按设计生效）。证据锚 = §0-E9 + `crq-migrate.mjs --src` 头注。
- **near-miss（最险一刻）**：F3 的 title 全空——**若当时没手贱打开渲染出的 INDEX 看一眼**，94 件空标题条目 + 3 份空列 INDEX 会带着「机械对账全绿」的假象直接进入评审与 commit；判官抽 4 条核的是**条目文件**（正文在），INDEX 空列要到 human 晨读才炸。没出事的唯一原因 = 人读了渲染形态。教训：迁移类批次的验收必须包含「**渲染形态人读核**」，源↔目标对账不能替代。

## 4. 优化建议（按 ROI 排序）

1. **目录锚口径差**：治 F1 · 载体 = **已登记 CRQ 条目** `ARQ-260924-landing-gate-dir-anchor-gap`（触发 = 下次动 preflight/门解析面的治理批；候选 ①preflight advisory + ③模板纪律句，不预批 ②动承重门）· 收益 = 迁移类批次不再在 FROZEN 后炸格式例外。
2. **「改一处数字 → grep 同族全文」固化为 FROZEN 前机械动作**：治 F2 · 载体 = 留根 18（升格 `PROPOSAL.template.md` 起草 checklist，下批裁）；本批已以起草期声明⑥自行生效 · 收益 = 每轮评审少 5–10 条数字残留 findings。
3. **迁移类批次验收加「渲染形态人读核」硬项**：治 F3/near-miss · 载体 = 留根 18 同批带（模板 checklist 增一句「迁移/生成类改法，验收含渲染产物人读核，不得仅以源↔目标对账替代」）· 收益 = 堵「全绿假象」类最险失败模式。
4. **severity 用词与 rubric 归一**：载体 = PROPOSAL 留根 13（另批）· 收益 = 消 5 条常驻 advisory。
5. **render 自检标称 8/8 实为 7 组**：治 D5 · 载体 = 留批次（下次动 `crq-render.mjs` 顺带改打印；本批不改——FROZEN 文本引「8/8」，改代码打印会造新矛盾）· 收益 = cosmetic。

## 5. 一句总结

三轮评审 + 一轮核验共约 470k 子 agent tokens（上界口径，主线程未计）、human 触点 5 次（立项 + 三域/落点/archive 三裁 + 命名委托 + 「晨看再 merge」保留）、制品 4 件 + 自查清单 1 件 + 99 行格式例外展开；最险一刻 = title 全空被机械对账全绿掩护、靠人读渲染件才炸；**保险丝判定：两方独立结构接住了作者偏差**——判官 R1/R2 抓 14 处作者未觉（含 BLOCKING 的登记面缺 title、census 不可复现），verifier 独立重演 + 抽核抓 summary 3 处数字漂移 + 1 处空白变更，而主线程自查又补捉两侧都没点名的 adjudicator 写面漏改——**三层各有盲区、互为补位，互补性在本批得到实证**；最值得带走的一条 =「机械对账防不住字段整体漏迁——迁移批必须人读渲染形态」。

## 6. 本环遗留（条件节）

- **留根 18 条**照 FROZEN §6 保留（cdq 判据非机械化 / severity 归一 / workstreams·changes·taskpackages 豁免面 / 归档分片触发 / 模板升格候补等）。
- **verifier ADVISORY 5 项处置**：1（dogfood 越 manifest）= summary §3-10 已声明、 verifier 核实无自升级；2/3（INDEX 计数、归档计数）= summary 已更正（§3-12）；4（skill 尾换行）= 已补（Edit 工具，Bash 重定向被 enforce-scope 拦截——钩子活性实证）；5（自检标称计数）= 留批次（§4-5）。
- **格式例外待 human 晨间追认**（manifest 目录简写 → 99 行展开，先例 260913）；**OQ-2/3/5 按推荐位执行、同窗追认**；**merge 由 human 过目后执行**（human 指令「明早我看一下再merge」覆盖 Track B 自动合并默认）。
- 不关的根：`docs/crq/gov/` 12+1 条未结与 tech 15 条、requirement 1 条的**内容处置**全部不在本批（本批只搬容器）。
