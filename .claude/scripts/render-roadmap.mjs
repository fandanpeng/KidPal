#!/usr/bin/env node
/**
 * render-roadmap.mjs — workstream roadmap 的列车图 + 达成状态机械渲染器。
 *
 * 落地：260912-gov-review-portfolio-decomposition-chain T1/T4（FROZEN v1.0）。
 * 性质：**advisory 生成器，非门**——渲染失败只 warn，不作任何放行/拦截判据。
 *
 * 权威序（防第二事实源）：包集与达成状态的权威恒 = board（git common dir 下 devloop/）
 * + `.claude/taskpackages/<changeId>/taskpackage.json`；本脚本**只投影不裁决**。
 * roadmap.md ② 节由本脚本渲染、人工禁改；改依赖/排序 = 改权威侧再重跑（全量重算、幂等）。
 *
 * 输入面（FROZEN n-15 钉死）：
 *   - taskpackage JSON 集按 `origin.workstream` 圈定（实测边界：跨 workstream 的 dependsOn
 *     目标不在圈定内，渲染为带标注外部节点——FROZEN m-11/R2-m2 口径）；
 *   - board.json（默认 `$(git rev-parse --git-common-dir)/devloop/board.json`，可 --board 覆盖）；
 *   - deviation 事件载体 = 各包 pod-events.jsonl 的**既有事件型** `human-decision`
 *     + `payload.decision === "anchor3-deviation-ruling"` 区分值（append 姿势 `--actor human`；
 *     eventType 是 pod-events-append.mjs 机械闭集，本脚本不扩闭集）。
 *
 * 用法：
 *   node .claude/scripts/render-roadmap.mjs --workstream <slug> [--write-into <roadmap.md>]
 *                                          [--out <fragment.md>] [--board <path>]
 *                                          [--taskpackages-root <dir>] [--json]
 *   node .claude/scripts/render-roadmap.mjs --self-check
 *
 * 退出码：0 = 渲染成功（含 warn）；1 = --self-check 断言失败；2 = 参数/输入错误（fail-loud）。
 */

import { readFileSync, writeFileSync, readdirSync, existsSync, mkdirSync, rmSync, statSync, renameSync } from 'node:fs';
import { dirname, join, resolve, basename } from 'node:path';
import { fileURLToPath } from 'node:url';
import { execFileSync } from 'node:child_process';
import { tmpdir } from 'node:os';

const SCRIPT_DIR = dirname(fileURLToPath(import.meta.url));
const REPO_ROOT = resolve(SCRIPT_DIR, '..', '..');

const BEGIN_MARKER = '<!-- render-roadmap:begin -->';
const END_MARKER = '<!-- render-roadmap:end -->';
const DEVIATION_DECISION = 'anchor3-deviation-ruling';

function warn(msg) {
  process.stderr.write(`[render-roadmap] WARN ${msg}\n`);
}

function fail(msg) {
  process.stderr.write(`[render-roadmap] ERROR ${msg}\n`);
  process.exit(2);
}

function parseArgs(argv) {
  const args = { selfCheck: false, json: false };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    if (a === '--self-check') args.selfCheck = true;
    else if (a === '--json') args.json = true;
    else if (a === '--workstream') args.workstream = argv[++i];
    else if (a === '--write-into') args.writeInto = argv[++i];
    else if (a === '--out') args.out = argv[++i];
    else if (a === '--board') args.board = argv[++i];
    else if (a === '--taskpackages-root') args.taskpackagesRoot = argv[++i];
    else fail(`未知参数：${a}`);
  }
  return args;
}

function readJson(path, label) {
  try {
    return JSON.parse(readFileSync(path, 'utf8'));
  } catch (err) {
    warn(`${label} 读取/解析失败（${path}）：${err.message}`);
    return null;
  }
}

function readJsonl(path) {
  if (!existsSync(path)) return [];
  const out = [];
  for (const line of readFileSync(path, 'utf8').split(/\r?\n/)) {
    const t = line.trim();
    if (!t) continue;
    try {
      out.push(JSON.parse(t));
    } catch {
      warn(`pod-events 非法 JSON 行已跳过（${basename(path)}）`);
    }
  }
  return out;
}

/** 圈定：taskpackage JSON 集按 origin.workstream === slug。 */
function collectPackages(taskpackagesRoot, slug) {
  if (!existsSync(taskpackagesRoot)) {
    warn(`taskpackages 根不存在：${taskpackagesRoot}`);
    return [];
  }
  const pkgs = [];
  for (const entry of readdirSync(taskpackagesRoot)) {
    const dir = join(taskpackagesRoot, entry);
    if (!statSync(dir).isDirectory()) continue;
    const tpPath = join(dir, 'taskpackage.json');
    if (!existsSync(tpPath)) continue;
    const tp = readJson(tpPath, `taskpackage ${entry}`);
    if (!tp) continue;
    const ws = tp?.origin?.workstream;
    if (ws !== slug) continue;
    pkgs.push({
      changeId: tp.changeId || entry,
      dir,
      mergeOrder: Number.isFinite(tp.mergeOrder) ? tp.mergeOrder : null,
      dependsOn: Array.isArray(tp.dependsOn) ? tp.dependsOn.filter((d) => typeof d === 'string') : [],
      title: typeof tp.title === 'string' ? tp.title : '',
    });
  }
  pkgs.sort((a, b) => (a.mergeOrder ?? 9999) - (b.mergeOrder ?? 9999) || a.changeId.localeCompare(b.changeId));
  return pkgs;
}

/** board 解析：默认 git common dir 下 devloop/board.json（pod-coordination §2）。 */
function resolveBoardPath(explicit) {
  if (explicit) return explicit;
  try {
    const common = execFileSync('git', ['rev-parse', '--path-format=absolute', '--git-common-dir'], {
      cwd: REPO_ROOT,
      encoding: 'utf8',
    }).trim();
    return join(resolve(REPO_ROOT, common), 'devloop', 'board.json');
  } catch (err) {
    warn(`git common dir 解析失败（${err.message}）——达成状态降级为 pending/unknown`);
    return null;
  }
}

function readBoardStatuses(boardPath) {
  const map = new Map();
  if (!boardPath || !existsSync(boardPath)) {
    if (boardPath) warn(`board.json 不在（${boardPath}）——达成状态降级`);
    return map;
  }
  const board = readJson(boardPath, 'board.json');
  for (const pod of board?.pods ?? []) {
    if (pod && typeof pod.changeId === 'string') map.set(pod.changeId, pod.status ?? 'unknown');
  }
  return map;
}

/** deviation：既有事件型 human-decision + payload.decision 区分值（FROZEN ㉒/R3-n1）。 */
function collectDeviations(pkgDir) {
  const events = readJsonl(join(pkgDir, 'pod-events.jsonl'));
  return events
    .filter(
      (e) =>
        e?.eventType === 'human-decision' &&
        e?.payload?.decision === DEVIATION_DECISION,
    )
    .map((e) => ({ eventId: e.eventId, timestamp: e.timestamp, actor: e.actor }));
}

/** 达成状态：deviation 优先标注，其次 merged=achieved，其余投影 board 状态或 pending。 */
function achievement(boardStatus, deviations) {
  if (deviations.length > 0) {
    const anchors = deviations.map((d) => d.eventId).join(', ');
    return `deviation（裁定锚 ${anchors}）`;
  }
  if (boardStatus === 'merged') return 'achieved';
  if (boardStatus) return `pending（board: ${boardStatus}）`;
  return 'pending';
}

function mermaidId(changeId) {
  return changeId.replace(/[^A-Za-z0-9_]/g, '_');
}

function renderMermaid(pkgs, statuses, deviationsByPkg) {
  const inScope = new Set(pkgs.map((p) => p.changeId));
  const lines = ['```mermaid', 'graph LR'];
  const externals = new Set();
  for (const p of pkgs) {
    const dev = (deviationsByPkg.get(p.changeId) ?? []).length > 0;
    const merged = statuses.get(p.changeId) === 'merged';
    const label = `${p.changeId}${p.mergeOrder != null ? ` (mo${p.mergeOrder})` : ''}`;
    lines.push(`  ${mermaidId(p.changeId)}["${label}"]`);
    if (dev) lines.push(`  class ${mermaidId(p.changeId)} deviation`);
    else if (merged) lines.push(`  class ${mermaidId(p.changeId)} achieved`);
  }
  for (const p of pkgs) {
    for (const dep of p.dependsOn) {
      if (inScope.has(dep)) {
        lines.push(`  ${mermaidId(dep)} --> ${mermaidId(p.changeId)}`);
      } else {
        externals.add(dep);
        lines.push(`  ${mermaidId(dep)}["${dep}<br/>（跨 workstream 外部前置）"]`);
        lines.push(`  ${mermaidId(dep)} -.-> ${mermaidId(p.changeId)}`);
      }
    }
  }
  lines.push('  classDef achieved fill:#e6f4ea,stroke:#34a853');
  lines.push('  classDef deviation fill:#fce8e6,stroke:#ea4335');
  if (externals.size > 0) lines.push(`  class ${[...externals].map(mermaidId).join(',')} external`);
  lines.push('  classDef external stroke-dasharray: 4 3,color:#5f6368');
  lines.push('```');
  return { block: lines.join('\n'), externals: [...externals] };
}

function renderStatusTable(pkgs, statuses, deviationsByPkg) {
  const rows = [
    '| changeId | mergeOrder | 达成状态 | deviation 裁定锚 |',
    '|---|---|---|---|',
  ];
  for (const p of pkgs) {
    const devs = deviationsByPkg.get(p.changeId) ?? [];
    rows.push(
      `| ${p.changeId} | ${p.mergeOrder ?? '—'} | ${achievement(statuses.get(p.changeId), devs)} | ${
        devs.length ? devs.map((d) => `\`${d.eventId}\``).join(' ') : '—'
      } |`,
    );
  }
  return rows.join('\n');
}

function renderFragment({ slug, pkgs, statuses, deviationsByPkg, generatedAt }) {
  const { block, externals } = renderMermaid(pkgs, statuses, deviationsByPkg);
  const parts = [
    BEGIN_MARKER,
    `<!-- 本区块由 .claude/scripts/render-roadmap.mjs 机械渲染（workstream=${slug}，渲染时刻 ${generatedAt}）。`,
    '     人工禁改；渲染源唯一 = board + taskpackage JSON。全量重算、幂等。 -->',
    block,
    '',
    renderStatusTable(pkgs, statuses, deviationsByPkg),
    externals.length
      ? `\n> 外部前置（跨 workstream，不计入本 workstream 包集）：${externals.map((e) => `\`${e}\``).join('、')}`
      : '',
    END_MARKER,
  ];
  return parts.filter((l) => l !== '').join('\n') + '\n';
}

function buildModel(opts) {
  const taskpackagesRoot = opts.taskpackagesRoot ?? join(REPO_ROOT, '.claude', 'taskpackages');
  const pkgs = collectPackages(taskpackagesRoot, opts.workstream);
  if (pkgs.length === 0) warn(`workstream=${opts.workstream} 圈定到 0 个包（origin.workstream 无命中）`);
  const statuses = readBoardStatuses(resolveBoardPath(opts.board));
  const deviationsByPkg = new Map();
  for (const p of pkgs) deviationsByPkg.set(p.changeId, collectDeviations(p.dir));
  return { pkgs, statuses, deviationsByPkg };
}

function writeIntoFile(target, fragment) {
  if (!existsSync(target)) fail(`--write-into 目标不存在：${target}（先用 WORKSTREAM-ROADMAP.template.md 建 roadmap.md）`);
  const src = readFileSync(target, 'utf8');
  const b = src.indexOf(BEGIN_MARKER);
  const e = src.indexOf(END_MARKER);
  let out;
  if (b === -1 || e === -1 || e < b) {
    warn(`目标缺 render-roadmap 标记对（${target}）——fragment 追加到文末，请把它移进 ② 节`);
    out = src.replace(/\s*$/, '\n\n') + fragment;
  } else {
    out = src.slice(0, b) + fragment.replace(/\n$/, '') + src.slice(e + END_MARKER.length);
  }
  const tmp = `${target}.tmp-${process.pid}`;
  writeFileSync(tmp, out, 'utf8');
  renameSync(tmp, target);
  return out;
}

// ---------------------------------------------------------------- self-check

function assert(cond, name, failures) {
  if (cond) process.stdout.write(`  ok  ${name}\n`);
  else {
    process.stdout.write(`  FAIL ${name}\n`);
    failures.push(name);
  }
}

function writeFixtureTaskpackage(root, changeId, { workstream, mergeOrder, dependsOn }) {
  const dir = join(root, changeId);
  mkdirSync(dir, { recursive: true });
  writeFileSync(
    join(dir, 'taskpackage.json'),
    JSON.stringify(
      { changeId, mergeOrder, dependsOn, origin: { upstreamType: 'proposal', workstream } },
      null,
      2,
    ),
    'utf8',
  );
  return dir;
}

function selfCheck() {
  process.stdout.write('[render-roadmap] --self-check（hermetic fixture，不触仓内制品）\n');
  const failures = [];
  const root = join(tmpdir(), `render-roadmap-selfcheck-${process.pid}`);
  rmSync(root, { recursive: true, force: true });
  const tpRoot = join(root, 'taskpackages');
  mkdirSync(tpRoot, { recursive: true });

  // fixture：3 包在圈定内 + 1 包属别的 workstream + 1 跨 workstream 外部前置
  const p1 = writeFixtureTaskpackage(tpRoot, '260101-alpha', { workstream: 'ws-a', mergeOrder: 1, dependsOn: [] });
  writeFixtureTaskpackage(tpRoot, '260101-beta', { workstream: 'ws-a', mergeOrder: 2, dependsOn: ['260101-alpha', '260101-external'] });
  writeFixtureTaskpackage(tpRoot, '260101-gamma', { workstream: 'ws-a', mergeOrder: 3, dependsOn: [] });
  writeFixtureTaskpackage(tpRoot, '260101-other', { workstream: 'ws-b', mergeOrder: 9, dependsOn: [] });
  writeFixtureTaskpackage(tpRoot, '260101-external', { workstream: 'ws-c', mergeOrder: 0, dependsOn: [] });

  // board：alpha merged、beta running、gamma 缺席（降级 pending）
  const boardPath = join(root, 'board.json');
  writeFileSync(
    boardPath,
    JSON.stringify({
      pods: [
        { changeId: '260101-alpha', status: 'merged' },
        { changeId: '260101-beta', status: 'running' },
      ],
    }),
    'utf8',
  );

  // deviation：gamma 挂 human-decision + payload.decision 区分值（既有闭集事件型，actor=human 姿势）
  writeFileSync(
    join(tpRoot, '260101-gamma', 'pod-events.jsonl'),
    [
      JSON.stringify({ schemaVersion: 1, eventId: 'evt-ok-1', changeId: '260101-gamma', eventType: 'human-decision', actor: 'human', source: 'planner', payload: { decision: 'approve-escalated-merge', summary: 'x' } }),
      JSON.stringify({ schemaVersion: 1, eventId: 'evt-dev-1', changeId: '260101-gamma', eventType: 'human-decision', actor: 'human', source: 'planner', payload: { decision: DEVIATION_DECISION, summary: '锚 3 对账不一致，human 裁断' } }),
      JSON.stringify({ schemaVersion: 1, eventId: 'evt-noise', changeId: '260101-gamma', eventType: 'dispatch-completed', actor: 'planner', source: 'dispatch-stamp.mjs', payload: {} }),
    ].join('\n') + '\n',
    'utf8',
  );

  const model = buildModel({ workstream: 'ws-a', board: boardPath, taskpackagesRoot: tpRoot });
  assert(model.pkgs.length === 3, '圈定 = origin.workstream 命中 3 包（ws-b/ws-c 不入集）', failures);
  assert(
    model.pkgs.map((p) => p.changeId).join(',') === '260101-alpha,260101-beta,260101-gamma',
    '包序按 mergeOrder 升序',
    failures,
  );
  const devs = model.deviationsByPkg.get('260101-gamma') ?? [];
  assert(devs.length === 1 && devs[0].eventId === 'evt-dev-1', 'deviation 只认 human-decision + payload.decision 区分值', failures);
  assert(achievement(model.statuses.get('260101-alpha'), []) === 'achieved', 'merged → achieved', failures);
  assert(
    achievement(model.statuses.get('260101-gamma'), devs).startsWith('deviation（裁定锚 evt-dev-1'),
    'deviation 优先于 board 状态',
    failures,
  );
  assert(achievement(undefined, []) === 'pending', 'board 缺席 → pending（降级不炸）', failures);

  const { block, externals } = renderMermaid(model.pkgs, model.statuses, model.deviationsByPkg);
  assert(externals.length === 1 && externals[0] === '260101-external', '跨 workstream dependsOn → 外部节点', failures);
  assert(block.includes('跨 workstream 外部前置'), '外部节点带标注', failures);
  assert(block.includes('260101_alpha --> 260101_beta'), 'in-scope 依赖渲染实线边', failures);
  assert(block.includes('-.->'), '外部前置渲染虚线边', failures);

  const frag = renderFragment({
    slug: 'ws-a',
    pkgs: model.pkgs,
    statuses: model.statuses,
    deviationsByPkg: model.deviationsByPkg,
    generatedAt: 'FIXED',
  });
  assert(frag.startsWith(BEGIN_MARKER) && frag.trimEnd().endsWith(END_MARKER), 'fragment 带标记对', failures);

  // 幂等：同输入两次渲染逐字一致（generatedAt 固定）
  const frag2 = renderFragment({
    slug: 'ws-a',
    pkgs: buildModel({ workstream: 'ws-a', board: boardPath, taskpackagesRoot: tpRoot }).pkgs,
    statuses: readBoardStatuses(boardPath),
    deviationsByPkg: model.deviationsByPkg,
    generatedAt: 'FIXED',
  });
  assert(frag === frag2, '全量重算幂等（逐字一致）', failures);

  // --write-into 标记替换 + 再跑幂等
  const roadmap = join(root, 'roadmap.md');
  writeFileSync(roadmap, `# t\n\n## ② 列车图\n\n${BEGIN_MARKER}\n旧内容\n${END_MARKER}\n\n## ③\n`, 'utf8');
  const once = writeIntoFile(roadmap, frag);
  assert(once.includes('旧内容') === false && once.includes('260101-alpha'), '标记区块被替换', failures);
  const twice = writeIntoFile(roadmap, frag);
  assert(once === twice, '--write-into 重跑幂等', failures);
  assert(twice.includes('## ③'), '标记外正文未被吞', failures);

  rmSync(root, { recursive: true, force: true });
  if (failures.length > 0) {
    process.stderr.write(`[render-roadmap] self-check FAILED：${failures.length} 条\n`);
    process.exit(1);
  }
  process.stdout.write('[render-roadmap] self-check GREEN（全部断言通过）\n');
  process.exit(0);
}

// ---------------------------------------------------------------------- main

function main() {
  const args = parseArgs(process.argv.slice(2));
  if (args.selfCheck) selfCheck();
  if (!args.workstream) fail('缺 --workstream <slug>（或跑 --self-check）');

  const model = buildModel(args);
  const generatedAt = new Date().toISOString();
  const fragment = renderFragment({
    slug: args.workstream,
    pkgs: model.pkgs,
    statuses: model.statuses,
    deviationsByPkg: model.deviationsByPkg,
    generatedAt,
  });

  if (args.json) {
    process.stdout.write(
      JSON.stringify(
        {
          workstream: args.workstream,
          generatedAt,
          packages: model.pkgs.map((p) => ({
            changeId: p.changeId,
            mergeOrder: p.mergeOrder,
            dependsOn: p.dependsOn,
            boardStatus: model.statuses.get(p.changeId) ?? null,
            deviations: model.deviationsByPkg.get(p.changeId) ?? [],
            achievement: achievement(model.statuses.get(p.changeId), model.deviationsByPkg.get(p.changeId) ?? []),
          })),
        },
        null,
        2,
      ) + '\n',
      'utf8',
    );
    return;
  }

  if (args.writeInto) {
    writeIntoFile(resolve(args.writeInto), fragment);
    process.stdout.write(`[render-roadmap] 已写入 ${args.writeInto} 的 ② 节标记区块（${model.pkgs.length} 包）\n`);
    return;
  }

  if (args.out) {
    const outPath = resolve(args.out);
    mkdirSync(dirname(outPath), { recursive: true });
    writeFileSync(outPath, fragment, 'utf8');
    process.stdout.write(`[render-roadmap] fragment 已落 ${args.out}（${model.pkgs.length} 包）\n`);
    return;
  }

  process.stdout.write(fragment);
}

main();
