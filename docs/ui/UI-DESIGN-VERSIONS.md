# KidPal UI 设计稿版本声明（design version declaration）

> 本文件是 `docs/ui/kidpal-ui-design-vN.html` 版本族的**唯一声明面**（261027-gov-review-p5-spike-anchor-cluster T2 · ARQ-261017）。
> 形态镜像 `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 的先例：**声明面记当前版本 + 全量沿革，其它文面只留指针、不复述版本号**。
> 消费者：机检 `spec-anchor-check.mjs`（防御式解析本文件的 `current:` 行）、story 锚行（`write-story` 锚行规则第 4 条）、出图（`requirement-change` 档 2 流程 4b）、TaskPackage `designRefs[]`。
> 维护纪律：**母版升版 = 内容完整落盘 + 本文件 `current:` 行 + 逐版备注追加，同一次落盘**（禁中间态）；本文件 append-only，历史行不回改。
> 版本编号两族（判读歧义已实测）：本文件的 `vN` 是**设计 HTML 版号**；`UIUX-DESIGN-BRIEF.md` 头部 `版本：vN` 是 **brief 自身的版本**——brief 只在需求批升版，设计 HTML 在自己的版式批也升，两族自 2026-09 起错位（brief v5 ↔ 设计 v6，brief v6 ↔ 设计 v7）。引用设计稿一律用本文件，不引 brief 版本号。

## 机读声明（全文恰一行 `current:`，行首匹配；勿改格式）

current: kidpal-ui-design-v9.html

## 逐版备注（append-only；新版本追加在末尾）

| 版本 | 落盘日期 | 升版批 / 触发 | 要点（`data-od-id` 计数为实测） |
|---|---|---|---|
| （版本化前基座）`kidpal-ui-design.html` | 2026-08-27 | 治理迁移入仓（commit `9d626251`）后的初版 | 19 个 `data-od-id`，页级粗锚（`page-*` / `state-*` / `components`），无版本化命名 |
| v2 | 2026-08-29 | commit `770c3009`（吉祥物重绘 + P4 米字格改楷体） | 61 个 id；锚粒度细化为画布 / 状态 / 子态（`canvas-pX` / `state-*` / `shot-*`），首个完整母版 |
| v3 | 2026-09-09 | commit `54280819`（V3 定稿：human 三观感裁定 + 吉祥物蝴蝶结） | 61 个 id，id 集与 v2 完全一致——纯观感修订，不动锚 |
| v4 | 2026-09-14 | commit `4587bf62`（对齐 requirements v1.51） | 60 个 id；撤 6 个（`canvas-p4` / `p4-notes` / `pose-sleep` / `shot-p4` / `state-sleepy` / `swap-demo`，P4 页撤除与睡眠姿态砍除）、增 5 个（`char-notes` / `char-window` / `overlay-rules` / `p3-delete-confirm` / `shot-char`，查字窗上线） |
| v5 | 2026-09-22 → 2026-10-12 | commits `ec0804bd`…`101629e4`（ui-v5 系列 + `261011-p1-home-v5-rework`；要点四项自 `31d364d5`（2026-09-23）起） | 60 个 id，id 集与 v4 完全一致——纯版式重构（四态骨架归一 / 聆听态重排 / 提醒态右区收敛 / 首页明天组时间打头） |
| v6 | 2026-10-17 | commit `e6253317`，批 `261016-p3-story-body-sd-cache`（requirements v1.64） | 61 个 id（+1 = `p3-body-unavailable`，WF-F2-06 正文取不回提示覆盖层）。**本版曾出事故**：增量件（3 个 id）先占用 v6 命名 → 全仓 12 条 story 锚假红（涉 10 条 story），重建为完整母版后复绿——即 ARQ-261017 的立案现场 |
| v7 | 2026-10-19（2026-10-21 后补账） | commits `356a9e25` + `05cf4910`，批 `261019-p3-record-dialogue-guide`（requirements v1.65） | 64 个 id（+3 = `p3-recording-guide` / `p3-recording-guide-spec` / `state-organizing`），且为 v6 的**严格超集**（缺失 0）。按本批 intake R1 裁定由主线程「合并完整 v7 母版（v6 基底 + 三帧）」；15 画布全帧出图、三帧入库 `docs/ui/V7/` |
| v8 | 2026-10-28 | 批 `261028-font-typography-scaleup`（requirements updating v1.69，档 2 触 UI） | 64 个 id，与 v7 集合**完全一致**（缺失 0、新增 0）——纯版式/标记形态修订（P1/P2/P3 列表/P3 阅读四页：正文 16px、阅读标题 20px 居中省略、课表 pitch 19 + 带 78/78/69、※ 前缀全设备退役改课名下划线、列表标题超宽省略）；brief v6→v7 同批。变更帧 4 张入 `docs/ui/V8/`（canvas-p1 / canvas-p2 / canvas-p3 / p3-reader，规格单 = brief §8）；**产物标「随批 261028-font-typography-scaleup 在途」**，frozen 判据 = requirements §9 版本行在位 + brief 版本行对齐注记（已在位） |
| v9 | 2026-11-03 | 批 `261103-star-points-story-cover`（requirements updating v1.71，档 2 触 UI） | 78 个 id（v8 的 64 个全在、缺失 0；+14 = 画布锚 `story-cover` / `points-home` / `points-home-empty` / `points-home-nodata` / `points-home-negative` / `points-ledger` / `points-delete-confirm` + 结构锚 `shot-p3-points` / `points-home-notes` / `points-home-spec` / `shot-p3-ledger` / `points-ledger-notes` / `shot-p4-cover` / `story-cover-notes`）。**主页面 3→4**：新增 P3 星星积分本（主页 + 空账/数据未达/负余额三态 + 详细账本 + 账本删除确认同族帧）与 P4 故事封皮；原 P3 星星记录本下沉为 P4 封盘子页，**画布锚 id `canvas-p3` 保留不改**；`canvas-p1` / `canvas-p3` / `p3-reader` 仅页点 3→4（第 1 / 第 4 / 第 4 实心），`canvas-p2` 页点第 2 实心**并顺带对齐 requirements v1.70 真机轮**（槽位带 78/78/69→88/88/49、晚上容量 3→2 门；v8 未随该轮出图）；键帽条组件预览与规格表同步 4 点，全局手势表按 requirements §2.3 新矩阵重写；新增奖励场景标符号 `sc-choco` / `sc-movie` / `sc-castle` / `sc-gift` / `sc-flag`（资产登记供 K4）。brief v7→v8 同批。变更帧 11 张入 `docs/ui/V9/`（规格单 = brief §8）；**产物标「随批 261103-star-points-story-cover 在途」**，frozen 判据 = requirements §9 版本行在位 + brief 版本行对齐注记。**2026-11-03 终拍打回修订（同版号内，human 看图五条）**：详细账本与账本删除确认的兑换分录改单行（分值＋变动前→变动后余额＋原因＋日期）、积分主页四帧小伴位置标改用母版头部资产 `pt-cat` 并去「小伴」「游乐场」文字标签、游乐场城堡重绘、数据未达话术换「积分数据还在路上，等一等再来看」、故事封皮小伴探头让出书封（留缝不压叠）与「打开」键帽盒补全——受影响 7 帧重出 `docs/ui/V9/`（points-home / points-home-empty / points-home-nodata / points-home-negative / points-ledger / points-delete-confirm / story-cover）。**2026-11-03 晚二轮看图打回（同版号内，human 四条视觉/字号）**：canvas-p2 周标题行与行首槽位标签 11/12px→16px（对齐 frozen v1.70 真机同一 body 字体）、游乐场场景标城堡→摩天轮（符号 `sc-castle`→`sc-wheel`，points-home / points-home-empty / points-home-negative 三帧进度线同步）、进度线 `pt-cat` 统一悬于横线上方（底边 y30＝基线 y42 上方 12px，不压线/起点圈/刻度圈）、points-ledger 全帧 16px＋points-delete-confirm 待删行 16px（列宽/行高重排防溢出）——受影响 6 帧重出 `docs/ui/V9/`（canvas-p2 / points-home / points-home-empty / points-home-negative / points-ledger / points-delete-confirm）；data-od-id 集不变（78 个，无增删改）。**2026-11-03 晚三轮看图打回（同版号内，human 两条纯视觉）**：进度线游乐场摩天轮场景标 30×30 贴基线 → **42×42 悬于基线上方 12px**（与 pt-cat 同一悬空口径），进度线视窗 368×76 → 368×84、整块 top 44 → 38、基线 y42 → y54、刻度标签 y68 → y80（不压页名、不侵入「最近得到」）；故事封皮书封中央裸 ★ → **大金币式星徽**（新增符号 ic-coin，52px，同族徽章形态；右上 ★×5 计数不动）——受影响 4 帧重出 `docs/ui/V9/`（points-home / points-home-empty / points-home-negative / story-cover）；data-od-id 集不变（78 个） |

## 现行指针的依据记录（一次性，append-only）

**2026-10-27 · current 初设 = v7（本声明面设立批的判定依据）**：① v7 含 v6 全部 61 个 id 且 +3，严格超集、缺失 0，是最新且形质兼备的完整母版；② 最近两批消费实证均指向 v7——`261019-p3-record-dialogue-guide` 的 fixer brief 以 v7 三帧为改法权威、出图脚本（`tools/asset-pipeline/shoot_design.mjs`）按该批 intake 记录自 v7 HTML 全帧出图 15 画布、`261020-p3-rec-frame-visual` 真机观感终判后回同步 v7；③ 机检基线（改前按最高版号选稿）输出的 `design` 亦为 v7，声明化后输出不变；④ `UIUX-DESIGN-BRIEF.md` 头部「完整母版仍为 v6」的说法被严格超集关系证伪，属陈旧声明，不再作判据。

**2026-11-03 · current 翻 v9 的依据**：v9 为 v8 的严格超集（v8 全部 64 个 id 在位、缺失 0），新增 14 个本批锚；requirements 在途批 `261103-star-points-story-cover` 的 S-F2.6 / S-F6.1 / S-F6.2 三条锚行（`story-cover` / `points-home` / `points-ledger`）在 v9 首次落位。**本批母版分块合成落盘**：初版由三块按序 part 文件（`kidpal-ui-design-v9.part1.html` = 头与样式 + 修订记录；`part2` = 符号库/文档壳/P1/P2；`part3` = 本批新增页与其余各节）拼接为 `kidpal-ui-design-v9.html` 后删除 part 文件；**2026-11-03 终拍打回修订的母版重建**因单次写入体积上限改按 10 块 part 文件（`kidpal-ui-design-v9.part1.html` … `part10.html`）按序拼接覆盖同一 `kidpal-ui-design-v9.html`、拼接完成即删除 part 文件；拼接产物仍为同一 v9 完整母版（78 个 `data-od-id` 与父版一致）。**2026-11-03 晚二轮打回的母版重建**同因体积上限按 6 块 part 文件（`kidpal-ui-design-v9.part1.html` … `part6.html`）按序拼接覆盖同一 `kidpal-ui-design-v9.html`、拼接完成即删除 part 文件；拼接产物仍为同一 v9 完整母版（78 个 `data-od-id` 与父版一致）。合成完成前本指针指向的文件保持上一形态（完整、可机检，不会静默漂移）。**2026-11-03 晚三轮打回的母版修订**改用逐条定点替换脚本（读盘上现行母版 + 精确锚点替换；锚点缺失即中止、全量文件均不落盘），避免整本重写与 part 拼接的体积风险；执行后仍为同一 v9 完整母版（78 个 `data-od-id` 与父版一致）。

## 活图夹与冻结快照（页图版本语义；2026-10-27 OQ2 丙裁成文）

**页图只有一个活图夹**：`docs/ui/` 的页图（400×300 画布 / 状态 PNG，`designRefs[]` 的布局权威）只在**单一活图夹**内更新——活图夹始终反映本文件 `current:` 所指母版的现行形态，母版升版即覆盖更新。**升版冻结义务**：母版升版时，上一版页图**冻结为快照**，快照一旦落盘不再改动；已发出的 `designRefs[]` 指向冻结快照（快照 = 布局权威，不随时间漂移），新引用（`designRefs` / 出图 / 截图）一律以本文件 `current:` 为准、不钉版本夹（机制落地后生效；落地前维持现行版本夹惯例：`docs/ui/V<N>/` 逐批落变更帧）；旧包 `designRefs` 不追溯改。**本批（261027）只成文语义、不落机制**：frozen 快照的目录形态与生成流程、活图夹的具体目录名、`docs/ui/V5/` / `V6/` / `V7/` 存量页图迁移均另立需求面动作（登记见 261027-gov-review-p5-spike-anchor-cluster 的 PROPOSAL §6 留根 4）；机制落地前图夹维持现状。
