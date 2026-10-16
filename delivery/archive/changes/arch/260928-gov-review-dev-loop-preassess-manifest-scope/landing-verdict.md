# landing-verdict · 260928-gov-review-dev-loop-preassess-manifest-scope

> 判定者：landing-verifier（独立核验，只读 codebase）· 2026-09-28
> 核验基准：FROZEN PROPOSAL v1.0（commit `d475458`，§2 改法集 T1–T12 + landing-manifest 17 文件）+ 落地 commit `9d509b0` + landing 修复 commit `2a42e02`（FAIL-F1/F2 闭环）
> **演进记录**：首验 2026-09-28 **FAIL**（缺陷 1 阻断：scope-file 透传链 caller/callee 参数形态断裂，范围圈定在 verify.mjs final 相位与 merge quick-check 两条集成路径上静默失效；缺陷 2 随批修：T6 对账面 docs-sync 面子句未实现且 verify-gates.json `_doc` 失实）→ 主线程按修复指令定向修两件（commit `2a42e02`）→ 复验 2026-09-28 **PASS**。演进全程见「复验凭据」与「FAIL→PASS 演进记录」两节，原 FAIL 缺陷清单保留在后。

## verdict：**PASS**

首验三道全过面（结构门 + 15/17 文件语义忠实 + 破坏性自检全绿）继续有效；两条缺陷经 commit `2a42e02` 定向修复，复验全数闭环（缺陷 1 差异实证三态五测独立复跑通过、缺陷 2 `_doc` 回改如实带留痕）。**放行进入 Track B 第 5 步后段；merge 仍归 human manual-merge 门。**

## 一、结构门（机器 · 复验凭据）

```
node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260928-gov-review-dev-loop-preassess-manifest-scope d475458
→ ok: true, exit 0（复验 2026-09-28，verifier 亲跑；baseRef = landing 前 ref d475458，
  覆盖窗口 = 9d509b0 landing + 2a42e02 修复全量改动）
  frozen: true · packetBinding: true
  unauthorized: [] · claimedNotLanded: [] · plannedNotLanded: [] · gitDegraded: false
```

复验期曾出现一次 exit 1——经查为 verifier 自身 baseRef 姿势误用（`--base-ref` 旗标形式不被脚本识别，脚本按位置参数语义丢弃后退回默认 HEAD~1 只审 fix 增量，误报 13 文件 claimed-not-landed）；改用位置参数传 baseRef 后 ok:true。该误报与 landing 无关，记入本节防后续复验者重蹈。

## 二、缺陷 1（阻断）修复复验——闭环确认

**修复内容人读 diff（commit `2a42e02`）**：① 两 callee `parseScopeFileArg` 先识别 `--scope-file=` 前缀再回落空格形态（unit-run-check.mjs / acceptance-run-check.mjs 逐字同构）；② verify.mjs 对 timing=full 闸透传追加 `--phase=${phase}`（纵深：callee 自身 final 缺 scope-file 红闸判据在集成路径上生效）；③ 两 callee self-check 各加 3 接缝断言（`scope-arg-equals-form-parsed` / `scope-arg-space-form-parsed` / `scope-arg-absent-null`，实查在位）。

**三态五测独立复跑（verifier 亲跑，本机实跑，非采信主线程自报）**：

| # | 场景 | 预期 | 实测 |
|---|---|---|---|
| 1 | unit，`--phase=final --scope-file=`等号 + 坏清单 | 红（引用全不存在） | 红 ✓ |
| 2 | unit，`--phase=final --scope-file`空格 + 坏清单 | 红 | 红 ✓ |
| 3 | unit，`--phase=final` 缺参 | 红（缺 --scope-file） | 红 ✓ |
| 4 | unit，等号 + 有效清单（AGENTS.md，盘上存在） | 绿且 executor-skipped（scope 被消费） | 绿 exit 0，`phase: executor-skipped`，`scopeFile` 回显 ✓ |
| 5 | acceptance，同 #4 | 同 | 绿 exit 0，同构 ✓ |

测试 4/5 的 `scopeFile` 回显 + executor-skipped 状态证明等号形态（即 verify.mjs / merge-driver 的 caller 形态）现在被 callee 实际读入并求交——首验缺陷的「静默落空」已闭合。测试 3 证明 `--phase` 纵深透传后，集成路径上缺 scope-file 会真正触发 callee 红闸（首验时该判据因 verify.mjs 不传 --phase 而不可达）。

**merge-driver 侧注记（非缺陷）**：`runQuickCheck` 仍只追加等号形态 scope-file、不追加 `--phase`——callee 现已解析等号形态，scope 必被消费；`buildQuickCheckScope` 失败时提前 blocked，不会无 scope 直跑。唯一边角 = `no-changes` 时 scopeFile 为 null → acceptance 全仓跑，该态下实现面零改动、属无害保守默认，不构成缺陷。

## 三、缺陷 2（随批修）修复复验——闭环确认

主线程选「回改 `_doc` 去子句并留痕」：verify-gates.json change-manifest 闸 `_doc` 改为「对账面 = firmware/** + server/**（实现面；簿记/治理面豁免——FROZEN 条文的 docs-sync 面子句未实现为代码，landing-verdict FAIL-F2 回改本 _doc 留痕；manifest 显式列入的 docs 路径天然在对账内）」。选择合理：仓内无「声明的 docs-sync 面」独立字段，补实现等于发明新语义；canonical 文本与实现的自相矛盾已消除、留痕诚实。修复记录已 append 进 architect-landing-summary「landing-verifier R1 复核退回修复」节，内容与磁盘实况逐条一致。

## 四、自检机检复跑（复验 · verifier 亲跑）

```
unit-run-check.mjs --self-check         → ok:true（含新增 3 接缝断言）
acceptance-run-check.mjs --self-check   → ok:true（同构 3 断言）
verify.mjs --self-check                 → 10/10 绿
change-manifest-check.mjs --self-check  → 16/16，failed 0
taskpackage-required-check.mjs --self-check → 18/18 绿
merge-driver.mjs --self-check-quick-checks → F4 oracle 绿 + scope-map 5 断言绿
```

## 五、修复 commit 范围与夹带检查

`git diff 9d509b0 2a42e02 --name-only` = 恰 5 文件：`.claude/hooks/verify.mjs`、`.claude/scripts/unit-run-check.mjs`、`.claude/scripts/acceptance-run-check.mjs`、`.claude/verify-gates.json`（均 manifest 内）+ `architect-landing-summary.md`（review 证据目录制品，合法）——修复严格 confined 于 FAIL-F1/F2 指令面，零夹带。工作树 canonical 面（`.claude/**` + `AGENTS.md` + `docs/contracts/**`）相对 HEAD 零 diff。

## 六、首验已有效的核对结论（复验继续有效，不重复展开）

- **逐文件核对表 17/17**：首验 15 文件 ✅ + 缺陷涉及 2 文件（unit-run-check / acceptance-run-check 的 T9 主体判据、verify-gates.json 的 T6/T10 落位）经复验补齐为 ✅。首验表逐行结论（含判官 R3 两条 landing 注处置核对：dev-loop-operating-system.md:29 已清、loop-metrics.mjs:74 未动与 summary 一致）继续有效。
- **破坏性自检**：`docs/contracts/**`、agents/landing-verifier.md、pre-merge-escalation-gate.mjs、supervisor 相关、PROPOSAL §7 判官区、loop-metrics.mjs 零命中；landing = 17 manifest + summary 制品，零夹带。
- **残留词表（§5 item 5）**：12 形词表 .claude 面内 0 残留；「只跑本包档不算完成」恰 2 命中（integrator.md:81 floor 注预期保留 + pod-coordination.md:151 summary 申报的留根第 8 项）。
- **packet-binding**：summary packet-id == 被批 review id，landing 范围 ⊆ 被批 packet（结构门 + 人读双证）。
- **§5 其余验证集**：PROPOSAL 形态门（preflight 17 token）、INDEX 登记、quick-check 行为断言——首验/复验机检输出均在案。

## FAIL→PASS 演进记录（原 FAIL 凭据，保留备查）

**首验（2026-09-28）原缺陷清单（ErrorEnvelope 式）**：

| # | failingFile | expected | actual | category |
|---|---|---|---|---|
| 1 | `.claude/hooks/verify.mjs`（caller）+ `.claude/scripts/unit-run-check.mjs` / `acceptance-run-check.mjs`（callee）+ `.claude/scripts/merge-driver.mjs`（同型 caller） | FROZEN T9①②/T10①：verify.mjs / merge-driver 传入的 scope-file 必须被 run-check 实际消费（求交后跑子集） | caller 拼等号形态 `--scope-file=path`，callee `argv.indexOf('--scope-file')` 严格等值只认空格形态——透传静默落空；差异实证：等号形态被报「final 相位缺 --scope-file」。集成路径（verify.mjs 不向 run-check 传 --phase）既不红也不求交 → final 静默退化为全仓全量跑且以 scoped 面目记账；merge quick-check 同型失效（OQ-1=(a) 落空） | semanticMismatch / 实现缺陷（静默失效 + fail-closed 反转） |
| 2 | `.claude/scripts/change-manifest-check.mjs` + `.claude/verify-gates.json` | FROZEN T6（判官 R1-B2 钉死）：对账面 = firmware/** + server/** ∪ TaskPackage 显式声明的 docs-sync 面 | 实现仅 `firmware|server` 正则，docs-sync 面零消费逻辑；`_doc` 却声称包含该面——落地文本与实现自相矛盾 | 漏改（FROZEN 条文子句）+ 落地 _doc 失实 |

**根因假设（首验记录）**：缺陷 1 = caller/callee 两侧分别实现、未做端到端联通实跑（现有自检均为两侧各自单测，测不到接缝）；缺陷 2 = 落地期按最简正则实现对账面、_doc 按条文照抄未回核。

**闭环动作（主线程，commit `2a42e02`）**：缺陷 1 = callee 双形态解析 + verify.mjs `--phase` 纵深透传 + 6 接缝自检断言；缺陷 2 = `_doc` 回改去子句并留痕 + summary 修复记录 append。

## 结论

结构门（baseRef=d475458 窗口）exit 0 + 缺陷 1 三态五测独立复跑通过 + 缺陷 2 `_doc`/实现/summary 三方一致 + 自检全绿 + 修复零夹带 → **PASS，放行 commit 分支**。merge 归 human manual-merge 门；本 verdict + PROPOSAL + landing-summary + retro 构成 Track B 四件套证据链，retro 待补写闭环。
