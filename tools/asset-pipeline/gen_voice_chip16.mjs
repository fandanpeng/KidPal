/*
 * gen_voice_chip16.mjs — 语音轮标识件 16px 锐利 1bpp 资产生产 (TaskPackage
 * 260915-m1-k3-voice-round-ui device-round VQ-A7 r5)。依流程红线②「1bpp 资产禁运行时
 * lv_image_set_scale 缩放」——r4 曾把 48px/96px 源经 lv_image_set_scale 运行时缩到 ~16px,
 * 1bpp 阈值下笔画被抗锯齿稀释。本脚本在管线侧重出 16px 目标尺寸锐利资产 (程序化逐像素
 * 绘制 1bpp, 无抗锯齿稀释, 正是「16px 锐利」的达成形态), 源形态参照:
 *   - mic      = docs/ui/kidpal-ui-design-v4.html 的 #ic-mic SVG (麦克风: 圆角胶囊体
 *                + 两横条 + 侧杆 + 底部半圆 U 弧系座)
 *   - question = tools/asset-pipeline/gen_question.mjs 的 r3 问号剪影 (头部圆环 +
 *                右侧连接杆 + 底部实心点), 缩放到 16px。
 *
 * 产出两枚 16x16 1bpp KAP1 (冻结 little-endian 格式, art_assets.h) 落
 * firmware/main/assets/spiffs_root/:
 *   ic_mic_16.bin      -- 聆听态标识件 (WF-B-01)
 *   question_16.bin    -- 思考态标识件 (WF-B-02)
 * 旧 ic_mic_48.bin / question_96.bin 留闭集不删 (防归档齿参照断)。管线重跑幂等覆盖。
 *
 * 用法:  cd tools/asset-pipeline && node gen_voice_chip16.mjs
 */
import { writeFileSync, mkdirSync } from "node:fs";
import { resolve, dirname } from "node:path";
import { fileURLToPath } from "node:url";

const HERE = dirname(fileURLToPath(import.meta.url));
const SPIFFS = resolve(HERE, "..", "..", "firmware", "main", "assets", "spiffs_root");
const W = 16, H = 16, STRIDE = (W + 7) >> 3; /* = 2 bytes/row */

function makeCanvas() { return new Uint8Array(W * H); }
function setFn(cv) {
    return function set(x, y) {
        if (x < 0 || y < 0 || x >= W || y >= H) return;
        cv[y * W + x] = 1;
    };
}
function kap1Header(fmt, w, h, stride) {
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
function packKAP1(cv) {
    const data = Buffer.alloc(STRIDE * H);
    for (let y = 0; y < H; y++) {
        for (let x = 0; x < W; x++) {
            if (cv[y * W + x]) data[y * STRIDE + (x >> 3)] |= (0x80 >> (x & 7));
        }
    }
    return data;
}
/* 实心圆盘 */
function disc(set, cx, cy, r) {
    for (let dy = -Math.ceil(r); dy <= Math.ceil(r); dy++)
        for (let dx = -Math.ceil(r); dx <= Math.ceil(r); dx++)
            if (dx * dx + dy * dy <= r * r) set(cx + dx, cy + dy);
}
/* 圆环描边 (th = 线宽); 用整数像素偏移防 96px 整数半径->16px 分数半径导致 float 数组下标丢点 */
function ring(set, cx, cy, R, th) {
    const half = th / 2;
    const span = Math.ceil(R + half);
    for (let dy = -span; dy <= span; dy++)
        for (let dx = -span; dx <= span; dx++) {
            const d = Math.sqrt(dx * dx + dy * dy);
            if (Math.abs(d - R) <= half) set(cx + dx, cy + dy);
        }
}
/* 实心横竖条 */
function vbar(set, x0, x1, y0, y1) {
    for (let y = y0; y <= y1; y++) for (let x = x0; x <= x1; x++) set(x, y);
}
function hbar(set, x0, x1, y) { for (let x = x0; x <= x1; x++) set(x, y); }
/* 描边矩形 (t=线厚), 圆角由 ring 半环补 */
function strokeRect(set, x0, y0, x1, y1, t) {
    const half = (t - 1) / 2;
    for (let y = y0; y <= y1; y++)
        for (let x = x0; x <= x1; x++) {
            const nearHor = (y - y0 <= half) || (y1 - y <= half);
            const nearVer = (x - x0 <= half) || (x1 - x <= half);
            if (nearHor || nearVer) set(x, y);
        }
}

/* ---- mic (source #ic-mic 48x48 SVG) ---- */
function drawMic() {
    const cv = makeCanvas(); const set = setFn(cv);
    /* 麦克风胶囊体 (设计 rect x17..31(y0=6..29) : 缩 3 => 5.7..10.3, 中 x=8; y2..9.7) */
    /* 胶囊体外描边: 竖躯 x6..10, y1..9, 3px 厚的描边矩形 + 顶/底圆角段 */
    strokeRect(set, 6, 1, 10, 9, 3);          // 躯干壁 (壁厚3 → 内芯 x6..10 黑色)
    /* 内部两横条 (设计 M21 14 / M21 19 => y≈4.7 / 6.3) */
    hbar(set, 7, 8, 3);
    hbar(set, 7, 8, 6);
    /* 侧杆 (设计 M24 29..36 => x=8, y10..14; 细 2px, 下探到与底部 U 弧衔接) */
    vbar(set, 7, 8, 10, 13);
    /* 底部 U 弧系座 (设计 M15 33 a9 9 0 0 0 18 0 => 心(8,13.5) R≈3, 下半环开口向上) */
    const AR = 2.3, Ath = 2.0, cyA = 13;
    for (let dx = -4; dx <= 4; dx++)
        for (let dy = 0; dy <= 3; dy++) {
            const d = Math.sqrt(dx * dx + dy * dy);
            if (Math.abs(d - AR) <= Ath / 2) set(8 + dx, cyA + dy);
        }
    return cv;
}

/* ---- question (source r3 问号剪影, 96->16) ---- */
function drawQuestion() {
    const cv = makeCanvas(); const set = setFn(cv);
    /* 头部圆环 (设计 ring(48,36,16,8) => 心(8,6) R2.75 th1.25) */
    ring(set, 8, 5, 2.75, 1.25);
    /* 右侧连接杆 (设计 vbar 58..66 x 46..74 => x9.3..11, y7.7..12.3) */
    vbar(set, 9, 10, 7, 11);
    /* 底部实心点 (设计 disc(63,84,7) => 心(10.5,14) r1.17) */
    disc(set, 10, 14, 1.2);
    return cv;
}

/* ---- 落盘 + 自校验 (magic / ink 非空非满) ---- */
function emit(name, cv) {
    let ink = 0;
    for (let i = 0; i < cv.length; i++) ink += cv[i];
    if (!(ink > 0 && ink < W * H)) {
        throw new Error(`${name}: ink=${ink}/${W * H} 非空非满校验不过 — 资产形态失败`);
    }
    const blob = Buffer.concat([kap1Header(1, W, H, STRIDE), packKAP1(cv)]);
    const OUT = resolve(SPIFFS, name);
    writeFileSync(OUT, blob);
    const magic = blob.subarray(0, 4).toString("latin1");
    console.log(`wrote ${OUT}`);
    console.log(`  size=${16 + blob.length - 16}B ink=${ink}/${W * H} nonBlank=${ink > 0 && ink < W * H} magic=${magic}`);
    console.log(ascii(cv));
    return blob;
}
function ascii(cv) {
    const rows = [];
    rows.push("  " + "0123456789abcdef".split("").join(""));
    for (let y = 0; y < H; y++) {
        let s = "";
        for (let x = 0; x < W; x++) s += cv[y * W + x] ? "#" : ".";
        rows.push(String(y).padStart(1, " ") + " " + s);
    }
    return rows.join("\n");
}

mkdirSync(SPIFFS, { recursive: true });
emit("ic_mic_16.bin", drawMic());
emit("question_16.bin", drawQuestion());