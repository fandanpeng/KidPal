# retro · 260928-gov-review-dev-loop-preassess-manifest-scope（Track B 全环：三合一治理变更起草→评审三轮→landing→修复→复验 PASS）

> 主线程第一人称热记忆 · 2026-09-28 · harness 形态：判官（architect）/ 核验官（landing-verifier）= 同会话子 agent，SendMessage 续用同一实例（判官 R1→R3 保上下文；核验官 FAIL→PASS 保上下文）。

**【硬项 R1】** verdict 绿后即写，第一人称热记忆，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent token 计量 = harness usage 回传（subagent_tokens 字段，精确）；主线程 token 无独立计量 = 估计（按会话长对话 1M 级窗口计，误差 ±50%）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | plan mode 探查 + PROPOSAL v0.1（含 human 三取舍点 AskUserQuestion） | 2026-09-28 | 2026-09-28 | 同日会话内 | 主线程，未单计 |
| 评审 R1 | 判官 BLOCK（B1–B7 + S1–S5 + N1/N2） | 2026-09-28 | 2026-09-28 | ~6 min | 105,717 |
| 修订 v0.2 | B1–B7 闭环 + S 全采纳（commit 1b471a4） | 2026-09-28 | 2026-09-28 | 会话内 | 主线程 |
| 评审 R2 | 判官 BLOCK（仅 F1/F2 条文级） | 2026-09-28 | 2026-09-28 | ~3 min | 133,984（含 R1 上下文累计） |
| 修订 v0.3 | F1/F2 闭环（commit 256f433） | 2026-09-28 | 2026-09-28 | 会话内 | 主线程 |
| 评审 R3 | 判官 **GREEN**（含结构字段核对） | 2026-09-28 | 2026-09-28 | ~2 min | 152,380（累计） |
| FROZEN | 标注 + INDEX（commit d475458） | 2026-09-28 | 2026-09-28 | 会话内 | 主线程 |
| landing | T1–T12 落地 17 文件（commit 9d509b0） | 2026-09-28 | 2026-09-28 | 会话内 | 主线程（本环最大工作量段） |
| 核验 R1 | landing-verifier **FAIL**（F1 透传链断裂 / F2 _doc 失实） | 2026-09-28 | 2026-09-28 | ~7 min | 85,724 |
| landing 修复 | F1/F2 闭环（commit 2a42e02） | 2026-09-28 | 2026-09-28 | 会话内 | 主线程 |
| 核验 R2 | landing-verifier **PASS**（三态五测亲跑） | 2026-09-28 | 2026-09-28 | ~4 min | 100,367 |
| **合计** | 单日闭环；human 触点 2 次（三取舍点 + 预览后放行） | | | | 子 agent 合计 ≈578k |

对照：前批 260928-gov-review-portfolio-boundary-story-propagation 同日单环；本批判官轮次 3（B 系 7 条打回 + F 系 2 条），是熔断线（3 轮）内的满轮使用，R3 GREEN 未触发熔断。

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 判官（R1–R3，同实例累计） | 152,380 | ~26% | SendMessage 续实例省了重复上下文装载 |
| 核验官（R1+R2，同实例累计） | 186,091 | ~32% | FAIL 修复后复验 |
| 主线程（起草 + 评审修订 + landing + 修复） | 未单计（估计 1M 级窗口消耗，±50%） | — | landing 段最重（17 文件人读编辑） |

花费最大处：landing 段（17 文件逐个读改 + 六组自检实跑）。可压缩分析：判官 R1 的 7 条打回里 B1（编号撞号）/N1/N2 属起草期可自查的机械错（引据核了文本没核结构槽位）；下批起草 checklist 加「新增 Step 编号先 grep 占用」一条可省一轮。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · scope-file 透传链参数形态断裂（核验官抓获，本环最痛）**：现象 = verify.mjs/merge-driver 拼 `--scope-file=path` 等号形态，两 run-check callee 只认空格形态 → 透传静默落空，final 相位静默退化为全仓全量、该红不红 → 根因 = caller/callee 各自单测通过、接缝无端到端断言（两侧解析器同构错同向）→ 证据锚 = landing-verdict.md FAIL-F1（`node unit-run-check.mjs --phase=final --scope-file=/tmp/x` 红 vs 等号形态误报「缺 --scope-file」实证）。教训：**跨进程旗标契约必须有一端兼容双形态或加接缝自检**。
- **F2 · change-manifest 转义顺序 bug（自检自抓）**：现象 = glob `**/` 失配 → 根因 = 先转义字面量后挖通配 token，`**/` 被转义拆散 → 证据锚 = change-manifest-check.mjs globToMatcher 注释（self-check 16 断言首跑抓到）。正反馈：self-check 先行写的纪律直接变现。
- **F3 · 引据「文本对了、结构槽位没核」**：现象 = R1-B1 打回（新档编号 1c 撞已占用槽位 1c–1f、插位早于 TaskPackage 构造点）→ 根因 = 起草期核了引据逐字、没核 planner.md Process 的编号占用与执行时序 → 证据锚 = PROPOSAL §7 R1-B1（planner.md:45-48 实查）。near-miss 候补。
- **F4 · 「全量」残留清扫声明两度被扩**：现象 = v0.1 声明「不涉残留清扫」被 R1-B7 打回（实查 14+ 处），v0.2 词表仍漏 4 形被 R2-F1 再打 → 根因 = 按整句替换目标枚举而非按语义族全仓 grep，且同行多实例（build-feature.md:30 一行两形）会骗过逐词断言 → 证据锚 = PROPOSAL §7 B7/R2-F1。
- **near-miss（最险一刻）**：landing-verifier FAIL-F1 的形态——若核验官没做「caller 形态 vs callee 解析」接缝差异实证，圈定机制会在**集成路径静默失效**（verify.mjs gate-child 不传 --phase 给 run-check，callee 永远不红不圈），而六组自检全绿照常放行——本环核心目标（测试收敛）会以「自检全绿」的面目落空。没出事的原因 = landing-verifier 独立做了跨接缝差异实证（超出我给的 checklist 的主动性动作）。教训：核验官 prompt 里「独立核验、不信自报」不是客套，是本环真正接住缺陷的那根线。
- **F5 · changes-index upsert 首见 `mergedInMain: yes` 误判体验**：现象 = FROZEN 期 upsert 即显示已合 main → 根因 = 本仓治理批直接 commit main、工具按 commit 落点推导，非 bug → 处置 = 会话内核对后放行（本次 retro 留痕防后人重查）。

## 4. 优化建议（按 ROI 排序）

1. **跨进程旗标接缝自检义务成文**：治 F1 · 载体 = ARQ 收件箱（登记：caller/callee 分属两脚本的 CLI 旗标，落地义务含「一端兼容双形态 ∨ 端到端接缝断言」）· 预期收益 = 杜绝「各自单测绿、接缝静默断」类缺陷复发。
2. **起草期机械错自查三件套**：治 F3/F4 · 载体 = ARQ 收件箱（登记：① 新增编号槽位 grep 占用；② 语义族残留先全仓 grep 再写声明；③ 同行多实例断言写「逐形清」）· 预期收益 = 省 1–2 轮判官打回。
3. **landing-fidelity-check baseRef 是位置参数**：治核验期插曲 · 载体 = 登记不硬治（verdict 已留姿势注记防重蹈；脚本 `_doc` 补一句的成本低于改门）。
4. **pod-coordination.md:151 floor 注补齐**：治 R1-B7 边缘残留 · 载体 = 留批次（下批触碰 pod-coordination 时搭车，PROPOSAL §6-8 / landing-summary 已登记）。

## 5. 一句总结

单日闭环：human 触点 2 次、判官 3 轮（R1 BLOCK B1–B7 → R2 BLOCK F1/F2 → R3 GREEN）、核验官 2 轮（FAIL F1/F2 → PASS）、制品 4 件 + 17 manifest 文件 + 修复 commit；最险一刻 = scope-file 透传接缝静默失效被核验官主动性差异实证接住；**保险丝判定：两方独立结构（判官独立 + verifier 独立）接住了作者偏差**——判官接住编号/语境/清扫声明三类起草盲区，核验官接住全部六组自检都测不到的跨进程接缝缺陷（且是在主线程不知道该测什么的情况下）；最值得带走的一条 = **自检全绿 ≠ 机制生效：跨接缝契约必须有一端做差异实证**。

## 6. 本环遗留

- PROPOSAL §6 留根 1–7 全部随批生效（召回质量首三实包观察 / 全量人工触发位无 checklist 记 ARQ 候选 / merge-to-main skill 语义同步义务 / DEV-GOVERNANCE「全量」豁免）。
- 第 8 项（landing 期新增）：pod-coordination.md:151 floor 注补齐——留批次搭车（§4-4）。
- 第 9 项（核验期新增）：F2 处置 chosen = 回改 _doc 留痕；「docs-sync 面进对账」若将来要实现，需先立字段语义（避免发明）——登记不硬治。
- 判官 R3 两条 landing 注均闭环（dev-loop-operating-system.md:29 已清；loop-metrics.mjs:74 判词形族外未动）。
