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
