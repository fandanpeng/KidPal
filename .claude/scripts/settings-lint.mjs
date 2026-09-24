#!/usr/bin/env node
// settings-lint.mjs — settings 族 permissions 规则合法性静态检查（launch-prep 步骤 2 机检）
//
// 治理声明：owner=architect · approver=human · G3（launch 面机检；**不入 verify-gates 闸族**、不进任何既有闸链）。
// 批准物：delivery/changes/arch/260924-gov-review-settings-deny-contract-merge/PROPOSAL.md（FROZEN v0.4，T2-i；OQ-2=(a) human 裁定 2026-09-24）
//
// 病灶背景：`Write(path)` 文件权限规则被现行 CLI 判无效（文件权限检查只认 `Edit(path)` 规则，Edit 规则
//   覆盖所有文件编辑工具）——零执行力且硬阻塞 headless launch（exit 4，ARQ-260924-settings-write-deny-rule-cli-drift
//   实证，260924-perf-k1 launch 实测）；`claude doctor` 不检出该形态；CLI auto-update 常开，
//   本检 = 版本漂移的 launch 面前置防线（报错发生在 settings 加载期，与 launch 形态无关地打印）。
//
// 判定（只编码已实证口径，不发明 CLI 未声明的校验）：
//   permissions.{allow,ask,deny} 数组内规则串以 `Write(` 起头 → RED（已实证无效形态）
//   以 `Read(` 起头 → WARN（同族推断、未单独实证；advisory 不改退出码——PROPOSAL §6 留根 4）
//   JSON 解析失败 → RED。其余规则形态不判。
// 扫描面（机械枚举）：`.claude/settings.json` + `.claude/settings.local.json`（在场才扫）+ `.claude/*.settings.json`（glob）。
//   不扫 worktree 副本（launch-prep 在主 checkout 跑，Pod worktree 的 settings 随分支内容同源；分支期漂移为已声明接受残余，R1-N5）。
//
// 用法：
//   node .claude/scripts/settings-lint.mjs               # 扫描 settings 族
//   node .claude/scripts/settings-lint.mjs --self-check  # 夹具自检（判别性：反转检出逻辑必红）
//
// 退出码：0 = clean（WARN 允许）/ 1 = RED / 2 = 用法错。

import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { join, relative, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const projectDirOf = () => resolve(process.env.CLAUDE_PROJECT_DIR || process.cwd());

/** 枚举 settings 族文件（settings.json + settings.local.json（在场才扫）+ *.settings.json glob）。 */
export function settingsFamilyFiles(claudeDir) {
  const files = [];
  const push = (p) => { if (existsSync(p)) files.push(p); };
  push(join(claudeDir, 'settings.json'));
  push(join(claudeDir, 'settings.local.json'));
  if (existsSync(claudeDir)) {
    for (const name of readdirSync(claudeDir).sort()) {
      if (name.endsWith('.settings.json')) push(join(claudeDir, name));
    }
  }
  return [...new Set(files)];
}

/** 纯函数：lint 单份 settings 文本。返回 [{ line, rule, level, reason }]（self-check 可注入）。 */
export function lintText(text) {
  const findings = [];
  let doc;
  try {
    doc = JSON.parse(text);
  } catch (err) {
    findings.push({ line: 0, rule: '(parse)', level: 'RED', reason: `JSON 解析失败：${err.message}` });
    return findings;
  }
  const perms = doc && typeof doc === 'object' ? doc.permissions : null;
  if (!perms || typeof perms !== 'object') return findings;
  const lines = text.split('\n');
  for (const key of ['allow', 'ask', 'deny']) {
    const arr = perms[key];
    if (!Array.isArray(arr)) continue;
    for (const rule of arr) {
      if (typeof rule !== 'string') continue;
      const level = rule.startsWith('Write(') ? 'RED' : rule.startsWith('Read(') ? 'WARN' : null;
      if (!level) continue;
      // 行号 = 规则串字面（带引号）首次出现行（找不到记 0，仍报文件级命中）
      const needle = `"${rule}"`;
      const idx = lines.findIndex((l) => l.includes(needle));
      findings.push({
        line: idx >= 0 ? idx + 1 : 0,
        rule,
        level,
        reason:
          level === 'RED'
            ? 'Write(path) 文件权限规则被 CLI 判无效（文件权限检查只认 Edit(path) 规则）——零执行力且硬阻塞 headless launch exit 4（ARQ-260924）'
            : 'Read(path) 同族推断、未单独实证（advisory，不改退出码）',
      });
    }
  }
  return findings;
}

// ---- self-check（真空绿规约：夹具非空断言先行；判别性义务：坏夹具必须被检出、好夹具必须放行——反转检出逻辑必红）----
// 输出契约（FROZEN v0.4 T2-i / R1-F4）：夹具命名钉 bad-/good-（warn- 为 WARN 级夹具）前缀；逐例行含夹具名 + ok 标记；
// 尾行单行 JSON {"selfCheck":"settings-lint","pass":<n>,"fail":<m>,"total":<n+m>,"ok":<bool>}；任一例红 exit 1。
function selfCheck() {
  const cases = [];
  const mk = (name, text, expectRed, expectWarn) => {
    // 真空绿：夹具非空断言先行（非空且含 permissions 段，防「夹具没造对」的假绿）
    const fixtureNonEmpty = typeof text === 'string' && text.includes('permissions');
    const f = fixtureNonEmpty ? lintText(text) : [];
    const red = f.filter((x) => x.level === 'RED').length;
    const warn = f.filter((x) => x.level === 'WARN').length;
    const ok = fixtureNonEmpty && red === expectRed && warn === expectWarn;
    cases.push({ name, ok, got: `fixtureNonEmpty=${fixtureNonEmpty} red=${red} warn=${warn}（预期 red=${expectRed} warn=${expectWarn}）` });
  };
  // 坏夹具（bad- 前缀）：检出为 RED——删掉 lintText 的 Write( 判定分支，以下两例立即 fail
  mk('bad-write-deny', '{"permissions":{"deny":["Edit(./docs/contracts/**)","Write(./docs/contracts/**)"]}}', 1, 0);
  mk('bad-write-allow', '{"permissions":{"allow":["Write(./x/**)","Bash(git status)"]}}', 1, 0);
  mk('bad-json-unparsable', '{"permissions":', 1, 0);
  // 好夹具（good- 前缀）：放行——把判定扩到 Edit(/Bash( 即误伤翻红
  mk('good-edit-only-deny', '{"permissions":{"deny":["Edit(./docs/contracts/**)"],"ask":["Bash(git merge:*)"],"allow":["mcp__codebase-memory-mcp__*"]}}', 0, 0);
  mk('good-permissions-no-arrays', '{"permissions":{},"hooks":{}}', 0, 0);
  // WARN 级夹具（warn- 前缀）：Read( 同族推断降级、不改退出码
  mk('warn-read-deny', '{"permissions":{"deny":["Read(./x/**)"]}}', 0, 1);

  const fail = cases.filter((c) => !c.ok);
  for (const c of cases) process.stdout.write(`${c.ok ? 'ok' : 'FAIL'} ${c.name} — ${c.got}\n`);
  process.stdout.write(
    `${JSON.stringify({ selfCheck: 'settings-lint', pass: cases.length - fail.length, fail: fail.length, total: cases.length, ok: fail.length === 0 })}\n`,
  );
  process.exit(fail.length ? 1 : 0);
}

// ---- 主跑 ----
function main() {
  const args = process.argv.slice(2);
  if (args.includes('--self-check')) selfCheck();
  const unknown = args.filter((a) => a !== '--self-check');
  if (unknown.length > 0) {
    process.stderr.write(`[settings-lint] FATAL: 未知参数 ${unknown.join(' ')}（用法：settings-lint.mjs [--self-check]）\n`);
    process.exit(2);
  }

  const dir = projectDirOf();
  const claudeDir = join(dir, '.claude');
  const files = settingsFamilyFiles(claudeDir);
  if (files.length === 0) {
    process.stderr.write(`[settings-lint] FATAL: settings 族零文件（${claudeDir} 下无 settings.json / *.settings.json）——环境异常\n`);
    process.exit(2);
  }

  let red = 0;
  let warn = 0;
  for (const f of files) {
    const rel = relative(dir, f).split('\\').join('/');
    let text;
    try {
      text = readFileSync(f, 'utf8');
    } catch (err) {
      process.stdout.write(`RED ${rel}:0 (read) — 读取失败：${err.message}\n`);
      red += 1;
      continue;
    }
    for (const x of lintText(text)) {
      process.stdout.write(`${x.level} ${rel}:${x.line} ${x.rule} — ${x.reason}\n`);
      if (x.level === 'RED') red += 1;
      else warn += 1;
    }
  }
  process.stdout.write(`settings-lint: ${files.length} files, ${red} RED, ${warn} WARN\n`);
  process.exit(red > 0 ? 1 : 0);
}

// import.meta 守卫（与 pre-merge-escalation-gate.mjs T6 同形：被 import 时不执行 CLI）
function isDirectRun() {
  try {
    return !!process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href;
  } catch {
    return false;
  }
}

if (isDirectRun()) main();
