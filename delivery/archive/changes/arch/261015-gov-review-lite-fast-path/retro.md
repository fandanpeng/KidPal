# retro · 261015-gov-review-lite-fast-path（Track B 全环：立项 ack 闭环 → 单轮 GREEN → packet-bound 落地 → 核验中）

> 主线程第一人称热记忆 · 2026-10-15 · 乘客 #?（同日第 4 个 gov 批）· harness 形态 = 全常规：architect / architect-reviewer / landing-verifier 均零上下文子 agent，主线程纯编排 + packet-bound 落地直改。

**R1（环后趁热）**：landing-verifier 已绿（VERDICT: PASS——结构门 ok、逐 T 项 PASS、破坏性自检零越界、验证集独立复跑全绿），本 retro 即为终稿。
**R2（计量诚实）**：子 agent tokens 取任务通知逐段上报值（architect 三段、reviewer 一段）；主线程无直接计量，按会话体量估 ±30%。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 前置调查 | 主线程核五浪费源机制地面 + 思路对齐 + human 立项 ack（三轮对话） | 会话前段 | 会话前段 | ~30min（估） | 主线程 |
| 起草 v0.1 | architect 全新起草 + preflight | — | — | 21.4min | 177,904 |
| 改稿 v0.2 | OQ 三条裁定折进（OQ-3 裁 (c) 消解） | — | — | 5.5min | ~14k（累计 191,554 差值） |
| 改稿 v0.3 | R1 三 should + advisory 折进 | — | — | 3.3min | ~7.5k（累计 199,091 差值） |
| 评审 R1 | architect-reviewer 四查 + 判档，GREEN（0 blocking · 3 should · 1 advisory） | — | — | 11.8min | 93,700 |
| FROZEN | 主线程核对折进 → 标冻 → commit 8d130a6 | — | — | 分钟级 | 主线程 |
| landing | T1–T6 落 17 件 + V1–V5 全绿 + landing-summary | — | — | ~40min（估） | 主线程 |
| 核验 | landing-verifier 三件核验 | — | — | 进行中 | — |

对照：单轮 GREEN（无否回循环）是本批最大提速项；立项 ack 前置把 Track A 风险消在起草前。

## 2. Token 账

| 载体 | tokens | 备注 |
|---|---|---|
| architect（三段累计） | ~199k | 起草 178k + 改稿两段 ~22k |
| architect-reviewer | ~94k | 单轮 GREEN，无重审 |
| landing-verifier | 进行中 | — |
| 主线程 | 估 ±30% | 调查 + 编排 + 17 件落地直改 |

花费最大处：architect 起草（全新五段式 + 机制地面亲核）。可压缩分析：立项材料（ARQ 预消化 + 会话内已收敛的六工作项设计）直接进任务包，起草一次成型无返工——「human 先拍分叉再起草」比「起草带分叉去评审」省整轮否回。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 落地期自伤两处，全被 V1 秒抓**：① firmware-build-check self-check 编辑时引入重复 `const allOk` 声明（SyntaxError，脚本整体不可跑）；② enforce-scope `locateGitTree` 初版从 `dirname(startPath)` 起探，对目录形态的 homeProjectDir 永远找不到 `.git`（假树六臂两臂红）。根因：主线程直改代码无编译反馈环，靠 V1 义务兜。证据：两处 self-check 首跑即红、修复均在分钟级。教训成立而非新债——V1「落地前六自检全绿」正是为这类错误设计的保险丝。
- **F2 · Edit old_string 两次失配**：verify-gates.json 条目已带 _doc、verify.mjs 循环行手抄漏 `const`——先读后改即可避免；小事，列入主线程落地纪律提醒。
- **F3 · OQ-3 的「.async 图索引」在起草期被默认接受、human 一句反问消解**：architect 与主线程都沿 ARQ 叙事走了「异步不阻塞」，human 用「快照导入不就几分钟吗」把整条规则修订消掉（manifest 19→17 件、ARQ-260905 零触）。根因：起草侧对「ARQ 原文断言」的默认信任高于对「SSOT 现文（SKILL 步骤 5）」的复核。PROPOSAL 已按「矛盾如实呈现 + 留根」口径落。
- **near-miss（最险一刻）**：verify.mjs 接缝臂首跑失败时，表象是「seam 逻辑不对」，真因是 F1① 的语法错误让被 spawn 的 firmware-build-check 整体崩——若当时顺着 seam 逻辑去改 verify.mjs，会把对的代码改坏。靠「直接手跑 seam 命令复现」定位到真因。教训：集成臂红先隔离单件跑，再怀疑接缝。

## 4. 优化建议（按 ROI 排序）

1. **「分叉前置到立项 ack」写进 ARQ→批的默认动线**：治 F3 的上游——ARQ 条目自评 Track A 的「设计分叉」若能在立项对话中以推荐选项 + human 一次拍板闭合，本批证明可免整段对抗收敛。载体 = 登记不硬治（gov-change SSOT 已有「立项确认 ARQ 修法」条文，本批是其实践样本，不另立规矩）。
2. **落地期「先跑被改脚本单件自检再跑全量 V1」**：治 near-miss 的排查成本（本批靠手跑 seam 定位，顺序反了会走弯路）。载体 = 登记不硬治（V1 义务不动，主线程操作习惯）。
3. **AI 起草的 file:line 快照在 packet-bound 落地全程零漂移**（起草时点标注 + 落地逐条对锚，正面记录）——维持现有 PROPOSAL 模板的行号快照声明，不改。

## 5. 一句总结

子 agent 合计 ~293k tokens + 主线程一段会话；human 触点 3 次（立项 ack 两轮 + OQ 裁定）全承重；制品 4 件 + INDEX 行；单轮 GREEN 零否回为同日最快 gov 批。最险一刻 = 接缝臂失败的错误归因（被 F1 语法错误遮蔽）。**保险丝判定：两方独立结构接住作者偏差**——reviewer 抓出作者（architect 与主线程派发单）都漏的三处条文完备性缺口（F2 树处置分叉 / F3 完成标准自相矛盾 / F4 残留引用），verifier 独立逐件 diff 核验进行中；主线程落地自伤两处被 V1 机械保险丝接住。最值得带走的一条：**human 的领域反问（图索引到底几分钟）能消解整条治理修订——立项对话不是橡皮章，是成本最低的设计评审位。**

## 6. 本环遗留（条件节）

- V6（下一 lite 包首跑全链冒烟）/ V7（下一 worktree 包跨树实测）：由触发包 pod-retro 回填（PROPOSAL §5/§6 留根）。
- 图索引建库 >10min → tech 域债条目（OQ-3 口径）；pod-stop.mjs → 首例摩擦触发新 ARQ（OQ-1 裁 a）。
- 三条债条目（ARQ-261015-lite-package-execution-overhead / ARQ-261015-launch-prep-host-only-env-defer / TECH-261014）状态翻转归落地后主线程/渲染器。
- gateProfile 机械选闸化、riskTier-overrides-rigor 机器牙齿：维持留根（PROPOSAL §6）。
