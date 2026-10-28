// Governance 结构门共享单源（ARQ-097 收敛，gov-review-gate-input-format-contract landing 2026-07-30）。
// landing-fidelity-check.mjs / proposal-consolidation-check.mjs 原各持一份 isFrozen + TARGET_PATTERNS
// 拷贝（ALIGNMENT 注释声明原子改），现机械单源。dedup vs 内聚：isFrozen / TARGET_PATTERNS = 真双拷贝去重；
// CANONICAL_TEST / extractManifestBlock 原仅 fidelity 单份，随迁属内聚。
// 行为变更出处（皆经 human-approved FROZEN）：
//   B1 isFrozen 状态值主导词收窄（slug/引名中 draft/consolidated 子串不再否决同行先位的 FROZEN 真值——卡点 C 修复）
//   B2 extractTargets 废贪婪回退 → 无 manifest 围栏块显式 throw（fail-loud，调用方转 fail）
//   B3 extractTargetsFromPacket 新增（packet 侧 manifest 自愿 + 工具路径形状排除 + markdown 链接目标排除）
//   B4 isProposalMdPath / normalizeSourceRef 收敛（260731-gov-review-artifact-lifecycle-origin-teeth Phase 2 T4/CF-07，FROZEN 2026-08-01）：
//      taskpackage-required-check.mjs / devloop-artifact-check.mjs 原各持一份（真双拷贝去重）；closeout-check.mjs 原无拷贝
//      （T2 改法若就地写会造第三份）→ 三脚本改具名导入单源（ARQ-097 模式）。
//   B5 computeFrozenPin 纯新增（Phase 2 T1/OQ5）：标准 frozen pin = 行尾归一化（\r\n→\n）后 sha1:<16hex>，closeout-check.mjs warnDrift
//      消费端改调此单源（CRLF 跨平台字节一致）；带 runComputeFrozenPinSelfCheck 结构化自检（CF-12，ARQ-114 先例）。isFrozen 函数体零改动（S8 条件）。
//   B7 §2 锚清单解析 + 对账纯新增（260824-gov-review-track-b-drafting-hardening-2 T3，FROZEN 2026-08-25）：parseSection2AnchorLists
//      （显式锚清单对账：backtick 必须、迁移锚读 B、条件锚裁剪、或锚/缺锚/歧义锚 fail-loud）+ compareAnchorSets（并集 ≡ manifest 双向差）
//      + runSection2AnchorSelfCheck 结构化自检（非终端式）；消费端 = landing-fidelity-check.mjs 新断言（插入位置 = 前置链后、git 比对前）。

import { createHash } from 'node:crypto';
import { existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { CHANGE_ID_PREFIX_RE } from './changeid-shared.mjs';

// 治理面 canonical 路径（G1 指令根 / G2 角色流程定义 / G3 强制层 / delivery/workstreams 设计权威）。
export const TARGET_PATTERNS = [
  /\.claude\/(?:agents|commands|references|rules|skills)\/[A-Za-z0-9_.\/-]+/g,
  /\.claude\/(?:hooks|scripts)\/[A-Za-z0-9_.\/-]+/g,
  /\.claude\/(?:scope-map\.json|verify-gates\.json|[A-Za-z0-9.-]+\.settings\.json)/g,
  /docs\/[A-Za-z0-9_.\/-]+\.md/g,
  /delivery\/workstreams\/[A-Za-z0-9_.\/-]+/g,
  /\b(?:README|AGENTS|CLAUDE)\.md\b/g,
];

export const CANONICAL_TEST = /^(?:\.claude\/|docs\/|delivery\/workstreams\/|README\.md|AGENTS\.md|CLAUDE\.md)/;

// ANCHORABLE_TEST（260907-gov-review-arq-governance-batch-2 T1，FROZEN 2026-09-07）：
// §2 锚提取专用形态 = CANONICAL_TEST ∪ 仓根点文件（`.` 开头无内斜杠的单段文件名）∪ delivery 证据根前缀。
// 261015-gov-review-changes-taxonomy T6 第 5 条：delivery 根枚举扩 intake|retro|review|archive 四根
// （changes 根保留——归档内历史 PROPOSAL 仍可被 preflight 解析）；本项是波段二落地序列时序前置。
// 消费方 = 本文件 :386 锚过滤一处；changedNotInProposal 检测面（landing-fidelity-check.mjs:367）维持
// CANONICAL_TEST 不动——检测向严、锚提取向宽，两职责分离（R1-F8 消费点核对：全仓 CANONICAL_TEST 仅此两处）。
export const ANCHORABLE_TEST = /^(?:\.claude\/|docs\/|delivery\/(?:workstreams|milestones|taskpackages|intake|retro|review|archive|changes)\/|firmware\/|server\/|tools\/|README\.md|AGENTS\.md|CLAUDE\.md|\.[A-Za-z0-9_-]+$)/;

// closeout 收口制品谓词单源（260906-gov-review-index-status-truth-source T1，FROZEN 2026-09-06）：
//   消费方 = closeout-check.mjs（承重闸，本批具名导入替换本地拷贝）+ changes-index.mjs collectDev（INDEX dev 状态推导）。
//   防漂移钉死：清单值与判定逻辑只此一份；改清单 = 同时改变闸行为与投影语义，须经治理批。
// CHANGE_ID_CUTOFF 自 closeout-check.mjs 本地常量收敛（按符号名引用，不引行号——同批 T3 删常量后行号即漂）；
//   该处头注自列「禁各自另造」约定副本共 4 处（本批收敛其一；余 3 副本本批不动，见 §6 留根）。
export const CHANGE_ID_CUTOFF = '2026-07-23';

export function isPostCutoffChangeId(changeId) {
  return CHANGE_ID_PREFIX_RE.test(String(changeId))
    && String(changeId).slice(0, 6) >= CHANGE_ID_CUTOFF.replace(/-/g, '').slice(2);
}

export function closeoutRequiredFiles(changeId) {
  return isPostCutoffChangeId(changeId)
    ? ['docs-delta.md', 'review-packet.md']
    : ['taskpackage.json', 'docs-delta.md', 'review-packet.md'];
}

// B1：状态值主导词。顺序保持 AWAITING → 主导词 → 否定 → FROZEN 真值（F-IMPL-1：否定检查必须在主导词 false-return 之后）。
export function isFrozen(proposalText) {
  const lines = proposalText.split('\n').slice(0, 15);
  const statusLines = lines.filter(line =>
    /^\s*>?\s*\*{0,2}(?:Status|状态)\*{0,2}\s*[:：]/i.test(line)
  );
  if (statusLines.length === 0) return false;

  const statusLine = statusLines[0];

  if (statusLines.length > 1) {
    const frozen0 = /\bFROZEN\b/i.test(statusLines[0]);
    const frozen1 = /\bFROZEN\b/i.test(statusLines[1]);
    if (frozen0 !== frozen1) {
      console.error(JSON.stringify({
        advisory: 'conflicting-status-lines',
        message: 'Multiple status lines with conflicting frozen signals; using first-wins semantics',
        lines: statusLines.slice(0, 2),
      }));
    }
  }

  if (/AWAITING\s+(?:HUMAN\s+)?FREEZE/i.test(statusLine)) return false;

  // B1：值部（冒号后；`**` 加粗不影响位序）内 FROZEN 与 DRAFT/CONSOLIDATED 先出现者主导。
  const valuePart = statusLine.slice(statusLine.search(/[:：]/) + 1);
  const firstIdx = (re) => { const m = valuePart.match(re); return m ? m.index : Number.POSITIVE_INFINITY; };
  const iFrozen = firstIdx(/\bFROZEN\b/i);
  const iNonFrozen = firstIdx(/\b(?:DRAFT|CONSOLIDATED)\b/i);
  // DRAFT/CONSOLIDATED 先 → 主导 → 排除（FROZEN 在其后仅作注记，如「DRAFT（human 盖 FROZEN 后生效）」）。
  if (iNonFrozen < iFrozen) return false;

  if (/(?:not|未標|未标)\s*FROZEN/i.test(statusLine)) return false;

  return /\bFROZEN\b/i.test(statusLine);
}

// 双 landing lane 与作者实现凭据（261028-gov-review-author-implementation-lane）。
// 缺省恒为 packet-bound；新 lane 的凭据由 reviewer / verifier 分别写入，作者不得代写。
export const LANDING_LANES = Object.freeze({
  PACKET_BOUND: 'packet-bound',
  AUTHOR_IMPLEMENTATION: 'author-implementation',
});

export function parseLandingLane(text) {
  const values = [];
  for (const line of String(text || '').split(/\r?\n/).slice(0, 40)) {
    const match = line.match(/^[>\s]*\*{0,2}landing-lane\*{0,2}\s*[:：]\s*`?([A-Za-z-]+)`?/i);
    if (match) values.push(match[1]);
  }
  if (values.length === 0) return LANDING_LANES.PACKET_BOUND;
  if (new Set(values).size !== 1 || !Object.values(LANDING_LANES).includes(values[0])) {
    const err = new Error(`landing-lane 重复冲突或值出闭集：${values.join(', ')}`);
    err.code = 'INVALID_LANDING_LANE';
    throw err;
  }
  return values[0];
}

export function isEvidenceTailPath(path, changeId) {
  const normalized = String(path || '').replace(/\\/g, '/');
  return normalized === 'delivery/review/INDEX.md'
    || normalized === 'delivery/review/.changes-index-state.json'
    || normalized.startsWith(`delivery/review/${changeId}/`);
}

export function readImplementationEvidence(projectDir, changeId, kind, expectedLane = LANDING_LANES.AUTHOR_IMPLEMENTATION) {
  if (kind !== 'review' && kind !== 'verification') throw new Error(`未知实现凭据类型：${kind}`);
  const file = join(projectDir, 'delivery', 'review', changeId, `implementation-${kind}.json`);
  const fail = (reason) => ({ ok: false, reason, path: file, evidence: null });
  if (!existsSync(file)) return fail(`implementation-${kind}.json 不在场`);
  let evidence;
  try { evidence = JSON.parse(readFileSync(file, 'utf8')); }
  catch (error) { return fail(`implementation-${kind}.json 不可解析：${error.message}`); }
  const sha = (v) => typeof v === 'string' && /^[0-9a-f]{40}$/.test(v);
  if (evidence?.schemaVersion !== 1) return fail('schemaVersion 必须为 1');
  if (evidence.lane !== expectedLane || evidence.changeId !== changeId) return fail('lane/changeId 不符');
  if (!sha(evidence.reviewedCommit) || !sha(evidence.reviewedTree)) return fail('reviewedCommit/reviewedTree 必须为 40 位小写 SHA');
  if (kind === 'verification' && (!sha(evidence.verifiedCommit) || !sha(evidence.verifiedTree))) return fail('verifiedCommit/verifiedTree 必须为 40 位小写 SHA');
  if (!Array.isArray(evidence.changedFiles) || evidence.changedFiles.some((p) => typeof p !== 'string' || !p || p.includes('..') || p.startsWith('/'))) return fail('changedFiles 必须为仓根相对路径数组');
  return { ok: true, reason: null, path: file, evidence };
}

// B4：proposal 路径谓词单源（260731-gov-review-artifact-lifecycle-origin-teeth Phase 2 T4/CF-07，FROZEN 2026-08-01）。
// taskpackage-required-check.mjs / devloop-artifact-check.mjs 原各持一份 normalizeSourceRef + isProposalMdPath（真双拷贝）；
// closeout-check.mjs 原无拷贝（T2 就地改会造第三份）→ 三脚本具名导入单源（ARQ-097 模式，与 isFrozen 同文件）。
export function normalizeSourceRef(ref) {
  if (typeof ref !== 'string') return ref;
  const hashIndex = ref.indexOf('#');
  return hashIndex === -1 ? ref : ref.slice(0, hashIndex);
}

export function isProposalMdPath(path) {
  if (typeof path !== 'string') return false;
  const normalized = normalizeSourceRef(path);
  return (
    /^delivery\/changes\/(arch|dev)\/.+\/PROPOSAL\.md$/.test(normalized) || // 旧树分支保留——归档内历史引用可解析（261015 批 T6 第 5 条）
    /^delivery\/review\/.+\/PROPOSAL\.md$/.test(normalized) || // 261015-gov-review-changes-taxonomy T6：新树分支
    /^delivery\/workstreams\/.+\/PROPOSAL\.md$/.test(normalized)
  );
}

// B5：标准 frozen pin（Phase 2 T1/OQ5）：sha1:<16hex>，输入 = 文件内容行尾归一化（\r\n→\n）后取字节。
// CRLF 跨平台一致：core.autocrlf=true 下 producer(LF)/consumer(CRLF) 字节不一致 → 归一化令两端一致。
// 非 git hash-object（后者含 git blob header "blob <size>\0" + content，结构性不可比）。
export function computeFrozenPin(filePath) {
  const raw = readFileSync(filePath, 'utf8');
  const normalized = raw.replace(/\r\n/g, '\n');
  return 'sha1:' + createHash('sha1').update(normalized).digest('hex').slice(0, 16);
}

export function extractManifestBlock(text) {
  const found = new Set();
  for (const block of text.matchAll(/```landing-manifest\r?\n([\s\S]*?)```/g)) {
    for (const rawLine of block[1].split(/\r?\n/)) {
      const line = rawLine.trim().replace(/^`|`$/g, '');
      if (!line || line.startsWith('#')) continue;
      found.add(line);
    }
  }
  return found;
}

// B2：manifest 围栏块必需（fail-loud）。用于 landing-fidelity 的 PROPOSAL 侧 + summary 侧。
// authoring 格式契约 SSOT 见 architect-operating-model §5。
export function extractTargets(text, { source = 'input' } = {}) {
  const manifest = extractManifestBlock(text);
  if (manifest.size > 0) return [...manifest];
  const err = new Error(
    `缺 landing-manifest 围栏块（${source}）：authoring 格式契约要求 frozen PROPOSAL §5 与 architect-landing-summary 各含恰 canonical 目标路径集的 manifest 块（一行一条；SSOT 见 architect-operating-model §5）。旧目录重审缺块者补块，否则视为格式不合格。`
  );
  err.code = 'MISSING_LANDING_MANIFEST';
  throw err;
}

// B3：packet 侧——manifest 自愿（带块则消费）+ 无块则正则抽取并排除两类引用形状：
//   ① 工具路径形状（.claude/scripts/** / .claude/hooks/**）：门命令/工具调用永非 delta landing 目标；
//      真改脚本的 delta 必经 PROPOSAL manifest，由 landing-fidelity 在 landing 时 unauthorized/claimed-not-landed 兜底。
//   ② markdown 链接目标（`](path)` 形状，F-B3-1）：散文引用主流形状，几非 landing 声明。
export function extractTargetsFromPacket(text) {
  const manifest = extractManifestBlock(text);
  if (manifest.size > 0) return [...manifest];
  const found = new Set();
  for (const pattern of TARGET_PATTERNS) {
    for (const match of text.matchAll(pattern)) {
      const cleaned = match[0].replace(/[)\]}>.,，。、；;]+$/, '');
      if (!cleaned) continue;
      if (/^\.claude\/(?:scripts|hooks)\//.test(cleaned)) continue;
      if (text.slice(Math.max(0, match.index - 2), match.index) === '](') continue;
      found.add(cleaned);
    }
  }
  return [...found];
}

export function runIsFrozenSelfCheck() {
  const cases = [
    { input: '> **Status**: CONSOLIDATED — AWAITING HUMAN FREEZE\n\nFROZEN elsewhere in body', expected: false, label: 'awaiting-freeze-body-frozen' },
    { input: '> **Status**: DRAFT\n\n## FROZEN 定稿注记\nAPPROVED somewhere', expected: false, label: 'draft-body-frozen-refs' },
    { input: 'No status line anywhere\n\nFROZEN in body only', expected: false, label: 'no-status-line' },
    { input: '> **Status**: not FROZEN yet\n\nAPPROVED in body', expected: false, label: 'negation-not' },
    { input: '> **Status**: 未标 FROZEN/APPROVED\n\nSome FROZEN ref', expected: false, label: 'negation-chinese' },
    { input: '> **Status**: APPROVED\n\nBody text', expected: false, label: 'approved-only-no-frozen' },
    { input: '> **Status**:\n\nBody text', expected: false, label: 'empty-status-value' },
    { input: '> **Status**: FROZEN — HUMAN APPROVED 2026-07-24\n\nBody', expected: true, label: 'frozen-human-approved' },
    { input: '> 状态：**FROZEN（2026-07-22...）**\n\nBody', expected: true, label: 'chinese-frozen' },
    { input: '> 状态：**FROZEN / APPROVED (2026-07-22)**\n\nBody', expected: true, label: 'frozen-slash-approved' },
    { input: '> **Status**: FROZEN v4（2026-07-24）\n\nBody', expected: true, label: 'frozen-versioned' },
    { input: '> **status**: frozen\n\nBody', expected: true, label: 'case-insensitive' },
    { input: '> **Status**:FROZEN\n\nBody', expected: true, label: 'no-space-after-colon' },
    { input: '> **Status**: FROZEN\n> **Status**: DRAFT\n\nBody', expected: true, label: 'first-wins-frozen-then-draft' },
    { input: '  > **Status**: FROZEN\n\nBody', expected: true, label: 'indented-blockquote' },
    // B1 新增四例（gov-review-gate-input-format-contract）：
    { input: '> **Status**: FROZEN — implements gov-review-discussion-draft-authorship rule\n\nBody', expected: true, label: 'frozen-with-draft-slug-ref' },
    { input: '> **Status**: FROZEN — supersedes gate (consolidated slug)\n\nBody', expected: true, label: 'frozen-with-consolidated-ref' },
    { input: '> **Status**: DRAFT（human 盖 FROZEN 后生效）\n\nBody', expected: false, label: 'draft-dominant-with-later-frozen-note' },
    { input: '> **Status**: CONSOLIDATED — all fixes folded in; FROZEN note below\n\nBody', expected: false, label: 'consolidated-dominant' },
  ];
  let pass = 0;
  let failCount = 0;
  for (const testCase of cases) {
    const result = isFrozen(testCase.input);
    if (result === testCase.expected) pass++;
    else { failCount++; console.error(`FAIL [${testCase.label}]: got ${result}, expected ${testCase.expected}`); }
  }
  console.log(JSON.stringify({ selfCheck: 'isFrozen', pass, fail: failCount, total: cases.length, ok: failCount === 0 }));
  process.exit(failCount > 0 ? 1 : 0);
}

// B2/B3 提取语义自检（ARQ-114 先例：G3 解析器改动必带结构化自检）。非终端式：pass 续行、fail 即 exit 1。
export function runExtractTargetsSelfCheck() {
  let pass = 0;
  let failCount = 0;
  const ok = (cond, label) => { if (cond) pass++; else { failCount++; console.error(`FAIL [${label}]`); } };

  // B2：缺 manifest 块 → throw（fail-loud，err.code 可辨）
  try {
    extractTargets('no manifest here, only prose .claude/agents/foo.md');
    ok(false, 'B2-throw-on-missing-manifest');
  } catch (error) {
    ok(error.code === 'MISSING_LANDING_MANIFEST', 'B2-throw-on-missing-manifest');
  }
  // B2：恰块路径集；注释行/空行跳过
  const withBlock = '```landing-manifest\n.claude/agents/a.md\n# comment\n\n.claude/commands/b.md\n```';
  const fromBlock = extractTargets(withBlock, { source: 'self-check' }).sort();
  ok(JSON.stringify(fromBlock) === JSON.stringify(['.claude/agents/a.md', '.claude/commands/b.md']), 'B2-block-exact-set');
  // B3：packet 侧——工具路径排除 · 常规 canonical 保留 · markdown 链接目标排除
  const packet = 'run node .claude/scripts/landing-fidelity-check.mjs delivery/review/x then edit .claude/agents/x.md and see [spec](docs/kernel/K5-ai-provider.md)';
  const fromPacket = extractTargetsFromPacket(packet);
  ok(!fromPacket.includes('.claude/scripts/landing-fidelity-check.mjs'), 'B3-tool-path-excluded');
  ok(fromPacket.includes('.claude/agents/x.md'), 'B3-canonical-kept');
  ok(!fromPacket.includes('docs/kernel/K5-ai-provider.md'), 'B3-markdown-link-excluded');
  // B3：manifest 自愿——带块则消费块（不再走正则）
  const packetWithBlock = packet + '\n```landing-manifest\n.claude/agents/x.md\n```';
  const fromPacketBlock = extractTargetsFromPacket(packetWithBlock);
  ok(JSON.stringify(fromPacketBlock) === JSON.stringify(['.claude/agents/x.md']), 'B3-manifest-voluntary');

  console.log(JSON.stringify({ selfCheck: 'extractTargets', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
  if (failCount > 0) process.exit(1);
}

// B5 pin 算法结构化自检（CF-12；ARQ-114 先例）。
export function runComputeFrozenPinSelfCheck() {
  let pass = 0;
  let failCount = 0;
  const ok = (cond, label) => { if (cond) pass++; else { failCount++; console.error(`FAIL [${label}]`); } };

  const dir = mkdtempSync(join(tmpdir(), 'compute-frozen-pin-selfcheck-'));
  try {
    const crlfPath = join(dir, 'crlf.md');
    const lfPath = join(dir, 'lf.md');
    writeFileSync(crlfPath, 'line1\r\nline2\r\n');
    writeFileSync(lfPath, 'line1\nline2\n');
    const pinFromCrlf = computeFrozenPin(crlfPath);
    const pinFromLf = computeFrozenPin(lfPath);
    ok(pinFromCrlf === pinFromLf, 'crlf-lf-normalize-equal');
    ok(/^sha1:[0-9a-f]{16}$/.test(pinFromLf), 'shape-sha1-prefix-16hex');
    const knownLfPath = join(dir, 'known-lf.md');
    writeFileSync(knownLfPath, 'a\nb\n');
    ok(computeFrozenPin(knownLfPath) === 'sha1:05dec960e24d918b', 'known-vector-lf');
    const knownCrlfPath = join(dir, 'known-crlf.md');
    writeFileSync(knownCrlfPath, 'a\r\nb\r\n');
    ok(computeFrozenPin(knownCrlfPath) === 'sha1:05dec960e24d918b', 'known-vector-crlf');
  } finally {
    rmSync(dir, { recursive: true, force: true });
  }

  console.log(JSON.stringify({ selfCheck: 'computeFrozenPin', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
  if (failCount > 0) process.exit(1);
}

// B6：origin 谓词键单源（260801-gov-review-claude-consistency-gate，FROZEN v1.0 2026-08-01；ARQ-077 键族批次 1）。
// 类型键 = upstreamType ∨ type、路径键 = upstreamRef ∨ ref（恒 upstream* 优先，对偶 F-05 钉键纪律）；
// taskpackage-required-check / devloop-artifact-check / closeout-check / supervisor-verdict-gate / changes-index
// 原内联拷贝（类型谓词四份 + 路径谓词多份、两处回退顺序反向）改具名导入本单源（ARQ-097/B4 模式）；
// 防回退 = cross-surface-check.mjs INV-7（.claude/{scripts,hooks}/** 扫描面内 ASCII 内联形态机械红，本文件豁免）。
export function resolveOriginType(origin) {
  if (!origin || typeof origin !== 'object') return null;
  const value = origin.upstreamType || origin.type;
  return (typeof value === 'string' && value.trim()) ? value : null;
}

export function resolveOriginRef(origin) {
  if (!origin || typeof origin !== 'object') return null;
  const value = origin.upstreamRef || origin.ref;
  return (typeof value === 'string' && value.trim()) ? value : null;
}

// B8：frozen 谓词键族收敛（260825-gov-review-claude-consistency-batch2 T1，ARQ-077 腿4）：
// frozen 键 = upstreamFrozen ∨ frozen（恒 upstream* 优先，对偶 B6 type/ref 键纪律 / F-05 钉键）。
// 不对称原状：closeout-check.mjs:127 内联读双键、devloop-artifact-check.mjs:144 只读 upstreamFrozen。
// 在盘包零实例使用 origin.frozen（起草期 census）——预防性收敛。防回退 = cross-surface-check.mjs INV-7 pattern 扩认。
export function resolveOriginFrozen(origin) {
  if (!origin || typeof origin !== 'object') return null;
  const value = origin.upstreamFrozen || origin.frozen;
  return (typeof value === 'string' && value.trim()) ? value : null;
}

// B6 谓词键结构化自检（ARQ-114 先例）。非终端式：pass 续行、fail 即 exit 1。
// 关键语义：|| 只对 falsy 回退——whitespace 串 truthy 短路（旧内联 expr + nonEmptyString 的同形行为，逐点对等）。
export function runResolveOriginSelfCheck() {
  let pass = 0;
  let failCount = 0;
  const ok = (cond, label) => { if (cond) pass++; else { failCount++; console.error(`FAIL [${label}]`); } };

  // resolveOriginType
  ok(resolveOriginType(null) === null, 'type-null');
  ok(resolveOriginType(undefined) === null, 'type-undefined');
  ok(resolveOriginType('proposal') === null, 'type-non-object');
  ok(resolveOriginType({}) === null, 'type-empty-object');
  ok(resolveOriginType({ type: 'proposal' }) === 'proposal', 'type-single-type');
  ok(resolveOriginType({ upstreamType: 'proposal' }) === 'proposal', 'type-single-upstream');
  ok(resolveOriginType({ upstreamType: 'proposal', type: 'regression' }) === 'proposal', 'type-both-upstream-wins');
  ok(resolveOriginType({ upstreamType: '', type: 'proposal' }) === 'proposal', 'type-empty-upstream-falls-to-type');
  ok(resolveOriginType({ upstreamType: ' ', type: 'proposal' }) === null, 'type-whitespace-upstream-blocks-type');
  ok(resolveOriginType({ upstreamType: '  ' }) === null, 'type-whitespace-only');
  ok(resolveOriginType({ upstreamType: 42 }) === null, 'type-non-string-truthy');

  // resolveOriginRef
  ok(resolveOriginRef(null) === null, 'ref-null');
  ok(resolveOriginRef({}) === null, 'ref-empty-object');
  ok(resolveOriginRef({ ref: 'delivery/review/x/PROPOSAL.md' }) === 'delivery/review/x/PROPOSAL.md', 'ref-single-ref');
  ok(resolveOriginRef({ upstreamRef: 'delivery/review/x/PROPOSAL.md' }) === 'delivery/review/x/PROPOSAL.md', 'ref-single-upstream');
  ok(resolveOriginRef({ upstreamRef: 'a', ref: 'b' }) === 'a', 'ref-both-upstream-wins');
  ok(resolveOriginRef({ upstreamRef: ' ', ref: 'b' }) === null, 'ref-whitespace-upstream-blocks-ref');
  ok(resolveOriginRef({ upstreamRef: '', ref: 'b' }) === 'b', 'ref-empty-upstream-falls-to-ref');
  ok(resolveOriginRef({ ref: '   ' }) === null, 'ref-whitespace-only');
  ok(resolveOriginRef({ ref: 42 }) === null, 'ref-non-string-truthy');

  // resolveOriginFrozen（B8，260825-gov-review-claude-consistency-batch2 T1；形态逐点对偶上列）
  ok(resolveOriginFrozen(null) === null, 'frozen-null');
  ok(resolveOriginFrozen({}) === null, 'frozen-empty-object');
  ok(resolveOriginFrozen({ frozen: 'sha1:abc' }) === 'sha1:abc', 'frozen-single-frozen');
  ok(resolveOriginFrozen({ upstreamFrozen: 'sha1:abc' }) === 'sha1:abc', 'frozen-single-upstream');
  ok(resolveOriginFrozen({ upstreamFrozen: 'sha1:abc', frozen: 'sha1:def' }) === 'sha1:abc', 'frozen-both-upstream-wins');
  ok(resolveOriginFrozen({ upstreamFrozen: '', frozen: 'sha1:def' }) === 'sha1:def', 'frozen-empty-upstream-falls-to-frozen');
  ok(resolveOriginFrozen({ upstreamFrozen: ' ', frozen: 'sha1:def' }) === null, 'frozen-whitespace-upstream-blocks-frozen');
  ok(resolveOriginFrozen({ frozen: '   ' }) === null, 'frozen-whitespace-only');
  ok(resolveOriginFrozen({ frozen: 42 }) === null, 'frozen-non-string-truthy');

  console.log(JSON.stringify({ selfCheck: 'resolveOrigin', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
  if (failCount > 0) process.exit(1);
}

// B7：PROPOSAL §2 各 T 目标文件锚清单解析（260824-gov-review-track-b-drafting-hardening-2 T3，FROZEN 2026-08-25）。
// 规则形态 = 显式锚清单对账（不做全文自动提取——260802 R1-F1 已实证自动提取歧义爆炸）。规则钉死（FROZEN PROPOSAL T3）：
//   - 解析域 = 「## 2.」节内各 「### T<n>」条目；每条目须含「目标文件：」行（引号块形态允许，strip 前导 >）；
//   - backtick 必须：锚行路径须 backtick 包裹；有「目标文件：」行而无 backtick 路径 = 歧义锚（problem 点名）；
//   - T 条目缺锚行 = 缺锚清单（problem 点名，fail-loud 不假绿）；
//   - 迁移锚读 B：路径后注记（至下一路径间文本）含「迁出」→ 迁出路径仅表示法注记、不入并集；迁入路径入对账；
//   - 条件锚裁剪：路径后注记含「条件」/「触发条件」→ 条件锚，裁出不入并集（manifest 反映裁断）；注记不可识别/模糊 → 按必选锚对账（向严，防条件标注成绕过口）；
//   - 或锚：相邻两路径之间出现「或」= 或锚未决 → problem 点名（须 FROZEN/manifest 层定夺后重跑，不静默拆解、不吞过）。
//   - fenced code block 全跳（T3 子项 b，260920-gov-review-citation-anchor-residue，FROZEN 2026-09-20）：
//     §2 的围栏块 = 示范 / 原文钉死内容，**不是声明**（该立场有既有用法支撑：`checkVerbatimQuotes` 早已把 §2
//     围栏块正文当「作者原文钉死的目标文件内容」做字节级对账）。条目标题扫描与锚行扫描**都**跳围栏内（含开合行），
//     否则围栏内的 `### T<n>` 会造幻影条目并吞其后真锚行、围栏内的 `目标文件：` 会进锚并集造成 §2↔manifest 假失配
//     （R1-F5 两具夹具实测：围栏内路径带 backtick → anchorCount 1 + 真条目被判缺锚；裸写 → anchorCount 0 + 歧义锚）。
function fenceLineFlags(lines) {
  const flags = new Array(lines.length).fill(false);
  let inFence = false;
  for (let i = 0; i < lines.length; i++) {
    if (/^\s*```/.test(lines[i])) { flags[i] = true; inFence = !inFence; continue; }
    flags[i] = inFence;
  }
  return flags;
}

export function parseSection2AnchorLists(text) {
  const problems = [];
  const union = new Set();
  const lines = text.split(/\r?\n/);
  const start = lines.findIndex((line) => /^##\s*2[.、]/.test(line));
  if (start === -1) {
    return { union: [], problems: ['§2 改法节未找到：无法解析目标文件锚清单（结构不合格，fail-loud）'] };
  }
  const flags = fenceLineFlags(lines); // 全文件算一次，再按节域切片（下标与 lines 对齐）
  // 261026 批 T1（ARQ-261019-anchor-probe-fence-blind）：边界探测与锚行扫描同机制——跳围栏。
  // 围栏内以「## 」起头的行是原文钉死内容、不是节边界；旧行为在此提前截断 §2，其后 T 条目与锚行静默消失。
  let end = lines.length;
  let boundaryFound = false;
  for (let i = start + 1; i < lines.length; i++) {
    if (flags[i]) continue;
    if (/^##\s/.test(lines[i])) { end = i; boundaryFound = true; break; }
  }
  // 261026 批 T1：围栏未闭合（fence 定界行为奇数）且找不到非围栏节边界 → §2 扫描域吞至文末，其后条目不可见——fail-loud 点名。
  if (!boundaryFound && lines.reduce((n, l) => n + (/^\s*```/.test(l) ? 1 : 0), 0) % 2 === 1) {
    problems.push('围栏未闭合：fence 定界行为奇数且找不到非围栏节边界，§2 扫描域吞至文末、其后 T 条目与锚行不可见（fail-loud）');
  }
  const entries = [];
  let current = null;
  for (let i = start; i < end; i++) {
    const line = lines[i];
    if (flags[i]) {
      // 围栏内：不产生条目标题，也不算锚行；仅当已进入某条目时记为「围栏内文本」（后续锚行扫描会跳过）。
      if (current) current.lines.push({ text: line, fenced: true });
      continue;
    }
    const tMatch = line.match(/^###\s+(T\d+[^\n]*)/);
    if (tMatch) {
      current = { label: tMatch[1].trim(), lines: [] };
      entries.push(current);
    } else if (current) {
      current.lines.push({ text: line, fenced: false });
    }
  }
  if (entries.length === 0) {
    return { union: [], problems: problems.concat(['§2 无任何 T 条目（### T<n> 标题缺失）：无法对账锚清单（fail-loud）']) };
  }
  for (const entry of entries) {
    const anchorLines = entry.lines
      .filter((l) => !l.fenced)
      .map((l) => l.text.replace(/^\s*>\s*/, ''))
      .filter((stripped) => /^目标文件\s*[:：]/.test(stripped.trim()));
    if (anchorLines.length === 0) {
      problems.push(`缺锚清单：${entry.label} 无「目标文件：」行（fail-loud，不假绿）`);
      continue;
    }
    for (const anchorLine of anchorLines) {
      // 整裁腿形态（260825-gov-review-queue-hygiene-batch1 T1，收口批 G-2a landing-verdict ADVISORY-1）：
      // 「目标文件：无（整裁腿，不入锚集）」= 合法空锚，不入并集、不报歧义锚。
      // 规约依赖（R1-F1 注）：本分支假定仓内项目路径不以「无」字开头（锚行以路径起头）；现行规约下误伤面 = 零。
      const body = anchorLine.replace(/^目标文件\s*[:：]\s*/, '').trim();
      if (/^无/.test(body)) continue;
      const matches = [...anchorLine.matchAll(/`([^`\n]+)`/g)];
      if (matches.length === 0) {
        problems.push(`歧义锚：${entry.label} 「目标文件：」行无 backtick 包裹路径（锚行格式 = 路径须 backtick 包裹，向严按缺锚处理）`);
        continue;
      }
      // B7 第二变体 T1(a) · 路径形状过滤：只认锚可辨路径形态（T1，260907 批：改用 ANCHORABLE_TEST——
      // 认仓根点文件与 delivery 证据根制品形态，承 260906 批 T7/T9 整裁腿缺口根治）；内联代码 token（裸标识符 / --flag CLI /
      // 函数名）不入锚集、不参与歧义锚与或锚判定，仅作注记文本（腿6 批 PROPOSAL.md:104 实证）。
      const canonical = matches.filter((m) => ANCHORABLE_TEST.test(m[1].trim()));
      // T1(b) · 歧义锚兜底口径：过滤后仍零 canonical 路径 → 作者写错锚（全行只写内联代码、无一条真路径）→ fail-loud，
      // 不得因「有 backtick」就静默吞过。
      if (canonical.length === 0) {
        problems.push(`歧义锚：${entry.label} 「目标文件：」行所有 backtick token 均非 canonical 路径形态（作者写错锚 / 只写内联代码）——fail-loud，不假绿`);
        continue;
      }
      // 或锚先行整行判定：相邻 canonical 路径间隙含「或」→ 整行 RED、不消费任何路径（不静默拆解、不吞过）。
      let orFound = false;
      for (let i = 1; i < canonical.length; i++) {
        const between = anchorLine.slice(canonical[i - 1].index + canonical[i - 1][0].length, canonical[i].index);
        if (/或/.test(between)) { orFound = true; break; }
      }
      if (orFound) {
        problems.push(`或锚未决：${entry.label} 两路径间出现「或」——须 FROZEN/manifest 层定夺后重跑（不静默拆解、不吞过）`);
        continue;
      }
      for (let i = 0; i < canonical.length; i++) {
        const path = canonical[i][1].trim();
        const noteAfter = anchorLine.slice(
          canonical[i].index + canonical[i][0].length,
          i + 1 < canonical.length ? canonical[i + 1].index : anchorLine.length
        );
        if (/迁出/.test(noteAfter)) continue; // 迁移锚读 B：迁出路径仅表示法注记，不入对账
        if (/触发条件|条件/.test(noteAfter)) continue; // 条件锚裁剪：manifest 反映裁断
        union.add(path);
      }
    }
  }
  return { union: [...union], problems };
}

// B7：§2 锚清单并集 ↔ manifest 围栏块文件集双向差（对账 = 集合相等；失配双向点名）。
export function compareAnchorSets(section2Union, manifestTargets) {
  const unionSet = new Set(section2Union);
  const manifestSet = manifestTargets instanceof Set ? manifestTargets : new Set(manifestTargets);
  const onlyInSection2 = [...unionSet].filter((t) => !manifestSet.has(t));
  const onlyInManifest = [...manifestSet].filter((t) => !unionSet.has(t));
  return { onlyInSection2, onlyInManifest, aligned: onlyInSection2.length === 0 && onlyInManifest.length === 0 };
}

// B7 结构化自检（ARQ-114 先例）。非终端式：pass 续行、fail 即 exit 1。
// 编码 FROZEN T3 四族正负例：缺锚 fail-loud / 迁移锚并集（读 B）/ 条件锚裁剪 / 或锚 RED（另含歧义锚与无 §2）。
export function runSection2AnchorSelfCheck() {
  let pass = 0;
  let failCount = 0;
  const ok = (cond, label) => { if (cond) pass++; else { failCount++; console.error(`FAIL [${label}]`); } };

  // 正例：本批 §2 锚清单形态（两 T、多路径、「（改：…）」类注记 = 必选锚）
  const positive = [
    '## 2. 改法（T1…T2）',
    '### T1 · 示例甲',
    '改某模板一句。',
    '目标文件：`docs/templates/PROPOSAL.template.md`（改）。',
    '### T2 · 示例乙',
    '目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改：加断言调用）· `.claude/scripts/lib/governance-gate-shared.mjs`（改：加解析器 + 自检）。',
    '## 3. 影响面',
  ].join('\n');
  const posRes = parseSection2AnchorLists(positive);
  ok(posRes.problems.length === 0, 'positive-no-problems');
  ok(JSON.stringify([...posRes.union].sort()) === JSON.stringify(['.claude/scripts/landing-fidelity-check.mjs', '.claude/scripts/lib/governance-gate-shared.mjs', 'docs/templates/PROPOSAL.template.md']), 'positive-union-set');

  // 缺锚清单 fail-loud（点名 T）
  const missing = ['## 2. 改法', '### T1 · 缺锚示例', '改法正文无目标文件行。', '## 3. 影响面'].join('\n');
  const missRes = parseSection2AnchorLists(missing);
  ok(missRes.problems.some((p) => p.includes('缺锚清单') && p.includes('T1')), 'missing-anchor-fail-loud');

  // 迁移锚读 B：锚记迁出+迁入，并集只含迁入路径（正例）
  const migration = ['## 2. 改法', '### T1 · 迁移示例', '目标文件：`docs/old-location.md`（迁出）· `docs/new-location.md`（迁入）。', '## 3.'].join('\n');
  const migRes = parseSection2AnchorLists(migration);
  ok(migRes.problems.length === 0 && JSON.stringify(migRes.union) === JSON.stringify(['docs/new-location.md']), 'migration-read-b-union');
  // 迁移锚负例：manifest 记迁出路径 → compareAnchorSets 双向点名漂移
  const migDiff = compareAnchorSets(migRes.union, new Set(['docs/old-location.md']));
  ok(!migDiff.aligned && migDiff.onlyInManifest.includes('docs/old-location.md') && migDiff.onlyInSection2.includes('docs/new-location.md'), 'migration-drift-named');

  // 条件锚裁剪：条件注记路径不入并集，同在必选路径保留
  const conditional = ['## 2. 改法', '### T1 · 条件示例', '目标文件：`docs/always.md`（改）· `docs/conditional.md`（条件触发：仅当含示例措辞时）。', '## 3.'].join('\n');
  const condRes = parseSection2AnchorLists(conditional);
  ok(condRes.union.includes('docs/always.md') && !condRes.union.includes('docs/conditional.md'), 'conditional-anchor-trimmed');

  // 或锚遭遇 = RED 点名（不静默拆解、不吞过）
  const orAnchor = ['## 2. 改法', '### T1 · 或锚示例', '目标文件：`docs/a.md` 或 `docs/b.md`。', '## 3.'].join('\n');
  const orRes = parseSection2AnchorLists(orAnchor);
  ok(orRes.problems.some((p) => p.includes('或锚未决')), 'or-anchor-red');
  ok(!orRes.union.includes('docs/a.md') && !orRes.union.includes('docs/b.md'), 'or-anchor-not-silently-consumed');

  // 歧义锚：「目标文件：」行无 backtick 路径
  const noBacktick = ['## 2. 改法', '### T1 · 无 backtick 示例', '目标文件：docs/plain.md（改）。', '## 3.'].join('\n');
  const nbRes = parseSection2AnchorLists(noBacktick);
  ok(nbRes.problems.some((p) => p.includes('歧义锚')), 'no-backtick-ambiguous-anchor');

  // 无 §2 节 = 结构不合格 fail-loud
  const noS2 = parseSection2AnchorLists('only prose, no sections');
  ok(noS2.problems.some((p) => p.includes('§2 改法节未找到')), 'no-section2-fail-loud');

  // compareAnchorSets 对齐正例
  const aligned = compareAnchorSets(['a.md', 'b.md'], new Set(['b.md', 'a.md']));
  ok(aligned.aligned && aligned.onlyInSection2.length === 0 && aligned.onlyInManifest.length === 0, 'compare-aligned-positive');

  // 整裁腿形态（260825-gov-review-queue-hygiene-batch1 T1）：「目标文件：无」合法空锚——不入并集、不报歧义锚；与必选锚混排不误伤
  const trimmed = ['## 2. 改法', '### T1 · 常规腿', '改法正文。', '目标文件：`docs/a.md`（改）。', '### T2 · 整裁腿', '整裁说明。', '目标文件：无（整裁腿，不入锚集）。', '## 3.'].join('\n');
  const trimRes = parseSection2AnchorLists(trimmed);
  ok(trimRes.problems.length === 0, 'trimmed-leg-no-problems');
  ok(JSON.stringify([...trimRes.union].sort()) === JSON.stringify(['docs/a.md']), 'trimmed-leg-union-only-mandatory');

  // B7 第二变体 T1 自检：内联代码 token 不入锚集（腿6 批活例）+ 全内联代码 → 歧义锚 fail-loud
  const inlineCode = ['## 2. 改法', '### T1 · 内联代码示例', '目标文件：`.claude/scripts/portfolio-reconcile.mjs`（改：新增 `checkLaunchReachability` + `--check-launch`/`--base-ref` CLI + `selfCheck`）。', '## 3.'].join('\n');
  const inlineRes = parseSection2AnchorLists(inlineCode);
  ok(inlineRes.problems.length === 0 && JSON.stringify(inlineRes.union) === JSON.stringify(['.claude/scripts/portfolio-reconcile.mjs']), 'inline-code-only-canonical-union');
  const allInline = ['## 2. 改法', '### T1 · 全内联代码示例', '目标文件：见 `checkLaunchReachability` 实现。', '## 3.'].join('\n');
  const allInlineRes = parseSection2AnchorLists(allInline);
  ok(allInlineRes.problems.some((p) => p.includes('歧义锚')), 'all-inline-code-ambiguous-anchor');

  // T1（260907 批）自检：ANCHORABLE_TEST 两新形态——仓根点文件锚 + delivery 证据根制品锚，各一正例（均须入并集、零 problems）
  const dotfile = ['## 2. 改法', '### T1 · 仓根点文件锚示例', '改法正文。', '目标文件：`.gitattributes`（新增）。', '## 3.'].join('\n');
  const dotRes = parseSection2AnchorLists(dotfile);
  ok(dotRes.problems.length === 0, 'dotfile-anchor-no-problems');
  ok(JSON.stringify(dotRes.union) === JSON.stringify(['.gitattributes']), 'dotfile-anchor-union');
  const changesPath = ['## 2. 改法', '### T1 · changes 制品锚示例', '改法正文。', '目标文件：`delivery/taskpackages/260831-cloud-link-bringup/review-packet.md`（改：追加复核注）。', '## 3.'].join('\n');
  const chRes = parseSection2AnchorLists(changesPath);
  ok(chRes.problems.length === 0, 'changes-anchor-no-problems');
  ok(JSON.stringify(chRes.union) === JSON.stringify(['delivery/taskpackages/260831-cloud-link-bringup/review-packet.md']), 'changes-anchor-union');

  // fenced code block 跳过（T3 子项 b，260920-gov-review-citation-anchor-residue，FROZEN 2026-09-20）。
  // 夹具两具，形态取自 R1-F5 在真实旧行为下的实测：围栏内路径**带 backtick** / **裸写**。
  // 期望：两种形态下都**零幻影条目、零误判缺锚、围栏内 token 一律不入锚并集**。
  const fencedWithBacktick = [
    '## 2. 改法（T1）',
    '### T1 · 甲',
    '正文一句。',
    '目标文件：`.claude/scripts/real.mjs`（改）',
    '',
    '```md',
    '### T1 · 围栏内示范条目',
    '目标文件：`docs/templates/PROPOSAL.template.md`（改）',
    '```',
    '',
    '## 3. 影响面',
  ].join('\n');
  const fbRes = parseSection2AnchorLists(fencedWithBacktick);
  ok(fbRes.problems.length === 0, 'fence-backtick-no-problems');
  ok(JSON.stringify(fbRes.union) === JSON.stringify(['.claude/scripts/real.mjs']), 'fence-backtick-union-excludes-fenced');
  const fencedBare = fencedWithBacktick.replace('目标文件：`docs/templates/PROPOSAL.template.md`（改）', '目标文件：docs/templates/PROPOSAL.template.md（改）');
  const fnRes = parseSection2AnchorLists(fencedBare);
  ok(fnRes.problems.length === 0, 'fence-bare-no-problems');
  ok(JSON.stringify(fnRes.union) === JSON.stringify(['.claude/scripts/real.mjs']), 'fence-bare-union-excludes-fenced');
  // 反向正控：围栏内的 `### T<n>` 不得造出条目标题（否则其后真锚行会被吞）
  const fencedPhantom = [
    '## 2. 改法（T1）',
    '```md',
    '### T1 · 围栏内示范条目',
    '```',
    '### T1 · 真条目',
    '目标文件：`.claude/scripts/real.mjs`（改）',
    '## 3. 影响面',
  ].join('\n');
  const fpRes = parseSection2AnchorLists(fencedPhantom);
  ok(fpRes.problems.length === 0, 'fence-phantom-no-problems');
  ok(JSON.stringify(fpRes.union) === JSON.stringify(['.claude/scripts/real.mjs']), 'fence-phantom-no-swallow');
  // 261026 批 T1（ARQ-261019-anchor-probe-fence-blind）夹具：判别力 = 反转实现（边界探测不跳围栏）时
  // T2 锚行被截断出域，union 缺 docs/b.md、本组断言变红。
  const fenceBoundary = [
    '## 2. 改法（T1–T2）',
    '### T1 · 甲',
    '正文一句。',
    '目标文件：`docs/a.md`（改）',
    '',
    '```md',
    '## 围栏内原文钉死的节标题（不得作 §2 边界）',
    '```',
    '',
    '### T2 · 乙',
    '目标文件：`docs/b.md`（改）',
    '',
    '## 3. 影响面',
  ].join('\n');
  const fenceBoundaryRes = parseSection2AnchorLists(fenceBoundary);
  ok(fenceBoundaryRes.problems.length === 0, 'fence-boundary-no-problems');
  ok(JSON.stringify([...fenceBoundaryRes.union].sort()) === JSON.stringify(['docs/a.md', 'docs/b.md']), 'fence-boundary-union-keeps-post-fence-entry');
  // 反向正控：真节标题（非围栏）仍终止 §2——§3 内的 T9 标题不造条目、其锚行不入并集（防「跳围栏」被扩成「不认边界」）
  const genuineBoundary = [
    '## 2. 改法（T1）',
    '### T1 · 甲',
    '目标文件：`docs/a.md`（改）',
    '## 3. 影响面',
    '### T9 · §3 内的小节标题',
    '目标文件：`docs/z.md`（改）',
  ].join('\n');
  const genuineBoundaryRes = parseSection2AnchorLists(genuineBoundary);
  ok(genuineBoundaryRes.problems.length === 0 && JSON.stringify(genuineBoundaryRes.union) === JSON.stringify(['docs/a.md']), 'genuine-boundary-still-terminates');
  // 围栏未闭合 fail-loud 点名（判别力：反转实现无此 problem）
  const unclosedFence = [
    '## 2. 改法（T1）',
    '### T1 · 甲',
    '目标文件：`docs/a.md`（改）',
    '```md',
    '正文……（围栏未闭合）',
  ].join('\n');
  const unclosedRes = parseSection2AnchorLists(unclosedFence);
  ok(unclosedRes.problems.some((x) => x.includes('围栏未闭合')), 'unclosed-fence-fail-loud');

  console.log(JSON.stringify({ selfCheck: 'section2Anchors', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
  if (failCount > 0) process.exit(1);
}

// ---- B8 resolveHumanApproval 并入（260908-gov-review-merge-chain-hardening T6，FROZEN v1.0）----
// 纯新增并入（B5 先例）：从 pre-merge-escalation-gate.mjs 原样迁移五要素人批事件核验，gate 改具名导入单源；
// 既有导出与函数体零改动（本函数为新增导出，迁移时按 T5/B-6 裁定增 deps.decision 参数化——缺省
// 'approve-escalated-merge'，既有调用零改动；T5 defer/accept-risk 锚核验复用同一五要素路径、decision 枚举值参数化）。
// 消费方 = pre-merge-escalation-gate.mjs（具名导入）+ merge-driver.mjs approvalRefUnlock（T6 spawn→import，
// N5 已裁选项 2：解锁预核收窄为五要素事件核验，全闸由 Step 1b 同 invocation 背书）。

/**
 * 核验人批事件（原 gate T1 五要素：eventType/actor/payload.decision/changeId/schemaVersion）。
 * 读 <dir>/delivery/taskpackages/<changeId>/pod-events.jsonl，不在场时回退 pods 记录
 * （deps.podsRecordPath）里 worktreePath 下同路径。任一不符 → { ok:false, reason:<点名> }（fail-closed）。
 */
export function resolveHumanApproval(dir, changeId, eventId, deps = {}) {
  const readText = deps.readText || ((p) => (existsSync(p) ? readFileSync(p, 'utf8') : null));
  const expectedDecision = deps.decision || 'approve-escalated-merge';
  if (!eventId) return { ok: false, reason: 'eventId 缺失' };
  let raw = readText(join(dir, 'delivery', 'taskpackages', changeId, 'pod-events.jsonl'));
  if (raw === null && deps.podsRecordPath) {
    // 合并前事件可能只在 worktree 侧落盘：经 pods 记录 worktreePath 回退
    try {
      const wt = JSON.parse(readText(deps.podsRecordPath) || 'null')?.worktreePath;
      if (wt) raw = readText(join(wt, 'delivery', 'taskpackages', changeId, 'pod-events.jsonl'));
    } catch { /* 回退失败按不在场处理 */ }
  }
  if (raw === null) return { ok: false, reason: 'pod-events.jsonl 不在场（主仓与 worktree 回退均未找到）' };
  let hit = null;
  for (const line of raw.split('\n')) {
    const t = line.trim();
    if (!t) continue;
    try {
      const rec = JSON.parse(t);
      if (rec?.eventId === eventId) { hit = rec; break; }
    } catch { continue; }
  }
  if (!hit) return { ok: false, reason: `eventId=${eventId} 不在 pod-events 中` };
  if (hit.eventType !== 'human-decision') return { ok: false, reason: `eventType=${hit.eventType}（应为 human-decision）` };
  if (hit.actor !== 'human') return { ok: false, reason: `actor=${hit.actor}（应为 human）` };
  if (hit.payload?.decision !== expectedDecision) return { ok: false, reason: `payload.decision=${hit.payload?.decision}（应为 ${expectedDecision}）` };
  if (hit.changeId !== changeId) return { ok: false, reason: `changeId=${hit.changeId}（应为本 changeId ${changeId}）` };
  if (hit.schemaVersion !== 1) return { ok: false, reason: `schemaVersion=${hit.schemaVersion}（应为 1）` };
  return { ok: true, reason: 'human-approval verified' };
}

// ── verdict 行判据单源（260920-gov-review-verdict-anchor-align T2，FROZEN 2026-09-20）──
// 从 changes-index.mjs:177-178 迁入（export 单源，防派生判据双源分叉）。
// 消费方 = changes-index deriveLanded（逐行 exec，后出者胜）+ pre-merge-escalation-gate
// readGovAnchors 的 green 锚（逐行扫，任一行 token = GREEN 即 green）。
// 行首判据（^）+ 可选 > 引用前缀 + 可选加粗 + 可选反引号包 token（260920 批 §7 的
// `**Verdict：\`GREEN\`**` 形态为真实判官输出）。
export const VERDICT_STATUS_RE = /^\s*(?:>+\s*)?(?:\*{0,2})\s*(?:verdict|总判|总判定|结论|复验结论|判定)\s*(?:\*{0,2})\s*[:：=]\s*(?:[\x60*]{0,4})\s*(PASS|GREEN|RED|FAIL|BLOCK)\b/i;
export const VERDICT_HEADING_RE = /^#{1,4}\s*(?:\d{1,3}[.、]\s*)?(?:verdict|总判|总判定|结论|复验结论)\s*[:：]?\s*(?:[\x60*]{0,4})\s*(PASS|GREEN|RED|FAIL|BLOCK)?\b/i;
