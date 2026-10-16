#!/usr/bin/env node
// brief-inputs.mjs · dev-loop 派发输入面封闭化：从 TaskPackage refs + crossEnd 派生 input-brief
// 立法来源：delivery/archive/changes/arch/260908-gov-review-input-surface-frontload/PROPOSAL.md（FROZEN v1.0 · T2）
// 承载 DEV-GOVERNANCE「起包格式期望前置」第六条第 ①②③ 件（同批 T1）。
//
// 定位：**派生清单，不新造事实源**——只读 taskpackage.json 与被引文件，只写 taskpackages 目录内 brief。
// 四路展开：① 契约/spec 锚点节  ② 跨端对端面（crossEnd）  ③ 同 subsystem 先例封顶 3  ④ 超读申报段模板
// 第五路（260920-gov-review-bearing-strength-align T1，FROZEN 2026-09-20）：⑤ 跨包承接面——扫**两写面**
// （`delivery/taskpackages/*/taskpackage.json` 包面 + `delivery/workstreams/*/decision-coverage.json`
// workstream 面，排除 `_archive/`），取**承接包 = 本包 changeId** 的条目。**承接包判定统一为
// `carriedBy ?? owner`**（两写面在这一位上用不同的键：workstream 面读 `carriedBy`、包面读 `owner`——
// 按 `owner` 单键判会把「谁定语义」当成「谁承接」）；行为名**读侧兼容** `topic` ∨ `behavior`（包面存量写
// `behavior`）。零命中**明写 `none`**（防「没写」与「没有」混同）。方向 = 补可见性，不改任何门判定。
// 生成时点 = planner Step 7 派发前一次产单份（acceptance-author + builder 共用 `input-brief.md`——
// 261015-gov-review-lite-fast-path T4 双份并单份）；Step 8 核在场；Step 9 只引用。
// advisory（非门）：引 wire 节 ∧ counterparts 空 ∧ 无 exempt → stderr WARN 提示 planner 复核申报。

import { existsSync, mkdirSync, mkdtempSync, readFileSync, readdirSync, rmSync, writeFileSync } from 'node:fs';
import { resolve, join } from 'node:path';
import { tmpdir } from 'node:os';

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

const PRECEDENT_CAP = 3;
const ANCHOR_RE = /^(.*?)#(.+)$/;

function parseArgs(argv) {
  const out = { changeId: null, role: null, dryRun: false, selfCheck: false, regeneratedBecause: null };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    if (a === '--change-id') out.changeId = argv[++i];
    else if (a === '--dry-run') out.dryRun = true;
    else if (a === '--self-check') out.selfCheck = true;
    else if (a === '--regenerated-because') out.regeneratedBecause = argv[++i];
    // --role 已退役（261015-gov-review-lite-fast-path T4 双份并单份）：传入即 unknown argument fail-loud
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

// story 照录路（260928-gov-review-portfolio-boundary-story-propagation T2）：裸 story id
// （`S-Fx.y` / `S-Fx.y-ACn`）→ 定位 `docs/requirements.md` 的 `##### S-Fx.y · <标题>` 节 →
// 整节照录进 brief（machine-derived 投影、语义权威仍在 requirements 现文）。节定位失败走
// advisory 通道（不 exit 1，与第①路 missing/not-found 态同构，不猜）。
const STORY_ID_RE = /^S-F\d+\.\d+(-AC\d+)?$/;
const REQUIREMENTS_FILE = 'docs/requirements.md';

export function isStoryRef(ref) {
  return STORY_ID_RE.test(String(ref).trim());
}

function storySectionText(storyId) {
  const hs = headingsOf(REQUIREMENTS_FILE);
  const needle = storyId.replace(/-AC\d+$/, '').trim();
  const hit = hs.find((h) => h.title.split('·')[0].trim() === needle);
  if (!hit) return null;
  const next = hs.find((h) => h.line > hit.line && h.level <= hit.level);
  const lines = readLines(REQUIREMENTS_FILE);
  return { title: hit.title, line: hit.line, text: lines.slice(hit.line - 1, next ? next.line - 1 : undefined).join('\n').trimEnd() };
}

function renderStoryRoad(storyRefs, advisories, opts = {}) {
  // 基 id 去重（261015-gov-review-lite-fast-path T4）：`S-Fx.y-AC1` 与 `S-Fx.y-AC3` 同基 id——同节只贴一次、
  // AC 粒度回引标签并列；story 级引用与 AC 级引用同基 id 时同样合并。
  const byBase = new Map(); // base id -> { storyLevel: bool, acTags: [] }
  for (const r of storyRefs) {
    const id = String(r.path).trim();
    const base = id.replace(/-AC\d+$/, '');
    const entry = byBase.get(base) || { storyLevel: false, acTags: [] };
    if (id === base) entry.storyLevel = true;
    else entry.acTags.push(id);
    byBase.set(base, entry);
  }
  if (byBase.size === 0) return null;
  // bug 类降级（261015-gov-review-lite-fast-path T4，立项 ack 第 5 条）：sourceType ∈ bug 类且 planRef 在场 →
  // ①b 渲染指针形态（语义权威 = requirements 现文，不照录；story 节对本包仅作背景）。
  const bugDowngrade = !!opts.bugDowngrade;
  const L = [];
  if (bugDowngrade) {
    L.push('## ①b · story 语义权威（bug 类包 · 指针形态——改动指令源在 plan.md）');
  } else {
    L.push('## ①b · story 语义权威（S-Fx.y 整节照录 · machine-derived 投影）');
  }
  L.push('');
  for (const [base, entry] of byBase) {
    const tags = [...(entry.storyLevel ? [base] : []), ...entry.acTags];
    const tagLine = (entry.storyLevel && entry.acTags.length === 0)
      ? ''
      : ` · 本包声明 AC 粒度回引：${tags.map((t) => `\`${t}\``).join('、')}`;
    const sec = storySectionText(base);
    if (!sec) {
      L.push(`### ${base}`);
      L.push('');
      L.push('> ⚠ 未在 `docs/requirements.md` 定位到该 story 节（story id 可能写错或该节已迁移）——如实呈现、不猜；起包期核对 sourceRefs。');
      L.push('');
      advisories.push(`story ref 未定位到节：${base}（docs/requirements.md 无 \`${base} ·\` 标题节——核对 sourceRefs 拼写或该节是否已迁移）`);
      continue;
    }
    L.push(`### ${base} · 源 = \`docs/requirements.md\` 节首行 :${sec.line}${tagLine}`);
    L.push('');
    if (bugDowngrade) {
      L.push(`> 本包 sourceType 为 bug 类且 planRef 在场：**不照录整节**——语义权威 = \`docs/requirements.md\` 现文（\`${sec.title}\`），按需自读；story 节对本包仅作背景，真实改动指令 = plan.md（见头部改动指令源行）。`);
    } else {
      L.push('> **语义权威仍在 requirements 现文**；本段为脚本只读投影（整节照录、不改写不蒸馏），requirements 变更后重跑本脚本刷新。');
      L.push('');
      L.push('```markdown');
      L.push(sec.text);
      L.push('```');
    }
    L.push('');
  }
  return L;
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

// uiux 设计 refs 透传（260929-gov-review-req-loop-ui-author T5.3 立；260929-gov-review-ui-ladder-remap T7
// 剥离已撤并制品的子源检查——本函数只收 uiux block 的 designRefs/sharedUiRefs）。
// 在位才列、缺席不脑补（none = 结论不是省略）；悬空 ref 报 advisory 不入列。
function collectUiuxRefs(tp, repoRoot) {
  const out = [];
  const uiux = tp.uiux && typeof tp.uiux === 'object' ? tp.uiux : null;
  for (const key of ['designRefs', 'sharedUiRefs']) {
    const arr = uiux && Array.isArray(uiux[key]) ? uiux[key] : [];
    for (const raw of arr) {
      if (!nonEmptyString(raw)) continue;
      const p = String(raw).trim();
      out.push({ kind: key, path: p, present: existsSync(resolve(repoRoot, p)) });
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
      // T5（260924-gov-review-gate-parse-teeth）：missing 态补 advisory——与 bare 态同通道，
      // 引用腐坏（验收文件/冻结件/文档已迁移或归档）在 brief 生成期即报警，不再只在行内可见。
      advisories.push(`ref 指向的文件不在磁盘上：${r.ref}（引用腐坏——验收文件/冻结件/文档可能已迁移或归档，起包期核对 intake 四问）`);
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
// T1（260920-gov-review-reader-scripts-align，FROZEN 2026-09-20）：第三判定源——
// ESCALATE + 人批形态（本仓主流）：`human-decision` 事件 + `payload.decision` 含 `approve-escalated-merge`。
// 该形态源①（认 APPROVE）与源②（认 merge 事件名）皆不认；起草期实测为 **7 个存量包**的净缺口。
// `dir` 参数为自检注入用（默认 projectDir；不改既有调用方）。
function isMerged(changeId, dir = projectDir) {
  const verdictPath = resolve(dir, 'delivery/taskpackages', changeId, 'supervisor-verdict.json');
  if (existsSync(verdictPath)) {
    try {
      const v = JSON.parse(readFileSync(verdictPath, 'utf8').replace(/^﻿/, ''));
      const verdict = v.verdict || (v.decision && v.decision.verdict) || (v.result && v.result.verdict);
      if (String(verdict).toUpperCase() === 'APPROVE') return true;
    } catch { /* 非法 JSON 视作未判 */ }
  }
  const events = resolve(dir, 'delivery/taskpackages', changeId, 'pod-events.jsonl');
  if (existsSync(events)) {
    for (const line of readFileSync(events, 'utf8').split(/\r?\n/)) {
      if (!line.trim()) continue;
      if (/"eventType"\s*:\s*"merge-(completed|merged)"/.test(line)) return true;
      if (/"eventType"\s*:\s*"[^"]*merge[^"]*"/.test(line)) return true;
      if (/"eventType"\s*:\s*"human-decision"/.test(line) && /"decision"\s*:\s*"[^"]*approve-escalated-merge/.test(line)) return true;
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

// ⑤ 跨包承接面（260920-gov-review-bearing-strength-align T1）：承接包判定 = `carriedBy ?? owner`；
// 行为名读侧兼容 `topic` ∨ `behavior`；两写面扫描，`_archive/` 与自包登记（指回本包自己的承接）
// 不算「上游承接」故排除。解析失败**不静默**：以 notes 行呈现（可见、非致命——不影响本包其它路）。
function readJsonOrNull(path) {
  try { return JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, '')); } catch { return null; }
}

function cellText(v) {
  return String(v == null ? '' : v).replace(/\r?\n/g, ' ').replace(/\|/g, '\\|');
}

function collectCrossPackageCarry(tp, repoRoot = projectDir) {
  const rows = [];
  const notes = [];
  const selfId = tp.changeId;
  const push = (source, entry) => {
    if (!entry || typeof entry !== 'object') return;
    const carry = nonEmptyString(entry.carriedBy) ? entry.carriedBy : entry.owner;
    if (carry !== selfId) return;
    const behavior = nonEmptyString(entry.topic) ? entry.topic
      : (nonEmptyString(entry.behavior) ? entry.behavior : '(未命名行为)');
    rows.push({
      source,
      behavior,
      owner: nonEmptyString(entry.owner) ? entry.owner : '(缺省)',
      carriedBy: nonEmptyString(entry.carriedBy) ? entry.carriedBy : '(缺省=owner)',
      note: nonEmptyString(entry.note) ? entry.note : '—',
    });
  };
  // 写面一：包面（自包排除——本条路要送的是「上游交给我」的项，不是自登记）
  const pkBase = join(repoRoot, 'delivery', 'taskpackages');
  if (existsSync(pkBase)) {
    for (const dir of readdirSync(pkBase, { withFileTypes: true })) {
      if (!dir.isDirectory() || dir.name === 'current' || dir.name === selfId) continue;
      const p = join(pkBase, dir.name, 'taskpackage.json');
      if (!existsSync(p)) continue;
      const other = readJsonOrNull(p);
      if (!other) { notes.push(`> ⚠ 无法解析 \`delivery/taskpackages/${dir.name}/taskpackage.json\`（该写面本次未纳入）`); continue; }
      for (const e of Array.isArray(other.crossPackageBehaviors) ? other.crossPackageBehaviors : []) push(`package:${dir.name}`, e);
    }
  }
  // 写面二：workstream 面（`_archive/` 排除——归档不派单）
  const wsBase = join(repoRoot, 'delivery', 'workstreams');
  if (existsSync(wsBase)) {
    for (const dir of readdirSync(wsBase, { withFileTypes: true })) {
      if (!dir.isDirectory() || dir.name.startsWith('_')) continue;
      const p = join(wsBase, dir.name, 'decision-coverage.json');
      if (!existsSync(p)) continue;
      const dc = readJsonOrNull(p);
      if (!dc) { notes.push(`> ⚠ 无法解析 \`delivery/workstreams/${dir.name}/decision-coverage.json\`（该写面本次未纳入）`); continue; }
      for (const e of Array.isArray(dc.crossPackageBehaviors) ? dc.crossPackageBehaviors : []) push(`workstream:${dir.name}`, e);
    }
  }
  return { rows, notes };
}

// bug 类来源闭集（261015-gov-review-lite-fast-path T4）：命中且 planRef 在场 → ①b 指针形态 + 头部改动指令源行
const BUG_SOURCE_TYPES = ['regression', 'ci-failure', 'drift-report'];

function renderBrief(tp, opts = {}) {
  const errors = [];
  const advisories = [];
  const repoRoot = opts.repoRoot || projectDir;
  const now = new Date().toISOString();
  const bugDowngrade = BUG_SOURCE_TYPES.includes(tp.sourceType) && nonEmptyString(tp.planRef);
  const anchorRows = renderAnchorSections(collectRefs(tp).filter((r) => !isStoryRef(r.path)), advisories);
  const storyRoad = renderStoryRoad(collectRefs(tp).filter((r) => isStoryRef(r.path)), advisories, { bugDowngrade });
  const ce = renderCounterparts(tp, errors);
  const precedents = findPrecedents(tp);
  const carry = collectCrossPackageCarry(tp, repoRoot);
  const warn = wireAdvisory(tp, ce);

  const L = [];
  L.push(`# input-brief · ${tp.changeId}`);
  L.push('');
  L.push(`> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。`);
  L.push(`> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 \`brief-inputs.mjs\` 取新行号，不得照旧行号通读整文件。`);
  L.push(`> generated-at: ${now} · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）`);
  if (nonEmptyString(opts.regeneratedBecause)) {
    L.push(`> regenerated-because: ${String(opts.regeneratedBecause).trim()}（覆盖上一份快照，旧 generated-at 见 git diff 上一版本）`);
  }
  L.push(`> 来源 TaskPackage: \`delivery/taskpackages/${tp.changeId}/taskpackage.json\` · subsystem: ${tp.subsystem || '(空)'} · rigor: ${tp.rigor || 'full'}`);
  if (bugDowngrade) {
    L.push(`> **改动指令源 = plan.md**：\`${tp.planRef}\`（bug 类包——本 brief ①b 为指针形态，真实改动指令在此）。`);
  }
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
  if (storyRoad) {
    for (const l of storyRoad) L.push(l);
    L.push('');
  }
  // ①c（260929-gov-review-req-loop-ui-author T5.3 立；261015-gov-review-lite-fast-path T4 单份化：builder 为
  // 主要消费方，acceptance-author 直读 TaskPackage uiux block 的既有口径不变）——refs 入清单即入视野
  const uiuxRefs = collectUiuxRefs(tp, repoRoot);
  for (const d of uiuxRefs.filter((r) => !r.present)) {
    advisories.push(`uiux 设计 ref 指向的文件不在盘上：${d.path}（引用腐坏——核对 uiux block 或设计母版是否已迁移）`);
  }
  L.push(`## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）`);
  L.push('');
  if (uiuxRefs.some((r) => r.present)) {
    L.push('| 类别 | 路径 |');
    L.push('|---|---|');
    for (const r of uiuxRefs.filter((r) => r.present)) L.push(`| ${r.kind} | \`${r.path}\` |`);
    L.push('');
    L.push('页级 PNG（400×300）= **布局坐标定位权威**：layout 常量按图定，文字线框仅口径补充。');
  } else {
    L.push('`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。');
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
  L.push('## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）');
  L.push('');
  if (carry.notes.length > 0) {
    for (const n of carry.notes) L.push(n);
    L.push('');
  }
  if (carry.rows.length === 0) {
    L.push('`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。');
    L.push('');
    L.push('`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。');
  } else {
    L.push('| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |');
    L.push('|---|---|---|---|---|');
    for (const r of carry.rows) {
      L.push(`| \`${cellText(r.source)}\` | ${cellText(r.behavior)} | \`${cellText(r.owner)}\` | \`${cellText(r.carriedBy)}\` | ${cellText(r.note)} |`);
    }
    L.push('');
    L.push('上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。');
  }
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

  const a = renderBrief(fixtureTp);
  ok('① 锚点节形状', /\|\s*路径\s*\|\s*锚点\s*\|\s*节标题\s*\|\s*节首行\s*\|\s*状态\s*\|/.test(a.markdown)
      && a.markdown.includes(CONTRACT_WIRE_FILE), 'table header + contract path');
  ok('① 不抽正文', !/```(py|c|cpp|json)/.test(a.markdown), 'no fenced source bodies');
  ok('① 裸路径标「无锚点（整文件）」', a.markdown.includes('无锚点（整文件）')
      && a.advisories.some((s) => s.includes('无锚点')), 'DEV-GOVERNANCE.md bare ref → bare marker + advisory (R1/F12)');
  // T5（260924-gov-review-gate-parse-teeth）判别：缺失文件 → missing 行 + advisory（反转实现必须变红）。
  const miss = renderBrief({ ...fixtureTp, docRefs: [...fixtureTp.docRefs, 'docs/nonexistent-brief-inputs-probe.md#§9'] });
  ok('① 缺失文件 → missing 行 + advisory（T5）', miss.markdown.includes('（文件不存在）')
      && miss.advisories.some((s) => s.includes('不在磁盘上')), 'missing ref → missing row + advisory');
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

  // ①c uiux 设计 refs 透传（260929-gov-review-req-loop-ui-author T5.3）：两臂 + builder 专列 + 悬空 advisory
  const uiuxTp = { ...fixtureTp, uiux: { uiuxRequired: true, uiuxLevel: 2, designRefs: ['docs/ui/UIUX-DESIGN-BRIEF.md'], sharedUiRefs: [] } };
  const ub = renderBrief(uiuxTp);
  ok('uiux-refs 在位列（brief 含设计 ref 与定位权威句）', ub.markdown.includes('## ①c · UI 设计 refs')
      && ub.markdown.includes('docs/ui/UIUX-DESIGN-BRIEF.md')
      && ub.markdown.includes('布局坐标定位权威'), 'designRef listed with authority note');
  ok('uiux-refs ①c 主要消费方标注（单份共用、builder 主读）', renderBrief(uiuxTp).markdown.includes('本节主要消费方 = builder'),
      'single-file ①c notes builder as main consumer');
  ok('uiux-refs 缺席 → none 结论（不脑补）', renderBrief(fixtureTp).markdown.includes('`none` —— uiux block 不在场'), 'absent arm renders none');
  const uDangle = renderBrief({ ...fixtureTp, uiux: { uiuxRequired: true, uiuxLevel: 2, designRefs: ['docs/ui/no-such-page-probe.png'], sharedUiRefs: [] } });
  ok('uiux-refs 悬空 ref → advisory（在位才列）', uDangle.advisories.some((s) => s.includes('uiux 设计 ref 指向的文件不在盘上'))
      && !uDangle.markdown.includes('docs/ui/no-such-page-probe.png'), 'dangling ref excluded + advisory');
  ok('uiux-refs 夹具非空（设计 ref 指向的母版在盘）', existsSync(resolve(projectDir, 'docs/ui/UIUX-DESIGN-BRIEF.md')), 'fixture ref targets real file');

  // fail-loud 路径（§8-B1）
  const badTp = { ...fixtureTp, crossEnd: { counterparts: [{ path: 'firmware/main/nonexistent_xxx.cc', symbol: 'NoSuchSymbol' }] } };
  const bad = renderBrief(badTp);
  ok('② 解析失败 fail-loud', bad.errors.some((e) => e.kind === 'counterpart-unresolved' && e.entry.includes('NoSuchSymbol')),
      'errors carry named entry, not silent empty anchor');
  const okTp = { ...fixtureTp, crossEnd: { counterparts: [{ path: 'firmware/main/mcp_server.cc', symbol: 'McpServer' }] } };
  const good = renderBrief(okTp);
  ok('② 自有面正例可解析', good.errors.length === 0 && good.markdown.includes('firmware/main/mcp_server.cc'),
      'resolved range rendered');
  const offTp = { ...fixtureTp, crossEnd: { counterparts: [{ path: 'ESP32-S3-RLCD-4.2-Demo/x.cc', symbol: 'Y' }] } };
  const off = renderBrief(offTp);
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

  // T1（260920-gov-review-reader-scripts-align，FROZEN 2026-09-20）：isMerged 第三判定源双夹具——
  // ESCALATE + 人批形态的 decoy merged 包**须入选**、未 merged 包**须不入选**（R1-F1 教训：夹具须真实 fs、
  // 断言前先断夹具非空）。isMerged 带 dir 参数（自检注入用），故 temp dir 可行。
  const imDir = mkdtempSync(join(tmpdir(), 'brief-ism-'));
  const imMergedId = '260901-merged-decoy';
  const imOpenId = '260901-open-decoy';
  mkdirSync(join(imDir, 'delivery', 'taskpackages', imMergedId), { recursive: true });
  mkdirSync(join(imDir, 'delivery', 'taskpackages', imOpenId), { recursive: true });
  writeFileSync(join(imDir, 'delivery', 'taskpackages', imMergedId, 'pod-events.jsonl'),
    JSON.stringify({ schemaVersion: 1, changeId: imMergedId, eventType: 'human-decision', payload: { decision: 'approve-escalated-merge' } }) + '\n', 'utf8');
  writeFileSync(join(imDir, 'delivery', 'taskpackages', imOpenId, 'pod-events.jsonl'),
    JSON.stringify({ schemaVersion: 1, changeId: imOpenId, eventType: 'human-decision', payload: { decision: 'some-other-decision' } }) + '\n', 'utf8');
  ok('isMerged 夹具非空（两 decoy 的 pod-events 均在盘）',
      existsSync(join(imDir, 'delivery', 'taskpackages', imMergedId, 'pod-events.jsonl'))
      && existsSync(join(imDir, 'delivery', 'taskpackages', imOpenId, 'pod-events.jsonl')));
  ok('isMerged ESCALATE+人批 decoy 须入选（第三判定源）', isMerged(imMergedId, imDir) === true);
  ok('isMerged 其他 human-decision 不入选（decision 词面不含 approve-escalated-merge）', isMerged(imOpenId, imDir) === false);
  ok('isMerged 无事件文件 → false', isMerged('260901-absent-decoy', imDir) === false);
  rmSync(imDir, { recursive: true, force: true });

  // ⑤ 跨包承接面（260920-gov-review-bearing-strength-align T1，FROZEN 2026-09-20）：两写面 + 承接包判定
  // `carriedBy ?? owner` + 零命中 none + 归档/自包/反向 decoy 排除。夹具走 temp repo（repoRoot 注入）。
  const cpbDir = mkdtempSync(join(tmpdir(), 'brief-cpb-'));
  const cpbSelf = '260930-carry-self';
  const cpbOther = '260930-carry-other';
  mkdirSync(join(cpbDir, 'delivery', 'taskpackages', cpbSelf), { recursive: true });
  mkdirSync(join(cpbDir, 'delivery', 'taskpackages', cpbOther), { recursive: true });
  mkdirSync(join(cpbDir, 'delivery', 'workstreams', 'probe-ws'), { recursive: true });
  mkdirSync(join(cpbDir, 'delivery', 'workstreams', '_archive', '2026', 'old'), { recursive: true });
  mkdirSync(join(cpbDir, 'delivery', 'taskpackages', 'current'), { recursive: true });
  // 写面一 · 包面：存量键 `behavior` + `owner` 直指承接包（无 carriedBy）
  writeFileSync(join(cpbDir, 'delivery', 'taskpackages', cpbOther, 'taskpackage.json'), JSON.stringify({
    changeId: cpbOther,
    crossPackageBehaviors: [
      { behavior: '包面行为甲（owner 直指本包）', owner: cpbSelf, note: '存量键形态' },
      { behavior: '包面 decoy', owner: cpbOther, note: '承接包是别包，不得出现' },
    ],
  }), 'utf8');
  // 自包登记（owner/carriedBy 均指自己）→ 不算「上游承接」、不得出现在自己的 brief 里
  writeFileSync(join(cpbDir, 'delivery', 'taskpackages', cpbSelf, 'taskpackage.json'), JSON.stringify({
    changeId: cpbSelf,
    crossPackageBehaviors: [{ behavior: '自包登记 decoy', owner: cpbSelf, note: '自包不出现在自己的承接面' }],
  }), 'utf8');
  // 写面二 · workstream 面：canonical 四键，`owner ≠ carriedBy` 且承接包 = 本包
  writeFileSync(join(cpbDir, 'delivery', 'workstreams', 'probe-ws', 'decision-coverage.json'), JSON.stringify({
    crossPackageBehaviors: [
      { topic: 'workstream 行为乙（语义 owner ≠ 承接包）', owner: '260930-owner-k1', note: '活语义', carriedBy: cpbSelf },
      { topic: 'workstream decoy（owner 指本包但 carriedBy 是别包）', owner: cpbSelf, note: '不得出现——按 owner 单键判会误纳', carriedBy: '260930-owner-k1' },
    ],
  }), 'utf8');
  // 归档面（`_archive/`）→ 不派单、不得出现
  writeFileSync(join(cpbDir, 'delivery', 'workstreams', '_archive', '2026', 'old', 'decision-coverage.json'), JSON.stringify({
    crossPackageBehaviors: [{ topic: '归档 decoy', owner: 'x', note: '归档不派单', carriedBy: cpbSelf }],
  }), 'utf8');
  ok('⑤ 夹具非空（两写面 + 自包 + 归档四面均在盘）',
      existsSync(join(cpbDir, 'delivery', 'taskpackages', cpbOther, 'taskpackage.json'))
      && existsSync(join(cpbDir, 'delivery', 'taskpackages', cpbSelf, 'taskpackage.json'))
      && existsSync(join(cpbDir, 'delivery', 'workstreams', 'probe-ws', 'decision-coverage.json'))
      && existsSync(join(cpbDir, 'delivery', 'workstreams', '_archive', '2026', 'old', 'decision-coverage.json')));
  const cpbTp = { changeId: cpbSelf, subsystem: 'firmware', docRefs: [], sourceRefs: [], contractRefs: [], crossEnd: { counterparts: [], exempt: '' } };
  const cpb = renderBrief(cpbTp, { repoRoot: cpbDir });
  ok('⑤ 命中·包面（`behavior` 存量键 + `owner` 判承接）',
      cpb.markdown.includes('## ⑤ 跨包承接面') && cpb.markdown.includes('包面行为甲（owner 直指本包）')
      && cpb.markdown.includes(`package:${cpbOther}`));
  ok('⑤ 命中·workstream 面（按 `carriedBy` 判承接，`owner` 是别包）',
      cpb.markdown.includes(`workstream:probe-ws`) && cpb.markdown.includes('workstream 行为乙（语义 owner ≠ 承接包）')
      && cpb.markdown.includes('260930-owner-k1'));
  ok('⑤ 排除（承接包是别包 / `owner` 指本包但 `carriedBy` 是别包 / 自包登记 / 归档面）',
      !cpb.markdown.includes('包面 decoy') && !cpb.markdown.includes('workstream decoy')
      && !cpb.markdown.includes('自包登记 decoy') && !cpb.markdown.includes('归档 decoy'));
  const cpbNone = renderBrief({ ...cpbTp, changeId: '260930-nothing-carry' }, { repoRoot: cpbDir });
  ok('⑤ 零命中明写 `none`（不是省略）', cpbNone.markdown.includes('`none` —— 两写面'));
  rmSync(cpbDir, { recursive: true, force: true });

  // ①b story 照录路（260928-gov-review-portfolio-boundary-story-propagation T2）：整节照录 + AC
  // 复合句法加亮 + not-found advisory（不阻断、brief 仍产出，R2-F2 语义 = advisory 非 fail-loud）。
  ok('①b 夹具非空（docs/requirements.md 在盘且含 S-F1.4 节）', (() => {
    const p = join(projectDir, REQUIREMENTS_FILE);
    return existsSync(p) && readFileSync(p, 'utf8').includes('##### S-F1.4 · ');
  })());
  const storyTp = { ...fixtureTp, sourceRefs: [...fixtureTp.sourceRefs, 'S-F1.4', 'S-F1.4-AC3'] };
  const story = renderBrief(storyTp);
  ok('①b story ref 整节照录（节标题 + AC1 在投影内）', story.markdown.includes('## ①b · story 语义权威')
      && story.markdown.includes('日程提醒') && /AC1[：:]/.test(story.markdown), 'verbatim section excerpted');
  ok('①b story ref 不再落 ① 路 missing 表', !story.markdown.includes('（文件不存在）')
      && !/\| `S-F1\.4` \|/.test(story.markdown), 'routed out of anchor table (ARQ-260927 证据 5 缺口闭合)');
  ok('①b AC 复合句法加亮 + 照录语义声明', story.markdown.includes('`S-F1.4-AC3`')
      && story.markdown.includes('语义权威仍在 requirements 现文'), 'AC tag + authority note');
  const storyMiss = renderBrief({ ...fixtureTp, sourceRefs: [...fixtureTp.sourceRefs, 'S-F9.9'] });
  ok('①b not-found → advisory、不阻断（brief 仍产出）', storyMiss.markdown.includes('## ①b · story 语义权威')
      && storyMiss.advisories.some((s) => s.includes('story ref 未定位到节'))
      && storyMiss.errors.length === 0, 'advisory channel, no exit-1 class error');
  // T4（261015-gov-review-lite-fast-path）：基 id 去重 + bug 类降级 + proposal 照录不变 + 单份头部
  ok('①b 基 id 去重（同基两引用只贴一次、AC 标签并列）', (() => {
    const hits = (story.markdown.match(/### S-F1\.4 · 源 =/g) || []).length;
    return hits === 1 && story.markdown.includes('`S-F1.4`、`S-F1.4-AC3`');
  })(), 'one section per base id, merged AC tags');
  const bugTp = { ...fixtureTp, sourceType: 'regression', planRef: 'delivery/taskpackages/x/plan.md', sourceRefs: [...fixtureTp.sourceRefs, 'S-F1.4', 'S-F1.4-AC3'] };
  const bug = renderBrief(bugTp);
  ok('①b bug 类降级（指针形态 + 头部指令源行、不照录整节）', bug.markdown.includes('## ①b · story 语义权威（bug 类包 · 指针形态')
      && bug.markdown.includes('**改动指令源 = plan.md**')
      && bug.markdown.includes('不照录整节')
      && !bug.markdown.includes('> **语义权威仍在 requirements 现文**'), 'pointer form, no verbatim section');
  const propTp = { ...fixtureTp, sourceType: 'proposal', planRef: 'delivery/review/x/PROPOSAL.md', sourceRefs: [...fixtureTp.sourceRefs, 'S-F1.4'] }; // 261015 批 T1 顺修：原 planRef 指向已废止的 dev lane 旧址（原文见 FROZEN PROPOSAL T1 改法 4）；波段二随 T6 定稿为本行新树形态
  ok('①b proposal 来源照录不变（负向臂）', renderBrief(propTp).markdown.includes('> **语义权威仍在 requirements 现文**'), 'verbatim kept for proposal origin');
  ok('单份头部形态（消费方并列、无 role 残留）', a.markdown.includes('消费方: acceptance-author + builder')
      && !a.markdown.includes('input-brief · acceptance-author'), 'single-file header');
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

  const dir = resolve(projectDir, 'delivery/taskpackages', args.changeId);
  const tpPath = join(dir, 'taskpackage.json');
  if (!existsSync(tpPath)) fail('TaskPackage not found', { expected: tpPath });
  let tp;
  try { tp = JSON.parse(readFileSync(tpPath, 'utf8').replace(/^﻿/, '')); }
  catch (e) { fail('TaskPackage is not valid JSON', { tpPath, error: String(e.message || e) }); }

  const { markdown, errors, advisories, warnings } = renderBrief(tp, { regeneratedBecause: args.regeneratedBecause });

  for (const w of warnings) console.error(`brief-inputs: WARN(advisory) — ${w}`);
  for (const adv of advisories) console.error(`brief-inputs: ADV — ${adv}`);
  for (const adv of advisoriesGlobal) console.error(`brief-inputs: ADV — ${adv}`);

  if (errors.length > 0) {
    // fail-loud：不产出带空锚的 brief，避免子 agent 按「清单已封闭」误信
    console.error(`brief-inputs: ${errors.length} 条对端条目解析失败，不产出 brief：`);
    for (const e of errors) console.error(`  · ${e.kind}: ${e.entry}${e.reason ? ` (${e.reason})` : ''} — ${e.hint || '请修正 crossEnd 后重跑'}`);
    process.exit(1);
  }

  const outPath = join(dir, 'input-brief.md');
  if (args.dryRun) {
    process.stdout.write(markdown);
    return;
  }
  writeFileSync(outPath, markdown, 'utf8');
  console.log(JSON.stringify({ ok: true, script: 'brief-inputs', changeId: args.changeId, brief: outPath, warnings: warnings.length, advisories: advisories.length }, null, 2));
}

if (await isMainScript()) main();

// Windows 下 process.argv[1] 是相对/反斜杠形态，直接比 import.meta.url 会恒假（实测首跑静默退出）
async function isMainScript() {
  const { pathToFileURL } = await import('node:url');
  const entry = process.argv[1];
  return !!entry && import.meta.url === pathToFileURL(resolve(entry)).href;
}
