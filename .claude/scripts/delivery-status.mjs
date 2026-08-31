#!/usr/bin/env node
// 从 .delivery/milestones.jsonl（里程碑基线，镜像 core.Milestone + delivery.MilestoneDeliveryState）
// 计算完工比例并生成 .delivery/STATUS.md。
// 单一事实源 = milestones.jsonl；STATUS.md 是派生只读视图（勿手改）。
// 无外部依赖；可独立运行：node .claude/scripts/delivery-status.mjs
import { readFileSync, writeFileSync, existsSync } from 'node:fs';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

// state 阶段权重（在途预计完工度；只读派生、非计费、不归个人）
const STAGE_WEIGHT = {
  planning: 0,
  change_needed: 0.2,
  blocked: 0.2,
  in_delivery: 0.4,
  qa: 0.7,
  ready_for_acceptance: 0.9,
  accepted: 1,
  invoicing: 1,
  invoiced: 1,
  closed: 1,
};
// 「已验收」= accepted 及以后（internal 项目 accepted→closed，无开票）
const ACCEPTED_STATES = new Set(['accepted', 'invoicing', 'invoiced', 'closed']);

function parseMilestones(text) {
  const out = [];
  for (const line of text.split(/\r?\n/)) {
    const s = line.trim();
    if (!s || s.startsWith('//')) continue;
    out.push(JSON.parse(s));
  }
  return out;
}

function pct(n) {
  return `${Math.round(n * 1000) / 10}%`;
}

function renderMd({ milestones, total, acceptedCount, acceptedPct, inflightPct }) {
  const byPhase = new Map();
  for (const m of milestones) {
    if (!byPhase.has(m.phase)) byPhase.set(m.phase, []);
    byPhase.get(m.phase).push(m);
  }
  const lines = [];
  lines.push('# 交付完工度 · STATUS（自动生成，勿手改）');
  lines.push('');
  lines.push('> 由 [.claude/scripts/delivery-status.mjs](../.claude/scripts/delivery-status.mjs) 从 [milestones.jsonl](milestones.jsonl) 生成。');
  lines.push('> 单一事实源是 `milestones.jsonl`（里程碑基线，镜像 `core.Milestone` + `delivery.MilestoneDeliveryState`）；改状态去改它再重跑：`node .claude/scripts/delivery-status.mjs`。');
  lines.push('> 里程碑度量**交付（实现验收）**、非设计；完工比例只读派生、不手填、不数 `.planning/` todo。');
  lines.push('');
  lines.push('## 完工比例');
  lines.push('');
  lines.push(`- **已验收**：${pct(acceptedPct)}（${acceptedCount} / ${total} 里程碑 accepted 及以后）`);
  lines.push(`- **在途预计**：${pct(inflightPct)}（按 state 阶段加权：planning 0 · in_delivery .4 · qa .7 · ready .9 · accepted 1）`);
  lines.push('');
  lines.push('## 里程碑基线');
  lines.push('');
  for (const ph of [...byPhase.keys()].sort()) {
    lines.push(`### ${ph}`);
    lines.push('');
    lines.push('| 里程碑 | 状态 | 健康 | 下一步 | 验收依据 |');
    lines.push('|---|---|---|---|---|');
    for (const m of byPhase.get(ph)) {
      const ref = m.acceptanceRef ? `[ref](../${m.acceptanceRef})` : '—';
      lines.push(`| ${m.name} | \`${m.state}\` | ${m.health || '—'} | ${m.nextAction || '—'} | ${ref} |`);
    }
    lines.push('');
  }
  return lines.join('\n') + '\n';
}

export function statusSnapshot(projectDir) {
  const jsonlPath = join(projectDir, '.delivery', 'milestones.jsonl');
  if (!existsSync(jsonlPath)) {
    throw new Error(`milestones.jsonl not found at ${jsonlPath}`);
  }
  const milestones = parseMilestones(readFileSync(jsonlPath, 'utf8'));
  const total = milestones.length;
  let acceptedCount = 0;
  let weightSum = 0;
  for (const m of milestones) {
    if (ACCEPTED_STATES.has(m.state)) acceptedCount++;
    weightSum += STAGE_WEIGHT[m.state] ?? 0;
  }
  const acceptedPct = total ? acceptedCount / total : 0;
  const inflightPct = total ? weightSum / total : 0;
  const md = renderMd({ milestones, total, acceptedCount, acceptedPct, inflightPct });
  const outPath = join(projectDir, '.delivery', 'STATUS.md');
  return { total, acceptedCount, acceptedPct, inflightPct, outPath, md };
}

export function buildStatus(projectDir) {
  const snapshot = statusSnapshot(projectDir);
  writeFileSync(snapshot.outPath, snapshot.md, 'utf8');
  return snapshot;
}

export function checkStatus(projectDir) {
  const snapshot = statusSnapshot(projectDir);
  const current = existsSync(snapshot.outPath) ? readFileSync(snapshot.outPath, 'utf8') : '';
  return { ...snapshot, ok: current === snapshot.md };
}

function printStatusCheck(projectDir) {
  const r = checkStatus(projectDir);
  if (!r.ok) {
    console.error(`STATUS.md is stale: ${r.outPath}`);
    process.exit(1);
  }
  console.log(`STATUS.md is current: ${r.outPath}`);
}

const isMain = (() => {
  try {
    return fileURLToPath(import.meta.url) === process.argv[1];
  } catch {
    return false;
  }
})();
if (isMain) {
  const here = dirname(fileURLToPath(import.meta.url));
  const projectDir = join(here, '..', '..'); // .claude/scripts -> repo root
  if (process.argv.includes('--check')) {
    printStatusCheck(projectDir);
  } else {
    const r = buildStatus(projectDir);
    console.log(`STATUS.md written: ${r.outPath}`);
    console.log(`accepted ${pct(r.acceptedPct)} (${r.acceptedCount}/${r.total}), inflight ${pct(r.inflightPct)}`);
  }
}
