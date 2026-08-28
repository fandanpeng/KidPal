#!/usr/bin/env node
// .planning/ reconcile 引擎：
//   1) 扫描顶层 workstream 目录 + _archive/**，读各自 task_plan.md 的 YAML front-matter。
//   2) 顶层中 status ∈ {done, abandoned} 的 workstream → 移到 _archive/<closed 年份>/<slug>/。
//   3) 重建 INDEX.md（人读）与 index.jsonl（S5/工具只读）。
// 设计：无外部依赖、幂等、可独立运行（node .claude/scripts/planning-index.mjs）。
// 由 .claude/hooks/planning-sync.mjs 在 PostToolUse(.planning/** 编辑后) 触发；亦可手动跑。

import {
  existsSync,
  mkdirSync,
  readFileSync,
  readdirSync,
  renameSync,
  writeFileSync,
} from 'node:fs';
import { resolve, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const PLANNING_DIRNAME = '.planning';
const RESERVED = new Set(['README.md', 'INDEX.md', 'index.jsonl', '_archive']);
const ACTIVE_STATUSES = new Set(['active', 'blocked']);
const ARCHIVE_STATUSES = new Set(['done', 'abandoned']);

function planningDir(projectDir) {
  return resolve(projectDir, PLANNING_DIRNAME);
}

// 极简 YAML front-matter 解析：只取顶层 `key: value`，忽略缩进的列表项（如 changes:）。
function parseFrontMatter(text) {
  const clean = text.replace(/^\uFEFF/, '');
  const lines = clean.split(/\r?\n/);
  if (lines[0].trim() !== '---') return {};
  const fm = {};
  for (let i = 1; i < lines.length; i++) {
    const line = lines[i];
    if (line.trim() === '---') break;
    if (!line.trim() || /^\s/.test(line)) continue; // 跳过空行与缩进行（列表项）
    const m = line.match(/^([A-Za-z0-9_]+):\s*(.*)$/);
    if (!m) continue;
    const key = m[1];
    let val = m[2].trim().replace(/^["']|["']$/g, '');
    if (val === '' || val === '—' || val === '-') {
      fm[key] = null;
    } else {
      fm[key] = val;
    }
  }
  return fm;
}

function readWorkstream(taskPlanPath, slugFallback) {
  let fm = {};
  try {
    fm = parseFrontMatter(readFileSync(taskPlanPath, 'utf8'));
  } catch {
    /* 无 front-matter 或读失败 → 当 active 处理 */
  }
  const slug = fm.slug || slugFallback;
  return {
    slug,
    workstream: fm.workstream || slug,
    status: (fm.status || 'active').toLowerCase(),
    opened: fm.opened || null,
    closed: fm.closed || null,
    summary: fm.summary || '',
    owner: fm.owner || null,
    area: fm.area || null,
  };
}

function yearOf(closed) {
  const m = /^(\d{4})/.exec(closed || '');
  return m ? m[1] : String(new Date().getFullYear());
}

function listDirs(dir) {
  if (!existsSync(dir)) return [];
  return readdirSync(dir, { withFileTypes: true })
    .filter((e) => e.isDirectory())
    .map((e) => e.name);
}

// 把顶层中 done/abandoned 的 workstream 搬进 _archive/<year>/。返回移动记录。
function archiveCompleted(root) {
  const moves = [];
  for (const name of listDirs(root)) {
    if (RESERVED.has(name)) continue;
    const src = join(root, name);
    const taskPlan = join(src, 'task_plan.md');
    if (!existsSync(taskPlan)) continue;
    const ws = readWorkstream(taskPlan, name);
    if (!ARCHIVE_STATUSES.has(ws.status)) continue;
    const year = yearOf(ws.closed);
    const destDir = join(root, '_archive', year);
    const dest = join(destDir, name);
    if (existsSync(dest)) continue; // 已有同名归档：不覆盖，留给人处理
    mkdirSync(destDir, { recursive: true });
    renameSync(src, dest);
    moves.push({ slug: ws.slug, from: name, to: `_archive/${year}/${name}` });
  }
  return moves;
}

// 收集全部 workstream（顶层 + _archive/**）。
function collect(root) {
  const items = [];
  for (const name of listDirs(root)) {
    if (RESERVED.has(name)) continue;
    const taskPlan = join(root, name, 'task_plan.md');
    if (!existsSync(taskPlan)) continue;
    items.push({ ...readWorkstream(taskPlan, name), path: name });
  }
  const archiveRoot = join(root, '_archive');
  for (const year of listDirs(archiveRoot)) {
    for (const name of listDirs(join(archiveRoot, year))) {
      const taskPlan = join(archiveRoot, year, name, 'task_plan.md');
      if (!existsSync(taskPlan)) continue;
      items.push({
        ...readWorkstream(taskPlan, name),
        path: `_archive/${year}/${name}`,
      });
    }
  }
  return items;
}

function esc(s) {
  return String(s == null ? '' : s).replace(/\|/g, '\\|');
}

function buildIndexMd(items, now) {
  const active = items
    .filter((i) => ACTIVE_STATUSES.has(i.status))
    .sort((a, b) => String(a.opened || '').localeCompare(String(b.opened || '')));
  const archived = items
    .filter((i) => ARCHIVE_STATUSES.has(i.status))
    .sort((a, b) => String(b.closed || '').localeCompare(String(a.closed || '')));

  const out = [];
  out.push('# .planning Index');
  out.push('');
  out.push(
    `> 自动生成（hook：编辑 .planning/ 即重建；标记 done 即归档）。**勿手改**。最后更新 ${now}。`,
  );
  out.push('');
  out.push(`## Active (${active.length})`);
  out.push('');
  if (active.length) {
    out.push('| slug | status | area | owner | opened | summary |');
    out.push('|---|---|---|---|---|---|');
    for (const i of active) {
      const status = i.status === 'blocked' ? '**blocked**' : 'active';
      out.push(
        `| [${esc(i.slug)}](${i.path}/task_plan.md) | ${status} | ${esc(i.area) || '—'} | ${esc(i.owner) || '—'} | ${esc(i.opened) || '—'} | ${esc(i.summary)} |`,
      );
    }
  } else {
    out.push('_（无）_');
  }
  out.push('');
  out.push(`## Archived (${archived.length})`);
  out.push('');
  if (archived.length) {
    out.push('| slug | status | area | closed | summary |');
    out.push('|---|---|---|---|---|');
    for (const i of archived) {
      out.push(
        `| [${esc(i.slug)}](${i.path}/task_plan.md) | ${esc(i.status)} | ${esc(i.area) || '—'} | ${esc(i.closed) || '—'} | ${esc(i.summary)} |`,
      );
    }
  } else {
    out.push('_（无）_');
  }
  out.push('');
  return out.join('\n');
}

function buildIndexJsonl(items) {
  if (!items.length) return '';
  return (
    items
      .map((i) =>
        JSON.stringify({
          slug: i.slug,
          status: i.status,
          area: i.area,
          owner: i.owner,
          opened: i.opened,
          closed: i.closed,
          path: i.path,
          summary: i.summary,
        }),
      )
      .join('\n') + '\n'
  );
}

export function reconcile(projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd()) {
  const root = planningDir(projectDir);
  if (!existsSync(root)) return { ok: true, skipped: true, reason: 'no .planning/' };
  const moves = archiveCompleted(root);
  const items = collect(root);
  const now = new Date().toISOString().slice(0, 10);
  writeFileSync(join(root, 'INDEX.md'), buildIndexMd(items, now), 'utf8');
  writeFileSync(join(root, 'index.jsonl'), buildIndexJsonl(items), 'utf8');
  return { ok: true, moves, count: items.length };
}

// 直接运行（node .claude/scripts/planning-index.mjs）时执行一次 reconcile。
const invokedDirectly =
  process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url);
if (invokedDirectly) {
  console.log(JSON.stringify(reconcile(), null, 2));
}
