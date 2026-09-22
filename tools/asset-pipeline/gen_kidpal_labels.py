#!/usr/bin/env python3
"""
gen_kidpal_labels.py — 烘焙两枚首页固定标签 KAP1 1bpp 资产 (TaskPackage
260921-m3-k4-p1-cjk-dynamic-text 追加轮 [S5]; human 拍板 pad-events c4785230)。

资产:
  p1_label_jiexialai.bin  「接下来」大字标签 (卡顶左锚定, [S3] LABEL 带 130x44)
  p1_label_mingzao.bin    「明早」标签 (行 3 左槽, [S3] MINGZAO 槽 44x26;
                          文案是「明早」非「明天」, spec WF-F1-01 v1.21)

KAP1 冻结格式 (与 gen_art_spiffs.mjs / art_assets.c 同源,  逐字一致):
  offset 0 : magic  u32 = 0x3150414B ('KAP1')
  offset 4 : format u16 = 1 (KIDPAL_ART_FMT_1BPP)
  offset 6 : flags  u16 = 0
  offset 8 : w      u16
  offset 10: h      u16
  offset 12: stride u16 = (w+7)/8
  offset 14: reserved u16 = 0
  offset 16: 像素数据 stride*h 字节 (MSB-first, bit=1 为墨)

栅格化沿用既有管线开关 (gen_assets.mjs): ZCOOL KuaiLe 站酷快乐体 (本机 per-user
已装 / fonts/ZCOOLKuaiLe-Regular.ttf), 灰度亮度 < 127 判墨 (LUM_THRESHOLD)。字形
取墨盒 bbox 外扩 2px padding, 一并输出 PNG 供人工核对。

用法 (WSL, 仓根 cwd):
  python3 tools/asset-pipeline/gen_kidpal_labels.py
输出: firmware/main/assets/spiffs_root/{p1_label_jiexialai,p1_label_mingzao}.bin
      + tools/asset-pipeline/report/{jiexialai,mingzao}.png (人工核对)
"""
import os
import struct

from PIL import Image, ImageDraw, ImageFont

LUM_THRESHOLD = 127
FONT_PATH = "/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/tools/asset-pipeline/fonts/ZCOOLKuaiLe-Regular.ttf"
REPO = "/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch"
OUT_DIR = os.path.join(REPO, "firmware", "main", "assets", "spiffs_root")
REPORT_DIR = os.path.join(REPO, "tools", "asset-pipeline", "report")

KAP1_MAGIC = 0x3150414B  # "KAP1"


def render_shade(text, font, pad=0):
    """Render black text on white, return PIL grayscale image with pad."""
    # generous canvas, then crop to ink bbox
    w = max(len(text) * font.size * 2 + 64, 200)
    h = font.size * 2 + 64
    img = Image.new("RGB", (w, h), (255, 255, 255))
    ImageDraw.Draw(img).text((24, font.size // 3), text, fill=(0, 0, 0), font=font)
    gray = img.convert("L")
    px = gray.load()
    xs = []
    ys = []
    for yy in range(h):
        for xx in range(w):
            if px[xx, yy] < LUM_THRESHOLD:
                xs.append(xx)
                ys.append(yy)
    if not xs:
        raise RuntimeError(f"no ink rendered for {text!r}")
    x0, x1 = min(xs), max(xs)
    y0, y1 = min(ys), max(ys)
    x0 = max(0, x0 - pad); y0 = max(0, y0 - pad)
    x1 = min(w - 1, x1 + pad); y1 = min(h - 1, y1 + pad)
    return gray.crop((x0, y0, x1 + 1, y1 + 1))


def make_kap1(gray):
    """gray: PIL grayscale (white bg, ink darker). Returns bytes of KAP1."""
    w, h = gray.size
    stride = (w + 7) // 8
    out = bytearray(16 + stride * h)
    struct.pack_into("<I", out, 0, KAP1_MAGIC)
    struct.pack_into("<H", out, 4, 1)     # format = 1bpp
    struct.pack_into("<H", out, 6, 0)     # flags
    struct.pack_into("<H", out, 8, w)
    struct.pack_into("<H", out, 10, h)
    struct.pack_into("<H", out, 12, stride)
    struct.pack_into("<H", out, 14, 0)    # reserved
    px = gray.load()
    for y in range(h):
        for x in range(w):
            if px[x, y] < LUM_THRESHOLD:
                out[16 + y * stride + (x >> 3)] |= 0x80 >> (x & 7)
    return bytes(out)


def ink_count(gray):
    px = gray.load()
    w, h = gray.size
    return sum(1 for y in range(h) for x in range(w) if px[x, y] < LUM_THRESHOLD)


# PIL's ImageDraw not importable here redundantly; keep at top import.


def main():
    font = ImageFont.truetype(FONT_PATH, 30)
    jobs = [
        ("p1_label_jiexialai", "接下来", font, 2),
        ("p1_label_mingzao", "明早", ImageFont.truetype(FONT_PATH, 20), 2),
    ]
    for name, text, fnt, pad in jobs:
        gray = render_shade(text, fnt, pad)
        kap1 = make_kap1(gray)
        path = os.path.join(OUT_DIR, name + ".bin")
        with open(path, "wb") as f:
            f.write(kap1)
        png = os.path.join(REPORT_DIR, name + ".png")
        gray.save(png)
        print(f"WROTE {name}.bin {gray.size[0]}x{gray.size[1]} {len(kap1)}B "
              f"ink={ink_count(gray)} png={png}")


if __name__ == "__main__":
    main()