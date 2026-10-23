# pod-retro · 261022-m5-k1-prompt-carrier-migration

> 第一人称热记忆（planner，2026-10-23，integrator closeout 后 / supervisor 闸前）。只记过程体感，不载 tokens/gates/rounds/verdict/test results（权威在机器制品）。

## 卡点与体感

- **acceptance-author 的 taskpackage.json 写权被 scope 层拦**（enforce-scope 判它只许写 server/acceptance）——这本身是预期防线，但派单时我没预判到，导致冻结相位尾部多了一次「回执转 payload、planner 代写」的往返。派发书三条硬约定里的「scope 写权前置核对」我在这次派发前没逐条过，下次 acceptance-author 派单应在派单里预指派代落人（planner 代写 taskpackage 元数据字段），省一个往返。
- **supersede 影响面是 acceptance-author 用 grep sweep 自己扫出来的**（m3k3/m4k3/perf-k1 三件不在拆包 impactedTests 里）——拆包期的 impactedTests 种子靠人工枚举，漏了「解析三面指针」这类跨件守卫。扩单授权流程走得顺（planner 裁 (a) + 同批扩 changeManifest），但这类「守卫解析面 sweep」值得在 1g 档 architect 预评时就做，而不是冻结相位返工补。
- **final 轮第一次 invoke 因缺 `--scope-file` 被 verify.mjs fail-closed 举手**——我没先读 integrator Step 4 的「终验两步走」就把 final 轮排在自己头上。时序应该是：scoped 轮归 planner、final 轮归 integrator（携圈定清单）。轮号回收复用（r2 未产证据）没有留下脏状态，fail-closed 红闸这次帮了忙。
- **E2 二选一的「推荐」被冻结判法反推**——我在冻结相位推荐「新件取代」，builder 落地时发现判法组合下唯一自洽解是「原地改写」。教训：判法（验收断言的路径解析口径）在冻结时就锁死了定形空间，planner 的「推荐」若与判法不自洽，应在校对判法时就发现，而不是等 builder 报回来。下次对「二选一」类锚，冻结相位应先做一次判法自洽性推演。

## 顺的地方

- 拆包四件套质量高：plan.md 的可行性判据（尤其 marker 锚碰撞预告）让 builder 同批撤换「实时事实回合」锚零返工。
- verifier 退役后 planner Bash 直跑 verify.mjs 的形态很轻，evidence-check 一步出结论。
- integrator 一次收口干净：scope 圈定无降级、closeout 两制品 + commit 一气呵成。

## escalation themes

- 本包零 agent-escalation 事件、零 human-decision/human-correction 事件（扩单两项为 planner 权内决定，未升级）。
