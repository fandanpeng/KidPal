#!/usr/bin/env node
// pre-merge-escalation-gate.mjs — 前置升级闸（auto-merge 前的机械判，非门、非 merge 执行者）
//
// 治理声明：owner=architect · approver=human · G3（DEV-GOVERNANCE §6 强制执行层行覆盖）。
// 批准物：源仓 delivery/review/260831-gov-review-auto-merge-local-main/PROPOSAL.md（FROZEN v1.0，T1）
//   —— 同步自源仓 commit 6154a6c（治理迁移指南 §4.3 同步通道）；契约面路径按 KidPal 适配为 docs/contracts/**。
// 分工：本脚本只重算「例外清单是否命中」，命中即 exit 1 且报命中项；放行 exit 0。**合并前必经，
//   ——它放行 merge-driver 才跑**（merge-driver 在 processPod Step 1 前 invoke，见 PROPOSAL T2）。
//
// 例外清单（FROZEN §2 T1，按档适用）：
//   两档通用：
//     1. diff 触 docs/contracts/** 且同批无契约合并凭据（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖，三件联立）→ BLOCK（261015 批：机械凭据替代人签门；契约变更 = 架构变更的一种，走 /arch-change）
//     2. 源 commit 落在 main/master 上（off-main 隔离被破坏）：无独立分支可合 → BLOCK
//     3. change 存在未 resolve 的 agent-escalation 或 blocked 态 → BLOCK
//   dev-loop 档：
//     4. riskTier ∈ {high, critical}（读 supervisor-verdict.json）→ BLOCK
//     5. supervisor APPROVE provenance 不可机验 → BLOCK
//   Track B 档（治理态无 supervisor-verdict，替换为）：
//     6. architect GREEN 机锚缺失（PROPOSAL Status 标 FROZEN + §7 GREEN + landing-verdict 在产）→ BLOCK
//   两档通用（T1，261015-gov-review-changes-taxonomy 齿二）：
//     7. 合并结果树含废止路径残件（波段一 delivery/changes/dev；波段二随 T6 第 6 条扩为整伞
//        delivery/changes——dev lane 已由 261014 批撤销、伞已解散；scope-map 齿一拦 agent 会话写入，
//        本齿拦 git merge 通道带入旧布局分支）→ BLOCK，指引归位真家（映射见 delivery/archive/README.md）
//
// 用法：
//   node .claude/scripts/pre-merge-escalation-gate.mjs <changeId> [--source-ref <ref>] [--project-dir <dir>]
//   node .claude/scripts/pre-merge-escalation-gate.mjs <changeId> --human-approval-ref <eventId>  # ESCALATE 人批放行（核验 pod-events human-decision 事件）
//   node .claude/scripts/pre-merge-escalation-gate.mjs --self-check
//
// 退出码：0 放行 / 1 命中（BLOCK）/ 2 用法错。

import { execFileSync, execSync } from 'node:child_process';
import { existsSync, readFileSync, writeFileSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { resolveHumanApproval, VERDICT_STATUS_RE, isFrozen, extractManifestBlock } from './lib/governance-gate-shared.mjs'; // T6（260908 批）：五要素核验单源迁 lib，此处具名导入（函数体与 gate 原实现逐字一致 + deps.decision 参数化）；VERDICT_STATUS_RE 为 T2（260920 批）迁入的 verdict 行判据单源；isFrozen / extractManifestBlock 为 T15（261015 批）契约凭据解析单源

const projectDirOf = () => resolve(process.env.CLAUDE_PROJECT_DIR || process.cwd());

// ---- 纯函数：输入可注入，self-check 可测 ----

/** git rev-parse 一个 ref，失败返回 null。 */
function revParse(execFn, ref) {
  try {
    return execFn(`git rev-parse --verify ${ref}`).trim();
  } catch {
    return null;
  }
}

/** 判 a 是否 main/master 的祖先或等于（即 a 已含在 main 中 → 无独立可合分支）。 */
function sourceReachableFromMain(execFn, source) {
  const mainRef = revParse(execFn, 'main') || revParse(execFn, 'master');
  if (!mainRef) return { ok: null, reason: 'no-main-ref' }; // fail-closed：无 main ref 无法判，交上层
  const src = revParse(execFn, source);
  if (!src) return { ok: true, reason: 'source-absent' }; // 源 ref 不存在（未 commit）→ 更应 BLOCK 由 feature-branch 前置保证
  try {
    // --is-ancestor：src 是 main 的祖先 → src 已含在 main 里，无可合并内容（P4 破坏）
    execFn(`git merge-base --is-ancestor ${src} ${mainRef}`);
    return { ok: true, reason: 'source-on-main' };
  } catch {
    return { ok: false, reason: 'source-diverged' };
  }
}

/** 读 supervisor-verdict.json（dev-loop 档），缺失返回 null。 */
function readSupervisorVerdict(dir, changeId) {
  const p = join(dir, 'delivery', 'taskpackages', changeId, 'supervisor-verdict.json');
  if (!existsSync(p)) return null;
  try {
    return JSON.parse(readFileSync(p, 'utf8'));
  } catch {
    return null;
  }
}

// resolveHumanApproval 已迁 lib/governance-gate-shared.mjs（T6，260908 批 B8 并入）——本文件具名导入。
// 五要素人批事件核验（approve-escalated-merge）；T5 defer/accept-risk 锚核验复用同函数、decision 参数化。

/** 读治理 PROPOSAL §7 与 Status 行 + landing-verdict（Track B 档）。 */
// T2（260920-gov-review-verdict-anchor-align，FROZEN 2026-09-20）：green 判定抽为纯函数（self-check 可测）——
// 逐行 VERDICT_STATUS_RE、**取最后命中 token**（后出者胜，与 deriveLanded 的 :189 覆写语义一致）。
function isGreenVerdict(text) {
  let lastToken = '';
  for (const line of text.split('\n')) {
    const m = VERDICT_STATUS_RE.exec(line);
    if (m) lastToken = m[1].toUpperCase();
  }
  return lastToken === 'GREEN';
}

// 261026 批 T2（ARQ-261021-merge-gate-first-merge-blindspot）：契约凭据 verdict 锚取终态（后出者胜，与
// isGreenVerdict / deriveLanded 同语义）——landing-verdict 按 append-only 留 FAIL→PASS 轨迹时不再被首匹配遮蔽。
function lastVerdictLine(text) {
  let last = null;
  for (const line of text.split('\n')) if (VERDICT_STATUS_RE.test(line)) last = line;
  return last;
}

function readGovAnchors(dir, changeId) {
  // 261015-gov-review-changes-taxonomy T6 第 6 条：PROPOSAL/verdict 路径拼接随新树改 delivery/review/
  const proposalPath = join(dir, 'delivery', 'review', changeId, 'PROPOSAL.md');
  const landingVerdictPath = join(dir, 'delivery', 'review', changeId, 'landing-verdict.md');
  if (!existsSync(proposalPath)) return null;
  const text = readFileSync(proposalPath, 'utf8');
  const frozen = /^[>\s]*\*\*状态：FROZEN v\d+\.\d+/m.test(text); // 260905 T4：容忍 `> ` 引用前缀，与行首形态双命中（既有错位归位，260905-gov-review-verify-gates-align）
  // §7 含判官 GREEN verdict（260920-gov-review-verdict-anchor-align T2，FROZEN 2026-09-20）：
  // 改用 lib 单源 VERDICT_STATUS_RE（行首判据），**取最后命中的 token**（后出者胜，与 deriveLanded
  // 的 :189 覆写语义一致——防「GREEN 后重开」场景 fail-open）。token 集收 {GREEN}（R1-F2：删 PASS——
  // 260918 批 R1:176 已论证否决：spoof 面、判官语法无 PASS）。**裸 GREEN 分支删除**（ARQ-260909 原
  // 病灶：全文 token 匹配 + 样板行「GREEN 结论前必须在场」自带 GREEN ⇒ 恒真）。
  // RE 须容忍 token 前后的反引号（批① §7 的 `**Verdict：\`GREEN\`**` 形态为真实判官输出）。
  const green = isGreenVerdict(text);
  const landingVerdict = existsSync(landingVerdictPath);
  return { frozen, green, landingVerdict };
}

/** 枚举 contracts diff：--name-only <source>..<target>，命中 contracts/** 返回 true。 */
function hasContractsDiff(execFn, source, target) {
  try {
    const out = execFn(`git diff --name-only ${source}..${target}`);
    return out.split('\n').some((l) => l.startsWith('docs/contracts/'));
  } catch {
    return false;
  }
}

/**
 * T15（261015-gov-review-face-loop-unify）：契约合并机械凭据（三件联立）——ARQ-260923-contract-merge-deadpath 建议①兑现。
 * 同批存在 arch-review 目录 PROPOSAL：① FROZEN（lib isFrozen 单源）+ ② 同目录 landing-verdict PASS（VERDICT_STATUS_RE 单源）
 * + ③ manifest（lib extractManifestBlock 单源）覆盖全部契约 diff 路径 → 放行；否则 fail-closed 点名。
 * 解析一律复用 lib 单源（R1-F5：不另写解析，防 260918/260920 verdict-anchor 病灶复发）。
 */
function hasArchLoopContractEvidence(execFn, source, target, projectDir, deps = {}) {
  const readVerdict = deps.readVerdict || ((p) => (existsSync(p) ? readFileSync(p, 'utf8') : null));
  let diffOut;
  try {
    diffOut = execFn(`git diff --name-only ${source}..${target}`);
  } catch {
    return { ok: false, reason: 'contract-evidence: git diff 失败（fail-closed）', contractPaths: [] };
  }
  const lines = diffOut.split('\n').map((l) => l.trim()).filter(Boolean);
  const contractPaths = lines.filter((l) => l.startsWith('docs/contracts/'));
  if (contractPaths.length === 0) return { ok: true, reason: 'no-contract-diff', contractPaths };
  const proposalPaths = lines.filter((l) => /delivery\/review\/\d{6}-(?:gov|arch)-review-[^/]+\/PROPOSAL\.md$/.test(l)); // 261015 批 T6 第 6 条：契约凭据候选随新树改 review 形态
  const checked = [];
  for (const proposalPath of proposalPaths) {
    const dir = proposalPath.slice(0, proposalPath.lastIndexOf('/'));
    // 261026 批 T1（ARQ-261017-merge-gate-contract-evidence-ref）：候选 PROPOSAL 由构造必在 source 侧
    // （proposalPaths 来自 source..target diff）——source 优先、target 兜底（复用/已落批场景）。
    let proposalText = null;
    let refUsed = null;
    for (const ref of [source, target]) {
      try { proposalText = execFn(`git show ${ref}:${proposalPath}`); refUsed = ref; break; } catch { /* 下一 ref */ }
    }
    if (proposalText === null) { checked.push({ dir, refError: true, frozen: false, verdict: false, covered: false }); continue; }
    const frozen = proposalText ? isFrozen(proposalText) : false;
    const manifest = proposalText ? extractManifestBlock(proposalText) : new Set();
    const covered = contractPaths.every((p) => manifest.has(p));
    const vText = readVerdict(join(projectDir, dir, 'landing-verdict.md'));
    const vLine = vText ? lastVerdictLine(vText) : null;
    const verdict = Boolean(vLine && /\b(PASS|GREEN)\b/i.test(vLine));
    // 261026 批 T2（ARQ-261017 建议 4(c)）：verdict 在场但 VERDICT_STATUS_RE 命中 0 行 → 明细点名 verdictFormatMiss，
    // 连同生效的 refUsed 一起进「已检查」JSON——判据失败时说清「哪一件的形态不对」，消掉归因误导。
    const verdictFormatMiss = Boolean(vText) && !vLine;
    checked.push({ dir, refUsed, frozen, verdict, covered, ...(verdictFormatMiss ? { verdictFormatMiss } : {}) });
    if (frozen && verdict && covered) {
      return { ok: true, reason: `contract-evidence via ${dir}`, contractPaths };
    }
  }
  return {
    ok: false,
    reason: checked.length && checked.every((c) => c.refError)
      ? `contract-evidence: 候选 PROPOSAL 在 source/target 两 ref 均不可读（疑似取件 ref 或路径错，非“缺凭据”）；契约路径 = ${contractPaths.join(', ')}`
      : `contract-evidence: 无候选满足三件联立（FROZEN + PASS verdict + manifest 覆盖）；契约路径 = ${contractPaths.join(', ')}；已检查 = ${checked.length ? JSON.stringify(checked) : '无候选 PROPOSAL（同批无 arch-review 批）'}`,
    contractPaths,
  };
}

/**
 * T5（260908-gov-review-merge-chain-hardening，A-2 已裁：门内加项；A-8 已裁：放行方向——只拦可识别形态）：
 * 契约/治理 canonical 改动「交回→在册」机械核验。扫描 review-packet Findings 表行：
 * 行含字面 `| hard-violation |` 且行含 CONTRACT_SPEC_LITERALS 任一字面（B-4 写作侧对齐：
 * integrator.md findings 约定要求 Basis 以该前缀集起引；未匹配形态放行，不把自由 prose 写作变假红）→
 * 视为「契约类 hard-violation 在案」。在案者 MUST 二选一核销：
 *   a) `docs/debt/contract/`（含 `_archive/<年>/`）任一条目文件含本 changeId 字面（对应 CDQ 条目；
 *      261019 批起 CDQ 单文件队列并入 debt 账本 contract 域）；
 *   b) pod-events 有 human-decision 事件 payload.decision ∈ {defer-contract-change, accept-risk}
 *      且 changeId 匹配（B-6 锚格式，复用五要素事件形状）。
 * 返回 { present, accounted, hits:[findingLine 摘录] }。readText/listFiles 可注入，self-check 可测。
 */
// 字面集（260913-arch-review-spec-split 扩展：spec.md 拆分为 requirements.md + architecture.md 并归档）——
// 与 integrator.md Basis 前缀写作约定同批联动改（只改一边 = 闸与约定脱钩）；
// docs/spec.md 保留在集内：归档件，历史 findings 的 Basis 仍用它起引。
export const CONTRACT_SPEC_LITERALS = ['docs/contracts/**', 'docs/spec.md', 'docs/requirements.md', 'docs/architecture.md'];

export function contractFindingsAccounted(dir, changeId, deps = {}) {
  const readText = deps.readText || ((p) => (existsSync(p) ? readFileSync(p, 'utf8') : null));
  const readPacketText = () => {
    let t = readText(join(dir, 'delivery', 'taskpackages', changeId, 'review-packet.md'));
    if (t === null) t = readText(join(dir, 'delivery', 'taskpackages', changeId, 'review-packet.md'));
    if (t === null && deps.podsRecordPath) {
      try {
        const wt = JSON.parse(readText(deps.podsRecordPath) || 'null')?.worktreePath;
        if (wt) t = readText(join(wt, 'delivery', 'taskpackages', changeId, 'review-packet.md')) ?? readText(join(wt, 'delivery', 'taskpackages', changeId, 'review-packet.md'));
      } catch { /* worktree 回退失败按不在场处理 */ }
    }
    return t;
  };
  const packetText = readPacketText();
  if (packetText === null) return { present: false, accounted: true, hits: [] };
  // 261026 批 T4（ARQ-261011-contract-gate-literal-selfquote-false-positive）：判据从「行文本巧合」改锚
  // 「finding 的结构位」（integrator.md:40 六列表）——只认 Findings 表行 + Kind 列（第 3 单元格，去 * 后）
  // 前缀为 hard-violation + 行内仍含字面集（保留「契约相关」收缩）。历史行 Kind 写作 `**hard-violation**（原判）`
  // （261016-p3-story-body-sd-cache:71 实测），故必须前缀匹配而非等值；自述「无 hard-violation」散文行（非表行）自动免疫。
  const hits = packetText
    .split('\n')
    .filter((l) => {
      const t = l.trim();
      if (!t.startsWith('|')) return false;
      const kind = String(t.split('|').map((s) => s.trim())[2] || '').replace(/\*/g, '').toLowerCase();
      return kind.startsWith('hard-violation') && CONTRACT_SPEC_LITERALS.some((lit) => t.includes(lit));
    });
  if (hits.length === 0) return { present: false, accounted: true, hits: [] };
  // CDQ 核销读点（261019-gov-review-docs-root-ledger-rehome T3）：单文件读 → 目录扫描。
  // 递归列举 docs/debt/contract/（含 _archive/<年>/——已结条目被渲染器归位后仍须可查，否则已结批的
  // 后续复核包会被误拦），排除 INDEX.md（生成件；凭据本体 = 条目文件，INDEX 内容派生自条目、不独立作证）。
  // 目录不在场 → 空集（cdqHasRow=false → BLOCK，向严）。判定 = 任一条目文件文本含 changeId 字面。
  const listFiles = deps.listFiles || ((root) => {
    const out = [];
    const walk = (d) => {
      for (const ent of readdirSync(d, { withFileTypes: true })) {
        if (ent.isDirectory()) { walk(join(d, ent.name)); continue; }
        if (ent.name.endsWith('.md') && ent.name !== 'INDEX.md') out.push(join(d, ent.name));
      }
    };
    walk(root);
    return out;
  });
  const cdqDir = join(dir, 'docs', 'debt', 'contract');
  let cdqFiles = [];
  try { cdqFiles = listFiles(cdqDir); } catch { cdqFiles = []; } // 目录不在场 → 空集（向严）
  const cdqHasRow = cdqFiles.some((f) => (readText(f) || '').includes(changeId));
  const deferAnchored = (() => {
    const raw =
      readText(join(dir, 'delivery', 'taskpackages', changeId, 'pod-events.jsonl')) ??
      (deps.podsRecordPath
        ? (() => {
            try {
              const wt = JSON.parse(readText(deps.podsRecordPath) || 'null')?.worktreePath;
              return wt ? readText(join(wt, 'delivery', 'taskpackages', changeId, 'pod-events.jsonl')) : null;
            } catch { return null; }
          })()
        : null);
    if (raw === null) return false;
    return raw.split('\n').some((line) => {
      const t = line.trim();
      if (!t) return false;
      try {
        const rec = JSON.parse(t);
        return (
          rec?.eventType === 'human-decision' &&
          rec?.actor === 'human' &&
          rec?.changeId === changeId &&
          (rec?.payload?.decision === 'defer-contract-change' || rec?.payload?.decision === 'accept-risk')
        );
      } catch { return false; }
    });
  })();
  return { present: true, accounted: cdqHasRow || deferAnchored, hits };
}

/**
 * T1（260908-gov-review-escalation-conflict-ladder）：并行相交面探测（只读，判据换轴的机械件）。
 * intersectingFiles = 双方相对 merge-base 的改动文件交集；conflictedFiles = merge-tree 实撞集；
 * addAddFiles = 双侧新增相撞子集。分级路由（pod-coordination.md §10d）：
 *   零相交 → tier 'no-overlap'（① 放行）；实撞全部为已登记版本化制品形态 → 'union-safe'（② 不升人，
 *   走 runbook 并集 + 双方齿复跑）；其余实撞 / 探测失败 → 'tradeoff'（③ architect 消解评审，闸 BLOCK）。
 * 版本化制品形态兜底正则与 merge-driver VERSIONED_ARTIFACT_RE 同形——lib 收编留根（本批不改 lib，双源留根注记）。
 * execFn 可注入（self-check）；探测任何异常 → { probed:false, tier:'tradeoff' }（fail-closed，绝不误放行）。
 */
export const VERSIONED_ARTIFACT_RE_LOCAL = /\.v[0-9]+\.md$/; // 与 merge-driver 同形，lib 收编留根

// 261026 批 T5（ARQ-261014 ③）：派生态 carve-out——与 merge-driver.mjs:943-946 的 PROJECTION_CONFLICT_FILES
// 同形两份（lib 收编留根，对齐上方 VERSIONED_ARTIFACT_RE_LOCAL 先例注记）；input-brief.md 刻意不入
// （driver 不可解，入闸会造「闸放行、driver blocked」错位——PROPOSAL §6 留根 3）。
export const DERIVED_STATE_CARVE_OUT = new Set([
  'delivery/taskpackages/current/taskpackage.json',
  'delivery/taskpackages/current/verdict.json',
]);

// 261026 批 T3（ARQ-261017-merge-gate-scope-unstated，OQ1 human 裁候选 2）：账本类制品允许集——
// 从当前账本拓扑正推（.claude/rules/repo-structure.md「docs/debt/」行四域 + intake），
// 拓扑扩域时随 repo-structure.md 同批更新本常量，不按历史 lane 假设硬编码。
export const LEDGER_ARTIFACT_PREFIXES = ['delivery/intake/', 'docs/debt/gov/', 'docs/debt/tech/', 'docs/debt/requirement/', 'docs/debt/contract/'];

/**
 * T1（261015-gov-review-changes-taxonomy 齿二）：废止路径结构检查——对合并结果树跑 ls-tree，
 * 废止路径非空即残件被旧布局分支 merge 带入（齿一 scope-map denyCreate 只拦 agent 会话写入，
 * 管不到 git merge 通道）。execFn 可注入（self-check）；探测异常 fail-closed（交上层 BLOCK，
 * 绝不因探测失败误放行）。波段二（同批 T6 第 6 条）已扩为 ['delivery/changes']——伞解散后
 * delivery/archive/changes/ 是不同精确路径、ls-tree 不误伤。
 */
export const RETIRED_MERGE_PATHS = ['delivery/changes'];

export function retiredPathProbe(execFn, source, target) {
  try {
    // 复用 parallelOverlapProbe 的 merge-tree 调用面（--write-tree）：exit 1 = 有冲突，stdout 首行仍是结果树 oid
    let mt;
    try {
      mt = execFn(`git merge-tree --write-tree ${source} ${target}`);
    } catch (err) {
      mt = `${err.stdout || ''}`;
    }
    const tree = mt.split('\n')[0].trim();
    if (!tree || !/^[0-9a-f]{40,}$/.test(tree)) return { probed: false, error: 'merge-tree 无结果树 oid', paths: [] };
    const paths = [];
    for (const p of RETIRED_MERGE_PATHS) {
      const ls = execFn(`git ls-tree ${tree} -- ${p}`).trim();
      if (ls) paths.push(...ls.split('\n').map((s) => s.trim()).filter(Boolean));
    }
    return { probed: true, tree, paths };
  } catch (err) {
    return { probed: false, error: `${err.message || err}`.slice(0, 200), paths: [] };
  }
}

export function parallelOverlapProbe(execFn, source, target) {
  try {
    const base = execFn(`git merge-base ${target} ${source}`).trim();
    const srcFiles = execFn(`git diff --name-only ${base} ${source}`).split('\n').map((s) => s.trim()).filter(Boolean);
    const tgtFiles = execFn(`git diff --name-only ${base} ${target}`).split('\n').map((s) => s.trim()).filter(Boolean);
    const srcSet = new Set(srcFiles);
    const intersectingFiles = tgtFiles.filter((f) => srcSet.has(f));
    let conflictedFiles = [];
    try {
      const mt = execFn(`git merge-tree --write-tree --name-only ${source} ${target}`);
      conflictedFiles = mt.split('\n').slice(1).map((s) => s.trim()).filter(Boolean); // 首行 = tree oid
    } catch (err) {
      const out = `${err.stdout || ''}`;
      conflictedFiles = out.split('\n').slice(1).map((s) => s.trim()).filter(Boolean); // exit 1 = 有冲突，stdout 仍带清单
    }
    // 噪声滤清（V3 实测：--name-only 输出混 informational 行）——实撞文件必 ⊆ 双方改动交集
    conflictedFiles = [...new Set(conflictedFiles)].filter((f) => intersectingFiles.includes(f));
    // add/add 判定：conflicted ∩ 双侧新增（merge-base 中不存在）
    const addAddFiles = conflictedFiles.filter((f) => {
      try { execFn(`git cat-file -e ${base}:${f}`); return false; } catch { return true; }
    });
    const tier = (() => {
      if (!intersectingFiles.length) return 'no-overlap';
      // 核查结论（260918-gov-review-merge-chain-friction T1②，FROZEN 2026-09-19）：`merge=union` 文件
      // （如 `delivery/taskpackages/**/pod-events.jsonl`，`.gitattributes` 由 c16283b 落地）由 git 自动并、
      // **不出现在 conflictedFiles**，天然落下面本行判 union-safe（② 级不升人）——**无需为 union 文件加显式豁免**。
      // 依据 = 本行分支逻辑 + ARQ-260917 记录的 k3 修复轮 r15 合并实测「tier3 评审噪音收口」。
      // 再下一行 `VERSIONED_ARTIFACT_RE_LOCAL` 是另一条并列路径（版本化制品 carve-out），非 union 情形所依赖。
      if (!conflictedFiles.length) return 'union-safe'; // 相交但可自动并（不同 hunk git 自并）
      if (conflictedFiles.every((f) => VERSIONED_ARTIFACT_RE_LOCAL.test(f) || DERIVED_STATE_CARVE_OUT.has(f))) return 'union-safe'; // S1 carve-out：已登记形态（版本化制品 / 派生态投影，261026 批 T5 扩）→ ②
      return 'tradeoff';
    })();
    return { probed: true, tier, intersectingFiles, conflictedFiles, addAddFiles };
  } catch (err) {
    return { probed: false, tier: 'tradeoff', error: `${err.message || err}`.slice(0, 200), intersectingFiles: [], conflictedFiles: [], addAddFiles: [] };
  }
}

/**
 * 主判：给定可注入上下文，产 { blocked, reasons[] }。
 * ctx = { execFn, changeId, source, kind: 'devloop'|'gov'|'unknown',
 *         supervisorVerdict, govAnchors, contractsDiff: boolean }
 */
export function evaluate(ctx) {
  const reasons = [];
  // 261026 批 T3（ARQ-261017-merge-gate-scope-unstated）：notApplicable 必须先声明（ESM 严格模式下裸赋值抛
  // ReferenceError，见 §7 R2-F4）——unknown 档 B 段只赋值、return 只引用。
  let notApplicable = false;
  const push = (s) => reasons.push(s);

  // dev-loop 档人批豁免（T1，260904-gov-review-escalate-merge-approval；OQ2=(a)）：
  // verdict=ESCALATE + provenance 完整 + human-approval 事件核验 ok → 豁免 riskTier / 非 APPROVE / blocked 三条理由
  //（blocked 在此只是 ESCALATE 停等人类的驻留标记，人批事件即其 resolve）。永不豁免：provenance 缺失、off-main、契约面、verdict=BLOCK。
  const approval = ctx.humanApproval ?? null;
  const escalateExempt =
    ctx.kind === 'devloop' &&
    approval?.ok === true &&
    ctx.supervisorVerdict?.verdict === 'ESCALATE' &&
    !!ctx.supervisorVerdict?.provenance?.mechanicalSupervisorGate?.ran &&
    !!ctx.supervisorVerdict?.provenance?.mechanicalSupervisorGate?.producedBy;

  // 通用 2：off-main
  if (ctx.source) {
    const off = sourceReachableFromMain(ctx.execFn, ctx.source);
    if (off.ok === true && off.reason === 'source-on-main') {
      push('off-main：源 commit 已含在 main 内，无独立分支可合（隔离被破坏）');
    } else if (off.ok === null) {
      push('off-main：无 main ref 无法判（fail-closed）');
    } else if (off.reason === 'source-absent') {
      push('off-main：源 ref 不存在（未 commit），无合并可言');
    }
  }

  // 通用 1：contracts diff 无契约合并凭据（T15，261015 批：三件联立凭据替代人签门）
  if (ctx.contractsDiff && !ctx.hasContractGateEvidence.ok) {
    push(`docs/contracts/** diff 且同批无契约合并凭据（${ctx.hasContractGateEvidence.reason}）——契约变更 = 架构变更的一种，走 /arch-change loop（.claude/rules/contracts.md；凭据 = FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖，三件联立）`);
  }

  // 通用 5（T1，261015-gov-review-changes-taxonomy 齿二）：废止路径结构检查——合并结果树含残件即 BLOCK
  if (ctx.retiredProbe) {
    if (!ctx.retiredProbe.probed) {
      push(`retired-path-probe-fail-closed：合并结果树废止路径探测失败（${ctx.retiredProbe.error || 'unknown'}）——升级人工核，不得自动 merge`);
    } else if (ctx.retiredProbe.paths.length) {
      push(`retired-path-merge-block：合并结果树含废止路径残件 ${ctx.retiredProbe.paths.length} 条（${ctx.retiredProbe.paths.slice(0, 5).join(', ')}${ctx.retiredProbe.paths.length > 5 ? ' 等' : ''}）——旧布局分支带入；归位 delivery/taskpackages/ 同 id 目录后重跑（dev lane 已由 261014 批撤销，真家映射见 delivery/archive/README.md）`);
    }
  }

  // 通用 3：escalation 未 resolve / blocked（blocked 仅在人批豁免成立时视同 resolve，OQ2=(a)）
  if (ctx.blocked && !escalateExempt) push('change 处 blocked 态（未 resolve）');
  if (ctx.escalationUnresolved) push('存在未 resolve 的 agent-escalation 事件');

  // 通用 4（T5，260908 批门内加项）：契约类 hard-violation「交回→在册」核验。
  // 只拦可识别形态（A-8 放行方向）：Findings 行含 hard-violation + CONTRACT_SPEC_LITERALS 任一字面起引，
  // 且 CDQ 无对应行、pod-events 无 human defer/accept-risk 锚 → BLOCK；未匹配 / packet 不在场 → 放行。
  if (ctx.contractFindings?.present && !ctx.contractFindings.accounted) {
    push(`contract-findings-unaccounted：review-packet 有 ${ctx.contractFindings.hits.length} 条 hard-violation 且 Basis 引 docs/contracts/**|docs/spec.md|docs/requirements.md|docs/architecture.md，但 docs/debt/contract/ 无对应条目且无 human defer/accept-risk 锚（260908 批 T5；字面集 260913 批扩展；CDQ 家 261019 批迁 docs/debt/contract/）——核销 = 在 docs/debt/contract/ 立条目（骨架 docs/templates/debt-item.template.md）`);
  }

  // 人批 flag fail-closed：给了 flag 但核验不过 → BLOCK 且点名失败项
  if (approval && !approval.ok) push(`human-approval 核验失败：${approval.reason}`);

  if (ctx.kind === 'devloop') {
    // T1/T2（260908 批）：并行相交面分级路由——tradeoff（③ 级）BLOCK 交 architect 消解评审；
    // no-overlap / union-safe 不升人（② 级走 runbook 并集 + 双方齿复跑；探测失败已 fail-closed 归 tradeoff）。
    if (ctx.overlap?.tier === 'tradeoff') {
      const detail = ctx.overlap.probed
        ? `实撞 ${ctx.overlap.conflictedFiles.length} 件（${ctx.overlap.conflictedFiles.slice(0, 5).join(', ')}${ctx.overlap.conflictedFiles.length > 5 ? ' 等' : ''}）`
        : `探测失败（${ctx.overlap.error || 'unknown'}）`;
      push(`parallel-overlap-tier3：并行 Pod 相交有取舍，需 architect 消解评审（pod-coordination.md §10d）——${detail}`);
    }
    // dev-loop 档 4：riskTier（人批豁免成立时放行）
    const riskTier = ctx.supervisorVerdict?.riskTier;
    if ((riskTier === 'high' || riskTier === 'critical') && !escalateExempt) {
      push(`riskTier=${riskTier}（supervisor 升级）`);
    }
    // dev-loop 档 5：provenance（缺失永不豁免）
    const prov = ctx.supervisorVerdict?.provenance?.mechanicalSupervisorGate;
    if (!prov?.ran || !prov?.producedBy) {
      push('supervisor APPROVE provenance 不可机验（supervisor-verdict 缺失或 mechanicalSupervisorGate 不全）');
    } else if (ctx.supervisorVerdict?.verdict !== 'APPROVE' && !escalateExempt) {
      push(`supervisor verdict=${ctx.supervisorVerdict?.verdict}（非 APPROVE）`);
    }
  } else if (ctx.kind === 'gov') {
    // Track B 档 6：architect GREEN 机锚
    const a = ctx.govAnchors;
    if (!a) push('Track B：PROPOSAL 不在（无法验 architect GREEN 机锚）');
    else if (!a.frozen) push('Track B：PROPOSAL Status 未标 FROZEN');
    else if (!a.green) push('Track B：§7 无 architect GREEN verdict');
    else if (!a.landingVerdict) push('Track B：landing-verdict 不在产');
  } else {
    // 261026 批 T3（ARQ-261017-merge-gate-scope-unstated，OQ1 裁候选 2）：变更集全部落在账本类制品 →
    // 本闸不适用（NOT_APPLICABLE），交 merge-to-main skill 的 human 授权通道；否则仍 BLOCK。
    // 边界刻意保守：允许集外任何路径（固件 / shim / .claude/** / docs/contracts/** / delivery/workstreams/**
    // / delivery/retro/** / delivery/archive/** / docs/debt/README.md）都使 onlyLedger 不成立 → 仍 BLOCK（V4b retro-only 实测）。
    const diffPaths = ctx.diffPaths || [];
    const onlyLedger = diffPaths.length > 0 && diffPaths.every((p) => LEDGER_ARTIFACT_PREFIXES.some((pre) => p.startsWith(pre)));
    if (onlyLedger) notApplicable = true;
    else push(`unknown change kind（changeId 既无 delivery/taskpackages 也无 delivery/review）`);
  }

  return { blocked: reasons.length > 0, reasons, notApplicable };
}

/** 解 pods 记录路径：主仓优先，linked worktree 回退 git common dir。不在场返回 null。 */
function findPodsRecordPath(projectDir, changeId) {
  const mainPath = join(projectDir, '.git', 'devloop', 'pods', `${changeId}.json`);
  if (existsSync(mainPath)) return mainPath;
  try {
    const cd = execSync('git rev-parse --git-common-dir', { cwd: projectDir, encoding: 'utf8', stdio: 'pipe' }).trim();
    const commonPath = join(resolve(projectDir, cd), 'devloop', 'pods', `${changeId}.json`);
    if (existsSync(commonPath)) return commonPath;
  } catch { /* fallthrough */ }
  return null;
}

/** 从磁盘组 ctx（live path）。 */
function buildCtx(projectDir, changeId, sourceRef, humanApprovalRef = null) {
  const execFn = (c) => execSync(c, { encoding: 'utf8', stdio: 'pipe' });
  const devDir = join(projectDir, 'delivery', 'taskpackages', changeId);
  const archDir = join(projectDir, 'delivery', 'review', changeId); // 261015 批 T6：gov 批新家 delivery/review/（kind 探测随新树）
  const hasDev = existsSync(devDir);
  const hasArch = existsSync(archDir);
  const kind = hasArch ? 'gov' : hasDev ? 'devloop' : 'unknown';

  const source = sourceRef || revParse(execFn, 'HEAD');

  const supervisorVerdict = readSupervisorVerdict(projectDir, changeId);
  const govAnchors = kind === 'gov' ? readGovAnchors(projectDir, changeId) : null;
  const contractsDiff = hasContractsDiff(execFn, source, 'main');
  const diffPaths = (() => { try { return execFn(`git diff --name-only ${source}..main`).split('\n').map((l) => l.trim()).filter(Boolean); } catch { return []; } })(); // 261026 批 T3：unknown 档 applicability 判据（evaluate 只消费 ctx.diffPaths，缺此组装则 NOT_APPLICABLE 永不触发——§7 R1-F1）
  // T15（261015 批）：契约合并机械凭据——替换原 hasContractGateEvidence: false 硬编码（ARQ-260923 建议①兑现）
  const hasContractGateEvidence = hasArchLoopContractEvidence(execFn, source, 'main', projectDir);
  const podsRecordPath = findPodsRecordPath(projectDir, changeId);
  const blocked = (() => {
    if (!podsRecordPath) return false;
    try {
      return JSON.parse(readFileSync(podsRecordPath, 'utf8')).status === 'blocked';
    } catch {
      return false;
    }
  })();
  const humanApproval = humanApprovalRef
    ? resolveHumanApproval(projectDir, changeId, humanApprovalRef, { podsRecordPath })
    : null;
  const contractFindings = contractFindingsAccounted(projectDir, changeId, { podsRecordPath }); // T5（260908 批）
  // T1（260908 批）：并行相交面探测（dev-loop 档；source = 源 ref，target = main）
  const overlap = parallelOverlapProbe(execFn, source, 'main');
  // T1（261015-gov-review-changes-taxonomy 齿二）：废止路径结构检查（两档通用；target = main）
  const retiredProbe = retiredPathProbe(execFn, source, 'main');

  return {
    changeId,
    source,
    kind,
    execFn, // 热修（ARQ-260905-escalation-gate-live-execfn-gap，human 批 2026-09-05）：evaluate 的 off-main 判定依赖它，此前 buildCtx 漏带 → live 恒 fail-closed
    supervisorVerdict,
    govAnchors,
    contractsDiff,
    diffPaths, // 261026 批 T3：变更集路径（unknown 档 applicability 判据）
    hasContractGateEvidence, // T15（261015 批）：{ ok, reason, contractPaths }——ok=false 且 contractsDiff → 通用 1 BLOCK；原硬编码 false（恒 BLOCK 人合）已退役
    blocked,
    escalationUnresolved: false, // pod-events 聚合待接入；现 false（升级闸 v1 只覆盖 blocked 态 + 显式判定）
    humanApproval, // T1：--human-approval-ref 核验结果；未带 flag 为 null
    contractFindings, // T5（260908 批）：契约类 hard-violation 台账核验结果
    overlap, // T1（260908 批）：并行相交面探测结果（分级路由见 pod-coordination §10d）
    retiredProbe, // T1（261015-gov-review-changes-taxonomy 齿二）：合并结果树废止路径探测结果
  };
}

// ---- self-check（真空绿规约：断言输入非空 + 逐 case 预期）----
function selfCheck() {
  const cases = [];
  const mkCtx = (over) => ({
    execFn: (c) => {
      if (c.includes('--is-ancestor')) throw new Error('not ancestor');
      return 'x';
    },
    changeId: 'x',
    source: 'feature',
    kind: 'devloop',
    supervisorVerdict: {
      verdict: 'APPROVE',
      riskTier: 'low',
      provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'supervisor-verdict-gate.mjs' } },
    },
    govAnchors: null,
    contractsDiff: false,
    hasContractGateEvidence: { ok: true, reason: 'no-contract-diff', contractPaths: [] },
    blocked: false,
    escalationUnresolved: false,
    humanApproval: null, // T1：--human-approval-ref 核验结果
    contractFindings: { present: false, accounted: true, hits: [] }, // T5：默认不在场 → 放行
    overlap: { probed: true, tier: 'no-overlap', intersectingFiles: [], conflictedFiles: [], addAddFiles: [] }, // T1：默认零相交 → 放行
    retiredProbe: { probed: true, tree: 'x', paths: [] }, // 齿二（261015 批）：默认结果树干净 → 放行
    ...over,
  });

  const expect = (name, over, wantBlocked, wantReason) => {
    const r = evaluate(mkCtx(over));
    const ok = r.blocked === wantBlocked && r.reasons.some((x) => x.includes(wantReason));
    cases.push({ name, ok, got: JSON.stringify(r.reasons) });
  };

  // 放行：clean devloop 全绿
  cases.push({ name: 'devloop-clean-pass', ok: !evaluate(mkCtx({})).blocked, got: '' });
  // 通用 2 off-main：source-on-main
  expect('off-main-block', { execFn: (c) => (c.includes('--is-ancestor') ? '' : 'x'), source: 'feature' }, true, 'off-main');
  // 通用 1 contracts diff（T15 改写：凭据对象形态）
  expect('contracts-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: 无候选满足三件联立' } }, true, 'docs/contracts');
  // T15 契约合并凭据 5 例（261015 批；R1-F4：三件联立肢各有否定覆盖；真空绿规约）
  expect('contracts-diff-no-evidence-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: 无候选 PROPOSAL' } }, true, 'contract-evidence');
  cases.push({
    name: 'contracts-diff-evidence-pass',
    ok: !evaluate(mkCtx({ contractsDiff: true, hasContractGateEvidence: { ok: true, reason: 'contract-evidence via 261016-arch-review-t' } })).blocked,
    got: '',
  });
  expect('contracts-diff-proposal-not-frozen-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: FROZEN 缺' } }, true, 'contract-evidence');
  expect('contracts-diff-manifest-not-covering-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: manifest 未覆盖' } }, true, 'contract-evidence');
  expect('contracts-diff-verdict-not-pass-block', { contractsDiff: true, hasContractGateEvidence: { ok: false, reason: 'contract-evidence: verdict 非 PASS' } }, true, 'contract-evidence');
  cases.push({
    name: 'no-contracts-diff-unaffected',
    ok: !evaluate(mkCtx({ contractsDiff: false, hasContractGateEvidence: { ok: true, reason: 'no-contract-diff', contractPaths: [] } })).blocked,
    got: '',
  });
  {
    // T15 直接函数 fixture 4 例（真空绿规约：PROPOSAL fixture 含 manifest 围栏块非空；deps.readVerdict 注入）
    const FIX_PROPOSAL = [
      '# PROPOSAL · t',
      '> **状态：FROZEN v1.0 · 2026-10-15 · test**',
      '```landing-manifest',
      'docs/contracts/interface-contract.md',
      'docs/architecture.md',
      '```',
      '',
    ].join('\n');
    const FIX_DIFF = ['docs/contracts/interface-contract.md', 'delivery/review/261016-arch-review-t/PROPOSAL.md'].join('\n');
    const fxExec = (c) => (c.startsWith('git diff') ? FIX_DIFF : c.startsWith('git show') ? FIX_PROPOSAL : '');
    const r1 = hasArchLoopContractEvidence(fxExec, 'src', 'main', 'x', { readVerdict: () => 'verdict: PASS' });
    cases.push({ name: 'contract-evidence-fn-pass', ok: r1.ok === true, got: r1.reason });
    const r2 = hasArchLoopContractEvidence(fxExec, 'src', 'main', 'x', { readVerdict: () => 'verdict: BLOCK(F1)' });
    cases.push({ name: 'contract-evidence-fn-verdict-not-pass-block', ok: r2.ok === false && /verdict|联立/.test(r2.reason), got: r2.reason });
    const r3 = hasArchLoopContractEvidence((c) => (c.startsWith('git diff') ? FIX_DIFF : c.startsWith('git show') ? 'not frozen' : ''), 'src', 'main', 'x', { readVerdict: () => 'verdict: PASS' });
    cases.push({ name: 'contract-evidence-fn-not-frozen-block', ok: r3.ok === false && /FROZEN|联立/.test(r3.reason), got: r3.reason });
    const r4 = hasArchLoopContractEvidence(
      (c) => (c.startsWith('git diff') ? FIX_DIFF : c.startsWith('git show') ? FIX_PROPOSAL.replace('docs/contracts/interface-contract.md\n', '') : ''),
      'src', 'main', 'x',
      { readVerdict: () => 'verdict: PASS' },
    );
    cases.push({ name: 'contract-evidence-fn-manifest-not-covering-block', ok: r4.ok === false && /manifest|联立/.test(r4.reason), got: r4.reason });
  }
  // 通用 3 blocked
  expect('blocked-block', { blocked: true }, true, 'blocked');
  // dev-loop 档 4 riskTier high
  expect('riskTier-high-block', { supervisorVerdict: { verdict: 'APPROVE', riskTier: 'high', provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'x' } } } }, true, 'riskTier');
  // dev-loop 档 5 provenance 缺
  expect('provenance-block', { supervisorVerdict: { verdict: 'APPROVE', riskTier: 'low', provenance: null } }, true, 'provenance');
  // dev-loop 非 APPROVE
  expect('verdict-not-approve-block', { supervisorVerdict: { verdict: 'BLOCK', riskTier: 'low', provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'x' } } } }, true, '非 APPROVE');
  // gov 档 6：GREEN 机锚齐全 → 放行
  cases.push({
    name: 'gov-clean-pass',
    ok: !evaluate(mkCtx({ kind: 'gov', supervisorVerdict: null, govAnchors: { frozen: true, green: true, landingVerdict: true } })).blocked,
    got: '',
  });
  // gov 档 6：FROZEN 缺
  expect('gov-not-frozen-block', { kind: 'gov', supervisorVerdict: null, govAnchors: { frozen: false, green: true, landingVerdict: true } }, true, 'FROZEN');
  // unknown kind
  expect('unknown-kind-block', { kind: 'unknown', supervisorVerdict: null }, true, 'unknown');

  // T1 人批豁免 8 例（260904-gov-review-escalate-merge-approval；真空绿规约：输入非空、逐 case 有预期）
  const escVerdict = {
    verdict: 'ESCALATE',
    riskTier: 'high',
    provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'supervisor-verdict-gate.mjs' } },
  };
  // 1) ESCALATE 无 flag → 行为零变更（riskTier 理由仍在）
  expect('escalate-no-approval-still-block', { supervisorVerdict: escVerdict }, true, 'riskTier');
  // 2) ESCALATE + 人批核验 ok → 放行（含 blocked 联动豁免，OQ2=(a)）
  cases.push({
    name: 'escalate-with-approval-pass',
    ok: !evaluate(mkCtx({ supervisorVerdict: escVerdict, humanApproval: { ok: true, reason: 'human-approval verified' }, blocked: true })).blocked,
    got: '',
  });
  // 3-6) flag 给了但五要素核验失败 → fail-closed 点名失败项
  expect('approval-wrong-eventid-block', { supervisorVerdict: escVerdict, humanApproval: { ok: false, reason: 'eventId=x 不在 pod-events 中' } }, true, 'human-approval 核验失败');
  expect('approval-actor-not-human-block', { supervisorVerdict: escVerdict, humanApproval: { ok: false, reason: "actor=agent（应为 human）" } }, true, 'actor=');
  expect('approval-wrong-decision-block', { supervisorVerdict: escVerdict, humanApproval: { ok: false, reason: 'payload.decision=reject（应为 approve-escalated-merge）' } }, true, 'payload.decision=');
  expect('approval-wrong-changeid-block', { supervisorVerdict: escVerdict, humanApproval: { ok: false, reason: 'changeId=other（应为本 changeId x）' } }, true, 'changeId=');
  // 7) verdict=BLOCK 不可人批豁免（证据缺失不是风险接受能抹平的形态）
  expect('block-verdict-not-waived', { supervisorVerdict: { verdict: 'BLOCK', riskTier: 'high', provenance: { mechanicalSupervisorGate: { ran: true, producedBy: 'x' } } }, humanApproval: { ok: true, reason: 'human-approval verified' } }, true, '非 APPROVE');
  // 8) provenance 缺失永不豁免
  expect('escalate-provenance-missing-not-waived', { supervisorVerdict: { verdict: 'ESCALATE', riskTier: 'high', provenance: null }, humanApproval: { ok: true, reason: 'human-approval verified' } }, true, 'provenance');

  // T5 契约台账核验 3 例（260908 批；真空绿规约：fixture 非空断言先行——present 值由 contractFindingsAccounted 真实扫描产出的形状构造）
  // 1) 在案 + 无 CDQ 行无 defer 锚 → BLOCK 点名
  expect('contract-findings-unaccounted-block', { contractFindings: { present: true, accounted: false, hits: ['| SHOULD | hard-violation | contract | ... | docs/contracts/** §三 speech | 交回 CDQ |'] } }, true, 'contract-findings-unaccounted');
  // 2) 在案 + CDQ 行在场（accounted:true）→ 放行
  cases.push({ name: 'contract-findings-cdq-accounted-pass', ok: !evaluate(mkCtx({ contractFindings: { present: true, accounted: true, hits: ['docs/contracts/**'] } })).blocked, got: '' });
  // 3) 在案 + human defer 锚（accounted:true）→ 放行
  cases.push({ name: 'contract-findings-defer-anchored-pass', ok: !evaluate(mkCtx({ contractFindings: { present: true, accounted: true, hits: ['docs/spec.md'] } })).blocked, got: '' });

  // T5 字面集扩展 4 例（260913-arch-review-spec-split：spec.md 拆分 + 归档；真实扫 contractFindingsAccounted，注入 listFiles + readText 目录 fixture——261019 批起 CDQ 读点 = docs/debt/contract/ 目录扫描）
  const mkDirDeps = (packetLine, entries) => ({
    // entries: { 相对路径: 文本 }——键含 '_archive' 者模拟已结条目归位后场景
    listFiles: () => Object.keys(entries),
    readText: (p) => {
      if (p.includes('review-packet.md')) return `| SHOULD | hard-violation | contract | x | ${packetLine} | 交回 CDQ |\n`;
      const rel = Object.keys(entries).find((k) => p.endsWith(k) || p === k);
      if (rel) return entries[rel];
      return null; // pod-events 不在场 → 无 defer 锚
    },
  });
  {
    // 1) 新路径 docs/requirements.md → 在案，contract/ 无条目含 changeId → unaccounted
    const r = contractFindingsAccounted('x', 'chg-1', mkDirDeps('docs/requirements.md §3', { 'CDQ-260901-a.md': '# 条目\n' }));
    cases.push({ name: 'literal-requirements-detected', ok: r.present === true && r.accounted === false, got: JSON.stringify(r) });
    // 2) 新路径 docs/architecture.md + contract/ 有条目含本 changeId → accounted
    const r2 = contractFindingsAccounted('x', 'chg-1', mkDirDeps('docs/architecture.md §5', { 'CDQ-260901-a.md': '# 条目\nchg-1 挂账行\n' }));
    cases.push({ name: 'literal-architecture-cdq-accounted', ok: r2.present === true && r2.accounted === true, got: JSON.stringify(r2) });
    // 3) 归档件 docs/spec.md 字面保留（历史 findings 兼容）→ 在案
    const r3 = contractFindingsAccounted('x', 'chg-1', mkDirDeps('docs/spec.md §4', { 'CDQ-260901-a.md': '# 条目\n' }));
    cases.push({ name: 'literal-archived-spec-still-detected', ok: r3.present === true && r3.accounted === false, got: JSON.stringify(r3) });
    // 4) 非字面集路径（docs/notes/...）→ 不在案，放行（A-8 放行方向不回退）
    const r4 = contractFindingsAccounted('x', 'chg-1', mkDirDeps('docs/notes/foo.md', { 'CDQ-260901-a.md': '# 条目\n' }));
    cases.push({ name: 'literal-noncontract-path-not-present', ok: r4.present === false && r4.accounted === true, got: JSON.stringify(r4) });
    // 5) changeId 只在 _archive/2026/ 的已结条目里 → accounted（归位后可查 parity）
    const r5 = contractFindingsAccounted('x', 'chg-1', mkDirDeps('docs/architecture.md §5', { '_archive/2026/CDQ-260901-a.md': '# 已结条目\nchg-1 挂账行\n' }));
    cases.push({ name: 'cdq-archive-still-accounted', ok: r5.present === true && r5.accounted === true, got: JSON.stringify(r5) });
    // 6) 目录在场但无任何条目含 changeId → unaccounted（BLOCK 点名）
    const r6 = contractFindingsAccounted('x', 'chg-1', mkDirDeps('docs/architecture.md §5', { 'CDQ-260901-a.md': '# 条目\nchg-2 的账\n', 'CDQ-260902-b.md': '# 另一条\n' }));
    cases.push({ name: 'cdq-no-matching-entry-block', ok: r6.present === true && r6.accounted === false, got: JSON.stringify(r6) });
  }

  // 齿二（261015-gov-review-changes-taxonomy）两臂 + execFn 注入两态（真空绿规约：夹具 ls-tree 输出先断非空/空）
  // 1) 残件分支 BLOCK：结果树 ls-tree 命中废止路径
  expect('retired-path-merge-block', { retiredProbe: { probed: true, tree: 'x', paths: ['100644 blob a1\tdelivery/changes/dev/261015-x/review-packet.md'] } }, true, 'retired-path-merge-block');
  // 2) 探测失败 → fail-closed BLOCK
  expect('retired-path-probe-fail-closed-block', { retiredProbe: { probed: false, error: 'merge-tree 无结果树 oid', paths: [] } }, true, 'retired-path-probe-fail-closed');
  // 3) 干净分支 PASS：结果树 ls-tree 空（显式复证，默认 mkCtx 即此形态）
  cases.push({ name: 'retired-path-clean-pass', ok: !evaluate(mkCtx({ retiredProbe: { probed: true, tree: 'x', paths: [] } })).blocked, got: '' });
  // 5) 波段二（T6 第 6 条）旧伞分支 BLOCK 臂：伞级路径命中
  expect('retired-path-umbrella-block', { retiredProbe: { probed: true, tree: 'x', paths: ['040000 tree b2\tdelivery/changes'] } }, true, 'retired-path-merge-block');
  // 4) 函数级 execFn 注入两态：merge-tree 返回树 oid，ls-tree 空 = 干净 / 非空 = 残件
  {
    const TREE = 'a'.repeat(40);
    const fxExec = (lsOut) => (c) => {
      if (c.startsWith('git merge-tree')) return `${TREE}\n`;
      if (c.startsWith('git ls-tree')) return lsOut;
      throw new Error(`unexpected: ${c}`);
    };
    const cleanLs = '';
    const dirtyLs = '100644 blob a1b2c3d\tdelivery/changes/dev/261015-voice-rightcol-label-overflow/review-packet.md';
    const rc = retiredPathProbe(fxExec(cleanLs), 'src', 'main');
    cases.push({ name: 'retired-probe-fn-clean', ok: rc.probed === true && rc.tree === TREE && rc.paths.length === 0, got: JSON.stringify(rc) });
    const rd = retiredPathProbe(fxExec(dirtyLs), 'src', 'main');
    cases.push({ name: 'retired-probe-fn-dirty', ok: rd.probed === true && rd.paths.length === 1 && rd.paths[0].includes('delivery/changes/dev/'), got: JSON.stringify(rd) });
  }

  // T1 并行相交面分级路由 4 例（260908 批 ladder；human 追认 Track B + 继续落地后落地；真空绿规约：fixture 非空——文件清单构造非空）
  // 1) ② 级：实撞全部为已登记版本化制品形态 → union-safe，不升人（runbook 并集 + 双方齿复跑）
  cases.push({
    name: 'overlap-union-safe-pass',
    ok: !evaluate(mkCtx({ overlap: { probed: true, tier: 'union-safe', intersectingFiles: ['server/deploy/openclaw-system-prompt.v3.md', 'server/deploy/provider.config.yaml'], conflictedFiles: ['server/deploy/openclaw-system-prompt.v3.md'], addAddFiles: ['server/deploy/openclaw-system-prompt.v3.md'] } })).blocked,
    got: '',
  });
  // 2) ③ 级：同 hunk 相撞（非登记形态）→ BLOCK 点名 architect 消解评审
  expect('overlap-tradeoff-block', { overlap: { probed: true, tier: 'tradeoff', intersectingFiles: ['firmware/main/display/ui.c'], conflictedFiles: ['firmware/main/display/ui.c'], addAddFiles: [] } }, true, 'parallel-overlap-tier3');
  // 3) 探测失败 → fail-closed 归 ③ BLOCK
  expect('overlap-probe-fail-block', { overlap: { probed: false, tier: 'tradeoff', error: 'merge-base failed' } }, true, 'parallel-overlap-tier3');
  // 4) ① 级：零相交 → 放行（默认 mkCtx 即此形态，显式复证）
  cases.push({
    name: 'overlap-no-overlap-pass',
    ok: !evaluate(mkCtx({ overlap: { probed: true, tier: 'no-overlap', intersectingFiles: [], conflictedFiles: [], addAddFiles: [] } })).blocked,
    got: '',
  });
  // 5) V4 升级件内容宿主断言（260908 批 ladder 转绿补件）：probe-out JSON 形状必载相交清单/实撞集/双新增/tier 四键——
  // 升级件宿主 = overlap-probe.json（③④ 级的逐文件清单消费源），断言其序列化形状防回退
  {
    const v4Overlap = { probed: true, tier: 'tradeoff', intersectingFiles: ['a.c', 'b.c'], conflictedFiles: ['b.c'], addAddFiles: [] };
    const v4Json = JSON.stringify({ changeId: 'x', kind: 'devloop', overlap: v4Overlap });
    const v4KeysOk = ['"intersectingFiles"', '"conflictedFiles"', '"addAddFiles"', '"tier"'].every((k) => v4Json.includes(k)) && v4Json.includes('"b.c"');
    cases.push({ name: 'overlap-probeout-payload-keys-ok', ok: v4KeysOk, got: v4Json.slice(0, 80) });
  }

  // T2（260920-gov-review-verdict-anchor-align）：green 锚三夹具——
  // 样板行（GREEN 在行中、无 verdict 标签）不命中 / 规范 verdict 行命中 / BLOCK 行不命中。
  // 真空绿：样板行文本先断含 GREEN 字样（防夹具没造对）。
  {
    const boilerplate = '> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**';
    cases.push({ name: 'green-anchor-boilerplate-not-green', ok: !isGreenVerdict(boilerplate), got: `含 GREEN=${/GREEN/.test(boilerplate)} → isGreen=${isGreenVerdict(boilerplate)}` });
    cases.push({ name: 'green-anchor-verdict-line-green', ok: isGreenVerdict('verdict: GREEN'), got: `isGreen=${isGreenVerdict('verdict: GREEN')}` });
    cases.push({ name: 'green-anchor-block-not-green', ok: !isGreenVerdict('verdict: BLOCK(F1)'), got: `isGreen=${isGreenVerdict('verdict: BLOCK(F1)')}` });
  }

  // 261026 批 T1（ARQ-261017-merge-gate-contract-evidence-ref）：取件 source 优先——2 条判别夹具
  {
    const T1_PROPOSAL = [
      '# PROPOSAL · t1',
      '> **状态：FROZEN v1.0 · 2026-10-26 · test**',
      '```landing-manifest',
      'docs/contracts/interface-contract.md',
      '```',
      '',
    ].join('\n');
    const T1_DIFF = ['docs/contracts/interface-contract.md', 'delivery/review/261026-arch-review-t1/PROPOSAL.md'].join('\n');
    // source-only 可读（target=main 读不到——首合并形态）→ 三件齐备应 ok:true
    const rSrcOnly = hasArchLoopContractEvidence(
      (c) => { if (c.startsWith('git diff')) return T1_DIFF; if (c.startsWith('git show src:')) return T1_PROPOSAL; throw new Error('not on main'); },
      'src', 'main', 'x', { readVerdict: () => 'verdict: PASS' },
    );
    cases.push({ name: 'contract-evidence-source-first-pass', ok: rSrcOnly.ok === true, got: rSrcOnly.reason });
    // 两 ref 均不可读 → ok:false 且 reason 点名「两 ref 均不可读」（归因自检，非「缺凭据」误导）
    const rUnreadable = hasArchLoopContractEvidence(
      (c) => { if (c.startsWith('git diff')) return T1_DIFF; throw new Error('missing'); },
      'src', 'main', 'x', { readVerdict: () => 'verdict: PASS' },
    );
    cases.push({ name: 'contract-evidence-ref-unreadable-attributed', ok: rUnreadable.ok === false && rUnreadable.reason.includes('两 ref 均不可读'), got: rUnreadable.reason });
  }

  // 261026 批 T2（ARQ-261021）：verdict 锚取终态——2 条轨迹夹具
  {
    const T2_PROPOSAL = [
      '# PROPOSAL · t2',
      '> **状态：FROZEN v1.0 · 2026-10-26 · test**',
      '```landing-manifest',
      'docs/contracts/interface-contract.md',
      '```',
      '',
    ].join('\n');
    const T2_DIFF = ['docs/contracts/interface-contract.md', 'delivery/review/261026-arch-review-t2/PROPOSAL.md'].join('\n');
    const t2Exec = (c) => (c.startsWith('git diff') ? T2_DIFF : T2_PROPOSAL);
    const rUp = hasArchLoopContractEvidence(t2Exec, 'src', 'main', 'x', { readVerdict: () => 'verdict: FAIL\nverdict: PASS' });
    cases.push({ name: 'contract-evidence-verdict-fail-then-pass', ok: rUp.ok === true, got: rUp.reason });
    const rDown = hasArchLoopContractEvidence(t2Exec, 'src', 'main', 'x', { readVerdict: () => 'verdict: PASS\nverdict: FAIL' });
    cases.push({ name: 'contract-evidence-verdict-pass-then-fail-block', ok: rDown.ok === false && /联立|verdict/.test(rDown.reason), got: rDown.reason });
  }

  // 261026 批 T3（OQ1 裁候选 2）：ledger 允许集——6 条判别夹具（四域 + intake 正向、混入固件与 retro 边界反向）
  {
    const unknownCtx = (diffPaths) => mkCtx({ kind: 'unknown', diffPaths });
    const isNa = (r) => !r.blocked && r.notApplicable === true;
    cases.push({ name: 'ledger-intake-only-not-applicable', ok: isNa(evaluate(unknownCtx(['delivery/intake/261026-cross-face-x.md']))), got: '' });
    cases.push({ name: 'ledger-requirement-only-not-applicable', ok: isNa(evaluate(unknownCtx(['docs/debt/requirement/REQ-261026-x.md']))), got: '' });
    cases.push({ name: 'ledger-contract-only-not-applicable', ok: isNa(evaluate(unknownCtx(['docs/debt/contract/CDQ-261026-x.md']))), got: '' });
    cases.push({
      name: 'ledger-four-domains-plus-intake-not-applicable',
      ok: isNa(evaluate(unknownCtx(['delivery/intake/a.md', 'docs/debt/gov/b.md', 'docs/debt/tech/c.md', 'docs/debt/requirement/d.md', 'docs/debt/contract/e.md']))),
      got: '',
    });
    const rMixed = evaluate(unknownCtx(['docs/debt/gov/a.md', 'firmware/main.c']));
    cases.push({ name: 'ledger-with-firmware-still-blocked', ok: rMixed.blocked && rMixed.notApplicable === false && rMixed.reasons.some((x) => x.includes('unknown change kind')), got: JSON.stringify(rMixed.reasons) });
    const rRetro = evaluate(unknownCtx(['delivery/retro/2026-10-25-loop-retro.md']));
    cases.push({ name: 'ledger-retro-only-still-blocked', ok: rRetro.blocked && !rRetro.notApplicable, got: JSON.stringify(rRetro.reasons) });
  }

  // 261026 批 T4（ARQ-261011）：契约 findings 列位判据——3 条判别夹具
  {
    const t4Dep = (packetText) => ({ readText: (p) => (String(p).includes('review-packet.md') ? packetText : null), listFiles: () => [] });
    const T4_PACKET = [
      '| Severity | Kind | Area | Finding | Basis | Required action |',
      '| --- | --- | --- | --- | --- | --- |',
      '| BLOCKING | hard-violation | firmware | 违反笔顺 JSON 契约 | `docs/contracts/**` 起引 | 修 |',
      '无 hard-violation finding——如出现将以 `docs/contracts/**` 字面前缀起引 Basis。',
      '| SHOULD | 核销说明 | contract | 上一轮 finding 已核销 | `docs/contracts/**` 起引 | 已处理 |',
    ].join('\n');
    const rT4 = contractFindingsAccounted('.', '261026-fx-t4', t4Dep(T4_PACKET));
    cases.push({ name: 'contract-findings-table-row-hit', ok: rT4.present === true && rT4.hits.length === 1, got: `hits=${rT4.hits.length}` });
    const rProse = contractFindingsAccounted('.', '261026-fx-t4b', t4Dep('无 hard-violation finding——如出现将以 `docs/contracts/**` 字面前缀起引 Basis。'));
    cases.push({ name: 'contract-findings-prose-line-immune', ok: rProse.present === false && rProse.hits.length === 0, got: `hits=${rProse.hits.length}` });
    const rWriteoff = contractFindingsAccounted('.', '261026-fx-t4c', t4Dep('| SHOULD | 核销说明 | contract | 已核销 | `docs/contracts/**` 起引 | 已处理 |'));
    cases.push({ name: 'contract-findings-non-hv-kind-immune', ok: rWriteoff.present === false, got: `present=${rWriteoff.present}` });
  }

  // 261026 批 T5（ARQ-261014 ③）：派生态 carve-out——2 条函数级夹具（直接调 parallelOverlapProbe 注入
  // execFn，不手注 overlap.tier——手注夹具删 carve-out 后仍全绿，属无判别力假绿，起草期自曝在案）
  {
    const PROJ = 'delivery/taskpackages/current/taskpackage.json';
    const BOTH = [PROJ, 'firmware/main.c']; // 双侧同改两件——混撞件必须 ∈ 双侧改动交集，否则被噪声滤清剔除（夹具首轮实测抓出）
    const t5Exec = (conflicted) => (c) => {
      if (c.startsWith('git merge-base')) return 'base';
      if (c === 'git diff --name-only base feat' || c === 'git diff --name-only base main') return BOTH.join('\n');
      if (c.startsWith('git merge-tree --write-tree')) {
        if (conflicted.length) { const err = new Error('conflict'); err.stdout = ['treeoid', ...conflicted].join('\n'); throw err; }
        return 'treeoid';
      }
      if (c.startsWith('git cat-file -e')) throw new Error('absent'); // 双侧新增（merge-base 中不存在）
      return '';
    };
    const rProj = parallelOverlapProbe(t5Exec([PROJ]), 'feat', 'main');
    cases.push({ name: 'overlap-projection-derived-state-pass', ok: rProj.probed && rProj.tier === 'union-safe', got: JSON.stringify(rProj) });
    const rMix = parallelOverlapProbe(t5Exec([PROJ, 'firmware/main.c']), 'feat', 'main');
    cases.push({ name: 'overlap-projection-plus-other-tradeoff', ok: rMix.probed && rMix.tier === 'tradeoff', got: JSON.stringify(rMix) });
  }

  const fail = cases.filter((c) => !c.ok);
  process.stdout.write(
    `${JSON.stringify({ selfCheck: 'pre-merge-escalation-gate', pass: cases.length - fail.length, fail: fail.length, total: cases.length, ok: fail.length === 0, failures: fail.length ? fail : undefined })}\n`,
  );
  process.exit(fail.length ? 1 : 0);
}

// ---- 入口（T6，260908 批：import.meta 守卫——被 import 时不执行 CLI，可导入库 / 可执行脚本分离）----
function isDirectRun() {
  try {
    return !!process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href;
  } catch {
    return false;
  }
}

if (isDirectRun()) {
if (process.argv.includes('--self-check')) {
  selfCheck();
}

const args = process.argv.slice(2);
const changeId = args.find((a) => !a.startsWith('--'));
const sourceRef = (() => {
  const i = args.indexOf('--source-ref');
  return i >= 0 ? args[i + 1] : null;
})();
const projectDirArg = (() => {
  const i = args.indexOf('--project-dir');
  return i >= 0 ? args[i + 1] : null;
})();
const humanApprovalRef = (() => {
  const i = args.indexOf('--human-approval-ref');
  return i >= 0 ? args[i + 1] ?? null : null;
})();
const probeOutPath = (() => {
  const i = args.indexOf('--probe-out');
  return i >= 0 ? args[i + 1] ?? null : null;
})(); // T3（260908 批 ladder）：探测面 JSON 制品落盘路径（显式文件传递，非 stdout 解析——R1-S2）

if (humanApprovalRef !== null && (humanApprovalRef.startsWith('--') || humanApprovalRef === '')) {
  process.stderr.write('[pre-merge-escalation-gate] FATAL: --human-approval-ref 需要一个 <eventId> 参数\n');
  process.exit(2);
}

if (!changeId) {
  process.stderr.write('[pre-merge-escalation-gate] FATAL: missing <changeId>\n');
  process.exit(2);
}

const dir = projectDirArg ? resolve(projectDirArg) : projectDirOf();
const ctx = buildCtx(dir, changeId, sourceRef, humanApprovalRef);

// T1 防误用：人批豁免只对 dev-loop 档有意义（治理档有 FROZEN+GREEN+landing-verdict 机锚，无人批豁免概念）
if (humanApprovalRef && (ctx.kind === 'gov' || ctx.kind === 'unknown')) {
  process.stderr.write(`[pre-merge-escalation-gate] FATAL: --human-approval-ref 仅适用于 dev-loop 档（changeId=${changeId} kind=${ctx.kind}，无人批豁免概念）\n`);
  process.exit(2);
}

const result = evaluate(ctx);

// T3（260908 批 ladder）：探测面制品落盘——PASS 与 BLOCK 两态都落（升级件内容面消费），失败 advisory 不改退出码
if (probeOutPath) {
  try {
    writeFileSync(probeOutPath, JSON.stringify({ changeId, kind: ctx.kind, source: ctx.source, overlap: ctx.overlap }, null, 2), 'utf8');
  } catch (err) {
    process.stderr.write(`[pre-merge-escalation-gate] WARN: --probe-out 落盘失败（advisory）：${err.message}\n`);
  }
}

if (result.blocked) {
  process.stderr.write(`[pre-merge-escalation-gate] BLOCK (changeId=${changeId}, kind=${ctx.kind}):\n`);
  result.reasons.forEach((r) => process.stderr.write(`  - ${r}\n`));
  process.stderr.write('  → 升级 human，不得自动 merge。\n');
  process.exit(1);
}

// 261026 批 T3（OQ1 裁候选 2）：纯账本制品变更集 → 闸不适用（exit 0——不适用，不是放行），
// 交 merge-to-main skill 的 human 授权通道；通用检查（契约 diff 凭据 / off-main 等）已在 evaluate 内先行，
// 到达此分支 = 无例外命中。
if (result.notApplicable) {
  process.stdout.write(`[pre-merge-escalation-gate] NOT_APPLICABLE (changeId=${changeId}, kind=${ctx.kind}): 变更集仅含 ledger 类制品（delivery/intake/、docs/debt/gov/、docs/debt/tech/、docs/debt/requirement/、docs/debt/contract/）——本闸只管 dev/gov 两条 lane，此类合并按 merge-to-main skill 走 human 授权通道。\n`);
  process.exit(0);
}

const approvalNote = humanApprovalRef ? ` — human-approval verified (eventId=${humanApprovalRef})` : '';
process.stdout.write(`[pre-merge-escalation-gate] PASS (changeId=${changeId}, kind=${ctx.kind})${approvalNote} — 无例外命中，可交给 merge-driver。\n`);
process.exit(0);
} // end isDirectRun()