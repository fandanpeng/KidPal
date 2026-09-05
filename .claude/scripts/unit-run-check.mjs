#!/usr/bin/env node
// unit-run-check.mjs — 承重功能闸 `unit` 的 cmd（G3 强制层），KidPal 档 2 重写。
//
// 源仓（pnpm/vitest 单体）版治的是「pnpm -r run test:unit 部分覆盖静默假绿」——≥1 包定义
// test:unit 即跑那部分、静默跳过其余。本仓无 pnpm workspace，该假绿形态不成立；但等价风险仍在：
// 盘上有 unit 测试文件、却没有任何机制真跑它们 → 静默假绿。故本闸保留**设计意图**（治假绿），
// oracle 与执行器按本仓双实现面重写：
//
//   实现面 A · firmware/（ESP-IDF C/C++）：unit 测试 = host 侧 Unity/CTest（components/*/test 或
//     test/ 目录下 *.test.c/*.cpp，跑在 linux target 或 qemu）。oracle 检测：盘上存在 host 单测文件
//     但无对应可运行入口（idf.py --preview set-target linux + idf.py build 之外无 unity runner）→ 红。
//     ESP-IDF host 测试需单独 linux target 构建，本闸只做「有测试文件 ⇒ 有 runner 入口」结构闸，
//     真跑由 --self-check 之外的 full 相位经 idf.py 完成（实机/仿真耗时，scoped 相位不强制）。
//
//   实现面 B · server/（FastAPI shim）：unit 测试 = pytest（server/**/test_*.py 或 *_test.py）。
//     oracle 检测：盘上存在 pytest 文件但无 pytest 可运行证据（无 pytest.ini/pyproject/pytest 依赖
//     且无法 import）→ 红。决策通过后执行 `python -m pytest -q`（server 面）并透传 exit。
//
// 红半径 = verifier final（timing:full）——unit 不在合并 quick-check 半径。
//
// 结构化自检：`--self-check`（argv 触发 + isMain 守卫）跑决策电池 + 实盘 smoke。

import { existsSync, readFileSync, readdirSync, statSync } from 'node:fs';
import { resolve, join, relative } from 'node:path';
import { pathToFileURL } from 'node:url';
import { execSync } from 'node:child_process';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// 固件 host 单测文件（C/C++，Unity 框架，跑 host/qemu 非实机）。
const FW_UNIT_RE = /\.(test)\.(c|cpp|cc)$/;
// shim pytest 文件。
const PY_TEST_RE = /(^|\/)(test_[^/]*|[^/]*_test)\.py$/;
// 枚举时跳过的目录。
const SKIP_DIRS = new Set(['node_modules', 'build', 'dist', 'coverage', '.git', 'managed_components', '__pycache__', 'ESP32-S3-RLCD-4.2-Demo']);

function print(payload) { console.log(JSON.stringify(payload, null, 2)); }
function fail(message, details = {}) { print({ ok: false, gate: 'unit', message, ...details }); process.exit(1); }

// 递归收集匹配文件（相对 root）。
function collectFiles(root, re) {
  const hits = [];
  if (!existsSync(root)) return hits;
  const walk = (dir) => {
    let entries;
    try { entries = readdirSync(dir); } catch { return; }
    for (const entry of entries) {
      if (SKIP_DIRS.has(entry) || entry.startsWith('.')) continue;
      const full = join(dir, entry);
      let st;
      try { st = statSync(full); } catch { continue; }
      if (st.isDirectory()) walk(full);
      else if (st.isFile() && re.test(relative(root, full).split('\\').join('/'))) hits.push(relative(root, full).split('\\').join('/'));
    }
  };
  walk(root);
  return hits.sort();
}

// pytest 可运行证据：server/ 下有 pytest.ini / pyproject.toml / setup.cfg 之一，或 requirements 提及 pytest。
function hasPytestRunner(serverDir) {
  if (!existsSync(serverDir)) return false;
  for (const f of ['pytest.ini', 'pyproject.toml', 'setup.cfg', 'tox.ini']) {
    if (existsSync(join(serverDir, f))) return true;
  }
  for (const req of ['requirements.txt', 'requirements-dev.txt']) {
    const p = join(serverDir, req);
    if (existsSync(p) && /pytest/i.test(readFileSync(p, 'utf8'))) return true;
  }
  return false;
}

// 固件 host 单测 runner 证据：存在 linux/qemu target 配置或专门 host test 入口。
function hasFwUnitRunner(fwDir) {
  if (!existsSync(fwDir)) return false;
  // 约定：firmware/host_test/ 目录或 sdkconfig.ci.linux / CMakeLists 提及 linux target。
  if (existsSync(join(fwDir, 'host_test'))) return true;
  if (existsSync(join(fwDir, 'sdkconfig.ci.linux'))) return true;
  // 260904（human 授权直改，afd31ac 先例）：INF-A4 新约定 main/test/host_app/ 为可执行 linux-target runner，
  //   以 host_app 组件 sdkconfig.defaults 含 CONFIG_IDF_TARGET_LINUX=y 为判据。下游 host 包照 INF-A4 挂测试。
  const hostAppSdk = join(fwDir, 'main', 'test', 'host_app', 'sdkconfig.defaults');
  if (existsSync(hostAppSdk) && /CONFIG_IDF_TARGET_LINUX=y/.test(readFileSync(hostAppSdk, 'utf8'))) return true;
  return false;
}

// 纯决策（self-check 直调，不触盘）：
//   input = { fwUnitFiles: string[], fwHasRunner: bool, pyTestFiles: string[], pyHasRunner: bool }
// 返回 { ok, reason, violations }。
//   规则：有测试文件而无 runner 证据 → 红（真空绿）。无任何测试文件 → 跳过绿。
function decide(input) {
  const violations = [];
  if (input.fwUnitFiles.length > 0 && !input.fwHasRunner) {
    violations.push({ surface: 'firmware', unitFiles: input.fwUnitFiles, issue: 'has-unit-files-no-host-runner' });
  }
  if (input.pyTestFiles.length > 0 && !input.pyHasRunner) {
    violations.push({ surface: 'server', unitFiles: input.pyTestFiles, issue: 'has-pytest-files-no-runner' });
  }
  if (violations.length > 0) return { ok: false, reason: 'has-unit-files-without-runner', violations };
  return { ok: true, reason: null, violations: [] };
}

// host 用例判据：文件是否标 live（live on-target 验收不 auto-run，对偶固件 on-target「只结构闸」）。
function fileHasLiveMark(absPath) {
  try {
    return /@pytest\.mark\.live|pytestmark\s*=\s*pytest\.mark\.live/.test(readFileSync(absPath, 'utf8'));
  } catch {
    return false; // 读不到按 host 处理（fail-safe 向严：宁 auto-run 也不静默跳过未知）
  }
}

// pytest 是否可运行：未装 fail-open（对偶 firmware idf.py 缺失 fail-open）。
function pytestAvailable() {
  try { execSync('python -m pytest --version', { stdio: 'ignore' }); return true; } catch { return false; }
}

function collectReal(root) {
  const fwDir = join(root, 'firmware');
  const serverDir = join(root, 'server');
  return {
    fwUnitFiles: collectFiles(fwDir, FW_UNIT_RE),
    fwHasRunner: hasFwUnitRunner(fwDir),
    pyTestFiles: collectFiles(serverDir, PY_TEST_RE),
    pyHasRunner: hasPytestRunner(serverDir),
  };
}

function runSelfCheck() {
  const results = [];
  const ok = (name, cond, detail = '') => results.push({ name, ok: Boolean(cond), detail });

  // ① firmware 有 host 单测无 runner → 红
  const d1 = decide({ fwUnitFiles: ['main/storage.test.c'], fwHasRunner: false, pyTestFiles: [], pyHasRunner: false });
  ok('state1-fw-files-no-runner-red', !d1.ok && d1.reason === 'has-unit-files-without-runner' && d1.violations[0].surface === 'firmware');

  // ② server 有 pytest 无 runner → 红
  const d2 = decide({ fwUnitFiles: [], fwHasRunner: false, pyTestFiles: ['shim/test_store.py'], pyHasRunner: false });
  ok('state2-py-files-no-runner-red', !d2.ok && d2.violations[0].surface === 'server');

  // ③ 有文件有 runner → 绿
  const d3 = decide({ fwUnitFiles: ['main/storage.test.c'], fwHasRunner: true, pyTestFiles: ['shim/test_store.py'], pyHasRunner: true });
  ok('state3-files-with-runner-green', d3.ok);

  // ④ 全仓零测试文件 → 跳过绿（档 2 起步期合法；不红，由验收闸担纲）
  const d4 = decide({ fwUnitFiles: [], fwHasRunner: false, pyTestFiles: [], pyHasRunner: false });
  ok('state4-no-files-green', d4.ok);

  // ⑤ 命名样本固化
  ok('state5-fw-glob', FW_UNIT_RE.test('storage.test.c') && FW_UNIT_RE.test('a.test.cpp') && !FW_UNIT_RE.test('storage.c'));
  ok('state5-py-glob', PY_TEST_RE.test('test_store.py') && PY_TEST_RE.test('store_test.py') && PY_TEST_RE.test('shim/test_api.py') && !PY_TEST_RE.test('store.py'));

  // 实盘 smoke（本仓当前多半无测试 → 决策绿）
  try {
    const real = collectReal(projectDir);
    const dReal = decide(real);
    ok('real-repo-smoke', true, `fwFiles=${real.fwUnitFiles.length} pyFiles=${real.pyTestFiles.length} ok=${dReal.ok}`);
  } catch (e) {
    ok('real-repo-smoke', false, `enumeration failed: ${e.message}`);
  }

  const allOk = results.every((r) => r.ok);
  process.stdout.write(`${JSON.stringify({ selfCheck: 'unit-run-check', ok: allOk, results }, null, 2)}\n`);
  process.exit(allOk ? 0 : 1);
}

function isMain() {
  const invoked = process.argv[1];
  return Boolean(invoked && import.meta.url === pathToFileURL(resolve(invoked)).href);
}

if (isMain() && process.argv.includes('--self-check')) runSelfCheck();

if (isMain() && !process.argv.includes('--self-check')) {
  let real;
  try {
    real = collectReal(projectDir);
  } catch (e) {
    fail('oracle 枚举失败（fail-safe 向严：不吞成绿）', { error: e.message });
  }
  const verdict = decide(real);
  if (!verdict.ok) {
    fail('真空绿：以下面存在 unit 测试文件却无对应 runner 证据（有测试而无真跑 = 静默假绿）',
      { reason: verdict.reason, violations: verdict.violations });
  }
  // 执行器：仅 server 面 host-fast pytest 在闸内真跑；live on-target（@pytest.mark.live，如 cloud-link 验收）
  // 与固件 on-target 同待遇——只结构闸、不 auto-run。pytest 未装 fail-open（对偶 firmware idf.py 缺失）。
  if (real.pyHasRunner && real.pyTestFiles.length > 0) {
    const hostFiles = real.pyTestFiles.filter((f) => !fileHasLiveMark(join(projectDir, 'server', f)));
    if (hostFiles.length === 0) {
      print({ ok: true, gate: 'unit', phase: 'executor-skipped', reason: 'live-only on-target tests（@pytest.mark.live，手动触发）' });
    } else if (!pytestAvailable()) {
      print({ ok: true, gate: 'unit', phase: 'executor-skipped', reason: 'pytest 未安装（fail-open）' });
    } else {
      try {
        execSync(`python -m pytest -q ${hostFiles.join(' ')}`, { cwd: join(projectDir, 'server'), stdio: 'inherit' });
      } catch (err) {
        process.exit(typeof err.status === 'number' ? err.status : 1);
      }
    }
  }
}
