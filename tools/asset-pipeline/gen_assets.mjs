// KidPal asset rasterizer — 从设计稿 HTML 内嵌 SVG/CSS 抽取元素 → sharp 栅格化 → 阈值化 PNG + LVGL C 数组
// Owner: tools/asset-pipeline（dev-loop, TaskPackage 260831-ui-art-assets 接管全量艺术资产）。
// 用法: node gen_assets.mjs <design.html> <outDir>
// 产物:
//   outDir/assets.c / assets.h (LVGL lv_image_dsc_t 数组, CF=RGB565)
//   outDir/png/*.png                                 (阈值化 PNG, 人工核对/回归对照用)
//   outDir/9slice/<frame>.json + <frame>_<slice>.png (框族 9 宫格切片产物 + 元数据)
// 注意: assets.c / assets.h 为「生成勿手改」——改本脚本重跑即可再生；不得手改已生成文件后提交。
import { readFileSync, writeFileSync, mkdirSync } from 'fs';
import sharp from 'sharp';
import { createHash } from 'crypto';

const [htmlPath, outDir] = process.argv.slice(2);
if (!htmlPath || !outDir) { console.error('usage: node gen_assets.mjs <design.html> <outDir>'); process.exit(1); }
const html = readFileSync(htmlPath, 'utf8');
mkdirSync(outDir + '/png', { recursive: true });
mkdirSync(outDir + '/9slice', { recursive: true });

// 亮度阈值单一源(与真机/模拟器阈值一致性): 值 = 127, 与
//   firmware/sdkconfig            CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD=127
//   模拟器 lv_conf.h              LV_DRAW_SW_I1_LUM_THRESHOLD     127
//   模拟器快照工具 kidpal_snapshot.c   KIDPAL_SNAPSHOT_LUM_THRESHOLD = LV_DRAW_SW_I1_LUM_THRESHOLD
// 同值。luminance < LUM_THRESHOLD → 黑(墨水), 否则白。三处不得漂移(见 .claude 验收 B8)。
// 说明: 阈值取 128 会与固件 127 差一档（127/128 边界像素反相）；真机 flush 用「大于阈值」判白，
// 本脚本统一「luma < LUM_THRESHOLD(127)」判黑。
const LUM_THRESHOLD = 127;

// ── 1. 抽取 <defs> 共享件 + 各 symbol ──
const defsM = html.match(/<defs>([\s\S]*?)<\/defs>/);
if (!defsM) { console.error('no <defs> found'); process.exit(1); }
const defs = defsM[1];
const symbols = {};
for (const m of html.matchAll(/<symbol id="([^"]+)" viewBox="([^"]+)">([\s\S]*?)<\/symbol>/g)) {
  symbols[m[1]] = { viewBox: m[2], body: m[3] };
}
console.log('symbols found:', Object.keys(symbols).length);

// ── 2. 内联 P1 页面独有 SVG（温度计小人/水滴宝宝，不在 symbol 库里）──
const inlineSvgs = [];
for (const m of html.matchAll(/<svg class="(tilt\d+r?)" style="left:(\d+)px;top:(\d+)px;width:(\d+)px(?:;height:(\d+)px)?" viewBox="([^"]+)"><g>([\s\S]*?)<\/g><\/svg>/g)) {
  inlineSvgs.push({ tilt: m[1], x: +m[2], y: +m[3], w: +m[4], h: m[5] ? +m[5] : null, viewBox: m[6], body: m[7] });
}
const inlineNamed = { '64 96': 'thermo-man', '56 88': 'drop-baby' };
// V4 起画布退场的 V2/V3 legacy 内嵌件——现固件 home_p1 与 simulator kidpal_ui.c 仍在消费，
// 母版自 V3 固化于此（不再依赖设计稿携带），rotate 沿用当年 OQ-1(a) 烘斜口径。
const LEGACY_INLINE = {
  thermo_man: { viewBox: '0 0 64 96', w: 64, h: 96, rotate: -1, body: '<line x1="24" y1="34" x2="15" y2="26" stroke="#000" stroke-width="4" stroke-linecap="round"/><line x1="40" y1="34" x2="49" y2="26" stroke="#000" stroke-width="4" stroke-linecap="round"/><circle cx="14" cy="25" r="3.2" fill="#000"/><circle cx="50" cy="25" r="3.2" fill="#000"/><rect x="24" y="8" width="16" height="56" rx="8" fill="#fff" stroke="#000" stroke-width="4"/><circle cx="32" cy="70" r="12" fill="#fff" stroke="#000" stroke-width="4"/><line x1="32" y1="68" x2="32" y2="16" stroke="#000" stroke-width="7" stroke-linecap="round"/><path d="M38 14 h4 M38 20 h4 M38 26 h4" stroke="#000" stroke-width="3"/><circle cx="28.5" cy="68" r="1.8" fill="#000"/><circle cx="35.5" cy="68" r="1.8" fill="#000"/><path d="M28.5 72 q3.5 3 7 0" fill="none" stroke="#000" stroke-width="2.5" stroke-linecap="round"/><rect x="21" y="86" width="9" height="7" rx="3.5" fill="#000"/><rect x="34" y="86" width="9" height="7" rx="3.5" fill="#000"/>' },
  drop_baby: { viewBox: '0 0 56 88', w: 56, h: 88, rotate: 2, body: '<path d="M28 6 C37 19 44 27 44 37 A16 16 0 1 1 12 37 C12 27 19 19 28 6 Z" fill="#fff" stroke="#000" stroke-width="4"/><circle cx="22.5" cy="38" r="2.2" fill="#000"/><circle cx="33.5" cy="38" r="2.2" fill="#000"/><path d="M23 43.5 q5 4.5 10 0" fill="none" stroke="#000" stroke-width="3" stroke-linecap="round"/><use href="#cheekdots" transform="translate(16 46)"/><use href="#cheekdots" transform="translate(40 46)"/><line x1="7" y1="48" x2="1" y2="44" stroke="#000" stroke-width="3.5" stroke-linecap="round"/><line x1="49" y1="48" x2="55" y2="44" stroke="#000" stroke-width="3.5" stroke-linecap="round"/><circle cx="0.5" cy="43" r="2.6" fill="#000"/><circle cx="55.5" cy="43" r="2.6" fill="#000"/><rect x="19" y="79" width="8" height="6" rx="3" fill="#000"/><rect x="30" y="79" width="8" height="6" rx="3" fill="#000"/>' },
};

function wrapSvg(viewBox, body, extraDefs = true) {
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="${viewBox}">${extraDefs ? `<defs>${defs}</defs>` : ''}${body}</svg>`;
}

// svgUse(): symbol 内容内联（不嵌套 <use href="#id">，libvips 对 use→symbol→use→defs 的间接层解析不稳）。
// defs 随文档注入；symbol body 内部对 #mp-head 等 defs 的 <use> 引用落在同一文档内、可解析。
function svgUse(id) {
  const s = symbols[id];
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="${s.viewBox}"><defs>${defs}</defs>${s.body}</svg>`;
}

// ── 3. 栅格化核心：rotate 先于 flatten(防透明误判黑) + 1bpp 阈值 + RGB565 打包 ──
// flatten 在 greyscale/阈值判定之前（保留既有修复 E2-2）：SVG 无底色即透明——不 flatten 时透明像素 RGB=0
// 会被阈值误判为黑(整版全黑)。
async function renderGreyscale(svgStr, w, h, rotateDeg = 0) {
  let img = sharp(Buffer.from(svgStr), { density: 300 });
  img = img.resize(w, h, { fit: 'fill' });
  if (rotateDeg) {
    // OQ-1(a): 倾斜烘进资产——离线旋转后阈值化,锯齿离线可见可控（rotate 在 flatten 之前）。
    img = img.rotate(rotateDeg, { background: { r: 255, g: 255, b: 255, alpha: 1 } });
  }
  const { data, info } = await img.flatten({ background: '#ffffff' }).greyscale().raw().toBuffer({ resolveWithObject: true });
  return { data, w: info.width, h: info.height };
}

// 灰度 raw → RGB565(2字节/像素: 白=0xffff 黑=0x0000 低字节在前) + 阈值化 PNG 三通道数据
function packRgb565(data, w, h) {
  const stride = w * 2;
  const packed = Buffer.alloc(stride * h);
  const pngData = Buffer.alloc(w * h * 3);
  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      const black = data[y * w + x] < LUM_THRESHOLD;
      const o2 = y * stride + x * 2;
      packed[o2] = black ? 0x00 : 0xff;
      packed[o2 + 1] = black ? 0x00 : 0xff;
      const c = black ? 0 : 255;
      const o3 = (y * w + x) * 3; pngData[o3] = c; pngData[o3 + 1] = c; pngData[o3 + 2] = c;
    }
  }
  return { packed, stride, pngData };
}

async function writeThresholdPng(path, pngData, w, h) {
  await sharp(pngData, { raw: { width: w, height: h, channels: 3 } }).png().toFile(path);
}

// ── 4. 手绘框族：CSS 八值 border-radius → SVG 父本 ──
// 母版 = 设计稿 CSS class（非 symbol）：.frm / .frm-sm / .frm-flat。从八值不对称圆角构造 SVG path。
const HAND_R = { tl: [255, 15], tr: [15, 225], br: [225, 15], bl: [15, 255] };      // --hand-r
const HAND_R_SM = { tl: [125, 14], tr: [12, 135], br: [145, 12], bl: [14, 125] };   // --hand-r-sm

// CSS border-radius 比例缩放（radii 之和超 box 尺寸时整体按 f 缩小）
function scaleRadii(W, H, r) {
  const f = Math.min(
    W / (r.tl[0] + r.tr[0]), W / (r.bl[0] + r.br[0]),
    H / (r.tl[1] + r.bl[1]), H / (r.tr[1] + r.br[1]), 1);
  return { tl: [r.tl[0] * f, r.tl[1] * f], tr: [r.tr[0] * f, r.tr[1] * f], br: [r.br[0] * f, r.br[1] * f], bl: [r.bl[0] * f, r.bl[1] * f] };
}
// 整框 path（顺时针，四角椭圆弧 sweep=1）
function framePath(W, H, r, inset) {
  const x0 = inset, y0 = inset, x1 = W - inset, y1 = H - inset;
  const { tl, tr, br, bl } = r;
  return `M ${x0 + tl[0]} ${y0}` +
    ` L ${x1 - tr[0]} ${y0}` +
    ` A ${tr[0]} ${tr[1]} 0 0 1 ${x1} ${y0 + tr[1]}` +
    ` L ${x1} ${y1 - br[1]}` +
    ` A ${br[0]} ${br[1]} 0 0 1 ${x1 - br[0]} ${y1}` +
    ` L ${x0 + bl[0]} ${y1}` +
    ` A ${bl[0]} ${bl[1]} 0 0 1 ${x0} ${y1 - bl[1]}` +
    ` L ${x0} ${y0 + tl[1]}` +
    ` A ${tl[0]} ${tl[1]} 0 0 1 ${x0 + tl[0]} ${y0}` +
    ` Z`;
}
// .frm-flat 仅底 3px 实线 + 半径：底边两端的圆角上卷（近似，sweep=0 外凸）
function flatPath(W, H, r, inset) {
  const x0 = inset, y1 = H - inset;
  const { br, bl } = r;
  return `M ${x0} ${y1 - bl[1]}` +
    ` A ${bl[0]} ${bl[1]} 0 0 0 ${x0 + bl[0]} ${y1}` +
    ` L ${W - inset - br[0]} ${y1}` +
    ` A ${br[0]} ${br[1]} 0 0 0 ${W - inset} ${y1 - br[1]}`;
}
function frameSvg(W, H, radiiSpec, flat) {
  const r = scaleRadii(W, H, radiiSpec);
  const inset = 2; // 3px stroke 中心内缩 1.5，取 2 留滑边
  const d = flat ? flatPath(W, H, r, inset) : framePath(W, H, r, inset);
  const cap = flat ? ' stroke-linecap="round"' : '';
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${W} ${H}"><path d="${d}" fill="none" stroke="#000" stroke-width="3"${cap}/></svg>`;
}

// ── 5. 三档网点：CSS data-uri SVG → 4×4 平铺贴图（4px 格防缩糊）──
// 母版 = CSS 内嵌 data:image/svg+xml 的 <circle cx=2 cy=2 r=1.6/1.13/0.79>（非 symbol）。
// 4×4 直接栅格化会因中心采样把 d50/d25 都塌成 2×2、d12 塌成空白；改为超采样取覆盖 + Bayer 4×4 有序抖动，
// 得到忠实的 50%/25%/12% 网屏占比（每枚仍严格 4×4）。
const BAYER4 = [[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]];
const DOT_EXTRACT = {
  d50: /\.d50\{background-image:url\("data:image\/svg\+xml,([^"]+)"\)\}/,
  d25: /\.d25\{background-image:url\("data:image\/svg\+xml,([^"]+)"\)\}/,
  d12: /\.d12\{background-image:url\("data:image\/svg\+xml,([^"]+)"\)\}/,
};
function decodeDot(cls) {
  const m = html.match(DOT_EXTRACT[cls]);
  if (!m) return null;
  return decodeURIComponent(m[1]);
}
async function renderDot4x4(cls) {
  const svg = decodeDot(cls);
  const SS = 64;
  const { data } = await sharp(Buffer.from(svg), { density: 300 }).resize(SS, SS, { fit: 'fill' }).flatten({ background: '#ffffff' }).greyscale().raw().toBuffer({ resolveWithObject: true });
  const block = SS / 4;
  const cov = new Array(16);
  for (let by = 0; by < 4; by++) for (let bx = 0; bx < 4; bx++) {
    let b = 0;
    for (let yy = 0; yy < block; yy++) for (let xx = 0; xx < block; xx++) if (data[(by * block + yy) * SS + (bx * block + xx)] < LUM_THRESHOLD) b++;
    cov[by * 4 + bx] = b / (block * block);
  }
  const w = 4, h = 4;
  const pngData = Buffer.alloc(w * h * 3);
  const packed = Buffer.alloc(w * 2 * h);
  for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
    const black = cov[y * 4 + x] * 16 > BAYER4[y][x];
    const c = black ? 0 : 255;
    const o3 = (y * w + x) * 3; pngData[o3] = c; pngData[o3 + 1] = c; pngData[o3 + 2] = c;
    const o2 = y * (w * 2) + x * 2; packed[o2] = black ? 0 : 255; packed[o2 + 1] = black ? 0 : 255;
  }
  return { packed, stride: w * 2, pngData, w, h };
}

// ── 6. 框族 9 宫格切分（4 角 + 4 边 + 1 中心）──
function make9Slice(name, W, H, raw, c) {
  const slices = [
    { id: 'tl', x: 0, y: 0, w: c, h: c, stretch: false },
    { id: 'top', x: c, y: 0, w: W - 2 * c, h: c, stretch: 'x' },
    { id: 'tr', x: W - c, y: 0, w: c, h: c, stretch: false },
    { id: 'left', x: 0, y: c, w: c, h: H - 2 * c, stretch: 'y' },
    { id: 'center', x: c, y: c, w: W - 2 * c, h: H - 2 * c, stretch: 'both' },
    { id: 'right', x: W - c, y: c, w: c, h: H - 2 * c, stretch: 'y' },
    { id: 'bl', x: 0, y: H - c, w: c, h: c, stretch: false },
    { id: 'bottom', x: c, y: H - c, w: W - 2 * c, h: c, stretch: 'x' },
    { id: 'br', x: W - c, y: H - c, w: c, h: c, stretch: false },
  ];
  const files = [];
  for (const s of slices) {
    const buf = Buffer.alloc(s.w * s.h * 3);
    for (let y = 0; y < s.h; y++) for (let x = 0; x < s.w; x++) {
      const black = raw[(s.y + y) * W + (s.x + x)] < LUM_THRESHOLD;
      const ccol = black ? 0 : 255;
      const o3 = (y * s.w + x) * 3; buf[o3] = ccol; buf[o3 + 1] = ccol; buf[o3 + 2] = ccol;
    }
    files.push({ id: s.id, buf, w: s.w, h: s.h });
  }
  const meta = { name, base: { w: W, h: H }, corner: c, note: 'stretch=false 角片不变形；边片沿单轴、中心片沿两轴拉伸', slices };
  return { files, meta };
}

// ── 7. 字体：Baloo 2 以 @font-face data-uri 内嵌（真体）；ZCOOL KuaiLe 依赖 per-user 安装（真体）──
// 字体替身坑（E2-3/docs）：librsvg 对 ZCOOL(full 1.5MB) 的 @font-face data-uri 被静默忽略、回落系统字体，
// 对 Baloo 2 的 @font-face data-uri 生效。故 ZCOOL 走「本机已按 per-user 安装 ZCOOL KuaiLe → font-family 按名解析真体」
// 并在下方 doFontVerdict() 对「真体 vs 替身」做出机判并记录；Baloo 走内嵌 data-uri 保证不依赖本机环境。
function tryFont(path) { try { return readFileSync(path); } catch { return null; } }
const zcoolTtf = tryFont(new URL('./fonts/ZCOOLKuaiLe-Regular.ttf', import.meta.url));
const balooTtf = tryFont(new URL('./fonts/Baloo2-800.ttf', import.meta.url));
const balooB64 = balooTtf ? balooTtf.toString('base64') : '';
const BALOO_STYLE = balooB64
  ? `<style>@font-face{font-family:'B2';src:url(data:font/ttf;base64,${balooB64}) format('truetype')}</style>`
  : '';

async function renderGreyscaleHashed(svgStr, w, h) {
  const { data } = await renderGreyscale(svgStr, w, h);
  return { data, w, h, hash: createHash('md5').update(data).digest('hex').slice(0, 12) };
}
// 机判「真体 vs 替身」：与已知回落字体(Microsoft YaHei)渲染结果比对，不同 → 真体生效。
async function doFontVerdict(usedSvg, family) {
  const fallback = usedSvg.replace(/font-family="[^"]*"/, `font-family="Microsoft YaHei"`);
  const a = await renderGreyscaleHashed(usedSvg, 300, 60);
  const b = await renderGreyscaleHashed(fallback, 300, 60);
  const real = a.hash !== b.hash;
  console.log(`FONTVERDICT ${family}: ${real ? 'REAL (differs from Microsoft YaHei fallback)' : 'SUBSTITUTE (== fallback)'}`);
  return real;
}

// ── 8. imgfont 字glyph 定位（等宽箱体 + 基线对齐）──
// 数字 0-9 固定 baseline 底对齐；冒号按 bbox 垂直居中于数字中线。
function bboxOf(bit, w, h) {
  let x0 = w, y0 = h, x1 = -1, y1 = -1, any = false;
  for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) if (bit[y * w + x]) {
    any = true; if (x < x0) x0 = x; if (x > x1) x1 = x; if (y < y0) y0 = y; if (y > y1) y1 = y;
  }
  return any ? { x0, y0, x1, y1, cx: (x0 + x1) / 2, cy: (y0 + y1) / 2, h: y1 - y0 + 1, w: x1 - x0 + 1 } : null;
}
function recenter(raw, W, H, boxW, boxH, mode) {
  const bit = new Array(W * H);
  for (let i = 0; i < W * H; i++) bit[i] = raw[i] < LUM_THRESHOLD;
  const bb = bboxOf(bit, W, H);
  const out = Buffer.alloc(boxW * boxH, 255);
  if (!bb) return out;
  const dx = Math.round(boxW / 2 - bb.cx);
  const dy = mode === 'center' ? Math.round(boxH / 2 - bb.cy) : (boxH - 14 - (bb.y1 + 1)); // baseline: 墨底固定在 boxH-14
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    const nx = x + dx, ny = y + dy;
    if (!bit[y * W + x] || nx < 0 || ny < 0 || nx >= boxW || ny >= boxH) continue;
    out[ny * boxW + nx] = 0;
  }
  return out;
}

// ── 9. 资产清单 ──
const jobs = []; // {name, svg(w=null→dot/raw), w, h, rotate, kind, extra}

// 吉祥物六姿态（pose-sleep v1.14 已裁）：同族同尺寸 96×96，0°（姿态族中性；-1° 倾斜是 P1 摆放层关注点，不烘进姿态族）。
// pose_daily 由旧 98×98(-1°) 重新中立化为 96×96(0°)，保证七姿态逐枚同尺寸防跳位。
for (const id of ['daily', 'listen', 'think', 'talk', 'record', 'jump']) { // pose-sleep v1.14 已裁（磁盘旧 bin 留存给现固件语音页，不再再生）
  jobs.push({ name: `pose_${id}_96`, svg: svgUse(`pose-${id}`), w: 96, h: 96, rotate: 0, kind: 'svg' });
}
// 全图标族 14 枚：「48 盒」统一 48×48；ic-wave 波浪线 48×14 特例（非正方形）。
for (const id of ['apple', 'book', 'mic', 'drop', 'thermo', 'bell', 'batt', 'mag', 'star', 'cloud', 'penhand', 'note', 'moon']) {
  if (!symbols[`ic-${id}`]) continue; // V4 起符号库退场的 legacy 件，走下方 LEGACY_ICONS 兜底
  jobs.push({ name: `ic_${id}_48`, svg: svgUse(`ic-${id}`), w: 48, h: 48, rotate: 0, kind: 'svg' });
}
// legacy 图标三枚（V2/V3 符号库件，V4 换代退场；simulator 与 spiffs 白名单仍引用，母版自 V3 固化于此）
const LEGACY_ICONS = {
  'apple': { viewBox: '0 0 48 48', body: '<circle cx="23" cy="29" r="13.5" fill="#fff" stroke="#000" stroke-width="4"/><path d="M24 16 q-1 -7 -6 -9" fill="none" stroke="#000" stroke-width="4" stroke-linecap="round"/><path d="M25 13 q7 -8 15 -4 q-3 9 -15 6z" fill="#000"/>' },
  'penhand': { viewBox: '0 0 48 48', body: '<line x1="40" y1="6" x2="24" y2="25" stroke="#000" stroke-width="7" stroke-linecap="round"/><path d="M24 25 l-5.5 6.5 l7.5 -1.5 z" fill="#000"/><rect x="12" y="22" width="20" height="17" rx="8" fill="#fff" stroke="#000" stroke-width="4"/><path d="M28.5 27 h5 M28.5 32 h4" stroke="#000" stroke-width="3" stroke-linecap="round"/>' },
  'moon': { viewBox: '0 0 48 48', body: '<path d="M38 30 A16.5 16.5 0 1 1 20 8 a13 13 0 0 0 18 22 Z" fill="#fff" stroke="#000" stroke-width="4" stroke-linejoin="round"/>' },
};
for (const [id, d] of Object.entries(LEGACY_ICONS)) {
  jobs.push({ name: `ic_${id}_48`, svg: `<svg xmlns="http://www.w3.org/2000/svg" viewBox="${d.viewBox}"><defs>${defs}</defs>${d.body}</svg>`, w: 48, h: 48, rotate: 0, kind: 'svg' });
}
jobs.push({ name: 'ic_wave_48', svg: svgUse('ic-wave'), w: 48, h: 14, rotate: 0, kind: 'svg' });
// V5 增补（2026-09-22 human 五裁定）：wifi 双态补集 + 波浪线/星星按 400×300 画布目标尺寸直出（禁 Pod 运行时缩放）。
jobs.push({ name: 'ic_wifi_14', svg: svgUse('ic-wifi'), w: 14, h: 14, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_wifioff_14', svg: svgUse('ic-wifioff'), w: 14, h: 14, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_wave_58', svg: svgUse('ic-wave'), w: 58, h: 17, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_star_22', svg: svgUse('ic-star'), w: 22, h: 22, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_star_18', svg: svgUse('ic-star'), w: 18, h: 18, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_star_38', svg: svgUse('ic-star'), w: 38, h: 38, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_thermo_12', svg: svgUse('ic-thermo'), w: 12, h: 12, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_drop_12', svg: svgUse('ic-drop'), w: 12, h: 12, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_batt_15', svg: svgUse('ic-batt'), w: 15, h: 12, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_cloud_40_t2', svg: svgUse('ic-cloud'), w: 40, h: 40, rotate: -2, kind: 'svg' }); // tilt2 = rotate(-2deg)
jobs.push({ name: 'ic_mic_32', svg: svgUse('ic-mic'), w: 32, h: 32, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_bell_24', svg: svgUse('ic-bell'), w: 24, h: 24, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_bell_22', svg: svgUse('ic-bell'), w: 22, h: 22, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_wave_40', svg: svgUse('ic-wave'), w: 40, h: 12, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_star_20', svg: svgUse('ic-star'), w: 20, h: 20, rotate: 0, kind: 'svg' });
// 录音态声波线（路径自 V5 设计稿固化，206×34 非等比缩放同浏览器渲染口径）
jobs.push({ name: 'rec_wave', svg: `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 48 14"><path d="M2 7 L8 2 L13 12 L18 4 L24 12 L29 3 L35 11 L41 5 L46 9" fill="none" stroke="#000" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"/></svg>`, w: 206, h: 34, rotate: 0, kind: 'svg' });
// P1 反白胶囊标签（V5 口径 12.5px：无字体档，整块烘位图规避；文字 ZCOOL 替 Noto-900，装饰件可接受）
{
  const chip = (w, text) => `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${w} 21"><rect x="0.5" y="0.5" width="${w - 1}" height="20" rx="10" fill="#000"/><text x="${w / 2}" y="15" text-anchor="middle" font-family="ZCOOL KuaiLe" font-size="12.5" fill="#fff">${text}</text></svg>`;
  jobs.push({ name: 'p1_chip_jiexialai', svg: chip(58, '接下来'), w: 58, h: 21, rotate: 0, kind: 'svg' });
  jobs.push({ name: 'p1_chip_mingzao', svg: chip(46, '明早'), w: 46, h: 21, rotate: 0, kind: 'svg' });
}
// P3 语音入口提示条（虚线胶囊原生不可做 + 11.5px 文本无字体档 → 整块烘；文本为静态文案）
{
  const voiceChipSvg = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 252 24"><defs>${defs}</defs>` +
    `<rect x="1.25" y="1.25" width="249.5" height="21.5" rx="11" fill="none" stroke="#000" stroke-width="2.5" stroke-dasharray="4 4"/>` +
    `<g transform="translate(11 5) scale(0.292)">${symbols['ic-mic'].body}</g>` +
    `<text x="34" y="16" font-family="Noto Sans SC, Microsoft YaHei" font-weight="700" font-size="11.5" fill="#000">说「我要讲故事 / 帮我记下来」开录新的</text></svg>`;
  jobs.push({ name: 'voice_chip_p3', svg: voiceChipSvg, w: 252, h: 24, rotate: 0, kind: 'svg' });
// 聆听态声波弧（3 条同心弧自猫嘴向右上散出，56×56；消费方 ic_mic_16 由 gen_voice_chip16.mjs 出，不并入本闭集）
jobs.push({ name: 'arc_listen', svg: svgUse('arc-listen'), w: 56, h: 56, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_mic_22', svg: svgUse('ic-mic'), w: 22, h: 22, rotate: 0, kind: 'svg' });

// V5 静态气泡（框体为八值手绘圆角原生不可做 + 19px 无字体档 → 整块烘：手绘框 + 斜切尾巴 + 文字）
// 几何复刻 .bub CSS：border 3px、padding 4px 11px、line-height 1.75、::after 11×11 rotate(55°) bottom:-9px left:22px
function bubbleSvg(text, fontSize) {
  const w = Math.round(text.length * fontSize + 22 + 6);
  const h = Math.round(fontSize * 1.75 + 8 + 6);
  const r = scaleRadii(w, h, HAND_R_SM);
  const d = framePath(w, h, r, 2);
  const cx = 27.5, cy = h + 3.5; // 尾巴中心（bottom:-9px, left:22px, 11×11）
  const c = Math.cos(55 * Math.PI / 180), sn = Math.sin(55 * Math.PI / 180);
  const rot = (x, y) => [cx + x * c - y * sn, cy + x * sn + y * c];
  const [trx, try_] = rot(5.5, -5.5), [brx, bry] = rot(5.5, 5.5), [blx, bly] = rot(-5.5, 5.5);
  const poly = [[5.5, -5.5], [5.5, 5.5], [-5.5, 5.5], [-5.5, -5.5]].map(([x, y]) => rot(x, y).join(',')).join(' ');
  const baseline = Math.round(h / 2 + fontSize * 0.36);
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${w} ${h + 14}">` +
    `<polygon points="${poly}" fill="#ffffff"/>` +
    `<path d="${d}" fill="none" stroke="#000" stroke-width="3"/>` +
    `<polygon points="${poly}" fill="#ffffff"/>` +
    `<path d="M ${trx.toFixed(2)} ${try_.toFixed(2)} L ${brx.toFixed(2)} ${bry.toFixed(2)} L ${blx.toFixed(2)} ${bly.toFixed(2)}" fill="none" stroke="#000" stroke-width="3" stroke-linecap="round"/>` +
    `<text x="${w / 2}" y="${baseline}" text-anchor="middle" font-family="Noto Sans SC, Microsoft YaHei" font-weight="900" font-size="${fontSize}" fill="#000">${text}</text></svg>`;
}
{
  const BUBBLES = [
    ['bub_listening', '我在听！', 14],          // 聆听态（v5 降 14px 给回显让位；bob 动画运行时做）
    ['bub_recording', '跟着感觉说，我在记！', 14], // 录音态
    ['bub_rec_guide', '需要我帮你记点什么？', 14], // 录音引导态（复用聆听布局换气泡）
  ];
  for (const [name, text, fs] of BUBBLES) {
    const w = Math.round(text.length * fs + 28), h = Math.round(fs * 1.75 + 14) + 14;
    jobs.push({ name, svg: bubbleSvg(text, fs), w, h, rotate: 0, kind: 'svg' });
  }
}
}

// 框族整框（RGB565 参考整图 + 9 宫格切分父本）：frm 大框 / frm-sm 小框 / frm-flat 底框, 角片 24px。
const FRAMES = [
  { name: 'frm_236_236', W: 236, H: 236, spec: HAND_R, flat: false },
  { name: 'frm_sm_160_120', W: 160, H: 120, spec: HAND_R_SM, flat: false },
  { name: 'frm_flat_240_96', W: 240, H: 96, spec: HAND_R_SM, flat: true },
];
for (const f of FRAMES) jobs.push({
  name: f.name, svg: frameSvg(f.W, f.H, f.spec, f.flat), w: f.W, h: f.H, rotate: 0, kind: 'frame', frame: f,
});

// 三档网点 4×4（超采样覆盖 + Bayer 抖动）
for (const cls of ['d50', 'd25', 'd12']) jobs.push({ name: cls, kind: 'dot', cls });

// 成就印章 -8°（128×128 双圈 + 38px 星 + 「星星徽章」；V5 对齐：旧 V2 文案「故事徽章/第 3 颗」弃，编号 N 动态走文本不烘）
{
  const stampSvg = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 128 128">` +
    `<circle cx="64" cy="64" r="62" fill="none" stroke="#000" stroke-width="4"/>` +
    `<circle cx="64" cy="64" r="54" fill="none" stroke="#000" stroke-width="2.5" stroke-dasharray="5 5"/>` +
    `<defs>${defs}</defs>` +
    `<g transform="translate(45 16) scale(0.792)">${symbols['ic-star'].body}</g>` +
    `<text x="64" y="92" text-anchor="middle" font-family="ZCOOL KuaiLe" font-size="17" fill="#000">星星徽章</text>` +
    `</svg>`;
  jobs.push({ name: 'stamp_seal_r8', svg: stampSvg, w: 128, h: 128, rotate: -8, kind: 'svg' });
}

// 站酷快乐体样字（标题 30px 档，「我的故事 该上课啦」）
{
  const zcoolSample = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 300 52"><text x="8" y="40" font-family="ZCOOL KuaiLe" font-size="30" fill="#000">我的故事 该上课啦</text></svg>`;
  jobs.push({ name: 'font_zcool_30', svg: zcoolSample, w: 300, h: 52, rotate: 0, kind: 'zcool', fontVerdictSvg: zcoolSample });
}

// Baloo 2 动态时间 imgfont：数字 0-9 + 冒号，11 枚等宽箱体 48×64，真体(B2 @font-face)。
{
  const IMG_W = 48, IMG_H = 64;
  for (let d = 0; d <= 9; d++) {
    const svg = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${IMG_W} ${IMG_H}">${BALOO_STYLE}<text x="24" y="52" text-anchor="middle" font-family="B2" font-weight="800" font-size="54" fill="#000">${d}</text></svg>`;
    jobs.push({ name: `imgfont_digit_${d}`, svg, w: IMG_W, h: IMG_H, rotate: 0, kind: 'glyph', glyphMode: 'baseline' });
  }
  const colonSvg = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${IMG_W} ${IMG_H}">${BALOO_STYLE}<text x="24" y="52" text-anchor="middle" font-family="B2" font-weight="800" font-size="54" fill="#000">:</text></svg>`;
  jobs.push({ name: 'imgfont_colon', svg: colonSvg, w: IMG_W, h: IMG_H, rotate: 0, kind: 'glyph', glyphMode: 'center', balooSvg: colonSvg });
  // 一笔 imgfont 字体判据样本（数字 8）
  jobs[jobs.length - 1].fontVerdictSvg = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 48 64">${BALOO_STYLE}<text x="24" y="52" text-anchor="middle" font-family="B2" font-weight="800" font-size="54" fill="#000">8</text></svg>`;
}

// P1 既有 8 枚（保留，不删）：pose_daily 已在上方中立化重烘；其余 7 枚原样保留。
jobs.push({ name: 'doodle_star_26', svg: svgUse('ic-star'), w: 26, h: 26, rotate: 0, kind: 'svg' });
jobs.push({ name: 'doodle_cloud_52', svg: svgUse('ic-cloud'), w: 52, h: 52, rotate: 0, kind: 'svg' });
for (const s of inlineSvgs) {
  const key = s.viewBox.split(' ').slice(2).join(' ');
  const nm = inlineNamed[key];
  if (nm) {
    const deg = s.tilt === 'tilt1' ? -1 : s.tilt === 'tilt2r' ? 2 : 0;
    jobs.push({ name: nm.replace(/-/g, '_'), svg: wrapSvg(s.viewBox, `<g>${s.body}</g>`, true), w: s.w, h: s.h, rotate: deg, kind: 'svg' });
  }
}
for (const [nm, s] of Object.entries(LEGACY_INLINE)) {
  jobs.push({ name: nm, svg: wrapSvg(s.viewBox, `<g>${s.body}</g>`, true), w: s.w, h: s.h, rotate: s.rotate, kind: 'svg' });
}
// 大时间字块 10:24（P1 既有，-1° 倾斜整块，保留）
jobs.push({ name: 'time_10_24', svg: `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 244 108"><text x="122" y="86" text-anchor="middle" font-family="'Segoe UI','Arial Rounded MT Bold',sans-serif" font-weight="700" font-size="96" letter-spacing="-3" fill="#000">10  24</text></svg>`, w: 244, h: 108, rotate: -1, kind: 'svg' });
jobs.push({ name: 'ic_batt_32', svg: svgUse('ic-batt'), w: 32, h: 26, rotate: 0, kind: 'svg' });
jobs.push({ name: 'ic_bell_13', svg: svgUse('ic-bell'), w: 13, h: 13, rotate: 0, kind: 'svg' });

// ── 10. 主循环：栅格化 → 阈值 → 打包/切片 → 落盘 ──
const results = [];
let fontVerdictJobs = [];
for (const j of jobs) {
  try {
    if (j.kind === 'dot') {
      const r = await renderDot4x4(j.cls);
      await writeThresholdPng(`${outDir}/png/${j.name}.png`, r.pngData, r.w, r.h);
      results.push({ name: j.name, w: r.w, h: r.h, packed: r.packed, stride: r.stride });
      console.log(`OK ${j.name} ${r.w}x${r.h} (dot 4x4)`);
      continue;
    }
    const g = await renderGreyscale(j.svg, j.w, j.h, j.rotate);
    if (j.kind === 'frame') {
      // 整框 + 9 宫格切片双出
      const pk = packRgb565(g.data, g.w, g.h);
      await writeThresholdPng(`${outDir}/png/${j.name}.png`, pk.pngData, g.w, g.h);
      results.push({ name: j.name, w: g.w, h: g.h, packed: pk.packed, stride: pk.stride });
      const s9 = make9Slice(j.frame.name, g.w, g.h, g.data, 24);
      for (const f of s9.files) await writeThresholdPng(`${outDir}/9slice/${j.frame.name}_${f.id}.png`, f.buf, f.w, f.h);
      writeFileSync(`${outDir}/9slice/${j.frame.name}.json`, JSON.stringify(s9.meta, null, 2));
      console.log(`OK ${j.name} ${g.w}x${g.h} 9slice(9片, corner=24)`);
      continue;
    }
    if (j.kind === 'glyph') {
      const g2 = await renderGreyscale(j.svg, j.w, j.h, 0);
      const recentered = recenter(g2.data, g2.w, g2.h, j.w, j.h, j.glyphMode);
      const pk = packRgb565(recentered, j.w, j.h);
      await writeThresholdPng(`${outDir}/png/${j.name}.png`, pk.pngData, j.w, j.h);
      results.push({ name: j.name, w: j.w, h: j.h, packed: pk.packed, stride: pk.stride });
      console.log(`OK ${j.name} ${j.w}x${j.h} (glyph ${j.glyphMode})`);
      if (j.fontVerdictSvg) fontVerdictJobs.push({ svg: j.fontVerdictSvg, family: 'Baloo 2 (imgfont @font-face)' });
      continue;
    }
    const pk = packRgb565(g.data, g.w, g.h);
    await writeThresholdPng(`${outDir}/png/${j.name}.png`, pk.pngData, g.w, g.h);
    results.push({ name: j.name, w: g.w, h: g.h, packed: pk.packed, stride: pk.stride });
    console.log(`OK ${j.name} ${g.w}x${g.h} rot=${j.rotate} bytes=${pk.packed.length}`);
    if (j.kind === 'zcool' && j.fontVerdictSvg) fontVerdictJobs.push({ svg: j.fontVerdictSvg, family: 'ZCOOL KuaiLe (样字)' });
  } catch (e) {
    console.error(`FAIL ${j.name}: ${e.message}`);
  }
}

// 字体真体/替身机判（不影响产物，只落 verdict 进报告消费）
for (const fv of fontVerdictJobs) {
  try { await doFontVerdict(fv.svg, fv.family); } catch (e) { console.error(`FONTVERDICT FAIL: ${e.message}`); }
}

// ── 11. 生成 C 文件（LVGL 9 image descriptor, LV_COLOR_FORMAT_RGB565）──
let c = `/* Auto-generated by tools/asset-pipeline/gen_assets.mjs — 勿手改 */\n#include "assets.h"\n\n`;
let h = `/* Auto-generated by tools/asset-pipeline/gen_assets.mjs — 勿手改 */\n/* All lv_image_dsc_t below: LV_COLOR_FORMAT_RGB565 (.cf 定义见 assets.c)。 */\n#pragma once\n#include "lvgl/lvgl.h"\n\n`;
for (const r of results) {
  c += `static const uint8_t ${r.name}_map[] = {\n`;
  for (let i = 0; i < r.packed.length; i += 12) {
    c += '  ' + Array.from(r.packed.slice(i, i + 12)).map(b => '0x' + b.toString(16).padStart(2, '0')).join(',') + ',\n';
  }
  c += `};\nconst lv_image_dsc_t ${r.name} = {\n  .header = { .magic = LV_IMAGE_HEADER_MAGIC, .cf = LV_COLOR_FORMAT_RGB565, .flags = 0, .w = ${r.w}, .h = ${r.h}, .stride = ${r.stride} },\n  .data_size = ${r.packed.length},\n  .data = ${r.name}_map,\n};\n\n`;
  h += `extern const lv_image_dsc_t ${r.name}; /* LV_COLOR_FORMAT_RGB565 */\n`;
}
writeFileSync(outDir + '/assets.c', c);
writeFileSync(outDir + '/assets.h', h);
console.log(`wrote ${outDir}/assets.c assets.h (${results.length} assets)`);