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

## 2026-09-18 · K1 merged → 本批 = 单包 K2，launch 面就绪（树池第 8 棒同树续跑）

**调度判定**：`260917-m2-k1-char-link-defect-fix` 已 merged（16b0d4e，mo28）；K2 dependsOn [K1] 已就绪、K3 dependsOn [K1, K2] 仍等 K2 → 本批 = 单包 K2（mergeOrder 29）。依赖图说了算，不硬凑并行。树池串行复用第 8 棒，同树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`（merge-driver rebase 步已同步至 ca7ff81 = main tip，交接卫生 tracked 净 / 无游离 untracked，白名单 build/ 与 managed_components 软链保留）。

**`--check-launch`**：`node .claude/scripts/portfolio-reconcile.mjs --check-launch 260917-m2-k2-retract-p4-three-page --base-ref base/m2-char-window` → ok:true、blocking 空、warn 空（present = K2 taskpackage.json + decision-coverage.json）。board worktreePath 已预填本树、`--status running` 由 Pod intake（planner Step 1）落。

**上游已知红基线（本包 launch 侧现跑，firmware 面）**：
- **firmware host 面**：**39 Tests / 0 Failures / 0 Ignored**、exit 0（2026-09-18 @ HEAD `ca7ff81`；量测时 ninja no-work、elf 为 K1 收口态产物）。差异判停：`git diff --quiet be3cf9f HEAD -- firmware/` **有差异**（K1 动 char_stroke.c / char_page.cc / custom_lcd_display.cc）→ 按 260915 T3 协议重跑本面，本行取代 be3cf9f 的 35T 行成为 firmware 面现行锚（35→39 的增量为 K1 新齿，非本包责任）。命令（**elf 相对 host_app/ 目录，从仓根跑 `./build/...` 必报 No such file**）：`cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash ./espidf_wsl.sh build && wsl -d Ubuntu-24.04 -- bash -c "cd /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app && ./build/host_infra_test.elf"`。面域 = firmware。
- **server 面**：`git diff --quiet be3cf9f HEAD -- server/` **零差异** → 引用本文件上方锚行 37 passed / 0 failed @ be3cf9f，不重跑。面域 = server。
- K2 包 intake 消费同协议：本包未动 firmware/ 前再次起 runner 即引用 ca7ff81 锚行。

**intake 修正（planner 权内，K2 taskpackage 四项拆包期事实偏差）**：① `origin.type/upstreamType` milestone→delivery-story（taskpackage-required-check 值域闭集不认 milestone，与 K1 r1 同款形态税——上游拆包器产出闸不认的枚举值，已在 K1 retro 登记为待上溯项）；② `acceptanceTestsFile` 原指 `firmware/main/test/render_pages.acceptance.c`——**该文件不存在**（260911-render-pages 套件已随 260915 归档批移入 `acceptance/archive/`），改钉本包新套件 `retract_p4_three_page.acceptance.c` + 两处在世套件；③ `sourceRefs` 补齐真实改动面 `key_gesture_parser.h/.cc` + `lcd_display.cc`（页点总数与页序单源住 key_gesture_parser.h、KP_PAGE→PAGE_Pn 页索引映射住 lcd_display.cc，只列 render_node.h/.cc + keybar.c 会误导 builder 以为改一处枚举即可）；④ `redBaselineCommand` 的 cwd 歧义修（同上）。**冻结钉点现实改指**：plan.md 点名的 `schedule_reminder_b.acceptance.c` 已归档不跑，实际成批触发 = `key_layer_dispatch.acceptance.c` 的 KD-E1 零触碰守卫（正向断言 key_gesture_parser.h 含 PAGE_P4）+ `keybar_three_slot.acceptance.c` 的 KKB-E1 infra 齿（字面钉 `PAGE_COUNT == 4`，与其自注「M2 后随枚举自动 4→3、不需改本文件」自相矛盾）——两处走 acceptance-author 留痕修订。

**环境事实**：图索引沿用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（随树增量刷新）；固件环境等价化沿用树内 sdkconfig / dependencies.lock / managed_components 软链（host 构建刚实证可用）。host runner 热缓存随树保留（本次 ninja no-work 即热缓存生效）。

**launch 启动卡**：`.git/devloop/launch/prompt-260917-m2-k2-retract-p4-three-page.txt`（四要素收缩形态）。

## 2026-09-19 · K2 merged → 本批 = 单包 K3，launch 面就绪（树池第 9 棒同树续跑）

**调度判定**：`260917-m2-k2-retract-p4-three-page` 已 merged（2608b68，mo29）；K3 dependsOn [K1(16b0d4e), K2(2608b68)] 全部就绪、mergeOrder 30 为末包 → 本批 = 单包 K3。依赖图说了算，不硬凑并行。树池串行复用第 9 棒，同树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、常驻分支 `dev/260915-m1-k1a-key-layer-dispatch`。

**交接卫生 + 树同步**：换包前 tracked 净（`git status --short` 空）、无游离 untracked；白名单 `build/` 与 `managed_components` 软链保留。常驻分支原停 933a123（2 commits behind main，merge-driver rebase 步在本包 launch 前尚未落）——`git merge --ff-only main` 快进至 **210bb6e**（main tip），无冲突、无 merge commit。

**`--check-launch`**：`node .claude/scripts/portfolio-reconcile.mjs --check-launch 260917-m2-k3-char-window-overlay --base-ref base/m2-char-window` → ok:true、blocking 空、warn 空（present = K3 taskpackage.json + decision-coverage.json）。board worktreePath 已预填本树（queued），`--status running` 由 Pod intake（planner Step 1）落。

**launchPreconditions 人读核（pod-coordination §2b 该门不含机械齿，260913 human 裁）**：六条逐条核——① commit-before-launch：四件套 + workstream 制品 + milestones.jsonl planningRefs 已 commit base ref（be3cf9f），STATUS.md 已重建；② 门 1 先行：architect-reviewer 轻量 checklist 五项 GREEN + workType 分流已 append `review.md` 门 1 节（2026-09-17）；③ deviation 拍板：页点 4→3 为 M1-K2 既定收回、无新增 deviation；④ 真机硬依赖：A7/A8 需 human 持板、K1/K2 host 半须先 merged——**两条均已 merged**，前提满足；⑤ 冻结面修订通道：`key_layer_dispatch.acceptance.c` 层表修订走 acceptance-author 留痕（本包特有，已写入启动卡）；⑥ 图索引：复用树沿用项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 随树增量刷新。全部满足，无未决项。

**上游已知红基线（沿用 260909 T5 机械来源口径，本包 launch 侧现跑 firmware 面）**：
- **firmware host 面**：**46 Tests / 0 Failures / 0 Ignored**、exit 0（2026-09-19 @ HEAD `210bb6e`；含 K2 新增 `retract_p4_three_page.acceptance.c` 7 齿）。差异判停：`git diff --quiet ca7ff81 HEAD -- firmware/` **有差异**（K2 动 keybar / kidpal_render_node / lcd_display / key_gesture_parser / mcp_server / 两处在世套件 + 新增套件）→ 按 260915 T3 协议重跑本面，本行取代 ca7ff81 的 39T 行成为 firmware 面现行锚（39→46 的增量为 K2 新齿，非本包责任）。命令（**elf 相对 host_app/ 目录，从仓根跑 `./build/...` 必报 No such file**）：`cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- bash ./espidf_wsl.sh build && wsl -d Ubuntu-24.04 -- bash -c "cd /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app && ./build/host_infra_test.elf"`。面域 = firmware。零红。
- **server 面**：`git diff --quiet be3cf9f HEAD -- server/` **零差异** → 引用本文件上方锚行 37 passed / 0 failed @ be3cf9f，不重跑。面域 = server（本包 server/ 零改动，M2K3-E2 铁律）。
- K3 包 intake 消费同协议：本包未动 firmware/ 前再次起 runner 即引用 210bb6e 锚行。

**环境事实**：图索引沿用树既有项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`（moderate + persistence，随树自动增量刷新）；固件环境等价化沿用树内既有 sdkconfig / dependencies.lock / managed_components 软链（native python 核 `os.path.isdir` = True；host runner 本次现跑实证可用，跳过重建；热缓存生效，ninja 无全量冷编译）。

**launch 启动卡**：`.git/devloop/launch/prompt-260917-m2-k3-char-window-overlay.txt`（四要素收缩形态）。

**launch 形态**：human 在本树会话内指示「启动 dev loop 按流程定义跑、无 block 不请示」→ 本树既有会话即 Pod 本体，按 `claude --agent planner` 主线程语义就地起 loop（对偶 `/build-feature` 入口）。不另起第二会话同树并行（树池协议「一个目录一个会话」）。

## 2026-09-19 · K3 闭环 → merged（mo30，M2 三包全并；树池第 9 棒同树续跑）

**结果**：`260917-m2-k3-char-window-overlay` 已 merged 本地 main（`2785578`，人批凭据 `587714c5`）。M2 workstream 三包（mo28/29/30）全部并入 main。

**交付面**：页→窗改造全套（查字窗层注册 `KIDPAL_LAYER_CHAR_WINDOW=5` + REMINDER/PROVISION 上移、键位语义矩阵新层列 + `GLOBAL_ACT_CHAR_REPLAY`、5 分钟驻留计时纯逻辑模块 `char_window.{h,c}` + 设备侧驱动、右栏对账三项、`canvas-p4`→`char-window` 锚改名、K2 遗留两处写侧死态 API 收拢）。host 行为面全绿 + 设备 ESP-IDF 真编译过 + A1–A6 各一条变异反证。

**过程要点（耐久项）**：
1. **验收面欠覆盖一处承重落地面**：5 分钟驻留的设备侧驱动封冻时无齿覆盖（seam 只钉纯逻辑模块）——builder 按纪律拒绝脑补并如实上报 → planner 判归属 → acceptance-author 补 `seam [9] + TEST 8`（设计态红）→ builder 补实现 → 转绿。**A7 真机轮的「5 分钟自动关窗」项在此驱动接线前不可能真绿**，这是本包最值得记的一条。
2. **验收齿强度被实现者反向证伪**：A5a「部首行补上屏」原以「切区内符号计数 ≥1」实现、**注释即可满足**（实现者变异自证实测不红）；已引入注释剥离器收紧 13 处同类正向计数齿，受控变异复现证实有效。
3. **完备性反查命中假红**：`retract_p4_three_page` 两条 M2-K2「保留不动、删除归 M2-K3」守卫齿在本包收拢后必假红——由 acceptance-author 独立反查发现并翻正（K2 retro 动作清单第二次复利）。
4. **拆包产物腐坏连续第三棒命中**：`origin.type=milestone` 不在闸值域闭集 + `sourceRefs` 漏四处真实改动面（kidpal_render_node.h/.cc 覆盖层闭集与两处待收 API / keybar.h+.c / lcd_display.cc / 板级 dispatch 消费链），intake 期逐条对磁盘核出并补齐。

**遗留（未闭环，归后续）**：
- **A7/A8 真机确认轮未做（human 持板）**：F3 确认轮（S-F3.1 AC1–AC4 / S-F3.2 AC1 / S-F3.3 AC1–AC2 / 窗口交互包全项含 5 分钟自动关窗 / 提醒压查字窗层级 / K1 两缺陷 live 复证）+ NFR-PERF-2 5 秒档复测。**本包 host 半闭环、真机半未做——不得读成整包已验收**（review-packet 已明记）。
- 层 renumber 三处已知残留（归档件数值字面 / `reminder_overlay_unify.acceptance.c` stale 注释 / 另两处在世件结构同型裸正扫齿）——建议后续批统一 refresh。
- `kidpal_render_node.h` 的 `begin_page`「递归死锁」注释与实证递归锁相左（既有 NIT，未改）。

**边界外移交项（主线程执行，已完成）**：`milestones.jsonl` M2-char-window `state` `planning → in_delivery` + `delivery-status.mjs` 重建 STATUS.md——经 human 2026-09-19 显式授权「现在就补」执行，落 `21c4dd9` 并 ff 入 main。**待判**：该里程碑行的 `nextAction` 仍描述 K1/K2/K3 已完成的工作（撤 P4 / 页→窗 / 锚改名），内容已过期——planner 按授权范围只动了 `state`，未擅改 `nextAction`，留给 human 或 portfolio 收口时对账。
