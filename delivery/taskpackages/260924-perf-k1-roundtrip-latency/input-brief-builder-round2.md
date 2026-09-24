# 派发书 · builder · 260924-perf-k1-roundtrip-latency（第 2 版）

> 消费者：builder（实现五件冻结工件 + 执行 ECS 序列，B 案优先）。
> **语义权威** = `plan.md`（FROZEN v1.0）「阶段划分·builder 当晚序列」+ `taskpackage.json` exitCriteria[]；**A/B 主次以 `launch-prep-record.md`「human 五项业务裁定」B1 行为准**（B 案 thinking-off 优先，A 案三候选降 fallback；plan 冻结文本不回改）。
> **冻结契约权威** = `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py` 头注「**FROZEN OFFLINE CONTRACT**」节（第 29~108 行）——五件工件的路径、CLI、语义序、返回键全在那里逐字钉死，**照它实现，不要自由发挥**。
> 执行序列权威 = `architect-review-260924.md` §6（八步）；环境纪律 = skills `kidpal-ecs-gateway`（config trap：引擎 live 配置在 `deploy/xiaozhi-docker-data/.config.yaml`，别读诱饵）、`kidpal-live-smoke-debug`（三源日志）。**设备本轮不碰**（A3 真机轮 = human 持板）。

## 起点：八步序列的第 1~3 步已完成

主线程昨夜已在 dev-loop 外代执行并落证据（归属见 `launch-prep-record.md`「P0 主线程代执行注记」；证据 = `evidence/latency-baseline-260924.md` §7）：

- 步①回滚锚冻结：主机快照 + `openclaw sessions list` 计数 + `openclaw backup create`（`/root/2026-09-24T08-03-14.525+08-00-openclaw-backup.tar.gz`）+ `/root/.openclaw/openclaw.json.bak-260924-preP0`；
- 步②当晚基线重测：§7.2（带 v6，N=3 中位）；
- 步③P0 重启 + 五项核验全绿 + 探针复测：§7.3/§7.4。

**你从步④接续**（P1 声明工件落仓），依序做到步⑧。

## 你的作业面

### A. 实现五件冻结工件（这是把 18 条设计态红齿转绿的唯一路径）

落位目录 = `server/deploy/perf-k1/`，逐字实现冻结契约的五件：`openclaw-model-declaration.json`（JSON，`case ∈ {B,A}`）/ `apply_model_declaration.py` / `rollback_model_declaration.py` / `probe_gate.py`（纯判定器，离线可 import、导入期零网络）/ `probe_batch.py`（live 执行器，须含 `--check-prompt` 离线模式）。语义序、CLI 参数、返回键、阈值常量、`display_schema.validate_display` 真门用法，全按头注。

**不许改验收齿**（`server/acceptance/**` 在你的 deny 面）；实现与齿冲突 → 停下 fail-loud 上报 planner。

### B. B 案优先的 ECS 序列（步④~步⑧）

- **步④** 声明工件 + 幂等 apply + drift-guard（锚点不中 → **exit 3**）+ 时间戳备份 + 一步回滚，落仓；
- **步⑤** B 案 timebox **≤30min**：先用 `/think:off` 内联指令跑探针验质量与延迟（不改配置的最小验证），满意则声明工件落 `thinkingDefault: off|low` 并经 apply 脚本应用，**不手改**；到时未证实 → 放弃 B 直接走 A 案 fallback 批（弹药见 §8.3，A 案候选三选：deepseek-chat / deepseek-flash / v4-flash+low，先以 `openclaw models list --json` 查 `reasoning` 标志）；
- **步⑥** A 案 apply（若走到）+ 探针批 + **A2 全量判定**（display 块出现率与 schema 合法率 **100%**、课表备注读回、天气含真实检索数据、语音 ≤3 句）；**触发回滚线（任一批 <100%）→ 当场回滚留数据**（contingency 已预写，不现场决策）；
- **步⑦** 条件步：slow SQLite 重启后 30min 内再现/频繁 → `sessions archive/cleanup`（**backup 快照在先**）+ 复测（归因第三段分开记）；preflight 触发条件（A1 保底红 / 重启后 preflight>2.5s / slow SQLite 再现）任一 → **仅测量仪表化**（session create / context assemble / first POST 三段时戳 + workspace 注入清单，含 DREAMS.md 是否进上下文），**不改任何配置**；
- **步⑧** `docker restart xiaozhi-esp32-server`（给晨间 A3 干净起点）+ 探针批次新建 session archive（backup 在先）+ 证据整理。
- **改档/换模后首批探针缓存打穿属预期**，N≥3 中位数**从第二批起算**。

### C. 留痕机制（重要：你的机器写边界不含 `delivery/taskpackages/**`）

`enforce-scope` 机械拦截：你的 allow = `firmware/**` + `server/**` + `tools/**` + `delivery/workstreams/**`，**写不进 `delivery/taskpackages/**`**（验收齿那棒刚被同一条拦过）。故：

1. **原始运行证据**（探针批日志、主机快照、命令回执）写仓外暂存 `C:/tmp/perf-k1-run/`（照主线程 `/tmp/kidpal_probe.py` 先例，不进仓）；
2. **探针批结构化 JSON**（schema 同 `evaluate_batch` 的 batch，即冻结契约要求的 `probe-batches/<segment>.json`）也先落 `C:/tmp/perf-k1-run/probe-batches/`；
3. **回报时把「要落进仓的件」逐字交我**——我会代为落进 `delivery/taskpackages/260924-perf-k1-roundtrip-latency/evidence/` 与 `probe-batches/`，并 append 到 `evidence/latency-baseline-260924.md`。**不要为此绕边界**（上一棒未绕行是对的）。

### D. 铁律（plan「铁律约束」节全文适用，重点）

- 不改 `docs/contracts/**`；**不改引擎 `.config.yaml` 的 LLM 段**（base_url `/v1`、`model_name openclaw/default`、流式禁 `max_tokens`——契约 §四/V9 全保持）；prompt v6 零回改；不动 qqbot 配置；不升 OpenClaw；不加 agent、不做结构隔离。
- **动 `openclaw.json` 前必备份**；清理类动作必先 `openclaw backup`；每个 ECS 变更动作「先备份、后核验、留痕（命令 + 时间戳 + 结果）」；**凭证不入档**（从 ECS 侧配置取，不打印、不落仓）。
- 数字对不上门 → 如实记录走 contingency / 晨判，**不美化**；contingency 预写：A 案质量失败且 B 案不可行 → 回滚保 v4-flash 现状、只留 P0 收益、火力转 k2。
- 结构化日志带 correlationId（K6 纪律，探针批次号即关联键）。
- 时延判定用冻结阈值，不现场调门。

## 完成判据（交回 planner，由 verifier 判定，**不自评通过**）

1. 五件工件落仓且 `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/` **18 红全部转绿**（含 5 条 must-stay-green 保持绿），实跑数字回报；
2. 步④~⑧ 逐步留痕（命令 + 时间戳 + 结果），三段归因探针批（基线重测 / P0 后 / P1 后）各 N≥3 中位数 + 主机快照 + session 计数，**分开记不混算**；
3. 五项核验在改配置后的复跑记录（提醒 cron / qqbot 重连 / 18789 loopback / char-stroke marker / 引擎→18789）；
4. 待落仓件逐字交回（探针批 JSON + evidence 段），并说明哪些是实测、哪些未做及原因。

## 回报格式

简体中文，逐条：① 改动文件（含路径）；② 每步 ECS 动作 + 时间戳 + 结果；③ 探针批数字表（三题 × 三段，中位数 + 样本 + display 块命中）；④ 两闸 + pytest 实跑数字；⑤ 待 planner 落仓件；⑥ 需 planner/human 裁决的歧义（不自裁）。