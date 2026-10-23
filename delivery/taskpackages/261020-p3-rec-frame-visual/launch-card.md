# 启动卡 · 261020-p3-rec-frame-visual

跑 `261020-p3-rec-frame-visual`（单包，rigor=nano，riskTier=low；无 workstream/mergeOrder——triage 收尾分档，非拆包产物）。

- **reviewed plan**：`delivery/taskpackages/261020-p3-rec-frame-visual/plan.md`（triage 2026-10-20 architect 分流档 0 nano + human 触点①确认，意图定义五件在册）；`taskpackage.json` 已建（nano profile：acceptance = suiteGate 纯形态、红基线豁免）。
- **基线锚**（非拆包产物，无 workstream progress.md；两面现跑 2026-10-20 @ HEAD b8aaa82b，本树实测）：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance` → **229 passed / 0 failed / 4 deselected，零红**。
  - firmware host 面：espidf_wsl.sh build 绿 + `host_infra_test.elf` 实跑 → **220 Tests 3 Failures exit 3**，3 条全 = p1_auxbar AB-A1/A2/A4 先存红（与 261018/261019 基线同批）；story/record 区零红。
- **上一包留根**：`delivery/taskpackages/261019-p3-record-dialogue-guide/pod-retro.md`（本树最近一个 dev Pod；三条固化：诊断先行 / 冲突预检 / 扩单预授权，intake 逐条评估）。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（261015 批 T3 起 defer > required——本包 rigor=nano，scoped 相位 defer 短路优先于本行，final 相位恒跑）。
- **nano 义务三件**：diff 不越 changeManifest（story_page.c 单文件，越界停回 triage，无扩单通道）；白盒复核（detect_changes 实际爆炸半径 vs plan 五件⑤）记 review-packet；真机目检 human 终判（本 Pod 到门停等 `awaiting-human:real-device`，不自行烧录——观感直修先例）。
- **会话标识**：interactive 会话（2026-10-20 点火，worktree C:/AI/KidPal-pod-m2，分支 dev/pod-m2）。

## 命令行

① 人在场交互式（本次实际形态——主线程 /build-feature 已在本会话内联进 intake，Pod 子 agent 同会话派发）：

```
claude --settings .claude/dev-loop.settings.json --agent planner
```

② 无人值守 headless（不阻塞在场）：

```
CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261020-p3-rec-frame-visual —— 开场即读 delivery/taskpackages/261020-p3-rec-frame-visual/taskpackage.json + plan.md；nano 档：真机目检门到门即停（status=awaiting-human:real-device），见本启动卡"
```
