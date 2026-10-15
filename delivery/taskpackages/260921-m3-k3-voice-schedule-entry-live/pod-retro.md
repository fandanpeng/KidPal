# pod-retro · 260921-m3-k3-voice-schedule-entry-live（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts / verifier-evidence / pod-events 为权威，本文件不复述。escalation themes：本包零 agent-escalation 事件（pod-events 交叉引：human-decision `b3de38db` 设备轮完成续跑指令、friction over-scan ×2、dispatch ×3）。

## 卡点与体感

1. **这一棒的形状是「offline 半程 + 真机半程」两段式**，最特别的决定是 A3 两齿设计态红跨相位：offline 阶段就停在 12/14，我派 builder 时显式写了「不要试图让它们转绿、不要预写 device-round-record.md」——防线提前到派发 prompt 而不是靠闸兜。事后看这个提前钉防线的动作是对的：两个 subagent 都没有试图造假绿，A3 齿一直红到真机轮制品落盘才转绿。**「预期红」必须在派发时就说破，否则子 agent 会把「全绿」当默认目标。**
2. **851.5s 归因更正是本棒最有价值的纠偏**。我转述 coordinator 素材时把串口 851.5s 的 `view=home upcoming_apply ok` 归因为「三触发③录入后即时推送」，integrator 做三源时间戳对账后发现该时点早于录入实证、实为 farewell 轮同拍——二手目击素材里埋着归因偏差，幸好 integrator 没有照抄我的转述而是按机器证据改写。**教训：human 口头目击与机器日志的对应关系，进记录前必须过时间戳对账，转述链每多一手就多一分归因漂移。**
3. **current/verdict.json 的修改我第一反应是「谁动了指针面」**，读了 diff 才确认是 verify.mjs 自己 fs-write 的最新 verdict 副本（scoped r1 产物）。「先核实再处置」这次又省了一轮误回滚——共享投影面上的机器再生文件，先查 producedBy 再定性。
4. **trigger=immediate 机器日志缺位是本棒最大的未闭环**（review-packet SHOULD）。human 目击 + 37→40 门数实证都在，但 journalctl 全量回溯零命中——M3K1-A5 的 ③ 半只能按「human 目击 + 实证 + 机器日志缺位」的程度如实收口，没写全闭环。这是 honest-degradation 的正确做法，但也说明 K1 部署面（drop 入口挂载/目录一致性）有笔欠账等着还。
5. **scope-map 的 planner 边界这次完全没硌手**：device-round-record.md 归 integrator 落（其 allow 覆盖 delivery/changes/dev/**），我只需要在派单里把素材和如实标注纪律交清楚。前几棒验证过的「验收归 acceptance-author、taskpackage 回填我代落」分工，这棒加上「设备轮记录归 integrator」，边界越来越顺。
6. human 分层出场继续成立：launch 授权 + 设备轮持板 + 续跑指令三次，loop 中途零打断。

## 别扭的 acceptance / 帮倒忙的钩子

- A3 落地齿钉死 device-round-record.md 路径——先例口径、无摩擦，acceptance-author 的「提前知会」提醒用上了（直接写进了 integrator 派单）。
- loop-retro 阈值 advisory 到 17（14 pending + 3 waiting）——发起责任在主线程/human，收尾报告提请（K2 retro 就提过一次，继续欠着）。

## 人工纠偏体感

- human 的续跑指令里带了一条重要的树池提示（分支上 K4 立项 commit 随批进 main 属预期）——避免我把 40ce8c7 当异常升级。**协调方对共享分支的并发改动主动报备，这个模式值得保持。**
- 「合并批准凭据另行转交，勿自行宣称已批」——边界钉得准，我停在 supervisor gate 等凭据。

## 下次改进

- **二手目击素材进设备轮记录前，先跑一遍「机器时间戳 ↔ 口头归因」对账再定归因**——这条应进 kidpal-serial-monitor 的记录纪律（素材转述者与记录撰写者分离时尤其）。
- 派发 prompt 里给 subagent 的「事实素材」要标注来源可信度分级（human 口述 / 机器日志原文 / 我的转述），让记录者知道哪段可以照抄、哪段必须重验。
- current/ 投影面的非常规修改：先读 producedBy/ts 再定性，别急着当缺陷上报。
