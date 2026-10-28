/*
 * Frozen acceptance tests — TaskPackage 261028-p3-record-session-mirror-desync-fix
 * (subsystem: firmware/main/display/kidpal_render_node.cc（镜像单点派生 + show_home
 *  守卫链）+ firmware/main/application.cc（Idle 收尾沿同步 + HandleStartListeningEvent
 *  早退清位）+ firmware/main/mcp_server.cc（收尾同步桥）; rigor=full; riskTier=high;
 *  sourceType=regression; P3 记录会话双滞后修复——故事列表页卡死、长按 KEY 回不了首页)。
 *
 * Story 语义权威 = reviewed plan（delivery/taskpackages/261028-p3-record-session-
 * mirror-desync-fix/plan.md，architect /triage-change 2026-10-27 比例化 review +
 * human 触点①确认 2026-10-28：档位 ②Full+high；修复范围 M1+M2+M3 全修；被拒会话
 * 语义 = 静默放弃、再按一次）。human 立项原话（2026-10-27）：「查一下现在在 story
 * list 页面卡住了，长按key 也不回首页」。
 *
 * judge 判注（诚实申报，plan 文首逐字）：修法落点 application.cc /
 * kidpal_render_node.cc / mcp_server.cc 三个设备专有 TU **均不进 host 构建**——host
 * 验收断言只能覆盖源码结构锚 + 一条纯逻辑 canary；运行时正确性必须由真机轮
 * RSL2-A5 T1-T5 承载。哪些断言是结构锚、哪些可执行、哪些只能真机目检，按下表
 * 逐条写清，不把结构锚写成行为断言：
 *
 *   结构锚（源锚代理，TEST 1/2/3/5）——RSL2-A1/A2/A3 的 host 侧全部承载形态；
 *   可执行断言（TEST 4 canary）——RSL2-A4 冻结语义复跑（weak 哨兵 + host runner
 *   内 display/story_page.c strong symbol 覆盖，RSR-A3 第二站点）；
 *   真机目检（RSL2-A5 T1-T5，live-manual，无自动断言、不计入 runner 通过面）——
 *   三处修法的全部运行时半。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；三 TU 均为既有设备侧文件
 * additive 改造，零新文件、零新公共头——plan changeManifest 逐字）:
 *
 *   [RSL2-C1] M1（I1 镜像恒等于权威视图页态——单点派生; home = display/
 *        kidpal_render_node.cc 的 kidpal_render_node_cache_story() 内）:
 *          cache_story 在 `v != NULL` 分支内、memcpy(&s_story_view, ...) 之后写
 *          镜像: `s_story_session_page = v->page;`（或等价 setter 调用
 *          kidpal_render_node_story_session_page(v->page)——plan 新增断言种子 1
 *          「或等价 setter 调用」逐字）。
 *          负锚（防作弊/防回归）: kidpal_render_node.cc **可执行源**（注释剥离
 *          后）内 s_story_session_page 的赋值形态出现点恒 2 处——setter 定义体
 *          （:161 桥 kidpal_render_node_story_session_page）+ cache_story 体；
 *          不得散落到其它任何位置（plan「死同类」结论逐字：镜像写入边必须与
 *          真实状态边同源，不得按「给每条收尾路径补一次复位」修）。
 *          既有两处显式桥投递（mcp_server.cc VAD enter 路径 + KidpalStorySessionEndApply）
 *          保留为幂等冗余（plan M1 依据段「保留原桥与两处显式调用」）——本文件
 *          以 mcp_server.cc 全文件调用计数 >= 2 的在守锚承载，不断言其删除。
 *
 *   [RSL2-C2] M2（I1 收尾沿确定性同步; home = mcp_server.cc
 *        KidpalStorySessionEndApply 旁新增一符 + application.cc Idle 分支调用点
 *        + extern 声明——声明位置不钉，机制细节留 builder）:
 *          void KidpalStorySessionEndSync(void);
 *            -- 函数体 = kidpal_render_node_story_session_page(g_story_view.page)
 *               （镜像 := 权威视图页态）。函数体内**禁**调
 *               kidpal_render_node_cache_story——它会把 s_story_data_received
 *               置 1，破坏 AC6 冷启动空态（plan M2 硬约束逐字）。
 *          application.cc Idle 分支调用点次序（切片序锚，plan M2 落位「:1242-1248
 *          块之后、kidpal_keybar_scene_reset/voice_end 之前」+ 261025-openclaw-
 *          dreaming-disable 留根「M2 插行不得插在 kidpal_keybar_scene_reset 与其
 *          判据之间」——本序锚把两约束合并钉死，chrome_ghost CG-A1 scene 复位锚
 *          同区在守）:
 *            kidpal_story_session_end( -> KidpalStorySessionEndApply( ->
 *            KidpalStorySessionEndSync( -> kidpal_keybar_scene_reset( ->
 *            kidpal_render_node_voice_end(
 *
 *   [RSL2-C3] M3（I2 ctx 的生命周期 = 聆听会话真的起来了; home = application.cc
 *        HandleStartListeningEvent() 三处「聆听没真起来」早退边缘各加
 *        kidpal_story_session_ctx_set(0);）:
 *          - kDeviceStateWifiConfiguring 分支（return 前）;
 *          - !protocol_ 分支（"Protocol not initialized", return 前）;
 *          - auto_connect_in_flight_ 分支（"AutoConnect in-flight", return 前）。
 *          覆盖两个置位入口（mcp_server.cc KSPV2_ACT_REC_SESSION 消费位 :606 与
 *          control/start_recording 触发词入口 :1450——plan 泄漏 B 双入口逐字）。
 *          Activating 分支不加（落到 Idle 已有清位; plan M3 逐字）——本文件**不钉**
 *          Activating 负向断言，留 builder 裁量，防过冻结（R1 既有沿零改动）。
 *
 *   [RSL2-C4] 冻结面 canary + 回归网（must-stay-green）:
 *          - kidpal_story_session_end 冻结语义不变（[R2] 权威 = record_session_
 *            repair.acceptance.c 文件头）: LIST/READER/DELETE/STAMP/ORGANIZING
 *            返 0 且 out_page 零触碰、仅 RECORDING 返 1 且 *out = KSTORY_PAGE_LIST、
 *            out_page == NULL 返 -1。本文件为 RSR-A3 的**第二站点复跑**（plan
 *            新增断言种子 4）；权威站点 = record_session_repair.acceptance.c
 *            （继续在跑，本文件不重复其登记、不改动该件）。
 *          - 回归网八族登记在守（host CMakeLists HOST_ACCEPTANCE_SRCS）:
 *            record_session_repair.acceptance.c / record_session_repair.test.c /
 *            record_dialogue_guide.acceptance.c / record_session_live.acceptance.c /
 *            story_page_v2.acceptance.c / chrome_ghost.acceptance.c /
 *            record_dialogue_v7_fix.test.c / key_layer_dispatch.acceptance.c
 *            + 本件登记（builder FIRST ACTION，注册序在 host_infra_smoke 之前）
 *            + display/story_page.c 实现源在册（canary strong symbol 通道, EQ-4）。
 * ===========================================================================
 * 契约 / 决策条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 齿」节）:
 *
 * | 条目 | 承载断言 | 性质档 |
 * |---|---|---|
 * | RSL2-A1（M1 镜像跟随权威，含负锚） | TEST 1（cache_story 体镜像写入正向锚 + 赋值形态全文件恰 2 处且落点 = setter 体与 cache_story 体的窗内含负锚） | 源锚代理（机制级锚点；**不等于运行时观察**——运行时半归 RSL2-A5） |
 * | RSL2-A2（M2 Idle 收尾沿次序） | TEST 2（Idle 切片五符号序锚 + mcp_server.cc Sync 定义体恰含 story_session_page 调用、cache_story 零命中负锚） | 源锚代理 |
 * | RSL2-A3（M3 三早退边缘 ctx 清位） | TEST 3（三分支切片各含 ctx_set(0) + 函数内计数 >= 3 + 既有消费锚在守） | 源锚代理 |
 * | RSL2-A4（冻结语义 canary + 回归网八族 must-stay-green） | TEST 4（[R2] 决策表全列复跑，weak 哨兵 / strong symbol 双态）+ TEST 5（登记守）+ host 全量实跑对照 237T3F（运行面承载） | 行为级（canary）+ 源锚代理（登记守）+ 实跑记录 |
 * | RSL2-A5（真机轮 T1-T5） | **live-manual，无自动断言**（kind=structural，DLG-A6 / CG-A5 先例口径，不计入 runner 通过面） | 真机观察（human 持板） |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1（RSL2-A1 运行时半）: TEST 1 只证明源码具备「镜像 = 权威视图页态」的
 *         写入形状；设备上镜像真恒等、show_home 守卫真放行（`show_home suppressed`
 *         不再出现、PAGE_RENDER 的 sview/ssess 不再分叉）-> 真机轮 RSL2-A5
 *         T1 / T2（LIST 态 ssess=0）/ T5（连续两轮不假 live）承载。
 *   GAP-2（RSL2-A2 运行时半）: 收尾沿镜像真同步、voice_end [R5] 页态解析不再把
 *         丢弃后的 LIST 复活成 RECORDING -> 真机轮 T1 / T2 / T3 承载。
 *   GAP-3（RSL2-A3 运行时半）: 被拒 REC_SESSION 后 ctx 真清、**KEY 长按**（不是
 *         BOOT 长按——plan 按键口径修正：BOOT 长按在基层是空分支）真回首页 ->
 *         真机轮 T1 承载；普通语音轮零变化（M3 不误伤）-> T4 承载。
 *   GAP-4（canary 独立通道形态）: TEST 4 独立通道红 = 哨兵态（story_page.c 不入
 *         独立链接——其 lvgl/esp_log include 仅 host_app 环境可编）；冻结语义
 *         本体已在世，运行面判据 = host runner strong symbol 通道（RSR 独立通道
 *         常态同款）。该红非本包 story 缺失，红基线记账时单列，不计入 story 红。
 *   GAP-5（结构锚总括）: 全部结构断言不证明设备运行时表现（EQ-1）。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1「符号在场 ≙ 接线落地」: 源锚代理本质——TEST 1-3 断言的是源码形状，
 *         不是运行时行为（GAP-1/2/3/5）。
 *   EQ-2「cache_story 内镜像写 ≙ 镜像恒等权威」: 依据 = plan 对 g_story_view.page
 *         每次变更终经 cache_story 的逐点核对（ScheduleStoryRender / VAD enter /
 *         SessionEndApply / 盖章 cache-only / story_text 五点，plan M1 依据段）；
 *         等价假设的真实性由真机轮 T1/T2/T5 承载。
 *   EQ-3「canary 枚举 == story_page.h 真值」: TEST 4 直接 include ../display/
 *         story_page.h 用枚举名（不经数值镜像）；KSTORY_PAGE_* 数值等值由
 *         story_page_v2 冻结验收承载。
 *   EQ-4「哨兵被强符号覆盖 ≙ canary 判据生效」: host runner 中 display/story_page.c
 *         必须在册（TEST 5 登记守校验）；独立通道哨兵红 = 通道形态（GAP-4）。
 *
 * coverage 标记（coverage marker）随 TEST_CASE 名称与首行注释逐条给出。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder FIRST ACTION: 向 firmware/main/test/host_app/main/CMakeLists.txt
 *      的 HOST_ACCEPTANCE_SRCS 追加 "../../record_session_lifecycle.acceptance.c"
 *      （置于 host_infra_smoke.acceptance.c 之前；登记行不在 acceptance-author
 *      写权内——261022-diag 先例口径；本包零新增实现源登记，story_page.c 既有在册）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      firmware/main/test/host_app/build/host_infra_test.elf
 *   本文件亦可独立冒烟（冻结相位实编 + 设计态红取证，diag_log 通道 B 同款）:
 *      cd firmware/main/test/acceptance/red-baseline
 *      gcc -std=c11 -Wall -Wextra -I. -o /tmp/rsl2_red_runner \
 *          ../../record_session_lifecycle.acceptance.c runner.c
 *      cd <repo root>; /tmp/rsl2_red_runner   # exit code = 失败用例数
 *   结构扫描类断言支持 KIDPAL_REPO_ROOT 显式注入仓根（缺省走 walk-up 探测，仓根 /
 *   host_app / red-baseline 三处均可跑；acceptance-evidence「root 注入」纪律）。
 *
 * RED baseline（冻结相位形状；冻结落盘时实跑，逐条红名见交付报告与下方实跑记录）:
 *   设计态（净工作树: M1/M2/M3 三处改造未落、本件未登记——登记归 builder FIRST
 *   ACTION）。通道 B 独立冒烟预期 5 个 TEST_CASE 全红:
 *     TEST 1 -> RSL2-A1 M1 镜像写入缺席（cache_story 体无镜像写；赋值形态计数
 *               1 != 2——现值仅 setter 一处）
 *     TEST 2 -> RSL2-A2 M2 Sync 缺席（application.cc Idle 切片序锚断在
 *               KidpalStorySessionEndSync + mcp_server.cc 无 Sync 定义）
 *     TEST 3 -> RSL2-A3 M3 清位缺席（WifiConfiguring 分支切片无 ctx_set(0)）
 *     TEST 4 -> RSL2-A4 canary 独立通道哨兵红（通道形态，GAP-4——非 story 缺失；
 *               host runner strong symbol 通道 must-stay-green）
 *     TEST 5 -> RSL2-A4 登记守（本件未登记进 HOST_ACCEPTANCE_SRCS）
 *   通道 A host 全量 runner: 本件登记前不进 runner（预期，运行面零扰动）；已知
 *   基线 237 Tests / 3 Failures（3 条全为既有 p1_auxbar AB-A1/A2/A4 先存红，锚
 *   aecded8b；launch-card 差异判停 2026-10-28 实测 aecded8b..HEAD firmware/ 零 diff）。
 *   冻结落盘实跑（2026-10-28，双通道 + 双向可满足性）:
 *   〔A〕host 全量 runner（工作树、本件尚未登记进 runner——261018/261022-diag 先例
 *        同口径）: 差异判停实测 `git diff --quiet aecded8b HEAD -- firmware/` exit 0
 *        -> 引用既有锚 `237 Tests 3 Failures`（3 条全为 p1_auxbar AB-A1/A2/A4 先存红，
 *        与本包正交；本件未编入 runner，运行面零扰动）。
 *   〔B〕独立冒烟通道（red-baseline/rsl2_red_wrapper.sh, COMPILE_EXIT=0，-Wall
 *        -Wextra 零 warning）: `5 Tests 5 Failures`（RUN_EXIT=5），红名 verbatim：
 *          TEST 1 -> RSL2-A1「cache_story 体内无镜像写入（s_story_session_page =
 *                    v->page 或等价 setter 调用）」
 *          TEST 2 -> RSL2-A2「application.cc Idle 分支无 KidpalStorySessionEndSync(
 *                    调用——收尾沿无确定性镜像同步」
 *          TEST 3 -> RSL2-A3「WifiConfiguring 早退分支未清记录语境标志」
 *          TEST 4 -> RSL2-A4「RED(通道形态): 独立通道未链接 display/story_page.c，
 *                    canary 落在 weak 哨兵 -99」（GAP-4，非 story 缺失，红基线
 *                    记账单列；host runner strong symbol 通道 must-stay-green）
 *          TEST 5 -> RSL2-A4「host_app/main/CMakeLists.txt 未登记 record_session_
 *                    lifecycle.acceptance.c——builder FIRST ACTION」
 *        前四红逐条对应当前 story 缺失的真实行为（M1/M2/M3 三处改造未落 + 登记缺位）。
 *   双向可满足性（red-baseline/rsl2_sat_harness.py，/mnt/c/tmp/rsl2_work 合成树
 *   KIDPAL_REPO_ROOT 注入 + 一次性参考实现，均不写入工作树）: 绿树（M1+M2+M3 一次性
 *   参考实现 + 登记）`5 Tests 0 Failures`；六处定向变异各自只打红对应 TEST_CASE
 *   （每处重建净树 + 单变异 + 重编重跑）——摘 M1 镜像写 -> TEST 1 红 / 镜像赋值散落
 *   第三处（rn_record_session_live_locked 体内）-> TEST 1 红（恰 2 处负锚咬合）/
 *   Sync 调用移到 voice_end 之后 -> TEST 2 红（序锚）/ Sync 体改调 cache_story ->
 *   TEST 2 红（AC6 负锚）/ 摘 auto_connect 分支清位 -> TEST 3 红 / 摘登记 -> TEST 5 红
 *   ——断言非空过。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: RSL2-A1 -- M1 镜像单点派生（结构）：cache_story 体镜像写入
 *             （s_story_session_page = v->page 或等价 setter 调用，落在
 *             memcpy(&s_story_view,...) 之后）+ 负锚（赋值形态全文件恰 2 处、
 *             逐点落位 setter 体 + cache_story 体）+ mcp_server.cc 既有双投递
 *             在守（TEST 1）
 *   coverage: RSL2-A2 -- M2 收尾沿确定性同步（结构）：application.cc Idle 切片
 *             五符号序锚（session_end -> Apply -> Sync -> keybar_scene_reset ->
 *             voice_end）+ mcp_server.cc Sync 定义体恰含
 *             kidpal_render_node_story_session_page(g_story_view.page、体内
 *             cache_story 零命中（AC6 冷启动空态负锚）（TEST 2）
 *   coverage: RSL2-A3 -- M3 三早退边缘 ctx 清位（结构）：WifiConfiguring /
 *             !protocol_ / auto_connect_in_flight_ 三分支切片各含
 *             kidpal_story_session_ctx_set(0 且函数内计数 >= 3 + 既有
 *             ctx_active 消费与 SetListeningMode 锚在守（TEST 3）
 *   coverage: RSL2-A4 -- 冻结语义 canary（行为）：kidpal_story_session_end
 *             LIST/READER/DELETE/STAMP/ORGANIZING -> 0 且 out 零触碰、RECORDING
 *             -> 1 且 out = LIST、NULL out -> -1、非录音态幂等（TEST 4）+
 *             回归网登记守（本件 + 八冻结件 + story_page.c 实现源，TEST 5）+
 *             host 全量实跑对照 237T3F（先存红 = p1_auxbar AB-A1/A2/A4，锚
 *             aecded8b），零新增意外红
 *   coverage: RSL2-A5 -- **live-manual 真机轮观测项，不落自动断言、不计入 runner
 *             通过面**（TaskPackage 标 kind=structural；DLG-A6 / CG-A5 先例同款
 *             口径）。human 持板执行 plan 真机轮 T1-T5（T1 复现路径：VAD 进录音
 *             -> BOOT 丢弃 -> auto-connect 期按 KEY 被拒 -> KEY 长按必须回首页，
 *             日志无 show_home suppressed、sview/ssess 不分叉；T2 保存路径 LIST
 *             态 ssess=0；T3 整理收尾后长按回首页可用；T4 负回归：普通语音轮不
 *             进录音态、录音期间服务端 home 推送仍被抑制；T5 连续两轮会话不假
 *             live 不锁死）。按键口径 = KEY 长按（BOOT 长按在基层是空分支，
 *             plan 按键口径修正）。本 Pod 到真机门前停等
 *             （status=awaiting-human:real-device），不自行烧录。
 * --------------------------------------------------------------------------- */

#include <unity.h>

#include "../display/story_page.h" /* KSTORY_PAGE_* 枚举（EQ-3; 纯声明头 host 可编） */

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* -------------------------------------------------------------------------- *
 * 未实现哨兵（TEST 4 canary 用；diag_log / chrome_ghost 同款纪律）：强定义 home =
 * display/story_page.c（host runner 既有在册源），host runner 内被强符号覆盖；
 * 独立通道不链接 story_page.c -> 哨兵态红 = 通道形态（GAP-4），不是编译错。
 * ------------------------------------------------------------------------ */
#define RSL2_UNIMPLEMENTED (-99)

__attribute__((weak)) int kidpal_story_session_end(int page_state, int *out_page)
{
    (void)page_state; (void)out_page;
    return RSL2_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * 工具：失败消息 / 仓根定位 / 文件读取 / 注释剥离 / 窗内搜索（自包含复制自
 * diag_log / chrome_ghost 的助手族，命名前缀 rsl2_；全 static，不与先例符号冲突）。
 * ------------------------------------------------------------------------ */

#define RSL2_MAX_PATH 1024
#define RSL2_MAX_READ (8u * 1024u * 1024u)

static char rsl2_fm_buf[1024];
static const char *rsl2_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *rsl2_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(rsl2_fm_buf, sizeof(rsl2_fm_buf), fmt, ap);
    va_end(ap);
    return rsl2_fm_buf;
}

static int rsl2_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char rsl2_root[RSL2_MAX_PATH];
static int rsl2_root_ready;

/* 仓根定位（acceptance-evidence「root 注入」纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由相对候选点探测——不依赖某个调用 cwd 的绝对路径假设。 */
static int rsl2_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[RSL2_MAX_PATH];
    const char *env_root;
    size_t i;

    if (rsl2_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (rsl2_is_regular(probe)) {
            snprintf(rsl2_root, sizeof(rsl2_root), "%s", env_root);
            rsl2_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (rsl2_is_regular(probe)) {
            snprintf(rsl2_root, sizeof(rsl2_root), "%s", cands[i]);
            rsl2_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int rsl2_rel(char *out, size_t cap, const char *rel)
{
    if (out == NULL || rel == NULL) {
        return 0;
    }
    if (!rsl2_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", rsl2_root, rel);
    return 1;
}

static char *rsl2_read(const char *rel, size_t *out_len)
{
    char p[RSL2_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!rsl2_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > RSL2_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

/* 注释剥离（260917-m2-k3 sealing pass / diag_log / chrome_ghost 同款）：正向断言
 * 只被**可执行源**满足——注释字节抹成空格；字符串 / 字符字面量体保留（不把字面量
 * 里的 // 误判为注释）。保持缓冲长度，调用方负责 free。 */
static char *rsl2_strip_c_comments(const char *src)
{
    size_t n, i = 0;
    char *out;

    assert(src != NULL); /* logic guard: NULL source is a test bug, not a red */
    n = strlen(src);
    out = (char *)malloc(n + 1);
    assert(out != NULL); /* logic guard: host OOM must fail loudly */
    memcpy(out, src, n + 1);

    while (i < n) {
        char c = out[i];
        if (c == '"' || c == '\'') {
            char q = c;
            i++;
            while (i < n) {
                if (out[i] == '\\' && i + 1 < n) { i += 2; continue; }
                if (out[i] == q) { i++; break; }
                i++;
            }
            continue;
        }
        if (c == '/' && i + 1 < n && out[i + 1] == '/') {
            while (i < n && out[i] != '\n') { out[i] = ' '; i++; }
            continue;
        }
        if (c == '/' && i + 1 < n && out[i + 1] == '*') {
            out[i] = ' '; out[i + 1] = ' '; i += 2;
            while (i < n) {
                if (out[i] == '*' && i + 1 < n && out[i + 1] == '/') {
                    out[i] = ' '; out[i + 1] = ' '; i += 2; break;
                }
                if (out[i] != '\n') { out[i] = ' '; }
                i++;
            }
            continue;
        }
        i++;
    }
    return out;
}

static char *rsl2_strip_file(const char *rel)
{
    char *buf = rsl2_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = rsl2_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 有界子串搜索（窗内 needle 在场判定）。未命中 -> NULL。 */
static const char *rsl2_find_in(const char *hay, const char *end, const char *needle)
{
    size_t nl;

    if (hay == NULL || needle == NULL || end == NULL || hay >= end) {
        return NULL;
    }
    nl = strlen(needle);
    if (nl == 0) {
        return hay;
    }
    for (; hay + nl <= end; hay++) {
        if (hay[0] == needle[0] && memcmp(hay, needle, nl) == 0) {
            return hay;
        }
    }
    return NULL;
}

/* 裸源子串出现计数（负向断言专用；调用方传入已剥注释窗时天然只计可执行源）。 */
static int rsl2_count_sub(const char *hay, const char *needle)
{
    int n = 0;
    const char *p = hay;
    size_t nl;

    if (hay == NULL || needle == NULL) {
        return 0;
    }
    nl = strlen(needle);
    if (nl == 0) {
        return 0;
    }
    while ((p = strstr(p, needle)) != NULL) {
        n++;
        p++;
    }
    return n;
}

/* 有界子串计数（[hay, hay+len) 内 needle 出现次数；窗切片负向断言专用）。 */
static int rsl2_count_in(const char *hay, size_t len, const char *needle)
{
    int n = 0;
    size_t nl;
    const char *end;

    if (hay == NULL || needle == NULL || len == 0) {
        return 0;
    }
    nl = strlen(needle);
    if (nl == 0 || nl > len) {
        return 0;
    }
    end = hay + len;
    for (; hay + nl <= end; hay++) {
        if (hay[0] == needle[0] && memcmp(hay, needle, nl) == 0) {
            n++;
        }
    }
    return n;
}

/* 函数窗终点：起点 + win 字节，钳制到缓冲末尾。 */
static const char *rsl2_win_end(const char *start, size_t win)
{
    const char *end = start + win;
    size_t total = strlen(start);

    if ((size_t)(end - start) > total) {
        end = start + total;
    }
    return end;
}

/* 全序判定：[region, end) 内 a、b 依次在场（a 先于 b）。 */
static int rsl2_order_in(const char *region, const char *end, const char *a,
                         const char *b)
{
    const char *pa = rsl2_find_in(region, end, a);
    const char *pb;

    if (pa == NULL) {
        return 0;
    }
    pb = rsl2_find_in(pa + strlen(a), end, b);
    return pb != NULL;
}

/* 变量赋值形态出现点定位（负锚专用）：needle 后首个非空白字符 == '=' 且非 "=="，
 * 且**前一个词不是 "int"**（排除文件级声明 `static int s_story_session_page = 0;`
 * 的初始化——只计赋值语句）。位置写入 out 数组，返回个数。 */
static int rsl2_assign_sites(const char *hay, const char *varname,
                             const char *out[], int max_out)
{
    size_t vl;
    int n = 0;
    const char *p = hay;

    if (hay == NULL || varname == NULL) {
        return 0;
    }
    vl = strlen(varname);
    while ((p = strstr(p, varname)) != NULL) {
        const char *q = p + vl;
        const char *w;
        size_t wl;
        int is_decl;

        if (n >= max_out) {
            break;
        }
        while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') {
            q++;
        }
        if (q[0] != '=' || q[1] == '=') {
            p++;
            continue;
        }
        /* 回退取前一个标识符词，判是否声明（"static int x = ..."） */
        w = p;
        while (w > hay && (w[-1] == ' ' || w[-1] == '\t' || w[-1] == '\r' ||
                           w[-1] == '\n' || w[-1] == '*' || w[-1] == '\t')) {
            w--;
        }
        wl = 0;
        while (w > hay + wl &&
               ((w[-1 - wl] >= 'a' && w[-1 - wl] <= 'z') ||
                (w[-1 - wl] >= 'A' && w[-1 - wl] <= 'Z') ||
                (w[-1 - wl] >= '0' && w[-1 - wl] <= '9') ||
                w[-1 - wl] == '_')) {
            wl++;
        }
        is_decl = (wl == 3 && w[-3] == 'i' && w[-2] == 'n' && w[-1] == 't');
        if (!is_decl) {
            out[n++] = p;
        }
        p += vl;
    }
    return n;
}

#define RSL2_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(rsl2_locate_root(), \
    "RSL2 RED(infra): repo root not located from cwd — run from the repo root, " \
    "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, " \
    "or pass KIDPAL_REPO_ROOT (see acceptance-evidence root injection rule)")

/* 结构扫描面（源锚；相对仓根）。 */
#define RSL2_RENDER_CC  "firmware/main/display/kidpal_render_node.cc"
#define RSL2_APP_CC     "firmware/main/application.cc"
#define RSL2_MCP_CC     "firmware/main/mcp_server.cc"
#define RSL2_HOST_CMAKE "firmware/main/test/host_app/main/CMakeLists.txt"

/* 函数/区域窗（既有函数体实测余量; 窗漂移时 RED(infra) 提示复核锚） */
#define RSL2_FN_WINDOW  1200u  /* cache_story / setter / Sync 等小函数体 */
#define RSL2_SYNC_WIN   400u   /* KidpalStorySessionEndSync 定义体（恰一调） */

/* cache_story 定义锚（kidpal_render_node.cc :1181 既有形态）。 */
#define RSL2_CACHE_FN "void kidpal_render_node_cache_story(const kidpal_story_view_t *v,"
/* setter 定义锚（kidpal_render_node.cc :161 既有形态）。 */
#define RSL2_SETTER_FN "void kidpal_render_node_story_session_page(int page)"
/* Idle 分支切片锚（application.cc HandleStateChangedEvent switch; :1231/:1280）。 */
#define RSL2_IDLE_START "case kDeviceStateIdle:"
#define RSL2_IDLE_END   "case kDeviceStateConnecting:"
/* HandleStartListeningEvent 函数切片锚（:1039/:1095）。 */
#define RSL3_HSL_START "void Application::HandleStartListeningEvent("
#define RSL3_HSL_END   "void Application::HandleStopListeningEvent("
/* M2 新符。 */
#define RSL2_SYNC_SYM "KidpalStorySessionEndSync("
/* M3 清位调用（ctx_set(0 —— 既有 Idle 沿同款形态）。 */
#define RSL2_CTX_CLEAR "kidpal_story_session_ctx_set(0"

/* ========================================================================== *
 * TEST 1 — RSL2-A1 M1 镜像单点派生（结构源锚 + 负锚）。
 * cache_story 体镜像写入（s_story_session_page = v->page 或等价 setter 调用）+
 * 负锚：kidpal_render_node.cc 可执行源内赋值形态恒 2 处（setter 体 + cache_story
 * 体），不得散落其它位置 + mcp_server.cc 既有双投递在守 + 扫描器自检。
 * RED today（设计态）: cache_story 体无镜像写（赋值形态仅 setter 1 处 != 2）。
 * ========================================================================== */
/* coverage: RSL2-A1 -- M1 镜像单点派生（结构）：cache_story 体镜像写入 + 赋值
 *           形态恰 2 处逐点落位负锚 + mcp_server.cc 既有双投递在守 */
TEST_CASE("record session lifecycle: mirror derived at cache_story single source (RSL2-A1)",
          "[record_session_lifecycle][RSL2-A1][structural]")
{
    char *code;
    char *raw;
    const char *p_setter;
    const char *p_setter_end;
    const char *p_cache;
    const char *p_cache_end;
    const char *sites[8];
    int n_sites;

    RSL2_REQUIRE_ROOT();

    /* ---- 扫描器自检（diag_log TEST 4 同款：自检不成立 = 全部结构断言失去咬合力） */
    raw = rsl2_read(RSL2_RENDER_CC, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(raw, "RSL2-A1 RED(infra): kidpal_render_node.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "裸读节点缓存") != NULL,
        "RSL2-A1 RED(infra): kidpal_render_node.cc 已知注释锚缺席——注释剥离自检"
        "无法成立（文件被重写？）。");
    free(raw);
    code = rsl2_strip_file(RSL2_RENDER_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "RSL2-A1 RED(infra): kidpal_render_node.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "裸读节点缓存") == NULL,
        "RSL2-A1 RED(infra): 注释剥离失效（已知注释文本仍留在可执行源里）——本文件"
        "全部结构断言会因注释文本假绿，须复核扫描器");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "rn_record_session_live_locked") != NULL,
        "RSL2-A1 RED(infra): kidpal_render_node.cc 无 rn_record_session_live_locked"
        "（守卫判据本体被改名，既有锚漂移——须复核不得改判为绿）");

    /* ---- 既有定义锚在守（不得改名/移位——后续窗切取的前提） ----------------- */
    p_setter = strstr(code, RSL2_SETTER_FN);
    TEST_ASSERT_NOT_NULL_MESSAGE(p_setter,
        "RSL2-A1 RED(infra): kidpal_render_node.cc 无 kidpal_render_node_story_"
        "session_page 定义锚——镜像 setter 桥被改名（[R5] seam 既有形态，须复核"
        "不得改判为绿）");
    p_setter_end = rsl2_win_end(p_setter, RSL2_FN_WINDOW);
    p_cache = strstr(code, RSL2_CACHE_FN);
    TEST_ASSERT_NOT_NULL_MESSAGE(p_cache,
        "RSL2-A1 RED(infra): kidpal_render_node.cc 无 kidpal_render_node_cache_story "
        "定义锚——缓存装载单点被改名（须复核不得改判为绿）");
    /* cache_story 窗界 = 定义锚至下一函数（refresh_story），不靠裸窗长——防窗内
     * 误吞后续函数体的赋值形态。 */
    p_cache_end = strstr(p_cache, "void kidpal_render_node_refresh_story");
    if (p_cache_end == NULL) {
        p_cache_end = rsl2_win_end(p_cache, RSL2_FN_WINDOW);
    }

    /* ---- 正向锚（RED today）: cache_story 体内镜像写入 ---------------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl2_find_in(p_cache, p_cache_end, "s_story_session_page = v->page") != NULL ||
        rsl2_find_in(p_cache, p_cache_end,
                     "kidpal_render_node_story_session_page(v->page") != NULL,
        "RSL2-A1 RED: kidpal_render_node_cache_story 体内无镜像写入（"
        "s_story_session_page = v->page 或等价 setter 调用）——镜像与权威视图页态"
        "双载体写入边不同源（泄漏 A 根因未治：story_page_input 先改页再返码的"
        "收尾路径上镜像恒停 RECORDING，长按 KEY 永远被 show_home 守卫抑制）");

    /* ---- 负锚（防作弊/防回归）: 赋值形态全文件恰 2 处、逐点落位 -------------- */
    n_sites = rsl2_assign_sites(code, "s_story_session_page", sites, 8);
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, n_sites,
        rsl2_fm("RSL2-A1 RED: kidpal_render_node.cc 可执行源内 s_story_session_page "
                "赋值形态出现 %d 处（冻结基线恒 2: setter 定义体 + cache_story 体）"
                "——镜像写入散落 = 写入边与真实状态边再度不同源（plan「死同类」"
                "结论: 必须按镜像 = 权威视图页态的不变量修，不得按给每条收尾路径"
                "补一次复位修）", n_sites));
    TEST_ASSERT_TRUE_MESSAGE(sites[0] >= p_setter && sites[0] < p_setter_end,
        "RSL2-A1 RED: 第 1 处镜像赋值不在 setter 定义体内（kidpal_render_node_"
        "story_session_page 桥）——镜像写入落点漂移");
    TEST_ASSERT_TRUE_MESSAGE(sites[1] >= p_cache && sites[1] < p_cache_end,
        "RSL2-A1 RED: 第 2 处镜像赋值不在 kidpal_render_node_cache_story 体内——"
        "镜像写入未随缓存装载单点派生（I1 不变量未建立）");
    /* 写入须派生自权威视图（RHS 含 v->page 或经 setter 转调——正向锚已断其一，
     * 此处再断赋值点邻域确在 v != NULL 分支语境：赋值点之前同窗出现 memcpy
     * &s_story_view）。 */
    TEST_ASSERT_TRUE_MESSAGE(rsl2_find_in(p_cache, sites[1], "memcpy(&s_story_view") != NULL,
        "RSL2-A1 RED: 镜像赋值未落在 cache_story 的 v != NULL 分支语境（之前无 "
        "memcpy(&s_story_view）——镜像来源不是入参权威视图");

    free(code);

    /* ---- 既有双投递在守（mcp_server.cc; plan M1 保留幂等冗余） --------------- */
    code = rsl2_strip_file(RSL2_MCP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "RSL2-A1 RED(infra): mcp_server.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        rsl2_count_sub(code, "kidpal_render_node_story_session_page(") >= 2,
        "RSL2-A1 RED(infra): mcp_server.cc 既有镜像桥投递 < 2 处（VAD enter + "
        "KidpalStorySessionEndApply）——幂等冗余被删（plan M1 依据段保留义务，"
        "须复核不得改判为绿）");
    free(code);
}

/* ========================================================================== *
 * TEST 2 — RSL2-A2 M2 Idle 收尾沿确定性同步（结构源锚 + 序锚 + 负锚）。
 * application.cc Idle 切片五符号序锚 + mcp_server.cc Sync 定义体恰含
 * story_session_page 调用、cache_story 零命中（AC6 冷启动空态负锚）。
 * RED today（设计态）: KidpalStorySessionEndSync 全仓缺席。
 * ========================================================================== */
/* coverage: RSL2-A2 -- M2 收尾沿同步（结构）：Idle 切片五符号序锚 + mcp_server.cc
 *           Sync 定义体形状 + cache_story 零命中负锚 */
TEST_CASE("record session lifecycle: idle branch end-sync order and sync bridge shape (RSL2-A2)",
          "[record_session_lifecycle][RSL2-A2][structural]")
{
    char *app;
    char *mcp;
    const char *idle;
    const char *idle_end;
    const char *p_sync;

    RSL2_REQUIRE_ROOT();

    /* ---- application.cc Idle 分支切片五符号序锚（RED today: Sync 缺席） ------ */
    app = rsl2_strip_file(RSL2_APP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(app, "RSL2-A2 RED(infra): application.cc unreadable");
    idle = strstr(app, RSL2_IDLE_START);
    TEST_ASSERT_NOT_NULL_MESSAGE(idle,
        "RSL2-A2 RED(infra): application.cc 无 case kDeviceStateIdle: 切片锚"
        "（HandleStateChangedEvent 被重构，须复核锚不得改判为绿）");
    idle_end = strstr(idle + strlen(RSL2_IDLE_START), RSL2_IDLE_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(idle_end,
        "RSL2-A2 RED(infra): Idle 分支切片终点 case kDeviceStateConnecting: 缺席"
        "（须复核锚不得改判为绿）");

    /* 既有锚在守（收尾沿既有链不得被 M2 插行连带拆动）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl2_order_in(idle, idle_end, "kidpal_story_session_end(",
                      "KidpalStorySessionEndApply("),
        "RSL2-A2 RED(infra): Idle 切片 kidpal_story_session_end( -> "
        "KidpalStorySessionEndApply( 既有序漂移（[R2] 收尾消费链既有形态，须复核"
        "不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        rsl2_find_in(idle, idle_end, "kidpal_keybar_scene_reset(") != NULL,
        "RSL2-A2 RED(infra): Idle 切片 kidpal_keybar_scene_reset( 缺席（chrome_ghost "
        "CG-A1 既有锚被 M2 改造连带破坏，须复核不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        rsl2_find_in(idle, idle_end, "kidpal_render_node_voice_end(") != NULL,
        "RSL2-A2 RED(infra): Idle 切片 kidpal_render_node_voice_end( 缺席（前台回落"
        "既有沿被删，须复核不得改判为绿）");

    /* M2 调用点 + 次序（plan M2 落位 + openclaw 留根约束合并钉死）。 */
    p_sync = rsl2_find_in(idle, idle_end, RSL2_SYNC_SYM);
    TEST_ASSERT_NOT_NULL_MESSAGE(p_sync,
        "RSL2-A2 RED: application.cc Idle 分支无 KidpalStorySessionEndSync( 调用——"
        "收尾沿无确定性镜像同步（泄漏 A 的 Idle 沿收尾路径：输入函数先改页再返码，"
        "session_end 判据见 LIST 返 0、镜像永不落地，show_home 永久 no-op 未治）");
    TEST_ASSERT_TRUE_MESSAGE(
        rsl2_order_in(idle, idle_end, "KidpalStorySessionEndApply(", RSL2_SYNC_SYM) &&
        rsl2_order_in(idle, idle_end, RSL2_SYNC_SYM, "kidpal_keybar_scene_reset(") &&
        rsl2_order_in(idle, idle_end, "kidpal_keybar_scene_reset(",
                      "kidpal_render_node_voice_end("),
        "RSL2-A2 RED: Idle 切片收尾沿次序漂移——冻结序 = session_end -> Apply -> "
        "Sync -> keybar_scene_reset -> voice_end（plan M2「:1242-1248 块之后、"
        "kidpal_keybar_scene_reset/voice_end 之前」+ openclaw 留根「不得插在 "
        "keybar_scene_reset 与其判据之间」合并钉死；M2 插错位 = 主循环 STATE_CHANGED "
        "先于 SCHEDULE 的旧值窗复现）");
    free(app);

    /* ---- mcp_server.cc Sync 定义体形状（恰一调 + cache_story 零命中负锚） ---- */
    mcp = rsl2_strip_file(RSL2_MCP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(mcp, "RSL2-A2 RED(infra): mcp_server.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(mcp, RSL2_SYNC_SYM) != NULL,
        "RSL2-A2 RED: mcp_server.cc 无 KidpalStorySessionEndSync( 定义——收尾同步桥"
        "未落（plan changeManifest「mcp_server.cc 新增 KidpalStorySessionEndSync() "
        "（仅此一符）」未实现）");
    p_sync = strstr(mcp, "void KidpalStorySessionEndSync(");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_sync,
        "RSL2-A2 RED: mcp_server.cc 无 void KidpalStorySessionEndSync( 定义锚"
        "（签名形态漂移，须复核）");
    {
        const char *body_end = rsl2_win_end(p_sync, RSL2_SYNC_WIN);
        TEST_ASSERT_TRUE_MESSAGE(
            rsl2_find_in(p_sync, body_end,
                         "kidpal_render_node_story_session_page(g_story_view.page") != NULL,
            "RSL2-A2 RED: KidpalStorySessionEndSync 函数体未投递权威视图页态（"
            "kidpal_render_node_story_session_page(g_story_view.page 缺席）——镜像 "
            ":= 权威的同步语义缺失");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            rsl2_count_in(p_sync, (size_t)(body_end - p_sync),
                          "kidpal_render_node_cache_story("),
            "RSL2-A2 RED: KidpalStorySessionEndSync 函数体调用了 "
            "kidpal_render_node_cache_story——plan M2 硬约束违反（cache_story 会把 "
            "s_story_data_received 置 1，破坏 AC6 冷启动空态判据）");
    }
    free(mcp);
}

/* ========================================================================== *
 * TEST 3 — RSL2-A3 M3 三早退边缘 ctx 清位（结构源锚）。
 * HandleStartListeningEvent 内 WifiConfiguring / !protocol_ /
 * auto_connect_in_flight_ 三分支切片各含 kidpal_story_session_ctx_set(0 +
 * 函数内计数 >= 3 + 既有消费锚在守。
 * RED today（设计态）: 三分支切片均无 ctx_set(0（泄漏 B 根因未治）。
 * ========================================================================== */
/* coverage: RSL2-A3 -- M3 三早退边缘 ctx 清位（结构）：三分支切片逐支含
 *           kidpal_story_session_ctx_set(0 + 计数 >= 3 + 既有消费锚在守 */
TEST_CASE("record session lifecycle: start-listening early-exit branches clear ctx (RSL2-A3)",
          "[record_session_lifecycle][RSL2-A3][structural]")
{
    char *app;
    const char *fn;
    const char *fn_end;
    const char *br;
    const char *br_end;
    int n;

    RSL2_REQUIRE_ROOT();

    app = rsl2_strip_file(RSL2_APP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(app, "RSL2-A3 RED(infra): application.cc unreadable");
    fn = strstr(app, RSL3_HSL_START);
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "RSL2-A3 RED(infra): application.cc 无 HandleStartListeningEvent 定义锚"
        "（多入口中枢被改名，须复核锚不得改判为绿）");
    fn_end = strstr(fn + strlen(RSL3_HSL_START), RSL3_HSL_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(fn_end,
        "RSL2-A3 RED(infra): HandleStartListeningEvent 切片终点 HandleStopListening"
        "Event 缺席（须复核锚不得改判为绿）");

    /* ---- 既有消费锚在守（[G3] 聆听形态切换链不得被 M3 连带拆动） ------------- */
    n = rsl2_count_in(fn, (size_t)(fn_end - fn), "kidpal_story_session_ctx_active(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        rsl2_fm("RSL2-A3 RED(infra): HandleStartListeningEvent 内 ctx_active 消费 "
                "仅 %d 处（冻结基线 >= 2: Idle 形态切换 + Speaking 打断衔接）——"
                "[G3] 既有消费链被拆动，须复核不得改判为绿", n));
    TEST_ASSERT_TRUE_MESSAGE(
        rsl2_find_in(fn, fn_end, "SetListeningMode(") != NULL,
        "RSL2-A3 RED(infra): HandleStartListeningEvent 内 SetListeningMode( 缺席"
        "（既有形态切换锚漂移，须复核不得改判为绿）");

    /* ---- 三个早退分支逐支清位（RED today） ----------------------------------- */
    br = rsl2_find_in(fn, fn_end, "kDeviceStateWifiConfiguring");
    TEST_ASSERT_NOT_NULL_MESSAGE(br,
        "RSL2-A3 RED(infra): 函数内无 kDeviceStateWifiConfiguring 分支锚（须复核）");
    br_end = rsl2_find_in(br, fn_end, "return;");
    TEST_ASSERT_NOT_NULL_MESSAGE(br_end, "RSL2-A3 RED(infra): WifiConfiguring 分支 return 缺席");
    TEST_ASSERT_TRUE_MESSAGE(rsl2_find_in(br, br_end, RSL2_CTX_CLEAR) != NULL,
        "RSL2-A3 RED: WifiConfiguring 早退分支未清记录语境标志（kidpal_story_"
        "session_ctx_set(0 缺席）——置位后聆听没真起来即 return，ctx 永久停 1 "
        "（泄漏 B 入口一形态）");

    br = rsl2_find_in(fn, fn_end, "Protocol not initialized");
    TEST_ASSERT_NOT_NULL_MESSAGE(br,
        "RSL2-A3 RED(infra): 函数内无 !protocol_ 分支日志锚（须复核）");
    br_end = rsl2_find_in(br, fn_end, "return;");
    TEST_ASSERT_NOT_NULL_MESSAGE(br_end, "RSL2-A3 RED(infra): !protocol_ 分支 return 缺席");
    TEST_ASSERT_TRUE_MESSAGE(rsl2_find_in(br, br_end, RSL2_CTX_CLEAR) != NULL,
        "RSL2-A3 RED: !protocol_ 早退分支未清记录语境标志——protocol 未初始化时"
        "被拒的 REC_SESSION 留下永久 ctx=1（泄漏 B 形态）");

    br = rsl2_find_in(fn, fn_end, "AutoConnect in-flight");
    TEST_ASSERT_NOT_NULL_MESSAGE(br,
        "RSL2-A3 RED(infra): 函数内无 auto_connect_in_flight_ 分支日志锚（须复核；"
        "真机日志实证 21:34:45 「AutoConnect in-flight, ignore manual start "
        "listening」正是本次卡死的直接现场）");
    br_end = rsl2_find_in(br, fn_end, "return;");
    TEST_ASSERT_NOT_NULL_MESSAGE(br_end, "RSL2-A3 RED(infra): auto_connect 分支 return 缺席");
    TEST_ASSERT_TRUE_MESSAGE(rsl2_find_in(br, br_end, RSL2_CTX_CLEAR) != NULL,
        "RSL2-A3 RED: auto_connect_in_flight_ 早退分支未清记录语境标志——KPLOG "
        "2026-10-27 21:34:45 实证的卡死直接根因未治（被拒后 ctx 永久停 1，长按 "
        "KEY 回不了首页）");

    /* ---- 函数内清位计数 >= 3（防只补一处） ----------------------------------- */
    n = rsl2_count_in(fn, (size_t)(fn_end - fn), RSL2_CTX_CLEAR);
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        rsl2_fm("RSL2-A3 RED: HandleStartListeningEvent 内 ctx 清位仅 %d 处（冻结"
                "基线 >= 3: 三处「聆听没真起来」早退边缘逐处清位）——只补一处 = "
                "其余入口仍泄漏（plan M3 逐字）", n));

    free(app);
}

/* ========================================================================== *
 * TEST 4 — RSL2-A4 冻结语义 canary（行为级；[R2] 决策表复跑，RSR-A3 第二站点）。
 * kidpal_story_session_end: RECORDING -> 1 且 out = LIST；LIST/READER/DELETE/
 * STAMP/ORGANIZING -> 0 且 out 零触碰；NULL out -> -1；非录音态幂等。
 * RED today（独立通道）: story_page.c 不入独立链接 -> weak 哨兵 -99（通道形态,
 * GAP-4；host runner strong symbol 通道 must-stay-green）。
 * ========================================================================== */
/* coverage: RSL2-A4 -- 冻结语义 canary（行为）：[R2] 决策表全列复跑（RSR-A3
 *           第二站点; 权威站点 record_session_repair.acceptance.c 继续在跑） */
TEST_CASE("record session lifecycle: frozen session_end semantics canary (RSL2-A4)",
          "[record_session_lifecycle][RSL2-A4][linux][behavior]")
{
    int out;
    int rc;

    /* ---- RECORDING -> 1 且 out = LIST（[R2] 唯一放行列） --------------------- */
    out = -123;
    rc = kidpal_story_session_end(KSTORY_PAGE_RECORDING, &out);
    TEST_ASSERT_TRUE_MESSAGE(rc != RSL2_UNIMPLEMENTED,
        "RSL2-A4 RED(通道形态): 独立通道未链接 display/story_page.c，canary 落在 "
        "weak 哨兵 -99（GAP-4——冻结语义本体已在世，非本包 story 缺失；运行面判据 "
        "= host runner strong symbol 通道，RSR 独立通道常态同款）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, rc,
        "RSL2-A4 RED: kidpal_story_session_end(RECORDING) 返回非 1——[R2] 冻结语义"
        "漂移（收尾判据权威 = record_session_repair.acceptance.c [R2]，本包不得"
        "放宽/改写）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_PAGE_LIST, out,
        "RSL2-A4 RED: RECORDING 出参非 KSTORY_PAGE_LIST——收尾复位页态漂移");

    /* ---- 非录音态闭集 -> 0 且 out 零触碰（不得误清列表/阅读/删除/盖章/整理） -- */
    {
        static const int not_rec[] = {
            KSTORY_PAGE_LIST, KSTORY_PAGE_READER, KSTORY_PAGE_DELETE,
            KSTORY_PAGE_STAMP, KSTORY_PAGE_ORGANIZING
        };
        int i;
        for (i = 0; i < (int)(sizeof(not_rec) / sizeof(not_rec[0])); i++) {
            out = -77;
            rc = kidpal_story_session_end(not_rec[i], &out);
            TEST_ASSERT_TRUE_MESSAGE(rc == 0 && out == -77,
                rsl2_fm("RSL2-A4 RED: kidpal_story_session_end(页态 %d) = (rc=%d, "
                        "out=%d)——[R2] 冻结语义漂移（非 RECORDING 必须返 0 且 "
                        "out_page 零触碰；ORGANIZING 收尾不落本判据是 plan 明确"
                        "不选 B3 的前提）", not_rec[i], rc, out));
        }
    }

    /* ---- 非录音态幂等：同入参两次调用同出 ------------------------------------ */
    out = -77;
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_session_end(KSTORY_PAGE_LIST, &out),
        "RSL2-A4 RED: 幂等段首调非 0");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_session_end(KSTORY_PAGE_LIST, &out),
        "RSL2-A4 RED: 幂等段二调非 0——非录音态收尾必须幂等 no-op");

    /* ---- 畸形入参防御: NULL out -> -1（不崩） -------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_session_end(KSTORY_PAGE_RECORDING, NULL),
        "RSL2-A4 RED: out_page == NULL 未返 -1——畸形入参拒绝缺失（[R2] 冻结防御列）");
}

/* ========================================================================== *
 * TEST 5 — RSL2-A4 回归网登记守（anti-vacuity / 注册序 / canary 实现源在册）。
 * 本件登记进 HOST_ACCEPTANCE_SRCS（builder FIRST ACTION）+ 回归网八族冻结件
 * 登记在守 + display/story_page.c 实现源在册（EQ-4 strong symbol 通道）。
 * RED today（设计态）: 本件未登记进 host CMakeLists（其余为 must-stay-green 守卫）。
 * ========================================================================== */
/* coverage: RSL2-A4 -- 回归网登记守：本件登记 + 注册序（smoke 之前）+ 八冻结件
 *           在守 + story_page.c 实现源在守；运行面 = host 全量实跑对照 237T3F */
TEST_CASE("record session lifecycle: regression net registration guards — self + eight frozen files (RSL2-A4)",
          "[record_session_lifecycle][RSL2-A4][structural]")
{
    char *cmake;
    const char *p_self;
    const char *p_smoke;

    RSL2_REQUIRE_ROOT();

    cmake = rsl2_read(RSL2_HOST_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake, "RSL2-A4 RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "host_infra_smoke.acceptance.c") != NULL,
        "RSL2-A4 RED(infra): host CMakeLists 无 host_infra_smoke.acceptance.c——smoke "
        "末位锚缺席（注册序守卫失去基准，须复核不得改判为绿）");

    /* ---- 本件登记（RED today; builder FIRST ACTION） ------------------------- */
    p_self = strstr(cmake, "record_session_lifecycle.acceptance.c");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_self,
        "RSL2-A4 RED: host_app/main/CMakeLists.txt 未登记 record_session_lifecycle."
        "acceptance.c——本冻结验收件不进 host runner（全部断言不会被执行 = 空过绿）；"
        "builder FIRST ACTION（261022-diag 先例口径，登记行不在 acceptance-author "
        "写权内）");
    p_smoke = strstr(cmake, "host_infra_smoke.acceptance.c");
    TEST_ASSERT_TRUE_MESSAGE(p_self < p_smoke,
        "RSL2-A4 RED: record_session_lifecycle.acceptance.c 登记在 "
        "host_infra_smoke.acceptance.c 之后——必须置于 smoke 之前（smoke 末位 = "
        "最先执行，包 0 boundary 注册序守卫）");

    /* ---- 回归网八族在守（plan「必须保持绿」清单逐件; 不被本包登记改动静默摘除） */
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "record_session_repair.acceptance.c") != NULL,
        "RSL2-A4 RED: 回归网成员 record_session_repair.acceptance.c 从 host 登记"
        "消失——[R2]/[R5] 冻结 seam 权威验收件（RSR-A3 语义 / RSR-A7 判别位）必须"
        "继续全量在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "record_session_repair.test.c") != NULL,
        "RSL2-A4 RED: 回归网成员 record_session_repair.test.c 从 host 登记消失——"
        "[R2] 边界/幂等补强测试必须继续在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "record_dialogue_guide.acceptance.c") != NULL,
        "RSL2-A4 RED: 回归网成员 record_dialogue_guide.acceptance.c 从 host 登记"
        "消失——RDG-F4 ctx 顺序锚 / RDG-F5 冻结齿必须继续在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "record_session_live.acceptance.c") != NULL,
        "RSL2-A4 RED: 回归网成员 record_session_live.acceptance.c 从 host 登记消失"
        "——RSL-A5 正负锚（STOP/DISCARD 接线）必须继续在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "story_page_v2.acceptance.c") != NULL,
        "RSL2-A4 RED: 回归网成员 story_page_v2.acceptance.c 从 host 登记消失——"
        "v1.47 输入表冻结决策必须继续在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "chrome_ghost.acceptance.c") != NULL,
        "RSL2-A4 RED: 回归网成员 chrome_ghost.acceptance.c 从 host 登记消失——"
        "CG-A1 收尾沿 scene 复位锚（与本件 Idle 序锚同区互守）必须继续在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "record_dialogue_v7_fix.test.c") != NULL,
        "RSL2-A4 RED: 回归网成员 record_dialogue_v7_fix.test.c 从 host 登记消失——"
        "show_home/voice_end 守卫在世测试必须继续在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "key_layer_dispatch.acceptance.c") != NULL,
        "RSL2-A4 RED: 回归网成员 key_layer_dispatch.acceptance.c 从 host 登记消失"
        "——键路由/层栈冻结语义必须继续在跑");

    /* ---- canary 实现源在册（EQ-4: strong symbol 通道） ----------------------- */
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "../../../display/story_page.c") != NULL,
        "RSL2-A4 RED: 实现源 display/story_page.c 从 host 登记消失——canary 冻结"
        "语义 strong symbol 无处落地（weak 哨兵红会掩盖真实回归, EQ-4）");
    free(cmake);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-28 初版冻结（TaskPackage 261028-p3-record-session-mirror-desync-fix;
 *               acceptance[] RSL2-A1~A4 + RSL2-A5 structural/live-manual 为冻结
 *               边界; 语义权威 = reviewed plan（architect /triage-change
 *               2026-10-27 比例化 review + human 触点①确认 2026-10-28））。五个
 *               TEST_CASE：RSL2-A1 结构一件（M1 镜像单点派生 + 恰 2 处负锚）、
 *               RSL2-A2 结构一件（M2 Idle 五符号序锚 + Sync 定义体形状 + cache_
 *               story 零命中负锚）、RSL2-A3 结构一件（M3 三早退分支逐支清位 +
 *               计数 >= 3）、RSL2-A4 行为一件（[R2] 冻结语义 canary，weak 哨兵/
 *               strong symbol 双态）+ 结构一件（登记守 + 回归网八族）。RSL2-A5
 *               为 live-manual 真机轮观测项（T1-T5），不落自动断言，本 Pod 到
 *               真机门前停等 human。输出散文纪律：验收测试 case 不用黑话。
 * --------------------------------------------------------------------------- */
