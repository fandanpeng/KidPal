---
changeId: 261022-m5-k4-history-corpus-backfill
title: 历史 corpus backfill——session-corpus 全量照记进 Honcho（不滤姓名、不分回合性质），幂等可重跑、前后计数报告、清库回滚
rigor: full
riskTier: high
sourceType: proposal
reviewed-by: architect（workstream PROPOSAL FROZEN v1.0 + arch PROPOSAL FROZEN v1.1；门 1 待 human 派 architect）
date: 2026-10-22
gateProfile: ["migration", "security", "real-db"]
dependsOn: ["261022-m5-k3-honcho-memory-production"]
mergeOrder: 54
---

# 背景摘录（锚「问题与根因（整改型）」，PROPOSAL §2）

- **记忆飞轮在设备形态下不闭环**：OpenClaw 原生记忆 deep 阶段「Ranked N, Promoted 0」（检索信号饿死）、`memory_search` 搜「王一诺」命中「王水」；改接 Honcho 专用画像服务后，新回合可积累，但**历史语料仍在 session-corpus 里，不在画像中**——孩子 9-30 讲过的「王一诺」等事实需一次性补录才有跨会话回忆价值。
- **human 2026-10-22 终拍口径**（R6 / OQ-3）：backfill **照记全量**——不区分回合性质、含第三方姓名（孩子社交事实是记忆目标；语料系孩子主动讲述且家长知情）；保留幂等 / 回滚 / 计数报告三件。
- **AC5 口径**（requirements v1.67 S-F5.1）：历史内容照常记入，旧内容与新聊天一视同仁；设备流程回合不入画像的边界自记忆子系统上线起对新增回合生效，不回溯历史。
- **删除 / 重建口径**（NFR-PRIV-1 / OQ-10）：记忆库不备份，靠重建——重建源 = session-corpus 源语料 + OpenClaw 会话存储；backfill 即重建路径。

# 可行性判据

- 源语料在 ECS 本机、只读可得；Honcho API 已由 P3 生产化并暴露 loopback。
- 幂等判据可在 plan 阶段定形（消息指纹 / session+turn 键 / 去重写入），host 面可离线验证「二次跑零增量」。
- 清库回滚可在测试库或快照窗口演练，不触碰源语料。
- 前置：P3 已合并（peer 映射与画像边界）；P3 的资格门不影响 backfill（历史照记显式豁免）。
- **风险提示（arch PROPOSAL §4 预判）**：若过滤 / 回滚设计未收敛，riskTier 升 critical 并回 planner/architect 重评——本包拆包期按 high 起步，builder 期若发现必须引入 PII 过滤或不可逆数据移动，停、升级。

# 跨包事实与共享冻结面约束（只有 RTE 看全局的部分）

- **peer 映射沿用 P3**：device 级 peer（`user=<device_id>` → `agent:kidpal:openai-user:<device_id>`）；不得为历史语料另造 peer 键或多设备身份。
- **与 P3 资格门的分层**：历史照记 = 显式豁免；新增回合仍走资格门。实现上两条路径不得共用「无 user 也入库」的宽松分支，否则会把一次性回合污染面扩大到新回合。
- **源语料只读**：backfill 不得改写 / 移动 / 清理 session-corpus；回滚只清记忆库侧（画像 / observation / 消息存储），不删 OpenClaw 完整日志、源语料、诊断日志。
- **manifest / verify-deploy 与 P1/P3 串行共享**：本包新增 backfill 资产条目；P1 的 `.config.yaml` 锚集与 P3 的 Honcho 锚不得回退。
- **计数报告 = workstream 收口对账件**（workstream PROPOSAL §5）：P4 的前后计数报告是 M5 收口轮的对账依据，必须落盘可读。
- **非本包**：Honcho 栈部署与插件（P3）、运行时记忆注入（P3）、历史语料的采集（既有 session-corpus）。

# 铁律约束

- 数据迁移类：只增不删源；回滚路径必须先于全量执行验证；禁止在 live 上先跑后补回滚。
- 契约面只读；记忆库形状非契约；不新增端云帧或 display 变化。
- 不引入 PII 过滤器（human 终拍）；不得以「隐私」为由自行缩圈。
- 出网：backfill 只走已记账的 deriver→DeepSeek / embedding→DashScope 两条；不得新增出网路径。
- 本 Pod 只做本包 changeManifest 内文件；发现需要改 spec / 契约 → 停、走逃生门。

# demo 证据形态（与 roadmap 一致）

- host runner 输出：幂等二次跑零增量、计数报告形状、回滚脚本离线契约。
- live 运维：human 在 ECS 执行全量 backfill + 计数报告过目 + 画像抽查命中；清库回滚演练一次。
- 真机轮：随 P5 收口轮抽问历史事实（如「王一诺」相关）验证补录生效，不单独占轮。
