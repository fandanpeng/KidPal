#!/usr/bin/env node
// 每 loop 收尾聚合脚本：读 verify-runs.jsonl + dispatch-log.jsonl + git numstat
// → 产 loop-metrics.json（四桶耗时 + 轮数 + 每闸 pass/fail+耗时 + diff 大小）。
// 由 integrator 在 closeout 运行，钉在 closeout-check.mjs 之前。
// 缺数据 fail-open + 标 metricsCompleteness，不红、不挡合并门。
//
// 用法：
//   node .claude/scripts/loop-metrics.mjs [--change-id <id>]

import { existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { execSync } from 'node:child_process';
import { EVENTS_SCHEMA_CUTOFF } from './pod-events-append.mjs';
import { CHANGE_ID_PREFIX_RE } from './lib/changeid-shared.mjs'; // T1 单源收敛（260906-gov-review-arq-governance-batch）

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// ── changeId 解析：CHANGE_ID_PREFIX_RE 自 lib/changeid-shared.mjs 单源导入（DP-F11 / M-A10）；
//    stripPrefix / validDatePrefix 仍本地（R1-F7 边界声明，二轮收敛留后续治理批）──

function validDatePrefix(yyMMdd) {
  const month = Number(yyMMdd.slice(2, 4));
  const day = Number(yyMMdd.slice(4, 6));
  return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

function stripPrefix(changeId) {
  const m = CHANGE_ID_PREFIX_RE.exec(String(changeId));
  return m ? String(changeId).slice(7) : String(changeId);
}

// ── helpers ──

function readJson(path) {
  try {
    return JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
  } catch {
    return null;
  }
}

function readJsonlLines(path) {
  try {
    const text = readFileSync(path, 'utf8');
    const lines = text.split(/\r?\n/).filter((l) => l.trim());
    const records = [];
    for (const line of lines) {
      try {
        records.push(JSON.parse(line));
      } catch {
        // 悬空尾 / 截断行：容错跳过（DP-F8(3)）
      }
    }
    return { records, truncated: lines.length > records.length };
  } catch {
    return { records: [], truncated: false };
  }
}

function resolveChangeId() {
  const explicit = process.argv.find((a, i) => a === '--change-id' && i + 1 < process.argv.length);
  if (explicit) return process.argv[process.argv.indexOf('--change-id') + 1];

  const current = resolve(projectDir, '.claude', 'taskpackages', 'current', 'taskpackage.json');
  if (!existsSync(current)) return null;
  const tp = readJson(current);
  return tp && tp.changeId ? tp.changeId : null;
}

function countVerifierRounds(verifyRuns) {
  // 排除 integrator-full-verify 的全量运行（DP-F7），只计 phase=verifier-round
  return verifyRuns.filter(
    (r) => r.phase === 'verifier-round' && r.caller !== 'integrator-full-verify',
  ).length;
}

function countFixerRounds(verifyRuns) {
  // fixer 轮数 = 在 planner 交接中记的 dispatch-log 或通过 verify-runs 间接推导
  // 此处从 verify-runs 的 phase 字段取（phase=fixer-attempt）
  return verifyRuns.filter((r) => r.phase === 'fixer-attempt').length;
}

function aggregateGates(verifyRuns) {
  // 每道闸最后一次的 pass/fail + 累计耗时 + reran 次数
  const gateMap = new Map(); // gateId → { ms, passed, reran }
  for (const run of verifyRuns) {
    if (!run.results || !Array.isArray(run.results)) continue;
    for (const g of run.results) {
      const id = g.id || g.name || 'unknown';
      const existing = gateMap.get(id);
      if (existing) {
        existing.ms += g.ms || 0;
        existing.reran += 1;
        // 最后一次的结果覆盖（因 fail-fast，最后一轮的值是最终状态）
        existing.passed = g.ok !== undefined ? g.ok : g.passed;
      } else {
        gateMap.set(id, {
          name: id,
          passed: g.ok !== undefined ? g.ok : true,
          ms: g.ms || 0,
          reran: 1,
        });
      }
    }
  }
  return [...gateMap.values()];
}

function deriveTiming(verifyRuns, dispatchLog, notes) {
  // testExecMs = Σ verify-run 所有 gate ms
  let testExecMs = 0;
  for (const run of verifyRuns) {
    if (run.results && Array.isArray(run.results)) {
      testExecMs += run.results.reduce((sum, g) => sum + (g.ms || 0), 0);
    }
  }

  // llmGenMs / waitMs：由 dispatch-log 的 harness durationMs 配 tokens/toolUses 比推导（v1 近似）
  let llmGenMs = 0;
  let waitMs = 0;
  let hasDispatchData = false;

  if (dispatchLog.length > 0) {
    hasDispatchData = true;
    let totalDispatchMs = 0;
    let sumRatioScore = 0; // token/duration 比归一化得分（高 = LLM 活跃段）

    for (const d of dispatchLog) {
      totalDispatchMs += d.durationMs || 0;
    }

    // 使用 token/duration 比分离「LLM 活跃 vs 停机干等」
    // 启发式：token 密度高（> 0.5 tok/ms 即每秒 500+ tokens）→ LLM 活跃段
    //          token 密度极低（< 0.01 tok/ms）+ toolUses ≈ 0 → 可能停机干等
    for (const d of dispatchLog) {
      const dur = d.durationMs || 0;
      const tok = d.tokens || 0;
      const tools = d.toolUses || 0;
      if (dur === 0) continue;

      const tokDensity = tok / dur; // tokens per ms
      if (tokDensity >= 0.0005 || tools > 0) {
        // LLM 活跃或有工具调用 → 主要归 LLM
        llmGenMs += dur;
      } else if (tokDensity < 0.00001 && tools === 0) {
        // 极低 token 密度 + 无工具调用 → 可能停机
        waitMs += dur;
      } else {
        // 模糊区：按 tokDensity 比例分
        const activeRatio = Math.min(1, Math.max(0, tokDensity * 2000)); // 0.0005 → 1.0
        llmGenMs += Math.round(dur * activeRatio);
        waitMs += Math.round(dur * (1 - activeRatio));
      }
    }

    // 从 llmGenMs 中扣掉内嵌的 test 执行时间（v1 近似，可能重叠）
    if (testExecMs > 0 && llmGenMs > testExecMs) {
      llmGenMs -= testExecMs;
      notes.push('llmGenMs: deducted testExecMs (v1 approximation; may overlap with embedded tool/test time)');
    }
  }

  const totalMs = testExecMs + llmGenMs + waitMs;
  const unattributedMs = 0; // v1 不做余量估算

  return {
    totalMs,
    llmGenMs,
    testExecMs,
    waitMs,
    unattributedMs,
    hasDispatchData,
  };
}

function gitDiffStats() {
  try {
    // vs baseCommit：从 taskpackage 读或 fallback HEAD~1
    const out = execSync('git diff --numstat HEAD~1', {
      cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
    });
    let prodLoc = 0;
    let testLoc = 0;
    let filesChanged = 0;
    for (const line of out.split(/\r?\n/).filter(Boolean)) {
      const parts = line.split(/\s+/);
      if (parts.length < 3) continue;
      const added = parseInt(parts[0], 10) || 0;
      const removed = parseInt(parts[1], 10) || 0;
      const file = parts[2];
      const loc = added + removed;
      filesChanged++;
      if (/\.(test|spec|acceptance)\./.test(file) || /__tests__|acceptance|__mocks__/.test(file)) {
        testLoc += loc;
      } else {
        prodLoc += loc;
      }
    }
    return { filesChanged, prodLoc, testLoc };
  } catch {
    return { filesChanged: 0, prodLoc: 0, testLoc: 0 };
  }
}

// ── main ──

const changeId = resolveChangeId();
if (!changeId) {
  process.stderr.write('[loop-metrics] no changeId found; skipping (fail-open)\n');
  process.exit(0);
}

// ── Post-cutoff: on-demand only, no committed loop-metrics.json (Package B) ──
const isPostEventsCutoff = (() => {
  const m = CHANGE_ID_PREFIX_RE.exec(String(changeId));
  if (!m) return false;
  return m[1] >= EVENTS_SCHEMA_CUTOFF.replace(/-/g, '').slice(2);
})();

if (isPostEventsCutoff) {
  // For post-cutoff Pods: read dispatch-completed from pod-events.jsonl, emit coverage, return only
  const tpDirPostCutoff = resolve(projectDir, '.claude', 'taskpackages', changeId);
  const eventsPath = resolve(tpDirPostCutoff, 'pod-events.jsonl');
  const verifyPathPostCutoff = resolve(tpDirPostCutoff, 'verifier-evidence', 'verify-runs.jsonl');

  // Fallback: verify-runs may still be at top level of taskpackages dir
  const verifyPathFallback = resolve(tpDirPostCutoff, 'verify-runs.jsonl');
  const actualVerifyPath = existsSync(verifyPathPostCutoff) ? verifyPathPostCutoff : verifyPathFallback;

  const { records: verifyRuns } = readJsonlLines(actualVerifyPath);
  const { records: events } = readJsonlLines(eventsPath);

  // Count dispatches from pod-events
  const dispatches = events.filter(e => e.eventType === 'dispatch-completed');
  let dispatchesTotal = dispatches.length;
  let dispatchesWithTokenData = 0;
  let dispatchesUnavailable = 0;

  for (const d of dispatches) {
    const tu = d.payload && d.payload.tokenUsage;
    if (!tu || tu.status === 'unavailable') { dispatchesUnavailable++; continue; }
    if (tu.status === 'exact') { dispatchesWithTokenData++; }
  }

  const metricsObj = {
    schemaVersion: '1.0',
    changeId,
    postCutoff: true,
    authority: 'derived',
    dispatchesTotal,
    dispatchesWithTokenData,
    dispatchesUnavailable,
    verifierRuns: countVerifierRounds(verifyRuns),
    fixerRounds: countFixerRounds(verifyRuns),
    gates: aggregateGates(verifyRuns),
    note: 'post-cutoff: on-demand only, not committed (Package B)',
  };

  // Output to stdout only — do NOT write loop-metrics.json for post-cutoff
  console.log(JSON.stringify(metricsObj, null, 2));
  process.stderr.write(`[loop-metrics] ${changeId} (post-cutoff): on-demand metrics emitted to stdout; NOT committed\n`);
  process.exit(0);
}

const notes = [];
let metricsCompleteness = 'full';

const tpDir = resolve(projectDir, '.claude', 'taskpackages', changeId);
const verifyPath = resolve(tpDir, 'verify-runs.jsonl');
const dispatchPath = resolve(tpDir, 'dispatch-log.jsonl');

const { records: verifyRuns, truncated: verifyTruncated } = readJsonlLines(verifyPath);
const { records: dispatchLog, truncated: dispatchTruncated } = readJsonlLines(dispatchPath);

if (verifyTruncated) {
  notes.push('verify-runs.jsonl: some lines truncated/skipped');
  metricsCompleteness = 'timing:partial(verify-runs-truncated-rows)';
}
if (dispatchTruncated) {
  notes.push('dispatch-log.jsonl: some lines truncated/skipped');
  metricsCompleteness = metricsCompleteness === 'full'
    ? 'timing:partial(dispatch-log-truncated-rows)'
    : metricsCompleteness + '; dispatch-log truncated';
}

const verifierRuns = countVerifierRounds(verifyRuns);
const fixerRounds = countFixerRounds(verifyRuns);
const gates = aggregateGates(verifyRuns);
const timing = deriveTiming(verifyRuns, dispatchLog, notes);
const diff = gitDiffStats();

if (!timing.hasDispatchData) {
  if (metricsCompleteness === 'full') metricsCompleteness = 'timing:partial(no-dispatch-stamps)';
  else metricsCompleteness += '; no-dispatch-stamps';
}

// 比值分离实证边界标注（DP-FD7）
if (timing.hasDispatchData && timing.llmGenMs > 0) {
  // 诚实声明：正常 loop 上未验证
  notes.push(
    'llmGen/wait split: ratio-split empirically from single R3 anomaly, unvalidated on normal loops; threshold approximate',
  );
  if (metricsCompleteness === 'full') {
    metricsCompleteness =
      'timing:partial(llmGen/wait approximate; ratio-split empirically from single R3 anomaly, unvalidated on normal loops; threshold undefined)';
  }
}

// 检测失败轮 fail-fast 截断（DP-F3③）
const hasFailFastTruncation = verifyRuns.some(
  (r) => r.results && Array.isArray(r.results) && r.results.some((g) => g.ok === false),
);
if (hasFailFastTruncation) {
  if (metricsCompleteness === 'full') {
    metricsCompleteness = 'timing:partial(fail-fast-truncated; late gates not sampled in failed rounds)';
  } else if (!metricsCompleteness.includes('fail-fast')) {
    metricsCompleteness += '; fail-fast-truncated';
  }
}

// 读 terminalOutcome / riskTier
const changeDir = resolve(projectDir, 'changes', 'dev', changeId);
const verdictPath = resolve(changeDir, 'supervisor-verdict.json');
const taskPackagePath = resolve(changeDir, 'taskpackage.json');

let terminalOutcome = 'unknown';
let riskTier = 'unknown';

const verdictObj = readJson(verdictPath);
if (verdictObj && typeof verdictObj.verdict === 'string') {
  terminalOutcome = verdictObj.verdict.toUpperCase();
}

const tp = readJson(taskPackagePath);
if (tp && tp.riskTier) {
  riskTier = tp.riskTier;
}

const metrics = {
  schemaVersion: '1.0',
  changeId,
  riskTier,
  terminalOutcome,
  authority: 'derived',
  sourceRefs: {
    rounds: 'verify-runs.jsonl',
    gates: 'verify-runs.jsonl',
    terminalOutcome: 'supervisor-verdict.json',
  },
  timing: {
    totalMs: timing.totalMs,
    llmGenMs: timing.llmGenMs,
    testExecMs: timing.testExecMs,
    waitMs: timing.waitMs,
    unattributedMs: timing.unattributedMs,
  },
  rounds: {
    fixer: fixerRounds,
    verifierRuns,
  },
  gates,
  diff,
  metricsCompleteness,
  notes,
};

// 产出路径：优先 changes/dev/<changeId>/，若不存在则 .claude/taskpackages/<changeId>/
let outputDir;
if (existsSync(changeDir)) {
  outputDir = changeDir;
} else {
  outputDir = tpDir;
  mkdirSync(outputDir, { recursive: true });
}

const outputPath = resolve(outputDir, 'loop-metrics.json');
writeFileSync(outputPath, JSON.stringify(metrics, null, 2), 'utf8');

process.stderr.write(
  `[loop-metrics] ${changeId}: verifierRuns=${verifierRuns} fixer=${fixerRounds} ` +
    `gates=${gates.length} testExecMs=${timing.testExecMs} llmGenMs=${timing.llmGenMs} ` +
    `completeness=${metricsCompleteness} → ${outputPath}\n`,
);
process.exit(0);
