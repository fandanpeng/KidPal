---
id: ARQ-260924-settings-write-deny-rule-cli-drift
title: `.claude/dev-loop.settings.json` + `.claude/review-loop.settings.json` 的 `Write(./docs/contracts/**)` deny 规则被现行 CLI 判非法并**硬阻塞 launch**（exit 4）——CLI settings 校验新口径：文件权限检查只认 `Edit(path)` 规则（Edit 规则覆盖所有文件编辑工具），`Write(path)` 规则不再被匹配
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下个治理快批（headless 恢复使用前必须完成；交互式 launch 亦建议先行）
opened: 2026-09-24
closed: ""
closedBy: ""
paths: []
source: 260924-perf-k1 launch 实测（pod-perf-k1-launch.log）+ auto-mode classifier 两次拒绝回执 + §9 勘误（fb8b169）
---

**证据**（2026-09-24 01:28 CST 主线程，260924-perf-k1 headless launch 实测）：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner` 启动即败，日志唯一行（C:/tmp/pod-perf-k1-launch.log，235B）逐字：「Permission deny rule (.claude\dev-loop.settings.json): Write(./docs/contracts/**) is not matched by file permission checks — only Edit(path) rules are. Use Edit(./docs/contracts/**) instead (Edit rules cover all file-editing tools).」，exit 4；两文件 deny 段均为 `[Edit(./docs/contracts/**), Write(./docs/contracts/**)]` 两行——Edit 行已在且按 CLI 口径覆盖所有文件编辑工具，Write 行在新 CLI 下零执行力、纯阻塞 launch。主线程先后两种修复形态（仓外 fixed 副本 / 仓内 Edit 删无效行）均被 auto-mode classifier 拦截（Self-Modification：移除 deny 规则须 human 本人提出）——**机械强制层变更即使语义无损也不得 agent 自批，拦截正确**，修复权留 human/治理快路径。**建议改动**（候选，不自批）：① human 批准后删两文件 Write 行（Edit 行承载冻结语义；scope-map universal.bashDenyWrite + enforce-scope 钩子双拦不变，「内核层+钩子层」双保险仍在）；② settings 校验 dry-run 机检入治理机检面（防 CLI 升级再静默炸 launch 面）；③ 排查 settings 族其余旧形态规则同病面。**riskTier: low**。owner=architect。**勘误与进展补记（2026-09-24 晨）**：①成因勘误——**非「CLI 升级致校验新口径」**：本机唯一 CLI 安装 = npm 全局 2.1.268，装于 2026-09-11 12:40 后未变（where claude 唯一命中；AppData 各 Claude 目录为 desktop/遥测非本体）；K7（260923）交互式 launch + 同一份 settings 跑通全程，本包 260924 01:28 **首次 headless（-p）launch** 即被硬拒 exit 4——无效规则长期潜伏（至少 9/11 前即在），**触发条件 = headless 与交互式对无效权限规则的容忍差**（K7 交互 launch 是否带 --settings 无法从现存证据判定，两可如实记；若带 --settings 的交互式同样 exit 4，则修复对两形态均为前置）。证据 = 260924-perf-k1 evidence/latency-baseline-260924.md §9（pod 分支 fb8b169）。②建议②措辞随之修正为「**headless launch 面 settings 合法性 dry-run 机检**」（launch-prep 增一步：以目标 settings 跑一次最小 -p 冒烟）。③human 裁定（2026-09-24 晨）：**暂不以 headless 跑 launch，改交互式形态**——本 ARQ 的「launch 阻塞」紧迫性降级；无效行删除仍建议照做，逐字 patch 已呈 human 授权中（批准前主线程不动 .claude/**）。 **终裁（2026-09-24 晨，human 原话「不删，我们不走 headless 不就行了，这个留个 gov 去处理」）：两行不删，逐字 patch 撤回不执行；本 ARQ 整件移交 gov 批处置（裁量面 = 删与否 / headless settings dry-run 机检 / 交互式带 --settings 是否同病的实测）；在此之前 launch 一律交互式，主线程与 Pod 对 .claude/** 零触碰。**
