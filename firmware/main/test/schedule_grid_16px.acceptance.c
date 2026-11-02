/*
 * Frozen acceptance tests — TaskPackage 261028-m6-k2b-p2-schedule-16px (subsystem:
 * firmware/main/display 课表周网格 16px 版式与临时标记形态; M6 列车 P2, 课表网格
 * 16px——pitch 19、带高 78/78/69、容量 4/4/3 与课名下划线 · mergeOrder 62 ·
 * rigor=full / riskTier=high · sourceType=proposal, dependsOn
 * 261028-m6-k1-font-assets-runtime-loading（P0 已 merged）+ 261028-m6-k2a-p1-home-16px
 * （P1 已 merged，mergeOrder 串行）。
 *
 * Story: S-F1.2 · 课表页看「一周总览」（AC1、AC3、AC4）+ 补充规则容量 4/4/3 与超限
 * 静默截断口径（requirements v1.69）。决策面 = PROPOSAL §3.1 F-1（课表正文 16px、
 * 键帽条等 14px 域不动）/ F-3（pitch 19 + 带高 78/78/69 + 容量 4/4/3 + 无标记格
 * 可见 2 字）/ F-8（KSG_COURSE_CAP=84 不动，只改按槽位可画上限）/ F-9（一次性课程
 * ※ 前缀改课名下划线 2px 程序化画线，※ 字形探测/兜底退役；★ 星标不动）+ §3.2
 * OQ4（超限维持静默截断，不新增提示）/ OQ2（母版 v8 只重出四页之课表帧）。
 * 授权链 = TaskPackage 261028-m6-k2b-p2-schedule-16px（exitCriteria P2S-E1/E2/E3）
 * + requirements v1.69 版本行 + brief v7 对齐注记（均在位）。
 * 本件与 schedule_grid.acceptance.c 的分工：本件钉 16px 新形态细节（度量合理域 /
 * pitch 精确值 / 带高 / 容量 4/4/3 / 下划线几何 / ※ 退役），schedule_grid.acceptance.c
 * 经收紧型 supersede（REVISION LOG 2026-11-02 条）钉回归不变式（解析矩阵 / 越带
 * 负锚 / 星标 / 今天列 / 胶水结构）。两件断言互不弱化、互不重复钉死同一像素。
 *
 * ===========================================================================
 * 三分申报（P1 先例同款；设备 TU 不进 host 构建的部分不写假行为断言）:
 *
 *   [host 可验]（本文件全部 TEST_CASE）:
 *     - 16px 句柄消费的行为级证据：mount fonts_root 后经真实渲染路径量课程名墨
 *       高（16px 度量合理域）+ 判别力负控（同一夹具 14px 兜底态可判别）——
 *       retro F1 教训落实：钉度量合理域，不只钉返回码。
 *     - pitch 19 精确锚（同名双课 span 差 == 19）、槽位带 78/78/69（分隔线行
 *       位置 + 旧位置负锚）、容量 4/4/3 与静默截断（帧等价）、下划线几何
 *       （帧 diff 定位：2px 随名宽、名墨之下）、★ 保留、※ 退役负锚。
 *     - 结构半走 FAS-MANIFEST 构建清单机制（P0/P1 同款——human 2026-10-28 定向
 *       裁定运行时禁扫源码树；本件结构面断言在既有 FAS 清单块上扩键，键集见
 *       下 [M] 节，接线增量逐字落 acceptance-wiring-patch.md）。
 *   [设备专有]（host 构建不覆盖，不写假行为断言）:
 *     - 设备侧 cbin 装载等价性归 P0 机制 + FAR-E3 真机轮（P1 裁定 2 口径不变）；
 *       schedule_page.c 为 host/设备两侧同源 TU，消费面行为经 host 帧真测承载。
 *   [真机轮专属]（P2S-E5 / PROPOSAL §5，本文件零 host 齿、不伪装）:
 *     - pitch 19 紧凑观感、4/4/3 真实课表观感、下划线/★ 真机可读性、长课截断
 *       表现——本包自有真机轮（P2S-E5），结果供 P3 终端收口轮复证。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements）:
 *
 *   [F2] 课名 16px 字面源 = P0 统一句柄 [K4] 直接消费（本包 seam 形态决策，与
 *        P1 [S7] 差异申报：home_p1.c 是零 LVGL 纯逻辑面、必须经 kidpal_p1_font.h
 *        间接缝；schedule_page.c 本身 include lvgl.h，故**不另造间接 seam**——
 *        直接 #include "kidpal_font.h" 消费 kidpal_font_body()，P0 [K4] 语义
 *        「P1/P2/P3 唯一消费口、不得各自另造加载/降级分支」的最短兑现。零新
 *        头文件、零 host_infra_seam.c / kidpal_render_node.cc 改动）:
 *
 *     schedule_page.c: ksg_text_font() 改返回 kidpal_font_body()（恒非 NULL——
 *       缺 assets 时 [K2]/[K3] 内部落 font_puhui_14_1 14px 兜底，P0 FAS-A4 语义；
 *       不新增 NULL 回落分支，P0 FAS-A7 render_node_puhui14 canary 不破）。
 *       旧 host source_han_14 / 设备 font_puhui_14_1 直连字面随本包退役（字面
 *       由 kidpal_font.cc 兜底链承载）。
 *     行为证据 = TEST 1 度量齿（经真实渲染路径量墨高，mount 态 → 16px 合理域；
 *       缺 assets 态 → 14px 合理域，两态可判别）。
 *
 *   [F2-L] 版式重基线（F-3/P2S-E1）:
 *     - KSG_ROW_PITCH 16 → 19（schedule_page.c；KSG_FONT_MIN_PX=14 保留不动，
 *       NFR-READ-1 下限语义不变——19 >= 14，FAS 键 schedule_ksg_font_min_px 不破）。
 *     - ksg_slot_ys {30,103}{104,177}{178,254} → {30,107}{108,185}{186,254}
 *       （带高 74/74/77 → 78/78/69；slot2 下缘贴 KSG_GRID_BOTTOM=254 不变）。
 *     - 表头带 y[0..29]、标签列 x[0..39]、day 列带宽 51、KSG_GRID_BOTTOM=254、
 *       今天列整列外框加粗、y>=255 网格外带严格零墨——全部零偏移（回归不变式
 *       由 schedule_grid.acceptance.c 继续钉）。
 *     - schedule_page.h 头注版式区钉同步（FAS 键判读字面见 [M]）。
 *
 *   [F2-C] 容量重基线（F-8/OQ4/P2S-E2）:
 *     - 渲染层按槽位可画上限 = 上午 4 / 下午 4 / 晚上 3（旧实现统一 4——slot2
 *       收 3）；超限静默截断（不滚动、不新增提示、不画第 5/第 4 行）。
 *     - KSG_COURSE_CAP=84 与云端全量下发零偏移：缓存/解析 cap 不得被误缩成
 *       渲染上限（schedule_grid.acceptance.c cap 负断言 + 本件 TEST 3 全量
 *       84 条解析齿共同承载）。
 *     - 截断态（n 超槽位上限）的渲染返回值**不钉**（旧实现返回 n；builder 保持
 *       返回 n 或改返回实画数皆不红——帧断言两种都过；本件截断齿只钉帧等价）。
 *
 *   [F2-M] 临时课标记（F-9/P2S-E3；与 P1 [U1] 首页半同口径——crossPackageBehaviors
 *        第 2 条「全设备临时标记下划线」课表半）:
 *     - 临时信号通道 = 既有 wire flags bit0（KSG_FLAG_TEMP 0x1，B1 一次性安排
 *       区块）；wire 形状/producer 零改（schedule.py week_grid_payload 临时安排
 *       flags|=1 实测锚定；ARQ-261029：producer 不写 ※ 进 name——断言一律锚
 *       flags bit0，不锚 name 前缀）。
 *     - 渲染语义：① ※ 前缀退役——不画、不占课名前缀位（名从格左缘起笔，课名
 *       可见字数不被标记挤占）；② **课名墨右缘 x 跨度上、名墨底缘下方相邻行起
 *       画 2px 横线**（宽 = 名 x 跨度含端点、色 = 行墨色、clip 进行带与格带；
 *       程序化画线、无字体字形依赖）；③ 固定课（flags bit0 = 0）不画线。
 *     - ※ 退役负锚：schedule_page.c 不再含 ※ 码点字面（0x203B / U+203B）与
 *       程序化 ※ 兜底（ksg_marker_temp_proc）——字形探测分支若仍服务 ★ 可保留
 *       （★ 语义不动），探测不得再服务 bit0。
 *     - ★ 星标（flags bit1）不变：可见、落格带、与临时标记可区分、flags=3 叠加
 *       不被吞（schedule_grid.acceptance.c M3K2-A3 重锚件 + 本件几何齿共同承载）。
 *
 *   [M] FAS-MANIFEST 扩键（结构面断言采集通道；在既有 FAS 清单块上扩键，P0/P1
 *       同款——机制全文见 P0 font_assets_runtime.acceptance.c 头注 FAS-MANIFEST 节；
 *       接线增量逐字落 acceptance-wiring-patch.md）:
 *         host_srcs_schedule_grid_16px  本验收件编入 HOST_ACCEPTANCE_SRCS
 *         sched_c_font_seam             schedule_page.c 含 "kidpal_font_body"
 *                                       （[F2] 直接消费 P0 [K4]）
 *         sched_c_pitch_macro           schedule_page.c 含 "#define KSG_ROW_PITCH"
 *         sched_c_pitch_not16           schedule_page.c **不**含 "KSG_ROW_PITCH  16"
 *                                       且**不**含 "KSG_ROW_PITCH 16"（负向——
 *                                       旧 pitch 值退役）
 *         sched_c_no_temp_cp            schedule_page.c **不**含 "0x203B" 且**不**含
 *                                       "U+203B"（负向——※ 码点退役）
 *         sched_c_no_temp_fallback      schedule_page.c **不**含 "ksg_marker_temp_proc"
 *                                       （负向——程序化 ※ 兜底退役）
 *         sched_c_star_kept             schedule_page.c 含 "0x2605"（★ 保留）
 *         sched_h_band_anchors          schedule_page.h 含 "y[30..107]" 且含
 *                                       "y[186..254]"（头注版式区钉新带字面）
 *       复用既有键（零 CMake 增量，本件只读）：schedule_ksg_course_cap
 *       （F-8：#define KSG_COURSE_CAP 84 在世）、schedule_ksg_font_min_px
 *       （NFR-READ-1 14px 下限在世）。
 *
 * ---------------------------------------------------------------------------
 * 跨端申报（acceptance 纪律：跨端 wire 形状断言对照对端实际行为钉，不按本侧想象）:
 *   对端 = server/gateway/schedule.py week_grid_payload（crossEnd counterpart，
 *   launch-card 钉零 producer diff）。2026-11-02 实读锚定：学校课/固定课外课
 *   flags 基 0；临时安排逐门 flags |= 1（bit0）；星标经 group_upcoming 首项
 *   flags |= 2（bit1）；**producer 不往 name 写任何临时标记**（※ 前缀不在
 *   name——ARQ-261029 在案，上游写入侧待派单）。因此 [F2-M] 钉的 flags bit0
 *   锚定与 requirements v1.69 AC3「课名下带下划线标记」完全同源；真机轮若当周
 *   无临时课样本，下划线观测按「观测受限」记录、不记失败（launch-card ARQ 注记）。
 *
 * ---------------------------------------------------------------------------
 * 契约条目 ↔ 断言的强度申报（acceptance-evidence.md 体例）:
 *
 *   SG16-A1（P2S-E1/F-1 课名切 16px 运行时句柄）
 *     -> TEST 1 承载：行为级（mount fonts_root 后经真实渲染路径量课程名墨高落
 *        16px 证据域 [13..16]，修订轮以实测重基线）+ 判别力负控（缺 assets 态同一夹具墨高 [10..13]、
 *        两态严格可判——防「返回码即绿」假绿，retro F1 教训）+ 机制级（FAS 键
 *        sched_c_font_seam = schedule_page.c 直连 [K4] 字面）。弱于契约处显式
 *        声明：墨高窗依据 = 16px CJK 盒 16px 档的合理域，非逐字形像素终判
 *        （字形像素归 design review + 真机轮 P2S-E5）；设备 cbin 装载等价性归
 *        P0 机制 + FAR-E3。
 *   SG16-A2（P2S-E1/F-3 pitch 19）
 *     -> TEST 1 tooth 3 承载：行为级（同名双课 span 差 == 19 精确锚；同名夹具
 *        消除两行字形墨高差，delta 恰等于 pitch）。schedule_grid.acceptance.c
 *        收紧件钉下限 >= 19（异名夹具防漂移），两件互证不互斥。
 *   SG16-A3（P2S-E1/F-3 带高 78/78/69 + 零偏移面）
 *     -> TEST 2 承载：行为级（空网格分隔线行位置 107/185 全行墨 + 旧位置
 *        103/177 全行白负锚 = 带移而非加线；(day,slot) 格归属 diff 落新带；
 *        y>=255 零墨）。机制级 = FAS 键 sched_c_pitch_not16 + sched_h_band_
 *        anchors。弱于契约处：带高由分隔线位置 + 表头/底界锚间接钉（78 =
 *        108-30、69 = 255-186），不逐像素钉带内家具。
 *   SG16-A4（P2S-E2/F-8/OQ4 容量 4/4/3 + 静默截断 + CAP=84）
 *     -> TEST 3 承载：行为级（slot0/slot1 第 4 门画、第 5 门帧等价截断；slot2
 *        第 3 门画、第 4 门帧等价截断——帧等价同时承载「不滚动、不新增提示、
 *        不画超限行」三否定语义；diff containment = 截断不走溢出）+ 行为级
 *        （84 条全量载荷解析 == 84 = 缓存/解析 cap 不被误缩）。机制级 = FAS 键
 *        schedule_ksg_course_cap 复用。弱于契约处：截断态返回值不钉（[F2-C]
 *        申报）；「云端全量下发」的 producer 半 = server 面回归
 *        （impactedTestsSeed[2]，server 零 diff），本件只钉设备消费半。
 *   SG16-A5（P2S-E3/F-9 下划线 + ※ 退役 + ★ 不变 + 可见 2 字）
 *     -> TEST 4 承载：行为级（temp/fixed/star 帧 diff 定位：下划线在世、diff
 *        行全在名墨之下 = ※ 前缀不占笔进、离散 diff 行数 == 2 且各行宽随名
 *        [12..60]、固定课不画线、star 可见可区分、flags=3 叠加、无 flags 确定
 *        性、无标记格 2 字墨宽 >= 28）+ 机制级（FAS 负向三键 = ※ 码点/兜底
 *        退役 + 正向键 = ★ 保留）。弱于契约处：下划线 x 跨度只钉 [12..60] 窗
 *        不钉逐像素形状（像素形态归 design review P2S-E5 + 真机轮）；「可见
 *        2 字」钉墨宽下界 28（16px 两字 = 32，14px 兜底态 = 28 恰过——可见性
 *        下限语义，16px 增益由 SG16-A1 承载）。
 *   SG16-A6（P2S-E3/E2/E1 结构面 + 接线守卫）
 *     -> TEST 5 承载：构建清单档（[M] 八新键 + 两复用键，configure 期事实）。
 *        行为本体由 TEST 1-4 兑现，两档并列互证。
 *
 * ---------------------------------------------------------------------------
 * Coverage map（exitCriteria -> test）:
 *   coverage: P2S-E1（16px 句柄 + pitch 19 + 带高 78/78/69 + 零偏移面）-> TEST 1（SG16-A1/A2）+ TEST 2（SG16-A3）+ TEST 5 机制键
 *   coverage: P2S-E2（容量 4/4/3 + 静默截断 + CAP=84 + 云端全量不变）-> TEST 3（SG16-A4）+ TEST 5 机制键
 *   coverage: P2S-E3（下划线 + ※ 退役 + ★ 不变 + 可见 2 字 + wire 不变）-> TEST 4（SG16-A5）+ TEST 5 结构键 + schedule_grid.acceptance.c 收紧件（flags 保真/星标/越带）
 *   coverage: P2S-E5（真机轮）-> 零 host 齿（头注三分申报挂账，不阻塞 host 半）
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（HOST_TEST_CONVENTIONS.md [C4.2] 约定；接线增量逐字落
 * acceptance-wiring-patch.md）:
 *   1) firmware/main/display/schedule_page.c:
 *      a. #include "kidpal_font.h"；ksg_text_font() 改返回 kidpal_font_body()
 *         （[F2]；删除 LV_FONT_SOURCE_HAN_SANS_SC_14_CJK / ESP_PLATFORM 字面
 *         直连分支——兜底由 kidpal_font.cc [K2]/[K3] 承载，勿新增 NULL 回落）。
 *      b. KSG_ROW_PITCH 16 → 19（[F2-L]）；ksg_slot_ys → {30,107}{108,185}{186,254}
 *         （行标签垂直居中偏移随带高自行重算——观感面不钉）。
 *      c. 渲染层按槽位可画上限 4/4/3（[F2-C]；KSG_COURSE_CAP=84 与解析/缓存
 *         零触碰——cap 误缩即红）。
 *      d. [F2-M]：删除 0x203B / ksg_marker_temp_proc / bit0 的 ksg_draw_marker
 *         路径；flags bit0 → 课名下 2px 下划线（程序化画线：名墨右缘 x 跨度、
 *         名墨底缘下方相邻行起 2px、clip 格带；名不扣前缀宽——与 P1 [U1] 首页
 *         半同口径）；★ bit1 路径零偏移（0x2605 / star 兜底保留）。
 *   2) firmware/main/display/schedule_page.h：头注版式区钉同步——槽位行带行改
 *      「slot0 y[30..107] / slot1 y[108..185] / slot2 y[186..254]」（FAS 键
 *      sched_h_band_anchors 判读字面 = "y[30..107]" 与 "y[186..254]"）；flags
 *      bit0 注记改「临时课（课名下 2px 下划线）」；pitch 注记改 19。
 *   3) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 注册 "../../schedule_grid_16px.acceptance.c"
 *      （p1_home_16px 条目之后、smoke 之前）+ FAS 块扩 [M] 八键 + 新增
 *      CMAKE_CONFIGURE_DEPENDS 三文件（schedule_page.c / schedule_page.h /
 *      无新头文件）——逐字增量见 acceptance-wiring-patch.md。
 *   4) device build 闸：xiaozhi.bin 构建过（schedule_page.c/kidpal_font.cc 均
 *      已在设备构建清单，本包零新增 TU）。
 *   5) mutation 自证（mutationRequired=true）：变异不得保留被测符号原字面为
 *      子串（KR-A3 教训）。预申报断言面 = TEST 1 tooth 3（KSG_ROW_PITCH 回退
 *      16 → pitch 锚红）+ TEST 2（槽位带回退旧值 → 分隔线/负锚齿红）+ TEST 3
 *      tooth（slot2 上限回 4 → 帧等价齿红；KSG_COURSE_CAP 缩 84 → 解析齿红）
 *      + TEST 4 tooth（下划线摘除 / 画线改 1px 或 3px / 画回 ※ / 名扣前缀宽 →
 *      对应齿红；star 删除 → star 齿红）+ TEST 5 键回退（0x203B 回写 /
 *      kidpal_font_body 摘除 → 对应键红）；取证由 builder/fixer 实测、planner
 *      代落 taskpackage mutationEvidence，本文件不落取证。
 *   6) device build 闸 + 真机（P2S-E5）：真机观感/下划线可读性/长课表现归本包
 *      自有真机轮（launch-card 预检在案），结果供 P3 终端轮复证；本文件 host
 *      断言不以 host 字形冒充真机终判（HOST_TEST_CONVENTIONS「字体现实」节）。
 *
 * ---------------------------------------------------------------------------
 * Run protocol（HOST_TEST_CONVENTIONS.md [C4.4]）:
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf
 *      （root 自手持 cwd 定位，仓根或 host_app 目录执行皆可）
 *   RED baseline 判读（本冻结时点，接线增量未落线）:
 *     - 本文件未编入 host runner（host_srcs_schedule_grid_16px 键尚不存在）——
 *       红基线通道 = C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4；编译
 *       通道 error 为零）+ 解析性红定性（逐齿对现状必红的事实源，见文件尾
 *       「RED baseline 判读口径」段）+ schedule_grid.acceptance.c 收紧件与
 *       restore 镜像经通道① host runner 现跑（红名单见 taskpackage
 *       redBaselineResult）。builder 落 BUILDER DUTY 3) 后转通道③ 全量。
 *     - C 类实编（仓根执行）：lv_conf/esp 头经 host 构建同一套 include 面解析——
 *       裸 -I managed_components 集合在本树不可解析 lv_conf_kconfig.h 的 esp_attr.h,
 *       故过程 = 取 build/compile_commands.json 中任一 LVGL-including 验收件
 *       （如 p1_home_16px.acceptance.c）条目的编译命令，删去 `-c` 与 `-o <输出
 *       路径>` 两参，末尾源文件替换为本件并加 `-fsyntax-only`（2026-11-02 实测
 *       rc=0、零 error 零 warning——含 -Wall -Wextra 全 host 工具链旗标集）。
 *       等价展开旗标骨架 = -DESP_PLATFORM -DLV_CONF_INCLUDE_SIMPLE -I build/config
 *       -I host_app/main -I display -I esp-idf components…（不逐字复抄, 以构建
 *       产物为单源）。
 *     - 执行通道红基线（接线落线后）期望见文件尾判读段。
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

#include <lvgl.h>

#include "../display/kidpal_font.h" /* P0 [K1]-[K4] 统一句柄 seam（强定义 =
                                     * display/kidpal_font.cc，host runner 在世） */

/* 编译内 14px 兜底（font_puhui_14_1.c 已编入 host runner，强定义在世）。 */
extern const lv_font_t font_puhui_14_1;

/* host 渲染缝（host_infra_seam.c 提供，恒链入 runner）。只探针。 */
int host_infra_render_frame(void);
int32_t host_infra_read_pixel(int x, int y);

/* -------------------------------------------------------------------------- *
 * Frozen offline contract 常量 + weak 哨兵（schedule_grid.acceptance.c /
 * schedule_grid_restore.acceptance.c 先例：多重 weak 合法、语义一致；host runner
 * 里由在世 schedule_page.c 强定义覆盖）。SGX_UNIMPLEMENTED(-2) = 「frozen contract
 * not implemented」解析性红标记。
 * ------------------------------------------------------------------------ */
#define SGX_UNIMPLEMENTED (-2)

#define SGX_NAME_CAP   48
#define SGX_FLAG_TEMP  0x1
#define SGX_FLAG_STAR  0x2

typedef struct {
    int  day;
    int  slot;
    char name[SGX_NAME_CAP];
    int  flags;
} kidpal_schedule_course_t;

__attribute__((weak)) int kidpal_schedule_grid_parse(const char *data_json,
                                                     kidpal_schedule_course_t *courses,
                                                     int cap)
{
    (void)data_json; (void)courses; (void)cap;
    return SGX_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_schedule_grid_render(const kidpal_schedule_course_t *courses,
                                                      int n, int today_col)
{
    (void)courses; (void)n; (void)today_col;
    return SGX_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * 版式重基线常量（F-3/P2S-E1；schedule_page.c 落地值 == 本表，不得偏移）。
 * ------------------------------------------------------------------------ */
#define SGX_FB_W 400
#define SGX_FB_H 300
#define SGX_WHITE 0xFFFFFFul
#define SGX_NOT_RENDERED ((int32_t)-1)
#define SGX_GRID_BOTTOM 254
#define SGX_PITCH_PX   19  /* F-3：16px 下格内课程名行 pitch（原 16 收紧重基线） */
#define SGX_COURSE_CAP 84  /* F-8：缓存/解析 cap 定容冻结值不动（7 天 × 3 格 × 每格 4 门） */

/* 槽位行带 {y0, y1}（表头带 y[0..29] 不变）：带高 78/78/69，slot2 下缘贴
 * KSG_GRID_BOTTOM=254。 */
static const int sgx_slot_ys[3][2] = { { 30, 107 }, { 108, 185 }, { 186, 254 } };

/* 按槽位可画上限（F-3/OQ4；渲染层截断，缓存/解析 cap 84 不动）。 */
static const int sgx_slot_caps[3] = { 4, 4, 3 };

/* 16px 度量合理域（SG16-A1；2026-11-02 修订轮以证据重基线：builder 实测 16px
 * 资产 line_height=23/base_line=5、「课」box 15x14、「口」box 12x13——CJK 墨高
 * 实际 14, 冻结时点预判窗 [15..18] 下界越证据域；P1 同资产同值。修订窗 = 16px
 * 态 [13..16] / 14px 兜底态 [10..13]——两窗在 13 处搭接, tooth 1 窗口对旧 14px
 * 字面不再独立红, 基线判别力由 tooth 2 严格不等式（h16 > h14, 同夹具同渲染路径
 * 两 mount 态）承载）。 */
#define SGX_INK16_MIN 13
#define SGX_INK16_MAX 16
#define SGX_INK14_MIN 10
#define SGX_INK14_MAX 13

/* 下划线判读窗（[F2-M]；随名宽——2 字 16px 名 ≈ 32px）。 */
#define SGX_UL_MIN_PX 12
#define SGX_UL_MAX_PX 60

/* 无标记格可见 2 字墨宽下界（16px 两字 32、14px 兜底态 28 恰过——可见性下限）。 */
#define SGX_2CHAR_MIN_PX 28

/* -------------------------------------------------------------------------- *
 * Repo-root 定位（只服务 fonts_root 路径与 FAS 清单产物解析；运行时禁扫源码树
 * ——human 2026-10-28 裁定，P0/P1 先例同款）。ELF fopen 不认 /mnt/c 绝对路径，
 * 相对路径自手持 cwd 定位。
 * ------------------------------------------------------------------------ */
#define SGX_MAX_PATH 1024
#define SGX_MAX_READ (8u * 1024u * 1024u)
#define SGX_FONTS_ROOT "firmware/main/assets/fonts_root"
#define SGX_FAS_MANIFEST "firmware/main/test/host_app/build/fas_structure_manifest.txt"

static char sgx_root[SGX_MAX_PATH];
static int sgx_root_ready;

static int sgx_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int sgx_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[SGX_MAX_PATH];
    size_t i;

    if (sgx_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (sgx_is_regular(probe)) {
            snprintf(sgx_root, sizeof(sgx_root), "%s", cands[i]);
            sgx_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int sgx_rel(char *out, size_t cap, const char *rel)
{
    if (!sgx_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", sgx_root, rel);
    return 1;
}

static uint8_t *sgx_read_bin(const char *rel, size_t *out_len)
{
    char p[SGX_MAX_PATH];
    FILE *f;
    long sz;
    uint8_t *buf;

    if (out_len != NULL) {
        *out_len = 0;
    }
    if (!sgx_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SGX_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static char *sgx_read_text(const char *rel)
{
    size_t len;
    uint8_t *raw = sgx_read_bin(rel, &len);
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

#define SGX_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(sgx_locate_root(), \
    "SGX RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

static char sgx_fm_buf[1024];
static const char *sgx_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *sgx_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(sgx_fm_buf, sizeof(sgx_fm_buf), fmt, ap);
    va_end(ap);
    return sgx_fm_buf;
}

static void sgx_require_impl(int value, const char *what)
{
    if (value == SGX_UNIMPLEMENTED) {
        TEST_ASSERT_TRUE_MESSAGE(0, sgx_fm(
            "SGX RED: frozen contract NOT implemented — %s（builder must land "
            "[F2]/[F2-L]/[F2-C]/[F2-M] per schedule_grid_16px.acceptance.c header）",
            what));
    }
}

/* -------------------------------------------------------------------------- *
 * [K1] host 测试 reader：fonts_root 直读（F-7 同源；P0/P1 fas/p16 先例同款）。
 * ------------------------------------------------------------------------ */
static int sgx_host_reader(const char *name, const uint8_t **data, size_t *size)
{
    char rel[SGX_MAX_PATH];
    uint8_t *buf;
    size_t len;

    snprintf(rel, sizeof(rel), "%s/%s", SGX_FONTS_ROOT, name);
    buf = sgx_read_bin(rel, &len);
    if (buf == NULL) {
        return -1;
    }
    *data = buf;
    *size = len;
    return 0;
}

/* 全缺 reader（复位 14px 兜底态——mount 态不得泄漏进后续执行的其它冻结件，
 * P1 tooth 4 纪律；mount(NULL 等价语义全缺 reader) 后 body() 恒等
 * font_puhui_14_1，P0 FAS-A4 合法完成态）。 */
static int sgx_missing_reader(const char *name, const uint8_t **data, size_t *size)
{
    (void)name; (void)data; (void)size;
    return -1;
}

/* -------------------------------------------------------------------------- *
 * FAS-MANIFEST 消费面（p16_mfact 同款：行首锚定 key=，未知键返回 -1 = infra 红）。
 * ------------------------------------------------------------------------ */
static int sgx_mfact(const char *key)
{
    static char cache[8 * 1024];
    static char val[32];
    static int loaded;
    const char *line;
    size_t klen = strlen(key);

    if (!loaded) {
        char *txt = sgx_read_text(SGX_FAS_MANIFEST);
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

static void sgx_require_fact(int fact, const char *key, const char *what)
{
    if (fact == -1) {
        TEST_ASSERT_TRUE_MESSAGE(0, sgx_fm(
            "SGX RED(infra): 结构清单键 %s 缺失或清单未生成——重跑 host build 让 "
            "configure 期重采集（键集 = acceptance-wiring-patch.md；机制见 P0 "
            "font_assets_runtime.acceptance.c 头注 FAS-MANIFEST 节）", key));
        return;
    }
    TEST_ASSERT_TRUE_MESSAGE(fact == 1, sgx_fm(
        "SGX RED: 结构事实 %s 不成立——%s（事实由 host CMakeLists configure 期采集，"
        "机制见冻结件头注 [M] 节）", key, what));
}

/* -------------------------------------------------------------------------- *
 * 帧探针 helpers（ksg_/p16_ 先例同款）。探针失败（未渲染）大声红，绝不读成零墨。
 * ------------------------------------------------------------------------ */
static int32_t sgx_f_a[SGX_FB_W * SGX_FB_H];
static int32_t sgx_f_b[SGX_FB_W * SGX_FB_H];
static int32_t sgx_f_c[SGX_FB_W * SGX_FB_H];
static int32_t sgx_f_d[SGX_FB_W * SGX_FB_H];

static int sgx_snap(int32_t *buf)
{
    int x, y;

    for (y = 0; y < SGX_FB_H; y++) {
        for (x = 0; x < SGX_FB_W; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == SGX_NOT_RENDERED) {
                return -1;
            }
            buf[y * SGX_FB_W + x] = px;
        }
    }
    return 0;
}

static void sgx_render_strict(const kidpal_schedule_course_t *cs, int n, int today,
                              int32_t *buf)
{
    int r = kidpal_schedule_grid_render(cs, n, today);

    sgx_require_impl(r, "kidpal_schedule_grid_render unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == n,
        sgx_fm("SGX render RED: render face reported %d != n=%d — 常规（非截断态）"
               "必须返回 n（frozen contract）", r, n));
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE_MESSAGE(sgx_snap(buf) == 0,
        "SGX RED(infra): frame not rendered — 探针缝断裂");
}

/* 截断态渲染：返回值语义不钉（[F2-C] 申报——旧实现返回 n、实画数皆不红），
 * 只收帧。 */
static void sgx_render_loose(const kidpal_schedule_course_t *cs, int n, int today,
                             int32_t *buf)
{
    int r = kidpal_schedule_grid_render(cs, n, today);

    sgx_require_impl(r, "kidpal_schedule_grid_render unimplemented");
    (void)r;
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE_MESSAGE(sgx_snap(buf) == 0,
        "SGX RED(infra): frame not rendered — 探针缝断裂");
}

/* diff 统计：a/b 相异像素总数与落在 [x0,y0,x1,y1]（含端点）带内的数目。 */
static void sgx_diff_stats(const int32_t *a, const int32_t *b,
                           int x0, int y0, int x1, int y1,
                           int *out_total, int *out_in_band)
{
    int total = 0, in_band = 0;
    int x, y;

    if (x0 < 0) { x0 = 0; }
    if (y0 < 0) { y0 = 0; }
    if (x1 >= SGX_FB_W) { x1 = SGX_FB_W - 1; }
    if (y1 >= SGX_FB_H) { y1 = SGX_FB_H - 1; }
    for (y = 0; y < SGX_FB_H; y++) {
        for (x = 0; x < SGX_FB_W; x++) {
            if (a[y * SGX_FB_W + x] != b[y * SGX_FB_W + x]) {
                total++;
                if (x >= x0 && x <= x1 && y >= y0 && y <= y1) {
                    in_band++;
                }
            }
        }
    }
    *out_total = total;
    *out_in_band = in_band;
}

/* 带内暗墨（非白、非未渲染哨兵）垂直跨度与暗像素计数（x/y 各内缩 2px 采样——
 * 表格分隔线画在带边界时不入样）。 */
static void sgx_ink_span(const int32_t *f, int x0, int y0, int x1, int y1,
                         int *out_min_y, int *out_max_y, int *out_dark)
{
    int min_y = -1, max_y = -1, dark = 0;
    int x, y;

    for (y = y0 + 2; y <= y1 - 2; y++) {
        for (x = x0 + 2; x <= x1 - 2; x++) {
            int32_t p = f[y * SGX_FB_W + x];
            if (p >= 0 && p != (int32_t)SGX_WHITE) {
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

/* 带内（快照帧）最大墨行（自上而下扫，最后含墨行）。x/y 双向内缩 2px：槽位带
 * 上下缘横线与带左缘日列竖线（x0 处, 贯穿全带高）皆家具不入样——max_row 只量
 * 课名/标记墨（2026-11-02 修订轮缺陷 5+7：旧实现 y 向不缩命中年 254 网格底线、
 * x 向不缩命中 x0 竖线 → m 恒为带底, 「标记在名墨之下」与 y>=255 零墨互斥）。 */
static int sgx_max_ink_row(const int32_t *f, int x0, int y0, int x1, int y1)
{
    int x, y;
    int max_row = y0 + 1;

    for (y = y0 + 2; y <= y1 - 2; y++) {
        for (x = x0 + 2; x <= x1 - 2; x++) {
            if (f[y * SGX_FB_W + x] != (int32_t)SGX_WHITE) {
                max_row = y;
                break;
            }
        }
    }
    return max_row;
}

/* 带内文字区（行 y0..max_row）墨列跨度。x 向内缩 2px：日列带左缘竖线（带界
 * 家具，贯穿全带高）不入样；min/max 取扫描域**全局**极值（2026-11-02 修订轮
 * 缺陷 5+7：旧实现 min_x 为「首枚墨像素」（最高墨行的首墨——两字形不等高时
 * 只有较高字形入 min）、max_x 为「末次赋值」，都非全局极值）。max_row 由调用
 * 方先证有墨（应传 sgx_max_ink_row 的内缩域结果）。 */
static int sgx_text_col_span(const int32_t *f, int x0, int y0, int x1, int max_row,
                             int *out_min_x, int *out_max_x)
{
    int x, y;
    int min_x = x1 + 1, max_x = -1;

    for (y = y0; y <= max_row; y++) {
        for (x = x0 + 2; x <= x1 - 2; x++) {
            if (f[y * SGX_FB_W + x] != (int32_t)SGX_WHITE) {
                if (x < min_x) {
                    min_x = x;
                }
                if (x > max_x) {
                    max_x = x;
                }
            }
        }
    }
    *out_min_x = (min_x <= x1) ? min_x : -1;
    *out_max_x = max_x;
    return (max_x >= 0) ? (max_x - min_x + 1) : 0;
}

/* 帧内整行 y（x 0..397）是否全墨（分隔线行）/ 全白（旧行位置负锚）。 */
static int sgx_row_all_dark(const int32_t *f, int y)
{
    int x;
    for (x = 0; x <= 397; x++) {
        int32_t p = f[y * SGX_FB_W + x];
        if (p < 0 || p == (int32_t)SGX_WHITE) {
            return 0;
        }
    }
    return 1;
}

/* 旧行位置负锚：除网格竖线家具列（x=0/397 边缘与 40+51k 日列线，全部贯穿
 * y 0..254）之外全白——旧分隔线行必然残留竖线墨，「全行白」逻辑上不可满足
 * （2026-11-02 修订轮缺陷 2）。 */
static int sgx_row_white_outside_vlines(const int32_t *f, int y)
{
    int x;
    for (x = 1; x <= 396; x++) {
        int32_t p;
        if (x >= 40 && x <= 346 && ((x - 40) % 51) == 0) {
            continue; /* 日列竖线家具列 */
        }
        p = f[y * SGX_FB_W + x];
        if (p >= 0 && p != (int32_t)SGX_WHITE) {
            return 0;
        }
    }
    return 1;
}

/* rect 行 y 内 a/b 的 diff 像素数（x 取 rect[0]..rect[1]）。 */
static int sgx_row_diff_px(const int32_t *a, const int32_t *b, int x0, int x1, int y)
{
    int x, d = 0;
    for (x = x0; x <= x1; x++) {
        if (a[y * SGX_FB_W + x] != b[y * SGX_FB_W + x]) {
            d++;
        }
    }
    return d;
}

/* 带内逐行 diff（a vs b）：离散 diff 行数 + 行界。 */
static void sgx_diff_rows(const int32_t *a, const int32_t *b, int x0, int y0, int x1,
                          int y1, int *out_rows, int *out_min_row, int *out_max_row)
{
    int x, y;
    int rows = 0, min_row = -1, max_row = -1;

    for (y = y0; y <= y1; y++) {
        int rd = 0;
        for (x = x0; x <= x1; x++) {
            if (a[y * SGX_FB_W + x] != b[y * SGX_FB_W + x]) {
                rd++;
            }
        }
        if (rd > 0) {
            rows++;
            if (min_row < 0) {
                min_row = y;
            }
            max_row = y;
        }
    }
    *out_rows = rows;
    *out_min_row = min_row;
    *out_max_row = max_row;
}

/* -------------------------------------------------------------------------- *
 * Fixture helpers。
 * ------------------------------------------------------------------------ */
static kidpal_schedule_course_t *sgx_course(kidpal_schedule_course_t *c, int day,
                                            int slot, const char *name, int flags)
{
    memset(c, 0, sizeof(*c));
    c->day = day;
    c->slot = slot;
    snprintf(c->name, sizeof(c->name), "%s", name);
    c->flags = flags;
    return c;
}

static void sgx_day_band(int day, int *x0, int *x1)
{
    *x0 = 40 + 51 * day;
    *x1 = *x0 + 50;
}

/* 同 (day,slot) 填 count 门课（名字池 = 既有测试已核验字形集）。 */
static int sgx_fill_slot(kidpal_schedule_course_t *cs, int day, int slot, int count)
{
    static const char *const names[5] = {
        "语文", "数学", "口语", "舞蹈", "编程",
    };
    int i;

    for (i = 0; i < count; i++) {
        sgx_course(&cs[i], day, slot, names[i % 5], 0);
    }
    return count;
}

/* ========================================================================== *
 * TEST 1 — SG16-A1/A2：16px 句柄消费度量合理域 + 判别负控 + pitch 19 精确锚
 *   tooth 0 (infra gate, must-stay-green): mount fonts_root -> body() 真资产句柄
 *            （非 14px、16 档 [20..25]）+ '课' glyph dsc 盒度量合理 [12..18]^2
 *            （P0 资产链自诊断, P1 tooth 0b 同款）。
 *   tooth 1 (行为级): mount 态渲染「课程」单课——名墨高 ∈ [13..16]（16px 证据域;
 *            修订轮以实测重基线——14px 旧字面域与 13 搭接, 本窗对旧实现不再独立红,
 *            基线判别力由 tooth 2 严格不等式承载。retro F1：钉度量不只钉返回码）。
 *   tooth 2 (判别负控, RED 设计态): 全缺 mount 态同一夹具名墨高 ∈ [10..13] 且 < tooth 1 实测
 *            （14px/16px 经真实渲染路径确可判别——两态不可判 = infra 红自诊断）。
 *   tooth 3 (行为级, RED 设计态): 同名双课（语文×2）同格堆叠 span 差 == 19
 *            （同名夹具下 delta 恰等于 pitch——精确锚，KSG_ROW_PITCH 必须 = 19）。
 *   tooth 4 (复位): mount(missing reader) 复位 14px 兜底态——防跨文件泄漏。
 *   RED（设计态）: tooth 1（ksg_text_font 仍 14px 字面）+ tooth 3（pitch 仍 16）；
 *            tooth 0/2(半)/4 现状即绿或随 tooth 1 转绿。
 * ========================================================================== */
/* coverage: SG16-A1 -- 16px 句柄消费：名墨高 16px 证据域 [13..16]（修订轮重基线）+ 14px 兜底判别负控（严格不等式）+ P0 资产 gate + mount 态复位 */
/* covers: SG16-A2 -- pitch 19 精确锚（同名双课 span 差 == 19, KSG_ROW_PITCH 必须 = 19） */
TEST_CASE("schedule grid 16px: 16px句柄消费度量与pitch19锚 (SG16-A1/A2)",
          "[schedule_grid_16px][SG16-A1][SG16-A2][behavior]")
{
    const lv_font_t *body;
    kidpal_schedule_course_t cs[2];
    int rc, min_a, max_a, dark_a, min_b, max_b, dark_b, h16, h14, x0, x1, y0, y1;

    SGX_REQUIRE_ROOT();

    /* tooth 0: P0 资产 gate（must-stay-green）。 */
    rc = kidpal_font_mount_assets(sgx_host_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        sgx_fm("SGX RED(infra): mount_assets(rc=%d) 非完成态——P0 资产/装载底座缺场"
               "（font_body_16.bin / kidpal_font.cc，P0 交付面）", rc));
    body = kidpal_font_body();
    TEST_ASSERT_TRUE_MESSAGE(body != NULL && body != &font_puhui_14_1,
        "SGX RED(infra): body() 缺场或落 14px 兜底——资产在库却未装载（P0 装载面断裂）");
    TEST_ASSERT_TRUE_MESSAGE(body->line_height >= 20 && body->line_height <= 25,
        sgx_fm("SGX RED(infra): body16 line_height=%d 越出 16px 档 [20..25]（P0 FAS-A2 "
               "标定档）", (int)body->line_height));
    {
        lv_font_glyph_dsc_t g0;
        int ok0 = lv_font_get_glyph_dsc(body, &g0, 0x8BFEu /* 课 */, 0);
        TEST_ASSERT_TRUE_MESSAGE(ok0 && g0.box_w >= 12 && g0.box_w <= 18 &&
                                     g0.box_h >= 12 && g0.box_h <= 18,
            sgx_fm("SGX RED(infra): body16 '课' glyph dsc 盒度量不合理（ok=%d "
                   "box=%dx%d）——cbin 资产链自诊断红（P1 tooth 0b 同款，非本包设计态）",
                   ok0, ok0 ? (int)g0.box_w : -1, ok0 ? (int)g0.box_h : -1));
    }

    sgx_day_band(0, &x0, &x1);
    y0 = sgx_slot_ys[0][0];
    y1 = sgx_slot_ys[0][1];

    /* tooth 1: mount 态（16px 消费）名墨高合理域。 */
    sgx_render_strict(sgx_course(&cs[0], 0, 0, "课程", 0), 1, -1, sgx_f_a);
    sgx_ink_span(sgx_f_a, x0, y0, x1, y1, &min_a, &max_a, &dark_a);
    TEST_ASSERT_TRUE_MESSAGE(dark_a > 0 && min_a >= 0,
        "SGX-A1 RED(infra): course name produced no measurable ink in its cell — "
        "16px 资产缺「课程」字形或格归属失真（fixture 字形集已核验, 缺场即 infra 红）");
    h16 = max_a - min_a + 1;
    TEST_ASSERT_TRUE_MESSAGE(h16 >= SGX_INK16_MIN && h16 <= SGX_INK16_MAX,
        sgx_fm("SGX-A1 RED: 课名墨高 %d 越出 16px 合理域 [%d..%d]——课表正文未切 P0 "
               "16px 句柄（[F2] 未实现或 16px 字形度量异常——14px 旧字面域与窗口搭接, "
               "判别力由 tooth 2 承载）或 16px "
               "字形度量异常（须附实测证据重基线, 不静默放宽）",
               h16, SGX_INK16_MIN, SGX_INK16_MAX));

    /* tooth 2: 判别负控——全缺 mount（14px 兜底）同一夹具。 */
    rc = kidpal_font_mount_assets(sgx_missing_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        "SGX RED(infra): 复位 mount rc!=0——全缺 reader 非完成态（P0 [K3] 降级语义破）");
    sgx_render_strict(sgx_course(&cs[0], 0, 0, "课程", 0), 1, -1, sgx_f_b);
    sgx_ink_span(sgx_f_b, x0, y0, x1, y1, &min_b, &max_b, &dark_b);
    TEST_ASSERT_TRUE_MESSAGE(dark_b > 0 && min_b >= 0,
        "SGX RED(infra): 14px 兜底态名墨缺场——夹具字形缺场（fixture 对齐协商, 不静默）");
    h14 = max_b - min_b + 1;
    TEST_ASSERT_TRUE_MESSAGE(h14 >= SGX_INK14_MIN && h14 <= SGX_INK14_MAX,
        sgx_fm("SGX-A1 RED(infra): 14px 兜底态墨高 %d 越出 [%d..%d]——兜底链度量异常"
               "（P0 [K2] 兜底面, 非本包设计态）", h14, SGX_INK14_MIN, SGX_INK14_MAX));
    TEST_ASSERT_TRUE_MESSAGE(h16 > h14,
        sgx_fm("SGX-A1 RED(infra): 16px 态墨高 %d 不大于 14px 态 %d——两态不可判别, "
               "tooth 1 合理域断言失去判别力（infra 修正不静默）", h16, h14));

    /* tooth 3: pitch 19 精确锚（重新 mount 16px 态——与本齿判读态一致）。 */
    rc = kidpal_font_mount_assets(sgx_host_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "SGX RED(infra): re-mount rc!=0");
    sgx_render_strict(sgx_course(&cs[0], 0, 0, "语文", 0), 1, -1, sgx_f_c);
    sgx_ink_span(sgx_f_c, x0, y0, x1, y1, &min_a, &max_a, &dark_a);
    TEST_ASSERT_TRUE_MESSAGE(dark_a > 0 && min_a >= 0,
        "SGX RED(infra): 单课帧名墨缺场");
    sgx_fill_slot(cs, 0, 0, 2); /* 同名双课：语文×2（delta 恰 = pitch） */
    sgx_render_strict(cs, 2, -1, sgx_f_d);
    sgx_ink_span(sgx_f_d, x0, y0, x1, y1, &min_b, &max_b, &dark_b);
    TEST_ASSERT_TRUE_MESSAGE(dark_b > dark_a,
        "SGX-A2 RED: 第二课未在同格向下堆叠——格内多课竖排缺失");
    TEST_ASSERT_TRUE_MESSAGE(max_b - min_b == SGX_PITCH_PX + (max_a - min_a),
        sgx_fm("SGX-A2 RED: 同名双课 span 差 %d != pitch 19——KSG_ROW_PITCH 必须 = 19 "
               "（F-3/P2S-E1 重基线; 现状 16 = 设计态红）",
               (max_b - min_b) - (max_a - min_a)));

    /* tooth 4: 复位兜底态（防 mount 态跨文件泄漏）。 */
    rc = kidpal_font_mount_assets(sgx_missing_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "SGX RED(infra): 复位 mount rc!=0");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_font_body() == &font_puhui_14_1,
        "SGX RED(infra): 复位后 body() != font_puhui_14_1——14px 兜底语义被破");
}

/* ========================================================================== *
 * TEST 2 — SG16-A3：槽位带 78/78/69 重基线（分隔线行位置 + 旧位置负锚 + 格归属）
 *   tooth 1 (must-stay-green): 空网格表头底线 y=29 全行墨 + 网格底线 y=254 全行墨
 *            （表头带与 KSG_GRID_BOTTOM 零偏移）。
 *   tooth 2 (RED 设计态): 新分隔线 y=107 / y=185 全行墨（F-3 带移）。
 *   tooth 3 (RED 设计态): 旧分隔线位置 y=103 / y=177 全行白（负锚——带移不是加线;
 *            现状线在旧位 → 红）。
 *   tooth 4 (RED 设计态): (day,slot) 格归属——slot1 两门课帧 diff 全落新带
 *            （schedule_grid.acceptance.c TEST 8 重锚件镜像; 现状旧带渲染墨
 *            y106 起落新带 108 外 → 红）。
 *   tooth 5 (must-stay-green): y>=255 网格外带严格零墨（越带负锚, 越域继承）。
 * ========================================================================== */
/* coverage: SG16-A3 -- 槽位带 78/78/69：新分隔线行 107/185 全行墨 + 旧位置 103/177 全行白负锚 + 表头 29 / 底界 254 零偏移 + (day,slot) 格归属落新带 + y>=255 零墨 */
TEST_CASE("schedule grid 16px: 槽位带78/78/69与格归属 (SG16-A3)",
          "[schedule_grid_16px][SG16-A3][behavior]")
{
    kidpal_schedule_course_t cs[2];
    int x0, x1, y0, y1, total, in_band;

    SGX_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "SGX RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    /* 空网格：只有家具。 */
    sgx_render_strict(NULL, 0, -1, sgx_f_a);

    /* tooth 1: 表头底线与网格底线零偏移（must-stay-green）。 */
    TEST_ASSERT_TRUE_MESSAGE(sgx_row_all_dark(sgx_f_a, 29),
        "SGX-A3 RED: 表头底线 y=29 非全行墨——表头带被本包改动（零偏移面回归）");
    TEST_ASSERT_TRUE_MESSAGE(sgx_row_all_dark(sgx_f_a, SGX_GRID_BOTTOM),
        "SGX-A3 RED: 网格底线 y=254 非全行墨——KSG_GRID_BOTTOM 被改动（零偏移面回归）");

    /* tooth 2: 新分隔线行（F-3 带移）。 */
    TEST_ASSERT_TRUE_MESSAGE(sgx_row_all_dark(sgx_f_a, sgx_slot_ys[0][1]),
        "SGX-A3 RED: 新 slot0 分隔线 y=107 非全行墨——槽位带未按 78/78/69 重基线"
        "（现状线在 y=103 = 设计态红）");
    TEST_ASSERT_TRUE_MESSAGE(sgx_row_all_dark(sgx_f_a, sgx_slot_ys[1][1]),
        "SGX-A3 RED: 新 slot1 分隔线 y=185 非全行墨——槽位带未按 78/78/69 重基线"
        "（现状线在 y=177 = 设计态红）");

    /* tooth 3: 旧位置负锚（带移而非加线；竖线家具列容忍——修订轮缺陷 2）。 */
    TEST_ASSERT_TRUE_MESSAGE(sgx_row_white_outside_vlines(sgx_f_a, 103),
        "SGX-A3 RED: 旧分隔线位置 y=103 仍有非竖线墨——旧带未退役（F-3 带移要求线"
        "随带走, 新旧并存 = 版式失真）");
    TEST_ASSERT_TRUE_MESSAGE(sgx_row_white_outside_vlines(sgx_f_a, 177),
        "SGX-A3 RED: 旧分隔线位置 y=177 仍有非竖线墨——旧带未退役（同上）");

    /* tooth 4: (day,slot) 格归属（新带）——slot1 两门课异日帧 diff 全落两格带并集。 */
    sgx_render_strict(sgx_course(&cs[0], 2, 1, "语文", 0), 1, -1, sgx_f_a);
    sgx_render_strict(sgx_course(&cs[1], 4, 1, "数学", 0), 1, -1, sgx_f_b);
    sgx_diff_stats(sgx_f_a, sgx_f_b, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "SGX-A3 RED: 两门异日课程产生全同帧——课程没有按 (day,slot) 落格渲染");
    sgx_day_band(2, &x0, &x1);
    y0 = sgx_slot_ys[1][0];
    y1 = sgx_slot_ys[1][1];
    {
        int full_total, in2, xa, ya, xb, yb;
        sgx_diff_stats(sgx_f_a, sgx_f_b, x0, y0, x1, y1, &total, &in_band);
        sgx_day_band(4, &xa, &xb);
        ya = y0;
        yb = y1;
        sgx_diff_stats(sgx_f_a, sgx_f_b, xa, ya, xb, yb, &full_total, &in2);
        TEST_ASSERT_TRUE_MESSAGE(in_band + in2 == full_total,
            "SGX-A3 RED: 课程 diff 像素落在新槽位带/日列带之外——格归属未随 78/78/69 "
            "重基线（现状旧带 y104 起 = 设计态红）");
    }

    /* tooth 5: 网格外带零墨（越带负锚, 越域继承——slot2 有课帧下验证）。 */
    sgx_render_strict(sgx_course(&cs[0], 6, 2, "舞蹈", 0), 1, -1, sgx_f_a);
    {
        int dark = 0;
        int x, y;
        for (y = SGX_GRID_BOTTOM + 1; y < SGX_FB_H; y++) {
            for (x = 0; x < SGX_FB_W; x++) {
                int32_t p = sgx_f_a[y * SGX_FB_W + x];
                if (p >= 0 && p != (int32_t)SGX_WHITE) {
                    dark++;
                }
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(dark == 0,
            "SGX-A3 RED: grid frame paints ink below y=254——网格溢出可用区下缘"
            "（键帽条三槽常驻不被网格遮挡）");
    }
}

/* ========================================================================== *
 * TEST 3 — SG16-A4：容量 4/4/3 + 静默截断 + CAP=84 与云端全量不变
 *   tooth 1 (must-stay-green, F-8): 84 条全量载荷（7 天 × 3 槽 × 4 门）解析
 *            == 84——缓存/解析 cap 不得被误缩成渲染上限。
 *   tooth 2 (must-stay-green): slot0 第 3→4 门 diff > 0 且落带（第 4 门真画）。
 *   tooth 3 (must-stay-green): slot0 第 4→5 门 diff == 0 全帧（静默截断：不画
 *            第 5 行、不滚动、不新增提示——任何溢出形态都会破坏帧等价）。
 *   tooth 4 (must-stay-green): slot1 同 tooth 2/3（上限 4）。
 *   tooth 5 (RED 设计态): slot2 第 2→3 门 diff > 0（第 3 门真画）+ 第 3→4 门
 *            diff == 0（上限 3；现状上限 4 → 第 4 门被画 → 红）。
 *   tooth 6 (must-stay-green): 加门 diff containment——slot0 第 4 门 / slot2
 *            第 3 门 diff 全落本槽带（截断降级不走溢出）。
 * ========================================================================== */
/* coverage: SG16-A4 -- 容量 4/4/3：slot0/1 第 4 门画第 5 门帧等价 + slot2 第 3 门画第 4 门帧等价（静默截断不滚动不提示）+ diff containment + 84 条全量解析（F-8 缓存/解析 cap 不误缩） */
TEST_CASE("schedule grid 16px: 槽位容量4/4/3与静默截断 (SG16-A4)",
          "[schedule_grid_16px][SG16-A4][behavior]")
{
    kidpal_schedule_course_t cs[5];
    static char payload[16384];
    kidpal_schedule_course_t cap84[SGX_COURSE_CAP];
    size_t used = 0;
    int d, s, i, n, x0, x1, total, in_band;

    SGX_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "SGX RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    /* tooth 1: 云端全量 84 条解析（F-8 must-stay-green）。 */
    {
        static const char *const names[5] = {
            "语文", "数学", "口语", "舞蹈", "编程",
        };
        used += (size_t)snprintf(payload, sizeof(payload) - used,
                                 "{\"week\": {\"courses\": [");
        for (d = 0; d < 7; d++) {
            for (s = 0; s < 3; s++) {
                for (i = 0; i < 4; i++) {
                    used += (size_t)snprintf(payload + used, sizeof(payload) - used,
                                             "%s{\"day\": %d, \"slot\": %d, "
                                             "\"name\": \"%s\", \"flags\": 0}",
                                             (d == 0 && s == 0 && i == 0) ? "" : ", ",
                                             d, s, names[i % 5]);
                }
            }
        }
        snprintf(payload + used, sizeof(payload) - used, "]}}");
    }
    memset(cap84, 0, sizeof(cap84));
    n = kidpal_schedule_grid_parse(payload, cap84, SGX_COURSE_CAP);
    sgx_require_impl(n, "kidpal_schedule_grid_parse unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(n == SGX_COURSE_CAP,
        sgx_fm("SGX-A4 RED: 84 条全量载荷解析 = %d != 84——KSG_COURSE_CAP 被误缩成"
               "渲染上限或解析面回归（F-8: 缓存/解析 cap 定容 84 不动, 云端仍全量"
               "下发, 截断只发生在渲染层）", n));

    /* slot0（上限 = sgx_slot_caps[0]=4）：must-stay-green 两齿。 */
    sgx_day_band(0, &x0, &x1);
    sgx_fill_slot(cs, 0, 0, sgx_slot_caps[0] - 1);
    sgx_render_strict(cs, sgx_slot_caps[0] - 1, -1, sgx_f_a);
    sgx_fill_slot(cs, 0, 0, sgx_slot_caps[0]);
    sgx_render_strict(cs, sgx_slot_caps[0], -1, sgx_f_b);
    sgx_diff_stats(sgx_f_a, sgx_f_b, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "SGX-A4 RED: 上午第 4 门课未引入任何墨——容量 4 未兑现（不画 = 错误截断）");
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        "SGX-A4 RED: 上午第 4 门 diff 落在 slot0 带外——格内溢出未走截断降级");
    sgx_fill_slot(cs, 0, 0, sgx_slot_caps[0] + 1);
    sgx_render_loose(cs, sgx_slot_caps[0] + 1, -1, sgx_f_c);
    /* 帧对 = (超限帧 f_c, 满容量帧 f_b)——修订轮缺陷 3 修正（旧实现误比 f_a vs
     * f_b, 与紧邻 total>0 同一帧对直接矛盾、逻辑上不可满足）。 */
    sgx_diff_stats(sgx_f_c, sgx_f_b, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        "SGX-A4 RED: 上午第 5 门课改变了帧——超限未静默截断（滚动/提示/溢出第 5 行"
        "皆破坏帧等价; OQ4: 不滚动、不新增提示）");

    /* slot1（上限 = sgx_slot_caps[1]=4）：同 slot0 两齿。 */
    sgx_fill_slot(cs, 1, 1, sgx_slot_caps[1] - 1);
    sgx_render_strict(cs, sgx_slot_caps[1] - 1, -1, sgx_f_a);
    sgx_fill_slot(cs, 1, 1, sgx_slot_caps[1]);
    sgx_render_strict(cs, sgx_slot_caps[1], -1, sgx_f_b);
    sgx_diff_stats(sgx_f_a, sgx_f_b, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "SGX-A4 RED: 下午第 4 门课未引入任何墨——容量 4 未兑现");
    sgx_fill_slot(cs, 1, 1, sgx_slot_caps[1] + 1);
    sgx_render_loose(cs, sgx_slot_caps[1] + 1, -1, sgx_f_c);
    sgx_diff_stats(sgx_f_c, sgx_f_b, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        "SGX-A4 RED: 下午第 5 门课改变了帧——超限未静默截断");

    /* slot2（上限 = sgx_slot_caps[2]=3）：RED 设计态核心（现状上限 4 → 第 4 门
     * 被画）。 */
    sgx_fill_slot(cs, 2, 2, sgx_slot_caps[2] - 1);
    sgx_render_strict(cs, sgx_slot_caps[2] - 1, -1, sgx_f_a);
    sgx_fill_slot(cs, 2, 2, sgx_slot_caps[2]);
    sgx_render_strict(cs, sgx_slot_caps[2], -1, sgx_f_b);
    sgx_diff_stats(sgx_f_a, sgx_f_b, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "SGX-A4 RED: 晚上第 3 门课未引入任何墨——容量 3 未兑现");
    sgx_fill_slot(cs, 2, 2, sgx_slot_caps[2] + 1);
    sgx_render_loose(cs, sgx_slot_caps[2] + 1, -1, sgx_f_c);
    sgx_diff_stats(sgx_f_c, sgx_f_b, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        "SGX-A4 RED: 晚上第 4 门课改变了帧——slot2 上限未收到 3（现状上限 4 = 设计"
        "态红; F-3/OQ4: 第 4 门静默截断, 不滚动不提示）");

    /* tooth 6: containment——slot0 第 4 门 / slot2 第 3 门 diff 全落本格带。 */
    sgx_day_band(0, &x0, &x1);
    sgx_fill_slot(cs, 0, 0, sgx_slot_caps[0] - 1);
    sgx_render_strict(cs, sgx_slot_caps[0] - 1, -1, sgx_f_a);
    sgx_fill_slot(cs, 0, 0, sgx_slot_caps[0]);
    sgx_render_strict(cs, sgx_slot_caps[0], -1, sgx_f_b);
    sgx_diff_stats(sgx_f_a, sgx_f_b, x0, sgx_slot_ys[0][0], x1, sgx_slot_ys[0][1],
                   &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0 && in_band == total,
        "SGX-A4 RED: 上午第 4 门 diff 落在 (day0,slot0) 格带外——截断降级不走溢出");
    /* slot2 半用 day2 课程（fill_slot(cs,2,2,…)）——x 带必须取 day2
     * （修订轮缺陷 4 修正：旧实现沿用 day0 带判 day2 课程, 恒不可满足）。 */
    sgx_day_band(2, &x0, &x1);
    sgx_fill_slot(cs, 2, 2, sgx_slot_caps[2] - 1);
    sgx_render_strict(cs, sgx_slot_caps[2] - 1, -1, sgx_f_a);
    sgx_fill_slot(cs, 2, 2, sgx_slot_caps[2]);
    sgx_render_strict(cs, sgx_slot_caps[2], -1, sgx_f_b);
    sgx_diff_stats(sgx_f_a, sgx_f_b, x0, sgx_slot_ys[2][0], x1, sgx_slot_ys[2][1],
                   &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0 && in_band == total,
        "SGX-A4 RED: 晚上第 3 门 diff 落在 (day2,slot2) 格带外——截断降级不走溢出");
}

/* ========================================================================== *
 * TEST 4 — SG16-A5：临时课下划线 + ※ 退役 + ★ 不变 + 无标记格可见 2 字
 *   夹具 = (day3, slot2)「口语」单课（ARQ-261029：flags bit0 锚定, name 无前缀）。
 *   tooth 1 (infra): mount 16px 态 + 基准帧（flags=0）名墨在格带。
 *   tooth 2 (must-stay-green): 无标记格可见 2 字——名墨列跨 >= 28（16px 两字 32,
 *            14px 兜底态 28 恰过 = 可见性下限; 16px 增益由 SG16-A1 承载）。
 *   tooth 3 (RED 设计态, F-9): temp 帧（bit0=1）vs base 帧 diff 首行 > base 名墨
 *            最大行——标记只在名墨之下（※ 前缀字形/名笔进被占必侵文字行 → 红;
 *            现状画 ※ → 红）。
 *   tooth 4 (RED 设计态): 下划线离散 diff 行数 == 2 且各行 diff ∈ [12..60]
 *            （2px 随名宽; 现状 ※ 字形墨 ~10 行 → 红）。
 *   tooth 5 (RED 设计态): temp/base diff 全落格带（下划线不越带; 现状 slot2 旧带
 *            渲染部分墨落新带外 → 红）。
 *   tooth 6 (must-stay-green): star 帧（bit1=1）vs base diff > 0 落带（★ 可见）。
 *   tooth 7 (must-stay-green): temp 帧 vs star 帧 diff > 0（两标记可区分）。
 *   tooth 8 (RED 设计态): flags=3 帧 vs star 帧 diff > 0 落带（下划线与 ★ 叠加,
 *            星标不被吞）。
 *   tooth 9 (must-stay-green): 固定课不画线——base 帧名墨最大行之下的带内剩余行
 *            严格零墨; 且 base 帧两次渲染逐像素确定（无随机标记）。
 *   tooth 10 (复位): mount(missing reader) 复位兜底态。
 * ========================================================================== */
/* coverage: SG16-A5 -- 临时课下划线（diff 首行 > 名墨最大行 + 2px 随名宽 + 落带）+ ※ 退役（不占笔进）+ ★ 可见可区分叠加 + 无标记格 2 字墨宽 + 固定课不画线 + 确定性 */
TEST_CASE("schedule grid 16px: 临时课下划线与星标 (SG16-A5)",
          "[schedule_grid_16px][SG16-A5][behavior]")
{
    kidpal_schedule_course_t base_f, f1, f2, f3;
    int rc, x0, x1, y0, y1, min_y, max_y, dark, total, in_band;
    int rows, min_row, max_row, m, y, span, min_x, max_x;

    SGX_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "SGX RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    /* tooth 1: mount 16px 态 + 基准帧。 */
    rc = kidpal_font_mount_assets(sgx_host_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        "SGX RED(infra): mount_assets(rc!=0)——P0 底座缺场（同 TEST 1 tooth 0）");
    sgx_day_band(3, &x0, &x1);
    y0 = sgx_slot_ys[2][0];
    y1 = sgx_slot_ys[2][1];
    sgx_render_strict(sgx_course(&base_f, 3, 2, "口语", 0), 1, -1, sgx_f_a);
    sgx_ink_span(sgx_f_a, x0, y0, x1, y1, &min_y, &max_y, &dark);
    TEST_ASSERT_TRUE_MESSAGE(dark > 0 && min_y >= 0,
        "SGX RED(infra): 基准帧名墨缺场——16px 资产缺「口语」字形或格归属失真");

    /* tooth 2: 无标记格可见 2 字。 */
    m = sgx_max_ink_row(sgx_f_a, x0, y0, x1, y1);
    span = sgx_text_col_span(sgx_f_a, x0, y0, x1, m, &min_x, &max_x);
    TEST_ASSERT_TRUE_MESSAGE(span >= SGX_2CHAR_MIN_PX,
        sgx_fm("SGX-A5 RED: 无标记格 2 字课名墨宽 %d < %d——课名可见字数不足"
               "（S-F1.2 补充规则: 无标记格 16px 下可见 2 字）", span,
               SGX_2CHAR_MIN_PX));

    /* temp 帧（bit0=1, name 无前缀）。 */
    sgx_render_strict(sgx_course(&f1, 3, 2, "口语", SGX_FLAG_TEMP), 1, -1, sgx_f_b);

    /* tooth 3: 标记只在名墨之下（※ 前缀退役 + 不占笔进）。 */
    sgx_diff_rows(sgx_f_a, sgx_f_b, x0, y0, x1, y1, &rows, &min_row, &max_row);
    TEST_ASSERT_TRUE_MESSAGE(rows > 0,
        "SGX-A5 RED: 临时课帧与固定课帧 diff 为零——临时标记未实现且 ※ 字形未画"
        "（[F2-M] 未落地, 设计态红）");
    TEST_ASSERT_TRUE_MESSAGE(min_row > m,
        sgx_fm("SGX-A5 RED: 临时标记 diff 首行 %d <= base 名墨最大行 %d——※ 前缀"
               "字形未退役（被画进文字行）或名笔进被占（前缀宽未让位, [F2-M] ①）",
               min_row, m));

    /* tooth 4: 2px 随名宽。 */
    TEST_ASSERT_TRUE_MESSAGE(rows == 2,
        sgx_fm("SGX-A5 RED: 下划线离散 diff 行数 %d != 2——画线高度非 2px（[F2-M] ②: "
               "名墨底缘下方相邻行起 2px 横线）", rows));
    for (y = min_row; y <= max_row; y++) {
        int rd = sgx_row_diff_px(sgx_f_a, sgx_f_b, x0, x1, y);
        TEST_ASSERT_TRUE_MESSAGE(rd >= SGX_UL_MIN_PX && rd <= SGX_UL_MAX_PX,
            sgx_fm("SGX-A5 RED: 下划线行 %d diff 像素 %d 越出 [%d..%d]——画线宽度不随"
                   "名宽（过窄 = 画线缺失 / 过宽 = 整带画线错形, [F2-M] ②）",
                   y, rd, SGX_UL_MIN_PX, SGX_UL_MAX_PX));
    }

    /* tooth 5: 下划线落格带。 */
    sgx_diff_stats(sgx_f_a, sgx_f_b, x0, y0, x1, y1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(in_band == total,
        sgx_fm("SGX-A5 RED: 临时标记帧差 %d/%d 落在 (day3,slot2) 格带外——下划线越带"
               "或渲染越区", total - in_band, total));

    /* tooth 6: ★ 可见（bit1）。 */
    sgx_render_strict(sgx_course(&f2, 3, 2, "口语", SGX_FLAG_STAR), 1, -1, sgx_f_c);
    sgx_diff_stats(sgx_f_a, sgx_f_c, x0, y0, x1, y1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0 && in_band == total,
        "SGX-A5 RED: flags bit1 (星标) 无可见带内 diff——★ 星标缺失或越格"
        "（S-F1.2 AC4; F-9 星标不动）");

    /* tooth 7: 两标记可区分。 */
    sgx_diff_stats(sgx_f_b, sgx_f_c, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "SGX-A5 RED: 临时标记与 ★ 星标帧全同——两标记必须可区分（临时安排 ≠ 当前"
        "接下来星标, 语义混同即失真）");

    /* tooth 8: flags=3 叠加（下划线 + 星标）。 */
    sgx_render_strict(sgx_course(&f3, 3, 2, "口语", SGX_FLAG_TEMP | SGX_FLAG_STAR),
                      1, -1, sgx_f_d);
    sgx_diff_stats(sgx_f_c, sgx_f_d, x0, y0, x1, y1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0 && in_band == total,
        "SGX-A5 RED: flags=3 帧相对 star 帧无带内增量——下划线与 ★ 叠加缺失或星标"
        "被吞（位掩码叠加渲染）");

    /* tooth 9: 固定课不画线 + 确定性。（扫描域 x/y 双向内缩 2px 与
     * sgx_max_ink_row/sgx_text_col_span 一致——修订轮缺陷 6：旧实现扫到带底
     * y=254 网格底线与带左缘 x=193 日列竖线, 与家具恒矛盾。） */
    {
        int clean = 1;
        for (y = m + 1; y <= y1 - 2; y++) {
            int xx;
            for (xx = x0 + 2; xx <= x1 - 2; xx++) {
                if (sgx_f_a[y * SGX_FB_W + xx] != (int32_t)SGX_WHITE) {
                    clean = 0;
                }
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(clean,
            sgx_fm("SGX-A5 RED: base 帧（flags=0）名墨最大行 %d 之下带内仍有墨——固定"
                   "课被画线（[F2-M] ③ 固定不画线义务破）或文字越带", m));
    }
    sgx_render_strict(sgx_course(&base_f, 3, 2, "口语", 0), 1, -1, sgx_f_d);
    sgx_diff_stats(sgx_f_a, sgx_f_d, 0, 0, SGX_FB_W - 1, SGX_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        "SGX-A5 RED: flags=0 renders are not deterministic——无标记课不得随机带标记");

    /* tooth 10: 复位兜底态。 */
    rc = kidpal_font_mount_assets(sgx_missing_reader);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0, "SGX RED(infra): 复位 mount rc!=0");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_font_body() == &font_puhui_14_1,
        "SGX RED(infra): 复位后 body() != font_puhui_14_1——14px 兜底语义被破");
}

/* ========================================================================== *
 * TEST 5 — SG16-A6：FAS-MANIFEST 扩键结构半（构建清单档）
 *   tooth 1: 八新键逐键（键缺 = infra 红自诊断——接线增量未落线；键 0 = 结构红
 *            设计态）。
 *   tooth 2: 两复用键（schedule_ksg_course_cap = F-8、schedule_ksg_font_min_px =
 *            NFR-READ-1 下限在世——must-stay-green）。
 *   tooth 3: 清单面自检（版本键 + 负向探针 + 产物可见）。
 *   RED（设计态）: sched_c_font_seam / sched_c_pitch_not16 / sched_c_no_temp_cp /
 *            sched_c_no_temp_fallback / sched_h_band_anchors 五键 = 0（现状字面
 *            在世/缺位）；接线键缺 = infra 红（acceptance-wiring-patch.md 未落线）。
 * ========================================================================== */
/* coverage: SG16-A6 -- FAS 扩键结构半：kidpal_font_body 直连 + pitch 19 字面 + ※ 码点/兜底退役负向键 + ★ 保留 + 头注新带字面 + CAP=84/MINPX 复用键 + 接线注册 */
TEST_CASE("schedule grid 16px: 结构半-FAS清单扩键 (SG16-A6)",
          "[schedule_grid_16px][SG16-A6][structural]")
{
    SGX_REQUIRE_ROOT();

    /* tooth 1: 八新键。 */
    sgx_require_fact(sgx_mfact("host_srcs_schedule_grid_16px"),
                     "host_srcs_schedule_grid_16px",
                     "schedule_grid_16px.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS"
                     "（acceptance-wiring-patch.md 增量 1）");
    sgx_require_fact(sgx_mfact("sched_c_font_seam"), "sched_c_font_seam",
                     "schedule_page.c 未直连 P0 [K4] kidpal_font_body——课名 16px "
                     "字面源未切（[F2], BUILDER DUTY 1a)）");
    sgx_require_fact(sgx_mfact("sched_c_pitch_macro"), "sched_c_pitch_macro",
                     "schedule_page.c 无 #define KSG_ROW_PITCH——pitch 宏缺失");
    sgx_require_fact(sgx_mfact("sched_c_pitch_not16"), "sched_c_pitch_not16",
                     "schedule_page.c 仍含 KSG_ROW_PITCH 16 旧值字面——pitch 未重基线"
                     "为 19（[F2-L], 设计态红）");
    sgx_require_fact(sgx_mfact("sched_c_no_temp_cp"), "sched_c_no_temp_cp",
                     "schedule_page.c 仍含 ※ 码点字面（0x203B / U+203B）——※ 前缀"
                     "未退役（[F2-M], 设计态红）");
    sgx_require_fact(sgx_mfact("sched_c_no_temp_fallback"), "sched_c_no_temp_fallback",
                     "schedule_page.c 仍含 ksg_marker_temp_proc——程序化 ※ 兜底未"
                     "退役（[F2-M], 设计态红）");
    sgx_require_fact(sgx_mfact("sched_c_star_kept"), "sched_c_star_kept",
                     "schedule_page.c 丢失 0x2605——★ 星标被连带拆除（F-9 星标不动"
                     "铁律破）");
    sgx_require_fact(sgx_mfact("sched_h_band_anchors"), "sched_h_band_anchors",
                     "schedule_page.h 头注版式区钉未随 78/78/69 重基线（缺 "
                     "y[30..107] / y[186..254] 字面, [F2-L], BUILDER DUTY 2)）");

    /* tooth 2: 复用键（F-8 / NFR-READ-1, must-stay-green）。 */
    sgx_require_fact(sgx_mfact("schedule_ksg_course_cap"), "schedule_ksg_course_cap",
                     "schedule_page.h #define KSG_COURSE_CAP 84 锚丢失——F-8 定容"
                     "冻结值被改（缓存/解析 cap 不得缩成渲染上限）");
    sgx_require_fact(sgx_mfact("schedule_ksg_font_min_px"), "schedule_ksg_font_min_px",
                     "schedule_page.h #define KSG_FONT_MIN_PX 14 锚丢失——NFR-READ-1 "
                     "14px 下限语义被改（v1.69: 下限语义不变）");

    /* tooth 3: 清单面自检。 */
    TEST_ASSERT_TRUE_MESSAGE(sgx_mfact("fas_manifest_version") == 1,
        "SGX RED(infra): 清单版本键缺失——FAS 清单机制失灵或版本漂移");
    TEST_ASSERT_TRUE_MESSAGE(sgx_mfact("__sgx_no_such_key__") == -1,
        "SGX RED(infra): 负向探针失败——未知键被误报为在（清单解析失灵）");
    {
        char p[SGX_MAX_PATH];
        TEST_ASSERT_TRUE_MESSAGE(sgx_rel(p, sizeof(p), SGX_FAS_MANIFEST) &&
                                     sgx_is_regular(p),
            "SGX RED(infra): 清单产物不可见——未 build 或路径漂移");
    }
}

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-11-02）:
 *
 *   通道① host runner 全量（本文件未接线前）= schedule_grid.acceptance.c 收紧型
 *     supersede 件红基线（见 taskpackage redBaselineResult——存量 auxbar×3 +
 *     收紧件 TEST 8/9/11 设计态红）; 本文件不在 runner 内、不占 Tests 数。
 *   通道② C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4，命令见头注 Run
 *     protocol）：编译通道 error 为零 = 验收文件本身可编译。
 *   通道③ 接线落线后（builder step-0 现跑）执行通道期望（Tests = 253 + 5 = 258）:
 *     - 本包义务红（设计态, builder 落 BUILDER DUTY 1)-2) 后转绿）:
 *       TEST 1 tooth 1（[F2] 未切——名墨高 14px 域）+ tooth 3（pitch 16 != 19）;
 *       TEST 2 tooth 2/3（分隔线在旧位 103/177）/ tooth 4（格归属旧带）;
 *       TEST 3 tooth 5 后半（slot2 第 4 门被画, 上限未收 3）;
 *       TEST 4 tooth 3/4/5/8（※ 在画/下划线未实现）;
 *       TEST 5 结构红五键（sched_c_font_seam / sched_c_pitch_not16 /
 *       sched_c_no_temp_cp / sched_c_no_temp_fallback / sched_h_band_anchors）。
 *     - MUST-STAY-GREEN at baseline（现状即真）: TEST 1 tooth 0/2(14px 半)/4;
 *       TEST 2 tooth 1/5; TEST 3 tooth 1/2/3/4/6 与 tooth 5 前半; TEST 4 tooth
 *       2/6/7/9; TEST 5 接线键 + 复用两键 + 负向探针。
 *     - 除上述义务红与 p1_auxbar AB-A1/A2/A4 存量外零其它新增红；任何 infra 红
 *       （清单键缺/探针失灵/mount 失败/字形缺场）= 接线或底座缺陷，不冒充设计
 *       态红——停下回报。**零段错误**。
 *   真机半（P2S-E5）: pitch 19 紧凑观感 / 4/4/3 容量观感 / 下划线 ★ 可读性 /
 *     长课截断——本包自有真机轮, 供 P3 终端轮复证; 本文件零 host 齿不伪装。
 * --------------------------------------------------------------------------- */
/* REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-11-02  initial freeze (acceptance-author, TaskPackage
 *               261028-m6-k2b-p2-schedule-16px; 授权链 = 本包 exitCriteria
 *               P2S-E1/E2/E3 + requirements v1.69 版本行 + brief v7 + FROZEN
 *               PROPOSAL F-1/F-3/F-8/F-9 + §3.2 OQ4/OQ2 + impactedTestsSeed)。
 *               seam 形态决策留痕：课名 16px 字面源走 P0 [K4] kidpal_font_body()
 *               直接消费（schedule_page.c 本身 LVGL-ful, 无需 P1 [S7] 型间接缝;
 *               零新头文件、零 changeManifest 新增——与任务面「seam 头文件名可
 *               自定」的偏差已向 planner 申报）。fixture 字形集（语文/数学/口语/
 *               舞蹈/编程/课程/课）全部取自既有冻结件已核验字形（host source_han
 *               子集 ∩ puhui14 兜底面 ∩ 16px 全量面三态在场, p1_home_16px
 *               探针集同源纪律）。16px 墨高窗冻结时点为 [15..18]/[10..14], 修订轮
 *               盒度量合理域判读（非逐字形像素终判）——若 builder 实测越窗且
 *               字形度量正常, 按 tighten-only 窗口以证据重基线, 不静默放宽。
 *   2026-11-02  修订轮（acceptance-author; builder step-0 实现落地后现跑 258 Tests /
 *               7 Failures, 暴露本冻结件 6 处逻辑上不可满足缺陷——builder 以不入库
 *               诊断副本定位; 本轮 = 缺陷修正 + 1 处以证据重基线窗口, 零负锚削弱）:
 *               ① SG16-A1 度量窗以证据重基线（initial freeze 条预授权）:
 *                  [15..18]/[10..14] → [13..16]/[10..13]——builder 实测 16px 资产
 *                  line_height=23/base_line=5、「课」box 15x14、「口」box 12x13,
 *                  CJK 墨高实际 14（P1 同资产同值）。两窗在 13 搭接 → tooth 1 窗口
 *                  对旧 14px 字面不再独立红, 基线判别力由 tooth 2 严格不等式承载。
 *               ② TEST 2 tooth 3 旧位置负锚修正：y=103/177「全行白」→「除竖线家具
 *                  列外全白」（sgx_row_white_outside_vlines）——7 条日列竖线贯穿
 *                  y 0..254, 旧分隔线行必然残留竖线墨（原断言逻辑上不可满足）。
 *                  负锚强度不减：非竖线墨仍一票否决。
 *               ③ TEST 3 三处截断帧等价帧对修正：diff(超限帧 f_c, 满容量帧 f_b)
 *                  ——旧实现误比 f_a vs f_b, 与紧邻 total>0 断言同一帧对直接矛盾,
 *                  逻辑上不可满足。
 *               ④ TEST 3 tooth 6 slot2 containment x 带修正：day2 课程（fill_slot
 *                  (cs,2,2,…)）用 day2 带——旧实现沿用 day0 带, 恒不可满足。
 *               ⑤ helper 家族（缺陷 5+6 + 实跑追加的 min 极值半, builder 诊断后
 *                  本轮以帧证据补全）: sgx_max_ink_row x/y 双向内缩 2px——旧实现
 *                  命中带底 y=254 网格底线与带左缘 x0 竖线 → m 恒为带底 252,
 *                  「标记在名墨之下」与 y>=255 零墨互斥; sgx_text_col_span x 内缩
 *                  2px + min/max 改扫描域**全局**极值——旧实现 min_x = 首枚墨像素、
 *                  max_x = 末次赋值, 两字形不等高（口 12x13 vs 语 14x14）时只有
 *                  较高字形入 min, 实测 span 13 vs 帧证据真实墨宽 30; TEST 4
 *                  tooth 9 扫描域同步双向内缩。
 *               ⑥ 附带：sgx_slot_caps 改由 TEST 3 消费（上限值全部由常量表派生）,
 *                  builder 为此加的 -Wno-error=unused-const-variable 编译豁免不再
 *                  需要, 可摘除。
 *               修订后现跑（2026-11-02, 通道① host runner）: **258 Tests /
 *               3 Failures / 0 Ignored** = 仅存量 p1_auxbar AB-A1/A2/A4（launch-card
 *               基线锚在册）——SG16-A1..A6 全绿, 既有 247 齿零偏移, 零段错误。
 *   2026-11-02  coverage 标记拆分（机检格式, acceptance-coverage-check.mjs 的
 *               hasCoverageMarker 要求标记后紧跟单一断言 ID——TEST 1 合并标记
 *               「coverage: SG16-A1/SG16-A2」拆为 coverage: SG16-A1 + covers:
 *               SG16-A2 两条; 纯注释改动, 断言语义零改动, 现跑通道①复核 258/3
 *               维持）。
 * --------------------------------------------------------------------------- */
