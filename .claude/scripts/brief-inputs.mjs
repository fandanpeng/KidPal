#!/usr/bin/env node
// brief-inputs.mjs · dev-loop 派发输入面封闭化：从 TaskPackage refs + crossEnd 派生 input-brief
// 立法来源：delivery/changes/arch/260908-gov-review-input-surface-frontload/PROPOSAL.md（FROZEN v1.0 · T2）
// 承载 DEV-GOVERNANCE「起包格式期望前置」第六条第 ①②③ 件（同批 T1）。
//
// 定位：**派生清单，不新造事实源**——只读 taskpackage.json 与被引文件，只写 taskpackages 目录内 brief。
// 四路展开：① 契约/spec 锚点节  ② 跨端对端面（crossEnd）  ③ 同 subsystem 先例封顶 3  ④ 超读申报段模板
// 生成时点 = planner Step 7 派发前一次产双份（acceptance-author + builder）；Step 8 核在场；Step 9 只引用。
// advisory（非门）：引 wire 节 ∧ counterparts 空 ∧ 无 exempt → stderr WARN 提示 planner 复核申报。

import { existsSync, readFileSync, writeFileSync, readdirSync } from 'node:fs';
import { resolve, join } from 'node:path';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// ── wire 节常量（advisory 谓词的唯一事实源）────────────────────────────────────
// 判定源 = docs/contracts/interface-contract.md 的节号。**契约经人类契约门改节号时，须同批同步本常量。**
// 无维护锚的硬编码节号 = 契约改号后 WARN 静默失效（vacuous advisory），故在此显式钉死并注释。
// 单源义务（R2-F1 教训）：§1 P2「净增覆盖」census 与本 advisory 必须共用本常量 + 同一「锚点绑定文件名」
// 谓词（isWireRef 即该谓词本体，普查脚本直接 import 它，不得另写一套）。
// 实现期待评（是否并入集合，凭 pilot 数据裁）：§1.2 / §1.3（display 顶层结构 / view 类型）、§二（课表 MD）。
export const CONTRACT_WIRE_FILE = 'docs/contracts/interface-contract.md';
export const WIRE_SECTIONS = ['§1.5', '§三', '§四'];
// ──────────────────────────────────────────────────────────────────────────────

const ROLES = ['acceptance-author', 'builder'];
const PRECEDENT_CAP = 3;
const ANCHOR_RE = /^(.*?)#(.+)$/;

function parseArgs(argv) {
  const out = { changeId: null, role: null, dryRun: false, selfCheck: false, regeneratedBecause: null };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    if (a === '--change-id') out.changeId = argv[++i];
    else if (a === '--role') out.role = argv[++i];
    else if (a === '--dry-run') out.dryRun = true;
    else if (a === '--self-check') out.selfCheck = true;
    else if (a === '--regenerated-because') out.regeneratedBecause = argv[++i];
    else fail('unknown argument', { arg: a });
  }
  return out;
}

function fail(message, details = {}) {
  console.error(`brief-inputs: FAIL — ${message}`);
  if (Object.keys(details).length > 0) console.error(JSON.stringify(details, null, 2));
  process.exit(1);
}

function nonEmptyString(v) {
  return typeof v === 'string' && v.trim().length > 0;
}

// 锚点拆分：'docs/contracts/interface-contract.md#§1.5' → { path, anchor }；裸路径 → anchor = null
function splitRef(ref) {
  const s = String(ref).trim();
  const m = ANCHOR_RE.exec(s);
  if (!m) return { path: s, anchor: null };
  return { path: m[1], anchor: m[2] };
}

// advisory 谓词本体：锚点**绑定**契约文件名 ∧ 节 ∈ WIRE_SECTIONS。
// （R2-F1 的错法就是只匹配节号文本不绑文件名——'docs/notes/feature-inventory.md#§1.5' 会被误纳。）
export function isWireRef(ref) {
  const { path, anchor } = splitRef(ref);
  if (!anchor) return false;
  if (path !== CONTRACT_WIRE_FILE) return false;
  return WIRE_SECTIONS.some((w) => anchor === w || anchor.startsWith(w));
}

function collectRefs(tp) {
  const buckets = ['contractRefs', 'docRefs', 'sourceRefs'];
  const seen = new Set();
  const out = [];
  for (const b of buckets) {
    const arr = Array.isArray(tp[b]) ? tp[b] : [];
    for (const raw of arr) {
      if (!nonEmptyString(raw)) continue;
      const key = String(raw).trim();
      if (seen.has(key)) continue;
      seen.add(key);
      out.push({ bucket: b, ref: key, ...splitRef(key) });
    }
  }
  return out;
}

// 读目标文件的 markdown 标题索引（进程内缓存）
const headingCache = new Map();
function headingsOf(relPath) {
  if (headingCache.has(relPath)) return headingCache.get(relPath);
  const abs = resolve(projectDir, relPath);
  let list = [];
  if (existsSync(abs)) {
    const lines = readFileSync(abs, 'utf8').replace(/^﻿/, '').split(/\r?\n/);
    list = lines
      .map((text, idx) => ({ line: idx + 1, text }))
      .filter((r) => /^#{1,6}\s/.test(r.text))
      .map((r) => ({ line: r.line, level: (r.text.match(/^#+/) || ['#'])[0].length, title: r.text.replace(/^#+\s*/, '').trim() }));
  }
  headingCache.set(relPath, list);
  return list;
}

// 锚点 → 所属标题节：先精确「标题以锚点文本开头」，再退「标题含锚点文本」，都拿不到则 not-found（如实呈现，不猜）
function locateSection(relPath, anchor) {
  const needle = String(anchor).replace(/^§\s*/, '').trim();
  const hs = headingsOf(relPath);
  const norm = (t) => t.replace(/^§\s*/, '').replace(/\s+/g, ' ').trim();
  let hit = hs.find((h) => norm(h.title).startsWith(norm(needle)));
  if (!hit) hit = hs.find((h) => norm(h.title).includes(norm(needle)));
  return hit || null;
}

function renderAnchorSections(refs, advisories) {
  const rows = [];
  for (const r of refs) {
    if (!existsSync(resolve(projectDir, r.path))) {
      rows.push({ status: 'missing', ref: r.ref, path: r.path, anchor: r.anchor, title: '（文件不存在）' });
      continue;
    }
    if (!r.anchor) {
      rows.push({ status: 'bare', ref: r.ref, path: r.path, anchor: null, title: '无锚点（整文件）' });
      advisories.push(`ref 无锚点，清单退化为整文件引用：${r.ref}（建议起包期补 #锚点 以封闭阅读面）`);
      continue;
    }
    const sec = locateSection(r.path, r.anchor);
    rows.push({
      status: sec ? 'ok' : 'not-found',
      ref: r.ref,
      path: r.path,
      anchor: r.anchor,
      title: sec ? sec.title : '（未定位到标题节）',
      secLine: sec ? sec.line : null,
    });
  }
  return rows;
}

// ② 跨端对端面：解析 symbol 行号区间（生成时点快照，非长期事实源）
function readLines(relPath) {
  return readFileSync(resolve(projectDir, relPath), 'utf8').replace(/^﻿/, '').split(/\r?\n/);
}

export function resolveSymbolRange(relPath, symbol) {
  const abs = resolve(projectDir, relPath);
  if (!existsSync(abs)) return { error: 'path-not-found', path: relPath, symbol };
  const lines = readLines(relPath);
  const hits = [];
  for (let i = 0; i < lines.length; i += 1) {
    if (new RegExp(`(^|[^\\w:])${escapeRe(symbol)}([\\w:(]|$)`).test(lines[i])) hits.push(i + 1);
  }
  if (hits.length === 0) return { error: 'symbol-not-found', path: relPath, symbol, totalLines: lines.length };
  const start = hits[0];
  let end = Math.min(lines.length, start + 40);
  let depth = 0;
  let opened = false;
  for (let i = start - 1; i < lines.length; i += 1) {
    for (const ch of lines[i]) {
      if (ch === '{') { depth += 1; opened = true; }
      else if (ch === '}') depth -= 1;
    }
    if (opened && depth <= 0) { end = i + 1; break; }
    if (i - (start - 1) > 400) break; // 病态大块封顶，避免整文件
  }
  return { path: relPath, symbol, start, end, extraHits: hits.slice(1, 10), totalLines: lines.length };
}

function escapeRe(s) {
  return String(s).replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

function renderCounterparts(tp, errors) {
  const ce = tp.crossEnd;
  if (!ce || typeof ce !== 'object') {
    return { present: false, resolved: [], skeleton: false, exempt: null };
  }
  const parts = Array.isArray(ce.counterparts) ? ce.counterparts : [];
  const exempt = nonEmptyString(ce.exempt) ? String(ce.exempt).trim() : null;
  const resolved = [];
  for (const [i, entry] of parts.entries()) {
    const path = entry && typeof entry === 'object' ? entry.path : null;
    const symbol = entry && typeof entry === 'object' ? entry.symbol : null;
    if (!nonEmptyString(path) || !nonEmptyString(symbol)) {
      errors.push({ kind: 'counterpart-shape', entry: `counterparts[${i}]`, hint: '每条须含 path + symbol 两键' });
      continue;
    }
    // 本批覆盖档 = 自有实现面消费方（firmware/** ∨ server/**）；②③档留 ARQ（PROPOSAL §6）
    if (!(path.startsWith('firmware/') || path.startsWith('server/'))) {
      advisoriesGlobal.push(`counterparts[${i}] 指向本批未覆盖档（只读参考树 / 黑盒无源码消费方见 §6 留根）：${path} — 本批只承载自有实现面，请改填 exempt 或留待该 ARQ 收编`);
      continue;
    }
    const r = resolveSymbolRange(path, symbol);
    if (r.error) {
      // fail-loud（§8-B1）：绝不静默产出空锚——静默空锚会让 crossEnd 的牙全泄
      errors.push({ kind: 'counterpart-unresolved', entry: `${path}#${symbol}`, reason: r.error });
      continue;
    }
    resolved.push(r);
  }
  return { present: true, resolved, skeleton: parts.length === 0, exempt, raw: ce };
}

// ③ 先例封顶：三判定源钉死（R1/F8）——merged / subsystem token 包含 / changeId 日期前缀
function isMerged(changeId) {
  const verdictPath = resolve(projectDir, 'delivery/changes/dev', changeId, 'supervisor-verdict.json');
  if (existsSync(verdictPath)) {
    try {
      const v = JSON.parse(readFileSync(verdictPath, 'utf8').replace(/^﻿/, ''));
      const verdict = v.verdict || (v.decision && v.decision.verdict) || (v.result && v.result.verdict);
      if (String(verdict).toUpperCase() === 'APPROVE') return true;
    } catch { /* 非法 JSON 视作未判 */ }
  }
  const events = resolve(projectDir, 'delivery/taskpackages', changeId, 'pod-events.jsonl');
  if (existsSync(events)) {
    for (const line of readFileSync(events, 'utf8').split(/\r?\n/)) {
      if (!line.trim()) continue;
      if (/"eventType"\s*:\s*"merge-(completed|merged)"/.test(line)) return true;
      if (/"eventType"\s*:\s*"[^"]*merge[^"]*"/.test(line)) return true;
    }
  }
  return false;
}

function subsystemTokens(tp) {
  return new Set(String(tp.subsystem || '').split('+').map((t) => t.trim()).filter(Boolean));
}

function datePrefix(changeId) {
  const m = /^(\d{6})-/.exec(String(changeId));
  return m ? m[1] : '000000';
}

function findPrecedents(tp, repoRoot = projectDir) {
  const selfId = tp.changeId;
  const mine = subsystemTokens(tp);
  const base = join(repoRoot, 'delivery', 'taskpackages');
  if (!existsSync(base)) return [];
  const cands = [];
  for (const dir of readdirSync(base, { withFileTypes: true })) {
    if (!dir.isDirectory()) continue;
    if (dir.name === 'current') continue; // 指针副本，排除（R1/F2 census 污染实例）
    if (dir.name === selfId) continue;
    const tpPath = join(base, dir.name, 'taskpackage.json');
    if (!existsSync(tpPath)) continue;
    let other;
    try { other = JSON.parse(readFileSync(tpPath, 'utf8').replace(/^﻿/, '')); } catch { continue; }
    if (!Array.isArray(other.acceptanceTestsFile) || other.acceptanceTestsFile.length === 0) continue;
    const theirs = subsystemTokens(other);
    const overlap = [...mine].some((t) => theirs.has(t));
    if (!overlap) continue; // token 化包含，非字符串相等
    if (!isMerged(other.changeId || dir.name)) continue;
    cands.push({ changeId: other.changeId || dir.name, date: datePrefix(other.changeId || dir.name), files: other.acceptanceTestsFile });
  }
  cands.sort((a, b) => (a.date < b.date ? 1 : a.date > b.date ? -1 : String(a.changeId).localeCompare(String(b.changeId))));
  return cands.slice(0, PRECEDENT_CAP);
}

// advisory（非门）：wire 节引用 ∧ counterparts 空 ∧ 无 exempt → WARN
function wireAdvisory(tp, ce) {
  const refs = collectRefs(tp);
  const hitsWire = refs.filter((r) => isWireRef(r.ref));
  const counterpartsEmpty = !ce.present || ce.skeleton;
  const hasExempt = !!(ce.present && ce.exempt);
  if (hitsWire.length > 0 && counterpartsEmpty && !hasExempt) {
    return `本包 refs 引契约 wire 节（${hitsWire.map((r) => r.anchor).join(', ')}）但 crossEnd.counterparts 为空且未声明 exempt——请 planner 复核跨端申报（判断不了按存疑向严填 counterparts，不填 exempt）。`;
  }
  return null;
}

const advisoriesGlobal = [];

function renderBrief(tp, role, opts = {}) {
  const errors = [];
  const advisories = [];
  const now = new Date().toISOString();
  const anchorRows = renderAnchorSections(collectRefs(tp), advisories);
  const ce = renderCounterparts(tp, errors);
  const precedents = findPrecedents(tp);
  const warn = wireAdvisory(tp, ce);

  const L = [];
  L.push(`# input-brief · ${role} · ${tp.changeId}`);
  L.push('');
  L.push(`> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。`);
  L.push(`> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 \`brief-inputs.mjs\` 取新行号，不得照旧行号通读整文件。`);
  L.push(`> generated-at: ${now} · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: ${role}`);
  if (nonEmptyString(opts.regeneratedBecause)) {
    L.push(`> regenerated-because: ${String(opts.regeneratedBecause).trim()}（覆盖上一份快照，旧 generated-at 见 git diff 上一版本）`);
  }
  L.push(`> 来源 TaskPackage: \`delivery/taskpackages/${tp.changeId}/taskpackage.json\` · subsystem: ${tp.subsystem || '(空)'} · rigor: ${tp.rigor || 'full'}`);
  L.push('');
  L.push(`## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）`);
  L.push('');
  L.push('| 路径 | 锚点 | 节标题 | 节首行 | 状态 |');
  L.push('|---|---|---|---|---|');
  if (anchorRows.length === 0) L.push('| （refs 为空） | — | — | — | empty |');
  for (const r of anchorRows) {
    L.push(`| \`${r.path}\` | ${r.anchor ? `\`${r.anchor}\`` : '—'} | ${r.title} | ${r.secLine != null ? r.secLine : '—'} | ${r.status} |`);
  }
  L.push('');
  L.push(`## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）`);
  L.push('');
  if (!ce.present) {
    L.push('`crossEnd` 字段**不在场**。起包期必须补骨架 `{ "counterparts": [] }`（空骨架也行，字段必须在场）——见 DEV-GOVERNANCE「起包格式期望前置」第 6 条。');
  } else if (ce.exempt) {
    L.push(`本包声明 **exempt**（不适用跨端对端阅读）：`);
    L.push('');
    L.push(`> ${ce.exempt}`);
    L.push('');
    L.push('豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。');
  } else if (ce.skeleton) {
    L.push('`counterparts` 为空骨架且未声明 exempt → **finalize 态闸将红点名**（taskpackage-required-check T3.2 二选一）。');
  }
  if (ce.resolved.length > 0) {
    L.push('| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |');
    L.push('|---|---|---|---|');
    for (const r of ce.resolved) {
      L.push(`| \`${r.path}\` | \`${r.symbol}\` | :${r.start}-${r.end} | ${r.extraHits.length ? r.extraHits.join(', ') : '—'} |`);
    }
    L.push('');
    L.push('上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。');
  }
  L.push('');
  L.push(`## ③ 先例（同 subsystem 已 merged 包，封顶 ${PRECEDENT_CAP} 个，按 changeId 日期前缀倒序）`);
  L.push('');
  if (precedents.length === 0) {
    L.push('（无同 subsystem 的已 merged 先例包）');
  } else {
    L.push('| 先例包 | 冻结验收文件 |');
    L.push('|---|---|');
    for (const p of precedents) L.push(`| ${p.changeId} | ${p.files.map((f) => `\`${f}\``).join(' · ')} |`);
    L.push('');
    L.push('封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。');
  }
  L.push('');
  L.push(`## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）`);
  L.push('');
  L.push('交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。');
  L.push('');
  L.push('| 清单外文件 | 原因（一句话） |');
  L.push('|---|---|');
  L.push('| （无） | — |');
  L.push('');

  // 全局 ADV（②③档越界提示等）并入返回值，调用方只见一个 advisories 数组（防双清单漂移）
  const allAdvisories = advisories.concat(advisoriesGlobal.splice(0));
  return { markdown: L.join('\n') + '\n', errors, advisories: allAdvisories, warnings: warn ? [warn] : [] };
}

// ── self-check：真空绿规约（fixture 非空断言先行）──────────────────────────────
function selfCheck() {
  const checks = [];
  const ok = (name, pass, detail = '') => checks.push({ name, pass, detail });

  const fixtureTp = {
    changeId: '260907-g-relay-fix',
    subsystem: 'server',
    rigor: 'lite',
    contractRefs: [CONTRACT_WIRE_FILE],
    docRefs: [`${CONTRACT_WIRE_FILE}#§1.5`, `${CONTRACT_WIRE_FILE}#§三`],
    sourceRefs: [`${CONTRACT_WIRE_FILE}#§三`, 'docs/DEV-GOVERNANCE.md'],
    crossEnd: { counterparts: [], exempt: '' },
    acceptanceTestsFile: ['server/acceptance/test_g_relay_fix.acceptance.py'],
  };
  // 反真空绿：fixture 本身非空先行断言
  ok('fixture-nonempty', Array.isArray(fixtureTp.docRefs) && fixtureTp.docRefs.length >= 2
      && !!fixtureTp.crossEnd, JSON.stringify(fixtureTp.docRefs));

  const a = renderBrief(fixtureTp, 'acceptance-author');
  ok('① 锚点节形状', /\|\s*路径\s*\|\s*锚点\s*\|\s*节标题\s*\|\s*节首行\s*\|\s*状态\s*\|/.test(a.markdown)
      && a.markdown.includes(CONTRACT_WIRE_FILE), 'table header + contract path');
  ok('① 不抽正文', !/```(py|c|cpp|json)/.test(a.markdown), 'no fenced source bodies');
  ok('① 裸路径标「无锚点（整文件）」', a.markdown.includes('无锚点（整文件）')
      && a.advisories.some((s) => s.includes('无锚点')), 'DEV-GOVERNANCE.md bare ref → bare marker + advisory (R1/F12)');
  ok('② 对端面节在场（空骨架分支）', a.markdown.includes('## ② 跨端对端面')
      && a.markdown.includes('finalize 态闸将红点名'), 'skeleton branch text');
  ok('③ 先例封顶 ≤3', (() => {
    const rows = a.markdown.split('\n').filter((l) => /^\| 260\d{3}-/.test(l));
    return rows.length <= PRECEDENT_CAP;
  })(), 'precedent rows capped');
  ok('④ 超读申报空表', a.markdown.includes('| （无） | — |'), 'empty declaration table');
  ok('快照语义成文', a.markdown.includes('非长期事实源') && /generated-at:/.test(a.markdown), 'R1/F11');

  // wire advisory 谓词（含 R2-F1 的「绑文件名」臂）
  ok('isWireRef 正例（绑契约文件名）', isWireRef(`${CONTRACT_WIRE_FILE}#§四`), 'true expected');
  ok('isWireRef 反例（异文件同节）', !isWireRef('docs/notes/feature-inventory.md#§1.5'),
      'R2-F1 误计实例须判 false');
  ok('isWireRef 反例（裸路径无锚）', !isWireRef(CONTRACT_WIRE_FILE), 'bare path not wire');
  ok('wire WARN 触发', a.warnings.length === 1 && a.warnings[0].includes('crossEnd'), 'warn when counterparts empty & no exempt');

  // fail-loud 路径（§8-B1）
  const badTp = { ...fixtureTp, crossEnd: { counterparts: [{ path: 'firmware/main/nonexistent_xxx.cc', symbol: 'NoSuchSymbol' }] } };
  const bad = renderBrief(badTp, 'builder');
  ok('② 解析失败 fail-loud', bad.errors.some((e) => e.kind === 'counterpart-unresolved' && e.entry.includes('NoSuchSymbol')),
      'errors carry named entry, not silent empty anchor');
  const okTp = { ...fixtureTp, crossEnd: { counterparts: [{ path: 'firmware/main/mcp_server.cc', symbol: 'McpServer' }] } };
  const good = renderBrief(okTp, 'builder');
  ok('② 自有面正例可解析', good.errors.length === 0 && good.markdown.includes('firmware/main/mcp_server.cc'),
      'resolved range rendered');
  const offTp = { ...fixtureTp, crossEnd: { counterparts: [{ path: 'ESP32-S3-RLCD-4.2-Demo/x.cc', symbol: 'Y' }] } };
  const off = renderBrief(offTp, 'builder');
  ok('②③档越界提示（ADV 真发出、不静默不硬红）', off.advisories.some((s) => s.includes('未覆盖档'))
      && !off.errors.some((e) => e.kind === 'counterpart-unresolved'),
      'landing-verifier 复核指出原臂含恒真合取（>=0）系 vacuous，改正为必验 ADV 实发');

  // 三判定源抽样（R1/F8）
  const mergedProbe = isMerged('260829-story-p3') || isMerged('260829-schedule-p2');
  ok('③ merged 判定源可用（APPROVE ∨ merge 事件）', mergedProbe, 'at least one known-merged package resolves true');
  ok('③ subsystem token 包含（非字符串相等）', (() => {
    const t = subsystemTokens({ subsystem: 'firmware+server' });
    return t.has('firmware') && t.has('server') && t.size === 2;
  })(), 'tokenization');
  ok('③ 时间轴 = changeId 日期前缀', datePrefix('260903-gateway-directionB') === '260903', 'prefix extraction');
  ok('current/ 指针副本排除', !existsSync(join(projectDir, 'delivery/taskpackages'))
      || (() => {
        const names = readdirSync(join(projectDir, 'delivery/taskpackages'), { withFileTypes: true })
          .filter((d) => d.isDirectory()).map((d) => d.name);
        const probe = { changeId: 'x', subsystem: names.length ? 'firmware+server' : 'none', acceptanceTestsFile: [] };
        const found = findPrecedents(probe).some((p) => p.changeId === 'current');
        return !found;
      })(), 'no precedent may be named current');

  const failed = checks.filter((c) => !c.pass);
  for (const c of checks) console.log(`${c.pass ? 'PASS' : 'FAIL'}  ${c.name}${c.detail ? `  [${c.detail}]` : ''}`);
  console.log(`\nbrief-inputs --self-check: ${checks.length - failed.length}/${checks.length} passed`);
  if (failed.length > 0) {
    console.error('SELF-CHECK RED');
    process.exit(1);
  }
  console.log('SELF-CHECK GREEN');
  process.exit(0);
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  if (args.selfCheck) selfCheck();
  if (!args.changeId) fail('--change-id required');
  if (!ROLES.includes(args.role)) fail('--role must be acceptance-author|builder', { got: args.role });

  const dir = resolve(projectDir, 'delivery/taskpackages', args.changeId);
  const tpPath = join(dir, 'taskpackage.json');
  if (!existsSync(tpPath)) fail('TaskPackage not found', { expected: tpPath });
  let tp;
  try { tp = JSON.parse(readFileSync(tpPath, 'utf8').replace(/^﻿/, '')); }
  catch (e) { fail('TaskPackage is not valid JSON', { tpPath, error: String(e.message || e) }); }

  const { markdown, errors, advisories, warnings } = renderBrief(tp, args.role, { regeneratedBecause: args.regeneratedBecause });

  for (const w of warnings) console.error(`brief-inputs: WARN(advisory) — ${w}`);
  for (const adv of advisories) console.error(`brief-inputs: ADV — ${adv}`);
  for (const adv of advisoriesGlobal) console.error(`brief-inputs: ADV — ${adv}`);

  if (errors.length > 0) {
    // fail-loud：不产出带空锚的 brief，避免子 agent 按「清单已封闭」误信
    console.error(`brief-inputs: ${errors.length} 条对端条目解析失败，不产出 brief：`);
    for (const e of errors) console.error(`  · ${e.kind}: ${e.entry}${e.reason ? ` (${e.reason})` : ''} — ${e.hint || '请修正 crossEnd 后重跑'}`);
    process.exit(1);
  }

  const outPath = join(dir, `input-brief-${args.role}.md`);
  if (args.dryRun) {
    process.stdout.write(markdown);
    return;
  }
  writeFileSync(outPath, markdown, 'utf8');
  console.log(JSON.stringify({ ok: true, script: 'brief-inputs', role: args.role, changeId: args.changeId, brief: outPath, warnings: warnings.length, advisories: advisories.length }, null, 2));
}

if (await isMainScript()) main();

// Windows 下 process.argv[1] 是相对/反斜杠形态，直接比 import.meta.url 会恒假（实测首跑静默退出）
async function isMainScript() {
  const { pathToFileURL } = await import('node:url');
  const entry = process.argv[1];
  return !!entry && import.meta.url === pathToFileURL(resolve(entry)).href;
}
