---
changeId: 261028-font-typography-scaleup
sources:
  humanVerbatim:
    - "现在我们有 TF 卡了，字体是不是可以再加得更丰富一点？比如这几个地方：故事列表的字体是不是可以大一点？首页的字体也可以大一点。故事内容里，标题比正文可以大一点，我们先调查现状，讨论方案，先不要动手改什么东西"
    - "首页 接下来和明天里的字体 从14调到18 有什么影响，显示几行，单号能显示几个字"
    - "课表页 可以 把晚上压缩，一版晚上最多两门课外课"
    - "16px 需要格内行距 22（现状 16），为什么要22这么大的行距，原来是 14字体16的行距，显示比较紧凑，看起来也OK的"
    - "选19"
    - "故事页分析一下，list 页面16 的字号有没有问题，故事阅读页，标题20 剧中显示，正文 16"
    - "同意你的推荐，我还想一次性课程加星号，改成下划线 是不是可行"
  refs:
    - ARQ-260831-small-cjk-1bpp-readability（upstream debt：NFR-READ-1 14px 下限真机实测；本批立项按 change-intake §3a 以 consumed-by 结项）
    - 2026-10-28 真机九档字号对照复测轮（font_size_test_page 烧板看屏拍照；字号档位据此拍定）
    - NFR-READ-1（requirements.md 可读性下限口径，本批不动其下限语义）
faceMap:
  requirements: 动（档 2：跨 4 story + 触 UI）——①S-F2.3 补充规则钉死「正文排版 13.5px 起、行高 1.72」（requirements.md:479）改 16px+20px 必改；②S-F1.2「逐门列出」无容量上限（:268、:274），晚上 4→3 须补「上午 4/下午 4/晚上 3、超限截断降级不滚动」口径；③一次性课程标记 ※→下划线（human 2026-10-28 会中提出，主线程 L1 答可行，标记口径随批改写）；④NFR-READ-1 下限 14px 不改（键帽条/辅助条/查字窗仍 14px）；⑤§9 追加版本行（当前 frozen v1.68）；⑥触 UI：设计母版升 v8 + P1/P2/P3 列表/P3 阅读四页重出图（UI-DESIGN-VERSIONS.md 母版指针）
  architecture: 动——§5.5.3 字体管线三族扩两族（16px 正文 + 20px 标题）并钉运行时加载路径（architecture.md:299-301）；§5.5.2 资产预算注记（:295-297）；三处过时表述勘误（「16px 以下未实测」:121/:158/:436）；建议新增 ADR-017「字体落位 = assets 分区 .bin + cbin 运行时装载 + 编译内 14px 兜底」（ADR-016 为末号）
  contract: 不动——interface-contract.md 全文「字体/字号/px/每格/容量/font」零命中（§1.3② :65-89 只钉「同 (day,slot) 多条逐门列出」无条数上限；B1 :198-242 无关）；格容量纯属板端渲染容量，云端照旧全量下发
  implementation: 动（③ 拆包）——共享字体符号 font_puhui_14_1 生产侧 28 处跨 6 文件、测试侧 115 处（Grep 计数）；home_p1.c line_h=22/text_yoff、schedule_page KSG_ROW_PITCH 16→19 + 槽位带 74/74/77→78/78/69 + 按槽位上限 4/4/3、story_page 正文 16px + 自然行 4→3 + 标题 20px 居中省略 + 列表标题省略；字体落位 = assets 分区 .bin + cbin 运行时装载（编入固件被 ota_1 余量 302KB 否决：16px 全量约 324KB）；五档 = ③ 拆包 / workstream riskTier high / 全包 rigor=full（architect 裁，human 触点① 2026-10-28 确认）；spike 前置命中（assets .bin→cbin 装载→1bpp 渲染→页面消费 通路设备上从未接过，fonts_root 全树零消费者）
  governance: 不动——agents/gate/scope-map/settings/verify-gates 零命中（architect 核实）
facePlan:
  - 顺序1 需求面（重面，本会话内联或 headless）：/requirement-change 档 2 + 触 UI——story-author 盘上 updating 起草（S-F2.3 排版句 16px/20px + S-F1.2 容量口径 + 标记口径 ※→下划线 + §9 版本行）→ architect 可行性三件 → uiux-author 生成模式（母版 v8 + 四页出图规格单，主线程跑 shoot_design.mjs 出图）→ requirement-reviewer 四查 + 触 UI 第五查 → human 看图终拍 → 主线程翻 frozen（v1.69）+ §9 记账
  - 顺序2 架构+契约面：/arch-change——architect 起草（§5.5.3/§5.5.2 改写 + 三处勘误 + ADR-017）→ architect-reviewer 四查 → GREEN → 主线程标 FROZEN → packet-bound 落地 → landing-verifier → retro；契约面零触点预判（若实现期发现需容量上网线，按逃生门转 architect-reviewer 四分类）
  - 顺序3 实现面：本门收尾 = ③ 拆包——human 盖 FROZEN（触点②）后 portfolio-planner 拆包（P0 spike → P1 首页/P2 课表/P3 故事，launch 前置 = anchors.req + anchors.arch 回填 + P0 完成）；各包 /build-feature 跑 dev loop
anchors:
  req: "requirements.md §9 版本行 v1.69（2026-10-28 frozen，human 终拍「ok，那没问题了」）——S-F2.3 排版句 16px/20px 居中省略 + 分屏行为口径 / S-F1.2 容量 4/4/3 + 临时标记课名下划线 / S-F2.1 标题省略 / NFR-READ-1 下限不变注记 / 档 0 勘误两处随批；UI 产物 frozen 态 = 母版 v8（current: 已翻）+ brief v7 + V8 四帧（UI-ready 判据 = §9 v1.69 行 + brief 对齐注记均在位）；流程留痕 = §9 v1.69 行依据列（author 两轮 + architect 全 ✅ + reviewer 五查 R1 闭合 + 终拍）"
  arch: （空，待 /arch-change 批 changeId 回填）
  contract: "N/A——契约面零触点（architect 取证：interface-contract.md 关键字零命中）"
  imp: （空，待 workstream FROZEN + 拆包后回填 taskpackage id）
openQ:
  - id: q1-一次性课程标记改下划线
    question: 一次性课程标记 ※（U+203B，前缀占 12px）改成下划线是否可行？
    ruling: 主线程 L1 答可行并纳入本批（human 2026-10-28 会中提出）——下划线改为课名下 2px 程序化画线，※ 字形探测/兜底分支退役；临时课课名不再扣前缀宽，16px 下可见字数 1→2 字回升；牵连 = S-F1.2 标记口径改写 + 母版页图同步 + schedule_grid.acceptance ※ 断言重写，全部 rides 需求面档 2 与 P2 包，边际成本近零；★（星标课程）标记不动；最终措辞与观感由需求面起草 + human 看图终拍兜底
    status: resolved
  - id: q2-实验页与测试字体处置
    question: 九档字号实验页（font_size_test_page + font_test_10..32 约百 KB 产线编入）是否本批退役？
    ruling: human 触点① 2026-10-28 仅勾选「20px 全量 CJK」、未勾选退役项——不退役，产线编入维持现状，文件留仓备复测；workstream PROPOSAL 不列此项
    status: resolved
  - id: q3-容量超限用户提示
    question: 晚上第 4 门课静默不画是否需要用户可见提示？
    ruling: human 触点① 未勾选新增提示项——维持既有「格内溢出走截断降级」不变式，不新增交互语义；如需求面起草中认为须提示，在 story 草稿中显式提出交 requirement-reviewer 四查
    status: resolved
  - id: q4-RP-A4-残余未登记debt
    question: 「font_body 运行时接线」残余只在 taskpackage 文档（260920-m2-charwindow-card-fix plan.md:18）未登记 debt，是否补登记？
    ruling: 主线程 L0 裁不另建条目——本 spine faceMap 与 workstream PROPOSAL §2 已承载该事实，P0 spike 包即其正式清偿载体，避免双头记账
    status: resolved
  - id: q5-首页临时事件标记同批统一
    question: 首页明天区临时事件标记 ※（WF-F1-01「春游 ※临时」）与课表页 F-9 同符号异语境，是否随批统一改下划线？
    ruling: **human 2026-10-28 裁随批统一改下划线**——全设备 ※ 退役、符号语言一致；牵连 S-F1.1 线框注记 1 处 + 设计稿 v8（story-author updating 态补稿）；F-9 语义边界由「课表一次性课程」扩为「全设备临时标记」
    status: resolved
  - id: q6-NFR-READ-1历史句勘误
    question: NFR-READ-1 验证行「实验页已撤」表述已过时（2026-10-28 复测轮回仓、spine q2 裁不退役），是否随批勘误？
    ruling: 主线程 L0 裁不改——历史留痕句 append-only 保护（2026-09-15 批时为真），author 已加的当前态注记即对冲载体
    status: resolved
status: in-flight
---

