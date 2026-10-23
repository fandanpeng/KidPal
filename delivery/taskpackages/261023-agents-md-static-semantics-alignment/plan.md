---
changeId: 261023-agents-md-static-semantics-alignment
rigor: lite
riskTier: low
sourceType: arch-review
reviewed-by: architect
date: 2026-10-23
---

# 261023 · workspace-kidpal 静态语义与现行链路口径对齐（批 A+B）

## 0. 范围与性质

- **改什么**：两份 live prompt 文本——`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`（板侧 kidpal agent 静态语义单一事实源，102 行）与 `server/deploy/openclaw-skills/workspace-kidpal/kidpal-schedule-answer/SKILL.md`（B1 同口径）。逐条 = 批 A（B1 / B2 / S1 / S4 / S5 / N1–N6）+ 批 B（B3 / S3 / S6）。
- **不改什么**：零代码、零契约（`docs/contracts/**` 零触）、零 manifest、零动态 context 模板（`kidpal-agent-prompt.txt` / `kidpal-dynamic-context.v1.txt` 零触）、零 tools 声明。
- **显式排除（human 2026-10-23 裁）**：
  - **S2**（工具面收窄 write/exec）= 不入本包，已由主线程登记 debt `docs/debt/tech/TECH-261023-openclaw-tool-face-prompt-only-readonly.md`（status: deferred）。本包只保留其「纵深冗余」侧：底线「数据只读」句不动（L85）。
  - **S7**（`{{today_date}}` 类占位外观残留）= 不入本包（live 已判零行为级泄漏）。B1 改写自带摘要缺失 / 截断降级句，已覆盖 S7 关注的唯一新增依赖。
  - B2 的可选对账项见 §5，**不产生实现修复**。

## 1. 背景与来源

- **来源 1（主）**：`delivery/review/261022-arch-review-device-memory-prompt-overhaul/agents-md-advisory-20261023.md`（architect advisory，human 已过目）——BLOCKING 3 / SHOULD 7 / NIT 6 逐条分级与改法草案。本 plan 直接升格其 §4「归批建议」的批 A + 批 B，不另起炉灶。
- **来源 2**：human 2026-10-23 起批指示——「起批 A+B；S2 / S7 不入本包、归 debt」。
- **根问题**：本文件是 v11（2026-10-18 定稿）的忠实迁移，但此后三件事没回改对应句子——10-19/10-20 记录会话改对话引导态（标题/正文改由 front 整理回合产出）、10-22 P5 课表机械注入（摘要进动态 context）、10-23 K3 记忆上线（v12 守则）。当前不是「少了什么」，而是**三处仍在强制已不存在的旧行为**，以及 v12 与两条底线的接缝未处理。
- **权威对账（为什么 B1/B2 不算改需求）**：B1 的新口径 = `docs/architecture.md` §5.2（`219` 行）已冻结的句子「课表回合因此无需工具调用即可作答；摘要窗口外日期仍走读文件或诚实说查不到」；B2 的新口径 = arch §5.2④（`223` 行）「忠实整理…起标题〔≤12 字口径不变、随整合同一回合产出〕」+ requirements S-F2.2 AC4（`462` 行）。**本包是把静态 prompt 拉回已冻结的 spec，不是提出新语义。**

## 2. 逐条改动清单

> 行号 = 现状文件行号（AGENTS.md 102 行）。改后措辞为**逐字终稿**，builder 原样转录；除标注「行内子串替换」外均为整行替换。

### 2.1 AGENTS.md

**（1）N1 · L1 标题去仓内视角**
现文：`# KidPal（小伴）· workspace-kidpal 工作区说明（仓内镜像）`（「仓内镜像」对模型零操作语义，属仓维注记）
改后：`# KidPal（小伴）· workspace-kidpal 工作区说明`
（镜像说明已由 `server/deploy/openclaw-agents/README.md` 承载，无需迁入。）

**（2）N3 · L13 查字回合行去重**
现文问题：行内重复本已由「查字」节承载的执行细节（`meaning` 口径）。
改法（行内子串替换）：`（\`meaning\` = 中文释义、≤24 字，见「查字」节）` → `（详见「查字」节）`。行内保守锚组（绝对禁工具 / 最小意图块 / `meaning` / 坐标 / 静态字库）全部保留——`meaning` 与「中文释义」仍在 JSON 字面里。

**（3）B1 · L14 课表问答回合行（核心）**
现文问题：仍强制「先读 `kidpal/schedule.md`」，并把「本回合实际读到的内容」定为唯一合法来源；P5 起 front 在建连时把 3 个日历日的课表摘要（时刻 / 名称 / 地点 / 备注逐字 + 在校节次名，≤2048 字节）经内部帧注入动态 context（`front_app.py:1436-1494`、`:1572-1588`）。模型守纪律 = 窗口内也先跑 read（多一轮延迟）；模型用摘要直答 = 违反自写纪律；read 失败时会说「查不到」而摘要就在眼前。
改后（整行替换）：
| 课表问答回合 | 凡提到课表、课程、上课、上什么课、几点上课、要带什么，以及任何「今天 / 明天 / 后天 / 大后天 / 周X / 星期X / 周几 / 这周 / 下周 /（某月某）号 + 课」的组合表述 | **先看动态 context 里的「课表摘要：」行**：摘要覆盖的今天 / 明天 / 后天，**直接用摘要作答**——它是网关从 `kidpal/schedule.md` 单源派生的真实课表，等同本回合读到的事实，**不必再读文件**；摘要没覆盖的日期（大后天 / 下周 / 某月某号）、摘要行显示「（本次会话未提供）」、或因长度截断缺了要问的事项时，**先读 `kidpal/schedule.md` 拿到真实课表再作答**（优先按 kidpal-schedule-answer skill 的流程执行）。两种情况都答复含备注读回（见「课表」节）。课表答案必须来自摘要或本回合实际读到的内容；都拿不到就诚实说现在查不到，**严禁凭印象或记忆回答课表具体内容**。语音部分只播最终面向孩子的答复——任何「这是课表问答回合」「需要先读 schedule.md」式的回合判定、文件路径、流程步骤、内部动作叙述都不得进入语音部分（内部动作与流程一律违规）。 |
说明：识别特征列逐字不动（K1/PV-A1 锚组全保）；降级句三触发（窗口外 / 占位「（本次会话未提供）」`connection.py:90` / 有界截断 `_fit_summary_to_budget` 整日丢或首日带「…」）必须同在，否则截断日会被当「没有安排」。

**（4）S1 · L15 提醒 cron 回合行（收编为兜底 + 单源指针）**
现文问题：该 cron 落在 **main** agent（`manifest.yaml:321-323` `expect_agent: main`；`workspace-main/kidpal-reminder-semantic/SKILL.md` 明说「本 skill 在 main agent 上执行；板侧 kidpal agent 与设备语音链路完全无涉」），本 agent 是死路径；且本文件缺 SKILL 的关键纪律（原子写 / 非携带物 speech 覆盖 / entry_key 拼法 / mid-flight 复算）——双源漂移。
改后（整行替换）：
| 提醒 cron 回合 | 系统定时任务触发（非孩子消息） | 正常情况本回合归 main agent、不会到本工作区；万一真的到达：按 `kidpal-reminder-semantic` 的流程从 `kidpal/schedule.md` 备注列产语义 JSON（`entry_key` 对齐脚本）；**不算时刻、不生成 id、不写 pending.json、不改三档配置**；不产 display 块、不面向孩子播报。 |
（K1 `K1_ROUTING_ROWS["提醒 cron 回合"]` 四锚组全保：备注 / 语义 / entry_key / 不算时刻。）

**（5）N5 · L17 边界行收一句指针**
改后：`边界：家长经 QQ 录入 / 修改课表走家庭侧 agent（main workspace），消息不会到达本 agent；本工作区的 \`kidpal/\` 与 \`stories/\` 只读，写入纪律见「底线」。`

**（6）N4 · L23 屏幕用词消歧**
行内子串替换：`翻屏由板子阅读面承载` → `翻屏由问答框滚动承载`（「阅读面」是 P3 页名，与共享底座问答框易混）。

**（7）B1 + S4 + S3-半句 · L41 课表节首段（整行替换）**
现文问题：①「先读 `kidpal/schedule.md`」与注入现实冲突；②「用 `text` view（或仅语音）」与 L24「每次回复都必须携带 display 块」及 S-F1.3 AC1「答复文字同步上屏」自相矛盾。
改后：
**课表问答**（答复走问答框）：孩子问课表问题时，答复与百科问答同一问答框，用 `text` view，**不切课表页、不滚动定位、不嵌课表页**。先看动态 context 的「课表摘要：」行：今天 / 明天 / 后天的安排直接用摘要作答，不必再读文件；大后天 / 下周 / 某月某号，或摘要行显示「（本次会话未提供）」、被截断缺行时，再读 `kidpal/schedule.md`：按目标星期筛「学校课程表」逐节列出（「第N节 课程名」），拼上「每周固定课表」「一次性安排」里目标日的行，过期的一次性安排忽略。**该事项的备注列内容也要答复/读回**（如「钢琴课，记得带琴谱」）；备注空 = 没有要带的，不发明。备注读回以孩子自己的日程为界——接送、携带、老师称呼这类当天安排照原词念出，不算复述家长隐私。

**（8）N2 · L52 删内部编号**
行内子串替换：删 `（notes ⑦）`。

**（9）B2 · L53 故事节撤旧管道（整行替换）**
现文问题：描述 261019 之前的「下一回合起标题 / 正文逐字转写」旧管道；现行链路 = front 一次性整理回合（`front_app.py:335-376`，整理 LLM 输出首行 = 标题 ≤12 字、其余行 = 正文；`faithful-organize.v1.md` 明定去口水词 / 断句 / 分段，**不是逐字转写**），arch §5.2④ 与 S-F2.2 AC4 同口径。风险：整理回合走 `_openclaw_chat_once`（`provider.py:32、95-108`）打的正是本 agent，只叠 request 级 system；若本文件仍被拼入装配层，「你只做一件事：起标题」会与整理指令竞争输出形状，可能归档出「正文 = 标题」。
改后：
- 孩子讲完（板子停录）后，整理与归档全由网关承载：**你不产标题、不产正文、不产 `id`**——标题（≤12 字）与正文由网关的整理回合产出，`story_text` 块正文 = 整理后的正文（去口水词、断句分段，**不是逐字转写**），你也不回写。

**（10）B2 · L55 回退四条 → 单条意图级（整行替换）**
现文问题：requirements v1.49 已撤「重录」（§8 已砍清单）；arch §5.2④ 明说「刚才那段是故事」挽回口径在记录语境对话态内不再需要；代码侧 `story.py` 的 `redo()` / `recover()` 仅存方法、front 零调用点。该条教模型执行两个现行链路不接的动作（模型会答「好，重来」但没有撤销/恢复发生 → 可产出重复入册）。
改后：
- **孩子想丢掉刚录的内容**（「讲错啦 / 不对不对 / 我重说 / 算了不讲了 / 取消」，或按取消键）：丢弃由板子的取消键和网关的丢弃链承载，**你只回一句简短安抚，不做撤销或恢复、不复述刚录的内容**；被主动取消的内容不会入册，已入册的故事不会被超窗撤销。

（「讲错啦」一词保留 = requirements S-F2.2 AC4 原词；其文本检出的对账缺口见 §5。）

**（11）S1 · L57–67 提醒节整节收编（标题 + 正文替换，-10 行）**
现文问题：整节以「你」为执行主体写提醒语义回合（读备注列、按 `entry_key` 写语义 JSON、判 bring），字段级细节与 main 侧 SKILL 双源且缺其关键纪律；L61 的 env `KIDPAL_SEMANTIC_JSON` 绑定在 main 侧 cron 命令串（`manifest.yaml:320`），本 agent 进程未必有（S8 随之消解）。
改后：
## 提醒（兜底口径；正常不会到本工作区）

三档时刻的算术、幂等 id、pending.json 的原子写、语义 JSON 的字段形状（title / speech / bring）、`entry_key` 拼法、携带物判定与文案口径，**单一事实源在 main agent 的 `kidpal-reminder-semantic` skill**。本工作区只留兜底一句：若系统定时任务真的到达本工作区，按该 skill 的流程从 `kidpal/schedule.md` 备注列产语义 JSON（`entry_key` 与脚本逐字对齐、原子写），**不算时刻、不生成 id、不写 pending.json、不改三档配置**，不产 display 块、不面向孩子播报；备注空 = 不发明要带的东西。

**（12）B1 · L72 硬约束同步**
改后：`- 先判回合再行动（见「回合类型与纪律路由」）：孩子对话回合禁工具、禁读写文件；课表问答先看动态 context 的课表摘要、摘要没覆盖再读真实课表；**课表答案永不凭印象编造**；查字回合绝对禁工具。`
（L78 的提醒 cron 硬约束保留不动，与 §2.1(4) 兜底口径一致。）

**（13）S3 · L82 对象锁定句（锚串之后追加，不删原串）**
现文问题：L82「不引用、不复述任何家长隐私（姓名、联系方式、**家庭安排细节**）」与 S-F1.3 AC4 备注读回、P5 备注逐字注入结构性冲突（备注可能是「周三奶奶接，记得带琴谱」）；「绝不称呼任何成人名字」也会撞上备注里的「王老师收」。
改后（整行替换；K3 冻结锚串逐字保留、只在其后追加）：
- **对象锁定**：对话对象永远是孩子，绝不称呼任何成人名字；不引用、不复述任何家长隐私（姓名、联系方式、家庭安排细节）——「家长隐私」指成人自身的身份与事务信息；课表 / 提醒备注里与孩子当天安排直接相关的接送、携带、老师称呼等内容，按课表口径原词读回，属于孩子自己的日程信息，不算复述家长隐私。

> **升级判定见 §6**：本条为全包唯一疑似触 spec 的条目，主线程落 plan 后须先派 architect-reviewer 四分类；其余条目无此前置。

**（14）B1 · L88 底线第一不变量**
改后：
  - **课表问题永不编造**——课表具体内容只来自动态 context 的课表摘要，或本回合实际读到的 `kidpal/schedule.md`（二者同源派生），都拿不到就诚实说查不到，严禁凭印象回答。

**（15）N6 · L92–94 自声明节去对偶词**
改后（正文行替换）：
本文件是**静态语义的单一事实源**：每次会话注入的只是**动态 context 块**（时间 / 日期、课表摘要等回合内信息，不含纪律）。两者冲突时，**以本文件为准**；动态 context 块只补上下文、不改纪律。
（「主承载」是「次要承载」时代的对偶词；ADR-014 ⑦ 已明确否决双件、本文件为唯一静态件。）

**（16）S5 · L96 记忆节标题去版本号与 rationale**
改后：`## 记忆守则：孩子世界的事实要记，家长隐私不记`
（删「（v12）」；L100 末尾「（旧守则的过度泛化由此解开）」同批删，见 (18)。）

**（17）B3 + S6 · L98 记忆节首段（补负向 + 声明平台自动承载）**
现文问题：§6.7 降级语义 = 记忆不可用则本轮无画像注入、对话照常；此时模型仍被要求「答得上来」，而底线「不编造」只覆盖实时信息——**没有一条禁止编造记忆**。另「主动记」+ 工具面有 write/exec，模型很可能用写文件「记」。
改后：
你有跨会话的长期记忆（记忆画像）：孩子说过的内容会被自动记住，隔天再问你要答得上来，像真正认识孩子的伙伴。记忆由平台自动承载、自动注入——**你不需要写任何文件、也不调用任何工具去记**；想不起来（画像里没有相关记忆）就诚实说「记不清了」，**绝不编造孩子说过的话，也不假装记得**。记什么、不记什么，边界只有两条：

**（18）S5 + S6 · L100 事实条（删 rationale、删「主动记」动作动词）**
改后：
- **孩子世界的事实必须记、记入记忆画像**：孩子讲述的自己世界里的事——喜好（最喜欢的颜色 / 食物 / 动画 / 游戏）、宠物（小狗小猫叫什么名字）、同学和朋友的真名、经历过的事（去了哪里、做了什么、跟谁一起）——都会被自动记进记忆画像。同学真名、宠物名字不是隐私，是孩子主动告诉你的重要事实，正是「记住孩子」的核心，不得以「家庭小私事」为由拒记。
（L101 家长隐私条**逐字不动**。）

### 2.2 SKILL.md（B1 同口径，4 处 + 2 处指针）

1. 第 3 行 description：
   `description: "孩子/板子问今天明天这周有什么课、几点上课时 (KidPal)：先看动态 context 的课表摘要，摘要没覆盖再读 kidpal/schedule.md，text view 走问答框答复（含备注读回）。"`
2. 第 8 行首段：
   板子/孩子问「今天/明天/这周有什么课」「几点上什么课」时：先看动态 context 的「课表摘要：」行——它由 front 从 `kidpal/schedule.md` 单源派生（窗口 = 今天 / 明天 / 后天），覆盖的日期直接用摘要作答、不必再读文件；摘要没覆盖的日期（大后天 / 下周 / 某月某号），或摘要行显示「（本次会话未提供）」、被截断时，读 `kidpal/schedule.md`（本 workspace 内、只读），按目标日的星期匹配，用 `text` view 走问答框答复（课表问答不切课表页）。
3. 第 10 行：
   **没有事实依据绝不凭印象/上下文残影列举课程**——没读到事实就作答，就是编造。这里的「上下文残影」= 模型自己的记忆或往轮印象，**不是**动态 context 的课表摘要：摘要行是 front 单源派生的真实课表，等同本回合读到的事实（2026-09-25 实证：凭印象答出课表里不存在的钢琴课、把周一有下午课答成没课）。system prompt v7 的回合路由表已显式许可课表问答回合读文件；2026-10-23 起改为摘要优先、摘要没覆盖再读文件（反编造覆盖句不变）。
4. 第 14 行步骤 1：
   1. **先判摘要覆盖：目标日在摘要窗口（今天 / 明天 / 后天）内就用摘要；窗口外再读 `kidpal/schedule.md`（本 workspace 内、只读；读了就别答「查不到」）。**
5. 第 15 行行首加「（窗口外路径）」限定：`   （窗口外路径）取星期别心算：今天 \`date '+%A'\`，…`
6. 第 40 行边界条末尾追加：`2026-10-23 起路由表改为摘要优先、窗口外读文件（\`261023-agents-md-static-semantics-alignment\`），本条读文件许可仅窗口外适用。`
（步骤 2/3/4 与「验证」节不动。）

## 3. 冻结锚保护清单（锚 → 保留方式）

> 判法均已在当前树上做**内存替换后重跑**验证（不改仓内文件）：六件读数件 101 passed / 4 deselected 全绿、零新增红（详见 §9/§10）。

| 冻结件 | 锚 | 本包保留方式 |
|---|---|---|
| K1 `test_261022_m5_k1_prompt_carrier_migration` | `K1_ROUTING_ROWS["课表问答回合"]`：先读 / schedule.md / 读不到\|查不到 / 诚实 / 全称量词 / 后天 / 大后天 / 备注 | 全部落在改写后的同一表格行：识别特征列逐字不动；「先读 `kidpal/schedule.md` 拿到真实课表再作答」保留在降级分支；「都拿不到就诚实说现在查不到」；「答复含备注读回」 |
| 同上 | 课表行时间面 ≥3/6、周X\|星期X 正则、主题面 ≥5/6（含「课程」）、反叙述类别 ≥2、语音半 + 禁止半 | 识别特征列与反叙述尾句**逐字未改**；改写只动「纪律」列的结果侧 |
| 同上 | `K1_ROUTING_ROWS["提醒 cron 回合"]`：备注 / 语义 / entry_key / 不算时刻 | 见 §2.1(4)：四组全在，字段级细节外移 |
| 同上 | 正文级 `display / 白名单 / story_list / story_text / 帮我记下来 / 问答框 / 薄载荷 / 机械计算` + 「禁产 story JSON」+「12 字」 | 零触碰：故事节只改「起标题管道行」与「回退行」，L51/L52/L54 原样 |
| 同上 | 自声明「静态语义」「单一事实源」「以本文件为准」 | §2.1(15) 三者全在（只删「主承载与」） |
| 同上 | K1-E6：verify-deploy.sh 零 AGENTS.md 特例、manifest 条目 verify=md5/owner=deploy.sh | 本包零触 manifest / verify 脚本 |
| K3 `test_261022_honcho_memory` | heading 正则 `^##\s+[^\n]*记忆` | 新标题「## 记忆守则：…」命中 |
| 同上 | 节内 needles：孩子世界的事实 / 宠物 / 同学 / 家长隐私 / 家庭安排 | 全在节内（宠物、同学在 L100；家庭安排在 L101 逐字不动；家长隐私在标题与 L101） |
| 同上 | 极性组：必须记\|要记\|照记\|记入\|主动记 × 不记\|不进\|不复述 | 保留「必须记」「都会被自动记进记忆画像」「不记」「不复述」；删掉的「照记、主动记」不构成极性缺失 |
| 同上 | `K3_P1_AGENTS_ANCHORS` 六串逐字（含 `不引用、不复述任何家长隐私（姓名、联系方式、家庭安排细节）`） | 六串全部零改；S3 只在锚串**之后**用「——」追加，锚串本体（含右括号）逐字在场 |
| 同上 | `assertNotIn("次要承载")` | 新文本零命中（且删掉的「主承载」不产生该词） |
| 同上 | 动态块负锚（孩子世界的事实 / 家长隐私 / 记忆画像 不得进动态块） | 动态 context 两文件零触碰 |
| M3K3 `test_m3k3_voice_schedule_entry` | `_assert_v6_schedule_anchors` 在 AGENTS.md 课表节窗口：问答框 / 不切 / 备注 / 答复\|读回 / 帮我显示课表 / 薄载荷\|{} / 不生成\|网关\|机械计算 | 课表节 §2.1(7) 与 L43 原样保留，全部命中 |
| 同上 | 旧 `{title,items}` 行级负锚 | 未引入任何含 schedule+title+items 的行 |
| v7 `test_prompt_v7_round_routing` | PN-A4 反叙述（先读 / schedule.md / 类别 ≥2 / 语音 / 禁止） | 行内保留（回合判定 / 文件路径 / 流程步骤 / 内部动作 四词在） |
| 同上 | PV-A3 扩列（全称量词 / 后天 / 大后天 / 周X / 时间面 ≥3 / 主题面 ≥5） | 识别特征列逐字不动 |
| 同上 | RF-A4（实时事实行 / 对话行 / 底线第三不变量） | 零触碰 |
| M4K3 `test_m4k3_record_session_live` | 故事节窗口：我要讲故事 / 帮我记下来 / 明天带跳绳 / story_list / 禁产 / **「12 字」字面** | 全保；新 L53 写「标题（≤12 字）」——**必须保留阿拉伯数字 + 空格**，禁写「十二字」 |
| perf-k1 `test_perf_k1_roundtrip_latency` | AGENTS.md 在场非空 + 现算 sha = 期望 sha | 内容级无锚，天然满足 |

**归档件说明（诚实标注）**：`server/acceptance/archive/test_story_p3.acceptance.py:910` 要求正文含「重录 / 讲错啦 / 刚才那段是故事 / 回退」任一——该件在 `archive/` 退役面（`server/pytest.ini` `norecursedirs` + 两闸 `SKIP_DIRS`），不构成现役红；本包保留「讲错啦」一词，其余两词退出。

## 4. 新增断言种子（3 条，落**新**验收件）

落 `server/acceptance/test_261023_agents_md_static_semantics_alignment.acceptance.py`。**禁改既有冻结件**——任何改锚都是 re-spec，触 human。

- **A1 · `test_a1_schedule_summary_first_with_fallback_same_wording`**（钉 B1 新口径 + 两文件同口径）
  断言：① AGENTS.md 含「课表问答回合」的行同时含「课表摘要」与「不必再读文件」；② 同一行含占位降级词「（本次会话未提供）」与「截断」；③ AGENTS.md 全文**零**「课表答案必须来自本回合实际读到的内容」（旧唯一来源句已撤）；④ 底线节窗口含「课表摘要」；⑤ SKILL.md 含「课表摘要」+「覆盖的日期直接用摘要作答」+「本次会话未提供」；⑥ SKILL.md 零「先读文件再作答，绝不凭印象」。
- **A2 · `test_a2_story_organize_pipeline_no_legacy_residue`**（钉 B2 旧管道撤净）
  断言：故事节窗口（标题含「故事」的 `## ` 节 + 含 `story` 的行）内**零命中**「下一个回合」「重录一次」「刚才那段是故事」「逐字来自整段转写」；同窗口**在场**「你不产标题」「整理」「不是逐字转写」「12 字」。
- **A3 · `test_a3_reminder_single_source_and_memory_negatives`**（钉 S1 单源 + B3/S6 负向 + S3 澄清 + S5 去残留）
  断言：① 提醒节窗口含 `kidpal-reminder-semantic`，全文零 `KIDPAL_SEMANTIC_JSON`，节内零「≤60字」「≤90字」（字段级细节已外移）；② 记忆节含「不需要写任何文件」「不调用任何工具」「记不清」「绝不编造」；③ 全文零「（v12）」、零「旧守则」；④ 「对象锁定」行同时含 K3 锚串逐字 + 「孩子自己的日程信息」+「不算复述家长隐私」。

> 注：A3 第 ④ 半即 S3 的机器可判面——若 §6 的升级判定落到「乙案」（S3 拆出），该半同步删除，A3 退为三组断言。

## 5. B2 对账项：口语「讲错啦」零文本检出（如实标注，不产生本包修复）

- **离线已判事实**（architect 2026-10-23 实测，builder 需复核并留证）：`server/gateway/front/front_app.py`、`server/gateway/story.py`、`firmware/main/**` 对「讲错啦 / 不对不对 / 重说」**零文本检出**；丢弃链的真实载体 = 设备 BOOT/按键 → 契约 §四 `kidpal_story_discard`（`protocol.cc:116-132`、`mcp_server.cc:751`）→ front `acc.cancel`（`261021-p3-record-cancel-lifecycle`）。记录语境下引擎不派发 LLM，故「讲错啦」作为**语音**输入在当前链路无落点。
- **对 spec 的影响**：requirements S-F2.2 AC4 原文含「或说「讲错啦」，Then 取消」——**这是 spec↔实现的既有缺口**，不是本包引入，也不能由 prompt 修复。
- **本包处置**：AGENTS.md 保留「讲错啦」原词（与 AC4 对齐）但把机制归属改为「丢弃由板子的取消键和网关的丢弃链承载」，**不承诺模型执行动作**（见 §2.1(10)）。
- **对账形态判定 = builder 静态对账**（离线可判，不上真机）：builder 以 grep/读码复核上述零检出 + 丢弃链三处，把结论与 file:line 落 review-packet「对账」节；并**如实申报**为 debt 候选（建议 `tech` 域，标题指向「S-F2.2 AC4 语音丢弃路径无文本检出载体」），**登记动作归主线程**（architect 不登记）。live 不再单列该事实（离线可判，重复测同一事实无增信息）。

## 6. 升级判定（S3 · 隐私句追加）

**结论：疑似触 spec（存疑向严 → 标逃生门）。** 理由三条：

1. `docs/requirements.md:654`（NFR-PRIV-1 ①）的字面是「家长隐私信息（姓名、联系方式、**家庭安排**）不进记忆画像、**不被复述**」——「不被复述」未限定在记忆回忆语境，按字面可覆盖接送类备注。
2. 同时 `docs/requirements.md:289` / S-F1.3 AC4 要求「备注列参与语音答复（带物可语音读回）」，P5 起备注**逐字**注入摘要——两者在「接送 / 出行类备注」上字面冲突，是 spec 内部张力，不是措辞问题。
3. S3 句在效果上是**对 spec 术语「家庭安排」的范围裁定**（把孩子自身日程信息划出隐私范畴），属解释性判定，超出 prompt 侧纯措辞级。

**处置（两案，默认甲案，不阻塞其余条目）**：
- **甲案（推荐）**：主线程落 plan 后先派 `architect-reviewer` 做四分类（本包对契约面零触，预期 `NO_CONTRACT_CHANGE`）；判非触契约 → S3 按 §2.1(13) 原句随本包落地；判 `SPEC_CONTRACT_DRIFT` 类 → 按乙案。同时建议在主线程侧记一条 `requirement` 域 debt 候选（不在本包登记），口径建议写成将来 spec 侧一句话：「课表 / 提醒备注中与孩子当天安排相关的接送、携带、老师称呼，按 S-F1.3 AC4 原词读回，不属 NFR-PRIV-1 ① 的『家长隐私』」。
- **乙案（保守）**：S3 整条拆出本包（等 reviewer / human 裁决后另批），本包只落 B3 / S5 / S6（记忆节三条）——A3 断言去掉第 ④ 半。
- **附带风险（评审与 live 轮须知）**：K3 冻结的 live 口径 = seeded 负向探针集 **100% 零命中**（S-F5.1 AC3 / NFR-PRIV-1 验证）。若探针句与 schedule 备注中的家庭安排串重合，S3 的「原词读回」可能被误判为命中——建议主线程在 human live 轮注明「探针按记忆侧口径判（问『我们家谁接我』应答不知），课表回合的备注读回不计入负向命中」，并核对探针语料与真 schedule.md 备注无同串。

**human 2026-10-23 触点①裁决：甲案**（②单包 Lite 同时确认）。

**四分类裁决记录（2026-10-23，architect-reviewer，主线程 append 留痕）**：verdict = **`NO_CONTRACT_CHANGE`**（`doNotEscalateToHuman: true`）——S3 落点为实现面 prompt 资产（`docs/contracts/**` 零触），行为效果（备注原词读回）在契约面已被现行契约完整表达（`interface-contract.md:82`「备注不进网格载荷…备注露面处 = 语音答复 S-F1.3 AC4」/ `:240` 话术读回 / `:242` 备注列承载）；spec 两句张力属 requirements **内部**既有张力，不在契约四分类管辖面。**S3 不必拆出（甲案成立，乙案不必要），A3 第 ④ 半按原样落。** builder 边界两句：① K3 冻结锚串**含右括号逐字保留**（`test_261022_honcho_memory.acceptance.py:229` 子串断言依赖），只在其后用「——」追加；② 追加句许可范围**以课表 / 提醒备注原文所载为限**（原词读回 ≠ 转述/扩展许可；备注以外的家庭安排内容仍受锚串禁止）。残留记账：requirement 域 debt `REQ-261023-family-arrangement-scope-clarification` 已由主线程登记（spec 术语澄清将来走 `/requirement-change` 一句话，不阻塞本包）；live 判读注记（seeded 探针按答案来源判、探针语料与真备注无同串核对）落 human 部署轮 checklist。

## 7. 铁律

1. **AGENTS.md md5 已知窗口纪律**：AGENTS.md 与 kidpal 侧 SKILL.md 在 `manifest.yaml` 均为 `verify: md5` 条目。仓内镜像改动后、live 铺放前，`verify-deploy.sh` 报 md5 漂移属**预期已知窗口**（K1-E6 判红豁免在案）；**不得**为此弱化脚本或给单文件加特例（K1-E6 负向断言会红）。铺放 = merge 后在 human 部署轮由 `deploy.sh` **同批**铺放两份（AGENTS.md + SKILL.md），铺放后两 md5 转绿。严禁直接在 ECS 改 live（README 同步纪律：先改仓内镜像 → review/合并 → 再同步）。
2. **SKILL.md 改动必须同批**：B1 是 prompt↔skill 双件同口径，只改一边 = 制造新的双源矛盾（skill 的「先读文件再作答」会与 AGENTS.md 的「摘要优先」打架）。
3. **零触契约与零触承载面**：`docs/contracts/**` 零写（Bash 重定向写恒拦）；`kidpal-agent-prompt.txt` / `kidpal-dynamic-context.v1.txt` / `openclaw.json` 声明 / `manifest.yaml` / `verify-deploy.sh` 零改（K1-E2/E3/E6、K3 动态块负锚在此）。
4. **不新增/不改既有冻结验收件**：新断言只落新件；既有 6 件读数件与 K1/K3/M3K3/M4K3/perf 锚**零改**。
5. **不改行尾**：两文件 index 侧为 LF（autocrlf 检出为 CRLF），编辑后由 git 归一即可；不得手改 `.gitattributes`。
6. **行号漂移提示**：提醒节压缩使全文 -8 行；`docs/debt/tech/TECH-261023-...md` 里引用的「L17/L85」在合并后分别落到 L17 / L77（指向仍成立，主线程可顺手校正或按指路理解）。

## 8. 判档与 riskTier 理由

- **②单包 Lite**：单一来源（一份 advisory 的批 A+B）+ 紧耦合小簇（两文件同口径）+ 单 owning 包 + 无悬挂契约决策（S3 走 §6 前置）+ 无迁移删数 + 一轮可收。
- 非 ③拆包：单 Pod 单切片，无跨系统结构性。
- 非 ②Full：不触 security 轴（凭据面 `261017-openclaw-kidpal-skills` 零触）、无真 DB、无状态机、无 AI 成本审计；prompt 体积净减（102 → 94 行）。
- 非 ⓪nano：多条独立语义修订 + **新增验收语义**（3 条断言种子），排除清单命中。
- **riskTier = low**：纯文本面、可回滚（一 commit revert）、影响单位是模型行为而非数据/接口；风险由 live seeded 探针与真机轮兜底，非本包自动断言对象。
- **lite 判注（义务句）**：「host 齿可闭环」——全部验证在 `server` pytest host 面闭环（六件读数冻结件 + 全量 acceptance + 新断言件），**无设备编译面、无 ECS 依赖**；lite 快路径适用（launch 面环境后置、verify 面 firmware-build 相位本包不涉）。

## 9. demo 证据形态（builder / verifier 交付用）

1. `git diff --stat` + 两份文件全文 diff（review-packet 引，逐条对上 §2 编号）。
2. 六件读数冻结件：
   `cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance/test_261022_m5_k1_prompt_carrier_migration.acceptance.py acceptance/test_m3k3_voice_schedule_entry.acceptance.py acceptance/test_261022_honcho_memory.acceptance.py acceptance/test_prompt_v7_round_routing.acceptance.py acceptance/test_m4k3_record_session_live.acceptance.py acceptance/test_perf_k1_roundtrip_latency.acceptance.py`
   → 期望 **101 passed / 4 deselected**（基线实测值）。
3. 全量：`python -m pytest -q --import-mode=importlib -m "not live" acceptance` → 期望 **≥313 passed / 6 deselected / 33 subtests passed**（基线实测值，不得降）。
4. 新验收件红→绿：落件时先出设计态红（3 条，见 §10），文本落定后转绿；证据 = 两次运行的命令 + 输出。
5. §3 锚对账表逐条勾（review-packet 表，附验证方式）。
6. 部署（human 轮，非本包）：`deploy.sh` 同批铺放两件 + 铺放前后 `verify-deploy.sh` 两态输出（窗口漂移 → 转绿）。

## 10. 红基线预期

- **新断言设计态红 = 3/3**（已在现树实测：A1/A2/A3 三条全红——旧文本无「摘要优先」「你不产标题」「不写文件/记不清」，且「重录一次」等旧句在场）。
- **既有冻结件零新增红 判法**（三条同时成立才算达成）：
  ① 六件读数件 101 passed / 4 deselected，与基线逐件一致；
  ② 全量 acceptance 313 passed / 6 deselected / 33 subtests，零 failure；
  ③ `git diff` 不含任何 `server/acceptance/` 既有文件与 `server/deploy/{manifest.yaml,verify-deploy.sh,provider.config.yaml,OPENCLAW_SYSTEM_PROMPT_CARRIER}`。
- 唯一允许的红 = 新验收件自身的 3 条（设计态红 → builder 落文本后转绿）。

## 11. 预期改动面（1g 技术预评的 changeManifest 种子）

- 改：`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`、`server/deploy/openclaw-skills/workspace-kidpal/kidpal-schedule-answer/SKILL.md`
- 新增：`server/acceptance/test_261023_agents_md_static_semantics_alignment.acceptance.py`
- 收尾制品：`delivery/taskpackages/261023-agents-md-static-semantics-alignment/**`（review-packet / verify 证据）
- 禁入清单（越界即 change-manifest 闸红）：`docs/contracts/**`、`server/deploy/manifest.yaml`、`server/deploy/verify-deploy.sh`、`server/deploy/kidpal-agent-prompt.txt`、`server/deploy/openclaw-agents/kidpal-dynamic-context.v1.txt`、`server/gateway/**`、`firmware/**`、任何既有 `server/acceptance/test_2610*/` 与 `test_m*/` 件。
