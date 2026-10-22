# Pod Retro — 261022-p3-record-chrome-ghost-fix（planner 第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/rounds/verdict/测试结果（权威在 machine artifacts / pod-events / verifier-evidence），只记过程体感。

## 本 Pod 最有价值的教训

**docRefs 是 docs-sync 闸的锚面，不是参考文献列表**。r2 唯一红就是它：我把 debt 条目和 plan.md 写进 docRefs 时，没意识到机检要求这两份文件正文里逐字出现声明的验收 id（CG-A1..A5）——plan.md 写于冻结齿之前，自然没有 CG 锚。补齐动作很小（plan.md 冻结齿映射节 + debt 条目 consumed-by 追记），但多花了一整轮。下次 acceptance id 定稿的同时就回填 docRefs 文件锚句，不等 verify 红了再补。

## 卡点

- **final round 序数不是「final 的第 1 轮」**：派单写了 `--round 1`，机械闸拒绝——round 序数跨相位全局单调，本包 scoped 已烧掉 r1/r2/r3，final 必须是 r4（「round 1 is not next completed ordinal 4」）。闸是对的（序数连续才能防 evidence 抽换），是我派单措辞按直觉写了 1。以后派单统一写「final round = next completed ordinal」。
- **mutation 取证从合成树升级到真实树**：r1 mutation-check 红，fixer 七变异逐个真实树改、跑通道 B、sha256 逐字节还原——这是对的做法但成本不低（七次编译-跑-还原循环）。好在冻结矩阵预测逐条命中（每变异只打红对应 TEST_CASE），零意外返工。倒推：如果 acceptance-author 冻结时就把「哪个变异打红哪个 TEST_CASE」的矩阵写死，fixer 取证就是机械执行——这个矩阵这次 taskpackage 里有了（redBaselineResult 七处定向变异段），是值得保持的先例。
- **changeManifest 缩面要主动申报**：种子里的 kidpal_voice_states.c / kidpal_render_node.cc/.h 最终零 diff（预评把复位落点裁到了 application.cc、层栈收口复用既有通道）。缩面是好事，但如果不留痕，下一个读包的人会对着 manifest 找不存在的改动。已在 review-packet 白盒对账节和 scope-final §2 双侧记了。

## 顺的部分

- 双通道红基线一次冻结到位：227T3F 与锚 0df2c0c3 全同零扰动（通道 A）+ 5T5F 全设计态红（通道 B），builder 一轮转绿后零新增红。设计态红先行的老纪律在这包又立功——三个缺陷三个断言全部「现状实证红」，没有一条靠想象。
- detect_changes 圈定干净利落：seed 58 / impacted 18 全落 firmware/main，零 server/ 跨面，帧类型负锚（不新发明下行帧类型）从 triage 一路守到实现，crossEnd 豁免声明与图实测互证，契约门全程零出场。
- 三轮 verify（红→红→绿）每轮只红一件事，修复都是小动作——mutation 取证一轮补齐、docs 锚一轮补齐，没有复合红。
- 真机门纪律照旧：CG-A5 live-manual 与 diag-log-content 收尾真机轮合并执行省烧板，Pod 到门前停等。

## 下次改进

1. TaskPackage Finalize 时同步回填 docRefs 锚句（验收 id 定稿即写，不等闸红）。
2. 派单模板的 final verification 命令位写「--round <next completed ordinal>」，不写死数字。
3. 「七处定向变异 × 预测打红 TEST_CASE」矩阵进 acceptance-author 冻结标准产出（本包已自发做到，值得固化）。
