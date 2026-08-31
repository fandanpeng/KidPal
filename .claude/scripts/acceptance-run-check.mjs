#!/usr/bin/env node
// acceptance-run-check.mjs — 承重功能闸 `acceptance` 的 cmd（G3 强制层），KidPal 档 2 重写。
//
// 源仓（pnpm/vitest 单体）版治「pnpm -r run test:acceptance 部分覆盖静默假绿」：盘上含
// `.acceptance.` infix 文件的包必须接 test:acceptance script，否则 pnpm 静默跳过 → 假绿。
// 本仓无 pnpm workspace，但**设计意图**（治「有验收测试而无真跑」假绿）保留，oracle 按双实现面重写：
//
//   实现面 A · firmware/（ESP-IDF C/C++）：验收 = on-target Unity 测试（test/ 或 components/*/test
//     下 *.acceptance.c/.cpp，实机/qemu 跑）∪ 端云协议验收脚本。oracle 检测：盘上存在冻结验收文件
//     但无对应可运行入口（acceptance runner 目录/脚本缺失）→ 红。**实机验收无法在本闸自动真跑**——
//     本闸只做「有验收文件 ⇒ 有验收入口」结构闸；实机跑由 acceptance 相位人工/台架触发，
//     证据回落 verifier-evidence（见 acceptance-evidence.md）。这是非纯软件项目的诚实降级：
//     本闸担保「不会静默跳过」，不担保「实机已过」。
//
//   实现面 B · server/（FastAPI shim）：验收 = pytest 标记 @pytest.mark.acceptance 或
//     server/**/acceptance/test_*.py。oracle：盘上存在 acceptance 测试但无 pytest runner → 红。
//     决策通过后执行 `python -m pytest -q -m acceptance`（或 acceptance 目录）并透传 exit。
//
// 红半径 = repo-wide 合并（merge-driver cwd=worktreePath 全仓跑）：任一面有验收文件未接入口阻塞合并。
//
// 结构化自检：`--self-check`（argv 触发 + isMain 守卫）跑决策电池 + 实盘 smoke。

import { existsSync, readFileSync, readdirSync, statSync } from 'node:fs';
import { resolve, join, relative } from 'node:path';
import { pathToFileURL } from 'node:url';
import { execSync } from 'node:child_process';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// 固件 on-target 验收文件（Unity，*.acceptance.c/.cpp）。
const FW_ACC_RE = /\.acceptance\.(c|cpp|cc)$/;
// shim pytest 验收文件（acceptance 目录下 test_*.py）。
const PY_ACC_RE = /(^|\/)acceptance\/(test_[^/]*|[^/]*_test)\.py$/;
const SKIP_DIRS = new Set(['node_modules', 'build', 'dist', 'coverage', '.git', 'managed_components', '__pycache__', 'ESP32-S3-RLCD-4.2-Demo']);

function print(payload) { console.log(JSON.stringify(payload, null, 2)); }
function fail(message, details = {}) { print({ ok: false, gate: 'acceptance', message, ...details }); process.exit(1); }

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

// 固件验收 runner 证据：firmware/acceptance/ 目录（on-target 测试工程）或 run_acceptance 脚本。
function hasFwAcceptanceRunner(fwDir) {
  if (!existsSync(fwDir)) return false;
  if (existsSync(join(fwDir, 'acceptance'))) return true;
  for (const f of ['run_acceptance.sh', 'run_acceptance.py']) {
    if (existsSync(join(fwDir, f))) return true;
  }
  return false;
}

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

// 纯决策（self-check 直调）：
//   input = { fwAccFiles, fwHasRunner, pyAccFiles, pyHasRunner }
// 有验收文件而无 runner → 红；全无 → 跳过绿（起步期合法，由 verifier 证据闸担纲「真跑过」）。
function decide(input) {
  const violations = [];
  if (input.fwAccFiles.length > 0 && !input.fwHasRunner) {
    violations.push({ surface: 'firmware', acceptanceFiles: input.fwAccFiles, issue: 'has-acceptance-files-no-runner' });
  }
  if (input.pyAccFiles.length > 0 && !input.pyHasRunner) {
    violations.push({ surface: 'server', acceptanceFiles: input.pyAccFiles, issue: 'has-acceptance-files-no-runner' });
  }
  if (violations.length > 0) return { ok: false, reason: 'has-acceptance-files-without-runner', violations };
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
    fwAccFiles: collectFiles(fwDir, FW_ACC_RE),
    fwHasRunner: hasFwAcceptanceRunner(fwDir),
    pyAccFiles: collectFiles(serverDir, PY_ACC_RE),
    pyHasRunner: hasPytestRunner(serverDir),
  };
}

function runSelfCheck() {
  const results = [];
  const ok = (name, cond, detail = '') => results.push({ name, ok: Boolean(cond), detail });

  const d1 = decide({ fwAccFiles: ['test/stroke.acceptance.c'], fwHasRunner: false, pyAccFiles: [], pyHasRunner: false });
  ok('state1-fw-files-no-runner-red', !d1.ok && d1.reason === 'has-acceptance-files-without-runner' && d1.violations[0].surface === 'firmware');

  const d2 = decide({ fwAccFiles: [], fwHasRunner: false, pyAccFiles: ['acceptance/test_shim_api.py'], pyHasRunner: false });
  ok('state2-py-files-no-runner-red', !d2.ok && d2.violations[0].surface === 'server');

  const d3 = decide({ fwAccFiles: ['test/stroke.acceptance.c'], fwHasRunner: true, pyAccFiles: ['acceptance/test_shim_api.py'], pyHasRunner: true });
  ok('state3-files-with-runner-green', d3.ok);

  const d4 = decide({ fwAccFiles: [], fwHasRunner: false, pyAccFiles: [], pyHasRunner: false });
  ok('state4-no-files-green', d4.ok);

  ok('state5-fw-glob', FW_ACC_RE.test('stroke.acceptance.c') && !FW_ACC_RE.test('stroke.c') && !FW_ACC_RE.test('stroke.test.c'));
  ok('state5-py-glob', PY_ACC_RE.test('acceptance/test_shim_api.py') && !PY_ACC_RE.test('test_store.py') && !PY_ACC_RE.test('shim/test_api.py'));

  try {
    const real = collectReal(projectDir);
    const dReal = decide(real);
    ok('real-repo-smoke', true, `fwAcc=${real.fwAccFiles.length} pyAcc=${real.pyAccFiles.length} ok=${dReal.ok}`);
  } catch (e) {
    ok('real-repo-smoke', false, `enumeration failed: ${e.message}`);
  }

  const allOk = results.every((r) => r.ok);
  process.stdout.write(`${JSON.stringify({ selfCheck: 'acceptance-run-check', ok: allOk, results }, null, 2)}\n`);
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
    fail('真空绿：以下面存在冻结验收文件却无对应验收入口（有验收而无真跑 = 静默假绿）',
      { reason: verdict.reason, violations: verdict.violations });
  }
  print({
    ok: true, gate: 'acceptance', phase: 'oracle-pass',
    surfaces: { firmwareAcceptanceFiles: real.fwAccFiles.length, serverAcceptanceFiles: real.pyAccFiles.length },
    note: '固件 on-target 验收需实机/qemu，由 acceptance 相位人工/台架触发并落 verifier-evidence；本闸担保不静默跳过，不担保实机已过。',
  });
  // 执行器：仅 server 面 host-fast 的 pytest acceptance 在闸内真跑；live on-target（@pytest.mark.live，如 cloud-link 验收）
  // 与固件 on-target 同待遇——只结构闸、不 auto-run（merge/CI 无 SSH/凭据，auto-run 必假红）。pytest 未装也 fail-open。
  if (real.pyHasRunner && real.pyAccFiles.length > 0) {
    const hostFiles = real.pyAccFiles.filter((f) => !fileHasLiveMark(join(projectDir, 'server', f)));
    if (hostFiles.length === 0) {
      print({ ok: true, gate: 'acceptance', phase: 'executor-skipped', reason: 'live-only on-target acceptance（@pytest.mark.live，手动触发；对偶固件 on-target 结构闸）' });
    } else if (!pytestAvailable()) {
      print({ ok: true, gate: 'acceptance', phase: 'executor-skipped', reason: 'pytest 未安装（fail-open，对偶 firmware idf.py 缺失）' });
    } else {
      try {
        execSync(`python -m pytest -q ${hostFiles.join(' ')}`, { cwd: join(projectDir, 'server'), stdio: 'inherit' });
      } catch (err) {
        process.exit(typeof err.status === 'number' ? err.status : 1);
      }
    }
  }
}
