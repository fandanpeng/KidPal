# gate-tooling-gap · G4 verify-logs 对「全绿 Pod」结构性不可满足

> 发现相位：`261116-channel-resident-core` 收尾（planner）。性质 = **gate 工具缺口**，非本包证据缺失。
> 状态：已向 human 升级（pod-events `agent-escalation` 90304f5e / 7b218c6c）；本文件供主线程修复与复核使用。

## 现象

`node .claude/hooks/supervisor-gate-check.mjs` 在本包恒红：

```
merge-gate BLOCK: G4 FAILED: no verify-logs found with content.
The verifier gate logs (.claude/verify-logs/*.log) are missing or empty — gates were not actually run.
```

`.claude/verify-logs/` 目录**不存在**。

## 根因（代码实证）

- `supervisor-gate-check.mjs` 的 G4（`:48` `{id:'G4', name:'verify-logs-existence', optional:false}`，实现 `:176-181` / 判点 `:717-724`）**无豁免分支、无 env 旁路**，仅看目录内 `.log` 是否非空。
- 唯一写该目录的是 `verify.mjs`：`writeGateLog(gate, out)`（定义 `:191-202`）唯一调用点在 **`if (!ok)`** 内（`:288-290`）。
- 推论：**gate 全绿的轮次不产生任何日志**。本包 scoped r1、final r2 均十闸全绿（`verifier-evidence/rounds/r1.json` / `r2.json`，`ok:true`），故目录从未创建 → G4 恒红 → Stop 门无法放行。

## 本包「闸确实跑过」的旁证（G4 想查的东西其实在）

- `delivery/taskpackages/261116-channel-resident-core/verifier-evidence/rounds/r1.json`（scoped，8 闸 ok）
- `delivery/taskpackages/261116-channel-resident-core/verifier-evidence/rounds/r2.json`（final，10 闸 ok，含 `VERIFY_FW_BUILD_REQUIRED=1` 真编译）
- `delivery/taskpackages/261116-channel-resident-core/verify-runs.jsonl`（verify.mjs 每轮机械 append）
- `node .claude/scripts/verifier-evidence-check.mjs --change-id 261116-channel-resident-core --phase final --round 2 --invocation-id e5cb4370-0470-4df6-a1ab-316ac66cb4aa` → exit 0 / `factOk:true`
- 功能套件实跑（planner 亲跑）：host `306 Tests / 3 Failures`（3 红 = 上游存量 `p1_auxbar_style`），server `473 passed / 0 failed`

## 建议修法（二选一，均属治理面，须主线程 / `/gov-change`）

1. **（推荐，最小）** 把 `verify.mjs` 的 `writeGateLog` 调用移出 `if (!ok)`：每闸每轮无条件落 log（失败仍保留 logTail → ErrorEnvelope 路径不变）。产物语义变为「本 Pod 的每轮每闸轨迹」，G4 恢复本意（证明闸跑过）。参考 patch：

```diff
--- a/.claude/hooks/verify.mjs
+++ b/.claude/hooks/verify.mjs
@@
-    if (!ok) {
-      const logFile = writeGateLog(gate, out);
+    /* G4 修复（261116 批）：全绿轮次也落 log——G4 的语义是「闸跑过」，不是「闸红过」。
+     * 文件名已含 Date.now() + gate.id，每轮独立；日增长按 gate 数 × 轮数，可接受。 */
+    const logFile = writeGateLog(gate, out);
+    if (!ok) {
       const error = {
         name: 'GateFailure',
         category: categoryFor(gate),
```

2. G4 改为接受等价的机械证据源（`verifier-evidence/rounds/*.json` 或 `verify-runs.jsonl` 非空），并保留目录检查作 advisory。

**不建议**：为过闸而制造一次失败 log（伪造证据）；或以 `touch` 造空/假 log。

## 影响面

所有「从第一轮起全绿」的 Pod 都会在会话收尾被 G4 拦死（本包是首例实测）。有红轮的 Pod 因失败分支已写过 log，不受影响——这也是该缺口长期未被发现的原因。

## 复现

```bash
cd <worktree>
rm -rf .claude/verify-logs
VERIFY_FW_BUILD_REQUIRED=1 node .claude/hooks/verify.mjs --evidence-mode=round --phase=scoped \
  --change-id <changeId> --round <n> --invocation-id <uuid>   # 全绿
ls .claude/verify-logs/   # 不存在
node .claude/hooks/supervisor-gate-check.mjs   # G4 BLOCK
```