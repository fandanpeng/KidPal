#!/usr/bin/env node
// 每次 planner Agent() dispatch 收工后调用，把 harness 回传值落盘。
// Pre-cutoff: 程序内部 fs-append 一行到 .claude/taskpackages/<id>/dispatch-log.jsonl。
// Post-cutoff (EVENTS_SCHEMA_CUTOFF): writes dispatch-completed event to pod-events.jsonl
//   via pod-events-append.mjs. Does NOT write dispatch-log.jsonl for post-cutoff Pods (F1).
// 对偶 portfolio-reconcile.mjs（程序内 fs 写，非 shell 重定向）。
//
// PACKAGE: gov-review-retro-convergence (Package B, FROZEN 2026-07-25)
//
// 用法（post-cutoff structured tokens）：
//   node .claude/scripts/dispatch-stamp.mjs \
//     --dispatch-id <id> --actor <name> --phase <phase> \
//     --duration-ms <ms> --tool-uses <n> --terminal-status <status> \
//     --token-status exact|unavailable \
//     [--token-input <n>] [--token-output <n>] [--token-cache-read <n>] \
//     [--token-cache-write <n>] [--token-total <n>] [--token-source <s>] \
//     [--token-reason <s>] [--task-package-id <id>]
//
// 用法（legacy, pre-cutoff）：
//   node .claude/scripts/dispatch-stamp.mjs \
//     --dispatch-id <id> --actor <name> --phase <phase> \
//     --duration-ms <ms> --tokens <n> --tool-uses <n> \
//     --terminal-status <status> [--task-package-id <id>]

import { appendFileSync, existsSync, mkdirSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { EVENTS_SCHEMA_CUTOFF, appendPodEvent } from './pod-events-append.mjs';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

const CHANGE_ID_PREFIX_RE = /^(\d{6})-[a-zA-Z0-9-]+$/;

function flagVal(name) {
  const i = process.argv.indexOf(name);
  return i >= 0 ? process.argv[i + 1] : undefined;
}

function required(name) {
  const v = flagVal(name);
  if (!v) {
    process.stderr.write(`[dispatch-stamp] missing required flag: ${name}\n`);
    process.exit(2);
  }
  return v;
}

function taskPackageDir() {
  const explicit = flagVal('--task-package-id');
  if (explicit) return resolve(projectDir, '.claude', 'taskpackages', explicit);

  const current = resolve(projectDir, '.claude', 'taskpackages', 'current', 'taskpackage.json');
  if (!existsSync(current)) return null;
  try {
    const tp = JSON.parse(readFileSync(current, 'utf8').replace(/^\uFEFF/, ''));
    return tp.changeId ? resolve(projectDir, '.claude', 'taskpackages', tp.changeId) : null;
  } catch {
    return null;
  }
}

function resolveChangeId() {
  const explicit = flagVal('--task-package-id');
  if (explicit) return explicit;

  const current = resolve(projectDir, '.claude', 'taskpackages', 'current', 'taskpackage.json');
  if (!existsSync(current)) return null;
  try {
    const tp = JSON.parse(readFileSync(current, 'utf8').replace(/^\uFEFF/, ''));
    return tp.changeId || null;
  } catch {
    return null;
  }
}

function isPostEventsCutoff(changeId) {
  const m = CHANGE_ID_PREFIX_RE.exec(String(changeId));
  if (!m) return false;
  const prefix = m[1]; // YYMMDD
  const cutoffCompact = EVENTS_SCHEMA_CUTOFF.replace(/-/g, '').slice(2); // '2026-07-25' → '260725'
  return prefix >= cutoffCompact;
}

function optionalNonNegativeInteger(name) {
  const raw = flagVal(name);
  if (raw === undefined) return undefined;
  if (!/^\d+$/.test(raw) || !Number.isSafeInteger(Number(raw))) {
    process.stderr.write(`[dispatch-stamp] ${name} must be a non-negative safe integer\n`);
    process.exit(2);
  }
  return Number(raw);
}

function buildTokenUsage() {
  // Check for structured token flags first
  const tokenStatus = flagVal('--token-status');
  if (tokenStatus) {
    if (tokenStatus === 'exact') {
      const tu = { status: 'exact', source: flagVal('--token-source') || 'harness' };
      const input = optionalNonNegativeInteger('--token-input');
      const output = optionalNonNegativeInteger('--token-output');
      const cacheRead = optionalNonNegativeInteger('--token-cache-read');
      const cacheWrite = optionalNonNegativeInteger('--token-cache-write');
      const reportedTotal = optionalNonNegativeInteger('--token-total');
      if (input !== undefined) tu.input = input;
      if (output !== undefined) tu.output = output;
      if (cacheRead !== undefined) tu.cacheRead = cacheRead;
      if (cacheWrite !== undefined) tu.cacheWrite = cacheWrite;
      if (input === undefined && output === undefined && reportedTotal === undefined) {
        process.stderr.write('[dispatch-stamp] exact token usage requires --token-total or input/output values\n');
        process.exit(2);
      }
      if (input !== undefined && output !== undefined) {
        const computedTotal = input + output;
        if (reportedTotal !== undefined && reportedTotal !== computedTotal) {
          process.stderr.write('[dispatch-stamp] --token-total must equal input + output when both are provided\n');
          process.exit(2);
        }
        tu.total = computedTotal;
      } else if (reportedTotal !== undefined) {
        tu.total = reportedTotal;
      }
      if (tu.total === 0) tu.note = 'harness-reported-zero';
      return tu;
    } else if (tokenStatus === 'unavailable') {
      return { status: 'unavailable', reason: flagVal('--token-reason') || 'harness-did-not-report-token-usage' };
    } else {
      process.stderr.write(`[dispatch-stamp] --token-status must be "exact" or "unavailable"\n`);
      process.exit(2);
    }
  }

  // Legacy: --tokens <n> (pre-cutoff backward compat)
  const tokensStr = flagVal('--tokens');
  if (tokensStr !== undefined) {
    if (!/^\d+$/.test(tokensStr) || !Number.isSafeInteger(Number(tokensStr))) {
      process.stderr.write('[dispatch-stamp] --tokens must be a non-negative safe integer\n');
      process.exit(2);
    }
    const tokens = Number(tokensStr);
    if (tokens > 0) {
      return { status: 'exact', total: tokens, source: 'harness' };
    } else {
      // n===0 → unavailable (never encode unknown as zero)
      return { status: 'unavailable', reason: 'legacy-zero-value' };
    }
  }

  // Neither provided — unavailable
  return { status: 'unavailable', reason: 'no-token-flags-provided' };
}

function run() {
  const dispatchId = required('--dispatch-id');
  const actor = required('--actor');
  const phase = required('--phase');
  const durationMs = parseInt(required('--duration-ms'), 10);
  const toolUses = parseInt(required('--tool-uses'), 10);
  const terminalStatus = required('--terminal-status');
  const ts = new Date().toISOString();

  if (isNaN(durationMs) || isNaN(toolUses)) {
    process.stderr.write('[dispatch-stamp] duration-ms, tool-uses must be integers\n');
    process.exit(2);
  }

  const changeId = resolveChangeId();
  const tpDir = taskPackageDir();
  if (!tpDir || !changeId) {
    process.stderr.write('[dispatch-stamp] no TaskPackage context; skipping dispatch stamp\n');
    process.exit(0);
  }

  mkdirSync(tpDir, { recursive: true });

  const tokenUsage = buildTokenUsage();

  if (isPostEventsCutoff(changeId)) {
    // Post-cutoff: write to pod-events.jsonl ONLY (F1: no dispatch-log.jsonl)
    try {
      const result = appendPodEvent(changeId, {
        eventType: 'dispatch-completed',
        actor,
        source: 'dispatch-stamp.mjs',
        payload: {
          dispatchId,
          phase,
          durationMs,
          toolUses,
          terminalStatus,
          tokenUsage,
        },
      });
      process.stderr.write(`[dispatch-stamp] ${dispatchId}: ${actor}/${phase} ${durationMs}ms -> pod-events.jsonl\n`);
    } catch (e) {
      process.stderr.write(`[dispatch-stamp] pod-events append error: ${e.message}\n`);
      process.exit(1);
    }
  } else {
    // Pre-cutoff: legacy dispatch-log.jsonl behavior
    const tokens = tokenUsage.status === 'exact' ? (tokenUsage.total || 0) : 0;
    const record = {
      dispatchId,
      actor,
      phase,
      durationMs,
      tokens,
      toolUses,
      terminalStatus,
      ts,
    };

    const logPath = resolve(tpDir, 'dispatch-log.jsonl');
    appendFileSync(logPath, `${JSON.stringify(record)}\n`);
    process.stderr.write(`[dispatch-stamp] ${dispatchId}: ${actor}/${phase} ${durationMs}ms ${tokens}tok -> ${logPath}\n`);
  }

  process.exit(0);
}

run();
