#!/usr/bin/env node
// 最小 Pod Journal（层 A）事件追加器。
// 供 planner 调用，程序内部 fs-append round-outcome / friction 事件到
// .claude/taskpackages/<id>/pod-journal.jsonl。
// 对偶 dispatch-stamp.mjs / portfolio-reconcile.mjs（程序内 fs 写，非 shell 重定向）。
// 无 hash-chain / journalHeadHash（层 A 不上重机器）。
//
// Post-cutoff (EVENTS_SCHEMA_CUTOFF >= 250725):
//   - `friction` events redirect to pod-events.jsonl via pod-events-append.mjs
//   - `round-outcome` events are DROPPED (verifier evidence is authority; derived events retired)
//   - Does NOT write pod-journal.jsonl for post-cutoff Pods
//
// PACKAGE: gov-review-retro-convergence (Package B, FROZEN 2026-07-25)
//
// 用法：
//   node .claude/scripts/journal-append.mjs \
//     --event-type round-outcome --actor planner --round 2 --role verifier \
//     --ok true --caught "caught cross-spec pollution in gate 6" \
//     [--task-package-id <id>]
//
//   node .claude/scripts/journal-append.mjs \
//     --event-type friction --actor planner \
//     --friction-type stall --summary "planner: dispatch R3-fixer hung ~12min, no tool calls" \
//     [--task-package-id <id>]

import { appendFileSync, existsSync, mkdirSync, readFileSync } from 'node:fs';
import { basename, resolve } from 'node:path';
import { EVENTS_SCHEMA_CUTOFF, appendPodEvent } from './pod-events-append.mjs';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

const CHANGE_ID_PREFIX_RE = /^(\d{6})-[a-z0-9-]+$/;

function flagVal(name) {
  const i = process.argv.indexOf(name);
  return i >= 0 ? process.argv[i + 1] : undefined;
}

function required(name) {
  const v = flagVal(name);
  if (!v) {
    process.stderr.write(`[journal-append] missing required flag: ${name}\n`);
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
    const tp = JSON.parse(readFileSync(current, 'utf8').replace(/^﻿/, ''));
    return tp.changeId ? resolve(projectDir, '.claude', 'taskpackages', tp.changeId) : null;
  } catch {
    return null;
  }
}

// 闭集事件类型（层 A）
const VALID_EVENT_TYPES = new Set(['round-outcome', 'friction']);

function isPostEventsCutoff(changeId) {
  const m = CHANGE_ID_PREFIX_RE.exec(String(changeId));
  if (!m) return false;
  const prefix = m[1];
  const cutoffCompact = EVENTS_SCHEMA_CUTOFF.replace(/-/g, '').slice(2);
  return prefix >= cutoffCompact;
}

function run() {
  const eventType = required('--event-type');
  if (!VALID_EVENT_TYPES.has(eventType)) {
    process.stderr.write(`[journal-append] invalid event-type "${eventType}"; must be one of: ${[...VALID_EVENT_TYPES].join(', ')}\n`);
    process.exit(2);
  }

  const actor = required('--actor');
  const ts = new Date().toISOString();

  const tpDir = taskPackageDir();
  if (!tpDir) {
    process.stderr.write('[journal-append] no TaskPackage context; skipping journal append\n');
    process.exit(0);
  }

  const changeId = flagVal('--task-package-id') || basename(tpDir);

  // ── Post-cutoff redirect (Package B) ──
  if (isPostEventsCutoff(changeId)) {
    if (eventType === 'round-outcome') {
      // DROPPED: verifier evidence is authority; derived round-outcome events retired for post-cutoff
      process.stderr.write(`[journal-append] post-cutoff: round-outcome DROPPED (verifier evidence is authority)\n`);
      process.exit(0);
    }
    if (eventType === 'friction') {
      // Redirect to pod-events.jsonl
      const frictionType = required('--friction-type');
      const summary = required('--summary');
      try {
        const result = appendPodEvent(changeId, {
          eventType: 'friction',
          actor,
          source: 'journal-append.mjs',
          payload: { frictionType, summary, artifactRefs: [] },
        });
        process.stderr.write(`[journal-append] post-cutoff: friction -> pod-events.jsonl (${result.eventId || 'skipped'})\n`);
      } catch (e) {
        process.stderr.write(`[journal-append] post-cutoff pod-events error: ${e.message}\n`);
        process.exit(1);
      }
      process.exit(0);
    }
  }

  // ── Pre-cutoff: legacy pod-journal.jsonl behavior ──

  let summary = '';
  const artifactRefs = [];

  const event = {
    eventId: null,
    ts,
    actor,
    eventType,
    summary: '',
    artifactRefs: [],
  };

  if (eventType === 'round-outcome') {
    const round = parseInt(required('--round'), 10);
    const role = required('--role');
    const okStr = flagVal('--ok');
    const ok = okStr === 'true' ? true : okStr === 'false' ? false : null;
    const caught = flagVal('--caught') || '';

    if (isNaN(round)) {
      process.stderr.write('[journal-append] --round must be an integer\n');
      process.exit(2);
    }
    if (ok === null) {
      process.stderr.write('[journal-append] --ok required (true|false) for round-outcome\n');
      process.exit(2);
    }

    summary = `R${round} ${role} ${ok ? 'PASS' : 'FAIL'}${caught ? ': ' + caught : ''}`;
    artifactRefs.push(`verify-runs.jsonl#r${round}`);

    Object.assign(event, {
      eventId: `${changeId}#r${round}-${role}`,
      summary,
      artifactRefs,
      round,
      role,
      ok,
      authority: 'derived',
      sourceRef: 'verify-runs.jsonl',
      caught: caught || null,
    });
  } else if (eventType === 'friction') {
    const frictionType = required('--friction-type');
    summary = required('--summary');

    Object.assign(event, {
      eventId: `${changeId}#friction-${frictionType}-${ts.slice(0, 19).replace(/[:\-.T]/g, '')}`,
      summary,
      artifactRefs,
      frictionType,
    });
  }

  // bounded check: summary max 500 chars
  if (event.summary.length > 500) {
    process.stderr.write('[journal-append] WARNING: summary exceeds 500 chars; truncating\n');
    event.summary = event.summary.slice(0, 497) + '...';
  }

  mkdirSync(tpDir, { recursive: true });
  const journalPath = resolve(tpDir, 'pod-journal.jsonl');
  appendFileSync(journalPath, `${JSON.stringify(event)}\n`);

  process.stderr.write(`[journal-append] ${event.eventId}: ${eventType} -> ${journalPath}\n`);
  process.exit(0);
}

run();
