---
changeId: 261022-m5-k2b-persistent-session
title: 持久会话——引擎请求携 user=<device_id>、只发本回合，OpenClaw 服务端持久会话成为对话历史唯一权威；补丁漂移护栏同批
rigor: full
riskTier: high
sourceType: proposal
reviewed-by: architect（workstream PROPOSAL FROZEN v1.0 + arch PROPOSAL FROZEN v1.1；门 1 待 human 派 architect）
date: 2026-10-22
gateProfile: ["evidence-integrity", "test-quality"]
dependsOn: ["261022-m5-k1-prompt-carrier-migration"]
mergeOrder: 52
---

# 背景摘录（锚「现状与差距（新建型）」，PROPOSAL §2）

- **跨会话事实答不出**：openai-http 不带 `user` 字段 → 每请求 `openai:<randomUUID>` 新 session（部署版 bundle `resolveSessionKey` 逐字实证：`const mainKey = user ? `${prefix}-user:${user}` : `${prefix}:${randomUUID()}``）；v11 孩子对话回合禁工具 + active-memory 不覆盖 one-shot → 记忆读路径三重封死。孩子 9-30 讲过的「王一诺」2026-10-22 问起答不出。
- **双历史关系已由架构批定案**（arch PROPOSAL T2 / ADR-015）：对话历史唯一权威 = OpenClaw 服务端持久会话（键 `agent:kidpal:openai-user:<device_id>`）；引擎 RAM `Dialogue` 降为本地非权威副本，只用于本地簿记，**一律不向 OpenClaw 重放**；前端不持对话历史。记忆库（Honcho）不是会话上下文第二真相源。
- **契约已定案**（`docs/contracts/interface-contract.md` §四 :287）：省略 `user` 时回落旧行为（每请求随机 session、调用方自携带历史），向后兼容。
- **补丁漂移同族**：`ARQ-260907-engine-deploy-patch-drift` 的护栏形态（bind-mount + 启动标记行 + manifest md5）即本包先例。

# 可行性判据

- OpenClaw 侧 `user` 能力已在场（部署版 bundle 实读）；引擎侧补丁随本包，无需 OpenClaw 升级。
- host 面可闭环：payload 形状（恰一条新 user message / 无 history / `user=device_id`）可离线断言；两轮追问连续性可用 loopback 探针复验；省略 `user` 的降级路径可负锚验证。
- live 面：human 部署后真机跨 WS 新会话回忆复证；持久会话增长与 auto-compaction 成本只记观察，不在本包定阈值（arch §8.1 风险 14）。
- 前置：P1 已合并（共享 `engine-patch/connection.py` 与部署面）；架构批 §8.2 / 契约 §四 定案已在（commit `0411e957`）。

# 跨包事实与共享冻结面约束（只有 RTE 看全局的部分）

- **`user=<device_id>` 是全列车共享常量**：P2 定义其请求字段形态与合法值；P3 的资格门以「是否携带 `user`」为唯一判据（读写同门）；P4 的 device 级 peer 映射由它派生。三包必须使用同一字面与同一来源（设备 id），不得各造变体或 sentinel user（arch T4 候选乙已否决）。
- **`engine-patch/connection.py` 与 P5 共享**：本包合并后才允许 P3/P4；P5 可先于本包合并（merge order 51 < 52），因此本包 rebase 时必须保留 P5 的动态 context 装配段，不得整体覆盖。
- **`docker-compose.yml` 的 bind-mount 列表**：本包新增 `engine-patch/openai.py` 挂载；下次有引擎面变更时 deploy.sh 只提示 `docker compose up -d`，不自动重建容器（容器重建 = 设备全断）。本包须在 launch-card / 交付说明里写明这一人工生效步。
- **P1 的静态/动态承载不变式不得被本包破坏**：本包只改 payload 的历史与 user 字段，不改 prompt 装配层；两轮探针的请求形状须与 P1 后的动态块形态一致。
- **非本包**：Honcho 插件/画像（P3）、backfill（P4）、课表注入内容（P5）。记忆读路径的「三重封死」中，active-memory 替换归 P3，本包只解 session 键与历史重放两重。

# 铁律约束

- 契约面只读；`user` 是引擎→OpenClaw 的 additive 字段，设备端零感知，不得引入任何端云帧或 display 变化。
- 不得「每轮透传历史 + user」双写：只发本回合是 ADR-015 的硬语义；引擎 RAM 副本不得重放。
- 部署副本补丁必须带启动标记行（docker logs 唯一机械报警锚）；manifest 登记 + verify-deploy md5 同批。
- 不引 sentinel user、不改 Intent/thinking 档（nointent / thinking-off 零触碰）。
- 本 Pod 只做本包 changeManifest 内文件；发现需要改契约或架构 → 停、走逃生门。

# demo 证据形态（与 roadmap 一致）

- host runner 输出：payload 形状断言 + 两轮追问连续性探针（离线/loopback）+ 省略 user 降级负锚。
- 部署后 live：human 持板，新 WS 新会话问此前讲过的事实能答出；引擎日志见补丁启动标记行。
- 真机轮默认随 P5 收口轮一次性做；本包 live 半以部署后探针为主。
