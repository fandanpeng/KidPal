#!/usr/bin/env node
// Static and evidence-based mutation gate. Actual source mutation is performed by
// verifier/fixer workflow; this script rejects missing or insensitive evidence.
// 260813-gate-accuracy-batch T2：对称 stringify 匹配（OQ-3=(a)）+ 主流程函数化 + argv 触发型 --self-check（ARQ-114 先例）。

import { existsSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { mentionsId, structuralAcceptanceIds } from './lib/acceptance-id-shared.mjs'; // 260731 批：匹配逻辑收敛单源（原本地 mentionsId/structuralAcceptanceIds 拷贝已删；raw-id 有意分歧经 extractId 参数保全，见下注）

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function taskPackagePath() {
  const explicit = process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH;
  if (explicit) return resolve(projectDir, explicit);

  const current = resolve(projectDir, '.claude/taskpackages/current/taskpackage.json');
  return existsSync(current) ? current : null;
}

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'mutation-check', message, ...details });
  process.exit(1);
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^\uFEFF/, ''));
}

// ARQ-097 deliberate divergence：本函数**有意不**用 ./lib/acceptance-id-shared.mjs 的 extractShortId 包裹——
// mutation-check 需要**全 id**（raw）；先提短 id 会钝化匹配（短 id 提不到后缀）。见 lib 头部注记 + PROPOSAL §1 有意变体。
// 260731 批：匹配逻辑（mentionsId/structuralAcceptanceIds）已收敛入 lib 单源，经 extractId 参数传本 raw 提取器——
// 分歧点 = id 形态、非匹配逻辑（同批 mentionsId 去裸后缀 = 力度变更，与 coverage 侧同适用）。
// 套件级条目（suiteGate:true / kind:'structural'）mutation 证据语义 = **不适用**——套件级回归门非逐 AC 敏感判定对象（M-4 成文）。
function acceptanceId(value) {
  if (typeof value === 'string') return value;
  if (value && typeof value === 'object') return value.id || value.acceptanceId || value.name;
  return null;
}

function evidenceRecords(value) {
  if (!value) return [];
  if (Array.isArray(value)) return value;
  if (typeof value === 'object') return Object.entries(value).map(([acceptanceId, record]) => ({ acceptanceId, ...record }));
  return [];
}

function recordIsSensitive(record) {
  const text = JSON.stringify(record).toLowerCase();
  return record.sensitive === true
    || record.ok === true
    || record.pass === true
    || /sensitive|turned red|变红|failed as expected/.test(text);
}

// 260813-gate-accuracy-batch T2（OQ-3=(a) 对称 stringify）：record 经 JSON.stringify 后其内 acceptance 文本的 `"` 被转义为 `\"`，
// raw id（含未转义 `"`）对 stringify 空间结构性失配（TP3 B-1 等 6 实例谱系）。id 以 JSON.stringify(id).slice(1,-1)
// = id 在 stringify 空间中的精确形态匹配。**行为不变断言严格条件**：id 不含任何 JSON 需转义字符（`"` / `\` / 控制字符）时
// escapedId === id，匹配逐字节不变；含 `\` 不含 `"` 的 id 行为变化方向 = 既有假阴转命中（向严无害）。
function evidenceMatchesId(records, id) {
  const escapedId = JSON.stringify(String(id)).slice(1, -1);
  return records.some((record) => mentionsId(JSON.stringify(record), escapedId) && recordIsSensitive(record));
}

function hasUnpatternedThrow(content) {
  return /\.rejects\s*\.\s*toThrow(?:Error)?\s*\(\s*\)/.test(content)
    || /\.toThrow(?:Error)?\s*\(\s*\)/.test(content);
}

function isShallowStatusOnly(content) {
  const checksStatus200 = /status(?:Code)?\)?\s*\.?(?:toBe|toEqual)?\s*\(?\s*200\b/i.test(content)
    || /\.status\s*\(\s*200\s*\)/i.test(content);
  if (!checksStatus200) return false;
  const hasBehaviorEvidence = /schema\.parse|safeParse|findUnique|findFirst|count\s*\(|emittedEvents|toContainEqual|locator\s*\(|toHaveText|toBeVisible|Audit|AiRun|correlationId|event|create\s*\(/i.test(content);
  return !hasBehaviorEvidence;
}

function main() {
  const pathToTaskPackage = taskPackagePath();
  if (!pathToTaskPackage) {
    print({ ok: true, gate: 'mutation-check', skipped: true, reason: 'no TaskPackage context' });
    process.exit(0);
  }

  if (!existsSync(pathToTaskPackage)) fail('TaskPackage file does not exist', { taskPackage: pathToTaskPackage });

  const taskPackage = readJson(pathToTaskPackage);
  if (taskPackage.mutationRequired === false) {
    const reason = taskPackage.mutationSkipReason || taskPackage.mutationReason;
    if (!reason || String(reason).trim().length < 12) {
      fail('mutationRequired=false must include mutationSkipReason/mutationReason', { taskPackage: pathToTaskPackage });
    }
    print({ ok: true, gate: 'mutation-check', skipped: true, reason });
    process.exit(0);
  }

  const acceptance = (Array.isArray(taskPackage.acceptance) ? taskPackage.acceptance : [])
    .map(acceptanceId)
    .filter(Boolean);
  const { structural, suiteGates } = structuralAcceptanceIds(taskPackage, acceptance, { extractId: acceptanceId });
  const exempt = new Set([...structural, ...suiteGates]);
  const functionalAcceptance = acceptance.filter((id) => !exempt.has(id));
  const files = Array.isArray(taskPackage.acceptanceTestsFile) ? taskPackage.acceptanceTestsFile : [];

  for (const file of files) {
    const absolutePath = resolve(projectDir, file);
    if (!existsSync(absolutePath)) continue;
    const content = readFileSync(absolutePath, 'utf8');
    if (hasUnpatternedThrow(content)) fail('Acceptance tests contain unpatterned toThrow assertions', { file });
    if (isShallowStatusOnly(content)) fail('Acceptance tests contain status-only shallow assertions', { file });
  }

  if (functionalAcceptance.length === 0) {
    print({ ok: true, gate: 'mutation-check', skipped: true, reason: 'only structural acceptance ids', structural: [...structural], suiteGates: [...suiteGates] });
    process.exit(0);
  }

  const records = evidenceRecords(taskPackage.mutationEvidence || taskPackage.mutationCheck || taskPackage.mutationResults);
  if (records.length === 0) {
    fail('TaskPackage must include mutationEvidence for each functional acceptance id', { functionalAcceptance });
  }

  const missingSensitive = functionalAcceptance.filter((id) => !evidenceMatchesId(records, id));

  if (missingSensitive.length > 0) {
    fail('Mutation evidence is missing or insensitive for some acceptance ids', { missingSensitive });
  }

  print({ ok: true, gate: 'mutation-check', functionalAcceptance, structural: [...structural], suiteGates: [...suiteGates] });
}

// argv 触发型自检（260813-gate-accuracy-batch T2.3，ARQ-114 先例义务；机械化调用方缺位 = ARQ-260731-g3-selfcheck-mechanical-runner 消费面）。
// 结构注：self-check 分支先于 main() 内无 TaskPackage 上下文早退——裸跑 --self-check 不得走 taskPackagePath 假绿路径。
function selfCheck() {
  let pass = 0;
  let failCount = 0;
  const ok = (cond, name) => {
    if (cond) pass += 1;
    else { failCount += 1; console.error(`FAIL [${name}]`); }
  };

  // 正案——含双引号字面量 id + 对应敏感 record → 命中（TP3 B-1 形态重现：改前结构性失配，改后命中）
  const quotedId = 'TP3-B1: 金额字面量 {"value":"44万 RMB"} 落库';
  const quotedRecord = { acceptanceId: quotedId, sensitive: true, mutation: '移除解析器', result: 'turned red' };
  ok(evidenceMatchesId([quotedRecord], quotedId) === true, 'sc-quoted-id-positive');

  // 负案——无证据 record → 不命中（闸仍红）
  ok(evidenceMatchesId([], quotedId) === false, 'sc-no-evidence-negative');

  // 负案——record 不敏感（无 sensitive 信号）→ 不命中
  ok(evidenceMatchesId([{ acceptanceId: quotedId, note: '无敏感信号' }], quotedId) === false, 'sc-insensitive-record-negative');

  // 不变案——严格条件（不含 `"`/`\`/控制字符）id：escapedId === id，行为逐字节不变
  const plainId = 'TP2-A1: confirm 异步入队与发送（无引号、全角角括号「」）';
  ok(JSON.stringify(plainId).slice(1, -1) === plainId, 'sc-plain-id-escape-identity');
  const plainRecord = { acceptanceId: plainId, sensitive: true };
  ok(evidenceMatchesId([plainRecord], plainId) === true, 'sc-plain-id-invariant-hit');

  // raw-id 有意分歧保全（ARQ-097 deliberate divergence）——acceptanceId 提取器不走 extractShortId，返回 raw 全 id
  ok(acceptanceId('S1-A1: 整句描述') === 'S1-A1: 整句描述', 'sc-raw-id-divergence-preserved');
  ok(acceptanceId({ id: 'S1-A1: 整句描述' }) === 'S1-A1: 整句描述', 'sc-raw-id-object-form-preserved');

  // 方向案——含 `\` 不含 `"` 的 id：既有假阴转命中（向严无害，F5(v) 声明方向）
  const backslashId = '路径 C:\\AI\\x 的验收';
  ok(evidenceMatchesId([{ acceptanceId: backslashId, sensitive: true }], backslashId) === true, 'sc-backslash-id-false-negative-to-hit');

  console.log(JSON.stringify({ selfCheck: 'mutation-check', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
  if (failCount > 0) process.exit(1);
}

// isMain 守卫（对偶 lib/acceptance-id-shared.mjs:182-185 / supervisor-verdict-gate.mjs:703 形态）：
// 现行文件为扁平顶层脚本，本批函数化后以守卫收口；import 劫持面 = 零（R1 核无 JS import 方），守卫为对偶纪律非当下缺口。
function isMain() {
  const invoked = process.argv[1];
  return Boolean(invoked && import.meta.url === pathToFileURL(resolve(invoked)).href);
}

if (isMain() && process.argv.includes('--self-check')) selfCheck();
else if (isMain()) main();
