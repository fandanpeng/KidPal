# Pod Retro — 261022-p3-record-front-polish-fix（integrator 第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/rounds/verdict/测试结果（权威在 machine artifacts / verifier-evidence / review-packet），只记过程体感。

## 本 Pod 最有价值的教训

**blast radius 圈定要先切 base 再读数**。我先跑了一发 base=main 的 detect_changes，60 个 changed files、impacted 里混着两个并联固件包的全部符号——如果照单收进 scope 就把别人的影响面背到自己包上了。切到包首提交父 `80bc2027` 重跑，11 个 changed files、impacted 106 全落 server 三模块，crossEnd 豁免声明和实测一拍即合。并联包同分支收口时，共享分支 diff 和包内 diff 是两张图，圈定永远用后者。

## 卡点

- **首轮 final 命令漏传 `--change-id`，红在 falsy 校验上**：bearing 路径的 normalizeExpected 直接 throw「changeId must be portable ASCII」。命令模板长（七个参数），漏一个就空转一轮。以后长命令先逐参数对模板再回车，或者把命令行整个写进 scope-final §4 先自查一遍。
- **unit 闸的 scope 交集数字第一次看会愣**：scope 清单里 server/tests 只圈了 2 件，闸报「filesWithLiveMark 0/9」——因为 unit 闸收集面是 server/**/test_*.py，验收件也在其中，9 = 7 验收 + 2 单测。不是圈定失效，是收集面口径比直觉宽。数字对不上时先读闸的收集正则，别急着怀疑自己的清单。
- **SHOULD finding 的定位花了点功夫**：窗口过滤先于 routing 判定的边界（pending_stop 末句转写可被滤）不在任何冻结齿的覆盖形态里——冻结齿盯住了窗内丢弃和窗外放行，没盯住「stop 之后才到的转写」。五轴 review 里我沿着「窗内每条路径被滤后走哪条路」推了一遍才找到。以后 review 时间过滤类改动，优先推「每个路由分支 × 每个过滤位」的交叉形态。

## 顺的部分

- 这个包的 verify 链是三个并联包里最省心的：docs-sync 零红轮（launch 面把 docRefs 锚句一次落对，chrome-ghost 的教训被吸收了）、mutation 冻结矩阵五发五中零意外、r1/r2 每轮只红一件事且修复都是单点小动作。
- planner 的 unit 层承载裁定写得很清楚（行为级 8 齿即承载，不扩 manifest），我在 scope 里把 detect_changes 映射到的两件 server/tests 单测圈进真跑——比裁定更严而不是更松，这种「裁定之上白捡的强度」是最舒服的执行位。
- 三轮 scoped + 一轮 final 的 round 序数派单写对了（final = 4，next completed ordinal），上一包 pod-retro 里的坑没有重踩。

## 下次改进

1. 圈定类命令（detect_changes / verify）先切包内 base，base=main 只作共享分支健康参考，两图不混用。
2. 长命令参数多于五个时，落盘到制品 §4 执行位再复制执行，自查成本低过空转一轮。
3. 时间窗类护栏（窗口过滤/截止判断）review 时固定过一遍「过滤位 × 路由分支」交叉矩阵——这次 SHOULD finding 就是这么找到的，值得变成动作而不是运气。
