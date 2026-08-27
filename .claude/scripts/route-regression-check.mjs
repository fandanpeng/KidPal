#!/usr/bin/env node
// route-regression-check.mjs — A3 route-regression harness (§H 硬门1 / C-EXP-1 / [H3])
// Static, deterministic PROXY gate for orchestrator routing. Does NOT run the LLM.
// Asserts: (1) command files still name the right entry agent; (2) entry/orchestrator
// agent descriptions still contain their required routing-signal substrings (§B.1/§3.2);
// (3) tools:Agent() dispatch whitelists still include required members.
// [H3] --self-check: deleting any required signal from a description must flip the check RED
//   → proves the gate is falsifiable (the mechanical arbiter for F-H3).
// owner=architect (G3). Landing-time/manual tool (no CI/pre-commit per human 2026-07-08).
// Exit 0 = all green; exit 1 = a routing signal / dispatch whitelist regressed (real gate).
import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const CFG = JSON.parse(readFileSync(join(ROOT, '.claude', 'route-fixtures', 'routing.json'), 'utf8'));
const selfCheck = process.argv.includes('--self-check');

const fails = [];
const oks = [];
const read = (p) => readFileSync(join(ROOT, p), 'utf8');

// Extract the frontmatter `description:` value (single-line, quotes stripped).
function descriptionOf(agent) {
  const txt = read(`.claude/agents/${agent}.md`);
  const m = txt.match(/^description:\s*(.*)$/m);
  if (!m) throw new Error(`no description in ${agent}.md`);
  return m[1].trim().replace(/^["']|["']$/g, '');
}
// Extract the Agent(...) members from the frontmatter `tools:` line.
function toolsAgentsOfSource(source) {
  const m = source.match(/^tools:.*Agent\(([^)]*)\)/m);
  if (!m) return [];
  return m[1].split(',').map((s) => s.trim());
}
function toolsAgentsOf(agent) {
  return toolsAgentsOfSource(read(`.claude/agents/${agent}.md`));
}
// Extract the `agents: [...]` inline-array members from a .github mirror frontmatter.
function mirrorAgentsOf(relPath) {
  const txt = read(relPath);
  const m = txt.match(/^agents:\s*\[([^\]]*)\]\s*$/m);
  if (!m) return null; // 非 inline 形态 → 调用方 fail-loud（对偶 INV-5 :208）
  return m[1].split(',').map((s) => s.trim()).filter(Boolean);
}

// (1) command → entry agent routing
for (const { command, entryAgent } of CFG.commandEntry) {
  const txt = read(command);
  const re = new RegExp(`你现在是[^\\n]{0,40}${entryAgent}`);
  if (re.test(txt)) oks.push(`route ${command} → ${entryAgent}`);
  else fails.push(`ROUTE: ${command} no longer routes to "${entryAgent}" (missing 你现在是…${entryAgent})`);
  // [H3] 可证伪自检（260825-gov-review-queue-hygiene-batch1 T2(a)，收口批 2 留根）：剥离路由片段 → 检查必须转红。
  if (selfCheck) {
    const mutated = txt.replace(re, '');
    if (re.test(mutated)) fails.push(`SELF-CHECK: route ${command} → ${entryAgent} not falsifiable — still matches after removal`);
    else oks.push(`self-check route ${command} → ${entryAgent} → RED when removed ✓`);
  }
}

// (2) required routing-signal substrings in descriptions
for (const [agent, signals] of Object.entries(CFG.agentRequiredSignals)) {
  let desc;
  try { desc = descriptionOf(agent); } catch (e) { fails.push(`SIGNAL: ${e.message}`); continue; }
  for (const sig of signals) {
    if (desc.includes(sig)) oks.push(`signal ${agent} ⊇ "${sig}"`);
    else fails.push(`SIGNAL: ${agent} description dropped required routing signal "${sig}"`);
    // [H3] falsifiability self-check: remove the signal → check MUST now fail
    if (selfCheck) {
      const stripped = desc.split(sig).join('');
      if (stripped.includes(sig)) fails.push(`SELF-CHECK: "${sig}" (${agent}) not falsifiable — still present after removal`);
      else oks.push(`self-check ${agent}/"${sig}" → RED when removed ✓`);
    }
  }
}

// (3) tools:Agent() dispatch whitelist
for (const [agent, required] of Object.entries(CFG.agentRequiredToolsAgents)) {
  const have = toolsAgentsOf(agent);
  for (const member of required) {
    if (have.includes(member)) oks.push(`tools ${agent} ⊇ Agent(${member})`);
    else fails.push(`TOOLS: ${agent} tools:Agent() dropped "${member}" (dispatch whitelist regressed)`);
    // [H3] 可证伪自检（腿3）：从 Agent() 白名单剥离该成员 → 检查必须转红（对偶环 2 信号剥离；
    // split/join 剥离对子串重名成员〔如 architect ⊂ architect-reviewer〕为过剥离——过剥离只令断言更保守，不造假绿）。
    if (selfCheck) {
      const txt = read(`.claude/agents/${agent}.md`);
      const toolsLine = (txt.match(/^tools:.*$/m) || [''])[0];
      const mutated = toolsAgentsOfSource(txt.replace(toolsLine, toolsLine.split(member).join('')));
      if (mutated.includes(member)) fails.push(`SELF-CHECK: "Agent(${member})" (${agent}) not falsifiable — still present after removal`);
      else oks.push(`self-check tools ${agent}/Agent(${member}) → RED when removed ✓`);
    }
  }
}

// (4) .github 镜像面：mirror agents: 白名单 ⊇ agentRequiredToolsAgents 必需成员（ARQ-077 腿3；
// cross-surface INV-5 只守活性〔成员 ∈ 现存 agent 基名集〕，完整性此前无断言）。镜像面动态发现，无新 fixture 键。
const MIRROR_DIR = '.github/agents';
if (existsSync(join(ROOT, MIRROR_DIR))) {
  for (const file of readdirSync(join(ROOT, MIRROR_DIR)).filter((f) => f.endsWith('.agent.md')).sort()) {
    const canonical = file.replace(/\.agent\.md$/, '');
    const required = CFG.agentRequiredToolsAgents[canonical];
    if (!required) { oks.push(`mirror ${file} (no agentRequiredToolsAgents entry; completeness skipped)`); continue; }
    const have = mirrorAgentsOf(join(MIRROR_DIR, file));
    if (have === null) { fails.push(`MIRROR: ${MIRROR_DIR}/${file} agents: line is not inline-array form (fail-loud)`); continue; }
    for (const member of required) {
      if (have.includes(member)) oks.push(`mirror ${file} ⊇ Agent(${member})`);
      else fails.push(`MIRROR: ${file} agents: dropped "${member}" (agentRequiredToolsAgents.${canonical} requires it)`);
      // [H3] 可证伪自检（260825-gov-review-queue-hygiene-batch1 T2(b)，收口批 2 R1-F1）：从镜像 agents: 行剥离该成员 → 重取必缺。
      // 剥离形态对偶环 3 tools 自检；split/join 对子串重名成员为过剥离——过剥离只令断言更保守，不造假绿（与环 3 注释对称，R1-F2 注）。
      if (selfCheck) {
        const mirrorTxt = read(join(MIRROR_DIR, file));
        const agentsLine = (mirrorTxt.match(/^agents:.*$/m) || [''])[0];
        const mutatedLine = agentsLine.split(member).join('');
        const rem = mutatedLine.match(/^agents:\s*\[([^\]]*)\]\s*$/);
        const mutatedMembers = rem ? rem[1].split(',').map((s) => s.trim()).filter(Boolean) : [];
        if (mutatedMembers.includes(member)) fails.push(`SELF-CHECK: mirror ${file} Agent(${member}) not falsifiable — still present after removal`);
        else oks.push(`self-check mirror ${file}/Agent(${member}) → RED when removed ✓`);
      }
    }
  }
}

console.log(`route-regression-check: ${oks.length} ok, ${fails.length} fail${selfCheck ? ' [--self-check]' : ''}`);
for (const f of fails) console.log(`  ✗ ${f}`);
if (fails.length === 0 && selfCheck) console.log('  ✓ all required signals/whitelists falsifiable (deleting any → RED)');
process.exit(fails.length === 0 ? 0 : 1);
