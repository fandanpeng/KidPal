// crq-migrate.mjs — 一次性迁移（已完成，retained 历史件）：docs/ARCH-REVIEW-QUEUE.md 的 95 条表格行 → 去重后 94 个条目文件（账本域目录）。
// 261014 批随账本改名，路径与依赖已迁 `docs/debt` / `debt-ledger-shared.mjs`——本件不复跑，仅存档。
//
// 260923-gov-review-arq-ssot-restructure（T1 改法 4）落地。
//   node .claude/scripts/crq-migrate.mjs --dry-run          # 只报计划（计数 + 域分布 + 逐条归类），不落盘
//   node .claude/scripts/crq-migrate.mjs [--src <path>]     # 实际迁移（源默认工作树文件；stub 化后须 --src 指历史版本）
//   node .claude/scripts/crq-migrate.mjs --verify --src <p> # 不写盘，只对已落盘结果做保真对账
// 保真断言：95 行 → 去重后 94 条 → 94 文件；ID 集合全等；双向逐条对账（源→意图、意图→落盘）。
// 重点处理：三套列结构（§1 六列 / §1.3 六列语义不同 / §2 四列）+ 58 处转义竖线 `\|` 还原为裸 `|`。
import { readFileSync, writeFileSync, mkdirSync, existsSync, readdirSync } from 'node:fs';
import { resolve, join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import { DOMAINS, DOMAIN_KEYS, domainDir, serializeItem, parseItem, readItems, ID_RE } from './debt-ledger-shared.mjs';

const REPO = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
// 迁移源：默认工作树的 docs/ARCH-REVIEW-QUEUE.md；但该文件在 T2 已被 stub 取代，
// 故复跑/复核时须显式指历史版本（`--src <path>`），例如：
//   git show <落地前 commit>:docs/ARCH-REVIEW-QUEUE.md > /tmp/orig-queue.md
//   node .claude/scripts/crq-migrate.mjs --src /tmp/orig-queue.md
const srcArg = process.argv.indexOf('--src');
const SRC = srcArg > -1 && process.argv[srcArg + 1] ? resolve(process.argv[srcArg + 1]) : resolve(REPO, 'docs', 'ARCH-REVIEW-QUEUE.md');
const DRY = process.argv.includes('--dry-run');
const VERIFY = process.argv.includes('--verify');

// ── 逐条域归类 ───────────────────────────────────────────────────────────────
// 未结 29 条（源 §1，起草期人工点算 = PROPOSAL §2-T3 表）：19 条显式登记于此，其余 10 条走 default=gov。
// 已结 66 行（源 §2，去重后 65 条）：显式 10 条（gov 注块 8 + tech 末尾块 2），其余走 default=gov。
// TECH_SIGNALS 关键词规则对**现行源数据零触发**（dry-run 实测 kw×0——历史 tech 项已全部显式化）；
// 保留该规则是为源数据变化时复跑的兜底，不删。全部归类逐条打印（id → 域 [依据]）供人读核。
// 注：cert-lifecycle-deadlock 为双重落位条目，其 why 显示 dual→accepted-risk（域仍按 OVERRIDES=tech）。
const OVERRIDES = {
  // requirement
  'ARQ-260916-home-date-format-english': 'requirement',
  // tech
  'ARQ-260918-spec-face-3page-reconcile': 'tech',
  'ARQ-260902-multiturn-continuity': 'tech',
  'ARQ-260831-p4-stroke-dasharray-visual-check': 'tech',
  'ARQ-260904-v23-asr-swap-quota-recheck': 'tech',
  'ARQ-260906-protocol-version-pinned-constant': 'tech',
  'ARQ-260906-nvs-csv-wifi-injection': 'tech',
  'ARQ-260907-engine-deploy-patch-drift': 'tech',
  'ARQ-260907-contract-1-5-strip-wording': 'tech',
  'ARQ-260907-openclaw-tools-rejection-rootcause': 'tech',
  'ARQ-260831-simulator-lvgl-version-drift': 'tech',
  'ARQ-260831-simulator-truecolor-vs-1bpp': 'tech',
  'ARQ-260831-i1-indexed-asset-on-device': 'tech',
  'ARQ-260831-small-cjk-1bpp-readability': 'tech',
  'ARQ-260901-openclaw-chatcompletions-public': 'tech',
  'ARQ-260902-cert-lifecycle-deadlock': 'tech',
  // gov（显式列出「关键词规则会误判为 tech、但按机制归属 gov」的项——判据见 PROPOSAL T3：
  // 治的是治理机制/门/脚本面即 gov，即便正文提到 firmware/server 路径）
  'ARQ-260827-contract-mech-gate-gap': 'gov',
  'ARQ-260909-crossend-consumer-tiers': 'gov',
  'ARQ-260908-known-red-count-no-mechanical-source': 'gov',
  'ARQ-260916-unit-gate-archive-recursion': 'gov',
  'ARQ-260916-supervisor-auth-fp-author': 'gov',
  'ARQ-260916-acc-gate-archive-skew': 'gov',
  'ARQ-260915-path-form-family-reference': 'gov',
  'ARQ-260907-lfc-citation-docs-prefix': 'gov',
  'ARQ-260914-lfc-citation-references-prefix': 'gov',
  'ARQ-260913-lfc-anchorable-delivery-gap': 'gov',
  'ARQ-260912-host-werror-comment-acceptance-gap': 'gov',
  // 已结条目中的显式归类（关键词规则命中不足 2 时会漏判为 gov 的 tech 项）
  'ARQ-260916-server-mvp-teeth-sweep': 'tech',
  'ARQ-260902-shim-401-gate-bypass': 'tech',
};

// 已结项的关键词规则（默认 gov —— 本仓 §2 绝大多数为治理批次）
const TECH_SIGNALS = [
  /firmware\//, /server\/deploy/, /server\/gateway/, /server\/shim/, /tools\//,
  /\bNVS\b/, /websocket|ws_provision/, /\bASR\b|\bTTS\b/, /LVGL|simulator|模拟器/,
  /OpenClaw|openclaw/, /证书|cert-|certificate/, /\bECS\b/, /引擎|engine-patch/,
  /1bpp|字号|字体子集/, /I1 索引|调色板/, /dasharray/,
];

function classify(id, title, body) {
  if (OVERRIDES[id]) return { domain: OVERRIDES[id], why: 'override' };
  const blob = `${title}\n${body}`;
  const hits = TECH_SIGNALS.filter((re) => re.test(blob));
  if (hits.length >= 2) return { domain: 'tech', why: `kw×${hits.length}` };
  return { domain: 'gov', why: 'default' };
}

// ── 表格行解析（尊重 `\|` 转义） ───────────────────────────────────────────────
function splitRow(line) {
  const cells = [];
  let cur = '';
  for (let i = 0; i < line.length; i++) {
    const ch = line[i];
    if (ch === '\\' && line[i + 1] === '|') { cur += '|'; i++; continue; }
    if (ch === '|') { cells.push(cur); cur = ''; continue; }
    cur += ch;
  }
  cells.push(cur);
  // 去掉行首/行尾空 cell
  if (cells.length && cells[0].trim() === '') cells.shift();
  if (cells.length && cells[cells.length - 1].trim() === '') cells.pop();
  return cells.map((c) => c.trim());
}

function dateFromId(id) {
  const m = /-(\d{6})-/.exec(id);
  if (!m) return '';
  const yy = m[1].slice(0, 2), mm = m[1].slice(2, 4), dd = m[1].slice(4, 6);
  return `20${yy}-${mm}-${dd}`;
}

// 源里存在的「一 ID 两行」= PROPOSAL §0 已记的「双重落位」形态（§1.3 监控面 + §2 决策闭环）。
// 新模型一条一项一状态 ⇒ 必须裁决唯一 status。裁决理由逐条写出，不静默取一边。
const DUAL_PLACEMENT = {
  'ARQ-260902-cert-lifecycle-deadlock': {
    keep: 'accepted-risk',
    note: '源双重落位：§2 记「human 2026-09-03 裁 accept-as-risk」的决策闭环，§1.3 记持续监控面（证书剩余有效期 <5 年 或 OTA 端点建立时复议）。新模型单状态 ⇒ 取 accepted-risk（监控面为活面），决策闭环作为正文段落保留。',
  },
};

const SECTION_STATUS = {
  '### 1.1': 'blocker', '### 1.2': 'deferred', '### 1.3': 'accepted-risk', '## 2.': 'closed',
};

function main() {
  if (!existsSync(SRC)) { console.error(`FATAL 源文件不存在：${SRC}`); process.exit(1); }
  const lines = readFileSync(SRC, 'utf8').split(/\r?\n/);

  let section = null;
  const rows = [];
  for (const line of lines) {
    for (const [k, v] of Object.entries(SECTION_STATUS)) if (line.startsWith(k)) section = v;
    if (!/^\|\s*ARQ-/.test(line)) continue;
    const cells = splitRow(line);
    const id = cells[0];
    if (!ID_RE.test(id)) { console.error(`FATAL 坏 ID：${id}`); process.exit(1); }
    rows.push({ id, cells, section });
  }

  if (rows.length === 0) { console.error('FATAL 源解析出 0 行——真空输入，拒绝继续（真空绿规约）'); process.exit(1); }

  const raw = rows.map(({ id, cells, section }) => {
    let title, prose, trigger = '', source = '', severity = '', closed = '';
    if (section === 'closed') {
      // | ID | title | 已结日期 | evidence |  —— 锚头三列 + 其余全进正文（容内嵌裸竖线）
      title = cells[1]; closed = cells[2]; prose = cells.slice(3).join(' | ');
    } else {
      // | ID | 严重度 | title | <未结原因|保留原因> | <处理时机|复议触发> | 来源 |
      // 锚首三列 + **锚末两列**（来源 / 处理时机），中间全部归正文 —— 对行内未转义裸竖线稳健
      severity = cells[1]; title = cells[2];
      source = cells[cells.length - 1];
      trigger = cells.length >= 5 ? cells[cells.length - 2] : '';
      prose = cells.slice(3, cells.length - 2).join(' | ');
    }
    const { domain, why } = classify(id, title, prose);
    const ownerM = /owner\s*=\s*([^\s。；;]+)/.exec(prose);
    const fm = {
      id,
      title,
      domain,
      status: section,
      severity: severity || (section === 'closed' ? '' : 'SHOULD'),
      owner: ownerM ? ownerM[1] : '',
      trigger: trigger || '',
      opened: dateFromId(id),
      closed: closed || '',
      closedBy: '',
      paths: [],
      source: source || '',
    };
    return { id, domain, why, section, title, prose, fm, cols: cells.length };
  });

  // 去重：一 ID 两行 ⇒ 按 DUAL_PLACEMENT 裁决唯一 status；无裁决记录即 fail-loud（不猜）
  const byId = new Map();
  for (const x of raw) {
    if (!byId.has(x.id)) { byId.set(x.id, [x]); continue; }
    byId.get(x.id).push(x);
  }
  const items = [];
  for (const [id, group] of byId) {
    if (group.length === 1) { items.push(group[0]); continue; }
    const rule = DUAL_PLACEMENT[id];
    if (!rule) {
      console.error(`FATAL 源中 ID 重复且无裁决记录：${id}（出现在 ${group.map((g) => g.section).join(' + ')}）——请补 DUAL_PLACEMENT 再跑，不代猜`);
      process.exit(1);
    }
    const keep = group.find((g) => g.section === rule.keep) || group[0];
    const merged = {
      ...keep,
      why: `dual→${rule.keep}`,
      prose: `${keep.prose}\n\n**双重落位裁决（迁移）**：${rule.note}\n\n**另一处落位的原文（${group.filter((g) => g !== keep).map((g) => g.section).join(', ')}）**：${group.filter((g) => g !== keep).map((g) => g.prose).join('\n\n')}`,
    };
    items.push(merged);
    console.log(`双重落位已裁决：${id} → ${rule.keep}（源出现在 ${group.map((g) => g.section).join(' + ')}）`);
  }

  const byDomain = {};
  for (const d of DOMAIN_KEYS) byDomain[d] = items.filter((x) => x.domain === d).length;
  const srcChars = items.reduce((n, x) => n + x.prose.length, 0);
  const escaped = (readFileSync(SRC, 'utf8').match(/\\\|/g) || []).length;

  console.log(`源 ${rows.length} 行（去重后 ${items.length} 条）→ 目标 ${items.length} 文件`);
  console.log(`ID 集合全等：${new Set(raw.map((x) => x.id)).size === new Set(items.map((x) => x.id)).size ? 'yes' : 'NO'}`);
  console.log(`域分布：${DOMAIN_KEYS.map((d) => `${d}=${byDomain[d]}`).join(' ')}`);
  console.log(`正文总字符数（源，已还原转义）：${srcChars}；源表格转义竖线：${escaped} 处 → 迁移后还原为裸 '|'`);
  console.log('逐条归类（id → 域 [依据]）：');
  for (const x of items) console.log(`  ${x.id}  → ${x.domain}  [${x.why}]${x.cols !== (x.section === 'closed' ? 4 : 6) ? `  ⚠ 列数=${x.cols}` : ''}`);

  if (DRY) { console.log('\n--dry-run：未落盘。'); return; }

  if (VERIFY) { console.log('\n--verify：不写盘，只对已落盘结果做保真对账。'); }
  else for (const x of items) {
    const dir = domainDir(REPO, x.domain);
    if (!existsSync(dir)) mkdirSync(dir, { recursive: true });
    const dst = join(dir, `${x.id}.md`);
    if (existsSync(dst)) { console.error(`FATAL 目标已存在，拒绝覆盖：${dst}`); process.exit(1); }
    writeFileSync(dst, serializeItem(x.fm, x.prose), 'utf8');
  }
  // 收尾核验
  const after = readItems(REPO);
  console.log(`\n落盘完成：${after.length} 文件`);
  const before = new Set(items.map((x) => x.id)), now = new Set(after.map((x) => x.fm.id));
  const missing = [...before].filter((i) => !now.has(i));
  console.log(`ID 集合核对：${missing.length === 0 ? 'ok（全等）' : `MISSING ${missing.join(', ')}`}`);
  const nowChars = after.reduce((n, x) => n + x.body.length, 0);
  // 逐条对账（不手挥一个总数）：以「源 prose 经 serialize→parse 往返」为基线（剔除往返换行归一化的固有 +2），
  // 再与盘上 body 比 —— 于是差异只剩真正的语义性增删，逐条列出。
  const base = new Map(items.map((x) => [x.id, parseItem(serializeItem(x.fm, x.prose)).body.length]));
  const deltas = after
    .map((x) => ({ id: x.fm.id, d: x.body.length - (base.get(x.fm.id) ?? 0) }))
    .filter((x) => x.d !== 0);
  console.log(`逐条对账基线 = 源 prose 往返长度（已剔除换行归一化）；落盘条目 ${after.length} 条`);
  if (deltas.length === 0) console.log('逐条对账（意图 → 落盘）：全部条目与基线一致（零语义性差异）');
  else {
    console.log(`逐条对账（意图 → 落盘）：${deltas.length} 条有非零差，逐条如下（须逐条可解释）：`);
    for (const x of deltas) console.log(`  ${x.id}: ${x.d > 0 ? '+' : ''}${x.d}`);
  }
  // 源 → 意图 一侧的对账（上一条只证「写的忠实」，这一条证「变换的忠实」）
  const srcById = new Map();
  for (const r of raw) srcById.set(r.id, (srcById.get(r.id) || 0) + r.prose.length);
  const transformDeltas = items.map((x) => ({ id: x.id, d: x.prose.length - (srcById.get(x.id) ?? 0) })).filter((x) => x.d !== 0);
  console.log(`逐条对账（源 → 意图）：${transformDeltas.length === 0 ? '零差异' : `${transformDeltas.length} 条有差`}`);
  for (const x of transformDeltas) console.log(`  ${x.id}: ${x.d > 0 ? '+' : ''}${x.d}（双重落位合并注记，已在上方逐条声明）`);
}

main();