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
