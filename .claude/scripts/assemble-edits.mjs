#!/usr/bin/env node
// 锚文装配器（assemble-edits）——治理批起草期共享沙盘装配器（261027-gov-review-draft-cost-relief T1 · ARQ-261027）
//
// 补的缺口：architect 起草 PROPOSAL 但不落 canonical、§2 又须逐字节承诺改法，于是 P5 / P6 每批现写一次性
// 装配脚手架、批后即删；P6 版按行号降序替换 / 插入、每处先核首末行锚文——上游一动即漂移中止、清单须重排。
// 本工具以「旧串锚定」替代行号基线——
// 清单逐条（file / oldStr / newStr / label）断言 oldStr 在现行磁盘内容中恰命中 1 次，多文件顺序施加，
// 落仓外副本并回报 sha256。旧串 = §2 的逐字引文，天然抗漂移：上游漂移 → 0 命中 → fail-loud 中止。
//
// 用法（cwd = 仓根；仓根 = CLAUDE_PROJECT_DIR || cwd，与仓内其它脚本同口径）：
//   node .claude/scripts/assemble-edits.mjs <编辑清单.json> <仓外输出根>
//   node .claude/scripts/assemble-edits.mjs --self-check
// 编辑清单（JSON）：
//   { "edits": [ { "file": "<仓根相对路径>", "label": "<人读名>", "oldStr": "…", "newStr": "…" } ],
//     "copy":  [ "<仓根相对路径>" | { "from": "<仓根相对路径>", "to": "<仓根相对路径>，可省 = from" } ] }
//   copy 可省：原样复制的伴随文件（如脚本 import 的 lib/、_scratch 里的新文件原型），让副本可跑。
//
// 语义：
//   1. 逐条断言恰 1 次命中：0 次（上游漂移 / 清单陈旧）与多次（锚串太短）都中止；全部断言通过才写盘——
//      副本不出现半装配态。
//   2. 顺序施加：同一文件多条按清单顺序，后一条的 oldStr 在前一条的结果上查找（链式编辑可用）。
//   3. 字符串操作保行尾原样（不做 LF/CRLF 归一、不剥 BOM）；插入行 = oldStr 取锚行、newStr = 锚行 + 新文本。
//   4. 输出根必须在仓外（packet-bound 起草副本不落 canonical 的机械兜底）——指向仓内直接 exit 2；仓界判定先对最深
//      存在祖先 realpath（符号链接 / junction 不构成绕行，见 isInsideRepo 注）。
// 退出码：0 全部命中且写盘成功 / 1 断言失败（未写任何文件）/ 2 用法或输入错误（含输出根在仓内）。

import { existsSync, mkdirSync, mkdtempSync, readFileSync, realpathSync, rmSync, symlinkSync, unlinkSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { basename, dirname, isAbsolute, join, relative, resolve } from 'node:path';
import { tmpdir } from 'node:os';

const projectDir = resolve(process.env.CLAUDE_PROJECT_DIR || process.cwd());

const sha256 = (s) => createHash('sha256').update(s, 'utf8').digest('hex');
const countOccurrences = (hay, needle) => hay.split(needle).length - 1;
// 0 命中且目标文件为 CRLF、清单为 LF 时给出诊断——本工具不归一（语义 3），提示而不是代偿。
const eolHint = (content, oldStr) => (oldStr.includes('\n') && !oldStr.includes('\r\n') && content.includes('\r\n')
  ? '（目标文件含 CRLF、清单为 LF——本工具不归一，请按现行行尾写清单）' : '');

// 仓内判定（261027 批 R1-F2 加固）：先对「最深存在祖先」做 realpath 再判，最后拼回不存在的尾部。
// resolve + relative 不解析符号链接 / junction——蓄意构造的 junction 可让仓内路径被判「仓外」（R1 实测两向：
// 仓内 junction 被放行、反向 junction 被当输出根）；realpath 之后链接面不再有绕法（自检含 junction 断言）。
function realpathDeepest(p) {
  let cur = resolve(p);
  const tail = [];
  while (!existsSync(cur)) {
    const parent = dirname(cur);
    if (parent === cur) return cur;
    tail.unshift(basename(cur));
    cur = parent;
  }
  return join(realpathSync(cur), ...tail);
}

function isInsideRepo(abs, repoRoot) {
  const rel = relative(realpathDeepest(repoRoot), realpathDeepest(abs));
  return rel === '' || (!rel.startsWith('..') && !isAbsolute(rel));
}

// 纯函数装配（不写盘）：返回 { ok, plan, failures }；任一断言失败 → plan 恒空（中止语义）。
function assemble(list, repoRoot, readFile = (p) => readFileSync(p, 'utf8')) {
  const failures = [];
  const entries = new Map();
  const edits = list && Array.isArray(list.edits) ? list.edits : [];
  const copyList = Array.isArray(list && list.copy) ? list.copy : [];
  if (!edits.length && !copyList.length) failures.push({ kind: 'input', message: '清单缺 edits 与 copy（至少一件）' });
  edits.forEach((e, i) => {
    const label = (e && e.label) || `#${i + 1}`;
    const rel = e && e.file;
    if (typeof rel !== 'string' || !rel || isAbsolute(rel) || !isInsideRepo(resolve(repoRoot, rel), repoRoot)) {
      failures.push({ kind: 'input', index: i, label, message: 'file 须为仓根内相对路径' });
      return;
    }
    if (typeof e.oldStr !== 'string' || !e.oldStr) { failures.push({ kind: 'input', index: i, label, file: rel, message: 'oldStr 须为非空字符串' }); return; }
    if (typeof e.newStr !== 'string') { failures.push({ kind: 'input', index: i, label, file: rel, message: 'newStr 须为字符串' }); return; }
    let entry = entries.get(rel);
    if (!entry) {
      const abs = resolve(repoRoot, rel);
      if (!existsSync(abs)) { failures.push({ kind: 'missing-file', index: i, label, file: rel, message: '目标文件不存在' }); return; }
      const text = readFile(abs);
      entry = { before: text, content: text, labels: [] };
      entries.set(rel, entry);
    }
    const n = countOccurrences(entry.content, e.oldStr);
    if (n !== 1) {
      const why = n === 0 ? 'oldStr 命中 0 次（上游漂移或清单陈旧）' : `oldStr 命中 ${n} 次（须恰 1 次：加长锚串）`;
      failures.push({ kind: 'anchor-count', index: i, label, file: rel, count: n, message: why + eolHint(entry.content, e.oldStr) });
      return;
    }
    // 函数式替换：newStr 中的 $& / $1 等按字面落盘（字符串式 replace 会把它们当替换模式解释）。
    entry.content = entry.content.replace(e.oldStr, () => e.newStr);
    entry.labels.push(label);
  });
  const copies = [];
  for (const c of copyList) {
    const from = typeof c === 'string' ? c : c && c.from;
    const to = typeof c === 'string' ? c : (c && c.to) || c.from;
    const bad = (p) => typeof p !== 'string' || !p || isAbsolute(p) || !isInsideRepo(resolve(repoRoot, p), repoRoot);
    if (bad(from) || bad(to)) { failures.push({ kind: 'input', copy: c, message: 'copy 条目须为仓根内相对路径' }); continue; }
    const abs = resolve(repoRoot, from);
    if (!existsSync(abs)) { failures.push({ kind: 'missing-file', file: from, message: 'copy 源文件不存在' }); continue; }
    copies.push({ rel: to, text: readFile(abs) });
  }
  if (failures.length) return { ok: false, plan: [], failures };
  const plan = [...entries].map(([rel, x]) => ({ rel, before: x.before, after: x.content, labels: x.labels }))
    .concat(copies.map((c) => ({ rel: c.rel, before: c.text, after: c.text, labels: ['copy'] })));
  return { ok: true, plan, failures: [] };
}

function writePlan(plan, outRoot) {
  return plan.map((p) => {
    const abs = resolve(outRoot, p.rel);
    mkdirSync(dirname(abs), { recursive: true });
    writeFileSync(abs, p.after, 'utf8');
    return { path: p.rel, edits: p.labels, beforeSha256: sha256(p.before), afterSha256: sha256(p.after), afterLines: p.after.split('\n').length };
  });
}

// 自检（判别性 = 反转实现必须变红）：0 命中 / 多命中中止；成功拷贝 byte 等值；链式顺序；插入形态；
// CRLF 原样（反转 = 归一 → 行尾断言红）；$& 字面（反转 = 字符串式 replace → 字面断言红）；失败即零写盘；
// copy{from,to}；输出根仓内判定；真实路径（不存在尾段拼回 + junction 指仓内须拦）；CRLF 诊断句。
function runAssembleSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'assemble-edits', label, got, want })); }
  };
  const root = mkdtempSync(join(tmpdir(), 'assemble-edits-'));
  const repo = join(root, 'repo');
  mkdirSync(join(repo, 'sub'), { recursive: true });
  writeFileSync(join(repo, 'a.txt'), '头\n锚行\n尾\n', 'utf8');
  writeFileSync(join(repo, 'b.txt'), '重复\n重复\n', 'utf8');
  writeFileSync(join(repo, 'crlf.txt'), '甲\r\n乙\r\n丙\r\n', 'utf8');
  writeFileSync(join(repo, 'sub', 'src.mjs'), '导出甲\n', 'utf8');
  const one = (file, oldStr, newStr) => assemble({ edits: [{ file, oldStr, newStr }] }, repo);
  const after1 = (r) => (r.plan[0] || {}).after;
  const fail1 = (r) => r.failures[0] || {};
  check('0命中·中止', fail1(one('a.txt', '不存在的串', 'x')).count, 0);
  check('多命中·中止', fail1(one('b.txt', '重复', 'x')).count, 2);
  const okRes = one('a.txt', '锚行', '锚行');
  check('成功·ok', okRes.ok, true);
  check('成功·byte 等值', after1(okRes), '头\n锚行\n尾\n');
  const chain = assemble({ edits: [
    { file: 'a.txt', oldStr: '锚行', newStr: '锚行·甲' },
    { file: 'a.txt', oldStr: '锚行·甲\n尾', newStr: '链\n尾' },
  ] }, repo);
  check('链式·第二条在前一条结果上查找', after1(chain), '头\n链\n尾\n');
  const ins = one('a.txt', '锚行', '锚行\n新增行');
  check('插入形态·锚行保留且恰 1 次', countOccurrences(after1(ins) || '', '锚行'), 1);
  check('插入形态·新行落位', after1(ins), '头\n锚行\n新增行\n尾\n');
  const crlf = one('crlf.txt', '乙\r\n', '乙\r\n新增\r\n');
  check('CRLF·行尾原样（CRLF 计数 = 纯 LF 计数 = 4）', JSON.stringify([countOccurrences(after1(crlf) || '', '\r\n'), countOccurrences(after1(crlf) || '', '\n')]), JSON.stringify([4, 4]));
  const dollar = one('a.txt', '锚行', '锚行$&$1');
  check('$& 字面落盘', after1(dollar), '头\n锚行$&$1\n尾\n');
  const mixed = assemble({ edits: [{ file: 'a.txt', oldStr: '锚行', newStr: 'X' }, { file: 'b.txt', oldStr: '无', newStr: 'y' }] }, repo);
  check('任一失败·中止且零写盘计划', JSON.stringify([mixed.ok, mixed.plan.length]), JSON.stringify([false, 0]));
  const cp = assemble({ edits: [{ file: 'a.txt', oldStr: '头', newStr: '首' }], copy: [{ from: 'sub/src.mjs', to: 'sub/dst.mjs' }] }, repo);
  check('copy{from,to}·同字节', (cp.plan.find((p) => p.rel === 'sub/dst.mjs') || {}).after, '导出甲\n');
  check('输出根仓内·拦截', isInsideRepo(join(repo, 'out'), repo), true);
  check('输出根仓外·放行', isInsideRepo(join(root, 'out'), repo), false);
  // 真实路径（261027 批 R1-F2）：① 不存在尾段须拼回最深存在祖先；② 指向仓内的 junction 不得被当「仓外」。
  check('真实路径·不存在尾段拼回最深存在祖先', realpathDeepest(join(root, 'no', 'such', 'x')), join(realpathSync(root), 'no', 'such', 'x'));
  let linked = true;
  try { symlinkSync(repo, join(root, 'link'), 'junction'); } catch { linked = false; }
  if (!linked) console.error(JSON.stringify({ selfCheck: 'assemble-edits', skip: 'junction 不可建（平台 / 权限）——链接面断言按跳过记绿，真空绿风险如实申报' }));
  check('junction 指仓内·须拦（不可建时跳过记绿）', linked ? isInsideRepo(join(root, 'link', 'out'), repo) : true, true);
  writeFileSync(join(repo, 'crlf2.txt'), '甲\r\n乙\r\n', 'utf8');
  check('CRLF 与 LF 清单不符·诊断句', String(fail1(one('crlf2.txt', '乙\n', '丙\n')).message).includes('CRLF'), true);
  if (linked) { try { unlinkSync(join(root, 'link')); } catch { /* 已不存在 */ } }
  rmSync(root, { recursive: true, force: true });
  console.log(JSON.stringify({ selfCheck: 'assemble-edits', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

function main(argv) {
  if (argv.includes('--self-check')) { runAssembleSelfCheck(); return; }
  const [listArg, outArg] = argv;
  if (!listArg || !outArg) {
    console.log(JSON.stringify({ ok: false, gate: 'assemble-edits', message: '用法：assemble-edits.mjs <编辑清单.json> <仓外输出根>；或 --self-check' }));
    process.exit(2);
  }
  let list;
  try { list = JSON.parse(readFileSync(resolve(projectDir, listArg), 'utf8')); } catch (e) {
    console.log(JSON.stringify({ ok: false, gate: 'assemble-edits', message: `清单不可读或非法 JSON：${e.message}` }));
    process.exit(2);
  }
  const outAbs = resolve(outArg);
  if (isInsideRepo(outAbs, projectDir)) {
    console.log(JSON.stringify({ ok: false, gate: 'assemble-edits', outputRoot: outAbs, message: '输出根在仓内——沙盘副本必须落仓外（architect 不落 canonical 的机械兜底）' }));
    process.exit(2);
  }
  const res = assemble(list, projectDir);
  if (!res.ok) {
    console.log(JSON.stringify({ ok: false, gate: 'assemble-edits', failures: res.failures, note: '断言失败：未写任何文件' }));
    process.exit(1);
  }
  const files = writePlan(res.plan, outAbs);
  console.log(JSON.stringify({ ok: true, gate: 'assemble-edits', outputRoot: outAbs, files }));
}

main(process.argv.slice(2));
