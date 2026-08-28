#!/usr/bin/env node
// Checks integrator closeout artifacts once a change is ready for the merge gate.
//
// C4 (gov-review-devloop-evidence-led, FROZEN 2026-07-24):
//   - retired test-evidence.md, final-summary.md from requiredFiles (machine data → verdict.json + verification.json)
//     【F-10 复活规则（程序条款 / pending）】test-evidence.md 在「机器产线未运行」时复活为手工替代件
//     （verifier 亲跑命令 + 输出摘要 + exit code，即 260726 实际形态）——此为程序条款（降级手工替代集）/pending，
//     requiredFiles:228-230 逻辑不变（机器产线运行时仍退役；避免注释↔代码主动分歧，对偶所攻 devloop-artifact 文本↔行为同构）。
//   - taskpackage.json conditional: required for legacy (pre-cutoff) packages only;
//     post-cutoff packages resolve TaskPackage from .claude/taskpackages/<id>/ (gov-review-taskpkg-lifecycle, FROZEN 2026-07-25)
//   - verification.json advisory check added
// closeout-hardening (gov-review-devloop-closeout-hardening, 2026-07-27 human-approved, Q5=warn):
//   - closeout 制品 SSOT（dev-loop.md「closeout 制品 SSOT」表为承重/blocking 状态单一权威，本头注与之对齐、不重述分类）：
//       verifier-round fact（verify.mjs round 证据）= 承重机器件；supervisor-verdict.json = 唯一 blocking 机器件（supervisor Stop 门 G2）；
//       verdict.json / verification.json = 投影·advisory（non-bearing，本脚本 machineEvidenceFiles 维持 advisory-only）。
//   - F-04：closeout 相位程序化调用 taskpackage-required-check（承重闸单源），本体删 decisionCoverage/story 浅版重实现（本无，不新增），仅留 pod-retro 牙。
//   - pod-retro.md 存在性牙（post-cutoff，Q5=warn advisory + 零追溯白名单；verifier 闸族 closeout-check 门内加项，非新增工程门 F-09）。
//   - F-13：loop-retro pending 计数 advisory 提示位（调 computeRetroScope 不复制；只 advisory 不红、不新增工程门）。
//   - F-05：本脚本 proposal-条件逻辑键恒读 lib 单源 `resolveOriginType`（governance-gate-shared.mjs；多份拷贝已收敛单源，cross-surface INV-7 机械防回退）。
// Package B (gov-review-retro-convergence, FROZEN 2026-07-25):
//   - Post-EVENTS_SCHEMA_CUTOFF (>= 250725): session-digest.md, loop-health.md, loop-metrics.json
//     removed from required/advisory (retired for new-format Pods; pod-events.jsonl is advisory check)
//   - Layered relaxation on Package A (self-modification acknowledged, human-approved F9)
// OWNER: architect (G3 enforcement layer)
// metrics-ledger advisory section (loop-metrics.json + pod-journal.jsonl) remains untouched below
// - changeIdArchiveCandidates 改 YYMMDD- 前缀感知（原按日期后缀 -YYYY-MM-DD 剥）
// - 新增 UPSTREAM_DRIFT detection（convention-only / warn，DP-4 / F3）
// - 新增 changes/INDEX.md 单行增量 upsert（best-effort；全量重建见 changes-index.mjs）
// - sourceType-without-origin 过渡告警（advisory，不红）

import { existsSync, readFileSync, readdirSync, writeFileSync } from 'node:fs';
import { basename, resolve } from 'node:path';
import { execSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { checkStatus } from './delivery-status.mjs';
import { EVENTS_SCHEMA_CUTOFF } from './pod-events-append.mjs';
import { computeRetroScope } from './loop-health-report.mjs';
import { computeFrozenPin, isFrozen, isProposalMdPath, normalizeSourceRef, resolveOriginFrozen, resolveOriginRef, resolveOriginType } from './lib/governance-gate-shared.mjs';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
const cliArgs = process.argv.slice(2);
const updateIndexRequested = cliArgs.includes('--update-index');
const readOnlyRequested = cliArgs.includes('--read-only');
const positionalArgs = cliArgs.filter((arg) => !arg.startsWith('--'));

function taskPackagePath() {
  const explicit = positionalArgs[0] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH;
  if (explicit && !explicit.startsWith('--')) return resolve(projectDir, explicit);

  const current = resolve(projectDir, '.claude/taskpackages/current/taskpackage.json');
  return existsSync(current) ? current : null;
}

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'closeout', message, ...details });
  process.exit(1);
}

const unknownFlags = cliArgs.filter((arg) => arg.startsWith('--') && !['--update-index', '--read-only'].includes(arg));
if (unknownFlags.length > 0) fail('unknown closeout-check flag(s)', { unknownFlags });
if (updateIndexRequested && readOnlyRequested) fail('--update-index and --read-only are mutually exclusive');

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
}

function readText(path) {
  return readFileSync(path, 'utf8').replace(/^﻿/, '');
}

function asArray(value) {
  if (!value) return [];
  return Array.isArray(value) ? value : [value];
}

function nonEmptyString(value) {
  return typeof value === 'string' && value.trim().length > 0;
}

// ── changeId 日期前缀 + 零追溯白名单谓词（DP-2 / F8，单一语义）──
// cutoff 谓词单源（F-03）：convention 级同步清单 = 四文件——closeout-check.mjs（本文件）/ taskpackage-required-check.mjs /
// devloop-artifact-check.mjs / hooks/supervisor-gate-check.mjs，禁各自另造（CHANGE_ID_CUTOFF 同值 '2026-07-23'）。
// 两层谓词（v0.3 据 F-03 统一措辞）：① 前缀比较为主（isPostCutoff，对齐 :225）；② 无前缀包走 gitFirstCommitDate 白名单。
const CHANGE_ID_CUTOFF = '2026-07-23';
const CHANGE_ID_PREFIX_RE = /^(\d{6})-[a-z0-9-]+$/;
// F-13 loop-retro 触发阈值（pending+waiting 合计；默认值，Q2 终裁前 provisional——或日历兜底如双周）。
const RETRO_TRIGGER_THRESHOLD = 3;

function validDatePrefix(yyMMdd) {
  const month = Number(yyMMdd.slice(2, 4));
  const day = Number(yyMMdd.slice(4, 6));
  return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

function stripPrefix(changeId) {
  const m = CHANGE_ID_PREFIX_RE.exec(String(changeId));
  return m ? String(changeId).slice(7) : String(changeId);
}

function gitFirstCommitDate(relPath) {
  try {
    const out = execSync(`git log --diff-filter=A --follow --format=%cs -- "${relPath}"`, {
      cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
    });
    const lines = out.split(/\r?\n/).map(l => l.trim()).filter(Boolean);
    return lines.length ? lines[lines.length - 1] : null;
  } catch {
    return null;
  }
}

// ── UPSTREAM_DRIFT（DP-4 / F3，convention-only/warn；Phase 2 T2/T3/T4 路径枢轴 + computeFrozenPin + sha1: skip）──
function isPlaceholderFrozen(frozen) {
  return /^(<|todo|tbd|placeholder|pending|none|wip|unset)/i.test(String(frozen));
}

function warnDrift(payload, taskPackage) {
  const origin = (taskPackage.origin && typeof taskPackage.origin === 'object') ? taskPackage.origin : null;
  if (!origin) return;
  // T2：路径枢轴早退（替换 upstreamType==='proposal' 字符串判）；键解析 upstreamRef ∨ ref
  const ref = resolveOriginRef(origin);
  if (!nonEmptyString(ref) || !isProposalMdPath(normalizeSourceRef(ref))) return;
  const frozen = resolveOriginFrozen(origin);
  if (!nonEmptyString(frozen) || isPlaceholderFrozen(frozen)) return;
  // CF-09：非 sha1: 前缀 pin → 过渡 convention-only-skip
  if (!frozen.startsWith('sha1:')) return;

  const refPath = resolve(projectDir, normalizeSourceRef(ref));
  if (!existsSync(refPath)) {
    // T3/H3：ref 缺失 → advisory + skip，不红拦
    payload.advisories.push(`UPSTREAM_DRIFT: origin.ref ${ref} does not exist; cannot compare frozen pin (drift cannot verify; advisory-only)`);
    return;
  }
  // T1/OQ5：共享 helper（行尾归一化 → producer/consumer 字节一致）
  const currentHash = computeFrozenPin(refPath);
  if (currentHash !== frozen) {
    payload.advisories.push(`UPSTREAM_DRIFT: ${ref} content-hash (${currentHash}) ≠ origin.upstreamFrozen (${frozen}). Upstream PROPOSAL may have drifted since this package was cut. UPSTREAM_DRIFT = convention-only/warn / ARQ-260723-artifact-lifecycle-teeth.`);
  }
}

// ── changes/INDEX.md 单行增量 upsert（best-effort；全量重建见 changes-index.mjs）──
function upsertIndexRow(changeId, taskPackage, archDir) {
  const indexPath = resolve(projectDir, 'changes', 'INDEX.md');
  if (!existsSync(indexPath)) return 'index-missing-skipped'; // 首次 closeout 需先跑 changes-index.mjs 产 header
  const origin = (taskPackage.origin && typeof taskPackage.origin === 'object') ? taskPackage.origin : null;
  const prefixMatch = String(changeId).match(CHANGE_ID_PREFIX_RE);
  const date = prefixMatch && prefixMatch[1]
    ? `20${prefixMatch[1].slice(0,2)}-${prefixMatch[1].slice(2,4)}-${prefixMatch[1].slice(4,6)}`
    : (gitFirstCommitDate(`changes/dev/${changeId}`) || '');
  const originRef = resolveOriginRef(origin) || '';
  const workstream = (origin && origin.workstream) || '';

  const newRow = `| [${changeId}](../changes/dev/${changeId}/) | ${date || '—'} | ${originRef || '—'} | closed | ${workstream || '—'} |`;

  let content = readText(indexPath);
  const originalContent = content;
  let action = 'inserted';
  // 如果已有此行 → 替换；否则在 dev section 末尾追加
  const escapedId = changeId.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
  const rowRe = new RegExp(`^\\| \\[${escapedId}\\]\\(.*?\\) \\|.*\\| closed \\|.*\\|$`, 'm');
  if (rowRe.test(content)) {
    content = content.replace(rowRe, newRow);
    action = 'replaced';
  } else {
    // 在 ## dev 与 ## arch 之间插入（追到 dev section 末尾）
    const devSectionStart = content.indexOf('\n## dev');
    const archSectionStart = content.indexOf('\n## arch', devSectionStart + 1);
    if (devSectionStart >= 0 && archSectionStart > devSectionStart) {
      content = content.slice(0, archSectionStart) + newRow + '\n' + content.slice(archSectionStart);
    } else {
      return 'index-header-skipped'; // fallback：header 不标准，skip
    }
  }
  if (content === originalContent) return 'unchanged';
  writeFileSync(indexPath, content, 'utf8');
  return action;
}

// ── changeIdArchiveCandidates（前缀感知版：识别并剥 YYMMDD- 前缀 + 兼容日期后缀 -YYYY-MM-DD + 纯 slug）──
function changeIdArchiveCandidates(changeId) {
  const str = String(changeId);
  const withoutDate = str.replace(/-\d{4}-\d{2}-\d{2}$/, ''); // 去后缀: slug-YYYY-MM-DD
  const noPrefix = stripPrefix(str);                           // 去前缀: YYMMDD-slug → slug

  const changesRoot = resolve(projectDir, 'changes', 'dev');
  const candidates = [resolve(changesRoot, str)];
  if (withoutDate !== str) candidates.push(resolve(changesRoot, withoutDate));
  if (noPrefix !== str) candidates.push(resolve(changesRoot, noPrefix));

  if (existsSync(changesRoot)) {
    for (const entry of readdirSync(changesRoot, { withFileTypes: true })) {
      if (!entry.isDirectory()) continue;
      const name = entry.name;
      if (str.startsWith(name) || name.startsWith(noPrefix) || name.startsWith(withoutDate)) {
        candidates.push(resolve(changesRoot, name));
      }
    }
  }

  return [...new Set(candidates)];
}

function findChangeDir(changeId) {
  return changeIdArchiveCandidates(changeId).find((candidate) => existsSync(candidate)) || null;
}

// ── F-04 承重闸程序化调用（taskpackage-required-check 单源；调用=单源≠重复，Q6 reconcile）──
// closeout-check 本体不再自实现 decisionCoverage/story 浅版终检——承重校验委托承重闸单源。
// 非 Claude harness 降级时手工跑该闸兜底承重（见 dev-loop.md「非 Claude harness 降级 profile」）。
function runBearingGate(gatePath, tpPath) {
  try {
    const out = execSync(`"${process.execPath}" "${gatePath}" "${tpPath}"`, {
      cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
    });
    return { ok: true, output: out };
  } catch (err) {
    return { ok: false, output: `${err.stdout || ''}${err.stderr || ''}`, status: err.status };
  }
}

// ── main ──

const pathToTaskPackage = taskPackagePath();
if (!pathToTaskPackage) {
  print({ ok: true, gate: 'closeout', skipped: true, reason: 'no TaskPackage context' });
  process.exit(0);
}

if (!existsSync(pathToTaskPackage)) fail('TaskPackage file does not exist', { taskPackage: pathToTaskPackage });

const taskPackage = readJson(pathToTaskPackage);
const changeId = taskPackage.changeId;
if (!changeId || typeof changeId !== 'string') fail('TaskPackage.changeId is required', { taskPackage: pathToTaskPackage });

const advisories = [];

// ── changeId 日期前缀自检（advisory，不红）──
const hasPrefix = CHANGE_ID_PREFIX_RE.test(String(changeId));
if (hasPrefix) {
  const prefixedDate = String(changeId).slice(0, 6);
  if (!validDatePrefix(prefixedDate)) {
    advisories.push(`changeId date prefix (${prefixedDate}) is not valid YYMMDD (month 01-12, day 01-31)`);
  }
} else {
  const firstCommit = gitFirstCommitDate(`changes/dev/${changeId}`) || gitFirstCommitDate(`.claude/taskpackages/${changeId}`);
  if (firstCommit && firstCommit >= CHANGE_ID_CUTOFF) {
    advisories.push(`changeId lacks YYMMDD- prefix but first commit ${firstCommit} >= cutoff ${CHANGE_ID_CUTOFF}; missing-date-prefix blocks at taskpackage-required gate for new packages`);
  }
}

const changeDir = findChangeDir(changeId);
if (!changeDir) fail('changes/dev/<changeId> folder is missing', { changeId, expected: `changes/dev/${changeId}` });

// Post-cutoff packages no longer require taskpackage.json in changes/dev/ (gov-review-taskpkg-lifecycle).
// TaskPackage is resolved from .claude/taskpackages/<id>/ for post-cutoff packages.
const isPostCutoff = hasPrefix && String(changeId).slice(0, 6) >= CHANGE_ID_CUTOFF.replace(/-/g, '').slice(2);
// Post-EVENTS_SCHEMA_CUTOFF: session-digest/loop-health/loop-metrics retired (Package B)
const isPostEventsCutoff = hasPrefix && String(changeId).slice(0, 6) >= EVENTS_SCHEMA_CUTOFF.replace(/-/g, '').slice(2);
const requiredFiles = isPostCutoff
  ? ['docs-delta.md', 'review-packet.md']
  : ['taskpackage.json', 'docs-delta.md', 'review-packet.md'];
const machineEvidenceFiles = ['verdict.json', 'verification.json'];
const missing = requiredFiles.filter((file) => !existsSync(resolve(changeDir, file)));
if (missing.length > 0) fail('changes/dev/<changeId> is missing standard closeout artifacts', { changeId, missing });

// Machine evidence (verdict.json / verification.json) — advisory, not blocking
const missingMachineEvidence = machineEvidenceFiles.filter((file) => !existsSync(resolve(changeDir, file)));
if (missingMachineEvidence.length > 0) {
  advisories.push(`machine evidence missing: ${missingMachineEvidence.join(', ')} — verdict/verification should be produced by scripts (not agents); advisory-only`);
}

// Post-EVENTS_SCHEMA_CUTOFF: pod-events.jsonl advisory check (Package B, non-blocking)
if (isPostEventsCutoff) {
  const podEventsPath = resolve(projectDir, '.claude', 'taskpackages', changeId, 'pod-events.jsonl');
  if (!existsSync(podEventsPath)) {
    advisories.push('pod-events.jsonl not found in .claude/taskpackages/<id>/ — new-format Pod should have pod-events (advisory-only, not blocking)');
  }
}

// ── post-cutoff pod-retro.md 存在性牙（gov-review-devloop-closeout-hardening §3.2 / Q5=warn / F-14/F-03）──
// closeout 相位承重牙 = verifier 闸族 closeout-check 门内加项（非新增工程门，F-09）。力度 = warn（Q5 human 已裁，本批）：
// advisory + 零追溯白名单（仅 post-cutoff 新包检查，pre-cutoff / 无前缀老包豁免）。
// 牙谓词钉死对齐 isPostCutoff:225 前缀谓词（随 F-03）——复用本文件单一 isPostCutoff 布尔，不另造谓词。
if (isPostCutoff) {
  const podRetroPath = resolve(changeDir, 'pod-retro.md');
  if (!existsSync(podRetroPath)) {
    advisories.push('pod-retro.md missing in changes/dev/<id>/ — planner first-person hot memory not recorded (Q5=warn advisory; zero-retroactivity: post-cutoff only). planner 收尾相位 should write it after integrator closeout, before supervisor gate.');
  }
}

// ── F-13 loop-retro pending 计数 advisory 提示位（§3.1 / Q2 复合触发）──
// 程序化调用 computeRetroScope（不复制其计数逻辑）；pending+waiting 合计 ≥ 阈值（或日历兜底）→ advisory 提示。
// 只 advisory 不红、不新增工程门（verifier 闸族 closeout-check 门内一行输出，F-09）；发起责任显式归主线程（防 advisory 无 owner）。
try {
  const scope = computeRetroScope(projectDir);
  const pendingPlusWaiting = scope.pendingAnalysis.length + scope.waitingOnLoopHealth.length;
  if (pendingPlusWaiting >= RETRO_TRIGGER_THRESHOLD) {
    advisories.push(`loop-retro trigger threshold reached: ${scope.pendingAnalysis.length} pending + ${scope.waitingOnLoopHealth.length} waiting = ${pendingPlusWaiting} ≥ ${RETRO_TRIGGER_THRESHOLD} (or calendar fallback, e.g. biweekly). 发起责任=主线程（run /loop-retro）。advisory-only, not a gate.`);
  }
} catch (e) {
  advisories.push(`loop-retro scope advisory skipped (computeRetroScope degraded: ${e.message || e}) — advisory-only, does not affect closeout`);
}

// ── F-04 closeout 相位程序化调用承重闸单源（taskpackage-required-check）──
// 承重校验（必填字段 + decisionCoverage 深校验 landsIn/acceptance 回查）委托承重闸单源；本体仅留 pod-retro 牙（上）。
// 调用 = 单源 ≠ 重复（Q6 reconcile）：不在 closeout-check 另写浅版「过终检却 fail 承重闸」，而是程序化复用承重闸深校验。
const bearingGatePath = resolve(projectDir, '.claude', 'scripts', 'taskpackage-required-check.mjs');
const bearing = runBearingGate(bearingGatePath, pathToTaskPackage);
if (!bearing.ok) {
  fail('closeout phase: bearing gate taskpackage-required-check FAILED (single source, F-04)', {
    changeId, bearingGate: 'taskpackage-required-check', output: String(bearing.output).slice(0, 2000),
  });
}

if (taskPackage.deliveryMilestoneStateChanged === true || taskPackage.milestoneStateChanged === true) {
  const milestones = resolve(projectDir, '.delivery/milestones.jsonl');
  const status = resolve(projectDir, '.delivery/STATUS.md');
  if (!existsSync(milestones) || !existsSync(status)) {
    fail('delivery milestone state changed but .delivery files are missing', { milestones: existsSync(milestones), status: existsSync(status) });
  }
  const statusCheck = checkStatus(projectDir);
  if (!statusCheck.ok) {
    fail('.delivery/STATUS.md is stale; run node .claude/scripts/delivery-status.mjs', {
      milestones: '.delivery/milestones.jsonl',
      status: '.delivery/STATUS.md',
    });
  }
}

if (taskPackage.planningWorkstreamRequired === true || taskPackage.workstreamRequired === true) {
  const refs = asArray(taskPackage.planningRefs || taskPackage.planningRef);
  if (refs.length === 0) fail('planning workstream required but TaskPackage has no planningRef(s)', { changeId });
  const missingRefs = refs.filter((ref) => !existsSync(resolve(projectDir, ref)));
  if (missingRefs.length > 0) fail('planningRef(s) are missing', { missingRefs });
}

// ── 过渡期告警（advisory）──
const origin = (taskPackage.origin && typeof taskPackage.origin === 'object') ? taskPackage.origin : null;
const hasOriginType = !!resolveOriginType(origin);
if (nonEmptyString(taskPackage.sourceType) && !hasOriginType) {
  advisories.push('package has sourceType but no origin — encouraged to migrate to origin (transition advisory, no fail)');
}

// ── UPSTREAM_DRIFT（convention-only/warn）──
warnDrift({ advisories }, taskPackage);

// ── leg ③ origin.ref FROZEN 守卫（OQ1-A/OQ2-A, warn-only; Phase 2 T3, H3 修订）──
const frozenGuardRef = resolveOriginRef(origin);
if (nonEmptyString(frozenGuardRef) && isProposalMdPath(normalizeSourceRef(frozenGuardRef))) {
  const proposalPath = resolve(projectDir, normalizeSourceRef(frozenGuardRef));
  if (!existsSync(proposalPath)) {
    advisories.push(`origin.ref ${frozenGuardRef} absent on disk; FROZEN status unverifiable (advisory-only, skipped)`);
  } else if (!isFrozen(readText(proposalPath))) {
    advisories.push(`origin.ref ${frozenGuardRef} points to a PROPOSAL that is not FROZEN (isFrozen body-text check); decomposition upstream should be frozen before slicing (warn-only, not blocking)`);
  }
}


// ── loop-metrics.json advisory（观测设施，缺失不红，DP-F9）──
// Post-EVENTS_SCHEMA_CUTOFF: loop-metrics.json is retired (on-demand only, not committed)
if (!isPostEventsCutoff) {
  const loopMetricsPath = resolve(changeDir, 'loop-metrics.json');
  if (!existsSync(loopMetricsPath)) {
    advisories.push('loop-metrics.json is missing — observability artifact not produced (advisory-only, does not block merge)');
  }
}

// ── pod-journal.jsonl advisory（叙事車道层 A，缺失不红，M-A15）──
// Post-EVENTS_SCHEMA_CUTOFF: pod-journal.jsonl is retired (subsumed by pod-events.jsonl)
if (!isPostEventsCutoff) {
  const podJournalPath = resolve(changeDir, 'pod-journal.jsonl');
  if (!existsSync(podJournalPath)) {
    advisories.push('pod-journal.jsonl is missing — narrative lane artifact not produced (advisory-only, does not block merge)');
  }
}

// ── INDEX 增量刷行（best-effort，投影面，不红）──
let indexProjection = updateIndexRequested ? 'requested' : 'skipped-read-only';
if (updateIndexRequested) {
  try {
    indexProjection = upsertIndexRow(changeId, taskPackage);
  } catch (e) {
    indexProjection = 'skipped-error';
    advisories.push(`INDEX upsert skipped (best-effort): ${e.message || e}`);
  }
}

print({ ok: true, gate: 'closeout', changeId, changeDir: `changes/dev/${basename(changeDir)}`, requiredFiles, advisories, indexProjection });
if (missing.length > 0) process.exit(1);