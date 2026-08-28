#!/usr/bin/env node
// Canary / 回归测试：验证 enforce-scope.mjs 的 per-agent 边界。
// 触发来源：gov-review-arch-change-autorun 的 F3（大小写逃逸）+ F1（subagent 硬门依赖 agent_type）。
// 覆盖范围：
//   - F3 全覆盖：大小写变体 / 大写+后缀 的 agent_type 归一后仍被 deny 拦。
//   - F1 hook 侧契约：有 agent_type → 强制；无 agent_type（主线程）→ per-agent 不约束（carve-out）。
//   - 注意：本 canary **不能**验证真实 Claude Code runtime 是否把 agent_type 传进 hook——那是运行时集成，
//     由 enforce-scope.mjs 头注释「已对照官方文档」担保；本 canary 只钉死「一旦传入，hook 行为正确」。
// 运行：node .claude/scripts/enforce-scope-canary.mjs   （全过 exit 0；任一失败 exit 1）
// 不依赖测试框架（本仓无 package.json / test runner），直接 spawn 真钩子断言退出码。

import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { dirname, resolve } from 'node:path';

const here = dirname(fileURLToPath(import.meta.url));
const repoRoot = resolve(here, '..', '..');
const hook = resolve(here, '..', 'hooks', 'enforce-scope.mjs');

// exit 2 = 拦截（越界）；exit 0 = 放行。agent:undefined 模拟主线程（无 agent_type）。
const cases = [
  { name: 'architect subagent 写 .claude/** → 拦', agent: 'architect', file: '.claude/agents/planner.md', expect: 2 },
  { name: '大小写变体 "Architect" 写 .claude/** → 归一后仍拦（F3）', agent: 'Architect', file: '.claude/agents/planner.md', expect: 2 },
  { name: '大写+后缀 "ARCHITECT-foo" 写 .claude/** → 归一+回退基础角色仍拦（F3）', agent: 'ARCHITECT-foo', file: '.claude/agents/planner.md', expect: 2 },
  { name: 'architect 写 docs/systems/** → 放行（起草面 allow）', agent: 'architect', file: 'docs/systems/S1-leads/spec.md', expect: 0 },
  { name: 'architect 写 contracts/** → 拦（契约冻结 deny）', agent: 'architect', file: 'contracts/events/index.ts', expect: 2 },
  { name: 'architect 写 docs/00-company-blueprint.md → 拦（L0 deny）', agent: 'architect', file: 'docs/00-company-blueprint.md', expect: 2 },
  { name: '主线程（无 agent_type）写 .claude/** → 放行（per-agent 不约束）', agent: undefined, file: '.claude/agents/planner.md', expect: 0 },
];

let failed = 0;
for (const c of cases) {
  const input = { tool_input: { file_path: c.file }, cwd: repoRoot };
  if (c.agent !== undefined) input.agent_type = c.agent;
  const res = spawnSync(process.execPath, [hook], {
    input: JSON.stringify(input),
    env: { ...process.env, CLAUDE_PROJECT_DIR: repoRoot },
    encoding: 'utf8',
  });
  const code = res.status;
  const ok = code === c.expect;
  if (!ok) failed++;
  console.log(`${ok ? 'PASS' : 'FAIL'}  [exit ${code}, 期望 ${c.expect}]  ${c.name}`);
  if (!ok && res.stderr) console.log(`        stderr: ${res.stderr.trim().split('\n')[0]}`);
}

console.log(`\nenforce-scope-canary: ${cases.length - failed}/${cases.length} 通过`);
process.exit(failed === 0 ? 0 : 1);
