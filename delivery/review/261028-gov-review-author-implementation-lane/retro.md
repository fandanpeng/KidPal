# retro · 261028-gov-review-author-implementation-lane（Track B：设计 R1–R5 → human 指示主线程直接实施 → R6 真实 diff GREEN → landing-verifier PASS + 4 ADVISORY）

> 主线程第一人称热记忆 · 2026-10-28 · gov-debt-reduction 后续 enabling 批 · architect / reviewer / verifier 均为独立子 agent；R6 与 verifier 使用新实例，独立审真实实现。

## 1. 时间账

| 相位 | 内容 | 实测 |
|---|---|---|
| 设计讨论 | P8 成本复盘 → human 拍 scripts+hooks、gov worktree、同树验证 | 形成 design.md |
| PROPOSAL R1–R5 | 双 lane、写权、证据尾、默认翻转、路线 B 与临时写权 | R1 BLOCK(F1) → R2 BLOCK(F7) → R3 GREEN → R4 BLOCK(F8) → R5 BLOCK(F11) |
| 路线改道 | human 指出不能被规则困死，明确主线程直接改，再做独立评审与验证 | 停止继续打磨 PROPOSAL；本批按 packet-bound 直接实施，未执行临时写权三态链 |
| 直接实现 | 26 件 manifest：双 lane 文面、scope-map、hook、模板、reviewer/verifier、结构门、合并闸 | 未新增一次性装配脚本；原起草 `_scratch/` 已删除 |
| R6 真实 diff 评审 | 新 reviewer 实例逐件审实现并独立黑盒探测 | GREEN；F13/F14 顺手修，F15 留根 |
| landing 核验 | 新 verifier 实例逐文件 26/26 + 全量机检 + 独立黑盒 13/13 | PASS + 4 ADVISORY；计数笔误 20→19 已更正 |

对照：P8 为 42 处编辑、1434 行 PROPOSAL、4 个临时脚本、18 个仓外副本。本批后半段改为直接实现后，没有再做逐字未来代码装配，也没有新增仓外装配副本；真实 Git 仓夹具转为常驻自检。

## 2. Token 账

子 agent 数字取 harness usage；同一 resume agent 报累计值，不重复相加。主线程无逐相 token 计量，不装精确。

| 载体 | tokens | 备注 |
|---|---|---|
| architect（设计起草与修订，最终累计） | 314,937 | 多次 429 中断续跑 |
| R1–R5 reviewer（最终累计） | 167,506 | 含隔离树权限探针 |
| R6 真实实现 reviewer | 122,642 | 新实例，38 次工具调用 |
| landing-verifier | 108,325 | 新实例，29 次工具调用 |
| 主线程直接实现与机检 | 未逐相计量 | 不估算精确值 |

最大可避免成本 = 在权限写法尚未实测前反复修改 PROPOSAL。R4/R5 的隔离树探针证明，真正需要回答的是“当前 hook 到底放行什么”，而不是继续润色授权文字。human 直接实施的指示把流程切回实现—评审—验证三件事。

## 3. 摩擦与根因

- **F1 · 先写权限方案、后测权限语义**：临时授权连续遗漏 agent def 与 G1 文件，根因是 deny 优先、allow 非空未命中也拦，单改 deny 并不足够。R4/R5 reviewer 用隔离树实测接住。
- **F2 · 把转写凭证误当验证本体**：26 件中大量行为规格若强行补逐字块，只会再复制一份实现。最终明确：需要保证的是被审、被验、被合并为同一实现，而不是 PROPOSAL 内再写一份未来代码。
- **F3 · auto mode 自改权限拦截**：直接实施中两次被分类器拦截；切手动后继续，没有绕过。剩余修改主要是测试、文面与独立核验。
- **F4 · Windows `^` 与夹具形状**：真实 Git 仓 happy path 初次失败，原因是 shell 形式 `rev-parse <sha>^{tree}` 在 Windows 下不可靠；改 `execFileSync` 参数数组后双 CLI 通过。另一个 verification 夹具缺 verifiedCommit，也被自检抓住。
- **near-miss**：R6 后 summary 将 enforce-scope 自检写成 20 项，verifier 独立复跑实测 19 项并作为 ADVISORY 返回。没有功能影响，但说明核验确实在重算数字，而不是抄回执。
- **留根 F15**：`git -C<path>`、`--work-tree=<path>`、`cd <树> && git add` 仍可绕过 Bash 同树检查。合并闸的 Git 重算和独立 verifier 仍在，但不得宣称 hook 是完整沙箱。

## 4. 优化建议

1. **P9 直接作为首个 scripts 新通道试点**：载体 = 后续批；实测主线程代写/逐字块/装配副本均为 0，reviewedTree==verifiedTree，旧 SHA 与越 manifest 能拦。
2. **收紧 Bash 三种绕过形态**：载体 = 后续小批或 ARQ 候选；`-C`、长选项等号形与 `cd && git` 应在同树检查中显式处理，并补消费层反例。
3. **保持常驻真实 Git 仓夹具**：载体 = 已完成，本批落在 pre-merge `--self-check`；后续改闸不得退回只测解析函数。
4. **首个 hooks 批单独记录加载与副作用**：载体 = 后续批；不得拿 P9 scripts 结果宣称 hooks 已实证。

## 5. 一句总结

本批真正的降本不是少做检查，而是取消主线程代写和文本复制对账，改为真实实现 + 独立 reviewer + 同树 verifier；**保险丝判定：两方独立结构接住作者偏差**——R6 找出残留反事实文面，verifier 独立纠正计数并验证敏感权限面无夹带。最值得带走的一条：权限问题先跑真 hook / 真 Git，再写制度文字。

## 6. 本环遗留

- P9 首个 scripts 试点与默认翻转三条件尚未发生；本批不宣称新通道已日常实证。
- 首个 hooks 新通道实证未做。
- F15 Bash 非完整沙箱留根，后续专项收紧。
- `ARQ-281028-mutation-fixture-consumer-layer` 保持 deferred，待 P9 实证后结项；`ARQ-281028-assembler-import-closure-and-root-param` 不消化，旧 lane 仍需要。
- verifier ADVISORY-1 计数已更正；其余三条为预期 advisory、F15 留痕与引据生成件说明，无未闭合 BLOCK。
