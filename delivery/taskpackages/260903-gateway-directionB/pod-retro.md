# Pod Retro — 260903-gateway-directionB（planner 第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/rounds/verdict/测试结果（权威在 machine artifacts / pod-events / verifier-evidence），只记过程体感。

## 卡点

- **infra 级卡点占了本 Pod 近一半挂钟**：auto-mode 权限分类器在本环境（CC Switch→火山 ark GLM）永久失效。排查走了三段弯路——先误判为 ark 429 过载（16:07–17:12 确有 429 窗口，是干扰项），再误判为「进程内卡死退避」（彻底重启也不恢复才推翻），最后靠 curl 直测才钉死真因：分类器调用带 `thinking:{type:"disabled"}`，ark GLM 全系模型一律 400。终解是 human 把 opus/sonnet 槽位映射切到接受该参数的 deepseek-v4-flash。教训：**harness 内建工具（WebSearch/WebFetch）的失败信息会伪装成网络问题，先用日志+curl 做请求级取证，别信报错的表面措辞**。
- **给已收工 agent 发的排队消息会丢**：两次 SendMessage「queued for delivery at next tool round」都没被 builder 消化，agent 照旧按原任务收工。教训：**续派任务要么在 agent 停止前送达，要么收到 completion 后确认报告里没有新任务再重发一次**，不能假设队列可靠。

## verifier↔fixer 绕轮的主观判断

本包实际没走 fixer——两轮机械红都不是代码问题：①mutation-check 红，因为 portfolio 起草的 taskpackage 不带 mutationEvidence（Pod S 那代是主线程后来补的）；②docs-sync 红，因为 docRefs 里挂了按 V-id 组织的 handoff.json（不含 GWA id）。两个都是 **taskpackage 质量前置问题被机械闸兜住**，闸工作正常。主观建议：portfolio-planner 拆包时把 mutationEvidence 骨架和 docRefs 提及断言的口径预填掉，别让每个 Pod 都在闸上现学。

## 别扭的 acceptance

A2 的结构面是 presence 子串检查（builder 如实报了「prose 注释也命中」），真牙齿在 live wss 可达——mutationEvidence 里标了低敏感性。冻结时如果 live 面权重这么高，结构面断言可以更尖一点（比如锚定特定配置键而非全文 contains）。

## 帮倒忙的钩子/规则

- 8 个闸脚本共享的 CHANGE_ID_PREFIX_RE 不认大写字母，本包 changeId 带 B 直接被误判 pre-cutoff 走旧通道（dispatch-stamp 实证写错文件）。human 批了一次性热修（事件 e9b8c34c）。这个 bug 是**成套复制的**——一处定义八处拷贝，改的时候要全量扫。
- auto-mode 分类器在 builder 子会话里拦公网绑定时要求「human 本人一句话」，主会话转述的裁定不算数——安全上合理，但「转述不算」的口径最好在派发 prompt 里提前声明，省一轮往返。

## 人工纠偏体感

human 三次介入都踩在点子上：token-plan 基址（key 一直 401 时给的，直接解锁了端点定位）、引擎 boot 三选一拍 Docker（避免了依赖闭包泥潭）、公网绑定授权要求原话（builder 权限闸的合理要求）。前期在分类器故障上 human 也陪跑了很久——以后这类环境级故障应该更早建议 human 切手工模式止损，而不是陪着我一路排查到底。

## 下次改进

1. 大写 changeId 的闸正则问题已修，但「闸脚本正则不识别合法 id」这类问题应该在**起名时**就自查（新包 id 起完先跑一遍 dispatch-stamp 干验）。
2. portfolio taskpackage 草案的验收路径要对 scope-map 白名单预检（本包 server/tests/ 错位是 portfolio 侧的笔误级问题）。
3. 会话开始就探测环境分类器可用性（发一条需要判定的探测命令），别等第一个真实写操作才暴露。
4. builder 的 live 实测矩阵（鉴权三连/证书/端口锁）在 prompt 里就明确「先 loopback 验证再要公网授权」，可以少一轮升级往返。

## 升级主题（pod-events 交叉引用）

- 分类器 infra 故障与三段误诊：事件 4d05fb9b（friction/degraded）。
- 闸正则热修：human-decision e9b8c34c + escalation 对 8bd94968/90e5637d。
- 公网绑定权限闸往返：builder 报告 §四（pending human 原话授权，review-packet residual risk #1）。
