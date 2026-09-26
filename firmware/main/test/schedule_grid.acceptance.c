/*
 * Frozen acceptance tests — TaskPackage 260921-m3-k2-p2-week-grid-page (subsystem:
 * firmware; M3-K2; P2 一周总览网格页整页重写 + 首页「接下来」注入接线 · mergeOrder 34 ·
 * rigor=full / riskTier=high).
 *
 * Story: S-F1.2 · 课表页看「一周总览」（AC1–AC4）+ S-F1.1 · 常显首页看「接下来干嘛」
 * （AC1–AC2 消费接线半）。设计权威 = docs/requirements.md S-F1.2（WF-F1-02 周网格线框
 * v1.19/v1.20）+ NFR-READ-1（1bpp CJK 字号下限 14px 实测定案）；契约形状权威 =
 * docs/contracts/interface-contract.md §1.3②（schedule 周网格 data，2026-09-21 契约门
 * HUMAN_APPROVED 定稿）+ §1.3⑥（home 可选 data.upcoming，additive）。
 *
 * 跨端对端事实（本文件断言按对端实际接受什么钉，不按本侧想象；SSOT =
 * server/gateway/schedule.py week_grid_payload / home_upcoming_payload + front_app.py
 * _display_wire_arguments 实测）：
 *   - schedule data wire = {"week":{"courses":[{"day":int,"slot":int,"name":str,
 *     "flags":int}, …]}}；day 0 基（周一=0..周日=6）、slot 0 上午/1 下午/2 晚上、
 *     flags bit0=※ 临时 bit1=星标（键集精确 {day,slot,name,flags}）；day/slot/flags 是
 *     JSON 数字、name 是字符串。设备收到的 `data` 参数 = front
 *     json.dumps(data, ensure_ascii=False) 字符串 → **默认分隔符含空格**（": " / ", "），
 *     解析必须容忍空白（本文件 canonical 载荷按 wire 实况带空格钉）。
 *   - home data wire = {"upcoming":{"day_key":"今天","items":[{time,name},…],
 *     "preview":{time,name}|null}}；**preview 可为 JSON null**（明日无项时 producer
 *     发 null，设备必须容忍 = 无预告，不降级）；preview.time 恒 "HH:MM"（K1 pod-retro
 *     裁决口径：载荷钉明日首项 HH:MM；「明早」等自然语言标签是设备渲染标签——而
 *     home_p1 冻结 seam p1_valid_item 只接受可解析 H:MM/HH:MM，故任何非 HH:MM 的
 *     preview.time（含「明早」字面）在 seam 处被拒、设备 -1 降级，绝不盲渲染）。
 *   - 薄载荷 {view:schedule, data:{}} 若泄漏直达设备 → data="{}" → 解析 -1 降级
 *     （front 正常截获展开；设备侧绝不盲渲染，V27）。旧列表形状
 *     {title,items} 同样无 "week" 键 → -1。
 *
 * Nature: host-runnable structural (tree-scan) + BEHAVIORAL (weak-sentinel 冻结离线
 * 契约 + LVGL 软渲染真帧探针) frozen acceptance。范式 = 归档 schedule_p2.acceptance.c
 * （weak 哨兵 frozen offline contract）+ p1_home_battery.acceptance.c（区钉 + 注入缝
 * 结构齿）双先例合体。红基线红在真实缺失行为（weak 哨兵 -2 / 树扫描缺失），绝不红在
 * 链接/编译错误。
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT（builder MUST implement；本文件 weak 哨兵在实现接线进
 * host runner 后被强定义覆盖。建议家 = firmware/main/display/schedule_page.{c,h}
 * 整页重写（旧列表版式作废，共用 safe-truncate/JSON 扫描/降级纪律）+ 新纯逻辑模块
 * firmware/main/display/home_upcoming.{c,h}（A5 接线层））:
 *
 *   typedef struct {
 *       int  day;            -- 0..6, 0=周一..6=周日（契约 §1.3② day 0 基）
 *       int  slot;           -- 0..2, 0=上午 1=下午 2=晚上
 *       char name[KSG_NAME_CAP]; -- 课程名 UTF-8 bytes（safe-truncated, 恒 NUL 结尾）
 *       int  flags;          -- KSG_FLAG_TEMP(0x1 ※) | KSG_FLAG_STAR(0x2 星标), 0..3
 *   } kidpal_schedule_course_t;
 *
 *   #define KSG_NAME_CAP   48
 *   #define KSG_FLAG_TEMP  0x1   -- bit0 ※ 临时安排（B1 一次性安排区块）
 *   #define KSG_FLAG_STAR  0x2   -- bit1 星标呼应（group_upcoming 当前「接下来」项）
 *   #define KSG_COURSE_CAP 84    -- 周网格缓存容量上限 = 7 天 × 3 格 × 每格 4 门
 *                                     （cache_schedule 网格化定容冻结值；渲染节点
 *                                      缓存数组按此容量定容，越界丢弃不溢出）
 *   #define KSG_FONT_MIN_PX 14   -- NFR-READ-1 格内课程名行 pitch 下限（1bpp CJK 实测）
 *
 *   int kidpal_schedule_grid_parse(const char *data_json,
 *                                  kidpal_schedule_course_t *courses, int cap);
 *       解析 schedule view 的 data 对象 JSON 字符串（mcp_server kidpal/display handler
 *       持有的 `data` 参数，非整个 {view,…} 信封）。形状 = {"week":{"courses":[…]}}；
 *       每条目键集精确 {day,slot,name,flags}（day/slot/flags 必须是 JSON 数字且在
 *       0..6 / 0..2 / 0..3 值域；name 必须非缺失非空字符串）。返回课程条数（>=0，
 *       {"week":{"courses":[]}} = 0 空课表合法页）；-1 于：NULL / 畸形 JSON / 缺 "week"
 *       / courses 非数组 / 条目非对象 / 缺 day·slot·name / 越界值 / flags 越域或非
 *       数字 / cap < 条数（绝不溢出/静默截断）。尾部垃圾 = 畸形 → -1。
 *       容忍（additive）：flags 缺省解析为 0；未知扩展字段忽略；JSON 空白（wire
 *       json.dumps 默认 ": "/", "）照常解析。name 按 KSG_NAME_CAP safe-truncate。
 *       -1 = 设备降级信号：调用方必须友好降级、绝不盲渲染（V27）。
 *
 *   int kidpal_schedule_grid_render(const kidpal_schedule_course_t *courses, int n,
 *                                   int today_col);
 *       P2 一周总览网格渲染面（纯 LVGL 画在 ACTIVE screen——lv_obj_/lv_label only,
 *       无 Board/Display class 依赖, host linux target 与设备两侧同源可编）。先清屏
 *       （幂等可反复调用），画 7 列（周一..周日）× 3 行（上午/下午/晚上）整屏网格 +
 *       行首槽位标签列，页内无标题无提示行（v1.20 已删，操作提示归全局键帽条）：
 *       - 版式区钉（布局在位钉，非字体/颜色钉；400×300 减键帽条 chrome）：
 *           网格可用区 y[0..KSG_GRID_BOTTOM=254]（2026-09-26 真机对齐轮 251→254：设计
 *           canvas-p2 网格底线实测 y254、底条上缘 y262；旧值系贴旧底条顶 252 的适配位）；
 *           y>=255 = 网格外带，网格**严格零墨**
 *           （键帽条三槽常驻不被网格遮挡, M3K2-A6）。
 *           表头带 y[0..29]；槽位行带 slot0 y[30..103] / slot1 y[104..177] /
 *           slot2 y[178..254]；标签列 x[0..39]；day 列带 x[40+51*d .. 90+51*d]（d=0..6）。
 *           (day,slot) 格 = day 列带 × slot 行带；格内多课**逐门竖排**（同格多条目
 *           自上而下堆叠），课程名行 pitch >= KSG_FONT_MIN_PX(14)（NFR-READ-1）；
 *           **不做格内滚动**，格内溢出走截断降级（spec S-F1.2 补充规则口径）。
 *       - today_col ∈ 0..6 → 该 day 列带整列高亮（粗框/反色，与未高亮列在真帧上可
 *         区分），高亮墨不越出该列带；today_col 越域（含 -1 = 未注入星期 / 7 / 负值）
 *         → 无任何列高亮（不画假「今天」；兜底纪律同 home_p1 日期未知不画假日期）。
 *       - flags bit0 → ※ 临时标记与课程名同格可见；bit1 → 星标（★）与课程名同格
 *         可见；两标记在真帧上互相可区分（※≠★）；无 flags 的课不带任何标记。
 *       - 返回 n（成功绘制的课程条数, n=0 空课表合法页画空网格骨架）；-1 于非法参数
 *         （n<0、n>0 且 courses==NULL）；绝不崩溃。
 *
 *   void kidpal_render_node_inject_weekday(int mon0_wday);
 *       [device face, 结构齿承载] 渲染节点星期注入缝（M1-K1a RTC 注入家族同款：
 *       inject_rtc/inject_env/inject_battery 之后的第四缝）。mon0_wday = 设备本地
 *       星期（周一=0..周日=6，与 producer day 同基）；0..6 合法写入，越域忽略保持
 *       原值（防御，不崩）。板级在 inject_rtc/inject_env 同一 RTC 装配点调用（LVGL
 *       就绪后）；本地零点跨天后板级重读 RTC 即切换（P2 网格每次落屏重取今天列，
 *       今天列 = 注入星期列；「今天」按设备本地日期零点切换、不依赖云端，NFR-OFF-1
 *       语义一致）。
 *
 *   void kidpal_render_node_cache_schedule_grid(const kidpal_schedule_course_t *courses,
 *                                               int n);
 *       [device face, 结构齿承载] cache_schedule 网格化（旧 16 项列表缓存作废）：
 *       mcp_server schedule 路由解析成功后调此填充周网格缓存（容量 KSG_COURSE_CAP，
 *       n > cap 时只拷前 cap 条、绝不溢出；n<=0 或 courses==NULL 时清空缓存 = 空态）。
 *       BOOT 翻页 cycle_next 浏览读此缓存渲染 P2；缓存空 → 友好空态降级画面
 *       （R-S0.2 纪律沿用），绝不盲渲染。
 *
 *   int kidpal_home_upcoming_apply(const char *data_json);
 *       A5 接线层纯逻辑面（家 = firmware/main/display/home_upcoming.{c,h}；**只消费
 *       home_p1 冻结 seam，home_p1.{c,h} 零修改**）。data_json = home 路由 `data`
 *       参数字符串。形状 = {"upcoming":{"day_key":…,"items":[{time,name}…],
 *       "preview":{time,name}|null}}。语义：
 *       - data_json NULL / "" / "{}" / 无 "upcoming" 键 → 返回 0 且 seam 状态零变化
 *         （旧 {view:home} 无 data 行为完全不变，向后兼容——契约 §1.3⑥ additive）。
 *       - "upcoming" 在：day_key 必须为 1..15 字节字符串；items 数组每项 {time,name}
 *         且 time 可解析 H:MM/HH:MM、name 非空；count <= HP1_ITEM_MAX；preview 可缺/
 *         null（= 无预告）或 {time,name} 同项校验 → 经 kidpal_home_p1_schedule_inject
 *         注入（seam 全量校验，拒绝即整包拒）。成功返回 0；任何畸形/越界/seam 拒收
 *         → 返回 -1 且 seam 状态零变化（绝不半写、绝不盲渲染）。**preview.time 非
 *         H:MM/HH:MM（如「明早」字面）→ -1**（K1 裁决口径：载荷钉 HH:MM；自然语言
 *         标签不入 seam）。
 *       - 设备接线：mcp_server home 路由 data 非空时调 apply；注入成功且 P1 首页在
 *         屏（LOCAL_PAGE HOME 前台）→ 重渲染首页日程主体（云端一推首页跟着换）；
 *         缺/空 data 行为不变（既有 show_home 路径零偏移）。断网无推送 = 日程区不
 *         更新（NFR-OFF-1 语义保持：apply 只在推送到达时被调，设备零轮询）。
 * ===========================================================================
 *
 * M3K2-A7（真机网格目测 + 翻页手感）挂账声明：真机半 deferred 归 M3-K3 设备轮
 * （crossPackageBehaviors 第 3 条，单真机不两轮撞车）。本文件冻结其结构半 =
 * 「P2 网格页模块在设备树 + 编入设备构建 CMakeLists（device build 闸 M3K2-E3 的
 * 装配事实面）」；真机 1bpp 目测 / BOOT 翻页手感 / 今天列真机观感**不在本包验收
 * 执行面**，最终目击证据归 260921-m3-k3-voice-schedule-entry-live（M3K3-A3）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria / sourceRef -> test):
 *   coverage: M3K2-A1（周网格版式齿 WF-F1-02: 7 列×3 行整屏网格 + 格内多课竖排 +
 *             字号 >=14px + 不做格内滚动）-> test 1（结构半: 模块在世 + 设备构建
 *             装配 + 渲染面接线 + 降级语义标记 + A7 结构半）+ test 8（render 真帧:
 *             格归属 diff 定位 + 键帽条带零墨）+ test 9（render 真帧: 行 pitch
 *             >=14px 真测 + 二课同格不外溢 = 不做格内滚动的承载半）
 *   coverage: M3K2-A2（今天列高亮齿: 设备本地日期推导 + 整列高亮 + 零点切换）->
 *             test 3（结构半: inject_weekday 缝 + 板级 RTC 装配点接线 + 家族在世
 *             守卫）+ test 10（render 真帧: today_col 0..6 逐列高亮带浓度 + 跨天
 *             切换 diff + 越域不高亮）
 *   coverage: M3K2-A3（※ 临时标记 + 星标呼应齿: flags 位渲染）-> test 2（行为半:
 *             flags 0..3 逐位保真解析）+ test 11（render 真帧: ※/★ 各自可见 +
 *             互相可区分 + 无 flags 不带标记）
 *   coverage: M3K2-A4（新形状消费 + 缓存齿: 周网格缓存 + 自足式最小 JSON 解析 +
 *             畸形/缺字段/越界 cap 一律 -1 + V27）-> test 1（结构半: 路由消费新形状
 *             + 缓存 setter 接线）+ test 2（行为半: canonical/空白容忍/空课表/薄载荷/
 *             旧形状/畸形矩阵/cap 越界/safe-truncate 全覆盖）
 *   coverage: M3K2-A5（首页「接下来」注入齿: 冻结 seam 消费 + 缺/空 data 行为不变 +
 *             NFR-OFF-1）-> test 4（行为半: canonical 注入/previewer null/缺 data
 *             零变化/畸形 -1 零变化/「明早」拒收）+ test 5（结构半: apply 接线 +
 *             home_p1 seam 零改 canary）
 *   coverage: M3K2-A6（页循环/键帽条回归靶，非修订面）-> test 6（三页循环 PAGE_COUNT/
 *             路由续接/cycle 模步/键帽条刷新在世/key_layer_dispatch 齿接线在世/
 *             keybar P2 槽表在世——既有齿零偏移通过）+ test 8 键帽条带零墨齿
 *   coverage: M3K2-A7（structural 挂账）-> test 1 结构半（设备构建装配事实）+ 本文件头
 *             挂账声明（真机半归 K3）
 *   coverage: M3K2-E2（铁律 canary）-> test 5（home_p1 seam 零改）+ test 6（回归靶
 *             在世守卫）
 *
 * ---------------------------------------------------------------------------
 * BUILDER DUTY 清单（HOST_TEST_CONVENTIONS.md [C4.2] 约定）：
 *   1) firmware/main/display/schedule_page.{c,h} 整页重写：落 kidpal_schedule_grid_parse
 *      / kidpal_schedule_grid_render（FROZEN OFFLINE CONTRACT 上文）；自足式最小 JSON
 *      扫描（无外部 JSON 库）、safe-truncate、-1 降级纪律沿用。
 *   2) firmware/main/display/kidpal_render_node.{h,cc}：落 kidpal_render_node_inject_weekday
 *      （.h C linkage 声明 + .cc 定义）与 kidpal_render_node_cache_schedule_grid（网格
 *      缓存 KSG_COURSE_CAP 定容）；cycle_next KP_PAGE_SCHEDULE 分支改从网格缓存经
 *      kidpal_schedule_grid_render 渲染（today_col = 注入星期列；缓存空 → 友好空态）。
 *   3) firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc：板级 RTC
 *      装配点（inject_rtc/inject_env 同点）调用 kidpal_render_node_inject_weekday。
 *   4) firmware/main/display/home_upcoming.{c,h} 新建（A5 纯逻辑接线层）+ mcp_server.cc
 *      home 路由 data 非空时消费 kidpal_home_upcoming_apply + P1 在屏重渲染接线；
 *      home_p1.{c,h} 与归档 home_p1.acceptance.c **零修改**（铁律）。
 *   5) host 接线：firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *      加 "../../schedule_grid.acceptance.c" 与被测实现源（schedule_page.c 重写体 +
 *      home_upcoming.c；home_p1.c 已在册），置于 "../../host_infra_smoke.acceptance.c"
 *      之前（smoke 保持末位 = 最先执行）。home_p1.c 强符号覆盖本文件 home_p1 seam
 *      weak 哨兵（p1_home_battery 同款）。
 *   6) device build 闸（M3K2-E3）：schedule_page.c/home_upcoming.c 编入
 *      firmware/main/CMakeLists.txt（本文件 test 1 钉装配事实）。
 *   7) mutation 自证（mutationRequired=true）：逐验收齿做 RED→GREEN 自证——对每条
 *      mutation 记录「变异内容 → 原变异重放转红（红名 verbatim）→ 还原复绿」；
 *      变异命名**不得保留被测符号原字面为子串**（KR-A3 教训——strstr 子串仍命中则
 *      变异不红；用 _STUB 类整词替换）。counterEvidence 按 taskpackage
 *      mutationEvidence.perAcceptance 逐条回填（acceptance/exitCriteria/mutation/
 *      evidence）。
 *
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash -c 'VERIFY_FW_BUILD_REQUIRED=1
 *      bash /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/
 *      espidf_wsl.sh build'
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash -c 'cd /mnt/c/AI/KidPal-pod-m1-
 *      k1a-key-layer-dispatch/firmware/main/test/host_app && VERIFY_FW_BUILD_REQUIRED=1
 *      ./build/host_infra_test.elf'
 *   Freeze-phase red baseline：本文件冻结时点（builder 未接线）以独立 stub 脚手架
 *   （firmware/main/test/acceptance/red-baseline/unity.h + runner.c，非冻结件）把本
 *   文件在 WSL 编原生 ELF 当场执行——行为齿经 weak 哨兵红在「frozen contract not
 *   implemented」，结构齿红在树扫描真实缺失，守卫齿必须保持绿；LVGL render 齿经
 *   __has_include("lvgl.h") 守卫在 stub 编译中排除，其红态在 builder 接线进 host
 *   runner 时兑现（如实记录，schedule_p2 先例同款）。
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

/* 共享底座契约头（相对路径, host runner 与独立 stub 编译都可达）。 */
#include "../key_gesture_parser.h"
#include "../display/view_router.h"
/* home_p1 冻结契约头（seam 形状/常量零偏移；A5 行为齿经本头类型消费 seam 状态）。 */
#include "../display/home_p1.h"

#define KSG_MAX_PATH 1024
#define KSG_MAX_READ (8u * 1024u * 1024u)

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — constants + weak defaults.
 * ------------------------------------------------------------------------ */
#define KSG_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */

#define KSG_NAME_CAP   48
#define KSG_FLAG_TEMP  0x1
#define KSG_FLAG_STAR  0x2
#define KSG_FLAGS_MAX  3
#define KSG_COURSE_CAP 84 /* 7 天 × 3 格 × 每格 4 门（网格缓存定容冻结值） */
#define KSG_GRID_BOTTOM 254
#define KSG_FONT_MIN_PX 14

typedef struct {
    int  day;
    int  slot;
    char name[KSG_NAME_CAP];
    int  flags;
} kidpal_schedule_course_t;

__attribute__((weak)) int kidpal_schedule_grid_parse(const char *data_json,
                                                     kidpal_schedule_course_t *courses,
                                                     int cap)
{
    (void)data_json; (void)courses; (void)cap;
    return KSG_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_schedule_grid_render(const kidpal_schedule_course_t *courses,
                                                      int n, int today_col)
{
    (void)courses; (void)n; (void)today_col;
    return KSG_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_home_upcoming_apply(const char *data_json)
{
    (void)data_json;
    return KSG_UNIMPLEMENTED;
}

/* home_p1 seam 读侧哨兵（stub 编译链路用；host runner 由在世 home_p1.c 强定义覆盖，
 * p1_home_battery.acceptance.c 亦持同型 weak——多重 weak 合法、语义一致）。 */
__attribute__((weak)) int kidpal_home_p1_schedule_count(void)
{
    return KSG_UNIMPLEMENTED;
}
__attribute__((weak)) const hp1_item_t *kidpal_home_p1_schedule_item(int index)
{
    (void)index;
    return NULL;
}
__attribute__((weak)) int kidpal_home_p1_schedule_preview(hp1_item_t *out)
{
    (void)out;
    return KSG_UNIMPLEMENTED;
}

/* 包 2 底座真实强符号（host runner 里由 view_router.c 提供；独立 stub 编译未链接
 * —— 以 weak 指针判空跳过，M3K2-A6 路由续接守卫内注记）。 */
extern __attribute__((weak)) int kidpal_display_route(const char *view);

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), KSP/HPB precedent.
 * All scans run against the REAL working tree located from the runner cwd.
 * Directories named test / host_app / build / .git / assets are EXCLUDED from
 * every scan (test/ hosts THIS frozen file whose comments legitimately mention
 * the frozen needles; host_app/build are build artifacts; assets are binary art).
 * ------------------------------------------------------------------------ */

static char ksg_root[KSG_MAX_PATH];
static int ksg_root_ready;

static int ksg_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int ksg_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int ksg_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int ksg_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KSG_MAX_PATH];

    if (ksg_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (ksg_is_regular(probe)) {
            snprintf(ksg_root, sizeof(ksg_root), "%s", cands[i]);
            ksg_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int ksg_rel(char *out, size_t cap, const char *rel)
{
    if (!ksg_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", ksg_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", ksg_root, rel);
    return 1;
}

static int ksg_exists(const char *rel)
{
    char p[KSG_MAX_PATH];
    return ksg_rel(p, sizeof(p), rel) && ksg_is_regular(p);
}

static char *ksg_read(const char *rel, size_t *out_len)
{
    char p[KSG_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!ksg_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KSG_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int ksg_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int ksg_file_contains(const char *rel, const char *needle)
{
    char *buf = ksg_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = ksg_contains(buf, needle);
    free(buf);
    return r;
}

static int ksg_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = ksg_read(rel, NULL);
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

static int ksg_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
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
        char abs_next[KSG_MAX_PATH];
        char rel_next[KSG_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            ksg_is_skipped_dir(e->d_name)) {
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
            count += ksg_count_files_with_needle(abs_next, rel_next, needle,
                                                 first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KSG_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL);
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                if (ksg_contains(buf, needle)) {
                    if (count == 0 && first_hit != NULL) {
                        snprintf(first_hit, cap, "%s", rel_next);
                    }
                    count++;
                }
            }
            free(buf);
            fclose(f);
        }
    }
    closedir(d);
    return count;
}

static int ksg_scan_count(const char *rel_dir, const char *needle,
                          char *first_hit, size_t cap)
{
    char abs[KSG_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!ksg_rel(abs, sizeof(abs), rel_dir) || !ksg_is_dir(abs)) {
        return 0;
    }
    return ksg_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

/* 消歧扫描（schedule_p2 REVISION LOG 先例）：统计「内容含 needle1（必需）且含
 * alt1|alt2|alt3 至少其一」的文件数——把定位收敛到定义签名文件族，消费调用点/
 * 哨兵文件不再能代表定义模块。 */
static int ksg_count_files_with_needle_alt(const char *abs_dir, const char *rel_prefix,
                                           const char *needle1, const char *alt1,
                                           const char *alt2, const char *alt3,
                                           char *first_hit, size_t cap, int depth)
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
        char abs_next[KSG_MAX_PATH];
        char rel_next[KSG_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            ksg_is_skipped_dir(e->d_name)) {
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
            count += ksg_count_files_with_needle_alt(abs_next, rel_next, needle1,
                                                     alt1, alt2, alt3,
                                                     first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KSG_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL);
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                int m = ksg_contains(buf, needle1) &&
                        ((alt1 != NULL && ksg_contains(buf, alt1)) ||
                         (alt2 != NULL && ksg_contains(buf, alt2)) ||
                         (alt3 != NULL && ksg_contains(buf, alt3)));
                if (m) {
                    if (count == 0 && first_hit != NULL) {
                        snprintf(first_hit, cap, "%s", rel_next);
                    }
                    count++;
                }
            }
            free(buf);
            fclose(f);
        }
    }
    closedir(d);
    return count;
}

static int ksg_scan_count_alt(const char *rel_dir, const char *needle1,
                              const char *alt1, const char *alt2, const char *alt3,
                              char *first_hit, size_t cap)
{
    char abs[KSG_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!ksg_rel(abs, sizeof(abs), rel_dir) || !ksg_is_dir(abs)) {
        return 0;
    }
    return ksg_count_files_with_needle_alt(abs, rel_dir, needle1, alt1, alt2, alt3,
                                           first_hit, cap, 0);
}

#define KSG_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(ksg_locate_root(), \
    "KSG RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen offline contract not being implemented yet. */
static void ksg_require_implemented(int value, const char *what)
{
    if (value == KSG_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KSG RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the pure-logic schedule grid / home upcoming "
                 "per the frozen contract in firmware/main/test/schedule_grid."
                 "acceptance.c header and wire it into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* Canonical schedule grid data payload（契约 §1.3② + schedule.py week_grid_payload
 * 实测 wire 形状——front json.dumps 默认分隔符带空格, day/slot/flags 为 JSON 数字）,
 * as the mcp_server.cc kidpal/display handler holds it: the data OBJECT string. */
static const char *KSG_PAYLOAD_CANONICAL =
    "{\"week\": {\"courses\": [{\"day\": 0, \"slot\": 0, \"name\": \"语文\", \"flags\": 0}, "
    "{\"day\": 0, \"slot\": 2, \"name\": \"口语\", \"flags\": 1}, "
    "{\"day\": 5, \"slot\": 2, \"name\": \"舞蹈\", \"flags\": 2}, "
    "{\"day\": 6, \"slot\": 2, \"name\": \"编程\", \"flags\": 3}]}}";

/* Canonical home upcoming data payload（契约 §1.3⑥ + schedule.py home_upcoming_payload
 * 实测 wire 形状：day_key 恒「今天」、preview.time 恒 HH:MM）。 */
static const char *KSG_UPCOMING_CANONICAL =
    "{\"upcoming\": {\"day_key\": \"今天\", "
    "\"items\": [{\"time\": \"14:00\", \"name\": \"钢琴课\"}, "
    "{\"time\": \"16:30\", \"name\": \"英语培训\"}], "
    "\"preview\": {\"time\": \"08:30\", \"name\": \"春游\"}}}";

/* -------------------------------------------------------------------------- *
 * M3K2-A1/A4 (+A7 结构半) — P2 周网格消费面装配结构（structural, WF-F1-02 /
 * 契约 §1.3②）。Teeth, each a REAL repo fact:
 *   1) a grid page module exists under firmware/main implementing the frozen
 *      contract symbol kidpal_schedule_grid_parse(;
 *   2) the module is compiled into the DEVICE build: firmware/main/CMakeLists.txt
 *      lists the file's basename（A7 结构半 = 设备构建装配事实, M3K2-E3 闸面）;
 *   3) the mcp kidpal/display schedule route really consumes the NEW shape:
 *      mcp_server.cc calls kidpal_schedule_grid_parse(（旧列表解析不再是路由面）;
 *   4) the render face is REACHED: kidpal_schedule_grid_render( referenced by
 *      >= 2 distinct files (definition + consumer call site);
 *   5) the parse contract carries the explicit degrade semantic marker
 *      （-1 -> friendly hint, never blind render, V27）;
 *   6) the grid cache is wired: mcp_server.cc -> kidpal_render_node_cache_schedule_grid
 *      call + render_node definition（cache_schedule 网格化, BOOT 翻页浏览源）;
 *   7) the cycle branch renders from the grid cache: kidpal_render_node.cc calls
 *      kidpal_schedule_grid_render(;
 *   guards (must stay green): 包 2 路由底座在世 + kidpal/display 注册。
 * RED today: teeth 1)-7) no implementation; guards green (native).
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: P2 周网格消费面装配结构 (M3K2-A1/M3K2-A4 structural)",
          "[schedule_grid][M3K2-A1][M3K2-A4][structural]")
{
    char hit[KSG_MAX_PATH];
    int n_parse, n_render;

    KSG_REQUIRE_ROOT();

    /* tooth 1: 网格页模块存在于设备树 */
    n_parse = ksg_scan_count("firmware/main", "kidpal_schedule_grid_parse(",
                             hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_parse >= 1,
        "KSG-A1 RED: no module under firmware/main implements the frozen schedule "
        "grid contract (kidpal_schedule_grid_parse absent) — P2 周网格消费面缺失 "
        "(契约 §1.3②, WF-F1-02)");

    /* tooth 2: 模块被设备构建引用（CMakeLists 装配，非浮文件; M3K2-A7 结构半/E3 闸面） */
    {
        char base[KSG_MAX_PATH];
        const char *slash = strrchr(hit, '/');
        snprintf(base, sizeof(base), "%s", slash ? slash + 1 : hit);
        TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/CMakeLists.txt", base),
            "KSG-A1 RED: the schedule grid source is not referenced by "
            "firmware/main/CMakeLists.txt — 模块未编入设备构建（装配缺失, "
            "M3K2-A7 结构半 / M3K2-E3 device build 闸面）");
    }

    /* tooth 3: mcp schedule 路由真正消费新形状（grid parse 进路由） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/mcp_server.cc",
                                               "kidpal_schedule_grid_parse("),
        "KSG-A4 RED: mcp_server.cc does not call kidpal_schedule_grid_parse( — "
        "kidpal/display schedule 路由未消费契约 §1.3② 周网格新形状 "
        "（旧列表形状消费面未退役）");

    /* tooth 4: 渲染面定义 + 消费调用点 >= 2 个文件 */
    n_render = ksg_scan_count("firmware/main", "kidpal_schedule_grid_render(",
                              hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_render >= 2,
        "KSG-A1 RED: kidpal_schedule_grid_render is defined but never called — "
        "P2 网格渲染面未接入消费链路（仅定义不调用不算接入）");

    /* tooth 5: 解析失败 = 显式降级语义标记（绝不盲渲染, V27 纪律在 data 形状上）。
     * 定位收敛到定义签名（schedule_p2 REVISION LOG 先例同款）。 */
    {
        char def_hit[KSG_MAX_PATH];
        int n_def = ksg_scan_count_alt("firmware/main", "int kidpal_schedule_grid_parse(",
                                       "降级", "degrade", "-1", def_hit, sizeof(def_hit));
        TEST_ASSERT_TRUE_MESSAGE(n_def >= 1,
            "KSG-A4 RED: the grid parse definition module carries no parse-degrade "
            "semantic marker — 解析失败必须显式降级而非盲渲染 (V27 / spec §4.0)");
        (void)def_hit;
    }

    /* tooth 6: 周网格缓存接线（mcp 调用 + 渲染节点 setter 定义） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/mcp_server.cc",
                                               "kidpal_render_node_cache_schedule_grid("),
        "KSG-A4 RED: mcp_server.cc does not call kidpal_render_node_cache_schedule_grid( "
        "— 周网格缓存未接（cache_schedule 网格化缺失, BOOT 翻页浏览无源）");
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/display/kidpal_render_node.cc",
                                               "kidpal_render_node_cache_schedule_grid("),
        "KSG-A4 RED: kidpal_render_node.cc carries no kidpal_render_node_cache_schedule_grid "
        "definition — 渲染节点网格缓存 setter 缺失");

    /* tooth 7: cycle 分支从网格缓存渲染 */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/display/kidpal_render_node.cc",
                                               "kidpal_schedule_grid_render("),
        "KSG-A1 RED: kidpal_render_node.cc does not call kidpal_schedule_grid_render( — "
        "BOOT 翻页 cycle 分支未接网格渲染面（P2 浏览仍是旧列表/空壳）");

    /* guards (must stay green): 包 2 路由底座在世, 路由续接不回退 */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/mcp_server.cc",
                                               "display/view_router.h"),
        "KSG-A1 RED(infra): mcp_server.cc no longer includes display/view_router.h — "
        "包 2 路由底座被移除");
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/CMakeLists.txt", "view_router.c"),
        "KSG-A1 RED(infra): view_router.c no longer compiled into the device build — "
        "包 2 底座装配被破坏");
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/mcp_server.cc", "kidpal/display"),
        "KSG-A1 RED(infra): kidpal/display tool no longer registered in mcp_server.cc — "
        "display 下发到达面被破坏");
}

/* -------------------------------------------------------------------------- *
 * M3K2-A4 (device face) — 周网格新形状载荷解析与降级（behavioral, frozen contract）
 * Cross-end pinned: day/slot/flags 是 JSON 数字; wire 默认分隔符带空格; 薄载荷
 * data="{}" 与旧列表形状都必须 -1（绝不盲渲染）。RED today: weak sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: 周网格新形状载荷解析与降级 (M3K2-A4)",
          "[schedule_grid][M3K2-A4]")
{
    kidpal_schedule_course_t courses[KSG_COURSE_CAP];
    int n;

    /* canonical payload: 4 courses, fields exact, flags 逐位保真（跨端 wire 带空格） */
    memset(courses, 0, sizeof(courses));
    n = kidpal_schedule_grid_parse(KSG_PAYLOAD_CANONICAL, courses, KSG_COURSE_CAP);
    ksg_require_implemented(n, "kidpal_schedule_grid_parse unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(n == 4,
        "KSG-A4 RED: canonical contract §1.3② payload did not parse to exactly 4 courses");
    TEST_ASSERT_TRUE_MESSAGE(courses[0].day == 0 && courses[0].slot == 0 &&
                                 strcmp(courses[0].name, "语文") == 0 &&
                                 courses[0].flags == 0,
        "KSG-A4 RED: course 0 fields wrong — day/slot/name/flags 必须逐字段保真 "
        "(契约 §1.3②, day 0 基 周一=0)");
    TEST_ASSERT_TRUE_MESSAGE(courses[1].day == 0 && courses[1].slot == 2 &&
                                 strcmp(courses[1].name, "口语") == 0 &&
                                 courses[1].flags == KSG_FLAG_TEMP,
        "KSG-A4 RED: course 1 flags bit0 (※ 临时) lost in parse — 临时标记位必须保真");
    TEST_ASSERT_TRUE_MESSAGE(courses[2].day == 5 && courses[2].slot == 2 &&
                                 courses[2].flags == KSG_FLAG_STAR,
        "KSG-A4 RED: course 2 flags bit1 (星标) lost in parse — 星标呼应位必须保真");
    TEST_ASSERT_TRUE_MESSAGE(courses[3].day == 6 && courses[3].flags == 3,
        "KSG-A4 RED: course 3 combined flags (※+星标) lost in parse");

    /* 一格多门 = 同 (day,slot) 多条逐门保留（producer 逐门下发, WF-F1-02 竖排数据面） */
    {
        const char *p = "{\"week\":{\"courses\":[{\"day\":2,\"slot\":0,\"name\":\"语文\","
                        "\"flags\":0},{\"day\":2,\"slot\":0,\"name\":\"数学\",\"flags\":0}]}}";
        memset(courses, 0, sizeof(courses));
        n = kidpal_schedule_grid_parse(p, courses, KSG_COURSE_CAP);
        TEST_ASSERT_TRUE_MESSAGE(n == 2,
            "KSG-A4 RED: same-cell multi-course entries must each survive the parse "
            "（一格多门逐门列出, 不得合并/丢弃）");
    }

    /* tolerant: missing flags -> 0, extra unknown fields ignored (additive tolerance) */
    {
        const char *p = "{\"week\":{\"courses\":[{\"day\":1,\"slot\":1,\"name\":\"美术\","
                        "\"extra\":1}],\"tz\":\"Asia/Shanghai\"}}";
        memset(courses, 0, sizeof(courses));
        n = kidpal_schedule_grid_parse(p, courses, KSG_COURSE_CAP);
        TEST_ASSERT_TRUE_MESSAGE(n == 1,
            "KSG-A4 RED: payload with missing flags / extra unknown fields must parse "
            "（flags 缺省 0、扩展位忽略, additive tolerance）——不得因扩展位整包拒绝");
        TEST_ASSERT_TRUE_MESSAGE(courses[0].flags == 0,
            "KSG-A4 RED: missing flags must default to 0");
    }

    /* empty courses array is a valid (empty) week page */
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[]}}", courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KSG-A4 RED: empty courses array must parse to 0 (空课表合法页)");

    /* degrade: 薄载荷 data="{}"（front 截获展开前的意图薄载荷若泄漏直达设备）
     * -> -1（绝不盲渲染, V27）；旧列表形状 {title,items} 无 week 键 -> -1 */
    n = kidpal_schedule_grid_parse("{}", courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: thin payload data={} must degrade (-1) — 薄载荷不直达设备, "
        "泄漏到达也绝不盲渲染 (V27)");
    n = kidpal_schedule_grid_parse(
        "{\"title\":\"今天下午\",\"items\":[{\"time\":\"14:00\",\"name\":\"钢琴课\"}]}",
        courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: old list shape {title,items} must be rejected (-1) — 旧形状已在 "
        "契约门 2026-09-21 废止, 设备侧不得盲渲染旧列表");

    /* degrade matrix: malformed / missing / out-of-range -> -1 */
    n = kidpal_schedule_grid_parse("{\"week\":", courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSG-A4 RED: malformed JSON must return -1");
    n = kidpal_schedule_grid_parse(NULL, courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSG-A4 RED: NULL payload must be defended (-1)");
    n = kidpal_schedule_grid_parse("{\"courses\":[]}", courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: missing week must be rejected (-1) — week 是契约 data 形状必带键");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":{}}}", courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSG-A4 RED: non-array courses must be rejected (-1)");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[1]}}", courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSG-A4 RED: non-object course entry must be rejected (-1)");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[{\"slot\":0,\"name\":\"x\",\"flags\":0}]}}",
                                   courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSG-A4 RED: course missing day must be rejected (-1)");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[{\"day\":7,\"slot\":0,\"name\":\"x\",\"flags\":0}]}}",
                                   courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: day=7 out of range must be rejected (-1) — day 值域 0..6");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[{\"day\":0,\"slot\":3,\"name\":\"x\",\"flags\":0}]}}",
                                   courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: slot=3 out of range must be rejected (-1) — slot 值域 0..2");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[{\"day\":0,\"slot\":0,\"flags\":0}]}}",
                                   courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSG-A4 RED: course missing name must be rejected (-1)");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[{\"day\":0,\"slot\":0,\"name\":5,\"flags\":0}]}}",
                                   courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSG-A4 RED: non-string name must be rejected (-1)");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[{\"day\":0,\"slot\":0,\"name\":\"x\",\"flags\":4}]}}",
                                   courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: flags=4 beyond bit0|bit1 must be rejected (-1) — flags 值域 0..3");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[{\"day\":0,\"slot\":0,\"name\":\"x\",\"flags\":\"1\"}]}}",
                                   courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: non-integer flags must be rejected (-1) — 跨端 wire flags 是数字");
    n = kidpal_schedule_grid_parse("{\"week\":{\"courses\":[]}}垃圾", courses, KSG_COURSE_CAP);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: trailing garbage must be rejected (-1) — 尾部垃圾 = 畸形 JSON");

    /* degrade: cap too small -> -1 (never overflow/silent truncate) */
    memset(courses, 0, sizeof(courses));
    n = kidpal_schedule_grid_parse(KSG_PAYLOAD_CANONICAL, courses, 3);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A4 RED: cap < course count must return -1, never overflow/silent truncate");

    /* name safe-truncate: 60-byte name parses, stays within KSG_NAME_CAP (NUL-terminated) */
    {
        const char *p = "{\"week\":{\"courses\":[{\"day\":0,\"slot\":0,\"name\":"
                        "\"一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十"
                        "一二三四五六七八九十一二三四五六七八九十\",\"flags\":0}]}}";
        memset(courses, 0, sizeof(courses));
        n = kidpal_schedule_grid_parse(p, courses, KSG_COURSE_CAP);
        TEST_ASSERT_TRUE_MESSAGE(n == 1,
            "KSG-A4 RED: over-long name must still parse (safe-truncate, not reject)");
        TEST_ASSERT_TRUE_MESSAGE(strlen(courses[0].name) <= KSG_NAME_CAP - 1,
            "KSG-A4 RED: truncated name must stay within KSG_NAME_CAP (never overflow)");
    }
}

/* -------------------------------------------------------------------------- *
 * M3K2-A2 (structural) — 今天列推导注入缝接线（设备本地日期 → 星期注入 → 今天列）。
 * 「今天」按设备本地日期零点切换、不依赖云端（spec S-F1.2 补充规则, NFR-OFF-1 语义
 * 一致）；注入缝命名与签名对齐 inject_rtc/inject_env/inject_battery 家族。
 * RED today: 无任何星期注入缝（RTC 装配点缺接线）。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: 今天列推导注入 seam 接线 (M3K2-A2 structural)",
          "[schedule_grid][M3K2-A2][structural]")
{
    KSG_REQUIRE_ROOT();

    /* tooth 1: 渲染节点注入缝声明（.h, C linkage 家族同款） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/display/kidpal_render_node.h",
                                               "kidpal_render_node_inject_weekday("),
        "KSG-A2 RED: kidpal_render_node.h declares no kidpal_render_node_inject_weekday( "
        "— 星期注入缝缺失（设备本地日期→今天列推导无入口, S-F1.2 今天列）");

    /* tooth 2: 渲染节点注入缝定义（.cc） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_count_in_file("firmware/main/display/kidpal_render_node.cc",
                                               "kidpal_render_node_inject_weekday(") >= 1,
        "KSG-A2 RED: kidpal_render_node.cc defines no kidpal_render_node_inject_weekday( "
        "— 注入缝只有声明没有实现");

    /* tooth 3: 板级 RTC 装配点真接线（inject_rtc/inject_env 同点消费） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains(
                                 "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc",
                                 "kidpal_render_node_inject_weekday("),
        "KSG-A2 RED: the board RTC assembly point does not call "
        "kidpal_render_node_inject_weekday( — 板级星期数据未接线, 今天列推导无数据源 "
        "（本地零点跨天切换无承载）");

    /* guard: inject 家族在世（inject_rtc 兄弟缝不被本包拆除） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/display/kidpal_render_node.cc",
                                               "kidpal_render_node_inject_rtc("),
        "KSG-A2 RED(infra): kidpal_render_node_inject_rtc no longer present — "
        "RTC 注入家族被破坏（M1-K1a 冻结面回归）");
}

/* -------------------------------------------------------------------------- *
 * M3K2-A5 (behavioral) — 首页「接下来」data.upcoming 消费注入冻结 seam。
 * seam = home_p1 冻结面（零修改, 只消费）; preview null 容忍; 缺/空 data 行为不变;
 * 「明早」等非 HH:MM preview.time 拒收（K1 裁决口径）。RED today: weak sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("home upcoming: data.upcoming 消费注入冻结 seam (M3K2-A5)",
          "[home_upcoming][M3K2-A5]")
{
    const hp1_item_t *it;
    hp1_item_t pv;
    int before, n;

    /* canonical payload -> seam 注入成功, 逐字段可读回 */
    n = kidpal_home_upcoming_apply(KSG_UPCOMING_CANONICAL);
    ksg_require_implemented(n, "kidpal_home_upcoming_apply unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KSG-A5 RED: canonical contract §1.3⑥ upcoming payload must apply cleanly (0)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 2,
        "KSG-A5 RED: seam item count must be 2 after canonical apply — 注入未达冻结 seam");
    it = kidpal_home_p1_schedule_item(0);
    TEST_ASSERT_TRUE_MESSAGE(it != NULL && strcmp(it->time, "14:00") == 0 &&
                                 strcmp(it->name, "钢琴课") == 0,
        "KSG-A5 RED: seam item 0 must carry 14:00 钢琴课 verbatim (payload 即 seam 注入"
        "参数的 JSON 化, 契约 §1.3⑥)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_preview(&pv) == 0 &&
                                 strcmp(pv.time, "08:30") == 0 &&
                                 strcmp(pv.name, "春游") == 0,
        "KSG-A5 RED: preview must land in the frozen seam verbatim (08:30 春游)");

    /* preview null（producer 明日无项时发 null）-> 成功且预告清空, 不降级 */
    n = kidpal_home_upcoming_apply(
        "{\"upcoming\": {\"day_key\": \"今天\", "
        "\"items\": [{\"time\": \"09:00\", \"name\": \"上学\"}], \"preview\": null}}");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KSG-A4/A5 RED: preview:null must be tolerated (明日无项 = 无预告, 不降级) — "
        "producer wire 实况 (schedule.py home_upcoming_payload)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == 1,
        "KSG-A5 RED: seam item count must be 1 after null-preview apply");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_preview(&pv) == -1,
        "KSG-A5 RED: after null preview the seam must report no preview (-1)");

    /* 缺/空 data -> 0 且 seam 状态零变化（旧 {view:home} 行为完全不变, 向后兼容） */
    before = kidpal_home_p1_schedule_count();
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_upcoming_apply(NULL) == 0,
        "KSG-A5 RED: NULL data (legacy {view:home}) must be a no-op success (0)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_upcoming_apply("") == 0,
        "KSG-A5 RED: empty data string must be a no-op success (0)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_upcoming_apply("{}") == 0,
        "KSG-A5 RED: empty object data must be a no-op success (0)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_upcoming_apply("{\"other\":1}") == 0,
        "KSG-A5 RED: data without upcoming key must be a no-op success (0)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == before,
        "KSG-A5 RED: no-op applies must leave the frozen seam state untouched "
        "（缺/空 data 行为不变, 向后兼容）");

    /* 畸形/越界/seam 拒收 -> -1 且 seam 状态零变化（绝不半写、绝不盲渲染） */
    before = kidpal_home_p1_schedule_count();
    n = kidpal_home_upcoming_apply("{\"upcoming\": {\"items\": []}}");
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A5 RED: upcoming without day_key must degrade (-1) — day_key 必带");
    n = kidpal_home_upcoming_apply(
        "{\"upcoming\": {\"day_key\": \"今天\", \"items\": [{\"name\": \"钢琴课\"}]}}");
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A5 RED: item missing time must degrade (-1) — time 必带");
    n = kidpal_home_upcoming_apply(
        "{\"upcoming\": {\"day_key\": \"今天\", \"items\": [{\"time\": \"14:00\", "
        "\"name\": \"钢琴课\"}], \"preview\": {\"time\": \"明早\", \"name\": \"春游\"}}}");
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A5 RED: non-HH:MM preview.time (明早 literal) must be rejected (-1) — "
        "K1 裁决口径: 载荷钉明日首项 HH:MM, 自然语言标签不入冻结 seam, 绝不盲渲染");
    n = kidpal_home_upcoming_apply(
        "{\"upcoming\": {\"day_key\": \"今天\", \"items\": ["
        "{\"time\": \"09:00\", \"name\": \"1\"}, {\"time\": \"10:00\", \"name\": \"2\"}, "
        "{\"time\": \"11:00\", \"name\": \"3\"}, {\"time\": \"12:00\", \"name\": \"4\"}, "
        "{\"time\": \"13:00\", \"name\": \"5\"}, {\"time\": \"14:00\", \"name\": \"6\"}, "
        "{\"time\": \"15:00\", \"name\": \"7\"}, {\"time\": \"16:00\", \"name\": \"8\"}, "
        "{\"time\": \"17:00\", \"name\": \"9\"}]}}");
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSG-A5 RED: 9 items beyond HP1_ITEM_MAX(8) must degrade (-1) — seam 容量越界 "
        "整包拒收");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_schedule_count() == before,
        "KSG-A5 RED: every rejected apply must leave the seam state untouched "
        "（拒绝整包不半写, home_p1 seam 冻结语义）");
}

/* -------------------------------------------------------------------------- *
 * M3K2-A5/E2 (structural) — 注入接线结构 + home_p1 seam 零改 canary（铁律）。
 * RED today: mcp home 路由未消费 upcoming / apply 模块不存在。
 * ------------------------------------------------------------------------ */
TEST_CASE("home upcoming: 注入接线结构 + home_p1 seam 零改 canary (M3K2-A5/E2)",
          "[home_upcoming][M3K2-A5][structural]")
{
    char hit[KSG_MAX_PATH];
    int n_apply;

    KSG_REQUIRE_ROOT();

    /* tooth 1: apply 定义 + mcp 消费调用点 >= 2 个文件 */
    n_apply = ksg_scan_count("firmware/main", "kidpal_home_upcoming_apply(",
                             hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_apply >= 2,
        "KSG-A5 RED: kidpal_home_upcoming_apply needs >= 2 referencing files "
        "(definition + mcp home route consumer) — 定义而未被路由消费不算接线");

    /* tooth 2: mcp home 路由消费 upcoming（data 非空时 apply） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/mcp_server.cc", "upcoming"),
        "KSG-A5 RED: mcp_server.cc home route carries no upcoming consumption — "
        "首页「接下来」注入接线缺失 (契约 §1.3⑥, S-F1.1 AC1/AC2 消费半)");

    /* tooth 3: 接线层模块编入设备构建 */
    {
        char base[KSG_MAX_PATH];
        const char *slash = strrchr(hit, '/');
        snprintf(base, sizeof(base), "%s", slash ? slash + 1 : hit);
        TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/CMakeLists.txt", base),
            "KSG-A5 RED: the home upcoming module is not referenced by "
            "firmware/main/CMakeLists.txt — 接线层未编入设备构建");
    }

    /* canary (must stay green): home_p1 冻结 seam 零修改（E2 铁律——仅消费） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains(
                                 "firmware/main/display/home_p1.h",
                                 "kidpal_home_p1_schedule_inject(const char *day_key,"),
        "KSG-E2 RED(infra): home_p1.h frozen seam signature drifted — "
        "冻结 seam 签名不得偏移");
    TEST_ASSERT_FALSE_MESSAGE(ksg_file_contains("firmware/main/display/home_p1.c", "upcoming"),
        "KSG-E2 RED: home_p1.c consumes upcoming — home_p1 冻结 seam 必须零修改 "
        "（A5 接线层在 home_upcoming 模块, 不回写冻结面）");
    TEST_ASSERT_FALSE_MESSAGE(
        ksg_file_contains("firmware/main/display/home_p1.c", "kidpal_home_upcoming_apply"),
        "KSG-E2 RED: home_p1.c references kidpal_home_upcoming_apply — 冻结面零修改 "
        "canary 被破坏");
}

/* -------------------------------------------------------------------------- *
 * M3K2-A6 — 页循环/键帽条回归靶（回归齿非修订面, 既有齿零偏移）。冻结时点必须
 * 全绿（守卫）: 三页循环 PAGE_COUNT=3、schedule/home 路由续接、cycle 模步在世、
 * 键帽条刷新接线在世、key_layer_dispatch 冻结齿接线在世、keybar P2 槽表在世。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: 回归靶——三页循环/键帽条/路由续接 (M3K2-A6)",
          "[schedule_grid][M3K2-A6][guards]")
{
    KSG_REQUIRE_ROOT();

    /* 三页循环 HOME→SCHEDULE→STORY（M2-K2 既定, P4 撤页后枚举=3） */
    TEST_ASSERT_TRUE_MESSAGE(PAGE_COUNT == 3,
        "KSG-A6 RED: PAGE_COUNT != 3 — 三页循环枚举被本包改动（回归靶非修订面）");

    /* 路由续接不回退（host-runner-only: 独立 stub 未链接 view_router.c 时跳过） */
    if (kidpal_display_route != NULL) {
        TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("schedule") == VRT_ROUTE_SCHEDULE,
            "KSG-A6 RED: view \"schedule\" no longer routes to VRT_ROUTE_SCHEDULE — "
            "包 2 共享路由底座续接被破坏 (契约 §1.3②)");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("home") == VRT_ROUTE_HOME,
            "KSG-A6 RED: view \"home\" no longer routes to VRT_ROUTE_HOME — "
            "A5 注入接线挂在 home 路由上, 路由面不得漂移");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("__ksg_bogus__") == VRT_ROUTE_UNKNOWN,
            "KSG-A6 RED: unknown view must still degrade to VRT_ROUTE_UNKNOWN (V27)");
    }

    /* cycle 模步在世（BOOT 翻页三页回绕载体不被网格重写拆掉） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/display/kidpal_render_node.cc",
                                               "% KP_PAGE_COUNT"),
        "KSG-A6 RED: cycle_next modulo step (% KP_PAGE_COUNT) absent — "
        "三页回绕载体被改动（回归靶非修订面）");

    /* 键帽条刷新接线在世（页面渲染权威每页落屏后驱动三槽刷新, KB-A1） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_count_in_file("firmware/main/display/kidpal_render_node.cc",
                                               "rn_keybar_sync_locked") >= 2,
        "KSG-A6 RED: rn_keybar_sync_locked call sites < 2 — 键帽条三槽刷新接线被 "
        "网格重写拆掉（键帽条常驻不被遮挡, M3K2-A6）");

    /* 回归靶冻结齿仍在 host runner 接线（零偏移判据的装配事实） */
    TEST_ASSERT_TRUE_MESSAGE(
        ksg_file_contains("firmware/main/test/host_app/main/CMakeLists.txt",
                          "../../key_layer_dispatch.acceptance.c"),
        "KSG-A6 RED(infra): key_layer_dispatch.acceptance.c no longer wired into "
        "HOST_ACCEPTANCE_SRCS — 键位分层派发回归靶被摘除");

    /* keybar P2 槽表在世（「问课程」右槽页槽表行不被本包改动） */
    TEST_ASSERT_TRUE_MESSAGE(ksg_file_contains("firmware/main/display/keybar.c", "问课程"),
        "KSG-A6 RED: keybar.c P2 slot row (问课程) missing — 键帽条页槽表被改动");
}

/* -------------------------------------------------------------------------- *
 * boundary / security — 扫描器自检（防假绿）。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: 扫描器自检 (boundary/security)",
          "[schedule_grid][boundary][security]")
{
    char hit[KSG_MAX_PATH];
    int n;

    KSG_REQUIRE_ROOT();

    /* scanner integrity: real file exists / bogus does not / no-match needle matches
     * nothing / counter counts a known literal — negative teeth must be CAPABLE of
     * failing (防假绿). */
    TEST_ASSERT_TRUE_MESSAGE(ksg_exists("firmware/main/display/view_router.c"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(ksg_exists("firmware/main/display/__ksg_no_such_file__.c"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(ksg_read("firmware/main/display/__ksg_no_such_file__.c", NULL),
        "boundary RED(infra): ksg_read returned data for a nonexistent file");
    n = ksg_scan_count("firmware/main/display", "__ksg_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = ksg_count_in_file("firmware/main/display/view_router.c", "VRT_ROUTE_SCHEDULE");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "boundary RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
}

/* -------------------------------------------------------------------------- *
 * Render-face teeth — host-runner-only LVGL 软渲染真帧探针（evidence for A1 版式 /
 * A2 今天列 / A3 标记; 真实屏幕观感殿后 M3K2-A7 真机半）。机制同 schedule_p2 先例：
 * 包 0 冻结渲染缝 host_infra_render_frame() 初始化 + 置 rendered 态, 被测渲染契约
 * 函数在活动屏真画, lv_refr_now 后 host_infra_read_pixel 全分辨率采样断言真实像素。
 * 需要 lvgl.h include path — 独立红基线 stub 编译时被编译排除（如实记录, 其红态在
 * builder 接线进 host runner 时兑现）。
 *
 * 探针纪律（furniture-invariant）：版式断言全部用**帧 diff 定位**——家具（表格线/
 * 表头/槽位标签）在两帧恒同、diff 相消, 断言只锁「差集像素落在哪个区钉带」。
 * 区钉带边界 2px 内缩采样（表格分隔线画在格边界时不入样）。
 * ------------------------------------------------------------------------ */
#if defined(__has_include)
#if __has_include("lvgl.h")
#define KSG_HAVE_LVGL 1
#include <lvgl.h>
#endif
#endif

#ifdef KSG_HAVE_LVGL

extern int host_infra_render_frame(void);
extern int32_t host_infra_read_pixel(int x, int y);

#define KSG_FB_W 400
#define KSG_FB_H 300
#define KSG_WHITE 0xFFFFFFul

static int32_t f_a[KSG_FB_W * KSG_FB_H];
static int32_t f_b[KSG_FB_W * KSG_FB_H];

static void ksg_snap(int32_t *buf)
{
    for (int y = 0; y < KSG_FB_H; y++) {
        for (int x = 0; x < KSG_FB_W; x++) {
            buf[y * KSG_FB_W + x] = host_infra_read_pixel(x, y);
        }
    }
}

static void ksg_render_snap(const kidpal_schedule_course_t *courses, int n, int today_col,
                            int32_t *buf)
{
    int r = kidpal_schedule_grid_render(courses, n, today_col);
    ksg_require_implemented(r, "kidpal_schedule_grid_render unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == n,
        "KSG render RED: render face did not report the full course count drawn");
    lv_refr_now(NULL);
    ksg_snap(buf);
}

/* diff 统计：a/b 相异像素总数与落在 [x0,y0,x1,y1]（含端点）带内的数目。 */
static void ksg_diff_stats(const int32_t *a, const int32_t *b,
                           int x0, int y0, int x1, int y1,
                           int *out_total, int *out_in_band)
{
    int total = 0, in_band = 0;
    if (x0 < 0) { x0 = 0; }
    if (y0 < 0) { y0 = 0; }
    if (x1 >= KSG_FB_W) { x1 = KSG_FB_W - 1; }
    if (y1 >= KSG_FB_H) { y1 = KSG_FB_H - 1; }
    for (int y = 0; y < KSG_FB_H; y++) {
        for (int x = 0; x < KSG_FB_W; x++) {
            if (a[y * KSG_FB_W + x] != b[y * KSG_FB_W + x]) {
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

/* 带内暗墨（非白、非未渲染哨兵）垂直跨度与暗像素计数（x/y 各内缩 2px 采样）。 */
static void ksg_ink_span(const int32_t *f, int x0, int y0, int x1, int y1,
                         int *out_min_y, int *out_max_y, int *out_dark)
{
    int min_y = -1, max_y = -1, dark = 0;
    for (int y = y0 + 2; y <= y1 - 2; y++) {
        for (int x = x0 + 2; x <= x1 - 2; x++) {
            int32_t p = f[y * KSG_FB_W + x];
            if (p >= 0 && p != (int32_t)KSG_WHITE) {
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

static void ksg_day_band(int day, int *x0, int *x1)
{
    *x0 = 40 + 51 * day;
    *x1 = *x0 + 50;
}

static void ksg_slot_band(int slot, int *y0, int *y1)
{
    static const int ys[3][2] = { { 30, 103 }, { 104, 177 }, { 178, 251 } };
    *y0 = ys[slot][0];
    *y1 = ys[slot][1];
}

static kidpal_schedule_course_t *ksg_course(kidpal_schedule_course_t *c, int day, int slot,
                                            const char *name, int flags)
{
    memset(c, 0, sizeof(*c));
    c->day = day;
    c->slot = slot;
    snprintf(c->name, sizeof(c->name), "%s", name);
    c->flags = flags;
    return c;
}

/* ------------------------------------------------------------------------ *
 * M3K2-A1 render — 网格版式真帧：(day,slot) 格归属 diff 定位 + 键帽条带零墨。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: 网格版式真帧——格归属与零溢出 (M3K2-A1 render)",
          "[schedule_grid][M3K2-A1][render]")
{
    kidpal_schedule_course_t cs[2];
    int total, in_band, x0, x1, y0, y1, dark;

    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KSG render RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    /* 格归属：同一槽位行两门课分别落 (day2,slot1) 与 (day4,slot1)——diff 像素必须
     * 全部落在两个格带并集内（7×3 网格格归属的真实证据） */
    ksg_render_snap(ksg_course(&cs[0], 2, 1, "语文", 0), 1, -1, f_a);
    ksg_render_snap(ksg_course(&cs[1], 4, 1, "数学", 0), 1, -1, f_b);
    ksg_diff_stats(f_a, f_b, 0, 0, KSG_FB_W - 1, KSG_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "KSG-A1 render RED: two courses in different day columns produce identical "
        "frames — 课程没有按 (day,slot) 落格渲染");
    ksg_day_band(2, &x0, &x1);
    ksg_slot_band(1, &y0, &y1);
    {
        int full_total, yb0 = y0, yb1 = y1, in2;
        int xa0 = x0, xa1 = x1;
        ksg_diff_stats(f_a, f_b, xa0, yb0, xa1, yb1, &total, &in_band);
        ksg_day_band(4, &x0, &x1);
        ksg_diff_stats(f_a, f_b, x0, yb0, x1, yb1, &full_total, &in2);
        TEST_ASSERT_TRUE_MESSAGE(in_band + in2 == full_total,
            "KSG-A1 render RED: course-diff pixels fall outside their (day,slot) cell "
            "bands — 网格格归属失真（WF-F1-02 版式区钉）");
    }

    /* 网格外带零墨：网格可用区下缘 y>=255 严格无墨（旧口径 y>=252 系贴旧底条顶 252；
     * 底条顶 2026-09-26 移到 262——本齿只钉网格自身不外溢, 底条带本身由其 chrome 齿钉） */
    dark = 0;
    for (int y = KSG_GRID_BOTTOM + 1; y < KSG_FB_H; y++) {
        for (int x = 0; x < KSG_FB_W; x++) {
            int32_t p = f_a[y * KSG_FB_W + x];
            if (p >= 0 && p != (int32_t)KSG_WHITE) {
                dark++;
            }
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(dark == 0,
        "KSG-A1/A6 render RED: grid frame paints ink below y=254 — 网格溢出可用区下缘 "
        "（键帽条三槽常驻不被网格遮挡）");

    /* 空网格 + 非法参数：不崩 */
    {
        int r = kidpal_schedule_grid_render(NULL, 0, -1);
        TEST_ASSERT_TRUE_MESSAGE(r == 0,
            "KSG-A1 render RED: empty course set must render an empty grid skeleton (0)");
        lv_refr_now(NULL);
        r = kidpal_schedule_grid_render(NULL, 3, -1);
        TEST_ASSERT_TRUE_MESSAGE(r == -1,
            "KSG-A1 render RED: NULL courses with n>0 must be rejected (-1), never crash");
        lv_refr_now(NULL);
        r = kidpal_schedule_grid_render(cs, -1, -1);
        TEST_ASSERT_TRUE_MESSAGE(r == -1,
            "KSG-A1 render RED: negative n must be rejected (-1), never crash");
        lv_refr_now(NULL);
    }
}

/* ------------------------------------------------------------------------ *
 * M3K2-A1/NFR-READ-1 render — 格内多课竖排行 pitch >= 14px 真测 + 不做格内滚动
 * （二课同格 diff 全落本格带 = 不外溢不滚走）。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: 格内行 pitch 14px 下限真帧 (M3K2-A1/NFR-READ-1 render)",
          "[schedule_grid][M3K2-A1][render]")
{
    kidpal_schedule_course_t cs[2];
    int x0, x1, y0, y1, min_a, max_a, dark_a, min_b, max_b, dark_b;
    int total, in_band;

    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KSG render RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    ksg_day_band(0, &x0, &x1);
    ksg_slot_band(0, &y0, &y1);

    /* 单课帧 vs 双课同格帧：第二课必须在同格内向下堆叠, 且行 pitch >= 14px */
    ksg_render_snap(ksg_course(&cs[0], 0, 0, "语文", 0), 1, -1, f_a);
    ksg_ink_span(f_a, x0, y0, x1, y1, &min_a, &max_a, &dark_a);
    TEST_ASSERT_TRUE_MESSAGE(dark_a > 0 && min_a >= 0,
        "KSG-A1 render RED(infra): course name produced no measurable ink in its cell "
        "— host 子集字体缺「语文」字形或格归属失真（字体现实见 HOST_TEST_CONVENTIONS "
        "「字体现实」节; 若系后者即真红）");

    ksg_course(&cs[0], 0, 0, "语文", 0);
    ksg_course(&cs[1], 0, 0, "数学", 0);
    ksg_render_snap(cs, 2, -1, f_b);
    ksg_ink_span(f_b, x0, y0, x1, y1, &min_b, &max_b, &dark_b);
    TEST_ASSERT_TRUE_MESSAGE(dark_b > dark_a,
        "KSG-A1 render RED: adding a second course to the same cell added no ink — "
        "格内多课逐门竖排缺失 (WF-F1-02)");
    TEST_ASSERT_TRUE_MESSAGE(max_b - min_b >= max_a - min_a + KSG_FONT_MIN_PX,
        "KSG-A1 render RED: stacked second course line pitch < 14px — NFR-READ-1 "
        "1bpp CJK 字号下限被突破（真机 14px 贴线, 12px 起不可辨）");

    /* 不做格内滚动/不外溢：加课引入的 diff 像素必须全部留在本格带内 */
    ksg_diff_stats(f_a, f_b, x0, y0, x1, y1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0 && in_band == total,
        "KSG-A1 render RED: second-course pixels leak outside their cell band — "
        "格内溢出必须走截断降级、绝不滚动/外溢 (spec S-F1.2 补充规则)");
}

/* ------------------------------------------------------------------------ *
 * M3K2-A2 render — 今天列整列高亮与跨天切换真帧。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: 今天列整列高亮与跨天切换真帧 (M3K2-A2 render)",
          "[schedule_grid][M3K2-A2][render]")
{
    kidpal_schedule_course_t cs[2];
    int total, in_band, x0, x1, y0, y1;

    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KSG render RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    ksg_course(&cs[0], 1, 0, "语文", 0);
    ksg_course(&cs[1], 5, 2, "舞蹈", 0);
    ksg_render_snap(cs, 2, -1, f_a); /* base：无今天列（未注入星期/未知日期） */

    /* 逐列注入：today_col=k 的高亮 diff 必须全部落在 day-k 列带（整列高亮 + 不越带） */
    for (int k = 0; k < 7; k++) {
        int t, ib;
        ksg_render_snap(cs, 2, k, f_b);
        ksg_diff_stats(f_a, f_b, 0, 0, KSG_FB_W - 1, KSG_GRID_BOTTOM, &t, &ib);
        TEST_ASSERT_TRUE_MESSAGE(t > 0,
            "KSG-A2 render RED: today_col=k produced a frame identical to the "
            "no-today baseline — 今天列整列高亮缺失 (S-F1.2 AC2)");
        ksg_day_band(k, &x0, &x1);
        ksg_diff_stats(f_a, f_b, x0, 0, x1, KSG_GRID_BOTTOM, &t, &ib);
        TEST_ASSERT_TRUE_MESSAGE(ib == t,
            "KSG-A2 render RED: today-column highlight ink leaks outside the day-k "
            "column band — 整列高亮不越出本列（S-F1.2 AC2 版式钉）");
    }

    /* 跨天切换（host 注入跨天状态断言切换）：today_col=2 vs today_col=5 的 diff
     * 必须分别落在两列带并集（本地零点跨天 → 注入星期变化 → 高亮列切换） */
    ksg_render_snap(cs, 2, 2, f_a);
    ksg_render_snap(cs, 2, 5, f_b);
    ksg_diff_stats(f_a, f_b, 0, 0, KSG_FB_W - 1, KSG_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0,
        "KSG-A2 render RED: switching the injected weekday did not move the "
        "highlight — 本地零点跨天切换无承载（S-F1.2 今天列零点切换）");
    {
        int full_total, in2, xa, xb;
        ksg_day_band(2, &x0, &x1);
        xa = x0;
        ksg_diff_stats(f_a, f_b, xa, 0, x1, KSG_GRID_BOTTOM, &total, &in_band);
        ksg_day_band(5, &x0, &x1);
        xb = x0;
        ksg_diff_stats(f_a, f_b, xb, 0, x1, KSG_GRID_BOTTOM, &full_total, &in2);
        TEST_ASSERT_TRUE_MESSAGE(in_band + in2 == full_total,
            "KSG-A2 render RED: weekday-switch highlight diff falls outside the two "
            "day column bands — 高亮列切换失真");
    }

    /* 越域 today_col（7 / -3）→ 无任何列高亮（不画假「今天」） */
    ksg_render_snap(cs, 2, -1, f_a);
    ksg_render_snap(cs, 2, 7, f_b);
    ksg_diff_stats(f_a, f_b, 0, 0, KSG_FB_W - 1, KSG_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        "KSG-A2 render RED: out-of-range today_col=7 must highlight nothing "
        "（不画假今天, 兜底纪律同 home_p1 日期未知）");
    ksg_render_snap(cs, 2, -3, f_b);
    ksg_diff_stats(f_a, f_b, 0, 0, KSG_FB_W - 1, KSG_FB_H - 1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total == 0,
        "KSG-A2 render RED: negative today_col must highlight nothing");
    (void)y0; (void)y1;
}

/* ------------------------------------------------------------------------ *
 * M3K2-A3 render — ※ 临时标记 / 星标呼应真帧（flags 位渲染与固定课区分）。
 * 注：※(U+203B)/★(U+2605) 在 GB2312 全量 font_body.bin 有字形（真机 K3 目测终审）;
 * host 子集字体若缺字形, builder 须让标记在 host 帧可见（兜底画法纪律同 home_p1
 * 描边兜底）——「标记可见」本身即验收面（孩子能一眼区分临时课）。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule grid: ※/星标标记真帧 (M3K2-A3 render)",
          "[schedule_grid][M3K2-A3][render]")
{
    kidpal_schedule_course_t base_f, f1, f2, f3;
    int total, in_band, x0, x1, y0, y1;

    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KSG render RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    ksg_day_band(3, &x0, &x1);
    ksg_slot_band(2, &y0, &y1);

    ksg_render_snap(ksg_course(&base_f, 3, 2, "口语", 0), 1, -1, f_a); /* 无标记基准 */

    /* flags bit0 (※ 临时)：diff 必须非零且全落本格带 */
    ksg_render_snap(ksg_course(&f1, 3, 2, "口语", KSG_FLAG_TEMP), 1, -1, f_b);
    ksg_diff_stats(f_a, f_b, x0, y0, x1, y1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0 && in_band == total,
        "KSG-A3 render RED: flags bit0 (※ 临时安排) produced no visible in-cell ink "
        "diff — ※ 标记缺失或不可见/越格（S-F1.2 AC3: 临时安排与固定课区分）");

    /* flags bit1 (星标)：可见、落本格带、且与 ※ 互相可区分 */
    ksg_render_snap(ksg_course(&f2, 3, 2, "口语", KSG_FLAG_STAR), 1, -1, f_b);
    ksg_diff_stats(f_a, f_b, x0, y0, x1, y1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0 && in_band == total,
        "KSG-A3 render RED: flags bit1 (星标呼应) produced no visible in-cell ink diff "
        "— 星标缺失或不可见/越格（S-F1.2 AC4: 与首页「接下来」当前项呼应）");
    {
        static int32_t f_tmp[KSG_FB_W * KSG_FB_H];
        int t, ib;
        ksg_render_snap(&f1, 1, -1, f_tmp);
        ksg_diff_stats(f_tmp, f_b, x0, y0, x1, y1, &t, &ib);
        TEST_ASSERT_TRUE_MESSAGE(t > 0,
            "KSG-A3 render RED: ※ and ★ markers are pixel-identical — 两标记必须可区分 "
            "（临时安排 ≠ 当前接下来星标, 语义混同即失真）");
    }

    /* flags=3（※+星标叠加）：在 bit0 帧之上仍有可见增量（星标不被 ※ 吞） */
    ksg_render_snap(ksg_course(&f3, 3, 2, "口语", KSG_FLAG_TEMP | KSG_FLAG_STAR), 1, -1, f_b);
    ksg_diff_stats(f_a, f_b, x0, y0, x1, y1, &total, &in_band);
    TEST_ASSERT_TRUE_MESSAGE(total > 0 && in_band == total,
        "KSG-A3 render RED: combined flags=3 frame lost both markers — 位掩码叠加渲染缺失");

    /* 无 flags 的课不带任何标记（基准帧即无标记帧, 已由 base 帧承载） */
    {
        static int32_t f_base2[KSG_FB_W * KSG_FB_H];
        int t, ib;
        ksg_render_snap(&base_f, 1, -1, f_base2);
        ksg_diff_stats(f_a, f_base2, 0, 0, KSG_FB_W - 1, KSG_FB_H - 1, &t, &ib);
        TEST_ASSERT_TRUE_MESSAGE(t == 0,
            "KSG-A3 render RED: flags=0 renders are not deterministic — 无标记课不得"
            "随机带标记");
    }
}

#endif /* KSG_HAVE_LVGL */

/* --------------------------------------------------------------------------- *
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-21  initial freeze (acceptance-author, TaskPackage 260921-m3-k2-p2-week-grid-page).
 * --------------------------------------------------------------------------- */
