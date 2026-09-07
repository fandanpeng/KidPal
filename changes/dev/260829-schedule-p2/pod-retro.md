# pod-retro — 260829-schedule-p2（第一人称热记忆）

> planner 第一人称过程体感，2026-09-07。负向边界：不复述/不载 tokens、gates、rounds、verdict、test results（权威在 machine artifacts / pod-events / verifier-evidence）。

## 卡点与绕轮体感

1. **pod board 文件位置探索浪费了几次探测**。launch 输入写 `devloop/pods/260829-schedule-p2.json`，本 worktree 与主 checkout 顶层都没有——实际在主 checkout 的 `.git/devloop/pods/`（git 内部目录、不入版本控制）。下次 launch prompt 直接写 `.git/devloop/pods/` 全路径可省这一段。

2. **mutation-check 的「骨架 ≠ 证据」让我烧了一轮 verifier**。我在 Finalize 阶段按 T6 前置清单回填了 mutationEvidence 骨架（每条验收的变异点清单），round 1 红在「missing or insensitive」——闸要的是**已执行**的实录（变异→观测红→sha256 还原核销），骨架只是执行计划。反思：骨架本身没错（T6 要求的就是骨架），但「执行 mutation 矩阵」这一步应该在 builder 交付时一并完成（builder 最了解自己建议的变异点），而不是等 verifier 红了再续派。下次 heavy 包直接把 mutation 取证写进 builder 派发 brief 的完成判据。

3. **mentionsId 的全文包含匹配是个安静的坑**。builder 回报的 mutation 证据里 acceptance 字段是截短形态（如「P2-A3: P2 课表页渲染 今天/明天 列表」），而闸按 taskpackage 精确全文做 `text.includes(id)` 匹配——直接回填会继续红且报错信息不会告诉你差在哪。我在回填时把每条记录的 acceptance 字段规范成 taskpackage 精确全文才过。给未来取证者的口径：**证据记录的 acceptance 字段一律复制 taskpackage 原文，不要改写、不要缩写**。

4. **WSL 9p 慢 IO 是本 Pod 最大的墙钟成本**。builder 报 set-target linux ~13 分钟、全量 build ~12 分钟，mutation 取证要多轮重编译更是放大。builder 自己用 setsid nohup 脱离会话跑通了，没烧我的挂钟——这个应对值得沉淀进 HOST_TEST_CONVENTIONS。

## 验收与实现质量体感

5. **frozen offline contract + weak 哨兵的设计很好用**。acceptance-author 把红基线钉在「行为缺失」而非「链接失败」，builder 拿到文件头契约就有清晰实现目标，verifier 两轮收绿、零 fixer 轮。这套模式在纯逻辑包可以复用。

6. **mutation 取证暴露的 3 个齿粒度局限是有价值的副产物**：P2-A1 删消费调用点齿不敏感（文件粒度计数被声明+定义满足）、P2-A3 同刻双项排序无断言承载、P2-A5 子串模糊无反例数据。都不是假绿（实现本身正确），但齿的「精确性由缺失数据承载」这点值得 acceptance-author tighten-only 补强——已由 builder 如实记录在 mutationEvidence notes，留给后继批次。

7. **home-p1 并行对齐比预想顺**。两端 blockingDecision 同日各自裁定 NOT-BLOCKED 且互相引用了对方的归属（它把 producer 链路风险明确划给我），producer/consumer 形状以契约 display 块为锚没有出现硬凑分歧。多 Pod 并行时这种「在 blockingDecision 里显式声明归属边界」的做法值得保持。

## escalation themes（cross-ref pod-events）

- 本 Pod 无 agent-escalation 事件。唯一的 judgment call = intake blockingDecision（Intent:nointent，eventId add1139d，NOT-BLOCKED），逐段拆链论证落在事件 payload 里，未升级 human——事后看这个裁定是对的：两条链路（语音问/录入）都不经被拒的 tools 路径。
- friction：无 friction 事件。round 1 的 mutation-check 红属于流程语义未吃透（我方），非机器故障。
