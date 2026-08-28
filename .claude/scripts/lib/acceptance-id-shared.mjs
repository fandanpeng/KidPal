// Shared acceptance-id extraction — single source for the F4/F5.2 short-id logic
// previously duplicated (executable code byte-for-byte) in acceptance-coverage-check.mjs
// and docs-sync-check.mjs. ARQ-097 convergence (gov-review-gate-command-convergence,
// human-approved 2026-07-30).
//
// owner=architect, approver=human (G3 script, see DEV-GOVERNANCE.md §6)

import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
//
// 260731-gov-review-acceptance-coverage-structural-exemption（FROZEN v1.0，human-approved
// 2026-07-31）：structuralAcceptanceIds / mentionsId 收敛入本单源（原 acceptance-coverage-check
// 与 mutation-check 两份本地拷贝删净，定义点恰 1 处）。语义变更：
//  - mentionsId 去裸后缀 \b 分支，收紧为全 id 子串匹配（k8 后缀碰撞假阴闭合 = 力度变更，成文辨析
//    见 dev-loop.md §3 A10 段；区间缩写「A1..A14」/prose 同物提及自此不豁免，须写全 id 或显式对象标记）。
//  - 豁免唯一一等通道 = 显式对象标记：kind:'structural'（非代码可测）/ suiteGate:true（套件级回归门），
//    双集合返回供审计打印；:84 governanceWords prose 洗白已移除（恢复成文门义，dev-loop.md:156 早已明禁）。
//  - exitCriteria「structuralWords + 全 id 提及」豁免路径保留（OQ3 保守变体，含对象分支）。
//
// DELIBERATE NON-CONSUMERS / 有意变体（do not "fix" these without review）:
//  - mutation-check.mjs 消费本模块的 mentionsId / structuralAcceptanceIds，但经 extractId 参数
//    传 RAW id 提取器（不 extractShortId）——分歧点是 id 形态、非匹配逻辑（ARQ-097 登记的
//    deliberate divergence；260731 批收敛匹配逻辑后此分歧保留）。
//  - docs-sync-check.mjs:47-51 docMentionsAcceptance 的 \b 裸后缀语义 = deliberate residual
//    （待 ARQ-260731-docsync-suffix-mention-residual 裁）——docs 闸假绿（自带弱点注记）非承重覆盖
//    假阴，docs 速记「A1」可能是有意指称，盲去后缀会造 docs 闸假红；不得静默「修」。
//
// 形状校验（互斥 fail-loud / 未知值 fail-safe / 对象条目 id 提取）不在本库——
// 一线在 taskpackage-required-check.mjs（260731 批 M-6）；本库只认 kind==='structural' 与
// suiteGate===true，其余值不豁免（fail-safe 方向 = 红）。

// F4: 存量三种格式统一——整句 "TP-7-A1: 描述" 提取短 id；纯短 id（无冒号）原样返回。
export function extractShortId(value) {
  if (typeof value !== 'string') return value;
  const match = value.match(/^\s*([A-Za-z0-9][A-Za-z0-9._-]*)\s*:\s/); // F4 提取正则（PROPOSAL 逐字面）
  return match ? match[1] : value;
}

// 对象形式先取 .id 再过同一提取（防御 id 字段本身也是整句，实证见 tp8-s2-adapter A5）。
export function acceptanceId(value) {
  if (typeof value === 'string') return extractShortId(value); // F4: 字符串条目过提取
  if (value && typeof value === 'object') return extractShortId(value.id || value.acceptanceId || value.name); // F4: 对象 .id 过同一提取
  return null;
}

// 全 id 提及判定（260731 批：去 :71 裸后缀分支 = 力度变更）。只认子串（含完整 id 出现）；
// 裸末段 \b 匹配（跨 id 碰撞源）已删——「A1..A14」区间缩写、prose 同物提及自此不匹配。
export function mentionsId(text, id) {
  if (!text || !id) return false;
  return text.includes(id);
}

const structuralWords = /structural|typecheck|firmware-build|idf\.py|contract-lint|compile/i;

// 豁免判定单源。返回 { structural, suiteGates } 双集合（审计打印分离；exempt = 两集合并集）。
// 通道集（OQ3 保守变体终态）：
//   acceptance[] 对象条目显式标记（唯一一等通道）：kind:'structural' → structural；suiteGate:true → suiteGates。
//   exitCriteria[] structuralWords + 全 id 提及 → structural（字符串分支 + 对象分支同语义，去后缀后）。
// 已移除：acceptance prose governanceWords 词表洗白（:84，CS-7 gaming 通道）。
// extractId：默认 acceptanceId（F4 短 id）；mutation-check 传 raw 提取器保有意分歧（id 形态分歧，非匹配逻辑）。
export function structuralAcceptanceIds(taskPackage, acceptance, { extractId = acceptanceId } = {}) {
  const structural = new Set();
  const suiteGates = new Set();

  for (const item of Array.isArray(taskPackage.acceptance) ? taskPackage.acceptance : []) {
    const id = extractId(item);
    if (!id || typeof item !== 'object') continue;
    if (item.kind === 'structural') structural.add(id);
    else if (item.suiteGate === true) suiteGates.add(id);
  }

  for (const criterion of Array.isArray(taskPackage.exitCriteria) ? taskPackage.exitCriteria : []) {
    if (typeof criterion === 'string') {
      if (!structuralWords.test(criterion)) continue;
      for (const id of acceptance) if (mentionsId(criterion, id)) structural.add(id);
    } else if (criterion && typeof criterion === 'object') {
      const text = JSON.stringify(criterion);
      if (criterion.kind === 'structural' || structuralWords.test(text)) {
        for (const id of acceptance) if (mentionsId(text, id)) structural.add(id);
      }
    }
  }

  return { structural, suiteGates };
}

// ARQ-114 结构化自检（G3 解析器改动必带 teeth）。acceptanceId 类：整句提取 / 纯短 id / 对象三键
//（id/acceptanceId/name）/ 设计动机防御例（id 字段本身为整句，tp8-s2-adapter A5）/
// 负例（正则需 `:\s` 冒号后空白）/ null / 非串非对象。
// structural 类（260731 批 7 类必载，负例 assert 方向非可选）：
//  ① 对象 kind:'structural' 豁免 ② suiteGate:true 豁免（入 suiteGates 集）
//  ③ 后缀碰撞不豁免（k8 形负例：S1-A1 准则 + S2-A1 不被洗）④ prose-governance-词不豁免（CS-7 形负例）
//  ⑤ structural 准则全 id 提及豁免（K7-S5 形，OQ3 保留路径）⑥ 全 id/short ref 兼容 + 裸后缀负例
//  ⑦ 对象条目 id 提取回归（M-6 联动：对象条目 id 可入 acceptanceIds）。
//  另：mutation raw-id 变体用例（F-07③：extractId=raw 时对象 kind/suiteGate 仍按 raw id 豁免）。
export function runAcceptanceIdSelfCheck() {
  const cases = [
    { input: 'TP-7-A1: S1 existing acceptance...', expect: 'TP-7-A1', label: 'string-full-sentence' },
    { input: 'S1-A2', expect: 'S1-A2', label: 'string-bare-short-id' },
    { input: { id: 'S3-A1' }, expect: 'S3-A1', label: 'object-id' },
    { input: { acceptanceId: 'X' }, expect: 'X', label: 'object-acceptanceId' },
    { input: { name: 'Y' }, expect: 'Y', label: 'object-name' },
    { input: { id: 'TP-9-A3: long desc' }, expect: 'TP-9-A3', label: 'object-id-is-full-sentence (tp8-s2-adapter A5)' },
    { input: 'TP-7-A1:desc', expect: 'TP-7-A1:desc', label: 'negative-no-space-after-colon (regex needs :\\s)' },
    { input: null, expect: null, label: 'null' },
    { input: 123, expect: null, label: 'non-string-non-object' },
  ];
  let pass = 0;
  let fail = 0;
  const check = (label, got, expect) => {
    if (got === expect) {
      pass += 1;
    } else {
      fail += 1;
      process.stderr.write(`FAIL ${label}: got ${JSON.stringify(got)} expect ${JSON.stringify(expect)}\n`);
    }
  };
  for (const c of cases) check(c.label, acceptanceId(c.input), c.expect);

  // ① 对象 kind:'structural' 豁免
  {
    const { structural, suiteGates } = structuralAcceptanceIds({ acceptance: [{ id: 'TP-1-A9', kind: 'structural' }] }, ['TP-1-A9']);
    check('structural-1: object kind structural exempt', structural.has('TP-1-A9') && suiteGates.size === 0, true);
  }
  // ② suiteGate:true 豁免（入 suiteGates 集，非 structural）
  {
    const { structural, suiteGates } = structuralAcceptanceIds({ acceptance: [{ id: 'LP-14', suiteGate: true }] }, ['LP-14']);
    check('structural-2: suiteGate exempt into suiteGates set', suiteGates.has('LP-14') && !structural.has('LP-14'), true);
  }
  // ③ 后缀碰撞不豁免（k8 形负例）：S1-A1 准则含 prisma + S2-A1 不得被 \bA1\b 连带洗
  {
    const { structural } = structuralAcceptanceIds(
      { acceptance: ['S1-A1', 'S2-A1'], exitCriteria: ['contract-lint gate: S1-A1 clean'] },
      ['S1-A1', 'S2-A1'],
    );
    check('structural-3a: full-id mention still exempts S1-A1', structural.has('S1-A1'), true);
    check('structural-3b: NEGATIVE suffix collision must NOT wash S2-A1', structural.has('S2-A1'), false);
  }
  // ④ prose-governance-词不豁免（CS-7 形负例）：acceptance 字符串含 "integrator" 无显式标记 → 仍须覆盖
  {
    const { structural, suiteGates } = structuralAcceptanceIds(
      { acceptance: ['CS-7: 既有套件不新增红，归 verifier/integrator 承载'] },
      ['CS-7'],
    );
    check('structural-4: NEGATIVE prose governance word must NOT exempt CS-7', structural.has('CS-7') || suiteGates.has('CS-7'), false);
  }
  // ⑤ structural 准则全 id 提及豁免（K7-S5 形，OQ3 保留路径）
  {
    const { structural } = structuralAcceptanceIds(
      { acceptance: ['K7-S5'], exitCriteria: ['firmware-build: K7-S5 clean'] },
      ['K7-S5'],
    );
    check('structural-5: exitCriteria structuralWords + full-id mention exempts K7-S5', structural.has('K7-S5'), true);
  }
  // ⑥ mentionsId 全 id / short ref 兼容 + 裸后缀负例
  check('structural-6a: mentionsId full-id substring', mentionsId('criterion S1-A1 ok', 'S1-A1'), true);
  check('structural-6b: NEGATIVE bare suffix no match (A1..A14 interval)', mentionsId('S1-A1..A14 all green', 'P1-A14'), false);
  // ⑦ 对象条目 id 提取回归（M-6 联动：对象条目 id 可入 acceptanceIds）
  check('structural-7: object entry id extraction (suiteGate entry)', acceptanceId({ id: 'LP-14', suiteGate: true }), 'LP-14');
  // mutation raw-id 变体（F-07③）：raw 提取器（不 extractShortId）下对象标记仍豁免、id 形态为 raw
  {
    const rawExtractId = (value) => {
      if (typeof value === 'string') return value;
      if (value && typeof value === 'object') return value.id || value.acceptanceId || value.name;
      return null;
    };
    const { structural, suiteGates } = structuralAcceptanceIds(
      { acceptance: [{ id: 'S1-A1: full raw sentence', kind: 'structural' }, { id: 'LP-14', suiteGate: true }] },
      ['S1-A1: full raw sentence', 'LP-14'],
      { extractId: rawExtractId },
    );
    check('structural-8a: mutation raw-id variant kind exempt (raw form)', structural.has('S1-A1: full raw sentence'), true);
    check('structural-8b: mutation raw-id variant suiteGate exempt', suiteGates.has('LP-14'), true);
  }

  process.stdout.write(`${JSON.stringify({ selfCheck: 'acceptance-id-shared', pass, fail, total: pass + fail, ok: fail === 0 })}\n`);
  if (fail > 0) process.exit(1);
}

// isMain 守卫（260731 批 F-07：lib 是被 import 的库，无守卫塞自检 = import 副作用劫持面）。
// 对齐本仓 isMain 形态（retro-staleness-check.mjs:98-101）。
function isMain() {
  const invoked = process.argv[1];
  return Boolean(invoked && import.meta.url === pathToFileURL(resolve(invoked)).href);
}

if (isMain() && process.argv.includes('--self-check')) runAcceptanceIdSelfCheck();
