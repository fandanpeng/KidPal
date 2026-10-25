# pod-retro · 261025-kidpal-tools-profile-fix

> 第一人称热记忆（planner 收尾相位；integrator closeout 之后、supervisor gate 之前）。
> 只记过程体感与主观判断，不复述、不冠权威、不载 tokens / gates / rounds / verdict / 测试数字——
> 那些以 machine artifacts / pod-events / verifier-evidence 为权威。

## 这一包跑下来的主观感受

这是我从半成品续跑的包。上一次派发被配额打断，留下的 plan.md 结构其实写得很对（背景、目标态六条、
红线），我几乎没有返工就沿用了它的骨架，只补了 frontmatter、技术方案节和 demo 证据形态。**教训：
半成品不必然是负债，先读、判断可用性，比推倒重写省得多。** 但也要警惕——半成品里的「四项」措辞
是上一代的，直接抄会把旧口径固化进新包；我逐条对着 live provenance 与现有工件核了一遍才敢用。

最值得记的一个判断：一开始我以为这包只是「把 profile 加进声明」，读完代码才发现它会**连带打破
K3 冻结件的逐字锚**（memory_isolation 把 honcho 声明的 drift 四项钉死了）。这是典型的「改一个文件、
红在另一个包」的跨包耦合。如果我不去读那份验收件，builder 改完声明后 memory_isolation 会红，
而修复它又会被当成「fixer 返工」——责任会落到错误的角色上。**起包期多看一层跨包冻结件，把义务写进
TaskPackage 与 changeManifest，比事后追责便宜得多。**

## 卡点与绕轮

本轮没有 verifier↔fixer 绕轮，一次 builder 就转绿。不是运气好，是 architect 1g 预评把三个最容易
做错的语义提前钉死了：① profile 缺席必须**接受**（那是 P0 的升级起点，拒绝它等于拒绝自己）；
② 幂等 SKIP 必须与 drift 联合（否则 deny 非空被裸 SKIP 绕过）；③ 父键畸形要与键缺席拆开。
第 ② 条是 architect 对我拟定方案的**实质反对**，我当场采纳——这是我这一轮最认可的一次「预评挡下
我自己没想到的洞」。如果直接按我原方案写，deny 非空场景会静默 SKIP，而那正是安全红线。

小的磕碰在收尾：acceptance-coverage 门报 K3-E6 未覆盖，原因是标记写成 `coverage: A1, K3-E6`
（id 不在首位）而门的正则只认紧邻。我犹豫过要不要直接标记 acceptance 为 structural 走豁免，
最后否掉了——那是用豁免掩盖文档形态问题。派了一轮极窄的 acceptance-author 修补，只动一行顺序，
数字不变。**门的设计本身值得复议**：要求「id 紧邻 coverage 关键词」让多 id 标记很容易踩坑，
而错的不是断言而是排版。这是我这轮对钩子/门唯一想吐槽的点。

## 别扭的 acceptance 与锚换代

K1-E4 已经是第二次换代（上一批四项→五项，本批再叠 profile 维）。冻结件越滚越大，头注的「旧→新
对照」段已经很长。我理解这是显式换代纪律的代价，但也在想：**同一份验收件被连续两批换代，是不是
该考虑把「声明面 schema 断言」与「行为面真跑」拆成两个文件**，让换代半径小一点。这条不是本包要
解决的，留给 loop-retro 参考。

另外 builder 提了一个我没有预先想到的点：manifest 的 note 里写 `{profile,allow}` 花括号会让 YAML
flow mapping 解析失败，必须整条加引号。这是「逐字替换」指令与 YAML 语法的冲突；builder 做了正确
的选择并如实申报。我接受了，但把它记进 review-packet 的 Change Size，避免下一轮有人以为是无故
改格式。**派单时写「逐字替换」要留一个「语法必需时可偏离并申报」的口子**，这轮是运气好，builder
自己想到了。

## 人工纠偏体感

整个过程没有 human 中途干预（本包是主线程按 human 既定指示自建的 follow-up）。唯一需要人裁的
位置就是合并门本身——按指令，无论 APPROVE 还是 ESCALATE 都停等。我认为这个停等是对的：live 已经
是目标态，仓内只是把已验证形态固化；但 push 存量与合并节奏归 human，Pod 不该替人决定。

## 下次改进

1. 起包读半成品时，先做一次「旧口径扫描」，把半成品里残留的上代措辞列出来，再决定改写范围。
2. 跨包冻结件耦合清单应当在 Step 1 就做（本包是读代码时才发现的），可以把「改动面落在哪些
   acceptance 文件里」当成 intake 的一个固定问句。
3. 对「多 id 标记」的 coverage 门，建议在 loop-retro 提一次：正则放宽为「关键词后 40 字符内出现
   id」或允许 `coverage:` 列表中任意位置，避免排版问题误报。

## escalation themes（cross-reference pod-events）

- 无 `agent-escalation` 事件（本包未触发升级链）。两处 friction 均为 over-scan 记账，属
  acceptance-author 的工具面阅读申报，非阻塞：`5f6a56a4-50fe-42f2-a06a-b46740109b69`（首轮
  6 文件，判红基线与门正则所需）、`0399a3b1-b640-4686-a962-26aac1dc311c`（修补轮 2 文件）。
