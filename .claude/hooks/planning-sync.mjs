#!/usr/bin/env node
// PostToolUse 钩子：当某次 Edit/Write/MultiEdit 触及 .planning/ 时，触发 reconcile
//   —— done/abandoned 的 workstream 搬进 _archive/<year>/，并重建 INDEX.md / index.jsonl。
// 非阻塞：始终 exit 0；失败只打印告警，不打断会话（PostToolUse 不应拦截）。
//
// 关键事实：
//   - 钩子从 stdin 读取 Claude Code 的 JSON，含 tool_input.file_path。
//   - reconcile 用 fs 直接写文件（非 Edit/Write 工具），不会再触发本钩子，无递归。

import { readFileSync } from 'node:fs';
import { resolve, relative, sep } from 'node:path';
import { reconcile } from '../scripts/planning-index.mjs';

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
  if (!rel.startsWith('.planning/')) process.exit(0); // 只在 .planning/ 编辑后触发

  try {
    const res = reconcile(projectDir);
    if (res && Array.isArray(res.moves) && res.moves.length) {
      process.stderr.write(
        `[planning-sync] 已归档：${res.moves.map((m) => `${m.from} → ${m.to}`).join('; ')}\n`,
      );
    }
  } catch (err) {
    process.stderr.write(`[planning-sync] 跳过（${String((err && err.message) || err)}）\n`);
  }
  process.exit(0);
}

main();
