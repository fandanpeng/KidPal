/*
 * Frozen acceptance tests — TaskPackage 260915-m1-n1b-p1-home-battery (subsystem:
 * firmware; M1-N1b; 首页新版式 + 电量 ADC 接线 · mergeOrder 24 · rigor=full).
 *
 * Story: S-F1.1 · 常显首页看「接下来干嘛」（AC1–AC4）。设计权威 = docs/requirements.md
 * S-F1.1（WF-F1-01 v1.4 重排序：辅助条小图标前置 + 「接下来」为主体）+ arch §5.5.4。
 * 电量端到端 owner = 本包独占（crossPackageBehaviors 已登记）：板级 ADC →
 * GetBatteryLevel override → 渲染节点电池注入缝 → 辅助条显示。
 *
 * 现状差距（plan.md 背景摘录）：联网小标链路已实现（基座 UpdateStatusBar 每 10s 刷
 * GetNetworkStateIcon）；电量链路三处断点——板类 adc1_handle/cali_handle/vbat_status
 * 死成员、无 GetBatteryLevel override（board.cc 基座默认 return false）、渲染节点无
 * 电池注入缝（s_battery_pct 恒 100）。P1-A4 系注入假状态验收——本文件 PH-A3 机制齿
 * 必须能区分「真 ADC 数据到达渲染缝」vs「恒值注入」。
 *
 * Nature: HOST-SIDE STRUCTURAL (tree-scan) + BEHAVIORAL (pixel-probe / 决策表) frozen
 * acceptance，key_layer_dispatch.acceptance.c（K1a，同目录先例）+ 归档 home_p1.
 * acceptance.c（FROZEN OFFLINE CONTRACT 先例，260829-home-p1）双范式合体。低电闪烁
 * 机制（doze_state / blink_visible / 1s 周期）沿用归档 home_p1 契约在世实现
 *（firmware/main/display/home_p1.c），本文件把它重新编入 host runner 并钉进新版式。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 弱哨兵
 * 行为级验证；「home =」为冻结落点）:
 *
 *   [1] 渲染节点电池注入缝（home = firmware/main/display/kidpal_render_node.{h,cc}）:
 *       void kidpal_render_node_inject_battery(int battery_pct, int charging);
 *         -- 命名与签名对齐既有 inject_rtc / inject_env 注入缝家族（.h 声明 C linkage
 *            + .cc 定义）。函数体写 s_battery_pct（钳位 0..100，越界丢弃保持原值）与
 *            s_charging（非 0 归 1）。「s_battery_pct 恒 100」销账：100 只保留在声明
 *            初始化器（保守默认值），注入函数是唯二写点。三个 hp1_state 组装点
 *           （rn_render_home_locked / rn_render_kidpal_page_locked HOME 分支 /
 *            kidpal_render_node_tick）继续消费 s_battery_pct / s_charging（现状已在读，
 *            守卫齿钉「不被拆」）。板级在 InitializeSensors 后、inject_rtc/inject_env
 *            同一装配点调用注入（LVGL 就绪后）。
 *       结构齿钉注入缝函数体（region 切区：签名行 → get_page 签名行）内含
 *         s_battery_pct 与 s_charging 写点；且 kidpal_render_node.cc 内
 *         s_battery_pct 出现次数 >= 5（现状 4 = 声明 + 三读、无注入写点 = 恒值注入
 *         在世证据）。
 *
 *   [2] 板级 ADC 接线（home = firmware/main/boards/waveshare-s3-rlcd-4.2/
 *       waveshare-s3-rlcd-4.2.cc，参考树 03_ADC_Test / Adc_GetBatteryLevel 只读重写）:
 *       - CustomBoard override GetBatteryLevel(int &level, bool &charging,
 *         bool &discharging)（board.cc 基座默认 return false 的现状销账）；
 *       - AdcBatteryMonitor（boards/common/adc_battery_monitor.h 基座）实装接入；
 *       - 三个死成员 adc1_handle / cali_handle / vbat_status 接线（各自出现次数
 *         >= 2 = 声明 + 真实使用；现状各恰 1 = 死成员在世证据）；
 *       - kidpal_render_node_inject_battery( 被板级消费（ADC 读数 → 注入缝，
 *         「真数据到达渲染缝」的接线证据；恒值注入无法满足本齿 + [1] 函数体齿）。
 *
 *   [3] 辅助条新版式（home = firmware/main/display/home_p1.c，纯逻辑渲染，WF-F1-01
 *       v1.4 重排序）：400x300 帧，①辅助条 y[0..39] 横贯条，左→右 = 时间 · 日期 ·
 *       温度(图标前置) · 湿度(图标前置，**不裸数字**) · 电量(图标前置)——联网小标
 *       打头由基座 chrome 状态栏承载（LvglDisplay::UpdateStatusBar 消费
 *       GetNetworkStateIcon，链路已实现，本包只钉点 = test 6），不进 home_p1 帧。
 *       ②日程主体：小伴常驻演出（左）+ 「接下来」最近一项最醒目（右）+ 倒计时贴纸；
 *       ③后续事项逐项（小一号）+ 明日预告。布局钉区域矩形（含端点，builder 须把对应
 *       元素画进对应区域；钉区是布局在位钉，不是字体/颜色钉——纯美学留 design review）：
 *
 *         HPB_RECT_AUX_STRIP  {  0,  0, 399,  39 }  辅助条横贯带
 *         HPB_RECT_AUX_TIME   { 36,  6, 108,  29 }  时间 "10:24"（小字）
 *         HPB_RECT_AUX_DATE   {112,  6, 208,  29 }  日期星期（"Www DD Mon"/"DD Mon"）
 *         HPB_RECT_AUX_TEMP   {210,  6, 278,  29 }  温度（整元素, 含 °C 后缀）
 *           HPB_RECT_AUX_TEMP_ICON {210,  6, 230,  29}   图标半（前置）
 *           HPB_RECT_AUX_TEMP_VAL  {234,  6, 278,  29}   数值半（含 °C）
 *         HPB_RECT_AUX_HUM    {282,  6, 338,  29 }  湿度（整元素）
 *           HPB_RECT_AUX_HUM_ICON  {282,  6, 302,  29}   图标半（"45%" 不裸数字）
 *           HPB_RECT_AUX_HUM_VAL   {302,  6, 338,  29}   数值半
 *         HPB_RECT_AUX_BATT   {342,  6, 396,  29 }  电量（整元素）
 *           HPB_RECT_AUX_BATT_ICON {342,  6, 362,  29}   图标半（前置）
 *           HPB_RECT_AUX_BATT_VAL  {364,  6, 396,  29}   数值半
 *         （辅助条钉区底缘 29 = 上虚线 y30-32 重钉后不含虚线行；元素真实墨底 29（电量图标）
 *         ——2026-09-26 对齐轮第二批次随虚线 y33→y30 同钉）
 *         HPB_RECT_MASCOT     {  8, 86, 128, 242 }  小伴常驻演出（2026-09-26 下移 (28,116)）
 *         HPB_RECT_NEXT       {103, 35, 392, 242 }  「接下来」卡整体（R2: 标签带 + 日程行流带）
 *         HPB_RECT_LABEL      {103, 35, 250,  98 }  「接下来」标签带（云朵+烘焙帽标, 固定卡题, R2）
 *         HPB_RECT_ROWS       {152, 68, 392, 133 }  日程行流带（ROW1..ROW3 域, R2;
 *                                                   2026-10-13 补充轮随 22px pitch 重锚）
 *         HPB_RECT_STICKER_NEG {236, 42, 390, 66 }  旧倒计时贴纸/大时间右上区（退役负断言, R2; 2026-09-26 重锚）
 *         HPB_RECT_ROW2       {152, 90, 392, 111 }  后续事项行（行流第二行, R2;
 *                                                   2026-10-13 补充轮随 22px pitch 重锚）
 *         HPB_RECT_MINGZAO    {148,124, 200, 149 }  明早标签槽（帽标独立行, R2; 2026-09-26 重构）
 *         HPB_RECT_PREVIEW_NEG {152,244, 392, 258}  卡底之下退役负断言带（退役负断言, R2;
 *                                                   2026-09-26 x0 152; 2026-10-13 重锚
 *                                                   y220→244 = 卡底 242 + 2px 缓冲）
 *         HPB_RECT_PV_ROW1    {152,155, 392, 176 }  明天区第 1 行带（多行容量回归齿参照齿, 2026-10-13 补充轮）
 *         HPB_RECT_PV_ROW2    {152,177, 392, 198 }  明天区第 2 行带（多行容量回归齿, 2026-10-13 补充轮）
 *         HPB_RECT_PV_ROW3    {152,199, 392, 220 }  明天区第 3 行带（多行容量回归齿, 2026-10-13 补充轮）
 *         HPB_RECT_TODAY_BOT  {152,222, 392, 242 }  今天区卡底残线带（守卫负断言, 2026-10-13 补充轮语义重锚）
 *         HPB_RECT_TOMORROW   {136,118, 392, 242 }  明天区宽域带（两态分叉/containment 判读, 2026-10-13 补充轮重锚）
 *
 *       R2 跨包冲击修订（2026-09-22, 授权 = pod-events c4785230 human-decision
 *       plan-B-decode-fix-plus-wireframe-alignment）: 卡内版式改行流语义，权威钉区
 *       面 = p1_cjk_name.acceptance.c [S3]（本表为其在 HPB 夹具域的投影）——倒计时
 *       贴纸退役（随时钟缺口挂账不做）、大 imgfont 时间退役、「接下来」改烘焙大字
 *       标签（p1_label_jiexialai.bin，固定卡题——空日程仍在场）、后续事项行上移
 *       ROW2 行流带、预告并入主体第三行（明早烘焙标签 p1_label_mingzao.bin 落
 *       MINGZAO 槽 + 时间起笔 x>=184）。上方 ②③ 描述中的贴纸/旧预告独立带为退役
 *       面（STICKER_NEG / PREVIEW_NEG 负断言承载）；兜底义务照旧适用于 LABEL /
 *       MINGZAO 有墨钉区（art 未挂载兜底路径同样有墨）。
 *       兜底义务（host 全量默认跑不挂 art 资产）：每个「有墨」钉区必须在 art 未挂载
 *       的兜底路径下也有墨（描边/兜底字形同款纪律，归档 home_p1 先例）；每个「零墨」
 *       钉区在两条路径下都必须严格零墨。温湿度缺席（env_valid=0）= 整元素隐藏、
 *       不画假数据（归档契约语义沿用）。
 *
 *   [4] 低电闪烁（PH-A4 / AC3，归档 home_p1 契约沿用）：doze = battery_pct <=
 *       HP1_DOZE_BATTERY_THRESHOLD_PCT(15) 且 charging==0；充电恒醒（覆盖任何电量）。
 *       doze 灭相窗（HP1_BLINK_PERIOD_BATTERY_MS=1000，可见窗 3/4 周期）内辅助条电池
 *       **整元素**（图标 + 数值）不画 = 区内严格零墨，亮相恢复；充电即停闪恒画。
 *       **已砍项零触碰**：低电不占小伴姿态通道——home_p1.c / kidpal_render_node.cc
 *       零 pose_sleep 词（§8 已砍清单）。
 *
 *   [5] 范围纪律（PH-E1）：首页日程数据口径声明面在世——home_p1.h schedule API 族
 *       （schedule_inject/count/item/preview 访问族 + day_rollover + countdown;
 *       2026-10-11 起含 preview_count/preview_item 多项访问族, 5 参 inject 终形,
 *       REVISION LOG 2026-10-11 条），本包只动版式与辅助条，不触数据链路（课表数据
 *       接首页归 M3 决策 26）。需求/契约面零修订 canary。
 *
 *   [TR5] 真机轮 live-manual structural 豁免（2026-10-13, TaskPackage
 *       261013-p1-tomorrow-row-sliver-fix; TB-A4 261012 story_page_v2 同款先例口径,
 *       **不计入 runner 通过面**）: human 持板验收四项——①复现场景（今天空 AC5 话术 +
 *       明天 >= 2 项）明天区第 2 项完整字形渲染、无 1px 残线（截图与修复前对比留证）;
 *       ②明天 >= 3 项第 3 项起整行让位、干净不出现; ③今天 >= 7 项卡底 y=242 无残线、
 *       放不下的项整行让位; ④烧录 = app 分区单烧（纯代码变更、无 spiffs 资产变更）。
 *       host 帧像素齿（TEST 4.5 tooth 4/5/6）只钉 host 回归语义（TR-A4/A6/A7/A8）, 不替代
 *       真机探真——真机字形渲染/让位观感仍归本豁免段 human 在场确认; host 侧不为
 *       [TR5] 四项另立像素断言。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: PH-A1 首页版式（接下来主体 + 贴纸退役负断言 + 小伴常驻 + R2 行流
 *             语义）-> test 1（pixel-probe: 主体/贴纸退役/小伴/ROW2 后续事项/
 *             明早槽预告在位 + 旧预告带退役零墨 + 空日程固定卡题在场/行流带零墨 +
 *             旧版式大时间让位判别）
 *   coverage: PH-A2 辅助条小图标前置 -> test 2（pixel-probe: 五元素区 + 温/湿/电
 *             图标半/数值半两两在位 = 图标前置、湿度不裸数字；env 缺席隐藏）+
 *             test 3（结构半: 辅助条渲染消费 hp1_state 电量字段）
 *   coverage: PH-A3 电量 ADC 接线机制齿 -> test 1（行为半: 注入假状态驱动渲染缝——
 *             恒值注入 100 无法满足低电灭相零墨齿）+ test 3（结构半: 注入缝 [1] +
 *             板级接线 [2]，恒值注入现状必红）
 *   coverage: PH-A4 低电闪烁 -> test 1（行为半: ≤15% 未充电每秒闪 + 充电即停）+
 *             test 2（决策表: doze 阈值相对行为 + 1s 闪相位 + 睡姿演出零触碰守卫）
 *   coverage: PH-A5 断网态钉点 -> test 4（WiFi 斜杠链路在世钉点 + chrome 消费守卫 +
 *             首页本地渲染零网络依赖）
 *   coverage: PH-A6 真机确认轮 -> 不在本文件（kind:structural 对象条目，human 持板
 *             ADC 实测；证据落 delivery/changes/dev/260915-m1-n1b-p1-home-battery/）
 *   coverage: PH-A7 host runner 终跑 -> test 5（注册/接线结构守卫）+ 本文件尾
 *             「PH-A7 终跑判读」段（suite 级判读：相对有效基线零新增红 + 本包新齿全绿）
 *   coverage: PH-E1 范围纪律 -> test 5 tooth 2/3（schedule API 面零触碰 + 需求契约
 *             canary）
 *   coverage: HV5-AC5/AC6/AC7 空态三态渲染（2026-10-11, requirements v1.61）->
 *             tooth 5 重钉（AC5 话术在场——空日程不再留白）+ test 4.5（帧分叉:
 *             AC5 话术态 vs rollover 驱动的 AC7 未达态行流带显式分叉; 明天区
 *             preview []/null 两态显式分叉 + 切换不扰他区 + preview 满载 clip
 *             containment; 状态机数据半归 schedule_grid.acceptance.c test 4/4b）
 *   coverage: TR-A1 行流底界对齐卡底（2026-10-13 regression 包
 *             261013-p1-tomorrow-row-sliver-fix, 实现面 = home_p1.c p1_draw_card,
 *             本文件不触实现）-> TEST 4.5 tooth 4（明天区第 2 行带整行容纳回归齿:
 *             pv_cy1 未对齐卡底 242 即红）
 *   coverage: TR-A2 整行容纳守卫（2026-10-13 regression 包
 *             261013-p1-tomorrow-row-sliver-fix, 实现面 = home_p1.c p1_draw_card,
 *             本文件不触实现）-> TEST 4.5 tooth 5（mutation 断言面: 摘整行容纳
 *             守卫必红）
 *   coverage: TR-A4 明天区第 2 行带整行容纳回归齿（2026-10-13 regression 包
 *             261013-p1-tomorrow-row-sliver-fix, 实现面 = home_p1.c p1_draw_card,
 *             本文件不触实现）-> TEST 4.5 tooth 4（mutation 断言面: 改回
 *             pv_cy1=218 必红）
 *   coverage: TR-A3 冻结件钉区重锚 + 授权链留痕 -> 头注表 / 区域注 / TEST 1 tooth 4
 *             与 TEST 4.5 tooth 3 断言消息矩形数值同步 + 文件尾 REVISION LOG
 *             2026-10-13 条（append 不覆盖）
 *   coverage: TR-A5 真机轮 live-manual -> structural 豁免（头注 [TR5] 段在先申报,
 *             TB-A4 261012 story_page_v2 同款先例口径, 不计入 runner 通过面）
 *   coverage: TR-A6 行距参数族压缩（2026-10-13 补充轮, 实现面 = home_p1.c
 *             p1_draw_card, 本文件不触实现）-> TEST 4.5 tooth 4（重锚后明天项行带
 *             155-176 / 177-198 / 199-220 墨达标; 参数族回退 line_h / 组头偏移族任一
 *             形状变更 -> 行带错位红）
 *   coverage: TR-A7 组级容纳门（2026-10-13 补充轮新增语义, 实现面 = home_p1.c
 *             p1_draw_card, 本文件不触实现）-> TEST 4.5 tooth 6（今天 >= 6 项时明天区
 *             整块让位: preview 3 项帧 vs preview 0 项帧在明天区宽域带逐像素相同;
 *             摘组级门 -> 半截帽标入带必红）
 *   coverage: TR-A8 两冻结件重锚批（2026-10-13 补充轮; p1_home_battery.acceptance.c
 *             7 常量 + 齿重锚两处 + p1_cjk_name.acceptance.c 4 常量重锚）-> 头注表 /
 *             区域注 / TEST 1 tooth 4/5 与 TEST 4.5 tooth 3/4/5 消息矩形数值同步 +
 *             文件尾 REVISION LOG 同日补充轮条（append 不覆盖）
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（接线 + 实现落点，HOST_TEST_CONVENTIONS.md [C4.2] 约定）：
 *   1) firmware/main/display/home_p1.c 按 [3] 冻结区域重排版式（辅助条图标前置 +
 *      「接下来」主体 + 兜底义务 + [4] doze 整元素闪 gate）。
 *   2) firmware/main/display/kidpal_render_node.{h,cc} 落 [1] 注入缝（声明 + 定义 +
 *      三组装点消费保持；定义置于 get_page 之前——结构齿切区锚）。
 *   3) firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc 落 [2]
 *      板级接线（AdcBatteryMonitor + GetBatteryLevel override + 死成员接线 +
 *      inject_battery 装配点调用）。
 *   4) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 加 "../../p1_home_battery.acceptance.c" 与
 *      "../../../display/home_p1.c"（行为齿强符号覆盖 weak 哨兵的关键），置于
 *      "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位 = 最先执行）。
 *      art_assets.c 可选接线（挂资产后图标齿探真资产像素；不接则走兜底路径，钉区
 *      有墨义务同样成立）。
 *   5) mutation 自证（mutationRequired=true）：变异命名**不得保留被测符号原字面为
 *      子串**（KR-A3 _TOGGLE_X 教训——strstr 子串仍命中则变异不红；用 _STUB 类整词
 *      替换），mutationEvidence.perAcceptance 逐条回填。
 *      mutation 断言面齿名（2026-10-13 主轮申报, 取证由 builder/fixer 实测、planner
 *      代落）: TEST 4.5 tooth 4（明天区第 2 行带回归齿——改回 pv_cy1=218 必红）+
 *      tooth 5（今天区卡底残线负断言——摘整行容纳守卫必红）+ 重锚 PREVIEW_NEG 负断言
 *      （TEST 1 tooth 4 / TEST 4.5 tooth 3——越卡底溢出必红）。
 *      mutation 断言面齿名（2026-10-13 补充轮申报）: TEST 4.5 tooth 4（多行容量回归齿
 *      ——参数族回退 line_h 22→28 / 组头偏移族 ty+8·ty+13·ty+43 改回旧值 / pv_cy1 回退
 *      -> 行带错位墨 < 阈值必红）+ tooth 5（负断言带 {152,222,392,242}——摘今天区整行
 *      容纳守卫 -> 第 8 项残墨入带必红）+ tooth 6（组级容纳门——摘 if 包裹 -> 今天 >= 6
 *      项时半截帽标入明天区宽域带必红）+ 重锚常量回退（hpb_rect_pv_row2 / pcn_rect_row2
 *      等重锚常量回退旧值 -> 对应行带断言错位必红）。
 *
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见上 BUILDER DUTY。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-
 *      key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *      （git-bash 下 MSYS_NO_PATHCONV=1 必加；idf.py 无 -T 选项）
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-
 *      layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf
 *   Freeze-phase red baseline: 见文件尾「RED baseline 判读口径」段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

/* home_p1 冻结契约头（归档 home_p1.acceptance.c 的在世实现面；形状/常量零偏移）。
 * 2026-10-11 注：home_p1.h 在本包 builder 落地前仍带旧 4 参 inject 声明——本冻结件
 * 按修订后冻结契约（授权链见文件尾 REVISION LOG 2026-10-11 条）钉 5 参终形。宏改写
 * 把头内旧声明改名旁路（仅声明面、无链接语义），使本文件在落地前旧头与落地后新头
 * 两侧都可编（DEV-GOVERNANCE 验收测试纪律 §4 C 类实编通道）。 */
#define kidpal_home_p1_schedule_inject kidpal_home_p1_schedule_inject_hdr_decl
#include "../display/home_p1.h"
#undef kidpal_home_p1_schedule_inject

/* HP1_PREVIEW_MAX（明天区缓存定容冻结值）在 home_p1.h 落地前本文件自持同值；
 * #ifdef 守卫——builder 落地新头后以头内值为准（两侧同值, REVISION LOG 2026-10-11）。 */
#ifndef HP1_PREVIEW_MAX
#define HP1_PREVIEW_MAX 8
#endif
/* art 资产契约头（仅为 weak 哨兵签名一致；被测面 = home_p1.c 消费的 5 原语）。 */
#include "../display/art_assets.h"

/* -------------------------------------------------------------------------- *
 * Frozen numeric codes / regions（seam [3]；帧规格 == home_p1.c P1_FB_*，不得偏移）
 * ------------------------------------------------------------------------ */
#define HPB_UNIMPLEMENTED (-2) /* weak 哨兵 — 永不是合法返回值 */

#define HPB_FRAME_W 400
#define HPB_FRAME_H 300
#define HPB_INK_WHITE 0xFFFFFFul
#define HPB_NOT_RENDERED_SENTINEL ((int32_t)-1)

/* 「有墨」钉区最小墨像素数；整元素区 >= 8、图标/数值半区 >= 4、大区 >= 20。 */
#define HPB_INK_MIN_ELEM 8
#define HPB_INK_MIN_HALF 4
#define HPB_INK_MIN_LARGE 20
/* R2: 「接下来」标签带 / 明早槽在场下界（与 PCN_INK_MIN_LABEL=30 /
 * PCN_INK_MIN_MINGZAO=12 同源同值——兜底描边路径同样须过阈，有墨义务两条路径成立）。 */
#define HPB_INK_MIN_LABEL 30
#define HPB_INK_MIN_MINGZAO 12
/* 2026-10-13 补充轮 TR-A4/A8: 明天区行带整行容纳下界（多行容量回归齿）。阈值
 * 按 R < T < F 纪律重标定（方法 + 实测值见文件尾 REVISION LOG 2026-10-13 补充轮条）:
 * R = 净 HEAD 旧 28px 几何下新行带内的残/错位墨实测 min 三带 = 84（673/246/84,
 * 今天空 + preview 3 项夹具, 帽标 + 首行残墨入带）; F = 完整 14px 行流文本整行墨
 * （同文本族, 主轮同夹具实测锚 157）。T=110 ∈ (84, 157) —— 行带错位（参数族回退
 * line_h/组头偏移/pv_cy1）-> 首/次行带墨坍缩 < T 红 / 整行绿。 */
#define HPB_INK_MIN_PV_ROW2 110

/* 区域矩形 {x1, y1, x2, y2} 含端点（seam [3] 布局在位钉）。
 * 2026-09-26 真机对齐轮重钉（canvas-p1 实测，授权 = human 看屏比对拍图逐项裁定）：
 * 辅助条三元素右移重排（温度 210 / 湿度 282 / 电量 342，温度数值半含 °C 后缀），
 * 钉区底缘 33→31（上虚线重钉 y33-35, 钉区不含虚线行——否则缺席/灭相零墨齿被虚线
 * 墨误触发）；卡区整体上移右移（云 ic_cloud_40_t2 @103,29 / 帽标 @151,41 / 内容列
 * x152 / ROW1 y62）；预告重构为帽标独立行（MINGZAO @148,148）+ 项行在下；
 * STICKER_NEG 重锚到「帽标右 · 虚线下 · 行流上」空档 {236,36,390,60}（旧右上区已
 * 是行流带合法领空）；PREVIEW_NEG x0 136→152（sparkle 装饰墨 x≤145 让位）。
 * 2026-10-13 regression 包重锚（授权链见文件尾 REVISION LOG 2026-10-13 条）:
 * PREVIEW_NEG 上缘 220→244（pv_cy1 对齐卡底 242 后 220..242 段退役为明天区第 2 行
 * 合法域, 负断言语义改「不越卡区底界」）; 新增 PV_ROW2 第 2 行带与 TODAY_BOT 卡底
 * 残线带。
 * 2026-10-13 补充轮重锚（授权链 = TaskPackage 261013 补充轮, 见文件尾 REVISION LOG
 * 同日补充轮条 + sibling preview-round.md）: 行距压缩 line_h 22 / 组头偏移族（虚线
 * ty+8 / 帽标 ty+13 / 项行 ty+43）使 rows/row2/mingzao/tomorrow 及 pv_row1/2 随公式
 * 平移（明天项行带 155/177/199, 今天第 2 行 90-111, 帽标 124-149）; TODAY_BOT 语义
 * 重锚 {152,222,392,242}（今天第 8 项越卡底整行让位带）; 新增 PV_ROW3 第 3 行带;
 * PREVIEW_NEG 及 PCN preview_neg 等零改动清单逐齿验证保持。 */
static const int hpb_rect_aux_strip[4]    = {  0,  0, 399,  39 };
static const int hpb_rect_aux_time[4]     = { 36,  6, 108,  29 };
static const int hpb_rect_aux_date[4]     = {112,  6, 208,  29 };
static const int hpb_rect_aux_temp[4]     = {210,  6, 278,  29 };
static const int hpb_rect_aux_temp_icon[4]= {210,  6, 230,  29 };
static const int hpb_rect_aux_temp_val[4] = {234,  6, 278,  29 };
static const int hpb_rect_aux_hum[4]      = {282,  6, 338,  29 };
static const int hpb_rect_aux_hum_icon[4] = {282,  6, 302,  29 };
static const int hpb_rect_aux_hum_val[4]  = {302,  6, 338,  29 };
static const int hpb_rect_aux_batt[4]     = {342,  6, 396,  29 };
static const int hpb_rect_aux_batt_icon[4]= {342,  6, 362,  29 };
static const int hpb_rect_aux_batt_val[4] = {364,  6, 396,  29 };
static const int hpb_rect_mascot[4]       = {  8, 86, 128, 242 };
static const int hpb_rect_next[4]         = {103, 35, 392, 242 };
/* R2 跨包冲击修订（行流语义，PCN [S3] 对齐）: 旧 STICKER/NEXTLIST/PREVIEW 三个
 * v1.4 正向钉区退役，改标签带 + 日程行流带 + 两退役面负断言区（区域矩形变更的
 * 授权与逐 tooth 说明见头注 R2 注记与文件尾 REVISION LOG；2026-09-26 对齐轮再钉
 * 见上方注记）。 */
static const int hpb_rect_label[4]        = {103, 35, 250,  98 };
static const int hpb_rect_rows[4]         = {152, 68, 392, 133 };
static const int hpb_rect_sticker_neg[4]  = {236, 42, 390,  66 };
static const int hpb_rect_row2[4]         = {152, 90, 392, 111 };
static const int hpb_rect_mingzao[4]      = {148,124, 200, 149 };
/* 2026-10-13 重锚（regression 包 261013, 授权链见 REVISION LOG 同日条）: 上缘
 * 220→244 = 卡底 242 + 2px 缓冲（沿用「合法底界 + 2px」既有纪律）。pv_cy1 对齐卡底
 * 后 220..242 段退役为明天区第 2 行合法域, 负断言语义从「不压旧预告退役带」改
 * 「不越卡区底界」——判别力保持: 任何越卡底溢出仍红（逐齿推演 + 实跑复核见头注
 * 2026-10-13 追记）。 */
static const int hpb_rect_preview_neg[4]  = {152,244, 392, 258 };
/* 2026-10-13 补充轮 TR-A8: 明天区行带族。行距压缩 line_h 28→22 + 组头偏移族
 * (虚线 ty+8 / 帽标 ty+13 / 项行 ty+43) 后, 今天空（AC5 话术, s_count=0）档
 * ty=112 → 明天项行带 155-176 / 177-198 / 199-220（文字 = 带顶 +4..+17）。三带
 * 为 TEST 4.5 tooth 4 多行容量回归齿判读面（阈值见 HPB_INK_MIN_PV_ROW2 注）。 */
static const int hpb_rect_pv_row1[4]      = {152,155, 392, 176 };
static const int hpb_rect_pv_row2[4]      = {152,177, 392, 198 };
static const int hpb_rect_pv_row3[4]      = {152,199, 392, 220 };
/* 2026-10-13 补充轮 TR-A8 语义重锚: 今天区卡底残线带。今天 >= 8 项时第 8 项行带
 * 222..243（文字 226..239）越卡底 242 达 1px——整行容纳守卫落地后第 8 项整行让位、
 * 带 222..242 干净（摘守卫变异 = 第 8 项文字 226..239 入带必红）。 */
static const int hpb_rect_today_bot[4]    = {152,222, 392, 242 };
/* 2026-10-11 HV5: 明天区宽域带; 2026-10-13 补充轮上缘 142→118（对齐紧凑上浮后明天区
 * 真实足迹: s<=2 档虚线 120-122 / 帽标 125-148 / 项行 155-242, 旧 y0=142 漏虚线与帽标
 * 顶）。宽域只用于「两态帧分叉/溢出 containment/组级让位」判读——明天区多项行流的
 * 具体落位是 builder 版式自由度, 不钉行位。 */
static const int hpb_rect_tomorrow[4]     = {136,118, 392, 242 };

/* 板级装配文件（seam [2] home）与渲染节点/显示链被扫文件。 */
#define HPB_BOARD_CC   "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define HPB_RN_H       "firmware/main/display/kidpal_render_node.h"
#define HPB_RN_CC      "firmware/main/display/kidpal_render_node.cc"
#define HPB_HOME_P1_C  "firmware/main/display/home_p1.c"
#define HPB_HOME_P1_H  "firmware/main/display/home_p1.h"
#define HPB_WIFI_CC    "firmware/main/boards/common/wifi_board.cc"
#define HPB_LVGL_CC    "firmware/main/display/lvgl_display/lvgl_display.cc"

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak sentinels（强定义落地后覆盖）。
 * home_p1.c（归档契约实现，在世）由 builder 按第 4) 条接线进 host runner 后，
 * 其强定义覆盖以下哨兵；接线前哨兵返回 HPB_UNIMPLEMENTED，红基线读作
 * 「frozen contract not implemented（host 接线缺失）」——真实缺失行为红，绝非链接
 * 错误。art 原语哨兵使 home_p1.c 单独接线也能链（未接 art_assets.c 时走兜底路径）。
 * ------------------------------------------------------------------------ */
__attribute__((weak)) int kidpal_home_p1_schedule_inject(const char *day_key,
                                                         const hp1_item_t *items,
                                                         int count,
                                                         const hp1_item_t *next_day_items,
                                                         int next_day_count)
{
    (void)day_key; (void)items; (void)count;
    (void)next_day_items; (void)next_day_count;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_schedule_count(void)
{
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) const hp1_item_t *kidpal_home_p1_schedule_item(int index)
{
    (void)index;
    return NULL;
}
__attribute__((weak)) int kidpal_home_p1_schedule_preview_count(void)
{
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) const hp1_item_t *kidpal_home_p1_schedule_preview_item(int index)
{
    (void)index;
    return NULL;
}
__attribute__((weak)) int kidpal_home_p1_day_rollover(const char *now_day)
{
    (void)now_day;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_countdown(const hp1_item_t *items, int count,
                                                   int now_h, int now_m,
                                                   int *out_index, int *out_hours,
                                                   int *out_minutes)
{
    (void)items; (void)count; (void)now_h; (void)now_m;
    (void)out_index; (void)out_hours; (void)out_minutes;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_blink_visible(int tick_ms, int period_ms)
{
    (void)tick_ms; (void)period_ms;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_doze_state(int battery_pct, int charging)
{
    (void)battery_pct; (void)charging;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_set_state(const hp1_state_t *st)
{
    (void)st;
    return HPB_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_home_p1_render_frame(void)
{
    return HPB_UNIMPLEMENTED;
}

/* art 原语哨兵（签名 == art_assets.h；未接 art_assets.c 时 home_p1 兜底路径生效）。 */
__attribute__((weak)) int kidpal_art_mount(const char *root)
{
    (void)root;
    return -1;
}
__attribute__((weak)) int kidpal_art_load(const char *path, kidpal_art_img_t *out)
{
    (void)path; (void)out;
    return -1;
}
__attribute__((weak)) int kidpal_art_pixel(const kidpal_art_img_t *img, int x, int y)
{
    (void)img; (void)x; (void)y;
    return 0;
}
__attribute__((weak)) int kidpal_art_glyph(char ch, kidpal_art_img_t *out)
{
    (void)ch; (void)out;
    return -1;
}
__attribute__((weak)) int kidpal_art_glyph_advance(void)
{
    return 0;
}

/* host 渲染缝（host_app/main/host_infra_seam.c 提供，恒链入 runner）。只探针。 */
int32_t host_infra_read_pixel(int x, int y);

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿，K1a 先例）。全部扫描跑在真实工作树上，
 * root 以手持 cwd 定位（仓根或 firmware/main/test/host_app 均可）。test / host_app /
 * build / .git / assets 目录排除——test 目录住着本冻结文件自身（其注释合法提及 needle）。
 * ------------------------------------------------------------------------ */
#define HPB_MAX_PATH 1024
#define HPB_MAX_READ (8u * 1024u * 1024u)

static char hpb_root[HPB_MAX_PATH];
static int hpb_root_ready;

static int hpb_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int hpb_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int hpb_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int hpb_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[HPB_MAX_PATH];

    if (hpb_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (hpb_is_regular(probe)) {
            snprintf(hpb_root, sizeof(hpb_root), "%s", cands[i]);
            hpb_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int hpb_rel(char *out, size_t cap, const char *rel)
{
    if (!hpb_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", hpb_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", hpb_root, rel);
    return 1;
}

static int hpb_exists(const char *rel)
{
    char p[HPB_MAX_PATH];
    return hpb_rel(p, sizeof(p), rel) && hpb_is_regular(p);
}

static char *hpb_read(const char *rel, size_t *out_len)
{
    char p[HPB_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!hpb_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > HPB_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

static int hpb_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int hpb_file_contains(const char *rel, const char *needle)
{
    char *buf = hpb_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = hpb_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int hpb_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = hpb_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = buf; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    free(buf);
    return count;
}

static int hpb_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                       const char *needle, char *first_hit, size_t cap,
                                       int depth)
{
    DIR *d;
    struct dirent *e;
    int count = 0;

    if (depth > 10) {
        return 0;
    }
    d = opendir(abs_dir);
    if (d == NULL) {
        return 0;
    }
    while ((e = readdir(d)) != NULL) {
        char abs_next[HPB_MAX_PATH];
        char rel_next[HPB_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            hpb_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (rel_prefix[0] != '\0') {
            snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_prefix, e->d_name);
        } else {
            snprintf(rel_next, sizeof(rel_next), "%s", e->d_name);
        }
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += hpb_count_files_with_needle(abs_next, rel_next, needle, first_hit,
                                                 cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > HPB_MAX_READ) {
                continue;
            }
            {
                char *buf = hpb_read(rel_next, NULL);
                if (buf != NULL) {
                    if (hpb_contains(buf, needle)) {
                        if (count == 0 && first_hit != NULL) {
                            snprintf(first_hit, cap, "%s", rel_next);
                        }
                        count++;
                    }
                    free(buf);
                }
            }
        }
    }
    closedir(d);
    return count;
}

static int hpb_scan_count(const char *rel_dir, const char *needle, char *first_hit,
                          size_t cap)
{
    char abs[HPB_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!hpb_rel(abs, sizeof(abs), rel_dir) || !hpb_is_dir(abs)) {
        return 0;
    }
    return hpb_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL
 * (unreadable file / either needle absent). Caller frees. */
static char *hpb_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = hpb_read(rel, NULL);
    char *s, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    e = strstr(s + strlen(start_needle), end_needle);
    if (e == NULL) {
        e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int hpb_count_in_slice(const char *slice, const char *needle)
{
    int count = 0;
    const char *h;
    size_t n;

    if (slice == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = slice; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    return count;
}

#define HPB_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(hpb_locate_root(), \
    "HPB RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen seam not being implemented/wired yet. */
static void hpb_require_implemented(int value, const char *what)
{
    if (value == HPB_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "HPB RED: frozen seam NOT implemented/wired — %s "
                 "(builder must wire firmware/main/display/home_p1.c and this file into "
                 "HOST_ACCEPTANCE_SRCS and land the seam per the contract header in "
                 "firmware/main/test/p1_home_battery.acceptance.c)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（K1a 先例）。 */
static char hpb_fm_buf[1024];
static const char *hpb_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *hpb_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(hpb_fm_buf, sizeof(hpb_fm_buf), fmt, ap);
    va_end(ap);
    return hpb_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * Pixel-probe helper: count ink (non-white, non-sentinel) pixels inside a frozen
 * region. Returns -1 on probe failure (not rendered / seam broken) so callers can
 * fail loudly instead of reading a missing frame as "zero ink".
 * ------------------------------------------------------------------------ */
static int hpb_count_ink(const int rect[4])
{
    int x, y;
    int ink = 0;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = rect[0]; x <= rect[2]; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == HPB_NOT_RENDERED_SENTINEL) {
                return -1;
            }
            if ((uint32_t)px != HPB_INK_WHITE) {
                ink++;
            }
        }
    }
    return ink;
}

/* -------------------------------------------------------------------------- *
 * 2026-10-11 HV5 帧快照/分叉 helpers（additive）：区域帧快照 + 两帧差异像素计数。
 * AC7「数据未达提示与无安排话术显式分叉——孩子分得清」的 render 半证据：两态各渲染
 * 一帧, 帧必须在判读带内可区分（diff > 0）。探针失败（未渲染/越界）返回 -1 让调用方
 * 大声红, 绝不把缺帧读成「零差异」。
 * ------------------------------------------------------------------------ */
static uint32_t hpb_snap_buf[HPB_FRAME_W * HPB_FRAME_H];

static int hpb_snap_region(const int rect[4])
{
    int x, y;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = rect[0]; x <= rect[2]; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == HPB_NOT_RENDERED_SENTINEL) {
                return -1;
            }
            hpb_snap_buf[(size_t)y * HPB_FRAME_W + (size_t)x] = (uint32_t)px;
        }
    }
    return 0;
}

static int hpb_diff_region(const int rect[4])
{
    int x, y;
    int diff = 0;

    for (y = rect[1]; y <= rect[3]; y++) {
        for (x = rect[0]; x <= rect[2]; x++) {
            int32_t px = host_infra_read_pixel(x, y);
            if (px == HPB_NOT_RENDERED_SENTINEL) {
                return -1;
            }
            if ((uint32_t)px !=
                hpb_snap_buf[(size_t)y * HPB_FRAME_W + (size_t)x]) {
                diff++;
            }
        }
    }
    return diff;
}

/* Fixture: full-state inject（时间/温湿度/电量/充电/tick 全注入，P1-E2 硬件殿后纪律）。 */
static void hpb_set_fixture(int hour, int minute, int env_valid, int temp_c,
                            int humidity_pct, int battery_pct, int charging,
                            int tick_ms)
{
    hp1_state_t st;
    int rc;

    memset(&st, 0, sizeof(st));
    st.hour = hour;
    st.minute = minute;
    /* 星期显式钉「未知」（2026-09-26 对齐轮）：本夹具不涉日期（month/mday 恒 0 →
     * 日期走兜底描边），memset 后的 0 会静默等于「周一」——显式 -1 = 语义「未知」,
     * 不静默拿假星期（wday 零值 = 周一 属结构 footgun, 新夹具一律显式赋值）。 */
    st.wday = -1;
    st.env_valid = env_valid;
    st.temp_c = temp_c;
    st.humidity_pct = humidity_pct;
    st.battery_pct = battery_pct;
    st.charging = charging;
    st.tick_ms = tick_ms;
    rc = kidpal_home_p1_set_state(&st);
    hpb_require_implemented(rc, "kidpal_home_p1_set_state unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): set_state fixture rejected (rc=%d)——校验门把合法夹具"
               "当非法，渲染面无法驱动", rc));
}

/* Fixture: schedule inject（2 项今日日程 + 次日 preview 1 项列表, producer 分组结果
 * verbatim; 2026-10-11 seam 终形 5 参——preview 由单指针改数组+计数, REVISION LOG
 * 2026-10-11 条）。 */
static void hpb_inject_schedule_two_items(void)
{
    hp1_item_t items[2];
    hp1_item_t pv[1];
    int rc;

    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    snprintf(items[1].name, sizeof(items[1].name), "英语培训");
    memset(pv, 0, sizeof(pv));
    snprintf(pv[0].time, sizeof(pv[0].time), "09:00");
    snprintf(pv[0].name, sizeof(pv[0].name), "游泳课");

    rc = kidpal_home_p1_schedule_inject("2026-09-15", items, 2, pv, 1);
    hpb_require_implemented(rc, "kidpal_home_p1_schedule_inject unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): schedule_inject fixture rejected (rc=%d)", rc));
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — PH-A1/PH-A3/PH-A4 行为半：注入缝驱动渲染 + 新版式主体在位
 * 「接下来」主体（标签带 + 行流带）+ 贴纸退役负断言 + 小伴常驻 + ROW2 后续事项/
 * 第三行预告 + 旧版式大时间让位判别（PH-A1，R2 行流语义重钉）；
 * 电池注入缝假状态驱动渲染缝（PH-A3 行为半——恒值注入 100 无法满足低电灭相零墨齿，
 * P1-A4 注入假状态教训落齿）；低电 ≤15% 未充电每秒闪 + 充电即停（PH-A4 行为半）。
 * RED today: home_p1.c 未接进 host runner（weak 哨兵 -2）。
 * ------------------------------------------------------------------------ */
/* coverage: PH-A1 -- 新版式主体在位（R2 行流语义）：接下来卡（标签带+行流带）+贴纸退役负断言+小伴常驻+ROW2 后续事项+明早槽/旧预告带退役 + 空日程固定卡题在场/行流带零墨 + 旧版式大时间让位判别 */
/* coverage: PH-A3 -- 电池注入缝行为半：注入假状态驱动渲染缝——低电灭相窗零墨齿，恒值注入(100)必红 */
/* coverage: PH-A4 -- 低电闪烁行为半：≤15% 未充电 1s 闪 + 充电即停恒画 */
TEST_CASE("首页电量: 注入缝驱动渲染+新版式主体+低电闪停 (PH-A1/PH-A3/PH-A4)",
          "[p1_home_battery][PH-A1][PH-A3][PH-A4][behavior]")
{
    int ink;

    /* ---- fixture A: 有日程 + 电量 85（PH-A1 主体在位） ---------------------- */
    hpb_inject_schedule_two_items();
    hpb_set_fixture(10, 24, 1, 22, 45, 85, 0, 0);
    {
        int rc = kidpal_home_p1_render_frame();
        hpb_require_implemented(rc, "kidpal_home_p1_render_frame unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame rc=%d——P1 帧渲染失败", rc));
    }

    /* tooth 1 (PH-A1): 「接下来」最近一项主体区在位（最醒目）。 */
    ink = hpb_count_ink(hpb_rect_next);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_LARGE,
        hpb_fm("HPB-A1 RED: 接下来主体区墨 %d < %d——最近一项主体未落新版式钉区 "
               "(WF-F1-01 v1.4 ②)", ink, HPB_INK_MIN_LARGE));

    /* tooth 2 (PH-A1, R2): 倒计时贴纸退役——旧贴纸/大时间右上区任何夹具严格零墨
     * （260921 追加轮 human 拍板: 贴纸不做随时钟缺口挂账、大 imgfont 时间退役、
     * 预告不入右上独立区; 口径对齐 PCN [S3] STICKER_NEG = PCN TEST 8 tooth 2）。 */
    ink = hpb_count_ink(hpb_rect_sticker_neg);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        hpb_fm("HPB-A1 RED: 旧贴纸/大时间右上区 {236,36,390,60} 墨 %d != 0——倒计时"
               "贴纸/大 imgfont 时间未退役，或预告被放进右上独立区（R2 版式退役面"
               "在世渲染, AC1）", ink));

    /* tooth 3 (PH-A1): 小伴常驻演出区在位（姿态/表情保留，非冷冰冰任务清单）。 */
    ink = hpb_count_ink(hpb_rect_mascot);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_LARGE,
        hpb_fm("HPB-A1 RED: 小伴常驻演出区墨 %d < %d——常驻演出被新版式挤掉 (AC1)",
               ink, HPB_INK_MIN_LARGE));

    /* tooth 4 (PH-A1, R2): 后续事项行 + 末行预告新版式语义——后续事项（原③逐项）
     * 上移 ROW2 行流带在位; 预告并入主体第三行（明早标签槽 {148,124,200,149} 在场
     * + 旧预告独立带标准夹具严格零墨）。预告时间起笔 x>=184 的构成判别由 PCN
     * TEST 8 tooth 4 承载（帧 diff 机制），本齿钉在场与退役面。
     * 2026-10-13 补充轮: ROW2/MINGZAO 随 22px pitch 重锚（90-111 / 124-149）——净
     * HEAD（28px）下 ROW2 半带残墨 >= 8 仍绿、MINGZAO 带错位零墨红（设计态）。 */
    ink = hpb_count_ink(hpb_rect_row2);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A1 RED: 后续事项行 ROW2 {152,90,392,111} 墨 %d < %d——后续事项"
               "未上行行流第二行（R2 版式未落地）", ink, HPB_INK_MIN_ELEM));
    ink = hpb_count_ink(hpb_rect_mingzao);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_MINGZAO,
        hpb_fm("HPB-A1 RED: 明早标签槽 {148,124,200,149} 墨 %d < %d——预告未并入主体"
               "第三行（明早烘焙标签缺失, R2 版式未落地）", ink,
               HPB_INK_MIN_MINGZAO));
    ink = hpb_count_ink(hpb_rect_preview_neg);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        hpb_fm("HPB-A1 RED: 卡底之下重锚负断言带 {152,244,392,258} 墨 %d != 0——"
               "墨越出卡区底界 y242（旧预告独立带版式在世 R2 退役面, 或渲染溢出越"
               "卡底; 2026-10-13 重锚语义「不越卡区底界」）", ink));

    /* tooth 5 (PH-A1, R2; 2026-10-11 AC5 语义重钉): 空日程——「接下来」烘焙大字标签
     * 为固定卡题仍在场（LABEL 带墨 >= 30）+ 日程行流带 {152,68,392,133} 墨 >= 8 =
     * AC5 无安排话术「今天接下来没有安排啦」在场（requirements v1.61: 「不再整区
     * 留白」——supersede R2 行流门控空语义; 授权链见 REVISION LOG 2026-10-11 条）。
     * 话术与数据未达提示的显式分叉（AC7）由文件尾 HV5 CASE 帧分叉齿承载; 旧版式
     * 大时间越界判别语义仍由 ROW1 落带与辅助条时间齿（TEST 3 tooth 1）分头承载。 */
    {
        int rc;
        rc = kidpal_home_p1_schedule_inject("2026-09-15", NULL, 0, NULL, -1);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): schedule_inject(count=0) rc=%d——「今日无待办」"
                   "确认空（AC5 话术态数据半）路径回归", rc));
        hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
        rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(empty) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_label);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_LABEL,
        hpb_fm("HPB-A1 RED: 空日程标签带 {103,35,250,98} 墨 %d < %d——「接下来」固定"
               "卡题未在场（R2 烘焙标签版式未落地）", ink, HPB_INK_MIN_LABEL));
    ink = hpb_count_ink(hpb_rect_rows);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A5 RED: 空日程行流带 {152,68,392,133} 墨 %d < %d——AC5 无安排话术"
               "未渲染、整区留白（requirements v1.61「不再整区留白」, 旧行流门控空"
               "语义已随 v1.61 退役）", ink, HPB_INK_MIN_ELEM));

    /* ---- fixture B: PH-A3 行为半 + PH-A4 行为半（电池注入缝驱动渲染缝） ---- */
    /* tooth 6 (PH-A4): 低电 10% 未充电，灭相窗内（tick=900，周期 1000 可见窗 3/4）
     * 电池整元素（图标+数值）零墨——「显示 100」恒值注入现状对本齿必红
     * (P1-A4 注入假状态教训: 断言必须能区分真数据到达渲染缝 vs 恒值注入)。 */
    hpb_set_fixture(10, 24, 1, 22, 45, 10, 0, 900);
    {
        int rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(doze off-phase) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_aux_batt);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        hpb_fm("HPB-A4 RED: 低电灭相窗电池区墨 %d != 0——doze 闪烁未门控渲染，或"
               "电量恒值注入(100)未销账 (AC3)", ink));

    /* tooth 7 (PH-A4): 同状态亮相窗（tick=0）电池区在位——闪 = 相位门控而非消失。 */
    hpb_set_fixture(10, 24, 1, 22, 45, 10, 0, 0);
    {
        int rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(doze on-phase) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_aux_batt);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A4 RED: 低电亮相窗电池区墨 %d < %d——电池元素丢失（闪变成熄灭）",
               ink, HPB_INK_MIN_ELEM));

    /* tooth 8 (PH-A4): 充电即停闪——低电 + charging=1 + 灭相窗 tick 仍恒画 (AC3)。 */
    hpb_set_fixture(10, 24, 1, 22, 45, 10, 1, 900);
    {
        int rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(charging) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_aux_batt);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A4 RED: 充电态灭相窗电池区墨 %d < %d——充电「叫醒」未覆盖 "
               "doze 灭相 (AC3)", ink, HPB_INK_MIN_ELEM));

    /* tooth 9 (PH-A4): 非低电恒画——85% + 灭相窗 tick 不闪。 */
    hpb_set_fixture(10, 24, 1, 22, 45, 85, 0, 900);
    {
        int rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(normal batt) rc=%d", rc));
    }
    ink = hpb_count_ink(hpb_rect_aux_batt);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A4 RED: 非低电电池区墨 %d < %d——doze 误伤正常电量显示", ink,
               HPB_INK_MIN_ELEM));
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — PH-A4 决策表 + 1s 闪相位 + 已砍项零触碰守卫
 * doze_state 阈值相对行为（≤15 触发、充电恒醒、防御 -1）+ blink_visible 1s 周期
 * 相位窗 + pose_sleep 零命中（低电不占姿态通道，§8 已砍）。
 * ------------------------------------------------------------------------ */
/* coverage: PH-A4 -- 低电打盹决策表：阈值相对行为+充电叫醒+防御；1s 闪相位窗；pose_sleep 零触碰守卫 */
TEST_CASE("首页电量: 低电决策表+闪相位+睡姿零触碰 (PH-A4)",
          "[p1_home_battery][PH-A4][decision]")
{
    int d;

    d = kidpal_home_p1_doze_state(10, 0);
    hpb_require_implemented(d, "kidpal_home_p1_doze_state unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(d == 1,
        hpb_fm("HPB-A4 RED: doze_state(10,0)=%d 期望 1——电量低于阈值未触发低电态 "
               "(AC3)", d));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(HP1_DOZE_BATTERY_THRESHOLD_PCT, 0) == 1,
        "HPB-A4 RED: doze_state(15,0)!=1——阈值边界（<= 触发）漂移");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(HP1_DOZE_BATTERY_THRESHOLD_PCT + 1, 0) == 0,
        "HPB-A4 RED: doze_state(16,0)!=0——阈值上方误触发");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(100, 0) == 0,
        "HPB-A4 RED: doze_state(100,0)!=0——满电误触发");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(0, 1) == 0 &&
                                 kidpal_home_p1_doze_state(100, 1) == 0,
        "HPB-A4 RED: 充电态 doze!=0——充电「叫醒」未覆盖任何电量 (AC3)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_doze_state(-1, 0) == -1 &&
                                 kidpal_home_p1_doze_state(101, 0) == -1 &&
                                 kidpal_home_p1_doze_state(50, 2) == -1,
        "HPB-A4 RED: 非法入参未防御（-1）——电量/充电越界值被静默接受");

    /* 1s 闪相位窗（与冒号眨眼共用相位函数；可见窗 = 周期 3/4）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_blink_visible(0, HP1_BLINK_PERIOD_BATTERY_MS) == 1 &&
                                 kidpal_home_p1_blink_visible(749, HP1_BLINK_PERIOD_BATTERY_MS) == 1 &&
                                 kidpal_home_p1_blink_visible(750, HP1_BLINK_PERIOD_BATTERY_MS) == 0 &&
                                 kidpal_home_p1_blink_visible(999, HP1_BLINK_PERIOD_BATTERY_MS) == 0 &&
                                 kidpal_home_p1_blink_visible(1000, HP1_BLINK_PERIOD_BATTERY_MS) == 1,
        "HPB-A4 RED: blink_visible 1s 周期相位窗漂移——每秒闪的灭窗不在 [750,1000)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_blink_visible(5, 0) == 1 &&
                                 kidpal_home_p1_blink_visible(-1, 1000) == 1,
        "HPB-A4 RED: blink_visible 防御语义漂移（period<=0 / tick<0 须恒可见）");

    /* 已砍项零触碰守卫：低电不占小伴姿态通道（§8 已砍清单）。 */
    HPB_REQUIRE_ROOT();
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_in_file(HPB_HOME_P1_C, "pose_sleep") == 0 &&
                                 hpb_count_in_file(HPB_RN_CC, "pose_sleep") == 0,
        "HPB-A4 RED: 首页渲染面出现 pose_sleep——低电打盹演出被复活（§8 已砍项"
        "零触碰红线：低电只保留辅助条电池图标每秒闪最小信号）");
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — PH-A2/PH-A3 结构半：辅助条图标前置 pixel-probe + 注入缝/板级接线 tree-scan
 * pixel 半（fixture: env 有效 + 电量 85）：辅助条五元素区 + 温/湿/电图标半与数值半
 * 两两在位（图标前置 = 「湿度 45% 不裸数字」的结构判别）+ env 缺席隐藏；tree 半：
 * 注入缝 [1]（.h 声明 + .cc 函数体写点 + 三文件消费链 + s_battery_pct >= 5）+
 * 板级接线 [2]（AdcBatteryMonitor + GetBatteryLevel override + 三死成员各 >= 2）。
 * ------------------------------------------------------------------------ */
/* coverage: PH-A2 -- 辅助条图标前置：五元素区在位 + 温/湿/电图标半+数值半两两在位（湿度不裸数字）+ env 缺席隐藏 */
/* coverage: PH-A3 -- 电量 ADC 接线结构齿：注入缝符号级（.h+.cc 函数体写点+三文件消费链+s_battery_pct>=5）+ 板级 override+死成员接线 */
TEST_CASE("首页电量: 辅助条图标前置+ADC接线结构 (PH-A2/PH-A3)",
          "[p1_home_battery][PH-A2][PH-A3][structural]")
{
    char hit[HPB_MAX_PATH];
    char *region;
    int n, ink, rc;

    /* ---- pixel 半: 辅助条五元素 + 图标半/数值半 ----------------------------- */
    hpb_inject_schedule_two_items();
    hpb_set_fixture(10, 24, 1, 22, 45, 85, 0, 0);
    rc = kidpal_home_p1_render_frame();
    hpb_require_implemented(rc, "kidpal_home_p1_render_frame unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame rc=%d", rc));

    /* tooth 0: 辅助条横贯带整体有墨（y[0..39] 版式带存在）。 */
    ink = hpb_count_ink(hpb_rect_aux_strip);
    TEST_ASSERT_TRUE_MESSAGE(ink >= 30,
        hpb_fm("HPB-A2 RED: 辅助条横贯带墨 %d < 30——y[0..39] 版式带缺失 (v1.4 ①)",
               ink));

    /* tooth 1: 时间/日期区在位（辅助条左段，v1.4 重排序落位）。 */
    ink = hpb_count_ink(hpb_rect_aux_time);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A2 RED: 辅助条时间区墨 %d < %d——时间未收进辅助条 (v1.4 ①)",
               ink, HPB_INK_MIN_ELEM));
    ink = hpb_count_ink(hpb_rect_aux_date);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HPB-A2 RED: 辅助条日期区墨 %d < %d——日期未收进辅助条", ink,
               HPB_INK_MIN_ELEM));

    /* tooth 2: 温度元素 = 图标半 + 数值半两两在位（图标前置）。 */
    ink = hpb_count_ink(hpb_rect_aux_temp_icon);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条温度图标半区墨 %d < %d——温度缺前置小图标", ink,
               HPB_INK_MIN_HALF));
    ink = hpb_count_ink(hpb_rect_aux_temp_val);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条温度数值半区墨 %d < %d——温度数值缺失", ink,
               HPB_INK_MIN_HALF));

    /* tooth 3 (AC1 湿度不裸数字): 湿度元素 = 图标半 + 数值半两两在位。 */
    ink = hpb_count_ink(hpb_rect_aux_hum_icon);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条湿度图标半区墨 %d < %d——湿度 45%% 裸数字挨电池"
               "（缺前置小图标）", ink, HPB_INK_MIN_HALF));
    ink = hpb_count_ink(hpb_rect_aux_hum_val);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条湿度数值半区墨 %d < %d——湿度数值缺失", ink,
               HPB_INK_MIN_HALF));

    /* tooth 4: 电量元素 = 图标半 + 数值半两两在位。 */
    ink = hpb_count_ink(hpb_rect_aux_batt_icon);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条电池图标半区墨 %d < %d——电量缺前置小图标", ink,
               HPB_INK_MIN_HALF));
    ink = hpb_count_ink(hpb_rect_aux_batt_val);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_HALF,
        hpb_fm("HPB-A2 RED: 辅助条电池数值半区墨 %d < %d——电量数值缺失", ink,
               HPB_INK_MIN_HALF));

    /* tooth 5: env 缺席（env_valid=0）= 温/湿整元素隐藏、不画假数据；时间/日期/
     * 电量照常（断网/缺传感时本地数据照常的渲染半，服务 PH-A5）。 */
    hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame(env absent) rc=%d", rc));
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_ink(hpb_rect_aux_temp) == 0 &&
                                 hpb_count_ink(hpb_rect_aux_hum) == 0,
        "HPB-A2 RED: env_valid=0 时温/湿区仍有墨——缺席未隐藏或画了假数据"
        "（归档 home_p1 契约: 缺席即隐藏）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_ink(hpb_rect_aux_time) >= HPB_INK_MIN_ELEM &&
                                 hpb_count_ink(hpb_rect_aux_batt) >= HPB_INK_MIN_ELEM,
        "HPB-A2 RED: env 缺席时时间/电量区失墨——本地数据未照常显示");

    /* tooth 6 (2026-09-26 对齐轮新增): 星期字段注入门与无星期回退——wday=-1（未注入/
     * 未知）合法且渲染不画假星期; 越域 wday=7 必须被 set_state 拒（假星期不可注入）。
     * 回退渲染面 = 日期区退回 "DD Mon"（本齿钉门与值域, 回退位差由 PCN 日期路径齿
     * 与真机看板承接）。 */
    {
        hp1_state_t st;
        memset(&st, 0, sizeof(st));
        st.hour = 10;
        st.minute = 24;
        st.month = 9;
        st.mday = 22;
        st.wday = -1;
        st.env_valid = 0;
        st.battery_pct = 85;
        st.tick_ms = 0;
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
            "HPB-A2 RED: set_state 拒了 wday=-1——无星期回退路径被注入门误杀");
        st.wday = 7;
        TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == -1,
            "HPB-A2 RED: set_state 收了越域 wday=7——假星期值可注入渲染面");
        /* 复位夹具状态（后续齿沿用 env 缺席夹具语义）。 */
        hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
    }

    /* ---- tree 半: 注入缝 [1] + 板级接线 [2] -------------------------------- */
    HPB_REQUIRE_ROOT();

    /* tooth 6: 注入缝声明落 .h（消费面 C linkage，inject_rtc/inject_env 家族同款）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_RN_H,
                                               "kidpal_render_node_inject_battery"),
        "HPB-A3 RED: kidpal_render_node.h 未声明 kidpal_render_node_inject_battery——"
        "渲染节点电池注入缝缺失（s_battery_pct 恒 100 现状在世）");

    /* tooth 7: 注入缝函数体写 s_battery_pct / s_charging（真写点，非空壳）。
     * 冻结落点：定义置于 get_page 之前（结构齿切区锚，BUILDER DUTY 2))。 */
    region = hpb_region_between(HPB_RN_CC,
                                "void kidpal_render_node_inject_battery(",
                                "int kidpal_render_node_get_page(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "HPB-A3 RED: kidpal_render_node.cc 的 inject_battery 函数区切取失败——"
        "定义缺失，或未置于 get_page 之前（冻结落点见 seam [1]）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_in_slice(region, "s_battery_pct") >= 1 &&
                                 hpb_count_in_slice(region, "s_charging") >= 1,
        "HPB-A3 RED: inject_battery 函数体未写 s_battery_pct/s_charging——注入缝"
        "空壳，电池数据到不了渲染状态（恒值注入在世）");
    free(region);

    /* tooth 8: 注入缝三文件消费链（.h 声明 + .cc 定义 + 板级装配点调用）>= 3 文件。 */
    n = hpb_scan_count("firmware/main", "kidpal_render_node_inject_battery(", hit,
                       sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        hpb_fm("HPB-A3 RED: kidpal_render_node_inject_battery( 仅 %d 文件引用"
               "（期望 >= 3: 声明 + 定义 + 板级调用）——ADC 读数到渲染缝的接线链断裂",
               n));

    /* tooth 9: 板级 override + AdcBatteryMonitor 实装（基座默认 false 销账）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_BOARD_CC, "GetBatteryLevel") &&
                                 hpb_file_contains(HPB_BOARD_CC, "AdcBatteryMonitor"),
        "HPB-A3 RED: 板类无 GetBatteryLevel override / AdcBatteryMonitor 实装——"
        "board.cc 基座默认 return false 的现状未销账，真机电量仍取不到");

    /* tooth 10: 三死成员接线（各出现 >= 2 = 声明 + 真实使用；现状各恰 1 = 死成员）。 */
    n = hpb_count_in_file(HPB_BOARD_CC, "adc1_handle");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        hpb_fm("HPB-A3 RED: 板类 adc1_handle 仅 %d 处出现（期望 >= 2）——死成员未接线",
               n));
    n = hpb_count_in_file(HPB_BOARD_CC, "cali_handle");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        hpb_fm("HPB-A3 RED: 板类 cali_handle 仅 %d 处出现（期望 >= 2）——死成员未接线",
               n));
    n = hpb_count_in_file(HPB_BOARD_CC, "vbat_status");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        hpb_fm("HPB-A3 RED: 板类 vbat_status 仅 %d 处出现（期望 >= 2）——死成员未接线",
               n));

    /* tooth 11 (guard): s_battery_pct 出现次数 >= 5（声明 + 注入写点 + 三组装点读；
     * 现状 4 = 恒值注入在世证据）。组装点读被拆 = 渲染面拿不到电池数据。 */
    n = hpb_count_in_file(HPB_RN_CC, "s_battery_pct");
    TEST_ASSERT_TRUE_MESSAGE(n >= 5,
        hpb_fm("HPB-A3 RED: kidpal_render_node.cc s_battery_pct 仅 %d 处（期望 "
               ">= 5: 声明+注入写点+三组装点读）——注入写点缺失（恒值注入）或组装点"
               "读被拆", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — PH-A5 断网态钉点（链路已实现，本包只钉点）
 * WiFi 斜杠链路在世钉点（wifi_board.cc GetNetworkStateIcon 断网分支）+ chrome 消费
 * 守卫（lvgl_display.cc UpdateStatusBar 每 10s 刷）+ 首页本地渲染零网络依赖
 * （断网时时间/温湿度/电量照常本地显示的渲染半）。
 * ------------------------------------------------------------------------ */
/* coverage: PH-A5 -- 断网态钉点：GetNetworkStateIcon 断网 WiFi-slash 分支在世 + UpdateStatusBar 消费守卫 + 首页本地渲染零网络依赖 */
TEST_CASE("断网态: WiFi斜杠钉点+本地数据照常 (PH-A5)",
          "[p1_home_battery][PH-A5][structural]")
{
    char *region;

    HPB_REQUIRE_ROOT();

    /* tooth 1: 断网分支在世——GetNetworkStateIcon 函数区内含 !IsConnected ->
     * FONT_AWESOME_WIFI_SLASH（AC4 联网小标变断网态的链路事实源）。 */
    region = hpb_region_between(HPB_WIFI_CC,
                                "const char* WifiBoard::GetNetworkStateIcon()",
                                "std::string WifiBoard::GetBoardJson");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "HPB-A5 RED(infra): wifi_board.cc 的 GetNetworkStateIcon 函数区切取失败"
        "（签名锚漂移——链路被改动须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_contains(region, "FONT_AWESOME_WIFI_SLASH") &&
                                 hpb_contains(region, "IsConnected"),
        "HPB-A5 RED: GetNetworkStateIcon 断网分支（!IsConnected -> WiFi_SLASH）"
        "丢失——断网时联网小标不再变 WiFi 斜杠 (AC4)");
    free(region);

    /* tooth 2 (guard): chrome 消费守卫——lvgl_display.cc UpdateStatusBar 消费
     * GetNetworkStateIcon（基座每 10s 刷联网小标；「联网小标打头」的承载位）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_LVGL_CC, "GetNetworkStateIcon"),
        "HPB-A5 RED: lvgl_display.cc 不再消费 GetNetworkStateIcon——联网小标刷新链路"
        "被拆（PH-A5 只钉点、零触碰红线）");

    /* tooth 3: 首页本地渲染零网络依赖——home_p1.c 无网络词（断网时时间/温湿度/
     * 电量照常本地显示、日程区不更新的渲染半；网络态由 chrome 层承载）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_count_in_file(HPB_HOME_P1_C, "GetNetworkStateIcon") == 0 &&
                                 hpb_count_in_file(HPB_HOME_P1_C, "websocket") == 0 &&
                                 hpb_count_in_file(HPB_HOME_P1_C, "esp_http") == 0,
        "HPB-A5 RED: home_p1.c 出现网络依赖词——首页本地渲染被网络门控（断网时"
        "本地数据必须照常显示，NFR-OFF-1 / AC4）");
}

/* -------------------------------------------------------------------------- *
 * TEST 4.5 (2026-10-11 HV5) — 空态三态渲染分叉：AC5 话术 vs AC7 未达提示 vs AC6
 *   明天区话术/未达 提示的显式分叉（帧分叉齿）+ 明天区多项溢出 containment。
 *   状态机数据半（wire 三态 → preview_count 三态映射、rollover 清缓存转未达态）由
 *   schedule_grid.acceptance.c test 4/4b 承载; 本 CASE 钉渲染可观察半。
 * ------------------------------------------------------------------------ */
/* coverage: HV5-AC5 -- 无安排话术 vs 数据未达提示在「接下来」行流带显式分叉（帧 diff）+ rollover 驱动跨 0:00 转未达态（AC5×AC7 冲突点） */
/* coverage: HV5-AC6 -- 明天区 preview []（确认空）vs null（未提供）两态渲染显式分叉（明天区宽域帧 diff） */
/* coverage: HV5-AC7 -- 未达提示在场（非留白）+ 明天区状态切换不扰「接下来」区（占位不破布局 containment）+ preview 8 项满载不越明天区宽域（clip containment） */
TEST_CASE("首页空态三态渲染分叉 (HV5-AC5/AC6/AC7)",
          "[p1_home_battery][HV5-AC5][HV5-AC6][HV5-AC7][behavior]")
{
    hp1_item_t items[1];
    hp1_item_t pv_full[HP1_PREVIEW_MAX];
    int rc, ink, diff;
    int i;

    /* fixture: 1 项今日日程（明天区状态切换时「接下来」区内容恒定, 分叉判读不混入） */
    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");

    /* ---- tooth 1 (HV5-AC5/AC7): 接下来区 话术态 vs 未达态 显式分叉 ----------------
     * AC5 态 = inject(count=0)（s_has_day=1 && count==0）→ 话术「今天接下来没有
     * 安排啦」; AC7 态 = day_rollover 跨 0:00 清缓存（s_has_day 清零）→ 数据未达
     * 友好提示（不出话术）。两态在行流带必须逐像素可区分（孩子分得清「没课」还是
     * 「没连上网」, AC7 显式分叉）且未达态也非留白（友好提示在场）。 */
    rc = kidpal_home_p1_schedule_inject("2026-10-11", NULL, 0, NULL, -1);
    hpb_require_implemented(rc, "kidpal_home_p1_schedule_inject unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): schedule_inject(count=0) rc=%d — AC5 话术态数据半",
               rc));
    hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame(AC5 idle) rc=%d", rc));
    ink = hpb_count_ink(hpb_rect_rows);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HV5-AC5 RED: AC5 态行流带墨 %d < %d——无安排话术未渲染（整区留白, "
               "requirements v1.61）", ink, HPB_INK_MIN_ELEM));
    TEST_ASSERT_TRUE_MESSAGE(hpb_snap_region(hpb_rect_rows) == 0,
        "HPB RED(infra): rows-region frame snapshot failed — 探针缝失灵");
    rc = kidpal_home_p1_day_rollover("2026-10-12");
    TEST_ASSERT_TRUE_MESSAGE(rc == 1,
        hpb_fm("HPB RED(infra): day_rollover rc=%d != 1——跨 0:00 清缓存路径回归", rc));
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame(AC7 rollover) rc=%d", rc));
    ink = hpb_count_ink(hpb_rect_rows);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HV5-AC7 RED: rollover 未达态行流带墨 %d < %d——数据未达友好提示缺失"
               "（整区留白, AC7）", ink, HPB_INK_MIN_ELEM));
    diff = hpb_diff_region(hpb_rect_rows);
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "HV5-AC7 RED: AC7 未达态帧与 AC5 话术态帧在行流带逐像素相同——两态渲染混同"
        "（孩子分不清「没课」还是「没连上网」, AC7 显式分叉被破）");

    /* ---- tooth 2 (HV5-AC6/AC7): 明天区 确认空话术态 vs 未提供未达态 显式分叉 -----
     * preview []（AC6 话术）vs preview null（AC7 未达提示）——「接下来」区夹具恒定,
     * 两帧差异必须全部落在明天区宽域带（分叉真实发生在明天区, 不扰他区）。 */
    rc = kidpal_home_p1_schedule_inject("2026-10-12", items, 1, NULL, 0);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): schedule_inject(preview=[]) rc=%d — AC6 话术态", rc));
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame(AC6 tomorrow empty) rc=%d", rc));
    TEST_ASSERT_TRUE_MESSAGE(hpb_snap_region(hpb_rect_tomorrow) == 0,
        "HPB RED(infra): tomorrow-region frame snapshot failed — 探针缝失灵");
    /* 行流带快照同样取自 AC6 帧（与明天区带判读域不相交, 共享缓冲按绝对坐标存取）:
     * 之后 diff(rows) == 0 才是「明天区状态切换不扰接下来区」的有效判读。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_snap_region(hpb_rect_rows) == 0,
        "HPB RED(infra): rows-region frame snapshot failed — 探针缝失灵");
    /* 占位不破布局 containment：明天区两态切换不得扰「接下来」行流带（夹具恒定 →
     * 行流带两帧必须逐像素相同）。 */
    rc = kidpal_home_p1_schedule_inject("2026-10-12", items, 1, NULL, -1);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): schedule_inject(preview=null) rc=%d — AC7 未达态", rc));
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame(AC7 tomorrow null) rc=%d", rc));
    diff = hpb_diff_region(hpb_rect_tomorrow);
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "HV5-AC6 RED: preview [] (确认空话术态) 与 preview null (未达态) 在明天区"
        "宽域带逐像素相同——两态渲染混同（AC6/AC7 显式分叉被破, 孩子分不清「明天"
        "没课」还是「没收到课表」）");
    diff = hpb_diff_region(hpb_rect_rows);
    TEST_ASSERT_TRUE_MESSAGE(diff == 0,
        "HV5-AC7 RED: 明天区两态切换扰动了「接下来」行流带（占位不破布局优先级 "
        "containment 被破, requirements S-F1.1 补充规则）");

    /* ---- tooth 3 (HV5-AC2 containment): preview 8 项满载 → 明天区行流 clip 进宽域
     * 带（渲染溢出走截断降级, 不越带压卡底之下重锚负断言带/不破卡区; 2026-10-13
     * 重锚后判读域只放宽, 断言语义零偏移） */
    memset(pv_full, 0, sizeof(pv_full));
    for (i = 0; i < HP1_PREVIEW_MAX; i++) {
        snprintf(pv_full[i].time, sizeof(pv_full[i].time), "%02d:00", 8 + i);
        snprintf(pv_full[i].name, sizeof(pv_full[i].name), "课%d", i + 1);
    }
    rc = kidpal_home_p1_schedule_inject("2026-10-12", items, 1, pv_full,
                                        HP1_PREVIEW_MAX);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): schedule_inject(preview max) rc=%d", rc));
    rc = kidpal_home_p1_render_frame();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        hpb_fm("HPB RED(infra): render_frame(preview max) rc=%d", rc));
    ink = hpb_count_ink(hpb_rect_tomorrow);
    TEST_ASSERT_TRUE_MESSAGE(ink >= HPB_INK_MIN_ELEM,
        hpb_fm("HV5-AC2 RED: preview 满载明天区宽域带墨 %d < %d——多项行流未渲染",
               ink, HPB_INK_MIN_ELEM));
    ink = hpb_count_ink(hpb_rect_preview_neg);
    TEST_ASSERT_TRUE_MESSAGE(ink == 0,
        hpb_fm("HV5-AC2 RED: preview 满载溢出压入卡底之下重锚带 {152,244,392,258}"
               "墨 %d != 0——明天区行流未 clip 进宽域带且越出卡区底界 y242（渲染溢出"
               "必须截断降级, plan 要点 3 containment; 2026-10-13 重锚语义）", ink));

    /* ---- tooth 4 (TR-A4/A6/A8 明天区多行容量回归齿, 2026-10-13 补充轮重锚) ---------
     * 夹具 = 今天空（AC5 话术态, s_count=0）+ preview 3 项 -> 新几何 ty = 68 + 2*22 =
     * 112, 项行带 155-176 / 177-198 / 199-220（文字 = 带顶 +4..+17）。本齿钉三行带墨
     * 均 >= HPB_INK_MIN_PV_ROW2（阈值按 R < T < F 重标定, 见文件尾 REVISION LOG
     * 2026-10-13 补充轮条: R = 净 HEAD 28px 几何下三带内残/错位墨实测 min, F = 完整
     * 14px 行流文本整行墨）。净 HEAD 下带错位（帽标 157-181 / 项行 184 起）-> 首行带墨
     * 坍缩 < 阈值 = 设计态红; builder 落 line_h=22 + 组头偏移族（ty+8/ty+13/ty+43）后三
     * 带各含完整一行 -> 转绿。参数族回退（line_h 22→28 / 组头偏移族改回旧值 / pv_cy1
     * 回退）-> 行带错位 -> 本齿必红（mutation 断言面承载齿, BUILDER DUTY 5)）。 */
    {
        hp1_item_t pv3[3];
        int ink1, ink2, ink3;

        memset(pv3, 0, sizeof(pv3));
        snprintf(pv3[0].time, sizeof(pv3[0].time), "09:00");
        snprintf(pv3[0].name, sizeof(pv3[0].name), "游泳课");
        snprintf(pv3[1].time, sizeof(pv3[1].time), "10:30");
        snprintf(pv3[1].name, sizeof(pv3[1].name), "英语课");
        snprintf(pv3[2].time, sizeof(pv3[2].time), "11:00");
        snprintf(pv3[2].name, sizeof(pv3[2].name), "美术课");
        rc = kidpal_home_p1_schedule_inject("2026-10-13", NULL, 0, pv3, 3);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): schedule_inject(AC5+pv3) rc=%d — 多行容量夹具", rc));
        hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
        rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(多行容量) rc=%d", rc));
        ink1 = hpb_count_ink(hpb_rect_pv_row1);
        ink2 = hpb_count_ink(hpb_rect_pv_row2);
        ink3 = hpb_count_ink(hpb_rect_pv_row3);
        TEST_ASSERT_TRUE_MESSAGE(ink1 >= HPB_INK_MIN_PV_ROW2 &&
                                     ink2 >= HPB_INK_MIN_PV_ROW2 &&
                                     ink3 >= HPB_INK_MIN_PV_ROW2,
            hpb_fm("TR-A8 RED: 明天区多行容量回归齿——第 1/2/3 行带墨 %d/%d/%d 分别须 >= "
                   "%d（今天空 + preview 3 项, ty=112, 行带 155-176/177-198/199-220）; 行带"
                   "错位（参数族回退 line_h/组头偏移/pv_cy1）即坍缩 < 阈值",
                   ink1, ink2, ink3, HPB_INK_MIN_PV_ROW2));
    }

    /* ---- tooth 5 (TR-A2/A8 今天区卡底守卫负断言, 2026-10-13 补充轮语义重锚) ---------
     * 今天 8 项: 第 8 项行带 222..243（文字 226..239）越卡底 242 达 1px——整行容纳守卫
     * 落地后第 8 项整行让位、带 {152,222,392,242} 干净。零墨断言 = TR-A9 真机判定④的
     * host 帧回归代理（真机探真仍归 [TR5] 豁免段）; 摘整行容纳守卫的变异对本齿必红
     * （第 8 项文字 226..239 入带, mutation 断言面, BUILDER DUTY 5)）。
     * 净 HEAD（28px）下今天 6 项满行至 y235、第 6 项文字 214..227 残墨入带 = 设计态红。 */
    {
        hp1_item_t many[8];
        int i;

        memset(many, 0, sizeof(many));
        for (i = 0; i < 8; i++) {
            snprintf(many[i].time, sizeof(many[i].time), "%02d:00", 8 + i);
            snprintf(many[i].name, sizeof(many[i].name), "课%d", i + 1);
        }
        rc = kidpal_home_p1_schedule_inject("2026-10-13", many, 8, NULL, -1);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): schedule_inject(today8) rc=%d", rc));
        hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
        rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(today8) rc=%d", rc));
        ink = hpb_count_ink(hpb_rect_today_bot);
        TEST_ASSERT_TRUE_MESSAGE(ink == 0,
            hpb_fm("TR-A2 RED: 今天区卡底残线带 {152,222,392,242} 墨 %d != 0——第 8 项"
                   "（行带 222-243）跨卡底未整行让位（整行容纳守卫缺失, 261013 regression）",
                   ink));
    }

    /* ---- tooth 6 (TR-A7 组级容纳门, 2026-10-13 补充轮新增语义) ----------------------
     * 组级门语义（plan 修订 A 改动点 A1 第 6 条）: 明天区整块（组间虚线 + 帽标 +
     * 项行/AC6/AC7 三态）包在 if (ty + 43 + line_h - 1 <= pv_cy1) 内（ty <= 178）——
     * 今天 >= 6 项时 ty >= 200、整区让位（不画半截帽标/孤儿虚线）。判别: 同一 6 项今天
     * 日程下, preview 3 项帧 vs preview 0 项帧在明天区宽域带必须逐像素相同（两帧明天区
     * 均整体让位 = 无明天墨源）; 摘门 -> preview 3 帧帽标入带、两帧分叉 -> diff != 0 必红。
     * 净 HEAD（28px, 无组级门）下今天 6 项 ty=236 -> 明天区内容（帽标 269..）越出宽域带
     * 下缘, 带内 diff 为零 —— 本齿基线即绿（MUST-STAY-GREEN, 见文件尾 REVISION LOG 推导表）。 */
    {
        hp1_item_t six[6];
        hp1_item_t pv3[3];
        int i, dv;

        memset(six, 0, sizeof(six));
        for (i = 0; i < 6; i++) {
            snprintf(six[i].time, sizeof(six[i].time), "%02d:00", 8 + i);
            snprintf(six[i].name, sizeof(six[i].name), "课%d", i + 1);
        }
        memset(pv3, 0, sizeof(pv3));
        snprintf(pv3[0].time, sizeof(pv3[0].time), "09:00");
        snprintf(pv3[0].name, sizeof(pv3[0].name), "游泳课");
        snprintf(pv3[1].time, sizeof(pv3[1].time), "10:30");
        snprintf(pv3[1].name, sizeof(pv3[1].name), "英语课");
        snprintf(pv3[2].time, sizeof(pv3[2].time), "11:00");
        snprintf(pv3[2].name, sizeof(pv3[2].name), "美术课");

        /* preview 3 项帧（今天 6 项 —— 组级门在世则明天区整体让位）。 */
        rc = kidpal_home_p1_schedule_inject("2026-10-13", six, 6, pv3, 3);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): schedule_inject(today6+pv3) rc=%d — TR-A7 夹具", rc));
        hpb_set_fixture(10, 24, 0, 0, 0, 85, 0, 0);
        rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(today6+pv3) rc=%d", rc));
        TEST_ASSERT_TRUE_MESSAGE(hpb_snap_region(hpb_rect_tomorrow) == 0,
            "HPB RED(infra): tomorrow-region frame snapshot failed — 探针缝失灵");
        /* preview 0 项帧（今天同 6 项, 明天区改 AC6 确认空话术 —— 组级门在世同样整区让位）。 */
        rc = kidpal_home_p1_schedule_inject("2026-10-13", six, 6, NULL, 0);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): schedule_inject(today6+pv0) rc=%d", rc));
        rc = kidpal_home_p1_render_frame();
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            hpb_fm("HPB RED(infra): render_frame(today6+pv0) rc=%d", rc));
        dv = hpb_diff_region(hpb_rect_tomorrow);
        TEST_ASSERT_TRUE_MESSAGE(dv == 0,
            hpb_fm("TR-A7 RED: 今天 6 项时 preview 3 项帧 vs 0 项帧在明天区宽域带 diff %d "
                   "!= 0——组级容纳门缺失, 明天区未整体让位（半截帽标/孤儿虚线入带; "
                   "ty=200 > 178 应整区让位）", dv));
    }
}

/* ---------------------------------------------------------------------------
 * TEST 5 — PH-A7/PH-E1 终跑守卫：注册接线 + 数据口径零触碰 + 需求契约 canary +
 * 扫描器自检
 *   PH-A7 结构半（本验收文件 + home_p1.c 是否被 builder 接线进 HOST_ACCEPTANCE_SRCS）
 *   PH-E1 范围纪律（schedule API 面零触碰 + 需求/契约面零修订 canary）
 *   扫描器自检（结构齿负向必须能红，防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: PH-A7 -- host runner 终跑：本文件+home_p1.c 注册进 HOST_ACCEPTANCE_SRCS 结构守卫 + suite 级判读锚 */
/* coverage: PH-E1 -- 范围纪律守卫：schedule API 面零触碰（数据口径零改动）+ 需求/契约面零修订 canary */
TEST_CASE("终跑守卫: 注册接线+数据口径零触碰+需求契约canary+扫描器自检 (PH-A7/PH-E1)",
          "[p1_home_battery][PH-A7][PH-E1][boundary]")
{
    char *mk;
    char hit[HPB_MAX_PATH];
    int n;

    HPB_REQUIRE_ROOT();

    /* tooth 1: 本文件 + home_p1.c 接线进 HOST_ACCEPTANCE_SRCS 且排在 smoke 之前
     *（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。RED until builder wires. */
    mk = hpb_read("firmware/main/test/host_app/main/CMakeLists.txt", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "HPB RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "p1_home_battery.acceptance.c");
        const char *impl = strstr(mk, "../../../display/home_p1.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "PH-A7 RED: p1_home_battery.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS "
            "— host 验收构建未接本包（BUILDER DUTY 4))");
        TEST_ASSERT_TRUE_MESSAGE(impl != NULL,
            "PH-A7 RED: firmware/main/display/home_p1.c 未接线进 HOST_ACCEPTANCE_SRCS "
            "— 行为齿强符号无处落地（weak 哨兵将持续红）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "HPB RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS "
            "丢失 — 包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "PH-A7 RED: p1_home_battery.acceptance.c 必须排在 host_infra_smoke."
            "acceptance.c 之前 (smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2 (PH-E1 guard): 日程数据口径声明面在世——home_p1.h schedule API 族
     * （inject/count/item/preview 访问族/day_rollover/countdown）。2026-10-11 随
     * CCP 触点③ 追加 preview_count/preview_item 多项访问族（REVISION LOG 2026-10-11
     * 条; 旧单项 preview 子串被 preview_count 天然覆盖, 声明面在世判据 additive）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_HOME_P1_H,
                                               "kidpal_home_p1_schedule_inject") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_schedule_count") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_schedule_item") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_schedule_preview"
                                                   "_count") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_schedule_preview"
                                                   "_item") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_day_rollover") &&
                                 hpb_file_contains(HPB_HOME_P1_H,
                                                   "kidpal_home_p1_countdown"),
        "PH-E1 RED: home_p1.h schedule API 声明面缺失——日程数据口径被触碰"
        "（含 2026-10-11 preview 多项访问族）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains(HPB_HOME_P1_C,
                                               "kidpal_home_p1_schedule_inject"),
        "PH-E1 RED: home_p1.c schedule_inject 实现缺失——日程数据链路被拆");

    /* tooth 3 (PH-E1 canary): 需求面在世且 S-F1.1 锚在世 + 契约面在世（零修订）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_exists("docs/requirements.md"),
        "PH-E1 RED(infra): docs/requirements.md 丢失——需求面被触碰");
    TEST_ASSERT_TRUE_MESSAGE(hpb_file_contains("docs/requirements.md", "S-F1.1"),
        "PH-E1 RED: requirements S-F1.1 锚丢失——需求面被修订（发现冲突走 spec-lane "
        "逃生门, 不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(hpb_exists("docs/contracts/interface-contract.md"),
        "HPB RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 4: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(hpb_exists(HPB_RN_CC),
        "HPB RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(hpb_exists("firmware/main/__hpb_no_such__.cc"),
        "HPB RED(infra): scanner reports a nonexistent file as present");
    n = hpb_scan_count("firmware/main", "__hpb_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "HPB RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = hpb_count_in_file(HPB_RN_CC, "s_battery_pct");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "HPB RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    {
        char *region2 = hpb_region_between(HPB_RN_CC,
                                           "void kidpal_render_node_inject_rtc(",
                                           "void kidpal_render_node_inject_env(");
        TEST_ASSERT_NOT_NULL_MESSAGE(region2,
            "HPB RED(infra): region slicer cannot slice a known function — 切区器失灵");
        TEST_ASSERT_TRUE_MESSAGE(hpb_count_in_slice(region2, "s_rtc_hour") >= 1,
            "HPB RED(infra): region slicer lost a known literal — 切区器计数失灵");
        free(region2);
    }
}

/* ---------------------------------------------------------------------------
 * PH-A7 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   有效基线 = 2026-09-15 本文件冻结时 host 面全量既有齿实跑（现役齿 = K1a
 *   key_layer_dispatch + 包 0 host_infra_smoke，2026-09-15 544a804 归档后收敛），
 *   实测数字与在册红逐条点名见 taskpackage redBaselineResult（planner 转写）。
 *   判读纪律 = 新增红才是本包责任，在册红逐条点名、不代修。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）:
 *     - 本文件 test 1-3（home_p1.c 未接进 runner 的 weak 哨兵红 + seam [1]/[2]
 *       未落地的结构红——真实缺失行为）;
 *     - test 5 tooth 1（注册接线守卫, builder 接线后转绿）。
 *   MUST-STAY-GREEN at baseline（钉点/守卫齿, 现状即绿）:
 *     - test 2 决策表半（doze/blink 归档契约实现已在与面——随 home_p1.c 接线进
 *       runner 后同样必绿; pose_sleep 守卫现状绿）;
 *     - test 4 全部（PH-A5 钉点: 断网链路已实现, 本包只钉点）;
 *     - test 5 tooth 2/3/4（数据口径零触碰 + 需求契约 canary + 扫描器自检）。
 *   PH-A6 真机确认轮: human 持板 ADC 实测（电量值合理、低电闪烁/充电停闪真机可见）,
 *   证据落 delivery/changes/dev/260915-m1-n1b-p1-home-battery/。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author）:
 *   通道① 既有 host runner 全量实跑（新齿未接线前）= 有效基线（见 PH-A7 终跑判读）。
 *   通道② 本文件新齿 C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline -I firmware/main/test firmware/main/test/p1_home_battery.
 *         acceptance.c
 *     编译通道 error 为零（验收文件本身可编译）+ 解析性红定性（逐齿对现状必红的
 *     事实源, 2026-09-15 grep 实测）:
 *       - kidpal_render_node_inject_battery: .h/.cc/board.cc 全零命中;
 *       - 板类 adc1_handle/cali_handle/vbat_status 各恰 1 处（死成员=仅声明）,
 *         AdcBatteryMonitor / GetBatteryLevel 板类零命中;
 *       - kidpal_render_node.cc s_battery_pct 恰 4 处（声明+三读, 无注入写点 =
 *         恒值注入在世）;
 *       - home_p1.c / kidpal_render_node.cc 零 pose_sleep（守卫齿现状绿）;
 *       - wifi_board.cc GetNetworkStateIcon 区含 FONT_AWESOME_WIFI_SLASH + IsConnected
 *         （钉点齿现状绿）;
 *       - host_app/main/CMakeLists.txt 无 p1_home_battery.acceptance.c 与
 *         display/home_p1.c 接线行（test 5 tooth 1 红）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）。
 *
 *   R2 追记（2026-09-22 跨包冲击修订红基线, acceptance-author 实跑; 授权 =
 *   pod-events c4785230 human-decision plan-B-decode-fix-plus-wireframe-alignment）:
 *     修订前基线 = 当前树全量 host runner 实跑 83 Tests / 7 Failures——红全部为
 *     p1_cjk_name 追加轮设计态红（在册于该文件 RED baseline 判读口径通道②, 非本包
 *     责任）; HPB 五 CASE 全绿（零偏移锚）。
 *     修订后（builder 未落 [S3]/[S5] 新版式）本包义务红（设计态, builder 落新版式
 *     后转绿, 不计违约）= 本文件 TEST 1: 实跑红点 tooth 2（旧贴纸/大时间右上区在世
 *     渲染）; tooth 4/5 同 CASE 内被 Unity abort 屏蔽、解析性必红——tooth 4 旧预告
 *     独立带含 p1_draw_preview 兜底描边 {138,222,390,240} 带墨（home_p1.c 在世）+
 *     明早槽零墨, tooth 5 空日程 p1_draw_next 系整体门控不调用 → LABEL 带零墨
 *     < 30（语义反转: 新版式下标签固定卡题须在场）。其余齿零偏移。
 *
 *   2026-10-13 追记（regression 包 261013-p1-tomorrow-row-sliver-fix 冻结红基线,
 *   acceptance-author 实跑; 状态 = 净 HEAD + 未修复实现 + 重锚齿/新齿, 命令 =
 *   Run protocol 1)+2)）:
 *     设计态义务红 = TEST 4.5 tooth 4（TR-A4 明天区第 2 行带回归齿——净 HEAD 实测
 *     带内墨 15 < 阈值 80, 残线行为未修复）; tooth 5（今天区卡底零残线负断言）与
 *     tooth 4 同 CASE, 被 Unity abort 屏蔽、解析性必红（守卫未落, 残线在 y=242）,
 *     builder 落地后两齿同转绿。
 *     重锚齿 MUST-STAY-GREEN at baseline: TEST 1 tooth 4 与 TEST 4.5 tooth 1-3 在
 *     净 HEAD + 重锚后实跑全绿（重锚只放宽判读域——新增域 220..242 在净 HEAD 全部
 *     既有夹具下零墨, 逐齿推演与实跑一致; 标定会话 165 Tests / 1 Failure, 唯一红
 *     即 tooth 4 设计态红, 终跑计数与红名 verbatim 见 REVISION LOG 2026-10-13 条）。
 *
 *   2026-10-13 补充轮追记（TaskPackage 261013 补充轮行距压缩 + 组级容纳门冻结红基线,
 *   acceptance-author 实跑; 状态 = 净 HEAD 28px 实现 + 重锚齿/新齿, 命令 = Run
 *   protocol 1)+2)）:
 *     全量 = 165 Tests / 4 Failures / 0 Ignored（exit 4）。本文件设计态义务红 = TEST 4.5
 *     tooth 4：TR-A8 RED: 明天区多行容量回归齿——第 1/2/3 行带墨 673/246/84 分别须 >=
 *     110（今天空 + preview 3 项, ty=112, 行带 155-176/177-198/199-220）; 行带错位（参数
 *     族回退 line_h/组头偏移/pv_cy1）即坍缩 < 阈值。tooth 5 与 tooth 4 同 CASE, 被 Unity
 *     abort 屏蔽、解析性必红。另 3 红在 sibling p1_cjk_name.acceptance.c（重锚批设计态红,
 *     单在彼文件 REVISION LOG 记; 非本文件新增红）。MUST-STAY-GREEN at baseline 见
 *     REVISION LOG 2026-10-13 补充轮条⑤; 推导表之外变红: 无。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-15  initial freeze (acceptance-author, TaskPackage
 *               260915-m1-n1b-p1-home-battery).
 *   2026-09-22  跨包冲击修订（planner 派发修订窗口; 授权 = 260921-m3-k4-p1-cjk-
 *               dynamic-text pod-events 事件 c4785230 human-decision
 *               plan-B-decode-fix-plus-wireframe-alignment——首页「接下来」卡版式
 *               对齐 WF-F1-01: 大 imgfont 时间退役、「接下来」改烘焙大字标签、
 *               文字行 14px 行流语义、预告并入主体第三行、倒计时贴纸随时钟缺口
 *               挂账不做; 冲击面 = PCN 头注「跨包冲击登记」三齿）:
 *               ① tooth 2 贴纸在位 -> 退役负断言: 钉区改 STICKER_NEG
 *               {268,50,390,92} 任何夹具严格零墨（贴纸/大时间/预告右上区三重退役;
 *               口径对齐 PCN [S3] = PCN TEST 8 tooth 2; 旧正向矩形下界 y130 与
 *               新 ROW1/ROW2 行带重叠, 不可作零墨区, 故取 PCN 同款退役面）。
 *               ② tooth 4 后续事项区+预告区在位 -> R2 行流语义: 后续事项半重钉
 *               ROW2 {136,124,392,150} 在位（原 NEXTLIST {136,172,392,214} 在新版
 *               式下 = ROW3 尾 + FLOW 承载域, 标准夹具无恒定墨源——保留即对
 *               builder 落位方式脆断, 故随行流语义重钉）; 预告半重钉明早标签槽
 *               {136,152,180,178} 墨 >= 12 + 旧预告独立带 {136,220,392,258} 标准
 *               夹具零墨（预告上行第三行; 时间起笔 x>=184 构成判别由 PCN TEST 8
 *               tooth 4 承载, 本齿钉在场与退役面）。登记面仅点名预告半, 后续事项
 *               半的同 tooth 重钉系该 tooth 自身语义随版式演进的必然（非放宽,
 *               在场义务保留）。
 *               ③ tooth 5 空日程主体区零墨 -> 固定卡题口径: 「接下来」烘焙大字
 *               标签为固定卡题, 空日程仍在场（LABEL {136,48,266,92} 墨 >= 30）+
 *               日程行流带 {152,68,392,140}（ROW1..ROW3 域）严格零墨（行流门控）。
 *               旧「整区零墨」与固定卡题语义直接冲突（跨包冲击登记第 3 条）。
 *               ④ 新增 HPB_INK_MIN_LABEL=30 / HPB_INK_MIN_MINGZAO=12（与 PCN
 *               同名阈值同源同值）; 静态矩形表同步重钉（sticker/nextlist/preview
 *               三常量替换为 label/rows/sticker_neg/row2/mingzao/preview_neg）。
 *               ⑤ 零触碰面: teeth 1/3/6-9 与 TEST 2-5 全部零偏移; 被测面 /
 *               PCN 冻结件 / contracts / requirements 零触碰; CMakeLists 零改
 *               （HPB 已注册在册）。红基线见头注「R2 追记」。
 *               ── 2026-09-26 真机对齐轮追记（P1 拍图↔canvas-p1 逐项对齐,
 *               human 看屏裁定授权, ui-v5-align workstream 直改批模式）──
 *               钉区常量整批重钉（canvas-p1 像素实测 2026-09-26）: 辅助条三元素
 *               右移（温度 210 / 湿度 282 / 电量 342; 温度数值半含 °C 后缀——
 *               imgfont_degree 真字形, gen_imgfont_letters.py 生成 + art_assets.c
 *               0xB0 分流）; 辅助条钉区底缘 33→31（上虚线重钉 y33-35, 钉区不含
 *               虚线行——否则缺席/灭相零墨齿被虚线墨误触发）; MASCOT 重钉
 *               {8,86,128,242}（猫下移 (28,116), 兜底描边随移）; NEXT/LABEL/
 *               ROWS/ROW2 随卡区上移右移重钉（云 ic_cloud_40_t2 @103,29 /
 *               帽标 @151,41 / 内容列 x152 / ROW1 y62）; MINGZAO 重钉
 *               {148,148,200,175}（预告重构 = 帽标独立行 + 项行在下, 设计结构）;
 *               STICKER_NEG 重锚 {236,36,390,60}（旧右上区已是行流带合法领空）;
 *               PREVIEW_NEG x0 136→152（sparkle 装饰墨 x≤145 让位）。日期加星期
 *               "Www DD Mon"（hp1_state_t 增 wday 字段, inject_weekday 同源缝,
 *               周一=0 基; set_state 门 -1..6）。波浪涂鸦换 ic_wave_58（原
 *               rec_wave 折线系误用, human 指出）。实跑 = 103 Tests / 0 Failures
 *               / OK（2026-09-26 主线程实跑锚）。
 *               ── 2026-10-11 冻结件修订（TaskPackage 261011-p1-home-v5-rework;
 *               授权链双引用 = 「CCP id 261011-p1-home-v5-rework（HUMAN_APPROVED
 *               2026-10-11）+ docs/requirements.md §9 v1.61 版本行（2026-10-11）」,
 *               plan R3 明示派发）──
 *               ① seam 终形适配：weak inject 哨兵改 5 参（next_day_items/
 *               next_day_count）+ 旧单项 preview 哨兵退役 → preview_count/
 *               preview_item 双哨兵; home_p1.h 旧 4 参声明以宏改写旁路（仅声明面,
 *               builder 落地新头后本文件零改续编）; 标准夹具 preview 改 1 项数组
 *               （5 参调用）; 空日程夹具 preview 半改未提供态（NULL, -1）。
 *               ② tooth 5 语义重钉（AC5, v1.61「不再整区留白」supersede R2 行流
 *               门控空语义）：空日程 → 行流带 {152,68,392,140} 墨 >= 8（AC5 话术
 *               「今天接下来没有安排啦」在场）; LABEL 带在场半零偏移保留。
 *               ③ 新增 TEST 4.5（HV5-AC5/AC6/AC7, additive）：空态三态渲染分叉帧
 *               diff 齿——AC5 话术态 vs rollover 驱动的 AC7 未达态在行流带逐像素
 *               可区分且两态均非留白; 明天区 preview []（确认空）vs null（未提供）
 *               两态在明天区宽域带 {136,142,392,218} 逐像素可区分 + 切换不扰行流带
 *               （占位不破布局 containment）+ preview 8 项满载不越宽域压 PREVIEW_NEG
 *               退役面（clip containment）; 新增 hpb_rect_tomorrow 宽域常量与
 *               hpb_snap_region/hpb_diff_region 帧快照 helpers（additive, 探针失败
 *               大声红）。帧 diff 钉「两态可区分」; 话术↔态的绑定由
 *               schedule_grid.acceptance.c apply 三态映射齿 + 渲染源话术字面齿共同
 *               承载（行为等价假设显式化: 帧分叉不验文字内容本身）。
 *               ④ TEST 5 tooth 2 additive：schedule API 声明面判据补 preview_count/
 *               preview_item（旧 preview 子串被 preview_count 天然覆盖）。
 *               零触碰面：teeth 1-4/6-8 与 TEST 2-4 既有齿、钉区常量既有值、
 *               CMakeLists（HPB 已注册在册）、被测面零触碰。
 *               ── 2026-10-13 冻结件修订（TaskPackage 261013-p1-tomorrow-row-sliver-fix;
 *               授权链 = 「TaskPackage 261013-p1-tomorrow-row-sliver-fix（human triage
 *               触点① 确认）」; sourceType=regression, rigor=lite, riskTier=low）──
 *               根因 = 明天区行流独立 clip 底界 pv_cy1=218 硬编码（约束来源已退役的
 *               旧 PREVIEW_NEG 负断言带, 非当前布局必需）+ p1_blit_flow 逐像素行 clip
 *               （py > cy1 跳过）把部分越界行剃成 1px 残线; 真机截图实证: 今天区空
 *               （AC5 话术）时明天区第 2 项剩 y=218 一条 1px 墨线。
 *               ① 钉区重锚（TR-A3）: hpb_rect_preview_neg {152,220,392,258} →
 *               {152,244,392,258}（pv_cy1 对齐卡底 242 后 220..242 段退役为明天区
 *               第 2 行合法域; 负断言语义从「不压旧预告退役带」改「不越卡区底界」
 *               = 卡底 242 + 2px 缓冲, 沿用「合法底界 + 2px」既有纪律）;
 *               hpb_rect_tomorrow {136,142,392,218} → {136,142,392,242}（宽域带对齐
 *               明天区真实足迹, 仍只判读不钉行位, 既有用法逐齿推演均成立）。同步面 =
 *               头注表 / 区域注 / TEST 1 tooth 4 与 TEST 4.5 tooth 3 断言消息矩形
 *               数值（断言语义不弱化: 零墨义务保留, 判读域语义随重锚换锚）。
 *               判别力保全论证: 220..242 段退役为合法域后, 卡底 containment 改由
 *               三重承载——重锚后 PREVIEW_NEG {152,244,392,258}（越卡底溢出仍红）+
 *               整行容纳守卫行为齿（tooth 4/5）+ 既有 PCN A10 底界外带齿（p1_cjk_name
 *               零触碰回归观察面）。逐齿推演: 既有齿判读域只放宽——新增域 220..242
 *               在净 HEAD 全部既有夹具下零墨或行为不变（标准夹具明天区仅 1 行
 *               184..211、卡区 clip cy1=242 本就无墨越界）, 无齿漂移; 实跑复核一致
 *               （见头注 2026-10-13 追记）。
 *               ② 新增回归齿（TR-A4 采纳 + TR-A2 守卫负断言, additive）:
 *               TEST 4.5 tooth 4——明天区第 2 行带 {152,212,392,239} 墨 >=
 *               HPB_INK_MIN_PV_ROW2=80（阈值标定 = 冻结会话净 HEAD 实跑 TEMP-CALIB
 *               一次测量: 残线墨 R=15 < T=80 < 同夹具第 1 行整行墨 F=157; 第 1/2 行
 *               同为「时间+课名」14px 行流文本, 整行墨同量级, T=80 对 R 约 5× 裕度、
 *               对 F 约 2× 裕度——1px 残线红 / 整行绿; 附第 1 行带 {152,184,392,211}
 *               在场下界参照齿, 防夹具失锚假红）+ tooth 5——今天区卡底残线带
 *               {152,242,392,242} 严格零墨（今天 >= 7 项第 7 项跨卡底同病同修的
 *               负断言; 兼承「摘守卫」变异面——明天区守卫在 pv_cy1=242 下摘除后
 *               第 3 行整行越界零墨、像素不可观测, 今天区 1px 残线可观测故由
 *               tooth 5 承载守卫变异判别）。两齿均为净 HEAD 设计态红（残线行为
 *               未修复）, builder 落地后转绿。
 *               mutation 断言面（BUILDER DUTY 5) 申报; 取证由 builder/fixer 实测、
 *               planner 代落 taskpackage mutationEvidence, 本文件不落取证）:
 *               改回 pv_cy1=218 → tooth 4 红; 摘整行容纳守卫 → tooth 5 红;
 *               越卡底溢出 → 重锚 PREVIEW_NEG 负断言（TEST 1 tooth 4 / TEST 4.5
 *               tooth 3）红。
 *               ③ TR-A5 真机轮 live-manual structural 豁免（头注 [TR5] 段在先申报,
 *               TB-A4 261012 story_page_v2 同款先例口径, 不计入 runner 通过面）;
 *               host 帧像素齿（tooth 4/5）只钉 host 回归语义, 不替代真机探真,
 *               host 侧不为 [TR5] 四项另立像素断言。
 *               零触碰面: p1_cjk_name.acceptance.c / schedule_grid.acceptance.c 零改
 *               （回归观察面, 同 runner 实跑确认不因重锚破）; TEST 1-4 既有齿断言
 *               语义零偏移（仅 tooth 4 / tooth 3 断言消息矩形数值与措辞随重锚同步）;
 *               CMakeLists 零改（HPB 已注册在册）; 被测面本文件不触（实现归 builder）。
 *   ── 2026-10-13 补充轮冻结件修订（TaskPackage 261013-p1-tomorrow-row-sliver-fix 补充
 *   轮; 授权链 = 「TaskPackage 261013 补充轮（human triage 触点① 确认 + 真机预览定稿
 *   裁定 + 载体改判并入本包, 见 sibling preview-round.md）」; human 裁定不开新包、并入
 *   本包; plan 修订 A = 行距压缩 line_h 28→22 + 组头偏移族（虚线 ty+8 / 帽标 ty+13 /
 *   项行 ty+43）+ 组级容纳门; 紧凑上浮锚点 max(2,s_count) 保留）──
 *               ① 钉区常量重锚（TR-A8, 随 22px pitch 公式平移）: hpb_rect_row2
 *               {152,96,392,122} → {152,90,392,111}; hpb_rect_mingzao {148,154,200,181}
 *               → {148,124,200,149}; hpb_rect_rows {152,68,392,140} → {152,68,392,133}
 *               （取语义纯净域, 排除 s<=2 档虚线 120-122 卷入带）; hpb_rect_tomorrow
 *               {136,142,392,242} → {136,118,392,242}（上缘对齐明天区真实足迹）;
 *               hpb_rect_pv_row1 {152,184,392,211} → {152,155,392,176}; hpb_rect_pv_row2
 *               {152,212,392,239} → {152,177,392,198}; 新增 hpb_rect_pv_row3
 *               {152,199,392,220}; hpb_rect_today_bot {152,242,392,242} → {152,222,392,242}
 *               （语义重锚 = 今天第 8 项越卡底整行让位带）。零改动: hpb_rect_preview_neg
 *               {152,244,392,258} / next / label / mascot / sticker_neg / row1 族 / aux 族。
 *               ② 齿重锚（TR-A4/A6）: TEST 4.5 tooth 4 重定为**明天区多行容量回归齿**
 *               ——夹具今天空（AC5 话术态, s_count=0）+ preview 3 项, 新几何 ty=112,
 *               项行带 155-176 / 177-198 / 199-220; 断言三行带墨均 >=
 *               HPB_INK_MIN_PV_ROW2（重标定值见下）；TEST 4.5 tooth 5 夹具 7→8 项,
 *               负断言带改 {152,222,392,242} 零墨 = 第 8 项（行带 222-243）整行让位。
 *               ③ 新增 TEST 4.5 tooth 6（TR-A7, 组级容纳门）: 今天 6 项时 preview 3 项帧
 *               vs 0 项帧在明天区宽域带逐像素相同（组级门整区让位, ty=200 > 178）;
 *               摘门 -> 半截帽标入带 diff != 0 必红。
 *               ④ 阈值重标定（TR-A8, HPB_INK_MIN_PV_ROW2 80 → 110, R<T<F 纪律）:
 *               R = 净 HEAD 旧 28px 几何下新行带内残/错位墨实测 min 三带 = 84（实跑
 *               verbatim「第 1/2/3 行带墨 673/246/84」——帽标 + 首/次行残墨入带, 今天空
 *               + preview 3 项夹具, ty=124 档）; F = 完整 14px 行流文本整行墨（同文本族,
 *               主轮同夹具实测锚 157）。T=110 ∈ (84,157): 净 HEAD -> 第 3 行带 84 < 110 红
 *               / 新几何三带各含完整一行（≈157）-> 绿。
 *               ⑤ 红基线（2026-10-13 补充轮, acceptance-author 实跑; 状态 = 净 HEAD 28px
 *               实现 + 重锚齿/新齿, 命令 = Run protocol 1)+2)）: 165 Tests / 4 Failures。
 *               本文件设计态红 = TEST 4.5 tooth 4（TR-A8 多行容量, 三带 673/246/84,
 *               min < 110）; tooth 5（TR-A2 today_bot {152,222,392,242} 旧几何残墨 != 0）
 *               与 tooth 4 同 CASE, 被 Unity abort 屏蔽、解析性必红（builder 落 22px 后
 *               同转绿）。**MUST-STAY-GREEN at baseline（实跑绿）**: TEST 1 tooth 4/5（R2
 *               重锚带恰含旧几何虚线段 -> >= 12/8 绿）; TEST 4.5 tooth 1/2/3（HV5 分叉齿,
 *               pv_cy1 已由主轮修）; TEST 4.5 tooth 6（TR-A7; probe 实跑验证绿——旧几何
 *               今天 6 项 ty=236 明天区内容越出宽域带下缘、带内 diff 0）。**推导表之外
 *               变红: 无**。
 *               ⑥ 跨文件: p1_cjk_name.acceptance.c 同批重锚（本包首次入 changeManifest;
 *               授权链同——human 载体改判）, 其红色单在彼文件 REVISION LOG 记（pcn TEST 1
 *               tooth 3 / TEST 5 A3 / TEST 8 tooth 4 三处设计态红——band 位置 vs 28px
 *               实现错位）。schedule_grid.acceptance.c 零改动 canary 实跑绿。两冻结件
 *               REVISION LOG append 不覆盖既有行。
 *               ⑦ mutation 面迁移（TR-A6/A7/A8）: 参数族回退 line_h / 组头偏移族 /
 *               pv_cy1 -> tooth 4 红; 摘今天区整行容纳守卫 -> tooth 5 红; 摘组级容纳门 ->
 *               tooth 6 红; 重锚常量回退 hpb_rect_pv_row2 / pcn_rect_row2 等 -> 对应行带
 *               断言错位红（取证由 builder/fixer 实测、planner 代落 taskpackage）。
 *               预申报面见 BUILDER DUTY 5)。
 * --------------------------------------------------------------------------- */
