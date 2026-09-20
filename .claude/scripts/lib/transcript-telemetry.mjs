#!/usr/bin/env node
// 会话 transcript 遥测读取库（只读派生，无副作用）。
//
// PACKAGE: 260920-gov-review-dispatch-telemetry-source（Track B，FROZEN v1.0）
// OWNER: architect（G3 治理面脚本）
//
// 背景（PROPOSAL §0）：`dispatch-completed` 事件长期只记 harness 回传的单值
// （口径未文档化、与逐消息账对不上），主线程自身消耗零观测。本库引入第二条只读
// 数据源——Claude Code 落在磁盘上的会话 transcript，逐消息带 `message.usage`
// 四路分项与 `message.model`（实际生效模型）。
//
// **该通道是仓外、未文档化的 harness 内部产物**（本机实测 2.1.276）。故三条硬约束：
//   ① 失败即降级、绝不抛出——取不到就如实报 found:false，绝不禁用派发、绝不阻断 stamp；
//   ② 路径可覆盖（KIDPAL_CLAUDE_PROJECTS_DIR），供夹具与异常环境使用；
//   ③ 不做模型名白名单——代理层命名会漂（实测 deepseek-flash / deepseek-v4-flash 并存），
//      只透传字符串，不校验、不映射。
//
// **权威性界定**（R1-C1）：本库读出的是**仓内 token 归因最忠实记录**（模型 API 回传的
// 逐消息 usage），**不是计费真值**——provider 计费可因缓存折扣等与之不同。计费对账不在本批范围。
//
// 用法：
//   node .claude/scripts/lib/transcript-telemetry.mjs --self-check
//
// 被 dispatch-stamp.mjs 消费（post-cutoff 分支）。

import { existsSync, readFileSync, readdirSync, mkdtempSync, mkdirSync, writeFileSync, rmSync } from 'node:fs';
import { resolve, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { tmpdir, homedir } from 'node:os';

// ── 路径根（每次调用读 env，便于夹具覆盖）──────────────────────────────────
export function projectsRoot() {
  return process.env.KIDPAL_CLAUDE_PROJECTS_DIR || join(homedir(), '.claude', 'projects');
}

// ── 谓词（R2-N4 折入：钉死勿改）───────────────────────────────────────────
// transcript 里三个字段并存且**值不同名**：顶层 `o.type`、`message.role`、`message.type`。
// **`message.type` 的值是 "message"、不是 "assistant"**——误取它计数恒 0，且不抛错、不报红，
// 直接产出「主线程零消耗」这个看似合理的错误结论。实测：同一文件按 `message.type === 'assistant'`
// 计数 = 0，按 `o.type === 'assistant'` 计数 = 6。
function isAssistantRecord(o) {
  return !!o && o.type === 'assistant' && !!o.message;
}

function usageOf(o) {
  const u = o && o.message && o.message.usage;
  return u && typeof u === 'object' ? u : null;
}

// 按 message.id 去重（E3 实测陷阱）：同一次 API 响应的 usage 会按 content block 落多遍
// （`apiBlockIndex` 0/1…），数值完全相同。不去重即按块数虚增。
// 无 message.id 的记录**不参与去重**（无法判定归属），按独立条目计入——fail-open 取真值而非丢数据。
function dedupeByMessageId(records) {
  const byId = new Map();
  const noId = [];
  for (const r of records) {
    const id = r && r.message && r.message.id;
    if (typeof id !== 'string' || id === '') { noId.push(r); continue; }
    if (!byId.has(id)) byId.set(id, r); // 首见即定，后续同 id 的块共享同一 usage
  }
  return [...byId.values(), ...noId];
}

function readJsonlRecords(path) {
  const text = readFileSync(path, 'utf8');
  const records = [];
  for (const line of text.split(/\r?\n/)) {
    if (!line.trim()) continue;
    try { records.push(JSON.parse(line)); } catch { /* 残缺行跳过（fail-open，仅丢该行） */ }
  }
  return records;
}

function accumulate(records) {
  const agg = { input: 0, output: 0, cacheRead: 0, cacheWrite: 0, model: null };
  let counted = 0;
  for (const r of records) {
    const u = usageOf(r);
    if (!u) continue;
    agg.input += typeof u.input_tokens === 'number' ? u.input_tokens : 0;
    agg.output += typeof u.output_tokens === 'number' ? u.output_tokens : 0;
    agg.cacheRead += typeof u.cache_read_input_tokens === 'number' ? u.cache_read_input_tokens : 0;
    agg.cacheWrite += typeof u.cache_creation_input_tokens === 'number' ? u.cache_creation_input_tokens : 0;
    const m = r.message && r.message.model;
    if (typeof m === 'string' && m) agg.model = m; // 取最后观测到的（最新）
    counted++;
  }
  agg.total = agg.input + agg.output; // 政策：total = input + output，cache 两路不并入
  agg.messageCount = counted;
  return agg;
}

// ── 派发侧：按 agentId 跨全部项目目录定位 ──────────────────────────────────
// E8 实测：worktree 池复用会让 transcript 落在**另一个** worktree 的会话目录下，
// 故**不可假设同目录**、须全目录扫。
export function locateDispatchTranscript(agentId) {
  if (typeof agentId !== 'string' || !/^[A-Za-z0-9_-]+$/.test(agentId)) return null; // 防路径穿越 + 自造 id 快速失败
  const root = projectsRoot();
  if (!existsSync(root)) return null;
  let projectDirs;
  try { projectDirs = readdirSync(root, { withFileTypes: true }); } catch { return null; }
  for (const d of projectDirs) {
    if (!d.isDirectory()) continue;
    const slugDir = join(root, d.name);
    let sessions;
    try { sessions = readdirSync(slugDir, { withFileTypes: true }); } catch { continue; }
    for (const s of sessions) {
      if (!s.isDirectory()) continue;
      const candidate = join(slugDir, s.name, 'subagents', `agent-${agentId}.jsonl`);
      if (existsSync(candidate)) {
        return { path: candidate, projectDirName: d.name, sessionDirName: s.name };
      }
    }
  }
  return null;
}

export function readDispatchTelemetry(agentId) {
  try {
    const loc = locateDispatchTranscript(agentId);
    if (!loc) return { found: false, reason: 'transcript-not-found' };
    const all = readJsonlRecords(loc.path); // 只读一次（transcript 可达数 MB）
    const records = dedupeByMessageId(all.filter(isAssistantRecord));
    if (records.length === 0) return { found: false, reason: 'no-assistant-records' };
    const agg = accumulate(records);
    // sessionId 取记录内的字段（自洽锚），缺则回退目录名
    const sample = all.find(r => typeof r.sessionId === 'string');
    return {
      found: true,
      source: 'transcript',
      path: loc.path,
      projectDirName: loc.projectDirName,
      sessionId: (sample && sample.sessionId) || loc.sessionDirName,
      input: agg.input,
      output: agg.output,
      cacheRead: agg.cacheRead,
      cacheWrite: agg.cacheWrite,
      total: agg.total,
      model: agg.model,
      messageCount: agg.messageCount,
    };
  } catch (e) {
    return { found: false, reason: `read-error: ${e.message}` };
  }
}

// ── 主线程侧：按 sessionId 读主会话，累计 ─────────────────────────────────
// 过滤 `isSidechain !== true`（主线程消息）+ 顶层 `o.type === 'assistant'`（R2-N4）。
export function readMainThreadUsage(sessionId, projectDirName) {
  try {
    if (typeof sessionId !== 'string' || !sessionId) return { found: false, reason: 'no-session-id' };
    if (typeof projectDirName !== 'string' || !projectDirName) return { found: false, reason: 'no-project-dir' };
    const path = join(projectsRoot(), projectDirName, `${sessionId}.jsonl`);
    if (!existsSync(path)) return { found: false, reason: 'main-transcript-not-found' };
    const all = readJsonlRecords(path);
    const principal = all.filter(r => isAssistantRecord(r) && r.isSidechain !== true);
    const records = dedupeByMessageId(principal);
    const agg = accumulate(records);
    return {
      found: true,
      source: 'transcript',
      input: agg.input,
      output: agg.output,
      cacheRead: agg.cacheRead,
      cacheWrite: agg.cacheWrite,
      total: agg.total,
      model: agg.model,
      messageCount: agg.messageCount,
    };
  } catch (e) {
    return { found: false, reason: `read-error: ${e.message}` };
  }
}

// ── 自检（夹具；真空绿规约：先断言夹具输入非空，空夹具判 FAIL）──────────────
function rec(id, { input = 0, output = 0, cacheRead = 0, cacheWrite = 0, model = 'x', sidechain = false, blocks = 1 } = {}) {
  return Array.from({ length: blocks }, () => ({
    type: 'assistant',
    isSidechain: sidechain,
    sessionId: 'sess-1',
    message: {
      id,
      type: 'message',       // 陷阱字段：值恒为 "message"，误用它计数为 0
      role: 'assistant',
      model,
      usage: {
        input_tokens: input,
        output_tokens: output,
        cache_read_input_tokens: cacheRead,
        cache_creation_input_tokens: cacheWrite,
      },
    },
  }));
}

function writeJsonl(path, records) {
  mkdirSync(resolve(path, '..'), { recursive: true });
  writeFileSync(path, records.map(r => JSON.stringify(r)).join('\n') + '\n');
}

export function selfCheck() {
  const dir = mkdtempSync(join(tmpdir(), 'kidpal-transcript-telemetry-'));
  const prev = process.env.KIDPAL_CLAUDE_PROJECTS_DIR;
  const cases = [];
  const add = (name, fn) => { try { cases.push({ name, ok: !!fn() }); } catch (e) { cases.push({ name, ok: false, error: e.message }); } };

  try {
    const slug = 'fixture-proj';
    const sess = 'sess-1';
    const subDir = join(dir, slug, sess, 'subagents');
    mkdirSync(subDir, { recursive: true });

    // 派发夹具：两条消息，其中一条落 3 个块（同 message.id）——去重后 input 应 = 10+20，不是 10+20*3
    writeJsonl(join(subDir, 'agent-aaa.jsonl'), [
      ...rec('m1', { input: 10, output: 1, cacheRead: 100, cacheWrite: 0, model: 'eff-model', blocks: 3 }),
      ...rec('m2', { input: 20, output: 2, cacheRead: 200, cacheWrite: 5, model: 'eff-model', blocks: 2 }),
    ]);

    // 主线程夹具：混入 isSidechain:true 的记录——其 usage 必须**不**计入。
    // 另注：`p2` 落 **3 个块（同 message.id）**——这是主线程侧**去重的真控制**：
    // 不去重会得 input = 7 + 3×3 = 16，去重后才是 10（R 轮核验 RED 的处置，见 PROPOSAL §7）。
    writeJsonl(join(dir, slug, `${sess}.jsonl`), [
      ...rec('p1', { input: 7, output: 1, cacheRead: 70, cacheWrite: 0, model: 'thread-model' }),
      ...rec('p2', { input: 3, output: 1, cacheRead: 30, cacheWrite: 0, model: 'thread-model', blocks: 3 }),
      ...rec('sc1', { input: 9999, output: 9999, sidechain: true, model: 'side-model' }), // 干扰项
    ]);

    process.env.KIDPAL_CLAUDE_PROJECTS_DIR = dir;

    add('fixture-nonempty', () => {
      // 真空绿规约：夹具必须真落盘
      return existsSync(join(subDir, 'agent-aaa.jsonl')) && existsSync(join(dir, slug, `${sess}.jsonl`));
    });

    add('dispatch-hit', () => {
      const r = readDispatchTelemetry('aaa');
      return r.found === true && r.messageCount === 2;
    });

    add('dispatch-dedupe-no-inflation', () => {
      const r = readDispatchTelemetry('aaa');
      // 不去重会得 input = 10*3 + 20*2 = 70；去重后 = 30
      return r.input === 30 && r.output === 3 && r.cacheRead === 300 && r.cacheWrite === 5;
    });

    add('dispatch-total-is-input-plus-output', () => {
      const r = readDispatchTelemetry('aaa');
      return r.total === r.input + r.output && r.total === 33;
    });

    add('dispatch-effective-model-surface', () => {
      return readDispatchTelemetry('aaa').model === 'eff-model';
    });

    add('dispatch-sessionid-from-record', () => {
      const r = readDispatchTelemetry('aaa');
      return r.sessionId === 'sess-1' && r.projectDirName === slug;
    });

    add('dispatch-missing-file-degrades', () => {
      const r = readDispatchTelemetry('does-not-exist');
      return r.found === false && typeof r.reason === 'string';
    });

    add('dispatch-selfinvented-id-degrades', () => {
      // 历史自造标签（含连字符、非 agentId 形态）也必须 fail-soft 而非抛
      const r = readDispatchTelemetry('acceptance-author-r1');
      return r.found === false && typeof r.reason === 'string';
    });

    add('dispatch-no-throw-on-missing-root', () => {
      const keep = process.env.KIDPAL_CLAUDE_PROJECTS_DIR;
      process.env.KIDPAL_CLAUDE_PROJECTS_DIR = join(dir, 'nope');
      const r = readDispatchTelemetry('aaa');
      process.env.KIDPAL_CLAUDE_PROJECTS_DIR = keep;
      return r.found === false;
    });

    add('main-thread-hit-and-sidechain-excluded', () => {
      const r = readMainThreadUsage('sess-1', slug);
      // sidechain 干扰项 (9999) 必须未被计入
      return r.found === true && r.input === 10 && r.output === 2 && r.cacheRead === 100;
    });

    // 主线程侧去重的**真控制**（R 轮核验 RED 的处置）：`p2` 同 message.id 落 3 块。
    // 去掉 `readMainThreadUsage` 里的 `dedupeByMessageId` 会让本断言变红（input 变 16、messageCount 变 4）。
    add('main-thread-dedupe-no-inflation', () => {
      const r = readMainThreadUsage('sess-1', slug);
      return r.input === 10 && r.output === 2 && r.cacheRead === 100 && r.messageCount === 2;
    });

    add('main-thread-total-and-model', () => {
      const r = readMainThreadUsage('sess-1', slug);
      return r.total === r.input + r.output && r.total === 12 && r.model === 'thread-model';
    });

    add('main-thread-missing-session-degrades', () => {
      const r = readMainThreadUsage('nope-session', slug);
      return r.found === false;
    });

    add('main-thread-no-args-degrades', () => {
      return readMainThreadUsage(null, null).found === false;
    });

    add('predicate-rejects-message-type-trap', () => {
      // 正控：若谓词误取 message.type（值恒为 "message"），下面会得 0
      const wrong = writeJsonl;
      const probe = (r) => isAssistantRecord(r);
      const real = rec('z1', { input: 5 });
      const trap = real.map(r => ({ ...r, type: 'message' })); // 模拟误取：顶层 type 换成 "message"
      return probe(real[0]) === true && probe(trap[0]) === false;
    });

    add('selfcheck-fixture-actually-empty-sentinel', () => {
      // 反向哨兵：空目录必须得 found:false（防「夹具其实是空的也全绿」）
      const empty = mkdtempSync(join(tmpdir(), 'kidpal-empty-'));
      const keep = process.env.KIDPAL_CLAUDE_PROJECTS_DIR;
      process.env.KIDPAL_CLAUDE_PROJECTS_DIR = empty;
      const r = readDispatchTelemetry('aaa');
      process.env.KIDPAL_CLAUDE_PROJECTS_DIR = keep;
      rmSync(empty, { recursive: true, force: true });
      return r.found === false;
    });
  } finally {
    if (prev === undefined) delete process.env.KIDPAL_CLAUDE_PROJECTS_DIR;
    else process.env.KIDPAL_CLAUDE_PROJECTS_DIR = prev;
    rmSync(dir, { recursive: true, force: true });
  }

  const failures = cases.filter(c => !c.ok);
  const ok = cases.length > 0 && failures.length === 0;
  process.stdout.write(`${JSON.stringify({
    selfCheck: 'transcript-telemetry',
    pass: cases.length - failures.length,
    fail: failures.length,
    total: cases.length,
    ok,
    failures: ok ? undefined : failures,
  })}\n`);
  return ok;
}

const invokedDirectly = process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url);
if (process.argv.includes('--self-check')) process.exit(selfCheck() ? 0 : 1);
if (invokedDirectly) {
  process.stderr.write('transcript-telemetry: 本件是库，无 CLI 行为；用 --self-check 跑自检。\n');
  process.exit(0);
}
