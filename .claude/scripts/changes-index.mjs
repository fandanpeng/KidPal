#!/usr/bin/env node
// delivery/{review,intake,taskpackages}/INDEX.md 幂等全量重建 + 单行 upsert（G3，对偶 .claude/scripts/planning-index.mjs）。
// 扫描 delivery/taskpackages/*/ + delivery/review/*/ + delivery/intake/<changeId>.md，收 origin.ref/origin.workstream/
// changeId/日期/状态，重建三份 lane INDEX（260915-gov-review-index-split lane 分文件；261015-gov-review-changes-taxonomy
// T5/T6：arch lane 迁 review、新增 intake lane、扫描根随新树）：
//   delivery/taskpackages/INDEX.md（dev 段）+ delivery/review/INDEX.md（review 段，旧 changes/arch）+
//   delivery/intake/INDEX.md（intake 段，spine 状态投影 + 漂移检测标记——T5，检测不拦截）。
// 幂等——每次同输入产同输出；INDEX 头标「勿手改」。
//
// 用法：
//   node .claude/scripts/changes-index.mjs                    # 幂等全量重建（定期纠偏/手改后修复）
//   node .claude/scripts/changes-index.mjs --upsert <id>      # review 单行 upsert（状态转移时 agent 主动跑，只动一行）
//   node .claude/scripts/changes-index.mjs --backfill         # 一次性回填（日期/已合main/landed，零 git fork 读缓存）
//   node .claude/scripts/changes-index.mjs --stale-report     # 陈旧批次观察面
//   node .claude/scripts/changes-index.mjs --self-check       # 结构化自检（ARQ-114 先例）
// 由 closeout-check 增量刷 dev INDEX 行（互补——全量重建是手改/漂移的纠偏手段）。
// 本脚本不写 index.jsonl。
//
// review 状态机（260823-gov-review-arq-queue-restructure，Track B R2 GREEN；261015 批 T6 随新树）：
//   draft → frozen → landed（终态）。frozen 复用 lib isFrozen（状态行非全文）；
//   landed = landing-verdict.md 在场且最终 verdict ∈ {PASS, GREEN}；已合 main = 目录**末提交**被本地 main 包含
//   （260909-gov-review-gate-truthfulness T6：旧判据用首提交，draft 入册即被误报 mergedInMain=yes）。
//   状态全 immutable/单调收敛 → 一次算完钉死；已算结果缓存进 .changes-index-state.json 供零 fork 复用。
//
// intake 状态机（260928-gov-review-cross-face-intake-spine 立，change-intake.md §2.7 值域；T5 投影齿）：
//   intake → in-flight → awaiting-human:<gate> / awaiting-upstream / awaiting-human:<openQ> → closed。
//   漂移检测两规则（human 裁定②：清单齿 + INDEX 漂移标，检测不拦截；复议触发 = 下轮 loop-retro 仍见漂移标）：
//   ① anchors 命中面全回填但 status ≠ closed → ⚠未闭环；② awaiting-* 滞留超 7 天 → ⚠滞留Nd
//   （滞留天数代理 = 今天 − spine 文件末提交日；阈值酌定理由与重估条款见 PROPOSAL T5 第 2 条 / §6）。

import { existsSync, readFileSync, readdirSync, writeFileSync, mkdirSync, rmSync } from 'node:fs';
import { resolve, join, basename } from 'node:path';
import { execSync, spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { resolveOriginRef, isFrozen, closeoutRequiredFiles, VERDICT_STATUS_RE, VERDICT_HEADING_RE } from './lib/governance-gate-shared.mjs';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// 状态缓存（一次性钉死结果），供零 fork 复用。tracked，幂等。
// 261015-gov-review-changes-taxonomy T6 第 4 条：随 review 新树迁址（旧址随伞整体归档）。
const STATE_CACHE_PATH = resolve(projectDir, 'delivery', 'review', '.changes-index-state.json');

// 人工 adjudication（landing 期一次性判定，immutable）：机械强信号判不出的发散旧批 + 进行中批。
// 每个值 = true（人工核 verdict 文件确认已落地）。判据记录于对应批目录（landing 期 verdict 人工判定清单；261015 批 T6 前旧址 delivery/review/，随伞归档 delivery/archive/changes/arch/260823-gov-review-arq-queue-restructure/）。
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
  // landing 期补判 · 260920 批（260920-gov-review-verdict-anchor-align T4，human 2026-09-20 OQ2 裁
  // 「本批落 + 注明依据」）——六批的 verdict 行形态**各自不同**、均不被 deriveLanded 的锚识别
  // （六成因见该批 §2 T4 证据段）；逐条注明「依据 = 该批 landing-verdict 的 verdict 行 verbatim」，
  // 供事后抽查。六批的目录末提交均已在 main（该批起草期实核）：
  '260907-gov-review-architect-dispatch-model-override': true, // landing-verdict `:3` = `**verdict：APPROVE**`（token 不在集）
  '260908-gov-review-escalation-conflict-ladder': true,        // `:95` = `### 7.0 Verdict：**GREEN（放行建议…）**`（点分编号 7.0 不被认）
  '260908-gov-review-merge-chain-hardening': true,             // `:5` = `## 0. Verdict：**RED（…）**`；`:164` = 散文「五前提全部满足…全绿」（转绿无 token 行）
  '260907-gov-review-merge-friction': true,                    // `:98` = `**绿（GREEN）**——…`（无标签）
  '260904-gov-review-escalate-merge-approval': true,           // `:77` = `**最终 verdict：GREEN。**`（`最终` 挡在标签前）
  '260830-arch-review-ui-impl-strategy': true,                 // `:9` = `**绿（放行建议）——…**`（无标签）
  // 第 7 条（**起草期 census 遗漏补登**，2026-09-20 落地期现跑发现）：该批的「已合main」在 T4 起草 census 时
  // 尚为空（后由 backfill 重算为 yes），故未入原 6 条清单；实况 = 已落地（复核轮 PASS）：
  '260914-arch-review-kidpal-session-ctx-contract': true,      // `:92` = `**LANDING VERDICT: PASS**`（label 为 `LANDING VERDICT` 非 `verdict`，行首判据不认）
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

// 目录末提交（260909 T6）：mergedInMain 的语义 =「该批全部制品已并入 main」，须看末提交而非首提交。
function gitLastCommitSha(relPath) {
  try {
    return execSync(`git log -1 --format=%H -- "${relPath}"`, {
      cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
    }).trim();
  } catch {
    return '';
  }
}

// mergedInMain 派生单源（260909 T6，upsertArchRow 与 backfill 两路同调——防派生判据双源分叉）。
function dirMergedInMain(relPath) {
  return gitContainedInMain(gitLastCommitSha(relPath));
}

function gitCommitDate(sha) {
  try {
    return execSync(`git show -s --format=%cs ${sha}`, { cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim();
  } catch {
    return '';
  }
}

// 已合 main（immutable）：目录**末提交**被本地 main 包含（260909 T6 前为首提交，语义 =「全部制品已并入」）。
// 前提 = 合 main 动作完成后跑一次（R1-F3）；本地 main 假定单调不回退。
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

// dev 状态推导（260906-gov-review-index-status-truth-source T2，替代 R5 判据）：
//   closed = closeoutRequiredFiles(changeId) 全在场（与 closeout-check 承重闸同 predicate，lib 单源）。
//   R5 判据（verification.json/verdict.json 在场）作废——该两件按 closeout-hardening SSOT 为 advisory、永不晋升包目录，
//   对全包恒 false（ARQ-260906-index-rebuild-dev-status-drift 病灶）；sv 值/在场均不可作判据（§1 证据④⑤）。
export function deriveDevClosed(dir, changeId) {
  return closeoutRequiredFiles(changeId).every((f) => existsSync(join(dir, f)));
}
// collectDev 内：const status = deriveDevClosed(dir, name.name) ? 'closed' : 'open';

// 收 dev dirs（delivery/taskpackages/*/ 子目录，changeId 形状，可能含 closeout 制品）。
function collectDev() {
  const root = resolve(projectDir, 'delivery', 'taskpackages');
  const items = [];
  if (!existsSync(root)) return items;
  for (const name of readdirSync(root, { withFileTypes: true })) {
    if (!name.isDirectory()) continue;
    if (name.name === 'current') continue; // 排除 current/ 活工作面（261014-gov-review-dev-lane-merge-taskpackages T2：扫描根迁 taskpackages 后的连带新语义）
    const dir = join(root, name.name);
    const tp = join(dir, 'taskpackage.json');
    const tpFallback = resolve(projectDir, 'delivery', 'taskpackages', name.name, 'taskpackage.json');
    let originRef = '', originWorkstream = '';
    const tpPath = existsSync(tp) ? tp : (existsSync(tpFallback) ? tpFallback : null);
    if (tpPath) {
      try {
        const o = readJson(tpPath).origin || {};
        originRef = resolveOriginRef(o) || '';
        originWorkstream = o.workstream || '';
      } catch { /* 非 JSON 或缺 origin */ }
    }
    const date = parseChangeIdDate(name.name) || gitFirstCommitDate(`delivery/taskpackages/${name.name}`);
    const status = deriveDevClosed(dir, name.name) ? 'closed' : 'open';
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
// VERDICT_STATUS_RE / VERDICT_HEADING_RE 已迁 lib 单源（260920-gov-review-verdict-anchor-align T2——
// 升级闸 readGovAnchors 的 green 锚共用同一判据，防双源分叉；import 在上）。
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
      // JSON 行锚（260906-gov-review-index-status-truth-source T2②）：机器 JSON 块 "verdict": "TOKEN" 是 verifier 程序化产出、
      // 行文漂移度最低的第二锚；行末取（后出者胜），与既有锚冲突时以机器锚为终。
      const j = /"verdict"\s*:\s*"(PASS|GREEN|RED|FAIL|BLOCK)"/.exec(line);
      if (j) { finalToken = j[1].toUpperCase(); pendingHeading = false; }
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

// 单目录 review 状态计算（供 collectReview 全量与 --upsert 单行复用）。cache 提供已钉死的 date/mergedInMain（零 fork）；frozen/landed 从文件现推（廉价、无 fork）。
function computeReviewEntry(name, cache) {
  const dir = join(resolve(projectDir, 'delivery', 'review'), name);
  const propPath = join(dir, 'PROPOSAL.md');
  const cached = cache[name] || {};
  let status = 'draft';
  if (existsSync(propPath)) {
    try { if (isFrozen(readText(propPath))) status = 'frozen'; } catch { /* ignore */ }
  }
  if (deriveLanded(dir, cached)) status = 'landed'; // landed 是终态，覆盖 frozen（① 人工 adjudication 缓存优先）
  const date = cached.date || parseChangeIdDate(name) || gitFirstCommitDate(`delivery/review/${name}`);
  const mergedInMain = cached.mergedInMain !== undefined ? cached.mergedInMain : '';
  return { changeId: name, date, lane: 'review', originRef: '', status, workstream: '', mergedInMain };
}

// 收 review dirs（delivery/review/*/ 子目录，读 PROPOSAL.md 状态行 + landing-verdict.md 取状态）。
function collectReview(cache = {}) {
  const root = resolve(projectDir, 'delivery', 'review');
  const items = [];
  if (!existsSync(root)) return items;
  for (const name of readdirSync(root, { withFileTypes: true })) {
    if (!name.isDirectory()) continue;
    items.push(computeReviewEntry(name.name, cache));
  }
  return items;
}

// ── stale-report：机械 census（只读、永不删除、永不改写）──
// 遍历 delivery/review/*/，deriveLanded（复用单源 + cache）+ gitFirstCommitDate → staleCandidate 过滤，
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
  const root = resolve(projectDir, 'delivery', 'review');
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
    const firstCommit = gitFirstCommitDate(`delivery/review/${name.name}`);
    if (staleCandidate({ landed, firstCommit, thresholdDays, now })) {
      stale.push({ dir: name.name, firstCommit, landed, fileCount });
    }
  }
  return { ok: true, mode: 'stale-report', thresholdDays, stale };
}

// ── intake lane（261015 批 T5）：spine frontmatter 解析 + 漂移检测（检测不拦截）──
// spine 文件形态 = delivery/intake/<changeId>.md（companion 设计稿 <changeId>.design.md 非 spine，T4 命名规约）。
// faceMap 命中面探测双形态：结构化子键（`  requirements: 动…`）与 inline 散文（「需求面动…；governance 不动」）。
// 滞留天数代理 = 今天 − spine 末提交日（git last commit）；阈值 7 天（human 裁定②，重估条款见 PROPOSAL §6）。
const INTAKE_FACES = [
  { abbr: 'req', keys: ['requirements', '需求面'] },
  { abbr: 'arch', keys: ['architecture', '架构面'] },
  { abbr: 'contract', keys: ['contract', '契约面'] },
  { abbr: 'imp', keys: ['implementation', '实现面'] },
  { abbr: 'gov', keys: ['governance'] },
];
const INTAKE_STATUS_ENUM = /^intake$|^in-flight$|^awaiting-/;

function parseSpine(text) {
  const lines = text.split('\n').map((l) => l.replace(/\r$/, '')); // spine 工作树 CRLF 容忍（autocrlf 检出形态；261015 批 T5 实测六件因 ---\r 漏解析）
  if (lines[0] !== '---') return null;
  let end = -1;
  for (let i = 1; i < lines.length; i++) if (lines[i] === '---') { end = i; break; }
  if (end < 0) return null;
  const fields = {};
  let cur = null;
  for (let i = 1; i < end; i++) {
    const l = lines[i];
    const m = /^([A-Za-z][A-Za-z0-9]*):(.*)$/.exec(l);
    if (m) { cur = m[1]; fields[cur] = (fields[cur] ? `${fields[cur]}\n` : '') + m[2].trim(); }
    else if (cur && l.trim()) { fields[cur] += `\n${l.trim()}`; }
  }
  return fields;
}

// faceMap 命中面（返回缩写数组）。值以「不动」开头 = 缺席；结构化「动/疑似触」与 inline 段不含「不动」均 = 命中。
function faceMapHits(faceMap) {
  if (!faceMap) return [];
  const hits = [];
  for (const def of INTAKE_FACES) {
    const alt = def.keys.join('|');
    const structRe = new RegExp(`(?:^|\\n)\\s*(?:${alt}):([^\\n]*)`, 'i');
    const sm = structRe.exec(`\n${faceMap}`);
    if (sm) {
      if (!/^不动/.test(sm[1].trim())) hits.push(def.abbr);
      continue;
    }
    // inline 散文：关键词出现处到下一面关键词的段不含「不动」= 命中
    const positions = [];
    for (const k of def.keys) {
      let idx = faceMap.indexOf(k);
      while (idx >= 0) { positions.push(idx); idx = faceMap.indexOf(k, idx + 1); }
    }
    if (!positions.length) continue;
    // 全文先收所有面关键词位置（本面段 = 本面首现到最近的其它面关键词）
    const all = [];
    for (const d2 of INTAKE_FACES) for (const k2 of d2.keys) {
      let idx = faceMap.indexOf(k2);
      while (idx >= 0) { all.push(idx); idx = faceMap.indexOf(k2, idx + 1); }
    }
    all.sort((a, b) => a - b);
    const start = Math.min(...positions);
    const next = all.find((p) => p > start);
    const segment = faceMap.slice(start, next === undefined ? undefined : next);
    if (!segment.includes('不动')) hits.push(def.abbr);
  }
  return hits;
}

// anchors 回填进度：锚键非空且不含「待回填」= 已回填。值域 = 键后到下一锚键 / `}` / 行尾的最小者。
function anchorsProgress(anchorsField) {
  if (!anchorsField) return { filled: 0, total: 0 };
  const text = `\n${anchorsField}`;
  const keyRe = /(?:\{|,|^|\n)\s*(req|arch|contract|imp|gov)\s*:/g;
  const found = [];
  let m;
  while ((m = keyRe.exec(text))) found.push({ key: m[1], at: m.index + m[0].length });
  let filled = 0;
  for (let i = 0; i < found.length; i++) {
    const ends = [found[i + 1] ? found[i + 1].at : text.length];
    const nl = text.indexOf('\n', found[i].at); if (nl >= 0) ends.push(nl);
    const br = text.indexOf('}', found[i].at); if (br >= 0) ends.push(br);
    const v = text.slice(found[i].at, Math.min(...ends));
    if (v.trim() && !v.includes('待回填')) filled++;
  }
  return { filled, total: found.length };
}

// 漂移检测两规则（T5 第 2 条，检测不拦截）+ 值域外显示标（第 3 条：不静默归一）。
function intakeDrift({ statusBase, hitCount, filled, lastCommitDate, today, spineOk = true, hasStatus = true }) {
  const marks = [];
  // 261026 批 T5③（ARQ-261025-intake-index-retro-scope-hygiene）：源文件缺口与值域外分开报——
  // frontmatter 未闭合 / 缺 status 字段时，旧行为只落「⚠值域外」（原文语义是 status 写了域外值），
  // 掩盖真实病灶（spine 文件损坏）。缺口型先报缺口、不再叠加值域外；已解析且 status 在场的域外值仍走值域外。
  if (!spineOk) { marks.push('⚠源文件缺口（frontmatter 未闭合）'); return marks; }
  if (!hasStatus) { marks.push('⚠源文件缺口（缺 status 字段）'); return marks; }
  if (!INTAKE_STATUS_ENUM.test(statusBase) && statusBase !== 'closed') marks.push('⚠值域外');
  if (hitCount > 0 && filled >= hitCount && statusBase !== 'closed') marks.push('⚠未闭环');
  if (/^awaiting-/.test(statusBase) && lastCommitDate) {
    const d = Math.floor(daysBetweenIso(lastCommitDate, today));
    if (!Number.isNaN(d) && d > 7) marks.push(`⚠滞留${d}d`);
  }
  return marks;
}

function collectIntake() {
  const root = resolve(projectDir, 'delivery', 'intake');
  const items = [];
  if (!existsSync(root)) return items;
  const today = todayLocalIso();
  for (const name of readdirSync(root, { withFileTypes: true })) {
    if (!name.isFile()) continue;
    if (!/^\d{6}-.+\.md$/.test(name.name) || name.name.endsWith('.design.md')) continue;
    const rel = `delivery/intake/${name.name}`;
    let fields = null;
    try { fields = parseSpine(readText(join(root, name.name))); } catch { fields = null; }
    const changeId = (fields && fields.changeId) || name.name.replace(/\.md$/, '');
    const statusRaw = ((fields && fields.status) || '').split('\n')[0];
    const statusBase = ( /^(intake|in-flight|awaiting-[^\s（(]+|closed)/.exec(statusRaw) || [] )[1] || statusRaw.split(/[\s（(]/)[0] || '—';
    const hits = fields ? faceMapHits(fields.faceMap) : [];
    const prog = fields ? anchorsProgress(fields.anchors) : { filled: 0 };
    const last = gitCommitDate(gitLastCommitSha(rel));
    const drift = intakeDrift({ statusBase, hitCount: hits.length, filled: prog.filled, lastCommitDate: last.slice(0, 10), today, spineOk: fields !== null, hasStatus: statusRaw.trim().length > 0 });
    items.push({
      lane: 'intake', changeId,
      faces: hits.join('+') || '—',
      status: statusBase || '—',
      anchorsProgress: hits.length ? `${prog.filled}/${hits.length}` : '—',
      date: parseChangeIdDate(changeId) || last.slice(0, 10) || '—',
      drift: drift.join(' '),
    });
  }
  return items;
}

// lane INDEX 写出（260915-gov-review-index-split：dev / review / intake 各一份，按 lane 分文件消除跨 lane 合并冲突）。
// 261015 批 T6 第 4 条：review 段落址随新树 delivery/review/INDEX.md（头部指针行指向归档内旧 INDEX 由全量重建注入）；T5：intake 段新立。
function laneIndexPath(lane) {
  if (lane === 'dev') return resolve(projectDir, 'delivery', 'taskpackages', 'INDEX.md');
  if (lane === 'intake') return resolve(projectDir, 'delivery', 'intake', 'INDEX.md');
  return resolve(projectDir, 'delivery', 'review', 'INDEX.md');
}

function indexHeader(lane) {
  const out = [];
  out.push('<!-- AUTO-GENERATED by .claude/scripts/changes-index.mjs — 勿手改（投影，源 = 各包 origin / 目录）。');
  out.push('手改会被全量重建覆盖。INDEX 不是第二事实源，不做拦阻依据。-->');
  out.push('');
  out.push(`# ${lane} changes Index`);
  out.push('');
  return out;
}

function buildDevIndexMd(dev) {
  const out = indexHeader('dev');
  out.push('> 自动生成（changes-index.mjs 幂等重建 / closeout-check 增量刷行 / 状态转移时全量重建收敛）。**勿手改**。');
  out.push('> 列：changeId | 日期 | origin.ref | 状态 | workstream。dev 状态：closed = closeoutRequiredFiles 全在场（lib 单源）。');
  out.push('');
  out.push(`## dev (${dev.length})`);
  out.push('');
  if (dev.length) {
    out.push('| changeId | 日期 | origin.ref | 状态 | workstream |');
    out.push('|---|---|---|---|---|');
    for (const i of dev) {
      out.push(`| [${esc(i.changeId)}](${i.changeId}/) | ${i.date || '—'} | ${esc(i.originRef) || '—'} | ${esc(i.status)} | ${esc(i.workstream) || '—'} |`);
    }
  } else {
    out.push('_（无）_');
  }
  out.push('');
  return out.join('\n');
}

function buildReviewIndexMd(review) {
  const out = indexHeader('review');
  out.push('> 自动生成（changes-index.mjs 幂等重建 / 状态转移时 `--upsert <id>` 单行钉进）。**勿手改**。');
  out.push('> 列：changeId | 日期 | 状态 | 已合main。review 状态机：draft→frozen→landed(终态)。');
  out.push('> 2026-10-15 前历史批见 delivery/archive/changes/arch/INDEX.md（261015-gov-review-changes-taxonomy T6 第 4 条归档指针）。');
  out.push('');
  out.push(`## review (${review.length})`);
  out.push('');
  if (review.length) {
    out.push('| changeId | 日期 | 状态 | 已合main |');
    out.push('|---|---|---|---|');
    for (const i of review) {
      out.push(`| [${esc(i.changeId)}](${i.changeId}/) | ${i.date || '—'} | ${esc(i.status)} | ${esc(i.mergedInMain) || '—'} |`);
    }
  } else {
    out.push('_（无）_');
  }
  out.push('');
  return out.join('\n');
}

function buildIntakeIndexMd(intake) {
  const out = indexHeader('intake');
  out.push('> 自动生成（changes-index.mjs 幂等重建）。**勿手改**。');
  out.push('> 列：changeId | 触面 | status | anchors回填 | 日期 | 漂移标。intake 状态机（change-intake.md §2.7）：intake→in-flight→awaiting-*→closed。');
  out.push('> 漂移标（T5，检测不拦截）：⚠未闭环 = anchors 命中面全回填但 status ≠ closed；⚠滞留Nd = awaiting-* 滞留超 7 天；⚠值域外 = status 出值域（原样显示、不归一）；⚠源文件缺口 = spine frontmatter 未闭合 / 缺 status 字段（261026 批：与值域外分开报，缺口型不叠加值域外）。复议触发 = 下轮 loop-retro 仍见漂移标。');
  out.push('');
  out.push(`## intake (${intake.length})`);
  out.push('');
  if (intake.length) {
    out.push('| changeId | 触面 | status | anchors回填 | 日期 | 漂移标 |');
    out.push('|---|---|---|---|---|---|');
    for (const i of intake) {
      out.push(`| ${esc(i.changeId)} | ${esc(i.faces)} | ${esc(i.status)} | ${esc(i.anchorsProgress)} | ${i.date || '—'} | ${esc(i.drift) || '—'} |`);
    }
  } else {
    out.push('_（无）_');
  }
  out.push('');
  return out.join('\n');
}

function buildIndexMd(items) {
  const dev = items.filter(i => i.lane === 'dev').sort((a, b) => String(b.date || '').localeCompare(String(a.date || '')));
  const review = items.filter(i => i.lane === 'review').sort((a, b) => String(b.date || '').localeCompare(String(a.date || '')));
  const intake = items.filter(i => i.lane === 'intake').sort((a, b) => String(b.date || '').localeCompare(String(a.date || '')));
  return { dev: buildDevIndexMd(dev), review: buildReviewIndexMd(review), intake: buildIntakeIndexMd(intake) };
}

// ── 命名断言（260825-gov-review-track-b-naming-convention T3）：日期前缀 review 目录名须命中写槽 token ──
// 只拦日期前缀新目录（parseChangeIdDate 判定；非法日期前缀自然落空不断言）；旧目录 / 非日期前缀宽容。
// fail-loud 点名只挂 upsert（起草期收口）；全量重建与 backfill 不经本断言（存量安全）。
function checkArchDirNaming(id) {
  if (!parseChangeIdDate(id)) return null; // 非日期前缀或非法日期前缀 → 不断言
  if (/gov-review|arch-review/.test(id)) return null;
  return `naming: delivery/review/${id} 缺写槽 token（gov-review / arch-review）——日期前缀提案目录名必须命中 .claude/scope-map.json review-agent 写槽，否则评审/核验期制品写入被 enforce-scope 拦截（见 protocol「双轨路由」节命名条款）`;
}

// ── 单行 upsert：在 review INDEX 中定位 <id> 的行 → 替换或插入（只动一行，对偶 closeout-check dev INDEX 刷行）──
// 261015 批 T6：lane 键 arch→review、扫描根随新树（函数名保留 upsertArchRow 免散引改动，语义已随 lane 迁移）。
function upsertArchRow(indexPath, id, cache) {
  const namingError = checkArchDirNaming(id);
  if (namingError) return { ok: false, error: namingError };
  if (!existsSync(join(resolve(projectDir, 'delivery', 'review'), id))) {
    return { ok: false, error: `review dir delivery/review/${id} absent on disk` };
  }
  if (!existsSync(indexPath)) return { ok: false, error: 'delivery/review/INDEX.md absent — run full rebuild first' };
  const entry = computeReviewEntry(id, cache);
  // 已合 main：按目录末提交现推并钉进缓存（260909 T6；前提 = 合 main 完成后跑本 upsert，R1-F3）
  const merged = dirMergedInMain(`delivery/review/${id}`);
  // T3（260920-gov-review-verdict-anchor-align，FROZEN 2026-09-20）：守卫收真值型——
  // gitContainedInMain 返回 'yes'/'no'/''（:118），'no' 在 JS 中为真值 ⇒ 旧守卫会把 'no' 也钉进缓存，
  // 而全量重建缓存优先（:240）⇒ 合并后该列不翻 yes。收窄为只钉 'yes'（已合并的 immutable 事实）。
  if (merged === 'yes') { entry.mergedInMain = merged; cache[id] = { ...(cache[id] || {}), date: entry.date, mergedInMain: merged }; }
  const newRow = `| [${esc(entry.changeId)}](${entry.changeId}/) | ${entry.date || '—'} | ${esc(entry.status)} | ${esc(entry.mergedInMain) || '—'} |`;

  const content = readText(indexPath);
  const lines = content.split('\n');
  const archStart = lines.findIndex(l => /^## review /.test(l));
  if (archStart < 0) return { ok: false, error: 'review INDEX header non-standard (no "## review" section)' };
  const linkRe = new RegExp(`^\\| \\[${id.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\]\\(${id.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}/`);
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

// ── 一次性回填：对全部 review 目录算 date/mergedInMain（fork git 一次）+ landed 状态，写缓存 + 全量重建 ──
function backfill() {
  const cache = loadStateCache();
  const root = resolve(projectDir, 'delivery', 'review');
  const names = existsSync(root) ? readdirSync(root, { withFileTypes: true }).filter(d => d.isDirectory()).map(d => d.name) : [];
  let computed = 0, landedCount = 0;
  for (const name of names) {
    const sha = gitFirstCommitSha(`delivery/review/${name}`);
    const date = parseChangeIdDate(name) || (sha ? gitCommitDate(sha) : '');
    const merged = dirMergedInMain(`delivery/review/${name}`);
    const landed = deriveLanded(join(root, name), cache[name]);
    if (landed) landedCount++;
    // T3（260920-gov-review-verdict-anchor-align，R1-F7 采纳）：merged === ''（git 不可用）时保旧值，
    // 防「已钉的 yes 被空答复抹掉」。
    cache[name] = { date, mergedInMain: merged === '' ? (cache[name] ? cache[name].mergedInMain : '') : merged, landed };
    computed++;
  }
  saveStateCache(cache);
  const items = [...collectDev(), ...collectReview(cache), ...collectIntake()];
  const indexMd = buildIndexMd(items);
  writeFileSync(laneIndexPath('dev'), indexMd.dev, 'utf8');
  writeFileSync(laneIndexPath('review'), indexMd.review, 'utf8');
  writeFileSync(laneIndexPath('intake'), indexMd.intake, 'utf8');
  return { ok: true, backfilled: computed, reviewCount: items.filter(i => i.lane === 'review').length, intakeCount: items.filter(i => i.lane === 'intake').length, landed: landedCount };
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
  // landed verdict 闭集（F1 回归）：用 os.tmpdir() 夹具测 deriveLanded 真函数（不写 delivery/review、不污染收件箱）
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
  // 编号标题 + 加粗散文 / JSON 行锚（260906-gov-review-index-status-truth-source T2② + T4；3 实况 arch 批翻判回归夹具）
  const f5 = mkFix('e', ['## 7. Verdict', '', '**GREEN** — 实际落地 == FROZEN PROPOSAL v1.0 的 landing 清单']);
  if (f5) { t('landed: 编号标题 + 加粗散文 GREEN → true（260905/260906 实况格式回归）', deriveLanded(f5, {}), true); rmFix(f5); }
  const f6 = mkFix('f', ['{"ok": true, "changeId": "x", "verdict": "GREEN"}']);
  if (f6) { t('landed: JSON 行锚 "verdict": "GREEN" → true', deriveLanded(f6, {}), true); rmFix(f6); }
  const f7 = mkFix('g', ['## 7. Verdict', '', '**RED** — 未通过']);
  if (f7) { t('landed: 编号标题 + 加粗散文 RED → false', deriveLanded(f7, {}), false); rmFix(f7); }
  // dev 收口谓词（260906-gov-review-index-status-truth-source T2/T4；显式夹具非空输入，防真空绿）
  const mkDevFix = (id, files) => {
    const d = resolve(osTmp, 'changes-index-selfcheck-dev-' + id);
    try {
      mkdirSync(d, { recursive: true });
      for (const f of files) writeFileSync(join(d, f), 'x', 'utf8');
      return d;
    } catch { return null; }
  };
  const dv1 = mkDevFix('a', ['docs-delta.md', 'review-packet.md']);
  if (dv1) { t('dev: post-cutoff 收口两件全在场 → closed', deriveDevClosed(dv1, '260906-x'), true); rmFix(dv1); }
  const dv2 = mkDevFix('b', ['docs-delta.md']);
  if (dv2) { t('dev: 缺 review-packet.md → open', deriveDevClosed(dv2, '260906-x'), false); rmFix(dv2); }
  const dv3 = mkDevFix('c', ['docs-delta.md', 'review-packet.md']);
  if (dv3) { t('dev: pre-cutoff 缺 taskpackage.json → open', deriveDevClosed(dv3, '260101-x'), false); rmFix(dv3); }
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
  // T3（260924-gov-review-gate-parse-teeth）：help / unknown-flag 入口面判别（spawn 子进程断言退出码，
  // 与主流程互斥——self-check 内 spawn 不会触发全量重建）。rma8 病灶 = 想看用法反触发全量重建。
  const runExit = (flagArgs) => spawnSync(process.execPath, [process.argv[1], ...flagArgs], { encoding: 'utf8', timeout: 60000 }).status;
  t('cli: --help → exit 0 且不重建', runExit(['--help']), 0);
  t('cli: 未知旗标 → exit 2（不静默全量重建）', runExit(['--bogus-selfcheck-probe']), 2);
  // intake lane 新臂（261015 批 T5；真空绿规约：fixture 构造非空输入，逐 case 有预期）
  const spineInline = ['---', 'changeId: 261010-x', 'sources: s', 'faceMap: 需求面动（…）；架构面动（…）；契约面疑似动（…）；实现面动（…）；governance 不动', 'anchors: {req: "v1", arch: "v2", contract: "v3", imp: "v4"}', 'openQ: o', 'status: in-flight', '---', '正文'].join('\n');
  const sf = parseSpine(spineInline);
  t('intake: inline spine 解析七字段', sf && sf.changeId === '261010-x' && sf.status === 'in-flight' && !!sf.anchors, true);
  t('intake: faceMap inline 形态命中面（governance 不动缺席）', JSON.stringify(faceMapHits(sf.faceMap)), JSON.stringify(['req', 'arch', 'contract', 'imp']));
  const spineStruct = ['---', 'changeId: 261012-y', 'faceMap:', '  requirements: 动（轻量记账）', '  architecture: 动（指针刷）', '  contract: 疑似触（…）', '  implementation: 动（…）', '  governance: 不动', 'anchors:', '  req: "已回填"', '  contract: "已回填"', '  arch: "已回填"', '  imp: "已回填"', 'status: in-flight', '---'].join('\n');
  const ss = parseSpine(spineStruct);
  t('intake: faceMap 结构化形态命中面（不动缺席）', JSON.stringify(faceMapHits(ss.faceMap)), JSON.stringify(['req', 'arch', 'contract', 'imp']));
  const prog = anchorsProgress(ss.anchors);
  t('intake: anchors 全回填 4/4', JSON.stringify(prog), JSON.stringify({ filled: 4, total: 4 }));
  const progWait = anchorsProgress('anchors: {req: "待回填", arch: "待回填", contract: "待回填"}');
  t('intake: anchors 待回填 → 0/3', JSON.stringify(progWait), JSON.stringify({ filled: 0, total: 3 }));
  t('intake: 漂移① 全回填但 status≠closed → ⚠未闭环', JSON.stringify(intakeDrift({ statusBase: 'in-flight', hitCount: 4, filled: 4, lastCommitDate: '2026-10-01', today: '2026-10-16' })), JSON.stringify(['⚠未闭环']));
  t('intake: 漂移② awaiting-* 滞留 >7d → ⚠滞留Nd', JSON.stringify(intakeDrift({ statusBase: 'awaiting-human:gate', hitCount: 2, filled: 0, lastCommitDate: '2026-10-01', today: '2026-10-16' })), JSON.stringify(['⚠滞留15d']));
  t('intake: awaiting-* 未超 7d → 无标', JSON.stringify(intakeDrift({ statusBase: 'awaiting-human:gate', hitCount: 2, filled: 0, lastCommitDate: '2026-10-12', today: '2026-10-16' })), JSON.stringify([]));
  t('intake: 值域外 status（landed）→ ⚠值域外', JSON.stringify(intakeDrift({ statusBase: 'landed', hitCount: 3, filled: 3, lastCommitDate: '2026-10-01', today: '2026-10-16' })), JSON.stringify(['⚠值域外', '⚠未闭环']));
  t('intake: closed 全回填 → 无标', JSON.stringify(intakeDrift({ statusBase: 'closed', hitCount: 3, filled: 3, lastCommitDate: '2026-10-01', today: '2026-10-16' })), JSON.stringify([]));
  // 261026 批 T5③：源文件缺口与值域外分流（判别力：不传 spineOk/hasStatus 时缺口 fixture 落 ⚠值域外 变红）
  t('intake: frontmatter 未闭合 → ⚠源文件缺口', JSON.stringify(intakeDrift({ statusBase: '—', hitCount: 0, filled: 0, lastCommitDate: '', today: '2026-10-16', spineOk: false, hasStatus: false })), JSON.stringify(['⚠源文件缺口（frontmatter 未闭合）']));
  t('intake: 已解析但缺 status → ⚠源文件缺口', JSON.stringify(intakeDrift({ statusBase: '—', hitCount: 2, filled: 0, lastCommitDate: '2026-10-01', today: '2026-10-16', spineOk: true, hasStatus: false })), JSON.stringify(['⚠源文件缺口（缺 status 字段）']));
  t('intake: 值域外 status → 不误标缺口', JSON.stringify(intakeDrift({ statusBase: 'landed', hitCount: 3, filled: 3, lastCommitDate: '2026-10-01', today: '2026-10-16', spineOk: true, hasStatus: true })), JSON.stringify(['⚠值域外', '⚠未闭环']));
  const pass = cases.filter(c => c.pass).length;
  const ok = pass === cases.length;
  return { selfCheck: 'changes-index', ok, pass, fail: cases.length - pass, total: cases.length, cases: ok ? undefined : cases.filter(c => !c.pass) };
}

function main() {
  const cache = loadStateCache();
  const items = [...collectDev(), ...collectReview(cache), ...collectIntake()];
  const indexMd = buildIndexMd(items);
  writeFileSync(laneIndexPath('dev'), indexMd.dev, 'utf8');
  writeFileSync(laneIndexPath('review'), indexMd.review, 'utf8');
  writeFileSync(laneIndexPath('intake'), indexMd.intake, 'utf8');
  return { ok: true, devCount: items.filter(i => i.lane === 'dev').length, reviewCount: items.filter(i => i.lane === 'review').length, intakeCount: items.filter(i => i.lane === 'intake').length };
}

const CHANGES_INDEX_USAGE = `用法：node .claude/scripts/changes-index.mjs [--upsert <changeId> | --backfill | --stale-report | --self-check | --help]
  --upsert <change-id>   review 单行 upsert（状态转移时 agent 主动跑，只动一行）
  --backfill             一次性回填（日期/已合main/landed，零 git fork 读缓存）
  --stale-report         只读列出未落地且首提交超阈的陈旧批次目录（GC 观察面）
  --self-check           结构化自检（ARQ-114 先例）
  --help / -h            本用法（exit 0）
裸跑（无参数）= 幂等全量重建三 lane INDEX；未知 -- 旗标 = 用法错误 exit 2（260924-gov-review-gate-parse-teeth T3：不静默触发全量重建，rma8 病灶闭合）。`;

const invokedDirectly = process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url);
if (invokedDirectly) {
  const args = process.argv.slice(2);
  let result;
  if (args[0] === '--help' || args[0] === '-h') {
    console.log(CHANGES_INDEX_USAGE);
    process.exit(0);
  } else if (args[0] === '--self-check') {
    result = selfCheck();
  } else if (args[0] === '--backfill') {
    result = backfill();
  } else if (args[0] === '--stale-report') {
    result = staleReport();
  } else if (args[0] === '--upsert') {
    const id = args[1];
    if (!id) { console.error(JSON.stringify({ ok: false, error: '--upsert requires <changeId>' })); process.exit(2); }
    const cache = loadStateCache();
    result = upsertArchRow(laneIndexPath('review'), id, cache);
    saveStateCache(cache);
  } else if (args[0] !== undefined && args[0].startsWith('--')) {
    // T3（260924-gov-review-gate-parse-teeth）：未知旗标 = 用法错误 exit 2，不静默落 main() 全量重建。
    console.error(`未知旗标：${args[0]}\n${CHANGES_INDEX_USAGE}`);
    process.exit(2);
  } else {
    result = main();
  }
  console.log(JSON.stringify(result, null, 2));
  if (result && result.ok === false) process.exit(1);
  if (result && result.selfCheck && !result.ok) process.exit(1);
}

export { main, upsertArchRow, backfill, staleReport, selfCheck, computeReviewEntry, deriveLanded, staleCandidate, parseSpine, faceMapHits, anchorsProgress, intakeDrift, collectIntake };
