# PROPOSAL · 261025-spec-reconciliation-memory-writing-scope

> **状态：FROZEN v1.0 · 2026-10-26 · 主线程凭「立项 ack + architect-reviewer GREEN（§7）」（Track B；architect 起草、architect-reviewer 评审、零否回）**
> **承载来源**：spine `delivery/intake/261025-spec-reconciliation-memory-writing-scope.md`（跨面 intake 路由账；需求面 anchor 已回填 = v1.68 frozen）；消费 `docs/debt/tech/_archive/2026/TECH-261025-dreaming-capture-pollutes-honcho.md`（closed，consumed-by 本 spine）；引用不改 `docs/debt/tech/TECH-261026-oneshot-wrapper-prompt-derived-facts.md`（deferred）。
> **review 轨**：Track B 常驻轻路径的**主路径**（自评判主，判档归 architect-reviewer、只升不降，见「判档自评」）。Track A 不触发——方向已收敛（human 2026-10-25 建 spine 时定「把 spec 改成已验证实况」；需求面 v1.68 已按内容归属 frozen），无跨系统结构性变更、不触契约。
> **packet-binding（FROZEN 生效）**：packet-id = 本 PROPOSAL §2 改法集（T1–T9）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（纯文档面，零实现、零部署、零数据、零契约；但改的是隐私边界的主表述与类级规则定义，误写会误导下游实现与验收——故不取 low，交独立评审 + landing-verifier 逐字核）。
> **review profile**：架构面（`docs/architecture.md`）+ 决策记录注记（`docs/decisions/ADR.md`）+ 术语表（`docs/glossary.md`）；G1 / G2 / G3 零触；契约面零触。
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行在场（GREEN 前必须，FROZEN 期限；主线程标 FROZEN 前人工复核）。

```landing-manifest
docs/architecture.md
docs/decisions/ADR.md
docs/glossary.md
```

> **落盘路径与目录名注记（派发路径的机械偏离，已实证）**：派发指定路径为 `delivery/review/261025-spec-reconciliation-memory-writing-scope/PROPOSAL.md`，该目录名**不含 `arch-review` 写槽 token**，起草期实测被 `enforce-scope.mjs` 拦截（提示原文：`scope: agent "architect" 只能写 … delivery/review/*arch-review-*/**, delivery/review/*gov-review-*/**；delivery/review/261025-spec-reconciliation-memory-writing-scope/PROPOSAL.md 超出边界。`）。按 `.claude/references/arch-review-protocol.md`「双轨路由」命名条款（缺 token 会连带拦评审记录与 verdict 写入），本件落盘于合规目录 `delivery/review/261025-arch-review-spec-reconciliation-memory-writing-scope/PROPOSAL.md`。**changeId 仍是 spine 的 `261025-spec-reconciliation-memory-writing-scope`**（spine `anchors.arch` 与 §12 变更记录行引上述合规目录）；arch 批目录名与上游 spine id 不同名在本仓有先例（261023 批对 261022 spine），可追溯性由 slug 承接。请主线程回填 spine anchors 时按合规目录记。

---

## 判档自评（轻 / 主）

| 轻路径判据（SSOT = `.claude/references/arch-change.md`「轻路径判据」） | 判定 | 依据 |
|---|---|---|
| ① 零语义变化 | **不满足** | 记忆写入边界由「两把配置键逐行枚举」换成「类级规则 + 按内容归属的三类划分」，并新增 dreaming 实例处置、八条残余与未闭合申报——边界语义发生实质换代 |
| ② 改动性质限于错字 / 排版 / 表述澄清 / 润色 / 锚行 / 指针 | **不满足** | 含机制描述的替换（采集排除条件、生成器关闭的界限）与新增申报块，非纯措辞 |
| ③ 不触 `docs/contracts/**` | 满足 | 零契约改动（依据见 §0「契约面零动作」） |
| ④ 不触 §1.2 / §2.2 / §2.3 / §5.x 视图结构 / §9 演进路线 | 部分 | 触 §5.1.2 组件表一行（front 行内一句替换，非视图结构重组）；§1.2 / §2.2 / §2.3 / §9 零触 |

**自评 = 主路径**（① 决定性否定）。本件已按主路径文书重量起草；判官若认为①的「机制含义」判定可放宽，可直接裁轻而不需改写本件；判档归判官、只升不降。

## 0. 想实现的效果与差距

**目标效果**：`docs/architecture.md` 的记忆写入边界与当前 live 实况一致——边界写成一条**类级规则**（平台内部系统回合不入画像；设备流程回合与孩子真实聊天统一归属、按内容参与记忆；家长侧回合单列在外），dreaming 作为该类实例注明并把「关的是生成器、不是采集分类」的界限写清；`docs/decisions/ADR.md` ADR-016 以带日期注记如实记录决策②的机制枚举部分已失效；需求面 v1.68 的「内容归属」口径在架构面有唯一、可执行的落点。

**差距（一句话）**：§6.7 现文把写入边界写成两把配置键（`noisePatterns` + `captureSystemRuns=false`）的逐行枚举——第一把已于 2026-10-24 退役（live 实读为 `[]`），第二把挡不住 dreaming（2026-10-25 实测占 owner 最近 100 条结论窗口 13/100）；同时设备流程回合已改为统一携 `user` 落设备 session 键族，现文「user 消息被挡、不入库」整段与实况相反。

**契约面零动作（依据）**：记忆写入语义不在 `docs/contracts/**`——契约面定义的端云数据格式（笔顺 JSON / display 响应块 / 课表 MD / shim API）不承载记忆写入归属；契约 §四 `kidpal_story_delete` 只在 NFR-PRIV-1 被**引用**为「故事删除不触记忆库」的现行边界，不被本批改写；`kidpal_session_ctx` / `start_organizing` 等既有载体零改。故本批不携带契约包三节。

**非目标（本批不做）**：

- 不改需求面——`docs/requirements.md` v1.68 已 frozen，本批以其为语义输入，逐字不回改；
- 不写实现 / 部署 / 数据动作——dreaming 开关（`261025-openclaw-dreaming-disable`）与脏数据清理（human 运维轮，105 条 + 5 会话）已另行完成，本批只把结果写进 spec；
- 不补 `subagent:*` 排除机制、不改 honcho 插件、不改 deriver 提示词——`TECH-261026` 的四条候选路径交后续立项选路；
- 不动已 closed 的 `TECH-261025` 与只引不改的 `TECH-261026` 条目正文；
- 不碰 ECS 与任何 live 数据。

## 2. 问题、根因与改法（T1…T9）

**总根因（一次说清）**：§6.7 的写法是「按当时可见的两把配置键逐行枚举边界」，而这两把键都不是内容判据——`noisePatterns` 只能整条丢消息（2026-10-24 因整条丢掉了孩子原话而退役），`captureSystemRuns=false` 只按会话分类排除三条件。边界真正成立靠的是**内容归属**（谁说的话）与**回合类别**（平台自己调度还是孩子开口），而不是某几个配置键是否在位。逐行枚举的形态每遇到一次实现变更就要整段重写，且读者会把配置键误读为防线本身；本批把边界提升为类级规则，把配置键降为「现行机制与界限」的事实记载。

### T1 · §6.7 写入范围总段换代：类级规则 + 内容归属三类 + 两把杠杆实况与界限

**问题**：`docs/architecture.md:360` 整段（「实际边界由三件构成」①②③）双失效——① 的 `noisePatterns` 已退役，② 的 `captureSystemRuns=false` 挡不住 dreaming，③ 的「读侧不区分回合」仍成立但被写成残余尾句、未与类级规则并置。读者据此会以为写入边界已被两个配置键守住，而实际 13/100 的画像窗口被机器自言自语占位。

**证据**：

- 现文逐字（`docs/architecture.md:360`）：「实际边界由三件构成：① 写侧以插件 `config.noisePatterns` 挡 front 两条一次性 prompt 的 **user 消息**（配置键在上游插件 schema 内；接线生效随部署轮验收）；② 系统回合（cron / 心跳）由 `captureSystemRuns=false`（默认值）排除；③ 读侧画像注入**不区分回合**。」
- `noisePatterns` 退役 = human 2026-10-24 R7/OQ-4（一次性回合统一携 `user=<device_id>`），live 实读 `[]`（`docs/requirements.md:759` v1.68 证据列；`delivery/taskpackages/261024-oneshot-user-uniformity/docs-delta.md` §一第 1/3 行）。
- dreaming 不被 `captureSystemRuns` 排除 = `delivery/review/261022-arch-review-device-memory-prompt-overhaul/dreaming-capture-noise-20261025.md` §2 第 3–5 条（插件 `helpers.js` `isSystemRun` 三条件、`dreaming-narrative-*` 落 chat 分支）与 §3 量化（owner 侧 17 条、最近 100 条窗口 13/100）。
- 生成器关闭实况 = `server/deploy/261025-openclaw-dreaming-disable/dreaming-disable-declaration.json` `_note`（写面 = 恰一个布尔叶键 `plugins.entries.memory-core.config.dreaming.enabled=false`；开关无 per-agent 作用域）+ `docs/debt/tech/_archive/2026/TECH-261025-dreaming-capture-pollutes-honcho.md` 数据清理 append（2026-10-26：删 105 条 + 5 会话，窗口 13/100 → 0/100）。
- 「记忆巩固」歧义的两类对象 = `delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-recall-architecture-20261024.md:90`（Honcho `dream（omni）` = 对每个 collection 做 deduction / induction 的服务端巩固；`HON/config.py:1405-1416` 阈值与调度），同文件 `:48`（`card_refresh` dream 刷新 peer card，omni dream 不刷新 card）——它不采集新回合。

**根因**：边界被写成「配置键在位即边界成立」，而配置键（a）是可退役的部署事实、（b）按会话分类而非内容判定、（c）覆盖不全时不会报错。逐行枚举于是随每次实现变更整体过期，并诱导读者把「配置存在」当成「边界成立」。

**改法**：`docs/architecture.md:360` 整段替换为下述四段（首段保留插件全局生效与 2026-10-22 误写沿革的更正史实；其后三段为类级规则、两类「记忆巩固」区分、两把杠杆实况）。替换后完整文本：

> **记忆写入范围（类级规则，以已验证行为为准）**：生产记忆接线 = OpenClaw 上游插件 `@honcho-ai/openclaw-honcho`（经 OpenClaw 自有插件管理器安装、pin 版本），采集（落消息 / 抽 observation）与注入（`before_prompt_build` 画像）按回合钩子**全局生效**——插件没有「仅携 `user` 的回合才入库」这类逐回合判据（2026-10-23 实况；2026-10-22 初版按「请求是否携 `user` 即是否入库」的写法源于探针配置缩略式的误读，已废止）。写入边界按**内容归属**定义，分三类：
>
> - **孩子的原话**——无论承载它的是孩子真实聊天，还是设备为孩子运行的流程回合（故事整理、卡壳引导等）：照常记入、照常参与记忆。一次性回合自 2026-10-24（R7/OQ-4）起统一携 `user=<device_id>` 落同一设备 session 键族，归属差异只体现在**内容**，不体现在会话归属；
> - **小伴或系统生成的回复产物**——整理成篇的故事、引导语、叙事生成文本等：可以留存于记忆存储，但不视为孩子说的事实、不据以形成关于孩子的结论；
> - **平台内部系统回合**——cron / 心跳 / 记忆巩固与叙事生成等由平台自身调度、孩子没有开口的回合：内容不进画像。**类级规则**（下表为实例枚举；新回合形态入册前须实测其写 / 读行为，按实测入表）。类级边界自记忆子系统上线（2026-10-22）起对新增回合生效；历史语料照记（见下「历史 backfill」）。**类别优先于内容**：平台内部系统回合即使正文夹带孩子旧原话片段（dreaming 的叙事素材即此类），整回合仍不入画像——那些片段在原始对话回合里已各自记过，不因被后台回合二次引用而计入。
>
> 「记忆巩固」一词在两类系统里指向不同对象，**不可按词面一刀切**：OpenClaw `memory-core` 的 **dreaming** 是「被当对话捕获的后台子回合」（每相位在素材满足时跑一次写梦境日记的 subagent turn，其 user 消息 = 叙事指令 + 真实对话片段），属本类排除对象；Honcho 自身的 **dream**（omni / `card_refresh`）是**服务端巩固机制**——不采集新回合，只对已入库消息做 deduction / induction、产 conclusions 与 peer card，属合法记忆机制，不在「内容不进画像」的排除范围。
>
> **现行机制与界限（两把杠杆）**：① **采集侧的分类排除** = 插件 `isSystemRun` 三条件（`classifySession === "cron"` / `trigger ∈ {cron, heartbeat}` / `provenance === "internal_system"`）+ `captureSystemRuns=false`（默认）——它按会话分类工作、不是内容判据，只覆盖 cron / 心跳 / 显式 internal_system 三类；② **`noisePatterns` 已退役**（2026-10-24 R7/OQ-4：一次性回合不再整条丢消息；live 实读为 `[]`），不再是边界件。dreaming 是①的**已知实例**：其 session key `dreaming-narrative-memory-core-v2-<phase>-<hash>` 不命中三条件、落 chat 分支被当普通对话采集（2026-10-25 实测占 owner 最近 100 条结论窗口 13/100）；现行防线 = **生成器被全局关闭**（`plugins.entries.memory-core.config.dreaming.enabled=false`，`261025-openclaw-dreaming-disable` 部署 + 2026-10-26 数据清理，清理后窗口 0/100）——**关的是生成器、不是采集分类**；重开 dreaming 即复发（重评触发与整体重开口径见该部署声明 `_note`）。

为什么这样改：对根因（a）——配置键退役后边界表述不再随之失效，因为边界锚在内容归属与回合类别；对根因（b）——明确写出「分类排除不是内容判据」的界限，读者不会再把 `captureSystemRuns=false` 当万能挡板；对根因（c）——dreaming 的覆盖不全被如实写成实例 + 生成器关闭，并明示「重开即复发」。另加「类别优先于内容」一句，专堵一条可预见的误读：dreaming 的 user 消息里夹带真实孩子原话片段，若不写优先序，读者会拿第一类（孩子的原话照记）替平台内部回合开豁免口子——该片段在原始对话回合里已记过，后台回合不构成第二次计入的理由。

目标文件：`docs/architecture.md`（改）

### T2 · §6.7 实例枚举表：1/2/3 行换代、5 行外列、新增 dreaming 实例行

**问题**：`:364-366` 三行仍按旧口径写——第 1/2 行说「user 消息被 `noisePatterns` 挡、不入库」，与 2026-10-24 后「统一携 `user`、照常入库」的实况相反；第 3 行「排除：cron 会话分类 + `captureSystemRuns=false`」结论仍成立，但须标明它是**平台内部系统回合**的一个实例；家长 QQ 行（`:368`）照常入库但未标明它落在类级边界之外；dreaming 这个已被实测命中的实例不在表内。

**证据**：

- 现文逐字（`:364`）：「user 消息被 `noisePatterns` 挡、不入库；**模型回复仍落库**（残余；引导句不含孩子隐私）；**读侧仍注入孩子画像**（残余）」；（`:365`）「user 消息（孩子原话）被 `noisePatterns` 挡；**整理产物仍落库**…」；（`:366`）「排除：cron 会话分类 + `captureSystemRuns=false`（默认）」；（`:368`）「**消息照常落库**（残余）；家长回合按 `senderId` 归独立通道 peer…」。
- 一次性回合携 `user` 的实现 = `server/gateway/front/front_app.py:405-411`（整理回合 `chat_kwargs = {"user": device_id} if device_id else {}`）与 `:853-857`（引导回合同口径）；`server/gateway/provider.py:99-115`（`openclaw_stream_request` 的 `user` 可选、真值才加键）。
- 包装指令与素材混在同一条 user 消息 = `front_app.py:409-410`（整理）与 `:848-849`（引导）的恒定包装模板；`TECH-261026` 证据 1。
- 家长侧独立 peer = `docs/architecture.md:368` 现文（保留，只加「边界外第三类」定位）。

**根因**：同一根因（逐行枚举跟随实现漂移）；表 1/2 行是 `noisePatterns` 退役的直承受害面，且表本身没有被宣称为「类级规则的实例枚举」，读者会误以为表就是边界全集。

**改法**：替换表前引导句与 1/2/3/5 行、在 6 行后追加第 7 行、更新表后注记。逐字如下。

表前引导句（替换现文「因此「设备流程触发的回合不入画像」在本形态下**只对 user 侧输入成立**——模型侧回复落库与读侧注入是有申报的残余；完整枚举（S-F5.1 AC5 的「完整枚举由架构文档定义」即本表）：」）：

> 下表是类级规则的**实例枚举与实测归属**（完整枚举由本节承载，S-F5.1 AC5 所引「完整枚举由架构文档定义」即此表；以实测为准、不按设计意图推定）：

第 1 行：

> | 1 | 卡壳引导回合 | front 一次性 LLM（`_openclaw_chat_once`；user 消息 = 恒定包装「孩子到目前为止讲的内容：…\n\n孩子停顿了，请按指令给一句轻声引导。」；携 `user=<device_id>`） | **入库、照常参与记忆**（孩子原话照记；包装指令部分的派生残余见（四））；模型回复落库、只产 agent 侧结论（见（二））；读侧画像照常注入（见（一）） |

第 2 行：

> | 2 | 忠实整理回合 | 同上（恒定包装「孩子讲的内容：…\n\n请按指令把这段话忠实整理成一篇。」；携 `user=<device_id>`） | **入库**：孩子原话照记；整理产物 = 小伴生成的回复产物，留存于记忆存储但不据以形成关于孩子的结论；读侧仍注入孩子画像——整理回合可能被画像「带跑」（忠实性质量风险，真机抽检 3–5 例）；同一段内容先进引导、再进整理 = 两遍入库（见（三）） |

第 3 行：

> | 3 | 提醒到点语义回合 | OpenClaw cron → **main** agent 产 `semantic.json` | **排除**（平台内部系统回合实例）：cron 会话分类 + `captureSystemRuns=false`（默认）；其 user 内容是机器生成的任务提示、不是孩子原话 |

第 5 行：

> | 5 | 家长 QQ 录入 / 查日志回合 | QQ 通道 → main agent | **消息照常落库**（残余）；家长回合按 `senderId` 归独立通道 peer（插件默认 `per-sender` 策略，不污染家庭 owner 画像）——它既不属于「孩子的话」也不属于「平台内部系统回合」，是类级边界之外的**第三类（家长侧）**；家长内容可被 `honcho_*` 工具（如跨会话检索工具 `honcho_search_messages`）检索到的风险待部署轮实测，未闭合前按未闭合风险记账 |

第 7 行（新增，置于第 6 行之后）：

> | 7 | 记忆巩固 / 叙事生成（`memory-core` dreaming） | OpenClaw `memory-core` 插件后台 subagent turn（session key `dreaming-narrative-memory-core-v2-<phase>-<hash>`） | **平台内部系统回合实例，但采集侧无排除机制**（key 落 chat 分支、`isSystemRun` 不命中）——现行处置 = 生成器全局关闭（2026-10-25 部署 + 2026-10-26 数据清理，owner 最近 100 条结论窗口 13/100 → 0/100）；重开 dreaming 即复发 |

表后注记（替换 `:371` 现文）：

> （记录语境期间引擎侧本就无 LLM 回合〔ADR-013 读法乙〕，不在枚举内。表外的 Honcho 自身 dream（omni / `card_refresh`）不是回合、是服务端巩固机制，见上文「记忆巩固」两类区分。新回合形态入册前须实测其写 / 读行为，按实测入表——**以实测为准，不按设计意图推定**；front / QQ 两行的部署轮实测随验收包落盘，与本表不符时以实测为准回填。）

目标文件：`docs/architecture.md`（改）

### T3 · §6.7 新增「残余与未闭合申报」块（八条）

**问题**：现文把残余零散写在表行与尾句里（「模型回复仍落库」「读侧仍注入」），未成块申报；architect 2026-10-26 核实的多项已知残余（包装指令派生、双回合重复、身份缺席降级、多腿归属、`subagent:*` 未闭合、可验证性缺口）在架构面没有落点，只有需求面 v1.68 的一句概括引用。读者无法从架构文档得知「边界成立到什么程度、哪里还没闭合」。

**证据**：

- 包装指令派生 = `TECH-261026` 证据 1–3（源消息形态 `front_app.py:409-410` / `:848-849`；live 实证 owner 侧 6 条过程性结论，最近 100 条窗口第 35–69 位）。
- 双回合重复 = `delivery/taskpackages/261024-oneshot-user-uniformity/docs-delta.md` §三第 3 条（同一段孩子内容先进引导、再进整理 → 消息库两遍、observation 可能重复膨胀）。
- 身份缺席降级 = `front_app.py:853-857` 与 `:1246-1250`（日志 `oneshot_user=absent reason=device_id_missing`）；`provider.py:99-115`（缺 `user` 键 = 回落旧行为）；`docs/decisions/ADR.md:130` ADR-015 ③（省略 `user` = 每请求随机 session）。
- 多腿归属 = `front_app.py:324-334` 函数文档逐字：「会话腿集 -> 设备身份（第一条携 device-id 头的腿原值）。多腿兜底归属申报残余：映射缺席退 `_alert_legs` 时取第一条腿——单设备 MVP 成立（决策 22）；多设备将来会错归属，不在本包解。」
- `subagent:*` = `dreaming-capture-noise-20261025.md` §4 末（`isSystemRun` 对 `subagent:` 前缀同样只参与分类、不参与排除）；`server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` §9（升级盘点四家族必须为空，含 `subagent:*`）。
- 可验证性缺口 = `docs/requirements.md:603`（v1.68 可行性 🟡：类级边界只能静态配置核 + 运维观测、无管线机械闸）与 `:759` v1.68 流程留痕（同句）。

**根因**：架构文档此前只登记「已实测的行为」，未设「已知未闭合」的固定位置；残余散在表行里，一旦表行改写就会被冲掉（本批 T2 正是一次改写）。

**改法**：在表后注记与「**记忆身份**」段之间插入独立加粗块，标题为「**残余与未闭合申报**」，八条编号（一）–（八），逐字如下（每条一句事实 + 去向；不承诺已修）：

> **残余与未闭合申报**（human 2026-10-26 裁「按现状记录、本批不修实现」；需求面同口径申报 = S-F5.1 补充规则「已知残余」，本条不重复展开）：
>
> - （一）**读侧画像注入不区分回合**——`before_prompt_build` 对所有回合统一注入，写侧的内容归属不改变读侧行为；凡进入记忆库的内容都可能经 representation / 检索回到后续回合的上下文。
> - （二）**入库 ≠ 归属**——模型回复（assistant 消息）照常落库并产 agent 侧结论（`agent-kidpal`），但观察者扇出按 role 判定，assistant 消息不进 owner（孩子）画像；「留存于记忆存储」与「据以形成关于孩子的事实结论」是两件事，设备流程回合的回复产物同此。
> - （三）**引导 + 整理双回合重复喂同一段孩子内容**——消息库两遍、observation 可能重复膨胀（检索质量面；261024 批申报，部署轮看检索）。
> - （四）**包装指令被抽成过程性结论**——设备流程回合的 user 消息 = 孩子原话 + 机器写的整理 / 引导指令混合（`server/gateway/front/front_app.py:409-410` / `:848-849` 恒定包装），deriver 按 role 而非 authorship 扇出，指令部分会被抽成「孩子要求整理成文」一类过程性结论（live 实证 owner 侧 6 条，位于最近 100 条窗口第 35–69 位）。当前处理 = 运营面定期清理；机制修复另行立项（候选路径四条：改包装形态 / deriver 提示词补丁 / 运营清理 / 上游提报；依据 = `docs/debt/tech/TECH-261026-oneshot-wrapper-prompt-derived-facts.md`）。
> - （五）**缺席 `device_id` 时的降级**——一次性回合取不到设备身份时不携 `user`（`front_app.py:853-857` / `:1246-1250`，日志 `oneshot_user=absent`），OpenClaw 回落「每请求随机 session + 调用方自携带历史」（ADR-015 ③）；插件采集与注入仍全局生效——**降级的是会话连续性，不是记忆排除**。
> - （六）**多腿归属单设备 MVP 口径**——设备身份取第一条携 device-id 头的会话腿（`front_app.py:324-334`）；单设备 MVP 成立；多设备时按第一条腿归属会出错，多设备身份体系立项时解。
> - （七）**`subagent:*` 家族未闭合**——插件 `classifySession` 把 `subagent:` 前缀单独分类，但 `isSystemRun` 不排除，当前无机制保证其不入库（无命中数据，如实申报未闭合）。运维防线 = 部署 runbook §9 升级盘点（`dreaming-narrative-*` / `cron:*` / `heartbeat:*` / `subagent:*` 四家族必须为空）；命中即采集策略漂移，先盘点、机制修复另立实现批。
> - （八）**类级边界在现有实现面无管线机械闸**——可判的只有两层：① 静态配置实读（`openclaw config get` 核 dreaming 叶与插件配置键）；② Honcho 会话家族盘点（运维轮）。Phase 1 只有运维观测、无管线机械闸（requirements v1.68 S-F5.1 可行性行 🟡）。

为什么这样改：对根因——把「已知未闭合」固定在架构文档的稳定位置，表行改写不再冲掉残余；同时把「可验证到什么程度」写成两层可判手段，避免下游把类级规则误当成已被管线强制的保证。

目标文件：`docs/architecture.md`（改）

### T4 · §6.7 记忆身份段补一次性回合归属；画像边界段「来源门」换代

**问题**：`:373` 记忆身份段只写设备主流链路进 session 键族，未写一次性回合自 2026-10-24 起也进同一键族——读者会把「引导 / 整理回合」误当会话外调用；`:375` 画像边界的来源门仍写「上表记忆写入范围（user 侧输入为主边界…）」，沿用旧口径且与 T1/T2 的类级规则不一致。

**证据**：

- `:373` 现文逐字：「**记忆身份**：记忆库内 peer = 家庭单 `owner`（另有 agent 侧 `agent-kidpal`）；设备身份体现在 **session key 家族** `agent:kidpal:openai-user:<device_id>`——每设备一族会话，换设备 = 新 session 家族（会话连续性重置），**画像仍为家庭单 owner、不按设备隔离**（§1.3）。」
- `:375` 现文逐字（① 段）：「① **来源门** = 上表记忆写入范围（user 侧输入为主边界，模型侧回复与读侧注入的残余见各行申报）；」

**根因**：同一根因（边界换代后，周边指针句未随之换代；2026-10-24 的变更当时只落了实现，spec 修订归本批）。

**改法**：

- `:373` 段尾追加一句（原文其余不动）：「一次性回合（引导 / 整理）自 2026-10-24 起同样携该 device 身份落同一 session 键族；身份无法确定时的降级见残余（五）。」
- `:375` 的 ① 整句替换为：「① **来源门** = 上文的类级规则与内容归属（孩子的原话照记；机器产物留存但不据以形成关于孩子的结论；平台内部系统回合不入画像）——读侧注入不区分回合、模型回复落库为已申报残余（见（一）（二））；」

目标文件：`docs/architecture.md`（改）

### T5 · §5.1.2 front 行一句替换；记忆子系统行核后不动

**问题**：`docs/architecture.md:192`（front 网关行）尾部写「引导 / 整理一次性 LLM 回合不携 `user`（user 消息不入库，模型回复与读侧画像注入为已申报残余）」——与 live 实况相反（一次性回合自 2026-10-24 起携 `user=<device_id>`，user 消息照常入库），且被需求面 v1.68 与本批 §6.7 换代甩旧。这是 spine 后补的漏列项。

**证据**：

- `:192` 现文该句逐字：「；**引导 / 整理一次性 LLM 回合不携 `user`**（§6.7 记忆写入范围：user 消息不入库，模型回复与读侧画像注入为已申报残余）」
- 实现 = `front_app.py:405-411`（整理回合携 `user` 调用）与 `:853-857`（引导回合）；`provider.py:99-115` 注释「Optional ``user`` (261024-oneshot-user-uniformity)」。
- spine `faceMap` 标注：`:192` 是「architect 2026-10-26 核出的 spine 漏列项，与 live 实况相反，必须进架构面 landing 清单」。

**根因**：261024 实现批动了 front 的一次性回合调用形态，但该行的括号注记仍停留在 261023 口径（当时一次性回合确实不入库）；261024 批的 docs-delta 把 spec 修订全部挂给在途批，本批即其落点。

**改法**：该句整句替换为（整行其余文字一字不动）：

> ；**引导 / 整理一次性 LLM 回合携 `user=<device_id>` 落同一设备 session 键族**（2026-10-24 R7/OQ-4；§6.7 按内容归属：孩子原话照常参与记忆，包装指令与模型回复的残余见该节残余申报）

`docs/architecture.md:203`（记忆子系统行）**核后不动**：现文「写入范围与残余见 §6.7」是指针式表述，§6.7 换代后指针目标仍有效，无须改写。

目标文件：`docs/architecture.md`（改）

### T6 · §4 NFR-PRIV-1 映射行换代

**问题**：`docs/architecture.md:155` 仍写「`noisePatterns` 挡 front 一次性回合 user 消息 + `captureSystemRuns=false` 挡系统回合」——两个机制陈述一个已退役、一个覆盖不全；且与需求面 NFR-PRIV-1 ① 的新类级边界表述不一致。

**证据**：`:155` 现文逐字：「| NFR-PRIV-1 记忆边界与保留 | 记忆写入范围（已验证接线：`noisePatterns` 挡 front 一次性回合 user 消息 + `captureSystemRuns=false` 挡系统回合；模型回复与读侧注入残余逐行申报）+ 抽取三层纵深（来源门 / 抽取策略 / 注入过滤）+ 自托管不出网 + 删除 = 运维手工 | §6.7 / §5.1.2 记忆子系统 / §5.4 记忆行 |」；需求面新口径 = `docs/requirements.md:654` NFR-PRIV-1 ①（类级边界 + 统一归属 + 回指 S-F5.1 补充规则）。

**根因**：同 T1 根因；映射行是 §6.7 的投影，源换代后投影未同步。

**改法**：该行第二列替换为：

> 记忆写入范围（类级规则：平台内部系统回合不入画像；设备流程回合与孩子真实聊天统一归属、按内容参与记忆；家长侧回合列在边界外——机制实况与残余见 §6.7 残余申报）+ 抽取三层纵深（来源门 / 抽取策略 / 注入过滤）+ 自托管不出网 + 删除 = 运维手工

目标文件：`docs/architecture.md`（改）

### T7 · ADR-016 决策② 带日期注记（不改写决策本体）

**问题**：`docs/decisions/ADR.md:138` ADR-016 决策② 仍写「front 一次性回合的 user 消息以插件 `noisePatterns` 挡」——该机制已退役；`captureSystemRuns=false` 的覆盖也已被 2026-10-25 实测收窄。ADR 是决策记录，header 明确「append-only：不覆盖、不改写既有条目」，故不能重写决策②。

**证据**：

- `docs/decisions/ADR.md:138` 现文逐字（②）：「② 写入范围 = **已验证接线的实际行为**：front 一次性回合的 user 消息以插件 `noisePatterns` 挡、模型回复落库（残余）；cron / 心跳由 `captureSystemRuns=false` 排除；QQ 家长消息落库、按 `senderId` 归独立通道 peer，`honcho_*` 工具可检索风险待实测；读侧画像注入不区分回合（残余）。残余逐行申报于 arch §6.7。」
- ADR 写入纪律 = `docs/decisions/ADR.md:6`：「**append-only：不覆盖、不改写既有条目**；推翻旧决策 = 新增一条并注明 supersedes ADR-XXX。」
- 同文件先例形态 = ADR-016 对 ADR-014 的 supersede 记法（`:141`「ADR-014（本 ADR supersede 其 ②③…）」）与 ADR-011 的「备选与放弃原因（supersedes 注记）」行（`:100`，取代 ADR-005 / ADR-010 的部分否决理由——同样以**新增行**承载、不回改原文）。

**根因**：决策② 的**本体**（写入口径 = 已验证接线的实际行为、残余逐行申报于 arch §6.7）仍然成立且正是本批遵循的原则；失效的只是它当时枚举的两个机制事实。用 supersede 新 ADR 会误伤本体（本体没被推翻），用改写会违 append-only——故取「带日期注记」：正文零改，条目末尾追加一条注记行。

**改法**：在 ADR-016 条目「**关联**」行之后追加一条注记（位置 = 该条目最后一行，不插入其他条目之前；原文一字不改）：

> - **2026-10-26 注记（决策②的机制枚举部分失效，决策本体不变）**：`noisePatterns` 已于 2026-10-24 退役（R7/OQ-4：一次性回合统一携 `user=<device_id>`、孩子原话照常参与记忆；live 实读为 `[]`），决策② 所列「front 一次性回合的 user 消息以插件 `noisePatterns` 挡」不再成立；`captureSystemRuns=false` 的覆盖也由 2026-10-25 实测收窄——`memory-core` dreaming 子回合落 chat 分支、不被该键排除，现以「生成器全局关闭」处置。决策② 的**本体**（写入口径 = 已验证接线的实际行为、残余逐行申报于 arch §6.7）不变；现行边界与全部残余的权威 = arch §6.7（`261025-spec-reconciliation-memory-writing-scope`）。

**形态理由（本批要求显式交代）**：注记形态胜在三处——① 不违 ADR append-only 纪律（正文零改，只追加一行）；② 不制造第二条决策与既有本体分叉（新建 supersede ADR 会让读者以为「已验证接线」原则被推翻，而它没有）；③ 失效的是机制事实而非决策方向，注记正好把「哪部分过期、哪部分仍立」切在人读的一行里。若判官认为 ADR-016 应整体 supersede，那属方向级重裁，超出本批（本批以此为默认形态，提请评审确认）。

目标文件：`docs/decisions/ADR.md`（改）

### T8 · 术语表新增「平台内部系统回合」词条

**问题**：本批引入的「平台内部系统回合」是与「设备流程回合」「家长侧回合」并列的类级术语，而术语表只有「记忆画像」「observation」「记忆子系统」三条；不落词条，三个近义说法（「系统回合」「后台回合」「平台自动运行的回合」）会在后续批里继续漂移，且「记忆巩固」一词的两义（OpenClaw dreaming vs Honcho dream）没有术语锚。

**证据**：`docs/glossary.md:12`（记忆画像）`/ :31`（observation）`/ :32`（记忆子系统）三条现状；需求面 v1.68 与本批 §6.7 均使用「平台内部系统回合」作类名（`docs/requirements.md:601`、`:654`）。

**根因**：类级边界换代同时引入类名，术语表未同步即留下三义漂移面——本仓「术语消歧」先例（glossary 词条随 261022 批落）即为此设。

**改法**：在 glossary 记忆相关词条区追加（位置 = 「记忆子系统」词条之后）：

> - **平台内部系统回合**：由平台自身调度、孩子没有开口的后台回合（cron / 心跳 / 记忆巩固与叙事生成等）——内容不进记忆画像（类级边界）。与「设备流程回合」（故事整理 / 卡壳引导等，承载孩子原话、照常参与记忆）及「家长侧回合」（QQ 等，按 per-sender 归独立 peer、不污染 owner 画像）互斥不替代。完整定义与实例枚举 = arch §6.7（261025-spec-reconciliation-memory-writing-scope）。

目标文件：`docs/glossary.md`（改）

### T9 · §12 变更记录表落本批 changeId 行

**问题**：架构面的「已冻结」消费判据 = §12 变更记录节对应 changeId 行在场（`launchPreconditions` 消费）；本批不落行，下游无法判定 spec 已定稿。

**证据**：`docs/architecture.md:485` 纪律逐字：「本文件每次改动都在下表追加一行（新行在上，倒序）——changeId、日期、改了哪些节、依据。**changeId 即定稿标记**…「已冻结」的消费判据 = 本节对应 changeId 行在场」。

**根因**：流程要求，非缺陷。

**改法**：在 §12 表头行（`:488`）与现行首行（`:489`，261023 批）之间插入：

> | 261025-spec-reconciliation-memory-writing-scope | 2026-10-26 | §4（NFR-PRIV-1 映射行：两把配置键口径 → 类级规则 + 内容归属）/ §5.1.2（front 行一次性回合携 `user` 落设备 session 键族；记忆子系统行指针核后不动）/ §6.7 改写（逐行枚举 → 类级规则 + 现行机制与界限 + 七行实例表 + 残余与未闭合申报八条 + 记忆身份补一次性回合归属 + 画像边界来源门换代） | delivery/review/261025-arch-review-spec-reconciliation-memory-writing-scope/PROPOSAL.md（需求面 v1.68 为语义输入、不回改；契约面零动作；依据 = requirements v1.68 + spine + TECH-261026 + 261025 部署包与 docs-delta + architect 2026-10-26 核实） |

目标文件：`docs/architecture.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 架构 spec | `docs/architecture.md` | 改（T1–T6、T9） | §4 映射行 / §5.1.2 front 行 / §6.7 五处 / §12 变更记录行；§1.2 / §2.2 / §2.3 / §9 零触 |
| 决策记录 | `docs/decisions/ADR.md` | 改（T7 注记） | 只追加一行注记，正文零改；ADR-014 ③ 已被 ADR-016 ③ supersede，本批不触 |
| 术语表 | `docs/glossary.md` | 改（T8） | 新增一词条；既有三条零改 |
| 需求 spec | `docs/requirements.md` | 零触 | v1.68 frozen；本批以其为语义输入，逐字不回改 |
| 契约面 | `docs/contracts/**` | 零触 | 记忆写入语义不在契约内；§四 `kidpal_story_delete` 只引用不定义 |
| 治理面 | `.claude/**`、`AGENTS.md`、`CLAUDE.md`、`docs/DEV-GOVERNANCE.md` | 零触 | 无流程 / 角色 / 门 / 脚本变更 |
| 实现面 | `firmware/**`、`server/**` | 零触（只读取证） | 本批引用代码行仅作证据；无任何写入 |
| 部署 / 数据 | `server/deploy/**`、ECS | 零触 | dreaming 开关与清理已由 261025 包与运维轮完成 |
| debt 账本 | `docs/debt/**` | 零触 | TECH-261025 已 closed 归档；TECH-261026 deferred、本批只引用 |

**逐点核对「核后不动」的邻接面**（供评审查牵连面）：`docs/architecture.md:55`（§1.3 记忆身份范围声明）与 :283–284（§5.4 记忆两行）均为指针式且口径正确；`:40`（§1.2 Honcho 依赖行）、`:415`（§8.1 风险 13 降级）只引 §6.7 小节号，节号不变；§10 决策历史档无记忆写入口径条目；ADR-014 :122 ③ 的「完整枚举六行」表述已被 ADR-016 ③ supersede（append-only，不回改）。

**复扫 census（2026-10-26 起草期实测，命令与输出）**：

- `grep -ro "noisePatterns" docs | wc -l` = 7；分布 = `docs/architecture.md` 4 行（`:155 :360 :364 :365`）/ `docs/decisions/ADR.md` 1（`:138`）/ `docs/requirements.md` 1（`:759`，v1.68 历史行，append-only 不回改）/ `docs/debt/tech/TECH-261026-...md` 1（debt 正文，本批不改）。
- `grep -ro "captureSystemRuns" docs | wc -l` = 6；分布 = architecture 3 行（`:155 :360 :366`）/ ADR 1（`:138`）/ requirements 1（`:759` 历史行）/ debt 归档 TECH-261025 1（归档件不回改）。
- `grep -ro "一次性回合" docs | wc -l` = 3；分布 = architecture 1（`:192`）/ ADR 1（`:138`）/ TECH-261026 1。
- 本批落地后预期：architecture 中 `noisePatterns` 仅余 T1 新段 1 处（作为「已退役」事实记载），`captureSystemRuns` 仅余 T1/T2 的「分类排除三条件」记载；ADR 因 T7 注记新增 1 处提及；requirements / debt 不动。**模式集完备性论证**：目标词只有两种形态——单一无空格英文标识符（`noisePatterns` / `captureSystemRuns`）与中文短语「一次性回合」，仓内无变体拼写、无中英混写先例；大小写变体经同一条 grep 一并覆盖（英文标识符在仓内只以小写首字母驼峰形态出现）。故不另列词形变体表。

**未实测项**（不写断言）：本批所有实况断言均来自既有实测制品与代码现读；**本批不新增任何 live 实测**——`noisePatterns=[]`、dreaming 13/100→0/100、owner 侧过程性结论 6 条均转引 2026-10-26 需求面 v1.68 证据行与 2026-10-25/26 制品，起草期已逐条回读原处核对（requirements v1.68 行、TECH-261025 归档正文、TECH-261026 正文、dreaming 诊断报告 §2/§3、261024 docs-delta §一/§三）。

## 4. human 取舍点（OQ）

**OQ-1（架构面裁，本批内容）：`subagent:*` 家族本批怎么处置**（spine OQ-2 的落点）

- (a) **申报未闭合 + 运维盘点防线**（推荐）：本批零实现，无法补排除机制；当前无命中数据，按 T3（七）如实申报，防线 = 部署 runbook §9 四家族盘点，机制修复另立实现批。
- (b) 本批扩为跨面（触实现面）：架构落笔同时起实现包补 `subagent:*` 排除。会破本批「纯文档面、零实现」定位，需 human 立项并把实现面五档分流并入——建议单独立项，不塞进本批。
- (c) 不申报、等有命中数据再说：反对——类级边界若声称完整而实际不完整，等于把「不知道」写成「已守」。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**landing 前（pre-commit）**：

1. `node .claude/scripts/spec-anchor-check.mjs` —— 预期 `ok: true`、`errors: []`（起草期基线 2026-10-26 实测：ok true / errors [] / storyCount 14 / archSectionCount 48）；本批不改小节编号，锚应零新增悬空。
2. `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261025-arch-review-spec-reconciliation-memory-writing-scope/PROPOSAL.md` —— 预期无 `bold-anchor-line` / 缺锚类格式错；`anchorUnion` 成员名单逐字 = {`docs/architecture.md`, `docs/decisions/ADR.md`, `docs/glossary.md`}（与 manifest 三行集合相等）；`advisories` 目录 token 提示逐条核（锚行注记不得夹带路径记号）。
3. `git status --porcelain docs/requirements.md docs/contracts` —— 预期输出为空（需求面与契约面零触）。
4. `git diff --stat` 落地后预期只含三个目标文件；`docs/architecture.md` 的改动面只落在 §4 映射行、§5.1.2 front 行、§6.7、§12 行，`docs/decisions/ADR.md` 只多一条注记行，`docs/glossary.md` 只多一条词条。

**landing 后（post-commit，landing-verifier / 判官 spot-check）**：

5. `grep -c "noisePatterns" docs/architecture.md` —— 预期 = 1（仅 T1 新段的退役事实句；原 :155 / :364 / :365 已换代）。
6. `grep -n "平台内部系统回合" docs/architecture.md docs/glossary.md` —— 预期 architecture 命中 ≥3 处（类级规则段、第 3 或第 7 行、残余或表注）且 glossary 命中 1 处。
7. `grep -n "dreaming-narrative-memory-core-v2" docs/architecture.md` —— 预期 ≥1（T1 段与第 7 行）。
8. `grep -n "oneshot_user=absent" docs/architecture.md` —— 预期 ≥1（残余（五））。
9. `grep -n "2026-10-26 注记" docs/decisions/ADR.md` —— 预期 = 1，且落在 ADR-016 条目内（用 `sed -n '135,150p'` 核位置，不得插到其他条目之前）。
10. `grep -n "261025-spec-reconciliation-memory-writing-scope" docs/architecture.md` —— 预期 ≥1（§12 变更记录行）。
11. `node .claude/scripts/landing-fidelity-check.mjs`（全量）—— landing-verifier 跑，预期绿；与 frozen PROPOSAL 的 landing 清单逐文件核对。

**时点归属**：第 1–3 条 = pre-commit；第 4 条 = 落地后、commit 前；第 5–11 条 = post-commit。

## 6. 留根清单

- **`TECH-261026`（deferred）**：包装指令被抽成过程性结论——四条候选路径（A 改包装形态 / B deriver 提示词补丁 / C 运营清理 / D 上游提报）交后续立项选路；owner = dev-loop + 主线程 + architect。
- **`subagent:*` 排除机制**：本批只申报未闭合（T3 七）；补排除属实现面另立批（OQ-1）。
- **类级边界无管线机械闸**：Phase 1 只有静态配置实读 + 运维盘点两层；若将来起探针批，须把 `dreaming-narrative-*` / `cron:*` / `heartbeat:*` / `subagent:*` 四家族白名单一并设计（部署 runbook §9 已钉盘点口径）。
- **dreaming 重开触发**：`dreaming.enabled` 无 per-agent 作用域；重开必须整体开、并重评 kidpal 侧采集污染（权威 = 部署声明 `_note`）。本批不改该声明。
- **`TECH-261025`（closed）**：归档正文不回改；其 `consumed-by` 已指向本 spine。
- **ADR-014 ③「完整枚举六行」**：已被 ADR-016 ③ supersede，append-only 不回改；本批 §12 行写明换代到七行实例表。
- **spine OQ-5**（`TECH-261023` 条目仍写「四项」）：debt 账本 append-only 且不属 spec 面，本批不顺手改。
- **目录名与 changeId 的双轨**：review 批目录带 `arch-review` token（机械要求），changeId 沿 spine 原名（路由要求）；两条线由 slug 对齐，主线程回填申报 spine `anchors.arch` 时记合规目录。
- **自律件**：起草期已按模板「起草期自检」执行——本件落盘后即跑 `--preflight`、`spec-anchor-check`、census grep 并核字节；§5 验证集命令均为起草期亲核过的入口（`spec-anchor-check.mjs` 实测 ok true；`landing-fidelity-check.mjs --preflight` 入口在脚本用法内；`changes-index.mjs --upsert` 归主线程状态转移）。

## 7. 评审记录

（判官 append-only 区；待 architect-reviewer 四查 + 判档。主线程不预判结论。）

### R1 · architect-reviewer 评审记录（2026-10-26）

**地面真值核验（判官亲读亲跑，非转述采信）**：

- 引文逐字核对一致：`docs/architecture.md` :40 / :55 / :103 / :155 / :192 / :203 / :223 / :283-284 / :360 / :364-366 / :368 / :371 / :373 / :375 / :415 / :470 / :485-489；`docs/decisions/ADR.md` :6 / :100 / :114 / :122 / :130 / :138 / :141；`docs/glossary.md` :12 / :31 / :32；`docs/requirements.md` :598 / :601 / :603 / :654 / :759（v1.68 frozen）。
- 代码证据亲读一致：`front_app.py:324-334`（多腿兜底申报残余逐字在场）/ `:405-411` / `:848-857`（恒定包装 + 携 `user` + `oneshot_user=absent` 日志）/ `:1246-1250`；`provider.py:99-115`（`user` 可选、真值才加键、缺键回落旧行为）。
- 制品亲读一致：TECH-261026（6 条 owner 过程性结论、窗口第 35–69 位、四候选路径）；TECH-261025 归档 append（删 105 条 + 5 会话、窗口 13/100 → 0/100）；dreaming-capture-noise-20261025.md §2/§3/§4（`isSystemRun` 三条件、`dreaming-narrative-*` 落 chat 分支、owner 侧 17 条、13/100、`subagent:` 相邻风险）；261024 docs-delta §一六行 + §三残余清单；dreaming-disable-declaration.json `_note`（恰一个布尔叶键、开关无 per-agent 作用域、重评触发口径）；DEPLOY-RUNBOOK.md §9（四家族盘点，`subagent:*` 在列）；k4-recall-architecture-20261024.md :48 / :90（Honcho dream（omni）= 服务端巩固、不采集新回合）。
- 机械门亲跑：`spec-anchor-check.mjs` ok:true / errors [] / storyCount 14；`landing-fidelity-check.mjs --preflight` ok:true / anchorUnion 恰 {architecture.md, ADR.md, glossary.md} 与 manifest 三行集合相等 / advisories []。
- census 复核：`noisePatterns` 7 处、`captureSystemRuns` 6 处分布与 §3 一致；「一次性回合」总数 3 处一致（见备注 N1 的行位勘误）。

**四查**：

1. **意图保真 — PASS**。T1 三类划分与 AC5 四段一一对应（第一段 = 孩子的原话照记、第二段 = 小伴/系统回复产物不据以形成结论、第三段 = 平台内部系统回合不进画像；第四段历史补录由「历史 backfill」句承接）；类级边界生效时点句与 requirements :601 同口径。「类别优先于内容」有授权链：spine faceMap 目标形态（architect 裁类级规则）+ v1.68 §9 行「dreaming 类并入」+ AC5 第三段整体排除语义；且与实况一致（片段在原始对话回合已各记一次，后台回合不构成二次计入）。逐处改前/改后均以 live 实测制品为据，符合 human 硬要求「实践已经有了去补原来的文档，跟实践一致」——无发明新行为。T2 表行与 T5 的「统一携 `user` 落设备 session 键族」与 `front_app.py` 实测逐字吻合。
2. **纪律合规 — PASS**。契约面零动作依据成立（契约四域 = 笔顺 JSON / display 块 / 课表 MD / shim API，均不承载记忆写入归属；`kidpal_story_delete` 仅被 NFR-PRIV-1 引用、本批不改）；需求面零改（验证集第 3 条守门）；决策本体归 ADR（T7 注记形态不回改正文）；manifest 三文件精确、每 T 项给到 file:line 与逐字改前/改后；arc42 骨架 / 锚稳定规则 / 分工句无违例。
3. **撞车复核 — PASS**。T7 与 ADR append-only 纪律（:6）及 ADR-011 supersede 注记先例（:100）同构；T1 类级规则不与 ADR-016 决策本体冲突（本体「已验证接线」原则正是本批遵循的原则，失效仅机制枚举，注记已切分）；T2 第 7 行（无机制排除、生成器关闭）与第 3 行（机制排除）是同类两实例、第 5 行是边界外第三类，互斥不打架；§6.7 残余块与 requirements 补充规则「已知残余」分工清晰（arch 承载机制细节、需求面回指，NFR 粒度张力已由 human 终拍维持）；glossary 新词条与既有三条零冲突；§12 行格式与既有行同构。
4. **牵连面完整 — PASS**。独立 sweep 确认 stale 断言全集 = :155 / :192 / :360 / :364-366（T1/T2/T5/T6 全覆盖）；`:103` / `:223` / `:470` 与 ADR-013 :114 的「一次性 LLM 回合」字样无记忆边界断言、非过时面。落地后 live 残留（均为豁免或已承载）：ADR-016 决策②本体（append-only，T7 注记承载）、requirements :759 v1.68 历史行（append-only 豁免）、TECH-261026 正文 `noisePatterns` 提及（debt 正文事实性陈述仍真）、TECH-261025 归档件（豁免）、ADR-014 :122 ③「完整枚举六行」（已被 ADR-016 ③ supersede，留根已记）。反向锚牵连：requirements :602 / :656 「支撑 arch §5.1 …随架构文档修订后对账」由本批 §12 行闭环；§5.4 / §5.1.2 / §1.3 指针句核后不动判定成立（:203 / :283 / :55 实读均为指针式且口径正确）。

**两处特裁**：

- **T8（glossary 新词条）：收**。依据三条：glossary 头部纪律「新词先落这里再使用」，本批在 frozen 需求面与 §6.7 同时引入类名「平台内部系统回合」，不落词条即留下三义漂移面；「记忆巩固」两义（OpenClaw dreaming vs Honcho dream）确无既有术语锚；v1.67 批 requirement-reviewer 曾以「glossary 词条承接缺位」否回同类缺口，先例同构。词条为指针式（权威 = arch §6.7），无 SSOT 分叉；manifest 保留 glossary.md。
- **OQ-1（`subagent:*`）：处置 (a) 可接受**。本批立项 ack 钉死「纯文档面、无代码、无部署」，扩为跨面（选项 b）违反 spine 范围且应单独立项——与 PROPOSAL 建议一致；选项 (c) 把「不知道」写成「已守」，违背「存疑向严」。防线实在：DEPLOY-RUNBOOK.md §9 四家族盘点已在场（`subagent:*` 在列，亲读确认）；「无命中数据、如实申报未闭合」与 dreaming-capture-noise §4 相邻风险口径逐字一致。留根「机制修复另立实现批」去向明确。

**判档与升级**：

- **判档 = 主路径**。轻路径判据①（零语义变化）决定性不满足——记忆写入边界由配置键枚举换代为类级规则 + 内容归属三类 + 八条残余申报，边界语义实质换代；②亦不满足（机制描述替换非措辞级）。architect 自评主路径正确，无升档空间，Track A 判据三条（方向未收敛 / 评审发现方向级缺口 / 声明红队）均不命中——方向已由 human 建 spine（「把 spec 改成已验证实况」）+ 需求面 v1.68 frozen 收敛。
- **riskTier 复核 = low-medium，维持**。纯文档面（零实现 / 零部署 / 零数据 / 零契约）支撑 low；改的是隐私边界主表述与类级规则定义、误写误导下游，支撑升出 low——定档合理。

**非阻塞备注**（不构成否回；落地时顺手可改可不改）：

- N1：§3 census「一次性回合」分布记 architecture 1（`:192`）有行位笔误——该精确子串实际落在 `:155`（`:192` 是「一次性 LLM 回合」、不含该子串）。两处均已被 T5/T6 覆盖，无覆盖缺口。
- N2：T9 引「表头行（`:488`）」实为 `:487`（`:488` 是分隔行）；插入意图（新行置顶）不受影响。
- N3：T7 注记「残余逐行申报于 arch §6.7」是对决策②原文的引用式复述；落地后残余已由逐行改为块状申报（（一）–（八）），注记末句「现行边界与全部残余的权威 = arch §6.7」已消解指向歧义，不必改。

verdict: GREEN
