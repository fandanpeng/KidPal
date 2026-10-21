# PROPOSAL · 261021-arch-review-p3-record-cancel-lifecycle

> **状态：FROZEN v1.1 · 2026-10-21 · architect 起草（Track B；R1 BLOCK(F1,F2) → v1.1 修毕 → R2 GREEN）**——FROZEN 凭据 = human 立项 ack（spine q1–q4 resolved，human 触点① 2026-10-21 四项裁决）+ architect-reviewer R2 GREEN（§7，2026-10-21）；主线程凭两凭据标 FROZEN（作者≠判官）。FROZEN 后 §2 改法集（T1–T5）+ §5 验证集 = packet-id，落地 packet-bound，不改内容。来源 = 跨面 intake spine `delivery/intake/261021-p3-record-cancel-lifecycle.md`（facePlan 顺序 1，q1–q4 全 resolved；human 触点① 2026-10-21 四项裁决：② Full 确认 / OQ1 = Design B 新增 `kidpal_story_discard` / OQ2 = cancel-on-disconnect 维持 / OQ3 = 复验③重定义 (a)）。**v1.1 修订摘要（R1 否回应改，逐 finding 对应）**：R1-F1 = 增列 T5（§5.1.2 front 网关行 :187 端→网关私有消息闭集枚举随批补 `kidpal_story_discard` 一词，additive 零行为语义；v1.0 两处引 :186 系行号偏一，顺手校正；§6 同名留根条目撤销、§4 声明 3 收口为「判官已裁随批补」）；R1-F2 = 契约包 impact「未升级 front」句按评审实测机制改写（不识别 type → 透传引擎 → 无认领分支忽略 no-op），「OpenClaw 零影响」结论维持。R1 三件正式裁定落 §4（四分类主触点 CONTRACT_CHANGE_REQUIRED / 次触点 NO_CONTRACT_CHANGE / 判档主路径 retro 照常）。
> **承载 ARQ**：无独立 ARQ。来源三件：spine（上引）+ debt 条目 `docs/debt/tech/_archive/2026/TECH-261021-story-session-cancel-lifecycle.md`（已结项归档，consumed-by 本 changeId）+ reviewed plan `delivery/taskpackages/261021-p3-record-cancel-lifecycle/plan.md`（triage 已过；**本 PROPOSAL 的架构+契约面内容以它为唯一事实源，不发明新裁定**）。
> **review 轨**：Track B（默认路由；SSOT：`.claude/references/arch-change.md`）。**判档 = 主路径**（architect-reviewer R1 正式裁定：轻路径判据 ①③④ 三否，arch-change.md :23–:25；全件制品 + retro 照常在列）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 PROPOSAL §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（触点面窄：契约面 additive 单行 + 变更记录条，与既有端→网关私有消息同构先例复用；架构面一句取消链对账 + §12 定稿标记行；不改任何既有消息 / 场景 / 决策语义——销毁语义本身是需求面已冻结 S-F2.2 AC4 的端→云载体补齐，行为保持向严）。
> **review profile**：架构面 + 契约面（触契约分支，显式标注）；G1/G2/G3 治理面 = 零（不触 `.claude/**`）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是结构字段，GREEN 结论前必须在场；判官随 GREEN 结论核对、主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/contracts/interface-contract.md
docs/architecture.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：P3 记录会话的「用户主动丢弃」在端云两侧闭环——孩子在录音聆听中按 BOOT 丢弃，设备在断开连接前先发一条显式通知 `kidpal_story_discard`，网关按本连接会话权威取消该记录会话、素材销毁不入册；契约面补这一条端→网关私有扩展消息（与 `kidpal_story_delete` 先例同构），架构面把场景④ 的取消链显式化（显式丢弃链 + 断连 cancel 兜底）。

**现状差距**（三症状同根判读，业务语言；逐点根因链与修位见 reviewed plan §根因，本面不重复）：

1. 真机实证（debt source：TF 卡 KPLOG003/004 时间线逐点取证，human 三次报告 + 日志实锤）：**症状③** 录音中丢弃后，server 侧已派出的引导 LLM 回合照常完成，片刻后设备对着空气播报引导语（三次实证延迟 23–55 秒）；**症状⑤** 用户明确丢弃的内容约 100 秒后被 server 卡壳 sweep 当「未关会话」自动收尾归档，列表条目 6→7——**用户想销毁的素材被保留归档**（含隐私语义）；**症状④** server 驱动收尾后设备侧会话 ctx 不清，列表页长按回首页被守卫六连拦。
2. 同根 = 取消/收尾生命周期两侧未闭环：设备→server 方向丢弃零上行帧（只关设备侧、不通知 server，③⑤同源）；server→设备方向 server 驱动收尾不清设备 ctx（④）。契约面无「端→云销毁通知」载体——既有 wire 面无一承载该语义（复用通道 abort / listen stop / 断连隐式三条已由 architect 读码实证排除，spine q1 留痕）。
3. 需求面零缺口：「丢弃 = 销毁素材、不入册」语义锚已存在（requirements S-F2.2 AC4，:462 现核：「按 BOOT（短按/长按/双击同义）……取消、已录内容丢弃退回列表——不设重录恢复」）；本批补的是它的**端→云契约载体**与**架构链路表述**，不是新需求语义。

**非目标（本批不做，防 scope 漂移）**：实现面 F1–F4 全部不在本批 diff（teardown 悬挂修复 / discard 消费分支与发送链 / 在途引导守卫与腿归口 / start_organizing settle——归 dev 包 `261021-p3-record-cancel-lifecycle`，plan.md 承载，launch 前置 = 本批契约 anchor 回填）；需求面零变更（spine faceMap req 行：S-F2.2 AC4 既有语义实现对齐）；abort / listen-stop / 断连隐式复用排除论证已定案不复议（spine q1）；「讲错啦」语音取消通道不在本批（spine q4 L0 裁不另登，red-baseline story-p3 V31(b) RED 既有承载位）；ADR-013 / 决策 25/29 本体不动（spine faceMap arch 行：两侧生命周期闭环 = 已批准架构内实现补齐，不命中升级判据）。

---

## 2. 问题、根因与改法（T1–T5 + 契约包）

> 起草期实证留痕：本节全部 file:line 引用于 2026-10-21 起草本会话逐一 Read 现核——interface-contract.md :176（§1.4 `start_organizing` 行）/:178（表后变更记录条）/:274–:280（§四 `kidpal_session_ctx` / `kidpal_story_delete` / `kidpal_story_fetch` 行与节尾）、architecture.md :187（§5.1.2 front 网关行）/:215（§5.2 场景④ 全段）/:436–:445（§12 头部与先例行）、requirements.md :462（S-F2.2 AC4）；spine / plan.md / debt 归档条目全文现读；形态对标先例 = `delivery/review/261019-arch-review-p3-record-dialogue-guide/PROPOSAL.md` + `architect-landing-summary.md` + `docs/debt/contract/CDQ-261019-p3-record-dialogue-guide.md` / `CDQ-261017-p3-story-body-sd-cache.md`（只学形不抄内容）；轻路径判据现核 = `.claude/references/arch-change.md` :23–:25。基线机检（2026-10-21 实跑，输出见 §5 起草期实跑记录）：`kidpal_story_discard` 契约面 0 命中、`start_organizing` 契约面 2 命中、architecture.md「丢弃」4 命中（:131/:215/:323/:363，:215 处为丢稿口径语义、非用户丢弃链）、`spec-anchor-check.mjs` ok:true / errors:[] / designIdCount 64。**v1.1 补核（R1-F1/F2，2026-10-21 现核）**：architecture.md :187 闭集枚举行现核逐字（v1.0 两处引 :186 系行号偏一，顺手校正）；front_app.py `_consume_kidpal_control` 未识别 type 落 `return False`、caller 随即 `await upstream.send(text)` 透传——机制实质本会话实读吻合，行号引用以 R1 评审实读记录为准（§7：front_app.py :451–:527 / :994–:1002 + connection.py :549–:583）。

### T1 · 契约 §四新增 `kidpal_story_discard` 行（端→网关私有扩展消息）+ 变更记录条

**问题**：用户在记录会话中按 BOOT 丢弃（requirements S-F2.2 AC4 已冻结语义：取消、已录内容丢弃退回列表、不设重录恢复），设备侧只关会话、**零上行帧**——server 的 StoryAccumulator / 卡壳 sweep / 在途引导回合全然不知（症状③⑤同根）；现行契约 §四端→网关私有消息面（`kidpal_session_ctx` / `kidpal_story_delete` / `kidpal_story_fetch`）无一承载「主动丢弃、销毁素材」语义，复用既有通道（abort / listen stop / 断连隐式）三条已由 architect 实证排除（spine q1 留痕：abort 同会话态承载「打断继续聆听」正面冲突；listen stop = 归档触发语义相反；断连通道无法区分主动丢弃 vs 意外掉线）。

**证据**：
- 契约 §四现状（2026-10-21 现核逐字）：:274 `kidpal_session_ctx` 行、:275 `kidpal_story_delete` 行（「**网关消费并拦截、不下透引擎**……**单向通知、无应答消息**……front 幂等承接……省略整条消息 = 无删除，向后兼容。」）、:276 `kidpal_story_fetch` 行（「（端→网关，KidPal 私有扩展消息；2026-10-17 新增，`261016-p3-story-body-sd-cache`）」行内新增标记形态）；`grep -c "kidpal_story_discard"` = 0（基线实跑）。
- human 裁定：spine q1（human 2026-10-21 触点①）「**Design B 新增 `kidpal_story_discard`**……破坏性用户意图走显式可审计通道，与 kidpal_story_delete 先例同构；契约走 /arch-change 契约包三节随批原子落（261019 先例：arch 批先落、实现后跟）」。
- reviewed plan F2（唯一事实源照抄要点）：形状 `{"session_id":"...","type":"kidpal_story_discard"}`；网关整条消费拦截不下透、幂等（session 已 pop 时 cancel 返回 False 即 no-op）、审计 log 一行；**会话绑定以连接权威——front 按本连接 session_id 取消，不信载荷值**；设备在 DISCARD_RECORD case 于 CloseAudioChannel 之前同帧序发送（同一 TCP 有序，front 泵先消费 discard 再收 disconnect）。

**根因**：「丢弃」是破坏性用户意图，其端→云通知必须是显式、可审计、与断连可区分的通道；契约面在 delete（已归档篇目硬删）之外缺一枚针对**在途记录会话**的销毁通知——这正是 S-F2.2 AC4 端→云载体的空缺，属「现有契约无法表达已批准设计」。

**改法**：§四新增一行（插入位 = :276 `kidpal_story_fetch` 行之后、:277 `mcp` 工具调用行之前），措辞风格对齐 :275/:276 既有行（行内新增标记 + 「网关消费并拦截、不下透引擎」+ 幂等/审计 + 单向无应答 + 省略兼容句五件）；随后落一条独立变更记录段（照 §1.4 :178 表后变更记录条格式）。逐字见下方围栏。

```text
【插入 1 · §四新增行（interface-contract.md :276 行之后）】

- **`kidpal_story_discard` 消息**（端→网关，KidPal 私有扩展消息；2026-10-21 新增，`261021-p3-record-cancel-lifecycle`）：设备在记录会话中用户主动丢弃（BOOT 丢弃手势，requirements S-F2.2 AC4「取消、已录内容丢弃退回列表」）时发送 `{"session_id":"...","type":"kidpal_story_discard"}`——发送时机 = 设备关闭音频通道（断连）**之前**同连接先行发送（同一 TCP 有序，网关泵先消费 discard 再处理断连）。**网关消费并拦截、不下透引擎**：front 按**本连接的 session_id 权威**取消该记录会话（**不信载荷中的 session_id 值**——会话绑定以连接为准）：累积缓冲销毁、素材不入册（不触发归档、不触发 `story_list` 重推）、在途引导/整理 LLM 回合产出丢弃；会话已释放后重复/迟到的 discard = 幂等 no-op；审计日志一行。**单向通知、无应答消息**（与 `kidpal_story_delete` 先例同型同构）。省略整条消息 = 无显式丢弃，断连隐式取消（cancel-on-disconnect）兜底语义维持不变，向后兼容。

【插入 2 · 变更记录条（紧随插入 1 新行之后，独立段；照 §1.4 :178 变更记录条格式）】

（契约变更记录：2026-10-21 经 `/arch-change` 批准新增 §四 `kidpal_story_discard`（端→网关私有扩展消息：用户主动丢弃记录会话、素材销毁不入册；网关按本连接 session_id 权威取消、不信载荷值，幂等 + 审计，单向无应答；与 `kidpal_story_delete` 先例同构）——requirements S-F2.2 AC4 丢弃语义的端→云载体补齐 + arch §5.2 场景④ 取消链显式化；§1.4 `start_organizing` 行包络核对结论 = 既有行文已在包络内、零改（见同批 PROPOSAL T2）；随 `261021-p3-record-cancel-lifecycle` 同批原子落地，见 delivery/review/261021-arch-review-p3-record-cancel-lifecycle/PROPOSAL.md。）
```

目标文件：`docs/contracts/interface-contract.md`（改）

### T2 · §1.4 `start_organizing` 行包络核对（核对结论 = 零改）

**问题**：dev 包 F4 修法 = `start_organizing` 消费补 `StopListening` settle（语音收尾 / 自动收尾两路径 front 检出时设备仍在 Listening，settle 后 Idle 清位链接管、守卫自然放行——症状④）。问题收窄为一问：该行既有行文「设备离开录音聆听演出」是否已把「含结束聆听」包在语义包络内——在包络内则契约侧零改（F4 = 实现侧补齐既有规范），不在则至多一行措辞补（**不得扩行为语义**）。

**证据**：
- interface-contract.md:176 该行板子行为列（2026-10-21 现核逐字）：「设备离开录音聆听演出、切整理过渡演出（v7 `state-organizing` 帧、思考态复用）；整理中 KEY/BOOT 均不响应（非交互过渡态，requirements v1.65 §2.3 录音态子态按键口径）；收到 `story_text`（正常）或降级安抚帧（失败）即离态；三式收尾 front 统一下发、重复幂等。**网关决定类动作**……」
- KEY 收尾路径形状（plan F4 + debt 同根判读）：KEY 路径设备已先自行 Idle（聆听已结束），`start_organizing` 到达时只剩切页渲染——该行写定时的成立形状；语音 / 自动收尾两路径设备仍 Listening，是**实现未达到该行已规定的行为终点**（症状④ = 实现缺口，非契约缺口）。
- spine faceMap architecture 行：「两侧生命周期闭环 = 已批准架构内实现补齐，不命中升级判据」；plan「逃生门核验」节：「start_organizing『结束聆听』行为细化预判落既有行『设备离开录音聆听演出』语义包络内至多措辞对账」。

**根因（核对推理）**：「录音聆听演出」是复合态称谓（v1.65「录音聆听」配对 + 演出帧）——记录会话进行中的设备态，**聆听行为是该态的组成**；「离开录音聆听演出」的唯一自洽读法 = 离开该复合态（演出与聆听一并结束）——整理过渡态下设备不可能继续聆听录音，否则与同行「切整理过渡演出」「收到 `story_text` 即离态」自相矛盾，且与需求面已冻结的 S-F2.2 AC4/AC5（收尾后会话结束、整理、归档）冲突。settle 发出的 listen stop 帧属基础协议聆听消息面（§四 `kidpal_session_ctx` 行「设备聆听仍经既有 listen 消息承载」），非新 wire 形状。

**改法（核对结论）**：**契约侧该行零改**——「设备离开录音聆听演出（含结束聆听）」已在既有行文包络内；F4 settle = 实现侧对两路径补齐「真正离开录音聆听」的既有规范，不扩行为语义。本触点四分类 = **NO_CONTRACT_CHANGE**（architect-reviewer R1 正式裁定，零改维持——见 §7 正式裁定 1）。

目标文件：`docs/contracts/interface-contract.md`（核对对象，零改——本 T 无 diff）

### T3 · 架构 §5.2 场景④ 取消链显式化一句插入

**问题**：场景④（:215）是记录归档链路的端到端权威，现文覆盖触发 / 引导 / 收尾三式 / 整理 / 归档 / story_list 三时点 / relay 收紧 / **删除**链，**独缺用户丢弃（取消）链**——「丢弃」一词在 :215 仅以「丢稿口径」（整理/引导链路失败当轮素材不归档）语义出现，用户主动丢弃的端→云链路在架构面无表述（2026-10-21 现核：architecture.md「丢弃」4 命中 :131/:215/:323/:363，无一为用户丢弃取消链）。**不降为零改**——现状文字读不出该语义（缺整链，非措辞含糊）。

**证据**：
- architecture.md:215 场景④ 尾段（2026-10-21 现核逐字，插入锚点见下方围栏原文列）。
- spine faceMap architecture 行：「场景④链路描述（architecture.md:215）补取消链显式化一句对账……ADR-013 / 决策 25 本体不动」；q2（human 触点①裁）「维持 cancel-on-disconnect（HWV-A11 既有设计语义……）」。
- 形态先例：场景④既有「**删除** = 契约 §四 `kidpal_story_delete`（端→网关单向）→ ……」单句链式表述——本插入句同形对仗。

**根因**：261019 批重写场景④时取消链尚不存在（丢弃只关设备侧是当时实现事实）；本批契约面补齐 wire 载体后，端到端权威必须同步显式化取消链，否则下游实现包与读者无取消链的架构锚可引。

**改法**：场景④「**删除** =」句之前插入一句「**丢弃** =」取消链表述（单句、与「删除」句同形；插入后原文其余文字一字不动）。逐字见下方围栏。

```text
【插入 · §5.2 场景④（architecture.md :215）「**删除** =」句之前】

锚点原文（逐字）：……**引擎侧来源的 `story_*` display 块经 front relay 收紧降级**（story 数据单源 = front 机械推送）。**删除** = 契约 §四 `kidpal_story_delete`（端→网关单向）→ ……

插入后形态：……**引擎侧来源的 `story_*` display 块经 front relay 收紧降级**（story 数据单源 = front 机械推送）。**丢弃** = 取消链显式化（2026-10-21，`261021-p3-record-cancel-lifecycle`）：记录会话中按 BOOT 丢弃（requirements S-F2.2 AC4「取消、已录内容丢弃退回列表——不设重录恢复」）→ 设备于关闭音频通道前同连接先行发送契约 §四 `kidpal_story_discard`（端→网关单向；网关按本连接 session_id 权威取消、不信载荷值）→ front 取消该记录会话（累积缓冲销毁、素材不入册、不触发归档与 `story_list` 重推、在途引导/整理产出丢弃，幂等 + 审计日志）；断连隐式取消（cancel-on-disconnect）维持为兜底通道（human 2026-10-21 裁维持，spine q2）。**删除** = 契约 §四 `kidpal_story_delete`（端→网关单向）→ ……
```

目标文件：`docs/architecture.md`（改）

### T4 · architecture.md §12 变更记录 changeId 行 append（定稿标记）

**问题**：arch 面语义性改动的定稿标记 = `docs/architecture.md` 文末「变更记录」节的 changeId 行（arch-change.md「定稿载体」节：「架构内容变更的定稿标记 = §12 changeId 行」；消费保护 = `launchPreconditions`「支撑语义 → architecture.md 已冻结」判据 = 对应 changeId 行在场）；不显式列入改法集，packet-bound landing 不会落它，facePlan 顺序 2 实现包 launch 前置不可满足。

**证据**：architecture.md:438 §12 头部「本文件每次改动都在下表追加一行（新行在上，倒序）」（2026-10-21 现核）；先例 = :442（261019-p3-record-dialogue-guide 行，含「契约面同批原子落」括注形态）。

**根因**：packet-id = §2 改法集 + §5 验证集；§12 append 动作须显式成 T（261019 先例 T7 同款）。

**改法**：§12 表顶部（261019-p3-record-dialogue-guide 行之上）追加一行。changeId 取 **spine changeId `261021-p3-record-cancel-lifecycle`**（跨面 intake，下游按 spine 锚消费）；依据列不含「FROZEN vX.Y」占位 token。逐字见下方围栏。

```text
【插入 · §12 变更记录表首行（新行在上，倒序）】

| 261021-p3-record-cancel-lifecycle | 2026-10-21 | §5.2 场景④ 补取消链显式化一句（设备丢弃 → 契约 §四 `kidpal_story_discard` → 网关按连接权威 cancel；断连 cancel 为兜底） | delivery/review/261021-arch-review-p3-record-cancel-lifecycle/PROPOSAL.md（契约面同批原子落：§四新增 `kidpal_story_discard` + 变更记录条；§1.4 `start_organizing` 行包络核对零改） |
```

目标文件：`docs/architecture.md`（改）

### T5 · §5.1.2 front 网关行端→网关私有消息闭集枚举随批补一词（R1-F1 裁定随批补）

**问题**：§5.1.2 front 网关行（:187）「上行泵拦截 `kidpal_session_ctx`/`kidpal_story_delete` 整条消费不下透引擎」是全仓唯一的端→网关私有消息闭集枚举（R1 全仓 sweep 证实，见 §7）；本批 T1 落地后该闭集与新消息自相矛盾——同文件场景④（T3）有取消链而 §5.1.2 闭集未含，下游可引闭集对抗 discard 消费分支的架构锚。

**证据**：
- architecture.md:187 该行片段（2026-10-21 现核逐字）：「**故事面**（上行泵拦截 `kidpal_session_ctx`/`kidpal_story_delete` 整条消费不下透引擎；`source_page=story` 切记录语境对话态（v1.65 取代「累积态」）；……」
- R1 查四 F1（裁定随批补，见 §7）：「……语义成分只升不降，随批补 `kidpal_story_discard` 一词（additive、零行为语义）」。

**根因**：闭集枚举是私有消息面的架构锚，新增消息须同批对账；v1.0 起草期按派遣边界锁（§5.2 一处 + §12 行）留界交判官，R1 裁定随批补。

**改法**：枚举个词 additive 插入（零行为语义；行内其余内容不动），逐字替换对见下方围栏。

```text
【替换 · §5.1.2 front 网关行（architecture.md :187）片段】
原：上行泵拦截 `kidpal_session_ctx`/`kidpal_story_delete` 整条消费不下透引擎
新：上行泵拦截 `kidpal_session_ctx`/`kidpal_story_delete`/`kidpal_story_discard` 整条消费不下透引擎
```

目标文件：`docs/architecture.md`（改）

### 契约包 · 端云协议

**proposedContractDiff**（逐面最小 diff）：
- **shimApi / §四 端云协议剖面**：新增 `kidpal_story_discard` 行（端→网关私有扩展消息，`{"session_id":"...","type":"kidpal_story_discard"}`；网关整条消费拦截不下透、按本连接 session_id 权威取消、幂等 + 审计、单向无应答）+ 变更记录条一条。逐字见 §2 T1。
- **displayBlock / §1.4**：`start_organizing` 行**零改**（包络核对结论 = 既有行文「设备离开录音聆听演出」含结束聆听，见 §2 T2）。
- strokeFormat / scheduleMD：未触，正确省略。

**impact**：固件（mcp_server.cc DISCARD_RECORD case 补发送 + protocols/protocol.h/.cc `SendKidpalStoryDiscard` 逐字仿 `SendKidpalStoryDelete` 形态，归 dev 包 F2）、front（front_app.py `_consume_kidpal_control` 新增 discard 消费分支 + teardown / 守卫 / 腿归口，归 dev 包 F1–F3；story.py 预判零改~极小）、引擎 / OpenClaw 本体**零影响**（discard 与 `kidpal_session_ctx` / `kidpal_story_delete` 同型，在 front 上行泵整条拦截、不下透引擎；引擎不感知本消息，OpenClaw 无消费面）。**既有消费不受影响论证**：§四既有消息（session_ctx / delete / fetch / mcp / llm / system）形状与语义零改；`start_organizing` 及 §1.4 表三 action 语义不变；新消息为 additive 行——未升级固件不发送（省略整条消息 = 无显式丢弃，断连兜底语义维持）；未升级 front 不识别该 type → 现行代码原样透传引擎（front_app.py:526-527 return False + caller :1002 透传），引擎按 type 分发无认领分支、忽略即 no-op（过渡期瞬时态；两侧同批部署常态下不出现——additive 先例三批 session_ctx/delete/fetch 过渡期实际路径同此，R1-F2 评审实测）；可靠性依赖同一 TCP 有序（discard 先于断连到达），不引入请求-应答对（ADR-005 边界保持，与 delete 同型）；不破坏已跑通实测链路。详见 §3 sweep 表。

**validationPlan**：本批落地核验 = 逐字 diff 对账（T1 插入 1/2、T3 插入句、T4 §12 行 byte 对账，landing-verifier 逐文件核）+ 机械断言（§5 条 3/4/5：`kidpal_story_discard` 计数基线 0 → ≥3、`start_organizing` 计数恒 2、场景④「取消链显式化」恰 1 命中）+ `spec-anchor-check.mjs` + `pre-merge-escalation-gate.mjs` 机械凭据闸三件联立（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖契约路径）。**wire 语义端到端验证归 dev 包**（非本批门）：reviewed plan 验收种子 1–3（① 累积态会话 + discard 到达 → cancel 落、无归档、story_list total 不变、sweep 不再命中，重复 / 非累积态 discard 幂等 no-op；② 在途引导回合 + discard/断连中到达 → 不注入、不 mark、无会话壳复活；③ 设备断连 + 引擎静默 → teardown 限时收尾、cancel(reason="ws_disconnect") 落）+ 真机复验①（录音中丢弃：不播引导、不归档、列表条目数不增）。详见 §5。

---

## 3. 影响面 sweep 分类

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 契约 canonical | `docs/contracts/interface-contract.md` | 改 | T1 §四新增行 + 变更记录条；T2 `start_organizing` 行核对零改 |
| 架构 canonical | `docs/architecture.md` | 改 | T3 §5.2 场景④ 一句插入 + T4 §12 行 append + T5 §5.1.2 闭集枚举一词（R1-F1） |
| 契约消费·固件 | `firmware/main/mcp_server.cc` / `firmware/main/protocols/protocol.h` / `firmware/main/protocols/protocol.cc` | 未触（本批） | discard 发送链 + `start_organizing` settle，归 dev 包 F2/F4（plan.md changeManifest 在册） |
| 契约消费·front | `server/gateway/front/front_app.py` / `server/gateway/story.py` | 未触（本批） | discard 消费分支 / teardown 悬挂修复 / 在途引导守卫与腿归口，归 dev 包 F1–F3 |
| 引擎 / OpenClaw | `server/deploy/engine-patch/` / OpenClaw 本体 | 未触 | discard 在 front 上行泵拦截、不下透引擎；OpenClaw 零影响 |
| 需求面 | `docs/requirements.md` | 未触 | S-F2.2 AC4 既有语义（:462）= 本批上游权威，零新增需求语义（spine faceMap req 行） |
| 治理面 | `.claude/**` | 未触 | G1/G2/G3 零命中（spine faceMap governance 行，architect grep 核实） |
| 牵连 debt（新档） | CDQ 新条目（contract 域） | 主线程 landing 动作 | 附录全文；非 manifest 目标（261019 先例：跨时间账本、非被 diff 的契约路径） |
| 牵连 spine | intake spine anchors | 主线程 landing 后回填 | anchors.arch / anchors.contract 回填（现为空串）；anchors.req = N/A 不变 |

> sweep 说明：本面落地只改两个 canonical 文件（见 landing-manifest）；实现面（固件 / front）归 dev-loop 承载包，不属本面落地范围。census 基线输出见 §5 起草期实跑记录（2026-10-21 实跑）。

---

## 4. human 取舍点（OQ）

**无新增 human 取舍点。** 四项裁决已在 spine 拍定（q1 = Design B 新增 `kidpal_story_discard`；q2 = cancel-on-disconnect 维持；q3 = 复验③重定义 (a) 不动需求面；q4 = 「讲错啦」不另登），本批无新分叉。**R1 正式裁定记录（architect-reviewer 2026-10-21，全文见 §7）**：

1. **判档 = 主路径**（R1 已裁：轻路径判据 ①③④ 三否，arch-change.md :23–:25；全件制品 + retro 照常在列——spine facePlan 顺序 1「瘦制品五件过闸（retro 豁免）」预判以本裁定覆盖，主线程 anchors 回填时记判档结论）。
2. **契约四分类（R1 已裁）**：主触点（§四新增行 + 变更记录条）= **CONTRACT_CHANGE_REQUIRED**（现有契约无法表达已批准设计 S-F2.2 AC4 的端→云销毁通知载体；R1 反证检查五项全过）；次触点（§1.4 `start_organizing`）= **NO_CONTRACT_CHANGE**（T2 包络核对成立，维持零改）。
3. **§5.1.2 闭集枚举（R1-F1 裁定随批补）**：判官已裁随批补 `kidpal_story_discard` 一词（additive、零行为语义），v1.1 已增列 T5 落地该裁定（目标文件同 T3/T4，manifest 无需扩）。

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **PROPOSAL 结构门（pre-commit 干跑；起草期已实跑 2026-10-21）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261021-arch-review-p3-record-cancel-lifecycle/PROPOSAL.md` → 预期 `ok:true`、`findings:[]`，anchorUnion 成员 = {docs/contracts/interface-contract.md, docs/architecture.md} ≡ landing-manifest、无夹带路径 token。结果留痕：见本节末尾「起草期实跑记录」。
2. **锚完整性（pre-commit 基线已跑 + post-commit 复跑）**：`node .claude/scripts/spec-anchor-check.mjs` → 基线（2026-10-21 实跑）：`ok:true`、`errors:[]`、designIdCount 64；post-commit 复跑预期 `errors` 仍零、designIdCount 不跌（本批不改小节编号、不触 UI 锚）。
3. **契约新增行硬断言（pre-commit 基线已跑 + post-commit 硬断言）**：基线 `grep -c "kidpal_story_discard" docs/contracts/interface-contract.md` = **0**（2026-10-21 实跑，exit 1 无命中）。post-commit 硬断言：同命令 ≥ **3** 命中（§四新行 2 处〔行首消息名 + JSON 形状内 type 值〕+ 变更记录条 1 处），且 `grep -n "kidpal_story_discard"` 命中全部落在 §四区间内（§四头部之后、「待 OpenClaw 确认点」节之前——行号随插入漂移，按区间人读核）。
4. **`start_organizing` 零改硬断言（pre-commit 基线已跑 + post-commit 硬断言）**：基线 `grep -c "start_organizing" docs/contracts/interface-contract.md` = **2**（2026-10-21 实跑：§1.4 表行 + 表后变更记录条）。post-commit 硬断言：同命令仍 = **2**（T2 零改）；且 `git diff docs/contracts/interface-contract.md` 中 §1.4 表（:171–:176）与 :178 变更记录条无改动（人读 diff 核）。
5. **架构取消链句硬断言（pre-commit 基线已跑 + post-commit 硬断言）**：基线 `grep -c "取消链显式化" docs/architecture.md` = **0**（2026-10-21 实跑）。post-commit 硬断言：同命令恰 **1** 命中且落在 §5.2 场景④段内（人读核区间）；`git diff docs/architecture.md` 仅含 §5.2 场景④ 一句插入 + §12 一行 append + §5.1.2 闭集枚举一词三处（人读 diff，边界守持——场景④ 其余文字、§5.1.2 行内其余内容、ADR/决策档零改）。
6. **契约机械凭据闸（post-commit，合并前）**：`node .claude/scripts/pre-merge-escalation-gate.mjs` 对 `docs/contracts/interface-contract.md` diff 三件联立（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖契约路径）→ 缺一 BLOCK。
7. **逐字 diff 对账（人读，post-commit）**：landing-verifier 逐文件核对实际落地 == 本 PROPOSAL §2 各 T 插入/替换段——T1 插入 1（一行）+ 插入 2（一段变更记录条）、T3 一句插入、T4 一行 append、T5 一词替换；另附录 CDQ 新档与 spine anchors 回填（主线程账本动作，非 diff 目标）。
8. **边界守持核（人读，post-commit）**：§四既有消息行（`kidpal_session_ctx` / `kidpal_story_delete` / `kidpal_story_fetch` / mcp / llm / system）零改；场景④ 三式收尾 / story_list 三时点 / relay 收紧 / 删除链 / 误判兜底原样；§1.4 全表与 :178 原样；`git diff` 仅限 landing-manifest 两文件（requirements / `.claude/**` / 实现面零改）。
9. **wire 语义端到端（实现面证据，非本批落地门）**：dev 包 `261021-p3-record-cancel-lifecycle` 验收种子 1–3 + 真机复验①（reviewed plan 验收种子节；契约包 validationPlan 已引）——本批落地不等待、不替代该证据，仅作下游消费锚。

**起草期实跑记录**（2026-10-21）：第 2 条 spec-anchor-check 基线 = `ok:true` / `errors:[]` / designIdCount 64 / storyCount 13 / anchorLines 33；第 3 条 `kidpal_story_discard` 契约面 0 命中（exit 1）；第 4 条 `start_organizing` 契约面 2 命中；第 5 条 `取消链显式化` architecture.md 0 命中、「丢弃」architecture.md 4 命中（:131/:215/:323/:363，人读核 :215 处为丢稿口径语义）；第 1 条 preflight 干跑 = `ok:true` / `findings:[]` / `advisories:[]` / anchorUnion = {docs/architecture.md, docs/contracts/interface-contract.md} ≡ landing-manifest（实跑输出摘要，完整 JSON 见会话留痕）。

---

## 6. 留根清单

- **「讲错啦」语音取消通道**：双侧未接线，非本批目标；red-baseline story-p3 V31(b) RED 既有承载位，spine q4 L0 裁不另登 debt（S-F2.2 AC4 语音取消语义属需求面后续批）。
- **实现面 F1–F4 全量**：归 dev 包 `261021-p3-record-cancel-lifecycle`（② 单包 Full / riskTier high；launch 前置 = 本批契约面 anchor 回填，change-intake §8；烧板轮不可省——mcp_server.cc / protocol.cc 属设备编译单元）。
- **teardown 悬挂修复（F1）与 wire 无关的必修项**：arch 面不重复记，plan.md §根因 1 承载（gather 悬挂 → finally 不执行 → cancel 不落；spine q2 已裁 cancel-on-disconnect 语义维持，F1 修的是机制不是语义）。
- **CDQ 登记**（主线程账本动作，landing 时以附录全文落 contract 域新档，非 §2 改法 manifest 目标——261019 先例同款裁定：跨时间账本、非被 diff 的契约路径；fourClass = CONTRACT_CHANGE_REQUIRED，R1 已裁与附录一致）。
- **spine anchors 回填**（主线程 landing 后动作）：anchors.arch = 「architecture.md §12 changeId 行 `261021-p3-record-cancel-lifecycle`（landed）——§5.2 场景④ 取消链显式化一句」；anchors.contract = 「interface-contract.md 两触点 LANDED（CDQ-261021-p3-record-cancel-lifecycle）：§四新增 `kidpal_story_discard` + 变更记录条；§1.4 `start_organizing` 行包络核对零改」。

---

## 附录 · CDQ-261021-p3-record-cancel-lifecycle 落盘全文（主线程 landing 时逐字用；本批起草期不落盘）

```text
---
id: CDQ-261021-p3-record-cancel-lifecycle
title: 新增 kidpal_story_discard（端→网关私有扩展消息：用户主动丢弃记录会话、素材销毁不入册）
domain: contract
status: deferred
severity: SHOULD
owner: dev-loop
trigger: 实现面承载包（② 单包 Full；launch 前置 = 契约面 anchor 回填）
opened: 2026-10-21
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/review/261021-arch-review-p3-record-cancel-lifecycle/]
fourClass: CONTRACT_CHANGE_REQUIRED
loopEvidence: 261021-arch-review-p3-record-cancel-lifecycle PROPOSAL（R2 GREEN 后由主线程回填凭据全文 + landing-verdict PASS）
---

# CDQ-261021-p3-record-cancel-lifecycle · 新增 kidpal_story_discard（端→网关私有扩展消息）

契约面两触点：① §四新增 `kidpal_story_discard` 行（端→网关，`{"session_id":"...","type":"kidpal_story_discard"}`；用户主动丢弃记录会话、素材销毁不入册——requirements S-F2.2 AC4 端→云载体补齐；网关按本连接 session_id 权威取消、不信载荷值，幂等 + 审计，单向无应答；与 `kidpal_story_delete` 先例同构）+ 变更记录条一条；② §1.4 `start_organizing` 行包络核对结论 = 既有行文「设备离开录音聆听演出」已在包络内（含结束聆听）、零改（该触点四分类 = NO_CONTRACT_CHANGE）。

**四分类 = CONTRACT_CHANGE_REQUIRED**（主触点：现有契约无法表达已批准设计 S-F2.2 AC4 的端→云销毁通知语义）——architect-reviewer R1 正式裁定（2026-10-21，反证检查五项全过）；次触点 `start_organizing` = NO_CONTRACT_CHANGE 零改维持。

**剩余条件**：

实现面承载包 261021-p3-record-cancel-lifecycle（② 单包 Full：front discard 消费分支 / teardown 悬挂修复 / 在途引导守卫与腿归口；固件 DISCARD_RECORD 发送链 + `start_organizing` settle；launch 前置 = 本批契约面 anchor 回填）。
```

---

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · architect-reviewer（2026-10-21）— verdict: BLOCK(F1,F2)

主线程代录（评审人工具面只读，评审记录全文由评审人返回、逐字转录）。

**查一 · 意图保真 — PASS**：逐点对账 spine q1–q4 四裁决与 triage reviewed plan——T1 承载 Design B（形状 / 网关整条消费拦截不下透 / 连接权威不信载荷值 / CloseAudioChannel 之前同帧序发送均逐点出自 reviewed plan F2）；q2 cancel-on-disconnect 在 T1 插入句 + T3 兜底条款双处忠实；q3/q4 在 §0 非目标 + §6 留根双载；无发明 triage 未裁新语义（T2 是核对结论非新裁且自锁只升不降）。

**查二 · 纪律合规 — F2 打回**：契约包三节齐备达标（diff 逐面最小 + impact 四向消费方 + validationPlan 可执行带基线数）；承重锚引文抽核全对（:274–:277 插入位 / §1.4 :176 / 架构 :215 锚句 / §12 :438 纪律行 / requirements :462）；验证集与 preflight 留痕评审人全部复跑吻合；落地清单封闭。**F2（打回项）**：impact「未升级 front 收到未知 type 按既有拦截面处理」与代码事实不符——实测 front_app.py:526-527 `_consume_kidpal_control` 对未知 type `return False`，caller :1002 随即 `await upstream.send(text)` 透传引擎；引擎 connection.py:549 `_route_message` 按 type 分发、`kidpal_story_discard` 无认领分支（:431/:467/:499 均不匹配）落入 `handleTextMessage` 无分支忽略。结论「OpenClaw 零影响」维持成立，但机制表述须改写为「未升级 front 不识别该 type → 现行代码原样透传引擎，引擎按 type 分发无认领分支、忽略即 no-op（过渡期瞬时态；两侧同批部署常态下不出现）」——additive 先例三批（session_ctx/delete/fetch）过渡期实际路径同此。事实断言精度属契约包 evidence 义务，不可带病 GREEN。

**查三 · 撞车复核 — PASS**：在途 spine 全量核对零交叠（closed 批 261016/261017/261018/261019 与本批改区是消费非冲突；在途 261012/261014 零交叠；§12 表「新行在上」双批 append 天然并容）；语义撞车核过（:273 abort 行、:276 fetch 史实注记、:215 既有「丢稿口径」与 T3 插入句同段共存由 changeId 括注消歧）；`kidpal_story_discard` 全仓契约面 0 命中。

**查四 · 牵连面完整 — F1 打回**：**F1（打回项，裁定随批补）**：架构 §5.1.2 front 网关行（实测 **:187**，PROPOSAL 两处引 :186 系行号偏一，v1.1 顺手校正）「上行泵拦截 `kidpal_session_ctx`/`kidpal_story_delete` 整条消费不下透引擎」是本仓架构面唯一的端→网关私有消息闭集枚举（全仓 sweep 证实）——本批落地后该闭集与新消息自相矛盾；语义成分只升不降，随批补 `kidpal_story_discard` 一词（additive、零行为语义），v1.1 增列 T5（目标文件同 T3/T4，manifest 无需扩，packet-id 行同步）；该行补后 §6「§5.1.2 front 行枚举」留根条目撤销。独立裁三点：(a) 判档主路径成立（retro 豁免不适用）；(b) T4 §12 changeId 行增列正当（定稿载体 + launch 前置判据，261019 先例 T7 同款）；(c) §5.1.2 一词须随批补（即 F1；漏补后果 = 同文件场景④与 §5.1.2 自相矛盾，下游可引闭集未含对抗 F2 消费分支架构锚）。附带观察（非打回）：intake INDEX 对本 spine 投影空值 + ⚠值域外（faceMap 平文字串疑似未被 changes-index.mjs 解析），检测不拦截，主线程 anchors 回填时顺带核对。

**正式裁定三件**：
1. 契约四分类：主触点（§四新增行 + 变更记录条）= **CONTRACT_CHANGE_REQUIRED**（现有契约无法表达已批准设计 S-F2.2 AC4 的端→云销毁通知载体；反证检查五项全过——adapter 不可解 / 无扩展字段可放 / 无命名撞 / 非二期能力前塞 / 无形状可复用）；次触点（§1.4 start_organizing）= **NO_CONTRACT_CHANGE**（T2 包络核对成立，settle 的 listen stop 属基础协议聆听消息面非新 wire 形状，维持零改）。
2. 轻/主判档 = **主路径**（arch-change.md :23–:25 判据 ①③④ 三否；后果 = 全件制品 + retro 照常，瘦制品五件过闸与 retro 豁免不适用；spine facePlan 顺序 1 预判以此裁定覆盖，主线程 anchors 回填时记判档结论）。
3. verdict = **BLOCK(F1,F2)**——两件均原地返修级（不升级 human、不影响目标效果），architect v1.1 修稿后 R2 聚焦复核。

**评审人实跑核验清单（2026-10-21）**：契约 grep kidpal_story_discard=0 / start_organizing=2；架构 grep 取消链显式化=0、丢弃 4 命中（:131/:215/:323/:363，:215 实读为丢稿口径）；spec-anchor-check ok:true（designIdCount 64 / storyCount 13 / anchorLines 33）；landing-fidelity-check --preflight ok:true / findings:[] / anchorUnion ≡ manifest；承重锚逐字比对 :274–:277 / :176 / :178 / :215 / :438 / :442 / requirements :462 / :303 全一致（§5.1.2 实测 :187）；front_app.py :451–:527 / :994–:1002 与 connection.py :549–:583 实读（F2 依据）；全仓闭集枚举 sweep 唯 §5.1.2 :187 一处（F1 依据）；intake INDEX 13 条 + 261012/261014 spine 实读零交叠；CDQ-261019 frontmatter 字段集比对一致。

### R2 · architect-reviewer（2026-10-21）— verdict: GREEN

主线程代录（评审人返回文本逐字转录）。R2 聚焦复核（R1 两件打回项 + 非阻塞项，不重开全四查）全部通过。

**R1-F1 修订到位 — 过**：T5 已增列（§2 :118–:136），替换对围栏「原」侧与 docs/architecture.md :187 实文逐字比对一致（评审人实跑 `sed -n '187p'` + grep 提取确认）；「新」侧仅 additive 插入 `/kidpal_story_discard` 一词，行内其余不动，零行为语义。联动五处同步齐全：packet-id 行改法集 T1–T5（:6）；§3 sweep 架构 canonical 行增 T5（:156）；§5 条 5 post-commit diff 描述改「三处」（:185）；§5 条 7 逐字对账增「T5 一词替换」（:187）；manifest 未扩（仍两文件，T5 目标已被覆盖，:12–:15 实核）。§6「§5.1.2 front 行枚举」留根条目已撤销（现存五条无该条）；§4 声明 3 收口为「判官已裁随批补，v1.1 已增列 T5 落地该裁定」（:175）。

**R1-F2 修订到位 — 过**：impact「未升级 front」句（:145）已按 R1 实测机制改写——「不识别该 type → 现行代码原样透传引擎（front_app.py:526-527 return False + caller :1002 透传）→ 引擎按 type 分发无认领分支、忽略即 no-op（过渡期瞬时态；两侧同批部署常态下不出现——additive 先例三批 session_ctx/delete/fetch 过渡期实际路径同此）」，与 R1 评审实读逐点吻合；「OpenClaw 零影响」结论维持且论证链自洽。

**非阻塞项收口 — 过**：:186→:187 校正到位（全篇余留 `:186` 三处均为修订摘要 / R1 评审记录的历史引述，属如实留痕、非活引用）；「判档建议 / 待裁 / 归 architect-reviewer / 预填」类措辞全篇（§7 除外）grep 零命中——§2 T2、§4 三条裁定记录、头部判档行均已改「R1 正式裁定」定案语态；CDQ 附录 fourClass 与 R1 正式裁定一致、四分类段改 R1 正式裁定措辞、loopEvidence 改「R2 GREEN 后由主线程回填凭据全文 + landing-verdict PASS」——回填时点语义正确。

**新矛盾扫描（R2 增量）— 零命中**：T5「新」句与 :187 行内后续内容拼接自然；§5 条 3 契约计数断言不受 T5 影响；§5 条 5 与条 8 边界守持描述互相一致；状态行 v1.1 + 修订摘要逐 finding 对应、如实；主线程代录的 R1 记录与评审人返回文本核对忠实、verdict token `verdict: BLOCK(F1,F2)` 形态未被改动。

**R2 实跑核验**：landing-fidelity-check --preflight 复跑 = ok:true / findings:[] / advisories:[] / anchorUnion ≡ manifest（docs/contracts/interface-contract.md + docs/architecture.md）；architecture.md :187 实文与 T5「原」侧逐字比对一致；`:186` 残留与措辞残留 grep 结果如上；§7 代录 R1 记录与评审人原文比对忠实。

**正式裁定（维持 R1，无新增）**：四分类主触点 = CONTRACT_CHANGE_REQUIRED、次触点 = NO_CONTRACT_CHANGE；判档 = 主路径（全件制品 + retro 照常，瘦制品五件过闸与 retro 豁免不适用）。

**主线程确认句**：本批「立项 ack + architect-reviewer GREEN」两凭据中 GREEN 一项已足——human 立项 ack 经 spine 触点①（2026-10-21 四项裁决 q1–q4 resolved）在场；主线程可凭「立项 ack（spine q1–q4 resolved，human 触点① 2026-10-21）+ 本 R2 GREEN」标 FROZEN，随后按 packet-id（§2 改法集 T1–T5 + §5 验证集）packet-bound 落地，landing-manifest 两文件（docs/contracts/interface-contract.md / docs/architecture.md）；CDQ 附录与 spine anchors 回填按 §6 留根清单随 landing 动作执行；retro 照常在列（主路径不豁免）。
