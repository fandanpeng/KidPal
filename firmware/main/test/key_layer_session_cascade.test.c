/*
 * key_layer_session_cascade.test.c — 包 260915-m1-k1a-key-layer-dispatch KD-A5 真机轮
 * 行为①修复（round 1 human 裁定重跑）的**回归守卫**（普通 `.test.c`，非冻结验收，
 * HOST_TEST_CONVENTIONS [C4.3]）。
 *
 * 现场：覆盖层（VOICE 层）顶层按 BOOT -> 路由正确（key_route_layer 覆盖层列 ->
 * GLOBAL_ACT_OVERLAY_CLOSE -> dismiss_front_overlay 关层回原页），**但语音会话未级联
 * 结束**——状态机停在 listening、human 还能继续对话（真机日志
 * delivery/changes/dev/260915-m1-k1a-key-layer-dispatch/device-round-keylayer-r1.log
 * 两处 action=4 后均无 listening->idle，119 行 listening->speaking 证实会话存活）。
 * 修法 = 板级 dispatch OVERLAY_CLOSE case 增加会话级联：前台为 VOICE 覆盖层且设备在
 * 会话态时复用 KEY 单击同一条 ToggleChatState 会话结束路径（listening =
 * CloseAudioChannel）；判据 = 新增 kidpal_render_node_front_is_voice()（只认 VOICE
 * 单层，提醒弹窗等非会话覆盖层关层语义不变）。requirements §2.3「问答·聆听/思考」行
 * BOOT 短按 = 结束会话回原页。
 *
 * 齿族（结构齿，被测 board/render-node 实现不进 host 构建 -> 按 key_route_a1 /
 * hardware_validation 先例做树级扫描；同族检查全覆盖——dismiss_front_overlay 设备侧
 * 仅两个消费点：板级 OVERLAY_CLOSE dispatch case 与 keya_overlay_consume 提醒路径）：
 *   T1 级联接线齿 —— 板级 OVERLAY_CLOSE case 内须同时含 front_is_voice 判据 +
 *      ToggleChatState 会话结束 + dismiss_front_overlay 关层三件套；
 *   T2 判据落点齿 —— front_is_voice 在 kidpal_render_node.h/.cc 声明 + 定义在位
 *      （只认 KIDPAL_LAYER_VOICE）；
 *   T3 非会话不变齿 —— keya_overlay_consume（提醒弹窗顶层裁决消费路径）不得出现
 *      ToggleChatState（提醒弹窗关层语义零改动：本就无会话，不级联）。
 *
 * Run protocol: 随 host runner 一并执行（本文件已接线进
 * firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS）：
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 */
#include "sdkconfig.h"

#if defined(CONFIG_IDF_TARGET_LINUX)

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 本守卫扫描的工作树文件（相对仓根）。 */
#define KSC_F_BOARD_CC "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define KSC_F_RN_H     "firmware/main/display/kidpal_render_node.h"
#define KSC_F_RN_CC    "firmware/main/display/kidpal_render_node.cc"
#define KSC_MAX_READ   (8u * 1024u * 1024u)
#define KSC_MAX_PATH   1024

/* -------------------------------------------------------------------------- *
 * 工作树定位 + 读取（hardware_validation.acceptance.c / story_page_stack.test.c
 * 同款先例：root 以手持 cwd 定位，从仓根或 firmware/main/test/host_app 两处都可跑）。
 * ------------------------------------------------------------------------ */

static char ksc_root[KSC_MAX_PATH];
static int ksc_root_ready;

static int ksc_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KSC_MAX_PATH];

    if (ksc_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        FILE *f = fopen(probe, "rb");
        if (f != NULL) {
            fclose(f);
            snprintf(ksc_root, sizeof(ksc_root), "%s", cands[i]);
            ksc_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static char *ksc_read(const char *rel)
{
    char path[KSC_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!ksc_locate_root()) {
        return NULL;
    }
    snprintf(path, sizeof(path), "%s/%s", ksc_root, rel);
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KSC_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "green" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    fclose(f);
    buf[sz] = '\0';
    return buf;
}

/* -------------------------------------------------------------------------- *
 * TEST_CASE：KD-A5 会话级联守卫（T1 + T2 + T3，单 CASE 三齿）
 * ------------------------------------------------------------------------ */

TEST_CASE("key layer cascade: OVERLAY_CLOSE on VOICE overlay ends the session, reminder path stays session-free (KD-A5)", "[key_layer][KD-A5][linux]")
{
    char *board_cc = ksc_read(KSC_F_BOARD_CC);
    assert(board_cc != NULL); /* logic guard: missing worktree file must fail loudly */

    /* T1 级联接线齿：OVERLAY_CLOSE case 段（case 起点至 switch 的 default）内须
     * 同时在位三件套——front_is_voice 判据 + ToggleChatState 会话结束 + dismiss
     * 关层。判据缺失即回退为「只关屏不结束会话」的 r1 缺陷形态。 */
    const char *cs = strstr(board_cc, "case GLOBAL_ACT_OVERLAY_CLOSE:");
    assert(cs != NULL); /* logic guard: dispatch case must exist in board source */
    const char *ce = strstr(cs, "\n    default:");
    assert(ce != NULL && ce > cs); /* logic guard: switch default must follow */
    size_t seg_len = (size_t)(ce - cs);
    char *seg = (char *)malloc(seg_len + 1);
    assert(seg != NULL);
    memcpy(seg, cs, seg_len);
    seg[seg_len] = '\0';

    assert(strstr(seg, "kidpal_render_node_front_is_voice") != NULL);
    assert(strstr(seg, "ToggleChatState") != NULL);
    assert(strstr(seg, "kidpal_render_node_dismiss_front_overlay") != NULL);
    free(seg);

    /* T3 非会话不变齿：keya_overlay_consume（提醒弹窗顶层裁决 -> DISMISS 即关层
     * 消费）窗口内不得出现 ToggleChatState——提醒弹窗关层语义不变（本就无会话，
     * 不级联）。窗口 = 函数起点起 640 字节，足够覆盖其函数体（含 KRM_KEY_DISMISS
     * 消费点）、不足以上溯到 OVERLAY_CLOSE case 的新增级联注释。 */
    const char *oc = strstr(board_cc, "static int keya_overlay_consume");
    assert(oc != NULL); /* logic guard: overlay consume fn must exist */
    const char *oc_end = oc + 640;
    assert((size_t)(oc_end - board_cc) <= strlen(board_cc));
    /* in-window sanity: 函数体内 KRM_KEY_DISMISS 消费点在窗口内
     * （从函数起点向后找——doc 注释里的同名 token 在函数起点之前、不作数）。 */
    const char *krm = strstr(oc, "KRM_KEY_DISMISS");
    assert(krm != NULL && krm < oc_end);
    char *oc_win = (char *)malloc(641);
    assert(oc_win != NULL);
    memcpy(oc_win, oc, 640);
    oc_win[640] = '\0';
    assert(strstr(oc_win, "ToggleChatState") == NULL);
    free(oc_win);
    free(board_cc);

    /* T2 判据落点齿：front_is_voice 声明（.h）+ 定义（.cc）在位，且定义只认
     * KIDPAL_LAYER_VOICE 单层（提防误扩成 overlay 全集导致提醒弹窗关层被误级联）。 */
    char *rn_h = ksc_read(KSC_F_RN_H);
    assert(rn_h != NULL);
    assert(strstr(rn_h, "int kidpal_render_node_front_is_voice(void);") != NULL);
    free(rn_h);

    char *rn_cc = ksc_read(KSC_F_RN_CC);
    assert(rn_cc != NULL);
    const char *def = strstr(rn_cc, "int kidpal_render_node_front_is_voice(void)");
    assert(def != NULL); /* definition must exist in render node impl */
    assert(strstr(def, "KIDPAL_LAYER_VOICE") != NULL &&
           strstr(def, "KIDPAL_LAYER_VOICE") < def + 256); /* single-layer criterion nearby */
    free(rn_cc);
}

#endif /* CONFIG_IDF_TARGET_LINUX */
