// make_snapshots.mjs — 生成阈值化出样截图（9 宫格拉伸至多种尺寸 / imgfont 数字连排 / 资产拼版），供 human 目测。
// 所有输出仍为阈值化 PNG（唯一值 ⊆ {0,255}，nearest 缩放不产生中间灰度）。
import sharp from 'sharp';
import { readFileSync, existsSync, mkdirSync } from 'fs';

const NINE = 'C:/AI/KidPal/tools/simulator/LvglWindowsSimulator/assets/9slice';
const PNG = 'C:/AI/KidPal/tools/simulator/LvglWindowsSimulator/assets/png';
const SNAP = 'C:/AI/KidPal/tools/asset-pipeline/report/snapshots';
mkdirSync(SNAP, { recursive: true });

// 阈值化 PNG raw 拼接：把多枚阈值化 PNG 拼成一张白色底板 + 阈值化内容（nearest 保持 0/255）
async function tile(rows) {
  // rows: array of rows; each row = [{path,w,h(可选)}]; 每行高度 = 行内最大 h
  const rowH = rows.map(r => Math.max(...r.map(c => c.h)));
  const pad = 4;
  let W = 0;
  for (const row of rows) W = Math.max(W, row.reduce((a, c) => a + c.w, 0) + (row.length + 1) * pad);
  const H = rowH.reduce((a, h) => a + h, 0) + (rows.length + 1) * pad;
  const canvas = Buffer.alloc(W * H * 3, 255);
  let cy = pad;
  for (let ri = 0; ri < rows.length; ri++) {
    const row = rows[ri];
    let cx = pad;
    for (const cell of row) {
      const buf = await sharp(cell.path).resize(cell.w, cell.h, { fit: 'fill', kernel: 'nearest' }).raw().toBuffer({ resolveWithObject: true });
      for (let y = 0; y < buf.info.height; y++) for (let x = 0; x < buf.info.width; x++) {
        const oo = ((cy + y) * W + (cx + x)) * 3;
        const oi = (y * buf.info.width + x) * 3;
        canvas[oo] = buf.data[oi]; canvas[oo + 1] = buf.data[oi + 1]; canvas[oo + 2] = buf.data[oi + 2];
      }
      cx += cell.w + pad;
    }
    cy += rowH[ri] + pad;
  }
  return sharp(canvas, { raw: { width: W, height: H, channels: 3 } }).png();
}

// 9 宫格拉伸：角片固定、边片/中心片按目标尺寸 nearest 拉伸后拼合
async function stretch9(meta, tw, th) {
  const c = meta.corner;
  const target = {
    tl: [0, 0, c, c], top: [c, 0, tw - 2 * c, c], tr: [tw - c, 0, c, c],
    left: [0, c, c, th - 2 * c], center: [c, c, tw - 2 * c, th - 2 * c], right: [tw - c, c, c, th - 2 * c],
    bl: [0, th - c, c, c], bottom: [c, th - c, tw - 2 * c, c], br: [tw - c, th - c, c, c],
  };
  const composites = [];
  for (const s of meta.slices) {
    const [x, y, w, h] = target[s.id];
    const buf = await sharp(`${NINE}/${meta.name}_${s.id}.png`).resize(w, h, { fit: 'fill', kernel: 'nearest' }).png().toBuffer();
    composites.push({ input: buf, left: x, top: y });
  }
  return sharp({ create: { width: tw, height: th, channels: 3, background: { r: 255, g: 255, b: 255 } } }).composite(composites).png().toBuffer();
}

// 1) 框族 9 宫格拉伸至 2 个目标尺寸
for (const f of ['frm_236_236', 'frm_sm_160_120', 'frm_flat_240_96']) {
  const meta = JSON.parse(readFileSync(`${NINE}/${f}.json`, 'utf8'));
  const sizes = f === 'frm_236_236' ? [[320, 320], [400, 240]] : f === 'frm_sm_160_120' ? [[240, 160], [320, 96]] : [[360, 64], [200, 64]];
  for (const [tw, th] of sizes) {
    const buf = await stretch9(meta, tw, th);
    await sharp(buf).png().toFile(`${SNAP}/${f}_stretch_${tw}x${th}.png`);
  }
}

// 2) imgfont 数字连排 0-9 + 冒号
await tile([
  Array.from({ length: 11 }, (_, i) => ({ path: `${PNG}/imgfont_${i < 10 ? 'digit_' + i : 'colon'}.png`, w: 48, h: 64 })),
]).then(p => p.toFile(`${SNAP}/imgfont_row.png`));

// 3) 七姿态拼版
await tile([
  ['daily', 'listen', 'think', 'talk', 'record', 'sleep', 'jump'].map(id => ({ path: `${PNG}/pose_${id}_96.png`, w: 96, h: 96 })),
]).then(p => p.toFile(`${SNAP}/poses_row.png`));

// 4) 14 图标拼版
const iconIds = ['apple', 'book', 'mic', 'drop', 'thermo', 'bell', 'batt', 'mag', 'star', 'cloud', 'penhand', 'note', 'moon', 'wave'];
await tile([
  iconIds.slice(0, 7).map(id => ({ path: `${PNG}/ic_${id}_48.png`, w: 48, h: 48 })),
  iconIds.slice(7).map(id => ({ path: `${PNG}/ic_${id}_48.png`, w: 48, h: id === 'wave' ? 14 : 48 })),
]).then(p => p.toFile(`${SNAP}/icons_grid.png`));

// 5) 框族整框 + 印章 + 网点拼版
await tile([
  [{ path: `${PNG}/frm_236_236.png`, w: 236, h: 236 }, { path: `${PNG}/frm_sm_160_120.png`, w: 160, h: 120 }, { path: `${PNG}/frm_flat_240_96.png`, w: 240, h: 96 }],
  [{ path: `${PNG}/stamp_seal_r8.png`, w: 145, h: 145 }, { path: `${PNG}/font_zcool_30.png`, w: 300, h: 52 }, { path: `${PNG}/d50.png`, w: 4, h: 4 }],
]).then(p => p.toFile(`${SNAP}/frames_stamp_dots.png`));

console.log('snapshots written to', SNAP);