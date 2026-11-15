# pod-retro · 261104-k5-story-cover-nav

> Pod 收尾复盘（planner 第一人称热记忆，closeout advisory 指名）。形态 = VS Code 扩展会话内联
> planner；base ref `03630b65`；pod-events 全程。只记根因、摩擦、纠正与未解项，不复述 tokens/gates/rounds。

## 顺利面（保持）

- **launch 面基线全现跑**：server 447 passed / host 292-3 都在当天量到并带 SHA 落锚，两处分叉簿记（K3/K4 叠跑遗留）在开工前 rebase 抹平——后续包 intake 没背旧债。
- **1g 预评挡掉两个会变贵的错**：① 封皮落 `story_page.c` 而非 `render_node.cc`，使封皮四件/计数两态可做 host 真帧像素断言（否则只能真机目测）；② 提前发现 `KSTORY_ART_CACHE_SLOTS=7` 已占满，扩容进实现而不是等真机「资产不显示」再排。
- **human 两项裁定前置**（离页帧 restore 归属 / 板级 BOOT 钩子），避免了 acceptance 冻完再翻案的返工。
- **变异证据逐条可复现**：10/10 都有「改坏 → 目标用例原文变红 → 还原 → 复绿」的成对证据，且实现面 zero-diff 有 `git diff --stat` 实证。
- **扩单通道按规矩走通**：`mcp_server.h` 出界 → 不先改后补，architect 评估 APPROVE 后才 append manifest；`StoryEnterCover` 的实现偏离也经 architect 复核确认「不用 ScheduleStoryRender 是对的」。

## 踩坑与教训（供 loop-retro）

1. **`acceptance-coverage` 认的是标记字面，不是语义指向**：验收件里写 `coverage: S-F2.6-AC1 -> KCN-C1` 人读合理，闸只认 `coverage: KCN-C1`；round 1 因此整批报 missing。**共性**：新验收件冻完必须自己先跑一次 `acceptance-coverage-check`（位置参数形态跑，见 4），把「断言对但标记不匹配」这类红挡在派 builder 之前。
2. **冻结件跨用例状态残留第二次命中**（SPV2-A1 tooth 3 留 COVER、tooth 4 未复位）：K2/K4 之后已写进派单「跨用例一致性自检」，仍然发生。**共性**：自检条款目前是散文义务，落不到执行——建议把「同一 TEST_CASE 内复用的 view 在换语义前显式复位」做成 acceptance-author 派单硬条目 + 冻结前 grep 复查点。
3. **变异证据的墙钟成本被低估**（90 分钟，9 轮 host 全量重编+301 用例全跑）：瓶颈不是变异本身，是每轮 CMake regen + 全量 ELF 跑。**共性**：host 面变异可改窄跑（Unity `--test-filter` / 单 TU 重编）把 10 条压到 3 轮以内；建议下一包派单直接给窄跑配方。
4. **机检脚本旗标面不一致**：`acceptance-coverage-check.mjs` 不认 `--change-id`（当位置参数路径），`mutation-check.mjs` 认。两次派单都写错、靠执行者自行纠正。候选 ARQ：补 `--change-id` 对齐先例。
5. **manifest 起草连续两包漏「实现会动的 TU」**（K4 漏生成件命名律、K5 漏类声明头文件）：粗 glob 只列 .cc 会漏 .h。**共性**：manifest 起草按「这次会改哪些编译单元与其声明」展开一遍，而不是照抄 plan 的落点列表。

## 未解项 / 交接（交主线程与 human）

- **真机轮 K5S-E4 未跑**：BOOT 四页并集、封皮→列表→阅读逐层返回手感、封皮观感 design review（含「封皮未画小伴」偏差项）、语音「打开故事」端到端、restore 不抢前台真实时序（含「READER/RECORDING 驻留多一次基层重绘」闪帧观察项）、**归档失败离页帧落封皮而非列表**的观感确认、删除/盖章计数并集。`pre-merge-escalation-gate` 已按预期 BLOCK 在 `device-round-evidence-missing`。
- **合并门 = supervisor ESCALATE**（riskTier high 结构性，evidenceComplete=true，completedRounds=3）：需 human 人批 + 真机轮证据两件同时满足才可 merge-driver。
- **跨包联合收口 K5S-E5 归主线程**：整列车真机 + 真实 QQ 端到端轮，K5 合并 ≠ workstream 完成。
- **远程发布**（`release-pr.mjs --publish` 只推 `release/*` 并开 PR，PR 合并仍 human）与 K4 的 ECS 部署轮一起交主线程。
- **K3 歧义删除观察项**仍未闭环（已按 human 裁定继续观察）。
- **loop-retro 阈值再次触线**（27 ≥ 3）：发起责任 = 主线程。
