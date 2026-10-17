# 启动卡 · 261016-p3-story-body-sd-cache

跑 261016-p3-story-body-sd-cache（非 workstream 拆包产物：单包，rigor=full，riskTier=**high**；无 mergeOrder）。

- **reviewed plan**：`delivery/taskpackages/261016-p3-story-body-sd-cache/plan.md`（triage 2026-10-17：architect 分流原判 ③拆包，human 触点①改判 **②单包 Full**；riskTier high 不降——改的是档位不是风险级）。intake 读 plan 建 taskpackage.json 并**首提交**：`--check-launch --base-ref main` 当前唯一 blocking = taskpackage.json 不在基准 ref，属本流程预期，intake 首提交后即消。plan 附录「结构化种子」= intake 直接输入（changeManifest 9 项 / impactedTests 三面 / sourceRefs / origin / gateProfile / crossEnd / decisionCoverage）。
- **上游 anchors（spine 三面全回填，launch 放行）**：需求 v1.64 frozen（S-F2.1 AC2 兑现 + 新增 AC7 + 正文缓存边界 + WF-F2-06）；架构 261017 批 landing b3f50ebd（决策 24 carve-out + ADR-011 + 六处禁令重述点）；契约 CDQ-261017 LANDED（§四 `kidpal_story_fetch` + §1.3④ `story_text` 消费语义）。spine = `delivery/intake/261016-p3-story-body-sd-cache.md`（`imp` anchor 待回填，归本包 closeout）。
- **基线锚 @ b6cc8945（本树实测，producer 五要素）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance` → **174 passed / 0 failed / 4 deselected / 33 subtests passed，exit 0**（2026-10-17；面域 = server）。零红。
  - firmware host 面：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **182 Tests / 3 Failures / 0 Ignored**（2026-10-17；面域 = firmware；runner exit 0，判红以 summary 行为准）。3 红 = `p1_auxbar_style` 的 AB-A1 / AB-A2 / AB-A4——261015-p1-auxbar-style-fix 的 04f8409a「看屏轮打回直修」改了实现（ic_batt_15→ic_batt_20、thermo 落位 x0=218 越出齿域 [211,217]、wifi 位图退役）而冻结齿未同步 = **上游已知红，非本包责任**；首轮 verifier 见红先对基线，新增红才算本包红；基线自身的消解另立批。
- **上一包留根**：`delivery/taskpackages/261016-sd-card-mount/pod-retro.md`——intake 逐条评估；两条与本包直接相关：① scope-map 要按 agent 粒度核「doc-sync 落点写权」（planner deny `docs/**`，以 `--agent planner` 起的 Pod 主会话里「主线程代落」通道不成立）；② 措辞上区分「无 agent_type 裸主会话」与「`--agent <role>` Pod 主会话」。
- **固件严格模式默认开**：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开；本包 rigor=full，无 defer 短路）。
- **riskTier=high 的下游形态（预期，非缺陷）**：supervisor 合并门升级 human；真机 live-manual 轮 human 持板（SD 读回 roundtrip / 双击三类记录进阅读 / 离线降级提示 / id 错配回归位 / 预取期语音不卡顿）。
- **运行面**：worktree = `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`，= main tip b6cc8945、零独有提交）；专属图索引 `C-AI-KidPal-pod-m2` 在册（moderate，ready）；固件环境在位（sdkconfig / dependencies.lock / host_app sdkconfig / managed_components 软链 / host build 热缓存）。
- **会话标识**：待点火后回填（叫停时核「进程确实已死」用）。

## 命令行（worktree = C:/AI/KidPal-pod-m2，分支 dev/pod-m2）

① 人在场交互式：

```
claude --settings .claude/dev-loop.settings.json --agent planner
```

② 无人值守 headless：

```
CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261016-p3-story-body-sd-cache —— 从 delivery/taskpackages/261016-p3-story-body-sd-cache/plan.md 读 reviewed plan 做 intake（建 taskpackage.json 并首提交，消 check-launch blocking）；changeId=261016-p3-story-body-sd-cache；真机轮前停等 human，见启动卡 riskTier 条款"
```

> 注记：`--dangerously-skip-permissions` 形态被平台分类器硬拦（260924 批 + memory 实证），headless 用上方无旗标形态，依赖 dev-loop.settings.json allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑。
