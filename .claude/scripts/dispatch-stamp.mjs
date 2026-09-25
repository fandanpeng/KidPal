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
// **T2（260920-gov-review-dispatch-telemetry-source，FROZEN v1.0）**：post-cutoff 分支另读
// 会话 transcript（见 lib/transcript-telemetry.mjs）自动派生 token 四路分项、**实际生效模型**
// 与主线程累计消耗。此时 `--dispatch-id` **必须传 harness 回传的 agentId**——它是 transcript 的
// 连接键。取值优先级：transcript → 旗标 → unavailable。取不到时**如实降级**，绝不禁用派发。
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
import { readDispatchTelemetry, readMainThreadUsage } from './lib/transcript-telemetry.mjs'; // T2（260920-gov-review-dispatch-telemetry-source）

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

// T2（260920-gov-review-dispatch-telemetry-source，FROZEN v1.0）：harness 回传的**单值**是哪个旗标
// ——post-cutoff = `--token-total`，legacy = `--tokens`。点名它是为了与**四路旗标**
// （`--token-input` / `--token-output` / `--token-cache-read` / `--token-cache-write`）区分开：
// 那四路是「调用方手喂分项」的通道，与 harness 回传无关，两者不是一回事（R2-N3 折入）。
function harnessReportedSingle() {
  const total = optionalNonNegativeInteger('--token-total');
  if (total !== undefined) return total;
  const legacy = flagVal('--tokens');
  if (legacy !== undefined && /^\d+$/.test(legacy)) return Number(legacy);
  return undefined;
}

// harness 回传的 agentId 形态：17 位十六进制（实测 `aaecbb47025725868` / `ae098421d664d6298`）。
// 历史自造标签（`builder-r1` / `verify-r2` / `integrator-closeout`）不匹配 → 触发 advisory。
const AGENT_ID_SHAPE = /^[0-9a-f]{17}$/i;

// 主线程累计消耗的 payload 段。取不到时如实降级，**绝不禁用派发**。
function mainThreadUsagePayload(sessionId, projectDirName) {
  const m = readMainThreadUsage(sessionId, projectDirName);
  if (!m.found) return { status: 'unavailable', reason: m.reason };
  return {
    source: 'transcript',
    input: m.input,
    output: m.output,
    cacheRead: m.cacheRead,
    cacheWrite: m.cacheWrite,
    total: m.total,
    model: m.model,
    messageCount: m.messageCount,
  };
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
  // T7（260924-gov-review-gate-parse-teeth，OQ-1=(c)）：校验结果以 `expectCheck` 值入事件 payload——
  // 含 absent（旗标缺省，既往只能靠污染症状倒推的盲区）、match、mismatch、no-context 四态；
  // 判定语义零变更（exit 行为逐字不动），纯事件面可观测性增量。
  const expectChangeId = flagVal('--expect-change-id') || null;
  let expectCheck = 'absent';
  if (expectChangeId) {
    if (!changeId) {
      expectCheck = 'no-context';
      process.stderr.write(`dispatch-stamp: --expect-change-id ${expectChangeId} 在场但无 TaskPackage 上下文（解析值 = none，来源 ${resolved.source}）——fail-loud\n`);
      process.exit(2);
    }
    if (changeId !== expectChangeId) {
      expectCheck = 'mismatch';
      process.stderr.write(`dispatch-stamp: changeId 不符——解析值 ${changeId}（来源 ${resolved.source}）≠ 期望值 ${expectChangeId}\n`);
      process.exit(2);
    }
    expectCheck = 'match';
  }

  if (!tpDir || !changeId) {
    process.stderr.write('[dispatch-stamp] no TaskPackage context; skipping dispatch stamp\n');
    process.exit(0);
  }

  mkdirSync(tpDir, { recursive: true });

  const flagTokenUsage = buildTokenUsage();
  // T2（260920-gov-review-reader-scripts-align，FROZEN 2026-09-20）：**可选** `--model <id>`——
  // 传了则写入 payload.model；不传则 **不带该键**（向后兼容——既有调用方行为零变更）。
  // T2（260920-gov-review-dispatch-telemetry-source，FROZEN v1.0）：该旗标**降为回退值**——
  // 它只能表达「**请求的**模型」（槽位名），transcript 里的 `message.model` 才是「**生效的**模型」。
  const flagModel = flagVal('--model') || null;

  if (isPostEventsCutoff(changeId)) {
    // Post-cutoff: write to pod-events.jsonl ONLY (F1: no dispatch-log.jsonl)
    try {
      // ── T2（FROZEN v1.0）：transcript 派生。**只在 post-cutoff 分支**——pre-cutoff 走
      // legacy dispatch-log.jsonl，数据模型不同，保持零变更以压风险面。──
      const telemetry = readDispatchTelemetry(dispatchId);

      // 取值优先级（FROZEN §2 T2）：transcript → 旗标 → unavailable。
      // **命中时 `status` 必须为 `'exact'`**——它是既有消费方（loop-metrics.mjs:242 /
      // pod-events-aggregate.mjs:76）的**唯一分桶键**，它们只判 status、不读 source；
      // 只写 source 会让新源落进 unavailable 桶、覆盖度静默不涨且无门报红（R1-S1）。
      const tokenUsage = telemetry.found
        ? {
            status: 'exact',
            source: 'transcript',
            input: telemetry.input,
            output: telemetry.output,
            cacheRead: telemetry.cacheRead,
            cacheWrite: telemetry.cacheWrite,
            total: telemetry.total,
          }
        : flagTokenUsage;

      // harness 单值交叉核对（可选，E2 那类对账的证据基础）。注意该「单值」= `--token-total`
      // （legacy = `--tokens`），**不是四路旗标**（R2-N3）。
      const harnessReported = harnessReportedSingle();
      if (harnessReported !== undefined) tokenUsage.harnessReported = harnessReported;

      // 实际生效模型：transcript 优先，`--model` 旗标回退；`modelSource` 标明取自哪条路。
      const effectiveModel = telemetry.found && telemetry.model ? telemetry.model : null;
      const model = effectiveModel || flagModel;
      const modelSource = effectiveModel ? 'transcript' : (flagModel ? 'flag' : null);

      // 主线程自身消耗（子项 c）：累计检查点语义——retro 取同包首尾两次 stamp 做差即得区间消耗。
      // **仅命中派发 transcript 时带该键**：未命中时连 sessionId 都无从得知（连接键就是 agentId），
      // 此时凭空写一个 unavailable 段只会给老形态事件添噪。承本件既有约定——「不传则不带该键」
      // （见上方 `--model` 注释），也正是 §5 #4「逐字段一致」得以**字面成立**的前提。
      const mainThreadUsage = telemetry.found
        ? mainThreadUsagePayload(telemetry.sessionId, telemetry.projectDirName)
        : null;

      // 连接键未命中且 id 不具 agentId 形态 → advisory（fail-soft，不改 exit code）。
      // 这正是历史上 51 条自造标签（`builder-r1` 类）的可观测化。
      if (!telemetry.found && !AGENT_ID_SHAPE.test(dispatchId)) {
        process.stderr.write(
          `[dispatch-stamp] advisory: --dispatch-id "${dispatchId}" 不像 harness agentId（17 位十六进制）` +
          `且未命中 transcript（${telemetry.reason}）——本次遥测将不可归因。planner.md [DISPATCH-STAMP] 要求传 harness 回传的 agentId。\n`
        );
      }

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
          ...(modelSource ? { modelSource } : {}),
          ...(mainThreadUsage ? { mainThreadUsage } : {}),
          // T7（260924-gov-review-gate-parse-teeth，OQ-1=(c)）：上下文来源 + expect 校验结果（additive 双键；
          // 量化触发器入 ARQ-260913 追注：absent ≥1 → 启动 (b) 闸级强制批 / 累计 30 全 match → settled）。
          contextSource: resolved.source,
          expectCheck,
        },
      });
      process.stderr.write(`[dispatch-stamp] ${dispatchId}: ${actor}/${phase} ${durationMs}ms -> pod-events.jsonl (tokens: ${tokenUsage.status}/${tokenUsage.source || '-'}${model ? `, model: ${model} (${modelSource})` : ''})\n`);
    } catch (e) {
      process.stderr.write(`[dispatch-stamp] pod-events append error: ${e.message}\n`);
      process.exit(1);
    }
  } else {
    // Pre-cutoff: legacy dispatch-log.jsonl behavior
    const tokens = flagTokenUsage.status === 'exact' ? (flagTokenUsage.total || 0) : 0;
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
