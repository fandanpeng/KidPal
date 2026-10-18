# 启动卡 · 261017-prompt-v11-schedule-recall

跑 261017-prompt-v11-schedule-recall（非 workstream 拆包产物：单包，rigor=**lite**，riskTier=**medium**；无 mergeOrder）。

- **reviewed plan**：`delivery/taskpackages/261017-prompt-v11-schedule-recall/plan.md`（triage 2026-10-18：architect 判 ②单包 Lite，human 触点①确认）。intake 读 plan 建 taskpackage.json 并**首提交**：`--check-launch --base-ref main` 当前唯一 blocking = taskpackage.json 不在基准 ref，属本流程预期，intake 首提交后即消。
- **spine（两 debt 的消费账本）**：`delivery/intake/261017-prompt-v11-schedule-recall.md`，`status: in-flight`。facePlan：**实现面先行、架构面后置**——架构面只是 `docs/architecture.md` §5.5.5 / §5.1 的现状指针 docs-sync，但 `docs/architecture.md` 只有 architect 有写权（`integrator.allow` 不含、`planner.deny` 含 `docs/**`），故**不在本包内**，由 dev 包落地后另起 `--agent architect` 会话翻指针；需求/契约/治理面零变更。closeout 时回填 `imp` anchor 并把 status 翻 `closed`。
- **消费的 debt 条目**：`docs/debt/tech/TECH-261017-schedule-answer-skip-read-hallucination.md`（MAJOR）与 `docs/debt/tech/TECH-261017-xiaozhi-template-residue-deploy-ssot-gap.md`（SHOULD）；closeout 时翻 `status: closed` + 正文结项行 `consumed-by: 261017-prompt-v11-schedule-recall`，重跑 `node .claude/scripts/debt-render.mjs`。
- **基线锚 @ aa0bbd32（本树实测，producer 五要素）**：
  - server 面：`python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance` → **181 passed / 0 failed / 4 deselected / 33 subtests passed，exit 0**（2026-10-18；面域 = server）。零已知红。
  - firmware host 面：本包零固件改动，不量测；首轮 verifier 若因他因见红，先对基线再判。
- **上一包留根**：`delivery/taskpackages/261016-p3-story-body-sd-cache/pod-retro.md`——intake 逐条评估。
- **lite 快路径**：`rigor=lite` 且 triage 未声明「快路径不适用」（host 齿可闭环：server pytest + loopback 探针 + `verify-deploy.sh`；零设备编译 seam）。固件环境等价化按快路径后置；本包实际零固件改动，不触发。**专属图索引已在册**（`C-AI-KidPal-pod-m2`，moderate），维持 intake 前在册。
- **`riskTier=medium` 的下游形态**：supervisor 合并门按常序；不预期升级 human。
- **人工位（headless 到门即停）**：
  1. **ECS 部署**——`.config.yaml` overlay 重跑、openclaw.json skills apply、引擎重启都只在 ECS 上做，按仓规「engine-patch / 部署面改动只 commit 到分支、不自动部署」，human 在场执行；
  2. **真机 live 轮**——问「后天都有什么课程？」核 `read` 动作与周一课表、`openclaw skills list --agent kidpal` = 3、兜底语不含「小智」；human 持板。
- **运行面**：worktree = `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`）；board 条目 `devloop/pods/261017-prompt-v11-schedule-recall.json`（status=queued）；专属图索引 `C-AI-KidPal-pod-m2` 在册。
- **会话标识**：点火后回填（叫停时核「进程确实已死」用）。

## 命令行（worktree = C:/AI/KidPal-pod-m2，分支 dev/pod-m2）

① 人在场交互式：

```
claude --settings .claude/dev-loop.settings.json --agent planner
```

② 无人值守 headless：

```
CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0 claude -p --settings .claude/dev-loop.settings.json --agent planner "/build-feature 261017-prompt-v11-schedule-recall —— 从 delivery/taskpackages/261017-prompt-v11-schedule-recall/plan.md 读 reviewed plan 做 intake（建 taskpackage.json 并首提交，消 check-launch blocking）；架构 docs-sync（docs/architecture.md 指针）不在本包、由 dev 落地后另起 architect 会话；changeId=261017-prompt-v11-schedule-recall；ECS 部署与真机轮前停等 human，见启动卡人工位"
```

> 注记：`--dangerously-skip-permissions` 形态被平台分类器硬拦（260924 批 + memory 实证），headless 用上方无旗标形态，依赖 dev-loop.settings.json allowlist；allowlist 缺项一步会失败停下，回主会话补权限再续跑。
