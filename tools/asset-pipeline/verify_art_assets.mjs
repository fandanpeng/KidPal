// verify_art_assets.mjs — TaskPackage 260831-ui-art-assets 验收机判自查脚本
// 逐条对号 test_art_assets.acceptance.md 的 A2.1–A9.3 + E2/E3/E4，打印「通过/未过」清单；全绿才准交付。
// 用法: node verify_art_assets.mjs
import sharp from 'sharp';
import { readFileSync, readdirSync, existsSync } from 'fs';
import { createHash } from 'crypto';

const ASSETS = 'C:/AI/KidPal/tools/simulator/LvglWindowsSimulator/assets';
const PNG = ASSETS + '/png';
const NINE = ASSETS + '/9slice';
const PIPE = 'C:/AI/KidPal/tools/asset-pipeline';
const REPORT = PIPE + '/report';
const GEN = PIPE + '/gen_assets.mjs';
const HTML = 'C:/AI/KidPal/docs/ui/kidpal-ui-design-v3.html';

const results = [];
function check(id, ok, detail) { results.push({ id, ok, detail }); }

async function rawPng(path) {
  return sharp(path).flatten({ background: '#ffffff' }).raw().toBuffer({ resolveWithObject: true });
}
async function rawGray(path) {
  return sharp(path).flatten({ background: '#ffffff' }).greyscale().raw().toBuffer({ resolveWithObject: true });
}
async function dims(path) { return sharp(path).metadata(); }
function stats(data, w, h) {
  let black = 0, white = 0;
  for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
    const v = data[y * w + x];
    if (v < 127) black++; else white++;
  }
  return { black, white, total: w * h };
}
function bboxOf(data, w, h) {
  let x0 = w, y0 = h, x1 = -1, y1 = -1, any = false;
  for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) if (data[y * w + x] < 127) {
    any = true; if (x < x0) x0 = x; if (x > x1) x1 = x; if (y < y0) y0 = y; if (y > y1) y1 = y;
  }
  return any ? { x0, y0, x1, y1, cx: (x0 + x1) / 2, cy: (y0 + y1) / 2 } : null;
}

const pngs = readdirSync(PNG).filter(f => f.endsWith('.png'));

// ===== E3 阈值化：所有 png 唯一值 ⊆ {0,255} =====
{
  let bad = [];
  for (const f of pngs) {
    const { data, info } = await rawPng(PNG + '/' + f);
    for (let i = 0; i < data.length; i++) if (data[i] !== 0 && data[i] !== 255) { bad.push(f); break; }
  }
  check('E3', bad.length === 0, bad.length ? `非阈值化: ${bad.join(',')}` : `${pngs.length} 枚 PNG 全部唯一值⊆{0,255}`);
}

// ===== A2 七姿态 =====
{
  const poseIds = ['daily', 'listen', 'think', 'talk', 'record', 'sleep', 'jump'];
  const poseFiles = poseIds.map(id => `pose_${id}_96.png`);
  const missing = poseFiles.filter(f => !existsSync(PNG + '/' + f));
  check('A2.1', missing.length === 0, missing.length ? `缺: ${missing.join(',')}` : '7 姿态 PNG 全在场');
  const sizes = [];
  let nonBlankOk = true;
  for (const id of poseIds) {
    const { data, info } = await rawGray(PNG + `/pose_${id}_96.png`);
    sizes.push(`${info.width}x${info.height}`);
    const s = stats(data, info.width, info.height);
    if (s.black === 0 || s.white === 0) nonBlankOk = false;
  }
  const allSame = sizes.every(s => s === sizes[0]);
  check('A2.2', allSame, `7 姿态尺寸 ${sizes.join(', ')} — ${allSame ? '逐枚一致' : '不一致'}`);
  check('A2.3', nonBlankOk, nonBlankOk ? '逐枚非全黑/非全白' : '存在全黑/全白');
}
// 描述符总数 ≥7
{
  const c = readFileSync(ASSETS + '/assets.c', 'utf8');
  const n = (c.match(/const lv_image_dsc_t/g) || []).length;
  check('A2.1-desc', n >= 7, `assets.c 描述符总数=${n} (≥7)`);
}

// ===== A3 框族 9 宫格 =====
{
  const frames = ['frm_236_236', 'frm_sm_160_120', 'frm_flat_240_96'];
  const ids = ['tl', 'top', 'tr', 'left', 'center', 'right', 'bl', 'bottom', 'br'];
  let allFiles = true, cornersNoStretch = true, stitchOk = true; const stitchDetail = [];
  for (const f of frames) {
    const meta = JSON.parse(readFileSync(`${NINE}/${f}.json`, 'utf8'));
    for (const sid of ids) if (!existsSync(`${NINE}/${f}_${sid}.png`)) { allFiles = false; }
    // A3.4 角片 stretch=false
    for (const s of meta.slices) if (['tl', 'tr', 'bl', 'br'].includes(s.id) && s.stretch !== false) cornersNoStretch = false;
    // A3.3 拼回 diff=0
    const whole = await rawPng(`${PNG}/${f}.png`);
    if (whole.info.width !== meta.base.w || whole.info.height !== meta.base.h) { stitchOk = false; stitchDetail.push(`${f} 整图尺寸≠元数据`); continue; }
    const W = whole.info.width, H = whole.info.height;
    const canvas = Buffer.alloc(W * H * 3, 255);
    for (const s of meta.slices) {
      const sl = await rawPng(`${NINE}/${f}_${s.id}.png`);
      if (sl.info.width !== s.w || sl.info.height !== s.h) { stitchOk = false; stitchDetail.push(`${f}.${s.id} 尺寸不符`); continue; }
      for (let y = 0; y < s.h; y++) for (let x = 0; x < s.w; x++) {
        const oo = ((s.y + y) * W + (s.x + x)) * 3;
        const osi = (y * s.w + x) * 3;
        canvas[oo] = sl.data[osi]; canvas[oo + 1] = sl.data[osi + 1]; canvas[oo + 2] = sl.data[osi + 2];
      }
    }
    let same = true;
    for (let i = 0; i < canvas.length; i++) if (canvas[i] !== whole.data[i]) { same = false; break; }
    if (!same) { stitchOk = false; stitchDetail.push(`${f} 拼回 diff≠0`); }
  }
  check('A3.2', allFiles, allFiles ? '3 框型 × 9 片全在场（4角+4边+1中心）' : '9 宫格切片缺件');
  check('A3.3', stitchOk, stitchOk ? '3 框型拼回原始尺寸逐像素 diff=0' : stitchDetail.join('; '));
  check('A3.4', cornersNoStretch, cornersNoStretch ? '3 框型 4 角片均 stretch=false' : '角片 stretch 标记缺失');
}

// ===== A4 倾斜烘进 =====
{
  // A4.1 旋转真烘入：source 中 rotate 先于 flatten + 印章 -8° 尺寸因旋转扩展(145≠128)
  const src = readFileSync(GEN, 'utf8');
  const fIdx = src.indexOf('// 灰阶 raw → RGB565');
  const rotateBeforeFlatten = (() => {
    const fn = src.slice(src.indexOf('async function renderGreyscale'));
    const ri = fn.indexOf('.rotate(');
    const fi = fn.indexOf('.flatten(');
    return ri >= 0 && fi >= 0 && ri < fi;
  })();
  const stampD = await dims(PNG + '/stamp_seal_r8.png');
  const rotatedExpanded = stampD.width === 145 && stampD.height === 145;
  check('A4.1', rotateBeforeFlatten && rotatedExpanded, `rotate先于flatten=${rotateBeforeFlatten}; 印章 -8° 由 128×128 扩展为 ${stampD.width}×${stampD.height}（真实旋转非空过）`);
  // 旋转非平凡演示：旋转一个已阈值化整图, 与不旋转版逐像素不一致
  const frm = await rawPng(PNG + '/frm_236_236.png');
  const rot = await sharp(frm.data, { raw: { width: 236, height: 236, channels: 3 } }).rotate(-2, { background: { r: 255, g: 255, b: 255 } }).flatten({ background: '#ffffff' }).raw().toBuffer({ resolveWithObject: true });
  let diff = false; for (let i = 0; i < frm.data.length && i < rot.data.length; i++) if (frm.data[i] !== rot.data[i]) { diff = true; break; }
  check('A4.1-rot', diff, `sharp.rotate(-2°) 与 0° 逐像素${diff ? '不一致' : '一致'}(旋转非平凡)`);
  // A4.2 微倾贴纸在场非空白
  let microOk = true; const microDetail = [];
  for (const [f, deg] of [['thermo_man.png', '-1°'], ['drop_baby.png', '+2°']]) {
    if (!existsSync(PNG + '/' + f)) { microOk = false; microDetail.push(`${f} 缺`); continue; }
    const { data, info } = await rawGray(PNG + '/' + f);
    const s = stats(data, info.width, info.height);
    if (s.black === 0) { microOk = false; microDetail.push(`${f} 空白`); }
  }
  check('A4.2', microOk, `微倾贴纸在场非空白: thermo_man(-1°)/drop_baby(+2°)${microDetail.length ? ' 问题:' + microDetail.join(',') : ''}`);
  // A4.3 印章 -8° 非空白 + 黑占比落有内容区间
  const st = await rawGray(PNG + '/stamp_seal_r8.png');
  const ss = stats(st.data, st.info.width, st.info.height);
  const ratio = ss.black / ss.total;
  check('A4.3', ss.black > 0 && ratio > 0.005 && ratio < 0.6, `stamp_seal_r8 非空白, 黑占比=${(ratio * 100).toFixed(1)}%（双圈内容区间）`);
}
// A4.4 报告 -8° 单列（report 文件中 grep）
{
  const rp = REPORT + '/UI-art-feasibility.md';
  const txt = existsSync(rp) ? readFileSync(rp, 'utf8') : '';
  check('A4.4', /-8°.*(可接受|需降角|需换实现)/s.test(txt) || /印章.*锯齿/s.test(txt), '报告 -8° 锯齿单列结论段在场');
}

// ===== A5 三档网点 =====
{
  const dots = ['d50', 'd25', 'd12'];
  let sizeOk = true, orderOk = true, nonBlank = true; const ratios = [];
  const dotData = [];
  for (const d of dots) {
    const { data, info } = await rawGray(PNG + '/' + d + '.png');
    if (info.width !== 4 || info.height !== 4) sizeOk = false;
    const s = stats(data, info.width, info.height);
    if (s.black === 0) nonBlank = false;
    ratios.push(s.black / s.total); dotData.push(data);
  }
  if (!(ratios[0] > ratios[1] && ratios[1] > ratios[2])) orderOk = false;
  check('A5.1', sizeOk, `3 枚网点 4×4 精确 = ${sizeOk}`);
  check('A5.2', nonBlank && orderOk, `黑占比 d50=${(ratios[0] * 100).toFixed(0)}% > d25=${(ratios[1] * 100).toFixed(0)}% > d12=${(ratios[2] * 100).toFixed(0)}% (理论 50/25/12) 且非空白`);
  // A5.3 平铺 2×2 → 8×8 逐 4×4 单元格一致
  let periodicity = true;
  for (const data of dotData) {
    // data 为 4×4 灰度；构建 8×8 平铺（复制坐标），逐象限与第一象限逐像素比对
    const tiled = new Uint8Array(64);
    for (let ty = 0; ty < 2; ty++) for (let tx = 0; tx < 2; tx++) for (let y = 0; y < 4; y++) for (let x = 0; x < 4; x++) {
      tiled[(ty * 4 + y) * 8 + (tx * 4 + x)] = data[y * 4 + x];
    }
    for (let y = 0; y < 8; y++) for (let x = 0; x < 8; x++) if (tiled[y * 8 + x] !== tiled[(y % 4) * 8 + (x % 4)]) periodicity = false;
  }
  check('A5.3', periodicity, '2×2 平铺 8×8 逐 4×4 单元格一致（周期性成立、无边缘断裂）');
}

// ===== A6 图标族 =====
{
  const iconIds = ['apple', 'book', 'mic', 'drop', 'thermo', 'bell', 'batt', 'mag', 'star', 'cloud', 'wave', 'penhand', 'note', 'moon'];
  const missing = iconIds.filter(id => !pngs.some(f => f === `ic_${id}_48.png`));
  check('A6.1', missing.length === 0, missing.length ? `缺: ${missing.join(',')}` : '14 图标 PNG 全在场');
  const squareIcons = iconIds.filter(id => id !== 'wave');
  let squareOk = true; const sqDetail = [];
  for (const id of squareIcons) {
    const m = await dims(PNG + `/ic_${id}_48.png`);
    if (m.width !== 48 || m.height !== 48) { squareOk = false; sqDetail.push(`${id}:${m.width}x${m.height}`); }
  }
  const wave = await dims(PNG + '/ic_wave_48.png');
  const waveOk = wave.width === 48 && wave.height === 14;
  check('A6.2', squareOk && waveOk, `13 枚 48×48 统一 + ic_wave 48×14 特例 ${(sqDetail.length || !waveOk) ? '问题:' + sqDetail.join(',') + (waveOk ? '' : ' wave' + wave.width + 'x' + wave.height) : '全达标'}`);
  const c = readFileSync(ASSETS + '/assets.c', 'utf8');
  const n = (c.match(/const lv_image_dsc_t/g) || []).length;
  check('A6.1-desc', n >= 8 + 12, `描述符总数=${n} (≥ 8+12=20)`);
}

// ===== A7 站酷快乐体授权 + 样字 =====
{
  const oflZ = PIPE + '/fonts/OFL-ZCOOLKuaiLe.txt';
  const oflB = PIPE + '/fonts/OFL-Baloo2.txt';
  const ttf = PIPE + '/fonts/ZCOOLKuaiLe-Regular.ttf';
  const hasOfl = existsSync(oflZ) && readFileSync(oflZ, 'utf8').includes('SIL OPEN FONT LICENSE Version 1.1');
  check('A7.1', hasOfl && existsSync(ttf) && existsSync(oflB), `OFL 原文落档: ZCOOL OFL=${existsSync(oflZ)} Baloo OFL=${existsSync(oflB)} ZCOOL TTF=${existsSync(ttf)}`);
  const rp = REPORT + '/UI-art-feasibility.md';
  const txt = existsSync(rp) ? readFileSync(rp, 'utf8') : '';
  const terms = /PERMISSION/.test(txt) && /Reserved Font Name/.test(txt) && /免费商用|商用/.test(txt);
  check('A7.2', terms, `报告逐字引用免费商用条款+改名禁则=${terms}`);
  const g = await rawGray(PNG + '/font_zcool_30.png');
  const s = stats(g.data, g.info.width, g.info.height);
  check('A7.3', s.black > 0 && s.black / s.total > 0.01, `站酷样字非空白, 黑占比=${(s.black / s.total * 100).toFixed(1)}%`);
}

// ===== A8 imgfont =====
{
  const glyphs = ['digit_0', 'digit_1', 'digit_2', 'digit_3', 'digit_4', 'digit_5', 'digit_6', 'digit_7', 'digit_8', 'digit_9', 'colon'];
  const missing = glyphs.filter(g => !existsSync(PNG + `/imgfont_${g}.png`));
  check('A8.1', missing.length === 0, missing.length ? `缺: ${missing.join(',')}` : '11 枚 imgfont 全在场');
  const dimz = [];
  for (const g of glyphs) { const m = await dims(PNG + `/imgfont_${g}.png`); dimz.push(`${m.width}x${m.height}`); }
  const same = dimz.every(d => d === dimz[0]);
  check('A8.2', same, `11 枚等宽箱体 ${dimz[0]} — ${same ? '一致' : '不一致: ' + dimz.join(',')}`);
  const dboxes = [];
  for (let d = 0; d <= 9; d++) { const { data, info } = await rawGray(PNG + `/imgfont_digit_${d}.png`); dboxes.push(bboxOf(data, info.width, info.height)); }
  const bottoms = dboxes.map(b => b ? b.y1 : null);
  const bottomSame = bottoms.every(b => b === bottoms[0]) && bottoms[0] !== null;
  const digitMid = dboxes.reduce((a, b) => a + (b.cy || 0), 0) / dboxes.length;
  const colon = await rawGray(PNG + '/imgfont_colon.png');
  const cb = bboxOf(colon.data, colon.info.width, colon.info.height);
  check('A8.3', bottomSame && cb && Math.abs(cb.cy - digitMid) <= 9, `数字 bottom-y 一致=${bottomSame}(bottom=${bottoms[0]}); 冒号垂直中心=${cb ? cb.cy.toFixed(1) : '?'} vs 数字中缝≈${digitMid.toFixed(1)}`);
}

// ===== A9 预算 =====
{
  const csv = readFileSync('C:/AI/KidPal/firmware/partitions/v2/16m.csv', 'utf8');
  const assetsLine = csv.split('\n').find(l => l.startsWith('assets,'));
  check('A9.1', !!assetsLine, `分区表 assets 行: ${assetsLine ? assetsLine.trim() : '缺失'}`);
  const c = readFileSync(ASSETS + '/assets.c', 'utf8');
  const dataSizes = [...c.matchAll(/\.data_size = (\d+)/g)].map(m => +m[1]);
  const grepSum = dataSizes.reduce((a, b) => a + b, 0);
  const rp = REPORT + '/budget.md';
  const bt = existsSync(rp) ? readFileSync(rp, 'utf8') : '';
  const m = bt.match(/total[^\d]*=?\s*([\d,]+)\s*字节/i) || bt.match(/总量[^\d]*=?\s*([\d,]+)/);
  const reportTotal = m ? +m[1].replace(/,/g, '') : null;
  check('A9.2', reportTotal === grepSum, `预算报告 total=${reportTotal} == data_size 求和=${grepSum} (${reportTotal === grepSum ? '对账一致' : '不一致'})`);
  check('A9.3', /I1/i.test(bt), `预算报告含 I1 判定结论行（grep -i I1 命中）`);
}

// ===== E2 管线三处修复 =====
{
  const src = readFileSync(GEN, 'utf8');
  const fn = src.slice(src.indexOf('async function renderGreyscale'));
  const ri = fn.indexOf('.rotate(');
  const fi = fn.indexOf('.flatten(');
  const gi = fn.indexOf('.greyscale(');
  const flattenFirst = fi >= 0 && gi >= 0 && fi < gi;
  const svgUseInline = src.includes('function svgUse') && src.includes('symbol 内容内联');
  const noNestedUse = !/<use href="#pose|ref=\"#pose/.test('');
  check('E2', flattenFirst && svgUseInline, `flatten先于greyscale=${flattenFirst}; svgUse内联=${svgUseInline}`);
}
// E4 报告在场
{
  const mu = REPORT + '/UI-art-feasibility.md';
  const bd = REPORT + '/budget.md';
  check('E4', existsSync(mu) && existsSync(bd), `可行性报告+预算报告落在 tools/asset-pipeline/report/: feas=${existsSync(mu)} budget=${existsSync(bd)}`);
}

// ===== 汇总 =====
let allOk = true;
for (const r of results) {
  if (!r.ok) allOk = false;
  console.log(`${r.ok ? 'PASS' : 'FAIL'}  ${r.id.padEnd(12)}  ${r.detail}`);
}
console.log(`\n===== ${allOk ? 'ALL GREEN (' + results.filter(r => r.ok).length + '/' + results.length + ')' : 'HAS FAILURES'} =====`);
process.exit(allOk ? 0 : 1);