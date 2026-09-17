# progress · m2-char-window（M2 阶段二·查字窗）

> workstream 级 launch 调度判定与基线锚的 append-only 记录（2026-09-17 建；行不覆盖）。

## 2026-09-17 · 拆包完成，门 1 待发起

**拆包记录**：里程碑 `delivery/milestones/milestones.jsonl#M2-char-window`（state=planning）nextAction 两块拆 3 包——

- `260917-m2-k1-char-link-defect-fix`（mergeOrder 28，dependsOn []）：查字链路两缺陷正式修复（LVGL 栈定容收编 + SVG 解析器补 C 命令）；
- `260917-m2-k2-retract-p4-three-page`（mergeOrder 29，dependsOn [K1]）：撤 P4 常驻页、页循环 4→3、枚举对齐 P1→P2→P3、keybar 页点 4→3（收回 M1-K2 deviation）；
- `260917-m2-k3-char-window-overlay`（mergeOrder 30，dependsOn [K1, K2]）：页→窗改造 + 右栏对账三项 + 锚改名 + F3 真机确认轮 + NFR-PERF-2 live 复测。

board 已初始化（三包 queued）；workstream roadmap ② 节机械渲染（render-roadmap.mjs）；decision-coverage.json 含 launchPreconditions 六条。

**重叠面核查（拆包前置）**：已核 260915-m1-k1a/k2/k3/k4 与 260917-m1-k4-rma8-fixbatch 五个已合并包——两缺陷均未触及（char_stroke.c 仍仅 M/L/Q/Z、custom_lcd_display.cc:126-131 16K 栈仍为一次性实验注记）；§2.2 行 67 提醒层级已由 K4/rma8 闭环，K3 只做查字窗侧对账。

**门 1 状态**：四件套齐备，**待 human 发起门 1**（architect 轻量 checklist review + human 过目切法）——GREEN 前不 commit-before-launch、不交 launch。

**基线锚**：未采——Pod intake 时 acceptance-author 实跑红基线落账（host runner 口径见各包 redBaselineCommand；k2 树池复用时的 consumer 差异判停规则沿用 m1-home-qa progress 2026-09-16 在案形态）。

**边界外移交项（portfolio-planner scope 拦截，主线程执行）**：
1. `delivery/milestones/milestones.jsonl` M2-char-window 行 planningRefs 挂 `delivery/workstreams/m2-char-window`（enforce-scope 拦：本角色只可写 taskpackages/** 与 workstreams/**）；
2. 挂接后跑 `node .claude/scripts/delivery-status.mjs` 重建 STATUS.md；
3. dev INDEX 行在包晋升 `delivery/changes/dev/<changeId>/` 时由 closeout 流自动刷入（changes-index `--upsert` 为 arch lane 专用，dev 新包无需也无法预先钉行）。

## 2026-09-17 · 门 1 GREEN，launch 面就绪（本批 = 单包 K1）

**门 1 裁决**：architect-reviewer 轻量 checklist 五项全过 = **GREEN**（裁决记录 = 本 workstream `review.md` 门 1 节）——覆盖完整 / DAG 与 merge order 一致 / 包边界单改不双改 / story 锚点引用合规 / 零契约改动裁定成立（D3 C 命令 = 解析补全，判 NO_CONTRACT_CHANGE，本门即充任 K1 的 architect 快审确认）。workType 分流：三包均实现型，K3 尾部挂 human 持板真机半轮（A7/A8）。附带观察（不阻断）：契约 §1.3 ①「切到 P4 查字页」为撤页前过时描述句，日后有其它契约改动时同 PR 顺带核对。

**调度判定**：本批 = 单包 K1（mergeOrder 28）。依赖图说了算——K2 dependsOn [K1]、K3 dependsOn [K1, K2] 均未就绪；当前无 running Pod。human 指示：拆包后在树池既有树内启动第一个包，树池串行复用第 7 棒（K1a → N1b → K2 → K3 → K4 → rma8 → 本包），同树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`（rma8 merged 后 merge-driver rebase 步已同步，不换不删）。

**commit-before-launch（F-03/F-05）**：拆包四件套 + workstream 制品 + milestones.jsonl planningRefs 挂接 + STATUS.md 重建，已 commit 本树常驻分支 `be3cf9f`；专用 base ref = `base/m2-char-window`（指向 be3cf9f）。`--check-launch 260917-m2-k1-char-link-defect-fix --base-ref base/m2-char-window` → ok:true，blocking 空。board worktreePath 已预填本树。

**上游已知红基线（260909 T5 机械来源口径，本树现跑 @ be3cf9f）**：
- **server 面**：`python -m pytest -q --import-mode=importlib -m "not live" c:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/tests` → **37 passed / 0 failed**（2026-09-17；面域 = server；现行面 = server/tests，旧 server/acceptance 已随 shim 退役整档归档、pytest.ini norecursedirs 挡递归——m1-home-qa 时代的「8 failed/119 passed」锚行作废，引用以本行为准）。零红。
- **firmware host 面**：WSL host runner 现跑（`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && ./build/host_infra_test.elf`，树内热缓存增量编译）→ **35 Tests / 0 Failures / 0 Ignored**，exit 0（2026-09-17；面域 = firmware）。零红——m1-home-qa 时代的三条既有红（VRT-A2/WSA-A6/WSA-A4）已随后续批次消解，本基线从零起算。
- Pod intake 按 260915 T3 差异判停协议消费本段：`git diff --quiet be3cf9f HEAD -- server/`（或 `-- firmware/`）零差异 → 引用本行不重跑；有差异 → 同命令重跑该面。

**环境事实**：图索引复用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（moderate + persistence，status ready，17172 nodes / 45427 edges，随树自动增量刷新）；固件环境等价化沿用树内既有 sdkconfig / dependencies.lock / managed_components 软链（host 构建刚实证可用，跳过重建）。

**launch 启动卡**：已落 git common dir `devloop/launch/prompt-260917-m2-k1-char-link-defect-fix.txt`（四要素收缩形态）。

**遗留 human 项**：launch 由 human 过目切法后执行（交互会话形态默认；主线程确认后可由 planner 即时启动）——本 skill 不代跑 launch。
