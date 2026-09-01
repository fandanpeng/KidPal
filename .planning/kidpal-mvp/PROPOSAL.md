# PROPOSAL · kidpal-mvp 开发包拆分（workstream 设计权威）

> **状态：FROZEN v0.3 · 2026-09-01 · human 盖冻结（凭据：human 立项 ack + architect 单轮 review GREEN[.planning/kidpal-mvp/review.md] + human 三项拍板 C1/C2/C3 二次修订）**
> **frozen pin**：权威 pin 以 13 包 `origin.upstreamFrozen` 与 review.md 记录为准（raw-content 行尾归一化算法，task-intake-and-slicing.md:53）。**自指规避**：pin 不入本文件文头（写进来即改内容、pin 必漂移）；本行仅为人读留痕、非机判基准。
> **workstream**：`.planning/kidpal-mvp/`
> **来源**：人类指令「portfolio planner 给 spec 拆开发包」，经三轮对齐收口（切分轴=能力竖切、测试策略=模拟优先硬件殿后、加 tracer-bullet 冒烟批、WIP=3）+ 2026-09-01 三项拍板二次修订（C1 char 延迟修法 / C2 新增美术集成包 / C3 验证批事实更新）。
> **单一事实源**：[spec.md](../../docs/spec.md)（功能/story）+ [interface-contract.md](../../docs/contracts/interface-contract.md)（E1/B1/D3/提醒三契约，已冻结）。
> **riskTier**：本 workstream 整体 high（跨 firmware+server 双实现面、含声学包 9a 与多状态机/跨系统包）；各包 tier 见 §2 表。
> **本件性质**：大 feature 拆包设计权威（change-triage 档③），非 Track B 治理批——故无 landing-manifest/packet-binding 字段；落地 = portfolio-planner 依本件落 13 个 TaskPackage，非主线程 packet-bound。

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 spec §4 的全部 story（F1 课表 / F2 故事 / F3 查字 + §4.0 共享底座）拆成一组**排好序、带依赖、模拟优先、硬件殿后**的可并行 TaskPackage，先用最小量烧一次板子证明「板↔shim↔OpenClaw」架构可行，再在电脑上完成主体开发，最后殿后烧板子做声学与物理验收。

**现状差距**：spec 与契约已就绪；**云端链路已实证打通**（2026-09-01 验证批：`/v1/chat` 全链路通、DeepSeek display 格式遵循度 12/12、faster-whisper ASR＋edge-tts TTS 就位、最小 shim `server/shim.py` 已并入 main）。**剩余差距集中在固件侧**——固件基座（`firmware` = 小智 V2.1.0 整树拷贝）无 host 测试基建、无 linux target 配置、无测试目录（2026-08-29 实测）；V3 设计稿的美术资产尚未落地固件（资产管线出图已实证，固件侧 SPIFFS 资产加载基建未建）；char 查询延迟 109s 待用服务端静态字库方案消解。

**非目标（本批不做）**：
- 不改 `docs/contracts/**` 任何契约（各包只消费契约；疑似契约变更走 architect-reviewer + 人类契约门）。
- 不改 vendor 面 `ESP32-S3-RLCD-4.2-Demo/**`（只读）。
- 不做 spec §9 已标「二期」的项：WebSocket 流式、WakeNet 唤醒词、TLS/wss 安全加固——这些不在本 workstream。
- S-F2.5（云端润色故事）标可选，MVP 后置，不进任何包的必做验收。

## 1. 拆分依据（地面真值）

拆包切法遵循 `.claude/references/task-intake-and-slicing.md` §垂直切片维：**每包承载一个可独立演示/验证的完整能力，不按 DB→API→UI 技术层横切**。KidPal 是跨端系统，一条功能天然跨 `firmware/`+`server/`，故多数包跨两面——这是能力的自然形状，非横切。

四个直接决定拆法的事实约束：

1. **契约已冻结**（interface-contract.md：E1 统一响应 / B1 课表 MD / D3 笔顺 JSON / 提醒文件结构）。包只消费契约；凡疑似要动契约 → 标 `contractBlocked`，先走契约门，不进 Pod。
2. **双实现面已登记**（scope-map.json `owned`）：`firmware/`（小智整树基座，板级 overlay 在 `main/boards/waveshare-s3-rlcd-4.2/`）+ `server/`（shim）。板子是哑终端，智能全在 OpenClaw；固件包只负责「收契约 JSON → 渲染/播报」。**本修订起 shim 从「预留未建」前移为「已实证」**——验证批 `server/shim.py`（`/v1/chat` 已通、asr/tts/alerts 为 stub）已并入 main，包 S/1 直接复用/补全。
3. **AEC 语音喊停是最大技术风险**（spec §5.2 自标「最大不确定点」、§9「最大技术风险点」），依赖真实声学环境，电脑模拟不出 → 单列独立风险包、殿后实测。
4. **char 查询延迟瓶颈已定位**（2026-09-01 验证批，C1 依据）：延迟随输出体量放大（text 4.9s / story_list 17.9s / char 109.1s），大头是 DeepSeek 把 strokes/medians 坐标 JSON 逐 token 复述；makemeahanzi 是静态字库，应确定性查表、不走生成路径 → 决定包 4 云侧范围（见 §2/§4）。

**测试策略分层（本轮对齐产出，固化进各包）**：

| 层 | 能否电脑测 | 手段 |
|---|---|---|
| 纯逻辑（状态机/display解析/课表筛选/贝塞尔/坐标变换） | ✅ 脱离硬件 | ESP-IDF host-based 单测（linux target，编译本机可执行） |
| server（shim） | ✅ 本是 PC 服务 | FastAPI 本机 pytest + httpx，挂真/打桩 OpenClaw |
| UI 渲染 | ⚠️ 半可行 | LVGL SDL 后端 PC 模拟（需包 0 搭建） |
| 硬实时音频（AEC/VAD 喊停） | ❌ 必须真机 | 依赖真实麦克风+扬声器声学 |
| 物理交互/电气（按键/双麦/RTC 断电/低电/RLCD 观感） | ❌ 必须真机 | 烧板子冒烟 |

由此得三段式：**阶段 A 冒烟打通 → 阶段 B 模拟开发 → 阶段 C 硬件验收**。

## 2. 拆分总览（13 包 · 三段 · merge order）

### 阶段 A · 冒烟打通（最小量，先烧一次板子证架构）

| # | changeId | 能力切片 | 面 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|---|
| S | `260829-smoke-e2e` | **架构可行性冒烟（缩域，C3）**：复用验证批已并入 main 的 `server/shim.py`（不自建 shim），只做**设备侧闭环**——KEY→麦克风→shim→TTS→喇叭出声 + 屏显一行字 + 配网指向 shim。只证「设备↔shim↔OpenClaw」链路通，零业务 | 板+云 | high | device（先行烧录，唯一前置烧板） | — | 1 |

### 阶段 B · 模拟开发（host/PC 跑，不碰板子）

| # | changeId | 能力切片 | 面 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|---|
| 0 | `260829-host-test-infra` | **host 测试基建**（先行包）：ESP-IDF linux target 单测骨架 + LVGL SDL 屏幕模拟 + host 测试目录约定 | firmware | high | 自身即基建 | — | 2 |
| 1 | `260829-shim-skeleton` | shim 完整化（**起点前移，C3**）：在验证批 `server/shim.py`（`/v1/chat` 已通、asr/tts/alerts 为 stub）上补全四接口 `/v1/asr /v1/chat /v1/tts /v1/alerts` + 设备鉴权 + OpenClaw loopback 转发。**launch 前置闸：先处置 ARQ-260901-shim-api-contract-gap**（§4/§7） | server | high | 纯 PC pytest | S | 3 |
| 2 | `260829-voice-roundtrip` | 语音回环完整化 + E1 display 切分下发（正文→TTS、display JSON→板渲染）。**contractBlocked 已解除**（待确认点 1 实证 12/12） | 板+云 | high | hybrid（host+PC） | S,0,1 | 4 |
| 3 | `260829-session-logic` | 会话状态机（聆听/思考/播报/超时/结束）+ KEY/BOOT 全局按键手势解析（§4.0/§5.3 纯逻辑部分） | 板 | high | host 状态转移测试 | 0 | 5 |
| 4 | `260829-char-stroke` | 查生字：笔顺 JSON（D3）消费 + 贝塞尔离散 + 1024→400×300 坐标变换 + 1bpp 光栅 + 米字格渲染 + 逐笔动画 + 右栏速查。**云侧范围扩充（C1）**：服务端静态字库查表拼装 D3 块 + 目标字表预热，LLM 只出意图＋字（详见 §4/§7 闸 A） | 板+云 | high | host（渲染纯函数全测） | 0,2 | 6 |
| 5 | `260829-home-p1` | 首页常显信息牌：RTC 时间 + SHTC3 温湿度 + 今日日程下发缓存 + 低电打盹 | 板 | medium | host + SDL 版面 | 0,2 | 7 |
| 6 | `260829-schedule-p2` | 课表页：今天/明天渲染 + 语音问课表滚动定位 + 家长录入→可消费链路（B1）。**contractBlocked 已解除**（待确认点 2 实证 OC-11） | 板+云 | high | host 筛选逻辑 + PC 云端 | 0,1 | 8 |
| 7 | `260829-schedule-reminder` | 课表提醒：cron→/v1/alerts→提醒弹窗 + RTC 断网兜底（离线红线）。⚠️ 仍依赖契约「待 OpenClaw 确认点 3」（提醒 cron 未验证），contractBlocked 维持 | 板+云 | high | host 提醒触发逻辑 | 6 | 9 |
| 8 | `260829-story-p3` | 故事：列表 + 录制上传归档 + 回看/回听 + 成就盖章（story_list/story_text 契约） | 板+云 | high | host + PC 云端 | 0,1 | 10 |
| U | `260901-ui-art-integration` | **美术资产集成（新，C2）**：固件侧资产管线落成（SPIFFS assets 分区 + 1bpp 位图/imgfont/9-slice/抖动网点加载）+ V3 美术资产本体落地（吉祥物/手绘框族/网点/图标族/字体）+ 各 UI 面统一接入换皮 | 板 + tools | high | host+SDL（资产加载/渲染）+ device 观感殿后 9b | 0,2,4,5,6,7,8 | 11 |

### 阶段 C · 硬件验收（殿后烧板子）

| # | changeId | 能力切片 | riskTier | 测试 | dependsOn | mergeOrder |
|---|---|---|---|---|---|---|
| 9b | `260829-hardware-validation` | 物理冒烟：按键手感 / 双麦 / RTC 断电续航 / 低电 / RLCD 真屏观感 + 全功能端到端真机。**补两条（C3）**：16px 以下 CJK 1bpp 真机可读性（定 V3 字号下限）+ 前光确认 | high | device | 2~8 + U | 12 |
| 9a | `260829-audio-bargein-hw` | **AEC 语音喊停实测调阈值**（最大技术风险，隔离成片；KEY 单击打断兜底 100% 可靠先行验证） | high | device | 2,9b | 13 |

**新包 U 与页面包的关系（设计取舍，详见 §3）**：页面包 2/4/5/6/7/8 **不 dependsOn U**，先用「LVGL 原生骨架 + 占位资产」跑绿，不被未出的 V3 设计稿锁死；U **殿后**提供资产基建＋资产本体＋统一换皮，dependsOn 反过来覆盖这些页面包（0,2,4,5,6,7,8），mergeOrder 落在最晚页面包（8）之后、9b 之前。包 3 纯状态机逻辑无渲染，不入 U 换皮范围（其 mascot pose 演出由消费它的页面承载、随 U 一并换皮）。

## 3. 关键路径与并行调度

**关键路径（最长链）**：`S → {0|1} → 2 → {4|5} → U → 9b → 9a`（**7 级**，较 v0.2 的 6 级 **+1**——新增美术集成包 U 作为 9b 前的一段串行）。包 3/6/8 不在最长链上（3 只依赖 0，6/8 只依赖 0/1，可与 2 并行）；U 依赖 4/5/6/7/8 中的最晚者（按依赖深度 4/5 经 2、7 经 6 都达 level 4），故 U 实际等到的是「4/5 之一 + 7（经 6）就位」后的那一步，再串 U(11)→9b(12)→9a(13)。

- **S 是「前置通行证」**：用最薄实现证明三层架构串通——整个项目的命脉假设。S 绿了才往 B 段投开发量。S 只做联通、零业务（缩域后不自建 shim、直接复用 `server/shim.py`，量更小）。
- **B 段 10 包全程电脑跑**（9 个原有功能包 + 新包 U），拜包 0 host 基建所赐；包 1 在验证批 shim 上补全（不重复造）。包 1 dependsOn S 是**验证序依赖**（S 先实证「板↔shim↔OpenClaw」链路、包 1 再扩 shim 四接口＋鉴权，非产物依赖——S 缩域后不自建 shim）。
- **新包 U 殿后于页面包、不反锁页面**：页面包先用 LVGL 原生骨架 + 占位资产跑绿；U 提供资产基建 + 资产本体 + 统一换皮。U 的 launch 额外受前置输入「V3 设计稿」约束（§7）。U 是**跨页共享资源层切片**（非技术层横切）：页面垂直切片经占位资产先行保全，U 作迁移批殿后，9b 作整合复验。U 标 high 的依据（非跨板+云，而是）：blast radius 覆盖全 UI 面（唯一跨 4/5/6/7/8 渲染路径的包）+ SPIFFS assets 分区级改动 + 真机强绑定（SPIFFS 文件源路径/1bpp 观感/16px CJK/前光）+ 串 U→9b→9a 关键路径中段的 9b last gate。
- **C 段殿后烧板子**：9a 声学喊停（最可能反复返工、单独隔离成片）+ 9b 物理冒烟与端到端（较机械）。9a 排在 9b 后，因喊停依赖完整链路先真机就位。

**B 段收口**：9b dependsOn 2~8 + U（全部 B 段功能包 + 美术集成包；不含包 0 host 基建——真机测试不需要 host 基建），其中**最晚 merge 的是 U（mergeOrder 11）**——9b 真正等的是 U（换皮后的 UI），而非 mergeOrder 较小的 7(9)/8(10)。merge 顺序 7(9)、8(10)、U(11) 先于 9b(12)，但 9b 须待全部就绪。

**并行调度（WIP=3，human 已裁，不变）**：
- 起跑：S 单独（串行，它是通行证）。
- S 绿后：包 0 + 包 1 并行（互不粘）。
- 0/1 就绪后：**立即可并行的是包 2、3、6、8**（2 需 0/1/S，3 需 0，6 需 0/1，8 需 0/1）；**包 4、5 需待包 2 就绪**后加入并行池；**包 U 需待 2/4/5/6/7/8 全部就绪且 V3 设计稿已出**后加入。WIP limit = 3（同时最多 3 个活跃 Pod）。
- merge 串行化：merge-driver 按 mergeOrder 逐个 rebase+quick-check+merge，触共享文件/契约面者优先（包 0/1 先并，减少后续冲突面）。U 殿后 merge（11），避免与 2/4/5/6/7/8 的渲染代码并行冲突。

**每包 `testHost` 标注**（固化测试策略，供 acceptance-author 写断言；与 §2 表口径一致）：S=device；0=基建自身；1=host(PC)；2=hybrid(host+PC)；3=host；4=hybrid（host 渲染 + PC 字库查表单测 + live char 延迟验收）；5=host+SDL；6=host+PC云端；7=host；8=host+PC云端；U=host+SDL（资产加载/渲染 host 验证）+ device 观感殿后 9b；9a/9b=device。

**硬件强绑定子能力显式殿后 9b**（host 段只测布局/逻辑，真实硬件读取留 device 验收，避免 acceptance 误写成「host 已验」）：包 5 的 RTC 走时 + SHTC3 温湿度直读、包 8 的麦克风采音、包 U 的 SPIFFS 真机文件源路径 + 1bpp 真机观感 + 16px 以下 CJK 真机可读性 + 前光确认。

## 4. 契约面与冲突面扫描

- **契约消费，不修改**：13 包全部只读消费 interface-contract.md 的 E1/B1/D3/提醒四契约。无一包在 Pod 内改 `docs/contracts/**`。
- **contractBlocked 标注（更新后仅剩包 7）**：原三个 blocked 包——包 2（待确认点 1）、包 6（待确认点 2）、包 7（待确认点 3）。**验证批已实证点 1（DeepSeek display 遵循度 12/12）与点 2（课表 MD 解析 OC-11）→ 包 2、6 解除**；**点 3（提醒 cron 可行性）未验证 → 包 7 维持**，降级预案（RTC 本地提醒兜底先行、云端 cron 链路待确认后补）保留。
- **本修订新增两处契约前置闸（C1/C3，见 §7 留根）**：
  - **闸 A（C1）**：契约 §1.6「对 OpenClaw 的要求」system prompt 对「DeepSeek 输出 char 数据方式」的约束需随静态字库方案调整（不再由 DeepSeek 生成 D3 块数据、改为 OpenClaw 侧查表拼装）→ **包 4 launch 前走 architect-reviewer 四分类预审**；E1/D3 格式本身不变（不触契约冻结面，只可能改 §1.6 描述性措辞）。
  - **闸 B（C3）**：`ARQ-260901-shim-api-contract-gap`（shim `/v1/*` 字段信封 schema 未入契约，§九为拟新增节）→ **包 1 launch 前走契约门四分类**。
- **跨包共享面**：包 0（host 基建）是 2/3/4/5/6/8/U 的公共依赖 → 必须先 merge（mergeOrder 2，仅次于 S）。包 2（语音回环 + E1 切分）是 4/5 的公共底座 → mergeOrder 4。新包 U（美术集成）对 2/4/5/6/7/8 的渲染代码做统一换皮 → **殿后 merge（mergeOrder 11）**，最小化与页面包的冲突面。
- **常量互斥型**：无（本批为全新功能 + 全新资产，无多包/已冻结 spec 对同一常量 pin 互斥值）。
- **行为碰撞型（本次拍板新增一处，launch 前预警）**：新包 U 的统一换皮会改页面包 2/4/5/6/7/8 已写就的「占位资产」渲染路径；若换皮撞到页面包 host acceptance 中对占位资产的断言，按**冻结面优先**处理——U 不得 supersede 页面包既有验收，换皮验收独立成 U 的 acceptance，**U 的 acceptance 显式包含「换皮后各页面 host 回归（布局/逻辑不破）」一条（独立于 9b 真机的快廉捕获点）**，页面原有验收另由 9b 真机端到端复验为附加通道。此交叉面在 U launch 前由 planner 显式核对，不带冲突进 U launch。

## 5. human 取舍点（已裁，留痕）

| OQ | 选项 | 裁定 | 日期 |
|---|---|---|---|
| 切分轴 | (a) 能力竖切（跨端）/ (b) 按端分层 | **(a)** | 2026-08-29 |
| 测试策略 | (A) 先搭 host 基建 / (B) 不搭全烧板 | **(A)** 先搭基建（包 0） | 2026-08-29 |
| 冒烟批 | 是否加 tracer-bullet 冒烟批 | **加**（包 S 先行烧板证架构） | 2026-08-29 |
| 硬件殿后粒度 | (a) 单一大批 / (b) 拆 9a 声学 + 9b 冒烟 | **(b)** 拆两个 | 2026-08-29 |
| WIP limit | 1 / 2~3 | **WIP=3** | 2026-08-29 |
| 9a riskTier（review F3-1） | critical（原标）/ high | **改 high** | 2026-08-29 |
| 6/8 riskTier（review F3-2） | medium / high 求稳 | **升 high 求稳** | 2026-08-29 |
| C1 char 查询 109s 修法 | (a) 服务端静态字库查表 + 预热 / (b) 流式/其他 | **(a)** 服务端静态字库（makemeahanzi）＋目标字表预热＋LRU 缓存，LLM 只出意图＋字，挂 OpenClaw 侧、绝不入 shim | 2026-09-01 |
| C2 美术资产落地归属 | (a) 新增「美术资产集成包」认领 / (b) 各页面包各自认领 | **(a)** 新增第 13 包（`260901-ui-art-integration`），依赖包 0，B 段殿后合入；V3 设计稿为前置输入 | 2026-09-01 |
| C3 验证批轻量事实更新 | 契约待确认点 1/2 实证解封、S 缩域、包 1 起点前移、9b 补真机验收 | **照收口结论更新**（见 §0/§2/§4/§7） | 2026-09-01 |

## 6. 验证集（拆包正确性的机械/人工核验）

- 每包 TaskPackage 落盘 `.claude/taskpackages/<changeId>/taskpackage.json`，含 `origin`（upstreamType=planning-workstream 指回本件）/`sourceRefs`/`riskTier`/`gateProfile`/`dependsOn`/`mergeOrder`/`testHost`。
- `node .claude/scripts/portfolio-reconcile.mjs --set-pod <changeId> --status queued --merge-order <N>` 初始化 **13 个** Pod 到 board，WIP=3。
- decisionCoverage：**13 包** acceptance 并集须覆盖 spec §4 全部 story（S-F1.1~F1.5 / S-F2.1~F2.4 / S-F3.1~F3.3 + §4.0 共享底座）+ §6 接口 + §7 已定决策中可由固件/shim 承载者 + **§5.6/§5.7 UI 美术层（含 §7 决策 17 美术资产管线，新包 U 承载）**；§5.6 教学字体红线（米字格示范字须标准规范字形）由**包 4（渲染侧，主）＋ U（字体资产侧，确认）**共担，落 decision-coverage.json 时显式映射。落 `.planning/kidpal-mvp/decision-coverage.json`（冻结后由主线程同步 12→13 映射：新增 U 承载项 + 9b 跨包换皮兜底）。
- 依赖无环：S→{0,1}→…→U→9b→9a 为 DAG，mergeOrder 拓扑有序。

## 7. 留根清单

- **S-F2.5 云端润色**：可选，MVP 后置，不进本 workstream 必做验收；若做则单开 Lite 包。
- **二期项**（WebSocket 流式 / WakeNet 唤醒 / TLS 加固 / 家长端形态细化 / 功耗实测优化）：spec §9 已列，本 workstream 不覆盖，留待后续 milestone。
- **契约待确认点 1/2/3**（interface-contract.md §待确认点）：点 1（DeepSeek display 遵循度）与点 2（课表 MD 解析）已由验证批实证解决；**点 3（提醒 cron 可行性）仍未验证 → 包 7 launch 前需 human 确认或沿用 RTC 本地兜底降级预案**。
- **前置闸 A（C1）**：契约 §1.6「对 OpenClaw 的要求」system prompt 对「DeepSeek 输出 char 数据方式」的约束需随静态字库方案调整（不再由 DeepSeek 生成 D3 块数据、改为 OpenClaw 侧查表拼装）——包 4 launch 前走 architect-reviewer 四分类预审；E1/D3 格式本身不变。落地顺序（C1 拆包内）：包 4 先做 usage/stream profiling 定死延迟三段比例，再落「汉字→D3 块」确定性查表拼装（数据源 makemeahanzi，挂 OpenClaw 侧、守 §1.5 薄桥红线绝不入 shim），LLM 只出意图＋字，目标字表（小学低年级常用字/课本生字）预热＋LRU 缓存；验收预期 char 查询 109s → 5 秒档（与 text 同级）。**测试宿主**：查表拼装模块走 PC 级单测（逐字对照 makemeahanzi 数据源）；char 延迟 5 秒档验收对真 ECS/OpenClaw 跑、按 ARQ-260901-g3-server-live-acceptance 口径标 live（不 auto-run）。
- **前置闸 B（C3）**：`ARQ-260901-shim-api-contract-gap`（shim `/v1/*` 字段信封 schema 未入契约，§九为拟新增节）→ 包 1 launch 前走契约门四分类。
- **V3 设计稿（新包 U 的显式前置输入，C2）**：尚未开始；其设计输入 = `docs/notes/validation-batch-conclusion.md` §4（美术全数可行、预算有余、倾斜/手绘框/网点烘进离线资产、站酷快乐体授权可用）；OQ-3(a) 三项（框族 255px 大半径角 / -8° 印章内圈虚线 / 造型与网点密度）目测 human 已通过。**U 依赖 V3 设计稿出资产母版，设计稿未出则 U 不得 launch**。同期登记 §6 真机留根：I1 格式真机 9.3 + SPIFFS 文件源路径可用性（资产预算 3.85MB < 8MB 分区、余 54.1%，不超，优先级维持现状）。
- **host 基建的 ESP-IDF linux target 兼容性**：小智工程含 ESP-SR AFE 等硬绑定芯片的组件，包 0 需实测哪些模块可 host 编译、哪些只能 device——这是包 0 自身的验收内容，不预判结论。
- **「4 页循环翻页」跨页整合归属**：§7 决策 14「4 页循环 BOOT 翻页」是穿越包 5/6/8/4 四个真实页面的全局行为；包 3 只做按键手势**解析**纯逻辑，跨页循环的端到端整合 host 阶段无专包认领，**显式声明由包 9b（全功能端到端真机）兜底验收**，各页面内的页内特例（如 P3 阅读态 BOOT 长按翻屏、P2 BOOT 长按今⇄明）仍由各自包验收。
- **9b 新增两条真机验收（C3）**：① 16px 以下 CJK 在 1bpp 真机可读性（直接决定 V3 字号下限）；② 前光确认（反射屏暗处可见性，spec §5.6 待确认项）。
- **char 数据「设备端热字包」候选方案（缓议，2026-09-01 human 拍「先不动设备，真机测试后看效果再说」）**：两级形态 = 设备端热字包（目标字集本地查表，打包后 ~0.5–1MB，assets 分区余量 ~4MB 可容）＋服务端全字库兜底；可得毫秒级首帧＋离线回看，代价是 spec §7 决策 1「数据全在 OpenClaw」需按「预取缓存」定性、分区余量共享、字集增长无轻量通道。**复议触发 = 真机测试后 char 查询实际体验**。本稿维持纯服务端字库。

## 8. 评审记录

（architect append-only 区；主线程不预判结论。）

**v0.3 单轮 dev-side review（architect，2026-09-01）**：GREEN，附 10 条建议级修正（F1–F10，全文本级、主线程回灌、无回问 human 项）。四项确认判断：U 成立为合法切片（F1）、U=high 站得住（F4）、包 1 dependsOn S 转验证序依赖仍成立（F10）、C1 守契约 §1.5 薄桥红线。详见 review.md v0.3 段。
