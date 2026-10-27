#!/usr/bin/env node
// Portfolio 协调 reconcile 引擎（多 Pod 并行 → 串行合并）：
//   聚合 devloop/pods/*.json → 重算 devloop/board.json + 轮转 merge token。
//   运行时协调态放共享 git common dir（$(git rev-parse --git-common-dir)/devloop/），
//   跨 linked worktree 共享、不入版本控制。机制见 .claude/references/pod-coordination.md。
//
// 设计：无外部依赖、幂等（每次从全量 pods/*.json 重算）、可独立运行。
//   - board/token 单一逻辑写者 = 本脚本，用 mkdir advisory lock 串行化，temp+rename 原子写。
//   - Pod 只写自己的 pods/<changeId>.json（--set-pod），本脚本从不改 pods/*.json（只读聚合）。
//   - reconcile 回收崩溃 holder 时只在 board 视图里标 abandoned，不回写 pod 文件。
//
// 用法：
//   node .claude/scripts/portfolio-reconcile.mjs                         # 只 reconcile（聚合 + 轮转 token）
//   node .claude/scripts/portfolio-reconcile.mjs --set-pod <id> --status ready-to-merge [--merge-order 3]
//                                                                        # 写本 Pod 状态后再 reconcile
//
// 环境变量：
//   DEVLOOP_TOKEN_TTL_SEC   merge token 崩溃回收 TTL（默认 1800，须 > 正常合并门时长）
//   DEVLOOP_LOCK_STALE_SEC  reconcile.lock stale 阈值（默认 60）

import {
  existsSync,
  mkdirSync,
  readFileSync,
  readdirSync,
  renameSync,
  rmdirSync,
  statSync,
  writeFileSync,
  unlinkSync,
} from 'node:fs';
import { resolve, join } from 'node:path';
import { execFileSync } from 'node:child_process';
import { isProposalMdPath, normalizeSourceRef, resolveOriginRef } from './lib/governance-gate-shared.mjs';

const POD_STATUSES = new Set([
  'queued',
  'running',
  'verifier-green',
  'ready-to-merge',
  'merging',
  'merged',
  'blocked',
  'abandoned',
]);
const TERMINAL_OR_RELEASED = new Set(['merged', 'blocked', 'abandoned']);
// 261026 批 T4（ARQ-261025-wait-state-closed-set）：等人操作停等的受控枚举族 awaiting-human[:<reason>]。
// 只扩调度态投影面；放行权不在 status 档位，在各自门的人批凭据核验（见 pod-coordination §3）。
const AWAITING_HUMAN_RE = /^awaiting-human(:[a-z0-9][a-z0-9-]*)?$/;
export function isAwaitingHumanStatus(status) {
  return typeof status === 'string' && AWAITING_HUMAN_RE.test(status);
}
export function isValidPodStatus(status) {
  return POD_STATUSES.has(status) || isAwaitingHumanStatus(status);
}

const TOKEN_TTL_SEC = Number(process.env.DEVLOOP_TOKEN_TTL_SEC || 1800);
const LOCK_STALE_SEC = Number(process.env.DEVLOOP_LOCK_STALE_SEC || 60);

// 解析共享 git common dir 下的 devloop 目录。主 worktree 返回相对 .git，
// linked worktree 返回绝对路径，故一律 resolve(projectDir, ...)。
function resolveDevloopDir(projectDir) {
  let commonDir;
  try {
    commonDir = execFileSync('git', ['rev-parse', '--git-common-dir'], {
      cwd: projectDir,
      encoding: 'utf8',
    }).trim();
  } catch {
    return null; // 非 git 仓库：优雅退出
  }
  const abs = resolve(projectDir, commonDir);
  const devloop = join(abs, 'devloop');
  mkdirSync(join(devloop, 'pods'), { recursive: true });
  return devloop;
}

function lockPath(devloopDir) {
  return join(devloopDir, 'reconcile.lock');
}

// mkdir advisory lock；EEXIST 时按 mtime 判 stale：陈旧则强制回收再抢，否则放弃。
function acquireLock(devloopDir) {
  const lp = lockPath(devloopDir);
  try {
    mkdirSync(lp);
    return true;
  } catch (err) {
    if (err && err.code === 'EEXIST') {
      try {
        const ageSec = (Date.now() - statSync(lp).mtimeMs) / 1000;
        if (ageSec > LOCK_STALE_SEC) {
          rmdirSync(lp); // 回收陈旧锁（持锁进程崩溃残留）
          mkdirSync(lp);
          return true;
        }
      } catch {
        /* 竞争中被他人回收/重建：放弃本次，幂等等下次 */
      }
      return false;
    }
    throw err;
  }
}

function releaseLock(devloopDir) {
  try {
    rmdirSync(lockPath(devloopDir));
  } catch {
    /* 已被回收：忽略 */
  }
}

function readJson(path) {
  try {
    return JSON.parse(readFileSync(path, 'utf8'));
  } catch {
    return null;
  }
}

function readPods(devloopDir) {
  const podsDir = join(devloopDir, 'pods');
  if (!existsSync(podsDir)) return [];
  const pods = [];
  for (const name of readdirSync(podsDir)) {
    if (!name.endsWith('.json')) continue;
    const data = readJson(join(podsDir, name));
    if (!data || !data.changeId) continue;
    pods.push({
      changeId: String(data.changeId),
      status: isValidPodStatus(data.status) ? data.status : 'queued',
      mergeOrder: Number.isFinite(data.mergeOrder) ? data.mergeOrder : Number.MAX_SAFE_INTEGER,
      updatedAt: data.updatedAt || null,
    });
  }
  return pods;
}

// 原子写：temp + rename。
function writeAtomic(path, text) {
  const tmp = `${path}.tmp-${process.pid}-${Date.now()}`;
  writeFileSync(tmp, text);
  renameSync(tmp, path);
}

// 写本 Pod 状态（单写者 = 调用方所在 Pod 会话）；不碰 board。
export function setPod(projectDir, { changeId, status, mergeOrder, worktreePath } = {}) {
  if (!changeId) throw new Error('setPod: changeId required');
  if (!isValidPodStatus(status)) throw new Error(`setPod: invalid status "${status}"`);
  const devloopDir = resolveDevloopDir(projectDir);
  if (!devloopDir) return null;
  const podPath = join(devloopDir, 'pods', `${changeId}.json`);
  const prev = readJson(podPath) || {};
  const rec = {
    changeId: String(changeId),
    status,
    mergeOrder: Number.isFinite(mergeOrder)
      ? mergeOrder
      : Number.isFinite(prev.mergeOrder)
        ? prev.mergeOrder
        : null,
    worktreePath: worktreePath || prev.worktreePath || null,
    updatedAt: new Date().toISOString(),
  };
  writeAtomic(podPath, `${JSON.stringify(rec, null, 2)}\n`);
  return rec;
}

// merge token 状态机（见 pod-coordination.md §8）。返回 { token, pods(视图), alerts }。
function computeToken(pods, prevBoard) {
  const alerts = [];
  const byId = new Map(pods.map((p) => [p.changeId, p]));
  const view = pods.map((p) => ({ ...p }));
  const viewById = new Map(view.map((p) => [p.changeId, p]));
  let token =
    prevBoard && prevBoard.mergeToken
      ? { holder: prevBoard.mergeToken.holder ?? null, grantedAt: prevBoard.mergeToken.grantedAt ?? null }
      : { holder: null, grantedAt: null };

  // 1) 校验/回收当前 holder
  if (token.holder) {
    const h = byId.get(token.holder);
    if (!h || TERMINAL_OR_RELEASED.has(h.status) || isAwaitingHumanStatus(h.status)) {
      token = { holder: null, grantedAt: null }; // merged/blocked/escalate(→blocked) 释放
    } else if (
      token.grantedAt &&
      (Date.now() - Date.parse(token.grantedAt)) / 1000 > TOKEN_TTL_SEC
    ) {
      // holder 崩溃/abandon：超 TTL 未合并 → 回收（board 视图标 abandoned，不回写 pod 文件）
      const vh = viewById.get(token.holder);
      if (vh) vh.status = 'abandoned';
      alerts.push({
        kind: 'merge-token-reclaimed',
        changeId: token.holder,
        reason: `holder held > TTL ${TOKEN_TTL_SEC}s without merge`,
      });
      token = { holder: null, grantedAt: null };
    }
  }

  // 2) token 空闲 → 从 ready-to-merge 里按 merge order 选下一个（原位次，防饿死）
  if (!token.holder) {
    const ready = view
      .filter((p) => p.status === 'ready-to-merge')
      .sort(
        (a, b) => a.mergeOrder - b.mergeOrder || String(a.changeId).localeCompare(String(b.changeId)),
      );
    if (ready.length) {
      token = { holder: ready[0].changeId, grantedAt: new Date().toISOString() };
    }
  }

  return { token, pods: view, alerts };
}

function countByStatus(pods) {
  const wip = {};
  for (const p of pods) wip[p.status] = (wip[p.status] || 0) + 1;
  return wip;
}

// 幂等全量 reconcile：聚合 pods → 重算 board + 轮转 token。lock 抢不到就幂等退出。
export function reconcile(projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd()) {
  const devloopDir = resolveDevloopDir(projectDir);
  if (!devloopDir) return { ok: false, reason: 'not-a-git-repo' };
  if (!acquireLock(devloopDir)) return { ok: true, skipped: 'locked' };
  try {
    const pods = readPods(devloopDir);
    const prevBoard = readJson(join(devloopDir, 'board.json'));
    const { token, pods: view, alerts } = computeToken(pods, prevBoard);
    const board = {
      generatedAt: new Date().toISOString(),
      podCount: view.length,
      wip: countByStatus(view),
      mergeToken: token,
      pods: view.sort((a, b) => a.mergeOrder - b.mergeOrder),
      alerts,
    };
    writeAtomic(join(devloopDir, 'board.json'), `${JSON.stringify(board, null, 2)}\n`);
    return { ok: true, board, devloopDir };
  } finally {
    releaseLock(devloopDir);
  }
}

// ---- launch 可达性机械守卫（§2b check 集，260825-gov-review-launch-reachability-guard T1）----
// §2b check 集机械化：对基准 ref 逐件 `git cat-file -e`。blocking 缺 → 拒绝；warn 缺 → 点名不拒。
// 注入缝（R1-F3 修订）：exists / show 第三/四参可选，注入测试桩；缺省走 git 直派——自检传桩测纯逻辑，
// 落地行为走缺省分支，二者同函数体、无第二份逻辑。
// 判据键纪律（F-05 / 批 2 B6/B8）：origin 读取走 lib 单源，不内联自产键。
// ── launchPreconditions 读侧（260921-gov-review-launch-pipeline T6，承载 ARQ-260914）──────────
// 形态：[{ text, scope, status, evidence? }]；scope ∈ "workstream" | "package:<changeId>"；
//       status ∈ "unverified" | "verified"。旧形态裸 string **向后兼容** = { scope:'workstream', status:'unverified' }。
// 语义：① 按包过滤 —— 只取 scope === 'workstream' ∨ scope === `package:<本包>`，其余折叠为一行计数（不逐条列）；
//       ② 只报 status !== 'verified' 的条目；③ 旧形态（裸 string）不逐条报、只汇总计数（防永久噪音，且不静默丢失）。
// **warn 通道，非 blocking 源**——本函数不改任何门的判定语义、不新增工程门。
export function summarizeLaunchPreconditions(raw, changeId) {
  if (!Array.isArray(raw)) return null;
  const pending = [];
  let legacyCount = 0, otherPackagesCount = 0, verifiedCount = 0;
  for (const item of raw) {
    const legacy = typeof item === 'string';
    const text = legacy ? item : (item && typeof item.text === 'string' ? item.text : '');
    const scope = legacy ? 'workstream' : ((item && item.scope) || 'workstream');
    const status = legacy ? 'unverified' : ((item && item.status) || 'unverified');
    if (scope !== 'workstream' && scope !== `package:${changeId}`) { otherPackagesCount += 1; continue; }
    if (status === 'verified') { verifiedCount += 1; continue; }
    if (legacy) legacyCount += 1; else pending.push({ text, scope });
  }
  return { pending, legacyCount, otherPackagesCount, verifiedCount, total: raw.length };
}

export function checkLaunchReachability(projectDir, { changeId, baseRef, exists: injectExists, show: injectShow }) {
  if (!changeId || !baseRef) throw new Error('checkLaunchReachability: changeId & baseRef required');
  const exists = injectExists || ((path) => { try { execFileSync('git', ['cat-file', '-e', `${baseRef}:${path}`], { cwd: projectDir, stdio: 'ignore' }); return true; } catch { return false; } });
  const show = injectShow || ((path) => execFileSync('git', ['show', `${baseRef}:${path}`], { cwd: projectDir, encoding: 'utf8' }));
  const blocking = [], warn = [], present = [];
  let launchPreconditions = null;

  // ① taskpackage.json —— blocking
  const tpPath = `delivery/taskpackages/${changeId}/taskpackage.json`;
  if (!exists(tpPath)) blocking.push(tpPath);
  else {
    present.push(tpPath);
    // ② PROPOSAL —— blocking（origin.upstreamRef 解析；剥片段；双路解析复用 isProposalMdPath）
    let taskPackage = null;
    try { taskPackage = JSON.parse(show(tpPath)); } catch { taskPackage = null; }
    const origin = (taskPackage && taskPackage.origin && typeof taskPackage.origin === 'object') ? taskPackage.origin : null;
    const ref = normalizeSourceRef(resolveOriginRef(origin)); // fragment-stripped + 单源
    if (ref && isProposalMdPath(ref)) {
      if (exists(ref)) present.push(ref); else blocking.push(`${ref} (PROPOSAL)`);
    }
    // PROPOSAL 对非 proposal 型上游（无 upstreamRef / 非 PROPOSAL 路径）= 不适用，不查不拒。
    // ③ decision-coverage.json —— warn（仅当有 workstream）
    const ws = origin && origin.workstream;
    if (ws) {
      const dcPath = `delivery/workstreams/${ws}/decision-coverage.json`;
      if (exists(dcPath)) {
        present.push(dcPath);
        // ④ launchPreconditions —— warn（260921-gov-review-launch-pipeline T6；承载 ARQ-260914）
        //    读得到就按包过滤汇总；读不到（文件非 JSON / 无该键）则保持 null，不报红、不改 ok。
        let dc = null;
        try { dc = JSON.parse(show(dcPath)); } catch { dc = null; }
        launchPreconditions = summarizeLaunchPreconditions(dc && dc.launchPreconditions, changeId);
      } else warn.push(dcPath);
    }
  }
  return { ok: blocking.length === 0, changeId, baseRef, blocking, warn, present, launchPreconditions };
}

// ---- 自检（ARQ-114 先例；注入缝测纯逻辑、不触 git；260825-gov-review-launch-reachability-guard T1(c)）----
export function selfCheck() {
  const cases = [];
  const t = (name, actual, expected) => cases.push({ name, pass: JSON.stringify(actual) === JSON.stringify(expected), actual, expected });
  const mkStubs = (files) => {
    const exists = (p) => Object.prototype.hasOwnProperty.call(files, p);
    const show = (p) => {
      if (!Object.prototype.hasOwnProperty.call(files, p)) throw new Error(`selfCheck stub: ${p} absent`);
      return files[p];
    };
    // 真空绿规约：先断言注入桩非空（注入缝失效则下方全真空绿）
    if (typeof exists !== 'function' || typeof show !== 'function') throw new Error('selfCheck: stub injection failed');
    return { exists, show };
  };
  const TP = 'delivery/taskpackages/cid-a/taskpackage.json';

  // ① proposal 型双路 delivery/review/**（旧树归档形态经 isProposalMdPath 兼容）三件齐 → ok:true
  let w = mkStubs({
    [TP]: JSON.stringify({ origin: { upstreamType: 'proposal', upstreamRef: 'delivery/review/x/PROPOSAL.md', workstream: 'ws-a' } }),
    'delivery/review/x/PROPOSAL.md': '# PROPOSAL',
    'delivery/workstreams/ws-a/decision-coverage.json': '{}',
  });
  let r = checkLaunchReachability('.', { changeId: 'cid-a', baseRef: 'REF', exists: w.exists, show: w.show });
  t('proposal-dualpath-all-present', { ok: r.ok, blocking: r.blocking, warn: r.warn, presentCount: r.present.length }, { ok: true, blocking: [], warn: [], presentCount: 3 });

  // ② hybrid 带片段，剥除后命中 → ok:true
  w = mkStubs({
    [TP]: JSON.stringify({ origin: { upstreamType: 'regression', upstreamRef: 'delivery/workstreams/ws-b/PROPOSAL.md#§4.1', workstream: 'ws-b' } }),
    'delivery/workstreams/ws-b/PROPOSAL.md': '# PROPOSAL',
    'delivery/workstreams/ws-b/decision-coverage.json': '{}',
  });
  r = checkLaunchReachability('.', { changeId: 'cid-a', baseRef: 'REF', exists: w.exists, show: w.show });
  t('hybrid-fragment-stripped-hit', { ok: r.ok, blocking: r.blocking }, { ok: true, blocking: [] });

  // ③ 非 proposal 上游 → PROPOSAL 不适用，不查不拒 → ok:true
  w = mkStubs({ [TP]: JSON.stringify({ origin: { upstreamType: 'regression', upstreamRef: 'issue-123' } }) });
  r = checkLaunchReachability('.', { changeId: 'cid-a', baseRef: 'REF', exists: w.exists, show: w.show });
  t('non-proposal-upstream-proposal-na', { ok: r.ok, blocking: r.blocking }, { ok: true, blocking: [] });

  // ④ proposal 型但 PROPOSAL 缺 → ok:false + 点名
  w = mkStubs({ [TP]: JSON.stringify({ origin: { upstreamType: 'proposal', upstreamRef: 'delivery/review/y/PROPOSAL.md' } }) });
  r = checkLaunchReachability('.', { changeId: 'cid-a', baseRef: 'REF', exists: w.exists, show: w.show });
  t('proposal-missing-blocking-named', { ok: r.ok, hasBlocking: r.blocking.length > 0, named: r.blocking.some((b) => b.includes('delivery/review/y/PROPOSAL.md')) }, { ok: false, hasBlocking: true, named: true });

  // ⑤ launchPreconditions 新形态（对象）：按包过滤 + 只报未核（260921-gov-review-launch-pipeline T6）
  //    可分辨性：不按 scope 过滤 → A4 会混入 pending；不按 status 过滤 → A3 会混入 pending。
  w = mkStubs({
    [TP]: JSON.stringify({ origin: { upstreamType: 'regression', upstreamRef: 'issue-1', workstream: 'ws-c' } }),
    'delivery/workstreams/ws-c/decision-coverage.json': JSON.stringify({ launchPreconditions: [
      { text: 'A1', scope: 'workstream', status: 'unverified' },
      { text: 'A2', scope: 'package:cid-a', status: 'unverified' },
      { text: 'A3', scope: 'package:cid-a', status: 'verified' },
      { text: 'A4', scope: 'package:other-pkg', status: 'unverified' },
    ] }),
  });
  r = checkLaunchReachability('.', { changeId: 'cid-a', baseRef: 'REF', exists: w.exists, show: w.show });
  t('launchpreconds-object-filtered-by-scope-and-status', {
    pendingTexts: r.launchPreconditions.pending.map((p) => p.text),
    legacyCount: r.launchPreconditions.legacyCount,
    otherPackagesCount: r.launchPreconditions.otherPackagesCount,
    verifiedCount: r.launchPreconditions.verifiedCount,
  }, { pendingTexts: ['A1', 'A2'], legacyCount: 0, otherPackagesCount: 1, verifiedCount: 1 });

  // ⑥ launchPreconditions 旧形态（裸 string）：走一行汇总、不逐条进 pending（260921-gov-review-launch-pipeline T6）
  //    可分辨性：若把旧形态当新形态直接报，pending 会是 3 条而非 0。
  w = mkStubs({
    [TP]: JSON.stringify({ origin: { upstreamType: 'regression', upstreamRef: 'issue-1', workstream: 'ws-d' } }),
    'delivery/workstreams/ws-d/decision-coverage.json': JSON.stringify({ launchPreconditions: ['L1', 'L2', 'L3'] }),
  });
  r = checkLaunchReachability('.', { changeId: 'cid-a', baseRef: 'REF', exists: w.exists, show: w.show });
  t('launchpreconds-legacy-summarized-not-listed', {
    pendingLen: r.launchPreconditions.pending.length,
    legacyCount: r.launchPreconditions.legacyCount,
  }, { pendingLen: 0, legacyCount: 3 });

  // ⑦ 261026 批 T4：等人操作停等族（判别力：去掉 family 判据时后两条"非法"变红；放宽成任意冒号串时同样变红）
  t('pod-status: awaiting-human 裸值合法', isValidPodStatus('awaiting-human'), true);
  t('pod-status: awaiting-human:<reason> 合法', isValidPodStatus('awaiting-human:real-device'), true);
  t('pod-status: reason 大写/空格非法', isValidPodStatus('awaiting-human:Real Device'), false);
  t('pod-status: 空 reason 非法', isValidPodStatus('awaiting-human:'), false);
  t('pod-status: 既有八值仍合法', isValidPodStatus('ready-to-merge'), true);
  t('pod-status: 拼写错误仍非法', isValidPodStatus('blockedd'), false);

  const pass = cases.filter((c) => c.pass).length;
  const okAll = pass === cases.length;
  return { selfCheck: 'portfolio-reconcile', ok: okAll, pass, fail: cases.length - pass, total: cases.length, cases: okAll ? undefined : cases.filter((c) => !c.pass) };
}

// ---- CLI ----
function parseArgs(argv) {
  const args = {};
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (a === '--check-launch') args.checkLaunch = argv[++i];
    else if (a === '--base-ref') args.baseRef = argv[++i];
    else if (a === '--self-check') args.selfCheckFlag = true;
    else if (a === '--set-pod') args.setPod = argv[++i];
    else if (a === '--status') args.status = argv[++i];
    else if (a === '--merge-order') args.mergeOrder = Number(argv[++i]);
    else if (a === '--worktree-path') args.worktreePath = argv[++i];
  }
  return args;
}

function isMain() {
  const invoked = process.argv[1] ? resolve(process.argv[1]) : '';
  const self = resolve(new URL(import.meta.url).pathname);
  // Windows: URL pathname 带前导斜杠，resolve 归一后比较结尾
  return invoked && (self === invoked || self.endsWith(invoked) || invoked.endsWith('portfolio-reconcile.mjs'));
}

if (isMain()) {
  const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
  const args = parseArgs(process.argv.slice(2));
  if (args.selfCheckFlag) {
    const res = selfCheck();
    process.stdout.write(`${JSON.stringify(res, null, 2)}\n`);
    process.exit(res.ok ? 0 : 1);
  }
  if (args.checkLaunch) {
    // check-launch 模式：只读、不触发 reconcile、不写协调态（先于 --set-pod 分支）
    if (!args.baseRef) {
      process.stdout.write(`${JSON.stringify({ ok: false, error: '--check-launch requires --base-ref <ref>' }, null, 2)}\n`);
      process.exit(2);
    }
    const res = checkLaunchReachability(projectDir, { changeId: args.checkLaunch, baseRef: args.baseRef });
    process.stdout.write(`${JSON.stringify(res, null, 2)}\n`);
    // launchPreconditions 人读汇总（260921-gov-review-launch-pipeline T6；**warn 通道，不阻断**）
    const lp = res.launchPreconditions;
    if (lp) {
      for (const p of lp.pending) process.stdout.write(`[launch-precondition] 未核（${p.scope}）：${p.text}\n`);
      if (lp.otherPackagesCount > 0) process.stdout.write(`[launch-precondition] 另 ${lp.otherPackagesCount} 条属其他包（已折叠，不逐条列）\n`);
      if (lp.legacyCount > 0) process.stdout.write(`[launch-precondition] 本 workstream 尚有 ${lp.legacyCount} 条旧形态（裸 string）前置未迁移，按 workstream 级呈现\n`);
      if (lp.pending.length === 0 && lp.legacyCount === 0) process.stdout.write(`[launch-precondition] 本包相关前置无待核项（已核 ${lp.verifiedCount} 条）\n`);
    }
    if (!res.ok) {
      process.stdout.write(`[check-launch] 拆包产物未 commit 到基准 ref：${res.blocking.join('、')}——回 portfolio-planner 补 commit（pod-coordination §2b）\n`);
      process.exit(1);
    }
    process.exit(0);
  }
  if (args.setPod) {
    const rec = setPod(projectDir, {
      changeId: args.setPod,
      status: args.status,
      mergeOrder: args.mergeOrder,
      worktreePath: args.worktreePath,
    });
    process.stdout.write(`[portfolio] pod set: ${JSON.stringify(rec)}\n`);
  }
  const res = reconcile(projectDir);
  if (res && res.board) {
    const t = res.board.mergeToken;
    process.stdout.write(
      `[portfolio] reconciled: ${res.board.podCount} pods, mergeToken.holder=${t.holder || 'none'}` +
        (res.board.alerts.length ? `, alerts=${res.board.alerts.length}` : '') +
        '\n',
    );
  } else {
    process.stdout.write(`[portfolio] ${JSON.stringify(res)}\n`);
  }
}
