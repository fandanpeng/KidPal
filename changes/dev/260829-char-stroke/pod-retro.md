# Pod Retro — 260829-char-stroke（planner 第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/rounds/verdict/测试结果（权威在 machine artifacts / pod-events / verifier-evidence），只记过程体感。

## 本 Pod 最有价值的教训

**spec 记载 ≠ 外部数据集真值——offline fixture 是自造的，会在错误假设上全绿**。spec §3.0.2 写着 dictionary.txt 含笔画数，acceptance-author 据此造了带 `strokes` 键的 fixture、冻结断言拿 `dic.get("strokes")` 对照——全绿；真数据集（9574 字实读）根本没有这个键，live 一跑：warm 静默拦 9574 字（wrote 0）、live 断言恒 None 对照必红。这与上包（g-relay-fix）的「wire 形态必须对照对端解析器」是同族坑：**凡断言打到外部世界（数据源/对端解析器/第三方部署），输入清单里必须有外部实况的抽样证据**，fixture 至少一条按真记录形状造。这轮是部署期才炸，多花了 acceptance-author 一轮裁定 + builder 一轮 diff。

## 卡点

- **auto-mode 分类器拦变异探针**——mutation-check 闸要求真实变异→齿红→还原，分类器把「改实现→跑测试→还原」判成造假绿，方向完全颠倒。AskUserQuestion 一次拍板后继续。这是分类器第二次在本仓 dev-loop 的合法工作流上误伤（上包拦 ECS 操作），已成本模式：**凡「看起来像攻击」的验证动作，派发/执行前先落 human-decision 事件背书**。
- **部署制品的三轮往返**：生成器假设源码 checkout、实际是 npm dist bundle（锚点全变）→ 锚点注入到函数定义体（作用域无变量，运行必炸）→ 单行锚 vs 多行 tab 实况。每轮 4-10 分钟。「写部署生成器前先探测目标形态」这一步在 launch prompt 的环境事实里没有，吃了三轮。信封 bug（缓存值=data 层没包 view/data）是第四轮，属于 TS 镜像与 python 契约语义对照不严——镜像类制品每次改完应逐 emit 点对照冻结契约语义。
- **live 延迟的措辞考古**：169s 冷启动的元凶不是坐标复述（C1 已解），是 v2 措辞「data 原样使用查字工具返回的完整结果」诱发 agent 真去找工具（16 轮 LLM 调用）。改成「只出最小意图块、出口自动替换 + 一回合作答纪律」后收敛到预算内。教训：**prompt 里的机制描述如果暗示「有个工具等你调」，agent 就会去调——机制描述必须写成「你绝不需要调它」**。
- **小坑**：后台命令链 `cd` 后 shell cwd 残留，pytest 两次空跑才反应过来；SendMessage 排队丢失第四期未犯（上包的「completion 后核对」操作有效）。

## 顺的部分

- 闸 A 干净利落：architect-reviewer 判 ④ 无需改契约（C1 只改数据生产方式不改 wire format），契约门零出场，一次预审全销账。
- Explore 对 openclaw-main 机制排查一击锁定机制 B（HTTP 路径无回复改写 hook、skill 纯 prompt、tools 拒收根因 ≈ 版本 <2026.5.12），省了至少两轮试错。
- acceptance-author 的 CHR-A4 修齿是本 Pod 最漂亮的一手：否决 builder 的 flat compound 候选（数值反推证明是假绿），改单调用五角星真钉 even-odd vs nonzero——「验收文件自相矛盾」处置成为弱符号先例的又一次健康运转。
- 变异探针 8/8 全红全还原、sha256 纪律全程零漂移；两处首探不敏感如实入 notes（bezier x 项 / pinyin join），没有为了「证据好看」硬凑。
- live 三齿一次过；生产部署全程备份 + 漂移闸守住（生成器拒改错误版本的行为两次真实触发）。

## 下次改进

1. 外部数据源验收：acceptance-author 输入清单固定加「真记录字段抽样打印」；offline fixture ≥1 条按真形状造。
2. 部署类制品派发模板加前置步骤「先探测目标形态（bundle/source、锚点 grep、ESM/CJS）再写生成器」。
3. 变异探针的 human 背书前置化：launch prompt 或派发模板声明「planner 直跑探针循环 = human 预批动作」，省一次拦截往返。
4. OpenClaw agent loop 冷会话多轮 bootstrap 的平台级延迟（~2.77s 热底线 vs 数十秒冷）值得单独 ARQ——与本包机制无关但直接限制 C1 的延迟下限，挂 architect 定方向。
5. dist 补丁的版本漂移闸已进生成器（真实触发两次），但「补丁后 dist 的启动自检」（网关起来后探针一句确认补丁在位）还是人眼——可进 runbook 自动化。
