# Pod Retro · 260911-ecs-deploy-convergence（第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/gates/rounds/verdict/test 结果（那些以机器件与 pod-events 为权威）；只记过程体感、卡点、别扭的 acceptance、帮倒忙的钩子与下次改进。

## 这次最拧巴的地方

**D5 auth 裁决两次反转，是我这个包最大的教训。** 一开始 architect triage 和 plan 都把 live `auth.enabled=false` 定性为「调试残留」，我在 S0.3 问 human 时也顺着这个假设走，human 也顺理成章选了「恢复 true」。结果部署后设备全员连不上——根因是引擎 auth 是 HMAC 动态签名机制，跟决策22 的静态 token 压根不是一套东西，overlay 脚本头注释一直写错了「enabled:true → force shared bearer token」。这个错从 260903 就躺在注释里，直到本包真正开窗 apply 才炸出来。**教训：部署类包不要只按「注释说了啥」下裁决，先读引擎/服务的真实 auth 实现再定谳。** per-delta 定谳的前提如果是错的，整个方向跟着错。

## 最耗时的其实是「漂移家族」比立案时想的深

立案时说「≥4 处漂移」，实际一路挖出来的漂移是连带的一整族：overlay 脚本 6 处 + ECS 上压根没有 overlay 脚本（首铺）+ front/gateway 代码停在 260907 + engine-patch 停在 260907 不含 kidpal_speak。每一层都得 human 授权远端写、都得重启容器让设备重连。**「部署收敛」包的 volume 不是 diff 行数能预估的，得按「live 侧有多少层制品没跟上」来估。**

## 提醒面的 live 真相和冻结件假设不符

L3/L4 是这包扛的 live 五齿里最不确定的两颗。实测下来：L3 推送 + 语音下行真的通了，但 L4 渲染回执 live 从未发生——设备只回 ack:false 受理帧，富弹窗没渲染、掉回普通文字上屏（human 亲眼看到「该出发了」三个字而不是提醒框）。这不是部署问题，是闪录固件的提醒弹窗面可能没生效。**probe≠exercise 的价值在这里现形了**：之前 L3/L4 一直是「pending-human」，从没真正 exercise 过，第一次真推就暴露固件面缺口。这类「看起来已验收、实则从未真跑」的 live 齿，谁扛谁吃亏，但必须诚实记录。

## human 全程陪同让我最舒服的点

D5 前提被推翻时、引擎补丁要同步时，human 都在场且能逐条拍板，AskUserQuestion 的「点名 + specifics」授权虽然多问了几次（分类器强制），但每次都拿到了干净授权，留痕也全。唯一想改进的：**休眠断连这个产品级短板，如果我在开窗前先跑一次「无 device 在线时的 fire」实验，能更早暴露，不用等到 human 觉得『这设计没用』才意识到。**

## 下次改进

1. 部署类包 intake 就列「live 侧制品清单 vs repo 清单」的 diff 面，作为 scope-map 的一部分核对，别等 S0 挨个撞。
2. 涉及 auth/安全面 live 裁决前，先 grep 引擎真实实现（auth.py/websocket_server.py），再问 human。
3. live 齿第一次真 exercise 的期望管理：提前跟 human 说「这里可能是第一次真跑，可能暴露固件缺口」，别让人以为是部署翻车。