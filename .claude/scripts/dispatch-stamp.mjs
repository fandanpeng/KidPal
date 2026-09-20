#!/usr/bin/env node
// 每次 planner Agent() dispatch 收工后调用，把 harness 回传值落盘。
// Pre-cutoff: 程序内部 fs-append 一行到 delivery/taskpackages/<id>/dispatch-log.jsonl。
// Post-cutoff (EVENTS_SCHEMA_CUTOFF): writes dispatch-completed event to pod-events.jsonl
//   via pod-events-append.mjs. Does NOT write dispatch-log.jsonl for post-cutoff Pods (F1).
// 对偶 portfolio-reconcile.mjs（程序内 fs 写，非 shell 重定向）。
//
// PACKAGE: gov-review-retro-convergence (Package B, FROZEN 2026-07-25)
//
// 用法（post-cutoff structured tokens）：
//   node .claude/scripts/dispatch-stamp.mjs \
//     --dispatch-id <id> --actor <name> --phase <phase> \
//     --duration-ms <ms> --tool-uses <n> --terminal-status <status> \
//     --token-status exact|unavailable \
//     [--token-input <n>] [--token-output <n>] [--token-cache-read <n>] \
//     [--token-cache-write <n>] [--token-total <n>] [--token-source <s>] \
//     [--token-reason <s>] [--task-package-id <id>]
//
// 用法（legacy, pre-cutoff）：
//   node .claude/scripts/dispatch-stamp.mjs \
//     --dispatch-id <id> --actor <name> --phase <phase> \
//     --duration-ms <ms> --tokens <n> --tool-uses <n> \
//     --terminal-status <status> [--task-package-id <id>]

import { appendFileSync, existsSync, mkdirSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { EVENTS_SCHEMA_CUTOFF, appendPodEvent } from './pod-events-append.mjs';
import { CHANGE_ID_PREFIX_RE } from './lib/changeid-shared.mjs'; // T1 单源收敛（260906-gov-review-arq-governance-batch）

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function flagVal(name) {
  const i = process.argv.indexOf(name);
  return i >= 0 ? process.argv[i + 1] : undefined;
}

function required(name) {
  const v = flagVal(name);
  if (!v) {
    process.stderr.write(`[dispatch-stamp] missing required flag: ${name}\n`);
    process.exit(2);
  }
  return v;
}

function taskPackageDir() {
  const explicit = flagVal('--task-package-id');
  if (explicit) return resolve(projectDir, 'delivery', 'taskpackages', explicit);

  const current = resolve(projectDir, 'delivery', 'taskpackages', 'current', 'taskpackage.json');
  if (!existsSync(current)) return null;
  try {
    const tp = JSON.parse(readFileSync(current, 'utf8').replace(/^\uFEFF/, ''));
    return tp.changeId ? resolve(projectDir, 'delivery', 'taskpackages', tp.changeId) : null;
  } catch {
    return null;
  }
}

// 260918-gov-review-closeout-chain-friction T2\u2460\uFF08FROZEN 2026-09-19\uFF09\uFF1A\u89E3\u6790\u987B**\u540C\u65F6\u56DE\u62A5\u6765\u6E90**\u2014\u2014
// \u672C\u51FD\u6570\u6709\u4E24\u6761\u8DEF\u5F84\uFF08\u663E\u5F0F flag / \u8BFB current/ \u6307\u9488\uFF09\uFF0C\u8C03\u7528\u70B9\u8981\u5982\u5B9E\u56DE\u663E\u300C\u672C\u6B21\u5B9E\u9645\u6765\u81EA\u54EA\u6761\u300D\uFF0C
// \u4E0D\u5F97\u56FA\u5B9A\u5199\u540E\u8005\uFF08R1-F6\uFF09\u3002\u6545\u5185\u90E8\u8FD4\u56DE { changeId, source }\uFF0C\u65E7\u7B7E\u540D\u4FDD\u7559\u4E3A\u8584\u5C01\u88C5\u3002
function resolveChangeIdWithSource() {
  const explicit = flagVal('--task-package-id');
  if (explicit) return { changeId: explicit, source: '--task-package-id' };

  const current = resolve(projectDir, 'delivery', 'taskpackages', 'current', 'taskpackage.json');
  if (!existsSync(current)) return { changeId: null, source: 'delivery/taskpackages/current/taskpackage.json\uFF08\u4E0D\u5B58\u5728\uFF09' };
  try {
    const tp = JSON.parse(readFileSync(current, 'utf8').replace(/^\uFEFF/, ''));
    return { changeId: tp.changeId || null, source: 'delivery/taskpackages/current/taskpackage.json' };
  } catch {
    return { changeId: null, source: 'delivery/taskpackages/current/taskpackage.json\uFF08\u4E0D\u53EF\u89E3\u6790\uFF09' };
  }
}

function resolveChangeId() {
  return resolveChangeIdWithSource().changeId;
}

function isPostEventsCutoff(changeId) {
  const m = CHANGE_ID_PREFIX_RE.exec(String(changeId));
  if (!m) return false;
  const prefix = m[1]; // YYMMDD
  const cutoffCompact = EVENTS_SCHEMA_CUTOFF.replace(/-/g, '').slice(2); // '2026-07-25' → '260725'
  return prefix >= cutoffCompact;
}

function optionalNonNegativeInteger(name) {
  const raw = flagVal(name);
  if (raw === undefined) return undefined;
  if (!/^\d+$/.test(raw) || !Number.isSafeInteger(Number(raw))) {
    process.stderr.write(`[dispatch-stamp] ${name} must be a non-negative safe integer\n`);
    process.exit(2);
  }
  return Number(raw);
}

function buildTokenUsage() {
  // Check for structured token flags first
  const tokenStatus = flagVal('--token-status');
  if (tokenStatus) {
    if (tokenStatus === 'exact') {
      const tu = { status: 'exact', source: flagVal('--token-source') || 'harness' };
      const input = optionalNonNegativeInteger('--token-input');
      const output = optionalNonNegativeInteger('--token-output');
      const cacheRead = optionalNonNegativeInteger('--token-cache-read');
      const cacheWrite = optionalNonNegativeInteger('--token-cache-write');
      const reportedTotal = optionalNonNegativeInteger('--token-total');
      if (input !== undefined) tu.input = input;
      if (output !== undefined) tu.output = output;
      if (cacheRead !== undefined) tu.cacheRead = cacheRead;
      if (cacheWrite !== undefined) tu.cacheWrite = cacheWrite;
      if (input === undefined && output === undefined && reportedTotal === undefined) {
        process.stderr.write('[dispatch-stamp] exact token usage requires --token-total or input/output values\n');
        process.exit(2);
      }
      if (input !== undefined && output !== undefined) {
        const computedTotal = input + output;
        if (reportedTotal !== undefined && reportedTotal !== computedTotal) {
          process.stderr.write('[dispatch-stamp] --token-total must equal input + output when both are provided\n');
          process.exit(2);
        }
        tu.total = computedTotal;
      } else if (reportedTotal !== undefined) {
        tu.total = reportedTotal;
      }
      if (tu.total === 0) tu.note = 'harness-reported-zero';
      return tu;
    } else if (tokenStatus === 'unavailable') {
      return { status: 'unavailable', reason: flagVal('--token-reason') || 'harness-did-not-report-token-usage' };
    } else {
      process.stderr.write(`[dispatch-stamp] --token-status must be "exact" or "unavailable"\n`);
      process.exit(2);
    }
  }

  // Legacy: --tokens <n> (pre-cutoff backward compat)
  const tokensStr = flagVal('--tokens');
  if (tokensStr !== undefined) {
    if (!/^\d+$/.test(tokensStr) || !Number.isSafeInteger(Number(tokensStr))) {
      process.stderr.write('[dispatch-stamp] --tokens must be a non-negative safe integer\n');
      process.exit(2);
    }
    const tokens = Number(tokensStr);
    if (tokens > 0) {
      return { status: 'exact', total: tokens, source: 'harness' };
    } else {
      // n===0 → unavailable (never encode unknown as zero)
      return { status: 'unavailable', reason: 'legacy-zero-value' };
    }
  }

  // Neither provided — unavailable
  return { status: 'unavailable', reason: 'no-token-flags-provided' };
}

function run() {
  const dispatchId = required('--dispatch-id');
  const actor = required('--actor');
  const phase = required('--phase');
  const durationMs = parseInt(required('--duration-ms'), 10);
  const toolUses = parseInt(required('--tool-uses'), 10);
  const terminalStatus = required('--terminal-status');
  const ts = new Date().toISOString();

  if (isNaN(durationMs) || isNaN(toolUses)) {
    process.stderr.write('[dispatch-stamp] duration-ms, tool-uses must be integers\n');
    process.exit(2);
  }

  const resolved = resolveChangeIdWithSource();
  const changeId = resolved.changeId;
  const tpDir = taskPackageDir();

  // T2①（FROZEN §2）：**始终**回显所解析 changeId 与其**实际来源**（错与对在输出上不再无差别）。
  process.stderr.write(`dispatch-stamp: resolved changeId = ${changeId || '(none)'} (from ${resolved.source})\n`);

  // T2②（FROZEN §2，含 R1-F6 的交互定义）：`--expect-change-id` 为**可选**硬校验开关——
  // 传了则「不符」与「无上下文」两态均 fail-loud（后者因调用方已明确表达「本包应在场」的期望）；
  // **不传则行为与改前完全一致**（含下方 no-context → exit 0）。
  const expectChangeId = flagVal('--expect-change-id') || null;
  if (expectChangeId) {
    if (!changeId) {
      process.stderr.write(`dispatch-stamp: --expect-change-id ${expectChangeId} 在场但无 TaskPackage 上下文（解析值 = none，来源 ${resolved.source}）——fail-loud\n`);
      process.exit(2);
    }
    if (changeId !== expectChangeId) {
      process.stderr.write(`dispatch-stamp: changeId 不符——解析值 ${changeId}（来源 ${resolved.source}）≠ 期望值 ${expectChangeId}\n`);
      process.exit(2);
    }
  }

  if (!tpDir || !changeId) {
    process.stderr.write('[dispatch-stamp] no TaskPackage context; skipping dispatch stamp\n');
    process.exit(0);
  }

  mkdirSync(tpDir, { recursive: true });

  const tokenUsage = buildTokenUsage();
  // T2（260920-gov-review-reader-scripts-align，FROZEN 2026-09-20）：**可选** `--model <id>`——
  // 传了则写入 payload.model（「实际生效模型」，MODEL-TIER 条款 `planner.md:59` 的落点）；
  // 不传则 **不带该键**（向后兼容——既有调用方行为零变更，且不制造「0 充数」式伪值）。
  const model = flagVal('--model') || null;

  if (isPostEventsCutoff(changeId)) {
    // Post-cutoff: write to pod-events.jsonl ONLY (F1: no dispatch-log.jsonl)
    try {
      const result = appendPodEvent(changeId, {
        eventType: 'dispatch-completed',
        actor,
        source: 'dispatch-stamp.mjs',
        payload: {
          dispatchId,
          phase,
          durationMs,
          toolUses,
          terminalStatus,
          tokenUsage,
          ...(model ? { model } : {}),
        },
      });
      process.stderr.write(`[dispatch-stamp] ${dispatchId}: ${actor}/${phase} ${durationMs}ms -> pod-events.jsonl\n`);
    } catch (e) {
      process.stderr.write(`[dispatch-stamp] pod-events append error: ${e.message}\n`);
      process.exit(1);
    }
  } else {
    // Pre-cutoff: legacy dispatch-log.jsonl behavior
    const tokens = tokenUsage.status === 'exact' ? (tokenUsage.total || 0) : 0;
    const record = {
      dispatchId,
      actor,
      phase,
      durationMs,
      tokens,
      toolUses,
      terminalStatus,
      ts,
    };

    const logPath = resolve(tpDir, 'dispatch-log.jsonl');
    appendFileSync(logPath, `${JSON.stringify(record)}\n`);
    process.stderr.write(`[dispatch-stamp] ${dispatchId}: ${actor}/${phase} ${durationMs}ms ${tokens}tok -> ${logPath}\n`);
  }

  process.exit(0);
}

run();
