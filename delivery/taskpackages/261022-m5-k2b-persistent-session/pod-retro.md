# pod-retro · 261022-m5-k2b-persistent-session（planner 第一人称热记忆）

> 只记第一人称过程体感（卡点、绕轮原因、别扭的 acceptance、帮倒忙的钩子、人工纠偏体感、下次改进）。不复述 tokens/gates/rounds/verdict/test results——那些以 machine artifacts / pod-events / verifier-evidence 为权威。

- **跑得最顺的一包**：M5 列车三个包下来，本包首次做到 acceptance-author 判法与 builder 实现零绕轮——11 条设计态红一次转绿、verifier scoped r1 与 final r2 均首轮过。对比 K1 的 E2 二选一折腾和 P5 的通道形态扩单，ROI 明显来自冻结判法把落位名、签名、marker 字面、AST 锚全部钉死在头注（FROZEN OFFLINE CONTRACT 四面），builder 没有形态自由度可挥霍。下次复杂接线包沿用这个强度。
- **判法设计里最值的一笔**：marker 探针门控（`_kidpal_provider_supports_user` 失败打 error 不打 marker）。当初 acceptance-author 自主提出时我未预见其价值——bind-mount 部署丢失时 stock provider 静默吞 user kwargs，无门控 marker 就是假绿。这是「验收作者比实现者更懂部署失效模式」的实例，值得保持给 acceptance-author 独立的部署失效推演空间。
- **小坑（我自己造成）**：dispatch-stamp 在子 agent 通知回传前先落了一条 duration/tool-uses=0 的条目，回传后再补实测修正条。P5 也有双条，但成因不同（那次是字段核验 fail-closed 重落）。下次等 task-notification 的 usage 块再落 stamp，单条到位。
- **bookkeeping 纯体力项**：current/ 投影三件（taskpackage/input-brief/verdict）手工 cp/rm，包间重复。量小不值得立批，但若 M5 后续 P3/P4 再出一次「忘删 stale verdict」类手误，应考虑 ARQ 收编成脚本。
- **非缺陷观察**：acceptance-author 与 builder 都因 manifest 边界主动停手（unit 补件、清单外文件），纪律执行零纠偏成本——full rigor 包的写权防线在本 Pod 已是肌肉记忆。
- **遗留给部署轮的体感提醒**：本包部署有两个人工点（compose up -d 重建容器 = 设备全断窗口；K2-E6 真机跨 WS 回忆），比 K1/P5 的「deploy.sh 一键」多一步。人批合并时要把这点说在前头，避免 human 按旧节奏跑 deploy.sh 后以为生效了。
