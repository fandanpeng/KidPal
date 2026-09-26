#!/usr/bin/env python3
"""
gen_imgfont_letters.py — KidPal 字母字形资产生成（TaskPackage 260915-m1-k1a-key-layer-dispatch，M1-N1b 日期 DD-Mon）

lineage（与既有 imgfont 数字同源）：
  master 字体 = tools/asset-pipeline/fonts/Baloo2-800.ttf —— 与
  gen_fonts.sh（font_num_baloo.bin）及 gen_assets.mjs（imgfont_digit_0..9 / imgfont_colon
  的 Baloo 2 @font-face 内嵌）同一 Baloo 2 800 母版字体。既有数字字形 cell = 48x64
  （imgfont_digit_0.bin w=48 h=64 stride=6，数字 ink ~42px 高、baseline 落 near y=50）。
  本脚本对 A-Z + a-z 全 52 字母各生成一枚 KAP1 1bpp bin，
  落 firmware/main/assets/spiffs_root/imgfont_letter_%c.bin（大写 A-Z）与
  firmware/main/assets/spiffs_root/imgfont_letter_lc_%c.bin（小写 a-z）共 52 枚、
  名字大小写分流。

  NTFS 碰撞缘由（跨端缺陷修复 260915-m1-n1b-p1-home-battery 任务 A）：Windows NTFS
  文件名大小写不敏感——本脚本若对大小写字母用同一名字模板，`imgfont_letter_a.bin`
  与 `imgfont_letter_A.bin` 会写进同一宿主文件（后写的小写覆盖大写），导致现存 bin
  内容实为小写。host fopen 也不区分大小写故截图侥幸正常，但设备 SPIFFS 大小写敏感、
  `imgfont_letter_e.bin`/`_p.bin` 不存在会渲染失败（真机 "Sep" 渲染成 "S"）。故按
  大小写分流编码：大写保持原名字（内容须真大写字形），小写改用 `imgfont_letter_lc_`
  前缀规避碰撞。解码侧 kidpal_art_glyph（art_assets.c）按同样规则分流取文件。

尺寸对齐：cell 同数字 = 48x64、stride=6；baseline 锚 y=50（anchor='ms'）、font-size=65
  使大写字母 ink 高 ~42px 与数字墨高对齐（数字 '8' ink_y[8..49] h=42）。小写字母自然
  比大写矮（x-height ~34px，descender g/p/q/y/j 落到 y~62）——与同日数字混排时视觉
  x-height/cap-height 对齐，符合「DD Mon」正确 typography。

KAP1 冻结格式（见 tools/asset-pipeline/gen_art_spiffs.mjs 头注 + art_assets.c 解码）：
  offset 0 : magic  u32 = 0x3150414B（'K','A','P','1' LE）
  offset 4 : format u16 = 1（KIDPAL_ART_FMT_1BPP）
  offset 6 : flags  u16 = 0
  offset 8 : w      u16 = 48
  offset 10: h      u16 = 64
  offset 12: stride u16 = (w+7)/8 = 6
  offset 14: reserved u16 = 0
  offset 16: 像素数据 stride*h 字节（MSB-first，bit=1 为墨）
阈值化：灰度 < 128 = 墨（bit 1）；>=128 = 纸（bit 0）。

与 gen_art_spiffs.mjs 的共存关系：gen_art_spiffs.mjs 只写其 47 枚闭集白名单件，采用
「上游 assets.c descriptor 只读 + 整文件覆盖写」——它重跑不会删除本脚本产出的 52 枚
字母 bin（不写同名文件），字母 bin 不进入白名单管线。本脚本独立重跑幂等覆盖自己的
52 枚 bin。

用法：cd 仓库根 && python tools/asset-pipeline/gen_imgfont_letters.py
输出：firmware/main/assets/spiffs_root/imgfont_letter_{A..Z}.bin（26 枚大写）
     + firmware/main/assets/spiffs_root/imgfont_letter_lc_{a..z}.bin（26 枚小写）
     ，共 52 枚。
"""
import os
import struct
from PIL import Image, ImageFont, ImageDraw

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
FONT_TTF = os.path.join(REPO, "tools", "asset-pipeline", "fonts", "Baloo2-800.ttf")
OUT_DIR = os.path.join(REPO, "firmware", "main", "assets", "spiffs_root")

CELL_W = 48
CELL_H = 64
BASELINE_Y = 50   # anchor='ms' 的 baseline（与数字字形同 cell 几何）
BASE_SIZE = 65    # 大写字母 ink ~42px 对齐数字

KAP1_MAGIC = 0x3150414B

# '%' 记号（M1-N1b 真机看屏终验：手工基元 p1_draw_pct 两轮修形仍不达标 -> 改走真字形
# 管线）。与字母同母版 Baloo2-800、同 48x64 cell、同 baseline 锚 B，仅文件名固定为
# imgfont_pct.bin（避开字母大小写分流命名；与 gen_art_spiffs.mjs 47 枚闭集白名单不相交，
# 该白名单不含 imgfont_pct，重跑互不覆盖，本脚本独立重跑幂等）。
PCT_OUT = "imgfont_pct.bin"
PCT_CHAR = "%"

# '°' 度符号（2026-09-26 真机对齐轮，canvas-p1 温度 "23°C"）：与字母同母版 Baloo2-800、
# 同 48x64 cell、同 baseline 锚（° 渲染在 cap 高度处自然落上半格）。固定名
# imgfont_degree.bin（非字母、避开大小写分流命名；与 gen_art_spiffs.mjs 闭集白名单
# 不相交，重跑互不覆盖，本脚本独立重跑幂等）。解码侧 art_assets.c kidpal_art_glyph
# 按 (unsigned char)0xB0 分流取本文件。
DEGREE_OUT = "imgfont_degree.bin"
DEGREE_CHAR = "°"

def render_kap1(ch):
    font = ImageFont.truetype(FONT_TTF, BASE_SIZE)
    img = Image.new("L", (CELL_W, CELL_H), 255)
    ImageDraw.Draw(img).text((CELL_W // 2, BASELINE_Y), ch, font=font, fill=0,
                             anchor="ms")
    px = img.load()
    stride = (CELL_W + 7) // 8
    out = bytearray(16 + stride * CELL_H)
    out[0:4] = struct.pack("<I", KAP1_MAGIC)
    struct.pack_into("<HHHHHH", out, 4, 1, 0, CELL_W, CELL_H, stride, 0)
    for y in range(CELL_H):
        for x in range(CELL_W):
            v = px[x, y]
            if v < 128:
                out[16 + y * stride + (x >> 3)] |= (0x80 >> (x & 7))
    return bytes(out)

def main():
    os.makedirs(OUT_DIR, exist_ok=True)
    letters = [chr(c) for c in range(ord("A"), ord("Z") + 1)] + \
              [chr(c) for c in range(ord("a"), ord("z") + 1)]
    written = []
    for ch in letters:
        data = render_kap1(ch)
        # 大小写分流编码（NTFS 大小写不敏感属跨端缺陷——同名模板会使 `_a.bin` 覆盖
        # `_A.bin`；大写保原名字、小写用 `_lc_` 前缀，与 art_assets.c kidpal_art_glyph
        # 解码侧同规）。大写文件名恰是字形本身，必须内容为真大写。
        if "A" <= ch <= "Z":
            name = f"imgfont_letter_{ch}.bin"
        else:
            name = f"imgfont_letter_lc_{ch}.bin"
        with open(os.path.join(OUT_DIR, name), "wb") as f:
            f.write(data)
        written.append(name)
    # '%' 记号（M1-N1b 看屏终验：手工基元不达标改真字形）；同 cell 同基线锚。
    pct = render_kap1(PCT_CHAR)
    with open(os.path.join(OUT_DIR, PCT_OUT), "wb") as f:
        f.write(pct)
    written.append(PCT_OUT)
    # '°' 度符号（2026-09-26 真机对齐轮：温度 °C 后缀）；同 cell 同基线锚。
    deg = render_kap1(DEGREE_CHAR)
    with open(os.path.join(OUT_DIR, DEGREE_OUT), "wb") as f:
        f.write(deg)
    written.append(DEGREE_OUT)
    print(f"WROTE {len(written)} KAP1 letter/pct assets -> {OUT_DIR}")
    for n in written:
        print("  ", n)

if __name__ == "__main__":
    main()