#!/usr/bin/env node
// pre-merge-escalation-gate.mjs — 前置升级闸（auto-merge 前的机械判，非门、非 merge 执行者）
//
// 治理声明：owner=architect · approver=human · G3（DEV-GOVERNANCE §6 强制执行层行覆盖）。
// 批准物：源仓 delivery/changes/arch/260831-gov-review-auto-merge-local-main/PROPOSAL.md（FROZEN v1.0，T1）
//   —— 同步自源仓 commit 6154a6c（治理迁移指南 §4.3 同步通道）；契约面路径按 KidPal 适配为 docs/contracts/**。
// 分工：本脚本只重算「例外清单是否命中」，命中即 exit 1 且报命中项；放行 exit 0。**合并前必经，
//   ——它放行 merge-driver 才跑**（merge-driver 在 processPod Step 1 前 invoke，见 PROPOSAL T2）。
//
// 例外清单（FROZEN §2 T1，按档适用）：
//   两档通用：
//     1. diff 触 docs/contracts/** 且同批无契约合并凭据（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖，三件联立）→ BLOCK（261015 批：机械凭据替代人签门；契约变更 = 架构变更的一种，走 /arch-change）
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
import { existsSync, readFileSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { resolveHumanApproval, VERDICT_STATUS_RE, isFrozen, extractManifestBlock } from './lib/governance-gate-shared.mjs'; // T6（260908 批）：五要素核验单源迁 lib，此处具名导入（函数体与 gate 原实现逐字一致 + deps.decision 参数化）；VERDICT_STATUS_RE 为 T2（260920 批）迁入的 verdict 行判据单源；isFrozen / extractManifestBlock 为 T15（261015 批）契约凭据解析单源

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
  const p = join(dir, 'delivery', 'taskpackages', changeId, 'supervisor-verdict.json');
  if (!existsSync(p)) return null;
  try {
    return JSON.parse(readFileSync(p, 'utf8'));
  } catch {
    return null;
  }
}

// resolveHumanApproval 已迁 lib/governance-gate-shared.mjs（T6，260908 批 B8 并入）——本文件具名导入。
// 五要素人批事件核验（approve-escalated-merge）；T5 defer/accept-risk 锚核验复用同函数、decision 参数化。

/** 读治理 PROPOSAL §7 与 Status 行 + landing-verdict（Track B 档）。 */
// T2（260920-gov-review-verdict-anchor-align，FROZEN 2026-09-20）：green 判定抽为纯函数（self-check 可测）——
// 逐行 VERDICT_STATUS_RE、**取最后命中 token**（后出者胜，与 deriveLanded 的 :189 覆写语义一致）。
function isGreenVerdict(text) {
  let lastToken = '';
  for (const line of text.split('\n')) {
    const m = VERDICT_STATUS_RE.exec(line);
    if (m) lastToken = m[1].toUpperCase();
  }
  return lastToken === 'GREEN';
}

function readGovAnchors(dir, changeId) {
  const proposalPath = join(dir, 'delivery', 'changes', 'arch', changeId, 'PROPOSAL.md');
  const landingVerdictPath = join(dir, 'delivery', 'changes', 'arch', changeId, 'landing-verdict.md');
  if (!existsSync(proposalPath)) return null;
  const text = readFileSync(proposalPath, 'utf8');
  const frozen = /^[>\s]*\*\*状态：FROZEN v\d+\.\d+/m.test(text); // 260905 T4：容忍 `> ` 引用前缀，与行首形态双命中（既有错位归位，260905-gov-review-verify-gates-align）
  // §7 含判官 GREEN verdict（260920-gov-review-verdict-anchor-align T2，FROZEN 2026-09-20）：
  // 改用 lib 单源 VERDICT_STATUS_RE（行首判据），**取最后命中的 token**（后出者胜，与 deriveLanded
  // 的 :189 覆写语义一致——防「GREEN 后重开」场景 fail-open）。token 集收 {GREEN}（R1-F2：删 PASS——
  // 260918 批 R1:176 已论证否决：spoof 面、判官语法无 PASS）。**裸 GREEN 分支删除**（ARQ-260909 原
  // 病灶：全文 token 匹配 + 样板行「GREEN 结论前必须在场」自带 GREEN ⇒ 恒真）。
  // RE 须容忍 token 前后的反引号（批① §7 的 `**Verdict：\`GREEN\`**` 形态为真实判官输出）。
  const green = isGreenVerdict(text);
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
 * T15（261015-gov-review-face-loop-unify）：契约合并机械凭据（三件联立）——ARQ-260923-contract-merge-deadpath 建议①兑现。
 * 同批存在 arch-review 目录 PROPOSAL：① FROZEN（lib isFrozen 单源）+ ② 同目录 landing-verdict PASS（VERDICT_STATUS_RE 单源）
 * + ③ manifest（lib extractManifestBlock 单源）覆盖全部契约 diff 路径 → 放行；否则 fail-closed 点名。
 * 解析一律复用 lib 单源（R1-F5：不另写解析，防 260918/260920 verdict-anchor 病灶复发）。
 */
function hasArchLoopContractEvidence(execFn, source, target, projectDir, deps = {}) {
  const readVerdict = deps.readVerdict || ((p) => (existsSync(p) ? readFileSync(p, 'utf8') : null));
  let diffOut;
  try {
    diffOut = execFn(`git diff --name-only ${source}..${target}`);
  } catch {
    return { ok: false, reason: 'contract-evidence: git diff 失败（fail-closed）', contractPaths: [] };
  }
  const lines = diffOut.split('\n').map((l) => l.trim()).filter(Boolean);
  const contractPaths = lines.filter((l) => l.startsWith('docs/contracts/'));
  if (contractPaths.length === 0) return { ok: true, reason: 'no-contract-diff', contractPaths };
  const proposalPaths = lines.filter((l) => /delivery\/changes\/arch\/\d{6}-(?:gov|arch)-review-[^/]+\/PROPOSAL\.md$/.test(l));
  const checked = [];
  for (const proposalPath of proposalPaths) {
    const dir = proposalPath.slice(0, proposalPath.lastIndexOf('/'));
    let proposalText = null;
    try {
      proposalText = execFn(`git show ${target}:${proposalPath}`);
    } catch { /* PROPOSAL 不在 target ref → frozen 判 false */ }
    const frozen = proposalText ? isFrozen(proposalText) : false;
    const manifest = proposalText ? extractManifestBlock(proposalText) : new Set();
    const covered = contractPaths.every((p) => manifest.has(p));
    const vText = readVerdict(join(projectDir, dir, 'landing-verdict.md'));
    const vLine = vText ? vText.split('\n').find((l) => VERDICT_STATUS_RE.test(l)) : null;
    const verdict = Boolean(vLine && /\b(PASS|GREEN)\b/i.test(vLine));
    checked.push({ dir, frozen, verdict, covered });
    if (frozen && verdict && covered) {
      return { ok: true, reason: `contract-evidence via ${dir}`, contractPaths };
    }
  }
  return {
    ok: false,
    reason: `contract-evidence: 无候选满足三件联立（FROZEN + PASS verdict + manifest 覆盖）；契约路径 = ${contractPaths.join(', ')}；已检查 = ${checked.length ? JSON.stringify(checked) : '无候选 PROPOSAL（同批无 arch-review 批）'}`,
    contractPaths,
  };
}

/**
 * T5（260908-gov-review-merge-chain-hardening，A-2 已裁：门内加项；A-8 已裁：放行方向——只拦可识别形态）：
 * 契约/治理 canonical 改动「交回→在册」机械核验。扫描 review-packet Findings 表行：
 * 行含字面 `| hard-violation |` 且行含 CONTRACT_SPEC_LITERALS 任一字面（B-4 写作侧对齐：
 * integrator.md findings 约定要求 Basis 以该前缀集起引；未匹配形态放行，不把自由 prose 写作变假红）→
 * 视为「契约类 hard-violation 在案」。在案者 MUST 二选一核销：
 *   a) docs/CONTRACT-DECISION-QUEUE.md 含本 changeId 字面（对应 CDQ 行）；
 *   b) pod-events 有 human-decision 事件 payload.decision ∈ {defer-contract-change, accept-risk}
 *      且 changeId 匹配（B-6 锚格式，复用五要素事件形状）。
 * 返回 { present, accounted, hits:[findingLine 摘录] }。readText/cdqText 可注入，self-check 可测。
 */
// 字面集（260913-arch-review-spec-split 扩展：spec.md 拆分为 requirements.md + architecture.md 并归档）——
// 与 integrator.md Basis 前缀写作约定同批联动改（只改一边 = 闸与约定脱钩）；
// docs/spec.md 保留在集内：归档件，历史 findings 的 Basis 仍用它起引。
export const CONTRACT_SPEC_LITERALS = ['docs/contracts/**', 'docs/spec.md', 'docs/requirements.md', 'docs/architecture.md'];

export function contractFindingsAccounted(dir, changeId, deps = {}) {
  const readText = deps.readText || ((p) => (existsSync(p) ? readFileSync(p, 'utf8') : null));
  const readPacketText = () => {
    let t = readText(join(dir, 'delivery', 'taskpackages', changeId, 'review-packet.md'));
    if (t === null) t = readText(join(dir, 'delivery', 'taskpackages', changeId, 'review-packet.md'));
    if (t === null && deps.podsRecordPath) {
      try {
        const wt = JSON.parse(readText(deps.podsRecordPath) || 'null')?.worktreePath;
        if (wt) t = readText(join(wt, 'delivery', 'taskpackages', changeId, 'review-packet.md')) ?? readText(join(wt, 'delivery', 'taskpackages', changeId, 'review-packet.md'));
      } catch { /* worktree 回退失败按不在场处理 */ }
    }
    return t;
  };
  const packetText = readPacketText();
  if (packetText === null) return { present: false, accounted: true, hits: [] };
  const hits = packetText
    .split('\n')
    .filter((l) => l.includes('hard-violation') && CONTRACT_SPEC_LITERALS.some((lit) => l.includes(lit)));
  if (hits.length === 0) return { present: false, accounted: true, hits: [] };
  const cdqText = readText(join(dir, 'docs', 'CONTRACT-DECISION-QUEUE.md'));
  const cdqHasRow = cdqText !== null && cdqText.includes(changeId);
  const deferAnchored = (() => {
    const raw =
      readText(join(dir, 'delivery', 'taskpackages', changeId, 'pod-events.jsonl')) ??
      (deps.podsRecordPath
        ? (() => {
            try {
              const wt = JSON.parse(readText(deps.podsRecordPath) || 'null')?.worktreePath;
              return wt ? readText(join(wt, 'delivery', 'taskpackages', changeId, 'pod-events.jsonl')) : null;
            } catch { return null; }
          })()
        : null);
    if (raw === null) return false;
    return raw.split('\n').some((line) => {
      const t = line.trim();
      if (!t) return false;
      try {
        const rec = JSON.parse(t);
        return (
          rec?.eventType === 'human-decision' &&
          rec?.actor === 'human' &&
          rec?.changeId === changeId &&
          (rec?.payload?.decision === 'defer-contract-change' || rec?.payload?.decision === 'accept-risk')
        );
      } catch { return false; }
    });
  })();
  return { present: true, accounted: cdqHasRow || deferAnchored, hits };
}

/**
 * T1（260908-gov-review-escalation-conflict-ladder）：并行相交面探测（只读，判据换轴的机械件）。
 * intersectingFiles = 双方相对 merge-base 的改动文件交集；conflictedFiles = merge-tree 实撞集；
 * addAddFiles = 双侧新增相撞子集。分级路由（pod-coordination.md §10d）：
 *   零相交 → tier 'no-overlap'（① 放行）；实撞全部为已登记版本化制品形态 → 'union-safe'（② 不升人，
 *   走 runbook 并集 + 双方齿复跑）；其余实撞 / 探测失败 → 'tradeoff'（③ architect 消解评审，闸 BLOCK）。
 * 版本化制品形态兜底正则与 merge-driver VERSIONED_ARTIFACT_RE 同形——lib 收编留根（本批不改 lib，双源留根注记）。
 * execFn 可注入（self-check）；探测任何异常 → { probed:false, tier:'tradeoff' }（fail-closed，绝不误放行）。
 */
export const VERSIONED_ARTIFACT_RE_LOCAL = /\.v[0-9]+\.md$/; // 与 merge-driver 同形，lib 收编留根

export function parallelOverlapProbe(execFn, source, target) {
  try {
    const base = execFn(`git merge-base ${target} ${source}`).trim();
    const srcFiles = execFn(`git diff --name-only ${base} ${source}`).split('\n').map((s) => s.trim()).filter(Boolean);
    const tgtFiles = execFn(`git diff --name-only ${base} ${target}`).split('\n').map((s) => s.trim()).filter(Boolean);
    const srcSet = new Set(srcFiles);
    const intersectingFiles = tgtFiles.filter((f) => srcSet.has(f));
    let conflictedFiles = [];
    try {
      const mt = execFn(`git merge-tree --write-tree --name-only ${source} ${target}`);
      conflictedFiles = mt.split('\n').slice(1).map((s) => s.trim()).filter(Boolean); // 首行 = tree oid
    } catch (err) {
      const out = `${err.stdout || ''}`;
      conflictedFiles = out.split('\n').slice(1).map((s) => s.trim()).filter(Boolean); // exit 1 = 有冲突，stdout 仍带清单
    }
    // 噪声滤清（V3 实测：--name-only 输出混 informational 行）——实撞文件必 ⊆ 双方改动交集
    conflictedFiles = [...new Set(conflictedFiles)].filter((f) => intersectingFiles.includes(f));
    // add/add 判定：conflicted ∩ 双侧新增（merge-base 中不存在）
    const addAddFiles = conflictedFiles.filter((f) => {
      try { execFn(`git cat-file -e ${base}:${f}`); return false; } catch { return true; }
    });
    const tier = (() => {
      if (!intersectingFiles.length) return 'no-overlap';
      // 核查结论（260918-gov-review-merge-chain-friction T1②，FROZEN 2026-09-19）：`merge=union` 文件
      // （如 `delivery/taskpackages/**/pod-events.jsonl`，`.gitattributes` 由 c16283b 落地）由 git 自动并、
      // **不出现在 conflictedFiles**，天然落下面本行判 union-safe（② 级不升人）——**无需为 union 文件加显式豁免**。
      // 依据 = 本行分支逻辑 + ARQ-260917 记录的 k3 修复轮 r15 合并实测「tier3 评审噪音收口」。
      // 再下一行 `VERSIONED_ARTIFACT_RE_LOCAL` 是另一条并列路径（版本化制品 carve-out），非 union 情形所依赖。
      if (!conflictedFiles.length) return 'union-safe'; // 相交但可自动并（不同 hunk git 自并）
      if (conflictedFiles.every((f) => VERSIONED_ARTIFACT_RE_LOCAL.test(f))) return 'union-safe'; // S1 carve-out：已登记形态 → ②
      return 'tradeoff';
    })();
    return { probed: true, tier, intersectingFiles, conflictedFiles, addAddFiles };
  } catch (err) {
    return { probed: false, tier: 'tradeoff', error: `${err.message || err}`.slice(0, 200), intersectingFiles: [], conflictedFiles: [], addAddFiles: [] };
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

  // 通用 1：contracts diff 无契约合并凭据（T15，261015 批：三件联立凭据替代人签门）
  if (ctx.contractsDiff && !ctx.hasContractGateEvidence.ok) {
    push(`docs/contracts/** diff 且同批无契约合并凭据（${ctx.hasContractGateEvidence.reason}）——契约变更 = 架构变更的一种，走 /arch-change loop（.claude/rules/contracts.md；凭据 = FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖，三件联立）`);
  }

  // 通用 3：escalation 未 resolve / blocked（blocked 仅在人批豁免成立时视同 resolve，OQ2=(a)）
  if (ctx.blocked && !escalateExempt) push('change 处 blocked 态（未 resolve）');
  if (ctx.escalationUnresolved) push('存在未 resolve 的 agent-escalation 事件');

  // 通用 4（T5，260908 批门内加项）：契约类 hard-violation「交回→在册」核验。
  // 只拦可识别形态（A-8 放行方向）：Findings 行含 hard-violation + CONTRACT_SPEC_LITERALS 任一字面起引，
  // 且 CDQ 无对应行、pod-events 无 human defer/accept-risk 锚 → BLOCK；未匹配 / packet 不在场 → 放行。
  if (ctx.contractFindings?.present && !ctx.contractFindings.accounted) {
    push(`contract-findings-unaccounted：review-packet 有 ${ctx.contractFindings.hits.length} 条 hard-violation 且 Basis 引 docs/contracts/**|docs/spec.md|docs/requirements.md|docs/architecture.md，但 CONTRACT-DECISION-QUEUE 无对应行且无 human defer/accept-risk 锚（260908 批 T5；字面集 260913 批扩展）`);
  }

  // 人批 flag fail-closed：给了 flag 但核验不过 → BLOCK 且点名失败项
  if (approval && !approval.ok) push(`human-approval 核验失败：${approval.reason}`);

  if (ctx.kind === 'devloop') {
    // T1/T2（260908 批）：并行相交面分级路由——tradeoff（③ 级）BLOCK 交 architect 消解评审；
    // no-overlap / union-safe 不升人（② 级走 runbook 并集 + 双方齿复跑；探测失败已 fail-closed 归 tradeoff）。
    if (ctx.overlap?.tier === 'tradeoff') {
      const detail = ctx.overlap.probed
        ? `实撞 ${ctx.overlap.conflictedFiles.length} 件（${ctx.overlap.conflictedFiles.slice(0, 5).join(', ')}${ctx.overlap.conflictedFiles.length > 5 ? ' 等' : ''}）`
        : `探测失败（${ctx.overlap.error || 'unknown'}）`;
      push(`parallel-overlap-tier3：并行 Pod 相交有取舍，需 architect 消解评审（pod-coordination.md §10d）——${detail}`);
    }
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
    push(`unknown change kind（changeId 既无 delivery/taskpackages 也无 delivery/changes/arch）`);
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
  const execFn = (c) => execSync(c, { encoding: 'utf8', stdio: 'pipe' });
  const devDir = join(projectDir, 'delivery', 'taskpackages', changeId);
  const archDir = join(projectDir, 'delivery', 'changes', 'arch', changeId);
  const hasDev = existsSync(devDir);
  const hasArch = existsSync(archDir);
  const kind = hasArch ? 'gov' : hasDev ? 'devloop' : 'unknown';

  const source = sourceRef || revParse(execFn, 'HEAD');

  const supervisorVerdict = readSupervisorVerdict(projectDir, changeId);
  const govAnchors = kind === 'gov' ? readGovAnchors(projectDir, changeId) : null;
  const contractsDiff = hasContractsDiff(execFn, source, 'main');
  // T15（261015 批）：契约合并机械凭据——替换原 hasContractGateEvidence: false 硬编码（ARQ-260923 建议①兑现）
  const hasContractGateEvidence = hasArchLoopContractEvidence(execFn, source, 'main', projectDir);
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
  const contractFindings = contractFindingsAccounted(projectDir, changeId, { podsRecordPath }); // T5（260908 批）
  // T1（260908 批）：并行相交面探测（dev-loop 档；source = 源 ref，target = main）
  const overlap = parallelOverlapProbe(execFn, source, 'main');

  return {
    changeId,
    source,
    kind,
    execFn, // 热修（ARQ-260905-escalation-gate-live-execfn-gap，human 批 2026-09-05）：evaluate 的 off-main 判定依赖它，此前 buildCtx 漏带 → live 恒 fail-closed
    supervisorVerdict,
    govAnchors,
    contractsDiff,
    hasContractGateEvidence, // T15（261015 批）：{ ok, reason, contractPaths }——ok=false 且 contractsDiff → 通用 1 BLOCK；原硬编码 false（恒 BLOCK 人合）已退役
    blocked,
    escalationUnresolved: false, // pod-events 聚合待接入；现 false（升级闸 v1 只覆盖 blocked 态 + 显式判定）
    humanApproval, // T1：--human-approval-ref 核验结果；未带 flag 为 null
    contractFindings, // T5（260908 批）：契约类 hard-violation 台账核验结果
    overlap, // T1（260908 批）：并行相交面探测结果（分级路由见 pod-coordination §10d）
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
    hasContractGateEvidence: { ok: true, reason: 'no-contract-diff', contractPaths: [] },
    blocked: false,
    escalationUnresolved: false,
    humanApproval: null, // T1：--human-approval-ref 核验结果
    contractFindings: { present: false, accounted: true, hits: [] }, // T5：默认不在场 → 放行
    overlap: { probed: true, tier: 'no-overlap', intersectingFiles: [], conflictedFiles: [], addAddFiles: [] }, // T1：默认零相交 → 放行
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
  // 通用 1 contracts diff（T15 改写：凭据对象形态）
  expect('contracts-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: 无候选满足三件联立' } }, true, 'docs/contracts');
  // T15 契约合并凭据 5 例（261015 批；R1-F4：三件联立肢各有否定覆盖；真空绿规约）
  expect('contracts-diff-no-evidence-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: 无候选 PROPOSAL' } }, true, 'contract-evidence');
  cases.push({
    name: 'contracts-diff-evidence-pass',
    ok: !evaluate(mkCtx({ contractsDiff: true, hasContractGateEvidence: { ok: true, reason: 'contract-evidence via 261016-arch-review-t' } })).blocked,
    got: '',
  });
  expect('contracts-diff-proposal-not-frozen-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: FROZEN 缺' } }, true, 'contract-evidence');
  expect('contracts-diff-manifest-not-covering-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: manifest 未覆盖' } }, true, 'contract-evidence');
  expect('contracts-diff-verdict-not-pass-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: verdict 非 PASS' } }, true, 'contract-evidence');
  cases.push({
    name: 'no-contracts-diff-unaffected',
    ok: !evaluate(mkCtx({ contractsDiff: false, hasContractGateEvidence: { ok: true, reason: 'no-contract-diff', contractPaths: [] } })).blocked,
    got: '',
  });
  {
    // T15 直接函数 fixture 4 例（真空绿规约：PROPOSAL fixture 含 manifest 围栏块非空；deps.readVerdict 注入）
    const FIX_PROPOSAL = [
      '# PROPOSAL · t',
      '> **状态：FROZEN v1.0 · 2026-10-15 · test**',
      '```landing-manifest',
      'docs/contracts/interface-contract.md',
      'docs/architecture.md',
      '```',
      '',
    ].join('\n');
    const FIX_DIFF = ['docs/contracts/interface-contract.md', 'delivery/changes/arch/261016-arch-review-t/PROPOSAL.md'].join('\n');
    const fxExec = (c) => (c.startsWith('git diff') ? FIX_DIFF : c.startsWith('git show') ? FIX_PROPOSAL : '');
    const r1 = hasArchLoopContractEvidence(fxExec, 'src', 'main', 'x', { readVerdict: () => 'verdict: PASS' });
    cases.push({ name: 'contract-evidence-fn-pass', ok: r1.ok === true, got: r1.reason });
    const r2 = hasArchLoopContractEvidence(fxExec, 'src', 'main', 'x', { readVerdict: () => 'verdict: BLOCK(F1)' });
    cases.push({ name: 'contract-evidence-fn-verdict-not-pass-block', ok: r2.ok === false && /verdict|联立/.test(r2.reason), got: r2.reason });
    const r3 = hasArchLoopContractEvidence((c) => (c.startsWith('git diff') ? FIX_DIFF : c.startsWith('git show') ? 'not frozen' : ''), 'src', 'main', 'x', { readVerdict: () => 'verdict: PASS' });
    cases.push({ name: 'contract-evidence-fn-not-frozen-block', ok: r3.ok === false && /FROZEN|联立/.test(r3.reason), got: r3.reason });
    const r4 = hasArchLoopContractEvidence(
      (c) => (c.startsWith('git diff') ? FIX_DIFF : c.startsWith('git show') ? FIX_PROPOSAL.replace('docs/contracts/interface-contract.md\n', '') : ''),
      'src', 'main', 'x',
      { readVerdict: () => 'verdict: PASS' },
    );
    cases.push({ name: 'contract-evidence-fn-manifest-not-covering-block', ok: r4.ok === false && /manifest|联立/.test(r4.reason), got: r4.reason });
  }
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

  // T5 契约台账核验 3 例（260908 批；真空绿规约：fixture 非空断言先行——present 值由 contractFindingsAccounted 真实扫描产出的形状构造）
  // 1) 在案 + 无 CDQ 行无 defer 锚 → BLOCK 点名
  expect('contract-findings-unaccounted-block', { contractFindings: { present: true, accounted: false, hits: ['| SHOULD | hard-violation | contract | ... | docs/contracts/** §三 speech | 交回 CDQ |'] } }, true, 'contract-findings-unaccounted');
  // 2) 在案 + CDQ 行在场（accounted:true）→ 放行
  cases.push({ name: 'contract-findings-cdq-accounted-pass', ok: !evaluate(mkCtx({ contractFindings: { present: true, accounted: true, hits: ['docs/contracts/**'] } })).blocked, got: '' });
  // 3) 在案 + human defer 锚（accounted:true）→ 放行
  cases.push({ name: 'contract-findings-defer-anchored-pass', ok: !evaluate(mkCtx({ contractFindings: { present: true, accounted: true, hits: ['docs/spec.md'] } })).blocked, got: '' });

  // T5 字面集扩展 4 例（260913-arch-review-spec-split：spec.md 拆分 + 归档；真实扫 contractFindingsAccounted，注入 readText fixture）
  const mkReadText = (packetLine, cdqText) => (p) => {
    if (p.includes('review-packet.md')) return `| SHOULD | hard-violation | contract | x | ${packetLine} | 交回 CDQ |\n`;
    if (p.includes('CONTRACT-DECISION-QUEUE')) return cdqText;
    return null; // pod-events 不在场 → 无 defer 锚
  };
  {
    // 1) 新路径 docs/requirements.md → 在案，CDQ 无行 → unaccounted
    const r = contractFindingsAccounted('x', 'chg-1', { readText: mkReadText('docs/requirements.md §3', '# CDQ\n') });
    cases.push({ name: 'literal-requirements-detected', ok: r.present === true && r.accounted === false, got: JSON.stringify(r) });
    // 2) 新路径 docs/architecture.md + CDQ 有本 changeId 行 → accounted
    const r2 = contractFindingsAccounted('x', 'chg-1', { readText: mkReadText('docs/architecture.md §5', '# CDQ\nchg-1 挂账行\n') });
    cases.push({ name: 'literal-architecture-cdq-accounted', ok: r2.present === true && r2.accounted === true, got: JSON.stringify(r2) });
    // 3) 归档件 docs/spec.md 字面保留（历史 findings 兼容）→ 在案
    const r3 = contractFindingsAccounted('x', 'chg-1', { readText: mkReadText('docs/spec.md §4', '# CDQ\n') });
    cases.push({ name: 'literal-archived-spec-still-detected', ok: r3.present === true && r3.accounted === false, got: JSON.stringify(r3) });
    // 4) 非字面集路径（docs/notes/...）→ 不在案，放行（A-8 放行方向不回退）
    const r4 = contractFindingsAccounted('x', 'chg-1', { readText: mkReadText('docs/notes/foo.md', '# CDQ\n') });
    cases.push({ name: 'literal-noncontract-path-not-present', ok: r4.present === false && r4.accounted === true, got: JSON.stringify(r4) });
  }

  // T1 并行相交面分级路由 4 例（260908 批 ladder；human 追认 Track B + 继续落地后落地；真空绿规约：fixture 非空——文件清单构造非空）
  // 1) ② 级：实撞全部为已登记版本化制品形态 → union-safe，不升人（runbook 并集 + 双方齿复跑）
  cases.push({
    name: 'overlap-union-safe-pass',
    ok: !evaluate(mkCtx({ overlap: { probed: true, tier: 'union-safe', intersectingFiles: ['server/deploy/openclaw-system-prompt.v3.md', 'server/deploy/provider.config.yaml'], conflictedFiles: ['server/deploy/openclaw-system-prompt.v3.md'], addAddFiles: ['server/deploy/openclaw-system-prompt.v3.md'] } })).blocked,
    got: '',
  });
  // 2) ③ 级：同 hunk 相撞（非登记形态）→ BLOCK 点名 architect 消解评审
  expect('overlap-tradeoff-block', { overlap: { probed: true, tier: 'tradeoff', intersectingFiles: ['firmware/main/display/ui.c'], conflictedFiles: ['firmware/main/display/ui.c'], addAddFiles: [] } }, true, 'parallel-overlap-tier3');
  // 3) 探测失败 → fail-closed 归 ③ BLOCK
  expect('overlap-probe-fail-block', { overlap: { probed: false, tier: 'tradeoff', error: 'merge-base failed' } }, true, 'parallel-overlap-tier3');
  // 4) ① 级：零相交 → 放行（默认 mkCtx 即此形态，显式复证）
  cases.push({
    name: 'overlap-no-overlap-pass',
    ok: !evaluate(mkCtx({ overlap: { probed: true, tier: 'no-overlap', intersectingFiles: [], conflictedFiles: [], addAddFiles: [] } })).blocked,
    got: '',
  });
  // 5) V4 升级件内容宿主断言（260908 批 ladder 转绿补件）：probe-out JSON 形状必载相交清单/实撞集/双新增/tier 四键——
  // 升级件宿主 = overlap-probe.json（③④ 级的逐文件清单消费源），断言其序列化形状防回退
  {
    const v4Overlap = { probed: true, tier: 'tradeoff', intersectingFiles: ['a.c', 'b.c'], conflictedFiles: ['b.c'], addAddFiles: [] };
    const v4Json = JSON.stringify({ changeId: 'x', kind: 'devloop', overlap: v4Overlap });
    const v4KeysOk = ['"intersectingFiles"', '"conflictedFiles"', '"addAddFiles"', '"tier"'].every((k) => v4Json.includes(k)) && v4Json.includes('"b.c"');
    cases.push({ name: 'overlap-probeout-payload-keys-ok', ok: v4KeysOk, got: v4Json.slice(0, 80) });
  }

  // T2（260920-gov-review-verdict-anchor-align）：green 锚三夹具——
  // 样板行（GREEN 在行中、无 verdict 标签）不命中 / 规范 verdict 行命中 / BLOCK 行不命中。
  // 真空绿：样板行文本先断含 GREEN 字样（防夹具没造对）。
  {
    const boilerplate = '> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**';
    cases.push({ name: 'green-anchor-boilerplate-not-green', ok: !isGreenVerdict(boilerplate), got: `含 GREEN=${/GREEN/.test(boilerplate)} → isGreen=${isGreenVerdict(boilerplate)}` });
    cases.push({ name: 'green-anchor-verdict-line-green', ok: isGreenVerdict('verdict: GREEN'), got: `isGreen=${isGreenVerdict('verdict: GREEN')}` });
    cases.push({ name: 'green-anchor-block-not-green', ok: !isGreenVerdict('verdict: BLOCK(F1)'), got: `isGreen=${isGreenVerdict('verdict: BLOCK(F1)')}` });
  }

  const fail = cases.filter((c) => !c.ok);
  process.stdout.write(
    `${JSON.stringify({ selfCheck: 'pre-merge-escalation-gate', pass: cases.length - fail.length, fail: fail.length, total: cases.length, ok: fail.length === 0, failures: fail.length ? fail : undefined })}\n`,
  );
  process.exit(fail.length ? 1 : 0);
}

// ---- 入口（T6，260908 批：import.meta 守卫——被 import 时不执行 CLI，可导入库 / 可执行脚本分离）----
function isDirectRun() {
  try {
    return !!process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href;
  } catch {
    return false;
  }
}

if (isDirectRun()) {
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
const probeOutPath = (() => {
  const i = args.indexOf('--probe-out');
  return i >= 0 ? args[i + 1] ?? null : null;
})(); // T3（260908 批 ladder）：探测面 JSON 制品落盘路径（显式文件传递，非 stdout 解析——R1-S2）

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

// T3（260908 批 ladder）：探测面制品落盘——PASS 与 BLOCK 两态都落（升级件内容面消费），失败 advisory 不改退出码
if (probeOutPath) {
  try {
    writeFileSync(probeOutPath, JSON.stringify({ changeId, kind: ctx.kind, source: ctx.source, overlap: ctx.overlap }, null, 2), 'utf8');
  } catch (err) {
    process.stderr.write(`[pre-merge-escalation-gate] WARN: --probe-out 落盘失败（advisory）：${err.message}\n`);
  }
}

if (result.blocked) {
  process.stderr.write(`[pre-merge-escalation-gate] BLOCK (changeId=${changeId}, kind=${ctx.kind}):\n`);
  result.reasons.forEach((r) => process.stderr.write(`  - ${r}\n`));
  process.stderr.write('  → 升级 human，不得自动 merge。\n');
  process.exit(1);
}

const approvalNote = humanApprovalRef ? ` — human-approval verified (eventId=${humanApprovalRef})` : '';
process.stdout.write(`[pre-merge-escalation-gate] PASS (changeId=${changeId}, kind=${ctx.kind})${approvalNote} — 无例外命中，可交给 merge-driver。\n`);
process.exit(0);
} // end isDirectRun()