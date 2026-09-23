#!/usr/bin/env node
/*
 * gen_art_spiffs.mjs — 固件侧 KAP1 资产转换（TaskPackage 260901-ui-art-integration, 包 U）
 *
 * 上游母版纪律：单一母版 = tools/simulator/LvglWindowsSimulator/assets/assets.c（由
 * gen_assets.mjs 从 V3 设计稿生成，阈值化 RGB565，唯一值集 ⊆ {0x0000, 0xFFFF}）。
 * 本脚本把 68 枚闭集白名单资产逐枚从 RGB565 转成 KAP1 1bpp 位图文件，落入
 * firmware/main/assets/spiffs_root/。文件名 = 上游管线资产名 + ".bin"。
 *
 * KAP1 冻结格式（left firmware/main/test/ui_art_integration.acceptance.c 头）：
 *   offset 0 : magic  u32 = 0x3150414B（字节序 'K''A''P''1'）
 *   offset 4 : format u16 = 1 (KIDPAL_ART_FMT_1BPP)
 *   offset 6 : flags  u16 = 0
 *   offset 8 : w      u16
 *   offset 10: h      u16
 *   offset 12: stride u16 = (w+7)/8
 *   offset 14: reserved u16 = 0
 *   offset 16: 像素数据 stride*h 字节（MSB-first，bit=1 为墨）
 *
 * 语义：上游 RGB565 0x0000 = 墨（黑）-> 1bpp bit 1；0xFFFF = 纸（白）-> bit 0。
 * 任何非阈值化中间值 = 转换错误（拒绝静默）。
 *
 * 用法：node tools/asset-pipeline/gen_art_spiffs.mjs
 * 输出：firmware/main/assets/spiffs_root/<name>.bin（47 枚闭集）
 */
import { readFileSync, writeFileSync, mkdirSync, existsSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const __dirname = dirname(fileURLToPath(import.meta.url));
const REPO       = join(__dirname, '..', '..');
const ASSETS_C   = join(REPO, 'tools', 'simulator', 'LvglWindowsSimulator', 'assets', 'assets.c');
const OUT_DIR    = join(REPO, 'firmware', 'main', 'assets', 'spiffs_root');

/* 68 枚闭集白名单（= acceptance artt_expected[] 文件根名，= assets.h 符号名） */
const WHITELIST = [
  /* 吉祥物 7 姿态 */
  'pose_daily_96', 'pose_listen_96', 'pose_think_96', 'pose_talk_96',
  'pose_record_96', 'pose_jump_96',  // pose_sleep_96 v1.14 已裁，不再再生（磁盘旧 bin 留存）
  /* 图标族 14 */
  'ic_apple_48', 'ic_book_48', 'ic_mic_48', 'ic_drop_48', 'ic_thermo_48',
  'ic_bell_48', 'ic_batt_48', 'ic_mag_48', 'ic_star_48', 'ic_cloud_48',
  'ic_penhand_48', 'ic_note_48', 'ic_moon_48', 'ic_wave_48',
  /* V5 增补（2026-09-22 human 五裁定）：wifi 双态 + 波浪线/星星画布目标尺寸直出 */
  'ic_wifi_14', 'ic_wifioff_14', 'ic_wave_58', 'ic_star_22', 'ic_star_18', 'ic_star_38',
  /* V5 补缺批（2026-09-23）：小尺寸族 + 云朵烘斜 + 声波线 + 胶囊/语音条整块烘 */
  'ic_thermo_12', 'ic_drop_12', 'ic_batt_15', 'ic_cloud_40_t2', 'ic_mic_32',
  'ic_bell_24', 'ic_bell_22', 'ic_wave_40', 'ic_star_20', 'rec_wave',
  'p1_chip_jiexialai', 'p1_chip_mingzao', 'voice_chip_p3',
  'bub_listening', 'bub_recording', 'bub_rec_guide', 'bub_thinking', 'bub_speaking', 'arc_listen', 'ic_mic_22',
  'qmark_trio', 'bell_trio',
  /* 手绘框族 3 */
  'frm_236_236', 'frm_sm_160_120', 'frm_flat_240_96',
  /* 抖动网点 3 */
  'd50', 'd25', 'd12',
  /* 印章 + 站酷快乐体样字 */
  'stamp_seal_r8', 'font_zcool_30',
  /* Baloo 2 动态时间 imgfont 11 枚 */
  'imgfont_digit_0', 'imgfont_digit_1', 'imgfont_digit_2', 'imgfont_digit_3',
  'imgfont_digit_4', 'imgfont_digit_5', 'imgfont_digit_6', 'imgfont_digit_7',
  'imgfont_digit_8', 'imgfont_digit_9', 'imgfont_colon',
  /* P1 既有 7 */
  'time_10_24', 'thermo_man', 'drop_baby', 'ic_batt_32', 'ic_bell_13',
  'doodle_star_26', 'doodle_cloud_52',
];

const KAP1_MAGIC_LE = 0x3150414b; // "KAP1"

function u16le(v) { return [(v & 0xff), ((v >> 8) & 0xff)]; }
function u32le(v) { return [v & 0xff, (v >> 8) & 0xff, (v >> 16) & 0xff, (v >> 24) & 0xff]; }

function symbolIndex(text, sym) {
  /* 定位 "const lv_image_dsc_t <sym> = {"; 返回该 descriptor 起始 index */
  const re = new RegExp(`const lv_image_dsc_t ${sym}\\s*=\\s*\\{`);
  const m = re.exec(text);
  return m ? m.index : -1;
}

function parseMapBytes(text, sym) {
  /* 定位 "<sym>_map[] = { ... }"，抽取全部 0xNN 字节 */
  const re = new RegExp(`${sym}_map\\[\\]\\s*=\\s*\\{`);
  const m = re.exec(text);
  if (!m) throw new Error(`map not found for ${sym}`);
  let i = text.indexOf('{', m.index);
  let j = text.indexOf('}', i);
  const body = text.slice(i + 1, j);
  const bytes = [];
  const tokRe = /0x([0-9a-fA-F]{2})/g;
  let t;
  while ((t = tokRe.exec(body)) !== null) {
    bytes.push(parseInt(t[1], 16));
  }
  return bytes;
}

function parseDescriptor(text, sym) {
  const idx = symbolIndex(text, sym);
  if (idx < 0) throw new Error(`descriptor not found for ${sym}`);
  const head = text.slice(idx, idx + 400);
  const wRe = /\.w\s*=\s*(\d+)/;
  const hRe = /\.h\s*=\s*(\d+)/;
  const wm = wRe.exec(head), hm = hRe.exec(head);
  if (!wm || !hm) throw new Error(`dims not found for ${sym}`);
  return { w: parseInt(wm[1], 10), h: parseInt(hm[1], 10) };
}

function makeKAP1(w, h, mapBytes) {
  /* mapBytes = RGB565 LE 像素字节；转换为 1bpp MSB-first, bit=1 墨 */
  const stride = (w + 7) >> 3;
  const dataLen = stride * h;
  const out = Buffer.alloc(16 + dataLen);
  out.writeUInt32LE(KAP1_MAGIC_LE, 0);   // magic
  out.writeUInt16LE(1, 4);               // format = 1bpp
  out.writeUInt16LE(0, 6);               // flags
  out.writeUInt16LE(w, 8);
  out.writeUInt16LE(h, 10);
  out.writeUInt16LE(stride, 12);
  out.writeUInt16LE(0, 14);              // reserved

  let px = 0;
  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      const i = px * 2;
      const v = mapBytes[i] | (mapBytes[i + 1] << 8);
      let bit;
      if (v === 0x0000) bit = 1;        // 墨
      else if (v === 0xffff) bit = 0;   // 纸
      else {
        throw new Error(`non-thresholded pixel value 0x${v.toString(16)} at (${x},${y}) — 语义拒绝`);
      }
      const byteIdx = 16 + y * stride + (x >> 3);
      if (bit) out[byteIdx] |= (0x80 >> (x & 7));
      px++;
    }
  }
  return out;
}

function main() {
  const text = readFileSync(ASSETS_C, 'utf8');
  if (!existsSync(OUT_DIR)) mkdirSync(OUT_DIR, { recursive: true });

  let total = 0;
  const written = [];
  for (const name of WHITELIST) {
    const { w, h } = parseDescriptor(text, name);
    const map = parseMapBytes(text, name);
    const expectedBytes = w * h * 2;
    if (map.length !== expectedBytes) {
      throw new Error(`${name} map byte count ${map.length} != expected ${expectedBytes}`);
    }
    const buf = makeKAP1(w, h, map);
    const outPath = join(OUT_DIR, `${name}.bin`);
    writeFileSync(outPath, buf);
    written.push(`${name}.bin ${w}x${h} ${buf.length}B`);
    total += buf.length;
  }

  console.log(`WROTE ${written.length} KAP1 assets -> ${OUT_DIR} (${total} bytes)`);
  for (const s of written) console.log(`  ${s}`);
}

main();