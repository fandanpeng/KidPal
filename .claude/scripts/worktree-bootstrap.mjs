#!/usr/bin/env node
// worktree env bootstrap（KidPal 档 3：Pod worktree 冷启动等价化）。
//
// 一行调用完成新 worktree 冷启动的确定性动作（本仓化，源仓 pnpm/prisma 三步在此映射为）：
//   ① 从主 checkout 镜像 gitignored `.env`（根级 + `server/shim/.env`）到当前 worktree——
//      git worktree 不迁移 gitignored 文件；目标已存在则跳过不覆盖（密钥文件 safety-first）。
//   ② 固件面配置探测：若 worktree 内有 sdkconfig 缺失而主 checkout 有，则镜像之（板级/特性配置，
//      ESP-IDF 构建前置）；sdkconfig.defaults 属版本控制不镜像。
//   ③ shim 面 Python 依赖安装：`python -m pip install -r server/shim/requirements.txt`
//      （仅当该文件存在；ESP-IDF 固件构建 idf.py 依赖系统级环境，不在此引导）。
//
// 非闸、不常驻：只在新 worktree 冷启动时由 launch 方 / planner 在 worktree 目录内调用一次。
// codebase-memory MCP 索引新建、board worktreePath 预填属 launch 编排面，不进本脚本。
//   图索引引导半自动（对齐源仓 260827-gov-review-worktree-graph-index-bootstrap T4）：本仓一旦 commit
//   `.codebase-memory/graph.db.zst` 快照（`index_repository(persistence=true)`），worktree 在 bootstrap 后跑一次
//   索引即从快照导入 + 增量补差（近零成本），root_path 由工具自动重写为 worktree 路径；本仓尚未 commit 快照时，
//   工作树索引仍由 launch 面同名建（项目名 = 目录名保障，缺则图工具 project not found）。
//
// 用法：node .claude/scripts/worktree-bootstrap.mjs [--source <主checkout路径>] [--dry-run] [--help]

import { existsSync, copyFileSync, mkdirSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { execSync, spawnSync } from 'node:child_process';

// gitignored 环境/配置文件候选（存在才镜像；目标已存在跳过）。
const ENV_CANDIDATES = ['.env', 'server/shim/.env', 'sdkconfig'];
// shim 依赖清单（存在才跑 pip install）。
const SHIM_REQUIREMENTS = 'server/shim/requirements.txt';

function usage() {
  console.log(`worktree-bootstrap — 新 worktree 冷启动等价化（在当前 worktree 目录内运行）

用法: node .claude/scripts/worktree-bootstrap.mjs [选项]

动作（确定性）:
  1. 从主 checkout 镜像 gitignored 配置（${ENV_CANDIDATES.join('、')}）；目标已存在则跳过不覆盖
  2. 若 ${SHIM_REQUIREMENTS} 存在 → python -m pip install -r（shim 面依赖）
     （ESP-IDF 固件构建 idf.py 用系统级环境，不在此引导）

选项:
  --source <path>  主 checkout 路径（默认：git rev-parse --git-common-dir 的父目录）
  --dry-run        只打印将执行的动作，不落盘不跑安装
  --help           打印本帮助并退出

退出码: 0 成功；1 参数/探测失败；2 依赖安装失败。
注: MCP 索引新建与 board worktreePath 预填不属本脚本（launch 编排面人工步骤）。`);
}

function parseArgs(argv) {
  const opts = { source: null, dryRun: false, help: false };
  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    if (arg === '--help' || arg === '-h') opts.help = true;
    else if (arg === '--dry-run') opts.dryRun = true;
    else if (arg === '--source') opts.source = argv[++i];
    else {
      console.error(`未知参数: ${arg}`);
      usage();
      process.exit(1);
    }
  }
  if (opts.source !== null && typeof opts.source !== 'string') {
    console.error('--source 需要值');
    process.exit(1);
  }
  return opts;
}

function detectMainCheckout(worktreeRoot) {
  try {
    const raw = execSync('git rev-parse --git-common-dir', { cwd: worktreeRoot, encoding: 'utf8' }).trim();
    return dirname(resolve(worktreeRoot, raw));
  } catch {
    return null;
  }
}

function stepMirrorEnv(sourceRoot, worktreeRoot, dryRun) {
  console.log(`[1/2] 配置镜像：${sourceRoot} → ${worktreeRoot}`);
  let mirrored = 0;
  for (const rel of ENV_CANDIDATES) {
    const src = join(sourceRoot, rel);
    const dst = join(worktreeRoot, rel);
    if (!existsSync(src)) {
      console.log(`  跳过 ${rel}：源不存在`);
      continue;
    }
    if (existsSync(dst)) {
      console.log(`  跳过 ${rel}：目标已存在（不覆盖；如需强刷先手工删除）`);
      continue;
    }
    if (dryRun) {
      console.log(`  [dry-run] 将镜像 ${rel}`);
      continue;
    }
    mkdirSync(dirname(dst), { recursive: true });
    copyFileSync(src, dst);
    mirrored += 1;
    console.log(`  已镜像 ${rel}`);
  }
  return mirrored;
}

function runStep(label, cmd, args, cwd, dryRun) {
  console.log(`[${label}] ${cmd} ${args.join(' ')}`);
  if (dryRun) {
    console.log('  [dry-run] 跳过执行');
    return 0;
  }
  const result = spawnSync(cmd, args, {
    cwd,
    stdio: 'inherit',
    shell: process.platform === 'win32',
  });
  if (result.status !== 0) {
    console.error(`${cmd} ${args.join(' ')} 失败（exit ${result.status ?? 'null'}）`);
    return 2;
  }
  return 0;
}

const opts = parseArgs(process.argv.slice(2));
if (opts.help) {
  usage();
  process.exit(0);
}

const worktreeRoot = process.cwd();
const sourceRoot = opts.source ? resolve(process.cwd(), opts.source) : detectMainCheckout(worktreeRoot);
if (!sourceRoot) {
  console.error('无法探测主 checkout（git rev-parse --git-common-dir 失败）；用 --source <主checkout路径> 显式指定');
  process.exit(1);
}

stepMirrorEnv(sourceRoot, worktreeRoot, opts.dryRun);

// shim 面依赖安装（仅当清单存在）。
const shimReq = join(worktreeRoot, SHIM_REQUIREMENTS);
if (existsSync(shimReq)) {
  const pipExit = runStep('2/2', 'python', ['-m', 'pip', 'install', '-r', SHIM_REQUIREMENTS], worktreeRoot, opts.dryRun);
  if (pipExit !== 0) process.exit(pipExit);
} else {
  console.log(`[2/2] 跳过 pip install：${SHIM_REQUIREMENTS} 不存在（shim 面未建）`);
}

console.log(opts.dryRun ? 'dry-run 完成：未落盘、未执行安装。' : 'worktree bootstrap 完成：配置镜像 + shim 依赖就绪。');
