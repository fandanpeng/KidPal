# -*- coding: utf-8 -*-
# Scratch validation (acceptance-author, non-frozen scaffold): independently decode
# font_puhui_14_1.c raw data per official PLAIN bpp=1 continuous-bitstream semantics
# and print ASCII art for the A11 sample codepoints. Validates the algorithm that is
# transcribed into the frozen A11 reference decoder (p1_cjk_name.acceptance.c TEST 10)
# before freezing. Kept beside the red-baseline scaffold summaries as derivation
# evidence; not compiled into any runner.
import re

FONT = r"firmware/managed_components/78__xiaozhi-fonts/src/font_puhui_14_1.c"
src = open(FONT, encoding="utf-8", errors="replace").read()

# ---- glyph_bitmap ----
m = re.search(r"const uint8_t glyph_bitmap\[\] =\s*\{(.*?)\n\};", src, re.S)
body = re.sub(r"/\*.*?\*/", "", m.group(1), flags=re.S)
bitmap = [int(t.strip(), 0) for t in body.split(",") if t.strip()]
print("bitmap bytes:", len(bitmap))

# ---- glyph_dsc entries ----
m = re.search(r"lv_font_fmt_txt_glyph_dsc_t glyph_dsc\[\] = \{(.*?)\n\};", src, re.S)
body = re.sub(r"/\*.*?\*/", "", m.group(1), flags=re.S)
gdsc = []
for e in re.findall(r"\{([^{}]*)\}", body):
    d = dict(re.findall(r"\.(\w+) = (-?\w+)", e))
    gdsc.append({k: int(d[k], 0) for k in d})
print("glyph_dsc entries:", len(gdsc))

# ---- named arrays (unicode_list_N uint16, glyph_id_ofs_list_N uint8) ----
# NOTE: glyph_id_ofs_list values are DECIMAL in this generator's output; parse
# positionally by comma tokens (int(x, 0)), not by 0x-hex regex.
named = {}
for am in re.finditer(r"static const (uint8_t|uint16_t) (\w+)\[\] =\s*\{(.*?)\n\};", src, re.S):
    typ, name, b = am.group(1), am.group(2), re.sub(r"/\*.*?\*/", "", am.group(3), flags=re.S)
    named[name] = [int(t.strip(), 0) for t in b.split(",") if t.strip()]

# ---- cmaps ----
m = re.search(r"static const lv_font_fmt_txt_cmap_t cmaps\[\] =\s*\{(.*?)\n\};", src, re.S)
body = re.sub(r"/\*.*?\*/", "", m.group(1), flags=re.S)
cmaps = []
for e in re.findall(r"\{([^{}]*)\}", body):
    d = dict(re.findall(r"\.(\w+) = ([^,}]+)", e))
    cmaps.append({
        "range_start": int(d["range_start"], 0),
        "range_length": int(d["range_length"], 0),
        "glyph_id_start": int(d["glyph_id_start"], 0),
        "type": d["type"].strip(),
        "unicode_list": d.get("unicode_list", "NULL").strip(),
        "glyph_id_ofs_list": d.get("glyph_id_ofs_list", "NULL").strip(),
        "list_length": int(d["list_length"], 0),
    })
print("cmaps:", len(cmaps))

TINY_F0 = "LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY"
FULL_F0 = "LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL"
TINY_SP = "LV_FONT_FMT_TXT_CMAP_SPARSE_TINY"

def glyph_id(cp):
    for cm in cmaps:
        rcp = cp - cm["range_start"]
        if rcp >= cm["range_length"]:
            continue
        t = cm["type"]
        if t == TINY_F0:
            return cm["glyph_id_start"] + rcp
        if t == FULL_F0:
            arr = named[cm["glyph_id_ofs_list"]]
            if arr[rcp] == 0 and cp != cm["range_start"]:
                continue
            return cm["glyph_id_start"] + arr[rcp]
        if t == TINY_SP:
            ul = named[cm["unicode_list"]]
            for i, v in enumerate(ul):
                if v == rcp:
                    return cm["glyph_id_start"] + i
            return 0
        raise RuntimeError("unhandled cmap type " + t)
    return 0

def decode(cp):
    gid = glyph_id(cp)
    if gid == 0:
        return None
    g = gdsc[gid]
    w, h, bi = g["box_w"], g["box_h"], g["bitmap_index"]
    bits = [(bitmap[bi + (k >> 3)] >> (7 - (k & 7))) & 1 for k in range(w * h)]
    art = "\n".join("".join("#" if bits[y * w + x] else "." for x in range(w)) for y in range(h))
    return {"gid": gid, "w": w, "h": h, "adv_raw": g["adv_w"],
            "adv_px": (g["adv_w"] + 8) >> 4, "art": art}

samples = [("课",0x8BFE),("程",0x7A0B),("钢",0x94A2),("琴",0x7434),("舞",0x821E),
           ("蹈",0x8E48),("体",0x4F53),("育",0x80B2),("春",0x6625),("游",0x6E38),
           ("8",0x38),("W",0x57),(":",0x3A)]
for ch, cp in samples:
    r = decode(cp)
    assert r, "glyph missing: %r" % ch
    print("\n=== U+%04X %s  gid=%d box=%dx%d adv_raw=%d adv_px=%d stride_out_bits=%d" %
          (cp, ch, r["gid"], r["w"], r["h"], r["adv_raw"], r["adv_px"], (r["w"]+7)//8))
    print(r["art"])

# ============ Second font: host Source Han subset (bpp=4 PLAIN) ============
FONT2 = r"firmware/managed_components/lvgl__lvgl/src/font/lv_font_source_han_sans_sc_14_cjk.c"
src2 = open(FONT2, encoding="utf-8", errors="replace").read()
m2 = re.search(r"const uint8_t glyph_bitmap\[\] =\s*\{(.*?)\n\};", src2, re.S)
body2 = re.sub(r"/\*.*?\*/", "", m2.group(1), flags=re.S)
bitmap2 = [int(t.strip(), 0) for t in body2.split(",") if t.strip()]
m2 = re.search(r"lv_font_fmt_txt_glyph_dsc_t glyph_dsc\[\] =\s*\{(.*?)\n\};", src2, re.S)
body2 = re.sub(r"/\*.*?\*/", "", m2.group(1), flags=re.S)
gdsc2 = []
for e in re.findall(r"\{([^{}]*)\}", body2):
    d = dict(re.findall(r"\.(\w+) = (-?\w+)", e))
    gdsc2.append({k: int(d[k], 0) for k in d})
named2 = {}
for am in re.finditer(r"static const (uint8_t|uint16_t) (\w+)\[\] =\s*\{(.*?)\n\};", src2, re.S):
    name, b = am.group(2), re.sub(r"/\*.*?\*/", "", am.group(3), flags=re.S)
    named2[name] = [int(t.strip(), 0) for t in b.split(",") if t.strip()]
m2 = re.search(r"lv_font_fmt_txt_cmap_t cmaps\[\] =\s*\{(.*?)\n\};", src2, re.S)
body2 = re.sub(r"/\*.*?\*/", "", m2.group(1), flags=re.S)
cmaps2 = []
for e in re.findall(r"\{([^{}]*)\}", body2):
    d = dict(re.findall(r"\.(\w+) = ([^,}]+)", e))
    cmaps2.append({
        "range_start": int(d["range_start"], 0),
        "range_length": int(d["range_length"], 0),
        "glyph_id_start": int(d["glyph_id_start"], 0),
        "type": d["type"].strip(),
        "unicode_list": d.get("unicode_list", "NULL").strip(),
        "glyph_id_ofs_list": d.get("glyph_id_ofs_list", "NULL").strip(),
        "list_length": int(d["list_length"], 0),
    })
print("\n[font2 SourceHan] bitmap bytes:", len(bitmap2), "gdsc:", len(gdsc2), "cmaps:", len(cmaps2))

def glyph_id2(cp):
    for cm in cmaps2:
        rcp = cp - cm["range_start"]
        if rcp >= cm["range_length"]:
            continue
        t = cm["type"]
        if t == TINY_F0:
            return cm["glyph_id_start"] + rcp
        if t == FULL_F0:
            arr = named2[cm["glyph_id_ofs_list"]]
            if arr[rcp] == 0 and cp != cm["range_start"]:
                continue
            return cm["glyph_id_start"] + arr[rcp]
        if t == TINY_SP:
            ul = named2[cm["unicode_list"]]
            for i, v in enumerate(ul):
                if v == rcp:
                    return cm["glyph_id_start"] + i
            return 0
        raise RuntimeError("unhandled cmap type " + t)
    return 0

OPA4_GE128 = [n >= 8 for n in range(16)]  # opa4_table[i] >= 128 <=> i >= 8

def decode2(cp):
    gid = glyph_id2(cp)
    if gid == 0:
        return None
    g = gdsc2[gid]
    w, h, bi = g["box_w"], g["box_h"], g["bitmap_index"]
    bits = []
    for k in range(w * h):
        byte = bitmap2[bi + (k >> 1)]
        nib = (byte >> 4) if (k & 1) == 0 else (byte & 0xF)
        bits.append(1 if OPA4_GE128[nib] else 0)
    art = "\n".join("".join("#" if bits[y * w + x] else "." for x in range(w)) for y in range(h))
    return {"gid": gid, "w": w, "h": h, "adv_raw": g["adv_w"],
            "adv_px": (g["adv_w"] + 8) >> 4, "art": art}

samples2 = [("课",0x8BFE),("钢",0x94A2),("琴",0x7434),("8",0x38),("W",0x57),(":",0x3A)]
for ch, cp in samples2:
    r = decode2(cp)
    if r is None:
        print("\n=== U+%04X %s MISSING in SourceHan subset" % (cp, ch))
        continue
    print("\n=== [font2] U+%04X %s  gid=%d box=%dx%d adv_raw=%d adv_px=%d" %
          (cp, ch, r["gid"], r["w"], r["h"], r["adv_raw"], r["adv_px"]))
    print(r["art"])
