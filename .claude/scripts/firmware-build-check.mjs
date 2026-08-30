#!/usr/bin/env node
// firmware-build-check.mjs — 承重结构闸 `firmware-build`（G3 强制层），KidPal 档 2 新写。
//
// 等价源仓 schema+types 双闸：ESP-IDF 固件「能编译过」即固件侧的结构/类型闸（C/C++ 编译器检类型、
// 链接器检符号、idf.py 检组件依赖）。本仓无 Prisma/tsc，这是固件面最贴近的结构判据。
//
// 工具链可得性策略（诚实，不假绿不假红）：
//   - idf.py 可用  → 真跑 `idf.py build`（firmware/ 为工程根），透传 exit。
//   - idf.py 不可用（开发机未装 ESP-IDF 环境 / CI 未配）→ **降级为告警放行**（ok:true, degraded:true），
//     并在输出显式标注「固件编译未在本机核验」。这非假绿：本闸担保的是「有工具链时编译必须过」，
//     工具链缺失是环境事实而非测试通过。实机/完整编译由 acceptance 相位或台架担纲。
//
// 可选严格模式：VERIFY_FW_BUILD_REQUIRED=1 时，idf.py 缺失也红（CI/台架用，强制真编译）。
//
// 结构化自检：`--self-check`（argv 触发 + isMain 守卫）。

import { existsSync } from 'node:fs';
import { resolve, join } from 'node:path';
import { pathToFileURL } from 'node:url';
import { spawnSync } from 'node:child_process';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
const FW_DIR = join(projectDir, 'firmware');

function print(payload) { console.log(JSON.stringify(payload, null, 2)); }

function idfAvailable() {
  const r = spawnSync('idf.py', ['--version'], { stdio: 'pipe', shell: process.platform === 'win32' });
  return !r.error && r.status === 0;
}

function firmwarePresent() {
  return existsSync(FW_DIR) && (existsSync(join(FW_DIR, 'CMakeLists.txt')) || existsSync(join(FW_DIR, 'sdkconfig')));
}

// 纯决策（self-check 直调）：input = { fwPresent, idfOk, required }
function decide(input) {
  if (!input.fwPresent) {
    return { ok: true, skip: true, reason: 'no-firmware-surface:firmware/ 未建（档 2 起步期合法）' };
  }
  if (input.idfOk) {
    return { ok: true, run: true, reason: 'idf.py 可用，真编译' };
  }
  if (input.required) {
    return { ok: false, reason: 'idf.py 不可用且 VERIFY_FW_BUILD_REQUIRED=1：强制真编译失败' };
  }
  return { ok: true, degraded: true, reason: 'idf.py 不可用：固件编译未在本机核验（降级放行，非假绿）' };
}

function runSelfCheck() {
  const results = [];
  const ok = (name, cond, detail = '') => results.push({ name, ok: Boolean(cond), detail });
  ok('no-firmware-skip-green', decide({ fwPresent: false, idfOk: false, required: false }).skip === true);
  ok('idf-available-runs', decide({ fwPresent: true, idfOk: true, required: false }).run === true);
  ok('idf-missing-degraded-green', decide({ fwPresent: true, idfOk: false, required: false }).degraded === true);
  ok('idf-missing-required-red', decide({ fwPresent: true, idfOk: false, required: true }).ok === false);
  const allOk = results.every((r) => r.ok);
  process.stdout.write(`${JSON.stringify({ selfCheck: 'firmware-build-check', ok: allOk, results }, null, 2)}\n`);
  process.exit(allOk ? 0 : 1);
}

function isMain() {
  const invoked = process.argv[1];
  return Boolean(invoked && import.meta.url === pathToFileURL(resolve(invoked)).href);
}

if (isMain() && process.argv.includes('--self-check')) runSelfCheck();

if (isMain() && !process.argv.includes('--self-check')) {
  const required = process.env.VERIFY_FW_BUILD_REQUIRED === '1';
  const verdict = decide({ fwPresent: firmwarePresent(), idfOk: idfAvailable(), required });
  if (!verdict.ok) {
    print({ ok: false, gate: 'firmware-build', message: verdict.reason });
    process.exit(1);
  }
  if (verdict.skip || verdict.degraded) {
    print({ ok: true, gate: 'firmware-build', skip: Boolean(verdict.skip), degraded: Boolean(verdict.degraded), reason: verdict.reason });
    process.exit(0);
  }
  // idf.py 可用且 firmware 在 → 真编译，透传 exit。
  print({ ok: true, gate: 'firmware-build', phase: 'building', reason: verdict.reason });
  const r = spawnSync('idf.py', ['build'], { cwd: FW_DIR, stdio: 'inherit', shell: process.platform === 'win32' });
  if (r.error || r.status !== 0) {
    print({ ok: false, gate: 'firmware-build', message: `idf.py build 失败（exit ${r.status ?? 'null'}）` });
    process.exit(typeof r.status === 'number' ? r.status : 1);
  }
  print({ ok: true, gate: 'firmware-build', phase: 'build-pass' });
}
