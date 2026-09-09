#!/usr/bin/env node
// firmware-build-check.mjs — 承重结构闸 `firmware-build`（G3 强制层），KidPal 档 2 新写。
//
// 等价源仓 schema+types 双闸：ESP-IDF 固件「能编译过」即固件侧的结构/类型闸（C/C++ 编译器检类型、
// 链接器检符号、idf.py 检组件依赖）。本仓无 Prisma/tsc，这是固件面最贴近的结构判据。
//
// 工具链可得性策略（诚实，不假绿不假红）：
//   - 两路探测（260909-gov-review-gate-truthfulness T3）：PATH 上的 idf.py ∨ 本仓实际通路
//     （kidpal-idf-env skill 登记的 venv python + idf_run.py wrapper）；任一路通即真跑编译，透传 exit。
//   - 两路都不通 → **降级为告警放行**（ok:true, degraded:true），且降级必须带机器可判字段
//     （buildAttempted / toolchainProbes / strictModeEnv）供 verify.mjs 上收——降级不再只是人读 stdout。
//     这非假绿：本闸担保的是「有工具链时编译必须过」，工具链缺失是环境事实而非测试通过。
//
// 可选严格模式：VERIFY_FW_BUILD_REQUIRED=1 时，工具链缺失也红（CI/台架/Pod 用，强制真编译；
// launch 面默认注入见 kidpal-pod-launch-prep SKILL.md「环境既成事实」段）。
//
// 结构化自检：`--self-check`（argv 触发 + isMain 守卫）。

import { existsSync } from 'node:fs';
import { resolve, join } from 'node:path';
import { pathToFileURL } from 'node:url';
import { spawnSync } from 'node:child_process';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
const FW_DIR = join(projectDir, 'firmware');

// 工具链探测路径常量（与 .claude/skills/kidpal-idf-env/SKILL.md 同源——改 skill 路径必同改此处，防静默漂移回 degraded）：
const IDF_VENV_PYTHON = 'C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe';
const IDF_RUN_WRAPPER = 'c:/tmp/idf_run.py'; // 用法："<venv python>" c:/tmp/idf_run.py build（cwd = firmware/；勿传 idf.py 作参数）

function print(payload) { console.log(JSON.stringify(payload, null, 2)); }

// 路 A：PATH 上的 idf.py。
function probePathIdf() {
  const r = spawnSync('idf.py', ['--version'], { stdio: 'pipe', shell: process.platform === 'win32' });
  return { path: 'PATH:idf.py', ok: !r.error && r.status === 0 };
}

// 路 B：venv python + idf_run.py wrapper（本仓实际通路，MSYSTEM 陷阱由 wrapper 内解）。
function probeVenvWrapper() {
  const label = `venv:${IDF_VENV_PYTHON} + ${IDF_RUN_WRAPPER}`;
  if (!existsSync(IDF_VENV_PYTHON) || !existsSync(IDF_RUN_WRAPPER)) {
    return { path: label, ok: false, detail: 'venv python 或 wrapper 不存在' };
  }
  const r = spawnSync(IDF_VENV_PYTHON, [IDF_RUN_WRAPPER, '--version'], { stdio: 'pipe' });
  return { path: label, ok: !r.error && r.status === 0 };
}

// 两路探测：任一路通即工具链可得；probes 全量留痕（降级时进输出字段）。
function probeToolchain() {
  const probes = [probePathIdf(), probeVenvWrapper()];
  return { ok: probes.some((p) => p.ok), probes };
}

function firmwarePresent() {
  return existsSync(FW_DIR) && (existsSync(join(FW_DIR, 'CMakeLists.txt')) || existsSync(join(FW_DIR, 'sdkconfig')));
}

// 纯决策（self-check 直调）：input = { fwPresent, idfOk, required, probes? }
function decide(input) {
  if (!input.fwPresent) {
    return { ok: true, skip: true, reason: 'no-firmware-surface:firmware/ 未建（档 2 起步期合法）' };
  }
  if (input.idfOk) {
    return { ok: true, run: true, buildAttempted: true, reason: '工具链可用，真编译' };
  }
  if (input.required) {
    return { ok: false, reason: '工具链两路探测均不可用且 VERIFY_FW_BUILD_REQUIRED=1：强制真编译失败' };
  }
  return {
    ok: true,
    degraded: true,
    buildAttempted: false,
    toolchainProbes: input.probes || [],
    strictModeEnv: 'VERIFY_FW_BUILD_REQUIRED unset',
    reason: '工具链两路探测均不可用：固件编译未在本机核验（降级放行，非假绿；degraded 字段供 verify.mjs 上收）',
  };
}

function runSelfCheck() {
  const results = [];
  const ok = (name, cond, detail = '') => results.push({ name, ok: Boolean(cond), detail });
  ok('no-firmware-skip-green', decide({ fwPresent: false, idfOk: false, required: false }).skip === true);
  ok('idf-available-runs', decide({ fwPresent: true, idfOk: true, required: false }).run === true);
  ok('idf-missing-degraded-green', decide({ fwPresent: true, idfOk: false, required: false }).degraded === true);
  ok('idf-missing-required-red', decide({ fwPresent: true, idfOk: false, required: true }).ok === false);
  // 260909 T3：双路探测形状 + 降级字段在场（字段即 verify.mjs 上收判据，缺失即退回静默放行）。
  const tc = probeToolchain();
  ok('toolchain-dual-probe', tc.probes.length === 2
    && tc.probes.every((p) => typeof p.path === 'string' && typeof p.ok === 'boolean')
    && tc.ok === tc.probes.some((p) => p.ok),
    tc.probes.map((p) => `${p.path}=${p.ok}`).join(' | '));
  const d = decide({ fwPresent: true, idfOk: false, required: false, probes: [{ path: 'x', ok: false }] });
  ok('degraded-fields-present', d.buildAttempted === false && Array.isArray(d.toolchainProbes) && typeof d.strictModeEnv === 'string');
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
  const tc = probeToolchain();
  const verdict = decide({ fwPresent: firmwarePresent(), idfOk: tc.ok, required, probes: tc.probes });
  if (!verdict.ok) {
    print({ ok: false, gate: 'firmware-build', message: verdict.reason });
    process.exit(1);
  }
  if (verdict.skip || verdict.degraded) {
    print({ ok: true, gate: 'firmware-build', skip: Boolean(verdict.skip), degraded: Boolean(verdict.degraded), reason: verdict.reason,
      ...(verdict.degraded ? { buildAttempted: false, toolchainProbes: tc.probes, strictModeEnv: verdict.strictModeEnv } : {}) });
    process.exit(0);
  }
  // 工具链可用且 firmware 在 → 真编译，透传 exit；按实际通路选命令（PATH idf.py 或 venv+wrapper）。
  print({ ok: true, gate: 'firmware-build', phase: 'building', reason: verdict.reason, via: tc.probes.find((p) => p.ok)?.path });
  const r = tc.probes[0].ok
    ? spawnSync('idf.py', ['build'], { cwd: FW_DIR, stdio: 'inherit', shell: process.platform === 'win32' })
    : spawnSync(IDF_VENV_PYTHON, [IDF_RUN_WRAPPER, 'build'], { cwd: FW_DIR, stdio: 'inherit' });
  if (r.error || r.status !== 0) {
    print({ ok: false, gate: 'firmware-build', message: `固件 build 失败（exit ${r.status ?? 'null'}）` });
    process.exit(typeof r.status === 'number' ? r.status : 1);
  }
  print({ ok: true, gate: 'firmware-build', phase: 'build-pass' });
}
