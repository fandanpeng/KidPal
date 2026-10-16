# retro · 260908-gov-review-input-surface-frontload（Track B 全环：起包输入面封闭化 + 跨端申报闸，含一次熔断换判官）

> 主线程第一人称热记忆 · 2026-09-09 · 乘客 #本仓第 18 批（`changes/INDEX.md` arch 段计数）· harness 形态：**判官 = typed `architect` agent 主线程派发**（R1–R3 同实例 resume、R4 = 熔断后零上下文新实例），核验官 = typed `landing-verifier`，非降级 coder 形态。跨乘客对照：本批是「**同判官三轮未 GREEN → 熔断条款被真实触发**」的第一例（此前各批最多两轮即 GREEN）。

**【硬项 R1 · 环后趁热】** verdict 绿（09:12）后即写，未隔场。

## 1. 时间账

| 相位 | 内容 | 锚点 | 时长 |
|---|---|---|---|
| 起草 | PROPOSAL v0.1 入册（前会话产物，本会话接手） | commit `014adfe`，09-08 **17:27** | — |
| 立项 ack → R1 | human ack → 派零上下文判官全面审 | R1 记录 `architect-review-r1.md` | **34.0 min** |
| v0.2 回灌 | R1 十四条 + human 五裁（OQ1/OQ2/OQ3 + F1 全量重跑 + ②③档留 ARQ）逐条落文 + 遥测现跑 | 本会话 | ~25 min（估，含 V1–V5 首轮实跑） |
| R2 | 同判官复核 v0.2 → NEEDS-WORK（SHOULD 2 / COULD 4，含**抓回主线程新加的 census 错**） | — | **16.5 min** |
| v0.3 回灌 | 六条 + 一处主线程自查残留 | — | ~12 min（估） |
| R3 | 同判官复核 v0.3 → NEEDS-WORK（**SHOULD 1**，零判断量）→ **触发熔断** | — | **9.2 min** |
| v0.4 回灌 | R3-F1 verbatim 替换句 + §8-E 归因；human 三选一裁 ③ | — | ~6 min（估） |
| R4 | **新判官实例**（零上下文）→ **GREEN，census 第三次独立重算逐数吻合** | `architect-review-r4.md`，mtime 07:50 | **21.1 min** |
| FROZEN + landing | 标 FROZEN v1.0 → 11/11 manifest 落地（新脚本 477 行 + 闸扩三条 + 四 def + 三 references + 聚合加列） | PROPOSAL mtime 07:52 / commit `7935842` 09:17 | ~90 min（估，含 V1–V5 全跑） |
| 核验 | `landing-verifier` → **GREEN 无 RED**，六探针破坏性自检 | `landing-verdict.md`，mtime 09:12 | **13.8 min** |
| **合计** | 判官四轮 + 核验一轮 = **独立面 94.6 min**；全程跨 09-08 17:27 → 09-09 09:17 | | **≈15.8 小时挂钟**（含 human 在场问答与主线程非连续作业，纯工作时间远小于此） |

对照：前一批 `escalation-conflict-ladder`（同日、同轨、六件 manifest）判官两轮 GREEN；本批四轮（含熔断换人）+ 独立面时长约为其 2 倍，**多出来的成本几乎全花在作者自产的事实性错误被反复打回上**（见 §3，三起全出自我手，非改法设计问题）。

## 2. Token 账

**【硬项 R2 · 计量诚实】计量方式声明**：harness 只回传子 agent 的 `subagent_tokens` 单值，**口径未知且与 tool 调用数明显不配套**（R1 报 15,677 tokens / 40 tool uses / 34 分钟，不可能是一次会话的真实消耗量），因此**本表不给 token 总数的伪精确值**，只登记回传原值并标「不可用」。主线程侧零分项（无 input/output/cache 拆分，无实际生效模型）——**这正是本批 §6 留根第一项、也是它第一次在本批自身闭环里造成实际障碍：retro 硬项 R2 在缺计量的前提下无法诚实完成**。误差界：一个数量级。

| 载体 | harness 回传 `subagent_tokens` | 可信度 | 备注（tool_uses / 时长） |
|---|---|---|---|
| 判官 R1 | 15,677 | **不可用** | 40 uses / 34.0 min |
| 判官 R2 | 17,910 | 不可用 | 8 uses / 16.5 min |
| 判官 R3 | 20,435 | 不可用 | 5 uses / 9.2 min |
| 判官 R4（新实例） | 12,973 | 不可用 | 27 uses / 21.1 min |
| 核验官 | 11,095 | 不可用 | 48 uses / 13.8 min |
| 主线程（起草 + 回灌 + 落地） | **无字段** | — | §6 留根项，本批未解 |

花费最大处：无法判定（缺主线程分项与 cache 拆分）。可压缩分析：四轮判官里 R2/R3 合计 25.7 min 全部在修**作者自己引入或作者自己写错**的数字（v0.1 遥测 as-of、v0.2 census），若起草期即按「防御性数字必须与上线谓词同源 + pin as-of」自检，可省一整轮（≈32 min + 一次熔断）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · 作者自产事实错三起（最痛，代价 = 一轮判官 + 一次熔断）**：
  - **① 遥测 as-of 未 pin**（R1/F1 BLOCKING）：现象 = 判官按文中命令复现得 91/13/47，文内写 72/12/40 → 根因 = 分析会话跑在 `260829-schedule-reminder` 遥测 merge（`e8b73c0`，09-08T16:53）之前，而 PROPOSAL 17:27 提交，**既未 pin as-of 也未声明排除** → 证据锚 = `architect-review-r1.md` A 节对账表（减法后三量严丝合缝，属确定归因非猜测）；实质后果 = 被排除的恰是全数据集**最大长尾**（builder 109.4min/226tools、integrator 71.9min/248tools），integrator 行「无长尾」在全量下为假。
  - **② census 谓词未绑文件名**（R2-F1 SHOULD）：现象 = 我 v0.2 新加的「净增覆盖 4/19，成员含 simulator-lvgl-align」被复算为 **5/19、成员应为 gateway-directionB** → 根因 = 普查按节号宽匹配未绑 `interface-contract.md` 文件名（该包 `§1.5` 实指 `docs/notes/feature-inventory.md`），且节集合与 T2 常量分叉 → 证据锚 = `landing-verdict.md` 独立探针（`interface-contract.md#§四`=true / `feature-inventory.md#§1.5`=false）+ 现文 `isWireRef`。
  - **③ 着色句缝错事件**（R3-F1 SHOULD）：现象 = 我给 gateway-directionB 加了「OpenClaw 拒 tools 数组那次」这顶帽子 → 根因 = **为给数字加画面感而重写一套归属**，未复用机判事实源 → 证据锚 = 该包全目录 grep「tools 数组 / nointent」零命中，事件原始记录在 `changes/dev/260903-device-ws-switch/review-packet.md:76` 的 SHOULD 行。
  - **三起同根**：prose 里的防御性 / 着色性陈述**没有复用机判谓词的单一事实源**。本批把这条写成条文（T2 advisory 单源声明、`WIRE_SECTIONS` 常量 `export` 供普查复用），**条文在落地前先打了作者自己两次**——这是本批最硬的自证。
- **F2 · 单判官连续轮对新面变钝**：现象 = R2 判官自记「R1 漏审诚实自记」（V5④ 要求四 def 含 `input-brief`，但 T4 portfolio-planner 那条改法压根不产出该元素）→ 根因 = 判官复用自己上一轮的 finding 清单作复核框架时，对「本轮改法与验证臂的可满足性」这类新交叉面着力不足 → 证据锚 = `architect-review-r1.md` R2 节 R2-F2 + `architect-review-r4.md` 对该臂的再核。**注意反面证据**：同判官 R1 抓出的 F1/F3/F7 三条 BLOCKING 都是新实例未必看得见的深度项——所以结论不是「换判官更好」，而是「**换实例补横截面**」。
- **F3 · 熳断计数不看严重度**：现象 = R3 只剩**一条零判断量的 SHOULD**（替换句判官已给、我照抄即闭），仍按「3 轮未 GREEN」触发升 human → 根因 = 熔断条款以「是否 GREEN」计数、不以残余严重度加权 → 证据锚 = §7 R3 条目 + `architect-review-r1.md` R3 节 severity 论证（判官特意写了「防放宽质疑」的理由）。**代价实测 = R4 新实例 21.1 min 复核两条焦点**。不自动改规则（治理形态归 human），交 §4 建议 6 拍板。
- **F4 · 冻结文本行锚 landing 后即位移**：现象 = PROPOSAL 里 `planner.md:109`、`acceptance-author.md:46`、`dev-loop.md:229` 等行锚在本批落地后位移 1–2 行 → 根因 = 冻结文本用行号锚而落地本身改变了行号 → 证据锚 = `landing-verdict.md` D4。**本批机制自己给过答案**：T2② 把行号定性为快照、非长期事实源；同一个纪律没应用到 PROPOSAL 自己的引用上。
- **F5 · 结构门 citation 检查的短式解析局限**：现象 = `landing-fidelity-check.mjs` 报 14 条 `file-not-exist` → 根因 = 它对 `builder.md:24` 这类短式引用按仓根解析（真身在 `.claude/agents/`）→ 证据锚 = 核验者逐条实查文件与引文俱在（`landing-verdict.md` 结构门节）。属工具噪声非落地缺陷，但会让人误读门报。
- **near-miss（最险一刻）**：F1① 那类错误里，**最险的不是数字错，而是我差点为了省一轮而把 R3 那条拔成 COULD、改完自己判 GREEN 直接标 FROZEN**——判官当时特意写了 severity 论证防这一手（「非 COULD：虚假史实随 FROZEN 入 human 裁断记录，文内不可自恢复」）。若我那样做了，一个史实错置会随 FROZEN 变成 human 裁断依据的一部分，且此后无人能恢复原文。**没出的原因 = 判官把严重度理由写在纸面上，让「降档」变成可被下一方追责的动作而非我的临场裁量**；教训 = 作者不自判 GREEN 这类硬约束不是形式，它挡的就是这一手。

## 4. 优化建议（按 ROI 排序，每条标载体）

1. **ARQ 收编 merged 判定源第三臂（最高 ROI，机制级缺陷）**：治 D1（本批落地实测暴露）——`brief-inputs.mjs` 的 `isMerged()` 只认 `supervisor-verdict.json` APPROVE ∨ eventType 含 "merge"，而本仓主合并形态是 **ESCALATE + `human-decision` 事件 `payload.decision="approve-escalated-merge"`**，两源皆失 → 只认出 4/19 真 merged 包（实证 ≥8），先例路给的是陈旧样本。方向 fail-closed（少读不误扩），故本批不回改冻结文。载体 = **固化候选 → ARQ 收件箱 `ARQ-260909-precedent-merged-detection-gap`**（含修法与 `--self-check` 正控要求：decoy merged 包须入选）。预期收益 = 先例封顶这路从「恒给旧包」变成「真给最新三件」，V6 pilot 的观察数据才有意义。
2. **ARQ 收编前置升级闸 gov GREEN 机锚向宽**：治三轮复述的同一条（R1 范围外观察、R3/R4 再述）——`pre-merge-escalation-gate.mjs:83-84` 的 regex 含裸 `GREEN` 分支，任何正文出现该字样的 PROPOSAL 恒满足第 6 关（**本批 §7 判官 verdict 文本自带该词，即自证其害**）。载体 = **固化候选 → ARQ 收件箱 `ARQ-260909-gate-green-anchor-regex-loose`**，修法 = 收窄至 verdict 行/段形态 + 反例 fixture（正文含 GREEN 而无 verdict 须判不满足）。
3. **ARQ 收编消费方②③档分档**：治 §6 已裁留根——`crossEnd.counterparts` 的 `{path,symbol}` 只能表达自有实现面，只读参考树（版本控制外）与黑盒消费者（OpenClaw、物理声/显）无处落；黑盒档真正需要的是「哪条 live 断言钉住它」的 oracle 锚语义，本批无数据支撑不硬造。载体 = **固化候选 → ARQ 收件箱 `ARQ-260909-crossend-consumer-tiers`**，处理时机 = pilot 首个真遇②③档的包。
4. **ARQ 收编遥测计量补全（本批 retro 亲测受阻）**：治 §2——`subagent_tokens` 口径不明 + 主线程零分项 + 44/91 事件无 token 精确值 + 22 条 planner 事件仅 2 条有 durationMs，导致 retro 硬项 R2 无法诚实完成。载体 = **固化候选 → ARQ 收件箱 `ARQ-260909-dispatch-stamp-telemetry-gaps`**（§6 既有留根的合并升级，含「实际生效模型」「token 分项」「主线程自身观测」三子项）。
5. **治理件引用自家 canonical 改用短语锚**：治 F4——PROPOSAL / landing-summary 引用自家文件时用「引文片段 + 定位谓词」而非行号；行号在落地即失效是本批机制自己论证过的性质。载体 = **登记不硬治**：写进下批 Instruction Budget 复评清单 + `authoring` 类 reference 的既有撰写纪律（不新开条文、不为本条动 G 面）。
6. **熳断计数按残余严重度加权（交 human 拍，不自动改）**：治 F3——现「3 轮未 GREEN」不区分「剩 3 条 BLOCKING」与「剩 1 条零判断量 SHOULD」，后者仍触发换实例（实测成本 21 min）。可选形态：残余全为 COULD 且各条修法已给 verbatim 时可判「等效 GREEN（附条件）」并记档；**但这条一旦放宽就会给「作者求快」开门**（见 F5 near-miss），所以我倾向**不改**，只在 human 觉得本批这类情况反复出现时再议。载体 = **留 human 判断**（治理形态归 human，本 retro 不代裁、不预埋）。
7. **V6 pilot 观察项打包**：交日落复盘——前 2–3 包凭新 `actors` 列对 §1 P1 基线（acceptance 52.0–61.2min、builder 上限 109.4min/226tools、integrator 71.9min/248tools）；守护指标口径已钉死（story-p3 冻结后再派 3 次、fixer 13 包共 3 次），**恶化即回补 brief 展开规则而非废机制**；同批回看 R4 残余 3（Step 9 重跑触发词未含「crossEnd 迟填」分支）与 F5 结构门 citation 噪声（是否值得给脚本加路径解析回落）。载体 = **留批次**（`/loop-retro` cadence 消费）。

## 5. 一句总结

**成本**：独立面 94.6 min（判官 4 轮 + 核验 1 轮），跨挂钟 15.8 小时，token 不可信计量（见 §2 诚实声明）· **human 触点**：出场 5 次，其中**拍板 3 次**（立项 ack、五裁 OQ1/OQ2/OQ3+F1+②③档、熔断三选一裁 ③），另 2 次是要解释（OQ3 详解、画图问「在哪一步」）不决策 · **制品**：6 件（PROPOSAL v0.1→v1.0 / r1 三轮 / r4 / landing-summary / landing-verdict / retro）+ 11 canonical 文件 · 对照前批：同轨同日的 ladder 批两轮 GREEN，本批四轮，**差额几乎全由作者自产事实错造成**。**最险一刻** = 差点为省一轮把 R3 那条降格自判 GREEN（§3 near-miss）。**保险丝判定：两方独立结构接住作者偏差——接住了，且是三处独立冗余各接一类**：① 判官独立接住三轮共四条只有我能犯的错（F1①②③ + B1/B2 加码定性）；② **换实例**接住同判官对交叉新面的钝感（R2 自认 R1 漏审、R4 确认零残留并给过渡态实证）；③ 核验官独立接住我 summary 自报不实（474≠340 行）与**只在自测里才现形的空断言**（那条 `>=0` 恒真合取——我改完它首跑即 RED 18/19，等于当场演示了「看起来像验证、实则证不了什么」，正是本批 V 集反照透镜要抓的形态在自己脚本上的复现）。**最值得带走的一条**：防御性和着色性的 prose 也要复用机判谓词的单一事实源——本批为写这条条文，先在遥测、census、事件归属上各违约一次，第三次才被条文本身兜住。

## 6. 本环遗留

- **待登记 ARQ 四条**（建议 1–4，ID 已在文中占名）：`precedent-merged-detection-gap` / `gate-green-anchor-regex-loose` / `crossend-consumer-tiers` / `dispatch-stamp-telemetry-gaps`——**登记动作 = 本 retro 之后立即执行**（载体诚实性要求，不留「建议了但没收件」的债）。
- **不关的根**：F5 citation 短式解析噪声（工具局限，观察）；D2 exempt 地板措辞偏严（向严、接受，后续件对齐）；D3 `isWireRef` `startsWith` 宽判（advisory-only）；均**登记不硬治**，随建议 5/7 的 cadence 回看一并收。
- **过渡态在册**（F4 落地实况）：裸跑承重闸现 RED 3 项（`current/` 指向已 merge 包无新字段），无在飞 pod 故不阻塞；下个包若在飞按 T3「过渡注记」补跑 brief + 补填 crossEnd 一次即绿。
- **pilot 首包必盯两件事**：① 首个真包的 `input-brief` 是否真被子 agent 当封闭清单消费（还是照旧自扫）——机制生效性的第一证；② `over-scan` 事件是否真被产出（无生产者的枚举值 = 死 canon，R1/F14 的检验点）。
