# retro · 261028-gov-review-p8-gate-semantics-cluster（Track B 全环：R1 BLOCK(F1) → R2 BLOCK(F1′ 闸层残段) → R3 GREEN → 落地 → PASS + 5 ADVISORY）

> 主线程第一人称热记忆 · 2026-10-28 · gov-debt-reduction workstream 第 8 乘客（P8 门语义簇）· harness 形态：architect / architect-reviewer / landing-verifier 均为 headless 子 agent 全上下文形态；判官三轮同一实例保持评审记忆连续。跨乘客对照：R2 抓「夹具判别层错位」是 fixtures-must-discriminate 纪律的第二次实证（上次：某批两处全绿溜过，判官+核验官各抓一次）。

**【硬项 R1 · 环后趁热、第一人称】** verdict PASS 后即写，第一人称，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent tokens 来自 harness usage 回报（精确）；主线程本会话无逐相计量——§2 主线程行按会话体感估计，误差界 ±30%。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 每条 F 均带 现象 → 根因 → 证据锚。

**【硬项 R4 · 建议必标载体】** §4 每条建议均标载体。

**【硬项 R5 · 保险丝判定】** 见 §5：判官独立接住作者偏差（R1/R2 两轮），landing-verifier 独立接住主线程披露准确性（一次疑似偏差经逐字节比对证实为核验官自身误读，非作者偏差）。

## 1. 时间账

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 立项 | 复证两条 ARQ + human 两拍（AskUserQuestion 二轮，第一轮 human 反馈「看不懂」、主线程白话转译后拍定） | 2026-10-28 上午 | 同日 | ~1h（含白话转译往返） | 主线程 ~15 万（估） |
| 起草 | architect PROPOSAL v0.1（中途 429 打断一次、换号续跑） + OQ 三拍回注 | 同日 | 同日 | ~24min 跑批 + 中断等待 | 418,835（81 tools） |
| 评审 R1 | 判官四查 → BLOCK(F1) + 2 NIT | 同日 | 同日 | ~17min | 113,101（59 tools） |
| 改稿 v0.2 | F1 双保险 + F2/F3 | 同日 | 同日 | ~11min | 449,335（38 tools） |
| 评审 R2 | 判官聚焦复验 → BLOCK(F1′ 闸层残段) + F4 NIT | 同日 | 同日 | ~6min | 133,131（16 tools） |
| 改稿 v0.3 | 案① 闸层闭合 + 夹具升层 + F4（中途 429 打断一次、续跑收尾） | 同日 | 同日 | ~2min 跑批 | 470,698（6 tools，含续跑前中断轮） |
| 评审 R3 | 判官三重实证 → GREEN | 同日 | 同日 | ~6.5min | 146,603（14 tools） |
| FROZEN + 暂停 | human 指示 review 通过即暂停；主线程标 FROZEN v0.3、修 F5 一行 | 同日 | 同日 | ~10min | 主线程少量 |
| landing | 主线程 V1–V9 全套 + landing-summary（human「landing吧」后） | 同日 | 同日 | ~30min | 主线程 ~10 万（估） |
| 核验 | landing-verifier → PASS + 5 ADVISORY | 同日 | 同日 | ~9min | 109,407（34 tools） |
| **合计** | 同日单日全环（含两次 429 等待） | | | 有效跑批 ~1.5h | 子 agent 1,841,110（逐载具精确） |

对照：P6.5 实测「同量级实现全批成本较 P6 降 ~40%」；本批 42 处编辑/15 件为该系列最大批（P7.5 为 17 处/8 件），评审三轮属该系列首次 BLOCK×2，成本上涨主要在两轮改稿，符合「评审强度换正确性」预期。

## 2. Token 账

计量方式：子 agent 行 = harness usage 精确值；主线程两行 = 会话体感估计（±30%）。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| architect（起草 + 两轮改稿） | ~1,338,868 | ~78% | 含 429 中断续跑 |
| 判官（R1–R3） | ~392,835 | ~23% | 三轮同实例 |
| 核验官 | 109,407 | ~6% | 全流程核验，34 tools |
| 主线程（复证 + 立项 + landing + 记账） | ~25 万（估 ±30%） | — | 含 ARQ-281028 插话登记 |

花费最大处：architect 改稿轮（v0.2 449k 跑 11 分钟、v0.3 470k 跑 2 分钟——重装配 + 全量自证占大头）。可压缩分析：判别脚本硬编码基线路径导致落地期 sed 改指 + import 闭包手工补齐，都是可固化的机械环节（见 §4）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 自检电池全绿而闸层有洞（判别夹具打错层）**：现象 = v0.2 自检 85/85 + mutation 6/6 全绿，但设备门「最懒路径」在闸消费层照旧放行 → 根因 = 两个负例与 mutation 判别 case 全打在 assess 层，evaluate 层零判别 → 证据锚 = R2 判官五案直调实测 `blocked=false`（PROPOSAL §7 R2 段；`pre-merge-escalation-gate.mjs` T3b `!required` 分支写死 `required:false` vs T3c 判据 `required && !ok`）。教训成句：**判别 case 必须打在消费/拦截层，不是判据计算层**。
- **F2 · human 立项读条目懵**：现象 = P8 立项第一轮 human 两问均答「没太看懂」→ 根因 = ARQ 条目散文假定读者已带机制上下文 → 证据锚 = human 原话（已登记 `ARQ-281028-arq-entry-plain-language`，本环当场用白话转译化解，立项 ack 顺利完成）。
- **F3 · 429 配额两断**：现象 = 起草与 v0.3 改稿各被 5 小时配额打断一次 → 根因 = 单账号配额上限，非流程问题 → 证据锚 = 两次 task-notification（429，重置 13:47 / 18:56）。agent 上下文原地续跑零丢失，值得保持的做法：**中断点自述进度**使续跑指令可以精确接上。
- **F4 · 装配副本缺 import 闭包**：现象 = 副本上跑三闸自检 ERR_MODULE_NOT_FOUND（verifier-evidence-check.mjs + lib/ 三件）→ 根因 = 装配器 copy 清单只含本批新文件，未含被改脚本的共享依赖 → 证据锚 = 主线程落地实跑报错 + 手工补齐后自检通过。
- **F5 · 判别脚本硬编码判官基线路径**：现象 = `_scratch/mutation-check.mjs` / `evaluate-five-cases.mjs` 写死 `C:/tmp/p8-assembly9`，落地复跑需 sed 改指 → 根因 = 判官/起草各自建副本目录、脚本不参数化 → 证据锚 = 两脚本 :5 / :1。
- **near-miss（最险一刻）**：R2 若判官只复跑自检电池（85/85 + 6/6 全绿）而不做 evaluate 层直调，F1′ 洞会带 FROZEN 落地——设备轮硬门在最懒路径上形同虚设，而这恰是本批要治的「带病合并」同型风险。没出的原因 = 判官按其纪律对消费层做了五案直调（未采信回执）。教训：验证集义务里「反转实现变红」必须钉死**在哪一层变红**。

## 4. 优化建议（按 ROI 排序）

1. **mutation/判别夹具层别义务成文**：治 F1 · 载体 = ARQ 收件箱（候选修法：验证集义务加一句「判别 case 须落在消费/拦截层；计算层与消费层分离时两層各一个判别」；若批内自检设施支持，mutation-check 对多文件闸脚本按消费层出口断言） · 预期收益 = 消灭「电池全绿闸层带洞」这类静默假绿。
2. **装配器 copy 清单补依赖闭包或自检**：治 F4 · 载体 = ARQ 收件箱（候选修法：assemble-edits 对被改 `.mjs` 扫 import 闭包、缺失即 advisory 提示或允许 copy 声明；或 runbook 一句话） · 预期收益 = 落地期副本自检零手工。
3. **判别脚本根路径参数化**：治 F5 · 载体 = 并入建议 2 同一 ARQ（scratch 模板首行 `const root = process.argv[2] || …`） · 预期收益 = 落地复跑免 sed。
4. **立项材料白话段**：治 F2 · 载体 = 已登记 `ARQ-281028-arq-entry-plain-language`（本环实证白话转译有效，条目侧修法待立项）。
5. **429 中断续跑协议**：治 F3 · 载体 = 登记不硬治（harness 平台行为；agent 中断点自述 + 主线程精确续跑指令已实证有效，保持惯例即可）。

## 5. 一句总结

单日全环、子 agent 1,841,110 tokens、human 触点 3 次（立项两拍 + OQ 三拍一次 + landing 指令一次，其中立项因条目可读性多花一轮白话转译）；最险一刻 = R2 前自检全绿而闸层带洞、靠判官独立直调接住；**保险丝判定：两方独立结构接住作者偏差**——判官独立两轮 BLOCK（F1 计算层、F1′ 消费层各一次）把 architect 两版「自证全绿」的草稿打回，landing-verifier 独立核验 15 件逐文件 diff 且对自身一次疑似偏差做了逐字节自证；最值得带走的一条 = **判别夹具必须打在消费层**（电池全绿 ≠ 消费层正确）。

## 6. 本环遗留（留根去向）

- FROZEN §6 七条留根全部保留（抽缝个案 / 登记集首批非完备 / generated-noise 无种子 / 在途包不追溯 / 日志期限 / 先例扫描规模 / OQ-3 拆批备选）——去向 = 各条目与 §6 原文，不另开账。
- §4 建议 1–3 待登记 ARQ（主线程落地 commit 后随即登记）。
- landing-verifier ADVISORY 五条（归档件 CRLF cosmetic / INDEX 连带刷新 / V4 等价性留痕 / .codebase-memory 不 stage / cite-check notChecked 预期）均不阻 PASS、无需处置动作。
