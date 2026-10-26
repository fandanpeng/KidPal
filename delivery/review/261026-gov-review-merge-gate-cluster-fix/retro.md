# retro · 261026-gov-review-merge-gate-cluster-fix（Track B 全环：合并闸/强制执行层六根一次收口）

> 主线程第一人称热记忆 · 2026-10-26 · 单批（无乘客并列）· harness 形态：typed architect / architect-reviewer / landing-verifier 子 agent（各 resume 语境延续）· 无跨乘客对照——本批是 P1 排产首单，对照基线取本批自身起草期测量。

**【硬项 R1 · 环后趁热、第一人称】** landing-verdict PASS + 7 ADVISORY 当轮即写，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent 用量来源 = harness task notification 的 usage 字段（architect 四轮与 reviewer 三轮的数字为**同 agent resume 累计值**，非单轮增量——下表按累计口径填并注明）；主线程 token 无计量，仅记工具调用次数级体感。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 每条 F 均带证据锚。

**【硬项 R4 · 建议必标载体】** §4 每条建议标载体。

**【硬项 R5 · 保险丝判定】** §5 明确判定。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens（子 agent） |
|---|---|---|---|---|---|
| 起草 | architect v0.1（复证六条 + 原型 + fixture 9 份） | 2026-10-26 | 同日 | ~16.5 min | 289,735（134 调用） |
| OQ 修订 | v0.2（human 四裁回传：允许集收窄重测） | 同日 | 同日 | ~4.1 min | 336,226（累计） |
| 评审 R1 | 判官 BLOCK(F1,F2,F3)+NIT（全量亲核） | 同日 | 同日 | ~13.9 min | 110,808（48 调用） |
| 修订 | v0.3（F1–F3+NIT） | 同日 | 同日 | ~3.0 min | 363,793（累计） |
| 评审 R2 | 判官 BLOCK(F4)（四项到位 + 补获漏检） | 同日 | 同日 | ~5.3 min | 127,819（累计） |
| 修订 | v0.4（F4 单行 + 反向实测） | 同日 | 同日 | ~0.8 min | 370,619（累计） |
| 评审 R3 | 判官 GREEN（单点） | 同日 | 同日 | ~1.6 min | 134,590（累计） |
| FROZEN → landing | 主线程逐 T 落 7 文件 + 15 夹具 + landing-summary | 同日 | 同日 | ~30 min（含 4 组实跑） | 主线程（无计量） |
| 核验 | landing-verifier PASS + 7 ADVISORY | 同日 | 同日 | ~11.9 min | 100,578（45 调用） |

对照：起草是最大单相位（复证 + 原型 + fixture 全套前置），判官 R1 的全量亲核约 14 分钟——评审成本集中在第一轮，R2/R3 收敛期逐轮减半以下。

## 2. Token 账

计量方式诚实声明：子 agent 数值来自 harness 通知字段，architect / reviewer 为 **resume 累计**（单轮增量不可拆，误差界 = 最后两轮差值量级 ±10%）；主线程无计量。

| 载体 | tokens | 备注 |
|---|---|---|
| 主线程（编排 + 落地 + 本文） | 无计量 | 约 40 次工具调用（读锚区段 + 12 处 Edit + 6 组实跑） |
| architect（累计四轮） | ~370,619 | 复证 + 原型 + 三次修订 |
| architect-reviewer（累计三轮） | ~134,590 | R1 全量 + R2/R3 聚焦 |
| landing-verifier | ~100,578 | 独立全量复跑 |

花费最大处 = architect 的起草期复证与原型（复证六条 ARQ + 9 份 fixture + 反向变异是快路径就绪的必要成本）。可压缩分析：R1-F1/F4 两处规格不完整若在起草时以「替换段自足性」自检（声明 + 组装 + 消费三件同稿）可省一轮判官往返。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 替换段规格不完整两连（R1-F1 / R2-F4）**：现象 = 判官连续两轮抓出「代码段引用了不存在的接源/声明」（`ctx.diffPaths` 无 buildCtx 组装、`notApplicable` 无声明——后者在 dev/gov 公共路径上逐字落地即崩）→ 根因 = 原型先行、文字稿后写：原型里这些行存在，誊进 PROPOSAL 的替换段时只摘了「变化的部分」，未按「可逐字落地的自足件」重构 → 证据锚 = PROPOSAL §7 R1-F1（Grep `diffPaths` 零命中）/ R2-F4（Grep `notApplicable = false` 零命中）。
- **F2 · 夹具无判别力两连（两处同根、两处都真红过）**：现象 = ① 起草期手注 `overlap.tier` 的夹具删掉 carve-out 后仍全绿；② 落地期混撞夹具首轮 65/1 红（`firmware/main.c` 不在双侧改动交集被噪声滤清剔除）→ 根因 = 测试层级错位：夹具测了 `evaluate` 的分支消费、没测探测器的产出；交集语义没进夹具构造 → 证据锚 = PROPOSAL §2 T5 起草期自曝注记 + 本批落地 V1 首轮实跑输出。教训 = 闸类夹具一律函数级注入（直调 probe、execFn 造假 git 输出）且混撞件必须 ∈ 交集。
- **F3 · 转述链精度三连小伤（V5 NIT + A3 + A7）**：现象 = V5 预期写了命令不输出的第三字段；summary 注记 2 引用「判官口径」无出处、注记 5 计数与 §6 实枚举差一 → 根因 = 手写转述未逐字回对来源 → 证据锚 = landing-verdict ADVISORY A3/A7。无落地缺陷，全是「引用精度」面。
- **near-miss（最险一刻）**：主树会话里的钩子副本在合并 main 前仍是旧代码（verifier A6）——若此刻有人从主树会话跑跨树写入探针，会被旧钩子误拦并可能再次触发「绕行 Python 直写」的事故形态。没出事的原因 = 本批全程在 gov worktree 落地与验证、主树未动。教训 = 钩子/闸类落地后、合并前是「两树行为不一致」窗口，验证动作要钉在落地树。

## 4. 优化建议（按 ROI 排序）

1. **「替换段自足性」起草自检**：治 F1 · 载体 = ARQ 收件箱（登记一条 SHOULD：PROPOSAL.template 起草期自检增一句——每个替换代码段必须包含其引用的全部新标识符的声明与组装，Grep 自证）· 预期收益 = 每批省 0.5–1 轮判官往返。
2. **闸类夹具「函数级 + 交集意识」约定**：治 F2 · 载体 = 登记不硬治（本批 T5 证据段已自曝留痕；若再现一次同根假绿，升 ARQ 进 template 硬项）· 预期收益 = 消灭「照镜子」夹具形态。
3. **summary/retro 引用「现读再粘」**：治 F3 · 载体 = 已有纪律（auto-memory `carry-canonical-briefs-verbatim` 同族）——本批主线程对判官简报做到了，对 summary 注记没做到；复诵成本已付，无需新机制 · 预期收益 = 消灭 A3/A7 型精度瑕疵。
4. **跨树落地的「两树不一致窗口」注记**：治 near-miss · 载体 = 留批次（钩子类批的 landing-summary 模板可加一行提示；等下一个钩子批再定）· 预期收益 = 防合并前主树侧误判。

## 5. 一句总结

成本 = 子 agent 三角色 ~606k tokens（累计口径）+ 主线程半日内闭环，human 触点 2 次（立项 ack + 四 OQ 一并、无中场追加）；制品 4 件齐（PROPOSAL 754 行 + landing-summary + landing-verdict PASS+7A + 本文）；最险一刻 = 合并前主树钩子副本旧代码窗口（未触发）；**保险丝判定：两方独立结构接住了作者偏差**——判官两轮抓出作者（architect）替换段的两处规格不完整（其一 R1 漏检、R2 补获，双判互备成立），核验官独立复跑全部验证集并抓出主线程 summary 的三处精度瑕疵；最值得带走的一条 = **原型的「能跑」不等于文字稿的「可逐字落地」，替换段必须自足**。

## 6. 本环遗留

- **ADVISORY 7 条处置**：A1（V14/V15 post-commit）→ commit 后主线程执行并记录；A2（反向变异不可复跑）→ 接受，判别力由夹具注入旧形态 + T5 首轮红自证；A3/A7（summary 精度）→ 已于 verdict 后修正 summary 两处文字；A4（fixture-nonempty 常真断言）→ 接受（字面夹具形态，无判别力损失）；A5（T3 注释漏 workstreams token）→ 已补（注释级，自检复跑 66/66 绿）；A6（主树副本旧代码窗口）→ 合并 main 后自愈，无需动作。
- **留根 10 项**：去向 = FROZEN §6（260924② 拆出后批、ARQ-261026-final-phase-dispatch-contract-unwritten 已登记、input-brief / 双源集 / INDEX 陈旧检测 / T4③ / 闸 cwd / 子模块 / FATAL 语义 / 账本枚举边界各带复议触发）。
- **六条 ARQ 结项**：verdict 绿后主线程原地翻字段（closed + closedBy = 本批 landing-verdict），随落地 commit 入账。
