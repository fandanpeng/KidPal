# pod-retro · 261013-lang-config-thinking-fix（第一人称热记忆）

> planner 收尾相位 · 2026-10-14 · integrator closeout（febba14）之后、supervisor 机械闸之前写。只记第一人称过程体感，不复述不冠权威：tokens / gates / rounds / verdict / test results 以 machine artifacts（verifier-evidence/rounds/*.json）、pod-events、closeout 制品为权威。

## 卡点（按时间序）

1. **起包期误跑 verify.mjs**——Step 7 前我探脚本用法时无参直跑了 verify.mjs，当时 current/ 指针还指着 sibling 包（merged 态陈旧投影），结果闸组对着别人家的 changeId 真跑了一遍编译（白烧一次首建量级时间），还给 sibling 的 verify-runs.jsonl 和 current/verdict.json 各落了一条污染记录，事后 git restore 还原 + friction 落账。教训值一句钉死：**current/ 指针刷新必须是 Pod 的第一组写动作**，任何 hook/脚本调用都排在它后面。
2. **验收载体与机检形态脱节**——我给本包选了 `.acceptance.md` 载体（有 260831 两包先例），但起包时没预跑一遍 acceptance-coverage 的形态正则；r1 红在「文件里全是 shell 判定命令、没有一个机检可识别的 assert 调用形态」。冻结件本身内容是好的，纯粹是形态适配欠账。下次编译层包起包，draft 落盘后就该跑一次 coverage-check 干跑，把「coverage: LG-Ax + assert 形态」写进 acceptance-author 派单，省一轮返工。
3. **mutationEvidence 起包骨架想少了**——我只按「两个天然红机制」给了 2 条 perAcceptance，r2 mutation-check 要求每条 acceptance id 都有变异判别。事后补了 4 条真实变异 + 3 条同源承载声明。Lite 包不是豁免口径：**起包时按「每条 acceptance 一条变异（或显式同源/deferred 申报）」数一遍**，别让 builder 跑完才发现账不平。
4. **taskpackage 原子写教训**——我用 heredoc python 直写 taskpackage.json 把文件写成了 1 字节（原因未完全查明，heredoc 管道写入路径可疑），幸而 current/ 投影是注入前的完好副本，恢复无损。此后所有回填改走「写 .new → 内存自校验 → os.replace」原子路径（injector 脚本即此形态）。**TaskPackage 是被三个闸消费的承重件，永远不要原地覆盖写**。
5. **final 相位 scope-file 前置没进我的派单模板**——第一次 final 轮被 fail-closed 红闸举手（缺 --scope-file），integrator 派单里也没含 final 轮职责，导致收口后我还要自己补跑 + 回填占位。integrator 派单模板该加一句：落盘 test-scope-final.txt 后由 planner 补 final 轮，派单写明「final 轮归 planner，integrator 只备清单」。
6. **账本归档机制会静默弄陈旧 TaskPackage refs**——integrator 收口把 TECH 条目 crq-render 归档进 `_archive/2026/`，我 sourceRefs/docRefs 里指向旧路径的引用立刻陈旧，final r4 docs-sync 红才暴露。1f 权内修正解决，但这属于「收口动作反噬起包引用」的系统性小刺，值得在 dev-loop 参考里记一笔：引用 CRQ 条目的包，refs 按归档位申报或起包时就指向稳定锚（spine/INDEX 行）。

## verifier↔fixer 绕轮体感

三轮 scoped + 两轮 final，红的全部是「TaskPackage/冻结件簿记面」而非实现面——builder 一次交付即绿（LG-A1..A9 + B1/B2 + 补证 M-A2/A3/A5/A7 一次过）。实现质量好；返工债全在我这侧的起包完备度（卡点 2/3/5）。fixer 一次都没派——红闸都由 planner 权内修正或 acceptance-author 增量修订消解。

## 别扭的验收 / 钩子体感

- enforce-scope 两次拦我（Bash 重定向写 delivery/taskpackages、Write 到 /c/tmp）——拦截语义都对，派单书③的「代存」流程也顺；只是 mutation 取证这类高频代存件，planner 的代存动作每次都要绕一次 Bash 禁令改走 Write，体感略繁琐但不算伤害。
- acceptance-author 为过 coverage 闸发明了「coverage: LG-A1 (T1)marker」胶水词形态——机检正则与 id 提取的缝隙逼出来的 hack，能用且已注记，但这属于机检实现细节泄漏进了冻结件文本，将来 coverage-check 若改提取逻辑这批标记会静默失效。记为潜在腐化点。

## 人工纠偏体感

human 全程零触点（Lite 线、无必升面命中），无请示、无纠偏，Pod 自走到底。

## escalation themes（pod-events 交叉核对）

零 agent-escalation 事件。pod-events 全程 = dispatch-completed ×6（acceptance-author×2 / builder×2 / integrator）+ friction（degraded×1 误跑污染、over-scan×3）。无 human-decision / human-correction。本次 Pod 无升级主题。
