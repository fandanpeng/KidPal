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
import { classifyFixerRootCause } from './pod-events-append.mjs';

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

// ── T1/T2/T3/T4（261026-gov-review-retro-efficiency-fidelity，FROZEN v0.4）──
const GOVERNANCE_ROOTS = ['.claude/', 'docs/', 'delivery/'];
const GOVERNANCE_ROOT_FILES = new Set(['AGENTS.md', 'CLAUDE.md']);
const SHA_SOURCE_PRIORITY = { 'merge-completed': 3, 'merged-legacy': 2, 'commit-message': 1 };

// T1 单源 glob 方言：仓根锚定；** 跨段（dir/** 匹配零层及以下、**/x 匹配零层）；* 单段内、不跨 /；大小写敏感。
function manifestGlobToRegExp(pattern) {
  let p = String(pattern);
  p = p.replace(/[.+?^${}()|[\]\\]/g, '\\$&');
  // 先把 ** 家族换成占位符，避免后续 * → [^/]* 把 ** 展开出的 .* 再改写。
  p = p.replace(/\*\*\//g, '\u0000');
  p = p.replace(/\/\*\*/g, '\u0001');
  p = p.replace(/\*\*/g, '\u0002');
  p = p.replace(/\*/g, '[^/]*');
  p = p.replace(/\u0000/g, '(?:.*/)?');
  p = p.replace(/\u0001/g, '(?:/.*)?');
  p = p.replace(/\u0002/g, '.*');
  return new RegExp(`^${p}$`);
}

function pathHitsGovernance(p) {
  const s = String(p);
  if (GOVERNANCE_ROOT_FILES.has(s)) return true;
  return GOVERNANCE_ROOTS.some(root => s === root.slice(0, -1) || s.startsWith(root));
}

// T2 governance 判据：subsystem 非空且其全部路径取值命中治理根，或 changeManifest 非空且全部条目命中治理根；
// 字段缺失或空一律不命中（不按 sourceType 猜、不按 id 猜）。
function subsystemHitsGovernance(subsystem) {
  if (typeof subsystem !== 'string' || !subsystem.trim()) return false;
  const s = subsystem.trim();
  if (GOVERNANCE_ROOT_FILES.has(s)) return true;
  const tokens = s.match(/[A-Za-z0-9_.@-]+(?:\/[A-Za-z0-9_.@*-]+)+/g) || [];
  if (tokens.length === 0) return false;
  return tokens.every(t => pathHitsGovernance(t));
}

function manifestHitsGovernance(manifest) {
  if (!Array.isArray(manifest) || manifest.length === 0) return false;
  return manifest.every(entry => pathHitsGovernance(String(entry)));
}

function readTaskpackage(id) {
  const p = resolve(TASKS_DIR, id, 'taskpackage.json');
  if (!existsSync(p)) return null;
  try { return JSON.parse(readFileSync(p, 'utf8').replace(/^﻿/, '')); } catch { return null; }
}

function computeTier(tp) {
  const rigor = tp && typeof tp.rigor === 'string' && tp.rigor ? tp.rigor : null;
  const workstream = tp && tp.origin && typeof tp.origin.workstream === 'string' && tp.origin.workstream ? tp.origin.workstream : null;
  const governance = !!(tp && (subsystemHitsGovernance(tp.subsystem) || manifestHitsGovernance(tp.changeManifest)));
  const slice = !!workstream;
  const value = governance ? 'governance' : (rigor ? rigor + (slice ? '·拆包' : '') : '');
  const source = !tp ? 'absent' : ((rigor || governance) ? 'taskpackage' : 'missing-rigor');
  return { rigor, governance, slice, workstream, value, source };
}

// T1 主判据：subject 分类器。named 恒采纳；batch 按 changeManifest 切分；backsync 不采纳。
function classifyMergeSubject(subject, id) {
  const s = String(subject || '');
  if (s.startsWith(`Merge ${id}`) || s.includes(`merge: dev/${id}`)) return 'named';
  const ids = new Set((s.match(/\b\d{6}-[a-z0-9][a-z0-9-]*/gi) || []).map(x => x.toLowerCase()));
  return ids.size >= 2 ? 'batch' : 'backsync';
}

function effectiveMergeVerdict(subject, id, shared) {
  return shared ? 'batch' : classifyMergeSubject(subject, id);
}

function resolveCommitSha(raw) {
  if (!raw) return null;
  try {
    const out = execFileSync('git', ['-C', ROOT, 'rev-parse', '--verify', `${String(raw).trim()}^{commit}`], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).trim();
    return out || null;
  } catch { return null; }
}

function subjectOf(sha) {
  try {
    return execFileSync('git', ['-C', ROOT, 'log', '-1', '--format=%s', sha], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] }).trim();
  } catch { return ''; }
}

function touchedFiles(sha) {
  try {
    return execFileSync('git', ['-C', ROOT, 'diff', '--name-only', `${sha}^1`, sha], { encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 }).split('\n').filter(Boolean);
  } catch { return []; }
}

function numstatOne(sha) {
  const perFile = new Map();
  try {
    const out = execFileSync('git', ['-C', ROOT, 'diff', '--numstat', `${sha}^1`, sha], { encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 });
    for (const line of out.split('\n')) {
      if (!line.trim()) continue;
      const [a, d, p] = line.split('\t');
      if (p === undefined) continue;
      const n = (a === '-' ? 0 : Number(a)) + (d === '-' ? 0 : Number(d));
      perFile.set(p, Math.max(perFile.get(p) || 0, n));
    }
  } catch { /* unavailable */ }
  return perFile;
}

function changeIdsInSubject(subject) {
  return [...new Set((String(subject || '').match(/\b\d{6}-[a-z0-9][a-z0-9-]*/gi) || []).map(x => x.toLowerCase()))];
}

function splitBatchPaths(perFile, participants, manifestsById) {
  const assignments = new Map();
  const unattributed = [];
  const assign = (id, path, n) => {
    if (!assignments.has(id)) assignments.set(id, new Map());
    const m = assignments.get(id);
    m.set(path, Math.max(m.get(path) || 0, n));
  };
  for (const [path, n] of perFile) {
    const owner = participants.find(id => path.startsWith(`delivery/taskpackages/${id}/`));
    if (owner) { assign(owner, path, n); continue; }
    const hits = participants.filter(id => {
      const manifest = manifestsById.get(id);
      return Array.isArray(manifest) && manifest.some(g => manifestGlobToRegExp(g).test(path));
    });
    if (hits.length === 1) { assign(hits[0], path, n); continue; }
    unattributed.push({ path, reason: hits.length === 0 ? 'no-manifest' : 'ambiguous-manifest', candidates: hits });
  }
  return { assignments, unattributed };
}

const BATCH_SPLIT_CACHE = new Map();
function getBatchSplit(sha, subject, allIds) {
  const key = `${sha}|${allIds.join(',')}`;
  if (BATCH_SPLIT_CACHE.has(key)) return BATCH_SPLIT_CACHE.get(key);
  const perFile = numstatOne(sha);
  const participants = changeIdsInSubject(subject).filter(id => allIds.includes(id));
  const manifestsById = new Map(participants.map(id => [id, readTaskpackage(id)?.changeManifest]));
  const split = splitBatchPaths(perFile, participants, manifestsById);
  split.unattributed = split.unattributed.map(u => ({ sha, ...u }));
  split.files = perFile.size;
  BATCH_SPLIT_CACHE.set(key, split);
  return split;
}

function unionShas(...lists) {
  const s = new Set();
  for (const l of lists) for (const x of l) s.add(x);
  return [...s];
}

function buildCandidateShas(entries) {
  const commit = entries.filter(e => e.sources.has('commit-message')).map(e => e.sha);
  const event = entries.filter(e => e.sources.has('merge-completed') || e.sources.has('merged-legacy')).map(e => e.sha);
  return unionShas(commit, event);
}

function buildCandidateIndex(ids, eventsById) {
  const index = new Map();
  const add = (sha, source, podId, subject) => {
    if (!sha) return;
    let e = index.get(sha);
    if (!e) {
      e = { sha, subject: subject || '', sources: new Set(), pods: new Set(), filesTouched: null };
      index.set(sha, e);
    }
    if (subject && !e.subject) e.subject = subject;
    e.sources.add(source);
    e.pods.add(podId);
  };
  for (const id of ids) {
    let out = '';
    try {
      out = execFileSync('git', ['-C', ROOT, 'log', '--all', '--merges', '--format=%H%x09%s', `--grep=${id}`], { encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 });
    } catch { /* no git */ }
    for (const line of out.split('\n').filter(Boolean)) {
      const [sha, subject] = line.split('\t');
      add(resolveCommitSha(sha) || sha, 'commit-message', id, subject);
    }
    for (const e of (eventsById.get(id) || [])) {
      if (e.eventType === 'merge-completed' && e.payload && e.payload.mergeSha) add(resolveCommitSha(e.payload.mergeSha), 'merge-completed', id, null);
      if (e.eventType === 'merged' && e.payload && e.payload.mergedCommit) add(resolveCommitSha(e.payload.mergedCommit), 'merged-legacy', id, null);
    }
  }
  for (const e of index.values()) {
    if (!e.subject) e.subject = subjectOf(e.sha);
    e.filesTouched = touchedFiles(e.sha).length;
  }
  return index;
}

function collectMergeData(id, index, allIds) {
  const entries = [...index.values()].filter(e => e.pods.has(id));
  const bySha = new Map(entries.map(e => [e.sha, e]));
  const perFile = new Map();
  const attribution = [];
  const sources = new Set();
  let mergeCount = 0;
  let sawBatch = false;
  for (const sha of buildCandidateShas(entries)) {
    const e = bySha.get(sha);
    if (!e) continue;
    const shared = e.pods.size >= 2;
    const verdict = effectiveMergeVerdict(e.subject, id, shared);
    const shaSource = [...e.sources].sort((a, b) => (SHA_SOURCE_PRIORITY[b] || 0) - (SHA_SOURCE_PRIORITY[a] || 0))[0] || null;
    attribution.push({
      sha,
      subject: e.subject,
      verdict,
      filesTouched: e.filesTouched,
      reason: shared ? 'shared-sha-guard' : (verdict === 'named' ? 'named-merge' : (verdict === 'batch' ? 'multi-change-subject' : 'backsync-subject')),
      shaSource,
    });
    for (const s of e.sources) sources.add(s);
    if (verdict === 'backsync') continue;
    mergeCount++;
    if (verdict === 'named') {
      for (const [path, n] of numstatOne(sha)) perFile.set(path, Math.max(perFile.get(path) || 0, n));
    } else {
      sawBatch = true;
      const split = getBatchSplit(sha, e.subject, allIds);
      const assigned = split.assignments.get(id);
      if (assigned) for (const [path, n] of assigned) perFile.set(path, Math.max(perFile.get(path) || 0, n));
    }
  }
  let unattributedReason = null;
  if (perFile.size === 0) {
    if (mergeCount === 0) unattributedReason = 'no-merge-commit';
    else if (sawBatch) unattributedReason = 'batch-merge-unattributable';
    else unattributedReason = 'merge-diff-unavailable';
  }
  return {
    perFile,
    attribution,
    attributionSources: [...sources].sort(),
    mergeCount,
    multiMerge: mergeCount > 1,
    unattributedReason,
  };
}

function blankLines(reason) {
  return { codeLines: null, testLines: null, docLines: null, ratio: null, ratioDocImpl: null, outsidePaths: {}, unattributedReason: reason };
}

function computeLinesFromPerFile(perFile, unattributedReason) {
  if (!perFile || perFile.size === 0) return blankLines(unattributedReason || 'no-merge-commit');
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

// T3 生命周期：起包取最早 pod-started、收口取最晚 pod-closed；缺失不写 0、不回退首末事件差。
function computeLifecycle(events, agentMs) {
  const starts = [], closes = [];
  for (const e of events) {
    if (!e || !e.timestamp) continue;
    const t = Date.parse(e.timestamp);
    if (isNaN(t)) continue;
    if (e.eventType === 'pod-started') starts.push(t);
    if (e.eventType === 'pod-closed') closes.push(t);
  }
  starts.sort((a, b) => a - b);
  closes.sort((a, b) => a - b);
  const start = starts.length ? starts[0] : null;
  const close = closes.length ? closes[closes.length - 1] : null;
  const podStartAt = start != null ? new Date(start).toISOString() : null;
  const podCloseoutAt = close != null ? new Date(close).toISOString() : null;
  let observationWindowMs = null;
  let waitingMs = null;
  let unknownReason = null;
  if (start == null && close == null) unknownReason = 'no-lifecycle-data';
  else if (start == null) unknownReason = 'no-start';
  else if (close == null) unknownReason = 'no-closeout';
  else {
    const w = close - start;
    if (w < 0) unknownReason = 'negative-window';
    else {
      observationWindowMs = w;
      if (typeof agentMs === 'number') {
        const wait = w - agentMs;
        if (wait >= 0) waitingMs = wait;
        else unknownReason = 'negative-window';
      }
    }
  }
  return {
    podStartAt,
    podCloseoutAt,
    observationWindowMs,
    waitingMs,
    lifecycleSource: (start != null || close != null) ? 'event' : 'absent',
    unknownReason,
  };
}

// T3 重复盖章：严格三条件（同 dispatchId + 该行 durationMs===0 + exact total 与同 id 非零时长行相等）。
function dedupeDispatchEvents(events) {
  const groups = new Map();
  const withoutDispatchId = [];
  for (const e of events) {
    if (e.eventType !== 'dispatch-completed') continue;
    const dispatchId = e.payload && e.payload.dispatchId;
    if (!dispatchId) { withoutDispatchId.push(e); continue; }
    if (!groups.has(dispatchId)) groups.set(dispatchId, []);
    groups.get(dispatchId).push(e);
  }
  const restamped = new Set();
  const ambiguousIdGroups = [];
  let restampedToken = 0;
  for (const [dispatchId, rows] of groups) {
    if (rows.length < 2) continue;
    const totalOf = (r) => (r.payload && r.payload.tokenUsage && r.payload.tokenUsage.status === 'exact' && typeof r.payload.tokenUsage.total === 'number') ? r.payload.tokenUsage.total : null;
    const durOf = (r) => (r.payload && typeof r.payload.durationMs === 'number') ? r.payload.durationMs : null;
    const nonzero = rows.filter(r => { const d = durOf(r); return d != null && d > 0 && totalOf(r) != null; });
    const zero = rows.filter(r => durOf(r) === 0 && totalOf(r) != null);
    for (const z of zero) {
      if (nonzero.some(n => totalOf(n) === totalOf(z))) {
        restamped.add(z);
        restampedToken += totalOf(z);
      }
    }
    const remaining = rows.filter(r => !restamped.has(r));
    if (remaining.length > 1) {
      ambiguousIdGroups.push({
        dispatchId,
        changeId: remaining[0].changeId || null,
        rows: rows.length,
        reason: 'distinct-values',
        rowsDetail: remaining.map(r => ({ durationMs: durOf(r), total: totalOf(r) })),
      });
    }
  }
  return {
    events: events.filter(e => e.eventType !== 'dispatch-completed' || !restamped.has(e)),
    dedupe: {
      restampedRows: restamped.size,
      restampedToken,
      affectedIds: [...new Set([...restamped].map(e => e.changeId).filter(Boolean))],
      ambiguousIdGroups,
      withoutDispatchId: withoutDispatchId.length,
    },
  };
}

// T4 fixer 根因：只统计 actor=fixer 且 rootCauseStatus=exact 的派发；与 friction 不 join、不双记。
function computeFixerRootCause(events) {
  const map = new Map();
  for (const e of events) {
    if (e.eventType !== 'dispatch-completed' || e.actor !== 'fixer') continue;
    if (!e.payload || e.payload.rootCauseStatus !== 'exact' || !e.payload.rootCause) continue;
    const k = e.payload.rootCause;
    if (!map.has(k)) map.set(k, { rootCause: k, dispatchCount: 0, token: 0, durationMs: 0, pods: new Set() });
    const row = map.get(k);
    row.dispatchCount++;
    const tu = e.payload.tokenUsage;
    if (tu && tu.status === 'exact' && typeof tu.total === 'number') row.token += tu.total;
    if (typeof e.payload.durationMs === 'number') row.durationMs += e.payload.durationMs;
    row.pods.add(e.changeId);
    map.set(k, row);
  }
  return [...map.values()]
    .map(r => ({ rootCause: r.rootCause, dispatchCount: r.dispatchCount, token: r.token, durationMs: r.durationMs, pods: r.pods.size }))
    .sort((a, b) => b.token - a.token);
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

function computeLines(id, shas) {
  if (!shas || shas.length === 0) return blankLines('no-merge-commit');
  const perFile = numstatSum(shas);
  if (perFile.size === 0) return blankLines('merge-diff-unavailable');
  return computeLinesFromPerFile(perFile, null);
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
  const ev = opts.events ? { events: opts.events, malformed: opts.malformed || 0, exists: opts.exists !== false } : readEvents(id);
  const deduped = dedupeDispatchEvents(ev.events);
  const events = deduped.events;
  const tokens = sumTokens(events);
  const dur = sumDuration(events);
  const models = modelsOf(events);
  const cps = plannerCheckpoints(events);
  const delta = plannerDelta(cps);
  const merge = opts.withLines
    ? collectMergeData(id, opts.candidateIndex, opts.allIds)
    : { perFile: new Map(), attribution: [], attributionSources: [], mergeCount: 0, multiMerge: false, unattributedReason: 'lines-disabled' };
  const lines = opts.withLines ? computeLinesFromPerFile(merge.perFile, merge.unattributedReason) : blankLines('lines-disabled');
  const wall = wallMs(ev.events);
  const actors = {};
  for (const e of events) {
    if (e.eventType !== 'dispatch-completed') continue;
    const a = e.actor || 'unknown';
    actors[a] = actors[a] || { dispatches: 0, token: 0, exact: 0, durationMs: 0, durationCount: 0 };
    actors[a].dispatches++;
    const tu = e.payload && e.payload.tokenUsage;
    if (tu && tu.status === 'exact' && typeof tu.total === 'number') { actors[a].token += tu.total; actors[a].exact++; }
    const d = e.payload && e.payload.durationMs;
    if (typeof d === 'number') { actors[a].durationMs += d; actors[a].durationCount++; }
  }
  const tier = computeTier(readTaskpackage(id));
  const lifecycle = computeLifecycle(ev.events, dur.ms);
  return {
    changeId: id,
    tier,
    tierValue: tier.value,
    models,
    codeLines: lines.codeLines,
    testLines: lines.testLines,
    docLines: lines.docLines,
    ratio: lines.ratio,
    ratioDocImpl: lines.ratioDocImpl,
    outsidePaths: lines.outsidePaths,
    unattributedReason: lines.unattributedReason,
    mergeAttribution: merge.attribution,
    attributionSources: merge.attributionSources,
    mergeCount: merge.mergeCount,
    multiMerge: merge.multiMerge,
    token: tokens.total,
    tokenExactDispatches: tokens.exact,
    tokenUnavailableDispatches: tokens.unavailable,
    totalDispatches: tokens.totalDispatches,
    agentMs: dur.ms,
    durationCount: dur.count,
    wallMs: wall,
    podStartAt: lifecycle.podStartAt,
    podCloseoutAt: lifecycle.podCloseoutAt,
    observationWindowMs: lifecycle.observationWindowMs,
    waitingMs: lifecycle.waitingMs,
    lifecycleSource: lifecycle.lifecycleSource,
    unknownReason: lifecycle.unknownReason,
    dedupe: deduped.dedupe,
    fixerRootCause: computeFixerRootCause(events),
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

function aggregate(pods, opts = {}) {
  const roles = {};
  const unmappedActors = new Set();
  const unmappedModels = new Set();
  let totalToken = 0, totalDispatchMs = 0;
  let plannerToken = 0, plannerSpanMs = 0, plannerValid = 0;
  let plannerIdentical = 0, plannerDecreasing = 0, plannerAdjacentDecreasing = 0, plannerInsufficient = 0;
  let outsideLines = 0, malformed = 0;
  const outsideFiles = new Set();
  let attributedPods = 0, exactTokenPods = 0, modelPods = 0;
  const tierCounts = { full: 0, lite: 0, nano: 0, governance: 0, slice: 0, blank: 0 };
  const dedupeTotals = { restampedRows: 0, restampedToken: 0, ambiguousIdGroups: 0, withoutDispatchId: 0 };
  const dedupePods = new Set();
  const fixerRootCauseMap = new Map();

  for (const p of pods) {
    if (p.codeLines !== null) attributedPods++;
    if (p.tokenExactDispatches > 0) exactTokenPods++;
    if (p.models.length > 0) modelPods++;
    const op = p.outsidePaths || {};
    outsideLines += Object.values(op).reduce((a, b) => a + b, 0);
    for (const path of Object.keys(op)) outsideFiles.add(path);
    malformed += p.malformed;
    const tv = p.tierValue || (p.tier && p.tier.value) || '';
    if (tv === 'governance') tierCounts.governance++;
    else if (tv.startsWith('full')) tierCounts.full++;
    else if (tv.startsWith('lite')) tierCounts.lite++;
    else if (tv.startsWith('nano')) tierCounts.nano++;
    else tierCounts.blank++;
    if (tv.includes('·拆包')) tierCounts.slice++;
    if (p.dedupe) {
      dedupeTotals.restampedRows += p.dedupe.restampedRows || 0;
      dedupeTotals.restampedToken += p.dedupe.restampedToken || 0;
      dedupeTotals.ambiguousIdGroups += (p.dedupe.ambiguousIdGroups || []).length;
      dedupeTotals.withoutDispatchId += p.dedupe.withoutDispatchId || 0;
      if ((p.dedupe.restampedRows || 0) > 0) dedupePods.add(p.changeId);
    }
    for (const r of p.fixerRootCause || []) {
      if (!fixerRootCauseMap.has(r.rootCause)) fixerRootCauseMap.set(r.rootCause, { rootCause: r.rootCause, dispatchCount: 0, token: 0, durationMs: 0, pods: new Set() });
      const cur = fixerRootCauseMap.get(r.rootCause);
      cur.dispatchCount += r.dispatchCount;
      cur.token += r.token;
      cur.durationMs += r.durationMs;
      cur.pods.add(p.changeId);
    }
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
    fixerRootCause: [...fixerRootCauseMap.values()]
      .map(r => ({ rootCause: r.rootCause, dispatchCount: r.dispatchCount, token: r.token, durationMs: r.durationMs, pods: r.pods.size }))
      .sort((a, b) => b.token - a.token),
    batchUnattributedPaths: opts.batchUnattributedPaths || [],
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
      tierCounts,
      dedupe: {
        restampedRows: dedupeTotals.restampedRows,
        restampedToken: dedupeTotals.restampedToken,
        affectedPods: [...dedupePods].sort(),
        ambiguousIdGroups: dedupeTotals.ambiguousIdGroups,
        withoutDispatchId: dedupeTotals.withoutDispatchId,
      },
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
  lines.push(`> 口径唯一权威 = \`.claude/scripts/retro-efficiency.mjs\`。代码/文档按 human 2026-10-26 二分域规则：源码恒代码、Markdown 恒文档、数据/配置与二进制按消费域（治理域→文档、业务域→代码）。token 只求和 \`exact\`；行数归因不到留空，不写 0；墙钟 = 首末事件差（旧近似，事件稀疏时低估）；观察窗 = 真实起包→收口，历史包无该数据。`);
  lines.push('');
  lines.push('## 一、Pod 表');
  lines.push('');
  lines.push('| changeId | 档位 | 模型 | 代码行 | 其中测试行 | 文档行 | 文档代码比 | token | agent 分钟 | 墙钟分钟 |');
  lines.push('|---|---|---|---|---|---|---|---|---|---|');
  for (const p of pods) {
    const tokenCell = p.tokenExactDispatches > 0 ? `${fmtNum(p.token)} (${p.tokenExactDispatches}/${p.totalDispatches})` : '';
    const ratio = p.ratio === null ? '' : p.ratio.toFixed(2);
    lines.push(`| ${p.changeId} | ${p.tierValue || ''} | ${modelDisplay(p.models, null)} | ${fmtNum(p.codeLines)} | ${fmtNum(p.testLines)} | ${fmtNum(p.docLines)} | ${ratio} | ${tokenCell} | ${fmtMin(p.agentMs)} | ${fmtMin(p.wallMs)} |`);
  }
  lines.push('');
  lines.push('> 文档代码比 = 文档行 ÷ 代码行（代码行 = 实现 + 测试）；代码行为 0 或行数归因不到时留空。`--json` 另给 `testLines` / `ratioDocImpl`（文档行 ÷ 实现行）。行数为空的 Pod 见 `--json.unattributedReason`。');
  lines.push('> 档位 = `taskpackage.json.rigor` 原值（`full`/`lite`/`nano`），`origin.workstream` 非空加 `·拆包`；`governance` 按 subsystem / changeManifest 全落治理根判定；缺 taskpackage 或 rigor 缺值留空，不推测。');
  lines.push('> 行数只归因包内命名合并与按 manifest 可切分的批量合并；回同步合并、批量合并中无主或歧义的文件不计入，见 `--json.mergeAttribution` / `batchUnattributedPaths`。同一文件在多次合并中被改写时取 max，故多笔合并的口径是下界，不是求和。`merged-legacy` 是历史直写、非 canonical 事件类型。');
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
  lines.push('## 三、时间拆分');
  lines.push('');
  lines.push('| changeId | 观察窗分钟 | agent 分钟 | 等待分钟 | 数据来源 |');
  lines.push('|---|---|---|---|---|');
  for (const p of pods) {
    let windowCell = '';
    let source = 'unknown';
    if (p.observationWindowMs != null) { windowCell = fmtMin(p.observationWindowMs); source = 'lifecycle'; }
    else if (p.wallMs != null) { windowCell = fmtMin(p.wallMs); source = 'wall-clock-legacy'; }
    const waitCell = p.waitingMs != null ? fmtMin(p.waitingMs) : '';
    lines.push(`| ${p.changeId} | ${windowCell} | ${fmtMin(p.agentMs)} | ${waitCell} | ${source} |`);
  }
  lines.push('');
  lines.push('> 观察窗 = 真实起包→收口；缺失显示 unknown、不显示 0、不倒推。`wall-clock-legacy` = 旧的首末事件差近似（事件稀疏时低估）。等待分钟 = 观察窗 − agent 分钟；差为负记 `negative-window`，不 clamp 成 0。');
  lines.push('');
  lines.push('## 四、fixer 根因分布');
  lines.push('');
  if (agg.fixerRootCause && agg.fixerRootCause.length) {
    lines.push('| 根因 | 派发次数 | token | agent 分钟 | Pod 数 |');
    lines.push('|---|---|---|---|---|');
    for (const r of agg.fixerRootCause) lines.push(`| ${r.rootCause} | ${r.dispatchCount} | ${fmtNum(r.token)} | ${fmtMin(r.durationMs)} | ${r.pods} |`);
  } else {
    const fixerRole = (agg.roles || []).find(r => r.role === 'fixer');
    lines.push(`fixer 根因：0/${fixerRole ? fixerRole.dispatches : 0} 派发带根因（本批之前的历史派发无此字段）。`);
  }
  lines.push('');
  lines.push('> `friction` 是 planner 视角的过程摩擦，`rootCause` 是单次 fixer 派发的归因标签；两者粒度与生产者不同，不 join、不双记、不相加。');
  lines.push('');
  lines.push('## 五、数据缺口');
  lines.push('');
  const c = agg.coverage;
  const tc = c.tierCounts || {};
  lines.push(`- 行集 ${c.totalPods} 个 Pod：行数可归因 ${c.attributedPods}、归因不到 ${c.unattributedPods}（留空）。`);
  lines.push(`- 档位：full ${tc.full || 0} / lite ${tc.lite || 0} / nano ${tc.nano || 0} / governance ${tc.governance || 0} / 拆包 ${tc.slice || 0} / 留空 ${tc.blank || 0}。`);
  lines.push(`- token：有 exact ${c.exactTokenPods}、无 exact ${c.noTokenPods}。`);
  lines.push(`- 模型：有记录 ${c.modelPods}、无记录 ${c.noModelPods}。`);
  lines.push(`- planner 检查点：可算 ${c.plannerValid}、首末相同 ${c.plannerIdentical}、首末为负 ${c.plannerDecreasing}、出现相邻倒退 ${c.plannerAdjacentDecreasing}、检查点不足 ${c.plannerInsufficient}。`);
  const dd = c.dedupe || {};
  lines.push(`- 重复盖章：剔除 ${dd.restampedRows || 0} 行 / ${fmtNum(dd.restampedToken || 0)} token（涉及 ${(dd.affectedPods || []).length} 个 Pod）；id 复用组 ${dd.ambiguousIdGroups || 0} 个（只计数、未剔除）；无 dispatchId 的 dispatch ${dd.withoutDispatchId || 0} 条。`);
  lines.push(`- 批量合并切分无主/歧义路径：${(agg.batchUnattributedPaths || []).length} 条（见 \`--json.batchUnattributedPaths\`）。`);
  lines.push(`- 域外路径：${c.outsideKnownRootsFiles} 个文件 / ${c.outsideKnownRootsLines} 行（仅作诊断计数，已按类型规则确定归类）。`);
  if (c.malformedLines > 0) lines.push(`- 畸形事件行：${c.malformedLines}（已跳过，不影响其余行）。`);
  return lines.join('\n');
}

function buildPods(opts) {
  let ids = listPodIds();
  if (opts.changeId) ids = ids.filter(id => id === opts.changeId);
  if (opts.since) ids = ids.filter(id => id.slice(0, 6) >= opts.since);
  if (opts.until) ids = ids.filter(id => id.slice(0, 6) <= opts.until);
  BATCH_SPLIT_CACHE.clear();
  const readById = new Map();
  const eventsById = new Map();
  for (const id of ids) {
    const r = readEvents(id);
    readById.set(id, r);
    eventsById.set(id, r.events);
  }
  const candidateIndex = opts.withLines ? buildCandidateIndex(ids, eventsById) : new Map();
  const pods = ids.map(id => {
    const r = readById.get(id);
    return analyzePod(id, { withLines: opts.withLines, events: r.events, malformed: r.malformed, exists: r.exists, candidateIndex, allIds: ids });
  });
  const batchUnattributedPaths = [];
  const seen = new Set();
  for (const split of BATCH_SPLIT_CACHE.values()) {
    for (const u of split.unattributed) {
      const k = `${u.sha}|${u.path}`;
      if (seen.has(k)) continue;
      seen.add(k);
      batchUnattributedPaths.push(u);
    }
  }
  return { pods, batchUnattributedPaths };
}

function runMain() {
  const since = normDate(flagVal('--since'));
  const until = normDate(flagVal('--until'));
  const changeId = flagVal('--change-id') || null;
  const json = process.argv.includes('--json');
  const { pods, batchUnattributedPaths } = buildPods({ since, until, changeId, withLines: true });
  const agg = aggregate(pods, { batchUnattributedPaths });
  if (json) {
    console.log(JSON.stringify({ ok: true, generatedAt: new Date().toISOString(), scope: { since, until, changeId }, coverage: agg.coverage, totals: agg.totals, pods, roles: agg.roles, scriptRoles: agg.scriptRoles, unmappedActors: agg.unmappedActors, unmappedModels: agg.unmappedModels, fixerRootCause: agg.fixerRootCause, batchUnattributedPaths: agg.batchUnattributedPaths }, null, 2));
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
  // ── 261026-gov-review-retro-efficiency-fidelity 新增（14 → 23）──
  check('named-only-drops-backsync', () => {
    const named = classifyMergeSubject('Merge 261022-a-subject (merge-driver)', '261022-a-subject');
    const batch = classifyMergeSubject('Merge dev/pod-m2: 261022-a-subject + 261022-b-subject', '261022-a-subject');
    const backsync = classifyMergeSubject('merge: main→gov 回同步（含 261022-a-subject）', '261022-a-subject');
    return named === 'named' && batch === 'batch' && backsync === 'backsync';
  });
  check('batch-split-ambiguous-dropped', () => {
    const perFile = new Map([
      ['delivery/taskpackages/261022-a-subject/notes.md', 1],
      ['shared/intersection.c', 2],
      ['shared/deep/nested/intersection.c', 6],
      ['a/only.c', 3],
      ['a/deep/nested/only.c', 7],
      ['b/only.c', 4],
      ['b/deep/nested/only.c', 8],
      ['nobody.c', 5],
    ]);
    const manifests = new Map([
      ['261022-a-subject', ['shared/**', 'a/**']],
      ['261022-b-subject', ['shared/**', 'b/**']],
    ]);
    const split = splitBatchPaths(perFile, ['261022-a-subject', '261022-b-subject'], manifests);
    const a = split.assignments.get('261022-a-subject') || new Map();
    const b = split.assignments.get('261022-b-subject') || new Map();
    return a.has('delivery/taskpackages/261022-a-subject/notes.md') &&
      a.has('a/only.c') && a.has('a/deep/nested/only.c') &&
      b.has('b/only.c') && b.has('b/deep/nested/only.c') &&
      !a.has('shared/intersection.c') && !b.has('shared/intersection.c') &&
      !a.has('shared/deep/nested/intersection.c') && !b.has('shared/deep/nested/intersection.c') &&
      split.unattributed.some(u => u.path === 'shared/intersection.c' && u.reason === 'ambiguous-manifest') &&
      split.unattributed.some(u => u.path === 'shared/deep/nested/intersection.c' && u.reason === 'ambiguous-manifest') &&
      split.unattributed.some(u => u.path === 'nobody.c' && u.reason === 'no-manifest');
  });
  check('event-sha-union-not-replace', () => {
    const shas = buildCandidateShas([
      { sha: 'aaaa', sources: new Set(['commit-message']) },
      { sha: 'bbbb', sources: new Set(['merge-completed']) },
    ]);
    return shas.includes('aaaa') && shas.includes('bbbb') && shas.length === 2;
  });
  check('shared-sha-forces-batch', () => {
    return effectiveMergeVerdict('Merge 261022-a-subject (merge-driver)', '261022-a-subject', true) === 'batch' &&
      effectiveMergeVerdict('Merge 261022-a-subject (merge-driver)', '261022-a-subject', false) === 'named';
  });
  check('tier-column-classification', () => {
    const full = computeTier({ rigor: 'full', subsystem: 'firmware/main', changeManifest: ['firmware/main/x.c'] });
    const gov = computeTier({ rigor: 'full', subsystem: '.claude/scripts', changeManifest: ['.claude/scripts/x.mjs'] });
    const missing = computeTier({ subsystem: 'firmware/main' });
    const slice = computeTier({ rigor: 'lite', subsystem: 'server/gateway', origin: { workstream: 'ws' } });
    return full.value === 'full' && !full.governance &&
      gov.value === 'governance' && gov.governance &&
      missing.value === '' && missing.source === 'missing-rigor' &&
      slice.value === 'lite·拆包' && slice.slice;
  });
  check('lifecycle-window-unknown-not-zero', () => {
    const onlyStart = computeLifecycle([{ eventType: 'pod-started', timestamp: '2026-01-01T00:00:00Z' }], 1000);
    const none = computeLifecycle([], 1000);
    const onlyClose = computeLifecycle([{ eventType: 'pod-closed', timestamp: '2026-01-01T00:00:00Z' }], 1000);
    return onlyStart.observationWindowMs === null && onlyStart.unknownReason === 'no-closeout' &&
      none.observationWindowMs === null && none.unknownReason === 'no-lifecycle-data' &&
      onlyClose.observationWindowMs === null && onlyClose.unknownReason === 'no-start';
  });
  check('waiting-negative-unknown', () => {
    const r = computeLifecycle([
      { eventType: 'pod-started', timestamp: '2026-01-01T00:00:00Z' },
      { eventType: 'pod-closed', timestamp: '2026-01-01T00:01:00Z' },
    ], 120000);
    return r.observationWindowMs === 60000 && r.waitingMs === null && r.unknownReason === 'negative-window';
  });
  check('restamp-dedupe-triple-condition', () => {
    const ev = [
      { eventType: 'dispatch-completed', changeId: 'x', actor: 'builder', payload: { dispatchId: 'd1', durationMs: 1000, tokenUsage: { status: 'exact', total: 100 } } },
      { eventType: 'dispatch-completed', changeId: 'x', actor: 'builder', payload: { dispatchId: 'd1', durationMs: 0, tokenUsage: { status: 'exact', total: 100 } } },
      { eventType: 'dispatch-completed', changeId: 'x', actor: 'builder', payload: { dispatchId: 'd2', durationMs: 1000, tokenUsage: { status: 'exact', total: 100 } } },
      { eventType: 'dispatch-completed', changeId: 'x', actor: 'builder', payload: { dispatchId: 'd2', durationMs: 0, tokenUsage: { status: 'exact', total: 200 } } },
      { eventType: 'dispatch-completed', changeId: 'x', actor: 'builder', payload: { durationMs: 1000, tokenUsage: { status: 'exact', total: 300 } } },
    ];
    const r = dedupeDispatchEvents(ev);
    const tokens = sumTokens(r.events);
    return r.dedupe.restampedRows === 1 && r.dedupe.restampedToken === 100 &&
      r.events.filter(e => e.eventType === 'dispatch-completed').length === 4 &&
      r.dedupe.ambiguousIdGroups.length === 1 &&
      r.dedupe.withoutDispatchId === 1 &&
      tokens.total === 700;
  });
  check('root-cause-enum-closed', () => {
    const ok = classifyFixerRootCause('fixer', 'acceptance-gap');
    const bad = classifyFixerRootCause('fixer', 'nope');
    const non = classifyFixerRootCause('builder', 'environment');
    const absent = classifyFixerRootCause('fixer', undefined);
    return ok.rootCause === 'acceptance-gap' && ok.rootCauseStatus === 'exact' &&
      bad.rootCause === null && bad.rootCauseStatus === 'invalid' && !!bad.rootCauseNote &&
      non.rootCauseStatus === 'ignored-non-fixer' && !('rootCause' in non) &&
      Object.keys(absent).length === 0;
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
