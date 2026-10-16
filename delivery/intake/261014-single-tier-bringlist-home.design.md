# 立项设计稿 · 261014-single-tier-bringlist-home

> **状态：DRAFT v0.1 · 2026-10-14 · 待 human 立项 ack + OQ-2/3/4 裁定**
> **角色**：spine（`delivery/changes/intake/261014-single-tier-bringlist-home.md`）的立项对齐 companion——承载「为什么改 / 往哪改 / 改动多大」与 T1–T4 设计；下游各面流程（requirement-change 档 2 / 契约门 / dev-loop）以本稿为设计输入。**本稿不设 FROZEN-landing**（无独立评审批，2026-10-14 human 裁，见 OQ-1）——设计对抗审由 requirement-change 档 2 的 architect 必派承担，各面文本由各面门冻结，spine anchors 对账。
> **routing 裁定记录（原 OQ-1，2026-10-14 human 裁）**：触契约面命中升级判据①，human 裁「降为立项稿、不建独立评审批」——spine 分面推进，各面门照旧；本稿从 Track B PROPOSAL 降格重写（治理 machinery 字段——packet-binding / riskTier / review profile / landing-manifest——随之撤除）。

---

## 0. 想实现的效果与差距

**目标效果一句话**：设备提醒只保留「事项开始前 15 分钟」单档，校内时段（08:00–18:00，周中）静默；「上学要带的东西」升为按日期维护的带物清单一等公民，由 OpenClaw 从家长转发的老师消息中拆出、AI 落笔精简；首页「接下来」与「明天」两区的「上学」行后面直接跟当天清单文字（如「08:00 上学 · 带跳绳、水壶」），校内一次性事项折叠进「上学」项不再单列——家长与孩子**一瞥**首页即知「接下来干什么、要准备什么」，不再依赖到点语音。

**现状差距**：

1. 提醒为三档阶梯（前一天 20:30 / 当天 07:45 / 事项前 15 分钟，S-F1.4 补充规则 docs/requirements.md:303），其中前晚档与早上档真机使用无效（human 2026-10-14：「我们早上 7 点 45 的通知，包括晚上睡前的通知，并没有起到什么作用」）；
2. 携带物信息依附于课表行备注列、只经三档提醒语音露出——提醒砍单档后信息将无处安放；且老师消息的带物经常**不依附任何事件**（「明天带跳绳」），备注列承载不了独立于事件的清单；
3. 校内一次性事项（如体测）的时刻是录入默认值（08:00），家庭在校内时段既不可知也不可控，逐条列出是假精度（human 2026-10-14：「在学校内怎么安排我们是不知道的，也控制不了，所以它分为几点几点，对我们来说没有意义」）；
4. 首页载荷逐项仅 `{time,name}`（schedule.py:343 group_upcoming 返回形状；契约 §1.3⑥ / 板端 seam home_p1.h 同形），无处挂清单文字。

**非目标（本批不做）**：

- 老师消息中的**作业任务与知识陪读内容**（2026-10-13 讨论的 C/D 类）不入本批——等本批落地后按真实使用频率另立变更；
- 「学校课程表」区块（B1 三区块之一）不动，课表页周网格（S-F1.2，week_grid_payload 独立代码路径）不动——折叠只作用于首页「接下来/明天」分组与提醒排程；
- 语音问课表（S-F1.3）不动——它读 schedule.md 原文作答，不受折叠影响；
- 提醒投递机制（pending.json → AlertDispatcher → kidpal/alert、重试到确认、幂等）全部不动——本批只改**排程侧产什么**，不改**投递侧怎么送**；
- 带物清单不做逐项勾选/完成态（MVP 纯展示）。

## 2. 设计与改法（T1…T4）

**起草期诚实注记**：本节 file:line 引据为主线程起草会话（2026-10-14）实读核对的当场快照；`reminder_schedule_lib.py` 的三槽生成内循环**未逐行读**，其行为断言以三处旁证（openclaw-reminder.config.yaml:18-20 三键、architecture.md:196「三档算术」、kidpal-schedule-write SKILL.md 步骤 4 期望输出「planned N alert slots」）钉住——T1 承载包 builder 动工前须先实读确认，机检断言随 T1 测试改写一并落地。

### T1 · 提醒单档化（三档 → 事前 15 分钟单档）

**问题**：三档阶梯中前晚档（20:30）与早上档（07:45）对真实家庭节奏无效——提醒发出时孩子未在设备旁、家长已形成「忽略」习惯；三档并存还造成上学时刻事件的档②③同分钟撞车（07:45 双弹，设计推演，未真机取证——builder 实测确认后随测试钉死）。

**证据**：openclaw-reminder.config.yaml:18-20（`night_at: "20:30"` / `morning_at: "07:45"` / `before_min: 15` 三键）；S-F1.4 补充规则（docs/requirements.md:303「三档时刻 = 前一天 20:30 / 当天 07:45 / 事项开始前 15 分钟」）；human 2026-10-14 反馈原话（spine sources 逐字收录）。

**根因**：三档设计（决策 20）假设「到点语音推送」是携带物信息的主要送达面；实际家庭的主送达面是**出门前看一眼设备**（常显首页），到点推送只对「人在设备旁且临近开始」的场景有效。

**改法**：配置三键收敛为单键 `before_min: 15`（`night_at`/`morning_at` 删除）；`reminder_schedule_lib.py` 排程循环从每事项产三槽改为产单槽（幂等 id 派生规则保持「schedule 条目 + 档位 + 事件日期」形态，档位域收缩为单值——id 稳定性语义不变，跨重算复用）。需求面 S-F1.4 同步修订（requirement-change 档 2）：补充规则「三段阶梯」改单档口径、AC1 Given 例改锚（原例「When 今天 20:30 到点」改「When 事项开始前 15 分钟到点」）；S-F1.4 补充规则中「弹窗驻留不设自动消失」的裁断理由引用了前晚档过夜场景（v1.44），修订时随档位语义一并复查措辞（裁断本身——驻留到按键——不变）。语义侧零改动：kidpal-reminder-semantic skill 的 speech 覆盖自设计起即档位无关（SKILL.md「禁用今天/明天」纪律），单档化后照常成立。architecture.md §5.1 组件行（:196「三档算术 20:30/07:45/前 15min」）与 §5.2 场景⑤（:216「重算三档」）镜像改单档措辞，随需求面同批。

目标文件：server/deploy/openclaw-reminder.config.yaml（改）· server/deploy/openclaw-patch/reminder_schedule_lib.py（改）· server/tests/test_reminder_units.py（改）· docs/requirements.md（改，经 requirement-change 档 2）· docs/architecture.md（改，镜像两处）

### T2 · 校内时段静默窗口（08:00–18:00 周中一次性事项不提醒）

**问题**：家长转述的校内事项（体测、校内活动）无真实时刻，录入被迫填默认值；其「前 15 分钟」提醒触发时孩子在学校、无任何行动能力——纯噪音。human 已裁这类事项**不提醒**（2026-10-14：「卷进去的就不用提醒了」）。

**证据**：openclaw-schedule.config.yaml:13（`school_start: "08:00"`——校内时刻默认值机制的现成先例）；S-F1.4 补充规则范围口径（docs/requirements.md:303「学校课程表区块不提醒（在校例行）」——静默窗口是该口径的自然延伸）。

**根因**：提醒范围口径目前按「区块」（学校课程表不提醒）划分，而校内临时事项落「一次性安排」区块——区块口径接不住「校内时段」这个时间维度。

**改法**：`openclaw-schedule.config.yaml` 增 `school_end: "18:00"`（human 2026-10-14 口头定值，OQ-3 随批确认）；`reminder_schedule_lib.py` 排程循环增静默判定——周中（周一~周五）且 start < school_end 的一次性事项**不产提醒槽**（每周固定课外班不受影响——它们天然在放学后；判定只按开始时刻单值比较，无时段重叠逻辑）。需求面 S-F1.4 补充规则范围口径同步扩写（「校内时段 08:00–18:00 的一次性安排不提醒」）。此改与 T1 同文件同循环，一个 dev 包承载。

目标文件：server/deploy/openclaw-schedule.config.yaml（改）· server/deploy/openclaw-patch/reminder_schedule_lib.py（改）· docs/requirements.md（改，经 requirement-change 档 2）

### T3 · 带物清单一等公民（新数据面 + OpenClaw 录入纪律）

**问题**：「上学要带的东西」目前在课表行备注列里，语义上依附事件；老师消息的带物经常无事件可挂（「明天带跳绳」），备注格也承载不了多物品清单。human 已裁把它升为独立数据面（2026-10-14：「有一个就是上学需要带的东西，这可以再仔细讨论一下」→ 讨论收敛为带物清单模型）。

**证据**：AGENTS.md 家庭面纪律（server/deploy/openclaw-skills/workspace-main/AGENTS.md:240「要带的东西进备注列」——现行录入纪律）；kidpal-reminder-semantic SKILL.md 步骤 4（语义回合从备注判 bring——现管道中带物判定是备注的派生）；front 读课表经 `/opt/kidpal/schedule.md` 软链单源（kidpal-schedule-write SKILL.md 开头），同型软链机制可复用。

**根因**：数据模型缺一个「按日期组织的、独立于事件的上学期物清单」承载。

**改法**：新增 `kidpal/bringlist.md`（OpenClaw main workspace，`/opt/kidpal/bringlist.md` 软链同 schedule.md 先例），格式 = 两列表格逐字表头 `| 日期 | 带什么 |`，日期纯 `YYYY-MM-DD`（严禁星期后缀——B1 日期事故同款纪律）、一天一行、**清单文字由 OpenClaw 落笔时精简**（human 2026-10-14：「让 AI 直接把它精简」——精简质量在写侧保证，front/板端零截断逻辑），过期行 OpenClaw 录入时顺带清理。录入纪律写进 AGENTS.md 家庭面段 + kidpal-schedule-write skill 扩一步（家长消息拆两处落：事件 → schedule.md 一次性安排照旧；带物 → bringlist.md 当日行合并去重）。front 侧经 `KIDPAL_BRINGLIST_MD` env 读文件（kidpal-front.service Environment 增一行），机械解析取当日行——**零判定逻辑进 wire**：清单文字原样透传，缺文件/当日无行/解析失败一律降级为无清单（不告警不阻塞，对齐 reminders.py 缺文件安静空转先例）。manifest.yaml 增该消费位登记。

目标文件：server/deploy/openclaw-skills/workspace-main/AGENTS.md（改）· server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md（改）· server/gateway/front/front_app.py（改）· server/deploy/manifest.yaml（改）· server/deploy/kidpal-front.service（改）

### T4 · 首页「上学」行挂清单 + 校内事项折叠（契约 additive + 板端渲染）

**问题**：首页两区载荷逐项 `{time,name}` 无处挂清单文字；校内一次性事项按时刻单列是假精度，应折叠进「上学」项。

**证据**：schedule.py:289-344（group_upcoming 单源：周中学校课合并「上学」项 + 培训班/一次性逐项，返回 {time,name,location}）；S-F1.1 AC2 与补充规则（docs/requirements.md:227/:235——分组口径的需求权威）；契约 §1.3⑥ home view data.upcoming 逐项 {time,name}（interface-contract.md，K1 裁决 HH:MM 字面入 wire）；板端 seam home_p1.h:75-93（preview 多项访问族，2026-10-11 多项化时板端同形改的先例）；HP1_PREVIEW_MAX = 8（home_p1.h:22，屏容量上限先例）。

**根因**：分组口径（决策 26）把「一次性安排」一律逐项列出，未区分校内/校外；载荷形状没有清单承载位。

**改法**（三面同批，契约面走契约门）：

- **契约面**：§1.3⑥ `data.upcoming` 逐项增 **additive 可选字段**承载上学行清单文字（字段名与形状——单字符串 vs brings 数组——由契约 packet 定稿，建议单字符串：与「AI 落笔精简、原样透传」模型一致、板端渲染零拼装）；§三 首句「按课表算三档时刻」改单档口径措辞（T1 契约镜像）。板端对无该字段的旧载荷照常渲染（additive 向后兼容），板端缺字段时上学行不显示清单（降级无害）。
- **front 面**：`group_upcoming` 增折叠判定——周中 start < school_end 的一次性事项不单列，其存在使当日「上学」项成立（与学校课合并同款）；「上学」项载荷带清单字段 = front 从 bringlist.md 取当日行透传。分组口径修订属决策 26 范围，需求面 S-F1.1 AC2/补充规则同步（requirement-change 档 2 同批）。「明天」区 preview 同款处理（同一 group_upcoming 单源，天然一致）。
- **固件面**：home_p1 接线层（home_upcoming.c）解析新可选字段并注入 seam；home_p1.c 渲染清单文字于「上学」行下方（字号小一档、占位不破布局优先级——接下来 > 明天区 > 小伴演出，S-F1.1 补充规则）；屏预算按 human 给的口径（每区 ≥3 行、一行十几个字，2026-10-14）——清单超长时板端整行容纳守卫截尾（261013-p1-tomorrow-row-sliver-fix 的两区整行容纳守卫先例），**写侧精简为主、板端守卫为底**。板端对旧载荷（无字段）过渡容忍：字段缺省 = 现行为不变。

需求面修订交 story-author 起草时附本 T 为设计输入；UI 批次（首页版式加清单行）触 uiux-author 出图 + human 看图拍板（档 2 流程既定环节）；docs/ui V5 母版明天区/接下来区示例行同步归该环节（v1.61 先例）。

目标文件：server/gateway/schedule.py（改）· docs/contracts/interface-contract.md（改，契约门 packet）· firmware/main/display/home_p1.h（改）· firmware/main/display/home_p1.c（改）· firmware/main/display/home_upcoming.c（改）· firmware/main/test/acceptance/（改，条件触发——首页验收齿随新字段扩展，具体文件 builder 定）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 需求 | docs/requirements.md | G2 | S-F1.4 补充规则/AC1 + S-F1.1 AC2/补充规则 + S-F1.5 录入面；requirement-change 档 2 承载 |
| 架构 | docs/architecture.md | G3 | §5.1 :196 组件行 + §5.2 :216 场景⑤ 镜像措辞；场景① 刷新三触发口径复查 |
| 契约 | docs/contracts/interface-contract.md | 契约门 | §1.3⑥ additive 字段 + §三 单档措辞；architect-reviewer 四分类 + human 签 |
| 实现·服务端 | openclaw-reminder.config.yaml / openclaw-schedule.config.yaml / reminder_schedule_lib.py / schedule.py / front_app.py | G3 | 排程单档化 + 静默窗口 + 折叠拼装 + bringlist 读取 |
| 实现·OpenClaw 面 | AGENTS.md / kidpal-schedule-write SKILL.md | G3 | 录入纪律扩带物清单（创作面，非代码） |
| 实现·固件 | home_p1.h / home_p1.c / home_upcoming.c | G2 | additive 字段解析 + 清单行渲染 + 旧载荷容忍 |
| 部署 | manifest.yaml / kidpal-front.service | G3 | 新消费位登记 + env |
| 消费方·语义回合 | kidpal-reminder-semantic SKILL.md | 未触 | speech 档位无关设计，单档化零改动（SKILL.md:45-47 纪律已核） |
| 消费方·课表问答 | kidpal-schedule-answer SKILL.md | 未触 | 读 schedule.md 原文，折叠/清单不影响 |
| 消费方·周网格 | week_grid_payload（schedule.py） | 未触 | 独立代码路径，折叠只作用 group_upcoming |

## 4. human 取舍点（OQ）

**OQ-1（已裁 · 2026-10-14）· 路由**：human 裁「降为立项稿、不建独立评审批」——spine 分面推进、各面门照旧，本稿据此从 Track B PROPOSAL 降格重写。

**OQ-2 · 带物清单要不要早晨语音兜底**：(a) **纯首页展示、零提醒**（推荐——清单天天在首页挂着，出门前必然可见；真发现漏看再加，那是一行配置的事）；(b) school_start 前 15 分钟发一条合并语音（「今天上学要带：跳绳、水壶」——当天清单非空才响，等于旧早上档以「有带物才响」的新形态复活）。

**OQ-3 · 校内窗口边界确认**：(a) **school_end: "18:00"**（human 2026-10-14 口头定值，随批进配置即可调）；(b) 其他值（请直接给）。

**OQ-4 · 被折叠的校内事项是否完全不上屏**：(a) **完全折叠**（推荐——「上学」行 + 清单已承载全部家庭可行动信息，事项名如「体测」对家长无行动意义；schedule.md 照常留档、语音问答照常可查）；(b) 折叠时刻但保留事项名清单（如「上学 · 体测：带跳绳」——信息多一点，但清单文字混入事件名，AI 精简纪律变复杂）。

## 5. 验证集草案（供各面承载包消费，非独立批 landing 门）

1. `python -m pytest server/tests -q`——全绿；含 T1/T2 新增断言：单档排程（每事项恰一槽、fire_at = start − 15min）、静默窗口（周中 start < school_end 一次性事项零槽、课外班不受影响）、group_upcoming 折叠（校内一次性事项并入上学项、校外照旧单列）。
2. bringlist 解析 smoke（归入 server/tests）——样例文件当日行命中 / 缺文件 / 坏表头 / 过期行四分支全绿。
3. 固件 host 测试按仓内既有入口跑（入口与基线 builder 开工时钉——261012-p3 收口记录 host 全绿基线在册）——含新字段解析与旧载荷容忍两齿。
4. `node .claude/scripts/spec-anchor-check.mjs`——需求/架构修订后锚检绿。
5. `bash -n` 三部署脚本 + manifest yaml-ok（部署面回归）。
6. verify-deploy.sh（post-deploy）——消费位/env/软链三件在场。
7. 真机轮（post-deploy，殿后）：QQ 转发一条真实老师消息 → 首页上学行清单可见 → 校内事项零提醒 → 校外事项前 15 分钟单提醒。

## 6. 留根清单

- 作业任务与知识陪读内容（C/D 类）的录入与承载——本批非目标，落地后按使用频率另立变更（2026-10-13 讨论开放问题）。
- semantic.json 的 bring 判定与带物清单的关系——本批清单由 OpenClaw 直写 bringlist.md，语义回合继续服务提醒 speech；两路并存，合并与否留 retro 观察。
- 「上学」合并项的在校时段 end 推断逻辑（schedule.py:326-332）在折叠后的语义——折叠只增候选不删合并逻辑，但「进行中」窗口与静默窗口的交互值得 retro 看一眼。
- 档②③同分钟撞车（08:00 事项 07:45 双弹）随单档化自然消失，不再单独立项。
