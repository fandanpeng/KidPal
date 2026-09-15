#!/usr/bin/env node
// 每 Pod loop-health 机器信号提取器（per-change，非跨 Pod 聚合器）：
//   从单个 delivery/changes/dev/<change-id>/ 的 loop 过程交付物（taskpackage / test-evidence /
//   review-packet / supervisor-verdict）+ session-digest.md 抽确定性信号，
//   供 integrator 写该 Pod 自己的 delivery/changes/dev/<change-id>/loop-health.md（Pod 自总结）。
//   跨 Pod 复盘（提取共性）在 .claude/commands/loop-retro.md，读各 Pod 的 loop-health.md。
//
// 关键纪律（见 .claude/references/dev-loop-operating-system.md §5–§6）：
//   - 层级：loop 过程交付物 + session-digest → 每 Pod loop-health → retro 跨 Pod 提共性。
//     digest 是 loop-health 的输入，不与 loop-health 平级。
//   - 事实源仍是 machine artifacts + session-digest；本脚本只抽信号、不复制正文、不覆盖它们。
//   - 缺 session-digest → 该 Pod loop-health 只能 partial/advisory（缺“对话/纠偏”维度）。
//
// 设计：无外部依赖、幂等、可独立运行。
//   node .claude/scripts/loop-health-report.mjs <change-id>   # 抽单个 Pod 的信号（给 integrator 写 loop-health.md）
//   node .claude/scripts/loop-health-report.mjs --all         # 便捷扫描全部（给 retro 起步；非“唯一 loop-health”）
//   node .claude/scripts/loop-health-report.mjs --pending     # retro 游标：算出「未被任何 retro 分析过」的 Pod（+ 待补 loop-health 的）
//   node .claude/scripts/loop-health-report.mjs --record-retro <retroId> [--covered a,b] [--note "..."]
//                                                             # retro 收口：把本轮已分析的 change-id 追加进 .claude/loop-retro/ledger.jsonl（游标推进）
//   node .claude/scripts/loop-health-report.mjs --writeoff <id1,id2,...> --reason "HUMAN-APPROVED: ..."
//                                                             # 销账（human-approved 独立手动）：把结构性永不析出 Pod 追加进 ledger excludedChangeIds（游标第三态，源桶排除移出触发计数）。不接 hook/auto-run/retro 例行

import { appendFileSync, existsSync, mkdirSync, readFileSync, readdirSync } from 'node:fs';
import { resolve, join } from 'node:path';
import { EVENTS_SCHEMA_CUTOFF } from './pod-events-append.mjs';
import { CHANGE_ID_PREFIX_RE } from './lib/changeid-shared.mjs'; // T1 单源收敛（260906-gov-review-arq-governance-batch），原 _LHR 别名消除

// R5 (gov-review-devloop-evidence-led, FROZEN 2026-07-24):
//   - C4 retired test-evidence.md, final-summary.md (replaced by verdict.json + verification.json machine evidence)
//   - taskpackage.json stays (live file is authoritative; delivery/changes/dev/ snapshot retired)
//   - verdict.json + verification.json added as machine evidence artifacts

const STANDARD_ARTIFACTS = [
  'taskpackage.json',
  'docs-delta.md',
  'review-packet.md',
  'session-digest.md',
];

function readJson(path) {
  try {
    return JSON.parse(readFileSync(path, 'utf8'));
  } catch {
    return null;
  }
}

function readText(path) {
  try {
    return readFileSync(path, 'utf8');
  } catch {
    return null;
  }
}

function listChangeDirs(devDir) {
  if (!existsSync(devDir)) return [];
  return readdirSync(devDir, { withFileTypes: true })
    .filter((e) => e.isDirectory())
    .map((e) => e.name);
}

// 轻量信号：digest 里是否含纠偏 / 未决关键词（advisory，不做 NLP）。
function digestSignals(text) {
  if (!text) return null;
  const has = (re) => re.test(text);
  return {
    hasHumanCorrections: has(/human\s*corrections?|纠偏|更正|修正方向/i),
    hasUnresolved: has(/unresolved|未决|遗留|待办/i),
    hasCrossDevice: has(/cross[- ]?device|跨设备|跨机器/i),
    hasFollowUp: has(/follow[- ]?up|后续工作|followup/i),
  };
}

// 单个 Pod 的 loop-health 机器信号（per-change）。integrator 据此写该 Pod 的 loop-health.md。
export function buildChangeHealth(projectDir, changeId) {
  const dir = resolve(projectDir, 'delivery', 'changes', 'dev', changeId);
  const present = {};
  const missing = [];
  for (const a of STANDARD_ARTIFACTS) {
    const ok = existsSync(join(dir, a));
    present[a] = ok;
    if (!ok) missing.push(a);
  }

  const tp = readJson(join(dir, 'taskpackage.json'));
  const riskTier = tp && tp.riskTier ? tp.riskTier : 'unknown';

  const verdictObj = readJson(join(dir, 'supervisor-verdict.json'));
  const verdict =
    verdictObj && typeof verdictObj.verdict === 'string'
      ? verdictObj.verdict.toUpperCase()
      : 'unknown';

  const digestText = readText(join(dir, 'session-digest.md'));
  const digestPresent = Boolean(digestText);
  // ── pod-journal.jsonl advisory read（层 A 叙事车道，不参与 gating，DP-FD8）──
  const journalText = readText(join(dir, 'pod-journal.jsonl'));
  const journalEvents = journalText
    ? journalText.split(/\r?\n/).filter((l) => l.trim()).map((l) => {
        try { return JSON.parse(l); } catch { return null; }
      }).filter(Boolean)
    : [];
  const journalPresent = journalEvents.length > 0;

  // 缺 digest 或缺标准制品 → 该 Pod loop-health 只能 advisory
  const partial = !digestPresent || missing.length > 0;

  return {
    changeId,
    exists: existsSync(dir),
    riskTier,
    verdict,
    blockedOrEscalated: verdict === 'BLOCK' || verdict === 'ESCALATE',
    present,
    missingArtifacts: missing,
    digestPresent,
    digest: digestText ? digestSignals(digestText) : null,
    partial,
    partialReason: partial
      ? 'missing session-digest and/or standard artifacts on this change; loop-health is advisory'
      : null,
    note: 'per-Pod machine signals; integrator folds into delivery/changes/dev/<id>/loop-health.md. Not a source of truth. See dev-loop-operating-system.md §5.',
  };
}

// 便捷全量扫描：为 retro 起步用；权威的每 Pod loop-health 是各 delivery/changes/dev/<id>/loop-health.md（叙述+信号）。
export function scanAllChanges(projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd()) {
  const devDir = resolve(projectDir, 'delivery', 'changes', 'dev');
  const changes = listChangeDirs(devDir).map((id) => buildChangeHealth(projectDir, id));
  return {
    generatedAt: new Date().toISOString(),
    scannedBy:
      'loop-health-report.mjs --all (convenience scan for retro; per-Pod loop-health.md is authoritative)',
    changesScanned: changes.length,
    changes,
  };
}

// ---- retro 游标（已分析 / 未分析）：.claude/loop-retro/ledger.jsonl（append-only，随 main 版本控制）----
// 每个已完成的 retro 追加一行 { retroId, ranAt, coveredChangeIds[], advisoryChangeIds[], note }。
// 销账行（human-approved）追加 { retroId:"writeoff-...", excludedChangeIds[], note }（游标第三态）。
// 「已分析」= 所有 ledger 行 coveredChangeIds 的并集；「待分析」= 有 loop-health.md 但不在该并集里。
// 「销账/excluded」= 所有行 excludedChangeIds 的并集——源桶排除、移出触发计数（不算已分析亦不计 waiting）。

function retroLedgerPath(projectDir) {
  return resolve(projectDir, '.claude', 'loop-retro', 'ledger.jsonl');
}

export function readRetroLedger(projectDir) {
  const text = readText(retroLedgerPath(projectDir));
  if (!text) return [];
  return text
    .split(/\r?\n/)
    .filter((l) => l.trim())
    .map((l) => {
      try {
        return JSON.parse(l);
      } catch {
        return null;
      }
    })
    .filter(Boolean);
}

function isPostEventsCutoff(changeId) {
  const m = CHANGE_ID_PREFIX_RE.exec(String(changeId));
  if (!m) return false;
  return m[1] >= EVENTS_SCHEMA_CUTOFF.replace(/-/g, '').slice(2);
}

// 算出 retro 本轮范围：谁已分析、谁待分析、谁还缺 loop-health.md（无法分析）。
// F4 (exclusive dual-format cursor):
//   Post-cutoff + pod-events.jsonl exists → new-format (pending analysis via pod-events)
//   Else loop-health.md exists → old-format (pending analysis)
//   Else → waiting
export function computeRetroScope(projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd()) {
  const devDir = resolve(projectDir, 'delivery', 'changes', 'dev');
  const tpRoot = resolve(projectDir, 'delivery', 'taskpackages');
  const ledger = readRetroLedger(projectDir);
  const covered = new Set(
    ledger.flatMap((r) => (Array.isArray(r.coveredChangeIds) ? r.coveredChangeIds : [])),
  );
  // 第三态（gov-review-retro-waiting-writeoff，FROZEN 2026-08-01）：human-approved 销账的结构性永不析出 Pod。
  //   F5 守卫对偶 coveredChangeIds（writeoff 行无 coveredChangeIds 时安全贡献 []）。
  const excluded = new Set(
    ledger.flatMap((r) => (Array.isArray(r.excludedChangeIds) ? r.excludedChangeIds : [])),
  );
  const pendingAnalysis = [];
  const waitingOnLoopHealth = [];
  const excludedFromTrigger = [];
  for (const id of listChangeDirs(devDir)) {
    // 源桶排除：excluded Pod 在分桶源头跳过，根本不进 pendingAnalysis/waitingOnLoopHealth
    //   → 3 消费方（closeout-check / retro-staleness-check / merge-driver）单源自动传导，零改动（F4）。
    if (excluded.has(id)) { excludedFromTrigger.push(id); continue; } // human-approved 销账（游标第三态）
    if (covered.has(id)) continue; // 已被某轮 retro 分析过

    // F4 exclusive dispatch: new-format takes priority for post-cutoff Pods
    if (isPostEventsCutoff(id) && existsSync(join(tpRoot, id, 'pod-events.jsonl'))) {
      const podEventsPath = join(tpRoot, id, 'pod-events.jsonl');
      if (existsSync(podEventsPath)) pendingAnalysis.push(id); // new-format Pod, ready for analysis
    } else if (existsSync(join(devDir, id, 'loop-health.md'))) {
      // F4 fallback: old-format evidence remains analyzable even for a post-cutoff id.
      pendingAnalysis.push(id);
    } else {
      waitingOnLoopHealth.push(id);
    }
  }
  return {
    generatedAt: new Date().toISOString(),
    retroRuns: ledger.length,
    alreadyAnalyzedCount: covered.size,
    pendingAnalysis, // 本轮要分析（有 loop-health.md 或 pod-events.jsonl、未分析过、未销账）
    waitingOnLoopHealth, // 待 integrator 补 loop-health.md / planner 产 pod-events.jsonl 后才能进 retro（未销账）
    excludedFromTrigger, // human-approved 销账的永不析出 Pod（单独成桶报绝对规模，fail-safe 不隐藏）
    note: 'cursor = union of ledger coveredChangeIds; third state = union of excludedChangeIds (human-approved writeoff, source-bucket exclusion). F4: post-cutoff uses pod-events.jsonl; pre-cutoff uses loop-health.md. See dev-loop-operating-system.md §6.',
  };
}

// retro 收口：把本轮已分析的 change-id 追加进 ledger（游标推进）。默认 covered = 当前 pendingAnalysis。
export function recordRetro(projectDir, { retroId, covered, advisory, note } = {}) {
  if (!retroId) throw new Error('recordRetro: retroId required');
  const scope = computeRetroScope(projectDir);
  const rec = {
    retroId,
    ranAt: new Date().toISOString(),
    coveredChangeIds: Array.isArray(covered) ? covered : scope.pendingAnalysis,
    advisoryChangeIds: Array.isArray(advisory) ? advisory : scope.waitingOnLoopHealth,
    note: note || null,
  };
  mkdirSync(resolve(projectDir, '.claude', 'loop-retro'), { recursive: true });
  appendFileSync(retroLedgerPath(projectDir), `${JSON.stringify(rec)}\n`);
  return rec;
}

// 销账（human-approved，gov-review-retro-waiting-writeoff FROZEN 2026-08-01）：把结构性永不析出 Pod 的
//   change-id 追加进 ledger 的 excludedChangeIds（游标第三态），computeRetroScope 源桶排除、移出触发计数。
//   **独立手动机制（F3，对偶 --record-retro）**：不接任何 hook / auto-run / loop-retro 例行步骤，脚本不自调
//   自裁；writer = 脚本机制（主线程调用）/ approver = human。note 为自证 convention 字符串（对偶 amnesty，
//   非机器校验）；销账资格 = human-only 裁决（OQ2）。撤销走 git revert（OQ1 不可逆并集）。
export function recordWriteoff(projectDir, { ids, reason } = {}) {
  if (!Array.isArray(ids) || ids.length === 0) {
    throw new Error('recordWriteoff: ids (non-empty array) required');
  }
  const rec = {
    retroId: `writeoff-${Date.now()}`,
    excludedChangeIds: ids,
    note: `HUMAN-APPROVED: ${reason || 'human-approved writeoff of structurally never-analyzable Pod(s)'}`,
  };
  mkdirSync(resolve(projectDir, '.claude', 'loop-retro'), { recursive: true });
  appendFileSync(retroLedgerPath(projectDir), `${JSON.stringify(rec)}\n`);
  return rec;
}

// ---- CLI ----
function isMain() {
  const invoked = process.argv[1] ? resolve(process.argv[1]) : '';
  return invoked && invoked.endsWith('loop-health-report.mjs');
}

if (isMain()) {
  const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
  const argv = process.argv.slice(2);
  const flagVal = (name) => {
    const i = argv.indexOf(name);
    return i >= 0 ? argv[i + 1] : undefined;
  };
  if (argv.includes('--pending')) {
    const scope = computeRetroScope(projectDir);
    process.stdout.write(`${JSON.stringify(scope, null, 2)}\n`);
    process.stderr.write(
      `[loop-health] --pending: ${scope.pendingAnalysis.length} to analyze, ` +
        `${scope.waitingOnLoopHealth.length} waiting on loop-health.md, ` +
        `${scope.excludedFromTrigger.length} excluded (human-approved writeoff), ` +
        `${scope.alreadyAnalyzedCount} already analyzed across ${scope.retroRuns} retro run(s).\n`,
    );
  } else if (argv.includes('--record-retro')) {
    const retroId = flagVal('--record-retro');
    if (!retroId || retroId.startsWith('--')) {
      process.stderr.write(
        'usage: loop-health-report.mjs --record-retro <retroId> [--covered a,b] [--advisory c,d] [--note "..."]\n',
      );
      process.exit(2);
    }
    const splitCsv = (v) => (v ? v.split(',').map((s) => s.trim()).filter(Boolean) : undefined);
    const rec = recordRetro(projectDir, {
      retroId,
      covered: splitCsv(flagVal('--covered')),
      advisory: splitCsv(flagVal('--advisory')),
      note: flagVal('--note'),
    });
    process.stdout.write(`${JSON.stringify(rec, null, 2)}\n`);
    process.stderr.write(
      `[loop-health] recorded retro ${rec.retroId}: covered ${rec.coveredChangeIds.length}, advisory ${rec.advisoryChangeIds.length} → .claude/loop-retro/ledger.jsonl\n`,
    );
  } else if (argv.includes('--writeoff')) {
    // 销账写入（human-approved 独立手动机制，对偶 --record-retro；不接 hook/auto-run/retro 例行，F3）。
    const splitCsv = (v) => (v ? v.split(',').map((s) => s.trim()).filter(Boolean) : undefined);
    const ids = splitCsv(flagVal('--writeoff'));
    const reason = flagVal('--reason');
    if (!ids || ids.length === 0) {
      process.stderr.write(
        'usage: loop-health-report.mjs --writeoff <id1,id2,...> --reason "HUMAN-APPROVED: ..."\n' +
          '  independent manual human-approved writeoff; NOT wired to any hook/auto-run/retro step.\n',
      );
      process.exit(2);
    }
    const rec = recordWriteoff(projectDir, { ids, reason });
    process.stdout.write(`${JSON.stringify(rec, null, 2)}\n`);
    process.stderr.write(
      `[loop-health] writeoff ${rec.retroId}: excluded ${rec.excludedChangeIds.length} change-id(s) → .claude/loop-retro/ledger.jsonl (human-approved; revert via git revert)\n`,
    );
  } else if (argv.includes('--all')) {
    const scan = scanAllChanges(projectDir);
    const advisory = scan.changes.filter((c) => c.partial).length;
    process.stdout.write(`${JSON.stringify(scan, null, 2)}\n`);
    process.stderr.write(
      `[loop-health] --all: ${scan.changesScanned} changes scanned` +
        (advisory ? `, ${advisory} advisory (missing digest/artifacts)` : '') +
        '. Per-Pod loop-health.md is authoritative; retro reads those.\n',
    );
  } else {
    const changeId = argv.find((a) => !a.startsWith('--'));
    if (!changeId) {
      process.stderr.write(
        'usage: loop-health-report.mjs <change-id>   (or --all for a convenience scan)\n',
      );
      process.exit(2);
    }
    const health = buildChangeHealth(projectDir, changeId);
    process.stdout.write(`${JSON.stringify(health, null, 2)}\n`);
    process.stderr.write(
      `[loop-health] ${changeId}: verdict=${health.verdict}, partial=${health.partial}` +
        (health.missingArtifacts.length ? `, missing=${health.missingArtifacts.join(',')}` : '') +
        `. integrator: fold into delivery/changes/dev/${changeId}/loop-health.md\n`,
    );
  }
}
