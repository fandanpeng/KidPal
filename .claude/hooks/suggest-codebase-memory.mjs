#!/usr/bin/env node
// suggest-codebase-memory.mjs — 告警级 advisory 钩子（非门）
//
// 治理声明：owner=architect · approver=human · G3（DEV-GOVERNANCE §6 强制执行层行覆盖）· 告警级非门（对偶先例 lint-descriptions.mjs）。
// 批准物：changes/arch/gov-review-codebase-memory-enforcement/decision-packet.md（§2 修正版受众闸 + §4 对账断言清单）。
// 本钩子是仓内首个逐工具调用的运行时模型上下文注入钩子（触发类无先例，定价见 packet §7-2 / 簇⑥-6d）。
//
// 行为规格（decision-packet §2 修正版 + §4 固化）：
//   - 不变量 I1（非阻塞）：一切路径 exit 0。JSON parse 失败 / 状态文件损坏 / 任何异常 → 静默 exit 0。
//     PreToolUse 分支永不 exit 2。
//   - 受众闸（v2 · 全名优先 + 受限克隆回退，与 enforce-scope.mjs:114-119 同构）：
//       ELIGIBLE_FULL = { builder, fixer, planner, portfolio-planner, acceptance-author, integrator }
//       CLONE_BASES   = { builder, fixer }（仅 Tier-2 克隆基座；planner/pp/aa/integrator 无克隆变体，不回退）
//       ① 无 agent_type（主线程/人类会话）→ eligible
//       ② lower(agent_type) ∈ ELIGIBLE_FULL → eligible（全名查表优先）
//       ③ split('-')[0] ∈ CLONE_BASES → eligible（仅 builder-* / fixer-*）
//       ④ 否则 → 静默 exit 0
//     拓扑注记：planner / portfolio-planner 现行恒以主线程跑（无 agent_type）→ 经分支①进入；
//     全名留在 ELIGIBLE_FULL 是防未来子代理化。acceptance-author 是真被 planner 派发的 subagent → 经分支②。
//   - 事件 A（PostToolUse 且 tool_name ∈ {Grep, Glob}）：代码面启发——路径白名单为主
//     （firmware/**、server/**、docs/contracts/**），文件型析取仅与索引根相交时生效；
//     .claude/**、docs/**、changes/**、.planning/**、*.md 显式静默。节流 grepReminders cap=2/session。
//   - 事件 B（PreToolUse 且 tool_name=Bash 且 /\bgit\s+commit\b/i 命中 command）：节流 commitReminders cap=1/session。
//   - 提醒通道：exit 0 + stdout JSON { hookSpecificOutput: { hookEventName, additionalContext } }——文本注入模型上下文、不改判定。
//   - 会话状态：os.tmpdir()/claude-cbm-suggest-<session_id>.json { grepReminders, commitReminders }；
//     损坏/缺失 → 视为全新（fail-open）。单注册（仅 .claude/settings.json）→ 无并行竞写（CH-3 裁定）。

import { readFileSync, writeFileSync, existsSync } from 'node:fs';
import { join } from 'node:path';
import { tmpdir } from 'node:os';

const ELIGIBLE_FULL = new Set(['builder', 'fixer', 'planner', 'portfolio-planner', 'acceptance-author', 'integrator']);
const CLONE_BASES = new Set(['builder', 'fixer']);

const GREP_CAP = 2;
const COMMIT_CAP = 1;

const REMIND_GREP =
  '代码探索（定义/调用者/影响面）优先 mcp__codebase-memory-mcp__search_graph / search_code / trace_path（图增强结果）；Grep 留给纯文本/文档/证据面。（本会话此提醒至多 2 次）';
const REMIND_COMMIT =
  'commit 前按 AGENTS.md CodebaseMemory 纪律核对影响面：工作树与索引 checkout 一致时跑 detect_changes；Pod worktree 内如实标 N/A (worktree)，用 git diff --stat + trace_path 抽查替代。';

function readStdin() {
  try {
    return readFileSync(0, 'utf8');
  } catch {
    return '';
  }
}

// 受众闸（decision-packet §2 修正版三步算法）
function eligible(input) {
  const agentType = input?.agent_type;
  if (agentType == null || agentType === '') return true; // 分支①：主线程 / 人类会话
  const k = String(agentType).toLowerCase();
  if (ELIGIBLE_FULL.has(k)) return true; // 分支②：全名查表优先
  if (CLONE_BASES.has(k.split('-')[0])) return true; // 分支③：仅克隆基座回退
  return false; // 分支④：静默
}

// 事件 A 代码面启发（路径白名单为主 + 文件型析取仅与索引根相交时生效）
function isCodeSurface(toolInput, projectDir) {
  const ti = toolInput || {};
  const signals = [ti.path, ti.glob, ti.type, ti.pattern].filter((s) => typeof s === 'string' && s.length > 0);
  // 显式静默面（优先判定）：.claude/**、docs/**、changes/**、.planning/**、*.md、纯文本文件型
  for (const s of signals) {
    const p = s.replace(/\\/g, '/');
    if (/(^|\/)\.claude(\/|$)/.test(p)) return false;
    if (/(^|\/)(docs|changes|\.planning)(\/|$)/.test(p)) return false;
    if (/\.md$/i.test(p)) return false;
  }
  const typeStr = typeof ti.type === 'string' ? ti.type.toLowerCase() : '';
  if (['md', 'markdown', 'txt', 'text'].includes(typeStr)) return false;
  // 路径白名单：firmware/**、server/**、docs/contracts/**（实现面 + 契约面）
  for (const s of signals) {
    const p = s.replace(/\\/g, '/');
    if (/(^|\/)(firmware|server|docs\/contracts)(\/|$)/.test(p)) return true;
  }
  // 文件型析取（c/cpp/h/hpp/py）：仅在会话项目与索引根相交（含被索引目录）时生效
  const fileTypeHit =
    ['c', 'cpp', 'h', 'hpp', 'py'].includes(typeStr) ||
    signals.some((s) => /\*?\.(c|cpp|h|hpp|py)$/i.test(s.replace(/\\/g, '/')));
  if (fileTypeHit) {
    return ['firmware', 'server', 'docs'].some((d) => existsSync(join(projectDir, d)));
  }
  return false;
}

function loadState(statePath) {
  try {
    const s = JSON.parse(readFileSync(statePath, 'utf8'));
    return {
      grepReminders: Number.isInteger(s?.grepReminders) ? s.grepReminders : 0,
      commitReminders: Number.isInteger(s?.commitReminders) ? s.commitReminders : 0,
    };
  } catch {
    return { grepReminders: 0, commitReminders: 0 }; // fail-open：损坏/缺失视为全新
  }
}

function saveState(statePath, state) {
  try {
    writeFileSync(statePath, JSON.stringify(state));
  } catch {
    // 状态写失败不阻塞（I1）
  }
}

function remind(hookEventName, text) {
  try {
    process.stdout.write(
      JSON.stringify({ hookSpecificOutput: { hookEventName, additionalContext: text } }) + '\n',
    );
  } catch {
    // 输出失败不阻塞（I1）
  }
}

function main() {
  let input;
  try {
    input = JSON.parse(readStdin() || '{}');
  } catch {
    process.exit(0); // I1：JSON parse 失败静默
  }

  if (!eligible(input)) process.exit(0);

  const projectDir = process.env.CLAUDE_PROJECT_DIR || input?.cwd || process.cwd();
  const sessionId = String(input?.session_id || 'unknown').replace(/[^A-Za-z0-9_-]/g, '_');
  const statePath = join(tmpdir(), `claude-cbm-suggest-${sessionId}.json`);
  const state = loadState(statePath);

  const event = input?.hook_event_name;
  const toolName = input?.tool_name;

  // 事件 A：PostToolUse Grep|Glob
  if (event === 'PostToolUse' && (toolName === 'Grep' || toolName === 'Glob')) {
    if (!isCodeSurface(input?.tool_input, projectDir)) process.exit(0);
    if (state.grepReminders >= GREP_CAP) process.exit(0);
    state.grepReminders += 1;
    saveState(statePath, state);
    remind(event, REMIND_GREP);
    process.exit(0);
  }

  // 事件 B：PreToolUse Bash + git commit（本分支永不 exit 2——I1）
  if (event === 'PreToolUse' && toolName === 'Bash') {
    const command = String(input?.tool_input?.command || '');
    if (!/\bgit\s+commit\b/i.test(command)) process.exit(0);
    if (state.commitReminders >= COMMIT_CAP) process.exit(0);
    state.commitReminders += 1;
    saveState(statePath, state);
    remind(event, REMIND_COMMIT);
    process.exit(0);
  }

  process.exit(0);
}

try {
  main();
} catch {
  process.exit(0); // I1：任何异常静默
}
process.exit(0);
