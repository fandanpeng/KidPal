---
rigor: full
riskTier: high
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-24
---

# M4-K3 记录会话跨端链路 + prompt v7 + live 真机确认轮（阶段四收口）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M4-star-notebook`（state=planning）nextAction 块①记录会话（录音+归档链路）跨端半 + live 收口。
> story 语义权威 = `docs/requirements.md` 现文（S-F2.2 AC1–AC4 + §2.1 记录会话段 + §2.3 矩阵引导态/录音态两行 + WF-F2-02）；支撑锚 = arch §5.2 场景④、§5.5.4、§5.5.5、to-be ⑥；契约面 = interface-contract.md §四（kidpal_session_ctx **已 LANDED**，本包只落实现）/§1.4（start_recording 既有）。

## 背景摘录（新建型锚「现状与差距」）

- **现状·session_ctx 双端零实现**：契约 §四已 LANDED（CDQ-260914-kidpal-session-ctx 明写「实现面归 dev-loop 后续包」）；固件 grep 零命中——protocol 层既有 Send* 面 = 音频/唤醒词/聆听起停/abort/mcp（`protocol.h:70-75`：SendAudio/SendWakeWordDetected/SendStartListening/SendStopListening/SendAbortSpeaking/SendMcpMessage），**无 KidPal §四私有消息类型发送原语**（门 1 F4 枚举修正）；会话发起入口 = `application.cc:763 HandleToggleChatEvent`（arch to-be ⑥ 既定接线点：读 `kidpal_layer_top()`）；front 侧拦截零命中（K1 本批落地，本包 dependsOn）。
- **现状·录音态演出半成品**：REC 闪点 + 大时长 + 声波柱在世（`story_page.c` 录音态渲染，K2 本批对齐 WF-F2-02 版式与回显占位缝）；`control/start_recording` 消费 + `archive_pend` 归档出口 + `kidpal_story_on_control` 幂等在世；**话语回显区无消费接线**（stt 帧现只进语音轮聆听回显，不进 story 视图）；引导态（P3 按 KEY 后、开口前）无实现。
- **现状·手势语义旧口径**：冻结输入表录音态 KEY 长按=丢弃（v1.47 已反转为=保存）；BOOT 丢弃族（短/长/双击同义）设备侧不存在（对账⑥）；K2 落输入表语义位、本包接会话动作（收尾=listen-stop 既有 / 丢弃=cancel 语义 → front acc.cancel 既有）。
- **现状·prompt v6 单指令**：故事意图段只有「我要讲故事」（「帮我记下来」全 deploy 目录零命中）；V31 四条回退（讲错啦丢弃/漏判挽回等）在 v6 故事段既有。
- **现状·设备侧无 timer 生产者**：`ses_machine_dispatch` 生产代码零调用（60s/120s 双阈值 = M1 遗留 to-be ②）——本包**不复活**（120s 收尾全在 front，K1-A5；板端只消费收尾结果演出）。
- **差距**（本包交付）：① 板端 session_ctx 发送（原语扩展 + 会话发起入口 + source_page 映射）；② 引导态 + 录音态回显消费接线；③ 录音手势族会话接线（保存/丢弃/引导态结束）；④ prompt v7（双指令 + 影子口径收口）；⑤ live 真机确认轮一次性收口全链（含 K1/K2 deferred live 半）。

## 改法取舍

- **发送原语复用 K2 缝**：K2 为 `kidpal_story_delete` 落的 protocol 层私有消息发送原语，本包同族扩展 `kidpal_session_ctx`（同一缝、不改 K2 冻结断言）——两个 §四消息一个发送面，避免第二套原语。
- **source_page 映射读取面**：`kidpal_layer_top()` / 页枚举（P3 在屏 → story；P1 → home；P2 → schedule——闭集三值全映射，非仅 story；契约 §四「其余取值现版本无行为（闭集预留）」，发送侧一次做全防未来再开固件面）。映射在会话发起入口单点（HandleToggleChatEvent + 唤醒词路径同查），不在多处散落。
- **时序钉死**：session_ctx 在**首帧音频前**（server hello 之后、SendStartListening 之前）——发晚则 front 已按普通会话切句送 LLM，累积态切换失效；通道复用的后续会话同样发送（契约「重复发送安全」= front 幂等入口 K1-A2 承接）。发送失败/通道未就绪 = 降级普通会话（不阻塞、不重试风暴），向后兼容负锚。
- **引导态 = 语音轮底座消费非新渲染面**：`kidpal_voice_states` 场景码（M1-K3 冻结）+ 气泡文案参数化「需要我帮你记点什么？」（WF-B-01 布局复用，v1.32 口径）；开口（UTT_START 边）切录音态（K2 冻结的 story 录音渲染面接管）。引导态全手势 = 结束会话回列表无归档（矩阵引导态行——不触发累积，front 侧会话建立但零 stt 即结束 = 无归档自然语义，板端不发 save）。
- **回显消费接线**：stt 帧（`OnIncomingJson` 既有分发）→ 累积态时同时喂 story 视图回显区（K2 占位缝落地：整段累积、切句只用于回显）；语音轮聆听回显路径零触碰（问答会话语义不变）。
- **prompt v7（取舍点，D8）**：新文件不回改 v6（版本化演进纪律）；变更面 = ① 故事意图段扩双指令（「我要讲故事」/「帮我记下来」同判 → control/start_recording）；② 影子口径收口——story_list/story_text 由 front 机械推送单源，v7 故事段明示不产 story 类 display 块（CCP §3 Impact 收口面）；③ V31 回退/标题 ≤12 字/故事分工既有段零偏移继承。carrier 声明 + provider.config 三键同步翻转（ADR-001 接线不变式：渲染面==声明面==被测面）。
- **120s/收尾词全在 front**（K1-A5 已落）：板端零新增计时器、零收尾词判定——收尾结果（story_text+seq 下发）驱动演出链（fresh_save 判定既有 → 盖章 → 回列表）。问答 60s 语义零触碰。
- **live 确认轮编排**（取舍点）：一次性长捕获（单真机不撞车 + human 在场持板，kidpal-flash-device / kidpal-serial-monitor 纪律；**双分区烧录**：若 K2 落了新 art 资产则 app+art 双烧，否则 app-only 合规）+ 三源日志关联（串口 + front journalctl + 引擎 docker logs，kidpal-live-smoke-debug 纪律）；A6 八步目击序在 plan 冻结，失败任一步 = 轮内定位（fixer 预算）而非重开轮。

## 阶段划分

1. acceptance-author 冻齿：发送缝齿（A1）+ 引导态/回显齿（A2）+ 手势接线齿（A3）+ prompt v7 offline 断言（A4，版本化守卫形态）+ 120s 收口齿（A5）；实跑红基线落账（板端双通道 + server 面）。
2. builder：protocol/application 发送接线 + mcp_server stt 回显消费 + 录音手势会话动作 + prompt v7 三制品（prompt 文件/carrier/provider.config）。
3. verifier↔fixer：host 全量零新增红 + server pytest 零新增红 + device build 闸。
4. integrator：ECS 部署（front K1 面已部署前提核对 + prompt v7 接线 apply-xiaozhi-overlay 渲染）→ live 确认轮预备（烧录 + 三源采集布防）。
5. live 确认轮（human 在场，A6 八步序 + A7 并入）→ 证据落账 → supervisor 合并门 → closeout（progress.md K3 段 + pod-retro + docs-sync + milestones M4 行 state 收口提议）。

## 铁律约束

- 不改 `docs/contracts/**` 与 spec；session_ctx/delete 形状 = 契约冻结版零自定。
- prompt v7 新文件不回改 v6；carrier 声明翻转与渲染面接线的不变式守卫（禁版本字面断言，ADR-001）。
- 板端零新增计时器（60s/120s 设备侧 timer to-be ② 不复活）；`ses_machine_dispatch` 生产零调用现状不在本包改（M1 遗留面）。
- K1 front 冻结语义（拦截/累积/收尾/账本）与 K2 冻结面（story_page 输入表/渲染缝/发送原语）只消费零修改；`kidpal_voice_states`/语音轮四态 M1 冻结面零触碰。
- OpenClaw 本体（ECS）架构零改动（prompt = 部署制品模式，既有交接/渲染链）。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## 边角（review 前自查登记，builder 易漏）

- session_ctx 时序（首帧音频前）与通道复用会话同发——两处最易漏（只接首连 / 发在音频后）。
- 唤醒词路径（「你好小智」）也过会话发起入口——source_page 映射单点须覆盖两入口（KEY 按 + 唤醒词）。
- 引导态结束（未开口）≠ 丢弃（已录内容）——前者无归档无 cancel 副作用、后者 acc.cancel；两条路径的 front 侧语义区分勿混。
- 「讲错啦」语音丢弃 = V31 回退既有链路（prompt 判定 → cancel 语义），本包只核对零偏移、不重接。
- prompt v7 双指令判定不得引入「记下来」类过度触发（如「帮我记下来明天带跳绳」是课表录入意图——v7 故事段与课表段意图边界措辞随冻齿断言）。
- live 轮 ⑦ 重启流水号不回退 = front 侧账本证据（K1 面）+ 设备侧盖章 N 目测双证。

## demo 证据形态复述

offline：host 齿（发送缝/引导态/手势接线/回显消费）+ prompt v7 版本化 offline 断言 RED→GREEN 全程留痕 + server pytest 回归零新增红 + device build 绿；live：human 持板真机确认轮（A6 八步序一次性目击 + 三源日志关联取证 + human 目击记名）——demo 口径 = offline 全量绿 + live 轮证据落账（含 K1-A7/K2-A7 deferred 半收口声明）。
