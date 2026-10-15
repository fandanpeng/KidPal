# pod-retro · 260917-m2-k1-char-link-defect-fix（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts（verifier-evidence/rounds、pod-events、taskpackage、verify-runs）为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 实读），friction 条目 cross-ref 同文件 pod-events.jsonl（over-scan 两条 + degraded 一条，eventId 在 pod-events.jsonl）。

## 卡点与体感

1. **rma8 retro 的改进项这次真的内化了，买到了直接收益**：上一包第 1 条教训「mutationEvidence record 形态须全串 id + 敏感信号」我在派 builder 前就翻了 rma8 落盘形态、读了 mutation-check 的 `recordIsSensitive` 源码，回填后**先裸跑 mutation-check 预检再进 verifier**——mutation 闸一次绿，rma8 时代那种「verifier 红一轮才学会语法」的税这次没有交。体感：retro 改进项变成动作清单（预检闸再进 verifier）是可复制的，下包继续。

2. **r1 唯一一轮红是拆包元数据形态税，但根子在上游不在本 Pod**：`origin.type/upstreamType="milestone"` 不在 taskpackage-required 闸的值域闭集（`resolveOriginType` 优先取 `upstreamType`），portfolio-planner 拆包时产出的是闸不认的枚举值。我在 finalize 权内两步修掉（type→delivery-story、upstreamType→delivery-story），但**第一次只改 origin.type 没生效**——闸的解析优先级文档没读透就动手，白跑一次检查。这个「portfolio-planner 产 milestone 枚举 ↔ 闸值域不认」的错位值得登记给上游（拆包器应产闸值域内 origin 值，里程碑来源用 sourceRefs/upstreamRef 锚点表达、不用非标枚举）。

3. **crossEnd 对端符号名是拆包器凭印象写的，brief-inputs fail-loud 兜住了**：包里写 `char_stroke_parse_path`，真实符号是 `kidpal_char_stroke_parse`——brief-inputs 拒产 brief 强制我先修。图索引对该文件零命中（增量索引滞后），降级 Grep 定位。体感：fail-loud 闸 + 降级路径都好使；但「对端符号名未经源码核对就进包」本身是拆包期债务，若闸没有 fail-loud 这就变成 builder 期的静默错配。

4. **device build 的焦虑这次没有了**：rma8 时我担心 builder host-only 声明会让 firmware-build 闸红；这次直接预判「Windows 侧工具链真编译、闸会兜住」，builder 如实降级声明、闸绿收场，全程零虚惊。上一包的体感红利兑现了。

5. **T3 判停协议省钱省得顺手**：两面零差异直接引用锚行（server 37P/0F、host 35T/0F @ be3cf9f），既有套件基线零重跑；新齿的 RED 证据独立实跑——「套件基线引用」与「新齿红基线实跑」两层分开，语义干净，没有为此多绕一轮。

## 别扭的 acceptance / 帮倒忙的钩子

- taskpackage-required 闸报错文本「origin.type (milestone) must equal sourceType」里的 "(milestone)" 实际来自 upstreamType——报错文案与字段来源有歧义，我按字面去改 origin.type 第一次没生效。建议闸文案标注解析优先级（upstreamType ∨ type）。
- mutation-check 传参形态是位置参数、无 `--change-id` 旗标——我按 verify.mjs 的习惯传旗标被当成了路径。小坑，`--help` 分支的缺失是连锁的老问题（rma8 已在案 changes-index 同款）。
- enforce-scope 拦 builder 写 taskpackage 正确（mutation 证据落盘归我），builder 如实回传文本未越界——钩子边界和 agent 的诚实声明配合良好，无帮倒忙。

## 人工纠偏体感

- 本包零 loop 中人工纠偏：launch 卡指示明确（判停协议、严格模式声明、留根评估要求全在卡上），全程未触发 AskUserQuestion，升级链一次未用。

## 下次改进

- **对闸传参先看一眼 argv 解析形态再调**（本轮 mutation-check 位置参数小坑）；治理脚本传参形态不统一是系统性小税，逐次现查比猜快。
- **拆包元数据闸值域自查**：接手 portfolio-planner 拆的包时，intake 期先裸跑一次 taskpackage-required-check——r1 的形态税本可在派 acceptance-author 前消掉（当时闸检查不在我的起包 checklist 里，加进去）。
- **对端符号核对动作前置**：intake 期对 crossEnd counterparts 做一次源码级符号实查（Grep/graph），不信任拆包期凭记忆的符号名——brief-inputs 会兜，但早核省一次返工。
