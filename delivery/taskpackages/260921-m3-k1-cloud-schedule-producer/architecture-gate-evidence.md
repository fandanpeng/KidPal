# Architecture Gate Evidence · 260921-m3-k1-cloud-schedule-producer

> Gate: M3 课表周网格形状 + 首页 upcoming 承载（契约变更，随 K1 包同 PR）
> Date: 2026-09-21 ｜ Status: HUMAN_APPROVED
> Packet: [CONTRACT-CHANGE-PACKET.md](CONTRACT-CHANGE-PACKET.md)（§5 Human Ruling = 2026-09-21 human 契约门五问裁定）

## 1. Gate Scope

- `docs/contracts/interface-contract.md` §1.3②（schedule view data 形状重定义：旧列表 → 周网格）+ §1.3⑥（home view 增可选 data 承载 upcoming）。
- 连带改写面（Q2 裁甲案则随本 diff）：§1.5 网关职责第 3 条、§1.6 system prompt 约束模板、§2.3 DeepSeek 解析行。
- Out of scope：B1 课表 MD 结构（表体零变更）、§三提醒推送语义、§四端云协议剖面、决策 26 三触发行为策略（权威 = arch §5.2 场景① + §10，不进契约）。

## 2. Inputs Reviewed

CONTRACT-CHANGE-PACKET.md v2（PRECHECKED 版）；docs/requirements.md（v1.19 修订行、S-F1.1/S-F1.2/S-F1.3 补充规则与 AC、§8 已砍清单、§6 阶段三）；docs/architecture.md（§5.1.2、§5.2 场景①、§10 决策 26）；消费面实文（server/gateway/schedule.py、display_schema.py、firmware/main/display/home_p1.h、schedule_page.c、server/deploy/openclaw-system-prompt.v5.md 课表 producer 段）；architect-reviewer 预审报告（2026-09-21 主线程会话）。

## 3. Architect-Reviewer Classification

| 触点 | 分类 | 依据 |
|---|---|---|
| ① E1 §1.3② schedule view 形状重定义 | SPEC_CONTRACT_DRIFT | requirements v1.19（human 定案）S-F1.2 周网格 + §8 已砍列表视图——旧契约 `{title,items}` 列表形为已砍语义，契约滞后于已批 spec，改契约向 spec 看齐 = 漂移对齐 |
| ② E1 §1.3⑥ home 增可选 data | CONTRACT_CHANGE_REQUIRED | 契约无句子与 spec 冲突、但无法表达已批设计（S-F1.1「接下来」由云端算好下发 + 决策 26）——非冲突是缺口 |

两触点均命中升级判据①（触契约面 → 直走契约门），human 签字义务不因 spec 已批而豁免。

## 4. Human / Architecture Decision

- Architect-reviewer 预审：2026-09-21，PRECHECKED（分类见 §3；漏问点 a–f 已并入 packet v2 Human Question 五问）。
- Human 契约门：**HUMAN_APPROVED**（2026-09-21，human 裁五问，裁定全文 = packet §5）——Q1 周网格扁平形状 + day 0 基映射 + 备注不进网格；Q2 甲案（薄载荷 front 展开）+ §1.5/§1.6/§2.3 连带改写一并批；Q3 home data additive；Q4 upcoming 由 front 机械算（OpenClaw 专注话术）；Q5 过渡口径收紧。

## 5. Applied Diff Inventory

（契约 diff 落地时回填——逐文件：interface-contract.md 触点小节、schedule.py 模块头钉行、CONTRACT-DECISION-QUEUE.md 登记行；additive vs breaking 标注。本仓无 batch-level 结构 gatekeeper 脚本（ARQ-260827-contract-mech-gate-gap），whitelist sync 义务不适用、人工 diff 逐字核对替代。）

## 6. Closeout Conditions

- C1：human 契约门对五问裁定（owner = human；status = **done**，2026-09-21，packet §5）。
- C2：契约 diff 随 K1 同 PR 落地 + schedule.py 钉行更新（owner = K1 builder/integrator；status = pending）。
- C3：`validate_display` 三样本回归（新周网格形状 / `data:{}` 薄载荷 / home 带 data 与缺 data 各一）入 evidence/（owner = K1 verifier；status = pending）。
- C4：ADR 追加 + 契约正文变更记录注记 + queue 行登记（owner = K1 integrator；status = pending）。

## 7. Validation Evidence

（随包收口回填：契约 diff 人工逐字核对记录、validate_display 三样本输出、板端旧固件对 home+data 多余键忽略性 host 验证条；端云联调冒烟挂 M3-K3 设备轮 structural deferred。）

## 8. Docs / Queue Sync

（落地时回填：interface-contract.md 变更记录行（先例 §1.3③ total 2026-08-29 行）、docs/ADR.md 追加、CONTRACT-DECISION-QUEUE.md 登记、workstream decision-coverage.json 契约裁定行核对。）

## 9. Rollback / Follow-up

- Rollback = 撤契约 diff + schedule.py 钉行还原（K1 分支 merge 前）；已 merge 则 revert 单包（--no-ff 可回滚先例）。
- Follow-up：prompt v5 课表段影子契约收口归 K3（v6 bump + 「不再产旧形状 JSON」断言）；过渡窗口期口径按 Q5 裁定执行。

## 10. Verdict

- Not closed。K1 不得在 human 契约门 APPROVED 前触 `docs/contracts/**`（launchPreconditions 第 1 条 = 包级 launch 前置）；门后 dev 可按裁定 diff 实现。
