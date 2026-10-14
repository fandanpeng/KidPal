#!/usr/bin/env node
// change-manifest-check.mjs — changeManifest 对账闸（260928-gov-review-dev-loop-preassess-manifest-scope T6）
//
// 职责：git diff 涉及的对账面文件 vs TaskPackage `changeManifest[]` glob 全覆盖断言。
// 红条件（fail-closed）：清单外实现面文件在场 ∨ manifest 缺失/为空/不可解析（post-cutoff 包）。
// 对账面 = 自有实现面（firmware/** + server/**）——治理/交付/机械簿记面（delivery/**、.claude/**、docs/** 等）
// 豁免（各自由既有纪律管；防 pod-events / verifier-evidence / brief / test-scope sibling 等簿记写入常态红）。
// 存量兼容：与 taskpackage-required-check.mjs 共享同一 changeId cutoff 谓词（gitFirstCommitDate < MANIFEST_CUTOFF 放行）。
// 无 TaskPackage 上下文 → N/A 跳过（human 跳流程例外不经本闸；对偶 dispatch-stamp 行为）。
// Pod worktree 内 TaskPackage 定位沿用 current-pointer-shared 惯例（current/ 投影优先）。

import { existsSync, readFileSync, mkdtempSync, rmSync, writeFileSync, mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve, sep } from 'node:path';
import { execSync } from 'node:child_process';
import { resolveCurrentTaskPackage, resolveExplicitTaskPackage } from './lib/current-pointer-shared.mjs';

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'change-manifest', message, ...details });
  process.exit(1);
}

// ── cutoff 谓词（与 taskpackage-required-check.mjs CHANGE_ID_CUTOFF / AC_BACKREF_CUTOFF 同型）──
const MANIFEST_CUTOFF = '2026-09-28';

function gitFirstCommitDate(relPath) {
  try {
    const out = execSync(`git log --diff-filter=A --follow --format=%cs -- "${relPath}"`, {
      cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
    });
    const lines = out.split(/\r?\n/).map(l => l.trim()).filter(Boolean);
    return lines.length ? lines[lines.length - 1] : null;
  } catch {
    return null;
  }
}

// ── 对账面（T6：自有实现面；scope-map 实现面 = firmware/** + server/**）──
const RECONCILE_FACE_RE = /^(?:firmware|server)\//;

// ── glob → matcher（T4：文件/目录 glob，仓根相对；无通配符条目按目录前缀匹配）──
export function globToMatcher(entry) {
  const normalized = String(entry).trim().replace(/^\.\//, '').replace(/\\/g, '/');
  if (!normalized || normalized.startsWith('/') || normalized.includes('..') || /[A-Z]:/.test(normalized)) return null;
  const hasGlob = /[*?[\]]/.test(normalized);
  if (!hasGlob) {
    const prefix = normalized.endsWith('/') ? normalized : `${normalized}/`;
    return { re: null, prefix, exact: normalized };
  }
  // 先挖通配 token（占位符）、再转义字面量、后回填——顺序反了会让 `**/` 被转义拆散而失配（self-check 抓过）
  const T2S = '\u0000', T2 = '\u0001', T1 = '\u0002', TQ = '\u0003';
  let src = normalized
    .replace(/\*\*\//g, T2S)
    .replace(/\*\*/g, T2)
    .replace(/\*/g, T1)
    .replace(/\?/g, TQ)
    .replace(/[.+^${}()|[\]\\]/g, '\\$&');
  src = `^${src
    .split(T2S).join('(?:.*/)?')
    .split(T2).join('.*')
    .split(T1).join('[^/]*')
    .split(TQ).join('[^/]')}$`;
  return { re: new RegExp(src), prefix: null, exact: null };
}

export function manifestMatches(manifest, relPath) {
  const p = relPath.replace(/\\/g, '/');
  for (const entry of manifest) {
    const m = globToMatcher(entry);
    if (!m) continue;
    if (m.re ? m.re.test(p) : p === m.exact || p.startsWith(m.prefix)) return true;
  }
  return false;
}

// ── git status --porcelain 解析（staged ∨ unstaged ∨ untracked；rename 取两侧）──
export function parsePorcelain(out) {
  const files = new Set();
  for (const line of out.split(/\r?\n/)) {
    if (!line.trim()) continue;
    let path = line.slice(3);
    const arrow = path.indexOf(' -> ');
    if (arrow !== -1) {
      files.add(path.slice(0, arrow));
      files.add(path.slice(arrow + 4));
      continue;
    }
    if ((path.startsWith('"') && path.endsWith('"')) || path.endsWith('"')) {
      path = path.replace(/^"|"$/g, '').replace(/\\\\/g, '\\');
    }
    files.add(path);
  }
  return [...files];
}

export function changedFiles() {
  try {
    const out = execSync('git status --porcelain', { cwd: projectDir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    return parsePorcelain(out);
  } catch {
    return null;
  }
}

function taskPackagePath() {
  const explicit = process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH;
  if (explicit) return resolveExplicitTaskPackage('change-manifest', projectDir, explicit, '位置参数/环境变量').path;
  return resolveCurrentTaskPackage('change-manifest', projectDir).path;
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
}

function main() {
  if (process.argv.includes('--self-check')) return selfCheck();

  let tpPath;
  try {
    tpPath = taskPackagePath();
  } catch {
    print({ ok: true, gate: 'change-manifest', skipped: 'no-taskpackage-context', message: '无 TaskPackage 上下文 → N/A 跳过（human 跳流程例外不经本闸）' });
    return;
  }
  if (!tpPath || !existsSync(tpPath)) {
    print({ ok: true, gate: 'change-manifest', skipped: 'no-taskpackage-context', message: 'TaskPackage 解析失败 → N/A 跳过' });
    return;
  }

  // 存量兼容：与 taskpackage-required-check 共享 cutoff 谓词（pre-cutoff 在途包两闸同豁免）
  const firstDate = gitFirstCommitDate(tpPath);
  if (firstDate && firstDate < MANIFEST_CUTOFF) {
    print({ ok: true, gate: 'change-manifest', skipped: 'pre-cutoff', message: `gitFirstCommitDate ${firstDate} < MANIFEST_CUTOFF ${MANIFEST_CUTOFF} → 存量放行` });
    return;
  }

  let tp;
  try {
    tp = readJson(tpPath);
  } catch (e) {
    return fail('TaskPackage 不可解析（fail-closed）', { taskpackage: tpPath, error: String(e) });
  }

  if (!Array.isArray(tp.changeManifest) || tp.changeManifest.length === 0) {
    return fail('changeManifest 缺失或为空（fail-closed；产出 = architect 前置评估 1g 档，扩单唯一通道 = architect 评估）', { taskpackage: tpPath });
  }
  const bad = tp.changeManifest.filter((e) => !globToMatcher(e));
  if (bad.length) {
    return fail('changeManifest 含非法 glob 形态（仓根相对；禁绝对路径/穿越/反斜杠）', { taskpackage: tpPath, illegal: bad });
  }

  const files = changedFiles();
  if (!files) {
    return fail('git status 不可用（fail-closed；本闸无法对账即红，不假绿）');
  }

  const face = files.filter((f) => RECONCILE_FACE_RE.test(f.replace(/\\/g, '/')));
  const outOfManifest = face.filter((f) => !manifestMatches(tp.changeManifest, f));
  if (outOfManifest.length) {
    return fail('对账面存在清单外改动（fail-closed）：停手 → 带根因假设交 planner 派 architect 扩单（扩单留痕：manifest append + 评估依据）', {
      taskpackage: tpPath,
      outOfManifest,
      reconcileFace: 'firmware/** + server/**（簿记/治理面豁免，见 _doc）',
    });
  }

  print({
    ok: true,
    gate: 'change-manifest',
    message: '对账面改动全部在 changeManifest 内（或对账面无改动）',
    taskpackage: tpPath,
    manifestSize: tp.changeManifest.length,
    reconciledFaceCount: face.length,
  });
}

// ── self-check（纯函数 fixture，仿 taskpackage-required-check 惯例）──
function selfCheck() {
  const checks = [];
  const check = (name, actual, expected) => checks.push({ name, pass: actual === expected, actual, expected });

  // glob 匹配
  check('glob·目录前缀', manifestMatches(['firmware/main'], 'firmware/main/foo.c'), true);
  check('glob·目录前缀不越界', manifestMatches(['firmware/main'], 'firmware/mainx/foo.c'), false);
  check('glob·文件精确', manifestMatches(['server/app/store.py'], 'server/app/store.py'), true);
  check('glob·单星不过目录', manifestMatches(['firmware/*/*.c'], 'firmware/a/b/c.c'), false);
  check('glob·双星跨目录', manifestMatches(['firmware/**/*.c'], 'firmware/a/b/c.c'), true);
  check('glob·问号', manifestMatches(['server/test_?.py'], 'server/test_1.py'), true);
  // 非法形态
  check('glob·绝对路径拒', globToMatcher('/etc/passwd'), null);
  check('glob·穿越拒', globToMatcher('firmware/../secret'), null);
  // porcelain 解析
  const por = parsePorcelain([' M firmware/a.c', 'M  server/b.py', '?? delivery/taskpackages/x/pod-events.jsonl', 'R  old.c -> new.c', 'A  "sp ace.c"'].join('\n'));
  check('porcelain·修改', por.includes('firmware/a.c'), true);
  check('porcelain·暂存', por.includes('server/b.py'), true);
  check('porcelain·未跟踪', por.includes('delivery/taskpackages/x/pod-events.jsonl'), true);
  check('porcelain·rename 两侧', por.includes('old.c') && por.includes('new.c'), true);
  check('porcelain·带引号', por.includes('sp ace.c'), true);
  // 对账面过滤
  check('face·实现面命中', RECONCILE_FACE_RE.test('firmware/a.c'), true);
  check('face·簿记面豁免', RECONCILE_FACE_RE.test('delivery/taskpackages/x/pod-events.jsonl'), false);
  check('face·治理面豁免', RECONCILE_FACE_RE.test('.claude/hooks/verify.mjs'), false);

  const failed = checks.filter((c) => !c.pass);
  print({ ok: failed.length === 0, gate: 'change-manifest', mode: 'self-check', total: checks.length, failed: failed.length, checks });
  process.exit(failed.length ? 1 : 0);
}

main();
