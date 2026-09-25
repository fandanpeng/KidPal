#!/usr/bin/env node
// current/ 指针解析回显齿单源（260924-gov-review-gate-parse-teeth T6，Track B FROZEN v0.4 落地）。
//
// ARQ-260913-current-pointer-implicit-global 残余①机械齿：经 delivery/taskpackages/current/ 解析
// 包上下文的脚本群，在指针指错时静默按错包跑（三包历史实证：九闸全绿跑错包 / 簿记写入上一批 /
// 投影不刷静默假绿）。dispatch-stamp 已有回显齿（`resolveChangeIdWithSource`，260918 T2 落地）、
// verifier-evidence-check 已有 mismatch fail-loud 齿（bearing 模式）——本件把「始终回显所解析
// changeId 与来源」推广到其余 14 个解析点（钩子 3 + 脚本 11）。
//
// 纪律（dispatch-stamp R1-F6 先例）：**始终**回显——错与对在输出上不再无差别；source 如实区分
// `--change-id 旗标` / `位置参数/环境变量` / `current/ 指针` 三类，不得固定写某一类。
// 判定语义零变更：纯 stderr 可见性齿，不改任何 exit code / ok 判定。
// 消费面注：闸/钩子的 stderr 由 verify.mjs boundedText 采集进 gate log（截断承载），
// 回显行不含 notExecuted 三信号 pattern（degraded / executor-skipped / allLiveDeselected）字面。

import { existsSync, mkdirSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

export const CURRENT_TASKPACKAGE_REL = 'delivery/taskpackages/current/taskpackage.json';

export function echoResolution(scriptName, changeId, source) {
  process.stderr.write(`[${scriptName}] TaskPackage 上下文解析：changeId=${changeId || '(none)'}（来源：${source}）\n`);
}

// 读 changeId：undefined = 文件不可读/不可解析；null = 无 changeId 字段。
function readChangeIdOrUndefined(path) {
  try {
    const tp = JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
    return tp.changeId || null;
  } catch {
    return undefined;
  }
}

// current/ 指针回退解析 + 回显。返回 { path, changeId, found }。
export function resolveCurrentTaskPackage(scriptName, projectDir) {
  const path = resolve(projectDir, CURRENT_TASKPACKAGE_REL);
  if (!existsSync(path)) {
    echoResolution(scriptName, null, `${CURRENT_TASKPACKAGE_REL}（不存在）`);
    return { path, changeId: null, found: false };
  }
  const changeId = readChangeIdOrUndefined(path);
  if (changeId === undefined) {
    echoResolution(scriptName, null, `${CURRENT_TASKPACKAGE_REL}（不可解析）`);
    return { path, changeId: null, found: false };
  }
  echoResolution(scriptName, changeId, CURRENT_TASKPACKAGE_REL);
  return { path, changeId, found: true };
}

// 显式传参解析 + 回显（source 如实标 kind：`位置参数` / `环境变量` / `--change-id 旗标` 等）。
export function resolveExplicitTaskPackage(scriptName, projectDir, explicitPath, kind = '显式传参') {
  const path = resolve(projectDir, explicitPath);
  const changeId = existsSync(path) ? readChangeIdOrUndefined(path) : undefined;
  const readable = changeId !== undefined;
  echoResolution(scriptName, readable ? changeId : null, readable ? `${kind}：${explicitPath}` : `${kind}：${explicitPath}（不可读/不可解析）`);
  return { path, changeId: readable ? changeId : null, found: readable };
}

// ---- self-check（真空绿规约：断言输入非空 + 逐 case 预期）----
function selfCheck() {
  let pass = 0;
  let failCount = 0;
  const ok = (cond, name) => { if (cond) pass += 1; else { failCount += 1; console.error(`FAIL [${name}]`); } };
  const tmp = mkdtempSync(join(tmpdir(), 'cps-selfcheck-'));
  const written = [];
  const orig = process.stderr.write.bind(process.stderr);
  const capture = () => { const lines = []; process.stderr.write = (s, ...rest) => { lines.push(String(s)); return true; }; return lines; };
  const restore = () => { process.stderr.write = orig; };
  try {
    // case 1：current/ 在场且可解析 → 回显 changeId + 来源为指针路径
    const dir = join(tmp, 'proj1'); mkdirSync(join(dir, 'delivery/taskpackages/current'), { recursive: true });
    writeFileSync(join(dir, 'delivery/taskpackages/current/taskpackage.json'), JSON.stringify({ changeId: 'demo-pkg-1' }));
    let lines = capture();
    const r1 = resolveCurrentTaskPackage('demo-script', dir);
    restore(); written.push(...lines);
    ok(r1.changeId === 'demo-pkg-1' && r1.found === true, 'current-resolved-changeId');
    ok(lines.some((l) => l.includes('changeId=demo-pkg-1') && l.includes(CURRENT_TASKPACKAGE_REL)), 'current-echo-line-content');
    // case 2：current/ 不存在 → changeId (none) + 「不存在」如实标注（判别性：不得静默）
    const dir2 = join(tmp, 'proj2'); mkdirSync(dir2, { recursive: true });
    lines = capture();
    const r2 = resolveCurrentTaskPackage('demo-script', dir2);
    restore(); written.push(...lines);
    ok(r2.found === false && r2.changeId === null, 'current-missing-null');
    ok(lines.some((l) => l.includes('changeId=(none)') && l.includes('不存在')), 'current-missing-echo-honest');
    // case 3：显式传参（可读文件）→ 回显实 changeId + kind 标注
    const expFile = join(dir, 'tp.json');
    writeFileSync(expFile, JSON.stringify({ changeId: 'demo-pkg-2' }));
    lines = capture();
    const r3 = resolveExplicitTaskPackage('demo-script', dir, 'tp.json', '位置参数');
    restore(); written.push(...lines);
    ok(r3.changeId === 'demo-pkg-2', 'explicit-resolved-changeId');
    ok(lines.some((l) => l.includes('changeId=demo-pkg-2') && l.includes('位置参数')), 'explicit-echo-line-content');
    // case 4：不可解析 JSON → 如实标注、changeId null（判别性：不假绿）
    writeFileSync(expFile, '{not json');
    lines = capture();
    const r4 = resolveExplicitTaskPackage('demo-script', dir, 'tp.json', '位置参数');
    restore(); written.push(...lines);
    ok(r4.found === false && lines.some((l) => l.includes('不可解析')), 'explicit-unparsable-honest');
  } finally {
    restore();
    try { rmSync(tmp, { recursive: true, force: true }); } catch { /* best effort */ }
  }
  console.log(JSON.stringify({ selfCheck: 'current-pointer-shared', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
  process.exit(failCount === 0 ? 0 : 1);
}

// isMain 守卫（acceptance-id-shared.mjs:182-185 对偶形态）：lib 的 exit 型 self-check 只在**本文件为入口**时跑——
// 否则 import 方带 --self-check 时会先跑本自检并 process.exit，抢先终止宿主自检（接线类真空绿陷阱，
// 260924-gov-review-gate-parse-teeth 落地期实测抓出：verify.mjs / mutation-check 宿主自检曾被子进程自检遮蔽）。
const invokedDirectly = process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url);
if (invokedDirectly && process.argv.includes('--self-check')) selfCheck();
