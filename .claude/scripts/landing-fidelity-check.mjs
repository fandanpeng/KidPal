#!/usr/bin/env node
// Governance profile landing 保真度门（landing-fidelity-check）。
//
// 补的缺口：architect canonical landing 之后，没有任何独立一方核验「实际落地的 canonical 文件
// == frozen PROPOSAL 的 landing 清单」——今天只靠人类在 merge 门手动 spot-check（A9 原话）。
// 这是治理 review loop 缺失的 verifier 槽（builder≠verifier）的机械层；agent 层是 landing-verifier。
//
// 本脚本只做**结构保真度**（确定性、退出码，对偶 proposal-consolidation-check.mjs / arch-review-handoff-check.mjs）：
//   硬失败（exit 1）：
//     1. PROPOSAL.md 或 architect-landing-summary.md 缺失。
//     2. PROPOSAL.md 未冻结（Status 行无 FROZEN 标记）。
//     3. landing-summary 无 packet-binding（缺 packet-id / 被批 review id）。
//     4. landing-summary 清单 ⊄ PROPOSAL 清单（落地超出授权 = scope creep）。
//     5. landing-summary 声称落的文件，git 里没有改动（claimed-but-not-landed）。
//     6. PROPOSAL 清单里的文件，git 里没有改动（planned-but-not-landed，静默漏落）。
//     7. PROPOSAL.md 或 architect-landing-summary.md 缺 ```landing-manifest 围栏块（B2 fail-loud，格式不合格）。
//     8. PROPOSAL §2 各 T 目标文件锚清单并集 ≢ manifest 围栏块文件集，或某 T 缺锚清单 / 歧义锚 / 或锚未决
//        （T3 锚清单对账断言，260824-gov-review-track-b-drafting-hardening-2；fail-loud 点名，向严不漏放）。
//   advisory（只报告，不失败）：
//     - git 改动里、不在 PROPOSAL 清单的 canonical 文件（可能是同工作树其它未提交 landing——交给 moderator/人看）。
//
// 语义保真（改法对不对、有无私货）归 landing-verifier 人读 diff，本脚本不判。
//
// 用法：node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/gov-review-<id> [baseRef]
// 「已落地」判定 = 工作树改动 ∪ 已提交改动；默认相对 HEAD~1，给 baseRef 则相对该 ref（含未提交）。
// landing 清单来源：读 PROPOSAL.md / architect-landing-summary.md 里的 ```landing-manifest 围栏块
// （一行一个 canonical 路径，authoring 格式契约见 architect-operating-model §5）。
// B2 fail-loud：任一侧无该块 → 显式 RED（格式不合格），不再正则兜底。

import { existsSync, mkdirSync, readdirSync, readFileSync, mkdtempSync, writeFileSync, rmSync } from 'node:fs';
import { basename, resolve, join } from 'node:path';
import { tmpdir } from 'node:os';
import { execSync, spawnSync } from 'node:child_process';
import {
  CANONICAL_TEST,
  compareAnchorSets,
  extractTargets,
  isFrozen,
  parseSection2AnchorLists,
  runComputeFrozenPinSelfCheck,
  runExtractTargetsSelfCheck,
  runIsFrozenSelfCheck,
  runResolveOriginSelfCheck,
  runSection2AnchorSelfCheck,
} from './lib/governance-gate-shared.mjs';
// isFrozen / TARGET_PATTERNS / extractTargets / CANONICAL_TEST 单源：./lib/governance-gate-shared.mjs（ARQ-097）。

const BOM = String.fromCharCode(0xFEFF);
const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
// --preflight（260918-gov-review-batch-drafting-friction T2，FROZEN 2026-09-19）：冻结前只读干跑入口。
// 抽成纯函数以便自检断言「干跑模式不误触 reviewDir/baseRef 解析」——否则 `--preflight` 自身会被
// 位置参数当路径消费（PROPOSAL §2 T2「入口接线注意」明列此风险）。常规模式行为与改前逐字一致。
function parseCliArgs(rawArgv) {
  const argv = rawArgv.slice(2);
  const idx = argv.indexOf('--preflight');
  if (idx >= 0) return { preflight: argv[idx + 1] || null, positional: null, baseRef: null };
  return { preflight: null, positional: argv[0] || null, baseRef: argv[1] || null };
}
const CLI = parseCliArgs(process.argv);
const preflightArg = CLI.preflight;
const reviewDir = CLI.positional ? resolve(projectDir, CLI.positional) : null;
// 可选 baseRef：判定「已落地」时相对哪个 ref 看改动。不给则默认工作树 ∪ 最近一次 commit/merge（HEAD~1..HEAD）。
const baseRef = CLI.baseRef || null;

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'landing-fidelity', message, ...details });
  process.exit(1);
}

function readText(path) {
  return readFileSync(path, 'utf8').replace(new RegExp('^' + BOM), '');
}

// ── 引据锚点检查（260826-gov-review-claude-consistency-batch3 T1，advisory-only）──
// 提取 PROPOSAL 全文 backtick 包裹的 `<路径>:<行号>` 形态引据（正则限 .mjs/.md/.json 扩展名 + 行号，
// 裸数字 / 非路径形态不提取，防噪声），逐条查文件存在 + 行号 ≤ 文件行数；问题只入观察列，不产 fail。
// T1（260918-gov-review-batch-drafting-friction，FROZEN 2026-09-19）：citation 裸名引据的前缀候选域 = **并集形态**。
// 现有四前缀**原样保留**（防回归）+ 派生集（`docs/` 与 `.claude/` 树下**递归全部目录**）。
// R1-F1 实测：若改成「一级子目录派生**取代**硬编码」（v0.2 字面），会丢 `docs/` 根（根不是子目录）
// 与 `.claude/scripts/lib/`（两级路径），致已消警引据回退为 file-not-exist；并集形态下回退 0。
// 派生前缀集钉字典序排序（R1-F5：确定性兜底）。按 dir 缓存——自检夹具用临时目录，键不同即重新派生（防假绿）。
const CITATION_BASE_PREFIXES = ['.claude/scripts/', '.claude/scripts/lib/', '.claude/hooks/', 'docs/'];
const citationPrefixCache = new Map();
function citationPrefixes(dir) {
  const key = resolve(dir);
  if (citationPrefixCache.has(key)) return citationPrefixCache.get(key);
  const out = new Set(CITATION_BASE_PREFIXES);
  for (const root of ['docs', '.claude']) {
    const walk = (rel) => {
      let entries;
      try { entries = readdirSync(resolve(dir, rel), { withFileTypes: true }); } catch { return; }
      for (const e of entries) {
        if (!e.isDirectory() || e.name.startsWith('.')) continue;
        out.add(`${rel}/${e.name}/`);
        walk(`${rel}/${e.name}`);
      }
    };
    walk(root);
  }
  const list = [...out].sort();
  citationPrefixCache.set(key, list);
  return list;
}

const CITATION_RE = /`([^\s`]*\.(?:mjs|md|json):\d+)`/g;
function checkCitationAnchors(proposalText, projectDir) {
  const checked = [];
  const problems = [];
  const resolved = [];
  const re = new RegExp(CITATION_RE.source, 'g');
  let m;
  while ((m = re.exec(proposalText)) !== null) {
    const citation = m[1];
    checked.push(citation);
    const mm = citation.match(/^(.*?):(\d+)$/);
    if (!mm) continue;
    const relPath = mm[1];
    const line = parseInt(mm[2], 10);
    const abs = resolve(projectDir, relPath);
    if (!existsSync(abs)) {
      // T2（260907 批）：裸文件名引用前缀重解析——候选前缀依次试探，命中即消警并留痕（resolvedByPrefix 进观察列，不静默）。
      // 260907 热修（human 批准一次性跳过治理批，追认载体 = ARQ-260907-lfc-citation-docs-prefix）：候选域补 `docs/`——
      // 裸 docs 文件名引据（如 `DEV-GOVERNANCE.md:280`）实存而报 file-not-exist（verifier 观察 4②）。
      let resolvedBy = null;
      // T1（260918-gov-review-batch-drafting-friction，FROZEN 2026-09-19）：候选域由硬编码四前缀改
      // **并集形态**（原四前缀 ∪ `docs/`、`.claude/` 树下递归派生集）——治「手工清单随仓结构增长漂移」，
      // 覆盖 references 面与 docs 子目录面裸名引据。消警留痕形态不变（仍走 resolvedByPrefix 观察列）。
      for (const prefix of citationPrefixes(projectDir)) {
        if (existsSync(resolve(projectDir, prefix + relPath))) { resolvedBy = prefix; break; }
      }
      if (resolvedBy) { resolved.push({ citation, resolvedAs: `${resolvedBy}${relPath}:${line}` }); continue; }
      problems.push({ citation, reason: 'file-not-exist' }); continue;
    }
    let lineCount = 0;
    try { lineCount = readFileSync(abs, 'utf8').split('\n').length; }
    catch { problems.push({ citation, reason: 'file-unreadable' }); continue; }
    if (line > lineCount) problems.push({ citation, reason: `line-out-of-range（line ${line} > ${lineCount}）` });
  }
  return { checked, problems, resolved };
}

// ── 冻结前干跑（T2，260918-gov-review-batch-drafting-friction，FROZEN 2026-09-19）──
// **只解析、不对账**：报两类起草期错。不做 §2↔manifest 比对（那是 landing 期的事）。
// 不参与任何门判定、不进任何闸链（PROPOSAL §2 T2 明列）。
//
// 第 1 类 加粗锚行 `**目标文件**：`——解析器只认裸形态，故它在 parser 侧表现为「缺锚清单」；
//   另加专用探针具名报出。**探针须行首锚定 + 限 §2 节域**：全文正扫会误咬正文里「引用该错误形态」
//   的 prose（R2-F8 实证——本批 PROPOSAL 自身即命中多行、全为行内引用的散文），行首锚定 + 限域后为 0。
// 第 2 类 无锚清单（含整行缺失）——由 parser 原生 problems 直接给出。
const BOLD_ANCHOR_RE = /^\s*\*{1,2}\s*目标文件\s*\*{0,2}\s*[:：]/;

// §2 节域切片：`## 2. ...` 行起、至下一个 `## ` 行前止。
function section2Body(text) {
  const lines = text.split(/\r?\n/);
  const start = lines.findIndex((l) => /^##\s*2[.、]?\s/.test(l));
  if (start < 0) return null;
  let end = lines.length;
  for (let i = start + 1; i < lines.length; i++) {
    if (/^##\s/.test(lines[i])) { end = i; break; }
  }
  return lines.slice(start + 1, end);
}

// 纯判定（供自检直接调用）：返回两类错的 findings 列表。
function preflightFindings(text) {
  const body = section2Body(text);
  const boldLines = [];
  if (body) body.forEach((l, i) => { if (BOLD_ANCHOR_RE.test(l)) boldLines.push(i + 1); });
  const problems = parseSection2AnchorLists(text).problems || [];
  const findings = [];
  if (boldLines.length) findings.push({ class: 'bold-anchor-line', lines: boldLines });
  if (problems.length) findings.push({ class: 'missing-anchor-list', detail: problems });
  return { bodyFound: !!body, findings };
}

function runPreflight(proposalPath) {
  const abs = resolve(projectDir, proposalPath);
  if (!existsSync(abs)) {
    print({ ok: false, gate: 'landing-preflight', message: 'PROPOSAL 不存在', proposal: proposalPath });
    process.exit(1);
  }
  const { bodyFound, findings } = preflightFindings(readText(abs));
  print({
    ok: findings.length === 0,
    gate: 'landing-preflight',
    proposal: proposalPath,
    section2BodyFound: bodyFound,
    anchorCount: [...(parseSection2AnchorLists(readText(abs)).union || [])].length,
    findings,
    note: '只读干跑：只保证 §2 锚清单「形态可解析」，不做 §2↔manifest 对账；不参与任何门判定。',
  });
  process.exit(findings.length === 0 ? 0 : 1);
}

// 干跑自检（T2）：① CLI 解析「干跑模式不误触 positional/baseRef」；② 加粗探针「真命中 / 散文不误报 / 限域生效 / 裸形态不误报」。
function runPreflightSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'landing-preflight', label, got, want })); }
  };
  const p = parseCliArgs(['node', 'x', '--preflight', 'some/PROPOSAL.md']);
  check('干跑模式 preflight 取值', p.preflight, 'some/PROPOSAL.md');
  check('干跑模式 positional 为 null（不误触 reviewDir）', p.positional, null);
  check('干跑模式 baseRef 为 null', p.baseRef, null);
  const q = parseCliArgs(['node', 'x', 'dir', 'ref']);
  check('常规模式 positional 不变', q.positional, 'dir');
  check('常规模式 baseRef 不变', q.baseRef, 'ref');
  const boldCls = (t) => preflightFindings(t).findings.filter((f) => f.class === 'bold-anchor-line').length;
  const boldTrue = ['## 2. 改法（T1）', '', '**目标文件**：`.claude/scripts/a.mjs`（改）', '', '## 3. 影响面'].join('\n');
  check('§2 域内行首加粗锚行必命中', boldCls(boldTrue), 1);
  const boldProse = ['## 2. 改法（T1）', '', '改法：把 `**目标文件**：` 这个错误形态修掉（行内引用）。', '', '## 3. 影响面'].join('\n');
  check('散文行内引用不误报（R2-F8）', boldCls(boldProse), 0);
  const boldOutside = ['## 1. 问题', '', '**目标文件**：`.claude/scripts/a.mjs`', '', '## 3. 影响面'].join('\n');
  check('§2 域外不报（限域生效）', boldCls(boldOutside), 0);
  const bareOk = ['## 2. 改法（T1）', '', '目标文件：`.claude/scripts/a.mjs`（改）', '', '## 3. 影响面'].join('\n');
  check('裸形态（正形）不误报', boldCls(bareOk), 0);
  console.log(JSON.stringify({ selfCheck: 'landing-preflight', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// 纯逻辑自检（四例，真空绿：夹具非空、断言 checked/problems 精确计数）。临时夹具文件 → 例后清理。
function runCitationAnchorsSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'citation-anchors', label, got, want })); }
  };
  const fixtureDir = mkdtempSync(join(tmpdir(), 'cit-anchor-'));
  const fixtureAbs = join(fixtureDir, '__fixture__.md');
  writeFileSync(fixtureAbs, 'a\nb\nc\n', 'utf8'); // 3 行
  const fixtureCite = fixtureAbs.replace(/\\/g, '/');
  const nonexistent = join(fixtureDir, 'missing.mjs').replace(/\\/g, '/');
  const text = `见 \`${fixtureCite}:1\` 界内；见 \`${fixtureCite}:99\` 越界；见 \`${nonexistent}:5\` 不存在；见 \`v0.3:2\` 非路径不应提取。`;
  const { checked, problems } = checkCitationAnchors(text, projectDir);
  rmSync(fixtureDir, { recursive: true, force: true });
  check('checked 数（v0.3:2 不提取）', checked.length, 3);
  check('problems 数（越界 + 不存在）', problems.length, 2);
  // T2（260907 批）前缀重解析自检：裸文件名引用 + .claude/scripts/lib/ 夹具 → resolved 非空、零 problems
  const prefixDir = mkdtempSync(join(tmpdir(), 'cit-prefix-'));
  mkdirSync(join(prefixDir, '.claude', 'scripts', 'lib'), { recursive: true });
  writeFileSync(join(prefixDir, '.claude', 'scripts', 'lib', 'fixture.mjs'), 'a\nb\nc\n', 'utf8');
  const prefixText = '见 `fixture.mjs:2`（裸文件名，应经前缀解析命中）。';
  const prefixRes = checkCitationAnchors(prefixText, prefixDir);
  rmSync(prefixDir, { recursive: true, force: true });
  check('prefix-resolved 数', prefixRes.resolved.length, 1);
  check('prefix-resolved 解析形态', prefixRes.resolved[0] && prefixRes.resolved[0].resolvedAs, '.claude/scripts/lib/fixture.mjs:2');
  check('prefix 零 problems', prefixRes.problems.length, 0);
  // 260907 热修自检：docs/ 裸名前缀重解析（ARQ-260907-lfc-citation-docs-prefix）
  const docsDir = mkdtempSync(join(tmpdir(), 'cit-docs-'));
  mkdirSync(join(docsDir, 'docs'), { recursive: true });
  writeFileSync(join(docsDir, 'docs', 'GOV.md'), 'a\nb\nc\n', 'utf8');
  const docsRes = checkCitationAnchors('见 `GOV.md:2`（裸 docs 文件名，应经 docs/ 前缀解析命中）。', docsDir);
  rmSync(docsDir, { recursive: true, force: true });
  check('docs-prefix-resolved 数', docsRes.resolved.length, 1);
  check('docs-prefix 解析形态', docsRes.resolved[0] && docsRes.resolved[0].resolvedAs, 'docs/GOV.md:2');
  check('docs-prefix 零 problems', docsRes.problems.length, 0);
  // 防回归具名声明（T1，260918 批）：上两条（`.claude/scripts/lib/fixture.mjs` 与 `docs/GOV.md`）即
  // PROPOSAL §2 T1 所称「两防回归夹具」——**确认既有件保留并在此具名**，非本批新增（R2-F12 采纳）。
  // 本批新增两夹具：references 面 / docs 子目录面裸名——并集形态后须 resolved。
  const refDir = mkdtempSync(join(tmpdir(), 'cit-ref-'));
  mkdirSync(join(refDir, '.claude', 'references'), { recursive: true });
  writeFileSync(join(refDir, '.claude', 'references', 'dev-loop-operating-system.md'), 'a\nb\nc\n', 'utf8');
  const refRes = checkCitationAnchors('见 `dev-loop-operating-system.md:2`（references 面裸名）。', refDir);
  rmSync(refDir, { recursive: true, force: true });
  check('references-prefix-resolved 数', refRes.resolved.length, 1);
  check('references-prefix 解析形态', refRes.resolved[0] && refRes.resolved[0].resolvedAs, '.claude/references/dev-loop-operating-system.md:2');
  check('references-prefix 零 problems', refRes.problems.length, 0);
  const tplDir = mkdtempSync(join(tmpdir(), 'cit-tpl-'));
  mkdirSync(join(tplDir, 'docs', 'templates'), { recursive: true });
  writeFileSync(join(tplDir, 'docs', 'templates', 'PROPOSAL.template.md'), 'a\nb\nc\n', 'utf8');
  const tplRes = checkCitationAnchors('见 `PROPOSAL.template.md:2`（docs 子目录面裸名）。', tplDir);
  rmSync(tplDir, { recursive: true, force: true });
  check('templates-prefix-resolved 数', tplRes.resolved.length, 1);
  check('templates-prefix 解析形态', tplRes.resolved[0] && tplRes.resolved[0].resolvedAs, 'docs/templates/PROPOSAL.template.md:2');
  check('templates-prefix 零 problems', tplRes.problems.length, 0);
  console.log(JSON.stringify({ selfCheck: 'citation-anchors', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// ── lint-descriptions 触发式附跑（260826-gov-review-claude-consistency-batch3 T2，advisory-only）──
// 纯判定「何时触发」（diff 面含 .claude/agents/**）；接线只取输出不取退出码（该脚本恒 exit 0 = warning-level）。
function shouldRunLintDescriptions(changedFiles) {
  return [...changedFiles].some((f) => f.startsWith('.claude/agents/'));
}

function runLintDescriptionsTriggerSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'lint-descriptions-trigger', label, got, want })); }
  };
  check('agents 面在 → 触发', shouldRunLintDescriptions(new Set(['.claude/agents/foo.md'])), true);
  check('agents 面不在 → 不触发', shouldRunLintDescriptions(new Set(['.claude/scripts/x.mjs'])), false);
  console.log(JSON.stringify({ selfCheck: 'lint-descriptions-trigger', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// ── §2 钉死引号块 ↔ 落地文件机械逐字对账（260826-gov-review-g3-infra-convergence T2，advisory-only）──
// 提取 PROPOSAL §2 各 T 节内围栏代码块（```）正文（作者约定「原文钉死」最可机辨形态），逐块在 landing-manifest
// 目标文件集中做字节级 includes 对账；问题只入观察列、不产 fail（力度 = OQ-2 推荐 (a) advisory 起步）。
function checkVerbatimQuotes(proposalText, projectDir, targetFiles) {
  const checked = [];
  const missing = [];
  const secStart = proposalText.search(/^##\s*2[.、]/m);
  if (secStart === -1) return { checked, missing };
  const secRest = proposalText.slice(secStart);
  const secEnd = secRest.search(/\n##\s/m);
  const secText = secEnd === -1 ? secRest : secRest.slice(0, secEnd);
  const fenceRe = /```[a-zA-Z]*\n([\s\S]*?)```/g;
  let m;
  while ((m = fenceRe.exec(secText)) !== null) {
    const block = m[1];
    if (!block.trim()) continue;
    let found = false;
    for (const target of targetFiles) {
      const abs = resolve(projectDir, target);
      if (!existsSync(abs)) continue;
      let content;
      try { content = readFileSync(abs, 'utf8'); } catch { continue; }
      // T2（260907 批）：双侧行尾归一后比对——仓内 .mjs 工作树全量 CRLF（autocrlf 实况）、PROPOSAL 经 Write 落盘 LF，
      // 逐字节 includes 必假（260906-gov-review-index-status-truth-source 批 2 条 advisory 假失配实证）。
      const norm = (s) => s.replace(/\r\n/g, '\n');
      if (norm(content).includes(norm(block))) { found = true; break; }
    }
    checked.push(block);
    if (!found) missing.push({ block: block.trim().slice(0, 120), reason: 'not-found-verbatim-in-any-target' });
  }
  return { checked, missing };
}

function runVerbatimQuotesSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'verbatim-quotes', label, got, want })); }
  };
  const fixtureDir = mkdtempSync(join(tmpdir(), 'vq-'));
  const block = 'const EXACT = 1;\nconst KEEP = 2;';
  writeFileSync(join(fixtureDir, 'target.mjs'), `prefix\n${block}\nsuffix\n`, 'utf8');
  const proposal = ['## 2. 改法', '### T1 · 示例', '```js', block, '```', '## 3. 影响面'].join('\n');
  const okRes = checkVerbatimQuotes(proposal, fixtureDir, ['target.mjs']);
  check('verbatim-present-missing', okRes.missing.length, 0);
  check('verbatim-present-checked', okRes.checked.length, 1);
  const driftRes = checkVerbatimQuotes(proposal.replace('const KEEP = 2;', 'const KEEP = 3;'), fixtureDir, ['target.mjs']);
  check('verbatim-drift-missing', driftRes.missing.length, 1);
  const noFenceRes = checkVerbatimQuotes('# t\n## 2. 改法\n无代码块。\n## 3.', fixtureDir, ['target.mjs']);
  check('verbatim-no-fence-checked', noFenceRes.checked.length, 0);
  // T2（260907 批）CRLF 自检：LF 围栏块 × CRLF 目标文件 → 归一后命中（归一前必假，防回归）
  const crlfTarget = mkdtempSync(join(tmpdir(), 'vq-crlf-'));
  writeFileSync(join(crlfTarget, 'target.cjs'), `prefix\r\n${block.replace(/\n/g, '\r\n')}\r\nsuffix\r\n`, 'utf8');
  const crlfRes = checkVerbatimQuotes(proposal, crlfTarget, ['target.cjs']);
  check('verbatim-crlf-missing', crlfRes.missing.length, 0);
  check('verbatim-crlf-checked', crlfRes.checked.length, 1);
  rmSync(crlfTarget, { recursive: true, force: true });
  rmSync(fixtureDir, { recursive: true, force: true });
  console.log(JSON.stringify({ selfCheck: 'verbatim-quotes', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// 自检接线顺序审查（真空绿规约）：runIsFrozenSelfCheck 为终端式（无条件 process.exit），必须列最后；
// 其余非终端式自检（含 B7 新增 section2Anchors、T2 新增 verbatim-quotes、260918 批新增 landing-preflight）排在其前，否则永不执行。
if (process.argv.includes('--self-check')) { runExtractTargetsSelfCheck(); runSection2AnchorSelfCheck(); runResolveOriginSelfCheck(); runComputeFrozenPinSelfCheck(); runCitationAnchorsSelfCheck(); runPreflightSelfCheck(); runLintDescriptionsTriggerSelfCheck(); runVerbatimQuotesSelfCheck(); runIsFrozenSelfCheck(); }

// 干跑入口（T2，260918 批）：只读、不对账、不参与任何门判定、不进任何闸链。
// 置于 --self-check 之后（两者同给时先跑自检并以其退出码为准）。
if (process.argv.indexOf('--preflight') >= 0) {
  if (!preflightArg) {
    print({ ok: false, gate: 'landing-preflight', message: '--preflight 需要一个 PROPOSAL 路径参数' });
    process.exit(1);
  }
  runPreflight(preflightArg);
}

function runGit(cmd) {
  try {
    return execSync(cmd, { cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
  } catch (error) {
    return null;
  }
}

function addNames(out, files) {
  for (const rawLine of (out || '').split(/\r?\n/)) {
    let path = rawLine.trim().replace(/^"|"$/g, '');
    if (path.includes(' -> ')) path = path.split(' -> ').pop();
    path = path.replace(/\/$/, '');
    if (path) files.add(path);
  }
}

// 「已落地」= 工作树改动 ∪ 已提交改动。
// 这样脚本在两个时点都能正确判定：architect 落完**未提交**（改动在工作树）与**已提交/已合并**（改动进了 commit）。
// 已提交改动来源：给了 baseRef → `git diff --name-only <baseRef>`（相对 baseRef 的全部改动，含未提交）；
// 否则默认最近一次 commit/merge（`git diff --name-only HEAD~1 HEAD`，merge commit 取相对第一父的差）。
function gitChangedFiles(sinceRef) {
  const files = new Set();
  const porcelain = runGit('git status --porcelain -uall'); // -uall：untracked 文件逐个列出（默认把未跟踪目录折叠成目录条目 → 新目录内新增文件会被误判 not-landed；本批 dogfood 发现并修）
  if (porcelain === null) return { files, degraded: true, reason: 'git status failed' };
  // porcelain 行首两字符是 XY 状态码，路径从第 4 列起。
  for (const rawLine of porcelain.split(/\r?\n/)) {
    if (!rawLine.trim()) continue;
    let path = rawLine.slice(3).trim();
    if (path.includes(' -> ')) path = path.split(' -> ').pop();
    path = path.replace(/^"|"$/g, '').replace(/\/$/, '');
    if (path) files.add(path);
  }
  const committed = sinceRef ? runGit(`git diff --name-only ${sinceRef}`) : runGit('git diff --name-only HEAD~1 HEAD');
  addNames(committed, files);
  return { files, degraded: false };
}

if (!reviewDir) {
  fail('review directory argument is required', {
    usage: 'node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/gov-review-<id>',
  });
}
if (!existsSync(reviewDir)) fail('review directory does not exist', { reviewDir });

const proposalPath = resolve(reviewDir, 'PROPOSAL.md');
const summaryPath = resolve(reviewDir, 'architect-landing-summary.md');
const relDir = `delivery/changes/arch/${basename(reviewDir)}`;

if (!existsSync(proposalPath)) fail('PROPOSAL.md is required（frozen 权威设计 + landing 清单）', { reviewDir: relDir });
if (!existsSync(summaryPath)) fail('architect-landing-summary.md is required（landing 声称 + packet-binding）', { reviewDir: relDir });

const proposalText = readText(proposalPath);
const summaryText = readText(summaryPath);
const errors = [];

if (!isFrozen(proposalText)) {
  errors.push('PROPOSAL.md 未冻结：Status 行缺 FROZEN 标记（landing 保真度以 frozen 稿为准；仅 Status 行有效，正文提及不算）');
}
if (!/packet-id/i.test(summaryText) || !/(?:gov|arch)-review-[A-Za-z0-9-]+/i.test(summaryText)) {
  errors.push('architect-landing-summary.md 缺 packet-binding（须引 packet-id + 被批 review id，A9）');
}

// B2 fail-loud：清单来源 = manifest 围栏块；缺块 = 格式不合格 RED（err.code 可辨，非正则兜底）。
let proposalTargets;
let summaryTargets;
try {
  proposalTargets = new Set(extractTargets(proposalText, { source: 'PROPOSAL.md' }));
  summaryTargets = new Set(extractTargets(summaryText, { source: 'architect-landing-summary.md' }));
} catch (error) {
  if (error.code === 'MISSING_LANDING_MANIFEST') fail(error.message, { reviewDir: relDir });
  throw error;
}

// T3（260824-gov-review-track-b-drafting-hardening-2）：§2 各 T 目标文件锚清单并集 ≡ manifest 围栏块文件集一致性断言。
// 插入位置 = 闸前置链（PROPOSAL/summary 存在 → packet-binding → 双侧 manifest 块在场）之后、manifest ↔ git 改动比对之前；
// 仅加断言、既有判据不动。解析语义（显式锚清单对账 / backtick 必须 / 迁移锚读 B / 条件锚裁剪 / 或锚、缺锚、歧义锚
// fail-loud）单源 governance-gate-shared.mjs B7；problems 进 errors（非冻结亦可达，不提前硬失败）。
{
  const { union: section2Union, problems: anchorProblems } = parseSection2AnchorLists(proposalText);
  for (const problem of anchorProblems) {
    errors.push(`PROPOSAL §2 锚清单：${problem}`);
  }
  if (anchorProblems.length === 0) {
    const { onlyInSection2, onlyInManifest, aligned } = compareAnchorSets(section2Union, proposalTargets);
    if (!aligned) {
      errors.push(
        `PROPOSAL §2 锚清单 ≢ manifest 围栏块（对账失配）：仅 §2 有：${onlyInSection2.join(', ') || '（无）'}；仅 manifest 有：${onlyInManifest.join(', ') || '（无）'}`
      );
    }
  }
}

// 4. summary ⊆ proposal（不许落地超出 PROPOSAL 授权）。
const unauthorized = [...summaryTargets].filter((t) => !proposalTargets.has(t));
if (unauthorized.length > 0) {
  errors.push(`landing-summary 声称落了 PROPOSAL 未授权的文件（scope creep）：${unauthorized.join(', ')}`);
}

// git 改动集（工作树 ∪ 已提交；见 gitChangedFiles）。
const { files: changed, degraded, reason } = gitChangedFiles(baseRef);

// 5. summary 声称落的 → git 里有改动。
const claimedNotLanded = [...summaryTargets].filter((t) => !changed.has(t));
// 6. PROPOSAL 清单 → git 里有改动（无静默漏落）。
const plannedNotLanded = [...proposalTargets].filter((t) => !changed.has(t));

if (!degraded) {
  if (claimedNotLanded.length > 0) {
    errors.push(`landing-summary 声称落了、但 git 工作树无改动的文件（claimed-but-not-landed）：${claimedNotLanded.join(', ')}`);
  }
  if (plannedNotLanded.length > 0) {
    errors.push(`PROPOSAL 清单里、但 git 工作树无改动的文件（planned-but-not-landed，静默漏落）：${plannedNotLanded.join(', ')}`);
  }
}

if (errors.length > 0) {
  const notLanded = claimedNotLanded.length > 0 || plannedNotLanded.length > 0;
  const hint =
    !baseRef && notLanded
      ? 'worktree / 最近一次 commit 未见这些目标——landing 可能已提交/合并到更早的 commit。用 baseRef=<landing 前的 ref> 重跑以审计已提交的 landing（如 git log 找到 landing 前的 commit）。若 worktree 确有未提交 landing 却仍报此错，才是真漏落。'
      : undefined;
  fail('governance landing 保真度核验未过', { reviewDir: relDir, errors, gitDegraded: degraded, hint });
}

// --- Cross-surface alignment gate (GAR-001) ---
// 本仓实况（2026-08-31）：cross-surface-check.mjs 未自原仓迁入（其输入面 route-fixtures/
// 与 dev-loop.settings.json 本仓均不存在），子门降级为「脚本在场才跑」。原仓全量版见
// c:\AI\AInativecompany\.claude\scripts\cross-surface-check.mjs；若将来 dev-loop 面迁齐，
// 把脚本补迁回本目录即自动恢复启用。登记：ARQ-260831-cross-surface-check-missing（已结）。
const shouldRunCrossSurface = [...changed].some(
  (file) =>
    file.startsWith('.claude/agents/') ||
    file === '.claude/scope-map.json' ||
    /^\.claude\/[^/]+\.settings\.json$/.test(file) ||
    file.startsWith('.claude/route-fixtures/') ||
    file.startsWith('.claude/scripts/') ||
    file.startsWith('.claude/hooks/'),
);
const crossSurfaceScript = resolve(projectDir, '.claude/scripts/cross-surface-check.mjs');
if (shouldRunCrossSurface && existsSync(crossSurfaceScript)) {
  const result = spawnSync(process.execPath, [crossSurfaceScript, '--json'], {
    cwd: projectDir,
    encoding: 'utf8',
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  if (result.status !== 0) {
    fail('cross-surface alignment sub-gate failed', {
      subGate: 'cross-surface-check',
      subExitCode: result.status,
      subOutput: result.stdout?.trim() || result.stderr?.trim() || '(no output)',
    });
  }
}
// --- End cross-surface alignment gate (GAR-001) ---

// --- lint-descriptions 触发式附跑（260826-gov-review-claude-consistency-batch3 T2，advisory-only）---
// 判定复用 changed 集：diff 面含 .claude/agents/** 时附跑一次；只取输出不取退出码（恒 exit 0 的 warning-level lint），
// 不因它改判；diff 面不含 agents 时不跑（零开销）。
const lintDescriptionsAdvisory = (() => {
  if (!shouldRunLintDescriptions(changed)) {
    return { ran: false, note: 'landing diff 面不含 .claude/agents/**，未附跑 lint-descriptions' };
  }
  const lintScript = resolve(projectDir, '.claude/scripts/lint-descriptions.mjs');
  const res = spawnSync(process.execPath, [lintScript], {
    cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
  });
  const out = (res.stdout?.trim() || res.stderr?.trim() || '(no output)').slice(0, 4000);
  return { ran: true, exitCode: res.status, isExit0Expected: true, output: out };
})();

// --- 引据锚点观察列（260826-gov-review-claude-consistency-batch3 T1，advisory-only）---
const { checked: citationChecked, problems: citationProblems, resolved: citationResolved } = checkCitationAnchors(proposalText, projectDir);
const citationAnchorAdvisory = { checked: citationChecked.length, problems: citationProblems, resolvedByPrefix: citationResolved };

// --- §2 钉死引号块逐字对账观察列（260826-gov-review-g3-infra-convergence T2，advisory-only）---
const verbatimQuoteAdvisory = checkVerbatimQuotes(proposalText, projectDir, [...proposalTargets]);

// advisory：git 改动里、不在 PROPOSAL 清单的 canonical 文件（可能其它未提交 landing，交 moderator/人看）。
const changedNotInProposal = [...changed].filter((f) => CANONICAL_TEST.test(f) && !proposalTargets.has(f));

print({
  ok: true,
  gate: 'landing-fidelity',
  reviewDir: relDir,
  baseRef: baseRef || 'HEAD~1 (default: worktree ∪ last commit/merge)',
  frozen: true,
  packetBinding: true,
  proposalTargets: [...proposalTargets],
  summaryTargets: [...summaryTargets],
  unauthorized: [],
  claimedNotLanded: degraded ? 'skipped-git-degraded' : [],
  plannedNotLanded: degraded ? 'skipped-git-degraded' : [],
  gitDegraded: degraded,
  citationAnchorAdvisory,
  lintDescriptionsAdvisory,
  verbatimQuoteAdvisory,
  advisory: {
    changedCanonicalNotInProposal: changedNotInProposal,
    note: degraded
      ? `git 状态不可用（${reason}）：claimed/planned-landed 机检跳过，需人工核对；结构授权检查已过。`
      : '结构保真已过；语义保真（改法对不对 / 有无私货）归 landing-verifier 人读 diff。changedCanonicalNotInProposal 可能是同工作树其它未提交 landing，交 moderator/人确认。',
  },
});
