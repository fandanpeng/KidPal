# pod-retro · 261028-m6-k2a-p1-home-16px

> 主线程（planner）第一人称热记忆 · 2026-10-29 · dev-loop Pod（VS Code 扩展会话，与 P0 同会话同树接棒）。

**【R1】verify 全链绿 + integrator 收口后即写；supervisor 闸前落盘。**

**【R2】计量诚实**：子 agent tokens 有计量——acceptance-author 三段（断电/网关中断重续）383k + 404k + 中断前段；builder 489k；integrator ~90k（估）。主线程估 ~100k（±40%）。时长：P1 验收+实现+收口约 5h 有效墙钟（含断电与网关中断两段恢复）。

## 1. 时间账（估）

| 相位 | 内容 | 时长（估） |
|---|---|---|
| launch 面 | 启动卡 + 基线判停（两面零差异引用旧锚） | ~20min |
| 验收冻结 | 三段（断电中断 / 网关错误中断 / 收口）+ architect advisory 轮 | ~3h |
| 实现 | builder step-0 + 实现 + 变异 + 设备 build | ~1.5h |
| 收口 | verify 全链 + integrator + 本 retro | ~40min |

## 2. Token 账

花费最大处 = acceptance-author（~790k 三段，含两次中断重入）。可压缩：无中断约省 30%。

## 3. 摩擦与卡点

- **F1 · P0 链缺陷在 P1 冻结轮才暴露**：现象 = 红基线通道①跑出 PCN RED(P0-chain)，追出 78 fork cbin writer 恒产 LARGE=1 而 host LARGE=0 → 根因 = P0 冻结件只钉返回码不钉度量合理性（假绿）+ P0 真机轮被后置 → 证据锚 = acceptance-author 报告篇首 + architect advisory（设备 LARGE=1 在案，sdkconfig.defaults:57）。**教训成立**：P0 retro 的「未验证前提最便宜一步实测先行」在 P1 兑现为拦截——自诊断红 tooth 0b/PCN tooth 5 把缺陷钉成可诊断形态，architect advisory 一轮收敛修法（仅 host 开 LARGE）。流程运转正常的证据：跨包验收咬合（P1 supersede 设计暴露 P0 假绿）正是冻结件交叉红的预期功能。
- **F2 · 两次环境中断（断电 / API 网关空响应）**：worktree 持态 + 子 agent transcript 续跑，零损失。教训：SendMessage 续跑时先 `git status` 读地面真值再写指令，避免凭记忆重派。
- **F3 · F-9 上游缺口跨端核对**：acceptance-author 主动跨端核对 server producer（[U1] 通道真机永不触发风险）→ ARQ-261029 立账 + human 裁上游补写入侧。教训：消费侧冻结前先核对 producer 实况，防「host 全绿真机零样本」。
- **near-miss**：fixer 首轮判读基线 3 Failures 与实测 4 不符——若机械放行会误判；实际第 4 条是 P1 设计态红（PCN-A11），冻结件头注逐字预测过。判读基线必须按「红名单逐条对照」而非「数量相等」。
- **保险丝判定：接住**——P1 冻结件交叉红拦下 P0 假绿；verify 机械闸拦 mutation 证据格式错（同 P0 教训，本轮一次转录即过）。

## 4. 优化建议

1. **「度量合理性断言」进 host 验收惯例**：治 F1 · 载体 = ARQ 收件箱候选（凡装载二进制字体/图形资产的验收，加 canary 码点盒/步进合理域断言，防「返回 true 即绿」）。
2. **判读基线用红名单不用红数**：登记不硬治（本 retro F4 记录）。
3. 上游写入侧任务分流：待 human 确认后走 /triage-change（ARQ-261029 在账）。

## 5. 一句总结

成本 ≈ 1.37M 子 agent tokens + ~5h，human 触点 1 次（F-9 路由）· 最险一刻 = P0 假绿若未被 P1 冻结件咬出，将带到真机轮才发现 16px 全废 · **保险丝判定：接住**（跨包冻结件交叉红 + 机械闸双拦）· 最值得带走的一条 = 「验收断言要钉度量合理性，不只钉返回码」。

## 6. 本环遗留

- ARQ-261029 上游 ※ 写入侧任务（blocker，真机轮前须落位；待 human 确认派单方式）。
- 真机半（16px 观感终判 + 缺 assets 降级 + 下划线可读性）→ P0+P1 统一真机轮（human 裁定载体）。
- P0/P1 合并门：human 后置裁定——真机轮看过效果后一起合并。
- 存量 p1_auxbar 三红跨包在册；loop-retro 阈值到（主线程择机）。
