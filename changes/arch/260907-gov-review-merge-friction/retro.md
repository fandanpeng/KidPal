# retro · 260907-gov-review-merge-friction（Track B 全环：立项 review → R1 打回 → 修 → R2 GREEN → landing → verdict RED→修复→绿）

> 主线程第一人称热记忆 · 2026-09-07 · 乘客 #n（同日第三治理批）· harness 形态 = 判官 / 核验官均为独立子 agent（architect 判官携 R2 续场复用上下文；landing-verifier 续场复验）· 跨乘客对照：本批 OQ2 在立项 review 当场被 human 复议改判，是「human 取舍点」真正起作用的一例。

**【硬项 R1 · 环后趁热、第一人称】** verdict 绿后即写，无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent tokens 有 harness 实测（见下表）；主线程无计量，按会话规模估计 ~150k tokens（±30%，含大量 git/源码核证往返）。

## 1. 时间账

| 相位 | 内容 | 时长 | tokens |
|---|---|---|---|
| 立项 review | 主线程地面真值逐条核证（9 项声明全验）+ 两处起草面修正 + human 三 OQ | 会话内（未单独计时） | 主线程 |
| OQ2 复议 + T6 增补 | human 否决「merged 行 main 永久缺行」→ 实测 mergeCwd 机制 → 新增 T6 | 同上 | 主线程 |
| 评审 R1 | 判官 NOT-GREEN，六项全起草面、零升级项 | 318 s | 106,276 |
| R2 复核 | 六项修正逐条核销 → GREEN | 46 s | 281（续场缓存） |
| landing | T1–T6 落地 + 六自检 + 新自检 10/10 + 两 commit | 主线程 | 主线程 |
| 核验 R1 | landing-verifier RED（D1 文件名 / D2 非阻断） | 405 s | 50,816 |
| 核验 R2 | 修复复验 → GREEN | 131 s | 374（续场缓存） |

对照：判官/核验官的续场复用（SendMessage 续同一 agent）把第二轮成本压到 ~1%（281/374 vs 首轮 10 万级），是本环最划算的一步。

## 2. Token 账

计量方式：子 agent = harness usage 实测（上表）；主线程 = 估计 ±30%。

花费最大处：R1 判官首轮（106k）——评审清单七件 canonical 全文亲核 + 行号级抽查，值这个价；主线程最大头在「立项 review 地面真值逐条核证」（9 项声明每条跑 git/grep），这部分同时把 R1 判官的地面真值抽查变成零打回，复利为正。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · landing summary 文件名不合规（verdict 唯一阻断项）**：现象 = 我落成 `landing-summary.md`，结构门硬编码要 `architect-landing-summary.md`，且文件缺席时 packet-binding/manifest/git 三项机检全部不跑（fail 弱位置：只在 message 报缺文件，不报连锁）→ 根因 = 我按 AGENTS.md 第 4 步「写 landing summary」字面命名，没先跑结构门或查门脚本硬编码名 → 证据锚 landing-fidelity-check.mjs:274 + 本批 verdict D1。教训：**landing 落盘前先跑一次结构门**（名字/格式类硬编码约定让机器先说）。
- **F2 · PROPOSAL 锚行格式不被门识别**：`**目标文件：`（粗体）不命中门正则 `^目标文件\s*[:：]`（landing-fidelity-check.mjs + governance-gate-shared.mjs parseSection2AnchorLists），七条「缺锚清单」误报式暴露 → 根因 = 起草时自造了粗体变体，模板先例没核对 → 证据锚本批 verdict 修复段。教训：结构字段的字面形态以门正则为准，不以视觉美观为准。
- **F3 · gate 脚本顶层直跑不可 import**：T1 冻结文写「复用既有五要素核验」，但 pre-merge-escalation-gate.mjs 无 import.meta 守卫、import 即带跑全闸 + process.exit → 根因 = 本仓脚本无「可导入库 vs 可执行脚本」分离约定 → 证据锚 pre-merge-escalation-gate.mjs 尾部顶层代码。落法 = spawn（「同 Step 1b 透传路径」的字面复用），语义无损但留根：库/脚本分离值得单独治理。
- **near-miss（最险一刻）**：T1 若按冻结文「board reconcile 后、ready 收集前」字面实现（setPod 后直接进 ready 收集），会落地即失效——内存 board 是 freshBoard 快照、setPod 只写 pod 文件。被 R1 判官 F-1 抓住（判官亲核 main() 数据流）而非我自己发现。教训：改状态机时「谁持真值」要跟数据流核到底，冻结文的一句落点描述不等于实现充分条件。

## 4. 优化建议（按 ROI 排序）

1. **landing 前先跑结构门自检**：治 F1/F2 · 载体 = 登记不硬治（写入主线程 landing 习惯；结构门已是机器闸，无需新门）· 预期收益 = verdict RED 轮次归零。
2. **库/脚本分离**：治 F3 · 载体 = ARQ 收件箱（建议：共享函数提取至 `.claude/scripts/lib/`，可执行脚本薄壳化；涉及 gate 等承重脚本，须独立治理批）· 预期收益 = 「复用核验」类改法从 spawn 变 import，消双跑开销。
3. **判官/核验官续场复用惯例化**：载体 = 登记不硬治（Track B 编排习惯）· 预期收益 = 复审轮成本 ~1%。

## 5. 一句总结

成本 = 子 agent ~158k tokens 实测 + 主线程估计 ~150k（±30%）/ 触点 = human 3 次（立项 ack + OQ 三裁、OQ2 当场复议改判 (c)）/ 制品 4 件齐 · 最险一刻 = T1 字面实现即失效（判官 F-1 接住）· **保险丝判定：两方独立结构接住作者偏差**——判官 R1-F1 抓住 T1 机械缺口、verifier R1-D1 抓住文件名硬编码，两处都是作者（我）自查漏掉的，独立结构各接住一次 · 最值得带走的一条 = **落地前先跑结构门，让机器先说硬编码约定**。

## 6. 本环遗留

- **行为级最终权重未了**：PROPOSAL §5 第 8 项「下一个真实 Pod merge 段一次跑成」随合并后首例 Pod 观测（Step 0 收口 / T1 解锁 / T3 特判 / T6 簿记均为夹具级验证，真机合并是终审）。
- **INDEX 收敛欠账**：main 的 changes/INDEX.md 缺 schedule-p2 行（立项 review 实测），待下批 changes-index.mjs 全量重建补齐（T5 ② 口径已载）。
- **ARQ-260907-escalation-cross-system-overfire**：不承载，Track A 另行（§6 留根指针）。
