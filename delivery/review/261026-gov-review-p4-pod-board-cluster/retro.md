# retro · 261026-gov-review-p4-pod-board-cluster（Track B 全环：Pod/board 面簇五 ARQ 并批清偿）

> 主线程第一人称热记忆 · 2026-10-27 · workstream gov-debt-reduction P4 波（本 workstream 第 4 乘客）· harness 形态：architect / architect-reviewer / landing-verifier = 本会话后台子 agent（fable 模型），主线程 Opus 4.8 纯编排 + packet-bound 落地 · 跨乘客对照：与 P1 / P1.5 / P2 同日连跑，本乘客首次遇到中途配额熔断与判官记录入档损伤两类新摩擦。

**【硬项 R1】** verdict 绿后即写，第一人称，无隔场补记（核验 PASS 后立即撰写）。

**【硬项 R2】** token 计量来源 = 各子 agent 的任务通知 usage 字段（subagent_tokens / tool_uses / duration_ms 实测值）；主线程自身 token 无计量，按轮次规模估 ±30%。时长同为实测（duration_ms），主线程落地段为人工编辑 + 验证脚本多轮实跑，估 40 分钟 ±10。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 复证 + 立项 | 五条 ARQ 复证 + task_plan 落 ack + OQ 预置 | 10-26 晚 | 10-26 晚 | ~20 min | 主线程 |
| 起草 v0.1 | architect 任务书派发 → PROPOSAL 546 行落盘 | 10-26 晚 | 10-27 凌晨 | 实跑 ~92 min（含 429 熔断等待 ~65 min） | 1,035,104（三段累计，含一次 429 中断重续） |
| OQ 裁定 | human OQ1=(a) / OQ2=(b) 前置拍板 | 10-27 | 10-27 | <5 min | 主线程 |
| 评审 R1 | verdict BLOCK(F1,F3)，2 SHOULD + 3 NIT，判档复核 | 10-27 | 10-27 | 749 s | 138,811 |
| 改稿 v0.2 | R1 五条收口 + OQ2=(b) 扩改（manifest 11→12，riskTier→medium） | 10-27 | 10-27 | 348 s | （并入起草累计） |
| 评审 R2 | verdict BLOCK(F1)（纯记录保真）+ 1 NIT | 10-27 | 10-27 | 537 s | 201,187 |
| 改稿 v0.3 | F1 复原 + F2 枚举补全 | 10-27 | 10-27 | 160 s | （并入起草累计） |
| 评审 R3 | verdict GREEN（零 findings）+ R2 记录自复原 | 10-27 | 10-27 | 278 s | 224,149 |
| FROZEN + landing | 状态翻转 + 12 文件 packet-bound 落地 + V1–V8 | 10-27 | 10-27 | ~40 min ±10 | 主线程 |
| 核验 | landing-verifier verdict PASS + 2 ADVISORY | 10-27 | 10-27 | 400 s | 89,575 |
| **合计** | | | | 主观历时约 5 小时（其中 ~1 小时为配额等待） | 子 agent 实测 ~1.69M |

对照：P2 批（同日）全环 R1 单轮 GREEN；本批评审三轮、为四乘客中最多——增量全部来自「判官记录入档损伤」这一类纯记录保真问题（R2 唯一 BLOCKING 即此），改法与验证集一次通过。

## 2. Token 账

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（复证 + 立项 + 落地 + 收尾） | 无计量（估 300k–450k） | ~20% ± | 落地 12 文件逐处 Edit + 验证脚本三轮调试 |
| architect | 1,035,104 | ~61% | 含 429 中断前段与三次续跑 |
| 判官（R1–R3） | 564,147 | ~33% | R1 全量 139k；R2/R3 聚焦轮 201k/224k（R2 含 OQ2 扩改面亲核、R3 含 R2 记录复原） |
| 核验官 | 89,575 | ~5% | 最省一轮 |

花费最大处 = architect 起草（五条并批 + 装配实跑双份验证）。可压缩分析：装配版实跑（B10–B16）占了起草大头，但它正是 R1/R2 零改法级 findings 的直接原因——物有所值，不建议压。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · §7 判官记录入档转写损伤（两次，本批独有痛点）**：现象 = R1 留痕被拼接复原丢 `$'` 两字符 + 引入换行；R2 记录 F1 段吞字两处 + 尾段重复写入五份，且 architect 报告「重复在另一会话写入后再次出现」 → 根因（推断）= agent 续跑/重放侧的文件写入副作用叠加 `$'` 转义吞字，非手工编辑；两次都由「作者侧结构修复 + 判官持原文机械 diff 复原 + R3 grep census（各节恰 1）」收口 → 证据锚 = PROPOSAL §7 R2-F1 全文、修订记录 #8–#11、R3 记录「并发写入核查」段。
- **F2 · 配额熔断打断起草**：现象 = architect 起草中途 429（5 小时窗口耗尽），PROPOSAL 未落盘 → 根因 = 外部配额，非流程 → 处置 = 同实例续跑（SendMessage 原上下文接续），零进度损失 → 证据锚 = task-notification 429 报文 + 续跑后交付完整。教训 = **中断续跑优于重派**，任务书上下文在实例里不在盘上。
- **F3 · MSYS grep 无 shell spawn 的单引号参数改写怪癖**：现象 = Node `execFileSync('grep', ['-cF', "'settings-presence',", …])` 恒返 0，bash 直测同模式 = 1 → 根因 = MSYS 运行时对含 `'` 的 argv 改写（`presence',` 同样中招）→ 证据锚 = landing-summary §3 注记 1；处置 = 该条改 bash 直测判定。
- **F4 · 主线程验证夹具自伤两次**：V4 首版读浅一层（`reconcile()` 返回 `{ok, board}` 我读成顶层）+ token 持久层认错文件（实际 = `board.json` 的 `mergeToken`）+ 多 Pod 污染（释放后 token 被规则 2 立即轮转给下一个 ready Pod，误判泄漏） → 根因 = 凭 B14 输出反推夹具、未先读源码 → 证据锚 = `portfolio-reconcile.mjs:178`（prevBoard.mergeToken）/ `:203`（规则 2 轮转）。
- **near-miss（最险一刻）**：landing 时我在 loop-retro.md 接缝处第一版顺手把「仍无动作。、token」润色成通顺连接——若未对照冻结处方逐字核，就是一次未经留痕的 FROZEN 文本偏离，landing-verifier 大概率打回甚至漏网。教训 = **接缝处的「顺手改通顺」是 packet-bound 落地最隐蔽的偏差源**；改法 = 接缝逐字节按处方，别扭就写注记（本批 landing-summary §3 注记 2 即此形态）。

## 4. 优化建议（按 ROI 排序）

1. **判官记录入档自检句**：治 F1 · 载体 = ARQ 收件箱（gov 域）· 建议 = track-b-judge-brief.md 增一句「append §7 记录后立即 `grep -c '^### R<n>'` / 各固定节计数自检，≠1 即重写」——把 R3 的人工 census 变成判官义务；预期收益 = 消灭「下一轮评审被迫审记录保真」这类纯摩擦轮。
2. **MSYS 单引号怪癖记入技能/注记**：治 F3 · 载体 = 登记不硬治（本 retro + landing-summary 已留痕；如复发再立 tech 域 ARQ）· 预期收益 = 下次写验证脚本直接走 Node 内读文件或 bash 直测，省一轮调试。
3. **中断续跑先例**：治 F2 · 载体 = 登记不硬治（SendMessage 同实例续跑已实证可行，无需机制）· 预期收益 = 429 类中断不再触发重派浪费。

## 5. 一句总结

成本 = 子 agent 实测 ~1.69M tokens / 主观 ~5 小时（含 1 小时配额等待）/ human 触点 3 次（立项四拍、OQ 两裁、重试一次）/ 制品 = PROPOSAL(706 行) + summary + verdict + retro 四件 + 5 条 ARQ 结项 · 最险一刻 = 接缝句顺手润色 near-miss（未遂）· **保险丝判定：两方独立结构接住了作者偏差**——判官 R1 抓到起草方 grep 引据不实、R2 抓到作者侧「零改动」声明不实（记录级），核验官独立抓到 CRLF 观察层差异并如实降级为 ADVISORY；三层无一漏放。最值得带走的一条 = **「评审环 burns 轮次于记录保真」是新故障类——写侧自检一句就能消**。

## 6. 本环遗留（留根去向）

- **260918 候选③门策略 + 扩展形态实测**：OQ1=(a) 观察期，复议触发 = PROPOSAL §6 留根 1/2（`settings-presence` 降级率统计 / 首次实测）；条目结项时未闭合面写明。
- **shared-branch 方案②（merge-driver `<base>..<head>` 扩参）+ worktree 池化**：human 已裁另登记独立专项批（workstream task_plan 立项 ack③ 在案；PROPOSAL 留根 7 互指）；池化批落地时回收启动卡 base ref 形态。
- **`scanAllChanges` 的 current 假阳性**（留根 4）、**261021 spine 本体修补**（留根 5，主线程 intake 事务）、**settings-presence 不进聚合器**（留根 6）：复议触发各在 PROPOSAL §6。
