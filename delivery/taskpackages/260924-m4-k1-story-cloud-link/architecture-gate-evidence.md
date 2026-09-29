# Architecture Gate Evidence · CDQ-260924-m4-k1（story_list 语义拆分 + story_text seq + kidpal_story_delete）

> Status: **LANDED**（2026-09-24；landing commit = 常驻分支契约落地批，sha 见 progress.md 落地块）
> 骨架 = `.claude/references/contract-change-evidence.md`（九节 + Status 头）；先例 = 260921-m3-k1 同名文件。

## 1. Gate Scope

- **在 scope**：`docs/contracts/interface-contract.md` 五处——§1.3③ story_list（节标题 + 板子行为句 + total 语义条 + items 窗口条 + 变更记录行）/ §1.3④ story_text（节标题 + 示例 + 行为句 + seq 条 + 变更记录行）/ §1.5 第 3 条（story floor + relay 收紧句）与新增第 6 条（故事面机械职责）/ §1.6 模板 story 段 / §四 `kidpal_story_delete` 新条目；`server/gateway/story.py` 三处 docstring 钉行（模块头 :12-14 / list_stories :181-184 / story_text :212-213，doc 零行为）。
- **明确不在 scope**：§1.4 control 表（NO_CONTRACT_CHANGE，预审裁定）；`kidpal_session_ctx` §四条目（CDQ-260914 已 LANDED，零改动）；arch 面收口（§5.2④/§5.4/§5.1.2/§7 在办①②④销账 = K1 integrator docs-sync，条件 C4）；prompt v6 三处钉行文面收口（= K3 v7，条件 C5；K1 落地批后 v6 产块指令被 Q7 relay 条款闸口事实失效）。

## 2. Inputs Reviewed

CCP v2（`CONTRACT-CHANGE-PACKET.md`，含 §5 十问裁定逐字）；architect-reviewer 四分类预审报告（2026-09-24，NEEDS_REVISION → 修订清单 7 项 + 漏问点 1–7 全并入 v2，主线程磁盘核验记录 = review.md 契约预审节）；门 1 architect 拆包 review（GREEN，F1–F9）；requirements F2 现文（S-F2.1~S-F2.4 / §2.1 记录会话段 / §2.3 矩阵 / §8 已砍清单 v1.47/v1.49/v1.52 行）；arch §5.2 场景④ / §5.4 故事行【链路载体待契约】/ §7 在办②④ / to-be ⑥；ADR-005（device RPC 否决边界）；CDQ-260914-kidpal-session-ctx（判据一致先例）；readiness 磁盘实证（KSTORY_LIST_MAX=16 / stamp_index=counter+1 / list_stories 零调用点 / v6 单指令——progress.md 拆包块 + review.md 核验节）。

## 3. Architect-Reviewer Classification（收编 CCP §0 裁定表）

| 变更面 | 分类 | 依据 |
|---|---|---|
| §1.3③ total 语义拆分 + 行为句 + items 窗口 + 标题术语 | SPEC_CONTRACT_DRIFT（CDQ 行标签 ACCOMPANIES_SPEC_CHANGE 沿 M3 先例） | requirements v1.47/v1.26/v1.27 冻结在前，契约追认 |
| §1.3④ `data.seq` 新增 | CONTRACT_CHANGE_REQUIRED | spec 只钉 N 语义不钉载体（先例 CDQ-260829 total / CDQ-260921 upcoming） |
| §1.3④ 朗读句清理 | SPEC_CONTRACT_DRIFT | v1.52 砍朗读 |
| §四 `kidpal_story_delete` + §1.5-6 + §1.5-3 story 句 + §1.6 story 段 | CONTRACT_CHANGE_REQUIRED（③连带随触） | arch §5.4【链路载体待契约】+ §7 在办②；先例 CDQ-260914 |
| §1.4 control 表 | NO_CONTRACT_CHANGE | 方向语义不符（丙案否决成立） |

## 4. Human / Architecture Decision

**APPROVED**（human 契约门，2026-09-24，AskUserQuestion 四问打包形态）：十问裁定 = Q1 认可 / **Q2 改判丙案**（≠拟答乙案——items 最新 ≤16 条窗口 + total 全量、板端 cap 不抬；知情注记含旧条目设备端可达性收窄）/ Q3 认可 / Q4 认可（降级板端自定）/ Q5 认可（甲案 + 隐式 ack 超时重发）/ Q6 认可（硬删 + 审计 + 边界仅 story MD + 家长通道挂账）/ Q7 认可（relay 收紧 story_* 单源）/ Q8 认可（story floor）/ Q9 认可（§1.6 story 段随 diff）/ Q10 认可（推送触发归 arch、不设 restore）。逐字记录 = CCP §5。同批过目 = 门 1 切法认可（三包拆法）。落地通道 = 逐字 patch human 过目（2026-09-24 会话内全文贴示）→ 批「落」→ `git apply`（CCP 落地通道纪律，CRLF 以工作树实态生成、`git apply --check` 先行绿）。

## 5. Applied Diff Inventory

| 文件 | 面 | 性质 |
|---|---|---|
| `docs/contracts/interface-contract.md` | +16/−8：§1.3③ 标题/行为句/total 条/items 窗口条/变更记录行；§1.3④ 标题/示例 seq/行为句/seq 条/变更记录行；§1.5-3 story floor+relay 句、§1.5-6 新条；§1.6 模板 story 段；§四 delete 条目 | additive = seq、delete 消息、items 窗口条；modifying = total 语义收窄、两行为句、§1.5-3、§1.6 模板；breaking = 无（过渡窗口安全 = CCP §3：旧板端 total 渲染无破坏 / seq 忽略 / delete 无触发面 / story_text_parse 跳过未知键 :707-712） |
| `server/gateway/story.py` | 三处 docstring 钉行（+8/−6） | doc 零行为（pytest 134 passed 实证，§7） |

批级 gatekeeper sync：story_p3 旧冻结验收板/云双侧已 archive（无在世契约扫描齿面）；CDQ-260913 ②③ canary（key_route_a1/screen_unification/test_schedule_reminder 的 spec.md 断言）与本 diff 无涉；`SANCTIONED_CONTRACT_CHANGES` 类白名单机制本仓无在建批（无 batch-level structural gatekeeper 命中）。

## 6. Closeout Conditions

| # | 条件 | owner | status |
|---|---|---|---|
| C1 | Q7 relay 收紧 + Q8 story floor 机械执行面实现（契约条款的执行齿） | K1 builder（M4K1-A2/A3 关联齿） | **done**（2026-09-24 integrator 收口）——证据 = 冻结齿 `test_a1_relay_rejects_engine_side_story_blocks_q7` + `test_a1_story_list_total_must_be_nonneg_int` / `test_a1_story_list_items_three_keys_required` / `test_a1_story_text_seq_must_be_int_when_present`（+ 正样本 / 不过宽守卫 `test_a1_relay_keeps_other_views_unchanged`）+ `evidence/mutation-replay.md` MUT-A1 组（4/4 转红、还原复绿）+ 实现 commit（sha 回填见本件 §6 尾注；实现面 = `server/gateway/display_schema.py` Q8 floor + `server/gateway/front/front_app.py` Q7 relay 收紧） |
| C2 | 板端窗口消费（items ≤16 + 计数恒信 total + seq 盖章消费） | K2（M4K2-A2/A5） | **done**（2026-09-30 K2 integrator 收口代落）——证据 = K2 冻结齿 SPV2-A7（计数双口径 guard：total=23/窗口 16 时计数=23 非 16 + 删除 −1；mutation 组 1/1 转红还原）+ SPV2-A8（seq 盖章消费逐字二值文案 + 缺省降级不带 N；mutation 组 1/1 转红还原）+ SPV2-B3（story_list 重推到达对账冲销消费接线）+ host 全量 128/0 + final verifier fact r4（`delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/verifier-evidence/rounds/r4.json`，ok:true） |
| C3 | 板端 delete 发送原语 + 隐式 ack 超时重发 + live 复证 | K2（发送/重发）+ K3（live，M4K3-A6⑥⑦） | **K2 半 done（2026-09-30 K2 integrator 收口代落）**——发送原语四件套（protocol `SendKidpalStoryDelete` + application 委托 + mcp_server reconcile/resend/idle/debounce 四针眼消费）与隐式 ack 重发决策表（SPV2-A4/A5/B3 + SPV2-C1）host 全绿 + mutation 13/13；**live 复证半 pending 归 K3**（M4K3-A6，设备轮 human 在场目击） |
| C4 | arch §5.2④ 推送触发扩展 + §5.4 载体收口 + §7 在办①②④销账 + §5.1.2 现状行 | K1 integrator docs-sync | **done**（2026-09-29 human spec 批「落」落地：docs/architecture.md 五 hunk 经 node 字节级直写 apply——git apply 本环境对 CRLF 文件不可用如实记录；defer 修订句先置换「现役两时点 + ①恢复归 K3」；落地 commit c735062，副本先验 6+/5- 精确、其余 419 行零触碰；记录见 workstream review.md「human 判断点落定（重拆批）」节） |
| C5 | prompt v7 三处 v6 钉行文面收口 + carrier 翻转 | K3（M4K3-A4） | pending |

> **C1 实现 commit sha 回填（2026-09-24 integrator）**：**`5ac633a`**（本包 integrator 收口 commit，分支 `dev/260915-m1-k1a-key-layer-dispatch`；`sha` 自指不可解故以「实现 commit + 本回填 commit」两步落账，对齐冻结件 REVISION LOG sha 链「据实分记」先例）。
> C1 证据链 = 实现面（`server/gateway/display_schema.py` Q8 story floor + `server/gateway/front/front_app.py` Q7 relay 收紧，随 `server/gateway/story.py` 与本包单测同批提交）+ 冻结齿（A1 组 8 齿全绿，sha256 `5d4ebbf2…c625a287`）+ `evidence/mutation-replay.md` MUT-A1 组（4/4 转红、还原复绿 `182 passed, 4 deselected`）+ integrator 全量复跑（`182 passed, 4 deselected in 4.15s`）。

## 7. Validation Evidence

- `git apply --check` → 绿（落地前）；`git apply --verbose` → 两文件 cleanly（+24/−14 与 patch --stat 自证一致）。
- validate_display 四样本 → `evidence/validate-display-samples.md`（S1–S3 正样本 ok / S4 负样本白名单拒绝 reason 逐字；如实注记 Q8 floor 归 C1 未实现）。
- `python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **134 passed / 4 deselected / 0 failed**（钉行零行为回归证据）。
- 人工 diff 逐字核对（pass-oracle 第 4 关无脚本 gatekeeper，ARQ-260827-contract-mech-gate-gap 在册降级，如实记录）——核对清单：CCP §1 钉行表四处（story.py :12-14/:181-184/:212-213 已落 ✓；v6 :75-76/:153-155/:271-273 不回改、归 C5 ✓）+ Q2/Q5/Q7/Q8/Q9 裁定落文处逐一对 §5 裁定 ✓ + 变更记录行两处 ✓。

## 8. Docs / Queue Sync

CDQ 三行 append（CDQ-260924-m4-k1-storylist-semantics / -storytext-seq / -story-delete，均 LANDED）；ADR-007 append（seq 账本 + 16 条窗口丙案 + 删除单向消息/隐式 ack，含 Q2 改判备选记录）；契约正文变更记录行 ×2（diff 内）；requirements/arch 零改（arch 收口归 C4、AC3 清扫归 K2 docs-sync）；workstream 面 = progress.md 落地块 + decision-coverage LP1 翻 verified。

## 9. Rollback / Follow-up

- **Rollback**：`git revert <landing-commit>` 单批全还原（契约 diff + story.py 钉行 + CDQ 三行 + ADR-007 + evidence 同 commit）；board 三包 queued 态与拆包制品不受影响（独立 commit）。
- **Follow-up deferred**：家长侧删除通道（Q6 挂账，产品面归 requirements 非契约项）；旧条目设备端可达面扩展（Q2 注记，如需要 = 新需求另立、触 device RPC 边界再裁）；Q8 floor 的脏样本负齿扩样（C1 落地后 K1 acceptance 复跑，evidence 快照不回改）。
