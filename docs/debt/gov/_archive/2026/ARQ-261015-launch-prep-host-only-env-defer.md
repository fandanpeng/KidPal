---
id: ARQ-261015-launch-prep-host-only-env-defer
title: launch-prep 对 host-only 可验 Lite 包缺「环境后置」快路径——前置整套固件环境等价化对不依赖 IDF 的包是纯开销，每树重复收取
domain: gov
status: closed
closed: 2026-10-15
severity: SHOULD
owner: architect
trigger: 下一个 host-only 可验 Lite 包 triage/拉起前；或下一治理批顺带
opened: 2026-10-15
paths: [".claude/skills/kidpal-pod-launch-prep/SKILL.md", ".claude/commands/build-feature.md", ".claude/agents/planner.md", ".claude/references/task-intake-and-slicing.md", ".claude/rules/repo-structure.md"]
source: ["delivery/taskpackages/261015-voice-rightcol-label-overflow/taskpackage.json（commit 31d3ec3，triage→launch 实际执行）", "git worktree list 在册 C:/AI/KidPal-pod-261015-voice-rightcol（dev/261015-voice-rightcol-label-overflow，2026-10-15 11:20 建）", "headless planner 会话 transcript（C:/Users/15802/.claude/projects/C--AI-KidPal-pod-m2/97d01997-0f5a-4cef-9de2-7b93bbcaad68.jsonl，intake→建包→worktree→作用域引导全时间线可复核）"]
---

**证据**：261015-voice-rightcol-label-overflow（3 行 LVGL label 宽度修复）triage 后拉起 headless planner Pod，实测时间线（transcript 可复核，2026-10-15 本地时）：intake 尽调 11:13→11:18（~5 min）→ 建包 + 机械闸 11:18:57→11:20:02（~1 min）→ `git worktree add` 11:20:02（秒级）→ 作用域引导 11:20:53 起（~3 min 仍在进行）。按 kidpal-pod-launch-prep SKILL.md:17-18 与 `.claude/rules/repo-structure.md`「图索引项目归属」节，launch 面还须办：固件环境等价化（SKILL.md 步骤 4）+ 专属图索引（moderate 全量，**「launch 面必办，不得降级读文件」ARQ-260905 规则钉死**）。而本包 host 验收全部走 `firmware/main/test/host_app` target（gcc 即可，不依赖 IDF），IDF 构建只在真机烧录轮需要一次——human 判定环境准备成本远超代码改动成本（human 原话口径：「修改代码可能就几分钟」，环境准备分钟到十几分钟级且每树重复收取），指示在途包降级（固件环境活后置真机轮前、图索引不阻塞 host loop）为**一次性偏离**（261015 包已按此执行，先例不自动扩为常驻规则）。成本事实：SKILL.md 步骤 4 配方里 managed_components 软链共享主 checkout 缓存、IDF 工具链机器全局——每树固件环境等价化本身分钟级；每树真正重复的大头是专属图索引（moderate 全量索引）与 intake/作用域引导。

**影响面**：所有「host-only 可验」的小包（显示层、纯逻辑 seam、host 齿可闭环的 Lite bug/小 feature）拉 Pod 时都要为用不上的固件环境支付 launch 面成本——图索引全量索引（本仓规模十几分钟级）+ 环境等价化 + 作用域引导；包越小占比越荒谬（本包代码改动 3 行）。不修的代价：小包吞吐被 launch 面拖慢，human 对「小活拉 Pod」望而却步（本次 human 已明示「搞一次太贵了」），倾向绕过 dev-loop 直修——恰是治理路由想防的。关联面：TECH-261014-host-app-sdkconfig-env-drift 记的 host_app sdkconfig 镜像缺位问题同在 launch 面步骤 4 的镜像清单语义内，若立「环境后置」快路径，该镜像义务须一并后移或明示豁免，防两条款打架。

**建议改动**（候选，交 review、不自批；**存在真设计分叉，见风险段——不走快路径落地**）：amend kidpal-pod-launch-prep SKILL.md + build-feature.md launch 面预检 + planner.md Step 1 + task-intake-and-slicing.md launchPreconditions，为 host-only 可验的 Lite 包立「环境后置」快路径：launch 面只做树 + 分支 + 作用域引导；固件环境等价化降级为「真机轮前置条件」（届时一次性做，本树 IDF build 失败允许退主 checkout 构建烧录）；专属图索引允许后置或暂降级 Grep 不阻塞 host loop。须裁决的设计分叉：① 资格判据谁裁、何时判（triage architect 申报 vs 机械判「包内无 firmware 设备侧验收」）；② 与 ARQ-260905 钉死规则的冲突——「专属图索引 launch 面必办、不得降级」是否对 host-only 包开例外、还是改为「真机轮前置」，该规则当年为消歧「worktree 图索引规则」而立，翻转它需 human 权衡回归风险；③ 后置点的锚定（真机轮入口验一条「环境就绪」预检，防 builder 半途需要固件构建时才发现没准备）。

**风险**：riskTier: medium——改治理机制面（launch 面纪律），影响所有后续 Pod 的环境就绪语义，翻转一条 human 曾钉死的规则（ARQ-260905）；无契约面触确（`.claude/**` 均不属 docs/contracts）。**review profile：full-adversarial + human（Track A）**——Track B 三要件中 ①执行测出（yes，本条证据段）、③爆炸半径已执行揭示（yes，实测时间线）齐，但 ②「修复明确无真设计分叉」不成立：资格判据、ARQ-260905 例外的取舍、后置锚点三处均需 human 拍板，按「存疑向严」如实标 full，不走 Track B 快道。owner = architect（起草候选）→ human 裁定 → review-loop 落地。

---

**结项（2026-10-15）**：治理批 261015-gov-review-lite-fast-path 随批闭合——「环境后置」快路径落 T2（SKILL §3b：池优先、新树免做唯一件 = 固件环境等价化后置真机轮、启动卡真机轮预检行）；①资格判据 = 复用 rigor 不新立字段（change-triage §7 判注义务 + YAGNI 复议触发器）；②后置锚 = final 相位机械牙 + 启动卡预检行双锚；③ARQ-260905 图索引规则零修订（OQ-3 裁 (c)，必办性与阻塞性均不变）。本条「图索引十几分钟级」叙事与 SKILL 步骤 5「快照导入分钟级」矛盾按「疑似快照流漂移、留根观察」呈现。凭据：PROPOSAL FROZEN v0.3（8d130a6）+ R1 GREEN + landing-verdict PASS。
