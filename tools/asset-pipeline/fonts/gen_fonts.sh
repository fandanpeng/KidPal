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

echo "=== 校验 (非 KAP1 + 尺寸) ==="
for f in font_body.bin font_title_zcool.bin font_num_baloo.bin; do
    sz=$(stat -c%s "$OUT/$f")
    magic=$(head -c4 "$OUT/$f" | od -An -c | tr -d ' \n')
    echo "  $f size=$sz magic=$magic nonKAP1=$( [ "$magic" != 000k1ap1 ] && [ "$magic" != KAP1 ] && echo yes || echo no )"
done