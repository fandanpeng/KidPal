#!/usr/bin/env node
// contract-lint.mjs — 承重结构闸 `contract-lint`（G3 强制层），KidPal 档 2 新写。
//
// 源仓（TS 契约包）版 lint contracts/schema/** 的 TypeScript 契约。本仓契约面 = 单文件 markdown
// 协议（docs/contracts/interface-contract.md：笔顺 JSON D3 / display 块 E1 / 课表 MD B1 / shim API），
// 无 TS schema 可 lint。等价 lint 目标转为该 markdown 契约的**结构完整性**（dependency-free）：
//
//   ① 契约文件存在且非空（缺失/空 = 契约面崩坏，红）；
//   ② 顶层节标题存在（一、二、三…编号节 / 关键协议块），缺失 = 结构破坏（红）；
//   ③ 契约内 ```json / ```display 代码围栏成对闭合（奇数个 ``` = 协议块截断，红）；
//   ④ 文档内相对链接（[x](../systems/...) 等）指向的文件存在（断链 = 引用腐化，warn 不红）。
//
// 这是 markdown 契约面的「机械一致性」下限守卫——比 TS 契约 lint 弱（无法做类型级校验），
// 但胜过无。更深的 spec↔contract 语义对账已在档 1 诚实降级为人工（ARQ-260827-contract-mech-gate-gap
// 追踪），本闸不冒充那个能力。
//
// 结构化自检：`--self-check`（argv 触发 + isMain 守卫）跑合成电池 + 实盘 smoke。

import { existsSync, readFileSync } from 'node:fs';
import { resolve, dirname, join } from 'node:path';
import { pathToFileURL } from 'node:url';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
const CONTRACT_REL = 'docs/contracts/interface-contract.md';

function print(payload) { console.log(JSON.stringify(payload, null, 2)); }

// 纯 lint（self-check 直调，传入文本）。返回 { errors: string[], warnings: string[] }。
function lintContractText(text) {
  const errors = [];
  const warnings = [];
  if (!text || text.trim() === '') {
    errors.push('contract-empty:契约文件为空');
    return { errors, warnings };
  }
  // ① 顶层编号节标题（## 一、 / ## 二、 …）。
  const sectionCount = (text.match(/^##\s+[一二三四五六七八九十]/gm) || []).length;
  if (sectionCount === 0) errors.push('contract-no-sections:缺顶层编号节标题（## 一、…）');
  // ② 代码围栏成对闭合。
  const fenceCount = (text.match(/^```/gm) || []).length;
  if (fenceCount % 2 !== 0) errors.push('contract-fence-unbalanced:代码围栏未成对（fence 计数=' + fenceCount + '）');
  // ③ 关键协议块标记（display 指令 / 笔顺 / 课表，任一为失即警）。
  for (const marker of ['display', '笔顺', '课表']) {
    if (!text.includes(marker)) warnings.push(`contract-marker-missing:未见关键协议标记「${marker}」`);
  }
  return { errors, warnings };
}

// 实盘：lint 契约文本 + 相对链接存在性。
function lintReal(root) {
  const errors = [];
  const warnings = [];
  const contractPath = resolve(root, CONTRACT_REL);
  if (!existsSync(contractPath)) {
    errors.push(`contract-missing:契约文件不存在 ${CONTRACT_REL}`);
    return { errors, warnings };
  }
  const text = readFileSync(contractPath, 'utf8');
  const r = lintContractText(text);
  errors.push(...r.errors);
  warnings.push(...r.warnings);
  // ④ 相对链接存在性（warn 不红）。
  const linkRe = /\]\((\.{1,2}\/[^)\s]+)\)/g;
  let m;
  while ((m = linkRe.exec(text)) !== null) {
    const target = resolve(dirname(contractPath), m[1]);
    if (!existsSync(target)) warnings.push(`contract-link-broken:相对链接目标不存在 ${m[1]}`);
  }
  return { errors, warnings };
}

function runSelfCheck() {
  const results = [];
  const ok = (name, cond, detail = '') => results.push({ name, ok: Boolean(cond), detail });

  const good = '# C\n\n## 一、甲\n\n```json\n{}\n```\n\ndisplay 笔顺 课表\n';
  ok('good-contract-passes', lintContractText(good).errors.length === 0);

  ok('empty-contract-red', lintContractText('').errors.some((e) => e.startsWith('contract-empty')));

  const noSec = '# C\n\n正文 display 笔顺 课表\n```json\n{}\n```\n';
  ok('no-sections-red', lintContractText(noSec).errors.some((e) => e.startsWith('contract-no-sections')));

  const unbal = '## 一、甲\n```json\n{}\ndisplay 笔顺 课表\n'; // 奇数围栏
  ok('fence-unbalanced-red', lintContractText(unbal).errors.some((e) => e.startsWith('contract-fence-unbalanced')));

  const noMarker = '## 一、甲\n```json\n{}\n```\n正文\n';
  const wm = lintContractText(noMarker);
  ok('marker-missing-warn-not-red', wm.errors.length === 0 && wm.warnings.length > 0);

  // 实盘 smoke：本仓契约应过（或如实报 warn）。
  try {
    const r = lintReal(projectDir);
    ok('real-contract-smoke', r.errors.length === 0, `errors=[${r.errors.join(';')}] warnings=${r.warnings.length}`);
  } catch (e) {
    ok('real-contract-smoke', false, `exception: ${e.message}`);
  }

  const allOk = results.every((r) => r.ok);
  process.stdout.write(`${JSON.stringify({ selfCheck: 'contract-lint', ok: allOk, results }, null, 2)}\n`);
  process.exit(allOk ? 0 : 1);
}

function isMain() {
  const invoked = process.argv[1];
  return Boolean(invoked && import.meta.url === pathToFileURL(resolve(invoked)).href);
}

if (isMain() && process.argv.includes('--self-check')) runSelfCheck();

if (isMain() && !process.argv.includes('--self-check')) {
  const { errors, warnings } = lintReal(projectDir);
  if (warnings.length > 0) print({ ok: true, gate: 'contract-lint', warnings });
  if (errors.length > 0) {
    print({ ok: false, gate: 'contract-lint', message: 'markdown 契约面结构不完整', errors, warnings });
    process.exit(1);
  }
  if (warnings.length === 0) print({ ok: true, gate: 'contract-lint', contract: CONTRACT_REL });
}
