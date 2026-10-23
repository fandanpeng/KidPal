# PROPOSAL · 261023-arch-review-memory-wiring-verified-behavior

> **状态：FROZEN v1.0 · 2026-10-23**（R2 GREEN 在案 §7；立项 ack = human 2026-10-23「确认 FROZEN」+ OQ-1=(a)/OQ-2=(a) 裁决；判档 = 主路径；判官 = architect-reviewer，作者 ≠ 审者；R1/R2 评审记录见 §7）。
> **承载 ARQ**：无新增 ARQ。来源 = human 2026-10-23 裁决（方案甲：原样复用探针轮验证过的接线、残余偏差如实申报、spec 改成已验证行为；原则原话「我们之前测试过、已经验证过能用的功能，就直接用起来，不要再去发明什么新的东西了。」）+ `delivery/review/261022-arch-review-device-memory-prompt-overhaul/memory-wiring-advisory-20261023.md`（§1 探针轮接线证据 / §5 §6.7 六行逐行差 / §6 方案甲语义）。留根 = `docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md`（证据 7 的键形措辞勘误建议由主线程 append 记账，architect 不做登记）。
> **review 轨**：Track B。自评主路径（见下「判档自评」；判档归 architect-reviewer、只升不降）。Track A 不触发——方向已由 human 2026-10-23 裁「方案甲」收敛，判据 = 收敛度清晰。
> **变化性质**：**已验证行为取代未验证设计**——不是新设计、不是机制发明。本批把 `docs/architecture.md` §6.7 / §1.3 中「未经任何一轮验证、且上游插件不承载」的写法，纠正为探针轮端到端验证过（2026-10-22）、并由 2026-10-23 只读复核（插件源码 / schema / 生产配置状态）与部署轮实况共同支撑的实际接线行为，并把挡不住的残余逐行申报。
> **packet-binding（FROZEN 生效）**：packet-id = 本 PROPOSAL §2 改法集（T1–T8）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（纯文档面零实现改动；含隐私口径语义订正 → 不取 low，交独立评审 + landing-verifier 逐字核）。
> **review profile**：架构面（`docs/architecture.md` + `docs/decisions/ADR.md`）；G1 / G2 / G3 零触；契约面零触（`docs/contracts/**` 不改）。
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行在场（GREEN 前必须，FROZEN 期限；主线程标 FROZEN 前人工复核）。

```landing-manifest
docs/architecture.md
docs/decisions/ADR.md
docs/glossary.md
```

---

## 判档自评（轻 / 主）

| 轻路径判据（SSOT = `.claude/references/arch-change.md`「轻路径判据」） | 判定 | 依据 |
|---|---|---|
| ① 零语义变化 | **不满足** | 机制描述由「逐回合资格门（写读同门、非资格回合不写记忆、不注入画像）」改为「已验证接线的实际写入范围 + 逐行残余申报」——读侧画像注入、front 模型回复落库、QQ 家长消息落库为新增的语义记载 |
| ② 改动性质限于错字 / 排版 / 表述澄清 / 润色 / 锚行 / 指针 | 部分 | 表内多行为表述澄清，但改法含语义订正，不构成纯措辞 |
| ③ 不触 `docs/contracts/**` | 满足 | 零契约改动；契约 §四「会话连续性承载」与记忆口径无关 |
| ④ 不触 §1.2 / §2.2 / §2.3 / §5.x 视图结构 / §9 | §1.2 / §2.2 / §2.3 / §9 零触；§5.1.2 / §5.4 为行内措辞订正、非视图结构重组 | — |

**自评 = 主路径**（① 决定性否定）。若判官认为「把与 human 已裁方向不符的错误机制描述纠正为已验证行为」在① 的「机制 / 行为的含义」上不构成含义变化，可裁轻路径；本件已具备瘦制品五件的全部结构件（Status 行 / 改法清单 / 判档依据 / §7 评审区 / landing-manifest / 验证集），裁轻时不需改写。判档归判官、只升不降。

## 0. 想实现的效果与差距

**目标效果**：`docs/architecture.md` 的记忆小节如实描述「现在跑的是什么」，使冻结验收、需求面口径与实现现实三者不再互相骗——human 原则「验证过的优先，不发明新东西」在 spec 面落地。

**差距（一句话）**：§6.7 现文写的是一个从未被验证、上游插件也不提供的「资格门」；§1.3 写的是「设备级 peer（换设备 = 新 peer、旧画像不迁移）」，而探针轮实测的是「设备级 session + 家庭单 owner 画像」。两者在 2026-10-23 部署轮被证伪，须按实况改写。

**非目标（本批不做）**：

- 不触实现面与部署面——生产接线（安装插件 / 4 键配置 / `noisePatterns`）与 K3 冻结验收锚换代 = 批 B 独立 TaskPackage，不归本批；
- 不改需求面——`docs/requirements.md` S-F5.1 AC5 / 补充规则 / NFR-PRIV-1 ① 中与新口径存在张力的措辞，由 `/requirement-change` 另批同步（§3 列清单，本批只列不改）；
- 不改契约（`docs/contracts/**` 零触）；
- 不做甲+（vendor 上游插件打最小补丁）——human 已裁方案甲；残余实测不可接受时另立批；
- 不动历史证据——K3 TaskPackage、K3 冻结验收件、261022 PROPOSAL / landing-verdict / retro 均保持原文。

## 2. 问题、根因与改法（T1…T8）

**总根因（一次说清）**：探针轮的 debt 记录（证据 7）用了缩略写法——把 `plugins.entries.openclaw-honcho.hooks.allowConversationAccess` 写成 `hooks.allowConversationAccess`，把 session key `agent:kidpal:openai-user:<device_id>` 当成 peer 键。261022 架构批把缩略写法当成字面路径抄进声明工件与 §6.7 / §1.3，并据此设计了「逐回合资格门」；该能力上游插件从未提供、也从未被任何一轮验证。2026-10-23 部署轮实证：自研 carrier 三层失效（配置形状错 / 不是插件 / 调用的 Honcho 端点不存在），而探针验证过的官方插件接线行为与 §6.7 现文不符。

### T1 · §6.7 改写为「记忆写入范围 + 六行实况表 + 记忆身份」

**问题**：§6.7 现文以「资格门（唯一判据，出生即定）」开篇，断言「读侧同门——非资格回合不注入画像」「main agent 的 QQ / cron 回合、任何不携 `user` 的调用一律不入库」「设备流程触发的回合因此结构性不入画像」。已验证接线的实际行为与此三段全部不符（front 一次性回合写侧只挡 user 消息 / 读侧照常注入；QQ 回合消息照常落库；身份不是设备级 peer）。

**证据**（起草期现读，2026-10-23）：

- `docs/architecture.md:360` 现文逐字：「**资格门（唯一判据，出生即定）**：仅当回合来自 kidpal agent 的语音链路、且请求携带 `user=<device_id>` 时，该回合内容才可进入记忆管线（写 observation / 落消息）；**读侧同门**——非资格回合不注入画像。资格门**全局生效**（不只 kidpal）：main agent 的 QQ / cron 回合、任何不携 `user` 的调用一律不入库。设备流程触发的回合因此结构性不入画像，完整枚举（S-F5.1 AC5 的「完整枚举由架构文档定义」即本表）：」
- advisory §5 逐行差（`memory-wiring-advisory-20261023.md:120-127`）：第 1/2 行「写侧与读侧都漏」；第 3 行「已排除（cron session key 分类 + `captureSystemRuns=false`）」；第 5 行「照常入库；QQ 有 `senderId` → 新装默认 `per-sender` 策略会给家长单独 peer……但消息仍落库，且插件给 agent 注册了 `honcho_search_messages`」；设备语音行「peer = `owner`（不是 device 级 peer）」。
- advisory §3（`:102`）：「`openclaw-honcho` 的 capture 只排除 cron/心跳/系统回合（`helpers.js isSystemRun` + `captureSystemRuns=false` 默认），对『没有 user 字段的一次性回合』与『QQ 家长回合』没有过滤」——资格门不存在是插件能力面的事实，不是配置调错。
- advisory §6 方案甲（`:142`）：「`config.noisePatterns`（上游已文档化、schema 内）加两条锚定正则 `/^孩子(到目前为止)?讲的内容：/`，把 front 两条一次性 prompt 的 user 消息挡在库外；残余是 assistant 回复单条」。

**根因**：把探针配置的缩略写法当字面路径、把 session key 当 peer 键（见总根因），并据此发明了一个上游不具备的机制承诺。

**改法**：替换 §6.7 全节（`docs/architecture.md` :356–:379，自 `### 6.7 记忆写入资格与画像` 起、至「**降级**：……用户无感知）。」止）为下列文本（逐字；`历史 backfill` / `删除语义` / `降级` 三段保持原文不动）：

```
### 6.7 记忆写入范围与画像

**在哪侧：记忆子系统（§5.1.2）+ OpenClaw 侧插件接线（跨 front / 引擎 / OpenClaw 的部署面行为）。**

**记忆写入范围（以已验证行为为准）**：生产记忆接线 = OpenClaw 上游插件 `@honcho-ai/openclaw-honcho`（经 OpenClaw 自有插件管理器安装、pin 版本），采集（落消息 / 抽 observation）与注入（`before_prompt_build` 画像）按回合钩子**全局生效**——插件没有「仅携 `user` 的回合才入库」这类逐回合判据。2026-10-22 初版以「请求是否携 `user` 即是否入库」为唯一判据的写法，源于对探针配置缩略式的误读，未经任何一轮验证、也不被上游插件承载，本版按 2026-10-23 实况改写。实际边界由三件构成：① 写侧以插件 `config.noisePatterns` 挡 front 两条一次性 prompt 的 **user 消息**（配置键在上游插件 schema 内；接线生效随部署轮验收）；② 系统回合（cron / 心跳）由 `captureSystemRuns=false`（默认值）排除；③ 读侧画像注入**不区分回合**。因此「设备流程触发的回合不入画像」在本形态下**只对 user 侧输入成立**——模型侧回复落库与读侧注入是有申报的残余；完整枚举（S-F5.1 AC5 的「完整枚举由架构文档定义」即本表）：

| # | 回合 | 触发 / 承载 | 实际记忆行为与残余 |
|---|---|---|---|
| 1 | 卡壳引导回合 | front 一次性 LLM（`_openclaw_chat_once`，request 级 system 指令） | user 消息被 `noisePatterns` 挡、不入库；**模型回复仍落库**（残余；引导句不含孩子隐私）；**读侧仍注入孩子画像**（残余） |
| 2 | 忠实整理回合 | 同上 | user 消息（孩子原话）被 `noisePatterns` 挡；**整理产物仍落库**（残余；内容本身是孩子自己的故事，是否被 deriver 抽成 observation 随 P3 实测）；**读侧仍注入孩子画像**——整理回合可能被画像「带跑」（忠实性质量风险，真机抽检 3–5 例） |
| 3 | 提醒到点语义回合 | OpenClaw cron → **main** agent 产 `semantic.json` | 排除：cron 会话分类 + `captureSystemRuns=false`（默认） |
| 4 | 提醒三档机械脚本 | `reminder_schedule_lib.py` | 不适用（无 LLM 回合） |
| 5 | 家长 QQ 录入 / 查日志回合 | QQ 通道 → main agent | **消息照常落库**（残余）；家长回合按 `senderId` 归独立通道 peer（插件默认 `per-sender` 策略，不污染家庭 owner 画像）；家长内容可被 `honcho_*` 工具（如跨会话检索工具 `honcho_search_messages`）检索到的风险待部署轮实测，未闭合前按未闭合风险记账 |
| 6 | 故障 / 兜底固定文案 | front 固定安抚句 / 引擎 `system_error_response` | 不适用（无 LLM 回合） |

（记录语境期间引擎侧本就无 LLM 回合〔ADR-013 读法乙〕，不在枚举内。新回合形态入册前须实测其写 / 读行为，按实测入表——**以实测为准，不按设计意图推定**；front / QQ 两行的部署轮实测随验收包落盘，与本表不符时以实测为准回填。）

**记忆身份**：记忆库内 peer = 家庭单 `owner`（另有 agent 侧 `agent-kidpal`）；设备身份体现在 **session key 家族** `agent:kidpal:openai-user:<device_id>`——每设备一族会话，换设备 = 新 session 家族（会话连续性重置），**画像仍为家庭单 owner、不按设备隔离**（§1.3）。

**画像边界（AC3 / AC4 抽取边界，三层纵深）**：① **来源门** = 上表记忆写入范围（user 侧输入为主边界，模型侧回复与读侧注入的残余见各行申报）；② **抽取策略** = deriver 只抽「孩子世界的事实」（喜好 / 宠物 / 同学 / 经历），家长隐私信息（姓名 / 联系方式 / 家庭安排）不产 observation——策略承载首选 Honcho deriver 侧的提示配置；若自托管版本不支持自定义 deriver 提示，退回「持久化前消息侧过滤」；③ **注入边界** = representation 注入前的类别过滤（低置信 / 敏感类不回注）。验收判据 = seeded 负向探针集 100% 零命中（NFR-PRIV-1）；三层任一失守即不达标。P3 实测成形后回填本小节。

**历史 backfill**：2026-09-30 前 `session-corpus` 全量照记（human 2026-10-22 裁：不分回合性质、含第三方姓名）；幂等可重跑、前后计数报告、清库回滚（实现归 P4）。

**删除语义**：NFR-PRIV-1 —— 运维面手工删除记忆画像与 observation / 消息存储；不删 OpenClaw 完整日志、历史补录源语料、设备诊断日志（各按既有接受口径保留）。

**降级**：记忆服务不可用 → 本轮无画像注入、对话与回复照常（不阻塞、不静默重试风暴）；记忆失败不进 §6.3 的用户可见兜底（用户无感知）。
```

目标文件：`docs/architecture.md`（改）

### T2 · §1.3 记忆身份体系措辞纠正（设备级 peer → 设备级 session + 家庭单 owner 画像）

**问题**：§1.3 写「本批**设备级 peer** 先行（换设备 = 新 peer、旧画像不迁移）」。探针轮 Honcho 库内 peers 只有 `owner` 与 `agent-kidpal` 两个，没有任何 `agent:kidpal:openai-user:<device_id>` 形态的 peer；设备差异体现在 sessions 的 `openclawSessionKey`（session key 家族），不在 peer 层。按现文承诺，换设备应换画像——实测是同一家庭 owner 画像，承诺失真。

**证据**：

- `docs/architecture.md:55` 现文逐字：「- 多设备 / 多孩子**记忆身份体系**——本批**设备级 peer** 先行（换设备 = 新 peer、旧画像不迁移）；身份体系另立（§6.7 / OQ-6 留根）。」
- advisory §1.3（`memory-wiring-advisory-20261023.md:51-58`）：「**peers**：只有两个 —— `owner`、`agent-kidpal`。**没有** `agent:kidpal:openai-user:<device_id>` 这类 peer。」「**结论：探针验证的是「设备级 session + 家庭单 owner 画像」，不是「设备级 peer」。** debt 证据 7 里写的 `openclawSessionKey=agent:kidpal:openai-user:device-probe-honcho` 是 **Honcho session 的 metadata / OpenClaw 的 session key**，不是 peer 键。」

**根因**：把 session key 当 peer 键（总根因的第二个面）。「session key 家族」与「peer」在 Honcho 里是两类实体，初版混用。

**改法**：`docs/architecture.md:55` 整行替换为（逐字）：

```
- 多设备 / 多孩子**记忆身份体系**——本批不做；已验证形态 = **设备级 session**（session key 家族 `agent:kidpal:openai-user:<device_id>`）+ **家庭单 owner 画像**（Honcho peer = `owner`，不按设备隔离）；换设备 = 新 session 家族（会话连续性重置），画像不变；身份体系另立（§6.7 / OQ-6 留根）。
```

目标文件：`docs/architecture.md`（改）

### T3 · §4 NFR-PRIV-1 映射行同步

**问题**：§4 质量属性映射表 NFR-PRIV-1 行的架构对策仍写「记忆写入资格门（白名单入册）」。该机制不存在（T1），映射行会与 §6.7 新文自相矛盾。

**证据**：`docs/architecture.md:155` 现文逐字：「| NFR-PRIV-1 记忆边界与保留 | 记忆写入资格门（白名单入册）+ 抽取三层纵深（来源门 / 抽取策略 / 注入过滤）+ 自托管不出网 + 删除 = 运维手工 | §6.7 / §5.1.2 记忆子系统 / §5.4 记忆行 |」；`grep -c "资格门" docs/architecture.md` 基线 = 8（2026-10-23 实跑）。

**根因**：同一机制的投影句未随 T1 同步（261022 批同一节写入，本批须联动）。

**改法**：`:155` 整行替换为（逐字）：

```
| NFR-PRIV-1 记忆边界与保留 | 记忆写入范围（已验证接线：`noisePatterns` 挡 front 一次性回合 user 消息 + `captureSystemRuns=false` 挡系统回合；模型回复与读侧注入残余逐行申报）+ 抽取三层纵深（来源门 / 抽取策略 / 注入过滤）+ 自托管不出网 + 删除 = 运维手工 | §6.7 / §5.1.2 记忆子系统 / §5.4 记忆行 |
```

目标文件：`docs/architecture.md`（改）

### T4 · §5.1.2 两处括注同步（front 行 / 记忆子系统行）

**问题**：§5.1.2 front 行括注写「§6.7 资格门：非资格回合不写记忆、不注入画像」——与实测相反（读侧照常注入、模型回复照常落库）；记忆子系统行写「写入受 §6.7 资格门约束」，引用一个不存在的小节结论。

**证据**：

- `docs/architecture.md:192` 行内片段逐字：「**引导 / 整理一次性 LLM 回合不携 `user`**（§6.7 资格门：非资格回合不写记忆、不注入画像）；」
- `docs/architecture.md:203` 行内片段逐字：「写入受 §6.7 资格门约束；」
- `grep -c "受 §6.7 资格门约束" docs/architecture.md` 基线 = 2（2026-10-23 实跑；:203 与 :283）。

**根因**：投影句把发明物当成既有结论引用。

**改法**：两处片段各自原样替换（逐字）：

`:192` 中——

```
**引导 / 整理一次性 LLM 回合不携 `user`**（§6.7 记忆写入范围：user 消息不入库，模型回复与读侧画像注入为已申报残余）；
```

`:203` 中——

```
写入范围与残余见 §6.7；
```

目标文件：`docs/architecture.md`（改）

### T5 · §5.4 记忆·会话消息行同步

**问题**：数据视图「记忆·会话消息」行写者列写「（每轮 persist，受 §6.7 资格门约束）」，同上。

**证据**：`docs/architecture.md:283` 行内片段逐字：「OpenClaw Honcho 插件（每轮 persist，受 §6.7 资格门约束）」。

**根因**：同 T4。

**改法**：`:283` 行内片段原样替换为（逐字）：

```
OpenClaw Honcho 插件（每轮 persist；写入范围与残余见 §6.7）
```

目标文件：`docs/architecture.md`（改）

### T6 · §12 变更记录追加行（changeId = 本批冻结点）

**问题**：§12 是「已冻结」的消费判据（`launchPreconditions` 消费 = 对应 changeId 行在场），本批若不追加行，架构面变更对下游不可见。

**证据**：`docs/architecture.md:483` 纪律句「本文件每次改动都在下表追加一行（新行在上，倒序）——changeId、日期、改了哪些节、依据」；现行首行 = `261022-device-memory-prompt-overhaul`（`:487`）。

**根因**：流程义务，非缺陷。

**改法**：在 §12 表格首行（`261022-device-memory-prompt-overhaul` 行之上）插入（逐字）：

```
| 261023-arch-review-memory-wiring-verified-behavior | 2026-10-23 | §1.3（记忆身份体系实测口径：设备级 session + 家庭单 owner 画像）/ §4（NFR-PRIV-1 映射行口径）/ §5.1.2（front 行括注、记忆子系统行）/ §5.4（记忆·会话消息行）/ §6.7 改写（逐回合判据 → 记忆写入范围 + 六行实况表 + 残余申报 + 记忆身份） | delivery/review/261023-arch-review-memory-wiring-verified-behavior/PROPOSAL.md（依据：human 2026-10-23 方案甲裁决 + 261022 批 advisory memory-wiring-advisory-20261023 §1/§5/§6；K3 冻结验收与生产接线换代归批 B） |
```

目标文件：`docs/architecture.md`（改）

### T8 · glossary「记忆子系统」词条描述语同步（R1-F1）

> **节序说明**：本 T 物理置于 T7 之前——`parseSection2AnchorLists` 在 T7 围栏内 `## ADR-016` 行处截断 §2 节域（见 T7 锚行前置说明），置于 T7 之后将不被解析、preflight 假红。编号沿用评审 F1 的「T8」指称，不重排。

**问题**：`docs/glossary.md:32`「记忆子系统」词条的描述语写「写入资格与边界见 arch §6.7」。§6.7 标题换代为「记忆写入范围与画像」后，「资格」语面指向本批明确退役的机制名；节号指针虽成立，但描述语属把 §6.7 结论投影到别处的句子，同步标准与 T3 / T4 / T5 同等（起草期初版 §3 判「指向仍成立」= 误判，R1-F1 驳回）。

**证据**：`docs/glossary.md:32` 现文逐字：「**记忆子系统**：云端自托管的 Honcho 服务栈（ECS 本机五容器、端口全 loopback），承担消息 / 会话存储、observation 抽取（deriver）与画像表示（representation），在 prompt 构建前注入画像；写入资格与边界见 arch §6.7，语义权威 = requirements NFR-PRIV-1，出网记账 = NFR-SEC-2 ⑤（261022-device-memory-prompt-overhaul）。」（起草期现读，2026-10-23）

**根因**：同 T3–T5——投影句未随 §6.7 换代同步，且初版 census pattern 未覆盖「写入资格」语面（见 §3 完备性先验勘误）。

**改法**：该词条内片段原样替换（逐字）：

```
写入范围与残余见 arch §6.7
```

（词条其余文字不动；末尾溯源注记 `（261022-device-memory-prompt-overhaul）` 保留。）

目标文件：`docs/glossary.md`（改）

### T7 · ADR-016 追加（supersedes ADR-014 ②③）

**问题**：`docs/decisions/ADR.md` ADR-014 的决策 ②「记忆身份 = **设备级 peer**（`user=<device_id>` → `agent:kidpal:openai-user:<device_id>`）」与 ③「**资格门** = 仅 kidpal 语音链路且携 `user` 的回合可读写记忆」与本批 canonical 纠正直接冲突。ADR 为 append-only（文件头：「append-only：不覆盖、不改写既有条目；推翻旧决策 = 新增一条并注明 supersedes ADR-XXX」），不能就地改 ADR-014。

**证据**：`docs/decisions/ADR.md:119-123`（ADR-014 标题与决策 ②③，起草期现读）；ADR 文件头 :5 写入者与 append-only 约定。

**根因**：261022 批把同一错误写进两级制品（canonical spec + ADR）；只改 spec 会把错误留在决策账上，authority ladder 第 4 级与第 2 级长期冲突。

目标文件：`docs/decisions/ADR.md`（改）

> 锚行前置说明：下面的待追加文本围栏块首行是 `## ADR-016` 标题形态；`parseSection2AnchorLists` 的 §2 边界按行首 `## ` 截断、**不跳围栏**（`landing-fidelity-check.mjs --preflight` 实跑 2026-10-23 证：锚行置围栏后会被截出 §2、误报缺锚清单）。故本 T 的锚行置于围栏前——形态与机械门同形，内容不变。

**改法**：在 `docs/decisions/ADR.md` 文末追加一条（逐字；append-only，既有条目零改）：

```
## ADR-016 · 记忆身份与写入口径以已验证接线为准：设备级 session + 家庭单 owner 画像（supersedes ADR-014 ②③）

- **日期**：2026-10-23（`261023-arch-review-memory-wiring-verified-behavior` 架构面 Track B）
- **决策**：① 记忆身份 = **设备级 session**（session key 家族 `agent:kidpal:openai-user:<device_id>`）+ **家庭单 `owner` 画像**；设备之间不隔离画像（换设备 = 新 session 家族，画像不变）。② 写入范围 = **已验证接线的实际行为**：front 一次性回合的 user 消息以插件 `noisePatterns` 挡、模型回复落库（残余）；cron / 心跳由 `captureSystemRuns=false` 排除；QQ 家长消息落库、按 `senderId` 归独立通道 peer，`honcho_*` 工具可检索风险待实测；读侧画像注入不区分回合（残余）。残余逐行申报于 arch §6.7。③ 弃用「逐回合资格门」表述与 K3 自研 carrier 路线。
- **理由**：human 2026-10-23 裁「验证过的优先，不发明新东西」。探针轮端到端验证的是「设备级 session + 家庭单 owner 画像」；资格门从未被任何一轮验证、上游插件亦不提供（advisory §3/§5）。把已验证行为如实记账，优于维持一个永不满足的冻结口径。
- **备选与放弃原因**：甲+（vendor 上游插件打约 10 行最小补丁保资格门字面）——放弃：本批遵循 human 原则不改已验证物，残余实测不可接受时另立补丁批；修 K3 自研 carrier——放弃：三层失效（配置形状错 / 不是插件 / 调用的 Honcho 端点不存在）。
- **关联**：arch §1.3·§4·§5.1.2·§5.4·§6.7；ADR-014（本 ADR supersede 其 ②③，①④⑤⑥⑦ 不变）·ADR-015；requirements NFR-PRIV-1 / S-F5.1 AC5（需求面措辞同步另批）；证据 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/memory-wiring-advisory-20261023.md。
```

## 3. 影响面 sweep 分类（UNCLASSIFIED = 0）

| 面 | 文件 / 节 | 类别 | 说明 |
|---|---|---|---|
| 架构 spec | `docs/architecture.md` §1.3 / §4 / §5.1.2 / §5.4 / §6.7 / §12 | 改 | 本批 T1–T6 |
| 决策记录 | `docs/decisions/ADR.md`（文末追加） | 追加 | T7 ADR-016；ADR-014 原文零改（append-only） |
| 需求面 | `docs/requirements.md` S-F5.1 AC5（:598）/ 补充规则（:601）/ 可行性行（:603）/ NFR-PRIV-1 ①（:654） | 未触（需另批） | 与本批新口径存在张力的措辞清单如下；走 `/requirement-change`，时序见 OQ-1 |
| 术语表 | `docs/glossary.md`（:32「记忆子系统」词条） | 改 | T8——描述语「写入资格与边界见 arch §6.7」同步为「写入范围与残余见 arch §6.7」（R1-F1） |
| 契约面 | `docs/contracts/**` | 未触 | 会话连续性承载（ADR-015）与记忆写入口径无关；零契约 diff |
| 实现 / 部署面 | `server/deploy/261022-openclaw-honcho-plugin/**`、`server/deploy/manifest.yaml`、`server/acceptance/test_261022_memory_isolation.acceptance.py` | 未触（批 B） | 生产接线 + K3 冻结验收锚换代归批 B 独立 TaskPackage；本批 §6.7 为批 B 的口径依据。**锚行协调注记（R1-F5）**：批 B `delivery/taskpackages/261023-honcho-memory-wiring-restoration/input-brief.md:16` 的 §6.7 锚行（「6.7 记忆写入资格与画像 | 356」）在本批落地后由批 B planner 于包内刷新为「6.7 记忆写入范围与画像」+ 新行号，不改其冻结语义 |
| 历史证据 | `delivery/taskpackages/261022-m5-k3-honcho-memory-production/**`、`delivery/review/261022-arch-review-device-memory-prompt-overhaul/**` | 未触 | 冻结证据 append-only，不改写；K3 包内「device 级 peer / peer 键不另造」表述由批 B 收口时在包内记账 |
| debt 账本 | `docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md` | 未触 | 证据 7 的键形措辞勘误建议由主线程 append；architect 不做登记（登记动作 = 主线程） |
| 图索引快照 | `.codebase-memory/**` | 未触 | 机器再生面 |

**需求面张力清单（本批只列不改）**——四处逐字与建议处置：

1. `docs/requirements.md:598` AC5：「When 回合发生，Then 其内容不进记忆画像」→ 建议改为「其 user 侧输入不进画像；模型侧回复与读侧注入的残余见架构文档 §6.7」。
2. `docs/requirements.md:601` 补充规则：「设备流程触发的回合不入画像、历史语料照记（含第三方姓名）见 AC5……设备流程回合不入画像的边界自记忆子系统上线起对新增回合生效」→ 同口径收窄。
3. `docs/requirements.md:654` NFR-PRIV-1 ①：前半句「设备自身流程触发的回合不进画像（现知含故事整理、卡壳引导等，完整枚举见 S-F5.1 AC5 与架构文档）」→ 阈值措辞按新口径收窄（枚举权威仍在架构文档）。**同条后半句「家长隐私信息……不进记忆画像、不被复述」亦有张力（R1-F4）**：T1 残余第 5 行（QQ 家长消息照常落库 + 插件注册 `honcho_search_messages` 跨会话检索风险，OQ-3 未闭合前按未闭合风险记账）意味着「不被复述」的保障暂非结构性的；该半句的收口随 OQ-3 裁定一并由需求面批处理，本批只挂账不预判。
4. `docs/requirements.md:603` 可行性行「🟡（AC1/AC2 已实证……AC3/AC4 的抽取边界与 AC5 的机械回合隔离待架构面定形，随实现包验收）」（R1-F2）：「AC5 的机械回合隔离待架构面定形」恰是本批退役的承诺形态（机械隔离不存在）→ 建议改为「AC5 边界已由架构文档 §6.7 定形（含残余申报），随实现包验收」。

⚠️ **评审注意（撞车面）**：本批改法落地后、需求面同步批落地前，canonical spec 与 NFR-PRIV-1 阈值存在**已知且已申报**的张力。这不是遗漏，是跨面批的时序结果（OQ-1）；不得以「与本批无关」略过，也不得在本批直接改需求面（需求面归 `/requirement-change`，阈值归 requirements）。该需求面批**须与 `docs/debt/requirement/REQ-261023-family-arrangement-scope-clarification.md` 合并起草**（R1-F3：同触 :654；该条目 status = deferred、trigger = 下一次需求面变更批）。

**【硬项 · sweep 透镜义务】**：本批不新增门 / 断言 / 判定 pattern → 该项不适用（一句声明；不造假绿）。

**【硬项 · 复扫模式完备性先验】**（R1-F1 勘误后）本批退役的语面 = ①「资格」语面：`资格门` / `记忆写入资格门` / `记忆写入资格与画像` / `写入资格与边界`（后两类只有把模式扩到「记忆写入资格\|写入资格」才能命中）；②「设备级 peer」：`设备级 peer` / `设备级peer` / `device 级 peer` / `device 级peer`；③`白名单入册`。census 命令（2026-10-23 实跑；排除 `docs/archive/` 与 `docs/notes/`）：

```
grep -rn "资格门\|记忆写入资格\|写入资格\|设备级 *peer\|device *级 *peer\|白名单入册" --include=*.md docs/
```

命中：`docs/architecture.md` :55 / :155 / :192 / :203 / :283 / :356 / :358 / :360 / :371 / :373 / :487；`docs/glossary.md` :32；`docs/decisions/ADR.md` :119 / :122 / :123。**勘误记录（如实）**：初版 census 只列「资格门 / 设备级 peer / 白名单入册」四语面，该 pattern **命不中** `:487`（含「记忆写入资格与画像」）与 glossary :32（含「写入资格与边界」）；R1-F1 指出后按上文更宽 pattern 复核，并据此弃用初版「glossary 指向仍成立」的判读、新增 T8。**完备性论证**：该语面只作中文散文成句使用，无英文别名、无标识符形态（非代码常量、非配置键），机器面（`.claude/scripts` / hooks / verify-gates）零使用——本批不触机器面。处置：`docs/architecture.md` 命中由 T1–T6 改写或替换；`docs/glossary.md:32` 由 T8 替换；`docs/decisions/ADR.md` 命中为 append-only 历史条目，由 T7 新增 ADR-016 显式 supersede、不改写原文；`:487` 历史行按「历史记录类文件一律 append，不覆盖既有行」保持原文——因此落地后 `记忆写入资格与画像` 在全仓仍有 1 处（历史 changelog），见 §5 第 6 条。

## 4. human 取舍点（OQ）

各 OQ 请 human 在立项 ack 时或评审中裁，裁定与依据落本件本节。

### OQ-1 · 需求面同步的时序

- (a) **arch 批先落、紧随另起 `/requirement-change` 批**同步 AC5 / NFR-PRIV-1 措辞（**推荐**）——架构文档是 AC5 枚举的权威落点，需求批措辞会引用新 §6.7；arch 先落避免需求批引空锚。代价 = 中间窗口内需求阈值与 arch 表述不一致（窗口应尽量短）。
- (b) 两批并行、同期落地——无窗口，但两批互引、FROZEN 节奏需对齐。
- (c) 暂不同步、登记 debt accept-as-risk——口径长期不一致，不推荐。

附条件（R1 评审意见，随 OQ-1 一并交 human 裁）：若选 (a)/(b)，该需求批须按 R1-F2 覆盖 `:603` 可行性行、按 R1-F3 与 `REQ-261023-family-arrangement-scope-clarification` 合并起草、按 R1-F4 覆盖 NFR-PRIV-1 ①「不被复述」半句。

### OQ-2 · ADR-016 是否同批携带

- (a) **同批携带（推荐；R1 评审同意）**——canonical 与决策账同步，authority ladder 第 4 级无冲突残留（manifest 含 `docs/decisions/ADR.md`）。
- (b) 另批补 ADR——本批 manifest 裁掉 ADR 路径；ADR-014 ②③ 与 canonical 冲突在册，登记一条留根。

### OQ-3 · 家长 QQ 内容可被 `honcho_*` 工具检索的处置（归批 B 部署轮实测后）

- (a) 部署轮实测 `tools.allow` 是否管得住插件注册工具；管不住则 `tools.deny`（**推荐**，按实测定）——advisory §5 末段指出插件工具有独立 allowlist 通道，很可能不受 `tools.allow` 约束。
- (b) 不实测、直接 deny——稳但可能砍掉有用工具；
- (c) accept-as-risk——需 human 明示。

### OQ-4 · `noisePatterns` 部署轮实测失效时的回退

- (a) **以实测回填 §6.7**（推荐）——spec 继续如实，残余扩大即照实扩表；
- (b) 升甲+（vendor 上游插件打最小补丁）保字面——新增代码面，须另立批。

## 5. 验证集（landing 前机械前置 + landing 后核验）

每条标评估时点。数字基线 = 2026-10-23 起草期实跑（§5 末「起草期实证留痕」）。

1. **pre-commit · PROPOSAL 形态自检**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261023-arch-review-memory-wiring-verified-behavior/PROPOSAL.md` → `ok:true`、`anchorUnion` 恰 = `["docs/architecture.md","docs/decisions/ADR.md","docs/glossary.md"]`、`findings` 为空（零 `bold-anchor-line`、零缺锚）。
2. **post-commit · 锚完整性**：`node .claude/scripts/spec-anchor-check.mjs` → `ok:true` / `errors:[]`（基线：`ok:true` / `storyCount 14` / `anchorLines 36`）。
3. **post-commit · 退役语面清账**：`grep -c "资格门" docs/architecture.md` → `0`（基线 `8`）。
4. **post-commit · 身份措辞**：`grep -c "设备级 peer" docs/architecture.md` → `0`（基线 `1`）；`grep -c "设备级 session" docs/architecture.md` → `≥2`（基线 `0`）。
5. **post-commit · 投影句**：`grep -c "受 §6.7 资格门约束" docs/architecture.md` → `0`（基线 `2`）。
6. **post-commit · 标题换代**：`grep -c "记忆写入资格与画像" docs/architecture.md` → `1`（只应剩 :487 历史行；新标题 = `### 6.7 记忆写入范围与画像`，须 `grep -c "### 6.7 记忆写入范围与画像"` → `1`）；全仓同语面 `grep -rn "记忆写入资格\|写入资格" --include=*.md docs/ | grep -v "^docs/archive/" | wc -l` → `1`（唯一残留 = :487 历史行；glossary 已由 T8 清账）。
7. **post-commit · glossary 投影句**：`grep -c "写入资格与边界" docs/glossary.md` → `0`（基线 `1`）；`grep -c "写入范围与残余见 arch §6.7" docs/glossary.md` → `1`（基线 `0`）。
8. **post-commit · 决策账 append-only**：`grep -c "^## ADR-016" docs/decisions/ADR.md` → `1`（基线 `0`）；`git diff --numstat docs/decisions/ADR.md` 删除行 = `0`。
9. **post-commit · 变更记录钉点**：`grep -c "261023-arch-review-memory-wiring-verified-behavior" docs/architecture.md` → `≥1`（§12 行在场）。
10. **post-commit · 未改段落保全**：`git diff docs/architecture.md` 中 `历史 backfill` / `删除语义` / `降级` 三段零增删（§6.7 重写时逐字带过）；`git diff --numstat docs/glossary.md` 应为 `1` 增 `1` 删（只动 :32 一词条内片段）。
11. **post-commit · 人读逐字核（landing-verifier）**：T1 全节与 PROPOSAL 围栏块字节对账；T2–T5 / T8 五处 old→new 逐字；T6 / T7 追加行逐字。

## 6. 留根清单

- **需求面同步（OQ-1）**：另批 `/requirement-change` 改 AC5 / 补充规则 / 可行性行（:603，R1-F2）/ NFR-PRIV-1 ①（含「不被复述」半句，R1-F4）；未落前张力在册（§3 明示）。**该需求批须与 `docs/debt/requirement/REQ-261023-family-arrangement-scope-clarification.md` 合并起草（R1-F3：同触 :654；该条目 status = deferred、trigger = 下一次需求面变更批）。**
- **批 B（实现面）**：生产接线（插件安装 + 4 键配置 + `noisePatterns`）、K3 冻结验收锚换代（advisory §7 清单）、`honcho_*` 工具暴露实测（OQ-3）、`noisePatterns` 生效实测（OQ-4）——均归批 B 独立 TaskPackage，本批只提供口径依据。**锚行协调（R1-F5）**：批 B `delivery/taskpackages/261023-honcho-memory-wiring-restoration/input-brief.md:16` 的 §6.7 锚行由批 B planner 在包内刷新（旧标题 / 行号 → 新标题 + 新行号），不改其冻结语义。
- **ADR-014 ②③**：由 T7 ADR-016 显式 supersede；若 OQ-2 选 (b)，冲突留账。
- **debt 证据 7 措辞勘误**：建议主线程在批 B 收口时 append 一条（键形 `plugins.entries.openclaw-honcho.hooks.allowConversationAccess`、session key ≠ peer 键）。
- **K3 TaskPackage 内「device 级 peer / peer 键不另造」表述**：历史证据不改，批 B 收口记账。
- **残余风险观测**：整理回合被画像「带跑」的真机抽检（3–5 例）归 P3 实测；QQ 消息落库残余随 OQ-3。
- **回滚**：纯文档改动——回滚 = `git revert <本批合并 commit>`；无数据迁移、无部署面副作用。ADR-016 为文末追加条目，revert 后 ADR-014 原文自然复效，无中间态。

## 附录一 · 起草期实证留痕（author 区，非评审结论）

- 现读（本次会话）：`docs/architecture.md` :1–:492 全篇（重点 :55 / :155 / :192 / :203 / :283 / :356–:379 / :398–:417 / :487）、`docs/requirements.md` :583–:605 与 :648–:660、`docs/decisions/ADR.md` :1–:5 与 :111–:130、`docs/glossary.md` :12 / :31 / :32、`docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md` :1–:40、`delivery/review/261022-arch-review-device-memory-prompt-overhaul/memory-wiring-advisory-20261023.md` 全文、`docs/templates/PROPOSAL.template.md` 全文、`.claude/references/arch-change.md` 全文、`.claude/scope-map.json` architect 条目。
- 实测命令与基线输出：`grep -c "资格门" docs/architecture.md` → `8`；`grep -c "设备级 peer"` → `1`；`grep -c "设备级 session"` → `0`；`grep -c "受 §6.7 资格门约束"` → `2`；`grep -c "记忆写入资格与画像"` → `2`；`grep -c "^## ADR-016" docs/decisions/ADR.md` → `0`；`node .claude/scripts/spec-anchor-check.mjs` → `ok:true` / `storyCount:14` / `anchorLines:36` / `errors:[]`。
- preflight 实跑（R1 折返修订后，2026-10-23 同会话执行，逐字）：

```
$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261023-arch-review-memory-wiring-verified-behavior/PROPOSAL.md
{
  "ok": true,
  "gate": "landing-preflight",
  "proposal": "delivery/review/261023-arch-review-memory-wiring-verified-behavior/PROPOSAL.md",
  "section2BodyFound": true,
  "anchorCount": 3,
  "anchorUnion": [
    "docs/architecture.md",
    "docs/decisions/ADR.md",
    "docs/glossary.md"
  ],
  "advisories": [],
  "findings": [],
  "note": "只读干跑：只保证 §2 锚清单「形态可解析」，不做 §2↔manifest 对账；不参与任何门判定。"
}
```

  首跑（v0.1）曾报 `缺锚清单：T7 …`（exit 1）——根因 = `parseSection2AnchorLists` 的 §2 边界按行首 `## ` 截断且不跳围栏，T7 围栏内的 `## ADR-016` 标题行把锚行截出 §2；已按「锚行前置」修正（T8 同理物理前置，均在件内留说明）。R1 折返后复跑得上述 ok（anchorCount 2 → 3）。
- 基线补充（R1-F1 宽 pattern 复核，2026-10-23 实跑）：`grep -c "写入资格与边界" docs/glossary.md` → `1`；`grep -rn "资格门\|记忆写入资格\|写入资格\|设备级 *peer\|device *级 *peer\|白名单入册" --include=*.md docs/`（排除 archive / notes）按文件计 = `docs/architecture.md` 11 处 / `docs/decisions/ADR.md` 3 处 / `docs/glossary.md` 1 处。
- 未实测即标未实测的断言：`noisePatterns` 的拦截行为（键在插件 schema 内已证、行为未实测——批 B 部署轮验）；QQ `per-sender` 默认策略与 `honcho_*` 工具可达性（advisory 代码读证，未实测——批 B 验）。

---

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · architect-reviewer 评审（2026-10-23）

**判官**：architect-reviewer（作者 ≠ 审者）。**评审基线**：地面真值亲核——architecture.md :55/:155/:192/:203/:283/:356–:379/:483–:487 现读；requirements.md :594–:605/:650–:658 现读；ADR.md :1–:8/:115–:130 现读；advisory §1/§3/§5/§6 引文逐字比对；glossary :32 现读；批 B `delivery/taskpackages/261023-honcho-memory-wiring-restoration/`（plan.md / taskpackage.json / input-brief.md / launch-card.md）现读；`docs/debt/requirement/REQ-261023-family-arrangement-scope-clarification.md` 现读；退役语面全仓 census 复跑（`grep -rn "资格门\|设备级 *peer\|device *级 *peer\|白名单入册\|记忆写入资格" --include=*.md docs/`）——命中面与 §3 census 一致（architecture.md :55/:155/:192/:203/:283/:356/:358/:360/:371/:373/:487 + ADR.md :119/:122/:123），另见 F1 的 glossary 语面漏项。基线计数亲测复核：`资格门` = 8、`设备级 peer` = 1、`受 §6.7 资格门约束` = 2、`记忆写入资格与画像` = 2——与 §5 验证集标注全部相符。

#### 四查结论

1. **意图保真 · 过**。T1–T7 忠实于 human「方案甲 = 已验证行为 + 残余如实申报」：T1 六行实况表与残余申报逐行对得上 advisory §5 逐行差与 §6 方案甲语义（第 1/2 行读侧注入 + 模型回复落库、第 3 行 captureSystemRuns=false、第 5 行 QQ 落库 + per-sender + `honcho_search_messages` 风险、设备语音行 peer=`owner`）；T2 与 advisory §1.3「设备级 session + 家庭单 owner 画像」结论一致；甲+ 只出现在「备选与放弃原因」（T7）与 OQ-4(b)，未被夹带为改法；未实测断言（`noisePatterns` 行为、QQ `per-sender` 默认策略、`honcho_*` 可达性）在起草期实证留痕如实标未实测。无借机夹带新设计。
2. **纪律合规 · 过（一项见 F1）**。ADR append-only 纪律（T7 文末追加、ADR-014 原文零改、supersede 注记形态与文件头约定同形）；§12 changeId 行追加（T6）；「决策本体归 ADR.md」「需求与阈值归 requirements.md（本批只列不改）」「契约零触」均成立；author 未落任何 canonical（本评审时点 docs/architecture.md 与 ADR.md 与改前逐字一致，已实测比对关键行）。
3. **撞车复核 · 三项见 F3/F4/F5**。与批 B 分工自洽（批 B 零 canonical spec/contracts 改动、spec 口径修正归面 C——taskpackage.json `docsSyncReason` 原文证实）；与 REQ-261023-family-arrangement-scope-clarification 的交叉本批未挂账（F3）；requirements 张力清单有漏项（F2/F4）。
4. **牵连面完整 · 一项见 F1**。§5.1.2/§5.4/§4:155 投影句已由 T3/T4/T5 找齐；glossary「记忆子系统」词条的描述语投影漏同步（F1）；反向锚牵连成立——requirements S-F5.1 锚行引 arch §5.1/§1.2，本批不改节结构，spec-anchor-check 基线 ok:true 可维持。

#### 判档终裁：**主路径**

轻路径判据①（零语义变化）决定性不满足——「逐回合资格门」→「记忆写入范围 + 逐行残余申报」改的是机制含义本身，残余申报（模型回复落库、读侧注入不区分回合、QQ 消息落库）是新增语义记载，author 自评主路径成立，判官复裁一致，无升档事由（方向已由 human 2026-10-23 裁甲收敛，Track A 判据①②均不触发）。主路径全套制品义务照常。

#### Findings（verdict: BLOCK(F1,F2,F3,F4,F5)——全部为 author 可修的改法集/清单补全项，无一动摇方向；修毕复跑预检即可复审判 GREEN）

- **F1（REBUT §3 术语表行「指向仍成立」；牵连面漏项）**：`docs/glossary.md:32`「记忆子系统」词条含描述语「**写入资格与边界**见 arch §6.7」——§6.7 标题由「记忆写入资格与画像」换代为「记忆写入范围与画像」后，该描述语指向一个本批明确退役的机制名（「资格」语面）。节号指针成立，但本批对 :155/:192/:203/:283 四处投影句的同步标准（T3/T4/T5）应同等适用于 glossary 词条描述语——它同样是把 §6.7 结论投影到别处的句子。修法（二选一，推荐 a）：(a) landing-manifest 增 `docs/glossary.md` + 改法集增 T8（描述语改为「写入范围与残余见 arch §6.7」一类，一行改动）；(b) §3 该行改判「需另批」并在 §6 留根。附带勘误：§3 复扫模式完备性先验的 census 命令（四语面 pattern）与命中清单不一致——`:487`（含「记忆写入资格与画像」）与 glossary 描述语（含「写入资格与边界」）均不被所列 pattern 命中，实际执行用过更宽语面；应把 pattern 补为含「记忆写入资格\|写入资格」并如实记录，否则 §5 验证集 6（`记忆写入资格与画像` → 1）的退役论证不完备。
- **F2（§3 需求面张力清单漏 :603）**：§3 sweep 表需求面行列了四处（S-F5.1 AC5 :598 / 补充规则 :601 / 可行性行 :603 / NFR-PRIV-1 ① :654），随后的「需求面张力清单」只列三处——:603（「AC5 的机械回合隔离待架构面定形，随实现包验收」）被表格点名后从清单脱落，而「机械回合隔离」恰是本批 T1 明示退役的承诺形态。修法：张力清单补第四处（建议措辞：「可行性行的『AC5 的机械回合隔离待架构面定形』→ 改为『AC5 边界已由架构文档 §6.7 定形（含残余申报），随实现包验收』」），或说明排除理由。
- **F3（REQ debt 撞车未挂账）**：`docs/debt/requirement/REQ-261023-family-arrangement-scope-clarification.md`（status: deferred，trigger = 下一次需求面变更批）与本批 OQ-1 的需求面同步批**同触 `docs/requirements.md:654`（NFR-PRIV-1 ①）**——两处待改一字面，未来 `/requirement-change` 批须合并起草，否则同节两改互撞。§3 与 §6 留根清单均未交叉引用该条目。修法：§6 留根「需求面同步（OQ-1）」行补一句「与 REQ-261023-family-arrangement-scope-clarification 合并起草（同触 :654）」。
- **F4（NFR-PRIV-1 ①「不被复述」张力未列）**：:654 ① 还有半句「家长隐私信息……不进记忆画像、**不被复述**」——T1 残余第 5 行（QQ 家长消息照常落库 + 插件注册 `honcho_search_messages` 跨会话检索、OQ-3 未闭合前按未闭合风险记账）与「不被复述」存在潜在张力（检索工具若暴露，家长内容存在被复述给孩子的通道）。§3 张力清单第 3 条只引了 ① 的「设备自身流程触发的回合不进画像」半句。修法：张力清单第 3 条扩为两句——补注「①『不被复述』与 §6.7 残余第 5 行（QQ 落库 + `honcho_*` 可检索风险，OQ-3）的张力随 OQ-3 裁定一并由需求面批收口」，使未来需求批不漏此半句。
- **F5（批 B input-brief 锚行协调，非 canonical 冲突）**：批 B `delivery/taskpackages/261023-honcho-memory-wiring-restoration/input-brief.md:16` 锚行 =「docs/architecture.md | §6.7 | **6.7 记忆写入资格与画像** | 356 | ok」——本批落地后该锚行标题与行号均过期（新标题 + §6.7 重写）。批 B 冻结语义已明言「spec 口径修正归面 C、本包零 canonical 改动」，依赖方向无冲突；但批 B planner 在跑，input-brief 属批 B 自家制品（planner 可改），本批落地时序会撞上批 B 在途。修法：§3 sweep 表「实现 / 面部署」行或 §6 留根批 B 行补一句协调注记——「批 B input-brief.md:16 的 §6.7 锚行在本批落地后由 planner 于批 B 包内刷新（旧标题/行号 → 记忆写入范围与画像），不改其冻结语义」。

#### OQ reviewer 意见（裁定权在 human）

- **OQ-1**：同意 (a)（arch 批先落、紧随 `/requirement-change`）——架构文档是 AC5 枚举权威落点，需求批措辞须引新 §6.7；附条件：该需求批按 F2 补 :603、按 F3 与 REQ-261023-family-arrangement 合并起草、按 F4 覆盖「不被复述」半句，窗口内张力已由 §3 ⚠️ 段如实申报，可接受。
- **OQ-2**：同意 (a)（ADR-016 同批携带）——authority ladder 第 4 级与第 2 级不得留已知冲突；manifest 已含 `docs/decisions/ADR.md`，无增量成本；另批方案 (b) 会在 ADR-014 ②③ 与 canonical 之间开一个无人值守的冲突窗口，不取。
- **OQ-3 / OQ-4**：归批 B 部署轮，无评审异议；OQ-3 推荐 (a)（先实测 `tools.allow` 是否约束插件工具、管不住再 deny）与 advisory §5「pluginToolAllowlist 独立通道」读证一致；OQ-4 推荐 (a)（以实测回填 §6.7）与「以实测为准，不按设计意图推定」的表内纪律自洽。

verdict: BLOCK(F1,F2,F3,F4,F5)

---

## 附录二 · R1 折返修订记录（author，2026-10-23，v0.1 → v0.2）

R1 verdict = BLOCK(F1–F5)，全部为改法集 / 清单补全项，方向零动摇。逐条处置：

- **F1** → 新增 T8（`docs/glossary.md:32`「记忆子系统」词条描述语 → 「写入范围与残余见 arch §6.7」）；manifest 增 `docs/glossary.md`；§3 术语表行改判「改」；§3 census pattern 扩为「记忆写入资格\|写入资格」并如实记录初版 pattern 命不中 :487 与 glossary :32 的勘误；§5 验证集 1/6/7/10/11 同步调整（anchorUnion 三路径、glossary 清账、全仓残留 1 处 = :487 历史行）。
- **F2** → §3 张力清单补第 4 条（`:603` 可行性行「AC5 的机械回合隔离待架构面定形」= 本批退役的承诺形态）。
- **F3** → §6「需求面同步（OQ-1）」行补 REQ-261023-family-arrangement-scope-clarification 合并起草交叉引用（同触 :654）；OQ-1 附条件同步。
- **F4** → §3 张力清单第 3 条补 NFR-PRIV-1 ①「不被复述」半句（与 T1 残余第 5 行 QQ 检索风险 / OQ-3 关联，随 OQ-3 由需求面批收口）。
- **F5** → §3「实现 / 部署面」行与 §6「批 B」行补 input-brief.md:16 锚行协调注记（批 B planner 包内刷新，不改冻结语义）。
- 复跑留痕：preflight `ok:true` / `anchorCount:3` / `findings:[]`；`spec-anchor-check` `ok:true` / `errors:[]`（基线不变）。


### R2 · architect-reviewer 复核（2026-10-23，聚焦 F1–F5 修订到位性 + 新矛盾扫描）

**判官**：architect-reviewer（同 R1 实例；作者 ≠ 审者）。**复核基线（地面真值亲核）**：v0.2 全文通读；两门亲跑——`landing-fidelity-check.mjs --preflight` → `ok:true` / `anchorCount:3` / `anchorUnion` 恰 = `["docs/architecture.md","docs/decisions/ADR.md","docs/glossary.md"]` / `findings:[]`；`spec-anchor-check.mjs` → `ok:true` / `storyCount:14` / `anchorLines:36` / `errors:[]`（基线不变）。canonical 零触亲证：`git status --porcelain docs/` 空（零改动）；`docs/glossary.md:32` 仍为「写入资格与边界见 arch §6.7」旧文（= T8 尚未落地）；`docs/decisions/ADR.md` 尾条仍为 ADR-015（ADR-016 未落）；`grep -c "设备级 session" docs/architecture.md` = 0（基线）。R1 评审记录（§7 :356 起）逐字未被 author 触碰（append-only 纪律成立）。

#### F1–F5 修订到位性（逐条对账）

- **F1 · 到位**。T8（:190–:208）：问题描述 / 证据（glossary :32 逐字引文与我 R1 现读一致）/ 改法（「写入资格与边界见 arch §6.7」→「写入范围与残余见 arch §6.7」，词条其余不动、溯源注记保留）三件齐；manifest 增 `docs/glossary.md`（:15）；§3 术语表行改判「改」（:241）；census pattern 扩为含「记忆写入资格\|写入资格」（:262）并**如实记录初版 pattern 命不中 :487 与 glossary :32 的勘误**（:265）——REBUT 事由已被 author 采纳为自认勘误，符合「不造假绿」纪律；§5 验证集 1（anchorUnion 三路径）/ 6（全仓残留 1 处 = :487 历史行）/ 7（glossary 双向计数）/ 10（glossary numstat 1增1删）/ 11（T8 入逐字核清单）同步到位。T8 物理置于 T7 之前 + 节序说明（:192）——机械必要性成立（`parseSection2AnchorLists` §2 节域在 T7 围栏内 `## ADR-016` 行首截断；我亲跑 preflight 得 ok:true、anchorCount:3，证明该布局被正确解析），编号沿用 R1-F1 指称不重排，可接受。
- **F2 · 到位**。§3 张力清单补第 4 条（:253）：:603 逐字引用与 requirements 现文相符（「AC3/AC4 的抽取边界与 AC5 的机械回合隔离待架构面定形，随实现包验收」），建议措辞与我 R1 建议同义，三处→四处，sweep 表与清单不再脱节。
- **F3 · 到位**。§3 ⚠️ 段（:255）与 §6 留根（:313）均补「该需求面批须与 REQ-261023-family-arrangement-scope-clarification 合并起草（同触 :654；status=deferred、trigger=下一次需求面变更批）」，OQ-1 附条件（:277）同步——交叉引用三处落点齐，未来 `/requirement-change` 批撞线风险已挂账。
- **F4 · 到位**。§3 张力清单第 3 条扩为两句（:252）：「不被复述」半句的张力挂账、与 T1 残余第 5 行（QQ 落库 + `honcho_search_messages` 可检索风险）关联、随 OQ-3 裁定由需求面批收口、本批只挂账不预判——措辞克制、不越权替需求面定结论，与我 R1 修法一致。
- **F5 · 到位**。§3 sweep「实现 / 部署面」行（:243）与 §6 批 B 行（:314）均补 input-brief.md:16 锚行协调注记（批 B planner 包内刷新旧标题/行号 → 新标题 + 新行号，不改冻结语义）——责任面（批 B planner）与不变式（冻结语义）都写清了。

#### 判档复核

主路径维持（R1 终裁不变；v0.2 未引入任何降档事由——T8 为投影句一行语义同步，同属主路径语义订正范畴，无新增轻路径成分）。

#### 新矛盾扫描（R2 范围）

- 验证集 4（`设备级 session` ≥2）复核：落地后来源 = T2 :55 新行 + T6 §12 行各 1 处，恰 = 2，阈值 ≥2 成立（边界贴合，但成立；landing-verifier 实测即知）。
- §2 改法集编号顺序 T1–T6、T8、T7：为机械门解析而物理前置，编号与 F1 指称连续，件内两处说明（:192 / :220）已留痕——记录性注记，非缺陷。
- T8 保留词条尾注 `（261022-device-memory-prompt-overhaul）`：该注记是词条**溯源标记**（词条由 261022 批立），非本次改动的依据行，保留正确；若按本仓惯例欲补「261023 批改描述语」溯源，属可选项，不构成打回项。
- 附录一 / 附录二均标「author 区，非评审结论」，与 §7 判官区分离清晰；author 未触碰判官区，判官区外 author 无任何评审结论性表述。
- 全仓退役语面复扫（R1 宽 pattern）：命中面 = architecture.md 11 处 / ADR.md 3 处 / glossary.md 1 处，与 §3 census 及附录一基线补充一致，无漏网新命中；docs/debt/、docs/notes/、docs/archive/ 无在册命中需要处置（debt 证据 7 键形勘误已挂批 B 收口记账）。
- OQ 承接核对：OQ-1 附条件三条（:603 / 合并起草 / 「不被复述」）已录入 OQ-1 节（:277）；OQ-2 = (a) R1 同意意见已标注（:281）。OQ 裁定权仍在 human（立项 ack 时）。

无新矛盾。

verdict: GREEN
