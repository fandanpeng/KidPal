# input-brief · builder · 260924-perf-k1-roundtrip-latency

> 消费者:builder(实现 + 当晚 ECS 执行)。语义权威 = 本包 `plan.md`(FROZEN v1.0)「阶段划分·builder 当晚序列」+ `taskpackage.json` exitCriteria[];执行序列权威 = `architect-review-260924.md` §6(八步)。环境纪律 = skills:`kidpal-ecs-gateway`(config trap / 读改重启)、`kidpal-live-smoke-debug`(三源日志)、`kidpal-serial-monitor`(晨间轮用,当晚不碰设备)。

## 当晚执行序列(八步,逐步留痕 evidence/ append-only)

1. **回滚锚冻结**:主机快照(free -m / pressure 告警计数)+ `openclaw sessions list` 计数 + `openclaw backup create`(SQLite 快照)+ `/root/.openclaw/openclaw.json` 时间戳备份。
2. **探针脚本落仓**(server/ 下,acceptance-author 已冻 hash 断言齿):messages=[system(v6 仓内文件逐字), user(题)]、stream、N≥3、中位数、计时、主机快照、session 计数;**先跑当晚基线重测**(现配置,对照锚;跨夜数字禁比)。
3. **P0**:`systemctl restart openclaw-gateway` → **五项核验**(cron kidpal-reminder-plan 在册 / qqbot 5min 内重连 / 18789 loopback / char-stroke dist marker / 引擎→18789 可达)→ 探针复测(归因第二段)。
4. **P1 工件落仓**:模型声明 artifact + 幂等 apply 脚本(drift-guard:jq 锚点核对现状,不中 exit 3 拒绝执行;时间戳备份;一步回滚脚本)——形态照抄 `server/deploy/openclaw-patch/apply-openclaw-patch.sh` 纪律。
5. **B 案 timebox ≤30min**:查 OpenClaw models 配置能力 / 一次带备份的 catalog 试验,证实 v4-flash 可关 thinking 则 A/B 都测;到时未证实 → 放弃 B 直接 A(不烧窗口)。
6. **A 案 apply + 探针批 + A2 全量判定**:allowlist 增补 deepseek-chat + primary 改指(经步骤 4 脚本,不手改);display 块出现率+合法率 **100%**、课表备注读回、天气真实检索数据、≤3 句;**触发回滚线(任一批 <100%)→ 当晚回滚留数据**(contingency,plan.md 已预写,不现场决策)。
7. **条件步**:slow SQLite 重启后 30min 内再现/频繁 → `sessions archive/cleanup`(步骤 1 快照在先)+ 复测(归因第三段分开记);preflight 触发条件(A1 保底红 / 重启后 preflight>2.5s / slow SQLite 再现)任一 → 仅测量仪表化(三段时戳 + workspace 注入清单,DREAMS.md 是否进上下文),**不改配置**。
8. **收尾**:`docker restart xiaozhi-esp32-server`(晨间真机轮干净起点)+ 探针批次新建 session archive(backup 在先)+ 证据整理 append + 晨间决策包素材成文(交 integrator)。

## 铁律(plan.md 铁律约束节全文适用,重点摘录)

- 不改 `docs/contracts/**`;**不改引擎 `.config.yaml` LLM 段**;prompt v6 零触碰;不动 qqbot 配置;不升 OpenClaw;不加 agent。
- 凭证不入档(脚本内从 ECS 侧配置取、shell 变量传递、不打印);evidence append-only 不覆盖;每个 ECS 变更动作前先备份、后核验、留痕(命令+时间戳+结果)。
- 时延判定用冻结阈值(plan.md 验收口径),不现场调门;数字对不上门 → 如实记录走 contingency/晨判,不美化。
- 结构化日志带 correlationId(K6 纪律,探针批次号即关联键)。
- 完成交回 planner 由 verifier 判定,不自评通过。
