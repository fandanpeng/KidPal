#!/usr/bin/env node
// PostToolUse 钩子：当某次 Edit/Write/MultiEdit 触及 delivery/taskpackages/ 或 delivery/changes/dev/
//   （Pod 在 repo 侧的进展信号）时，触发 portfolio reconcile，保持共享 board 新鲜。
// 非阻塞：始终 exit 0；失败只打印告警，不打断会话（PostToolUse 不应拦截）。
//
// 关键事实（对齐 .claude/references/pod-coordination.md §4/§9）：
//   - hook 绝不直接写 board：它只调 reconcile()，board 单一逻辑写者仍是 portfolio-reconcile.mjs
//     （内部抢 advisory lock + 原子写）。多 Pod 会话并发触发时只有一个真正跑，其余幂等退出。
//   - Pod 状态文件（devloop/pods/<id>.json）由 Pod 显式 `--set-pod` 写（单写者），不在本钩子里推断。
//   - reconcile 用 fs/子进程写 git common dir，不经 Edit/Write 工具，不会递归触发本钩子。

import { readFileSync } from 'node:fs';
import { resolve, relative, sep } from 'node:path';
import { reconcile } from '../scripts/portfolio-reconcile.mjs';

function readStdin() {
  try {
    return readFileSync(0, 'utf8');
  } catch {
    return '';
  }
}

function main() {
  let input;
  try {
    input = JSON.parse(readStdin());
  } catch {
    process.exit(0); // 拿不到结构化输入就不干预
  }

  const filePath = input?.tool_input?.file_path;
  if (!filePath) process.exit(0);

  const projectDir = process.env.CLAUDE_PROJECT_DIR || input.cwd || process.cwd();
  const abs = resolve(input.cwd || projectDir, filePath);
  const rel = relative(projectDir, abs).split(sep).join('/');
  // 只在 Pod 进展信号（taskpackages / delivery/changes/dev）编辑后触发
  if (!rel.startsWith('delivery/taskpackages/') && !rel.startsWith('delivery/changes/dev/')) {
    process.exit(0);
  }

  try {
    const res = reconcile(projectDir);
    if (res && res.board && res.board.alerts && res.board.alerts.length) {
      process.stderr.write(
        `[portfolio-sync] 告警：${res.board.alerts.map((a) => `${a.kind}(${a.changeId})`).join('; ')}\n`,
      );
    }
  } catch (err) {
    process.stderr.write(`[portfolio-sync] 跳过（${String((err && err.message) || err)}）\n`);
  }
  process.exit(0);
}

main();
