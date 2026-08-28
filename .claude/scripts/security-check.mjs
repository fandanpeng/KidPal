#!/usr/bin/env node
// Validates security review evidence for TaskPackages that require it.

import { existsSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function taskPackagePath() {
  const explicit = process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH;
  if (explicit) return resolve(projectDir, explicit);

  const current = resolve(projectDir, '.claude/taskpackages/current/taskpackage.json');
  return existsSync(current) ? current : null;
}

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'security', message, ...details });
  process.exit(1);
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^\uFEFF/, ''));
}

function textEvidence(value) {
  if (!value) return '';
  if (typeof value === 'string') return value;
  return JSON.stringify(value);
}

const pathToTaskPackage = taskPackagePath();
if (!pathToTaskPackage) {
  print({ ok: true, gate: 'security', skipped: true, reason: 'no TaskPackage context' });
  process.exit(0);
}

if (!existsSync(pathToTaskPackage)) {
  fail('TaskPackage file does not exist', { taskPackage: pathToTaskPackage });
}

const taskPackage = readJson(pathToTaskPackage);
const securityReviewRequired = taskPackage.securityReviewRequired === true;

if (!securityReviewRequired) {
  print({ ok: true, gate: 'security', securityReviewRequired: false });
  process.exit(0);
}

const evidence = textEvidence(
  taskPackage.securityEvidence || taskPackage.securityReviewNotes || taskPackage.securityReview,
).trim();

if (evidence.length < 40) {
  fail('securityReviewRequired=true requires securityEvidence/securityReviewNotes', {
    taskPackage: pathToTaskPackage,
  });
}

const requiredAxes = Array.isArray(taskPackage.securityAxes) ? taskPackage.securityAxes : [];
const missingAxes = requiredAxes.filter((axis) => !evidence.toLowerCase().includes(String(axis).toLowerCase()));

if (missingAxes.length > 0) {
  fail('security evidence does not mention all declared securityAxes', { missingAxes });
}

print({
  ok: true,
  gate: 'security',
  taskPackage: pathToTaskPackage,
  securityReviewRequired: true,
  securityAxes: requiredAxes,
});