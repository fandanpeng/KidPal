# pod-retro · 261021-p3-record-cancel-lifecycle（第一人称热记忆）

> 标签：第一人称热记忆。只记我在场的过程体感与判断；token / gates / rounds / verdict / 测试数字以 machine artifacts、verifier-evidence/rounds/、verdict.json 为权威。

## 这个包整体给我的感觉

full 档回归修复包的教科书形跑法：上游（triage + /arch-change 契约批）同日闭环后接棒，spine anchors 全 resolved、契约 LANDED、OQ 四条全裁——intake 阶段我没有做任何设计裁决，全部工作是把 plan.md 的冻结技术输入机械转译成 taskpackage。体感上**跨会话接棒的顺畅度完全取决于上游会话有没有把「launch 前置」条款写进 spine**（本包 facePlan 顺序2 写明了锚回填 + 触点③，我照单执行零歧义）；261019 建立的这个先例第二次跑已经不需要重新理解。

## 卡点与绕轮的主观判断

- **verify 内循环三轮的构成值得细看**：r1 红 = mutation-check（证据义务未收口，builder 补战役）、r2 红 = security 闸（securityEvidence 我 finalize 时留空未回填）、r3 绿。**两轮都不是实现缺陷**——r1 是流程时序（mutation 本来就在 builder 收尾义务里，只是我提前触发了 verify），r2 是我自己的 finalize 遗漏。教训：**finalize 时对着闸清单自查一遍比事后挨闸红便宜**——securityAxes 有值而 securityEvidence 为空这个不对称，我在写文件时就该看见。
- **acceptance 探针缺陷在 builder 侧被抓住而不是 verifier 侧**：builder 复验发现 `_display_payloads` name 层级拷错（负向断言空过、A4 正向断言不可满足），树外内存补丁证明修后 9/9 绿才停回来。这个走法对了——**冻结验收的缺陷若流到 verifier 才发现，会被误读为实现红、白烧一轮 fixer**。builder「先证红因在探针再停回」的纪律（含树外补丁证据）值得固化为 builder 常规动作。
- **mutation 战役作为收口义务下放 builder 是对的**：五条全 sensitive=true、还原 md5 零漂移、还顺手交了一条 tooth 强度观察（RSL-A8 tooth1 文件计数可被注释满足——注释自指试跑实测）。我原以为 mutation 要么 acceptance-author 要么独立做，实际上**刚写完实现的人对自己的变异面最熟**。附带收获：F1 附加变异（settle 半）恰好单红 RDL TEST 8，等于免费多验了一条齿的敏感度。
- **同实例续跑派发（SendMessage 回原 agent）在 acceptance-author 重钉和 mutation 战役两处都用了**：上下文完整带回、不用重读冻结件。代价是原实例的上下文窗口在长会话里持续膨胀（acceptance-author 两次合计 53 万 token）——重钉这类「读上下文为主、写一小块」的任务同实例划算，全新战役另起实例。

## 跨包留根

- **U3 腿集回退**：[SC-D] 空表回退全局 `_alert_legs` 是为保 RDG-A6 must-stay-green 的取舍（已入 review-packet 语义决策节）。后续若有人改 RDG-A6 的腿登记形态，这条回退要同批摘除——**留着不摘是无害死代码，摘 RDG-A6 不摘回退会裸喷全局腿**。
- **真机轮四项**（awaiting-human:real-device）：discard 帧序端到端 / BOOT 丢弃不归档 / 收尾后长按放行（show_home 守卫）/ 复验③(a) 引导刚停瞬间丢弃。烧板不可省，mcp_server.cc / protocol.cc 都是设备编译单元。
- **debt 建议**：RSL-A8 tooth1 注释可满足的强度缺口——变异常规候选里有现成修法（变异注释规避符号字面是 workaround 不是修复），值得 arch/治理面 tighten（结构性 = 齿强度判据，不是单包实现面）。

## 帮倒忙 / 帮上忙的钩子与规则

- 帮上忙：brief-inputs 对 crossEnd 对端符号 fail-loud（`SendKidpalStoryDiscard symbol-not-found`）——它逼我把「待建符号」换成「在世锚点」（KSPV2_ACT_DISCARD_RECORD / SendKidpalStoryDelete），对端阅读面因此指向真实存在的代码。
- 帮上忙：taskpackage-required-check 对归档 debt 路径的 path-not-on-disk 红——上游会话把 TECH-261021 归档进 `_archive/2026/` 时没改我的引用，闸替我抓了死链。
- 帮上忙：scope-map 拦 Bash 重定向写 pod-events.jsonl（universal.bashDenyWrite），把我推回受门 Edit 通道——第一次撞是学费，第二次形成肌肉记忆。
- 略别扭：check-launch 闸对 in-session ② Full 起草路径永远红（拆包产物要合并后才进 base ref）——闸的语义是 launch 前「包已 commit 到基准」，单包会话内起草天然错位。文面已声明「预检非机械闸」，但每次 launch 都要人工解释一遍这个红，值得考虑按路径形状给单包起草路径一个 advisory 形态。
