# PROPOSAL · 260914-arch-review-kidpal-session-ctx-contract

> **状态：FROZEN v1.1 · 2026-09-15 · 凭据：human 立项 ack（2026-09-14「没有就现在定义……没问题再落」）+ architect R3 GREEN（2026-09-15，R1/R2 打回项全吸收）+ human 裁 OQ2 = 三值闭集（AskUserQuestion 2026-09-14）**
> **承载 ARQ**：无在档 ARQ——源自 2026-09-14 架构侧可行性 review 发现「S-F2.2 P3 信号载体随契约定但未挂契约门清单」；本批收口后如需挂账走 changes-index 登记。
> **review 轨**：Track B 常驻轻路径（SSOT：`AGENTS.md`「治理路由」节）。**触契约面 `docs/contracts/**` → 升级判据①命中，走契约门**：architect-reviewer 四分类预审 + human 契约门（human 2026-09-14 指示「没有就现在定义……请 architect review 一遍，没问题再落」= 立项 ack + 契约门预授权，授权记本 Status 行；契约门最终 human 签字 = OQ2 裁定，gate evidence 台账落原文）。
> **落地前置**：工作区另一批 v1.52（朗读砍除 + 可行性三态批改 + S-F3.1/3.2/3.3 锚行历史名去反引号）须**先行独立 commit**（本批 v1.53 版本序依赖）；T3.3（去反引号）已随 v1.52 先行落地（FROZEN 前动 canonical，如实记录），本批 landing diff 不含此项。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low——纯文档面 additive 变更（契约新增一条端→网关消息、两 spec 指针句更新、CDQ 登记），零行为变更（实现面归 dev-loop 后续包），向后兼容（省略消息 = 现状）。
> **review profile**：G2+G3 + 契约门（触面：docs/contracts/**（契约门）、docs/architecture.md、docs/requirements.md、docs/CONTRACT-DECISION-QUEUE.md；实现面 firmware/ server/ 零触）。

```landing-manifest
docs/contracts/interface-contract.md
docs/architecture.md
docs/requirements.md
docs/CONTRACT-DECISION-QUEUE.md
```

---

## 0. 想实现的效果与差距

目标效果一句话：S-F2.2「双入口开录」的 P3 会话上下文信号（设备告知网关「会话发起自 P3」）拿到契约定义的载体，消除「机制已进架构/需求、载体却无契约归属」的悬空态。

现状差距：需求与架构均已登记该信号（requirements S-F2.2 补充规则「技术信号 = 设备须把……网关即切累积态」；arch §5.2 场景④「信号载体随契约定」），但 `docs/contracts/interface-contract.md` 无对应消息定义，arch §7 契约门在办清单也只列了三件（schedule 网格 / 故事删除 / 音量调节）、不含本项——契约门时点易漏。

非目标：① 不实施固件/front 实现面（归 dev-loop 后续包，本批只登记）；② 不动 §1.4 control 动作表、E1 view 白名单、B1 课表结构；③ 不引入引擎侧任何感知或改动。

## 1. 问题与根因（地面真值）

### 1.1 信号无契约载体

- **证据 A（契约面）**：`docs/contracts/interface-contract.md` §四「端云协议剖面」板→云方向仅列 Opus 二进制帧与 `abort` 消息（:230「**`abort` 消息**（端→云）：喊停/打断……」）；hello 条目（:227）只写设备自报 audio_params。全文无 `kidpal_session_ctx` 或任何「会话来源页」载体（2026-09-14 grep 核实零命中）。
- **证据 B（spec 面悬空）**：`docs/architecture.md:213` 场景④ 原文「……或 P3 会话上下文信号（设备告知「会话发起自 P3」，网关会话建立即切累积态——requirements v1.32 扩展；**信号载体随契约定**）……」。requirements S-F2.2 补充规则：「技术信号 = 设备须把「会话发起自 P3」上下文告知云端、网关即切累积态（arch §5.1 故事归档组件·`start_recording` 时机扩展）」——两侧都指向契约，契约缺位。
- **证据 C（登记缺口）**：`docs/architecture.md` §7 契约门在办（:340）列三件：① E1 schedule 视图周网格；② 故事删除；③ 音量调节。无 P3 信号项。
- **根因**：需求 v1.32 扩展先行、架构重构（260914）收编场景④时保留「载体随契约定」占位，但契约门登记动作在对照 review 中漏项（本发现 = 2026-09-14 架构侧可行性 review 提出后 human 裁定「没有就现在定义」）。

### 1.2 现有累积态切换链（载体必须收敛的既有事实）

- **证据 D（唯一现役触发）**：`server/gateway/front/front_app.py:82-92` `_story_route_display_payload`——旁路消费 OpenClaw display 块中 `view=="control" && data["action"]=="start_recording"` → `_get_story_acc().start(session_id)`。触发词入口（S-F2.2 入口②）走此链；P3 入口（无触发词）无法走此链——意图判断发生在云端，而 P3 语境只有设备知道。
- **证据 E（front 逐帧解析现状）**：front_app.py 上行泵 `pump_client_to_upstream` 对每条文本帧 `json.loads`（:383 处仅用于 listen/stop 判定），hello 及未知类型**全部透传**（:343 注释「no text field (hello/state): verbatim」）；`ws_gateway`（:563-620）accept 后不解析 hello。→ front 具备拦截新消息类型的天然挂点，且现状对未知类型是透传语义（新消息须显式拦截才不下透）。
- **证据 F（幂等收敛点）**：`server/gateway/story.py:305-309` `StoryAccumulator.start`——注释明示「幂等：重复 start 不重复开会话、不清空已在累积的内容」。→ 新载体与 control/start_recording 双路收敛同一入口安全。
- **证据 G（设备侧上下文可得性）**：`firmware/main/protocols/websocket_protocol.cc:239-262` `GetHelloMessage()` 无参数、协议层不读页态；会话发起入口 `application.cc:575-610`（`HandleToggleChatEvent`）不查 kidpal_layer 栈；但层栈可查询（`firmware/main/display/kidpal_layer.h:24-52`，`kidpal_layer_top()`）。→ 设备侧实现（dev-loop 包）在 application 层发信号即可，协议层 hello 不动。引擎侧无感知需求：manual 聆听由既有 listen 消息承载（application.cc:739 `SendStartListening`），累积是 front 旁路机制。

## 2. 改法（T1–T4）

### T1 · 契约面：§四 新增 `kidpal_session_ctx` 消息条目

改什么：`docs/contracts/interface-contract.md` §四「端云协议剖面」——在「`abort` 消息」条目（:230）之后、「`mcp` 工具调用」条目之前插入一条。

怎么改（插入后完整句，逐字钉死）：

> - **`kidpal_session_ctx` 消息**（端→网关，KidPal 私有扩展消息、非小智原生子集；2026-09-14 新增）：设备在**每次发起会话时、首帧音频前**发送 `{"type":"kidpal_session_ctx","source_page":"story"}`——首连时序上在 server hello 之后，wss 常驻连接的通道复用后续会话同样发送，重复发送安全（收敛入口幂等）。`source_page` 取值闭集 = `story` / `home` / `schedule`（story = P3 星星记录本按 KEY 发起的记录语境会话）。**网关消费并拦截、不下透引擎**：`source_page=="story"` 时 front 即切该会话累积态（与 §1.4 `control/start_recording` 收敛同一幂等入口，重复触发不重复开会话）；其余取值现版本无行为（闭集预留，供未来切页语境扩展）。无论取值，整条消息一律由网关拦截。省略整条消息 = 普通会话，向后兼容。设备 manual 聆听仍经既有 listen 消息承载，引擎不感知本信号。

为什么：承载走独立 kidpal 前缀消息而非 hello 扩展——① 设备侧协议层 `GetHelloMessage` 无参数、注入页态需改签名（分层违例，证据 G）；② hello 会透传引擎、未知字段容错未验证，独立消息由 front 拦截后引擎零触（证据 E）；③ 命名先例 = 契约内既有 kidpal 前缀 mcp 载荷 `kidpal/display`/`kidpal/alert`（`kidpal_speak` 是 front→引擎内网注入消息、不在契约面，不引）。对根因 1.1/1.2。

目标文件：`docs/contracts/interface-contract.md`（改）

### T2 · 架构面：载体句落点 + 契约门清单补项 + to-be 登记

改什么/怎么改（三处，均逐字钉死）：

1. `docs/architecture.md:213` 场景④ 中「requirements v1.32 扩展；**信号载体随契约定**」（粗体包裹）→ 替换为「requirements v1.32 扩展；信号载体 = 契约 §四 `kidpal_session_ctx`（front 拦截消费、引擎不感知）」（粗体随替换移除）。
2. `docs/architecture.md` §7 契约门在办清单（:340）**整行替换**——现行行首「另登记三件」与追加 ④ 矛盾，替换后完整行：
   > - **契约门在办**：CDQ-260913-spec-split-fallout（spec.md 归档断链 + 验收基线重建，HUMAN_APPROVED 在办）；另登记四件随下次契约门裁（2026-09-14 对照 review）：① E1 schedule 视图承载一周总览网格数据（requirements v1.19）；② 「故事删除」动作（requirements v1.47/v1.48，§5.4 数据视图）；③ 「音量调节」动作（requirements v1.44，§5.1.1 MCP 工具面）；④ `kidpal_session_ctx` 端→网关消息（2026-09-14 已定义，随本批契约门落）。
3. `docs/architecture.md` §9 to-be ⑥（:379）「⑥ 新增 control 动作（故事删除/音量调节，随契约门）」→ 替换为「⑥ 新增 control 动作（故事删除/音量调节，随契约门）+ 端→网关 `kidpal_session_ctx` 发送/拦截（契约已定义；固件侧接线点 = application.cc 会话发起入口读 `kidpal_layer_top()`，front 侧 = 上行泵帧解析处拦截）」。

为什么：载体句消除「随契约定」占位、清单补项防契约门漏项、to-be 把实现面挂进 dev-loop 演进序列。对根因 1.1（证据 B/C）。

目标文件：`docs/architecture.md`（改）

### T3 · 需求面：S-F2.2 补充规则载体引用 + 变更记录行

改什么/怎么改：

1. requirements S-F2.2 补充规则中「（arch §5.1 故事归档组件·`start_recording` 时机扩展）」→ 替换为「（arch §5.1 故事归档组件；信号载体 = 契约 §四 `kidpal_session_ctx`，source_page=story，front 拦截切累积态）」。
2. §9 变更记录追加 v1.53 行（append-only，新行在上）：记「S-F2.2 信号载体契约落定（零语义变更，载体引用 + 变更行）」，依据 = 本批（human 指示 + architect GREEN + OQ2 裁三值）。**不含去反引号项**（该项归 v1.52 批记账）；同时 v1.52 变更行（本批 commit 前仍在工作区）补注「含 S-F3.1/3.2/3.3 锚行历史名去反引号」——去反引号记账随 v1.52 行、与其实际 commit 同体（R2-MINOR-1）。
3. 顺手清债（human 2026-09-14 问询「锚债是啥」后裁清）：S-F3.1/3.2/3.3 三条锚行括注中历史名 `canvas-p4` 去反引号（原 canvas-p4）——该名是 P4 撤页后设计稿正确缺席的旧 od-id，反引号包裹被 spec-anchor-check 误当现行 UI 锚致 3 条悬空误报；去反引号保留追溯文字、零语义变更，机检归零。**此项已提前应用于工作区、随 v1.52 独立 commit 先行落地（FROZEN 前动 canonical，如实记录）——本批 landing diff 不含此项。**

为什么：需求面指向契约的载体引用闭合。零 story/AC 语义变更、锚行可行性不动。

目标文件：`docs/requirements.md`（改）

### T4 · 契约门台账：CDQ 登记 + gate evidence 义务

改什么/怎么改：

1. `docs/CONTRACT-DECISION-QUEUE.md` 表末追加一行（append-only，沿用 8 列形态，逐字钉死）：
   > | CDQ-260914-kidpal-session-ctx | 2026-09-14 | `interface-contract.md` §四 新增「`kidpal_session_ctx` 消息（端→网关）」条目（source_page 三值闭集 story/home/schedule；网关拦截不下透引擎；仅 story 切累积态） | CONTRACT_CHANGE_REQUIRED（独立契约变更） | human 预授权 2026-09-14（「没有就现在定义……没问题再落」）+ OQ2 裁三值（AskUserQuestion 2026-09-14） | **PRECHECKED**（本批落地核验后转 LANDED） | [260914-arch-review-kidpal-session-ctx-contract](../changes/arch/260914-arch-review-kidpal-session-ctx-contract/) | architecture-gate-evidence.md 台账落地 + landing-verifier 绿 |
2. gate evidence 台账义务：本 change 目录在 landing 相位产出 `architecture-gate-evidence.md`（architect-reviewer 分类行、human decision 原文与日期、applied diff inventory、validation 结果——骨架按 `.claude/references/contract-change-evidence.md` 十节），主线程落地相位写、landing-verifier 核验。**此件为落地相位制品、不入 landing-manifest**（与 landing-summary 同理），在场性由 §5 验证项 6 核验。

为什么：契约面变更按 `.claude/references/contract-change-evidence.md`「Required Artifacts」须 Queue update + evidence 台账（260902/260913 先例同形态）。对 MAJOR-3。

目标文件：`docs/CONTRACT-DECISION-QUEUE.md`（改）

## 3. 影响面 sweep 分类（UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 契约 | docs/contracts/interface-contract.md | 契约变更·独立 | §四 additive 一条消息定义；走 architect-reviewer 四分类预审 + human 契约门（本批） |
| 契约门台账 | docs/CONTRACT-DECISION-QUEUE.md | G2 | T4.1 CDQ 行 append-only 登记 |
| spec | docs/architecture.md | G2 | 载体句/清单/to-be 三处指针更新 |
| spec | docs/requirements.md | G2 | S-F2.2 载体引用 + v1.53 变更行；另 S-F3.1/3.2/3.3 锚行历史名去反引号已提前应用、随 v1.52 独立 commit 先行落地（本批 landing diff 不含） |
| 实现 | firmware/、server/gateway/** | 未触 | 实现归 dev-loop 后续包（T2.3 已登记接线点）；本批零代码改动 |
| 消费方 | `spec-anchor-check.mjs` 机检 | 未触 | 契约无编号标题、不进机检范围；spec 锚行零变更 |

**硬项 1b 预跑**：本批不立新门/新断言；验证集 5.3 的一致性 grep 即 census（命令 + 输出留痕见 §5）。

## 4. human 取舍点（OQ）

- **OQ1（已裁）**：是否现在定义契约——human 2026-09-14：「有契约吗，没有就现在定义」。裁 = 定义（本批）。
- **OQ2（已裁）**：`source_page` 闭集宽度——human 2026-09-14 AskUserQuestion 裁 **(a) 三值 `story/home/schedule`**（推荐位采纳：机械校验白名单一次到位，未来切页语境可扩展；非 story 取值现版本无行为）。T1/T3.1 钉死文本已按三值 + 条件语义（仅 story 触发累积态）写定。
- **OQ3（已裁）**：实现面是否本批做——human 流程指示为「proposal → architect review → 落」，落 = 文档面；实现归 dev-loop（T2.3 登记）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

0. 落地前置（MAJOR-5）：v1.52 批（朗读砍除 + 可行性批改 + 去反引号）已先行独立 commit——landing 前用 `git log --oneline -1 -- docs/requirements.md` + `git status --short docs/requirements.md` 核对（工作区 requirements 应只剩本批改动）。
1. `node .claude/scripts/spec-anchor-check.mjs` → 复跑 `ok:true`、errors 空（T3.3 落地后基线已实测归零，2026-09-14 exit=0）。
2. 契约面 diff 范围断言：`git diff -- docs/contracts/interface-contract.md` 仅 §四 新增一条（人读 diff 核对 == T1 钉死文本），§1.4/E1 白名单/B1 零变更。
3. requirements diff 范围断言：人读 diff == T3.1（S-F2.2 载体引用句）+ T3.2（v1.53 变更行）+ 版本号行，不含 v1.52 残留（v1.52 已先行独立 commit）。
4. CDQ diff 范围断言：`git diff -- docs/CONTRACT-DECISION-QUEUE.md` 仅追加一行（T4.1），既有行零变更（append-only）。
5. 跨面一致性 census：`grep -rln "kidpal_session_ctx" docs/` → 恰 4 文件 = contract / architecture / requirements / CONTRACT-DECISION-QUEUE（PROPOSAL 与本批其余制品不计），语义口径一致（端→网关、front 拦截、引擎不感知、仅 story 触发累积态）。
6. gate evidence 台账在场：本目录 `architecture-gate-evidence.md` 存在且含分类行 / human decision 原文与日期 / diff inventory / validation 四要素。
7. `git diff --check` → 无空白错误；`git diff --cached --stat` 于 add 后、commit 前核验（仓规）。

## 6. 留根清单

- **实现面留根**：固件（application 层发信号 + KEY 路由 P3 判定）与 front（上行泵拦截）归 dev-loop 后续 TaskPackage；实现须走 red-baseline + 验收齿（现状信号不存在 = 天然红基线）。
- **登记不硬治**：E1 display_schema 的 view 白名单不含本消息（它不是 display 块、走上行泵直解析），无需 schema 扩展——此判断随实现包复核。
- **交复盘项**：契约门在办清单（arch §7）本次补项属「对照 review 漏项后补」——复盘时过一眼其余三件是否同样存在「spec 已提、清单未挂」漏项形态。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-14 · architect（Track B 评审循环）

**VERDICT: BLOCKED（9 项打回：MAJOR-1 ~ MAJOR-5 + MINOR-1 ~ MINOR-4）**——方向与载体选型成立（独立消息优于 hello 扩展、拦截语义正确、安全面成立、E1/§1.4 零影响属实、硬项 1a 抽验全部属实、机检归零实跑复核通过），打回项全部为文本级修订、无返工级缺陷。要点：T1 钉死句累积态切换未限定 source_page=story（MAJOR-1）；T1 时序「收到 server hello 后」与 §四 wss 常驻连接/通道复用场景矛盾（MAJOR-2）；契约门制品缺口——无 CDQ 登记 T 项、无 gate evidence 台账计划、§5.3 census「恰 4 文件」括注自相矛盾（MAJOR-3）；T2.2 追加 ④ 与 :340 行首「另登记三件」矛盾（MAJOR-4）；落地前置未声明——工作区 v1.52（朗读砍除）未提交改动同文件纠缠 + T3.3 已提前应用（MAJOR-5）。OQ2 不必先裁才能 GREEN，但裁 (b) 时 T1/T3.1 须机械同步改写。逐条修改指令见 [architect-review.md](architect-review.md)（本目录）。

### R2 · 2026-09-14 · architect（Track B 评审循环）

**VERDICT: BLOCKED（3 项打回：R2-MAJOR-1 + R2-MINOR-1 + R2-MINOR-2）**——R1 九项全部吸收、OQ2 裁定 (a) 三值已闭合（T1/T3.1 与裁定一致、条件语义正确）。R2 打回全部为文本级：① T4 锚清单含 change 目录制品 `architecture-gate-evidence.md` 而 manifest 无此行——`parseSection2AnchorLists` 认 changes/ 形态入锚集且「落地相位」注记不命中豁免，B2/T3 一致性断言落地时必 RED（推荐修法：gate-evidence 移出锚清单、T4.2 注明「落地相位制品、不入 manifest，由 §5 验证项 6 核验在场」）；② T3.2 v1.53 行记账「去反引号」与 T3.3「已随 v1.52 先行落地」矛盾——v1.52 行（未提交）补注该项并从 v1.53 行删去，或 v1.53 行加括注「（随 v1.52 先行落地）」；③ T4.1 CDQ 行应逐字钉死完整 8 列行文本。无方向性返工，修毕回炉 R3 预期 GREEN。逐条修改指令见 [architect-review.md](architect-review.md) R2 节。

### R3 · 2026-09-14 · architect（Track B 评审循环）

**VERDICT: GREEN（可 FROZEN）**——R2 三项全部吸收：① T4 锚清单仅 `docs/CONTRACT-DECISION-QUEUE.md`、gate-evidence 注明「落地相位制品、不入 manifest（与 landing-summary 同理）」——锚并集 = manifest 四行，B2/T3 断言不再触发；② v1.53 行不含去反引号项 + 实测工作区 v1.52 行已含「⑤ 锚行历史名去反引号」记账、随 v1.52 独立 commit 同体落地；③ T4.1 CDQ 行 8 列逐字钉死、列头与既有表对齐。头部与 T1/T2 段逐字核对无未申报改动。硬项 1a/2a/1b/1c 合规；技术方案（载体选型、条件语义、时序、鉴权面、向后兼容、E1/§1.4 零影响）R1/R2 已逐项核验成立、无回归；契约门链完整（四分类 + human 预授权 + OQ2 裁定收口 + CDQ 登记 + gate evidence 义务）。FROZEN 相位提示（非打回项）：Status 行版本标签按模板更新为 FROZEN vX.Y + 三凭据（human 立项 ack + 本 R3 GREEN + OQ2 已裁）；落地前置 = v1.52 先行独立 commit（§5.0 核对）；gate evidence 台账随 landing 相位产出（§5.6 核验）。完整记录见 [architect-review.md](architect-review.md) R3 节。
