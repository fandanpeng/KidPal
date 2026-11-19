# docs-delta —— 261117-session-end-intent-longretry

> integrator 收口制品。docsSyncRequired = **false**。
> 本文件记录本包对 docs 面（spec / 契约 / 架构 / ADR / debt）的结论与依据，供 closeout 与 supervisor 复核。

## 1. 判定

`docsSyncRequired` = **false**（本包 `taskpackage.json` 已声明，且与实测一致）。

## 2. 依据

- **契约面已随上游架构批 FROZEN 并 landed**：`docs/contracts/interface-contract.md` 的 §1.4（display 统一响应块「截获转发类」end_session）、§1.5（control 分流）、§1.6（告别回合纪律）由 workstream `261115-session-channel-resident` 的架构批（reviewedCommit `87dbf775`）冻结落地。本包 = 纯实现兑现 + prompt 静态承载对账，不改 spec 语义。
- **spec 面（`docs/requirements.md` / `docs/architecture.md`）语义零改动**：`requirements §2.1` 结束通道②③、`S-F1.4` 离线期必达、`architecture §5.2`（三通道同落点）/ §6.4（封顶长跑 + 人工意向位）均为本包已冻结的实现目标，非本包新增/改动。
- **实测零 docs diff**（`git diff 05417c1f -- docs/` = 空）：本包提交前 `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**` / `docs/decisions/**` / `docs/debt/**` 全部零 diff（debt 刷新见 §3，提交后为回填的**非语义**注记）。

## 3. debt 账本刷新（非 spec 语义改动）

按包级派单授权（integrator 持 `docs/debt/**` 写槽），对两条整改型缺陷锚做 **trigger / 收口注记刷新（append-only，不改 status）**，**不结项**（结项凭据须真机轮 B，归主线程）：

- `docs/debt/tech/TECH-261014-session-end-channels-unwired.md`：追加收口注记——实现已随本包落地（设备 control `case 0` 收口统一处理器 + 告别 prompt 承载）；剩余触发 = 真机轮 B B1 验收（说「拜拜」全链通）。
- `docs/debt/tech/TECH-261014-autoconnect-gives-up-silently.md`：追加收口注记——实现已随本包落地（{5,10,20,40,60} 封顶长跑 + 稳态 60s + 人工意向位）；剩余触发 = 真机轮 B B4/B5/B8 验收（拔网线自愈 / 重启 front 自愈 / 长跑期人工入口不僵死）。

两条目 `status` 维持 `deferred`，不归档、不结项。

## 4. 跨包 / 跨面交回（不在本包处理）

- **`docs/requirements.md:48` ⚠️ 对账注记刷新**：workstream roadmap §④ 明示为**跨包事项**，由主线程在列车收口时路由（或需求面对账小批），非本包 spec 语义改动。本包 docs 面零 diff，不代跑。
- **`OPENCLAW_SYSTEM_PROMPT_CARRIER` / `provider.config.yaml` / `openclaw-system-prompt.v11.md`**：实测三件均零 diff（`git diff 05417c1f` 空），ADR-001 三面相等不变式保绿；prompt 静态承载唯一改动落在 `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`（契约 §1.6 兑现），部署到 ECS 由 agent 按 `kidpal-ecs-gateway` 规程执行（真机轮 B 前）。

## 5. ADR

`adrRequired` = **false**（本包无新增架构裁决；全部实现落在既有 ADR-019 ⑥ / ④ 与 arch §5.2 / §6.4 包络内）。