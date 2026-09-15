# Workstream m1-home-qa · review 记录

> 本文件 = workstream 级 review 裁决耐久记录（门 1 等），append-only、行不覆盖。

## 门 1（拆包 review 门）—— 2026-09-15

**裁决：GREEN**（轻量五项 + workType 分流全过；两项非阻塞注记 + 一项已裁事项如实登记；无打回项、无 Track A 升级）。

- 派发：主线程 2026-09-15，human 已点头走 review，OQ1 裁 (a) 轻量档。判官 = architect，只读审四件套 + 上游权威，零写入 canonical。
- 审查基准：`delivery/milestones/milestones.jsonl` M1-home-qa nextAction 四块 + `docs/requirements.md` 现文（§2.2/§2.3/§2.4/§6 阶段一、S-F1.1/S-F1.3/S-F1.4/S-F4.1、§8）。
- 落盘注记：architect 对本路径的写入被 scope-map 机械拦（enforce-scope.mjs，architect 写边界不含 `delivery/workstreams/**`），裁决全文由主线程代为 append——判官不绕闸，主线程不受 per-agent enforce-scope 约束。

### 五项逐项结论

1. **切片逻辑：过。** 按行为切，每包一条人可观察判据（roadmap ③）；块①拆两包有显式职责理由（layer 分层裁决模型 vs chrome 渲染面，owner 各自独占）；无偏离垂直切片项。
2. **依赖序与 WIP：过，附一项勘误（非阻塞）。** mergeOrder 22→26 与 dependsOn 及列车图边完全一致；WIP=2 = DAG 双根包（KD、PH）最大并行，合理。**勘误：「W1 两包文件面零交叠」不成立**——KD-A2（cycle_next 清层/进页分离，实现于 `firmware/main/display/kidpal_render_node.cc:468` 区）与 PH-A3（电池注入缝，`s_battery_pct` 于同文件 `:90` 区）实际共编同一 .cc；两包 crossEnd 申报的 symbol 不同但 KD-A2 义务必触该文件。编辑区域不邻接、串行 merge + rebase + 合并后重跑 host 基线（pod-coordination 常规）可容，不改判 GREEN；launch 执行者注记勿以「零交叠」为前提省略后并包 rebase 后的 host 基线重跑。
3. **decision-coverage 完备：过。** 上游四块全映射（含 happy-flow 挂账、timer deferred 非 dropped）；跨包行为 owner 四条双侧登记一致（layer_top=KD、四态骨架=VQ、电量 ADC=PH、keybar chrome=KB）；launchPreconditions 六条齐（F-03 commit 前置、门 1 先行、deviation 拍板、RM 硬依赖、冻结面扫描、图索引）。
4. **意图陈述与上游一致：过，附一项 minor 注记（非阻塞）。** 五包逐一回读 requirements 对账通过（KD=§2.3 对账 ①⑤ 逐字锚；KB=§2.2 键帽条行逐字锚；PH=S-F1.1 AC1–AC4 + ADC 三处死成员逐字锚；VQ=§2.2 三态行 + S-F4.1 AC1 + NFR-PERF-1/NFR-READ-1；RM=S-F1.4 AC1–AC4/AC6 + v1.23/24/44 口径）。**minor 注记：S-F1.4 AC5（断网无提醒）未在 RM 包 acceptance 显式映射**——该行为由已交付云侧推送链路承载（RM-E1 云侧零改动），建议 RM 包 acceptance-author 起草时以既有钉点或注记对账一句，防 acceptance-coverage 面缺口。
5. **demo 证据形态可行：过。** host runner（前批 104/4 先例 + 在册既存红判读条款在）+ 模拟器截图 + 真机轮（五包各一项 `kind: structural` 结构性验收、human 持板、证据落 `changes/dev/`）；演出/手感/ADC 实测类 host 面不可证，真机轮为唯一可实证形态且不占 Pod 内 verifier 闸；本批五包真机项均为不可替代的人可观察判据，均属核心包口径，可行。

### workType 分流结论

- **key-layer-dispatch · 整改型：过。** 根因（对账 ① 硬路由耦合 / ⑤ 恒回首页）↔ 改法（取舍 A/B/C）逐条对上；红基线在位；冻结钉点成批改（KD-A4）显式走 acceptance-author 留痕修订 + 不豁免验证。
- **keybar-three-slot / p1-home-battery / voice-round-ui / reminder-overlay-unify · 新建型：过。** 各锚 = requirements 现状差距登记，切片与覆盖齐；无整包重构型（KD-A2 清层/进页分离是整改型包内的纯重构步骤，有机制齿钉住）。

### 已裁事项注记（非阻塞）

页点数量 deviation（spec §2.2 冻结口径 = 3 页点、撤 P4 归 M2；keybar 阶段一按现行 4 页渲染）——human 2026-09-15 已认可，deviation 已在 KB-A2/KB-E1/decision-coverage/launchPreconditions 四处显式登记，4→3 对账随 M2 收回。

### 升级建议

无：不触 `docs/contracts/**`、单实现面（firmware）、无不绿项，不升 Track A。GREEN 后按 launchPreconditions 交 launch（human 过目切法 + F-03 commit 前置）。
