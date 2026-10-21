# 启动卡 · 261021-p3-record-cancel-lifecycle

跑 `261021-p3-record-cancel-lifecycle`（单包 ② Full，rigor=full，riskTier=high，sourceType=regression；无 workstream/mergeOrder——triage 收尾分档，非拆包产物）。

- **reviewed plan**：`delivery/taskpackages/261021-p3-record-cancel-lifecycle/plan.md`（triage 2026-10-21 architect 分流 ② Full + human 触点① 确认；OQ1=Design B `kidpal_story_discard` / OQ2=cancel-on-disconnect 维持 / OQ3=复验③重定义(a)）；`taskpackage.json` 本会话 planner Step 4 起草（interactive 形态，起草后复跑 `--check-launch` 补绿——launch 时点唯一 blocking 项即此件）。
- **spine 上游面已闭环**：`delivery/intake/261021-p3-record-cancel-lifecycle.md` arch/contract anchors 已回填（architecture.md §12 changeId 行 2026-10-21 landed + interface-contract.md §四 `kidpal_story_discard` LANDED；FROZEN v1.1 + R2 GREEN；q1–q4 全 resolved）。
- **基线锚**（非拆包产物，无 workstream progress.md）：
  - server 面：**引用锚**——261020 启动卡锚行（SHA b8aaa82b，2026-10-20 现跑：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance` → 229 passed / 0 failed / 4 deselected 零红）；本树实测 `git diff --quiet b8aaa82b HEAD -- server` 零差异，三件齐（SHA + 命令 + 卡内落盘锚），合法引用不重跑。
  - firmware host 面：**现跑重锚**——2026-10-21 @ HEAD be93ca5e（本树实测）：host_app 冷构建（陈旧 gitignored sdkconfig 已按换包协议删除、从 tracked defaults 重生成）`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh -T main build` 绿 + `host_infra_test.elf` 实跑 → **220 Tests 3 Failures**，3 条全 = p1_auxbar AB-A1/A2/A4 先存红（与 261018/261019/261020 基线同批）；story/record 区零红。
- **上一包留根**：`delivery/taskpackages/261020-p3-rec-frame-visual/pod-retro.md`（本树最近一个 dev Pod；intake 逐条评估，构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)；本包 rigor=full，scoped/final 相位恒真编译，无 defer 短路）。
- **烧板轮不可省**：mcp_server.cc / protocol.cc 属设备编译单元（spine facePlan 顺序2 明示），真机轮按 full 环境备。
- **会话标识**：interactive 会话（2026-10-21 点火，worktree C:/AI/KidPal-pod-m2，分支 dev/pod-m2；主线程 /build-feature 已在本会话内联进 intake）。

## 命令行

① 人在场交互式（本次实际形态）：

```sh
claude --settings .claude/dev-loop.settings.json --agent planner
```

② 无人值守 headless（不阻塞在场）：

```sh
CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261021-p3-record-cancel-lifecycle —— 开场即读 delivery/taskpackages/261021-p3-record-cancel-lifecycle/taskpackage.json + plan.md；spine anchors 已回填（arch/contract LANDED）；full 档烧板轮不可省，见本启动卡"
```
