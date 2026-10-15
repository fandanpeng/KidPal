# Pod Retro — 260907-g-relay-fix（planner 第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/rounds/verdict/测试结果（权威在 machine artifacts / pod-events / verifier-evidence），只记过程体感。

## 本 Pod 最有价值的教训

**自创 wire 格式是集成陷阱的经典形态**。builder 按冻结契约把 relay 帧包成 `display_relay` 自创类型——结构面全绿、离线 harness 全通，真机上被设备固件一句 `Unknown message type` 扔掉。冻结验收的离线齿只能钉「我们自己说了算」的形状，**跨端 wire 形态必须对照对端的解析器源码钉**（这轮靠翻 application.cc/mcp_server.cc 才发现数字 id、data 字符串化两个隐藏雷）。建议：涉及设备/外部解析器的验收，acceptance-author 阶段就该把对端源码列为必读输入——这轮是冒烟炸了才补的课，多花了一整轮。

## 卡点

- **auto-mode 分类器在子会话拦 ECS 操作**（Credential Leakage / Data Exfiltration 两个形态）——builder 的 transcript 一旦出现过内联凭据痕迹就被终身盯防。处置 = 主线程代执行部署（本会话干净 + user 手工放行）。第二轮直接在派发 prompt 里预防性声明「被拦就停、主线程代执行」，省了硬碰。
- **MSYS 路径转换吃参数**：Git Bash 把 `/opt/...` 形态的独立参数改写成 Windows 路径，SFTP 报远端 ENOENT——排查了一轮才想到。`MSYS_NO_PATHCONV=1` 是正解，值得进 ops skill。
- **SendMessage 排队消息丢失第三次发生**（冻结修订轮 + 实现轮各一次）——续派任务的可靠性问题已成模式，一律按「completion 后核对报告、缺就重发」操作。

## 顺的部分

- architect 的数据流支点分析（引擎流式累积点是双面污染唯一未分叉汇聚点）一击定位，方案 A 的落点、分工边界、A→B 迁移关系全给了，human 一次拍板后零返工。
- 三轮真机冒烟的迭代节奏干净：每轮一个明确假设、串口+引擎日志双源对齐、证据即轮次产物。deepseek 代理下的各 agent 表现稳定。
- mutation-check 对 GRF-A5（evidence-based live 条目）也要突变证据——闸口径比 G 包时代更严了，evidence 落盘→改名→红→还原的套路第二次走，顺畅。

## 下次改进

1. 跨端协议验收：acceptance-author 输入清单固定加「对端解析器源码路径」。
2. 子 agent 的 ECS 操作模式固定为「主线程代执行」写进派发模板（省一轮拦截往返）。
3. 引擎部署副本补丁的启动标记行验证应进 deploy 脚本自动断言（现在靠人眼看日志）——ARQ-260907-engine-deploy-patch-drift 已挂。
