#!/usr/bin/env node
// On-demand pod-events aggregation: reads pod-events.jsonl for a given changeId,
// computes token totals (exact-only), coverage, escalation summary, friction summary.
// Skips malformed lines with stderr diagnostic (fail-open).
//
// OWNER: architect (G3 enforcement layer — advisory/non-authority)
// PACKAGE: gov-review-retro-convergence (Package B, FROZEN 2026-07-25)
//
// Usage:
//   node .claude/scripts/pod-events-aggregate.mjs --change-id <id>
//   node .claude/scripts/pod-events-aggregate.mjs  (uses current TaskPackage)
//   node .claude/scripts/pod-events-aggregate.mjs --self-check
//
// Output: JSON to stdout

import { existsSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { echoResolution, resolveCurrentTaskPackage } from './lib/current-pointer-shared.mjs'; // T6 回显齿（260924-gov-review-gate-parse-teeth）
import { VALID_EVENT_TYPES } from './pod-events-append.mjs';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function flagVal(name) {
  const i = process.argv.indexOf(name);
  return i >= 0 ? process.argv[i + 1] : undefined;
}

function resolveChangeId() {
  const explicit = flagVal('--change-id');
  if (explicit) {
    echoResolution('pod-events-aggregate', explicit, '--change-id 旗标');
    return explicit;
  }

  const resolved = resolveCurrentTaskPackage('pod-events-aggregate', projectDir);
  return resolved.found ? resolved.changeId : null;
}

function readEventsFile(eventsPath) {
  if (!existsSync(eventsPath)) return { events: [], malformedLines: 0 };

  const text = readFileSync(eventsPath, 'utf8');
  const lines = text.split(/\r?\n/).filter(l => l.trim());
  const events = [];
  let malformedLines = 0;

  for (const line of lines) {
    try {
      events.push(JSON.parse(line));
    } catch {
      malformedLines++;
      process.stderr.write(`[pod-events-aggregate] skipping malformed line: ${line.slice(0, 80)}...\n`);
    }
  }

  return { events, malformedLines };
}

function isPayloadObject(payload) {
  return payload !== null && typeof payload === 'object' && !Array.isArray(payload);
}

function schemaWarning(warnings, event, code, message) {
  warnings.push({
    eventId: event && typeof event === 'object' ? (event.eventId || event.id || null) : null,
    eventType: event && typeof event === 'object' ? (event.eventType || null) : null,
    code,
    message,
  });
}

function validateEvents(events) {
  const validEvents = [];
  const schemaWarnings = [];
  for (const event of events) {
    const envelopeObject = event !== null && typeof event === 'object' && !Array.isArray(event);
    const eventType = envelopeObject ? event.eventType : null;
    if (!envelopeObject || !VALID_EVENT_TYPES.has(eventType)) {
      schemaWarning(schemaWarnings, event, 'unknown-event-type', `事件类型不在闭集内：${String(eventType ?? '(missing)')}；计入 eventsTotal，不进入专项聚合`);
    }
    if (!envelopeObject || !isPayloadObject(event.payload)) {
      schemaWarning(schemaWarnings, event, 'invalid-payload', 'payload 必须是非 null、非数组的对象；本事件不进入专项聚合');
      continue;
    }
    if (!VALID_EVENT_TYPES.has(eventType)) continue;
    validEvents.push(event);
    if ((eventType === 'human-decision' || eventType === 'human-correction') && event.actor !== 'human') {
      schemaWarning(schemaWarnings, event, 'invalid-human-actor', `${eventType} 的 actor 必须为 human，实际为 ${String(event.actor ?? '(missing)')}`);
    }
    if (eventType === 'agent-escalation-resolved'
      && !(typeof event.payload.refEventId === 'string' && event.payload.refEventId.trim())
      && !(typeof event.payload.resolves === 'string' && event.payload.resolves.trim())) {
      schemaWarning(schemaWarnings, event, 'missing-closure-ref', 'agent-escalation-resolved 缺少 payload.refEventId（存量 resolves 别名亦可）');
    }
  }
  return { validEvents, schemaWarnings };
}

function aggregateTokens(events) {
  const dispatches = events.filter(e => e.eventType === 'dispatch-completed');
  let dispatchesTotal = dispatches.length;
  let dispatchesWithTokenData = 0;
  let dispatchesUnavailable = 0;
  let totalInput = 0;
  let totalOutput = 0;
  let totalCacheRead = 0;
  let totalCacheWrite = 0;
  let totalTokens = 0;

  for (const d of dispatches) {
    const tu = d.payload && d.payload.tokenUsage;
    if (!tu) {
      dispatchesUnavailable++;
      continue;
    }
    if (tu.status === 'exact') {
      dispatchesWithTokenData++;
      if (typeof tu.input === 'number') totalInput += tu.input;
      if (typeof tu.output === 'number') totalOutput += tu.output;
      if (typeof tu.cacheRead === 'number') totalCacheRead += tu.cacheRead;
      if (typeof tu.cacheWrite === 'number') totalCacheWrite += tu.cacheWrite;
      if (typeof tu.total === 'number') totalTokens += tu.total;
    } else if (tu.status === 'unavailable') {
      dispatchesUnavailable++;
    } else {
      // Unknown status — treat as unavailable, fail-open
      dispatchesUnavailable++;
    }
  }

  return {
    dispatchesTotal,
    dispatchesWithTokenData,
    dispatchesUnavailable,
    tokenTotals: {
      input: totalInput,
      output: totalOutput,
      cacheRead: totalCacheRead,
      cacheWrite: totalCacheWrite,
      total: totalTokens,
    },
  };
}

// T7（260908-gov-review-input-surface-frontload）· 长尾可观测面：per-actor 挂钟 / toolUses 分布 + 长尾计数
// 向后兼容：新只增不改既有输出键（PROPOSAL §3）。缺值如实计数为 durValid 之外的缺口，不补 0（对齐
// dispatch-stamp token 政策「绝不传 0 充数」）。阈值与 PROPOSAL §1 P1 / DEV-GOVERNANCE 起包格式期望前置
// 第 6 条同源 = 40min；本列纯 advisory（DP-D4，T7 维持 friction 既有定性），绝不当 merge 门。
const LONGTAIL_MIN_MS = 40 * 60 * 1000; // ≥40min 记长尾（与 §1 P1 阈值同源，不得两值）

function median(xs) {
  if (xs.length === 0) return null;
  const s = [...xs].sort((a, b) => a - b);
  const mid = Math.floor(s.length / 2);
  return s.length % 2 ? s[mid] : (s[mid - 1] + s[mid]) / 2;
}

function aggregateActors(events) {
  const dispatches = events.filter(e => e.eventType === 'dispatch-completed');
  const byActor = new Map();
  for (const d of dispatches) {
    const actor = d.actor || 'unknown';
    if (!byActor.has(actor)) byActor.set(actor, []);
    byActor.get(actor).push(d);
  }
  const rows = [];
  for (const [actor, ds] of byActor.entries()) {
    const withDur = ds.filter(d => typeof d.payload?.durationMs === 'number');
    const durations = withDur.map(d => d.payload.durationMs);
    const toolUses = withDur.map(d => d.payload?.toolUses).filter(n => typeof n === 'number');
    const longtail = withDur
      .filter(d => d.payload.durationMs >= LONGTAIL_MIN_MS)
      .map(d => ({
        eventId: d.eventId,
        phase: d.payload?.phase || null,
        durationMs: d.payload.durationMs,
        toolUses: typeof d.payload?.toolUses === 'number' ? d.payload.toolUses : null,
      }))
      .sort((a, b) => b.durationMs - a.durationMs);
    rows.push({
      actor,
      dispatches: ds.length,
      withDurationMs: withDur.length,
      durationMsMissing: ds.length - withDur.length,
      durationMsTotal: durations.reduce((a, b) => a + b, 0),
      durationMsMedian: median(durations),
      toolUsesMedian: median(toolUses),
      longtailCount: longtail.length,
      longtail,
    });
  }
  rows.sort((a, b) => b.durationMsTotal - a.durationMsTotal);
  return {
    longtailThresholdMin: LONGTAIL_MIN_MS / 60000,
    dispatchesTotal: dispatches.length,
    actors: rows,
  };
}

// T5（260920-gov-review-dispatch-telemetry-source，FROZEN v1.0）· 生效模型覆盖度 + token 来源分布。
// **只增键、不改既有键**（承本件 T7 同款纪律，既有消费方不破）。
// `bySource` 是 transcript 通道失效的**机械观察点**（R2-C3 折入）：命中率归零 = 该表里
// `transcript` 连续多个包恒 0——cadence 复盘读它，而非靠人眼「留意」。
function aggregateModels(events) {
  const dispatches = events.filter(e => e.eventType === 'dispatch-completed');
  const byModel = {};
  const bySource = {};
  let withModel = 0;
  for (const d of dispatches) {
    const m = d.payload && d.payload.model;
    if (typeof m === 'string' && m) {
      withModel++;
      byModel[m] = (byModel[m] || 0) + 1;
    }
    const tu = d.payload && d.payload.tokenUsage;
    const s = tu ? (tu.source || '(none)') : '(no-tokenUsage)';
    bySource[s] = (bySource[s] || 0) + 1;
  }
  return { dispatchesTotal: dispatches.length, withModel, byModel, bySource };
}

// T5 · 主线程自身消耗（子项 c）的**累计检查点序列**。语义 = 截至各次 stamp 的累计值，
// 故 retro 取同包首尾两点做差即得该包期间主线程消耗（`cumulativeDelta` 即此差）。
function aggregateMainThread(events) {
  const checkpoints = events
    .filter(e => e.eventType === 'dispatch-completed'
      && e.payload && e.payload.mainThreadUsage
      && e.payload.mainThreadUsage.source === 'transcript')
    .map(e => {
      const m = e.payload.mainThreadUsage;
      return {
        eventId: e.eventId || null,
        ts: e.timestamp || null,
        input: m.input, output: m.output,
        cacheRead: m.cacheRead, cacheWrite: m.cacheWrite,
        total: m.total, model: m.model || null,
      };
    })
    .sort((a, b) => String(a.ts).localeCompare(String(b.ts)));
  const first = checkpoints.length ? checkpoints[0] : null;
  const last = checkpoints.length ? checkpoints[checkpoints.length - 1] : null;
  return {
    checkpointsTotal: checkpoints.length,
    first,
    last,
    cumulativeDelta: first && last && checkpoints.length > 1
      ? {
          input: last.input - first.input,
          output: last.output - first.output,
          cacheRead: last.cacheRead - first.cacheRead,
          cacheWrite: last.cacheWrite - first.cacheWrite,
          total: last.total - first.total,
        }
      : null,
    checkpoints,
  };
}

function aggregateEscalations(events, schemaWarnings) {
  const escalations = events.filter(e => e.eventType === 'agent-escalation');
  const closures = events.filter(e => e.eventType === 'agent-escalation-resolved' || e.eventType === 'human-decision');
  const resolvedIds = new Set();

  for (const closure of closures) {
    const ref = typeof closure.payload.refEventId === 'string' && closure.payload.refEventId.trim()
      ? closure.payload.refEventId.trim()
      : (typeof closure.payload.resolves === 'string' && closure.payload.resolves.trim() ? closure.payload.resolves.trim() : null);
    // 无引用 human-decision 是普通决定；缺关联告警只针对 resolved 事件，由 validateEvents 负责。
    if (!ref) continue;
    const matches = escalations.filter(e => typeof e.eventId === 'string' && e.eventId.startsWith(ref));
    if (matches.length === 1) {
      resolvedIds.add(matches[0].eventId);
      continue;
    }
    const anyEventMatches = events.filter(e => typeof e.eventId === 'string' && e.eventId.startsWith(ref));
    const reason = matches.length > 1 ? 'ambiguous-prefix'
      : (anyEventMatches.length > 0 ? 'not-an-escalation' : 'no-match');
    schemaWarning(schemaWarnings, closure, 'invalid-closure-ref', `闭合引用 ${ref} 无效（${reason}）；不闭合任何 escalation`);
  }

  const byCategory = {};
  const unresolved = [];

  for (const esc of escalations) {
    const cat = (esc.payload && esc.payload.category) || 'unknown';
    byCategory[cat] = (byCategory[cat] || 0) + 1;
    if (!resolvedIds.has(esc.eventId)) {
      unresolved.push({
        eventId: esc.eventId,
        category: cat,
        summary: esc.payload && esc.payload.summary,
        severity: esc.payload && esc.payload.severity,
      });
    }
  }

  return {
    total: escalations.length,
    resolved: resolvedIds.size,
    unresolved,
    byCategory,
  };
}

function aggregateFriction(events) {
  const frictions = events.filter(e => e.eventType === 'friction');
  const byType = {};
  // T4（260907-gov-review-arq-governance-batch-2，ARQ-260907-friction-event-payload-schema）：
  // 残缺 friction 事件显式告警列——payload 空对象 / 缺 frictionType / 缺 summary 三判据，命中即入列
  //（含 changeId + eventId 侧定位）。既有 unknown 兜底保留（非标键仍归 unknown，残缺件同时出现在
  // malformedFrictions——告警与归档双轨），不再对空 payload 静默吞过（loop-retro GC3 实证：55a43813
  // 空 payload 内容机器侧不可恢复 = 实际证据丢失）。
  const malformedFrictions = [];

  for (const f of frictions) {
    const ft = (f.payload && f.payload.frictionType) || 'unknown';
    byType[ft] = (byType[ft] || 0) + 1;
    const payload = f.payload && typeof f.payload === 'object' && !Array.isArray(f.payload) ? f.payload : null;
    const missing = [];
    if (!payload || Object.keys(payload).length === 0) missing.push('empty-payload');
    else {
      if (!payload.frictionType) missing.push('missing-frictionType');
      if (!payload.summary) missing.push('missing-summary');
    }
    if (missing.length > 0) {
      malformedFrictions.push({ eventId: f.eventId || f.id || null, ts: f.ts || f.timestamp || null, missing });
    }
  }

  return {
    total: frictions.length,
    byType,
    malformedFrictions,
  };
}

function aggregateEvents(events, changeId, malformedLines = 0) {
  const { validEvents, schemaWarnings } = validateEvents(events);
  return {
    ok: true,
    changeId,
    eventsTotal: events.length,
    malformedLines,
    tokens: aggregateTokens(validEvents),
    actors: aggregateActors(validEvents), // T7 新列（只增键，既有消费方不破）
    models: aggregateModels(validEvents), // T5 新列（只增键；bySource 兼作 transcript 通道失效观察点）
    mainThread: aggregateMainThread(validEvents), // T5 新列（只增键；子项 c 的累计检查点序列）
    escalations: aggregateEscalations(validEvents, schemaWarnings),
    friction: aggregateFriction(validEvents),
    schemaWarningsTotal: schemaWarnings.length,
    schemaWarnings,
  };
}

function selfCheck() {
  const checks = [];
  const ok = (name, pass, detail = '') => checks.push({ name, pass, detail });
  const esc = (eventId) => ({ eventId, eventType: 'agent-escalation', actor: 'planner', payload: { category: 'other', summary: eventId } });
  const events = [
    esc('11111111-1111-1111-1111-111111111111'),
    esc('22222222-2222-2222-2222-222222222222'),
    esc('33333333-3333-3333-3333-333333333333'),
    esc('44444444-4444-4444-4444-444444444444'),
    { eventId: 'r1', eventType: 'agent-escalation-resolved', actor: 'planner', payload: { refEventId: '11111111-1111-1111-1111-111111111111' } },
    { eventId: 'h1', eventType: 'human-decision', actor: 'human', payload: { refEventId: '22222222-2222-2222-2222-222222222222', decision: 'close' } },
    { eventId: 'r3', eventType: 'agent-escalation-resolved', actor: 'planner', payload: { resolves: '33333333' } },
    { eventId: 'r1-duplicate', eventType: 'agent-escalation-resolved', actor: 'planner', payload: { refEventId: '11111111' } },
    { eventId: 'h-plain', eventType: 'human-decision', actor: 'human', payload: { decision: 'ordinary decision' } },
    { eventId: 'u1', eventType: 'future-event', actor: 'planner', payload: { tokenUsage: { status: 'exact', total: 999 } } },
    { eventId: 'p1', eventType: 'dispatch-completed', actor: 'builder', payload: null },
    { eventId: 'h-bad-actor', eventType: 'human-correction', actor: 'planner', payload: { summary: 'wrong owner' } },
    { eventId: 'r-missing', eventType: 'agent-escalation-resolved', actor: 'planner', payload: {} },
    { eventId: 'r-invalid', eventType: 'agent-escalation-resolved', actor: 'planner', payload: { refEventId: 'does-not-exist' } },
    { eventId: 'f-empty', eventType: 'friction', actor: 'planner', payload: {} },
    { eventId: 'pod-empty-ok', eventType: 'pod-closed', actor: 'planner', payload: {} },
  ];
  const result = aggregateEvents(events, 'self-check');
  const codes = result.schemaWarnings.map(w => w.code);
  ok('夹具非空且 fail-open', result.ok === true && result.eventsTotal === events.length && result.escalations.total === 4);
  ok('canonical / human / legacy 短前缀均闭合', result.escalations.resolved === 3
    && result.escalations.unresolved.length === 1
    && result.escalations.unresolved[0].eventId.startsWith('44444444'));
  ok('重复闭合不虚增 distinct resolved', result.escalations.resolved === 3
    && events.filter(e => e.eventType === 'agent-escalation-resolved' && (e.payload.refEventId || e.payload.resolves)).length === 4);
  ok('普通 human-decision 不误闭合也不报缺引用', result.escalations.resolved === 3
    && !result.schemaWarnings.some(w => w.eventId === 'h-plain'));
  ok('未知类型计总数但不进专项聚合', codes.includes('unknown-event-type')
    && result.tokens.dispatchesTotal === 0 && result.tokens.tokenTotals.total === 0);
  ok('无效 payload 告警且不阻断', codes.includes('invalid-payload') && result.ok === true);
  ok('错误 human actor 有定位告警', result.schemaWarnings.some(w => w.eventId === 'h-bad-actor' && w.code === 'invalid-human-actor'));
  ok('缺失与无效闭合引用均告警', codes.includes('missing-closure-ref') && codes.includes('invalid-closure-ref'));
  ok('空 pod-closed payload 合法、空 friction 仍入 malformed', !result.schemaWarnings.some(w => w.eventId === 'pod-empty-ok')
    && result.friction.malformedFrictions.some(f => f.eventId === 'f-empty'));
  ok('schemaWarnings 计数与明细一致', result.schemaWarningsTotal === result.schemaWarnings.length && result.schemaWarningsTotal >= 5);

  const ambiguous = aggregateEvents([
    esc('abcdef01-0000-0000-0000-000000000000'), esc('abcdef02-0000-0000-0000-000000000000'),
    { eventId: 'r-ambiguous', eventType: 'human-decision', actor: 'human', payload: { refEventId: 'abcdef' } },
  ], 'self-check-ambiguous');
  ok('短前缀不唯一只告警不闭合', ambiguous.escalations.resolved === 0 && ambiguous.escalations.unresolved.length === 2
    && ambiguous.schemaWarnings.some(w => w.code === 'invalid-closure-ref' && w.message.includes('ambiguous-prefix')));

  const nonEscalation = aggregateEvents([
    esc('eeeeeeee-0000-0000-0000-000000000000'),
    { eventId: 'decision-target', eventType: 'human-decision', actor: 'human', payload: {} },
    { eventId: 'r-non-esc', eventType: 'agent-escalation-resolved', actor: 'planner', payload: { refEventId: 'decision-target' } },
  ], 'self-check-non-escalation');
  ok('引用非 escalation 不误闭合', nonEscalation.escalations.resolved === 0
    && nonEscalation.schemaWarnings.some(w => w.code === 'invalid-closure-ref' && w.message.includes('not-an-escalation')));

  const failed = checks.filter(c => !c.pass);
  for (const c of checks) console.log(`${c.pass ? 'PASS' : 'FAIL'}  ${c.name}${c.detail ? `  [${c.detail}]` : ''}`);
  console.log(`\npod-events-aggregate --self-check: ${checks.length - failed.length}/${checks.length} passed`);
  if (failed.length > 0) {
    console.error('SELF-CHECK RED');
    process.exit(1);
  }
  console.log('SELF-CHECK GREEN');
  process.exit(0);
}

function run() {
  const changeId = resolveChangeId();
  if (!changeId) {
    process.stderr.write('[pod-events-aggregate] no changeId context\n');
    console.log(JSON.stringify({ ok: false, reason: 'no-change-id' }));
    process.exit(0);
  }

  const eventsPath = resolve(projectDir, 'delivery', 'taskpackages', changeId, 'pod-events.jsonl');
  const { events, malformedLines } = readEventsFile(eventsPath);
  const result = aggregateEvents(events, changeId, malformedLines);
  for (const warning of result.schemaWarnings) {
    process.stderr.write(`[pod-events-aggregate] schema-warning ${warning.code} eventId=${warning.eventId || '(none)'} eventType=${warning.eventType || '(none)'}: ${warning.message}\n`);
  }

  console.log(JSON.stringify(result, null, 2));
  process.exit(0);
}

if (process.argv.includes('--self-check')) selfCheck();
else run();
