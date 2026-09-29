# retro · 260928-gov-review-requirement-loop（Track B 全环：立项 ack → 三轮评审 → FROZEN → 七文件落地 → verifier PASS → 合并前收口）

> 主线程第一人称热记忆 · 2026-09-29 · 乘客 #11（gov lane 近批形态）· harness 形态：判官与核验官均为同 harness 子 agent（architect 判官中途遭 cc-switch 月配额 429 一次，SendMessage 续跑恢复、上下文无损）。

**【硬项 R1 · 环后趁热、第一人称】** verdict 绿后即写（landing-verifier PASS 当轮），未隔场。

**【硬项 R2 · 计量诚实】** 子 agent 侧有真实计量（harness usage 字段）：判官三轮合计 57419+79199+96764 ≈ 233k tokens、核验官 41k；主线程侧无逐相位计量，按会话总量估计并声明误差界 ±30%（见 §2 表）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 调研 + 起草 | 三问题调研（需求/架构面流程现状 + 红基线全量考）+ PROPOSAL v0.1 | 2026-09-28 晚 | 同晚 | ~1h | 主线程（估） |
| OQ 裁定 | human AskUserQuestion 四题全按推荐位裁 | 2026-09-28 | 同日 | 分钟级 | — |
| 评审 R1 | architect BLOCK(F1,F2)+F3–F7 MINOR | 2026-09-28 深夜 | — | ~5.4min | 57,419 |
| （中断） | cc-switch 月配额 429，SendMessage 续跑 | — | — | — | — |
| 评审 R2 | BLOCK(F8)——F1–F7 修订到位性复核 + 新引入错述抓出 | 2026-09-29 | — | ~3.7min | 79,199 |
| 评审 R3 | GREEN 收口 + manifest/packet-binding 复核 | 2026-09-29 | — | ~1.8min | 96,764 |
| FROZEN | 主线程凭据自冻 | 2026-09-29 | — | 分钟级 | — |
| landing | T1–T7 七文件 + V1–V6 + changes-index | 2026-09-29 | — | ~30min | 主线程（估） |
| 核验 | landing-verifier PASS（13 工具调用） | 2026-09-29 | — | ~3.2min | 41,334 |
| **合计** | 立项→合并前 | 2026-09-28 | 2026-09-29 | ~2 小时级 | 见 §2 |

对照：260928-gov-review-requirement-face 批（同面上一批）同量级；本批三轮 BLOCK 未触熔断（线 = 3 轮），human 触点 2 次（立项 ack + OQ 四题一次 AskUserQuestion）。

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（调研 + 起草 + 落地 + 记账） | ~60k–110k（估，±30%） | ~30–40% | 无逐相位计量，按会话 context 量级估 |
| 判官（R1–R3 合计） | 233,382 | ~55% | usage 实数：57,419 / 79,199 / 96,764 |
| 核验官 | 41,334 | ~10% | usage 实数 |

花费最大处 = 判官三轮（每轮独立重读 PROPOSAL + 抽验引据 + 实跑验证命令）。可压缩分析：R2/R3 若共享更紧凑的「上轮 verdict + 修订对照清单」输入可省部分重读，但独立性换成本是设计取舍，不建议压缩。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 判官被外部配额打断**：现象 = R1 评审进行中被 cc-switch 月配额 429 中断（57k tokens 投入后失联）→ 根因 = 模型档位配额管理在 harness 外（cc-switch 代理层），harness 无法感知/等待 → 证据锚 = 会话实录（agentId a3c390e141f5f1f19 中断通知 + SendMessage 续跑恢复）。教训 = 中断后 SendMessage 续跑比重派省（18 次工具调用的读盘上下文无损保留）。
- **F2 · V3 fixture 断言被 FROZEN 改法证伪**：现象 = 落地后 route-regression-check 红「story-author description dropped required routing signal 无 Write」→ 根因 = route-fixtures 断言编码的是上一批的旧路由信号，PROPOSAL 留根只写了「补新命令断言」、未预见旧断言被本批移除 → 证据锚 = routing.json:15（改前）+ V3 首跑输出。处置 = 最小修正 + landing-summary §1 申报偏差 + landing-verifier 核过接受。教训 = **改 description 信号集的批，验证集应预跑 route-regression-check 拿基线并在 §5 写明「哪些既有断言会被本批证伪」**——起草期跑一次 fixtures 对照即可预知。
- **F3 · landing-summary 引「改前 verbatim」触发 fidelity advisory**：现象 = 结构门报 verbatimQuoteAdvisory missing（PROPOSAL 引 scope-map 改前块）→ 根因 = 引据即改法对象时，落地后引文必然失配 → 证据锚 = fidelity 输出 verbatimQuoteAdvisory 段。非红，判读经验：advisory 读语义（改前态引文失配 = 预期），不需预清。
- **near-miss（最险一刻）**：起草 F1 修订时主线程写了「Bash 重定向写仍由 universal.bashDenyWrite 恒拦、无绕行口」——**凭印象断言、未实读 scope-map:25**，判官 R2 抓出（清单仅三项，两 allow 路径不在覆盖内）。为什么没出事 = 判官独立抽验 + gate-truthfulness 批立下的「如实表述」纪律。教训 = 修订期的承重断言与起草期同纪律——**每句机械行为断言落笔前实读目标文件**，修订疲劳期最容易虚报。

## 3b. 判官价值实录（本批评审真实抓到的）

R1 F1 = story-author frontmatter tools 行无写工具（正文授权全落空）——主线程起草期完全漏掉；R2 F8 = 主线程修订期引入的假保证——两处均为「已 ack 方向内的实质缺陷」，对抗评审的价值实锤。

## 4. 优化建议（按 ROI 控序）

1. **改 description 信号集的批：起草期预跑 fixtures 对照**：治 F2 · 载体 = ARQ 收件箱（gov 域登记）· 预期收益 = 消灭「landing 后 V3 红 + fixture 紧急修正」这一类落地偏差。
2. **中断续跑模式成文**：F1 的 SendMessage 续跑（上下文无损）优于重派——载体 = 登记不硬治（harness 行为，无脚本可固化），写进本 retro 供后续乘客参照。
3. **修订期断言纪律与起草期同规**：治 near-miss · 载体 = prompt-authoring rule 或 judge brief 增一句「修订引入的承重断言须附实读证据」· 留批次（下次碰 prompt-authoring rule 的批顺带）。
4. **fidelity advisory 判读注记**：F3 经验（改前态引文失配 = 预期 advisory）· 载体 = 登记不硬治，本 retro 承载。

## 5. 一句总结

成本 ≈ 334k tokens（子 agent 实数 275k + 主线程估 60–110k）/ ~2 小时级 / human 触点 2 次 / 制品 4 件齐（PROPOSAL + landing-summary + landing-verdict + retro）。最险一刻 = 修订期「无绕行口」假保证被判官 R2 抓出。**保险丝判定：两方独立结构接住作者偏差**——判官三轮独立抓出 tools 行缺漏（F1）与修订期错述（F8），landing-verifier 独立复核七文件 diff + 申报偏差核过接受，两道保险丝均实际生效。最值得带走的一条：**起草期预跑 fixtures 对照**（建议 1，登记 ARQ）。

## 6. 本环遗留

均为 FROZEN §6 既有留根（非缩水）：消费保护机检齿（实包 2–3 单后）、route-fixtures 补新命令断言（下批顺带）、架构档挂入（需求面 loop 实跑后）、architect.md 入口清单（self-def 边界）、launchPreconditions 接线与 spine 联动（ARQ-260928 批 / 首个 spine 实证）。
