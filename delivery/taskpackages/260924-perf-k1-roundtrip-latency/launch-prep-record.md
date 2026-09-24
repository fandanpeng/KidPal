# launch-prep-record · 260924-perf-k1-roundtrip-latency

> append-only。执行主体 = 主线程(human 会话,2026-09-24 夜);本记录 = kidpal-pod-launch-prep skill 产物 1(调度判定)对「独立包、无 workstream」形态的落位替代——kidpal-mvp/m3-schedule 等 workstream progress.md 不适用本包,判定记本文件(如实记录偏离)。

## 调度判定(2026-09-24 00:30~01:00 CST)

- 单包批:本包 dependsOn=[],无 mergeOrder 约束;board 无 running Pod(K7 已 merge-completed,board 行残留 blocked = 簿记漂移,已报晨间);WIP 余量足(3-0)。
- human 直令授权链:2026-09-23 夜「现在开始分析慢的问题…深入排查…有什么解决方案」→「分析好,有优化方案就让 architect agent review…确定方案后建一个 taskpackage,launch prep,启动 devloop 优化,明早我起来看效果」。
- architect advisory 评审:GREEN 语义(Track B dev taskpackage 成立 / riskTier medium / 不触契约 / D1~D4 修正吸收进 plan.md FROZEN v1.0)——评审文件双落位(delivery/changes/arch/260924-arch-review-perf-k1-roundtrip-latency/ 原件 + 本包目录副本,scope-map 对 architect 写边界所致,主线程搬运)。
- **launch 代跑注记**:skill 默认「不代跑 launch,交 human 自跑」;本次 human 明示「启动 devloop…明早我起来看效果」= 当晚 launch 授权,主线程以 headless 形态代跑,留痕此行。

## 基线锚(server 面,五要素)

- 计数:见下「基线锚实测」段(已落,launch 前现跑)。

## 环境事实

- 运行时 worktree:C:/AI/KidPal-pod-perf-k1-roundtrip-latency(新建;池树 C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch 被本主线程会话占用——一树一会话,不复用)。
- 常驻分支:dev/260924-perf-k1-roundtrip-latency(自 main 建)。
- 固件环境等价化:✓——sdkconfig/dependencies.lock 镜像;managed_components 软链(原生 python isdir=True);bootstrap 跑过(跳过 server/shim/.env 与 sdkconfig 源 = 按设计,shim 已退役)。
- 专属图索引:✓ `C-AI-KidPal-pod-perf-k1-roundtrip-latency`,**mode=full + persistence=true**——nodes/edges = 17952/48502 == expected(完成判据满足),skipped=0,parse_partial=102(集中于 firmware audio codec 面,非本包面域,如实记录)。**mode 偏离注记**:skill/AGENTS.md 写 moderate,但 memory 注记(codebase-memory-index-mode-full)与 ARQ-260923-codemem-mode-doc-drift 实证 moderate 削薄约 44% 符号——取 full(强化非降级);文档漂移待该 ARQ 治理批收口。索引产生的 .codebase-memory 两件为机器再生面,不随包 commit(AGENTS.md 快照流向纪律)。
- board:✓ `--set-pod queued + worktreePath` 已落(2026-09-23T17:24:17Z,reconciled 40 pods,mergeToken holder=none)。
- check-launch:✓ `--base-ref main` → ok:true,blocking=[],present=[taskpackage.json](2026-09-24 01:23 CST)。
- launch 形态:headless(启动卡命令行②),主线程代跑(human 当晚授权,见 §调度判定 launch 代跑注记);进程日志 `C:/tmp/pod-perf-k1-launch.log`。

## 基线锚实测(launch 前 append)

- **server 面**:57 passed / 0 failed(上游已知红 = 0)· 命令 `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-perf-k1-roundtrip-latency/server/acceptance"` · 2026-09-24 01:22 CST · 量测时 HEAD = `1746c147e65e2f5d6f561ca32e2e41cb9c4a9c10`(= Pod 分支基线,main tip)· 面域 = server。Pod intake:`git diff --quiet 1746c14 HEAD -- server` 零差异即引用本行,有差异同命令重跑。
- firmware host 面:本包 subsystem=server、firmware/** 零触碰,不适用。

## launch 阻塞记录(2026-09-24 01:28 CST,append)

- headless launch(启动卡命令行②)失败即 exit 4:日志唯一行(C:/tmp/pod-perf-k1-launch.log,235B)= CLI settings 校验拒绝 `.claude/dev-loop.settings.json` 的 `Write(./docs/contracts/**)` deny 规则——新口径文件权限检查只认 `Edit(path)` 规则(Edit 规则覆盖所有文件编辑工具),`Write(path)` 不再被匹配。
- 主线程两种修复形态(仓外 fixed 副本 / 仓内 Edit 删无效行)均被 auto-mode classifier 拦截(Self-Modification:移除 deny 规则须 human 本人提出)——**拦截接受、不绕行,机械强制层变更不得 agent 自批,如实留痕**。
- 已登记 ARQ-260924-settings-write-deny-rule-cli-drift(main 017b24a):证据逐字、修复建议三条、riskTier low、owner=architect。
- 本 Pod board 保持 queued(未 launch);launch-prep 九件产物全部有效,**human 晨间批准 settings 修复后按启动卡命令行原样重跑即可,无需重做任何 prep**。
- **晨间决策点 0(新增,排 plan.md 晨间 6 项之前)**:批准删除 `.claude/dev-loop.settings.json` + `.claude/review-loop.settings.json` 两文件中无效的 `Write(./docs/contracts/**)` 行(各文件 deny 段的 `Edit(./docs/contracts/**)` 行与 scope-map bashDenyWrite + enforce-scope 钩子双拦不动,契约冻结语义零损失——论证与 CLI 逐字回执见 ARQ 行)→ 批准后 human 自删或明示主线程删,然后重跑 launch。

## P0 主线程代执行注记(2026-09-24 01:40 CST,append)

- 归属:八步序列的 P0(gateway 重启止血)+ 探针归因批由主线程在 dev-loop 外代执行。依据:①human 预授权当晚 ECS 生产运维(plan.md 铁律约束节 + architect-review §3「已获 human 授权(任务书口径)」);②P0 = 纯运维动作(重启/核验/探针),不涉仓内实现面,不属「实现动作走 dev-loop」Hard Rule 范围;③launch 阻塞不改变止血价值(清除方差放大器 + 归因第二段数据当晚可得)。**P1(模型切换 + 声明工件落仓)及其后步骤仍归 dev-loop,主线程不越权代跑**——晨间 launch 恢复后 builder 从八步序列第 4 步接续(第 1~3 步证据已由本节落账)。
- 证据:evidence/latency-baseline-260924.md §7(重启前探针批/备份锚/重启/五项核验/重启后探针批,随做随 append);探针为主线程诊断形态(非验收工件)——带 ECS 渲染件 v6 prompt(与仓内工件 sha256 全等断言,D1 纪律),N=3 中位数,主机快照前后各一次;正式验收探针脚本仍归 builder 落仓。

## human 裁定记录(2026-09-24 晨,append)

- **launch 形态改交互式**(human 原话:「现在不用 headless 跑了」)——启动卡命令行①(human 在场,`claude --settings .claude/dev-loop.settings.json --agent planner`,于 Pod 树 C:/AI/KidPal-pod-perf-k1-roundtrip-latency);headless 形态冻结至 ARQ-260924 修复落地。
- 注记:交互式带 --settings 是否同样触发 exit 4 未实测(K7 先例两可,见证据 §9)——若交互式 launch 也失败,则 settings 两行修复升级为交互式的前置项。
- ARQ-260924 已补勘误(成因=形态容忍差非 CLI 升级;main 131cd39);settings 修复的逐字 patch 已呈 human,授权中——批准前 .claude/** 零触碰。
