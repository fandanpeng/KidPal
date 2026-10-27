# retro · 261027-gov-review-p7-devloop-doc-cluster（Track B 全环：起草一轮 GREEN → 判档升 Track B → 落地 → PASS+1 ADVISORY）

> 主线程第一人称热记忆 · 2026-10-27 · gov-debt-reduction workstream P7 波（四条）· harness 形态：architect / 判官 / 核验官均为零上下文子 agent，主线程纯编排 · 跨批对照：P6.5 同日批，成本口径可直接对照。

**【硬项 R1 · 环后趁热、第一人称】** verdict 落盘后随即撰写，全部相位在本会话内亲历，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent tokens 为 harness usage 报告（精确；verifier 报告在 retro 初稿后到达，已回填实值 54,279）。主线程 tokens 无计量，按经验估计并给误差界。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 逐条带证据锚。

**【硬项 R4 · 建议必标载体】** §4 逐条标载体：S1 已条目化（ARQ-261027-landing-summary-inflight-snapshot）；S2 登记不硬治。

**【硬项 R5 · 保险丝判定】** §5 明确判定：接住（判官升判档 + verifier 抓时序差各一次，作者偏差两处均被独立方接住）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 复证 + 立项 | 六条复证 + human 两拍（AskUserQuestion） | 本会话 | 本会话 | ~10 min | 主线程 |
| 起草 | DRAFT v0.1（341 行）+ 装配自证 | 本会话 | 本会话 | ~10 min（agent 计时 600s） | 243,411（usage 报告） |
| 评审 R1 | 四查 GREEN + 判档升 Track B | 本会话 | 本会话 | ~4.6 min（274s） | 90,899（usage 报告） |
| FROZEN | 立项 ack + R1 GREEN | — | — | 主线程单次 Edit | 主线程 |
| landing | 复跑装配 → 仓内 apply → V1–V5 | 本会话 | 本会话 | ~10 min | 主线程（若干 Bash/Write） |
| 核验 | landing-verifier PASS + 1 ADVISORY | 本会话 | 本会话 | 158s | 54,279（usage 报告） |
| **合计** | 立项到 verdict | — | — | ~30–35 min | 子 agent ~389k（主线程另计） |

对照：P6.5 同日批全环较 P6 降 ~40%；本批为纯文面批 + 装配器自消费，一轮 GREEN 零改稿， drafting 即自证到位，是装配器落地后第一个「起草即对账通过」的批。

## 2. Token 账

计量方式诚实声明：子 agent 三方中 architect 与判官有 harness usage 精确值；verifier 值缺失，按同类核验批量级估 ~60–90k（±30%）；主线程无计量，估 ~50–80k（±40%）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（复证 + 编排 + 落地 + 记账） | ~50–80k（估） | ~15% | 复证 grep 便宜；落地 Bash 逐条 |
| architect | 243,411 | ~55% | 含装配自证与 §2 逐字节承诺的实跑 |
| 判官（R1） | 90,899 | ~20% | 一轮 GREEN |
| 核验官 | 54,279 | ~14% | usage 报告 |

花费最大处：architect——逐字节承诺 + 装配自证 + 验证集实跑是本流程的固有重量，但换来判官一轮 GREEN 与落地零返工，总账仍省。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 判档灰区（轻路径自评 vs 判据②）**：现象 = 起草自评轻路径、判官判 Track B → 根因 = 轻路径判据②「措辞/格式/链接/勘误」枚举未涵盖「新增文档义务」这一常见批型，判官只能逐批个案裁量 → 证据锚 = gov-change.md:19 与 PROPOSAL §7 R1 判档段。本批零返工全靠 architect 在 §1 预注册向严声明 + 取舍 E 退路（改法集不变）——退路机制奏效，但口径灰区本身仍在，每批都要付一次裁量。
- **F2 · summary 在途件快照时序差**：现象 = landing-summary V4 记「在途件三条」、verifier 实跑时又多出 docs/debt/gov 八件（主线程结项动作在 summary 出具后执行）→ 根因 = summary 的在途件登记是写盘时点快照、无「以落地相位实跑为准」的口径句 → 证据锚 = landing-verdict.md ADVISORY A1。
- **near-miss（最险一刻）**：acceptance-evidence.md 工作树为 CRLF（`i/lf w/crlf`），T4 插入段若按 LF 写会造混行尾、且 `--cite-check` 归一口径下可能静默放过——起草期 B5 行尾断言（73 行 / CRLF 73 / 裸 LF 0）与装配器「不归一行尾」语义（清单按 CRLF 写该段）把它接住了。教训：CRLF 文件上的逐字编辑，行尾口径必须进清单而不是靠运气。

## 4. 优化建议（按 ROI 排序）

1. **landing-summary 在途件登记加「实跑时点口径」句**：治 F2 · 载体 = **已条目化 → `docs/debt/gov/ARQ-261027-landing-summary-inflight-snapshot.md`**（SHOULD，随下批消费）· 预期收益 = 消除每批 summary 与 verifier 实跑的时序差误读。
2. **轻路径判据②口径灰区**：治 F1 · 载体 = **登记不硬治**——本批证明「预注册向严声明 + 预置退路」已把灰区成本压到零返工；改判据文面属判据本身变更（恒 Track B），收益不抵频繁动 SSOT 的成本。复发观察：若连续多批都靠退路转 Track B，再议把「新增文档义务文面」显式写进判据②的反例枚举。

## 5. 一句总结

成本 ~400k 子 agent tokens / ~35–40 min / human 触点 1 次（立项两拍）· 制品 4 件齐（PROPOSAL + summary + verdict + retro）· 对照 P6.5 同量级，起草一轮 GREEN 零改稿、落地零返工，是装配器自消费后最顺的一批 · 最险一刻 = CRLF 文件上的逐字插入（B5 断言接住）· **保险丝判定：接住**——作者偏差①（轻路径自评）被判官独立升 Track B，偏差②（summary 快照时序差）被 verifier 独立抓成 A1，两方独立结构各接一次 · 最值得带走的一条：退路预注册（取舍 E 式）让判档升级零返工，值得每批沿用的起草惯例。

## 6. 本环遗留（条件节）

- 留根 1–6 见 FROZEN PROPOSAL §6（ARQ 结项已由主线程执行完毕；T1 无机械闸、T2 模式模板、T4 机械化边界、agents 面未触、生成件口径——复议触发各自在案）。
- P7.5 小批（closeout 写点映射 + opendesign 出图先例）待本批合并后开。
- V6 --upsert 与 V7 升级闸由主线程在 verdict 后执行（本 retro 撰写时点未到、不预写结果）。
