#!/usr/bin/env node
// ARQ-260907-architect-dispatch-model-override（gov-review-architect-dispatch-model-override，G3 机械防呆）：
// architect 派发携带 model 参数 → deny。frontmatter `model: fable`（architect.md:5）是 FROZEN 默认档，
// planner.md:71 ARCH-ADVISORY 档明文「派发不带 model 覆盖」；MODEL-TIER（planner.md:57）升档授权仅限 builder/fixer。
// 判据：PreToolUse(Agent|Task) tool_input.subagent_type === 'architect' 且 tool_input.model 为非空字符串 → exit 2。
// builder/fixer 等其它 agent 的 model 覆盖不在本闸范围（MODEL-TIER 授权面，不误伤）。
// 逃生门（human 裁定后生效，见 PROPOSAL OQ2）：env KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1 放行——
//   仅会话启动者/human 可设（Bash 子进程改不了 Claude Code 主进程环境，agent 无法自开）；设置时放行并打印 advisory。
//   完备性依赖（R1-F3）：agent 写 `.claude/**` 被封（scope-map universal.bashDenyWrite）兜住另一条路——
//   将来若放宽 `.claude` 写面，本逃生门随之敞开，须同步复评本门。
// self-check：--self-check 跑内置用例表，输出 JSON {selfCheck,pass,fail,total,ok}，fail>0 → exit 1。
//
// owner=architect, approver=human (G3 hook, see DEV-GOVERNANCE.md §6)

import { readFileSync } from 'node:fs';

function readStdin() {
  try {
    return readFileSync(0, 'utf8');
  } catch {
    return '';
  }
}

function block(reason) {
  process.stderr.write(reason);
  process.exit(2);
}

function judge(toolInput) {
  const subagentType = String(toolInput?.subagent_type || '');
  const model = toolInput?.model;
  if (subagentType !== 'architect') return { block: false };
  if (typeof model !== 'string' || model.trim() === '') return { block: false };
  return {
    block: true,
    reason: `agent-model-guard: architect 派发携带 model="${model}" — planner.md:71 ARCH-ADVISORY 档 FROZEN 条款「派发不带 model 覆盖」禁止覆盖，frontmatter model: fable 默认档才生效。去掉 model 参数重派；MODEL-TIER 升档授权仅限 builder/fixer（planner.md:57）。`,
  };
}

function runSelfCheck() {
  const cases = [
    // 假阴（architect 带 model → deny）
    { input: { subagent_type: 'architect', model: 'opus' }, expectBlock: true },
    { input: { subagent_type: 'architect', model: 'fable' }, expectBlock: true },
    { input: { subagent_type: 'architect', model: 'sonnet' }, expectBlock: true },
    { input: { subagent_type: 'architect', model: ' claude-opus-4-8 ' }, expectBlock: true }, // 带空白仍 deny
    // 假阳（architect 不带 model / 空串 → 放行）
    { input: { subagent_type: 'architect' }, expectBlock: false },
    { input: { subagent_type: 'architect', model: '' }, expectBlock: false },
    { input: { subagent_type: 'architect', model: '   ' }, expectBlock: false },
    { input: { subagent_type: 'architect', model: null }, expectBlock: false },
    // 授权升档面（builder/fixer/其它 agent 带 model → 放行，不误伤 MODEL-TIER）
    { input: { subagent_type: 'builder', model: 'opus' }, expectBlock: false },
    { input: { subagent_type: 'fixer', model: 'opus' }, expectBlock: false },
    { input: { subagent_type: 'integrator', model: 'opus' }, expectBlock: false },
    { input: { subagent_type: 'Explore', model: 'haiku' }, expectBlock: false },
    { input: { subagent_type: 'general-purpose', model: 'sonnet' }, expectBlock: false },
    // 畸形 input → 放行（fail-open：闸只对可判定的违例出手，不扩大误伤）
    { input: {}, expectBlock: false },
    { input: { subagent_type: 123, model: 'opus' }, expectBlock: false },
  ];
  let pass = 0;
  const failures = [];
  for (const c of cases) {
    const r = judge(c.input);
    if (r.block === c.expectBlock) pass++;
    else failures.push({ input: c.input, expect: c.expectBlock, got: r.block });
  }
  const ok = failures.length === 0;
  process.stdout.write(
    `${JSON.stringify({ selfCheck: 'agent-model-guard', pass, fail: failures.length, total: cases.length, ok, failures: ok ? undefined : failures })}\n`,
  );
  if (!ok) process.exitCode = 1;
}

// ---- 入口 ----
if (process.argv.includes('--self-check')) {
  runSelfCheck();
} else {
  let input;
  try {
    input = JSON.parse(readStdin() || '{}');
  } catch {
    process.exit(0);
  }
  const result = judge(input?.tool_input);
  if (result.block && process.env.KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE !== '1') {
    block(`blocked by ${result.reason}`);
  }
  if (result.block && process.env.KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE === '1') {
    process.stderr.write('agent-model-guard: KIDPAL_ALLOW_AGENT_MODEL_OVERRIDE=1 — human 一次性豁免生效，本次放行。\n');
  }
  process.exit(0);
}
