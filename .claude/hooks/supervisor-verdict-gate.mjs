#!/usr/bin/env node
// Owner: architect (G3 enforcement layer)
// Approver: human
// DEV-GOVERNANCE §6: G3 hooks/scripts → owner=architect, approver=human.
//
// 机械 supervisor-verdict 闸（gov-review-verdict-bearing-dual，HUMAN-APPROVED 2026-07-30，H1）。
// 对偶 verifier bearing 的信任源 = **四件机械事**（decision-packet §0，非「谁跑」）：
//   1. 判定 = 程序按机械判据产 0/1（riskTier 从 git diff 重派生 + 证据完整度消费 bearing facts）；
//   2. verdict write-once（脚本 fs-write：tmp openSync 'wx' 独占 + linkSync hardlink + fsync，对偶 verify.mjs publishFact:245/:250）；
//   3. closed-schema validator（bearing fact 消费复用 exported validateFactObject，verifier-evidence-check.mjs:276）；
//   4. 独立重验 = G6 第二方重算（supervisor-gate-check.mjs 从 immutable 输入重算 ≡ 磁盘，判者 G6 ≠ 产者本闸，FIX-6）。
//
// **planner 只 invoke（Bash 跑本脚本），不 Write verdict**（FIX-3，scope-map 收权 Option A，H6）。
//   本脚本内部 fs 写不过 Edit/Write 门（scope-map universal.bashDenyWrite _doc「程序内部 fs 写入不在覆盖内」），
//   与 verify.mjs 写 bearing fact 同机制。
//
// **SSOT 反漂移（C-5/FIX-2）**：riskTier 判据的权威文本 = `.claude/references/dev-loop.md:244`。
//   本程序是该 prose 的**机械投影、非第二权威**——判据注释逐处带 `:244`/`:106-111` 出处。改判据先改 dev-loop:244，再同步本投影。
//
// **provenance 诚实（P-3/FIX-7）**：程序产就诚实标程序产——verdict 产 `mechanicalSupervisorGate:{ran,producedBy,...}`，
//   不再要求不可满足的「真 supervisor agent 派发」（supervisor 2026-07-25 退役；反转 gov-review-supervisor-provenance-strength
//   FROZEN HUMAN-APPROVED 2026-07-25，H7 human 批准 2026-07-30），亦不放任 LLM 自述。
//
// **fail-safe 向严单调（P-4 铁律/FIX-5）**：逐失败分支向严——闸崩溃/无 verdict→BLOCK；riskTier 派生歧义→取严 high→ESCALATE；
//   证据门判不了→BLOCK；bearing fact 缺失→BLOCK、不匹配（validateFactObject !ok）→fail-loud exit≠0；
//   git 失败**不得**沿用 supervisor-gate-check G3「不 block」放行派生，须「不能派生→向严 ESCALATE」；
//   criterion 抛异常**绝不**吞成默认 low（fail 向 APPROVE = BLOCKING 级错误）。
//   向严但不过度（P-1 并存）：明确 low 信号充分（FIX-2 绿色通道）→ 正常 APPROVE；向严只作用真歧义，不作用明确 low。
//
// **威胁模型（H5，human 批准「同级行为保全即足」）**：本闸与 bearing 同保 LLM 漂移 / 流程意外 / 幻觉收口，
//   **不抗恶意内部人**（planner 物理仍可经任意 Bash/内部 fs 手写绕过，与 bearing 同残余面，对称接受 R-1）。
//
// 用法：
//   node .claude/hooks/supervisor-verdict-gate.mjs [--change-id <id>]      # planner invoke，产 delivery/changes/dev/<id>/supervisor-verdict.json（write-once）
//   node .claude/hooks/supervisor-verdict-gate.mjs --self-check            # ARQ-114 结构化自检（必带，非可选）

import {
  closeSync,
  existsSync,
  fsyncSync,
  linkSync,
  mkdirSync,
  openSync,
  readFileSync,
  readdirSync,
  statSync,
  unlinkSync,
  writeFileSync,
} from 'node:fs';
import { join, resolve } from 'node:path';
import { execSync, spawnSync } from 'node:child_process';
import { randomUUID } from 'node:crypto';
import { fileURLToPath } from 'node:url';
// FIX-4/RC-5：bearing fact 消费的唯一 fail-loud 单源（已 export，verifier-evidence-check.mjs:276）。
// 相对路径显式 .mjs（ESM 必需），对偶 verify.mjs:29 跨目录 import 先例（同 .claude/hooks/ → ../scripts/）。
import { validateFactObject } from '../scripts/verifier-evidence-check.mjs';
import { resolveOriginType } from '../scripts/lib/governance-gate-shared.mjs';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// ---- tier 序（向严单调的基础）----
// low/med 自动合并（证据完整）；high/critical 升级人类（dev-loop:244）。med = medium 别名。
const TIER_RANK = Object.freeze({ low: 1, medium: 2, high: 3, critical: 4 });

function normalizeTier(value) {
  const v = String(value || '').toLowerCase();
  if (v === 'med') return 'medium';
  if (v === 'low' || v === 'medium' || v === 'high' || v === 'critical') return v;
  return null; // 未知 → 调用方按「declared 不参与 max」处理（不被 planner 自填怪值牵走）
}

// max 只升不降（P-4）：declared 可升（保守无害），不可降（被 diff 驳回）。
function maxTier(a, b) {
  return (TIER_RANK[a] || 0) >= (TIER_RANK[b] || 0) ? a : b;
}

// effectiveTier = max(diffDerived, taskpackageDeclared)（FIX-1，取严 P-4）。
function effectiveTier(diffTier, declaredRaw) {
  const declared = normalizeTier(declaredRaw);
  if (!declared) return { tier: diffTier, declared: null };
  return { tier: maxTier(diffTier, declared), declared };
}

// ---- riskTier 机械判据（dev-loop:244 的机械投影，SSOT 反漂移 C-5）----

// critical 闭集（dev-loop:244「contracts/**、迁移、删数据为 critical」，宁滥勿漏 FIX-1）：
//   路径 = contracts/** ∨ migration 文件；内容 = 删数据闭集（esp_partition_erase_range/esp_partition_write/esp_ota_set_boot_partition/esp_flash_erase_region/nvs_flash_erase/sd_card_format/f_mkfs）。
//   content 轴归因（260803-critical-content-fp T2，判据不变、投影精度收敛）：perFile 归因覆盖全部 paths 时，
//   逐文件只扫生产面文件 diff 段、跳过测试/制品面排除闭集（ARTIFACT_TEST_EXCLUDE_PATH_RES，两轴共享单源）；归因失败回退全量 body 扫描 = 现状（向严单调）。
const CRITICAL_PATH_PATTERNS = [
  /(^|\/)docs\/contracts\//,      // 契约面（等价物：docs/contracts/**）
  /(^|\/)partitions?\.csv$/i,       // ESP-IDF 分区表（分区结构变更）
  /(^|\/)sdkconfig(\.\w+)?$/i,     // sdkconfig（编译配置，决定可引脚/特性）
];
const CRITICAL_CONTENT_PATTERNS = [
  /esp_partition_erase_range/i,        // 分区擦除（NVS / FATFS 存储面破坏）
  /esp_partition_write/i,              // 分区写入
  /esp_ota_set_boot_partition/i,       // OTA 启动分区切换（可致砖）
  /esp_flash_erase_region/i,           // flash 整片擦除
  /nvs_flash_erase/i,                  // NVS 全擦
  /sd_card_format|f_mkfs/i,            // SD 卡格式化
];

function classifyCritical(paths, content, perFile) {
  const reasons = [];
  for (const p of paths) {
    for (const re of CRITICAL_PATH_PATTERNS) {
      if (re.test(p)) { reasons.push(`critical-path:${p}`); break; }
    }
  }
  // content 轴：perFile 归因覆盖全部 paths → 逐文件只扫生产面文件段（reason 带归因，对偶 path 轴）；
  // 否则（null / 覆盖不全）回退全量 body 扫描 = 现状（260803-critical-content-fp T2，向严单调，自检电池 ⑪⑫ 固话）。
  const attributed = perFile instanceof Map && paths.every((p) => perFile.has(p));
  if (attributed) {
    for (const p of paths) {
      if (isArtifactTestExcluded(p)) continue; // 测试/制品面不承载生产删数据风险信号（排除闭集，两轴共享单源）
      const section = perFile.get(p);
      for (const re of CRITICAL_CONTENT_PATTERNS) {
        if (re.test(section)) reasons.push(`critical-content:${re.source}:${p}`);
      }
    }
  } else {
    const text = String(content || '');
    for (const re of CRITICAL_CONTENT_PATTERNS) {
      if (re.test(text)) reasons.push(`critical-content:${re.source}`);
    }
  }
  return { hit: reasons.length > 0, reasons };
}

// high 闭集（dev-loop:244「全 mock、跨系统、core.*、安全轴触发为 high」，FIX-2 四项 prose 机判，锚 diff/制品不锚 planner 自填布尔）：
const CORE_PATH_PATTERNS = [
  /(^|\/)docs\/contracts\//,           // 契约面（笔顺 JSON / display 块 / shim API / 课表 MD）——恒 critical，此处防御性列
  /(^|\/)firmware\/boards\//,          // 板级支持包（多板共享面，改动面窄而承重）
  /(^|\/)firmware\/partitions?\.csv$/i, // 分区表（承重配置）
  /(^|\/)sdkconfig(\.\w+)?$/i,          // sdkconfig（编译配置）
];
// 安全轴（dev-loop:244「安全轴触发」，FIX-2）：diff 触 security 闸消费面（auth/consent/cost-audit 三具名轴）。
// 260801-gov-review-supervisor-gate-auth-axis-fp（FROZEN v0.3.1）假阳治理——三件套（判据不变，机械投影收敛）：
//   ②paths 承重（优先）：路径子串闭集命中即触发——*auth* 子串天然覆盖 **/auth/**、authenticator、
//     oauth-client、reauth 路径（segment glob 不吃 oauth-client.ts，子串形态保 OAuth 族文件级命中）；
//     向宽代价原声明「*auth* 吃 author.ts 类文件名属向严可接受」——260916 实证两包证伪（input-brief-acceptance-author.md
//     制品路径致 riskTier 虚高 ESCALATE、靠 human-approval 凭据放行），paths 轴收窄：先跳过测试/制品面排除闭集
//     （ARQ-260916-supervisor-auth-fp-author，与 content 轴 OQ-1=(a) 共享 ARTIFACT_TEST_EXCLUDE_PATH_RES 单源）；
//     生产面路径命中零改，向严不减（归因失败回退全量 content 扫描仍覆盖制品面文本）。
//   ①content 收敛（补充）：字母边界匹配（(?<![a-z])/(?![a-z]) 非 \b 的 \w 语义——OAuth2 的 2 不阻断，
//     而 author/AuthSession/authenticator/authority/acceptance-author 右邻字母被杀）+ 命中词闭集过滤
//     （门自产词汇 auth-axis / security-axis-diff:auth + 防御性 acceptance-author；词提取字符集含 ':'
//     保证 reason 串整词提取命中闭集；闭集维护挂 ARQ-077 一致性门消费面——新 auth- 分段 agent 名须补录）。
//   已知微收窄（packet 声明）：reauthorize/reauthentication（reauth 右邻字母）content 不命中，paths *auth* 兜底。
// 260813-gov-review-gate-accuracy-batch T1 归因化（OQ-1=(a)/OQ-2=(a)/OQ-7=(i)）：content 轴 perFile 归因覆盖全部 paths 时
//   逐文件只扫生产面段、跳过测试/制品面排除闭集（ARTIFACT_TEST_EXCLUDE_PATH_RES，与 critical-content 共享单源）；
//   归因失败回退全量 body 扫描 = 上行原文行为（向严单调）；reason 保旧形态 `security-axis-diff:<word>` 不归因
//   （归因词形会因 token 截断绕开命中词闭集，回退模式下复活已闭合根——R1-F3 实证）。
const SECURITY_AXIS_PATH_PATTERNS = [
  /auth/i,              // *auth* 路径子串（含 oauth/reauth/authenticator 形态）
  /password/i,          // *password*
  /consent/i,           // *consent*
  /cost[-_ ]?audit/i,   // *cost-audit*（含 costAudit/cost_audit 形态，宁滥勿漏）
];
const SECURITY_AXIS_CONTENT_RE = /(?<![a-z])(?:o?auth|reauth|consent|cost[-_ ]?audit)(?![a-z])/gi;
// 命中词提取字符集：字母/数字/:/_/. /-（门 reason 串 security-axis-diff:auth 整词可提取）。
const SECURITY_AXIS_TOKEN_CHAR = /[A-Za-z0-9:_.-]/;
// 已知非安全命中词闭集（小写比对；ARQ-077 消费面：新 auth- 分段 agent 名在此补录）。
const SECURITY_AXIS_TOKEN_EXCLUSIONS = new Set([
  'auth-axis',
  'security-axis-diff:auth',
  'acceptance-author',
]);

function extractSecurityAxisToken(text, start, end) {
  let s = start;
  let e = end;
  while (s > 0 && SECURITY_AXIS_TOKEN_CHAR.test(text[s - 1])) s -= 1;
  while (e < text.length && SECURITY_AXIS_TOKEN_CHAR.test(text[e])) e += 1;
  return text.slice(s, e);
}

// content 轴：字母边界命中 ∧ 命中词 ∉ 闭集 → 触发（收敛 dev-loop 制品文本假阳，向严不漏真安全面）。
function securityAxisContentHit(text) {
  for (const m of text.matchAll(SECURITY_AXIS_CONTENT_RE)) {
    const token = extractSecurityAxisToken(text, m.index, m.index + m[0].length);
    if (!SECURITY_AXIS_TOKEN_EXCLUSIONS.has(token.toLowerCase())) {
      return { hit: true, reason: `security-axis-diff:${m[0].toLowerCase()}` };
    }
  }
  return { hit: false };
}
// 真库/事件断言信号（dev-loop:106-111「反 200=过」：DB 副作用 + 事件已发）。「全 mock」= 有测试面但无这些信号。
const REAL_EFFECT_MARKERS = [
  /esp_partition_|nvs_flash_|esp_ota_/,        // ESP-IDF 真硬件副作用 API
  /\b(xTaskCreate|xQueueSend|xSemaphoreTake)\b/,  // FreeRTOS 内核对象副作用
  /\b(fopen|fwrite|fflush|remove|mkdir)\b/,       // 文件系统副作用（SD/Flash）
  /\b(TEST_ASSERT|TEST_ASSERT_EQUAL|TEST_ASSERT_NOT_NULL)\b/,  // Unity 断言（ESP-IDF 测试框架）
];
const TEST_FILE_RE = /(\.spec\.|\.test\.|__tests__|\/acceptance\/)/;

// 测试/制品面排除闭集（260803-critical-content-fp T2 立，260813-gate-accuracy-batch OQ-1=(a) 升两轴共享单源）：
// critical-content 轴与 auth 轴 content 扫描共用——这些路径的 diff 内容 = 测试 cleanup / 制品 prose，
// 不承载生产删数据/安全面风险信号，content 扫描跳过；critical-path 轴与 high core 轴不受影响。
// 260916 扩消费面（ARQ-260916-supervisor-auth-fp-author）：auth paths 轴亦消费本闭集（securityAxisTriggered 路径环先跳过）——
// 制品面路径名同样不承载生产安全信号，消 *auth* 吃 input-brief-acceptance-author.md 类假阳；判据不变、投影精度收敛。
// 维护注：TEST_FILE_RE 复用单源；制品三根对应 repo-structure「工作产物去哪」（delivery/changes/** closeout / taskpackages 过程件 / delivery/workstreams workstream 记忆）。
const ARTIFACT_TEST_EXCLUDE_PATH_RES = [
  TEST_FILE_RE,                 // 测试面：.spec. / .test. / __tests__ / /acceptance/
  /^delivery\/changes\//,       // closeout 证据 + arch review 制品
  /^delivery\/taskpackages\//,  // dev-loop 过程件（pod-events / verify-runs / rounds）
  /^delivery\/workstreams\//,   // workstream 记忆 prose
];

function isArtifactTestExcluded(path) {
  for (const re of ARTIFACT_TEST_EXCLUDE_PATH_RES) {
    if (re.test(path)) return true;
  }
  return false;
}

// 跨系统轴退役（260908-gov-review-escalation-conflict-ladder T1，human 终裁 B1 修法 + OQ4）：
// 目录面启发式（原按 firmware/server 第二段数系统数 + subsystem 多值判定）对双面板 Pod 恒真
// （home-p1 误报实证），isCrossSystem 与其 distinctAppSystems 前置一并摘除；
// 真跨系统保护改由升级闸 parallelOverlapProbe 的并行相交面探测承担（判据换轴，pod-coordination.md §10d 分级路由）。
// verdict 的 APPROVE/BLOCK 语义与证据完整度判定不动；riskTier 派生保留其余轴（core.* / 全 mock / 安全轴）。

function isCoreStar(paths) {
  for (const p of paths) {
    for (const re of CORE_PATH_PATTERNS) {
      if (re.test(p)) return { hit: true, reason: `core.*:${p}` };
    }
  }
  return { hit: false };
}

// 安全轴触发（FIX-2）：diff 触安全面（paths 优先承重 + content 收敛补充，260801-auth-axis-fp）∨ declared。
// OR 结构 ⇒ 翻 securityReviewRequired=false 无效（diff 命中仍在，FIX-5 PM-5 阻翻面）。
// content 轴归因（260813-gate-accuracy-batch T1，对偶 critical-content T2 同构）：perFile 归因覆盖全部 paths 时，
// 逐文件只扫生产面段、跳过测试/制品面排除闭集（ARTIFACT_TEST_EXCLUDE_PATH_RES 共享单源，OQ-1=(a)）；
// 归因失败回退全量 body 扫描 = 原行为（向严单调，OQ-2=(a)）。reason 保旧形态不归因（OQ-7=(i)：排除闭集词形交互面归零）。
function securityAxisTriggered(paths, content, tp, perFile) {
  for (const p of paths) {
    // ARQ-260916-supervisor-auth-fp-author：测试/制品面路径不承载安全面路径信号——排除闭集与 content 轴共享单源，
    // 消 *auth* 子串吃 input-brief-acceptance-author.md 类制品路径的假阳（实证两包 riskTier 虚高 ESCALATE）。
    if (isArtifactTestExcluded(p)) continue;
    for (const re of SECURITY_AXIS_PATH_PATTERNS) {
      if (re.test(p)) return { hit: true, reason: `security-axis-path:${p}` };
    }
  }
  const attributed = perFile instanceof Map && paths.every((p) => perFile.has(p));
  if (attributed) {
    for (const p of paths) {
      if (isArtifactTestExcluded(p)) continue; // 测试/制品面不承载安全面风险信号（排除闭集，两轴共享单源）
      const sectionHit = securityAxisContentHit(perFile.get(p));
      if (sectionHit.hit) return sectionHit;
    }
  } else {
    const contentHit = securityAxisContentHit(String(content || ''));
    if (contentHit.hit) return contentHit;
  }
  if (tp) {
    if (tp.securityReviewRequired === true) return { hit: true, reason: 'security-axis-declared:securityReviewRequired=true' };
    if (Array.isArray(tp.securityAxes) && tp.securityAxes.length > 0) return { hit: true, reason: `security-axis-declared:securityAxes=${tp.securityAxes.join(',')}` };
  }
  return { hit: false };
}

// 全 mock（dev-loop:244 + :106-111，FIX-2）：diff 有测试面但无真库/事件断言信号。无测试面 → 非「全 mock」（门全绿由证据完整度闸兜底）。
function isAllMock(paths, content) {
  const testPaths = paths.filter((p) => TEST_FILE_RE.test(p));
  if (testPaths.length === 0) return { hit: false };
  const text = String(content || '');
  for (const re of REAL_EFFECT_MARKERS) {
    if (re.test(text)) return { hit: false }; // 有真库/事件断言 → 非全 mock
  }
  return { hit: true, reason: 'all-mock:test-files-no-real-db/event-assertion(dev-loop:106-111)' };
}

function classifyHigh(paths, content, tp, perFile) {
  const reasons = [];
  const core = isCoreStar(paths); if (core.hit) reasons.push(core.reason);
  // cross 轴已退役（260908 批 T1，判据换轴）：真跨系统保护归升级闸并行相交面探测（§10d）
  const mock = isAllMock(paths, content); if (mock.hit) reasons.push(mock.reason);
  const sec = securityAxisTriggered(paths, content, tp, perFile); if (sec.hit) reasons.push(sec.reason);
  return { hit: reasons.length > 0, reasons };
}

// riskTier 从 git diff 机械重派生为权威源（FIX-1，不信 planner 自填 taskpackage.riskTier，守 P-2 作者≠判官）。
// 向严单调（FIX-5）：git 失败 / criterion 异常 → ambiguous=true、tier=high（绝不吞成 low）。
function deriveDiffTier(paths, content, tp, opts = {}) {
  try {
    if (opts.gitFailed) {
      // git 失败不得沿用 G3「不 block」放行派生（supervisor-gate-check:106-110）→ 不能派生 → 向严（FIX-5）
      return { tier: 'high', ambiguous: true, reasons: ['git-failed:cannot-derive-riskTier→toward-strict(high/ESCALATE),FIX-5'] };
    }
    if (!Array.isArray(paths)) throw new Error('diff paths must be an array');
    const crit = classifyCritical(paths, content, opts.perFile);
    if (crit.hit) return { tier: 'critical', ambiguous: false, reasons: crit.reasons };
    const high = classifyHigh(paths, content, tp || {}, opts.perFile);
    if (high.hit) return { tier: 'high', ambiguous: false, reasons: high.reasons };
    // 绿色通道（FIX-2/P-1）：单系统 + 真库 + 无安全轴 + diff 无 critical/high 闭集命中 = 确定 low（防过严 ATAM-1）。
    return { tier: 'low', ambiguous: false, reasons: ['green-channel:no-critical/high-closed-set-hit(dev-loop:244)'] };
  } catch (error) {
    // FIX-5 BLOCKING：criterion 抛异常被 try/catch 吞成默认 low = 绝不允许（fail 向 APPROVE）→ ambiguous 向严。
    return { tier: 'high', ambiguous: true, reasons: [`criterion-exception:${error.message}→toward-strict(high),FIX-5`] };
  }
}

// ---- git diff 采集（向严语义，与 G3「不 block」区分）----
// 返回 { ok:true, paths, content } ∨ { ok:false, reason }。调用方对 !ok 一律向严（ESCALATE），不沿用 G3 passthrough。
function collectGitDiff(dir) {
  try {
    let base = 'main...HEAD';
    try { execSync('git rev-parse --verify main', { cwd: dir, stdio: 'pipe' }); }
    catch { base = 'HEAD'; } // main 不存在（bootstrap）→ 对 HEAD
    try { execSync('git symbolic-ref -q HEAD', { cwd: dir, stdio: 'pipe' }); }
    catch { base = 'HEAD'; } // detached HEAD → 对 HEAD
    // quotepath=false：--name-only 输出非 ASCII 路径为原始 UTF-8，保逐文件 spawnSync argv 可回传（260803-critical-content-fp T1）。
    const names = execSync(`git -c core.quotepath=false diff ${base} --name-only`, { cwd: dir, encoding: 'utf8' });
    const body = execSync(`git diff ${base}`, { cwd: dir, encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 });
    const paths = names.split(/\r?\n/).map((s) => s.trim()).filter(Boolean);
    return { ok: true, paths, content: body, perFile: collectPerFileDiff(dir, base, paths) };
  } catch (error) {
    return { ok: false, reason: `git-diff-failed:${error.message}` };
  }
}

// 采集层归因（260803-critical-content-fp T1）：逐文件 diff 段（path → section），补上 paths↔body 对应关系。
// fail-safe 向严单调：任何异常（spawnSync error / status≠0 / 在册 path 段为空——一致采集态下列出文件必有非空段，
// 空段 = 采集不一致证据，pathspec magic / TOCTOU 可致）→ 返回 null，调用方整体回退现行全量 body 扫描（最坏情形 = 现状）。
function collectPerFileDiff(dir, base, paths) {
  const perFile = new Map();
  for (const p of paths) {
    const r = spawnSync('git', ['-c', 'core.quotepath=false', 'diff', base, '--', p], { cwd: dir, encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 });
    if (r.error || r.status !== 0) return null;
    const section = String(r.stdout || '');
    if (section.trim() === '') return null;
    perFile.set(p, section);
  }
  return perFile;
}

// ---- bearing fact 消费（FIX-4：唯一 fail-loud 单源 validateFactObject）----

function findFinalBearingFacts(roundsDir) {
  if (!existsSync(roundsDir)) return [];
  const finals = [];
  for (const entry of readdirSync(roundsDir)) {
    const m = /^r([1-9]\d*)\.json$/.exec(entry);
    if (m) finals.push({ round: Number(m[1]), path: join(roundsDir, entry) });
  }
  return finals.sort((a, b) => a.round - b.round);
}

// completed-only round 序数（planner.md:62 机器痕迹）= bearing r<N>.json 个数（熔断计数 FIX-12 的机械输入）。
function countCompletedRounds(roundsDir) {
  return findFinalBearingFacts(roundsDir).length;
}

function failLoud(message) {
  process.stderr.write(`supervisor-verdict-gate FAIL-LOUD: ${message}\n`);
  process.exit(2);
}

// 消费 final（最高轮）bearing fact：必过 validateFactObject（FIX-4）。缺失 → { present:false }（调用方 → BLOCK，FIX-5）；
// 存在但 !ok → fail-loud exit≠0（FIX-4/RC-5 三消费方同源，schema 1.0→2.0 单点改同时 fail-loud）。不自读字段、不另造校验。
function consumeFinalBearingFact(dir, changeId) {
  const roundsDir = resolve(dir, 'delivery/taskpackages', changeId, 'verifier-evidence', 'rounds');
  const finals = findFinalBearingFacts(roundsDir);
  if (finals.length === 0) return { present: false, count: 0 };
  const last = finals[finals.length - 1];
  let fact;
  try {
    fact = JSON.parse(readFileSync(last.path, 'utf8').replace(/^﻿/, ''));
  } catch (error) {
    failLoud(`final bearing fact unreadable (${error.message}): ${last.path}`);
  }
  const expected = { changeId: fact.changeId, phase: fact.phase, round: last.round, invocationId: fact.invocationId };
  const validation = validateFactObject(fact, expected); // 唯一 fail-loud 单源（FIX-4）
  if (!validation.ok) {
    failLoud(`bearing fact failed validateFactObject (single fail-loud source): ${validation.errors.join('; ')}`);
  }
  return {
    present: true,
    fact,
    ok: fact.ok === true,
    ref: `delivery/taskpackages/${changeId}/verifier-evidence/rounds/r${last.round}.json`,
    count: finals.length,
  };
}

// ---- 裁决 ----
// critical/high → ESCALATE（优先级先于证据完整，不被短路）；low/med ∧ 证据完整 → APPROVE（恢复 auto-merge）；
// 证据不完整 ∨ 门红 → BLOCK；连续 ≥2 completed round 仍 BLOCK → 熔断 ESCALATE（FIX-12，向严）。
function decide({ diff, tp, declaredTier, evidenceComplete, completedRounds }) {
  const derivation = deriveDiffTier(diff.paths, diff.content, tp || {}, { gitFailed: !!diff.gitFailed, perFile: diff.perFile });
  const eff = effectiveTier(derivation.tier, declaredTier);
  const riskTier = eff.tier;
  const reasons = [...derivation.reasons];
  if (eff.declared && TIER_RANK[eff.declared] > TIER_RANK[derivation.tier]) reasons.push(`declared-raises:${eff.declared}`);

  let verdict;
  if (riskTier === 'critical' || riskTier === 'high') {
    verdict = 'ESCALATE';
    reasons.push('adjudicate:critical/high→ESCALATE(priority-before-evidence)');
  } else if (!evidenceComplete) {
    verdict = 'BLOCK';
    reasons.push('adjudicate:low/med∧evidence-incomplete→BLOCK');
    if (completedRounds >= 2) {
      verdict = 'ESCALATE';
      reasons.push(`circuit-breaker:${completedRounds}-completed-rounds-still-BLOCK→ESCALATE(FIX-12)`);
    }
  } else {
    verdict = 'APPROVE';
    reasons.push('adjudicate:low/med∧evidence-complete→APPROVE(restore-auto-merge)');
  }

  return {
    verdict,
    riskTier,
    reasons,
    riskTierDerivation: {
      diffTier: derivation.tier,
      declaredTier: eff.declared,
      effectiveTier: riskTier,
      ambiguous: derivation.ambiguous,
      diffReasons: derivation.reasons,
    },
  };
}

// ---- verdict write-once（FIX-3，对偶 verify.mjs publishFact:245/:250 + writeVerdictJson:321-328）----
function writeVerdictOnce(changeDir, verdictObj) {
  mkdirSync(changeDir, { recursive: true });
  const finalPath = resolve(changeDir, 'supervisor-verdict.json');
  const text = JSON.stringify(verdictObj, null, 2) + '\n';
  const tempName = `.supervisor-verdict.${process.pid}.${randomUUID()}.tmp`;
  const tempPath = resolve(changeDir, tempName);
  let fd;
  try {
    fd = openSync(tempPath, 'wx');        // 独占 temp（对偶 publishFact:245）
    writeFileSync(fd, text, 'utf8');
    fsyncSync(fd);
    closeSync(fd);
    fd = undefined;
    linkSync(tempPath, finalPath);        // hardlink → final；final 已存在则 EEXIST = write-once（对偶 publishFact:250）
    try { unlinkSync(tempPath); } catch { /* best effort */ }
    try {
      const dirFd = openSync(changeDir, 'r');
      try { fsyncSync(dirFd); } finally { closeSync(dirFd); }
    } catch { /* 目录 fsync best-effort，平台相关 */ }
  } catch (error) {
    if (fd !== undefined) { try { closeSync(fd); } catch { /* best effort */ } }
    try { unlinkSync(tempPath); } catch { /* best effort */ }
    if (error && error.code === 'EEXIST') {
      // write-once 承重观察点单一（FIX-3/FIX-5）：planner 无 Write 权改历史 verdict；输入不变重跑无意义（重算同果），
      // 真新 verdict 须新 bearing round/invocationId 留痕。不静默覆写。
      failLoud(
        'supervisor-verdict.json already exists (write-once bearing observation point, FIX-3/FIX-5). ' +
        'Re-running with unchanged input is meaningless; a genuinely new verdict requires a new bearing round (new invocationId). ' +
        'Remove the stale verdict explicitly only if a new round warrants it.',
      );
    }
    throw error;
  }
}

// ---- helpers（real-run I/O）----

function readCurrentChangeId(dir) {
  const p = resolve(dir, 'delivery/taskpackages/current/taskpackage.json');
  if (!existsSync(p)) return null;
  try { return JSON.parse(readFileSync(p, 'utf8').replace(/^﻿/, '')).changeId || null; } catch { return null; }
}

function readTaskPackage(dir, changeId) {
  const p = resolve(dir, 'delivery/taskpackages', changeId, 'taskpackage.json');
  if (!existsSync(p)) return {};
  try { return JSON.parse(readFileSync(p, 'utf8').replace(/^﻿/, '')); } catch { return {}; }
}

function collectVerifyLogs(dir) {
  const logs = resolve(dir, '.claude/verify-logs');
  if (!existsSync(logs)) return [];
  return readdirSync(logs)
    .filter((f) => f.endsWith('.log') && statSync(join(logs, f)).size > 0)
    .map((f) => `.claude/verify-logs/${f}`);
}

function parseArgs(argv = process.argv.slice(2)) {
  const args = {};
  for (let i = 0; i < argv.length; i += 1) {
    const token = argv[i];
    if (!token.startsWith('--')) continue;
    const eq = token.indexOf('=');
    if (eq >= 0) { args[token.slice(2, eq)] = token.slice(eq + 1); continue; }
    const key = token.slice(2);
    const next = argv[i + 1];
    if (next && !next.startsWith('--')) { args[key] = next; i += 1; } else { args[key] = true; }
  }
  return args;
}

function buildProvenance({ changeId, decision, bearing, closeoutArtifacts, verifyLogs, completedRounds, originType }) {
  return {
    provenanceVersion: '2.0',
    mechanicalSupervisorGate: {
      ran: true,
      producedBy: 'supervisor-verdict-gate.mjs',
      riskTierDerivation: decision.riskTierDerivation,
      evidenceCompleteness: {
        bearingFactRef: bearing.present ? bearing.ref : null,
        bearingFactOk: bearing.present ? bearing.ok === true : false,
        completedRounds,
        closeoutArtifactsChecked: closeoutArtifacts,
        verifyLogsPresent: verifyLogs.length > 0,
        // F-05 upstreamType∨type 双键镜像（ARQ-077）：informational provenance，**非** risk 判据输入（risk 锚 diff 不锚 planner 自填，P-2）。
        originType: originType || null,
      },
    },
    verifyLogRefs: verifyLogs,
    verifierEvidenceRefs: bearing.present ? [bearing.ref] : [],
    closeoutArtifactsChecked: closeoutArtifacts,
  };
}

function main() {
  const args = parseArgs();
  const changeId = args['change-id'] || readCurrentChangeId(projectDir);
  if (!changeId) {
    process.stderr.write('supervisor-verdict-gate: no TaskPackage context (no --change-id, no current taskpackage); nothing to do.\n');
    process.exit(0);
  }
  const tp = readTaskPackage(projectDir, changeId);
  const declaredTier = tp.riskTier || null;
  const originType = resolveOriginType(tp.origin); // F-05 双键（lib 单源），informational

  // riskTier 从 git diff 机械重派生（FIX-1）。git 失败 → gitFailed → deriveDiffTier 向严（FIX-5）。
  const git = collectGitDiff(projectDir);
  const diff = git.ok
    ? { paths: git.paths, content: git.content, perFile: git.perFile, gitFailed: false }
    : { paths: [], content: '', perFile: null, gitFailed: true };

  // 证据完整度（FIX-4 单源 fail-loud + FIX-5 缺失→BLOCK）。consumeFinalBearingFact 对 invalid fact fail-loud exit 2。
  const bearing = consumeFinalBearingFact(projectDir, changeId);
  const closeoutArtifacts = ['docs-delta.md', 'review-packet.md'].map((f) => `delivery/changes/dev/${changeId}/${f}`);
  const closeoutPresent = closeoutArtifacts.every((ref) => existsSync(resolve(projectDir, ref)));
  const verifyLogs = collectVerifyLogs(projectDir);
  const evidenceComplete = bearing.present && bearing.ok === true && closeoutPresent && verifyLogs.length > 0;
  const completedRounds = bearing.count || countCompletedRounds(resolve(projectDir, 'delivery/taskpackages', changeId, 'verifier-evidence', 'rounds'));

  const decision = decide({ diff, tp, declaredTier, evidenceComplete, completedRounds });
  const provenance = buildProvenance({ changeId, decision, bearing, closeoutArtifacts, verifyLogs, completedRounds, originType });
  const verdictObj = {
    verdict: decision.verdict,
    riskTier: decision.riskTier,
    decidedBy: 'supervisor-verdict-gate.mjs (mechanical, script fs-write)',
    producedBy: 'supervisor-verdict-gate.mjs',
    ts: new Date().toISOString(),
    changeId,
    originType,
    adjudication: { ...decision, circuitBreakerCompletedRounds: completedRounds, evidenceComplete },
    provenance,
  };

  writeVerdictOnce(resolve(projectDir, 'delivery', 'changes', 'dev', changeId), verdictObj);
  console.log(JSON.stringify({
    ok: decision.verdict === 'APPROVE',
    gate: 'supervisor-verdict-gate',
    changeId,
    verdict: decision.verdict,
    riskTier: decision.riskTier,
    reasons: decision.reasons,
    evidenceComplete,
    completedRounds,
    verdictRef: `delivery/changes/dev/${changeId}/supervisor-verdict.json`,
  }, null, 2));
  process.exit(0);
}

// ---- ARQ-114 结构化自检（必带，非可选；对偶 governance-gate-shared runExtractTargetsSelfCheck / merge-driver runQuickChecksSelfCheck）----

function makeSelfFact() {
  const now = new Date().toISOString();
  return {
    schemaVersion: '1.0',
    artifactType: 'verifier-round-fact',
    authority: 'bearing',
    producedBy: 'verify.mjs',
    changeId: '260730-self-check',
    phase: 'scoped',
    round: 1,
    invocationId: '11111111-1111-4111-8111-111111111111',
    startedAt: now,
    completedAt: now,
    observedGateExit: 0,
    ok: true,
    results: [{ id: 'self', gate: 'self', ok: true, exit: 0, exitCode: 0, stdout: '', stderr: '', summary: 'ok' }],
    error: null,
  };
}

function selfCheck() {
  let pass = 0;
  let failCount = 0;
  const ok = (cond, label) => { if (cond) pass += 1; else { failCount += 1; console.error(`FAIL [${label}]`); } };
  const identity = (f) => ({ changeId: f.changeId, phase: f.phase, round: f.round, invocationId: f.invocationId });

  // critical 闭集各模式 → critical（删数据闭集 + 契约/分区表路径，本仓化）
  ok(deriveDiffTier(['docs/contracts/interface-contract.md'], '', {}).tier === 'critical', 'critical-contracts-path');
  ok(deriveDiffTier(['firmware/partitions.csv'], '', {}).tier === 'critical', 'critical-partition-table-path');
  ok(deriveDiffTier(['firmware/sdkconfig'], '', {}).tier === 'critical', 'critical-sdkconfig-path');
  ok(deriveDiffTier(['firmware/main/storage.c'], 'esp_partition_erase_range(part, 0, size)', {}).tier === 'critical', 'critical-partition-erase');
  ok(deriveDiffTier(['firmware/main/storage.c'], 'esp_partition_write(part, 0, buf, len)', {}).tier === 'critical', 'critical-partition-write');
  ok(deriveDiffTier(['firmware/main/ota.c'], 'esp_ota_set_boot_partition(partition)', {}).tier === 'critical', 'critical-ota-switch');
  ok(deriveDiffTier(['firmware/main/flash.c'], 'esp_flash_erase_region(0, size)', {}).tier === 'critical', 'critical-flash-erase');
  ok(deriveDiffTier(['server/shim/store.py'], 'nvs_flash_erase()', {}).tier === 'critical', 'critical-nvs-erase');

  // high 闭集（core 面 + 全 mock + 安全轴；跨系统轴退役见 classifyHigh 注——保护归升级闸相交探测）
  // 契约路径同时命中 critical 闭集与 high core 面——critical 先检、宁滥勿漏 → critical（向严）。
  ok(deriveDiffTier(['docs/contracts/interface-contract.md'], '', {}).tier === 'critical', 'critical-docs-contracts-dominates');
  ok(deriveDiffTier(['firmware/boards/board_x/board.c'], '', {}).tier === 'high', 'high-firmware-boards');
  ok(deriveDiffTier(['firmware/main/board_config.c'], '', {}).tier === 'low', 'low-firmware-main-plain');
  // 退役轴回归夹具（260908 批 V1）：原目录面跨系统两例改判 low——目录面启发式退役、真跨系统保护归相交探测
  ok(deriveDiffTier(['firmware/main/a.c', 'server/shim/b.py'], '', {}).tier === 'low', 'retired-impl-axis-diff-now-low');
  ok(deriveDiffTier(['firmware/main/a.c'], '', { subsystem: ['firmware/main', 'server/shim'] }).tier === 'low', 'retired-impl-axis-subsystem-now-low');
  ok(deriveDiffTier(['firmware/main/a.test.c'], 'expect(result).toBe(1)', {}).tier === 'high', 'high-all-mock');
  ok(deriveDiffTier(['firmware/main/auth.c'], 'login auth handler', {}).tier === 'high', 'high-security-axis-diff');
  ok(deriveDiffTier(['firmware/main/a.c'], '', { securityReviewRequired: true }).tier === 'high', 'high-security-axis-declared');

  // 安全轴假阳反例电池（字母边界杀 author 族 + 命中词闭集杀门自产词汇）：
  ok(deriveDiffTier(['firmware/main/x.c'], '"actor":"acceptance-author"', {}).tier === 'low', 'fp-acceptance-author-actor-not-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'AuthSession shape reference in docs', {}).tier === 'low', 'fp-authsession-reference-not-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'Co-Authored-By: Claude <noreply@anthropic.com>', {}).tier === 'low', 'fp-co-authored-by-not-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'security-axis-diff:auth', {}).tier === 'low', 'fp-gate-reason-string-not-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], '跨系统 2 impl + auth-axis heuristic', {}).tier === 'low', 'fp-auth-axis-meta-not-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], '"authority":"bearing"', {}).tier === 'low', 'fp-authority-bearing-field-not-hit');
  // 向严正例：paths 承重 + OAuth 族 content 覆盖
  ok(deriveDiffTier(['firmware/main/reset_password.c'], '', {}).tier === 'high', 'security-path-password-hit');
  ok(deriveDiffTier(['server/shim/oauth_client.py'], '', {}).tier === 'high', 'security-path-oauth-client-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'OAuth2 token endpoint', {}).tier === 'high', 'security-content-oauth2-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'oauth-client registration', {}).tier === 'high', 'security-content-oauth-client-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'X-Auth-Header required', {}).tier === 'high', 'security-content-x-auth-header-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'costAudit report', {}).tier === 'high', 'security-content-costAudit-camel-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'reauthorize flow docs', {}).tier === 'low', 'known-narrowing-reauthorize-not-hit');

  // 明确 low 绿色通道 → low（硬件副作用信号 + 断言面，防过严）
  ok(deriveDiffTier(
    ['firmware/main/storage.c', 'firmware/main/storage.test.c'],
    'nvs_flash_write(); TEST_ASSERT_EQUAL(0, rc)',
    { subsystem: 'firmware/main' },
  ).tier === 'low', 'low-green-channel');

  // max(diff,declared) 只升不降
  ok(effectiveTier('critical', 'low').tier === 'critical', 'max-diff-critical-beats-declared-low');
  ok(effectiveTier('low', 'high').tier === 'high', 'max-declared-high-raises-diff-low');
  ok(effectiveTier('high', 'low').tier === 'high', 'max-declared-cannot-lower-diff-high');

  // 翻 securityReviewRequired=false 无效（diff 命中仍在）
  ok(deriveDiffTier(['firmware/main/auth.c'], 'auth login', { securityReviewRequired: false }).tier === 'high', 'security-flip-false-ineffective');

  // validateFactObject fail-loud 单源：伪造/缺字段 → 非静默绿
  const good = makeSelfFact();
  ok(validateFactObject(good, identity(good)).ok === true, 'validateFactObject-compliant-ok');
  const forged = { ...makeSelfFact(), ok: true, observedGateExit: 1 }; // ok:true 但 exit 1 → 不自洽
  ok(validateFactObject(forged, identity(forged)).ok === false, 'validateFactObject-forged-not-silent-green');
  ok(validateFactObject({ schemaVersion: '1.0' }, { changeId: 'x', phase: 'scoped', round: 1, invocationId: '11111111-1111-4111-8111-111111111111' }).ok === false, 'validateFactObject-missing-fields-fail-loud');

  // 向严分支：git 失败 → 歧义向严 high；criterion 异常不吞成 low
  ok(deriveDiffTier([], '', {}, { gitFailed: true }).ambiguous === true, 'git-failed-ambiguous');
  ok(deriveDiffTier([], '', {}, { gitFailed: true }).tier === 'high', 'git-failed-tier-high');
  ok(deriveDiffTier(null, '', {}).ambiguous === true, 'criterion-exception-ambiguous');
  ok(deriveDiffTier(null, '', {}).tier !== 'low', 'criterion-exception-never-low');

  // 裁决规则：critical/high→ESCALATE 先于证据完整（不被短路）；low∧完整→APPROVE；不完整→BLOCK；熔断 ≥2 轮→ESCALATE
  ok(decide({ diff: { paths: ['docs/contracts/x.md'], content: '', gitFailed: false }, tp: {}, declaredTier: null, evidenceComplete: true, completedRounds: 1 }).verdict === 'ESCALATE', 'adjudicate-critical-ESCALATE-even-if-evidence-complete');
  ok(decide({ diff: { paths: ['firmware/main/x.c', 'firmware/main/x.test.c'], content: 'nvs_flash_write()', gitFailed: false }, tp: { subsystem: 'firmware/main' }, declaredTier: 'low', evidenceComplete: true, completedRounds: 1 }).verdict === 'APPROVE', 'adjudicate-low-complete-APPROVE');
  ok(decide({ diff: { paths: ['firmware/main/x.c'], content: 'nvs_flash_', gitFailed: false }, tp: { subsystem: 'firmware/main' }, declaredTier: 'low', evidenceComplete: false, completedRounds: 1 }).verdict === 'BLOCK', 'adjudicate-low-incomplete-BLOCK');
  ok(decide({ diff: { paths: ['firmware/main/x.c'], content: 'nvs_flash_', gitFailed: false }, tp: { subsystem: 'firmware/main' }, declaredTier: 'low', evidenceComplete: false, completedRounds: 2 }).verdict === 'ESCALATE', 'circuit-breaker-2-rounds-BLOCK-to-ESCALATE');
  ok(decide({ diff: { paths: [], content: '', gitFailed: true }, tp: {}, declaredTier: 'low', evidenceComplete: true, completedRounds: 1 }).verdict === 'ESCALATE', 'git-failed-ESCALATE');

  // critical-content 归因电池（正+负成组）：测试/制品面段不命中（perFile 归因 + 排除闭集）
  const attrSpec = new Map([['firmware/main/test/storage.acceptance.test.c', '+esp_partition_erase_range(part, 0, size); // test teardown']]);
  ok(deriveDiffTier(['firmware/main/test/storage.acceptance.test.c'], 'esp_partition_erase_range(part, 0, size);', { subsystem: 'firmware/main' }, { perFile: attrSpec }).tier === 'low', 'fp-attr-spec-partition-erase-not-hit');
  const attrRaw = new Map([['firmware/main/test/store.test.c', '+nvs_flash_erase(); // fixture reset']]);
  ok(deriveDiffTier(['firmware/main/test/store.test.c'], 'nvs_flash_erase();', { subsystem: 'firmware/main' }, { perFile: attrRaw }).tier === 'low', 'fp-attr-spec-nvs-erase-not-hit');
  const attrPacket = new Map([['delivery/changes/dev/x/review-packet.md', '+cleanup uses nvs_flash_erase on test data']]);
  ok(deriveDiffTier(['delivery/changes/dev/x/review-packet.md'], 'cleanup uses nvs_flash_erase on test data', {}, { perFile: attrPacket }).tier === 'low', 'fp-attr-changes-prose-not-hit');
  const attrEvents = new Map([['delivery/taskpackages/x/pod-events.jsonl', '+{"summary":"nvs_flash_erase cleanup"}']]);
  ok(deriveDiffTier(['delivery/taskpackages/x/pod-events.jsonl'], '{"summary":"nvs_flash_erase cleanup"}', {}, { perFile: attrEvents }).tier === 'low', 'fp-attr-taskpackages-prose-not-hit');
  const attrPlanning = new Map([['delivery/workstreams/ws/progress.md', '+gate hit nvs_flash_erase token']]);
  ok(deriveDiffTier(['delivery/workstreams/ws/progress.md'], 'gate hit nvs_flash_erase token', {}, { perFile: attrPlanning }).tier === 'low', 'fp-attr-planning-prose-not-hit');
  const attrMixed = new Map([
    ['firmware/main/test/z.acceptance.test.c', '+esp_partition_erase_range(p, 0, n);'],
    ['firmware/main/services/x.c', '+esp_partition_read(p, 0, buf, n);'],
  ]);
  ok(deriveDiffTier(['firmware/main/test/z.acceptance.test.c', 'firmware/main/services/x.c'], 'esp_partition_erase_range(p,0,n); esp_partition_read(p,0,buf,n);', { subsystem: 'firmware/main' }, { perFile: attrMixed }).tier === 'low', 'fp-attr-mixed-package-not-hit');
  // 向严正案——生产面删数据仍 critical（判据不减）
  const attrProd = new Map([['firmware/main/services/contact.c', '+esp_partition_erase_range(part, 0, size); // prod wipe']]);
  ok(deriveDiffTier(['firmware/main/services/contact.c'], 'esp_partition_erase_range(part, 0, size);', {}, { perFile: attrProd }).tier === 'critical', 'attr-prod-partition-erase-still-critical');
  const attrScript = new Map([['server/scripts/wipe.py', '+nvs_flash_erase()']]);
  ok(deriveDiffTier(['server/scripts/wipe.py'], 'nvs_flash_erase()', {}, { perFile: attrScript }).tier === 'critical', 'attr-scripts-nvs-erase-still-critical');
  const attrDrop = new Map([['firmware/main/server/x.c', '+esp_flash_erase_region(0, size);']]);
  ok(deriveDiffTier(['firmware/main/server/x.c'], 'esp_flash_erase_region(0, size);', {}, { perFile: attrDrop }).tier === 'critical', 'attr-prod-flash-erase-still-critical');
  const attrContract = new Map([['docs/contracts/x.md', '+新增字段']]);
  ok(deriveDiffTier(['docs/contracts/x.md'], '新增字段', {}, { perFile: attrContract }).tier === 'critical', 'attr-contracts-path-still-critical');
  // fail-safe 回退案——perFile=null / 覆盖不全 → 全量扫描 = 向严方向固话
  ok(deriveDiffTier(['firmware/main/x.c'], 'esp_partition_erase_range(p,0,n)', {}, { perFile: null }).tier === 'critical', 'attr-fallback-null-perFile-whole-body');
  const attrPartial = new Map([['firmware/main/a.c', '+const a = 1;']]); // b.c 未覆盖
  ok(deriveDiffTier(['firmware/main/a.c', 'firmware/main/b.c'], 'esp_partition_erase_range(p,0,n)', {}, { perFile: attrPartial }).tier === 'critical', 'attr-fallback-partial-coverage-whole-body');

  // auth 轴归因电池（假阳重放 + 正案保全 + 固话 + 混合回退）
  const authAttrChanges = new Map([['delivery/changes/dev/x/review-packet.md', '+authz 轴证据：worker 以 SYSTEM_AUTH 运行，无新增 auth 面']]);
  ok(deriveDiffTier(['delivery/changes/dev/x/review-packet.md'], 'authz 轴证据：worker 以 SYSTEM_AUTH 运行，无新增 auth 面', {}, { perFile: authAttrChanges }).tier === 'low', 'fp-auth-attr-changes-prose-not-hit');
  const authAttrEvents = new Map([['delivery/taskpackages/x/pod-events.jsonl', '+{"summary":"security-axis-diff:auth 假阳记录"}']]);
  ok(deriveDiffTier(['delivery/taskpackages/x/pod-events.jsonl'], '{"summary":"security-axis-diff:auth 假阳记录"}', {}, { perFile: authAttrEvents }).tier === 'low', 'fp-auth-attr-taskpackages-prose-not-hit');
  const authAttrPlanning = new Map([['delivery/workstreams/ws/progress.md', '+前次假阳记录行：security-axis-diff:auth，human 风险接受']]);
  ok(deriveDiffTier(['delivery/workstreams/ws/progress.md'], '前次假阳记录行：security-axis-diff:auth，human 风险接受', {}, { perFile: authAttrPlanning }).tier === 'low', 'fp-auth-attr-planning-prose-not-hit');
  const authAttrSpec = new Map([['firmware/main/test/x.acceptance.test.c', '+// fixture: auth session seeded; nvs_flash_write()']]);
  ok(deriveDiffTier(['firmware/main/test/x.acceptance.test.c'], '// fixture: auth session seeded; nvs_flash_write()', { subsystem: 'firmware/main' }, { perFile: authAttrSpec }).tier === 'low', 'fp-auth-attr-spec-prose-not-hit');
  // 正案保全——paths 承重轴零改 / 生产面 content 仍扫 / declared 轴零改 / 归因失败回退向严
  ok(deriveDiffTier(['firmware/main/auth/guard.c'], 'void guard(void) {}', { subsystem: 'firmware/main' }, { perFile: new Map([['firmware/main/auth/guard.c', '+void guard(void) {}']]) }).tier === 'high', 'auth-path-axis-still-hit');
  const authAttrProd = new Map([['firmware/main/services/session.c', '+#include "../auth/guard.h"  // requireAuth']]);
  ok(deriveDiffTier(['firmware/main/services/session.c'], '#include "../auth/guard.h"  // requireAuth', { subsystem: 'firmware/main' }, { perFile: authAttrProd }).tier === 'high', 'auth-content-prod-still-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'int x = 1;', { subsystem: 'firmware/main', securityReviewRequired: true }, { perFile: new Map([['firmware/main/x.c', '+int x = 1;']]) }).tier === 'high', 'auth-declared-axis-still-hit');
  ok(deriveDiffTier(['firmware/main/x.c'], 'worker runs with auth context', {}, { perFile: null }).tier === 'high', 'auth-fallback-null-perFile-whole-body');
  // 自产 reason 旧形态被制品面引用 + 归因 → 不触发
  const authSelfReason = new Map([['delivery/changes/dev/x/progress.md', '+gate reason 引用：security-axis-diff:auth（前次实例记录）']]);
  ok(deriveDiffTier(['delivery/changes/dev/x/progress.md'], 'gate reason 引用：security-axis-diff:auth（前次实例记录）', {}, { perFile: authSelfReason }).tier === 'low', 'auth-self-reason-quoted-attributed-not-hit');
  // 生产面 prose（固件 CLAUDE.md 形态）含 auth 词汇 + 归因 → 仍触发（向严保全）
  const authResidualClaudeMd = new Map([['firmware/main/CLAUDE.md', '+Local Auth guard covers device routes']]);
  ok(deriveDiffTier(['firmware/main/CLAUDE.md'], 'Local Auth guard covers device routes', { subsystem: 'firmware/main' }, { perFile: authResidualClaudeMd }).tier === 'high', 'auth-residual-prod-claudemd-still-hit');
  // 混合回退——归因覆盖不全 → 全量扫描 = 向严
  const authAttrPartial = new Map([['delivery/changes/dev/x/review-packet.md', '+auth prose']]); // 第二路径未覆盖
  ok(deriveDiffTier(['delivery/changes/dev/x/review-packet.md', 'firmware/main/y.c'], 'auth prose', {}, { perFile: authAttrPartial }).tier === 'high', 'auth-fallback-partial-coverage-whole-body');

  // ARQ-260916-supervisor-auth-fp-author 路径轴收窄电池（paths 轴消费测试/制品面排除闭集，与 content 轴共享单源）
  // 假阳重放（实证形态）：input-brief 制品路径含 author 子串，归因可用 → paths 轴跳过、content 轴跳过 → low
  ok(deriveDiffTier(
    ['delivery/taskpackages/x/input-brief-acceptance-author.md'],
    'brief prose',
    { subsystem: 'delivery' },
    { perFile: new Map([['delivery/taskpackages/x/input-brief-acceptance-author.md', '+brief prose']]) },
  ).tier === 'low', 'fp-path-input-brief-author-attributed-not-hit');
  // 假阳重放：同路径、归因缺失 → 回退全量 content 扫描（内容无 auth 词）→ 仍 low（路径轴不再单独触发）
  ok(deriveDiffTier(['delivery/taskpackages/x/input-brief-per-file.md', 'delivery/taskpackages/x/input-brief-acceptance-author.md'], 'brief prose only', {}).tier === 'low', 'fp-path-input-brief-author-fallback-not-hit');
  // 向严保全：回退模式下制品面 content 含真 auth 词仍触发——收窄只作用于路径形态，content 全量扫描不减
  ok(deriveDiffTier(['delivery/taskpackages/x/input-brief-acceptance-author.md'], 'auth token refresh flow', {}).tier === 'high', 'auth-excluded-path-content-fallback-still-hit');
  // 向严保全：生产面 auth 路径命中零改（回归锚）
  ok(deriveDiffTier(['firmware/main/auth/guard.c'], '', {}).tier === 'high', 'auth-path-axis-still-hit-260916');

  console.log(JSON.stringify({ selfCheck: 'supervisor-verdict-gate', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
  if (failCount > 0) process.exit(1);
}

function isMain() {
  return process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url);
}

// 导出纯函数供 G6 第二方重算复用（supervisor-gate-check.mjs FIX-6）——单一机械源，防 canonical↔hook 漂移（RC-7②）。
export {
  TIER_RANK,
  normalizeTier,
  maxTier,
  effectiveTier,
  classifyCritical,
  classifyHigh,
  deriveDiffTier,
  collectGitDiff,
  countCompletedRounds,
  decide,
};

// isMain() 守卫承重：本闸被 supervisor-gate-check.mjs import 复用纯函数时（FIX-6 第二方重算），import 不得触发本闸
// self-check/main——process.argv 全进程共享，若无 isMain() 守卫，宿主跑 --self-check 会误触发本闸 selfCheck()（import 副作用）。
if (isMain() && process.argv.includes('--self-check')) selfCheck();
else if (isMain()) main();
