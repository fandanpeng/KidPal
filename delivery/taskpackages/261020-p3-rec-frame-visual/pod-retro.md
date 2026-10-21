# pod-retro · 261020-p3-rec-frame-visual（第一人称热记忆）

> 标签：第一人称热记忆。只记我在场的过程体感与判断；token / gates / rounds / verdict / 测试数字以 machine artifacts、verify-runs.jsonl、verdict.json 为权威。

## 这个包整体给我的感觉

nano 档的首跑，整条链是按 261016 nano tier 的设计意图走通的：triage 一次判档、意图定义五件直接当 plan、无 acceptance-author（suiteGate 纯形态）、builder 一轮落地、闸族 nano profile 全部认。体感上它和 lite 的差别不在流程长度而在**验收权威的位置**——没有新冻结齿，行为验收判分位挪到了白盒复核 + human 真机目检，这在「坐标微调」这类改动上是恰到好处的比例。

## 卡点与绕轮的主观判断

- **taskpackage-required 闸两次自修（r1-r4）**：nano 形态的两个必填我在首写时漏了——`crossEnd` 骨架（空 counterparts 也必须在场）和 `mutationSkipReason`（mutationRequired=false 必须带因）。都是闸红即修、一轮过。教训：**nano profile 的字段契约和 lite/full 一样是机判的**，首写 taskpackage.json 时应该先读一遍 taskpackage-required-check.mjs 的 nano 节再动笔，省两轮。
- **爆炸半径的量数口径在 plan 期就该钉死**：architect triage 时用 grep 实测 6 调用点/4 作用域，白盒复核 grep 到 7 调用点/5 作用域（kidpal_render_node.cc 的 KP_PAGE_STORY 分支 plan 期漏计）。作用域阈值 5≤5 仍达标没翻案，但如果当初阈值线画在调用点上就会翻——**口径（按作用域不按调用点）比数字本身承重**，幸而 human 触点①确认时把这个边界申报一起批了。
- **final 相位的 scope-file 依赖**：我在派 builder 前先跑了 verify（无 scope-file → fail-closed 举手），才知道 scoped → integrator 圈定 → final 的流序；与 261018 retro 记的是同一个坑，这次没有重踩第二次但第一次还是撞了（direct 跑 --phase=final 报缺 --scope-file）。这个流序值得从「个人跑包清单」升格为 launch-card 模板行。
- **真机门后的追加指令处理**：NRF-X4 目检时 human 当场追加了第三处（回显列上移）。我按「同文件同分支同意图族、manifest 未越界」并入本包并留痕 plan「追加修订」节，builder 同实例续跑。回头看这个判断是对的——为一个 y 坐标单开一个 nano 包的仪式成本远超收益；但**如果没有「manifest 未越界」这个硬判据兜着，这个判断就会变成扩单**。判据先行是敢当场并单的前提。

## 真机轮带出的三个缺陷（已记 debt，不在本包修）

真机目检轮附带暴露了 261019 对话引导链路的三个同根缺陷（丢弃不通知 server / server 收尾不清设备 ctx / 丢弃内容仍被自动归档），human 裁定记 `TECH-261021-story-session-cancel-lifecycle` 单开对话处理。本包的坐标改动与它们正交（白盒复核：diff 全在绘制实参，键路/ctx/sweep 零触）。**观察项两条 human 裁定再观察，不入 debt**：① 首次按键吞没（日志零痕迹，GPIO/去抖层级，需复现）；② 首页语音入口进录音页 10-16s 延迟（LLM 链路往返，属产品形态取舍——故事列表页 KEY 直达 2.8s 的快路径已在）。

## 帮倒忙 / 帮上忙的钩子与规则

- 帮上忙：verify-gates 的 defer 字段对 nano scoped 相位的短路（firmware-build 后置到 final 才真编译，scoped 轮秒回）；acceptance-coverage 对 suiteGate 纯形态的识别零噪音。
- 帮上忙：`show_home suppressed: record session live` 这行 warning 是 ④ 困死缺陷的唯一现场痕迹——长按有派发、只是被守卫拦，一行日志把「软件链路通、守卫拦截」和「按键吞没」两种假设干净分开了。log 纪律的直接回报。
- 略别扭：serial_catch_tight.py 的内层 while True 不回检截止时间（skill 已明示），每次用完都要手动 taskkill——2026-10-16 已知坑，仍未修。
