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
const SKIP_DIRS = new Set(['node_modules', 'build', 'dist', 'coverage', '.git', 'managed_components', '__pycache__', 'ESP32-S3-RLCD-4.2-Demo', 'archive']);

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
  // 260904（human 授权直改，afd31ac 先例）：`.acceptance.c` 冻结验收由 INF-A4 新约定 runner
  //   main/test/host_app/ 执行（ELF 实体跑 host_infra_smoke.acceptance.c），以 host_app sdkconfig.defaults 判据。
  const hostAppSdk = join(fwDir, 'main', 'test', 'host_app', 'sdkconfig.defaults');
  if (existsSync(hostAppSdk) && /CONFIG_IDF_TARGET_LINUX=y/.test(readFileSync(hostAppSdk, 'utf8'))) return true;
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

// live 标记统计（报告用；260909 T1 起不再是排除判据——排除改由 pytest `-m "not live"` 条目级筛）。
function fileHasLiveMark(absPath) {
  try {
    return /@pytest\.mark\.live|pytestmark\s*=\s*pytest\.mark\.live/.test(readFileSync(absPath, 'utf8'));
  } catch {
    return false; // 读不到按无标记计（仅统计用途；执行面不再有「一标记全屏蔽」路径）
  }
}

// pytest 是否可运行：未装 fail-open（对偶 firmware 工具链缺失降级）。
function pytestAvailable() {
  try { execSync('python -m pytest --version', { stdio: 'ignore' }); return true; } catch { return false; }
}

// 条目级两态收集（260909-gov-review-gate-truthfulness T1+T2）：`-m "not live"` 由 pytest 按标记筛除
// live 面，`--import-mode=importlib` 与包 redBaselineCommand 同源（点分 .acceptance.py 文件名在默认
// prepend 模式下必收集失败）。返回 { structuralCollected, liveDeselected }；收集异常（import 错误等，
// exit 非 0/5）返回 null → 交真实执行暴露，不向噪也不掩错。
function pytestCollectNotLive(files) {
  const cmd = `python -m pytest -q --collect-only --import-mode=importlib -m "not live" ${files.join(' ')}`;
  try {
    const out = execSync(cmd, { cwd: join(projectDir, 'server'), encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    return parseCollectSummary(out);
  } catch (err) {
    if (err.status === 5) return parseCollectSummary(`${err.stdout || ''}\n${err.stderr || ''}`);
    return null;
  }
}

// -q --collect-only 摘要形态（2026-09-09 实测）：混合件「14/18 tests collected (4 deselected)」；
// 全 deselect「no tests collected (18 deselected)」（exit 0）。判据 = collected 数字，不接受没报错就算过。
function parseCollectSummary(out) {
  const deselected = (out.match(/(\d+) deselected/) || [])[1];
  const m = out.match(/(\d+)\/(\d+) tests? collected/) || out.match(/(\d+) tests? collected/);
  return { structuralCollected: m ? Number(m[1]) : 0, liveDeselected: deselected ? Number(deselected) : 0 };
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

  // 260909 T1：条目级筛选摘要解析（判据 = collected/deselected 数字；实盘真跑断言由验证集 V2 承载）。
  const mix = parseCollectSummary('14/18 tests collected (4 deselected) in 0.06s');
  ok('mixed-file-structural-runs', mix.structuralCollected === 14 && mix.liveDeselected === 4);
  const allLive = parseCollectSummary('no tests collected (18 deselected) in 0.05s');
  ok('all-live-deselected-reported', allLive.structuralCollected === 0 && allLive.liveDeselected === 18);

  try {
    const real = collectReal(projectDir);
    const dReal = decide(real);
    ok('real-repo-smoke', true, `fwAcc=${real.fwAccFiles.length} pyAcc=${real.pyAccFiles.length} ok=${dReal.ok}`);
    // ARQ-260916-acc-gate-archive-skew（260918-gov-review-closeout-chain-friction T1①，FROZEN 2026-09-19）：
    // 归档退役面（*/archive/）不入枚举——镜像 unit 闸 `real-repo-no-archive-collected` 同款断言，
    // 使两闸退役口径同步（「移位即退役」语义）。任一面枚举命中 archive 路径即红，防 SKIP_DIRS 回退静默复发。
    const archiveHits = [...real.fwAccFiles, ...real.pyAccFiles].filter((f) => /(^|\/)archive\//.test(f));
    ok('real-repo-no-archive-collected', archiveHits.length === 0, archiveHits.join(',') || 'no-archive-in-enumeration');
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

// ── scope-file（260928-gov-review-dev-loop-preassess-manifest-scope T9，与 unit-run-check 同款）──
// final 相位按影响面圈定跑。红：phase=final 缺 scope-file（举手不退全量）；scope-file 引用文件
// 盘上全不存在（防 typo 假绿）。交集空 = 诚实绿跳过。scoped / 无 phase 直跑不消费，行为不变。
function parseScopeFileArg(argv) {
  const i = argv.indexOf('--scope-file');
  if (i === -1) return null;
  const v = argv[i + 1];
  if (!v || v.startsWith('--')) fail('--scope-file 需要路径参数（fail-loud）');
  return resolve(projectDir, v);
}

function readScopeList(p) {
  try {
    return readFileSync(p, 'utf8').split(/\r?\n/).map((l) => l.trim()).filter((l) => l && !l.startsWith('#'));
  } catch {
    fail(`scope-file 不可读（fail-closed）: ${p}`);
    return [];
  }
}

function scopeFilter(collected, surface, scopeList) {
  const set = new Set(scopeList.map((l) => l.replace(/\\/g, '/')));
  return collected.filter((f) => set.has(`${surface}/${f}`) || set.has(f));
}

function applyScope(real, scopeList) {
  const onDisk = scopeList.filter((l) => existsSync(join(projectDir, l)));
  if (scopeList.length > 0 && onDisk.length === 0) {
    fail('scope-file 引用文件在盘上全部不存在（防 typo 假绿，fail-closed）', { scopeFileEntries: scopeList.length });
  }
  real.fwAccFiles = scopeFilter(real.fwAccFiles, 'firmware', scopeList);
  real.pyAccFiles = scopeFilter(real.pyAccFiles, 'server', scopeList);
  return real;
}

if (isMain() && process.argv.includes('--self-check')) runSelfCheck();

if (isMain() && !process.argv.includes('--self-check')) {
  const scopePath = parseScopeFileArg(process.argv);
  const scopeList = scopePath ? readScopeList(scopePath) : null;
  const phase = (process.argv.find((a) => a.startsWith('--phase=')) || '').split('=')[1] || null;
  if (phase === 'final' && !scopeList) {
    fail('final 相位缺 --scope-file（红闸举手，260928 批 T9 fail-closed）：范围 = 影响面圈定清单（detect_changes + impactedTests 种子合成，integrator Step 4）；全量回归 = 人工触发位。缺清单/圈定失败 → 升级 human，不默认全量');
  }
  let real;
  try {
    real = collectReal(projectDir);
  } catch (e) {
    fail('oracle 枚举失败（fail-safe 向严：不吞成绿）', { error: e.message });
  }
  if (scopeList) applyScope(real, scopeList);
  if (scopeList && real.fwAccFiles.length === 0 && real.pyAccFiles.length === 0) {
    print({ ok: true, gate: 'acceptance', phase: 'executor-skipped', reason: 'scope 内 0 条可跑（交集空 = 诚实绿跳过，对偶 allLiveDeselected 绿）', scopeFile: scopePath });
    process.exit(0);
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
  // 执行器（260909-gov-review-gate-truthfulness T1+T2）：server 面 host 验收在闸内真跑；host/live 两态
  // 在**条目级**判——`-m "not live"` 由 pytest 按标记筛除 live 面（@pytest.mark.live，实机/on-target 不
  // auto-run，对偶固件 on-target 结构闸），禁按文件级标记整份排除（病灶实录：server/acceptance/ 9/9
  // 混合件曾被文件级判据整份放行）。执行口径与包 redBaselineCommand 同源（--import-mode=importlib）。
  // 全为 live 面由执行结果给出：collect 后 0 条可跑 → 显式 allLiveDeselected（非静默 skip）。
  // pytest 未装 fail-open。约定成文：docs/DEV-GOVERNANCE.md「## 验收测试纪律」§1。
  if (real.pyHasRunner && real.pyAccFiles.length > 0) {
    if (!pytestAvailable()) {
      print({ ok: true, gate: 'acceptance', phase: 'executor-skipped', reason: 'pytest 未安装（fail-open，对偶 firmware 工具链缺失降级）' });
    } else {
      const files = real.pyAccFiles;
      const filesWithLiveMark = files.filter((f) => fileHasLiveMark(join(projectDir, 'server', f))).length;
      const collect = pytestCollectNotLive(files);
      if (collect && collect.structuralCollected === 0) {
        print({ ok: true, gate: 'acceptance', phase: 'executor-skipped', allLiveDeselected: true,
          structuralCollected: 0, liveDeselected: collect.liveDeselected, filesWithLiveMark: `${filesWithLiveMark}/${files.length}`,
          reason: `全部为 live 面（-m "not live" 后 0 条可跑，deselected ${collect.liveDeselected} 条；live on-target 手动触发）` });
      } else {
        if (collect) {
          print({ ok: true, gate: 'acceptance', phase: 'executor-run',
            structuralCollected: collect.structuralCollected, liveDeselected: collect.liveDeselected,
            filesWithLiveMark: `${filesWithLiveMark}/${files.length}` });
        }
        try {
          execSync(`python -m pytest -q --import-mode=importlib -m "not live" ${files.join(' ')}`, { cwd: join(projectDir, 'server'), stdio: 'inherit' });
        } catch (err) {
          process.exit(typeof err.status === 'number' ? err.status : 1);
        }
      }
    }
  }
}
