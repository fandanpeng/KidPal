---
changeId: 261019-p3-record-dialogue-guide
sources:
  humanVerbatim:
    - "我们探讨一个方式：在 P3\"我要讲故事\"的时候，我们其实是给他设计了一个单独的聆听状态，我想把它改成对话式的。比如说孩子说\"我要讲故事\"或\"帮我记下来\"，这个时候它的对话态是个引导态。比如看孩子故事讲到了哪里，AI 可以引导式地跟他说\"那下一步应该是什么\"。等引导完了、小孩说\"讲完了\"之后，再把整个过程中小孩说的内容整理并转写成一个故事，而不是像原来那样小孩一直讲、它只在那儿听，后面才记下来"
    - "跑 triage"
  refs:
    - 261019-p3-record-dialogue-guide.design.md（companion 设计稿，human 2026-10-19 brainstorming 逐段批准；四决策 + 被否方案留痕）
    - 261001-m4-k3-record-session-live-repair（未 closed，Package B 撞车对象，见 openQ q1）
    - 261016-p3-story-body-sd-cache（in-flight，实现面同文件 story.py / front_app.py，launch 顺序约束）
    - TECH-261014-session-end-channels-unwired / TECH-261014-post-speak-listening-lingering / TECH-261014-engine-asr-hallucination-after-stop（牵连 debt，见正文）
    - ARQ-261017-anchor-check-master-unvalidated（UI 锚机检既有背景噪声，dispatch 时提示防误读）
faceMap:
  requirements: 动——/requirement-change 档 2 结构性（含触 UI）：S-F2.2 全篇重写（requirements.md:446-459，废「一刀切纯记录 / AI 不逐句回复 / 提问也被记录」）+ §2.1 记录会话段（:57）与 60 秒对账注记（:47）+ §2.4 P3 行（:183）联动 + WF-F2-02 增补注记（:384-397，只加注记不动四态通则）+ §7 开放问题 7 已拍条目加撤注（:680，120 秒收口裁定被取代）+ §5 NFR 新增卡壳阈值待定项 + §2.3 键位矩阵措辞核对（:144-145）；S-F1.4 AC6 提醒排队例外（:302）与归档下游 S-F2.1/2.3/2.4 不动
  architecture: 动——/arch-change：决策 25（architecture.md:423）改写形态待裁（正文 append 先例 = 决策 24 vs ADR 新增先例 = 261001 facePlan 预案；另须顺手修「机制细节见 §2.10」悬空指针，:397 已声明旧编号迁移）+ §5.1.2 front 网关行（:186）与 story.py 行（:193）计时器族重写 + §5.2 场景④（:214）链路加引导注入与整理步 + §5.1.1 会话状态机行（:169）/ §2.2 容器图（:91）/ §9 to-be②（:389）的 120s 设备侧超时语义待裁 + 可能触 §5.5.5 OpenClaw prompt 版本化（引导/整理指令载体决策）
  contract: 动（措辞级 + 一个待裁点）——wire 零新增预判成立（引导语走 kidpal_speak 注入 + 既有 tts 帧，整理在网关内部，归档下游现成）；但 interface-contract.md:175（§1.4 start_recording「累积态 / manual 聆听模式」措辞）与 :271（§四 kidpal_session_ctx「front 即切该会话累积态」）须随批措辞对账（预判 ACCOMPANIES_SPEC_CHANGE，261001 对 :170/:266 同类先例）；**待四分类裁定点**：语音收尾与自动收尾由云端检出，现行契约无云→端「会话收尾/进整理态」下行信号（候选 ⓐ 设备侧镜像扫 stt 回显帧 / ⓑ 容忍停留到 story_text 到达 / ⓒ 新增下行信号 → 契约包三节），与 TECH-261014-session-end-channels-unwired 同族
  implementation: 动——主战场 server/gateway/story.py（卡壳计时器族、引导轮次计数、自动收尾）+ server/gateway/front/front_app.py（引导 LLM 回合 + kidpal_speak 注入、整理调用、收尾流程改造）；固件侧 = 录音态引导播报渲染（停留记录屏换气泡）+ 收尾→思考态接线（思考页本就是 Phase 1 未接线修复项，requirements.md:64 ⚠️）；五档预判 = ② 单包 Full（riskTier medium–high，跨 firmware+server），最终归本门收尾分档
  governance: 不动——.claude/agents/** 定义 / gate 登记 / hooks / scope-map / settings 零命中（verify-gates.json grep story/record/累积 零命中）；ARQ-261017-anchor-check-master-unvalidated 的 story 锚悬空是既有治理债非本批制造，需求档 2 派 uiux-author 时经该机检面须提示防误读
facePlan:
  - 顺序1 需求面（轻面，本会话内联）：/requirement-change 档 2——story-author 盘上 updating 起草 + 档 2 architect 必派（可行性三件）+ uiux-author 生成模式出 WF-F2-02 增补 + requirement-reviewer 四查 + 触 UI 第五查 + human 终拍看图；定稿载体 = §9 v1.65 版本行
  - 顺序2 架构+契约面（重面，headless）：/arch-change Track B——决策 25 改写形态裁定 + §5.1.2/§5.2④/§2.2/§9 联动 + 思考态下行触发裁定点 + 契约措辞级对账随批原子落（四分类正式裁定）；**261001 Package B 定形义务是否吸收进本批 = openQ q1，随立项 ack 由 human 裁**
  - 顺序3 实现面（本门收尾动作）：anchors req/arch/contract 回填 + 261016-p3-story-body-sd-cache 合并完成后，/triage-change 五档收尾（预判 ② 单包 Full）→ planner /build-feature；launch 前置硬约束 = change-intake §8 spine 源条款 + 261016 合并序
anchors:
  req: "requirements.md §9 v1.65（2026-10-19 档 2 结构性 · 触 UI 定稿、frozen）——S-F2.2 全篇重写（对话式引导记录：默认闭嘴 + 卡壳轻引导 + 素材边界废除一刀切 + 收尾忠实整理 + AC5 自动收尾含被抑制轮 + 按键语义钉）+ NFR-PERF-3 新增（阈值待定）+ §7 开放问题 7 取代注记 + WF-F2-02 框后注记四件 + 连带 §1/§2.1/§2.3/§2.4/F2 intro；设计母版 v6→v7（三帧 p3-recording 修订 / p3-recording-guide / state-organizing，气泡左上演层顶部位式）+ BRIEF v6 + glossary 两条勘误；PNG = docs/ui/V7/ 三帧；human 终拍两轮通过（第一轮打回气泡位式）；reviewer 三轮 PASS；流程留痕见 §9 v1.65 依据列"
  arch: "architecture.md §12 changeId 行 `261019-p3-record-dialogue-guide`（2026-10-19 landed）——§10 决策 25 取代注记（悬空指针同修）+ ADR-013 新增（记录语境对话态 + Package B 引擎侧抑制定形 + `start_organizing` + provider 出口钉沿 §1.2 路径②）+ §5.1.2 front/story.py 行与 §5.2 场景④ 链路重写 + §2.3 / §5.1.1 / §9 to-be② / §5.5.4 / §4 NFR 映射表（补 NFR-PERF-3 行）/ §2.4 原则 3 联动对账；制品 = delivery/review/261019-arch-review-p3-record-dialogue-guide/（FROZEN v1.2 + R1 BLOCK(F1-F4)→v1.1/v1.2 修毕 + R2 GREEN + landing-verdict PASS）"
  contract: "interface-contract.md 三触点 LANDED（CDQ-261019-p3-record-dialogue-guide）：① §1.4 新增 `start_organizing`（网关决定类、引擎 display 流拒转）+ 表后变更记录条；② §1.4 `start_recording` 行 + §四 `kidpal_session_ctx` 条措辞对账（记录语境对话态 + 引擎侧抑制由 front 内部注入等价信号承载；wire 形状零改）；③ §1.4 节标题括注对账。四分类 = ACCOMPANIES_SPEC_CHANGE（reviewer 独立裁定）；同批 ADR-013 随落"
  imp: "TaskPackage 261019-p3-record-dialogue-guide **已收口并合本地 main**（merge-driver 人批链：human-decision 3c71f3b0 → merge commit af4e6145，main = 4928f14d；verifier r13/r14 全绿 + supervisor ESCALATE 人批）。实现历程 = S1-S4 初版 + fixer 四轮（D1 auth / D2 UAF 三崩 / D3 v7 渲染 / D4 幻听守卫与整理接线 + 帧 B 置位链）+ 需求 v1.66 收尾词表；真机 R1-R5 五轮，R5 全绿（引导 spoken=yes、整理「游乐场的一天」、零崩溃）"
openQ:
  - id: q1-261001-PackageB-吸收
    question: 261001-m4-k3-record-session-live-repair 的 Package B（D3-甲「引擎侧再动抑制 LLM」，冻结待 /arch-change 定形，触面与本批完全重叠：arch §5.2④ / 决策 25 / 契约 :266「引擎不感知本信号」）是否吸收进本批 /arch-change 一次定形？
    ruling: **human 2026-10-19 立项 ack 裁：吸收进本批**——/arch-change 一次 Track B 把「记录语境对话态 + 引擎侧抑制 LLM」同三节（§5.2④ / 决策 25 / 契约 :266「引擎不感知本信号」）一起定形；261001 spine 的 Package B 随本批闭环（其 anchors/结项由主线程在本批 arch 面落地后回填）
    status: resolved
  - id: q2-思考态下行触发
    question: 语音收尾 / 自动收尾（云端检出）后设备如何进思考/整理态——ⓐ 设备侧镜像扫 stt 回显帧（收尾词表双源漂移风险）ⓑ 容忍停留到 story_text 到达（数秒盯 REC 屏）ⓒ 新增下行信号（契约包三节）？
    ruling: 归 /arch-change 内裁（不必须 ack 点）；选 ⓐ/ⓑ 则契约 NO_CONTRACT_CHANGE（措辞对账除外），选 ⓒ 则 ACCOMPANIES_SPEC_CHANGE 契约包随批原子落
    status: deferred
  - id: q3-决策25改写形态
    question: 决策 25 改写 = 正文 append 注记（决策 24 先例）还是 ADR 新增（261001 facePlan 预案「冻结历史档不改正文」）？两先例并存。
    ruling: 归 /arch-change 起草时显式裁定；并顺手修决策 25「机制细节见 §2.10」悬空指针（:397 已声明旧编号迁移，实际承载 = §5.1.2 + §5.2④）
    status: deferred
  - id: q4-牵连debt注记
    question: TECH-261014-session-end-channels-unwired「双阈值 60s/120s timer 零 producer」范围随本批 120s 语义改写而收窄/重述，条目正文 append 注记时点？
    ruling: 主线程 L0 裁——本批 arch 面 frozen 后主线程 append 注记（结项不归本批）；TECH-261014-post-speak-listening-lingering（注入后尾随 auto-listen 会破坏记录会话）与 TECH-261014-engine-asr-hallucination-after-stop（静音段幻听污染整理输入）转实现面验收种子（fixture 含静音段 + 注入不尾随 auto-listen case）
    status: resolved
  - id: q5-Track判段
    question: 升级判据两条是否命中（① 跨系统结构性 ② 方向未收敛）→ Track A 段还是 Track B？
    ruling: **human 2026-10-19 立项 ack 确认：不命中，走 Track B**——wire 零新增（服务端语义动但非「协议与语义同时动」）、方向已由 human 逐段批准设计稿收敛；吸收 Package B 后 arch 批范围变宽但方向仍收敛
    status: resolved
  - id: q6-E1收声死锁修法
    question: 「别说话听我讲」收声与 AC5 自动收尾死锁（收声 = LLM 持续抑制 → 引导轮次恒 0 → 2 轮条件永不满足，120s 兜底已废）如何回补？
    ruling: 主线程 L1 裁（2026-10-19，architect 可行性三件 E1 升回后）——取修法 (a)：被抑制的引导轮照常计入轮次，「AI 收声」不等于「计时停摆」，第 2 轮计时到仍无回应即自动收尾。依据 = 设计稿决策 6「孩子真离开则归档无害」既定意图，非新语义。落点 = S-F2.2 补充规则 + AC5 + NFR-PERF-3 断言边界
    status: resolved
  - id: q7-可行性下游转注
    question: architect 可行性三件的 E3/E4 归属？
    ruling: E3（明示卡住检出器形态：关键词表 vs LLM 判定；引导 LLM 可返回空）→ /arch-change §5.1.2/§5.2④ 起草时写明；E4（忠实性 fixture 须显式含孩子中途提问句用例——提问句在 STT 累积输入内，滤除职责全压整理 LLM 指令）→ acceptance-author。可行性结论 = S-F2.2 维持 🟡，翻 ✅ 三条件 = 注入不尾随 auto-listen 绿（Package B 定形后）/ 静音段 fixture 绿 / q2 裁定落地+思考态接线真机可见
    status: resolved
  - id: q8-出图pending
    question: 4.5 步 od_* 出图（V7 三帧：p3-recording 修订 / p3-recording-guide 新增 / state-organizing 新增）时 OD daemon unreachable，且 Open Design 应用未在跑、会话内拉起失败（cmd start 静默无进程）——如何处置？
    ruling: 原 L0 裁 = 挂 human-decision 待 human 启动 Open Design。**改裁（2026-10-19 human 指点）：出图不依赖 Open Design 应用**——仓内自有脚本 tools/asset-pipeline/shoot_design.mjs（playwright 无头 Edge 逐画布精确截图、400×300 阈值化 1bpp）直接吃 v7 设计 HTML。已执行：15 画布全帧出图，本批三帧（p3-recording 修订 / p3-recording-guide / state-organizing）捡进 docs/ui/V7/（对齐 261016 先例每批只落变更帧），主线程目检通过后呈 human 终拍。遗留修正 = opendesign-connect skill 应增补此脚本先例（另行登记 ARQ，不挡本批）
    status: resolved
  - id: q9-审稿R2按键语义钉
    question: reviewer 审稿否回 R2——引导播报中 / 整理中两个新增子时机按键语义未钉且与 §2.1 :51 打断停播句双源冲突；R1（v7 增量页破坏机检基线）/ R3（右槽键帽「结束」不承担丢弃警告）/ R4-R8 打回
    ruling: 主线程 L1 裁（2026-10-19）——引导播报中 KEY 短按 = 打断引导播报继续聆听（回引 §2.1 :51）、BOOT = 无操作（防误触毁稿，v1.47 精神延伸）；整理中 KEY/BOOT 均不响应（非交互过渡态、键帽条照常——盖章演出先例同款）；录音态（讲着/静默）KEY/BOOT 不变。R1 = 主线程合并完整 v7 母版（v6 基底 + 三帧，spec-anchor-check 复绿 64 od-id）；R3 = uiux-author 右槽改「丢弃」+ 帧 B「打断」+ 帧 C 留空 + BRIEF 破坏性提示通则句；R6 = 主线程随批勘误 glossary 两条（记录语境 / 一刀切纯记录）；R2/R4/R5/R7/R8 = story-author 落盘
    status: resolved
  - id: q10-终拍气泡位打回
    question: human 终拍第一轮反馈——帧 A/B 气泡位置错：应与其它态一致放猫咪头顶上方（左演出层顶部），非右栏中部
    ruling: 终拍第一轮单点打回已闭合——uiux-author 改帧 A/B 气泡至左演出层顶部猫头顶上方（left 24/top 28，三帧位式统一）+ 规格表加「气泡位式」行 + BRIEF 同步；期间 uiux-author 全文件 Write 二次覆盖完整母版（R1 回归），主线程重放合并修复（机检复绿 15 画布）+ 重出三帧 PNG + 与 uiux-author 约定母版单写者（其后续只交增量页/补丁说明）。**human 终拍第二轮（2026-10-19）通过**（含帧 C 空键帽 = 非交互语义确认），requirements 翻 frozen v1.65
    status: resolved
status: closed（2026-10-20 结项：四面全闭环——req v1.65/v1.66 frozen、arch ADR-013、contract CDQ-261019、impl 合 main af4e6145；真机 R1-R5 五轮，R5 全绿）
---

# spine · P3 记录会话改对话式引导态（261019-p3-record-dialogue-guide）

> 触面 ≥2 跨面变更的路由账本（[change-intake.md](../../.claude/references/change-intake.md) 程序；spine 免冻——各面冻结语义由各面制品承载，anchors 记录冻结事实）。设计输入 = [261019-p3-record-dialogue-guide.design.md](261019-p3-record-dialogue-guide.design.md)（human 已批准）。

## 建档前查重（change-intake.md §3）

- **既有 spine 排查**：`delivery/intake/` 在册 14 份逐份按意图比对——261016-p3-story-body-sd-cache（正文缓存，in-flight）、261010-p3-storylist-cold-start（列表冷启动，closed）、261001-m4-k3-record-session-live-repair（同区域但意图 = 缺陷修复 + 引擎抑制定形，awaiting-upstream）均不同意图，**不重复建档**；与 261001 的关系 = 撞车牵连（openQ q1），非重复。
- **debt 三域排查**：gov 25 / tech 27 / requirement 0 未结条目无意图相同或相冲者；四条牵连项已入 refs 与 openQ q4。主动 idea 直进 intake（§3a「主动 idea 跳过 debt」）。

## 立项 ack（change-intake.md §4 · 唯一必停人类触点）

四面对齐「为什么改 / 往哪改 / 改动多大」+ 升级判据路由（openQ q5）+ 261001 Package B 吸收（openQ q1）→ **已 ack（2026-10-19）**：确认立项走 Track B；Package B 吸收进本批 /arch-change。

## 触面事实（architect 只读探测，行号依据见 faceMap；查出来非申报）

需求面档 2 结构性（含触 UI）｜架构面 /arch-change（决策 25 + §5.1.2 + §5.2④ + 120s 语义）｜契约面措辞级对账 + 一个待四分类裁定点（思考态下行触发）｜实现面 ② 单包 Full 预判（story.py / front_app.py 主战场 + 固件引导播报与思考态接线）｜governance 不动。

## 撞车与牵连（探测实证）

1. **261001 Package B 直接撞车**（触面完全重叠）→ openQ q1 呈 human。
2. **261016-p3-story-body-sd-cache in-flight**（imp 待合并门，同文件 story.py / front_app.py）→ 本批实现面 launch 须等其合并，facePlan 顺序 3 写死。
3. 探测修正主线程预判两处（留痕）：「arch §2.10 机制节」不存在（迁移前旧编号悬空自引用，:397 已声明；实际承载 = §5.1.2 + §5.2④）；契约面非严格「零改动」（措辞级 ACCOMPANIES_SPEC_CHANGE + 思考态下行触发裁定点）。
