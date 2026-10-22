# landing-summary · 261022-arch-review-device-memory-prompt-overhaul

> **packet-id** = FROZEN PROPOSAL（delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md，FROZEN v1.1）§2 改法集 T1–T7 + §5 验证集。主线程 packet-bound 落地于 2026-10-22；本件为 claimed 清单，独立核验归 landing-verifier。

## manifest 复述（七文件，与 PROPOSAL `landing-manifest` 块逐字一致）

```landing-manifest
docs/architecture.md
docs/contracts/interface-contract.md
docs/decisions/ADR.md
docs/glossary.md
server/deploy/manifest.yaml
server/deploy/openclaw-agents/README.md
server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md
```

## claimed 清单（逐文件）

**docs/architecture.md**（T1–T5 + T7 §12）：
- T1：§1.1 外部服务行（+Honcho 侧车 / +DashScope embedding / DeepSeek deriver 同域）；§1.2 DeepSeek 行 deriver 同域注记 + 新增 DashScope embedding 行 + Honcho 自托管栈行；§1.3 追加记忆身份体系范围声明条；§3.1 出网四条→五条；§4 NFR-SEC-2 行改五条 + 新增 NFR-PRIV-1 对策行；§5.1.2 新增记忆子系统行 + OpenClaw 本体行重写（对话历史 = 服务端持久会话 / 记忆画像经插件 / 静态语义承载 = AGENTS.md）；§5.3.2 新增记忆子系统行；§5.3.3 端口探针追加 18100 核验句；§5.4 追加记忆·会话消息行 + 记忆·画像行；§8.1 新增风险 13；§10 决策 19 追加出网路径⑤投影。
- T2：§2.2 总图整块替换（user=<device_id> + 持久会话 + 记忆侧车）；§5.2② 承载短语替换；§5.4 对话历史行拆两行（服务端持久会话权威 / 引擎本地副本不重放）；§8.1 风险 9 闭账句（含方案二失效句）+ **新增风险 14 行**（持久会话历史增长与 auto-compaction 成本观察项）；§8.2 开放问题 1 取代注记（整条）；§10 决策 21 尾追加部分取代注记。
- T3：§5.5.5 整节重写（静态主承载 = workspace AGENTS.md 缓存边界之上 / 动态 context 模板 / ADR-001 重定义两面 / OQ-8 裁定否决 v12 双件不新建 prompts/ 目录）。
- T4：新增 §6.7 记忆写入资格与画像（资格门 + 六行完整枚举 + 三层纵深 + backfill 照记全量 + 删除语义 + 降级）；§5.1.2 front 行尾追加（一次性回合不携 user + 课表摘要注入）。
- T5：§5.2② 段末追加动态 context 装配段（课表摘要 front 单源派生、引擎不解析）；§5.1.2 引擎补丁行尾追加动态 context 装配句；§5.1.3 依赖方向句替换（含课表摘要注入 + user 标识）。
- T7-3：§12 变更记录首行 `261022-device-memory-prompt-overhaul`。

**docs/contracts/interface-contract.md**（T6 契约包随批原子落）：
- §四「会话连续性承载」整句替换为定案记载（user 标识 + 服务端持久会话 + 只发新回合；省略 user 回落旧行为；原「部署期首验」与两条降级路径随定案撤销）；
- §四 变更记录段追加 2026-10-22 条；
- §1.6 引用块后追加承载对账注记（处 A 非必需项，随批顺手落）。

**docs/decisions/ADR.md**（T7-1）：追加 ADR-014（记忆架构，五件格式，含 amend ADR-001 声明）+ ADR-015（会话模型，五件格式）。

**docs/glossary.md**（T7-2）：「架构与系统」节 observation 后追加三词条——记忆子系统 / 持久会话 / 动态 context 块。

**server/deploy/manifest.yaml**（T3-4）：workspace-kidpal AGENTS.md 条目 note 值反转（次要承载 → 静态语义单一事实源）。

**server/deploy/openclaw-agents/README.md**（T3-3）：:20-22 三行替换为「护栏与静态语义承载」反转口径（kidpal 侧 AGENTS.md 主承载 / main 侧不经引擎注入）。

**server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md**（T3-2）：:13-15「## 冲突以注入 prompt 为准」小节整节替换为「## 静态语义单一事实源」小节（H2 标题形态落盘——**landing-summary 点名项**，R1 三处提请采纳第 3 条）；正文 = 主承载 + 动态 context 块分工、冲突以本文件为准。

## 时序约束（T3-5，转 P1 brief）

仓内镜像已改、live 未铺（P1 deploy 执行）；中间窗口 `verify-deploy.sh` 报 AGENTS.md md5 漂移 = 预期已知窗口；P1 落地前不得单独铺放该文件（护栏真空）。

## 已知边界

- 需求面 / 实现面 / `.claude/**` 零 diff（git diff --name-only 实证：diff 集合 = manifest 七文件 + `.codebase-memory/` 机器再生漂移〔纪律豁免不入库〕）。
- debt 条目收口（TECH-261022 / TECH-261018 consumed-by / prep 税复测回填）= 主线程登记动作，非本批 diff。
