# pod-retro · 261117-session-end-intent-longretry（P2）

> 第一人称热记忆（planner 全程在场的主观体感）。**不复述机器件**：tokens / gates / rounds / verdict / test results 以其自身制品为准。

## 这次顺在哪

- **预评前置的价值兑现了**。architect 的 planning 期预评把「本仓最密的一片源锚雷区」提前摊开：`application.cc` 的函数定义顺序本身就是冻结面、注释里写出被计数的符号字面会假红、`HandleStartListeningEvent` 不能新增 `return;`。builder 首轮一次全绿、没有 verifier↔fixer 绕轮，我主观判断主要归功于这份逐条雷区清单提前进了派单上下文，而不是运气。下一包如果同样落在 `application.cc` / `mcp_server.cc`，值得照抄这个「预评先行 + 雷区表进 brief」的形状。
- **验收冻结与实现分离得很干净**。acceptance-author 把新件写好后用 standalone 通道先证红，builder 只做登记与实现，两边没有互相踩写权。
- **变异证据没有走过场**。每条 SEI 一次「改坏 → 转红 → 还原」，而且 builder 主动申报了三条「offline 证不了、只能真机」的边界，没有拿结构齿的绿当行为证据。

## 卡点与别扭处

- **最大的流程摩擦是 host 验收件的 CMake 注册写权缺口**。acceptance-author 的写权只有 `*.acceptance.c`，注册行在 `CMakeLists.txt`，它写不了；而没注册就跑不出红基线。这次靠「standalone 通道单独跑新件」绕过，属于临时手法而非设计路径。我建议后续在 launch 面准备里加一步「新 host 验收件注册位预登记」，或者把该文件划进 acceptance-author 的窄写权（属治理面改动，归 `/gov-change`，不在本 Pod 自裁）。
- **SEI-C1 这种「构建接线即验收」的条目有点别扭**：它是注册序守卫，本质是 build wiring，却要占一条验收 id 才能被 `acceptance-coverage` 看见。语义是对的，但读起来像把构建纪律塞进了验收账。
- **input-brief 的锚点形式易碎**。`#§1.4-end_session` 这种「节号 + 行名」的复合锚解析不到，白白产出一条 advisory；改成纯节号即可。属于文档侧的小坑，改一行就好。

## 帮倒忙的钩子 / 规则

- 本会话是 VS Code 扩展形态，`dev-loop.settings.json` 三件按缺席处理（Start/Stop 的 supervisor 门、portfolio-sync 自动 reconcile、git merge/rebase 的 ask 门）。我在收尾相位手工跑 supervisor-gate-check 与 board reconcile——流程上没断，但**自动 reconcile 缺席意味着 board 状态要靠我自觉**，这是扩展形态的结构性降级，值得在下次扩展会话开局就写进预期。
- commit 时 hook 报了 `input-brief.md:87 new blank line at EOF`，来源是脚本生成的 brief，不是本包内容问题；这类「生成件触发钩子」的噪声会让收尾相位多一次判读成本。

## 人工纠偏体感

- human 一句「就在本分支，不要新建了，不要headless」直接改写了我按 `kidpal-pod-launch-prep` 默认分支走下去的路径（默认会评估建新树）。我一开始按 skill 走、发现树在、board 无冲突，才顺回 human 的意图。**教训：human 有明确指令时，launch 面 skill 的「按需新建」分支应当先被指令短路**，不要先按 skill 铺开再收。该口令已落 `human-decision` 事件（`a1339c43-e3e7-4031-bbe5-542e87ae2a67`）。
- 没有出现方向性争议需要撤回的场合；三处 planner 裁决（不抽缝 / 意向位不设时效 / 唤醒词两代码点）都在已 FROZEN 的 arch 包络内，human 未介入。

## escalation themes

- 本轮无 `agent-escalation` 事件，只有两条 `over-scan` friction（acceptance-author `ea093923…`、builder `4f6905bd…`），均为超读申报的例行记账，不是异常信号。
- 观察到的同族主题：**「写权面与测试可跑性不重合」**（CMake 注册、mutation-evidence 落盘两处都靠 planner 代落）。这可能是下一轮治理复盘值得收的一条。

## 下次改进（一句话清单）

1. launch 面预检里加「新 host 验收件注册位」检查，别等 acceptance-author 撞墙。
2. brief 锚点一律用纯节号，禁用「节号 + 行名」复合形式。
3. 扩展会话开局把「手工 supervisor 门 + 手工 reconcile」写进第一屏预期，别到收尾才想起来。