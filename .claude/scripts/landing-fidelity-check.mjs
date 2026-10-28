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
//
// 目录锚兜底（261027 批 T3 · ARQ-260924 残余②，human 2026-10-27 裁「仍劝阻、展开作兜底」）：manifest 里以 /
// 结尾的目录 token 按前缀展开为成员判定（任一下辖文件有改动即视为该行已落），展开明细进输出 dirAnchorExpansions。
// 仍劝阻使用——逐件枚举是正形（展开只验「有成员改动」、逐件漏落不再被点名）；纯文件路径行为不变（changed.has 精确判定）。
//
// 引文自查子命令（261027 批 T1 · ARQ-260914 残余「锚文逐字性 byte 比对脚本化」）：
//   node .claude/scripts/landing-fidelity-check.mjs --cite-check <PROPOSAL.md> [--phase draft|landing]
// 只读自查、不参与主门判定、不进任何闸链：draft（缺省按 Status 自动判）= §2「原文」引文 vs 磁盘现文；
// landing = §2「改后 / 替换后 / 插入 / 追加」及未标注围栏块 vs 落地文件。exit 0 全命中 / 1 有未命中 / 2 用法或输入缺失。

import { existsSync, mkdirSync, readdirSync, readFileSync, mkdtempSync, writeFileSync, rmSync } from 'node:fs';
import { basename, relative, resolve, join } from 'node:path';
import { tmpdir } from 'node:os';
import { execSync, spawnSync, execFileSync } from 'node:child_process';
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
  LANDING_LANES,
  parseLandingLane,
  isEvidenceTailPath,
  readImplementationEvidence,
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
  if (idx >= 0) return { preflight: argv[idx + 1] || null, citeCheck: null, phase: null, positional: null, baseRef: null };
  // 引文自查入口（261027 批 T1 · ARQ-260914）：与 --preflight 同为只读子命令，选项不被位置参数消费。
  const ci = argv.indexOf('--cite-check');
  if (ci >= 0) {
    const pi = argv.indexOf('--phase');
    return { preflight: null, citeCheck: argv[ci + 1] || null, phase: pi >= 0 ? argv[pi + 1] || null : null, positional: null, baseRef: null };
  }
  return { preflight: null, citeCheck: null, phase: null, positional: argv[0] || null, baseRef: argv[1] || null };
}
const CLI = parseCliArgs(process.argv);
const preflightArg = CLI.preflight;
const citeCheckArg = CLI.citeCheck;
const citeCheckPhase = CLI.phase;
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

// T9①（260924-gov-review-gate-parse-teeth，261027 批 T3 改口径）：目录 token advisory——preflight 接受目录锚；
// landing 门自 261027 批起对目录 token 按前缀展开兜底（261027 批前 = 逐文件精确匹配 changed.has，260923 批
// `docs/crq/` FROZEN 后首跑结构门 RED 两条 → human 追认 + 机械展开 99 行即旧口径实录）。human 2026-10-27 裁：
// 目录锚**可用但劝阻、展开是兜底不是许可**——逐件枚举是正形（展开只验「有成员改动」，逐件漏落不再被点名）。
// 本 advisory 把口径与迁移批先例回起草期；**不入 findings、不动 ok/exit**。
function dirTokenAdvisories(union) {
  return [...(union || [])]
    .filter((t) => String(t).endsWith('/'))
    .map((t) => `锚并集含目录 token「${t}」：可用但劝阻——manifest 逐件枚举成员是正形；landing 门对目录 token 仅按前缀展开兜底（任一下辖文件有改动即视为该行已落，逐件漏落不再被点名；迁移批先例：260914 批 139 行全枚举）`);
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

function runAuthorLaneStructureSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, cond) => { if (cond) pass++; else { fail++; console.error(`FAIL [${label}]`); } };
  check('lane-default-packet-bound', parseLandingLane('> **状态：FROZEN v1.0**') === LANDING_LANES.PACKET_BOUND);
  check('lane-author-explicit', parseLandingLane('> **landing-lane**：`author-implementation`') === LANDING_LANES.AUTHOR_IMPLEMENTATION);
  let unknownBlocked = false;
  try { parseLandingLane('> **landing-lane**：`mystery`'); } catch (e) { unknownBlocked = e.code === 'INVALID_LANDING_LANE'; }
  check('lane-unknown-fail-loud', unknownBlocked);
  let conflictBlocked = false;
  try { parseLandingLane('> **landing-lane**：`packet-bound`\n> **landing-lane**：`author-implementation`'); } catch (e) { conflictBlocked = e.code === 'INVALID_LANDING_LANE'; }
  check('lane-conflict-fail-loud', conflictBlocked);
  check('evidence-tail-review-dir', isEvidenceTailPath('delivery/review/x/landing-verdict.md', 'x'));
  check('evidence-tail-index-state', isEvidenceTailPath('delivery/review/.changes-index-state.json', 'x'));
  check('evidence-tail-other-batch-blocked', !isEvidenceTailPath('delivery/review/y/landing-verdict.md', 'x'));
  check('evidence-tail-script-blocked', !isEvidenceTailPath('.claude/scripts/x.mjs', 'x'));
  const dir = mkdtempSync(join(tmpdir(), 'author-lane-structure-'));
  try {
    const id = 'x';
    mkdirSync(join(dir, 'delivery', 'review', id), { recursive: true });
    const sha = 'a'.repeat(40);
    writeFileSync(join(dir, 'delivery', 'review', id, 'implementation-review.json'), JSON.stringify({ schemaVersion: 1, lane: 'author-implementation', changeId: id, reviewedCommit: sha, reviewedTree: sha, changedFiles: ['.claude/scripts/x.mjs'] }));
    check('evidence-valid-read', readImplementationEvidence(dir, id, 'review').ok);
    writeFileSync(join(dir, 'delivery', 'review', id, 'implementation-verification.json'), '{bad');
    check('evidence-malformed-fail-loud', !readImplementationEvidence(dir, id, 'verification').ok);
    writeFileSync(join(dir, 'delivery', 'review', id, 'implementation-verification.json'), JSON.stringify({ schemaVersion: 1, lane: 'author-implementation', changeId: id, reviewedCommit: sha, reviewedTree: sha, verifiedCommit: 'bad', verifiedTree: sha, changedFiles: [] }));
    check('evidence-bad-sha-fail-loud', !readImplementationEvidence(dir, id, 'verification').ok);
  } finally { rmSync(dir, { recursive: true, force: true }); }
  console.log(JSON.stringify({ selfCheck: 'author-lane-structure', pass, fail, total: pass + fail, ok: fail === 0 }));
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

// ── 目标文件解析（261027 批 T1 · ARQ-260914）：manifest 路径 → 实际可读路径候选 ──
// 债务条目归位兜底：已结 debt 条目由 debt-render 移入 docs/debt/<域>/_archive/<年>/（位置 = status 的纯函数），
// 落地核验时 manifest 旧路径已不存在——按归档位补找一次；找不到仍按原路径返回（后续读失败自然判 missing）。
function targetCandidates(target, projectDir) {
  const exact = resolve(projectDir, target);
  if (existsSync(exact)) return [exact];
  const m = String(target).match(/^(docs\/debt\/[A-Za-z]+\/)([^/]+)$/);
  if (!m) return [exact];
  const archiveRoot = resolve(projectDir, `${m[1]}_archive`);
  let years = [];
  try { years = readdirSync(archiveRoot, { withFileTypes: true }).filter((e) => e.isDirectory()).map((e) => e.name); } catch { return [exact]; }
  const hits = years.map((y) => resolve(archiveRoot, y, m[2])).filter((p) => existsSync(p));
  return hits.length ? hits : [exact];
}

// ── 引文 ↔ 目标文件逐字对账单源（261027 批 T1）：既有 advisory 与本批 --cite-check 共用同一判据 ──
// T2（260907 批）：双侧行尾归一后比对——仓内 .mjs 工作树全量 CRLF（autocrlf 实况）、PROPOSAL 经 Write 落盘 LF，
// 逐字节 includes 必假（260906-gov-review-index-status-truth-source 批 2 条 advisory 假失配实证）。
// 261026 批 T2（ARQ-261019-verbatim-quote-line-level-false-negative）：围栏块正文恒带块尾换行，
// 子句替换形态里目标句常嵌在长行中部、句后仍有接续文——比对前对引文 trim()（首尾空白不进判据，正文逐字仍在）。
// 返回**实际命中的文件**（仓根相对路径，Windows 反斜杠归一为 /）——归档位兜底命中时读者能看见真实落点，未命中 = null。
function findQuoteInTargets(quote, targetFiles, projectDir) {
  const norm = (s) => s.replace(/\r\n/g, '\n');
  const needle = norm(quote).trim();
  if (!needle) return null;
  for (const target of targetFiles) {
    for (const abs of targetCandidates(target, projectDir)) {
      let content;
      try { content = readFileSync(abs, 'utf8'); } catch { continue; }
      if (norm(content).includes(needle)) return relative(projectDir, abs).split('\\').join('/');
    }
  }
  return null;
}

// ── §2 围栏块提取（261027 批 T2 · ARQ-261026-verbatim-extract-backtick-cut）──
// 旧实现 /```[a-zA-Z]*\n([\s\S]*?)```/g 把块内容中的 ``` 字面（js 夹具串形态，如 '/^\s*```/.test(l)'）
// 当块边界提前闭合——块被腰斩、其后散文被误当钉死块（261026 批 landing 观察列 24 条伪 missing 实证）。
// 新实现 = 逐行状态机：开行 = 行首可选空白 + ``` + 可选 info 串；闭行须**独立成行**（``` 后只允许空白）。
// 未闭合块不入列（围栏未闭合由 parseSection2AnchorLists fail-loud 点名，本检查不重复判）。
function extractFencedBlocks(lines) {
  const blocks = [];
  let open = null;
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    if (open) {
      if (/^\s*```\s*$/.test(line)) {
        blocks.push({ block: open.content.join('\n'), openLine: open.openLine, closeLine: i, info: open.info });
        open = null;
      } else {
        open.content.push(line);
      }
      continue;
    }
    const m = line.match(/^\s*```(.*)$/);
    if (m) open = { info: m[1].trim(), openLine: i, content: [] };
  }
  return blocks;
}

// ── §2 钉死引号块 ↔ 落地文件机械逐字对账（260826-gov-review-g3-infra-convergence T2，advisory-only）──
// 提取 PROPOSAL §2 各 T 节内围栏代码块正文（作者约定「原文钉死」最可机辨形态），逐块在 landing-manifest
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
  for (const { block } of extractFencedBlocks(vqLines.slice(vqStart, vqEnd))) {
    if (!block.trim()) continue;
    const hit = findQuoteInTargets(block, targetFiles, projectDir);
    checked.push(block);
    if (!hit) missing.push({ block: block.trim().slice(0, 120), reason: 'not-found-verbatim-in-any-target' });
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
  // 261027 批 T2 夹具（ARQ-261026-verbatim-extract-backtick-cut）：块内容含行内 ``` 字面（js 夹具串形态），
  // 块须完整提取（未腰斩）、逐字对账仍命中。判别力：反转实现（闭合不要求独立成行）时块被切在首个行内 ``` 处，
  // checked[0] 变成截断串「const re = /^\s*」→ 本组 block-intact 断言变红（该截断串仍是目标文件子串，missing 不会变红，
  // 故判别以「块完整」断言为准——这是本条 bug 的真实形态：伪 missing 与错切散文并存，而非命中翻转）。
  const tickDir = mkdtempSync(join(tmpdir(), 'vq-backtick-'));
  const tickBlock = "const re = /^\\s*```/.test(l);\nconst arr = ['```md', 'x'];";
  writeFileSync(join(tickDir, 'f.mjs'), `head\n${tickBlock}\ntail\n`, 'utf8');
  const tickProposal = ['## 2. 改法', '### T1 · 反引号夹具', '```js', tickBlock, '```', '## 3. 影响面'].join('\n');
  const tickRes = checkVerbatimQuotes(tickProposal, tickDir, ['f.mjs']);
  check('verbatim-backtick-in-content-checked', tickRes.checked.length, 1);
  check('verbatim-backtick-in-content-block-intact', tickRes.checked[0] === tickBlock, true);
  check('verbatim-backtick-in-content-missing', tickRes.missing.length, 0);
  rmSync(tickDir, { recursive: true, force: true });
  rmSync(clauseDir, { recursive: true, force: true });
  rmSync(crlfTarget, { recursive: true, force: true });
  rmSync(fixtureDir, { recursive: true, force: true });
  console.log(JSON.stringify({ selfCheck: 'verbatim-quotes', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// ── 引文自查子命令 --cite-check（261027 批 T1 · ARQ-260914 残余「锚文逐字性 byte 比对脚本化」）──
// 定位：把判官 R1 / 核验官每次现写的 node byte 比对固化为可复用件（载体 = 本脚本子命令，human 2026-10-27 立项 ack ①）。
// 覆盖形态（v1，判据交评审裁）：
//   - 围栏块：label 行（围栏开行之上最近的非空行）含「原文」= 原文引文（draft 期须在盘）；含「改后 / 替换后 /
//     插入 / 追加」= 改后文本（landing 期须在盘）；两族以 label 中**最后出现的 cue** 定性（「…原文…→ 替换后…」
//     单行混合式按后者走）。两相位各自把「不检」的块列进 notChecked——覆盖态如实申报，不静默略过。
//   - 行内引文：cue 之后最后一个冒号之后恰一个 code-span 且行尾无其它内容（如 原文（`:13`，逐字节）：`…`）——
//     draft 期查原文、landing 期查改后族。
//   - 目标解析走 targetCandidates：债务条目已归位 docs/debt/<域>/_archive/<年>/ 时按归档位补找（260924 批核验
//     教训：debt-render 归位后 manifest 旧路径失效，旧 advisory 在 P5 批报 3 条归档位预期 miss）。
// 不覆盖（输出 coverage / notChecked 如实申报）：blockquote 承载的插入段、多 span / 混合散文行内形态。
// 效力：只读自查，**不参与主门判定、不进任何闸链**；退出码自管（0 全命中 / 1 有未命中 / 2 用法或输入缺失）。
const CITE_ORIGINAL_CUE_RE = /原文/;
const CITE_RESULT_CUE_RE = /改后|替换后|插入|追加/;

// label 定性：取最后一个 cue（「替换前原文」只命中原文 → original；「…原文…→ 替换后…」→ result）。
function classifyCueLabel(label) {
  const hits = [...String(label).matchAll(/原文|改后|替换后|插入|追加/g)];
  if (!hits.length) return 'unlabeled';
  return hits[hits.length - 1][0] === '原文' ? 'original' : 'result';
}

// 围栏块的 label 行 = 围栏开行之上最近的非空行（索引相对传入的 §2 节域行数组）。
function labelLineForBlock(lines, openLine) {
  for (let i = openLine - 1; i >= 0; i--) {
    if (!lines[i].trim()) continue;
    return { index: i, text: lines[i].trim() };
  }
  return { index: -1, text: '' };
}

// 行内引文：cue 与候选用 code-span 遮罩后的文本定性（引文正文里的「原文」等字面不参与定性）——
// 候选 = 行内最后一个 code-span；要求 cue 之后、候选之前出现冒号（排除「原文（`grep -cF` = 1）：」这类
// 注记 span），且该区间内无第二个 cue 字面（排除「`a` → 替换后：`b`」混合式，交 unverified 不猜）。
function maskCodeSpans(label) {
  return String(label).replace(/`[^`\n]+`/g, (m) => ' '.repeat(m.length));
}

function inlineQuoteAfterCue(label, cueRe) {
  const masked = maskCodeSpans(label);
  const cue = cueRe.exec(masked);
  if (!cue) return null;
  const spans = [...label.matchAll(/`([^`\n]+)`/g)];
  if (!spans.length) return null;
  const last = spans[spans.length - 1];
  if (last.index <= cue.index + cue[0].length) return null;
  const between = masked.slice(cue.index + cue[0].length, last.index);
  if (!/[：:]/.test(between)) return null;
  if (/原文|改后|替换后|插入|追加/.test(between)) return null;
  if (!/^[\s，。；、）)】]*$/.test(masked.slice(last.index + last[0].length))) return null;
  return last[1];
}

// 从 PROPOSAL 收集本轮时点应检的引文（纯函数，供 --cite-check 与自检共用）。
// 返回 { quotes, notChecked, unverified }：quotes = 须逐个对账的引文；notChecked = 本相位不检的围栏块（含定性）；
// unverified = 定性歧义或行内形态无法无歧义提取的线索（不猜、不静默略过）。
function collectCiteQuotes(text, phase) {
  const lines = text.split(/\r?\n/);
  const start = lines.findIndex((l) => /^##\s*2[.、]/.test(l));
  if (start === -1) {
    return { quotes: [], notChecked: [], unverified: [{ line: 0, cue: '§2 改法节未找到', reason: 'no-section2' }] };
  }
  const flags = fenceFlags(lines);
  let end = lines.length;
  for (let i = start + 1; i < lines.length; i++) {
    if (flags[i]) continue;
    if (/^##\s/.test(lines[i])) { end = i; break; }
  }
  const secLines = lines.slice(start, end);
  const quotes = [];
  const notChecked = [];
  const unverified = [];
  const labelIdx = new Set();
  for (const b of extractFencedBlocks(secLines)) {
    const label = labelLineForBlock(secLines, b.openLine);
    if (label.index >= 0) labelIdx.add(label.index);
    if (!b.block.trim()) continue;
    const role = classifyCueLabel(label.text);
    const lineNo = start + b.openLine + 1;
    const cue = label.text.slice(0, 120);
    if (phase === 'draft') {
      if (role === 'original') quotes.push({ role, form: 'fence', line: lineNo, cue, text: b.block });
      else notChecked.push({ line: lineNo, role, form: 'fence', cue });
    } else if (role === 'original') {
      // landing 期：「原文」块落地后应已消失或被替换，不作判据（P5 批 3 条预期 miss 即此形态）。
      notChecked.push({ line: lineNo, role, form: 'fence', cue });
    } else {
      quotes.push({ role: role === 'unlabeled' ? 'result' : role, form: 'fence', line: lineNo, cue, text: b.block });
    }
  }
  // 行内引文：围栏外、且不是某围栏块 label 的行。draft 只查原文；landing 只查改后族。
  const cueRe = phase === 'draft' ? CITE_ORIGINAL_CUE_RE : CITE_RESULT_CUE_RE;
  for (let i = 0; i < secLines.length; i++) {
    if (flags[start + i] || labelIdx.has(i)) continue;
    const l = secLines[i];
    if (!cueRe.test(l)) continue;
    const q = inlineQuoteAfterCue(l, cueRe);
    if (q) {
      quotes.push({ role: phase === 'draft' ? 'original' : 'result', form: 'inline', line: start + i + 1, cue: l.trim().slice(0, 120), text: q });
    } else if (/原文[（(：:]|改后[（(：:]|替换后[（(：:]|插入[（(：:]|追加[（(：:]/.test(l) && l.includes('`')) {
      unverified.push({ line: start + i + 1, cue: l.trim().slice(0, 120), reason: 'inline-ambiguous' });
    }
  }
  return { quotes, notChecked, unverified };
}

function runCiteCheck(proposalPath, phaseArg) {
  const abs = resolve(projectDir, proposalPath);
  if (!existsSync(abs)) {
    print({ ok: false, gate: 'landing-cite-check', message: 'PROPOSAL 不存在', proposal: proposalPath });
    process.exit(2);
  }
  const text = readText(abs);
  const frozen = isFrozen(text);
  const phase = phaseArg || (frozen ? 'landing' : 'draft');
  if (phase !== 'draft' && phase !== 'landing') {
    print({ ok: false, gate: 'landing-cite-check', message: `--phase 仅接受 draft / landing（实得 ${phaseArg}）`, proposal: proposalPath });
    process.exit(2);
  }
  let targets;
  try {
    targets = extractTargets(text, { source: 'PROPOSAL.md' });
  } catch (error) {
    print({ ok: false, gate: 'landing-cite-check', message: error.message, proposal: proposalPath });
    process.exit(2);
  }
  const { quotes, notChecked, unverified } = collectCiteQuotes(text, phase);
  const checked = [];
  const missing = [];
  for (const q of quotes) {
    const hit = findQuoteInTargets(q.text, targets, projectDir);
    checked.push({ role: q.role, form: q.form, line: q.line, cue: q.cue, matched: hit });
    if (!hit) missing.push({ role: q.role, form: q.form, line: q.line, cue: q.cue, excerpt: q.text.trim().slice(0, 120) });
  }
  print({
    ok: missing.length === 0,
    gate: 'landing-cite-check',
    proposal: proposalPath,
    phase,
    frozen,
    targetCount: targets.length,
    checkedCount: checked.length,
    checked,
    missing,
    notChecked,
    unverified,
    coverage: {
      fenced: true,
      inline: true,
      blockquote: false,
      note: 'blockquote 承载的插入段与多 span / 混合散文行内形态不在 v1 覆盖面（见本批 PROPOSAL §6 留根）。',
    },
    note: '只读自查：引文 ↔ 目标文件逐字对账（双侧行尾归一 + trim）；不参与任何门判定、不进任何闸链。',
  });
  process.exit(missing.length === 0 ? 0 : 1);
}

// 引文自查自检（261027 批 T1）：覆盖 CLI 解析、draft / landing 两相位分列、行内形态、label 定性、归档位兜底。
// 判别力：反转 classifyCueLabel（原文 / 改后混淆）→ 两相位 quotes 角色断言变红；放宽行内判据 → 行内组变红；
// 去掉归档兜底 → 归档组变红。
function runCiteCheckSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'cite-check', label, got, want })); }
  };
  const c1 = parseCliArgs(['node', 'x', '--cite-check', 'p.md', '--phase', 'landing']);
  check('cli·citeCheck 取值', c1.citeCheck, 'p.md');
  check('cli·phase 取值', c1.phase, 'landing');
  check('cli·positional 为 null（不误触 reviewDir）', c1.positional, null);
  check('cli·baseRef 为 null', c1.baseRef, null);
  check('cli·phase 缺省为 null（按 Status 自动判）', parseCliArgs(['node', 'x', '--cite-check', 'p.md']).phase, null);
  const dir = mkdtempSync(join(tmpdir(), 'cite-'));
  const target = 'target.md';
  writeFileSync(join(dir, target), '首行\n原文甲：逐字在场。\n改后乙：落地在场。\n末行\n', 'utf8');
  const proposal = [
    '## 2. 改法（T1）',
    '### T1 · 甲',
    '原文（逐字节）：',
    '```text',
    '原文甲：逐字在场。',
    '```',
    '',
    '改后（逐字节）：',
    '```text',
    '改后乙：落地在场。',
    '```',
    '',
    '目标文件：`target.md`（改）',
    '## 3. 影响面',
  ].join('\n');
  const draftRes = collectCiteQuotes(proposal, 'draft');
  check('draft·只检原文块', JSON.stringify(draftRes.quotes.map((q) => q.role)), JSON.stringify(['original']));
  check('draft·改后块进 notChecked', draftRes.notChecked.length, 1);
  check('draft·原文命中', findQuoteInTargets(draftRes.quotes[0].text, [target], dir), target);
  const landingRes = collectCiteQuotes(proposal, 'landing');
  check('landing·只检改后块', JSON.stringify(landingRes.quotes.map((q) => q.role)), JSON.stringify(['result']));
  check('landing·原文块进 notChecked', landingRes.notChecked.length, 1);
  check('landing·改后命中', findQuoteInTargets(landingRes.quotes[0].text, [target], dir), target);
  const drift = proposal.replace('原文甲：逐字在场。', '原文甲：逐字在场！');
  check('draft·漂移未命中', findQuoteInTargets(collectCiteQuotes(drift, 'draft').quotes[0].text, [target], dir), null);
  const inlineProposal = ['## 2. 改法（T1）', '### T1 · 乙', '原文（`:13`，逐字节）：`原文甲：逐字在场。`', '目标文件：`target.md`（改）', '## 3. 影响面'].join('\n');
  const inlineRes = collectCiteQuotes(inlineProposal, 'draft');
  check('draft·行内原文入检', inlineRes.quotes.length === 1 && inlineRes.quotes[0].form === 'inline', true);
  check('draft·行内原文命中', findQuoteInTargets(inlineRes.quotes[0].text, [target], dir), target);
  const inlineMiss = inlineProposal.replace('逐字在场。', '逐字在场?');
  check('draft·行内漂移未命中', collectCiteQuotes(inlineMiss, 'draft').quotes.length === 1 && findQuoteInTargets(collectCiteQuotes(inlineMiss, 'draft').quotes[0].text, [target], dir) === null, true);
  const mixed = ['## 2. 改法（T1）', '### T1 · 丙', '原文（逐字）：`甲` → 替换后（逐字）：`乙`', '目标文件：`target.md`（改）', '## 3. 影响面'].join('\n');
  check('draft·混合行进 unverified（不猜）', collectCiteQuotes(mixed, 'draft').unverified.length, 1);
  check('label·替换前原文仍判原文', classifyCueLabel('替换前原文（逐字）：'), 'original');
  check('label·混合行取最后 cue', classifyCueLabel('原文（逐字）：`a` → 替换后（逐字）：`b`'), 'result');
  check('label·无 cue 判未标注', classifyCueLabel('全文逐字节如下：'), 'unlabeled');
  // 归档位兜底：目标不在 manifest 旧路径、在 docs/debt/gov/_archive/2026/ —— 命中；非债务路径不回退。
  mkdirSync(join(dir, 'docs', 'debt', 'gov', '_archive', '2026'), { recursive: true });
  writeFileSync(join(dir, 'docs', 'debt', 'gov', '_archive', '2026', 'ARQ-x.md'), '归档原文行。\n', 'utf8');
  check('归档兜底·命中', findQuoteInTargets('归档原文行。', ['docs/debt/gov/ARQ-x.md'], dir), 'docs/debt/gov/_archive/2026/ARQ-x.md');
  check('归档兜底·非债务路径不回退', findQuoteInTargets('归档原文行。', ['docs/other/ARQ-x.md'], dir), null);
  rmSync(dir, { recursive: true, force: true });
  console.log(JSON.stringify({ selfCheck: 'cite-check', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// 目录锚前缀展开兜底（261027 批 T3 · ARQ-260924 残余②，human 2026-10-27 裁「仍劝阻、展开作兜底」）：
// manifest 目录 token（以 / 结尾）不再按 changed.has 精确匹配判假 RED——展开为其下成员判定（任一成员有改动
// 即视为该行已落），展开明细进 dirAnchorExpansions；逐件枚举仍是正形（展开放宽「静默漏落」检测，只作兜底）。
// 纯文件 token 行为不变（changed.has 精确成员判定，与改前逐字节同）。
function computeNotLanded(targets, changed) {
  const notLanded = [];
  const dirExpansions = [];
  for (const t of targets) {
    if (String(t).endsWith('/')) {
      const matched = [...changed].filter((f) => f.startsWith(t)).length;
      if (matched === 0) notLanded.push(t);
      else dirExpansions.push({ token: t, matched });
      continue;
    }
    if (!changed.has(t)) notLanded.push(t);
  }
  return { notLanded, dirExpansions };
}

// 目录锚自检（261027 批 T3）：两向判别——目录锚样例展开后不假 RED（改前形态 = changed.has 精确匹配必判未落）；
// 纯文件路径行为与改前一致（命中不报 / 未落照报 / 零展开明细）。尾 / 是目录锚唯一形态判据（无 / 走文件 token）。
function runDirAnchorExpansionSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'dir-anchor-expansion', label, got, want })); }
  };
  const changed = new Set(['docs/crq/a.md', 'docs/crq/sub/b.md', '.claude/scripts/x.mjs']);
  const dirHit = computeNotLanded(['docs/crq/'], changed);
  check('目录锚·有成员改动不判未落（改前假 RED 形态）', JSON.stringify(dirHit.notLanded), JSON.stringify([]));
  check('目录锚·展开明细', JSON.stringify(dirHit.dirExpansions), JSON.stringify([{ token: 'docs/crq/', matched: 2 }]));
  const dirMiss = computeNotLanded(['docs/none/'], changed);
  check('目录锚·零成员改动仍判未落', JSON.stringify(dirMiss.notLanded), JSON.stringify(['docs/none/']));
  check('目录锚·零命中无展开明细', dirMiss.dirExpansions.length, 0);
  const files = computeNotLanded(['.claude/scripts/x.mjs', 'docs/crq/a.md', '.claude/scripts/y.mjs'], changed);
  check('纯文件路径·命中不报 / 未落照报（行为不变）', JSON.stringify(files.notLanded), JSON.stringify(['.claude/scripts/y.mjs']));
  check('纯文件路径·无展开明细', files.dirExpansions.length, 0);
  const noSlash = computeNotLanded(['docs/crq'], changed);
  check('尾斜杠唯一判据·无尾 / 按文件 token 处理', JSON.stringify(noSlash.notLanded), JSON.stringify(['docs/crq']));
  console.log(JSON.stringify({ selfCheck: 'dir-anchor-expansion', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// 自检接线顺序审查（真空绿规约）：runIsFrozenSelfCheck 为终端式（无条件 process.exit），必须列最后；
// 其余非终端式自检（含 B7 新增 section2Anchors、T2 新增 verbatim-quotes、260918 批 landing-preflight、
// 261027 批 cite-check / dir-anchor-expansion）排在其前，否则永不执行。
if (process.argv.includes('--self-check')) { runExtractTargetsSelfCheck(); runSection2AnchorSelfCheck(); runResolveOriginSelfCheck(); runComputeFrozenPinSelfCheck(); runCitationAnchorsSelfCheck(); runPreflightSelfCheck(); runAuthorLaneStructureSelfCheck(); runCiteCheckSelfCheck(); runLintDescriptionsTriggerSelfCheck(); runVerbatimQuotesSelfCheck(); runDirAnchorExpansionSelfCheck(); runIsFrozenSelfCheck(); }

// 干跑入口（T2，260918 批）：只读、不对账、不参与任何门判定、不进任何闸链。
// 置于 --self-check 之后（两者同给时先跑自检并以其退出码为准）。
if (process.argv.indexOf('--preflight') >= 0) {
  if (!preflightArg) {
    print({ ok: false, gate: 'landing-preflight', message: '--preflight 需要一个 PROPOSAL 路径参数' });
    process.exit(1);
  }
  runPreflight(preflightArg);
}
// 引文自查入口（261027 批 T1 · ARQ-260914）：只读、不参与主门判定；与 --preflight 同置于主流程之前。
if (process.argv.indexOf('--cite-check') >= 0) {
  if (!citeCheckArg) {
    print({ ok: false, gate: 'landing-cite-check', message: '--cite-check 需要一个 PROPOSAL 路径参数' });
    process.exit(2);
  }
  runCiteCheck(citeCheckArg, citeCheckPhase);
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

// 作者实现 lane：不查未来文本，直接核 reviewer 钉点、Git tree、manifest 与工作树漂移。
let landingLane = LANDING_LANES.PACKET_BOUND;
try { landingLane = parseLandingLane(proposalText); } catch (error) { errors.push(error.message); }
if (landingLane === LANDING_LANES.AUTHOR_IMPLEMENTATION) {
  const changeId = basename(reviewDir);
  if ([...proposalTargets].some((p) => !/^\.claude\/(?:scripts|hooks)\//.test(p))) {
    errors.push('author-implementation manifest 必须全部位于 .claude/scripts/** 或 .claude/hooks/**');
  }
  const review = readImplementationEvidence(projectDir, changeId, 'review');
  if (!review.ok) errors.push(review.reason);
  else {
    const pin = review.evidence;
    let actualTree = null;
    try { actualTree = execFileSync('git', ['rev-parse', `${pin.reviewedCommit}^{tree}`], { cwd: projectDir, encoding: 'utf8', stdio: 'pipe' }).trim(); } catch { /* fail-closed below */ }
    if (!actualTree || actualTree !== pin.reviewedTree) errors.push('reviewedTree 与 reviewedCommit 的 Git tree 不符或 commit 不存在');
    const diff = runGit(`git diff --name-only main...${pin.reviewedCommit}`);
    if (diff === null) errors.push('main...reviewedCommit 重算失败（fail-closed）');
    else {
      const actual = new Set(diff.split(/\r?\n/).map((x) => x.trim()).filter(Boolean).filter((p) => !isEvidenceTailPath(p, changeId)));
      const expected = new Set(proposalTargets);
      const same = actual.size === expected.size && [...actual].every((p) => expected.has(p));
      if (!same) errors.push(`main...reviewedCommit 非证据改动 != manifest：${[...actual].sort().join(', ')}`);
    }
    if (!summaryText.includes(pin.reviewedCommit) || !summaryText.includes(pin.reviewedTree)) errors.push('summary 未复述 reviewedCommit/reviewedTree（交叉对账失败）');
  }
  const status = runGit('git status --porcelain -uall');
  if (status === null) errors.push('工作树实现漂移探测失败（fail-closed）');
  else {
    const drift = status.split(/\r?\n/).filter(Boolean).map((line) => line.slice(3).replace(/^"|"$/g, '').split(' -> ').pop())
      .filter((p) => /^\.claude\/(?:scripts|hooks)\//.test(p));
    if (drift.length) errors.push(`工作树含未提交 scripts/hooks 实现漂移：${drift.join(', ')}`);
  }
  if (errors.length) fail('author-implementation landing 保真度核验未过', { reviewDir: relDir, lane: landingLane, errors });
  const citation = checkCitationAnchors(proposalText, projectDir);
  print({
    ok: true,
    gate: 'landing-fidelity',
    reviewDir: relDir,
    lane: landingLane,
    frozen: true,
    packetBinding: true,
    proposalTargets: [...proposalTargets],
    summaryTargets: [...summaryTargets],
    reviewedCommit: review.evidence.reviewedCommit,
    reviewedTree: review.evidence.reviewedTree,
    citationAnchorAdvisory: { checked: citation.checked.length, problems: citation.problems, prefixSource: citation.prefixSource },
    verbatimQuoteAdvisory: { applicable: false, reason: 'author-implementation 不消费未来文本逐字块' },
  });
  process.exit(0);
}

// git 改动集（工作树 ∪ 已提交；见 gitChangedFiles）。
const { files: changed, degraded, reason } = gitChangedFiles(baseRef);

// 5. summary 声称落的 → git 里有改动（目录 token 按前缀展开兜底，261027 批 T3；纯文件路径行为不变）。
const summaryLanded = computeNotLanded([...summaryTargets], changed);
// 6. PROPOSAL 清单 → git 里有改动（无静默漏落）。
const proposalLanded = computeNotLanded([...proposalTargets], changed);
const claimedNotLanded = summaryLanded.notLanded;
const plannedNotLanded = proposalLanded.notLanded;
const dirAnchorExpansions = [...new Map([...summaryLanded.dirExpansions, ...proposalLanded.dirExpansions].map((x) => [x.token, x])).values()];

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
  fail('governance landing 保真度核验未过', { reviewDir: relDir, errors, gitDegraded: degraded, dirAnchorExpansions, hint });
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
  dirAnchorExpansions,
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
