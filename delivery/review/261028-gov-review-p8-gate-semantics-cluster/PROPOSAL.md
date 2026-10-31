# PROPOSAL · 261028-gov-review-p8-gate-semantics-cluster

> **状态：FROZEN v0.3 · 2026-10-28 · 凭「立项 ack（human 两拍 + OQ 三拍）+ R3 GREEN（R1 BLOCK(F1) → R2 BLOCK(F1′) → R3 GREEN）」主线程标 FROZEN；F5 NIT 已冻结前顺手修；landing 待 human 通知**
> **承载 ARQ**：`ARQ-261014-supervisor-escalate-preflight-ignored`（簇 A：申报 + 同型先例 + 噪音判别）· `ARQ-261001-host-blind-outcome-device-gate`（簇 B：设备轮硬门 + 配套全套；T3/T4 为批内独立全强度评审单元）。立项 ack = `delivery/workstreams/gov-debt-reduction/task_plan.md` P8 节，human 2026-10-28 两拍。
> **review 轨**：**Track B 主路径（起草期自评）**，触 hooks / scripts / verify-gates，命中轻路径排除集；判档归 architect-reviewer、只升不降。Track A 段不命中：方向已由 human 两拍收敛，无声明红队。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5，共 42 处旧串锚定编辑 + 1 件新增先例种子文件）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**high**。本批同时修改风险分级、合并阻断条件与证据语义；虽然自动折抵被逐轴白名单收窄且设备门向严，任何判据漂移仍可能同时造成假绿与假红，故按门机制高敏面申报。
> **review profile**：**G2+G3**。G2 = agents / references / skills；G3 = hooks / scripts / verify-gates / 新增先例种子配置。G1、契约面、spec 两文件与自有实现面均为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/hooks/supervisor-verdict-gate.mjs
.claude/hooks/supervisor-gate-check.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/supervisor-precedents.json
.claude/verify-gates.json
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/skills/kidpal-device-logs/SKILL.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/agents/planner.md
.claude/agents/acceptance-author.md
.claude/agents/builder.md
.claude/agents/integrator.md
docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md
docs/debt/gov/ARQ-261001-host-blind-outcome-device-gate.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：让 supervisor 对已经申报且有人批同型先例的已知验证形态停止重复升级，同时让用户可见、host 不可编的设备结果在合并前必须留下可机判的真机目击证据；两者共用一张启动卡申报行，但不共用风险折抵语义。

**现状差距**：supervisor 现只按 diff 重派生 riskTier 与 declared riskTier 取严，`isAllMock` 对测试文件无真库/事件标记一刀切；launch-card 的 all-mock 前置申报与历史人批不被消费。另一方面，host 全绿并不证明 `lcd_display.cc`、板级 wiring 等 host 不可编面上的用户可见结果正确，现有 defer-merge-pending-live-round 已是事实上的阻断链，却没有机械证据门。

**非目标**：不抽新的 host 可测决策缝（ARQ-261001 候选②留 dev-loop 个案）；不新增第七个工程门，设备轮折进既有 supervisor 合并门的 merge 期子判据；不允许 security/core/critical 或任何新轴自动折抵；不改 declared riskTier 只升不降；不改契约/spec/实现代码；不把完整 KPLOG 或儿童语音转写提交进公开仓；不追溯伪造在途包的真机证据。

## 1. 判档依据（Track B 主路径）

- **轻路径判据①不满足**：diff 触 `.claude/hooks/**`、`.claude/scripts/**`、`.claude/agents/**`，不在纯文档允许集。
- **轻路径判据②不满足**：本批改变风险分级与合并阻断行为，不是措辞、格式、链接或勘误。
- **轻路径判据③直接命中**：hooks / scripts / verify-gates 属排除集，哪怕只改一个字符也恒 Track B。
- **Track A 段不命中**：human 已选簇 A「①+②+③」与簇 B「①硬门+配套全套」，并追加 KPLOG 可作为佐证；根本方向已收敛。§4 三个 OQ 是载体、机械触发与拆批纪律的剩余取舍，若 reviewer 认为任一构成根本方向缺口，应按只升不降升级。
- **批内强度**：T3/T4 是 ARQ-261001 的独立全强度评审单元，不因与簇 A 同批而稀释。
- **拆分纪律明示例外（human 2026-10-28 拍定）**：436 行仍按一批推进；例外依据 = 立项拍批形态在先 + 批内独立 T 全强度评审保强度。

## 2. 问题、根因与改法（T1…T5）

**新机制 spike 申报**：本批不引入新产品子系统、外部依赖或数据迁移。新增的只是既有闸的消费字段、一个只读历史种子配置与包内证据格式；42 处编辑均已在仓外副本实跑，无另立 spike 批的必要。

**装配纪律**：所有 42 处编辑均旧串锚定，现盘恰命中 1 次后才写仓外副本。最终副本 = `C:/tmp/p8-assembly9`；清单与核对件 = 本批 `_scratch/`。脚本净增实测 436 行，拆分纪律的处置列 §4 OQ-3，不能按 fix 级小改搭车。

### 2.0 复证结论（2026-10-28）

1. **路径漂移属实**：supervisor 真身在 `.claude/hooks/supervisor-verdict-gate.mjs`，ARQ-261014 frontmatter 仍写 scripts；T5 随批修正。
2. **申报与先例零承载**：`isAllMock` 现文只查测试路径与 `REAL_EFFECT_MARKERS`，闸主流程没有 launch-card / precedent 读取；G6 直接重算同一 diff，若只改主闸会造成主闸 APPROVE、Stop 钩反拦，因此 T2 同改第二方重算。
3. **噪音有精确实证**：commit `249827150adff14ac9e3eba979614e9319e4d379` 删除的 5 行是 `Image Effects Configuration` 空节头；本批取其逆形态作正夹具，并配真实 `CONFIG_NEW_FEATURE=y` 反夹具。
4. **启动卡申报是散文而非机器格式**：`261012-kidpal-realtime-facts/launch-card.md:15` 与 `261010-prompt-v9-schedule-anti-narration/launch-card.md:13` 都写 all-mock 前置申报，现行 skill 模板没有机器消费行。T1 将其结构化而不改写历史卡。
5. **设备证据已有可复用形态**：`261001-m4-k3-record-session-live-repair/pod-events.jsonl` 依次记录 defer-merge-pending-live-round、真机通过、approve-escalated-merge；`261018-sd-log-persist/device-round-checklist.md` 已含 KPLOG 段与串口证据。T3/T4 将事实链变成包内最小结构，不另造流程。
6. **host 不可编不是单一文件**：`firmware/main/test/host_app/main/CMakeLists.txt` 只把显式源接入 host runner，`lcd_display.cc`、`kidpal_render_node.cc`、`application.cc`、`mcp_server.cc` 等被注记为设备侧；板级 wiring 也不能遗漏，故配置同时支持精确路径与 `firmware/main/boards/` 前缀。
7. **KPLOG 引用与隐私必须同时成立**：`kidpal-device-logs` 已定义段号、墙上时间与五类语义缝；仓库公开后，完整原日志不应成为提交物。T4 只要求最小脱敏语义摘录与 SHA-256。

### T1 · 启动卡申报与先例生产口径

**问题**：all-mock 与真机轮前置申报已经存在于 launch-card，但都是自然语言，supervisor 与 pre-merge 无法消费；human 每次只能重复判读同型包。

**证据**：两张历史启动卡的申报原文、现行 launch-prep 七要素模板、pod-events §5d 的 human-decision envelope，见 §2.0-2/4。

**根因**：申报没有稳定机器格式，人批事件也缺少「批准本次合并」与「同意登记为后续先例」的语义区分。

**改法**：在启动卡模板加入单行 JSON 验证形态申报；在 pod-events §5d 与 planner 两条人批路径规定 approvedVia + gatePrecedents 只有在 human 明确同意登记先例时才写。申报只提供匹配键，不自行降级。

**为什么这样改**：保留 launch-card 作为前置申报家、pod-events 作为新人批事件家，避免把历史散文硬解析成先例。

本 T 共 3 处旧串锚定编辑。以下原文与改后文本由同一份 _scratch/edits.json 机械展开，不作第二份手抄。

#### T1a-launch-card-gate-declaration · .claude/skills/kidpal-pod-launch-prep/SKILL.md

原文（现盘逐字）：
```text
固件严格模式默认开：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省；261015 批 T3 起 defer > required——lite/nano 包 scoped 相位 defer 短路优先于本行，final 相位恒跑）。
会话标识：<pid / 终端标识 / 启动时刻>（点火后由 launch 执行者回填——叫停时核「进程确实已死」用，见 pod-coordination「叫停协议」节）。
```

改后（逐字）：
```text
固件严格模式默认开：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省；261015 批 T3 起 defer > required——lite/nano 包 scoped 相位 defer 短路优先于本行，final 相位恒跑）。
验证形态申报（JSON）：{"schemaVersion":1,"precedents":{},"deviceOutcome":{"userVisible":false,"acceptanceRefs":[],"hostUncompilablePaths":[],"reason":"none"}}
会话标识：<pid / 终端标识 / 启动时刻>（点火后由 launch 执行者回填——叫停时核「进程确实已死」用，见 pod-coordination「叫停协议」节）。
```

#### T1b-launch-card-declaration-semantics · .claude/skills/kidpal-pod-launch-prep/SKILL.md

原文（现盘逐字）：
```text
- 身份探测 / 包事实 / 环境事实 / 教训全文**不再搬运**：planner 凭 changeId 裸起步自探测四处（taskpackage / upstreamRef / pods json / 本包启动卡——planner.md Step 1）；包事实在 taskpackage.json + plan.md + decision-coverage；环境事实的耐久家 = skills（kidpal-idf-env / kidpal-serial-monitor / kidpal-ecs-gateway / kidpal-live-smoke-debug）；基线家 = progress.md 带锚行；教训家 = 上一包 pod-retro.md；input-brief 与 C 侧 cwd 敏感判读等既有指针由 planner/verifier 自身定义与 260909 批制品承载。
```

改后（逐字）：
```text
- **验证形态申报是机器消费行，不是散文提示**：上例「验证形态申报（JSON）：」前缀后必须恰有一个单行 JSON 对象。`precedents` 只登记本包确有的同型验证形态：键只允许 `all-mock` / `generated-noise`，值形如 `{"pattern":"content-anchor-live-observation","changeId":"261010-prompt-v9-schedule-anti-narration"}`；pattern 与 changeId 必须指向已人批同型先例，申报本身不产生降级。`deviceOutcome` 用于设备轮：`userVisible=true` 时 `acceptanceRefs` 与 `hostUncompilablePaths` 均须非空；false 而实际 diff 触已登记 host 不可编路径时，`reason` 至少 12 字说明为何不是用户可见结果；未填（none）或保留尖括号占位符均视同未申报。启动卡缺行不阻 launch；但 supervisor 不消费缺失申报，前置升级闸对触 host 不可编路径而缺申报的包 fail-closed。
- 身份探测 / 包事实 / 环境事实 / 教训全文**不再搬运**：planner 凭 changeId 裸起步自探测四处（taskpackage / upstreamRef / pods json / 本包启动卡——planner.md Step 1）；包事实在 taskpackage.json + plan.md + decision-coverage；环境事实的耐久家 = skills（kidpal-idf-env / kidpal-serial-monitor / kidpal-ecs-gateway / kidpal-live-smoke-debug）；基线家 = progress.md 带锚行；教训家 = 上一包 pod-retro.md；input-brief 与 C 侧 cwd 敏感判读等既有指针由 planner/verifier 自身定义与 260909 批制品承载。
```

#### T1c-human-decision-precedent-schema · .claude/references/dev-loop-operating-system.md

原文（现盘逐字）：
```text
- human decision/correction 由 planner 在收到或检测到 human input 后 append。Verifier rounds、gate outcomes、test results、supervisor verdict 不进入 pod-events，仍以 immutable verifier evidence / supervisor verdict 为权威。
```

改后（逐字）：
```text
- human decision/correction 由 planner 在收到或检测到 human input 后 append。**当 human 明确批准把一次 ESCALATE 处置登记为可复用同型先例时**，该 `human-decision` 的 payload 除既有五要素外还须带 `approvedVia`（非空字符串，记录真实批准通道）与 `gatePrecedents:[{axis,pattern}]`；axis 只许 `all-mock` / `generated-noise`，pattern 是稳定短横串。planner 不得仅凭一次 approve-escalated-merge 自行补标签——「批准本次合并」与「同意成为后续先例」是两个语义，后者必须有人明确授权。Verifier rounds、gate outcomes、test results、supervisor verdict 不进入 pod-events，仍以 immutable verifier evidence / supervisor verdict 为权威。
```

目标文件：`.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改） · `.claude/references/dev-loop-operating-system.md`（改）

### T2 · supervisor 与 G6 同参降噪、逐轴先例折抵及空节噪音判别

**问题**：all-mock 已知形态与 sdkconfig 再生空节被反复升为 high/critical；只改主闸还会被 G6 第二方重算否决。

**证据**：supervisor :270-277 的一刀切启发式、G6 :762 的无申报重算、24982715 的 5 行空节实证、三条真实人批事件。

**根因**：风险派生缺少申报与先例上下文，critical-path 又只看 sdkconfig 路径不区分空节与真实配置。

**改法**：新增申报/先例读取、逐轴白名单折抵与快批提示；only all-mock/generated-noise 可折抵，必须 changeId+pattern 同型匹配。噪音还需满足纯新增空节谓词。G6 复用同一读取函数与同参派生。新增历史种子配置仅引用真实事件，未来先例只从 pod-events 累积，不双写。

**为什么这样改**：机械降噪的最小充分条件是「申报 + 已人批同型先例 + 轴内判别」，任何新轴仍走原升级路径；主闸与第二方必须同一事实源。

本 T 共 12 处旧串锚定编辑。以下原文与改后文本由同一份 _scratch/edits.json 机械展开，不作第二份手抄。

#### T2a-declaration-precedent-helpers · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
function isAllMock(paths, content) {
  const testPaths = paths.filter((p) => TEST_FILE_RE.test(p));
  if (testPaths.length === 0) return { hit: false };
  const text = String(content || '');
  for (const re of REAL_EFFECT_MARKERS) {
    if (re.test(text)) return { hit: false }; // 有真库/事件断言 → 非全 mock
  }
  return { hit: true, reason: 'all-mock:test-files-no-real-db/event-assertion(dev-loop:106-111)' };
}
```

改后（逐字）：
```text
function isAllMock(paths, content) {
  const testPaths = paths.filter((p) => TEST_FILE_RE.test(p));
  if (testPaths.length === 0) return { hit: false };
  const text = String(content || '');
  for (const re of REAL_EFFECT_MARKERS) {
    if (re.test(text)) return { hit: false }; // 有真库/事件断言 → 非全 mock
  }
  return { hit: true, reason: 'all-mock:test-files-no-real-db/event-assertion(dev-loop:106-111)' };
}

// ---- launch-card 申报与已人批同型先例（261028 P8；只折抵逐轴白名单）----
const OFFSETTABLE_AXES = new Set(['all-mock', 'generated-noise']);

function axisForReason(reason) {
  const s = String(reason || '');
  if (s.startsWith('all-mock:')) return 'all-mock';
  if (s.startsWith('critical-path:firmware/sdkconfig')) return 'generated-noise';
  if (s.startsWith('security-axis-')) return 'security';
  if (s.startsWith('core.*:')) return 'core';
  if (s.startsWith('critical-')) return 'critical';
  return 'other';
}

function parseGateDeclarationText(text) {
  const prefix = '验证形态申报（JSON）：';
  const lines = String(text || '').split(/\r?\n/).filter((l) => l.startsWith(prefix));
  if (lines.length !== 1) return { declaration: null, advisories: [`launch-card 验证形态申报行数=${lines.length}（应为 1；缺失/重复均不消费）`] };
  try {
    const declaration = JSON.parse(lines[0].slice(prefix.length));
    if (!declaration || declaration.schemaVersion !== 1 || typeof declaration !== 'object') throw new Error('schemaVersion 应为 1');
    return { declaration, advisories: [] };
  } catch (error) {
    return { declaration: null, advisories: [`launch-card 验证形态申报 JSON 不可解析（不消费）：${error.message}`] };
  }
}

function readGateDeclaration(dir, changeId) {
  const p = resolve(dir, 'delivery/taskpackages', changeId, 'launch-card.md');
  if (!existsSync(p)) return { declaration: null, advisories: ['launch-card.md 缺失（不消费申报）'] };
  return parseGateDeclarationText(readFileSync(p, 'utf8'));
}

function eventApprovedPrecedents(event) {
  if (event?.schemaVersion !== 1 || event?.eventType !== 'human-decision' || event?.actor !== 'human') return [];
  if (event?.payload?.decision !== 'approve-escalated-merge' || typeof event?.payload?.approvedVia !== 'string' || !event.payload.approvedVia.trim()) return [];
  if (!Array.isArray(event?.payload?.gatePrecedents)) return [];
  return event.payload.gatePrecedents.flatMap((p) => {
    if (!p || !OFFSETTABLE_AXES.has(p.axis) || typeof p.pattern !== 'string' || !/^[a-z0-9]+(?:-[a-z0-9]+)*$/.test(p.pattern)) return [];
    return [{ changeId: event.changeId, eventId: event.eventId, approvedVia: event.payload.approvedVia, axis: p.axis, pattern: p.pattern, sourceRef: `delivery/taskpackages/${event.changeId}/pod-events.jsonl` }];
  });
}

function readApprovedPrecedents(dir) {
  const precedents = [];
  const advisories = [];
  const registryPath = resolve(dir, '.claude/supervisor-precedents.json');
  if (existsSync(registryPath)) {
    try {
      const registry = JSON.parse(readFileSync(registryPath, 'utf8'));
      for (const seed of Array.isArray(registry.precedents) ? registry.precedents : []) {
        if (!seed || !OFFSETTABLE_AXES.has(seed.axis) || typeof seed.pattern !== 'string' || typeof seed.sourceRef !== 'string') {
          advisories.push('历史种子先例存在 malformed 条目（跳过，不折抵）');
          continue;
        }
        const eventPath = resolve(dir, seed.sourceRef);
        if (!existsSync(eventPath)) { advisories.push(`历史种子先例 sourceRef 缺失（跳过）：${seed.changeId}`); continue; }
        const hit = readFileSync(eventPath, 'utf8').split(/\r?\n/).flatMap((line) => {
          try { const e = JSON.parse(line); return e?.eventId === seed.eventId ? [e] : []; } catch { return []; }
        })[0];
        if (!hit || hit.eventType !== 'human-decision' || hit.actor !== 'human' || hit.changeId !== seed.changeId || hit.payload?.decision !== 'approve-escalated-merge') {
          advisories.push(`历史种子先例人批事件核验失败（跳过）：${seed.changeId}`);
          continue;
        }
        precedents.push({ changeId: seed.changeId, eventId: seed.eventId, approvedVia: String(seed.approvedVia || 'historical human-decision verified'), axis: seed.axis, pattern: seed.pattern, sourceRef: seed.sourceRef });
      }
    } catch (error) {
      advisories.push(`历史种子先例库不可读（全部不折抵）：${error.message}`);
    }
  }
  const root = resolve(dir, 'delivery/taskpackages');
  if (existsSync(root)) {
    for (const entry of readdirSync(root)) {
      const eventsPath = resolve(root, entry, 'pod-events.jsonl');
      if (!existsSync(eventsPath)) continue;
      for (const line of readFileSync(eventsPath, 'utf8').split(/\r?\n/)) {
        if (!line.trim()) continue;
        try { precedents.push(...eventApprovedPrecedents(JSON.parse(line))); }
        catch { /* malformed 行不产生先例；既有 pod-events fail-open 口径不变 */ }
      }
    }
  }
  const unique = [...new Map(precedents.map((p) => [`${p.changeId}|${p.eventId}|${p.axis}|${p.pattern}`, p])).values()];
  return { precedents: unique, advisories };
}

function matchDeclaredPrecedent(declaration, axis, precedents) {
  if (!OFFSETTABLE_AXES.has(axis)) return null;
  const claim = declaration?.precedents?.[axis];
  if (!claim || typeof claim.pattern !== 'string' || typeof claim.changeId !== 'string') return null;
  return (Array.isArray(precedents) ? precedents : []).find((p) => p.axis === axis && p.pattern === claim.pattern && p.changeId === claim.changeId) || null;
}

function isEmptySdkconfigRegeneration(perFile) {
  if (!(perFile instanceof Map) || !perFile.has('firmware/sdkconfig')) return false;
  const added = String(perFile.get('firmware/sdkconfig') || '').split(/\r?\n/)
    .filter((l) => /^\+(?!\+\+)/.test(l))
    .map((l) => l.slice(1));
  const removed = String(perFile.get('firmware/sdkconfig') || '').split(/\r?\n/).some((l) => /^-(?!--)/.test(l));
  // 只认 idf.py 再生的空节头：至少三行新增、全部为空行或注释、含 end of 节尾；任何配置赋值/删除都非噪音。
  return !removed && added.length >= 3 && added.every((l) => /^\s*(?:#.*)?$/.test(l)) && added.some((l) => /^\s*#\s*end of\s+/i.test(l));
}

```

#### T2b-derive-diff-tier-axis-offset · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
function deriveDiffTier(paths, content, tp, opts = {}) {
  try {
    if (opts.gitFailed) {
      // git 失败不得沿用 G3「不 block」放行派生（supervisor-gate-check:106-110）→ 不能派生 → 向严（FIX-5）
      return { tier: 'high', ambiguous: true, reasons: ['git-failed:cannot-derive-riskTier→toward-strict(high/ESCALATE),FIX-5'] };
    }
    if (!Array.isArray(paths)) throw new Error('diff paths must be an array');
    const crit = classifyCritical(paths, content, opts.perFile);
    if (crit.hit) return { tier: 'critical', ambiguous: false, reasons: crit.reasons };
    const high = classifyHigh(paths, content, tp || {}, opts.perFile);
    if (high.hit) return { tier: 'high', ambiguous: false, reasons: high.reasons };
    // 绿色通道（FIX-2/P-1）：单系统 + 真库 + 无安全轴 + diff 无 critical/high 闭集命中 = 确定 low（防过严 ATAM-1）。
    return { tier: 'low', ambiguous: false, reasons: ['green-channel:no-critical/high-closed-set-hit(dev-loop:244)'] };
  } catch (error) {
    // FIX-5 BLOCKING：criterion 抛异常被 try/catch 吞成默认 low = 绝不允许（fail 向 APPROVE）→ ambiguous 向严。
    return { tier: 'high', ambiguous: true, reasons: [`criterion-exception:${error.message}→toward-strict(high),FIX-5`] };
  }
}
```

改后（逐字）：
```text
function deriveDiffTier(paths, content, tp, opts = {}) {
  const advisories = [];
  const precedentHits = [];
  try {
    if (opts.gitFailed) {
      // git 失败不得沿用 G3「不 block」放行派生（supervisor-gate-check:106-110）→ 不能派生 → 向严（FIX-5）
      return { tier: 'high', ambiguous: true, reasons: ['git-failed:cannot-derive-riskTier→toward-strict(high/ESCALATE),FIX-5'], advisories, precedentHits };
    }
    if (!Array.isArray(paths)) throw new Error('diff paths must be an array');
    let effectivePaths = paths;
    if (paths.includes('firmware/sdkconfig') && isEmptySdkconfigRegeneration(opts.perFile)) {
      const hit = matchDeclaredPrecedent(opts.declaration, 'generated-noise', opts.precedents);
      if (hit) {
        effectivePaths = paths.filter((p) => p !== 'firmware/sdkconfig');
        precedentHits.push({ axis: 'generated-noise', changeId: hit.changeId, eventId: hit.eventId, pattern: hit.pattern });
        advisories.push(`generated-noise 先例折抵：${hit.changeId}/${hit.eventId}（仅 firmware/sdkconfig 空节；其它轴照算）`);
      }
    }
    const crit = classifyCritical(effectivePaths, content, opts.perFile);
    if (crit.hit) {
      const hints = crit.reasons.flatMap((reason) => {
        const axis = axisForReason(reason);
        if (axis === 'generated-noise' && !isEmptySdkconfigRegeneration(opts.perFile)) return [];
        const hit = matchDeclaredPrecedent(opts.declaration, axis, opts.precedents);
        return hit ? [`同款先例 changeId=${hit.changeId}（在册，eventId=${hit.eventId}），human 可凭先例快批；不折抵新轴`] : [];
      });
      return { tier: 'critical', ambiguous: false, reasons: crit.reasons, advisories: [...advisories, ...hints], precedentHits };
    }
    const high = classifyHigh(effectivePaths, content, tp || {}, opts.perFile);
    if (high.hit) {
      const remaining = [];
      for (const reason of high.reasons) {
        const axis = axisForReason(reason);
        const hit = matchDeclaredPrecedent(opts.declaration, axis, opts.precedents);
        if (axis === 'all-mock' && hit) {
          precedentHits.push({ axis, changeId: hit.changeId, eventId: hit.eventId, pattern: hit.pattern });
          advisories.push(`all-mock 先例折抵：${hit.changeId}/${hit.eventId}（只折抵 all-mock，新轴不折抵）`);
        } else {
          remaining.push(reason);
          if (hit) advisories.push(`同款先例 changeId=${hit.changeId}（在册），human 可凭先例快批；${axis} 轴不自动折抵`);
        }
      }
      if (remaining.length > 0) return { tier: 'high', ambiguous: false, reasons: remaining, advisories, precedentHits };
      return { tier: 'low', ambiguous: false, reasons: ['green-channel:all-high-reasons-offset-by-approved-same-pattern-precedent'], advisories, precedentHits };
    }
    // 绿色通道（FIX-2/P-1）：单系统 + 真库 + 无安全轴 + diff 无 critical/high 闭集命中 = 确定 low（防过严 ATAM-1）。
    return { tier: 'low', ambiguous: false, reasons: ['green-channel:no-critical/high-closed-set-hit(dev-loop:244)'], advisories, precedentHits };
  } catch (error) {
    // FIX-5 BLOCKING：criterion 抛异常被 try/catch 吞成默认 low = 绝不允许（fail 向 APPROVE）→ ambiguous 向严。
    return { tier: 'high', ambiguous: true, reasons: [`criterion-exception:${error.message}→toward-strict(high),FIX-5`], advisories, precedentHits };
  }
}
```

#### T2c-decide-thread-gate-context · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
function decide({ diff, tp, declaredTier, evidenceComplete, completedRounds }) {
  const derivation = deriveDiffTier(diff.paths, diff.content, tp || {}, { gitFailed: !!diff.gitFailed, perFile: diff.perFile });
```

改后（逐字）：
```text
function decide({ diff, tp, declaredTier, evidenceComplete, completedRounds }) {
  const derivation = deriveDiffTier(diff.paths, diff.content, tp || {}, {
    gitFailed: !!diff.gitFailed,
    perFile: diff.perFile,
    declaration: diff.declaration,
    precedents: diff.precedents,
  });
```

#### T2d-decision-return-advisories · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
      diffReasons: derivation.reasons,
    },
  };
}
```

改后（逐字）：
```text
      diffReasons: derivation.reasons,
      advisories: derivation.advisories || [],
      precedentHits: derivation.precedentHits || [],
    },
    advisories: derivation.advisories || [],
    precedentHits: derivation.precedentHits || [],
  };
}
```

#### T2e-main-read-declaration-precedents · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
  const git = collectGitDiff(projectDir);
  const diff = git.ok
    ? { paths: git.paths, content: git.content, perFile: git.perFile, gitFailed: false }
    : { paths: [], content: '', perFile: null, gitFailed: true };
```

改后（逐字）：
```text
  const git = collectGitDiff(projectDir);
  const declarationInfo = readGateDeclaration(projectDir, changeId);
  const precedentInfo = readApprovedPrecedents(projectDir);
  const diff = git.ok
    ? { paths: git.paths, content: git.content, perFile: git.perFile, gitFailed: false, declaration: declarationInfo.declaration, precedents: precedentInfo.precedents }
    : { paths: [], content: '', perFile: null, gitFailed: true, declaration: declarationInfo.declaration, precedents: precedentInfo.precedents };
```

#### T2f-main-combine-advisories · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
  const decision = decide({ diff, tp, declaredTier, evidenceComplete, completedRounds });
  const provenance = buildProvenance({ changeId, decision, bearing, closeoutArtifacts, verifyLogs, completedRounds, originType });
```

改后（逐字）：
```text
  const decision = decide({ diff, tp, declaredTier, evidenceComplete, completedRounds });
  decision.advisories = [...declarationInfo.advisories, ...precedentInfo.advisories, ...(decision.advisories || [])];
  decision.riskTierDerivation.advisories = decision.advisories;
  const provenance = buildProvenance({ changeId, decision, bearing, closeoutArtifacts, verifyLogs, completedRounds, originType });
```

#### T2g-console-advisories · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
    reasons: decision.reasons,
    evidenceComplete,
```

改后（逐字）：
```text
    reasons: decision.reasons,
    advisories: decision.advisories,
    precedentHits: decision.precedentHits,
    evidenceComplete,
```

#### T2h-discriminating-self-checks · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
  console.log(JSON.stringify({ selfCheck: 'supervisor-verdict-gate', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
```

改后（逐字）：
```text
  // 261028 P8：申报 + 同型人批先例的正反两臂；新轴永不折抵，噪音只认空节。
  const precedent = { changeId: '261010-prompt-v9-schedule-anti-narration', eventId: 'e02f05e3-e0f5-46b9-ba79-2b4027b8e090', axis: 'all-mock', pattern: 'content-anchor-live-observation' };
  const declaration = { schemaVersion: 1, precedents: { 'all-mock': { pattern: precedent.pattern, changeId: precedent.changeId } } };
  const allMockPaths = ['firmware/main/a.test.c'];
  ok(deriveDiffTier(allMockPaths, 'expect(x).toBe(1)', {}, { declaration, precedents: [precedent] }).tier === 'low', 'precedent-all-mock-offset-positive');
  ok(deriveDiffTier(allMockPaths, 'expect(x).toBe(1)', {}, { declaration, precedents: [precedent] }).precedentHits.length === 1, 'precedent-all-mock-advisory-recorded');
  ok(deriveDiffTier(allMockPaths, 'expect(x).toBe(1)', {}, { precedents: [precedent] }).tier === 'high', 'precedent-no-declaration-still-high');
  ok(deriveDiffTier(allMockPaths, 'expect(x).toBe(1)', {}, { declaration, precedents: [] }).tier === 'high', 'precedent-not-approved-still-high');
  ok(deriveDiffTier(allMockPaths, 'expect(x).toBe(1)', {}, { declaration: { schemaVersion: 1, precedents: { 'all-mock': { pattern: 'different-pattern', changeId: precedent.changeId } } }, precedents: [precedent] }).tier === 'high', 'precedent-pattern-mismatch-still-high');
  ok(deriveDiffTier(allMockPaths, 'auth login; expect(x).toBe(1)', {}, { declaration, precedents: [precedent] }).tier === 'high', 'precedent-new-security-axis-never-offset');
  ok(deriveDiffTier(['firmware/boards/x.c', ...allMockPaths], 'expect(x).toBe(1)', {}, { declaration, precedents: [precedent] }).tier === 'high', 'precedent-new-core-axis-never-offset');
  ok(deriveDiffTier(['docs/contracts/x.md', ...allMockPaths], 'expect(x).toBe(1)', {}, { declaration, precedents: [precedent] }).tier === 'critical', 'precedent-critical-never-offset');
  ok(effectiveTier(deriveDiffTier(allMockPaths, 'expect(x).toBe(1)', {}, { declaration, precedents: [precedent] }).tier, 'high').tier === 'high', 'precedent-cannot-lower-declared-high');
  const noiseSection = ['diff --git a/firmware/sdkconfig b/firmware/sdkconfig', '+#', '+# Image Effects Configuration', '+#', '+# end of Image Effects Configuration', '+#'].join('\n');
  const noiseMap = new Map([['firmware/sdkconfig', noiseSection]]);
  const noisePrecedent = { changeId: '261015-voice-rightcol-label-overflow', eventId: 'noise-event', axis: 'generated-noise', pattern: 'idf-empty-section' };
  const noiseDeclaration = { schemaVersion: 1, precedents: { 'generated-noise': { pattern: 'idf-empty-section', changeId: noisePrecedent.changeId } } };
  ok(isEmptySdkconfigRegeneration(noiseMap) === true, 'noise-empty-section-predicate-positive');
  ok(deriveDiffTier(['firmware/sdkconfig'], '', {}, { perFile: noiseMap, declaration: noiseDeclaration, precedents: [noisePrecedent] }).tier === 'low', 'noise-approved-empty-section-offset-positive');
  ok(deriveDiffTier(['firmware/sdkconfig'], '', {}, { perFile: noiseMap, declaration: noiseDeclaration, precedents: [] }).tier === 'critical', 'noise-without-precedent-still-critical');
  const substantiveMap = new Map([['firmware/sdkconfig', '+# Image Effects Configuration\n+# end of Image Effects Configuration\n+CONFIG_NEW_FEATURE=y']]);
  ok(isEmptySdkconfigRegeneration(substantiveMap) === false, 'noise-real-config-line-not-noise');
  const substantiveOutcome = deriveDiffTier(['firmware/sdkconfig'], '', {}, { perFile: substantiveMap, declaration: noiseDeclaration, precedents: [noisePrecedent] });
  ok(substantiveOutcome.tier === 'critical', 'noise-real-config-still-critical');
  ok(!substantiveOutcome.advisories.some((s) => s.includes('同款先例')), 'noise-real-config-no-misleading-precedent-hint');
  const removedMap = new Map([['firmware/sdkconfig', '-# Image Effects Configuration\n-# end of Image Effects Configuration\n+#']]);
  ok(isEmptySdkconfigRegeneration(removedMap) === false, 'noise-deletion-not-noise');
  const parsed = parseGateDeclarationText('验证形态申报（JSON）：' + JSON.stringify(declaration));
  ok(parsed.declaration?.precedents?.['all-mock']?.changeId === precedent.changeId, 'launch-declaration-parse-positive');
  ok(parseGateDeclarationText('').declaration === null, 'launch-declaration-missing-not-consumed');
  ok(parseGateDeclarationText('验证形态申报（JSON）：{bad').declaration === null, 'launch-declaration-malformed-not-consumed');
  const approvedEvent = { schemaVersion: 1, eventId: 'e1', changeId: 'x', eventType: 'human-decision', actor: 'human', payload: { decision: 'approve-escalated-merge', approvedVia: 'AskUserQuestion', gatePrecedents: [{ axis: 'all-mock', pattern: 'p' }] } };
  ok(eventApprovedPrecedents(approvedEvent).length === 1, 'human-precedent-schema-positive');
  ok(eventApprovedPrecedents({ ...approvedEvent, payload: { decision: 'approve-escalated-merge' } }).length === 0, 'approval-without-explicit-precedent-tag-not-library');
  ok(eventApprovedPrecedents({ ...approvedEvent, payload: { ...approvedEvent.payload, gatePrecedents: [{ axis: 'security', pattern: 'p' }] } }).length === 0, 'new-axis-tag-not-offsettable');

  console.log(JSON.stringify({ selfCheck: 'supervisor-verdict-gate', pass, fail: failCount, total: pass + failCount, ok: failCount === 0 }));
```

#### T2i-export-shared-gate-context · .claude/hooks/supervisor-verdict-gate.mjs

原文（现盘逐字）：
```text
  deriveDiffTier,
  collectGitDiff,
  countCompletedRounds,
  decide,
};
```

改后（逐字）：
```text
  deriveDiffTier,
  collectGitDiff,
  countCompletedRounds,
  decide,
  readGateDeclaration,
  readApprovedPrecedents,
  parseGateDeclarationText,
  matchDeclaredPrecedent,
  isEmptySdkconfigRegeneration,
};
```

#### T2j-g6-import-gate-context · .claude/hooks/supervisor-gate-check.mjs

原文（现盘逐字）：
```text
import { deriveDiffTier, collectGitDiff } from './supervisor-verdict-gate.mjs';
```

改后（逐字）：
```text
import { deriveDiffTier, collectGitDiff, readGateDeclaration, readApprovedPrecedents } from './supervisor-verdict-gate.mjs';
```

#### T2k-g6-recompute-same-context · .claude/hooks/supervisor-gate-check.mjs

原文（现盘逐字）：
```text
    const reDerived = deriveDiffTier(reGit.paths, reGit.content, {}, { perFile: reGit.perFile });
```

改后（逐字）：
```text
    const declarationInfo = readGateDeclaration(projectDir, changeId);
    const precedentInfo = readApprovedPrecedents(projectDir);
    const reDerived = deriveDiffTier(reGit.paths, reGit.content, {}, {
      perFile: reGit.perFile,
      declaration: declarationInfo.declaration,
      precedents: precedentInfo.precedents,
    });
```

#### T2l-risk-ssot-precedent-and-device-pointer · .claude/references/dev-loop.md

原文（现盘逐字）：
```text
任何 `BLOCKING` finding 都意味着不能进入合并门，必须交回 planner / fixer / 人类裁决。supervisor 合并门（机械闸 `supervisor-verdict-gate.mjs`）的低/中风险自动放行需要同时满足：真库/真实结果验收可信、mutation 逐 AC 敏感、closeout 全绿。全 mock、跨系统、core.*、安全轴触发为高风险；docs/contracts/**、迁移、删数据为 critical。**本行是 riskTier 机械判据的 SSOT**：`supervisor-verdict-gate.mjs` 是本判据的机械投影（非第二权威），改判据先改本行再同步该脚本（反漂移 C-5）。
```

改后（逐字）：
```text
任何 `BLOCKING` finding 都意味着不能进入合并门，必须交回 planner / fixer / 人类裁决。supervisor 合并门（机械闸 `supervisor-verdict-gate.mjs`）的低/中风险自动放行需要同时满足：真库/真实结果验收可信、mutation 逐 AC 敏感、closeout 全绿。全 mock、跨系统、core.*、安全轴触发为高风险；docs/contracts/**、迁移、删数据为 critical。**本段是 riskTier 机械判据的 SSOT**：261028 P8 起仅 `all-mock` 与 `generated-noise` 进入可折抵轴白名单——launch-card 申报的 changeId+pattern 必须命中已人批同型先例，且 generated-noise 另须满足 firmware/sdkconfig 纯空节判据；任何新轴、security、core、critical 均不自动折抵，先例只输出 human 快批提示。declared riskTier 仍只升不降。`supervisor-verdict-gate.mjs` 与 `supervisor-gate-check.mjs` 是本判据的同参机械投影（非第二权威），改判据先改本段再同步两脚本（反漂移 C-5）。**设备结果证据不改 riskTier**：用户可见且 host 不可编结果面的真机目击由前置升级闸作为 supervisor 合并门的 merge 期子判据消费，见 operating-system §7。
```

#### T2m-new-precedent-registry · .claude/supervisor-precedents.json（新增）

新增文件全文（逐字；装配器 copy 通道，不走 Bash 重定向）：
```json
{
  "schemaVersion": 1,
  "_doc": [
    "历史种子先例只引用已存在的 human-decision 事件，不改写历史 pod-events。",
    "新先例不双写到本文件；由对应包 pod-events.jsonl 的 human-decision.payload.gatePrecedents 累积。",
    "自动折抵只认 axis=all-mock 或 generated-noise，且必须同时匹配 changeId、axis、pattern 与 launch-card 申报。",
    "security、core、critical 与任何未登记轴永不自动折抵；先例命中信息只可作 human 快批提示。"
  ],
  "precedents": [
    {
      "changeId": "261010-prompt-v9-schedule-anti-narration",
      "eventId": "e02f05e3-e0f5-46b9-ba79-2b4027b8e090",
      "approvedVia": "AskUserQuestion(VS Code interactive)",
      "axis": "all-mock",
      "pattern": "content-anchor-live-observation",
      "sourceRef": "delivery/taskpackages/261010-prompt-v9-schedule-anti-narration/pod-events.jsonl"
    },
    {
      "changeId": "261012-kidpal-realtime-facts",
      "eventId": "9d4c1f68-2e5b-4a07-8b93-6f1a3d0c5e84",
      "approvedVia": "AskUserQuestion interactive",
      "axis": "all-mock",
      "pattern": "content-anchor-live-observation",
      "sourceRef": "delivery/taskpackages/261012-kidpal-realtime-facts/pod-events.jsonl"
    },
    {
      "changeId": "261023-agents-md-static-semantics-alignment",
      "eventId": "1c6076b2-1292-42c4-9b8f-0ba93bd3ac5c",
      "approvedVia": "main-session AskUserQuestion",
      "axis": "all-mock",
      "pattern": "prompt-static-semantics",
      "sourceRef": "delivery/taskpackages/261023-agents-md-static-semantics-alignment/pod-events.jsonl"
    }
  ]
}
```

目标文件：`.claude/hooks/supervisor-verdict-gate.mjs`（改） · `.claude/hooks/supervisor-gate-check.mjs`（改） · `.claude/supervisor-precedents.json`（新增） · `.claude/references/dev-loop.md`（改）

### T3 · 设备轮硬门落入 pre-merge，含 KPLOG 证据最小要件

**问题**：用户可见结果落在 host 不可编面时，host 全绿仍可能真机错误；defer-merge-pending-live-round 实际阻断但没有机械消费位。

**证据**：ARQ-261001 的 RSR-A6/A7 反证、lcd_display.cc 不在 host 编译面、既有 defer/真机/approve 事件链、KPLOG skill 的段号与语义缝。

**根因**：verifier 的 host/结构证据与合并所需真机目击之间缺少条件 blocking 制品及 merge 期校验。

**改法**：硬门放 pre-merge 而非 supervisor riskTier：它不改变风险高低，只在合并前检查 deviceOutcome。实际 diff 触登记 host 不可编路径即强制申报；acceptance 标记可覆盖非可见申辩。触发时逐 acceptanceRef 要求 PASS，支持人工目检、串口、KPLOG 三形态，并核来源 commit、固件 SHA-256、脱敏摘录 SHA-256。approve-escalated-merge 不豁免此子判据。

**为什么这样改**：pre-merge 是所有合并路径的最后机械消费位，能把 high-tier 人批与设备证据正交化；若塞入 supervisor verdict，会同时扩大 write-once 与 G6 的证明责任。

本 T 共 10 处旧串锚定编辑。以下原文与改后文本由同一份 _scratch/edits.json 机械展开，不作第二份手抄。

#### T3a-import-hash-and-shared-declaration · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
import { execFileSync, execSync } from 'node:child_process';
import { existsSync, readFileSync, writeFileSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
```

改后（逐字）：
```text
import { execFileSync, execSync } from 'node:child_process';
import { existsSync, readFileSync, writeFileSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { createHash } from 'node:crypto';
import { readGateDeclaration } from '../hooks/supervisor-verdict-gate.mjs';
```

#### T3b-device-outcome-helpers · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
/**
 * 主判：给定可注入上下文，产 { blocked, reasons[] }。
 * ctx = { execFn, changeId, source, kind: 'devloop'|'gov'|'unknown',
 *         supervisorVerdict, govAnchors, contractsDiff: boolean }
 */
export function evaluate(ctx) {
```

改后（逐字）：
```text

// ---- 用户可见且 host 不可编结果面的设备轮硬门（261028 P8；supervisor 合并门的 merge 期子判据）----
function parseDeviceEvidenceText(text) {
  const prefix = 'device-round-evidence（JSON）：';
  const lines = String(text || '').split(/\r?\n/).filter((l) => l.startsWith(prefix));
  if (lines.length !== 1) return { evidence: null, error: 'device-round-evidence JSON 行数=' + lines.length + '（应为 1）' };
  try {
    const evidence = JSON.parse(lines[0].slice(prefix.length));
    if (!evidence || evidence.schemaVersion !== 1) return { evidence: null, error: 'device-round-evidence schemaVersion 应为 1' };
    return { evidence, error: null };
  } catch (error) {
    return { evidence: null, error: 'device-round-evidence JSON 不可解析：' + error.message };
  }
}

function acceptanceDeviceMarkers(taskPackage, dir) {
  const acceptanceRefs = new Set();
  const hostUncompilablePaths = new Set();
  for (const ref of Array.isArray(taskPackage?.acceptanceTestsFile) ? taskPackage.acceptanceTestsFile : []) {
    const p = resolve(dir, ref);
    if (!existsSync(p)) continue;
    for (const line of readFileSync(p, 'utf8').split(/\r?\n/)) {
      let m = line.match(/device-outcome:\s*([A-Za-z0-9][A-Za-z0-9_-]*)/);
      if (m) acceptanceRefs.add(m[1]);
      m = line.match(/host-uncompilable-path:\s*(firmware\/[A-Za-z0-9_./-]+)/);
      if (m) hostUncompilablePaths.add(m[1]);
    }
  }
  return { acceptanceRefs: [...acceptanceRefs], hostUncompilablePaths: [...hostUncompilablePaths] };
}

export function assessDeviceOutcome(input, deps = {}) {
  const reasons = [];
  const advisories = [];
  const declaration = input.declaration || null;
  const declared = declaration?.deviceOutcome || null;
  const markers = input.markers || { acceptanceRefs: [], hostUncompilablePaths: [] };
  const hostExclusions = new Set(input.hostExclusions || []);
  const hostPrefixes = Array.isArray(input.hostUncompilablePrefixes) ? input.hostUncompilablePrefixes : [];
  const isHostExcluded = (p) => hostExclusions.has(p) || hostPrefixes.some((prefix) => p.startsWith(prefix));
  const touched = (input.sourceDiffPaths || []).filter(isHostExcluded);
  const markerRefs = new Set(markers.acceptanceRefs || []);
  const declaredVisible = declared?.userVisible === true;
  const requiredRefs = [...new Set([...(declaredVisible && Array.isArray(declared.acceptanceRefs) ? declared.acceptanceRefs : []), ...markerRefs])].sort();
  const declaredPaths = [...new Set([...(declaredVisible && Array.isArray(declared.hostUncompilablePaths) ? declared.hostUncompilablePaths : []), ...(markers.hostUncompilablePaths || [])])].sort();
  const hasAcceptanceDeclaration = markerRefs.size > 0 || (markers.hostUncompilablePaths || []).length > 0;

  if (touched.length > 0 && !declared && !hasAcceptanceDeclaration) {
    reasons.push('device-outcome-declaration-missing：实际 diff 触 host 不可编路径 ' + touched.join(', ') + '，launch-card 与 acceptance 均无设备结果申报');
  }
  if (declared?.userVisible === false && hasAcceptanceDeclaration) {
    reasons.push('device-outcome-declaration-conflict：launch-card 申报非用户可见，但 acceptance 含 device-outcome/host-uncompilable-path 标记');
  }
  const required = declaredVisible || hasAcceptanceDeclaration || reasons.some((r) => r.startsWith('device-outcome-declaration-missing'));
  if (!required) {
    const reason = String(declared?.reason || '').trim();
    if (touched.length > 0 && (reason.length < 12 || reason.startsWith('<') || reason === 'none')) {
      reasons.push('device-outcome-nonvisible-reason-missing：触 host 不可编路径但非用户可见的理由未填、为占位符或不足 12 字');
    }
    return { probed: true, required: reasons.length > 0, ok: reasons.length === 0, reasons, advisories, touchedHostPaths: touched, requiredAcceptanceRefs: [] };
  }

  if (requiredRefs.length === 0) reasons.push('device-outcome-acceptance-refs-empty：用户可见设备结果必须列 acceptanceRefs');
  if (declaredPaths.length === 0) reasons.push('device-outcome-host-paths-empty：用户可见设备结果必须列 hostUncompilablePaths');
  const unknownPaths = declaredPaths.filter((p) => !isHostExcluded(p));
  if (unknownPaths.length) reasons.push('device-outcome-host-path-unregistered：' + unknownPaths.join(', ') + ' 不在 verify-gates.deviceOutcomeGate.hostUncompilablePaths 登记集');
  const coveredDiffPaths = declaredPaths.filter((p) => (input.sourceDiffPaths || []).includes(p));
  if (declaredPaths.length > 0 && coveredDiffPaths.length === 0) reasons.push('device-outcome-path-not-in-diff：申报的 host 不可编路径未出现在本次合并 diff');

  const evidence = input.evidence;
  const verifyBinary = deps.verifyBinary || (() => false);
  const verifyExcerpt = deps.verifyExcerpt || (() => false);
  if (!evidence) {
    reasons.push('device-round-evidence-missing：合并前必须有 device-round-evidence.md 的结构化真机目击记录');
  } else {
    if (evidence.changeId !== input.changeId) reasons.push('device-round-evidence-changeId-mismatch');
    if (evidence.sourceCommit !== input.source) reasons.push('device-round-evidence-source-commit-mismatch：目击固件来源 commit 必须等于本次合并 HEAD');
    if (typeof evidence.binaryRef !== 'string' || !/^[0-9a-f]{64}$/.test(String(evidence.binarySha256 || '')) || !verifyBinary(evidence.binaryRef, evidence.binarySha256)) {
      reasons.push('device-round-evidence-binary-mismatch：binaryRef 不存在或 SHA-256 不符');
    }
    const observations = Array.isArray(evidence.observations) ? evidence.observations : [];
    const passedRefs = new Set();
    for (const obs of observations) {
      if (!obs || obs.result !== 'PASS' || typeof obs.acceptanceRef !== 'string') continue;
      if (obs.method === 'human-visual') {
        if (obs.observer === 'human' && typeof obs.observedAt === 'string' && obs.observedAt && typeof obs.note === 'string' && obs.note.trim().length >= 12) passedRefs.add(obs.acceptanceRef);
      } else if (obs.method === 'serial-capture' || obs.method === 'kplog') {
        const log = obs.log || {};
        const excerptOk = typeof log.excerptRef === 'string' && /^[0-9a-f]{64}$/.test(String(log.excerptSha256 || '')) && verifyExcerpt(log.excerptRef, log.excerptSha256);
        const kplogOk = obs.method !== 'kplog' || (/^KPLOG(?:0[0-2][0-9]|03[01])$/.test(String(log.segment || '')) && typeof log.window === 'string' && log.window.trim().length >= 8);
        if (excerptOk && kplogOk) passedRefs.add(obs.acceptanceRef);
      }
    }
    for (const ref of requiredRefs) if (!passedRefs.has(ref)) reasons.push('device-round-observation-missing-or-invalid：' + ref + ' 缺 PASS 目击，或证据最小要件不完整');
  }
  return {
    probed: true,
    required: true,
    ok: reasons.length === 0,
    reasons,
    advisories,
    touchedHostPaths: touched,
    declaredHostPaths: declaredPaths,
    coveredDiffPaths,
    requiredAcceptanceRefs: requiredRefs,
    evidenceRef: evidence ? 'delivery/taskpackages/' + input.changeId + '/device-round-evidence.md' : null,
  };
}

function collectDeviceOutcome(projectDir, changeId, source, sourceDiffPaths) {
  try {
    const config = JSON.parse(readFileSync(resolve(projectDir, '.claude/verify-gates.json'), 'utf8')).deviceOutcomeGate || {};
    const tpPath = resolve(projectDir, 'delivery/taskpackages', changeId, 'taskpackage.json');
    const taskPackage = existsSync(tpPath) ? JSON.parse(readFileSync(tpPath, 'utf8')) : {};
    const declaration = readGateDeclaration(projectDir, changeId).declaration;
    const markers = acceptanceDeviceMarkers(taskPackage, projectDir);
    const evidencePath = resolve(projectDir, 'delivery/taskpackages', changeId, 'device-round-evidence.md');
    const parsed = existsSync(evidencePath) ? parseDeviceEvidenceText(readFileSync(evidencePath, 'utf8')) : { evidence: null, error: null };
    const hashFile = (ref) => {
      if (typeof ref !== 'string' || ref.includes('..') || ref.startsWith('/')) return false;
      const p = resolve(projectDir, ref);
      return existsSync(p) ? createHash('sha256').update(readFileSync(p)).digest('hex') : null;
    };
    const result = assessDeviceOutcome({
      declaration,
      markers,
      hostExclusions: Array.isArray(config.hostUncompilablePaths) ? config.hostUncompilablePaths : [],
      hostUncompilablePrefixes: Array.isArray(config.hostUncompilablePrefixes) ? config.hostUncompilablePrefixes : [],
      sourceDiffPaths,
      evidence: parsed.evidence,
      source,
      changeId,
    }, {
      verifyBinary: (ref, sha) => hashFile(ref) === sha,
      verifyExcerpt: (ref, sha) => {
        const prefix = 'delivery/taskpackages/' + changeId + '/device-evidence/';
        return typeof ref === 'string' && ref.startsWith(prefix) && hashFile(ref) === sha;
      },
    });
    if (parsed.error) {
      result.required = true;
      result.ok = false;
      result.reasons.push(parsed.error);
    }
    return result;
  } catch (error) {
    return { probed: false, required: true, ok: false, reasons: ['device-outcome-probe-fail-closed：' + error.message], advisories: [] };
  }
}


/**
 * 主判：给定可注入上下文，产 { blocked, reasons[] }。
 * ctx = { execFn, changeId, source, kind: 'devloop'|'gov'|'unknown',
 *         supervisorVerdict, govAnchors, contractsDiff: boolean }
 */
export function evaluate(ctx) {
```

#### T3c-device-gate-not-human-waivable · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
  if (ctx.kind === 'devloop') {
    // T1/T2（260908 批）：并行相交面分级路由——tradeoff（③ 级）BLOCK 交 architect 消解评审；
```

改后（逐字）：
```text
  if (ctx.kind === 'devloop') {
    // 设备轮硬门（261028 P8）：approve-escalated-merge 只豁免 riskTier/verdict/blocked，不豁免真机结果证据。
    if (!ctx.deviceOutcome || ctx.deviceOutcome.probed !== true) {
      const detail = Array.isArray(ctx.deviceOutcome?.reasons) && ctx.deviceOutcome.reasons.length
        ? ctx.deviceOutcome.reasons.join('；')
        : '设备结果面探测失败（无详细原因）';
      const prefix = 'device-outcome-probe-fail-closed：';
      push((detail.startsWith(prefix) ? detail : prefix + detail) + '——升级人工核，不得自动 merge');
    } else if (ctx.deviceOutcome.required && !ctx.deviceOutcome.ok) {
      for (const reason of ctx.deviceOutcome.reasons) push(reason);
    }
    // T1/T2（260908 批）：并行相交面分级路由——tradeoff（③ 级）BLOCK 交 architect 消解评审；
```

#### T3d-build-device-context · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
  const retiredProbe = retiredPathProbe(execFn, source, 'main');

  return {
```

改后（逐字）：
```text
  const retiredProbe = retiredPathProbe(execFn, source, 'main');
  const sourceDiffPaths = kind === 'devloop'
    ? (() => { try { return execFn('git diff --name-only main...' + source).split('\n').map((l) => l.trim()).filter(Boolean); } catch { return []; } })()
    : [];
  const deviceOutcome = kind === 'devloop' ? collectDeviceOutcome(projectDir, changeId, source, sourceDiffPaths) : null;

  return {
```

#### T3e-return-device-context · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
    retiredProbe, // T1（261015-gov-review-changes-taxonomy 齿二）：合并结果树废止路径探测结果
  };
```

改后（逐字）：
```text
    retiredProbe, // T1（261015-gov-review-changes-taxonomy 齿二）：合并结果树废止路径探测结果
    deviceOutcome, // 261028 P8：设备轮硬门结果；人批豁免不消费、不覆盖
  };
```

#### T3f-self-check-default-clean-device · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
    retiredProbe: { probed: true, tree: 'x', paths: [] }, // 齿二（261015 批）：默认结果树干净 → 放行
    ...over,
```

改后（逐字）：
```text
    retiredProbe: { probed: true, tree: 'x', paths: [] }, // 齿二（261015 批）：默认结果树干净 → 放行
    deviceOutcome: { probed: true, required: false, ok: true, reasons: [], advisories: [] },
    ...over,
```

#### T3g-device-discriminating-self-checks · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
  const fail = cases.filter((c) => !c.ok);
```

改后（逐字）：
```text
  // 261028 P8：设备门三证据形态 + 缺证/错 commit/缺覆盖/新轴防漏升正反夹具。
  expect('device-required-missing-evidence-block', { deviceOutcome: { probed: true, required: true, ok: false, reasons: ['device-round-evidence-missing'] } }, true, 'device-round-evidence-missing');
  expect('device-probe-error-detail-surfaced', { deviceOutcome: { probed: false, required: true, ok: false, reasons: ['device-outcome-probe-fail-closed：fixture boom'] } }, true, 'fixture boom');
  {
    const r = evaluate(mkCtx({ deviceOutcome: { probed: false, required: true, ok: false, reasons: ['device-outcome-probe-fail-closed：fixture boom'] } }));
    const text = r.reasons.join('\n');
    cases.push({ name: 'device-probe-prefix-not-duplicated', ok: text.split('device-outcome-probe-fail-closed：').length - 1 === 1, got: text });
  }
  expect('device-human-approval-does-not-waive', { supervisorVerdict: escVerdict, humanApproval: { ok: true, reason: 'human-approval verified' }, deviceOutcome: { probed: true, required: true, ok: false, reasons: ['device-round-evidence-missing'] } }, true, 'device-round-evidence-missing');
  cases.push({ name: 'device-not-required-pass', ok: !evaluate(mkCtx({ deviceOutcome: { probed: true, required: false, ok: true, reasons: [] } })).blocked, got: '' });
  const baseDeviceInput = {
    declaration: { schemaVersion: 1, deviceOutcome: { userVisible: true, acceptanceRefs: ['RSR-A6'], hostUncompilablePaths: ['firmware/main/display/lcd_display.cc'] } },
    markers: { acceptanceRefs: [], hostUncompilablePaths: [] },
    hostExclusions: ['firmware/main/display/lcd_display.cc'],
    sourceDiffPaths: ['firmware/main/display/lcd_display.cc'],
    source: 'a'.repeat(40),
    changeId: '261028-device-fixture',
  };
  const baseEvidence = {
    schemaVersion: 1,
    changeId: baseDeviceInput.changeId,
    sourceCommit: baseDeviceInput.source,
    binaryRef: 'firmware/build/xiaozhi.bin',
    binarySha256: 'b'.repeat(64),
    observations: [],
  };
  const deviceDeps = { verifyBinary: () => true, verifyExcerpt: () => true };
  const observationFor = (method) => method === 'human-visual'
    ? { acceptanceRef: 'RSR-A6', result: 'PASS', method, observer: 'human', observedAt: '2026-10-28T10:00:00+08:00', note: 'human 持板确认录音态键帽与主标题均正确' }
    : { acceptanceRef: 'RSR-A6', result: 'PASS', method, log: { segment: method === 'kplog' ? 'KPLOG006' : undefined, window: method === 'kplog' ? '2026-10-28 10:00:00–10:00:05' : undefined, excerptRef: 'delivery/taskpackages/261028-device-fixture/device-evidence/rsr-a6.txt', excerptSha256: 'c'.repeat(64) } };
  for (const method of ['human-visual', 'serial-capture', 'kplog']) {
    const r = assessDeviceOutcome({ ...baseDeviceInput, evidence: { ...baseEvidence, observations: [observationFor(method)] } }, deviceDeps);
    cases.push({ name: 'device-evidence-' + method + '-pass', ok: r.required && r.ok, got: JSON.stringify(r.reasons) });
  }
  const badKplog = { ...observationFor('kplog'), log: { ...observationFor('kplog').log, segment: '', window: '' } };
  const badKplogResult = assessDeviceOutcome({ ...baseDeviceInput, evidence: { ...baseEvidence, observations: [badKplog] } }, deviceDeps);
  cases.push({ name: 'device-kplog-segment-and-window-required', ok: !badKplogResult.ok && badKplogResult.reasons.some((r) => r.includes('RSR-A6')), got: JSON.stringify(badKplogResult.reasons) });
  const outOfRangeKplog = { ...observationFor('kplog'), log: { ...observationFor('kplog').log, segment: 'KPLOG032' } };
  const outOfRangeResult = assessDeviceOutcome({ ...baseDeviceInput, evidence: { ...baseEvidence, observations: [outOfRangeKplog] } }, deviceDeps);
  cases.push({ name: 'device-kplog-032-out-of-rotation-block', ok: !outOfRangeResult.ok && outOfRangeResult.reasons.some((r) => r.includes('RSR-A6')), got: JSON.stringify(outOfRangeResult.reasons) });
  const badExcerptResult = assessDeviceOutcome({ ...baseDeviceInput, evidence: { ...baseEvidence, observations: [observationFor('kplog')] } }, { ...deviceDeps, verifyExcerpt: () => false });
  cases.push({ name: 'device-kplog-excerpt-hash-mismatch-block', ok: !badExcerptResult.ok && badExcerptResult.reasons.some((r) => r.includes('RSR-A6')), got: JSON.stringify(badExcerptResult.reasons) });
  const wrongCommit = assessDeviceOutcome({ ...baseDeviceInput, evidence: { ...baseEvidence, sourceCommit: 'd'.repeat(40), observations: [observationFor('kplog')] } }, deviceDeps);
  cases.push({ name: 'device-wrong-commit-block', ok: !wrongCommit.ok && wrongCommit.reasons.some((r) => r.includes('source-commit-mismatch')), got: JSON.stringify(wrongCommit.reasons) });
  const missingCoverage = assessDeviceOutcome({ ...baseDeviceInput, declaration: { schemaVersion: 1, deviceOutcome: { userVisible: true, acceptanceRefs: ['RSR-A6', 'RSR-A7'], hostUncompilablePaths: ['firmware/main/display/lcd_display.cc'] } }, evidence: { ...baseEvidence, observations: [observationFor('kplog')] } }, deviceDeps);
  cases.push({ name: 'device-missing-acceptance-coverage-block', ok: !missingCoverage.ok && missingCoverage.reasons.some((r) => r.includes('RSR-A7')), got: JSON.stringify(missingCoverage.reasons) });
  const noDeclaration = assessDeviceOutcome({ ...baseDeviceInput, declaration: null, evidence: { ...baseEvidence, observations: [observationFor('kplog')] } }, deviceDeps);
  cases.push({ name: 'device-host-diff-without-declaration-block', ok: noDeclaration.required && !noDeclaration.ok && noDeclaration.reasons.some((r) => r.includes('declaration-missing')), got: JSON.stringify(noDeclaration.reasons) });
  const nonVisible = assessDeviceOutcome({ ...baseDeviceInput, declaration: { schemaVersion: 1, deviceOutcome: { userVisible: false, reason: '仅设备内部诊断日志接线，不改变任何用户可见结果' } }, evidence: null }, deviceDeps);
  cases.push({ name: 'device-nonvisible-with-reason-pass', ok: !nonVisible.required && nonVisible.ok, got: JSON.stringify(nonVisible.reasons) });
  const defaultNone = assessDeviceOutcome({ ...baseDeviceInput, declaration: { schemaVersion: 1, deviceOutcome: { userVisible: false, reason: 'none' } }, evidence: null }, deviceDeps);
  cases.push({ name: 'device-default-none-touched-host-fail-closed', ok: !defaultNone.ok && defaultNone.reasons.some((r) => r.includes('nonvisible-reason-missing')), got: JSON.stringify(defaultNone.reasons) });
  const defaultNoneEval = evaluate(mkCtx({ deviceOutcome: defaultNone }));
  cases.push({ name: 'device-default-none-evaluate-block', ok: defaultNoneEval.blocked && defaultNoneEval.reasons.some((r) => r.includes('nonvisible-reason-missing')), got: JSON.stringify(defaultNoneEval) });
  const placeholderReason = assessDeviceOutcome({ ...baseDeviceInput, declaration: { schemaVersion: 1, deviceOutcome: { userVisible: false, reason: '<无用户可见结果面时填理由；无设备面可写 none>' } }, evidence: null }, deviceDeps);
  cases.push({ name: 'device-placeholder-reason-touched-host-fail-closed', ok: !placeholderReason.ok && placeholderReason.reasons.some((r) => r.includes('nonvisible-reason-missing')), got: JSON.stringify(placeholderReason.reasons) });
  const placeholderEval = evaluate(mkCtx({ deviceOutcome: placeholderReason }));
  cases.push({ name: 'device-placeholder-reason-evaluate-block', ok: placeholderEval.blocked && placeholderEval.reasons.some((r) => r.includes('nonvisible-reason-missing')), got: JSON.stringify(placeholderEval) });
  const boardPath = 'firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc';
  const boardOutcome = assessDeviceOutcome({ ...baseDeviceInput, declaration: { schemaVersion: 1, deviceOutcome: { userVisible: true, acceptanceRefs: ['RSR-A6'], hostUncompilablePaths: [boardPath] } }, sourceDiffPaths: [boardPath], hostUncompilablePrefixes: ['firmware/main/boards/'], evidence: { ...baseEvidence, observations: [observationFor('kplog')] } }, deviceDeps);
  cases.push({ name: 'device-board-wiring-prefix-covered', ok: boardOutcome.required && boardOutcome.ok, got: JSON.stringify(boardOutcome.reasons) });
  const acceptanceOverrides = assessDeviceOutcome({ ...baseDeviceInput, declaration: { schemaVersion: 1, deviceOutcome: { userVisible: false, reason: '错误申辩：实际 acceptance 已声明用户可见结果' } }, markers: { acceptanceRefs: ['RSR-A6'], hostUncompilablePaths: ['firmware/main/display/lcd_display.cc'] }, evidence: { ...baseEvidence, observations: [observationFor('kplog')] } }, deviceDeps);
  cases.push({ name: 'device-acceptance-marker-overrides-false-declaration', ok: acceptanceOverrides.required && !acceptanceOverrides.ok && acceptanceOverrides.reasons.some((r) => r.includes('declaration-conflict')), got: JSON.stringify(acceptanceOverrides.reasons) });
  const unknownHostPath = assessDeviceOutcome({ ...baseDeviceInput, hostExclusions: [], evidence: { ...baseEvidence, observations: [observationFor('kplog')] } }, deviceDeps);
  cases.push({ name: 'device-unregistered-host-path-fail-closed', ok: !unknownHostPath.ok && unknownHostPath.reasons.some((r) => r.includes('host-path-unregistered')), got: JSON.stringify(unknownHostPath.reasons) });

  const fail = cases.filter((c) => !c.ok);
```

#### T3h-probe-out-device-result · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
    writeFileSync(probeOutPath, JSON.stringify({ changeId, kind: ctx.kind, source: ctx.source, overlap: ctx.overlap }, null, 2), 'utf8');
```

改后（逐字）：
```text
    writeFileSync(probeOutPath, JSON.stringify({ changeId, kind: ctx.kind, source: ctx.source, overlap: ctx.overlap, deviceOutcome: ctx.deviceOutcome }, null, 2), 'utf8');
```

#### T3i-header-device-gate-line · .claude/scripts/pre-merge-escalation-gate.mjs

原文（现盘逐字）：
```text
//        delivery/changes——dev lane 已由 261014 批撤销、伞已解散；scope-map 齿一拦 agent 会话写入，
//        本齿拦 git merge 通道带入旧布局分支）→ BLOCK，指引归位真家（映射见 delivery/archive/README.md）
```

改后（逐字）：
```text
//        delivery/changes——dev lane 已由 261014 批撤销、伞已解散；scope-map 齿一拦 agent 会话写入，
//        本齿拦 git merge 通道带入旧布局分支）→ BLOCK，指引归位真家（映射见 delivery/archive/README.md）
//     8. 用户可见且 host 不可编结果面缺真机目击证据（261028 P8；deviceOutcome.required=true）→ BLOCK；
//        approve-escalated-merge 不豁免本子判据，证据最小要件含 human-visual / serial-capture / KPLOG 三形态之一
```

#### T3j-device-gate-config · .claude/verify-gates.json

原文（现盘逐字）：
```text
  ],
  "gates": [
```

改后（逐字）：
```text
  ],
  "deviceOutcomeGate": {
    "_doc": "前置升级闸的设备轮子判据配置，不是 verify.mjs 新增闸。consumer=.claude/scripts/pre-merge-escalation-gate.mjs。hostUncompilablePaths/Prefixes 是首批已登记排除表、非理论完备集；申报未登记路径或路径不在合并 diff 均 fail-closed。",
    "evidenceRef": "device-round-evidence.md",
    "hostUncompilablePaths": [
      "firmware/main/application.cc",
      "firmware/main/mcp_server.cc",
      "firmware/main/display/lcd_display.cc",
      "firmware/main/display/kidpal_render_node.cc",
      "firmware/main/diagnostics/sd_log_fs.cc",
      "firmware/main/diagnostics/log_cmd_port.cc"
    ],
    "hostUncompilablePrefixes": [
      "firmware/main/boards/"
    ]
  },
  "gates": [
```

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改） · `.claude/verify-gates.json`（改）

### T4 · 设备轮配套文面：chrome、证据收口、defer 链、日志保留与行为观察

**问题**：设备轮责任分散在 planner/acceptance/builder/integrator 与 skill 中，chrome 三件、诊断日志保留和 builder 行为面观察没有固定写点。

**证据**：2026-10-25 loop-retro T12 的 chrome 漏项、261001 纯日志诊断轮、261010 builder 主动上报运行时断链、现有 device-round-checklist。

**根因**：只有结果门的设想不足以生产证据；冻结、实现、收口和日志拉取各角色缺少同一条最小义务。

**改法**：acceptance 冻结新增 chrome 三件二选一与 device-outcome 标记；builder 交付新增行为面观察；integrator 新增设备证据收口；OS 承认 defer 常态链并规定日志保留；KPLOG skill 规定段号/时间窗/脱敏摘录引用；planner 明确人批不豁免设备证据。

**为什么这样改**：这些文面是硬门的生产侧，不是第二套口径；每处只写本角色动作，机械判据仍只在 pre-merge。

本 T 共 9 处旧串锚定编辑。以下原文与改后文本由同一份 _scratch/edits.json 机械展开，不作第二份手抄。

#### T4a-chrome-three-and-device-markers · .claude/agents/acceptance-author.md

原文（现盘逐字）：
```text
5. **UI 验收（有 `uiAcceptanceRefs[]` / 冻结 brief refs 时）**：把 task-critical UI 陈述转成真实断言——task-critical 状态、数据可见性/内容正确性、empty/error/permission/loading 状态、navigation/deep-link 行为、业务 gate/confirmation 状态、基本 a11y 可操作性。纯美学/微视觉（间距/颜色/非关键动画/视觉打磨）**不**转成脆性断言，留 design review。
6. 运行红基线；失败必须对应当前 story 缺失的真实行为。
```

改后（逐字）：
```text
5. **UI 验收（有 `uiAcceptanceRefs[]` / 冻结 brief refs 时）**：把 task-critical UI 陈述转成真实断言——task-critical 状态、数据可见性/内容正确性、empty/error/permission/loading 状态、navigation/deep-link 行为、业务 gate/confirmation 状态、基本 a11y 可操作性。纯美学/微视觉（间距/颜色/非关键动画/视觉打磨）**不**转成脆性断言，留 design review。
5b. **新态 chrome 三件自查（触 UI 包冻结前必答）**：逐个新态问「键帽槽 / 页点 / 主标题画法」是否各有验收断言；无者必须二选一——在 `decisionCoverage` / TaskPackage notes 标 `dropped`+理由，或挂真机目检并写入 `device-round-evidence.md` 的覆盖项，不得默认留在 integrator 语义 review 补刀。**设备结果标记**：验收条声明用户可见结果且实现落在 host 不可编面时，在冻结件头注写 `device-outcome: <acceptanceRef>` 与 `host-uncompilable-path: <firmware/...>`；两者成对、路径须与 launch-card 申报一致。该标记是前置升级闸的触发输入，不是注释装饰。
6. 运行红基线；失败必须对应当前 story 缺失的真实行为。
```

#### T4b-builder-behavior-observation · .claude/agents/builder.md

原文（现盘逐字）：
```text
- 本地 tsc/相关测试结果。
- docs/security/ADR 发现项，供 integrator 或 planner 处理。
```

改后（逐字）：
```text
- 本地 tsc/相关测试结果。
- **行为面观察（申报节，无也写 none）**：实现 / 自测中看到的运行时断链、host 与真机可能不等价的接缝、需要后续真机轮点名的可见结果；每项带日志 / KPLOG / 串口证据指针或明确「未运行」，不得只交测试绿结论。
- docs/security/ADR 发现项，供 integrator 或 planner 处理。
```

#### T4c-device-evidence-closeout-step · .claude/agents/integrator.md

原文（现盘逐字）：
```text
9. 若里程碑阶段跳变，更新 `delivery/milestones/milestones.jsonl` 并运行 `node .claude/scripts/delivery-status.mjs`；跨切片 workstream 要更新 `delivery/workstreams/<workstream>/progress.md`。
```

改后（逐字）：
```text
8d. **设备轮证据收口（deviceOutcome 触发时）**：把真机结果整理进 `delivery/taskpackages/<change-id>/device-round-evidence.md`，首行后恰一行 `device-round-evidence（JSON）：<单行 JSON>`。`sourceCommit` 必须等于合并 HEAD，`binaryRef/binarySha256` 必须对被目击固件；每个申报 acceptanceRef 各有一条 PASS observation。三形态最小要件：human-visual = human observer + observedAt + 具体现象；serial-capture = 包内脱敏摘录 + SHA-256；KPLOG = 段号 + 时间窗 + 包内脱敏摘录 + SHA-256。KPLOG 拉取与语义缝见 `kidpal-device-logs` skill；完整原始日志留仓外，包内只放最小语义段。结果同时写入 Review Packet「设备轮证据」节；缺项不伪造 PASS，交 planner 停等真机轮。
9. 若里程碑阶段跳变，更新 `delivery/milestones/milestones.jsonl` 并运行 `node .claude/scripts/delivery-status.mjs`；跨切片 workstream 要更新 `delivery/workstreams/<workstream>/progress.md`。
```

#### T4d-integrator-verification-device-evidence · .claude/agents/integrator.md

原文（现盘逐字）：
```text
- 已晋升 `delivery/taskpackages/<change-id>/docs-delta.md`、`review-packet.md`；post-cutoff 已确认不产 session-digest/loop-health/loop-metrics/pod-journal，并对 pod-events 做 non-blocking advisory 检查；pre-cutoff 保留 legacy 制品。另记录 `verdict.json` + `verification.json`、ADR、`delivery/workstreams`/`delivery/milestones`、`closeout-check`、commit、PR 与回滚证据，并明确等待 supervisor 裁断。
```

改后（逐字）：
```text
- 已晋升 `delivery/taskpackages/<change-id>/docs-delta.md`、`review-packet.md`；post-cutoff 已确认不产 session-digest/loop-health/loop-metrics/pod-journal，并对 pod-events 做 non-blocking advisory 检查；pre-cutoff 保留 legacy 制品。另记录 `verdict.json` + `verification.json`、ADR、`delivery/workstreams`/`delivery/milestones`、`closeout-check`、commit、PR 与回滚证据，并明确等待 supervisor 裁断。deviceOutcome 触发时另交 `device-round-evidence.md` 与 `device-evidence/` 最小脱敏摘录；未触发写 `none`。
```

#### T4e-merge-discipline-defer-chain-and-log-retention · .claude/references/dev-loop-operating-system.md

原文（现盘逐字）：
```text
- **并行开发、串行合并**：N 个 Pod 并行开发、各自闭环（含 integrator+supervisor）。supervisor APPROVE 后标 ready-to-merge。merge-driver.mjs（planner 自动触发，单 Pod --pod）按 mergeOrder 逐个：拿 token → rebase 到当前 main → quick-check → merge → 释放 token。supervisor 在 Pod 内把关（APPROVE/BLOCK/ESCALATE），merge-driver 执行机械合并（本地 --no-ff 可回滚）。谁都不能并行直落 main。
```

改后（逐字）：
```text
- **并行开发、串行合并**：N 个 Pod 并行开发、各自闭环（含 integrator+supervisor）。supervisor APPROVE 后标 ready-to-merge。merge-driver.mjs（planner 自动触发，单 Pod --pod）按 mergeOrder 逐个：拿 token → rebase 到当前 main → quick-check → merge → 释放 token。supervisor 在 Pod 内把关（APPROVE/BLOCK/ESCALATE），merge-driver 执行机械合并（本地 --no-ff 可回滚）。谁都不能并行直落 main。
- **设备轮是 supervisor 合并门的 merge 期子判据，不是第七个工程门**：凡验收声明用户可见结果且实现落在 host 不可编面，合并前必须有包内 `device-round-evidence.md`；前置升级闸机械消费，human 的 approve-escalated-merge 只处理 riskTier / verdict / blocked，不豁免此证据。`defer-merge-pending-live-round` 是既有常态收口链的正式名称：supervisor ESCALATE → 冻结分支不合并 → 烧录 / 部署并跑真机轮 → 结果落包制品 → 再裁 approve-escalated-merge；它不是「先合并、后补看」的许可。
- **诊断日志保留债成文**：用于定位或证明设备结果的 KPLOG / 串口日志不得随当轮收尾即删；完整原始件可留仓外，包内必须保留最小脱敏语义段、段号 / 时间窗与 SHA-256，使 review-packet 的结论可复核。含儿童语音转写的完整日志不入公开仓。
```

#### T4f-device-evidence-artifact-registration · .claude/references/dev-loop.md

原文（现盘逐字）：
```text
- `spike-evidence.md`（261027 批 T1，ARQ-261024）：新机制批 spike 相位的消费端实证记录——打点 / 日志片段 + 观测结论 + 首尾 canary 结果（spike 定义与验收锚见「新机制批 spike 相位」节）。**惯例非门**：不入 closeout 必交清单、不进任何闸；消费者 = 正式实现包的 intake（`planner.md` Step 1i）与 review-packet 的未覆盖面申报（写清「本批依赖的端到端链路已验证到什么程度」）。
```

改后（逐字）：
```text
- `spike-evidence.md`（261027 批 T1，ARQ-261024）：新机制批 spike 相位的消费端实证记录——打点 / 日志片段 + 观测结论 + 首尾 canary 结果（spike 定义与验收锚见「新机制批 spike 相位」节）。**惯例非门**：不入 closeout 必交清单、不进任何闸；消费者 = 正式实现包的 intake（`planner.md` Step 1i）与 review-packet 的未覆盖面申报（写清「本批依赖的端到端链路已验证到什么程度」）。
- `device-round-evidence.md` + `device-evidence/`（261028 P8）：用户可见且 host 不可编结果面的**条件 blocking** 制品。未触发 deviceOutcome 时不要求；触发时缺记录、来源 commit / 固件 SHA-256 不符、申报 acceptanceRef 未逐条 PASS 或证据最小要件缺失，前置升级闸 BLOCK。三形态 = 人工在场目检 / 串口 capture / KPLOG 拉取；KPLOG 必须含段号、时间窗与包内脱敏摘录。完整原始日志留仓外，公开仓只留最小语义段。
```

#### T4g-kplog-evidence-citation-contract · .claude/skills/kidpal-device-logs/SKILL.md

原文（现盘逐字）：
```text
Related: [[kidpal-flash-device]] [[kidpal-serial-monitor]] [[kidpal-ecs-gateway]] [[kidpal-live-smoke-debug]]
```

改后（逐字）：
```text
## 作为设备轮合并证据

KPLOG 可以独立作为真机目击佐证，不必另写「人工在场目检」。在包内 `device-round-evidence.md` 的 observation 写 `method: "kplog"`，并同时给：① `KPLOG000..031` 段号；② 覆盖观察行为的墙上时间窗（未同步时标 `BOOT+ms` 窗口）；③ 包内 `device-evidence/` 的最小脱敏语义摘录路径；④ 该摘录的 SHA-256。摘录必须能判别所声明的 acceptanceRef（如 PAGE_SWITCH / state_reject / key_route_dispatch / StateMachine 转移），只拉 tail 而不指到具体行为不算证据。

**隐私与保留**：完整 KPLOG 可含儿童语音转写，原文件留仓外；公开仓只提交完成判定所需的最小语义行，不提交整段对话原文。原始件在结论复核期不得随当轮收尾即删。

Related: [[kidpal-flash-device]] [[kidpal-serial-monitor]] [[kidpal-ecs-gateway]] [[kidpal-live-smoke-debug]]
```

#### T4h-single-pod-human-approval-precedent-and-device · .claude/agents/planner.md

原文（现盘逐字）：
```text
human 批准合并后：planner 落一条 pod-events `human-decision` 事件（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ 直跑 `node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>` 走标准 Step 1b→合并链（blocked 停等态由闸内联动豁免消解，OQ2=(a)）；人批不豁免 off-main/契约/provenance，闸照拦。
```

改后（逐字）：
```text
human 批准合并后：planner 落一条 pod-events `human-decision` 事件（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ 直跑 `node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>` 走标准 Step 1b→合并链（blocked 停等态由闸内联动豁免消解，OQ2=(a)）；人批不豁免 off-main/契约/provenance/deviceOutcome 真机证据，闸照拦。仅当 human 另明确说「登记为同型先例」时，payload 才附 `approvedVia` 与 `gatePrecedents:[{axis,pattern}]`（axis 仅 all-mock/generated-noise）；批准本次合并不自动等于授权复用为先例。
```

#### T4i-multi-pod-human-approval-precedent-and-device · .claude/agents/planner.md

原文（现盘逐字）：
```text
human 批准合并后：planner 落一条 pod-events `human-decision` 事件（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ 直跑 `node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>`（仍经 merge token 串行化；merge-gate 停等态由闸内联动豁免消解，OQ2=(a)）；人批不豁免 off-main/契约/provenance，闸照拦。
```

改后（逐字）：
```text
human 批准合并后：planner 落一条 pod-events `human-decision` 事件（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ 直跑 `node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>`（仍经 merge token 串行化；merge-gate 停等态由闸内联动豁免消解，OQ2=(a)）；人批不豁免 off-main/契约/provenance/deviceOutcome 真机证据，闸照拦。先例标签的同型授权与字段规则同上单 Pod 段，不得由 planner 自行补记。
```

目标文件：`.claude/agents/acceptance-author.md`（改） · `.claude/agents/builder.md`（改） · `.claude/agents/integrator.md`（改） · `.claude/agents/planner.md`（改） · `.claude/references/dev-loop-operating-system.md`（改） · `.claude/references/dev-loop.md`（改） · `.claude/skills/kidpal-device-logs/SKILL.md`（改）

### T5 · 两条 ARQ 路径修正与结项记账

**问题**：ARQ-261014 的 supervisor 路径已漂移；两条目需要在本批落地后留下实际承载与结项状态。

**证据**：现盘 supervisor 位于 hooks；两条目仍为 deferred。

**根因**：条目登记后脚本迁址未回写，闭环凭据尚未形成。

**改法**：修正 supervisor 路径，落地时翻 closed/closedBy 并 append 结项追注；debt-render 负责归位，INDEX 不手改。

**为什么这样改**：账本只记已落地的真实改法，不在 DRAFT 阶段提前宣称完成。

本 T 共 8 处旧串锚定编辑。以下原文与改后文本由同一份 _scratch/edits.json 机械展开，不作第二份手抄。

#### T5a-arq261014-path-drift-fix · docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md

原文（现盘逐字）：
```text
paths: [".claude/scripts/supervisor-verdict-gate.mjs", ".claude/scripts/pre-merge-escalation-gate.mjs"]
```

改后（逐字）：
```text
paths: [".claude/hooks/supervisor-verdict-gate.mjs", ".claude/scripts/pre-merge-escalation-gate.mjs"]
```

#### T5b-arq261014-status · docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md

原文（现盘逐字）：
```text
status: deferred
```

改后（逐字）：
```text
status: closed
```

#### T5c-arq261014-closed · docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md

原文（现盘逐字）：
```text
closed: ""
```

改后（逐字）：
```text
closed: 2026-10-28
```

#### T5d-arq261014-closedby · docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md

原文（现盘逐字）：
```text
closedBy: ""
```

改后（逐字）：
```text
closedBy: 261028-gov-review-p8-gate-semantics-cluster
```

#### T5e-arq261014-close-note · docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md

原文（现盘逐字）：
```text
**追加证据（loop-retro-2026-10-25，25 包复盘）**：① 261023-agents-md-static-semantics-alignment——all-mock 启发式把 prompt 文本对齐包判 high 并 ESCALATE，retro 称「结构上内生于该包型」，human 再次逐次人批；② 261015-voice-rightcol-label-overflow——G6 riskTier 重算把 firmware/sdkconfig 的 idf.py 再生噪音（空节 +5 行、零行为、不属 changeManifest）判 critical 并 ESCALATE，剥离噪音 commit 后 APPROVE。两类共性未变：闸不消费 launch-card 前置申报，也不区分「行为面真变」与「生成/投影面噪音」。建议修法补一条「噪音路径白名单 / 申报消费」子项。证据：delivery/retro/2026-10-25-loop-retro.md §二 T11。
```

改后（逐字）：
```text
**追加证据（loop-retro-2026-10-25，25 包复盘）**：① 261023-agents-md-static-semantics-alignment——all-mock 启发式把 prompt 文本对齐包判 high 并 ESCALATE，retro 称「结构上内生于该包型」，human 再次逐次人批；② 261015-voice-rightcol-label-overflow——G6 riskTier 重算把 firmware/sdkconfig 的 idf.py 再生噪音（空节 +5 行、零行为、不属 changeManifest）判 critical 并 ESCALATE，剥离噪音 commit 后 APPROVE。两类共性未变：闸不消费 launch-card 前置申报，也不区分「行为面真变」与「生成/投影面噪音」。建议修法补一条「噪音路径白名单 / 申报消费」子项。证据：delivery/retro/2026-10-25-loop-retro.md §二 T11。

**2026-10-28 · 结项（`261028-gov-review-p8-gate-semantics-cluster`）**：按 human 立项选定 ①+②+③ 落地——launch-card 增机器可读验证形态申报；supervisor 与 G6 同参消费申报及已人批同型先例，all-mock 命中降级为 advisory 留痕，ESCALATE 仍带先例快批提示；新先例经 human-decision 的 approvedVia + gatePrecedents 累积。折抵红线逐轴白名单：只 all-mock / generated-noise，security/core/critical/新轴零折抵，declared riskTier 只升不降。generated-noise 只认 firmware/sdkconfig 纯空节且同样需申报+先例，真实配置行仍 critical。frontmatter 的 supervisor 路径由 scripts 修正为 hooks，消除条目登记时漂移。
```

#### T5f-arq261001-status · docs/debt/gov/ARQ-261001-host-blind-outcome-device-gate.md

原文（现盘逐字）：
```text
status: deferred
```

改后（逐字）：
```text
status: closed
```

#### T5g-arq261001-add-closed-fields · docs/debt/gov/ARQ-261001-host-blind-outcome-device-gate.md

原文（现盘逐字）：
```text
opened: 2026-10-01
paths: [.claude/hooks/verify.mjs, .claude/verify-gates.json, .claude/scripts/pre-merge-escalation-gate.mjs, .claude/references/dev-loop-operating-system.md]
```

改后（逐字）：
```text
opened: 2026-10-01
closed: 2026-10-28
closedBy: 261028-gov-review-p8-gate-semantics-cluster
paths: [.claude/hooks/verify.mjs, .claude/verify-gates.json, .claude/scripts/pre-merge-escalation-gate.mjs, .claude/references/dev-loop-operating-system.md]
```

#### T5h-arq261001-close-note · docs/debt/gov/ARQ-261001-host-blind-outcome-device-gate.md

原文（现盘逐字）：
```text
**追加证据（loop-retro-2026-10-25，25 包复盘）**：261016-p3-story-body-sd-cache——覆盖层底栏键帽槽文案（说话·双击看/下移）、页点未隐藏、story_page 自画键帽被不透明底条遮挡（死绘制），三项全部不在冻结齿里，靠 integrator 五轴语义 review 抓住 BLOCKING 后 tighten-only 补齿。retro 判词：「行为对了不等于界面对了；『可见面』是一类独立的验收轴，靠人工语义 review 补刀不可复制」。同族正向：261016-sd-card-mount 的诚实豁免形态（代码面静态断言，明确不冒充真机观察）；261020-p3-rec-frame-visual 的真机目检追加修订。建议本条修法增一条 acceptance 侧动作：触 UI 的包验收冻结时显式问「新态 chrome 三件（键帽槽/页点/主标题画法）各有齿吗」，无齿须二选一申报 dropped 或挂真机目检。证据：delivery/retro/2026-10-25-loop-retro.md §二 T12。
```

改后（逐字）：
```text
**追加证据（loop-retro-2026-10-25，25 包复盘）**：261016-p3-story-body-sd-cache——覆盖层底栏键帽槽文案（说话·双击看/下移）、页点未隐藏、story_page 自画键帽被不透明底条遮挡（死绘制），三项全部不在冻结齿里，靠 integrator 五轴语义 review 抓住 BLOCKING 后 tighten-only 补齿。retro 判词：「行为对了不等于界面对了；『可见面』是一类独立的验收轴，靠人工语义 review 补刀不可复制」。同族正向：261016-sd-card-mount 的诚实豁免形态（代码面静态断言，明确不冒充真机观察）；261020-p3-rec-frame-visual 的真机目检追加修订。建议本条修法增一条 acceptance 侧动作：触 UI 的包验收冻结时显式问「新态 chrome 三件（键帽槽/页点/主标题画法）各有齿吗」，无齿须二选一申报 dropped 或挂真机目检。证据：delivery/retro/2026-10-25-loop-retro.md §二 T12。

**2026-10-28 · 结项（`261028-gov-review-p8-gate-semantics-cluster`）**：按 human 选定「①硬门+配套全套」落地——用户可见且 host 不可编结果面由前置升级闸在 merge 期强制消费 device-round-evidence，approve-escalated-merge 不豁免；defer-merge-pending-live-round 常态链在 operating-system §7 成文。证据三形态均可：人工在场目检 / 串口 capture / KPLOG 拉取；KPLOG 必须含段号、时间窗与包内脱敏摘录+SHA-256，落实 human「我们现在有日志，日志可以作为佐证」。chrome 三件自查、诊断日志保留、builder 行为面观察申报同批落。候选②结果决策抽 host 可测缝保留为 dev-loop 个案做法，不属本批。
```

目标文件：`docs/debt/gov/ARQ-261014-supervisor-escalate-preflight-ignored.md`（改） · `docs/debt/gov/ARQ-261001-host-blind-outcome-device-gate.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 风险分级 G3 | supervisor-verdict-gate + supervisor-gate-check | 改 | 申报/先例同参派生；主闸产 verdict，Stop 钩第二方重算 |
| 合并前置 G3 | pre-merge-escalation-gate + verify-gates.json | 改 | 设备轮子判据与 host 不可编登记集；merge-driver 前最后消费位 |
| 先例配置 G3 | supervisor-precedents.json | 新增 | 仅历史种子，真实事件逐一反查；未来先例不双写 |
| 角色与流程 G2 | planner / acceptance-author / builder / integrator | 改 | 先例授权、chrome 自查、行为观察、设备证据收口 |
| 共享规则 G2 | dev-loop.md + dev-loop-operating-system.md | 改 | riskTier SSOT、defer 常态链、条件 blocking 制品与日志保留 |
| 技能 G2 | launch-prep + device-logs | 改 | 启动卡申报格式、KPLOG 证据引用与隐私边界 |
| 账本 G4 | 两条 ARQ | 改 | 路径修正 + 结项追注；debt-render 归位 |
| G1 / 契约 / spec / 实现面 | AGENTS.md、CLAUDE.md、DEV-GOVERNANCE、docs/contracts、requirements、architecture、firmware、server | 未触 | 零 diff；设备门折进既有 supervisor 合并门，不改六门枚举 |

**新判定 pattern 全仓预跑**：2026-10-28 对 canonical `.claude/**`、历史包 `delivery/taskpackages/**` 与账本 `docs/debt/**` 分别搜索 `deviceOutcome|device-round-evidence|gatePrecedents|supervisor-precedents|hostUncompilablePaths|hostUncompilablePrefixes` 及 `device-outcome:|host-uncompilable-path:|验证形态申报（JSON）`，三面均为 **No matches found**；本批 `_scratch` 与仓外装配副本不计入。故没有既有机器格式需要兼容，历史自然语言申报只作背景，不参与折抵。

**host 编译面 sweep**：读取 `firmware/main/CMakeLists.txt` 与 `firmware/main/test/host_app/{CMakeLists.txt,main/CMakeLists.txt}`，host runner 是独立 linux-target 项目、只显式接入 HOST_ACCEPTANCE_SRCS；登记集取条目已实证与文面已点名的六个设备文件，并补 `firmware/main/boards/` 前缀。该集合明示为首批登记、非理论完备集；新路径未登记会 fail-closed，而不是静默放行。

**路径形态纪律**：本批不搬家、不改名，只修 ARQ frontmatter 的一处目录 token；`scripts/supervisor-verdict-gate.mjs` → `hooks/supervisor-verdict-gate.mjs` 按 path-form-family 的裸路径与 backtick 两形态核对。账本正文不全局清扫历史引用，append-only 保留。

## 4. human 取舍点（OQ）

### OQ-1 · 先例库载体如何分工

**背景**：历史人批已在 pod-events，但没有 gatePrecedents 标签；历史文件 append-only，不宜改写旧行。新人批又必须在 Pod 内可写，而 planner 不能直接更新 `.claude` 配置。

- **(a) 只使用 pod-events**：对历史包追加带 backfill 标记的新事件，未来也从事件读取。优点是不新增配置；代价是要给多个历史包补账， retro 计数会新增事件，且每次闸运行都扫全部包。
- **(b) 只使用独立登记文件**：查询集中，但新人批后需要 Pod 外主线程二次登记，容易形成「人批已发生、先例未入册」的时延与双写漂移。
- **(c) 历史种子文件 + 未来 pod-events，职责不重叠（推荐，当前草案）**：种子文件只引用三条既有真实事件；未来先例只写 pod-events，不回写种子。优点是历史不改写、未来无双写；代价是读取方有两个来源，需要去重与各自核验（本稿已实现并实测三枚种子）。

**human 裁定（2026-10-28）**：选 **(c)**——先例库载体 = 历史种子文件 + 未来只进 pod-events；改法集零变更。

### OQ-2 · 设备轮触发判据采用哪种机械形态

**背景**：「用户可见」是语义判断，闸不能从 C++ diff 自行推导；但只凭启动卡一句自证又可能与冻结验收脱节，nano 等无新冻结件的包也不能只靠 acceptance 扫描。

- **(a) 只看 launch-card 申报**：实现最简单，但 acceptance 已声明可见结果时仍可能被错误写成 false。
- **(b) 只扫 acceptance 标记与 host 排除表**：语义锚更强，但无新冻结验收件的包缺申报入口，且无法要求实际 diff 与申报路径一致。
- **(c) 启动卡 + acceptance 标记 + 实际 diff 三路咬合（推荐，当前草案）**：任一路声明可见即触发，acceptance 标记覆盖 false 申辩，host 路径必须出现在合并 diff；非可见豁免必须给至少 12 字理由。成本是三种输入都要维护，但机械判据只有 pre-merge 一处。

**human 裁定（2026-10-28）**：选 **(c)**——设备门触发 = 启动卡 + acceptance 标记 + 实际 diff 三路咬合；改法集零变更。

### OQ-3 · 脚本净增 436 行，是否确认本批作为拆分纪律的明示例外

**背景**：`gov-change.md` 规定 scripts 净增超过约 150 行或 feature 级判定重写应单独成批。本稿实测 supervisor +193、G6 +6、pre-merge +237，合计 436 行，且确属判定扩展；但 P8 立项时 human 已明确两条并批，T3 还复用 T2 的申报解析器。

- **(a) 确认两条并批为例外（推荐，忠实于已给立项 ack）**：理由是共享 launch-card schema 与解析器需要同批一致，簇 A/B 的合并门语义也应一起评审；本 OQ 把隐含例外变成 human 明示，不自行绕过拆分纪律。
- **(b) 拆成簇 A 与簇 B 两批**：先落申报/supervisor，再落设备门；每批评审面缩小，但总量仍属 feature 级，且第二批依赖第一批的解析器与启动卡格式，需要增加一次跨批同步与两轮全量验证。

**human 裁定（2026-10-28，针对 R1 前 414 行草案）**：选 **(a)**——一批 + 明示例外；响应 R1、R2 findings 的两轮机械加固（v0.2/v0.3）后为 436 行，仅是同一方向的 fail-closed 与诊断加固，沿用该例外，不重开方向。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 时点：V1–V6 为 pre-commit 仓外装配与 DRAFT 自证；V7–V10 为 landing 后核验。所有预期数字来自 2026-10-28 实跑，原始摘要另存 `_scratch/verification-baseline.json`。改稿轮只复跑受影响项，FROZEN 前与 landing 后两轮必须全量。

| ID | 命令 / 检查 | 起草期实跑基线与落地预期 |
|---|---|---|
| V1 锚定装配 | `node .claude/scripts/assemble-edits.mjs delivery/review/261028-gov-review-p8-gate-semantics-cluster/_scratch/edits.json C:/tmp/p8-assembly-final` | 42/42 oldStr 恰命中一次、15 件目标文件、1 件新增 copy；失败即中止、不写半成品。最终基线副本 `C:/tmp/p8-assembly9`。 |
| V2 语法与 JSON | 对三件装配脚本 `node --check`；对 verify-gates 与种子配置 `JSON.parse` | 三脚本语法通过；两 JSON 可解析。落地后同跑。 |
| V3 三闸自检 | 装配副本依次跑 supervisor --self-check、pre-merge --self-check、supervisor-gate-check --self-check | **96/96、88/88、行为电池 10/10**；基线原闸为 74/74、66/66、10/10，新增判别项为正反成对。 |
| V4 反转实现变红 | `node delivery/review/261028-gov-review-p8-gate-semantics-cluster/_scratch/mutation-check.mjs` | **6/6**：去掉先例匹配、扩大轴白名单、接受真实配置为噪音、允许占位符豁免设备门、跳过设备门、跳过 KPLOG 段号校验均 exit 1，并分别命中指定失败 case。 |
| V4b evaluate 五案直调 | `node delivery/review/261028-gov-review-p8-gate-semantics-cluster/_scratch/evaluate-five-cases.mjs` | **5/5**：A default-none、B 占位符、C required-invalid、E probe-failed 均 `blocked=true`；D 干净非可见理由保持 `blocked=false`；E 的 probe 前缀恰一次。 |
| V5 历史种子反查 | 以三份真实 pod-events + 装配种子运行 `readApprovedPrecedents` | **count=3、advisories=[]**；changeId 分别为 v9、realtime-facts、agents-md 三包。 |
| V6 DRAFT 结构自证 | `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>`；随后 `--cite-check <同一路径>` | **preflight ok=true、anchorCount=15、findings=[]、advisories=[]**；**cite-check phase=draft、checkedCount=42、missing=[]、unverified=[]**。42 个改后块与 1 个新增文件块留 landing 相位核对。 |
| V7 landing 保真 | 主线程 apply 后逐文件 sha256 对照装配副本；再跑 V2–V4 | 三件脚本与两配置的 sha 应等于 `_scratch/verification-baseline.json.afterSha256`；其余文件逐件对账，任何差异回滚重查。 |
| V8 landing cite-check | `node .claude/scripts/landing-fidelity-check.mjs --cite-check <本 PROPOSAL>` | landing 相位：原文块对现盘改前来源、改后块对落地文件；缺失=0。debt 条目若已归位，允许 landing 门既有 `_archive` 兜底。 |
| V9 角色与账本 | `node .claude/scripts/lint-descriptions.mjs`、`node .claude/scripts/route-regression-check.mjs --self-check`、`node .claude/scripts/debt-render.mjs`、`node .claude/scripts/debt-doctor.mjs` | description 未改，路由信号零退化；两条 ARQ closed 后由渲染器归位，INDEX 只再生不手改。若环境既有红，先立 pristine 基线再比较，不拿既有红充本批失败。 |
| V10 合并闸与生成件 | 完成 landing-summary/verdict/retro 后跑本批 `pre-merge-escalation-gate.mjs <changeId>`；合并后 `changes-index.mjs --upsert <changeId>` | gov 档 PASS 才允许合并；本批 DRAFT 另在落盘后先 upsert 一次。deviceOutcome 只对 devloop 档生效，本批 gov 合并不伪造设备证据。 |

**负向 × 正向配对说明**：all-mock 有「命中先例降 low / 缺申报、缺先例、pattern 不符仍 high」；混合 security/core/critical 有「先例在册仍不折抵」；sdkconfig 有「空节可折抵 / 真实配置与删除行仍 critical」；设备门有三种证据形态正向，及缺申报、默认 none 或尖括号占位符在 assess+evaluate 两层均拦截、错 commit、缺 acceptance 覆盖、KPLOG 缺段号或时间窗、KPLOG032 越界、摘录 hash 错、probe 明细上抛且不重复前缀、human 批准不豁免等负向；板级 wiring 前缀另有正向夹具。

### DRAFT 自证留痕

- 装配：`C:/tmp/p8-assembly9`，42/42，15 件；最终 pre-merge sha256 = `fa09b79d1cc43a5f335210e11c7a35e15d78fbcda0bcc46d9363f2226d493046`。
- 自检：supervisor 96/96；pre-merge 88/88；G6 10/10；mutation 6/6；历史种子 3/3。
- evaluate 五案：5/5；A/B/C/E 阻断、D 放行，E 的 probe 前缀恰一次。
- preflight：`ok=true`、`anchorCount=15`、`findings=[]`、`advisories=[]`。
- cite-check（draft）：`ok=true`、`checkedCount=42`、`missing=[]`、`unverified=[]`；42 个改后块与 1 个新增文件块留 landing 相位。

## 6. 留根清单

1. **结果决策抽 host 可测缝**：保留 ARQ-261001 候选②，按具体 dev-loop 包决定，不在本批机械要求。
2. **host 不可编登记集是首批而非完备集**：新增设备专有编译单元时须同批判 config；未登记路径在申报时会 BLOCK，不能由闸猜测。
3. **generated-noise 暂无历史种子**：voice-rightcol 的处置是剥离噪音后 APPROVE，不是人批同型噪音先例，故不伪造种子；首个真实先例须 human 明确登记。
4. **在途包迁移**：不追溯重建历史证据；在途包若触新判据，合并前补机器申报，已有真机结果可整理成新证据格式，没有结果则按硬门停等。
5. **原始日志保留位置与期限**：本批只规定不得随当轮即删及包内最小摘录，仓外原日志的统一保留期限/目录另由运维纪律决定。
6. **先例扫描规模**：当前逐包读 pod-events 成本可接受；若包数增长导致 supervisor 明显变慢，再做只读索引投影，不能把投影当第二权威。
7. **OQ-3 若改选拆批**：T5 两条目与验证集按簇拆分，T3 对 T2 申报解析器的依赖顺序写入后一批立项材料，不在落地期临时拆。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-28 - architect-reviewer

**四查完成**：意图保真 / 纪律合规 / 撞车复核 / 牵连面逐项过，1 项 BLOCKING（F1，属机械收紧、不改已批方向）、1 项 NIT。R2 聚焦 F1 修订到位性 + 修订是否引入新矛盾。

verdict: BLOCK(F1)

**F1（BLOCKING）· 设备门 fail-closed 被 T1a 模板默认占位符击穿**
- 证据：T1a 改后模板默认行（PROPOSAL :90）`"deviceOutcome":{"userVisible":false,...,"reason":"<无用户可见结果面时填理由；无设备面可写 none>"}`；T3b 判据（PROPOSAL :700-702）`if (touched.length > 0 && String(declared?.reason || '').trim().length < 12)` 只查长度地板。该占位符串实测 22+ 字 ≥ 12 字地板 → launch-card 照抄模板未填、而 diff 恰触已登记 host 不可编路径（如 `firmware/main/display/lcd_display.cc`）的包，会以「非用户可见 + 占位符理由」静默豁免真机证据，全程零 BLOCK。这与 T1b 自述「前置升级闸对触 host 不可编路径而缺申报的包 fail-closed」（PROPOSAL :103）及本批目标效果（host 全绿 ≠ 现象对 → 合并前必须真机目击）正面冲突：T3 硬门在「最懒路径」上不设防。
- 建议修法方向（不涉方向变更，二选一）：① 模板默认 reason 改为短于地板的 `none`——未填 + touched>0 → `device-outcome-nonvisible-reason-missing` 自然 BLOCK（fail-closed 归位），无设备面包 touched==0 本就不查 reason、语义不变，T1b 建议同步补一句「未填（none）视同未申报」；② `assessDeviceOutcome` 把占位符形态（`startsWith('<')` 或等于模板原文）判为未填。改后 T1a/T2h/T3g 夹具与 edits.json 同步重跑装配。

**F2（NIT）· probe 失败分支丢详细原因**：T3c probe 失败只 push 通用句「设备结果面探测失败，升级人工核」，`collectDeviceOutcome` catch 内已产出的 `'device-outcome-probe-fail-closed：' + error.message` 明细未并入 gate reasons，排障少一层。可在同分支并写 `ctx.deviceOutcome.reasons`。

**F3（NIT）· KPLOG 段号正则宽于轮转面**：`/^KPLOG[0-9]{3}$/`（PROPOSAL :733）接受 KPLOG032..999；skill 侧轮转面为 `KPLOG000..031`（`.claude/skills/kidpal-device-logs/SKILL.md:25`）。摘录须包内在册 + SHA-256 对账，伪造面未扩大，仅口径可顺手收紧为 `/^KPLOG(?:0[0-2][0-9]|03[01])$/` 或维持现状并留根。

**判档与升级判定（只升不降）**：维持 **Track B 主路径、riskTier=high、profile G2+G3**——轻路径三判据均不满足（diff 触 hooks/scripts/agents 命中排除集③、行为语义变更非措辞②、排除集直触①）；Track A 段不触发：human 2026-10-28 两拍 + OQ 三拍已收敛方向，本判未发现方向级缺口。注记：ARQ-261001 条目自declared「full-adversarial + human（Track A），不得回标 Track B」，已被 human 立项 ack「批形态 = 2 条一批、Track B 主路径、261001 批内独立 T 全强度评审」有意识覆写，覆写留痕在 `delivery/workstreams/gov-debt-reduction/task_plan.md` :124/:126——立项 ack 是该判断点的合法权威，不构成升级项。脚本拆分纪律：414 行明示例外 = human OQ-3 拍定，净增实测 193+6+215=414 属实。

**亲核留痕（全部实测，非转述采信）**：
1. 装配：`node .claude/scripts/assemble-edits.mjs <edits.json> C:/tmp/p8-review-r1` 实跑 ok=true、42/42 恰命中、15 件（14 改 + 1 copy）；pre-merge 装配件 sha256 = `a35fb5c8f75d9078781d0dd9b4d93d22bb49c199c9b285eba23e851516ff472f`，与 §5 DRAFT 留痕逐字一致。
2. V2–V6 复跑全绿：三脚本 `node --check` + 两 JSON parse 通过；自检 supervisor **96/96**、pre-merge **81/81**、G6 **10/10**（基线 74/66/10，新增判别项 +22/+15 与 T2h/T3g 逐条对得上）；mutation **5/5** 逐项 exit 1 且分别命中指定判别 case；种子反查 **count=3、advisories=[]**；preflight ok=true anchorCount=15 findings=[]；cite-check phase=draft checkedCount=42 missing=[] unverified=[]。
3. 事实断言抽查全对上：三条种子 human-decision 事件（e02f05e3…/9d4c1f68…/1c6076b2…）逐条在 `delivery/taskpackages/<id>/pod-events.jsonl` 实测在册，actor=human、decision=approve-escalated-merge；噪音 commit `24982715` 实测删 5 行 `Image Effects Configuration` 空节头；`CRITICAL_PATH_PATTERNS` 含 sdkconfig（supervisor-verdict-gate.mjs :91-95）→ `critical-path:<p>` reason（:109）与 `axisForReason` 前缀映射（critical-path:firmware/sdkconfig / security-axis- / core.*: / all-mock:）逐一吻合；`acceptanceTestsFile` 字段名与现盘 taskpackage schema 一致（acceptance-coverage-check.mjs:142 同消费）；host 不可编注记实测于 `firmware/main/test/host_app/main/CMakeLists.txt` :270/:278/:297。
4. 撞车复核：`delivery/review/INDEX.md` 现仅本批 draft，P5–P7.5 全 landed、无在途落点重叠；`verify.mjs` 只消费 `cfg.gates` 数组（:243-244），`deviceOutcomeGate` 兄弟键零双头、_doc 口径相符；`pod-events-append.mjs` payload 为开放对象，approvedVia/gatePrecedents 新字段写入兼容、eventType 闭集已含 human-decision；scope-map universal.denyCreate 仅拦 contracts/** 与 delivery/changes/**，`.claude/supervisor-precedents.json` 新建不受阻。
5. §3 sweep 断言独立复现：新 pattern 集（deviceOutcome/gatePrecedents/supervisor-precedents/hostUncompilablePath/device-round-evidence/device-outcome:/host-uncompilable-path:/验证形态申报（JSON））对 `.claude/**`、`delivery/taskpackages/**`、`docs/debt/**` 复测零命中。
6. 结构字段：landing-manifest 围栏块（:12-28）与 packet-id 行（:6）在场；manifest 15 件与 T1–T5 目标文件清单逐一吻合，G1/契约/spec/实现面零 diff 属实。

### R2 - 2026-10-28 - architect-reviewer（聚焦前轮 findings 修订到位性 + 新矛盾扫描）

**R1 findings 修订到位性**：F1 文面两臂（T1a 模板 `reason:"none"` + T3b `length<12 || startsWith('<') || ==='none'` + T1b「视同未申报」句）已落、F2 明细上抛已落（实测命中 detail）、F3 段号收紧 `/^KPLOG(?:0[0-2][0-9]|03[01])$/` + KPLOG032 负例已落——**但 F1 只闭合到 assess 层，合并闸消费层仍放行，实测复验击穿**（下 F1'）。F2/F3 判定到位。

verdict: BLOCK(F1)

**F1'（BLOCKING，F1 未闭合残段）· assess 层判 ok=false，evaluate 消费位仍放行**
- 实测证据（v0.2 装配副本 `C:/tmp/p8-review-full2`，`evaluate` 直调，off-main 等其余闸全绿基线下）：`deviceOutcome = { probed:true, required:false, ok:false, reasons:['device-outcome-nonvisible-reason-missing：…'] }` → **blocked = false, reasons = []**；占位符同形同结果。T3c 消费判据 `else if (ctx.deviceOutcome.required && !ctx.deviceOutcome.ok)`（PROPOSAL :818-819）把 `required=false ∧ ok=false` 的 reasons 整体丢弃——T3b `!required` 分支 return 写死 `required: false`（PROPOSAL :705）而 ok 由 reasons 推出，两字段自此脱钩。即：launch-card 照抄模板（reason=`none`）+ diff 触已登记 host 不可编路径的最懒路径，合并闸依旧零拦——R1-F1 的洞在闸层原样存活。
- 判别缺位佐证：两个新负例（PROPOSAL :931-934）只断言 assess 层 `!ok`，未断言 evaluate 层 `blocked=true`；故 85/85 电池与 mutation 6/6（含新 mutation `allow-placeholder-device-exemption`，其判别 case 也是 assess 层 :934）对本洞全绿——正违反「夹具必须判别反转实现」：反转被拦截的层（evaluate）没有夹具。
- 建议修法方向（最小改法，二选一）：① T3b `!required` 分支 return 改 `required: reasons.length > 0`——default-none/占位符路径 required 翻 true → T3c 现判据即拦；干净非设备包 reasons 空 → required=false ok=true 放行语义不变（实测 D 案对照）；② T3c 消费改 `else if (ctx.deviceOutcome.ok === false)` 一律 push。同时义务：把 default-none / placeholder 两负例升到 evaluate 层断言 `blocked=true`，mutation 判别 case 同步改打闸层 case，防 R3 绿后同层回退。

**F4（NIT，R2 新发现）· probe 失败 reason 双前缀**：`collectDeviceOutcome` catch 的 reason 已带 `device-outcome-probe-fail-closed：` 前缀（PROPOSAL :791），T3c 再包一层同前缀（:820）→ 实测输出 `device-outcome-probe-fail-closed：device-outcome-probe-fail-closed：fixture boom——升级人工核`。顺手去重（T3c 侧判 hasPrefix 再包，或 collect 侧裸 message）。

**新矛盾扫描（R2 范围内零其它发现）**：T1b「未填（none）或保留尖括号占位符均视同未申报」与 T3b 三条件判据一致（待 F1' 闸层闭合后文实完全对齐）；三路咬合判据（OQ-2c）未被修订触动；OQ-3 诚实注记（:1313「human 拍的是 R1 前 414 行草案…不重开方向」）核实无伪称重新拍板；§5 数字（85/85、6/6、426 = 193+6+227、新 sha `14a76f8e…`）与实测逐一吻合；supervisor / G6 / 种子件 sha 与 R1 相同，V5 三种子结论沿用。

**亲核留痕（全部实测）**：
1. 重装配 `C:/tmp/p8-review-r2`：ok=true、42/42、15 件；pre-merge 装配件 sha256 = `14a76f8e2af2895bd2324e3ce0120f59746212857b6f128734e847e9096e0594`，与 §5 DRAFT 留痕一致；行数 1176-949 = +227，合计 426 属实。
2. 自检复跑（全量 overlay 副本）：supervisor 96/96、pre-merge 85/85、G6 Battery 10/10；mutation 6/6（`allow-placeholder-device-exemption` 判别 case = assess 层 `device-placeholder-reason-touched-host-fail-closed`）。
3. **evaluate 层五案直调实测**（本 R2 关键动作）：A `required=false,ok=false`（default-none 形）→ **不拦**；B 占位符同形 → **不拦**；C `required=true,ok=false` → 拦 ✓；D `required=false,ok=true` → 放行 ✓；E `probed=false` → 拦 ✓ 且 detail 上抛（但见 F4 双前缀）。
4. preflight ok=true anchorCount=15；cite-check phase=draft checkedCount=42 missing=[] unverified=[]。
5. F3 实证：KPLOG032 负例在 85/85 电池内绿（`device-kplog-032-out-of-rotation-block`，PROPOSAL :918-920）；regex 逐字核对与 skill 轮转面 KPLOG000..031 一致。

**判档与升级判定（只升不降）**：维持 Track B 主路径、riskTier=high、G2+G3，无升降。F1' 是 R1-F1 的未闭合残段（闸消费层），不构成方向级缺口，不升级 human；已命中熔断观察线——**本批第 2 轮未 GREEN（R2/3），R3 若仍不 GREEN 触发熔断三选一**。R3 焦点唯一：F1' 闸层闭合 + 判别夹具升层，勿扩面。

### R3 - 2026-10-28 - architect-reviewer（聚焦 F1' 闸层闭合与 F4 去重的修订到位性）

**R2 findings 修订到位性**：F1' 选案①（T3b `!required` 分支 return 改 `required: reasons.length > 0`，PROPOSAL :704）落地到位；F4 前缀判存去重（:820 `detail.startsWith(prefix) ? detail : prefix + detail`）落地到位。两负例升 evaluate 层且以真实 assess 输出直喂 evaluate（:941-944），mutation 判别 case 改打闸层（`device-placeholder-reason-evaluate-block`）——R2 要求的三项义务全数兑现。

verdict: GREEN

**亲核留痕（全部实测，不采信回执）**：
1. 重装配 `C:/tmp/p8-review-r3`：ok=true、42/42、15 件；pre-merge 装配件 sha256 = `fa09b79d1cc43a5f335210e11c7a35e15d78fbcda0bcc46d9363f2226d493046` 与 §5 留痕一致；行数 1186-949 = +237，合计 193+6+237=436 属实。
2. 自检复跑：supervisor 96/96、pre-merge 88/88、G6 Battery 10/10；mutation 6/6，其中 `allow-placeholder-device-exemption` 判别 case = 闸层 `device-placeholder-reason-evaluate-block`（mutation-check.mjs :37 expect 逐字核对），exit 1 变红命中属实。
3. **evaluate 层复验（R2 同口径 + 修正）**：R2 的 A/B 两案手构形 `{required:false, ok:false}` 在 v0.3 已是 assess 层不可能产出的陈旧中间形——结构性核验：`assessDeviceOutcome` 三条 return 路径（装配件 :429 `!required` 分支 required⟺reasons.length>0 与 ok 互补、:516 catch required=true、:466-467 required 分支 required=true 字面）全部满足不变量 **probed ∧ ok=false ⟹ required=true**，故 T3c 现判据 `required && !ok` 对任何 assess 失败输出必拦。真实输出形直调：A2 default-none 形（required=true, ok=false）→ blocked=true ✓；B2 占位符形 → blocked=true ✓；C 案拦 ✓；D 案（干净非设备包 required=false ok=true）放行 ✓；E 案拦 ✓ 且 probe 前缀恰一次（`device-outcome-probe-fail-closed：fixture boom——升级人工核`，双前缀已消）。
4. F1' 判定闭合：闸层洞经「结构性不变量 + 88/88 电池内两枚 evaluate 层负例（真实 assess 输出直喂）+ mutation 闸层判别反转变红」三重实证关闭。
5. supervisor / G6 / verify-gates 装配件 sha 与 v0.1 相同（`eea9652e…` / `1f199556…` / `ddfa4afb…`），V5 历史种子复跑 count=3、advisories=[] 结论沿用。
6. preflight ok=true anchorCount=15 零 findings；cite-check phase=draft checkedCount=42 missing=[] unverified=[]。
7. 新矛盾扫描：T1b「未填（none）或保留尖括号占位符均视同未申报」与 assess/evaluate 两层行为经 F1' 闭合后完全对齐（assess 判 reason-missing → required 翻 true → 闸拦）；三路咬合（OQ-2c）未被触动；§5 数字（88/88、6/6、436、fa09b79d）逐一吻合；OQ-3 诚实注记实质保持（不伪称重新拍板）。

**F5（NIT，R3 唯一新发现，不阻 GREEN）· OQ-3 注记轮次措辞**：:1323「R1/R3 两轮机械加固」——两次加固实际是响应 R1（出 v0.2）与响应 R2（出 v0.3），写「R1/R3」易误读为第 1、3 轮。建议顺手改「响应 R1、R2 findings 的两轮机械加固（v0.2/v0.3）」，可随落地顺手改或留根不改。

**判档与升级判定（只升不降）**：维持 **Track B 主路径、riskTier=high、profile G2+G3**，无升降；无方向级缺口，不升级 human。

**熔断计数**：本批第 3 轮 **GREEN**——评审循环收敛（R1 BLOCK(1) → R2 BLOCK(1，闸层残段) → R3 GREEN），未触发熔断。主线程可凭「立项 ack + 本 GREEN」标 FROZEN，进入 packet-bound landing；F5 为 NIT 可落地顺手改或留根。
