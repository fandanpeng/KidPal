#!/usr/bin/env node
// Validates that a TaskPackage has frozen acceptance tests with coverage markers.
// The script is intentionally conservative and dependency-free so it can run
// before the monorepo runtime is fully bootstrapped.
//
// gov-review-verifier-gate-infra-fixes（human-approved 2026-07-15）——按 PROPOSAL F4 落地。
// 只改 acceptanceId() 的短 id 提取；structural 白名单与 mentionsId 不动（CH-5 → ARQ-098）。
//
// 260731-gov-review-acceptance-coverage-structural-exemption（FROZEN v1.0，human-approved 2026-07-31）：
// structuralAcceptanceIds / mentionsId 收敛入 ./lib/acceptance-id-shared.mjs 单源（本地拷贝删净）；
// :84 governanceWords prose 洗白移除（恢复成文门义，dev-loop.md:156 早已明禁）；
// mentionsId 去裸后缀 = 力度变更（k8 碰撞闭合；区间缩写/prose 同物提及不再豁免——成文辨析见 dev-loop.md §3 A10 段）；
// 新增 suiteGate:true 一等豁免（套件级回归门显式声明），stdout 分印 structural/suiteGates 供审计。
//
// 261120-gov-review-verify-gate-integrity-cluster T2（FROZEN v0.2；承载 ARQ-261117-acceptance-coverage-assertion-and-flag-gaps）：
// ① Unity 识别面加宽（OQ-2 裁定 = 变体 (c)：行首锚定 + 显式容纳 #define 预处理行）——修前纯 Unity 验收件假红
//    （`TEST_ASSERT_EQUAL_MESSAGE(` 中 ASSERT 前是下划线，`\bassert` 词边界不成立），实测 5 个跟踪文件受害。
//    既有六支模式、isShallowStatusOnly、hasCoverageMarker、豁免面、coverage 对账逻辑**全部不动**。
// ② 旗标面对齐 mutation-check.mjs 先例：--change-id / 位置参数与环境变量兼容 / 未知旗标 fail-loud / --help exit 0。
//    本闸修前对误用已是 exit 1（红但报错误话），故②不涉绿→红方向变更；不引入注释/字符串剥离（(b) 变体，
//    触既有门语义、按下方 :96-100 注记须另批 + human，本批不取）。

import { existsSync, readFileSync, mkdtempSync, mkdirSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { spawnSync } from 'node:child_process';
import { acceptanceId, structuralAcceptanceIds } from './lib/acceptance-id-shared.mjs'; // ARQ-097: F4 短 id 提取收敛单源；260731: structuralAcceptanceIds 同单源（原本地拷贝已删）
import { echoResolution, resolveCurrentTaskPackage, resolveExplicitTaskPackage } from './lib/current-pointer-shared.mjs'; // T6 回显（260924-gov-review-gate-parse-teeth）；261120 批 --change-id 分支复用

const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();

// ── 旗标面（T2②，FROZEN §4.2；先例 = mutation-check.mjs:18-30 / :243-258）──
const ACCEPTANCE_COVERAGE_USAGE = `用法：node .claude/scripts/acceptance-coverage-check.mjs [<taskpackage.json 路径>] [--change-id <id>] [--help] [--self-check]
  <路径>（位置参数）   TaskPackage 文件路径（向后兼容；env DEV_LOOP_TASKPACKAGE / TASKPACKAGE_PATH 同序回退）
  --change-id <id>     解析为 delivery/taskpackages/<id>/taskpackage.json（优先级最高）
  --self-check         自检（纯函数 + 真实消费层无参 CLI 夹具）
  --help / -h          本用法（exit 0）
未知 -- 旗标 = 用法错误 exit 2（不再把旗标当路径解析后报 "TaskPackage file does not exist" 这句错误话）。`;

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

// 位置参数 = argv 中第一个非旗标 token（未知 -- 旗标已在入口拒掉，故不会再把旗标当路径——修前病灶）。
function positionalArg() {
  return process.argv.slice(2).find((token) => !token.startsWith('-'))
    || process.env.DEV_LOOP_TASKPACKAGE
    || process.env.TASKPACKAGE_PATH
    || null;
}

function taskPackagePath() {
  const flagChangeId = flagVal('--change-id');
  if (flagChangeId) {
    echoResolution('acceptance-coverage-check', flagChangeId, '--change-id 旗标');
    return changeIdTaskPackagePath(flagChangeId);
  }
  const explicit = positionalArg();
  if (explicit) return resolveExplicitTaskPackage('acceptance-coverage-check', projectDir, explicit, '位置参数/环境变量').path;

  const resolved = resolveCurrentTaskPackage('acceptance-coverage-check', projectDir);
  return existsSync(resolved.path) ? resolved.path : null;
}

function print(payload) {
  console.log(JSON.stringify(payload, null, 2));
}

function fail(message, details = {}) {
  print({ ok: false, gate: 'acceptance-coverage', message, ...details });
  process.exit(1);
}

function readJson(path) {
  return JSON.parse(readFileSync(path, 'utf8').replace(/^\uFEFF/, ''));
}

function escapeRegExp(value) {
  return value.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

function hasCoverageMarker(content, acceptanceId) {
  const id = escapeRegExp(acceptanceId);
  return new RegExp(`(?:coverage|covers|@coverage)\\s*:?\\s*${id}\\b`, 'i').test(content)
    || new RegExp(`(?:describe|it|test)\\s*\\([^)]*${id}\\b`, 'i').test(content);
}

// extractShortId + acceptanceId 收敛入 ./lib/acceptance-id-shared.mjs（ARQ-097，F4 单源；import 见顶部）。hasCoverageMarker 仍用本地 escapeRegExp，未迁。

/**
 * Extract the short acceptance ref from a full acceptance string.
 * E.g. "TP-7-A1: S1 existing acceptance..." -> "TP-7-A1"
 *      "S-F1.2" -> "S-F1.2"
 * The short ref is the first whitespace-delimited or colon-terminated token
 * that matches the sourceRef pattern (alphanum-hyphen).
 */
function shortAcceptanceRef(fullId) {
  if (!fullId) return null;
  // If it's already a short ref (no colon, no long prose), return as-is
  const colonIdx = fullId.indexOf(':');
  if (colonIdx === -1 && fullId.length <= 20) return fullId;
  // Extract the token before the colon or first space
  const token = colonIdx >= 0 ? fullId.slice(0, colonIdx).trim() : fullId.split(/\s+/)[0];
  // Validate it looks like a ref: e.g. TP-7-A1, S-F1.2, ARQ-063-C1, TEMP-*
  if (/^[A-Za-z]+[-\d]+/.test(token) && token.length >= 4) return token;
  return fullId;
}

// ── Unity（ESP-IDF host 验收主力形态）识别面：261120 批 T2①，OQ-2 裁定 = 变体 (c) ──
// 形态 = **行首锚定** + 显式容纳 `#define` 预处理行。依据（FROZEN §2.3 / §4.1 / §7 OQ-2，均为起草期实测）：
//  - 行首锚定沿用本文件既有裸 `assert` 的先例（该行注释即写「避 prose 误报」），使注释行 / 字符串字面量内
//    **带括号**的 `TEST_ASSERT_*` 字样不构成「有断言」——不新开 prose fail-open 面（无锚定变体 (a) 会开）。
//  - 容 `#define` 行是为不漏判「断言只以宏包装形态出现」的件：仓内 4800 处调用形态中 78 处在 `#define` 行
//    （含 `firmware/main/test/acceptance/red-baseline/unity.h`，纯行首锚定会令其假红）。
//  - 已知宽松面（R1 NIT-4，刻意保留）：仅定义 `TEST_ASSERT_*` 宏而不调用的 header 会被认作有断言；
//    该文件从未被任何包列为 acceptanceTestsFile（实测 0 命中），不收紧。
// 前向边界（R1 OQ-2 注记，诚实声明）：**仅以行中调用**承载断言的未来验收件会假红（当日语料 0 例）。
// 处置 = 把断言写成行首（与既有裸 assert 先例一致），或另立治理批放宽锚定；**不得在本闸就地放宽**。
const UNITY_ASSERT_RE = /(?:^|\n)[ \t]*(?:#[ \t]*define\b[^\n]*)?TEST_(?:ASSERT|FAIL)[A-Z0-9_]*[ \t]*\(/m;

function hasRealAssertion(content) {
  return /\bexpect\s*\(/.test(content)
    || /\bassert\s*[.(]/.test(content)
    || /\.to(?:Be|Equal|Have|Contain|Match|Throw)/.test(content)
    // 260902 Pod S 一次性最小热修（human 拍板授权跳过流程，诚实记录于
    // 260829-smoke-e2e/pod-events.jsonl human-decision 0faa3a03；escalation 8bb97fb8）：
    // 补 Python 断言识别。原实现仅 JS 风格模式，Python unittest 冻结验收（实质满含
    // 真断言）被误判「无可识别断言」。纯追加，既有模式与门义（拦空壳验收）不改。
    || /\.assert[A-Z]\w*\s*\(/.test(content) // unittest/pytest: self.assertTrue/In/Equal(...)
    || /(?:^|\n)\s*assert\s+\S/m.test(content) // 裸 assert 语句（行首锚定，避 prose 误报）
    || /\.fail\s*\(/.test(content) // self.fail(...) / pytest.fail(...)
    // 261120 批 T2①：ESP-IDF Unity 宏断言（TEST_ASSERT_* / TEST_FAIL*，含 _MESSAGE 等全部后缀变体）。
    // 纯追加，既有六支模式与门义（拦空壳验收）不改；锚定策略与边界见上方 UNITY_ASSERT_RE 注记。
    || UNITY_ASSERT_RE.test(content);
}

function isShallowStatusOnly(content) {
  const checksStatus200 = /status(?:Code)?\)?\s*\.?(?:toBe|toEqual)?\s*\(?\s*200\b/i.test(content)
    || /\.status\s*\(\s*200\s*\)/i.test(content);
  if (!checksStatus200) return false;

  const hasBehaviorEvidence = /schema\.parse|safeParse|findUnique|findFirst|count\s*\(|emittedEvents|toContainEqual|locator\s*\(|toHaveText|toBeVisible|Audit|AiRun|correlationId|event/i.test(content);
  return !hasBehaviorEvidence;
}

// T2 自检（260906-gov-review-arq-governance-batch，ARQ-260902-acceptance-coverage-python-assert）：
// 固化 260902 Python 断言识别三支热修，正反例钉死防回腐。
// 已知边界（R1-F4 收束，如实声明）：hasRealAssertion 是纯正则、不剥注释/字符串——调用形态 assert 字样
// （如字符串 "assert(x)"、注释 # self.assertIn(a, b)）会被命中为「有真断言」，此为既有实现语义、本批不改
// （要剥注释 = 门语义变更，须另行走治理批 + human）。反例因此只钉 prose 形态。
// 261120 批 T2 扩充：① Unity 正负例（含 OQ-2 裁定 (c) 的两条 prose 负例与「仅行中调用」前向边界负例）；
// ② **消费层 CLI 夹具**（FROZEN §6 V4 / V5-A）——判别落在真实无参 spawn 形态，不只调内部函数
// （P8/P9 教训：夹具只打在判据计算层 → 自检全绿而闸消费层带洞）。既有 5 case 与预期逐字不改。
const SELF_PATH = resolve(process.argv[1] || '');
const AC_FIXTURE_ID = '269901-unity-probe';
const AC_FIXTURE_TEST_FILE = 'firmware/main/test/up.acceptance.c';

// 夹具文件一律 writeFileSync 构造：enforce-scope 的 universal.bashDenyWrite 按**路径字面**拦 Bash 重定向写
// delivery/taskpackages/current/taskpackage.json，即便目标是仓外临时目录（261120 批起草期实测被拦一次）。
function writeAcFixtureFile(dir, rel, body) {
  const abs = join(dir, rel);
  mkdirSync(dirname(abs), { recursive: true });
  writeFileSync(abs, body);
  return abs;
}

// canonical + current/ 双写（真仓 Pod 形态：无参读投影、--change-id 与位置参数读 canonical）。
// 本闸不做 git 调用，故夹具无需真 git 仓（与 change-manifest-check 的夹具不同）。
function makeAcFixture(content, opts = {}) {
  const dir = mkdtempSync(join(tmpdir(), 'ac-selfcheck-'));
  const tp = JSON.stringify({
    changeId: opts.changeId || AC_FIXTURE_ID,
    acceptance: opts.acceptance || ['UP-A1: unity 断言识别'],
    acceptanceTestsFile: [AC_FIXTURE_TEST_FILE],
  });
  writeAcFixtureFile(dir, `delivery/taskpackages/${AC_FIXTURE_ID}/taskpackage.json`, tp);
  writeAcFixtureFile(dir, 'delivery/taskpackages/current/taskpackage.json', tp);
  writeAcFixtureFile(dir, AC_FIXTURE_TEST_FILE, content);
  return dir;
}

// 真实消费层调用形态：零位置参数 + cwd 与 CLAUDE_PROJECT_DIR 同指夹具根
//（与 verify.mjs:271-272 对 timing:scoped 闸的调用同形态；FROZEN §6 V1 执行形态注、R1 NIT-3 措辞）。
function runAcInFixture(dir, args = []) {
  const run = spawnSync(process.execPath, [SELF_PATH, ...args], {
    cwd: dir, encoding: 'utf8', timeout: 60000, env: { ...process.env, CLAUDE_PROJECT_DIR: dir },
  });
  const stdout = String(run.stdout || '');
  const stderr = String(run.stderr || '');
  return { status: run.status, stdout, stderr, text: stdout + stderr };
}

// FROZEN §6 V4（7 类）+ V5-A（旗标面 5 条）；每条都以 CLI 入口判别。
function runCliSelfCheck() {
  const checks = [];
  const ok = (name, cond, detail) => checks.push({ name, pass: !!cond, ...(cond ? {} : { detail }) });
  const COV = '// coverage: UP-A1\n'; // coverage 标记先满足，被测的只是「有没有断言」这一问
  const cliCase = (name, content, expectStatus) => {
    const dir = makeAcFixture(content);
    try {
      const r = runAcInFixture(dir);
      ok(name, r.status === expectStatus, { status: r.status, expected: expectStatus, text: r.text.slice(0, 300) });
    } finally {
      try { rmSync(dir, { recursive: true, force: true }); } catch { /* best effort */ }
    }
  };

  // ── V4 表（第 3 列「修前实测」见 FROZEN §2.3 / §6 V4；此处断言修后预期）──
  cliCase('V4-1·纯 Unity 行首 TEST_ASSERT_*_MESSAGE → exit 0（修前 exit 1 假红）',
    COV + 'void test_up(void)\n{\n    TEST_ASSERT_EQUAL_MESSAGE(3, up_get(), "count");\n    TEST_ASSERT_TRUE(up_ok());\n}\n', 0);
  cliCase('V4-2·仅行首 TEST_FAIL_MESSAGE → exit 0（修前 exit 1 假红）',
    COV + 'void test_up(void)\n{\n    TEST_FAIL_MESSAGE("unexpected path reached");\n}\n', 0);
  cliCase('V4-3·仅 #define 宏包装断言 → exit 0（red-baseline/unity.h 形态；纯行首锚定会假红）',
    COV + '#define UP_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(up_locate_root(), "no root")\nvoid test_up(void)\n{\n    UP_REQUIRE_ROOT();\n}\n', 0);
  cliCase('V4-4·注释行内**带括号** Unity 字样 → exit 1（prose 负例保持红；(a) 无锚定变体会转绿）',
    COV + '// 说明：本文件应写 TEST_ASSERT_EQUAL_MESSAGE(3, x, "ctx") 但作者忘了\nvoid test_up(void)\n{\n    int x = 1;\n}\n', 1);
  cliCase('V4-5·字符串字面量内**带括号** Unity 字样 → exit 1（prose 负例保持红）',
    COV + 'void test_up(void)\n{\n    log_warn("remember to add TEST_ASSERT_TRUE(x)");\n}\n', 1);
  cliCase('V4-6·真无任何断言 → exit 1（识别面未放宽到恒真）',
    COV + 'void test_up(void)\n{\n    up_do_thing();\n}\n', 1);
  cliCase('V4-7a·Python self.assertIn → exit 0（既有识别面零回归）',
    COV + 'def test_up(self):\n    self.assertIn(b, a)\n', 0);
  cliCase('V4-7b·行首裸 assert → exit 0（既有识别面零回归）',
    COV + 'def test_up():\n    assert up_value() == 3\n', 0);

  // ── V5-A 旗标面 ──
  const dirF = makeAcFixture(COV + 'void test_up(void)\n{\n    TEST_ASSERT_TRUE(up_ok());\n}\n');
  try {
    const noArg = runAcInFixture(dirF);
    const viaFlag = runAcInFixture(dirF, ['--change-id', AC_FIXTURE_ID]);
    ok('V5-A·--change-id：解析 canonical 包且与无参同结论（exit 0）',
      viaFlag.status === 0 && noArg.status === 0, { noArg: noArg.status, viaFlag: viaFlag.status, text: viaFlag.text.slice(0, 300) });
    ok('V5-A·--change-id：stderr 回显来源 = --change-id 旗标',
      /来源：--change-id 旗标/.test(viaFlag.stderr), { stderr: viaFlag.stderr.slice(0, 200) });
    const viaPos = runAcInFixture(dirF, [`delivery/taskpackages/${AC_FIXTURE_ID}/taskpackage.json`]);
    ok('V5-A·位置参数兼容不回退：exit 0 + 回显来源 = 位置参数/环境变量',
      viaPos.status === 0 && /来源：位置参数\/环境变量/.test(viaPos.stderr), { status: viaPos.status, stderr: viaPos.stderr.slice(0, 200) });
    const bogus = runAcInFixture(dirF, ['--bogus-probe']);
    // 注：断言用 JSON message 形态而非裸字面——usage 文本自身为解释病灶引用了该短语，裸匹配会自相矛盾。
    ok('V5-A·未知旗标：非 0 退出 + usage 列出支持旗标集（修前 = exit 1 且报 "TaskPackage file does not exist" 错误话）',
      bogus.status !== 0 && /用法：/.test(bogus.text) && /--change-id/.test(bogus.text) && !/"message":\s*"TaskPackage file does not exist"/.test(bogus.text),
      { status: bogus.status, text: bogus.text.slice(0, 250) });
    const help = runAcInFixture(dirF, ['--help']);
    ok('V5-A·--help：exit 0 + usage（修前 = exit 1，--help 被当路径解析）',
      help.status === 0 && /用法：/.test(help.text), { status: help.status, text: help.text.slice(0, 250) });
    const typo = runAcInFixture(dirF, ['--change-id', 'no-such-pkg']);
    ok('V5-A·--change-id 指向不存在的包 → 非 0 且点名收到的值（不静默）',
      typo.status !== 0 && /no-such-pkg/.test(typo.text), { status: typo.status, text: typo.text.slice(0, 250) });
  } finally {
    try { rmSync(dirF, { recursive: true, force: true }); } catch { /* best effort */ }
  }

  const failures = checks.filter((c) => !c.pass);
  return { checks, pass: checks.length - failures.length, fail: failures.length, total: checks.length, failures };
}

function runSelfCheck() {
  const cases = [
    { name: 'python-unittest-positive', content: 'def test_x(self):\n    self.assertIn(b, a)\n', expect: true },
    { name: 'python-bare-assert-positive', content: 'def test_y():\n    assert resp.status_code == 200\n', expect: true },
    { name: 'prose-comment-negative', content: '# 断言风格说明：assertEqual 须在测试内调用\n', expect: false },
    { name: 'prose-string-negative', content: 'msg = "本次 assert 校验未通过"\n', expect: false },
    { name: 'empty-negative', content: '', expect: false },
    // ── 261120 批 T2① Unity 正负例（纯函数面；消费层判别在 runCliSelfCheck）──
    { name: 'unity-line-start-positive', content: 'void t(void)\n{\n    TEST_ASSERT_TRUE(up_ok());\n}\n', expect: true },
    { name: 'unity-message-suffix-positive', content: 'void t(void)\n{\n    TEST_ASSERT_EQUAL_MESSAGE(3, up_get(), "ctx");\n}\n', expect: true },
    { name: 'unity-test-fail-positive', content: 'void t(void)\n{\n    TEST_FAIL_MESSAGE("unexpected");\n}\n', expect: true },
    { name: 'unity-define-wrapper-positive', content: '#define UP_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(up_locate_root(), "no root")\n', expect: true },
    { name: 'unity-comment-call-shaped-negative', content: '// 应写 TEST_ASSERT_EQUAL_MESSAGE(3, x, "ctx")\nvoid t(void) { int x = 1; }\n', expect: false },
    { name: 'unity-string-call-shaped-negative', content: 'void t(void) { log_warn("add TEST_ASSERT_TRUE(x)"); }\n', expect: false },
    { name: 'unity-block-comment-negative', content: ' * TEST_ASSERT_EQUAL_UINT8_ARRAY is a 3-arg macro\n', expect: false },
    { name: 'unity-prose-no-paren-negative', content: '/* 判定走 TEST_ASSERT：abort 会带走整个 runner */\nvoid t(void) { up_run(); }\n', expect: false },
    // 前向边界（FROZEN §4.1 条 3 / R1 OQ-2 注记）：**仅**以行中调用承载断言 → 当下不识别（假红）。
    // 钉此负例是为了让「就地放宽锚定」必须显式改断言，而不是静默发生。
    { name: 'unity-midline-only-negative-forward-boundary', content: 'void t(void) { if (up_bad()) TEST_ASSERT_TRUE(up_ok()); }\n', expect: false },
  ];
  let pass = 0;
  const failures = [];
  for (const c of cases) {
    const got = hasRealAssertion(c.content);
    if (got === c.expect) pass += 1;
    else failures.push({ name: c.name, expect: c.expect, got });
  }
  const cli = runCliSelfCheck();
  // selfCheck 标签由 'acceptance-coverage-python-assert' 改为 'acceptance-coverage-assertion-and-flags'：
  // 覆盖面已不止 Python 断言（全仓无机械消费方解析该标签，261120 批起草期实测）。ok/pass/fail/total 键名不变。
  const result = {
    selfCheck: 'acceptance-coverage-assertion-and-flags',
    ok: failures.length === 0 && cli.fail === 0,
    pass: pass + cli.pass,
    fail: failures.length + cli.fail,
    total: cases.length + cli.total,
    assertionCases: { total: cases.length, pass, fail: failures.length },
    cliCases: { total: cli.total, pass: cli.pass, fail: cli.fail },
  };
  if (failures.length > 0) result.failures = failures;
  if (cli.failures.length > 0) result.cliFailures = cli.failures.map((c) => ({ name: c.name, detail: c.detail }));
  return result;
}

// ── 入口分派（先例 = mutation-check.mjs:243-258）：self-check → help → 未知旗标拒绝 → 主流程 ──
// 顺序承重：未知旗标必须先于主流程拒掉，否则 `--change-id` 一类旗标会被当位置参数路径解析（修前病灶）。
const argvTail = process.argv.slice(2);
if (process.argv.includes('--self-check')) {
  const result = runSelfCheck();
  print(result);
  process.exit(result.ok ? 0 : 1);
} else if (argvTail.includes('--help') || argvTail.includes('-h')) {
  console.log(ACCEPTANCE_COVERAGE_USAGE);
  process.exit(0);
} else {
  const unknown = argvTail.filter((token) => token.startsWith('--') && !KNOWN_FLAGS.has(token));
  if (unknown.length > 0) {
    console.error(`未知旗标：${unknown[0]}\n${ACCEPTANCE_COVERAGE_USAGE}`);
    process.exit(2);
  }
  if (process.argv.includes('--change-id') && flagVal('--change-id') === undefined) {
    console.error(`--change-id 缺值\n${ACCEPTANCE_COVERAGE_USAGE}`);
    process.exit(2);
  }
}

const pathToTaskPackage = taskPackagePath();
if (!pathToTaskPackage) {
  print({ ok: true, gate: 'acceptance-coverage', skipped: true, reason: 'no TaskPackage context' });
  process.exit(0);
}

if (!existsSync(pathToTaskPackage)) {
  fail('TaskPackage file does not exist', { taskPackage: pathToTaskPackage });
}

const taskPackage = readJson(pathToTaskPackage);
const acceptanceRaw = (Array.isArray(taskPackage.acceptance) ? taskPackage.acceptance : [])
  .map(acceptanceId)
  .filter(Boolean);
const acceptance = acceptanceRaw.map((id) => shortAcceptanceRef(id) || id);
const files = Array.isArray(taskPackage.acceptanceTestsFile) ? taskPackage.acceptanceTestsFile : [];
const { structural, suiteGates } = structuralAcceptanceIds(taskPackage, acceptance);
const exempt = new Set([...structural, ...suiteGates]);
const requiresAcceptanceFiles = acceptance.some((id) => !exempt.has(id));

if (acceptance.length === 0) {
  fail('TaskPackage.acceptance must list at least one acceptance/source ref', { taskPackage: pathToTaskPackage });
}

if (requiresAcceptanceFiles && files.length === 0) {
  fail('TaskPackage.acceptanceTestsFile must list frozen acceptance test files', { taskPackage: pathToTaskPackage });
}

const missingFiles = [];
const fileContents = [];
for (const file of files) {
  const absolutePath = resolve(projectDir, file);
  if (!existsSync(absolutePath)) {
    missingFiles.push(file);
    continue;
  }

  const content = readFileSync(absolutePath, 'utf8');
  fileContents.push({ file, content });

  if (!hasRealAssertion(content)) {
    fail('Acceptance test file has no recognizable assertion', { file });
  }

  if (isShallowStatusOnly(content)) {
    fail('Acceptance test appears to assert only HTTP 200 without behavior evidence', { file });
  }
}

if (missingFiles.length > 0) {
  fail('Acceptance test files are missing', { missingFiles });
}

const missingCoverage = [];
for (const acceptanceId of acceptance) {
  if (exempt.has(acceptanceId)) continue;
  const covered = fileContents.some(({ content }) => hasCoverageMarker(content, acceptanceId));
  if (!covered) missingCoverage.push(acceptanceId);
}

if (missingCoverage.length > 0) {
  fail('Some acceptance ids are not covered by frozen acceptance tests', { missingCoverage });
}

print({
  ok: true,
  gate: 'acceptance-coverage',
  taskPackage: pathToTaskPackage,
  acceptance,
  structural: [...structural],
  suiteGates: [...suiteGates],
  files,
});
