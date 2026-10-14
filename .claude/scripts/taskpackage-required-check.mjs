#!/usr/bin/env node
// Ensures dev-loop verification has a persisted TaskPackage context.
// This prevents chat-only TaskPackages from passing verifier gates.
//
// artifact-lifecycle 修订（gov-review-devloop-artifact-lifecycle，FROZEN 2026-07-22）：
// - origin 一等强键（upstreamType/upstreamRef/upstreamFrozen/workstream）
// - sourceType deprecated，origin.type ∨ sourceType 至少一个存在
// - decisionCoverage 触发改读 isProposal（`sourceType` ∨ lib 单源 `resolveOriginType(origin)`，即 origin.upstreamType ∨ origin.type）
// - proposal→docRefs back-check 由 origin.ref 接管（新包 / 过渡 carve-out 老包）
// - changeId YYMMDD- 前缀强制 + 零追溯白名单（DP-2 git 首提日期 cutoff）
// - sourceRefs 必填非空语义收窄为代码范围锚点（convention-only, ARQ-260723-artifact-lifecycle-teeth）
// crossEnd + input-brief 承重（260908-gov-review-input-surface-frontload T3，FROZEN v1.0）：
// - crossEnd 申报字段：Draft 空骨架必在；finalize 态 counterparts 非空 ∨ exempt ≥12 字符 二选一，双缺红点名
// - finalize 态两份 input-brief-<role>.md 必在场（生成时点 = planner Step 7 前置一次产双份）
// - 零追溯 = 已 merge 历史包（本闸只读 current taskpackage）；在飞包一次性过渡补救见 PROPOSAL T3「过渡注记」

import { existsSync, readFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { dirname, join, resolve } from 'node:path';
import { execSync } from 'node:child_process';
import { isProposalMdPath, normalizeSourceRef, resolveOriginRef, resolveOriginType, ANCHORABLE_TEST } from './lib/governance-gate-shared.mjs';
import { CHANGE_ID_PREFIX_RE } from './lib/changeid-shared.mjs'; // T1 单源收敛（260906-gov-review-arq-governance-batch）
import { resolveCurrentTaskPackage, resolveExplicitTaskPackage } from './lib/current-pointer-shared.mjs'; // T6 回显齿（260924-gov-review-gate-parse-teeth）

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function taskPackagePath() {
  const explicit = process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH;
  if (explicit) return resolveExplicitTaskPackage('taskpackage-required', projectDir, explicit, '位置参数/环境变量').path;
  return resolveCurrentTaskPackage('taskpackage-required', projectDir).path;
}

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'taskpackage-required', message, ...details });
  process.exit(1);
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
}

function nonEmptyString(value) {
  return typeof value === 'string' && value.trim().length > 0;
}

function nonEmptyArray(value) {
  return Array.isArray(value) && value.length > 0;
}

function isOneOf(value, allowed) {
  return allowed.includes(value);
}

// 260731-gov-review-acceptance-coverage-structural-exemption（FROZEN v1.0）M-6：
// acceptance[]/exitCriteria[] 对象条目 id 亦提取（:223+:224 原 filter(nonEmptyString) 静默丢弃对象条目
// → decisionCoverage.landsIn acceptance:<id> 误报 not-found，起包侧骨折点闭合）。
// 字符串路径保持原 .split(':')[0].trim() 提取语义不变（零行为漂移）；对象条目取 id/acceptanceId/name 后同法。
function extractEntryId(entry) {
  if (nonEmptyString(entry)) return entry.split(':')[0].trim();
  if (entry && typeof entry === 'object') {
    const raw = entry.id || entry.acceptanceId || entry.name;
    return nonEmptyString(raw) ? String(raw).split(':')[0].trim() : null;
  }
  return null;
}

function parseLandsIn(landsIn) {
  if (!nonEmptyString(landsIn)) return null;
  if (landsIn === 'dropped') return { form: 'dropped' };
  const colonIndex = landsIn.indexOf(':');
  if (colonIndex === -1 || colonIndex === 0) return null;
  const form = landsIn.slice(0, colonIndex);
  const id = landsIn.slice(colonIndex + 1);
  if (!nonEmptyString(id) || !['acceptance', 'exitCriteria', 'package'].includes(form)) return null;
  if (form === 'package' && !/^[a-z0-9-]+$/.test(id)) return null;
  return { form, id };
}

const SOURCE_TYPES = [
  'delivery-story',
  'planning-workstream',
  'arch-review',
  'human-ad-hoc',
  'ci-failure',
  'drift-report',
  'regression',
  'maintenance',
  'proposal',
];
const ORIGIN_TYPES = [...SOURCE_TYPES, 'human-confirmed-plan']; // origin.type 值域 = sourceType 枚举 + 受控扩展
const DELIVERY_IMPACTS = ['none', 'milestone-affecting'];
const PLANNING_IMPACTS = ['none', 'attach-existing', 'create-workstream', 'needs-triage'];
const RISK_TIERS = ['low', 'medium', 'high', 'critical'];

// ── changeId 日期前缀 + 零追溯白名单谓词（DP-2 / F8，单一语义，两 G3 脚本 convention 级同步）──
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
    return lines.length ? lines[lines.length - 1] : null; // earliest = last with --follow
  } catch {
    return null;
  }
}

// ── AC 回引齿（260928-gov-review-portfolio-boundary-story-propagation T4，finalize 态向严）──
// sourceRefs 命中 requirements story（`S-Fx.y` / `S-Fx.y-AC<n>`）的包：回引 AC 号集须 ==
// sourceRefs 声明的 AC 号集（对齐 planner Step 8b「多引、少引都是方向偏差信号」）；sourceRefs
// 未带 AC 号集 = 只核「至少一条 AC 回引在场」（回引来源 = sourceRefs AC 复合句法 ∨ acceptance
// 齿内 `S-Fx.y-AC<n>` token，二者居一即合 FROZEN 语义「acceptance 齿 / sourceRefs 必须带」）。
// Draft 态放行（finalize 代理 = `acceptanceTestsFile` 非空）。存量 carve-out（FROZEN T4 钉死，
// 对偶 DP-2 / CHANGE_ID_CUTOFF 先例）：包目录 gitFirstCommitDate < AC_BACKREF_CUTOFF = 存量放行。
const STORY_REF_RE = /^S-F\d+\.\d+(-AC\d+)?$/;
const AC_TAG_RE = /^(S-F\d+\.\d+)-AC(\d+)$/;
const AC_BACKREF_CUTOFF = '2026-09-28';

// changeManifest 完备性齿（260928-gov-review-dev-loop-preassess-manifest-scope T7）：
// 在场 + 非空 + 元素合法仓根相对 glob 形态（禁绝对路径/穿越/反斜杠）；缺失/空 = 红（fail-closed）。
// 存量 carve-out 对偶 AC_BACKREF_CUTOFF 先例：包目录 gitFirstCommitDate < MANIFEST_CUTOFF = 放行；
// cutoff 谓词与 change-manifest-check.mjs 的 MANIFEST_CUTOFF 同值共享（T6/T7 双闸同豁免）。
const MANIFEST_CUTOFF = '2026-09-28';
const MANIFEST_ENTRY_RE = /^[A-Za-z0-9_./?*\-]+$/;

function manifestEntryInvalid(entry) {
  if (typeof entry !== 'string' || entry.trim().length === 0) return true;
  const s = entry.trim().replace(/^\.\//, '');
  return s.startsWith('/') || s.includes('..') || s.includes('\\') || !MANIFEST_ENTRY_RE.test(s);
}

function collectManifestErrors(taskPackage, gitFn) {
  const errors = [];
  const cid = taskPackage.changeId;
  if (nonEmptyString(cid)) {
    let firstCommit = gitFn(`delivery/taskpackages/${cid}`);
    if (firstCommit === null) firstCommit = gitFn(`delivery/taskpackages/${cid}`);
    if (firstCommit !== null && firstCommit < MANIFEST_CUTOFF) return errors; // 存量放行（双闸同谓词）
  }
  if (!Array.isArray(taskPackage.changeManifest) || taskPackage.changeManifest.length === 0) {
    errors.push('changeManifest(must be a non-empty array of repo-relative file/dir globs; 产出 = architect 前置评估 planner Step 1g 档；扩单唯一通道 = architect 评估——260928-gov-review-dev-loop-preassess-manifest-scope T4/T7)');
    return errors;
  }
  const bad = taskPackage.changeManifest.filter((e) => manifestEntryInvalid(e));
  if (bad.length) {
    errors.push(`changeManifest has illegal glob entries (repo-relative only; no absolute/traversal/backslash): ${JSON.stringify(bad)}`);
  }
  return errors;
}

export function collectAcBackrefErrors(tp, opts = {}) {
  const errors = [];
  const isFinalizedState = nonEmptyArray(tp.acceptanceTestsFile);
  const sourceRefs = (Array.isArray(tp.sourceRefs) ? tp.sourceRefs : []).filter(nonEmptyString).map((r) => String(r).trim());
  const storyRefs = sourceRefs.filter((r) => STORY_REF_RE.test(r));
  if (!isFinalizedState || storyRefs.length === 0) return errors; // Draft 态放行 / 非 story 命中包不适用
  const cutoff = opts.cutoffDate || AC_BACKREF_CUTOFF;
  const gitFn = opts.gitFn || ((rel) => gitFirstCommitDate(rel));
  const cid = nonEmptyString(tp.changeId) ? String(tp.changeId) : '';
  let firstCommit = gitFn(`delivery/taskpackages/${cid}`);
  if (firstCommit === null) firstCommit = gitFn(`delivery/taskpackages/${cid}`);
  if (firstCommit !== null && firstCommit < cutoff) return errors; // 存量放行（landing-summary 记抽查处置）
  const declared = new Map(); // base -> Set<ac>（sourceRefs AC 复合句法声明的号集）
  const backref = new Set(); // `${base}#${ac}`：sourceRefs tags ∪ acceptance 齿内回引
  for (const r of storyRefs) {
    const m = AC_TAG_RE.exec(r);
    if (m) {
      if (!declared.has(m[1])) declared.set(m[1], new Set());
      declared.get(m[1]).add(m[2]);
      backref.add(`${m[1]}#${m[2]}`);
    }
  }
  const accBlob = (Array.isArray(tp.acceptance) ? tp.acceptance : [])
    .map((e) => (e && typeof e === 'object' ? JSON.stringify(e) : String(e))).join('\n');
  for (const m of accBlob.matchAll(/(S-F\d+\.\d+)-AC(\d+)/g)) backref.add(`${m[1]}#${m[2]}`);
  for (const base of new Set(storyRefs.map((r) => r.replace(/-AC\d+$/, '')))) {
    const declaredSet = declared.get(base);
    const baseBackrefs = new Set([...backref].filter((s) => s.startsWith(`${base}#`)).map((s) => s.split('#')[1]));
    if (!declaredSet || declaredSet.size === 0) {
      if (baseBackrefs.size === 0) {
        errors.push(`AC backref missing: story ${base}（sourceRefs 裸 story id 且 sourceRefs/acceptance 无任何 S-Fx.y-AC<n> 回引——finalize 态闸红，planner Step 8b 同口径）`);
      }
      continue;
    }
    const missingAcs = [...declaredSet].filter((ac) => !baseBackrefs.has(ac));
    const extraAcs = [...baseBackrefs].filter((ac) => !declaredSet.has(ac));
    if (missingAcs.length > 0) {
      errors.push(`AC backref set != declared set for ${base}: missing AC${missingAcs.join(', AC')}（少引 = 方向偏差信号）`);
    }
    if (extraAcs.length > 0) {
      errors.push(`AC backref set != declared set for ${base}: extra AC${extraAcs.join(', AC')}（多引 = 方向偏差信号）`);
    }
  }
  return errors;
}

// ── T5（260924-gov-review-gate-parse-teeth）：引用字段磁盘存在性 warn 齿（advisory，不拦门）──
// 七类引用字段（docRefs / sourceRefs / contractRefs / planningRefs / acceptanceTestsFile /
// origin.ref(upstreamRef) / crossEnd.counterparts[].path）取 fragment-stripped 路径、过 ANCHORABLE_TEST
// 形状过滤（非路径 token 如 story id「S-F1.1」跳过——起草期 census 129/694 为非路径），existsFn 不在场者
// 入 warnings；exit code 与 ok 判定零变更。零追溯姿势继承（头注 :15）：只查本闸实际读取的当前包
// （current 默认或显式传入），不扫历史（census 87/694 历史包陈旧引用实证——追溯即纯噪声）。
export function collectRefWarnings(tp, existsFn) {
  const warnings = [];
  const checkPath = (field, ref) => {
    if (typeof ref !== 'string' || !ref.trim()) return;
    const path = ref.split('#')[0].trim();
    if (!path || path.startsWith('http') || path.startsWith('<')) return;
    if (!ANCHORABLE_TEST.test(path)) return;
    if (!existsFn(path)) warnings.push({ field, ref, reason: 'path-not-on-disk', path });
  };
  for (const field of ['docRefs', 'sourceRefs', 'contractRefs', 'planningRefs', 'acceptanceTestsFile']) {
    for (const ref of Array.isArray(tp[field]) ? tp[field] : []) checkPath(field, ref);
  }
  const origin = (tp.origin && typeof tp.origin === 'object') ? tp.origin : {};
  for (const key of ['ref', 'upstreamRef']) checkPath(`origin.${key}`, origin[key]);
  const crossEnd = (tp.crossEnd && typeof tp.crossEnd === 'object') ? tp.crossEnd : {};
  const counterparts = Array.isArray(crossEnd.counterparts) ? crossEnd.counterparts : [];
  for (const entry of counterparts) {
    if (entry && typeof entry === 'object') checkPath('crossEnd.counterparts[].path', entry.path);
  }
  return warnings;
}

function selfCheck() {
  let pass = 0;
  let failCount = 0;
  const ok = (cond, name) => { if (cond) pass += 1; else { failCount += 1; console.error(`FAIL [${name}]`); } };
  const never = () => false;
  const always = () => true;
  // 判别：路径形假 ref（盘上无）→ warnings 非空；story-id 非路径 → 不产 warn（形状过滤）
  const w1 = collectRefWarnings({ docRefs: ['docs/nonexistent-xyz.md'], sourceRefs: ['S-F1.1'], acceptanceTestsFile: ['firmware/no-such.test.c'] }, never);
  ok(w1.length === 2 && w1.some((w) => w.field === 'docRefs') && w1.some((w) => w.field === 'acceptanceTestsFile'), 'sc-fake-refs-warn');
  ok(w1.every((w) => w.field !== 'sourceRefs'), 'sc-nonpath-token-skipped');
  // 判别：在场 ref → 空（反转实现必须变红）
  ok(collectRefWarnings({ docRefs: ['docs/some-file.md'] }, always).length === 0, 'sc-existing-ref-clean');
  // fragment 剥离：'#§x' 锚不进路径判定
  const w4 = collectRefWarnings({ docRefs: ['docs/gone.md#§1.2'] }, never);
  ok(w4.length === 1 && w4[0].path === 'docs/gone.md', 'sc-fragment-stripped');
  // crossEnd.counterparts[].path 同齿
  const w5 = collectRefWarnings({ crossEnd: { counterparts: [{ path: 'server/gone.py', symbol: 'x' }] } }, never);
  ok(w5.length === 1 && w5[0].field === 'crossEnd.counterparts[].path', 'sc-crossend-path-warn');
  // ── AC 回引齿（260928 批 T4）：Draft 放行 / finalize 缺回引红 / 号集相等绿 / 多引红 / 存量放行 ──
  const acBase = { changeId: '260930-ac-probe', sourceRefs: ['S-F1.4'], acceptance: [], acceptanceTestsFile: [], exitCriteria: [] };
  ok(collectAcBackrefErrors(acBase, { gitFn: () => null }).length === 0, 'sc-ac-draft-passthrough');
  ok(collectAcBackrefErrors({ ...acBase, acceptanceTestsFile: ['x.acceptance.c'], acceptance: ['KR-A1 提醒弹窗驻留'] }, { gitFn: () => null }).length === 1, 'sc-ac-finalize-missing-backref-red');
  ok(collectAcBackrefErrors({ ...acBase, acceptanceTestsFile: ['x.acceptance.c'], acceptance: ['KR-A1 S-F1.4-AC2 断言'] }, { gitFn: () => null }).length === 0, 'sc-ac-bare-one-backref-ok');
  const acTagged = { ...acBase, sourceRefs: ['S-F1.4-AC1', 'S-F1.4-AC3'], acceptanceTestsFile: ['x.acceptance.c'], acceptance: ['KR-A1 S-F1.4-AC1', 'KR-A2 S-F1.4-AC3'] };
  ok(collectAcBackrefErrors(acTagged, { gitFn: () => null }).length === 0, 'sc-ac-tagged-equal-ok');
  ok(collectAcBackrefErrors({ ...acTagged, acceptance: ['KR-A1 S-F1.4-AC1', 'KR-A2 S-F1.4-AC3', 'KR-A3 S-F1.4-AC2'] }, { gitFn: () => null }).length === 1, 'sc-ac-extra-red');
  ok(collectAcBackrefErrors({ ...acBase, acceptanceTestsFile: ['x.acceptance.c'], acceptance: ['KR-A1'] }, { gitFn: () => '2026-09-01' }).length === 0, 'sc-ac-legacy-carveout');
  ok(collectAcBackrefErrors({ ...acBase, acceptanceTestsFile: ['x.acceptance.c'], acceptance: ['KR-A1'] }, { gitFn: () => '2026-10-01' }).length === 1, 'sc-ac-new-package-red');
  // ── changeManifest 完备性齿（260928 批 T7）：缺失红 / 非法 glob 红 / 合法 glob 绿 / 存量放行 ──
  ok(collectManifestErrors({ changeId: '260930-mf-probe' }, () => null).length === 1, 'sc-manifest-missing-red');
  ok(collectManifestErrors({ changeId: '260930-mf-probe', changeManifest: [] }, () => null).length === 1, 'sc-manifest-empty-red');
  ok(collectManifestErrors({ changeId: '260930-mf-probe', changeManifest: ['/abs/path'] }, () => null).length === 1, 'sc-manifest-absolute-red');
  ok(collectManifestErrors({ changeId: '260930-mf-probe', changeManifest: ['firmware/main/../x'] }, () => null).length === 1, 'sc-manifest-traversal-red');
  ok(collectManifestErrors({ changeId: '260930-mf-probe', changeManifest: ['firmware/main/**', 'server/app/*.py'] }, () => null).length === 0, 'sc-manifest-globs-ok');
  ok(collectManifestErrors({ changeId: '260930-mf-probe', changeManifest: [] }, () => '2026-09-01').length === 0, 'sc-manifest-legacy-carveout');
  console.log(JSON.stringify({ selfCheck: 'taskpackage-required-check', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
  process.exit(failCount === 0 ? 0 : 1);
}

if (process.argv.includes('--self-check')) selfCheck();

const pathToTaskPackage = taskPackagePath();

if (!existsSync(pathToTaskPackage)) {
  fail('Dev-loop verification requires a persisted TaskPackage file', {
    expected: pathToTaskPackage,
    hint: 'planner must write delivery/taskpackages/<change-id>/taskpackage.json and refresh delivery/taskpackages/current/taskpackage.json before verifier runs',
  });
}

let taskPackage;
try {
  taskPackage = readJson(pathToTaskPackage);
} catch (error) {
  fail('TaskPackage is not valid JSON', { taskPackage: pathToTaskPackage, error: String(error.message || error) });
}

// ── canonical↔current 散列一致性校验（260824-gov-review-pod-worktree-launch-infra T4；ARQ-260823-taskpackage-canonical-current-drift）──
// 触发姿势边界（PROPOSAL F3 成文）：仅默认裸跑（闸实际读 current/ 指针副本）时生效；
// 显式传参（process.argv[2] / DEV_LOOP_TASKPACKAGE / TASKPACKAGE_PATH）与 closeout-check.mjs
// 程序化传 canonical 路径时不进入本段（显式姿势下 changeId 解析回同一文件、比对恒等，亦不触发新红）。
// current/ 或 canonical 任一缺失时按既有逻辑走：不新增 missing 判定（changeId 缺失/非法由下方既有字段校验承红）。
const explicitInvocation = !!(process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH);
if (!explicitInvocation) {
  const currentChangeId = nonEmptyString(taskPackage.changeId) ? String(taskPackage.changeId) : null;
  if (currentChangeId) {
    const canonicalPath = resolve(projectDir, 'delivery/taskpackages', currentChangeId, 'taskpackage.json');
    if (canonicalPath !== pathToTaskPackage && existsSync(canonicalPath)) {
      const sha256 = content => createHash('sha256').update(content).digest('hex');
      if (sha256(readFileSync(pathToTaskPackage, 'utf8')) !== sha256(readFileSync(canonicalPath, 'utf8'))) {
        fail('canonical 与 current/ 指针副本不一致，请刷新 current/（planner 改 canonical 后必须重新写回 current/）', {
          canonical: canonicalPath,
          current: pathToTaskPackage,
        });
      }
    }
  }
}

const missing = [];

// ── origin 一等强键（上游溯源结构；DP / F-series）──
const origin = (taskPackage.origin && typeof taskPackage.origin === 'object') ? taskPackage.origin : null;
const originType = resolveOriginType(origin);
const hasSourceType = nonEmptyString(taskPackage.sourceType);

// origin.type ∨ sourceType 至少一个存在（必填清单改：sourceType 不再无条件必填）
if (!originType && !hasSourceType) missing.push('origin.type∨sourceType(at least one)');

for (const field of ['changeId', 'subsystem', 'story', 'redBaselineCommand', 'redBaselineResult', 'deliveryImpact', 'planningImpact', 'riskTier']) {
  if (!nonEmptyString(taskPackage[field])) missing.push(field);
}

for (const field of ['acceptance', 'acceptanceTestsFile', 'exitCriteria', 'contractRefs', 'docRefs', 'sourceRefs', 'gateProfile']) {
  if (!nonEmptyArray(taskPackage[field])) missing.push(field);
}

// ── acceptance[] 对象条目形状校验（260731 批 M-1/M-6：kind/suiteGate 一等豁免标记，非可选）──
// 互斥 fail-loud：同条目同时标 kind:'structural' + suiteGate:true = 类别矛盾 → 红。
// 未知值 fail-safe：kind 仅认 'structural'、suiteGate 仅认 true；其余值不豁免（coverage/mutation 侧同义 fail-safe 红）并在此成文报错。
// 对象条目必须携带可提取 id（id/acceptanceId/name），否则 id 进不了 acceptanceIds。
if (Array.isArray(taskPackage.acceptance)) {
  for (const entry of taskPackage.acceptance) {
    if (!entry || typeof entry !== 'object') continue;
    const entryId = extractEntryId(entry) || '(无 id)';
    if (!extractEntryId(entry)) missing.push(`acceptance[${entryId}] object entry must carry id/acceptanceId/name`);
    if ('kind' in entry && entry.kind !== 'structural') {
      missing.push(`acceptance[${entryId}].kind unknown value ${JSON.stringify(entry.kind)} (only 'structural' supported; unknown values fail-safe to no-exemption)`);
    }
    if ('suiteGate' in entry && entry.suiteGate !== true) {
      missing.push(`acceptance[${entryId}].suiteGate must be true when present (got ${JSON.stringify(entry.suiteGate)}; fail-safe)`);
    }
    if (entry.kind === 'structural' && entry.suiteGate === true) {
      missing.push(`acceptance[${entryId}] must not carry both kind:'structural' and suiteGate:true (category contradiction; pick one)`);
    }
  }
}

if (hasSourceType && !isOneOf(taskPackage.sourceType, SOURCE_TYPES)) missing.push('sourceType(valid)');
if (originType && !isOneOf(originType, ORIGIN_TYPES)) missing.push(`origin.type(valid: ${ORIGIN_TYPES.join('|')})`);
if (nonEmptyString(taskPackage.deliveryImpact) && !isOneOf(taskPackage.deliveryImpact, DELIVERY_IMPACTS)) missing.push('deliveryImpact(valid)');
if (nonEmptyString(taskPackage.planningImpact) && !isOneOf(taskPackage.planningImpact, PLANNING_IMPACTS)) missing.push('planningImpact(valid)');
if (nonEmptyString(taskPackage.riskTier) && !isOneOf(taskPackage.riskTier, RISK_TIERS)) missing.push('riskTier(valid)');

// 并存一致性（tie-break = origin.type == sourceType，human-confirmed-plan 例外须 sourceType 缺省）
if (originType && hasSourceType) {
  if (originType === 'human-confirmed-plan') {
    missing.push('origin.type=human-confirmed-plan requires sourceType absent (this value has no sourceType mirror; origin.type ≠ human-confirmed-plan enforcement)');
  } else if (originType !== taskPackage.sourceType) {
    missing.push(`origin.type (${originType}) must equal sourceType (${taskPackage.sourceType}) when both present`);
  }
}

// ── changeId 日期前缀强制（新包）+ 零追溯白名单（DP-2）──
if (nonEmptyString(taskPackage.changeId)) {
  const cid = String(taskPackage.changeId);
  const prefixMatch = CHANGE_ID_PREFIX_RE.exec(cid);
  if (prefixMatch) {
    if (!validDatePrefix(prefixMatch[1])) {
      missing.push(`changeId YYMMDD prefix date literal invalid (month 01-12, day 01-31): ${prefixMatch[1]}`);
    }
  } else {
    // 无前缀 → 判白名单：git 首提 < cutoff = 老包放行
    let isGrandfathered = false;
    let firstCommit = gitFirstCommitDate(`delivery/taskpackages/${cid}`);
    if (firstCommit === null) firstCommit = gitFirstCommitDate(`delivery/taskpackages/${cid}`);
    if (firstCommit === null) {
      // git 不可用 / 新 dir 无历史 → degrade: 放行（convention 兜底，不误红）
      isGrandfathered = true;
    } else if (firstCommit < CHANGE_ID_CUTOFF) {
      isGrandfathered = true;
    }
    if (!isGrandfathered) {
      missing.push(`changeId must have YYMMDD- date prefix for new packages (first commit ${firstCommit || 'unknown'} >= cutoff ${CHANGE_ID_CUTOFF})`);
    }
  }
}

const planningRefs = Array.isArray(taskPackage.planningRefs) ? taskPackage.planningRefs : [];
if (['attach-existing', 'create-workstream'].includes(taskPackage.planningImpact) && planningRefs.length === 0) {
  missing.push('planningRefs(required when planningImpact attaches or creates workstream)');
}
if (taskPackage.planningImpact === 'none' && planningRefs.length > 0) {
  missing.push('planningRefs(empty when planningImpact=none)');
}
if (taskPackage.planningImpact === 'needs-triage') {
  missing.push('planningImpact must be resolved before verifier runs');
}

if (taskPackage.sourceType === 'planning-workstream' && planningRefs.length === 0) {
  missing.push('planningRefs(required when sourceType=planning-workstream)');
}

if (typeof taskPackage.iterationBudget !== 'number') missing.push('iterationBudget');
if (typeof taskPackage.fixBudget !== 'number') missing.push('fixBudget');

if (taskPackage.docsSyncRequired !== false && taskPackage.docsOwner !== 'integrator') {
  missing.push('docsOwner=integrator');
}

// ── decisionCoverage + proposal→docRefs back-check（DP-1 / F2 承重闭合）──
// 260731-gov-review-artifact-lifecycle-origin-teeth Phase 2（T1, FROZEN 2026-08-01）：isProposal 分裂为两个独立谓词：
//   isProposalByType = 字符串判（sourceType/originType === 'proposal'）—— DC 承重维持此触发（OQ4-B）
//   isProposalByPath = 路径判（upstreamRef∨ref fragment-stripped 走 isProposalMdPath）—— back-check 路径枢轴
// 三类包组合态：
//   真 proposal（260725/26/27）：byType=true, byPath=true → DC + back-check 皆触发
//   hybrid（260729 两包）：byType=false, byPath=true → DC 不触发，back-check 触发
//   纯 regression：皆 false → 皆不触发
//   sourceType=proposal 老包（ref 缺）：byType=true, byPath=false → 析取触发 back-check，sourceRefs carve-out 可达（CF-04）
const isProposalByType = !!(taskPackage.sourceType === 'proposal' || originType === 'proposal');
const isProposalByPath = !!(origin && isProposalMdPath(normalizeSourceRef(resolveOriginRef(origin) || '')));

const coverageErrors = [];
// DC 承重外闸：isProposalByType（字符串判）∨ decisionCoverage 包内数组
if (isProposalByType || Array.isArray(taskPackage.decisionCoverage)) {
  // DC hard-fail 维持字符串判（OQ4-B）
  if (isProposalByType && !nonEmptyArray(taskPackage.decisionCoverage)) {
    coverageErrors.push('decisionCoverage must be a non-empty array for proposal-来源 packages');
  }
  if (Array.isArray(taskPackage.decisionCoverage)) {
    const acceptanceIds = new Set(taskPackage.acceptance.map(extractEntryId).filter(Boolean));
    const exitCriteriaIds = new Set(taskPackage.exitCriteria.map(extractEntryId).filter(Boolean));
    const normalizedDocRefs = new Set(taskPackage.docRefs.filter(nonEmptyString).map(normalizeSourceRef));

    for (const [index, entry] of taskPackage.decisionCoverage.entries()) {
      if (!entry || typeof entry !== 'object') {
        coverageErrors.push(`decisionCoverage[${index}] must be an object`);
        continue;
      }
      if (!nonEmptyString(entry.decision)) coverageErrors.push(`decisionCoverage[${index}].decision required`);
      if (!nonEmptyString(entry.summary)) coverageErrors.push(`decisionCoverage[${index}].summary required`);
      if (!nonEmptyArray(entry.sourceRefs)) coverageErrors.push(`decisionCoverage[${index}].sourceRefs must be a non-empty array`);
      if (!nonEmptyString(entry.landsIn)) coverageErrors.push(`decisionCoverage[${index}].landsIn required`);

      const parsed = parseLandsIn(entry.landsIn);
      if (!parsed) {
        coverageErrors.push(`decisionCoverage[${index}].landsIn has invalid format: ${entry.landsIn}`);
      } else if (parsed.form === 'dropped') {
        if (!nonEmptyString(entry.reason) || entry.reason.trim().length < 12) {
          coverageErrors.push(`decisionCoverage[${index}].landsIn=dropped requires reason >= 12 chars`);
        }
      } else if (parsed.form === 'acceptance') {
        if (!acceptanceIds.has(parsed.id)) {
          coverageErrors.push(`decisionCoverage[${index}].landsIn acceptance id ${parsed.id} not found in acceptance[]`);
        }
      } else if (parsed.form === 'exitCriteria') {
        if (!exitCriteriaIds.has(parsed.id)) {
          coverageErrors.push(`decisionCoverage[${index}].landsIn exitCriteria id ${parsed.id} not found in exitCriteria[]`);
        }
      }

      if (Array.isArray(entry.sourceRefs)) {
        for (const ref of entry.sourceRefs) {
          if (isProposalMdPath(ref) && !normalizedDocRefs.has(normalizeSourceRef(ref))) {
            coverageErrors.push(`decisionCoverage[${index}].sourceRefs proposal path ${ref} must also appear in docRefs[]`);
          }
        }
      }
    }
  }
}

// proposal→docRefs back-check：提出 Array.isArray(decisionCoverage) 嵌套（T1 de-nest, CF-01 BLOCKING）
// 由析取（isProposalByType ∨ isProposalByPath，CF-04 保老包）独立驱动
if (isProposalByType || isProposalByPath) {
  const normalizedDocRefs = new Set(
    (Array.isArray(taskPackage.docRefs) ? taskPackage.docRefs : []).filter(nonEmptyString).map(normalizeSourceRef)
  );
  // :228 析取（CF-13）：originType==='proposal' 保住 enforcement；isProposalByPath 令 hybrid 纳入
  if (originType === 'proposal' || isProposalByPath) {
    const originRef = resolveOriginRef(origin)
      ? normalizeSourceRef(resolveOriginRef(origin)) : null;
    if (!originRef) {
      coverageErrors.push('origin.ref is required for origin.type=proposal');
    } else if (!isProposalMdPath(originRef)) {
      coverageErrors.push(`origin.ref ${originRef} is not a valid proposal .md path (expect delivery/changes/(arch|dev)/<id>/PROPOSAL.md or delivery/workstreams/<ws>/PROPOSAL.md)`);
    } else if (!normalizedDocRefs.has(originRef)) {
      coverageErrors.push(`proposal origin.ref ${originRef} must also appear in docRefs[]`);
    }
  } else {
    // 过渡 carve-out：sourceType=proposal 老包维持 sourceRefs-based back-check
    const sourceRefs = Array.isArray(taskPackage.sourceRefs) ? taskPackage.sourceRefs : [];
    const proposalMdPaths = sourceRefs.filter(isProposalMdPath);
    if (proposalMdPaths.length === 0) {
      coverageErrors.push('sourceRefs must include at least one proposal .md path for sourceType=proposal');
    } else {
      for (const path of proposalMdPaths) {
        if (!normalizedDocRefs.has(normalizeSourceRef(path))) {
          coverageErrors.push(`proposal sourceRef ${path} must also appear in docRefs[]`);
        }
      }
    }
  }
}

// ── crossEnd 申报 + input-brief 在场（260908-gov-review-input-surface-frontload T3；扩判定向严）──
// 三条：① 字段在场性（Draft 空骨架放行，同 mutationEvidence 条 = DEV-GOVERNANCE 起包格式期望前置第 1/6 条）；
// ② finalize 态二选一（counterparts 非空，每条含 path+symbol ∨ exempt 非空理由 ≥12 字符）；
// ③ finalize 态两份 input-brief 在场。
// finalize 代理 = `acceptanceTestsFile` 非空，系既有约定（本闸上方对七数组无条件必填 + planner Step 8 同义）：
// Draft 态本闸恒红、绿态仅 finalize 可达，故 ②③ 的分支条件恒有定义（PROPOSAL R1/E 节确认自洽）。
const CROSS_END_EXEMPT_MIN_CHARS = 12; // 对齐本闸 landsIn=dropped 的 reason 地板（防空破折号混关）
const crossEnd = (taskPackage.crossEnd && typeof taskPackage.crossEnd === 'object') ? taskPackage.crossEnd : null;
if (!crossEnd) {
  missing.push('crossEnd(骨架必在: {counterparts:[], exempt?:string}；空 counterparts 的骨架也算在场)');
} else if (!Array.isArray(crossEnd.counterparts)) {
  missing.push('crossEnd.counterparts(must be an array; empty skeleton allowed at Draft)');
}

const isFinalizedState = nonEmptyArray(taskPackage.acceptanceTestsFile);
if (isFinalizedState && crossEnd) {
  const counterparts = Array.isArray(crossEnd.counterparts) ? crossEnd.counterparts : [];
  for (const [i, entry] of counterparts.entries()) {
    const p = (entry && typeof entry === 'object') ? entry.path : null;
    const s = (entry && typeof entry === 'object') ? entry.symbol : null;
    if (!nonEmptyString(p) || !nonEmptyString(s)) {
      missing.push(`crossEnd.counterparts[${i}] must carry non-empty path + symbol`);
    }
  }
  const exempt = nonEmptyString(crossEnd.exempt) ? String(crossEnd.exempt).trim() : null;
  if (exempt !== null && exempt.length < CROSS_END_EXEMPT_MIN_CHARS) {
    missing.push(`crossEnd.exempt used as the not-applicable branch must be >= ${CROSS_END_EXEMPT_MIN_CHARS} chars (got ${exempt.length}); same floor as decisionCoverage landsIn=dropped reason`);
  }
  if (counterparts.length === 0 && exempt === null) {
    missing.push('crossEnd at finalize requires ONE OF: non-empty counterparts[] (each {path,symbol}) OR non-empty exempt reason — both missing；判断不了按存疑向严填 counterparts 而非 exempt（DEV-GOVERNANCE 起包格式期望前置 第 6 条）');
  }
}

if (isFinalizedState) {
  const tpDir = dirname(pathToTaskPackage);
  for (const role of ['acceptance-author', 'builder']) {
    const briefPath = join(tpDir, `input-brief-${role}.md`);
    if (!existsSync(briefPath)) {
      missing.push(`input-brief-${role}.md(not in taskpackage dir — planner Step 7 前置一次产双份: node .claude/scripts/brief-inputs.mjs --change-id ${taskPackage.changeId || '<id>'} --role ${role})`);
    }
  }
}

// ── AC 回引齿（260928-gov-review-portfolio-boundary-story-propagation T4）——主流程挂接 ──
coverageErrors.push(...collectAcBackrefErrors(taskPackage));

// ── changeManifest 完备性齿（260928-gov-review-dev-loop-preassess-manifest-scope T7）——主流程挂接 ──
coverageErrors.push(...collectManifestErrors(taskPackage, gitFirstCommitDate));

// T5：warnings 收集（advisory——ok 与 fail 两路输出均带，exit 判定不动）。
const refWarnings = collectRefWarnings(taskPackage, (p) => existsSync(resolve(projectDir, p)));

if (missing.length > 0 || coverageErrors.length > 0) {
  fail('TaskPackage is missing required finalized fields or has invalid decisionCoverage', {
    taskPackage: pathToTaskPackage,
    missing,
    coverageErrors,
    warnings: refWarnings,
  });
}

print({
  ok: true,
  gate: 'taskpackage-required',
  taskPackage: pathToTaskPackage,
  changeId: taskPackage.changeId,
  warnings: refWarnings,
});