#!/usr/bin/env node
// dev-loop deterministic verifier.
// Legacy direct mode keeps current verdict.json behavior. Bearing round mode
// additionally publishes one immutable verifier-round fact for FS-EVIDENCE-2.

import {
  appendFileSync,
  closeSync,
  existsSync,
  fsyncSync,
  linkSync,
  mkdirSync,
  openSync,
  readFileSync,
  unlinkSync,
  writeFileSync,
} from 'node:fs';
import { dirname, resolve } from 'node:path';
import { spawnSync } from 'node:child_process';
import { createHash, randomUUID } from 'node:crypto';
import { fileURLToPath } from 'node:url';
import {
  LIMITS,
  normalizeExpected,
  preflightProducer,
  serializeFact,
  truncateUtf8,
  validateFactObject,
} from '../scripts/verifier-evidence-check.mjs';
import { echoResolution, CURRENT_TASKPACKAGE_REL } from '../scripts/lib/current-pointer-shared.mjs'; // T6 回显齿（260924-gov-review-gate-parse-teeth）

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
const isGateChild = process.argv.includes('--gate-child');
const scriptPath = fileURLToPath(import.meta.url);

function parseArgs(argv = process.argv.slice(2)) {
  const args = {};
  for (let index = 0; index < argv.length; index += 1) {
    const token = argv[index];
    if (!token.startsWith('--')) continue;
    const eq = token.indexOf('=');
    if (eq >= 0) {
      args[token.slice(2, eq)] = token.slice(eq + 1);
      continue;
    }
    const key = token.slice(2);
    const next = argv[index + 1];
    if (next && !next.startsWith('--')) {
      args[key] = next;
      index += 1;
    } else {
      args[key] = true;
    }
  }
  return args;
}

function loadGates() {
  const p = resolve(projectDir, '.claude/verify-gates.json');
  return JSON.parse(readFileSync(p, 'utf8').replace(/^\uFEFF/, ''));
}

// 把闸门映射到 ErrorEnvelope.category（与 shared-types 的 ErrorCategory 对齐）。
function categoryFor(gate) {
  if (gate.kind === 'structural') return 'validation';
  if (gate.kind === 'functional') return 'ai-output';
  if (gate.kind === 'governance') return 'guardrail';
  if (gate.kind === 'security') return 'guardrail';
  if (gate.kind === 'documentation') return 'guardrail';
  return 'unknown';
}

// 稳定指纹：闸门 id + 第一条错误行的哈希，便于去重 / 追踪反复出现的同一类失败。
function fingerprint(id, text) {
  const firstErr = text.split('\n').find((line) => /error|fail|x|assert/i.test(line)) || text.slice(0, 200);
  return createHash('sha1').update(id + '|' + firstErr.trim()).digest('hex').slice(0, 12);
}

// ── timing mode ──
// scoped = 每轮 verifier 都跑；full = 仅终审 merge-readiness 跑一次。
// 默认 timing=scoped（未标则每轮跑，fail-safe）。
// --phase=final 时跑全部；否则只跑 timing=scoped 的闸。

function phaseFromArgs(args = parseArgs()) {
  return args.phase === 'final' ? 'final' : 'scoped';
}

// ── 未知 phase fail-loud（260924-gov-review-gate-parse-teeth T2，OQ-2=(a) human 批准向严）──
// 缺省（不传 --phase）→ scoped 是 :79-81 注释与 phaseFromArgs 的成文 fail-safe 默认，保留；
// 在场但非法（K7 实烧形态：--phase=full = timing 词汇误用为 phase 值）→ fail-loud，不再静默回落。
// bearing 路径由 normalizeExpected（verifier-evidence-check.mjs）同判据 fail-loud；本段补 legacy 路径缺口。
const VALID_PHASES = ['scoped', 'final'];
function phaseArgError(args) {
  if (args.phase === undefined) return null;
  if (typeof args.phase !== 'string' || !VALID_PHASES.includes(args.phase)) {
    return `[verify.mjs] 未知 --phase 值 ${JSON.stringify(args.phase)} —— 只认 scoped|final（fail-loud，不静默回落；bearing 路径 normalizeExpected 同判据）`;
  }
  return null;
}
function assertPhaseArg(args) {
  const message = phaseArgError(args);
  if (message !== null) {
    process.stderr.write(`${message}\n`);
    process.exit(2);
  }
}

function gateActive(gate, phase) {
  const timing = gate.timing || 'scoped'; // 未标默认 scoped
  if (timing === 'scoped') return true;
  if (timing === 'full' && phase === 'final') return true;
  return false;
}

function boundedText(value, maxBytes) {
  const text = Buffer.isBuffer(value) ? value.toString('utf8') : String(value || '');
  return truncateUtf8(text, maxBytes);
}

// ── verdict.json（机器事实，程序 fs-write，非 agent 落盘；R11）──
function writeVerdictJson(results, ok, error, phase, extra = {}) {
  const dir = resolve(projectDir, 'delivery', 'taskpackages');
  mkdirSync(dir, { recursive: true });
  const verdict = {
    producedBy: 'verify.mjs (script fs-write)',
    ts: new Date().toISOString(),
    phase: phase || phaseFromArgs(),
    ok,
    results,
    error: error || null,
    ...extra,
  };
  // 写入 current/verdict.json（最新一轮）
  const currentDir = resolve(dir, 'current');
  mkdirSync(currentDir, { recursive: true });
  writeFileSync(resolve(currentDir, 'verdict.json'), JSON.stringify(verdict, null, 2) + '\n', 'utf8');
}

// Dual-write: append structured run results to verify-runs.jsonl for timing aggregation.
// Writes on BOTH success and failure paths — fail-open, never blocks.
// OWNER: metrics-ledger (FROZEN v4). C1 does NOT modify this section.
function persistVerifyRuns(results, phase, caller) {
  try {
    const current = resolve(projectDir, 'delivery', 'taskpackages', 'current', 'taskpackage.json');
    if (!existsSync(current)) return; // no TaskPackage context → skip
    const tp = JSON.parse(readFileSync(current, 'utf8').replace(/^﻿/, ''));
    if (!tp.changeId) return;
    // T6（260924-gov-review-gate-parse-teeth）：回显齿——病例①承载者（home-p1 九闸跑错包实证），
    // 经 current/ 解析的 changeId 与来源始终可见（纯 stderr，不改任何判定；captured by gate log）。
    echoResolution('verify.mjs', tp.changeId, CURRENT_TASKPACKAGE_REL);
    const tpDir = resolve(projectDir, 'delivery', 'taskpackages', tp.changeId);
    mkdirSync(tpDir, { recursive: true });
    const record = {
      ts: new Date().toISOString(),
      phase: phase || 'verifier-round',
      caller: caller || 'verifier',
      results,
    };
    appendFileSync(resolve(tpDir, 'verify-runs.jsonl'), JSON.stringify(record) + '\n');
  } catch {
    // fail-open: persistence failure never blocks verify
  }
}

function writeGateLog(gate, output) {
  const dir = resolve(projectDir, '.claude/verify-logs');
  mkdirSync(dir, { recursive: true });
  const safeId = gate.id.replace(/[^a-z0-9_-]/gi, '-');
  const fileName = `${Date.now()}-${safeId}.log`;
  const absolutePath = resolve(dir, fileName);
  writeFileSync(
    absolutePath,
    [`gate=${gate.id}`, `kind=${gate.kind}`, `cmd=${gate.cmd}`, `phase=${phaseFromArgs()}`, '', output].join('\n'),
    'utf8',
  );
  return `.claude/verify-logs/${fileName}`;
}

// 260909-gov-review-gate-truthfulness T4：闸「没跑」状态从闸 stdout 解析上收为机器可判字段。
// 判据 = 闸输出含 degraded:true ∨ phase:"executor-skipped" ∨ allLiveDeselected:true。
// 效力边界（DP-D4 同型）：只产字段、不改本轮 ok 判定、不改 merge 放行——让上游与仪表盘能判；
// 机械消费（supervisor / pre-merge 读它）按 OQ4=(b) 留下一批。verifier 报告点名义务成文于
// docs/DEV-GOVERNANCE.md「## 验收测试纪律」§1。
function liftNotExecuted(stdout) {
  const degraded = /"degraded":\s*true/.test(stdout);
  const skipped = /"phase":\s*"executor-skipped"/.test(stdout);
  const allLive = /"allLiveDeselected":\s*true/.test(stdout);
  if (!degraded && !skipped && !allLive) return { notExecuted: false, notExecutedReason: '' };
  const m = stdout.match(/"reason":\s*"([^"\n]{0,300})/);
  return {
    notExecuted: true,
    notExecutedReason: (m && m[1]) || (degraded ? 'degraded' : allLive ? 'allLiveDeselected' : 'executor-skipped'),
  };
}

// 汇总层：gatesNotExecuted / gatesRun。round fact 顶层键白名单封闭（verifier-evidence-check 零改动），
// 故汇总落 verdict.json + child payload；round fact 内经 results[].notExecuted 承载（逐闸可判）。
function summarizeNotExecuted(results) {
  const gatesNotExecuted = results.filter((r) => r.notExecuted).map((r) => r.gate || r.id);
  return { gatesNotExecuted, gatesRun: results.length - gatesNotExecuted.length };
}

function runGateChild() {
  const cfg = loadGates();
  const results = [];
  const failures = []; // 260813-gate-accuracy-batch T3：final 相位全红清单收集（scoped 相位不用，遇红即停）
  const phase = phaseFromArgs();

  const activeGates = (cfg.gates || []).filter((gate) => gateActive(gate, phase));
  const skippedCount = (cfg.gates || []).length - activeGates.length;
  if (skippedCount > 0) {
    process.stderr.write(`[verify.mjs] phase=${phase}: running ${activeGates.length} gates, skipping ${skippedCount} full-only gates\n`);
  }

  for (const gate of activeGates) {
    const started = Date.now();
    const run = spawnSync(gate.cmd, { cwd: projectDir, shell: true, encoding: 'buffer', maxBuffer: LIMITS.maxFactBytes * 4 });
    const stdout = boundedText(run.stdout, LIMITS.perResultStdoutBytes);
    const stderr = boundedText(run.stderr, LIMITS.perResultStderrBytes);
    const out = stdout + stderr;
    const exitCode = typeof run.status === 'number' ? run.status : 1;
    const ok = exitCode === 0;
    results.push({
      id: gate.id,
      gate: gate.id,
      kind: gate.kind,
      ok,
      exit: exitCode,
      exitCode,
      ms: Date.now() - started,
      timing: gate.timing || 'scoped',
      stdout,
      stderr,
      ...liftNotExecuted(stdout),
      summary: truncateUtf8(ok ? `${gate.id} passed` : `${gate.id} failed`, LIMITS.perResultSummaryBytes),
    });

    if (!ok) {
      const logFile = writeGateLog(gate, out);
      const error = {
        name: 'GateFailure',
        category: categoryFor(gate),
        fingerprint: fingerprint(gate.id, out),
        failingStep: gate.id,
        message: `gate ${gate.id} failed (exit ${exitCode})`,
        cmd: gate.cmd,
        logFile,
        logTail: truncateUtf8(out.split('\n').slice(-40).join('\n'), LIMITS.errorFieldBytes),
      };
      if (phase === 'final') {
        // 260813-gate-accuracy-batch T3（OQ-4=(a) / ARQ-260805-final-stop-on-red-masking）：
        // final 相位（merge-readiness 终审）遇红不停、跑完全闸产全红清单——红信号完备性；
        // scoped 相位保留遇红即停快反馈（零改）。
        failures.push(error);
        continue;
      }
      console.log(JSON.stringify({ ok: false, results, error, phase }, null, 2));
      process.exit(1);
    }
  }

  if (failures.length > 0) {
    // final 相位有红：error = 首红 ErrorEnvelope（validateFactObject 要求 fact.error 为对象，保形）；
    // errors[] = 全红清单——仅 child stdout 瞬态（validateFactObject allowed 集无 errors 键），
    // 全红清单唯一持久载体 = results[]（+ verify-runs.jsonl 投影）。FROZEN v1.0 OQ-4=(a)/F7(iii)。
    console.log(JSON.stringify({ ok: false, results, error: failures[0], errors: failures, phase, ...summarizeNotExecuted(results) }, null, 2));
    process.exit(1);
  }

  console.log(JSON.stringify({ ok: true, results, error: null, phase, ...summarizeNotExecuted(results) }, null, 2));
  process.exit(0);
}

function runGateChildProcess(phase) {
  return spawnSync(process.execPath, [scriptPath, '--gate-child', `--phase=${phase}`], {
    cwd: projectDir,
    env: { ...process.env, CLAUDE_PROJECT_DIR: projectDir },
    encoding: 'utf8',
    maxBuffer: LIMITS.maxFactBytes * 4,
  });
}

function parseChildPayload(child) {
  try {
    const payload = JSON.parse(child.stdout || '{}');
    if (!payload || typeof payload.ok !== 'boolean' || !Array.isArray(payload.results)) {
      throw new Error('gate child payload missing ok/results');
    }
    return payload;
  } catch (error) {
    throw new Error(`gate child output parse failed: ${error.message}`);
  }
}

function runLegacy() {
  const phase = phaseFromArgs();
  const child = runGateChildProcess(phase);
  const payload = parseChildPayload(child);
  persistVerifyRuns(payload.results || [], 'verifier-round', 'verifier');
  writeVerdictJson(payload.results || [], !!payload.ok, payload.error || null, phase, { legacyNonBearing: true, ...summarizeNotExecuted(payload.results || []) });
  console.log(JSON.stringify({ ok: !!payload.ok, results: payload.results || [], error: payload.error || null, phase, ...summarizeNotExecuted(payload.results || []) }, null, 2));
  process.exit(child.status === 0 ? 0 : 1);
}

function publishFact(paths, text) {
  const tempName = `.r${paths.round}.${process.pid}.${randomUUID()}.tmp`;
  const tempPath = resolve(dirname(paths.finalPath), tempName);
  let fd;
  try {
    fd = openSync(tempPath, 'wx');
    writeFileSync(fd, text, 'utf8');
    fsyncSync(fd);
    closeSync(fd);
    fd = undefined;
    linkSync(tempPath, paths.finalPath);
    try { unlinkSync(tempPath); } catch { /* best effort */ }
    try {
      const dirFd = openSync(dirname(paths.finalPath), 'r');
      try { fsyncSync(dirFd); } finally { closeSync(dirFd); }
    } catch {
      // Directory fsync is best-effort and platform-dependent.
    }
  } catch (error) {
    if (fd !== undefined) {
      try { closeSync(fd); } catch { /* best effort */ }
    }
    try { unlinkSync(tempPath); } catch { /* best effort */ }
    throw error;
  }
}

function runBearing() {
  const args = parseArgs();
  const expected = normalizeExpected({ changeId: args['change-id'], phase: args.phase, round: args.round, invocationId: args['invocation-id'] });
  let paths;
  try {
    paths = preflightProducer(projectDir, expected);
  } catch (error) {
    process.stderr.write(truncateUtf8(`[verify.mjs] bearing preflight failed: ${error.message}\n`, LIMITS.parentStderrBytes));
    process.exit(2);
  }

  const startedAt = new Date().toISOString();
  const child = runGateChildProcess(expected.phase);
  const completedAt = new Date().toISOString();
  if (![0, 1].includes(child.status)) {
    process.stderr.write(truncateUtf8(`[verify.mjs] gate child infrastructure failure\n${child.stderr || ''}`, LIMITS.parentStderrBytes));
    process.exit(2);
  }
  let payload;
  try {
    payload = parseChildPayload(child);
  } catch (error) {
    process.stderr.write(truncateUtf8(`[verify.mjs] ${error.message}\n`, LIMITS.parentStderrBytes));
    process.exit(2);
  }
  const fact = {
    schemaVersion: '1.0',
    artifactType: 'verifier-round-fact',
    authority: 'bearing',
    producedBy: 'verify.mjs',
    changeId: expected.changeId,
    phase: expected.phase,
    round: expected.round,
    invocationId: expected.invocationId,
    startedAt,
    completedAt,
    observedGateExit: child.status,
    ok: child.status === 0,
    results: payload.results || [],
    error: child.status === 0 ? null : payload.error,
  };
  const validation = validateFactObject(fact, expected);
  if (!validation.ok) {
    process.stderr.write(truncateUtf8(`[verify.mjs] fact validation failed: ${validation.errors.join('; ')}\n`, LIMITS.parentStderrBytes));
    process.exit(2);
  }
  try {
    publishFact(paths, serializeFact(fact));
  } catch (error) {
    process.stderr.write(truncateUtf8(`[verify.mjs] fact publication failed: ${error.message}\n`, LIMITS.parentStderrBytes));
    process.exit(2);
  }
  try {
    persistVerifyRuns(fact.results, 'verifier-round', 'verifier');
    writeVerdictJson(fact.results, fact.ok, fact.error, expected.phase, {
      changeId: expected.changeId,
      round: expected.round,
      invocationId: expected.invocationId,
      evidenceRef: paths.evidenceRef,
      observedGateExit: fact.observedGateExit,
      legacyNonBearing: false,
      ...summarizeNotExecuted(fact.results),
    });
  } catch {
    // Non-bearing projection/observability cannot change published 0/1.
  }
  console.log(truncateUtf8(JSON.stringify({ ok: fact.ok, phase: fact.phase, round: fact.round, evidenceRef: paths.evidenceRef, observedGateExit: fact.observedGateExit }), LIMITS.parentStdoutBytes));
  process.exit(child.status);
}

// ── self-check（260924-gov-review-gate-parse-teeth T2 新增；argv 触发型，mutation-check 结构注同款：
// 分支先于 gate-child/legacy 派发，不真跑闸（valid 值覆盖由纯 case 承担，零 current/ 簿记副作用））──
function selfCheck() {
  const pass = [];
  const fail = [];
  const check = (name, ok) => (ok ? pass : fail).push(name);
  // 纯判定面（真空绿：断言输入非空 + 逐 case 预期）
  check('phase-arg-absent-allows', phaseArgError({}) === null);
  check('phase-arg-scoped-allows', phaseArgError({ phase: 'scoped' }) === null);
  check('phase-arg-final-allows', phaseArgError({ phase: 'final' }) === null);
  check('phase-arg-full-rejects', phaseArgError({ phase: 'full' }).includes('未知 --phase 值'));
  check('phase-arg-bare-flag-rejects', phaseArgError({ phase: true }).includes('未知 --phase 值'));
  check('phase-arg-garbage-rejects', phaseArgError({ phase: 'garbage' }).includes('未知 --phase 值'));
  // 入口面（spawn 子进程断言退出码；非法值校验先于一切派发 ⇒ 零副作用）
  const child = spawnSync(process.execPath, [scriptPath, '--phase=full'], {
    cwd: projectDir, encoding: 'utf8', timeout: 30000,
  });
  check('entry-full-exit-2', child.status === 2);
  check('entry-full-stderr-names-value', String(child.stderr || '').includes('未知 --phase 值'));
  console.log(JSON.stringify({ selfCheck: 'verify.mjs', pass: pass.length, fail: fail.length, total: pass.length + fail.length, ok: fail.length === 0, failures: fail.length ? fail : undefined }));
  process.exit(fail.length === 0 ? 0 : 1);
}

const args = parseArgs();
if (process.argv.includes('--self-check')) selfCheck();
assertPhaseArg(args);
if (isGateChild) runGateChild();
if (args['evidence-mode'] === 'round') runBearing();
runLegacy();
