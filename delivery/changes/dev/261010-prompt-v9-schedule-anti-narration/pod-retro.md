# pod-retro · 261010-prompt-v9-schedule-anti-narration（第一人称热记忆）

> planner 全程在场的**过程体感**。非第二事实源：不复述 tokens / gates / rounds / verdict / test 结果——那些以 machine artifacts / pod-events / verifier-evidence 为权威。标签：第一人称热记忆。

## 本包最大的顺畅点：pretext 齐的包跑起来是真的快

launch-card 的「intake 特别注意」六条 + 1f 核验 + 红基线锚引用（server 面零差异直接引用锚行不重跑），让我从 intake 到 builder 派发之间零停顿。上一包 retro 建议的「`mutationRequired=false` 须 `mutationSkipReason`」已经在拆包时预落实（taskpackage:80 在场），1f 只确认、不用修。**拆包期把可预落实的前置都做掉，Pod 侧就只剩执行**——这个正向循环值得保持。

## 我自己的两次写权口径误判，机械闸两次都对

本包我是两次派单时写权声明与机判不符：第一次给 integrator 声明「arch 可经 universal docsTopLevel 写」，实际 enforce-scope 按 agent 枚举拦下；第二次给 architect 声明「delivery/changes/dev 可写」，同样被拦。两次都是机械闸正确、我错，且都按既有纪律止步未绕行。**教训很具体：scope-map 的 universal 段与 agents 段是两套口径，派单写权声明只看 agents 段枚举**；两次拦截浪费了各一轮追加派发。我在派发书硬约定②下的「前置核对」动作做了，但核对的是 universal 段——核对动作本身要改：**以 agents 段枚举为准，universal 段只做背景**。值得进 CRQ：两套口径并存且 prose 无处明示「agents 段优先」，下一个 planner 还会踩。

## architect handoff 通道运转良好，但入口是我误触发的

integrator 被拦后我按 ARQ-260929 先例派 architect 落笔 spec 面，零偏差一次过，:150 NFR-SAFE-1 的 v7 残留也是 architect 自己捞出来申报的。体验好，但复盘看：**如果我在 1c 就以 agents 段枚举核对写权，这次 handoff 根本不需要发生**（我会直接在 integrator 派单里把 arch 半边拆给 architect）。handoff 通道是好的兜底，不是该走的常路。

## acceptance-author 的加强值得点名

PN-A2 并集窗口收紧为行级定位（防对话行既有词造成假绿）+ PN-A4 附世系锚抽检，两处都是超出 acceptance 字面的实质加强，且都在冻结件头注申报了理由。这种「按纪律收紧而不请示」的判断质量正是冻结件该有的样子。builder 一次命中零返工，说明齿的选节质量直接决定 builder 效率。

## 帮上忙 / 帮倒忙的钩子与规则

- **帮上忙**：`enforce-scope.mjs` 两次拦截虽然源于我的误判，但拦截本身干净利落、理由明确，agent 都没有试图绕行——机械闸的「可信拦截」价值在这包体现得很典型。
- **帮上忙**：`verify.mjs` final 相位 `--scope-file` 必带 + integrator 圈定的 scope 清单 7 条目全在盘，final round 一次过。
- **略绕**：`dispatch-stamp.mjs` 的 token 政策我第一次调用被拒（`--token-status exact` 无分项值），harness 只报总量时需要显式补 `--token-total`——报错信息清晰，一次学会，但模板里 token 旗标的必带条件 prose 没写死。

## 人工纠偏体感

全程无 human 入场——Lite 包、零契约/需求改动、verifier 一轮绿、integrator/architect 各自闭环，符合「loop 内非 blocking 不请示」口径。唯一 human 在场位是 upcoming 的 ECS 部署 + 真机轮（PN-A5 live 观测项，packet 已标 pending-human），属「human 只花时间在升级门/契约/真机/预算」的正例。

## escalation themes

- 无 `agent-escalation` 事件。
- friction 四条：over-scan 三条（acceptance-author 1 行 / builder 8 行 / integrator 9 行，均为 bounded 申报、纯记账）+ scope-drift 一条（写权口径误判族，见上）。
- loop-retro 触发阈值已到（closeout advisory 提示 pending+waiting 计数达标）——发起责任在主线程，已如实上浮给 human。

## 下次改进（三条）

1. **派单写权核对只看 scope-map agents 段枚举**，universal 段（含 docsTopLevel）只做背景；两套口径的差异进 CRQ。
2. token 旗标模板补一句「harness 只报总量时补 `--token-total`」进派发备忘。
3. acceptance-author 的「纪律内加强 + 头注申报」模式值得在派单时显式授权（本包是我口头给的，可成文）。
