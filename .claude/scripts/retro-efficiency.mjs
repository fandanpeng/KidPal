#!/usr/bin/env node
// retro-efficiency.mjs — 周期复盘效率表（只读、stdout、零写盘）
//
// OWNER: architect（治理面 Track B 批次 261024-gov-review-retro-efficiency-instrumentation）
//
// 四类 Pod 指标 + 一张按 Agent 角色汇总表，全部从现有 pod-events.jsonl 与 git 历史直接复算，
// 不新增任何每 Pod 记录、不改任何事件 schema、不碰任何门判据。
//
// 口径单一权威在本脚本；PROPOSAL 与 OS 只指向本脚本、不复述列定义。
//
// 用法：
//   node .claude/scripts/retro-efficiency.mjs [--since <YYMMDD|YYYY-MM-DD>] [--until <...>]
//                                             [--change-id <id>] [--json] [--self-check]
//
// 分类规则（human 2026-10-26 拍定，二分消费域）：
//   1. 源码（c/cc/cpp/h/hpp/py/mjs/js/ts/tsx/sh/ps1/s/ld）恒算代码，不论域；
//   2. Markdown / README 恒算文档，不论域；
//   3. 数据/配置（json/jsonl/yaml/yml/toml/ini/conf/cfg/env）按消费域：治理域→文档，业务域→代码；
//   4. 二进制/资产同理二分（numstat 计 0 行，仅影响文件计数）；
//   5. 其余类型按域：业务域→代码；治理域/产品文档域→文档；域外先按类型，类型未覆盖默认代码侧。
//   域定义：治理域 = delivery/**、.claude/**、docs/debt|intake|templates|archive/**、changes/**；
//           业务域 = firmware/**、server/**、tools/**；产品文档域 = docs/** 其余部分。

import { execFileSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync, statSync, writeFileSync, mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

const ROOT = process.env.CLAUDE_PROJECT_DIR || process.cwd();
const TASKS_DIR = resolve(ROOT, 'delivery', 'taskpackages');

const SOURCE_EXT = new Set(['.c', '.cc', '.cpp', '.h', '.hpp', '.py', '.mjs', '.js', '.ts', '.tsx', '.sh', '.ps1', '.s', '.ld']);
const DOC_EXT = new Set(['.md', '.mdx']);
const DATA_EXT = new Set(['.json', '.jsonl', '.yaml', '.yml', '.toml', '.ini', '.conf', '.cfg', '.env']);
const BINARY_EXT = new Set(['.bin', '.png', '.jpg', '.jpeg', '.ico', '.bmp', '.ttf', '.otf', '.zip', '.gz', '.zst', '.pdf', '.hex']);
const TEST_RE = /(^|\/)(tests?|__tests__)\/|\.(test|spec)\.[a-z]+$|_test\.(py|mjs|js|c|cc|cpp|h)$|^test_.*\.py$|\.acceptance\./i;

const AGENT_ROLES = new Set(['builder', 'acceptance-author', 'integrator', 'fixer', 'architect', 'architect-reviewer', 'landing-verifier', 'planner']);
const SCRIPT_ROLES = new Set(['verifier', 'supervisor', 'merge-driver']);
const HUMAN_ROLES = new Set(['human', 'human-deputy']);
const ACTOR_ALIAS = { 'main-thread': 'planner' };

const MODEL_FAMILIES = [
  [/^glm-5[-.]3/i, 'glm-5.3'],
  [/^deepseek-v4[-.]1/i, 'deepseek-v4.1'],
  [/^deepseek-v4-flash/i, 'deepseek-v4'],
  [/^deepseek-flash/i, 'deepseek-flash'],
  [/^kimi-k3/i, 'kimi-k3'],
  [/^deepseek-v4-pro/i, 'deepseek-v4-pro'],
  [/^<synthetic>$/i, 'synthetic'],
];

function flagVal(name) {
  const i = process.argv.indexOf(name);
  return i >= 0 ? process.argv[i + 1] : undefined;
}

function extOf(p) {
  const b = p.split('/').pop();
  const i = b.lastIndexOf('.');
  return i < 0 ? '' : b.slice(i).toLowerCase();
}

function domainOf(p) {
  if (p.startsWith('delivery/') || p.startsWith('.claude/') ||
      p.startsWith('docs/debt/') || p.startsWith('docs/intake/') ||
      p.startsWith('docs/templates/') || p.startsWith('docs/archive/') ||
      p.startsWith('changes/')) return 'governance';
  if (p.startsWith('firmware/') || p.startsWith('server/') || p.startsWith('tools/')) return 'business';
  if (p.startsWith('docs/')) return 'product-doc';
  return 'outside';
}

function classifyPath(p) {
  const e = extOf(p);
  if (SOURCE_EXT.has(e)) return 'code';
  if (DOC_EXT.has(e) || /(^|\/)README[^/]*$/i.test(p)) return 'doc';
  const d = domainOf(p);
  if (DATA_EXT.has(e)) return (d === 'governance' || d === 'product-doc') ? 'doc' : 'code';
  if (BINARY_EXT.has(e)) return (d === 'governance' || d === 'product-doc') ? 'doc' : 'code';
  return (d === 'governance' || d === 'product-doc') ? 'doc' : 'code';
}

function isTestPath(p) {
  return TEST_RE.test(p);
}

function actorKind(raw) {
  const name = ACTOR_ALIAS[raw] || raw || 'unknown';
  if (AGENT_ROLES.has(name)) return { kind: 'agent', name };
  if (SCRIPT_ROLES.has(name)) return { kind: 'script', name };
  if (HUMAN_ROLES.has(name)) return { kind: 'human', name };
  return { kind: 'unmapped', name };
}

function modelFamily(raw) {
  for (const [re, fam] of MODEL_FAMILIES) if (re.test(raw)) return fam;
  return null;
}

function modelDisplay(models, unmapped) {
  if (!models || models.length === 0) return '';
  const fams = new Set();
  for (const m of models) {
    const f = modelFamily(m);
    if (f) fams.add(f);
    else { fams.add(m); if (unmapped) unmapped.add(m); }
  }
  return [...fams].sort().join('+');
}

function numstatSum(shas) {
  const perFile = new Map();
  for (const sha of shas) {
    let out;
    try {
      out = execFileSync('git', ['-C', ROOT, 'diff', '--numstat', `${sha}^1`, sha], { encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 });
    } catch {
      continue;
    }
    for (const line of out.split('\n')) {
      if (!line.trim()) continue;
      const [a, d, p] = line.split('\t');
      if (p === undefined) continue;
      const n = (a === '-' ? 0 : Number(a)) + (d === '-' ? 0 : Number(d));
      perFile.set(p, Math.max(perFile.get(p) || 0, n));
    }
  }
  return perFile;
}

function findMergeShas(id) {
  let out;
  try {
    out = execFileSync('git', ['-C', ROOT, 'log', '--all', '--merges', '--format=%H%x09%s', `--grep=${id}`], { encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 });
  } catch {
    return [];
  }
  const marker = `delivery/taskpackages/${id}/`;
  const picked = [];
  for (const line of out.split('\n').filter(Boolean)) {
    const [sha, subject] = line.split('\t');
    let named = (subject || '').startsWith(`Merge ${id}`) || (subject || '').includes(`merge: dev/${id}`);
    let touches = false;
    try {
      touches = execFileSync('git', ['-C', ROOT, 'diff', '--name-only', `${sha}^1`, sha], { encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 }).includes(marker);
    } catch { /* ignore */ }
    if (named || touches) picked.push(sha);
  }
  return picked;
}

function computeLines(id, shas) {
  if (!shas || shas.length === 0) {
    return { codeLines: null, testLines: null, docLines: null, ratio: null, ratioDocImpl: null, outsidePaths: {}, unattributedReason: 'no-merge-commit' };
  }
  const perFile = numstatSum(shas);
  if (perFile.size === 0) {
    return { codeLines: null, testLines: null, docLines: null, ratio: null, ratioDocImpl: null, outsidePaths: {}, unattributedReason: 'merge-diff-unavailable' };
  }
  let code = 0, test = 0, doc = 0;
  const outsidePaths = {};
  for (const [p, n] of perFile) {
    if (domainOf(p) === 'outside') outsidePaths[p] = Math.max(outsidePaths[p] || 0, n);
    if (classifyPath(p) === 'doc') doc += n;
    else {
      code += n;
      if (isTestPath(p)) test += n;
    }
  }
  return {
    codeLines: code,
    testLines: test,
    docLines: doc,
    ratio: code === 0 ? null : doc / code,
    ratioDocImpl: (code - test) === 0 ? null : doc / (code - test),
    outsidePaths,
    unattributedReason: null,
  };
}

function readEvents(id) {
  const p = resolve(TASKS_DIR, id, 'pod-events.jsonl');
  if (!existsSync(p)) return { events: [], malformed: 0, exists: false };
  const events = [];
  let malformed = 0;
  for (const line of readFileSync(p, 'utf8').split(/\r?\n/)) {
    if (!line.trim()) continue;
    try { events.push(JSON.parse(line)); } catch { malformed++; }
  }
  return { events, malformed, exists: true };
}

function sumTokens(events) {
  let total = 0, exact = 0, unavailable = 0, totalDispatches = 0;
  for (const e of events) {
    if (e.eventType !== 'dispatch-completed') continue;
    totalDispatches++;
    const tu = e.payload && e.payload.tokenUsage;
    if (tu && tu.status === 'exact') {
      exact++;
      if (typeof tu.total === 'number') total += tu.total;
    } else unavailable++;
  }
  return { total, exact, unavailable, totalDispatches };
}

function sumDuration(events) {
  let ms = 0, count = 0;
  for (const e of events) {
    if (e.eventType !== 'dispatch-completed') continue;
    const d = e.payload && e.payload.durationMs;
    if (typeof d === 'number') { ms += d; count++; }
  }
  return { ms, count };
}

function wallMs(events) {
  const ts = events.map(e => e.timestamp).filter(Boolean).sort();
  if (ts.length < 2) return null;
  const a = new Date(ts[0]).getTime();
  const b = new Date(ts[ts.length - 1]).getTime();
  if (!Number.isFinite(a) || !Number.isFinite(b) || b < a) return null;
  return b - a;
}

function modelsOf(events) {
  const s = new Set();
  for (const e of events) {
    if (e.eventType !== 'dispatch-completed') continue;
    const m = e.payload && e.payload.model;
    if (typeof m === 'string' && m) s.add(m);
  }
  return [...s].sort();
}

function plannerCheckpoints(events) {
  const cps = [];
  for (const e of events) {
    if (e.eventType !== 'dispatch-completed') continue;
    const m = e.payload && e.payload.mainThreadUsage;
    if (m && m.source === 'transcript' && typeof m.total === 'number') cps.push({ ts: e.timestamp || '', total: m.total });
  }
  cps.sort((a, b) => String(a.ts).localeCompare(String(b.ts)));
  return cps;
}

function plannerDelta(cps) {
  if (!cps || cps.length < 2) return null;
  const first = cps[0].total;
  const last = cps[cps.length - 1].total;
  const d = last - first;
  return d > 0 ? d : null;
}

function plannerInvalidKind(cps) {
  if (!cps || cps.length < 2) return 'insufficient-checkpoints';
  const first = cps[0].total;
  const last = cps[cps.length - 1].total;
  if (last === first) return 'identical';
  if (last < first) return 'decreasing';
  for (let i = 1; i < cps.length; i++) if (cps[i].total < cps[i - 1].total) return 'decreasing-adjacent';
  return null;
}

function plannerFlags(cps) {
  const n = cps.length;
  if (n < 2) return { firstLastNegative: false, adjacentDecreasing: false, identical: false };
  const first = cps[0].total;
  const last = cps[n - 1].total;
  return {
    firstLastNegative: last < first,
    adjacentDecreasing: cps.some((c, i) => i > 0 && c.total < cps[i - 1].total),
    identical: last === first,
  };
}

function listPodIds() {
  if (!existsSync(TASKS_DIR)) return [];
  return readdirSync(TASKS_DIR, { withFileTypes: true })
    .filter(d => d.isDirectory() && d.name !== 'current')
    .map(d => d.name)
    .filter(id => existsSync(resolve(TASKS_DIR, id, 'taskpackage.json')) || existsSync(resolve(TASKS_DIR, id, 'pod-events.jsonl')))
    .sort();
}

function normDate(v) {
  if (!v) return null;
  const s = String(v).replace(/-/g, '');
  return s.length >= 6 ? s.slice(-6) : s;
}

function analyzePod(id, opts) {
  const ev = readEvents(id);
  const tokens = sumTokens(ev.events);
  const dur = sumDuration(ev.events);
  const models = modelsOf(ev.events);
  const cps = plannerCheckpoints(ev.events);
  const delta = plannerDelta(cps);
  const lines = opts.withLines ? computeLines(id, findMergeShas(id)) : { codeLines: null, testLines: null, docLines: null, ratio: null, ratioDocImpl: null, outsidePaths: {}, unattributedReason: 'lines-disabled' };
  const wall = wallMs(ev.events);
  const actors = {};
  for (const e of ev.events) {
    if (e.eventType !== 'dispatch-completed') continue;
    const a = e.actor || 'unknown';
    actors[a] = actors[a] || { dispatches: 0, token: 0, exact: 0, durationMs: 0, durationCount: 0 };
    actors[a].dispatches++;
    const tu = e.payload && e.payload.tokenUsage;
    if (tu && tu.status === 'exact' && typeof tu.total === 'number') { actors[a].token += tu.total; actors[a].exact++; }
    const d = e.payload && e.payload.durationMs;
    if (typeof d === 'number') { actors[a].durationMs += d; actors[a].durationCount++; }
  }
  return {
    changeId: id,
    models,
    codeLines: lines.codeLines,
    testLines: lines.testLines,
    docLines: lines.docLines,
    ratio: lines.ratio,
    ratioDocImpl: lines.ratioDocImpl,
    outsidePaths: lines.outsidePaths,
    unattributedReason: lines.unattributedReason,
    token: tokens.total,
    tokenExactDispatches: tokens.exact,
    tokenUnavailableDispatches: tokens.unavailable,
    totalDispatches: tokens.totalDispatches,
    agentMs: dur.ms,
    durationCount: dur.count,
    wallMs: wall,
    planner: {
      checkpoints: cps.length,
      delta: delta,
      invalid: cps.length >= 2 ? plannerInvalidKind(cps) : (cps.length === 1 ? 'insufficient-checkpoints' : 'no-checkpoints'),
      ...plannerFlags(cps),
    },
    actors,
    malformed: ev.malformed,
    hasEvents: ev.exists,
  };
}

function aggregate(pods) {
  const roles = {};
  const unmappedActors = new Set();
  const unmappedModels = new Set();
  let totalToken = 0, totalDispatchMs = 0;
  let plannerToken = 0, plannerSpanMs = 0, plannerValid = 0;
  let plannerIdentical = 0, plannerDecreasing = 0, plannerAdjacentDecreasing = 0, plannerInsufficient = 0;
  let outsideLines = 0, malformed = 0;
  const outsideFiles = new Set();
  let attributedPods = 0, exactTokenPods = 0, modelPods = 0;

  for (const p of pods) {
    if (p.codeLines !== null) attributedPods++;
    if (p.tokenExactDispatches > 0) exactTokenPods++;
    if (p.models.length > 0) modelPods++;
    const op = p.outsidePaths || {};
    outsideLines += Object.values(op).reduce((a, b) => a + b, 0);
    for (const path of Object.keys(op)) outsideFiles.add(path);
    malformed += p.malformed;
    if (p.planner.checkpoints === 1) plannerInsufficient++;
    if (p.planner.identical) plannerIdentical++;
    if (p.planner.firstLastNegative) plannerDecreasing++;
    if (p.planner.adjacentDecreasing) plannerAdjacentDecreasing++;
    if (p.planner.delta !== null) {
      plannerValid++;
      plannerToken += p.planner.delta;
      if (p.wallMs !== null && typeof p.agentMs === 'number') {
        const span = p.wallMs - p.agentMs;
        if (span > 0) plannerSpanMs += span;
      }
    }

    for (const [raw, a] of Object.entries(p.actors)) {
      const k = actorKind(raw);
      if (k.kind === 'unmapped') { unmappedActors.add(raw); continue; }
      if (k.kind === 'human') continue;
      if (k.kind === 'script') continue;
      if (k.name === 'planner') continue; // planner 走主线程检查点，不走 dispatch
      const key = k.name;
      roles[key] = roles[key] || { role: key, kind: 'agent', dispatches: 0, token: 0, durationMs: 0, podsWithToken: new Set(), podsWithDuration: new Set() };
      roles[key].dispatches += a.dispatches;
      roles[key].token += a.token;
      roles[key].durationMs += a.durationMs;
      if (a.exact > 0) roles[key].podsWithToken.add(p.changeId);
      if (a.durationCount > 0) roles[key].podsWithDuration.add(p.changeId);
      totalToken += a.token;
      totalDispatchMs += a.durationMs;
    }
    for (const m of p.models) if (!modelFamily(m)) unmappedModels.add(m);
  }

  totalToken += plannerToken; // token 占比分母含 planner
  const roleList = Object.values(roles).map(r => ({
    role: r.role,
    kind: r.kind,
    dispatches: r.dispatches,
    token: r.token,
    durationMs: r.durationMs,
    podsWithToken: r.podsWithToken.size,
    podsWithDuration: r.podsWithDuration.size,
  }));
  roleList.push({
    role: 'planner',
    kind: 'agent-main-thread',
    dispatches: 0,
    token: plannerToken,
    durationMs: null,
    spanMs: plannerSpanMs,
    podsWithToken: plannerValid,
    podsWithDuration: 0,
    coverage: `${plannerValid}/${pods.length}`,
  });
  roleList.sort((a, b) => (b.token || 0) - (a.token || 0));
  return {
    roles: roleList,
    scriptRoles: [...SCRIPT_ROLES],
    unmappedActors: [...unmappedActors].sort(),
    unmappedModels: [...unmappedModels].sort(),
    totals: { token: totalToken, dispatchMs: totalDispatchMs },
    coverage: {
      totalPods: pods.length,
      attributedPods,
      unattributedPods: pods.length - attributedPods,
      exactTokenPods,
      noTokenPods: pods.length - exactTokenPods,
      modelPods,
      noModelPods: pods.length - modelPods,
      plannerValid,
      plannerIdentical,
      plannerDecreasing,
      plannerAdjacentDecreasing,
      plannerInsufficient,
      outsideKnownRootsLines: outsideLines,
      outsideKnownRootsFiles: outsideFiles.size,
      malformedLines: malformed,
    },
  };
}

function fmtNum(n) {
  return typeof n === 'number' && Number.isFinite(n) ? Math.round(n).toLocaleString('en-US') : '';
}

function fmtMin(ms) {
  return typeof ms === 'number' && Number.isFinite(ms) ? (ms / 60000).toFixed(1) : '';
}

function pct(part, whole) {
  if (!whole) return '';
  return `${(part / whole * 100).toFixed(1)}%`;
}

function renderMarkdown(pods, agg, scope) {
  const lines = [];
  const scopeBits = [];
  if (scope.since) scopeBits.push(`since=${scope.since}`);
  if (scope.until) scopeBits.push(`until=${scope.until}`);
  if (scope.changeId) scopeBits.push(`changeId=${scope.changeId}`);
  lines.push(`# 效率表${scopeBits.length ? `（${scopeBits.join(' · ')}）` : ''}`);
  lines.push('');
  lines.push(`> 口径唯一权威 = \`.claude/scripts/retro-efficiency.mjs\`。代码/文档按 human 2026-10-26 二分域规则：源码恒代码、Markdown 恒文档、数据/配置与二进制按消费域（治理域→文档、业务域→代码）。token 只求和 \`exact\`；行数归因不到留空，不写 0；墙钟 = 首末事件差，事件稀疏时低估。`);
  lines.push('');
  lines.push('## 一、Pod 表');
  lines.push('');
  lines.push('| changeId | 模型 | 代码行 | 其中测试行 | 文档行 | 文档代码比 | token | agent 分钟 | 墙钟分钟 |');
  lines.push('|---|---|---|---|---|---|---|---|---|');
  for (const p of pods) {
    const tokenCell = p.tokenExactDispatches > 0 ? `${fmtNum(p.token)} (${p.tokenExactDispatches}/${p.totalDispatches})` : '';
    const ratio = p.ratio === null ? '' : p.ratio.toFixed(2);
    lines.push(`| ${p.changeId} | ${modelDisplay(p.models, null)} | ${fmtNum(p.codeLines)} | ${fmtNum(p.testLines)} | ${fmtNum(p.docLines)} | ${ratio} | ${tokenCell} | ${fmtMin(p.agentMs)} | ${fmtMin(p.wallMs)} |`);
  }
  lines.push('');
  lines.push('> 文档代码比 = 文档行 ÷ 代码行（代码行 = 实现 + 测试）；代码行为 0 或行数归因不到时留空。`--json` 另给 `testLines` / `ratioDocImpl`（文档行 ÷ 实现行）。行数为空的 Pod 见 `--json.unattributedReason`。');
  lines.push('');
  lines.push('## 二、按 Agent 角色汇总');
  lines.push('');
  lines.push('| 角色 | 派发次数 | exact token | agent 分钟 | token 占比 | 时长占比 | 覆盖（有数据 Pod 数） |');
  lines.push('|---|---|---|---|---|---|---|');
  const totalToken = agg.totals.token || 0;
  const totalMs = agg.totals.dispatchMs || 0;
  for (const r of agg.roles) {
    const isPlanner = r.role === 'planner';
    const tokenCell = r.token > 0 ? fmtNum(r.token) : '';
    const minuteCell = isPlanner ? (r.spanMs ? `${fmtMin(r.spanMs)}（主线程时段，含人等）` : '') : (typeof r.durationMs === 'number' ? fmtMin(r.durationMs) : '');
    const tokenShare = r.token > 0 ? pct(r.token, totalToken) : '';
    const durationShare = !isPlanner && typeof r.durationMs === 'number' && totalMs ? pct(r.durationMs, totalMs) : '';
    const coverage = isPlanner ? `${r.podsWithToken}/${agg.coverage.totalPods}` : `token ${r.podsWithToken} / 时长 ${r.podsWithDuration}`;
    lines.push(`| ${r.role} | ${isPlanner ? '—' : r.dispatches} | ${tokenCell} | ${minuteCell} | ${tokenShare} | ${durationShare} | ${coverage} |`);
  }
  lines.push(`| 脚本驱动（verifier / supervisor） | — | — | — | — | — | 不计入 agent 成本 |`);
  lines.push('');
  lines.push(`> token 占比分母 = 本表所有有 exact 值的行合计（含 planner，合计 ${fmtNum(totalToken)}）；时长占比分母 = 有 durationMs 的派发行合计（planner 时段与脚本行不并入）。planner 的时间列是「主线程时段 = 墙钟 − Σ派发时长」，**包含人等与串行等待，不是纯工作时长**，不并入时长占比。`);
  lines.push('');
  lines.push('## 三、数据缺口');
  lines.push('');
  const c = agg.coverage;
  lines.push(`- 行集 ${c.totalPods} 个 Pod：行数可归因 ${c.attributedPods}、归因不到 ${c.unattributedPods}（留空）。`);
  lines.push(`- token：有 exact ${c.exactTokenPods}、无 exact ${c.noTokenPods}。`);
  lines.push(`- 模型：有记录 ${c.modelPods}、无记录 ${c.noModelPods}。`);
  lines.push(`- planner 检查点：可算 ${c.plannerValid}、首末相同 ${c.plannerIdentical}、首末为负 ${c.plannerDecreasing}、出现相邻倒退 ${c.plannerAdjacentDecreasing}、检查点不足 ${c.plannerInsufficient}。`);
  lines.push(`- 域外路径：${c.outsideKnownRootsFiles} 个文件 / ${c.outsideKnownRootsLines} 行（仅作诊断计数，已按类型规则确定归类）。`);
  if (c.malformedLines > 0) lines.push(`- 畸形事件行：${c.malformedLines}（已跳过，不影响其余行）。`);
  return lines.join('\n');
}

function buildPods(opts) {
  let ids = listPodIds();
  if (opts.changeId) ids = ids.filter(id => id === opts.changeId);
  if (opts.since) ids = ids.filter(id => id.slice(0, 6) >= opts.since);
  if (opts.until) ids = ids.filter(id => id.slice(0, 6) <= opts.until);
  return ids.map(id => analyzePod(id, { withLines: true }));
}

function runMain() {
  const since = normDate(flagVal('--since'));
  const until = normDate(flagVal('--until'));
  const changeId = flagVal('--change-id') || null;
  const json = process.argv.includes('--json');
  const pods = buildPods({ since, until, changeId });
  const agg = aggregate(pods.map(p => ({ ...p, actors: p.actors })));
  if (json) {
    console.log(JSON.stringify({ ok: true, generatedAt: new Date().toISOString(), scope: { since, until, changeId }, coverage: agg.coverage, totals: agg.totals, pods, roles: agg.roles, scriptRoles: agg.scriptRoles, unmappedActors: agg.unmappedActors, unmappedModels: agg.unmappedModels }, null, 2));
  } else {
    console.log(renderMarkdown(pods, agg, { since, until, changeId }));
  }
  process.exit(0);
}

// ───────────────────────────── self-check ─────────────────────────────
function snapshotDir(dir) {
  const out = [];
  const walk = (d, rel) => {
    for (const name of readdirSync(d).sort()) {
      const p = join(d, name);
      const r = rel ? `${rel}/${name}` : name;
      const st = statSync(p);
      if (st.isDirectory()) { walk(p, r); continue; }
      const h = createHash('sha256').update(readFileSync(p)).digest('hex');
      out.push(`${r}:${st.size}:${h}`);
    }
  };
  walk(dir, '');
  return out.join('\n');
}

function pseudoEvents(entries) {
  return entries.map((e, i) => ({
    eventType: e.type || 'dispatch-completed',
    actor: e.actor || 'builder',
    timestamp: `2026-01-01T00:00:${String(i).padStart(2, '0')}Z`,
    payload: e.payload || {},
  }));
}

function aggregateRoles(events) {
  const pod = { changeId: 'fixture', actors: {}, models: [], planner: { delta: null, invalid: null }, wallMs: null, agentMs: 0 };
  for (const e of events) {
    if (e.eventType !== 'dispatch-completed') continue;
    const a = e.actor || 'unknown';
    pod.actors[a] = pod.actors[a] || { dispatches: 0, token: 0, exact: 0, durationMs: 0, durationCount: 0 };
    pod.actors[a].dispatches++;
    const tu = e.payload && e.payload.tokenUsage;
    if (tu && tu.status === 'exact' && typeof tu.total === 'number') { pod.actors[a].token += tu.total; pod.actors[a].exact++; }
    const d = e.payload && e.payload.durationMs;
    if (typeof d === 'number') { pod.actors[a].durationMs += d; pod.actors[a].durationCount++; }
  }
  return aggregate([pod]);
}

function selfCheck() {
  const results = [];
  const check = (name, fn) => {
    try {
      const r = fn();
      results.push({ name, ok: r === true || (r && r.ok === true), detail: r && r.detail ? r.detail : '' });
    } catch (e) {
      results.push({ name, ok: false, detail: e.message });
    }
  };

  check('doc-code-directional', () => classifyPath('firmware/main/x.c') === 'code' && classifyPath('docs/x.md') === 'doc');
  check('token-exact-only', () => {
    const ev = pseudoEvents([
      { payload: { tokenUsage: { status: 'exact', total: 100 } } },
      { payload: { tokenUsage: { status: 'unavailable', total: 999999 } } },
    ]);
    const r = sumTokens(ev);
    return r.total === 100 && r.exact === 1 && r.unavailable === 1;
  });
  check('no-merge-blank-not-zero', () => {
    const r = computeLines('fixture', []);
    return r.codeLines === null && r.testLines === null && r.docLines === null && r.ratio === null;
  });
  check('model-dedupe-join', () => {
    const u = new Set();
    const d = modelDisplay(['glm-5-3-flash', 'glm-5-3-flash-260828', 'unknown-x'], u);
    return d === 'glm-5.3+unknown-x' && u.has('unknown-x');
  });
  check('ratio-zero-code-blank', () => {
    const r = computeLines('fixture', []);
    return r.ratio === null;
  });
  check('no-write-side-effects', () => {
    const dir = mkdtempSync(join(tmpdir(), 'retro-eff-'));
    try {
      mkdirSync(join(dir, 'pod'), { recursive: true });
      writeFileSync(join(dir, 'pod', 'pod-events.jsonl'), JSON.stringify({ eventType: 'dispatch-completed', actor: 'builder', payload: { tokenUsage: { status: 'exact', total: 10 }, durationMs: 1000 } }) + '\n');
      const before = snapshotDir(dir);
      // 代表性只读路径：解析并汇总
      const ev = [];
      for (const line of readFileSync(join(dir, 'pod', 'pod-events.jsonl'), 'utf8').split('\n')) if (line.trim()) ev.push(JSON.parse(line));
      sumTokens(ev); sumDuration(ev); actorKind('builder'); modelFamily('glm-5-3-flash');
      const after = snapshotDir(dir);
      return before === after;
    } finally {
      rmSync(dir, { recursive: true, force: true });
    }
  });
  check('role-token-sum', () => {
    const ev = pseudoEvents([
      { actor: 'builder', payload: { tokenUsage: { status: 'exact', total: 100 }, durationMs: 1000 } },
      { actor: 'integrator', payload: { tokenUsage: { status: 'exact', total: 50 }, durationMs: 500 } },
      { actor: 'builder', payload: { tokenUsage: { status: 'unavailable', total: 999999 }, durationMs: 0 } },
    ]);
    const agg = aggregateRoles(ev);
    const b = agg.roles.find(r => r.role === 'builder');
    return b && b.token === 100 && agg.totals.token === 150;
  });
  check('planner-delta-validity', () => {
    return plannerDelta([{ total: 100 }, { total: 150 }]) === 50 &&
      plannerDelta([{ total: 150 }, { total: 100 }]) === null &&
      plannerDelta([{ total: 100 }, { total: 100 }]) === null;
  });
  check('model-family-map', () => {
    return modelFamily('glm-5-3-flash-260828') === 'glm-5.3' &&
      modelFamily('deepseek-v4-1-flash-260910') === 'deepseek-v4.1' &&
      modelFamily('totally-new-model') === null;
  });
  check('actor-classification', () => {
    return actorKind('main-thread').name === 'planner' &&
      actorKind('merge-driver').kind === 'script' &&
      actorKind('human').kind === 'human';
  });
  check('consumer-domain-classification', () => {
    return classifyPath('.claude/verify-gates.json') === 'doc' &&
      classifyPath('server/deploy/manifest.yaml') === 'code' &&
      classifyPath('firmware/main/test/fixture.json') === 'code' &&
      classifyPath('docs/architecture.md') === 'doc' &&
      classifyPath('firmware/main/x.c') === 'code';
  });
  check('outside-roots-default-code', () => {
    return classifyPath('.codebase-memory/artifact.json') === 'code' && domainOf('.codebase-memory/artifact.json') === 'outside';
  });
  check('test-column', () => {
    const p = 'firmware/main/test/foo.acceptance.c';
    return isTestPath(p) === true && classifyPath(p) === 'code';
  });
  check('governance-data-as-doc', () => {
    // 反向断言：若实现把治理域 JSON 判成 code，此条必红。
    return classifyPath('.claude/scope-map.json') === 'doc' && classifyPath('delivery/taskpackages/x/taskpackage.json') === 'doc';
  });

  let failed = 0;
  for (const r of results) {
    if (!r.ok) failed++;
    console.log(`${r.ok ? 'ok  ' : 'FAIL'} ${r.name}${r.detail ? ` — ${r.detail}` : ''}`);
  }
  console.log(`self-check: ${results.length - failed}/${results.length} passed`);
  process.exit(failed === 0 ? 0 : 1);
}

if (process.argv.includes('--self-check')) selfCheck();
else runMain();
