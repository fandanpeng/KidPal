---
id: ARQ-261019-local-memory-extraction-gaps
title: Claude Code 本机 memory 承载的治理知识未全量提取入仓——261019 复分析 8 条，1 条已被吸收、1 条部分吸收、3 条残留缺口（verdict 状态行义务 / 生成即验证 / OpenClaw sqlite 坑）、1 条取舍未决
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下一治理批（触 `.claude/agents/landing-verifier.md` 或 `docs/templates/PROPOSAL.template.md` 或 `kidpal-ecs-gateway` skill 的批次优先消费）
opened: 2026-10-19
closed: 2026-10-19
closedBy: delivery/review/261019-gov-review-local-memory-extraction/landing-verdict.md
paths: [.claude/agents/landing-verifier.md, docs/templates/PROPOSAL.template.md, .claude/skills/kidpal-ecs-gateway/SKILL.md]
source: [delivery/archive/changes/arch/260928-gov-review-cross-face-intake-spine/retro.md（F3 长文 Write 序列化损坏）, delivery/archive/changes/arch/260928-gov-review-cross-face-intake-spine/architect-landing-summary.md（T1 执行事故记录）, 本会话 2026-10-19 对本机 memory 与 repo 承载面的逐条对照实测]
---

**证据**：本机 Claude Code memory（`C:\Users\15802\.claude\projects\c--AI-KidPal\memory\`，8 条，最新一条 2026-09-29，**261015 治理批后无新增**）与本仓承载面逐条对照（2026-10-19 实测）：

1. **已吸收**：ui-visual-change-package-granularity——三判据 + 视觉对齐类切法判例已泛化入 `.claude/references/task-intake-and-slicing.md:197-208`（260928-gov-review-portfolio-boundary-story-propagation T6 承载）。本机 memory 已成冗余副本。
2. **部分吸收（缺口①）**：landing-verdict 状态行——「verdict 行形态义务」已在 `.claude/references/track-b-verifier-brief.md:33` 钉死（260920-gov-review-verdict-anchor-align T1），但 `.claude/agents/landing-verifier.md` 产出规格（:34、:68）仍无此义务——agent def 是派发时实际注入的指令面，brief 的义务进不了 dispatch prompt 即形同虚设（260929 批正是 verifier 写了完整 GREEN 结论但漏 token 行、INDEX 卡 frozen 的实测事故）。
3. **未提取（缺口②）**：write-serialization-corruption-guard——260928 批 retro F3 已定载体 =「登记 ARQ、作自检句③『生成即验证』扩写候选」（retro.md:45），但 debt 账本查无此条目（本条即补登）；`docs/templates/PROPOSAL.template.md` 起草期自检三句（:64-66 一带）仍未含「长文落地分块 + 写后机检自身产物」。
4. **未提取（缺口③）**：openclaw-ecs-reinstall——OpenClaw 凭据 sqlite 迁移死锁两表位置（`auth_profile_store` / `config_machine_state`）、`doctor --fix` 不可依赖、计费锁表 `auth_profile_state` 清理法、systemd unit 手写（`kidpal-ecs-gateway` SKILL.md grep sqlite/auth_profile/systemd/billing/doctor 零命中，2026-10-19 实测）。
5. **取舍未决（不入本条 paths）**：禁「齿」具名化——human 原裁「触碰时顺手改、不单独立项」（260929）；若决意写入 `AGENTS.md` 输出标准具名示例，因触 G1 须另行升 profile，本条不预批。
6. **留本机为正当（3 条）**：aliyun-ecs-ssh-access（含明文密钥，永不入仓；skill 已覆盖 ssh_helper.py 用法）、cc-switch-model-tier-mapping（单机代理环境，不可迁移）、governance-naming-for-resume（个人对外叙事，非工程事实）。

**影响面**：本机 memory 是单机单点——换机器 / 重装 / 新会话（含他机协作者与 headless Pod）即丢，且 memory 不回仓核验会随 repo 演化腐化（条目 1 已成冗余副本即实证：repo 已吸收后 memory 仍自称「定例」）。缺口①②③ 的共性 = 「教训已被执行测出、repo 内部分承载、但生成侧指令面缺一截」：缺口① 致 verdict 缺状态行事故可复发（INDEX 卡 frozen、主线程回派补行多跑一轮）；缺口② 致长文落地序列化损坏带病入库风险存续（260928 批 near-miss 两连）；缺口③ 致下次 OpenClaw 凭据 / 计费锁事故时运维知识不可得（ECS 上 `/tmp/clear_auth_failure.js` 亦属易丢制品）。不修代价 = 三类事故各自按原频率复发 + memory↔repo 漂移持续扩大。

**建议改动**（候选，交 review、不自批）：

- 缺口①：`.claude/agents/landing-verifier.md` 产出规格补一句 verdict 状态行形态义务（行首 `verdict: <TOKEN>`、token 闭集与 `changes-index.mjs` deriveLanded 消费面，与 `track-b-verifier-brief.md:33` 同文同源，两处不得漂移）。
- 缺口②：`docs/templates/PROPOSAL.template.md` 起草期自检块扩第四句（或并入③）：「长文（中文、>40 行）落地分块追加、写后立即机检自身产物（grep 噪音特征 + 行数核对 + 抽读首尾），不凭 Write 返回成功自信」。
- 缺口③：`kidpal-ecs-gateway` SKILL.md 增 OpenClaw 运维节：凭据两处 sqlite 表与迁移死锁绕法、计费锁 `usageStats` 清理、systemd unit 手写要点；密钥一律不入（沿用 skill 既有 Never persist secrets 红线）。
- 条目 1 处置：本批落地后由主线程删除本机冗余 memory（ui-visual-change-package-granularity），MEMORY.md 索引同步——memory 卫生属本机操作、不属仓内 landing。
- 便宜验证计划：落地后 grep 复核三处（`.claude/agents/landing-verifier.md` 含 `verdict:` 状态行义务句；`docs/templates/PROPOSAL.template.md` 自检块含「生成即验证」句；`kidpal-ecs-gateway` SKILL.md 含 sqlite/OpenClaw 运维节）+ 复跑 `debt-doctor.mjs` 零 RED。

**风险**：riskTier: low——三处均为文档面局部增补（agent def 一句 / 模板自检一句 / skill 一节），不动门脚本、不动机械判据、不触契约面；唯一连带注意点 = 缺口① 两处同文义务须防未来漂移（建议以「引一处、不复制」方式落）。review profile: Track B（三要件：① 执行测出——provenance 见 source 与本条证据节实测；② 问题清晰修复明确——三处候选改法均无设计分叉；③ 爆炸半径已揭示——三个文件局部，无连带面）。

**结项**（2026-10-19）：consumed-by: 261019-gov-review-local-memory-extraction——缺口①②③ + 条目 5 取舍四项全落（landing-verdict PASS + 2 ADVISORY，mergedInMain=yes，ff 自 21360279）；条目 1（ui-granularity 已吸收）随批外收尾删本机冗余 memory，条目 6 三条留本机维持。
