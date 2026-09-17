/*
 * gen_question.mjs — 飘问号资产生产 (TaskPackage 260915-m1-k3-voice-round-ui
 * device-round VQ-A7 r3 §C2, WF-B-02 思考态「飘问号」)。
 *
 * 产出 96x96 1bpp KAP1 (冻结 little-endian 格式, art_assets.h) 落
 * firmware/main/assets/spiffs_root/question_96.bin。独立 KAP1 资产形态 (区别于
 * imgfont 加 '?' 字形备选返钉路径——本资产为思考态三位置静态摆, 自绘简笔问号剪影,
 * 参 UIUX-DESIGN-BRIEF §3.1 pose-think 头顶问号画法语义)。管线重跑幂等覆盖。
 *
 * 用法:  cd tools/asset-pipeline && node gen_question.mjs
 */
import { writeFileSync, mkdirSync } from "node:fs";
import { resolve, dirname } from "node:path";
import { fileURLToPath } from "node:url";

const HERE = dirname(fileURLToPath(import.meta.url));
const OUT = resolve(HERE, "..", "..", "firmware", "main", "assets", "spiffs_root", "question_96.bin");
const W = 96, H = 96, STRIDE = (W + 7) >> 3;

function kap1_header(fmt, w, h, stride) {
    const b = Buffer.alloc(16);
    b.write("KAP1", 0, "latin1");
    b.writeUInt16LE(fmt, 4);
    b.writeUInt16LE(0, 6);       // flags
    b.writeUInt16LE(w, 8);
    b.writeUInt16LE(h, 10);
    b.writeUInt16LE(stride, 12);
    b.writeUInt16LE(0, 14);      // reserved
    return b;
}

const data = Buffer.alloc(STRIDE * H);
function set(x, y) {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    const byte = y * STRIDE + (x >> 3);
    data[byte] |= (0x80 >> (x & 7));
}
/* 实心圆盘 (dot / 相并段) */
function disc(cx, cy, r) {
    for (let dy = -r; dy <= r; dy++) {
        for (let dx = -r; dx <= r; dx++) {
            if (dx * dx + dy * dy <= r * r) set(cx + dx, cy + dy);
        }
    }
}
/* 圆环描边 (th = 线宽) —— 问号头部「o」形 */
function ring(cx, cy, R, th) {
    const half = th / 2;
    for (let dy = -(R + half); dy <= (R + half); dy++) {
        for (let dx = -(R + half); dx <= (R + half); dx++) {
            const d = Math.sqrt(dx * dx + dy * dy);
            if (Math.abs(d - R) <= half) set(cx + dx, cy + dy);
        }
    }
}
/* 实心竖条 —— 问号右侧下垂连接杆 */
function vbar(x0, x1, y0, y1) {
    for (let y = y0; y <= y1; y++) {
        for (let x = x0; x <= x1; x++) set(x, y);
    }
}

// 简笔问号剪影: 圆环头部 (48,36) + 右侧连接杆 + 底部实心点。
ring(48, 36, 16, 8);
vbar(58, 66, 46, 74);
disc(63, 84, 7);

mkdirSync(dirname(OUT), { recursive: true });
const blob = Buffer.concat([kap1_header(1, W, H, STRIDE), data]);
writeFileSync(OUT, blob);

let ink = 0;
for (const b of data) { for (let bit = 0; bit < 8; bit++) if (b & (0x80 >> bit)) ink++; }
const magic = blob.subarray(0, 4).toString("latin1");
console.log(`wrote ${OUT}`);
console.log(`  size=${16 + data.length}B ink=${ink}/${W * H} nonBlank=${ink > 0 && ink < W * H}`);
console.log(`  magic=${magic}`);