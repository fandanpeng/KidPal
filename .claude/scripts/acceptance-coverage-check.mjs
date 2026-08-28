#!/usr/bin/env node
// Validates that a TaskPackage has frozen acceptance tests with coverage markers.
// The script is intentionally conservative and dependency-free so it can run
// before the monorepo runtime is fully bootstrapped.
//
// gov-review-verifier-gate-infra-fixes（human-approved 2026-07-15）——按 PROPOSAL F4 落地。
// 只改 acceptanceId() 的短 id 提取；structural 白名单与 mentionsId 不动（CH-5 → ARQ-098）。
//
// 260731-gov-review-acceptance-coverage-structural-exemption（FROZEN v1.0，human-approved 2026-07-31）：
// structuralAcceptanceIds / mentionsId 收敛入 ./lib/acceptance-id-shared.mjs 单源（本地拷贝删净）；
// :84 governanceWords prose 洗白移除（恢复成文门义，dev-loop.md:156 早已明禁）；
// mentionsId 去裸后缀 = 力度变更（k8 碰撞闭合；区间缩写/prose 同物提及不再豁免——成文辨析见 dev-loop.md §3 A10 段）；
// 新增 suiteGate:true 一等豁免（套件级回归门显式声明），stdout 分印 structural/suiteGates 供审计。

import { existsSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { acceptanceId, structuralAcceptanceIds } from './lib/acceptance-id-shared.mjs'; // ARQ-097: F4 短 id 提取收敛单源；260731: structuralAcceptanceIds 同单源（原本地拷贝已删）

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
  print({ ok: false, gate: 'acceptance-coverage', message, ...details });
  process.exit(1);
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^\uFEFF/, ''));
}

function escapeRegExp(value) {
  return value.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

function hasCoverageMarker(content, acceptanceId) {
  const id = escapeRegExp(acceptanceId);
  return new RegExp(`(?:coverage|covers|@coverage)\\s*:?\\s*${id}\\b`, 'i').test(content)
    || new RegExp(`(?:describe|it|test)\\s*\\([^)]*${id}\\b`, 'i').test(content);
}

// extractShortId + acceptanceId 收敛入 ./lib/acceptance-id-shared.mjs（ARQ-097，F4 单源；import 见顶部）。hasCoverageMarker 仍用本地 escapeRegExp，未迁。

/**
 * Extract the short acceptance ref from a full acceptance string.
 * E.g. "TP-7-A1: S1 existing acceptance..." -> "TP-7-A1"
 *      "S1-A2" -> "S1-A2"
 * The short ref is the first whitespace-delimited or colon-terminated token
 * that matches the sourceRef pattern (alphanum-hyphen).
 */
function shortAcceptanceRef(fullId) {
  if (!fullId) return null;
  // If it's already a short ref (no colon, no long prose), return as-is
  const colonIdx = fullId.indexOf(':');
  if (colonIdx === -1 && fullId.length <= 20) return fullId;
  // Extract the token before the colon or first space
  const token = colonIdx >= 0 ? fullId.slice(0, colonIdx).trim() : fullId.split(/\s+/)[0];
  // Validate it looks like a ref: e.g. TP-7-A1, S1-A2, ARQ-063-C1, TEMP-*
  if (/^[A-Za-z]+[-\d]+/.test(token) && token.length >= 4) return token;
  return fullId;
}

function hasRealAssertion(content) {
  return /\bexpect\s*\(/.test(content)
    || /\bassert\s*[.(]/.test(content)
    || /\.to(?:Be|Equal|Have|Contain|Match|Throw)/.test(content);
}

function isShallowStatusOnly(content) {
  const checksStatus200 = /status(?:Code)?\)?\s*\.?(?:toBe|toEqual)?\s*\(?\s*200\b/i.test(content)
    || /\.status\s*\(\s*200\s*\)/i.test(content);
  if (!checksStatus200) return false;

  const hasBehaviorEvidence = /schema\.parse|safeParse|findUnique|findFirst|count\s*\(|emittedEvents|toContainEqual|locator\s*\(|toHaveText|toBeVisible|Audit|AiRun|correlationId|event/i.test(content);
  return !hasBehaviorEvidence;
}

const pathToTaskPackage = taskPackagePath();
if (!pathToTaskPackage) {
  print({ ok: true, gate: 'acceptance-coverage', skipped: true, reason: 'no TaskPackage context' });
  process.exit(0);
}

if (!existsSync(pathToTaskPackage)) {
  fail('TaskPackage file does not exist', { taskPackage: pathToTaskPackage });
}

const taskPackage = readJson(pathToTaskPackage);
const acceptanceRaw = (Array.isArray(taskPackage.acceptance) ? taskPackage.acceptance : [])
  .map(acceptanceId)
  .filter(Boolean);
const acceptance = acceptanceRaw.map((id) => shortAcceptanceRef(id) || id);
const files = Array.isArray(taskPackage.acceptanceTestsFile) ? taskPackage.acceptanceTestsFile : [];
const { structural, suiteGates } = structuralAcceptanceIds(taskPackage, acceptance);
const exempt = new Set([...structural, ...suiteGates]);
const requiresAcceptanceFiles = acceptance.some((id) => !exempt.has(id));

if (acceptance.length === 0) {
  fail('TaskPackage.acceptance must list at least one acceptance/source ref', { taskPackage: pathToTaskPackage });
}

if (requiresAcceptanceFiles && files.length === 0) {
  fail('TaskPackage.acceptanceTestsFile must list frozen acceptance test files', { taskPackage: pathToTaskPackage });
}

const missingFiles = [];
const fileContents = [];
for (const file of files) {
  const absolutePath = resolve(projectDir, file);
  if (!existsSync(absolutePath)) {
    missingFiles.push(file);
    continue;
  }

  const content = readFileSync(absolutePath, 'utf8');
  fileContents.push({ file, content });

  if (!hasRealAssertion(content)) {
    fail('Acceptance test file has no recognizable assertion', { file });
  }

  if (isShallowStatusOnly(content)) {
    fail('Acceptance test appears to assert only HTTP 200 without behavior evidence', { file });
  }
}

if (missingFiles.length > 0) {
  fail('Acceptance test files are missing', { missingFiles });
}

const missingCoverage = [];
for (const acceptanceId of acceptance) {
  if (exempt.has(acceptanceId)) continue;
  const covered = fileContents.some(({ content }) => hasCoverageMarker(content, acceptanceId));
  if (!covered) missingCoverage.push(acceptanceId);
}

if (missingCoverage.length > 0) {
  fail('Some acceptance ids are not covered by frozen acceptance tests', { missingCoverage });
}

print({
  ok: true,
  gate: 'acceptance-coverage',
  taskPackage: pathToTaskPackage,
  acceptance,
  structural: [...structural],
  suiteGates: [...suiteGates],
  files,
});
