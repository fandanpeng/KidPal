#!/usr/bin/env node
// changes/INDEX.md 幂等全量重建 + 单行 upsert（G3，对偶 .claude/scripts/planning-index.mjs）。
// 扫描 changes/dev/*/ + changes/arch/*/，收 origin.ref/origin.workstream/changeId/日期/状态，重建 changes/INDEX.md。
// 幂等——每次同输入产同输出；INDEX 头标「勿手改」。
//
// 用法：
//   node .claude/scripts/changes-index.mjs                    # 幂等全量重建（定期纠偏/手改后修复）
//   node .claude/scripts/changes-index.mjs --upsert <id>      # 单行 upsert（状态转移时 agent 主动跑，只动一行）
//   node .claude/scripts/changes-index.mjs --backfill         # 一次性回填（日期/已合main/landed，零 git fork 读缓存）
//   node .claude/scripts/changes-index.mjs --self-check       # 结构化自检（ARQ-114 先例）
// 由 closeout-check 增量刷 dev 行（互补——全量重建是手改/漂移的纠偏手段）。
// 本脚本不写 index.jsonl。
//
// arch 状态机（260823-gov-review-arq-queue-restructure，Track B R2 GREEN）：
//   draft → frozen → landed（终态）。frozen 复用 lib isFrozen（状态行非全文）；
//   landed = landing-verdict.md 在场且最终 verdict ∈ {PASS, GREEN}；已合 main = git branch --contains <首提交> 含本地 main。
//   状态全 immutable/单调收敛 → 一次算完钉死；已算结果缓存进 .changes-index-state.json 供零 fork 复用。

import { existsSync, readFileSync, readdirSync, writeFileSync, mkdirSync, rmSync } from 'node:fs';
import { resolve, join, basename } from 'node:path';
import { execSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { resolveOriginRef, isFrozen } from './lib/governance-gate-shared.mjs';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// 状态缓存（一次性钉死结果），供零 fork 复用。tracked，幂等。
const STATE_CACHE_PATH = resolve(projectDir, 'changes', '.changes-index-state.json');

// 人工 adjudication（landing 期一次性判定，immutable）：机械强信号判不出的发散旧批 + 进行中批。
// 每个值 = true（人工核 verdict 文件确认已落地）。判据记录于 changes/arch/260823-gov-review-arq-queue-restructure/（landing 期 verdict 人工判定清单）。
const LANDED_ADJUDICATION = {
  // 发散散文旧批（强信号判不出，人工核 verdict 结论为通过态）：
  '260729-gov-review-portfolio-handoff-failclosed': true,      // 转绿记录 + R-1~R-4 全绿 → 翻 GREEN（终态）
  '260730-gov-review-landing-report-ground-truth': true,
  '260730-gov-review-verdict-bearing-dual': true,             // 总裁 PASS，mustFixBeforeMerge=[]
  '260731-gov-review-retro-waiting-writeoff': true,           // RED 退回项：无 → 通过
  '260801-gov-review-stopbleeding-lite': true,
  'gov-review-arq-id-date-summary': true,
  'gov-review-closeout-authority-readonly-consumption': true, // ## Verdict: PASS（英文标题行）
  'gov-review-discussion-draft-authorship': true,
  'gov-review-supervisor-provenance-strength': true,
  // landing 期补判（强信号仍判不出的 3 个，人工核 verdict 结论为通过态）：
  '260731-gov-review-deny-main-push-hardening': true,         // 文末「故五复验 VERDICT = PASS」（五复验叙事行）
  '260813-gov-review-gate-accuracy-batch': true,              // 文末「复验结论：总判由 FAIL 转 PASS」（翻判）
  'gov-review-governance-doc-restructure': true,              // 「## 结论\n**GREEN。**」（token 在标题下一加粗行）
};

function loadStateCache() {
  try { return JSON.parse(readText(STATE_CACHE_PATH)); } catch { return {}; }
}
function saveStateCache(cache) {
  writeFileSync(STATE_CACHE_PATH, JSON.stringify(cache, null, 2) + '\n', 'utf8');
}

function readText(path) {
  return readFileSync(path, 'utf8').replace(/^﻿/, '');
}

function readJson(path) {
  return JSON.parse(readText(path));
}

function gitFirstCommitDate(relPath) {
  const sha = gitFirstCommitSha(relPath);
  return sha ? gitCommitDate(sha) : '';
}

function gitFirstCommitSha(relPath) {
  try {
    const out = execSync(`git log --diff-filter=A --follow --format=%H -- "${relPath}"`, {
      cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
    });
    const lines = out.split(/\r?\n/).map(l => l.trim()).filter(Boolean);
    return lines.length ? lines[lines.length - 1] : '';
  } catch {
    return '';
  }
}

function gitCommitDate(sha) {
  try {
    return execSync(`git show -s --format=%cs ${sha}`, { cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim();
  } catch {
    return '';
  }
}

// 已合 main（immutable）：目录首提交被本地 main 包含。前提 = 合 main 动作完成后跑一次（R1-F3）；本地 main 假定单调不回退。
function gitContainedInMain(sha) {
  if (!sha) return '';
  try {
    const out = execSync(`git branch --contains ${sha} --format=%(refname:short)`, {
      cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
    });
    const branches = out.split(/\r?\n/).map(l => l.trim());
    return branches.includes('main') ? 'yes' : 'no';
  } catch {
    return '';
  }
}

function parseChangeIdDate(changeId) {
  const m = /^(\d{6})-/.exec(String(changeId));
  if (!m) return '';
  const yy = m[1].slice(0, 2), mm = m[1].slice(2, 4), dd = m[1].slice(4, 6);
  if (mm < '01' || mm > '12' || dd < '01' || dd > '31') return '';
  return `20${yy}-${mm}-${dd}`;
}

function esc(s) {
  return String(s == null ? '' : s).replace(/\|/g, '\\|');
}

// 收 dev dirs（changes/dev/*/ 子目录，changeId 形状，可能含 closeout 制品）。
function collectDev() {
  const root = resolve(projectDir, 'changes', 'dev');
  const items = [];
  if (!existsSync(root)) return items;
  for (const name of readdirSync(root, { withFileTypes: true })) {
    if (!name.isDirectory()) continue;
    const dir = join(root, name.name);
    const tp = join(dir, 'taskpackage.json');
    const tpFallback = resolve(projectDir, '.claude', 'taskpackages', name.name, 'taskpackage.json');
    let originRef = '', originWorkstream = '';
    const tpPath = existsSync(tp) ? tp : (existsSync(tpFallback) ? tpFallback : null);
    if (tpPath) {
      try {
        const o = readJson(tpPath).origin || {};
        originRef = resolveOriginRef(o) || '';
        originWorkstream = o.workstream || '';
      } catch { /* 非 JSON 或缺 origin */ }
    }
    const date = parseChangeIdDate(name.name) || gitFirstCommitDate(`changes/dev/${name.name}`);
// R5 (gov-review-devloop-evidence-led, FROZEN 2026-07-24):
//   - C4 retired final-summary.md → status determination switched to verification.json

    const hasVerdict = existsSync(join(dir, 'verification.json')) || existsSync(join(dir, 'verdict.json'));
    const status = hasVerdict ? 'closed' : 'open';
    items.push({ changeId: name.name, date, lane: 'dev', originRef, status, workstream: originWorkstream });
  }
  return items;
}

// landed 判定（终态）：landing-verdict.md 在场且最终结论为通过态（闭集 {PASS, GREEN}）。
// 三层兜底，按可靠度排序：
//   ① 人工 adjudication 缓存（cache[id].landed===true）——发散旧批一次性人工钉死，最优先（immutable，判一次永不再判）；
//   ② 机械强信号——「状态行」（verdict/总判/结论 同行跟 token，兼容全角「：」）或「标题行 + 随后加粗 token」；
//   ③ 都不命中 → 非 landed（fail-closed：宁漏标不错标，漏标由人工 adjudication 兜底）。
// 新批强制规范 verdict 行（`> verdict: PASS/GREEN`）→ 100% 走 ②。旧批发散散文（「转绿记录」「验收通过」等叙事）不强行机械解析（太脆），归 ①。
const VERDICT_STATUS_RE = /^\s*(?:>+\s*)?(?:\*{0,2})\s*(?:verdict|总判|总判定|结论|复验结论|判定)\s*(?:\*{0,2})\s*[:：=]\s*\*{0,2}\s*(PASS|GREEN|RED|FAIL|BLOCK)\b/i;
const VERDICT_HEADING_RE = /^#{1,4}\s*(?:verdict|总判|总判定|结论|复验结论)\s*[:：]?\s*\*{0,2}\s*(PASS|GREEN|RED|FAIL|BLOCK)?\b/i;
function deriveLanded(dir, cached) {
  if ((cached && cached.landed === true) || LANDED_ADJUDICATION[basename(dir)] === true) return true; // ① 人工 adjudication 钉死
  const vp = join(dir, 'landing-verdict.md');
  if (!existsSync(vp)) return false;
  try {
    const lines = readText(vp).split('\n');
    let finalToken = '';
    let pendingHeading = false;
    for (const line of lines) {
      const s = VERDICT_STATUS_RE.exec(line);
      if (s) { finalToken = s[1].toUpperCase(); pendingHeading = false; continue; }
      const h = VERDICT_HEADING_RE.exec(line);
      if (h) {
        if (h[1]) { finalToken = h[1].toUpperCase(); pendingHeading = false; }
        else pendingHeading = true;
        continue;
      }
      if (pendingHeading) {
        const b = /^\s*\*{0,2}\s*(PASS|GREEN|RED|FAIL|BLOCK)\b/.exec(line);
        if (b) { finalToken = b[1].toUpperCase(); pendingHeading = false; }
      }
    }
    return finalToken === 'PASS' || finalToken === 'GREEN';
  } catch {
    return false;
  }
}

// ── 陈旧目录判定（260826-gov-review-arch-dir-gc T1）：census 观察面，只读、永不删除 ──
// 判据三件：landed（复用 deriveLanded 单源）真 → 非陈旧；firstCommit 空（git 不可用/无历史）→ 非陈旧（降级放行）；
// 否则首提交距 now 超 thresholdDays（默认 30，ARCH_GC_STALE_DAYS 可配）→ 陈旧候选。严格「超」（>），恰阈值之日不算陈旧。
function daysBetweenIso(fromIso, toIso) {
  const a = new Date(`${fromIso}T00:00:00Z`);
  const b = new Date(`${toIso}T00:00:00Z`);
  if (Number.isNaN(a.getTime()) || Number.isNaN(b.getTime())) return NaN;
  return (b.getTime() - a.getTime()) / 86400000;
}

function staleCandidate({ landed, firstCommit, thresholdDays, now }) {
  if (landed) return false;
  if (!firstCommit) return false;
  const d = daysBetweenIso(firstCommit, now);
  if (Number.isNaN(d)) return false;
  return d > (thresholdDays ?? 30);
}

// 单目录 arch 状态计算（供 collectArch 全量与 --upsert 单行复用）。cache 提供已钉死的 date/mergedInMain（零 fork）；frozen/landed 从文件现推（廉价、无 fork）。
function computeArchEntry(name, cache) {
  const dir = join(resolve(projectDir, 'changes', 'arch'), name);
  const propPath = join(dir, 'PROPOSAL.md');
  const cached = cache[name] || {};
  let status = 'draft';
  if (existsSync(propPath)) {
    try { if (isFrozen(readText(propPath))) status = 'frozen'; } catch { /* ignore */ }
  }
  if (deriveLanded(dir, cached)) status = 'landed'; // landed 是终态，覆盖 frozen（① 人工 adjudication 缓存优先）
  const date = cached.date || parseChangeIdDate(name) || gitFirstCommitDate(`changes/arch/${name}`);
  const mergedInMain = cached.mergedInMain !== undefined ? cached.mergedInMain : '';
  return { changeId: name, date, lane: 'arch', originRef: '', status, workstream: '', mergedInMain };
}

// 收 arch dirs（changes/arch/*/ 子目录，review 文件夹，读 PROPOSAL.md 状态行 + landing-verdict.md 取状态）。
function collectArch(cache = {}) {
  const root = resolve(projectDir, 'changes', 'arch');
  const items = [];
  if (!existsSync(root)) return items;
  for (const name of readdirSync(root, { withFileTypes: true })) {
    if (!name.isDirectory()) continue;
    items.push(computeArchEntry(name.name, cache));
  }
  return items;
}

// ── stale-report：机械 census（只读、永不删除、永不改写）──
// 遍历 changes/arch/*/，deriveLanded（复用单源 + cache）+ gitFirstCommitDate → staleCandidate 过滤，
// stdout 打 JSON；无陈旧时 stale:[]。fileCount = 目录直接子文件计数（非递归），纯观察列、不参与判定。
function todayLocalIso() {
  const d = new Date();
  const y = d.getFullYear();
  const m = String(d.getMonth() + 1).padStart(2, '0');
  const dd = String(d.getDate()).padStart(2, '0');
  return `${y}-${m}-${dd}`;
}

function staleReport() {
  const thresholdDays = parseInt(process.env.ARCH_GC_STALE_DAYS || '30', 10) || 30;
  const now = todayLocalIso();
  const root = resolve(projectDir, 'changes', 'arch');
  const stale = [];
  if (!existsSync(root)) return { ok: true, mode: 'stale-report', thresholdDays, stale };
  const cache = loadStateCache();
  for (const name of readdirSync(root, { withFileTypes: true })) {
    if (!name.isDirectory()) continue;
    const dir = join(root, name.name);
    let fileCount = 0;
    try {
      fileCount = readdirSync(dir, { withFileTypes: true }).filter(e => e.isFile()).length;
    } catch { fileCount = 0; }
    const landed = deriveLanded(dir, cache[name.name]);
    const firstCommit = gitFirstCommitDate(`changes/arch/${name.name}`);
    if (staleCandidate({ landed, firstCommit, thresholdDays, now })) {
      stale.push({ dir: name.name, firstCommit, landed, fileCount });
    }
  }
  return { ok: true, mode: 'stale-report', thresholdDays, stale };
}

function buildIndexMd(items) {
  const dev = items.filter(i => i.lane === 'dev').sort((a, b) => String(b.date || '').localeCompare(String(a.date || '')));
  const arch = items.filter(i => i.lane === 'arch').sort((a, b) => String(b.date || '').localeCompare(String(a.date || '')));
  const out = [];
  out.push('<!-- AUTO-GENERATED by .claude/scripts/changes-index.mjs — 勿手改（投影，源 = 各包 origin / 目录）。');
  out.push('手改会被全量重建覆盖。INDEX 不是第二事实源，不做拦阻依据。-->');
  out.push('');
  out.push('# changes Index');
  out.push('');
  out.push('> 自动生成（changes-index.mjs 幂等重建 / closeout-check dev 段增量刷行 / 状态转移时 `--upsert <id>` 单行钉进）。**勿手改**。');
  out.push('> 列：changeId | 日期 | dev/arch | origin.ref | 状态 | workstream；arch 段另含「已合main」。arch 状态机：draft→frozen→landed(终态)。');
  out.push('');
  out.push(`## dev (${dev.length})`);
  out.push('');
  if (dev.length) {
    out.push('| changeId | 日期 | origin.ref | 状态 | workstream |');
    out.push('|---|---|---|---|---|');
    for (const i of dev) {
      out.push(`| [${esc(i.changeId)}](../changes/dev/${i.changeId}/) | ${i.date || '—'} | ${esc(i.originRef) || '—'} | ${esc(i.status)} | ${esc(i.workstream) || '—'} |`);
    }
  } else {
    out.push('_（无）_');
  }
  out.push('');
  out.push(`## arch (${arch.length})`);
  out.push('');
  if (arch.length) {
    out.push('| changeId | 日期 | 状态 | 已合main |');
    out.push('|---|---|---|---|');
    for (const i of arch) {
      out.push(`| [${esc(i.changeId)}](../changes/arch/${i.changeId}/) | ${i.date || '—'} | ${esc(i.status)} | ${esc(i.mergedInMain) || '—'} |`);
    }
  } else {
    out.push('_（无）_');
  }
  out.push('');
  return out.join('\n');
}

// ── 命名断言（260825-gov-review-track-b-naming-convention T3）：日期前缀 arch 目录名须命中写槽 token ──
// 只拦日期前缀新目录（parseChangeIdDate 判定；非法日期前缀自然落空不断言）；旧目录 / 非日期前缀宽容。
// fail-loud 点名只挂 upsert（起草期收口）；全量重建与 backfill 不经本断言（存量安全）。
function checkArchDirNaming(id) {
  if (!parseChangeIdDate(id)) return null; // 非日期前缀或非法日期前缀 → 不断言
  if (/gov-review|arch-review/.test(id)) return null;
  return `naming: changes/arch/${id} 缺写槽 token（gov-review / arch-review）——日期前缀提案目录名必须命中 .claude/scope-map.json review-agent 写槽，否则评审/核验期制品写入被 enforce-scope 拦截（见 protocol「双轨路由」节命名条款）`;
}

// ── 单行 upsert：在 INDEX 中定位 <id> 的 arch 行 → 替换或插入（只动一行，对偶 closeout-check:163-177 dev 段先例）──
function upsertArchRow(indexPath, id, cache) {
  const namingError = checkArchDirNaming(id);
  if (namingError) return { ok: false, error: namingError };
  if (!existsSync(join(resolve(projectDir, 'changes', 'arch'), id))) {
    return { ok: false, error: `arch dir changes/arch/${id} absent on disk` };
  }
  if (!existsSync(indexPath)) return { ok: false, error: 'changes/INDEX.md absent — run full rebuild first' };
  const entry = computeArchEntry(id, cache);
  // 已合 main 是 immutable：现推并钉进缓存（前提 = 合 main 完成后跑本 upsert，R1-F3）
  const sha = gitFirstCommitSha(`changes/arch/${id}`);
  const merged = gitContainedInMain(sha);
  if (merged) { entry.mergedInMain = merged; cache[id] = { ...(cache[id] || {}), date: entry.date, mergedInMain: merged }; }
  const newRow = `| [${esc(entry.changeId)}](../changes/arch/${entry.changeId}/) | ${entry.date || '—'} | ${esc(entry.status)} | ${esc(entry.mergedInMain) || '—'} |`;

  const content = readText(indexPath);
  const lines = content.split('\n');
  const archStart = lines.findIndex(l => /^## arch /.test(l));
  if (archStart < 0) return { ok: false, error: 'INDEX header non-standard (no "## arch" section)' };
  const linkRe = new RegExp(`^\\| \\[${id.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\]\\(\\.\\./changes/arch/`);
  const idx = lines.findIndex((l, i) => i > archStart && linkRe.test(l));
  let action;
  if (idx >= 0) {
    if (lines[idx] === newRow) return { ok: true, action: 'unchanged', entry };
    lines[idx] = newRow; action = 'replaced';
  } else {
    // 插入：arch 段按日期降序，找首个 date <= 本行日期的数据行之前
    let insertAt = -1;
    for (let i = archStart + 1; i < lines.length; i++) {
      if (!lines[i].startsWith('| [')) continue;
      const m = /^\| \[[^\]]+\]\([^)]*\) \| (\d{4}-\d{2}-\d{2}) /.exec(lines[i]);
      const rowDate = m ? m[1] : '';
      if (rowDate <= (entry.date || '')) { insertAt = i; break; }
    }
    if (insertAt < 0) { // 段尾（或全段无更早日期的行）
      insertAt = lines.length;
      for (let i = archStart + 1; i < lines.length; i++) if (lines[i].startsWith('| [')) insertAt = i + 1;
    }
    lines.splice(insertAt, 0, newRow); action = 'inserted';
  }
  writeFileSync(indexPath, lines.join('\n'), 'utf8');
  return { ok: true, action, entry };
}

// ── 一次性回填：对全部 arch 目录算 date/mergedInMain（fork git 一次）+ landed 状态，写缓存 + 全量重建 ──
function backfill() {
  const cache = loadStateCache();
  const root = resolve(projectDir, 'changes', 'arch');
  const names = existsSync(root) ? readdirSync(root, { withFileTypes: true }).filter(d => d.isDirectory()).map(d => d.name) : [];
  let computed = 0, landedCount = 0;
  for (const name of names) {
    const sha = gitFirstCommitSha(`changes/arch/${name}`);
    const date = parseChangeIdDate(name) || (sha ? gitCommitDate(sha) : '');
    const merged = gitContainedInMain(sha);
    const landed = deriveLanded(join(root, name), cache[name]);
    if (landed) landedCount++;
    cache[name] = { date, mergedInMain: merged, landed };
    computed++;
  }
  saveStateCache(cache);
  const items = [...collectDev(), ...collectArch(cache)];
  writeFileSync(resolve(projectDir, 'changes', 'INDEX.md'), buildIndexMd(items), 'utf8');
  return { ok: true, backfilled: computed, archCount: items.filter(i => i.lane === 'arch').length, landed: landedCount };
}

// ── 结构化自检（ARQ-114 先例）──
function selfCheck() {
  const cases = [];
  const t = (name, actual, expected) => cases.push({ name, pass: JSON.stringify(actual) === JSON.stringify(expected), actual, expected });
  // frozen 状态行判定（复用 isFrozen 单源）
  t('frozen: 状态行 FROZEN → frozen', isFrozen('> 状态：**FROZEN v1.0**（human 冻结）\n正文提到 FROZEN 不影响'), true);
  t('frozen: 中文「状态：」FROZEN → frozen', isFrozen('> 状态：**FROZEN / APPROVED** v4'), true);
  t('frozen: Status 行 FROZEN → frozen', isFrozen('> Status: **FROZEN v1.0**'), true);
  t('frozen: DRAFT 主导（先位）→ 非 frozen', isFrozen('> Status: **DRAFT v0.5**（R2 GREEN 后标 FROZEN）'), false);
  t('frozen: 正文含 FROZEN 但无状态行 → 非 frozen（F2 回归）', isFrozen('# 提案\n正文说「评审通过后标 FROZEN」'), false);
  // landed verdict 闭集（F1 回归）：用 os.tmpdir() 夹具测 deriveLanded 真函数（不写 changes/arch、不污染收件箱）
  const osTmp = process.env.TMPDIR || process.env.TMP || process.env.TEMP || '/tmp';
  const mkFix = (id, verdictLines) => {
    const d = resolve(osTmp, 'changes-index-selfcheck-' + id);
    try {
      mkdirSync(d, { recursive: true });
      writeFileSync(join(d, 'landing-verdict.md'), verdictLines.join('\n'), 'utf8');
      return d;
    } catch { return null; }
  };
  const rmFix = (d) => { try { rmSync(d, { recursive: true, force: true }); } catch {} };
  const f1 = mkFix('a', ['**verdict = PASS（绿）**']);
  if (f1) { t('landed: verdict = PASS → true', deriveLanded(f1, {}), true); rmFix(f1); }
  const f2 = mkFix('b', ['> **verdict: GREEN**']);
  if (f2) { t('landed: verdict: GREEN → true（F1 回归）', deriveLanded(f2, {}), true); rmFix(f2); }
  const f3 = mkFix('c', ['**verdict: RED**', '...', '**verdict: GREEN**']);
  if (f3) { t('landed: RED→GREEN 翻判取最终 GREEN（F1 回归）', deriveLanded(f3, {}), true); rmFix(f3); }
  const f4 = mkFix('d', ['**verdict: GREEN**', '**verdict: RED**']);
  if (f4) { t('landed: 最终 verdict=RED → 非 landed', deriveLanded(f4, {}), false); rmFix(f4); }
  t('landed: 人工 adjudication 兜底（缓存 landed=true）', deriveLanded(resolve(osTmp, 'nonexistent-dir'), { landed: true }), true);
  t('landed: 无 landing-verdict → 非 landed', deriveLanded(resolve(osTmp, 'nonexistent-dir'), {}), false);
  // 日期前缀解析
  t('date: YYMMDD- 前缀解析', parseChangeIdDate('260823-gov-review-x'), '2026-08-23');
  t('date: 无前缀 → 空', parseChangeIdDate('gov-review-foo'), '');
  // 命名断言（260825-gov-review-track-b-naming-convention T3；ARQ-114 先例结构化用例）
  t('naming: 日期前缀 + gov-review token → 通过', checkArchDirNaming('260825-gov-review-x'), null);
  t('naming: 日期前缀 + arch-review token → 通过', checkArchDirNaming('260825-arch-review-x'), null);
  t('naming: 日期前缀缺 token → fail-loud 点名', String(checkArchDirNaming('260825-naming-fixture-no-token') || '').startsWith('naming:'), true);
  t('naming: 非日期前缀缺 token → 宽容', checkArchDirNaming('gov-review-foo'), null);
  t('naming: 非法日期前缀（999999-）→ 不断言', checkArchDirNaming('999999-foo'), null);
  // 陈旧判定（260826-gov-review-arch-dir-gc T1；真空绿：显式夹具非空输入）
  t('stale: 已落地且超龄 → 非陈旧（永久保留）', staleCandidate({ landed: true, firstCommit: '2020-01-01', thresholdDays: 30, now: '2026-08-26' }), false);
  t('stale: 未落地且超龄 → 陈旧', staleCandidate({ landed: false, firstCommit: '2026-07-01', thresholdDays: 30, now: '2026-08-26' }), true);
  t('stale: 未落地未超龄 → 非陈旧', staleCandidate({ landed: false, firstCommit: '2026-08-20', thresholdDays: 30, now: '2026-08-26' }), false);
  t('stale: 恰 30 日（未超）→ 非陈旧', staleCandidate({ landed: false, firstCommit: '2026-07-27', thresholdDays: 30, now: '2026-08-26' }), false);
  t('stale: firstCommit 空 → 非陈旧（降级）', staleCandidate({ landed: false, firstCommit: '', thresholdDays: 30, now: '2026-08-26' }), false);
  const pass = cases.filter(c => c.pass).length;
  const ok = pass === cases.length;
  return { selfCheck: 'changes-index', ok, pass, fail: cases.length - pass, total: cases.length, cases: ok ? undefined : cases.filter(c => !c.pass) };
}

function main() {
  const cache = loadStateCache();
  const items = [...collectDev(), ...collectArch(cache)];
  const indexMd = resolve(projectDir, 'changes', 'INDEX.md');
  writeFileSync(indexMd, buildIndexMd(items), 'utf8');
  return { ok: true, devCount: items.filter(i => i.lane === 'dev').length, archCount: items.filter(i => i.lane === 'arch').length };
}

const invokedDirectly = process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url);
if (invokedDirectly) {
  const args = process.argv.slice(2);
  let result;
  if (args[0] === '--self-check') {
    result = selfCheck();
  } else if (args[0] === '--backfill') {
    result = backfill();
  } else if (args[0] === '--stale-report') {
    result = staleReport();
  } else if (args[0] === '--upsert') {
    const id = args[1];
    if (!id) { console.error(JSON.stringify({ ok: false, error: '--upsert requires <changeId>' })); process.exit(2); }
    const cache = loadStateCache();
    result = upsertArchRow(resolve(projectDir, 'changes', 'INDEX.md'), id, cache);
    saveStateCache(cache);
  } else {
    result = main();
  }
  console.log(JSON.stringify(result, null, 2));
  if (result && result.ok === false) process.exit(1);
  if (result && result.selfCheck && !result.ok) process.exit(1);
}

export { main, upsertArchRow, backfill, staleReport, selfCheck, computeArchEntry, deriveLanded, staleCandidate };
