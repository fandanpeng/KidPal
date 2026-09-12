/*
 * gen_bubble_tail.mjs — V3 气泡「左下斜切小尾巴」资产生产 (TaskPackage
 * 260911-render-pages RP-A9/S0.1: U 闭集唯一新增 KAP1 = bubble_tail_24.bin)。
 *
 * 产出 24x24 1bpp KAP1 (冻结 little-endian 格式, art_assets.h) 落
 * firmware/main/assets/spiffs_root/。尾巴 = 左下三角斜切剪影 (V3 §06 气泡尾巴),
 * 非空白非全黑 (ART-A5 对账形状合规)。管线重跑幂等覆盖。
 *
 * 用法:  cd tools/asset-pipeline && node gen_bubble_tail.mjs
 */
import { writeFileSync, mkdirSync } from "node:fs";
import { resolve, dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const HERE = dirname(fileURLToPath(import.meta.url));
const OUT = resolve(HERE, "..", "..", "firmware", "main", "assets", "spiffs_root", "bubble_tail_24.bin");
const W = 24, H = 24, STRIDE = 3;

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

// 左下三角斜切尾巴: row y (顶层 y=0): 墨区集中在左侧下缘 (行越靠底、墨越多)。
const data = Buffer.alloc(STRIDE * H);
for (let y = 0; y < H; y++) {
    // 尾巴剪影: 自底部上的第 (H-y) 条水平左伸线的长度 = 随 y 增大而增长 (实心三角)。
    const inked = (y < 8) ? 0 : Math.max(2, Math.round((y - 8) / (H - 8) * 14));
    for (let x = 0; x < W && x < inked; x++) {
        const byte = y * STRIDE + (x >> 3);
        data[byte] |= (0x80 >> (x & 7));
    }
}

mkdirSync(dirname(OUT), { recursive: true });
const blob = Buffer.concat([kap1_header(1, W, H, STRIDE), data]);
writeFileSync(OUT, blob);

let ink = 0;
for (const b of data) { for (let bit = 0; bit < 8; bit++) if (b & (0x80 >> bit)) ink++; }
const magic = blob.subarray(0, 4).toString("latin1");
console.log(`wrote ${OUT}`);
console.log(`  size=${16 + data.length}B ink=${ink}/${W * H} nonBlank=${ink > 0 && ink < W * H}`);
console.log(`  magic=${magic}`);