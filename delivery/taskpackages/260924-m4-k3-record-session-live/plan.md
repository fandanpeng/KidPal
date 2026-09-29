---
rigor: full
riskTier: high
sourceType: delivery-story
reviewed-by: pending（门 1 重拆重审 2026-09-29）
date: 2026-09-29
---

# M4-K3 记录会话跨端链路 + prompt v7 + story_list 推送重裁 + live 真机确认轮（阶段四收口）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M4-star-notebook`（state=planning）nextAction 块①记录会话（录音+归档链路）跨端半 + live 收口。
> story 语义权威 = `docs/requirements.md` 现文（S-F2.2 AC1–AC4 + §2.1 记录会话段 + §2.3 矩阵引导态/录音态两行 + WF-F2-02）；支撑锚 = arch §5.2 场景④、§5.5.4、§5.5.5、to-be ⑥；契约面 = `docs/contracts/interface-contract.md` §四（kidpal_session_ctx **已 LANDED**，本包只落实现）/§1.4（start_recording 既有）。
> 重拆注记（2026-09-29）：本件按 260928/260929 拆包新规收窄为五件骨架——「改法取舍」「阶段划分」撤出（包内实现决策承载位 = 冻结 acceptance 断言化 + taskpackage notes + planner 派发 prompt；planner Step 1g architect 前置预评后补写「技术方案」节并留痕）；原拆包侧实现决策要点已压缩进 taskpackage `notes` 备查。重拆新增承载面：defer 包 260928-story-connect-push-defer 摘除 K1 建连即推后，①恢复语义归本包重裁（见跨包节）。

## 背景摘录（新建型锚「现状与差距」）

- **现状·session_ctx 双端零实现（板端半）**：契约 §四已 LANDED（CDQ-260914 明写「实现面归 dev-loop 后续包」）；固件 grep 零命中——protocol 层既有 Send* 面 = 音频/唤醒词/聆听起停/abort/mcp（`protocol.h:70-75`），**无 KidPal §四私有消息类型发送原语**（门 1 F4 枚举修正）；会话发起入口 = `application.cc:763 HandleToggleChatEvent`（arch to-be ⑥ 既定接线点：读 `kidpal_layer_top()`）；front 侧拦截 K1 已 merged（本包 dependsOn 实存前提）。
- **现状·录音态演出半成品**：REC 闪点 + 大时长 + 声波柱在世（story_page.c 录音态渲染，K2 本批对齐 WF-F2-02 版式与回显占位缝）；`control/start_recording` 消费 + `archive_pend` 归档出口 + `kidpal_story_on_control` 幂等在世；**话语回显区无消费接线**（stt 帧现只进语音轮聆听回显，不进 story 视图）；引导态（P3 按 KEY 后、开口前）无实现。
- **现状·手势语义旧口径**：冻结输入表录音态 KEY 长按=丢弃（v1.47 已反转为=保存）；BOOT 丢弃族（短/长/双击同义）设备侧不存在——K2 落输入表语义位、本包接会话动作（收尾=listen-stop 既有 / 丢弃=cancel 语义 → front acc.cancel 既有）。
- **现状·prompt v6 单指令**：故事意图段只有「我要讲故事」（「帮我记下来」全 deploy 目录零命中）；V31 四条回退（讲错啦丢弃/漏判挽回等）在 v6 故事段既有。
- **现状·story_list 推送两时点**：K1 原三时点中的①建连即推被 defer 包摘除（真机实证：设备每会话 WS 重连 → 重复推送盖页，旧固件无条件渲染 P3 页盖聆听态 + 零 CJK 字体半截页）；现役 = 归档后 + 删除后两时点；①恢复语义归本包重裁（defer review-packet 未覆盖面第 1 条：真机复验「设备侧停弹页面」也归本包设备轮）。
- **现状·设备侧无 timer 生产者**：`ses_machine_dispatch` 生产代码零调用（60s/120s 双阈值 = M1 遗留 to-be ②）——本包**不复活**（120s 收尾全在 front，K1-A5；板端只消费收尾结果演出）。
- **差距**（本包交付）：① 板端 session_ctx 发送（原语扩展 + 会话发起入口 + source_page 映射）；② 引导态 + 录音态回显消费接线；③ 录音手势族会话接线（保存/丢弃/引导态结束）；④ prompt v7（双指令 + 影子口径收口）；⑤ story_list 推送①时点重裁；⑥ live 真机确认轮一次性收口全链（含 K1/K2 deferred live 半）。

## 可行性判据

- **契约前置实存**：kidpal_session_ctx 契约 §四 LANDED（CDQ-260914），front 拦截半 K1 已 merged 本地 main（e12a8b94）——双端对端语义现文可核对，无契约改动面。
- **发送缝前置可期**：K2 在本包前落 kidpal_story_delete protocol 层发送原语（mergeOrder 39→40 串行）——session_ctx 发送为同族扩展，原语形态复用不新造（K2 未 merged 前本包不 launch，dependsOn 实存）。
- **语音轮底座在世**：`kidpal_voice_states` 场景码（M1-K3 冻结）+ WF-B-01 聆听布局 + 气泡文案参数化路径——引导态为底座消费非新渲染面。
- **回退与归档链路在世**：V31 四条回退（v6 故事段既有）、StoryAccumulator routing/recover（K1 merged 现文）、control/start_recording → archive_pend 出口既有。
- **prompt 版本化管线在世**：v1–v6 演进先例 + OPENCLAW_SYSTEM_PROMPT_CARRIER 声明翻转机制 + provider.config 三键同步 + 渲染链（apply-xiaozhi-overlay）——v7 走既有管线零新机制。
- **live 轮取证纪律在案**：kidpal-serial-monitor / kidpal-live-smoke-debug / kidpal-flash-device 技能齐，三源日志关联（串口 + front journalctl + 引擎 docker logs）先例多轮（M2/M3 设备轮）。

## 跨包事实与共享冻结面约束（含跨切片边角）

- **session_ctx 时序与幂等（契约 §四语义，front 侧 K1 冻结）**：发送时点 = 首帧音频前（server hello 之后）——发晚了 front 已按普通会话切句送 LLM，累积态切换失效；通道复用的后续会话（wss 常驻第 2+ 次会话）同样发送，不只首连；重复发送安全（front 幂等入口承接）。发送失败/通道未就绪 = 降级普通会话（不阻塞会话建立、不重试风暴），向后兼容负锚。
- **source_page 映射单点**：读取面 = `kidpal_layer_top()`/页枚举，映射在会话发起入口单点——**KEY 按与唤醒词「你好，小智」两入口都过该入口**（漏唤醒词路径 = 引导态失效）；P3→story 之外 home/schedule 闭集映射一次做全（契约 §四「其余取值现版本无行为（闭集预留）」——防未来再开固件面）。
- **story_list 推送①重裁（defer 承接）**：建连即推与设备每会话 WS 重连冲突实证在案（defer 包 review-packet）；重裁输入 = 冲突形态 + K3 新增的引导态会话建立时点（天然候选缝：session_ctx 发送同拍或 front 收到 ctx 后条件推送）；重裁结论 = acceptance 冻齿断言（推送时点行为级断言），defer 遗留的「真机复验设备侧停弹页面」随本包设备轮一并目击。
- **K2 冻结面（rebase 前提）**：本包 intake rebase K2——story_page 输入表语义位（KEY 长按=保存/BOOT=丢弃）、回显占位缝、kidpal_story_delete 发送原语均为 K2 冻结断言，本包同族扩展（session_ctx 原语同缝）不改 K2 语义；K2 host 半 merged 为本包设备轮前置（LP4）。
- **板端零新增计时器**：120s 收尾全在 front（K1-A5 front 驱动）——板端只消费收尾结果演出链（story_text+seq 下发 → 退 REC → 盖章 → 回列表）；60s 问答超时语义零触碰（M1 遗留 to-be ② 不复活）。
- **K1 front 冻结语义（只消费）**：拦截/累积/收尾词检测/120s/账本（StoryAccumulator routing/recover/cancel 语义）零触碰；「讲错啦」语音丢弃 = V31 回退既有链路，只核对零偏移、不重接。
- **prompt v7 约束面**：v7 新文件不回改 v6（版本化演进）；ADR-001 接线不变式 = 渲染面==声明面==被测面（v7 文件 + OPENCLAW_SYSTEM_PROMPT_CARRIER 声明翻转 + provider.config 三键同步）；影子口径收口 = story_list/story_text 由 front 机械推送单源，v7 故事段明示不产 story 类 display 块。
- **live 轮组织约束**：单真机不撞车 + human 在场持板（LP4）；K2 若落新 art 资产 = app+art 双分区烧录，否则 app-only 合规；A6 八步目击序内失败任一步 = 轮内定位（fixer 预算）而非重开轮。
- **跨切片边角（builder 易漏）**：① 引导态结束（未开口）≠ 录音态丢弃（已录内容）——前者无归档无 cancel 副作用、后者 acc.cancel，两条路径 front 侧语义勿混；② 「帮我记下来」双指令判定不得越界误触课表录入意图（「帮我记下来明天带跳绳」= 课表录入——v7 故事段与课表段意图边界措辞随冻齿断言）；③ live 轮「重启流水号不回退」目击 = front 侧账本证据（K1 面）+ 设备侧盖章 N 目测双证；④ source_page 映射若散落多处实现 = 漏入口根因——单点函数。

## 铁律约束

- 不改 `docs/contracts/**` 与 spec；session_ctx/delete 形状 = 契约冻结版零自定。
- prompt v7 新文件不回改 v6；carrier 声明翻转与渲染面接线的不变式守卫（禁版本字面断言，ADR-001）。
- 板端零新增计时器（60s/120s 设备侧 timer 不复活）；`ses_machine_dispatch` 生产零调用现状不在本包改（M1 遗留面）。
- K1 front 冻结语义（拦截/累积/收尾/账本）与 K2 冻结面（story_page 输入表/渲染缝/发送原语）只消费零修改；`kidpal_voice_states`/语音轮四态 M1 冻结面零触碰。
- OpenClaw 本体（ECS）架构零改动（prompt = 部署制品模式，既有交接/渲染链）。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## demo 证据形态复述

offline：host 齿（发送缝/引导态/手势接线/回显消费/推送重裁时点）+ prompt v7 版本化 offline 断言 RED→GREEN 全程留痕 + server pytest 回归零新增红 + device build 绿；live：human 持板真机确认轮（八步目击序一次性收口 + 三源日志关联取证 + human 目击记名）——demo 口径 = offline 全量绿 + live 轮证据落账（含 K1-A7/K2 真机目测半 deferred 收口声明 + defer 包「设备侧停弹页面」复验目击）。
