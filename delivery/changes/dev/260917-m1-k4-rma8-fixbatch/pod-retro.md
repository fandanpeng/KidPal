# pod-retro · 260917-m1-k4-rma8-fixbatch（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts（verifier-evidence/rounds、pod-events、taskpackage、verify-runs）为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 实读），friction 条目 cross-ref 同文件 pod-events.jsonl。

## 卡点与体感

1. **两轮 verifier 红都是「实质对、语法不对」的形态税**：r1 coverage 闸红在我让 acceptance-author 用 prose 写 coverage map（「FX-A4 -> TEST 6」）而闸只认 `coverage: FX-A4` marker 语法——落位声明实质完整，但没写成闸可机读的形态。r2 mutation 闸红在我转写 mutationEvidence 时只写了短 id「FX-A1」，而闸的 mentionsId 是拿 raw **全串** acceptance id 做子串匹配——K4 包 retro 第 1 条明明写过同款（「整串原文甩在 missingSensitive」），我还是踩了，第二次才对齐。教训内化：**K4 retro 的改进项「finalize 硬前置自查」我没有真正执行**——这次起包 checklist 我核了 scope-map、current 指针、briefs，但没核「mutationEvidence record 形态须含全串 id + 敏感信号」。retro 改进项要变成动作清单，不是读一遍就算。

2. **豁免通道的裁定我做了一次 taskpackage 侧、一次验收文件侧，边界体感是清晰的但值得留痕**：FX-A2/A3（真机三方互证/不变量靶）在 Pod 内确实非代码可测，我以 planner finalize 权改 taskpackage 打 kind:structural——这不是给闸放水，是把「deferred 真机」这个冻结时已声明的事实翻译成闸的机读形态；FX-A4/A5/A6 有真实 host 半，走验收文件 marker + suiteGate，让 acceptance-author 补。事后看这个分流是对的：凡有代码承载的都留在了可变异、可红的 functional 侧。

3. **补证轮是本包唯一「额外烧掉」的派发，但它买到了真东西**：FX-A5b 的 TEST 7 tooth 4/5 在 K4 批没有直接变异证据，builder 补证时发现 TEST 7 是返回式断言、同 TEST_CASE 先红即早停——单次变异只能证一颗齿，于是拆成两次独立变异各取证。这个「早停导致链式齿只验首齿」的盲区是 K4 批就存在但没有暴露的，这次被补证轮挖出来了，值得登记给 test-quality 闸的后续治理（返回式断言链齿的敏感性覆盖问题，非本包义务）。

4. **builder 的 deviceBuildCaveat 让我虚惊一场但方向是对的**：builder 声明本 Pod host-only 无法设备面编译 lcd_display.cc、建议 verifier 复核——我担心 firmware-build 闸会因此红。实际闸用 Windows 侧 ESP-IDF 工具链真编译通过（工具链在宿主不在 WSL）。体感：builder 如实声明「我没证到的」而不是静默，这种降级声明是纪律红利；下次 similar 情形我可以直接预判闸会兜住，少一层焦虑。

5. **FX-A7 截图缺口的归属裁定比预想顺**：K4 review-packet 已登记截图缺口未补，本包无新 UI 面，裁定「随 RM-A8 确认轮顺带补、不拆独立批」——human 持板轮本来就要取证，顺路成本最低。integrator 如实落了去向声明，没有静默丢。

## 别扭的 acceptance / 帮倒忙的钩子

- coverage marker 正则只认 `coverage:` 前缀形态、不认 prose 同义表达——门义是收紧的（260731 批有意去 prose 洗白），我接受这个代价，但「落位声明写两遍（prose 人读 + marker 机读）」的重复感是真的，或许治理批可以考虑 marker 行允许承载完整落位语义从而免掉 prose 双写。
- changes-index.mjs 无 --help 分支，integrator 想看用法反而触发了全量重建、顺带纠偏了 4 行历史 INDEX（k2 行状态判 open 与 board merged 有表观分歧，integrator 以 NIT-1 登记未深查）——工具的意外副作用面值得收，且 k2 INDEX 分歧留给主线程/上游核。
- universal.bashDenyWrite 连 integrator 清理 input-brief EOF 空行都拦——拦截正确（taskpackage 面是 planner 写权），只是 whitespace 级别也被拦略显粗，不追究。

## 人工纠偏体感

- 本包零 loop 中人工纠偏：launch 卡指示免人工交接、全程未触发 AskUserQuestion，升级链一次未用。riskTier=high 的 supervisor 合并门会升级人类，那是设计内的门、不是纠偏。

## 下次改进

- **起包 checklist 加第 9 条**：mutationEvidence record 形态自查（acceptanceId 用全串原文 + result/sensitive 敏感信号），finalize 时逐 record 对着 mutation-check 的 mentionsId 语义过一遍——这已经是 K4→本包第二次踩，第三次不能再靠闸抓。
- **coverage marker 与落位声明一次写对**：派 acceptance-author 时直接把 marker 语法示例（`/* coverage: FX-Ax -- … */`）写进派发 prompt，省一轮 verifier。
- **structural/suiteGate 豁免分流前置到 freeze 派发**：这次是我 r1 红后才裁定分流；下次 freeze 派发时就让 acceptance-author 按「host 可机判 / 真机 deferred」预分好，taskpackage 对象条目随 finalize 一次成型。
- TEST 7 返回式断言链齿早停问题 → 登记 test-quality 治理候选（NIT，非本 Pod 义务）。
