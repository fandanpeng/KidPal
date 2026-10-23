# 启动卡 · 261022-m5-k2b-persistent-session

跑 `261022-m5-k2b-persistent-session`（workstream device-memory-overhaul P2，rigor=full，riskTier=high，sourceType=proposal，mergeOrder=52，dependsOn=K1 已合并 ✓；P5 亦已合并部署，无在途冲突）。本树即 Pod 本体就地起 loop（K1/P5 同款）。

- **reviewed plan**：`delivery/taskpackages/261022-m5-k2b-persistent-session/plan.md`（跨包事实节五条全读：`user=<device_id>` 全列车共享常量 / connection.py 保留 P5 动态装配段 / bind-mount 人工生效步 / P1 承载不变式不破坏 / 非目标清单）。
- **上游面**：需求 v1.67 frozen；arch 批 T2/T6 landed（0411e957）；契约 §四 :287 定案（省略 user = 旧行为向后兼容）。launch 前置满足。
- **基线锚**：server acceptance = **278 passed / 4 deselected / 33 subtests**（P5 落地后，2026-10-23 本树实测）；unit 115 passed。本包红基线由 acceptance-author 冻结相位实跑回填（新断言设计态红）。
- **架构权威**：arch review PROPOSAL **T2**（payload 恰一条本回合 user message / 无 history 重放 / `user=device_id`）+ **ADR-015** 双历史单一权威（OpenClaw 持久会话唯一可重放；引擎 RAM Dialogue 降非权威副本不重放；前端不持历史）+ 契约 §四（省略 user 回落旧行为）。
- **纪律红线（冻结判法锚定）**：① 只发本回合是硬语义，不得「每轮透传历史 + user」双写；② 不引 sentinel user、不改 Intent/thinking 档（nointent / thinking-off 零触碰）；③ 补丁漂移护栏三面同批——bind-mount 挂 `engine-patch/openai.py`、connection.py 启动标记行 = 唯一机械报警锚、manifest md5 登记 + verify-deploy 核验；④ P1 后 payload 形状须与动态块装配形态一致（`{{schedule_summary}}` 填充等 P5 接线不回退）。
- **源 ref 核对**：`TECH-261022-device-memory-prompt-overhaul` 与 `docs/debt/tech/ARQ-260907-engine-deploy-patch-drift.md` 均在原路径在场；brief-inputs 对裸 token 形态报「文件不在磁盘」为探针误报（taskpackage.json 内带全路径同件 ref 可解析）。
- **live 停等**：K2-E6 live 观测项（human 部署后真机跨 WS 新会话仍记得此前讲过的事实；持久会话增长与 auto-compaction 成本只记观察、不在本包定阈值）挂 human 部署轮——host 面转绿后 status `awaiting-human:live-deploy`。部署面注意：**新增 bind-mount 后首次生效须 `docker compose up -d` 重建容器（= 设备全断、人工窗口），deploy.sh 不会自动重建**；connection.py 为 engine-face 重启（设备断连重连，K1/P5 同款）。
- **合并门预期**：riskTier=high → supervisor 闸按声明档即 high 判 ESCALATE 停人类合并门为**预期路径**（K1/P5 是 medium 被上浮，本包是真 high）；gateProfile 两项（evidence-integrity / test-quality）。
- **会话标识**：human 在场交互式会话（VSCode 扩展，2026-10-23 K1/P5 同会话续跑）。
