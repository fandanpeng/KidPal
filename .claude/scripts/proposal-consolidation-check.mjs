#!/usr/bin/env node
// Governance profile 定稿一致性门（proposal-consolidation-check）。
//
// 补的缺口：governance review 的 confirmed 改法若只留在 decision-packet.md / findings.md，
// 不回灌 PROPOSAL.md，则 PROPOSAL 起草后即「孤儿」，canonical landing 又绑定 decision-packet，
// 于是没有任何一步核验「落地的 == 批定的」（gov-review-proposal-consolidation-gate 修复）。
//
// 本脚本是**结构一致性**机器兜底（对偶 arch-review-handoff-check.mjs），不证语义正确——
// 语义正确（无漏改法 / 无新矛盾 / 改法对不对）是 review-moderator 收口人读核验的事，两层互补。
//
// 硬失败（exit 1）：
//   1. PROPOSAL.md 缺失。
//   2. decision-packet.md 缺失。
//   3. PROPOSAL.md 未冻结（Status 行无 FROZEN 标记）。
//   4. PROPOSAL.md 无「定稿」段（未回灌改法的证据）。
//   5. decision-packet 提取的 canonical 目标文件，有任一项未在 frozen PROPOSAL 中出现
//      （= PROPOSAL 没覆盖到某个要落地的文件，漂移）。packet 侧提取 = manifest 自愿 + 工具路径/
//      markdown 链接目标排除（B3，gov-review-gate-input-format-contract）；错误消息含引用 vs 目标辨别引导。
// advisory（只报告不失败）：finding-id 在 PROPOSAL 中的覆盖率。
//
// 用法：node .claude/scripts/proposal-consolidation-check.mjs changes/arch/gov-review-<id>

import { existsSync, readFileSync } from 'node:fs';
import { basename, resolve } from 'node:path';
import {
  extractTargetsFromPacket,
  isFrozen,
  runExtractTargetsSelfCheck,
  runIsFrozenSelfCheck,
} from './lib/governance-gate-shared.mjs';
// isFrozen / TARGET_PATTERNS / extractTargetsFromPacket 单源：./lib/governance-gate-shared.mjs（ARQ-097）。

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
const reviewDir = process.argv[2] ? resolve(projectDir, process.argv[2]) : null;

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'proposal-consolidation', message, ...details });
  process.exit(1);
}

function readText(path) {
  return readFileSync(path, 'utf8').replace(/^\uFEFF/, '');
}

function extractFindingIds(text) {
  const ids = new Set();
  for (const match of text.matchAll(/ARQ-\d{6}-[\w-]+|\b(?:F\d+|[FDA]-\d+|[A-Z]{2,}-\d+|ARQ-\d+)\b/g)) {
    ids.add(match[0]);
  }
  return [...ids];
}

function runExtractSelfCheck() {
  // F01/F02 钉死正则回归守卫（gov-review-arq-id-date-summary FROZEN 2026-07-28）：
  // 新格式 ARQ-<YYMMDD>-<english-kebab-slug> 与旧 ARQ-NNN 共存，C1+C2+C3 同时满足、不误伤其它 finding-id。
  const cases = [
    { input: 'ARQ-260727-suite-gate-exemption and ARQ-260727-routing-owner two', expected: ['ARQ-260727-suite-gate-exemption', 'ARQ-260727-routing-owner'], label: 'C2-sameDay-two-full' },
    { input: '| ARQ-260727-suite-gate | ARQ-260727-routing-owner |', expected: ['ARQ-260727-suite-gate', 'ARQ-260727-routing-owner'], label: 'C2-tableRow' },
    { input: 'see ARQ-260727-suite-gate-C1 subitem', expected: ['ARQ-260727-suite-gate-C1'], label: 'Q4-subitem-full' },
    { input: 'see ARQ-260727-suite-gate, and others', expected: ['ARQ-260727-suite-gate'], label: 'C3-asciiPunct-truncate' },
    { input: 'item ARQ-260727-suite-gate (note) follows', expected: ['ARQ-260727-suite-gate'], label: 'C3-paren-truncate' },
    { input: 'ARQ-103 ARQ-104 ARQ-100 ARQ-063', expected: ['ARQ-103', 'ARQ-104', 'ARQ-100', 'ARQ-063'], label: 'C1-oldIds' },
    { input: 'ARQ-063-C1', expected: ['ARQ-063'], label: 'C1-oldSubitem' },
    { input: 'F12 D-3 FDA-1 SNC-2', expected: ['F12', 'D-3', 'FDA-1', 'SNC-2'], label: 'C1-no-false-ARQ-merge' },
  ];
  let pass = 0;
  let failCount = 0;
  for (const testCase of cases) {
    const result = extractFindingIds(testCase.input).sort();
    const expected = [...testCase.expected].sort();
    if (JSON.stringify(result) === JSON.stringify(expected)) {
      pass++;
    } else {
      failCount++;
      console.error(`FAIL [${testCase.label}]: got ${JSON.stringify(result)}, expected ${JSON.stringify(expected)}`);
    }
  }
  console.log(JSON.stringify({ selfCheck: 'extractFindingIds', pass, fail: failCount, total: cases.length, ok: failCount === 0 }));
  if (failCount > 0) process.exit(1);
}

function hasConsolidationSection(proposalText) {
  // 定稿段：回灌改法的证据（「定稿」「Review 定稿记录」「Consolidation」标题/块）。
  return /定稿|Review 定稿记录|Consolidat/i.test(proposalText);
}

if (process.argv.includes('--self-check')) { runExtractSelfCheck(); runExtractTargetsSelfCheck(); runIsFrozenSelfCheck(); } // 顺序：extract 类先（非终端式）、isFrozen 收尾 exit

if (!reviewDir) {
  fail('review directory argument is required', {
    usage: 'node .claude/scripts/proposal-consolidation-check.mjs changes/arch/gov-review-<id>',
  });
}

if (!existsSync(reviewDir)) fail('review directory does not exist', { reviewDir });

const proposalPath = resolve(reviewDir, 'PROPOSAL.md');
const decisionPacketPath = resolve(reviewDir, 'decision-packet.md');
const findingsPath = resolve(reviewDir, 'findings.md');
const relDir = `changes/arch/${basename(reviewDir)}`;

if (!existsSync(proposalPath)) {
  fail('PROPOSAL.md is required (governance profile 定稿的单一权威设计)', { reviewDir: relDir });
}
if (!existsSync(decisionPacketPath)) {
  fail('decision-packet.md is required（本次 confirmed 改法 delta，定稿核验的对照源）', { reviewDir: relDir });
}

const proposalText = readText(proposalPath);
const decisionPacketText = readText(decisionPacketPath);
const errors = [];

if (!isFrozen(proposalText)) {
  errors.push('PROPOSAL.md 未冻结：Status 行缺 FROZEN 标记（human 盖 FROZEN 后才可 landing，作者≠判官；正文提及不算）');
}
if (!hasConsolidationSection(proposalText)) {
  errors.push('PROPOSAL.md 无「定稿」段：没有回灌 confirmed 改法的证据（应含「Review 定稿记录」或等价段）');
}

// 目标覆盖：decision-packet 提取的每个 canonical 目标都必须在 frozen PROPOSAL 中出现。
// B3：packet 侧提取 = manifest 自愿 + 工具路径形状排除 + markdown 链接目标排除（lib extractTargetsFromPacket）。
const declaredTargets = extractTargetsFromPacket(decisionPacketText);
const missingTargets = declaredTargets.filter((target) => !proposalText.includes(target));
if (missingTargets.length > 0) {
  errors.push(
    `PROPOSAL.md 未覆盖 decision-packet 提取的 canonical 目标：${missingTargets.join(', ')}。` +
    `先确认是否真 landing 目标——若为论证引用（非目标），PROPOSAL manifest（authoring 格式契约，SSOT 见 architect-operating-model §5）只列真目标即可放行；packet 可自愿带 landing-manifest 块消歧。确属漏回灌则回灌 PROPOSAL。`,
  );
}

if (errors.length > 0) {
  fail('governance 定稿一致性核验未过', { reviewDir: relDir, errors });
}

// advisory：finding-id 覆盖率（只报告，不失败——语义正确归 moderator 人读核验）。
const packetFindingIds = extractFindingIds(decisionPacketText);
const findingsText = existsSync(findingsPath) ? readText(findingsPath) : '';
const findingIds = [...new Set([...packetFindingIds, ...extractFindingIds(findingsText)])];
const findingsCovered = findingIds.filter((id) => proposalText.includes(id));
const findingsMissing = findingIds.filter((id) => !proposalText.includes(id));

print({
  ok: true,
  gate: 'proposal-consolidation',
  reviewDir: relDir,
  frozen: true,
  consolidationSection: true,
  declaredTargets,
  missingTargets: [],
  advisory: {
    findingIdsTotal: findingIds.length,
    findingIdsCovered: findingsCovered.length,
    findingIdsNotInProposal: findingsMissing,
    note: '结构一致性已过；finding 语义覆盖为 advisory，语义正确归 review-moderator 收口人读核验。',
  },
});
