# pod-retro — 260928-tech-260928-school-start-upcoming-empty（第一人称热记忆）

> planner 第一人称过程体感，非权威事实源——tokens/gates/rounds/verdict/test results 以
> machine artifacts / pod-events / verifier-evidence 为权威。

## 卡点与体感

1. **我自己的 Write 连续出毛病的这段最扎眼**：TaskPackage 起包阶段我连续三次把
   taskpackage.json 写坏（字段漏写 exitCriteria、sourceType/origin.type 用了闭集外的
   自造值 crq-tech-entry）。机械闸 taskpackage-required 一轮轮咬住不放，我修正一轮
   才发现「我写的」和「我以为我写的」不是一回事——闸是唯一救了我的东西。教训落成
   习惯：**起包落盘后立即机检校验**（node taskpackage-required-check.mjs），不要攒到
   verifier 相位才撞闸。
2. **字段的隐式耦合比想象多**：origin.type 解析优先 upstreamType，且 resolved 值必须
   等于 sourceType；mutationRequired=false 必须带 mutationSkipReason。这些规则不读
   脚本根本不知道，闸的红字信息其实写得清楚，第一轮就该去读闸脚本而不是猜。
3. **caller 派单与脚本现状有一处脱节**：派单说 dev INDEX 用
   `changes-index.mjs --upsert`，实际该命令只管 arch lane（dev lane = closeout-check
   `--update-index` 刷行）。我按脚本现实走 closeout-check --update-index 完成落账，
   与派单字面不符但与仓库机制一致，如实留痕。
4. **FS-EVIDENCE-3 抓到 integrator 漏 docs-delta.md**：第一轮 integrator 只产了
   review-packet。退回补收口（单独小 commit 4792f00）一次过。机械清单的价值再次
   兑现——「制品齐」不靠 integrator 自评。

## 顺畅的部分

acceptance-author 行为级钉齿质量很高（真跑推送路径 + 断言设备可见载荷 = bug 症状
本体，还主动补了配置缺失降级边界齿）；builder 补丁 2 hunk 零外溢一次绿；fixer 全程
未出场（零 ErrorEnvelope）；verify.mjs scoped round 3 一次全绿。

## 下次改进

- 起包即机检（写 taskpackage.json → 立即跑 taskpackage-required-check.mjs → 绿了再
  派 acceptance-author）。
- 派发前把闸脚本的闭集字段规则先读一遍再填字段，不凭记忆与模板。
