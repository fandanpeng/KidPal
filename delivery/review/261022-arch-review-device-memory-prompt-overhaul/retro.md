# retro · 261022-device-memory-prompt-overhaul（Track B 全环：架构+契约面 主路径，跨面列车第 2 面）

> 主线程第一人称热记忆 · 2026-10-22 · 本批 = 同日跨面列车的第 2 面（triage ③拆包 → 需求面档 2 v1.67 frozen → 本面），全流程同一主线程会话内联推进 · harness 形态：architect / architect-reviewer / landing-verifier 均为独立零上下文子 agent 实例，起草与审稿不互兼。

**【硬项 R1】** verdict 绿后即写，第一人称热记忆，无隔场补记。
**【硬项 R2】** token 无逐相精确计量；§2 用子 agent 任务通知的 usage 汇总数 + 主线程会话上下文占比估计，误差界 ±20%。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 立项/起草 | spine 消费 + architect 起草 PROPOSAL v1.0（601 行） | 约 15:20 | 约 15:45 | ~25min | 起草 ~290k |
| 评审 R1 | 四查 + 判档（BLOCK F1-F3） | ~15:50 | ~16:05 | ~15min | ~150k |
| 修订 | F1-F3 窄修订（v1.1） | ~16:10 | ~16:15 | ~5min | （含于起草会话续跑） |
| 复核 R2 | 靶向复核 GREEN | ~16:20 | ~16:25 | ~5min | ~4k |
| FROZEN+landing | 标冻 + 七文件 packet-bound 落地 | ~16:30 | ~17:00 | ~30min | 主线程（未计量） |
| 核验 V1→V2 | landing-verifier FAIL(F1,F2) → 修复 → 聚焦复核 PASS | ~17:05 | ~17:35 | ~30min | ~150k |
| **合计** | 立项 ack 消费（spine）起 ~2.5h，human 触点 0 次（本面；全列车触点①②另计） | | | | 全列车子 agent 合计 >1.5M |

对照：261019 需求档 2 批（触 UI、含出图与两轮终拍）同面耗时约一个工作日；本批无 UI、探针证据现成，明显更快。

## 2. Token 账

计量方式诚实声明：子 agent tokens 来自任务通知 usage 汇总（起草 290k / R1 150k / R2 4k / verifier 150k，±20%）；主线程与需求面共享会话无法拆分，不装精确。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（编排 + landing + 本 retro） | 未计量 | — | 会话累计已 >1.2M（含全列车前三面） |
| architect（起草 + 修订续跑） | ~300k | ~50% | 601 行 PROPOSAL + 逐节 fence 文本是大头 |
| architect-reviewer（R1 + R2） | ~155k | ~25% | R1 全量四查是大头，R2 靶向极省 |
| landing-verifier（V1 + V2） | ~150k | ~25% | V1 逐文件人读 diff 是必要成本 |

花费最大处：PROPOSAL 起草（601 行 + 七文件 fence 文本全量）。可压缩分析：改法集 fence 文本是 landing 的逐字依据，不可省；可省的是起草期 sweep 的部分实跑留痕（但它们正是 R1 一次仅三窄 finding 的原因——不压缩）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · landing 漏落风险 14 行**：现象 = T2-5 fence 是「风险 9 追加 + 风险 14 新行」复合件，主线程落地时执行了追加、漏了新行 → 根因 = 复合 fence 无逐件核对清单，落地按「改法语义」而非「fence 逐段」执行 → 证据锚 = landing-verdict F1（grep `^| 14 |` = 0）。教训：**复合 fence 必须拆成逐段 checklist 再落地**。
- **F2 · landing-summary 形态错**：现象 = 文件名 `landing-summary.md`（应为 `architect-landing-summary.md`）+ manifest 无围栏 → 结构门首跑即红 → 根因 = 未先读结构门脚本的硬编码约定（.claude/scripts/landing-fidelity-check.mjs 与既有 11 批次先例）→ 证据锚 = verifier F2。教训：**制品文件名/形态先查门脚本与先例批次，再写**。
- **F3 · PROPOSAL 状态行 FROZEN 标注假成功**：现象 = python replace 目标串不存在（no-op），但子串存在性检查误报成功 → 根因 = 用 `in` 检查（匹配到追加节文本）而非检查目标行本身 → 证据锚 = landing 后 PROPOSAL :3 仍 DRAFT（grep 复核发现）。教训：**替换断言必须锚定目标行、不是任意子串**。
- **F4 · python 批量落地整批回滚**：现象 = 四个替换一个锚未命中 → assert 全批中止，前三个已改的丢失需重跑 → 根因 = T5-4 锚行凭想象引用（「docker inspect 核 bind-mount 两映射」实际行文无「两」字）→ 教训：**锚必须现读现取，不凭 PROPOSAL 转述**。
- **near-miss（最险一刻）**：R1-F1 的机械齿自相矛盾（「部署期首验」计数断言预期 1、落地实况 2）——若未被抓，verify-deploy/核验链会在合并后报红，且契约 fence 与验证断言互斥的 PROPOSAL 会被冻结成「自证不可能绿」的packet。没出事的原因 = reviewer 对 §5 验证集做了逐条复算而非抽样。教训：**验证断言本身是最高危的评审对象**。

## 4. 优化建议（按 ROI 排序）

1. **landing 前置 checklist 自动化**：治 F1/F2 · 载体 = ARQ 收件箱（建议 landing-fidelity-check.mjs 增「fence 段计数 vs canonical 落点计数」自动对账，当前只对文件级集合） · 预期收益 = 复合 fence 漏段落类缺陷在主线程落地时即报，verifier 不再是第一发现人。
2. **制品形态速查卡**：治 F2 · 载体 = 登记不硬治（landing-summary 文件名/围栏形态已在本 retro 留痕；arch-change.md 可补一句「summary 文件名 = architect-landing-summary.md 硬编码」）· 预期收益 = 免一次 FAIL 轮。
3. **探针证据前置**：本批四查仅三窄 finding、R2 零新 finding 的主因 = 起草输入里带全量实测证据（debt 七组）+ 起草期现读现跑留痕 · 载体 = 留批次（下一列车沿用「探针先行 + 证据入 debt」模式）· 预期收益 = 评审轮次持续压在一轮。

## 5. 一句总结

本面 ~2.5h / human 触点 0（ack 与 FROZEN 均凭 spine 已裁事实）/ 制品 5 件（PROPOSAL v1.1 FROZEN + review 记录 R1/R2 + architect-landing-summary + landing-verdict PASS + 本 retro）/ 对照 261019 批显著轻快（无 UI、证据现成）· 最险一刻 = R1-F1 自相矛盾断言险些冻成死 packet，reviewer 逐条复算接住 · **保险丝判定：两方独立结构接住作者偏差**——R1 抓出起草侧自洽性缺陷（F1-F3），verifier 抓出主线程落地漏项（F1/F2），两道独立防线各接一次，作者（主线程+architect）自身检查全未发现 · 最值得带走的一条：**复合 fence 逐段 checklist + 制品形态先查门脚本**。

## 6. 本环遗留

- OQ-10（记忆库备份策略）待 human 终裁（推荐 (a) 不备份靠重建 + 重建源枚举/重评触发两条记账）。
- 一次性回合 prep 税复测（P1/P3）、抽取策略实现形态（deriver 可配置性 P3 早验）、多设备身份、删除自助界面、目录归纳——均已在 PROPOSAL §6.4 留根，归实现批/后续批。
- debt 收口：TECH-261022 主触发条目随 P1–P5 落地结项；TECH-261018 由 P5 消费；prep 税复测回填 TECH-261022-openclaw-prep-tax-memory-pressure。
