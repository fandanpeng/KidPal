// KidPal asset rasterizer — 从设计稿 HTML 内嵌 SVG 抽取元素 → sharp 栅格化 → 阈值化 PNG + LVGL C 数组
// Owner: tools/asset-pipeline（dev-loop, TaskPackage 260831-simulator-lvgl-align 接管正式化）。
// 用法: node gen_assets.mjs <design.html> <outDir>
// 产物: outDir/assets.c / assets.h (LVGL lv_image_dsc_t 数组, CF=RGB565) + outDir/png/*.png (阈值化,人工核对/回归对照用)。
// 注意: assets.c / assets.h 为「生成勿手改」——改本脚本重跑即可再生；不得手改已生成文件后提交。
import { readFileSync, writeFileSync, mkdirSync } from 'fs';
import sharp from 'sharp';

const [htmlPath, outDir] = process.argv.slice(2);
if (!htmlPath || !outDir) { console.error('usage: node gen_assets.mjs <design.html> <outDir>'); process.exit(1); }
const html = readFileSync(htmlPath, 'utf8');
mkdirSync(outDir + '/png', { recursive: true });

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
console.log('symbols found:', Object.keys(symbols).join(', '));

// ── 2. 内联 P1 页面独有 SVG（温度计小人/水滴宝宝，不在 symbol 库里）──
// 从 canvas-p1 抓带 viewBox 的内联 <svg ...><g>...</g></svg>，按 viewBox 尺寸归类
const inlineSvgs = [];
for (const m of html.matchAll(/<svg class="(tilt\d+r?)" style="left:(\d+)px;top:(\d+)px;width:(\d+)px(?:;height:(\d+)px)?" viewBox="([^"]+)"><g>([\s\S]*?)<\/g><\/svg>/g)) {
  inlineSvgs.push({ tilt: m[1], x: +m[2], y: +m[3], w: +m[4], h: m[5] ? +m[5] : null, viewBox: m[6], body: m[7] });
}
// 命名: 64x96 = 温度计, 56x88 = 水滴宝宝
const inlineNamed = { '64 96': 'thermo-man', '56 88': 'drop-baby' };

function wrapSvg(viewBox, body, extraDefs = true) {
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="${viewBox}">${extraDefs ? `<defs>${defs}</defs>` : ''}${body}</svg>`;
}

// href="#xxx" 在独立 SVG 里需要 xmlns:xlink 或 svg2 href——sharp/libvips 支持 <use href>,
// 但 defs 必须在同一文档内,已随 wrapSvg 注入。cheekdots 等 g id 在 defs 里。

// ── 3. 栅格化 + 1bpp 阈值 + 打包 ──
async function rasterize(name, svgStr, targetW, targetH, rotateDeg = 0) {
  let img = sharp(Buffer.from(svgStr), { density: 300 });
  const meta0 = await img.metadata();
  img = img.resize(targetW, targetH, { fit: 'fill' });
  if (rotateDeg) {
    // OQ-1(a): 倾斜烘进资产——离线旋转后阈值化,锯齿离线可见可控
    img = img.rotate(rotateDeg, { background: { r: 255, g: 255, b: 255, alpha: 1 } });
  }
  const { data, info } = await img.flatten({ background: '#ffffff' }).greyscale().raw().toBuffer({ resolveWithObject: true });
  const w = info.width, h = info.height;
  // 阈值化 + 直出 RGB565(2字节/像素): 白=0xffff 黑=0x0000, 低字节在前
  // flatten: SVG 无底色即透明——不 flatten 时透明像素 RGB=0, 会被阈值误判为黑(整版全黑)
  // 不产 I1 的原因: LVGL 9.5 软件渲染路径直接处理 I1 时绕过调色板(把位图当 alpha/灰度),
  // 仅 bin-decoder 对「文件源」走 decode_indexed_line 才查表——RGB565 双端行为一致,免去 I1 各坑
  // (调色板绕过 / stride 4 对齐 / 数据区头部 8 字节调色板等)。
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
  await sharp(pngData, { raw: { width: w, height: h, channels: 3 } }).png().toFile(`${outDir}/png/${name}.png`);
  return { name, w, h, packed, stride };
}

// ── 4. 资产清单（P1 用）──
// 尺寸按设计稿 canvas-p1 的 style 像素值;倾斜按设计稿 tilt class
const jobs = [];
// 吉祥物日常姿态: 设计稿 96x96 @ left:8 top:172, tilt1 = -1deg
jobs.push(['pose_daily_96', svgUse('pose-daily'), 96, 96, -1]);
// 涂鸦星: 26px @ bob(无旋转), 云 52px
jobs.push(['doodle_star_26', svgUse('ic-star'), 26, 26, 0]);
jobs.push(['doodle_cloud_52', svgUse('ic-cloud'), 52, 52, 0]);
// 温度计小人 40x66 tilt1(-1), 水滴宝宝 30x50 tilt2r(+2)
for (const s of inlineSvgs) {
  const key = s.viewBox.split(' ').slice(2).join(' ');
  const nm = inlineNamed[key];
  if (nm) {
    const deg = s.tilt === 'tilt1' ? -1 : s.tilt === 'tilt2r' ? 2 : 0;
    jobs.push([nm.replace(/-/g, '_'), wrapSvg(s.viewBox, `<g>${s.body}</g>`, true), s.w, s.h, deg]);
  }
}
// 大时间数字 10:24 —— 设计稿 110px 屏上约 96px、letter-spacing -3px、整体 -1° 倾斜。
// 文字栅格化资产: 消除原生标签行距/基线与冒号对齐问题; 数字内容变化时重跑本脚本(固件端计划 imgfont)
jobs.push(['time_10_24',
  `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 244 108"><text x="122" y="86" text-anchor="middle" font-family="'Segoe UI','Arial Rounded MT Bold',sans-serif" font-weight="700" font-size="96" letter-spacing="-3" fill="#000">10  24</text></svg>`,
  244, 108, -1]);
// 电池图标 32x26(设计稿 48 盒缩到 32 宽)
jobs.push(['ic_batt_32', svgUse('ic-batt'), 32, 26, 0]);
// 铃铛小图标 13px(倒计时贴纸用)
jobs.push(['ic_bell_13', svgUse('ic-bell'), 13, 13, 0]);

function svgUse(id) {
  const s = symbols[id];
  // symbol 内容内联（不嵌套 <use href="#id">，libvips 对 use→symbol→use→defs 的间接层解析不稳）
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="${s.viewBox}"><defs>${defs}</defs>${s.body}</svg>`;
}

const results = [];
for (const [name, svg, w, h, deg] of jobs) {
  try {
    const r = await rasterize(name, svg, w, h, deg);
    results.push(r);
    console.log(`OK ${name} ${r.w}x${r.h} rot=${deg} bytes=${r.packed.length}`);
  } catch (e) {
    console.error(`FAIL ${name}: ${e.message}`);
  }
}

// ── 5. 生成 C 文件（LVGL 9 image descriptor, LV_COLOR_FORMAT_RGB565）──
let c = `/* Auto-generated by tools/asset-pipeline/gen_assets.mjs — 勿手改 */\n#include "assets.h"\n\n`;
let h = `/* Auto-generated by tools/asset-pipeline/gen_assets.mjs — 勿手改 */\n/* All lv_image_dsc_t below: LV_COLOR_FORMAT_RGB565 (.cf 定义见 assets.c)。 */\n#pragma once\n#include "lvgl/lvgl.h"\n\n`;
for (const r of results) {
  c += `static const uint8_t ${r.name}_map[] = {\n`;
  for (let i = 0; i < r.packed.length; i += 12) {
    c += '  ' + Array.from(r.packed.slice(i, i + 12)).map(b => '0x' + b.toString(16).padStart(2, '0')).join(',') + ',\n';
  }
  const stride = r.stride;
  c += `};\nconst lv_image_dsc_t ${r.name} = {\n  .header = { .magic = LV_IMAGE_HEADER_MAGIC, .cf = LV_COLOR_FORMAT_RGB565, .flags = 0, .w = ${r.w}, .h = ${r.h}, .stride = ${stride} },\n  .data_size = ${r.packed.length},\n  .data = ${r.name}_map,\n};\n\n`;
  h += `extern const lv_image_dsc_t ${r.name}; /* LV_COLOR_FORMAT_RGB565 */\n`;
}
writeFileSync(outDir + '/assets.c', c);
writeFileSync(outDir + '/assets.h', h);
console.log(`wrote ${outDir}/assets.c assets.h (${results.length} assets)`);
