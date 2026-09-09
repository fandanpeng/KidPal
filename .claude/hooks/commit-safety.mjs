#!/usr/bin/env node
// commit-safety.mjs — 提醒级 advisory 钩子（非门，一切路径 exit 0）
//
// 治理声明：owner=architect · approver=human · G3（DEV-GOVERNANCE §6 强制执行层行覆盖）· 提醒级非门。
// 批准物：changes/arch/260802-gov-review-agents-md-optimize/PROPOSAL.md（FROZEN 2026-08-03，T7；OQ-2 已裁 (a) 提醒级）。
// 实证来源：/insights 报告 2026-08-02 —— 16 轮合并马拉松中 `git add` 被静默拒绝 → 残余冲突标记进 commit，事后才发现。
//
// 行为规格：
//   - 触发：PreToolUse · tool_name=Bash · command 含 `git add` 或 `git commit`（子串级正则；
//     误报容忍：`echo "git add"` 之类也会触发——advisory 级多提醒一次无害，设计上接受）。
//   - 时序注：PreToolUse 显示的是**命令执行前**的 staged 状态；`git add` 静默拒绝的实际捕获点
//     在后续 `git commit` 命令到来时（commit 触发已覆盖该场景）。
//   - 动作：跑 `git diff --cached --stat` + `git diff --cached --check`，把结果经
//     hookSpecificOutput.additionalContext 注入模型上下文（先例 suggest-codebase-memory.mjs；
//     裸 stdout 模型看不到，会静默失效——恰是本钩子要治的失败模式）。
//   - 不变量 I1（非阻塞）：一切路径 exit 0；JSON parse 失败 / git 不可用 / 任何异常 → 静默 exit 0。
//     提醒级不拦命令（硬拦与否另案评估，运行数据齐备后再议）。

import { execSync } from 'node:child_process';
import { readFileSync } from 'node:fs';

function readStdin() {
  try {
    return readFileSync(0, 'utf8');
  } catch {
    return '';
  }
}

// 触发分类（纯函数，self-check 可测）：返回 'add' | 'commit' | null
export function classify(command) {
  if (typeof command !== 'string' || command.length === 0) return null;
  if (/\bgit\s+add\b/.test(command)) return 'add';
  if (/\bgit\s+commit\b/.test(command)) return 'commit';
  return null;
}

function gitQuiet(args) {
  try {
    return execSync(`git ${args}`, { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'], timeout: 10000 }).trim();
  } catch (e) {
    // `git diff --check` 检出问题时会非零退出——输出恰是证据，收下
    const out = ((e && (e.stdout || '')) + '\n' + (e && (e.stderr || ''))).trim();
    return out || '';
  }
}

// 机器再生面（2026-09-09 human 一次性授权直落，跳过流程在册）：.codebase-memory 两件由 indexer
// 持续自刷新、常驻脏是常态。弱模型在 add/commit 现场纠结「要不要带上它们」的实测病灶，本提示在动作点给答案。
const GRAPH_CACHE_RE = /\.codebase-memory\/(artifact\.json|graph\.db\.zst)/;

// 纯函数（self-check 可测）：命令文本或 staged 快照命中两件即给提示行，否则 null。
export function graphCacheNotice(command, stagedStat) {
  if (GRAPH_CACHE_RE.test(command || '') || GRAPH_CACHE_RE.test(stagedStat || '')) {
    return '◆ .codebase-memory/artifact.json 与 graph.db.zst = 机器再生面：indexer 持续自刷新，按 human 约定不追交、脏着别硬清（合并冲突已由 .codebase-memory/.gitattributes 的 merge=ours 免除）；merge 预检遇其脏 = 常态，不计入清洁度判据。确要刷新快照入 main：按 AGENTS.md「CodebaseMemory 使用纪律」节节奏走。';
  }
  return null;
}

function buildAdvisory(trigger, command) {
  const stat = gitQuiet('diff --cached --stat');
  const check = gitQuiet('diff --cached --check');
  const lines = [
    `【commit-safety 提醒 · 触发 = git ${trigger}】`,
    '当前 staged 内容（命令执行前的快照）：',
    stat || '（staged 为空——git add 可能未生效或被静默拒绝，先跑 git status 核对）',
  ];
  const cacheNote = graphCacheNotice(command, stat);
  if (cacheNote) lines.push(cacheNote);
  if (check) {
    lines.push('⚠ git diff --cached --check 检出问题（残余冲突标记 / 空白错误）：', check);
  } else {
    lines.push('git diff --cached --check：无残余冲突标记。');
  }
  lines.push('（AGENTS.md Hard Rules：git add 后先验 staged 再 commit；历史记录类文件一律 append。）');
  return lines.join('\n');
}

function emitAdvisory(text) {
  process.stdout.write(
    `${JSON.stringify({
      hookSpecificOutput: { hookEventName: 'PreToolUse', additionalContext: text },
    })}\n`,
  );
}

// ---- self-check（真空绿规约：断言输入非空 + 逐 case 预期）----
function selfCheck() {
  const cases = [
    { name: 'add-trigger', input: { tool_name: 'Bash', tool_input: { command: 'git add AGENTS.md' } }, expect: 'add' },
    { name: 'commit-trigger', input: { tool_name: 'Bash', tool_input: { command: 'git commit -m "x"' } }, expect: 'commit' },
    { name: 'add-in-chain', input: { tool_name: 'Bash', tool_input: { command: 'git add . && git status' } }, expect: 'add' },
    { name: 'non-git-silent', input: { tool_name: 'Bash', tool_input: { command: 'ls -la' } }, expect: null },
    { name: 'git-status-silent', input: { tool_name: 'Bash', tool_input: { command: 'git status' } }, expect: null },
    // 误报容忍 case（R1-N4）：echo 里的 "git add" 也触发——advisory 级接受，仍须 exit 0
    { name: 'false-positive-tolerated', input: { tool_name: 'Bash', tool_input: { command: 'echo "git add"' } }, expect: 'add' },
    // 边界：空 / 缺字段 / 非 Bash 工具 → 静默
    { name: 'empty-command', input: { tool_name: 'Bash', tool_input: { command: '' } }, expect: null },
    { name: 'missing-tool-input', input: { tool_name: 'Bash' }, expect: null },
    { name: 'non-bash-tool', input: { tool_name: 'Edit', tool_input: { command: 'git add x' } }, expect: null },
  ];
  const failures = [];
  for (const c of cases) {
    const cmd = c.input.tool_name === 'Bash' ? c.input.tool_input?.command : undefined;
    const got = classify(cmd);
    if (got !== c.expect) failures.push(`${c.name}: expect=${c.expect} got=${got}`);
  }
  // 机器再生面提示（graphCacheNotice 纯函数面）：命令命中 / staged 命中 / 不命中三态
  const noticeCases = [
    { name: 'cache-command-hit', ok: graphCacheNotice('git add .codebase-memory/artifact.json', '') !== null },
    { name: 'cache-staged-hit', ok: graphCacheNotice('git commit -m x', ' .codebase-memory/graph.db.zst | bin 0 -> 5 bytes') !== null },
    { name: 'cache-clean-silent', ok: graphCacheNotice('git add AGENTS.md', ' AGENTS.md | 1 +') === null },
  ];
  for (const c of noticeCases) {
    if (!c.ok) failures.push(`${c.name}: graphCacheNotice 三态断言失败`);
  }
  const total = cases.length + noticeCases.length;
  const ok = failures.length === 0;
  process.stdout.write(
    `${JSON.stringify({ selfCheck: 'commit-safety', pass: total - failures.length, fail: failures.length, total, ok, failures: ok ? undefined : failures })}\n`,
  );
  process.exit(ok ? 0 : 1);
}

if (process.argv.includes('--self-check')) {
  selfCheck();
}

// ---- live 路径（不变量 I1：一切 exit 0）----
try {
  const raw = readStdin();
  if (!raw) process.exit(0); // 断言输入非空：无输入 → 静默
  const input = JSON.parse(raw);
  if (input?.tool_name !== 'Bash') process.exit(0);
  const trigger = classify(input?.tool_input?.command);
  if (!trigger) process.exit(0);
  emitAdvisory(buildAdvisory(trigger, input?.tool_input?.command));
  process.exit(0);
} catch {
  process.exit(0); // fail-open
}
