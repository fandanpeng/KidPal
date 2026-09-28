# architect-landing-summary · 260928-gov-review-dev-loop-preassess-manifest-scope

> **状态：landed · 2026-09-28 · 主线程 packet-bound 落地（manifest 17 文件）** · FROZEN v1.0（PROPOSAL，commit `d475458`）· R3 GREEN（§7）
> 本件由主线程落地后撰写，交 landing-verifier 独立核验（landing-verdict.md 为其产物）。
> **packet-binding**：packet-id = FROZEN PROPOSAL §2 改法集（T1–T12）+ §5 验证集；被批 review = `260928-gov-review-dev-loop-preassess-manifest-scope`（human 立项 ack + OQ-1=(a)/OQ-2=(a) + architect R3 GREEN 凭据见 PROPOSAL Status 行）。

```landing-manifest
AGENTS.md
.claude/agents/architect.md
.claude/agents/builder.md
.claude/agents/fixer.md
.claude/agents/integrator.md
.claude/agents/planner.md
.claude/commands/build-feature.md
.claude/hooks/verify.mjs
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/task-intake-and-slicing.md
.claude/scripts/acceptance-run-check.mjs
.claude/scripts/change-manifest-check.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/scripts/unit-run-check.mjs
.claude/verify-gates.json
```

## manifest 落地对账（17/17）

| # | 文件 | T | 处置摘要 |
|---|---|---|---|
| 1 | `.claude/agents/planner.md` | T1/T12 | 新增 Step 1g「ARCH-ADVISORY 技术预评档」（1f 之后、含三件产出 + 比例化简版 + 1g/10b 边界）；:65/:70「首次全量复验」→「final 复验（scope 圈定）」；Step 11「全量验证」→「范围圈定终验（final 相位按影响面 scope 圈定跑……全量回归 = 人工触发位）」 |
| 2 | `.claude/agents/architect.md` | T2 | description 追加 1g 档路由短语；新增「dev loop 前置技术评估（1g 档）」职责段（三件产出 + 扩单唯一通道 + 比例化简版 + advisory 效力声明）；tools/name/model/write scope 零动 |
| 3 | `.claude/agents/builder.md` | T5 | Boundaries 新增文件级边界条款（changeManifest 越界停手 → planner → architect 扩单；禁先改后补）；首条边界 `apps/sX-*` 旧 token 改本仓实况（scope-map 口径）；Rationalizations 加一行 |
| 4 | `.claude/agents/fixer.md` | T5 | 同款边界条款 + 旧 token 修正 + Rationalizations 行 |
| 5 | `.claude/agents/integrator.md` | T8 | description/标题/Role 三处改「影响面圈定终验」；Step 4 重写为两步走（范围圈定三分法 + `--scope-file` final 命令 + test-scope 路径入 final fact 必含行）；Step 5「全量 verify/e2e 绿后」→「final 复验（scope 圈定）/e2e 绿后」；Step 11b 改 advisory 记账 + 本仓图索引锚（C-AI-KidPal / Pod 按路径派生）+ N/A 路径收窄；消解复跑清单保留双包冻结件为 scope floor 注；Rationalizations / Red Flags / Verification 三处换新语义 |
| 6 | `.claude/references/dev-loop.md` | T3/T4/T12 | :60 图「全量 e2e」→「范围圈定终验（scope 圈定）」；:128 标准字段加 `changeManifest`；:129 plan.md 条目扩注 F8 唯一例外（技术方案节补写 + UPSTREAM_DRIFT 口径）；附加字段节新增 `changeManifest[]` 消费细则（产出/消费/扩单通道）；:132「终审全量闸」→「终审圈定范围闸」；:174 s2 行「全量 verify/e2e 绿后」→「final 复验（scope 圈定）/e2e 绿后」 |
| 7 | `.claude/references/task-intake-and-slicing.md` | T3 | :266 判据句后追加「例外」条款（技术方案节承载 architect 前置评估产出、planner 补写留痕） |
| 8 | `.claude/references/dev-loop-operating-system.md` | T12 | 角色表 integrator 行改「范围圈定终验（scope 圈定）」；:38 C1 注改 scope-file 圈定语义；:55 图「终审 full」改 scope-file 圈定 |
| 9 | `.claude/commands/build-feature.md` | T12 | Step 9「全量 e2e」+「全量 verify/e2e」两实例（同行）改新语义 |
| 10 | `AGENTS.md` | T11 | Hard Rules dev-loop 链描述加「planner Pod 派 architect（前置技术预评）」一位 |
| 11 | `.claude/verify-gates.json` | T6/T10 | 新闸 `change-manifest`（kind=governance、timing=scoped，排 taskpackage-required 后，带 _doc）；`_doc` timing 段改 scope-file 圈定 + 人工触发位语义；quick-check 条目改 blast radius 圈定 + fail-closed 举手 |
| 12 | `.claude/scripts/change-manifest-check.mjs` | T6 | 新建：glob 对账闸（对账面 = firmware/** + server/**；glob→matcher 纯函数 + porcelain 解析 + cutoff 谓词 + `--self-check` 16 断言） |
| 13 | `.claude/scripts/taskpackage-required-check.mjs` | T7 | `collectManifestErrors` 完备性齿（在场/非空/合法 glob 形态；MANIFEST_CUTOFF carve-out 与 T6 同值共享）+ self-check 6 断言（合计 18/18 绿） |
| 14 | `.claude/scripts/unit-run-check.mjs` | T9 | `--scope-file` 支持：final 缺 scope-file 红闸举手；scope 引用盘上全不存在红；交集空诚实绿跳过；scoped/无 phase 直跑行为零变 |
| 15 | `.claude/scripts/acceptance-run-check.mjs` | T9 | 同款 scope-file 支持（与 unit 同构） |
| 16 | `.claude/hooks/verify.mjs` | T9 | final 相位缺 `--scope-file` → exit 2 红闸举手（gate-child 层）；`--scope-file` 透传 timing=full 闸 cmd；bearing/legacy 双路径透传；self-check 新增 2 断言（合计 10/10 绿） |
| 17 | `.claude/scripts/merge-driver.mjs` | T10/T12 | `DETECT_CHANGES_PROJECT` 锚 `C-AI-AInativecompany` → `C-AI-KidPal`（env 可覆盖，与 integrator Step 4 同口径）；新增 `buildQuickCheckScope`（git diff 底面 + detect_changes enrich；图不可用/圈定失败 fail-closed blocked）；`runQuickCheck` 调用点对 acceptance 门追加 `--scope-file`（F4 oracle 不动，实测 `--self-check-quick-checks` 绿）；:408 注释「全量门套」→「终审圈定范围闸」 |

## 残留清扫核（§5 item 5 实测 2026-09-28）

- 「必须全量验证」/「全量 e2e」/「全量 bearing」/「全量 monorepo」/「全量端到端」/「全量 verify」/「全量闸门」/「全量闸」/「全量门套」/「全量复验」/「全量验证」：`.claude` 面内 **0 残留**。
- 「只跑本包档不算完成」：命中 2 处，均为**冲突消解复跑清单语境**（integrator.md:81 带 floor 注；`pod-coordination.md:151` 为 §10a runbook 镜像句）——属 §5 断言的预期保留类（floor 语境），非 final 相位铁律残留。`pod-coordination.md:151` 不在本批 manifest（评审期行清单未列、判官 landing 注未点名），其 floor 注补齐义务**留根交下批**（与 PROPOSAL §6-7 同型的边缘残留）。
- 裸「全量」残余扫描：余下命中均为无关语义（plan.md 全量义务 / 全量 body 扫描回退 / INDEX 全量重建 / 全量读 / 全量禁写），不在测试套件语义族内。
- `grep -n "全量功能套件" .claude/verify-gates.json` → 1 命中 = 新 timing 语义句。

## 机检自检实测（2026-09-28，本机实跑）

- `node .claude/scripts/change-manifest-check.mjs --self-check` → 16/16 绿（含 glob 转义顺序 bug 自抓自修：先挖通配 token 再转义字面量）。
- `node .claude/scripts/taskpackage-required-check.mjs --self-check` → 18/18 绿（新增 6 断言）。
- `node .claude/hooks/verify.mjs --self-check` → 10/10 绿（新增 final-missing-scope-file 2 断言；实 spawn 断言 exit 2 + stderr 点名）。
- `node .claude/scripts/merge-driver.mjs --self-check-quick-checks` → F4 oracle 绿 + scope 映射 5 断言绿。
- `node .claude/scripts/unit-run-check.mjs --self-check` / `acceptance-run-check.mjs --self-check` → exit 0。
- `verify-gates.json` JSON.parse 校验通过。

## 生效切点与过渡

- changeManifest 双闸（taskpackage-required 完备性齿 + change-manifest 对账）以 `MANIFEST_CUTOFF = 2026-09-28`（gitFirstCommitDate 谓词）划界：cut 点后新建包必带 `changeManifest[]`（planner Step 1g 档产出）；在途存量包两闸同豁免。
- final 相位 scope-file 红闸即时生效（verify.mjs gate-child 层 + 两 run-check 脚本双保险）；首跑 test-scope 生成流程见 integrator.md Step 4。
- merge-driver quick-check scope 圈定即时生效；图不可用 → blocked 举手（env `MERGE_DRIVER_DETECT_CHANGES_PROJECT` 可覆盖锚）。

## 遗留与留根指针

- PROPOSAL §6 留根 1–7 照旧生效；新增第 8 项（landing 期发现）：`pod-coordination.md:151` 消解 runbook 镜像句的 floor 注补齐（不在本批 manifest，下批搭车件）。
- 判官 R3 landing 注两项：dev-loop-operating-system.md:29 角色表「全量 e2e」已随 T12 清（本表 #8）；loop-metrics.mjs:74「全量运行」注释判词形族外、未动（语义键于 phase 过滤）。

## landing-verifier R1 复核退回修复（2026-09-28，landing-verdict FAIL-F1/F2）

- **FAIL-F1（阻断）scope-file 透传链参数形态断裂**：caller（verify.mjs / merge-driver）拼等号形态 `--scope-file=path`，callee 两 run-check 只认空格形态 → 透传静默落空（该圈不圈、该红不红，final 相位静默退化为全仓全量）。修复：两 callee `parseScopeFileArg` 兼容双形态（`--scope-file=` 前缀 ∨ 空格参数）；verify.mjs 对 timing=full 闸追加 `--phase=final` 透传（纵深：callee 自身执行 final 缺 scope-file 红闸判据）；两 callee self-check 各加 3 断言（双形态解析 + 缺参 null）。**三态实证（本机实跑）**：等号形态+坏清单=红(1)、空格形态+坏清单=红(1)、final 缺参=红(1)、等号形态+有效清单=绿(0) 且 phase=executor-skipped（scope 被消费、交集空诚实绿）、acceptance 同构=绿(0)。
- **FAIL-F2（随批修）T6 对账面 docs-sync 面子句未实现而 _doc 失实**：选「回改 _doc 去子句并留痕」——verify-gates.json 新闸 `_doc` 改为「对账面 = firmware/** + server/**（实现面；簿记/治理面豁免）」+ 留痕注（FROZEN 条文的 docs-sync 面子句未实现为代码；manifest 显式列入的 docs 路径天然在对账内）。未补实现（仓内无「声明的 docs-sync 面」独立字段，实现会是发明新语义——留 PROPOSAL §6 观察项）。
- 复跑自检全绿：change-manifest 16/16、taskpackage-required 18/18、verify.mjs 10/10、merge-driver F4+scope 5 断言、unit/acceptance self-check exit 0。
