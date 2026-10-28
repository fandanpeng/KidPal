#!/usr/bin/env node
// release-pr · 本地 main 的发布与回同步单一入口（261025-gov-review-release-pr-publish T1）。
//
// owner=architect, approver=human (G3 script, see DEV-GOVERNANCE.md §6)
//
// 职责边界（本脚本刻意不做的事）：
//   - 不合并 PR（PR 合并恒由 human 在 GitHub 网页点）；不推 main；不 force push；不删分支；不写仓库文件。
//   - push 只发生在 `--publish` 且预检全过之后；`--sync` 只做本地 `merge --ff-only origin/main`，不 push。
//
// 用法：
//   node .claude/scripts/release-pr.mjs --publish [--title "<标题>"] [--dry-run]
//   node .claude/scripts/release-pr.mjs --sync [--dry-run]
//   node .claude/scripts/release-pr.mjs --self-check
//
// 结构性承重点（PROPOSAL §2 T1 证据段）：发布分支 tip 恒等于推送时的本地 main tip。
// PR merge commit 的两个父是（合并时的 origin/main, release tip），因此本地 main 恒为该 merge commit
// 的祖先，human 合并后 `merge --ff-only origin/main` 永远成立。限定条件：发布后本地 main 不再前进；
// 若又前进，须再发一次（否则两侧仍可能互非祖先，merge-driver 会判 diverged）。
//
// 注意：本脚本内部的 push 在 `.claude/hooks/deny-main-push.mjs` 视野之外（钩子只看 Bash 命令串）。
// 防线 = 本脚本的 push argv 白名单 + scripts 作者写权限定 gov author-implementation 通道 + landing 人读 diff；发布防线不变。

import { execFileSync } from 'node:child_process';
import { mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import { tmpdir } from 'node:os';

const GIT_FETCH_TIMEOUT_MS = Number(process.env.RELEASE_PR_FETCH_TIMEOUT_MS || 60000);
const GH_TIMEOUT_MS = Number(process.env.RELEASE_PR_GH_TIMEOUT_MS || 60000);
const MAX_BRANCH_SUFFIX = 20;

// 常驻脏豁免：机器再生面，按路径计、不限状态码（M / D / ?? 一律放行）。
export const EXEMPT_DIRTY_PATHS = new Set([
  '.codebase-memory/artifact.json',
  '.codebase-memory/graph.db.zst',
]);

// push argv 白名单：任何 form 的 force / delete / mirror / all 都不允许。
const FORBIDDEN_PUSH_TOKENS = new Set(['-f', '--force', '--delete', '--mirror', '--all']);

// gh 子命令白名单：本脚本只会用到这三条。`pr merge` / `api` 等一律在调用前拒绝。
export const GH_ALLOWED_PREFIXES = [
  ['auth', 'status'],
  ['pr', 'list'],
  ['pr', 'create'],
];

// ---- 纯函数面（自检直调，不触网） ----

/**
 * 本地 main 与 origin/main 的关系。
 * equal = 无差异；ahead = 本地领先（origin/main 是 main 祖先）；behind = 本地落后；diverged = 互非祖先。
 */
export function resolveBranchRelation({ mainSha, originSha, isAncestor }) {
  if (!mainSha || !originSha) return 'unknown';
  if (mainSha === originSha) return 'equal';
  if (isAncestor(originSha, mainSha)) return 'ahead';
  if (isAncestor(mainSha, originSha)) return 'behind';
  return 'diverged';
}

/** `git status --porcelain -uall` 输出 → 仓根相对路径数组（rename 取新路径，剥引号，正斜杠归一）。 */
export function parsePorcelainPaths(statusOut) {
  const paths = [];
  for (const raw of String(statusOut || '').split(/\r?\n/)) {
    if (!raw.trim()) continue;
    let p = raw.slice(3); // 前两列状态码 + 一个空格
    if (p.includes(' -> ')) p = p.split(' -> ').pop();
    p = p.replace(/^"(.*)"$/, '$1').replace(/\\"/g, '"');
    paths.push(p.replace(/\\/g, '/'));
  }
  return paths;
}

/** 工作树清洁度：仅放行 EXEMPT_DIRTY_PATHS；返回 { ok, offenders }。 */
export function classifyDirty(statusOut) {
  const offenders = parsePorcelainPaths(statusOut).filter((p) => !EXEMPT_DIRTY_PATHS.has(p));
  return { ok: offenders.length === 0, offenders };
}

/**
 * 发布分支命名：`release/<YYMMDD>`，冲突依次 -2 / -3 …（上限 -20）。
 * 冲突来源 = 远端同名分支（tip 不同）∪ 历史 PR 的 headRefName（分支被自动删但名字仍算用掉）。
 * 远端同名且 tip 恰等于本地 main tip → 复用（覆盖「push 成功、开 PR 失败」的重跑），不再 push。
 */
export function pickBranchName({ ymd, mainSha, remoteBranches = new Map(), usedNames = new Set() }) {
  const base = `release/${ymd}`;
  for (let i = 1; i <= MAX_BRANCH_SUFFIX; i++) {
    const name = i === 1 ? base : `${base}-${i}`;
    const remoteSha = remoteBranches.get(name);
    if (remoteSha !== undefined) {
      if (remoteSha === mainSha) return { name, reuse: true };
      continue;
    }
    if (usedNames.has(name)) continue;
    return { name, reuse: false };
  }
  return { name: null, reuse: false, reason: 'exhausted' };
}

/** 从 `git log --first-parent --format=%s` 输出提取 merge-driver 合并行的 changeId（按序去重）。 */
export function extractChangeIds(logText) {
  const out = [];
  const seen = new Set();
  for (const line of String(logText || '').split(/\r?\n/)) {
    const m = /^Merge (\S+) \(merge-driver\)/.exec(line.trim());
    if (m && !seen.has(m[1])) {
      seen.add(m[1]);
      out.push(m[1]);
    }
  }
  return out;
}

/**
 * gov lane changeId 提取（261026 批 T7，ARQ-261026-release-pr-gov-lane-zero-pods）：从
 * `git diff --name-only origin/main..main -- delivery/review/` 输出取治理批目录前缀
 * （<YYMMDD>-(gov|arch)-review-<slug>），按序去重。gov 批无 merge-driver 合并行，changeId 只能从交付制品路径派生。
 */
export function extractGovChangeIds(diffNamesText) {
  const out = [];
  const seen = new Set();
  for (const line of String(diffNamesText || '').split(/\r?\n/)) {
    const m = /^delivery\/review\/(\d{6}-(?:gov|arch)-review-[A-Za-z0-9._-]+)\//.exec(line.trim());
    if (m && !seen.has(m[1])) { seen.add(m[1]); out.push(m[1]); }
  }
  return out;
}

/**
 * PR 标题（261026 批 T7）：dev lane = merge-driver 合并行（>0）→「N 个 Pod 合并」；
 * gov lane = delivery/review 治理批目录（dev 行为 0）→ 点名治理批；两线皆空 → 诚实报「变更线未识别」，
 * 不再落「0 个 Pod 合并」空门面（PR #40 实测病灶：gov 批发布时标题对评审者零信息量）。
 */
export function derivePrTitle({ ymd, suffix, changeIds, govChangeIds }) {
  if (changeIds.length) return `release(${ymd}${suffix}): ${changeIds.length} 个 Pod 合并`;
  if (govChangeIds.length) return `release(${ymd}${suffix}): gov 治理批 ${govChangeIds.join(', ')}`;
  return `release(${ymd}${suffix}): 变更线未识别（dev 合并行与 gov 交付制品均空）`;
}

/**
 * push argv 白名单（行为面判据）：push 子命令、显式 refspec、源 token 恒 `main`、
 * 目标 token 恒以 `refs/heads/release/` 起头、不含任何 force/delete/mirror/all 参数。
 * 违反即抛——调用方不得自行拼 argv 绕过本函数。
 */
export function assertPushArgv(argv) {
  if (!Array.isArray(argv) || argv[0] !== 'push') {
    throw new Error('push argv must start with the "push" subcommand');
  }
  const rest = argv.slice(1);
  for (const tok of rest) {
    if (FORBIDDEN_PUSH_TOKENS.has(tok) || tok.startsWith('--force')) {
      throw new Error(`forbidden push token: ${tok}`);
    }
  }
  const refspec = rest.find((t) => t.includes(':'));
  if (!refspec) throw new Error('push argv must carry an explicit <src>:<dst> refspec');
  const [src, dst] = refspec.split(':');
  if (src !== 'main') throw new Error(`push source must be exactly "main" (got "${src}")`);
  if (!dst.startsWith('refs/heads/release/')) {
    throw new Error(`push destination must start with refs/heads/release/ (got "${dst}")`);
  }
  return { src, dst };
}

/** 生成并校验 push argv。发布分支 tip 恒等于本地 main tip（refspec 源即 `main`）。 */
export function planPush({ branch }) {
  const argv = ['push', 'origin', `main:refs/heads/${branch}`];
  assertPushArgv(argv);
  return argv;
}

/** gh 子命令白名单判据。 */
export function judgeGhArgs(args) {
  const a = Array.isArray(args) ? args.map(String) : [];
  for (const prefix of GH_ALLOWED_PREFIXES) {
    if (prefix.every((tok, i) => a[i] === tok)) return { ok: true };
  }
  return { ok: false, reason: `gh subcommand not allowed: ${a.slice(0, 3).join(' ') || '<empty>'}` };
}

/** 所有 gh 调用都必须经这里；白名单外的子命令在调用前即被拒绝（runner 不会被执行）。 */
export function runGh(args, runner) {
  const verdict = judgeGhArgs(args);
  if (!verdict.ok) throw new Error(verdict.reason);
  return runner(args);
}

/** `gh pr create` argv（live 路径与自检夹具共用同一生成器）。 */
export function planPrCreate({ head, title, bodyFile }) {
  return ['pr', 'create', '--base', 'main', '--head', head, '--title', title, '--body-file', bodyFile];
}

export function buildPrBody({ changeIds, govChangeIds = [], baseSha, tipSha, branch }) {
  const section = changeIds.length
    ? ['本批已合并 Pod：', ...changeIds.map((id) => `- ${id}`)]
    : govChangeIds.length
      ? ['本发布为 gov 治理批：', ...govChangeIds.map((id) => `- gov 治理批 ${id}（制品见 delivery/review/${id}/）`)]
      : ['本变更线未识别（dev 的 merge-driver 合并行与 gov 交付制品均为空）：请在合并前于 PR 描述补充本批变更说明。'];
  return [
    `发布分支：\`${branch}\``,
    `基线（origin/main）：\`${baseSha}\``,
    `release tip（= 本地 main）：\`${tipSha}\``,
    '',
    ...section,
    '',
    '仓库仅允许 merge commit。human 在网页合并后，请在主 worktree 跑 `node .claude/scripts/release-pr.mjs --sync` 追平本地 main。',
  ].join('\n');
}

/** 本地日期 → YYMMDD。 */
export function todayYmd(d = new Date()) {
  return `${String(d.getFullYear()).slice(2)}${String(d.getMonth() + 1).padStart(2, '0')}${String(d.getDate()).padStart(2, '0')}`;
}

// ---- 编排面（deps 注入，自检直接喂 fake deps） ----

/**
 * --publish 编排。返回 { exit, reason, ... }。
 * 预检全部通过前不产生任何远端写。
 */
export function publishFlow(deps, { title = null } = {}) {
  const log = deps.log || (() => {});

  // 1. 运行位置：必须持有 refs/heads/main 的主 worktree。
  const branch = deps.currentBranch();
  if (branch !== 'main') {
    return { exit: 2, reason: 'wrong-worktree', message: `must run on branch main in the main worktree (current: ${branch || '<unknown>'})` };
  }

  // 2. 清洁度：仅放行 .codebase-memory 两件（按路径计，不限状态码）。
  const dirty = classifyDirty(deps.gitStatus());
  if (!dirty.ok) {
    return { exit: 2, reason: 'dirty', offenders: dirty.offenders, message: 'working tree is not clean (unlisted files below)' };
  }

  // 3. gh 可用。
  const auth = deps.ghAuthStatus();
  if (!auth.ok) return { exit: 2, reason: 'gh-auth', message: 'gh auth status failed (need repo scope)' };

  // 4. 分支关系可判定。
  const fetched = deps.fetchMain();
  if (!fetched.ok) return { exit: 2, reason: 'fetch-failed', message: `git fetch origin main failed: ${fetched.error || 'unknown'}` };
  const mainSha = deps.revParse('main');
  const originSha = deps.revParse('origin/main');
  const relation = resolveBranchRelation({ mainSha, originSha, isAncestor: deps.isAncestor });
  if (relation === 'equal') return { exit: 0, reason: 'nothing-to-publish', message: 'nothing to publish' };
  if (relation === 'behind') return { exit: 2, reason: 'behind', message: 'local main is behind origin/main — run --sync first' };
  if (relation === 'diverged') return { exit: 2, reason: 'diverged', message: 'local main and origin/main have diverged — human adjudication required' };
  if (relation !== 'ahead') return { exit: 2, reason: 'relation-unknown', message: `cannot determine main/origin-main relation (${relation})` };

  // 6. 幂等闸：同一 tip 已有 open PR → 打印 URL 直接退出（不 push、不建 PR）。
  const openPrs = deps.prList({ state: 'open', limit: 100 });
  const sameTip = openPrs.find((pr) => pr.headRefOid === mainSha);
  if (sameTip) {
    log(`an open PR already targets this exact main tip: ${sameTip.url}`);
    log('reminder: 发布后本地 main 若继续前进，须再发一次（否则合并后仍可能互非祖先）');
    return { exit: 0, reason: 'open-pr-same-tip', url: sameTip.url };
  }

  // 5. 分支命名（远端同名 tip 相同 → 复用；名字已被历史 PR 用过 → 换后缀）。
  const allPrs = deps.prList({ state: 'all', limit: 100 });
  const usedNames = new Set(allPrs.map((pr) => pr.headRefName));
  const picked = pickBranchName({
    ymd: deps.today(),
    mainSha,
    remoteBranches: deps.listRemoteReleaseBranches(),
    usedNames,
  });
  if (!picked.name) return { exit: 2, reason: 'name-exhausted', message: 'release branch suffix exhausted (-2 … -20)' };

  // 7. push（复用同 tip 分支时跳过）。
  if (!picked.reuse) {
    const argv = planPush({ branch: picked.name });
    const pushed = deps.push(argv);
    if (!pushed.ok) {
      return { exit: 1, reason: 'push-failed', branch: picked.name, error: pushed.error || 'unknown' };
    }
    log(`pushed main:refs/heads/${picked.name} (tip ${mainSha})`);
  } else {
    log(`reusing existing remote branch ${picked.name} (tip already equals local main)`);
  }

  // 8. 开 PR（261026 批 T7：lane 感知标题/正文——dev lane 记 Pod，gov lane 点名治理批与制品目录）。
  const suffix = picked.name.slice(`release/${deps.today()}`.length);
  const changeIds = extractChangeIds(deps.mergeLog());
  const govChangeIds = extractGovChangeIds(deps.reviewDiffNames ? deps.reviewDiffNames() : '');
  const prTitle = title || derivePrTitle({ ymd: deps.today(), suffix, changeIds, govChangeIds });
  const created = deps.ghCreatePr({
    head: picked.name,
    title: prTitle,
    body: buildPrBody({ changeIds, govChangeIds, baseSha: originSha, tipSha: mainSha, branch: picked.name }),
  });
  if (!created.ok) {
    return {
      exit: 1,
      reason: 'pr-create-failed',
      branch: picked.name,
      error: created.error || 'unknown',
      hint: 'rerun is safe — the same branch tip will be reused',
    };
  }

  log(`PR created: ${created.url}`);
  log('next: human merges the PR on GitHub (merge commit only); then run --sync in the main worktree');
  return { exit: 0, reason: 'published', url: created.url, branch: picked.name, changeIds };
}

/**
 * --sync 编排：只做本地 main 的 ff 追平，不 push。
 * equal → already in sync；behind → merge --ff-only origin/main；ahead / diverged → 拒绝交 human。
 */
export function syncFlow(deps) {
  const log = deps.log || (() => {});

  const branch = deps.currentBranch();
  if (branch !== 'main') {
    return { exit: 2, reason: 'wrong-worktree', message: `must run on branch main in the main worktree (current: ${branch || '<unknown>'})` };
  }
  const dirty = classifyDirty(deps.gitStatus());
  if (!dirty.ok) {
    return { exit: 2, reason: 'dirty', offenders: dirty.offenders, message: 'working tree is not clean' };
  }
  const fetched = deps.fetchMain();
  if (!fetched.ok) return { exit: 2, reason: 'fetch-failed', message: `git fetch origin main failed: ${fetched.error || 'unknown'}` };

  const mainSha = deps.revParse('main');
  const originSha = deps.revParse('origin/main');
  const relation = resolveBranchRelation({ mainSha, originSha, isAncestor: deps.isAncestor });

  if (relation === 'equal') {
    log('already in sync');
    return { exit: 0, reason: 'already-in-sync' };
  }
  if (relation === 'behind') {
    const ff = deps.ffMerge();
    if (!ff.ok) return { exit: 1, reason: 'ff-failed', error: ff.error || 'unknown' };
    const tip = deps.revParse('main');
    log(`fast-forwarded local main to ${tip}`);
    return { exit: 0, reason: 'fast-forwarded', tip };
  }
  return {
    exit: 2,
    reason: relation === 'diverged' ? 'diverged' : 'ahead',
    message: relation === 'ahead'
      ? 'local main is ahead of origin/main — publish first; --sync never moves origin'
      : 'local main and origin/main have diverged — human adjudication required',
  };
}

// ---- live deps ----

function git(args, { cwd = process.cwd(), env = {} } = {}) {
  try {
    const stdout = execFileSync('git', args, {
      cwd,
      encoding: 'utf8',
      stdio: ['ignore', 'pipe', 'pipe'],
      timeout: GIT_FETCH_TIMEOUT_MS,
      env: { ...process.env, ...env },
    });
    return { ok: true, stdout };
  } catch (error) {
    return { ok: false, error: `${error.stderr || error.message || error}`.trim().split('\n').slice(-3).join(' ') };
  }
}

function ghRunner(args) {
  try {
    const stdout = execFileSync('gh', args, {
      encoding: 'utf8',
      stdio: ['ignore', 'pipe', 'pipe'],
      timeout: GH_TIMEOUT_MS,
    });
    return { ok: true, stdout };
  } catch (error) {
    return { ok: false, error: `${error.stderr || error.message || error}`.trim().split('\n').slice(-3).join(' ') };
  }
}

function log(line) {
  process.stdout.write(`[release-pr] ${line}\n`);
}

export function makeLiveDeps({ dryRun = false, cwd = process.cwd(), now = () => new Date() } = {}) {
  return {
    log,
    currentBranch: () => git(['rev-parse', '--abbrev-ref', 'HEAD'], { cwd }).stdout?.trim() || null,
    gitStatus: () => git(['status', '--porcelain', '-uall'], { cwd }).stdout || '',
    fetchMain: () => {
      const r = git(['fetch', 'origin', 'main'], { cwd, env: { GIT_TERMINAL_PROMPT: '0' } });
      return { ok: r.ok, error: r.error };
    },
    revParse: (ref) => {
      const r = git(['rev-parse', '--verify', ref], { cwd });
      return r.ok ? r.stdout.trim() : null;
    },
    isAncestor: (a, b) => git(['merge-base', '--is-ancestor', a, b], { cwd }).ok,
    listRemoteReleaseBranches: () => {
      const r = git(['ls-remote', '--heads', 'origin', 'release/*'], { cwd });
      const map = new Map();
      if (!r.ok) return map;
      for (const line of r.stdout.split(/\r?\n/)) {
        const m = /^([0-9a-f]{7,40})\s+refs\/heads\/(release\/.+)$/.exec(line.trim());
        if (m) map.set(m[2], m[1]);
      }
      return map;
    },
    prList: ({ state, limit }) => {
      const args = ['pr', 'list', '--state', state, '--limit', String(limit), '--json', 'number,url,headRefName,headRefOid'];
      const r = runGh(args, ghRunner);
      if (!r.ok) return [];
      try {
        return JSON.parse(r.stdout || '[]');
      } catch {
        return [];
      }
    },
    ghAuthStatus: () => runGh(['auth', 'status'], ghRunner),
    today: () => todayYmd(now()),
    mergeLog: () => git(['log', '--first-parent', '--format=%s', 'origin/main..main'], { cwd }).stdout || '',
    reviewDiffNames: () => git(['diff', '--name-only', 'origin/main..main', '--', 'delivery/review/'], { cwd }).stdout || '',
    push: (argv) => {
      if (dryRun) {
        log(`[dry-run] would run: git ${argv.join(' ')}`);
        return { ok: true };
      }
      return git(argv, { cwd, env: { GIT_TERMINAL_PROMPT: '0' } });
    },
    ffMerge: () => {
      if (dryRun) {
        log('[dry-run] would run: git merge --ff-only origin/main');
        return { ok: true };
      }
      return git(['merge', '--ff-only', 'origin/main'], { cwd });
    },
    ghCreatePr: ({ head, title, body }) => {
      const argv = planPrCreate({ head, title, bodyFile: '<tmp>' });
      if (dryRun) {
        log(`[dry-run] would run: gh ${argv.join(' ')}`);
        log('[dry-run] PR body:');
        process.stdout.write(`${body}\n`);
        return { ok: true, url: '<dry-run>' };
      }
      const dir = mkdtempSync(join(tmpdir(), 'release-pr-'));
      const bodyFile = join(dir, 'body.md');
      try {
        writeFileSync(bodyFile, `${body}\n`, 'utf8');
        const r = runGh(planPrCreate({ head, title, bodyFile }), ghRunner);
        if (!r.ok) return { ok: false, error: r.error };
        const url = (r.stdout || '').trim().split(/\r?\n/).pop();
        return { ok: true, url };
      } finally {
        rmSync(dir, { recursive: true, force: true });
      }
    },
  };
}

// ---- 自检（V1：19 个夹具；全部注入 fake deps，不联网、不触远端） ----

function makeFakePublishDeps(overrides = {}) {
  const calls = { push: [], gh: [], lines: [] };
  const base = {
    log: (line) => calls.lines.push(line),
    currentBranch: () => 'main',
    gitStatus: () => ' M .codebase-memory/artifact.json\n D .codebase-memory/graph.db.zst\n',
    fetchMain: () => ({ ok: true }),
    revParse: (ref) => (ref === 'main' ? 'main-sha' : 'origin-sha'),
    isAncestor: (a, b) => a === 'origin-sha' && b === 'main-sha',
    listRemoteReleaseBranches: () => new Map(),
    prList: () => [],
    ghAuthStatus: () => ({ ok: true }),
    today: () => '261025',
    mergeLog: () => 'Merge 260101-p1 (merge-driver)\nchore: unrelated\nMerge 260102-p2 (merge-driver)\nMerge 260101-p1 (merge-driver)\n',
    reviewDiffNames: () => '',
    push: (argv) => {
      calls.push.push(argv);
      return { ok: true };
    },
    ffMerge: () => ({ ok: true }),
    ghCreatePr: ({ head, title, body }) => {
      const argv = planPrCreate({ head, title, bodyFile: '<tmp>' });
      runGh(argv, (a) => {
        calls.gh.push(a);
        return { ok: true, stdout: 'https://github.com/fandanpeng/KidPal/pull/99' };
      });
      return { ok: true, url: 'https://github.com/fandanpeng/KidPal/pull/99' };
    },
  };
  return { deps: { ...base, ...overrides }, calls };
}

export function runSelfCheck() {
  const results = [];
  const check = (label, cond, detail = '') => results.push({ label, pass: !!cond, detail });

  // 1–4：分支关系（经 publishFlow 走真实出口码与拒绝语义）。
  {
    const { deps } = makeFakePublishDeps({
      revParse: (ref) => (ref === 'main' ? 'same' : 'same'),
    });
    const r = publishFlow(deps);
    check('relation-equal', r.exit === 0 && r.reason === 'nothing-to-publish', `exit=${r.exit} reason=${r.reason}`);
  }
  {
    const { deps, calls } = makeFakePublishDeps();
    const r = publishFlow(deps);
    check('relation-ahead', r.exit === 0 && r.reason === 'published' && calls.push.length === 1, `exit=${r.exit} reason=${r.reason}`);
  }
  {
    const { deps } = makeFakePublishDeps({
      revParse: (ref) => (ref === 'main' ? 'main-sha' : 'origin-sha'),
      isAncestor: (a, b) => a === 'main-sha' && b === 'origin-sha', // 本地是 origin 的祖先 = behind
    });
    const r = publishFlow(deps);
    check('relation-behind', r.exit === 2 && r.reason === 'behind' && /--sync/.test(r.message || ''), `exit=${r.exit} reason=${r.reason}`);
  }
  {
    const { deps } = makeFakePublishDeps({ isAncestor: () => false }); // 互非祖先 = diverged
    const r = publishFlow(deps);
    check('relation-diverged', r.exit === 2 && r.reason === 'diverged' && /human/.test(r.message || ''), `exit=${r.exit} reason=${r.reason}`);
  }

  // 5–6：清洁度。
  {
    const d = classifyDirty(' M .codebase-memory/artifact.json\n D .codebase-memory/graph.db.zst\n');
    check('dirty-allowed-only', d.ok === true && d.offenders.length === 0, `offenders=${JSON.stringify(d.offenders)}`);
  }
  {
    const { deps } = makeFakePublishDeps({
      gitStatus: () => ' M .codebase-memory/artifact.json\n M delivery/review/INDEX.md\n',
    });
    const r = publishFlow(deps);
    check('dirty-offender-named', r.exit === 2 && r.reason === 'dirty' && (r.offenders || []).includes('delivery/review/INDEX.md'), `offenders=${JSON.stringify(r.offenders)}`);
  }

  // 7：运行位置。
  {
    const { deps } = makeFakePublishDeps({ currentBranch: () => 'gov/kidpal' });
    const r = publishFlow(deps);
    check('wrong-worktree', r.exit === 2 && r.reason === 'wrong-worktree', `exit=${r.exit} reason=${r.reason}`);
  }

  // 8–11：分支命名。
  {
    const p = pickBranchName({ ymd: '261025', mainSha: 'main-sha' });
    check('name-free', p.name === 'release/261025' && p.reuse === false, `name=${p.name} reuse=${p.reuse}`);
  }
  {
    const p = pickBranchName({ ymd: '261025', mainSha: 'main-sha', remoteBranches: new Map([['release/261025', 'other-sha']]) });
    check('name-collision-bump', p.name === 'release/261025-2' && p.reuse === false, `name=${p.name}`);
  }
  {
    const p = pickBranchName({ ymd: '261025', mainSha: 'main-sha', remoteBranches: new Map([['release/261025', 'main-sha']]) });
    check('name-same-sha-reuse', p.name === 'release/261025' && p.reuse === true, `name=${p.name} reuse=${p.reuse}`);
  }
  {
    const p = pickBranchName({ ymd: '261025', mainSha: 'main-sha', usedNames: new Set(['release/261025']) });
    check('name-used-by-merged-pr', p.name === 'release/261025-2' && p.reuse === false, `name=${p.name}`);
  }

  // 12：同 tip open PR 幂等闸。
  {
    const { deps, calls } = makeFakePublishDeps({
      prList: ({ state }) => (state === 'open' ? [{ headRefOid: 'main-sha', url: 'https://example.test/pull/7', headRefName: 'release/261025' }] : []),
    });
    const r = publishFlow(deps);
    check('open-pr-same-tip-noop', r.exit === 0 && r.reason === 'open-pr-same-tip' && r.url === 'https://example.test/pull/7' && calls.push.length === 0 && calls.gh.length === 0, `exit=${r.exit} reason=${r.reason} push=${calls.push.length} gh=${calls.gh.length}`);
  }

  // 13：changeId 提取。
  {
    const ids = extractChangeIds('Merge 260101-p1 (merge-driver)\nchore: unrelated\nMerge 260102-p2 (merge-driver)\nMerge 260101-p1 (merge-driver)\n');
    check('extract-changeids', JSON.stringify(ids) === JSON.stringify(['260101-p1', '260102-p2']), `ids=${JSON.stringify(ids)}`);
  }

  // 14：push argv 白名单（源恒 main、目标前缀、推送 tip == 本地 main tip）。
  {
    const argv = planPush({ branch: 'release/261025' });
    const okPlan = JSON.stringify(argv) === JSON.stringify(['push', 'origin', 'main:refs/heads/release/261025']);
    let tamperedRejected = false;
    try {
      assertPushArgv(['push', 'origin', 'dev/x:refs/heads/release/261025']);
    } catch {
      tamperedRejected = true;
    }
    let forceRejected = false;
    try {
      assertPushArgv(['push', 'origin', '--force', 'main:refs/heads/release/261025']);
    } catch {
      forceRejected = true;
    }
    check('push-plan-source-main', okPlan && tamperedRejected && forceRejected, `argv=${JSON.stringify(argv)} tampered=${tamperedRejected} force=${forceRejected}`);
  }

  // 15：gh 子命令白名单（白名单外调用前拒绝，runner 不执行）。
  {
    const seen = [];
    let rejected = false;
    try {
      runGh(['pr', 'merge', '31'], (args) => {
        seen.push(args);
        return { ok: true, stdout: '' };
      });
    } catch {
      rejected = true;
    }
    runGh(['pr', 'list', '--state', 'open'], (args) => {
      seen.push(args[0] + ' ' + args[1]);
      return { ok: true, stdout: '[]' };
    });
    const allowedOk = seen.length === 1 && seen[0] === 'pr list';
    check('gh-argv-whitelist', rejected && allowedOk, `rejected=${rejected} executed=${JSON.stringify(seen)}`);
  }

  // 16：happy path（create 阶段夹具，预检以 fake 结果打桩，不记录预检 gh 调用）。
  {
    const { deps, calls } = makeFakePublishDeps();
    const r = publishFlow(deps);
    const pushOnce = calls.push.length === 1;
    const pushCompliant = pushOnce && (() => {
      try {
        assertPushArgv(calls.push[0]);
        return true;
      } catch {
        return false;
      }
    })();
    const ghCreateCalls = calls.gh.filter((a) => a[0] === 'pr' && a[1] === 'create');
    const oneCreate = ghCreateCalls.length === 1;
    const argv = ghCreateCalls[0] || [];
    const baseOk = argv.includes('--base') && argv[argv.indexOf('--base') + 1] === 'main';
    const headOk = argv.includes('--head') && argv[argv.indexOf('--head') + 1] === 'release/261025';
    const urlPrinted = calls.lines.some((l) => l.includes('https://github.com/fandanpeng/KidPal/pull/99'));
    check(
      'publish-happy-path',
      r.exit === 0 && r.reason === 'published' && pushOnce && pushCompliant && oneCreate && baseOk && headOk && urlPrinted,
      `exit=${r.exit} push=${calls.push.length} gh=${JSON.stringify(calls.gh)} base=${baseOk} head=${headOk} url=${urlPrinted}`,
    );
  }

  // 17–19：--sync 三态。
  {
    const { deps, calls } = makeFakePublishDeps({ revParse: () => 'same' });
    const r = syncFlow(deps);
    check('sync-equal', r.exit === 0 && r.reason === 'already-in-sync' && calls.push.length === 0, `exit=${r.exit} reason=${r.reason}`);
  }
  {
    let ffCalls = 0;
    const { deps, calls } = makeFakePublishDeps({
      revParse: (ref) => (ref === 'main' ? 'main-sha' : 'origin-sha'),
      isAncestor: (a, b) => a === 'main-sha' && b === 'origin-sha',
      ffMerge: () => {
        ffCalls += 1;
        return { ok: true };
      },
    });
    const r = syncFlow(deps);
    check('sync-ff', r.exit === 0 && r.reason === 'fast-forwarded' && ffCalls === 1 && calls.push.length === 0, `exit=${r.exit} ff=${ffCalls}`);
  }
  {
    let ffCalls = 0;
    const { deps } = makeFakePublishDeps({
      ffMerge: () => {
        ffCalls += 1;
        return { ok: true };
      },
    });
    const r = syncFlow(deps); // 默认 fake 关系 = ahead
    check('sync-refused', r.exit === 2 && r.reason === 'ahead' && ffCalls === 0, `exit=${r.exit} reason=${r.reason} ff=${ffCalls}`);
  }
  // 20（261026 批 T7）：gov lane changeId 提取
  {
    const ids = extractGovChangeIds([
      'delivery/review/260101-gov-review-x/PROPOSAL.md',
      'delivery/review/260101-gov-review-x/retro.md',
      'delivery/review/260102-arch-review-y/PROPOSAL.md',
      'delivery/review/INDEX.md',
      'delivery/review/.changes-index-state.json',
      'delivery/intake/z.md',
    ].join('\n'));
    check('extract-gov-changeids', JSON.stringify(ids) === JSON.stringify(['260101-gov-review-x', '260102-arch-review-y']), `ids=${JSON.stringify(ids)}`);
  }
  // 21（261026 批 T7）：gov lane 横幅——标题点名治理批、正文给制品指针（正向），且不落「0 个 Pod」空门面（负向配对）
  {
    let captured = null;
    const { deps } = makeFakePublishDeps({
      mergeLog: () => '',
      reviewDiffNames: () => 'delivery/review/260101-gov-review-x/PROPOSAL.md\n',
      ghCreatePr: (payload) => { captured = payload; return { ok: true, url: 'https://example.test/pull/88' }; },
    });
    const r = publishFlow(deps);
    check('gov-lane-title', r.exit === 0 && !!captured && captured.title === 'release(261025): gov 治理批 260101-gov-review-x', `title=${captured && captured.title}`);
    check('gov-lane-body-pointer', !!captured && captured.body.includes('gov 治理批 260101-gov-review-x（制品见 delivery/review/260101-gov-review-x/）'), 'body missing pointer');
    check('gov-lane-no-zero-pod', !!captured && !captured.title.includes('个 Pod') && !captured.body.includes('个 Pod') && !captured.body.includes('未从 merge-driver 合并行提取到 changeId'), 'zero-pod facade still present');
  }
  // 22：dev lane 正向配对——合并行在场时标题仍记 Pod 数
  {
    let captured = null;
    const { deps } = makeFakePublishDeps({ ghCreatePr: (payload) => { captured = payload; return { ok: true, url: 'u' }; } });
    publishFlow(deps);
    check('dev-lane-title', !!captured && captured.title === 'release(261025): 2 个 Pod 合并', `title=${captured && captured.title}`);
  }
  // 23：两线皆空——诚实报「变更线未识别」
  {
    let captured = null;
    const { deps } = makeFakePublishDeps({ mergeLog: () => '', reviewDiffNames: () => 'delivery/review/INDEX.md\n', ghCreatePr: (payload) => { captured = payload; return { ok: true, url: 'u' }; } });
    publishFlow(deps);
    check('unknown-lane-title', !!captured && captured.title.includes('变更线未识别') && !captured.title.includes('个 Pod'), `title=${captured && captured.title}`);
    check('unknown-lane-body', !!captured && captured.body.includes('本变更线未识别') && !captured.body.includes('未从 merge-driver 合并行提取到 changeId'), 'body fallback mismatch');
  }

  const pass = results.filter((r) => r.pass).length;
  const fail = results.length - pass;
  for (const r of results) {
    process.stdout.write(`  ${r.pass ? 'PASS' : 'FAIL'} · ${r.label}${r.pass ? '' : ` — ${r.detail}`}\n`);
  }
  process.stdout.write(`${JSON.stringify({ selfCheck: 'release-pr', pass, fail, total: results.length, ok: fail === 0 })}\n`);
  return fail === 0;
}

// ---- CLI ----

function usage() {
  process.stderr.write(
    'usage: node .claude/scripts/release-pr.mjs --publish [--title <title>] [--dry-run]\n' +
      '       node .claude/scripts/release-pr.mjs --sync [--dry-run]\n' +
      '       node .claude/scripts/release-pr.mjs --self-check\n',
  );
}

function main() {
  const argv = process.argv.slice(2);
  if (argv.includes('--self-check')) {
    process.exit(runSelfCheck() ? 0 : 1);
  }

  const dryRun = argv.includes('--dry-run');
  const titleIdx = argv.indexOf('--title');
  const title = titleIdx >= 0 ? argv[titleIdx + 1] ?? null : null;
  const deps = makeLiveDeps({ dryRun });

  if (argv.includes('--publish')) {
    const r = publishFlow(deps, { title });
    if (r.exit === 0) {
      if (r.url) process.stdout.write(`[release-pr] PR: ${r.url}\n`);
      process.exit(0);
    }
    if (r.message) process.stderr.write(`[release-pr] ${r.reason}: ${r.message}\n`);
    if (r.offenders && r.offenders.length) {
      process.stderr.write(`[release-pr] offending paths:\n${r.offenders.map((p) => `  - ${p}`).join('\n')}\n`);
    }
    if (r.error) process.stderr.write(`[release-pr] ${r.error}\n`);
    if (r.hint) process.stderr.write(`[release-pr] hint: ${r.hint}\n`);
    process.exit(r.exit);
  }

  if (argv.includes('--sync')) {
    const r = syncFlow(deps);
    if (r.exit === 0) process.exit(0);
    if (r.message) process.stderr.write(`[release-pr] ${r.reason}: ${r.message}\n`);
    if (r.offenders && r.offenders.length) {
      process.stderr.write(`[release-pr] offending paths:\n${r.offenders.map((p) => `  - ${p}`).join('\n')}\n`);
    }
    if (r.error) process.stderr.write(`[release-pr] ${r.error}\n`);
    process.exit(r.exit);
  }

  usage();
  process.exit(2);
}

main();
