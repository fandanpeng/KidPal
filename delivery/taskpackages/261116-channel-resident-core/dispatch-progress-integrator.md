# dispatch-progress · integrator · 261116-channel-resident-core（真机轮 C 修复轮 · final r9 + 收口）

> 本文件 = 当前相位（integrator：final 终验 + 收口制品 + 本包 commit）的自包含进度快照。
> 覆盖写；不载 tokens / gates 明细 / verdict（权威归 verifier fact 与 supervisor-verdict）。
> 写于 2026-11-18；本包**未合并、未 push**，真机轮 C 未跑。

## 本轮范围（integrator 相位）

1. 范围圈定（Step 4 ①）→ 生成 `test-scope-final-r9.txt`。
2. final 轮 r9 实跑（`verify.mjs --evidence-mode=round --phase=final --scope-file … --round 9`）+ `verifier-evidence-check` 自检。
3. 收口制品：`review-packet.md` / `docs-delta.md` 追加轮 C 节；`dispatch-progress-integrator.md`（本文件）；本包 commit；`pod-closed` 事件；`closeout-check.mjs`（只读，不传 `--update-index`）。

## 已完成

### 1. 范围圈定

- 落盘件：`delivery/taskpackages/261116-channel-resident-core/test-scope-final-r9.txt`（31 条去重测试文件路径，一行一件 + `#` 注释；r7 的 `test-scope-final.txt` 保留为历史）。
- 合成来源：`acceptanceTestsFile`（6）∪ `impactedTestsSeed[]`（25 去重）∪ `detect_changes`。
  - 包级：project `C-AI-KidPal-pod-channel`，since `base/261115-session-channel-resident` / merge_base `3b3832cb`，inbound depth 2 → 89 changed files / seed 490 符号 / impacted_total 138（server/acceptance 78、server/gateway 21、server/tests 16、firmware/main 23）。
  - 本轮增量：`since HEAD`（覆盖未提交工作树）→ 18 changed files / seed 46 / impacted_total 51，与包级指向同一 server 测试面。
- 圈定路径：per-Pod 专属图索引可用（full），detect_changes 与 trace_path 均可用 → **正常圈定、未降级**。
- 相对 r7 只增一项：`server/tests/test_story_units.py`（architect 建议、impactedTestsSeed 已登记；story.py 三缝零 diff 的负锚）。
- 排除留痕：`server/acceptance/archive/test_schedule_reminder.acceptance.py`（archive 退役面，两闸 SKIP_DIRS）；`p1_auxbar_style.acceptance.c`（上游存量红）；`test_engine_round_close_units.py`（engine-patch 零触碰）。

### 2. final r9 实跑

- 命令：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261116-channel-resident-core/test-scope-final-r9.txt --change-id 261116-channel-resident-core --round 9 --invocation-id 9cee1805-4f5b-4621-bf69-be61013cb78f`
- 结果：`{"ok":true,"phase":"final","round":9,"observedGateExit":0}`，fact 落 `verifier-evidence/rounds/r9.json`；10 闸全跑全绿。
  - 七闸 pass：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ contract-lint（1 既有 warning）/ acceptance-coverage（31 id 全覆盖）/ mutation-check / docs-sync / security。
  - firmware-build `build-pass`（xiaozhi.bin 0x329580 bytes / 8% free）。
  - unit（full，scope 求交）**302 passed / 3 deselected / 102 subtests**（33.6s）。
  - acceptance（full，scope 求交）**204 passed / 3 deselected / 13 subtests**（34.1s）。
- 自检：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261116-channel-resident-core --phase final --round 9 --invocation-id 9cee1805-…` → exit 0 / factOk=true。
- 前置 r8 scoped：`r8.json` 八闸 ok / observedGateExit 0 / invocationId ffeeab66-d5f9-4a44-849a-40ba9faaf93b。

### 3. 实跑补充（integrator 亲跑）

- host runner（WSL Ubuntu-24.04，执行目录 `firmware/main/test/host_app`）：`321 Tests / 3 Failures / 0 Ignored`——3 红全为上游存量 `p1_auxbar_style.acceptance.c`（AB-A1/A2/A4），本轮固件零 diff。
- server 全量：`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance server/tests` → **799 passed / 0 failed / 9 deselected / 137 subtests**（78.4s）。

### 4. 收口制品

- `review-packet.md`：追加「真机轮 C 修复轮（final r9，2026-11-18）」节（十三小节：Change Size / 缺陷与修复 / Verification Evidence / Per-Acceptance 轮 C 4 条 / human 原话口径分法 / 上游残余与 builder 观察项 / 未覆盖面申报 GAP-C1/C2 + EQ-C1/EQ-C2 / Demo 对账 / 真机轮 C 复测清单 / 工具缺口 / Five-Axis / Findings 4 条 NIT / Residual Risk）。历史节零删。
- `docs-delta.md`：追加「真机轮 C 修复轮增记」——docsSyncRequired=false 与 docs/** 零 diff 实测一致；契约/requirements/architecture/ADR/debt 五类逐条未改；上游引擎静音幻觉建议主线程立项；spec 漂移沿用待裁四。

## 未完成 / 边界

- **未生成 `device-round-evidence.md`**、未申报任何真机 PASS（真机轮 C 未跑）。
- **未合并、未 push、未触发 merge-driver**；合并门（supervisor + 前置升级闸）未跑。
- 未改实现代码、验收测试、`docs/contracts/**`、`docs/**`；final 无红，无回退手。
- 已知工具缺口沿上一轮（G4 verify-logs / parallel-overlap-tier3 生成件误判 / change-manifest 经 current/ 恒判 pre-cutoff），归主线程。

## 下一步（交 planner / 主线程）

1. planner 跑 `supervisor-verdict-gate.mjs` 机械闸（本包 declared riskTier high → 预期 ESCALATE；证据完整）。
2. **真机轮 C 复测（human 持板）**：A6 / A7 / D3 空素材 / A5 回归 / 引导播报回归 + 本轮追加「整理占位回归 + `dropped=silent_asr_hallucination` 观察」+ 诊断采集项；通过后交付 `sourceCommit` + `sha256(xiaozhi.bin)`，才生成 `device-round-evidence.md`。
3. 工具缺口修复（G4 / tier3 union / manifest cutoff）与 spec 轻量注记归主线程。
4. 两条 debt（TECH-261114 / TECH-261012）维持 deferred，结项凭据 = 真机轮 C。