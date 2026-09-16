# progress · m1-home-qa（M1 阶段一·首页 + 百科问答）

> workstream 级 launch 调度判定与基线锚的 append-only 记录（本 workstream 无既有 progress.md，2026-09-16 起建；行不覆盖）。

## 2026-09-16 · launch 批：260915-m1-k2-keybar-three-slot（单包，M1-K2）

**调度判定**：本批 = 单包 k2。依赖图说了算——k3（mo25）dependsOn [k1a, k2]、k4（mo26）dependsOn [k1a, k2, k3]，均未就绪；k1a（mo22）、n1b（mo24）已 merged；当前无 running Pod，WIP=3 有余量。树池串行复用：K1a → N1b → 本包，同树 C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch、常驻分支 dev/260915-m1-k1a-key-layer-dispatch @ a398264 = main（merge-driver rebase 步已就位，不换不删）。

**门**：check-launch 2026-09-16 绿（`node .claude/scripts/portfolio-reconcile.mjs --check-launch 260915-m1-k2-keybar-three-slot --base-ref main`，blocking 空 / warn 空）。

**launch 前拍板（launchPreconditions 第 3 条）**：页点数量 deviation——阶段一按现行页循环渲染 4 页点（spec §2.2 冻结口径 3 点），4→3 对账归 M2 撤 P4 包收回。human 2026-09-16 AskUserQuestion 认可。

**交接卫生（复用树）**：tracked 净、untracked 零残留；managed_components 软链原生 python 验 isdir=True；build/ 热缓存随树保留。固件环境等价化与图索引按复用树规则跳过（图索引项目名 = C-AI-KidPal-pod-m1-k1a-key-layer-dispatch，随树增量刷新）。

**基线锚（producer 记锚，五要素）**：

- **server 面**：计数 = **0 collected（现役面空）**；命令 = `python -m pytest -q --import-mode=importlib -m "not live" --ignore=server/acceptance/archive C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance`；日期 = 2026-09-16；量测 HEAD = a398264；面域 = server。背景：ARQ-260916-server-mvp-teeth-sweep 闭账（77b6b0c）后 MVP/验证期 10 件整体归档入 `server/acceptance/archive/`，`acceptance/` 直下仅余 conftest——现役 server 验收齿为零。**量测口径警示**：裸 pytest（不带 --ignore）会递归收集 archive/ 并对现役 server 行为得 121F——这是归档件退役后仍被收集的 artifact（VRT-A2 先例同根），不是有效基线；launch 与 Pod 内量测必须带 `--ignore=server/acceptance/archive`。n1b r12 期的两条在册红（SMK-A4 / SRE-E4）随归档退出现役面，不再入基线失败面。
- **firmware host 面**：计数 = **13 Tests / 0 Failures**（n1b final r12，2026-09-16，8 基线齿 + n1b 新齿 5 全绿）；命令 = WSL host runner（`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && ./build/host_infra_test.elf`）；日期 = 2026-09-16；量测锚 = e990eb2（n1b 固件面最后漂移点；差异判停 `git diff --stat e990eb2 HEAD -- firmware/` 零差异，对本 HEAD a398264 有效引用）；面域 = firmware。Pod 内 acceptance-author 冻新齿时以 host 全量首跑为有效基线落账点（新测试无缓存、恒现跑）。

**consumer 差异判停（Pod intake 执行）**：两面各跑 `git diff --quiet e990eb2 HEAD -- <面域>`；firmware 面零差异 → 引用上述锚（SHA + 落盘锚 verbatim），不重跑；server 面现役面空态按重跑锚核（收集件数 = 0 即一致）；有差异 → 重跑该面（命令同记录）。

**遗留 human 项**：无——deviation 已拍板；launch prompt 交 human 在本树 VS Code 会话自跑（headless 为备选）。

## 2026-09-16 · launch 批：260915-m1-k3-voice-round-ui（单包，M1-K3）

**调度判定**：本批 = 单包 k3（mergeOrder 25）。依赖图说了算——k3 dependsOn [k1a, k2] 均已 merged（board 实读），k4（mo26）dependsOn 还含 k3 未就绪；当前无 running Pod，WIP=3 有余量。树池串行复用第 4 棒：K1a → N1b → K2 → 本包，同树 C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch、常驻分支 dev/260915-m1-k1a-key-layer-dispatch @ 0aed071 = main（k2 merged 后 merge-driver rebase 步已同步，不换不删）。

**门**：check-launch 2026-09-16 绿（`node .claude/scripts/portfolio-reconcile.mjs --check-launch 260915-m1-k3-voice-round-ui --base-ref main`，blocking 空 / warn 空）。

**交接卫生（复用树）**：tracked 净、untracked 零残留（git status --porcelain 空）；managed_components 软链原生 python 验 isdir=True；build/ 热缓存随树保留。固件环境等价化与图索引按复用树规则跳过（图索引项目名 = C-AI-KidPal-pod-m1-k1a-key-layer-dispatch，随树增量刷新，list_projects 实核在位）。

**基线锚**：引用上一段（2026-09-16 k2 批）两面锚——server 面 0 collected（a398264，--ignore=server/acceptance/archive 口径）+ firmware host 面 13 Tests / 0 Failures（e990eb2）；本 HEAD 已前移至 0aed071（其间隔两笔 ARQ-260916-unit-gate-archive-recursion gov commit），Pod intake 按差异判停协议逐面核（git diff --quiet <锚SHA> HEAD -- <面域>），零差异引用、有差异重跑。

**launch 方式**：human 指示本批免人工交接——环境就绪无问题后由主线程直接 Launch Planner（launch prompt 卡在 .git/devloop/launch/prompt-260915-m1-k3-voice-round-ui.txt）。遗留 human 项：无。
