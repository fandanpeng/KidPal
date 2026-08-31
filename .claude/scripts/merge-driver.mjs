#!/usr/bin/env node
// Merge Driver — merge token 的消费者（canonical consumer of the merge token）。
// 由 planner（dev-loop 单 Pod，--pod <changeId>）或主线程（Track B）在 supervisor APPROVE / architect GREEN
//   + 前置升级闸（pre-merge-escalation-gate.mjs）放行后自动触发；仍支持人工 CLI 手动运行。
// 串行化：按 mergeOrder 逐个 rebase → quick-check → merge → 释放 token → 下一个。
//
// owner=architect, approver=human (G3 script, see DEV-GOVERNANCE.md §6)
//
// 用法：
//   node .claude/scripts/merge-driver.mjs                 # 完整串行 merge
//   node .claude/scripts/merge-driver.mjs --dry-run       # 仅打印操作序列，不执行
//   node .claude/scripts/merge-driver.mjs --pod <changeId> # 只 merge 指定 Pod（调试用）
//   node .claude/scripts/merge-driver.mjs --human-approved-exemption <ref>  # acceptance 门红时按人类裁决豁免（须 existing-red-inventory.md 存在非空，仅豁免 acceptance 门）
//
// 环境变量：
//   DEVLOOP_TOKEN_TTL_SEC       同 portfolio-reconcile.mjs（默认 1800）
//   DEVLOOP_LOCK_STALE_SEC      同 portfolio-reconcile.mjs（默认 60）
//   MERGE_DRIVER_RETRY_MAX      quick-check 重试上限（默认 1）

import {
  existsSync,
  mkdirSync,
  readdirSync,
  readFileSync,
  rmdirSync,
  writeFileSync,
} from 'node:fs';
import { join, resolve, basename } from 'node:path';
import { execFileSync, execSync, spawnSync } from 'node:child_process';
import { setPod, reconcile } from './portfolio-reconcile.mjs';
import { computeRetroScope } from './loop-health-report.mjs';
import { shouldFireAdvisory } from './retro-staleness-check.mjs';

const RETRY_MAX = Number(process.env.MERGE_DRIVER_RETRY_MAX || 1);
const DRY_RUN = process.argv.includes('--dry-run');
const SINGLE_POD = (() => {
  const idx = process.argv.indexOf('--pod');
  return idx >= 0 ? process.argv[idx + 1] : null;
})();
const HUMAN_EXEMPTION = (() => {
  const idx = process.argv.indexOf('--human-approved-exemption');
  return idx >= 0 ? process.argv[idx + 1] : null;
})();

function log(msg) {
  process.stdout.write(`[merge-driver] ${msg}\n`);
}

function warn(msg) {
  process.stderr.write(`[merge-driver] WARN: ${msg}\n`);
}

function errorExit(msg) {
  process.stderr.write(`[merge-driver] FATAL: ${msg}\n`);
  process.exit(1);
}

function emitRetroStalenessAdvisory(
  projectDir,
  { computeScope = computeRetroScope, warnFn = warn, envThreshold = process.env.DEVLOOP_RETRO_STALE_THRESHOLD } = {},
) {
  try {
    const scope = computeScope(projectDir);
    if (!scope || !Array.isArray(scope.pendingAnalysis) || !Array.isArray(scope.waitingOnLoopHealth)) {
      throw new Error('computeRetroScope returned an invalid result shape');
    }

    const result = shouldFireAdvisory({
      pendingCount: scope.pendingAnalysis.length,
      waitingCount: scope.waitingOnLoopHealth.length,
      envThreshold,
    });

    for (const warning of result.warnings) warnFn(warning);
    if (!result.wouldFire) return result;

    warnFn('─── RETRO STALENESS ADVISORY ───');
    warnFn(`pending retro analysis: ${result.pendingCount}`);
    warnFn(`waiting on loop-health: ${result.waitingCount}`);
    warnFn(`combined: ${result.combined} (threshold: ${result.threshold}, source: ${result.thresholdSource})`);
    warnFn(`retro runs recorded: ${scope.retroRuns ?? 'unknown'}`);
    warnFn('pending remediation: run /loop-retro');
    warnFn('waiting remediation: owning integrator creates loop-health.md first');
    warnFn('────────────────────────────────');
    return result;
  } catch (error) {
    warnFn(`retro staleness check skipped (non-blocking): ${error.message}`);
    return null;
  }
}

function runRetroAdvisorySelfCheck() {
  const requireCheck = (condition, message) => {
    if (!condition) throw new Error(`retro advisory self-check failed: ${message}`);
  };

  for (const [label, computeScope] of [
    ['scope-throw', () => { throw new Error('injected scope failure'); }],
    ['malformed-scope', () => ({ pendingAnalysis: null, waitingOnLoopHealth: [] })],
  ]) {
    const warnings = [];
    const result = emitRetroStalenessAdvisory(process.cwd(), { computeScope, warnFn: (line) => warnings.push(line) });
    requireCheck(result === null, `${label} must return null`);
    requireCheck(warnings.length === 1 && warnings[0].includes('non-blocking'), `${label} must emit one non-blocking warning`);
  }

  const firedWarnings = [];
  const fired = emitRetroStalenessAdvisory(process.cwd(), {
    computeScope: () => ({ pendingAnalysis: Array(12), waitingOnLoopHealth: Array(29), retroRuns: 0 }),
    warnFn: (line) => firedWarnings.push(line),
  });
  requireCheck(fired?.wouldFire === true, 'combined 41 must fire at default threshold 10');
  requireCheck(firedWarnings.length <= 10, 'fired output must remain within 10 stderr lines');
  requireCheck(firedWarnings.some((line) => line.includes('/loop-retro')), 'pending remediation must be present');
  requireCheck(firedWarnings.some((line) => line.includes('integrator creates loop-health.md')), 'waiting remediation must be present');

  process.stdout.write(`${JSON.stringify({ selfCheck: 'merge-driver-retro-advisory', pass: 3, fail: 0, total: 3, maxWarningLines: firedWarnings.length, ok: true })}\n`);
}

// ---- ARQ-100：合并收口 detect_changes advisory（ref-based committed-diff 核对，advisory 非阻塞）----
// F1（fail-safe 承重墙）：整体 try/catch + catch-all 只 warn N/A、绝不重抛——processPod/main 调用处无外层 catch，
//   任何未捕获异常会 crash node → 非零退出 → 后续 ready Pod 全不处理 = 等效阻塞整批合并。对偶 emitRetroStalenessAdvisory catch 模式。
// F2：since 硬前提 + 假零守卫——无 since 不调用；changed_count==0 而 git diff 非空 → N/A（防虚报已核对，见 AGENTS.md「CodebaseMemory 使用纪律」节）。
// 降级一律 warn N/A、绝不 errorExit / setPod blocked / return false / 重抛（detect_changes 永不阻塞 merge、永不虚报影响面）。
const DETECT_CHANGES_BIN = 'codebase-memory-mcp';
const DETECT_CHANGES_PROJECT = 'C-AI-AInativecompany';
const DETECT_CHANGES_TIMEOUT_MS = Number(process.env.MERGE_DRIVER_DETECT_CHANGES_TIMEOUT_MS || 60000);
const DETECT_CHANGES_TOP_K = Number(process.env.MERGE_DRIVER_DETECT_CHANGES_TOP_K || 15);

function defaultDetectChangesCli(args) {
  // piped stdin（CLI 已弃用裸 JSON 位置参）。spawnSync ENOENT / 启动失败 → result.error。
  const res = spawnSync(DETECT_CHANGES_BIN, ['cli', '--json', 'detect_changes'], {
    input: JSON.stringify(args),
    encoding: 'utf8',
    timeout: DETECT_CHANGES_TIMEOUT_MS,
    stdio: ['pipe', 'pipe', 'pipe'],
  });
  if (res.error) return { error: res.error.message };
  if (res.status !== 0 && !res.stdout) return { error: `exit ${res.status}` };
  let outer;
  try { outer = JSON.parse(res.stdout); } catch { return { result: null }; }
  if (outer && outer.isError) return { isError: 'cli reported isError' };
  let result = outer && outer.structuredContent;
  if (!result && outer && outer.content && outer.content[0] && outer.content[0].text) {
    try { result = JSON.parse(outer.content[0].text); } catch { result = null; }
  }
  return { result: result || null };
}

function defaultGitDiffStat(wt, since) {
  try {
    const out = execSync(`git diff --stat ${since}..main`, { cwd: wt, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    return { nonEmpty: out.trim().length > 0 };
  } catch (error) {
    return { error: error.message };
  }
}

function defaultWriteDetectChangesCloseout(changeId, payload) {
  // best-effort 写 changes/dev/<changeId>/detect-changes-advisory.json；失败抛（被 F1 catch-all 接住，不阻塞 merge）。
  const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
  const dir = join(projectDir, 'changes', 'dev', changeId);
  if (!existsSync(dir)) return; // closeout 目录不存在（未晋升 changes/dev）→ 静默跳过
  writeFileSync(
    join(dir, 'detect-changes-advisory.json'),
    `${JSON.stringify({ changeId, generatedBy: 'merge-driver detect_changes advisory (ARQ-100)', ...payload }, null, 2)}\n`,
  );
}

function runDetectChangesAdvisory(
  wt,
  changeId,
  preMergeSha,
  {
    runCli = defaultDetectChangesCli,
    gitDiffStat = defaultGitDiffStat,
    writeCloseout = defaultWriteDetectChangesCloseout,
    warnFn = warn,
    logFn = log,
    topK = DETECT_CHANGES_TOP_K,
  } = {},
) {
  try {
    // F2 since 硬前提：无基准 ref 不调用（无 since 的 detect_changes 产假零）。
    if (!preMergeSha) {
      warnFn('detect_changes: N/A (no since baseline) — advisory skipped');
      return { ok: false, reason: 'no-since' };
    }
    const cli = runCli({ project: DETECT_CHANGES_PROJECT, since: preMergeSha });
    if (cli.error) {
      warnFn(`detect_changes: N/A (CLI unavailable: ${cli.error}) — advisory skipped`);
      return { ok: false, reason: 'cli-unavailable' };
    }
    if (cli.timeout) {
      warnFn('detect_changes: N/A (timeout) — advisory skipped');
      return { ok: false, reason: 'timeout' };
    }
    if (cli.isError) {
      warnFn(`detect_changes: N/A (index error: ${cli.isError}) — advisory skipped`);
      return { ok: false, reason: 'index-error' };
    }
    const parsed = cli.result; // {changed_files, changed_count, impacted_symbols, depth}
    if (!parsed || typeof parsed.changed_count !== 'number' || Number.isNaN(parsed.changed_count)) {
      warnFn('detect_changes: N/A (unparseable output) — advisory skipped');
      return { ok: false, reason: 'unparseable' };
    }
    // F2 假零守卫：changed_count==0 而 git diff 非空 → N/A（非「0」），防虚报已核对。
    if (parsed.changed_count === 0) {
      const diffStat = gitDiffStat(wt, preMergeSha);
      if (diffStat && diffStat.error) {
        warnFn(`detect_changes: N/A (cross-check unavailable: ${diffStat.error}) — advisory skipped`);
        return { ok: false, reason: 'cross-check-unavailable' };
      }
      if (diffStat && diffStat.nonEmpty) {
        warnFn('detect_changes: N/A (false-zero: diff non-empty but changed_count=0) — advisory skipped');
        return { ok: false, reason: 'false-zero' };
      }
    }
    const total = parsed.changed_count;
    const symbols = Array.isArray(parsed.impacted_symbols) ? parsed.impacted_symbols : [];
    const top = symbols.slice(0, topK).map((s) => (typeof s === 'string' ? s : (s && (s.qualified_name || s.name)) || JSON.stringify(s)));
    logFn(`detect_changes: changed_count=${total}, impacted_symbols=${symbols.length} (top${topK}: ${top.join(', ') || 'none'})`);
    writeCloseout(changeId, { changed_count: total, impacted_symbols_total: symbols.length, impacted_symbols_top: top }); // F5：入 F1 守卫
    return { ok: true, changedCount: total, impactedSymbols: symbols.length };
  } catch (error) {
    // F1 catch-all：绝不重抛。
    warnFn(`detect_changes: N/A (unexpected: ${error.message}) — advisory skipped`);
    return { ok: false, reason: 'unexpected' };
  }
}

function runDetectChangesSelfCheck() {
  const requireCheck = (condition, message) => {
    if (!condition) throw new Error(`detect_changes self-check failed: ${message}`);
  };
  const noop = () => {};
  let pass = 0;
  let fail = 0;
  const check = (cond, msg) => { if (cond) { pass++; } else { fail++; } requireCheck(cond, msg); };

  // 1. 正常案：changed_count>0 → 报数 + top-K 摘要 + ok。
  {
    const lines = [];
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
      runCli: () => ({ result: { changed_count: 114, impacted_symbols: Array.from({ length: 556 }, (_, i) => `sym${i}`), depth: 2 } }),
      gitDiffStat: () => ({ nonEmpty: true }),
      writeCloseout: noop,
      warnFn: (l) => lines.push(l), logFn: (l) => lines.push(l), topK: 15,
    });
    check(r.ok === true && r.changedCount === 114 && r.impactedSymbols === 556, 'normal case must report 114/556');
    check(lines.some((l) => l.includes('changed_count=114') && l.includes('top15')), 'normal case must log top-K summary');
  }
  // 2. since 硬前提（F2）：无 preMergeSha → N/A，且不调用 CLI。
  {
    let called = false; const lines = [];
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', null, {
      runCli: () => { called = true; return { result: { changed_count: 0 } }; },
      gitDiffStat: () => ({ nonEmpty: false }), writeCloseout: noop,
      warnFn: (l) => lines.push(l), logFn: (l) => lines.push(l),
    });
    check(r.ok === false && r.reason === 'no-since' && called === false, 'no-since must skip CLI and return N/A');
    check(lines.some((l) => l.includes('N/A (no since baseline)')), 'no-since must warn N/A');
  }
  // 3. 假零案（F2/F6）：changed_count=0 但 diff 非空 → N/A(false-zero)，非「0」。
  {
    const lines = [];
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
      runCli: () => ({ result: { changed_count: 0, impacted_symbols: [] } }),
      gitDiffStat: () => ({ nonEmpty: true }), writeCloseout: noop,
      warnFn: (l) => lines.push(l), logFn: (l) => lines.push(l),
    });
    check(r.ok === false && r.reason === 'false-zero', 'false-zero must return N/A not 0');
    check(lines.some((l) => l.includes('false-zero')), 'false-zero must warn false-zero');
  }
  // 4. CLI 缺失（result.error）→ N/A。
  {
    const lines = [];
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
      runCli: () => ({ error: 'spawn ENOENT' }), gitDiffStat: () => ({ nonEmpty: false }), writeCloseout: noop,
      warnFn: (l) => lines.push(l), logFn: (l) => lines.push(l),
    });
    check(r.ok === false && r.reason === 'cli-unavailable' && lines.some((l) => l.includes('CLI unavailable')), 'CLI error must N/A');
  }
  // 5. timeout → N/A。
  {
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
      runCli: () => ({ timeout: true }), gitDiffStat: () => ({ nonEmpty: false }), writeCloseout: noop, warnFn: noop, logFn: noop,
    });
    check(r.ok === false && r.reason === 'timeout', 'timeout must N/A');
  }
  // 6. isError → N/A。
  {
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
      runCli: () => ({ isError: 'boom' }), gitDiffStat: () => ({ nonEmpty: false }), writeCloseout: noop, warnFn: noop, logFn: noop,
    });
    check(r.ok === false && r.reason === 'index-error', 'isError must N/A');
  }
  // 7. unparseable → N/A。
  {
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
      runCli: () => ({ result: null }), gitDiffStat: () => ({ nonEmpty: false }), writeCloseout: noop, warnFn: noop, logFn: noop,
    });
    check(r.ok === false && r.reason === 'unparseable', 'unparseable must N/A');
  }
  // 8. catch-all 不重抛（F1/F6）：writeCloseout 抛异常 → 被捕获、N/A(unexpected)、绝不重抛。
  {
    const lines = [];
    let threw = false;
    let r;
    try {
      r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
        runCli: () => ({ result: { changed_count: 5, impacted_symbols: ['a'] } }),
        gitDiffStat: () => ({ nonEmpty: true }),
        writeCloseout: () => { throw new Error('injected closeout write failure'); },
        warnFn: (l) => lines.push(l), logFn: (l) => lines.push(l),
      });
    } catch { threw = true; }
    check(threw === false, 'catch-all must not rethrow');
    check(r.ok === false && r.reason === 'unexpected', 'closeout throw must yield N/A(unexpected)');
    check(lines.some((l) => l.includes('unexpected')), 'catch-all must warn unexpected');
  }
  // 9. gitDiffStat 命令失败（T4a）→ cross-check-unavailable，不冒充「报 0」。
  {
    const lines = [];
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
      runCli: () => ({ result: { changed_count: 0, impacted_symbols: [] } }),
      gitDiffStat: () => ({ error: 'git failed' }), writeCloseout: noop,
      warnFn: (l) => lines.push(l), logFn: (l) => lines.push(l),
    });
    check(r.ok === false && r.reason === 'cross-check-unavailable', 'gitDiffStat error must yield cross-check-unavailable');
    check(lines.some((l) => l.includes('cross-check unavailable')), 'must warn cross-check unavailable');
  }
  // 10. NaN changed_count（T4b）→ unparseable（typeof NaN==='number' 边角）。
  {
    const r = runDetectChangesAdvisory('/fake-wt', 'test-id', 'abc123', {
      runCli: () => ({ result: { changed_count: NaN, impacted_symbols: [] } }),
      gitDiffStat: () => ({ nonEmpty: false }), writeCloseout: noop, warnFn: noop, logFn: noop,
    });
    check(r.ok === false && r.reason === 'unparseable', 'NaN changed_count must yield unparseable');
  }

  process.stdout.write(`${JSON.stringify({ selfCheck: 'merge-driver-detect-changes', pass, fail, total: pass + fail, ok: fail === 0 })}\n`);
}

// 读 board.json 并确保是最新状态（无条件 reconcile）。
function freshBoard(projectDir) {
  log('reconciling board...');
  const res = reconcile(projectDir);
  if (!res || !res.board) errorExit('reconcile failed: ' + JSON.stringify(res));
  return { board: res.board, devloopDir: res.devloopDir };
}

// 读单个 pod 文件。
function readPod(devloopDir, changeId) {
  const p = join(devloopDir, 'pods', `${changeId}.json`);
  if (!existsSync(p)) return null;
  try {
    return JSON.parse(readFileSync(p, 'utf8'));
  } catch {
    return null;
  }
}

// 找到 Pod 的 worktree 路径。
// 优先用 pod 文件中的 worktreePath；fallback: 扫描 git worktree list 并匹配 .claude/taskpackages/<changeId>/。
function findWorktree(pod, projectDir) {
  if (pod && pod.worktreePath) {
    if (existsSync(pod.worktreePath)) return pod.worktreePath;
    warn(`worktreePath ${pod.worktreePath} not found; falling back to git worktree scan`);
  }

  const changeId = pod ? pod.changeId : null;
  if (!changeId) return null;

  // Fallback: scan git worktree list
  try {
    const out = execSync('git worktree list --porcelain', { cwd: projectDir, encoding: 'utf8' }).trim();
    const lines = out.split('\n');
    let currentPath = null;
    for (const line of lines) {
      if (line.startsWith('worktree ')) {
        currentPath = line.slice('worktree '.length);
        // Check if this worktree has the matching taskpackage
        const tp = join(currentPath, '.claude', 'taskpackages', changeId, 'taskpackage.json');
        if (existsSync(tp)) return currentPath;
      }
    }
  } catch {
    // fall through
  }
  return null;
}

// Quick-check gates: structural + acceptance（rebase 后冒烟闸）。
// ARQ-097 收敛：命令不再硬编码，改从 verify-gates.json（主仓 canonical 源）按显式 id 表取——
// 单一源、保当前执行序、保四门冒烟子集（全量门套已由 verifier 于 ready-to-merge 前跑，不可误扩全门）。
const QUICK_CHECK_IDS = ['types', 'schema', 'contract-lint', 'acceptance'];

// F4 字面 test oracle（非执行源）：自检断言「读源→选 id→保序」结果恒等于此，
// 守 verify-gates.json 自身漂移（ARQ-097 真病根）。不可改成对 verify-gates.json 自比（那恒真）。
const EXPECTED_QUICK_CHECK_CMDS = [
  // 本仓：无 tsc / prisma 等价物——结构闸走 idf.py build（固件编译即类型闸），契约闸走 contract-lint（本仓契约面 docs/contracts/** 的 markdown 协议检查脚本，档 2 新写）。
  'node .claude/scripts/contract-lint.mjs',
  'node .claude/scripts/acceptance-run-check.mjs',
];

// 忠实 port verify.mjs loadGates()：从进程所在仓（主 worktree）读 verify-gates.json。
function loadGates(projectDir) {
  const p = resolve(projectDir, '.claude/verify-gates.json');
  return JSON.parse(readFileSync(p, 'utf8').replace(/^\uFEFF/, ''));
}

// 按 QUICK_CHECK_IDS 表序从 gates 选门（不可 filter-json 自然序：json 原生序 schema→types 颠倒）。
function buildQuickChecks(projectDir) {
  const gates = loadGates(projectDir).gates || [];
  const byId = new Map(gates.map((g) => [g.id, g]));
  return QUICK_CHECK_IDS.map((id) => {
    const gate = byId.get(id);
    if (!gate || typeof gate.cmd !== 'string') errorExit(`verify-gates.json missing quick-check gate id="${id}" (ARQ-097 single source)`);
    return { id: gate.id, cmd: gate.cmd, kind: gate.kind };
  });
}

// D1（architect F3 第三案）：主仓为 canonical 源；若被检 worktree 亦有 verify-gates.json 且
// 所选四门 cmd 与主仓分歧 → 高声 warn（refuse-or-surface，不静默择一）。G3 冻结下分歧近恒异常。
function warnWorktreeGateDivergence(worktreePath, mainQuickChecks) {
  try {
    const wtGates = loadGates(worktreePath).gates || [];
    const wtById = new Map(wtGates.map((g) => [g.id, g]));
    const diverged = mainQuickChecks.filter((g) => {
      const wt = wtById.get(g.id);
      return wt && wt.cmd !== g.cmd;
    });
    if (diverged.length) {
      warn(`verify-gates.json DIVERGENCE between main repo and worktree ${worktreePath} (G3 should be frozen):`);
      diverged.forEach((g) => warn(`  gate=${g.id} main="${g.cmd}" worktree="${wtById.get(g.id).cmd}"`));
      warn('using MAIN REPO canonical commands; inspect the worktree G3 change before trusting this merge.');
    }
  } catch {
    // worktree 无 verify-gates.json（或不可读）→ 无分歧可言，用主仓源即可。
  }
}

// ARQ-114 / F4 结构化自检：读源→选 id→保序 的结果 == EXPECTED_QUICK_CHECK_CMDS 字面 oracle。
function runQuickChecksSelfCheck(projectDir) {
  const qc = buildQuickChecks(projectDir);
  const gotCmds = qc.map((g) => g.cmd);
  const gotIds = qc.map((g) => g.id);
  const ok = JSON.stringify(gotCmds) === JSON.stringify(EXPECTED_QUICK_CHECK_CMDS)
    && JSON.stringify(gotIds) === JSON.stringify(QUICK_CHECK_IDS);
  process.stdout.write(`${JSON.stringify({ selfCheck: 'merge-driver-quick-checks', ok, expectedCmds: EXPECTED_QUICK_CHECK_CMDS, gotCmds, expectedIds: QUICK_CHECK_IDS, gotIds })}\n`);
  if (!ok) process.exit(1);
}

// 在 worktree 中跑一次 quick check。返回 { ok, failingGate, logTail }。
function runQuickCheck(worktreePath, quickChecks) {
  for (const gate of quickChecks) {
    try {
      execSync(gate.cmd, { cwd: worktreePath, encoding: 'utf8', stdio: 'pipe' });
    } catch (err) {
      const out = (err.stdout || '') + (err.stderr || '');
      return {
        ok: false,
        failingGate: gate.id,
        logTail: out.split('\n').slice(-20).join('\n'),
      };
    }
  }
  return { ok: true, failingGate: null, logTail: null };
}

// ---- ARQ-260820-merge-driver-origin-main-stale：rebase 目标改「解析最新基线 ref」（fail-closed）----
// 根因：硬编码 `git rebase origin/main` 在本地工作流（agent 不 push、origin 陈旧）下恒撞过期基线。
// 改法（260823-gov-review-merge-driver-rebase-target T1，human OQ-1=(a)/OQ-2=(a) + R1-F1/F2）：
//   显式解析最新合并基线——本地工作流选本地 main（消除陈旧 origin 假基线）；远端领先切 origin（多机/已 push）；
//   单 ref 缺失降级到存活 ref + 高声 warn（覆盖合法 fresh-clone 冷启动，不错杀）；分叉/俱缺 fail-closed blocked。
// 判据向严不减：选不到合法基线 ref 时绝不放行到错基线。quick-check 门集与合并语义零改。
//
// 返回 { ok:true, ref, reason } 或 { ok:false, reason }。纯函数式：git 调用经 deps 注入，便于自检。
function resolveRebaseTarget({ revParse, isAncestor } = {}) {
  const tryRevParse = (ref) => {
    try {
      revParse(ref);
      return true;
    } catch {
      return false;
    }
  };
  const tryIsAncestor = (a, b) => {
    try {
      isAncestor(a, b);
      return true;
    } catch {
      return false;
    }
  };

  const mainExists = tryRevParse('main');
  const originExists = tryRevParse('origin/main');

  if (mainExists && originExists) {
    // 两 ref 俱存：origin 是否领先本地 main？
    const originAheadOfMain = tryIsAncestor('main', 'origin/main') && !tryIsAncestor('origin/main', 'main');
    if (originAheadOfMain) {
      // origin 领先（多机/已 push，远端有本地未合并提交）→ 切远端基线。
      return { ok: true, ref: 'origin/main', reason: 'remote-ahead' };
    }
    const originBehindOrEqual = tryIsAncestor('origin/main', 'main');
    if (originBehindOrEqual) {
      // origin 不领先（祖先或相等）→ 本地工作流主路径，用本地 main。
      return { ok: true, ref: 'main', reason: 'local' };
    }
    // 互非祖先 = 分叉，fail-closed。
    return { ok: false, reason: 'diverged' };
  }
  if (!mainExists && originExists) {
    // 本地 main 缺失（fresh-clone 冷启动）→ 存活 ref + warn，不错杀。
    return { ok: true, ref: 'origin/main', reason: 'local-main-missing' };
  }
  if (mainExists && !originExists) {
    // origin 缺失（无远端/离线）→ 存活 ref + warn。
    return { ok: true, ref: 'main', reason: 'origin-missing' };
  }
  // 两 ref 俱缺。
  return { ok: false, reason: 'both-missing' };
}

// 生产用薄壳：真 git 调用注入 resolveRebaseTarget。
function resolveRebaseTargetLive(cwd) {
  return resolveRebaseTarget({
    revParse: (ref) => execSync(`git rev-parse --verify ${ref}`, { cwd, encoding: 'utf8', stdio: 'pipe' }),
    isAncestor: (a, b) => execSync(`git merge-base --is-ancestor ${a} ${b}`, { cwd, encoding: 'utf8', stdio: 'pipe' }),
  });
}

// 自检：六分岔全覆盖（两俱存×3 + 单缺×2 + 俱缺×1）。驱动 resolveRebaseTarget 纯逻辑（注入假 git）。
function runRebaseTargetSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (condition, message) => {
    if (condition) {
      pass++;
    } else {
      fail++;
      process.stderr.write(`[merge-driver] rebase-target self-check FAIL: ${message}\n`);
    }
  };
  // mkDeps：exists 集合 + ancestor 真值表（key=`${a}|${b}`）。
  const mkDeps = ({ main = true, origin = true, ancestor = {} } = {}) => ({
    revParse: (ref) => {
      const ok = ref === 'main' ? main : origin;
      if (!ok) throw new Error(`no such ref ${ref}`);
    },
    isAncestor: (a, b) => {
      if (ancestor[`${a}|${b}`]) return;
      throw new Error(`${a} not ancestor of ${b}`);
    },
  });

  // 1. 两俱存·origin 落后本地（origin/main 是 main 祖先）→ 本地 main。
  {
    const r = resolveRebaseTarget(mkDeps({ ancestor: { 'origin/main|main': true } }));
    check(r.ok && r.ref === 'main' && r.reason === 'local', 'origin-behind → local main');
  }
  // 2. 两俱存·origin == main（互为祖先）→ 本地 main。
  {
    const r = resolveRebaseTarget(mkDeps({ ancestor: { 'origin/main|main': true, 'main|origin/main': true } }));
    check(r.ok && r.ref === 'main' && r.reason === 'local', 'origin-equal → local main');
  }
  // 3. 两俱存·origin 领先本地（main 是 origin 祖先、origin 非 main 祖先）→ origin/main。
  {
    const r = resolveRebaseTarget(mkDeps({ ancestor: { 'main|origin/main': true } }));
    check(r.ok && r.ref === 'origin/main' && r.reason === 'remote-ahead', 'origin-ahead → origin/main');
  }
  // 4. 两俱存·分叉（互非祖先）→ blocked diverged。
  {
    const r = resolveRebaseTarget(mkDeps({ ancestor: {} }));
    check(!r.ok && r.reason === 'diverged', 'diverged → blocked');
  }
  // 5. 单缺·本地 main 缺失 → origin/main + warn 理由。
  {
    const r = resolveRebaseTarget(mkDeps({ main: false, origin: true }));
    check(r.ok && r.ref === 'origin/main' && r.reason === 'local-main-missing', 'local-main-missing → origin/main');
  }
  // 6. 单缺·origin 缺失 → 本地 main + warn 理由。
  {
    const r = resolveRebaseTarget(mkDeps({ main: true, origin: false }));
    check(r.ok && r.ref === 'main' && r.reason === 'origin-missing', 'origin-missing → local main');
  }
  // 7. 两俱缺 → blocked both-missing。
  {
    const r = resolveRebaseTarget(mkDeps({ main: false, origin: false }));
    check(!r.ok && r.reason === 'both-missing', 'both-missing → blocked');
  }

  process.stdout.write(`${JSON.stringify({ selfCheck: 'merge-driver-rebase-target', pass, fail, total: pass + fail, ok: fail === 0 })}\n`);
  if (fail > 0) process.exit(1);
}


// 处理一个 Pod 的 merge 操作。返回 true=成功 merged，false=跳过或失败。
function processPod(changeId, devloopDir, projectDir, quickChecks) {
  const pod = readPod(devloopDir, changeId);
  if (!pod) {
    warn(`Pod ${changeId} not found in devloop/pods/; skipping`);
    return false;
  }

  // Step 1: 找 worktree（先于标记 merging，供前置升级闸取源 ref）
  const wt = findWorktree(pod, projectDir);
  if (!wt) {
    errorExit(`${changeId}: cannot locate worktree. Set worktreePath in pod file or create worktree.`);
  }
  log(`${changeId}: worktree at ${wt}`);
  warnWorktreeGateDivergence(wt, quickChecks); // ARQ-097 D1：主仓 canonical + worktree 分歧高声 warn

  // Step 1b: 前置升级闸（合并前必经；命中 → blocked 升级人，不 rebase、不 merge）
  log(`${changeId}: running pre-merge escalation gate...`);
  if (!DRY_RUN) {
    try {
      execFileSync(
        'node',
        [join(projectDir, '.claude', 'scripts', 'pre-merge-escalation-gate.mjs'), changeId, '--source-ref', 'HEAD', '--project-dir', projectDir],
        { cwd: wt, encoding: 'utf8', stdio: 'pipe' },
      );
      log(`${changeId}: escalation gate PASS`);
    } catch (err) {
      const out = `${err.stdout || ''}${err.stderr || ''}`;
      warn(`${changeId}: escalation gate BLOCK:\n${out.split('\n').slice(-12).join('\n')}`);
      setPod(projectDir, { changeId, status: 'blocked' });
      return false;
    }
  } else {
    log(`${changeId}: escalation gate skipped (DRY RUN)`);
  }

  // Step 1c: 标记 merging
  log(`${changeId}: marking merging`);
  if (!DRY_RUN) setPod(projectDir, { changeId, status: 'merging' });

  // Step 3: fetch + rebase
  log(`${changeId}: fetching origin...`);
  if (!DRY_RUN) {
    try {
      execSync('git fetch origin', { cwd: wt, encoding: 'utf8', stdio: 'pipe' });
    } catch (err) {
      warn(`${changeId}: git fetch failed: ${(err.stderr || '')}`);
      setPod(projectDir, { changeId, status: 'blocked' });
      return false;
    }

    // ARQ-260820：rebase 目标改「解析最新基线 ref」（本地 main 优先、origin 领先切 origin、单缺降级存活 ref、分叉/俱缺 fail-closed）。
    const target = resolveRebaseTargetLive(wt);
    if (!target.ok) {
      warn(`${changeId}: rebase target unresolved (${target.reason}) — fail-closed, human adjudication required`);
      setPod(projectDir, { changeId, status: 'blocked' });
      return false;
    }
    log(`${changeId}: rebase target = ${target.ref} (${target.reason})`);
    if (target.reason === 'local-main-missing' || target.reason === 'origin-missing') {
      warn(`${changeId}: single-ref fallback — ${target.reason}, rebasing onto surviving ref ${target.ref}`);
    }
    try {
      execSync(`git rebase ${target.ref}`, { cwd: wt, encoding: 'utf8', stdio: 'pipe' });
    } catch (err) {
      const stderr = err.stderr || '';
      warn(`${changeId}: rebase conflict — aborting rebase`);
      log(`${changeId}: ${stderr.split('\n').slice(-10).join('\n')}`);
      try { execSync('git rebase --abort', { cwd: wt, encoding: 'utf8', stdio: 'pipe' }); } catch { /* race: nothing to abort */ }
      setPod(projectDir, { changeId, status: 'blocked' });
      return false;
    }
  }
  log(`${changeId}: rebase OK`);

  // Step 4: quick check（rebase 冲突 → blocked; quick-check 失败 → 重试一次，再失败 → blocked）
  // 豁免例外（ARQ-260823-red-exemption T1）：acceptance 门红 + --human-approved-exemption <ref> + existing-red-inventory.md 存在非空
  // → 按人类裁决放行该门（其余 types/schema/contract-lint 已过、照拦）；制品不在场或非 acceptance 门红 → 不豁免。
  let qcOk = false;
  for (let attempt = 0; attempt <= RETRY_MAX; attempt++) {
    if (attempt > 0) log(`${changeId}: quick-check retry ${attempt}/${RETRY_MAX}`);
    const qc = runQuickCheck(wt, quickChecks);
    if (qc.ok) { qcOk = true; break; }
    if (qc.failingGate === 'acceptance' && HUMAN_EXEMPTION !== null) {
      // 豁免判据（shape-only，对齐 supervisor-gate-check G6）：人类 token（flag）+ 预存红分类制品存在非空。
      const inventory = join(projectDir, 'changes', 'dev', changeId, 'existing-red-inventory.md');
      let exemptByHuman = false;
      try {
        exemptByHuman = existsSync(inventory) && readFileSync(inventory, 'utf8').trim().length > 0;
      } catch {
        exemptByHuman = false;
      }
      if (exemptByHuman) {
        log(`${changeId}: human-approved exemption (ref=${HUMAN_EXEMPTION}) — acceptance gate red + existing-red-inventory.md present; proceeding on human ruling`);
        warn(`${changeId}: acceptance gate exempted by human ruling (ref=${HUMAN_EXEMPTION}) — types/schema/contract-lint already passed and remain enforced`);
        qcOk = true;
        break;
      }
      warn(`${changeId}: --human-approved-exemption given but existing-red-inventory.md absent/empty — refusing exemption`);
    }
    if (attempt === RETRY_MAX) {
      warn(`${changeId}: quick-check FAILED after ${RETRY_MAX + 1} attempt(s) — gate=${qc.failingGate}`);
      log(`${changeId}: logTail:\n${qc.logTail}`);
      setPod(projectDir, { changeId, status: 'blocked' });
      return false;
    }
    warn(`${changeId}: quick-check failed (gate=${qc.failingGate}), retrying...`);
  }
  if (!qcOk) return false; // 不可达（retry loop 已设 blocked），但保留防御
  log(`${changeId}: quick-check OK (all ${quickChecks.length} gates)`);

  // ARQ-100：合并前基准 ref（detect_changes since 前提，F3：Step 5 前捕获）。不可得 → advisory 降级 N/A，不影响 merge。
  let preMergeSha = null;
  if (!DRY_RUN) {
    try {
      preMergeSha = execSync('git rev-parse main', { cwd: wt, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim();
    } catch { preMergeSha = null; }
  }

  // Step 5: git merge
  log(`${changeId}: merging to main...`);
  if (!DRY_RUN) {
    try {
      // Get the current branch name, then merge it into main
      const branch = execSync('git rev-parse --abbrev-ref HEAD', { cwd: wt, encoding: 'utf8' }).trim();
      if (branch === 'main' || branch === 'master') {
        warn(`${changeId}: already on ${branch}, skipping git merge`);
      } else {
        // Checkout main in the worktree, merge the feature branch
        try {
          execSync(`git checkout main && git merge --no-ff ${branch} -m "Merge ${changeId} (merge-driver)"`, {
            cwd: wt,
            encoding: 'utf8',
            stdio: 'pipe',
          });
        } catch (mergeErr) {
          const merr = mergeErr.stderr || '';
          warn(`${changeId}: git merge failed: ${merr.split('\n').slice(-5).join('\n')}`);
          warn(`${changeId}: aborting merge and marking blocked. Resolve conflicts manually.`);
          try {
            execSync('git merge --abort', { cwd: wt, encoding: 'utf8', stdio: 'pipe' });
          } catch { /* no merge in progress */ }
          try {
            execSync('git checkout -', { cwd: wt, encoding: 'utf8', stdio: 'pipe' });
          } catch { /* already on branch */ }
          setPod(projectDir, { changeId, status: 'blocked' });
          return false;
        }
      }
    } catch (err) {
      warn(`${changeId}: branch detection failed: ${(err.stderr || '')}`);
      setPod(projectDir, { changeId, status: 'blocked' });
      return false;
    }
  }
  log(`${changeId}: merge OK`);

  // ARQ-100：detect_changes advisory（ref-based committed-diff 核对，advisory 非阻塞，降级 N/A，绝不阻塞 merge）。
  if (!DRY_RUN) runDetectChangesAdvisory(wt, changeId, preMergeSha);
  else log(`${changeId}: detect_changes advisory skipped (DRY RUN; would run detect_changes since=${preMergeSha || '<no baseline>'})`);

  // Step 6: 标记 merged
  if (!DRY_RUN) setPod(projectDir, { changeId, status: 'merged' });
  log(`${changeId}: DONE`);

  // Step 7: reconcile 释放 token
  if (!DRY_RUN) {
    const res = reconcile(projectDir);
    if (res && res.board && res.board.mergeToken) {
      log(`token released, next holder: ${res.board.mergeToken.holder || 'none'}`);
    }
  }

  return true;
}

// ---- Main ----
function main() {
  const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
  if (process.argv.includes('--self-check-retro-advisory')) {
    runRetroAdvisorySelfCheck();
    return;
  }
  if (process.argv.includes('--self-check-quick-checks')) {
    runQuickChecksSelfCheck(projectDir);
    return;
  }
  if (process.argv.includes('--self-check-detect-changes')) {
    runDetectChangesSelfCheck();
    return;
  }
  if (process.argv.includes('--self-check-rebase-target')) {
    runRebaseTargetSelfCheck();
    return;
  }
  const quickChecks = buildQuickChecks(projectDir); // ARQ-097：verify-gates.json 单源
  if (DRY_RUN) log('DRY RUN — no changes will be made');

  const { board, devloopDir } = freshBoard(projectDir);

  log(`board: ${board.podCount} pods, wip=${JSON.stringify(board.wip)}, mergeToken.holder=${board.mergeToken?.holder || 'none'}`);

  if (board.alerts && board.alerts.length) {
    board.alerts.forEach((a) => warn(`alert: ${a.kind} (${a.changeId}) — ${a.reason}`));
  }

  emitRetroStalenessAdvisory(projectDir);

  // Collect ready-to-merge pods
  const ready = board.pods
    .filter((p) => p.status === 'ready-to-merge')
    .sort((a, b) => a.mergeOrder - b.mergeOrder);

  if (!ready.length) {
    log('no ready-to-merge pods; nothing to do');
    process.exit(0);
  }

  log(`${ready.length} pod(s) ready to merge: ${ready.map((p) => `${p.changeId}(#${p.mergeOrder})`).join(', ')}`);

  // 单 Pod 模式
  if (SINGLE_POD) {
    const changeId = SINGLE_POD;
    const pod = ready.find((p) => p.changeId === changeId);
    if (!pod) errorExit(`pod ${changeId} is not in ready-to-merge state`);
    const ok = processPod(changeId, devloopDir, projectDir, quickChecks);
    process.exit(ok ? 0 : 1);
  }

  // 全量模式: 按 mergeOrder 逐 Pod 处理
  let merged = 0;
  let skipped = 0;
  for (const pod of ready) {
    log(`--- Processing ${pod.changeId} (mergeOrder=${pod.mergeOrder}) ---`);
    const ok = processPod(pod.changeId, devloopDir, projectDir, quickChecks);
    if (ok) merged++;
    else skipped++;
  }
  log(`SUMMARY: ${merged} merged, ${skipped} skipped/blocked, ${ready.length} total ready`);
  process.exit(skipped > 0 ? 1 : 0);
}

main();
