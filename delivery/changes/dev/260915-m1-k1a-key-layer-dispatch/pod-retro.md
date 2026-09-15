# pod-retro · 260915-m1-k1a-key-layer-dispatch（M1-K1a 键层派发）

> 第一人称热记忆（planner 主线程，integrator closeout 之后、supervisor gate 之前写）。
> 负向边界：本件不复述 tokens / gates / rounds / verdict / test results——权威在
> machine artifacts（verifier-evidence r1、pod-events.jsonl、review-packet.md、red-baseline-freeze.md）。
> escalation themes 交叉引用 pod-events eventId 见文末。

## 过程体感

前半程（intake → acceptance-author）非常顺：launch prompt 把并行批声明、KD-A4
批改纪律、留根 7 条都喂到嘴边，current/ 指针切换、brief-inputs 复验、coverage 自证
一次过，finalize 闸绿。acceptance-author 把机制齿（路由级双判别式）+ 结构齿（负向
扫描）+ KD-A4 十处批改一次成型，红基线复跑多出的那条红（RMD-A5）被它主动定性为
「设计态义务红、与 REVISION LOG 预测逐字吻合」——这条定性后来被 builder 落地转绿
实证，**预测式红基线判读（先说哪条会红、为什么红、落地即转绿）是本包最值钱的纪律
动作**。

## 卡点与绕轮原因（主观判断）

1. **sonnet 模型池 weekly 配额两连熔断（本包最大卡点）**：builder 跑到自测阶段
   先吃 429（5 小时窗），续跑后再次 429 且升级为 weekly（重置 09-21）——builder
   六项回报彻底断链。缓手是 planner 实测探出 **opus 通道独立可用**（配额按模型
   分池），后续 integrator 换 opus 绕开。代价：builder 的实现自述与变异执行证据
   断链，planner 只能独立复测补位（host 134/4 + 整编 exit0 + verify 两相位），
   mutation provenance 在 review-packet 如实降格为「builder 断链自述 + 终树全绿
   旁证」。**体感教训：多模型分池配额是可探测的事实，不是死局——熔断后先花一次
   小 agent 探通道，别直接挂起等重置。**
2. **enforce-scope 拦 acceptance-author 落 red-baseline-freeze.md**：agent 写域
   不含 delivery/taskpackages/**，与 launch 指令冲突；agent 按权限系统为准不绕行、
   全文回传 planner 转写（260911-render-pages 先例头注同款）。机制工作正常，但
   **launch prompt 应预先写明「红基线落账由 planner 转写」**，省掉 agent 侧一次
   碰壁-回传的往返。
3. **verify.mjs 位置小坑**：launch prompt 只说「verify.mjs」，实际在
   `.claude/hooks/verify.mjs`（scripts/ 下没有）；且默认 scoped 相位只跑 7 闸，
   final 相位（`--phase=final`）才跑 unit/acceptance 全量 9 闸。另外 final 相位
   对在册 server 基线红（SMK-A4/SRE-E4）是机械红——**final 判读必须带着基线判读
   一起看**，这个纪律 launch prompt 有预报，照做即无歧义。
4. **/mnt/c 编译税**（human 两次追问）：WSL 经 9P 访问 Windows 盘，新 worktree
   首编全量极慢（builder 1.5h 墙钟大头在此，token 消耗却正常——等待期不烧 token）。
   改进候选：**ccache 挂进 espidf_wsl.sh**（跨 worktree 共享编译缓存，并行 Pod
   双赢），交 /loop-retro 汇集。

## 别扭的点 / 帮倒忙的钩子

- intake 时 current/ 指针仍指 260911-render-pages（launch 面没切），planner 开工
  即切——留根第 2 条纪律救了一次，但**这步本可由 launch 面做完**。
- brief-inputs 对本包三个 ref 全报 ADV（无锚点、退化整文件引用）——契约/需求是
  平铺 markdown、锚点体系（#§2.3）是需求侧特有，ADV 属已知形态，未拦路。
- Bash 权限分类器（sonnet 驱动）在配额熔断窗口内也间歇不可用，只读工具不受影响
  ——配额熔断的伤害面比「不能派 agent」更大，**留痕动作（pod-events append）都
  可能被拦**，需要像本次一样事后补记。

## 人工纠偏体感

human 一次打断重派（acceptance-author 首派被拦后放行）+ 三次追问（进度 / WSL
慢因 / 能否上真板）。追问落在「时间去哪了」与「测试为什么只能在 WSL」——体感是
**launch 侧准备（worktree + 图索引 + 红基线首跑）占掉的半小时对 human 是黑箱**，
值得在 launch 完成时给一条显式耗时分解；host runner 的「真机不在环、恰是为了不摸
板子」这个定位也值得写进 Pod 级 FAQ，human 不必每次重问。

## mutation 纪律体感

11 条变异计划由 acceptance-author 冻结期预填（行为级命名、无原字面子串），执行侧
因 builder 断链未能取得六项回报——闸绿但证据强度降格已如实入档，**「可要求独立
复验」选项留给合并门**。KR-A3 教训（命名不含原子串）在预填时已吸收，执行侧无从
验证是本包 mutation 面的残缺一角。

## escalation themes（交叉引用 pod-events）

- `friction 61fe7b57`：builder 429 首熔断（5 小时窗）。
- `friction 5c70f75d`：weekly 配额升级熔断 + 分类器连坐留痕被拦（补记）。
- `dispatch-completed 0311eddc / 27d4125e / f25522de`：acceptance-author / verify /
  integrator 三次阶段收口。
- 无 agent-escalation / human-decision 事件——本包 human 出场均为会话内问答，
  未触 fail-closed。

## 下次改进（本 Pod 视角）

1. 配额熔断预案：熔断即探测备用模型池（一次小 agent 试金石），Pod prompt 写明
   「subagent 模型可换」不是固定 sonnet。
2. launch 面代办：切 current/ 指针、红基线落账写明「planner 转写」、launch 完成
   汇报附耗时分解。
3. ccache 进 espidf_wsl.sh（交 /loop-retro）。
4. builder 回报断链时，planner 独立复测的最小集 = host 全量 + 整编 + verify 两
   相位（本次三件都做了，成本可接受），mutation 独立复验按需追加。
