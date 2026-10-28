# retro · 261028-font-typography-scaleup（Track B 全环：架构文档批 §5.5.3 字体管线换代 + ADR-017）

> 主线程第一人称热记忆 · 2026-10-28 · 乘客 = 字体列车架构面批（同日第 3 批：需求档 2 收口 → 直修 commit → 本批）· harness 形态 = 起草/判官/核验官均为零共享上下文子 agent，主线程纯编排 + packet-bound 落地。

**R1**：verdict 绿后即写，热记忆，无隔场补记。
**R2**：subagent token 有 harness usage 计量（起草 202,522 / 判官 65,306 / verifier 约 12 万级未读回执——以「约 ±20%」声明）；主线程 token 无逐相计量，按会话承载估计。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | architect 起草 PROPOSAL（含实跑基线 A-D） | ~10:47 | ~11:02 | ~15min | 202,522 |
| 评审 R1 | architect-reviewer 四查 + 判档 | ~11:08 | ~11:15 | ~7min | 65,306 |
| FROZEN | 主线程折入 2 注记 + 翻状态 | ~11:16 | ~11:17 | ~1min | 主线程 |
| landing | 装配器 7/7 → packet-bound 复制 → 落后核验组 | ~11:18 | ~11:24 | ~6min | 主线程 |
| 核验 | landing-verifier 独立核验 | ~11:25 | ~11:35 | ~10min | 未读回执（估 12 万级 ±20%） |
| **合计** | | | | **约 40min** | **估 39 万级 ±20%** |

对照：同日需求面档 2 批（author 两轮 + architect + uiux-author + reviewer 五查 + 出图三轮）约 3 小时——架构文档批无出图与多轮否回，明显更轻。

## 2. Token 账

计量方式：subagent 用量有 metered usage（逐次通知载明）；verifier 回执未读按同类 verifier 批估计（±20%）；主线程无逐相 metering（会话级）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 起草 architect | 202,522 | ~52% | 实跑基线 A-D 逐项取证占大头 |
| 判官 R1 | 65,306 | ~17% | 独立复扫四查（未采信转述） |
| 核验官 | ~120,000（估） | ~31% | cite-check 定性 + diff 对账 |

花费最大处 = 起草期实跑取证；可压缩 = 基线类取证（分区表、grep 断言）可由主线程预跑随 dispatch 附上，判官抽查即可——但「触面事实查出来不是申报出来」纪律下保留独立实跑有其价值，不压。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · cite-check 工具 v1 无 replaced 语义**：现象 = 落地后跑 --cite-check 报 ok:false（5 处「现文」missing）→ 根因 = 工具把「现文（verbatim）」引用一律当作「须在 canonical 命中」的存活断言，不区分「替换前引文落地后应消失」→ 证据锚 = cite-check 输出 `missing[]` 5 项全部为 T1/T3/T5 被替换旧文；verifier 独立对照 `git show HEAD:docs/architecture.md` 证实旧文在 HEAD 逐字在场。差点被当 FAIL 处理（near-miss 见下）。
- **F2 · 行尾形态双源**：现象 = canonical 两文件均 CRLF、PROPOSAL verbatim 引文为 LF，装配清单须按目标文件行尾手写 → 根因 = assemble-edits 有意不归一（语义 3，防静默改写行尾）但 PROPOSAL 模板未提示「引文行尾以 canonical 现状为准」→ 证据锚 = assemble-edits.mjs 语义 3 + eolHint 诊断路径；本批靠生成器脚本统一转 CRLF 消解。
- **F3 · 起草→落盘誊写环节**：现象 = architect 不写文件，主线程人工誊写 ~122KB PROPOSAL（含嵌套围栏）→ 根因 = 起草/落地写域分离的有意设计 → 保真风险被两层机械闸接住：preflight（锚清单形态）+ 装配器 7/7 恰 1 命中（oldStr 逐字对账 = 誊写保真的间接证明）；本轮零偏差。
- **near-miss（最险一刻）**：cite-check ok:false 出现的瞬间——若机械照「结构门 false = FAIL」处置，会误判落地失败甚至回滚正确落地。没出的原因 = 工具输出自带「不参与任何门判定」note + missing 项逐条肉眼可对「恰为被替换旧文」；verifier 独立取证收口。教训 = 「ok:false ≠ 门 FAIL」的定性必须落到工具文档或 verdict 惯例里，不能靠当场悟。

## 4. 优化建议（按 ROI 排序）

1. **cite-check 增 replaced 语义**：治 F1 · 载体 = ARQ 收件箱（登记：现文引用按 T 语义判 replace 场景预期 missing，落地后对账改「新文全中 + 现文对 HEAD 版本命中」双断言）· 预期收益 = 消除每批落地后一次人工定性 + 消除误判 FAIL 风险。
2. **结构门 base=HEAD~1 噪声**：verifier ADVISORY（连续两批 landing 时 changedCanonicalNotInProposal 列出上一批 commit 文件）· 载体 = 并入建议 1 同一 ARQ · 收益 = verdict 报告降噪。
3. **PROPOSAL 模板补引文行尾提示**：治 F2 · 载体 = ARQ 收件箱（模板「现文（verbatim）」处注一句「引文以 canonical 现行行尾为准；装配清单按目标文件行尾书写」）· 收益 = 省每批一次行尾踩坑。
4. **装配清单从 PROPOSAL 自动抽取**：治 F3 · 载体 = 登记不硬治（PROPOSAL 围栏嵌套结构机器抽取易碎，等 cite-check/replaced 语义成熟后再评）· 收益 = 省主线程手写 gen 脚本。

## 5. 一句总结

成本约 40min / 39 万 tokens（±20%）/ human 触点 0 次（本批立项与语义均在列车级触点①②消化）/ 制品 5 件（PROPOSAL + summary + verdict + retro + §7 评审记录）· 最险一刻 = cite-check ok:false 的误判风险，被「advisory 自报 + verifier 独立定性」双保险接住 · **保险丝判定：两方独立结构接住作者偏差**——判官（R1 独立复扫四查，未采信转述）与核验官（diff 逐 hunk 程序化对账 + cite-check 定性独立取证）均以零共享上下文实跑，主线程誊写与落地零偏差经双方独立证实 · 最值得带走的一条 = **「ok:false ≠ 门 FAIL」——advisory 工具的失败形态必须先定性再处置**（→ 建议 1）。

## 6. 本环遗留

- 16px / 20px 新字体精确尺寸：P0 spike 实产回填（ADR-017 与 T2 注记均已挂指针）；若实测推翻容量结论回 /arch-change。
- P0-P3 实现分派：portfolio-planner 按 frozen workstream 拆包；launch 前置 = anchors.arch（本批 §12 行已落，待主线程回填 spine）+ P0 完成。
- 九档实验页与测试字体：不退役（spine q2）；不为腾 ota_1 空间删除。
- ADVISORY 2 条（cite-check replaced 语义 / 结构门 base 噪声）：去向 = §4 建议 1/2 的 ARQ。
