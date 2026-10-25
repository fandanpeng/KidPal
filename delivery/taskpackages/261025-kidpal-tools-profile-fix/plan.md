---
changeId: 261025-kidpal-tools-profile-fix
title: tools.profile full 落库——per-agent profile 覆盖全局 coding + 声明/apply/drift 面换代
rigor: lite
riskTier: high
sourceType: human-ad-hoc
planningImpact: attach-existing
gateProfile: ["security"]
date: 2026-10-25
mergeOrder: 57
dependsOn: ["261024-memory-recall-tool-enable"]
---

# 261025-kidpal-tools-profile-fix · plan（Lite，自建单切片）

## 背景（provenance，消费侧实证）

P0 批（`261024-memory-recall-tool-enable`，已合 main）把 `honcho_search_conclusions` 加进
`agents.entries.kidpal.tools.allow`（四项 → 五项），但 live 验证发现**加名不等于可达**：
`systemPromptReport.tools.entries` 仅 `[read, write, apply_patch, exec, web_search]`，
插件工具不在递给模型的工具集内。

根因（OpenClaw 工具过滤链，文档 `docs/tools/multi-agent-sandbox-tools.md:410` 明文）：
`profile → provider profile → global policy → provider policy → agent policy →
agent policy(provider) → sandbox → subagent` 逐层收窄。全局 `tools.profile: "coding"`
第一道即排除插件工具（`group:plugins` 不在 coding 内），之后 `tools.allow` 只能在
**已剩下的集**里挑——加不回插件工具名。

修复（live 已验证）= per-agent `agents.entries.kidpal.tools.profile: "full"` 覆盖全局
profile（agent profile 覆盖全局 profile 有文档依据 `:230`），allow 仍收窄到恰五项。
live 已应用并实测生效（`toolNames: ['honcho_search_conclusions']`、assistantTurns=2、
答出 9-30 事实「王一诺」）。

**技术权威 = 本 provenance（ECS live 实证，主线程亲核）；无 upstream PROPOSAL。**
前账 = `delivery/taskpackages/261024-memory-recall-tool-enable/taskpackage.json` notes ⑦
（跨包陈旧基线记账，本包即其 follow-up Lite 包）。

问题：仓内工件仍锚「四项 + 只改 allow」旧口径，且 `261022-openclaw-honcho-plugin`
声明 drift_guard 仍钉四项——live 已升级态与仓内基线脱节；下台新机/重建按仓内工件
apply 会漏 `tools.profile` 键，插件工具不可达的缺口会复发。

## 目标态（把 live 已验证形态落进仓）

1. `server/deploy/261022-openclaw-kidpal-tools/kidpal-agent-tools-declaration.json`
   —— 写入面扩为两个叶键：`agents.entries.kidpal.tools.profile = "full"` +
   `agents.entries.kidpal.tools.allow = 恰五项`（`deny` 不进写入面：live 升级态
   `deny: []` 由幂等分支天然覆盖；deny 非空 = 基线集以外 → fail-closed 判 drift）。
   drift_guard 升级基准换代：fresh-machine 态（profile 缺席 + allow 缺席/旧四项）与
   live 升级态（profile full + allow 五项）都接受；「deny 非空 / allow 项数不符 /
   已应用但 profile 非 full / 父键在场但非对象」一律判 drift。`_note` 显式换代留痕。
2. `server/deploy/261022-openclaw-kidpal-tools/apply_agent_tools.py` —— apply 模式
   同时落两叶键；`--gen-patch` 产物展平叶键**恰两个**（tools.profile / tools.allow）
   且值正确；头注去掉「只改 tools.allow」口径；幂等语义不变（对已升级 live
   = {profile: "full", allow: 五项, deny: []} 跑 apply 必须 SKIP 零写零备份）。
3. `server/deploy/261022-openclaw-kidpal-tools/rollback_agent_tools.py` —— 核验
   profile 键处理：现为**备份文件整档还原**机制（读 backup 内容原样写回 config），
   profile 键自动覆盖，语义与键数无关。**按实际机制如实申报，不改**（零 diff）。
4. `server/deploy/manifest.yaml` —— 三条 kidpal-tools note 陈旧字面换代：
   「恰四项 read/write/exec/web_search」→「profile full + 恰五项（含
   honcho_search_conclusions）」；「只改 agents.entries.kidpal.tools.allow」→
   「只改 agents.entries.kidpal.tools.{profile,allow}」。附 honcho-plugin 段
   「drift 四项」字面同步换代（见下 5）。md5 随文件内容自然更新（deploy.sh 以仓内为准）。
5. `server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json`
   + `apply_honcho_plugin.py:16` —— 声明 drift_guard 现状锚 `expect` 四项 → **五项**
   （新 P1 已应用态）；头注「现状锚 = P1 已应用态（tools.allow 四项在位）」换代。
   目标 = 对当前 live 跑 apply 得 SKIP（幂等分支先于 drift-guard），不是 exit 3；
   新机上 manifest 顺序先跑 tools applier（五项）再跑本插件 apply，drift 通过。
   **连带义务**：`server/acceptance/test_261022_memory_isolation.acceptance.py`
   的 K3-E6 冻结锚逐字钉四项（`K3_P1_TOOLS_EXPECT` 常量 + `_fixture_p1_applied`
   组合态），须**显式留痕换代**（五项目标态），同批入 changeManifest。
6. `server/acceptance/test_261022_openclaw_kidpal_tools.acceptance.py` —— 锚换代 +
   新增消费侧断言：①声明含 profile=full 且 allow 恰五项；②`--gen-patch` 产物展平
   叶键恰两个且值正确；③drift 判定四态（fresh 机 / 旧四项 live / 新五项+profile live
   各得接受；deny 非空 / allow 项数不符 / 已应用但 profile 非 full / 父键畸形各判 drift）；
   ④**防回归锚**：「tools.allow 加名 ≠ 工具可达——OpenClaw 过滤链 profile 先行，
   profile=full 是 honcho 工具可达的必要条件」（独立 case 或显式注释锚，把本次教训
   钉进验收件）；⑤既有 16 case 全绿或如实换代。**K1-E4 锚显式换代留痕。**

## 技术方案（planner+architect 预评）

> 2026-10-25 · architect Step 1g 前置技术预评档（只读 advisory；效力 = 不豁免任何门、不改验收与
> TaskPackage 事实、不构成通过判据）。以下为 planner 采纳后的落地方案；分歧点与纠错逐条留痕。

**T1 声明写入面**（D1 采纳）：`change:{path,allow}` → `changes:[{path,value}]` 双叶键，
顺序固定 profile 在前、allow 在后；`value` 深拷贝后写入；deny 不进写入面。apply 的
头注与报错文案同步（「声明 changes 缺 path/value」）。

**T2 drift_guard 语义**（D2 采纳 + 三处纠错）：

- 三条叶路径 AND：`tools.deny` expect_any `["unset", []]`、`tools.profile` expect_any
  `["unset","full"]`、`tools.allow` expect_any `["unset",[四项],[五项]]`。
- **纠错 ①**：profile **缺席 → 接受**（apply 补 full）；只有 profile **在场且值 ≠ "full"**
  （如 "coding"）才判 drift。原因 = 「allow 五项 + profile 缺席」正是 P0 已应用态，
  是本包最典型的升级起点；拒绝它等于拒绝自己的升级路径。
- **纠错 ②**：「allow 五项 + profile 缺席」须显式列入接受集（叶设计天然如此，但验收与
  `_note` 必须写明，不靠「碰巧接受」）。
- **纠错 ③**：叶路径下「tools 对象内未知同级键」被容忍（旧 object 形态「多键即拒」
  语义消失）——这是**放宽**，须在声明 `_note` 与验收头注留痕。另：allow 比较是
  **列表顺序敏感**相等，顺序不同 = drift（fail-closed，人对着 live 实读核对）。
- 父键畸形：`_json_get` 拆出 `_MALFORMED` 哨兵（父键在场但非对象 ≠ 键缺席），叶判定
  fail-closed 判 drift；哨兵不参与任何 `==` 相等判。

**T3 幂等预检定向收紧**（D3 修正，planner 采纳 architect 唯一实质分歧）：SKIP 条件从
「target == config」升级为「target == config **且** drift 基线无问题」。否则
`profile:full + allow 五项 + deny 非空` 会裸 SKIP，绕过「deny 非空判 drift」红线。
对 live（deny 空）行为不变（仍 SKIP 零写零备份）；两叶达标但 deny 非空 → exit 3。
与姊妹脚本 honcho 的「SKIP 先于 drift」有意不同，头注与验收须写清，防后来者当不一致修掉。

**T4 gen-patch**（D4 采纳）：产物展平叶键恰两个（tools.profile / tools.allow），值正确、
零 config 读写、config 不存在也可生成。

**T5 honcho 声明 drift 锚换代 = 方案 A**（D5 裁决）：`expect` 四项 → **五项**，不改
`apply_honcho_plugin.py` 的 `drift_problems` 代码（仅头注换代）。理由：顺序纪律（四项机器上
K3 apply 必须 exit 3，逼先跑 tools applier）、fresh machine 上 A/B 无差、改动面最小、回滚零变化。
live 上换代不可观测（插件 4 键已在目标态 → 幂等 SKIP 先于 drift），如实申报为离线证书。

**T6 冻结件换代义务**（D6 + R1/R2/R3/R9）：

- K1-E4（kidpal tools 冻结件）：`_declared_allow` 读 `changes`；schema 断言改双叶键；
  `_target_state` 补 profile；gen-patch 期望两叶键；`drift_guard_accepts_unset_and_legacy_four`
  **必须主动改写**（旧 helper 只看 allow 两候选，新声明下会「绿着但不覆盖 profile/deny
  语义」，是最危险形态）；「只改一键」断言升级为「pre 与终态展平叶差集恰 = {profile:"full",
  allow:五项}」。
- K3-E6（memory_isolation 冻结件）：`K3_P1_TOOLS_EXPECT` 常量与 `_fixture_p1_applied`
  组合态**同批改**（fixture 多用例共用，一处改四处验）；头注 FROZEN CONTRACT §B1 与
  docstring 显式留痕；`test_k3_e6_apply_drift_guard_exit3` 保两条变异并**追加第三条**
  「allow 仍四项 → exit 3」把顺序红线钉进机械判。
- K1-E4-C 防回归锚三件：①声明 changes 含 `tools.profile="full"`；②profile 在场且 ≠ "full"
  → exit 3（行为真跑）；③根因注释锚逐字「过滤链 profile 先行，allow 只能在剩余集里挑——
  加名 ≠ 可达」；另保留「allow 五 + profile 缺席可升级」用例作纠错 ① 的机械证据。

**T7 live 面收窄**（R5/R6）：部署轮只需 `--gen-patch` 产物对 live 做一次
`openclaw config patch --dry-run --json` 证 schema 接受（核 `tools` 子树未丢 deny），
零写零重启；「P1 applier 先、honcho applier 后」写进部署轮命令单（manifest 顺序只是提示）。

## 红线

- 不碰 `docs/contracts/**`；不改 `workspace-kidpal/AGENTS.md` 内容（本包零 AGENTS.md diff）；
- 不动 Honcho 栈与 backfill 数据；不做数据迁移；不碰 `server/gateway/**`；
- 不 push、不合并到 main、不碰 ECS（live 已是目标态，本包只落仓）；
- ECS 侧 probe 会话污染（diag-tool-probe*）不在本包内。

## 预算

- iterationBudget 2 / fixBudget 3 / supervisorBlockBudget 2。

## demo 证据形态

host：验收换代后全量套件绿（基线 = 本包冻结前实跑值）+ gen-patch 产物形状断言
（展平两叶键）+ drift 四态真跑；live：**零 host 断言**——`openclaw config patch
--dry-run` 接受产物 / `openclaw.json` 实升级态 / 真机「王一诺是谁」问答归 human
部署轮（本包只需在仓内把 live 已验证形态固化）。

## 锚兼容红线（既有冻结件）

- `test_261022_memory_isolation.acceptance.py` K3-E6：honcho 声明 drift 锚换代须
  显式留痕，`test_k3_e6_apply_drift_guard_exit3` 两条真漂移变异（改白名单 / 键缺席）
  语义保持——新基线仍不含 `unset`/`bash,browser`，故仍 exit 3；
- `test_261022_openclaw_kidpal_tools.acceptance.py` K1-E4 其余（apply 真跑四态 /
  备份 / 只改声明键 / 回滚 / 零入仓 / manifest）零语义回退，仅随目标态同步期望值；
- `test_261022_honcho_memory.acceptance.py` / `test_261017_openclaw_kidpal_skills.acceptance.py`
  同运行态文件组合面零回退。
