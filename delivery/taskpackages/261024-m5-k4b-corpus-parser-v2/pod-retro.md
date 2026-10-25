# pod-retro · 261024-m5-k4b-corpus-parser-v2

标签:第一人称热记忆(planner 主观过程体感;tokens/gates/rounds/verdict/test results 不载,以 machine artifacts / pod-events / verifier-evidence 为权威)。

## 卡点与体感

- **HOLD 时序的运气与教训**:human 的 HOLD 指令到达时 fixer 恰好完成——补丁留工作树未 commit、零 live 触碰,后来被 architect review 裁定 KEEP 为基底,一次返工都没烧。但如果 fixer 慢十分钟,HOLD 就会拦在半路留下半成品;下次收到 live 实证类修复指令时,对「主线程诊断」保持一个呼吸的怀疑是值得的——本轮对抗 review 把主线程方案的两处硬伤(只修 role 治不到主症状 / transcript 批次不可复现)全数推翻重裁,证明 HOLD 价值极高。
- **fixer 的包装标记裁定是本轮最关键的一次自由裁量**:冻结数 2879 在「剥离」语义下不可达(243 个载体段会变空),fixer 用 reviewer 自己的探针逐值核对后选了「保留只计数」变体,并把语义与键名字面不符(wrapperMarkersStripped)双侧注释钉死——验收 author 随后按同一语义构建期望,一轮全绿。教训:冻结锚定数是解析语义的试金石,实现前先问「这组数在哪种语义下可达」。
- **acceptance 的 A3/A4 机器相关性是新形态**:孩子语料不入仓(隐私红线)导致快照审计天然绑定机外路径,验收件用「缺席 skipTest 显式申报 + sha256 冻结防静默漂移」承载——比「要么入仓要么不测」的两难好,但这是本仓第一个依赖机外资产的验收测试,后续包引用此形态时记得申报依赖。
- **verify.mjs 的 --scope-file fail-closed 是好摩擦**:final 相位直接红让我必须显式产出圈定清单,而不是默认全量回归——本包 5 文件影响面自包含,清单 30 秒出。

## verifier↔fixer 绕轮原因主观判断

- 零红轮、零 fixer 返工——review 方案够精确(fixable 判据逐值冻结),fixer 与 acceptance-author 各一轮到位。流程摩擦只剩 Bash 重定向写 scope 文件被钩子拦(改用 Write,正确行为)。

## 人工纠偏体感

- 两次:human HOLD(改派发计划)+ human D1-D5 终裁(改技术方案)——均为 live 实证驱动的正确干预,非误工。

## 下次改进

1. live 实证 bug 的修复指令,派发前主动问主线程「要不要先过对抗 review」——不要等 HOLD 到达才停。
2. 解析类实现,先把「冻结期望数的语义可达性」验一遍再动工。
3. 机外资产类验收(快照审计)的申报模板已经成形(K4b 验收件可作先例),下个包直接引用。

## escalation themes(post-cutoff)

- 本包 pod-events:human-decision 两条(前包 HOLD `349ccf15` 归 K4 账面、本包无新升级);friction 两条均为 over-scan 纯记账;零 agent-escalation。
