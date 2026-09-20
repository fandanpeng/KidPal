/*
 * char_stroke.h — KidPal 查生字 (P4): D3 笔顺消费 + 1bpp 光栅 + 动画状态机 +
 * 速查卡 + 缺字段防御降级 的 FROZEN OFFLINE CONTRACT。
 *
 * 语义与数值码以冻结验收 firmware/main/test/char_stroke.acceptance.c 文件头
 * 为准，不得偏移；本头声明与该文件 fallback 块逐字一致（C linkage）。纯逻辑、
 * 无芯片/FreeRTOS/ESP-component/cJSON 依赖 -> 可编入 host_app linux target 与
 * 设备树两侧。实现见 char_stroke.c。
 *
 * 降级语义（闸 A 包内裁定, gate-a-prereview §2.2）：缺必需字段的 view:"char"
 * data 必须显式降级（友好提示、不盲渲染、不崩），对齐 view_router UNKNOWN 降级
 * 语义。
 */
#ifndef KIDPAL_CHAR_STROKE_H
#define KIDPAL_CHAR_STROKE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MCF-A2 编译期探针宏（240920-m2-charwindow-card-fix additive）: kidpal_char_info 增
 * meaning 成员后同步定义。frozen acceptance 的 TEST 6 行为齿以此宏为编译门——落地前
 * fallback 0 = 行为齿缺席编译（非空过, 红由 TEST 5 结构齿承载）。漏定义 = TEST 5 红。 */
#define KIDPAL_CHAR_STROKE_HAS_MEANING 1

typedef struct {
    float x, y;
} kidpal_cs_point;

typedef struct {
    const char *pinyin;      /* 带声调, verbatim from D3 (e.g. "yǒng") */
    const char *radical;     /* verbatim (e.g. "力") */
    int        stroke_count; /* from D3 strokeCount */
    const char *words;       /* 、-joined (e.g. "勇敢、勇气"), verbatim */
    const char *meaning;     /* 释义字典 definition (S-F3.2 AC1 五行口径), verbatim;
                                **NULL 容忍**——缺释义只损右栏一行、不降级整窗, 与
                                pinyin/radical/words 必非空语义不同（human 裁降级口径） */
} kidpal_char_info;

typedef struct {
    char pinyin[48];
    char radical[48];
    char stroke_total[48];   /* "共 <N> 画" */
    char progress[48];       /* "第 <X> 笔·共 <N> 笔" | "写完啦·共 <N> 笔" */
    char words[96];
    char meaning[96];        /* 释义, 容量对齐 words 惯例; 缺义数据面装配空串 */
} kidpal_char_card;

int kidpal_char_stroke_parse(const char *svg_path, kidpal_cs_point *out, int max_pts);
int kidpal_char_medians_parse(const char *median_json, kidpal_cs_point *out, int max_pts);
int kidpal_char_transform(float x, float y, int *sx, int *sy);
int kidpal_char_raster_stroke(const kidpal_cs_point *outline, int npts,
                              uint8_t *bitmap, int w, int h, int stride_bytes);
int kidpal_char_anim_state(int elapsed_ms, int n_strokes, int *cur_stroke, int *written);
int kidpal_char_stroke_state(int stroke_index, int written_count, int cur_stroke);
int kidpal_char_dash_dot_pitch(void);
int kidpal_char_card_build(const kidpal_char_info *info, int cur_stroke, kidpal_char_card *out);
int kidpal_char_data_validate(const char *data_json);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_CHAR_STROKE_H */