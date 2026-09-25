#!/usr/bin/env node
// Validates documentation-sync evidence for a dev-loop TaskPackage.
// This first version checks explicit doc refs and docs-sync acknowledgement;
// deeper semantic drift checks can be added without changing agent contracts.
//
// gov-review-verifier-gate-infra-fixes（human-approved 2026-07-15）——按 PROPOSAL F5 落地。
// fragment strip + acceptanceId 短 id 提取（与 F4 同款，维持 dependency-free 重复惯例）+
// CH-15 EISDIR 防御。
// 注（CH-14 证据基线标注）：mention 检查是「文件级 + 弱 token 命中」，非语义覆盖；
// 锚点存在性 strip 后不校验——既有弱点，非本次引入。

import { existsSync, readFileSync, statSync } from 'node:fs';
import { resolve } from 'node:path';
import { acceptanceId } from './lib/acceptance-id-shared.mjs'; // ARQ-097: F5.2/F4 短 id 提取收敛单源（原本地拷贝已删）
import { echoResolution, resolveCurrentTaskPackage, resolveExplicitTaskPackage } from './lib/current-pointer-shared.mjs'; // T6 回显齿（260924-gov-review-gate-parse-teeth）

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function taskPackagePath() {
  const explicit = process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH;
  if (explicit) return resolveExplicitTaskPackage('docs-sync-check', projectDir, explicit, '位置参数/环境变量').path;

  const resolved = resolveCurrentTaskPackage('docs-sync-check', projectDir);
  return existsSync(resolved.path) ? resolved.path : null;
}

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'docs-sync', message, ...details });
  process.exit(1);
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^\uFEFF/, ''));
}

function containsAny(content, values) {
  return values.some((value) => value && content.includes(value));
}

function escapeRegExp(value) {
  return value.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

function docMentionsAcceptance(content, acceptanceId) {
  if (content.includes(acceptanceId)) return true;
  const suffix = acceptanceId.includes('-') ? acceptanceId.split('-').at(-1) : null;
  return suffix ? new RegExp(`\\b${escapeRegExp(suffix)}\\b`).test(content) : false;
}

// extractShortId + acceptanceId 收敛入 ./lib/acceptance-id-shared.mjs（ARQ-097，F5.2/F4 单源；import 见顶部）。docMentionsAcceptance 仍用本地 escapeRegExp，未迁。

const pathToTaskPackage = taskPackagePath();
if (!pathToTaskPackage) {
  print({ ok: true, gate: 'docs-sync', skipped: true, reason: 'no TaskPackage context' });
  process.exit(0);
}

if (!existsSync(pathToTaskPackage)) {
  fail('TaskPackage file does not exist', { taskPackage: pathToTaskPackage });
}

const taskPackage = readJson(pathToTaskPackage);
const docsSyncRequired = taskPackage.docsSyncRequired !== false;
const docRefs = Array.isArray(taskPackage.docRefs) ? taskPackage.docRefs : [];
const acceptance = (Array.isArray(taskPackage.acceptance) ? taskPackage.acceptance : [])
  .map(acceptanceId)
  .filter(Boolean);
const docsNoChangeReason = taskPackage.docsNoChangeReason || taskPackage.docsSyncReason;

if (!docsSyncRequired) {
  const reason = taskPackage.docsSyncReason || taskPackage.docsNoChangeReason;
  if (!reason || String(reason).trim().length < 12) {
    fail('docsSyncRequired=false must include docsSyncReason/docsNoChangeReason', { taskPackage: pathToTaskPackage });
  }

  print({ ok: true, gate: 'docs-sync', docsSyncRequired: false, reason });
  process.exit(0);
}

if (docRefs.length === 0) {
  fail('docsSyncRequired=true requires TaskPackage.docRefs[]', { taskPackage: pathToTaskPackage });
}

const missingDocs = [];
const docsWithoutAcceptance = [];
const docsCopyingSchema = [];

for (const docRef of docRefs) {
  // F5.1: docRef 锚点惯例（path.md#6.1）——strip fragment 后再做文件检查；错误消息保留原始 docRef
  const filePart = String(docRef).split('#')[0];
  // CH-15 实现注记：纯 fragment（"#6.1"）strip 后为空 → 干净红，不进 resolve
  if (!filePart.trim()) {
    missingDocs.push(docRef);
    continue;
  }
  const absolutePath = resolve(projectDir, filePart);
  // CH-15 实现注记：目录型 docRef → 干净红（missingDocs）而非 readFileSync EISDIR crash
  if (!existsSync(absolutePath) || !statSync(absolutePath).isFile()) {
    missingDocs.push(docRef);
    continue;
  }

  const content = readFileSync(absolutePath, 'utf8');
  const mentionsAcceptance = acceptance.length === 0
    || containsAny(content, acceptance)
    || acceptance.some((acceptanceId) => docMentionsAcceptance(content, acceptanceId));

  if (!mentionsAcceptance && !docsNoChangeReason) {
    docsWithoutAcceptance.push(docRef);
  }

  if (/model\s+\w+\s*\{|@@schema\(|@relation\(|Unsupported\("vector/i.test(content)) {
    docsCopyingSchema.push(docRef);
  }
}

if (missingDocs.length > 0) {
  fail('docRefs contain missing files', { missingDocs });
}

if (docsWithoutAcceptance.length > 0) {
  fail('docRefs do not mention any declared acceptance ids', { docsWithoutAcceptance, acceptance });
}

if (docsCopyingSchema.length > 0) {
  fail('docs appear to copy contract schema details instead of referencing contracts', { docsCopyingSchema });
}

print({
  ok: true,
  gate: 'docs-sync',
  taskPackage: pathToTaskPackage,
  docRefs,
  acceptance,
  docsNoChangeReason: docsNoChangeReason || null,
});
