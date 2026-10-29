/*
 * Frozen acceptance tests — TaskPackage 261028-m6-k1-font-assets-runtime-loading
 * (subsystem: firmware 字体资产产线 + assets 分区/cbin 运行时装载 + host 同源字体
 * seam · mergeOrder 60 · rigor=full / riskTier=high · sourceType=proposal)。
 *
 * Story: 261028-font-typography-scaleup 列车 P0 spike（ADR-017 资产与运行时底座切片）。
 * 决策面 = PROPOSAL §3.1 F-5 / F-6 / F-7 + §3.2 OQ1 / OQ3 + docs/decisions/ADR.md
 * ADR-017（冻结: sha1:af2e3627a8a2f254）。
 *
 * ===========================================================================
 * 三分申报（launch-card 留根条款：设备 TU 不进 host 构建的包，验收设计一开始就
 * 把三分申报写死；HOST_TEST_CONVENTIONS.md INF-E2 矩阵同源）:
 *
 *   [host 可验]（本文件全部 TEST_CASE）:
 *     - 统一 kidpal_font 句柄 seam（[K1]-[K4]）的装载 / 降级 / 缺字回退语义。
 *       cbin 装载 host 半 = kidpal_font 内编译期指针宽度感知解码器（预基线修正
 *       裁定 2：host 行为级测试全经 [K2]，不直调 cbin_font_create——32-bit 假设
 *       装载器在 64-bit host 直调必崩，dsc 槽 @24 vs @40，step-0 段错误实测）。
 *     - 资产产线与资产面的结构半（gen_fonts.sh 扩产行 + --format cbin provenance
 *       + fonts_root 两枚 .bin 落盘 + assets 打包接线）——结构事实由 host
 *       CMakeLists configure 期采集进构建清单（FAS-MANIFEST，见下节），验收件
 *       运行时只消费构建产物与 fonts_root 真实 .bin（产物面）。源码树文本扫查
 *       已按 human 定向裁定（2026-10-28，planner 留痕）整体移除：运行时禁扫
 *       源码树、禁影子树镜像拷贝。
 *
 * ===========================================================================
 * FAS-MANIFEST 结构清单机制（结构面断言的唯一采集通道；替代运行时树扫）:
 *   采集 = firmware/main/test/host_app/main/CMakeLists.txt 的 FAS 块在 configure
 *   期完成（file(READ)/EXISTS + string(FIND)），结果写成构建产物
 *   build/fas_structure_manifest.txt（key=<0|1> 逐行）。验收件在测试运行时
 *   只读该构建产物（fas_mfact()），不读任何源文件——清单是构建产物，不是
 *   源码树镜像（零拷贝）。
 *   新鲜度 = 被扫源文件全部登记 CMAKE_CONFIGURE_DEPENDS：改任一被扫文件即触发
 *   re-configure 重采集；红判读若发现清单事实与树不符，先重跑 build 再判。
 *   红形态 = fas_require_fact：键值 0 = 结构红（对应 BUILDER DUTY 责任面）；
 *   清单缺失或键不在 = infra 红（自诊断，不冒充结构红）。负向探针（未知键返回
 *   缺）由 TEST 7 tooth 3 钉。变异面 = 被扫源文件字面即断言锚：变异触碰 needle
 *   或删被扫文件 → re-configure → 清单事实翻转 → 对应验收红。
 *   键集与键语义 = acceptance-wiring-patch.md 改动 3（预基线修正 2026-10-28 起：
 *   host_srcs_no_cbin_font 为负向接线事实——cbin_font.c 不得进 host 构建；
 *   gen_fonts_sh_format_cbin 为 provenance 键——产线须走 78 fork --format cbin）。
 *
 *   [设备专有]（三分申报续；host 构建不覆盖，不写假行为断言，由 device build 闸 +
 *     FAS-MANIFEST 构建清单承载；清单断言只钉 configure 期可验的符号/引用存在性 =
 *     构建清单档，非行为本体——INF-E2「设备版 OTA / NVS / 分区」行）:
 *     - cbin_font_create 真实装载面（设备 32-bit；cbin_font.c 仍编入设备构建，
 *       不进 host 构建——负向接线事实 host_srcs_no_cbin_font，裁定 2）。
 *     - assets 分区 mmap / InitializePartition / Assets::GetAssetData 真实数据返回
 *       （assets.cc 依赖 esp_partition/spi_flash_mmap/Board/Application，host 不可
 *       编——INF-E2「设备版 OTA / NVS / 分区」行）。
 *     - 两枚 .bin 打入 assets 分区烧写面（build_default_assets.py 产出 assets.bin +
 *       烧录，device build 相位证据）。
 *   [真机轮专属]（FAR-E3 收口要件，本文件零 host 齿、不伪装）:
 *     - 探针页看屏照片 + 串口日志证明 16px/20px 字形来自运行时资产（消费端观测）。
 *     - 缺 assets 降级开机（移除/隔离 assets 后 14px 正常开机、不黑屏）。
 *     - 1bpp 真机字形可读性终审。
 *
 * ===========================================================================
 * 预基线修正（2026-10-28，human 双裁；红基线尚未成立，非 supersede，builder 未
 * 实现——本段随冻结件头注留痕）:
 *   裁定 1 · provenance 换 78 fork：asset-pipeline 改用 78 的 lv_font_conv fork
 *     （--format cbin，版本钉死登记）。设备可装载 .bin = cbin 格式（32-bit
 *     lv_font_t 内存镜像）。机判面 = FAS-MANIFEST 新键 gen_fonts_sh_format_cbin
 *     （--format cbin 产线 needle，TEST 1）+ TEST 2 raw cbin 形态正检（首 8 字节
 *     函数指针槽全零——序列化不填、装载器回填；legacy SPIFFS 'head' 格式字节 0
 *     = 0x30 长度前缀，被此检排除）。
 *   裁定 2 · host 半 = 宽度感知解码器：cbin_font_create 按 32-bit 布局反序列化
 *     （dsc 槽 @24），64-bit host 的 lv_font_t dsc@40——直调必崩（builder step-0
 *     红基线 TEST 2 段错误实测）。改由 kidpal_font 编译期 host 分支内落指针宽度
 *     感知解码器（把 32-bit cbin 镜像反序列化为 host 64-bit lv_font_fmt_txt_dsc_t
 *     / lv_font_t），host 行为级测试全部经 [K2] 走该解码器；设备面仍
 *     cbin_font_create（cbin_font.c 仍编入设备构建）。cbin_font.c 不再编入 host
 *     构建（FAS-MANIFEST host_srcs_cbin_font → host_srcs_no_cbin_font 负向接线
 *     事实，TEST 6）；解码器与 cbin_font_create 的等价性验证归真机轮（FAR-E3）。
 *     F-7 意图不变——同一数据源，不同宽度装载面。
 *   连带 · legacy 线束基座 tooth 删除：旧 font_body.bin = 旧 SPIFFS 通用格式
 *     （非 cbin），原「旧 bin 装载 infra guard」tooth 不能当 cbin 基座；且任意
 *     非 cbin 字节进解码器属崩溃域，host 不钉（与 FAS-A4 弱于契约处同理）。
 *     线束自证改由 [K2] 弱哨兵判别（fas_require_handle）+ raw cbin 形态正检承载。
 * ---------------------------------------------------------------------------
 *
 * ===========================================================================
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-28  initial freeze + 预基线修正（见上「预基线修正」段, human 双裁）。
 *   2026-10-29  P0-chain 修复批 additive（planner 派发; 授权 = 本轮 P0-chain 修复
 *               批 + architect advisory 2026-10-29; tighten-only, FAS-A1..A8 既有
 *               语义零改）: TEST 2 新增 tooth 1b/2b——fas_assert_glyph_sane 字形
 *               度量合理性断言（'8'/0x38、'中'/0x4E2D、body 另加 '猬'/0x732C;
 *               box ∈ [4..64]、adv ∈ [1..64] px, 公共 dsc 已像素尺度化）, 封堵
 *               「dsc 返回 true 即绿」假绿面。根因 = 78 fork cbin writer
 *               （lv_table_glyf.js toCBin）恒产 LV_FONT_FMT_TXT_LARGE=1 16 字节
 *               glyph dsc 布局 vs host 构建 LARGE=0（host_app sdkconfig :665 not
 *               set）→ 误读垃圾（冻结时点 scratch 探针实测 '8' box=71x0 / '中'
 *               box=0x1, line_height=23 正常; device sdkconfig.defaults:57
 *               LARGE=y 在案 = host 单侧错配, 设备真机消费验证归 FAR-E3）。
 *               修复归属 = P0 fixer 轮（仅 host 开 CONFIG_LV_FONT_FMT_TXT_LARGE;
 *               font_puhui_14_1 指定初始化器自动重排无需重产; A11 独立参考解码器
 *               按字段名取值不受布局步长影响, tooth 1-4 原样绿——architect
 *               advisory 结论）。预期红判读: 修复前现跑 tooth 1b 首断言即红
 *               （红名 FAS-A2 RED(P0-chain): ... box=71x0 ...）, tooth 2b 同 CASE
 *               被 abort 屏蔽解析性必红; 修复后两 tooth 转绿。红基线留痕 =
 *               261028-m6-k2a-p1-home-16px acceptance-author 交付报告 §C（250
 *               Tests / 4 Failures 实测, 红名逐字在案）。
 * ---------------------------------------------------------------------------
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；强定义 =
 * firmware/main/display/kidpal_font.{h,cc}——changeManifest 在册。本文件 weak
 * 哨兵在实现接线进 host runner 后被强定义覆盖，多重 weak 合法（p1_cjk_name
 * [S1] 先例）。签名以本节为准逐字落 kidpal_font.h）:
 *
 *   [K1] 资产数据源缝（F-7 同源的关键铰链；设备生产强符号 = Assets::GetAssetData
 *        的薄包装，host 测试注入 fonts_root 直读 reader）:
 *
 *     typedef int (*kidpal_font_asset_reader_t)(const char *name,
 *                                               const uint8_t **data, size_t *size);
 *       语义与 Assets::GetAssetData 对齐：返回 0 = 命中（*data 指向裸 .bin 文件
 *       内容 = 打包容器 'ZZ' 前缀之后的 payload，*size = 字节数，缓冲存活期须覆盖
 *       句柄使用期）；非 0 = 缺失（不消费 data/size）。
 *
 *   [K2] cbin 装载核心缝（数据 -> 统一句柄 + 14px 兜底；F-5 host 可验半；
 *        预基线修正裁定 2 · host/设备双面同签名，装载面按目标分派）:
 *
 *     const lv_font_t *kidpal_font_from_cbin(const uint8_t *data, size_t size);
 *       data==NULL 或 size 过小（< 32，最小完整性守卫）-> 返回编译内
 *       font_puhui_14_1（缺文件/装载失败兜底，绝不返回 NULL、绝不崩）。
 *       合法 cbin 数据 -> 装载成功句柄。装载面分派：
 *         设备 = cbin_font_create（32-bit；cbin_font.c 编入设备构建——与设备
 *                LvglCBinFont 同一装载面）；
 *         host  = 编译期 host 分支内指针宽度感知解码器（把 32-bit cbin 镜像
 *                反序列化为 host 64-bit lv_font_fmt_txt_dsc_t / lv_font_t；
 *                host 构建不含 cbin_font.c——直调必崩，见预基线修正裁定 2）。
 *       解码器与 cbin_font_create 的等价性验证归真机轮（FAR-E3）。
 *       句柄存活期 = data 缓冲存活期（设备 = assets mmap 常驻）。
 *       深腐蚀数据的崩溃域行为不做 host 钉点（见强度申报 FAS-A4）。
 *
 *   [K3] 资产挂载缝（开机 mount；设备侧在 Assets::Apply 后调，host 测试注入）:
 *
 *     int kidpal_font_mount_assets(kidpal_font_asset_reader_t reader);
 *       对两枚冻结名各经 reader 取数并经 [K2] 装载；reader==NULL 等价「全部缺失」
 *       ——返回 0 且全部句柄落 font_puhui_14_1（mount 完成态，缺资产是合法完成态，
 *       不黑屏语义的 host 可验半）。
 *
 *   [K4] 页面消费句柄缝（统一 kidpal_font 句柄，P1/P2/P3 唯一消费口；不得各自
 *        另造加载或降级分支——crossPackageBehaviors 语义）:
 *
 *     const lv_font_t *kidpal_font_body(void);    -- 16px 正文句柄; 缺资产 -> 14px
 *     const lv_font_t *kidpal_font_title(void);   -- 20px 标题句柄; 缺资产 -> 14px
 *
 *       恒非 NULL。20px 缺字回退 16px（F-6）经 lv_font_t.fallback 链承载：
 *       title()->fallback == body()（LVGL label 渲染自动走 fallback——机制钉，
 *       理由见 FAS-A5 强度申报）。
 *
 *   [K5] 冻结资产名（[S5] 先例：文件名为本 seam 冻结面，builder 改名 = 契约协商
 *        走 planner，不静默改名）:
 *         font_body_16.bin    16px 正文全量 CJK 1bpp cbin（gen_fonts.sh 产——78
 *                             fork --format cbin，裁定 1；fonts_root）
 *         font_title_20.bin   20px 阅读标题全量 CJK 1bpp cbin（同产线，不做静态子集）
 *       旧三枚 fonts_root 资产（font_body.bin / font_title_zcool.bin /
 *       font_num_baloo.bin）零触（ADR-017 ④：不被追述为已完成生产接线）。
 *
 * ===========================================================================
 * 契约条目 ↔ 断言的强度申报（acceptance-evidence.md 体例；消费方 = integrator
 * 与判官按本头注核）:
 *
 *   FAS-A1（FAR-E1/F-5 产线扩产与资产落盘）
 *     -> TEST 1 承载：构建清单档（configure 期事实：gen_fonts.sh 扩产行在世 +
 *        --format cbin provenance（裁定 1）+ 设备 CMakeLists 接 fonts_root）+
 *        产物面（两枚冻结名 .bin 落盘）。行为本体由 TEST 2 兑现，两档并列互证。
 *   FAS-A2（FAR-E1/F-6/F-7 两枚 .bin 真 cbin 装载 + 1bpp + 尺寸档 + 全量覆盖）
 *     -> TEST 2 承载：行为级（真实 cbin 字节经 [K2] host 宽度感知解码器装载 +
 *        fdsc bpp==1 + line_height 档 + cmap 覆盖码点全量下界 + GB2312 二级
 *        canary 码点）+ raw cbin 形态正检（首 8 字节函数指针槽全零——cbin 镜像
 *        假设显式化：78 fork lv_font_conv --format cbin 序列化不填函数指针、由
 *        装载器回填（cbin_font_create :27-28）；legacy SPIFFS 'head' 格式被此检
 *        排除；fork 输出形态若不符 = fixture 对齐协商走 planner，不静默）。
 *        标定假设显式化：line_height 档按 Noto Sans SC 实测族（14px->20 / 16px->23 /
 *        20px->28，font_noto_basic_{14,16,20}_*.c 在案）钉 16px∈[20..25] /
 *        20px∈[26..36]；builder 若换源 TTF 致越档 = fixture 对齐协商（走 planner），
 *        不静默放宽。覆盖下界依据（cmap 覆盖码点总和，LVGL 9 无 glyph_cnt 字段）：GB2312 一级 3755 字，现役 30px 静态子集
 *        标题（~100 字）远低于 3000——「不做静态子集」的源无关判别面。
 *        **2026-10-29 P0-chain 修复批 additive（REVISION LOG 同日条）**：TEST 2
 *        tooth 1b/2b 字形度量合理性断言（fas_assert_glyph_sane——'8'/'中'/'猬'
 *        canary box/adv 合理域 [4..64]/[1..64]）封堵「dsc 返回 true 即绿」的假绿
 *        面（冻结时点缺陷在世实测：垃圾度量互等曾使既有齿假绿）；授权 = 本轮
 *        P0-chain 修复批，tighten-only、FAS-A1..A8 既有语义零改。
 *   FAS-A3（FAR-E2/F-7 统一句柄 mount 正常态）
 *     -> TEST 3 承载：行为级（fonts_root 直读 reader 注入 -> body/title 真资产句柄、
 *        非 14px、尺寸档正确、glyph 可查）。
 *   FAS-A4（FAR-E2/F-5 缺 assets/缺文件/装载失败回退 14px 不黑屏）
 *     -> TEST 4 承载：行为级（缺 assets mount -> body/title 恒等 font_puhui_14_1 且
 *        非 NULL；from_cbin(NULL)/短缓冲 -> 14px，装载面经 [K2] 按目标分派）。
 *        弱于契约处显式声明：深腐蚀数据
 *        的崩溃域行为不做 host 钉点（装载面对任意垃圾数据可能野指针——设备
 *        cbin_font_create / host 解码器同理，host 只钉最小完整性守卫 size<32
 *        路径）——「装载失败不黑屏」的强证据归
 *        真机缺 assets 降级开机轮（FAR-E3），host 半为守卫级。
 *   FAS-A5（F-6/OQ1 20px 缺字回退 16px）
 *     -> TEST 5 承载：行为级（自然探针码点 16px 在场 / 20px 缺场 -> title 句柄
 *        glyph 查询命中且盒度量 == body 同码点）+ 机制级（title()->fallback ==
 *        body()——机制钉理由：P3 页面经 LVGL label 消费 title 句柄，缺字回退必须
 *        经 fallback 链才能在 label 渲染生效，显式 getter 不服务 label 通路）。
 *        fixture 逃生门显式化：探针码点集若被 title 全覆盖（title ⊇ 探针集）=
 *        infra 红自诊断（fixture 换更稀有码点或与 planner 协商），不静默。
 *   FAS-A6（FAR-E2/F-7 host 同源 + 设备接线）
 *     -> TEST 6 承载：构建清单档（configure 期事实：kidpal_font.{h,cc} 存在且引用
 *        冻结名 + Assets::GetAssetData + LvglCBinFont/cbin_font_create +
 *        font_puhui_14_1（设备面）；host 构建不含 cbin_font.c——负向事实
 *        host_srcs_no_cbin_font，裁定 2；host SRCS 注册本验收件；gen_fonts.sh
 *        输出面 fonts_root）。行为半 = TEST 2/3 的 fonts_root 同路径真读
 *        （host 与设备同枚 .bin 的 host 侧兑现，经 [K2] 解码器装载）。
 *   FAS-A7（OQ3/FAR-E3 不变量：实验页与测试字体维持编入 + 14px 兜底不删）
 *     -> TEST 7 承载：构建清单档（configure 期事实：font_test_10..32 +
 *        font_size_test_page.cc 在世且编入设备构建；font_puhui_14_1 编入 host
 *        runner + 渲染节点引用在世）。
 *   FAS-A8（相邻冻结锚：p1_cjk_name [S1] seam 契约面 + schedule/story 常量零触）
 *     -> TEST 8 承载：构建清单档（canary 级——configure 期事实：P0 不预批 P1
 *        supersede，不做全局 14px 替换；p1_cjk_name [S1] 签名面在世 +
 *        kidpal_render_node 强符号在世；schedule_page.h KSG_COURSE_CAP 84 /
 *        KSG_FONT_MIN_PX 14 与 story_page.h 行数常量零触）。
 *   非 host 面（无齿，闸证据条目）：FAR-E1 ADR-017 精确字节数/余量 append-only
 *     回填（docs-sync，integrator）；FAR-E3 真机探针 + 缺 assets 降级开机
 *     （device-round gate）；FAR-E4 device build 绿（firmware-build gate）。
 *
 * ===========================================================================
 * Coverage map（acceptanceRef -> test）:
 *   coverage: FAS-A1 -- gen_fonts.sh 扩产 16px/20px 行 + --format cbin provenance（清单档）+ 冻结名两枚 .bin 落 fonts_root（产物面）+ 设备 CMakeLists 接 fonts_root（清单档）-> TEST 1
 *   coverage: FAS-A2 -- 两枚 .bin raw cbin 形态正检 + 经 [K2] 真装载 + 1bpp + 尺寸档 + cmap 覆盖码点全量下界 + GB2312 二级 canary -> TEST 2
 *   coverage: FAS-A3 -- 统一句柄 mount 正常态（fonts_root reader 注入 -> 16px/20px 真资产句柄）-> TEST 3
 *   coverage: FAS-A4 -- 缺 assets/缺文件/短缓冲回退 font_puhui_14_1 恒非 NULL -> TEST 4
 *   coverage: FAS-A5 -- 20px 缺字回退 16px（自然探针 + fallback 机制钉）-> TEST 5
 *   coverage: FAS-A6 -- kidpal_font 设备接线 + host 不编 cbin_font.c（负向，裁定 2）+ 验收件注册（构建清单档）-> TEST 6
 *   coverage: FAS-A7 -- OQ3 实验页/测试字体维持编入 + 14px 兜底不删（构建清单档 canary）-> TEST 7
 *   coverage: FAS-A8 -- p1_cjk_name [S1] 契约面 + schedule/story 常量零触（构建清单档 canary）-> TEST 8
 *
 * ===========================================================================
 * BUILDER DUTY 清单（HOST_TEST_CONVENTIONS.md [C4.2] 约定）:
 *   1) tools/asset-pipeline/fonts/gen_fonts.sh：产线切 78 的 lv_font_conv fork
 *      （--format cbin，版本钉死登记——裁定 1）；同产线扩产 16px 正文与 20px 阅读
 *      标题两枚全量 CJK 1bpp cbin .bin（[K5] 冻结名；20px 不做静态子集——cmap 覆盖
 *      码点下界由 TEST 2 机判）；产出落 firmware/main/assets/fonts_root/。
 *   2) firmware/main/assets/fonts_root/：两枚 .bin 落盘（运行 gen_fonts.sh；
 *      ADR-017 精确字节数 append-only 回填归 integrator docs-sync）。
 *   3) firmware/main/CMakeLists.txt（设备）：fonts_root 接入 assets 打包
 *      （DEFAULT_ASSETS_EXTRA_FILES 链）——两枚 .bin 打入 assets 分区。
 *   4) firmware/main/display/kidpal_font.{h,cc}：[K1]-[K4] seam 强定义（设备数据源
 *      = Assets::GetAssetData 薄包装；装载核心按目标分派——设备 = LvglCBinFont/
 *      cbin_font_create 同面，host = 编译期宽度感知解码器（裁定 2）；降级 =
 *      font_puhui_14_1；title fallback 链 = body）。头文件签名与本文件
 *      seam contract 节逐字一致。
 *   5) firmware/main/test/host_app/main/CMakeLists.txt：按
 *      acceptance-wiring-patch.md 增量重落——注册本验收件；摘除 cbin_font.c 编入、
 *      其 malloc force-include 块与 78 include dir 注入（裁定 2：cbin_font.c 不进
 *      host 构建）；FAS 结构清单块随增量更新（host_srcs_no_cbin_font 负向键 +
 *      gen_fonts_sh_format_cbin provenance 键）。
 *   6) mutation 自证（mutationRequired=true）：变异不得保留被测符号原字面为子串
 *      （KR-A3 教训——用整词替换类变异）；每条 FAS 断言至少一条变异转红，
 *      counterEvidence 红名以本冻结版红名为准。
 *   7) device build 闸 + 真机轮（FAR-E3/E4）：xiaozhi.bin 构建过 + assets 打包含两枚
 *      .bin；真机探针页看屏照片 + 串口日志 + 缺 assets 降级开机（不黑屏）；
 *      host 解码器与 cbin_font_create 等价性也在真机轮一并复核（裁定 2）。
 *
 * ===========================================================================
 * Run protocol（HOST_TEST_CONVENTIONS.md [C4.4]）:
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf
 *      （或 cd firmware/main/test/host_app 后 ./build/host_infra_test.elf——本文件
 *      root 自手持 cwd 定位，两处均可；定位仅服务产物/清单路径解析，不扫源文件）
 *   RED baseline 判读（本冻结时点, builder 未实现）: TEST 1-6 = 设计态红（结构清单
 *      事实 0 含 format_cbin/no_cbin_font 负向键 + 资产未产 + seam 未实现 = 解析性
 *      红，weak 哨兵在世）；TEST 7/8 = canary 恒绿（现状即真，证明清单面在世）。
 *      builder 落 BUILDER DUTY 后 TEST 1-6 转绿；TEST 7/8 恒绿。
 *      2026-10-29 P0-chain 追记（acceptance-author 红基线现跑留痕; 250 Tests /
 *      4 Failures 实测）: 本包交付态全绿（TEST 1-8 落地后基线 = 250/3 auxbar 存量）。
 *      **新增在册红（升级项, 非 P0 fixer 轮前可修面）**: TEST 2 tooth 1b/2b =
 *      FAS-A2 RED(P0-chain)——78 fork cbin writer 恒产 LV_FONT_FMT_TXT_LARGE=1
 *      16 字节 glyph dsc 布局 vs host 构建 LARGE=0（host_app sdkconfig :665 not
 *      set; 设备 sdkconfig.defaults:57 LARGE=y 在案 = host 单侧错配, architect
 *      advisory 2026-10-29）→ 字形度量垃圾（scratch 探针实测 '8' box=71x0 /
 *      '中' box=0x1, line_height=23 正常）。修复 = 仅 host 开
 *      CONFIG_LV_FONT_FMT_TXT_LARGE（host_app/sdkconfig.defaults + 完整重编；
 *      font_puhui_14_1 指定初始化器自动重排无需重产；A11 独立参考解码器按字段
 *      名取值不受布局步长影响, tooth 1-4 原样绿）——归属 P0 fixer 轮。修复后
 *      tooth 1b/2b 转绿。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

#include "lvgl.h"

/* 编译内 14px 兜底（font_puhui_14_1.c 已由 p1_cjk_name A11 轮编入 host runner，
 * 强定义在世——直接引用，weak 探针不需要）。 */
extern const lv_font_t font_puhui_14_1;

/* -------------------------------------------------------------------------- *
 * [K5] 冻结资产名与[K1]-[K4] seam 弱哨兵（强定义 = kidpal_font.{h,cc}，BUILDER
 * DUTY 4)/5)；-2 = 「frozen seam not implemented」解析性红标记（绝不红在链接错）。
 * ------------------------------------------------------------------------ */
#define FAS_UNIMPLEMENTED (-2)

#define FAS_ASSET_BODY_16  "font_body_16.bin"
#define FAS_ASSET_TITLE_20 "font_title_20.bin"
/* font_body.bin（legacy）宏已随预基线修正 2026-10-28 删除：legacy bin = 旧 SPIFFS
 * 通用格式非 cbin，不再作线束基座（TEST 2 留痕）；资产本体零触（ADR-017 ④）。 */
#define FAS_FONTS_ROOT     "firmware/main/assets/fonts_root"
/* FAS-MANIFEST 结构清单（构建产物；configure 期由 host CMakeLists 生成——机制见
 * 头注 FAS-MANIFEST 节。repo 根相对路径：ELF 只认相对路径，运行时不扫源码树）。 */
#define FAS_MANIFEST       "firmware/main/test/host_app/build/fas_structure_manifest.txt"

typedef int (*kidpal_font_asset_reader_t)(const char *name,
                                          const uint8_t **data, size_t *size);

__attribute__((weak)) const lv_font_t *kidpal_font_from_cbin(const uint8_t *data,
                                                             size_t size)
{
    (void)data; (void)size;
    return NULL; /* 弱默认 = seam 未实现（TEST 断言响设计态红） */
}

__attribute__((weak)) int kidpal_font_mount_assets(kidpal_font_asset_reader_t reader)
{
    (void)reader;
    return FAS_UNIMPLEMENTED;
}

__attribute__((weak)) const lv_font_t *kidpal_font_body(void)
{
    return NULL;
}

__attribute__((weak)) const lv_font_t *kidpal_font_title(void)
{
    return NULL;
}

/* -------------------------------------------------------------------------- *
 * Repo-root 定位（p1_cjk_name pcn_ 先例同款：root 自手持 cwd 定位，相对路径——
 * ESP-IDF linux target 的 ELF fopen 不认 /mnt/c 绝对路径，相对路径实测正常）。
 * ------------------------------------------------------------------------ */
#define FAS_MAX_PATH 1024
#define FAS_MAX_READ (8u * 1024u * 1024u)

static char fas_root[FAS_MAX_PATH];
static int fas_root_ready;

static int fas_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int fas_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[FAS_MAX_PATH];
    size_t i;

    if (fas_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (fas_is_regular(probe)) {
            snprintf(fas_root, sizeof(fas_root), "%s", cands[i]);
            fas_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int fas_rel(char *out, size_t cap, const char *rel)
{
    if (!fas_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", fas_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", fas_root, rel);
    return 1;
}

static int fas_exists(const char *rel)
{
    char p[FAS_MAX_PATH];
    return fas_rel(p, sizeof(p), rel) && fas_is_regular(p);
}

/* 读整文件（二进制，malloc 缓冲由调用方 free；NULL = 不可读）。 */
static uint8_t *fas_read_bin(const char *rel, size_t *out_len)
{
    char p[FAS_MAX_PATH];
    FILE *f;
    long sz;
    uint8_t *buf;

    if (out_len != NULL) {
        *out_len = 0;
    }
    if (!fas_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > FAS_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

/* 文本读（NUL 结尾；FAS-MANIFEST 构建产物清单消费用——非源文件扫描）。 */
static char *fas_read_text(const char *rel)
{
    size_t len;
    uint8_t *raw = fas_read_bin(rel, &len);
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

static int fas_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL);
    return strstr(hay, needle) != NULL;
}

#define FAS_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(fas_locate_root(), \
    "FAS RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

static char fas_fm_buf[1024];
static const char *fas_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *fas_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(fas_fm_buf, sizeof(fas_fm_buf), fmt, ap);
    va_end(ap);
    return fas_fm_buf;
}

/* seam 未实现解析性红（p1_cjk_name pcn_require_implemented 同款）。 */
static void fas_require_seam(int rc, const char *what)
{
    if (rc == FAS_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "FAS RED: frozen seam NOT implemented/wired — %s (builder must land "
                 "[K1]-[K4] in firmware/main/display/kidpal_font.{h,cc} per the "
                 "contract in font_assets_runtime.acceptance.c)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* -------------------------------------------------------------------------- *
 * FAS-MANIFEST 消费面：只读构建产物清单（非源文件扫描——human 定向裁定见头注）。
 * fas_mfact: 返回清单事实值 0/1；-1 = 清单缺失或键不在（infra 红语义，TEST 7
 * tooth 3 钉负向探针）。行锚定匹配（key 须在行首、后跟 '='），防子串误配。
 * ------------------------------------------------------------------------ */
static int fas_mfact(const char *key)
{
    static char cache[8 * 1024];
    static char val[32];
    static int loaded;
    const char *line;
    size_t klen = strlen(key);

    if (!loaded) {
        char *txt = fas_read_text(FAS_MANIFEST);
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

/* 结构事实断言：fact==0 = 结构红（builder 责任面）；fact==-1 = infra 红
 * （清单缺失/键不在——自诊断，不冒充结构红）。 */
static void fas_require_fact(int fact, const char *key, const char *what)
{
    if (fact == -1) {
        TEST_ASSERT_TRUE_MESSAGE(0, fas_fm(
            "FAS RED(infra): 结构清单键 %s 缺失或清单未生成——重跑 host build 让 "
            "configure 期重采集（清单 = 构建产物 fas_structure_manifest.txt；"
            "运行时禁扫源码树，机制见冻结件头注 FAS-MANIFEST）", key));
        return;
    }
    TEST_ASSERT_TRUE_MESSAGE(fact == 1, fas_fm(
        "FAS RED: 结构事实 %s 不成立——%s（事实由 host CMakeLists configure 期采集，"
        "机制见冻结件头注 FAS-MANIFEST）", key, what));
}

static void fas_require_handle(const void *h, const char *what)
{
    if (h == NULL) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "FAS RED: frozen seam NOT implemented/wired or returned NULL — %s "
                 "(weak 哨兵在世 = 设计态红; 强定义接线后此红必须消失)",
                 what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* -------------------------------------------------------------------------- *
 * [K1] host 测试 reader：fonts_root 直读（F-7 同源——与设备 Assets::GetAssetData
 * 消费同一枚打包源文件）。每次命中分配持久缓冲（泄漏有意：句柄存活期 = 数据存活期，
 * 测试进程内有界 ≤ ~4MB）。缺失返回非 0。
 * ------------------------------------------------------------------------ */
static int fas_host_reader(const char *name, const uint8_t **data, size_t *size)
{
    char rel[FAS_MAX_PATH];
    uint8_t *buf;
    size_t len;

    snprintf(rel, sizeof(rel), "%s/%s", FAS_FONTS_ROOT, name);
    buf = fas_read_bin(rel, &len);
    if (buf == NULL) {
        return -1;
    }
    *data = buf;
    *size = len;
    return 0;
}

/* 全缺 reader（FAS-A4 降级夹具）。 */
static int fas_missing_reader(const char *name, const uint8_t **data, size_t *size)
{
    (void)name; (void)data; (void)size;
    return -1;
}

/* -------------------------------------------------------------------------- *
 * [K2] 装载共用探针：真实 cbin 文件字节 -> raw 形态正检 -> [K2] 装载（host =
 * kidpal_font 编译期宽度感知解码器——裁定 2；不直调 cbin_font_create，32-bit
 * 假设装载器在 64-bit host 直调必崩）。返回句柄；断言失败不返回。
 * 持久分配有意（句柄存活期 = 数据存活期，测试进程内有界 ≤ ~4MB）。
 * ------------------------------------------------------------------------ */
static const lv_font_t *fas_load_cbin_file(const char *asset_rel)
{
    uint8_t *buf;
    size_t len;
    const lv_font_t *font;
    size_t i;

    buf = fas_read_bin(asset_rel, &len);
    TEST_ASSERT_TRUE_MESSAGE(buf != NULL,
        fas_fm("FAS RED(infra): %s 无法读取——文件存在性由调用方先行断言", asset_rel));
    /* raw cbin 形态正检（裁定 1 机判面）：32-bit lv_font_t 镜像的函数指针槽在
     * 序列化时不填、由装载器回填（cbin_font_create 装载时赋
     * lv_font_get_glyph_dsc_fmt_txt / lv_font_get_bitmap_fmt_txt），故首 8 字节
     * 全零；legacy SPIFFS 'head' 格式字节 0 = 0x30（长度前缀），被此检排除。
     * 形态假设显式化见头注 FAS-A2：fork 输出若不符 = fixture 对齐协商，不静默。 */
    TEST_ASSERT_TRUE_MESSAGE(len >= 32,
        fas_fm("FAS-A2 RED: %s 字节数 %u < 32——cbin 最小完整性下界未达（[K2] 守卫同界）",
               asset_rel, (unsigned)len));
    for (i = 0; i < 8; i++) {
        TEST_ASSERT_TRUE_MESSAGE(buf[i] == 0,
            fas_fm("FAS-A2 RED: %s 非 cbin 镜像形态——首 8 字节函数指针槽第 %u 字节"
                   "= 0x%02X 非零（78 fork --format cbin 序列化约定；误产旧格式？"
                   "裁定 1 provenance 检查）", asset_rel, (unsigned)i, buf[i]));
    }
    font = kidpal_font_from_cbin(buf, len);
    fas_require_handle(font, "kidpal_font_from_cbin");
    return font;
}

static const lv_font_fmt_txt_dsc_t *fas_fdsc(const lv_font_t *font)
{
    return (const lv_font_fmt_txt_dsc_t *)font->dsc;
}

static int fas_has_glyph(const lv_font_t *font, uint32_t cp)
{
    lv_font_glyph_dsc_t g;
    return lv_font_get_glyph_dsc(font, &g, cp, 0) ? 1 : 0;
}

/* cmap 覆盖码点总和（全量 vs 静态子集的源无关判别容量；见上方宏注）。 */
static uint32_t fas_cmap_coverage(const lv_font_t *font)
{
    const lv_font_fmt_txt_dsc_t *d = (const lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t total = 0;
    int i;

    for (i = 0; i < (int)d->cmap_num; i++) {
        const lv_font_fmt_txt_cmap_t *cm = &d->cmaps[i];
        if (cm->type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY ||
            cm->type == LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL) {
            total += cm->range_length;
        } else {
            total += cm->list_length;
        }
    }
    return total;
}

/* 16px 档（Noto 实测 23，标定假设见头注 FAS-A2）。 */
#define FAS_LH16_MIN 20
#define FAS_LH16_MAX 25
/* 20px 档（Noto 实测 28）。 */
#define FAS_LH20_MIN 26
#define FAS_LH20_MAX 36
/* 全量覆盖下界（cmap 覆盖码点总和；GB2312 一级 3755 字，静态子集 ~100 字远低于此）。
 * 注：LVGL 9 lv_font_fmt_txt_dsc_t 无 glyph_cnt 字段，覆盖容量经 cmap 表求和
 * （FORMAT0 型 = range_length，稀疏型 = list_length）——字符表覆盖容量代理，
 * 行为级假设显式化见头注 FAS-A2。 */
#define FAS_COV_BODY16_MIN 6000
#define FAS_COV_TITLE20_MIN 3000

/* GB2312 二级 canary（gen_fonts.sh 字表自检原三字——16px 全量硬 canary）。 */
static const uint32_t fas_cjk_canary[] = {
    0x732Cu, /* 猬 */ 0x72F8u, /* 狸 */ 0x732Du, /* 猾 */
};
/* 常用字 canary（两枚都应有）。 */
static const uint32_t fas_cjk_common[] = {
    0x94A2u, /* 钢 */ 0x7434u, /* 琴 */ 0x8BFEu, /* 课 */ 0x4E2Du, /* 中 */
};

/* 2026-10-29 P0-chain 修复批 additive（REVISION LOG 同日条；授权 = 本轮 P0-chain
 * 修复批，architect advisory 裁定 host 单侧 LARGE 错配归属 P0 fixer 轮）：
 * 字形度量合理性区间——cbin 装载后 glyph dsc 必须给出非垃圾盒度量（16px/20px
 * CJK box 4..64、ASCII 数字/冒号 box 4..64、adv 1..64 px；缺 assets 装载的
 * 编译内 14px 字体同域）。冻结时点缺陷在世实测红值：'8' box=71x0 adv=0（71>64）、
 * '中' box=0x1（0<4）——LARGE=1 16 字节 glyph dsc 表被 host LARGE=0 构建按 8 字节
 * 误读的错位读数；修复（host 开 CONFIG_LV_FONT_FMT_TXT_LARGE）后转绿。 */
#define FAS_GLYPH_BOX_MIN 4
#define FAS_GLYPH_BOX_MAX 64
#define FAS_GLYPH_ADV_MIN 1
#define FAS_GLYPH_ADV_MAX 64

static void fas_assert_glyph_sane(const lv_font_t *font, uint32_t cp,
                                  const char *what)
{
    lv_font_glyph_dsc_t g;
    int ok = lv_font_get_glyph_dsc(font, &g, cp, 0);

    TEST_ASSERT_TRUE_MESSAGE(ok,
        fas_fm("FAS-A2 RED(P0-chain): %s 码点 U+%04X glyph dsc 查询失败——cbin "
               "glyph dsc 表不可读（host LV_FONT_FMT_TXT_LARGE=0 vs 78 fork cbin "
               "writer 恒产 LARGE=1 16 字节布局错配; 修复批归属 P0 fixer 轮）",
               what, cp));
    TEST_ASSERT_TRUE_MESSAGE(g.box_w >= FAS_GLYPH_BOX_MIN &&
                                 g.box_w <= FAS_GLYPH_BOX_MAX &&
                                 g.box_h >= FAS_GLYPH_BOX_MIN &&
                                 g.box_h <= FAS_GLYPH_BOX_MAX &&
                                 g.adv_w >= FAS_GLYPH_ADV_MIN &&
                                 g.adv_w <= FAS_GLYPH_ADV_MAX,
        fas_fm("FAS-A2 RED(P0-chain): %s 码点 U+%04X 字形度量垃圾（box=%dx%d "
               "adv=%d 越出合理域 [%d..%d]/[%d..%d]）——glyph dsc 布局与消费侧 "
               "LVGL LV_FONT_FMT_TXT_LARGE 构建旗不匹配（host 错配已证实: host "
               "sdkconfig not set vs 78 fork writer 恒产 LARGE=1; 设备 "
               "sdkconfig.defaults:57 LARGE=y 在案、真机消费验证归 FAR-E3）。"
               "修复批归属 P0 fixer 轮，非本包设计态", what, cp, (int)g.box_w,
               (int)g.box_h, (int)g.adv_w, FAS_GLYPH_BOX_MIN, FAS_GLYPH_BOX_MAX,
               FAS_GLYPH_ADV_MIN, FAS_GLYPH_ADV_MAX));
}

static void fas_assert_full_font(const lv_font_t *font, int lh_min, int lh_max,
                                 uint32_t cov_min, const char *what)
{
    size_t i;
    TEST_ASSERT_TRUE_MESSAGE(font->line_height >= lh_min && font->line_height <= lh_max,
        fas_fm("FAS-A2 RED: %s line_height=%d 越出 [%d..%d] 档——尺寸档漂移"
               "（标定假设见冻结件头注 FAS-A2；换源 TTF 须走 planner 对齐）",
               what, (int)font->line_height, lh_min, lh_max));
    TEST_ASSERT_TRUE_MESSAGE(fas_fdsc(font)->bpp == 1,
        fas_fm("FAS-A2 RED: %s bpp=%d != 1——1bpp 义务被破（FAR-E1）", what,
               (int)fas_fdsc(font)->bpp));
    TEST_ASSERT_TRUE_MESSAGE(fas_fdsc(font)->bitmap_format == LV_FONT_FMT_TXT_PLAIN,
        fas_fm("FAS-A2 RED: %s bitmap_format=%d != PLAIN——1bpp 无压缩口径漂移", what,
               (int)fas_fdsc(font)->bitmap_format));
    TEST_ASSERT_TRUE_MESSAGE(fas_cmap_coverage(font) >= cov_min,
        fas_fm("FAS-A2 RED: %s cmap 覆盖码点 %u < %u——静态子集混入（F-6/OQ1 全量 "
               "CJK 义务；现役 30px 标题子集 ~100 字远低于此界）", what,
               (unsigned)fas_cmap_coverage(font), (unsigned)cov_min));
    for (i = 0; i < sizeof(fas_cjk_common) / sizeof(fas_cjk_common[0]); i++) {
        TEST_ASSERT_TRUE_MESSAGE(fas_has_glyph(font, fas_cjk_common[i]),
            fas_fm("FAS-A2 RED: %s 缺常用字 U+%04X——字形覆盖缺口", what,
                   fas_cjk_common[i]));
    }
}

/* ========================================================================== *
 * TEST 1 — FAS-A1：产线扩产与资产落盘（构建清单档 + 产物面；行为本体 = TEST 2）
 *   tooth 1: gen_fonts.sh 同产线扩产（--size 16 / --size 20 产线行在世 + --bpp 1
 *            口径 + 输出面 fonts_root）——configure 期采集，构建清单消费。
 *   tooth 2: 两枚冻结名 .bin 落 fonts_root（[K5] 冻结名；产物面 fas_exists）。
 *   tooth 3: assets 打包接线——设备 CMakeLists 接 fonts_root（configure 期事实；
 *            烧写/分区面 = 设备专有，见三分申报）。
 *   RED（设计态）: 扩产行未落 / .bin 未产 / 打包接线未接（清单事实 0）。
 * ========================================================================== */
/* coverage: FAS-A1 -- 产线扩产行（清单档）+ 冻结名两枚 .bin 落盘（产物面）+ 设备 CMakeLists 接 fonts_root（清单档） */
TEST_CASE("font assets: 产线扩产与资产落盘 (FAS-A1)",
          "[font_assets][FAS-A1][structural]")
{
    char rel[FAS_MAX_PATH];

    FAS_REQUIRE_ROOT();

    /* tooth 1: gen_fonts.sh 扩产（产线 provenance；configure 期事实）。 */
    fas_require_fact(fas_mfact("gen_fonts_sh_exists"), "gen_fonts_sh_exists",
                     "gen_fonts.sh 丢失——产线面被拆");
    fas_require_fact(fas_mfact("gen_fonts_sh_size16"), "gen_fonts_sh_size16",
                     "gen_fonts.sh 无 --size 16 产线行——16px 正文扩产缺失"
                     "（BUILDER DUTY 1)）");
    fas_require_fact(fas_mfact("gen_fonts_sh_size20"), "gen_fonts_sh_size20",
                     "gen_fonts.sh 无 --size 20 产线行——20px 阅读标题扩产缺失"
                     "（BUILDER DUTY 1)）");
    fas_require_fact(fas_mfact("gen_fonts_sh_bpp1_ge2"), "gen_fonts_sh_bpp1_ge2",
                     "gen_fonts.sh --bpp 1 产线行不足 2——1bpp 口径缺失");
    fas_require_fact(fas_mfact("gen_fonts_sh_format_cbin"), "gen_fonts_sh_format_cbin",
                     "gen_fonts.sh 未切 78 fork --format cbin 产线——provenance 裁定 1 "
                     "未落（cbin 格式资产无从产出）");

    /* tooth 2: 两枚冻结名 .bin 落盘（[K5] 产物面；行为本体 = TEST 2 真装载）。 */
    snprintf(rel, sizeof(rel), "%s/" FAS_ASSET_BODY_16, FAS_FONTS_ROOT);
    TEST_ASSERT_TRUE_MESSAGE(fas_exists(rel),
        fas_fm("FAS-A1 RED: %s 未落盘——16px 正文资产未产出（BUILDER DUTY 1)/2)）",
               rel));
    snprintf(rel, sizeof(rel), "%s/" FAS_ASSET_TITLE_20, FAS_FONTS_ROOT);
    TEST_ASSERT_TRUE_MESSAGE(fas_exists(rel),
        fas_fm("FAS-A1 RED: %s 未落盘——20px 阅读标题资产未产出（BUILDER DUTY 1)/2)）",
               rel));

    /* tooth 3: assets 打包接线（configure 期事实；设备专有烧写面不装 host 齿）。 */
    fas_require_fact(fas_mfact("device_cmakelists_fonts_root"),
                     "device_cmakelists_fonts_root",
                     "设备 CMakeLists 未接 fonts_root——两枚 .bin 打入 assets 分区的"
                     "打包接线缺失（BUILDER DUTY 3)）");
}

/* ========================================================================== *
 * TEST 2 — FAS-A2：两枚 .bin 真 cbin 装载 + 1bpp + 尺寸档 + 全量覆盖（行为级）
 *   tooth 1: font_body_16.bin raw cbin 形态正检（首 8 字节函数指针槽全零 +
 *            size>=32）+ 经 [K2] host 宽度感知解码器真装载 + line_height 16 档 +
 *            bpp==1 PLAIN + cmap 覆盖 >= 6000 + GB2312 二级 canary（猬/狸/猾）+
 *            常用字。
 *   tooth 2: font_title_20.bin 同构（20 档 + cmap 覆盖 >= 3000）。
 *   tooth 3: 尺寸判别——title.line_height > body.line_height（20px 严格大于 16px）。
 *   tooth 1b/2b (2026-10-29 P0-chain 修复批 additive): 字形度量合理性——'8'/'中'
 *            （+'猬'）glyph dsc box/adv 落合理域（fas_assert_glyph_sane）；缺陷
 *            在世 = 红（P0-chain 红名），host 开 LARGE 修复后转绿（fixer 轮收口）。
 *   预基线修正留痕：原 tooth 0（旧 font_body.bin 经 cbin_font_create 装载作线束
 *            自证）删除——legacy bin = 旧 SPIFFS 通用格式非 cbin（首 4 字节
 *            0x30+'head'），不能当 cbin 基座；且任意非 cbin 字节进解码器属崩溃
 *            域，host 不钉（与 FAS-A4 弱于契约处同理）。线束自证改由 [K2] 弱哨
 *            兵判别（fas_require_handle）+ raw cbin 形态正检承载。
 *   RED（设计态）: .bin 未产（tooth 1/2 前置存在性红）；seam 未实现 = weak 哨兵红。
 * ========================================================================== */
/* coverage: FAS-A2 -- 两枚 .bin raw cbin 形态正检 + 经 [K2] 真装载 + 1bpp + 尺寸档 + cmap 覆盖码点全量下界 + canary 码点（行为级） */
TEST_CASE("font assets: 两枚新bin真cbin装载+1bpp+尺寸档+全量覆盖 (FAS-A2)",
          "[font_assets][FAS-A2][behavior]")
{
    const lv_font_t *body;
    const lv_font_t *title;
    size_t i;
    char rel[FAS_MAX_PATH];

    FAS_REQUIRE_ROOT();

    /* tooth 1: 16px 正文真装载 + 全量覆盖（raw 形态正检在 fas_load_cbin_file 内）。 */
    snprintf(rel, sizeof(rel), "%s/" FAS_ASSET_BODY_16, FAS_FONTS_ROOT);
    TEST_ASSERT_TRUE_MESSAGE(fas_exists(rel),
        fas_fm("FAS-A2 RED: %s 未落盘——16px 资产未产出（BUILDER DUTY 1)/2)，"
               "TEST 1 结构半同源红）", rel));
    body = fas_load_cbin_file(rel);
    fas_assert_full_font(body, FAS_LH16_MIN, FAS_LH16_MAX,
                         FAS_COV_BODY16_MIN, FAS_ASSET_BODY_16);
    for (i = 0; i < sizeof(fas_cjk_canary) / sizeof(fas_cjk_canary[0]); i++) {
        TEST_ASSERT_TRUE_MESSAGE(fas_has_glyph(body, fas_cjk_canary[i]),
            fas_fm("FAS-A2 RED: %s 缺 GB2312 二级 canary U+%04X——静态子集或字表"
                   "裁剪混入（F-6 全量义务；gen_fonts.sh 字表自检三字）",
                   FAS_ASSET_BODY_16, fas_cjk_canary[i]));
    }

    /* tooth 1b (2026-10-29 P0-chain 修复批 additive, REVISION LOG 同日条): 字形
     * 度量合理性——cbin glyph dsc 布局与消费侧 LV_FONT_FMT_TXT_LARGE 构建旗
     * 匹配性（ASCII '8' = 冻结时点实测垃圾读数首个码点 box=71x0 / CJK '中'
     * box=0x1 / GB2312 二级 '猬'）。缺陷在世 = 红（红名 P16 同源指认 P0-chain），
     * host 开 LARGE 修复后转绿（fixer 轮收口）。 */
    fas_assert_glyph_sane(body, 0x0038u /* '8' */, FAS_ASSET_BODY_16);
    fas_assert_glyph_sane(body, 0x4E2Du /* 中 */, FAS_ASSET_BODY_16);
    fas_assert_glyph_sane(body, 0x732Cu /* 猬 */, FAS_ASSET_BODY_16);

    /* tooth 2: 20px 阅读标题真装载 + 全量覆盖。 */
    snprintf(rel, sizeof(rel), "%s/" FAS_ASSET_TITLE_20, FAS_FONTS_ROOT);
    TEST_ASSERT_TRUE_MESSAGE(fas_exists(rel),
        fas_fm("FAS-A2 RED: %s 未落盘——20px 资产未产出（BUILDER DUTY 1)/2)）", rel));
    title = fas_load_cbin_file(rel);
    fas_assert_full_font(title, FAS_LH20_MIN, FAS_LH20_MAX,
                         FAS_COV_TITLE20_MIN, FAS_ASSET_TITLE_20);
    /* tooth 2b (additive 同 tooth 1b): title20 同款度量合理性。 */
    fas_assert_glyph_sane(title, 0x0038u /* '8' */, FAS_ASSET_TITLE_20);
    fas_assert_glyph_sane(title, 0x4E2Du /* 中 */, FAS_ASSET_TITLE_20);

    /* tooth 3: 尺寸判别（标定假设源无关的严格序）。 */
    TEST_ASSERT_TRUE_MESSAGE(title->line_height > body->line_height,
        fas_fm("FAS-A2 RED: title line_height(%d) <= body line_height(%d)——"
               "20px 标题档未大于 16px 正文档", (int)title->line_height,
               (int)body->line_height));
}

/* ========================================================================== *
 * TEST 3 — FAS-A3：统一句柄 mount 正常态（行为级；F-5/F-7 host 可验半）
 *   tooth 1: [K3] mount（fonts_root reader 注入）返回 0。
 *   tooth 2: body() = 真资产句柄（非 14px、16 档、glyph 可查）。
 *   tooth 3: title() = 真资产句柄（非 14px、20 档）。
 *   tooth 4: host reader 源路径含 fonts_root（同源路径自证——与设备 GetAssetData
 *            消费同一枚打包源文件的 host 兑现面）。
 *   RED（设计态）: seam 未实现（weak 哨兵）。
 * ========================================================================== */
/* coverage: FAS-A3 -- 统一句柄 mount 正常态：fonts_root reader 注入 -> 16px/20px 真资产句柄非 14px */
TEST_CASE("font runtime: 统一句柄mount正常态 (FAS-A3)",
          "[font_assets][FAS-A3][behavior]")
{
    const lv_font_t *body;
    const lv_font_t *title;
    int rc;

    FAS_REQUIRE_ROOT();

    /* tooth 1: mount 完成。 */
    rc = kidpal_font_mount_assets(fas_host_reader);
    fas_require_seam(rc, "kidpal_font_mount_assets");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        fas_fm("FAS-A3 RED: mount_assets(rc=%d) 非完成态——正常资产在库时 mount 不应"
               "失败", rc));

    /* tooth 2: body 真资产句柄。 */
    body = kidpal_font_body();
    fas_require_handle(body, "kidpal_font_body");
    TEST_ASSERT_TRUE_MESSAGE(body != &font_puhui_14_1,
        "FAS-A3 RED: body() == font_puhui_14_1——资产在库却落 14px 兜底（装载/取数"
        "面断裂，非降级设计态）");
    TEST_ASSERT_TRUE_MESSAGE(body->line_height >= FAS_LH16_MIN &&
                                 body->line_height <= FAS_LH16_MAX,
        fas_fm("FAS-A3 RED: body() line_height=%d 越出 16px 档 [%d..%d]",
               (int)body->line_height, FAS_LH16_MIN, FAS_LH16_MAX));
    TEST_ASSERT_TRUE_MESSAGE(fas_has_glyph(body, 0x4E2Du /* 中 */),
        "FAS-A3 RED: body() 查『中』缺字形——句柄不可消费（P1/P2/P3 消费口断裂）");

    /* tooth 3: title 真资产句柄。 */
    title = kidpal_font_title();
    fas_require_handle(title, "kidpal_font_title");
    TEST_ASSERT_TRUE_MESSAGE(title != &font_puhui_14_1,
        "FAS-A3 RED: title() == font_puhui_14_1——资产在库却落 14px 兜底");
    TEST_ASSERT_TRUE_MESSAGE(title->line_height >= FAS_LH20_MIN &&
                                 title->line_height <= FAS_LH20_MAX,
        fas_fm("FAS-A3 RED: title() line_height=%d 越出 20px 档 [%d..%d]",
               (int)title->line_height, FAS_LH20_MIN, FAS_LH20_MAX));

    /* tooth 4: 同源路径自证——reader 解析的源目录 = 设备打包源目录。 */
    TEST_ASSERT_TRUE_MESSAGE(fas_contains(FAS_FONTS_ROOT,
                                          "firmware/main/assets/fonts_root"),
        "FAS RED(infra): reader 源根漂移——host 不再读 fonts_root（F-7 同源面断裂)");
}

/* ========================================================================== *
 * TEST 4 — FAS-A4：缺 assets/缺文件/装载失败回退 14px 不黑屏（行为级 + 守卫级）
 *   tooth 1: mount(NULL reader 等价全缺) 返回 0（降级是合法完成态）。
 *   tooth 2: body()/title() 恒等 &font_puhui_14_1 且恒非 NULL（不黑屏 host 半）。
 *   tooth 3: from_cbin(NULL, 0) == &font_puhui_14_1（缺文件半）。
 *   tooth 4: from_cbin(短缓冲) == &font_puhui_14_1（最小完整性守卫——不崩）。
 *   弱于契约处（头注 FAS-A4）：深腐蚀数据崩溃域不做 host 钉点，强证据归真机
 *   缺 assets 降级开机轮（FAR-E3）。
 *   RED（设计态）: seam 未实现（weak 哨兵）。
 * ========================================================================== */
/* coverage: FAS-A4 -- 缺 assets/缺文件/短缓冲回退 font_puhui_14_1 恒非 NULL 不崩 */
TEST_CASE("font runtime: 缺资产回退14px不黑屏 (FAS-A4)",
          "[font_assets][FAS-A4][boundary]")
{
    const lv_font_t *body;
    const lv_font_t *title;
    const lv_font_t *h;
    static const uint8_t short_buf[8] = { 0 };
    int rc;

    FAS_REQUIRE_ROOT();

    /* tooth 1: 全缺 mount = 合法完成态。 */
    rc = kidpal_font_mount_assets(fas_missing_reader);
    fas_require_seam(rc, "kidpal_font_mount_assets(missing reader)");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        fas_fm("FAS-A4 RED: 缺 assets mount rc=%d != 0——降级被当成致命错（不黑屏"
               "义务的 mount 半被破）", rc));

    /* tooth 2: 双句柄恒等 14px 兜底。 */
    body = kidpal_font_body();
    fas_require_handle(body, "kidpal_font_body(缺资产)");
    title = kidpal_font_title();
    fas_require_handle(title, "kidpal_font_title(缺资产)");
    TEST_ASSERT_TRUE_MESSAGE(body == &font_puhui_14_1,
        "FAS-A4 RED: 缺 assets 时 body() != font_puhui_14_1——兜底语义被破（F-5）");
    TEST_ASSERT_TRUE_MESSAGE(title == &font_puhui_14_1,
        "FAS-A4 RED: 缺 assets 时 title() != font_puhui_14_1——兜底语义被破（F-5）");

    /* tooth 3: 缺文件半。 */
    h = kidpal_font_from_cbin(NULL, 0);
    fas_require_handle(h, "kidpal_font_from_cbin(NULL)");
    TEST_ASSERT_TRUE_MESSAGE(h == &font_puhui_14_1,
        "FAS-A4 RED: from_cbin(NULL) 未回退 14px——缺文件兜底语义被破");

    /* tooth 4: 短缓冲守卫（不崩 + 回退）。 */
    h = kidpal_font_from_cbin(short_buf, sizeof(short_buf));
    fas_require_handle(h, "kidpal_font_from_cbin(short)");
    TEST_ASSERT_TRUE_MESSAGE(h == &font_puhui_14_1,
        "FAS-A4 RED: from_cbin(短缓冲) 未回退 14px——最小完整性守卫缺失");
}

/* ========================================================================== *
 * TEST 5 — FAS-A5：20px 缺字回退 16px（F-6/OQ1；行为级 + 机制级）
 *   tooth 1 (probe 自检): 候选码点中存在「body 在场 / title 裸查缺场」的自然
 *            缺口（防 title 全覆盖时 fixture 假绿——infra 红自诊断）。
 *   tooth 2 (行为半): title 句柄 glyph 查询（走 LVGL fallback 派发）命中该码点，
 *            且盒度量 == body 同码点（回退真落到 16px，非垃圾/非缺字丢弃）。
 *   tooth 3 (机制半): title()->fallback == body()（LVGL label 消费通路的机制钉，
 *            理由见头注 FAS-A5）。
 *   RED（设计态）: seam 未实现（weak 哨兵）。
 * ========================================================================== */
/* coverage: FAS-A5 -- 20px 缺字回退 16px：自然探针 glyph 命中且盒度量==body + fallback 机制钉 */
TEST_CASE("font runtime: 20px缺字回退16px (FAS-A5)",
          "[font_assets][FAS-A5][behavior]")
{
    static const uint32_t cands[] = {
        0x732Cu, /* 猬 */ 0x72F8u, /* 狸 */ 0x732Du, /* 猾 */
        0x7434u, /* 琴 */ 0x94A2u, /* 钢 */ 0x821Eu, /* 舞 */
    };
    const lv_font_t *body;
    const lv_font_t *title;
    lv_font_glyph_dsc_t g_title, g_body;
    uint32_t probe_cp = 0;
    size_t i;
    int rc;

    FAS_REQUIRE_ROOT();

    rc = kidpal_font_mount_assets(fas_host_reader);
    fas_require_seam(rc, "kidpal_font_mount_assets");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        fas_fm("FAS-A5 RED: mount rc=%d——正常态不可达", rc));

    body = kidpal_font_body();
    fas_require_handle(body, "kidpal_font_body");
    title = kidpal_font_title();
    fas_require_handle(title, "kidpal_font_title");

    /* tooth 1 (probe 自检): 找自然缺口码点。 */
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        uint32_t cp = cands[i];
        if (fas_has_glyph(body, cp) &&
            !title->get_glyph_dsc(title, &g_title, cp, 0)) {
            probe_cp = cp;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(probe_cp != 0,
        "FAS RED(infra): title 裸查覆盖了全部探针码点——自然缺口缺场（title ⊇ "
        "探针集）。fixture 需换更稀有码点，或 title 全量源选择使 16px/20px 同源"
        "全覆盖——缺字回退语义无法自然触发，转 planner 协商注入面，不静默");

    /* tooth 2 (行为半): fallback 派发命中且 == body 度量。 */
    TEST_ASSERT_TRUE_MESSAGE(lv_font_get_glyph_dsc(title, &g_title, probe_cp, 0),
        fas_fm("FAS-A5 RED: title 句柄对 U+%04X（20px 缺场/16px 在场）glyph 查询"
               "未命中——缺字回退 16px 语义缺失（F-6；缺字被丢弃 = 动态标题断字）",
               probe_cp));
    TEST_ASSERT_TRUE_MESSAGE(lv_font_get_glyph_dsc(body, &g_body, probe_cp, 0),
        "FAS RED(infra): body 句柄同码点查询失败——probe 自检与实现不一致");
    TEST_ASSERT_TRUE_MESSAGE(g_title.box_w == g_body.box_w &&
                                 g_title.box_h == g_body.box_h &&
                                 g_title.adv_w == g_body.adv_w,
        fas_fm("FAS-A5 RED: U+%04X 回退字形度量 (box %dx%d adv %d) != body 度量 "
               "(box %dx%d adv %d)——回退未落 16px 句柄或度量被改写", probe_cp,
               (int)g_title.box_w, (int)g_title.box_h, (int)g_title.adv_w,
               (int)g_body.box_w, (int)g_body.box_h, (int)g_body.adv_w));

    /* tooth 3 (机制半): fallback 链钉。 */
    TEST_ASSERT_TRUE_MESSAGE(title->fallback == body,
        "FAS-A5 RED: title()->fallback != body()——缺字回退未经 LVGL fallback 链"
        "承载（P3 页面 label 消费通路无法自动回退，机制钉见冻结件头注 FAS-A5）");
}

/* ========================================================================== *
 * TEST 6 — FAS-A6：host 同源 + 设备接线结构半（构建清单档）
 *   tooth 1: kidpal_font.{h,cc} configure 期事实——在世且引用冻结名 /
 *            GetAssetData / LvglCBinFont 或 cbin_font_create / font_puhui_14_1
 *            （设备接线面）。
 *   tooth 2: host 接线负向事实——cbin_font.c 不进 host 构建（裁定 2）+ 本验收件
 *            注册（host 装载半 = [K2] 解码器，在 kidpal_font.cc 内）。
 *   tooth 3 (guard): gen_fonts.sh 输出面 fonts_root 在世（产线面）。
 *   RED（设计态）: kidpal_font.{h,cc} 未落（清单事实 0）。
 * ========================================================================== */
/* coverage: FAS-A6 -- kidpal_font 设备接线（冻结名+GetAssetData+cbin 装载面+14px 兜底）+ host cbin_font.c 编入接线（构建清单档） */
TEST_CASE("font runtime: host同源+设备接线结构半 (FAS-A6)",
          "[font_assets][FAS-A6][structural]")
{
    FAS_REQUIRE_ROOT();

    /* tooth 1: kidpal_font.{h,cc} 设备接线（configure 期事实）。 */
    fas_require_fact(fas_mfact("kidpal_font_h_exists"), "kidpal_font_h_exists",
                     "firmware/main/display/kidpal_font.h 未落——[K1]-[K4] seam "
                     "声明缺失（BUILDER DUTY 4)）");
    fas_require_fact(fas_mfact("kidpal_font_cc_exists"), "kidpal_font_cc_exists",
                     "firmware/main/display/kidpal_font.cc 未落——[K1]-[K4] seam "
                     "强定义缺失（BUILDER DUTY 4)）");
    fas_require_fact(fas_mfact("kidpal_font_cc_names_frozen"),
                     "kidpal_font_cc_names_frozen",
                     "kidpal_font.cc 未引用两枚冻结名——设备侧资产取数名与 host 同源"
                     "名脱钩（F-7 同一枚 .bin 的接线面断裂；[K5] 冻结名）");
    fas_require_fact(fas_mfact("kidpal_font_cc_getassetdata"),
                     "kidpal_font_cc_getassetdata",
                     "kidpal_font.cc 未接 Assets::GetAssetData——设备数据源缝"
                     "（[K1] 生产强符号）缺失");
    fas_require_fact(fas_mfact("kidpal_font_cc_cbin_loader"),
                     "kidpal_font_cc_cbin_loader",
                     "kidpal_font.cc 未接 cbin 装载面（LvglCBinFont/cbin_font_create）"
                     "——与设备统一装载面脱钩（F-7）");
    fas_require_fact(fas_mfact("kidpal_font_cc_fallback14"),
                     "kidpal_font_cc_fallback14",
                     "kidpal_font.cc 未引用 font_puhui_14_1——14px 兜底未落到统一"
                     "句柄 seam（F-5 降级义务）");

    /* tooth 2: host 接线（裁定 2 负向事实：cbin_font.c 不进 host 构建；本验收件注册）。 */
    fas_require_fact(fas_mfact("host_srcs_no_cbin_font"), "host_srcs_no_cbin_font",
                     "cbin_font.c 被编入 host 构建——32-bit 假设装载器在 64-bit host "
                     "直调必崩（裁定 2：host 装载半 = kidpal_font 宽度感知解码器，"
                     "cbin_font.c 仅设备构建）");
    fas_require_fact(fas_mfact("host_srcs_acceptance"), "host_srcs_acceptance",
                     "font_assets_runtime.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS"
                     "——host 验收构建未接本包");

    /* tooth 3 (guard): 产线输出面。 */
    fas_require_fact(fas_mfact("gen_fonts_sh_fonts_root_out"),
                     "gen_fonts_sh_fonts_root_out",
                     "gen_fonts.sh 输出面 fonts_root 丢失——产线被拆");
}

/* ========================================================================== *
 * TEST 7 — FAS-A7：不变量 canary（OQ3 + 14px 兜底不删；构建清单档，恒绿义务）
 *   tooth 1: 九档实验字体 font_test_10..32 全部在世且编入设备构建 +
 *            font_size_test_page.cc 在世且编入（OQ3 维持编入，不退役）。
 *   tooth 2: font_puhui_14_1 兜底不删——host runner 编入在世 + 渲染节点引用在世
 *            （P0 不得全局替换 14px）。
 *   tooth 3 (清单面自检): 清单版本键在世 + 负向探针（未知键返回缺）+ 产物存在性
 *            探针（正 = 清单文件本体，负 = 不存在路径）。
 * ========================================================================== */
/* coverage: FAS-A7 -- OQ3 实验页/测试字体维持编入 + 14px 兜底不删（构建清单档 canary） */
TEST_CASE("font assets: 不变量canary-OQ3实验页+14px兜底 (FAS-A7)",
          "[font_assets][FAS-A7][guards]")
{
    static const char *const sizes[] = {
        "10", "12", "14", "16", "18", "20", "24", "28", "32",
    };
    size_t i;
    char key[64];

    FAS_REQUIRE_ROOT();

    /* tooth 1: OQ3 维持编入（configure 期事实，逐档钉）。 */
    for (i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        snprintf(key, sizeof(key), "font_test_%s_exists", sizes[i]);
        fas_require_fact(fas_mfact(key), key,
                         "九档实验字体退役（OQ3 裁定维持编入）");
        snprintf(key, sizeof(key), "font_test_%s_in_cmake", sizes[i]);
        fas_require_fact(fas_mfact(key), key,
                         "实验字体被退役出设备构建（OQ3）");
    }
    fas_require_fact(fas_mfact("size_test_page_exists"), "size_test_page_exists",
                     "font_size_test_page.cc 被删——九档实验页退役（OQ3）");
    fas_require_fact(fas_mfact("size_test_page_in_cmake"), "size_test_page_in_cmake",
                     "font_size_test_page.cc 未编入设备构建（OQ3）");

    /* tooth 2: 14px 兜底不删。 */
    fas_require_fact(fas_mfact("host_srcs_puhui14"), "host_srcs_puhui14",
                     "font_puhui_14_1 被摘出 host runner——编译内兜底面被拆（F-5）");
    fas_require_fact(fas_mfact("render_node_puhui14"), "render_node_puhui14",
                     "kidpal_render_node.cc 不再引用 font_puhui_14_1——P0 全局替换"
                     "了 14px（P0 不得全局替换；P1 消费切换归 261028-m6-k2a）");

    /* tooth 3: 清单面自检（版本键 + 负向探针 + 产物存在性正/负探针）。 */
    TEST_ASSERT_TRUE_MESSAGE(fas_mfact("fas_manifest_version") == 1,
        "FAS RED(infra): 清单版本键缺失——结构清单机制失灵或版本漂移");
    TEST_ASSERT_TRUE_MESSAGE(fas_mfact("__fas_no_such_key__") == -1,
        "FAS RED(infra): 负向探针失败——未知键被误报为在（清单解析失灵）");
    TEST_ASSERT_TRUE_MESSAGE(fas_exists(FAS_MANIFEST),
        "FAS RED(infra): 清单产物不可见——fas_exists 失灵或未 build");
    TEST_ASSERT_FALSE_MESSAGE(
        fas_exists("firmware/main/test/__fas_no_such__.c"),
        "FAS RED(infra): fas_exists 把不存在的产物路径报为在");
}

/* ========================================================================== *
 * TEST 8 — FAS-A8：相邻冻结锚 canary（构建清单档，恒绿义务）
 *   tooth 1: p1_cjk_name.acceptance.c [S1] seam 契约面在世（本包不改冻结件，
 *            P0 不预批 P1 supersede——签名面事实在世即锚）+ kidpal_render_node
 *            强符号在世（设备生产入口）。
 *   tooth 2: schedule 常量零触——KSG_COURSE_CAP 84 / KSG_FONT_MIN_PX 14。
 *   tooth 3: story 常量零触——KSTORY_LIST_ROWS_PER_SCREEN 4 /
 *            KSTORY_READER_LINES_PER_SCREEN 4。
 * ========================================================================== */
/* coverage: FAS-A8 -- p1_cjk_name [S1] 契约面在世 + schedule/story 常量零触（构建清单档 canary） */
TEST_CASE("font assets: 相邻冻结锚canary-p1cjk[S1]+schedule/story常量 (FAS-A8)",
          "[font_assets][FAS-A8][guards]")
{
    FAS_REQUIRE_ROOT();

    /* tooth 1: p1_cjk_name [S1] 契约面（configure 期事实）。 */
    fas_require_fact(fas_mfact("p1cjk_acceptance_exists"), "p1cjk_acceptance_exists",
                     "p1_cjk_name.acceptance.c 丢失——相邻冻结件被触碰（本包不改"
                     "既有冻结件铁律）");
    fas_require_fact(fas_mfact("p1cjk_s1_signature"), "p1cjk_s1_signature",
                     "p1_cjk_name [S1] seam 契约签名面缺失——相邻冻结件被修订"
                     "（P0 不预批 P1 supersede）");
    fas_require_fact(fas_mfact("render_node_p1_glyph"), "render_node_p1_glyph",
                     "kidpal_render_node.cc 丢 kidpal_p1_font_glyph 强符号——[S1] "
                     "设备生产入口被拆");

    /* tooth 2: schedule 常量零触。 */
    fas_require_fact(fas_mfact("schedule_ksg_course_cap"), "schedule_ksg_course_cap",
                     "KSG_COURSE_CAP 84 漂移——schedule 定容冻结值被本包触碰"
                     "（跨包事实：F-8 归 P2）");
    fas_require_fact(fas_mfact("schedule_ksg_font_min_px"), "schedule_ksg_font_min_px",
                     "KSG_FONT_MIN_PX 漂移——schedule pitch 下限被触碰");

    /* tooth 3: story 常量零触。 */
    fas_require_fact(fas_mfact("story_list_rows4"), "story_list_rows4",
                     "KSTORY_LIST_ROWS_PER_SCREEN 4 漂移——story 排版常量被本包"
                     "触碰（F-4 归 P3）");
    fas_require_fact(fas_mfact("story_reader_lines4"), "story_reader_lines4",
                     "KSTORY_READER_LINES_PER_SCREEN 4 漂移——story 排版常量被本包"
                     "触碰（F-4 归 P3）");
}
