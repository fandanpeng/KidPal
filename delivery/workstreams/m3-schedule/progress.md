# progress · m3-schedule

> 执行日志（append-only）：改了什么、测了什么、失败/修复。行不覆盖。

## 2026-09-21 · 拆包（portfolio-planner）

- 输入 = `delivery/milestones/milestones.jsonl` 行 `M3-schedule` nextAction 三块（sourceType=delivery-story）；story 机查两道通过（S-F1.1/S-F1.2/S-F1.3/S-F1.5 无已砍命中、可行性全 ✅）。
- 拆 3 包：`260921-m3-k1-cloud-schedule-producer`（mergeOrder 33）/ `260921-m3-k2-p2-week-grid-page`（34，dependsOn K1）/ `260921-m3-k3-voice-schedule-entry-live`（35，dependsOn K1+K2）。
- 重叠面核查结论（详 decision-coverage `_doc`）：无在办/已合并包触课表面冲突；旧 schedule_p2 冻结验收板/云两侧已 archive 退役；prompt v6 槽位空闲。
- **契约触点裁定：触契约面**——E1 §1.3② schedule view 旧列表形状 ↔ requirements v1.19 周网格口径 = spec↔contract 漂移 + 形状重定义；决策 26 首页 upcoming 下发通道契约无承载定义。CONTRACT-CHANGE-PACKET 草案落 K1 包目录，先走 architect-reviewer 四分类预审 + 人类契约门，契约 diff 随 K1 同 PR（launchPreconditions 第 1 条）。
- 真机半安排：K1/K2 的 live 半全部 structural deferred 挂 K3 设备轮（单真机不两轮撞车，M2 先例）。
- 四件套 + task_plan.md（INDEX 状态单一源）+ 双 input-brief（brief-inputs.mjs 拆包侧同批产物）落盘；roadmap ② 节 render-roadmap.mjs 机械渲染。

## 2026-09-21 · 门 1 + 契约预审（主线程派发，同日）

- **门 1（architect 轻量 checklist）**：首轮 verdict: BLOCK(F1)——三包 taskpackage `story` 字段写成技术方案句、违反 story 锚点引用冻结纪律（其余四项全过、实现现状四主张抽查属实）。主线程同日修复（story 字段改锚点引用 + ADV-1 决策 26 字面偏差注记补 K1 plan 取舍点 C），architect 复核转 **verdict: GREEN**。裁决耐久记录 = review.md 门 1 节。
- **契约预审（architect-reviewer 四分类，并行）**：触点①（schedule view 周网格）= SPEC_CONTRACT_DRIFT、触点②（home 可选 data）= CONTRACT_CHANGE_REQUIRED，状态 PRECHECKED；漏问点 a–f 已并入 CONTRACT-CHANGE-PACKET v2（Human Question 扩为五问：形状与 day 基 / 甲案与连带改写 / home data additive / upcoming producer 归属 / 过渡口径）。gate evidence 骨架落 K1 包 `architecture-gate-evidence.md`（十节，§3 收编预审裁定表）。
- **停等 human 两判断点**：① 门 1 后半 = 过目切法（GREEN 已在案）；② 人类契约门裁五问（K1 launch 硬前置）。commit-before-launch（base/m3-schedule）在两判断点后执行。

## 2026-09-21 · human 两判断点落定（同日）

- **过目切法**：human 认可三包拆法（roadmap ③ 节过目）。
- **契约门 HUMAN_APPROVED**：human 裁五问（packet §5 全文）——Q1 扁平周网格形状 + day 0 基映射 + **备注不进网格载荷**（网格/首页只放课程名，备注露面处 = 语音答复 S-F1.3 AC4 + 提醒 speech S-F1.4，human 问例「舞蹈课带护膝」确认口径）；Q2 甲案（薄载荷 front 展开 + §1.5/§1.6/§2.3 连带改写一并批）；Q3 home data additive；Q4 upcoming 由 front 机械算（human 确认 front = 自有网关、话术归 OpenClaw）；Q5 校验收紧。launchPreconditions 第 1 条翻 verified（evidence = packet §5 + gate evidence §4）。
- commit-before-launch：拆包产物 commit 到 base/m3-schedule（自 main 88b2ff82 建，仿 base/m2-char-window 先例）。

## 2026-09-21 · launch 面就绪（本批 = 单包 K1）

**调度判定**：本批 = 单包 K1（mergeOrder 33）。依赖图说了算——K2 dependsOn [K1]、K3 dependsOn [K1, K2] 均未就绪；当前无 running Pod、无 merge token holder。树池串行复用第 8 棒（K1a → N1b → K2 → K3 → K4 → rma8 → m2-charwindow-display-fix → 本包），同树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`（display-fix merged 后 merge-driver rebase 步已同步至 main 88b2ff82，不换不删）。

**commit-before-launch（F-03/F-05）**：拆包四件套 + 双 input-brief + workstream 制品 + milestones.jsonl planningRefs 挂接，已 commit 常驻分支 `40aefa4`；专用 base ref = `base/m3-schedule`（指向 40aefa4）。`--check-launch 260921-m3-k1-cloud-schedule-producer --base-ref base/m3-schedule` → ok:true，blocking 空、warn 空。

**上游已知红基线（260909 T5 机械来源口径，本树现跑 @ 40aefa4）**：
- **server 面**：`python -m pytest -q --import-mode=importlib -m "not live" "c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/tests" "c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance"` → **47 passed / 0 failed**（2026-09-21；面域 = server；archive 归档件由根级 pytest.ini norecursedirs 挡递归。m2-char-window 2026-09-17 锚行「server/tests 37 passed」后被 260920 display-fix 新增 test_char_meaning_merge 10 齿扩至本基线，引用以本行为准）。零红。
- **firmware host 面**：`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && ./build/host_infra_test.elf`（树内热缓存增量编译）→ **63 Tests / 0 Failures / 0 Ignored**，exit 0（2026-09-21；面域 = firmware。M2 期 35 Tests 经 rma8/display-fix 等批扩至 63）。零红。
- Pod intake 按 260915 T3 差异判停协议消费本段：`git diff --quiet 40aefa4 HEAD -- server/`（或 `-- firmware/`）零差异 → 引用本行不重跑；有差异 → 同命令重跑该面。

**环境事实**：图索引复用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（status ready，17345 nodes / 46026 edges，随树自动增量已跟上——server/gateway 面符号可达已实证）；固件环境等价化沿用树内既有 sdkconfig / dependencies.lock / managed_components 软链（host 构建刚实证 63T/0F，跳过重建）；board worktreePath 已预填本树。

**launch 启动卡**：已落 `delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/launch-card.md`（260921 T2 新落点，Git-tracked 随本段同 commit；五要素齐）。

**launchPreconditions 回写**（launch 执行者核验）：① 契约门 verified（packet §5 + gate evidence §4）；② commit-before-launch verified（40aefa4，本段）；③ 门 1 GREEN + human 过目 verified（review.md 门 1 节 + human 2026-09-21 过目认可）；④ 真机硬依赖 = package:K3（K3 launch 前再核，本批不涉及）；⑤ 冻结面与槽位 verified（archive 双侧在案、prompt v6 槽位空闲 2026-09-21 已核）；⑥ 图索引 verified（本段环境事实）。

**遗留 human 项**：launch 由 human 执行（交互会话形态默认）——本 skill 不代跑 launch。命令行两形态见启动卡第五要素。



