#!/usr/bin/env bash
# gen_fonts.sh — KidPal V3 字体生产管线 (WSL 侧, 规避 Windows cmd line 长度限制)。
# TaskPackage 260911-render-pages RP-A4/S0.1: 经 lv_font_conv 抽字表产三枚 SPIFFS
# 非 KAP1 字体 -> firmware/main/assets/fonts_root/ (S0.1 决策 3, 不并 B8 KAP1 闭集)。
#
# 用法:  cd tools/asset-pipeline && wsl -d Ubuntu-24.04 -- bash fonts/gen_fonts.sh
# 依赖:  tools/asset-pipeline/node_modules/lv_font_conv (npm install --no-save)
#         node (WSL 侧可用)
set -euo pipefail
cd "$(dirname "$0")"
DIR="$(pwd)"
# gen_fonts.sh 位于 tools/asset-pipeline/fonts/, 仓库根 = 上三级
ROOT="$(cd "$DIR/../../.." && pwd)"
LVFC_JS="$DIR/../node_modules/lv_font_conv/lv_font_conv.js"

# ---- provenance（裁定 1，2026-10-28 human 双裁 · 261028-m6-k1）------------------
# cbin 格式（32-bit lv_font_t 内存镜像，首 8 字节函数指针槽全零）由 78 的
# lv_font_conv **fork** 的 `--format cbin` 产出；标准 lvgl/lv_font_conv 只有
# dump/bin/lvgl，无 cbin。本产线的新字体（16px/20px）必须用该 fork：
#   来源 = https://github.com/78/lv_font_conv
#   钉版 = HEAD db011302d4f448027cbf1af6d0d47a27d59d22c1
#          （package version 1.5.2 + lib/writers/cbin/lv_font.js 存在为 fork 标记）
#   装法 = cd tools/asset-pipeline && npm install --no-save github:78/lv_font_conv
#   node_modules 已 gitignore，钉版以本注释 + 下方 writer 存在性校验为准（不静默产旧格式）。
if [ ! -f "$DIR/../node_modules/lv_font_conv/lib/writers/cbin/lv_font.js" ]; then
    echo "ERROR: lv_font_conv 缺 cbin writer —— 需 78 的 fork（见本文件 provenance 注释）" >&2
    exit 3
fi
BODY_SRC="$ROOT/firmware/managed_components/lvgl__lvgl/tests/src/test_files/fonts/noto/NotoSansSC-Regular.ttf"
TITLE_SRC="$DIR/ZCOOLKuaiLe-Regular.ttf"
NUM_SRC="$DIR/Baloo2-800.ttf"
OUT="$ROOT/firmware/main/assets/fonts_root"
TMP_BODY="$(mktemp)"

mkdir -p "$OUT"

node -e '
const t = new TextDecoder("gbk");
const set = new Set();
for (let c = 0x21; c <= 0x7e; c++) set.add(String.fromCharCode(c));
for (let hi = 0xa1; hi <= 0xf7; hi++) {
  for (let lo = 0xa1; lo <= 0xfe; lo++) {
    const ch = t.decode(new Uint8Array([hi, lo]));
    if (ch.length === 1 && ch.codePointAt(0) >= 0x20 && ch.codePointAt(0) !== 0xfffd) set.add(ch);
  }
}
const s = [...set].join("");
console.error("body charset size:", s.length, "has 猬/猾/狸:", s.includes("猬"), s.includes("猾"), s.includes("狸"));
process.stdout.write(s);
' > "$TMP_BODY"

BODY_SYMBOLS="$(cat "$TMP_BODY")"
rm -f "$TMP_BODY"

echo "=== 1/3 font_body.bin (14px 1bpp GB2312 全集) ==="
node "$LVFC_JS" --font "$BODY_SRC" --size 14 --bpp 1 --format bin --no-compress \
    --symbols "$BODY_SYMBOLS" -o "$OUT/font_body.bin"

echo "=== 2/3 font_title_zcool.bin (30px 1bpp 标题子集) ==="
# 字表 V5 对齐（2026-09-22）：V3 基础表 + 查字窗（部首日/组词/写完啦共笔）+ 删除确认（删掉这篇？）
# + 提醒（记得带画具！）+ 成就帧（星徽章颗入）+ 拼音 xīng + 补数字 345789 + ！？·
# 特殊字形 °/✔ 按 human 裁定暂不做。注意：reader 标题与提醒正文是动态文案，静态子集覆盖不了，
# 动态 ZCOOL 渲染路径（fallback body / 逐字形 blit）归 dev-loop 包定案。
node "$LVFC_JS" --font "$TITLE_SRC" --size 30 --bpp 1 --format bin \
    --symbols '我的故事今天的课明天的课今天下午查生字小伴讲完啦知道了不等了打断叫醒它说完啦还没有课表数据问我吧开录新说的要了有一颗已入册快来一起看书吧2026年月日时分第屏共翻页真机对照武英数语钢琴课美术班方块球游泳训练删掉这篇？记得带画具！星徽章颗入部首日组词笔xīng345789！？·' \
    -o "$OUT/font_title_zcool.bin"

echo "=== 3/3 font_num_baloo.bin (44px 1bpp 数字子集) ==="
node "$LVFC_JS" --font "$NUM_SRC" --size 44 --bpp 1 --format bin \
    --symbols '0123456789:.:' \
    -o "$OUT/font_num_baloo.bin"

echo "=== 4/5 font_body_16.bin (16px 1bpp 全量 CJK+Latin cbin) ==="
# 裁定 1 扩产（261028-m6-k1）。字表 = ASCII 可打印 + Latin-1/ExtA/ExtB(0xA0-0x24F)
# + GBK(0xA1-0xF7，含 GB2312 二级)。Latin 扩展参与 line_height（字形包围盒并集）
# 校准——取到 16px line_height=23（冻结 FAS-A2 标定 16px->23）。含 GB2312 二级
# 猬/狸/猾 全量 canary。**--format cbin（78 fork）** 产出设备可 cbin 装载的镜像。
mk_symbols() { # $1 = GBK hi 上界（0xf7=全量含二级；0xd7=符号区+一级，排除二级）
    node -e '
const t = new TextDecoder("gbk");
const set = new Set();
const hiMax = Number(process.argv[1]);
for (let c = 0x21; c <= 0x7e; c++) set.add(String.fromCharCode(c));
for (let c = 0xa0; c <= 0x24f; c++) set.add(String.fromCharCode(c));
for (let hi = 0xa1; hi <= hiMax; hi++) {
  for (let lo = 0xa1; lo <= 0xfe; lo++) {
    const ch = t.decode(new Uint8Array([hi, lo]));
    if (ch.length === 1 && ch.codePointAt(0) >= 0x20 && ch.codePointAt(0) !== 0xfffd) set.add(ch);
  }
}
if (process.argv[2] === "1") {
  // 冻结 FAS-A2 body 二级 canary 码点：0x732C 猬 / 0x72F8 狸 / 0x732D / 0x733E 猾。
  // 0x732D = 真实 CJK 统一表意（Noto 有字形）但不在 GBK 双字节表；0x733E 才是常见
  // 意义的「猾」（冻结件注释把 0x732D 标作猾 = 笔误，已回报 acceptance-author）。显式
  // 并入 -> body 真实覆盖这些码点（超集，非子集化）。
  for (const cp of [0x732c, 0x72f8, 0x732d, 0x733e]) set.add(String.fromCodePoint(cp));
}
process.stdout.write([...set].join(""));
' "$1" "${2:-0}"
}
BODY16_SYMBOLS="$(mk_symbols 0xf7 1)"
node "$LVFC_JS" --font "$BODY_SRC" --size 16 --bpp 1 --format cbin --no-compress \
    --symbols "$BODY16_SYMBOLS" -o "$OUT/font_body_16.bin"

echo "=== 5/5 font_title_20.bin (20px 1bpp 阅读标题 CJK 一级+符号+Latin cbin) ==="
# 20px 阅读标题（F-6/OQ1 全量口径的非静态子集：符号区 + GB2312 一级 3755 + Latin
# 全量；**排除 GB2312 二级 0xD8-0xF7**，使 猬/狸/猾 在 title 缺场而 body 在场——
# 制造 F-6「20px 缺字回退 16px」的自然缺口，供 FAS-A5 行为钉；20px line_height=28
# 对应冻结 FAS-A2 标定 20px->28)。
TITLE20_SYMBOLS="$(mk_symbols 0xd7 0)"
node "$LVFC_JS" --font "$BODY_SRC" --size 20 --bpp 1 --format cbin --no-compress \
    --symbols "$TITLE20_SYMBOLS" -o "$OUT/font_title_20.bin"

echo "=== 校验 (非 KAP1 + 尺寸) ==="
for f in font_body.bin font_title_zcool.bin font_num_baloo.bin font_body_16.bin font_title_20.bin; do
    sz=$(stat -c%s "$OUT/$f")
    magic=$(head -c4 "$OUT/$f" | od -An -c | tr -d ' \n')
    echo "  $f size=$sz magic=$magic nonKAP1=$( [ "$magic" != 000k1ap1 ] && [ "$magic" != KAP1 ] && echo yes || echo no )"
done