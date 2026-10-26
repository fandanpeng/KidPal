# retro · 261026-gov-review-retro-efficiency-fidelity（Track B 全环：效率表读停 + 生命周期 + fixer 根因）

> 主线程第一人称热记忆 · 2026-10-26 · 治理面乘客 · harness 形态：判官 / 核验官 = 本仓 typed agent（architect-reviewer / landing-verifier），架构师 = typed architect；主线程只编排与落地。

## 1. 时间账

硬数据锚点（子 agent 用 harness 回报的 `duration_ms`；主线程时段未机器计量，按文件 mtime 近似）：

| 相位 | 内容 | 开始（UTC） | 结束（UTC） | 时长 | tokens |
|---|---|---|---|---|---|
| 分析 | 25 包效率表时间线复核（本环前置，非本批） | 02:58 前 | 02:58 | — | 未计量 |
| 起草 v0.1 | architect 首稿 | ≈04:28 | 04:38 | 10.2 分钟 | 256,639 |
| 评审 R1 | architect-reviewer `BLOCK(F1..F7)` | 04:39 | 05:09 | 29.7 分钟 | 134,978 |
| 起草 v0.2 | 按五项 OQ 裁定修订 | 05:09 | 05:13 | 3.9 分钟 | 305,061 |
| 起草 v0.3 | R1 F1–F7 修订（含 429 中断后恢复） | 05:13 | 05:16 | 3.1 分钟 | 349,152 |
| 评审 R2 | architect-reviewer `GREEN` + N4–N6 | 05:16 | 05:20 | 4.1 分钟 | 178,524 |
| 起草 v0.4 | N4–N6 落件 | 05:20 | 05:21 | 1.2 分钟 | 359,422 |
| FROZEN | 主线程标 FROZEN v0.4 | 05:21 | 05:21 | 瞬时 | — |
| landing | 九个目标文件 + landing summary | 05:21 | 05:02 | ≈23 分钟（含验证集实跑） | 主线程未计量 |
| 核验 R1 | landing-verifier `FAIL(F1)` | 05:02 | 05:17 | 14.6 分钟 | 123,719 |
| 核验 R2 | landing-verifier `PASS + 4 ADVISORY` | 05:17 | 05:20 | 2.9 分钟 | 136,508 |
| 收口 | changes-index + retro + 升级闸 + 合并 | 05:20 | 进行中 | — | — |

对照：本环比 261024 批（只读脚本 + 命令文面，riskTier medium）多触合并链与三个 agent 定义；子 agent 合计约 **1.84M tokens / 69.8 分钟**，其中架构师四轮占 68.9%。主线程时段未机器计量，上表按 mtime 近似，误差约 ±5 分钟；不装精确。

## 2. Token 账

计量方式诚实声明：子 agent 用 harness 回报的 `subagent_tokens`（每轮单值，四轮架构师按轮求和；这些数含跨 resume 的重复上下文，**不是计费真值**）。主线程无 exact 计量，未换算成 token，误差界不明；下表占比只对子 agent 合计。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（分析 + 落地 + 验证集 + 收口） | 未计量 | — | 会话 UI 未回报；用上下文体量与工具调用判断，未换算 |
| 架构师（v0.1 + v0.2 + v0.3 + v0.4） | 1,270,274 | 68.9% | 四轮；v0.3 含 429 中断后恢复 |
| 判官（R1 + R2） | 313,502 | 17.0% | R1 29.7 分钟、R2 4.1 分钟 |
| 核验官（R1 + R2） | 260,227 | 14.1% | R1 14.6 分钟、R2 2.9 分钟 |
| **子 agent 合计** | **1,844,003** | 100% | 主线程不计入 |

花费最大处：架构师四轮（1.27M，68.9%）——首稿 256K，后续三轮因评审否回、OQ 裁定、R1 findings 与 429 恢复各重读上下文。可压缩分析：v0.2 与 v0.3 可以合并为一次修订（OQ 裁定与 R1 findings 同一批给），预计省一轮约 300K；v0.4 的 N4–N6 本可并入 v0.3（判官已说不阻冻结），省 359K。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 判官 R1 抓出「共享 mergeSha 识别批量合并」在真实数据上不可达**：现象是 OQ4=(b) 的 `merge-completed` + `mergeSha` 路径结构上产不出跨包共享 SHA；真实批量合并 `017de34c` 落的是 `eventType: "merged"` + `payload.mergedCommit`，且不在 `pod-events-append.mjs` 闭集内。根因是起草时把「事件优先」的愿望当成了生产事实，没有先亲核语料。证据锚：`delivery/review/261026-gov-review-retro-efficiency-fidelity/PROPOSAL.md` §7 R1-F1；`261022-diag-log-content/pod-events.jsonl` 的 `merged` 行。处置：v0.3 改为「subject 分类器为主判据、共享 SHA 降为 guard、`merged`/`mergedCommit` 只读消费不追认」。

- **F2 · 核验官 R1 抓出 `manifestGlobToRegExp` 的 `**` 展开次序 bug**：现象是 `dir/**` 只匹配一层，`firmware/main/test/host_app/main/CMakeLists.txt` 本应命中两包 manifest 的歧义桶，实际掉进无主桶；`batchUnattributedPaths` 实际 12 无主 + 5 歧义，而不是 summary 写的 11+6。根因是替换次序：先展开 `**` 成 `.*`，最后又把 `*` 替换成 `[^/]*`，把 `.*` 改写成 `.[^/]*`；更深的根因是自检夹具只放了一层路径，对 `**` 语义是真空绿。证据锚：`retro-efficiency.mjs` 的 `manifestGlobToRegExp`（修前）；`landing-verdict.md` R1-F1。处置：占位符次序、夹具补三层路径、删死代码 `findMergeShas`；复跑 11+6，三包行数与 token 总量不变。

- **F3 · 429 配额中断打断 v0.3 修订，且失败发生在写盘前**：现象是架构师收到 429 后终止，PROPOSAL 仍是 v0.2；恢复时需重新确认文件状态。根因是派发契约只覆盖正常返回，429 是环境常态。证据锚：task-notification 429（request id `021792987823333a7468c13ae862cf181a1dbcac0db1f82344d7c`）；恢复后 `grep 状态：` 仍为 v0.2。处置：恢复同 agent、重发 findings；本条与在册 `ARQ-261025-interrupted-dispatch-progress-note` 同根，不新立。

- **F4 · 读侧静态导入写侧脚本触发 lib 自检退出**：现象是 `retro-efficiency.mjs --self-check` 只输出 `changeid-prefix` / `transcript-telemetry` 两行 JSON 就 exit 0，23 条自检一条没跑。根因是 `dispatch-stamp.mjs` 依赖的 `lib/changeid-shared.mjs` / `lib/transcript-telemetry.mjs` 在 `process.argv` 含 `--self-check` 时自行输出并 `process.exit`；静态导入把这段副作用带进来了。证据锚：首次自检输出；`lib/transcript-telemetry.mjs:376` 的 `process.exit`。处置：把 `FIXER_ROOT_CAUSES` / `classifyFixerRootCause` 移到 `pod-events-append.mjs`（两脚本共同依赖、无自检副作用），读侧与写侧共用同一实现。

- **F5 · landing summary 把「起草期手算」写成「实测」**：现象是 R1 核验官发现 summary §1/§2 的 11+6 与当时实际 12+5 不符；根因是汇总时把 FROZEN 的期望值直接转写成实测输出，没等脚本复现。证据锚：`architect-landing-summary.md` 修前 §1/§2；`landing-verdict.md` R1-F1 的「伴生失实」。处置：修后 11+6 成为实测值，summary §3 第 8 条记录该轮；今后汇总必须标「起草期手算 / 落地实测」。

- **F6 · FROZEN 文本内部张力：§5-6 写「全部 25 行 unknown」，T3 值域含 `wall-clock-legacy`**：现象是核验官 R1 记 ADVISORY A2，落地按 T3 实现（22 行 wall-clock-legacy + 3 行 unknown）。根因是 §5 的断言句沿用了 v0.2 的旧值域，T3 改法后来扩了值域，评审透镜没有把「断言 vs 改法值域」对撞。证据锚：`PROPOSAL.md` §5 第 6 条 vs §2 T3；`landing-summary.md` §3 第 9 条。处置：summary 点名张力、按 T3 落地；不追改 FROZEN。

- **F7 · 旧 `findMergeShas` 成为死代码**：现象是 T1 改法把判据抽成 `classifyMergeSubject` 后，旧函数无调用点但留在文件里；核验官 R1 记 A3。根因是落地时只加新路、未清旧路。证据锚：`retro-efficiency.mjs:511`（修前）。处置：删除；全仓零悬空引用，`node --check` 通过。

- **F8 · 档位计数与起草期估计差一档**：现象是起草期估「full 10 / lite 14 / nano 1」，落地实测「full 9 / lite 15 / nano 1」。根因是起草期用旧快照数，落地按当前 `taskpackage.json` 现读。证据锚：`coverage.tierCounts`；summary §3 第 3 条。处置：按现跑为准，不追改 FROZEN 点名抽查值（三个抽查值全对）。

- **near-miss（最险一刻）**：F2 的 glob bug 差一点以「绿」通过。`batchUnattributedPaths` 的总数仍是 17（12+5 与 11+6 的和相同），三包行数和 token 总量也完全不变；如果核验官只看总数、或只跑自检夹具，就会放过一个「诊断桶分错」的真 bug。它没出事的原因是核验官真的读了正则、并用多层路径独立重放旧实现，发现旧夹具必红、新夹具才绿。教训：**总数不变不等于语义正确；glob / parser 类夹具必须放多层输入，并断言 reason 桶而不是只断言总数**。

## 4. 优化建议（按 ROI 排序）

1. **glob / parser 夹具强制多层输入 + reason 桶断言**：治 F2、F5 · 载体 = 本批已落地的 `batch-split-ambiguous-dropped`（三层路径）+ 建议进 PROPOSAL 模板的「夹具必须能判别」检查项（ARQ 收件箱，下一治理批） · 预期收益：把「总数巧合相等」的真空绿挡在自检层，而不是靠核验官人读补救。

2. **派发契约补「中断进度注记」**：治 F3 · 载体 = 已登记 `ARQ-261025-interrupted-dispatch-progress-note`（本批 429 实证追加证据） · 预期收益：429 / 配额中断后不必靠 `git status` 考古，恢复成本从「重新确认文件状态」降到「读一行进度」。

3. **纯函数/枚举放无自检副作用的共同依赖，或给 lib 的 `--self-check` 加 `isMain` 守卫**：治 F4 · 载体 = ARQ 收件箱（gov，下一动 lib 自检的批） · 预期收益：读侧导入写侧工具时不再被 lib 的终端 `process.exit` 截断；自检「跑了」与「真跑了」可区分。

4. **PROPOSAL §5 断言与 §2 改法值域做一次内部对撞**：治 F6 · 载体 = 判官评审透镜 6（验证集可执行性）的检查项 / ARQ 收件箱 · 预期收益：FROZEN 文本内部的旧值域残留（如 unknown vs wall-clock-legacy）在评审期暴露，而不是核验期记 ADVISORY。

5. **landing summary 强制区分「起草期手算」与「落地实测」**：治 F5 · 载体 = `architect-landing-summary.template.md` / ARQ 收件箱 · 预期收益：汇总不再把期望值写成实测，核验官不用先证明 summary 撒谎再修。

6. **为两类真实污染加临时 git 仓回归夹具**：治 F1 / F2 的同族风险 · 载体 = 下一动 `retro-efficiency.mjs` 的批（ARQ 收件箱） · 预期收益：现在自检只用合成 subject，真实 sync merge / batch merge 的形态仍靠语料现跑；临时仓夹具能把「只认命名合并」「批量按 manifest 切分」固化为可反转的红绿。

## 5. 一句总结

成本：子 agent 约 **1.84M tokens / 69.8 分钟**（架构师 68.9%、判官 17.0%、核验官 14.1%），主线程未计量；触点：human **1 次立项 ack + 1 批五项 OQ 裁定**，无例外升级、无人类合并前裁决；制品：PROPOSAL + landing-summary + landing-verdict + retro 四件。对照 261024 批：本批多触合并链与三个 agent 定义，架构师四轮是主要成本，v0.2/v0.3 可合并。最险一刻：glob `**` 次序 bug 在总数不变的情况下差点以绿通过，核验官用多层路径重放旧实现抓住。**保险丝判定：两方独立结构（判官独立 + 核验官独立）接住了作者偏差**——判官 R1 抓出共享 SHA 判据不可达，核验官 R1 抓出 glob 次序与 summary 失实；但自检自身对 `**` 是真空绿，说明独立人读不可省。最值得带走的一条：**总数不变不等于语义正确；reason 桶和多层输入才是 glob 类改法的判据**。

## 6. 本环遗留（条件节）

- **§5 第 16 条合并后观察**：下次真实 Pod 合并后核 `merge-completed.payload.mergeSha` 为 40 位、历史 1 条不被改写、`attributionSource` 两态；不在本批 landing 前置。
- **ARQ-261026 结项**：本批 GREEN + 落地后，T2 直接消费完成；结项登记动作归主线程（本 retro 写后、合并前或合并后补）。
- **T1 行数污染新根**：本批以改法收口，但 origin 的新 ARQ 尚未登记（PROPOSAL §6 第 1 条留根，须 human 批）；建议下一治理批登记或并入 ARQ-261014 追加证据。
- **`merged-legacy` 通道**：只读消费、不追认；未来若再出现手工多包合并，`merged`/`mergedCommit` 仍可能被写入，需在下次 retro 观察是否应转正式通道或改由 merge-driver 支持批量。
- **FIXER_ROOT_CAUSES 落位偏差**：核验官判可接受（单一来源 + 九文件 manifest），但 FROZEN 文本写的是「脚本内」；若下一批动 `dispatch-stamp.mjs`，可顺手把这段注释与 PROPOSAL 的措辞对齐。
