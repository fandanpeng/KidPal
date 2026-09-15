#!/usr/bin/env node
// Validates that architecture review findings needing implementation work have a dev-loop handoff.

import { existsSync, readFileSync } from 'node:fs';
import { basename, resolve } from 'node:path';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
const reviewDir = process.argv[2] ? resolve(projectDir, process.argv[2]) : null;

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'arch-review-handoff', message, ...details });
  process.exit(1);
}

function readText(path) {
  return readFileSync(path, 'utf8').replace(/^\uFEFF/, '');
}

function readJson(path) {
  return JSON.parse(readText(path));
}

function asArray(value) {
  if (!value) return [];
  return Array.isArray(value) ? value : [value];
}

function nonEmptyString(value) {
  return typeof value === 'string' && value.trim().length > 0;
}

function nonEmptyArray(value) {
  return Array.isArray(value) && value.length > 0;
}

function extractImplementationFindingIds(markdown) {
  const ids = new Set();
  const rows = markdown.split(/\r?\n/).filter((line) => /^\|\s*[^|]+\|/.test(line));

  for (const row of rows) {
    if (/^\|\s*-+\s*\|/.test(row) || /\bID\b/i.test(row)) continue;
    const cells = row.split('|').slice(1, -1).map((cell) => cell.trim());
    if (cells.length < 3) continue;

    const id = cells[0];
    const text = cells.join(' ').toUpperCase();
    const confirmed = /CONFIRMED/.test(text);
    const implementation = /FIX-(IMPLEMENTATION|DEV|CODE)|IMPLEMENTATION|DEV-LOOP|BUILDER|FIXER/.test(text);
    const notSpecOnly = !/FIX-SPEC/.test(text) || implementation;

    if (/^[A-Z]+-?\d+|^[FDA]-\d+|^F\d+|^ARQ-\d+/i.test(id) && confirmed && implementation && notSpecOnly) {
      ids.add(id);
    }
  }

  return [...ids];
}

if (!reviewDir) {
  fail('review directory argument is required', { usage: 'node .claude/scripts/arch-review-handoff-check.mjs delivery/changes/arch/arch-review-<spec>' });
}

if (!existsSync(reviewDir)) fail('review directory does not exist', { reviewDir });

const findingsPath = resolve(reviewDir, 'findings.md');
const reviewPacketPath = resolve(reviewDir, 'review-packet.md');
if (!existsSync(findingsPath) && !existsSync(reviewPacketPath)) {
  fail('review directory must contain findings.md or review-packet.md', { reviewDir });
}

const evidence = [findingsPath, reviewPacketPath]
  .filter((path) => existsSync(path))
  .map((path) => readText(path))
  .join('\n');
const requiredFindingIds = extractImplementationFindingIds(evidence);
const handoffPath = resolve(reviewDir, 'dev-loop-handoff.json');

if (requiredFindingIds.length === 0) {
  print({ ok: true, gate: 'arch-review-handoff', reviewDir, requiredFindingIds, skipped: true, reason: 'no confirmed implementation findings detected' });
  process.exit(0);
}

if (!existsSync(handoffPath)) {
  fail('confirmed implementation findings require dev-loop-handoff.json', { reviewDir, requiredFindingIds });
}

let handoff;
try {
  handoff = readJson(handoffPath);
} catch (error) {
  fail('dev-loop-handoff.json is not valid JSON', { handoff: handoffPath, error: String(error.message || error) });
}

const errors = [];
if (handoff.schemaVersion !== 1) errors.push('schemaVersion must be 1');
if (handoff.sourceType !== 'arch-review') errors.push('sourceType must be arch-review');
if (!nonEmptyString(handoff.reviewId)) errors.push('reviewId is required');
if (!nonEmptyArray(handoff.findings)) errors.push('findings[] is required');

const handoffFindings = new Map();
for (const finding of asArray(handoff.findings)) {
  if (!nonEmptyString(finding.findingId)) {
    errors.push('each finding requires findingId');
    continue;
  }
  handoffFindings.set(finding.findingId, finding);

  for (const field of ['sourceRefs', 'gateProfile', 'affectedSurfaces', 'acceptanceRefs', 'verificationHints']) {
    if (!nonEmptyArray(finding[field])) errors.push(`${finding.findingId}.${field} must be a non-empty array`);
  }
  if (!nonEmptyString(finding.severity)) errors.push(`${finding.findingId}.severity is required`);
  if (!['low', 'medium', 'high', 'critical'].includes(finding.riskTier)) errors.push(`${finding.findingId}.riskTier must be low|medium|high|critical`);
  if (!['none', 'milestone-affecting'].includes(finding.deliveryImpact)) errors.push(`${finding.findingId}.deliveryImpact must be none|milestone-affecting`);
  if (!['none', 'attach-existing', 'create-workstream', 'needs-triage'].includes(finding.planningRecommendation)) {
    errors.push(`${finding.findingId}.planningRecommendation must be none|attach-existing|create-workstream|needs-triage`);
  }
  if (!Array.isArray(finding.blockingDecisions)) errors.push(`${finding.findingId}.blockingDecisions must be an array`);
  if (!nonEmptyArray(finding.suggestedTaskPackages) && asArray(finding.blockingDecisions).length === 0) {
    errors.push(`${finding.findingId}.suggestedTaskPackages is required unless blocked by decisions`);
  }
}

const missing = requiredFindingIds.filter((id) => !handoffFindings.has(id));
if (missing.length > 0) errors.push(`handoff missing findings: ${missing.join(', ')}`);

if (errors.length > 0) {
  fail('dev-loop handoff is incomplete', { reviewDir, handoff: `delivery/changes/arch/${basename(reviewDir)}/dev-loop-handoff.json`, errors });
}

print({
  ok: true,
  gate: 'arch-review-handoff',
  reviewDir,
  handoff: `delivery/changes/arch/${basename(reviewDir)}/dev-loop-handoff.json`,
  requiredFindingIds,
  handoffFindingIds: [...handoffFindings.keys()],
});
