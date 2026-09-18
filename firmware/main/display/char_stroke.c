/*
 * char_stroke.c — KidPal 查生字 (P4): D3 笔顺消费 + 1bpp 光栅 + 动画状态机 +
 * 速查卡 + 缺字段防御降级 的 FROZEN OFFLINE CONTRACT 纯逻辑实现。
 *
 * 语义与数值码以冻结验收 firmware/main/test/char_stroke.acceptance.c 文件头
 * 为准，不得偏移；weak 哨兵被本文件的强定义覆盖后在 host runner 里果真被测。
 *
 * 纯逻辑、无芯片/FreeRTOS/ESP 组件依赖 -> 编入 host_app linux target 与设备树
 * 两侧。降级语义：缺必需字段 char 块 -> 显式 -1 (degrade)，调用方展示友好降级
 * 提示、绝不盲渲染 (闸 A 包内裁定 / view_router UNKNOWN 对偶)。
 */

#include "char_stroke.h"

/* 换皮接线声明面 (包 9b / ART-A6)：本模块是 P4 冻结纯逻辑合约——B8 打包红线禁止其
 * 引「按路径取位图 / 整帧烘焙」类资产原语（以免逐笔演示被整帧预烘焙位图替换），且参数化
 * 整帧缝 (draw_nine_slice/draw_dots) 会把 art_assets.c 的整帧 g_fb[400x300] (~480KB) 拖进
 * 设备 .dram0.bss（该缝设备端为弱默认 no-op，真机渲染节点殿后本包）。故 P4 真机资产
 * 消费（田字格大框位图）落在装配面 char_page.cc（非 host 编译、B8 不扫）。本 include
 * 是 ART-A6「六渲染面统一挂资产层」的接线声明，不在此处发起任何会拉入整帧 g_fb 的调用。 */
#include "art_assets.h"

#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 变换几何 —— 数值码与冻结验收一致 (spec §3.0.2) */
#define CS_SRC_SIZE 1024
#define CS_SCREEN_W 400
#define CS_SCREEN_H 300

/* 逐笔演示时序 (S-F3.1) */
#define CS_STROKE_MS 950
#define CS_REST_MS 2000

/* Q/C 贝塞尔离散密度：每 Q 段/C 段内点样本数 (>= 2 保证捕获曲线形状, CHR-A2)。
 * Q (二次) 与 C (三次) 共用同一采样密度, 端点精确落于曲线上。 */
#define CS_BEZIER_SEGS 8

/* 解析用绝对形 token */
typedef struct {
    char type;   /* 1 = command letter (num 存 ascii); 0 = number */
    float num;
} cs_tok;

/* 把单个 D3 stroke path 切成 token 序列。返回 token 数, -1 防御 (未知字符/超额) */
static int cs_tokenize(const char *path, cs_tok *toks, int cap)
{
    const char *p = path;
    int nt = 0;

    while (*p != '\0') {
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        char c = *p;
        /* C/c = 三次贝塞尔绝对/相对命令（词汇表闭集扩充至 makemeahanzi 实况 vocab——
         * D3 strokes 自由文本含 C, M2-K1 缺陷①修复；小写按既有 M/L/Q/Z 先例与绝对
         * 同语义消费, 未知字符拒绝语义不放宽） */
        if (c == 'M' || c == 'L' || c == 'Q' || c == 'C' || c == 'Z' ||
            c == 'm' || c == 'l' || c == 'q' || c == 'c' || c == 'z') {
            if (nt >= cap) {
                return -1;
            }
            toks[nt].type = 1;
            toks[nt].num = (float)(int)c;
            nt++;
            p++;
            continue;
        }
        if (c == '-' || c == '+' || c == '.' ||
            (c >= '0' && c <= '9')) {
            char *end = NULL;
            float v = strtof(p, &end);
            if (end == NULL || end == p) {
                return -1; /* 无数字可解析 */
            }
            if (nt >= cap) {
                return -1;
            }
            toks[nt].type = 0;
            toks[nt].num = v;
            nt++;
            p = end;
            continue;
        }
        return -1; /* 未知字符 */
    }
    return nt;
}

/* 读取两个连续 number token (x,y)。返回 0 成功, -1 失败 (取不到/读到命令) */
static int cs_read_pair(const cs_tok *toks, int nt, int *i, float *x, float *y)
{
    if (*i + 1 >= nt) {
        return -1;
    }
    if (toks[*i].type != 0 || toks[*i + 1].type != 0) {
        return -1;
    }
    *x = toks[*i].num;
    *y = toks[*i + 1].num;
    *i += 2;
    return 0;
}

/* 读取四个连续 number token (cx,cy,ex,ey)。返回 0 成功, -1 失败 */
static int cs_read_quad(const cs_tok *toks, int nt, int *i,
                        float *cx, float *cy, float *ex, float *ey)
{
    float x1, y1, x2, y2;
    if (cs_read_pair(toks, nt, i, &x1, &y1) != 0 ||
        cs_read_pair(toks, nt, i, &x2, &y2) != 0) {
        return -1;
    }
    *cx = x1; *cy = y1; *ex = x2; *ey = y2;
    return 0;
}

/* 追加一个点; 容量不足 -> -1 (绝不越界写, CHR-A2 overflow 语义) */
static int cs_push(kidpal_cs_point *out, int *count, int max_pts, float x, float y)
{
    if (*count >= max_pts) {
        return -1;
    }
    out[*count].x = x;
    out[*count].y = y;
    (*count)++;
    return 0;
}

/* 二次贝塞尔 B(t), t in [0,1], P0=(x0,y0) C=(cx,cy) P1=(ex,ey) */
static void cs_bezier_t(float t, float x0, float y0,
                        float cx, float cy, float ex, float ey,
                        float *bx, float *by)
{
    float u = 1.0f - t;
    *bx = u * u * x0 + 2.0f * u * t * cx + t * t * ex;
    *by = u * u * y0 + 2.0f * u * t * cy + t * t * ey;
}

/* 读取六个连续 number token (c1x,c1y,c2x,c2y,ex,ey) —— C 段 = 两控制点 + 终点
 * (makemeahanzi 词汇表实况形状)。返回 0 成功, -1 失败 */
static int cs_read_cubic(const cs_tok *toks, int nt, int *i,
                         float *c1x, float *c1y, float *c2x, float *c2y,
                         float *ex, float *ey)
{
    float x1, y1, x2, y2, x3, y3;
    if (cs_read_pair(toks, nt, i, &x1, &y1) != 0 ||
        cs_read_pair(toks, nt, i, &x2, &y2) != 0 ||
        cs_read_pair(toks, nt, i, &x3, &y3) != 0) {
        return -1;
    }
    *c1x = x1; *c1y = y1; *c2x = x2; *c2y = y2; *ex = x3; *ey = y3;
    return 0;
}

/* 三次贝塞尔 B(t), t in [0,1]: P0=(x0,y0) C1/C2=两控制点 P1=(ex,ey)
 * B(t) = (1-t)^3·P0 + 3(1-t)^2·t·C1 + 3(1-t)·t^2·C2 + t^3·P1 */
static void cs_bezier_cubic_t(float t, float x0, float y0,
                              float c1x, float c1y, float c2x, float c2y,
                              float ex, float ey,
                              float *bx, float *by)
{
    float u = 1.0f - t;
    float uu = u * u, tt = t * t;
    *bx = uu * u * x0 + (3.0f * uu * t) * c1x + (3.0f * u * tt) * c2x + tt * t * ex;
    *by = uu * u * y0 + (3.0f * uu * t) * c1y + (3.0f * u * tt) * c2y + tt * t * ey;
}

int kidpal_char_stroke_parse(const char *svg_path, kidpal_cs_point *out, int max_pts)
{
    cs_tok toks[256];
    int nt;
    int i = 0, count = 0;
    float cur_x = 0.0f, cur_y = 0.0f;
    float sub_x = 0.0f, sub_y = 0.0f;
    char cmd = 0;

    if (svg_path == NULL || out == NULL || max_pts <= 0) {
        return -1;
    }
    if (svg_path[0] == '\0') {
        return -1;
    }
    nt = cs_tokenize(svg_path, toks, (int)(sizeof(toks) / sizeof(toks[0])));
    if (nt <= 0) {
        return -1;
    }

    while (i < nt) {
        /* 取命令字母 (连续 number 隐式续接上一命令, SVG 语义) */
        if (toks[i].type == 1) {
            cmd = (char)(int)toks[i].num;
            i++;
        } else if (cmd == 0) {
            return -1; /* 首个 token 不是命令 */
        }

        switch (cmd) {
        case 'M':
        case 'm': {
            /* moveto: 首对 -> 起点并复位子路径; 后续对 -> 按 lineto (SVG 语义) */
            float x, y;
            if (cs_read_pair(toks, nt, &i, &x, &y) != 0) {
                return -1;
            }
            cur_x = x; cur_y = y;
            sub_x = x; sub_y = y;
            if (cs_push(out, &count, max_pts, x, y) != 0) {
                return -1;
            }
            /* 隐式续接按 L 处理 */
            if (i < nt && toks[i].type == 0) {
                do {
                    float lx, ly;
                    if (cs_read_pair(toks, nt, &i, &lx, &ly) != 0) {
                        return -1;
                    }
                    cur_x = lx; cur_y = ly;
                    if (cs_push(out, &count, max_pts, lx, ly) != 0) {
                        return -1;
                    }
                } while (i < nt && toks[i].type == 0);
            }
            break;
        }
        case 'L':
        case 'l': {
            /* lineto: 逐点精确 */
            while (i < nt && toks[i].type == 0) {
                float lx, ly;
                if (cs_read_pair(toks, nt, &i, &lx, &ly) != 0) {
                    return -1;
                }
                cur_x = lx; cur_y = ly;
                if (cs_push(out, &count, max_pts, lx, ly) != 0) {
                    return -1;
                }
            }
            break;
        }
        case 'Q':
        case 'q': {
            /* quadratic bezier: 连续 4 个 number = (控制点, 终点) */
            while (i < nt && toks[i].type == 0) {
                float cx, cy, ex, ey;
                if (cs_read_quad(toks, nt, &i, &cx, &cy, &ex, &ey) != 0) {
                    return -1;
                }
                /* 采样 >= 2 内点 (CS_BEZIER_SEGS), 端点精确落于曲线上 */
                for (int s = 1; s <= CS_BEZIER_SEGS; s++) {
                    float t = (float)s / (float)CS_BEZIER_SEGS;
                    float bx, by;
                    cs_bezier_t(t, cur_x, cur_y, cx, cy, ex, ey, &bx, &by);
                    if (s == CS_BEZIER_SEGS) {
                        bx = ex; by = ey; /* 端点精确 */
                    }
                    if (cs_push(out, &count, max_pts, bx, by) != 0) {
                        return -1;
                    }
                }
                cur_x = ex; cur_y = ey;
            }
            break;
        }
        case 'C':
        case 'c': {
            /* cubic bezier: 连续 6 个 number = (控制点1, 控制点2, 终点)。与既有 Q
             * 同款离散成点策略（CS_BEZIER_SEGS 采样 + 端点精确落于曲线）, 复用既点集
             * 管线, 不引入新渲染路径——M2-K1 缺陷①修复 (6/9 笔含 C 不再被整笔丢弃)。 */
            while (i < nt && toks[i].type == 0) {
                float c1x, c1y, c2x, c2y, ex, ey;
                if (cs_read_cubic(toks, nt, &i, &c1x, &c1y, &c2x, &c2y, &ex, &ey) != 0) {
                    return -1;
                }
                /* 采样 >= 2 内点 (CS_BEZIER_SEGS), 端点精确落于曲线上 */
                for (int s = 1; s <= CS_BEZIER_SEGS; s++) {
                    float t = (float)s / (float)CS_BEZIER_SEGS;
                    float bx, by;
                    cs_bezier_cubic_t(t, cur_x, cur_y, c1x, c1y, c2x, c2y,
                                      ex, ey, &bx, &by);
                    if (s == CS_BEZIER_SEGS) {
                        bx = ex; by = ey; /* 端点精确 */
                    }
                    if (cs_push(out, &count, max_pts, bx, by) != 0) {
                        return -1;
                    }
                }
                cur_x = ex; cur_y = ey;
            }
            break;
        }
        case 'Z':
        case 'z': {
            /* closepath: 回到当前子路径起点 (孔洞闭合, CHR-A4 依赖) */
            if (cs_push(out, &count, max_pts, sub_x, sub_y) != 0) {
                return -1;
            }
            break;
        }
        default:
            return -1; /* 未知命令 */
        }
    }

    if (count <= 0) {
        return -1;
    }
    return count;
}

int kidpal_char_medians_parse(const char *median_json, kidpal_cs_point *out, int max_pts)
{
    const char *p;
    int count = 0;

    if (median_json == NULL || out == NULL || max_pts <= 0) {
        return -1;
    }

    p = median_json;
    /* 跳过前导空白 */
    while (*p != '\0' && isspace((unsigned char)*p)) {
        p++;
    }
    if (*p != '[') {
        return -1;
    }
    p++;

    for (;;) {
        /* 期望 '[' */
        while (*p != '\0' && isspace((unsigned char)*p)) {
            p++;
        }
        if (*p == '\0') {
            return -1; /* 缺尾 ']' */
        }
        if (*p == ']') {
            /* 数组收尾 */
            p++;
            break;
        }
        if (*p != '[') {
            return -1;
        }
        p++;
        {
            char *end = NULL;
            float x = strtof(p, &end);
            if (end == NULL || end == p) {
                return -1;
            }
            p = end;
            while (*p != '\0' && isspace((unsigned char)*p)) {
                p++;
            }
            if (*p != ',') {
                return -1;
            }
            p++;
            while (*p != '\0' && isspace((unsigned char)*p)) {
                p++;
            }
            float y = strtof(p, &end);
            if (end == NULL || end == p) {
                return -1;
            }
            p = end;
            while (*p != '\0' && isspace((unsigned char)*p)) {
                p++;
            }
            if (*p != ']') {
                return -1;
            }
            p++;
            if (count >= max_pts) {
                return -1; /* 缓冲不足 */
            }
            out[count].x = x;
            out[count].y = y;
            count++;
        }
        /* 期望 ',' 或收尾 ']' */
        while (*p != '\0' && isspace((unsigned char)*p)) {
            p++;
        }
        if (*p == '\0') {
            return -1;
        }
        if (*p == ']') {
            p++;
            break;
        }
        if (*p != ',') {
            return -1;
        }
        p++;
    }

    if (count <= 0) {
        return -1;
    }
    return count;
}

int kidpal_char_transform(float x, float y, int *sx, int *sy)
{
    if (sx == NULL || sy == NULL) {
        return -1;
    }

    /* x 不镜像 (P3): 源 x 增大 -> 屏 x 增大。乘 (W-1)/SRC 保证 max < W (P1)。 */
    *sx = (int)(x * (float)(CS_SCREEN_W - 1) / (float)CS_SRC_SIZE);

    /* Y 翻转 (P2): 源 y 增大 -> 屏 y 严格减小 (字库坐标与屏幕 Y 朝向相反)。
     * 乘 (H-1)/SRC: sy 落在 [0, H-1] 屏内。域中心 (512,512) -> 149 < 150 (P4 偏上)。 */
    *sy = (int)(((float)CS_SRC_SIZE - y) * (float)(CS_SCREEN_H - 1) / (float)CS_SRC_SIZE);

    return 0;
}

/* 单行偶数-奇数扫描线填充: 对一行 y (像素中心 y+0.5), 收集边交 X, 排序后两两填充 */
static void cs_raster_row(const kidpal_cs_point *outline, int npts,
                          uint8_t *bitmap, int w, int stride_bytes, int y)
{
    float xs[256];
    int nx = 0;
    float yc = (float)y + 0.5f;

    /* 收集该行与多边形边的交点 (半开区间, 顶点去重) */
    for (int i = 0; i < npts; i++) {
        const kidpal_cs_point *a = &outline[i];
        const kidpal_cs_point *b = &outline[(i + 1) % npts];
        float ya = a->y, yb = b->y;
        if (ya == yb) {
            continue; /* 水平边 */
        }
        float lo = ya < yb ? ya : yb;
        float hi = ya < yb ? yb : ya;
        if (yc < lo || yc >= hi) {
            continue;
        }
        float t = (yc - ya) / (yb - ya);
        float x = a->x + t * (b->x - a->x);
        if (nx < (int)(sizeof(xs) / sizeof(xs[0]))) {
            xs[nx++] = x;
        }
    }

    /* 冒泡排序 x */
    for (int i = 0; i < nx - 1; i++) {
        for (int j = 0; j < nx - 1 - i; j++) {
            if (xs[j] > xs[j + 1]) {
                float tmp = xs[j];
                xs[j] = xs[j + 1];
                xs[j + 1] = tmp;
            }
        }
    }

    /* 偶数-奇数: 两两成对, 列 c 满足 xs[2k] <= c+0.5 < xs[2k+1] 时置位 */
    for (int k = 0; k + 1 < nx; k += 2) {
        float xa = xs[k], xb = xs[k + 1];
        for (int c = (int)ceilf(xa); c < (int)ceilf(xb); c++) {
            if (c < 0) {
                continue;
            }
            if (c >= w) {
                break;
            }
            /* 1bpp MSB-first: 置位 (overlay, 只置位不清除) */
            bitmap[(size_t)y * (size_t)stride_bytes + (size_t)(c / 8)] |=
                (uint8_t)(1u << (7 - (c % 8)));
        }
    }
}

int kidpal_char_raster_stroke(const kidpal_cs_point *outline, int npts,
                              uint8_t *bitmap, int w, int h, int stride_bytes)
{
    if (outline == NULL || bitmap == NULL) {
        return -1;
    }
    if (npts < 3 || w <= 0 || h <= 0 || stride_bytes <= 0) {
        return -1;
    }
    for (int y = 0; y < h; y++) {
        cs_raster_row(outline, npts, bitmap, w, stride_bytes, y);
    }
    return 0;
}

int kidpal_char_anim_state(int elapsed_ms, int n_strokes, int *cur_stroke, int *written)
{
    int cycle;

    if (n_strokes <= 0 || cur_stroke == NULL || written == NULL) {
        return -1;
    }
    cycle = n_strokes * CS_STROKE_MS + CS_REST_MS;
    if (cycle <= 0) {
        return -1;
    }
    {
        int m = elapsed_ms % cycle;
        if (m < n_strokes * CS_STROKE_MS) {
            /* WRITING: 每 950ms 步进一步 */
            *cur_stroke = m / CS_STROKE_MS;
            *written = *cur_stroke + 1;
            return 0; /* KIDPAL_CS_PHASE_WRITING */
        }
        /* REST: 演完一轮定格, 无当前笔 */
        *cur_stroke = -1;
        *written = n_strokes;
        return 1; /* KIDPAL_CS_PHASE_REST */
    }
}

int kidpal_char_stroke_state(int stroke_index, int written_count, int cur_stroke)
{
    if (stroke_index < 0) {
        return -1;
    }
    if (cur_stroke >= 0 && stroke_index == cur_stroke) {
        return 1; /* KIDPAL_CS_STATE_CURRENT —— 当前笔白衬边 + 握笔小手 */
    }
    if (stroke_index < written_count) {
        return 0; /* KIDPAL_CS_STATE_WRITTEN —— 已写实心黑 */
    }
    return 2; /* KIDPAL_CS_STATE_PENDING —— 未写虚线点链 */
}

int kidpal_char_dash_dot_pitch(void)
{
    return 6; /* S-F3.1: 未写虚线点链点距 6px */
}

int kidpal_char_card_build(const kidpal_char_info *info, int cur_stroke,
                           kidpal_char_card *out)
{
    /* 换皮 (包 260911-render-pages RP-A10/RP-A8): 米字格大框单份背景 = frm_236_236
     * (B8 打包策略: 笔形 3 态叠加复用 + 米字格单份背景, 禁逐笔逐态整帧), 由渲染侧
     * (char_page) 经 B8 闭集 frm_ 资产族铺开; 本模块为查字数据面, 观感归 RP-A5 真机轮。 */
    if (info == NULL || out == NULL ||
        info->pinyin == NULL || info->radical == NULL || info->words == NULL) {
        return -1;
    }
    if (info->stroke_count < 0) {
        return -1;
    }

    /* 拼音/部首/组词原样 (截断安全, 恒 NUL 结尾) */
    snprintf(out->pinyin, sizeof(out->pinyin), "%s", info->pinyin);
    snprintf(out->radical, sizeof(out->radical), "%s", info->radical);
    snprintf(out->words, sizeof(out->words), "%s", info->words);

    snprintf(out->stroke_total, sizeof(out->stroke_total),
             "共 %d 画", info->stroke_count);

    if (cur_stroke >= 0 && cur_stroke < info->stroke_count) {
        snprintf(out->progress, sizeof(out->progress),
                 "第 %d 笔·共 %d 笔", cur_stroke + 1, info->stroke_count);
    } else {
        /* 写完定格 (S-F3.3): cur < 0 或 >= N —— ✔ 为图标不在文本内 */
        snprintf(out->progress, sizeof(out->progress),
                 "写完啦·共 %d 笔", info->stroke_count);
    }
    return 0;
}

static const char *const CS_REQ_KEYS[] = {
    "char", "pinyin", "radical", "strokeCount", "strokes", "medians"
};

int kidpal_char_data_validate(const char *data_json)
{
    if (data_json == NULL || data_json[0] == '\0') {
        return -1;
    }
    /* 契约: key":value shape 不深查, 只要六个必需键 "key": 都在即通过。
     * 缺任一 -> -1 (显式降级, 不盲渲染)。 */
    for (size_t k = 0; k < sizeof(CS_REQ_KEYS) / sizeof(CS_REQ_KEYS[0]); k++) {
        char needle[32];
        size_t n = 0;
        n += (size_t)snprintf(needle + n, sizeof(needle) - n, "\"%s\":", CS_REQ_KEYS[k]);
        (void)n;
        if (strstr(data_json, needle) == NULL) {
            return -1;
        }
    }
    return 0;
}