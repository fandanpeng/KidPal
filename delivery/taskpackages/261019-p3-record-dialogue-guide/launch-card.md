# 启动卡 · 261019-p3-record-dialogue-guide

跑 `261019-p3-record-dialogue-guide`（单包，rigor=full，riskTier=high；无 workstream/mergeOrder——triage 收尾分档，非拆包产物）。

- **reviewed plan**：`delivery/taskpackages/261019-p3-record-dialogue-guide/plan.md`（triage 2026-10-19 architect 分流 ②单包 Full + human 触点①确认）；`taskpackage.json` 已建并 commit（fe05c844）——check-launch 对基准 ref `dev/pod-m2` **GREEN**（非拆包产物，基准非 main）。
- **上游 anchors（全已 landing，不再等任何上游面）**：req = requirements §9 v1.65（S-F2.2 全篇重写 + NFR-PERF-3）；arch = 261019-arch-review 批 landing（决策 25 改写 + ADR-013 + §5.1.2/§5.2④）；contract = CDQ-261019 LANDED（`start_organizing` 已入 docs/contracts/interface-contract.md §1.4）。硬前置 261016-p3-story-body-sd-cache 已合并（b2e2ceb3 在本树基线内，实测 ancestor）。
- **基线锚**（非拆包产物，无 workstream progress.md；两面分记）：
  - server 面：现跑 2026-10-19 @ HEAD fe05c844，`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance` → **210 passed / 0 failed / 4 deselected / 33 subtests，exit 0**。零红。
  - firmware host 面：带锚引用 `delivery/taskpackages/261018-sd-log-usb-read/review-packet.md:16`（量测 commit `6307c6aa`，2026-10-19 本树实测）：host ELF **210 Tests / 3 Failures exit 3**，3 条全 = p1_auxbar AB-A1/A2/A4 上游先存红；diff 判停 = `6307c6aa..HEAD -- firmware/` 零差异 → 引用有效，不重跑。
- **上一包留根**：`delivery/taskpackages/261018-sd-log-usb-read/pod-retro.md`（本树最近一个 dev Pod）；交接卫生已核——tracked 改动仅本卡与 dev INDEX 投影行，随本卡 commit 归零。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 rigor=full，无 defer 短路——scoped/final 恒跑）。
- **结构性义务两件**（plan §结构性风险，implementation 期红线）：① display_schema 自推校验分叉（自推不被自拒、relay 拒转语义一字不动，end_session 拒转断言保绿）；② engine-patch stop-truncation 豁免重绑（判据从 manual 模式重绑为记录语境标志，防 KEY 收尾末句被截断毁稿；261014 截断齿保绿）。
- **live/真机门前停等（本包特有）**：S2 LLM 行为 live 齿需真 OpenClaw loopback（18789）在位；S5 端到端联调 + 真机轮需 human 持板——到门即停，status 写 `awaiting-human:real-device`，观察项整理成持板操作单（含 S-F1.4 AC6 录音期提醒排队不回归项）。
- **合并门预期**：riskTier=high → supervisor 合并门升级 human（预期停 status=`awaiting-human:merge-gate`，属单向守卫正常行为，非故障）。
- **会话标识**：interactive 会话（2026-10-19 点火，worktree C:/AI/KidPal-pod-m2，分支 dev/pod-m2）。

## 命令行

① 人在场交互式（本次实际形态）：

```
claude --settings .claude/dev-loop.settings.json --agent planner
```

② 无人值守 headless（不阻塞在场）：

```
CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261019-p3-record-dialogue-guide —— 开场即读 delivery/taskpackages/261019-p3-record-dialogue-guide/taskpackage.json + plan.md + spine delivery/intake/261019-p3-record-dialogue-guide.md；riskTier=high 合并门到 human 即停（status=awaiting-human:merge-gate），真机门到门即停（status=awaiting-human:real-device），见本启动卡"
```
