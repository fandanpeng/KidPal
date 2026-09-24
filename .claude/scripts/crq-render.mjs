// crq-render.mjs — 从 docs/crq/** 渲染每域 INDEX.md + 归档年 INDEX.md（生成件），并执行归档归位。
//
// 260923-gov-review-arq-ssot-restructure（T2）落地。
//   node .claude/scripts/crq-render.mjs               # 归档归位 + 渲染全部 index
//   node .claude/scripts/crq-render.mjs --check       # 只检「盘上 == 应渲染」，不写盘不搬文件（doctor 件 6 复用）
//   node .claude/scripts/crq-render.mjs --self-check  # 结构化自检
// 纪律：INDEX.md 是**生成件**，勿手改；改状态去改条目文件再重跑本脚本。
//
// 归档（human 2026-09-24 裁定机制）：**位置 = 状态的纯函数，搬文件只有本脚本干，人永远不搬**。
//   status=closed 且 closed 带年份 → `_archive/<年>/<ID>.md`（镜像 delivery/workstreams/_archive/<年>/ 先例）；
//   重开条目自动搬回顶层（双向、幂等）。主 INDEX 的已结区只渲染「每年一行指针 + 计数」，
//   薄行详情在该年 `_archive/<年>/INDEX.md`。
//
// 确定性（T2-R5，承重）：输出不含任何「当前时间」依赖（初稿的「最近 7 天新增」已删——它会让
//   「无改动也 diff 非空」，把漂移检测变成永久假红）。排序：未结按 opened 升序、同日按 ID 升序；
//   升序 ⇒ 新条目落节尾 ⇒ 两分支各自末尾追加 ⇒ merge=union 几乎总无冲突（起草期实演 E6 证实）。
import { readFileSync, writeFileSync, existsSync, mkdirSync } from 'node:fs';
import { resolve, dirname, join } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import {
  DOMAINS, DOMAIN_KEYS, STATUS_SECTIONS, indexFile, domainDir, readItems,
  sortForRender, GENERATED_MARKER, checkItems, ARCHIVE_DIR, expectedRel,
  computeRelocations, applyRelocations, archiveIndexRel,
} from './crq-ledger-shared.mjs';

const REPO = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const isDirectRun = () => process.argv[1] && pathToFileURL(resolve(process.argv[1])).href === import.meta.url;

// 表格单元格转义：竖线须转义（否则破坏表结构——正是旧形态的事故①），换行压平
const cell = (s) => String(s ?? '').replace(/\|/g, '\\|').replace(/\r?\n/g, ' ').trim();

const SECTION_TITLE = Object.fromEntries(STATUS_SECTIONS);
const FOURTH_COL = { blocker: '处理时机', deferred: '等什么触发', 'accepted-risk': '复议触发' };

const closedYear = (fm) => {
  const m = /^(\d{4})-/.exec(String(fm.closed || ''));
  return m ? m[1] : null;
};

function indexHeader(domain, rel, extra) {
  const depth = rel.split('/').length - 1; // INDEX.md 相对仓根的深度差（docs/crq/<域>/ 或再深一层）
  const up = '../'.repeat(3 + depth);
  const out = [
    GENERATED_MARKER,
    '',
    `# ${DOMAINS[domain].prefix} · ${domain} 域账${extra || ''}（自动生成，勿手改）`,
    '',
    `> 由 [${up}.claude/scripts/crq-render.mjs](${up}.claude/scripts/crq-render.mjs) 从条目文件生成。`,
    `> **单一事实源 = 条目文件**（一条目一文件）。改状态去改条目文件再重跑：\`node .claude/scripts/crq-render.mjs\`。`,
    `> **勿手改本文件**——改了会在下次渲染被覆盖；若确已手改或 union 合并后未重渲染，\`crq-doctor.mjs\` 会报「待重渲染」。`,
    `> 引用本文件必须带目录（\`docs/crq/${DOMAINS[domain].dir}/${rel}\`）——仓内有多份同名 \`INDEX.md\`。`,
  ];
  return out;
}

/** 主 INDEX：未结三节（薄行表）+ 已结区（每年一行指针 + 计数）。 */
export function renderIndex(domain, items) {
  const mine = sortForRender(items.filter((x) => x.domain === domain && x.wellFormed));
  const open = mine.filter((x) => x.fm.status !== 'closed');
  const closed = mine.filter((x) => x.fm.status === 'closed');
  const countOf = (s) => open.filter((x) => x.fm.status === s).length;

  const out = indexHeader(domain, 'INDEX.md');
  out.push('');
  out.push(`**摘要**：未结 ${open.length}（待拍板 ${countOf('blocker')} / 待触发 ${countOf('deferred')} / 已接受风险 ${countOf('accepted-risk')}）；已结 ${closed.length}（归档索引见下）。`);
  out.push('');

  for (const [status] of STATUS_SECTIONS) {
    if (status === 'closed') continue;
    const rows = mine.filter((x) => x.fm.status === status);
    out.push(`## ${SECTION_TITLE[status]}（${rows.length}）`);
    out.push('');
    if (rows.length === 0) { out.push('_（无）_'); out.push(''); continue; }
    out.push(`| ID | 严重度 | title | ${FOURTH_COL[status]} |`);
    out.push('|---|---|---|---|');
    for (const x of rows) out.push(`| [${x.fm.id}](${x.fm.id}.md) | ${cell(x.fm.severity)} | ${cell(x.fm.title)} | ${cell(x.fm.trigger)} |`);
    out.push('');
  }

  out.push(`## 已结（${closed.length}）`);
  out.push('');
  out.push('> 已结条目文件由渲染器自动归位 `_archive/<结项年>/`（**人不搬文件**；重开自动搬回）。薄行索引按年在归档目录，主 INDEX 只留指针——这是「翻不到未结」旧病灶的根治。');
  out.push('');
  const byYear = new Map();
  for (const x of closed) {
    const y = closedYear(x.fm) || 'undated';
    byYear.set(y, (byYear.get(y) || 0) + 1);
  }
  if (byYear.size === 0) out.push('_（无）_');
  else {
    out.push('| 归档年 | 条数 | 索引 |');
    out.push('|---|---|---|');
    for (const [y, n] of [...byYear.entries()].sort()) {
      const rel = y === 'undated' ? null : archiveIndexRel(y);
      out.push(`| ${y === 'undated' ? '无结项日期（未归档，见下方薄行）' : y} | ${n} | ${rel ? `[${ARCHIVE_DIR}/${y}/INDEX.md](${rel})` : '—'} |`);
    }
  }
  const undated = closed.filter((x) => !closedYear(x.fm));
  if (undated.length) {
    out.push('');
    out.push('| ID | title | 已结日期 | 依据 |');
    out.push('|---|---|---|---|');
    for (const x of undated) out.push(`| [${x.fm.id}](${x.fm.id}.md) | ${cell(x.fm.title)} | ${cell(x.fm.closed) || '—'} | ${cell(x.fm.closedBy) || '—'} |`);
  }
  out.push('');
  return out.join('\n');
}

/** 归档年 INDEX：该年已结薄行（ID / title / 已结日期 / 依据）。 */
export function renderArchiveIndex(domain, year, items) {
  const rows = sortForRender(items.filter((x) => x.domain === domain && x.wellFormed && x.fm.status === 'closed' && closedYear(x.fm) === year));
  const out = indexHeader(domain, archiveIndexRel(year), ` · ${year} 年已结归档`);
  out.push('');
  out.push(`**${year} 年已结 ${rows.length} 条**（薄行：证据正文在条目文件里，点 ID 即达）。`);
  out.push('');
  out.push('| ID | title | 已结日期 | 依据 |');
  out.push('|---|---|---|---|');
  for (const x of rows) out.push(`| [${x.fm.id}](${x.fm.id}.md) | ${cell(x.fm.title)} | ${cell(x.fm.closed)} | ${cell(x.fm.closedBy) || '—'} |`);
  out.push('');
  return out.join('\n');
}

/** 该域应渲染的全部 index（主 + 归档年），rel → 内容。doctor --check 与写盘共用。 */
export function expectedIndexes(domain, items) {
  const map = new Map([['INDEX.md', renderIndex(domain, items)]]);
  const years = new Set(
    items.filter((x) => x.domain === domain && x.wellFormed && x.fm.status === 'closed')
      .map((x) => closedYear(x.fm)).filter(Boolean),
  );
  for (const y of [...years].sort()) map.set(archiveIndexRel(y), renderArchiveIndex(domain, y, items));
  return map;
}

function selfCheck() {
  const fails = [];
  const mk = (id, domain, status, opened, closed = '', rel = null) => ({
    wellFormed: true, domain, file: `${id}.md`, rel: rel || `${id}.md`,
    fm: { id, domain, status, opened, closed, severity: 'SHOULD', title: `t-${id}`, trigger: '' }, body: '',
  });
  // 夹具 1：未结排序 —— opened 升序、同日按 ID 升序（反转实现即变红）
  const items = [
    mk('ARQ-260902-b', 'gov', 'deferred', '2026-09-02'),
    mk('ARQ-260902-a', 'gov', 'deferred', '2026-09-02'),
    mk('ARQ-260901-c', 'gov', 'blocker', '2026-09-01'),
    mk('ARQ-260903-d', 'gov', 'closed', '2026-09-03', '2026-09-05', '_archive/2026/ARQ-260903-d.md'),
  ];
  const text = renderIndex('gov', items);
  const order = [...text.matchAll(/\[(ARQ-[0-9a-z-]+)\]\(ARQ/g)].map((m) => m[1]);
  const want = ['ARQ-260901-c', 'ARQ-260902-a', 'ARQ-260902-b']; // 已结不在主表，走指针
  if (JSON.stringify(order) !== JSON.stringify(want)) fails.push(`sort-order:got=${order.join(',')}`);
  // 夹具 2：两次渲染逐字节相同（确定性）
  if (renderIndex('gov', items) !== text) fails.push('not-deterministic');
  // 夹具 3：竖线转义（title 含 | 不得破表）
  const t2 = renderIndex('gov', [{ ...mk('ARQ-260902-x', 'gov', 'blocker', '2026-09-02'), fm: { ...mk('ARQ-260902-x', 'gov', 'blocker', '2026-09-02').fm, title: 'a|b' } }]);
  if (!t2.includes('a\\|b')) fails.push('pipe-not-escaped');
  // 夹具 4：已结走归档指针，不进主表（薄行）；归档年 INDEX 含该条薄行
  if (!text.includes('_archive/2026/INDEX.md')) fails.push('closed-pointer-missing');
  if (text.includes('t-ARQ-260903-d')) fails.push('closed-row-leaked-into-main');
  const arch = renderArchiveIndex('gov', '2026', items);
  if (!arch.includes('[ARQ-260903-d](ARQ-260903-d.md)')) fails.push('archive-index-row-missing');
  // 夹具 5：域过滤（tech 条目不出现在 gov index）
  if (renderIndex('gov', [mk('TECH-260902-z', 'tech', 'blocker', '2026-09-02')]).includes('TECH-260902-z')) fails.push('domain-filter-broken');
  // 夹具 6：归档归位是纯函数且双向 —— closed 在顶层→搬入；未结在归档→搬回；closed 无日期→不动
  const moves = computeRelocations([
    mk('ARQ-260904-e', 'gov', 'closed', '2026-09-04', '2026-09-06'),                                  // 顶层的已结 → 应搬入 _archive/2026/
    mk('ARQ-260905-f', 'gov', 'deferred', '2026-09-05', '', '_archive/2026/ARQ-260905-f.md'),        // 归档里的未结 → 应搬回顶层
    mk('ARQ-260906-g', 'gov', 'closed', '2026-09-06', ''),                                           // 已结但无日期 → 不动
    mk('ARQ-260907-h', 'gov', 'closed', '2026-09-07', '2026-09-08', '_archive/2026/ARQ-260907-h.md'),// 已在正确位置 → 不动
  ]);
  const got = moves.map((m) => `${m.id}:${m.from}→${m.to}`).sort();
  const wantMoves = [
    'ARQ-260904-e:ARQ-260904-e.md→_archive/2026/ARQ-260904-e.md',
    'ARQ-260905-f:_archive/2026/ARQ-260905-f.md→ARQ-260905-f.md',
  ].sort();
  if (JSON.stringify(got) !== JSON.stringify(wantMoves)) fails.push(`relocation:got=${got.join(' ; ')}`);
  // 夹具 7：expectedRel 与 computeRelocations 同源一致（防两处判据漂移）
  if (expectedRel({ status: 'closed', closed: '2026-01-02', id: 'X' }) !== '_archive/2026/X.md') fails.push('expectedRel');
  console.log(fails.length === 0 ? 'crq-render --self-check: ok (8/8)' : `crq-render --self-check: FAIL\n  ${fails.join('\n  ')}`);
  return fails.length === 0;
}

/** 写盘守卫（R6）：目标缺生成标记头 → fail-loud 拒写；CRLF 归一后再比（R7）。 */
function writeGuarded(dst, want, label) {
  if (existsSync(dst)) {
    const raw = readFileSync(dst, 'utf8');
    if (!raw.startsWith(GENERATED_MARKER)) {
      console.error(`FATAL 目标不是生成件（缺生成标记头），拒写：${dst}\n  若这是手工内容，请人工核对后再处理——不静默冲掉。`);
      process.exit(1);
    }
    if (raw.replace(/\r\n/g, '\n') === want) { console.log(`未变：${label}`); return false; }
  }
  mkdirSync(dirname(dst), { recursive: true });
  writeFileSync(dst, want, 'utf8');
  console.log(`已渲染：${label}`);
  return true;
}

function main() {
  if (process.argv.includes('--self-check')) process.exit(selfCheck() ? 0 : 1);
  let items = readItems(REPO);
  if (items.length === 0) { console.error('FATAL docs/crq/** 下 0 条目——真空输入，拒绝渲染（防真空绿）'); process.exit(1); }
  const checkOnly = process.argv.includes('--check');

  if (!checkOnly) {
    const moves = computeRelocations(items);
    if (moves.length) {
      applyRelocations(REPO, moves);
      for (const m of moves) console.log(`归档归位：${m.domain}/${m.from} → ${m.domain}/${m.to}`);
      items = readItems(REPO); // 搬迁后重读，保证渲染与盘上一致
    }
  }

  let drift = 0;
  for (const domain of DOMAIN_KEYS) {
    for (const [rel, want] of expectedIndexes(domain, items)) {
      const dst = join(domainDir(REPO, domain), rel);
      const label = `${DOMAINS[domain].dir}/${rel}`;
      if (checkOnly) {
        const have = existsSync(dst) ? readFileSync(dst, 'utf8').replace(/\r\n/g, '\n') : null;
        if (have !== want) { console.log(`待重渲染：docs/crq/${label}`); drift++; }
        continue;
      }
      writeGuarded(dst, want, label);
    }
  }
  const { red } = checkItems(items);
  if (red.length) console.log(`注：doctor 级红项 ${red.length} 条（本脚本不判定，跑 crq-doctor.mjs 看详情）`);
  if (checkOnly && drift) process.exit(2);
}

if (isDirectRun()) main();