# pod-retro · 261014-engine-stop-truncation-asr-filter（第一人称热记忆，planner 收尾相位 2026-10-14）

> 本文件只记第一人称过程体感；tokens / gates / rounds / verdict / test results 一概以 machine artifacts（verifier-evidence/rounds、pod-events.jsonl、dispatch-completed）为权威，此处不复述、不冠权威。

## 顺滑面

- 这是跑得最「直」的一类包：reviewed plan（architect /triage-change 产出）把三处改动、齿清单、harness 先例全部钉死，acceptance-author → builder 两跳零返工，红基线到绿一条直线。前置 plan 质量（改动点逐字、边界显式零触碰面）是主因。
- acceptance-author 的「双向可满足性实证」（模拟实现 7/7 转绿 + 去豁免变异恰 ET-b 独红）超出我的派单要求，直接把「齿是否真能抓住毁稿回归」的疑虑在冻结期消掉了——这个动作值得成为验收冻结的默认期望。

## 卡点 / 体感

- 两轮 scoped 红都是同一族「元数据伴生字段」缺口（mutationSkipReason、docsSyncReason）——我在 Draft 时把理由写进了 notes 自由文本，但闸读专字段。下次起包 Draft 阶段应对照闸脚本把 `*-Required=false` 类字段的伴生 reason 字段一次配齐，而不是等 fail-loud 教。属起包格式前置的盲区，不是流程问题。
- final 相位轮序踩了一次空转：r4 缺 --scope-file 举手（fail-closed 正确），我误以为 ordinal 被消耗、下一轮报了 5 被闸拒。final 轮次编号语义（失败未落 fact 不占序）下次直接记住。
- integrator 对 scope-file 的建议名（verifier-scope-final.json）与实际解析格式（行文本 .txt）做了正确偏离并留痕——派单里我凭想象给了 .json 名，若 integrator 不去读 verify.mjs 源码就会撞墙。派单给「产物契约」时应引用消费方的真实解析格式或让执行者以消费方为准。

## 别扭 / 摩擦

- brief ① 锚点表未回灌 acceptanceTestsFile[]（builder 回执发现的 tooling advisory）：验收件是硬前置却不在封闭清单里，靠派单 prompt 补位。建议 brief-inputs.mjs 从 acceptanceTestsFile 回灌 ① 表（登记不扩门）。
- launch-card.md 缺失（launch-argument 四址之一）——本包以 plan.md frontmatter 承载启动信息未受损，但 launch-card 缺位时「启动卡该写什么」没有骨架可循，每个包各写各的。

## 人工纠偏 / 升级

- 本 Pod 零 agent-escalation。human 触点① 在包前已确认（pod-events human-decision ef45a8ab）。真机五项复验与 ECS 手工部署归主线程/human，Pod 内无从体感。

## 下次改进

1. Draft TaskPackage 时静态自查 `mutationSkipReason` / `docsSyncReason`（凡 *Required=false 必带 reason 专字段）。
2. final 轮次语义：preflight 拒轮不占 ordinal，重跑同号。
3. 派 integrator 时 scope-file 契约改为「按 verify.mjs readScopeList 实际格式」而非自拟文件名。
