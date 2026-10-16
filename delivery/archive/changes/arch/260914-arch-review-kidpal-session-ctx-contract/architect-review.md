# architect-review · 260914-arch-review-kidpal-session-ctx-contract · R1

> 判官：architect（Track B 评审循环，只读 canonical、只写评审记录）。
> 评审日期：2026-09-14。评审对象：`PROPOSAL.md` DRAFT v1.0。
> 结论：**VERDICT: BLOCKED（9 项打回：5 MAJOR + 4 MINOR）**——方向与选型成立、无返工级缺陷，全部打回项为文本级修订；R1 修订回炉后预期可直接 GREEN。

---

## 一、硬项 1a 抽验结果（全部属实）

以下引用逐条亲核（Read / grep / 实跑），无虚引：

| 引用 | 核验结果 |
|---|---|
| contract §四 :223-234（abort :230、mcp :231、hello :227，无 kidpal_session_ctx） | ✅ 属实；插入位（abort 之后、mcp 之前）真实存在 |
| `docs/architecture.md:213` 场景④「信号载体随契约定」 | ✅ 属实（但见 MINOR-1：引文缺粗体标记） |
| `docs/architecture.md:340` 契约门在办三件 | ✅ 属实（但见 MAJOR-4：追加 ④ 与行首「三件」矛盾） |
| `docs/architecture.md:379` to-be ⑥ 原文 | ✅ 逐字一致 |
| `front_app.py:82-92` `_story_route_display_payload` | ✅ 属实 |
| `front_app.py:343` verbatim 透传注释 / `:383` 上行泵 json.loads | ✅ 属实（上行泵 :375-376 先转发后解析、未知类型透传语义成立） |
| `front_app.py:563-620` `ws_gateway`：Bearer 鉴权在 `accept()` 前（:570-576）、不解析 hello | ✅ 属实——**安全面结论成立：本消息在 WS accept 鉴权后、未鉴权不可达** |
| `story.py:305-309` `start` 幂等注释 | ✅ 逐字一致 |
| `websocket_protocol.cc:239-262` `GetHelloMessage()` 无参数 | ✅ 属实 |
| `kidpal_layer.h:24-52`（`kidpal_layer_top()` :49） | ✅ 属实 |
| `application.cc:575-610` `HandleToggleChatEvent` 不查层栈；`:739` `SendStartListening` | ✅ 属实 |
| requirements :428 S-F2.2 补充规则引用句 | ✅ 逐字一致 |
| §5.1 机检声称归零 | ✅ 实跑复核 `node .claude/scripts/spec-anchor-check.mjs` → `ok:true, errors:[], exit=0` |

## 二、技术方案评审（架构视角）

**载体选型：成立。** 独立 `kidpal_session_ctx` 消息优于 hello 扩展字段：hello 扩展需改 `GetHelloMessage` 签名注入页态（协议层读 UI 层，分层违例），且设备 hello 会经上行泵透传引擎、未知字段引擎容错未验证；独立消息由 front 显式拦截，引擎零触。上行泵单循环顺序处理保证消息先于音频帧消费，无竞态。拦截语义显式声明（未知类型现状是透传，新消息必须显式拦截才不下透）——T1 文本已写明，正确。

**向后兼容：成立。** 省略消息 = 现状普通会话；实现面零改动即旧行为不变。

**E1/§1.4 零影响：成立。** 本消息是端→网关上行文本消息，非 display 块、非 control 动作，E1 view 白名单与 §1.4 动作表确实无涉。

**时序与闭集语义：两处实质缺陷**，见 MAJOR-1 / MAJOR-2——钉死文本须修订后才能 FROZEN。

## 三、打回项

### MAJOR

**MAJOR-1 · T1 钉死文本语义歧义：累积态切换未限定 source_page 取值。**
T1 句「**网关消费并拦截、不下透引擎**：front 收到即切该会话累积态」是无条件的，但只有 `source_page=="story"` 应触发累积态；`home`/`schedule` 到达时切累积态 = 错误行为规范。且 home/schedule 的现版本行为（无动作、预留）完全未定义。
修改指令：T1 钉死句改为条件语义，例如「front 收到 `source_page=="story"` 即切该会话累积态（与 §1.4 `control/start_recording` 收敛同一幂等入口，重复触发不重复开会话）；其余取值现版本无行为（闭集预留，供未来切页语境扩展）；无论取值，整条消息一律由网关拦截、不下透引擎」。

**MAJOR-2 · T1 时序表述与 §四「wss 常驻连接」矛盾。**
T1 写「设备在收到 server hello 后、首帧音频前发送」——但契约 §四 传输层是 wss 常驻连接、`application.cc:597` 显示通道已开时直接复用不重连：同一连接上的**后续会话**没有新 server hello，时序规范覆盖不到。每次从 P3 发起记录会话（含通道复用场景）都必须能声明信号。
修改指令：时序改钉为「设备在**每次发起会话时、首帧音频前**发送（首连时序上在 server hello 之后；通道复用的后续会话同样发送）」，并注明重复发送安全（收敛入口幂等）。

**MAJOR-3 · 契约门制品缺口：无 CDQ 登记、无 gate evidence 计划、census 计数自相矛盾。**
本批 = CONTRACT_CHANGE_REQUIRED 直改契约，但：(a) `docs/CONTRACT-DECISION-QUEUE.md` 不在 landing-manifest、无对应 T 项——260913 批（CDQ-260913 行）与 260902 批（CDQ-260902 行）先例均为契约面变更登记 CDQ 行，`.claude/references/contract-change-evidence.md`「Required Artifacts」明列 Queue update = Yes for nontrivial gates；(b) 无 CONTRACT-CHANGE-PACKET / architecture-gate-evidence 台账计划（260902 先例产出两件，evidence reference 定为 Required）；(c) §5.3「恰 4 文件（contract / architecture / requirements + PROPOSAL 自身不计）」自相矛盾——括注说 PROPOSAL 不计却只列 3 个文件，第 4 个是什么没有说。
修改指令：① 增 T4（或并入 T1）：CDQ 行登记（四分类 CONTRACT_CHANGE_REQUIRED、human 门 = 2026-09-14 预授权 + OQ2 裁定收口、状态 PRECHECKED → 落地后 LANDED），landing-manifest 增 `docs/CONTRACT-DECISION-QUEUE.md`；② 本 change 目录产出 architecture-gate-evidence.md 台账（architect-reviewer 分类行 + human decision 原文/日期 + applied diff inventory + validation，骨架按 evidence reference 十节），可由主线程在 GREEN + OQ2 裁定后补、但 PROPOSAL 须显式列此义务；③ §5.3 census 改口：落地后 `grep -rn "kidpal_session_ctx" docs/` 恰 4 文件 = contract / architecture / requirements / CONTRACT-DECISION-QUEUE，PROPOSAL 与本批其余制品不计。

**MAJOR-4 · T2.2 落地指令会产生自相矛盾行。**
`architecture.md:340` 现行行首为「另登记**三件**随下次契约门裁（2026-09-14 对照 review）：①…②…③…」，末尾追加「④…」后「三件」与四项矛盾。
修改指令：钉死指令改为整行替换（或同步把「三件」改「四件」），给出替换后完整行。

**MAJOR-5 · 落地前置依赖未声明：工作区与另一批 v1.52 改动同文件纠缠、T3.3 已提前应用。**
实测工作区 `docs/requirements.md` 含另一批 v1.52（朗读砍除，49+/25-）未提交改动，且 T3.3（canvas-p4 去反引号，HEAD :494/:506/:519 带反引号 → 工作区已去）**已提前应用于工作区**（PROPOSAL §5.1「落地后基线已实测归零」已如实承认，但这是 FROZEN 前动 canonical）。后果：landing-verifier 逐文件人读 diff == frozen 清单在 v1.52 未先行独立 commit 前必然失真；§3 sweep 的 requirements 行未列锚行去反引号、§5.2 对 requirements 无 diff 范围断言。
修改指令：① PROPOSAL 显式登记落地前置 = v1.52 批先行独立 commit（本批 v1.53 行依赖其版本序）；② §3 sweep requirements 行补「S-F3.1/3.2/3.3 锚行历史名去反引号（已提前应用，随本批一并提交）」；③ §5 增 requirements diff 范围断言（人读 diff == T3 清单、不含 v1.52 残留）。

### MINOR

**MINOR-1 · T2.1 引用不逐字。** `architecture.md:213` 原文为「requirements v1.32 扩展；**信号载体随契约定**」（粗体包裹），T2.1 钉死的目标文本串按字面搜不到。修改指令：引文带上 `**` 或显式声明粗体随替换移除。

**MINOR-2 · T2.3 替换句丢「随契约门」。** 故事删除/音量调节两件仍随契约门在办（正是 :340 ①②③），新句删此后缀反而降准确性。修改指令：control 动作括注保留「，随契约门」。

**MINOR-3 · T1 命名论据不精确 + §四 标题错位。** `kidpal_speak` 并不在契约 §四（它是 front→引擎内网注入消息，仅见于 arch §5.2 与实现）；契约内 kidpal 前缀先例仅 mcp 载荷 `kidpal/display`/`kidpal/alert`。且 §四 标题自锚「小智协议消息子集」，本条是首条 KidPal 私有端→网关消息。修改指令：论据改引准确先例；T1 条目内加半句注记「KidPal 私有扩展消息（非小智原生子集）」。

**MINOR-4 · T1 引用实现类名 `StoryAccumulator.start`。** 契约 §四 既有耦合粒度到角色（网关/front/引擎），不到 Python 类名。建议改写「收敛同一幂等入口（重复触发不重复开会话）」；保留亦可，风格项不强求。

## 四、OQ2 裁定意见

**不必先裁才能 GREEN。** 模板流程即 architect GREEN 后 human 裁 OQ 于 FROZEN；但 T1/T3.1 钉死文本嵌三值闭集——human 若裁 (b) 仅 `story`，T1/T3.1 须机械同步改写（无须回炉重评），且 MAJOR-1 自然消解、MAJOR-2（时序）仍须修。若裁 (a) 三值，则 MAJOR-1 的条件语义修复必须做。建议 human 裁定时一并知悉此耦合。

## 五、Status 行授权充分性判断

human 2026-09-14「没有就现在定义……请 architect review 一遍，没问题再落」记于 Status 行，作为立项 ack + 契约门预授权**充分**；但 FROZEN 凭据必须含 human 对 OQ2 的裁定（这同时就是契约门的最终 human 签字），且 gate evidence 台账要落 human decision 原文与日期（MAJOR-3 ②）。

---

**VERDICT: BLOCKED（9 项打回：MAJOR-1 ~ MAJOR-5 + MINOR-1 ~ MINOR-4）**

---

# R2 · 2026-09-14 · architect（Track B 评审循环）

> 评审对象：PROPOSAL DRAFT v1.1。复核方式：v1.1 全文重读 + R1 九项逐条比对 + 新引入面复核（含 `landing-fidelity-check.mjs` / `governance-gate-shared.mjs` 锚解析逻辑实读）。

## R1 九项吸收确认（全部吸收）

| R1 项 | v1.1 落点 | 判定 |
|---|---|---|
| MAJOR-1（条件语义） | T1：`source_page=="story"` 时切累积态 + 重复触发不重复开会话 + 其余取值现版本无行为（闭集预留）+ 无论取值一律拦截 | ✅ |
| MAJOR-2（时序） | T1：每次发起会话时、首帧音频前 + 首连在 server hello 后 + 通道复用后续会话同样发送 + 重复发送安全 | ✅ |
| MAJOR-3（契约门制品） | 新 T4（CDQ 登记 + gate evidence 义务）、manifest 增 CDQ、§5.5 census 改口恰 4 文件含 CDQ、sweep 增台账行、packet-binding 改 T1–T4 | ✅（但见 R2-MAJOR-1：T4 引入新机械断言破坏点） |
| MAJOR-4（三件→四件） | T2.2 整行替换、替换后完整行钉死（与 :340 现行行逐字核对一致，仅三件→四件 + ④ 追加） | ✅ |
| MAJOR-5（落地前置） | Status「落地前置」行 + §3 sweep 补注 + §5 增 0/3/4/6 四条验证项 | ✅（但见 R2-MINOR-1：v1.53 行记账与先行落地矛盾） |
| MINOR-1（粗体逐字） | T2.1 引文带 `**` 并声明粗体随替换移除 | ✅ |
| MINOR-2（随契约门） | T2.3 保留「，随契约门」 | ✅ |
| MINOR-3（命名论据） | T1 改引契约内 `kidpal/display`/`kidpal/alert`、显式排除 `kidpal_speak`；条目加「KidPal 私有扩展消息、非小智原生子集」 | ✅ |
| MINOR-4（去类名） | T1 改「收敛同一幂等入口，重复触发不重复开会话」 | ✅ |

OQ2 已裁 (a) 三值闭集（Status 行 + §4 记录），T1/T3.1 钉死文本与裁定一致，条件语义已按三值写定——R1 MAJOR-1 的耦合风险消解。Status 行契约门授权链（预授权 + OQ2 裁定收口 + gate evidence 落原文）完整。

## R2 打回项

### MAJOR

**R2-MAJOR-1 · T4 锚清单含 change 目录制品而 manifest 无——`landing-fidelity-check.mjs` B2/T3 断言落地时必 RED（fail-loud）。**
实测核对：T4「目标文件：」行含 `changes/arch/260914-arch-review-kidpal-session-ctx-contract/architecture-gate-evidence.md`（新增，落地相位），而 landing-manifest 围栏块仅 4 个 docs 路径。`governance-gate-shared.mjs` 的 `parseSection2AnchorLists`（:345-419）认 changes/ 制品形态路径入锚集，豁免注记仅「迁出」「条件」两种——「落地相位」不命中；T3 断言（landing-fidelity-check.mjs :302-315）会报「仅 §2 有：changes/arch/.../architecture-gate-evidence.md」→ 前置升级闸 RED。
修改指令（推荐 A）：**A**——T4「目标文件：」行只留 `docs/CONTRACT-DECISION-QUEUE.md`，gate-evidence 移出锚清单、在 T4.2 正文写明「落地相位制品、不入 manifest，由 §5 验证项 6 核验在场」（与 landing-summary 不入 manifest 同理）；或 **B**——manifest 增第 5 行该路径（引入 change 目录制品入 manifest 的新形态，非先例，须连带确认 manifest↔git 改动比对容忍新增文件）。取 A 则 §3 sweep 无需改（台账行已列）；取 B 则 manifest 与 §5 各项相应同步。

### MINOR

**R2-MINOR-1 · T3.2 v1.53 变更行记账与 T3.3 先行落地矛盾。**
v1.53 行记「S-F2.2 信号载体契约落定 + 查字锚行历史名去反引号」，但 T3.3 钉死注明去反引号已随 v1.52 批先行 commit、本批 landing diff 不含——读者按 v1.53 行理解该变更随 v1.53 落地，git blame 却指向 v1.52 commit。实测工作区 v1.52 行（未提交）未记去反引号。
修改指令（二选一）：① v1.52 行（尚未 commit）补注「+ S-F3.1/3.2/3.3 锚行历史名去反引号（零语义变更，机检误报清零）」、v1.53 行删去该项（推荐——叙事与 diff 同 commit）；或 ② v1.53 行该项加括注「（随 v1.52 先行落地）」。

**R2-MINOR-2 · T4.1 CDQ 行未逐字钉死。**
T4.1 枚举字段但未给出完整 8 列行文本，落地时留即兴空间，与 T1/T2 钉死纪律不一致。修改指令：钉死完整行（含 CDQ-id / 日期 / 变更面（§四 新增端→网关消息 `kidpal_session_ctx`，additive）/ 四分类 CONTRACT_CHANGE_REQUIRED / human 门（2026-09-14 预授权 + OQ2 裁定）/ 状态 PRECHECKED / 关联 change / 剩余条件（落地核验后 LANDED））。

## 结论

R1 九项全部吸收、OQ2 裁定已闭合；R2 打回 3 项全部为 T4/T3 文本级修订（其中 R2-MAJOR-1 是机械闸破坏点、必须修），无方向性返工。修毕回炉 R3 预期 GREEN。

**VERDICT: BLOCKED（3 项打回：R2-MAJOR-1 + R2-MINOR-1 + R2-MINOR-2）**

---

# R3 · 2026-09-14 · architect（Track B 评审循环）

> 评审对象：PROPOSAL 正文 R2 修订版。复核方式：全文重读（含头部/T1 段未申报改动核对）+ R2 三项逐条比对 + 锚并集 ≡ manifest 机械核对 + 工作区 v1.52 行实读。

## R2 三项吸收确认（全部吸收）

| R2 项 | 修订落点 | 判定 |
|---|---|---|
| R2-MAJOR-1（锚清单/manifest 失配） | T4「目标文件：」行仅 `docs/CONTRACT-DECISION-QUEUE.md`；T4.2 注明 gate-evidence「落地相位制品、不入 landing-manifest（与 landing-summary 同理），在场性由 §5 验证项 6 核验」。锚并集 = contract / architecture / requirements / CDQ 四路径 = manifest 围栏块四行，`parseSection2AnchorLists` B2/T3 断言不再触发（T4.2 散文中的 backtick 路径不在「目标文件：」行、不入锚集） | ✅ |
| R2-MINOR-1（记账矛盾） | T3.2 v1.53 行不含去反引号项 + 显式指令 v1.52 行补注；实测工作区 v1.52 行已含「⑤ S-F3.1/3.2/3.3 锚行括注中历史名 canvas-p4 去反引号（零语义变更）……机检归零」，随 v1.52 独立 commit 同体落地——叙事与 diff 对齐 | ✅ |
| R2-MINOR-2（CDQ 行钉死） | T4.1 逐字钉死完整 8 列行（CDQ-id / 日期 / 变更面 / 四分类 CONTRACT_CHANGE_REQUIRED / human 门（预授权 + OQ2）/ 状态 PRECHECKED（落地核验后转 LANDED）/ 关联 change 相对链接 / 剩余条件），列头与既有表对齐、状态词合 CDQ vocabulary、含 `kidpal_session_ctx` 与 §5.5 census 一致 | ✅ |

头部（Status / 落地前置 / packet-binding / manifest）与 T1/T2 段逐字核对无未申报改动；T1 钉死文本（条件语义 + 时序 + 三值闭集 + 私有消息注记）维持 R2 已认可形态。

## 终评

- 硬项 1a 证据（R1 全量抽验属实）、硬项 2a（锚清单齐备、`### T<n>` 形态、承重指令逐字钉死含整行替换）、硬项 1b/1c（不立新门、census 机械枚举）全部合规。
- 技术方案：载体选型、条件语义、时序（常驻连接通道复用覆盖）、鉴权面（accept 前 Bearer 拦截）、向后兼容、E1/§1.4 零影响——R1/R2 已逐项核验成立，无回归。
- 契约门链完整：四分类 CONTRACT_CHANGE_REQUIRED、human 预授权（2026-09-14 原文记 Status 行）+ OQ2 裁定收口、CDQ 登记（T4.1）+ gate evidence 台账义务（T4.2）。
- FROZEN 相位提示（非打回项）：① Status 行版本标签现仍写「DRAFT v1.1 · R1 修订回炉待 R2」，FROZEN 时按模板更新为 FROZEN vX.Y + 三凭据（human 立项 ack + R3 GREEN + OQ2 已裁）；② 落地前置 = v1.52 批先行独立 commit（§5.0 核对）；③ gate evidence 台账随 landing 相位产出、landing-verifier 按 §5.6 核验。

**VERDICT: GREEN（可 FROZEN）**
