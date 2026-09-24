# pod-retro · 260924-m4-k1-story-cloud-link（planner 第一人称热记忆）

> 写于 integrator closeout 完成、final 终审绿之后、supervisor 机械闸之前。不复述 tokens / gates / rounds / verdict / 测试结果——权威在 pod-events.jsonl、verifier-evidence/rounds/、dispatch stamps 与 closeout 制品。

## 一、这一棒最省力的投入：把闸的判据在派发前读穿

intake 期我花了不少时间做三件「预读」：docs-sync 闸的判据全文（发现 mention 检查有 docsSyncReason 豁免分支——perf-k1 咬过人的结构性陷阱在本包不复发，且这是读脚本才知道的，光看上一包 retro 会误判）、firmware-build 的严格模式语义（无零 diff 跳过；工具链在位即真编译）、mutation-check 的证据形态（静态+证据制、structural 豁免）。回报是机械验证两轮都一次过、零闸红返工。主观结论：**机械闸时代 planner 最便宜的投资 = 派发前把只读闸逐个诊断实跑 + 读判据**，比红回来再猜闸的语义便宜一个量级。perf-k1 retro 的「ErrorEnvelope 先判 failingStep 属谁的面」我承接了，但更上一层是让它压根不产生。

## 二、冻结齿缺陷那一段：环路按设计走通了，但代价结构值得记

`test_a6_restart_survival` 的 fixture 缺陷（start 后漏 feed，空累积 stop→None）由 builder 实现期暴露、即停上报不自裁，我三方复核（单测复现 + archive 先例 + E1 齿），acceptance-author 经 §6 tighten-only 窗口修订（本仓首例 sha256 核销），builder 复跑闭环——全链零 human 介入、零越权。两点主观判断：

1. **该齿在冻结期就「红得不对」**：红基线只核「红没红」，不核「红因对不对」——notes 当时把红因记成「账本缺失」，真实原因是空累积缓冲，实现落地后仍红才暴露。改进想法（loop-retro 素材）：冻结设计态红齿时对每齿做红因归因自查（断言失败消息 ↔ 声明的缺失行为逐条比对），fixture 缺陷可以在冻结期就曝露，而不是烧一轮 builder。
2. **派发书里写「分析预警」而非「结论」的粒度是对的**：我预判到只补一行 feed 会让该齿在原变异下不可判别（账本与计数派生同为 4），把分析写进派发但把 fixture 形态裁定权留给 acceptance-author——它采纳并做出了删除步判别形态，一次闭合。既没越权替它裁，也防住了可预见的二次绕轮。

## 三、我自己的两处纪律漏（如实记）

1. **派单命令写错**：给 integrator 的收口义务里写了 `changes-index.mjs --upsert`——那是 arch lane 专属入口，dev lane 应走幂等全量重建。integrator 实测红了、读脚本自行诊断改道，还顺手把全量重建对 arch lane 的排序扰动还原掉。教训：**派单里的机械命令要么先验语义、要么只写目标不写命令**（把路径判断留给执行角色 + 脚本 fail-loud）。
2. **红基线第二方复跑撞上机器负载**：两个子代理收尾窗口期复跑套件，慢了一个数量级、超进后台（计数一致无实害）。时机敏感的复跑应错开子代理收尾，或在记录里注明负载情形——我在 progress 里注了，但当时没预判。

## 四、遥测观察（留痕不裁决）

各相位派发（含两次显式 opus 升档指令）的 transcript 派生**实际生效模型均为 deepseek-v4.1-flash**——升档指令被底层路由映射。交付质量未见受损（冻结齿、实现、收口全过机械闸与第二方复核），但「MODEL-TIER 升档在本环境是否真实生效」是分层治理的遥测面可观察事实，裁决归 human/loop-retro，我不越权自裁、只如实留痕。

## 五、结构性事实核定（供治理面消费，本包已按预案处置）

- **C4 类 arch 收口义务对 pod 内角色结构性不可达**：`docs/architecture.md` 写权 = 治理批 landing 通道专属（写入史印证）；scope-map integrator allow 仍含已废弃 `docs/systems/**` 而无 architecture.md——拆包后 docs 平铺已九个多月，这条 allow 没跟上。本包处置 = patch 形态 delta 落 docs-delta §4（五 hunk、单命中自检）+ 合并后 human spec 批落地（ARQ-260918 四处可同批搭载）；review-packet Findings #1 在案。**K2/K3 的 docsSyncReason 同样含 arch 面条目——下两棒 integrator 派单应把这套处置模板前置写进去**，别让它们自己再撞一遍写边界。
- **docs-sync 闸的「引用到的文档 ≠ 可写的文档」错配本体未治理**（本包靠 reason 分支通过）——perf-k1 §5 建议的 ARQ 未见登记，同族事实第二次过境，loop-retro 应正式收口。

## 六、escalation themes（cross-ref pod-events by eventId）

- `agent-escalation` **c1096ab9**（ambiguous-acceptance / blocking）→ `agent-escalation-resolved` **1be9997e**：冻结齿 fixture 缺陷全链（见 §二），resolution = §6 tighten-only 窗口，零 human 介入。
- `friction`（over-scan）×2：**3cb1bcea**（acceptance 阶段申报十七行，两行超先例封顶——front 离线装机先例）、**c71f29f4**（build 阶段申报四行——信封形状/原子写口径/收集配置/单测体例）。均为申报纪律常态面，advisory 记账不当门。
- 无 human-decision / human-correction 事件：必升面零触碰（契约门在 launch 前已完成 HUMAN_APPROVED；合并门在本件之后）。

## 七、下次改进（具体到动作）

1. acceptance-author 派单模板加「设计态红齿红因归因自查」一条（或推动机检化——loop-retro 素材，见 §二.1）。
2. 派单里的机械命令先验 lane/语义再写（§三.1 changes-index 教训）。
3. K2/K3 integrator 派单前置 C4 处置模板（patch 形态 delta + human spec 批 + gate evidence 保持 pending 的注记格式），直接引本包 docs-delta §4 为先例。
4. 向 human 的合并报告沿用 perf-k1 §八教训：首段一句白话结论，需要 human 动手的事列成清单置前，制品引用置后、被追问再展开。
