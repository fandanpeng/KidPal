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
//
// 261120-gov-review-verify-gate-integrity-cluster T1（FROZEN v0.2；承载 ARQ-261117-change-manifest-cutoff-projection-bypass）：
// ① cutoff 基准修正：pre-cutoff 判定的首提日改取**包目录** `delivery/taskpackages/<changeId>`（与
//    taskpackage-required-check.mjs:148/:172/:422、devloop-artifact-check.mjs:167-171、closeout-check.mjs:154/:244
//    同基准）。修前取解析出的 taskpackage.json 文件路径，无参形态下即 current/ 投影文件（首提 2026-09-11 < cutoff），
//    致 cutoff 后**每个包**命中 skipped:'pre-cutoff' 早退、对账从未生效（实测 53 包 / 200 份轮次证据）。
//    基准只能来自解析出的 changeId，**不得**由路径父目录 / 上溯推导（current/ 目录首提 2026-09-15，同样 < cutoff）。
// ② skipped 去黑箱：跳过与对账两路输出均回显 cutoffBasis{kind,path,firstCommitDate,cutoff} + changeId，
//    让 verifier evidence 自身能回答「这次为什么跳过 / 为什么没跳过」。
// ③ 旗标面收口（human 2026-11-21 裁定 = OQ-1 甲，先例 mutation-check.mjs）：--change-id / 未知旗标 exit 2 /
//    --help exit 0；并把两类早退分开——「压根没给上下文」保留 skipped:'no-taskpackage-context' + exit 0（N/A 语义
//    一字不改），「显式给了上下文但不可解析」改 fail-loud exit 1（**经 human 授权的绿→红方向变更**）。
//    真实闸链不受③影响：verify-gates.json 注册为无参、verify.mjs:271-272 原样 spawn 不追加旗标。
// cutoff 日期值、比较方向、对账面、glob 判据、N/A 与 pre-cutoff 两类语义均**未改**。

import { existsSync, readFileSync, mkdtempSync, rmSync, writeFileSync, mkdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve, sep } from 'node:path';
import { execSync, spawnSync } from 'node:child_process';
import { echoResolution, resolveCurrentTaskPackage, resolveExplicitTaskPackage } from './lib/current-pointer-shared.mjs';

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

// ── cutoff 基准（T1①，FROZEN §3.1）──
// 基准 = **包目录** `delivery/taskpackages/<changeId>` 的首提日，与姊妹闸同形；changeId 来自解析结果。
// 禁止路径几何回退（父目录 / 上溯）：无参形态下解析路径的父目录 = delivery/taskpackages/current，
// 其首提日（实测 2026-09-15）同样早于 cutoff，回退等于没修（FROZEN §2.1 已实测否决）。
// changeId 不可得 → kind:'unavailable' + firstCommitDate:null → 调用方按 post-cutoff 处理（fail-closed，不放行）。
// 纯函数（gitFn 可注入），判别 case 在 selfCheck。
export function cutoffBasis(changeId, gitFn = gitFirstCommitDate) {
  if (typeof changeId !== 'string' || !changeId.trim()) {
    return { kind: 'unavailable', path: null, firstCommitDate: null, cutoff: MANIFEST_CUTOFF, reason: 'changeId 不可得 → 不回退路径几何基准，按 post-cutoff 进入对账（fail-closed）' };
  }
  const rel = `delivery/taskpackages/${changeId.trim()}`;
  return { kind: 'package-dir', path: rel, firstCommitDate: gitFn(rel) || null, cutoff: MANIFEST_CUTOFF };
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

// ── 旗标面（T1③，FROZEN §3.2/§3.3；先例 = mutation-check.mjs:18-30 / :243-258）──
const CHANGE_MANIFEST_USAGE = `用法：node .claude/scripts/change-manifest-check.mjs [<taskpackage.json 路径>] [--change-id <id>] [--help] [--self-check]
  <路径>（位置参数）   TaskPackage 文件路径（向后兼容；env DEV_LOOP_TASKPACKAGE / TASKPACKAGE_PATH 同序回退）
  --change-id <id>     解析为 delivery/taskpackages/<id>/taskpackage.json（优先级最高）
  --self-check         自检（纯函数 + 真实消费层无参 CLI 夹具）
  --help / -h          本用法（exit 0）
未知 -- 旗标 = 用法错误 exit 2；--change-id 缺值 = 用法错误 exit 2。
显式给了上下文但不可解析（路径打错 / 文件不可读 / JSON 不可解析）= fail-loud exit 1，不再静默 skipped 假绿。
压根没给上下文（无参且 current/ 投影不可得或不可解析）= 既有 N/A 语义，skipped:'no-taskpackage-context' + exit 0。`;

const KNOWN_FLAGS = new Set(['--self-check', '--change-id', '--help', '-h']);

function flagVal(name) {
  const i = process.argv.indexOf(name);
  if (i === -1) return undefined;
  const next = process.argv[i + 1];
  return next !== undefined && !next.startsWith('--') ? next : undefined;
}

// 纯函数（self-check 可测）：--change-id → canonical taskpackage 路径（与 mutation-check.changeIdTaskPackagePath 同形）。
export function changeIdTaskPackagePath(changeId, root = projectDir) {
  return resolve(root, 'delivery/taskpackages', changeId, 'taskpackage.json');
}

function readableJson(path) {
  try {
    JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
    return true;
  } catch {
    return false;
  }
}

// 位置参数 = argv 中第一个非旗标 token（未知 -- 旗标已在入口拒掉，故此处不会把旗标当路径——修前病灶）。
function positionalArg() {
  return process.argv.slice(2).find((token) => !token.startsWith('-'))
    || process.env.DEV_LOOP_TASKPACKAGE
    || process.env.TASKPACKAGE_PATH
    || null;
}

// 上下文解析三形态（FROZEN §3.2）：--change-id 旗标 > 位置参数/环境变量 > current/ 指针回退。
// 返回 { path, changeId, found, explicit, source, received }：
//   explicit = 调用方是否**显式给了**上下文 → 决定 §3.3 的 fail-loud（explicit 且 !found）vs N/A（!explicit 且 !found）分支；
//   found    = 目标文件在场且 JSON 可解析（changeId 字段缺失仍算 found，属「上下文可读但基准不可得」，见 cutoffBasis）；
//   changeId = 基准派生用（旗标形态取旗标值；其余取文件内 changeId，由 current-pointer-shared 解析并回显）。
function resolveContext() {
  if (process.argv.includes('--change-id')) {
    const flagChangeId = flagVal('--change-id');
    const path = changeIdTaskPackagePath(flagChangeId);
    const found = existsSync(path) && readableJson(path);
    echoResolution('change-manifest', found ? flagChangeId : null, found ? '--change-id 旗标' : `--change-id 旗标：${flagChangeId}（不可读/不可解析）`);
    return { path, changeId: flagChangeId, found, explicit: true, source: '--change-id 旗标', received: `--change-id ${flagChangeId}` };
  }
  const explicitRaw = positionalArg();
  if (explicitRaw) {
    const resolved = resolveExplicitTaskPackage('change-manifest', projectDir, explicitRaw, '位置参数/环境变量');
    return { path: resolved.path, changeId: resolved.changeId, found: resolved.found, explicit: true, source: '位置参数/环境变量', received: explicitRaw };
  }
  const current = resolveCurrentTaskPackage('change-manifest', projectDir);
  return { path: current.path, changeId: current.changeId, found: current.found, explicit: false, source: 'current/ 指针', received: null };
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^﻿/, ''));
}

function main() {
  const ctx = resolveContext();
  const tpPath = ctx.path;

  // ── §3.3 第一行：压根没给上下文 → 既有 N/A 语义（一字不改：skipped:'no-taskpackage-context' + ok:true + exit 0）──
  // human 跳流程例外不经本闸；current/ 投影不可得或不可解析同归此支（JSON 不可解析另有 taskpackage-required-check
  // 的 fail('TaskPackage is not valid JSON') 在先承重，故此处保留 N/A 不构成假绿通道）。
  if (!ctx.explicit && !ctx.found) {
    print({
      ok: true,
      gate: 'change-manifest',
      skipped: 'no-taskpackage-context',
      changeId: ctx.changeId,
      contextSource: ctx.source,
      message: '无 TaskPackage 上下文 → N/A 跳过（human 跳流程例外不经本闸）',
    });
    return;
  }

  // ── §3.3 第二行：显式给了上下文但不可解析 → fail-loud（human 2026-11-21 授权的绿→红方向变更）──
  // 修前病灶：旗标被当路径 / 包名打错 / --help 一律 ok:true + exit 0（静默假绿，实测见 FROZEN §2.2）。
  if (ctx.explicit && !ctx.found) {
    return fail('显式 TaskPackage 上下文不可解析（fail-loud；不再静默 skipped 假绿）', {
      received: ctx.received,
      contextSource: ctx.source,
      resolvedPath: tpPath,
      hint: '位置参数/环境变量须指向在场且可解析的 taskpackage.json；或用 --change-id <id> 解析 delivery/taskpackages/<id>/taskpackage.json；无上下文请不传任何参数（走 N/A 跳过）',
    });
  }

  // ── §3.1 cutoff 基准 = 包目录首提日（与 taskpackage-required-check 同基准；pre-cutoff 在途包两闸同豁免）──
  const basis = cutoffBasis(ctx.changeId);
  if (basis.firstCommitDate && basis.firstCommitDate < MANIFEST_CUTOFF) {
    print({
      ok: true,
      gate: 'change-manifest',
      skipped: 'pre-cutoff',
      changeId: ctx.changeId,
      contextSource: ctx.source,
      cutoffBasis: basis,
      message: `包目录 ${basis.path} 首提日 ${basis.firstCommitDate} < MANIFEST_CUTOFF ${MANIFEST_CUTOFF} → 存量放行`,
    });
    return;
  }

  let tp;
  try {
    tp = readJson(tpPath);
  } catch (e) {
    return fail('TaskPackage 不可解析（fail-closed）', { taskpackage: tpPath, changeId: ctx.changeId, cutoffBasis: basis, error: String(e) });
  }

  if (!Array.isArray(tp.changeManifest) || tp.changeManifest.length === 0) {
    return fail('changeManifest 缺失或为空（fail-closed；产出 = architect 前置评估 1g 档，扩单唯一通道 = architect 评估）', { taskpackage: tpPath, changeId: ctx.changeId, cutoffBasis: basis });
  }
  const bad = tp.changeManifest.filter((e) => !globToMatcher(e));
  if (bad.length) {
    return fail('changeManifest 含非法 glob 形态（仓根相对；禁绝对路径/穿越/反斜杠）', { taskpackage: tpPath, changeId: ctx.changeId, cutoffBasis: basis, illegal: bad });
  }

  const files = changedFiles();
  if (!files) {
    return fail('git status 不可用（fail-closed；本闸无法对账即红，不假绿）', { changeId: ctx.changeId, cutoffBasis: basis });
  }

  const face = files.filter((f) => RECONCILE_FACE_RE.test(f.replace(/\\/g, '/')));
  const outOfManifest = face.filter((f) => !manifestMatches(tp.changeManifest, f));
  if (outOfManifest.length) {
    return fail('对账面存在清单外改动（fail-closed）：停手 → 带根因假设交 planner 派 architect 扩单（扩单留痕：manifest append + 评估依据）', {
      taskpackage: tpPath,
      changeId: ctx.changeId,
      cutoffBasis: basis,
      outOfManifest,
      reconcileFace: 'firmware/** + server/**（簿记/治理面豁免，见 _doc）',
    });
  }

  print({
    ok: true,
    gate: 'change-manifest',
    message: '对账面改动全部在 changeManifest 内（或对账面无改动）',
    taskpackage: tpPath,
    changeId: ctx.changeId,
    contextSource: ctx.source,
    cutoffBasis: basis,
    manifestSize: tp.changeManifest.length,
    reconciledFaceCount: face.length,
  });
}

// ── 消费层夹具（FROZEN §6 V1/V2/V5-B/V5-C）：真 git 仓 + 无参 CLI spawn ──
// 判别力硬条件（V1）：投影文件首提日 < cutoff 而**包目录**首提日 ≥ cutoff（两侧分离）——两侧同日期则夹具零判别力
// （修前修后同绿），起草期实测踩过一次，故 selfCheck 先自证三基准再断言。
// 夹具技术注（§7 判档事实②）：GIT_COMMITTER_DATE + commit --date 回填构造跨 cutoff 两侧历史，全仓此前无先例
// （git init 夹具先例 = landing-fidelity-check.mjs makeGitFixtureRepo）。
// 机械坑：夹具文件一律 writeFileSync 构造——enforce-scope 的 universal.bashDenyWrite 按路径字面拦 Bash 重定向写
// delivery/taskpackages/current/taskpackage.json，即便目标是仓外临时目录；git add 带 -c core.autocrlf=false 静音 CRLF 警告。
const FIXTURE_PRE_CUTOFF_DATE = '2026-09-11T10:00:00';
const SELF_PATH = resolve(process.argv[1] || '');

function fixtureGit(dir, args, env) {
  return execSync(`git ${args}`, { cwd: dir, encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'], env: { ...process.env, ...(env || {}) } });
}

function firstDateIn(dir, relPath) {
  try {
    const out = fixtureGit(dir, `log --diff-filter=A --follow --format=%cs -- "${relPath}"`);
    const lines = out.split(/\r?\n/).map((l) => l.trim()).filter(Boolean);
    return lines.length ? lines[lines.length - 1] : null;
  } catch {
    return null;
  }
}

function writeFixtureFile(dir, rel, content) {
  const abs = join(dir, rel);
  mkdirSync(dirname(abs), { recursive: true });
  writeFileSync(abs, content);
  return abs;
}

// 夹具仓：commit A 回填到 cutoff 前（只建 current/ 投影）→ commit B 当日建包目录 + 已跟踪 firmware/base.c
// 并重指向 current/ → 工作树留一个未提交、未被 manifest 覆盖的 firmware/new.c。
// 注：current/taskpackage.json 必须是**完整副本**（含 changeManifest），与真仓一致——无参形态下闸读的就是投影文件，
// 且 taskpackage-required-check.mjs:301-321 强制 canonical ↔ current 散列一致；只写 {changeId} 的瘦投影会让
// 无参判别落在「manifest 缺失」而非「清单外改动」上（起草期实测踩过，V1 的 outOfManifest 断言即为此而设）。
function writeFixtureTaskPackage(dir, changeId, manifest, { withCurrent = true } = {}) {
  const body = JSON.stringify({ changeId, changeManifest: manifest });
  writeFixtureFile(dir, `delivery/taskpackages/${changeId}/taskpackage.json`, body);
  if (withCurrent && existsSync(join(dir, 'delivery/taskpackages/current/taskpackage.json'))) {
    writeFixtureFile(dir, 'delivery/taskpackages/current/taskpackage.json', body);
  }
}

function makeFixtureRepo(opts = {}) {
  const o = { changeId: '269901-fixture-pkg', manifest: ['server/**'], backdatePackage: false, withCurrent: true, uncommittedImplFile: 'firmware/new.c', ...opts };
  const dir = mkdtempSync(join(tmpdir(), 'cm-selfcheck-'));
  const backdated = { GIT_COMMITTER_DATE: FIXTURE_PRE_CUTOFF_DATE };
  fixtureGit(dir, 'init -q .');
  fixtureGit(dir, 'config user.email selfcheck@example.com');
  fixtureGit(dir, 'config user.name selfcheck-fixture');
  if (o.withCurrent) {
    writeFixtureFile(dir, 'delivery/taskpackages/current/taskpackage.json', JSON.stringify({ changeId: '260901-old-pointer' }));
    fixtureGit(dir, '-c core.autocrlf=false -c core.safecrlf=false add -A');
    fixtureGit(dir, `commit -q --date=${FIXTURE_PRE_CUTOFF_DATE} -m fixture-backdated-current-projection`, backdated);
  }
  writeFixtureFile(dir, `delivery/taskpackages/${o.changeId}/taskpackage.json`, JSON.stringify({ changeId: o.changeId, changeManifest: o.manifest }));
  if (o.withCurrent) writeFixtureFile(dir, 'delivery/taskpackages/current/taskpackage.json', JSON.stringify({ changeId: o.changeId, changeManifest: o.manifest }));
  writeFixtureFile(dir, 'firmware/base.c', 'int fixture_base(void) { return 0; }\n');
  fixtureGit(dir, '-c core.autocrlf=false -c core.safecrlf=false add -A');
  if (o.backdatePackage) fixtureGit(dir, `commit -q --date=${FIXTURE_PRE_CUTOFF_DATE} -m fixture-backdated-package`, backdated);
  else fixtureGit(dir, 'commit -q -m fixture-post-cutoff-package');
  if (o.uncommittedImplFile) writeFixtureFile(dir, o.uncommittedImplFile, 'int fixture_uncovered(void) { return 1; }\n');
  return dir;
}

// 真实消费层调用形态：零位置参数（或指定 args）、cwd 与 CLAUDE_PROJECT_DIR 同指夹具仓——
// 与 verify.mjs:271-272 对 timing:scoped 闸的调用为同形态消费层调用（FROZEN §6 V1 执行形态，R1 NIT-3 措辞）。
function runGateInFixture(dir, args = []) {
  const run = spawnSync(process.execPath, [SELF_PATH, ...args], {
    cwd: dir, encoding: 'utf8', timeout: 120000, env: { ...process.env, CLAUDE_PROJECT_DIR: dir },
  });
  const stdout = String(run.stdout || '');
  const stderr = String(run.stderr || '');
  let json = null;
  try { json = JSON.parse(stdout); } catch { /* usage / 未知旗标走文本断言 */ }
  return { status: run.status, stdout, stderr, text: stdout + stderr, json };
}

// ── self-check（纯函数 fixture + 消费层 CLI 夹具，仿 taskpackage-required-check / mutation-check 惯例）──
function selfCheck() {
  const checks = [];
  const check = (name, actual, expected) => checks.push({ name, pass: actual === expected, actual, expected });
  const ok = (name, cond, detail) => checks.push({ name, pass: !!cond, ...(cond ? {} : { detail }) });

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
  // ── T1① cutoff 基准（纯函数面；消费层判别在下方 CLI 夹具）──
  check('cutoffBasis·changeId 不可得 → kind unavailable（不回退路径几何）', cutoffBasis(null, () => '2026-09-11').kind, 'unavailable');
  check('cutoffBasis·changeId 不可得 → 首提日 null（调用方按 post-cutoff 处理）', cutoffBasis('', () => '2026-09-11').firstCommitDate, null);
  check('cutoffBasis·有 changeId → 基准路径 = 包目录（与姊妹闸同形）', cutoffBasis('269901-x', () => null).path, 'delivery/taskpackages/269901-x');
  check('cutoffBasis·有 changeId → kind package-dir + 透传 gitFn 结果', cutoffBasis('269901-x', () => '2026-11-04').firstCommitDate, '2026-11-04');
  check('cutoffBasis·cutoff 常量随基准回显（去黑箱）', cutoffBasis('269901-x', () => null).cutoff, MANIFEST_CUTOFF);
  // ── T1③ --change-id → canonical 路径（纯函数面）──
  ok('flag·changeIdTaskPackagePath 解析 canonical 路径', changeIdTaskPackagePath('269901-x', '/repo').replace(/\\/g, '/').endsWith('/repo/delivery/taskpackages/269901-x/taskpackage.json'), changeIdTaskPackagePath('269901-x', '/repo'));

  // ══ 消费层 CLI 夹具（FROZEN §6 V1 / V2 / V5-B / V5-C）══
  // 夹具 A：post-cutoff 包 + 未覆盖的 firmware/new.c（V1 主判别）
  const dirA = makeFixtureRepo();
  try {
    // V1 夹具自证（硬条件，不成立则下列断言无判别力）
    const projDate = firstDateIn(dirA, 'delivery/taskpackages/current/taskpackage.json');
    const pkgDate = firstDateIn(dirA, 'delivery/taskpackages/269901-fixture-pkg');
    ok('V1·夹具自证：current/ 投影首提日 < cutoff', projDate && projDate < MANIFEST_CUTOFF, { projDate });
    ok('V1·夹具自证：包目录首提日 ≥ cutoff', pkgDate && pkgDate >= MANIFEST_CUTOFF, { pkgDate });
    ok('V1·夹具自证：两基准分处 cutoff 两侧（同侧 = 夹具无效）', projDate !== pkgDate, { projDate, pkgDate });

    const noArg = runGateInFixture(dirA);
    ok('V1·无参消费层：清单外实现面文件 → 退出码非 0', noArg.status !== 0, { status: noArg.status, stdout: noArg.stdout.slice(0, 400) });
    ok('V1·无参消费层：不再输出 skipped 键（修前 = skipped:"pre-cutoff"）', noArg.json && !('skipped' in noArg.json), { json: noArg.json });
    ok('V1·无参消费层：outOfManifest 点名 firmware/new.c', noArg.json && Array.isArray(noArg.json.outOfManifest) && noArg.json.outOfManifest.some((f) => String(f).replace(/\\/g, '/').includes('firmware/new.c')), { outOfManifest: noArg.json && noArg.json.outOfManifest });
    ok('V1·无参消费层：回显 cutoffBasis = 包目录 + 包目录首提日（非投影文件）',
      noArg.json && noArg.json.cutoffBasis && noArg.json.cutoffBasis.kind === 'package-dir'
      && String(noArg.json.cutoffBasis.path).replace(/\\/g, '/').endsWith('delivery/taskpackages/269901-fixture-pkg')
      && noArg.json.cutoffBasis.firstCommitDate === pkgDate,
      { basis: noArg.json && noArg.json.cutoffBasis, pkgDate });

    // V2·不 over-red：manifest 覆盖后必须绿（反「无条件 fail」）
    writeFixtureTaskPackage(dirA, '269901-fixture-pkg', ['firmware/**', 'server/**']);
    const covered = runGateInFixture(dirA);
    ok('V2·manifest 覆盖后：exit 0 / ok:true / 无 skipped / reconciledFaceCount ≥ 1',
      covered.status === 0 && covered.json && covered.json.ok === true && !('skipped' in covered.json) && covered.json.reconciledFaceCount >= 1,
      { status: covered.status, json: covered.json });
    writeFixtureTaskPackage(dirA, '269901-fixture-pkg', ['server/**']);

    // V5-B·--change-id 与无参同结论 + 回显来源如实
    const viaFlag = runGateInFixture(dirA, ['--change-id', '269901-fixture-pkg']);
    ok('V5-B·--change-id：与无参同结论（红 + outOfManifest 非空）', viaFlag.status !== 0 && viaFlag.json && Array.isArray(viaFlag.json.outOfManifest) && viaFlag.json.outOfManifest.length > 0, { status: viaFlag.status, json: viaFlag.json });
    ok('V5-B·--change-id：stderr 回显来源 = --change-id 旗标', /来源：--change-id 旗标/.test(viaFlag.stderr), { stderr: viaFlag.stderr.slice(0, 200) });
    // V5-B·位置参数兼容不回退
    const viaPos = runGateInFixture(dirA, ['delivery/taskpackages/269901-fixture-pkg/taskpackage.json']);
    ok('V5-B·位置参数兼容：与无参同结论（红 + outOfManifest 非空）', viaPos.status !== 0 && viaPos.json && Array.isArray(viaPos.json.outOfManifest) && viaPos.json.outOfManifest.length > 0, { status: viaPos.status, json: viaPos.json });
    ok('V5-B·位置参数：stderr 回显来源 = 位置参数/环境变量', /来源：位置参数\/环境变量/.test(viaPos.stderr), { stderr: viaPos.stderr.slice(0, 200) });
    // V5-B·未知旗标 fail-loud（修前 = exit 0 假绿）
    const bogus = runGateInFixture(dirA, ['--bogus-probe']);
    ok('V5-B·未知旗标：非 0 退出 + usage 列出支持旗标集', bogus.status !== 0 && /用法：/.test(bogus.text) && /--change-id/.test(bogus.text), { status: bogus.status, text: bogus.text.slice(0, 200) });
    // V5-B·--help exit 0（修前 = exit 0 但输出 skipped 假绿 JSON，非 usage）
    const help = runGateInFixture(dirA, ['--help']);
    ok('V5-B·--help：exit 0 + usage（不是 skipped 假绿 JSON）', help.status === 0 && /用法：/.test(help.text) && !/"ok":\s*true/.test(help.text), { status: help.status, text: help.text.slice(0, 200) });
    // V5-C 负例·显式路径打错 → fail-loud（修前 = exit 0 + skipped:"no-taskpackage-context" 假绿）
    const typo = runGateInFixture(dirA, ['delivery/taskpackages/no-such-pkg/taskpackage.json']);
    ok('V5-C·负例：显式路径打错 → 非 0 且点名收到的值', typo.status !== 0 && /no-such-pkg/.test(typo.text), { status: typo.status, text: typo.text.slice(0, 300) });
    // --change-id 指向不存在的包 → 同属显式不可解析，fail-loud
    const typoFlag = runGateInFixture(dirA, ['--change-id', 'no-such-pkg']);
    ok('V5-C·负例：--change-id 指向不存在的包 → 非 0（修前 exit 0 假绿）', typoFlag.status !== 0 && /no-such-pkg/.test(typoFlag.text), { status: typoFlag.status, text: typoFlag.text.slice(0, 300) });

    // V2·cutoff 基准不可得（包目录无提交历史）→ 不跳过、进入对账（fail-closed，防「取不到日期就放行」）
    writeFixtureFile(dirA, 'delivery/taskpackages/current/taskpackage.json', JSON.stringify({ changeId: '269902-never-committed', changeManifest: ['server/**'] }));
    const nullBasis = runGateInFixture(dirA);
    ok('V2·基准不可得：不跳过、进入对账并红（fail-closed）', nullBasis.status !== 0 && nullBasis.json && !('skipped' in nullBasis.json) && nullBasis.json.cutoffBasis.firstCommitDate === null, { status: nullBasis.status, json: nullBasis.json });
  } finally {
    try { rmSync(dirA, { recursive: true, force: true }); } catch { /* best effort */ }
  }

  // 夹具 B：真存量包（包目录也回填到 cutoff 前 + manifest 为空）→ pre-cutoff 放行语义一字不改
  // 判别力：manifest 为空，若放行分支失效则必红（fail('changeManifest 缺失或为空')）。
  const dirB = makeFixtureRepo({ changeId: '260901-legacy-pkg', manifest: [], backdatePackage: true });
  try {
    const legacyDate = firstDateIn(dirB, 'delivery/taskpackages/260901-legacy-pkg');
    ok('V2·夹具自证：存量包目录首提日 < cutoff', legacyDate && legacyDate < MANIFEST_CUTOFF, { legacyDate });
    const legacy = runGateInFixture(dirB);
    ok('V2·真存量包：exit 0 + skipped:"pre-cutoff"（放行语义不变）', legacy.status === 0 && legacy.json && legacy.json.skipped === 'pre-cutoff', { status: legacy.status, json: legacy.json });
    ok('V2·真存量包：回显基准 = 包目录 + 回填日期（不是投影文件路径）',
      legacy.json && legacy.json.cutoffBasis && legacy.json.cutoffBasis.kind === 'package-dir'
      && String(legacy.json.cutoffBasis.path).endsWith('260901-legacy-pkg')
      && legacy.json.cutoffBasis.firstCommitDate === legacyDate,
      { basis: legacy.json && legacy.json.cutoffBasis, legacyDate });
  } finally {
    try { rmSync(dirB, { recursive: true, force: true }); } catch { /* best effort */ }
  }

  // 夹具 C：压根没给上下文（无 current/、无任何参数）→ 既有 N/A 语义一字不改（V5-C 正例，防 fail-loud 过度扩张）
  const dirC = mkdtempSync(join(tmpdir(), 'cm-selfcheck-nocontext-'));
  try {
    const noContext = runGateInFixture(dirC);
    ok('V5-C·正例：无上下文 → exit 0 + skipped:"no-taskpackage-context"（N/A 语义不变）',
      noContext.status === 0 && noContext.json && noContext.json.skipped === 'no-taskpackage-context' && noContext.json.ok === true,
      { status: noContext.status, json: noContext.json });
  } finally {
    try { rmSync(dirC, { recursive: true, force: true }); } catch { /* best effort */ }
  }

  const failed = checks.filter((c) => !c.pass);
  print({ ok: failed.length === 0, gate: 'change-manifest', mode: 'self-check', total: checks.length, failed: failed.length, checks });
  process.exit(failed.length ? 1 : 0);
}

// ── 入口分派（先例 = mutation-check.mjs:243-258）：self-check → help → 未知旗标拒绝 → main ──
// 顺序承重：未知旗标必须先于 main() 拒掉，否则 `--change-id` 一类旗标会被当位置参数路径解析（修前病灶）。
const argvTail = process.argv.slice(2);
if (process.argv.includes('--self-check')) {
  selfCheck();
} else if (argvTail.includes('--help') || argvTail.includes('-h')) {
  console.log(CHANGE_MANIFEST_USAGE);
  process.exit(0);
} else {
  const unknown = argvTail.filter((token) => token.startsWith('--') && !KNOWN_FLAGS.has(token));
  if (unknown.length > 0) {
    console.error(`未知旗标：${unknown[0]}\n${CHANGE_MANIFEST_USAGE}`);
    process.exit(2);
  }
  if (process.argv.includes('--change-id') && flagVal('--change-id') === undefined) {
    console.error(`--change-id 缺值\n${CHANGE_MANIFEST_USAGE}`);
    process.exit(2);
  }
  main();
}
