# pod-retro · 261014-voice-round-entry-exit-fixes · 补充轮 261014-r2

> **第一人称热记忆**（planner 收尾相位，integrator closeout 之后、supervisor 机械闸之前）。只记过程体感与路由，不复述机器权威件（tokens / gates / rounds / verdict / test results 以 pod-events 与 verifier-evidence 为准，本文件不载）。

## 这轮是什么感觉

补充轮比原轮难写，不是为了多改几行代码，而是**「r1 的修法自己在真机上破了功」**这件事本身。r1 收口时所有齿全绿、结构齿钉得整整齐齐，我对「判据③ 采样」那段的信心是足的——结果真机一跑，多句回答的轮次播完就掉 Idle。根因回头看很朴素：`sentence_start` 是逐句消息，我把采样写点当成「每轮一次」，可它实际是「每句一次」。**结构齿钉住了「写点在 setSpeaking 之前」，却没钉住「写点该有条件」**——这是我这轮最实在的一课：源锚结构齿能钉位置，钉不住语义前提，语义前提要靠另一条负向齿去兜。acceptance-author 这轮补的负向齿（钉「无条件逐句写」形态不存在）正是我上一轮欠的那一块。

## 卡点与别扭

- **A2 的锚很滑**。冷通道毛刺那条，根因藏在析构时序里：`OpenAudioChannel` 裸重赋值 `websocket_`，旧对象的断连回调走了「非本地关」分类。修法只有三行，但「把修法钉成结构齿」要切 `OpenAudioChannel` 函数体并比位次。builder 一度被自己的注释坑了——注释里写了 `CreateWebSocket` 字面，撞上 tooth 的序锚，误红。这说明**源文本结构齿对"注释里出现关键字"极其敏感**，是一条需要写进教训的脆弱面（已作为 FYI finding 登记）。我倾向认为这不是 builder 的错，是结构齿这种形态的内生脆弱，将来要钉位次序的齿，最好先约定注释里不出现被扫的 token。
- **子集跑法的口径**：human 明确「只跑相关子集、不跑全量」，而 firmware host 是单 ELF、天然全量跑——「子集」在固件侧其实落成了「跑那个 ELF」（因为被测源文件根本不可 host 编，行为全靠源文本齿承载）。这个错位我花了几分钟才想顺：不是我在偷懒缩测，是这套 host 基建的物理形态决定的。值得记一笔，免得下次再来一遍。
- **投影漂移**：`current/plan.md` 停在 r1 版、没随 r2 立盘刷新。integrator 按边界没擅动，报了回来。这是**隐性全局指针**类的坑——current/ 里其余件（taskpackage、两份 brief、verdict）都刷了，就 plan.md 漏了，容易骗过「我刷过投影了」的直觉。收尾相位我补刷了。教训：刷新 current/ 投影应做成一次性原子动作（整目录对齐），不要逐件记得刷。

## 人工纠偏

human 这轮的出场方式是「加边界、不加指令」：立补充轮的同时把五条执行边界一次钉死（engine-patch 不重部署 / 真机不在本 Pod 范围 / 只跑子集 / 严格模式默认开 / commit 不 push 不 merge），并单点提醒「重跑 supervisor 闸前必须先删旧 verdict」。**「删旧 write-once verdict」这条如果没人提醒，我大概率会撞墙**——write-once 闸撞旧件时的报错不算自解释。这是 write-once 制品在「同 changeId 补轮」这一合法形态下的摩擦点，值得在机械层想想是否给个人机工程提示（不改闸判据），例如闸在发现旧件且 phase/round 更新时给一条「补轮请先显式删除」的 advisory。

## escalation / friction themes（cross-ref pod-events）

- `human-decision` `4fd33703-7ce1-4ccd-bf81-443f15db6b97`：r2 立盘 + 五条执行边界的 human 口径（本轮唯一的 human 触点，属既有免请示面上的概括性边界授权，非逐条请示）。
- `friction` `over-scan` `59d733b3-…`（acceptance-author）/ `83e248ef-…`（builder）：两条超读申报，读的都是**契约/源码锚点所在文件**（断言对象本身不在 input-brief 清单里）——申报非空但全部合理。这提示一个形态：**结构齿的断言对象文件天然不在 refs 派生面内**，超读申报在本类包里会结构性偏高，属预期噪声、非纪律松动。
- 本轮**无 agent-escalation**：两处根因都是 CONFIRMED、修法在计划期就裁定，没有触发升级链。

## 下一轮改进路由

1. **结构齿 + 负向齿配对**应成为「条件性语义」修复的默认姿态（本条已在本轮落齿，建议沉淀进 acceptance-author 的惯用形）。
2. **序锚型结构齿的 token 污染面**（注释撞关键字）建议进 acceptance-author/planner 的共同教训（可选：齿内加「只在代码行、排除注释行」的切片纪律）。
3. **补轮删旧 write-once verdict** 的人机工程摩擦（见上）→ 留给治理批评估是否加 advisory。
4. current/ 投影刷新原子化——整目录对齐优于逐件记得刷。
