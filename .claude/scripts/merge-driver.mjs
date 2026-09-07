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
//   node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>  # ESCALATE 人批合并：透传给前置升级闸核验 pod-events 人批事件，凭据进 merge commit message
//
// 环境变量：
//   DEVLOOP_TOKEN_TTL_SEC       同 portfolio-reconcile.mjs（默认 1800）
//   DEVLOOP_LOCK_STALE_SEC      同 portfolio-reconcile.mjs（默认 60）
//   MERGE_DRIVER_RETRY_MAX      quick-check 重试上限（默认 1）

import {
  appendFileSync,
  existsSync,
  mkdirSync,
  mkdtempSync,
  readdirSync,
  readFileSync,
  rmdirSync,
  rmSync,
  writeFileSync,
} from 'node:fs';
import { dirname, join, resolve, basename } from 'node:path';
import { tmpdir } from 'node:os';
import { randomUUID } from 'node:crypto';
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
// T2（260904-gov-review-escalate-merge-approval）：ESCALATE 人批合并凭据，透传给前置升级闸核验
const HUMAN_APPROVAL_REF = (() => {
  const idx = process.argv.indexOf('--human-approval-ref');
  return idx >= 0 ? process.argv[idx + 1] ?? null : null;
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

// Quick-check gates: contract-lint + acceptance（rebase 后冒烟闸）。
// ARQ-097 收敛：命令不再硬编码，改从 verify-gates.json（主仓 canonical 源）按显式 id 表取——
// 单一源、保当前执行序、保冒烟子集（全量门套已由 verifier 于 ready-to-merge 前跑，不可误扩全门）。
// 260905 对齐（260905-gov-review-verify-gates-align T1，OQ1=(a) 两门案）：QUICK_CHECK_IDS 由源仓
// 4 门残留（types/schema/contract-lint/acceptance）改为本仓 2 门——firmware-build（types+schema 折算物）
// 有意不入冒烟：真跑 idf.py build 分钟级不「quick」、无工具链 fail-open、verifier scoped 相位已承担；
// 冒烟保留「契约结构 + 验收入口」两个快门。
const QUICK_CHECK_IDS = ['contract-lint', 'acceptance'];

// F4 字面 test oracle（非执行源）：自检断言「读源→选 id→保序」结果恒等于此，
// 守 verify-gates.json 自身漂移（ARQ-097 真病根）。不可改成对 verify-gates.json 自比（那恒真）。
// 260905 对齐：QUICK_CHECK_IDS 由源仓 4 门残留改为本仓 2 门，本 oracle（迁移时即 2 条）未动。
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

// ---- Worktree 拓扑两步（T5 提取，ARQ-260905-merge-driver-worktree-topology）----
// 2026-09-05 human 裁定热修的核心语义，原内联于 processPod；提取为具名函数后，
// 主流程与 --self-check-worktree-topology 共用同一实现（self-check 不再复述逻辑，防假绿）。

// fetch 步谓词：有 origin remote 才 fetch。无 origin = 本地-only worktree → 跳过（不 fail-closed）。
function hasOriginRemote(cwd) {
  try {
    execSync('git remote get-url origin', { cwd, encoding: 'utf8', stdio: 'pipe' });
    return true;
  } catch {
    return false;
  }
}

// 定位持有 refs/heads/main 的 worktree 路径（git worktree list --porcelain 解析）。
// 无主 / 解析失败 → null（调用方回退原 cwd）。语义同原内联实现（含 try/catch 回退）。
function findMainHolderWorktree(cwd) {
  try {
    const wtList = execSync('git worktree list --porcelain', { cwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    for (const block of wtList.split('\n\n')) {
      const lines = block.split('\n');
      const wpath = (lines.find((l) => l.startsWith('worktree ')) || '').replace(/^worktree /, '');
      const wbranch = (lines.find((l) => l.startsWith('branch ')) || '').replace(/^branch /, '');
      if (wpath && wbranch === 'refs/heads/main') return wpath;
    }
  } catch { /* 解析失败：回退 */ }
  return null;
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

// 自检（T5，ARQ-260905-merge-driver-worktree-topology）：临时 git 仓库夹具（脚本自建、跑完即删）
// 断言拓扑两步语义——① 无 origin remote 时 fetch 步跳过（谓词为 false）且 rebase 目标解析仍落本地 main
// （不 fail-closed；有 origin 时谓词翻转）；② git worktree list --porcelain 解析能正确定位持有 main 的 worktree
// （从 feature worktree 找 = 主夹具路径；从主夹具找 = 自身）。驱动主流程同一实现 hasOriginRemote /
// findMainHolderWorktree / resolveRebaseTargetLive，非复述逻辑。
function runWorktreeTopologySelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (condition, message) => {
    if (condition) {
      pass++;
    } else {
      fail++;
      process.stderr.write(`[merge-driver] worktree-topology self-check FAIL: ${message}\n`);
    }
  };
  const tmp = mkdtempSync(join(tmpdir(), 'md-wt-topo-'));
  const featureWt = join(dirname(tmp), `${basename(tmp)}-feature`);
  // git worktree list --porcelain 输出正斜杠路径（Windows 实测 C:/...），node path 为反斜杠——比对前归一。
  const norm = (p) => (p || '').replace(/\\/g, '/');
  const run = (cmd, cwd) => execSync(cmd, { cwd, encoding: 'utf8', stdio: 'pipe' });
  try {
    run('git init', tmp);
    run('git symbolic-ref HEAD refs/heads/main', tmp); // 不依赖 init.defaultBranch 配置
    run('git -c user.name=selfcheck -c user.email=selfcheck@local commit --allow-empty -m init', tmp);

    // ① 无 origin：fetch 跳过谓词 false + 目标解析落本地 main（不 fail-closed；origin 缺失 reason=origin-missing 单缺降级语义）。
    check(hasOriginRemote(tmp) === false, 'no-origin repo must report hasOriginRemote=false (fetch-skip predicate)');
    const t1 = resolveRebaseTargetLive(tmp);
    check(t1.ok && t1.ref === 'main' && (t1.reason === 'local' || t1.reason === 'origin-missing'),
      `no-origin rebase target must resolve to local main (got ok=${t1.ok} ref=${t1.ref} reason=${t1.reason})`);
    // ①b 有 origin（仅 add 不 fetch）：谓词翻转 → fetch 步将执行。
    run('git remote add origin https://example.invalid/selfcheck.git', tmp);
    check(hasOriginRemote(tmp) === true, 'origin-present repo must report hasOriginRemote=true');

    // ② linked worktree 持 feature 分支，主夹具持 main：从 feature wt 定位主夹具。
    run(`git worktree add "${featureWt}" -b feature`, tmp);
    check(norm(findMainHolderWorktree(featureWt)) === norm(tmp), 'main holder from feature worktree must be the main checkout');
    check(norm(findMainHolderWorktree(tmp)) === norm(tmp), 'main holder from main checkout must be itself');

    // ③ T3（260907 批）：ensureGraphCacheSynced 内容级还原两形态——worktree 漂移（包 2 合并实证）+ staged 漂移（renormalize 实况）。
    const gdir = join(tmp, '.codebase-memory');
    mkdirSync(gdir, { recursive: true });
    writeFileSync(join(gdir, 'graph.db.zst'), 'head-snapshot-graph', 'utf8');
    writeFileSync(join(gdir, 'artifact.json'), '{"head":true}', 'utf8');
    run('git add .codebase-memory', tmp);
    run('git -c user.name=selfcheck -c user.email=selfcheck@local commit -m graph', tmp);
    // 形态一：worktree 内容漂移 + 旗标在册 → 还原内容 + 复旗标
    writeFileSync(join(gdir, 'graph.db.zst'), 'drifted-by-indexer', 'utf8');
    run('git update-index --skip-worktree .codebase-memory/graph.db.zst .codebase-memory/artifact.json', tmp);
    ensureGraphCacheSynced(tmp, 'selfcheck');
    check(readFileSync(join(gdir, 'graph.db.zst'), 'utf8') === 'head-snapshot-graph', 'worktree content drift must be restored to HEAD snapshot');
    check(/^[Ss] \.codebase-memory\/graph\.db\.zst$/m.test(run('git ls-files -v .codebase-memory', tmp)), 'skip-worktree flag must be re-set after content-drift sync');
    // 形态二：staged 漂移（renormalize 实况 = 旗标清 + 内容入 staging）→ index+worktree 双还原、净树
    run('git update-index --no-skip-worktree .codebase-memory/graph.db.zst .codebase-memory/artifact.json', tmp);
    writeFileSync(join(gdir, 'artifact.json'), '{"drifted":true}', 'utf8');
    run('git add .codebase-memory/artifact.json', tmp);
    ensureGraphCacheSynced(tmp, 'selfcheck');
    check(run('git status --porcelain', tmp).trim() === '', 'staged drift must be cleared (index+worktree both back to HEAD)');
    check(/^[Ss] \.codebase-memory\/artifact\.json$/m.test(run('git ls-files -v .codebase-memory', tmp)), 'skip-worktree flag must be re-set after staged-drift sync');
  } catch (err) {
    fail++;
    const detail = `${err.stderr || err.message || err}`.trim().split('\n').slice(-3).join(' ');
    process.stderr.write(`[merge-driver] worktree-topology self-check FAIL: fixture error: ${detail}\n`);
  } finally {
    try { execSync(`git worktree remove --force "${featureWt}"`, { cwd: tmp, encoding: 'utf8', stdio: 'pipe' }); } catch { /* 已删或不存在 */ }
    try { rmSync(featureWt, { recursive: true, force: true }); } catch { /* 临时残留 advisory */ }
    try { rmSync(tmp, { recursive: true, force: true }); } catch { /* 临时残留 advisory */ }
  }
  process.stdout.write(`${JSON.stringify({ selfCheck: 'merge-driver-worktree-topology', pass, fail, total: pass + fail, ok: fail === 0 })}\n`);
  if (fail > 0) process.exit(1);
}

// ---- T6（260906-gov-review-arq-governance-batch，ARQ-260906-live-graph-dirty-blocks-merge 根治）----
// 活图索引器（主 checkout + 各 worktree 专属进程）持续重写这两件被跟踪文件，git 树常驻脏，
// rebase/merge 步无条件 fail-closed。白名单固定两件（非模式匹配），幂等设 skip-worktree、永不 unset
//（unset 即复发）。只动本地 index 标记、不改历史、幂等可重入 → 置于 if (!DRY_RUN) 守卫之外
//（R1-F9 钉死：DRY_RUN 下照常执行并留痕，dry-run 验证路径真实覆盖该步）。
// ---- T3（260907-gov-review-arq-governance-batch-2，ARQ-260907-renormalize-clears-skip-worktree 根治）----
// 升内容级（ensureGraphCacheSynced）：260829-voice-roundtrip 合并实证——旗标完好（S 在、
// `0 set, 2 already flagged`）仍被挡：图索引自动刷新使两件内容与 HEAD 漂移，rebase 内部 checkout 按真实
// 字节比对拒绝（skip-worktree 不豁免内容比对）。幂等三拍：① S 在册先 no-skip-worktree；② `git checkout HEAD --`
// 还原内容（index 与工作树双还原——裸 `checkout --` 只还原 worktree←index，覆盖不了 renormalize 把内容
// 带入 staging 的实况，R1-F2）；③ 复设 skip-worktree。图缓存自刷新、无数据丢失面。stderr 留痕
// `N content-restored, M flag-only`，失败 advisory-only。**操作序注记**：`git add --renormalize .` 会清
// skip-worktree 旗标（同 ARQ 证据一）——凡跑 renormalize 后依赖本函数幂等复位，旗标与内容（含 staging）双兜底。
// 另：rebase 中断会在 rebase-merge/done 堆积重复 pick，手动续 rebase 须先清 todo（`git rebase --abort` 或
// 编辑 .git/rebase-merge/done），本脚本不改 rebase 逻辑本体。
const CODEBASE_MEMORY_DIRTY_PATHS = ['.codebase-memory/graph.db.zst', '.codebase-memory/artifact.json'];

// 对 cwd 仓库幂等同步两件活图缓存到 HEAD 快照并复设 skip-worktree（T3 内容级语义）。
// 成功路径 stderr 留痕一行；ls-files / update-index / checkout 失败走 warn advisory——不阻塞合并（修复型助手，
// 非门语义变更）。ls-files -v 大小写因 git 版本而异（S/s 均算已设），故按小写判等。
function ensureGraphCacheSynced(cwd, changeId) {
  let listed;
  try {
    listed = execSync(`git ls-files -v -- ${CODEBASE_MEMORY_DIRTY_PATHS.join(' ')}`, { cwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
  } catch (err) {
    warn(`${changeId}: 活图同步前置 ls-files 失败（advisory，不阻塞合并）：${`${err.stderr || ''}`.trim().split('\n').slice(-3).join(' ')}`);
    return;
  }
  const flagged = new Set();
  for (const line of listed.split(/\r?\n/)) {
    const m = /^([A-Za-z?]) +(.+)$/.exec(line);
    if (m && m[1].toLowerCase() === 's') flagged.add(m[2]);
  }
  let restoredCount = 0;
  let flagOnlyCount = 0;
  for (const p of CODEBASE_MEMORY_DIRTY_PATHS) {
    const wasFlagged = flagged.has(p);
    try {
      if (wasFlagged) {
        // ① 撤旗标——skip-worktree 在册时 checkout/update-index 对该路径的常规操作被 git 忽略，须先撤
        execSync(`git update-index --no-skip-worktree -- ${p}`, { cwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
      }
      // ② 内容级还原（HEAD 快照，index + worktree 双面）
      execSync(`git checkout HEAD -- ${p}`, { cwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
      restoredCount++;
      if (!wasFlagged) flagOnlyCount++;
    } catch (err) {
      warn(`${changeId}: 活图缓存还原失败 ${p}（advisory，不阻塞合并）：${`${err.stderr || ''}`.trim().split('\n').slice(-3).join(' ')}`);
    }
    // ③ 复旗标（无论②成败——旗标是常驻态，两件恒保持 S；失败时 git 自身的脏树门仍兜底 fail-closed）
    try {
      execSync(`git update-index --skip-worktree -- ${p}`, { cwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    } catch (err) {
      warn(`${changeId}: skip-worktree 复位失败 ${p}（advisory，不阻塞合并）：${`${err.stderr || ''}`.trim().split('\n').slice(-3).join(' ')}`);
    }
  }
  process.stderr.write(`[merge-driver] ${changeId}: graph cache synced in ${cwd} — ${restoredCount} content-restored, ${flagOnlyCount} flag-only (live graph drift guard, ARQ-260906 + 260907 T3)\n`);
}

// ---- Step 0 治理面自动收口（T2，260907-gov-review-merge-friction）----
// P2：pod-retro / supervisor-verdict / pod-events 追加等收尾制品产生于 integrator「最终」commit 之后，
// 无人 commit 则 rebase 步死于 unstaged。若本 worktree unstaged/untracked 全部落在本 Pod 治理面白名单
// （.claude/taskpackages/<pod-id>/** + changes/dev/<pod-id>/**）内 → 自动 add 精确路径 + chore commit；
// 任一文件白名单外 → warn 留痕并保持 fail 行为（rebase 步 fail-closed，绝不吞非治理面变更）。
function governanceFaceAutocommit(wt, changeId) {
  let out = '';
  try {
    out = execSync('git status --porcelain -uall', { cwd: wt, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
  } catch (err) {
    warn(`${changeId}: Step 0 status 探测失败（保持 fail 行为）：${`${err.stderr || ''}`.trim().split('\n').slice(-3).join(' ')}`);
    return false;
  }
  const lines = out.split('\n').filter((l) => l.trim());
  if (!lines.length) return true;
  // 白名单 <pod-id> 目录段精确等值判定（R1-F6）：changeId 不得含路径分隔符/保留名，
  // 防畸形 --pod 参数把 current/ 或他 Pod 目录拼进白名单。
  if (/[\\/]/.test(changeId) || changeId === 'current' || changeId === '.' || changeId === '..') {
    warn(`${changeId}: Step 0 拒绝——changeId 形状非法（含路径分隔符/保留名），白名单不生效`);
    return false;
  }
  const prefixes = [
    `.claude/taskpackages/${changeId}/`,
    `changes/dev/${changeId}/`,
  ];
  const inWhitelist = (p) => prefixes.some((pre) => p === pre.slice(0, -1) || p.startsWith(pre));
  const targets = [];
  const offenders = [];
  for (const line of lines) {
    const m = line.match(/^..\s+(.+)$/);
    if (!m) { offenders.push(line.trim()); continue; }
    let path = m[1];
    if (path.includes(' -> ')) path = path.split(' -> ').pop(); // rename 取新路径
    path = path.replace(/^"(.*)"$/, '$1');
    (inWhitelist(path) ? targets : offenders).push(path);
  }
  if (offenders.length) {
    warn(`${changeId}: Step 0 未收口——白名单外变更在场（保持 fail 行为，不吞非治理面变更）: ${offenders.slice(0, 8).join(', ')}${offenders.length > 8 ? ` 等 ${offenders.length} 项` : ''}`);
    return false;
  }
  try {
    execFileSync('git', ['add', '--', ...targets], { cwd: wt, stdio: ['ignore', 'pipe', 'pipe'] });
    execFileSync('git', ['commit', '-m', `chore(devloop): ${changeId} merge 前治理面收口——${targets.length} 文件（Step 0 白名单自动收口）`, '--', ...targets], { cwd: wt, stdio: ['ignore', 'pipe', 'pipe'] });
    log(`${changeId}: Step 0 治理面收口 commit 完成（${targets.length} 文件）`);
    return true;
  } catch (err) {
    warn(`${changeId}: Step 0 自动收口 commit 失败（保持 fail 行为）：${`${err.stderr || ''}`.trim().split('\n').slice(-5).join(' ')}`);
    return false;
  }
}

// ---- 投影冲突 --theirs 特判（T3，260907-gov-review-merge-friction）----
// current/ 投影为「最后写者赢」派生状态，双 Pod 近距合并 rebase 必冲突（P3），解法恒为被合并包赢。
// rebase 语境 theirs = 被重放的 Pod 侧（方向 R1 亲核）。冲突文件集含任一非特判文件 → 返回 false，
// 调用方整体 abort + blocked（特判不部分生效，R1-F4）；重放多 commit 冲突循环消解（R1-F4）。
const PROJECTION_CONFLICT_FILES = [
  '.claude/taskpackages/current/taskpackage.json',
  '.claude/taskpackages/current/verdict.json',
];
const PROJECTION_CONFLICT_MAX_ROUNDS = 16;
function tryResolveProjectionConflicts(wt, changeId) {
  for (let round = 1; round <= PROJECTION_CONFLICT_MAX_ROUNDS; round++) {
    let out = '';
    try {
      out = execSync('git diff --name-only --diff-filter=U', { cwd: wt, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    } catch {
      return false; // 探测失败按不可解处理 → 调用方 abort（fail-closed）
    }
    const conflicts = out.split('\n').map((s) => s.trim()).filter(Boolean);
    if (!conflicts.length) return false; // 未进入预期冲突态 → fail-closed 交回调用方 abort
    const nonSpecial = conflicts.filter((f) => !PROJECTION_CONFLICT_FILES.includes(f));
    if (nonSpecial.length) {
      warn(`${changeId}: rebase 冲突含非特判文件，投影特判不部分生效: ${nonSpecial.slice(0, 8).join(', ')}`);
      return false;
    }
    for (const f of conflicts) {
      execFileSync('git', ['checkout', '--theirs', '--', f], { cwd: wt, stdio: ['ignore', 'pipe', 'pipe'] }); // rebase 语境 theirs = 被重放的 Pod 侧
      execFileSync('git', ['add', '--', f], { cwd: wt, stdio: ['ignore', 'pipe', 'pipe'] });
    }
    warn(`${changeId}: 投影冲突特判 --theirs（被合并包赢，round ${round}）: ${conflicts.join(', ')}`);
    lastProjectionResolution = `投影冲突 --theirs ×${conflicts.length}（round ${round}）`;
    try {
      execSync('git rebase --continue', {
        cwd: wt,
        encoding: 'utf8',
        stdio: ['ignore', 'pipe', 'pipe'],
        env: { ...process.env, GIT_EDITOR: 'true' }, // 免交互（R1-F4）
      });
      return true; // rebase 完成
    } catch {
      continue; // 重放下一个 commit 再冲突 → 下一轮
    }
  }
  warn(`${changeId}: 投影冲突循环消解超出轮次上限（${PROJECTION_CONFLICT_MAX_ROUNDS}），按不可解处理`);
  return false;
}

// 投影冲突消解留痕（T6 payload 冲突消解摘要成分，本批 D2）：tryResolveProjectionConflicts 写、step5bBookkeeping 读。
let lastProjectionResolution = null;

// ---- Step 5b 具名实现（T6，260907-gov-review-merge-friction · OQ2 终裁 (c)）----
// merged 事件行落 main 簿记：merge 已成事实 → 簿记失败一律 advisory warn，绝不 errorExit / 未捕获抛出
// （对偶 detect-changes F1 承重墙，R1-F3）；commit pathspec 限定本 Pod pod-events 单文件，
// 防夹带 staged graph 残留（R1-F3）。
function step5bBookkeeping(mergeCwd, changeId, { mode } = {}) {
  try {
    if (/[\\/]/.test(changeId) || changeId === 'current' || changeId === '.' || changeId === '..') {
      throw new Error(`changeId 形状非法（${changeId}），簿记跳过`);
    }
    const evRel = `.claude/taskpackages/${changeId}/pod-events.jsonl`;
    const evAbs = join(mergeCwd, ...evRel.split('/'));
    const mergeSha = execSync('git rev-parse HEAD', { cwd: mergeCwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim();
    const event = {
      schemaVersion: 1,
      eventId: randomUUID(),
      changeId,
      timestamp: new Date().toISOString(),
      eventType: 'merge-completed',
      actor: 'merge-driver',
      source: 'merge-driver',
      payload: {
        summary: `merge-driver Step 5b 簿记：main tip ${mergeSha.slice(0, 12)}（${mode || (SINGLE_POD ? 'single' : 'board')} 模式）${HUMAN_APPROVAL_REF ? `；human-approval-ref=${HUMAN_APPROVAL_REF}` : ''}${lastProjectionResolution ? `；${lastProjectionResolution}` : ''}`,
      },
    };
    appendFileSync(evAbs, `${JSON.stringify(event)}\n`, 'utf8');
    execFileSync('git', ['add', '--', evRel], { cwd: mergeCwd, stdio: ['ignore', 'pipe', 'pipe'] });
    execFileSync('git', ['commit', '-m', `chore(devloop): ${changeId} merge 完成事件入 pod-events（merged ${mergeSha.slice(0, 12)}）`, '--', evRel], { cwd: mergeCwd, stdio: ['ignore', 'pipe', 'pipe'] });
    log(`${changeId}: Step 5b 簿记落 main（pod-events merge-completed @${mergeSha.slice(0, 12)}）`);
  } catch (err) {
    warn(`${changeId}: Step 5b 簿记失败（advisory，不阻塞合并链）：${`${err.stderr || err.message || ''}`.trim().split('\n').slice(-3).join(' ')}`);
  }
}

// ---- 人批豁免消解（T1，260907-gov-review-merge-friction）——兑现 planner.md:96 OQ2=(a) 既有承诺 ----
// 单 Pod 模式 + --human-approval-ref 且目标 Pod 状态 = blocked：以同 Step 1b 透传路径 spawn 前置升级闸
// 核验人批事件（gate 脚本顶层直跑故不可 import，spawn 即「同 Step 1b 透传路径」的字面复用）。
// 核验通过 → setPod(ready-to-merge) + reconcile + 刷新内存 board 视图（R1-F1：freshBoard 快照不回写，
// 漏此步则 T1 仍 no-op）；核验失败 fail-closed 保持 blocked 并报因。runner/refreshBoard 可注入供 self-check。
function approvalRefUnlock(projectDir, board, { changeId, humanRef, runner, refreshBoard, reconcileFn = reconcile, warnFn = warn } = {}) {
  const tp = (board.pods || []).find((p) => p.changeId === changeId);
  if (!tp || tp.status !== 'blocked') return 'not-applicable';
  const runGate = runner || (() => {
    const probeWt = findWorktree(tp, projectDir) || projectDir;
    const gateArgs = [join(projectDir, '.claude', 'scripts', 'pre-merge-escalation-gate.mjs'), changeId, '--source-ref', 'HEAD', '--project-dir', projectDir, '--human-approval-ref', humanRef];
    execFileSync('node', gateArgs, { cwd: probeWt, encoding: 'utf8', stdio: 'pipe' });
  });
  try {
    runGate();
  } catch (err) {
    const probeOut = `${err.stdout || ''}${err.stderr || ''}`.trim().split('\n').slice(-8).join('\n');
    warnFn(`${changeId}: approval-ref 核验失败，保持 blocked（fail-closed）：\n${probeOut}`);
    return 'kept-blocked';
  }
  setPod(projectDir, { changeId, status: 'ready-to-merge' });
  reconcileFn(projectDir);
  const refreshed = refreshBoard ? refreshBoard(projectDir) : freshBoard(projectDir).board;
  board.pods = refreshed.pods;
  warnFn(`${changeId}: approval-ref 解锁 blocked → ready-to-merge (ref=${humanRef})`);
  return 'unlocked';
}

// ---- 自检（T1/T2/T3/T4/T6 夹具，260907-gov-review-merge-friction）----
// 临时 git 仓夹具（脚本自建、跑完即删，对齐 worktree-topology 自检先例）。只测本批新增纯机械面：
// T1 用注入 runner/reconcileFn/refreshBoard（五要素核验本体归 gate 自检管辖，此处测解锁机制）；
// T2/T3/T4/T6 在临时仓真实跑 git（status/rebase/union/pathspec commit）。
function runMergeFrictionSelfCheck() {
  const results = [];
  const assert = (name, cond, detail = '') => results.push({ name, pass: !!cond, detail });
  const git = (cwd, args, opts = {}) =>
    execFileSync('git', args, { cwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'], ...opts }).trim();
  const mkRepo = (name) => {
    const repo = join(tmpdir(), `merge-driver-friction-${name}`);
    rmSync(repo, { recursive: true, force: true });
    mkdirSync(repo, { recursive: true });
    git(repo, ['init', '-b', 'main']);
    git(repo, ['config', 'user.email', 'self-check@example.local']);
    git(repo, ['config', 'user.name', 'merge-driver self-check']);
    return repo;
  };

  // ---- T1：人批豁免消解机制（注入 runner）----
  {
    const t1 = mkRepo('t1');
    try {
      setPod(t1, { changeId: '260901-t1p', status: 'blocked' });
      const boardA = { pods: [{ changeId: '260901-t1p', status: 'blocked' }] };
      const rA = approvalRefUnlock(t1, boardA, {
        changeId: '260901-t1p',
        humanRef: 'self-check-event',
        runner: () => {},
        reconcileFn: () => ({}),
        refreshBoard: () => ({ pods: [{ changeId: '260901-t1p', status: 'ready-to-merge' }] }),
        warnFn: () => {},
      });
      const fileA = JSON.parse(readFileSync(join(t1, '.git', 'devloop', 'pods', '260901-t1p.json'), 'utf8'));
      assert('T1a 有效 ref → 解锁 + 内存 board 刷新 + pod 文件 ready-to-merge',
        rA === 'unlocked' && boardA.pods[0].status === 'ready-to-merge' && fileA.status === 'ready-to-merge',
        `r=${rA} board=${boardA.pods[0].status} file=${fileA.status}`);
      setPod(t1, { changeId: '260901-t1q', status: 'blocked' });
      const boardB = { pods: [{ changeId: '260901-t1q', status: 'blocked' }] };
      const rB = approvalRefUnlock(t1, boardB, {
        changeId: '260901-t1q',
        humanRef: 'self-check-event',
        runner: () => { throw Object.assign(new Error('gate block'), { stderr: '五要素不符' }); },
        reconcileFn: () => ({}),
        warnFn: () => {},
      });
      const fileB = JSON.parse(readFileSync(join(t1, '.git', 'devloop', 'pods', '260901-t1q.json'), 'utf8'));
      assert('T1b 无效 ref → fail-closed 保持 blocked', rB === 'kept-blocked' && fileB.status === 'blocked' && boardB.pods[0].status === 'blocked',
        `r=${rB} file=${fileB.status}`);
      const rC = approvalRefUnlock(t1, { pods: [{ changeId: '260901-t1r', status: 'ready-to-merge' }] }, { changeId: '260901-t1r', humanRef: 'self-check-event', runner: () => {}, warnFn: () => {} });
      assert('T1c 非 blocked Pod 不适用（行为零变化）', rC === 'not-applicable', `r=${rC}`);
    } catch (err) {
      assert('T1 夹具异常', false, err.message);
    }
  }

  // ---- T2：Step 0 治理面自动收口 ----
  {
    const t2 = mkRepo('t2');
    try {
      writeFileSync(join(t2, 'base.txt'), 'base\n');
      git(t2, ['add', '--', 'base.txt']);
      git(t2, ['commit', '-m', 'init']);
      mkdirSync(join(t2, '.claude', 'taskpackages', '260901-t2p'), { recursive: true });
      writeFileSync(join(t2, '.claude', 'taskpackages', '260901-t2p', 'pod-events.jsonl'), '{}\n');
      mkdirSync(join(t2, 'changes', 'dev', '260901-t2p'), { recursive: true });
      writeFileSync(join(t2, 'changes', 'dev', '260901-t2p', 'pod-retro.md'), 'retro\n');
      const a = governanceFaceAutocommit(t2, '260901-t2p');
      const stA = git(t2, ['status', '--porcelain']);
      const msgA = git(t2, ['log', '-1', '--pretty=%s']);
      assert('T2a 白名单内 → 自动收口 commit 且 status 净', a === true && stA === '' && msgA.includes('merge 前治理面收口'), `ret=${a} status=${JSON.stringify(stA)} msg=${msgA}`);
      writeFileSync(join(t2, 'changes', 'dev', '260901-t2p', 'pod-retro.md'), 'retro2\n');
      writeFileSync(join(t2, 'stranger.txt'), 'offender\n');
      const b = governanceFaceAutocommit(t2, '260901-t2p');
      const stB = git(t2, ['status', '--porcelain']);
      assert('T2b 白名单外在场 → 拒绝且不吞', b === false && stB.includes('stranger.txt') && stB.includes('pod-retro.md'), `ret=${b} status=${JSON.stringify(stB)}`);
      const c = governanceFaceAutocommit(t2, 'current');
      assert('T2c 畸形 changeId（current）→ 拒绝', c === false, `ret=${c}`);
    } catch (err) {
      assert('T2 夹具异常', false, err.message);
    }
  }

  // ---- T3：投影冲突 --theirs 特判 ----
  {
    const t3 = mkRepo('t3');
    const proj = '.claude/taskpackages/current/taskpackage.json';
    try {
      mkdirSync(join(t3, '.claude', 'taskpackages', 'current'), { recursive: true });
      writeFileSync(join(t3, proj.replace(/\//g, '/')), '{"active":"base"}\n');
      git(t3, ['add', '--', '.']);
      git(t3, ['commit', '-m', 'init']);
      git(t3, ['checkout', '-b', 'feature']);
      writeFileSync(join(t3, ...proj.split('/')), '{"active":"feature"}\n');
      git(t3, ['commit', '-am', 'feature closeout projection']);
      git(t3, ['checkout', 'main']);
      writeFileSync(join(t3, ...proj.split('/')), '{"active":"main-side"}\n');
      git(t3, ['commit', '-am', 'main closeout projection']);
      git(t3, ['checkout', 'feature']);
      let threw = false;
      try { git(t3, ['rebase', 'main']); } catch { threw = true; }
      const res = threw ? tryResolveProjectionConflicts(t3, '260901-t3p') : false;
      const content = readFileSync(join(t3, ...proj.split('/')), 'utf8');
      const done = !existsSync(join(t3, '.git', 'rebase-merge')) && !existsSync(join(t3, '.git', 'rebase-apply'));
      assert('T3a 投影冲突 → --theirs（被合并包赢）自动消解且 rebase 完成', threw && res === true && done && content.includes('feature'), `threw=${threw} res=${res} done=${done} content=${content.trim()}`);
      // 混合集 → 整体拒绝
      git(t3, ['checkout', 'main']);
      writeFileSync(join(t3, 'shared.txt'), 'main\n');
      git(t3, ['add', '--', 'shared.txt']);
      git(t3, ['commit', '-m', 'main shared']);
      git(t3, ['checkout', 'feature']);
      writeFileSync(join(t3, 'shared.txt'), 'feature\n');
      git(t3, ['add', '--', 'shared.txt']);
      git(t3, ['commit', '-m', 'feature shared']);
      let threw2 = false;
      try { git(t3, ['rebase', 'main']); } catch { threw2 = true; }
      const res2 = threw2 ? tryResolveProjectionConflicts(t3, '260901-t3p') : false;
      assert('T3b 混合集 → 特判不部分生效（整体拒绝）', threw2 && res2 === false, `threw=${threw2} res=${res2}`);
      try { git(t3, ['rebase', '--abort']); } catch { /* 清场 */ }
    } catch (err) {
      assert('T3 夹具异常', false, err.message);
    }
  }

  // ---- T4：progress.md merge=union ----
  {
    const t4 = mkRepo('t4');
    try {
      writeFileSync(join(t4, '.gitattributes'), '.planning/kidpal-mvp/progress.md merge=union\n');
      mkdirSync(join(t4, '.planning', 'kidpal-mvp'), { recursive: true });
      writeFileSync(join(t4, '.planning', 'kidpal-mvp', 'progress.md'), 'base\n');
      git(t4, ['add', '--', '.']);
      git(t4, ['commit', '-m', 'init']);
      git(t4, ['checkout', '-b', 'sideA']);
      appendFileSync(join(t4, '.planning', 'kidpal-mvp', 'progress.md'), 'sideA-entry\n');
      git(t4, ['commit', '-am', 'sideA closeout record']);
      git(t4, ['checkout', 'main']);
      appendFileSync(join(t4, '.planning', 'kidpal-mvp', 'progress.md'), 'main-entry\n');
      git(t4, ['commit', '-am', 'main closeout record']);
      let mergeOk = true;
      try { git(t4, ['merge', 'sideA', '--no-edit']); } catch { mergeOk = false; }
      const content = readFileSync(join(t4, '.planning', 'kidpal-mvp', 'progress.md'), 'utf8');
      assert('T4 union merge 自动双侧保留', mergeOk && content.includes('sideA-entry') && content.includes('main-entry'), `ok=${mergeOk} content=${JSON.stringify(content)}`);
    } catch (err) {
      assert('T4 夹具异常', false, err.message);
    }
  }

  // ---- T6：Step 5b 簿记 pathspec 限定 ----
  {
    const t6 = mkRepo('t6');
    try {
      mkdirSync(join(t6, '.claude', 'taskpackages', '260901-t6p'), { recursive: true });
      writeFileSync(join(t6, '.claude', 'taskpackages', '260901-t6p', 'pod-events.jsonl'), '{"seed":true}\n');
      git(t6, ['add', '--', '.']);
      git(t6, ['commit', '-m', 'init']);
      writeFileSync(join(t6, 'decoy.txt'), 'staged residue\n');
      git(t6, ['add', '--', 'decoy.txt']); // 模拟 staged graph 残留
      step5bBookkeeping(t6, '260901-t6p', { mode: 'self-check' });
      const events = readFileSync(join(t6, '.claude', 'taskpackages', '260901-t6p', 'pod-events.jsonl'), 'utf8');
      const lastShow = git(t6, ['show', '--name-only', '--pretty=%s', 'HEAD']);
      const st = git(t6, ['status', '--porcelain']);
      assert('T6 簿记落行 + pathspec 不夹带 staged 残留',
        events.includes('"eventType":"merge-completed"') && lastShow.includes('pod-events.jsonl') && !lastShow.includes('decoy.txt') && st.includes('decoy.txt'),
        `events=${events.trim().slice(-80)} show=${JSON.stringify(lastShow)} status=${JSON.stringify(st)}`);
    } catch (err) {
      assert('T6 夹具异常', false, err.message);
    }
  }

  const pass = results.filter((r) => r.pass).length;
  const fail = results.length - pass;
  results.forEach((r) => {
    process.stdout.write(`  ${r.pass ? 'PASS' : 'FAIL'} · ${r.name}${r.pass ? '' : ` — ${r.detail}`}\n`);
  });
  process.stdout.write(`${JSON.stringify({ selfCheck: 'merge-driver-merge-friction', pass, fail, total: results.length, ok: fail === 0 })}\n`);
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
  // T2：--human-approval-ref 非空时透传，升级闸核验 pod-events 人批事件后可放行 ESCALATE 包（260904-gov-review-escalate-merge-approval）
  log(`${changeId}: running pre-merge escalation gate...`);
  if (!DRY_RUN) {
    try {
      const gateArgs = [join(projectDir, '.claude', 'scripts', 'pre-merge-escalation-gate.mjs'), changeId, '--source-ref', 'HEAD', '--project-dir', projectDir];
      if (HUMAN_APPROVAL_REF) gateArgs.push('--human-approval-ref', HUMAN_APPROVAL_REF);
      execFileSync(
        'node',
        gateArgs,
        { cwd: wt, encoding: 'utf8', stdio: 'pipe' },
      );
      log(`${changeId}: escalation gate PASS${HUMAN_APPROVAL_REF ? ` (human-approval-ref=${HUMAN_APPROVAL_REF})` : ''}`);
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

  // Step 0（T2，260907-gov-review-merge-friction）：merge 前治理面自动收口——rebase 步之前。
  // 白名单内 unstaged/untracked 自动 commit；白名单外 warn 留痕并保持 fail 行为（rebase 步 fail-closed）。
  if (!DRY_RUN) governanceFaceAutocommit(wt, changeId);

  // Step 3: fetch + rebase
  // T6：rebase 步（Pod worktree 内）之前幂等设 skip-worktree——置于 if (!DRY_RUN) 守卫之外（R1-F9）。
  ensureGraphCacheSynced(wt, changeId);
  log(`${changeId}: fetching origin...`);
  if (!DRY_RUN) {
    // Hot-fix（human 裁定 2026-09-05，pod-events 85667b2a）：本地开发 worktree/branch 不保证有远端——
    // 无 origin remote 时跳过 fetch（不再 fail-closed），rebase 目标由 resolveRebaseTargetLive 的
    // 「本地 main 优先、单缺降级存活 ref」语义兜底（ARQ-260820），fetch 缺席不改变目标解析结果。
    // 谓词 hasOriginRemote 为 T5 提取的具名实现（self-check 共用）。
    const hasOrigin = hasOriginRemote(wt);
    if (hasOrigin) {
      try {
        execSync('git fetch origin', { cwd: wt, encoding: 'utf8', stdio: 'pipe' });
      } catch (err) {
        warn(`${changeId}: git fetch failed: ${(err.stderr || '')}`);
        setPod(projectDir, { changeId, status: 'blocked' });
        return false;
      }
    } else {
      log(`${changeId}: no origin remote — skip fetch (local-only worktree), rebase target falls back to local main`);
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
      // T3（260907-gov-review-merge-friction）：rebase 冲突消解辅助——current/ 投影特判（被合并包赢）。
      // 特判不可解（混合集 / 超轮次 / 非冲突错误 / 未进入冲突态）→ 整体 abort + blocked，特判不部分生效（R1-F4）。
      if (!tryResolveProjectionConflicts(wt, changeId)) {
        const stderr = err.stderr || '';
        warn(`${changeId}: rebase conflict — aborting rebase`);
        log(`${changeId}: ${stderr.split('\n').slice(-10).join('\n')}`);
        try { execSync('git rebase --abort', { cwd: wt, encoding: 'utf8', stdio: 'pipe' }); } catch { /* race: nothing to abort */ }
        setPod(projectDir, { changeId, status: 'blocked' });
        return false;
      }
    }
  }
  log(`${changeId}: rebase OK`);

  // Step 4: quick check（rebase 冲突 → blocked; quick-check 失败 → 重试一次，再失败 → blocked）
  // 豁免例外（ARQ-260823-red-exemption T1）：acceptance 门红 + --human-approved-exemption <ref> + existing-red-inventory.md 存在非空
  // → 按人类裁决放行该门（其余 contract-lint 已过、照拦；260905 对齐两门案）；制品不在场或非 acceptance 门红 → 不豁免。
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
        warn(`${changeId}: acceptance gate exempted by human ruling (ref=${HUMAN_EXEMPTION}) — contract-lint already passed and remain enforced`);
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
  // merge 步在「持有 main 的 worktree」内执行（Hot-fix #2，见下）。T6：对该 worktree 幂等设
  // skip-worktree，先于守卫外执行（R1-F9：该步只动本地 index 标记、不改历史，DRY_RUN 下照常执行并留痕）。
  // mergeCwd 解析本身只读（git worktree list），DRY_RUN 下无害。findMainHolderWorktree 为 T5 提取的
  // 具名实现（self-check 共用）；无主 / 解析失败回退 wt。
  const mergeCwd = findMainHolderWorktree(wt) || wt;
  ensureGraphCacheSynced(mergeCwd, changeId);
  if (!DRY_RUN) {
    try {
      // Get the current branch name, then merge it into main
      const branch = execSync('git rev-parse --abbrev-ref HEAD', { cwd: wt, encoding: 'utf8' }).trim();
      if (branch === 'main' || branch === 'master') {
        warn(`${changeId}: already on ${branch}, skipping git merge`);
      } else {
        // Hot-fix #2（human 裁定 2026-09-05 延伸：merge-driver 须适配本地 worktree 拓扑，pod-events 85667b2a 同裁）：
        // worktree 场景下 main 由主 checkout 持有，本 worktree checkout main 会被 git 拒绝——
        // 改为定位持有 main 的 worktree，在那里执行 merge（等价语义：merge --no-ff 进本地 main）。
        // mergeCwd 已在守卫外解析（见 Step 5 开头），此处不再重复解析。
        // T2（OQ3=(a)）：人批合并时凭据强制进 merge commit message（对齐 Pod S 手工合并留痕形态）
        const mergeMsg = `Merge ${changeId} (merge-driver)${HUMAN_APPROVAL_REF ? ` human-approval-ref=${HUMAN_APPROVAL_REF}` : ''}`;
        try {
          execSync(`git merge --no-ff ${branch} -m "${mergeMsg}"`, {
            cwd: mergeCwd,
            encoding: 'utf8',
            stdio: 'pipe',
          });
        } catch (mergeErr) {
          const merr = mergeErr.stderr || '';
          warn(`${changeId}: git merge failed: ${merr.split('\n').slice(-5).join('\n')}`);
          warn(`${changeId}: aborting merge and marking blocked. Resolve conflicts manually.`);
          try {
            execSync('git merge --abort', { cwd: mergeCwd, encoding: 'utf8', stdio: 'pipe' });
          } catch { /* no merge in progress */ }
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

  // Step 5b（T6，260907-gov-review-merge-friction · OQ2 终裁 (c)）：merged 事件行落 main 簿记。
  // 实现提取为具名函数（self-check 共用同一实现，防假绿——对齐 worktree-topology 自检先例）。
  if (!DRY_RUN) step5bBookkeeping(mergeCwd, changeId);

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
  if (process.argv.includes('--self-check-worktree-topology')) {
    runWorktreeTopologySelfCheck();
    return;
  }
  if (process.argv.includes('--self-check-merge-friction')) {
    runMergeFrictionSelfCheck();
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

  // T1（260907-gov-review-merge-friction）：人批豁免消解——ready 收集之前（单 Pod + blocked + 有 ref 才生效，
  // 无 ref 的 blocked Pod 行为零变化；详见 approvalRefUnlock 注释）。
  if (SINGLE_POD && HUMAN_APPROVAL_REF && !DRY_RUN) {
    approvalRefUnlock(projectDir, board, { changeId: SINGLE_POD, humanRef: HUMAN_APPROVAL_REF });
  }

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
