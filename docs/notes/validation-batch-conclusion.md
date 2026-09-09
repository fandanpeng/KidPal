# KidPal 功能支撑验证批 · 最终结论（V3 设计输入 + 云端 MVP 链路打通证明）

> 日期：2026-09-01（验证批收口，三包已合并入 main）
> 性质：本档是 [.planning/kidpal-validation/](../../.planning/README.md) 功能支撑验证批的**收口汇总件**——PROPOSAL §3 要求三包各出独立报告、收口汇总成《V3 设计输入结论》＋《云端 MVP 链路打通证明》；本档即该汇总，把三份 dev 报告的结论提到一处，供 V3 设计稿与后续主体开发直接引用。
> 逐条实证明细在三份收口报告：[TP-A 云端链路](../../changes/dev/260831-cloud-link-bringup/cloud-link-validation-report.md) · [TP-B UI 可行性](../../changes/dev/260831-ui-art-assets/UI-art-feasibility.md) ＋ [预算](../../changes/dev/260831-ui-art-assets/budget.md) · [TP-C 模拟器对齐](../../changes/dev/260831-simulator-lvgl-align/review-packet.md)。逐点可行性分级在 [feature-inventory.md](feature-inventory.md)，实现策略在 [ui-implementation-strategy.md](ui-implementation-strategy.md)。
> 术语：OC-n = OpenClaw 云端对接功能点；CLB / SLA / UAA = 三个验证包（TP-A 云端 / TP-C 模拟器 / TP-B 资产）各自的验收项；OQ-3(a) = human 对阈值化截图「结构＋风格保真、不追像素级」的目测裁定口径；E1 / B1 / D3 = display 响应块 / 课表 MD / 笔顺 JSON 三个契约结构。

## 0. 一句话结论

**需求定稿后两条还没实证的"能不能实现"主线——UI 美术可行性 与 OpenClaw 云端链路——现在都实证成立，没有一项走不通；可以据此放开做 V3 设计稿（实现导向）。** 双层方案「LVGL 原生控件扛骨架＋预栅格化位图资产扛美术」经全量验证仍然立得住。

## 1. 云端 MVP 链路打通证明（TP-A）

「文字 → OpenClaw → 正文＋display 块 → shim 切分 → TTS 出声」这条最小闭环整条跑通。立项时实盘查出的"四块全没就位"（OpenAI 兼容端点 404、shim 不存在、faster-whisper / edge-tts 未装）已全部补齐：

| 项 | 结论 |
|---|---|
| OpenAI 兼容端点 `POST /v1/chat/completions`（OC-5） | 已开通，返回正常 completion |
| **DeepSeek display 格式遵循度（OC-6，命门）** | **12/12**——出现率与块内 JSON 可解析率均 1.0、失败样本 0；"语音＋屏幕双路"的 E1 方案成立，**不用换切分** |
| ECS 算力（OC-10） | 2 核 / 1.8 GB，本地 faster-whisper 只能 tiny 档，base/small 不可行 |
| ASR faster-whisper（OC-8） | tiny 中文短句转写正确、全链路 1.92s；Python 3.6.8 已另建 venv（3.11）解决 |
| TTS edge-tts（OC-9） | 晓晓音色童声 mp3 可播 |
| 最小 shim（OC-7） | `server/shim.py` `/v1/chat` 一路通（零业务逻辑薄桥），asr/tts 独立口＋alerts 留 stub |
| schedule.md 消费（OC-11） | DeepSeek 按 B1 课表结构正确筛选＋吐 `view:schedule` |

**9 项验收功能面全过**（冻结验收复跑 8/9：唯一那条红是"查字"请求冷启动 109 秒踩爆 90 秒 curl 超时的伪红，格式遵守已独立复证 12/12）。

### 头号延迟风险（进 M2）
**char 查询（查字＋笔顺 JSON）冷启动约 109 秒**，是普通文本查询（4.9s）/故事列表（17.9s）的 6~20 倍；瓶颈在 DeepSeek 侧 makemeahanzi 查字＋笔顺生成，不在 ECS 算力。MVP 同步 HTTP 形态下这是最刺眼的一条——**M2 须上流式/异步＋字库结果缓存**。

## 2. UI 美术资产可行性 ＋ 预算（TP-B）

V2 设计稿里"方案已定但没出图"的美术元素**全部走资产管线出图并机判转绿**：吉祥物 7 姿态、手绘不规则框族 9 宫格、元素倾斜（含 -8° 大角印章）、三档网点、14 枚全图标族、站酷快乐体（授权落档＋真体栅格验证）、Baloo 2 动态时间 imgfont。

- **没有 🔴 级（做不到）项，也没有"方案定了还没落地"的残留。**
- **预算**：已出图 47 枚真实合计 **599,698 字节**；加 P2–P4 拟用估算后预计设备总占用约 **3.85 MB**，对 SPIFFS assets 分区 8 MB 余量 **54.1%**——**不触发 I1 索引格式优化**（那条"真机可回收 16 倍空间"项因此维持原优先级、不升 P0）。

## 3. 地基可信（TP-C）

模拟器从 LVGL 9.5 降到与固件同版 9.3.x，消除"模拟器过 ≠ 真机过"的版本错位；1bpp 阈值化快照自查正式化。**正因为这一层，上面 UI 的模拟器目测结论才有"≈真机"的效力**——否则 TP-B 整套观判断都建立在失真地基上。

## 4. 给 V3 设计稿的设计输入结论

1. **可以放开画**：美术元素全数可行、预算有余、无需提前上 I1。
2. **倾斜 / 手绘框 / 网点全部烘进离线资产**（已实证，运行时零成本）。图标族取舍落定 = 离线位图资产（14 枚一次性绘形），图标字（imgfont）只留给 B7 那种"同字形逐字符变化的动态时间数字"。
3. **站酷快乐体授权可用**（SIL OFL 1.1，免费商用/可内嵌/可子集化）；子集化重分发保守按"改名分发"规避 Reserved Font Name 禁则。
4. V3 必须带三条**约束/待锁**（下方 §5/§6 展开）。

## 5. 归 human 目测裁定的观感保真（OQ-3(a)，锁 V3 前）

以下几处机判只能证"在场/尺寸/占比"，观感是否过关要你看阈值化 PNG 定：

1. **框族 255px 大半径角** 的 9 宫格 24px 角片忠实性——大半径"平缓手绘角"会被小角片裁成"紧弯角"，扭曲手绘感；不可接受则改固定整框或分档角片。
2. **-8° 印章内圈 2.5px 虚线**——是全场最细笔画，-8° 后有无断口/糊连；备选：虚线增粗至 3.5px / 降角至 -5° / 两圈分离叠加。
3. 造型（摇头耳朵是否成菱形）、网点密度观感（d50/d25/d12）保真。

快照在 `changes/dev/260831-ui-art-assets/snapshots/` 与 `tools/asset-pipeline/report/snapshots/`。

## 6. 真机留根项（本批不烧真机，登记待后续）

1. **I1 格式在真机 9.3 + SPIFFS 文件源路径**可用性未验证（预算既然不超，优先级维持现状）。
2. **16px 以下 CJK 在 1bpp 真机可读性**未实测——直接决定 V3 字号下限。
3. **P4 笔顺字盘打包策略**（V3 前必须锁）：不能"每笔每态各烘一份整帧"（保守预估可达 ~3.1 MB 顶到预算），须走"笔形 3 态叠加复用＋米字格单份背景"。
4. char 查询延迟流式化（见 §1）＋ 前光确认 / AEC 实测 / 功耗。

## 7. 契约面处置

本批**零触契约面** `docs/contracts/**`。验证暴露两处需正式定稿的缺口，均已登记走契约/治理门、未在本批直改：shim `/v1/*` 字段信封 schema 未入契约 §九（ARQ-260901-shim-api-contract-gap）、G3 闸 server 面 host/live 区分规范待 architect 固化（ARQ-260901-g3-server-live-acceptance）。

## 8. 美术资产集成（U 包 260901-ui-art-integration）验收锚点

> 2026-09-09 integrator closeout 按 append 纪律追加，上文各节零改动。

本 notes 的 TP-B UI 可行性结论（§2：双层方案成立、无 🔴 级项、预算不超）与 §6 真机留根项，对应美术资产集成 U 包的冻结验收（`firmware/main/test/ui_art_integration.acceptance.c`，docRef 锚点）逐项落位如下：

- **ART-A5 资产出图落地**：§2「全部走资产管线出图并机判转绿」的资产本体经 V3 设计稿定稿后落地为 `firmware/main/assets/spiffs_root/` 47 枚闭集（吉祥物 7 姿态/14 枚图标族/手绘框族/网点/站酷快乐体 + Baloo 2）。
- **ART-A2~A4 固件侧管线**：§2 双层方案中「预栅格化位图资产扛美术」的固件承载半——SPIFFS assets 分区 + 1bpp 位图加载（ART-A2）、imgfont 动态时间数字（ART-A3，对应 §4 第 2 条「imgfont 只留给动态时间数字」取舍）、9-slice 手绘框族 + 抖动网点 d50/d25/d12（ART-A4）。ART-A1 为装配结构齿。
- **ART-A6 六页换皮**：各 UI 渲染面统一接入共享资产层（消费 §4 第 2 条「倾斜/手绘框/网点全部烘进离线资产，运行时零成本」的落地面）。
- **ART-A7 host 回归**：换皮后各页面 host 回归（布局/逻辑不破），独立于真机观感通道。
- **殿后项对应**：§6 第 1/2 条真机留根（I1 真机文件源路径、16px 以下 CJK 1bpp 真机可读性）+ SPIFFS 真机文件源路径与真机观感，归 9b（ART-E2 显式殿后，本包 host+SDL 通道不覆盖）。
