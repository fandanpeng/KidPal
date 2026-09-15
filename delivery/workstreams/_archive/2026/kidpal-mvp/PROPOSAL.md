# PROPOSAL · kidpal-mvp 开发包重切（方向 B · workstream 设计权威）

> **状态：FROZEN v0.4 · 2026-09-03 · human 盖冻结（凭据：`docs/spec.md` v0.2 FROZEN §8 V33 重切 pin「冻结即触发、owner = portfolio-planner、相位 = 紧随冻结」 + architect 单轮 dev-side review GREEN-with-fixes（review.md v0.4 段，8 条建议级全回灌） + human 四项拍板 OQ-R1~R4）**
> **frozen pin**：权威 pin 以 15 包 `origin.upstreamFrozen` 与 decision-coverage.json 记录为准（raw-content 行尾归一化算法，task-intake-and-slicing.md:53）。**自指规避**：pin 不入本文件文头（写进来即改内容、pin 必漂移）；本行仅为人读留痕、非机判基准。
> **重切触发**：`docs/spec.md` v0.2 FROZEN（2026-09-03）§8 V33 pin——「本 spec 冻结即触发 portfolio 重切，owner = portfolio-planner，相位 = 紧随冻结」。本件即该 pin 的执行。
> **上游权威**：[spec.md](../../docs/spec.md) v0.2 FROZEN（功能/story，含新增 §4.4 F4）+ [interface-contract.md](../../docs/contracts/interface-contract.md) 方向 B 重写版（E1/B1/D3 + §三推送重写 + §四端云协议剖面，2026-09-03 原子落地）。
> **workstream**：`.planning/kidpal-mvp/`
> **本件性质**：大 feature 重切设计权威（change-triage 档③），非 Track B 治理批——无 landing-manifest/packet-binding 字段；落地 = portfolio-planner 依本件更新/新建/作废 TaskPackage，非主线程 packet-bound。
> **与 v0.3 的关系**：v0.3 的 13 包按「MVP 同步 HTTP」假设切分；方向 B（WebSocket + Opus 流式，官方小智服务端网关）冻结后，HTTP 底座作废。本稿 = v0.3 的增量重切，对账口径（两套正交口径，钉死不混用）：**保留/作废/新建** = 12 保留（含已 merged 的 S）+ 1 作废（包 1）+ 2 新建（G/WS）= **15 包**；**重写/小调/不动**是保留的子集口径——2 重写（2/7）、5 小调（5/6/8/9a/9b）、4 不动（0/3/4/U）+ S 已 merged。

---

## 0. 想实现的效果与差距

**目标效果一句话**（不变）：把 spec §4 全部 story（F1 课表 / F2 故事 / F3 查字 / **F4 通用问答（v0.2 新增）** + §4.0 共享底座）拆成一组排好序、带依赖、模拟优先、硬件殿后的可并行 TaskPackage。**方向变化**：链路形态从「设备↔HTTP shim↔OpenClaw」转为「设备 ⇄ wss ⇄ 流式网关（官方小智服务端）⇄ OpenClaw」，重切据此换底座、保能力切片。

**现状（2026-09-03 地面真值）**：
- Pod S（260829-smoke-e2e）已 merged：HTTP 形态「设备↔shim↔OpenClaw」串通实证（live 5/5），为**架构验证台阶**，方向 B 落地后按 spec §3.0.0 退役；其安全发现（401 门穿透纪律、设备 token 纪律）沿用进 V11。
- spec v0.2 FROZEN + 契约方向 B 重写已原子落地（工作区，尚未 commit——见 §7 留根①）；28 条决策、提醒解耦四态、强制鉴权、证书十年期全部定稿。
- 官方小智服务端（xinnan-tech/xiaozhi-esp32-server，MIT）已克隆：ECS 部署源 `/opt/kidpal/xiaozhi-esp32-server`（浅克隆）+ 本地参考 `ESP32-S3-RLCD-4.2-Demo/05_XiaoZhiServer/`（已建图索引 `ESP32-S3-RLCD-4.2-Demo-05_XiaoZhiServer`）。
- OpenClaw 原生 cron 已实证在跑（2026-09-02，含 class-reminder 类任务先例）——契约待确认点 3 销账。
- 三线验证已做：OpenClaw `stream:true` 实测通（坑 = 流式禁带 `max_tokens`）；固件审计零硬阻塞（原生 WS+Opus 管线在树、改动点仅服务器地址入口）；开源调研确认配置级接入。
- 对抗审实现侧三 finding 待进包：V10（固件证书加载面）/ V11（服务端强制鉴权承载）/ V27（display JSON 机械 schema 校验）——`dev-loop-handoff.json` 就位。

**剩余差距 = 方向 B 实现面全部未动**：网关部署/适配/鉴权（服务端零行）、设备 WS 切换与证书（固件侧入口未改）、mcp display/alert 通道（双端未建）、各 story 的设备渲染（v0.3 B 段切片大体有效、端云接缝全部换形）。

**非目标（本批不做）**：
- 不改 `docs/contracts/**`（方向 B 重写已冻结；各包只消费，疑似变更走契约门）。
- 不改 vendor 面 `ESP32-S3-RLCD-4.2-Demo/**`（只读）。
- 二期项不变：WakeNet 唤醒 / OTA 应答端点 / 小程序 / 功耗深度优化（功耗仅 9b 跑基线，决策 21 DEFER 三要素）。
- S-F2.5（云端润色）维持 dropped。

## 1. 拆分依据（地面真值更新）

垂直切片原则不变（每包承载可独立演示/验证的完整能力）。六个直接决定重切法的事实：

1. **契约方向 B 已冻结**：E1 ①–⑥ view / B1 表体 / D3 零改动；§三重写为推送语义（mcp kidpal/alert + 渲染回执出队 + 重试到确认）；§四端云协议剖面新增（hello/Opus 帧/stt/tts/abort/mcp/鉴权/会话连续性承载/禁 max_tokens）；§1.5 网关职责收敛（强制鉴权 + display 机械校验）；§1.6 system prompt 约束节保留。包只消费。
2. **网关形态 = 官方开源服务端 + 两个小补丁**（决策 18）：部署与配置是主体（LLM `type:openai` 接 OpenClaw、TTS=edge-tts、ASR=阿里云云端），自写仅 = display 切分校验与 mcp 中转（push helper）+ 提醒定时推送（各约几十行）。→ 服务端工作天然切两片：**基建片**（部署+配置+鉴权+校验+中转，服务所有包）与**业务片**（提醒排程推送、故事累积，随各自垂直切片）。
3. **设备侧 = 复活小智原生管线 + 换入口**（固件审计结论）：原生 WS+Opus 管线在树、编译通过、与 Pod S 新增面零冲突；改动点 = 配网直写 NVS（`websocket` 命名空间 url/token/version）+ 短路激活 + 证书加载（V10）+ ShimProtocol/mp3_decoder/PlayMp3 死代码消除。→ 这是一个内聚的设备片 = WS 包。
4. **首验驱动**（决策 19/22 + §6.3）：ASR provider 终选（V23）与多轮连续性（V21）都是**部署期第一件事**，必须在网关侧先跑、失败就地降级——→ 首验归网关基建片（G），不过则不往设备侧投量（fail-fast）。
5. **Pod S 台阶退役是安全事项而非仅清理**：shim 0.0.0.0:8080 + 安全组放行是未回收暴露面（Pod S 收口记录）；退役应与设备切换同批收口（早关早安全），且撞 Pod S 冻结验收（行为碰撞型，supersede 走 acceptance-author）。
6. **char 延迟修法（C1）与提醒模型（决策 20/23/27）**：前者不变（OpenClaw 侧查表拼装、守薄桥红线）；后者从「契约 blocked 包」变为「全链路明确包」（脑算网关推、解耦四态），是本重切中范围变化最大的能力切片。

**测试策略分层**：v0.3 表不变（纯逻辑 host / server pytest / UI 走 SDL / 声学真机 / 物理真机），新增一行：**ECS live 部署验收**（网关部署、鉴权实测、首验探针、ASR 开通）按 ARQ-260901-g3-server-live-acceptance 口径标 `@pytest.mark.live`、不 auto-run。

**三段式演进**：阶段 A 冒烟在 HTTP 形态已由 S 完成使命；**方向 B 的阶段 A = G → WS 对**（服务端就绪 → 设备切换 + 首链冒烟 + 首验）。阶段 B 模拟开发、阶段 C 硬件验收沿用。

## 2. 重切总览（15 包：13 live + S merged + 1 abandoned · merge order）

### 阶段 A · 方向 B 冒烟（服务端就绪 → 设备切换 → 首链 + 首验）

| # | changeId | 能力切片 | 面 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|---|
| S | `260829-smoke-e2e` | **已 merged**（HTTP 台阶，按计划退役；安全纪律沿用） | 板+云 | high | — | — | 1（merged） |
| 0 | `260829-host-test-infra` | host 测试基建（**范围不变**）：ESP-IDF linux target 单测 + LVGL SDL 模拟 + 目录约定。注：既有 worktree（base 53731c6）缺 v0.2 FROZEN 与重切产物，launch 前须自 v0.4 base ref 重建 | firmware | high | 自身即基建 | — | 2 |
| G | `260903-gateway-directionB` | **方向 B 流式网关 bring-up（新）**：xiaozhi-esp32-server ECS 部署 + provider 适配（LLM `type:openai` 接 OpenClaw【流式禁带 `max_tokens`】/ TTS edge-tts / ASR 阿里云云端开通与首验终选）+ **强制设备鉴权**（V11：token 未设=拒绝、机械短路语义、constant-time 比对，历史教训 ARQ-260902-shim-401-gate-bypass）+ ` ```display ` 切分 + **机械 schema 校验**（V27：view 白名单 + data shape/大小上限 + 脏 JSON 拒绝降级）+ mcp 中转基建（kidpal/display 与 kidpal/alert 传输 + JSON-RPC 回执）+ **网关兜底逻辑生成**（4s 安抚句定时器 / 错误兜底「网络出小差了」结束当轮 / V32 超句截断分屏，与 V27 同处网关 push 面；设备侧消费与验收归包 2）+ **部署期首验**（V21 两轮追问连续性探针 → 承载方案定案；V23 ASR 流式+VAD 切句兼容性终选）+ 自签证书签发（≥10 年 + 轮换脚本 + 到期告警，V25） | 云（server/+ECS） | high | PC pytest + ECS live | — | 3 |
| WS | `260903-device-ws-switch` | **设备切 WS + 方向 B 冒烟（新）**：配网直写 NVS（url/token/version）+ 短路激活 + **自签证书加载与 pinning**（V10）+ WS 握手 Bearer + 方向 B 首链冒烟（KEY→流式 ASR→OpenClaw→流式 TTS 播放+文字上屏）+ **HTTP 台阶退役**（固件 ShimProtocol/mp3_decoder/PlayMp3 死代码消除、ECS shim 服务停 + 8080 安全组关闭、server/shim.py 归档记录）+ Pod S 冻结验收 supersede 处置 | 板+云（收尾） | high | device + live | G | 4 |

### 阶段 B · 模拟开发（host/PC 跑，不碰板子）

| # | changeId | 能力切片 | 面 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|---|
| 2 | `260829-voice-roundtrip` | **重写**：流式语音回环（`stt`/`tts` 消息状态机 + 句级播放 + 文字同步上屏）+ display mcp 消费端（view 路由：text 全文上屏滚动 / home / control end_session·stop_speaking）+ **display 格式遵循度复验**（契约待确认点 1，统计性口径——方向 B 明示不复用 12/12 旧结论；G 只做机械校验、复验归本包，失败包内 resolve 不 gate）+ 网关兜底**设备侧消费与验收**（安抚句 / 错误兜底 / 格式不遵循降级显示；生成逻辑归 G）+ `abort` 消息 + OpenClaw 基础 system prompt（E1 遵循 + F4 三句纪律 + 儿童化）+ **F4/S-F4.1 通用问答承载** | 板+云 | high | hybrid（host+PC+live） | WS,0 | 5 |
| 3 | `260829-session-logic` | 范围不变：会话状态机（聆听/思考/播报/超时/结束）+ KEY/BOOT 全局手势解析；预留 manual 聆听/录音模式接合面供包 8 | 板 | high | host | 0 | 6 |
| 4 | `260829-char-stroke` | 范围不变：D3 消费 + 贝塞尔离散 + 坐标变换 + 1bpp 光栅 + 米字格渲染 + 逐笔动画 + 右栏速查；云侧 C1（OpenClaw 侧静态字库查表拼装 + 目标字表预热）。**闸 A 维持**：契约 §1.6 system prompt 措辞随查表方案调整，launch 前 architect-reviewer 四分类预审 | 板+云 | high | hybrid（host+PC live） | 0,2 | 7 |
| 5 | `260829-home-p1` | 小调：刷新三触发换 WS 语义（连接建立 / 每日 0:00 跨天 / 新建日程即时推，决策 26）+「接下来」分组呈现**消费**（producer = 包 6 OpenClaw 侧分组计算；本包只渲染分组结果 + 倒计时贴纸 + 明日预告）+ 低电打盹 | 板 | medium | host + SDL | 0,2 | 8 |
| 6 | `260829-schedule-p2` | 小调：课表页今天/明天 + 圆点两态 + 语音问课表滚动定位；录入通道 = **QQ 主**（OpenClaw 自改写即时触发三档重算 + P1 即时推）+ 直改 MD 副（无即时推，V22）；**「接下来」OpenClaw 侧分组计算（决策 26 producer：周中白天学校课合并「上学」取首节时刻、培训班/一次性逐项列出，与 QQ 自改写即时推同源）**；B1 表体零变更、`remind_before_min` 已标废止不消费 | 板+云 | high | host + PC | 0,2 | 9 |
| 8 | `260829-story-p3` | **服务端扩**：列表 + 回看/回听 + 盖章不变；录制 = **manual 聆听模式 + `start_recording` + 网关累积**（不按句送对话，停录整段转写归档）+ **显式回退路径**（KEY 长按取消 /「讲错啦」/ 停录 10s 内「重录」/ 漏判「刚才那段是故事」挽回，V31）+ 设备 REC 演出 | 板+云 | high | host + PC | 0,2 | 10 |
| 7 | `260829-schedule-reminder` | **重写 + contractBlocked 解除**（待确认点 3 已实证）：OpenClaw cron 三档排程（区块范围判：学校课程表区块不提醒；每周固定课表区块每周重复生成 + 一次性安排；幂等 id 确定性散列；写 `alerts/pending.json`；**speech 排程时预合成**）+ 网关提醒投递（每分钟轮询到点推送、重试到确认 60s×3、不做重连即重推、过期静默作废）+ 设备弹窗三件套（提示音 + TTS 读文案 + 驻留弹窗；排队逐个弹；**滞留弹窗顶替（V18）**；**渲染即回执出队**；易失 known-id 幂等去重；KEY 关闭/BOOT 关闭翻页）+ 断网无提醒验收（离线红线，决策 24） | 板+云 | high | host + live | 6 | 11 |
| U | `260901-ui-art-integration` | 范围不变：资产管线 + V3 资产本体 + 统一换皮（V3 设计稿为前置输入，未出不得 launch） | 板+tools | high | host+SDL+device 观感 | 0,2,4,5,6,7,8 | 12 |

### 阶段 C · 硬件验收（殿后烧板子）

| # | changeId | 能力切片 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|
| 9b | `260829-hardware-validation` | 原两条真机项（16px CJK 可读性 + 前光）不变；**新增**：实机续航基准 = 决策 21 DEFER 三要素承载（触发点在此、预警阈值、owner 是本包）+ 常驻 WS 连接功耗观察 + 证书真实设备握手冒烟 | high | device | 2~8+U | 13 |
| 9a | `260829-audio-bargein-hw` | 小调：AEC 语音喊停实测调阈值不变（最大技术风险隔离成片）+ `abort` 消息补充通道验证（云端取消生成省算力，不改本地即时停博主地位） | high | device | 2,9b | 14 |
| ~~1~~ | ~~`260829-shim-skeleton`~~ | **作废 → board `abandoned`**：HTTP shim 完整化方向被方向 B 整体作废；能力由 G（服务端网关）+ WS（设备链路）承接。作废非删除：taskpackage.json append 作废记录与承接指针，历史行不动 | — | — | — | 撤回 |

**承接映射（作废与重写的去向，防歧义）**：原包 1 的「设备鉴权」→ G（V11）；「四接口」→ 方向 B 协议剖面（G 配置 + WS 消费）；「alerts 出队」→ 7；「OpenClaw loopback 转发」→ G（provider `type:openai` 接 OpenClaw、真 token 不出内网，F4 补名）。原包 2 的「E1 display 切分下发」→ G 的切分校验 + 2 的消费端。原包 7 的「/v1/alerts 轮询 + RTC 离线兜底」→ 废止（推送语义 + 离线无提醒，决策 24）。

## 3. 关键路径与并行调度

**关键路径**：`G → WS → 2 → 6 → 7 → U → 9b → 9a`（**8 级**，较 v0.3 的 7 级 +1——服务端/设备 tracer-bullet 拆成一对串行包所致；取舍见 §5 OQ-R1）。包 3/4/5/8 不在最长链（3 只需 0；4/5/8 经 2 即达第 4 级）；U 等全部页面包 + V3 设计稿。

- **G 是方向 B 的通行证**：部署 + 鉴权 + 校验 + 首验（V21/V23）全在 G——首验失败就地降级/立项，不往固件侧投量。G 绿了才起 WS。
- **WS 是第二张通行证**：设备首链冒烟（流式语音一来一回 + 文字上屏）证明「设备⇄网关⇄OpenClaw」方向 B 形态串通；同批收口 HTTP 台阶退役与安全面（8080）。
- **B 段沿用 host-first**：2/3/4/5/6/8/U 主体电脑跑；7/8 的服务端组件（提醒投递、网关累积）是对 G 网关代码的 additive 扩展，靠 mergeOrder 串行化冲突（G=3 早并）。
- **C 段沿用**：9b（物理+续航基线）→ 9a（声学，最易返工、隔离）。

**并行调度（WIP=3 不变）**：
- 起跑：**G + 0 并行**（0 worktree 自 v0.4 base ref 重建）。
- G 绿 → **WS**；0 绿且 WS 绿 → **2**；0 绿即可起 **3**（3 与 2 并行，WIP 允许 2+3 同时在跑）。
- 2 就绪后：**4、5、6、8** 入并行池（WIP=3 限流）。6 就绪 → 7。全部页面 + V3 设计稿 → U。
- merge 串行化：merge-driver 按 mergeOrder 逐个 rebase+quick-check+merge。

**testHost 标注**：S=（merged）；0=基建自身；G=PC pytest + ECS live（部署/鉴权/首验标 `@pytest.mark.live`）；WS=device + live 冒烟；2=hybrid（host 状态机/渲染 + PC 遵循度 + live 链路）；3=host；4=hybrid（host 渲染 + PC 查表单测 + live char 延迟，C1 口径不变）；5=host+SDL；6=host+PC；8=host+PC；7=host（排程/去重/回执状态机）+ live（推送链路）；U=host+SDL+device 观感；9a/9b=device。

## 4. 契约面与冲突面扫描

- **契约消费，不修改**：14 包全部只读消费方向 B 重写版契约。0 包在 Pod 内改 `docs/contracts/**`。
- **contractBlocked = 空**：待确认点 3 已实证（包 7 解除）；点 1 复验归包 2 验收内容（不 gate-block）；点 2（课表 MD 解析）为传输无关结论、沿用。
- **闸 A（包 4，维持）**：契约 §1.6「对 OpenClaw 的要求」措辞随 C1 查表方案调整 → 包 4 launch 前 architect-reviewer 四分类预审（对象 = 重写后契约 §1.6，节仍在）。
- **G launch 前置门簇（新增，不带门进 launch）**：
  - ① **18789 锁 127.0.0.1**（V28 硬前置）：安全组/防火墙操作（human/ECS 管理面）+ G 部署期断言；与 ARQ-260901-openclaw-chatcompletions-public 复议触发合并。
  - ② **ARQ-260902-cert-lifecycle-deadlock 处置定夺**（human）：accept-as-risk 记账 vs OTA 提前——G 签发 ≥10 年证书前须定。
  - （阿里云 ASR 开通为 G 包内部步骤，非外部门——已列入 G 范围「开通与首验终选」，F6 纠偏。）
- **行为碰撞型（launch 前预警）**：① **Pod S 冻结验收**（live 5 条 + HTTP 结构断言）与 ② **TP-A（260831-cloud-link-bringup）冻结验收** `server/acceptance/test_cloud_link.acceptance.py`（CLB-A7「server 起最小 shim，POST /v1/chat 一路跑通」/ CLB-E2「shim 保持薄 ~200 行」等承载 HTTP shim 形状断言，与 Pod S 共享 `server/shim.py`，同撞退役对象）——**冻结面优先、supersede 走 acceptance-author**：WS intake 时由 acceptance-author 显式将两份冻结验收标 superseded（理由 = spec §3.0.0「迁移与既有资产」台阶退役条款；方向 B 退役吞并之），断言语义零变更、只改归属状态；不带冲突进 WS launch。TP-B（资产管线）/ TP-C（模拟器）已核实不载 shim/HTTP 形状、不撞。
- **常量互斥型**：无。
- **跨包共享面**：G = 服务端共享底座（7/8 的服务端组件都加在 G 网关代码上 → mergeOrder 3 先并）；2 = 设备 display 路由共享底座（4/5/6/8 消费 → mergeOrder 5）；0 = 固件测试基建（不变）；U = 殿后换皮（不变）。

## 5. human 取舍点（本重切已裁，留痕）

| OQ | 问题 | 选项 | 裁定 | 日期 |
|---|---|---|---|---|
| OQ-R1 | tracer-bullet 形态 | (a) G + WS 两包串行 / (b) 单包「方向 B 链路 bring-up」 | **(a) 两包串行**——fail-fast：V21/V23 首验失败挡在服务端、不烧固件投入；网关早并减少 7/8 服务端扩展的 rebase 冲突面；代价 = 关键路径 +1 级，接受 | 2026-09-03 |
| OQ-R2 | 包 1 处置 | (a) abandoned，能力并入 G/WS / (b) 缩圈保留为 HTTP 回退通道 | **(a) abandoned**——方向 B 冻结「唯一公网入口 = 网关」（决策 18/§3.0.0）；双通道违背冻结 + 8080 暴露面 + 401 门穿透教训（ARQ-260902-shim-401-gate-bypass） | 2026-09-03 |
| OQ-R3 | F4/S-F4.1 承载 | (a) 归包 2 / (b) 新开独立小包 | **(a) 归包 2**——F4 = 问答走对话链路 + text view 全文滚动，完全是包 2 切片内的事；遵循度/三句纪律验收与 display 复验同批发生 | 2026-09-03 |
| OQ-R4 | shim 退役相位 | (a) WS 包内同批退役 / (b) 留到 9b 收口 | **(a) WS 同批**——8080 安全组暴露面宜早关；设备服务器地址单一事实源；退役撞的冻结验收（Pod S + TP-A）一次 supersede 清完 | 2026-09-03 |

（v0.3 已裁项继续有效：能力竖切 / host 先行 / WIP=3 / 9a 拆片 / C1 字库 / C2 美术包 / 风险分级，不再重开。）

## 6. 验证集（重切正确性的机械/人工核验）

- taskpackage.json：**11 包内容更新**（2/7 重写、5/6/8/9a/9b 小调、0/3/4/U 字段微调 + origin 指回本件）+ 2 新建（G/WS，changeId 带 260903- 前缀）+ 1 作废（包 1，append 作废记录）；S 已 merged、taskpackage 不动。**总数对账 = 13 live + S merged + 1 abandoned = 15**。
- board：`portfolio-reconcile.mjs --set-pod` 落 **15 pod**——S=merged、1=abandoned、G/WS=queued（mergeOrder 3/4）、0~9a 重排（2=5、3=6、4=7、5=8、6=9、8=10、7=11、U=12、9b=13、9a=14），WIP=3。
- decision-coverage.json 更新：新增 **S-F4.1** → 包 2；新增「§6.1 协议剖面 + 决策 22 安全面」→ G+WS；dev-loop handoff 映射 **V10→WS / V11→G / V27→G（+2 消费端）**；**§4.0 错误兜底/安抚句 4s 定时器 / V32 超句截断分屏 → G（生成）+ 2（消费与验收）**（F2）；**决策 26 分组呈现 → 包 6（OpenClaw 侧 producer）+ 包 5（消费）**（F3）；contractBlockedPackages → 空；storyCoverage 各包 acceptance id 随重写更新。
- 依赖无环：G→WS→2→{4|5|6→7|8}→U→9b→9a 为 DAG，mergeOrder 拓扑有序（3→4→5→…→14）。
- F-03/F-05 交路（落地时序，见 §7 留根①②）：原子落地制品先 commit main → 重切产物 commit `planning/kidpal-mvp-base-v0.4` → worktree 自该 base 建。

## 7. 留根清单

1. **原子落地制品尚未 commit**（spec v0.2 / 契约重写 / ARQ/CDQ / progress / gate 证据，全在工作区）：F-03 前置——commit-before-launch 要求 Pod 的 base ref 含冻结 spec+契约。时序 = human 冻结本件后：先 commit 原子落地制品入 main，再建 `planning/kidpal-mvp-base-v0.4`（自主干）commit 重切产物。**需 human commit 授权**（治理约定：commit 须 human 要求）。
2. **包 0 既有 worktree 过期**（`C:/AI/KidPal-pod-host-test-infra`，base 53731c6）：缺 v0.2 FROZEN 与重切产物，launch 前自 v0.4 base ref 重建（旧 worktree 未开工，作废无损）。
3. **ARQ-260902-cert-lifecycle-deadlock**：G 前置门（human 定夺），见 §4。
4. **ARQ-260902-multiturn-continuity**：G 部署期首验即触发点；失败降级路径已预裁（网关适配传会话标识/历史立项，或 A4 降级单轮并记账）。
5. **V3 设计稿**：U 的前置输入，未开始（继承 v0.3）。
6. **char 热字包缓议 / 4 页循环翻页 9b 兜底 / 9b 两条真机项**：继承 v0.3，不变。
7. **契约待确认点 1/2 复验归属**：点 1 → 包 2 验收（方向 B 不复用 12/12）；点 2 结论传输无关、沿用实证。
8. **ASR provider 终选**：G 部署期首验（V23）；不达标降级链 = 非流式逐句 / 调 VAD / 转 SenseVoice（决策 19 兜底三触发）。
9. **Pod S 收口两件小事随 WS 包落地**：8080 安全组关闭 + `server/shim.py` 归档记录；另 ARQ-260902-shim-401-gate-bypass 要求的「TP-A 收口制品真实性复核注」归下一次验证批收口，不属本 workstream 包。
10. **G/WS merge 前确认 human 批准路径就绪**（F8）：两包均 high + 跨系统 + 安全轴，supervisor-verdict-gate 大概率判 ESCALATE；ARQ-260902-escalate-merge-no-human-path 未结（human 批准的 ESCALATE 无机械合并路径）——merge 前确认批准路径/机械缺口处置，不带缺口进 merge。

## 8. 评审记录

（architect append-only 区；主线程不预判结论。）

**v0.4 单轮 dev-side review（architect，2026-09-03）**：**GREEN-with-fixes**——8 条建议级修正（F1–F8，无阻断级、无回问 human 项）全部回灌本稿，详见 review.md v0.4 段。F1 = 计数 off-by-one（v0.2 同款先例），已冻前钉死（15 包 = 13 live + S merged + 1 abandoned；§6 的 11 更新 + 2 新建 + 1 作废、落 15 pod 同步校正）；F2 = 网关兜底逻辑生成归 G；F3 = 决策 26 分组 producer 归包 6；F4 = 承接映射补 loopback 转发；F5 = 行为碰撞型补 TP-A 冻结验收；F6 = ASR 开通移出前置门簇；F7 = 复验分工（G=机械校验 / 包 2=统计性复验）；F8 = 留根补 ESCALATE merge 路径确认。
