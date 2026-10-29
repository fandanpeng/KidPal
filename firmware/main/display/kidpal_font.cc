/*
 * kidpal_font.cc — 统一 kidpal_font 句柄 seam 强定义（[K1]-[K4]）
 * TaskPackage 261028-m6-k1-font-assets-runtime-loading（P0 spike）。
 *
 * 装载面按目标分派（预基线修正裁定 2 / 2026-10-28 human 双裁）：
 *   设备（CONFIG_IDF_TARGET_ESP32*）= cbin_font_create（32-bit cbin 内存镜像，与设备
 *         LvglCBinFont 同一装载面；cbin_font.c 编入设备构建）。
 *   host（CONFIG_IDF_TARGET_LINUX）  = 编译期指针宽度感知解码器：把 32-bit cbin
 *         内存镜像反序列化为 host 64-bit lv_font_fmt_txt_dsc_t/lv_font_t。host 构建
 *         不含 cbin_font.c——cbin_font_create 按 32-bit 布局读 dsc 槽（@24），64-bit
 *         host 的 dsc 槽在 @40，直调必崩（builder step-0 红基线段错误实测）。
 *         解码器与 cbin_font_create 等价性验证归真机轮（FAR-E3，见冻结件头注）。
 *
 * 数据源 = 同一枚 .bin（F-7 意图不变，不同宽度装载面）：设备经 Assets::GetAssetData
 * 取 assets 分区 mmap payload；host 测试注入 fonts_root 直读 reader（同一打包源文件）。
 *
 * cbin 序列化口径（32-bit lv_font_t，little-endian；与 cbin_font.c 逐字段一致）：
 *   lv_font_t     @0 get_glyph_dsc(4) @4 get_glyph_bitmap(4) @8 release_glyph(4)
 *                 @12 line_height(i32) @16 base_line(i32) @20 bitbyte(u8 subpx:2/
 *                     kerning:1/static_bitmap:1) @21 underline_pos @22 underline_thick
 *                 @23 pad @24 dsc(offset,4) @28 fallback(4) @32 user_data(4)  (sizeof 36)
 *   dsc           @0 glyph_bitmap(off,4) @4 glyph_dsc(off,4) @8 cmaps(off,4)
 *                 @12 kern_dsc(off,4) @16 kern_scale(u16) @18 bitfield(u16 cmap_num:9/
 *                     bpp:4/kern_classes:1/bitmap_format:2) @20 stride(u8)        (sizeof 24)
 *   cmap(20B)     @0 range_start(u32) @4 range_length(u16) @6 glyph_id_start(u16)
 *                 @8 unicode_list(off rel cmaps_base,4) @12 glyph_id_ofs_list(off,4)
 *                 @16 list_length(u16) @18 type(u8) @19 pad
 *   所有 offset 相对 D = bin_start + font.dsc（与 cbin_font_create 的 bin_addr 重生基一致）。
 *   glyph_dsc 数组与 glyph_bitmap 为定宽数据（无指针），直接指向 bin 缓冲（不拷贝）。
 */
#include "kidpal_font.h"

#include "sdkconfig.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* 编译内 14px 兜底（host 由 font_puhui_14_1.c 编入；设备由 xiaozhi-fonts 提供）。 */
LV_FONT_DECLARE(font_puhui_14_1);

/* 合法 cbin 最小完整性下界（[K2] 守卫同界）。 */
#define KIDPAL_FONT_MIN_CBIN_BYTES ((size_t)32)

#if defined(CONFIG_IDF_TARGET_LINUX)

/* ===========================================================================
 * host 装载面：32-bit cbin 镜像 -> 64-bit lv_font_t/lv_font_fmt_txt_dsc_t 宽度感知解码
 * =========================================================================== */
#include <stdlib.h>

static uint32_t kf_rd_u32(const uint8_t *b, size_t o)
{
    uint32_t v;
    memcpy(&v, b + o, sizeof(v)); /* host 与 cbin 同 little-endian */
    return v;
}

static uint16_t kf_rd_u16(const uint8_t *b, size_t o)
{
    uint16_t v;
    memcpy(&v, b + o, sizeof(v));
    return v;
}

static int32_t kf_rd_i32(const uint8_t *b, size_t o)
{
    int32_t v;
    memcpy(&v, b + o, sizeof(v));
    return v;
}

/* 32-bit cbin -> host 64-bit 句柄。返回 NULL = 载荷不可解析（越界），调用方回退 14px。
 * 分配 = font + dsc + cmaps 数组（小结构体）；字形位图/描述直接指向 data（缓冲存活期
 * 即句柄存活期，与 cbin_font_create 一致）。kern 不做解码（host 行为钉用 letter_next=0，
 * 不触发 kerning；等价性归真机轮）——dsc->kern_dsc 置 NULL、font->kerning 保留原值。 */
static const lv_font_t *kf_decode_host(const uint8_t *data, size_t size)
{
    const uint8_t *B = data;
    uint32_t dsc_off;
    const uint8_t *D;
    uint16_t bf;
    int cmap_num, bpp, bitmap_format;
    lv_font_fmt_txt_dsc_t *dsc;
    lv_font_t *font;
    uint8_t bitbyte;

    if (size < 36) {
        return NULL;
    }
    dsc_off = kf_rd_u32(B, 24);
    if (dsc_off > size || (size - dsc_off) < 24) {
        return NULL; /* dsc 结构越界 */
    }
    D = B + dsc_off;

    dsc = (lv_font_fmt_txt_dsc_t *)malloc(sizeof(*dsc));
    if (dsc == NULL) {
        return NULL;
    }
    font = (lv_font_t *)malloc(sizeof(*font));
    if (font == NULL) {
        free(dsc);
        return NULL;
    }
    memset(font, 0, sizeof(*font));
    memset(dsc, 0, sizeof(*dsc));

    bf = kf_rd_u16(D, 18);
    cmap_num = (int)(bf & 0x1FFu);          /* :9 */
    bpp = (int)((bf >> 9) & 0xFu);          /* :4 */
    bitmap_format = (int)((bf >> 14) & 0x3u); /* :2 */

    /* glyph_bitmap / glyph_dsc 偏移相对 D（与 cbin_font_create 的 bin_addr 重生基一致）。 */
    dsc->glyph_bitmap = (const uint8_t *)(D + kf_rd_u32(D, 0));
    dsc->glyph_dsc = (const lv_font_fmt_txt_glyph_dsc_t *)(D + kf_rd_u32(D, 4));
    dsc->kern_dsc = NULL;
    dsc->kern_scale = kf_rd_u16(D, 16);
    dsc->cmap_num = (uint16_t)cmap_num;
    dsc->bpp = (uint8_t)bpp;
    dsc->kern_classes = 0;
    dsc->bitmap_format = (uint8_t)bitmap_format;
    dsc->stride = *(const uint8_t *)(D + 20);

    if (cmap_num > 0) {
        const uint8_t *cmb = D + kf_rd_u32(D, 8); /* cmaps base */
        lv_font_fmt_txt_cmap_t *cm =
            (lv_font_fmt_txt_cmap_t *)malloc(sizeof(*cm) * (size_t)cmap_num);
        int i;
        if (cm == NULL) {
            free(dsc);
            free(font);
            return NULL;
        }
        for (i = 0; i < cmap_num; i++) {
            const uint8_t *r = cmb + (size_t)i * 20;
            uint32_t ul = kf_rd_u32(r, 8);
            uint32_t go = kf_rd_u32(r, 12);
            cm[i].range_start = kf_rd_u32(r, 0);
            cm[i].range_length = kf_rd_u16(r, 4);
            cm[i].glyph_id_start = kf_rd_u16(r, 6);
            /* offset 0 = NULL（与 cbin addr_add 的 if(ptr) 语义一致：0 不加基址）。 */
            cm[i].unicode_list = (ul != 0) ? (const uint16_t *)(cmb + ul) : NULL;
            cm[i].glyph_id_ofs_list = (go != 0) ? (const void *)(cmb + go) : NULL;
            cm[i].list_length = kf_rd_u16(r, 16);
            cm[i].type = (lv_font_fmt_txt_cmap_type_t)(*(const uint8_t *)(r + 18));
        }
        dsc->cmaps = cm;
    } else {
        dsc->cmaps = NULL;
    }

    font->get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt;
    font->get_glyph_bitmap = lv_font_get_bitmap_fmt_txt;
    font->release_glyph = NULL;
    font->line_height = kf_rd_i32(B, 12);
    font->base_line = kf_rd_i32(B, 16);
    bitbyte = *(const uint8_t *)(B + 20);
    font->subpx = bitbyte & 0x3u;
    font->kerning = (bitbyte >> 2) & 0x1u;
    font->static_bitmap = (bitbyte >> 3) & 0x1u;
    font->underline_position = (int8_t)(*(const uint8_t *)(B + 21));
    font->underline_thickness = (int8_t)(*(const uint8_t *)(B + 22));
    font->dsc = dsc;
    font->fallback = NULL;
    font->user_data = NULL;

    return font;
}

#else /* 设备面：cbin_font_create + Assets::GetAssetData */

#include <cbin_font.h>

#include <string>

#include "assets.h"

/* [K1] 设备数据源强符号：Assets::GetAssetData 薄包装（返回 'ZZ' 之后的裸 payload）。 */
static int kidpal_font_device_reader(const char *name, const uint8_t **data,
                                     size_t *size)
{
    void *ptr = NULL;
    size_t sz = 0;
    if (name == NULL || data == NULL || size == NULL) {
        return -1;
    }
    if (!Assets::GetInstance().GetAssetData(std::string(name), ptr, sz) ||
        ptr == NULL) {
        return -1;
    }
    *data = (const uint8_t *)ptr;
    *size = sz;
    return 0;
}

/* 设备开机挂载入口（assets.cc 在 Assets::Apply 成功后调用）：注入 Assets reader 完成
 * 两枚冻结名装载。LVGL 此时已就绪（Apply 内已用 lv_malloc/LvglCBinFont）。 */
extern "C" void kidpal_font_mount_from_assets(void)
{
    (void)kidpal_font_mount_assets(kidpal_font_device_reader);
}

#endif /* CONFIG_IDF_TARGET_LINUX */

/* ===========================================================================
 * [K2]-[K4] 公共 seam（两目标同一签名；装载面在 from_cbin 内按目标分派）
 * =========================================================================== */
extern "C" {

const lv_font_t *kidpal_font_from_cbin(const uint8_t *data, size_t size)
{
    if (data == NULL || size < KIDPAL_FONT_MIN_CBIN_BYTES) {
        return &font_puhui_14_1; /* 缺文件/短缓冲兜底，绝不返回 NULL */
    }
#if defined(CONFIG_IDF_TARGET_LINUX)
    {
        const lv_font_t *f = kf_decode_host(data, size);
        return (f != NULL) ? f : &font_puhui_14_1; /* 解析失败 -> 14px（不崩） */
    }
#else
    {
        lv_font_t *f = cbin_font_create((uint8_t *)data);
        return (f != NULL) ? f : &font_puhui_14_1;
    }
#endif
}

static const lv_font_t *s_kidpal_font_body = &font_puhui_14_1;
static const lv_font_t *s_kidpal_font_title = &font_puhui_14_1;

int kidpal_font_mount_assets(kidpal_font_asset_reader_t reader)
{
    const lv_font_t *body = &font_puhui_14_1;
    const lv_font_t *title = &font_puhui_14_1;
    const uint8_t *bd = NULL;
    size_t bs = 0;
    const uint8_t *td = NULL;
    size_t ts = 0;

    if (reader != NULL) {
        /* [K5] 冻结资产名（字面直写以便 manifest 结构事实 names_frozen 采集；与
         * kidpal_font.h 的 KIDPAL_FONT_ASSET_* 宏同值）。 */
        if (reader("font_body_16.bin", &bd, &bs) == 0 && bd != NULL) {
            body = kidpal_font_from_cbin(bd, bs);
        }
        if (reader("font_title_20.bin", &td, &ts) == 0 && td != NULL) {
            title = kidpal_font_from_cbin(td, ts);
        }
    }

    /* F-6：20px 缺字回退 16px —— 经 LVGL lv_font_t.fallback 链承载（label 消费通路）。
     * 仅当 title 是真资产句柄（非 14px 全局常量）才挂 fallback，绝不改写 font_puhui_14_1。 */
    if (title != &font_puhui_14_1) {
        ((lv_font_t *)(uintptr_t)title)->fallback = body;
    }

    s_kidpal_font_body = body;
    s_kidpal_font_title = title;
    return 0; /* 缺资产是合法完成态（降级=完成，不黑屏语义 host 可验半） */
}

const lv_font_t *kidpal_font_body(void)
{
    return s_kidpal_font_body;
}

const lv_font_t *kidpal_font_title(void)
{
    return s_kidpal_font_title;
}

} /* extern "C" */
