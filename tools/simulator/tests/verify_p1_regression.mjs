// verify_p1_regression.mjs
//
// Owner: tools/simulator/tests（dev-loop, TaskPackage 260831-simulator-lvgl-align）。
// Purpose: 对 9.3 模拟器产出的 1bpp 阈值化全屏快照做机判回归, 对应冻结验收
//          test_lvgl_align.acceptance.md 的 §4 A4.1 与 §5 A5.2/A5.3:
//          - A4.1 快照逐像素仅两级 {黑,白}(RGB 三通道同值且 ∈ {0,255})。
//          - A5.2 8 资产逐枚裁剪边界盒, 与源 PNG 对照黑白占比(容差固定 5%)。
//          - A5.3 8 资产预期坐标各有非白内容(非空白)。
// 用法: node tools/simulator/tests/verify_p1_regression.mjs <snapshot.png>
// 依赖: sharp(从 tools/asset-pipeline/node_modules 解析)。
import { createRequire } from 'module';
import { readFileSync } from 'fs';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';

const require = createRequire(fileURLToPath(import.meta.url));
const sharp = require(join(process.cwd(), 'tools', 'asset-pipeline', 'node_modules', 'sharp'));

const SNAPSHOT = process.argv[2];
if (!SNAPSHOT) { console.error('usage: node verify_p1_regression.mjs <snapshot.png>'); process.exit(2); }

const ASSETS_DIR = join(process.cwd(), 'tools', 'simulator', 'LvglWindowsSimulator', 'assets', 'png');
const TOLERANCE = 0.05; // 黑白占比容差, builder 定死 5%

// 8 资产布局坐标 = kidpal_ui.c 的 img_at() 调用坐标(与 acceptance §A5.3 一致)。
const ASSETS = [
  { name: 'time_10_24',      x: 14,  y: 42 },
  { name: 'pose_daily_96',   x: 8,   y: 172 },
  { name: 'ic_batt_32',      x: 330, y: 11 },
  { name: 'doodle_star_26',  x: 228, y: 14 },
  { name: 'doodle_cloud_52', x: 230, y: 30 },
  { name: 'thermo_man',      x: 118, y: 196 },
  { name: 'drop_baby',       x: 216, y: 204 },
  { name: 'ic_bell_13',      x: 372, y: 184 },
];

async function readRaw(path) {
  const { data, info } = await sharp(path).raw().toBuffer({ resolveWithObject: true });
  return { data, info };
}

function pxl(data, channels, x, y, w) {
  const o = (y * w + x) * channels;
  return [data[o], data[o + 1], data[o + 2]];
}

let failures = 0;
function check(ok, msg) {
  console.log(`${ok ? 'PASS' : 'FAIL'}  ${msg}`);
  if (!ok) failures++;
}

async function main() {
  const snap = await readRaw(SNAPSHOT);
  const sw = snap.info.width, sh = snap.info.height, ch = snap.info.channels;
  console.log(`snapshot: ${sw}x${sh} channels=${ch}`);

  // A4.1: distinct pixel value set must be ⊆ {[0,0,0],[255,255,255]}
  const distinct = new Set();
  for (let y = 0; y < sh; y++) {
    for (let x = 0; x < sw; x++) {
      const [r, g, b] = pxl(snap.data, ch, x, y, sw);
      distinct.add(`${r},${g},${b}`);
      if (r !== g || g !== b) distinct.add('__multi__');
    }
  }
  const allowed = new Set(['0,0,0', '255,255,255']);
  const third = [...distinct].filter((v) => !allowed.has(v));
  check(third.length === 0 && distinct.has('0,0,0') && distinct.has('255,255,255'),
    `A4.1 snapshot strictly 2-level {0,255}x3. distinct=${[...distinct].join(' | ')}${third.length ? ' — 3RD-VALUES: ' + third.join(' | ') : ''}`);

  // A5.2 / A5.3: per-asset
  for (const a of ASSETS) {
    const srcPath = join(ASSETS_DIR, `${a.name}.png`);
    let src;
    try { src = await readRaw(srcPath); }
    catch (e) { check(false, `A5.2 ${a.name}: source png missing (${e.message})`); continue; }
    const aw = src.info.width, ah = src.info.height;

    // source black ratio
    let srcBlack = 0, srcWhite = 0;
    for (let y = 0; y < ah; y++) for (let x = 0; x < aw; x++) {
      const [r] = pxl(src.data, src.info.channels, x, y, aw);
      if (r === 0) srcBlack++; else srcWhite++;
    }

    // snapshot crop at (a.x, a.y), clamped to screen
    const x0 = Math.max(0, a.x), y0 = Math.max(0, a.y);
    const x1 = Math.min(sw, a.x + aw), y1 = Math.min(sh, a.y + ah);
    let snapBlack = 0, snapWhite = 0;
    for (let y = y0; y < y1; y++) for (let x = x0; x < x1; x++) {
      const [r] = pxl(snap.data, ch, x, y, sw);
      if (r === 0) snapBlack++; else snapWhite++;
    }

    const srcTotal = srcBlack + srcWhite;
    const snapTotal = snapBlack + snapWhite;
    const srcRatio = srcTotal ? srcBlack / srcTotal : 0;
    const snapRatio = snapTotal ? snapBlack / snapTotal : 0;
    const diff = Math.abs(snapRatio - srcRatio);

    const nonEmpty = snapBlack > 0 && snapWhite > 0;
    check(nonEmpty, `A5.3 ${a.name} @(${a.x},${a.y}) ${aw}x${ah}: non-empty (black=${snapBlack} white=${snapWhite})`);
    check(diff <= TOLERANCE,
      `A5.2 ${a.name}: black-ratio snap=${(snapRatio * 100).toFixed(2)}% src=${(srcRatio * 100).toFixed(2)}% diff=${(diff * 100).toFixed(2)}% (tol=${(TOLERANCE * 100).toFixed(0)}%)`);
  }

  console.log(failures === 0 ? '\nALL CHECKS PASSED' : `\n${failures} CHECK(S) FAILED`);
  process.exit(failures === 0 ? 0 : 1);
}

main().catch((e) => { console.error('harness error:', e.message); process.exit(2); });