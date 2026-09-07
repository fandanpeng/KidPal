# Pod Retro — 260829-home-p1（planner 第一人称热记忆）

> 标签：第一人称热记忆。不载 tokens/rounds/verdict/测试结果（权威在 machine artifacts / pod-events / verifier-evidence），只记过程体感。

## 本 Pod 最有价值的教训

**「改变量」不等于「改行为」——mutation 探针必须变异语义而非字面量**。P1-A2 第一条探针把眨眼周期常量 4400 改成 4410，重编重跑纹丝不动（绿）：采样相位根本没越过可见窗边界，验收齿对 10ms 的常量位移不敏感。换成行为级变异（可见相位比较方向翻转 `<` → `>`）立刻三齿齐红。这轮之前的 mutation 惯例里没有这条显式准则——不是每次都撞得到（A3/A4/A5 的第一发变异就红了），但撞到时就白烧一轮构建。探针设计应该先读验收齿的断言形状再选变异点，而不是拿实现里最扎眼的字面量开刀。

第二课是**门的上下文指针是隐性全局状态**：verify.mjs 九个闸全读 `.claude/taskpackages/current/taskpackage.json`，而 worktree bootstrap 把上个包（g-relay-fix）的 current 残留带了过来。第一次 verify 跑出来 docRefs 全是 GRF-A*——九闸照样全绿（那些门只验 taskpackage 形状，不管它是不是本包的）。如果不是顺手瞄了一眼 docRefs，这轮「绿」就是跑在错误包上下文上的假绿。taskpackage-required-check 的 hint 里其实写了「planner must refresh current before verifier runs」，但那行字藏在脚本里，不踩到不会读。

## 卡点

- **跨 Pod 9p 争用让 WSL 构建路径不可预期**（WIP=3 三个 pod 并发）：host 构建间歇 D 态僵住数分钟，builder 被 build/ 目录上的僵尸 cmake 逼到 `-B build_hp1` 独立目录绕行。事后标准路径补验成功（556/556，结果一致），证明绕行只是环境应对不是形态分歧。教训：构建卡住先看是不是别家 pod 在打共享 .git 的 git describe，别急着怀疑自己的 CMake。
- **上游永红齿污染全量 runner 判读**：WSA 包的 flash.log / ecs-teardown-evidence.md 两个 evidence 齿文件从未入仓，main 本身就红——本包全量 runner 永远 EXIT≠0，绿判定只能收窄到「包内齿 + 守卫集」再人工排除。VRT-A2/E2 齿还有 CWD 敏感（host_app 目录跑 3 红、仓根跑 2 红）。逐条对账花了一轮，且将来每个 host 包都要重新对一遍这两条。
- **build_hp1/ 和 sdkconfig 逃出了既有 .gitignore 模式**：非标构建目录名不带 `build` 前缀照样该拦——integrator 已补 `firmware/.gitignore` 两行。

## 顺的部分

- acceptance-author 一轮冻结到位：weak-symbol frozen contract 模式第三次走（view_router 先例），11 函数签名 + 数值码 + builder duty 全钉在验收头注，builder 全程零回问、零验收争议。这个模式的边际成本已经很低了。
- mutation 四连探针打得干净：增量构建 15s 通路 + 逐齿转红 + 还原 sha256 逐字节一致，全轮没有一次「还原后发现基线漂了」。
- integrator 把 handoff 遗漏接住了：verifier 轮跑的是 legacy 非承载模式（没落 bearing fact），integrator 用 `--evidence-mode=round` 补跑 final r1 并过 verifier-evidence-check——两道独立结构这回真兜住了一次作者偏差。

## 下次改进

1. mutation 探针设计准则写进派发模板：优先变异比较方向/分支极性/位标语义，慎用常量字面量位移（除非齿显式钉该常量）；acceptance-author 在验收头注里给每齿标「对何种变异敏感」更佳——登记 ARQ 收件箱。
2. planner verifier 轮 checklist 固定第一步：核对 `.claude/taskpackages/current/taskpackage.json` 的 changeId == 本包，再跑 verify.mjs；verify.mjs 若能在输出里回显所验 changeId 会更稳——同上登记 ARQ。
3. 「evidence 文件未入仓即合并」是 WSA 包留下的闸缺口（上游包合并时 evidence 齿红但 ESCALATE 人批放行），每个后继 host 包都要重新对账——登记 ARQ 收件箱，候选修法 = supervisor-verdict-gate 对 evidence 齿红给显式降级/豁免字段而不是让 planner 每包手工排除。
