#!/usr/bin/env node
// Blocks direct pushes to main/master. Merge automation must go through PR/revertable flow.
//
// ARQ-260730-deny-main-push-implicit-ref-bypass（gov-review-deny-main-push-hardening，FROZEN v1.4）：
// 从 crude 整串匹配重写为「切分命令段 + 识别真 git push + 按实际目标（refspec / 当前分支）判定 + fail-safe 向严」。
// 闭合假阴（origin HEAD / --all / --mirror / 复合命令 / git 全局选项 -C/-c / 命令前缀 sudo·env / 分组 () /
//   命令替换 $(…) / 反斜杠续行 / 大小写 GIT push）与假阳（commit message 含 git push+main 字样、引号内操作符、
//   feature/main、feature:+feature2、echo "git push main"）。
//
// 实现顺序铁律（D，承重）：
//   ① 宽子串快速出口在前（命令无 git 子串 或 无 push 子串 → 放行；宁宽勿漏，精度后置）
//   ② R-3 命令替换哨兵在后、仅作用 git+push 相关命令（且可能藏 push → BLOCK）
//   ③ 精度全部后置到 code-token 阶段（引号内 = 数据）
// fail-safe 向严（铁律）：认不出即 BLOCK——解析歧义 / 当前分支不可得 / shell 包装内层不可解析 / 命令替换不可解析 /
//   递归超限 → 一律 BLOCK（绝无 fail 向放行分支）；命名的非 main 分支推送 + --dry-run 恒放行（不假阳）。
// 大小写规则（M-6，Windows 实证）：git 程序名 / push 子命令 / 快速出口 / 歧义检测一律大小写不敏感
//   （GIT/Git/GIT.EXE push 真实执行 push）；唯 refspec 目标与 main/master 相等比较大小写敏感（MAIN/Main 是不同分支→放行）。
// 启发式边界（诚实，对偶 enforce-scope bashDenyWrite 注）：抬高门槛非完整 shell 沙箱；${…} 参数展开
//   （git${IFS}push 混淆）不在覆盖（N-3 follow-up，现行钩子同漏）。
//
// owner=architect, approver=human (G3 hook, see DEV-GOVERNANCE.md §6)

import { readFileSync } from 'node:fs';
import { execFileSync } from 'node:child_process';

function readStdin() {
  try {
    return readFileSync(0, 'utf8');
  } catch {
    return '';
  }
}

function block(reason) {
  process.stderr.write(reason);
  process.exit(2);
}

// ---- 工具函数 ----

function stripQuotes(s) {
  let t = String(s);
  if (t.length >= 2 && ((t[0] === '"' && t[t.length - 1] === '"') || (t[0] === "'" && t[t.length - 1] === "'"))) {
    t = t.slice(1, -1);
  }
  return t;
}

// 取路径 basename，剥 .exe 后缀（GIT.EXE → GIT），供大小写不敏感比较。
function baseName(p) {
  const s = String(p).replace(/\\/g, '/');
  const parts = s.split('/');
  let last = parts[parts.length - 1] || '';
  last = last.replace(/\.exe$/i, '');
  return last;
}

// 默认 getCurrentBranch：git -C <cwd> rev-parse --abbrev-ref HEAD。失败 → null（fail-safe 向严）。
function defaultGetCurrentBranch(cwd) {
  try {
    const args = cwd ? ['-C', cwd, 'rev-parse', '--abbrev-ref', 'HEAD'] : ['rev-parse', '--abbrev-ref', 'HEAD'];
    return execFileSync('git', args, { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'], timeout: 15000 }).trim();
  } catch {
    return null;
  }
}

// ---- 引号感知 + 续行归一 切分命令段（按 && || ; | & 换行；引号内不切；单引号外 \<换行> 归一为空格 M-4）----
function splitCommandSegments(cmd) {
  const segments = [];
  let cur = '';
  let quote = null; // null | '"' | "'"
  let i = 0;
  const flush = () => {
    const s = cur.trim();
    if (s) segments.push(s);
    cur = '';
  };
  while (i < cmd.length) {
    const ch = cmd[i];
    if (quote) {
      cur += ch;
      if (ch === quote) quote = null;
      i++;
      continue;
    }
    if (ch === '"' || ch === "'") {
      quote = ch;
      cur += ch;
      i++;
      continue;
    }
    if (ch === '\\' && cmd[i + 1] === '\n') {
      // M-4：单引号外反斜杠续行 → 归一为空格（防 `git push origin \<换行>main` 被切断绕过）
      cur += ' ';
      i += 2;
      continue;
    }
    if (ch === '\n' || ch === ';') {
      flush();
      i++;
      continue;
    }
    if (ch === '&' && cmd[i + 1] === '&') {
      flush();
      i += 2;
      continue;
    }
    if (ch === '|' && cmd[i + 1] === '|') {
      flush();
      i += 2;
      continue;
    }
    if (ch === '|') {
      flush();
      i++;
      continue;
    }
    // 注：单个 & 不切（避免拆 2>&1 重定向）；后台 `cmd &` 罕见，非沙箱边界。
    cur += ch;
    i++;
  }
  flush();
  return segments;
}

// ---- code-token 化：按空白 + (){}（分组符，M-2）切 token；引号内 = 单一数据 token（M-3）----
function tokenize(seg) {
  const tokens = [];
  let cur = '';
  let quote = null;
  let i = 0;
  const flush = () => {
    if (cur !== '') {
      tokens.push(cur);
      cur = '';
    }
  };
  while (i < seg.length) {
    const ch = seg[i];
    if (quote) {
      cur += ch;
      if (ch === quote) quote = null;
      i++;
      continue;
    }
    if (ch === '"' || ch === "'") {
      quote = ch;
      cur += ch;
      i++;
      continue;
    }
    if (ch === '\\' && seg[i + 1] === '\n') {
      cur += ' ';
      i += 2;
      continue;
    }
    if (/\s/.test(ch) || ch === '(' || ch === ')' || ch === '{' || ch === '}') {
      // 空白 + 分组符 ()/{} 为分隔符（M-2：剥 (git push origin main) 首尾分组符）
      flush();
      i++;
      continue;
    }
    cur += ch;
    i++;
  }
  flush();
  return tokens;
}

// 是否存在独立的 git token（basename==git）+ 独立 push token（用于歧义兜底 R-5，区别于子串）。
function hasSeparateGitPushTokens(tokens) {
  let hasGit = false;
  let hasPush = false;
  for (const t of tokens) {
    if (baseName(t.replace(/^\\/, '')).toLowerCase() === 'git') hasGit = true;
    if (t.toLowerCase() === 'push') hasPush = true;
  }
  return hasGit && hasPush;
}

// ---- refspec 目标是否 == main/master（R-4 + M-1：先剥强推前缀 +，再剥 refs/heads/，全名相等，大小写敏感 M-6）----
function refspecTargetsMain(refspec) {
  let r = String(refspec);
  if (r.startsWith('+')) r = r.slice(1); // M-1：强推前缀 +main / +src:dst
  const colon = r.indexOf(':');
  let dest = colon >= 0 ? r.slice(colon + 1) : r; // src:dst 取 dst；裸分支目标即自身；:dst 取 dst（删远程分支）
  dest = dest.replace(/^refs\/heads\//, '');
  if (dest.startsWith('+')) dest = dest.slice(1); // 防御性：dst 侧 +
  return dest === 'main' || dest === 'master'; // M-6：大小写敏感（MAIN/Main 是不同分支 → 不命中）
}

const PREFIX_WHITELIST = new Set(['sudo', 'env', 'command', 'exec', 'nohup', 'time', 'builtin', 'stdbuf', 'nice', 'ionice']);
const SUDO_VALUE_OPTS = new Set(['-u', '-g', '-U', '-C', '-r', '-t', '-h', '-p', '-a', '-F', '-l']);
const SHELLS = new Set(['bash', 'sh', 'zsh', 'dash', 'ksh', 'ash']);

// 剥段首命令前缀白名单（sudo/env/command/exec/…）+ 其选项；返回净命令起始下标。
function stripPrefixes(tokens) {
  let idx = 0;
  while (idx < tokens.length) {
    const raw = tokens[idx].replace(/^\\/, '');
    const base = raw.toLowerCase();
    if (!PREFIX_WHITELIST.has(base)) break;
    idx++;
    if (base === 'env') {
      // env [VAR=val …] cmd —— 跳过 VAR=val
      while (idx < tokens.length && /^[A-Za-z_]\w*=/.test(tokens[idx])) idx++;
    } else if (base === 'sudo') {
      // sudo [-opts] [-u user …] cmd —— 跳过选项与其值
      let expectValue = false;
      while (idx < tokens.length) {
        const tk = tokens[idx];
        if (expectValue) {
          expectValue = false;
          idx++;
          continue;
        }
        if (tk.startsWith('-')) {
          if (SUDO_VALUE_OPTS.has(tk) && !tk.includes('=')) expectValue = true;
          idx++;
          continue;
        }
        break;
      }
    }
    // command/exec/nohup/time/builtin 等：仅剥前缀本身（选项罕见）
  }
  return idx;
}

// ---- fail-closed refspec 白名单（landing-verifier 四复验终局裁断 · 选项 B · human 批准）----
// 干净 git-refname 字符集（字母数字 + . _ - / + refspec 语法 + : @）。refspec 含任何 shell 元字符
// （$ ' " \ { } , [ ] ? * ~ ` 等，即非此字符集）→ 解析期不可定值 → 向严 BLOCK。
// 黑名单（认不出→放）对 shell 展开族（quote/dollar/ANSI-C/brace/glob/tilde，开放无穷）不可终止——landing-verifier
// 四轮实证每轮闭一形下一形即现（"main"→$'main'→$'\x6dain'→{main,foo}→mai[n]…）；白名单（认不出→拦）可终止，
// 一次永闭全族，非完整 shell 解析器（只是 refname 白名单）。代价 = 过拦 "feature-x"/$'feature-x'/Windows
// 反斜杠 remote（承重红线可接受：漏比误危险，agent 用裸 feature-x/正斜杠路径；对偶已接受 N-1）。
const REFSPEC_ALLOWED = /^[A-Za-z0-9._/+:@-]+$/;

// ---- git push 段目标判定（fail-safe 向严 · fail-closed 白名单）----
function judgeGitPush(pushArgs, cwd, getCurrentBranch) {
  if (pushArgs.includes('--dry-run')) return { block: false }; // dry-run 恒放行
  if (pushArgs.some((a) => a === '--all' || a === '--mirror')) {
    return { block: true, reason: 'git push --all/--mirror may carry main/master' };
  }
  const hasTags = pushArgs.includes('--tags');

  // positional（非 flag）参；-u/--set-upstream 的后续 remote+branch 仍当 positional。
  const positionals = pushArgs.filter((a) => !a.startsWith('-'));

  let needBranchCheck = false;
  for (const pRaw of positionals) {
    // fail-closed 白名单：含任何 shell 元字符（非干净 git-refname）→ 解析期不可定值 → 向严 BLOCK。
    if (!REFSPEC_ALLOWED.test(pRaw)) {
      return { block: true, reason: `git push refspec "${pRaw}" contains shell metacharacters (value undetermined at parse time, fail-safe toward block)` };
    }
    // 干净 refspec：剥强推前缀 +，判 HEAD/@（当前分支）或目标 main/master。
    const r = pRaw.startsWith('+') ? pRaw.slice(1) : pRaw;
    if (r === 'HEAD' || r === '@') {
      needBranchCheck = true; // HEAD/@ = 当前分支
      continue;
    }
    if (refspecTargetsMain(pRaw)) {
      return { block: true, reason: `git push targets main/master (refspec "${pRaw}")` };
    }
  }

  const onlyRemote = positionals.length === 1 && !positionals[0].includes(':') && positionals[0] !== 'HEAD' && positionals[0] !== '@';
  const implicit = needBranchCheck || positionals.length === 0 || onlyRemote;

  if (implicit) {
    // 纯 --tags（无分支 / HEAD refspec）→ 只推 tag，不查分支（放行）
    if (hasTags && !needBranchCheck && (positionals.length === 0 || onlyRemote)) return { block: false };
    const branch = getCurrentBranch(cwd);
    if (branch === null) {
      return { block: true, reason: 'git push (implicit current branch) — cannot determine branch (fail-safe toward block)' };
    }
    if (branch === 'main' || branch === 'master') {
      return { block: true, reason: `git push while on ${branch} (implicit current-branch push)` };
    }
    return { block: false };
  }

  // 有明确非 main 干净 refspec（git push origin feature-x / feature/branch / feature:+feature2 / MAIN）→ 放行
  return { block: false };
}

// ---- 段分析（识别 git push / shell 包装递归 / cwd 跟踪 / 歧义兜底）----
function analyzeSegment(seg, ctx) {
  const tokens = tokenize(seg);
  if (tokens.length === 0) return { block: false };

  // cwd 跟踪：cd <dir> / pushd <dir>
  const firstLower = tokens[0].replace(/^\\/, '').toLowerCase();
  if ((firstLower === 'cd' || firstLower === 'pushd') && tokens.length >= 2) {
    return { block: false, newCwd: stripQuotes(tokens[1]) };
  }

  const start = stripPrefixes(tokens);
  if (start >= tokens.length) return { block: false };

  const progBase = baseName(tokens[start].replace(/^\\/, '')).toLowerCase();

  // shell 包装递归：bash -c <str> / sh -c <str> / eval <str>（防 bash -c "git push…" 绕过）
  if (SHELLS.has(progBase)) {
    const rest = tokens.slice(start + 1);
    const cIdx = rest.indexOf('-c');
    if (cIdx >= 0 && cIdx + 1 < rest.length) {
      const inner = stripQuotes(rest[cIdx + 1]);
      return analyzeCommand(inner, ctx.getCurrentBranch, ctx.depth + 1);
    }
    return { block: false }; // shell 无 -c，非内联命令
  }
  if (progBase === 'eval') {
    const inner = tokens.slice(start + 1).map(stripQuotes).join(' ');
    return analyzeCommand(inner, ctx.getCurrentBranch, ctx.depth + 1);
  }

  // 识别 git（basename，大小写不敏感 M-6）
  if (progBase !== 'git') {
    // 非 git 命令。歧义兜底（R-5）：若存在独立 git + push token（非子串）→ fail-safe BLOCK。
    if (hasSeparateGitPushTokens(tokens.slice(start))) {
      return { block: true, reason: 'ambiguous git+push command structure (fail-safe toward block)' };
    }
    return { block: false }; // recognized 非 git 命令（echo/cat/make/…）
  }

  // 解析 git 全局选项（R-1 去相邻化），捕获 -C <dir>，定位 push 子命令
  let idx = start + 1;
  let cwdFromGitC = null;
  let subcommand = null;
  while (idx < tokens.length) {
    const t = tokens[idx];
    if (t === '-C' && idx + 1 < tokens.length) {
      cwdFromGitC = stripQuotes(tokens[idx + 1]);
      idx += 2;
      continue;
    }
    if (t === '-c' || t === '--git-dir' || t === '--work-tree' || t === '--namespace' || t === '--super-prefix' || t === '--exec-path') {
      idx += 2; // 选项 + 独立值
      continue;
    }
    if (t.startsWith('-')) {
      idx += 1; // --git-dir=x / -c k=v / 布尔全局选项
      continue;
    }
    subcommand = t.toLowerCase();
    idx++;
    break;
  }

  if (subcommand !== 'push') {
    // git 非 push 子命令（fetch/commit/status/…）→ recognized 非 push → 放行
    return { block: false };
  }

  const pushArgs = tokens.slice(idx);
  const cwd = cwdFromGitC || ctx.cwdHint;
  return judgeGitPush(pushArgs, cwd, ctx.getCurrentBranch);
}

// ---- 主分析（递归，依赖注入 getCurrentBranch 供自检桩替）----
function analyzeCommand(command, getCurrentBranch = defaultGetCurrentBranch, depth = 0) {
  if (depth > 8) return { block: true, reason: 'recursion depth exceeded (fail-safe toward block)' }; // 递归上限 8，超限向严
  const cmd = String(command || '');

  // 步骤 0：宽子串快速出口（在前，R-5）。无 git 子串 或 无 push 子串（大小写不敏感）→ 放行。
  const lower = cmd.toLowerCase();
  if (!lower.includes('git') || !lower.includes('push')) return { block: false };

  // 步骤 1：引号感知 + 续行归一 切分命令段。
  // R-3 命令替换哨兵**下沉到 git push 段判定**（judgeGitPush：push 参含命令替换 → 目标运行时未知 → BLOCK）；
  // 程序位替换 `$(which git) push` 由 R-5 歧义兜底捕获（() 为分隔符暴露 git/push token）。
  // 不做 command-level R-3——会过拦「提及 git push 的非 push 命令带命令替换」，
  // 如 `git commit -m "$(cat <<EOF …git push… EOF)"`（commit 非 push 段，应放行）。
  const segments = splitCommandSegments(cmd);
  let cwdHint = null;
  for (const seg of segments) {
    const r = analyzeSegment(seg, { getCurrentBranch, cwdHint, depth });
    if (r.newCwd !== undefined) cwdHint = r.newCwd;
    if (r.block) return { block: true, reason: r.reason };
  }
  return { block: false };
}

// ---- ARQ-114 结构化自检 ----
function runSelfCheck() {
  const cases = [
    // 假阴（断言整命令 BLOCK）
    { cmd: 'git push origin HEAD', branch: 'main', expectBlock: true },
    { cmd: 'git push --all', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push --mirror', branch: 'feature-x', expectBlock: true },
    { cmd: 'cd x && git push origin HEAD', branch: 'main', expectBlock: true },
    { cmd: 'git push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin HEAD:main', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin local:master', branch: 'feature-x', expectBlock: true },
    { cmd: 'bash -c "git push origin main"', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push', branch: 'master', expectBlock: true },
    // FN-1 git 全局选项
    { cmd: 'git -C /path push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: 'git -c user=x push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: 'git --git-dir=/x push origin main', branch: 'feature-x', expectBlock: true },
    // FN-2 命令前缀
    { cmd: 'sudo git push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: 'env git push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: 'command git push origin main', branch: 'feature-x', expectBlock: true },
    // FN-3 分组
    { cmd: '(git push origin main)', branch: 'feature-x', expectBlock: true },
    { cmd: '((git push origin main))', branch: 'feature-x', expectBlock: true },
    // FN-4 git 非段首 / 命令替换
    { cmd: '/usr/bin/git push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: '$(which git) push origin main', branch: 'feature-x', expectBlock: true }, // R-5 歧义兜底
    { cmd: 'git push origin $(get_branch)', branch: 'feature-x', expectBlock: true }, // R-3 精准化：push 参动态替换，目标未知
    { cmd: 'git push origin "$(echo main)"', branch: 'feature-x', expectBlock: true }, // R-3：引号内替换仍含 $(
    // M-1 强推前缀 +
    { cmd: 'git push origin +main', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin feature:+main', branch: 'feature-x', expectBlock: true },
    // M-4 反斜杠续行
    { cmd: 'git push origin \\\nmain', branch: 'feature-x', expectBlock: true },
    // M-6 大小写（Windows 回归）
    { cmd: 'GIT push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: 'Git push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: 'GIT.EXE push origin main', branch: 'feature-x', expectBlock: true },
    { cmd: 'sudo GIT push origin main', branch: 'feature-x', expectBlock: true },
    // quoted-refspec 活假阴（landing-verifier 实测：shell 剥引号后 git 收到 main，日常给分支加引号非混淆）
    { cmd: 'git push origin "main"', branch: 'feature-x', expectBlock: true },
    { cmd: "git push origin 'main'", branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin ma""in', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin "ma"in', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin +"main"', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin HEAD:"main"', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin "refs/heads/main"', branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin "HEAD"', branch: 'main', expectBlock: true },
    // bash dollar-quoting 活假阴（landing-verifier 复验实测：$'…'/$"…" shell 解析皆 main）
    { cmd: "git push origin $'main'", branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin $"main"', branch: 'feature-x', expectBlock: true },
    { cmd: "git push origin $'ma'$'in'", branch: 'feature-x', expectBlock: true },
    { cmd: 'git push origin $BRANCH', branch: 'feature-x', expectBlock: true }, // 变量展开，目标运行时未知 → 向严
    // ANSI-C 转义 dollar-quote 活假阴（landing-verifier 三复验：$'\xHH'/$'\nnn'/$'\uHHHH' shell 求值皆 main，归一不可定 → 向严 BLOCK）
    { cmd: "git push origin $'\\x6dain'", branch: 'feature-x', expectBlock: true }, // hex
    { cmd: "git push origin $'\\155ain'", branch: 'feature-x', expectBlock: true }, // octal
    { cmd: "git push origin $'m\\x61in'", branch: 'feature-x', expectBlock: true }, // 混合
    // brace expansion / glob 活假阴（landing-verifier 四复验终局：bash 在 git 前展开，白名单含 , [ ] * ? ~ 元字符即拦，一次永闭）
    { cmd: 'git push origin {main,foo}', branch: 'feature-x', expectBlock: true }, // brace 展开（含 , 元字符）
    { cmd: 'git push origin {x,main}', branch: 'feature-x', expectBlock: true }, // brace 展开
    { cmd: 'git push origin mai[n]', branch: 'feature-x', expectBlock: true }, // glob [ ]
    { cmd: 'git push origin m*', branch: 'feature-x', expectBlock: true }, // glob *
    // fail-safe：分支不可得 → BLOCK
    { cmd: 'git push', branch: null, expectBlock: true },
    // 假阳（断言整命令放行）
    { cmd: 'git commit -m "git push origin main was blocked"', branch: 'main', expectBlock: false },
    { cmd: 'echo "git push main"', branch: 'main', expectBlock: false },
    { cmd: 'git push origin feature-x', branch: 'main', expectBlock: false },
    { cmd: 'git push --dry-run origin main', branch: 'main', expectBlock: false },
    { cmd: 'git push origin HEAD', branch: 'feature-x', expectBlock: false },
    { cmd: 'git push -u origin feature-y', branch: 'main', expectBlock: false },
    { cmd: 'git commit -m "x; git push origin main"', branch: 'main', expectBlock: false }, // FP-1 引号感知
    { cmd: 'git push origin feature/main', branch: 'main', expectBlock: false }, // FP-2 全名相等
    { cmd: 'git push origin master-thesis', branch: 'main', expectBlock: false }, // FP-2
    { cmd: 'git push origin feature:+feature2', branch: 'main', expectBlock: false }, // M-1 对照
    { cmd: 'git push origin \\\nfeature-x', branch: 'main', expectBlock: false }, // M-4 对照（续行 feature）
    { cmd: 'cd $(git rev-parse --show-toplevel)', branch: 'main', expectBlock: false }, // M-5（无 push 子串，快速出口放行）
    { cmd: 'echo $(date)', branch: 'main', expectBlock: false }, // M-5
    { cmd: 'ls $(pwd)', branch: 'main', expectBlock: false }, // M-5
    { cmd: 'git commit -m "$(echo git push origin main)"', branch: 'main', expectBlock: false }, // R-3 精准化：commit 非 push 段，message 内替换不拦
    { cmd: 'git push origin "feature-x"', branch: 'main', expectBlock: true }, // 选项 B fail-closed：含 " 元字符 → 向严 BLOCK（过拦承重红线可接受）
    { cmd: 'git push origin "HEAD"', branch: 'feature-x', expectBlock: true }, // 选项 B：含 " → BLOCK（不再区分引号 HEAD）
    { cmd: "git push origin $'feature-x'", branch: 'main', expectBlock: true }, // 选项 B：含 $ 元字符 → 向严 BLOCK（过拦）
    { cmd: 'git push origin feature/branch', branch: 'main', expectBlock: false }, // 正斜杠分支名（/ 在 refname 白名单）→ 放行
    { cmd: 'git push origin MAIN', branch: 'main', expectBlock: false }, // M-6 分支相等大小写敏感
    { cmd: 'cat git-push-docs.md', branch: 'main', expectBlock: false }, // 顺序 D：cat 非 git，无独立 git/push token
  ];

  let pass = 0;
  let fail = 0;
  const failures = [];
  for (const c of cases) {
    const r = analyzeCommand(c.cmd, () => c.branch);
    if (r.block === c.expectBlock) {
      pass++;
    } else {
      fail++;
      failures.push({ cmd: c.cmd, expect: c.expectBlock, got: r.block, reason: r.reason });
    }
  }
  const ok = fail === 0;
  process.stdout.write(
    `${JSON.stringify({ selfCheck: 'deny-main-push', pass, fail, total: cases.length, ok, failures: ok ? undefined : failures })}\n`,
  );
  if (!ok) process.exitCode = 1;
}

// ---- 入口 ----
if (process.argv.includes('--self-check')) {
  runSelfCheck();
} else {
  let input;
  try {
    input = JSON.parse(readStdin() || '{}');
  } catch {
    process.exit(0);
  }
  const command = String(input?.tool_input?.command || input?.tool_input?.cmd || '').trim();
  const result = analyzeCommand(command);
  if (result.block) {
    block(
      `merge-gate: ${result.reason}. Direct git push to main/master is denied — prepare a PR/revertable merge path and require supervisor approval or human escalation.`,
    );
  }
  process.exit(0);
}
