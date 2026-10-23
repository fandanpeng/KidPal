# 启动卡 · 261022-m5-k1-prompt-carrier-migration

跑 `261022-m5-k1-prompt-carrier-migration`（workstream device-memory-overhaul P1，rigor=full，riskTier=medium，sourceType=proposal，mergeOrder=50，dependsOn=[]；五包单 Pod 串行首包）。

- **reviewed plan**：`delivery/taskpackages/261022-m5-k1-prompt-carrier-migration/plan.md`（拆包四件套 commit `af5dc071`，门 1 GREEN + human 终裁 A1–A5 落盘 `review.md`；上游复核见 workstream `progress.md` 2026-10-22 两节）。
- **上游面**：launch 前置 LP1–LP5 全满足——requirements v1.67 frozen（b4b9c90a）；arch changeId 行 `261022-device-memory-prompt-overhaul` landed（0411e957，landing-verdict PASS，§5.5.5 / ADR-001 重定义 / 契约 §四 :283 + §1.6 注记随批原子落）；commit-before-launch 已落 `dev/pod-m2`（非 main，LP5）。
- **基线锚**：本包只触 server 面（firmware 零 diff，crossEnd exempt）；server 面红基线 = `cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance`，由 acceptance-author 冻结相位实跑回填（基线预期 = 既有套件现行绿 + 新增断言设计态红）。固件 host 面 236T3F 先存红与本包无涉，不作锚。
- **已知窗口（判红豁免，写进派发即生效）**：AGENTS.md 仓内镜像改动后、live 铺放前 `verify-deploy.sh` 报该文件 md5 漂移属预期，不据此判红；**本包不得单独铺放该文件**（v11 语义未迁入时提前反转承载声明 = 护栏真空，arch PROPOSAL §5 第 11 条）。manifest marker 锚「实时事实回合」为 v11 静态内容锚，本包必须同批撤换为动态块锚，否则部署核验必红（plan.md「可行性判据」末条）。
- **live 门前停等（本包特有）**：K1-E7（prompt_tokens ≤11,000 + cache 命中 + 工具列表 4 项）与 K1-E8（SOUL/USER live 清理，launch card 人工步骤登记）均为 live 观测项——host 面转绿后 **status 写 `awaiting-human:live-deploy`**，停门等 human 执行 deploy.sh + 部署轮观测；Pod 不自行上 ECS。
- **合并门预期**：riskTier=medium 非 high，supervisor 机械闸正常判；gateProfile 三项（evidence-integrity / test-quality / ai-cost-audit）。
- **共享冻结面纪律**：与 P2/P3/P5 共享 `manifest.yaml` / `provider.config.yaml` / `apply-xiaozhi-overlay.sh` / `OPENCLAW_SYSTEM_PROMPT_CARRIER` / `workspace-kidpal/AGENTS.md`——本包先落，不得越写 P3 的 v12 守则内容；P5 消费本包动态块占位面；`openclaw.json` 不入仓（含 gateway 凭据），只入无 secret 声明 + apply/rollback/drift_guard（仿 `261017-openclaw-kidpal-skills` 先例）。
- **会话标识**：human 在场交互式会话（VSCode 扩展，2026-10-23 点火；本树即 Pod 本体就地起 loop，m2-char-window / 261010-p3-storylist 先例，不另建树不派 headless）。
