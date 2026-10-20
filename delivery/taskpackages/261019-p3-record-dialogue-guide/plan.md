---
changeId: 261019-p3-record-dialogue-guide
rigor: full
riskTier: high
sourceType: proposal
reviewed-by: architect
date: 2026-10-19
---

# 建什么 + 方案（P3 记录会话对话式引导记录：server 计时器族与引导/整理链路 + 固件两新态与聆听形态切换）

> **档位**：②单包 Full（architect 分流原判即 ②单包 Full，human 触点① 2026-10-19 确认；riskTier=high、rigor=full）。
> **changeId 复用说明**：沿用 `261019-p3-record-dialogue-guide`——跨面单一变更（spine / 需求 §9 v1.65 / 架构 §12 + ADR-013 / 契约 CDQ-261019 条目均已钉本 id），spine 与本包分属不同树不同 INDEX lane，复用保「一变一 id」跨面可追。
> **实现面范围的唯一权威** = `delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md`（FROZEN v1.2）§2 T1–T7 改法集 + §5 验证集 + §0 非目标；本 plan 只做实现侧分解，不改任何 FROZEN 裁定。

## 建什么

P3 记录会话从「闭嘴累积、停录整段转写」变**对话式引导记录**（requirements v1.65 frozen / S-F2.2 五 AC / NFR-PERF-3）：孩子讲着时 AI 默认闭嘴（切句只回显），停顿超卡壳阈值（起步约 8 秒、**待定**）或明示卡住时 AI 看着已讲内容轻声引导一句（**停留记录屏不跳页**），说「我说完了/讲完啦」、再按 KEY 或连续 2 轮引导无回应时收尾，全程孩子的话一次性**忠实整理**成篇归档（只用孩子原话，去口水/断句/分段/标题，不换表达不补情节）；引导语、孩子中途提问与 AI 回答都不进素材（废除旧一刀切，翻案留痕见 S-F2.2 补充规则）。

三端一次定形：**server**（卡壳计时器族 / 引导与整理 LLM 回合 / `start_organizing` 下发 / Package B 引擎抑制等价信号）+ **engine-patch**（记录语境标志认领 + chat 入口抑制 + stop-truncation 豁免重绑）+ **固件**（聆听形态切换 + `start_organizing` 消费 + 引导播报中/整理中两新态渲染 + 思考态接线）。

**上游 anchors（全已合 main e65a8731）**：req = requirements §9 v1.65；arch = 261019-arch-review 批 landing（五改法点 + ADR-013）；contract = CDQ-261019-p3-record-dialogue-guide LANDED（§1.4 新增 `start_organizing` + 两处措辞对账，wire 其余零新增）。硬前置 261016-p3-story-body-sd-cache 已合并（b2e2ceb3）。来源 = spine `delivery/intake/261019-p3-record-dialogue-guide.md`（q1-q11 全 resolved）+ design.md。

## 方案

### 实施分段（单包内 S1–S5，顺序推进，不拆 Pod）

拆包被否理由（architect 评审）：server 与固件是同一原子行为单元的两半——只有 server = AC2/AC4 半个功能不可见，只有固件 = 无东西可消费；拆两 Pod 串行依赖无并行收益，反把一套 FROZEN 决策集拆成两份记账。单包内分段即可达同等节奏控制。

| 段 | 内容 | 验收齿归属 |
|---|---|---|
| **S1 server 纯决策面** | story.py：明示卡壳词表检测纯函数（is_finish_phrase 同构子串扫描，词表 = 实现期可调常量）；StoryAccumulator 卡壳计时与引导轮次纯决策（每句活动时刻重置 = 既有 last_activity/feed 机制；轮次计数 + stuck-due + 2 轮无回应收尾决策；取代 timed_out_sessions 120s 语义——AUTO_STOP_SECONDS/timed_out_sessions 退役或重述，**既有冻结验收文件同步更新**）；忠实整理输入装配纯函数（只取孩子的话 = accum 拼接）；两枚 front 侧版本化 prompt 制品文件（新建，路径包内定；ADR-001 三面相等接线不变式；不走 OpenClaw 主 agent 共享 system prompt） | **host**（pytest 纯决策，时钟注入快进；NFR-PERF-3 五条边界全可 host 闭环） |
| **S2 server 接线面 + engine-patch** | front_app.py：_story_timeout_sweep 重写为卡壳计时 sweep（stuck-due → 引导回合；轮次 ≥2 仍无回应 → 自动收尾走整理归档；pending-stop 10s 挂起窗口保留）；引导 LLM 回合（累积孩子的话 + 引导制品指令 → openclaw_stream_request → loopback 18789；返回空 = 本轮不播报仍计轮）+ kidpal_speak 注入播报（reminders.py build_speech_injection/deliver_alert 先例复用；**录音期提醒排队延后规则不得被引导注入破坏**，requirements :68 唯一例外）；忠实整理 LLM 回合（一次性、忠实指令、标题 ≤12 字同回合产出）→ archive_story（既有零改）→ story_text(seq) + story_list 三时点②推送；三式收尾统一下发 start_organizing（重复幂等；KEY 路径设备本地先切 = 包内取舍，wire 语义不变）；Package B 等价信号注入引擎腿（进语境 = session_ctx/start_recording 两入口收敛点；出语境 = 三式收尾/取消/断连；服务端内部注入、ADR-003 kidpal_speak 同型、不入契约）；**display_schema.py 自推校验通路改造**（结构性风险 1，见下）；R1-N4 失败路径：当轮素材不归档 + §6.3 固定安抚句 + text 视图结束当轮。engine-patch/connection.py：认领等价信号置记录语境标志；**chat() depth==0 入口守卫块（:1204-1217，既有丢弃 + 幻听过滤同处）扩第三守卫：记录语境中跳过 LLM 派发**（dialogue.put 与 TTS FIRST 均在块内，跳过即零 LLM 零 TTS；stt 下行在 ASR 路径先于 chat、回显与 front 累积不受影响——未核 1，实现期先验证再定挂点）；**stop-truncation 豁免重绑**（结构性风险 3，见下） | **host + live**：机械面 host（front 纯函数 + 引擎补丁单收口离线可测先例 connection.py:91）；LLM 行为与抑制链 live（真 OpenClaw loopback + 假 stt 序列驱动，无设备可验：引导注入 / 引擎零 LLM 派发 / 整理产出 / 归档下发） |
| **S3 固件聆听形态 + 引导子态** | application.cc：记录语境聆听模式从 ManualStop（:974/:977 StartListening 路径）切到产逐句 stt 形态（AutoStop/Realtime 取舍包内裁——realtime 需 AEC 开，QA KEY 路径 :926 同族先例）；**kvr 场景守卫**：:776 ctx 选取现只按 listening_mode_，切非 manual 后每句 stt 会误映 KVST_SCENE_THINK → SetStatus(THINKING) 跳页——记录语境必须保持「切句只回显不进思考」（:773 守卫既有，:776-781 ctx 选取与记录语境标志联挂）；story_page.c 帧 A 修订（气泡「你讲吧，我听着呢」+ 左演出层顶部 24/28 位式 + 右槽「丢弃」——未核 3，现状按增量改）+ 帧 B 引导播报中子态（REC/大时长/声波/回显常驻、气泡换引导语、键帽左「打断」右留空；KEY 短按 = 打断引导继续聆听——AbortSpeaking 路径 :927-928 既有；**BOOT = 无操作——现 RECORDING 键位映射 BOOT 任一手势 = DISCARD_RECORD（story_page.c:1667-1673），子态不拦截则引导一句话期间按 BOOT 即毁稿，本包固件侧最关键的键位分叉**；子态载体 RECORDING+标志 vs 独立枚举包内裁） | **host_app 可闭环逻辑齿**（键位映射子态分支/回显/recording_enter/kvr 场景映射）+ **真机**（VAD 真沿、AFE/AEC realtime——host 不可机判不造 offline 假齿，human 目检兜底） |
| **S4 固件整理过渡态** | view_router.h/.c：VRT_CTRL_START_ORGANIZING = 3 **additive** 枚举（:35-40 冻结数值码 0/1/2 不偏移）+ kidpal_display_control_action 映射；mcp_server.cc 控制 switch 增 case 3（:1230-1263 区，story_consumed 日志同型）；story_page 新增 KSTORY_PAGE_ORGANIZING（枚举尾部 additive）：V7 state-organizing 帧渲染（pose-think 复用 + 三问号组 + 波浪线 + 云朵 + 「你刚说」标签 + 右区刚讲内容〔回显环 s_story_echo :1991-2048 可复用〕；键帽槽留空；**KEY/BOOT 均不响应**）；离态 = story_text 到达（既有渲染路径）或降级安抚 text 帧；**思考态接线（Phase 1 未接线修复项）——仅记录语境整理过渡这一条触发链，普通问答链路思考态不碰**（arch §9 to-be③ 原样保留） | **host_app**（on_control case 3 / 键位不响应 / 渲染分支纯逻辑齿）+ **真机**（state-organizing 帧真机可见 = 翻✅条件③） |
| **S5 端到端联调** | 全链真机轮：双入口 → 闭嘴回显 → 卡壳引导播报（不跳页）→ 三式收尾 → start_organizing → 整理中 → story_text → 盖章；S-F2.2 五 AC 全链 | **真机**（强制；含 S-F1.4 AC6 录音期提醒排队不回归、观感直修长环后补账） |

分段序：S1 → S2（server 独立 host+live 验收完毕再起固件）→ S3/S4（同 Pod 序贯）→ S5。固件消费已 live 验证的 server 行为面，联调期缺陷归因成本最低。

### 结构性风险（包计划期与实施期义务，plan 显式列）

1. **display_schema 自推校验卡点**：display_schema.py:70-77 对 GATEWAY_DECISION_CONTROL_ACTIONS 一律拒转（防引擎 display 流伪造），但 front 自推缝 _push_display_payload（front_app.py:164-176）同过 validate_display——start_organizing 入网关决定类后**自推会被自拒**。避让 = 自推通路加分叉（自推专用校验或 validate 增 self_push 形参），relay 路径拒转语义一字不动；既有冻结验收齿（end_session 拒转断言）必须保绿。擦边警觉：display_schema 分表演进属 server 侧机械承载、不触 docs/contracts/**；若实现中需要给 start_organizing 加 data 字段或改设备行为语义超出现行表行文字 → 立即停走 §6 逃生门 reviewer-first，不得原地扩。
2. **engine-patch stop-truncation 豁免重绑（高危）**：connection.py:457 截断豁免现按 `client_listen_mode != "manual"` 判定——记录语境恰是 manual 豁免的唯一在役受益方（pending-stop 迟到转写承重通道，:442-443 注释自述）。记录语境切非 manual 后豁免自动失效 → **KEY 收尾路径的末句转写被截断清空 = 毁稿**。避让 = 豁免判据从「manual 模式」重绑为「记录语境标志」（Package B 等价信号自然提供）；261014 既有截断齿（非记录会话 stop 截断 + 幻听过滤）保绿。
3. **同文件交界（261016 已合部分）**：交界评估 = 低——261016 落点 fetch_story（story.py:478-506）/ kidpal_story_fetch 消费（front_app.py:267-284）与本包改动区（StoryAccumulator 类 / _story_timeout_sweep / pump 内 story 缝）不同函数；避让 = 不动 fetch_story/_safe_story_path/kidpal_story_fetch 三块（_safe_story_path 是 delete/fetch 共享单源，本包零需求碰它）。
4. **application.cc × 261014 语音轮三守卫**：tts stop 出口判据③（:718-721）在引导 speak 到达时轮来源 = Listening → 播完回 Listening，正是所需；但聆听模式切 realtime 后判据①（manual → Idle）不再覆盖记录语境，三守卫行为须在新模式下重验；:776 kvr ctx 选取联挂记录语境标志（S3 义务已列）。
5. **story_page.c 接线量**：2177 行、5 个既有页态枚举数值被 host 验收钉死——新增全 additive（枚举尾部 append + switch 分支 + 键位映射分支 + on_control 扩展）；**唯一改写点 = RECORDING 键位 case 子态化**（帧 B BOOT 毁稿分叉）。

### 逃生门核验（architect 评审结论）

实现消费零新增契约/spec 改动——start_organizing 已随 arch 批 LANDED，固件 VRT/mcp_server/display_schema 全 additive 消费；等价信号按 ADR-013「服务端内部链路、形状不入契约」。

## 验收种子（交 acceptance-author 冻结；spine q4/q7 转注逐条落位）

1. **注入不尾随 auto-listen**（q4 / TECH-261014-post-speak-listening-lingering）：引导 kidpal_speak 注入后引擎不自动回会话式聆听、设备不离记录会话——齿 = 引擎补丁 host（抑制标志 × speak 链）+ 真机（引导播完继续录音）。该 debt 条目设备侧已修（261014 包三守卫），本种子是记录语境防回归 case，非重开条目。
2. **忠实性 fixture 含静音段**（q4 / TECH-261014-engine-asr-hallucination-after-stop）：整理 fixture 的 STT 输入显式含静音段幻听产物（"Thank you." 类串），断言整理输出不含之——引擎 chat 入口已有 _kidpal_is_asr_hallucination 过滤（:1214）但幻听串仍可能经 stt 帧进 front 累积缓冲，种子落整理 fixture 层，不假设引擎过滤兜住。
3. **E4 fixture 含孩子中途提问句**（q7-E4）：提问句在 STT 累积输入内，滤除职责全压整理 LLM 指令——fixture 断言输出不含问答内容（机判 = 事实点比对「输出不得含输入外实体」+ 输入内问答案点名排除；忠实度抽检留人工）。
4. **NFR-PERF-3 五条边界**（时钟注入快进，host 齿）：① 接着说即重置 ② 超阈值触发引导 ③ 明示卡壳词立即触发 ④ 连续 2 轮无回应自动收尾 ⑤ 收声抑制/返回空轮照常计数至自动收尾。阈值常量不落验收值（待定）。
5. **S-F2.2 五条 AC**：AC1 双入口同一记录语境对话态 + AI 默认闭嘴 + 逐句回显；AC2 卡壳轻引导（护栏：只基于已讲内容/不提供情节选项/不代编）+ 不跳页 + 引导播报中 KEY 打断/BOOT 无操作 + 引导语/提问/回答不进素材；AC3 盖章照旧；AC4 三式收尾 + 忠实整理 + 整理中 KEY/BOOT 不响应 + 新篇入列表 + 取消丢弃不设恢复；AC5 自动收尾含被抑制轮。
6. **翻 ✅ 三条件**（S-F2.2 可行性 🟡→✅）：① 注入不尾随 auto-listen 绿（= 种子 1）；② 静音段 fixture 绿（= 种子 2）；③ q2 裁定落地 + 思考态接线真机可见（= start_organizing → state-organizing 帧真机轮，S4/S5）。
7. **provider 出口断言**（PROPOSAL T3 第 5 条）：引导/整理回合的制品级 system prompt 约束该回合不触发工具调用（v11 路由表同款 prompt 纪律）。
8. **包外留根（非目标，plan 如实写）**：真机停顿手感走观感直修长环、闸链后补账；卡壳阈值拍数不属本包；261001 q3「说完了是否并入收尾词表」未确认前按 S-F2.2-AC2 原文「我说完了/讲完啦」实现（PROPOSAL §6 口径）；整理失败的素材保全取舍 = 当轮不归档 + 固定安抚句（不发明新语义）。

## 未核项申报（architect 评审如实标注；全是包内定量/挂点验证，不动摇分档）

1. 引擎 ASR 路径 stt 下行帧确切发出点（upstream receiveAudioHandle，非补丁文件内）——「chat 入口抑制不影响 stt 下行回显」是推断，S2 动工前先验证再定抑制挂点。
2. kidpal_render_node.c 的 SetStatus→场景映射内部（思考页「零调用」断点位置）——S4 动工前先读本体。
3. story_page.c RECORDING 渲染气泡区现状（:1354 后未读）——帧 A 修订增量未定量。
4. server/acceptance/ 既有冻结验收对 120s 语义的钉法（timed_out_sessions/AUTO_STOP_SECONDS 退役必碰，改动量未定量）。
5. reminders.py deliver_alert 内部与「录音期提醒排队延后」现役承载点——S2 复用注入先例前先读全。
6. P3 KEY 入口设备侧确切路由（kidpal_story_session_ctx_set 单点自述在 mcp_server REC_SESSION 消费位；按键→StartListening 沿未逐行追）。

## decisionCoverage（FROZEN PROPOSAL 裁定逐项映射）

Package B 读法乙（引擎侧抑制）→ S2 engine-patch 第三守卫 + 风险 2 重绑；E3 检出器形态（关键词表 + 引导可返回空）→ S1 词表纯函数 + S2 引导回合空返回计轮；q2 = ⓒ（新增 start_organizing 下行信号）→ S2 下发 + S4 消费；R1-F4 provider 出口 (a)（OpenClaw OpenAI 兼容端点 + 出网沿 §1.2 路径②零新增）→ S2 LLM 回合 + 两枚 prompt 制品；q6 收声不停摆计轮 → S1 轮次纯决策 + NFR 断言⑤；R1-N4 丢稿口径（当轮不归档 + 固定安抚句）→ S2 失败路径。
