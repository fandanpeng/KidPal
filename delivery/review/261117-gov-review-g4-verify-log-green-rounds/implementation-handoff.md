# 实施交接件 · 261117-gov-review-g4-verify-log-green-rounds

> **性质：作者预备的实施输入，不是字节合同。** 261029 批已撤销 packet-bound 逐字节搬运；本文件存在的唯一原因是**机械阻塞**（见下节）——作者本会话的树根不是 gov worktree，`enforce-scope.mjs` fail-closed 拦住了 `.claude/hooks/**` 的跨树写入。
> **实施会话的义务不变**：必须在 `C:/AI/KidPal-gov-g4`（分支 `gov/g4-verify-log-fix`）自己落笔、自己重跑本文件「已验证」列出的全部自测、自己落实施证据并自提 commit。**不得**凭本文件宣称通过，也不得把本文件当作 FROZEN 的字节承诺（FROZEN v2 冻的是意图 / 边界 / 关键方案 / 验收标准）。
> 本文件与 `PROPOSAL.md` 同属证据尾（`isEvidenceTailPath` 豁免），不入轻量 scope 对账。

## 1. 为什么需要交接（机械阻塞，逐字凭据）

作者在 `c:/AI/KidPal-pod-m2` 根的会话里对 `C:/AI/KidPal-gov-g4/.claude/hooks/verify.mjs` 发起 Edit，被 PreToolUse 钩子拦截：

```
PreToolUse:Edit hook error: [node "${CLAUDE_PROJECT_DIR}/.claude/hooks/enforce-scope.mjs"]:
scope(architect): scripts/hooks 作者实现只允许当前 gov worktree，禁止跨树或不可识别树写入。
```

判据在 `.claude/hooks/enforce-scope.mjs`：

- `:301-304` —— `agentKey === 'architect'` 且目标匹配 `^\.claude\/(?:scripts|hooks)\//` 时调 `architectNonMainViolation(abs, projectDir)`；
- `:168-173` —— `home = locateGitTree(homeProjectDir)`（= 会话的 `CLAUDE_PROJECT_DIR`）与 `target = locateGitTree(absPath)` 必须 `samePath(treeRoot)`，否则 fail-closed 报上述文案；
- `:243-249` —— 同一约束覆盖 Bash：`git -C/--git-dir/--work-tree` 跨树 `add`/`commit` 直接 block，且非跨树形态也要 `architectNonMainViolation(projectDir, projectDir)` 通过。

结论：本批的 `.claude/hooks/verify.mjs` 实施与 commit **必须**在树根为 `C:/AI/KidPal-gov-g4` 的会话内完成（该树分支 `gov/g4-verify-log-fix`，非 main，满足 `:184-187`）。作者未以任何方式绕过（无 Bash 重定向写 `.claude/**`、无 `git -C`、未改 settings / scope-map / 钩子）。

## 2. 基线钉点

| 项 | 值 |
|---|---|
| gov worktree | `C:/AI/KidPal-gov-g4`，分支 `gov/g4-verify-log-fix`，基 commit `390a7297` |
| 实施前 `.claude/hooks/verify.mjs` blob | `git hash-object` = `516b8da00119f447a5676b8149aca80e26c47327`（与 reviewer R2 §11 记录值一致；`pod-m2` 树同一 blob，两树无分歧） |
| 工作树行尾 | **CRLF**（`git show HEAD:` 导出的是 LF，**不可**用作逐字节基线；基线必须取工作树文件或经 CRLF 归一化） |
| 作者验证后的成品 blob（仓外 harness） | `6efe0e759712b19fa96b08bb26eb5aa351a4e2d4`（仅供参考；实施会话以自己落笔后的实际 blob 为准） |

## 3. 四处编辑（锚点唯一，逐个断言恰一次命中）

> 全部为**纯新增**，唯一被替换的既有行是 import 的 `node:path` 那一行。三处保护区（`writeGateLog` 函数体、`if (!ok) {…}` 整段、循环后全红清单块）**零字符变化**。

### 编辑 1 · 文件头 import（为 self-check 两案服务）

锚点（旧，恰一次）：

```js
import {
  appendFileSync,
  closeSync,
  existsSync,
  fsyncSync,
  linkSync,
  mkdirSync,
  openSync,
  readFileSync,
  unlinkSync,
  writeFileSync,
} from 'node:fs';
import { dirname, resolve } from 'node:path';
```

替换为：

```js
import {
  appendFileSync,
  closeSync,
  existsSync,
  fsyncSync,
  linkSync,
  mkdirSync,
  mkdtempSync,
  openSync,
  readFileSync,
  readdirSync,
  rmSync,
  statSync,
  unlinkSync,
  writeFileSync,
} from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { tmpdir } from 'node:os';
```

### 编辑 2 · 绿分支 best-effort gate log 写（主修）

锚点（旧，恰一次；即 `if (!ok) {…}` 的收尾 + `for` 循环的收尾）：

```js
      console.log(JSON.stringify({ ok: false, results, error, phase }, null, 2));
      process.exit(1);
    }
  }
```

替换为（**注意：另起独立 `if (ok) {` 块，绝不用 `} else {`** —— R2 §11.4 实现防呆，用 else 会把 `:309` 的 `    }` 变成 `    } else {`，当场触 FROZEN §5 B2 第 1 项）：

```js
      console.log(JSON.stringify({ ok: false, results, error, phase }, null, 2));
      process.exit(1);
    }

    // ── G4 产线修复（261117-gov-review-g4-verify-log-green-rounds，FROZEN v2 §3.1/§3.2）──
    // 绿闸也落一份 gate log：G4 / G6 / evidenceComplete 要证的是「闸跑过」，不是「闸红过」。此前
    // writeGateLog 的唯一调用点在上方 !ok 分支内，导致「从第一轮起全绿」的 Pod 目录从未创建 →
    // supervisor-gate-check G4 恒红（:719）、supervisor-verdict-gate evidenceComplete 恒 false（:698）、
    // G6 verifyLogRefs 空数组恒 block（:324 + validateRefArray 空数组判据）。
    // 形状纪律（设计审 R2 §11.4 实现防呆）：另起独立块，**不用 `} else {`**——else 会改动上方 if (!ok)
    // 的收尾行，直接触 FROZEN §5 B2 第 1 项（:288-309 除位置外无字符变化）。红路径不会落到本块：
    // final 相位红闸在 !ok 内 continue、scoped 相位红闸在 !ok 内 process.exit(1)，故不存在重复写。
    // best-effort：gate log 属非承重投影（承重件 = round fact，其 publishFact / validateFactObject 的
    // fail-loud 路径零改），写失败不得把一轮绿改判为红——同本文件「Non-bearing projection/observability
    // cannot change published 0/1」与 persistVerifyRuns 的 fail-open 惯例；但写失败必须在 stderr 点名，不静默。
    if (ok) {
      try {
        writeGateLog(gate, out);
      } catch (logError) {
        process.stderr.write(
          `[verify.mjs] gate log 写失败（非致命：本轮 ok 与 exit code 不变；G4/G6 证据缺口，收尾门可能红）：` +
            `gate=${gate.id} phase=${phase} reason=${logError && logError.message ? logError.message : String(logError)}\n`,
        );
      }
    }
  }
```

### 编辑 3 · self-check 两案（hermetic tmpdir 沙箱）

锚点（旧，恰一次）：

```js
function selfCheck() {
```

替换为「battery 函数 + 空行 + 原锚点行」：

```js
// ── G4 产线修复两案（261117-gov-review-g4-verify-log-green-rounds，FROZEN v2 §3.7）──
// hermetic：tmpdir 沙箱自造最小 verify-gates.json（恒绿 / 恒红闸各一条）+ CLAUDE_PROJECT_DIR 指向沙箱、
// 直跑 --gate-child（不经父进程）→ 零仓内副作用：不写 delivery/taskpackages/current/verdict.json、
// 不 append verify-runs.jsonl、不产 round fact。
// 闸命令用沙箱内落地的 .mjs 小脚本而非 `node -e "…"`：避开 Windows cmd.exe 引号脆性（FROZEN §3.7 实施风险提示）。
// 案名逐条输出（PASS / FAIL 各一行）= 真空绿规约：无案名 = 未执行（FROZEN §5 A1 判据）。
// 案 B 判别力范围（FROZEN §3.7 / 设计审 R1 F2）：只证「正常红轮 ErrorEnvelope 六字段形状回归」；
// 红轮**写失败**语义零变化不由本案承载，由 §5 B2 的 diff 逐字节核读承载（R2 §11.3 明裁不补故障注入臂）。
function runGateLogBattery(check) {
  const cases = [];
  const assert = (name, cond, detail = '') => cases.push({ name, ok: Boolean(cond), detail });
  const root = mkdtempSync(join(tmpdir(), 'verify-g4log-'));
  try {
    const makeSandbox = (gateId, exitCode, marker) => {
      const dir = join(root, gateId);
      mkdirSync(join(dir, '.claude'), { recursive: true });
      writeFileSync(join(dir, `${gateId}.mjs`), `console.log('${marker}');\nprocess.exit(${exitCode});\n`, 'utf8');
      writeFileSync(
        join(dir, '.claude', 'verify-gates.json'),
        JSON.stringify({ gates: [{ id: gateId, kind: 'governance', cmd: `node ${gateId}.mjs`, timing: 'scoped' }] }, null, 2),
        'utf8',
      );
      return dir;
    };
    const runGateChildIn = (dir) => spawnSync(process.execPath, [scriptPath, '--gate-child', '--phase=scoped'], {
      cwd: dir, encoding: 'utf8', timeout: 60000, env: { ...process.env, CLAUDE_PROJECT_DIR: dir },
    });
    const payloadOf = (child) => { try { return JSON.parse(child.stdout || '{}'); } catch { return {}; } };
    const logsOf = (dir) => {
      const logsDir = join(dir, '.claude', 'verify-logs');
      if (!existsSync(logsDir)) return [];
      return readdirSync(logsDir)
        .filter((f) => f.endsWith('.log'))
        .map((f) => ({ name: f, size: statSync(join(logsDir, f)).size, text: readFileSync(join(logsDir, f), 'utf8') }));
    };

    // 案 A：全绿轮次也落 gate log（G4 / G6 证据产线本体）
    {
      const dir = makeSandbox('probe-green', 0, 'probe-green-marker');
      const child = runGateChildIn(dir);
      const payload = payloadOf(child);
      const logs = logsOf(dir);
      assert(
        'green-round-writes-gate-log',
        child.status === 0 && payload.ok === true && logs.length >= 1
          && logs.some((l) => l.size > 0 && l.text.includes('gate=probe-green') && l.text.includes('probe-green-marker')),
        `exit=${child.status} ok=${payload.ok} logs=${logs.length} stderr=${String(child.stderr || '').slice(0, 160)}`,
      );
    }

    // 案 B：正常红轮 ErrorEnvelope 形状回归（logFile / logTail / fingerprint / failingStep / category / cmd）
    {
      const dir = makeSandbox('probe-red', 3, 'probe-red-failure-marker');
      const child = runGateChildIn(dir);
      const payload = payloadOf(child);
      const envelope = payload.error || {};
      const logs = logsOf(dir);
      const refParts = typeof envelope.logFile === 'string' ? envelope.logFile.split('/') : [];
      const refAbs = refParts.length > 0 ? join(dir, ...refParts) : null;
      assert(
        'red-round-envelope-unchanged',
        child.status === 1 && payload.ok === false
          && envelope.name === 'GateFailure' && envelope.failingStep === 'probe-red'
          && envelope.category === 'guardrail' && envelope.cmd === 'node probe-red.mjs'
          && typeof envelope.fingerprint === 'string' && /^[0-9a-f]{12}$/.test(envelope.fingerprint)
          && String(envelope.message || '').includes('exit 3')
          && refAbs !== null && String(envelope.logFile).startsWith('.claude/verify-logs/')
          && existsSync(refAbs) && statSync(refAbs).size > 0
          && String(envelope.logTail || '').includes('probe-red-failure-marker')
          && logs.length === 1,
        `exit=${child.status} envelope=${JSON.stringify(envelope).slice(0, 220)}`,
      );
    }
  } finally {
    try { rmSync(root, { recursive: true, force: true }); } catch { /* best-effort cleanup */ }
  }
  for (const c of cases) {
    console.log(`  [${c.ok ? 'PASS' : 'FAIL'}] ${c.name}${c.ok ? '' : ` (${c.detail})`}`);
    check(c.name, c.ok);
  }
}

function selfCheck() {
```

### 编辑 4 · 在 selfCheck 内接线（汇总 JSON 行字节不动）

锚点（旧，恰一次）：

```js
  console.log(JSON.stringify({ selfCheck: 'verify.mjs', pass: pass.length, fail: fail.length, total: pass.length + fail.length, ok: fail.length === 0, failures: fail.length ? fail : undefined }));
```

替换为：

```js
  // G4 产线修复两案（261117-gov-review-g4-verify-log-green-rounds）：hermetic tmpdir 沙箱，
  // 案名逐条输出（真空绿规约，FROZEN §5 A1）；计数仍并入下方同一 JSON 汇总行（汇总行字节不动）。
  runGateLogBattery(check);
  console.log(JSON.stringify({ selfCheck: 'verify.mjs', pass: pass.length, fail: fail.length, total: pass.length + fail.length, ok: fail.length === 0, failures: fail.length ? fail : undefined }));
```

## 4. 可选项（manifest 第二行，非必要）

`.claude/references/dev-loop.md` file-map 的 `hooks/verify.mjs` 行（编辑前 `:83`）现为：

```
| `hooks/verify.mjs` | 确定性验证执行器：按 verify-gates 跑闸门，产出与 K6 对齐的 ErrorEnvelope。 |
```

若改，建议追加一句（**改与不改都合规**，R1 §9.2 第 8 条 / N5 + R2 未反对；不改无需任何说明）：

```
| `hooks/verify.mjs` | 确定性验证执行器：按 verify-gates 跑闸门，产出与 K6 对齐的 ErrorEnvelope；**每闸每轮**落一份 gate log 到 `.claude/verify-logs/`（绿轮也落 = supervisor G4 / G6 `verifyLogRefs` / `evidenceComplete` 的证据产线，261117 批修复「仅红闸才落」的缺口）。 |
```

## 5. 实施会话必须自跑的验证（作者已在仓外 harness 跑过，结果见 §6；实施会话须自跑，不得引用）

1. **self-check**：`node .claude/hooks/verify.mjs --self-check` → exit 0、`ok:true`、`total:18`（原 16 + 新 2），且 stdout 逐条出现 `[PASS] green-round-writes-gate-log` 与 `[PASS] red-round-envelope-unchanged`（FROZEN §5 A1）。
2. **A2 判别力演示**：临时移除绿分支的 `writeGateLog(gate, out);` → 案 A 必须 FAIL（`logs=0`）、案 B 仍 PASS、self-check exit 1；随后 `git checkout -- .claude/hooks/verify.mjs`（或从备份还原）并以 `git hash-object` 核对已还原到实施 commit 的 blob。
3. **B2 逐字节核读**：`git diff -U0 main...HEAD -- .claude/hooks/verify.mjs` → **删除行必须只有 1 行**（`import { dirname, resolve } from 'node:path';`），hunk 只应落在 import 块、绿分支插入点、battery 函数、selfCheck 接线四处；三处保护区零删除零修改。可用 §7 的 `b2check.mjs` 做机械等值核（比人工核读更强）。
4. **B3**：由 B2 第 1+2 项承载（FROZEN §5 B3），批记录写明「B3 by B2 第 1+2 项」。
5. **A3 真实端到端**：**只能在一次性临时 worktree / clone 或仓外沙箱里跑**，禁在 gov worktree 跑（`verify.mjs` 会写入 tracked 的 `delivery/taskpackages/current/verdict.json` 与 `delivery/taskpackages/<current changeId>/verify-runs.jsonl` → 出 scope）。作者已用仓外多闸沙箱覆盖四形态，见 §6。
6. **A4**：`node .claude/hooks/supervisor-gate-check.mjs --self-check` → 仍 exit 0（该文件零改，属旁证）。
7. **C1/C2/C3**：`git diff --stat main...HEAD` 必须只含 `.claude/hooks/verify.mjs`（+ 可选 `.claude/references/dev-loop.md`）与本批证据尾；`supervisor-gate-check.mjs` / `supervisor-verdict-gate.mjs` / `verify-gates.json` / `persistVerifyRuns` 段零 diff。
8. **提交**：`git add` 后先 `git diff --cached --stat` 核面，再提真实实现 commit（非 main 分支）；随后在 PROPOSAL append 实施证据段（作者可写自己的实施证据，不改 reviewer 段、不自签 verdict、不写 `reviewedCommit` / `reviewedTree`）。

## 6. 作者已完成的仓外验证（harness = `/tmp/g4impl`，一次性、非仓内、已随会话消失；下列为实测输出）

**基线**：harness 内未改的 `verify.mjs`（blob `516b8da0…`）`--self-check` → `{"selfCheck":"verify.mjs","pass":16,"fail":0,"total":16,"ok":true}`，exit 0。

**改后 self-check**（FROZEN §5 A1）：

```
  [PASS] green-round-writes-gate-log
  [PASS] red-round-envelope-unchanged
{"selfCheck":"verify.mjs","pass":18,"fail":0,"total":18,"ok":true}
exit=0
```

**A2 判别力演示**（移除绿分支写后）：

```
  [FAIL] green-round-writes-gate-log (exit=0 ok=true logs=0 stderr=)
  [PASS] red-round-envelope-unchanged
{"selfCheck":"verify.mjs","pass":17,"fail":1,"total":18,"ok":false,"failures":["green-round-writes-gate-log"]}
exit=1
```

**案 B 判别力补充演示**（把红分支 `logFile,` 改为 `logFile: null,`，即破坏 ErrorEnvelope 字段）：

```
  [PASS] green-round-writes-gate-log
  [FAIL] red-round-envelope-unchanged (exit=1 envelope={"name":"GateFailure",…,"logFile":null,…})
{"selfCheck":"verify.mjs","pass":17,"fail":1,"total":18,"ok":false,"failures":["red-round-envelope-unchanged"]}
exit=1
```

两次演示后均从备份还原，`git hash-object` 复核 = `6efe0e759712b19fa96b08bb26eb5aa351a4e2d4`（与演示前一致）。

**B2 机械等值核**（`b2check.mjs`，基线 = gov worktree 未改文件 CRLF 形态，blob `516b8da0…`）：

```
IDENTICAL | R1 writeGateLog 函数体        | before=05a37f6e49e4fb94 (515B, 起行 191) after=05a37f6e49e4fb94 (515B, 起行 196) | 行数 14
IDENTICAL | R2 if (!ok) {…} 整段分支体    | before=c632d016b5c7e9d8 (842B, 起行 288) after=c632d016b5c7e9d8 (842B, 起行 293) | 行数 23
IDENTICAL | R3 循环后全红清单块           | before=7cc94d8965b451bf (458B, 起行 312) after=7cc94d8965b451bf (458B, 起行 339) | 行数 8
B2 三处保护区：逐字节全等（PASS）
```

三处区间与 FROZEN §5 B2 指定的 `:191-203` / `:288-309` / `:312-318` 一一对应（行数 14 / 23 / 8 = 区间行数 + 尾随换行）。

**diff 删除行普查**（`git diff --no-index -U0`）：`1 file changed, 113 insertions(+), 1 deletion(-)`；唯一删除行 = `-import { dirname, resolve } from 'node:path';`；六个 hunk 头 = `@@ -12,0 +13 @@`、`@@ -14,0 +16,3 @@`、`@@ -18 +22,2 @@`（以上 import 块）、`@@ -309,0 +315,22 @@ function runGateChild()`（绿分支）、`@@ -455,0 +483,82 @@ function runBearing()`（battery 函数）、`@@ -493,0 +603,3 @@ function selfCheck()`（接线）。

**A3 等价端到端**（仓外多闸沙箱、legacy 父模式，`CLAUDE_PROJECT_DIR` 指向沙箱）：

| 形态 | 命令 | exit | `.log` 数（非空） | 实跑闸 |
|---|---|---|---|---|
| 全绿 scoped | `--phase=scoped` | 0 | 3（3） | `g1:ok,g2:ok,g3:ok` |
| 中间红 scoped | `--phase=scoped` | 1 | 2（2） | `g1:ok,g2:RED`（`g3` 未跑 = 遇红即停不变） |
| final 相位（含 `timing=full` 闸） | `--phase=final --scope-file=scope.txt` | 0 | 2（2） | `g1:ok,g2:ok` |
| 同沙箱 scoped（清目录后） | `--phase=scoped` | 0 | 1（1） | `g1`（`timing=full` 闸被跳过 = gate 选择不变） |

四形态 stderr 均无「gate log 写失败」点名（best-effort 分支未触发）。绿 log 与红 log 内容形状完全一致，头部仍是四行：

```
gate=g1
kind=governance
cmd=node g1.mjs
phase=scoped

g1-marker
```

**副作用观察（顺带实证 FROZEN §3.4）**：同一沙箱目录跨次运行只增不减（第 4 形态首次跑出 3 个 log = 上次遗留 2 + 本次 1），与「目录 gitignored、从不清理、`collectVerifyLogs` 取全目录」的记账一致；清目录后重跑即回到 1。

## 7. `b2check.mjs`（机械等值核，供实施会话复用；放仓外任意目录跑，勿写入仓内）

```js
import { readFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
const before = readFileSync(process.argv[2], 'utf8');
const after = readFileSync(process.argv[3], 'utf8');
const eol = before.includes('\r\n') ? '\r\n' : '\n';
const sha = (s) => createHash('sha1').update(s, 'utf8').digest('hex').slice(0, 16);
function region(text, startAnchor, endAnchor, label) {
  const i = text.indexOf(startAnchor);
  if (i < 0) throw new Error(`${label}: start anchor not found`);
  if (text.indexOf(startAnchor, i + 1) >= 0) throw new Error(`${label}: start anchor not unique`);
  const j = text.indexOf(endAnchor, i + startAnchor.length);
  if (j < 0) throw new Error(`${label}: end anchor not found`);
  return text.slice(i, j + endAnchor.length);
}
const E = eol;
const specs = [
  ['R1 writeGateLog 函数体', `function writeGateLog(gate, output) {`, `${E}}${E}`],
  ['R2 if (!ok) {…} 整段分支体', `    if (!ok) {`, `      process.exit(1);${E}    }${E}`],
  ['R3 循环后全红清单块', `  if (failures.length > 0) {`, `${E}  }${E}`],
];
let allOk = true;
for (const [label, s, e] of specs) {
  const b = region(before, s, e, label);
  const a = region(after, s, e, label);
  const identical = b === a;
  if (!identical) allOk = false;
  const beforeLine = before.slice(0, before.indexOf(s)).split(E).length;
  const afterLine = after.slice(0, after.indexOf(s)).split(E).length;
  console.log(`${identical ? 'IDENTICAL' : 'DIFFERS  '} | ${label} | before=${sha(b)} (${b.length}B, 起行 ${beforeLine}) after=${sha(a)} (${a.length}B, 起行 ${afterLine}) | 行数 ${b.split(E).length}`);
}
console.log(allOk ? 'B2 三处保护区：逐字节全等（PASS）' : 'B2 FAIL');
process.exit(allOk ? 0 : 1);
```

跑法（Git Bash，注意 node 需 Windows 路径）：

```bash
cp .claude/hooks/verify.mjs /tmp/verify-before.mjs   # 实施前先存基线（工作树 CRLF 形态，勿用 git show）
# …实施四处编辑…
node /path/to/b2check.mjs "$(cygpath -w /tmp/verify-before.mjs)" "$(cygpath -w .claude/hooks/verify.mjs)"
```
