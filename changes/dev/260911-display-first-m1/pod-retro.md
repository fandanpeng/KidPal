# pod-retro · 260911-display-first-m1（第一人称热记忆）

> planner 收尾相位第一人称记录。只记过程体感，不载 tokens/gates/rounds/verdict/test results（那些以 verifier-evidence / pod-events / supervisor-verdict 为权威）。

## 过程体感

这一 Pod 干下来最大的体感是：**真正的实现改得很小、也很顺，耗时全花在「形态闭合」上**。builder 阶段 1-3 一次成型，红基线 6 齿干净利落转绿，front/固件复核都判「零代码变更」——说明 acceptance-author 把语义边界钉得够清楚，builder 照图施工没有返工。真正磨人的是 taskpackage.json 那一层：verifier 一闸一闸地抓出三个 finalize 缺口（DF-A3~A6 没标 structural、mutationEvidence 的 acceptanceId 我自作主张缩写了几行导致 raw-id 匹配失配、docsSyncRequired=false 没配 reason 字段）。每抓一个回去补一个、重跑一轮，三趟来回全是「改一行 JSON 再重验」，没一行是产品代码。主观判断：这类缺口要是 intake 阶段有个「taskpackage 形态预检」能一次列全，四轮能压成一轮。想想有没有必要，但我不是提纯这套东西的人，先记下来。

## 卡点

- **主仓共享 git config 被零字节化**（凌晨，非本会话动作）：全仓 19 个 worktree 的 git 集体失效，正是本 Pod 推进到 builder 后要跑 FS-EVIDENCE-1 `git diff` 的当口。这卡点是环境事故、不是流程能防的，但暴露一件事——整个 dev-loop 的验证链全都隐性依赖 git 可用，git 一瘫，连「确认 builder 改了哪些文件」都做不了。我按 deputy canon P-2.9（枚举外不裁而报）停等、写 incident 记录，没动那个我看不见原内容的共享 config——事后看这个决定是对的，human 回来一分钟就恢复了。
- **brief-inputs 的 symbol 正则不认 Python 常量赋值**：crossEnd 写 `mcp_relay.py#TOOL_DISPLAY` 时 fail-loud，因为 `TOOL_DISPLAY = "..."` 的 `=` 前空格不匹配 `[\w:(]`，换成 `display_tool_call`（def 形式）才过。这个坑浪费了一轮 intake，而且我一开始还差点以为是对端面真有问题。
- **pod-events 污染**：dispatch-stamp/journal-append 是通过 `current/` 指针解析 changeId 的，指针一天没切到本包，簿记就往上一批 PKG-2 的 pod-events 里写，还得 `git checkout` 还原再重灌。教训比「记得切指针」更深一层：**凡是用 current/ 指针的脚本，动手前先确认指针指向自己**。

## 别扭的验收 / 帮倒忙的规则

- mutation-check 的 raw-id 全串匹配，要求 mutationEvidence 的 acceptanceId 跟 acceptance[] 正文**逐字节一致**——我把「server/acceptance/」这类前缀「顺手简化」掉就直接失配。这条规则的语义（防 id 漂移）是对的，但「一字不可差」的严格度对「人肉抄写 acceptance 串」这个动作太苛刻，抄错一个 prefix 就假红。主观觉得这里值得一个「短 id 匹配 + 报 warning」的温和档，但这是闸面的架构决策，不归我裁决。
- acceptance-coverage 的 structural 豁免走「对象条目 kind:'structural'」唯一通道，而 acceptance-author 交付报告里明确写了 DF-A3~A6 是「pending 真机/已承载/部署」，却没把这结论转成对象条目编码——两边的契约没接上。作者判了、闸不知道，等于白判一遍。

## 人工纠偏体感

human 深夜那条「有疑问参考 deputy 定义裁决、记录下来明早看、硬件实测合并留明早」的授权，是整个夜间自治能跑起来的地基。它让我在「git config 事故」这种枚举外情形下，有明确的「不裁而报、留痕停等」依据，而不是硬着头皮修共享基建。deputy 的 P-2 九项「永不豁免」清单（契约门/ESCALATE 合并/远端写/接受风险……）实际上成了我夜里判断「什么绝对不能碰」的现成 checklist，不用每条都现场想一遍。

## 下次改进

- intake/finalize 阶段对 taskpackage.json 做一次「形态预检」：acceptance[] 里非离线可测条目有没有转 structural、mutationEvidence 的 id 是否 = acceptance 正文、docsSyncRequired=false 是否带 reason——把 verifier 的三趟「补 JSON 再重验」提前到派 builder 之前。
- 动 current/ 指针前先 `head` 一眼它现在指向谁；用了 current/ 解析的脚本（dispatch-stamp/journal-append）尤其要在当前包开始就先切指针。
- crossEnd 填对端 symbol 时优先填 def/class 形式的承载函数名，别填模块级常量赋值。