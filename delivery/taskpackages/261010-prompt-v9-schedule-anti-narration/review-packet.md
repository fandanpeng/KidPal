# Review Packet · 261010-prompt-v9-schedule-anti-narration

> author: integrator · date: 2026-10-11 · rigor: lite · riskTier: medium
> TaskPackage: `delivery/taskpackages/261010-prompt-v9-schedule-anti-narration/taskpackage.json`
> 阶段：已 verifier-green（scoped round 1 全绿），本 packet 供 supervisor 合并门复核；合并判裁归 supervisor 闸，本 packet 不自判通过。

## Change Size

- Files changed: 11（变更面 7 = changeManifest 全集：`server/deploy/openclaw-system-prompt.v9.md` 新建、`OPENCLAW_SYSTEM_PROMPT_CARRIER`、`provider.config.yaml`、`apply-xiaozhi-overlay.sh`、`server/acceptance/test_prompt_v7_round_routing.acceptance.py` additive 补齿、`server/deploy/README.md` 指针节、docs-sync 面 `docs/architecture.md`〔被 scope 钩子拦，handoff architect，见 docs-delta〕；治理面 4 = taskpackage / current 投影 / input-brief / verifier-evidence）。
- Net diff: server 面 +295/−27（其中验收齿 +266、v9 prompt 本体 +1 语义行、三面接线与 README 指针 +28/−26）；契约与需求面零行。
- Scope: 单面 server/deploy prompt 族版本化 bump（课表问答回合行补对称反叙述纪律）+ 四面接线 + docs-sync；固件零改动（firmware-build 闸绿实证零回归）。
- Split recommendation: 不拆——单语义（v8 漏修的课表行对称条款）驱动的原子 bump，拆开会出现接线与本体不同步的中间态，违反「三面接线一致」不变式。

## Verification Evidence

- Structural gates: verifier scoped round 1 全绿（`verifier-evidence/rounds/r1.json`，ok:true，invocationId `5f27af9a-9479-4c05-b574-09dee7fb8dd6`）——taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译 build-pass）/ contract-lint / acceptance-coverage（PN-A1..A5 全映射）/ mutation-check（豁免留痕）/ docs-sync / security 八闸 ok。
- Acceptance: RED 基线 `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance` = **4 failed / 140 passed / 4 deselected / 9 subtests**（4 红全为新增 PN 齿设计态红，逐点核对为真实缺失行为）；修后同命令 = **144 passed / 4 deselected / 9 subtests，0 failed**（既有 140 绿零降 + 4 齿转绿）。final bearing verification（phase=final + scope-file）待 planner 以本包 scope 清单执行：`delivery/changes/dev/261010-prompt-v9-schedule-anti-narration/test-scope-final.txt`。
- Security gate: security 闸绿（securityReviewRequired: false）；prompt 本体为纯文本纪律行，无凭据/注入面变化；overlay 渲染路径 secrets 注入逻辑零改。
- Mutation check: 豁免（TaskPackage mutationSkipReason 留痕）——纯 prompt 文本 bump + 指针接线，无分支/逻辑可 mutation；敏感性由 PN-A1~A4 双向断言承载（正钉 v9 纪律锚 + PN-A3 v8 sha256 零回改负锚）。
- Docs sync: README 指针节核对通过；arch §5.5.5/§5.1 指针刷新被 scope 钩子拦截 → handoff architect（详见 `docs-delta.md`），**此项未落盘前 docs-sync 面留一个 open 尾巴**，交 planner 派 architect 收口。

## Demo 对账

- **offline 齿红→绿全程留痕：已具备**——红基线 4 failed / 140 passed（redBaselineCommand 逐字实跑，4 红逐一核对为真实缺失行为）→ 修后 144 passed / 0 failed，见上 Verification Evidence 与 verify-runs.jsonl。
- **ECS 部署与真机轮（PN-A5 live 观测项）：pending-human**——部署 = v9 + overlay 重跑上 ECS，属 live 面操作，需 human 在场（决策 21 键先例：上传前核远端 == 基线、ecs_put.py exec_command+stdin + MSYS_NO_PATHCONV=1）；真机「明天有什么课」1 轮 + journalctl 与 docker logs 双侧逐句对账留痕，随部署同场执行。本 packet 如实标注为未完成，不预支绿。

## 未覆盖面申报

对照 acceptance[] + 契约条目 + 效果面逐项：

1. **PN-A5（live 观测项）**：无离线验收断言——模型输出风格漂移无法离线确定性断言根治，plan 前置申报为非确定性诚实申报。现状：本包 diff 未上 ECS，真机轮未跑。去向：human 在场位执行部署 + 真机轮 + 双侧日志对账（见 Demo 对账 pending-human 行）。
2. **引擎侧前导文本 strip 兜底**：plan 非目标显式排除（engine-patch 面扩大留账）——prompt 纪律若失效复发，前导文本仍可经 /v1 流式端点进 TTS。现状：未实现、未断言。去向：prompt 纪律失效复发时再评（plan.md 非目标条款）。
3. **其它回合行（查字行）复核**：plan 非目标——查字行已有「语音照常口语讲字义」正面要求兜着，v9 零偏移继承。现状：无新断言。去向：无（既有正面要求在位，PN-A4 接线版语义锚抽检覆盖世系继承面）。
4. **arch spec 指针**：见 Docs sync 行——刷新内容已拟好、写权不在 integrator，去向 architect handoff。
5. 其余面：**none**——契约条目（笔顺 JSON / display 块 / 课表 MD / shim API）零触，display 块规则零变；requirements 面零改。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无 `minimal:` 标记） | — | — | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | hard-violation | docs-sync | arch §5.5.5/§5.1「现状 = vN」指针落后接线两版（现写 v7、实际 v8→本包 v9；v8 批 260924-m4-k3-record-session-live 漏刷） | `docs/architecture.md` §5.5.5 版本化指针纪律（声明面/渲染面散文只留「当前 = vN」指针，指针须与接线一致）；漂移由本包申报收敛，刷新内容已拟好入 docs-delta.md | planner 派 architect 执行 docs-delta「spec 面 handoff」两处刷新，执行后回填 docs-delta 状态表 |
| NIT | judgement-call | acceptance | PN 批齿内嵌 docstring 长注释（头注 ⑥-⑩ 冻结范围复述）超出断言语义所需 | `server/acceptance/test_prompt_v7_round_routing.acceptance.py:25-59` changed hunk + Pruning heuristic（冻结件头注传承自 V7 批既有形态，一致性优先） | 不动（冻结件，改注释须走验收通道，收益低于扰动成本） |

## Five-Axis Review

- **Types（语义一致性）**：v9 = v8 全量 + 课表问答回合行单行扩行（diff 实证仅 :13 一行），「工具/文件动作照做…但语音只播最终答复」与对话行 v7 反叙述句式对称；查字行正面要求原样保留。世系继承由 PN-A4 附带锚抽检（严禁凭印象 / story_list / 问答框）机检。
- **Contracts（契约面）**：零触——prompt 意图摘要（契约 :184）不涉语音纪律，display 块规则零变；`prompt: |` 顶层键注入逻辑零改。
- **Process/state（接线一致性）**：三面接线一致 = v9（CARRIER artifact 行 == provider.config artifact/source/deploy_target 三键 == overlay PROMPT_SRC/PROMPT_DST），PN-A4 版本无关守卫机检；ADR-001 守卫从结构化指向解析、零版本字面断言，forward-bump 韧性保持。
- **Security（安全）**：无凭据、无注入面变化；token/证书/secrets 路径零触碰；prompt 文本反编造纪律增强（「严禁凭印象或记忆回答课表具体内容」保持 + 反叙述条款新增）不弱化既有安全语义。
- **Ops（可运维）**：部署 runbook（README 动作序）对本 bump 仍适用（overlay 重跑 + 重启 openclaw-gateway）；启动标记行等既有机械报警锚零改；远端 == 基线核验前置条件已写入 plan/TaskPackage 部署条目。

## Residual Risk

- Known limitation:
  - 模型侧输出风格漂移（deepseek-v4-flash 把回合表词汇当前导文本念出）只能由 prompt 纪律抑制，无引擎侧结构性 strip 兜底——离线验收无法证明根治，PN-A5 live 观测是唯一效果证据（pending-human）。
  - live 探针版本字面耦合（VRT-A4/A7 钉 v1、CHR-A8/A9 钉 v2，「所测 ≠ 生产所用」）为既有登记项 `ARQ-260908-versioned-guard-version-literal-coupling`，本包不扩改，offline 守卫（VRT-E4 裸名 needle / CHR-E3 / story P3 载体齿）已版本无关化，不受本 bump 影响。
  - 本包验收全为文件内容锚、无 DB/事件断言——若 supervisor 机械闸判 all-mock 触发 riskTier 重算 high → 需 human 放行（TaskPackage notes 前置申报，先例在案）。
- Follow-up recommended: architect handoff 收口 arch 指针（唯一 docs-sync open 尾巴）。
- Human decision needed: ECS 部署 + 真机课表问答 1 轮（PN-A5）——human 在场位，部署时按决策 21 键先例核远端 == 基线；真机轮双侧日志逐句对账留痕。
- `taskpackage.decisionCoverage` 中 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目：**none**（本 TaskPackage 无 decisionCoverage 字段，无风险接受式丢弃决策）。
