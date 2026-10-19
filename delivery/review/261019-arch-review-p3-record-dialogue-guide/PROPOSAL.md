# PROPOSAL · 261019-arch-review-p3-record-dialogue-guide

> **状态：FROZEN v1.2 · 2026-10-19 · architect 起草（Track B 主路径）**——来源 = 跨面 intake spine `delivery/intake/261019-p3-record-dialogue-guide.md`（facePlan 顺序 2，human 2026-10-19 立项 ack：Track B；261001 Package B 吸收进本批）。FROZEN 凭据（2026-10-19 主线程回填）：human 立项 ack（spine §立项 ack，2026-10-19）+ architect-reviewer **R2 GREEN**（R1 否回 BLOCK(F1-F4) → v1.1 修毕 → R2 聚焦复核 GREEN，见 §7 两轮评审记录）+ 判档结论 = **主路径**（轻路径四判据全否）+ 契约四分类 = **ACCOMPANIES_SPEC_CHANGE**（T5/T6，reviewer 独立裁定）。**v1.1 修订摘要（R1 否回应改，逐 finding 对应）**：R1-F1/F2/F3 = §5 第 3/4/5 条硬断言按判官建议改写——断言改用不被本批新文本引述的旧句全形与计数断言，基线全部 2026-10-19 复跑重写，且新断言已对 §2 改法集与 ADR-013 新文本逐一跑反证；R1-F4 = 取判官三选一之 (a)——引导/整理两条一次性 LLM 回合经既有 OpenClaw OpenAI 兼容端点承载、request 级 system prompt 携带制品指令、出网沿 §1.2 路径②记账零新增（前置条件由三件实证成立，落 T3 第 5 条与契约包 impact，不新增 human 触点）；R1-N1 = T5 插入 2 契约变更记录补枚举 §四 `kidpal_session_ctx` 触点与节标题触点；R1-N2 = §1.4 节标题括注对账入 T5 替换 3；R1-N3 = §4 NFR 映射表补 NFR-PERF-3 行入 T4 替换 7；R1-N4 = 场景④失败句丢稿口径显式化（当轮素材不归档）+ 素材保全取舍入 §6 留根（不发明新需求语义）。**v1.2 修订摘要（R2 GREEN 附带两 NIT，FROZEN 前顺手收）**：R2-N1 = T7 §12 行改动面列在「§5.5.4」后补「§4 NFR 映射表」，与 T4 七处联动对齐；R2-N2 = §5 第 4 条断言措辞钉「恰 2 处命中（内联 append 时按 grep -o 或人读计）」，消除 grep 计行歧义。
> **承载 ARQ**：无独立 ARQ。来源 = spine 两件：`delivery/intake/261019-p3-record-dialogue-guide.md`（本变更全链身份）+ `delivery/intake/261001-m4-k3-record-session-live-repair.md`（Package B = D3-甲「引擎侧再动抑制 LLM」，spine openQ q1 已裁吸收进本批一次定形）。上游需求面 = requirements v1.65（2026-10-19 frozen）。
> **review 轨**：Track B **主路径**（非轻路径——触 `docs/contracts/**` + 改 §10 决策历史档注记 + 新增 ADR + 机制重写 = 非零语义；轻路径判据 ①②③ 三否）。SSOT：`.claude/references/arch-change.md`。
> **packet-binding（FROZEN 生效）**：packet-id = 本 PROPOSAL §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触契约面 + 决策历史档注记 + 新增 ADR + 首个记录语境云→端控制动作；但方向已由 human 逐段批准设计稿收敛，wire 新增仅一枚 additive 控制动作行，归档下游零改）。
> **review profile**：架构面 + 契约面（触契约分支，显式标注）；G1/G2/G3 治理面 = 零（不触 `.claude/**`）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是结构字段，GREEN 结论前必须在场；判官随 GREEN 结论核对、主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/architecture.md
docs/ADR.md
docs/contracts/interface-contract.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：P3 记录会话从「闭嘴累积、停录整段转写」改为**对话式引导记录**——孩子讲着时 AI 默认闭嘴，停顿超卡壳阈值或明示卡住时 AI 看着已讲内容轻声引导一句（停留记录屏不跳页），说「我说完了」/「讲完啦」、再按 KEY 或连续 2 轮引导无回应时收尾，全程孩子说的话一次性**忠实整理**成篇归档；架构面把决策 25 第二条聆听语义、§5.1.2 计时器族、§5.2 场景④ 链路、120s 语义四处联动一次定形，261001 Package B（引擎侧抑制 LLM）同批定形，契约面新增一枚 `start_organizing` 控制动作并做措辞级对账。

**现状差距**（需求面 v1.65 已 frozen，本面只给架构/契约侧缺口）：

1. 决策 25（architecture.md:423）仍断言「manual 聆听模式 + 网关累积（停录整段转写归档）」，且「机制细节见 §2.10」是迁移前旧编号悬空指针（:397 已声明旧编号迁移，实际承载 = §5.1.2 + §5.2 场景④）——manual 模式 ASR 在 listen stop 才触发、录音期间产不出逐句 stt（261001 q8 真机复核），语音收尾与卡壳引导在该形态下物理不可能。
2. §5.1.2 front 网关行（:186）与 story.py 行（:193）仍是「120s 超时 sweep」/「`timed_out_sessions` 120s 纯决策」表述；§5.2 场景④（:214）链路无引导注入、无整理步、无引擎抑制，收尾三式含「120s 无语音超时」。
3. 120s 语义残留四处：§2.3 会话管理行（:91）、§5.1.1 会话状态机行（:169，引 requirements v1.46 已被 v1.65 取代）、§9 to-be②（:389）；§5.1.1 story_page 行（:173）与 §5.5.4 记录会话演出行（:293）未含 v7 新增两帧（引导播报中 / 整理中）；§2.4 原则 3（:99）「网关恒薄」与 front 发起引导/整理 LLM 回合存在文本张力。
4. 契约 :175（§1.4 `start_recording`「累积态 / manual 聆听模式」）与 :271（§四 `kidpal_session_ctx`「front 即切该会话累积态」「设备 manual 聆听…引擎不感知本信号」）措辞随 v1.65 与 Package B 定形失真；且现行契约无云→端「记录会话收尾/进整理态」下行信号，v1.65 AC4「整理期间以思考态盖住延迟」无 wire 载体。
5. Package B（261001 spine，human 触点①已裁「引擎侧再动抑制 LLM」读法乙）冻结待定形：front 截不住引擎→OpenClaw 的 loopback 直达调用，抑制必须在引擎侧，机制形态（front→引擎 等价信号）本批一次定形。

**非目标（本批不做）**：卡壳阈值拍数（NFR-PERF-3 标待定、真机调优后立项）；60 秒问答会话超时语义与引擎 24h 兜底/告别语关闭配置不动（§6.4 / 决策 21 历史叙述原样）；归档下游零改（`archive_story` / seq 账本 / `story_list` 三时点推送 / 盖章演出 / 删除链路）；引导/整理指令的具体文案与 prompt 制品文件本体（归实现面承载包，arch 面只定载体归属与机制）；普通问答链路的思考态渲染接线（§9 to-be③ 原样保留）；TECH-261014 三条目正文 append（主线程落地后动作，见 §6）；固件/server 实现（归实现面承载包，五档分档预判 ② 单包 Full，须待 261016-p3-story-body-sd-cache 合并）。

**为什么变（翻案留痕，设计稿末节 + 261001 技术证成）**：本批有意识翻两次已定裁定——① 决策 25（2026-09-02 第三轮 grilling 定 manual 聆听 + 网关累积）与 ② S-F2.2「一刀切纯记录、提问也被记录」（2026-09-14 human 裁，需求面 v1.65 已翻、本批落架构侧连带）。**当初定的理由** = 聆听语义简单可靠、录音不被 AI 插话污染。**现在变的理由** = 孩子对一台沉默设备讲完完整故事实际很难（卡壳即放弃，叙事脚手架对 8–12 岁目标是真实价值）；对话态下问答天然被滤出不进素材，「提问被记录」的代价随之消解；且 manual 聆听形态在技术上已被真机证伪（产不出逐句 stt，261001 q8）——翻案动因与取舍全貌 = `delivery/intake/261019-p3-record-dialogue-guide.design.md`（human 2026-10-19 逐段批准）。

---

## 2. 问题、根因与改法（T1…T7 + 契约包）

> 起草期实证留痕：本节全部 file:line 引用于 2026-10-19 起草本会话逐一 Read 现核（architecture.md :91/:99/:169/:173/:186/:193/:214/:293/:389/:397/:423、interface-contract.md :175/:271、requirements.md S-F2.2 :453-467 / NFR-PERF-3 :614-618 / §7 开放问题 7 :694）；实现侧断言现核 = story.py:55（`AUTO_STOP_SECONDS = 120`）/:124-135（`is_finish_phrase` 子串扫描）、front_app.py:312-319（`_story_timeout_sweep` = pending-stop 10s 窗口 + 120s 收尾）、display_schema.py:43-45（`GATEWAY_DECISION_CONTROL_ACTIONS` / `FORWARDABLE_CONTROL_ACTIONS`）、mcp_server.cc:1236-1247（control action 枚举消费 case 0/1/2）。数字类引用（行号/计数）均起草期现跑，命令与输出见 §5 各条基线。**v1.1 补核（R1-F4/F3/N2/N3，2026-10-19 现核）**：provider.py:31/:45/:53/:97-108（front 侧 LLM 适配器与 `openclaw_stream_request`）、apply-xiaozhi-overlay.sh:115-119（版本化 prompt 经 `.config.yaml` 顶层 `prompt:` 键注入）、engine-patch connection.py:764-768/:1191-1192（`change_system_prompt` 每请求 system 消息承载）、server/evidence/260903-gateway-directionB/v21-multiturn-continuity-probe.md:15-17（端点 stateless、行为由每轮全量 messages 转向）、architecture.md:38（出网路径②）、:147（NFR-PERF-2 行）、interface-contract.md:165（§1.4 节标题）、requirements.md:614（NFR-PERF-3 全名「记录会话卡壳引导时机」）。

### T1 · 决策 25 处置：正文 append 取代注记 + 悬空指针同修 + ADR-013 登记新决策本体

**问题**：决策 25（architecture.md:423）仍断言「manual 聆听模式 + 网关累积（停录整段转写归档）」，与 v1.65 对话式引导记录直接冲突；其尾句「机制细节见 §2.10」是迁移前旧编号悬空指针（:397 头部已声明旧编号映射，但本条正文未给实际落点），读者按指针找不到机制节。

**证据**：
- architecture.md:423 原文：「25. **故事录入流式机制（2026-09-02 第三轮 grilling 定）**：manual 聆听模式 + 网关累积（开始信号 = E1 control 扩展 `start_recording`，停录整段转写归档）；闲聊（VAD 自动断句逐句进对话）与录故事（累积到停）两条聆听语义正式分流。机制细节见 §2.10。」（2026-10-19 现核逐字）
- 形态先例一（append 注记）：决策 24（architecture.md:422）句尾 append「史实更正 + 窄例外」段（261016/261018 两批）；决策 4（:402）「⚠️ 已被决策 9/18 取代」、决策 6（:404）「⚠️ 已被决策 19 取代」句内取代注记。
- 形态先例二（ADR 新增）：261001 spine facePlan 顺序 3「:416 决策 25 属 :390 起冻结历史档，按 ADR 新增修订、不改正文」（预案，Package B 未落地故无落成的 ADR 实例）；§10 头部（:397）「2026-09-14 起，新架构决策一律登记 ADR.md（append-only，五件格式）；本节以下为 MVP 时代决策历史档，冻结不再新增」。
- 映射依据：docs/notes/arch-restructure-skeleton.md:48「§2.10（录音归档） → §5.1（归档组件）+§5.2」。

**根因**：决策 25 的变更性质是**机制被取代**（第二条聆听语义形态换代），不是决策 24 式「原决策仍成立下的史实更正/窄例外」；§10 历史档冻结纪律（不改正文、不新增条目）与「新决策一律进 ADR.md」条款各自管一半，单取任一先例都留缺口：纯 append 会把新决策本体违例落进冻结历史档，纯 ADR 会让 :423 原句无在场指针继续断言旧机制。

**改法**（**spine openQ q3 授权起草裁定**：形态 = **复合先例**——正文 append 取代注记 + 新决策本体登记 ADR-013）：
- :423 正文不动，句尾 append 一段「已被 ADR-013 取代」注记（照决策 4/6 取代注记 + 决策 24 句尾 append 双先例），注记内顺手修悬空指针（指向实际承载 §5.1.2 + §5.2 场景④）。逐字插入段见下方围栏【插入 1】。
- 新增 ADR-013（append 到 `docs/ADR.md` 尾，五件格式）登记新决策本体（记录语境对话态 + Package B 引擎侧抑制定形 + `start_organizing` 下行触发），supersedes 决策 25 机制面、保留「两条聆听语义分流」本体。逐字草案见 §6「ADR-013 落地附件」。

```text
【插入 1 · §10 决策 25 条目句尾 append】

⚠️ **已被 ADR-013 取代（`261019-p3-record-dialogue-guide`，2026-10-19；261001-m4-k3-record-session-live-repair Package B 同批定形）**：「闲聊与录故事两条聆听语义正式分流」的分流本体保留；被取代的是第二条形态——「manual 聆听 + 网关累积（停录整段转写归档）」→「记录语境对话态 = 累积 + 卡壳轻引导 + 收尾一次性忠实整理」（requirements v1.65）。技术证成 = manual 模式 ASR 在 listen stop 才触发、录音期间产不出逐句 stt，收尾词前置检测物理不可能（261001 q8 真机复核）；产品证成 = 孩子对沉默设备讲完完整故事实际很难、对话态天然消解「提问被记录」代价（设计稿翻案说明，human 2026-10-19 逐段批准）。「机制细节见 §2.10」为迁移前旧编号悬空指针（本节头部已声明旧编号映射），实际承载 = §5.1.2 + §5.2 场景④（映射表 = notes/arch-restructure-skeleton.md）。
```

目标文件：`docs/architecture.md`（改）· `docs/ADR.md`（改）

### T2 · §5.1.2 服务端组件行重写：front 网关行 + story.py 行（计时器族换代）

**问题**：front 网关行（:186）故事面表述停在「切累积态 / 命中收尾词 stop 归档 / 120s 超时 sweep」，story.py 行（:193）停在「`is_finish_phrase` 收尾词检测 + `StoryAccumulator.timed_out_sessions` 120s 纯决策」——120s 纯静默超时已被 v1.65「卡壳引导 2 轮无回应自动收尾」取代，且两行均无引导注入、整理步、引擎抑制三件新机制；front「故事机械面」称谓在 front 发起引导/整理 LLM 回合后不再纯机械。

**证据**：
- architecture.md:186 front 网关行片段（2026-10-19 现核逐字）：「**故事机械面**（上行泵拦截 `kidpal_session_ctx`/`kidpal_story_delete` 整条消费不下透引擎；`source_page=story` 切累积态；stt 帧喂累积 + 累积态命中收尾词 stop 归档；listen-stop 归档 + 下发 `story_text(seq)`；……relay 收紧引擎侧 `story_*` 块；120s 超时 sweep）、提醒轮询循环」。
- architecture.md:193 story.py 行片段（现核逐字）：「`is_finish_phrase` 收尾词检测 + `StoryAccumulator.timed_out_sessions` 120s 纯决策」。
- 实现侧现核：story.py:55 `AUTO_STOP_SECONDS = 120`、:598-604 `timed_out_sessions`（纯决策、边界语义 >=）；front_app.py:312-319 `_story_timeout_sweep` 先走 pending-stop 10s 窗口（261001-m4-k3-story-archive-order 归档时序机制）再走 120s 自动收尾。

**根因**：120s 纯静默超时是「闭嘴累积」语义的配套兜底；记录语境改对话态后，兜底语义换代为「卡壳计时触发引导 + 轮次计数 + 2 轮无回应自动收尾」，组件行表述未随需求面换代。

**改法**：front 行三处片段替换 + story.py 行一处片段替换（逐字替换对见下方围栏；行内其余内容——故事面以外的职责、story_list 三时点自推沿革段——不动）。卡壳阈值不落数值（阈值权威 = requirements NFR-PERF-3，「需求与阈值归 requirements.md」分工句）；pending-stop 挂起窗口（KEY 收尾后等迟到转写，261001-m4-k3-story-archive-order 立）保留。

```text
【替换 1 · §5.1.2 front 行 :186 片段 A】
原；`source_page=story` 切累积态；stt 帧喂累积 + 累积态命中收尾词 stop 归档；listen-stop 归档 + 下发 `story_text(seq)`；
新；`source_page=story` 切记录语境对话态（v1.65 取代「累积态」）；stt 帧喂累积缓冲（只回显、不逐句进对话）+ 卡壳计时器族（每句讲完起计时〔阈值权威 = requirements NFR-PERF-3〕、接着说重置、计时到或明示卡壳词命中触发引导 LLM 回合 + `kidpal_speak` 注入播报〔ADR-003 同型〕，返回空/收声抑制 = 本轮不播报仍计轮，连续 2 轮无回应自动收尾）+ 记录语境等价信号注入引擎侧抑制 LLM（ADR-013）；收尾检出 → 下发 `start_organizing` → 一次性忠实整理 → 归档 + 下发 `story_text(seq)`；

【替换 2 · §5.1.2 front 行 :186 片段 B】
原；relay 收紧引擎侧 `story_*` 块；120s 超时 sweep）、提醒轮询循环
新；relay 收紧引擎侧 `story_*` 块；卡壳计时 sweep（取代 120s 超时 sweep；pending-stop 挂起等转写窗口保留〔261001-m4-k3-story-archive-order〕）+ 引导/整理一次性 LLM 回合发起（机械触发归网关、语义生成在脑——ADR-002 混合分层同型，ADR-013））、提醒轮询循环

【替换 3 · §5.1.2 front 行 :186 片段 C（称谓）】
原**故事机械面**（上行泵拦截
新**故事面**（上行泵拦截

【替换 4 · §5.1.2 story.py 行 :193 片段】
原；`is_finish_phrase` 收尾词检测 + `StoryAccumulator.timed_out_sessions` 120s 纯决策 |
新；`is_finish_phrase` 收尾词检测 + 明示卡壳词表检测（同构子串扫描，词表 = 实现期可调常量）+ `StoryAccumulator` 卡壳计时与引导轮次纯决策（每句活动时刻重置计时、计时到 = 引导触发、连续 2 轮无回应 = 自动收尾、被抑制/返回空轮照常计轮——取代 `timed_out_sessions` 120s 纯静默决策；pending-stop 挂起窗口保留）+ 忠实整理输入装配（只取孩子的话；引导语与问答滤除） |
```

目标文件：`docs/architecture.md`（改）

### T3 · §5.2 场景④ 链路重写：对话式引导记录端到端 + Package B 一次定形 + E3 检出器形态 + prompt 载体决策

**问题**：场景④（:214）是记录归档链路的端到端权威，现文为「引擎 manual 聆听 + front 旁路累积 → 收尾三式（含 120s 无语音超时）→ 归档 → story_text 下发」——无卡壳引导注入、无忠实整理步、无引擎侧抑制（Package B 冻结待定的本体），且「切累积态」「引擎不感知」等措辞随 v1.65 与 Package B 定形失真。

**证据**：
- architecture.md:214 场景④ 全段（2026-10-19 现核逐字，待替换区间见下方围栏【替换 1】原文列）。
- 261001 spine openQ q6（human 触点①裁）：「引擎侧再动抑制 LLM（取「全程无 LLM 回合」读法 (乙)）……须 Track A 定形 front→引擎 抑制机制」；q8（主线程复核真机日志）：「录音**期间**确无 stt（manual 模式 ASR 在 listen stop 才触发），故收尾词前置检测物理上不可能」。
- 261019 spine openQ q1（human 立项 ack）：Package B 吸收进本批一次定形；q6（主线程 L1 裁）：被抑制的引导轮照常计入轮次；q7-E3：明示卡住检出器形态（关键词表 vs LLM 判定；引导 LLM 可返回空 = 本轮不引导）归本批起草时写明。
- 设计稿「机制」节六步（human 2026-10-19 逐段批准）与「引导语护栏」（只基于已讲内容提问、不提供情节选项、不代编）。
- provider 出口实证（R1-F4 前置条件核验，2026-10-19 现核，详见改法第 5 条）：provider.py:31/:45/:53/:97-108、apply-xiaozhi-overlay.sh:115-119、engine-patch connection.py:764-768/:1191-1192、v21-multiturn-continuity-probe.md:15-17、architecture.md:38（§1.2 出网路径②）。

**根因**：场景④ 的链路语义属「闭嘴累积」时代；需求面 v1.65 换代后，端到端权威必须同步换代，否则下游实现包无链路权威可引；Package B 的抑制机制（引擎侧、front 注入等价信号）在 arch 面无落点，契约 :271「引擎不感知本信号」句随之失真（T6 对账）。

**改法**：整段重写场景④ 的链路前半（从「**④ 记录归档**：触发词」到「盖章演出（N = 入册流水号只增不减）」），**story_list 三时点推送 / relay 收紧 / 删除链路后半段原样保留**；段尾「误判兜底」句同步更新。逐字替换对见下方围栏（原文列 = :214 现核逐字）。新链路承载五项裁定：

1. **Package B 定形（261001 q6/q8 + 261019 q1）**：聆听形态换代为可产逐句 stt 的 D3-甲形态（manual 模式作废）；记录语境期间引擎侧不对 stt 跑 LLM 回合——front 截不住引擎→OpenClaw 的 loopback 直达调用，抑制必须在引擎侧：front 经既有内部注入通道（ADR-003 `kidpal_speak`〔front→引擎腿注入内部指令、引擎补丁认领转调既有音频链〕同型先例）向引擎注入等价记录语境信号，引擎补丁认领后跳过 LLM 派发；front↔引擎为服务端内部链路，**信号形状不入契约**（契约面只对 :271 尾句做措辞对账，T6）。
2. **E3 检出器形态（spine openQ q7-E3 授权起草写明）**：明示卡住检出器 = **关键词表子串扫描**（`is_finish_phrase` 同构先例，story.py:124-135 现核）——确定性、可离线验收断言、词表为实现期可调常量；不取 LLM 判定（每条 stt 都过 LLM 判卡壳 = 每段付一轮 LLM 延迟与成本，且不可离线断言）。**引导 LLM 可返回空 = 本轮不引导但轮次照计**——与 q6 已裁「被抑制轮照常计轮」同构：孩子说「别说话听我讲」的收声抑制即「引导 LLM 持续返回空」形态，收声不停摆，第 2 轮计时到仍无回应即自动收尾，死锁修法闭环。
3. **q6 计轮规则落地**：连续 2 轮引导无回应自动收尾，轮次含被抑制与返回空的不播报轮。
4. **整理步**：全程孩子的话一次性发忠实整理 LLM 回合（只用原话与事实、不换表达不补情节、起标题 ≤12 字口径不变随整合同一回合产出）；KEY 路径沿 pending-stop 挂起窗口等迟到转写（261001-m4-k3-story-archive-order 既有机制保留）；归档下游（`archive_story` / seq 账本 / story_list / 盖章）零改。
5. **prompt 载体决策（评估 §5.5.5 是否触——结论：不触）**：引导/整理指令 = **front 侧版本化 prompt 制品**（独立一次性 LLM 回合、不走 OpenClaw 主 agent 共享 system prompt）。理由：§5.5.5 标题即限定范围 = OpenClaw system prompt 单制品；主 agent system prompt 被语音闲聊/课表/QQ 录入共用，塞进记录语境专用指令 = 污染共享提示词且无法按调用收窄；front 侧独立制品可随场景独立演进，接线不变式沿 ADR-001（渲染面 == 声明面 == 被测面）三面相等纪律，制品文件与守卫归实现面承载包落。**provider 出口钉（R1-F4 三选一取 (a)，前置条件实证成立）**：两条一次性 LLM 回合**经既有 OpenClaw OpenAI 兼容端点承载**（loopback `127.0.0.1:18789`，决策 18/23），request 级 system prompt 携带制品指令——前置条件「该端点可携带制品级 system prompt」由三件实证成立：① 生产承载机制本身就是请求级 system 消息——版本化 prompt 制品经 `.config.yaml` 顶层 `prompt:` 键注入（apply-xiaozhi-overlay.sh:115-119）→ 引擎 `change_system_prompt`（engine-patch connection.py:764-768 / :1191-1192）→ 每请求 messages 携带 system——主 agent 行为全由请求携带的 system prompt 转向（§5.5.5 版本化即此载体）；② V21 探针实证端点 stateless、行为由每轮全量 messages 转向（v21-multiturn-continuity-probe.md:15-17，「显式透传全历史 → 正确回」）；③ front 侧适配器既有在册——`server/gateway/provider.py` `LLM_OPENCLAW_OPENAI`（:45，registry :53）+ `openclaw_stream_request(messages)`（:97-108，调用方全量构造 messages、恒不带 max_tokens〔契约 §四 / V9 硬约束〕），additive registry（GWA-E4）本为 front 侧 LLM 调用预留，零新增 provider 族。**出网记账零新增路径**：front→18789 为 loopback 内部调用，真实出网发生在 OpenClaw 内部到 DeepSeek = §1.2 出网路径②（architecture.md:38）记账面不动；§1.2 外部依赖清单与 NFR-SEC-2 记账本批均不触（(b) 新增路径⑤与 (c) 新增 OQ 均不启用）。实现期断言种子（归实现面承载包验收）：引导/整理回合的制品级 system prompt 约束该回合不触发工具调用（主 agent 路由表同款 prompt 纪律，v11 先例）。

```text
【替换 1 · §5.2 场景④ :214 链路前半整段替换】
原：**④ 记录归档**：触发词（「我要讲故事」/「帮我记下来」）或 P3 会话上下文信号（设备告知「会话发起自 P3」，网关会话建立即切累积态——requirements v1.32 扩展；信号载体 = 契约 §四 `kidpal_session_ctx`——**front 上行泵整条拦截消费、无论取值、引擎不感知**，TLS 之后不下透）→ 引擎 manual 聆听 + front 旁路累积（stt 帧喂入，AI 不逐句回复）→ **收尾三式**（front 机械扫描 stt 文本命中收尾词「我说完了/讲完啦」→ stop 归档 / 再按 KEY（listen-stop）→ 归档 + 下发 / **120s 无语音超时** front 侧计时 sweep 自动收尾；三式皆走正常归档、不丢弃）→ `story.py` 归档（**持久流水号账本 `<stories_dir>/.seq.json`** 分配 `seq=max+1`、只增不减——同 id 重写不变、删除不回退、重启存活、账本缺失/损坏重建不回退 + 告警）+ 起标题（模型一回合只起 ≤12 字 title）→ `story_text`（**携带 `data.seq`**）下发渲染 + 盖章演出（N = 入册流水号只增不减）
新：**④ 记录归档**（对话式引导记录——requirements v1.65 / `261019-p3-record-dialogue-guide` 重写，取代旧「累积转写」链路；决策本体 = ADR-013）：触发词（「我要讲故事」/「帮我记下来」）或 P3 会话上下文信号（设备告知「会话发起自 P3」，网关会话建立即切**记录语境对话态**——requirements v1.65 取代 v1.32「累积态」；信号载体 = 契约 §四 `kidpal_session_ctx`——**front 上行泵整条拦截消费、无论取值、引擎不感知本消息**，TLS 之后不下透）→ 聆听产逐句 stt（D3-甲聆听形态，manual 模式作废——manual 模式 ASR 在 listen stop 才触发、录音期间产不出 stt，收尾词前置检测与卡壳引导物理不可能，261001 q8 真机复核）+ front 旁路累积（stt 帧喂入会话记录缓冲、右区回显照旧，AI 不逐句回复）+ **引擎侧抑制 LLM**（261001 Package B 定形：记录语境期间引擎不对 stt 跑 LLM 回合——human 261001 触点①裁读法乙「全程无 LLM 回合」；front 截不住引擎→OpenClaw 的 loopback 直达调用，抑制必须在引擎侧：front 经既有内部注入通道向引擎注入等价记录语境信号〔ADR-003 `kidpal_speak` 同型先例、引擎补丁认领〕使引擎侧跳过 LLM 派发；front↔引擎为服务端内部链路、信号形状不入契约）→ **卡壳轻引导**（每句讲完起卡壳计时——阈值权威 = requirements NFR-PERF-3〔待定、真机调优后立项拍数〕；孩子接着说即重置；计时到或明示卡壳词命中——检出器 = 关键词表子串扫描、`is_finish_phrase` 同构——→ front 把目前累积的孩子的话发一次性引导 LLM 回合〔引导指令：只基于已讲内容提问、不提供情节选项、不代编情节；**可返回空 = 本轮不引导但轮次照计**——孩子说「别说话听我讲」的收声抑制同此形态，收声不停摆〕→ 轻引导一句 → `kidpal_speak` 注入播报〔ADR-003 既有音频链〕→ 继续聆听；设备停留记录屏不跳页、引导播报中 KEY 短按 = 打断引导继续聆听）→ **收尾三式**（front 机械扫描 stt 文本命中收尾词「我说完了/讲完啦」/ 再按 KEY（listen-stop）/ **连续 2 轮引导无回应自动收尾**〔取代 120s 无语音超时 sweep；轮次含被抑制与返回空的不播报轮——收声不停摆〕；三式皆走正常归档、不丢弃）→ 下发 `start_organizing`（契约 §1.4 新增控制动作：设备切整理过渡演出、整理中 KEY/BOOT 均不响应；三式统一下发、重复幂等）→ **忠实整理**（全程孩子的话一次性发 LLM〔忠实整理指令：只用孩子原话与事实，去口水词、断句、分段、起标题〔≤12 字口径不变、随整合同一回合产出〕，不换表达、不补情节；引导语、孩子中途提问与 AI 回答滤除不进素材〕；KEY 路径沿 pending-stop 挂起窗口等迟到转写〔261001-m4-k3-story-archive-order 既有机制保留〕）→ `story.py` 归档（**持久流水号账本 `<stories_dir>/.seq.json`** 分配 `seq=max+1`、只增不减——同 id 重写不变、删除不回退、重启存活、账本缺失/损坏重建不回退 + 告警）→ `story_text`（**携带 `data.seq`**）下发渲染（离整理过渡态）+ 盖章演出（N = 入册流水号只增不减）。引导/整理指令载体 = **front 侧版本化 prompt 制品**（独立一次性 LLM 回合、不走 OpenClaw 主 agent 共享 system prompt——**不触 §5.5.5**；接线不变式沿 ADR-001 三面相等，制品随实现包落）；整理/引导链路失败 = **当轮素材不归档**（孩子已讲内容不写成故事、不分配 seq——丢稿口径显式化；失败态素材保全取舍设计稿与 v1.65 均未覆盖，归 §6 留根、不发明新需求语义）+ §6.3 固定安抚句 + text 视图结束当轮（自然离整理过渡态）。卡壳引导与忠实整理 = front 承载计时/计数/触发与指令装配（机械层）+ 脑侧一次性 LLM 回合生成（语义层）——ADR-002 混合分层同型，§2.4 原则 3 注记

【替换 2 · §5.2 场景④ :214 段尾误判兜底句】
原：误判兜底见 §10 决策 25 与 requirements v1.47/v1.49 修订。
新：误判兜底见 §10 决策 25 取代注记与 requirements v1.47/v1.49 修订；「刚才那段是故事」挽回口径在记录语境对话态内不再需要（闲聊语境、其他页保留——requirements v1.65 S-F2.2 补充规则）。
```

目标文件：`docs/architecture.md`（改）

### T4 · 120s 语义联动对账：§2.3 / §5.1.1 两行 / §9 to-be② / §5.5.4 / §2.4 原则 3 + §4 NFR 映射表补 NFR-PERF-3 行（七处）

**问题**：120s 录音态超时语义在 architecture.md 有四处残留引用（:91 / :169 / :389 外加上下文联动的 :173 story_page 行与 :293 演出行未含 v7 新帧），§2.4 原则 3（:99）「网关恒薄」字面「front 只做终结/鉴权/校验/relay/投递，业务逻辑 100% 在脑」与 front 发起引导/整理 LLM 回合存在文本张力——留原文必触发判官四查③撞车与④牵连面否回。另 §4 NFR 对策映射表（:144-152）表头自述「每条 NFR 指到满足它的机制」，却未随 v1.65 新增 NFR-PERF-3 行（R1-N3）——T2 恰在重写其机制承载行，缺行 = 映射表对自身自述口径不全。

**证据**（2026-10-19 现核逐字，替换对见围栏）：
- :91 §2.3 会话管理行：「基于小智 ToggleChatState 改；60s/120s 双阈值（§5.1 会话状态机）」。
- :169 §5.1.1 会话状态机行尾：「录音态例外 = 120s 自动收尾归档，requirements v1.46」——v1.65 已取代 v1.46 该裁定（requirements.md:694 开放问题 7 撤注现核）。
- :389 §9 to-be②：「② 60s/120s 双阈值 timer 生产者接线（会话状态机激活）」。
- :173 §5.1.1 页面组件行：「story_page 含列表/阅读/录音/盖章四态」。
- :293 §5.5.4 记录会话演出行：「**记录会话演出**（`p3-recording`/`p3-stamp-frame`+`comp-stamp`）MVP 已实现验收」——v7 新增 `p3-recording-guide` / `state-organizing` 两帧（requirements.md:730 v1.65 行现核）。
- :99 §2.4 原则 3：「**网关恒薄**：front 只做终结/鉴权/校验/relay/投递，业务逻辑 100% 在脑（决策 20「脑算网推」）；」。
- 反证保留面：:325（§6.4 引擎 120s 断连配置修正历史叙述）与 :419（决策 21 历史档）的 120s 为引擎侧配置沿革、与录音态超时无关，**不动**（起草期 grep 基线见 §5 第 3 条）。
- :147 §4 NFR 表 NFR-PERF-2 行（2026-10-19 现核逐字，见围栏【替换 7】原文列）；requirements NFR-PERF-3 全名 = 「记录会话卡壳引导时机」（requirements.md:614 现核），其支撑列自指「arch §5.1（故事归档组件）」。

**根因**：120s 语义换代只动了主链路（T2/T3），散布的引用点与张力点未随批收窄；NFR-PERF-3 是 v1.65 新增 NFR，映射表行随需求面定稿后未补。

**改法**：七处逐字替换/append/插行（见下方围栏）。60s 问答超时语义、§6.4/决策 21 历史叙述、§9 to-be③ 思考态接线（普通问答链路）均不动。

```text
【替换 1 · §2.3 会话管理行 :91 说明格】
原：基于小智 ToggleChatState 改；60s/120s 双阈值（§5.1 会话状态机）
新：基于小智 ToggleChatState 改；60s 问答会话超时（§5.1 会话状态机；记录会话不适用 60s——v1.65 起其超时语义 = 网关侧卡壳引导 2 轮无回应自动收尾，原 120s 设备侧阈值作废，见 §5.2 场景④）

【替换 2 · §5.1.1 会话状态机行 :169 尾句】
原：（requirements §2.1 对账；录音态例外 = 120s 自动收尾归档，requirements v1.46）
新：（requirements §2.1 对账；录音态例外 = 不适用 60s 会话超时；自动收尾 = 网关侧卡壳引导 2 轮无回应——requirements v1.65 取代 v1.46「120s 自动收尾归档」口径）

【替换 3 · §9 to-be② :389】
原：② 60s/120s 双阈值 timer 生产者接线（会话状态机激活）；
新：② 60s 问答会话 timer 生产者接线（会话状态机激活；原 120s 录音态阈值随 v1.65 作废——记录会话自动收尾归网关卡壳计时器族〔§5.2 场景④〕，本条接线范围相应收窄为单阈值）；

【替换 4 · §5.1.1 页面组件行 :173 片段】
原：story_page 含列表/阅读/录音/盖章四态
新：story_page 含列表/阅读/录音（v1.65 增引导播报中子态）/整理过渡（v1.65 新增，契约 §1.4 `start_organizing` 触发）/盖章态

【替换 5 · §5.5.4 记录会话演出行 :293 片段】
原：- **记录会话演出**（`p3-recording`/`p3-stamp-frame`+`comp-stamp`）MVP 已实现验收；
新：- **记录会话演出**（`p3-recording`〔v7 修订〕/`p3-recording-guide`〔v7 新增·引导播报中〕/`state-organizing`〔v7 新增·整理中、思考态复用——下行触发 = 契约 §1.4 `start_organizing`〕/`p3-stamp-frame`+`comp-stamp`）：录音/盖章两态 MVP 已实现验收，引导播报中/整理中两帧随本批实现面包接线；

【插入 6 · §2.4 原则 3 :99 句尾 append】
**同型切分注记（`261019-p3-record-dialogue-guide`，2026-10-19）**：记录语境对话态的卡壳引导与收尾忠实整理 = front 承载计时/计数/触发与指令装配（机械层）+ 脑侧一次性 LLM 回合生成（语义层）——ADR-002「可机判性切分」混合分层同型，推理与生成 100% 在脑，本原则语义不变（决策本体 = ADR-013）。

【替换 7 · §4 NFR 对策映射表 :147 NFR-PERF-2 行后插行（R1-N3）】
原：| NFR-PERF-2 查字 5 秒档 | OpenClaw 侧 `warm_char_cache.py` 预热 9574 字全量 `char_cache.json` + char_stroke hold-and-replace 缓存；【修复项】live 冷启动实测 ~109s，流式/异步优化待做 | §5.1 OpenClaw 补丁组件 |
新：| NFR-PERF-2 查字 5 秒档 | OpenClaw 侧 `warm_char_cache.py` 预热 9574 字全量 `char_cache.json` + char_stroke hold-and-replace 缓存；【修复项】live 冷启动实测 ~109s，流式/异步优化待做 | §5.1 OpenClaw 补丁组件 |
| NFR-PERF-3 记录会话卡壳引导时机 | front 卡壳计时器族（每句讲完起计时、接着说重置、计时到或明示卡壳词命中触发引导 LLM 回合、连续 2 轮无回应自动收尾；阈值权威 = requirements NFR-PERF-3〔待定、真机调优后立项拍数〕）+ `StoryAccumulator` 卡壳计时与引导轮次纯决策 | §5.1.2 front/story.py 组件行 / §5.2 场景④ |
```

目标文件：`docs/architecture.md`（改）

### T5 · 思考态下行触发（q2 裁定 = ⓒ 新增云→端控制动作）：契约 §1.4 新增 `start_organizing`

**问题**：语音收尾与自动收尾由云端（front）检出，设备无法自知「该离开录音聆听演出、进整理过渡态」；v1.65 AC4 已冻结承诺「整理期间以思考态（或等效过渡演出）盖住延迟」（requirements.md:461 现核），现行契约无该下行信号。

**证据**：
- 契约 §1.4 表现状（interface-contract.md:171-175 现核）：仅 `end_session` / `stop_speaking` / `start_recording` 三行。
- display_schema.py:43-45（现核）：`GATEWAY_DECISION_CONTROL_ACTIONS = frozenset({"end_session"})`（网关决定类、引擎 display 流拒转）/ `FORWARDABLE_CONTROL_ACTIONS = frozenset({"stop_speaking", "start_recording"})`——`start_organizing` 与 `end_session` 同型 = 网关（front）检出后的决定，非引擎 display 流可透传动作；引擎侧伪造同名 action 因不在 FORWARDABLE 表被机械层拒转（display_schema.py:70-77 现核），front 自推不经 relay（§1.5 第 3 条 Q7 先例）。
- 设备消费先例（现核）：mcp_server.cc:1236-1247 control action 枚举消费（case 0/1/2），:132 `start_recording` 幂等注释「网关重试不得重置孩子已录时长/重复建 timer」。
- 三候选评估（spine openQ q2）：ⓐ 设备侧镜像扫 stt 回显帧——收尾词表双源漂移风险，且**只能覆盖语音收尾一式**：自动收尾（云端 2 轮判定）设备无从镜像，覆盖不全即出局；ⓑ 容忍停留记录屏到 story_text 到达——违反 v1.65 AC4 已冻结承诺，忠实整理数秒级延迟下孩子盯 REC 屏且 REC 仍闪 = 误导「还在录」；ⓒ 新增下行信号——覆盖三式收尾全路径、单源在 front、复用既有 control view 通道（additive 表行扩展）。

**根因**：收尾检出在云端（front 扫 stt / 计时计数），整理过渡是纯设备 UI 态，两端之间缺一枚单向告知信号；既有 wire 面（stt/tts 状态消息、mcp display、story_text）无一承载「收尾已检出、请切过渡态」语义。

**改法**（**spine openQ q2 授权起草裁定 = ⓒ**）：契约 §1.4 表格新增一行 `start_organizing`（网关决定类控制动作，与 `end_session` 同型），并在表后加一条契约变更记录（照 §1.3③/④ 既有「（契约变更记录：…）」先例）；另 §1.4 节标题（:165「控制类指令（OpenClaw 判意图后下发）」）对 `start_organizing`（front 检出、不经 OpenClaw）字面失配，随批括注对账（R1-N2，【替换 3】）。逐字见下方围栏。设备行为 = 切整理过渡演出（v7 `state-organizing` 帧，思考态复用；气泡文案归设计稿/文案单源）、整理中 KEY/BOOT 均不响应；离态 = 收到 `story_text`（正常）或降级安抚帧（失败——§6.3 固定安抚句 + text 视图结束当轮自然离态）；三式收尾 front 统一下发、重复幂等。KEY 收尾路径设备已知本地按键、可本地先切（实现面取舍），wire 语义不变——front 统一下发保单一事实源。整理过渡态的播报文案（「我来整理一下…」式）可经 `kidpal_speak` 注入承载（既有 ADR-003 通道，零新增 wire）。

```text
【插入 1 · §1.4 控制 action 表新增行（interface-contract.md :175 `start_recording` 行之后）】

| `start_organizing` | 记录会话收尾检出（语音收尾词 / 再按 KEY / 连续 2 轮引导无回应自动收尾——front 检出） | 设备离开录音聆听演出、切整理过渡演出（v7 `state-organizing` 帧、思考态复用）；整理中 KEY/BOOT 均不响应（非交互过渡态，requirements v1.65 §2.3 录音态子态按键口径）；收到 `story_text`（正常）或降级安抚帧（失败）即离态；三式收尾 front 统一下发、重复幂等。**网关决定类动作**（与 `end_session` 同型）：front 机械下发、不经引擎 display 流，引擎侧同名 action 拒转（2026-10-19 新增，`261019-p3-record-dialogue-guide`） |

【插入 2 · §1.4 表后契约变更记录（照 §1.3③/④ 先例形态）】

（契约变更记录：2026-10-19 经 `/arch-change` 批准新增 `start_organizing` 控制动作（记录会话收尾 → 整理过渡态；网关决定类、引擎 display 流拒转）+ §1.4 节标题括注对账（「OpenClaw 判意图后下发」→「OpenClaw 判意图或 front 检出后下发」）+ `start_recording` 行措辞对账（「累积态 / manual 聆听模式」→ 记录语境对话态）+ §四 `kidpal_session_ctx` 条措辞对账（「切累积态 / manual 聆听 / 引擎不感知本信号」→ 记录语境对话态 + 引擎侧抑制由 front 内部注入等价信号承载）——requirements v1.65 S-F2.2 对话式引导记录 + ADR-013；随 `261019-p3-record-dialogue-guide` 同批原子落地，见 delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md。）

【替换 3 · §1.4 节标题（interface-contract.md :165）括注对账（R1-N2）】
原：### 1.4 控制类指令（OpenClaw 判意图后下发）
新：### 1.4 控制类指令（OpenClaw 判意图或 front 检出后下发）
```

目标文件：`docs/contracts/interface-contract.md`（改）

### T6 · 契约措辞级对账：§1.4 `start_recording` 行（:175）+ §四 `kidpal_session_ctx` 尾句（:271）

**问题**：:175「网关切该会话为累积态（不按句送对话）、设备切 REC 录音演出（manual 聆听模式）」与 :271「`source_page=="story"` 时 front 即切该会话累积态」「设备 manual 聆听仍经既有 listen 消息承载，引擎不感知本信号」两处措辞随 v1.65（累积态 → 记录语境对话态）与 Package B 定形（引擎侧需感知等价信号以抑制 LLM——「引擎不感知本信号」字面失真）漂移。

**证据**：
- interface-contract.md:175 与 :271 全行（2026-10-19 现核逐字，见下方围栏原文列）。
- 先例：261001 spine faceMap contract 行对 :170/:266 的同类「机制表述随取甲漂移」对账预判（ACCOMPANIES_SPEC_CHANGE 方向）；261019 spine faceMap contract 行「预判 ACCOMPANIES_SPEC_CHANGE」。
- 语义边界：`kidpal_session_ctx` 消息的拦截事实不变（front 整条消费、不下透引擎——引擎不感知**本消息本身**）；变化的是 front 另经服务端内部注入通道向引擎注入**等价信号**（T3 第 1 条），该通道为 front↔引擎内部链路、不入契约。

**根因**：契约措辞写定于「累积态 + manual 聆听 + 引擎零感知」时代；v1.65 与 Package B 定形后，「累积态」称谓、「manual 聆听模式」、「引擎不感知本信号」三处字面与已批准设计冲突。

**改法**：两处逐字替换（见下方围栏）。wire 形状零改（消息形状、取值闭集、幂等入口、向后兼容句全保留）——纯措辞对账；四分类提案 = **ACCOMPANIES_SPEC_CHANGE**（随 v1.65 + 本批 arch 变更携带，同批原子落；非 SPEC_CONTRACT_DRIFT——是契约措辞追上已 frozen 需求，非两边冲突须裁；非 CONTRACT_CHANGE_REQUIRED——非独立契约变更）。

```text
【替换 1 · §1.4 `start_recording` 行（interface-contract.md :175）】
原：| `start_recording` | 孩子说"我要讲故事"（云端判意图） | 网关切该会话为累积态（不按句送对话）、设备切 REC 录音演出（manual 聆听模式）；误判回退路径见 spec S-F2.2（2026-09-03 新增，方向 B 故事录入流式） |
新：| `start_recording` | 孩子说"我要讲故事"（云端判意图） | 网关切该会话为记录语境对话态（stt 只喂累积缓冲与回显、不逐句进对话；卡壳轻引导与收尾忠实整理机制权威 = arch §5.2 场景④）、设备切 REC 录音演出；误判回退路径见 spec S-F2.2（2026-09-03 新增，方向 B 故事录入流式；2026-10-19 措辞对账——「累积态 / manual 聆听模式」随 v1.65 对话式引导记录改写，`261019-p3-record-dialogue-guide`） |

【替换 2 · §四 `kidpal_session_ctx` 条（interface-contract.md :271）两片段】
原 A：`source_page=="story"` 时 front 即切该会话累积态（与 §1.4 `control/start_recording` 收敛同一幂等入口，重复触发不重复开会话）；
新 A：`source_page=="story"` 时 front 即切该会话记录语境对话态（与 §1.4 `control/start_recording` 收敛同一幂等入口，重复触发不重复开会话）；
原 B：设备 manual 聆听仍经既有 listen 消息承载，引擎不感知本信号。
新 B：设备聆听仍经既有 listen 消息承载（聆听形态取值归实现面，v1.65 起 manual 模式作废）；引擎不感知本消息本身——记录语境期间的引擎侧 LLM 抑制由 front 经既有内部注入通道注入等价信号承载（front↔引擎为服务端内部链路、不入本契约；机制权威 = arch §5.2 场景④ / ADR-013）。
```

目标文件：`docs/contracts/interface-contract.md`（改）

### T7 · architecture.md §12 变更记录 changeId 行 append（定稿标记）

**问题**：arch 面语义性改动的定稿标记 = `docs/architecture.md` 文末「变更记录」节的 changeId 行（arch-change.md「定稿载体」节）；不显式列入改法集，packet-bound landing 不会落它，下游实现面承载包 launch 前置「arch 已冻结」不可满足。

**证据**：architecture.md:438 §12 头部「本文件每次改动都在下表追加一行（新行在上，倒序）」（2026-10-19 现核）；先例 = :442-444 三行（261018/261016/261015）；261017 PROPOSAL T6 + reviewer R1-F1 否回（同缺口先例）。

**根因**：packet-id = §2 改法集 + §5 验证集；§12 append 动作须显式成 T。

**改法**：§12 表顶部（261018-sd-log-persist 行之上）追加一行。changeId 取 **spine changeId `261019-p3-record-dialogue-guide`**（跨面 intake，下游按 spine 锚消费，261017 R2 changeId 取值裁决同款）；依据列不含「FROZEN vX.Y」占位 token（261017 R2 stale token 教训）。逐字见下方围栏。

```text
【插入 · §12 变更记录表首行（新行在上，倒序）】

| 261019-p3-record-dialogue-guide | 2026-10-19 | §10 决策 25 取代注记（悬空指针同修）+ ADR-013 新增（记录语境对话态 + 引擎侧抑制定形）+ §5.1.2 front/story.py 行与 §5.2 场景④ 链路重写（卡壳计时器族 / 引导注入 / 忠实整理 / `start_organizing`）+ §2.3 / §5.1.1 / §9 to-be② / §5.5.4 / §4 NFR 映射表 / §2.4 原则 3 联动对账 | delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md（契约面同批原子落：§1.4 新增 `start_organizing` + §1.4/§四措辞对账） |
```

目标文件：`docs/architecture.md`（改）

### 契约包 · 端云协议

**proposedContractDiff**（逐面最小 diff）：
- **displayBlock / §1.4 控制类指令**：新增 `start_organizing` 控制动作行（additive 表行；网关决定类、引擎 display 流拒转）+ 表后契约变更记录一条 + §1.4 节标题括注对账（「OpenClaw 判意图后下发」→「OpenClaw 判意图或 front 检出后下发」，R1-N2）；`start_recording` 行措辞对账（「累积态 / manual 聆听模式」→ 记录语境对话态，wire 形状零改）。逐字见 §2 T5 / T6。
- **shimApi / §四 端云协议剖面**：`kidpal_session_ctx` 条措辞对账（「切累积态」→「切记录语境对话态」；尾句「manual 聆听…引擎不感知本信号」→ 聆听形态归实现 + 引擎侧抑制由 front 内部注入等价信号承载；消息形状/闭集/幂等/兼容句零改）。逐字见 §2 T6。
- strokeFormat / scheduleMD：未触，正确省略。

**impact**：固件（mcp_server.cc control action 枚举增 `start_organizing` 消费 + story_page 整理过渡帧渲染 + 引导播报渲染，归实现面承载包）、front（front_app.py 卡壳计时器族 / 引导与整理 LLM 回合 / `start_organizing` 下发 / 等价信号注入，story.py 词表与轮次纯决策，归实现面承载包）、引擎补丁（connection.py 认领等价信号、记录语境跳过 LLM 派发，归实现面承载包）、OpenClaw 本体零影响（引导/整理 = 一次性 LLM 回合经既有 OpenAI 兼容端点承载、request 级 system prompt 携带制品指令——主 agent system prompt 制品与 OpenClaw agent 配置均不动；出网沿 §1.2 路径② DeepSeek 经 OpenClaw 记账、零新增出网路径——R1-F4 取 (a)，证据见 §2 T3 第 5 条）；既有三 action（`end_session`/`stop_speaking`/`start_recording`）语义不变（`start_recording` 行仅措辞对账）；`kidpal_session_ctx` / `kidpal_story_delete` / `kidpal_story_fetch` 形状与语义不变；不破坏已跑通实测链路。详见 §3 sweep 表。

**validationPlan**：逐字 diff 对账（T5/T6 插入与替换段 byte 对账）+ 契约残留 grep（R1-F3 收窄版：旧句全形「网关切该会话为累积态 / 即切该会话累积态 / 设备 manual 聆听仍经既有 listen 消息承载」零命中、`start_organizing` ≥ 2 命中）+ `spec-anchor-check.mjs` + `pre-merge-escalation-gate.mjs` 机械凭据闸三件联立（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖契约路径）。详见 §5。

---

## 3. 影响面 sweep 分类

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 架构 canonical | `docs/architecture.md` | 改 | T1 决策 25 注记 + T2 §5.1.2 两行 + T3 §5.2④ + T4 七处联动 + T7 §12 行 |
| 架构 canonical · §1.2 出网枚举 | `docs/architecture.md` §1.2 段 | 未触 | 引导/整理 LLM 回合经 OpenClaw 兼容端点承载、出网沿路径②记账零新增（R1-F4 取 (a)，证据 = §2 T3 第 5 条）；§1.2 外部依赖清单与 NFR-SEC-2 记账面均不动 |
| ADR | `docs/ADR.md` | 改 | 新增 ADR-013（§6 附件逐字）；既有条目原文不动（append-only） |
| 契约 canonical | `docs/contracts/interface-contract.md` | 改 | T5 §1.4 新增行 + 变更记录；T6 :175/:271 措辞对账 |
| 契约消费·固件 | `firmware/main/mcp_server.cc` / `display/story_page.c` / `application.cc` | 未触（本批） | control action 消费扩展 + 整理过渡帧/引导播报渲染 + 聆听形态切换，归实现面承载包（五档分档，非本面落地） |
| 契约消费·front/引擎 | `server/gateway/front/front_app.py` / `server/gateway/story.py` / `server/deploy/engine-patch/connection.py` | 未触（本批） | 计时器族/引导/整理/注入/抑制实现，归实现面承载包；注意 261016-p3-story-body-sd-cache 实现面包 in-flight 同文件（story.py / front_app.py），launch 序约束见 §6 |
| prompt 制品 | front 侧版本化 prompt 文件（新建，路径归实现包定） | 未触（本批） | 引导/整理指令载体（T3 第 5 条）；§5.5.5 不触 |
| 需求面 | `docs/requirements.md` | 未触 | v1.65 已 frozen（需求面档 2 定稿，本批上游权威，不回改） |
| 治理面 | `.claude/**` | 未触 | G1/G2/G3 零命中 |
| 牵连 debt | `docs/debt/tech/TECH-261014-session-end-channels-unwired.md` | 注记义务（非本批落地） | 其「双阈值 60s/120s」范围随本批 120s 作废收窄；spine q4 已裁 = 主线程在本批 arch 面 frozen 后 append 注记（结项不归本批），条目正文 append 不由本 PROPOSAL 落地 |
| 牵连 debt | `docs/debt/tech/TECH-261014-post-speak-listening-lingering.md` / `TECH-261014-engine-asr-hallucination-after-stop.md` | 验收种子转移 | spine q4 已裁 = 转实现面验收种子（注入不尾随 auto-listen case；fixture 含静音段），见 §6 |

> sweep 说明：本面落地只改三个 canonical 文件（见 landing-manifest）；实现面（固件 / front / 引擎补丁 / prompt 制品）归 dev-loop 承载包，不属本面落地范围。census 命令与基线输出见 §5 第 3/4/5 条（起草期实跑）。

---

## 4. human 取舍点（OQ）

**无新增 human 取舍点。** 三个起草期裁定点均已在 spine openQ 授权范围内自裁并落进 §2 改法（各 T 已标注授权来源）：q2（思考态下行触发 = ⓒ，T5）、q3（决策 25 改写形态 = append 注记 + ADR-013 复合先例，T1）、q7-E3（明示卡住检出器 = 关键词表 + 引导 LLM 可返回空，T3 第 2 条）。**R1 否回应改注记（v1.1）**：R1-F4（引导/整理 LLM 回合 provider 出口）按判官所给三选一取 **(a)**——前置条件「OpenClaw OpenAI 兼容端点可携带制品级 system prompt」由起草实证成立（三件证据落 §2 T3 第 5 条），出网沿 §1.2 路径②记账零新增，不触需求冻结面、不新增 human 触点；(b) 新增路径⑤与 (c) 新增 OQ 均未启用。判档建议 = 主路径（轻路径判据 ①②③ 三否：语义变更 + 触契约 + 触 §10/§5.x 承重节），reviewer 只升不降；契约四分类建议 = `ACCOMPANIES_SPEC_CHANGE`（T6），reviewer 独立裁定。

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **PROPOSAL 结构门（pre-commit 干跑；起草期已实跑 2026-10-19）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md` → 预期 `ok:true`、`findings:[]`，anchorUnion 成员 = {docs/ADR.md, docs/architecture.md, docs/contracts/interface-contract.md} ≡ landing-manifest、无夹带路径 token（对照模板读法注第 3 条逐 token 核）。结果留痕：见本节末尾「起草期实跑记录」。
2. **锚完整性（pre-commit 基线已跑 + post-commit 复跑）**：`node .claude/scripts/spec-anchor-check.mjs` → 基线（2026-10-19 实跑）：`ok:true`、`errors:[]`、designIdCount 64；post-commit 复跑预期 `errors` 仍零、designIdCount 不跌（本批不改小节编号、不触 UI 锚）。
3. **120s 语义残留对账（pre-commit 基线已跑 + post-commit 硬断言）**：基线命令 `grep -n "120s\|120 秒" docs/architecture.md`（2026-10-19 v1.1 复跑）命中 :91/:169/:186/:193/:214/:325/:389/:419 八处。post-commit 硬断言（R1-F2 收窄版——pattern 只取不被本批任何新文本引述的旧句全形；「取代 X」式引述属新文本有意留痕、不在断言面）：`grep -n "60s/120s 双阈值\|front 侧计时 sweep 自动收尾\|120s 纯决策" docs/architecture.md` → **零命中**（三 pattern 基线命中 = `60s/120s 双阈值` :91/:389、`front 侧计时 sweep 自动收尾` :214、`120s 纯决策` :193，均落在被替换行内；已对 §2 改法集与 ADR-013 全部新文本逐一反证——新文本仅含「取代 120s 超时 sweep」「〔取代 120s 无语音超时 sweep〕」「`timed_out_sessions` 120s 纯静默决策」「原 120s 设备侧/录音态阈值作废」「v1.46「120s 自动收尾归档」」「取代 120s 纯静默超时」六种引述/改写形，无一命中三 pattern）；:325（§6.4 引擎配置沿革）与 :419（决策 21 历史档）保留不动（人读核对）。
4. **悬空指针对账（pre-commit 基线已跑 + post-commit 硬断言）**：基线 `grep -n "机制细节见 §2.10" docs/architecture.md`（2026-10-19 v1.1 复跑）= 1 命中（:423）。post-commit 硬断言（R1-F1 修订版——T1 正文不动、注记逐字引述旧指针句，断言相应改为计数 + 落点断言而非零命中）：**恰 2 处命中、均落决策 25 条目区间**（内联 append 时按 `grep -o` 或人读计——:423 原句一处 + T1 append 注记引述一处，注记属同一条目内句尾 append；`grep -n` 计行则为 1 行 2 处，人读核对区间归属），且注记行含「实际承载 = §5.1.2 + §5.2 场景④」在场（`grep -n "实际承载 = §5.1.2 + §5.2 场景④" docs/architecture.md` ≥ 1 命中，人读核对其与注记同行）。
5. **契约措辞对账（pre-commit 基线已跑 + post-commit 硬断言）**：基线 `grep -n "manual 聆听\|累积态" docs/contracts/interface-contract.md`（2026-10-19 v1.1 复跑）= 2 行命中（:175 / :271），`grep -o` 词级计数 = 4 词（两行各两词）。post-commit 硬断言（R1-F3 修订版——T5 变更记录与 T6 对账注记有意引述旧词，断言改用具体旧句全形零命中）：`grep -n "网关切该会话为累积态\|即切该会话累积态\|设备 manual 聆听仍经既有 listen 消息承载" docs/contracts/interface-contract.md` → **零命中**（三旧句全形基线命中 = :175「网关切该会话为累积态」、:271「即切该会话累积态」+「设备 manual 聆听仍经既有 listen 消息承载」；已对 T5 插入 1/2、替换 3 与 T6 全部新文本逐一反证——新文本仅含「累积态 / manual 聆听模式」「切累积态 / manual 聆听 / 引擎不感知本信号」式引述片段与「设备聆听仍经既有 listen 消息承载（聆听形态取值归实现面…）」改写句，无一命中三全形）；`grep -n "start_organizing" docs/contracts/interface-contract.md` ≥ 2 命中（§1.4 表行 + 变更记录条）。
6. **契约机械凭据闸（post-commit，合并前）**：`node .claude/scripts/pre-merge-escalation-gate.mjs` 对 `docs/contracts/interface-contract.md` diff 三件联立（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖契约路径）→ 缺一 BLOCK。
7. **逐字 diff 对账（人读，post-commit）**：landing-verifier 逐文件核对实际落地 == 本 PROPOSAL §2 各 T 插入/替换段——T1 一段 append、T2 四片段、T3 整段替换 + 尾句、T4 七处、T5 一行 + 一条变更记录 + 一处节标题括注、T6 三片段、T7 一行；另 §6 ADR-013 附件（ADR.md 尾 append）与 CDQ 行（主线程账本动作）。
8. **边界守持核（人读，post-commit）**：60s 问答超时语义不动；§6.4/决策 21 引擎配置历史叙述原样；归档下游（seq 账本 / story_list 三时点 / 盖章 / 删除链路）零改；`start_organizing` 为 additive 表行、既有三 action 语义不变；§5.5.5 零偏移（`git diff docs/architecture.md` 中 §5.5.5 段无改动——人读 diff 核对）。

**起草期实跑记录**（落地前能跑的先跑；v1.1 全部基线 2026-10-19 复跑重写，数字现跑现写）：第 2 条 spec-anchor-check 基线 = `ok:true`、`errors:[]`、designIdCount 64（2026-10-19 v1.1 复跑）；第 3 条 120s 八处命中复跑不变，F2 收窄三 pattern 基线命中 = `60s/120s 双阈值` :91/:389、`front 侧计时 sweep 自动收尾` :214、`120s 纯决策` :193；第 4 条 `机制细节见 §2.10` 恰 1 命中（:423）复跑不变；第 5 条三旧句全形基线命中 = :175（`网关切该会话为累积态`）、:271（`即切该会话累积态` + `设备 manual 聆听仍经既有 listen 消息承载`），词级旧基线 2 行 4 词复跑不变；第 1 条 preflight 干跑（v1.1 修稿后复跑）= `ok:true`、`findings:[]`、anchorUnion = {docs/ADR.md, docs/architecture.md, docs/contracts/interface-contract.md}（实跑输出摘要，完整 JSON 见会话留痕）。

---

## 6. 留根清单

- **TECH-261014-session-end-channels-unwired 注记义务**：本批 120s 语义作废使其「双阈值 60s/120s timer 零 producer」范围收窄为单阈值；spine q4 已裁 = 主线程在本批 arch 面 frozen 后 append 注记（结项不归本批），条目正文 append 不属本 PROPOSAL manifest。
- **验收种子转移（spine q4 / q7-E4，交实现面承载包 acceptance-author）**：① 注入不尾随 auto-listen case（TECH-261014-post-speak-listening-lingering——`kidpal_speak` 引导注入后引擎不得自动回聆听态破坏记录会话）；② 忠实性 fixture 含静音段（TECH-261014-engine-asr-hallucination-after-stop——静音段幻听不污染整理输入）；③ 忠实性 fixture 显式含孩子中途提问句用例（q7-E4——提问句在 STT 累积输入内，滤除职责全压整理 LLM 指令）；④ 卡壳计时五条边界断言（接着说重置 / 超阈值触发 / 明示卡壳立即触发 / 2 轮无回应自动收尾 / 被抑制轮照常计数，requirements NFR-PERF-3 验证节）；⑤ 真机停顿手感走观感直修长环、闸链后补账（设计稿验收种子节）。
- **261001 spine 结项回填**：Package B 随本批闭环后，261001 spine 的 anchors/结项由主线程回填（spine q1 ruling 原文）；其 q3「说完了是否并入收尾词表」deferred 项随实现面一并裁（未确认前按 S-F2.2-AC2 原文「我说完了/讲完啦」实现）。
- **261016-p3-story-body-sd-cache 合并序**：实现面承载包 launch 前置硬约束 = 该包合并完成（spine facePlan 顺序 3；同文件 story.py / front_app.py）。
- **卡壳阈值拍数**：NFR-PERF-3 标待定，真机调优后立项拍数，本批不拍。
- **整理/引导链路失败的素材保全取舍（R1-N4 留根）**：设计稿与 requirements v1.65 均未覆盖「孩子讲完整个故事而整理 LLM 回合失败」时已累积素材（孩子原话）是否保全 / 如何保全（raw 转写兜底归档 / 重试 / 明示丢弃）——本批只在场景④显式化丢稿口径（当轮素材不归档 + 固定安抚句，见 T3 替换 1 尾句），失败态素材保全取舍归设计/需求面另行补裁，本批不发明新需求语义。
- **CDQ 登记**（主线程账本动作，landing 时 append 到 `docs/CONTRACT-DECISION-QUEUE.md`，非 §2 改法 manifest 目标——261017 R2 NIT-3 裁定同款：CDQ 是跨时间账本、非被 diff 的契约路径；状态 PRECHECKED→LANDED 随 landing-verdict 翻）：行草案如下，供主线程 landing 时逐字用：

```text
| CDQ-261019-p3-record-dialogue-guide | 2026-10-19 | interface-contract.md 三触点：① §1.4 新增 `start_organizing` 控制动作（记录会话收尾 → 整理过渡态；网关决定类、引擎 display 流拒转）+ 表后变更记录条；② §1.4 `start_recording` 行 + §四 `kidpal_session_ctx` 条措辞对账（「累积态 / manual 聆听模式 / 引擎不感知本信号」→ 记录语境对话态 + 引擎侧抑制由 front 内部注入等价信号承载；wire 形状零改）；③ §1.4 节标题括注对账（「OpenClaw 判意图后下发」→「OpenClaw 判意图或 front 检出后下发」——`start_organizing` = front 检出不经 OpenClaw） | ACCOMPANIES_SPEC_CHANGE（随 requirements v1.65 + ADR-013 同批原子落） | 261019-arch-review-p3-record-dialogue-guide PROPOSAL（reviewer GREEN + landing-verdict PASS） | PRECHECKED | 261019-p3-record-dialogue-guide | 实现面承载包（五档分档，待 261016-p3-story-body-sd-cache 合并）；ADR-013 随批落 |
```

**ADR-013 落地附件**（packet-bound landing 逐字 append 到 `docs/ADR.md` 尾）：

```text
## ADR-013 · 记录语境聆听语义 = 累积 + 卡壳轻引导 + 收尾忠实整理（对话式引导记录）；记录语境期间引擎侧抑制 LLM 回合（supersedes 决策 25 机制面）

- **日期**：2026-10-19（`261019-p3-record-dialogue-guide` 架构+契约面 Track B；261001-m4-k3-record-session-live-repair Package B 同批吸收定形）
- **决策**：① 决策 25「闲聊与录故事两条聆听语义分流」本体保留，第二条形态由「manual 聆听 + 网关累积（停录整段转写归档）」取代为「记录语境对话态 = 累积 + 卡壳轻引导 + 收尾一次性忠实整理」——聆听产逐句 stt（manual 模式作废）；卡壳计时器族（每句讲完起计时、接着说重置、计时到或明示卡壳词命中触发一句轻引导；引导 LLM 可返回空 = 本轮不播报仍计轮；连续 2 轮无回应自动收尾，取代 120s 纯静默超时；阈值权威 = requirements NFR-PERF-3）；整理 = 只用孩子原话与事实（忠实整理），引导语 / 孩子中途提问与 AI 回答滤除不进素材。② **Package B 定形**：记录语境期间引擎侧不对 stt 跑 LLM 回合（261001 human 触点①裁读法乙「全程无 LLM 回合」）——`kidpal_session_ctx` 仍由 front 拦截不下透，front 经既有内部注入通道（ADR-003 `kidpal_speak` 同型、引擎补丁认领）向引擎注入等价记录语境信号使引擎侧跳过 LLM 派发；front↔引擎为服务端内部链路，信号形状不入契约。③ 收尾检出（语音收尾词 / 再按 KEY / 连续 2 轮引导无回应自动收尾）→ front 下发 `start_organizing` 控制动作（契约 §1.4 新增，网关决定类）→ 设备切整理过渡演出（思考态复用）→ 整理完成 `story_text` 下发离态。④ **provider 出口钉**：引导/整理两条一次性 LLM 回合经既有 OpenClaw OpenAI 兼容端点承载（loopback 127.0.0.1:18789；request 级 system prompt 携带 front 侧版本化制品指令、不走主 agent 共享 system prompt——端点请求级 system 消息承载机制 = §5.5.5 版本化同款，front 侧适配 = provider.py `LLM_OPENCLAW_OPENAI` + `openclaw_stream_request` 既有 additive 适配器），出网沿 §1.2 路径②记账、零新增出网路径（R1-F4 裁 (a)）。
- **理由**：技术证成 = manual 模式 ASR 在 listen stop 才触发、录音期间产不出逐句 stt，收尾词前置检测与卡壳引导在 manual 下物理不可能（261001 q8 真机复核：stt 在停录后 1.7 秒才回）；产品证成 = 孩子对一台沉默设备讲完完整故事实际很难（卡壳即放弃），叙事脚手架对 8–12 岁目标是真实价值，且对话态下问答天然被滤出不进素材、2026-09-14「提问也被记录」一刀切的代价随之消解（human 2026-10-19 逐段批准设计稿）。引擎侧抑制取 front 注入等价信号：引擎→OpenClaw 的 loopback 直达调用不经 front，front 截不住（261001 q6），抑制必须在引擎侧；注入通道复用 ADR-003 已验证形态，不新建链路。`start_organizing` 取新增下行信号：三式收尾有两式（语音收尾词 / 自动收尾）检出在云端，设备无从自知；设备侧镜像扫 stt 覆盖不全（自动收尾镜像不出）且词表双源漂移，容忍停留 REC 屏违反 v1.65 AC4 已冻结承诺。
- **备选与放弃原因**：① 维持 manual 聆听 + 停录整段转写——放弃：产不出逐句 stt，语音收尾与卡壳引导物理不可能（261001 q8）；② front 侧抑制 LLM——放弃：引擎→OpenClaw loopback 调用不经 front、截不住（261001 q6 已裁引擎侧再动）；③ 思考态触发 ⓐ 设备侧镜像扫 stt 回显帧——放弃：收尾词表双源漂移 + 自动收尾（云端 2 轮判定）设备无从镜像、覆盖不全；④ ⓑ 容忍停留记录屏到 story_text 到达——放弃：违反 v1.65 AC4「整理期间思考态盖住延迟」冻结承诺，数秒级整理延迟下 REC 仍闪 = 误导；⑤ 每段接话 / 故事引导+纯录双模式 / 边讲边增量整理——放弃：设计稿被否方案留痕（故事切成一问一答碎片、入口意图分类成关键路径、贵且易跑偏）；⑥ front 直连 LLM provider（新增 §1.2 出网路径⑤）——放弃：front→OpenClaw loopback 适配器既有在册（GWA-E4 additive registry）、端点请求级 system prompt 承载能力已实证，直连徒增一条出网记账路径与 NFR-SEC-2 对账成本、零收益。
- **关联**：决策 25（§10 取代注记）；261001 spine（Package B / q6 / q8）与 261019 spine（q1 吸收 / q5 Track B / q6 计轮 / q7-E3）；requirements v1.65（S-F2.2 / NFR-PERF-3 / §7 开放问题 7 撤注）；契约 §1.4 `start_organizing` + §1.4 节标题括注 + `start_recording` 措辞对账 + §四 `kidpal_session_ctx` 尾句对账；ADR-001（指令制品接线不变式）/ ADR-002（混合分层同型切分）/ ADR-003（`kidpal_speak` 注入先例）；设计权威 = delivery/intake/261019-p3-record-dialogue-guide.design.md（human 2026-10-19 逐段批准）。
```

---

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · architect-reviewer · 2026-10-19 · verdict: BLOCK(F1,F2,F3,F4)

前置核验（判官亲核，非转述采信）：PROPOSAL 引用的 file:line 与「原」列逐字核对全部属实——architecture.md :91/:99/:169/:173/:186/:193/:214/:293/:389/:423/:438/:442-444、interface-contract.md :171-175/:271、requirements.md S-F2.2 :453-467 / NFR-PERF-3 :614-618 / §7 开放问题 7 :694 / §9 v1.65 行 :730 / §2.1 :47·:57 / §2.3 :153、notes/arch-restructure-skeleton.md:48、story.py:55/:124-135/:598-604、front_app.py:312-319、display_schema.py:43-45/:70-77（实际路径 server/gateway/display_schema.py，PROPOSAL 未误标路径）、mcp_server.cc:1236-1247/:132。起草期基线命令复跑实证：120s grep 恰 8 命中（:91/:169/:186/:193/:214/:325/:389/:419，其中 :325/:419 确属决策 21 引擎侧配置沿革、应保留）；「机制细节见 §2.10」恰 1 命中；契约「manual 聆听|累积态」恰 2 行（:175/:271）；landing-fidelity-check.mjs --preflight 实跑 ok:true、findings:[]、anchorUnion 三文件 ≡ landing-manifest；spec-anchor-check.mjs 实跑 ok:true、errors:[]、designIdCount 64（ARQ-261017 既有噪声未计入，本次也无 story 锚告警出现）。

【查项① 意图保真——PASS】目标效果、非目标、翻案留痕忠实于 spine humanVerbatim 与 human 逐段批准设计稿；设计稿六步机制、忠实整理护栏、收尾三式、2 轮自动收尾、被否方案留痕（ADR-013 备选⑤）均已承载。三处起草期自裁均在 spine openQ 授权范围内且依据实质引用：q2=ⓒ（ruling 明文「归 /arch-change 内裁」，三候选评估在 T5 证据列）、q3=复合先例（ruling 明文「归 /arch-change 起草时显式裁定」；决策 4/6 取代注记 :402/:404 与决策 24 句尾 append :422 双先例亲核在场，冻结纪律缺口论证成立）、q7-E3=关键词表+可返回空（ruling 明文「起草时写明」，T2/T3 落点正确；q6 计轮裁定与收声死锁修法闭环一致）。Package B 吸收依据（261001 q6 读法乙 / q8 真机复核）逐字属实。

【查项② 纪律合规——4 BLOCK（验证集与改法集自相矛盾）】三份分工句守持良好；§10 冻结纪律复合先例成立；ADR 五件格式合规（与 ADR-011/012 同构，ADR-013 为下一空号亲核）；契约包三节字段名精确；landing-manifest 围栏块与 packet-id 行在场且 preflight 实测绿；T7 changeId 取 spine changeId、无 FROZEN 占位 token。但 §5 验证集三条 post-commit 硬断言被本批自己的改法文本击穿：

- R1-F1｜查项②｜BLOCK｜PROPOSAL §5 第 4 条（:289）vs T1（:55、:61）：第 4 条断言 post-commit `grep -n "机制细节见 §2.10"` 零命中，括注理由「T1 注记改写该尾句」与 T1 改法「:423 正文不动、句尾 append 注记」直接矛盾，且 T1 插入 1 注记文本本身逐字含「机制细节见 §2.10」——post-commit 实际 = 2 命中，断言必然不可满足。建议：断言改为「恰 2 命中、均落决策 25 条目区间（原句 + 注记引述），且注记行含『实际承载 = §5.1.2 + §5.2 场景④』在场」。
- R1-F2｜查项②｜BLOCK｜PROPOSAL §5 第 3 条（:288）vs T2 替换 2（:86）、T3 替换 1（:122）：硬断言 pattern 含「120s 超时 sweep」「120s 无语音超时」，而 T2 新文本含「卡壳计时 sweep（取代 120s 超时 sweep；…）」、T3 新文本含「〔取代 120s 无语音超时 sweep；…〕」——「取代 X」引述句逐字含 pattern，post-commit 断言必然非零命中。建议：pattern 收窄为不被任何新文本引述的旧句全形（如「front 侧计时 sweep 自动收尾」「`timed_out_sessions` 120s 纯决策」「60s/120s 双阈值」→ 零命中），或明写排除含「取代」引述行的规则。
- R1-F3｜查项②｜BLOCK｜PROPOSAL §5 第 5 条（:290）vs T5 插入 2（:196）、T6 替换 1（:217）：「manual 聆听 / 累积态 均零命中」同样被自身击穿——T5 契约变更记录与 T6 对账注记都逐字引述「累积态 / manual 聆听模式」。建议：断言改用具体旧句全形（「网关切该会话为累积态」「即切该会话累积态」「设备 manual 聆听仍经既有 listen 消息承载」→ 零命中；`start_organizing` ≥ 2 命中保留）。
- R1-F4｜查项④ 牵连面｜BLOCK｜PROPOSAL T3 第 5 条（:117）+ 契约包 impact（:253）vs architecture.md §1.2（:38-40）：引导/整理两条一次性 LLM 回合的 provider 出口未钉。impact「OpenClaw 本体零影响」+ 第 5 条「不走 OpenClaw 主 agent 共享 system prompt」合读强烈指向 front 直连 LLM provider（经 OpenClaw agent 回合必吃主 system prompt）——而 §1.2 出网路径枚举 = ①②③④（② = DeepSeek 经 OpenClaw，NFR-SEC-2 记账面），front 直连 DeepSeek = 第五条出网路径，本批 §1.2 未动、impact 未声明记账去向，需求面 v1.65 已 frozen 不可回改。出网记账是 human 记账面，不能留给实现包默认决定。建议三选一并落进 T3/impact：(a) 钉「经既有 OpenClaw OpenAI 兼容端点承载、出网沿②记账零新增路径」（须先证实该端点可携带制品级 system prompt）；(b) front 直连 → 本批随批 append §1.2 出网路径⑤行并显式处理与 requirements NFR-SEC-2 的记账对账（涉需求面冻结面，须呈 human 触点）；(c) 列为新增 OQ 升 human 裁。

【查项③ 撞车复核——PASS（附 2 NIT）】与既有节 / ADR 既有条目 / 决策历史档无冲突；与 261001 Package B 定形口径一致（q6 读法乙、q8 顺序缺陷、pending-stop 保留均对齐）；§5.5.5 不触论证成立（:298 标题即限定 OpenClaw system prompt 单制品）；:325/:419 引擎侧 120s 反证保留面亲核确认属决策 21 配置沿革、应不动；§2.4 原则 3「网关恒薄」张力处置成立（机械层/语义层切分沿 ADR-002 同型 + ADR-004 机械计算先例）。

【查项④ 牵连面完整——1 BLOCK（R1-F4）+ NIT】120s 残留自 grep 复核确认恰八处、T 集覆盖六处、:325/:419 正确保留；requirements v1.65 各联动点被 T 集正确消费或正确声明未触；场景⑤ :216「录音态期间提醒排队延后（v1.46）」规则被 v1.65 明文延续、不需动；TECH-261014 三条目处置与 spine q4 裁定一致；261016 in-flight 合并序约束在 §3/§6 写死；架构侧「累积态/manual 聆听」残留全文件 grep 确认无漏网。另：

- R1-N1｜查项②｜NIT｜PROPOSAL :196 vs :223/:250/:309：T5 插入 2 的契约变更记录枚举漏列 T6 替换 2（§四 kidpal_session_ctx :271 措辞对账），而 proposedContractDiff 与 CDQ 行草案均列明两触点。建议补枚举或在 §四 另立记录条。
- R1-N2｜查项③｜NIT｜interface-contract.md :165 vs T5：§1.4 节标题「控制类指令（OpenClaw 判意图后下发）」对 start_organizing（front 检出、不经 OpenClaw）字面失配。建议标题括注对账为「OpenClaw 判意图或 front 检出后下发」。
- R1-N3｜查项④｜NIT｜architecture.md :144-152：§4 NFR 对策映射表未随 v1.65 新增 NFR-PERF-3 行（表头自述「每条 NFR 指到满足它的机制」），而 T2 恰在重写其机制承载行。建议补一行映射（front 卡壳计时器族 / §5.1.2 + §5.2 场景④）。
- R1-N4｜查项①/④｜NIT｜PROPOSAL :122：整理/引导链路失败的丢稿姿态未显式化——只写「§6.3 固定安抚句 + text 视图结束当轮」，隐含全程素材丢弃（孩子讲完整个故事而整理失败即全失），设计稿与 v1.65 均未覆盖该失败态。建议场景④补一句丢稿口径，或登记留根由设计/需求面补失败态取舍。

备注（非 finding）：① spine faceMap 将 :91 标为「§2.2 容器图」，实际 :91 属 §2.3 端云职责表——PROPOSAL 按 §2.3 处置正确，属 spine 侧行号漂移被起草期正确吸收；② ADR-013 备选⑤收了设计稿六条被否方案中的三条，未收的三条（润色改写/双版本并存/按情节段落引导）属需求面已裁的内容形态议题，可接受。

【判档结论（只升不降）】主路径维持，无降档空间——轻路径四判据全不成立：① 多处语义变更；② 改法性质 = 机制重写（计时器族换代 + 链路重写 + Package B 定形）；③ 触契约（T5 新增表行 + T6 对账）；④ 触 §2.3/§5.1.1/§5.1.2/§5.2/§5.5.4/§9 承重节。Track A 段升级判断：不升——Track A 判据按 SSOT 只看收敛度，方向已由 human 逐段批准设计稿 + 立项 ack（q5 显式裁 Track B）收敛；wire 新增一枚 additive 控制动作不改变收敛度结论（261016 kidpal_story_fetch 新增请求-应答环先例同款）。

【契约四分类结论（独立裁定）】T5（新增 start_organizing additive 表行）= ACCOMPANIES_SPEC_CHANGE——现行契约无该下行信号（非 NO_CONTRACT_CHANGE / IMPLEMENTATION_ADAPTER）；随本批 arch 变更携带、与已 frozen 的 v1.65 AC4 同向原子落、无「裁改哪边」冲突（非 SPEC_CONTRACT_DRIFT / CONTRACT_CHANGE_REQUIRED）。T6（:175/:271 措辞对账）= ACCOMPANIES_SPEC_CHANGE——契约措辞追上已 frozen 需求，wire 形状零改，定性正确。


### R2 · architect-reviewer · 2026-10-19 · verdict: GREEN

R2 轮次纪律：聚焦 R1 四 BLOCK + 四 NIT 的修订到位性与修法波及面，不重开全四查；修法制造的新问题逐条核。全程只读亲核，未采信修稿自述。

【一、R1-F1/F2/F3 新断言反证（重点①）——三条全部闭合，反证属实】

- F1（§5 第 4 条，:301）：断言改为「恰 2 命中、均落决策 25 条目区间（人读核对区间归属）+ 注记行含『实际承载 = §5.1.2 + §5.2 场景④』在场（grep ≥ 1）」。亲核 T1 插入 1（:61）确实逐字含该句；断言与 T1 改法（正文不动 + append）自洽，R1 矛盾消除。
- F2（§5 第 3 条，:300）：pattern 收窄为 `60s/120s 双阈值` / `front 侧计时 sweep 自动收尾` / `120s 纯决策`。判官自行复跑基线 grep：命中恰 :91/:389（双阈值）、:214（front 侧计时 sweep 自动收尾）、:193（纯决策），与 PROPOSAL 自述逐字一致、均在被替换行内。反证独立复核：逐一扫过将落盘的全部新文本（T1 注记、T2 替换 1-4、T3 替换 1-2、T4 替换 1-7 与插入 6、T7 行、ADR-013 附件）——「取代 120s 超时 sweep」「〔取代 120s 无语音超时 sweep〕」「`timed_out_sessions` 120s 纯静默决策」等引述形均不命中三 pattern（「120s 纯静默决策」不含「120s 纯决策」子串）。断言可满足。
- F3（§5 第 5 条，:302）：pattern 收窄为三旧句全形。基线 grep 复跑恰 :175（网关切该会话为累积态）、:271（即切该会话累积态 + 设备 manual 聆听仍经既有 listen 消息承载），与自述一致。反证独立复核：T5 插入 1/插入 2/替换 3 与 T6 新文本均不含三全形（插入 2 的「切累积态 / manual 聆听 / 引擎不感知本信号」「累积态 / manual 聆听模式」引述、T6 新 B「设备聆听仍经既有 listen 消息承载」去 manual 改写均不构成命中）；`start_organizing` ≥ 2 断言保留；契约包 validationPlan（:266）已同步收窄。断言可满足。

【二、R1-F4 取 (a) 三件实证逐字核（重点②）——全部属实，出口钉成立】

- 实证① apply-xiaozhi-overlay.sh（:110-124 区，引用 :115-119）：亲核确认版本化 prompt 以顶层 `prompt: |` literal block 注入 .config.yaml，注释明言 connection.py 以 config["prompt"] 取用。
- 实证② engine-patch connection.py：:764-768 = 引擎读 config["prompt"] → get_quick_prompt → change_system_prompt；:1191-1192 = change_system_prompt → dialogue.update_system_message——请求级 system 消息承载机制成立，且这正是生产上 prompt v1→v11 版本化生效的既有载体（§5.5.5 同款），端点对调用方自带 system 消息的遵从性有生产实证背书。
- 实证③ provider.py：:31 OPENCLAW_BASE_URL = http://127.0.0.1:18789、:45 LLM_OPENCLAW_OPENAI、:53 registry、:97-108 openclaw_stream_request(messages) 全量 messages 构造、恒不带 max_tokens——逐行属实；该函数有冻结验收 needle（test_gateway_directionB.acceptance.py GWA-A4 断言在册），「front 侧适配器既有在册、零新增 provider 族」成立。
- 佐证 v21 探针（:15-17）：亲核确认「独立调用不传历史 → 『没有记录』（stateless）+ 显式透传全历史 → 正确回」——每轮行为由请求全量 messages 转向，支撑 request 级 system prompt 携带制品指令的可行性。
- 记账面：front→18789 为 loopback 内部调用、真实出网在 OpenClaw 内部→DeepSeek = §1.2 路径②（architecture.md:38 亲核）；§3 sweep 新增「§1.2 出网枚举 未触」行（:275）在场；ADR-013 决策补④（:331）+ 备选补⑥（:333）在场；impact（:264）已钉死出口与记账；§4 OQ 节记录取 (a)、(b)/(c) 未启用（:292）。
- 语义自洽性：「不走主 agent 共享 system prompt」与「§5.5.5 版本化同款」表述自洽——共享 v11 制品不经由这两条回合的 system 消息承载；agent 侧行为残余风险（工具调用等）已由实现期断言种子显式接管（T3 第 5 条末句，v11 路由表同款纪律），归位正确。F4 闭合，无新 human 触点。

【三、N1-N4 修法与计数同步（重点③）——四处修毕，发现一处同步残留】

- N1：T5 插入 2（:203）已补枚举 §四 kidpal_session_ctx 措辞对账 + §1.4 节标题对账（四项全列）；CDQ 行草案（:322）改「三触点」齐；新增枚举文本经反证不击穿 F3 三全形断言。
- N2：T5 替换 3（:205-207）原文列与 interface-contract.md:165 实文逐字一致；新括注消除失配；§5 第 7 条与 proposedContractDiff 均已计入「一处节标题括注」。
- N3：T4 替换 7（:174-177）原文列与 architecture.md:147 实文逐字一致；新行 NFR 名与 requirements :614 全名一致、机制指向与 T2 一致；T4 标题/问题/证据/根因/改法四处计数改「七处」、§3 sweep 与 §5 第 7 条同步。但发现一处同步残留：T7 的 §12 行草案（:252）改动面列未随 N3 更新——见 R2-N1。
- N4：T3 替换 1 尾句已显式化「当轮素材不归档（孩子已讲内容不写成故事、不分配 seq）」；§6 留根新增「失败态素材保全取舍」条目（:318），「不发明新需求语义」框架正确，与设计稿/v1.65 无冲突。

【四、修法波及面与新矛盾扫描（重点④）】

v1.1 preflight 判官复跑 ok:true、findings:[]、anchorUnion 三文件 ≡ manifest；F2/F3 三 pattern 对全部将落盘新文本的反证独立通过；N1 补枚举不击穿 F3；修法未触 T1/T2/T3 主体、T6、契约四分类与判档结论（F4 取 (a) 不新增 wire、不动契约面，ACCOMPANIES_SPEC_CHANGE 裁定不变）。spec-anchor-check 基线 R1 已亲跑 ok:true、canonical 本轮未动，无需重跑。

- R2-N1｜查项②（计数同步残留）｜NIT｜PROPOSAL :252（T7 §12 行草案）vs T4 替换 7（:174-177）：§12 改动面列现为「…+ §2.3 / §5.1.1 / §9 to-be② / §5.5.4 / §2.4 原则 3 联动对账」，缺「§4 NFR 对策映射表补 NFR-PERF-3 行」——N3 修法给 T4 增了第七处、§12 行是变更账本，改动面枚举应完整。建议 T7 围栏行在「§5.5.4」后补「§4 NFR 映射表」。
- R2-N2｜查项②（断言措辞健壮性）｜NIT｜PROPOSAL :301（§5 第 4 条）：「恰 2 命中」未钉 grep 计行还是计匹配——若落地时注记以同段内联 append 进 :423 行，grep -n 计行 = 1、字面执行会误报。断言已含人读核对与确定性次级断言（「实际承载…」grep ≥ 1），风险低；建议措辞钉为「恰 2 处命中（内联 append 时按 grep -o 或人读计）」彻底消除歧义。

verdict: GREEN——R1 四 BLOCK 全部修毕且修法经判官独立反证属实，四 NIT 修法到位；两条新 NIT（R2-N1、R2-N2）均不挡 GREEN，建议随 FROZEN 前最后一稿顺手收掉（均一行改动）。判档结论（主路径）与契约四分类结论（T5/T6 = ACCOMPANIES_SPEC_CHANGE）维持 R1 裁定不变。主线程可凭「立项 ack + 本 GREEN + 判档结论」标 FROZEN；landing-manifest 围栏块与 packet-id 行在场已随 GREEN 复核（结构字段硬条款）。
