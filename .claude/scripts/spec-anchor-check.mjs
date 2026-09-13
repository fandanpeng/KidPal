#!/usr/bin/env node
// spec-anchor-check.mjs — requirements.md 锚定机检（260913-arch-review-spec-split T3）
//
// 正向三检（默认）：
//   ① 支撑锚 arch §x.y → architecture.md 真实存在的标题（悬空 = 报错）；
//   ② story 锚行的 UI 锚 → docs/ui/ 现行设计稿真实存在的 data-od-id；
//   ③ 可行性 🔴 必须同行挂开放问题/ARQ 引用。
// --reverse：反向视图（arch 小节 → 引用它的 story/NFR/底座清单）。反向视图只由本脚本生成，不手写双向维护。
// --diff-ids <旧设计稿> <新设计稿>：两版 data-od-id 全量 diff（设计稿迭代断锚前置检查，
//   配合 write-story skill「锚行规则」第 5 条；被 story 引用而消失的 id 显式点名）。
//
// 拦阻强度：手动 / review 时跑（OQ-4 裁 (a)），不挂 hook。
// 真空绿规约：先断言两文件非空 + story 计数 > 0 + 锚行计数 > 0，再谈检查通过。
//
// 用法：
//   node .claude/scripts/spec-anchor-check.mjs
//   node .claude/scripts/spec-anchor-check.mjs --reverse
//   node .claude/scripts/spec-anchor-check.mjs --diff-ids docs/ui/kidpal-ui-design-v3.html docs/ui/kidpal-ui-design-v4.html
//
// 退出码：0 全绿 / 1 有悬空或违规 / 2 用法或输入缺失。

import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';

const root = resolve(process.env.CLAUDE_PROJECT_DIR || process.cwd());
const REQ_PATH = join(root, 'docs', 'requirements.md');
const ARCH_PATH = join(root, 'docs', 'architecture.md');
const UI_DIR = join(root, 'docs', 'ui');

function extractOdIds(html) {
  const ids = new Set();
  for (const m of html.matchAll(/data-od-id="([^"]+)"/g)) ids.add(m[1]);
  return ids;
}

/** 现行设计稿 = docs/ui/ 下版本号最大的 kidpal-ui-design-vN.html。 */
function currentDesignFile() {
  if (!existsSync(UI_DIR)) return null;
  const files = readdirSync(UI_DIR).filter((f) => /^kidpal-ui-design-v\d+\.html$/.test(f));
  if (!files.length) return null;
  files.sort((a, b) => Number(b.match(/v(\d+)/)[1]) - Number(a.match(/v(\d+)/)[1]));
  return files[0];
}

/** architecture.md 的编号小节标题集合：{ '2.4': '端云职责划分…', … } */
function parseArchHeadings(text) {
  const map = new Map();
  for (const m of text.matchAll(/^#{2,4}\s+(\d+(?:\.\d+)*)\.?\s+(.+)$/gm)) map.set(m[1], m[2].trim());
  return map;
}

/** 逐行扫描 requirements.md 的锚行（**锚**…）与 NFR 支撑行（支撑：arch §…）。 */
function scanAnchorLines(reqText) {
  const rows = [];
  let currentRef = '(文件头部)';
  reqText.split('\n').forEach((line, i) => {
    const h = line.match(/^#+\s+(S-F\d+\.\d+|NFR-[A-Z]+-\d+)/);
    if (h) currentRef = h[1];
    if (/^##\s+2\.\s/.test(line)) currentRef = '底座（§2）';
    const supportMatch = line.match(/支撑[锚]?\s*[=：:]\s*([^｜|]*)/);
    const hasSupport = !!supportMatch;
    const hasUi = /UI\s*=\s*/.test(line);
    const hasFeasibility = /可行性\s*[=：:]/.test(line);
    if (!hasSupport && !hasUi && !hasFeasibility) return;
    const supportSeg = supportMatch?.[1] ?? '';
    const uiSeg = hasUi ? (line.match(/UI\s*=\s*([^｜|]*)/)?.[1] ?? '') : '';
    const archRefs = [...supportSeg.matchAll(/§(\d+(?:\.\d+)*)/g)].map((m) => m[1]);
    // UI id 只认小写连字符实物 id（`canvas-pX` 这类占位大写 X 天然不匹配）
    const uiIds = [...uiSeg.matchAll(/`([a-z0-9][a-z0-9-]*)`/g)].map((m) => m[1]);
    rows.push({
      lineNo: i + 1,
      ref: currentRef,
      archRefs,
      uiIds,
      red: /可行性\s*[=：:]\s*🔴/.test(line),
      accounted: /ARQ-\d|开放问题/.test(line),
      raw: line.trim(),
    });
  });
  return rows;
}

// ---- --diff-ids ----
function diffIds(oldPath, newPath) {
  const a = resolve(root, oldPath);
  const b = resolve(root, newPath);
  if (!existsSync(a) || !existsSync(b)) {
    process.stderr.write(`[spec-anchor-check] FATAL: 设计稿不存在（${oldPath} / ${newPath}）\n`);
    process.exit(2);
  }
  const oldIds = extractOdIds(readFileSync(a, 'utf8'));
  const newIds = extractOdIds(readFileSync(b, 'utf8'));
  const removed = [...oldIds].filter((x) => !newIds.has(x));
  const added = [...newIds].filter((x) => !oldIds.has(x));
  let reqText = '';
  if (existsSync(REQ_PATH)) reqText = readFileSync(REQ_PATH, 'utf8');
  const removedReferenced = removed.filter((id) => reqText.includes(`\`${id}\``));
  process.stdout.write(
    `${JSON.stringify(
      {
        diffIds: { old: oldPath, new: newPath, oldCount: oldIds.size, newCount: newIds.size },
        removed,
        added,
        removedReferencedByRequirements: removedReferenced,
        verdict: removedReferenced.length === 0 ? 'OK（无被 story 引用的断锚）' : 'BROKEN-ANCHORS（下列 id 被 requirements.md 引用但新稿已消失，须显式处置）',
      },
      null,
      2,
    )}\n`,
  );
  process.exit(removedReferenced.length ? 1 : 0);
}

// ---- 主流程 ----
const args = process.argv.slice(2);
if (args.includes('--diff-ids')) {
  const i = args.indexOf('--diff-ids');
  if (!args[i + 1] || !args[i + 2]) {
    process.stderr.write('[spec-anchor-check] FATAL: --diff-ids 需要 <旧设计稿> <新设计稿> 两个路径\n');
    process.exit(2);
  }
  diffIds(args[i + 1], args[i + 2]);
}

if (!existsSync(REQ_PATH) || !existsSync(ARCH_PATH)) {
  process.stderr.write('[spec-anchor-check] FATAL: docs/requirements.md 或 docs/architecture.md 不存在\n');
  process.exit(2);
}
const reqText = readFileSync(REQ_PATH, 'utf8');
const archText = readFileSync(ARCH_PATH, 'utf8');

// 真空绿规约：先断言输入非空 + story 计数 > 0 + 锚行计数 > 0
const storyCount = (reqText.match(/^#+\s+S-F\d+\.\d+/gm) || []).length;
const archMap = parseArchHeadings(archText);
const rows = scanAnchorLines(reqText);
const structuralFailures = [];
if (reqText.trim().length === 0) structuralFailures.push('requirements.md 为空');
if (archText.trim().length === 0) structuralFailures.push('architecture.md 为空');
if (storyCount === 0) structuralFailures.push('requirements.md story 计数 = 0');
if (archMap.size === 0) structuralFailures.push('architecture.md 编号小节计数 = 0');
if (rows.length === 0) structuralFailures.push('requirements.md 锚行计数 = 0');
if (structuralFailures.length) {
  process.stderr.write(`[spec-anchor-check] FATAL（真空绿规约）: ${structuralFailures.join('；')}\n`);
  process.exit(2);
}

const designFile = currentDesignFile();
const designIds = designFile ? extractOdIds(readFileSync(join(UI_DIR, designFile), 'utf8')) : new Set();

if (args.includes('--reverse')) {
  const reverse = new Map();
  for (const r of rows) {
    for (const sec of r.archRefs) {
      if (!reverse.has(sec)) reverse.set(sec, []);
      reverse.get(sec).push(r.ref);
    }
  }
  const out = [...reverse.entries()]
    .sort((a, b) => a[0].localeCompare(b[0], undefined, { numeric: true }))
    .map(([sec, refs]) => ({
      archSection: `§${sec}`,
      archTitle: archMap.get(sec) ?? '(悬空!)',
      referencedBy: [...new Set(refs)],
    }));
  process.stdout.write(`${JSON.stringify({ reverse: out, storyCount, anchorLines: rows.length }, null, 2)}\n`);
  process.exit(out.length ? 0 : 1);
}

// 正向三检
const errors = [];
for (const r of rows) {
  for (const sec of r.archRefs) {
    if (!archMap.has(sec)) errors.push(`${r.ref}（L${r.lineNo}）：支撑锚 arch §${sec} 在 architecture.md 无对应标题`);
  }
  for (const id of r.uiIds) {
    if (!designIds.has(id)) errors.push(`${r.ref}（L${r.lineNo}）：UI 锚 \`${id}\` 在现行设计稿 ${designFile ?? '(缺失!)'} 无对应 data-od-id`);
  }
  if (r.red && !r.accounted) errors.push(`${r.ref}（L${r.lineNo}）：可行性 🔴 未同行挂开放问题/ARQ 引用`);
}

process.stdout.write(
  `${JSON.stringify(
    {
      specAnchorCheck: 'forward',
      design: designFile,
      designIdCount: designIds.size,
      archSectionCount: archMap.size,
      storyCount,
      anchorLines: rows.length,
      errors,
      ok: errors.length === 0,
    },
    null,
    2,
  )}\n`,
);
process.exit(errors.length ? 1 : 0);
