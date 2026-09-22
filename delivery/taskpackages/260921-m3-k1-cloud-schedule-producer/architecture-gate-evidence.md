# Architecture Gate Evidence · 260921-m3-k1-cloud-schedule-producer

> Gate: M3 课表周网格形状 + 首页 upcoming 承载（契约变更，随 K1 包同 PR）
> Date: 2026-09-21 ｜ Status: LANDED（2026-09-21 契约 diff + 随包收口同批落地）
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

落地方式 = human 选乙通道（AskUserQuestion 2026-09-21「授权我 git apply」）——主线程起草逐字 patch（`C:/tmp/contract-grid.patch`，4 hunk）→ human 过目确认 → `git apply` 一次性落盘（对契约冻结红线的显式一次性豁免，全程留痕本节）。

| 文件 | 触点 | 性质 | 内容 |
|---|---|---|---|
| `docs/contracts/interface-contract.md` §1.3② | schedule view 整段重写 | **breaking（形状重定义）** | 旧 `{title,items}` 列表形即废 → 周网格 `{week:{courses:[{day,slot,name,flags}]}}`（day 0 基 周一=0、flags bit0=※/bit1=星标、备注不进载荷）+ 意图薄载荷 `data:{}` front 截获展开 + 「今天」列板端本地推导 + 行为句（V27）+ 变更记录行 |
| `docs/contracts/interface-contract.md` §1.3⑥ | home view 增可选 `data.upcoming` | **additive（缺 data 行为不变）** | day_key/items/preview 对齐 `home_p1.h` 冻结 seam；producer = front 机械算（Q4）；推送时机不进契约注记 + 变更记录行 |
| `docs/contracts/interface-contract.md` §1.5 第 3 条 | 连带改写（Q2/Q5） | 语义增写 | schedule 薄载荷截获展开 + 旧形状拒绝降级 + schedule.py 单源/零自研红线注记 |
| `docs/contracts/interface-contract.md` §1.6 | 连带改写（Q2） | 模板增写 | system prompt 课表段：话术答复、不产课表 JSON、薄载荷 |
| `docs/contracts/interface-contract.md` §2.3 | 连带改写（Q2） | 行改写 | 「DeepSeek 解析」→「DeepSeek/OpenClaw 课表职责」：不再产 schedule JSON |
| `server/gateway/schedule.py` 模块头 | 契约钉行 | doc 注释（零行为） | 旧钉行「{title,items} 零增减、不进 UI 语义态」→ 新形状钉行 + flags 承载取代注记（packet Impact 预告的同 PR 义务） |

- 落地 diff 净量：+35/−10（git diff --stat，29 行实质变更），逐字 patch 全文见落地会话 2026-09-21（hunk 级展示已过目）。
- 本仓无 batch-level 结构 gatekeeper 脚本（ARQ-260827-contract-mech-gate-gap），whitelist sync 义务不适用；替代核验见 §7。
- 注：本表 6 项由主线程在 human 乙通道授权下一次性落盘（schedule.py 钉行原 C2 owner = K1 builder——提前至本批随同 PR 契约面落地，human 全范围收口授权在案；K1 builder 后续实现以「增量、既有函数零偏移」为界，不再触碰本钉行以外的契约耦合面）。

## 6. Closeout Conditions

- C1：human 契约门对五问裁定（owner = human；status = **done**，2026-09-21，packet §5）。
- C2：契约 diff 随 K1 同 PR 落地 + schedule.py 钉行更新（owner = K1 builder/integrator；status = **done** 2026-09-21——human 乙通道授权下主线程提前落地，见 §5 注记；K1 分支未开实现前契约面已在基线）。
- C3：`validate_display` 三样本回归（新周网格形状 / `data:{}` 薄载荷 / home 带 data 与缺 data 各一）入 evidence/（owner = K1 verifier；status = **done** 2026-09-21 主线程预跑，ALL-SAMPLES-GREEN——另附 sample-4 旧列表形状现状透传注记；K1 verifier 复跑或引用本件均可）。
- C4：ADR 追加 + 契约正文变更记录注记 + queue 行登记（owner = K1 integrator；status = **done** 2026-09-21——ADR-004、契约 §1.3②/§1.3⑥ 变更记录行、CDQ-260921-m3-k1-schedule-grid）。

## 7. Validation Evidence

- **逐字 diff 核对（人工降级，ARQ-260827-contract-mech-gate-gap 在册如实记）**：patch 4 hunk 与 packet §1 触点一一对应（§1.3②/§1.3⑥/§1.5+§1.6/§2.3），hunk 计数与触点计数相等，无范围外变更；apply 后 `git diff -- docs/contracts/` = +35/−10。
- **validate_display 三样本回归**：ALL-SAMPLES-GREEN，原始输出 = [evidence/validate_display-three-samples-20260921.txt](evidence/validate_display-three-samples-20260921.txt)（含 sample-3b 缺 data 向后兼容与 sample-4 旧形状现状透传注记；形状收紧 = front to-be，归 K1 实现）。
- **server 面 zero-new-red**：`python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **47 passed / 0 failed**（2026-09-21，schedule.py 钉行 doc 改动后实跑；与 progress.md 2026-09-21 基线锚 47 passed 一致）。
- 板端旧固件对 home+data 多余键忽略性 host 验证：挂 K1 dev-loop 冻齿（结构 deferred，随 A2–A4 批补）。

## 8. Docs / Queue Sync

- `docs/contracts/interface-contract.md`：§1.3② / §1.3⑥ 变更记录行各一（先例 §1.3③ total 2026-08-29 行同形态）；§1.5/§1.6/§2.3 连带改写已并入正文。
- `docs/ADR.md`：ADR-004 追加（producer 归属 + 形状重定义 durable semantics）。
- `docs/CONTRACT-DECISION-QUEUE.md`：CDQ-260921-m3-k1-schedule-grid 行登记（状态 LANDED）。
- `server/gateway/schedule.py`：模块头契约钉行更新（doc 注释，行为零改动）。
- spec 面（requirements.md / architecture.md）：零改动——spec v1.19 口径已冻结在前，本 diff 是契约向 spec 看齐；workstream decision-coverage 契约裁定行核对归 K1 integrator 收口时核。

## 9. Rollback / Follow-up

- Rollback = 撤契约 diff + schedule.py 钉行还原（K1 分支 merge 前，`git revert` 单 commit 或还原 working tree 均可——本批尚未 commit 前即 working tree 可弃）；已 merge 则 revert 单包（--no-ff 可回滚先例）。
- Follow-up：① prompt v5 课表段影子契约收口归 K3（v6 bump + 「不再产旧形状 JSON」断言）；② Q5 校验收紧的 front 侧实现（旧形状拒绝+降级）归 K1 A4 实现齿；③ 过渡窗口期（契约已落、front 未收紧、K3 未 bump）内 OpenClaw 仍产旧形状 JSON 会透传至 K2 重写后的板端 → V27 降级兜底，K1/K3 merge order 先于板端切换消费即可消除（mergeOrder 33 < 34 < 35 已保证）。

## 10. Verdict

- **LANDED（not CLOSED）**：契约 diff + 随包收口义务已落地、验证证据在案（C1–C4 全 done）；实现面（A2–A4 齿、front 收紧、prompt v6）归 K1/K3 dev-loop，dev 可按裁定 diff 实现。CLOSED 待 K1 integrator 收口核对本 ledger + queue 行翻 CLOSED·已结。
