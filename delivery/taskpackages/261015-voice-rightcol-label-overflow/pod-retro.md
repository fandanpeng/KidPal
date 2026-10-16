# pod-retro · 261015-voice-rightcol-label-overflow

> 第一人称热记忆（planner 主观过程体感，2026-10-15 重派会话）。非权威面：tokens / gates / rounds / verdict / test results 以 machine artifacts / pod-events / verifier-evidence 为准，本文件不复述不冠权威。

## 卡点与体感

1. **重派消费在途存量这条路走通了**：上一轮被中断的 acceptance-author 产出（568 行冻结件）零改动续用，重派只做「对照冻结边界逐条核对 + 独立冒烟重取证」，没烧重写预算。下次同类重派继续先核后用，别条件反射推倒。
2. **环境前置比想象顺**：TECH-261014 处置口径（拷 host_app/sdkconfig CJK=y + 全量重构建）一次生效，host 套件没有出现渲染齿假红。后台冷构建被我第一次的 `| tail` 管道坑了（输出全程缓冲看不到进度）还撞了后台时限被杀——重启用「直接落日志文件」才可观察。教训：长构建永远落日志文件，不进管道。
3. **mutation-check 元数据缺口是本轮唯一 verifier 红**（r1）：VR-A4 反证齿没落独立 perAcceptance 条目、VR-A5 漏标 structural kind——都是起包时骨架没铺全的债，不是实现问题。VR-A5 的 structural 标注是形态校正非语义变更（原文逐字保留、留痕在 taskpackage），VR-A4 补的条目是诚实取证（三族变异里它确实三次同源红）。起包格式期望前置清单下次要给「反证齿/回归观察面」这类非典型条目留显式模板位。
4. **模型档位环境坑**：builder 默认 sonnet 档经 cc-switch 代理连续 API 400，改 fable 档（kimi-k3 生效）一次通过。这是环境侧降级非选型判断——如果代理侧修了 sonnet 通路，本包这类小改完全够 sonnet。
5. **分支树共享（human 裁定不建 worktree）的噪音代价小但非零**：firmware/sdkconfig 出现一次 idf.py 再生噪音 diff（我还原过一次，final firmware-build 闸跑真编译时又再生了，最终 integrator 以「已验证态=提交树」理由随包提交留痕）。树隔离的成本收益账在纯固件小包上其实很薄，但一次性指示就一次性遵守。
6. **人工纠偏体感**：无。human 派单五条边界（增量核验/在途续用/环境纪律/停门/目录钉死）全程够用，没有中途需要请示的判断点。VR-A6 真机轮是下一站（human 持板），本会话到 supervisor 闸为止。

## 下次改进

- 起包骨架期就把 structural/反证齿类 acceptance 条目的 mutation 证据形态铺对（省一轮 verify 红）。
- 长后台命令一律落日志文件。
- 子 agent 派发 prompt 里工作目录钉死 + 写权预指派这两条纪律继续有效（本轮零写权事故）。

## escalation themes（cross-ref pod-events）

本轮零 agent-escalation 事件。friction 三条：over-scan×2（acceptance-author 4 行 / builder 1 行，均派发指派必需）、degraded×2（builder sonnet 档 API 400 改 fable；firmware/sdkconfig 再生噪音还原）。human-decision 两条（skip-worktree 一次性 / 重派续跑口径）。
