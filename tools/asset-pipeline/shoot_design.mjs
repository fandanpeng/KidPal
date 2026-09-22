// shoot_design.mjs — 设计稿 HTML → 每画布精确截图（布局锚图片，给 dev-loop Pod 消费）。
// 原理：无头 Edge 逐 `.dev[data-od-id]` 元素截图（元素级裁剪，绝不手抠区域）。
// 每画布出两版：
//   docs/ui/<VER>/<id>.png        — 2×（800×600）抗锯齿，看细节
//   docs/ui/<VER>/<id>.1bpp.png   — 1:1（400×300）阈值化纯黑白，真机 1bpp 观感的唯一真相版
// 用法：node shoot_design.mjs <html 绝对路径> <输出目录>   （输出目录须已存在，旧同名文件被覆盖）
import { chromium } from 'playwright-core';
import sharp from 'sharp';
import { mkdirSync } from 'fs';
import { pathToFileURL } from 'url';

const [htmlPath, outDir] = process.argv.slice(2);
if (!htmlPath || !outDir) { console.error('usage: node shoot_design.mjs <html> <outdir>'); process.exit(1); }
mkdirSync(outDir, { recursive: true });

const browser = await chromium.launch({ channel: 'msedge', headless: true });
const page = await browser.newPage({ viewport: { width: 1600, height: 1200 }, deviceScaleFactor: 1 });
page.on('console', m => { if (m.type() === 'error') console.warn('[page error]', m.text()); });
await page.goto(pathToFileURL(htmlPath).href, { waitUntil: 'networkidle', timeout: 60000 });
await page.evaluate(() => document.fonts.ready);
await page.waitForTimeout(500); // webfont 换装 + SVG <use> 解析余量

// 去掉 .dev 的装饰 outline（画布边框是展示用描边，不属于 400×300 内容）；
// 冻结动画（.bob/.wiggle 抖动是随机相位的，会污染跨版本像素对比）
await page.addStyleTag({ content: '.dev{outline:none !important} *{animation:none !important; transition:none !important}' });

// 每个 .dev 取名：自身 data-od-id（主画布），否则向上找最近的 data-od-id 祖先（子画面 figure）
// 只截布局尺寸为 400×300 的整幅画布；comp-* 组件预览（自定义小尺寸 .dev）跳过
const meta = await page.$$eval('.dev', els => els.map(e => {
  let n = e;
  while (n && !n.dataset.odId) n = n.parentElement;
  return { id: n?.dataset.odId, w: e.offsetWidth, h: e.offsetHeight };
}));
const devs = await page.$$('.dev');
const shots = [];
for (let i = 0; i < devs.length; i++) {
  const { id, w, h } = meta[i];
  if (w !== 400 || h !== 300) { console.log(`  (skip ${id ?? '<anon>'}: ${w}x${h} 非整幅画布)`); continue; }
  shots.push({ el: devs[i], id });
}
console.log('canvases:', shots.map(s => s.id).join(', '));

async function shootAll(z, suffix, threshold) {
  await page.addStyleTag({ content: `.devsz{--z:${z} !important}` });
  await page.waitForTimeout(150);
  const W = 400 * z, H = 300 * z;
  for (const { el, id } of shots) {
    await el.scrollIntoViewIfNeeded();
    // 整数像素吸附 + 整数 clip：el.screenshot() 内部会再滚动，与吸附竞争，且跨版本页面
    // 高度不同会让文字光栅相位漂移（实测未改页面可差到 3%+）。改为手动滚动后按整数
    // 视口 clip 截图，完全接管滚动，保证同内容跨版本逐像素一致。
    const d = await el.evaluate(e => {
      const r = e.getBoundingClientRect();
      return { dx: Math.round(r.x) - r.x, dy: Math.round(r.y) - r.y, x: Math.round(r.x), y: Math.round(r.y) };
    });
    const t = `translate(${d.dx}px, ${d.dy}px) scale(${z})`;
    if (d.dx || d.dy) await el.evaluate((e, tr) => { e.style.transform = tr; }, t);
    const buf = await page.screenshot({ clip: { x: d.x, y: d.y, width: W, height: H } });
    if (d.dx || d.dy) await el.evaluate(e => { e.style.transform = ''; });
    // 元素可能落在小数坐标上（截图高度会进位 +1），裁回精确 400×300·z
    const shot = sharp(buf).extract({ left: 0, top: 0, width: W, height: H });
    const out = `${outDir}/${id}${suffix}`;
    if (threshold) {
      // 真机 1bpp 口径：灰度抗锯齿 → 128 阈值化，唯一值 ⊆ {0,255}
      await shot.grayscale().threshold(128).png().toFile(out);
    } else {
      await shot.png().toFile(out);
    }
    console.log(`  ${out} ${W}x${H}`);
  }
}

await shootAll(2, '.png', false);      // 2× 抗锯齿细节版
await shootAll(1, '.1bpp.png', true);  // 1:1 阈值化真相版

await browser.close();
console.log('done →', outDir);
