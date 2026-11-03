/*
 * Frozen acceptance tests — TaskPackage 261028-m6-k3-p3-story-typography (subsystem:
 * firmware/main/display 故事列表/阅读渲染 + mcp_server 阅读分页消费; M6 列车 P3 终端包,
 * mergeOrder 63 · rigor=full / riskTier=high · sourceType=proposal, dependsOn
 * 261028-m6-k1-font-assets-runtime-loading（P0）+ 261028-m6-k2a-p1-home-16px（P1）+
 * 261028-m6-k2b-p2-schedule-16px（P2）——三依赖均已 merged 本地 main）。
 *
 * Story: S-F2.1 · 看「星星记录本」列表（AC1 标题超宽尾部省略、不挤占日期列——日期
 * 完整可见）+ S-F2.3 · 回看记录（AC1 标题 + 正文分页; AC2 手势面零回归）+ S-F2.3
 * 补充规则排版句（2026-10-28 批改写: 标题 20px 水平居中单行、超宽尾部省略; 正文
 * 16px; 每屏自然行数随排版在实现侧定）。决策面 = PROPOSAL §3.1 F-1（故事两态消费
 * P0 字号句柄、录音/盖章等未变更面不跟随放大）/ F-4（阅读正文自然行 4→3、长自然行
 * 折行不越屏、翻页手感真机终判）/ F-6（20px 标题缺字回退 16px, 经 fallback 链——
 * 语义本体归 P0 FAS-A5, 本件只钉真实消费点守卫）+ §1-目的4（列表长标题省略不压
 * 日期列）+ §5（M6 四页并集终端真机收口, P4 并入本包）。授权链 = TaskPackage
 * 261028-m6-k3-p3-story-typography exitCriteria P3T-E1/E2/E3 + requirements
 * v1.69 S-F2.1 AC1 / S-F2.3 补充规则行 + brief v7 对齐注记（均在位）。
 * 本件与 story_page_v2.acceptance.c 的分工: 本件钉 16px/20px 新排版形态（字号度量
 * 合理域 / 标题居中省略 / 自然行 3 / 未变更页态 14px 负半 / mcp 同步结构锚）,
 * story_page_v2.acceptance.c 经收紧型 supersede（REVISION LOG revision 5）钉回归
 * 不变式（输入表矩阵 / 分屏纯逻辑 / 删除子态 / seq / restore / 空态两态）。两件
 * 断言互不弱化、互不重复钉死同一像素。列表分屏 / 计数 / 边框选中 / 空态语义不变
 * 的回归义务由既有齿继续承载（SPV2-A2 分屏纯逻辑 / SPV2-A7 counter 恒信 total /
 * SPV2-B2 tooth 6 边框 / SLP-SLA5 空态两态），本件零重复钉。
 *
 * ===========================================================================
 * 三分申报（P2 schedule_grid_16px 先例同款; 设备 TU 不进 host 构建的部分不写假
 * 行为断言）:
 *
 *   [host 可验]（本文件全部 TEST_CASE）:
 *     - 16px/20px 句柄消费的行为级证据: mount fonts_root 后经真实渲染路径
 *       （kidpal_story_page_render）量列表行标题 / 阅读正文 / 阅读标题墨高落各自
 *       字号证据域 + 14px 兜底态判别负控（严格不等式, 两态可判——retro F1 教训:
 *       钉度量合理域不只钉返回码; 窗口为 2026-11-02 host 实测标定, 见强度申报）。
 *     - 长标题省略语义（行为级）: 单行（墨高 <= 18）+ 不压日期列（分离带 x[316..319]
 *       零墨）+ 日期完整可见 + 触发填充（省略已触发）。省略号字形像素形态归
 *       design review（弱于契约处申报④）。
 *     - 标题 20px 居中 / 单行 / 不压正文（行为级）+ 20px→16px 回退消费点守卫
 *       （机制级, title()->fallback == body()）。
 *     - 自然行 3: 常量单源断言（include story_page.h 直读 KSTORY_READER_LINES_PER_
 *       SCREEN）+ open_reader/reader_slice 行为配对 + KSTORY_LIST_ROWS_PER_SCREEN=4
 *       零偏移守卫。
 *     - 未变更页态 14px 负半（F-1）: DELETE 提问 / BODY_UNAVAILABLE 大字 / STAMP
 *       文案 / RECORDING 气泡四带墨高守 14px 域。
 *     - 结构半走 FAS-MANIFEST 构建清单机制（P0/P1/P2 同款——human 2026-10-28 定向
 *       裁定运行时禁扫源码树; 键集见下 [M] 节, 接线增量逐字落 acceptance-wiring-
 *       patch.md）。
 *   [设备专有]（host 构建不覆盖, 不写假行为断言）:
 *     - 设备侧 cbin 装载等价性归 P0 机制 + FAR-E3 真机轮（P1 裁定 2 口径不变）;
 *       story_page.c 为 host/设备两侧同源 TU, 消费面行为经 host 帧真测承载。
 *     - mcp_server.cc 两个 KSTORY_READER_LINES_PER_SCREEN 消费点（:691 卡分支 /
 *       :1885 story_text 路由, 2026-11-02 Grep 复核在役）不进 host 构建——结构源锚
 *       经 FAS 键 story_mcp_lines_sync（符号引用计数 >= 2）承载, 不假 host 行为。
 *   [真机轮专属]（P3T-E4 / PROPOSAL §5, 本文件零 host 齿、不伪装）:
 *     - 四页并集目击（故事两态新字号 + 长标题/超限截断 + 阅读翻页手感 + P1/P2 复证
 *       + 缺 assets 14px 降级——后者 human 2026-10-29 已裁豁免）; 翻页手感与短段落
 *       疏密 = F-4 真机终判; human 终判后才宣布 M6 收口。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements; home = 冻结落点）:
 *
 *   [T1] 列表行 16px（home = firmware/main/display/story_page.c KSTORY_PAGE_LIST
 *        渲染分支; S-F2.1 AC1 + 目的4 + P3T-E1）:
 *          -- 行标题 label 消费 P0 统一句柄 kidpal_font_body()（16px 字面源单点,
 *             P2 [F2] 同款直接消费形态——story_page.c 为 LVGL-ful TU, 不另造间接
 *             seam; 与 P0 [K4]「P1/P2/P3 唯一消费口」纪律对齐）;
 *          -- 长标题单行尾部省略: 标题 label 定宽 + LV_LABEL_LONG_DOT（省略号形态
 *             像素归 design review, 本件钉单行 + 分离 + 填充三行为面）;
 *          -- 不压日期列: 标题墨与日期墨之间分离带（x[316..319], 日期列 x=320 起）
 *             零墨; 日期完整可见（日期带在墨、不越右缘）;
 *          -- 列表分屏（KSTORY_LIST_ROWS_PER_SCREEN=4）/ 计数 / 边框选中 / 空态
 *             语义不变（既有 SPV2 齿续钉, 本件零触碰）。
 *
 *   [T2] 阅读正文 16px（home = story_page.c KSTORY_PAGE_READER 渲染分支 body
 *        label; S-F2.3 补充规则 + P3T-E2 前半）: body label 消费
 *        kidpal_font_body()（同 [T1] 字面源单点）。
 *
 *   [T3] 自然行 4→3（home = firmware/main/display/story_page.h
 *        KSTORY_READER_LINES_PER_SCREEN; F-4 + P3T-E2/E3）:
 *          -- #define KSTORY_READER_LINES_PER_SCREEN 4 -> 3（单源常量, 本件 include
 *             story_page.h 直读钉值——机制级, mcp 两消费点经符号引用自动同步）;
 *          -- #define KSTORY_LIST_ROWS_PER_SCREEN 4 零偏移（列表分屏不随动——
 *             16px 行高 23 < 行距 42, 列表无重排需求; story_page_v2 [K8] 齿续钉）;
 *          -- 行为配对: open_reader（常量注入）7 自然行 -> 3 屏; reader_slice
 *             第 0 屏恰含 3 自然行（2 个 '\n'）; 越界屏 NULL 守卫（SPV2-A6 续钉）。
 *
 *   [T4] 长自然行折行不越屏（home = story_page.c READER body label 定宽 + WRAP;
 *        F-4 + P3T-E2 前半）: 单条 30 字自然行经折行分为多个视觉行, 墨不越
 *        x[12..396]（label 区 16..384 + 容差）且折行确已发生（垂直墨跨 >= 26）。
 *
 *   [T5] 阅读标题 20px（home = story_page.c READER 渲染分支标题 label; 目的2/
 *        S-F2.3 补充规则 + P3T-E2 后半）:
 *          -- 标题 label 消费 kidpal_font_title()（20px 字面源单点）;
 *          -- 水平居中（短标题墨心 |c-200| <= 24）、单行（墨高 <= 26）、超宽尾部
 *             省略（22 字长标题填充触发）、不压正文（标题墨 max_y <= 42 与正文墨
 *             min_y >= 43 分离——现役版式锚 y8/44 零重排假设, 见强度申报⑤）;
 *          -- 20px 缺字回退 16px 消费点守卫: mount 态 kidpal_font_title()->
 *             fallback == kidpal_font_body()（机制级; 缺字回退语义本体 = P0
 *             FAS-A5 自然缺口探针 + 盒度量等价, 本件不重复钉）。
 *
 *   [T6] 未变更页态不跟随放大（F-1 负半; plan.md 铁律「录音、整理、盖章、删除
 *        确认、正文取不回提示等未变更页态沿用既有设计」+ P3T-E1/E3 回归半）:
 *          -- DELETE 提问 / BODY_UNAVAILABLE 大字 / STAMP 文案 / RECORDING 气泡
 *             四带墨高守 14px 域 [10..13]（mount 态下测——若未变更页态被误切
 *             kidpal_font_body() 且资产在位, 墨高 14 即红; 缺 assets 态下测不可
 *             判别, 故本齿恒 mount 态执行）;
 *          -- 页态/输入/云端载荷形状零回归 = 既有齿承载（SPV2-A1/A4/A6 决策表、
 *             SLP-SLA4 restore wire 容忍、契约 §1.3③④ 形状齿; server 面
 *             crossEnd counterparts 零 producer diff 预期——launch-card 在案）。
 *
 *   [T7] mcp 同步 + 结构面（FAS-MANIFEST 扩键; P3T-E3）:
 *          -- story_mcp_lines_sync: mcp_server.cc 内 "KSTORY_READER_LINES_PER_
 *             SCREEN" 符号引用 >= 2（两个消费点恒符号引用、不得回退字面硬编码——
 *             常量改值即两点自动同步; 本键 must-stay-green, 符号引用丢失即红）;
 *          -- story_c_font_body_seam / story_c_font_title_seam: story_page.c 消费
 *             P0 [K4] 两句柄字面（[T1]/[T2]/[T5] 机制锚）;
 *          -- story_h_reader_lines3 / story_h_reader_lines_not4: story_page.h 新值
 *             3 在世 + 旧值 4 退役（负向键, P2 sched_c_pitch_not16 同款）;
 *          -- story_c_dot_ge3: story_page.c "LV_LABEL_LONG_DOT" >= 3（现役 2 处
 *             录音/整理回显钳制 + 新增列表标题/阅读标题省略——省略机制 canary,
 *             与 [T1]/[T5] 行为面互证; 计数键非精确落点钉, 落点由行为带承载）;
 *          -- story_c_puhui14_kept: story_page.c "font_puhui_14_1" 在世（14px 兜底
 *             面未被全局替换——F-1 未变更面机制半, 与 SPV2-B2 tooth 1 重基线锚、
 *             STY-A6 行为半三面互证）;
 *          -- story_list_rows4 复用键零偏移 + fas_manifest_version / 负向探针
 *             / 产物可见（清单面自检）。
 *
 *   [M] FAS-MANIFEST 扩键（结构面断言采集通道; 在既有 FAS 清单块上扩键, P0/P1/P2
 *       同款——机制全文见 P0 font_assets_runtime.acceptance.c 头注 FAS-MANIFEST 节;
 *       接线增量逐字落 acceptance-wiring-patch.md）:
 *         host_srcs_story_typography   本验收件编入 HOST_ACCEPTANCE_SRCS
 *         story_c_font_body_seam       story_page.c 含 "kidpal_font_body"
 *         story_c_font_title_seam      story_page.c 含 "kidpal_font_title"
 *         story_c_puhui14_kept         story_page.c 含 "font_puhui_14_1"
 *         story_c_dot_ge3              story_page.c "LV_LABEL_LONG_DOT" 计数 >= 3
 *         story_h_reader_lines3        story_page.h 含
 *                                      "#define KSTORY_READER_LINES_PER_SCREEN 3"
 *         story_h_reader_lines_not4    story_page.h **不**含
 *                                      "KSTORY_READER_LINES_PER_SCREEN 4"（单/双
 *                                      空格两变体, 负向——旧值退役）
 *         story_mcp_lines_sync         mcp_server.cc "KSTORY_READER_LINES_PER_
 *                                      SCREEN" 计数 >= 2（两个消费点符号引用）
 *       复用既有键（零 CMake 增量, 本件只读）: story_list_rows4（列表分屏 4 零
 *       偏移）、fas_manifest_version。
 *       同批换锚（非本件新增, 落 acceptance-wiring-patch.md）: 旧键
 *       story_reader_lines4（needle "#define KSTORY_READER_LINES_PER_SCREEN 4"）
 *       退役——P0 FAS-A8 tooth 3 经收紧型 supersede 换锚为 story_reader_lines3
 *       （font_assets_runtime.acceptance.c REVISION LOG 2026-11-02 条; 旧 canary
 *       语义「P0 不触 story 常量」由 story_list_rows4 + 本件新键接力）。
 *
 * ---------------------------------------------------------------------------
 * 跨端申报（acceptance 纪律: 跨端 wire 形状断言对照对端实际行为钉, 不按本侧想象）:
 *   对端 = server/gateway/story.py list_stories + server/gateway/front/front_app.py
 *   _relay_display_blocks_to_device（crossEnd counterparts, launch-card 钉零 producer
 *   diff）。2026-11-02 实读锚定: 本包只消费 story_list/story_text 载荷、不改任何
 *   wire 形状（seq/restore/分页均在消费面）; 自然行 4→3 只改板端排版常量, ack 的
 *   "screens" 数值随载荷与常量自然变化（形状不变、数值非钉面）。因此本件零新增
 *   wire 形状断言, 形状回归由既有齿（SPV2-A7 16 窗口 / SLP-SLA4 restore 容忍 /
 *   SPV2-A8 seq）+ server 面零 diff 承载。
 *
 * ---------------------------------------------------------------------------
 * 契约条目 ↔ 断言的强度申报（acceptance-evidence.md 体例）:
 *
 *   STY-A1（S-F2.1-AC1 / 目的4 / P3T-E1 列表 16px + 长标题省略不压日期）
 *     -> TEST 1 承载: 行为级（标题墨高 16px 证据域 [13..16] + 14px 兜底判别负控
 *        严格不等式 + 分离带零墨 + 日期在墨 + 单行 + 填充）+ 机制级（FAS 键
 *        story_c_font_body_seam / story_c_dot_ge3）。弱于契约处: ①省略号字形像素
 *        形态不钉（LONG_DOT 机制经键 + 单行/填充行为面互证, 像素归 design review
 *        + 真机轮）; ②「日期完整可见」钉日期带在墨 + 不越右缘, 不逐字钉 5 字形全
 *        可见（版式归真机轮）。
 *   STY-A2（S-F2.3 补充规则 / P3T-E2 阅读正文 16px）
 *     -> TEST 2 承载: 行为级（正文墨高 [13..16] + 判别负控）+ 机制级（同 [M]）。
 *        弱于契约处: 墨高窗为证据域非逐字形像素终判（同 SG16-A1 口径）。
 *   STY-A3（F-4 / P3T-E2/E3 自然行 3）
 *     -> TEST 3 承载: 机制级（include 直读常量 == 3 + rows4 == 4 零偏移）+ 行为级
 *        （open_reader 7 行 3 屏 / slice 恰 3 自然行）。弱于契约处: 每屏自然行数
 *        requirements 明示「实现侧定、不钉死于本文件」——本处钉 3 的授权 =
 *        PROPOSAL F-4 决策 + taskpackage P3T-E2（需求面留了实现侧自由度, 列车
 *        决策面收口为 3; 若真机轮 F-4 终判改值, 走收紧型 supersede 留痕）。
 *   STY-A4（F-4 / P3T-E2 长自然行折行不越屏）
 *     -> TEST 4 承载: 行为级（折行发生 + 墨不越 x[12..396]）。弱于契约处: 「不越
 *        屏」钉到 label 区 + 4px 容差, 不逐像素钉右缘; 底缘溢出场景（3 自然行全
 *        折行 × 23px 行高 < 版面可用高）经常量 3 + 行高几何承载, 不另设极端夹具。
 *   STY-A5（目的2 / S-F2.3 补充规则 / P3T-E2 阅读标题 20px 居中单行省略 + 回退）
 *     -> TEST 5 承载: 行为级（墨高 [16..20] / 居中 ±24 / 单行 / 长标题填充 /
 *        标题-正文分离带）+ 机制级（fallback 链消费点守卫 + story_c_font_title_
 *        seam）。弱于契约处: ①居中容差 ±24 非逐像素（版式归 design review）; ②
 *        「约 19 个汉字可见」需求句不逐字钉可见字数, 以填充触发 + 单行承载; ③缺
 *        字回退语义本体（自然缺口码点盒度量等价）归 P0 FAS-A5, 本件只钉 fallback
 *        链在消费点在世——真实缺字渲染帧归真机轮。
 *   STY-A6（F-1 负半 / P3T-E1/E3 回归半 未变更页态不跟随放大）
 *     -> TEST 6 承载: 行为级（四带 14px 域 [10..13], mount 态判别成立）。弱于契约
 *        处: 四带为未变更页态代表样本（LIST 计数/页脚/分屏指示、空态话术等 chrome
 *        字号不钉——14px 域归其既有齿 + design review; 四带覆盖 plan.md 铁律点名的
 *        五页态中 host 可达的全部四个; 整理态 ORGANIZING 与录音态共用 ksy_bubble
 *        helper 路径, 经录音带间接承载）。
 *   STY-A7（P3T-E3 mcp 同步 + 结构面 + 接线守卫）
 *     -> TEST 7 承载: 构建清单档（[M] 八新键 + 一复用键 + 自检）。行为本体由
 *        TEST 1-6 兑现, 两档并列互证。
 *
 *   fixture 自含性: 全部行为齿 fixture = 手构 kidpal_story_list_t / kidpal_story_
 *   text_t + fonts_root 真资产（P0 交付面, fixture 依赖申报: font_body_16.bin /
 *   font_title_20.bin 在库 + host_infra_seam.c 强符号在世）; 无时间真值/环境前提。
 *   host/真机近似假设: host ARGB8888 软渲染墨判（非白像素）是 1bpp 真机墨的过集
 *   （SPV2 头注同口径; 真机半挂 P3T-E4）。
 *   盲区申报: ① mcp_server.cc 消费点行为半 host 不可达（设备 TU）——story_mcp_
 *     lines_sync 结构锚承载「符号引用在世」, 常量改值即两点自动同步, 无第二真相
 *     面可漂移; 页态/载荷形状回归由既有齿 + server 面承载; ② 翻页手感 / 疏密 /
 *     真机观感 = P3T-E4 真机轮（结构性豁免, 不计入 runner 通过面）; ③ 省略号像素
 *     形态（上①）; ④ device build 闸（xiaozhi.bin 过构建）= builder 义务, 非本
 *     文件齿。
 *
 * ---------------------------------------------------------------------------
 * Coverage map（exitCriteria / sourceRefs -> test）:
 *   coverage: P3T-E1（列表 16px + 长标题省略不压日期 + 分屏/计数/边框/空态不变）-> STY-A1（TEST 1）+ STY-A7 机制键 + 既有 SPV2-A2/A7/B2 tooth6/SLP-SLA5 续钉
 *   coverage: S-F2.1-AC1（标题超宽尾部省略、不挤占日期列——日期完整可见）-> STY-A1
 *   coverage: P3T-E2 前半（阅读正文 16px + 自然行 4→3 + 长自然行折行不越屏）-> STY-A2（TEST 2）+ STY-A3（TEST 3）+ STY-A4（TEST 4）
 *   coverage: P3T-E2 后半（标题 20px 居中单行省略不压正文 + 20px 缺字回退 16px）-> STY-A5（TEST 5）
 *   coverage: S-F2.3-AC1（标题 + 正文分页）/ S-F2.3-补充规则（16px/20px 排版句）-> STY-A2/A3/A4/A5
 *   coverage: P3T-E3（mcp 两消费点随 4→3 同步 + 页态/输入/云端形状不变回归）-> STY-A7（TEST 7 story_mcp_lines_sync）+ STY-A6（TEST 6）+ 既有 SPV2 决策表/SLP-SLA4 续钉
 *   coverage: F-1（故事两态消费 P0 句柄 + 未变更面不跟随放大）-> STY-A1/A2/A5 + STY-A6 + story_c_font_body_seam/title_seam/puhui14_kept
 *   coverage: F-4（自然行 4→3 + 折行不越屏; 翻页手感真机终判）-> STY-A3/A4; 真机半挂 P3T-E4（零 host 齿不伪装）
 *   coverage: F-6（20px 缺字回退 16px, 真实消费点）-> STY-A5 fallback 守卫; 语义本体 = P0 FAS-A5（承载归属申报）
 *   coverage: PROPOSAL §5 / P3T-E4（M6 四页并集终端真机轮 + human 终判）-> 零 host 齿（头注三分申报挂账, 不阻塞 host 半）
 *   coverage: 目的4 / 目的2 -> STY-A1 / STY-A5
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（HOST_TEST_CONVENTIONS.md [C4.2]; 接线增量逐字落
 * acceptance-wiring-patch.md）:
 *   1) firmware/main/display/story_page.c:
 *      a. #include "kidpal_font.h"; LIST 行标题 label / READER body label 切
 *         kidpal_font_body(); READER 标题 label 切 kidpal_font_title()（[T1]/
 *         [T2]/[T5]; 不新增 NULL 回落——P0 [K2]/[K3] 兜底链承载; 未变更页态
 *         （DELETE/BODY_UNAVAILABLE/STAMP/RECORDING/ORGANIZING + 计数/页脚/分屏
 *         指示/空态话术）字号零触碰（F-1 负半, STY-A6 四带守卫）。
 *      b. LIST 长标题省略: 标题 label 定宽（右缘让位日期列 x=320）+
 *         LV_LABEL_LONG_DOT（[T1]）。
 *      c. READER 标题: 20px 句柄 + 水平居中 + 定宽单行省略（[T5]）; body 定宽 +
 *         WRAP 保持（[T4]）。
 *   2) firmware/main/display/story_page.h: #define KSTORY_READER_LINES_PER_SCREEN
 *      4 -> 3（[T3]）+ 头注排版注记同步（13.5px/1.72 旧注记随 16px/20px 更新）;
 *      KSTORY_LIST_ROWS_PER_SCREEN 4 零触碰。
 *   3) firmware/main/mcp_server.cc: 两消费点已恒符号引用, 预期零改动（若注释同步
 *      亦不触符号面; story_mcp_lines_sync 守卫符号引用 >= 2）。
 *   4) host 接线: HOST_ACCEPTANCE_SRCS 注册 "../../story_typography.acceptance.c"
 *      （schedule_grid_16px 条目之后、smoke 之前）+ FAS 块扩 [M] 八键 + 旧
 *      story_reader_lines4 键退役换新 + CMAKE_CONFIGURE_DEPENDS 增 story_page.c /
 *      mcp_server.cc——逐字增量见 acceptance-wiring-patch.md。
 *   5) device build 闸: xiaozhi.bin 构建过（story_page.c / mcp_server.cc 均已在
 *      设备构建清单, 本包零新增 TU）。
 *   6) mutation 自证（mutationRequired=true; 取证由 builder/fixer 实测、planner
 *      代落 taskpackage mutationEvidence, 本文件不落取证）:
 *      M-STY1 摘列表标题定宽/DOT 或回 14px 字面 -> TEST 1 分离带/墨高红;
 *      M-STY2 READER body 回 14px -> TEST 2 墨高红;
 *      M-STY3 常量回 4 -> TEST 3 常量/screens/slice 三 tooth 红;
 *      M-STY4 摘 body 定宽/WRAP -> TEST 4 越界红;
 *      M-STY5 标题回 14px / 去居中 / 去 DOT -> TEST 5 墨高/居中/填充红;
 *      M-STY6 未变更页态误切 kidpal_font_body -> TEST 6 对应带红;
 *      M-STY7 [M] 键对应字面回退 -> TEST 7 对应键红。
 *
 * ---------------------------------------------------------------------------
 * Run protocol（HOST_TEST_CONVENTIONS.md [C4.4]）:
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf
 *      （root 自手持 cwd 定位, 仓根或 host_app 目录执行皆可）
 *   RED baseline 判读见文件尾。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <lvgl.h>

/* P0 [K1]-[K4] 统一句柄 seam（强定义 = display/kidpal_font.cc, host runner 在世）。 */
#include "../display/kidpal_font.h"

/* [T3] 常量单源直读（story_page.h = KSTORY_LIST_ROWS_PER_SCREEN /
 * KSTORY_READER_LINES_PER_SCREEN 单源; 与 SPV2 的镜像形态不同——本件钉的是头内
 * 常量值本身, include 直读即把断言编译绑定到单源, 头改值断言随动）。 */
#include "../display/story_page.h"

/* 编译内 14px 兜底（font_puhui_14_1.c 已编入 host runner, 强定义在世）。 */
extern const lv_font_t font_puhui_14_1;

/* host 渲染缝（host_infra_seam.c 提供, 恒链入 runner）。只探针。 */
int host_infra_render_frame(void);
int32_t host_infra_read_pixel(int x, int y);

/* -------------------------------------------------------------------------- *
 * 常量与标定窗（2026-11-02 host 实测标定, 临时探针程序实测后删除; 与 P2
 * schedule_grid_16px 同一 16px 资产同源同值——body16 line_height=23、CJK 墨高
 * 14-15（课 box 15x14 / 口 12x13）; title20 line_height=28、CJK 墨高 17-19（我
 * box 19x19 / 星 18x17）; 14px puhui 兜底域承 P2 实测 [10..13]。若 builder 实测
 * 越窗且字形度量正常, 按 tighten-only 窗口以证据重基线, 不静默放宽。）
 * ------------------------------------------------------------------------ */
#define KST_FB_W 400
#define KST_FB_H 300
#define KST_WHITE 0xFFFFFFul
#define KST_NOT_RENDERED ((int32_t)-1)

/* 16px 证据域（正文/列表行标题; 实测 14-15）。 */
#define KST_INK16_MIN 13
#define KST_INK16_MAX 16
/* 14px 兜底域（实测 11-12）。 */
#define KST_INK14_MIN 10
#define KST_INK14_MAX 13
/* 20px 证据域（阅读标题; 实测 17-19）。 */
#define KST_INK20_MIN 16
#define KST_INK20_MAX 20

/* 版面锚（现役版式零重排假设——KSTORY_LIST_ROW_Y0=44/ROW_STEP=42、READER body
 * y=44、标题 y=8; builder 调整行几何 = fixture 对齐协商走 planner, 不静默放宽）。 */
#define KST_ROW_BAND_Y0   42   /* 列表行 0 内容带（避开选中框 3px 边框行 38..40） */
#define KST_ROW_BAND_Y1   70   /* （避开下边框行 71..73） */
#define KST_TITLE_ZONE_X0 36   /* 列表标题带左界（避开 x16 星星图标） */
#define KST_TITLE_ZONE_X1 315  /* 右界（日期列 x=320 左侧留分离带） */
#define KST_GAP_X0        316  /* 标题/日期分离带（零墨义务） */
#define KST_GAP_X1        319
#define KST_DATE_ZONE_X0  318
#define KST_DATE_ZONE_X1  392
#define KST_TITLE_FILL_MIN 240 /* 长标题省略触发填充下界（可用宽 ~276px） */
#define KST_READER_TITLE_BAND_Y0 2
#define KST_READER_TITLE_BAND_Y1 41
#define KST_READER_BODY_BAND_Y0  44
#define KST_READER_BODY_BAND_Y1  200
#define KST_BODY_X0 12
#define KST_BODY_X1 396
#define KST_CENTER_TARGET 200  /* 400px 版面水平中心 */
#define KST_CENTER_TOL    24

/* -------------------------------------------------------------------------- *
 * Repo-root 定位（只服务 fonts_root 路径与 FAS 清单产物解析; 运行时禁扫源码树
 * ——human 2026-10-28 裁定, P0/P1/P2 先例同款）。ELF fopen 相对路径自手持 cwd
 * 定位（root 注入纪律 = 不依赖调用 cwd 的绝对路径假设）。
 * ------------------------------------------------------------------------ */
#define KST_MAX_PATH 1024
#define KST_MAX_READ (8u * 1024u * 1024u)
#define KST_FONTS_ROOT "firmware/main/assets/fonts_root"
#define KST_FAS_MANIFEST "firmware/main/test/host_app/build/fas_structure_manifest.txt"

static char kst_root[KST_MAX_PATH];
static int kst_root_ready;

static int kst_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int kst_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int kst_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KST_MAX_PATH];
    size_t i;

    if (kst_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kst_is_regular(probe)) {
            snprintf(kst_root, sizeof(kst_root), "%s", cands[i]);
            kst_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int kst_rel(char *out, size_t cap, const char *rel)
{
    if (!kst_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", kst_root, rel);
    return 1;
}

static uint8_t *kst_read_bin(const char *rel, size_t *out_len)
{
    char p[KST_MAX_PATH];
    FILE *f;
    long sz;
    uint8_t *buf;

    if (out_len != NULL) {
        *out_len = 0;
    }
    if (!kst_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KST_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (uint8_t *)malloc((size_t)sz);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

static char *kst_read_text(const char *rel)
{
    size_t len;
    uint8_t *raw = kst_read_bin(rel, &len);
    char *buf;

    if (raw == NULL) {
        return NULL;
    }
    buf = (char *)malloc((size_t)len + 1);
    assert(buf != NULL);
    memcpy(buf, raw, len);
    buf[len] = '\0';
    free(raw);
    return buf;
}

#define KST_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kst_locate_root(), \
    "KST RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

static char kst_fm_buf[1024];
static const char *kst_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *kst_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(kst_fm_buf, sizeof(kst_fm_buf), fmt, ap);
    va_end(ap);
    return kst_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * [K1] host 测试 reader: fonts_root 直读（F-7 同源; P0/P1/P2 先例同款）。
 * ------------------------------------------------------------------------ */
static int kst_host_reader(const char *name, const uint8_t **data, size_t *size)
{
    char rel[KST_MAX_PATH];
    uint8_t *buf;
    size_t len;

    snprintf(rel, sizeof(rel), "%s/%s", KST_FONTS_ROOT, name);
    buf = kst_read_bin(rel, &len);
    if (buf == NULL) {
        return -1;
    }
    *data = buf;
    *size = len;
    return 0;
}

/* 全缺 reader（复位 14px 兜底态——mount 态不得泄漏进后续执行的其它冻结件, P2
 * tooth 4 纪律; mount 后 body()/title() 恒等 font_puhui_14_1, P0 FAS-A4 合法
 * 完成态）。 */
static int kst_missing_reader(const char *name, const uint8_t **data, size_t *size)
{
    (void)name; (void)data; (void)size;
    return -1;
}

/* -------------------------------------------------------------------------- *
 * FAS-MANIFEST 消费面（sgx/fas 先例同款: 行首锚定 key=, 未知键返回 -1 = infra 红）。
 * ------------------------------------------------------------------------ */
static int kst_mfact(const char *key)
{
    static char cache[8 * 1024];
    static char val[32];
    static int loaded;
    const char *line;
    size_t klen = strlen(key);

    if (!loaded) {
        char *txt = kst_read_text(KST_FAS_MANIFEST);
        loaded = 1;
        if (txt != NULL) {
            if (strlen(txt) < sizeof(cache)) {
                memcpy(cache, txt, strlen(txt) + 1);
            }
            free(txt);
        }
    }
    for (line = cache; (line = strstr(line, key)) != NULL; line += 1) {
        const char *v;
        const char *e;
        size_t n;
        if ((line != cache && line[-1] != '\n') || line[klen] != '=') {
            continue;
        }
        v = line + klen + 1;
        e = strchr(v, '\n');
        n = (e != NULL) ? (size_t)(e - v) : strlen(v);
        if (n == 0 || n >= sizeof(val)) {
            return -1;
        }
        memcpy(val, v, n);
        val[n] = '\0';
        return atoi(val);
    }
    return -1;
}

static void kst_require_fact(int fact, const char *key, const char *what)
{
    if (fact == -1) {
        TEST_ASSERT_TRUE_MESSAGE(0, kst_fm(
            "KST RED(infra): 结构清单键 %s 缺失或清单未生成——重跑 host build 让 "
            "configure 期重采集（键集 = acceptance-wiring-patch.md; 机制见 P0 "
            "font_assets_runtime.acceptance.c 头注 FAS-MANIFEST 节）", key));
        return;
    }
    TEST_ASSERT_TRUE_MESSAGE(fact == 1, kst_fm(
        "KST RED: 结构事实 %s 不成立——%s（事实由 host CMakeLists configure 期采集, "
        "机制见冻结件头注 [M] 节）", key, what));
}

/* -------------------------------------------------------------------------- *
 * 帧探针 helpers（sgx_/ksg_ 先例同款）。探针失败（未渲染）大声红, 绝不读成零墨。
 * ------------------------------------------------------------------------ */
static int32_t kst_f_a[KST_FB_W * KST_FB_H];
static int32_t kst_f_b[KST_FB_W * KST_FB_H];

static int kst_snap(int32_t *buf)
{
    int x, y;

    for (y = 0; y < KST_FB_H; y++) {
        for (x = 0; x < KST_FB_W; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == KST_NOT_RENDERED) {
                return -1;
            }
            buf[y * KST_FB_W + x] = px;
        }
    }
    return 0;
}

/* 渲染 + 直刷 + 收帧（SPV2-B1 R5 时序先例: render_frame 起手一次, render 后
 * lv_refr_now 直刷再探针——render_frame 在 render 后调会整帧盖掉被测内容）。 */
static void kst_render_snap(const kidpal_story_view_t *v,
                            const kidpal_story_list_t *list,
                            const kidpal_story_text_t *story,
                            int32_t *buf)
{
    int rc = kidpal_story_page_render(v, list, story);

    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        kst_fm("KST RED: kidpal_story_page_render rc=%d——渲染面显式降级（fixture "
               "前提被破坏, 非设计态红）", rc));
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE_MESSAGE(kst_snap(buf) == 0,
        "KST RED(infra): frame not rendered — 探针缝断裂");
}

/* 带内暗墨垂直跨度与暗像素计数（sgx_ink_span 同款, 非白即墨）。 */
static void kst_ink_span(const int32_t *f, int x0, int y0, int x1, int y1,
                         int *out_min_y, int *out_max_y, int *out_dark)
{
    int min_y = -1, max_y = -1, dark = 0;
    int x, y;

    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            int32_t p = f[y * KST_FB_W + x];
            if (p >= 0 && p != (int32_t)KST_WHITE) {
                dark++;
                if (min_y < 0) {
                    min_y = y;
                }
                max_y = y;
            }
        }
    }
    *out_min_y = min_y;
    *out_max_y = max_y;
    *out_dark = dark;
}

/* 带内暗墨水平跨度（min_x/max_x + 暗像素数）。 */
static void kst_ink_cols(const int32_t *f, int x0, int y0, int x1, int y1,
                         int *out_min_x, int *out_max_x, int *out_dark)
{
    int min_x = -1, max_x = -1, dark = 0;
    int x, y;

    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            int32_t p = f[y * KST_FB_W + x];
            if (p >= 0 && p != (int32_t)KST_WHITE) {
                dark++;
                if (min_x < 0 || x < min_x) {
                    min_x = x;
                }
                if (x > max_x) {
                    max_x = x;
                }
            }
        }
    }
    *out_min_x = min_x;
    *out_max_x = max_x;
    *out_dark = dark;
}

/* 带内是否有墨（分离带零墨义务用）。 */
static int kst_ink_count(const int32_t *f, int x0, int y0, int x1, int y1)
{
    int dark = 0;
    int x, y;

    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            int32_t p = f[y * KST_FB_W + x];
            if (p >= 0 && p != (int32_t)KST_WHITE) {
                dark++;
            }
        }
    }
    return dark;
}

/* -------------------------------------------------------------------------- *
 * Fixture helpers。字形池 = 既有冻结件已核验字形（SPV2-B1 真帧探针集: 小兔子找
 * 朋友 / 勇敢的小明 / 我的大冒险——puhui14 兜底面 ∩ 16px/20px 全量面三态在场,
 * 判别负控不因字形缺场假红）。
 * ------------------------------------------------------------------------ */
static void kst_make_list1(kidpal_story_list_t *l, const char *title)
{
    memset(l, 0, sizeof(*l));
    l->total = 1;
    l->count = 1;
    snprintf(l->items[0].id, sizeof(l->items[0].id), "2026-08-20-a");
    snprintf(l->items[0].title, sizeof(l->items[0].title), "%s", title);
    snprintf(l->items[0].date, sizeof(l->items[0].date), "08-20");
}

static void kst_make_text(kidpal_story_text_t *st, const char *title,
                          const char *text)
{
    memset(st, 0, sizeof(*st));
    snprintf(st->id, sizeof(st->id), "2026-08-20-a");
    snprintf(st->title, sizeof(st->title), "%s", title);
    snprintf(st->text, sizeof(st->text), "%s", text);
}

/* mount gate（P0 资产链自诊断; must-stay-green）。 */
static void kst_mount_gate(void)
{
    const lv_font_t *body;
    int rc = kidpal_font_mount_assets(kst_host_reader);

    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        kst_fm("KST RED(infra): mount_assets(rc=%d) 非完成态——P0 资产/装载底座缺场"
               "（font_body_16.bin / kidpal_font.cc, P0 交付面）", rc));
    body = kidpal_font_body();
    TEST_ASSERT_TRUE_MESSAGE(body != NULL && body != &font_puhui_14_1,
        "KST RED(infra): body() 缺场或落 14px 兜底——资产在库却未装载（P0 装载面断裂）");
    TEST_ASSERT_TRUE_MESSAGE(body->line_height >= 20 && body->line_height <= 25,
        kst_fm("KST RED(infra): body16 line_height=%d 越出 16px 档 [20..25]（P0 "
               "FAS-A2 标定档）", (int)body->line_height));
}

/* 复位兜底态（防 mount 态跨文件泄漏）。 */
static void kst_reset_fallback(void)
{
    int rc = kidpal_font_mount_assets(kst_missing_reader);

    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): 复位 mount rc!=0");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_font_body() == &font_puhui_14_1 &&
                                 kidpal_font_title() == &font_puhui_14_1,
        "KST RED(infra): 复位后 body()/title() != font_puhui_14_1——14px 兜底语义被破");
}

/* 长标题夹具（22 字, 字形池内重复组合; 66B < title[96]; 16px 下 352px > 可用
 * ~276px 触发省略, 14px 旧字面下 308px 亦越 x=320 日期列——设计态红可归因）。 */
#define KST_LONG_TITLE \
    "小兔子找朋友勇敢的小明我的大冒险小兔子找朋友"

/* ========================================================================== *
 * TEST 1 — STY-A1: 列表 16px + 长标题省略不压日期（S-F2.1-AC1 / 目的4 / P3T-E1）
 *   tooth 0 (infra gate, must-stay-green): mount fonts_root -> body() 真资产句柄。
 *   tooth 1 (行为级, RED 设计态): mount 态渲染 1 行长标题列表——标题带墨高 ∈
 *            [13..16]（16px 证据域; 现状 14px 字面 -> 红）。
 *   tooth 2 (判别负控): 全缺 mount 态同一夹具标题带墨高 ∈ [10..13] 且 < tooth 1
 *            实测（14px/16px 经真实渲染路径确可判别）。
 *   tooth 3 (行为级, RED 设计态): 标题/日期分离带 x[316..319] 零墨（现状无定宽
 *            无 DOT, 22 字 @14px 标题墨越 x=320 压日期列 -> 红）。
 *   tooth 4 (must-stay-green): 日期带在墨（完整可见的在墨半）。
 *   tooth 5 (guard): 标题单行——标题带墨高 <= 18（折行即两视觉行 >= 30 红）。
 *   tooth 6 (guard): 省略触发填充——标题带墨宽 >= 240（可用宽 ~276px, 不足 = 定宽
 *            过窄牺牲可读）。
 *   tooth 7 (复位): mount(missing reader) 复位兜底态。
 *   RED（设计态）: tooth 1（14px 旧字面）+ tooth 3（标题压日期列, 目的4 缺失）。
 * ========================================================================== */
/* coverage: STY-A1 -- 列表行 16px 墨高证据域 + 判别负控 + 长标题单行省略不压日期列（分离带零墨）+ 日期在墨 + 省略触发填充 */
TEST_CASE("story typography: 列表16px+长标题省略不压日期 (STY-A1)",
          "[story_typography][STY-A1][behavior]")
{
    kidpal_story_list_t l;
    kidpal_story_view_t v;
    int rc, min_y, max_y, dark, min_x, max_x, dark_x, h16, h14, gap;

    KST_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KST RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    /* tooth 0: P0 资产 gate（must-stay-green）。 */
    kst_mount_gate();

    kst_make_list1(&l, KST_LONG_TITLE);
    rc = kidpal_story_view_init(&v, &l);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): view_init 失败——fixture 前提破坏");
    v.screens = 1;
    kst_render_snap(&v, &l, NULL, kst_f_a);

    /* tooth 1: 标题带 16px 墨高。 */
    kst_ink_span(kst_f_a, KST_TITLE_ZONE_X0, KST_ROW_BAND_Y0,
                 KST_TITLE_ZONE_X1, KST_ROW_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): 标题带零墨——夹具字形缺场（fixture 对齐协商, 不静默）");
    h16 = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h16 >= KST_INK16_MIN && h16 <= KST_INK16_MAX,
        kst_fm("KST-A1 RED: 列表标题墨高 %d 越出 16px 合理域 [%d..%d]——列表未切 P0 "
               "16px 句柄（[T1] 未实现, 现状 14px 字面 = 设计态红）或 16px 字形度量"
               "异常（须附实测证据重基线, 不静默放宽）", h16, KST_INK16_MIN,
               KST_INK16_MAX));

    /* tooth 2: 判别负控——全缺 mount（14px 兜底）同一夹具。 */
    rc = kidpal_font_mount_assets(kst_missing_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): 复位 mount rc!=0");
    kst_render_snap(&v, &l, NULL, kst_f_b);
    kst_ink_span(kst_f_b, KST_TITLE_ZONE_X0, KST_ROW_BAND_Y0,
                 KST_TITLE_ZONE_X1, KST_ROW_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): 14px 兜底态标题墨缺场——夹具字形缺场（fixture 对齐协商）");
    h14 = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h14 >= KST_INK14_MIN && h14 <= KST_INK14_MAX,
        kst_fm("KST-A1 RED(infra): 14px 兜底态墨高 %d 越出 [%d..%d]——兜底链度量异常"
               "（P0 [K2] 兜底面, 非本包设计态）", h14, KST_INK14_MIN, KST_INK14_MAX));
    TEST_ASSERT_TRUE_MESSAGE(h16 > h14,
        kst_fm("KST-A1 RED(infra): 16px 态墨高 %d 不大于 14px 态 %d——两态不可判别, "
               "tooth 1 合理域断言失去判别力（infra 修正不静默）", h16, h14));

    /* 重新 mount 16px 态（与后续 tooth 判读态一致）。 */
    rc = kidpal_font_mount_assets(kst_host_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): re-mount rc!=0");
    kst_render_snap(&v, &l, NULL, kst_f_a);

    /* tooth 3: 分离带零墨——不压日期列（目的4 核心; RED 设计态）。 */
    gap = kst_ink_count(kst_f_a, KST_GAP_X0, KST_ROW_BAND_Y0,
                        KST_GAP_X1, KST_ROW_BAND_Y1);
    TEST_ASSERT_TRUE_MESSAGE(gap == 0,
        kst_fm("KST-A1 RED: 标题/日期分离带 x[%d..%d] 墨点 %d != 0——长标题尾部省略"
               "缺失, 标题墨压日期列（S-F2.1 AC1「不挤占日期列」/ 目的4; 现状无定宽"
               "无 LONG_DOT = 设计态红）", KST_GAP_X0, KST_GAP_X1, gap));

    /* tooth 4: 日期带在墨（日期完整可见的在墨半; must-stay-green）。 */
    kst_ink_cols(kst_f_a, KST_DATE_ZONE_X0, KST_ROW_BAND_Y0,
                 KST_DATE_ZONE_X1, KST_ROW_BAND_Y1, &min_x, &max_x, &dark_x);
    TEST_ASSERT_TRUE_MESSAGE(dark_x > 0 && min_x >= KST_DATE_ZONE_X0,
        kst_fm("KST-A1 RED: 日期带零墨或越左界（min_x=%d dark=%d）——日期列被挤压或"
               "移位（S-F2.1 AC1「日期完整可见」）", min_x, dark_x));

    /* tooth 5: 单行 guard——折行即红。 */
    kst_ink_span(kst_f_a, KST_TITLE_ZONE_X0, KST_ROW_BAND_Y0,
                 KST_TITLE_ZONE_X1, KST_ROW_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(max_y - min_y + 1 <= 18,
        kst_fm("KST-A1 RED: 标题带墨高 %d > 18——长标题折行成两视觉行（省略语义要"
               "求单行尾部省略, 非换行）", max_y - min_y + 1));

    /* tooth 6: 省略触发填充 guard——定宽过窄即红。 */
    kst_ink_cols(kst_f_a, KST_TITLE_ZONE_X0, KST_ROW_BAND_Y0,
                 KST_TITLE_ZONE_X1, KST_ROW_BAND_Y1, &min_x, &max_x, &dark_x);
    TEST_ASSERT_TRUE_MESSAGE(max_x >= KST_TITLE_FILL_MIN,
        kst_fm("KST-A1 RED: 长标题填充 max_x=%d < %d——标题定宽过窄, 22 字省略触发"
               "面不足（省略已触发则应填至接近分离带）", max_x, KST_TITLE_FILL_MIN));

    /* tooth 7: 复位兜底态。 */
    kst_reset_fallback();
}

/* ========================================================================== *
 * TEST 2 — STY-A2: 阅读正文 16px（S-F2.3 补充规则 / P3T-E2 前半）
 *   tooth 0 (must-stay-green): mount gate。
 *   tooth 1 (行为级, RED 设计态): mount 态渲染单自然行短文——正文带墨高 ∈
 *            [13..16]（现状 14px 字面 -> 红）。
 *   tooth 2 (判别负控): 全缺 mount 态同一夹具墨高 ∈ [10..13] 且 < tooth 1 实测。
 *   tooth 3 (复位): 复位兜底态。
 *   （REVISION LOG revision 6 R12: 原 fixture 两自然行 "小兔子找朋友\n我的大冒险"
 *   经整带 y[44..200] 量 min_y..max_y = 两视觉行墨跨 ~37, 恒越 [13..16] = 对任何
 *   实现不可满足, 且与 STY-A4（同一 body 标签须 WRAP 折行、墨跨 >= 26）互斥——
 *   冻结件自身夹具缺陷, builder 实证（265/5 红名 verbatim 在 pod-events）。修订 =
 *   fixture 收敛单自然行, 「正文墨高 16px 证据域 + 判别负控」语义零缩水; 多行
 *   折行语义由 STY-A4 独立承载。）
 * ========================================================================== */
/* coverage: STY-A2 -- 阅读正文 16px 墨高证据域 + 14px 兜底判别负控（S-F2.3 补充规则正文 16px） */
TEST_CASE("story typography: 阅读正文16px (STY-A2)",
          "[story_typography][STY-A2][behavior]")
{
    kidpal_story_list_t l;
    kidpal_story_text_t st;
    kidpal_story_view_t v;
    int rc, min_y, max_y, dark, h16, h14;

    KST_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KST RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    kst_mount_gate();

    kst_make_list1(&l, "我的大冒险");
    /* R12: 单自然行夹具（revision 6; 旧两自然行夹具与墨高窗/STY-A4 互斥不可满足）。 */
    kst_make_text(&st, "我的大冒险", "小兔子找朋友");
    rc = kidpal_story_open_reader(&v, &st, KSTORY_READER_LINES_PER_SCREEN);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && v.page == KSTORY_PAGE_READER,
        "KST RED(infra): open_reader 失败——fixture 前提破坏");
    kst_render_snap(&v, &l, &st, kst_f_a);

    /* tooth 1: 正文带 16px 墨高（带 y 从 44 起, 标题带 y[2..41] 不入样）。 */
    kst_ink_span(kst_f_a, KST_BODY_X0, KST_READER_BODY_BAND_Y0,
                 KST_BODY_X1, KST_READER_BODY_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): 正文带零墨——夹具字形缺场（fixture 对齐协商, 不静默）");
    h16 = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h16 >= KST_INK16_MIN && h16 <= KST_INK16_MAX,
        kst_fm("KST-A2 RED: 阅读正文墨高 %d 越出 16px 合理域 [%d..%d]——正文未切 P0 "
               "16px 句柄（[T2] 未实现, 现状 14px 字面 = 设计态红）或 16px 字形度量"
               "异常（须附实测证据重基线, 不静默放宽）", h16, KST_INK16_MIN,
               KST_INK16_MAX));

    /* tooth 2: 判别负控。 */
    rc = kidpal_font_mount_assets(kst_missing_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): 复位 mount rc!=0");
    kst_render_snap(&v, &l, &st, kst_f_b);
    kst_ink_span(kst_f_b, KST_BODY_X0, KST_READER_BODY_BAND_Y0,
                 KST_BODY_X1, KST_READER_BODY_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): 14px 兜底态正文墨缺场");
    h14 = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h14 >= KST_INK14_MIN && h14 <= KST_INK14_MAX,
        kst_fm("KST-A2 RED(infra): 14px 兜底态墨高 %d 越出 [%d..%d]——兜底链度量异常",
               h14, KST_INK14_MIN, KST_INK14_MAX));
    TEST_ASSERT_TRUE_MESSAGE(h16 > h14,
        kst_fm("KST-A2 RED(infra): 16px 态墨高 %d 不大于 14px 态 %d——两态不可判别",
               h16, h14));

    kst_reset_fallback();
}

/* ========================================================================== *
 * TEST 3 — STY-A3: 自然行 4→3（F-4 / P3T-E2 前半 + P3T-E3 同步面）
 *   tooth 1 (机制级, RED 设计态): include 直读常量——KSTORY_READER_LINES_PER_SCREEN
 *            == 3（现状 4 -> 红）且 != 4（旧值退役）; KSTORY_LIST_ROWS_PER_SCREEN
 *            == 4 零偏移（列表分屏不随动, must-stay-green）。
 *   tooth 2 (行为级, RED 设计态): open_reader（常量注入）7 自然行 -> 3 屏
 *            （现状 4 行/屏 -> 2 屏 -> 红）。
 *   tooth 3 (行为级, RED 设计态): reader_slice 第 0 屏恰含 3 自然行（切片内 2 个
 *            '\n'; 现状 4 行 -> 3 个 -> 红）; 第 2 屏含末行; 第 3 屏 NULL（守卫）。
 *   RED（设计态）: tooth 1/2/3（同根 = 常量未改）。
 * ========================================================================== */
/* coverage: STY-A3 -- 自然行 4→3: 常量单源直读 ==3（rows4 零偏移）+ open_reader 7 行 3 屏 + slice 恰 3 自然行行为配对 */
TEST_CASE("story typography: 自然行4→3常量与行为配对 (STY-A3)",
          "[story_typography][STY-A3][structural][behavior]")
{
    kidpal_story_text_t st;
    kidpal_story_view_t v;
    const char *slice;
    int lines, rows, slen, i, nl;
    int rc;

    /* tooth 1: 常量单源（include 直读, 断言编译绑定到 story_page.h; 经局部变量
     * 比较, 避免常量折叠告警）。 */
    lines = KSTORY_READER_LINES_PER_SCREEN;
    rows = KSTORY_LIST_ROWS_PER_SCREEN;
    TEST_ASSERT_TRUE_MESSAGE(rows == 4,
        kst_fm("KST-A3 RED: KSTORY_LIST_ROWS_PER_SCREEN = %d, 期望 4 零偏移——列表"
               "分屏不随 16px 排版改动（16px 行高 23 < 行距 42, 零重排需求; 既有 "
               "SPV2-A2 [K8] 齿续钉）", rows));
    TEST_ASSERT_TRUE_MESSAGE(lines == 3,
        kst_fm("KST-A3 RED: KSTORY_READER_LINES_PER_SCREEN = %d, 期望 3——自然行 "
               "4→3 未落（F-4/P3T-E2, story_page.h 单源; 现状 4 = 设计态红）", lines));
    TEST_ASSERT_TRUE_MESSAGE(lines != 4,
        "KST-A3 RED: 阅读每屏自然行仍为旧值 4——旧锚未退役（F-4 重基线）");

    /* fixture: 7 自然行（内容为解析面消费, 无字形依赖）。 */
    kst_make_text(&st, "长文", "L1\nL2\nL3\nL4\nL5\nL6\nL7");

    /* tooth 2: open_reader 常量注入 -> 3 屏。 */
    rc = kidpal_story_open_reader(&v, &st, lines);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && v.page == KSTORY_PAGE_READER &&
                                 v.screens == 3 && v.screen == 0,
        kst_fm("KST-A3 RED: open_reader(7 自然行, 每屏 %d) -> screens=%d, 期望 3"
               "（ceil(7/3)）——分页行为未随常量收敛（现状 4 行/屏 = 2 屏, 设计态红）",
               lines, v.screens));

    /* tooth 3: 切片行为配对。 */
    slice = kidpal_story_reader_slice(&st, 0, lines, &slen);
    TEST_ASSERT_TRUE_MESSAGE(slice != NULL && slen > 0,
        "KST-A3 RED: 第 0 屏切片 NULL——切片面回归");
    nl = 0;
    for (i = 0; i < slen; i++) {
        if (slice[i] == '\n') {
            nl++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(nl == 2,
        kst_fm("KST-A3 RED: 第 0 屏切片含 %d 个换行, 期望 2（恰 3 自然行/屏）——"
               "切片粒度未随 4→3 收敛（现状 4 行 = 3 换行, 设计态红）", nl));
    slice = kidpal_story_reader_slice(&st, 2, lines, &slen);
    TEST_ASSERT_TRUE_MESSAGE(slice != NULL && strstr(slice, "L7") != NULL,
        "KST-A3 RED: 第 2 屏（末屏）切片未含末行 L7——切片边界漂移");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_reader_slice(&st, 3, lines, &slen) == NULL,
        "KST-A3 RED(guard): 越界屏（>= screens）未按 NULL 拒收");
}

/* ========================================================================== *
 * TEST 4 — STY-A4: 长自然行折行不越屏（F-4 / P3T-E2 前半; guard——现状 WRAP 在世,
 *          本齿为折行语义回归守卫, 设计态绿）
 *   tooth 1 (mount): 16px 态。
 *   tooth 2 (行为级): 单条 30 字自然行渲染——折行发生（正文带垂直墨跨 >= 26, 两
 *            视觉行）且墨不越 x[12..396]（label 区 + 容差; 摘定宽/WRAP 即溢出红）。
 *   tooth 3 (复位): 复位兜底态。
 * ========================================================================== */
/* coverage: STY-A4 -- 长自然行折行不越屏（折行发生 + 墨不越 label 区/屏缘, [T4]） */
TEST_CASE("story typography: 长自然行折行不越屏 (STY-A4)",
          "[story_typography][STY-A4][behavior]")
{
    kidpal_story_list_t l;
    kidpal_story_text_t st;
    kidpal_story_view_t v;
    int rc, min_y, max_y, dark, min_x, max_x, dark_x;

    KST_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KST RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    rc = kidpal_font_mount_assets(kst_host_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): mount rc!=0");

    /* 30 字单自然行: 14px 420px / 16px 480px 均 > 368px label 宽 -> 两态都折行
     * （本齿对两字号态都绿 = 纯折行语义守卫）。 */
    kst_make_list1(&l, "我的大冒险");
    kst_make_text(&st, "我的大冒险",
                  "小兔子找朋友勇敢的小明我的大冒险小兔子找朋友勇敢的小明我的大");
    rc = kidpal_story_open_reader(&v, &st, KSTORY_READER_LINES_PER_SCREEN);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): open_reader 失败");
    kst_render_snap(&v, &l, &st, kst_f_a);

    kst_ink_span(kst_f_a, KST_BODY_X0, KST_READER_BODY_BAND_Y0,
                 KST_BODY_X1, KST_READER_BODY_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): 正文带零墨——夹具字形缺场");
    TEST_ASSERT_TRUE_MESSAGE(max_y - min_y + 1 >= 26,
        kst_fm("KST-A4 RED: 30 字自然行垂直墨跨 %d < 26——折行未发生（整行被裁剪/省"
               "略而非换行, F-4「长自然行折行不越屏」的折行半缺失）",
               max_y - min_y + 1));
    kst_ink_cols(kst_f_a, 0, KST_READER_BODY_BAND_Y0, KST_FB_W - 1,
                 KST_READER_BODY_BAND_Y1, &min_x, &max_x, &dark_x);
    TEST_ASSERT_TRUE_MESSAGE(min_x >= KST_BODY_X0 && max_x <= KST_BODY_X1,
        kst_fm("KST-A4 RED: 正文墨 x[%d..%d] 越出 [%d..%d]——长自然行折行越屏（body "
               "定宽 + WRAP 语义被拆, F-4/P3T-E2）", min_x, max_x, KST_BODY_X0,
               KST_BODY_X1));

    kst_reset_fallback();
}

/* ========================================================================== *
 * TEST 5 — STY-A5: 阅读标题 20px 居中/单行/省略/不压正文 + 16px 回退消费点守卫
 *          （目的2 / S-F2.3 补充规则 / F-6 消费点半 / P3T-E2 后半）
 *   tooth 1 (mount): 16px 态 + title 真资产句柄（非 14px、20 档 [26..36]）。
 *   tooth 2 (行为级, RED 设计态): 短标题（我的大冒险）——标题带墨高 ∈ [16..20]
 *            （现状 14px -> 红）+ 水平居中 |墨心-200| <= 24（现状左对齐 -> 红）。
 *   tooth 3 (guard): 不压正文——标题带墨 max_y <= 42 且正文带 min_y >= 43（分离）。
 *   tooth 4 (行为级): 长标题（22 字）——单行（墨高 <= 26）+ 填充触发（min_x <= 80
 *            且 max_x >= 300）。
 *   tooth 5 (机制级, must-stay-green): title()->fallback == body()（20px 缺字回退
 *            16px 的 label 消费通路; 语义本体 = P0 FAS-A5）。
 *   tooth 6 (复位): 复位兜底态。
 *   RED（设计态）: tooth 2 墨高窗 + 居中。
 * ========================================================================== */
/* coverage: STY-A5 -- 阅读标题 20px: 墨高 20px 证据域 + 水平居中 + 单行 + 长标题省略填充 + 不压正文分离 + fallback 链消费点守卫 */
TEST_CASE("story typography: 阅读标题20px居中省略+回退守卫 (STY-A5)",
          "[story_typography][STY-A5][behavior]")
{
    kidpal_story_list_t l;
    kidpal_story_text_t st;
    kidpal_story_view_t v;
    const lv_font_t *title, *body;
    int rc, min_y, max_y, dark, min_x, max_x, dark_x, h20, center;

    KST_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KST RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    /* tooth 1: mount gate + title 20 档。 */
    kst_mount_gate();
    title = kidpal_font_title();
    TEST_ASSERT_TRUE_MESSAGE(title != NULL && title != &font_puhui_14_1,
        "KST RED(infra): title() 缺场或落 14px 兜底——20px 资产在库却未装载");
    TEST_ASSERT_TRUE_MESSAGE(title->line_height >= 26 && title->line_height <= 36,
        kst_fm("KST RED(infra): title20 line_height=%d 越出 20px 档 [26..36]（P0 "
               "FAS-A2 标定档）", (int)title->line_height));
    body = kidpal_font_body();

    /* tooth 2: 短标题——20px 墨高 + 居中。 */
    kst_make_list1(&l, "我的大冒险");
    kst_make_text(&st, "我的大冒险", "小兔子找朋友");
    rc = kidpal_story_open_reader(&v, &st, KSTORY_READER_LINES_PER_SCREEN);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): open_reader 失败");
    kst_render_snap(&v, &l, &st, kst_f_a);
    kst_ink_span(kst_f_a, 0, KST_READER_TITLE_BAND_Y0, KST_FB_W - 1,
                 KST_READER_TITLE_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): 标题带零墨——夹具字形缺场");
    h20 = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h20 >= KST_INK20_MIN && h20 <= KST_INK20_MAX,
        kst_fm("KST-A5 RED: 阅读标题墨高 %d 越出 20px 合理域 [%d..%d]——标题未切 P0 "
               "20px 句柄（[T5] 未实现, 现状 14px 字面 = 设计态红）或 20px 字形度量"
               "异常（须附实测证据重基线, 不静默放宽）", h20, KST_INK20_MIN,
               KST_INK20_MAX));
    kst_ink_cols(kst_f_a, 0, KST_READER_TITLE_BAND_Y0, KST_FB_W - 1,
                 KST_READER_TITLE_BAND_Y1, &min_x, &max_x, &dark_x);
    center = (min_x + max_x) / 2;
    TEST_ASSERT_TRUE_MESSAGE(center >= KST_CENTER_TARGET - KST_CENTER_TOL &&
                                 center <= KST_CENTER_TARGET + KST_CENTER_TOL,
        kst_fm("KST-A5 RED: 标题墨心 x=%d 越出居中带 [%d±%d]——阅读标题未水平居中"
               "（目的2/S-F2.3 补充规则; 现状 x=12 左对齐 = 设计态红）", center,
               KST_CENTER_TARGET, KST_CENTER_TOL));

    /* tooth 3: 不压正文（分离 guard; 标题版式锚 y8 / 正文 y44 零重排假设）。 */
    TEST_ASSERT_TRUE_MESSAGE(max_y <= 42,
        kst_fm("KST-A5 RED: 标题墨 max_y=%d > 42——标题侵入正文带（不压正文义务破; "
               "或标题折行/版式重排——fixture 对齐协商走 planner, 不静默）", max_y));
    kst_ink_span(kst_f_a, KST_BODY_X0, KST_READER_BODY_BAND_Y0,
                 KST_BODY_X1, KST_READER_BODY_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 43,
        kst_fm("KST-A5 RED: 正文带零墨或 min_y=%d < 43——正文版式被重排/标题被压",
               min_y));

    /* tooth 4: 长标题——单行 + 填充触发。 */
    kst_make_text(&st, KST_LONG_TITLE, "小兔子找朋友");
    rc = kidpal_story_open_reader(&v, &st, KSTORY_READER_LINES_PER_SCREEN);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): open_reader 长标题半失败");
    kst_render_snap(&v, &l, &st, kst_f_a);
    kst_ink_span(kst_f_a, 0, KST_READER_TITLE_BAND_Y0, KST_FB_W - 1,
                 KST_READER_TITLE_BAND_Y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && max_y - min_y + 1 <= 26,
        kst_fm("KST-A5 RED: 长标题墨高 %d > 26——折行成两视觉行（单行省略义务破）",
               max_y - min_y + 1));
    kst_ink_cols(kst_f_a, 0, KST_READER_TITLE_BAND_Y0, KST_FB_W - 1,
                 KST_READER_TITLE_BAND_Y1, &min_x, &max_x, &dark_x);
    TEST_ASSERT_TRUE_MESSAGE(min_x <= 80 && max_x >= 300,
        kst_fm("KST-A5 RED: 长标题墨 x[%d..%d] 未近满宽填充——20px 尾部省略触发面"
               "不足（22 字 @20px = 440px > 版面, 省略应触发并近满宽）", min_x,
               max_x));

    /* tooth 5: fallback 链消费点守卫（F-6 机制半; must-stay-green——P0 FAS-A5
     * tooth 3 同锚, 此处钉「真实消费点拿到的句柄」仍带链）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_font_title()->fallback == body,
        "KST-A5 RED: title()->fallback != body()——20px 缺字回退 16px 消费点守卫破"
        "（P0 [K3] fallback 链被拆或 mount 后被改写; F-6）");

    kst_reset_fallback();
}

/* ========================================================================== *
 * TEST 6 — STY-A6: 未变更页态不跟随放大（F-1 负半 / plan.md 铁律 / P3T-E1/E3
 *          回归半; guard——现状即 14px, 设计态绿; 误放大即红）
 *   tooth 1 (mount): 16px 态下测（缺 assets 态 14px 兜底不可判别, 判别力前提 =
 *            资产在位而未变更页态仍守 14px）。
 *   tooth 2: DELETE 提问带墨高 ∈ [10..13]。
 *   tooth 3: BODY_UNAVAILABLE 大字带墨高 ∈ [10..13]。
 *   tooth 4: STAMP 文案带墨高 ∈ [10..13]。
 *   tooth 5: RECORDING 气泡带墨高 ∈ [10..13]（ORGANIZING 同 helper 路径间接承载）。
 *   tooth 6 (复位): 复位兜底态。
 * ========================================================================== */
/* coverage: STY-A6 -- 未变更页态 14px 负半（DELETE/BODY_UNAVAILABLE/STAMP/RECORDING 四带, F-1「未变更面不跟随放大」） */
TEST_CASE("story typography: 未变更页态不跟随放大 (STY-A6)",
          "[story_typography][STY-A6][behavior]")
{
    kidpal_story_list_t l;
    kidpal_story_text_t st;
    kidpal_story_view_t v;
    int rc, min_y, max_y, dark, h;

    KST_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KST RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    kst_mount_gate();
    kst_make_list1(&l, "我的大冒险");
    kst_make_text(&st, "我的大冒险", "小兔子找朋友\n我的大冒险");

    /* tooth 2: DELETE 提问（「删掉这篇？」@(140,56), 14px）——待删条目盒 y118 起
     * 不入样。 */
    rc = kidpal_story_view_init(&v, &l);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "KST RED(infra): view_init 失败");
    rc = kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
    TEST_ASSERT_TRUE_MESSAGE(rc == KSPV2_ACT_DELETE_ENTER && v.page == KSTORY_PAGE_DELETE,
        "KST RED(infra): 进入删除确认子态失败——fixture 前提破坏（SPV2-A1 tooth 6 同径）");
    kst_render_snap(&v, &l, NULL, kst_f_a);
    kst_ink_span(kst_f_a, 136, 50, 266, 82, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): DELETE 提问带零墨——fixture 前提破坏");
    h = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h >= KST_INK14_MIN && h <= KST_INK14_MAX,
        kst_fm("KST-A6 RED: DELETE 提问墨高 %d 越出 14px 域 [%d..%d]——删除确认页态"
               "被放大（F-1 未变更页态不跟随放大; plan.md 铁律）", h, KST_INK14_MIN,
               KST_INK14_MAX));

    /* tooth 3: BODY_UNAVAILABLE 大字（「这篇暂时打不开」TOP_MID y=64, 14px）——
     * 副句 y=96 与条目盒 y130 不入样。 */
    kidpal_story_view_init(&v, &l);
    v.page = KSTORY_PAGE_BODY_UNAVAILABLE;
    kst_render_snap(&v, &l, NULL, kst_f_a);
    kst_ink_span(kst_f_a, 140, 58, 262, 88, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): BODY_UNAVAILABLE 大字带零墨——fixture 前提破坏");
    h = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h >= KST_INK14_MIN && h <= KST_INK14_MAX,
        kst_fm("KST-A6 RED: 正文取不回提示大字墨高 %d 越出 14px 域 [%d..%d]——取不"
               "回提示页态被放大（F-1; plan.md 铁律）", h, KST_INK14_MIN,
               KST_INK14_MAX));

    /* tooth 4: STAMP 文案（居中 y=158, 14px）——印章外圈（sz=140 @y=10, 6px 边框
     * + -8° transform 渲染缘）底缘侵入 y<=151（R13 实测修订: 旧带 y0=150 与圈底
     * 缘相切, 带内墨 = 圈侵入非文案越窗, 基线态即红 = 冻结件夹具缺陷）; y0 收至
     * 152 避开圆环侵入带, 文案墨 y158..169 完整在带。STAMP 几何零触碰（F-1 未
     * 变更页态铁律 + SPV2-B1 文案带在墨锚零触碰）。pose y>=178 不入样。 */
    kidpal_story_view_init(&v, &l);
    rc = kidpal_story_on_saved(&v, "2026-08-28-d", 6);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && v.page == KSTORY_PAGE_STAMP,
        "KST RED(infra): on_saved 进盖章帧失败——fixture 前提破坏");
    kst_render_snap(&v, &l, &st, kst_f_a);
    kst_ink_span(kst_f_a, 60, 152, 340, 176, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): STAMP 文案带零墨——fixture 前提破坏");
    h = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h >= KST_INK14_MIN && h <= KST_INK14_MAX,
        kst_fm("KST-A6 RED: 盖章文案墨高 %d 越出 14px 域 [%d..%d]——盖章页态被放大"
               "（F-1; SPV2-B1 文案带在墨锚续钉）", h, KST_INK14_MIN, KST_INK14_MAX));

    /* tooth 5: RECORDING 气泡（v7 帧 A 「你讲吧，我听着呢」@(35,36), 14px）——
     * 气泡框边 x24..27/y28..30 不入样, 回显列 x>=180 不入样（ORGANIZING 共用
     * ksy_bubble helper 路径, 经此带间接承载）。 */
    kidpal_story_view_init(&v, &l);
    v.page = KSTORY_PAGE_RECORDING;
    kst_render_snap(&v, &l, NULL, kst_f_a);
    kst_ink_span(kst_f_a, 40, 34, 150, 52, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "KST RED(infra): RECORDING 气泡带零墨——fixture 前提破坏");
    h = max_y - min_y + 1;
    TEST_ASSERT_TRUE_MESSAGE(h >= KST_INK14_MIN && h <= KST_INK14_MAX,
        kst_fm("KST-A6 RED: 录音态气泡墨高 %d 越出 14px 域 [%d..%d]——录音/整理页态"
               "被放大（F-1; plan.md 铁律点名页态）", h, KST_INK14_MIN, KST_INK14_MAX));

    kst_reset_fallback();
}

/* ========================================================================== *
 * TEST 7 — STY-A7: FAS-MANIFEST 扩键结构半（构建清单档; P3T-E3 mcp 同步 + 接线
 *          守卫）
 *   tooth 1: 八新键逐键（键缺 = infra 红自诊断——接线增量未落线; 键 0 = 结构红
 *            设计态）。
 *   tooth 2: 复用键（story_list_rows4 = 列表分屏 4 零偏移, must-stay-green）。
 *   tooth 3: 清单面自检（版本键 + 负向探针 + 产物可见）。
 *   RED（设计态）: story_c_font_body_seam / story_c_font_title_seam /
 *            story_h_reader_lines3 / story_h_reader_lines_not4 / story_c_dot_ge3
 *            五键 = 0（现状字面在世/缺位）; story_mcp_lines_sync / puhui14_kept /
 *            rows4 = must-stay-green; 接线键缺 = infra 红（wiring 未落线）。
 * ========================================================================== */
/* coverage: STY-A7 -- FAS 扩键结构半: mcp 符号引用同步 + P0 句柄字面源 + 常量 3 新值/4 退役 + DOT 机制 canary + 14px 兜底面在世 + 接线注册 */
TEST_CASE("story typography: 结构半-FAS清单扩键 (STY-A7)",
          "[story_typography][STY-A7][structural]")
{
    KST_REQUIRE_ROOT();

    /* tooth 1: 八新键。 */
    kst_require_fact(kst_mfact("host_srcs_story_typography"),
                     "host_srcs_story_typography",
                     "story_typography.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS"
                     "（acceptance-wiring-patch.md 增量 1）");
    kst_require_fact(kst_mfact("story_c_font_body_seam"), "story_c_font_body_seam",
                     "story_page.c 未消费 P0 [K4] kidpal_font_body——列表/阅读正文 "
                     "16px 字面源未切（[T1]/[T2], BUILDER DUTY 1a, 设计态红）");
    kst_require_fact(kst_mfact("story_c_font_title_seam"), "story_c_font_title_seam",
                     "story_page.c 未消费 P0 [K4] kidpal_font_title——阅读标题 20px "
                     "字面源未切（[T5], BUILDER DUTY 1a, 设计态红）");
    kst_require_fact(kst_mfact("story_c_puhui14_kept"), "story_c_puhui14_kept",
                     "story_page.c 丢失 font_puhui_14_1——14px 兜底面被全局替换"
                     "（F-1 未变更面纪律破; 行为半 = STY-A6）");
    kst_require_fact(kst_mfact("story_c_dot_ge3"), "story_c_dot_ge3",
                     "story_page.c LV_LABEL_LONG_DOT 计数 < 3——列表/阅读标题省略"
                     "机制未落（[T1]/[T5]; 现役 2 处回显钳制, 新增省略 = 设计态红）");
    kst_require_fact(kst_mfact("story_h_reader_lines3"), "story_h_reader_lines3",
                     "story_page.h 无 #define KSTORY_READER_LINES_PER_SCREEN 3——"
                     "自然行 4→3 未落（[T3], BUILDER DUTY 2, 设计态红）");
    kst_require_fact(kst_mfact("story_h_reader_lines_not4"), "story_h_reader_lines_not4",
                     "story_page.h 仍含 KSTORY_READER_LINES_PER_SCREEN 4 旧值字面"
                     "——旧锚未退役（F-4 重基线, 设计态红）");
    kst_require_fact(kst_mfact("story_mcp_lines_sync"), "story_mcp_lines_sync",
                     "mcp_server.cc KSTORY_READER_LINES_PER_SCREEN 符号引用 < 2——"
                     "两个消费点符号引用面被拆/回退字面硬编码（P3T-E3 同步前提; "
                     "must-stay-green, 红 = 真回归）");

    /* tooth 2: 复用键（must-stay-green）。 */
    kst_require_fact(kst_mfact("story_list_rows4"), "story_list_rows4",
                     "story_page.h #define KSTORY_LIST_ROWS_PER_SCREEN 4 锚丢失——"
                     "列表分屏单源被连带触碰（列表分屏语义不变义务破）");

    /* tooth 3: 清单面自检。 */
    TEST_ASSERT_TRUE_MESSAGE(kst_mfact("fas_manifest_version") == 1,
        "KST RED(infra): 清单版本键缺失——FAS 清单机制失灵或版本漂移");
    TEST_ASSERT_TRUE_MESSAGE(kst_mfact("__kst_no_such_key__") == -1,
        "KST RED(infra): 负向探针失败——未知键被误报为在（清单解析失灵）");
    {
        char p[KST_MAX_PATH];
        TEST_ASSERT_TRUE_MESSAGE(kst_rel(p, sizeof(p), KST_FAS_MANIFEST) &&
                                     kst_is_regular(p),
            "KST RED(infra): 清单产物不可见——未 build 或路径漂移");
    }
}

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-11-02）:
 *
 *   通道① host runner 全量（本文件未接线前）= story_page_v2 收紧件
 *     （REVISION LOG revision 5: SPV2-B2 设计态红）+ font_assets_runtime FAS-A8
 *     换锚红基线（见 taskpackage redBaselineResult——存量 auxbar×3 + 两换锚设计
 *     态红）; 本文件不在 runner 内、不占 Tests 数。
 *   通道② C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4, 命令见头注 Run
 *     protocol 同款 compile_commands 旗标法）: 编译通道 error 为零 = 验收文件
 *     本身可编译（2026-11-02 实测 rc=0、零 error）。
 *   通道③ 接线落线后（builder step-0 现跑）执行通道期望（Tests = 258 + 7 = 265）:
 *     - 本包义务红（设计态, builder 落 BUILDER DUTY 1)-2) 后转绿）:
 *       TEST 1 tooth 1（列表标题 14px 旧字面 -> 墨高越窗）+ tooth 3（无定宽无
 *         DOT, 22 字标题墨压日期列分离带）;
 *       TEST 2 tooth 1（阅读正文 14px 旧字面）;
 *       TEST 3 tooth 1（常量仍 4）+ tooth 2（open_reader 7 行 -> 2 屏）+ tooth 3
 *         前半（切片 3 换行 != 2）;
 *       TEST 5 tooth 2（标题 14px 墨高越窗 + 左对齐墨心越居中带）;
 *       TEST 7 五结构键（story_c_font_body_seam / story_c_font_title_seam /
 *         story_c_dot_ge3 / story_h_reader_lines3 / story_h_reader_lines_not4）。
 *     - MUST-STAY-GREEN at baseline（现状即真）: TEST 1 tooth 0/2(14px 半)/4/5/6;
 *       TEST 2 tooth 0/2(14px 半); TEST 3 rows4 半 + tooth 3 末屏/越界守卫半;
 *       TEST 4 全部; TEST 5 tooth 1/3/4/5; TEST 6 全部; TEST 7 接线键 +
 *       story_mcp_lines_sync / story_c_puhui14_kept / story_list_rows4 + 自检。
 *     - 除上述义务红与 p1_auxbar AB-A1/A2/A4 存量外零其它新增红; 任何 infra 红
 *       （清单键缺/探针失灵/mount 失败/字形缺场）= 接线或底座缺陷, 不冒充设计
 *       态红——停下回报。**零段错误**。
 *   版式锚协商点（fixture 对齐路径, 不静默放宽）: 列表行几何 KSTORY_LIST_ROW_Y0=
 *     44/ROW_STEP=42、READER 标题 y=8 / 正文 y=44、日期列 x=320——builder 调整行
 *     几何须走 planner 对齐本件带表（P2 修订轮先例）。
 *   真机半（P3T-E4）: 四页并集目击 / 长标题与超限截断 / 翻页手感（F-4 终判）/
 *     缺 assets 14px 降级（human 2026-10-29 豁免在案）——本包终端真机轮, human
 *     终判后 M6 收口; 本文件零 host 齿不伪装。
 * --------------------------------------------------------------------------- */
/* REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-11-02  initial freeze (acceptance-author, TaskPackage
 *               261028-m6-k3-p3-story-typography; 授权链 = 本包 exitCriteria
 *               P3T-E1/E2/E3 + requirements v1.69 S-F2.1 AC1 / S-F2.3 补充规则
 *               排版句（2026-10-28 批改写）+ FROZEN PROPOSAL F-1/F-4/F-6 +
 *               §1-目的2/4 + §5 + impactedTestsSeed）。标定窗 host 实测（临时
 *               探针程序实测后删除）: body16 line_height=23 / CJK 墨高 14-15、
 *               title20 line_height=28 / CJK 墨高 17-19、title->fallback==body
 *               实证——窗口 [13..16]/[16..20]/[10..13] 据此冻结（P2 同资产
 *               [13..16]/[10..13] 同源同值）。fixture 字形池 = 既有冻结件已核验
 *               字形（SPV2-B1 探针集）。seam 形态决策留痕: story_page.c 为
 *               LVGL-ful TU, 直接消费 P0 [K4]（P2 [F2] 同款, 不另造间接 seam）。
 *               与任务面偏差申报: 无（changeManifest 五件 = taskpackage 在册面）。
 *   2026-11-02  revision 6 (收紧型修订轮, builder 交付后通道① host 终态 265 Tests
 *               / 5 Failures——义务红 6 条全绿 + auxbar×3 存量, 剩 STY-A2/STY-A6
 *               两红经 builder 实证为冻结件夹具几何不可满足〔非实现回归; 基线对照
 *               一致 + 上移探针转绿〕; 授权链不变 = 本包 exitCriteria P3T-E2 +
 *               requirements v1.69 + PROPOSAL F-1/F-4/F-6; 只动冻结件、实现零触碰）:
 *     R12 STY-A2 夹具收敛单自然行——旧夹具两自然行 "小兔子找朋友\n我的大冒险" 经
 *        整带 y[44..200] 量 min_y..max_y = 两视觉行墨跨 ~37, 恒越 [13..16] = 对
 *        任何实现不可满足, 且与 STY-A4（同一 body 标签须 WRAP 折行、墨跨 >= 26）
 *        互斥。修订 = fixture 改单自然行 "小兔子找朋友"; 「正文墨高 16px 证据域 +
 *        14px 判别负控」语义零缩水（窗口 [13..16]/[10..13] 经单行实测维持——单行
 *        16px 墨高 14-15 / 14px 11-12, 冻结窗不动）; 多行折行语义由 STY-A4 独立
 *        承载（零重复钉）。
 *     R13 STY-A6 STAMP 判读带 y0 150 -> 152——旧带 y[150..176] 与印章外圈（sz=140
 *        @y=10、6px 边框 + -8° transform 渲染缘）底缘相切, 带内墨 = 圈侵入非文案
 *        越窗, 基线态即红 = 冻结件夹具缺陷（builder 实测证据在 pod-events）。修订
 *        = 带顶收至 152 避开圆环侵入带; 文案墨 y158..169 完整在带, 14px 判别语义
 *        成立（修订后现跑绿）。**STAMP 渲染几何零触碰**（F-1 未变更页态铁律 +
 *        SPV2-B1 文案带在墨锚零触碰）。
 *     R14 FAS 键名漂移申报（非断言修订, 纯留痕）: 接线补丁增量 6b 发键
 *        story_h_reader_lines3（本文件 [M]/[T7] 消费）, font_assets_runtime
 *        FAS-A8 revision 3 消费键 story_reader_lines3（无 _h_）; builder 在
 *        CMakeLists 增发同值别名桥接（两键同事实同值）。**申报: story_reader_
 *        lines3 = story_h_reader_lines3 的接线别名（同事实、恒同值）, 两键并存为
 *        准**——不改 CMake、不改键名, 两件消费各自键名均合法; 任一键缺失/两键值
 *        分叉 = 接线缺陷（infra 红）。
 *     修订后通道① host runner 全量现跑（builder 实现在树）: **265 Tests /
 *        3 Failures / 0 Ignored** = 仅存量 p1_auxbar AB-A1/A2/A4（launch-card
 *        基线锚在册）——本文件 STY-A1..A7 全绿, 既有齿零偏移, 零段错误。
 *     修订 diff 纯收紧核对: 既有断言仅 STY-A2 fixture 行与 STY-A6 STAMP 带两处
 *        修正（均为冻结件自身夹具缺陷修正, tighten-only「修自己 fixture 缺陷」
 *        条款）, 其余断言零触碰; REVISION LOG 留痕如上。
 * --------------------------------------------------------------------------- */
