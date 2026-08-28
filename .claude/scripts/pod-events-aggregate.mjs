#!/usr/bin/env node
// On-demand pod-events aggregation: reads pod-events.jsonl for a given changeId,
// computes token totals (exact-only), coverage, escalation summary, friction summary.
// Skips malformed lines with stderr diagnostic (fail-open).
//
// OWNER: architect (G3 enforcement layer — advisory/non-authority)
// PACKAGE: gov-review-retro-convergence (Package B, FROZEN 2026-07-25)
//
// Usage:
//   node .claude/scripts/pod-events-aggregate.mjs --change-id <id>
//   node .claude/scripts/pod-events-aggregate.mjs  (uses current TaskPackage)
//
// Output: JSON to stdout

import { existsSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function flagVal(name) {
  const i = process.argv.indexOf(name);
  return i >= 0 ? process.argv[i + 1] : undefined;
}

function resolveChangeId() {
  const explicit = flagVal('--change-id');
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

function readEventsFile(eventsPath) {
  if (!existsSync(eventsPath)) return { events: [], malformedLines: 0 };

  const text = readFileSync(eventsPath, 'utf8');
  const lines = text.split(/\r?\n/).filter(l => l.trim());
  const events = [];
  let malformedLines = 0;

  for (const line of lines) {
    try {
      events.push(JSON.parse(line));
    } catch {
      malformedLines++;
      process.stderr.write(`[pod-events-aggregate] skipping malformed line: ${line.slice(0, 80)}...\n`);
    }
  }

  return { events, malformedLines };
}

function aggregateTokens(events) {
  const dispatches = events.filter(e => e.eventType === 'dispatch-completed');
  let dispatchesTotal = dispatches.length;
  let dispatchesWithTokenData = 0;
  let dispatchesUnavailable = 0;
  let totalInput = 0;
  let totalOutput = 0;
  let totalCacheRead = 0;
  let totalCacheWrite = 0;
  let totalTokens = 0;

  for (const d of dispatches) {
    const tu = d.payload && d.payload.tokenUsage;
    if (!tu) {
      dispatchesUnavailable++;
      continue;
    }
    if (tu.status === 'exact') {
      dispatchesWithTokenData++;
      if (typeof tu.input === 'number') totalInput += tu.input;
      if (typeof tu.output === 'number') totalOutput += tu.output;
      if (typeof tu.cacheRead === 'number') totalCacheRead += tu.cacheRead;
      if (typeof tu.cacheWrite === 'number') totalCacheWrite += tu.cacheWrite;
      if (typeof tu.total === 'number') totalTokens += tu.total;
    } else if (tu.status === 'unavailable') {
      dispatchesUnavailable++;
    } else {
      // Unknown status — treat as unavailable, fail-open
      dispatchesUnavailable++;
    }
  }

  return {
    dispatchesTotal,
    dispatchesWithTokenData,
    dispatchesUnavailable,
    tokenTotals: {
      input: totalInput,
      output: totalOutput,
      cacheRead: totalCacheRead,
      cacheWrite: totalCacheWrite,
      total: totalTokens,
    },
  };
}

function aggregateEscalations(events) {
  const escalations = events.filter(e => e.eventType === 'agent-escalation');
  const resolutions = events.filter(e => e.eventType === 'agent-escalation-resolved');
  const resolvedIds = new Set(resolutions.map(r => r.payload && r.payload.refEventId));

  const byCategory = {};
  const unresolved = [];

  for (const esc of escalations) {
    const cat = (esc.payload && esc.payload.category) || 'unknown';
    byCategory[cat] = (byCategory[cat] || 0) + 1;
    if (!resolvedIds.has(esc.eventId)) {
      unresolved.push({
        eventId: esc.eventId,
        category: cat,
        summary: esc.payload && esc.payload.summary,
        severity: esc.payload && esc.payload.severity,
      });
    }
  }

  return {
    total: escalations.length,
    resolved: resolutions.length,
    unresolved,
    byCategory,
  };
}

function aggregateFriction(events) {
  const frictions = events.filter(e => e.eventType === 'friction');
  const byType = {};

  for (const f of frictions) {
    const ft = (f.payload && f.payload.frictionType) || 'unknown';
    byType[ft] = (byType[ft] || 0) + 1;
  }

  return {
    total: frictions.length,
    byType,
  };
}

function run() {
  const changeId = resolveChangeId();
  if (!changeId) {
    process.stderr.write('[pod-events-aggregate] no changeId context\n');
    console.log(JSON.stringify({ ok: false, reason: 'no-change-id' }));
    process.exit(0);
  }

  const eventsPath = resolve(projectDir, '.claude', 'taskpackages', changeId, 'pod-events.jsonl');
  const { events, malformedLines } = readEventsFile(eventsPath);

  const result = {
    ok: true,
    changeId,
    eventsTotal: events.length,
    malformedLines,
    tokens: aggregateTokens(events),
    escalations: aggregateEscalations(events),
    friction: aggregateFriction(events),
  };

  console.log(JSON.stringify(result, null, 2));
  process.exit(0);
}

run();
