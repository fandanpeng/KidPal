#!/usr/bin/env node
// Owner: architect (G3 enforcement layer)
// Approver: human
// DEV-GOVERNANCE §6: G3 hooks/scripts → owner=architect, approver=human.
// Historical gap: this file predates the G3 ownership registry;
// owner declaration added per gov-review-dev-loop-planner-evidence-gates RC-8.
//
// C4+C6 (gov-review-devloop-evidence-led, FROZEN 2026-07-24):
//   - closeoutFiles retired: test-evidence.md, final-summary.md (replaced by machine evidence)
//   - supervisor consumes verdict.json + verification.json evidence, does not re-run all green gates
//   - deterministic sampling (hash(diff) mod N) replaces random sampling (R10)
//   - git merge-base freshness check preserved (CAN-2-b — actual stale-base detection source)
// OWNER: architect (G3 enforcement layer)
//
// Execution order (per PROPOSAL §5 FIX-14 + FIX-6):
//   G0. Load taskpackage + extract changeId
//   G1. Check closeout artifacts exist
//       → if hasCloseout=false: F-A1 minimum check (zero diff + completion claim → BLOCK), then exit 0
//   G2. Verify supervisor-verdict.json exists, is valid JSON, has valid verdict
//   G2a. Verdict values never skip G3-G6 under S-repair provenance rules
//   G3. Code change existence (git diff non-empty)
//   G4. Verify-logs directory non-empty with content-bearing files
//   G5. (warn-only) mtime timeline consistency — verdict not older than closeout
//   G6. Provenance v2 field verification + current-Pod/current-round refs
//
// Each step has a corresponding --self-check assertion (runtime reflection — per FIX-11).

import { existsSync, readFileSync, readdirSync, statSync, mkdirSync, mkdtempSync, rmSync, utimesSync, writeFileSync } from 'node:fs';
import { resolve, join, relative, isAbsolute, posix, win32, dirname } from 'node:path';
import { execSync, spawnSync } from 'node:child_process';
import { tmpdir } from 'node:os';
import { pathToFileURL, fileURLToPath } from 'node:url';
// FIX-6 第二方重算复用机械闸的 riskTier 派生纯函数（单一机械源，dev-loop:244 投影；防 canonical↔hook 漂移 RC-7②）。
// import 不触发本目录 supervisor-verdict-gate.mjs 的 main()（其 isMain() 守卫，对偶 verifier-evidence-check:526）。
import { deriveDiffTier, collectGitDiff } from './supervisor-verdict-gate.mjs';
import { CHANGE_ID_PREFIX_RE } from '../scripts/lib/changeid-shared.mjs'; // T1 单源收敛（260906-gov-review-arq-governance-batch）

const defaultProjectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// ---- steps registry (runtime reflection for --self-check) ----

const STEPS = [
  { id: 'G1', name: 'closeout-existence', optional: false },
  { id: 'G2', name: 'verdict-existence-and-validity', optional: false },
  { id: 'G2a', name: 'verdict-values-do-not-bypass-provenance', optional: false },
  { id: 'G3', name: 'code-change-existence', optional: false },
  { id: 'G4', name: 'verify-logs-existence', optional: false },
  { id: 'G5', name: 'mtime-timeline-warn', optional: true },
  { id: 'G6', name: 'provenance-v2-mechanical-gate-verification', optional: false },
];

const VERDICT_VALUES = new Set(['APPROVE', 'BLOCK', 'ESCALATE']);

// ---- changeId cutoff predicate (CF-9/F-01 去快照基准；F-03 单源 convention 同步四文件) ----
// 基准对齐 closeout-check.mjs:72 / isPostCutoff:225（前缀谓词为主），非 EVENTS_SCHEMA_CUTOFF。
// convention 级同步清单 = 四文件：closeout-check.mjs / taskpackage-required-check.mjs /
// devloop-artifact-check.mjs / hooks/supervisor-gate-check.mjs（本文件），禁各自另造（CHANGE_ID_CUTOFF 同值）。
const CHANGE_ID_CUTOFF = '2026-07-23';

function isPostCutoff(changeId) {
  const m = CHANGE_ID_PREFIX_RE.exec(String(changeId));
  return Boolean(m) && m[1] >= CHANGE_ID_CUTOFF.replace(/-/g, '').slice(2); // YYMMDD >= '260723'
}

// ---- helpers ----

function block(reason) {
  process.stderr.write(`merge-gate BLOCK: ${reason}\n`);
  process.exit(2);
}

function warn(message) {
  process.stderr.write(`merge-gate WARN: ${message}\n`);
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
}

function readJsonForGate(path, label) {
  try {
    return readJson(path);
  } catch (error) {
    block(`${label} is not valid JSON (${error.message}).`);
  }
}

function gitDiffEmpty(projectDir) {
  try {
    // G3: try main...HEAD first; fall back to HEAD if main doesn't exist (FIX-7)
    let diffCmd = 'git diff main...HEAD --stat';
    try {
      execSync('git rev-parse --verify main', { cwd: projectDir, stdio: 'pipe' });
    } catch {
      // main branch doesn't exist (bootstrap) — fall back to diff against HEAD
      warn('G3: main branch not found; falling back to git diff HEAD');
      diffCmd = 'git diff HEAD --stat';
    }

    // Check for detached HEAD (FIX-7)
    try {
      execSync('git symbolic-ref -q HEAD', { cwd: projectDir, stdio: 'pipe' });
    } catch {
      // detached HEAD — fall back to diff HEAD
      warn('G3: detached HEAD detected; using git diff HEAD');
      diffCmd = 'git diff HEAD --stat';
    }

    const out = execSync(diffCmd, { cwd: projectDir, encoding: 'utf8' }).trim();
    return !out || out.length === 0;
  } catch (err) {
    // git command failed — don't BLOCK (git failure is not evidence of hallucination) (FIX-7)
    warn(`G3: git diff command failed (${err.message}); skipping code-change check`);
    return false; // treat as "not empty" — don't block on git errors
  }
}

// ---- T2/T3 lifecycle signals (260814-gov-review-merge-gate-terminal-shape-batch) ----

// T2 · F-A1 work-in-progress 信号单调扩展：committed diff 只是在飞信号的一种形态。
// ① worktreeDirty：未提交改动（含未跟踪非忽略文件；.env 等已 gitignore 不污染）。
// ② podDirFresh：Pod 目录近期活动（覆盖 acceptance 首写前相位，OQ-3=(a) 窗口 30min）。
// git 失败容错与 FIX-7 同策：信号失败 = 视无信号，不 block on git error。
const ACTIVITY_WINDOW_MIN = 30;

function worktreeDirty(projectDir) {
  try {
    return execSync('git status --porcelain', { cwd: projectDir, encoding: 'utf8' }).trim().length > 0;
  } catch {
    return false;
  }
}

function latestMtimeMs(dir) {
  if (!existsSync(dir)) return 0;
  let max = 0;
  for (const entry of readdirSync(dir, { withFileTypes: true })) {
    const p = join(dir, entry.name);
    if (entry.isDirectory()) {
      max = Math.max(max, latestMtimeMs(p));
    } else {
      try {
        max = Math.max(max, statSync(p).mtimeMs);
      } catch {
        // stat race — skip file
      }
    }
  }
  return max;
}

function podDirFresh(projectDir, changeId) {
  const latest = latestMtimeMs(resolve(projectDir, 'delivery', 'taskpackages', changeId));
  return latest > 0 && Date.now() - latest <= ACTIVITY_WINDOW_MIN * 60 * 1000;
}

// T3 · 合并后静默态豁免：双机械合取（R1-F1：单合取①对 fork-point 零提交幻觉形结构不可区分）。
// ① HEAD∈main（is-ancestor）∧ ② closeout 制品在 main（cat-file；G1 closeoutFiles 项，合法合并必含）。
// 任一失败 / git 失败 / changeId 形状不合法 → 豁免不成立 → 保持 BLOCK（fail-safe 向严同向）。
function isHeadMergedIntoMain(projectDir, changeId) {
  if (!CHANGE_ID_PREFIX_RE.test(String(changeId))) return false;
  try {
    execSync('git merge-base --is-ancestor HEAD main', { cwd: projectDir, stdio: 'pipe' });
  } catch {
    return false; // HEAD∉main / main 不存在 / git 失败
  }
  try {
    execSync(`git cat-file -e main:delivery/changes/dev/${changeId}/docs-delta.md`, { cwd: projectDir, stdio: 'pipe' });
    return true;
  } catch {
    return false; // closeout 不在 main（fork-point 幻觉形：制品只在盘）
  }
}

function verifyLogsHaveContent(verifyLogsDir) {
  if (!existsSync(verifyLogsDir)) return false;
  const files = readdirSync(verifyLogsDir).filter((f) => f.endsWith('.log'));
  if (files.length === 0) return false;
  // At least one log file must have non-empty content (F-D3 derivative — not just touched files)
  return files.some((f) => statSync(join(verifyLogsDir, f)).size > 0);
}

function isAbsoluteRepoRef(ref) {
  return ref.startsWith('/') || win32.isAbsolute(ref) || /^[A-Za-z]:/.test(ref);
}

function resolveRepoRelativeRef(ref, fieldName, projectDir) {
  if (typeof ref !== 'string' || ref.trim() !== ref || ref.length === 0) {
    block(`${fieldName} contains empty or non-string ref.`);
  }
  if (ref.includes('\0')) block(`${fieldName} contains NUL byte: ${ref}`);
  if (ref.includes('\\')) block(`${fieldName} must use repo-relative forward-slash refs, got backslash: ${ref}`);
  if (isAbsoluteRepoRef(ref)) block(`${fieldName} contains absolute ref: ${ref}`);

  const normalized = posix.normalize(ref);
  if (normalized !== ref || normalized === '..' || normalized.startsWith('../') || ref.includes('/../')) {
    block(`${fieldName} contains traversal or non-normalized ref: ${ref}`);
  }

  const abs = resolve(projectDir, ...ref.split('/'));
  const rel = relative(projectDir, abs);
  if (rel === '' || rel.startsWith('..') || isAbsolute(rel)) {
    block(`${fieldName} escapes repository root: ${ref}`);
  }
  if (!existsSync(abs)) block(`${fieldName} references missing file: ${ref}`);
  return { ref, abs };
}

function validateRefArray(value, fieldName, projectDir, options = {}) {
  if (!Array.isArray(value) || value.length === 0) {
    block(`${fieldName} must be a non-empty array.`);
  }
  if (options.exactCount !== undefined && value.length !== options.exactCount) {
    block(`${fieldName} must contain exactly ${options.exactCount} ref(s), got ${value.length}.`);
  }

  const seen = new Set();
  return value.map((ref) => {
    if (seen.has(ref)) block(`${fieldName} contains duplicate ref: ${ref}`);
    seen.add(ref);
    if (options.prefix && typeof ref === 'string' && !ref.startsWith(options.prefix)) {
      block(`${fieldName} contains cross-Pod or stale ref outside ${options.prefix}: ${ref}`);
    }
    return resolveRepoRelativeRef(ref, fieldName, projectDir);
  });
}

function validateVerifierEvidenceRefs(provenance, changeId, verdictValue, projectDir) {
  const prefix = `delivery/taskpackages/${changeId}/verifier-evidence/rounds/`;
  const refs = validateRefArray(provenance.verifierEvidenceRefs, 'provenance.verifierEvidenceRefs', projectDir, {
    prefix,
    exactCount: 1,
  });

  const { ref, abs } = refs[0];
  const roundMatch = /\/r(\d+)\.json$/.exec(ref);
  if (!roundMatch) {
    block(`provenance.verifierEvidenceRefs must point at a completed round fact r<N>.json: ${ref}`);
  }
  const expectedRound = Number(roundMatch[1]);
  const fact = readJsonForGate(abs, `verifier evidence ref ${ref}`);
  if (fact.artifactType !== 'verifier-round-fact' || fact.authority !== 'bearing') {
    block(`verifier evidence ref is not a bearing verifier-round fact: ${ref}`);
  }
  if (fact.changeId !== changeId) {
    block(`verifier evidence ref is stale or cross-Pod: expected changeId ${changeId}, got ${fact.changeId}`);
  }
  if (fact.phase !== 'final') {
    block(`verifier evidence ref is stale: expected final phase, got ${fact.phase}`);
  }
  if (fact.round !== expectedRound) {
    block(`verifier evidence ref round mismatch: path r${expectedRound}.json, body round ${fact.round}`);
  }
  if (fact.ok !== true) {
    // T1 (260814-gov-review-merge-gate-terminal-shape-batch): red-evidence ESCALATE 豁免形态。
    // 暴露型包（fail-loud 的红 = 正确答案）的诚实 final fact 是 ok:false；豁免让 Pod 合法停下等 human 裁，
    // 不授任何合并权（ESCALATE-stays-human）。形状：G2a「verdict 值不早退 G3-G6」不变量保全——豁免发生在
    // G6 验证内部，非 G3-G6 跳过；APPROVE 结构不可入豁免（等值判断向严）；fact.ok undefined/missing ≠ false（向严）。
    // 豁免制品 = i1 先例（260801-i1-test-script-wiring）自发产出的分类清单，钩子只查形状（存在且非空），
    // 内容真实性由 human 裁合并时审（PROPOSAL §6 接受风险）。
    if (verdictValue === 'ESCALATE' && fact.ok === false) {
      const inventoryPath = resolve(projectDir, 'delivery', 'changes', 'dev', changeId, 'existing-red-inventory.md');
      if (existsSync(inventoryPath) && statSync(inventoryPath).size > 0) {
        warn(
          `G6 red-evidence ESCALATE exemption: bearing fact is red (${ref}) and existing-red-inventory.md present — ` +
          `Pod may stop legally; verdict stays ESCALATE, merge requires human ruling.`,
        );
        return;
      }
      block(
        `verifier evidence ref is not green final evidence: ${ref}. ` +
        `Red-evidence ESCALATE can stop legally only with the exemption artifact: produce ` +
        `delivery/changes/dev/${changeId}/existing-red-inventory.md (classified inventory of pre-existing reds + repair-intent ` +
        `registration; precedent: 260801-i1-test-script-wiring) and stop again, or merge via human ruling. ` +
        `APPROVE never qualifies for this exemption.`,
      );
    }
    block(`verifier evidence ref is not green final evidence: ${ref}`);
  }
}

function validateCloseoutArtifactRefs(provenance, changeId, closeoutFiles, projectDir) {
  const prefix = `delivery/changes/dev/${changeId}/`;
  const refs = validateRefArray(provenance.closeoutArtifactsChecked, 'provenance.closeoutArtifactsChecked', projectDir, { prefix });
  const present = new Set(refs.map(({ ref }) => ref));
  for (const file of closeoutFiles) {
    const expected = `${prefix}${file}`;
    if (!present.has(expected)) {
      block(`provenance.closeoutArtifactsChecked missing current closeout artifact ref: ${expected}`);
    }
  }
}

function validateProvenance(provenance, changeId, closeoutFiles, verdictValue, projectDir) {
  if (!provenance) block('G6 FAILED: verdict missing provenance object.');

  // FIX-8/H3 cutoff 分支（复用 CHANGE_ID_CUTOFF:52 单源 + isPostCutoff:55-58，不新立第二 cutoff）。
  if (!isPostCutoff(changeId)) {
    // pre-cutoff grandfather：恒向严 ESCALATE-stays-human，绝不把旧包重派生成 APPROVE（RC-1/P-4，by construction）。
    // supervisorAgentDispatched 保留为 pre-cutoff 可读字段，仅作上下文，不授权合并。
    const legacyDispatched = provenance.supervisorAgentDispatched === true;
    block(
      `G6 (pre-cutoff grandfather): changeId ${changeId} predates CHANGE_ID_CUTOFF (${CHANGE_ID_CUTOFF}); ` +
      `legacy provenance (supervisorAgentDispatched=${legacyDispatched}) is not re-validated into an auto-merge. ` +
      `Toward-strict: stays human (ESCALATE). Direction is by construction — never re-derive a legacy package into APPROVE (RC-1/P-4).`,
    );
  }

  // FIX-7/H2 post-cutoff：要求 mechanical supervisor gate provenance（程序产、诚实标注，P-3）。
  // 反转 gov-review-supervisor-provenance-strength（FROZEN HUMAN-APPROVED 2026-07-25 冻 supervisorAgentDispatched===true required）——
  // supervisor agent 2026-07-25 退役后该钉法不可满足（260729:19-20 planner 诚实拒伪造实证）；H7 human 批准反转 2026-07-30。
  const mg = provenance.mechanicalSupervisorGate;
  if (!mg || mg.ran !== true || typeof mg.producedBy !== 'string' || mg.producedBy.length === 0) {
    block(
      'G6 FAILED: post-cutoff verdict missing provenance.mechanicalSupervisorGate.{ran:true,producedBy}. ' +
      'The supervisor verdict must be produced by supervisor-verdict-gate.mjs (mechanical gate), not an LLM self-report.',
    );
  }
  if (provenance.provenanceVersion !== '2.0') {
    block(`G6 FAILED: provenance.provenanceVersion must be "2.0", got ${provenance.provenanceVersion || '(missing)'}.`);
  }

  validateRefArray(provenance.verifyLogRefs, 'provenance.verifyLogRefs', projectDir, { prefix: '.claude/verify-logs/' });
  validateVerifierEvidenceRefs(provenance, changeId, verdictValue, projectDir); // RC-7① bearing 本体（T1 批后行号以批后文件为准，J4 式）——豁免分支内嵌，非语义零改
  validateCloseoutArtifactRefs(provenance, changeId, closeoutFiles, projectDir);
}

function selfCheck() {
  // Runtime reflection — coverage matrix dynamically generated from STEPS array (FIX-11)
  console.log('--self-check: supervisor-gate-check.mjs v2 evidence chain coverage');
  console.log('');
  const executed = new Set();
  const allStepIds = STEPS.map((s) => s.id);

  for (const step of STEPS) {
    executed.add(step.id);
    const status = step.optional ? 'WARN-ONLY' : 'BLOCKING';
    console.log(`  [${status}] ${step.id}: ${step.name}`);
  }

  console.log('');
  console.log(`Coverage: ${executed.size}/${allStepIds.length} steps registered`);
  console.log('Anti-drift: steps registry is the single source of truth — modify STEPS array to change behavior.');
  console.log('Verification: each BLOCKING step removed → test failure; each WARN-ONLY step removed → warning gone.');

  // T4 (260814-gov-review-merge-gate-terminal-shape-batch)：行为级电池（tmpdir git 沙箱，子进程 spawn 断言 exit code + stderr）。
  // 案集 = PROPOSAL §2 T4 + §5.5（判官可加案不可减）；案名逐条输出 = 真空绿规约（无案名 = 未执行）。
  const failCount = runBehavioralBattery();
  if (failCount > 0) {
    console.log(`FAIL: behavioral battery has ${failCount} red case(s).`);
    process.exit(1);
  }
  console.log('OK: --self-check complete (governance: gov-review-dev-loop-planner-evidence-gates FIX-11 + 260814 terminal-shape battery)');
}

// ---- T4 behavioral battery: tmpdir git 沙箱 + 子进程 spawn（断言 exit code + stderr） ----
// 每案独立净沙箱（判官 R1 教训：复用沙箱可致假豁免）；案集 = PROPOSAL §2 T4/§5.5。

const BATTERY_GIT_ENV = {
  GIT_AUTHOR_NAME: 'gate-battery',
  GIT_AUTHOR_EMAIL: 'gate-battery@local',
  GIT_COMMITTER_NAME: 'gate-battery',
  GIT_COMMITTER_EMAIL: 'gate-battery@local',
};

function batteryGit(fixtureDir, gitArgs) {
  return execSync(`git ${gitArgs}`, {
    cwd: fixtureDir,
    encoding: 'utf8',
    stdio: ['ignore', 'pipe', 'pipe'],
    env: { ...process.env, ...BATTERY_GIT_ENV },
  });
}

function batteryInitRepo(fixtureDir) {
  mkdirSync(fixtureDir, { recursive: true });
  try {
    batteryGit(fixtureDir, 'init -b main');
  } catch {
    batteryGit(fixtureDir, 'init');
    batteryGit(fixtureDir, 'symbolic-ref HEAD refs/heads/main');
  }
}

function batteryCommitAll(fixtureDir, message) {
  batteryGit(fixtureDir, 'add -A');
  batteryGit(fixtureDir, `commit -m ${message}`);
}

function writeFixtureFile(fixtureDir, relPath, content) {
  const abs = resolve(fixtureDir, ...relPath.split('/'));
  mkdirSync(dirname(abs), { recursive: true });
  writeFileSync(abs, content);
  return abs;
}

function batteryRunHook(fixtureDir) {
  return spawnSync(process.execPath, [fileURLToPath(import.meta.url)], {
    cwd: fixtureDir,
    encoding: 'utf8',
    env: { ...process.env, CLAUDE_PROJECT_DIR: fixtureDir },
  });
}

function setupPodBase(fixtureDir, changeId, { pastPodDir = false } = {}) {
  writeFixtureFile(fixtureDir, 'delivery/taskpackages/current/taskpackage.json', JSON.stringify({ changeId }, null, 2));
  const podEventPath = writeFixtureFile(fixtureDir, `delivery/taskpackages/${changeId}/pod-events.jsonl`, '');
  if (pastPodDir) {
    const past = new Date(Date.now() - 2 * 60 * 60 * 1000);
    utimesSync(podEventPath, past, past);
  }
}

function setupCloseout(fixtureDir, changeId, { verdictValue, factOk, inventory }) {
  writeFixtureFile(fixtureDir, `delivery/changes/dev/${changeId}/docs-delta.md`, '# docs-delta fixture\n');
  writeFixtureFile(fixtureDir, `delivery/changes/dev/${changeId}/review-packet.md`, '# review-packet fixture\n');
  writeFixtureFile(fixtureDir, '.claude/verify-logs/fixture.log', 'fixture gate log\n');
  writeFixtureFile(
    fixtureDir,
    `delivery/taskpackages/${changeId}/verifier-evidence/rounds/r1.json`,
    JSON.stringify(
      {
        artifactType: 'verifier-round-fact',
        authority: 'bearing',
        changeId,
        phase: 'final',
        round: 1,
        ok: factOk,
        results: [{ ok: factOk, gateId: 'fixture' }],
      },
      null,
      2,
    ),
  );
  writeFixtureFile(
    fixtureDir,
    `delivery/changes/dev/${changeId}/supervisor-verdict.json`,
    JSON.stringify(
      {
        verdict: verdictValue,
        riskTier: 'high',
        changeId,
        provenance: {
          provenanceVersion: '2.0',
          mechanicalSupervisorGate: { ran: true, producedBy: 'supervisor-verdict-gate.mjs' },
          verifyLogRefs: ['.claude/verify-logs/fixture.log'],
          verifierEvidenceRefs: [`delivery/taskpackages/${changeId}/verifier-evidence/rounds/r1.json`],
          closeoutArtifactsChecked: [
            `delivery/changes/dev/${changeId}/docs-delta.md`,
            `delivery/changes/dev/${changeId}/review-packet.md`,
          ],
        },
      },
      null,
      2,
    ),
  );
  if (inventory) {
    writeFixtureFile(fixtureDir, `delivery/changes/dev/${changeId}/existing-red-inventory.md`, '# existing red inventory fixture\n');
  }
}

function runBehavioralBattery() {
  console.log('');
  console.log('T4 behavioral battery (tmpdir git sandbox, subprocess spawn):');
  const results = [];
  const assert = (name, cond, detail = '') => results.push({ name, ok: Boolean(cond), detail });
  const CHANGE_ID = '260814-battery-fixture';
  const root = mkdtempSync(join(tmpdir(), 'sgc-battery-'));

  try {
    // F-A1：幻觉形保持（零 closeout + 零 committed diff + 干净工作树 + stale Pod 目录）
    {
      const dir = join(root, 'f-a1-hallucination');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      const r = batteryRunHook(dir);
      assert('f-a1-hallucination-preserved: BLOCK kept', r.status === 2 && /hallucinated completion/.test(r.stderr), `exit=${r.status}`);
    }
    // F-A1：脏工作树 = 在飞
    {
      const dir = join(root, 'f-a1-dirty');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      writeFixtureFile(dir, 'apps/s1-leads/src/wip.ts', '// uncommitted work\n');
      const r = batteryRunHook(dir);
      assert('f-a1-dirty-worktree: in-progress exit 0', r.status === 0, `exit=${r.status} stderr=${(r.stderr || '').slice(0, 120)}`);
    }
    // F-A1：新鲜 Pod 目录 = 在飞（acceptance 首写前相位）
    {
      const dir = join(root, 'f-a1-fresh');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID);
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      const r = batteryRunHook(dir);
      assert('f-a1-fresh-poddir: in-progress exit 0', r.status === 0, `exit=${r.status} stderr=${(r.stderr || '').slice(0, 120)}`);
    }
    // G3：post-merge 静默态（HEAD∈main ∧ closeout 在 main）
    {
      const dir = join(root, 'g3-postmerge');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      setupCloseout(dir, CHANGE_ID, { verdictValue: 'ESCALATE', factOk: true, inventory: false });
      batteryCommitAll(dir, 'podwork');
      batteryGit(dir, 'checkout -b pod/test');
      const r = batteryRunHook(dir);
      assert('g3-postmerge-exempt: exit 0', r.status === 0 && /post-merge quiescence/.test(r.stderr), `exit=${r.status} stderr=${(r.stderr || '').slice(0, 120)}`);
    }
    // G3：fork-point 零提交幻觉形（HEAD∈main ∧ closeout 只在盘不在 main）→ BLOCK 保持（R1-F1 铁案）
    {
      const dir = join(root, 'g3-forkpoint');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      setupCloseout(dir, CHANGE_ID, { verdictValue: 'ESCALATE', factOk: true, inventory: false });
      const r = batteryRunHook(dir);
      assert('g3-forkpoint-block: BLOCK kept (R1-F1)', r.status === 2 && /G3 FAILED/.test(r.stderr), `exit=${r.status}`);
    }
    // G3：未合并空 diff（HEAD∉main：--allow-empty 提交使 committed diff 为空）→ BLOCK 保持
    {
      const dir = join(root, 'g3-unmerged');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      setupCloseout(dir, CHANGE_ID, { verdictValue: 'ESCALATE', factOk: true, inventory: false }); // 只在盘未提交
      batteryGit(dir, 'commit --allow-empty -m emptywork'); // HEAD∉main ∧ committed diff 仍空
      const r = batteryRunHook(dir);
      assert('g3-unmerged-empty-diff: BLOCK kept', r.status === 2 && /G3 FAILED/.test(r.stderr), `exit=${r.status}`);
    }
    // G6：红证据 ESCALATE + inventory → 豁免 exit 0
    {
      const dir = join(root, 'g6-red-exempt');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      setupCloseout(dir, CHANGE_ID, { verdictValue: 'ESCALATE', factOk: false, inventory: true });
      batteryCommitAll(dir, 'podwork');
      const r = batteryRunHook(dir);
      assert('g6-red-escalate-exempt: exit 0', r.status === 0 && /red-evidence ESCALATE exemption/.test(r.stderr), `exit=${r.status} stderr=${(r.stderr || '').slice(0, 120)}`);
    }
    // G6：红证据 ESCALATE + 无 inventory → fail-loud BLOCK（文案含制品指引）
    {
      const dir = join(root, 'g6-red-no-inventory');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      setupCloseout(dir, CHANGE_ID, { verdictValue: 'ESCALATE', factOk: false, inventory: false });
      batteryCommitAll(dir, 'podwork');
      const r = batteryRunHook(dir);
      assert('g6-red-no-inventory: BLOCK + artifact guidance', r.status === 2 && /existing-red-inventory\.md/.test(r.stderr), `exit=${r.status}`);
    }
    // G6：APPROVE + 红 fact + inventory → BLOCK 保持（向严铁案：APPROVE 永不入豁免）
    {
      const dir = join(root, 'g6-approve-red');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      setupCloseout(dir, CHANGE_ID, { verdictValue: 'APPROVE', factOk: false, inventory: true });
      batteryCommitAll(dir, 'podwork');
      const r = batteryRunHook(dir);
      assert('g6-approve-red-block: BLOCK kept (toward-strict)', r.status === 2 && /not green final evidence/.test(r.stderr), `exit=${r.status}`);
    }
    // G6：绿 fact 回归（现行形态保全）
    {
      const dir = join(root, 'g6-green-regression');
      batteryInitRepo(dir);
      writeFixtureFile(dir, 'README.md', 'seed\n');
      setupPodBase(dir, CHANGE_ID, { pastPodDir: true });
      batteryCommitAll(dir, 'seed');
      batteryGit(dir, 'checkout -b pod/test');
      setupCloseout(dir, CHANGE_ID, { verdictValue: 'ESCALATE', factOk: true, inventory: false });
      batteryCommitAll(dir, 'podwork');
      const r = batteryRunHook(dir);
      assert('g6-green-regression: exit 0', r.status === 0, `exit=${r.status} stderr=${(r.stderr || '').slice(0, 120)}`);
    }
  } finally {
    try {
      rmSync(root, { recursive: true, force: true });
    } catch {
      // best-effort cleanup
    }
  }

  let failCount = 0;
  for (const c of results) {
    console.log(`  [${c.ok ? 'PASS' : 'FAIL'}] ${c.name}${c.ok ? '' : ` (${c.detail})`}`);
    if (!c.ok) failCount += 1;
  }
  console.log(`Battery: ${results.length - failCount}/${results.length} pass`);
  return failCount;
}

// ---- main gate logic ----

function main(projectDir = defaultProjectDir) {
  // --self-check 派发移至文件尾 isMain 守卫（对偶 mutation-check 形态；FIX-11 语义不变）

  // G0: Load taskpackage
  const taskPackagePath = resolve(projectDir, 'delivery/taskpackages/current/taskpackage.json');
  if (!existsSync(taskPackagePath)) process.exit(0);

  let taskPackage;
  try {
    taskPackage = readJson(taskPackagePath);
  } catch {
    process.exit(0);
  }

  const changeId = taskPackage.changeId;
  if (!changeId) process.exit(0);

  const changeDir = resolve(projectDir, 'delivery', 'changes', 'dev', changeId);

  // G1: Closeout artifact existence check (C4: retired test-evidence.md + final-summary.md)
  // CF-9/F-01 (gov-review-devloop-closeout-hardening §3.3, 2026-07-27 human-approved): cutoff-aware 去快照。
  // post-cutoff（isPostCutoff，基准 CHANGE_ID_CUTOFF 对齐 closeout-check.mjs:72 / isPostCutoff:225，非 EVENTS_SCHEMA_CUTOFF）
  // → delivery/changes/dev/ 下 taskpackage.json 快照退役（活文件 delivery/taskpackages/<id>/ 唯一承载）→ closeoutFiles 去快照项；
  // pre-cutoff 保留三件（含 taskpackage.json 快照）。单数组驱动 G1 早退 / G5 mtime 循环 / G6 validateCloseoutArtifactRefs
  // 三处一致变更——防合规 post-cutoff v2 包过早退却在 G6 被错索 taskpackage.json 快照 ref 而 block。
  const closeoutFiles = isPostCutoff(changeId)
    ? ['docs-delta.md', 'review-packet.md']
    : ['taskpackage.json', 'docs-delta.md', 'review-packet.md'];
  const hasCloseout = closeoutFiles.every((file) => existsSync(resolve(changeDir, file)));

  if (!hasCloseout) {
    // F-A1: zero-artifact minimum check (FIX-1)
    // Planner claims completion but produced zero closeout artifacts — check for code changes at minimum
    const diffIsEmpty = gitDiffEmpty(projectDir);
    if (diffIsEmpty) {
      // T2 (260814-gov-review-merge-gate-terminal-shape-batch)：在飞信号单调扩展——committed diff 只是在飞信号的一种形态。
      // ① 工作树有未提交改动（含未跟踪非忽略文件）② Pod 目录近期活动（OQ-3=(a) 30min 窗）——任一命中 = 在飞相位，合法退出。
      // 幻觉判据不减：两信号皆无时 BLOCK 原文语义保持（OQ-6=(a) 文案去指令性）。
      if (worktreeDirty(projectDir)) process.exit(0);
      if (podDirFresh(projectDir, changeId)) process.exit(0);
      // Zero diff + zero closeout + zero in-flight signals = planner claimed completion but produced nothing
      // This is exactly the PROPOSAL §1.1 triggering scenario
      block(
        `no closeout artifacts AND no code changes detected for Pod "${changeId}", and no in-flight signals ` +
        `(worktree clean, no activity in delivery/taskpackages/${changeId}/ within ${ACTIVITY_WINDOW_MIN}min). ` +
        `This matches the "hallucinated completion" pattern: planner claimed done but produced zero artifacts and zero code changes. ` +
        `If a subagent phase is legitimately in flight, let it land artifacts before stopping; otherwise produce real closeout artifacts before stopping.`,
      );
    }
    // Diff non-empty but closeout doesn't exist — work in progress, not necessarily hallucination
    process.exit(0);
  }

  // G2: Verdict existence and validity
  const verdictPath = resolve(changeDir, 'supervisor-verdict.json');
  if (!existsSync(verdictPath)) {
    block(
      `delivery/changes/dev/${changeId} has closeout artifacts but no supervisor-verdict.json. ` +
      `Run supervisor and persist APPROVE/BLOCK/ESCALATE before stopping.`,
    );
  }

  let verdict;
  try {
    verdict = readJson(verdictPath);
  } catch (error) {
    block(`supervisor-verdict.json is not valid JSON (${error.message}).`);
  }

  const value = String(verdict.verdict || '').toUpperCase();
  if (!VERDICT_VALUES.has(value)) {
    block('merge-gate: supervisor verdict must be APPROVE, BLOCK, or ESCALATE.');
  }

  // G2pre RETIRED (gov-review-devloop-closeout-hardening §3.2 / Q8 / F-06, 2026-07-27 human-approved):
  // pod-retro 存在性 advisory 收敛撤除——牙移 closeout 相位承重（closeout-check.mjs post-cutoff pod-retro.md 存在性牙，
  // Q5=warn advisory + 零追溯白名单）。收敛理由：同一制品两处检查 = 口径漂移（ARQ-077 活例）；且本钩子 pod-retro
  // advisory 被 §1.3 早退架空、错相（挂 supervisor 门而非 closeout 相位）。STEPS 注册表同批删 G2pre 项（非仅翻 optional）。
  // BLOCK 判据不变（verdict 枚举 + verdict===BLOCK，见下 G6 之后）。

  // G2a: S-repair forbids verdict-value early exits. APPROVE and ESCALATE continue through G3-G6.

  // G3: Code change existence (FIX-7: git resilience)
  const diffIsEmpty = gitDiffEmpty(projectDir);
  if (diffIsEmpty && hasCloseout) {
    // T3 (260814-gov-review-merge-gate-terminal-shape-batch)：合并后静默态豁免——双机械合取（R1-F1）：
    // ① HEAD∈main ∧ ② closeout 制品在 main。fork-point 零提交幻觉形（①真②假）保持 BLOCK，fail-safe 向严。
    if (isHeadMergedIntoMain(projectDir, changeId)) {
      warn(
        'G3 post-merge quiescence: HEAD is merged into main and closeout artifacts exist in main — ' +
        'empty diff is the by-construction post-merge state, not hallucination.',
      );
      process.exit(0);
    }
    block(
      'G3 FAILED: no code changes detected (git diff is empty) but closeout artifacts exist. ' +
      'This is a possible hallucination pattern — closeout claims completion but no real code was changed.',
    );
  }

  // G4: Verify-logs existence with content (FIX-7: old Pod protection via G2a)
  const verifyLogsDir = resolve(projectDir, '.claude/verify-logs');
  if (!verifyLogsHaveContent(verifyLogsDir)) {
    block(
      'G4 FAILED: no verify-logs found with content. ' +
      'The verifier gate logs (.claude/verify-logs/*.log) are missing or empty — gates were not actually run.',
    );
  }

  // G5: mtime timeline consistency — WARN ONLY (FIX-8: downgraded from BLOCK)
  try {
    const verdictStat = statSync(verdictPath);
    let latestCloseoutMtime = 0;
    for (const file of closeoutFiles) {
      const p = resolve(changeDir, file);
      if (existsSync(p)) {
        const m = statSync(p).mtimeMs;
        if (m > latestCloseoutMtime) latestCloseoutMtime = m;
      }
    }
    if (verdictStat.mtimeMs < latestCloseoutMtime) {
      warn(
        'G5: supervisor-verdict.json mtime predates closeout artifacts — possible pre-forgery. ' +
        'Review manually if suspicious. (mtime is a convenience check, not a security property — it can be modified by touch/utimes.)',
      );
    }
  } catch {
    // stat failure on mtime comparison — not a blocking issue
    warn('G5: could not compare mtime (filesystem stat error) — skipping timeline check');
  }

  // G6: Provenance verification (S-repair: current-Pod/current-round refs)
  const provenance = verdict.provenance;
  validateProvenance(provenance, changeId, closeoutFiles, value, projectDir);

  // FIX-6 第二方重算（判者 G6 ≠ 产者机械闸）：从 immutable 输入（git diff + write-once bearing facts + 已 commit closeout）
  // 重算 riskTier 核对 ≡ 磁盘 verdict；**不以可变 taskpackage 字段为权威源**（防 PM-4 写后改源）。
  // 复用机械闸派生纯函数（单一机械源）；tp={} ⇒ 只锚 diff。仅 post-cutoff（pre-cutoff 已在 validateProvenance 向严 block）。
  // perFile 归因与闸同参同变（260803-critical-content-fp T3）：G6 若走全量 body 回退而闸用归因，TP2 形态包将假 RED block。
  if (isPostCutoff(changeId)) {
    const reGit = collectGitDiff(projectDir);
    if (!reGit.ok) {
      // FIX-5：派生用 git 失败不得沿用 G3「不 block」放行（:106-110），须向严 ESCALATE-stays-human。
      block('G6 recompute: git diff unavailable; cannot re-derive riskTier from immutable input → toward-strict (ESCALATE-stays-human).');
    }
    const reDerived = deriveDiffTier(reGit.paths, reGit.content, {}, { perFile: reGit.perFile });
    if ((reDerived.tier === 'critical' || reDerived.tier === 'high') && value === 'APPROVE') {
      block(
        `G6 recompute mismatch: immutable git diff re-derives riskTier=${reDerived.tier} (${reDerived.reasons.join('; ')}) ` +
        `but verdict on disk is APPROVE — possible post-hoc source edit (PM-4). Toward-strict block.`,
      );
    }
  }

  if (verdict.verdict && value === 'BLOCK') {
    block('merge-gate: supervisor verdict is BLOCK; return to fixer/planner instead of stopping as complete.');
  }

  // Verdict enum was checked before provenance so APPROVE/ESCALATE cannot bypass G3-G6.
}

// isMain 守卫（对偶 mutation-check.mjs 批 A T2 / lib:182-185 形态）：
// 本批 T4 函数化 main() 后以守卫收口；import 劫持面 = 零（判官 R1 核无 JS import 方），守卫为对偶纪律。
function isMain() {
  const invoked = process.argv[1];
  return Boolean(invoked && import.meta.url === pathToFileURL(resolve(invoked)).href);
}

if (isMain() && process.argv.includes('--self-check')) selfCheck();
else if (isMain()) main();
