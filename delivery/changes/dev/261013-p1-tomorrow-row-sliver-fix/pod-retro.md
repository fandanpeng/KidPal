# pod-retro · 261013-p1-tomorrow-row-sliver-fix（planner 第一人称热记忆，2026-10-13）

> 标签：第一人称热记忆。不复述、不冠权威、不载 tokens/gates/rounds/verdict/test results——那些以 machine artifacts / pod-events / verifier-evidence 为权威。escalation themes 引 pod-events eventId。

## 过程体感

- **最顺的一段是 intake**：plan.md 是 architect /triage-change 产出、带 changeManifest 和 impactedTests 种子，我按 1f 四问逐条对磁盘核验（源码行号、冻结件钉区行、REVISION LOG 位置全部在位零漂移），起包一遍过。这种「上游把方案钉死到行号」的包，planner 的活只剩编排——Lite 比例名副其实。
- **两轮 verifier 红都是「形状差」不是「行为差」**：r1 红 = 冻结件头注 coverage 行合写 `TR-A1/TR-A2`，机械正则只认 coverage: 后第一个 id；r2 红 = perAcceptance 没逐 id 覆盖（TR-A1/TR-A3 缺条目）。都是我起包时对机械闸形状预期不足：coverage map 我写任务书时没预演正则，perAcceptance 骨架起包只给了两条（TR-A4/TR-A2）——起包格式期望前置的「mutationEvidence 骨架」其实该按 acceptance[] 全集起骨架，哪怕 turnedRed=false 占位。**下次起包直接给 N 条全量骨架**，省掉 r2 这一轮。
- **TR-A3 的变异面在冻结件自身**（钉区常量回滚），builder/fixer 都没写权——我在派单时没预判到这一点，r2 红了才意识到要 acceptance-author 实测。scope-map 分权表在 mutation 变异面选型时就要过一遍：变异面落在谁的写权域里，取证责任人就是谁。
- **acceptance-author 的主动加钉（tooth 5）是好抓手**：明天区守卫摘除在 pv_cy1=242 下像素不可观测，ta 自己推演出改钉今天区卡底残线带——比我派单里写的还细。裁量权给到位 + 判别力论证义务写清楚，验收质量就上来了。
- **一次自伤**：Edit 回填 mutation 取证时 old_string 尾巴带了 `}\n    ],` 而 new_string 没带，JSON 结构破了。靠 JSON.parse + required-check 双牙当场抓回。长 JSON 逐条 Edit 时闭合照样要盯。
- **final 轮的 --scope-file fail-closed 举手是好设计**：我第一次跑 phase=final 没带清单直接被拦（exit 2 红闸举手而非默默全量），integrator 已落 test-scope-final.txt，补上就跑。把这个参数写进我自己的流程记忆——integrator 回执里其实提到了文件，我该先读回执再跑闸。
- **模型档位体感**：acceptance-author 冻结/补丁两轮和 builder 一轮都是常规形态，默认档够用；全程无一轮需要升档。

## escalation themes（引 pod-events）

- 本包零 agent-escalation 事件（全程无 Red Flag / blocker / out-of-scope）。红闸全在 verifier 机械面内消化，没出过 Pod 边界。

## 补充轮补记（2026-10-13 晚，human 裁定并入本包）

- **载体改判是这轮最值得记的形态**：human 用「作为上一个包的补充，别再出一个包了，太小了」把行距压缩并进本包，`p1_cjk_name.acceptance.c` 从「零改动回归观察面」变成「必须重锚的载体」——主轮我还特意把它当越界探针，补充轮它成了正面验收面。同一文件在一个包里先当哨兵后当载体，靠的是 human 直令 + taskpackage 留痕，不是流程自己会想到的。下次遇到「上轮零触碰面因几何变化必须随动」，记得先问一句这是不是载体改判，别硬扛零改动口径。
- **阈值假绿是 acceptance-author 自己揪出来的**：主轮 T=80 在旧几何下三带均 ≥80 仍绿（残墨+资产 cap 抬墨），ta 实测 R=84 后抬到 110 才恢复判别力。如果我按「照抄主轮阈值」放行，这个包会带着一个假绿齿合并——这是本批最实质的质量守住。
- **组级门需要新齿**：plan 明载「无既有齿覆盖」，acceptance-author 自己裁「需新齿」并立 tooth 6，净 HEAD 基线即绿（组级门在旧实现下天然成立）。把「该不该为新语义立齿」的裁量权交给验收作者、只要判别力论证到位，产出比我派单写的细。
- **builder 如实申报 deferred 的那条（重锚常量回退变异越其写权）**：他没糊弄成「做了」，而是说清越权 + 由参族回退同源承载判别力。诚实申报比凑数取证有价值。
- **合并门的 INDEX 撞面这轮又来一次**：与前一轮同构（分支侧 INDEX 是 rebase 时取的 main 快照，main 又推进了新行）。同一机制第二次手工解，进一步印证「changes INDEX 单文件 union 登记」是真实缺口——已建议另批治理立项，本包不越权动 .gitattributes。
- **模型档位**：补充轮的 acceptance-author 重锚批（72 tool-uses）与 builder A1 均默认档完成，无需升档；上游把几何推导表写进 plan 是这轮能压缩轮次的直接原因。

## 下次改进（攒给 loop-retro）

1. 起包 mutationEvidence 骨架 = acceptance[] 全集逐条（占位 turnedRed=false），不给 verifier mutation-check 留形状红的机会。
2. 派单前把「变异面落谁的写权域」作为 scope 核对的一问（本次答案 = acceptance-author，差点派错 fixer）。
3. final 轮 --scope-file 参数：先读 integrator 回执拿制品路径再跑闸，不凭记忆裸跑。
4. 「上轮零改动面 + 本轮几何平移」= 载体改判信号：主动向 human 确认是否并入本包 changeManifest，别默认零改动。
5. 生成件（INDEX 类）双渲染撞面：合并前一律先取 target 侧消文本壳 + 合并后重建归位——同一手法第二次用，已固化。
