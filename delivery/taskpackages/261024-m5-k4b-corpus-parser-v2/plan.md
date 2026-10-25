---
changeId: 261024-m5-k4b-corpus-parser-v2
title: corpus 解析器 v2——双层识别+归组+内嵌 turn 拆分+created_at 回填+toolResult 路由+报告扩展;rollback 选择性删除
rigor: lite
riskTier: high
sourceType: arch-review
reviewed-by: architect(k4-attribution-fix-review-20261024.md 对抗 review)+ human D1-D5 五项裁决
date: 2026-10-24
gateProfile: ["migration", "security"]
dependsOn: ["261022-m5-k4-history-corpus-backfill"]
mergeOrder: 55
---

# 背景(live 实证,architect 对抗 review 确认)

- K4 重灌后 corpus 批次 1652 条全部归 owner;根因双层前缀 `[kidpal/sessions/<uuid>#L5] User: <内容>` 被当单层解析,首层源引用当 role。
- **主症状不止 role**:源消息 440/444/445 全是 `User:` 开头、内容里内嵌「小伴的话」——只修第二层 role 症状照旧(review F1)。252 个 user 组含内嵌 `User:`/`Assistant:` 标记,246 组带包装标记。
- deriver 已把 agent 话术抽成孩子事实(「owner advised 星星 to return the two yuan to 王一诺」等);521 份 corpus documents 全部在污染窗口内,按 session 整体回滚重灌是唯一干净路径(F8)。

# human 裁决(D1-D5,2026-10-24)

D1=解析层「归组+内嵌拆分」(review 修正案 1);D2=transcript 批次一并回滚、不重灌;D3=created_at 回填文件日期当天 12:00 +08:00;D4=EMBEDDING_MODEL_CONFIG__MAX_BATCH_SIZE=10(仓内 .env.example 同步)+ 允许少量截断超阈升级;D5=重灌排低峰窗口。

# 铁律

- fixer 未 commit 补丁 KEEP 为基底(review §1.5);解析器 v2 从工作树现状续作。
- 真实语料快照(c:/tmp/k4-corpus-review/txt)只读、不入仓(孩子语料隐私红线);验收绑 sha256。
- additive 报告键零破坏冻结顶层键;单层旧格式回落逐字保留。
- 契约面只读;live 运维段(Phase 1.5-4)零 host 断言,Pod 产出命令单,执行归主线程经 human 授权。
- 技术方案权威 = review §2.1 逐条,本 plan 不转抄全文(防双源漂移)。

# demo 证据形态

host runner 输出:Gate A(A1 单测 / A2 acceptance / A3 快照审计逐值等 / A4 拆分抽样)+ 命令单(供 live 轮照着敲)。
