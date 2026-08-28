# Minimal Implementation Reference / 最小实现参考

builder 与 fixer 在写实现 / 打补丁时消费本规则，避免过度设计。它只约束**实现代码的体量与形态**，不削弱验收覆盖、测试、安全或可运维证据。

> 命名提醒：本仓库里"偷懒"指跳过测试与证据（被各 agent 的 Rationalizations 反驳）。这里讲的是"少写不必要的代码"，与"偷懒"是两回事，不要混用叙事。

## The ladder / 决策阶梯

写任何代码前，停在第一个成立的台阶；两阶都成立时取较高那阶然后继续——它是反射，不是研究项目：

1. **这东西需要存在吗？** 投机需求就跳过，并用一行说明为什么。(YAGNI)
2. **标准库已经做了吗？** 用标准库。
3. **平台 / 运行时原生特性覆盖吗？** 用原生（DB 约束优于应用层校验，框架内建优于手写）。
4. **已安装依赖能解决吗？** 用它；几行能搞定的事别新增依赖。
5. **能不能一行？** 一行。
6. **才轮到：** 满足验收的最小实现。

两个台阶体量相同时，选边界条件更正确的那个——最小实现是少写代码，不是选更脆的算法。

## Hard floors / 永远不许简化掉

下列内容不在精简范围内；简化它们 = 缺陷，不叫"最小实现"：

- 信任边界输入校验（见 `security-hardening.md`）。
- 防数据丢失的错误处理、ErrorEnvelope、correlationId（见 `observability-k6.md`）。
- 安全与权限 / secret 处理（见 `security-hardening.md`）。
- 可达性基础。
- 任一声明的 `Sx-Ay` 验收点的真实断言（见 `acceptance-evidence.md`）。
- 用户或契约明确要求的行为。

非平凡逻辑（分支、循环、解析、money / 安全路径）必须留下一个能跑的 guard——这是最小实现的一部分，不是冗余，verifier / review 不得当成可删项。

## Mark deliberate simplifications / 故意简化要留痕

故意走捷径时，用 `minimal:` 注释标出**已知上限**和**升级触发条件**，让"以后再说"不会烂成"永远不做"：

```ts
// minimal: 内存单锁，吞吐成瓶颈时换 per-account 锁
// minimal: O(n^2) 扫描，n 超过 ~1e3 时换索引或哈希
```

- 约定格式：`minimal: <已知上限>, <何时升级>`。
- 没有升级触发条件的标记会被 integrator 打 `no-trigger`，那种最容易烂掉。
- integrator 在合并门前 grep 这些标记，收成简化债务台账放进 Review Packet（见 `code-review-gate.md`）。

## Scope within the dev loop / 在 dev loop 内的边界

- 只管**实现代码**的体量与形态，不管流程证据：验收测试、ErrorEnvelope、Review Packet、ADR 仍要完整，不许借"精简"删掉它们。
- 强度停在 "full" 等价：阶梯 + 护栏即可。不要把 YAGNI 极端化到漏掉任何声明的验收点。
- 与 `code-review-gate.md` 第 5 轴（simplicity）配对：本参考管"动手前怎么少写"，review gate 管"事后哪些该删"。
