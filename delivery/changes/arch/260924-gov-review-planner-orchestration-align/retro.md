# retro · 260924-gov-review-planner-orchestration-align（Track B 全环：起草→R1 BLOCK→v0.3→R2 GREEN→FROZEN→landing→verifier FAIL→补块→PASS→commit）

> 主线程第一人称热记忆 · 2026-09-24 · 乘客编号未在册（本仓 Track B 批次未建乘客计数面，如实标）· harness 形态：判官 = architect 子 agent（同实例 R1/R2 resume）、核验官 = landing-verifier 子 agent（同实例首轮/复验 resume），均专职 agent 类型、非零上下文降级形态。

**【硬项 R1】** verdict PASS（2026-09-24）后即写，环内趁热、第一人称；无隔场补记。

**【硬项 R2 · 计量诚实】** 子 agent tokens/时长 = harness usage 回传精确值（四次派发各自回传；同实例 resume 的第二次回传是否含首次累计**无法从回传判定**，两可如实记，合计值按「各次回传相加」口径给上界）。主线程各相位无独立计量，按会话墙钟估计、误差界 ±。

## 1. 时间账

| 相位 | 内容 | 时长 | tokens |
|---|---|---|---|
| 调研 + 归类 | 21 条未结 ARQ 摘要（Explore 子 agent）+ 10 项处方落点机械复证（Explore 子 agent）+ 批次方案 | ~40 min（估 ±10） | 两 agent 回传未单列（计入会话） |
| 起草 | PROPOSAL v0.1 + T1–T6 脚手架 + §5 全量实跑 + 立项包 | ~50 min（估 ±15） | 主线程（无计量） |
| 立项 ack | human 前端对齐三段 + OQ-1/2/3 裁定（同场一次触点） | human 侧 | — |
| 评审 R1 | architect 判官：BLOCK（F1–F4 + N1–N4） | 2464 s（精确） | 131,530（精确回传） |
| v0.3 修订 | 10 处 PROPOSAL 修订 + preflight 复跑 | ~10 min（估 ±5） | 主线程 |
| 评审 R2 | 同判官 resume：GREEN | 580 s（精确） | 170,717（精确回传，累计口径两可） |
| FROZEN + landing | 标 FROZEN v0.3 + T7 账本写回（6 条目 + render/doctor/幂等双证）+ V 集全量复跑 + landing-summary | ~25 min（估 ±10） | 主线程 |
| 核验首轮 | landing-verifier：FAIL（唯一阻断 = summary 缺 manifest 块） | 864 s（精确） | 112,035（精确回传） |
| 修复 + 复验 | 补块 + 注记 7 + 结构门复跑绿 → 同核验官 resume：PASS | 422 s（精确） | 131,757（精确回传，累计口径两可） |
| commit + retro | 9792b1c（17 files, +681/−36）+ 本件 | ~15 min（估 ±5） | 主线程 |
| **合计** | 七步闭环全走 | **~2.7 h 墙钟（估 ±30 min）** | 子 agent 回传合计 ≤546,044（上界口径见 R2 声明） |

对照：260923-gov-review-arq-ssot-restructure（94 条目迁移、R1–R3 三轮）为近期最重批；本批两轮收敛 + 一次核验返修，属中位偏轻。

## 2. Token 账

计量方式诚实声明：子 agent = harness usage 精确回传（见表）；主线程无分相位计量，不装精确。

| 载体 | tokens | 备注 |
|---|---|---|
| 主线程（调研编排 + 起草 + landing + retro） | 无计量 | 大头为 21 条目全文读入 + planner.md/task-intake 全文读入（起草精度所需，不悔） |
| 判官 architect（R1+R2） | ≤302,247 | R2 为 resume，回传是否含 R1 累计两可 |
| 核验官 landing-verifier（首轮+复验） | ≤243,792 | 同上口径注 |
| 调研双 Explore | 未单列 | 21 条摘要 + 10 项机械复证并行，产出密度高，值回 |

花费最大处：判官 R1 的 14 处插入文本 byte 级比对 + 全套闸亲跑——**这正是它抓到两条 BLOCKING 锚文失逐字的原因，不压缩**。可压缩分析：主线程起草期若先用机械提取（`grep -o` 取锚文原文）代替目视转录，R1 的 F1/F2 本可在起草期自灭（省一轮评审 ~131K tokens）——见 §3-F1/§4-2。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 锚文目视转录两处失逐字（R1 两条 BLOCKING 的根因）**：现象——脚手架期一次 Edit「string not found」，python 探针也报目标串不存在；当时归因「Windows 编码搞坏探测」，换下一行做锚绕过，PROPOSAL 引文未改 → 判官 byte 比对抓出「对端条目」实为「对端面」（planner.md HEAD:42）、加粗「**origin**」实为 code-span「`origin`」（task-intake:56）。根因——锚文从长行 Read/grep 输出**目视转录**而非机械复制，两字形近致误；**更深的根因是把真信号当工具故障**：python 的 -1 与 Edit 失败都是磁盘实况的正确报告，被「编码损坏」假设掩盖。证据锚：PROPOSAL §7 R1-F1/F2；本会话 Edit 失败与探针记录。这是 memory「改法里的引文是承重指令、落地逐词核」教训的再次显形（260914 批首实例）。
- **F2 · landing-summary 缺 landing-manifest 围栏块 → 核验首轮 FAIL**：现象——结构门 RED（structural-format-missing-manifest）；根因——summary 侧块判据（architect-operating-model.md:122「architect-landing-summary 同含其块」）在评审期无门覆盖（preflight 只查 PROPOSAL 侧、landing-fidelity 主模式才查双侧），主线程手写 summary 时未对照先例四批形态（对照即见块）；评审 R2「结构字段在场」声明实指 PROPOSAL 侧，无人核 summary 侧。证据锚：landing-verdict 首轮 FAIL 段 + ADVISORY-1。
- **F3 · §5 V5 幂等断言评估时点缺陷**：现象——「crq-render 复跑后 `git diff --exit-code -- docs/crq/` → exit 0」在 commit 前恒假（工作树对 HEAD 必有 diff）；根因——起草期实跑义务对「landing 后态」断言存在盲区（未来态无法干跑），断言未标注评估时点；处置——summary §3③ 以双证（render「未变」输出 + sha256 前后全等）+ commit 后终证口径满足断言语义，核验官复验认可（复跑五 INDEX 全「未变」）。与 260921 批 V7「断言过宽」同族（PROPOSAL §5 断言形态缺陷、判官未抓）——**该族第二实例**。证据锚：summary §2 V5 口径注、verdict 复验要点 4。
- **F4 · Windows Git Bash 下 python 中文探针双重编码坑**：现象——`python -c` 内联中文串 argv 损坏 + stdout cp1252 打印报错，探测结果不可信；根因——Windows python 控制台/argv 编码与 UTF-8 源不一致；处置——弃内联探针，改用 Read 工具 / grep / 脚本文件 + 输出落文件。教训与 F1 交织：**编码坑真实存在，但它掩盖的那次 -1 是真信号**——「工具坏了」与「引文错了」要先用第二信号源区分（grep 同行命中但 Edit 失败 = 引文有误；grep 也不命中 = 串真不在）。
- **near-miss（最险一刻）**：F1 的绕过路径——若当时信了「编码损坏」假设顺手把 PROPOSAL 锚文「修正」成 python 能找到的形态（或反向把脚手架改成迁就错误锚文），就会把一个转录错误升级成**冻结件里的真锚文错误**，landing-verifier 逐 byte 对账时要么炸在更晚的相位、要么被「两侧一致地错」静默放行。没出事的原因：绕过时选择了**不触碰引文**的中性锚（下一行整行），错误只留在 PROPOSAL 引文侧、被判官 byte 比对独立抓出。教训：单工具负结果先用第二信号源交叉，再决定改谁。

## 4. 优化建议（按 ROI 排序）

1. **锚文机械提取纪律**：PROPOSAL 承重 landing 指令的「锚定原文」一律机械提取（`grep -o` / 单行 Read 后复制，落笔前 byte 对账一次），禁长输出目视转录——治 F1/F4 · 载体 = 本 retro 登记 + memory 追注（既有「位置副词是承重指令」条目的实例强化）；**固化候选**：`PROPOSAL.template.md` 起草期自检两句扩第三句 → 交 gov-arq 登记（G2 模板面、一行级）。预期收益：锚文类 BLOCKING 在起草期自灭，省一轮评审（本批实测 ~131K tokens / 41 min）。
2. **summary 侧 manifest 块的评审期覆盖**：判官「结构字段核对」义务显式写明「summary 侧亦须在场」或 preflight 增查——治 F2 · 载体 = gov-arq 登记（核验官 ADVISORY-1 原处置：留根级、非本批修；登记后随批 C 或模板小修批落）。预期收益：消掉一次核验返修往返（本批实测 ~7 min + 132K tokens）。
3. **§5 断言标注评估时点**：凡「landing 后态」断言必须标 pre-commit/post-commit 评估时点与 commit 前等效证据形态——治 F3（260921 V7 同族第二实例）· 载体 = gov-arq 登记（与 ARQ-260914 残余「断言口径↔保留面对账」同族并批候补）。预期收益：断言恒假红/恒真空两类形态错在起草期暴露。
4. **Windows python 中文探针姿势**：登记不硬治（个人作业纪律：内联 argv 禁中文、输出落文件、或用 Read/grep 替代）——治 F4 · 载体 = 本 retro 记录即可。

## 5. 一句总结

墙钟 ~2.7 h、子 agent ≤546K tokens（上界口径）、human 触点 **1 次**（立项 ack + 三 OQ 同场；FAIL→修复→PASS 全程零 human 出场，前端对齐设计如预期运转）、制品 4 件 + INDEX 行 + 账本 8 件写回；最险一刻 = 把真信号（Edit 失败 + 探针 -1）当工具故障绕过、错误锚文险入冻结件；**保险丝判定：两方独立结构接住作者偏差**——判官 byte 比对抓住两条主线程自查两轮都没发现的锚文失逐字（BLOCKING 级），核验官结构门抓住判官两轮都没核的 summary 侧缺块（FAIL 级），且 F3 断言缺陷由主线程自申报、核验官复核口径——三层各接一类，无一漏网；最值得带走的一条 = **锚文目视转录是承重指令的头号污染源，机械提取 + byte 对账必须成为起草期反射**（建议 1）。

## 6. 本环遗留（批次型变更特有）

- PROPOSAL §6 留根 1–9 全数有效：1/3（G3 机械齿 + 噪声件）→ 批 C；2（uiux 件，含起草期实测完整引用面清单）→ 批 D；4（260918 残余，实测前置仍欠）；5/6/9（自律件）；7（存疑样本已核证闭合）；8（settings Write deny 件）→ 批 B。
- 核验官 ADVISORY-1 → 本 retro §4-2 转 gov-arq 登记候选（批 C 候补），处置已获核验官确认无需另办。
- 判官 R1 附带勘误收讫：hardExclusionsOf 实际位置 = `.claude/scripts/crq-ledger-shared.mjs:68`（主线程派单引注 `lib/` 子路径有误；PROPOSAL 未引该路径，无需改文件）——派单引注同属「目视转录」污染面，随建议 1 纪律一并治。
- commit 后 V5 终证（`git diff --exit-code -- docs/crq/` 于 landing commit 后 + render 复跑）随合并相位补跑，结果追记于本文件末行（append-only）。

**V5 终证追记（2026-09-24，landing commit 9792b1c 后实跑）**：`crq-render.mjs` 复跑输出「未变」×5（全部 INDEX）；`git diff --exit-code -- docs/crq/` → **exit 0**（工作树 == HEAD，幂等终证成立）。§2 V5 口径注的 post-commit 评估时点断言自此闭合。

**摩擦面补遗追记（2026-09-24，合并后登记相位，append-only）**：retro 撰写时未及捕捉的第四类摩擦在 ARQ 登记相位现形——**主线程派判官/核验官时第三次违反「原样携带 canonical 简报」纪律**：两份任务书均手写（引 AGENTS.md/protocol 条文替代 `.claude/references/track-b-{judge,verifier}-brief.md`，派遣时未检索该二件），且**各漏同一条 verdict 行形态机锚义务**（行首 `verdict: <TOKEN>`——升级闸 readGovAnchors / changes-index deriveLanded 的机器消费面），两 officer 均靠自身能力补正、全程静默无实害（升级闸 PASS、INDEX landed/mergedInMain 均正确）。根因非 canonical 缺口（brief 自述「每环复制本简报为派遣 prompt」），是主线程「行动时想不起查记忆」——载体 = memory 条目 `carry-canonical-briefs-verbatim` 追注第三实例 + 动作化升级（触发点绑流程步骤不绑记忆召回），**不登记 ARQ**。本追记为 §3 摩擦面补遗（F5），§5 保险丝判定不变且反获佐证：两 officer 在任务书缺陷下仍各自独立抓出真问题。
