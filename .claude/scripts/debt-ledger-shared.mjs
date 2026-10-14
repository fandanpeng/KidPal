// debt-ledger-shared.mjs — debt 账本（docs/debt/**）机制单源库。
//
// 260923-gov-review-arq-ssot-restructure（T2）落地：解析 / 序列化 / 字段闭集 / 域配置 /
// 排序规则 / G 层与硬排除推导，只此一份。消费方 = debt-render.mjs（渲染）/ debt-doctor.mjs
// （检查）/ crq-migrate.mjs（一次性迁移，retained 历史件）。禁各自另造副本。
//
// 形态：每条目一文件 docs/debt/<域>/<ID>.md = YAML frontmatter（机读状态）+ 正文（append-only 散文）。
// 纪律：改状态去改条目文件再重跑 debt-render.mjs；三份 INDEX.md 是生成件，勿手改。
import { readdirSync, readFileSync, existsSync, mkdirSync, renameSync } from 'node:fs';
import { resolve, join, dirname, basename } from 'node:path';

// ── 域配置（T3）：域 → 目录 / index / ID 前缀 / 额外字段 ──────────────────────────
export const DOMAINS = {
  gov:        { dir: 'gov',         prefix: 'ARQ',  extra: [] },
  tech:       { dir: 'tech',        prefix: 'TECH', extra: ['handoff', 'cdq'] },
  requirement:{ dir: 'requirement', prefix: 'REQ',  extra: ['affects', 'decision', 'landedIn'] },
};
export const DOMAIN_KEYS = Object.keys(DOMAINS);
export const LEDGER_ROOT = ['docs', 'debt'];

export function domainOfPrefix(prefix) {
  for (const [k, v] of Object.entries(DOMAINS)) if (v.prefix === prefix) return k;
  return null;
}

// ── 闭集 ─────────────────────────────────────────────────────────────────────
export const STATUSES = ['blocker', 'deferred', 'accepted-risk', 'closed'];
// severity 闭集 = **实况并集**（起草期实测，见下），非凭印象：
//   rubric（arch-review-rubric.md，gov-arq skill 引其五档）= BLOCKING / SHOULD / NIT / OPTIONAL / FYI；
//   但源队列 29 条未结实况 = SHOULD 23 / COULD 4 / NIT 1 / MAJOR 1 —— **COULD 与 MAJOR 不在 rubric 五档内**。
//   故闭集取并集（迁移不造数据、不静默改档），并把「非 rubric 档位」做成 advisory 计数，
//   把这条「队列实际用词 ≠ 自己引用的 rubric」的漂移显式留在册（另见 §6 留根）。
export const SEVERITIES = ['BLOCKING', 'MAJOR', 'SHOULD', 'COULD', 'NIT', 'OPTIONAL', 'FYI'];
export const RUBRIC_SEVERITIES = ['BLOCKING', 'SHOULD', 'NIT', 'OPTIONAL', 'FYI'];
export const ID_RE = /^(ARQ|TECH|REQ)-\d{6}-[a-z0-9-]+$/;
// ID 前缀 → 域。注意 `ARQ`：它是**祖辈前缀**（95 条在册，改名要动全仓引用且无收益）——
//   规则 = 「新条目用本域前缀」+「`ARQ-` 在任何域都放行（历史词汇，告警计数）」。
//   把 `ARQ-` 硬绑 gov 会与本批「迁移不改名」直接冲突（本批起草期由 doctor 抓出该自相矛盾）。
export const PREFIX_TO_DOMAIN = { ARQ: 'gov', TECH: 'tech', REQ: 'requirement' };
export const GRANDFATHER_PREFIX = 'ARQ';

// §1.1/§1.2/§1.3/§2 四节 ↘ status 的对应（T2 改法 2：分节由 status 派生）
// 「待触发」= 旧队列 §1.2 Deferred/forcing-function 的自明化改名（human 2026-09-24 问「顺延代表啥」——词不自明即改）
export const STATUS_SECTIONS = [
  ['blocker', '待拍板'],
  ['deferred', '待触发'],
  ['accepted-risk', '已接受风险'],
  ['closed', '已结索引'],
];

export const REQUIRED_FIELDS = ['id', 'title', 'domain', 'status', 'severity', 'opened'];

// ── G 层 / 机检硬排除 推导（T3 改法 5：6 个布尔由 1 个 paths[] 推） ──────────────
export const GATE_LAYER_RULES = [
  ['G1', /^(AGENTS\.md|CLAUDE\.md|README\.md|docs\/DEV-GOVERNANCE\.md)$/],
  ['G2', /^\.claude\/(agents|commands|references|rules|skills)\//],
  ['G3', /^(\.claude\/(scope-map\.json|hooks\/|scripts\/|[^/]*\.settings\.json)|\.claude\/verify-gates\.json)/],
  ['G4', /^delivery\/(changes\/|taskpackages\/|workstreams\/)/],
];

export function gateLayersOf(paths) {
  const hit = new Set();
  for (const p of paths || []) for (const [layer, re] of GATE_LAYER_RULES) if (re.test(p)) hit.add(layer);
  return [...hit].sort();
}

// 机检硬排除（gov-arq skill 步骤 3 的六个布尔）——由路径推，不靠人填
export function hardExclusionsOf(paths) {
  const ps = paths || [];
  return {
    contracts: ps.some((p) => p.startsWith('docs/contracts/')),
    agentFrontmatter: ps.some((p) => /^\.claude\/agents\/[^/]+\.md$/.test(p)),
    selfDef: ps.some((p) => p === '.claude/agents/architect.md'),
    reviewLoopAgentDef: ps.some((p) => /^\.claude\/agents\/(architect-reviewer|review-moderator|scenario-author|challenger|defender|adjudicator|landing-verifier)\.md$/.test(p)),
    newGate: ps.some((p) => /^(\.claude\/(hooks|verify-gates\.json)|\.claude\/scripts\/.*gate.*\.mjs)/.test(p)),
    specContractDrift: ps.some((p) => p.startsWith('docs/contracts/')),
  };
}

// ── 解析 / 序列化 ────────────────────────────────────────────────────────────
// 极简 YAML 子集：`key: value` / `key: [a, b]` / 空值。够本 schema 用，不引依赖。
function parseScalar(raw) {
  const s = raw.trim();
  if (s === '' || s === '[]') return s === '[]' ? [] : '';
  if (s.startsWith('[') && s.endsWith(']')) {
    const inner = s.slice(1, -1).trim();
    if (!inner) return [];
    return inner.split(',').map((x) => x.trim().replace(/^["']|["']$/g, '')).filter(Boolean);
  }
  return s.replace(/^["']|["']$/g, '');
}

export function parseItem(text) {
  const m = /^---\r?\n([\s\S]*?)\r?\n---\r?\n?([\s\S]*)$/.exec(text);
  if (!m) return { fm: {}, body: text, wellFormed: false };
  const fm = {};
  for (const line of m[1].split(/\r?\n/)) {
    if (!line.trim() || line.trim().startsWith('#')) continue;
    const i = line.indexOf(':');
    if (i < 0) continue;
    fm[line.slice(0, i).trim()] = parseScalar(line.slice(i + 1));
  }
  return { fm, body: m[2], wellFormed: true };
}

const NEEDS_QUOTE = /[:#\[\]{}]|^\s|\s$|^$/;
function fmtScalar(v) {
  if (Array.isArray(v)) return `[${v.join(', ')}]`;
  const s = String(v ?? '');
  if (s === '') return '""';
  return NEEDS_QUOTE.test(s) ? `"${s.replace(/"/g, '\\"')}"` : s;
}

// 字段输出顺序钉死（确定性渲染的前提之一）
const FM_ORDER = ['id', 'title', 'domain', 'status', 'severity', 'owner', 'trigger', 'opened', 'closed', 'closedBy', 'paths', 'source', 'handoff', 'cdq', 'affects', 'decision', 'landedIn'];

export function serializeItem(fm, body) {
  const keys = [...FM_ORDER.filter((k) => k in fm), ...Object.keys(fm).filter((k) => !FM_ORDER.includes(k)).sort()];
  const head = keys.map((k) => `${k}: ${fmtScalar(fm[k])}`).join('\n');
  return `---\n${head}\n---\n\n${String(body).replace(/^\n+/, '').replace(/\s*$/, '')}\n`;
}

// ── 读取账本 ────────────────────────────────────────────────────────────────
export function ledgerDir(repoRoot) { return resolve(repoRoot, ...LEDGER_ROOT); }
export function domainDir(repoRoot, domain) { return join(ledgerDir(repoRoot), DOMAINS[domain].dir); }
export function indexFile(repoRoot, domain) { return join(domainDir(repoRoot, domain), 'INDEX.md'); }

// ── 归档（位置 = 状态的纯函数；搬文件只有脚本干，人永远不搬） ─────────────────
// 判据：status=closed 且 closed 带年份 → `_archive/<年>/<ID>.md`（镜像 delivery/workstreams/_archive/<年>/ 先例）；
// 其余（未结 / closed 但无日期）→ 域顶层。重开条目由同一函数算回顶层——双向、幂等。
export const ARCHIVE_DIR = '_archive';

export function expectedRel(fm) {
  if (String(fm.status) === 'closed') {
    const m = /^(\d{4})-/.exec(String(fm.closed || ''));
    if (m) return `${ARCHIVE_DIR}/${m[1]}/${fm.id}.md`;
  }
  return `${fm.id}.md`;
}

export function archiveIndexRel(year) { return `${ARCHIVE_DIR}/${year}/INDEX.md`; }

/** 纯函数：算出所有「现位置 ≠ 应位置」的搬迁（不落盘，供 render 应用与 self-check 夹具）。 */
export function computeRelocations(items) {
  const moves = [];
  for (const it of items) {
    if (!it.wellFormed || !it.fm.id) continue;
    const want = expectedRel(it.fm);
    if (want !== it.rel) moves.push({ domain: it.domain, id: it.fm.id, from: it.rel, to: want });
  }
  return moves;
}

export function applyRelocations(repoRoot, moves) {
  for (const mv of moves) {
    const src = join(domainDir(repoRoot, mv.domain), mv.from);
    const dst = join(domainDir(repoRoot, mv.domain), mv.to);
    mkdirSync(dirname(dst), { recursive: true });
    renameSync(src, dst);
  }
}

export function readItems(repoRoot, { includeBroken = true } = {}) {
  const items = [];
  const push = (domain, absPath, rel) => {
    const text = readFileSync(absPath, 'utf8');
    const { fm, body, wellFormed } = parseItem(text);
    if (!wellFormed && !includeBroken) return;
    items.push({ domain, file: basename(absPath), rel, path: absPath, fm, body, wellFormed });
  };
  for (const domain of DOMAIN_KEYS) {
    const dir = domainDir(repoRoot, domain);
    if (!existsSync(dir)) continue;
    for (const name of readdirSync(dir).sort()) {
      const p = join(dir, name);
      if (name === ARCHIVE_DIR) {
        for (const year of readdirSync(p).sort()) {
          const ydir = join(p, year);
          for (const f of readdirSync(ydir).sort()) {
            if (f.endsWith('.md') && f !== 'INDEX.md') push(domain, join(ydir, f), `${ARCHIVE_DIR}/${year}/${f}`);
          }
        }
        continue;
      }
      if (!name.endsWith('.md') || name === 'INDEX.md') continue;
      push(domain, p, name);
    }
  }
  return items;
}

// ── 渲染确定性（T2-R5，承重） ─────────────────────────────────────────────────
// 未结按 opened 升序、同日期按 ID 升序；已结按 closed 升序、同日期按 ID 升序。
// 升序是为合并友好而选：新条目落节尾 ⇒ 两分支 diff 是「各自末尾追加」⇒ union 合并几乎无冲突。
export function sortForRender(items) {
  const ord = (x) => (x.fm.status === 'closed' ? String(x.fm.closed || '') : String(x.fm.opened || ''));
  return [...items].sort((a, b) => {
    const ka = ord(a), kb = ord(b);
    if (ka !== kb) return ka < kb ? -1 : 1;
    return String(a.fm.id) < String(b.fm.id) ? -1 : 1;
  });
}

export const GENERATED_MARKER = '<!-- GENERATED by .claude/scripts/debt-render.mjs — 勿手改。改状态去改条目文件再重跑渲染。 -->';

// ── 检查（doctor 消费） ──────────────────────────────────────────────────────
export function checkItems(items) {
  const red = [], advisory = [];
  const seen = new Map();
  for (const it of items) {
    const { fm } = it;
    const where = `${it.domain}/${it.file}`;
    if (!it.wellFormed) { red.push({ at: where, code: 'no-frontmatter', msg: '文件无 frontmatter 块' }); continue; }
    for (const f of REQUIRED_FIELDS) {
      if (f === 'severity' && String(fm.status) === 'closed') continue; // §2 已结行无 severity 列，迁移不造数据
      if (!(f in fm) || fm[f] === '') red.push({ at: where, code: 'missing-field', msg: `缺必填字段 ${f}` });
    }
    if (fm.id) {
      if (!ID_RE.test(String(fm.id))) red.push({ at: where, code: 'bad-id', msg: `ID 形态不合规：${fm.id}` });
      else if (String(fm.id) !== it.file.replace(/\.md$/, '')) red.push({ at: where, code: 'id-filename-mismatch', msg: 'ID 与文件名不一致' });
      if (seen.has(fm.id)) red.push({ at: where, code: 'duplicate-id', msg: `ID 重复（另见 ${seen.get(fm.id)}）` });
      else seen.set(fm.id, where);
      const m = /^([A-Z]+)-/.exec(String(fm.id));
      if (m) {
        const owner = PREFIX_TO_DOMAIN[m[1]];
        if (!owner) red.push({ at: where, code: 'unknown-prefix', msg: `未知 ID 前缀：${m[1]}` });
        else if (owner !== it.domain) {
          if (m[1] === GRANDFATHER_PREFIX) {
            advisory.push({ at: where, code: 'legacy-prefix-off-domain', msg: `祖辈前缀 ${m[1]} 出现在 ${it.domain} 域（迁移不改名；新条目须用本域前缀）` });
          } else {
            red.push({ at: where, code: 'prefix-domain-mismatch', msg: `前缀 ${m[1]} 属 ${owner}，条目却在 ${it.domain}` });
          }
        }
      }
    }
    if (fm.domain && fm.domain !== it.domain) red.push({ at: where, code: 'domain-dir-mismatch', msg: `domain=${fm.domain} 与所在目录 ${it.domain} 不符` });
    if (fm.status && !STATUSES.includes(String(fm.status))) red.push({ at: where, code: 'bad-status', msg: `status 出闭集：${fm.status}` });
    if (fm.severity) {
      if (!SEVERITIES.includes(String(fm.severity))) red.push({ at: where, code: 'bad-severity', msg: `severity 出闭集：${fm.severity}` });
      else if (!RUBRIC_SEVERITIES.includes(String(fm.severity))) advisory.push({ at: where, code: 'severity-off-rubric', msg: `severity=${fm.severity} 不在 rubric 五档内（队列实况用词，待归一）` });
    }
    // 内容性待补 —— 告警级，不阻断（迁移项普遍命中）
    if (it.domain === 'tech' && !fm.handoff) advisory.push({ at: where, code: 'tech-no-handoff', msg: 'tech 条目未填 handoff 去向' });
    if ((!fm.paths || fm.paths.length === 0)) advisory.push({ at: where, code: 'no-paths', msg: '未申报 paths[]，G 层不可机判' });
    if (String(fm.status) === 'closed' && !/^\d{4}-/.test(String(fm.closed || ''))) advisory.push({ at: where, code: 'closed-no-date', msg: '已结但无 closed 日期——无法按年归档' });
    // 位置 == f(状态)：归档归位由渲染器执行，人搬错/漏跑渲染都会在此现形
    if (it.rel && fm.id && fm.status) {
      const want = expectedRel(fm);
      if (want !== it.rel) red.push({ at: where, code: 'mislocated', msg: `位置应为 ${want}（现 ${it.rel}）——跑 debt-render.mjs 自动归位` });
    }
  }
  return { red, advisory };
}