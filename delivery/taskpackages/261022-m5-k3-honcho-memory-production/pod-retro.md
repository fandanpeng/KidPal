# pod-retro · 261022-m5-k3-honcho-memory-production

> 第一人称热记忆（planner，2026-10-23，K1/P5/P2 同会话续跑 K3）。只记过程体感：卡点、绕轮原因的主观判断、别扭的规则、下次改进。tokens / gates / rounds / verdict / test results 以 machine artifacts（verifier-evidence / pod-events / verify-runs）为权威，本文不复述。

## 卡点与绕轮

- **acceptance-author 派发被 API 429（周配额）拦腰**：冻结相位主体工作（两验收件 1741 行）其实已落盘，只是收口回填没做。恢复后我以 planner 身份接手了回填（红基线实跑、acceptance 七条目、redBaselineResult）——这半截是流程意外下的代笔，非常态。体感：429 中断的 agent 不留任何回执，只能靠 git status 考古盘点痕迹；**下次遇到中断先盘点落盘痕迹再决定续派还是接手**，这次走对了，但盘点花的心思本可省——若派发模板约定「中断前先落进度注记」会更省。
- **redBaselineResult 里我自己写错了转绿目标**（296 vs 296+17=313）：冻结时点 passed 计数和红转绿后的全量数字是两个口径，builder 当场指出口径差。verifier 若机械对字面会空红一轮。教训：**回填「目标数字」类字段时把口径算式写进原文**（这次修正就是这么写的），防下一手误读。
- **verifier scoped r1 唯一红 = security 闸要 securityEvidence 字段**：拆包 Draft 态没人填这个字段（acceptance-author 回填范围、拆包模板都没列它），闸红后我读了 security-check.mjs 判据、planner 补写即消。这不是实现缺陷，是**拆包模板与闸判据的字段清单没对齐**——gateProfile 声明 security 的包，Draft 态就该有 securityEvidence 占位或闸对该字段缺失降 warning。候选治理小改（走 /gov-change，不归本包）。

## 顺与不顺

- builder 一轮全绿（49 tool-uses，acceptance/unit 一次过），中途唯一插曲（manifest note 含 `[` 致 YAML 解析错牵连 9 既有红）自己发现自修，没烧 verifier 轮。K3-E6 声明式 patch 工作台复用 261017/P1 同形制是本轮最省力的决定——先例复用第三次验证有效。
- front_app.py 零触碰符合预期（P2 已把 payload 单源做实，E2 must-stay-green 现状即绿）——拆包时「若需才改」的判断兑现，避免了无谓扩面。
- integrator 一轮 + 一张小补票（verdict/verification final 投影漏晋升）。advisory 明说「交 planner 用脚本补产」但**并没有补产脚本**（K2b 也是 integrator 手做投影）——advisory 措辞与实际机制不符，下次要么真落个投影脚本、要么把措辞改成「退回 integrator 晋升」。

## 治理体感（不改门，只记）

- brief-inputs 对裸 token ref（NFR-PRIV-1 / TECH-*）报「文件不在磁盘」连续第三包误报，advisory 噪声稳定存在但无害——若某天真有 ref 腐坏，报警可信度会被这种常驻误报稀释。
- dispatch-stamp 的 transcript 派生模型显示 glm/deepseek 映射，与本会话模型不同——如实记账，不懂就不评。
- loop-retro 阈值到了（30 ≥ 3）——已在回执中报 human，发起归主线程，本包不背。

## 下次改进（给自己）

1. 派发模板加一句「被中断时把已落盘进度写进包目录 progress-note」。
2. 回填数字字段时附口径算式。
3. 起包 checklist 增加「gateProfile 含 security → Draft 态预置 securityEvidence 骨架」候选（走 /gov-change 裁）。
