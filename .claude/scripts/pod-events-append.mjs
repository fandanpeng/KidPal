#!/usr/bin/env node
// Shared pod-events append utility: validates envelope, generates eventId, enforces
// closed event type set, appends validated JSON line to pod-events.jsonl.
// Used by dispatch-stamp.mjs, journal-append.mjs, and planner escalation bridge.
//
// OWNER: architect (G3 enforcement layer — advisory/non-authority)
// PACKAGE: gov-review-retro-convergence (Package B, FROZEN 2026-07-25)
//
// Usage (programmatic — called by other scripts, not directly by agents):
//   import { appendPodEvent } from './pod-events-append.mjs';
//   await appendPodEvent(changeId, { eventType, actor, source, payload });
//
// CLI usage (for planner escalation bridge):
//   node .claude/scripts/pod-events-append.mjs \
//     --change-id <id> --event-type <type> --actor <name> --source <name> \
//     --payload '<json>'

import { appendFileSync, existsSync, mkdirSync } from 'node:fs';
import { resolve } from 'node:path';
import { randomUUID } from 'node:crypto';
import { echoResolution, resolveCurrentTaskPackage } from './lib/current-pointer-shared.mjs'; // T6 回显齿（260924-gov-review-gate-parse-teeth）

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// Package B landing date — pods with date-prefix >= this use new format
export const EVENTS_SCHEMA_CUTOFF = '2026-07-25';

// Closed event type set (v1)
const VALID_EVENT_TYPES = new Set([
  'dispatch-completed',
  'agent-escalation',
  'agent-escalation-resolved',
  'human-decision',
  'human-correction',
  'friction',
]);

/**
 * Validate and append a pod-event to the pod-events.jsonl for the given changeId.
 * Returns { ok, eventId } on success, throws on validation failure.
 * If no TaskPackage context exists, returns { ok: true, skipped: true }.
 */
export function appendPodEvent(changeId, { eventType, actor, source, payload }) {
  if (!changeId || typeof changeId !== 'string') {
    throw new Error('[pod-events-append] changeId is required');
  }

  if (!VALID_EVENT_TYPES.has(eventType)) {
    throw new Error(`[pod-events-append] invalid eventType "${eventType}"; must be one of: ${[...VALID_EVENT_TYPES].join(', ')}`);
  }

  if (!actor || typeof actor !== 'string') {
    throw new Error('[pod-events-append] actor is required');
  }

  if (!source || typeof source !== 'string') {
    throw new Error('[pod-events-append] source is required');
  }

  if (!payload || typeof payload !== 'object') {
    throw new Error('[pod-events-append] payload must be an object');
  }

  const tpDir = resolve(projectDir, 'delivery', 'taskpackages', changeId);
  if (!existsSync(tpDir)) {
    // No TaskPackage directory — skip gracefully (exit 0 behavior)
    return { ok: true, skipped: true, reason: 'no-taskpackage-dir' };
  }

  const eventId = randomUUID();
  const timestamp = new Date().toISOString();

  const envelope = {
    schemaVersion: 1,
    eventId,
    changeId,
    timestamp,
    eventType,
    actor,
    source,
    payload,
  };

  const eventsPath = resolve(tpDir, 'pod-events.jsonl');
  appendFileSync(eventsPath, `${JSON.stringify(envelope)}\n`);

  return { ok: true, eventId };
}

// ── CLI mode ──
function flagVal(name) {
  const i = process.argv.indexOf(name);
  return i >= 0 ? process.argv[i + 1] : undefined;
}

function resolveChangeId() {
  const explicit = flagVal('--change-id');
  if (explicit) {
    echoResolution('pod-events-append', explicit, '--change-id \u65D7\u6807');
    return explicit;
  }

  const resolved = resolveCurrentTaskPackage('pod-events-append', projectDir);
  return resolved.found ? resolved.changeId : null;
}

// Only run CLI if invoked directly (not imported)
const isMain = process.argv[1] && resolve(process.argv[1]) === resolve(import.meta.url.replace('file:///', '').replace('file://', ''));

if (isMain || (process.argv[1] && process.argv[1].endsWith('pod-events-append.mjs') && process.argv.length > 2)) {
  const eventType = flagVal('--event-type');
  const actor = flagVal('--actor');
  const source = flagVal('--source');
  const payloadStr = flagVal('--payload');
  const changeId = resolveChangeId();

  if (!changeId) {
    process.stderr.write('[pod-events-append] no changeId context; skipping\n');
    process.exit(0);
  }

  if (!eventType || !actor || !source) {
    process.stderr.write('[pod-events-append] --event-type, --actor, --source are required\n');
    process.exit(2);
  }

  let payload = {};
  if (payloadStr) {
    try {
      payload = JSON.parse(payloadStr);
    } catch (e) {
      process.stderr.write(`[pod-events-append] --payload must be valid JSON: ${e.message}\n`);
      process.exit(2);
    }
  }

  try {
    const result = appendPodEvent(changeId, { eventType, actor, source, payload });
    if (result.skipped) {
      process.stderr.write(`[pod-events-append] skipped: ${result.reason}\n`);
    } else {
      process.stderr.write(`[pod-events-append] ${result.eventId}: ${eventType} by ${actor}\n`);
    }
    process.exit(0);
  } catch (e) {
    process.stderr.write(`[pod-events-append] ERROR: ${e.message}\n`);
    process.exit(1);
  }
}
