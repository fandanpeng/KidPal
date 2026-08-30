# PROPOSAL · kidpal-mvp 开发包拆分（workstream 设计权威）

> **状态：FROZEN v0.2 · 2026-08-29 · human 盖冻结（凭据：human 立项 ack + architect 单轮 review GREEN[.planning/kidpal-mvp/review.md] + human 裁 F3-1/F3-2[9a→high、6/8→high]）**
> **frozen pin**：`sha1:256fa5dcf67b599b`（raw-content 行尾归一化算法，task-intake-and-slicing.md:53）。**自指规避**：pin 钉的是冻结定稿内容，不落本文件文头（写进来即改内容、pin 必漂移）；权威 pin 以 12 包 `origin.upstreamFrozen` 与 review.md 记录为准，本行仅为人读留痕、非机判基准。
> **workstream**：`.planning/kidpal-mvp/`
> **来源**：人类指令「portfolio planner 给 spec 拆开发包」，经三轮对齐收口（切分轴=能力竖切、测试策略=模拟优先硬件殿后、加 tracer-bullet 冒烟批、WIP=3）。
> **单一事实源**：[spec.md](../../docs/spec.md)（功能/story）+ [interface-contract.md](../../docs/contracts/interface-contract.md)（E1/B1/D3/提醒三契约，已冻结）。
> **riskTier**：本 workstream 整体 high（跨 firmware+server 双实现面、含声学包 9a 与多状态机/跨系统包）；各包 tier 见 §2 表。
> **本件性质**：大 feature 拆包设计权威（change-triage 档③），非 Track B 治理批——故无 landing-manifest/packet-binding 字段；落地 = portfolio-planner 依本件落 11 个 TaskPackage，非主线程 packet-bound。

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 spec §4 的全部 story（F1 课表 / F2 故事 / F3 查字 + §4.0 共享底座）拆成一组**排好序、带依赖、模拟优先、硬件殿后**的可并行 TaskPackage，先用最小量烧一次板子证明「板↔shim↔OpenClaw」架构可行，再在电脑上完成主体开发，最后殿后烧板子做声学与物理验收。

**现状差距**：spec 与契约已就绪，但没有任何开发包拆分；固件基座（`firmware` = 小智 V2.1.0 整树拷贝）无 host 测试基建、无 linux target 配置、无测试目录（2026-08-29 实测：`ls firmware/main/` 无 test*，`grep linux sdkconfig.defaults*` 无命中）；`server/`（shim）预留未建。

**非目标（本批不做）**：
- 不改 `docs/contracts/**` 任何契约（各包只消费契约；疑似契约变更走 architect-reviewer + 人类契约门）。
- 不改 vendor 面 `ESP32-S3-RLCD-4.2-Demo/**`（只读）。
- 不做 spec §9 已标「二期」的项：WebSocket 流式、WakeNet 唤醒词、TLS/wss 安全加固——这些不在本 workstream。
- S-F2.5（云端润色故事）标可选，MVP 后置，不进任何包的必做验收。

## 1. 拆分依据（地面真值）

拆包切法遵循 `.claude/references/task-intake-and-slicing.md` §垂直切片维：**每包承载一个可独立演示/验证的完整能力，不按 DB→API→UI 技术层横切**。KidPal 是跨端系统，一条功能天然跨 `firmware/`+`server/`，故多数包跨两面——这是能力的自然形状，非横切。

三个直接决定拆法的事实约束：

1. **契约已冻结**（interface-contract.md：E1 统一响应 / B1 课表 MD / D3 笔顺 JSON / 提醒文件结构）。包只消费契约；凡疑似要动契约 → 标 `contractBlocked`，先走契约门，不进 Pod。
2. **双实现面已登记**（scope-map.json `owned`）：`firmware/`（小智整树基座，板级 overlay 在 `main/boards/waveshare-s3-rlcd-4.2/`）+ `server/`（shim，未建）。板子是哑终端，智能全在 OpenClaw；固件包只负责「收契约 JSON → 渲染/播报」。
3. **AEC 语音喊停是最大技术风险**（spec §5.2 自标「最大不确定点」、§9「最大技术风险点」），依赖真实声学环境，电脑模拟不出 → 单列 critical 包、殿后实测。

**测试策略分层（本轮对齐产出，固化进各包）**：

| 层 | 能否电脑测 | 手段 |
|---|---|---|
| 纯逻辑（状态机/display解析/课表筛选/贝塞尔/坐标变换） | ✅ 脱离硬件 | ESP-IDF host-based 单测（linux target，编译本机可执行） |
| server（shim） | ✅ 本是 PC 服务 | FastAPI 本机 pytest + httpx，挂真/打桩 OpenClaw |
| UI 渲染 | ⚠️ 半可行 | LVGL SDL 后端 PC 模拟（需包 0 搭建） |
| 硬实时音频（AEC/VAD 喊停） | ❌ 必须真机 | 依赖真实麦克风+扬声器声学 |
| 物理交互/电气（按键/双麦/RTC 断电/低电/RLCD 观感） | ❌ 必须真机 | 烧板子冒烟 |

由此得三段式：**阶段 A 冒烟打通 → 阶段 B 模拟开发 → 阶段 C 硬件验收**。

## 2. 拆分总览（11 包 · 三段 · merge order）

### 阶段 A · 冒烟打通（最小量，先烧一次板子证架构）

| # | changeId | 能力切片 | 面 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|---|
| S | `260829-smoke-e2e` | **架构可行性冒烟**：最小 shim（能转发即可）+ 最小固件（KEY→ASR→OpenClaw→TTS→喇叭出声 + 屏显一行字）+ 配网指向 shim。只证「设备↔shim↔OpenClaw」链路通，零业务 | 板+云 | high | device（先行烧录，唯一前置烧板） | — | 1 |

### 阶段 B · 模拟开发（host/PC 跑，不碰板子）

| # | changeId | 能力切片 | 面 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|---|
| 0 | `260829-host-test-infra` | **host 测试基建**（先行包）：ESP-IDF linux target 单测骨架 + LVGL SDL 屏幕模拟 + host 测试目录约定 | firmware | high | 自身即基建 | — | 2 |
| 1 | `260829-shim-skeleton` | shim 完整化：四接口 `/v1/asr /v1/chat /v1/tts /v1/alerts` + 设备鉴权 + OpenClaw loopback 转发（在 S 的最小 shim 上补全） | server | high | 纯 PC pytest | S | 3 |
| 2 | `260829-voice-roundtrip` | 语音回环完整化 + E1 display 切分下发（正文→TTS、display JSON→板渲染） | 板+云 | high | hybrid（host+PC） | S,0,1 | 4 |
| 3 | `260829-session-logic` | 会话状态机（聆听/思考/播报/超时/结束）+ KEY/BOOT 全局按键手势解析（§4.0/§5.3 纯逻辑部分） | 板 | high | host 状态转移测试 | 0 | 5 |
| 4 | `260829-char-stroke` | 查生字：笔顺 JSON（D3）消费 + 贝塞尔离散 + 1024→400×300 坐标变换 + 1bpp 光栅 + 米字格渲染 + 逐笔动画 + 右栏速查 | 板+云 | high | host（渲染纯函数全测） | 0,2 | 6 |
| 5 | `260829-home-p1` | 首页常显信息牌：RTC 时间 + SHTC3 温湿度 + 今日日程下发缓存 + 低电打盹 | 板 | medium | host + SDL 版面 | 0,2 | 7 |
| 6 | `260829-schedule-p2` | 课表页：今天/明天渲染 + 语音问课表滚动定位 + 家长录入→可消费链路（B1） | 板+云 | high | host 筛选逻辑 + PC 云端 | 0,1 | 8 |
| 7 | `260829-schedule-reminder` | 课表提醒：cron→/v1/alerts→提醒弹窗 + RTC 断网兜底（离线红线）。⚠️ 依赖契约「待 OpenClaw 确认点 3」 | 板+云 | high | host 提醒触发逻辑 | 6 | 9 |
| 8 | `260829-story-p3` | 故事：列表 + 录制上传归档 + 回看/回听 + 成就盖章（story_list/story_text 契约） | 板+云 | high | host + PC 云端 | 0,1 | 10 |

### 阶段 C · 硬件验收（殿后烧板子）

| # | changeId | 能力切片 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|
| 9b | `260829-hardware-validation` | 物理冒烟：按键手感 / 双麦 / RTC 断电续航 / 低电 / RLCD 真屏观感 + 全功能端到端真机 | high | device | 2~8 | 11 |
| 9a | `260829-audio-bargein-hw` | **AEC 语音喊停实测调阈值**（最大技术风险，隔离成片；KEY 单击打断兜底 100% 可靠先行验证） | high | device | 2,9b | 12 |

## 3. 关键路径与并行调度

**关键路径（最长链）**：`S → {0|1} → 2 → {4|5} → 9b → 9a`（6 级）。包 3/6/8 不在最长链上（3 只依赖 0，可与 2 并行）。

- **S 是「前置通行证」**：用最薄实现证明三层架构串通——整个项目的命脉假设。S 绿了才往 B 段投开发量。S 只做联通、零业务，量最小。
- **B 段 9 包全程电脑跑**，拜包 0 host 基建所赐；包 1 在 S 的最小 shim 上补全（不重复造）。
- **C 段殿后烧板子**：9a 声学喊停（最可能反复返工、单独隔离成片）+ 9b 物理冒烟与端到端（较机械）。9a 排在 9b 后，因喊停依赖完整链路先真机就位。

**B 段收口**：9b dependsOn 2~8（全部 B 段功能包），其中**最晚 merge 的是包 8（mergeOrder 10）**——9b 真正等的是 8，而非 mergeOrder 较小的 7。merge 顺序 7(9) 先于 8(10)，但 9b 须待两者都就绪。

**并行调度（WIP=3，human 已裁）**：
- 起跑：S 单独（串行，它是通行证）。
- S 绿后：包 0 + 包 1 并行（互不粘）。
- 0/1 就绪后：**立即可并行的是包 2、3、6、8**（2 需 0/1/S，3 需 0，6 需 0/1，8 需 0/1）；**包 4、5 需待包 2 就绪**（它们依赖 2）后加入并行池。**WIP limit = 3**（同时最多 3 个活跃 Pod）。
- merge 串行化：merge-driver 按 mergeOrder 逐个 rebase+quick-check+merge，触共享文件/契约面者优先（包 0/1 先并，减少后续冲突面）。

**每包 `testHost` 标注**（固化测试策略，供 acceptance-author 写断言；与 §2 表口径一致）：S=device；0=基建自身；1=host(PC)；2=hybrid(host+PC)；3=host；4=host；5=host+SDL；6=host+PC云端；7=host；8=host+PC云端；9a/9b=device。

**硬件强绑定子能力显式殿后 9b**（host 段只测布局/逻辑，真实硬件读取留 device 验收，避免 acceptance 误写成「host 已验」）：包 5 的 RTC 走时 + SHTC3 温湿度直读、包 8 的麦克风采音。

## 4. 契约面与冲突面扫描

- **契约消费，不修改**：11 包全部只读消费 interface-contract.md 的 E1/B1/D3/提醒四契约。无一包在 Pod 内改 `docs/contracts/**`。
- **contractBlocked 标注（完整列）**：依赖契约「待 OpenClaw 确认点」（interface-contract.md §待确认点）的包有三个——包 2（待确认点 1：DeepSeek display 格式遵循度）、包 6（待确认点 2：课表 MD 解析）、包 7（待确认点 3：提醒 cron 可行性）。launch 前须 human 确认对应点已解决或定降级预案（如包 7 降级为「RTC 本地提醒兜底先行、云端 cron 链路待确认后补」）。详见 §7 留根。
- **跨包共享面**：包 0（host 基建）是 2/3/4/5/6/8 的公共依赖 → 必须先 merge（mergeOrder 2，仅次于 S）。包 2（语音回环 + E1 切分）是 4/5/6/8 的公共底座 → mergeOrder 4。此排序最小化跨 Pod 冲突。
- **无常量互斥/行为碰撞**：本批为全新功能开发，无既有冻结验收可撞（`firmware/` 当前无业务验收测试）。

## 5. human 取舍点（已裁，留痕）

| OQ | 选项 | 裁定 | 日期 |
|---|---|---|---|
| 切分轴 | (a) 能力竖切（跨端）/ (b) 按端分层 | **(a)** | 2026-08-29 |
| 测试策略 | (A) 先搭 host 基建 / (B) 不搭全烧板 | **(A)** 先搭基建（包 0） | 2026-08-29 |
| 冒烟批 | 是否加 tracer-bullet 冒烟批 | **加**（包 S 先行烧板证架构） | 2026-08-29 |
| 硬件殿后粒度 | (a) 单一大批 / (b) 拆 9a 声学 + 9b 冒烟 | **(b)** 拆两个 | 2026-08-29 |
| WIP limit | 1 / 2~3 | **WIP=3** | 2026-08-29 |
| 9a riskTier（review F3-1） | critical（原标，不符 §riskTier critical 定义）/ high | **改 high** | 2026-08-29 |
| 6/8 riskTier（review F3-2） | medium（与 cross-system=high 有张力）/ high 求稳 | **升 high 求稳** | 2026-08-29 |

## 6. 验证集（拆包正确性的机械/人工核验）

- 每包 TaskPackage 落盘 `.claude/taskpackages/<changeId>/taskpackage.json`，含 `origin`（upstreamType=planning-workstream 指回本件）/`sourceRefs`/`riskTier`/`gateProfile`/`dependsOn`/`mergeOrder`/`testHost`。
- `node .claude/scripts/portfolio-reconcile.mjs --set-pod <changeId> --status queued --merge-order <N>` 初始化 11 个 Pod 到 board，WIP=3。
- decisionCoverage：11 包 acceptance 并集须覆盖 spec §4 全部 story（S-F1.1~F1.5 / S-F2.1~F2.4 / S-F3.1~F3.3 + §4.0 共享底座）+ §6 接口 + §7 已定决策中可由固件/shim 承载者。落 `.planning/kidpal-mvp/decision-coverage.json`。
- 依赖无环：S→{0,1}→…→9a 为 DAG，mergeOrder 拓扑有序。

## 7. 留根清单

- **S-F2.5 云端润色**：可选，MVP 后置，不进本 workstream 必做验收；若做则单开 Lite 包。
- **二期项**（WebSocket 流式 / WakeNet 唤醒 / TLS 加固 / 家长端形态细化 / 功耗实测优化）：spec §9 已列，本 workstream 不覆盖，留待后续 milestone。
- **契约待确认点 1/2/3**（interface-contract.md §待确认点）：DeepSeek display 格式遵循度、课表 MD 解析、提醒 cron 可行性——不阻塞拆包，但在对应包（2/6/7）launch 前需 human 确认或定降级预案。
- **host 基建的 ESP-IDF linux target 兼容性**：小智工程含 ESP-SR AFE 等硬绑定芯片的组件，包 0 需实测哪些模块可 host 编译、哪些只能 device——这是包 0 自身的验收内容，不预判结论。
- **「4 页循环翻页」跨页整合归属**：§7 决策 14「4 页循环 BOOT 翻页」是穿越包 5/6/8/4 四个真实页面的全局行为；包 3 只做按键手势**解析**纯逻辑，跨页循环的端到端整合 host 阶段无专包认领，**显式声明由包 9b（全功能端到端真机）兜底验收**，各页面内的页内特例（如 P3 阅读态 BOOT 长按翻屏、P2 BOOT 长按今⇄明）仍由各自包验收。

## 8. 评审记录

（architect append-only 区；主线程不预判结论。）
