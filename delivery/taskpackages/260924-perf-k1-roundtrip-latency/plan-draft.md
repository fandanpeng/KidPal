---
状态:DRAFT(待 architect 评审)
日期:2026-09-24
riskTier: medium(候选)
触契约面:预判否(待 architect 复核)
---

# 260924-perf-k1 语音问答往返延迟优化(10~15s → 目标 ≤5s)

> 问题来源:human 实测「问一个问题要等 10~15s 才有返回」(2026-09-23 晚真机轮)。
> 证据基线:`evidence/latency-baseline-260924.md`(三源日志拆解 + loopback 探针 + 主机状态,优化前数据已冻结)。

## 1. 为什么改(业务语言)

小伴是儿童语音陪伴产品,问答等待 10~15s(天气类 28~31s)远超儿童交互耐心阈值,是当前最伤体验的单点。排查证明延迟不在设备、不在 front、不在引擎 ASR/TTS 主段,而在 **OpenClaw 网关内部**:①推理模型产 CoT 拖长每轮生成(闲聊一句话也要 9s);②agent 多轮工具循环(课表 3 轮、天气 3 轮);③每条消息 2.7~6.1s 的 preflight(会话装载 + slow SQLite + 2GB 主机 swap 抖动放大)。

## 2. 往哪改(技术方案,分四步、按杠杆排序)

### P0 运维止血(当晚立即,零代码)
- 重启 `openclaw-gateway`(清 3 天累积的内存压力/RSS 600MB;日志自身建议如此)→ 重跑探针,量化「主机劣化」分量。
- 重启引擎 docker(可选,同窗口)。
- **不动** qqbot 通道、不升 OpenClaw 2.0.4(夜间不引入新变量)。

### P1 模型切换(最大杠杆,config-only)
- 主案 A:voice 默认 agent 切 **`deepseek-chat`(非推理)**:`modelPolicy.allow` 增补 + `agents.defaults.model.primary` 改指。预期单轮生成 8~9s → 1~2s。
- 备案 B:若探针证明 `deepseek-v4-flash` 支持关 thinking(请求级参数或 catalog `reasoning:false` 生效),优先 B(同模型去 CoT,质量损失最小)。
- **探针裁决**:两案各跑标准 3 题 ×3 次,比总耗时 + 答复质量(display 块形状、备注读回、儿童语气),数据说话后定案。
- 落仓防漂移:`openclaw.json` 的模型选择现状**不在仓内跟踪**(patch 链只管 char-stroke dist)——本包把模型/白名单选择做成仓内声明 + 幂等 apply 脚本(沿 overlay/patch 既有形态,备份 + 回滚一步),杜绝「ECS 上改了、仓里不知道」的诱饵配置陷阱重演。

### P2 轮次与会话治理(第二大杠杆)
- 课表 3 轮 → 1~2 轮:查清 3 轮各在干什么(工具编排 or profile/memory 隐轮),prompt 或工具配置收敛「查课表一次拿全即答」。若需动 system prompt → **v7 bump**(版本化演进,v6 不回改),只动必要段。
- 会话历史累积:引擎 session 全天恒定,OpenClaw 侧历史只增不减 → 评估 voice 通道的历史上限/定期 reset 策略(compaction 配置),压 preflight 与 prefill。
- preflight 仪表化:开 debug 级日志跑一轮探针,把 2.7~6.1s 拆到具体阶段(session load / context build / sqlite),再决定治理动作——**不盲改**。

### P3 主机容量(human 决策点,不阻塞当晚)
- 2GB RAM + 790MB swap 已用是方差放大器(同题 7.6s vs 17.3s)。建议升 4GB(月成本增量小);当晚以 P0 重启缓解。→ 出 decision packet 条目,晨起 human 拍板。

## 3. 改动多大(量级)

- ECS 运维动作:gateway 重启、openclaw.json 两处键 + 备份(可一步回滚)。
- 仓内:`server/deploy/` 新增模型声明 artifact + apply 脚本(或扩 openclaw-patch 链);可能 prompt v7(视 P2 裁决);探针/验收脚本 `server/tests` 或 `server/acceptance` 新增。
- **不动**:firmware/**、docs/contracts/**、引擎 .config.yaml 的 LLM 段(model_name 仍 openclaw/default)、front。
- 触面评估:server 部署面 + ECS 运维面,跨系统但非结构性(协议语义零改动)→ 预判 Track B / dev taskpackage,不触契约门(待 architect 复核 display 块与 §四 约束不受模型切换影响)。

## 4. 验收口径(冻结候选,数字待 architect 校准)

- **A1 探针门**(offline,当晚可判):标准 3 题(闲聊/课表/天气)×3 次直连 OpenClaw,优化后中位数:闲聊 ≤4s、课表 ≤6s、天气 ≤10s(基线 13.1/12.5/18.7s)。
- **A2 质量守卫**:课表/天气答复仍含合法 ```display JSON 块(薄载荷形状)、课表答复含备注读回(S-F1.3 AC1/AC4 口径不回退)、儿童语气抽检。
- **A3 引擎日志门**(晨间真机轮,human 持板):`大模型收到用户消息`→`发送第一段语音` 闲聊 ≤5s、课表 ≤7s(含 TTS 首句 1.2~2s)。
- **A4 回滚验证**:openclaw.json 备份一键还原 + 探针复测回到基线形态。
- mutation 自证:验收脚本先在基线配置上跑出 RED(超时判定生效),再在优化配置上 GREEN。

## 5. 风险与开放问题(请 architect 重点裁)

1. **质量回退风险**:非推理模型对 v6 prompt 的 display 块纪律/备注读回遵循度是否够?A2 守卫是否足以兜底,还是该保 B 案(v4-flash 关 thinking)?
2. **qqbot 共 agent**:modelPolicy/primary 改的是 agents.defaults——qqbot 通道是否共用同一 agent、要不要隔离(per-agent override)?
3. **P2 是否入本包**:prompt v7 + 会话策略改动面更大,是本包一次做完,还是拆 k2 后续包(本包只做 P0+P1,晨间先看大头效果)?
4. **preflight 未知数**:2.7~6.1s 尚无阶段级拆解,P1 之后若 preflight 成为新大头,P2 仪表化是否升级为必做项?
5. **门数字校准**:A1/A3 阈值是拍的,请按 DeepSeek 非推理模型现实速度校准(过紧假红、过松无感)。
6. 探针在 default agent 上留会话痕迹(与真机轮同通道)——是否需要独立探针会话隔离,避免污染儿童会话历史?

## 6. 执行编排(确定方案后)

建 TaskPackage(本目录)→ kidpal-pod-launch-prep(worktree/branch/board)→ dev-loop(acceptance-author 冻齿 → builder → verifier↔fixer → integrator)→ 当晚 P0+P1 落地 + 探针验收 → 晨间 human 真机轮 A3 + P3 拍板。
