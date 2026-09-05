#!/usr/bin/env node
// pre-merge-escalation-gate.mjs — 前置升级闸（auto-merge 前的机械判，非门、非 merge 执行者）
//
// 治理声明：owner=architect · approver=human · G3（DEV-GOVERNANCE §6 强制执行层行覆盖）。
// 批准物：源仓 changes/arch/260831-gov-review-auto-merge-local-main/PROPOSAL.md（FROZEN v1.0，T1）
//   —— 同步自源仓 commit 6154a6c（治理迁移指南 §4.3 同步通道）；契约面路径按 KidPal 适配为 docs/contracts/**。
// 分工：本脚本只重算「例外清单是否命中」，命中即 exit 1 且报命中项；放行 exit 0。**合并前必经，
//   ——它放行 merge-driver 才跑**（merge-driver 在 processPod Step 1 前 invoke，见 PROPOSAL T2）。
//
// 例外清单（FROZEN §2 T1，按档适用）：
//   两档通用：
//     1. diff 触 docs/contracts/** 且同批无契约门证据 → BLOCK（契约面人签字制不动）
//     2. 源 commit 落在 main/master 上（off-main 隔离被破坏）：无独立分支可合 → BLOCK
//     3. change 存在未 resolve 的 agent-escalation 或 blocked 态 → BLOCK
//   dev-loop 档：
//     4. riskTier ∈ {high, critical}（读 supervisor-verdict.json）→ BLOCK
//     5. supervisor APPROVE provenance 不可机验 → BLOCK
//   Track B 档（治理态无 supervisor-verdict，替换为）：
//     6. architect GREEN 机锚缺失（PROPOSAL Status 标 FROZEN + §7 GREEN + landing-verdict 在产）→ BLOCK
//
// 用法：
//   node .claude/scripts/pre-merge-escalation-gate.mjs <changeId> [--source-ref <ref>] [--project-dir <dir>]
//   node .claude/scripts/pre-merge-escalation-gate.mjs <changeId> --human-approval-ref <eventId>  # ESCALATE 人批放行（核验 pod-events human-decision 事件）
//   node .claude/scripts/pre-merge-escalation-gate.mjs --self-check
//
// 退出码：0 放行 / 1 命中（BLOCK）/ 2 用法错。

import { execFileSync, execSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { join, resolve } from 'node:path';

const projectDirOf = () => resolve(process.env.CLAUDE_PROJECT_DIR || process.cwd());

// ---- 纯函数：输入可注入，self-check 可测 ----

/** git rev-parse 一个 ref，失败返回 null。 */
function revParse(execFn, ref) {
  try {
    return execFn(`git rev-parse --verify ${ref}`).trim();
  } catch {
    return null;
  }
}

/** 判 a 是否 main/master 的祖先或等于（即 a 已含在 main 中 → 无独立可合分支）。 */
function sourceReachableFromMain(execFn, source) {
  const mainRef = revParse(execFn, 'main') || revParse(execFn, 'master');
  if (!mainRef) return { ok: null, reason: 'no-main-ref' }; // fail-closed：无 main ref 无法判，交上层
  const src = revParse(execFn, source);
  if (!src) return { ok: true, reason: 'source-absent' }; // 源 ref 不存在（未 commit）→ 更应 BLOCK 由 feature-branch 前置保证
  try {
    // --is-ancestor：src 是 main 的祖先 → src 已含在 main 里，无可合并内容（P4 破坏）
    execFn(`git merge-base --is-ancestor ${src} ${mainRef}`);
    return { ok: true, reason: 'source-on-main' };
  } catch {
    return { ok: false, reason: 'source-diverged' };
  }
}

/** 读 supervisor-verdict.json（dev-loop 档），缺失返回 null。 */
function readSupervisorVerdict(dir, changeId) {
  const p = join(dir, 'changes', 'dev', changeId, 'supervisor-verdict.json');
  if (!existsSync(p)) return null;
  try {
    return JSON.parse(readFileSync(p, 'utf8'));
  } catch {
    return null;
  }
}

/**
 * 核验 ESCALATE 人批事件（T1，260904-gov-review-escalate-merge-approval；OQ1=(a)）：
 * 读 <dir>/.claude/taskpackages/<changeId>/pod-events.jsonl，不在场时回退 pods 记录
 * （deps.podsRecordPath，由调用方解好 git common dir）里 worktreePath 下同路径。
 * 找 eventId 命中记录，五要素全过才 ok：eventType/actor/payload.decision/changeId/schemaVersion。
 * 任一不符 → { ok:false, reason:<点名哪要素不符> }（fail-closed）。readText 可注入，self-check 可测。
 */
export function resolveHumanApproval(dir, changeId, eventId, deps = {}) {
  const readText = deps.readText || ((p) => (existsSync(p) ? readFileSync(p, 'utf8') : null));
  if (!eventId) return { ok: false, reason: 'eventId 缺失' };
  let raw = readText(join(dir, '.claude', 'taskpackages', changeId, 'pod-events.jsonl'));
  if (raw === null && deps.podsRecordPath) {
    // 合并前事件可能只在 worktree 侧落盘：经 pods 记录 worktreePath 回退
    try {
      const wt = JSON.parse(readText(deps.podsRecordPath) || 'null')?.worktreePath;
      if (wt) raw = readText(join(wt, '.claude', 'taskpackages', changeId, 'pod-events.jsonl'));
    } catch { /* 回退失败按不在场处理 */ }
  }
  if (raw === null) return { ok: false, reason: 'pod-events.jsonl 不在场（主仓与 worktree 回退均未找到）' };
  let hit = null;
  for (const line of raw.split('\n')) {
    const t = line.trim();
    if (!t) continue;
    try {
      const rec = JSON.parse(t);
      if (rec?.eventId === eventId) { hit = rec; break; }
    } catch { continue; }
  }
  if (!hit) return { ok: false, reason: `eventId=${eventId} 不在 pod-events 中` };
  if (hit.eventType !== 'human-decision') return { ok: false, reason: `eventType=${hit.eventType}（应为 human-decision）` };
  if (hit.actor !== 'human') return { ok: false, reason: `actor=${hit.actor}（应为 human）` };
  if (hit.payload?.decision !== 'approve-escalated-merge') return { ok: false, reason: `payload.decision=${hit.payload?.decision}（应为 approve-escalated-merge）` };
  if (hit.changeId !== changeId) return { ok: false, reason: `changeId=${hit.changeId}（应为本 changeId ${changeId}）` };
  if (hit.schemaVersion !== 1) return { ok: false, reason: `schemaVersion=${hit.schemaVersion}（应为 1）` };
  return { ok: true, reason: 'human-approval verified' };
}

/** 读治理 PROPOSAL §7 与 Status 行 + landing-verdict（Track B 档）。 */
function readGovAnchors(dir, changeId) {
  const proposalPath = join(dir, 'changes', 'arch', changeId, 'PROPOSAL.md');
  const landingVerdictPath = join(dir, 'changes', 'arch', changeId, 'landing-verdict.md');
  if (!existsSync(proposalPath)) return null;
  const text = readFileSync(proposalPath, 'utf8');
  const frozen = /^\*\*状态：FROZEN v\d+\.\d+/m.test(text);
  // §7 含判官 GREEN verdict：宽松匹配「GREEN」且「verdict」同段（拒绝仅 history 提及 GREEN）
  const green = /verdict[：:]\s*GREEN|结论[：:]\s*GREEN|GREEN/gi.test(text);
  const landingVerdict = existsSync(landingVerdictPath);
  return { frozen, green, landingVerdict };
}

/** 枚举 contracts diff：--name-only <source>..<target>，命中 contracts/** 返回 true。 */
function hasContractsDiff(execFn, source, target) {
  try {
    const out = execFn(`git diff --name-only ${source}..${target}`);
    return out.split('\n').some((l) => l.startsWith('docs/contracts/'));
  } catch {
    return false;
  }
}

/**
 * 主判：给定可注入上下文，产 { blocked, reasons[] }。
 * ctx = { execFn, changeId, source, kind: 'devloop'|'gov'|'unknown',
 *         supervisorVerdict, govAnchors, contractsDiff: boolean }
 */
export function evaluate(ctx) {
  const reasons = [];
  const push = (s) => reasons.push(s);

  // dev-loop 档人批豁免（T1，260904-gov-review-escalate-merge-approval；OQ2=(a)）：
  // verdict=ESCALATE + provenance 完整 + human-approval 事件核验 ok → 豁免 riskTier / 非 APPROVE / blocked 三条理由
  //（blocked 在此只是 ESCALATE 停等人类的驻留标记，人批事件即其 resolve）。永不豁免：provenance 缺失、off-main、契约面、verdict=BLOCK。
  const approval = ctx.humanApproval ?? null;
  const escalateExempt =
    ctx.kind === 'devloop' &&
    approval?.ok === true &&
    ctx.supervisorVerdict?.verdict === 'ESCALATE' &&
    !!ctx.supervisorVerdict?.provenance?.mechanicalSupervisorGate?.ran &&
    !!ctx.supervisorVerdict?.provenance?.mechanicalSupervisorGate?.producedBy;

  // 通用 2：off-main
  if (ctx.source) {
    const off = sourceReachableFromMain(ctx.execFn, ctx.source);
    if (off.ok === true && off.reason === 'source-on-main') {
      push('off-main：源 commit 已含在 main 内，无独立分支可合（隔离被破坏）');
    } else if (off.ok === null) {
      push('off-main：无 main ref 无法判（fail-closed）');
    } else if (off.reason === 'source-absent') {
      push('off-main：源 ref 不存在（未 commit），无合并可言');
    }
  }

  // 通用 1：contracts diff 无契约门证据
  if (ctx.contractsDiff && !ctx.hasContractGateEvidence) {
    push('docs/contracts/** diff 且同批无契约门证据（契约面人签字制不动）');
  }

  // 通用 3：escalation 未 resolve / blocked（blocked 仅在人批豁免成立时视同 resolve，OQ2=(a)）
  if (ctx.blocked && !escalateExempt) push('change 处 blocked 态（未 resolve）');
  if (ctx.escalationUnresolved) push('存在未 resolve 的 agent-escalation 事件');

  // 人批 flag fail-closed：给了 flag 但核验不过 → BLOCK 且点名失败项
  if (approval && !approval.ok) push(`human-approval 核验失败：${approval.reason}`);

  if (ctx.kind === 'devloop') {
    // dev-loop 档 4：riskTier（人批豁免成立时放行）
    const riskTier = ctx.supervisorVerdict?.riskTier;
    if ((riskTier === 'high' || riskTier === 'critical') && !escalateExempt) {
      push(`riskTier=${riskTier}（supervisor 升级）`);
    }
    // dev-loop 档 5：provenance（缺失永不豁免）
    const prov = ctx.supervisorVerdict?.provenance?.mechanicalSupervisorGate;
    if (!prov?.ran || !prov?.producedBy) {
      push('supervisor APPROVE provenance 不可机验（supervisor-verdict 缺失或 mechanicalSupervisorGate 不全）');
    } else if (ctx.supervisorVerdict?.verdict !== 'APPROVE' && !escalateExempt) {
      push(`supervisor verdict=${ctx.supervisorVerdict?.verdict}（非 APPROVE）`);
    }
  } else if (ctx.kind === 'gov') {
    // Track B 档 6：architect GREEN 机锚
    const a = ctx.govAnchors;
    if (!a) push('Track B：PROPOSAL 不在（无法验 architect GREEN 机锚）');
    else if (!a.frozen) push('Track B：PROPOSAL Status 未标 FROZEN');
    else if (!a.green) push('Track B：§7 无 architect GREEN verdict');
    else if (!a.landingVerdict) push('Track B：landing-verdict 不在产');
  } else {
    push(`unknown change kind（changeId 既无 changes/dev 也无 changes/arch）`);
  }

  return { blocked: reasons.length > 0, reasons };
}

/** 解 pods 记录路径：主仓优先，linked worktree 回退 git common dir。不在场返回 null。 */
function findPodsRecordPath(projectDir, changeId) {
  const mainPath = join(projectDir, '.git', 'devloop', 'pods', `${changeId}.json`);
  if (existsSync(mainPath)) return mainPath;
  try {
    const cd = execSync('git rev-parse --git-common-dir', { cwd: projectDir, encoding: 'utf8', stdio: 'pipe' }).trim();
    const commonPath = join(resolve(projectDir, cd), 'devloop', 'pods', `${changeId}.json`);
    if (existsSync(commonPath)) return commonPath;
  } catch { /* fallthrough */ }
  return null;
}

/** 从磁盘组 ctx（live path）。 */
function buildCtx(projectDir, changeId, sourceRef, humanApprovalRef = null) {
  const devDir = join(projectDir, 'changes', 'dev', changeId);
  const archDir = join(projectDir, 'changes', 'arch', changeId);
  const hasDev = existsSync(devDir);
  const hasArch = existsSync(archDir);
  const kind = hasArch ? 'gov' : hasDev ? 'devloop' : 'unknown';

  const source = sourceRef || revParse((c) => execSync(c, { encoding: 'utf8', stdio: 'pipe' }), 'HEAD');

  const supervisorVerdict = readSupervisorVerdict(projectDir, changeId);
  const govAnchors = kind === 'gov' ? readGovAnchors(projectDir, changeId) : null;
  const contractsDiff = hasContractsDiff(
    (c) => execSync(c, { encoding: 'utf8', stdio: 'pipe' }),
    source,
    'main',
  );
  const podsRecordPath = findPodsRecordPath(projectDir, changeId);
  const blocked = (() => {
    if (!podsRecordPath) return false;
    try {
      return JSON.parse(readFileSync(podsRecordPath, 'utf8')).status === 'blocked';
    } catch {
      return false;
    }
  })();
  const humanApproval = humanApprovalRef
    ? resolveHumanApproval(projectDir, changeId, humanApprovalRef, { podsRecordPath })
    : null;

  return {
    changeId,
    source,
    kind,
    supervisorVerdict,
    govAnchors,
    contractsDiff,
    hasContractGateEvidence: false, // 契约门证据（contract-change-packet）后续接入；现 false → 契约面 diff 恒 BLOCK（向严）
    blocked,
    escalationUnresolved: false, // pod-events 聚合待接入；现 false（升级闸 v1 只覆盖 blocked 态 + 显式判定）
    humanApproval, // T1：--human-approval-ref 核验结果；未带 flag 为 null
  };
}

// ---- self-check（真空绿规约：断言输入非空 + 逐 case 预期）----
function selfCheck() {
  const cases = [];
  const mkCtx = (over) => ({
    execFn: (c) => {
      if (c.includes('--is-ancestor')) throw new Error('not ancestor');
      return 'x';
    },
    changeId: 'x',
    source: 'feature',
    kind: 'devloop',
    supervisorVerdict: {
      verdict: 'APPROVE',
      riskTier: 'low',
      provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'supervisor-verdict-gate.mjs' } },
    },
    govAnchors: null,
    contractsDiff: false,
    hasContractGateEvidence: false,
    blocked: false,
    escalationUnresolved: false,
    humanApproval: null, // T1：--human-approval-ref 核验结果
    ...over,
  });

  const expect = (name, over, wantBlocked, wantReason) => {
    const r = evaluate(mkCtx(over));
    const ok = r.blocked === wantBlocked && r.reasons.some((x) => x.includes(wantReason));
    cases.push({ name, ok, got: JSON.stringify(r.reasons) });
  };

  // 放行：clean devloop 全绿
  cases.push({ name: 'devloop-clean-pass', ok: !evaluate(mkCtx({})).blocked, got: '' });
  // 通用 2 off-main：source-on-main
  expect('off-main-block', { execFn: (c) => (c.includes('--is-ancestor') ? '' : 'x'), source: 'feature' }, true, 'off-main');
  // 通用 1 contracts diff
  expect('contracts-block', { contractsDiff: true }, true, 'docs/contracts');
  // 通用 3 blocked
  expect('blocked-block', { blocked: true }, true, 'blocked');
  // dev-loop 档 4 riskTier high
  expect('riskTier-high-block', { supervisorVerdict: { verdict: 'APPROVE', riskTier: 'high', provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'x' } } } }, true, 'riskTier');
  // dev-loop 档 5 provenance 缺
  expect('provenance-block', { supervisorVerdict: { verdict: 'APPROVE', riskTier: 'low', provenance: null } }, true, 'provenance');
  // dev-loop 非 APPROVE
  expect('verdict-not-approve-block', { supervisorVerdict: { verdict: 'BLOCK', riskTier: 'low', provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'x' } } } }, true, '非 APPROVE');
  // gov 档 6：GREEN 机锚齐全 → 放行
  cases.push({
    name: 'gov-clean-pass',
    ok: !evaluate(mkCtx({ kind: 'gov', supervisorVerdict: null, govAnchors: { frozen: true, green: true, landingVerdict: true } })).blocked,
    got: '',
  });
  // gov 档 6：FROZEN 缺
  expect('gov-not-frozen-block', { kind: 'gov', supervisorVerdict: null, govAnchors: { frozen: false, green: true, landingVerdict: true } }, true, 'FROZEN');
  // unknown kind
  expect('unknown-kind-block', { kind: 'unknown', supervisorVerdict: null }, true, 'unknown');

  // T1 人批豁免 8 例（260904-gov-review-escalate-merge-approval；真空绿规约：输入非空、逐 case 有预期）
  const escVerdict = {
    verdict: 'ESCALATE',
    riskTier: 'high',
    provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'supervisor-verdict-gate.mjs' } },
  };
  // 1) ESCALATE 无 flag → 行为零变更（riskTier 理由仍在）
  expect('escalate-no-approval-still-block', { supervisorVerdict: escVerdict }, true, 'riskTier');
  // 2) ESCALATE + 人批核验 ok → 放行（含 blocked 联动豁免，OQ2=(a)）
  cases.push({
    name: 'escalate-with-approval-pass',
    ok: !evaluate(mkCtx({ supervisorVerdict: escVerdict, humanApproval: { ok: true, reason: 'human-approval verified' }, blocked: true })).blocked,
    got: '',
  });
  // 3-6) flag 给了但五要素核验失败 → fail-closed 点名失败项
  expect('approval-wrong-eventid-block', { supervisorVerdict: escVerdict, humanApproval: { ok: false, reason: 'eventId=x 不在 pod-events 中' } }, true, 'human-approval 核验失败');
  expect('approval-actor-not-human-block', { supervisorVerdict: escVerdict, humanApproval: { ok: false, reason: "actor=agent（应为 human）" } }, true, 'actor=');
  expect('approval-wrong-decision-block', { supervisorVerdict: escVerdict, humanApproval: { ok: false, reason: 'payload.decision=reject（应为 approve-escalated-merge）' } }, true, 'payload.decision=');
  expect('approval-wrong-changeid-block', { supervisorVerdict: escVerdict, humanApproval: { ok: false, reason: 'changeId=other（应为本 changeId x）' } }, true, 'changeId=');
  // 7) verdict=BLOCK 不可人批豁免（证据缺失不是风险接受能抹平的形态）
  expect('block-verdict-not-waived', { supervisorVerdict: { verdict: 'BLOCK', riskTier: 'high', provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'x' } } }, humanApproval: { ok: true, reason: 'human-approval verified' } }, true, '非 APPROVE');
  // 8) provenance 缺失永不豁免
  expect('escalate-provenance-missing-not-waived', { supervisorVerdict: { verdict: 'ESCALATE', riskTier: 'high', provenance: null }, humanApproval: { ok: true, reason: 'human-approval verified' } }, true, 'provenance');

  const fail = cases.filter((c) => !c.ok);
  process.stdout.write(
    `${JSON.stringify({ selfCheck: 'pre-merge-escalation-gate', pass: cases.length - fail.length, fail: fail.length, total: cases.length, ok: fail.length === 0, failures: fail.length ? fail : undefined })}\n`,
  );
  process.exit(fail.length ? 1 : 0);
}

// ---- 入口 ----
if (process.argv.includes('--self-check')) {
  selfCheck();
}

const args = process.argv.slice(2);
const changeId = args.find((a) => !a.startsWith('--'));
const sourceRef = (() => {
  const i = args.indexOf('--source-ref');
  return i >= 0 ? args[i + 1] : null;
})();
const projectDirArg = (() => {
  const i = args.indexOf('--project-dir');
  return i >= 0 ? args[i + 1] : null;
})();
const humanApprovalRef = (() => {
  const i = args.indexOf('--human-approval-ref');
  return i >= 0 ? args[i + 1] ?? null : null;
})();

if (humanApprovalRef !== null && (humanApprovalRef.startsWith('--') || humanApprovalRef === '')) {
  process.stderr.write('[pre-merge-escalation-gate] FATAL: --human-approval-ref 需要一个 <eventId> 参数\n');
  process.exit(2);
}

if (!changeId) {
  process.stderr.write('[pre-merge-escalation-gate] FATAL: missing <changeId>\n');
  process.exit(2);
}

const dir = projectDirArg ? resolve(projectDirArg) : projectDirOf();
const ctx = buildCtx(dir, changeId, sourceRef, humanApprovalRef);

// T1 防误用：人批豁免只对 dev-loop 档有意义（治理档有 FROZEN+GREEN+landing-verdict 机锚，无人批豁免概念）
if (humanApprovalRef && (ctx.kind === 'gov' || ctx.kind === 'unknown')) {
  process.stderr.write(`[pre-merge-escalation-gate] FATAL: --human-approval-ref 仅适用于 dev-loop 档（changeId=${changeId} kind=${ctx.kind}，无人批豁免概念）\n`);
  process.exit(2);
}

const result = evaluate(ctx);

if (result.blocked) {
  process.stderr.write(`[pre-merge-escalation-gate] BLOCK (changeId=${changeId}, kind=${ctx.kind}):\n`);
  result.reasons.forEach((r) => process.stderr.write(`  - ${r}\n`));
  process.stderr.write('  → 升级 human，不得自动 merge。\n');
  process.exit(1);
}

const approvalNote = humanApprovalRef ? ` — human-approval verified (eventId=${humanApprovalRef})` : '';
process.stdout.write(`[pre-merge-escalation-gate] PASS (changeId=${changeId}, kind=${ctx.kind})${approvalNote} — 无例外命中，可交给 merge-driver。\n`);
process.exit(0);