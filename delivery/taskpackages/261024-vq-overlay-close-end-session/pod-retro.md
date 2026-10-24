# pod-retro · 261024-vq-overlay-close-end-session

> 第一人称热记忆（planner 全程在场的 subjective 体感）。**不载** tokens / gates / rounds / verdict / test results——这些以 machine artifacts / pod-events / verifier-evidence 为权威。Post-cutoff：escalation themes 段 cross-ref pod-events。

## 过程体感

- **最大的卡点是制品接口形态不匹配，不是代码**。三轮 scoped/红轮里没有一轮是 fixer 的活——红因全是验收件与 TaskPackage 侧缺口（coverage 标记缺、mutationEvidence 空骨架、scope-file 叙事形态）。fixer 全程未出场，这对 regression 包合理，但说明「红轮 = 退 fixer」的直觉在本包不成立：先看红因落在谁的写权面再路由。
- **mutationEvidence 骨架是我在起包期就该填的**。起包格式期望前置明确列了「mutationEvidence 骨架」条款，triage 落盘留了空骨架，我 finalize 时只回填了红基线结果、没把 perAcceptance 的红基线 mutation 计划一并写进去，导致 round 2 被该闸打回、我事后用 r1/r2 evidence 补写。下一包：finalize TaskPackage 时 perAcceptance 至少带「mutant 面 + 预期红形态」计划条目，不留给验证期返工。
- **scope-file 的消费接口要写进 integrator 派单**。integrator 产出叙事 md（detect_changes 对照表很完整），但 unit/acceptance 闸的 readScopeList 只吃纯路径行清单，final r4 被防 typo fail-closed 打回。我代转写了 scope-list.txt（1:1 投影、留痕注释），但正确做法是派单时给格式样板（或要求双产出：叙事 md + 机器清单）。这类「producer 语义对、接口形态错」的摩擦，派单模板一句话就能免掉。
- **verify round 序数被 fail-closed 举手占号**这件事第一二次遇到容易懵：缺 scope-file 的举手也会写 fact、占掉序数，重跑要从下一个序数继续。行为合理（immutable fact 流水），但值得记住——举手轮 ≠ 白跑。
- **r3 firmware-build 的瞬时 degraded**（工具链探测抖动、buildAttempted=false）拖到 final 才被 integrator 点名成 F-1。final 相位带 VERIFY_FW_BUILD_REQUIRED=1 重跑即真编译闭环，无实质问题；但若 scoped 轮就显形会更早消。环境抖动的降级放行是诚实的，代价是把闭环压力后移——可接受。
- **acceptance-author 的 advisory 备读机制很好用**（RC-A2 头注措辞滞后由它自己申报、我追加一轮顺手修注），比 verifier 抓住再返工省一轮。
- **真机门停等设计顺畅**：launch-card 把「到门即停」预期写死，本相位没有犹豫空间；status 闭集缺 awaiting-human:real-device 的老 friction 仍在，按既有口径退 blocked + 事件承载（本次照办）。

## Escalation themes

- 本包零 agent-escalation（pod-events 仅有 3 条 over-scan friction：acceptance-author 4 条 / builder 6 条 / integrator 3 条超读申报，纯记账）。
- 收尾将追加 1 条真机门停等事件（category=environment，human 持板资源门），resolution pending human。

## 下次改进（可执行）

1. 派 integrator 时附 scope-file 机器消费格式样板（纯路径行清单 + `#` 注释），或要求 narrative + list 双产出。
2. finalize TaskPackage 时把 mutationEvidence.perAcceptance 的计划形态写齐，不留空骨架进验证期。
3. regression 包的红轮路由先问「红因落在谁的写权面」，acceptance 件侧缺口直接回 acceptance-author，不进 fixer。
