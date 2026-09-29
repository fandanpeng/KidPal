# Mutation 取证 · story_page_v2（TaskPackage 260924-m4-k2-p3-star-notebook-ui）

> builder 实做、planner 代存（2026-09-29；builder 对 `firmware/main/test/acceptance/**` 无写权，存储位裁定 = 本包目录，canonical counterEvidence 在 taskpackage.mutationEvidence.perAcceptance）。
> 通道 = host runner 全量（espidf_wsl.sh build + host_infra_test.elf）。每条 mutation：应用 → 齿红（红名 verbatim 见下）→ 还原 → 复跑确认还原位（还原位全套 = 128 Tests / 3 Failures，3 红均为冻结件缺陷待 acceptance-author 修订——SPV2-A4 / SPV2-A6 / SPV2-B1，与下表 mutation 无关）。已实做 11 / 13；SPV2-A4、SPV2-B1 两条因冻结件自身红被前置 abort 遮蔽、修订前不可观测（详见末节），挂 acceptance-author 修订后补做。

| ID | mutation | 观测红名（verbatim） |
|---|---|---|
| SPV2-A1 | 输入表 LIST KEY 短按回退旧码（return 1） | `SPV2-A1 RED: LIST 态 KEY 短按动作码 = 1, 期望 REC_SESSION(=7)——v1.47 矩阵「KEY = 开记录语境会话」未进输入表（旧码残留）` |
| SPV2-A2 | cursor_step 屏底分支删「落首项」（屏底退化为普通步进） | `SPV2-A2 RED: 屏底步进 r=1 cursor=4 screen=0, 期望 1/4/1（落该屏首项）——「光标到当前屏底再按翻下一屏并落该屏首项」语义缺失` |
| SPV2-A3 | debounce_gate 恒 1（消歧全局生效） | `SPV2-A3 RED: 非 LIST 态消歧门控非 0——消歧逻辑全局生效违反状态门控（其余状态单击必须零延迟, notes 边角②）` |
| SPV2-A5 | resend_due 上限判 tries < MAX 改 tries > MAX | `SPV2-A5 RED: tries=3 仍判重发——超过冻结上限 3 次, 重发无界` |
| SPV2-A6 | READER 末屏 wrap 改钳制不回绕（旧语义回归） | `SPV2-A6 RED: 末屏再翻动作码 = 8 screen=5 page=1, 期望 PAGE_TURN/0/READER——「末屏再翻 wrap 回首屏、永不离页」缺失` |
| SPV2-A7 | counter 钉①删去（total 权威退位） | `SPV2-A7 RED: total=23/count=16 时计数 = 16——items.length 顶替了 total（契约 §1.3③ load-bearing: >16 篇时窗口条数 != 星星计数）` |
| SPV2-A8 | stamp_text 降级分支删去（恒带 N） | `SPV2-A8 RED: seq 缺省降级文案 = 「星星徽章·第 -1 颗已入册！」, 期望逐字「星星徽章已入册！」（不带 N——契约 Q4 板端自定, 决策点 3 冻结值）` |
| SPV2-A9 | 层值改直接追加 8（不插入不上移） | `SPV2-A9 RED: 层值接线未落 STORY_DELETE=8 REMINDER=6 PROVISION=7, 期望 6/7/8——1g 裁读「符号级插入 + 上移」未执行（「append-only 低于 REMINDER」双约束唯一解 = 插入 6 + 上移 7/8）` |
| SPV2-B2 | story_page.c「帮我记下来」标签删除（footer 退单指令） | `SPV2-B2 RED: story_page.c 常驻语音入口无双指令——「我要讲故事」+「帮我记下来」须双触发词在世（intro 对账③; 旧单指令口径退役）` |
| SPV2-B3 | mcp_server.cc reconcile/resend/idle/debounce 四消费针眼失效（调用名后缀化） | `SPV2-B3 RED: mcp_server.cc 未消费删除链路/消歧新缝——纯逻辑决策表无 tick 驱动方（story_list 重推到达对账 / 5s 重发 / 10s idle / 300ms 消歧门控）` |
| SPV2-C1 | CMakeLists 注册行改名（story_page_v2.acceptance.c 摘除） | `SPV2-C1 RED: story_page_v2.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS——host 验收构建未接本包（BUILDER DUTY: 头注 Run protocol 0) 同款接线）` |

## 未实做两条（冻结件缺陷遮蔽，修订后补做）

- **SPV2-A4**（mutation = DELETE 态 KEY 双击改返 GLOBAL(-2)）：目标断言在该 TEST 的 tooth 3，但冻结 fixture 在 tooth 3 之前已因双按缺陷 abort（`SPV2-A4 RED(infra): 无法经 BOOT 长按进入删除确认子态（fixture 前提失败）`）——mutation 的期望红名无法观测。acceptance-author 修订 fixture（:979 探针调用与 :983 断言重复驱动同一视图）后补做。
- **SPV2-B1**（mutation = 单个 label 字体接线抽掉）：目标断言为 CJK 墨点计数，但冻结件在墨点探针之前已因 `host_infra_render_frame() > 0` 与在世 seam 契约（0 == 出帧，p1/schedule 先例同口径）矛盾而 abort（`SPV2-B1 RED(infra): host_infra_render_frame 未出帧——host 渲染 seam 未接`）。修订断言为 `== 0` 后补做。

## 备注

- SPV2-B3 / SPV2-C1 为运行时文件扫描齿：mutation 只改被扫文件文本、无需重编；其余 8 条为编译期行为齿：每条独立走 build+run 循环。
- 所有 mutation 已还原；还原位全套复跑 = 128 Tests / 3 Failures（仅 SPV2-A4 / SPV2-A6 / SPV2-B1 三条冻结件缺陷红，与本表无关）。

## 冻结件缺陷三处（builder 根因分析，planner 三方核验中 → acceptance-author 修订）

1. **SPV2-A4 fixture 双按缺陷（不可满足）**：`story_page_v2.acceptance.c:979` 的 `kspv2_require_impl(input(BOOT,LONG),…)` 探针调用会消费视图（进 DELETE 子态），:983 断言再次驱动同一手势——页态已 DELETE，按 :1021 tooth 2 冻结决策表必须返 DELETE_CANCEL(7)，与 :983 期望 DELETE_ENTER(5) 对同一可观测状态互斥。红基线未暴露因哨兵红在 view_init 先行 abort。修法建议：:979-980 探针移除（或改不消费视图只读检查），:983 单次驱动。
2. **SPV2-A6 tooth 4 off-by-one**：tooth 3 已断言末屏 wrap 后 `v.screen == 0`，tooth 4 从 0 双击（[K1] 头注「BOOT 双=PAGE_TURN」+ requirements S-F2.3 AC2「双击同短按」= 翻一屏）只能到 1，冻结断言钉 2——与 requirements 直接矛盾。修法建议：期望值改 1（或 tooth 3 后重锚 screen=1）。
3. **SPV2-B1 seam 返回值契约矛盾**：冻结断言 `host_infra_render_frame() > 0`，而在世包 0 渲染缝 `host_infra_seam.c` 成功契约 = 返回 0（schedule_grid / p1 诸齿均钉 == 0）。修法建议：断言改 `== 0`。
