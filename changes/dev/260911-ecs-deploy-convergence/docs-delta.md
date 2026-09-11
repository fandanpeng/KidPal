# docs-delta — 260911-ecs-deploy-convergence（批次① ECS 部署收敛 · 漂移修复 + 提醒 cron §8 部署 + live 五齿 + 证据回收）

> 性质：证据型部署收敛包（subsystem=server，rigor=full，riskTier=high，gateProfile=live-deploy/evidence-recovery，
> `docsSyncRequired=true`，`docsOwner=integrator`，`deliveryImpact=milestone-affecting`，mergeOrder 16）。
> 核心结论：**本包动的是部署载体面的文档/制品，canonical 面（`docs/contracts/**`、`docs/spec.md`）
> 零改动**；docs-sync 闸 r5 绿已由 six docRefs 覆盖指针 + acceptance 全 id 满足。本文件按交办把
> 实际落地的文档/部署制品变更面逐条列出，并**诚实标注 repo 侧已 commit 与 ECS 侧已同步的对应关系**
> ——部署窗口（2026-09-11 10:33–12:10，human 在场）里的 live 侧同步（engine-patch / gateway / schedule.md /
> cron / .config.yaml）**不产生本 repo 的 diff**，只产生 ECS 生产态变更，故不在 `git diff` 里可见；
> 这里显式登记，避免「diff 缺席」被误读为「没做」。
>
> **docsSyncRequired 判定（integrator）**：`docsSyncRequired=true` 已由覆盖指针满足，无需回写 canonical
> 文档本体。六个 docRefs 是全仓既有文件，本包对它们的履职是「读引用 + 部署承载」，不是「改写」。
> 详见 §A。

## A. docRefs 现状与无需回写理由

| docRef | 现状 | 本包履职 | 无需回写理由 |
|---|---|---|---|
| `changes/dev/260829-hardware-validation/review-packet.md`（#F3） | 承重 review packet（在上游 main 已并，本包 `origin.upstreamRef` 同源） | 只读消费 F3（ECS shim 停服 + 8080 安全洞未回填）作为 #13 证据回收的源头 | 上游审计件，本包不写；本包产物落自身 `changes/dev/260911-ecs-deploy-convergence/`，不晋升他人目录 |
| `changes/dev/260829-hardware-validation/PROGRESS-SUMMARY-260910.md#§四C / §七` | 上游包 9b 分期方案（本包来源锚） | 溯源引用 | 上游规划件，零回写 |
| `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`（§8 覆盖指针） | 本包 stage 1 **已 commit**：§8 头补 4 行「覆盖指针」，明写 §8.2-8.4 执行由本包 ECS-A4/ECS-A5 承载 | repository 侧承载真跑职责 | 已完成（含于 stage 1 commit `28e4319`，§B 第 4 行） |
| `server/deploy/provider.config.yaml` | 本包 stage 1 **已 commit**：`providers.asr.provider` → `qwen3_asr_flash` + `api_key_env: DASHSCOPE_API_KEY` + 终选指针注释（原 ASR 声明面与 live 漂移） | 声明面对齐 live 终态（Qwen3ASRFlash） | 已完成（§B 第 2 行） |
| `.claude/taskpackages/260829-hardware-validation/device-acceptance-checklist.md`（表 C） | 上游包 L1-L5 判绿判红标准载体 | 只读引用表 C 作为 live 五齿判据 | 上游冻结件，本包不写 |
| `server/deploy/README.md` | 本包 stage 1 **已 commit**：ASR 凭据行由「阿里云云 ASR（`ALIYUN_ASR_APPKEY`/`ALIYUN_ASR_TOKEN`）」改「云端 ASR（`DASHSCOPE_API_KEY`，终选 Qwen3ASRFlash）」 | S1.3 顺检 README 不再述旧 provider 值 | 已完成（§B 第 3 行） |

## B. 本包实际落地的文档/部署制品变更面（repo 侧 ↔ ECS 侧对应）

| # | 制品 | 变更面 | 承载侧 | 对应 commit / 凭据 |
|---|---|---|---|---|
| 1 | `server/deploy/apply-xiaozhi-overlay.sh` | 漂移全量对齐 D1-D6：ASR→Qwen3ASRFlash（`DASHSCOPE_API_KEY`）、Intent→nointent、base_url 补 `/v1`、TTS type→edge、补渲 `server.ip:127.0.0.1/port:8010`、**D5 引擎 auth 二次纠偏回 `enabled:false`**（HMAC 动态签名与决策22 静态 token 不相容，理由注释钉死）+ ALIYUN_ASR_APPKEY/TOKEN 淘汰移除 | **repo 侧已 commit**（stage 1 + D5 纠偏） | `28e4319`（D1-D6，auth 当时声明 true）+ `6f110c0`（D5 二次纠偏回 false，human 裁 pod-events `f9322b9c`） |
| 2 | `server/deploy/provider.config.yaml` | `providers.asr` 声明对齐 Qwen3ASRFlash 终选（provider=`qwen3_asr_flash`、`api_key_env=DASHSCOPE_API_KEY`、`model_name=qwen3-asr-flash`）+ 决策指针；GWA-A4 齿命中面（aliyun/dashscope/qwen3）保留 | **repo 侧已 commit** | `28e4319` |
| 3 | `server/deploy/README.md` | ASR 凭据行对齐终选（去 ALIYUN_ASR_APPKEY/TOKEN，改 DASHSCOPE_API_KEY）；凭据纪律/grep 零泄漏段落原样保留 | **repo 侧已 commit** | `28e4319` |
| 4 | `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` | §8 头补 4 行「覆盖指针」（§8.2-8.4 执行归本包 ECS-A4/ECS-A5 承载；live 五齿证据落 `changes/dev/260829-schedule-reminder/`） | **repo 侧已 commit** | `28e4319` |
| 5 | `server/deploy/engine-patch/connection.py` + `kidpal_display_strip.py`（两件） | 引擎补丁同步到 ECS（ECS 旧版为 260907 不含 `_kidpal_try_speak`，本窗推送 repo 现版 + `.bak-260911` 备份 + `docker restart xiaozhi-esp32-server`），修复提醒语音处理器缺失 | **ECS 侧已同步**（repo 侧零新 diff——两文件已在先前包 commit） | human 授权 pod-events `2ed56680`；live-findings note finding #2（`_kidpal_try_speak` await 报错，真 bug 归后续修复） |
| 6 | gateway 代码（`reminders.py` / `front_app.py` / `mcp_relay.py` 等提醒投递面） | ECS 侧部署（apply 铺提醒排程脚本 + cron 载荷就位 + front.env 注入），live 四 provider 全对 + prompt v4 在场 + 一轮语音端到端 0 断链 | **ECS 侧已同步**（repo 侧零新 diff——gateway 提醒面已在 260829-schedule-reminder 包 commit） | live-findings note ECS-A3 绿 |
| 7 | `schedule.md` 学校课程表区块（ECS `/root/.openclaw/workspace/kidpal/schedule.md`） | 写入学校课程表 CSV 转置 + 每周固定课表 + 一次性条目「2026-09-11 11:45-11:55 部署核验测试提醒@客厅」（测试条目事后可删）；写入前备份 `schedule.md.bak-260911-l3l4` | **ECS 侧已同步**（生产课表文件，不在 repo；repo 侧 `gateway/schedule.py` 消费面零改动） | human 授权 pod-events `6e7d3417`；支持 L3/L4 临期档位实测 |
| 8 | live 五齿证据 | 归位 `changes/dev/260829-schedule-reminder/live-five-teeth-evidence-260911.md`（L1 cron / L2 pending.json 对照契约 §三 / L3 L4 占位 / L5 语义 fallback）+ L1 `state.lastRunStatus=ok` 回填 | **repo 侧已 commit**（本文件 §C 归位，integrator 相位） | 本相位归位动作，见 review-packet §Verification Evidence |

## C. 明确没动的面（及为什么）

### C-1 `docs/contracts/**` 与 `docs/spec.md` 零触碰

- 本分支整个 diff（含 stage 1 `28e4319` + `6f110c0`）未触 `docs/**` 任何文件；`contract-lint` 闸 r5 `ok:true`
  （闸对象 = `docs/contracts/interface-contract.md`）。
- 本包对契约的消费仅两处**只读**：① `docs/contracts/interface-contract.md#§三`（L2 pending.json 载荷形状逐字段
  对照，decisionCoverage `contract-§三-alerts-pending形状` 条目 `landsIn: acceptance:ECS-A5`）；② §三 是 live
  五齿 L2 对照的判据源，不改契约文字。
- D5 引擎 auth 纠偏（`enabled:false`）不构成 spec 漂移：决策22 静态共享 token 的强制点在 **front 层**（不变），
  本纠偏只是把引擎层 HMAC 动态签名（`core/auth.py` AuthManager）与静态 token 的不相容事实固化进 overlay 注释——
  安全边界 = engine 腿 loopback 127.0.0.1:8010 + front 层 token 强制（260903 架构实况），`docs/spec.md` 一字未动。

### C-2 冻结面 git 零触碰

- `*.acceptance.*` 冻结验收件、`firmware/**` 冻结固件面、`docs/contracts/**`、`managed_components/**` 本窗口
  git 零触碰（ECS-E3 exitCriteria 满足；live-findings note「回滚就绪」节已逐字登记）。

### C-3 本相位收口件（本文件同目录）

- `changes/dev/260911-ecs-deploy-convergence/{docs-delta.md, review-packet.md}`（closeout blocking 两件）。
- `verdict.json` / `verification.json` 为投影·advisory（non-bearing，脚本 fs-write）；本包机器产线已运行
  （`verify.mjs` round 证据完整），不晋升（见 review-packet）。

## D. 机器可核据（integrator 本相位实跑）

```
git diff --name-only 6364a89..HEAD -- docs/                    → 空输出（canonical docs 零触碰）
git show --stat 28e4319                                        → repo 侧 17 文件（4 件部署文档 + taskpackage/ledger/evidence）
git show --stat 6f110c0                                        → 5 文件（overlay D5 纠偏 + taskpackage/ledger）
node .claude/scripts/closeout-check.mjs --update-index         → 见 review-packet §Closeout（本相位实跑结果）
cd server && python -m pytest acceptance/ -q --import-mode=importlib
    → 全档验收套复跑结果见 review-packet §Verification Evidence（留红如案逐条登记）
```