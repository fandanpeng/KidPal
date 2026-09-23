# Launch Card · 260924-perf-k1-roundtrip-latency

跑 260924-perf-k1-roundtrip-latency(独立包,无 workstream——human 2026-09-23 夜直令「分析慢的问题…确定方案后建 taskpackage、launch prep、启动 devloop,明早看效果」;mergeOrder n/a,riskTier medium / rigor full)。

## 基线锚(launch-prep 差异判停协议)

- **server 面**:见 `launch-prep-record.md` §基线锚 带 SHA 五要素行(Pod intake:`git diff --quiet <anchor-sha> HEAD -- server` 零差异即引用、有差异重跑)。
- firmware host 面:本包 subsystem=server、firmware/** 零触碰——不适用(如 verifier 意外触 fw 面,按协议现跑并如实记)。
- 本包验收红基线(acceptance-author 新冻结齿)恒现跑,无缓存可言。

## 上一包留根

`delivery/changes/dev/260923-m3-k7-schedule-grid-restore/pod-retro.md`——intake 逐条评估(构成 blockingDecision → fail-closed 上报)。注记:board 上 K7 行 status=blocked 而其 pod-events 已有 merge-completed(2026-09-23T12:50Z,主线程人工路径簿记)——board 漂移已报晨间 human,不阻塞本包。

## 固件构建闸

固件严格模式默认开:`VERIFY_FW_BUILD_REQUIRED=1`(无关闭留痕即开,260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量,未设即降级,此声明行随卡走不得省)。本包 server 面,预期不触发 fw build。

## 命令行(两形态并列)

① 人在场交互式:`claude --settings .claude/dev-loop.settings.json --agent planner`
② 无人值守 headless:`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`(另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`)——**本次采用 ②**(human 夜间预授权,晨起看效果;launch 由主线程代跑,偏离「launch 交 human 自跑」默认已如实记录于 launch-prep-record.md)。

## 交接注记

- dispatch-stamp 恒传 `--task-package-id 260924-perf-k1-roundtrip-latency`;mutation-check 回填前先读 checker 的 id 提取器/匹配语义(ARQ-097 口径)。
- **ECS 生产运维已获 human 当晚预授权**(重启 gateway / 经仓内脚本切模型 / 条件步 sessions 清理 / 引擎 docker 重启)——逐步留痕即可,无需再升级;真机 A3 轮 = 晨间 human 持板,当晚不代跑不造假绿。
- 执行序列权威 = architect-review §6 八步(plan.md 阶段划分已吸收);ECS 纪律 = kidpal-ecs-gateway skill(config trap:live 配置在 `deploy/xiaozhi-docker-data/.config.yaml`,别读诱饵)。
- 晨间 human 决策点 6 项见 plan.md;integrator 负责 morning-decision-package.md 成文。
