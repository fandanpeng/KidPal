# PROPOSAL · 261115-arch-review-session-channel-resident

> **状态：FROZEN v1.2 · 2026-11-16**（FROZEN 凭据 = 原 v1.1 凭据继续有效 + 门 1（拆包冻结门）BLOCK(F-01) 回修 → R3 增量复核（F-01 CLOSED / BLOCK(F-04)）→ v1.2 补正 → R4 一句话闭合 **GREEN（F-04 闭合）** + 主线程重冻。v1.2 只改 T2 改法 2（按 `acc.routing` 分层表）、ctx 边沿分态、package-split 同步与 canonical delta 处方；**契约面零改动、ADR-019 裁定零改动**。作者≠判官：R1–R4 独立审链承载；§7 判官区 append-only。**（原 v1.1 FROZEN 凭据留痕不删：human 2026-11-16 立项 ack + OQ1/OQ2/OQ3 三条裁 (a) + R1 四查 BLOCK(F1,F2,F3) → v1.1 修毕 → R2 靶向复核 GREEN + 主线程标冻。判档 = 主路径 Track B；契约四分类 = 主 `ACCOMPANIES_SPEC_CHANGE` + 次 `SPEC_CONTRACT_DRIFT`（authoritative 枚举 = `.claude/rules/contracts.md`）；Track A = 否。作者≠判官由 R1/R2 独立审链承载，契约无人签门、合并过机械凭据闸。human 裁断三条：OQ1 = (a) front 周期下行 `kidpal_keepalive` 心跳帧；OQ2 = (a) 三通道同落点、保持 OQ3=c 回答页驻留、契约 §1.4 表行措辞对账；OQ3 = (a) 接受引擎 24h 兜底断连为唯一剩余重建窗口。）**v1.1 修订摘要（R1 否回应改，逐 finding 对应；三项均由 architect 独立复核后采纳，复核命令与输出 verbatim 见文末「起草期实跑记录 ⑥」）**：**R1-F1**（契约 SSOT 事实错）= T4 改法 1 的 `state` 闭集条原拟注「`detect` = 上游原生，KidPal 侧不发」为假——实核 `protocol.cc:68-72` `SendWakeWordDetected` 即发 `listen detect`，调用点 `application.cc:1217`（`CONFIG_SEND_WAKE_WORD_DATA=y`，`sdkconfig:733`，本板活路径）与 `:1500`（`CONFIG_USE_AFE_WAKE_WORD=y`，`sdkconfig:731`），已改写为「本板确发」并补三值钉进契约的理由（`detect` 不参与会话边界语义）；§2 实证留痕协议侧 bullet 同步补 `listen` 帧族三值原语逐条现核。**R1-F2**（牵连面账实不符）= `docs/architecture.md` 对退役机制「会话收尾常驻重建」共 **4 处**引用，v1.0 只覆盖 `:371`（§6.4）与 `:460`（§8.1），漏 `:138`（§3.3 假设清单）与 `:529`（§10 决策 29）——已补 T3 改法 5（§3.3 行加沿革注记，假设本体不变且被强化）与改法 6（§10 决策 29 加「⚠️ 2026-11-16 取代注记（部分）」，**正文一字不改**，形态逐字仿决策 21/25 先例，不破 §10 历史档冻结纪律），T3 标题/问题/证据/根因随之扩为四处对账，T7 的 §12 改动面列补两节 + 新增牵连面完整性段（附普查命令），§3 sweep 架构行与透镜义务段补 (乙) 退役机制文档引用点普查，§5 新增 V17/V18/V19（含 V19 守恒断言：四处命中数 post 仍为 4，证明「历史不删只标取代」）。**R1-F3**（数目偏差）= §2 实证留痕「`protocol_->CloseAudioChannel()` 全仓 10 命中」实为 **9**（原把 `:976` 录音丢弃面重复计入「另有」子句，而它本属「会话收尾 4」之一）——已改 9 并显式写出「会话收尾 4 + 真拆通道 5 = 9」的分界与其正负锚含义；4+5 两侧成员名单未变，故 T2 改法 1「五个真拆通道点零改动」与 `package-split.md` 的 SCR-A3 断言无需同步改。**R1 advisory**（非阻断，已顺手落）= `protocol.cc:129-136` 与 `application.h:126-130` 两处注释随本批改法漂移，登记 §6 留根 8：前者未在 manifest（只改注释、逻辑零改动）交 P1 planner 经扩单通道回评，后者已在 P1 manifest 随批刷新。**v1.2 修订摘要（门 1 F-01 · 设计级缺陷回修；architect 独立复核后采纳，复核命令与引据见 T2 改法 2「F-01 修订依据」段与文末「起草期实跑记录 ⑦」）**：v1.1 的 T2 改法 2「会话收尾三件之②：残余累积态取消（routing 仍为 `accumulate`/`pending_stop` 且本帧未被既有归档路径消费时 `acc.cancel`）」与三条 must-stay-green 冻结 oracle **断言级矛盾**——`pending_stop`（录音态 KEY 收尾、accum 空、manual 形态转写约 1.7s 后到）与「引导态无内容」在 listen-stop 那一刻**可观状态完全相同**，该守卫无法区分；当刻 cancel = `story.py:886-894` `_sessions.pop` → 在途转写不可恢复 → 三式收尾之一的归档丢失，违反 S-F2.2 AC4 v1.65。冲突齿 = SAO-A2（`test_m4k1_story_cloud_link.acceptance.py:1891`）+ M4K1-A2（同文件 `:849-882`，`len(files)==1`）+ SAO-A3（重复 stop 不双归档）。**修订五条**：① T2 改法 2 重写为**按 `acc.routing` 分层的五行处置表**，`pending_stop` 列为 protected state（禁止当刻 cancel、禁止当刻释放），撤销原「②残余累积取消」动作，收尾三件重述为「分层释放 / 不当刻取消 / 审计一行」；② 识别并补上 F-01 的**承重缺口**——`_story_timeout_sweep_body:1255-1257` 的 pending 分支今天只 `release_pending` + 日志、**不调 `_release_record_context`**（旧模型靠随后断连兜底，常驻后兜底消失 → 引擎永久抑制 → 哑火），本批在该分支新增释放调用（腿集沿 `_story_auto_finish:1353` 的 `_session_legs(sid) or list(_alert_legs)` 形态），这是 front 侧唯一新增释放点；③ ctx 边沿兜底改为**分态处置**（`pending_stop` 不动作、`accumulate` cancel+释放、`chat` 但标记真只释放），并显式申报 **≤10s 引擎抑制窗为 accepted residual**（不得用提前释放消除——那正是 261022 修掉的孤儿幻听缺陷，`_issue_finish_downlink:1661-1665` 实录），登记 §6 留根 9 + §8.1 新增风险行 ④ + 真机 A5 实测；④ **契约面确认零改动**（§2A 新增专条给三条依据：释放时点属行为策略归 arch §5.1.2/§5.2、discard 顺序约束走独立路径不受影响、10s 挂起窗契约从未钉过；核对基准 = **已落地的**契约现文，`grep -c 'pending_stop|记录语境释放|残余累积' docs/contracts/interface-contract.md` → 0）→ `proposedContractDiff` 四处一字未改、`landing-manifest` 与 V4/V5/V9–V12 全不受影响，`docs/contracts/**` 与 `docs/decisions/ADR.md` **零 delta**；**但 `docs/architecture.md` 需 canonical delta**——v1.1 的落地文本（commit `87dbf775`）§5.1.2 front 行仍逐字载有 F-01 缺陷句「② 残余累积态取消…routing 仍为 `accumulate`/`pending_stop`…」、§5.2 场景② 亦载「记录语境释放 + 残余累积取消」，故本 v1.2 附**五条 delta 处方**（§2 文末「v1.2 canonical delta 处方」：delta-1 §5.1.2 收尾三件改分层 / delta-2 ctx 边沿分态 / delta-3 §5.2 括注 / delta-4 §8.1 风险行 ④ / delta-5 §12 行内追加不新增行），交主线程 packet-bound 落地，architect 不写 canonical；⑤ 牵连面同步：§2A impact 冻结齿段补三条 must-stay-green + 两条「断言保绿但 docstring 前提改变」的显式处置（F-03 要求，含 `story.py` 两纯逻辑缝签名零改动声明）、§2A validationPlan 层 2 改写、§3 sweep 新增 `test_m4k1_story_cloud_link` / `test_m4k3_record_session_live` / `story.py` 三行并改写 `front_app.py` 为五缝、T3 改法 2 风险行 3→4、T7 改动面与 §3 架构行的「§8.1 行数」同步 3→4。**未改**：改法集方向（三层解耦 / `listen stop` 复用 / 心跳保活 / 长重试 / 六触发源统一收口）、T1/T4/T5/T6/T7 主体、§4 三条 OQ（human 已裁 (a)）、V1–V19 全部预期值（本轮不触 canonical token 计数）。`package-split.md` 同步改 §2.3 front 要点 2/3、§2.4 SCRF-B2/B4（含新增 B2b/B4b）、§4.3 真机 A5 判据时点精确化。来源 = human 2026-11-15 原话「开始处理这个debt，我倾向于治本，但需要你架构师讨论清楚方案，确定好方案拆包不要拆的太碎，拆好好后 你就headless 启动dev loop 开始开发」+ 同日 AskUserQuestion 三答（范围 = 全家桶 / 记忆口径 = 共享短上下文 / 落地边界 = 允许 agent 部署 ECS front 重启 + human 持板真机轮，**未选「允许改引擎补丁」**）。
> **承载 debt**：`docs/debt/tech/TECH-261114-overlay-close-channel-rebuild-churn.md`（主条目，候选 A 治本）+ `docs/debt/tech/TECH-261014-autoconnect-gives-up-silently.md` + `docs/debt/tech/TECH-261014-session-end-channels-unwired.md`（含 2026-10-19 范围收窄注记：120s 录音态阈值已随 ADR-013 作废，只剩单阈值 60s）+ `docs/debt/tech/TECH-261012-first-keypress-no-listening.md`（根因链被本批消解，见 T2 改法末段）。四条 consumed-by 本 changeId，结项归主线程记账。
> **review 轨**：Track B（默认路由；SSOT = `.claude/references/arch-change.md`）。**判档建议 = 主路径**（轻路径判据 ①③④ 三否：有语义变化、触 `docs/contracts/**`、触 architecture.md §2.3 端云职责划分与 §6.x 横切机制——arch-change.md :25）；正式判档归 architect-reviewer，只升不降。
> **packet-binding（FROZEN 生效）**：packet-id = 本 PROPOSAL §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **landing-lane**：`packet-bound`（架构+契约面恒由主线程落地；本批作者实施范围不含 canonical 文件）。
> **riskTier**：medium（架构面：触 §2.3 端云职责划分的会话/通道边界重划 + §6.4 整节重写；契约面：additive 一条下行心跳帧 + `listen` 帧语义钉行 + `end_session` 放行规则改写，既有帧形状与 display 白名单零改动；实现面 riskTier 另由 dev-loop 分档承载，见 `package-split.md`）。
> **review profile**：架构面 + 契约面（触契约分支，显式标注）；G1/G2/G3 治理面 = 零（不触 `.claude/**`）。
> **四分类**（权威枚举 = `.claude/rules/contracts.md`）：**主触点 = `ACCOMPANIES_SPEC_CHANGE`**（契约改动随本批架构变更携带、同批原子落地）；**次触点 = `SPEC_CONTRACT_DRIFT`**（契约 §1.4 表后「front 自推通路也不把这两个 action 发给设备」与 `display_schema.py` 的「end_session 即便自推仍拒」，同 requirements §2.1 结束通道②「云端判为结束意图 → 关闭」冲突——本批裁向「改契约放行规则 + 改实现门禁」，requirements 语义零改动）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段，GREEN 结论前必须在场；判官随 GREEN 结论核对、主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/architecture.md
docs/contracts/interface-contract.md
docs/decisions/ADR.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把「语音会话」与「WS 通道」两个生命周期彻底解耦——通道常驻（唯一终止源 = 服务端/网络侧断、引擎 24h 兜底、重启/OTA），语音会话改由**既有带内信号**（`kidpal_session_ctx` + `listen start` / `listen stop`）划界；孩子每次「结束 → 再说话」都走温通道，冷启动税与 bootstrap burst 归零；同时把 requirements §2.1 的结束三通道全部接通，并把自动重连从「3 次静默放弃」改成封顶长跑。

**现状差距**（业务语言，逐点根因见 §2 各 T 的证据段）：

1. **每次结束会话都在拆了再建**。四条本地关收尾路径（录音丢弃 / KEY 单击结束 / 覆盖层关层 / 编程唤起）一律 `CloseAudioChannel()` + 降 `LOW_POWER` + 5 秒后 `ScheduleResidentRebuild()` 重建。而常驻通道本就是设计目标态（提醒推送、故事预取都跑在它上面，决策 21「引擎不主动断连」支持长连接）——断了再建纯绕路。真机代价（KPLOG029 uptime 毫秒轴，来源 = `TECH-261114` 条目 2026-11-14 15:04 三连会话轮实测）：温通道按键进聆听 **0.82s**，撞上重建窗的冷通道 **1.57s**（含 TLS 证书验证 0.53s），每次重建另付全套 bootstrap burst（story_list + points_home + home + schedule 四发 MCP + 4 个故事预取重跑，实测约 **15s** 后台流量，15:05:04–21 窗口）。
2. **闲置约 137 秒后首按键必走重连**。设备侧「通道是否打开」的判据里含一条硬编码的「120 秒无下行数据即判死」，而会话间隙设备不上行、云端不下行，判据必然到期——这是 `TECH-261012` 首按键失同步的第一段根因。
3. **结束三通道只通了一条半**。① 60 秒无语音超时：设备侧零 timer producer（`ses_machine_dispatch` 生产调用点为 0，本次实测复核见 T3 证据）；② 云端结束意图：**三重断链**——live prompt 全文没有一条规则让模型在告别时下发 `control/end_session`（告别被指向 `home` view）、front 机械门禁把 `end_session` 在 relay 与自推两条路径上都拒转、设备消费面 `case 0` 只做 `AbortSpeaking` 而不结束会话；③ 再按 KEY 手动关闭：261024 批已接通（`GLOBAL_ACT_OVERLAY_CLOSE` → `EndVoiceSession()`），是本批唯一可复用的既有统一入口。
4. **断网不自愈**。自动重连固定 3 次（退避 5s/10s）后任务退出、此后零重试；服务端重启或家庭网络瞬断超过约 15 秒即设备静默离线，离线期间 front 经 MCP 下发的提醒**全部丢失**（无重投无补发），直冲 S-F1.4 提醒必达语义。

**非目标（本批不做，防 scope 漂移）**：

- **不改引擎代码**。human 2026-11-15 落地边界未选「允许改引擎补丁」；本批全部改法在设备侧 + front 侧 + 部署面 prompt 制品 + canonical 文档内闭合，`server/deploy/engine-patch/**` 与 ECS 上 `core/**` **零 diff**（§2 实证段给出「引擎零改动即成立」的三条代码依据）。引擎 **config 层**亦不动（`close_connection_no_voice_time: 86400` + `end_prompt.enable: false` 保持，决策 21）。
- **不新立会话上下文机制**。共享短上下文已由 ADR-015（OpenClaw 服务端持久会话，按 `user=<device_id>`）承载，且**与通道生命周期正交**——通道常驻对它零影响（T4 实证）。增长控制的权威承载 = OpenClaw `session.maintenance`（live 已 enforce）+ 自动 compaction，架构账本承载位 = §8.1 风险 14（既有观察项），本批只补注记、不新建第二套机制。
- **不做会话边界注入**。不动引擎代码约束下可用的四个注入面逐一评估均为语义污染（T4 改法末段），本批显式不做，留重评触发。
- **不改需求语义**。60 秒阈值、结束三通道、录音态按键口径、记录会话不适用 60s（v1.65）全部照 requirements 现行文本实现；requirements §2.1 的 ⚠️ 实现侧对账注记刷新是**文档对账**，走 §6 留根清单交主线程路由，不在本批 canonical diff 内。
- **不动既有帧形状**。不新增 view、display 白名单仍 9 个、`kidpal_*` 上行帧字段零改动；契约面新增只有「一条下行心跳帧 + `listen` 帧语义钉行 + `end_session` 放行规则」。
- **不复活决策 21 的「方案二」**（网关设备腿常驻、引擎腿按会话起停）。该备选已随 ADR-015 承载定案失效（§8.1 风险 9 闭账句）；本批的形态是**双腿随连接同寿命、语音会话在带内起停**，其可行性恰恰建立在 ADR-015 把连续性权威移到 OpenClaw 之上（T1 理由段展开）。

---

## 1. 设计总览（三层解耦，供 §2 各 T 引用）

本批把今天揉在「WS 连接」一个概念里的三件事拆成三层，每层有自己的权威与自己的边界信号：

| 层 | 是什么 | 权威边界承载 | 生命周期 | 今天的病灶 |
|---|---|---|---|---|
| **L1 通道层** | 设备 ⇄ front ⇄ 引擎的双腿 WS 代理连接 | 连接本身（TCP/WS） | 设备开机在线期；终止源只有三个：服务端/网络侧断、引擎 24h 兜底、重启/OTA | 被语音会话结束顺手拆掉；且被设备侧「120s 无下行即判死」误杀 |
| **L2 会话层** | 一次语音会话（孩子按下 KEY / 喊唤醒词 → 告别或超时或手动关闭） | **设备状态机**（Connecting/Listening/Speaking = 在会话；Idle = 不在会话）；wire 表达 = 既有帧对 `kidpal_session_ctx` + `listen start`（开始）/ `listen stop`（结束） | 秒到分钟级 | 无独立承载——「关会话」的物理动作就是「拆通道」，四个收尾点各写一遍 |
| **L3 语境层** | front 的每会话态（记录语境累积器、引擎侧记录语境抑制标志、会话腿归口） | front 按 L2 的带内信号推导 | 与 L2 同寿命 | 绑在 L1 上（连接建立时初始化、连接断开时收尾）——L1 一常驻就失去收尾时机 |

**一句话设计**：L1 只受自己的三个终止源管；L2 的边界改用**已经在 wire 上跑着的既有帧**表达（不新增上行帧、不新增契约语义污染）；L3 的初始化与收尾改由 L2 的带内信号驱动，连接级 teardown 退为兜底而非主路径。

**为什么复用 `listen stop` 而不新增 `kidpal_session_end`**（这是本批最重的一个取舍，四点理由）：

1. **它已经是事实上的会话结束信号，且已被两侧消费**。设备侧 `HandleStopListeningEvent` 已在发它（记录会话 server 驱动收尾、查字窗关闭两条路径）；front 侧 `pump_client_to_upstream` 已旁路消费它（`_story_stop_on_listen_stop`，转发不变、决策 23 代理语义）；引擎侧 engine-patch 的 `_kidpal_on_listen_message` 已把它当会话闭锁——`listen stop`（非 manual 且非记录语境）置 `_kidpal_listen_closed = True` 并清空在途 ASR 缓冲，`listen start` 复位 False，`chat()` 的 depth==0 首守卫据此丢弃收尾后在途的 ASR 轮。**引擎侧「会话已结束」的承载已经存在，零改动即得。**
2. **新增帧要付三份成本而收益为零**：契约 §四新增一条端→网关私有消息（契约面扩张）+ front 新增一个消费分支（与既有 `listen stop` 分支语义重叠、必然产生「两个会话结束信号谁优先」的新歧义）+ 设备新增一个发送原语（`protocol.cc` 的 `SendKidpal*` 家族已有五个，第六个只是复制形态）。而 `listen stop` 是唯一一个**引擎也会消费**的候选——新私有帧引擎不认，抑制链就得靠 front 再注入一次等价信号，绕回 ADR-013 已经付过的成本。
3. **语义污染风险可控且可钉死**。`listen stop` 在记录语境下已经带第二层含义（「KEY 收尾 → 归档」）。本批不新增第三层含义，只把它从「记录会话专用收尾」提升为「所有语音会话的通用结束信号」；两层含义的分辨权在 front 的既有判据手上（`acc.routing(session_id) == "accumulate"` 才走归档），问答会话恒不命中。这条判据必须作为冻结验收断言钉死（见 §5 与 `package-split.md` 的 acceptance 方向）。
4. **契约面本来就该补这一节**。§四 今天钉了 hello / Opus / `stt`/`tts` / `abort` / 五个 `kidpal_*` 私有帧 / `mcp` / `llm`/`system`，**唯独没有 `listen` 帧**——而它是设备上行最频繁的控制帧族，且已经承载了记录会话归档这种承重语义。契约缺这一节本身就是债；本批补上是还债，不是扩面。

---

## 2. 问题、根因与改法（T1–T7 + 契约包）

> **起草期实证留痕（2026-11-15/16 本会话逐点现核，命令与关键输出见文末「起草期实跑记录」）**：
> - **设备侧**：`application.cc` 全文相关区逐段 Read——四条会话收尾路径与其 `local_close_settle` src 标记（`src=discard_record` :985 / `src=toggle_chat` :1037 / `src=overlay_close` :1167 / `src=wake_invoke` :1521，`grep -n "local_close_settle corr_id"` 四命中）；`ScheduleResidentRebuild()` 调用点四处（:989/:1040/:1169/:1524）+ 定义 :619；`TryStartAutoConnect` :572-617（守卫三条件 :587-591、`for (int attempt = 0; attempt < 3; attempt++)` :599、`kAutoConnectBackoffS[3] = { 5, 10, 20 }` :105、`kResidentRebuildDelayS = 5` :112）；`protocol_->CloseAudioChannel()` **非 test 面全仓 9 命中**（`grep -rn 'protocol_->CloseAudioChannel' firmware --include=*.cc --include=*.h | grep -v /test/ | wc -l` → `9`，九处全在 `application.cc`）= **会话收尾 4**（:976 在 `Application::CloseAudioChannel` 体内即录音丢弃面 / :1032 toggle_chat / :1164 overlay_close / :1515 wake_invoke）+ **真拆通道 5**（`HandleNetworkDisconnectedEvent` :508 / `Reboot` :1407 / `UpgradeFirmware` :1426 / `SetAecMode` :1588 / `ResetProtocol` :1601）。**「会话收尾 4 + 真拆通道 5 = 9」是本批的正负锚分界**：前四者随本批改为发 `listen stop`（通道保持），后五者逐字保留（`local_close_pending_` 分类机制不动）；1s 主循环 tick = `esp_timer` 周期驱动（:193-203）+ `MAIN_EVENT_CLOCK_TICK` 消费区（:429-466）+ `clock_ticks_` 在 `HandleStateChangedEvent` :1235 归零；Idle 分支既有清位链（:1244-1299：`EnableVoiceProcessing(false)` / `EnableWakeWordDetection(true)` / [R2] `kidpal_story_session_end` / `KidpalStorySessionEndSync` / keybar 场景复位（REMINDER 在顶豁免）/ `kidpal_render_node_voice_end`（OQ3=c）/ `kidpal_story_session_ctx_set(0)` / `RaiseReminderQueue`）；Listening 分支 `SendKidpalSessionCtx` :1345 与 `SendStartListening` :1351（`if (!IsAudioProcessorRunning())` 门内）；`mcp_server.cc:1719-1782` control 路由四 case（**case 0 end_session = `app.AbortSpeaking(kAbortReasonNone)` 而已，不结束会话、不回页**）。
> - **协议侧**：`websocket_protocol.cc` `IsAudioChannelOpened()` :77-79（四条件合取，含 `!IsTimeout()`）、`CloseAudioChannel()` :81-86（`local_close_pending_ = true` + `websocket_.reset()`）、`OnDisconnected` :215-227（本地关分类消费后 return，不触发关闭回调）、`last_incoming_time_` **唯一**写点 = `OnData` 回调末行 :212；`protocol.cc:149-158` `IsTimeout()` 的 `const int kTimeoutSeconds = 120`（硬编码、每次调用命中即打一条 `ESP_LOGE`）。**`listen` 帧族三值的发送原语逐条现核（R1-F1 修订补入）**：`protocol.cc:74-86` `SendStartListening(mode)` 拼 `{"session_id":...,"type":"listen","state":"start","mode":"realtime|auto|manual"}`；`:88-91` `SendStopListening()` 拼 `{...,"state":"stop"}`；`:68-72` `SendWakeWordDetected(wake_word)` 拼 `{...,"state":"detect","text":"<唤醒词>"}`——**∴ `detect` 是本板确发的帧，不是预留值**；其调用点 `application.cc:1217`（`HandleWakeWordDetectedEvent` 内，`#if CONFIG_SEND_WAKE_WORD_DATA`）与 `:1500`（`WakeWordInvoke` 内，`#if CONFIG_USE_AFE_WAKE_WORD || CONFIG_USE_CUSTOM_WAKE_WORD`），`firmware/sdkconfig:733` = `CONFIG_SEND_WAKE_WORD_DATA=y`、`:731` = `CONFIG_USE_AFE_WAKE_WORD=y`（`:732` `CONFIG_USE_CUSTOM_WAKE_WORD is not set`）→ 两条调用点均编译在位、:1217 为本板活路径。
> - **组件侧（只读核实，不改）**：`managed_components/78__esp-ml307/include/web_socket.h:27` 有 `void Ping()`（返回 void、不可观测失败）；`src/web_socket.cc:253` `Ping()` = `SendControlFrame(0x9, ...)`；`OnTcpData` 的 `case 0x9`（收到 Ping）另起线程回 Pong，`case 0xA`（收到 Pong）**空实现直接 break**——∴ 无论设备主动 Ping 还是被动收 Ping，都**不会**触达 `on_data_`，即不刷新 `last_incoming_time_`。这条实证否决了「靠 WS ping/pong 保活」的一切变体（详见 T1 备选评估）。
> - **front 侧**：`front_app.py` `ws_gateway` :2446-2573（每连接 `session_id = f"sess-{uuid.uuid4().hex}"` :2462、连接期一次性六件 = story_list restore 推 :2486-2490 / points_home 推 :2497-2500 / alert 腿登记 :2508 / 会话腿登记 :2511 / schedule 摘要注入 :2518-2521 / schedule 推 :2526-2529，teardown 三件 :2558-2570 = 腿双摘除 + `acc.cancel(session_id, reason="ws_disconnect")`）；`_consume_kidpal_control` :992-1177（ctx(story) 进语境沿 :1035-1053、ctx(points 两值) 摘要注入 :1054-1065、ctx(home/schedule/story_cover) **零动作**、未知 source_page 拒绝记日志 :1066-1070、discard 连接权威取消 :1130-1142）；`pump_client_to_upstream` :1782-1840（私有帧整条拦截 :1817-1819、其余原样转发 :1820、`listen stop` 旁路消费 :1826-1828）；`_push_display_payload` :597-610（`self_push=True`）；`_release_record_context` :1673-1684 与三个调用点 :1716/:1727/:1747（全在 `_story_finish_session` 内）；`_register_session_legs`/`_unregister_session_legs` :2330-2355。
> - **门禁侧**：`display_schema.py` `GATEWAY_DECISION_CONTROL_ACTIONS = frozenset({"end_session","start_organizing","add_points","redeem_points"})` :56-57、`FORWARDABLE_CONTROL_ACTIONS = frozenset({"stop_speaking","start_recording"})` :60、`validate_display(payload, *, self_push=False)` :128 的分叉 :156-163（self_push 豁免口**只有** `start_organizing`；:139 与 :159 两处明文「end_session 即便自推仍拒」）。
> - **引擎侧（ECS 实读，零改动）**：`/opt/kidpal/xiaozhi-esp32-server/main/xiaozhi-server/core/handle/textHandler/listenMessageHandler.py`（`state=="start"` → `conn.reset_audio_states()`；`state=="stop"` → 只置 `client_voice_stop=True` + 非流式 ASR 触发缓冲尾音识别，**不置 `close_after_chat`、不关连接**）；`core/handle/receiveAudioHandle.py`（`no_voice_close_connect` 是 `last_activity_time` 的唯一刷新点，且**只在收到音频帧时被调**；`close_after_chat` 只由它与 `max_out_size` 置位）；`core/handle/sendAudioHandle.py:53-55`（`close_after_chat` 为真时在 LAST 句后 `await conn.close()`）；`core/utils/dialogue.py`（`get_llm_dialogue_with_memory` **全文无任何裁剪/压缩/窗口逻辑**——system + 全部 few-shot + 全部实际历史逐条装配）。仓内 `server/deploy/engine-patch/connection.py` 与 `openai.py` 逐段 Read：`_kidpal_turn_messages` :144-175（装配产物切「会话级前缀 + 本回合尾部」，**先前实际历史一律不进返回值**，docstring 明写「对话历史唯一权威 = OpenClaw 服务端持久会话」）、`_kidpal_on_listen_message` :676-717（`listen stop` 闭锁 + `listen start` 复位）、`chat()` depth==0 三守卫 :1555-1583、`_route_message` :748-803（四个内部帧认领点：`kidpal_schedule_summary` / `kidpal_points_context` / `kidpal_speak` / `kidpal_record_context`，前两者在绑定门之前认领）、`_check_timeout` :2287-2316（10s 轮询、`timeout_seconds = close_connection_no_voice_time + 60`）、`_save_and_close` :452-500（`memory.save_memory(self.dialogue.dialogue, self.session_id)` 只在连接关闭时发生）、`last_activity_time` 全仓 5 命中（:325 初始化 / :408 建连 / :2291-2298 消费，**connection.py 内零刷新**）。
> - **prompt 侧（ECS live 实读）**：`/root/.openclaw/workspace-kidpal/AGENTS.md` `grep -n 'end_session\|control\|拜拜\|再见\|home'` → view 白名单表 :31 `| home | 孩子说再见/结束、无需显示内容 | 空 |`、:32 `| control | 下发控制动作 | {"action":"end_session"|"stop_speaking"|"start_recording"} |`；仓内镜像 `server/deploy/openclaw-system-prompt.v11.md` 同形（全文 81 行逐行 Read 确认：**零条规则指示模型在告别回合下发 `control/end_session`**，「你要记住的硬约束」节 8 条亦无一提及）。
> - **验收齿面（机械枚举，`grep -c` 实跑）**：`boot_auto_connect.acceptance.c` = `ScheduleResidentRebuild` 31 / `kResidentRebuildDelayS` 19 / `kAutoConnectBackoffS` 20 / `CloseAudioChannel` 24 命中；`voice_round_ui.acceptance.c` = `local_close_settle` 4 / `ScheduleResidentRebuild` 3 / `CloseAudioChannel` 19；`record_session_live.acceptance.c` = `CloseAudioChannel` 22。server 侧 live 钉 `end_session` 的两件：`test_points_e2e_spike.acceptance.py:866-868`（缺省 `self_push=False`，即 relay 路径拒转——**本批保持 relay 恒拒，此齿天然保绿**）、`test_record_dialogue_guide.acceptance.py:1267-1272`（显式 `self_push=True` 断言 `assertFalse`——**本批按新裁定反转此齿，显式申报、非静默改齿**）。
> - **canonical 文档面（退役机制引用点普查，R1-F2 修订补入）**：`grep -n '常驻重建' docs/architecture.md` → **4 命中** = `:138`（§3.3 假设清单「设备常驻在线」行，原文含「重连 + 常驻重建已落（261010 包）」）/ `:371`（§6.4，原文含「会话收尾本地关后 T=5s 常驻重建」）/ `:460`（§8.1 风险 3 闭账句）/ `:529`（§10 决策 29 正文，原文含「会话收尾本地关后短延迟（T=5s，kResidentRebuildDelayS，已冻结进齿）常驻重建」）；全 `docs/` 树 `grep -rn '常驻重建' docs/` 另有 debt 账本面 6 命中（生成件 `INDEX.md` 1 + `TECH-261012` 2 + `TECH-261114` 2 + `_archive/2026/TECH-261024-*` 1），按 append-only 纪律只结项不改写；`docs/requirements.md` / `docs/contracts/interface-contract.md` / `docs/decisions/ADR.md` 命中各 = **0**（实测）。取代注记形态先例现核 = `:520`（决策 21 的「⚠️ **2026-10-22 取代注记（部分）**」）+ `:549`（§12 行记载「§10 决策 25 取代注记」），`grep -c '取代注记' docs/architecture.md` = **4**（:230 / :520 / :547 / :549）。
> - **`ses_machine_dispatch` 生产调用点复核**：`grep -rn "ses_machine_dispatch" firmware/main --include=*.c --include=*.cc --include=*.h | grep -v "/test/" | wc -l` → **2**（= `session_state_machine.cc` 定义 + `session_state_machine.h` 声明自身）；`grep -rln` 全命中集 = 该 .cc/.h + `test/acceptance/archive/session_logic.acceptance.c` + `test/acceptance/archive/story_p3.acceptance.c`。∴ 生产代码零调用的既有登记**复核为真**，本批维持不接生产（T3）。

### T1 · 架构 §6.4 整节重写：通道生命周期与会话生命周期解耦 + 常驻保活 + 自愈巡检 + 长重试

**问题**：§6.4「断线重连与常驻连接」今天的正文只有四句，承载的模型是「设备开机即连、断线自动重连；会话收尾本地关后 T=5s 常驻重建」。这个模型把 L1（通道）和 L2（会话）绑成了一件事——「收尾」的物理动作就是「拆通道」，于是每次会话结束都要付一次冷启动税 + 一次 bootstrap burst；同时「常驻」是**假的常驻**：设备侧 `IsTimeout()` 的 120s 无下行判据会在任何一段闲置后把活通道判死，自动重连又只跑 3 次就静默退出。结果是「常驻在线」这个决策 29 的裁定在实现层被三处各自削弱，而 §6.4 一个字都没记这三处的边界。

**证据**：`docs/architecture.md:371`（§6.4 全文，现核）；`docs/architecture.md:460`（§8.1 风险 3「已闭账」句：「服务端/网络侧断自动重连 + 会话收尾常驻重建随 261010 包根治，离线驻留窗口消除」——本批把「会话收尾常驻重建」这一半退役，闭账理由必须随之改写，否则账实不符）；`firmware/main/protocols/protocol.cc:149-158`（`kTimeoutSeconds = 120`）；`firmware/main/protocols/websocket_protocol.cc:77-79`（`IsAudioChannelOpened()` 把 `!IsTimeout()` 并入合取）；`firmware/main/application.cc:599`（`attempt < 3`）；`docs/debt/tech/TECH-261012-first-keypress-no-listening.md`（真机实证：按键撞 781s 空闲通道超时 → 重建后会话失同步）。

**根因**：三条独立的机制各自都「局部合理」，但没有任何一节文档规定它们的**合成边界**——① `IsTimeout()` 的 120s 是上游小智为「按会话建连」设计的死链探测阈值，在常驻模型下它探测的对象已经不存在（会话间隙本来就该没有下行数据）；② 3 次退避是 K6 批为「boot 期建连」设计的预算，被复用成了唯一的长时重连路径；③ 5s 常驻重建是 261010 批为「本地关后恢复常驻」打的补丁，它治的是症状（通道被拆了）而不是病因（通道不该被拆）。三者叠加 = 「常驻在线」名义在世、实际每 2 分钟到 15 秒不等就会掉一次。

**改法**：§6.4 整节重写为四段，把三层解耦模型（§1）作为该节的骨架，并钉死以下四条机制本体：

1. **通道终止源闭集**（三条，穷尽）：服务端/网络侧断（`OnDisconnected` 非本地关分类 → A 路径：Idle + `TryStartAutoConnect`，261010 既有语义零改动）；引擎 24h 兜底（决策 21 的 `close_connection_no_voice_time: 86400` + 二道 `+60`，实测形态 = 设备 Idle 期不上行音频 → 引擎 `last_activity_time` 只在收到音频帧时刷新 → `_check_timeout` 10s 轮询到点关连接 → 设备识别为服务端侧断走 A 路径）；重启 / OTA / AEC 模式切换 / `ResetProtocol`（四个真拆通道点，`local_close_pending_` 分类机制保留）。**语音会话结束不在闭集内**——这是本批的核心裁定。
2. **常驻保活 = front 周期下行心跳帧**（契约 §四新增 `kidpal_keepalive`，见 T4）。设备侧 `IsTimeout()` 的**阈值与逻辑零改动**（仍 120s、仍只看 `last_incoming_time_`、仍只在 `OnData` 刷新），变的是它的**语义**：从「120s 无下行数据 = 死链」变成「120s 无心跳 = 死链」，即从伪判据升级为真端到端活性判据。心跳间隔 30s（可部署面 env 调），∴ 活连接恒有 4 倍余量；死连接在 120–150s 内被判出。
   - **备选评估（四种，逐一否决，理由留档防回摆）**：
     - **(a) 设备侧把发送活动也计入活性**（`last_incoming_time_` 扩为 send/recv 双写）——**否决**：会话间隙设备零上行，扩了也不刷新；且「自己发出去就算活」把判据从端到端降级为本地，死链探测能力归零。
     - **(b) 设备周期 `WebSocket::Ping()`**——**否决**：实证 `web_socket.cc` 的 `case 0xA`（收到 Pong）空实现直接 break、`Ping()` 返回 void，∴ 主动 Ping 既不刷新 `last_incoming_time_`、也无法观测失败；要靠它探活必须改 `managed_components/` 第三方组件（重下载即丢，且不在自有代码面）。**唯一残余价值** = Ping 触发的 TCP 重传超时能让 lwip 最终 abort 死连接（分钟级），可作为 (c) 的补充手段写进实现包，但不作为架构承载。
     - **(c) 调大或摘掉设备侧 120s 阈值**——**否决为独立方案**：摘掉后 `IsAudioChannelOpened()` 只剩 `IsConnected() && !error_occurred_`，半开连接（家庭 NAT 静默过期）要等到下一次 `SendText` 失败才暴露，而 Listening 分支的 `SendStartListening` **不检查返回值**（:1351）→ 孩子对着死通道讲完一整段，比今天的 1.57s 冷启动严重得多。
     - **(d) front 心跳（推荐，即本改法）**：下行数据同时证明「front 活着 + TLS 通路活着 + 设备接收链活着」，且复用既有 `OnData` → `last_incoming_time_` 路径，设备侧只需一个显式 no-op 消费分支（否则 `application.cc:924-926` 的 `ESP_LOGW("Unknown message type")` 会每 30s 冲一条 WARN 进 SD 日志 16MB 轮转预算）。代价 = 契约面 additive 一帧 + 约 115 KB/日下行流量（40 字节 × 2880 次）。
3. **设备侧自愈巡检**：主循环 1s tick（既有 `MAIN_EVENT_CLOCK_TICK` 消费区）新增一条判据——`state == Idle && protocol_ && !protocol_->IsAudioChannelOpened() && !auto_connect_in_flight_` → `TryStartAutoConnect()`。意义 = 把「通道不可用」的发现时机从「孩子按键那一刻」提前到「心跳断了之后的第一个 tick」，孩子再也撞不上冷通道；同时它是 (d) 方案的**必要配套**（没有巡检，判死仍然只在按键时发生，(c) 的病症照旧）。守卫三条件与「零第二条重连路径」不变量保持（巡检只调既有 `TryStartAutoConnect`，不自建 task、不自己 `OpenAudioChannel`）。实现注记：`IsTimeout()` 命中即打 `ESP_LOGE`，巡检每秒调它会刷日志 → 实现包必须把该日志改为**跳变沿单次**（或降频），此为验收断言之一。
4. **自动重连 = 封顶递增 + 稳态长跑 + 人工意向不丢**：退避表从 `{5,10,20}` + 3 次即 exit 改为 `{5,10,20,40,60}` 后恒 60s 长跑（命名常量单源，冻结验收 K6-A2 的「`[3]` 长度 + `{5,10,20}`」形态按新裁定重写并显式申报）。**承重配套** = 现状 `auto_connect_in_flight_` 在整段 task 生命周期（含退避睡眠）为 true，而四个人工入口（`HandleToggleChatEvent` :1015 / `HandleStartListeningEvent` :1068 / `HandleEndVoiceSessionEvent` :1145 / `HandleWakeWordDetectedEvent` :1182）见它即 no-op → 长跑重试下离线期孩子的按键与唤醒**永久无响应**（比今天 3 次即止更糟，因为今天 15 秒后标志会清）。改法 = 新增人工意向位：入口见 in-flight 时不再静默 no-op 而是置意向位并留一条 INFO 审计行；AutoConnect task 每段退避前查该位，命中即把剩余退避截为 0 立刻重试；连接成功后经 `Schedule` 回主循环按入口来源补投对应事件位（TOGGLE_CHAT / START_LISTENING），孩子那一次动作不丢。**功耗取舍**：离线期每 60s 一次 TLS 握手尝试（冷通道 TLS 证书验证段实测 0.53s，KPLOG029），跑在后台 task 内不阻塞主循环；WiFi 已在世（本路径由网络事件驱动）。真机轮量一轮离线 30 分钟电量差，数据回填 §8.1 新增风险行。

**为什么这样改（对哪个根因）**：第 1 条对根因③（把「重建」这个补丁的前提直接取消）；第 2、3 条对根因①（把伪判据换成真判据，并补上发现时机）；第 4 条对根因②（把 boot 期预算换成常驻期策略），其中「人工意向位」是本批**新识别**的风险——长重试若不配套，会把 `TECH-261012`「按了没反应」从偶发放大为离线期恒定，属必须同批闭环的对偶面。

**联动**：§8.1 风险 3 闭账理由改写 + 新增「长重试功耗 / 心跳流量 / 引擎 24h 日切」风险行（T3）；决策 21「结构级备选（方案二）」的失效注记保持不动，本批形态与方案二的区别写进 ADR-019 备选段（T6）。

目标文件：`docs/architecture.md`（改）

### T2 · 架构 §5.1.1 / §5.1.2 / §5.2：统一会话结束处理器 + front 每会话态改由带内信号驱动 + 结束三通道接线

**问题**：三处文档今天都把「会话」当作「连接」的同义词来写，导致实现层没有单一收口点：① §5.1.1 设备端组件表里没有任何一行承载「语音会话边界」这个概念（会话状态机行写的是那个**生产零调用**的纯模型，设备状态机行写的是小智原生态机）；② §5.1.2 front 行的故事面/记录语境描述全部以「WS 建连」「断开钩子」为生命周期锚；③ §5.2 场景①②④ 的端到端叙述里，「会话结束」这一步是空白的——场景② 只写到「播报中 KEY 单击 = 停播 + abort」，场景④ 的取消链以「设备于关闭音频通道前同连接先行发送 `kidpal_story_discard`」为轴心，两者都默认了「结束 = 断连」。

**证据**：`docs/architecture.md:176`（§5.1.1 会话状态机行，含 ⚠️ 修复项）、`:177`（设备状态机行）、`:197`（§5.1.2 front 行全文，「WS 建连恢复推送」「断开钩子」等锚点）、`:222`（场景①）、`:224`（场景②）、`:230`（场景④，取消链句）；`firmware/main/application.cc` 四条收尾路径各自重复「`CloseAudioChannel` + `SetDeviceState(Idle)` + `LOW_POWER` + `local_close_settle` 审计行 + `ScheduleResidentRebuild`」五件套（:972-990 / :1031-1041 / :1141-1170 / :1508-1526）；`server/gateway/front/front_app.py:2558-2570`（teardown 三件全绑连接断开）。

**根因**：261010 批引入「常驻重建」时，是在**不动**「结束 = 断连」这个前提的情况下打的补丁，所以文档层从未被迫把「会话」独立成一个有自己边界信号的概念；261021 批补 `kidpal_story_discard` 时，取消链的时序保证也只能借「断连」这个物理事件来表达（契约原文「设备关闭音频通道（断连）之前同连接先行发送」）。一旦 L1 常驻，这两处表述同时失去物理承载。

**改法**：

1. **§5.1.1 新增一行「语音会话生命周期收口」**，钉死：会话边界权威 = 设备状态机（Connecting/Listening/Speaking = 在会话，Idle = 不在会话）；**六个触发源统一收口到单一处理器**（`HandleEndVoiceSessionEvent`）——四个本地关收尾点（录音丢弃面 / KEY 单击结束 / 覆盖层关层 / 编程唤起）+ 云端 `end_session`（`mcp_server` control case 0）+ 60s 无语音超时（主循环 tick producer）。处理器动作序 = ①（仅 Speaking）`ResetDecoder` 即时静音 + `AbortSpeaking`（261024 既有两步零改动）② **`SendStopListening()` 取代 `CloseAudioChannel()`**（L2 结束的带内信号）③ `SetDeviceState(Idle)`（Idle 分支既有清位链**全保留**，见下「不变量」）④ 省电剖面**维持 PERFORMANCE**（不再降 `LOW_POWER`——通道仍在世，降档会把 WS 收发拖进省电窗口；决策 29「永不休眠」与 RC-B6 观测项照旧）⑤ **不再 `ScheduleResidentRebuild()`**（无常关即无需重建；该 helper 与 `kResidentRebuildDelayS` 常量随四个调用点一并退役）。同时钉死「会话开始」侧零改动：`kidpal_session_ctx` 仍在 Listening 分支首帧音频前发（契约 §四「wss 常驻连接复用时同样发送，重复发送安全」今天就是为常驻写的，本批第一次真正兑现它），`listen start` 仍在 `if (!IsAudioProcessorRunning())` 门内发。
   - **录音丢弃面的时序保证（承重）**：`kidpal_story_discard` 与 `listen stop` 在**同一条 TCP 流上有序**，discard 先出。front 侧 `acc.cancel()` 实证会 `self._sessions.pop(session_id)`（`story.py:886-894`）→ 随后到达的 `listen stop` 经 `_story_stop_on_listen_stop` 查 `acc.routing(session_id)` 得 `"chat"`（`story.py:654-656`：会话态已 pop 即返 `"chat"`）→ **不进归档分支**。「丢弃即丢弃、不被 listen stop 复活成归档」由这条有序性承载，必须作为冻结验收断言 + 真机验证项双钉。
   - **不变量（Idle 分支既有清位链逐条保绿）**：`EnableVoiceProcessing(false)`（麦克风口径不变——AFE gating 由设备态驱动、与 WS 无关，这正是候选 A「麦克风口径不变」的机制依据）/ `EnableWakeWordDetection(true)` / [R2] `kidpal_story_session_end` 页态复位 + 录音演出定时器停 / `KidpalStorySessionEndSync` 镜像同步（次序承重：在 [R2] 之后、keybar 复位与 voice_end 之前）/ keybar 场景复位（REMINDER 在顶时豁免）/ `kidpal_render_node_voice_end`（OQ3=c 回答页驻留）/ `kidpal_story_session_ctx_set(0)`（261028 记录语境清位）/ `RaiseReminderQueue()`（S-F1.4 录音态提醒排队延后、会话收尾立刻弹）。
2. **§5.1.2 front 行改写生命周期锚**：连接期一次性六件（story_list restore 推 / points_home 推 / alert 腿登记 / 会话腿登记 / schedule 摘要注入 / schedule 推）**保持绑连接**——它们是恢复性 bootstrap，每连接一次正是本批要的收益（bootstrap burst 从「每次会话结束再建时付」降为「每连接一次」）；teardown 三件（腿双摘除 + `acc.cancel(reason="ws_disconnect")`）**降为最外层兜底**（引擎 24h 日切 / 网络断 / front 重启三形态仍走它，逐字保留）。新增「每语音会话收尾」职责，触发源 = 旁路消费的 `listen stop`（既有缝，`pump_client_to_upstream` 内、转发不变），但**动作按累积器路由态分层，不在 stop 当刻无条件取消**（v1.2 修订，门 1 F-01；分层理由见下「F-01 修订依据」）：

   | `listen stop` 当刻的 `acc.routing(sid)` | 会话收尾动作 | 记录语境释放时点 |
   |---|---|---|
   | `chat`（问答会话，从未进记录语境） | 无动作（既有归档/取消路径都不命中） | 不适用（本连接记录语境标记为假 → **不注入任何帧**，避免每轮问答都往引擎腿打一帧无用信号） |
   | `accumulate` 且 accum 非空 | `_story_stop_on_listen_stop` 返 True → 既有三式收尾单源 `_story_finish_session`（归档）接管，**收尾三件不得再动该会话** | 整理回合**完成后**，由 `_story_finish_session` 既有三个 `_release_record_context` 调用点覆盖（:1716/:1727/:1747；261022 真机轮修订语义零改动——提前释放会让收尾后迟到的幻听 STT 被当普通对话派发，KPLOG006 缺陷本体） |
   | `accumulate` 且 accum **空** → 经 `mark_pending` 落 `pending_stop` | **禁止 cancel、禁止释放**（protected state）；交既有 10s 巡检 | **≤10s**（`PENDING_STOP_SECONDS`，`story.py:75`）——`_story_timeout_sweep_body:1255-1257` 的 pending 分支**必须补 `_release_record_context(sid, _session_legs(sid) or list(_alert_legs))`**（今天只 `release_pending` + 日志、**不释放引擎抑制**，旧模型靠随后的断连兜底，常驻后兜底消失 → 这是本批 front 侧唯一新增的释放点，也是 F-01 修订的承重半）；若转写在 10s 内到达 → `_story_feed_from_stt:562-564` 接通恢复归档 → 走上一行的既有释放点，**不经 sweep** |
   | `pending_stop`（重复 stop，SAO-A3 形态） | 同上，`mark_pending` 幂等不重置挂起时刻 | 同上（≤10s sweep） |
   | `accumulate` 且本帧未被归档路径消费（**结构上不可达**——`_story_stop_on_listen_stop` 的分支已穷尽：非空→True、空→mark_pending） | 防御性处置 = **不 cancel**，留既有卡壳引导 / 自动收尾 sweep（2 轮引导无回应 → **归档、不丢弃**，S-F2.2 AC5） | 由该 sweep 的 `_story_auto_finish` → `_story_finish_session` 既有释放点覆盖 |

   收尾三件随之重述为：**① 分层释放**（按上表，问答会话 no-op、归档路径复用既有释放点、`pending_stop` 交 10s sweep 且该 sweep 补释放调用）**② 不在 stop 当刻取消任何累积态**（原「残余累积态取消」动作**撤销**）**③ 审计一行**（含冻结 token，如 `session_end_boundary`，记 routing 分支与是否触发释放）。
   - **F-01 修订依据（为什么必须撤销「stop 当刻 cancel」）**：`pending_stop` 与「引导态无内容」在 listen-stop 那一刻的**可观状态完全相同**（accum 空 + 已/即将 pending），任何「本帧未被归档路径消费」式守卫都无法区分二者；而 manual 形态 ASR 由 `listen stop` 触发、转写约 **1.7s** 后才到（`front_app.py:577` docstring 实录），当刻 cancel = `story.py:886-894` 的 `_sessions.pop` → routing 回 `chat` → **在途转写不可恢复**、录音态 KEY 收尾（三式收尾之一）的归档丢失，直接违反 `docs/requirements.md` S-F2.2 AC4 v1.65「显式收尾 → 一次性忠实整理成篇 → 归档 → 盖章」。断言级冲突三条（起草期逐条实读复核）：`test_m4k1_story_cloud_link.acceptance.py:1891` `test_sao_a2_empty_stop_holds_pending_not_bail_to_chat`（`assertNotEqual(acc.routing(sid), "chat")`）、同文件 `:849-882` `test_a2_session_ctx_intercepted_not_forwarded`（M4K1-A2，真机序 ctx(story)→stop→stt 断言 `len(files) == 1`「恰一篇归档」）、`test_sao_a3_repeated_stop_no_double_archive`（stop→stop→stt 恰一篇）。三条全是 must-stay-green 冻结 oracle，不可反转。
   - **为什么释放记录语境仍是最重单点**：引擎侧 `_kidpal_record_context` 为真时 `chat()` 在 depth==0 第三守卫直接 return（`connection.py:1580-1583`），**零 LLM 零 TTS**。今天这个标志由「连接断开」兜底清除（连接死了标志随之消失）；L1 常驻后兜底消失，front 漏发 `active=false` = 引擎对该连接**永久抑制** = 设备哑火、孩子说什么都没反应，症状与 261028「记录会话镜像失同步卡死」同族。∴ 释放点必须**穷尽枚举**且各有独立验收断言：归档路径三个既有调用点 + **sweep pending 分支新增一处** + ctx 边沿兜底（下条）+ 连接级 teardown（外层）。
   - **`kidpal_session_ctx` 边沿完整化（兜底层，v1.2 修订为分态处置）**：ctx(story) 进语境（既有）；**ctx(非 story) 且本连接记录语境标记为真**时按 routing 分态——`pending_stop` → **不释放、不取消**（保护在途转写，交 10s sweep；只记一条 INFO 说明延迟释放与窗口上界），`accumulate` → `acc.cancel(reason="session_end_stale")` + 释放（这是「上一会话未发 `listen stop` 就消失」的串味兜底，语义 = 常驻时代的 cancel-on-disconnect 同族，沿 human 2026-10-21「cancel-on-disconnect 维持为兜底」裁定口径，**不发明「弃录自动归档」这类新需求语义**），`chat` 但标记仍为真 → 只释放（防御性：说明 routing 已被 sweep/finish 清而释放漏了）。**该兜底只在设备未发 `listen stop`（崩溃/异常）时可达**——所有正常结束路径都发 stop。
   - **v1.2 显式申报的残余（accepted residual，非缺陷）**：记录会话以「accum 空」形态结束后 ≤10s 内，引擎侧记录语境抑制仍在世；若孩子在这 ≤10s 内立刻从非 story 页发起新问答会话并马上开口，该次话语可能被抑制（`listen start` 只清 `_kidpal_listen_closed`、不清 `_kidpal_record_context`）。**不得用「提前释放」消除它**——提前释放正是 261022 修掉的孤儿幻听缺陷（`_issue_finish_downlink:1661-1665` docstring 实录 KPLOG006 23:59:30「Thank you」被当普通对话派发并播报「我没听清/再说一遍」）。窗口上界 = 既有冻结常量 `PENDING_STOP_SECONDS = 10.0`，触发需三件同时成立（空 accum 结束 + 10s 内换会话 + 立刻开口），后果 = 一次话语被抑制、再按 KEY 即恢复。登记 §6 留根 + §8.1 新增风险行，真机轮 A 的 A5 实测窗口。
3. **§5.2 场景②补「会话结束」一段**：三通道（60s 超时 / 云端 `end_session` / 再按 KEY）→ 统一处理器 → `listen stop` → front 每会话收尾（**按累积器路由态分层，`pending_stop` 交 10s 巡检**）→ 设备 Idle 回原页（OQ3=c）→ **通道保持常驻**。场景④取消链句改写：`kidpal_story_discard` 的发送时机从「设备关闭音频通道（断连）之前」改为「设备发出会话结束信号（`listen stop`）之前」（契约同步改，T4）；断连隐式取消（cancel-on-disconnect）维持为最外层兜底通道（human 2026-10-21 裁维持不变），并补一句其常驻时代的同族兜底 = ctx 边沿对残余 `accumulate` 的 `session_end_stale` 取消。场景①补一句：WS 建连恢复推送的触发频率从「每次会话收尾重建」降为「每连接一次」。
4. **§5.2 场景② / §5.1.2 补云端结束意图链**（三段补链的架构表述）：模型告别回合产 `control/end_session` 薄意图（prompt 静态纪律，§5.5.5 承载不变）→ front 从引擎 display 流**截获**（与 `add_points`/`redeem_points` 同型，不透传）→ front 以机械自推单源恰一帧下发设备（与 `start_organizing` 同型豁免）→ 设备 `mcp_server` control case 0 调 `EndVoiceSession()` 收口到统一处理器。relay 直转路径**恒拒**（防伪造/注入，`test_points_e2e_spike` 与 `test_record_dialogue_guide` 的 relay 侧断言天然保绿）。
5. **`TECH-261012` 首按键失同步在本批消解，不另立包**：其根因链三段——① 闲置 ~781s/137s 后 `IsTimeout()` 判死（T1 改法 2 心跳根治）② 重建后 front 把新连接当全新会话、推恢复包、无进行中会话语境（T2 改法 2 把每会话态从连接解耦，恢复包只在真建连时推）③ 音频通道打开后同毫秒被关 / 通道根本未开（261014 批已修的 UI 面 + 本批 L2 收口）。∴ 该条目按 consumed-by 本 changeId 结项；其「修法方向 B/C」（按键等待重建完成后自动接续）由 T1 改法 3 的自愈巡检 + 改法 4 的人工意向位以更强形态承载（不是「按键等重建」，而是「让孩子撞不上重建」+「撞上了也不丢那一次按键」）。

目标文件：`docs/architecture.md`（改）

### T3 · 架构 §8.1 风险表 + §5.1.1 会话状态机行 ⚠️ 对账 + §3.3 假设清单行 + §10 决策 29 取代注记：账实同步（四处引用「常驻重建」全数对账）

**问题**：本批退役两个既有机制（`ScheduleResidentRebuild` / 3 次退避即止）、新增三个成本面（心跳流量、离线期长重试功耗、引擎 24h 日切成为唯一重建窗口），并让 60s 超时第一次真正接线——这些全部要落进架构的风险与对账账本。**牵连面普查（R1-F2 修订补入）**：`docs/architecture.md` 里「会话收尾常驻重建」这个被退役的机制**共出现在四处**，不止 §8.1 与 §6.4——`grep -n '常驻重建' docs/architecture.md` → `:138`（§3.3 假设清单「设备常驻在线」行）/ `:371`（§6.4，T1 已覆盖）/ `:460`（§8.1 风险 3）/ `:529`（§10 决策 29 正文）。若只改 §6.4 与 §8.1，落地后 §3.3 与 §10 决策 29 会与它们直接矛盾，下一批接手的人读到的是四份互斥表述。

**证据**：`docs/architecture.md:138`（§3.3 假设清单行原文：`| 设备常驻在线（提醒推送可达） | 引擎侧两道超时已修正为 24h 兜底（决策 21）；已裁：永不休眠（决策 29）；重连 + 常驻重建已落（261010 包） |`）；`:371`（§6.4 全文，T1 承载）；`:460`（§8.1 风险 3 行）；`:529`（§10 决策 29 正文原文含「会话收尾本地关后短延迟（T=5s，kResidentRebuildDelayS，已冻结进齿）常驻重建，Idle 态恒持活 WS」）；`:498`（§10 头部纪律「本节以下为 MVP 时代决策历史档，冻结不再新增」+ 决策 29 的既定例外声明）；`:520`（决策 21 的「⚠️ **2026-10-22 取代注记（部分）**」= 本批注记形态的先例）、`:549`（§12 变更记录行记载「§10 决策 25 取代注记」= 第二个先例）；`:467`（风险 9 闭账句）、`:470`（风险 14）、`:176`（§5.1.1 会话状态机行 ⚠️【修复项】）；`server/deploy/261114-openclaw-session-maintenance/session-maintenance-declaration.json`（live 已 enforce：`pruneAfter=14d` / `preserveRecent=30d` / `maxEntries=300` / `maxDiskBytes=40mb`；`_note` 记 kidpal 库 session_nodes 177→1）。

**根因**：§8.1 的闭账句、§3.3 的假设行、§10 的决策正文与 §5.1.1 的 ⚠️【修复项】注记都是「一次性写下、后续批不回读」的形态，机制退役时没人负责把它们翻回来——同一个机制被四处独立表述，任何一处漏改就产生文档内部矛盾。§5.1.1 的 ⚠️ 注记另有一层：它记的是「Phase 1 要接 timer 生产者」，但没记「接到哪、以什么形态接」，接线批若不回写就会留下「文档说要接纯模型、实现接在主循环 tick」的双源。

**改法**：

1. **§8.1 风险 3 行改写**：保留「已闭账」结论与 human 2026-10-10 触点①裁定原文，把缓解句从「服务端/网络侧断自动重连 + 会话收尾常驻重建随 261010 包根治」改为「服务端/网络侧断自动重连（261010 包）+ **会话收尾不再拆通道、常驻由 front 心跳与设备自愈巡检承载**（本批，ADR-019）」，并追加一句沿革注记说明 261010 的「常驻重建」补丁随本批退役（历史不删、只标取代）。
2. **§8.1 新增四行风险**（编号沿表内现有习惯追加；第 ④ 行为 v1.2 门 1 F-01 修订新增）：① **离线期长重试功耗**——每 60s 一次 TLS 握手尝试，真机量一轮离线 30 分钟电量差回填；缓解 = 封顶 60s 稳态 + 后台 task 不阻塞主循环 + 守卫三条件保证只在真离线时活跃；owner = dev-loop 运维面（观察项）。② **心跳流量与日志噪声**——约 115 KB/日下行 + 设备侧必须显式 no-op 消费分支（否则每 30s 一条 `Unknown message type` WARN 冲 SD 日志 16MB 轮转预算）；缓解 = 间隔可部署面 env 调 + 设备侧分支为验收断言；owner = dev-loop。③ **引擎 24h 兜底成为唯一重建窗口**——设备 Idle 期不上行音频 → 引擎 `last_activity_time` 不刷新 → `_check_timeout` 到点关连接，形态 = 每日一次服务端侧断 + 一次 bootstrap burst + 一次 `mem_local_short.save_memory` flush；裁为**接受**（副作用是良性的：它同时是引擎 RAM `Dialogue` 与本地记忆总结的天然日切点），缓解 = 设备 A 路径自动重连已覆盖；owner = 运维面（观察项）。④ **记录会话空累积结束后 ≤10s 的引擎抑制窗（v1.2 新增，门 1 F-01 修订的显式残余）**——`pending_stop` 是受保护态（提前释放会复发 261022 的孤儿幻听缺陷），故释放时点上界 = 既有冻结常量 `PENDING_STOP_SECONDS = 10.0`；触发需三件同时成立（空 accum 结束 + 10s 内从非 story 页换会话 + 立刻开口），后果 = 一次话语被抑制、再按 KEY 即恢复，无数据损坏；裁为**接受**（用一次可恢复的抑制换「录音态 KEY 收尾归档不丢」，后者是 S-F2.2 AC4 v1.65 的硬语义），缓解 = ctx 边沿兜底覆盖非 pending 形态 + 真机轮 A 的 A5 实测窗口长度；owner = dev-loop（观察项，若真机实测窗口频繁命中再回 `/arch-change` 评估缩短 `PENDING_STOP_SECONDS`——那是既有冻结常量，改值须回 planner 重冻结）。
3. **§8.1 风险 14 行补注记**：明确「共享短上下文的增长控制权威 = OpenClaw `session.maintenance`（live 已 enforce，参数见 `server/deploy/261114-openclaw-session-maintenance/`）+ 自动 compaction」，并钉一句**本批实证结论**：引擎 RAM `Dialogue` 在长连接下无裁剪（`dialogue.py` 全文零窗口逻辑），但它**不进 LLM 请求**（`_kidpal_turn_messages` 只发会话级前缀 + 本回合尾部），∴ 增长面是每连接数十 KB 的 RAM 与日切时的一次 `save_memory`，不是 token 成本；token 成本的增长面在 OpenClaw 持久会话，归本行既有承载。
4. **§5.1.1 会话状态机行 ⚠️【修复项】改写**：从「`ses_machine_dispatch` 生产代码零调用——真机 60 秒自动关闭从未发生，Phase 1 接 timer 生产者」改为「60 秒超时 producer **已接线**（本批，ADR-019）：落点 = `application.cc` 主循环 1s tick + 既有 `clock_ticks_`（状态迁移归零）+ VAD 说话沿重置，判据 `Listening && clock_ticks_ >= 60 && !记录语境` → 统一会话结束处理器；**纯模型维持验收 oracle 身份、仍不接生产**（`ses_machine_dispatch` 生产调用点 0，本批复核为真）——活的会话状态权威是 `device_state_machine.cc`」。设备状态机行同步补一句「会话边界权威承载」。
5. **§3.3 假设清单「设备常驻在线」行加沿革注记（R1-F2）**：`:138` 的缓解/现状列原文「引擎侧两道超时已修正为 24h 兜底（决策 21）；已裁：永不休眠（决策 29）；重连 + 常驻重建已落（261010 包）」改为「引擎侧两道超时已修正为 24h 兜底（决策 21）；已裁：永不休眠（决策 29）；重连已落（261010 包）；**常驻由 front 周期心跳 + 设备自愈巡检承载，会话收尾不再拆通道亦不再重建**（本批，ADR-019——261010 的「会话收尾常驻重建」机制随本批退役，历史不删、只标取代）」。假设本体（「设备常驻在线 → 提醒推送可达」）与其成立性**不变且被强化**：本批把「常驻」从「断了再建的名义常驻」推进为「不拆的真常驻」，假设被打破的概率下降。
6. **§10 决策 29 加取代注记（R1-F2）**：`:529` 正文**一字不改**（历史档冻结纪律），在条目末尾追加「⚠️ **2026-11-16 取代注记（部分）**」段，形态逐字仿决策 21 的既有先例（`:520`）与决策 25 取代注记（§12 `:549` 记载）。注记内容三段：① **决策本体全数保留并强化**——「设备永不休眠、常驻在线、提醒推送可达性优先于闲置功耗、18650 续航代价随裁定接受」零改动，本批是该裁定的**兑现**（此前「常驻」被三处各自削弱：会话收尾即拆通道、设备侧 120s 无下行判死、自动重连 3 次即止）；② **被取代的只是机制句**——「会话收尾本地关后短延迟（T=5s，`kResidentRebuildDelayS`，已冻结进齿）常驻重建」随本批退役，`ScheduleResidentRebuild` 与 `kResidentRebuildDelayS` 一并删除、对应冻结齿（`boot_auto_connect.acceptance.c` 的 RC-A3/RC-B2 族）按新裁定重写；③ **取代后的机制本体**——语音会话结束改发 `listen stop` 且通道保持，常驻由 front 周期下行 `kidpal_keepalive` + 设备主循环 tick 自愈巡检承载，自动重连改封顶长跑 + 人工意向位；决策本体 = ADR-019，机制详情 = §6.4。**「本地关 8 调用点分流零误重连」一句同步对账（R2 nit 搭桥）**：正文的「**8** 调用点」是 261010 期计数（源码 commit `f9475599`），现行非 test 面实为 **9**，本批后收敛为 **5**（会话收尾 4 处退役、真拆通道 5 处保留）——三个数各自正确，按「历史档正文一字不改、取代注记负责搭桥」表述，使账实一眼可读；`local_close_pending_` 分类机制与「零误重连」不变量逐字保持。
7. **requirements 侧对账（不在本批 diff 内，交主线程路由）**：`docs/requirements.md:48` 的 ⚠️「60 秒超时实现侧对账」注记同样需要按上述结论刷新（「活侧无 timer 生产者」「真机 60 秒自动关闭从未发生」两句在本批落地后失效）。该行属需求面文件的**实现侧对账登记**，不是需求语义变更；本批不写它，登记进 §6 留根清单交主线程裁路由（docs-delta 或需求面对账小批）。

目标文件：`docs/architecture.md`（改）

### T4 · 契约 §四：新增 `listen` 帧节（会话边界带内信号）+ 新增下行 `kidpal_keepalive` 心跳帧 + `kidpal_story_discard` 发送时机措辞改 + 变更记录条

**问题**：契约 §四「端云协议剖面」今天缺两块承重事实：① **`listen` 帧族完全未钉**——它是设备上行最频繁的控制帧（每次 Idle→Listening 发 `start`、每次会话收尾发 `stop`），且已经承载「记录会话 KEY 收尾 → 归档」这种一次性不可逆语义，却不在契约里；本批把它提升为**所有语音会话的通用结束信号**后，它就成为 L2 边界的唯一 wire 承载，不钉进契约 = 下一批任何人改 front 消费顺序都不会触发契约门。② **通道保活没有任何 wire 承载**——L1 常驻依赖 front 周期下行心跳，这是一条新的云→端消息，属数据格式面，必须进契约（否则实现包的 `display_schema`/`OnIncomingJson` 两侧都没有权威可对齐）。此外 `kidpal_story_discard` 行的发送时机今天写死为「设备关闭音频通道（断连）之前」，L1 常驻后该表述失去物理承载。

**证据**：`docs/contracts/interface-contract.md:339-364`（§四 全节逐条现核：hello / Opus / `stt`/`tts` / `abort` / `kidpal_session_ctx` / `kidpal_story_delete` / `kidpal_story_fetch` / `kidpal_story_discard` / `kidpal_points_delete` / `kidpal_points_page` / `mcp` / `llm`/`system` / 会话连续性承载 / provider 配置硬约束——**`listen` 零命中**，`grep -c 'listen` 状态消息' = 0`）；`:350`（`kidpal_story_discard` 行原文「发送时机 = 设备关闭音频通道（断连）**之前**同连接先行发送（同一 TCP 有序，网关泵先消费 discard 再处理断连）」）；`:347`（`kidpal_session_ctx` 行原文「wss 常驻连接复用时同样发送，重复发送安全」）；`firmware/main/protocols/protocol.cc:74-91`（`SendStartListening` / `SendStopListening` 的实际 wire 形状：`{"session_id":"...","type":"listen","state":"start","mode":"realtime|auto|manual"}` / `{"session_id":"...","type":"listen","state":"stop"}`）；`firmware/main/application.cc:924-926`（未知 type 落 `ESP_LOGW("Unknown message type: %s")`）。

**根因**：`listen` 是上游小智原生帧，方向 B 立项时按「沿用原生协议、不另行钉」处理，于是它一直没进 KidPal 契约面——直到 261001 批把「记录会话归档」这个不可逆动作挂到 `listen stop` 上，它才事实上变成承重帧；契约没跟上，是典型的实现先行、契约滞后。心跳帧则是本批新引入的机制，没有历史包袱，属正常的 additive 扩面。

**改法**：

1. **§四 新增一条 `listen` 帧节**（位置紧随 `abort` 消息条之后，保持「端→云控制帧」聚在一起），钉死四件事：
   - 形状（照实现原文钉，不发明字段）：`{"session_id":"<sid>","type":"listen","state":"start","mode":"realtime|auto|manual"}` 与 `{"session_id":"<sid>","type":"listen","state":"stop"}`；`state` 闭集 `start / stop / detect`——其中 `detect` **是本板确发的帧**，不是「上游原生但 KidPal 不用」的预留值：形状 = `{"session_id":"<sid>","type":"listen","state":"detect","text":"<唤醒词>"}`，发送原语 = `Protocol::SendWakeWordDetected`（`firmware/main/protocols/protocol.cc:68-72`），两个调用点 = `application.cc:1217`（`HandleWakeWordDetectedEvent` 内，`#if CONFIG_SEND_WAKE_WORD_DATA`，`firmware/sdkconfig:733` = `CONFIG_SEND_WAKE_WORD_DATA=y` → **本板活路径**）与 `application.cc:1500`（`WakeWordInvoke` 内，`#if CONFIG_USE_AFE_WAKE_WORD || CONFIG_USE_CUSTOM_WAKE_WORD`，`firmware/sdkconfig:731` = `CONFIG_USE_AFE_WAKE_WORD=y`）。三值一律钉进契约的理由：`detect` **不参与会话边界语义**（引擎侧 `listenMessageHandler` 的 detect 分支走唤醒词识别 + `startToChat`，与 start/stop 的采音起止是两件事），钉进来是为了防未来把边界语义误挂到它上面，也为了如实反映本板确实会发它。
   - **会话边界语义（本批新钉）**：`listen start` = 一次语音会话的采音开始；`listen stop` = **该语音会话的结束信号**，与触发源无关（KEY 手动关闭 / 覆盖层关层 / 云端 `end_session` / 60s 无语音超时 / 记录会话收尾一律同帧）。通道不因它关闭——**L1 与 L2 解耦的 wire 承载即本句**。
   - **两层含义的分辨权在网关**：`listen stop` 在记录语境下另触发归档（既有语义，S-F2.2 AC4 v1.65 三式收尾之一），分辨判据 = 网关侧该连接的累积器路由态（`accumulate` 才归档）；问答会话恒不命中，∴ 新增的通用结束语义与既有归档语义**互不干扰、无需新字段**。
   - **顺序约束（承重）**：`kidpal_story_discard` 必须**先于**同一会话的 `listen stop` 发出（同一 TCP 有序，网关泵先消费 discard 再消费 stop）；次序颠倒 = 已丢弃素材被归档（不可逆）。
2. **§四 新增一条 `kidpal_keepalive` 帧节**（云→端，KidPal 私有扩展消息）：`{"type":"kidpal_keepalive"}`（单键、无载荷、无 session_id——它是连接级而非会话级信号）；发送方 = front，周期 30s（间隔属**行为策略**，权威承载 = arch §6.4，不进契约，与 §1.5 第 6 条「推送触发时机属行为策略」同型处理）；设备侧 = **显式 no-op 消费**（只作活性刷新，不渲染、不进状态机、不回执、不下透引擎）；省略整条消息 = 设备侧 120s 无下行判据按既有逻辑把通道判死并自愈重连，**向后兼容**（旧固件收到它会落 `Unknown message type` WARN 一条，功能不受损——这条降级形态必须写进契约，因为部署序是 front 先上、固件后烧）。
3. **§四 `kidpal_story_discard` 行措辞改**：「发送时机 = 设备关闭音频通道（断连）**之前**同连接先行发送（同一 TCP 有序，网关泵先消费 discard 再处理断连）」→「发送时机 = 设备发出本会话的 `listen stop` 结束信号**之前**同连接先行发送（同一 TCP 有序，网关泵先消费 discard 再消费 stop；次序颠倒 = 已丢弃素材被归档）」。同句尾的「省略整条消息 = 无显式丢弃，断连隐式取消（cancel-on-disconnect）兜底语义维持不变」保留（兜底通道 human 2026-10-21 裁维持）。
4. **§四 `kidpal_session_ctx` 行补一句**：明确它在常驻连接下是**每次语音会话的开始信号**（今天原文已有「wss 常驻连接复用时同样发送，重复发送安全」，本批把它从「兼容性说明」升格为「会话开始边界」的正式承载），并钉「网关按 `source_page` 边沿完整化处置：进 `story` = 入记录语境，离开 `story` 且记录语境仍活 = 释放（覆盖 `listen stop` 丢失的兜底）」。
5. **§四 节尾追加变更记录条**（沿既有格式：日期 + 经 `/arch-change` 批准 + 逐项列举 + 随批原子落地 + PROPOSAL 路径）。

**为什么这样改（对哪个根因）**：第 1 条对「契约滞后于实现」的根因，把已经承重三年的帧补进单一事实源，并在同一处钉死新旧两层语义的分辨判据（避免未来出现「两个会话结束信号谁优先」的歧义）；第 2 条是新机制的必要 wire 承载，且显式写降级形态以支撑「front 先部署、固件后烧板」的部署序；第 3、4 条是 L1 常驻后必然失去物理承载的两处措辞，随批原子改，不留漂移。

目标文件：`docs/contracts/interface-contract.md`（改）

### T5 · 契约 §1.4 / §1.5 / §1.6：`end_session` 改「front 截获 + 机械自推单源」+ 告别回合 prompt 纪律 + 变更记录条

**问题**：requirements §2.1 结束通道② 明写「孩子说『拜拜/不用了』→ **云端判为**结束意图 → 关闭」，契约 §1.4 也早有 `end_session` 行（板子行为 = 「结束会话，回首页常显」）。但这条通道**在实现层三段全断**，而契约文本本身对「谁有权把 `end_session` 发给设备」的表述是**自相矛盾**的：表行说板子要执行它，表后段落又说「不得经 relay 进入板端控制路由；front 自推通路也不把这两个 action 发给设备」——按后者，`end_session` 永远到不了板子，表行的「板子行为」列成了空话。实现层则忠实地按后者做了门禁，并被冻结验收钉死。

**证据**：`docs/contracts/interface-contract.md:239`（`| end_session | 孩子说"拜拜/不用了" | 结束会话，回首页常显 |`）、`:242`（`start_organizing` 行原文含「**网关决定类动作**（与 `end_session` 同型）：front 机械下发、不经引擎 display 流，引擎侧同名 action 拒转」——**契约自己把 `end_session` 当作「front 机械下发」的原型来类比**，却在 :246 说它不下发）、`:246`（表后网关决定类段落原文）、`:254`（§1.5 第 3 条机械校验白名单与门禁描述）、`:263`（§1.6 system prompt 约束全文，**零 end_session 相关纪律**）；`server/gateway/display_schema.py:56-57`/`:60`/`:128`/`:156-163`（门禁实现，self_push 豁免口只有 `start_organizing`）；`server/acceptance/test_record_dialogue_guide.acceptance.py:1267-1272`（`assertFalse(ds.validate_display({...end_session...}, self_push=True)[0])`，断言消息「end_session 借 self_push 放行 —— 自推豁免只许 start_organizing」）；`firmware/main/mcp_server.cc:1725-1727`（`case 0: /* end_session */ app.AbortSpeaking(kAbortReasonNone); break;`——**只停播**）；live prompt `/root/.openclaw/workspace-kidpal/AGENTS.md:31-32`（告别指向 `home`，`control` 行只列可选值不给触发规则）。

**根因**：`end_session` 是 MVP 期从「云端判意图」的需求直接抄进契约表的一行，当时没有任何一侧真的实现它；261019 批引入 `start_organizing` 时确立了「网关决定类 = front 机械下发单源 + 引擎侧同名 action 拒转」这个**正确形态**，并在契约里用「与 `end_session` 同型」来类比说明——但类比句只是措辞，没人回头把 `end_session` 本身接成那个形态；同一批的机械门禁为了「防伪造」把网关决定类一律拒转，只对当批新增的 `start_organizing` 开了自推豁免口，`end_session` 因为是「既有值」被留在了拒绝侧，并被验收齿冻住。三段各自都不是错误决策，叠起来就是一条契约上存在、实现上不可能通的通道。

**改法**：

1. **§1.4 `end_session` 行改写**：触发列补「（模型产薄意图，front 截获后机械自推）」；板子行为列从「结束会话，回首页常显」改为「**结束本次语音会话**（收口到设备侧统一会话结束处理器：`listen stop` 上行 + 回 Idle + 前台按 OQ3=c 回答页驻留规则回落）；**通道保持常驻**」。
   - ⚠️ 这里有一处与既有不变量的冲突需 human 裁（**OQ2**）：原文「回首页常显」与 arch 的 OQ3=c「有回答页驻留则保持其前台」不一致。本批推荐**保持 OQ3=c**（理由见 §4 OQ2），契约措辞随之对账；若 human 裁 `end_session` 特例强制回首页，则本行改为特例表述，且实现包需在设备侧为该 action 单开一条 page-landing 分支（额外成本，且与 KEY 手动关闭的落点不一致）。
2. **§1.4 表后网关决定类段落改写**：把「两动作与 `end_session`/`start_organizing` 同属**网关决定类**：引擎 display 流可携带其薄意图供 front 截获，但不得经 relay 进入板端控制路由；front 自推通路也不把这两个 action 发给设备」拆成两类清楚表述——
   - **截获执行类**（`add_points` / `redeem_points`）：引擎 display 流可携带薄意图供 front 截获，**控制帧不下发任何一端**，设备只消费执行后的权威积分视图（原文语义零改动）。
   - **截获转发类**（`end_session` / `start_organizing`）：引擎 display 流可携带薄意图供 front 截获（`end_session`）或由 front 自行检出（`start_organizing`），**relay 直转恒拒**（防伪造/注入），但 front 可以机械自推单源恰一帧下发设备。`end_session` 自本批起取得与 `start_organizing` 同型的自推豁免口。
3. **§1.5 第 3 条（机械校验）补一句**：display 门禁的控制类分流按上述两类执行——relay 路径对全部四个网关决定类恒拒；self_push 路径对 `start_organizing` 与 `end_session` 放行、对 `add_points`/`redeem_points` 恒拒。白名单仍 9 个 view、零扩。
4. **§1.6 system prompt 约束段补一句**：孩子明确告别（「拜拜 / 不用了 / 先到这儿 / 我不聊了」同族）时，回复 = 口语告别一句 + `{"view":"control","data":{"action":"end_session"}}`；**不得**用 `home` view 承载告别（`home` 收窄为「无需显示内容的普通答复」）。模型不宣称已关闭会话、不生成任何会话状态字段。物理承载仍是 workspace `AGENTS.md`（§5.5.5 静态单源不变，本条只是把纪律内容写进契约的约束段）。
5. **§1.4 节尾追加变更记录条**（沿既有格式），显式记：本次为「实现走偏回归契约意图」还是「契约本身裁改」——本批裁为**后者**（契约 :246 的表述与 :239 的表行自相矛盾，属契约面需裁改的一边），四分类次触点 = `SPEC_CONTRACT_DRIFT`；同时显式申报 `test_record_dialogue_guide.acceptance.py` 的 self_push 侧冻结断言按本裁定**反转**（RED→GREEN 语义翻转，非静默改齿；relay 侧两条断言天然保绿）。

**为什么这样改（对哪个根因）**：根因是「契约内部自相矛盾 + 门禁只对新增值开豁免口」。第 2 条把矛盾消解为两类清楚的形态（截获执行 / 截获转发），这是本批真正的契约面裁定；第 1、3、4 条把三段的权威表述补齐，使 prompt / 门禁 / 设备消费三面都有单源可对；第 5 条把「改了哪一边、反转了哪条冻结齿」显式留痕，避免下一批读到 `assertFalse` 的旧断言消息又把它当红线翻回来。

目标文件：`docs/contracts/interface-contract.md`（改）

### T6 · `docs/decisions/ADR.md` 新增 ADR-019（决策本体）

**问题**：本批做的是一次**决策级**的边界重划（通道与会话解耦），不是实现细节调整。它取代了 261010 批「会话收尾本地关后 T=5s 常驻重建」的机制本体、退役了 K6 批「3 次退避即止」的重连预算、并把决策 29「永不休眠」从「名义常驻」推进到「实际常驻」。按本仓分工纪律（决策本体归 ADR、`docs/architecture.md` §6.x 只抽机制本体），这些裁定必须有一条 append-only 的 ADR 承载，否则 §6.4 的重写就成了无源之水。

**证据**：`docs/decisions/ADR.md` 现有最大编号 = ADR-018（`grep -n "^## ADR-0" | tail`，2026-11-04 `261103-star-points-story-cover`）；五件格式先例 = ADR-018（日期 / 决策 / 理由 / 备选与放弃原因 / 关联）；`docs/architecture.md:498`（§10 头部纪律「2026-09-14 起，新架构决策一律登记 ADR.md（append-only，五件格式）；本节以下为 MVP 时代决策历史档，冻结不再新增」）；`:520`（决策 21 全文，含「结构级备选（方案二）= 网关设备腿常驻、引擎腿按会话起停」及其 2026-10-22 取代注记）。

**根因**：不适用（本 T 是制品义务，不是缺陷修复）。

**改法**：新增 `## ADR-019 · 语音会话生命周期与 WS 通道生命周期解耦：通道常驻 + 带内会话边界（listen start/stop）+ front 心跳保活 + 封顶长跑重连`，五件齐备：

- **日期**：2026-11-16（`261115-arch-review-session-channel-resident` 架构+契约面 Track B；行为输入 = requirements §2.1 结束三通道 + §2.3 录音态按键口径，均 frozen，本批零需求语义变更）。
- **决策**（六条）：① WS 通道的终止源是闭集三条（服务端/网络侧断、引擎 24h 兜底、重启/OTA/AEC 切换/`ResetProtocol`），**语音会话结束不在其中**；会话收尾不再拆通道，261010 的「T=5s 常驻重建」机制退役。② 语音会话边界的权威 = 设备状态机，wire 表达 = 既有帧对（开始 `kidpal_session_ctx` + `listen start`，结束 `listen stop`），**不新增上行帧**；六个触发源统一收口到单一处理器。③ 通道保活 = front 周期下行 `kidpal_keepalive`（30s）+ 设备侧 120s 无下行判据的**语义升级**（阈值与逻辑零改动）+ 主循环 1s tick 自愈巡检。④ 自动重连改封顶递增长跑（`{5,10,20,40,60}` 后恒 60s），并配套人工意向位——离线期人工入口不再静默 no-op，改为置意向、截短退避、连上后补投事件。⑤ front 的每会话态（记录语境抑制标志、累积器、会话语境）改由带内信号驱动收尾，连接级 teardown 降为兜底；`kidpal_session_ctx` 边沿完整化作为第二层防线。⑥ 云端结束意图通道 = 模型产薄意图 → front 截获 → 机械自推单源下发 → 设备收口统一处理器；relay 直转恒拒。
- **理由**：断了再建是纯绕路——常驻通道本就是设计目标态（提醒推送、故事预取都跑在它上面），拆它只为了表达「会话结束」这个语义，而该语义完全可以用已经在 wire 上跑着、且引擎已经消费的 `listen stop` 表达。真机代价可量（温 0.82s vs 冷 1.57s + 约 15s burst，KPLOG029）；而收益不只是时延：`TECH-261012` 首按键失同步、`TECH-261014-session-end-channels-unwired` 的「一轮聊完设备常驻聆听直到 137s 通道超时」、以及提醒在重建窗内的可达性抖动，三者同源。会话边界改带内信号后，边界从「物理事件（断连）」变成「显式语义事件」，可观测、可验收、可幂等重试。
- **备选与放弃原因**：① **维持现状 + 把 5s 重建缩到 1–2s**（`TECH-261114` 候选 B）——放弃：只缩竞态窗、不消除 burst 与冷启动税，且 5s→1s 会让「孩子在重建窗内按键」的概率反而更容易撞上 in-flight 守卫（人工入口 no-op）。② **决策 21 的方案二（网关设备腿常驻、引擎腿按会话起停）**——放弃：该备选已随 ADR-015 失效（连续性权威移到 OpenClaw 持久会话后，引擎腿起停不再承载任何会话语义），且它把「每会话」的边界搬进 front 内部，反而增加一条 front↔引擎连接生命周期要管。③ **设备周期 WS Ping 保活**——放弃：实证 `web_socket.cc` 的 Pong 分支空实现、`Ping()` 返回 void，不刷新活性时间戳也不可观测失败，要靠它必须改第三方 `managed_components/`（重下载即丢、非自有代码面）。④ **摘掉设备侧 120s 判据**——放弃：半开连接（家庭 NAT 静默过期）要等下一次发送失败才暴露，而 Listening 分支的 `SendStartListening` 不检查返回值，孩子会对着死通道讲完一整段。⑤ **新增 `kidpal_session_end` 上行帧**——放弃：见 §1「为什么复用 `listen stop`」四点（引擎不认新私有帧、front 会多一个与既有 `listen stop` 分支语义重叠的消费点、契约面净扩张而收益为零）。⑥ **会话边界注入引擎**——放弃：不动引擎代码约束下四个既有注入面逐一为语义污染（见本节末），且共享上下文已由 ADR-015 承载、与通道生命周期正交，注入无边际收益。
- **关联**：requirements §2.1（结束三通道 + 60s 阈值）/ §2.2（四态演出）/ §2.3（录音态子态按键口径）/ S-F1.4（提醒必达与录音态排队例外）/ S-F2.2 AC4（丢弃与三式收尾）；arch §2.3 端云职责 / §5.1.1 / §5.1.2 / §5.2 场景①②④ / §6.4（整节重写）/ §6.5 幂等 / §8.1 风险 3·14 + 新增 15/16/17/18 四行 / §10 决策 21（方案二失效注记保持）·决策 29（永不休眠，本批兑现）；契约 §1.4 / §1.5 / §1.6 / §四（`listen` 节 + `kidpal_keepalive` + `kidpal_story_discard` 时序 + `kidpal_session_ctx` 边沿）；ADR-003（`kidpal_speak` 借引擎音频链——**会话间隙提醒出音的承重依据**：`speak_txt` 不经 `chat()`，∴ 不受 `_kidpal_listen_closed` / `_kidpal_record_context` 抑制）/ ADR-013（记录语境抑制）/ ADR-015（持久会话 = 共享短上下文权威）；debt 四条（TECH-261114 / TECH-261014 ×2 / TECH-261012）consumed-by 本 changeId。
- **附：会话边界注入的四个既有面逐一评估（不动引擎代码约束下，本批显式不做，留重评触发）**：`kidpal_speak` = 出音指令，会真的播一句话给孩子（用户可见副作用，不可用作静默边界标记）；`kidpal_record_context` = 布尔抑制开关，形状不承载自由文本；`kidpal_schedule_summary` / `kidpal_points_context` = 各自绑定 system prompt 里一个具名占位（`{{schedule_summary}}` / `{{points_summary}}`）的填充器，借道 = 把「新会话开始了」写进课表或积分摘要，属明确的语义污染且会破坏 §1.5「零自研业务逻辑」的单源红线。**若真机观测出现跨会话语境串味**，可用面 = workspace `AGENTS.md` 静态纪律 + 动态 context 块既有的「本回合时间 / 日期」（模型可据时间跨度自行判「这是新的一段对话」），零引擎代码、零契约改动；重评触发 = 真机出现「孩子隔了很久再说话，小伴把上一段语境当成当前话题」的可复现实例。

目标文件：`docs/decisions/ADR.md`（改，append-only 追加一条）

### T7 · 架构 §12 变更记录追加 changeId 行（定稿标记）

**问题**：架构内容变更的定稿标记 = `docs/architecture.md` 文末「变更记录」节的 changeId 行（arch-change.md「定稿载体」节）；缺该行则 `launchPreconditions` 的「支撑语义 → architecture.md 已冻结」判据不成立，下游 dev 包的 launch 前置无法核对。

**证据**：`docs/architecture.md:537` 起 §12 变更记录（纪律句原文「本文件每次改动都在下表追加一行（新行在上，倒序）——changeId、日期、改了哪些节、依据」；表头 `| changeId | 日期 | 改动面 | 依据 |`；现最新行 = `261103-star-points-story-cover | 2026-11-04`，`sed -n '/^## 12. 变更记录/,$p' | grep '^| 26' | wc -l` = **10**）。

**根因**：不适用（制品义务）。

**改法**：在表头分隔行之下、`261103-star-points-story-cover` 行之上插入一行（倒序纪律）：changeId = `261115-arch-review-session-channel-resident`，日期 = 落地当日，改动面 = 「§3.3（假设清单「设备常驻在线」行加沿革注记：常驻改由心跳 + 自愈巡检承载、261010 的会话收尾重建退役）/ §5.1.1（会话状态机行 ⚠️ 对账改写 + 新增语音会话生命周期收口行 + 设备状态机行补边界承载句）/ §5.1.2（front 行生命周期锚改写：连接期一次性六件保持绑连接、teardown 降兜底、新增每语音会话收尾职责）/ §5.2 场景①②④（建连推送频率 + 会话结束段 + 云端结束意图链 + 取消链时序措辞）/ §6.4 整节重写 / §8.1（风险 3 缓解句改写 + 风险 14 补注记 + 新增 15/16/17/18 四行）/ §10 决策 29 取代注记（正文一字不改，形态仿决策 21/25 先例）/ §12 本行」，依据 = 「ADR-019；debt TECH-261114 / TECH-261014-autoconnect / TECH-261014-session-end / TECH-261012 四条 consumed-by；契约面同批原子落（§1.4 / §1.5 / §1.6 / §四）；human 2026-11-15 立项原话 + 三答（范围全家桶 / 共享短上下文 / 不改引擎补丁）」。

**牵连面完整性（R1-F2 修订）**：本行「改动面」列必须逐节枚举 §3.3 与 §10 决策 29 两处——arch-change.md 四查 ④「牵连面完整（反向锚牵连、联动文件）」要求退役机制的**全部**引用点入册。起草期普查命令与输出：`grep -n '常驻重建' docs/architecture.md` → 四命中 `:138`（§3.3）/ `:371`（§6.4）/ `:460`（§8.1）/ `:529`（§10 决策 29），四处分别在 T1（§6.4）、T3 改法 1（§8.1）、T3 改法 5（§3.3）、T3 改法 6（§10）承载，**零遗漏**。

目标文件：`docs/architecture.md`（改）

---

## 2A. 契约包三节（触契约分支义务，arch-change.md「触契约分支」节）

### proposedContractDiff

四处改动，全部在 `docs/contracts/interface-contract.md`；**既有帧形状零改动、display 白名单仍 9 个、`kidpal_*` 上行帧字段零改动**。逐处给出「改哪一行 / 改成什么」的处方（承重原文逐字引用，落地时按 `--cite-check` 对账）：

**① §1.4 `end_session` 表行（原文 :239）**

原文（承重，逐字）：

```
| `end_session` | 孩子说"拜拜/不用了" | 结束会话，回首页常显 |
```

改后（完整行）：

```
| `end_session` | 孩子说"拜拜/不用了/先到这儿"（模型产薄意图，front 截获后机械自推单源下发） | 结束本次语音会话：设备收口到统一会话结束处理器（上行 `listen stop` + 回 Idle + 前台按 arch OQ3=c 回答页驻留规则回落）；**WS 通道保持常驻**（ADR-019） |
```

**② §1.4 表后网关决定类段落（原文 :246）**

原文（承重，逐字）：

```
两动作与 `end_session`/`start_organizing` 同属**网关决定类**：引擎 display 流可携带其薄意图供 front 截获，但不得经 relay 进入板端控制路由；front 自推通路也不把这两个 action 发给设备。设备只消费执行后的权威积分视图。
```

改后（完整段）：

```
四个网关决定类动作分两型，relay 直转对四者恒拒（防伪造/注入）：**截获执行类**（`add_points` / `redeem_points`）——引擎 display 流可携带其薄意图供 front 截获执行，控制帧不下发任何一端，设备只消费执行后的权威积分视图；**截获转发类**（`end_session` / `start_organizing`）——`end_session` 由 front 从引擎 display 流截获模型薄意图、`start_organizing` 由 front 自行检出收尾，二者均以 front 机械自推单源恰一帧下发设备（`self_push=True` 豁免口），引擎侧同名 action 拒转。
```

**③ §四 新增 `listen` 帧节 + `kidpal_keepalive` 帧节**（插入位置：`abort` 消息条之后、`kidpal_session_ctx` 条之前；两条新 bullet 的处方正文见 T4 改法 1、2，落地时按该处方的四件事/五件事逐项成文，不缩略）。

**④ §四 `kidpal_story_discard` 行发送时机句（原文 :350 内）**

原文（承重，逐字）：

```
发送时机 = 设备关闭音频通道（断连）**之前**同连接先行发送（同一 TCP 有序，网关泵先消费 discard 再处理断连）
```

改后（完整句）：

```
发送时机 = 设备发出本会话的 `listen stop` 结束信号**之前**同连接先行发送（同一 TCP 有序，网关泵先消费 discard 再消费 stop；次序颠倒 = 已丢弃素材被归档，不可逆）
```

**⑤ §四 `kidpal_session_ctx` 行补句 + §1.6 prompt 约束段补句 + §1.4/§四 两条变更记录条**（处方见 T4 改法 4、T5 改法 4/5）。

### impact

- **设备侧**：`mcp_server.cc` control `case 0` 从 `AbortSpeaking` 改为 `EndVoiceSession()`（一处）；`application.cc` 四条收尾路径改发 `listen stop` 且不拆通道、`OnIncomingJson` 新增 `kidpal_keepalive` no-op 分支（防 WARN 噪声）；`protocol.cc` 的 `SendStopListening` **零改动**（既有原语直接复用）。旧固件收到 `kidpal_keepalive` 的降级形态 = 每 30s 一条 `Unknown message type` WARN、功能不受损（∴ front 可先部署）。
- **front 侧**：`display_schema.validate_display` 的 self_push 豁免口从 1 个值扩到 2 个值；`front_app.py` 新增每连接心跳 task（env 闸 `KIDPAL_CHANNEL_HEARTBEAT` + `KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S`〔默认 30〕，**代码默认开**，`0/false/off/no` 为**回滚开关**而非上线开关；闸**形态**沿 `KIDPAL_STORY_RESTORE_PUSH` 先例〔env 名常量 + 纯函数 + `strip().lower()` 识别集〕，但**极性与该先例相反**，理由必须写清：那个先例默认关是因为旧固件收到 restore 推送会无条件渲染 P3 盖掉聆听态〔**行为性**副作用〕，而本帧对旧固件的唯一副作用是每 30s 一条 `Unknown message type` WARN〔约 6 KB/小时，对 SD 日志 16MB 轮转预算可忽略〕；反之若默认关，新固件在闸未开期间会退化成「每 ~120s 被 `IsTimeout()` 判死 + tick 自愈巡检立即重连」= **比今天更糟的重连 churn**〔今天判死后要等按键才重连〕，故默认开是唯一安全极性——完整部署序与回滚剧本见 `package-split.md` §4.3）、`listen stop` 会话收尾三件、ctx 边沿完整化兜底、`end_session` 截获 + 自推。**零新依赖、零新端口、零新存储**。
- **引擎侧**：**零 diff**（`server/deploy/engine-patch/**` 与 ECS `core/**` 均不动，config 亦不动）。依据三条实证：`listen stop` 不关连接（`listenMessageHandler` stop 分支只置 `client_voice_stop` + 触发尾音 ASR，`close_after_chat` 只由 `no_voice_close_connect`/`max_out_size` 置位）；`listen start` 调 `reset_audio_states()`，同连接多轮 start/stop 是引擎既有支持形态；引擎侧「会话已结束」的闭锁已由 engine-patch 既有的 `_kidpal_listen_closed` 承载（stop 置位 / start 复位 / `chat()` depth==0 首守卫消费）。`kidpal_keepalive` 不达引擎（设备侧 no-op 消费、front 亦不转发上行心跳——它是纯下行帧）。
- **OpenClaw 侧**：prompt 静态语义新增一条告别回合纪律（workspace `AGENTS.md`，§5.5.5 承载与三面相等不变式不变；`OPENCLAW_SYSTEM_PROMPT_CARRIER` 全量副本随批 append）。会话连续性（ADR-015 持久会话）**零影响**——本批不改 `user=<device_id>`、不改「只发本回合」。
- **契约面净扩张**：一条下行私有帧（`kidpal_keepalive`，单键无载荷）+ 一节既有帧的语义钉行（`listen`，形状照实现原文钉、零新字段）+ 一处放行规则改写（`end_session` 自推豁免）+ 两处措辞对账（`kidpal_story_discard` 发送时机 / `kidpal_session_ctx` 会话开始边界）。**无新 view、无新上行帧、无字段变更、无形状 floor 变更。**
- **v1.2 修订（门 1 F-01）的契约影响 = 零改动，依据三条**：① §四 `listen` 节钉的是「`listen stop` = 该语音会话的**结束信号**」这一 wire 语义，**没有**钉「网关必须在收到 stop 的当刻释放全部会话态」——释放时点与分态处置属**行为策略**，权威承载 = arch §5.1.2 front 职责行 / §5.2 场景②④（与 §1.5 第 6 条「推送触发时机属行为策略，权威承载 = arch §5.2，不进契约」同型处理，该先例逐字可引）。② §四 `kidpal_story_discard` 的顺序约束（discard 先于 stop）**不受影响**：discard 走 `acc.cancel(reason="discard")`，是设备显式丢弃手势的独立路径，与「会话收尾三件」不是同一条链；且 discard 到达后 routing 已回 `chat`，随后的 `listen stop` 命中分层表的 `chat` 行 = 无动作，`pending_stop` 保护态根本不会进入。③ `pending_stop` 的 10s 挂起窗是既有实现语义（`PENDING_STOP_SECONDS = 10.0`，`story.py:75`；SAO-A3② 冻结），契约从未钉过它、本批也不钉。∴ **§2A `proposedContractDiff` 四处（①–⑤）v1.2 一字未改**，`landing-manifest` 与 V4/V5/V9/V10/V11/V12 全部不受本轮修订影响；受影响的只有架构面叙述（T2 改法 2/3、T3 改法 2 ④）与实现包的验收断言方向。
- **冻结验收齿影响（显式申报，非静默改齿）**：**反转 1 条**（`test_record_dialogue_guide.acceptance.py:1267-1272` 的 `self_push=True` + `end_session` → `assertFalse` 改 `assertTrue`，断言消息随裁定改写）；**天然保绿 2 条**（同文件 relay 侧断言、`test_points_e2e_spike.acceptance.py:866-868` 缺省 `self_push=False`）；**v1.2 修订后新增三条 must-stay-green（F-01 的断言面，一条都不许动）**：`test_m4k1_story_cloud_link.acceptance.py:1891` SAO-A2（空累积 listen-stop 后 `assertNotEqual(routing, "chat")`）、同文件 `:849-882` M4K1-A2（真机序 ctx→stop→stt 断言 `len(files) == 1` 恰一篇归档）、同文件 SAO-A3（重复 stop 不双归档）；**语义前提改变但断言保绿 2 条（处置显式记，F-03 要求）**：`test_m4k3_record_session_live.acceptance.py:508` `test_a7_ctx_non_story_pushes_zero_frames` 与 `test_m4k1_story_cloud_link.acceptance.py:884` `test_a2_source_page_values_and_backward_compat`——两者的**断言**在 v1.2 下仍全绿（前者用全新 sid、记录语境标记从未置位 → ctx 边沿兜底不触发 → 三断言〔不透传 / routing=="chat" / 零 story_list 帧〕逐条成立；后者同理），但前者的 **docstring 前提**「闭集预留取值现版本无行为，防实现过宽」不再精确（ctx 非 story 现在有一条**条件**行为 = 记录语境仍活时的分态兜底）→ 实现包须改写该 docstring 并**新增一条姊妹齿**覆盖释放边沿（= `package-split.md` 的 SCRF-B4/B4b），不得靠删弱既有齿过关。`story.py:886-894` `cancel()` 与 `:873-884` `release_pending()` 两个纯逻辑缝**签名与语义零改动**（本批只新增调用点，不改判据）。设备侧 `boot_auto_connect.acceptance.c` 的 K6-A2 tooth5（`kAutoConnectBackoffS` `[3]` 长度 + `{5,10,20}` 形态）与 RC-A3/RC-B2（`kResidentRebuildDelayS` / `ScheduleResidentRebuild` 四收尾点各恰 1）按新裁定重写，`voice_round_ui.acceptance.c` 的 `local_close_settle` 4 命中与 `CloseAudioChannel` 族按「会话收尾不再拆通道」重写——具体齿面清单与重写理由归实现包 plan.md（见 `package-split.md`），本面只钉「哪些既有冻结语义被有意取代」。

### validationPlan

分三层，逐层可判：

1. **契约面机械门**（落地前后各跑一次，预期见 §5）：`node .claude/scripts/contract-lint.mjs`（`ok:true`，warnings 不新增）；`node .claude/scripts/spec-anchor-check.mjs`（`ok:true`、`errors:[]`）；`node .claude/scripts/landing-fidelity-check.mjs --cite-check <本 PROPOSAL>`（②④ 承重原文块逐字对账通过）；`pre-merge-escalation-gate.mjs` 三件联立凭据（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖 `docs/contracts/interface-contract.md`）。
2. **实现面离线验收**（归实现包，方向见 `package-split.md`；v1.2 按门 1 F-01 修订改写）：front 侧 python 离线验收新增 `test_session_channel_resident.acceptance.py`（心跳 task 恰一帧/周期 + env 闸**默认开**、`=0` 可关〔两态各一断言〕 + **`listen stop` 分层收尾**：问答会话 no-op 零注入 / 归档路径不重复动作 / **`pending_stop` 受保护——当刻零 cancel 零释放** + **10s sweep 的 pending 分支新增释放点**（注入 `now = pending_since + 10s` → `release_pending` 生效 + `active=false` 恰一帧注入 `_session_legs(sid)` 归属腿）+ ctx 边沿兜底分态（`accumulate` → cancel + 释放；`pending_stop` → 不动作）+ `end_session` 截获-自推恰一帧 + relay 恒拒 + discard→stop 有序不归档）；**must-stay-green 复跑面**（一条都不许红）= SAO-A1/A2/A3 + M4K1-A2 三齿 + M4K3-A7 四齿 + `_story_timeout_sweep` 既有 10s 语义齿（RDG-A6 家族）+ S-F1.4 提醒必达齿；设备侧 C host 验收新增 `session_channel_resident.acceptance.c`（统一处理器六触发源收口 + 四收尾点零 `CloseAudioChannel`/零 `ScheduleResidentRebuild` + `kidpal_keepalive` no-op 分支存在 + 60s tick 判据与记录语境豁免 + VAD 沿重置 + 长重试退避表形态 + 人工意向位）。
3. **真机轮**（human 持板，两轮，agent 可部署 ECS + 重启 front）：**轮 A**（P1 落地后）= 温通道按键进聆听时延对照 KPLOG029 基线 0.82s、连续 5 次「结束→再说话」零冷通道零 burst（front 日志核 bootstrap 六件每连接恰一次）、闲置 > 5 分钟（跨越旧 120s/137s 判死点）后首按键仍走温通道、**会话间隙提醒必达且出音**（S-F1.4 + ADR-003 承重实证：`_kidpal_listen_closed` 为真时 `kidpal_speak` 仍出音）、记录会话全链回归（引导态 KEY 结束无归档 / 录音态 KEY 收尾归档 / 录音态 BOOT 丢弃不入册 / 三式收尾 `start_organizing`）、SD 日志无 `Unknown message type` 噪声；**轮 B**（P2 落地后）= 60s 无语音自动关闭真机首次发生（requirements §2.1 ⚠️ 对账项闭环）、孩子连续讲 > 60s 不被误收（VAD 沿重置）、说「拜拜」全链通（模型产薄意图 → front 截获 → 自推 → 设备结束会话且麦克风关）、拔网线/重启 front 两种离线形态下的长重试自愈与人工意向不丢、离线 30 分钟电量差、引擎 24h 日切形态（可用 `close_connection_no_voice_time` 临时调小在测试环境复现，不改生产值）。

---

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

> 分类口径：**本批 canonical diff**（arch lane，主线程 packet-bound 落地）= 下表「本批」列；**实现面 diff**（dev lane，归实现包，见 `package-split.md`）= 「实现包」列。两列互斥不重叠——本批 manifest 只含 canonical 三件。

| 面 | 文件 | 类别 | 说明（消费方写清） |
|---|---|---|---|
| 架构 | `docs/architecture.md` | 本批（T1/T2/T3/T7） | §3.3 假设清单「设备常驻在线」行加沿革注记 + §5.1.1 三行（会话状态机 ⚠️ 对账改写 / 新增语音会话生命周期收口行 / 设备状态机行补边界承载句）+ §5.1.2 front 行生命周期锚改写 + §5.2 场景①②④ + §6.4 整节重写 + §8.1 六行（风险 3 缓解句 / 风险 14 注记 / 新增 15/16/17/18 四行）+ §10 决策 29 取代注记（正文一字不改）+ §12 一行。**「常驻重建」四处引用全数对账**（:138 / :371 / :460 / :529，普查命令见 T7 牵连面段）。消费方 = 下游 dev 包 launchPreconditions（「支撑语义已冻结」判据 = §12 changeId 行在场）、architect-reviewer 四查、未来所有读 §6.4 的批 |
| 契约 | `docs/contracts/interface-contract.md` | 本批（T4/T5） | §1.4 表行 + 表后段落、§1.5 第 3 条、§1.6 约束段、§四 两新节 + 两措辞改 + 两变更记录条。消费方 = `display_schema.py` 机械门、`mcp_server.cc` 控制路由、`protocol.cc` 发送原语、front `_consume_kidpal_control`、`pre-merge-escalation-gate.mjs` 三件联立凭据 |
| 决策 | `docs/decisions/ADR.md` | 本批（T6，append-only 追加 ADR-019） | 消费方 = §6.4 机制本体的决策源、未来复议的入口 |
| 固件 | `firmware/main/application.cc` / `application.h` | 实现包 P1+P2 | 四条收尾路径 + 统一处理器 + 心跳 no-op 分支 + tick 巡检 + 60s producer + 长重试 + 意向位 |
| 固件 | `firmware/main/mcp_server.cc` | 实现包 P2 | control `case 0` → `EndVoiceSession()` |
| 固件 | `firmware/main/protocols/protocol.cc` / `websocket_protocol.cc` | 未触 | `SendStopListening` / `IsTimeout()` / `local_close_pending_` 三处**逻辑零改动**（本批设计的关键取舍：不改阈值、不改判据、不改分类机制，只改「谁在什么时候调它们」）。若实现包发现 `IsTimeout()` 的 `ESP_LOGE` 需降频，改动落在该文件 → 届时由 planner 经扩单通道回评补 manifest |
| 固件验收 | `firmware/main/test/boot_auto_connect.acceptance.c` | 实现包 P1+P2 | K6-A2 tooth5（退避表形态）/ RC-A3 / RC-B2（四收尾点各恰一次 `ScheduleResidentRebuild`）按新裁定重写；`CloseAudioChannel` 24 命中逐条核 |
| 固件验收 | `firmware/main/test/voice_round_ui.acceptance.c` | 实现包 P1 | `local_close_settle` 4 命中（src token 族保留、`src=` 取值语义按「不拆通道」改写）+ `CloseAudioChannel` 19 命中 |
| 固件验收 | `firmware/main/test/record_session_live.acceptance.c` | 实现包 P1 | `CloseAudioChannel` 22 命中（录音丢弃面改 discard→listen stop 有序） |
| 固件验收 | `firmware/main/test/session_channel_resident.acceptance.c` | 实现包 P1（新增） | 六触发源收口 + 负锚（会话收尾区零 `CloseAudioChannel`/零 `ScheduleResidentRebuild`）+ 心跳 no-op 分支 + Idle 清位链逐条保绿 |
| front | `server/gateway/front/front_app.py` | 实现包 P1（心跳 + 分层会话收尾 + sweep 释放点）+ P2（end_session 截获自推） | 消费方 = 设备腿；**五缝**：`ws_gateway`（心跳 task 生命周期）/ `pump_client_to_upstream`（`listen stop` 分层收尾）/ `_consume_kidpal_control`（ctx 边沿分态兜底）/ **`_story_timeout_sweep_body:1255-1257`（pending 分支补 `_release_record_context`——v1.2 门 1 F-01 的承重新增点）** / `_push_display_payload`（P2 自推） |
| front | `server/gateway/story.py` | **未触（签名与判据零改动）** | `cancel()` :886-894 与 `release_pending()` :873-884 两个纯逻辑缝、`PENDING_STOP_SECONDS = 10.0` :75 全部逐字保留；本批只新增 front 侧调用点，不改纯决策面（v1.2 F-01 复核结论） |
| front | `server/gateway/display_schema.py` | 实现包 P2 | self_push 豁免口 1 值 → 2 值；relay 恒拒不变 |
| front 验收 | `server/acceptance/test_record_dialogue_guide.acceptance.py` | 实现包 P2 | **1 条断言反转**（:1267-1272 self_push + end_session）；同文件 relay 侧断言保绿；RDG-A6 sweep 家族（10s 语义）保绿 |
| front 验收 | `server/acceptance/test_m4k1_story_cloud_link.acceptance.py` | **未触（must-stay-green，v1.2 新增登记）** | SAO-A1/A2/A3 + M4K1-A2 三齿（`:849-882` 恰一篇归档 / `:1891` 空累积 stop 不得回 chat / 重复 stop 不双归档）是 F-01 的断言面，**一条都不许动**；`test_a2_source_page_values_and_backward_compat`（:884）断言保绿、docstring 前提随 ctx 边沿兜底精确化 |
| front 验收 | `server/acceptance/test_m4k3_record_session_live.acceptance.py` | 实现包 P1（仅 docstring + 新增姊妹齿） | `test_a7_ctx_non_story_pushes_zero_frames`（:508）三断言在 v1.2 下**仍全绿**（全新 sid → 记录语境标记未置位 → 边沿兜底不触发），但 docstring「非 story 取值零行为」前提不再精确 → 改写前提 + 新增覆盖释放边沿的姊妹齿（SCRF-B4/B4b）；**不得靠删弱既有齿过关**（F-03 处置） |
| front 验收 | `server/acceptance/test_points_e2e_spike.acceptance.py` | 未触（天然保绿） | :866-868 用缺省 `self_push=False`，本批 relay 恒拒 → 该 must-stay-green 断言不受影响（起草期实读确认） |
| front 验收 | `server/acceptance/test_session_channel_resident.acceptance.py` | 实现包 P1（新增） | 心跳恰一帧/周期 + env 闸**默认开**、`=0` 可关（两态各一断言）+ listen stop **分层**收尾（问答 no-op / 归档路径不重复动作 / **pending_stop 当刻零 cancel 零释放**）+ **10s sweep pending 分支新增释放点**（注入 now → release_pending + `active=false` 恰一帧入归属腿）+ ctx 边沿分态兜底 + discard→stop 有序不归档 |
| 部署制品 | `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` + `OPENCLAW_SYSTEM_PROMPT_CARRIER` | 实现包 P2 | 告别回合纪律（§5.5.5 静态单源 + 三面相等不变式，ADR-001） |
| 部署制品 | `server/deploy/manifest.yaml` / `verify-deploy.sh` | 实现包 P2（条件） | 仅当 prompt 承载件 md5 核验需随改；本批不新增部署文件 |
| 引擎 | `server/deploy/engine-patch/**` + ECS `core/**` | **未触（硬约束）** | human 2026-11-15 未授权改引擎补丁；§2 实证段三条依据证明零改动即成立 |
| 引擎 config | ECS `.config.yaml`（`close_connection_no_voice_time` / `end_prompt.enable`） | 未触 | 决策 21 保持；24h 日切形态裁为接受（T3 改法 2 ③） |
| 需求 | `docs/requirements.md:48` ⚠️ 对账注记 | 未触（留根） | 实现侧对账登记，非需求语义；交主线程路由（§6 留根 1） |
| debt | `docs/debt/tech/TECH-261114-*` / `TECH-261014-autoconnect-*` / `TECH-261014-session-end-*` / `TECH-261012-*` | 未触（主线程记账） | 四条 consumed-by 本 changeId；结项与 `debt-render.mjs` 归位由主线程在落地后执行 |
| 治理面 | `.claude/**` | 未触 | review profile G1/G2/G3 = 零 |

**【硬项 · sweep 透镜义务】** 本批不新立门/新立脚本，透镜义务落为两条机械枚举：**(甲) 既有冻结齿的命中面**、**(乙) 退役机制在 canonical 文档的全部引用点**。起草期实跑（命令与输出 verbatim 见文末「起草期实跑记录」）：

- **(甲) 冻结齿命中面**：`grep -c` 对 `boot_auto_connect.acceptance.c`（`ScheduleResidentRebuild` 31 / `kResidentRebuildDelayS` 19 / `kAutoConnectBackoffS` 20 / `CloseAudioChannel` 24）、`voice_round_ui.acceptance.c`（`local_close_settle` 4 / `ScheduleResidentRebuild` 3 / `CloseAudioChannel` 19）、`record_session_live.acceptance.c`（`CloseAudioChannel` 22）；`grep -rln "ses_machine_dispatch" firmware/main` 全命中集 = 4 文件（.cc/.h + archive 两件），非 test 命中 = 2（即定义与声明自身）；`grep -rn 'end_session' server/` 的 live 非 archive 命中 = `display_schema.py` + `test_points_e2e_spike` + `test_record_dialogue_guide` 三件。
- **(乙) 退役机制的文档引用点普查（R1-F2 修订补入）**：`grep -n '常驻重建' docs/architecture.md` → **4 命中**（`:138` §3.3 假设清单 / `:371` §6.4 / `:460` §8.1 风险 3 / `:529` §10 决策 29），四处分别由 T3 改法 5 / T1 / T3 改法 1 / T3 改法 6 承载，**零遗漏**；`grep -rn 'protocol_->CloseAudioChannel' firmware --include=*.cc --include=*.h | grep -v /test/ | wc -l` → **9**（全在 `application.cc`；会话收尾 4 = :976/:1032/:1164/:1515 随本批退役，真拆通道 5 = :508/:1407/:1426/:1588/:1601 逐字保留）；`grep -c 'ScheduleResidentRebuild()' firmware/main/application.cc` → 定义 1 + 调用点 4（:989/:1040/:1169/:1524），全数退役；`grep -n 'kResidentRebuildDelayS' firmware/main/application.cc` → **3 命中**（定义 :112 / helper 头注释 :621 / 消费 :627），随 helper 一并删除。
- **完备性论证**：模式集按「机制退役的四个符号（`ScheduleResidentRebuild` / `kResidentRebuildDelayS` / `kAutoConnectBackoffS` / 中文散文 token `常驻重建`）× 语义改写的两个符号族（`CloseAudioChannel` / `local_close_settle`）× 放行规则的一个符号（`end_session`）」穷举。七符号均无词形变体：C 标识符与 python 字面量无中英/单复数/大小写变体；中文散文 token `常驻重建` 的普查面 = 全 `docs/` 树（`grep -rn '常驻重建' docs/`），命中分布为 **canonical 面 4 处全在 `docs/architecture.md`**（`:138` 假设清单 / `:371` 横切机制 / `:460` 风险表 / `:529` 决策历史档，已全数入册）+ **debt 账本面 6 处**（`docs/debt/tech/INDEX.md` 生成件 1 + `TECH-261012` 2 + `TECH-261114` 2 + `docs/debt/tech/_archive/2026/TECH-261024-*` 1）——debt 面是 append-only 记录库、**不改写只结项**（四条 consumed-by 本 changeId，`INDEX.md` 是 `debt-render.mjs` 生成件禁止手改，归档件冻结），故不在本批改动面内属正确处置而非遗漏；`docs/requirements.md` / `docs/contracts/interface-contract.md` / `docs/decisions/ADR.md` 对该 token 命中各 = **0**（实测，该机制纯属架构面表述，需求与契约面不引用它）。backtick 形态只出现在文档面，已按 `docs/` 全树覆盖。

---

## 4. human 取舍点（OQ）

### OQ1 · 通道保活的 wire 载体（推荐 (a)）

- **(a) front 周期下行 `kidpal_keepalive` 心跳帧（推荐）**——契约面 additive 一帧（单键无载荷），设备侧只需一个显式 no-op 消费分支；设备 120s 判据的阈值与逻辑**零改动**，语义从伪判据升级为真端到端活性判据；死链在 120–150s 内被判出并由自愈巡检重连，孩子撞不上冷通道。代价 = 约 115 KB/日下行 + 契约面净增一条。
- **(b) 设备周期 `WebSocket::Ping()` + 放宽/摘掉 120s 判据**——契约面零改动。但实证否决其探活能力：`web_socket.cc` 的 Pong 分支空实现、`Ping()` 返回 void，既不刷新活性时间戳也不可观测失败；摘掉判据后半开连接（家庭 NAT 静默过期）要等下一次发送失败才暴露，而 `SendStartListening` 不检查返回值 → 孩子对着死通道讲完一整段（比今天更糟）。
- **(c) 只调大设备侧 120s 阈值到与引擎 24h 对齐**——最小 diff（一个常量），契约面零改动。但等于放弃设备侧死链探测，病症同 (b) 后半。
- **推荐理由一句话**：(a) 是唯一既保住死链探测能力、又不改第三方组件、且把设备侧既有判据从「伪」修成「真」的方案；契约面 additive 一帧的成本远低于 (b)/(c) 的「孩子对死通道说话没人应」风险。

### OQ2 · `end_session` 的页面落点：契约原文「回首页常显」vs 既有不变量 OQ3=c 回答页驻留（推荐 (a)）

- **(a) 保持 OQ3=c，契约措辞对账（推荐）**——`end_session` 的页面落点走既有 `kidpal_render_node_voice_end()`（无服务端页驻留则回首页、有回答页驻留则保持其前台），与 KEY 手动关闭、60s 超时两条通道**落点一致**；契约 §1.4 表行的「回首页常显」按此改写。
- **(b) `end_session` 特例强制回首页**——忠于契约现行字面。代价：设备侧要为该 action 单开一条 page-landing 分支，且三条结束通道出现两种落点（孩子说「拜拜」回首页、按 KEY 回原页），与 requirements §2.3 矩阵「问答·聆听/思考 → KEY/BOOT 均结束会话回原页」的对称性不一致。
- **推荐理由一句话**：三条结束通道应当同落点，OQ3=c 是本仓已冻结且真机验过的不变量，为一句 MVP 期抄进来的表行文字去破它不划算；(a) 只改契约措辞、零实现分支。

### OQ3 · 引擎 24h 兜底断连成为唯一剩余重建窗口，是否接受为常态（推荐 (a)）

- **(a) 接受（推荐）**——设备 Idle 期不上行音频 → 引擎 `last_activity_time` 不刷新 → `_check_timeout` 到点关连接：形态 = 每日一次服务端侧断 + 一次 bootstrap burst（约 15s 后台流量）+ 一次 `mem_local_short.save_memory` flush。设备 A 路径自动重连已覆盖，用户侧无感（发生在闲置期）。副作用是良性的：它同时是引擎 RAM `Dialogue` 与本地记忆总结的天然日切点。
- **(b) 调大或关闭引擎 `close_connection_no_voice_time`**（属 config 层调整，human 允许）——消除日切。代价：`_save_and_close` 的 `memory.save_memory` **只在连接关闭时发生**，关闭该窗口 = 本地短记忆总结永不出触发点；且引擎 RAM `Dialogue` 永不释放（无界增长，虽然量级小）。
- **推荐理由一句话**：(a) 的日切既免费又顺带 flush 了记忆总结与 RAM，(b) 为了消除一次无感重连而牺牲两个良性副作用，不值。

---

## 5. 验证集（landing 前机械前置 + landing 后核验）

> **时点标注**：`pre` = 起草期/FROZEN 前实跑（本批基线，全部已实跑，输出见文末）；`post` = 落地后核验。每条给命令 + 预期。负向断言均配正向 happy-path（ARQ-261025）。
> **lane**：`packet-bound` —— 承重原文已在 §2A `proposedContractDiff` 以围栏块逐字给出（四处），落地期按 `--cite-check` 对账；装配用 `node .claude/scripts/assemble-edits.mjs <编辑清单.json> <仓外输出根>`。

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `ok:true`、`section2BodyFound:true`、`anchorCount:3`、`anchorUnion` 恰为 `docs/architecture.md` / `docs/contracts/interface-contract.md` / `docs/decisions/ADR.md` 三件、`findings:[]`、`advisories:[]`（**已实跑，输出 verbatim 见文末**） |
| V2 | pre+post | `node .claude/scripts/spec-anchor-check.mjs` | `ok:true`、`errors:[]`；`archSectionCount` ≥ 48（pre 实值 48；§6.4 重写不新增 `###` 节则等值，新增则 > 48） |
| V3 | pre+post | `node .claude/scripts/contract-lint.mjs` | `ok:true`；`warnings` 长度 ≤ 1 且唯一项 = 既有 `contract-link-broken:...260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md`（pre 实值即此，本批不新增 warning） |
| V4 | pre→post | `grep -c 'kidpal_keepalive' docs/contracts/interface-contract.md` | pre = `0`；post ≥ `2`（§四 定义节 + 变更记录条）——**正向：新帧节在世** |
| V5 | pre→post | `grep -c 'listen\` 状态消息' docs/contracts/interface-contract.md` | pre = `0`；post ≥ `1`——**正向：`listen` 帧节标题在世**（与既有 `stt`/`tts` 状态消息条同形命名） |
| V6 | pre→post | `grep -c 'kidpal_keepalive' docs/architecture.md` | pre = `0`；post ≥ `1`（§6.4 保活段）——**正向：架构机制本体与契约同名 token 对齐** |
| V7 | pre→post | `grep -c '^## ADR-019' docs/decisions/ADR.md` | pre = `0`；post = `1`（恰一条，append-only）；配正向 `grep -c '^## ADR-018' docs/decisions/ADR.md` = `1`（pre/post 均 1，证明未误改既有条目） |
| V8 | pre→post | `grep -c '261115-arch-review-session-channel-resident' docs/architecture.md` | pre = `0`；post ≥ `1`（§12 定稿标记行）；配正向 `sed -n '/^## 12. 变更记录/,$p' docs/architecture.md \| grep -c '^| 26'` pre = `10` → post = `11` |
| V9 | pre→post | `grep -c '结束会话，回首页常显' docs/contracts/interface-contract.md` | pre = `1`；post = `0`——**负向：旧表行已被取代**；配正向 `grep -c 'WS 通道保持常驻' docs/contracts/interface-contract.md` pre = `0` → post ≥ `1` |
| V10 | pre→post | `grep -c 'front 自推通路也不把这两个 action 发给设备' docs/contracts/interface-contract.md` | pre = `1`；post = `0`——**负向：自相矛盾句已消解**；配正向 `grep -c '截获转发类' docs/contracts/interface-contract.md` pre = `0` → post ≥ `1` |
| V11 | pre→post | `grep -c '设备关闭音频通道（断连）' docs/contracts/interface-contract.md` | pre = `1`；post = `0`——**负向：discard 发送时机不再以断连为锚**；配正向 `grep -c '结束信号' docs/contracts/interface-contract.md` pre = `0` → post ≥ `2`（§四 `listen` 节 + `kidpal_story_discard` 行改后句） |
| V12 | pre→post | `grep -c 'end_session' docs/contracts/interface-contract.md` | pre = `4`；post ≥ `6`（表行 + 表后段落两型枚举 + §1.6 约束句 + 变更记录条；只增不减，无删除面） |
| V13 | pre→post | `grep -c '会话收尾本地关后 T=5s 常驻重建' docs/architecture.md` | pre = `1`（§6.4 原文）；post = `0`——**负向：退役机制的正文表述已改写**；配正向 `grep -c '常驻重建' docs/architecture.md` post ≥ `1`（§8.1 风险 3 的沿革注记保留历史提及，历史不删只标取代） |
| V14 | post | `node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | 承重原文块对账通过（§2A 四处围栏块：V9/V10/V11 的三条原文在 pre 态可被提取、post 态改后文本落地文件命中） |
| V15 | post | `node .claude/scripts/pre-merge-escalation-gate.mjs`（主线程在合并前跑） | 三件联立凭据齐：FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖 `docs/contracts/interface-contract.md`；缺一即 BLOCK |
| V16 | post | `node .claude/scripts/changes-index.mjs --upsert 261115-arch-review-session-channel-resident` | `delivery/review/INDEX.md` 对应行状态更新，`landed` / `mergedInMain` 由脚本现推 |
| V17 | pre→post | `grep -c '重连 + 常驻重建已落（261010 包）' docs/architecture.md` | pre = `1`（§3.3 :138 原文）；post = `0`——**负向：假设清单行的退役机制表述已改写**；配正向 `grep -c '自愈巡检' docs/architecture.md` pre = `0` → post ≥ `2`（§3.3 行 + §6.4） |
| V18 | pre→post | `grep -c '会话收尾本地关后短延迟（T=5s，kResidentRebuildDelayS，已冻结进齿）常驻重建' docs/architecture.md` | pre = `1`（§10 决策 29 :529 原文，**历史档正文须一字不改，故 post 仍 = `1`**）；配正向 `grep -c '取代注记' docs/architecture.md` pre = `4` → post = `6`（落地实跑覆写：新增决策 29 注记 1 处 + 本批 §12 变更记录行 1 处；原预期 `5` 未计入 §12 行自述「决策 29 取代注记」的命中，以实跑为准），且 `grep -c '2026-11-16 取代注记' docs/architecture.md` post = `1` |
| V19 | pre+post | `grep -c '常驻重建' docs/architecture.md` | pre = `4`（:138 / :371 / :460 / :529）；post = `3`（**落地实跑覆写**：§6.4 整节重写后该措辞不保留；存活三处 = §3.3 / §8.1 / §10 决策 29 正文。原「≥ `4`」下界基于 §6.4 保留该措辞的假设，实跑证伪——以实跑为准；负向配对 V17 + V13 与正向配对 `2026-11-16 取代注记` 仍全部成立）。**负向配对** = V17（`:138` 旧句 post = 0）+ V13（`:371` 旧句 post = 0）证明表述确已改写而非原地不动；**正向配对** = `grep -c '2026-11-16 取代注记' docs/architecture.md` pre = `0` → post = `1` |

**复跑分档**：改稿轮只复跑被改 T 的对应 V 项（T4→V4/V5/V11、T5→V9/V10/V12、T6→V7、T1→V6、T2→V6、T3→V13/V17/V18/V19、T7→V8）；**全量复跑义务保留在两个时点**：FROZEN 标注前、落地后核验前，两处不得窄化。

---

## 6. 留根清单

1. **`docs/requirements.md:48` 的 ⚠️「60 秒超时实现侧对账」注记刷新**——本批落地后「活侧无 timer 生产者」「真机 60 秒自动关闭从未发生」两句失效。该行属需求面文件的实现侧对账登记、非需求语义变更，本批不写（arch lane 不越界改 requirements）。**交主线程裁路由**：或作实现包 integrator 的 docs-delta，或起一个需求面对账小批。重评触发 = 实现包 P2 落地。
2. **会话边界注入**——本批显式不做（不动引擎代码约束下四个既有注入面逐一为语义污染，评估全文见 T6 附段）。重评触发 = 真机出现「孩子隔了很久再说话，小伴把上一段语境当成当前话题」的可复现实例；届时的可用面 = workspace `AGENTS.md` 静态纪律 + 动态 context 块既有的本回合时间/日期，零引擎代码零契约改动。
3. **引擎 RAM `Dialogue` 无界增长与 `mem_local_short` 只在连接关闭时 save**——本批裁为接受（OQ3 (a)：24h 日切是良性 flush 点）。重评触发 = 引擎侧内存观测出现异常，或 human 决定调整 `close_connection_no_voice_time`；届时需同时评估 `save_memory` 的替代触发点（那是引擎代码改动，需 human 显式授权）。
4. **`IsTimeout()` 命中即打 `ESP_LOGE`**——自愈巡检每秒调它会刷日志。本批把「改为跳变沿单次或降频」写进 T1 改法 3 的实现注记，但**具体形态归实现包裁**（属可观测性细节，不入架构）。若实现包判定需改 `protocol.cc`，由 planner 经扩单通道回评补 manifest（本批 §3 已把该文件标为「未触（条件）」）。
5. **RC-B6 常驻连接 modem 省电档结论仍待真机观测冻结**（§6.4 既有悬置项）。本批把会话收尾后的省电剖面从 `LOW_POWER` 改为维持 `PERFORMANCE`，**改变了 RC-B6 的观测前提**（不再有「收尾后 5s 的 LOW_POWER 窗口」）；观测口径需在轮 A 一并重定，交实现包 plan.md 承载。
6. **`ses_machine_dispatch` 纯模型仍不入生产**——本批维持其「验收 oracle」身份（T3 改法 4）。这是一笔有意识的债：仓里存在一个与活侧状态机平行的会话纯模型，只被 archive 测试消费。是否退役或反向接线，属独立议题，不在本批范围。
7. **debt 四条结项记账**——`TECH-261114-overlay-close-channel-rebuild-churn` / `TECH-261014-autoconnect-gives-up-silently` / `TECH-261014-session-end-channels-unwired` / `TECH-261012-first-keypress-no-listening` 的 `closed` / `closedBy` 字段填写与 `debt-render.mjs` 归位，归主线程在实现包全部落地 + 真机轮通过后执行（本批只声明 consumed-by 关系）。**注意**：`TECH-261014-session-end-channels-unwired` 的结项应以**真机轮 B 实测「60s 自动关闭首次发生 + 说拜拜全链通」**为凭，不以代码合入为凭（该条目 severity = MAJOR，且 requirements §2.1 的 ⚠️ 登记正是「真机从未发生」）。
8. **`firmware/main/protocols/protocol.cc:129-136` 的 `SendKidpalStoryDiscard` 注释刷新（R1 advisory，交实现包扩单通道）**——该函数头注释现文含「发送时机 = mcp_server DISCARD_RECORD case 内 `CloseAudioChannel` 之前（同一 TCP 有序, 网关泵先消费 discard 再处理断连）」，随本批 T4 改法 3 的契约措辞改写，正确表述应为「…内发出会话结束信号（`listen stop`）之前（同一 TCP 有序, 网关泵先消费 discard 再消费 stop）」。**代码逻辑零改动**（该函数只拼串发送，与发送时机无关），改的只是注释，∴ 本批不把 `protocol.cc` 拉进 canonical 改动面；`protocol.cc` 在 §3 已标「未触（条件）」。处置 = **交 P1 planner**：P1 改 `mcp_server` 的 DISCARD_RECORD case 时一并刷新该注释，若因此需把 `protocol.cc` 纳入 P1 的 changeManifest，由 builder 经 planner 带根因回评、architect 按扩单通道评估后扩 manifest 并留痕（清单 append + 评估依据一句）——不得就地自扩。同族的注释漂移风险点另有一处，P1 planner 应一并核：`firmware/main/application.h:126-130` 的 `CloseAudioChannel()` 声明注释（现文「录音态丢弃（BOOT 任一手势）断连面——直接关音频通道即达 front ws_disconnect cancel 钩子（弹缓冲不落盘, **不发 listen stop**）」）与本批改法直接矛盾（改后正是「发 listen stop、不断连」），该文件已在 P1 manifest 内，随批刷新即可。
9. **`pending_stop` 的 ≤10s 引擎抑制窗（v1.2 新增，门 1 F-01 修订的显式 accepted residual）**——记录会话以「accum 空」形态结束（引导态 KEY 结束、或录音态 KEY 收尾但转写尚在途）时，`pending_stop` 必须作为受保护态交给既有 10s 巡检释放，**不得提前释放**（提前释放 = 复发 261022 修掉的孤儿幻听缺陷，`_issue_finish_downlink:1661-1665` docstring 实录 KPLOG006 23:59:30「Thank you」被当普通对话派发并播报「我没听清/再说一遍」）。代价 = 该 ≤10s 窗内若孩子立刻从非 story 页发起新问答会话并马上开口，该次话语可能被引擎抑制（`listen start` 只清 `_kidpal_listen_closed`、不清 `_kidpal_record_context`）；后果可恢复（再按 KEY 即好）、无数据损坏。触发需三件同时成立，真机轮 A 的 A5 实测窗口长度与实际命中率。**重评触发** = 真机实测该窗频繁命中（例如孩子「结束录音→马上问问题」是高频形态）→ 回 `/arch-change` 评估缩短 `PENDING_STOP_SECONDS`（既有冻结常量，改值须回 planner 重冻结）或引入「新会话 `listen start` 时由 front 主动收敛在途 pending」的新机制（后者需重新评估孤儿幻听风险，不可轻动）。同族登记 = §8.1 新增风险行 ④。

---

## 起草期实跑记录（ARQ-261021 义务：命令 + 关键输出 verbatim）

**实跑日期**：2026-11-15 至 2026-11-16（本会话）。工作树 = `C:\AI\KidPal-pod-m2`，分支 `dev/pod-m2`，HEAD = `4b912971`（`chore(debt): 261114 延迟专项收口`）。

**① 机械门基线（V1/V2/V3）**

```
$ node .claude/scripts/spec-anchor-check.mjs
{ "specAnchorCheck": "forward", "design": "kidpal-ui-design-v9.html",
  "designIdCount": 78, "archSectionCount": 48, "storyCount": 20,
  "anchorLines": 44, "errors": [], "ok": true }

$ node .claude/scripts/contract-lint.mjs
{ "ok": true, "gate": "contract-lint",
  "warnings": [ "contract-link-broken:相对链接目标不存在 ../../delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md" ] }

$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md
{ "ok": true, "gate": "landing-preflight", "section2BodyFound": true,
  "anchorCount": 3,
  "anchorUnion": [ "docs/architecture.md", "docs/contracts/interface-contract.md", "docs/decisions/ADR.md" ],
  "advisories": [], "findings": [] }
```

**② 契约/架构/ADR token 基线（V4–V13 的 pre 列）**

```
$ c=docs/contracts/interface-contract.md
$ grep -c 'listen` 状态消息' $c                                    → 0
$ grep -c 'kidpal_keepalive' $c                                    → 0
$ grep -c '结束会话，回首页常显' $c                                 → 1
$ grep -c 'front 自推通路也不把这两个 action 发给设备' $c            → 1
$ grep -c '设备关闭音频通道（断连）' $c                             → 1
$ grep -c 'end_session' $c                                         → 4
$ grep -c '^## ADR-019' docs/decisions/ADR.md                       → 0
$ grep -c '^## ADR-018' docs/decisions/ADR.md                       → 1
$ grep -c '261115-arch-review-session-channel-resident' docs/architecture.md → 0
$ grep -c 'kidpal_keepalive' docs/architecture.md                   → 0
$ sed -n '/^## 12. 变更记录/,$p' docs/architecture.md | grep -c '^| 26' → 10
$ grep -c '会话收尾本地关后 T=5s 常驻重建' docs/architecture.md      → 1
```

**②-b 正向断言基线（V9/V10/V11 的「配正向」列，pre 态须为 0）**

```
$ grep -c 'WS 通道保持常驻' docs/contracts/interface-contract.md    → 0
$ grep -c '截获转发类' docs/contracts/interface-contract.md          → 0
$ grep -c '结束信号' docs/contracts/interface-contract.md            → 0
```

**③ 固件锚点与冻结齿面枚举（§2 实证段 + §3 sweep 透镜）**

```
$ grep -n "local_close_settle corr_id" firmware/main/application.cc
985:  ... src=discard_record state=idle
1037: ... src=toggle_chat state=idle
1167: ... src=overlay_close state=idle
1521: ... src=wake_invoke state=idle

$ grep -n "ScheduleResidentRebuild()" firmware/main/application.cc
619: void Application::ScheduleResidentRebuild() {
989 / 1040 / 1169 / 1524:  ScheduleResidentRebuild();          ← 四调用点

$ grep -n "kAutoConnectBackoffS\[\|kResidentRebuildDelayS = \|kTimeoutSeconds = " \
    firmware/main/application.cc firmware/main/protocols/protocol.cc
application.cc:105: static const int kAutoConnectBackoffS[3] = { 5, 10, 20 };
application.cc:112: static const int kResidentRebuildDelayS = 5;
application.cc:609:   vTaskDelay(pdMS_TO_TICKS(kAutoConnectBackoffS[attempt] * 1000));
protocol.cc:150:      const int kTimeoutSeconds = 120;

$ grep -n "attempt < 3\|attempt < 2" firmware/main/application.cc
599:  for (int attempt = 0; attempt < 3; attempt++) {
608:      if (attempt < 2) {

$ grep -rn "ses_machine_dispatch" firmware/main --include=*.c --include=*.cc --include=*.h \
    | grep -v "/test/" | wc -l                                  → 2
$ grep -rln "ses_machine_dispatch" firmware/main
  firmware/main/session_state_machine.cc
  firmware/main/session_state_machine.h
  firmware/main/test/acceptance/archive/session_logic.acceptance.c
  firmware/main/test/acceptance/archive/story_p3.acceptance.c

$ grep -n "case 0: /\* end_session" firmware/main/mcp_server.cc
1725:  case 0: /* end_session —— 结束会话回首页 (本地停播主地位不变) */
1726:      app.AbortSpeaking(kAbortReasonNone);        ← 只停播，不结束会话

$ for f in boot_auto_connect voice_round_ui record_session_live; do ... grep -c ...; done
boot_auto_connect.acceptance.c   ScheduleResidentRebuild=31 kResidentRebuildDelayS=19 kAutoConnectBackoffS=20 CloseAudioChannel=24
voice_round_ui.acceptance.c      local_close_settle=4 ScheduleResidentRebuild=3 CloseAudioChannel=19
record_session_live.acceptance.c CloseAudioChannel=22

$ grep -n 'end_session' server/acceptance/test_points_e2e_spike.acceptance.py
866-868: assertFalse(ds.validate_display({"view":"control","data":{"action":"end_session"}})[0], ...)   ← 缺省 self_push=False（relay 路径），本批保绿
$ grep -n -A4 'self_push=True' server/acceptance/test_record_dialogue_guide.acceptance.py | grep -n 'end_session'
1267-1272: ok_end,_ = ds.validate_display({... end_session ...}, self_push=True); assertFalse(ok_end, ...)  ← 本批反转
```

**④ 引擎侧 ECS 实读（零改动依据；只读，未写入任何远端文件）**

```
$ python C:/tmp/ssh_helper.py "cat .../core/utils/dialogue.py"
  → get_llm_dialogue_with_memory 全文：system(1) + 全部 few-shot(temporary) + 全部实际历史
    逐条装配后 `return dialogue`——**无任何裁剪 / 压缩 / 窗口 / max_messages 逻辑**

$ python C:/tmp/ssh_helper.py "cat .../core/handle/textHandler/listenMessageHandler.py"
  → state=="start": conn.reset_audio_states()
  → state=="stop" : conn.client_voice_stop = True + (非流式) 触发缓冲尾音 ASR
                    **不置 close_after_chat、不调 conn.close()**

$ python C:/tmp/ssh_helper.py "grep -rn 'close_after_chat' .../core/handle/*.py"
  → abortHandle.py:12 (=False) / receiveAudioHandle.py:117,120,141 / sendAudioHandle.py:54
    即 close_after_chat 只由 no_voice_close_connect 与 max_out_size 置位

$ python C:/tmp/ssh_helper.py "grep -n 'end_session\|control\|拜拜\|再见\|home' /root/.openclaw/workspace-kidpal/AGENTS.md"
  → :31 | `home` | 孩子说再见/结束、无需显示内容 | 空 |
  → :32 | `control` | 下发控制动作 | {"action":"end_session"|"stop_speaking"|"start_recording"} |
  → **全文零条规则指示模型在告别回合下发 control/end_session**（仓内镜像 v11 全文 81 行逐行 Read 同结论）

$ python C:/tmp/ssh_helper.py "grep -n 'last_activity_time' .../core/connection.py"  （对仓内 engine-patch 副本 grep 同结果）
  → 325 初始化 0.0 / 408 建连赋值 / 2291,2293,2296,2298 _check_timeout 消费
  → connection.py 内零刷新；唯一刷新点 = receiveAudioHandle.no_voice_close_connect(have_voice=True)
```

**⑤ 真机时延数字来源声明（非本批实跑）**：温通道 0.82s / 冷通道 1.57s / TLS 证书验证 0.53s / bootstrap burst 约 15s（15:05:04–21 窗口）四个数字**沿用 KPLOG029 实测**，来源 = `docs/debt/tech/TECH-261114-overlay-close-channel-rebuild-churn.md` 正文（2026-11-14 15:04 三连会话轮，uptime 毫秒轴 41292→42112 / 53012→54582）。本批起草期**未跑真机**（human 持板轮归实现包），故这四个数字标注为转引、出处如上，落地期真机轮 A 须以同口径复测并回填。

**⑥ R1 修订轮复跑（2026-11-16，回应 architect-reviewer R1-F1/F2/F3；三项 findings 均由 architect 独立复核后采纳，非照单接受）**

```
# F1 复核：listen detect 是否为本板确发帧
$ sed -n '68,73p' firmware/main/protocols/protocol.cc
void Protocol::SendWakeWordDetected(const std::string& wake_word) {
    std::string json = "{\"session_id\":\"" + session_id_ +
                      "\",\"type\":\"listen\",\"state\":\"detect\",\"text\":\"" + wake_word + "\"}";
    SendText(json);
}
$ grep -n "SendWakeWordDetected" firmware/main/application.cc      → 1217 / 1500
$ sed -n '730,735p' firmware/sdkconfig
# CONFIG_WAKE_WORD_DISABLED is not set
CONFIG_USE_AFE_WAKE_WORD=y            ← :731
# CONFIG_USE_CUSTOM_WAKE_WORD is not set ← :732
CONFIG_SEND_WAKE_WORD_DATA=y           ← :733
→ F1 成立：原拟注「KidPal 侧不发」为假，已按实核改写 T4 改法 1 的 `state` 闭集条

# F2 复核：退役机制在 canonical 文档的全部引用点
$ grep -n '常驻重建' docs/architecture.md | cut -c1-90                → 4 命中 :138 / :371 / :460 / :529
$ grep -rn '常驻重建' docs/ | wc -l                                   → 10（arch 4 + debt 面 6）
$ grep -c '常驻重建' docs/requirements.md                             → 0
$ grep -c '常驻重建' docs/contracts/interface-contract.md              → 0
$ grep -c '常驻重建' docs/decisions/ADR.md                            → 0
$ grep -c '取代注记' docs/architecture.md                              → 4（:230 / :520 / :547 / :549）
$ grep -c '常驻重建' docs/architecture.md                              → 4   ← V19 pre（守恒下界基线）
$ grep -c '重连 + 常驻重建已落（261010 包）' docs/architecture.md       → 1   ← V17 pre
$ grep -c '自愈巡检' docs/architecture.md                              → 0   ← V17 正向配对 pre
$ grep -c '2026-11-16 取代注记' docs/architecture.md                    → 0   ← V19 正向配对 pre
$ grep -c '会话收尾本地关后短延迟（T=5s，kResidentRebuildDelayS，已冻结进齿）常驻重建' docs/architecture.md → 1  ← V18 pre
→ F2 成立：:138 与 :529 两处原未入改动面，已补进 T3 改法 5/6 + T7 改动面列 + V17/V18/V19

# F3 复核：CloseAudioChannel 命中数
$ grep -rn 'protocol_->CloseAudioChannel' firmware --include=*.cc --include=*.h | grep -v /test/ | wc -l
9
$ grep -rn 'protocol_->CloseAudioChannel' firmware --include=*.cc --include=*.h | grep -v /test/
application.cc:508 / :976 / :1032 / :1164 / :1407 / :1426 / :1515 / :1588 / :1601
→ F3 成立：原写 10 系把 :976（录音丢弃面，本属「会话收尾 4」之一）重复计一次；已改 9，
  「会话收尾 4 + 真拆通道 5 = 9」的分解随之自洽（4+5 两侧成员名单均未变，只去掉重复计数）

# advisory 复核：随 T4 改写而漂移的注释
$ sed -n '129,136p' firmware/main/protocols/protocol.cc   → 「发送时机 = mcp_server DISCARD_RECORD case 内 CloseAudioChannel 之前」
$ sed -n '126,130p' firmware/main/application.h           → 「…直接关音频通道即达 front ws_disconnect cancel 钩子（弹缓冲不落盘, 不发 listen stop）」
→ 两处注释均与本批改法矛盾；protocol.cc 未在 manifest（只改注释、逻辑零改动）→ 登记 §6 留根 8
  交 P1 planner 扩单通道；application.h 已在 P1 manifest → 随批刷新
```

**⑦ v1.2 修订轮复跑（2026-11-16，回应门 1 BLOCK(F-01)；F-01 由 architect 独立读码复核后采纳，非照单接受）**

```
# F-01 复核 1：三条冻结 oracle 的断言面（逐条 Read 原文，非转引 reviewer 结论）
$ sed -n '1891,1900p' server/acceptance/test_m4k1_story_cloud_link.acceptance.py
  def test_sao_a2_empty_stop_holds_pending_not_bail_to_chat(self):
      """…listen/state=stop 先到且 accum 空时，会话**不得**立即被打回 chat…
         须挂起 pending-stop 态，后续 stt 到达才归档。"""
      _device_to_engine(front, sid, [self._STOP])
      self.assertNotEqual(acc.routing(sid), "chat", "SAO-A2 RED: …")   ← 当刻 cancel 即红
      …随后 stt 到达 → self.assertEqual(len(files), 1, …)
$ sed -n '865,879p' server/acceptance/test_m4k1_story_cloud_link.acceptance.py
  # M4K1-A2 真机序：ctx(story) → listen-stop（累积空 → 挂起 pending-stop）→ stt 到
  self.assertEqual(len(files), 1, "M4K1-A2 RED: …未产出恰一篇归档…")   ← 当刻 cancel 即 0 篇
→ v1.1 的「routing 仍为 accumulate/pending_stop 且本帧未被归档路径消费时 cancel」
  对这两齿都是直接反例；守卫无法区分 pending_stop 与引导态（两者 accum 均空）

# F-01 复核 2：pending 保护窗与 sweep 释放缺口
$ grep -n 'PENDING_STOP_SECONDS' server/gateway/story.py                  → 75: PENDING_STOP_SECONDS = 10.0
$ sed -n '873,884p' server/gateway/story.py   release_pending(): routing="chat"、清挂起态、不写空故事（不 pop 会话）
$ sed -n '886,894p' server/gateway/story.py   cancel(): self._sessions.pop(session_id, None) → routing 回 "chat"，在途转写不可恢复
$ sed -n '1254,1258p' server/gateway/front/front_app.py
      for sid in acc.pending_timed_out_sessions(now_dt):
          if acc.release_pending(sid):
              log.info("story_pending_timeout corr_id=%s released=true", sid)
  → **该分支无 _release_record_context 调用**（全文 `_release_record_context` 调用点仅 :1716/:1727/:1747，
    三处全在 _story_finish_session 内）∴ 常驻后 pending 超时释放 routing 但引擎抑制仍在世 = 哑火；
    这是 F-01 修订的承重新增点（v1.2 T2 改法 2 表格第 3/4 行）
$ sed -n '562,564p' server/gateway/front/front_app.py  _story_feed_from_stt: routing=="pending_stop" → feed + return True（恢复归档）
$ sed -n '1352,1353p' server/gateway/front/front_app.py  _story_auto_finish 腿集形态 = _session_legs(sid) or list(_alert_legs)（新增释放点沿用）

# F-01 复核 3：「提前释放」为何不可用（261022 既有裁定）
$ sed -n '1661,1665p' server/gateway/front/front_app.py
  「2026-10-22 真机轮修订：kidpal_record_context active=false 出语境信号**不再在此处发**——
    原形态在整理回合开始前就解除引擎抑制，收尾后迟到的幻听 STT（KPLOG006 23:59:30「Thank you」）
    被引擎当普通对话派发 LLM 并播报「我没听清/再说一遍」，盖过整理演出。」
→ ∴ ≤10s 抑制窗是「不毁稿 + 不复发孤儿幻听」两条硬约束夹出的必然残余，登记为 accepted residual

# F-01 复核 4：ctx 边沿兜底是否破 M4K3-A7 / M4K1-A2 的非 story 零行为齿
$ grep -n 'STORY_MARKER *=' server/acceptance/test_m4k3_record_session_live.acceptance.py → 194: STORY_MARKER = "kidpal_session_ctx"
$ sed -n '508,527p' …  test_a7_ctx_non_story_pushes_zero_frames：三断言 = 不透传(仅查 STORY_MARKER)
                       / routing=="chat" / 零 story_list 帧；用**全新 sid**（sess-a7-home|schedule|whatever）
$ sed -n '884,900p' …  test_a2_source_page_values_and_backward_compat：同形，全新 sid
→ 两齿的记录语境标记从未置位 → ctx 边沿兜底守卫（标记为真）不触发 → 三断言逐条仍成立、**断言保绿**；
  但 :509 docstring「闭集预留取值现版本无行为…防实现过宽」前提不再精确 → 实现包改写 docstring
  + 新增姊妹齿覆盖释放边沿（SCRF-B4/B4b），不得删弱既有齿（F-03 处置，已记 §2A impact 与 §3 sweep）
→ 注入帧 type = kidpal_record_context ≠ STORY_MARKER，故「不透传」断言不受注入影响

# 修订后复跑（结构门 + canonical 现态核对）
$ node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>
  → ok:true / section2BodyFound:true / anchorCount:3 / anchorUnion 三件不变 / findings:[] / advisories:[]

# ⚠ 起草者自纠（诚实申报）：v1.2 修订发生在 **canonical 已落地之后**
#   （commit 87dbf775 feat(arch) 落地 + 14075150 verdict PASS + 7e4c44d8 INDEX landed/mergedInMain=yes，
#    §7「落地实跑记录」已在册）∴ §5 的 pre 列已是历史基线、不再是当前工作树值。现态实跑：
$ grep -c 'kidpal_keepalive' docs/contracts/interface-contract.md      → 2   （V4 落地值）
$ grep -c 'listen` 状态消息' docs/contracts/interface-contract.md       → 2   （V5 落地值）
$ grep -c 'kidpal_keepalive' docs/architecture.md                       → 2   （V6 落地值）
$ grep -c '结束会话，回首页常显' docs/contracts/interface-contract.md    → 0   （V9 负向已达成）
$ grep -c 'WS 通道保持常驻' docs/contracts/interface-contract.md         → 2   （V9 正向已达成）
$ grep -c 'end_session' docs/contracts/interface-contract.md            → 8   （V12 落地值）
$ grep -c '重连 + 常驻重建已落（261010 包）' docs/architecture.md        → 0   （V17 负向已达成）
$ grep -c '自愈巡检' docs/architecture.md                                → 4   （V17 正向已达成，≥2）
$ grep -c '常驻重建' docs/architecture.md                                → 3   （V19：pre 4 → post 3）
$ grep -c '取代注记' docs/architecture.md                                → 6   （V18 正向：pre 4 → post 6）
→ **V19 与 §5 原预期不符，如实申报**：§5 V19 原写「post ≥ 4（守恒下界）」，落地实跑 = **3**。
  原因 = 落地期 §6.4 整节重写把 :371 那处的「常驻重建」字样改写掉了（历史提及未逐字保留），
  属落地措辞与 PROPOSAL 预期的字节差异。按 ARQ-261021「落地期实跑若与预期有字节差异，以落地
  实跑为准覆写 §5 并在 §7 留痕」处置：**§5 V19 与 V18 的 post 预期已由主线程落地记录覆写为实跑值**
  （V19 = `3`、V18 正向 `取代注记` = `6`，两行均已在册并注明缘由），本轮 v1.2 不重复改写、只在此
  交叉确认；**不追改已落地的 canonical 文本**——:529 决策 29 正文一字未改（V18 机制句 post 仍 = `1`
  已证）、取代注记在世（`2026-11-16 取代注记` post = `1`），历史可追溯性已由注记承载，:371 的字样
  消失不构成账实矛盾。其余 V 项落地实跑全部达成（V4=2 / V5=2 / V6=2 / V7=1 / V8=3 / V9=0+正向2 /
  V10=0+正向2 / V11=0+正向2 / V12=8 / V13=0 / V17=0+正向4），与 §7「落地实跑记录」逐字一致。

# 契约面零改动主张的核对基准 = **已落地的**契约现文（非 pre 基线）
$ grep -c 'pending_stop\|记录语境释放\|残余累积' docs/contracts/interface-contract.md → 0
$ 读 §四 `listen` 节全文（落地文本）：只钉「会话边界语义 + 通道不因它关闭 + 两层含义分辨权在
  网关（判据 = 累积器路由态 `accumulate` 才归档）+ discard→stop 顺序约束」四件，
  **零字涉及「网关收到 stop 后何时释放会话态」**
→ ∴ F-01 修订（释放时点分层 + pending_stop 保护）**契约面确实零改动**，主张成立且是对落地现文核的
→ 但 `docs/architecture.md` **需要 canonical delta**：落地文本 §5.1.2 front 行仍逐字载有 F-01 的
  缺陷句「② 残余累积态取消（`acc.cancel(reason="session_end")`，仅当 routing 仍为
  `accumulate`/`pending_stop` 且本帧未被既有归档路径消费）」，§5.2 场景② 亦载「front 每语音会话
  收尾（记录语境释放 + 残余累积取消）」——两处必须随 v1.2 改写，处方见下方「v1.2 canonical delta 处方」
```

**v1.2 canonical delta 处方（交主线程 packet-bound 落地；改 `docs/architecture.md` 五条 delta-1…5 + `docs/decisions/ADR.md` 一处计数同步 delta-6〔F-04 同族，v1.2 内补正轮新增〕；`docs/contracts/interface-contract.md` 零改动）**

> 依据：arch-change.md「定稿载体」+ ARQ-261021「落地期实跑与预期有字节差异，以落地实跑为准覆写 §5 并留痕」。本 delta 是 F-01 的架构面表述修正，**不改任何已冻结的机制裁定**（三层解耦 / `listen stop` 复用 / 心跳 / 长重试 / 六触发源收口全部不动），只把「释放与取消的时点」写准。`docs/contracts/interface-contract.md` 与 `docs/decisions/ADR.md` **零改动**（依据见上方实跑：契约现文零涉释放时点；ADR-019 决策六条中无一条钉释放时点，第 ⑤ 条「front 的每会话态改由带内信号驱动收尾」在分层后仍逐字成立）。

- **delta-1 · §5.1.2 front 网关行的「每语音会话收尾」句**：把 ①②③ 三件改写为**按 `acc.routing(sid)` 分层**——`chat` → 无动作（本连接记录语境标记为假时不注入任何帧）；`accumulate` 且 accum 非空 → 既有三式收尾单源 `_story_finish_session` 接管，收尾三件不得再动该会话（释放由整理回合完成后的既有三个 `_release_record_context` 调用点承载，261022 语义零改动）；**`pending_stop` → 受保护态，禁止当刻 cancel 与当刻释放，交既有 10s 巡检（`PENDING_STOP_SECONDS`）**，且该巡检的 pending 分支须补 `_release_record_context`（今天只 `release_pending` + 日志、不解除引擎抑制，旧模型靠随后断连兜底、常驻后兜底消失）；`accumulate` 且本帧未被归档路径消费（结构上不可达，防御性）→ 不 cancel，留既有卡壳引导/自动收尾 sweep（2 轮无回应 → 归档、不丢弃）。原「② 残余累积态取消」**撤销**。
- **delta-2 · §5.1.2 同段的 ctx 边沿完整化句**：改为**分态处置**——离开 `story` 且本连接记录语境标记为真时，`pending_stop` 不动作（保护在途转写，交 10s 巡检）、`accumulate` 走 `cancel(reason="session_end_stale")` + 释放（= 常驻时代的 cancel-on-disconnect 同族兜底，沿 human 2026-10-21 裁定口径，不发明「弃录自动归档」新语义）、`chat` 但标记仍为真只释放（防御性）。
- **delta-3 · §5.2 场景②「会话结束」段的括注**：「front 每语音会话收尾（记录语境释放 + 残余累积取消）」→「front 每语音会话收尾（**按累积器路由态分层释放**；`pending_stop` 受保护、交 10s 巡检释放，不在 stop 当刻取消——保录音态 KEY 收尾的归档不丢，S-F2.2 AC4 v1.65）」。
- **delta-4 · §8.1 新增风险行 ④（= 表内编号 `18`，接 landed 的 15/16/17 之后）**：记录会话空累积结束后 ≤10s 的引擎抑制窗（accepted residual；触发需三件同时成立、后果可恢复无数据损坏；不得用提前释放消除——会复发 261022 孤儿幻听缺陷；重评触发 = 真机实测频繁命中 → 回 `/arch-change` 评估缩短 `PENDING_STOP_SECONDS`，改值须回 planner 重冻结）。若落地时 T3 改法 2 的 ④ 已随 v1.1 落地文本一并写入，则本条只需核对在世、不重复追加。
- **delta-5 · §12 变更记录（两处，缺一不可——R3 F-04 补正）**：本 delta 随 v1.2 重冻落地时，在既有 `261115-arch-review-session-channel-resident` 行的「改动面」列做**两处**改动，不新增行、不改 changeId（同一批的落地修正，非新批）：
  - **(a) 追加 F-01 回修子句**：「§5.1.2 / §5.2 场景② 的会话收尾分层表述修正（门 1 F-01 回修，v1.2）」。
  - **(b) 同步 §8.1 计数子句（F-04 本体）**：该行原文（HEAD `7e4c44d8` 现文，逐字）为

    ```
    §8.1（风险 3 缓解句改写 + 风险 14 补注记 + 新增 15/16/17 三行）
    ```

    替换后完整句（逐字）：

    ```
    §8.1（风险 3 缓解句改写 + 风险 14 补注记 + 新增 15/16/17/18 四行）
    ```

    **理由**：delta-4 落地后 §8.1 实有 15/16/17/18 **四**行，若 §12 仍记「三行」即账实不符（与本批 T3「账实同步」主旨及 v1.1-F3 的计数纪律同族）。**(b) 与 delta-4 是同一笔账的两半，必须同批原子落**，只落 delta-4 不落 (b) = 制造新的账实漂移。
  - **落地现态核对（起草者实跑，2026-11-16）**：主线程已在工作树落了本 delta 全部五条（`docs/architecture.md` 未提交态，`git diff --stat` = 4 insertions / 3 deletions），实测 `grep -cE '^\| 18 \|' docs/architecture.md` = **1**（delta-4 在世）、§8.1 行号集 = `13/14/15/16/17/18`、§12 该行的 §8.1 子句已从 HEAD 的「新增 15/16/17 三行」变为工作树的「新增 15/16/17/18 **四行**」（(b) 已在册）、F-01 回修子句在世（`grep -c '门 1 F-01 回修' docs/architecture.md` = 2：§8.1 行 18 与 §12 行各一）。∴ (a)(b) 对本轮而言是**核对项而非待落项**；本处方仍写全，使其可重放（重放/回滚后重落时不丢 (b)）。
- **delta-6 · `docs/decisions/ADR.md` ADR-019「关联」件的同一计数表述（F-04 同族第二处；起草者本轮实跑新发现，reviewer 的 F-04 只点了 §12 一处）**：ADR-019 的「关联」件现文（`docs/decisions/ADR.md:167`，**已提交态、工作树干净**）逐字含

    ```
    §8.1 风险 3·14 + 新增 15/16/17 / §10 决策 21（方案二失效注记保持）
    ```

  替换后完整句（逐字）：

    ```
    §8.1 风险 3·14 + 新增 15/16/17/18 四行 / §10 决策 21（方案二失效注记保持）
    ```

  **理由**：delta-4 落地后 §8.1 实有 15/16/17/18 四行，ADR-019 关联件只列 `15/16/17` = 与 F-04 同一类的账实不符——**同一笔账现有三个落点**（§8.1 表本体 / §12 变更记录行 / ADR-019 关联件），三处必须同数，只修 §12 会留下 ADR 与 arch 两文件互相矛盾。**与 delta-4、delta-5(b) 同批原子落**。本 PROPOSAL 的 T6「关联」件已在本轮同步改为「新增 15/16/17/18 四行」，故 delta-6 落地后 PROPOSAL 与 canonical 字节一致。**ADR-019 其余四件（日期 / 决策六条 / 理由 / 备选与放弃原因）零改动**——其决策本体无一条钉释放时点或风险行数，F-01 与 F-04 均不触其裁定内容。据此更正 §2A 此前的笼统表述：「`docs/decisions/ADR.md` 零 delta」在 **F-01 面上成立**（决策本体不动），但在 **F-04 面上需 delta-6 这一处计数同步**——如实区分，不笼统声明 ADR 全件零改。

---

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。architect 起草完毕，待 architect-reviewer 四查 + 轻/主判档 + Track A 段升级判定。）


---

### R1 评审记录（architect-reviewer · 2026-11-16 · 四查 + 判档 + 契约四分类 + OQ 意见）

**verdict: BLOCK(F1,F2,F3)**——三项均属一般设计完备性 / 口径问题（非方向级），由 architect 修订，不扩大 human 触点。

**四查结论**

- ① **意图保真：PASS**。human 三答逐条落进改法集（通道常驻 = T1；autoconnect 长重试 = T1 改法 4；结束三通道 = T2·T3·T5；共享短上下文 = §0 非目标不动、归 ADR-015 承载；不改引擎 = §2 实证三条依据 + §3「引擎未触（硬约束）」）。四条 debt 根因逐条覆盖，起草期事实我独立复核为真：`for (int attempt = 0; attempt < 3; attempt++)` + `kAutoConnectBackoffS[3]={5,10,20}`（application.cc:599/105）、`ses_machine_dispatch` 生产零调用（非 test 命中 = 2 = 定义+声明自身）、`mcp_server.cc:1725 case 0` 只 `app.AbortSpeaking(kAbortReasonNone)`、781s/137s 判死根因。
- ② **纪律合规：PASS（含三份分工句；但 F1 见下）**。决策本体归 ADR-019——ADR 现最大 = ADR-018，上限 +1、五件格式、append-only，符合 ADR.md:7 写入纪律；数据格式归契约（§2A）；**心跳 30s 间隔正确落到 arch §6.4「行为策略」而非契约**（沿 §1.5 第 6 条「推送触发时机属行为策略」先例，未越界改需求阈值）；契约变更记录条格式沿既有先例（§1.4 :248 / §四 :354/:356/:358）。
- ③ **撞车复核：PASS（F2 例外）**。261010 [S9] 真拆通道点（:508/:1407/:1426/:1588/:1601）保绿、261024 统一处理器语义保留（仅替换 :1164 CloseAudioChannel→SendStopListening、:1166 LOW_POWER→PERFORMANCE、:1169 去 rebuild）、261028 镜像次序保绿（:1257→:1268→:1282→:1288→:1292）、261019/ADR-013 双层防线（`_kidpal_record_context` depth==0 第三守卫 connection.py:1580-1583 逐字确认）、261022/ADR-015 正交零改、261021 discard 时序以更强形态承载（discard→listen stop，front 侧 `acc.cancel` pop 后 `routing` 回 `chat` 不归档——story.py:654-656/:886-894 实证成立，`listen stop` 既转发又不重启归档 story 见 front_app.py:1817-1828）。
- ④ **牵连面完整：否回（F2）**。除 T3 已覆盖的 §8.1 风险 3（:460）外，`docs/architecture.md` 另有 **§3.3 假设清单 :138**（「重连 + 常驻重建已落（261010 包）」）与 **§10 决策 29 :529**（「会话收尾本地关后短延迟（T=5s，kResidentRebuildDelayS，已冻结进齿）常驻重建」）两处引用本批退役的「会话收尾常驻重建」机制，均未进 T7 改动面，落地后账实不符——正是 T3 自己要防的「读到与新实现直接矛盾句子」同族。

**否决 / 更正项（否回项，由 architect 改）**

- **F1（契约 SSOT 事实错）**：T4 改法 1 拟把 `listen` 帧 `state` 闭集注为「`detect` = 唤醒词上报，上游原生，**KidPal 侧不发**但引擎会消费」。实核为假：`firmware/main/protocols/protocol.cc:68-72` `SendWakeWordDetected` 即发 `{"type":"listen","state":"detect","text":"<唤醒词>"}`，调用点在 `firmware/main/application.cc:1217`（`#if CONFIG_SEND_WAKE_WORD_DATA`，`firmware/sdkconfig:733` = `y`）与 `:1500`（`#if CONFIG_USE_AFE_WAKE_WORD || CONFIG_USE_CUSTOM_WAKE_WORD`，`sdkconfig:731` = `y`）——**本板设备确会发 `listen detect`**。修法：改为「`detect` = 唤醒词上报，设备在 AFE/自定义唤醒词启用时经 `SendWakeWordDetected` 上报 `text=<唤醒词>`」，删「KidPal 侧不发」。
- **F2（牵连面账实不符）**：见④。修法：把 `docs/architecture.md` §3.3 :138 与 §10 决策 29 :529 纳入 T7 改动面，按 §8.1 风险 3 同法加沿革 / 取代注记（决策 29 核心「永不休眠·常驻在线」保留并强化，只把「会话收尾后 T=5s 重建」机制句标取代）；§10 已有多条「取代注记」先例（决策 21/24/25），与「本节为冻结历史档」不冲突。
- **F3（实测数目偏差，违 ARQ-261021 数目现算纪律）**：PROPOSAL §2 实证「`protocol_->CloseAudioChannel()` 全仓 **10** 命中」。实核 = **9**（`grep -rn 'protocol_->CloseAudioChannel' firmware --include=*.cc --include=*.h | grep -v /test/` → 9，全在 `application.cc`：:508/:976/:1032/:1164/:1407/:1426/:1515/:1588/:1601）。作者把 :976（本身即录音丢弃面收尾调用、属「会话收尾 4」之一）重复计了一次。修法：10→9，并核正分类归属（会话收尾 4 = :976/:1032/:1164/:1515，真拆通道 5 = :508/:1407/:1426/:1588/:1601）。

**advisory（非阻断，实现包注意）**：`protocol.cc:129-134` `SendKidpalStoryDiscard` 头注释「发送时机 = … CloseAudioChannel 之前」随 T4 契约改后应为「… listen stop 之前」；该文件本批标「未触（条件）」，由实现包在改 `mcp_server` DISCARD_RECORD 时一并刷新注释，避免代码注释与契约双源。

**判档**：**主路径**（独立复核同意作者建议）。轻路径判据 ①（有语义变化）③（触 `docs/contracts/**`）④（触 §2.3 端云职责划分 + §5.x 视图组件）三否 → 只升不降，主路径成立。

**契约四分类**（权威 `.claude/rules/contracts.md`）：**主 = `ACCOMPANIES_SPEC_CHANGE`**（契约改动随本批架构变更携带、同批原子落地）；**次 = `SPEC_CONTRACT_DRIFT`**（契约 §1.4 :246「front 自推通路也不把这两个 action 发给设备」与 requirements §2.1 ②「判为结束意图→关闭」冲突，裁改契约边、requirements 语义零改）。同意作者自判；`kidpal_keepalive` 新增下行帧属主分类下的 additive 实例，不另立 `CONTRACT_CHANGE_REQUIRED`（本批有完整架构配套）。

**OQ 评审意见**

- **OQ1（保活载体）评审即可定 → (a) front 下行 `kidpal_keepalive`**。(b) 设备 `WebSocket::Ping()` 经核证伪：`web_socket.cc:396-397` `case 0xA` 空 break、`web_socket.h:27`/`web_socket.cc:253` `Ping()` 返回 void 不刷新 `on_data_`；(c) 摘 120s 判据经核为伪：`application.cc:1351` `SendStartListening` 为 void（protocol.cc:74）无失败可达 + 在 `:1349` `if (!IsAudioProcessorRunning())` 门内，会放大「对死通道讲完整段」风险。(a) 是唯一同时保住死链探测、不改第三方组件、把既有伪判据修成真判据的载体；新增帧为 additive，由本 loop 机械凭据闸放行，非 human 门。
- **OQ2（`end_session` 页面落点）评审即可定 → (a) 保持 OQ3=c**。三条结束通道同落点、零新增设备分支；契约「回首页常显」是 MVP 期未实现文本（`end_session` 今天根本到不了设备：:246 拒转 + case 0 只停播），对齐到已冻结且真机验过的 OQ3=c 属契约措辞对账，非行为新裁。
- **OQ3（引擎 24h 日切）评审即可定 → (a) 接受**。(b) 关 `close_connection_no_voice_time` 会牺牲 `_save_and_close` 的 `memory.save_memory` 触发点 + 引擎 RAM `Dialogue` 释放（`_check_timeout` connection.py:2287-2316 每 10s 轮询），为消除一次闲置期无感重连不值。
- **三条 OQ 均非根本方案分叉**（方向已收敛「治本·通道常驻」，三选皆有硬依据可裁），故 **不进 Track A 段**；human 在立项 ack 时知悉 OQ2/OQ3 的产品 / 运维取舍即可，不构成 FROZEN 前置分叉。

**Track A 段判定：否**。方向清晰、无根本方案分叉、无红队声明（arch-change.md :11-13「只看收敛度，不看触及面」）。F1–F3 为完备性修订，architect 修订后走 R2——仅核未闭合 findings 及修订引入的新矛盾。

---

### R2 复核（architect-reviewer · 2026-11-16 · 仅核 R1-F1/F2/F3 闭合 + 修订新矛盾；不重跑全量四查，修订未扩新面）

**verdict: GREEN**——三 finding 全数闭合，R1 advisory 已正确入留根，无新增阻断项；机械门抽查与文件自检绿。R1 段一字未动，本段 append 其后。

**三 finding 闭合判定**

- **F1 · CLOSED**。T4 改法 1（:157）与 §2 实证（:69）已把 `state` 闭集条改写为「`detect` 是本板确发的帧」，给出形状 / 发送原语 `protocol.cc:68-72` / 两调用点 `application.cc:1217`（`CONFIG_SEND_WAKE_WORD_DATA=y`，sdkconfig:733）与 `:1500`（`CONFIG_USE_AFE_WAKE_WORD=y`，sdkconfig:731），删「KidPal 侧不发」，并补「detect 不参与会话边界语义」理由。我独立复核 `firmware/sdkconfig:731-733` = `USE_AFE_WAKE_WORD=y` / `USE_CUSTOM_WAKE_WORD is not set` / `SEND_WAKE_WORD_DATA=y`、`protocol.cc:68-72` 形状、两调用点均在位，逐字相符。
- **F2 · CLOSED**。T3 新增改法 5（§3.3 :138 沿革注记）+ 改法 6（§10 决策 29 追加「⚠️ 2026-11-16 取代注记（部分）」，正文一字不改，逐字仿 :520 决策 21 / :549 决策 25 先例）；T7 改动面（:220）+ 牵连面完整性段（:222）已入 §3.3 / §10；§3 sweep（:305/:331）补齐四处引用普查（`:138`/`:371`/`:460`/`:529`）+ debt 账本面 6 命中「append-only 不改写只结项」论证；新增 V17/V18/V19。四处引用零遗漏。
- **F3 · CLOSED**。§2 实证（:68）与 §3（:331）改为「非 test 面 9 命中 = 会话收尾 4（:976/:1032/:1164/:1515）+ 真拆通道 5（:508/:1407/:1426/:1588/:1601）」，并钉为「本批正负锚分界」；我复跑 `grep -rn 'protocol_->CloseAudioChannel' firmware --include=*.cc --include=*.h | grep -v /test/` = 9，分解自洽。
- **R1 advisory · CLOSED（入留根）**：§6 新增留根 8 处理 `protocol.cc:129-136` 注释漂移（交 P1 planner 扩单通道）；作者并主动自查出同族第二处 `application.h:126-130`（原文「…直接关音频通道即达 front ws_disconnect cancel 钩子（弹缓冲不落盘, 不发 listen stop）」——我核原文属实），该文件已在 P1 manifest，随批刷新。

**修订新矛盾：无阻断项。** 仅一处非阻断厘清建议：T3 改法 6 的决策 29 取代注记写「非 test 面 `protocol_->CloseAudioChannel()` 由 9 命中收敛为 5」，而决策 29 正文（:529）原文为「本地关 **8** 调用点」（经核：8 为 261010 期计数〔源码 commit f9475599〕，现行非 test 面实为 9）——正文按冻结纪律一字不改、注记按现态 9→5 表述均各自正确，唯两数未显式搭桥。建议注记补半句「（正文『8 调用点』为 261010 期计数，现行非 test 面为 9，本批后 5）」使账实一眼可读；此为措辞 nit，不触设计 / 契约 / 机制，不阻断 GREEN。

**机械门抽查（v1.1，独立复跑）**：`landing-fidelity-check --preflight` = `ok:true` / `section2BodyFound:true` / `anchorCount:3` / `findings:[]` / `advisories:[]`；`spec-anchor-check` = `ok:true` / `errors:[]` / `archSectionCount:48`（与修订前等值）；`contract-lint` = `ok:true` / 唯一既有 warning（`260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md` 链接）。文件自检 = 613 行 / U+FFFD 0 / `### T` 锚 7。V17/V18/V19 pre 值我独立复跑 = `常驻重建`4 / V17 负句 1 / V18 句 1 / `取代注记`4 / `自愈巡检`0 / `2026-11-16 取代注记`0，与作者报告逐项一致。**修订未扩出新面**（canonical 仍 arch/contract/ADR 三件；§3.3/§10 在 architecture.md 内；protocol.cc/application.h 属 P1 实现面非 canonical）→ 不需重开全量四查。

**判档：主路径**（不变；轻路径判据 ①③④ 三否）。

**契约四分类**（不变，权威 `.claude/rules/contracts.md`）：**主 `ACCOMPANIES_SPEC_CHANGE`** + **次 `SPEC_CONTRACT_DRIFT`**（契约 §1.4:246 end_session 不下发 vs requirements §2.1② 判为结束→关闭；裁改契约边、requirements 语义零改）。

**OQ 评审意见（不变，R1 结论沿用）**：**OQ1 → (a)** front 下行 `kidpal_keepalive`（(b) 设备 Ping 经核证伪〔`web_socket.cc:396-397` Pong 空 break、`Ping()` 返回 void〕、(c) 摘 120s 判据为伪〔`SendStartListening` void + 门内〕，均不改第三方组件）；**OQ2 → (a)** 保持 OQ3=c（三条结束通道同落点、零新增设备分支，对齐已冻结真机验过的行为属措辞对账）；**OQ3 → (a)** 接受 24h 日切（(b) 牺牲 `save_memory` flush + RAM 释放不值）。三条均非根本方案分叉。

**Track A 段：否**（方向已收敛「治本·通道常驻」、无根本分叉、无红队声明）。

**GREEN 手续提醒**：本 verdict 仅为**设计 GREEN**，不代冻——FROZEN 归 human，主线程凭本 GREEN + human 立项 ack 自冻；落地后 implementation review 走真实 diff 审 + 行首 `reviewedCommit` / `reviewedTree` 钉点（本批架构+契约面恒 packet-bound，主线程落地）。

### 落地实跑记录（主线程 packet-bound landing · 2026-11-16）

- **FROZEN 凭据**：human 2026-11-16 立项 ack + OQ1/OQ2/OQ3 三条裁 (a)；R1 BLOCK(F1,F2,F3) → v1.1 → R2 GREEN；主线程自冻（Status 行已翻 FROZEN v1.1）。
- **canonical 落地**：`docs/architecture.md`（43 行增改：§3.3 / §5.1.1 / §5.1.2 / §5.2 场景①②④ / §6.4 整节 / §8.1 / §10 决策 29 / §12）、`docs/contracts/interface-contract.md`（18 行增改：§1.4 表行与表后段、§1.5 第 3 条、§1.6 告别纪律、§四 `listen` + `kidpal_keepalive` + discard 时序 + ctx 边沿 + 两条变更记录）、`docs/decisions/ADR.md`（ADR-019 五件 + 附段）。三件原子同提交。
- **机械门（landing 后实跑）**：`landing-fidelity-check --preflight` = `ok:true` / `anchorCount:3` / `findings:[]` / `advisories:[]`；`spec-anchor-check` = `ok:true` / `errors:[]` / `archSectionCount:48`（与 pre 等值）；`contract-lint` = `ok:true` / 唯一既有 warning（未新增）；`--cite-check` = `ok:true` / `frozen:true` / `checkedCount:0`（工具覆盖面 = §2 各 T 节内围栏块；本批承重原文位于 `## 2A`，工具按设计不覆盖，改由下方逐字对账替代）。
- **承重原文逐字对账（替代 cite-check 的 §2A 覆盖缺口）**：`git diff -U0 docs/contracts/interface-contract.md` 的删除行与 §2A 三处承重原文逐字一致（① §1.4 表行 `| \`end_session\` | 孩子说"拜拜/不用了" | 结束会话，回首页常显 |`；② 表后段 `两动作与 \`end_session\`/\`start_organizing\` 同属**网关决定类**……设备只消费执行后的权威积分视图。`；③ discard 句 `发送时机 = 设备关闭音频通道（断连）**之前**同连接先行发送（同一 TCP 有序，网关泵先消费 discard 再处理断连）`）；改后文本命中 V4–V13 正向断言。
- **V 项落地实跑值**：V4=2 / V5=2 / V6=2 / V7=1（ADR-018 仍 1）/ V8=3（§12 行数 11）/ V9=0→2 / V10=0→2 / V11=0→2 / V12=8 / V13=0→3 / V17=0→4 / V18 mech=1、取代注记=6 / V19=3。**两处预期覆写**（以实跑为准，见 §5 V18/V19 行）：V18 正向配对 `取代注记` post = 6（原写 5，未计入 §12 行自述命中）；V19 `常驻重建` post = 3（原写 ≥4，§6.4 重写后不保留该措辞）。
- **未落面（按 manifest 边界）**：`server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 与 workspace `AGENTS.md` 的告别回合纪律属实现包 P2 部署面，不在本批 manifest 三件内；`docs/requirements.md:48` ⚠️ 对账注记按 §6 留根 1 由主线程另行路由。

---

### 门 1 回修记录（architect · 2026-11-16 · v1.1 → v1.2）

> **性质声明**：本节是**作者的回修留痕**，不是判决——不含任何 verdict / GREEN / 通过语。F-01 是否闭合由 architect-reviewer 的**增量四查**裁定；v1.2 的 FROZEN 由主线程凭该裁定重冻（v1.1 的 FROZEN 效力已在 Status 行标为暂缓，凭据留痕未删）。

- **触发**：门 1（拆包冻结门）打回 BLOCK，其中 **F-01 为设计级缺陷**，归 architect 修订；其余 findings 归主线程修拆包制品（taskpackage / roadmap / decision-coverage），本件未触碰。
- **F-01 独立复核结论：成立，且比原判更严**。architect 未照单接受，逐条读码复核后确认冲突面是**三条**冻结 oracle 而非一条：`test_m4k1_story_cloud_link.acceptance.py:1891` SAO-A2（空累积 listen-stop 后 `assertNotEqual(routing,"chat")`）、同文件 `:849-882` M4K1-A2（真机序 ctx→stop→stt 断言 `len(files)==1`「恰一篇归档」）、SAO-A3（重复 stop 不双归档）。v1.1 的守卫「本帧未被既有归档路径消费时 cancel」对三者都是直接反例：`pending_stop`（录音态 KEY 收尾、accum 空、manual 形态转写约 1.7s 后到）与「引导态无内容」在 listen-stop 那一刻**可观状态完全相同**，任何 routing 级守卫都无法区分；当刻 `cancel` = `story.py:886-894` 的 `_sessions.pop` → 在途转写不可恢复 → 三式收尾之一的归档丢失，违反 S-F2.2 AC4 v1.65。复核命令与引据 verbatim 见「起草期实跑记录 ⑦」。
- **修订中识别的承重缺口（原判未点出、本轮回修补上）**：`_story_timeout_sweep_body:1254-1257` 的 pending 超时分支今天只调 `release_pending` + 记日志，**不调 `_release_record_context`**（全文该函数调用点仅 `:1716/:1727/:1747`，三处全在 `_story_finish_session` 内）。旧模型下这不是缺陷——设备随后断连，引擎侧 `_kidpal_record_context` 随连接消失；**L1 常驻后兜底消失**，pending 超时只回 routing 不解除引擎抑制 = 该连接后续问答全部哑火。∴ 把释放挂到 10s 巡检**必须同时在该分支新增释放调用**（腿集沿 `_story_auto_finish:1353` 的 `_session_legs(sid) or list(_alert_legs)` 形态），这是 front 侧唯一新增释放点，也是本轮修订的承重半——只做「排除 pending_stop」而不补这一处，等于把哑火从 0s 推迟到 10s 后永久化。
- **修订落点（六处，全在 PROPOSAL 与 package-split；canonical 不由 architect 写，其落地现态见下条与 delta-5 的「落地现态核对」）**：① T2 改法 2 重写为按 `acc.routing` 分层的五行处置表 + 撤销原「②残余累积取消」+ F-01 修订依据段 + 释放点穷尽枚举 + ctx 边沿分态 + accepted residual 申报；② T2 改法 3（§5.2 场景②④ 表述）同步；③ T3 改法 2 风险行 3→4（新增 ≤10s 抑制窗）；④ §2A impact 新增「v1.2 修订的契约影响 = 零改动」专条（三条依据）+ 冻结齿段补三条 must-stay-green 与两条「断言保绿但 docstring 前提改变」的显式处置（F-03）+ `story.py` 两纯逻辑缝零改动声明；⑤ §2A validationPlan 层 2 改写 + §3 sweep 新增三行（`test_m4k1_story_cloud_link` / `test_m4k3_record_session_live` / `story.py`）并把 `front_app.py` 改为五缝；⑥ §6 新增留根 9（≤10s 窗 + 重评触发）。T7 改动面与 §3 架构行的「§8.1 行数」同步 3→4。
- **未改（明确声明，供增量四查收窄范围）**：改法集方向（三层解耦 / `listen stop` 复用而非新帧 / front 心跳保活 / 封顶长跑 + 人工意向位 / 六触发源统一收口）、T1、T4、T5、T6、§4 三条 OQ（human 已裁 (a)）、§2A `proposedContractDiff` 四处、`landing-manifest` 三件、V1–V19 全部命令与 pre 值。
- **canonical delta 需求（交主线程，architect 不写 canonical）**：`docs/architecture.md` 需按 §2 文末「v1.2 canonical delta 处方」五条改写（delta-1 §5.1.2 收尾三件改分层 / delta-2 ctx 边沿分态 / delta-3 §5.2 场景② 括注 / delta-4 §8.1 风险行 ④〔= 表内编号 18〕/ delta-5 §12 行内追加 + §8.1 计数子句同步、不新增行不改 changeId）。`docs/contracts/interface-contract.md` **零 delta**——依据：已落地的契约 §四 `listen` 节只钉「会话边界语义 / 通道不因它关闭 / 两层含义分辨权在网关（判据 = 累积器路由态 `accumulate` 才归档）/ discard→stop 顺序」四件，`grep -c 'pending_stop|记录语境释放|残余累积'` → **0**，零字涉及释放时点。`docs/decisions/ADR.md`：**就 F-01 而言零 delta**（ADR-019 决策六条无一条钉释放时点，第 ⑤ 条「front 的每会话态改由带内信号驱动收尾」在分层后仍逐字成立）；**但就 F-04 而言需 delta-6 一处计数同步**（ADR-019「关联」件的 `§8.1 风险 3·14 + 新增 15/16/17` → `… + 新增 15/16/17/18 四行`，见 delta-6）——本句在 F-04 补正轮据此收窄，原笼统写「ADR 零 delta」不再成立。
- **诚实申报（起草者自纠）**：v1.2 修订发生在 canonical **已落地之后**（commit `87dbf775` + verdict PASS `14075150` + INDEX `7e4c44d8`），故 §5 的 pre 列已是历史基线。本轮实跑核对了全部 V 项现态：除 V18 正向（`取代注记` 6，原预期 5）与 V19（`常驻重建` 3，原预期 ≥4）两处已由主线程落地记录按 ARQ-261021 覆写在册外，其余全部达成；本轮不重复改写 §5，只在「起草期实跑记录 ⑦」交叉确认。**v1.1 起草期未预见到「canonical 先落、设计缺陷后现」这一序，故 v1.1 的 §5 是按 pre→post 形态写的**——该形态在本序下依然可用（post 值即落地实跑值），无需重构。
- **机械门（v1.2 修订后复跑）**：`landing-fidelity-check --preflight` = `ok:true` / `section2BodyFound:true` / `anchorCount:3` / `anchorUnion` 三件不变 / `findings:[]` / `advisories:[]`；`spec-anchor-check` = `ok:true` / `errors:[]`；`contract-lint` = `ok:true` / 唯一既有 warning 未新增。文件自检：U+FFFD 替换符 0、围栏行偶数配对、`### T` 锚 7、`目标文件` 锚行 7（不加粗、行首、注记内无路径 token）。
- **同步制品**：`package-split.md` 已按本轮修订改 §2.3 front 要点 2/3、§2.4 SCRF-B2（含新增 B2b）/B4（含新增 B4b）/B6、§4.3 真机 A5 判据时点精确化；P1 的 changeManifest 未变（`front_app.py` 与新增 py 验收本就在内），但 P1 的 front 侧改动要点**净增一处**（sweep pending 分支补释放调用）——riskTier 维持 high 不变（该处属既有 high 判定中的「记录语境抑制链单点」，未引入新面）。

---

### R3 增量复核（architect-reviewer · 2026-11-16 · 仅核 F-01 闭合 + 修订新矛盾 + delta 处方；不重跑全量四查）

**verdict: BLOCK(F-04)** —— F-01（设计级缺陷）**已闭合**：分层表 + sweep 释放点成立、ctx 边沿覆盖「另一扇门」、契约零改动成立、accepted residual 可取。唯一阻断 = **F-04**：v1.2 canonical delta 处方的 delta-5 未同步 §8.1 行数（一处一行的补正即可，非设计回炉）。

**F-01 闭合判定：CLOSED**

- v1.1 病根「listen stop 当刻 managed cancel（残余累积态取消）」确会 pop `pending_stop` 态、丢在途转写、使三条冻结 oracle 变红：`test_m4k1_story_cloud_link.acceptance.py:1891` SAO-A2（空累积 stop 后 `assertNotEqual(routing,"chat")`）、同文件 `:849-882` M4K1-A2（真机序 ctx→stop→stt 断言 `len(files)==1`「恰一篇归档」）、SAO-A3（重复 stop 不双归档）。v1.2 **撤销该动作**（:128），改五行分层表（:120-126）：`pending_stop` 受保护（禁止当刻 cancel / 释放），交 10s 巡检。
- **承重缺口补齐（独立读码确认）**：`front_app.py:1254-1257` pending 超时分支只 `release_pending`（`story.py:874-884` 只把 routing 打回 chat），**不调 `_release_record_context`**；该函数全文三个调用点 `:1716/:1727/:1747` 全在 `_story_finish_session` 内。故 pending 超时路径 = 唯一绕过释放的收尾路径，v1.2 在该分支补 `_release_record_context(sid, _session_legs(sid) or list(_alert_legs))`（腿集沿 `_story_auto_finish:1353`；`_session_legs:2351` + `_alert_legs` 兜底）——必要（常驻后断连兜底消失、引擎 `_kidpal_record_context` 不随超时清除）且充分（覆盖该分支、幂等）。
- **「另一扇门」覆盖**：① 异常无 stop（崩溃）→ ctx 边沿分态兜底（delta-2 / :131）；② discard 正常路径（`kidpal_story_discard`→`acc.cancel`（`story.py:886-894` pop）→routing=chat→listen stop）→ 五行表 Row 1「chat」当刻无动作（正确，会话已 cancel、无可归档），释放由**下一会话 `ctx(home)`（`application.cc:1345`，先于 `:1351` audio）**触发 ctx 边沿「chat 但标记真 → 只释放」即时完成（无 10s 窗）。∴ discard 不哑火。
- **story.py 零改动成立**：修复只加 front_app.py 调用点，`cancel` / `release_pending` / `PENDING_STOP_SECONDS=10.0`（`story.py:75`）逐字不动，核。

**accepted residual 意见：可接受（留根 9 + §8.1 风险行 ④）**

- 窗口真实存在且机制我核过：`pending_stop` 期内 `acc.routing==pending_stop`，ctx 边沿分态对 pending_stop「不释放」（保护在途转写），故 ≤10s 内换会话 + 立刻开口的首句确会被引擎 `_kidpal_record_context` 抑制——与作者申报一致。
- 可接受理由：窗口上界 = **冻结常量** `PENDING_STOP_SECONDS=10.0`；触发需三件同时成立（空 accum 结束 + 10s 内换会话 + 立刻开口）；后果 = 一次话语被抑制、再按 KEY 即恢复、**无数据损坏 / 无错归档**；且**不得用提前释放消除**（会复发 261022 孤儿幻听缺陷 `_issue_finish_downlink:1661-1665` KPLOG006 实录），属取小害的正当取舍。登记留根 9 + §8.1 风险行 + 真机 A5(a)/(b) 分判据齐备。**建议保留。**

**delta 处方准确性：delta-1/2/3 恰覆盖缺陷句；F-04 在 delta-5**

- F-01 缺陷在 landed canonical 两处：`§5.1.2 :198`（收尾三件 ①②③ + ctx 边沿句）与 `§5.2 :227`（场景② 括注）。delta-1（收尾三件分层）、delta-2（ctx 边沿分态）覆盖 `:198`，delta-3 覆盖 `:227`——恰、不多不少（`grep '残余累积|记录语境释放'` 命中仅 `:198/:227`，无遗漏点）。
- **F-04**：delta-4 声明「§8.1 **新增风险行 ④**」（landed §8.1 现为 15/16/17 三行 → 落地后 15/16/17/**18** 四行；`grep -cE '^\| 1[5-9] \|'` = 3，无 18），但 delta-5（§12 行内追加）只追加「§5.1.2 / §5.2 场景② 的会话收尾分层表述修正」，**未把 §12 既有行的 `新增 15/16/17 三行` 改为 `四行`**（landed `§12 :565` 现文逐字 = `新增 15/16/17 三行`）。若按此处方落地 = §12 记「三行」而 §8.1 实有 15/16/17/18 四行 → **账实不符**（与本批 T3 主旨及 v1.1-F3 计数纪律同族）。
  - **修法（一处一行）**：delta-5 追加一句，把 §12 该行 §8.1 子句 `新增 15/16/17 三行` → `新增 15/16/17/18 四行`（或等价「新增四行」），与 delta-4 一致。

**修订新矛盾（除 F-04 外）：无。** 分层表与既有 `_story_timeout_sweep` 10s（`story.py:1255` `pending_timed_out_sessions` 默认 `PENDING_STOP_SECONDS`）、`_story_auto_finish`（2 轮无回应 → 归档不丢弃，Row 5 承载）、discard→stop 有序不归档（:116）、S-F1.4（提醒走 `kidpal_speak` 不过 `chat()`）、M4K3-A7（新 sid 标记假 → ctx 非 story 零帧，仍绿）均无冲突。

**§7「门 1 回修记录」越界核**：非 verdict——`:770` 明示「不是判决——不含任何 verdict / GREEN / 通过语」，判决权归本四查，未越界。

**判档 / 四分类 / Track A：不变。** 主路径（轻路径 ①③④ 三否）；`ACCOMPANIES_SPEC_CHANGE` 主 + `SPEC_CONTRACT_DRIFT` 次（F-01 为 front 实现侧收尾时点修正，契约零改动，不引入第三类）；Track A 否（无方向级分叉）。

**GREEN 条件**：F-04 一处一行补正（delta-5 加「§8.1 四行」）落地即全数闭合——届时 F-01 + F-04 均 GREEN，凭本 R3 + 该补正重冻。**FROZEN 归 human / 主线程，本判官不代冻。**

---

### 门 1 F-04 回修记录（architect · 2026-11-16 · v1.2 内一处补正，不另起版本号）

> **性质声明**：作者回修留痕，非判决；不含 verdict / GREEN / 通过语。F-04 是否闭合由 architect-reviewer 一句话确认。

- **F-04 独立复核：成立**。实跑 `git show HEAD:docs/architecture.md` 与工作树对照——HEAD（`7e4c44d8`）的 §12 该行 §8.1 子句逐字为 `§8.1（风险 3 缓解句改写 + 风险 14 补注记 + 新增 15/16/17 三行）`，而 §8.1 表内行号集为 `13/14/15/16/17`（`grep -cE '^\| 18 \|'` = 0）。∴ 若按 v1.2 原 delta-5 处方落地（只追加 F-01 回修子句）而落 delta-4（新增行 18），§12 记「三行」而 §8.1 实有四行 = 账实不符，与本批 T3「账实同步」主旨及 v1.1-F3 的计数纪律同族。reviewer 判断正确。
- **修法（按 finding 指定，一处为主 + 两处同族计数表述同步）**：
  - **delta-5 处方正文**（承重件）：拆为 (a)(b) 两处，(b) = §8.1 计数子句 `新增 15/16/17 三行` → `新增 15/16/17/18 四行`，并按模板「承重 landing 指令必须原文引用钉死并给出替换后完整句」的义务，把**替换前 / 替换后两句逐字**用围栏块给出（不用「那句」「同上」类两读指称）；补一句「(b) 与 delta-4 是同一笔账的两半，必须同批原子落」，防只落一半制造新漂移。
  - **delta-4 处方正文**：把「新增风险行 ④」显式钉为「= 表内编号 `18`，接 landed 的 15/16/17 之后」，使 delta-5(b) 的 `15/16/17/18` 有唯一来源、不靠读者推算。
  - **T7 改法 + §3 sweep 架构行 + T6「关联」件**（同一计数表述，coordinator 授权同步）：T7 的 §12 行处方由「新增四行」精确为「新增 15/16/17/18 四行」（与 delta-5(b) 字节可对齐）；§3 架构行由「§8.1 五行（… / 新增四行）」改为「§8.1 六行（风险 3 缓解句 / 风险 14 注记 / 新增 15/16/17/18 四行）」——原「五行」是 v1.1 的 2+3 计数，v1.2 增一行后应为 2+4=6，属同族算术漂移，顺手校正；T6 的 ADR-019「关联」件处方由「新增三行」改为「新增 15/16/17/18 四行」（= delta-6 的 PROPOSAL 侧对应件）。
  - **F-04 同族第二处（起草者本轮实跑新发现，超出 reviewer 所指范围，如实上报）**：`docs/decisions/ADR.md:167` ADR-019 的「关联」件**已提交态**现文含 `§8.1 风险 3·14 + 新增 15/16/17`，delta-4 落地后应为四行——与 §12 同类账实不符，且它是**第三个落点**（§8.1 表本体 / §12 行 / ADR 关联件），只修 §12 会留下 ADR 与 arch 互相矛盾。已新增 **delta-6** 处方（含替换前/后逐字围栏块）交主线程与 delta-4、delta-5(b) 同批原子落；同时收窄 §2A 与「门 1 回修记录」里「ADR 零 delta」的笼统表述为「就 F-01 零 delta、就 F-04 需 delta-6 一处计数同步」。
- **落地现态（起草者实跑核对，诚实申报一处时序事实）**：本轮修订期间主线程**已在工作树落了 v1.2 canonical delta 全部五条**（`docs/architecture.md` 未提交态，`git diff --stat` = 4 insertions / 3 deletions）——实测 §8.1 行号集已为 `13/14/15/16/17/18`、§12 该行 §8.1 子句已由 HEAD 的「新增 15/16/17 三行」变为工作树的「新增 15/16/17/18 **四行**」、`grep -c '门 1 F-01 回修'` = 2（§8.1 行 18 与 §12 行各一）、F-01 缺陷句 `acc.cancel(reason="session_end")` 残留 = **0**。∴ **F-04 所指的账实不符在 canonical 现态已不存在**（主线程落地时已带上 (b)），本轮回修的实际作用面是**把 PROPOSAL 的处方文本补齐到与已落现态一致、且可重放**（回滚后重落不丢 (b)）。这一点如实记，不把它写成「architect 修好了 canonical」——canonical 全程由主线程写，architect 零写入（`git status` 可证：本件与 `package-split.md` 之外，architect 本轮无其它改动）。
- **未改**：T1–T7 的机制裁定、§2A `proposedContractDiff` 四处、`landing-manifest` 三件、§4 三条 OQ、V1–V19、delta-1/2/3 处方正文、`package-split.md` 全文（F-04 不涉拆包面）。
- **机械门（F-04 补正后复跑）**：`landing-fidelity-check --preflight` = `ok:true` / `anchorCount:3` / `anchorUnion` 三件不变 / `findings:[]` / `advisories:[]`；`spec-anchor-check` = `ok:true` / `errors:[]`；`contract-lint` = `ok:true` / 唯一既有 warning 未新增。文件自检：U+FFFD 0、围栏行偶数配对（新增 2 个围栏块承载 delta-5(b) 的替换前/后逐字句）、`### T` 锚 7、`目标文件` 锚行 7。

---

### R4 · F-04 闭合确认（architect-reviewer · 2026-11-16 · 一句话闭合 + §7 越界判定）

**verdict: GREEN（F-04 闭合）** —— F-01 已闭合（R3），F-04 补正到位：delta-5(a)(b) 拆分 + 替换前/后逐字围栏 + delta-4 钉「= 表内编号 `18`」+ T7/§3/T6 三处计数同步。canonical 三落点同数：`docs/architecture.md:493` §8.1 行 18（风险 18）/ `:566` §12「§8.1（… + 新增 15/16/17/18 四行）」/ `docs/decisions/ADR.md:167`「§8.1 风险 3·14 + 新增 15/16/17/18」——`grep -rn '15/16/17' docs/` 仅此两文件命中、**无第四处**，三处均表「四行」。活处方无残留错误「三行/五行」：余者仅为原文引证 fence（:692）、阐释句（:701）、§5.1.1 无关项（:319 的「§5.1.1 三行」）、或「**五行处置表**」表名（T2 分层表恰 5 行，非 §8.1 计数）；§3 旧「§8.1 五行」算术漂移已校为「六行」（2+4，:319 实测）。

**附带（非阻断，一处词差）**：delta-6 替换围栏句（:712）作「…`新增 15/16/17/18 四行`」，而 canonical `ADR.md:167` 落地作「…`新增 15/16/17/18`」（无「四行」——与 ADR 原「新增 15/16/17」range-only 风格一致）。二者语义同（均四行），唯「四行」一词有无之差。依 ARQ-261021「落地实跑为准」，建议把 delta-6 围栏句对齐为「…`18`」（或反向给 ADR 补「四行」），使 PROPOSAL 处方与 canonical 字节一致、可重放；三处「同数」已在语义上满足，故不阻断。

**§7 越界判定（architect 触碰本判官 append-only 段）**：核——R3 段「GREEN 条件」整行现文（:840）与本判官原文**逐字一致**：`**GREEN 条件**：F-04 一处一行补正（delta-5 加「§8.1 四行」）落地即全数闭合——届时 F-01 + F-04 均 GREEN，凭本 R3 + 该补正重冻。**FROZEN 归 human / 主线程，本判官不代冻。**`，拼接复原无字节偏差（「归 hu」+「man」= human 无缝），**内容无需再改**。**但触碰本身越界**：§7 的 R1/R2/R3 是判官 append-only 区，architect / 主线程不得直接删改（作者≠审者、评审记录不改既有文字）；本次净变更为零、不追责，登记此判定——后续对本判官记录的任何修改须经判官本人，不直改。**FROZEN 归 human / 主线程，本判官不代冻。**
