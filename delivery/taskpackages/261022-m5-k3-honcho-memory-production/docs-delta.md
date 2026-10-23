# Docs Delta — 261022-m5-k3-honcho-memory-production

- **docsSyncRequired**: false（TaskPackage `docsSyncReason` 在案：架构面 §1.1/§1.2/§3.1/§4/§5.1.2/§5.3.2/§5.3.3/§5.4/§6.7/§8.1 与 NFR-PRIV-1 / NFR-SEC-2 ⑤ 已由 261022 两面 landing（requirements v1.67 + arch FROZEN v1.1）落稿；本包只落实现面）
- **docsOwner**: integrator
- **spec 面漂移检出**: none——本包实现与 requirements v1.67 / arch FROZEN v1.1 / 契约（interface-contract.md §四零 diff，contract-lint 绿）无冲突；无需交 architect。

## 声明性注记（非 spec 变更，仅记账）

1. **K3-E7 live 观测项挂 human 部署轮**（structural、零 host 自动断言，`awaiting-human:live-deploy`）：跨会话回忆对照 / seeded 负向探针集 live 100% 零命中 / marker 探针 DB+日志双证 / 2C-4G prep 税与内存读数复测回填 `docs/debt/tech/TECH-261022-openclaw-prep-tax-memory-pressure.md`。Pod 不自行上 ECS，live 轮由 human 部署时执行。
2. **arch §6.7 抽取策略「实测成形后回填」**：若 live 实测后需改 spec（deriver 提示可配置性等），走 `/arch-change` 逃生门，不在 Pod 内自裁（integrator 对 `docs/requirements.md` / `docs/architecture.md` 无写权）。
3. **部署轮注意点**（human 部署时消费，均已在工件内自描述）：
   - Honcho 栈铺放：`/opt/honcho/.env` 先备好真实凭据（模板 `server/deploy/honcho/honcho.env.example`；apply 脚本缺 .env 拒跑），再跑 `apply-honcho-stack.sh`（幂等收敛）；`healthcheck-honcho.sh` 与 `verify-deploy.sh` honcho 段同口径。
   - openclaw.json 插件 patch：`apply_honcho_plugin.py` drift_guard 现状锚 = P1 tools.allow 已应用态（`["read","write","exec","web_search"]`），**必须先于本包 patch 应用 P1 patch**（顺序错 → drift_guard exit 3 拦截，属预期保护）；openclaw.json 整文件含 gateway 凭据、不入仓。
   - 删除/备份语义提醒：备份 = 不备份靠重建（OQ-10，重建源 = session-corpus 源语料 + OpenClaw 会话存储）；删除 = 运维面手工、范围 = 画像与 observation/消息存储（adminer 18102 本机台为查看面）。
   - 出网红线核验入口：每日 drift timer 消费 `verify-deploy.sh`（18100 非 loopback 监听 / 五容器健康门不满足均 DRIFT exit 1）。
