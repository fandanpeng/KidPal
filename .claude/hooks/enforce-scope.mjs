#!/usr/bin/env node
// PreToolUse 钩子：按 .claude/scope-map.json 强制每个 dev-loop agent 的可写路径边界。
// 从 stdin 读取 Claude Code 的钩子 JSON；当某个 agent 试图写越界文件时拦截（exit 2）。
// 用 Node 实现，跨平台、无 shell 依赖（Windows / macOS / Linux 通用）。
//
// 关键事实（已对照官方文档）：
//   - 子 agent 的 `tools` 字段是“工具级”允许列表，不是“路径级”；真正的路径冻结
//     靠 settings 的 permissions.deny + 本 PreToolUse 钩子来做。
//   - 钩子在子 agent 内触发时，输入里带 agent_type；主线程/人类没有该字段。
//   - PreToolUse 以 exit 2 拦截，stderr 内容回灌给模型作为拒绝原因。

import { existsSync, mkdtempSync, mkdirSync, readFileSync, rmSync, statSync, writeFileSync } from 'node:fs';
import { dirname, join, resolve, relative, sep } from 'node:path';
import { tmpdir } from 'node:os';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';

const scriptPath = fileURLToPath(import.meta.url);

function readStdin() {
  try {
    return readFileSync(0, 'utf8');
  } catch {
    return '';
  }
}

// 极简 glob -> RegExp：支持 ** (跨目录)、* (同级任意)、? (单字符)，分隔符统一为 /。
function globToRegExp(glob) {
  let re = '';
  for (let i = 0; i < glob.length; i++) {
    const c = glob[i];
    if (c === '*') {
      if (glob[i + 1] === '*') {
        re += '.*';
        i++;
        if (glob[i + 1] === '/') i++;
      } else {
        re += '[^/]*';
      }
    } else if (c === '?') {
      re += '[^/]';
    } else if ('\\^$.|+()[]{}'.includes(c)) {
      re += '\\' + c;
    } else {
      re += c;
    }
  }
  return new RegExp('^' + re + '$');
}

function matchesAny(relPath, globs) {
  return (globs || []).some((g) => globToRegExp(g).test(relPath));
}

// 通用结构红线检查（不分 agent，所有会话生效）。命中返回拦截理由，否则 null。
function checkUniversal(rel, universal) {
  if (matchesAny(rel, universal.denyCreate)) {
    return `scope(universal): 禁止写 ${rel}（.claude/scope-map.json universal.denyCreate——影子目录或受保护区）。`;
  }
  const dt = universal.docsTopLevel;
  if (dt && rel.startsWith('docs/')) {
    const parts = rel.split('/');
    if (parts.length === 2) {
      if (!(dt.allowFiles || []).includes(rel)) {
        return `scope(universal): 禁止在 docs/ 根新建 ${rel}。过程产物→ delivery/taskpackages/<id>/ 或 delivery/review/arch-review-<spec>/；若确为新的正式顶层文档，先登记进 .claude/scope-map.json 的 universal.docsTopLevel.allowFiles 与 .claude/rules/repo-structure.md。`;
      }
    } else if (parts.length >= 3) {
      const topDir = parts[0] + '/' + parts[1];
      if (!(dt.allowDirs || []).includes(topDir) && !(dt.allowFiles || []).includes(rel)) {
        return `scope(universal): docs/ 下未登记的子目录 ${topDir}/（已登记：${(dt.allowDirs || []).join(', ')}）。新增类别先登记进 universal.docsTopLevel.allowDirs 与 .claude/rules/repo-structure.md。`;
      }
    }
  }
  return null;
}

function block(reason) {
  process.stderr.write(reason);
  process.exit(2);
}

// 从 Bash 命令串里提取「显式重定向写入目标」（启发式，非完整 shell 解析）。
// 覆盖：> / >> / N> / N>> / &> / &>> 重定向、tee [-flags] file、dd of=file。
// 跳过：fd 复制（>&2 / 2>&1，目标以 & 起始或目标字符类直接排除 &）、/dev/{null,stderr,stdout,stdin,tty}、
//       here-string(<<<) 与 heredoc(<<) 本身（它们是 < 字符，不被 >>? 匹配）。
// 有意不覆盖（残余面）：cp/mv/rsync 的目的参数、以及被 spawn 程序内部的 fs 写入
//       （node verify.mjs 写 .claude/verify-logs、npm build/install 写 .next/node_modules、测试器写 coverage）。
//       这些要么目的参数难以可靠判定，要么写入发生在子程序内部、命令串里看不到路径。
// 定位：抬高「cat > contracts/… / cat > .claude/…」这类显式绕过的门槛，不是完整沙箱。
function extractBashWriteTargets(cmd) {
  const targets = new Set();
  const stripQuotes = (s) => s.replace(/^["']|["']$/g, '');
  const skippable = (t) =>
    !t || t.startsWith('&') || /^\/dev\/(null|stderr|stdout|stdin|tty)/.test(t);

  // 1) 输出重定向：前缀为行首或空白/分隔符；可选 fd 号；目标 token 可带成对引号。
  const redirectRe = /(?:^|[\s;|&(])(?:\d*)(?:&>>?|>>?)\s*("[^"]*"|'[^']*'|[^\s;|&<>]+)/g;
  let m;
  while ((m = redirectRe.exec(cmd))) {
    const t = stripQuotes(m[1]);
    if (!skippable(t)) targets.add(t);
  }

  // 2) tee（可带 -a 等标志，取第一个非标志目标；多文件罕见，首个已足够抬高门槛）。
  const teeRe =
    /(?:^|[\s;|&])tee\s+((?:"[^"]*"|'[^']*'|[^\s;|&<>]+)(?:\s+(?:"[^"]*"|'[^']*'|[^\s;|&<>]+))*)/g;
  while ((m = teeRe.exec(cmd))) {
    const toks = m[1]
      .split(/\s+/)
      .map(stripQuotes)
      .filter((t) => t && !t.startsWith('-'));
    if (toks.length && !skippable(toks[0])) targets.add(toks[0]);
  }

  // 3) dd of=file
  const ddRe = /\bof=("[^"]*"|'[^']*'|[^\s;|&<>]+)/g;
  while ((m = ddRe.exec(cmd))) {
    const t = stripQuotes(m[1]);
    if (!skippable(t)) targets.add(t);
  }

  return [...targets];
}

// ── worktree 感知重锚（261015-gov-review-lite-fast-path T5）──
// rel 锚定原假设单树（CLAUDE_PROJECT_DIR）；worktree 池化（260915 批）后「本仓」= 主 checkout + 多棵在册树。
// 目标路径落在本仓（同 common dir）任一在册树 → rel 重锚到该树、读**该树**的 .claude/scope-map.json 判定
// （Bash 重定向分支与 Edit/Write 分支同走重锚）；目标树 scope-map 缺失/不可读 = fail-closed 拦截（不向宽）；
// 非本仓路径（c:\tmp、异仓）行为不变。定位纯 fs（.git 文件/目录 + worktrees gitdir 映射），不 spawn git——钩子是热路径。
function locateGitTree(startPath) {
  // 从起点自身开始向上探（起点可以是目录——homeProjectDir 的 .git 在其自身内；也可以是不存在的文件路径——
  // join(file, '.git') 恒不存在，自然落到 dirname 继续）。
  let cur = startPath;
  for (;;) {
    const dotGit = join(cur, '.git');
    if (existsSync(dotGit)) {
      if (statSync(dotGit).isDirectory()) {
        return { treeRoot: cur, commonDir: dotGit }; // 主 checkout
      }
      const m = /^gitdir:\s*(.+)\s*$/.exec(readFileSync(dotGit, 'utf8'));
      if (m) {
        const gitdir = resolve(cur, m[1].trim()); // <commonDir>/worktrees/<name>/gitdir
        // commonDir = gitdir 上三级（…/worktrees/<name>/gitdir → …/worktrees/<name> → …/worktrees → <commonDir>）
        return { treeRoot: cur, commonDir: dirname(dirname(dirname(gitdir))) };
      }
    }
    const parent = dirname(cur);
    if (parent === cur) return null; // 到达盘根仍未找到
    cur = parent;
  }
}

function samePath(a, b) {
  return resolve(a).replace(/[\\/]+$/, '').toLowerCase() === resolve(b).replace(/[\\/]+$/, '').toLowerCase();
}

// 返回 { mode: 'legacy' }（本树 / 非本仓路径——行为不变）| { mode: 'anchor', treeRoot, map }（在册兄弟树重锚）
// | { mode: 'fail-closed', reason }（目标树 scope-map 缺失/不可读——拦截并点名）。
function anchorForTarget(absPath, homeProjectDir) {
  const home = locateGitTree(homeProjectDir);
  if (!home) return { mode: 'legacy' };
  const target = locateGitTree(absPath);
  if (!target || !samePath(target.commonDir, home.commonDir)) return { mode: 'legacy' }; // 非本仓路径
  if (samePath(target.treeRoot, home.treeRoot)) return { mode: 'legacy' }; // 本树 → 原逻辑
  const mapPath = resolve(target.treeRoot, '.claude', 'scope-map.json');
  let map;
  try {
    map = JSON.parse(readFileSync(mapPath, 'utf8'));
  } catch {
    return {
      mode: 'fail-closed',
      reason: `scope(anchor): 目标树 ${target.treeRoot} 的 .claude/scope-map.json 缺失或不可读——跨树重锚 fail-closed（261015-gov-review-lite-fast-path T5，不向宽）。`,
    };
  }
  return { mode: 'anchor', treeRoot: target.treeRoot, map };
}

function main() {
  const raw = readStdin();
  let input;
  try {
    input = JSON.parse(raw);
  } catch {
    process.exit(0); // 拿不到结构化输入就不干预
  }

  const projectDir = process.env.CLAUDE_PROJECT_DIR || input.cwd || process.cwd();
  let map;
  try {
    map = JSON.parse(readFileSync(resolve(projectDir, '.claude/scope-map.json'), 'utf8'));
  } catch {
    process.exit(0); // 没有 scope-map 就不干预
  }

  // ── Bash 分支：只守 universal.bashDenyWrite 结构红线（契约冻结 / 强制层自改）。
  //    不套 per-agent allow/deny —— verifier(deny[**]) 本职要跑 Bash 测试、builder 的
  //    build/install 会写 allow 外的产物目录，故 Bash 只拦「任何 agent 都不该用 shell
  //    重定向写」的少数结构面；程序内部 fs 写入不受此限（命令串里看不到路径）。
  if (input?.tool_name === 'Bash') {
    const cmd = input?.tool_input?.command || '';
    for (const tgt of extractBashWriteTargets(cmd)) {
      const absT = resolve(input.cwd || projectDir, tgt);
      const anchor = anchorForTarget(absT, projectDir);
      if (anchor.mode === 'fail-closed') block(anchor.reason);
      const baseDir = anchor.mode === 'anchor' ? anchor.treeRoot : projectDir;
      const denyList = (anchor.mode === 'anchor' ? anchor.map.universal?.bashDenyWrite : map.universal?.bashDenyWrite) || [];
      if (!denyList.length) continue;
      const relT = relative(baseDir, absT).split(sep).join('/');
      if (matchesAny(relT, denyList)) {
        block(
          `scope(universal): 禁止用 Bash 重定向写 ${relT}（universal.bashDenyWrite——契约冻结 / 强制层结构红线；跨树写按目标树 scope-map 判定）。` +
            `改写这些面请用受门的 Write/Edit（各有 per-agent allow + settings.deny 把关），不要用 cat >/tee/重定向绕过。`,
        );
      }
    }
    process.exit(0);
  }

  // ── Edit/Write/MultiEdit 分支（既有 per-agent 逻辑 + T5 跨树重锚）
  const filePath = input?.tool_input?.file_path;
  if (!filePath) process.exit(0); // 该工具调用没有目标文件路径

  const abs = resolve(input.cwd || projectDir, filePath);
  const anchor = anchorForTarget(abs, projectDir);
  if (anchor.mode === 'fail-closed') block(anchor.reason);
  const baseDir = anchor.mode === 'anchor' ? anchor.treeRoot : projectDir;
  const effMap = anchor.mode === 'anchor' ? anchor.map : map;
  const rel = relative(baseDir, abs).split(sep).join('/');

  // 通用结构红线：不分 agent，对所有会话（含普通会话 / 主线程 / 人类）生效。
  // 防止 docs/ 顶层被工作产物淹没、防止影子 docs/contracts/。
  if (effMap.universal) {
    const reason = checkUniversal(rel, effMap.universal);
    if (reason) block(reason);
  }

  const agent = input.agent_type;
  if (!agent) process.exit(0); // 主线程 / 人类：per-agent 边界不约束（仍受上面的 universal + settings.deny）

  const agents = effMap.agents || {};
  // 大小写归一：scope-map 的 agent key 全小写；runtime 若传大小写变体（如 "Architect"）
  // 不能借此逃逸 per-agent 边界（gov-review-arch-change-autorun F3）。
  const agentKey = String(agent).toLowerCase();
  let policy = agents[agentKey];
  if (!policy) {
    // 带后缀的克隆体回退继承基础角色：builder-s3-quote -> builder
    policy = agents[agentKey.split('-')[0]];
  }
  if (!policy) process.exit(0); // 未登记的 agent：本钩子不强加任何约束

  if (matchesAny(rel, policy.deny)) {
    block(`scope: agent "${agent}" 不可写 ${rel}（被 .claude/scope-map.json 的 deny 命中）。`);
  }
  if ((policy.allow || []).length > 0 && !matchesAny(rel, policy.allow)) {
    block(
      `scope: agent "${agent}" 只能写 ${policy.allow.join(', ')}；${rel} 超出边界。`,
    );
  }
  process.exit(0);
}

// ── self-check（261015-gov-review-lite-fast-path T5 新建；argv 触发型，entry 集成臂——temp dir 假树 +
// spawn 本钩子喂构造 stdin，对偶 brief-inputs.mjs mkdtemp 先例）──
function buildFakeRepo() {
  const root = mkdtempSync(join(tmpdir(), 'scope-t5-'));
  const repo = join(root, 'repo'); // 主 checkout（.git 目录）
  const podx = join(root, 'trees', 'pod-x'); // 在册兄弟树（.git 文件）
  const outside = join(root, 'outside'); // 非在册路径
  mkdirSync(join(repo, '.git', 'worktrees', 'pod-x'), { recursive: true });
  mkdirSync(join(repo, '.claude'), { recursive: true });
  mkdirSync(join(repo, 'firmware'), { recursive: true });
  mkdirSync(join(podx, '.claude'), { recursive: true });
  mkdirSync(join(podx, 'firmware'), { recursive: true });
  mkdirSync(join(podx, 'docs', 'contracts'), { recursive: true });
  mkdirSync(outside, { recursive: true });
  const gitdirFile = join(repo, '.git', 'worktrees', 'pod-x', 'gitdir');
  writeFileSync(gitdirFile, `${join(podx, '.git')}\n`, 'utf8');
  writeFileSync(join(podx, '.git'), `gitdir: ${gitdirFile}\n`, 'utf8');
  const mapShape = {
    universal: { denyCreate: [], allowFiles: [], allowDirs: ['docs/contracts'], bashDenyWrite: ['docs/contracts/**', '.claude/**'] },
    agents: { builder: { allow: ['firmware/**'], deny: [] } },
  };
  writeFileSync(join(repo, '.claude', 'scope-map.json'), JSON.stringify(mapShape), 'utf8');
  writeFileSync(join(podx, '.claude', 'scope-map.json'), JSON.stringify(mapShape), 'utf8');
  return { root, repo, podx, outside };
}

function selfCheck() {
  const results = [];
  const ok = (name, cond, detail = '') => results.push({ name, ok: Boolean(cond), detail });
  const fx = buildFakeRepo();
  try {
    const run = (toolName, toolInput, extra = {}) => spawnSync(process.execPath, [scriptPath], {
      input: JSON.stringify({ tool_name: toolName, tool_input: toolInput, agent_type: extra.agent, cwd: extra.cwd || fx.repo }),
      env: { ...process.env, CLAUDE_PROJECT_DIR: fx.repo },
      encoding: 'utf8', timeout: 30000,
    });
    // 夹具非空先行（真空绿规约）
    ok('fixture-nonempty', existsSync(join(fx.podx, '.claude', 'scope-map.json'))
      && existsSync(join(fx.repo, '.git', 'worktrees', 'pod-x', 'gitdir'))
      && readFileSync(join(fx.podx, '.git'), 'utf8').includes('gitdir:'));
    // ① 在册兄弟树写工作面放行（builder → 兄弟树 firmware/，按该树 scope-map 的 allow 判）
    const arm1 = run('Write', { file_path: join(fx.podx, 'firmware', 'x.c') }, { agent: 'builder' });
    ok('sibling-worktree-workfile-allowed', arm1.status === 0, `exit=${arm1.status} ${String(arm1.stderr || '').slice(0, 120)}`);
    // ② 在册兄弟树写红线面拦截（Bash 重定向 → 兄弟树 docs/contracts/，按该树 bashDenyWrite 判——主线程同样拦，反向洞封堵）
    const arm2 = run('Bash', { command: `echo x > "${join(fx.podx, 'docs', 'contracts', 'y.md')}"` });
    ok('sibling-worktree-redline-blocked', arm2.status === 2 && String(arm2.stderr || '').includes('bashDenyWrite'),
      `exit=${arm2.status}`);
    // ③ 非在册路径行为不变：per-agent 写仓外路径仍拦（allow 失配）；主线程写仓外路径仍放行（universal 不命中）
    const arm3a = run('Write', { file_path: join(fx.outside, 'x.txt') }, { agent: 'builder' });
    ok('outside-path-agent-blocked-legacy', arm3a.status === 2, `exit=${arm3a.status}`);
    const arm3b = run('Write', { file_path: join(fx.outside, 'x.txt') });
    ok('outside-path-mainthread-allowed-legacy', arm3b.status === 0, `exit=${arm3b.status}`);
    // ④ 本树行为不变：builder 写本 checkout 工作面照旧放行
    const arm4 = run('Write', { file_path: join(fx.repo, 'firmware', 'y.c') }, { agent: 'builder' });
    ok('home-tree-behavior-unchanged', arm4.status === 0, `exit=${arm4.status}`);
  } finally {
    rmSync(fx.root, { recursive: true, force: true });
  }
  const allOk = results.every((r) => r.ok);
  process.stdout.write(`${JSON.stringify({ selfCheck: 'enforce-scope', ok: allOk, results }, null, 2)}\n`);
  process.exit(allOk ? 0 : 1);
}

if (process.argv.includes('--self-check')) selfCheck();
main();
