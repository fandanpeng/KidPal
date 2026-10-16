# 架构 Review 场景库（arch-scenarios）

> challenger/scenario-author 的「拿什么逼问」素材:ATAM 质量属性场景 + 主线场景 + pre-mortem 目录。scenario-author 按 spec 类型挑场景冻进 charter,challenger 逐个走。
> ATAM(CMU/SEI)本身就是**技术架构评估法**;本库把它 + pre-mortem 落到本仓库的主线与失败类。

## 一、ATAM 质量属性效用树（utility tree 模板）

对被审 spec,先建效用树:根=「架构够好」,分支=质量属性,叶=可证伪的具体场景(刺激→环境→预期响应),每叶标 (重要度, 难度)。逐叶找 **敏感点 / 权衡点 / 风险点**。

| 质量属性 | 示例场景(刺激 → 预期响应) |
|---|---|
| 性能 / 吞吐 | 一次同步涌入 N 封邮件 → 入站不阻塞主线、背压可控 |
| 可改性 | 新增一个 email provider(QQ/163) → 只动适配层,领域模型不感知 |
| 可用性 / 失败 | 下游系统短暂不可用 → 事件重试/幂等,不丢不重复推进 |
| 一致性 / 并发 | 同一实体被两条流并发改 → 有明确赢家规则,不产生脏状态 |
| 安全 / 隔离 | 跨系统读他域数据 → 权限/scope 限定;外部不可信内容入站有信任边界 |
| 成本 | 大批内容自动入 LLM → 成本可观测 + policy 限范围,不爆 |

## 二、主线场景（公司价值链端到端）

走蓝图 §5 north-star flow,逐 handoff 验证被审 spec 在链上的进/出是否自洽:

```
商机 → 资格(Pipe) → 报价(SOW+估算+产能校验) → 合同 → 项目交付(QA) → 里程碑验收 → 开票 → 收款 → 收入/毛利 → 绩效+现金预测 → 收口+CaseStudy
```

- 每个 handoff:上游发什么事件、下游声明消费没有(轴 1 双向声明)。
- 六关卡(转化/定价/合同/排兵/开票/收口)落对位置、开票门是唯一跨系统门(轴 6)。
- 详细主线见 [docs/MAIN-LINE-SEQUENCE.md](../../docs/MAIN-LINE-SEQUENCE.md);跨 spec 终审专攻这条链上的断裂(毛利缺口类)。

## 三、Pre-mortem 目录（假设已失败,倒推原因）

对被审 spec 问:「6 个月后它在生产里出大事了 —— 为什么?」逐条倒推到设计因:

- **越权坍塌**:这个系统揽下的某能力(外部集成/合规/分类)做不动 / 该归别人 → 轴 0。
- **断链漏账**:某 expose 没人消费 / 某事件没声明 consumer → 轴 1(毛利缺口)。
- **相位错配**:依赖了还没上线的系统能力,没降级 → 轴 2。
- **漂移腐蚀**:图/spec 与 contracts 偏离,builder 照错的接 → 轴 4。
- **门错置**:下游订了门控事件 / 自创逐条人工门卡死流程 → 轴 6 / 轴 11。
- **幽灵需求**:某集成没具名外部系统/dedup/同意边界,builder 乱接 → 轴 8。
- **信任边界破**:外部不可信内容直入 LLM/落库,无 policy 闸 → 轴 10。
- **自动化断点**:某管控强制逐条人工审,AI-native 流程断在人这里 → 轴 11。
- **治理跨面不一致**（governance profile）:agent 定义 ↔ scope-map ↔ hooks ↔ settings ↔ 派发 prompt 对「能不能写 / 写哪」表述打架 → 被命令写文件的只读 agent 退化用 Bash heredoc,正文撇号打破 `bash -c` 外层引号（`unexpected EOF while looking for matching '`）卡死（2026-07-20 K1 review defender 崩溃）。→ 治理轴「跨面不一致 / authority≠write」;CANARY 全文见 [gov-review-defender-write-scope ADR-001](../../delivery/review/gov-review-defender-write-scope/ADR-001-defender-write-scope-and-bash-guard.md)。

## 四、按 spec 类型挑场景（scenario-author 用）

| spec 类型 | 主挑 |
|---|---|
| 技术重(K0 运行时/K3 事件总线/K9 编排) | ATAM 质量属性(性能/可用/并发/成本)+ pre-mortem 漂移/门错置 |
| 领域重(S1/S3/S5/S6/S7) | 主线场景 handoff + 轴 0/1/2/8 + pre-mortem 越权/断链 |
| 内核数据(K2 实体图) | 所有权/Tier(轴 5)+ 单一源(轴 4)+ 主线骨架自洽 |
| 公司级(L0 蓝图/Inbound Router) | 轴 0 边界 + 轴 11 AI-native + 关卡纪律一致性 |
