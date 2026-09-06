#!/usr/bin/env node
// Dev-loop artifact gate：origin 一等强键 + changeId 日期前缀的结构保质（devloop-artifact-check）。
// 这是 G3 standalone gate，对偶 taskpackage-required-check.mjs / closeout-check.mjs。
//
// 机检口径分层（gov-review-devloop-closeout-hardening §3.5 / Q6，2026-07-27 human-approved）:
//   scope: origin/changeId SHAPE ONLY（origin.type 值域 / origin.ref 合法性 / changeId 前缀）。
//   field completeness（必填字段 + decisionCoverage 承重校验）see taskpackage-required-check.mjs（承重闸）
//   + closeout-check.mjs terminal（closeout 相位程序化调用承重闸单源）。
//   本闸 ok:true 不作字段完整性证据（Q6：只加分层说明、不加 decisionCoverage 检查，避免两闸逻辑重复维护漂移）。
//   键名收敛（ARQ-077，260731-gov-review-artifact-lifecycle-origin-teeth Phase 2 T2 landing, 2026-08-01）：本闸 origin 类型键统一读
//   `origin.upstreamType ∨ origin.type`（对齐 taskpackage-required-check / closeout-check）；路径谓词键解析统一 `origin.upstreamRef ∨ origin.ref`。
//   原「只读 origin.type」不对称本批消除。
//
// 硬失败（exit 1）:
//   1. origin.type 值域不符（9 sourceType 枚举 + 受控扩展 human-confirmed-plan）。
//   2. origin.type=proposal 时 origin.ref 缺失 / 非合法 proposal .md 路径。
//   3. changeId 有 YYMMDD- 前缀但日期字面非法（月 01–12、日 01–31）。
//   4. 有新包形态（有前缀 or 已落 origin）但 changeId 缺 YYMMDD- 前缀（白名单不匹配即新包未遵循前缀规则）。
// advisory（不失败）:
//   - origin 与 sourceType 并存但取值不一致（过渡期告警，不红——兼容老包）。
//   - hybrid：ref 是 PROPOSAL 路径但 origin.type 非 proposal（shape 闸仅 advisory，H2/CF-02a）。
//   - origin.type=proposal 但 origin.upstreamFrozen 为空/占位符（过渡期告警）。
//   - 存 sourceType 但无 origin（过渡期告警——鼓励迁移）。
// 零追溯（白名单谓词，单一语义，见 DP-2）:
//   changeId 无前缀 → git 首提日期 < CUTOFF(2026-07-23) = 老包放行；≥ CUTOFF = 新包强制前缀。
//   cutoff 谓词单源（F-03）：convention 级同步清单 = 四文件——closeout-check.mjs / taskpackage-required-check.mjs /
//   devloop-artifact-check.mjs（本文件）/ hooks/supervisor-gate-check.mjs，禁各自另造（CHANGE_ID_CUTOFF 同值 '2026-07-23'）。
//
// 用法：node .claude/scripts/devloop-artifact-check.mjs [path/to/taskpackage.json]

import { existsSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { execSync } from 'node:child_process';
import { isProposalMdPath, normalizeSourceRef, resolveOriginFrozen, resolveOriginRef, resolveOriginType } from './lib/governance-gate-shared.mjs';
import { CHANGE_ID_PREFIX_RE } from './lib/changeid-shared.mjs'; // T1 单源收敛（260906-gov-review-arq-governance-batch）

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function taskPackagePath() {
  const explicit = process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH;
  if (explicit) return resolve(projectDir, explicit);
  return resolve(projectDir, '.claude/taskpackages/current/taskpackage.json');
}

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'devloop-artifact', message, ...details });
  process.exit(1);
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
}

function nonEmptyString(value) {
  return typeof value === 'string' && value.trim().length > 0;
}

// ── origin 一等强键 ──
const SOURCE_TYPES = [
  'delivery-story', 'planning-workstream', 'arch-review', 'human-ad-hoc',
  'ci-failure', 'drift-report', 'regression', 'maintenance', 'proposal',
];
const ORIGIN_TYPES = [...SOURCE_TYPES, 'human-confirmed-plan'];

// ── changeId 日期前缀 + 零追溯白名单（DP-2 / F8，单一语义，与 closeout-check.mjs 共享同一谓词）──
const CHANGE_ID_CUTOFF = '2026-07-23'; // landing cutoff（首提 < 此日 = 老包放行）

function validDatePrefix(yyMMdd) {
  const month = Number(yyMMdd.slice(2, 4));
  const day = Number(yyMMdd.slice(4, 6));
  return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

function gitFirstCommitDate(relPath) {
  try {
    const out = execSync(`git log --diff-filter=A --follow --format=%cs -- "${relPath}"`, {
      cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
    });
    const lines = out.split(/\r?\n/).map(l => l.trim()).filter(Boolean);
    return lines.length ? lines[lines.length - 1] : null; // earliest commit = last line with --follow
  } catch {
    return null;
  }
}

// ── main ──

const pathToTaskPackage = taskPackagePath();

if (!existsSync(pathToTaskPackage)) {
  print({ ok: true, gate: 'devloop-artifact', skipped: true, reason: 'no TaskPackage context' });
  process.exit(0);
}

let taskPackage;
try {
  taskPackage = readJson(pathToTaskPackage);
} catch (error) {
  fail('TaskPackage is not valid JSON', { taskPackage: pathToTaskPackage, error: String(error.message || error) });
}

const errors = [];
const advisories = [];
const changeId = taskPackage.changeId;
const hasChangeId = nonEmptyString(changeId);
const origin = (taskPackage.origin && typeof taskPackage.origin === 'object') ? taskPackage.origin : null;
const originType = resolveOriginType(origin);
const hasSourceType = nonEmptyString(taskPackage.sourceType);

// ── 1. origin.type 值域（若存在）──
if (originType && !ORIGIN_TYPES.includes(originType)) {
  errors.push(`origin.type must be one of: ${ORIGIN_TYPES.join('|')}; got ${originType}`);
}

// ── 2. origin 与 sourceType 并存一致性（advisory，过渡）──
if (originType && hasSourceType) {
  if (originType === 'human-confirmed-plan') {
    advisories.push('origin.type=human-confirmed-plan present alongside sourceType (this value has no sourceType mirror; expected sourceType absent)');
  } else if (originType !== taskPackage.sourceType) {
    advisories.push(`origin.type (${originType}) != sourceType (${taskPackage.sourceType}); should converge (transition advisory, no fail)`);
  }
}

// ── 3. proposal 型 → origin.ref 必填 + 合法性；hybrid（路径枢轴）→ advisory（Phase 2 T1/T2 + H2）──
const proposalRefRaw = resolveOriginRef(origin);
const isProposalByPath = !!proposalRefRaw && isProposalMdPath(normalizeSourceRef(proposalRefRaw));
if (originType === 'proposal') {
  const ref = proposalRefRaw ? normalizeSourceRef(proposalRefRaw) : null;
  if (!ref) {
    errors.push('origin.ref is required for origin.type=proposal');
  } else if (!isProposalMdPath(ref)) {
    errors.push(`origin.ref ${ref} is not a valid proposal .md path (expect changes/(arch|dev)/<id>/PROPOSAL.md or .planning/<ws>/PROPOSAL.md)`);
  }
} else if (isProposalByPath) {
  // hybrid（ref 是 PROPOSAL 路径但 type 非 proposal）：shape 闸仅 advisory（H2 / CF-02a）
  advisories.push(`origin.upstreamRef/ref ${proposalRefRaw} is a PROPOSAL path but origin.type=${originType || '(absent)'} (hybrid provenance); shape-only advisory, field completeness see taskpackage-required-check`);
}

// ── 4. origin.upstreamFrozen (proposal 型 ∨ 路径枢轴 & empty → advisory) ──
if ((originType === 'proposal' || isProposalByPath) && origin && !nonEmptyString(resolveOriginFrozen(origin))) {
  advisories.push('origin.upstreamFrozen (∨ frozen mirror key) is empty for a proposal-type/path-pivot origin (should pin FROZEN version for UPSTREAM_DRIFT detection)');
}

// ── 5. changeId 日期前缀 ──
if (hasChangeId) {
  const changeIdStr = String(changeId);
  const prefixMatch = CHANGE_ID_PREFIX_RE.exec(changeIdStr);

  if (prefixMatch) {
    // 有前缀 → 校验日期字面合法性
    if (!validDatePrefix(prefixMatch[1])) {
      errors.push(`changeId YYMMDD prefix date literal is invalid (month 01-12, day 01-31): ${prefixMatch[1]}`);
    }
    // 有前缀 = 新包形状 — good, validated
  } else {
    // 无前缀 → 判定是否是 grandfathered 老包（零追溯白名单）
    let isGrandfathered = false;
    let grandfatheredReason = '';

    // 用 changes/dev/<changeId> 的 git 首提日期判定（taskpackage 可能尚未落 dev 但用同一 changeId）
    // 先试 changes/dev，若不存在则试 .claude/taskpackages/<changeId>
    const devPath = `changes/dev/${changeIdStr}`;
    const tpPath = `.claude/taskpackages/${changeIdStr}`;
    let firstCommit = gitFirstCommitDate(devPath);
    if (firstCommit === null) {
      firstCommit = gitFirstCommitDate(tpPath);
    }

    if (firstCommit === null) {
      // git 不可用 / 新 dir 无历史 → 降级放行（convention 兜底，不误红）
      isGrandfathered = true;
      grandfatheredReason = 'git unavailable or no commit history — degraded, treat as grandfathered';
    } else if (firstCommit < CHANGE_ID_CUTOFF) {
      isGrandfathered = true;
      grandfatheredReason = `first commit ${firstCommit} < cutoff ${CHANGE_ID_CUTOFF}`;
    } else {
      grandfatheredReason = `first commit ${firstCommit} >= cutoff ${CHANGE_ID_CUTOFF}`;
    }

    if (!isGrandfathered) {
      errors.push(`changeId must have YYMMDD- date prefix for new packages (${grandfatheredReason}). Expected shape: YYMMDD-slug (e.g. 260722-foo-bar)`);
    }
  }
}

// ── 6. sourceType-without-origin advisory ──
if (hasSourceType && !originType) {
  advisories.push('package has sourceType but no origin — encouraged to migrate to origin (transition advisory, no fail)');
}

if (errors.length > 0) {
  fail('devloop artifact check failed', { taskPackage: pathToTaskPackage, changeId: changeId || null, errors, advisories });
}

print({
  ok: true,
  gate: 'devloop-artifact',
  scope: 'origin/changeId shape only; field completeness see taskpackage-required-check + closeout-check terminal',
  taskPackage: pathToTaskPackage,
  changeId: changeId || null,
  originType: originType || null,
  hasSourceType,
  advisories,
});
