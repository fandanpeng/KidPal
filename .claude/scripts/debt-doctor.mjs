// debt-doctor.mjs — debt 账本检查器（告警级 / 只读；不注册钩子、不进任何闸链）。
//
// 260923-gov-review-arq-ssot-restructure（T5）落地。把「簿记纪律」变成机检事实。
//   node .claude/scripts/debt-doctor.mjs               # 检查真实仓
//   node .claude/scripts/debt-doctor.mjs --self-check  # 结构化自检（每项检查各带反转夹具）
// 两档：red = 结构性错误（阻断级语义，须修）；advisory = 内容性待补（迁移项普遍命中，不阻断，只计数）。
import { readFileSync, existsSync } from 'node:fs';
import { resolve, dirname, join } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import {
  DOMAINS, DOMAIN_KEYS, domainDir, readItems, checkItems, parseItem,
  gateLayersOf, hardExclusionsOf, DOMAIN_KEYS as ALL_DOMAINS, LEDGER_ROOT,
} from './debt-ledger-shared.mjs';
import { expectedIndexes, renderIndex } from './debt-render.mjs';

const REPO = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const isDirectRun = () => process.argv[1] && pathToFileURL(resolve(process.argv[1])).href === import.meta.url;

/** 对一组已解析条目跑全部检查（doctor 与 self-check 共用同一逻辑，防自检假绿）。 */
export function runChecks(items, { indexTextOf } = {}) {
  const { red, advisory } = checkItems(items);
  const out = { red: [...red], advisory: [...advisory] };
  // 件 5：paths[] → G 层/硬排除推导（只在条目申报了 paths 时判——未申报走 advisory，见 checkItems）
  for (const it of items) {
    const paths = it.fm.paths || [];
    if (!paths.length) continue;
    const derived = gateLayersOf(paths);
    const declared = String(it.fm.gateLayers || '').split(/[,\s]+/).filter(Boolean).sort();
    if (declared.length && JSON.stringify(declared) !== JSON.stringify(derived)) {
      out.red.push({ at: `${it.domain}/${it.file}`, code: 'gate-layer-mismatch', msg: `自报 [${declared}] ≠ 由 paths 推导 [${derived}]` });
    }
    const ex = hardExclusionsOf(paths);
    const hit = Object.entries(ex).filter(([, v]) => v).map(([k]) => k);
    if (hit.length && String(it.fm.hardExclusions || '') !== 'true') {
      out.advisory.push({ at: `${it.domain}/${it.file}`, code: 'hard-exclusion-undeclared', msg: `paths 命中硬排除 [${hit.join(',')}] 但未标 hardExclusions: true` });
    }
  }
  // 件 6：漂移检测 —— 盘上 index ≠ 应渲染（覆盖三种成因：手改；merge=union 并出的乱序/陈旧计数；漏跑渲染）
  if (indexTextOf) {
    for (const domain of ALL_DOMAINS) {
      for (const [rel, want] of expectedIndexes(domain, items)) {
        const have = indexTextOf(domain, rel);
        const at = `${LEDGER_ROOT.join('/')}/${DOMAINS[domain].dir}/${rel}`;
        if (have === null) out.red.push({ at, code: 'index-missing', msg: '生成件不在场' });
        else if (have !== want) out.red.push({ at, code: 'index-stale', msg: '待重渲染（渲染后 git diff 非空）' });
      }
    }
  }
  return out;
}

export function realIndexReader() {
  return (domain, rel) => {
    const p = join(domainDir(REPO, domain), rel);
    if (!existsSync(p)) return null;
    return readFileSync(p, 'utf8').replace(/\r\n/g, '\n');
  };
}

function selfCheck() {
  const fails = [];
  const mk = (id, domain, status, extra = {}) => ({
    wellFormed: true, domain, file: `${id}.md`,
    fm: { id, domain, status, opened: '2026-09-01', severity: status === 'closed' ? '' : 'SHOULD', title: 't', trigger: '', paths: [], ...extra },
    body: '',
  });

  // 夹具 1 · ID 形态错 → 红（反转：修好 ID 即不红）
  if (!runChecks([mk('BAD-ID', 'gov', 'deferred')]).red.some((r) => r.code === 'bad-id')) fails.push('bad-id');
  // 夹具 2 · 非祖辈前缀↔域 不一致 → 红（TECH- 却在 gov）；正控（TECH- 落 tech）不红
  if (!runChecks([mk('TECH-260901-x', 'gov', 'deferred')]).red.some((r) => r.code === 'prefix-domain-mismatch')) fails.push('prefix-domain');
  if (runChecks([mk('TECH-260901-x', 'tech', 'deferred')]).red.some((r) => r.code === 'prefix-domain-mismatch')) fails.push('prefix-domain-false-positive');
  // 夹具 2b · 祖辈前缀 ARQ- 落在 tech → advisory（非红）——与「迁移不改名」一致
  const r2b = runChecks([mk('ARQ-260901-x', 'tech', 'deferred')]);
  if (!r2b.advisory.some((a) => a.code === 'legacy-prefix-off-domain')) fails.push('legacy-prefix-advisory');
  if (r2b.red.some((r) => r.code === 'prefix-domain-mismatch')) fails.push('legacy-prefix-should-not-be-red');
  // 夹具 2c · severity 出并集 → 红；在并集内但非 rubric 五档（COULD/MAJOR）→ advisory（非红）
  if (!runChecks([mk('ARQ-260901-x', 'gov', 'deferred', { severity: 'WAT' })]).red.some((r) => r.code === 'bad-severity')) fails.push('bad-severity');
  const r2c = runChecks([mk('ARQ-260901-x', 'gov', 'deferred', { severity: 'COULD' })]);
  if (!r2c.advisory.some((a) => a.code === 'severity-off-rubric')) fails.push('severity-off-rubric-advisory');
  if (r2c.red.some((r) => r.code === 'bad-severity')) fails.push('could-should-not-be-red');
  // 夹具 3 · 重复 ID → 红
  if (!runChecks([mk('ARQ-260901-x', 'gov', 'deferred'), mk('ARQ-260901-x', 'gov', 'deferred')]).red.some((r) => r.code === 'duplicate-id')) fails.push('dup-id');
  // 夹具 4 · status 出闭集 → 红
  if (!runChecks([mk('ARQ-260901-x', 'gov', 'bogus')]).red.some((r) => r.code === 'bad-status')) fails.push('bad-status');
  // 夹具 5 · tech 无 handoff → advisory（非红）
  const r5 = runChecks([mk('TECH-260901-x', 'tech', 'deferred')]);
  if (!r5.advisory.some((a) => a.code === 'tech-no-handoff')) fails.push('tech-handoff-advisory');
  if (r5.red.some((r) => r.code === 'tech-no-handoff')) fails.push('tech-handoff-should-not-be-red');
  // 夹具 6 · 缺必填字段 → 红（opened 空）；已结项豁免 severity（正控）
  if (!runChecks([mk('ARQ-260901-x', 'gov', 'deferred', { opened: '' })]).red.some((r) => r.code === 'missing-field')) fails.push('missing-field');
  if (runChecks([mk('ARQ-260901-x', 'gov', 'closed')]).red.some((r) => r.code === 'missing-field')) fails.push('closed-severity-exempt-broken');
  // 夹具 7 · 漂移检测：index 文本与应渲染不一致 → 红 index-stale；一致 → 不红
  const items = [mk('ARQ-260901-x', 'gov', 'deferred')];
  const stale = runChecks(items, { indexTextOf: () => 'garbage' });
  if (!stale.red.some((r) => r.code === 'index-stale')) fails.push('index-stale');
  const fresh = runChecks(items, { indexTextOf: (d, rel) => expectedIndexes(d, items).get(rel) ?? null });
  if (fresh.red.some((r) => r.code === 'index-stale' || r.code === 'index-missing')) fails.push('index-false-positive');
  // 夹具 8 · 无 frontmatter → 红
  const broken = { ...mk('ARQ-260901-x', 'gov', 'deferred'), wellFormed: false };
  if (!runChecks([broken]).red.some((r) => r.code === 'no-frontmatter')) fails.push('no-frontmatter');
  // 夹具 9 · gate-layers 自报与推导不一致 → 红
  const g = mk('ARQ-260901-x', 'gov', 'deferred', { paths: ['.claude/rules/x.md'], gateLayers: 'G3' });
  if (!runChecks([g]).red.some((r) => r.code === 'gate-layer-mismatch')) fails.push('gate-layer-mismatch');
  // 夹具 10 · 解析器正控：合法 frontmatter 解析出字段
  const parsed = parseItem('---\nid: ARQ-260901-x\ndomain: gov\npaths: [a/b.md, c/d.md]\n---\n\nbody\n');
  if (parsed.fm.id !== 'ARQ-260901-x' || parsed.fm.paths.length !== 2) fails.push('parse-roundtrip');
  // 夹具 11 · 位置 == f(状态)：已结在顶层 → 红 mislocated；已在正确归档位 → 不红（正控）
  const closedTop = { ...mk('ARQ-260902-y', 'gov', 'closed', { closed: '2026-09-05' }), rel: 'ARQ-260902-y.md' };
  if (!runChecks([closedTop]).red.some((r) => r.code === 'mislocated')) fails.push('mislocated');
  const closedArchived = { ...closedTop, rel: '_archive/2026/ARQ-260902-y.md' };
  if (runChecks([closedArchived]).red.some((r) => r.code === 'mislocated')) fails.push('mislocated-false-positive');
  // 夹具 12 · CDQ 前缀落 contract 域 → 不红（正控，261019 批第四域）
  if (runChecks([mk('CDQ-260913-a', 'contract', 'deferred', { fourClass: 'CONTRACT_CHANGE_REQUIRED' })]).red.length) fails.push('cdq-in-contract-false-positive');
  // 夹具 13 · CDQ 前缀落 gov 域 → 红 prefix-domain-mismatch（CDQ 非祖辈前缀）
  if (!runChecks([mk('CDQ-260913-a', 'gov', 'deferred')]).red.some((r) => r.code === 'prefix-domain-mismatch')) fails.push('cdq-prefix-domain');
  // 夹具 14 · contract 条目缺 fourClass → advisory contract-no-fourclass（非红）
  const r14 = runChecks([mk('CDQ-260913-a', 'contract', 'deferred')]);
  if (!r14.advisory.some((a) => a.code === 'contract-no-fourclass')) fails.push('contract-fourclass-advisory');
  if (r14.red.some((r) => r.code === 'contract-no-fourclass')) fails.push('contract-fourclass-should-not-be-red');
  // 夹具 15 · contract 条目带 fourClass → 零报（正控）
  if (runChecks([mk('CDQ-260913-a', 'contract', 'deferred', { fourClass: 'NO_CONTRACT_CHANGE' })]).advisory.some((a) => a.code === 'contract-no-fourclass')) fails.push('contract-fourclass-false-positive');

  console.log(fails.length === 0 ? 'debt-doctor --self-check: ok (17/17)' : `debt-doctor --self-check: FAIL\n  ${fails.join('\n  ')}`);
  return fails.length === 0;
}

function main() {
  if (process.argv.includes('--self-check')) process.exit(selfCheck() ? 0 : 1);
  const items = readItems(REPO);
  if (items.length === 0) { console.error(`FATAL ${LEDGER_ROOT.join('/')}/** 下 0 条目——真空输入，拒绝判绿（防真空绿）`); process.exit(1); }
  const { red, advisory } = runChecks(items, { indexTextOf: realIndexReader() });
  console.log(`条目 ${items.length} 条：${DOMAIN_KEYS.map((d) => `${d}=${items.filter((x) => x.domain === d).length}`).join(' ')}`);
  if (advisory.length) {
    const byCode = {};
    for (const a of advisory) byCode[a.code] = (byCode[a.code] || 0) + 1;
    console.log(`advisory ${advisory.length} 条（不阻断）：${Object.entries(byCode).map(([k, v]) => `${k}=${v}`).join(' ')}`);
  }
  if (red.length) {
    console.log(`\nRED ${red.length} 条：`);
    for (const r of red) console.log(`  [${r.code}] ${r.at} — ${r.msg}`);
    process.exit(1);
  }
  console.log('RED 零命中 —— ok');
}

if (isDirectRun()) main();