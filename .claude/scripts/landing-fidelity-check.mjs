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
// 用法：node .claude/scripts/landing-fidelity-check.mjs delivery/review/gov-review-<id> [baseRef]
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
//
// T1（260920-gov-review-citation-anchor-residue，FROZEN 2026-09-20）：候选域由「硬编码四项 ∪ 两树递归派生」
// 改为**由版本控制面派生**——语义 = 「仓内可被引用的目录」= 每个被跟踪 / 未被忽略的未跟踪文件的全部祖先目录。
// 零手工清单；build / vendor 产物因 `.gitignore` 自动排除；新增目录自动入集。治「逐批现撞现补前缀」——
// 该面此前已四轮扩域（260826 初版 → 260907 三候选前缀 → 260907 热修补 `docs/` → 260918 并集形态 + 两树递归）。
// R1-F1 实测更正：四项常量在 git 派生域下均被覆盖，但**两树递归域不含 `docs/` 与 `.claude/` 两个根**
// （walk-only 42 项，两根均不在集）——故四项常量**保留于回退分支**、正常 git 路径不消费它。
// 失败即留痕：git 不可用 → 回退静态派生并把 prefixSource 记为 'fallback-static'（不静默降级）。
// 派生前缀集钉字典序排序（确定性兜底）。按 dir 缓存——自检夹具用临时目录，键不同即重新派生（防假绿）。
const CITATION_BASE_PREFIXES = ['.claude/scripts/', '.claude/scripts/lib/', '.claude/hooks/', 'docs/'];
const citationPrefixCache = new Map();

// 回退分支专用：四项常量 ∪ `docs/`、`.claude/` 两树递归（**不含**两个根，靠常量补）。
function staticCitationPrefixes(dir) {
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
  return [...out].sort();
}

// 正常分支：git 索引面派生。任一 git 调用失败 → 返回 null（调用方转回退并留痕）。
function gitCitationPrefixes(dir) {
  const out = new Set();
  const addPath = (p) => {
    const segs = p.split('/');
    segs.pop(); // 去掉文件名，只取祖先目录
    let acc = '';
    for (const seg of segs) { acc += seg + '/'; out.add(acc); }
  };
  for (const args of [['ls-files', '-z'], ['ls-files', '-z', '--others', '--exclude-standard']]) {
    const res = spawnSync('git', args, { cwd: dir, encoding: 'utf8', maxBuffer: 1 << 28 });
    if (res.status !== 0 || typeof res.stdout !== 'string') return null;
    for (const p of res.stdout.split('\0')) if (p) addPath(p);
  }
  return [...out].sort();
}

// 返回 { prefixes, source }——source 进观察列 prefixSource，读者据此判读候选域宽度。
function citationPrefixes(dir) {
  const key = resolve(dir);
  if (citationPrefixCache.has(key)) return citationPrefixCache.get(key);
  const viaGit = gitCitationPrefixes(dir);
  const entry = viaGit
    ? { prefixes: viaGit, source: 'git-index' }
    : { prefixes: staticCitationPrefixes(dir), source: 'fallback-static' };
  citationPrefixCache.set(key, entry);
  return entry;
}

const CITATION_RE = /`([^\s`]*\.(?:mjs|md|json):\d+)`/g;
// T2（260920 批，FROZEN 2026-09-20）：结果由单一 `problems` 一分为六列。判定顺序（钉死，R2 后定）：
//   ① 字面直中 → 不报；直中但行号越界 / 不可读 → 仍进 problems（两类 reason 原样保留，不被本批触碰）
//   ② 路径段含 `…` / `...` → abbreviated（**必须先于前缀试探**，否则会命中碰巧同名文件而假绿）
//   ③ 前缀试探唯一命中 → resolvedByPrefix   ④ ≥2 命中 → ambiguousShortForm（向严，不取首个）
//   ⑤ 0 命中且路径首段是仓内实存一级目录 → problems 的 file-not-exist（真·断链）
//   ⑥ 0 命中且非仓根相对形态 → shortFormUnresolved（附补救句）
// 「一级目录集合」取自**同一份派生前缀集的一级段**（同源同缓存，不另造事实源）；回退域下该集合随之收窄
// （只有 docs / .claude），故 `delivery/...` 型引据会由 ⑤ 落 ⑥——**是分类漂移、不是假绿**（两类都不进 errors）。
function checkCitationAnchors(proposalText, projectDir) {
  const checked = [];
  const problems = [];
  const resolved = [];
  const abbreviated = [];
  const ambiguousShortForm = [];
  const shortFormUnresolved = [];
  const { prefixes, source: prefixSource } = citationPrefixes(projectDir);
  const topLevelDirs = new Set(prefixes.map((p) => p.split('/')[0]));
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
    if (existsSync(abs)) {
      let lineCount = 0;
      try { lineCount = readFileSync(abs, 'utf8').split('\n').length; }
      catch { problems.push({ citation, reason: 'file-unreadable' }); continue; }
      if (line > lineCount) problems.push({ citation, reason: `line-out-of-range（line ${line} > ${lineCount}）` });
      continue;
    }
    if (/[…]|\.\.\./.test(relPath)) { abbreviated.push({ citation }); continue; }
    const hits = prefixes.filter((prefix) => existsSync(resolve(projectDir, prefix + relPath)));
    if (hits.length === 1) { resolved.push({ citation, resolvedAs: `${hits[0]}${relPath}:${line}` }); continue; }
    if (hits.length > 1) { ambiguousShortForm.push({ citation, hits }); continue; }
    if (topLevelDirs.has(relPath.split('/')[0])) {
      problems.push({ citation, reason: 'file-not-exist' });
      continue;
    }
    shortFormUnresolved.push({ citation, hint: '建议写全仓相对路径，或核对是否已迁走' });
  }
  return { checked, problems, resolved, abbreviated, ambiguousShortForm, shortFormUnresolved, prefixSource };
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

// 围栏状态机（T3 子项 b，260920-gov-review-citation-anchor-residue，FROZEN 2026-09-20）：标记每行是否处于
// fenced code block 内（**含开合行自身**）。§2 的围栏块 = 示范 / 原文钉死内容，**不是声明**——该立场有仓内
// 既有用法支撑：`checkVerbatimQuotes` 早已把 §2 围栏块正文当「作者原文钉死的目标文件内容」做字节级对账。
function fenceFlags(lines) {
  const flags = new Array(lines.length).fill(false);
  let inFence = false;
  for (let i = 0; i < lines.length; i++) {
    if (/^\s*```/.test(lines[i])) { flags[i] = true; inFence = !inFence; continue; }
    flags[i] = inFence;
  }
  return flags;
}

// §2 节域切片：`## 2. ...` 行起、至下一个 `## ` 行前止。
// 返回值 = `{ body, offset, inFence }`——offset 供报**文件绝对行号**（T3 子项 a：v0.1 报的是节域内相对号，
// 作者须心算偏移才能定位）；inFence 为与 body 等长的围栏标记切片（T3 子项 b）。
function section2Body(text) {
  const lines = text.split(/\r?\n/);
  const start = lines.findIndex((l) => /^##\s*2[.、]?\s/.test(l));
  if (start < 0) return null;
  const flags = fenceFlags(lines);
  let end = lines.length;
  for (let i = start + 1; i < lines.length; i++) {
    if (flags[i]) continue; // 261026 批 T1：边界探测跳围栏（与 parseSection2AnchorLists 同机制）
    if (/^##\s/.test(lines[i])) { end = i; break; }
  }
  return { body: lines.slice(start + 1, end), offset: start + 1, inFence: flags.slice(start + 1, end) };
}

// 纯判定（供自检直接调用）：返回两类错的 findings 列表。行号一律**文件绝对行号**。
function preflightFindings(text) {
  const sec = section2Body(text);
  const boldLines = [];
  if (sec) {
    sec.body.forEach((l, i) => {
      if (sec.inFence[i]) return; // 围栏内的加粗锚行是示范文本，不是声明
      if (BOLD_ANCHOR_RE.test(l)) boldLines.push(i + 1 + sec.offset);
    });
  }
  const problems = parseSection2AnchorLists(text).problems || [];
  const findings = [];
  if (boldLines.length) findings.push({ class: 'bold-anchor-line', lines: boldLines });
  if (problems.length) findings.push({ class: 'missing-anchor-list', detail: problems });
  return { bodyFound: !!sec, findings };
}

// T9①（260924-gov-review-gate-parse-teeth）：目录 token advisory——preflight 接受目录锚、
// landing 门逐文件精确匹配（changed.has）不消费目录锚（260923 批实录：`docs/crq/` FROZEN 后首跑结构门 RED
// 两条 → human 追认 + 机械展开 99 行）。本 advisory 把反馈前移回起草期；**不入 findings、不动 ok/exit**。
function dirTokenAdvisories(union) {
  return [...(union || [])]
    .filter((t) => String(t).endsWith('/'))
    .map((t) => `锚并集含目录 token「${t}」：landing 门不消费目录锚（逐文件精确匹配 changed.has），manifest 须逐件枚举成员（迁移批先例：260914 批 139 行全枚举）`);
}

function runPreflight(proposalPath) {
  const abs = resolve(projectDir, proposalPath);
  if (!existsSync(abs)) {
    print({ ok: false, gate: 'landing-preflight', message: 'PROPOSAL 不存在', proposal: proposalPath });
    process.exit(1);
  }
  const text = readText(abs);
  const { bodyFound, findings } = preflightFindings(text);
  const union = [...(parseSection2AnchorLists(text).union || [])].sort();
  print({
    ok: findings.length === 0,
    gate: 'landing-preflight',
    proposal: proposalPath,
    section2BodyFound: bodyFound,
    anchorCount: union.length,
    // T3 子项 c：锚并集**成员名单**（不只是个数）——纯可见面，不是判据，`ok` 判定一字不动。
    // 治「锚行注记夹带路径 token」那一类（三轮证伪后缩圈移出、判据不机械化）：作者在起草期即可
    // 看到「哪些 token 进了锚集」，260912 那种注记夹带当场可见（该次锚并集 10 项 vs manifest 9 行）。
    anchorUnion: union,
    advisories: dirTokenAdvisories(union),
    findings,
    note: '只读干跑：只保证 §2 锚清单「形态可解析」，不做 §2↔manifest 对账；不参与任何门判定。',
  });
  process.exit(findings.length === 0 ? 0 : 1);
}

// 干跑自检（T2）：① CLI 解析「干跑模式不误触 positional/baseRef」；② 加粗探针「真命中 / 散文不误报 / 限域生效 / 裸形态不误报」；
// ③ T3 子项 a「报文件绝对行号」；④ T3 子项 b「跳围栏」；⑤ T3 子项 c「anchorUnion 可见面」。
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
  // ③ 行号精度（T3 子项 a，260920 批）：报**文件绝对行号**，非 §2 节域内相对号。
  // 夹具刻意让二者相差 ≥3：§2 标题在第 3 行、加粗锚行在第 6 行；旧行为报 3，新行为须报 6。
  const absLines = ['# 夹具', '', '## 2. 改法（T1）', '### T1 · 甲', '正文一句。', '**目标文件**：`.a.mjs`（改）', '', '## 3. 影响面'];
  const absText = absLines.join('\n');
  const absReported = (preflightFindings(absText).findings.find((f) => f.class === 'bold-anchor-line') || {}).lines || [];
  check('行号·报文件绝对行号（非节域相对号）', absReported[0], 6);
  check('行号·与环境真身行号一致', absLines.indexOf('**目标文件**：`.a.mjs`（改）') + 1, 6);
  // ④ 跳围栏（T3 子项 b）：§2 围栏内含 `### T<n>` 示范条目 + **裸** `目标文件：` 行。
  // 旧行为：造幻影条目、把真 T1 判成缺锚、并让围栏内 token 进锚并集 —— 本夹具须三项全消。
  const fenced = [
    '## 2. 改法（T1）',
    '### T1 · 甲',
    '正文一句。',
    '目标文件：`.claude/scripts/a.mjs`（改）',
    '',
    '```md',
    '### T1 · 围栏内示范条目',
    '目标文件：docs/templates/PROPOSAL.template.md（改）',
    '```',
    '',
    '## 3. 影响面',
  ].join('\n');
  const fencedProblems = parseSection2AnchorLists(fenced).problems.filter((x) => /^缺锚清单/.test(x));
  check('跳围栏·真 T1 不被判成缺锚（无幻影条目吞锚）', fencedProblems.length, 0);
  const fencedUnion = [...parseSection2AnchorLists(fenced).union].sort();
  check('跳围栏·锚并集只含真锚行路径', JSON.stringify(fencedUnion), JSON.stringify(['.claude/scripts/a.mjs']));
  check('跳围栏·围栏内 token 不入并集', fencedUnion.includes('docs/templates/PROPOSAL.template.md'), false);
  // 围栏内加粗锚行同样是示范文本，不得被加粗探针报出
  const fencedBold = ['## 2. 改法（T1）', '### T1 · 甲', '目标文件：`.a.mjs`（改）', '', '```md', '**目标文件**：`.b.mjs`（改）', '```', '', '## 3. 影响面'].join('\n');
  check('跳围栏·围栏内加粗锚行不报', boldCls(fencedBold), 0);
  // ⑤ anchorUnion 可见面（T3 子项 c）：输出的并集成员名单须与 anchorCount 一致。
  // 夹具须含 `### T<n>` 条目头——无条目时 parser 按 fail-loud 返回空并集 + 「无任何 T 条目」problem。
  const unionOk = ['## 2. 改法（T1）', '### T1 · 甲', '目标文件：`.claude/scripts/a.mjs`（改）', '', '## 3. 影响面'].join('\n');
  const unionDemo = [...parseSection2AnchorLists(unionOk).union].sort();
  check('anchorUnion·与 anchorCount 一致', unionDemo.length, 1);
  check('anchorUnion·成员名单内容', JSON.stringify(unionDemo), JSON.stringify(['.claude/scripts/a.mjs']));
  // ⑥ T9①（260924-gov-review-gate-parse-teeth）判别：目录 token advisory——含目录 token 非空、纯文件 token 空
  // （反转实现必须变红：若 advisory 误入 findings 则 runPreflight 的 ok 翻转，V9 live 断言另有覆盖）。
  const dirAdv = dirTokenAdvisories(['docs/crq/', '.claude/scripts/a.mjs']);
  check('目录 token advisory·非空且点名', dirAdv.length === 1 && dirAdv[0].includes('docs/crq/'), true);
  check('目录 token advisory·纯文件 token 为空', dirTokenAdvisories(['.claude/scripts/a.mjs']).length, 0);
  // 261026 批 T1：边界探测跳围栏——围栏内「## 」行不得截断 §2 域，其后加粗锚行仍须报出（绝对行号 9）。
  // 判别力：反转实现（section2Body 边界不跳围栏）时该行被截断出域、boldCls = 0 变红。
  const fenceBoundaryBold = ['# 夹具', '## 2. 改法（T1）', '### T1 · 甲', '目标文件：`.claude/scripts/a.mjs`（改）', '```md', '## 围栏内节标题', '```', '### T2 · 乙', '**目标文件**：`docs/b.md`（改）', '## 3. 影响面'].join('\n');
  const fbb = (preflightFindings(fenceBoundaryBold).findings.find((f) => f.class === 'bold-anchor-line') || {}).lines || [];
  check('跳围栏·围栏后加粗锚行仍报（绝对行号 9）', fbb[0], 9);
  console.log(JSON.stringify({ selfCheck: 'landing-preflight', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// 纯逻辑自检（真空绿：夹具非空、断言逐项精确计数）。临时夹具目录 → 例后清理。
// T1（260920 批）后引据夹具**须是真 git 仓**（候选域改由 git 索引面派生，非 git 目录走回退域、测不到主路径）。
function makeGitFixtureRepo(prefix) {
  const dir = mkdtempSync(join(tmpdir(), prefix));
  execSync('git init -q', { cwd: dir, stdio: 'ignore' });
  return dir;
}
function gitAddFixture(dir) {
  // autocrlf 静音：夹具文件由 writeFileSync 落 LF，Windows 全局 core.autocrlf=true 时 git add 会往 stderr 吐 CRLF 警告。
  execSync('git -c core.autocrlf=false -c core.safecrlf=false add -A', { cwd: dir, stdio: 'ignore' });
}
function runCitationAnchorsSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'citation-anchors', label, got, want })); }
  };
  // ── 例 1：提取与行号越界（非 git 夹具，直中分支不依赖候选域）──
  const fixtureDir = mkdtempSync(join(tmpdir(), 'cit-anchor-'));
  const fixtureAbs = join(fixtureDir, '__fixture__.md');
  writeFileSync(fixtureAbs, 'a\nb\nc\n', 'utf8'); // 3 行
  const fixtureCite = fixtureAbs.replace(/\\/g, '/');
  const nonexistent = join(fixtureDir, 'missing.mjs').replace(/\\/g, '/');
  const text = `见 \`${fixtureCite}:1\` 界内；见 \`${fixtureCite}:99\` 越界；见 \`${nonexistent}:5\` 不存在；见 \`v0.3:2\` 非路径不应提取。`;
  const { checked, problems } = checkCitationAnchors(text, projectDir);
  rmSync(fixtureDir, { recursive: true, force: true });
  check('checked 数（v0.3:2 不提取）', checked.length, 3);
  check('problems 数（越界 1 条；不存在那条归 shortFormUnresolved 非 problems）', problems.length, 1);
  check('problems reason = line-out-of-range', problems[0] && /^line-out-of-range/.test(problems[0].reason), true);

  // ── 例 2：六列分类族（真 git 仓）+ gitignore 正控 ──
  const repo = makeGitFixtureRepo('cit-classify-');
  mkdirSync(join(repo, 'docs', 'templates'), { recursive: true });
  mkdirSync(join(repo, 'delivery', 'workstreams', 'm1'), { recursive: true });
  mkdirSync(join(repo, 'a'), { recursive: true });
  mkdirSync(join(repo, 'b'), { recursive: true });
  mkdirSync(join(repo, 'build', 'sub'), { recursive: true });
  writeFileSync(join(repo, '.gitignore'), 'build/\n', 'utf8');
  writeFileSync(join(repo, 'docs', 'templates', 'PROPOSAL.template.md'), 'a\nb\nc\n', 'utf8');
  writeFileSync(join(repo, 'docs', 'gone.md'), 'a\nb\nc\n', 'utf8');
  writeFileSync(join(repo, 'delivery', 'workstreams', 'm1', 'short.mjs'), 'a\nb\nc\n', 'utf8');
  writeFileSync(join(repo, 'delivery', 'workstreams', 'm1', 'progress.md'), 'a\nb\nc\n', 'utf8');
  writeFileSync(join(repo, 'a', 'progress.md'), 'a\nb\nc\n', 'utf8');
  writeFileSync(join(repo, 'b', 'progress.md'), 'a\nb\nc\n', 'utf8');
  writeFileSync(join(repo, 'build', 'sub', 'gen.mjs'), 'a\nb\nc\n', 'utf8'); // gitignore 覆盖 → build/ 不得成为候选前缀
  gitAddFixture(repo);
  const classifyText = [
    '见 `docs/templates/PROPOSAL.template.md:2`（字面直中，不报）。',
    '见 `m1/short.mjs:2`（前缀省略，唯一命中 delivery/workstreams/）。',
    '见 `progress.md:2`（中段省略，a/ 与 b/ 与 delivery/ 下各一份 → 多命中）。',
    '见 `docs/gone.md:5`（首段 docs 是实存一级目录 → 真断链）。',
    '见 `nope/deep.mjs:5`（首段 nope 非一级目录 → 短形态不中）。',
    '见 `…/PROPOSAL.md:126`（缩写形态，不是字面路径）。',
  ].join('\n');
  const cls = checkCitationAnchors(classifyText, repo);
  check('分类·prefixSource = git-index', cls.prefixSource, 'git-index');
  check('分类·problems 数（仅 docs/gone.md 真断链）', cls.problems.length, 1);
  check('分类·problems 条目', cls.problems[0] && cls.problems[0].citation, 'docs/gone.md:5');
  check('分类·resolvedByPrefix 数', cls.resolved.length, 1);
  check('分类·resolvedByPrefix 形态', cls.resolved[0] && cls.resolved[0].resolvedAs, 'delivery/workstreams/m1/short.mjs:2');
  check('分类·abbreviated 数', cls.abbreviated.length, 1);
  check('分类·ambiguousShortForm 数（progress.md 在 a/ b/ delivery/ 三处）', cls.ambiguousShortForm.length, 1);
  check('分类·ambiguousShortForm 命中数 ≥3', cls.ambiguousShortForm[0] && cls.ambiguousShortForm[0].hits.length, 3);
  check('分类·shortFormUnresolved 数', cls.shortFormUnresolved.length, 1);
  check('分类·字面直中不进任何分类列', cls.checked.length, 6);
  // gitignore 正控（T1 承重断言之一）：`build/` 被 .gitignore 覆盖 → 不得成为候选前缀。
  // 夹具形态须**排除「字面直中」这条捷径**——故不引 `build/...` 本身（那会直接命中工作树里的实存文件、
  // 绕开候选域），而引一个**只有 `build/` 成为前缀时才解析得到**的短形态 `sub/gen.mjs`。
  // 判据：若候选域把 build/ 收了进来 → 它会被 resolved；正确行为 = 落 shortFormUnresolved、resolved 为空。
  const ignored = checkCitationAnchors('见 `sub/gen.mjs:2`（真身在 gitignore 的 build/ 下，不得经候选域解析）。', repo);
  check('gitignore 正控·build/ 未成为候选前缀（resolved 为空）', ignored.resolved.length, 0);
  check('gitignore 正控·归类为 shortFormUnresolved（首段 sub 非一级目录）', ignored.shortFormUnresolved.length, 1);
  check('gitignore 正控·problems 为空', ignored.problems.length, 0);
  rmSync(repo, { recursive: true, force: true });

  // ── 例 3：非 git 回退分支（直接单测 staticCitationPrefixes，不依赖「git 恰好在某目录失败」这种脆断言）──
  const staticList = staticCitationPrefixes(process.cwd());
  check('回退分支·含四项常量之一 docs/', staticList.includes('docs/'), true);
  check('回退分支·含两树递归产物 .claude/hooks/', staticList.includes('.claude/hooks/'), true);
  check('回退分支·不含 delivery 面（回退域比 git 域窄，见 §2 T1 改法 2）', staticList.some((p) => p.startsWith('delivery/')), false);
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
  // 261026 批 T1：§2 切片跳围栏（围栏内以「## 」起头的行不是节边界；旧行为会提前截断、其后钉死块不被检）
  const vqLines = proposalText.split(/\r?\n/);
  const vqStart = vqLines.findIndex((l) => /^##\s*2[.、]/.test(l));
  if (vqStart === -1) return { checked, missing };
  const vqFlags = fenceFlags(vqLines);
  let vqEnd = vqLines.length;
  for (let i = vqStart + 1; i < vqLines.length; i++) {
    if (vqFlags[i]) continue;
    if (/^##\s/.test(vqLines[i])) { vqEnd = i; break; }
  }
  const secText = vqLines.slice(vqStart, vqEnd).join('\n');
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
      // 261026 批 T2（ARQ-261019-verbatim-quote-line-level-false-negative）：围栏块正文恒带块尾换行，
      // 子句替换形态里目标句常嵌在长行中部、句后仍有接续文——比对前对块 trim()（首尾空白不进判据，正文逐字仍在）。
      if (norm(content).includes(norm(block).trim())) { found = true; break; }
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
  // 261026 批 T2 夹具：子句嵌长行中部 + 块尾换行（判别力：反转实现不 trim 时 missing = 1 变红）
  const clauseDir = mkdtempSync(join(tmpdir(), 'vq-clause-'));
  writeFileSync(join(clauseDir, 'long.md'), '前缀甲：**。句后接续文、同一行内。\n', 'utf8');
  const clauseProposal = ['## 2. 改法', '### T1 · 子句', '```markdown', '前缀甲：**。', '```', '## 3. 影响面'].join('\n');
  const clauseRes = checkVerbatimQuotes(clauseProposal, clauseDir, ['long.md']);
  check('verbatim-clause-midline-trailing-newline-missing', clauseRes.missing.length, 0);
  check('verbatim-clause-checked', clauseRes.checked.length, 1);
  // 尾换行变体正控：块内容确实不在目标 → 仍报 missing（trim 不放宽判据）
  const clauseDrift = clauseProposal.split('前缀甲：**。').join('前缀甲：**。改');
  check('verbatim-clause-drift-still-missing', checkVerbatimQuotes(clauseDrift, clauseDir, ['long.md']).missing.length, 1);
  // 261026 批 T1 夹具：§2 切片跳围栏——围栏内含「## 」行后，其后钉死块仍须被检（反转实现截断 → checked 1 变红）
  const boundaryProposal = ['## 2. 改法', '### T1 · 甲', '```md', '## 围栏内节标题', '```', '', '### T2 · 乙', '```js', block, '```', '## 3. 影响面'].join('\n');
  check('verbatim-fence-boundary-checked', checkVerbatimQuotes(boundaryProposal, fixtureDir, ['target.mjs']).checked.length, 2);
  rmSync(clauseDir, { recursive: true, force: true });
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
    usage: 'node .claude/scripts/landing-fidelity-check.mjs delivery/review/gov-review-<id>',
  });
}
if (!existsSync(reviewDir)) fail('review directory does not exist', { reviewDir });

const proposalPath = resolve(reviewDir, 'PROPOSAL.md');
const summaryPath = resolve(reviewDir, 'architect-landing-summary.md');
const relDir = `delivery/review/${basename(reviewDir)}`;

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
// T2（260920 批）：六列——`problems`（真·断链 + 两条行号 / 可读性 reason）、`resolvedByPrefix`（唯一命中）、
// `abbreviated` / `ambiguousShortForm` / `shortFormUnresolved`（三类非断链成因）、`prefixSource`（候选域来源）。
// 字段名 `problems` / `resolvedByPrefix` 保持不变（既有 landing-summary / verdict 引用它们）。
const {
  checked: citationChecked,
  problems: citationProblems,
  resolved: citationResolved,
  abbreviated: citationAbbreviated,
  ambiguousShortForm: citationAmbiguous,
  shortFormUnresolved: citationShortFormUnresolved,
  prefixSource: citationPrefixSource,
} = checkCitationAnchors(proposalText, projectDir);
const citationAnchorAdvisory = {
  checked: citationChecked.length,
  problems: citationProblems,
  resolvedByPrefix: citationResolved,
  abbreviated: citationAbbreviated,
  ambiguousShortForm: citationAmbiguous,
  shortFormUnresolved: citationShortFormUnresolved,
  prefixSource: citationPrefixSource,
};

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
