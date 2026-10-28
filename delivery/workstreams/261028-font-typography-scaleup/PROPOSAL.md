---
workstream: 261028-font-typography-scaleup
workType: [new-capability, remediation]
status: frozen
opened: 2026-10-28
upstreamRefs:
  - human 2026-10-28 会话原话（spine delivery/intake/261028-font-typography-scaleup.md sources 逐字留痕）
  - docs/debt/tech/ARQ-260831-small-cjk-1bpp-readability.md（本批 consumed-by 结项）
  - 2026-10-28 真机九档字号对照复测轮（font_size_test_page 烧板看屏）
---

# 字体体系升级：正文 16px / 标题 20px / 字体资产运行时装载

> **状态：FROZEN v1.0 · 2026-10-28**（FROZEN 凭据：human grilling 裁 OQ + architect 分流 review GREEN + human 盖冻结。作者≠判官。触点① 2026-10-28 已过：③ 拆包 + 需求档 2 触 UI + 容量口径落字确认；**触点② 2026-10-28 human 原话「proposal 我没问题，frozen 进入下一步」盖 FROZEN**。）

## 1. 要达成什么（业务目的清单）

**【硬项 · 枚举式】**

| # | 业务目的 | 可观察判据 | workType | 来源 |
|---|---|---|---|---|
| 1 | 首页「接下来/明天」卡、故事列表、阅读正文、课表网格正文类文字 14px→16px | 真机上四页正文字比现在大一号且不丢行、不破容量；行数容量按 §3 裁定表保持 | new-capability | S-F1.1 · 首页日程卡、S-F1.2 · 一周课表总览、S-F2.1 · 故事列表、S-F2.3 · 故事阅读 |
| 2 | 阅读页标题 20px 居中（单行、超宽省略、不压正文） | 真机阅读页标题明显大于正文且水平居中；超长标题尾部省略号、单行 | new-capability | human 原话「标题比正文可以大一点」+ S-F2.3（排版句待需求批落字） |
| 3 | 课表网格行距与槽位带随字号重排，格内容量 4/4/3 | pitch 19、带高 78/78/69 真机不叠行；晚上第 4 门课按截断降级不滚动、不越带 | new-capability | human 原话「选19」+ S-F1.2（容量口径待需求批落字） |
| 4 | 故事列表标题超宽省略，不压日期列 | 长标题尾部省略号，日期列完整可见 | remediation | 实测：story_page.h:39 title[96] 上限 31 字 vs 列表标题可用 280px（14px 时已可越 20 字，16px 收窄至 17 字） |
| 5 | 字体资产两枚全量 1bpp .bin（16px 正文 / 20px 标题）经 assets 分区运行时装载，缺资产降级编译内 14px 不黑屏 | 真机无 TF/无 assets 时仍以 14px 正常开机；assets 在位时四页以新字号渲染 | new-capability | 预算事实：ota_1 余量 302KB < 16px 全量约 324KB（编入固件否决）；assets 余量约 5.6MB |
| 6 | 架构表述与实现对齐（字体管线节不再「写 SPIFFS 加载、实际编入」漂移） | architecture.md §5.5.3 与实际装载路径一致；ARQ-260831-small-cjk-1bpp-readability 以本 changeId 结项 | remediation | architecture.md:299-301 vs fonts_root 全树零消费者实测 |
| 7 | 一次性课程标记 ※ 前缀改为课名下划线 | 临时课课名下有 2px 下划线、无 ※ 前缀；课名可见字数由 1 字回升至 2 字（16px） | new-capability | human 2026-10-28 会中提出（spine openQ q1） |

**【硬项 · 起草侧自检两查】**

- [x] 清单引用 story 编号无一命中「已砍清单」（S-F1.1/F1.2/F2.1/F2.3 均在册活 story）。
- [x] 引用 story 可行性三态无 🔴（字号/版式为渲染常量与资产问题，无需求不可行标记）。

## 2. 现在在哪（形态随 workType 分三子形）

### 现状与差距（新建型）

- 全设备正文统一 font_puhui_14_1（14px 1bpp 全量 CJK，编入固件）；字号体系无第二档。
- 字体产线 gen_fonts.sh（lv_font_conv）现役，fonts_root 已有 14px body / 30px zcool 静态子集 / 44px 数字三枚 .bin，但**全树零运行时消费者**（260911-render-pages 落资产、接线拆后续包，至今未接）。
- assets 分区（8MB mmap，现用 2.79MB 余 5.6MB）+ `Assets::GetAssetData`（ZZ magic）+ `LvglCBinFont`（cbin_font_create）机制在编未用于本板字体。
- host 测试以编入 font_puhui_14_1.c 跑（host_app/main/CMakeLists.txt:150）；HOST_TEST_CONVENTIONS.md:237-249 明文 host 字形永不等于真机。
- 差距 = 16px/20px 两枚字体资产不存在、运行时装载通路未打通、四页版式常量未重排、标题省略语义缺失。

### 问题与根因（整改型）

- **问题 1**：故事列表超长标题（>17 字 @16px）会压到日期列。证据：story_page.c:1295 标题 label 无宽度无长模式，title[96] 上限 31 字。根因 = label 未设宽度 + 未设 LV_LABEL_LONG_DOT。
- **问题 2**：架构 §5.5.3 写「三族 SPIFFS 加载」与实现（编入固件第三方字体）漂移。根因 = RP-A4 运行时接线拆后续包后架构句未随动。
- **问题 3**：S-F2.3 排版句钉 13.5px/1.72 与本批 16px 冲突；S-F1.2「逐门列出」无容量上限与晚上 3 门降级冲突。根因 = 需求措辞先于本批决策，须档 2 修订（非实现缺陷）。

## 3. 怎么走（方案空间 + 裁定）

### 3.1 方案空间与裁定

**【硬项 · 每条裁定带 spec 关系标注】**

| 裁定 | 内容 | spec 关系 | 说明 |
|---|---|---|---|
| F-1 | 字号档：正文/列表 16px、阅读标题 20px、键帽条/辅助条/查字窗维持 14px | `需修订` | S-F2.3 补充规则 13.5px 句（requirements.md:479）须先改；执行语义 = 各页面包硬 launch 依赖 |
| F-2 | 首页 line_h=22 不动、只调 text_yoff，容量表零损失 | `within-spec` | font_test_16 line_height=18 与 14px 同（architect 核实）；S-F1.1 未钉像素 |
| F-3 | 课表格容量 4/4/3 + 带高 78/78/69 + pitch 19 + 无标记格可见 2 字（human 核算接受） | `需修订` | S-F1.2 补充规则补容量上限与超限口径（human 触点① 2026-10-28 确认落字） |
| F-4 | 阅读正文自然行 4→3（折行溢出风险随行数下降收敛），翻页手感真机验 | `需修订` | S-F2.3 排版句随批；K1 seam 明文不锁数值 |
| F-5 | 字体落位 = assets 分区 .bin + cbin 运行时装载 + 编译内 14px 兜底；否决编入固件（ota_1 余量 302KB） | `需修订` | architecture §5.5.3 改写；建议 ADR-017 |
| F-6 | 20px 标题取全量 CJK（缺字走 16px 回退），不做静态子集 | `spec-silent` | human 触点① 2026-10-28 勾选确认；标题为动态文本，子集必缺字 |
| F-7 | host 与设备共用同一枚 .bin（host 编入 cbin_font.c 读同文件），收掉 host 字体 seam | `spec-silent` | spike 包内一并验证 |
| F-8 | 课上缓存 KSG_COURSE_CAP 84 不动，只改按槽位可画上限 4/4/3——冻结面改动压到最小 | `within-spec` | schedule_page.h:31 |
| F-9 | 一次性课程标记 ※ 前缀改课名下划线（2px 程序化画线），※ 字形探测/兜底分支退役；★ 星标不动 | `需修订` | S-F1.2 标记口径改写 + 母版页图同步 + schedule_grid 验收断言重写（human 2026-10-28 提出，spine openQ q1） |

### 3.2 human 取舍点（OQ）与裁定

| OQ | 选项 | 裁定 |
|---|---|---|
| OQ1 20px 字体形态 | (a) 全量 CJK + 16px 回退 / (b) 静态子集 | **human 2026-10-28 触点① 裁 (a) 全量**（assets 余量支持；动态标题子集必缺字） |
| OQ2 母版 v8 范围 | (a) 只重出四页 / (b) 全量重出 | 主线程采 architect 推荐位 (a) 只改四页（v6→v7「完整母版合并」先例）；随 uiux-author 出图单体现，human 看图终拍兜底 |
| OQ3 九档实验页退役 | (a) 本批退役 / (b) 维持编入 | **human 触点① 未勾选退役项 → (b) 维持编入**，文件留仓备复测 |
| OQ4 容量超限提示 | (a) 新增提示 / (b) 维持静默截断 | **human 触点① 未勾选 → (b) 维持既有截断降级**，不新增交互语义（spine openQ q3） |
| OQ5 需求面档位 | (a) 档 2 + 触 UI / (b) 档 1 不出图 | **human 2026-10-28 触点① 裁 (a) 档 2 + 触 UI**（母版 v8 + 四页出图 + 看图终拍） |

### 3.3 非目标（防 scope creep）

键帽条/辅助条/查字窗/提醒弹窗/语音轮四态字号不动；不做分区表重排；不做字体子集化裁剪；不把格容量写上网线（契约面零触点）；不做 OTA 字体下发；不退役九档实验页与测试字体（OQ3）。

### 3.4 逃生门申报

实现期发现与 spec / 契约冲突时走 `/arch-change` 判级 + human 冻结；契约面走 architect-reviewer 四分类后随批原子落，不在 Pod 内自裁。

## 4. 拆包预判（意图提示，非权威）

- **P0 `font-assets-runtime-loading`（spike，Full/high）**——gen_fonts.sh 扩产 16/20px 全量 .bin；assets 打包与烧写；开机 mount + kidpal_font 运行时句柄 + 缺资产降级；host 同源加载；探针页真机取证。**消费端观测**：真机看屏拍照 + 日志 + 缺 assets 不黑屏。→ 阻塞 P1/P2/P3。
- **P1 `p1-home-16px`（Full）**——home_p1.c text_yoff + kidpal_render_node.cc 字体句柄切换 + 两份冻结验收重基线。
- **P2 `p2-schedule-16px`（Full/high）**——槽位带/行距/按槽位上限/下划线标记/可见字数 + 冻结头留痕修订 + restore 镜像同步。
- **P3 `p3-story-typography`（Full）**——列表 16px + 标题省略；阅读 16px + 3 自然行 + 20px 居中标题 + 省略；story 断言重基线。
- **（可选）P4 真机观感轮**——human 看屏终判，可与 P2/P3 并行取证。
- **launchPreconditions**：P1/P2/P3 挂 anchor.req（v1.69 frozen）+ anchor.arch；全部挂 P0 完成。**blocking edge：P0 → P1**（kidpal_render_node.cc 文件重叠，显式串行，不交两 Pod 并行）。
- **冻结验收文件修订纪律**（收紧型留痕、注释不算证据、撤改法必红）写进各包 exitCriteria。

## 5. workstream 收口标准

真机轮：四页新字号看屏终判（含课表 pitch 19 紧凑观感与翻页手感）+ 缺 assets 降级开机验证 + 长标题/超限课截断表现；owner 包 = P4（或并入 P3 收口轮）。

## 6. 留根清单

- 工作树 dev/pod-m2 现有 3 个未提交改动（font_size_test_page 防崩修正 + custom_lcd_display 注记 + sdkconfig 噪声）——P0 开工前先单独收编，不与本列车混提。
- host 字体 seam 若 F-7 验证不彻底，残余留 P4 真机轮观测。

## 7. 评审记录

- 2026-10-28 `/triage-change` architect 分流 review：findings + sign-off 见同目录 `review.md`；裁决 = ③ 拆包、全包 full、workstream riskTier high。
- 2026-10-28 human 触点①：③ 拆包确认 + 需求档 2 触 UI 确认 + 容量口径落字确认 + F-6 勾选 + OQ3/OQ4 未勾选（维持推荐态）+ F-9 会中新增。
- 2026-10-28 human 触点② 盖 FROZEN v1.0（原话「proposal 我没问题，frozen 进入下一步」）；阅读正文自然行 4→3 的防溢出取法附注真机定夺点（短段落故事每屏 3 行显疏 vs 翻页成本，由 P3 包真机轮终判）。
