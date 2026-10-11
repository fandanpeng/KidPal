# Launch Card — 261011-p1-home-v5-rework

跑 261011-p1-home-v5-rework（独立变更，无 workstream，mergeOrder 47；② 单包 Full〔riskTier high〕：P1 首页落地 requirements v1.61——空态话术两态分叉 + 明天区多项化〔preview 数组 + 三态语义〕+ chip「明早」→「明天」定名 + 契约同步）。常驻树一树一会话串行接力（上一包 261010-device-reconnect 已 merged）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-10-11 · 261011 launch prep」段带 SHA 行（@4a9e87f 双面实测，两面零红）——server 面 144 passed / 0 failed / 4 deselected / 9 subtests；firmware host 面 153 Tests / 0 Failures。intake 差异判停：`git diff --quiet 4a9e87f HEAD -- <面域>` 零差异 → 引用锚行不重跑；有差异 → 重跑该面。
- **上一包留根**：`delivery/changes/dev/261010-device-reconnect-on-server-close/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：
  ① **契约已批（先批后落）**：`CONTRACT-CHANGE-PACKET.md`（同目录）Status=HUMAN_APPROVED（2026-10-11，human 契约门 AskUserQuestion 批准，pod-events human-decision 在案）。builder 落契约 diff = CCP §3 逐字 patch → `git apply`（**CRLF 保全**：契约文件 CRLF，apply 后人工 diff 逐字核对留痕——CCP 先例通道，非 Write/Edit 改契约）；契约面收口义务 C1/C2/C3 见 `architecture-gate-evidence.md` §6（CDQ 行 + crq/tech 记账 + ADR 追加归 integrator）。
  ② **冻结件修订授权链（plan R3）**：seam 形改触碰 schedule_grid.acceptance.c（A5 段 + :1037 签名齿）、p1_home_battery.acceptance.c（:693/:771）、p1_cjk_name.acceptance.c（:748-791）、server M3K1-A3——超出 tighten-only 窗口，**acceptance-author 派发 prompt 须明示授权 = CCP id + requirements §9 v1.61 行**，各冻结件 REVISION LOG 引用留痕，防 tighten-only 误拦或审查打回。
  ③ **producer 语义（plan R1）**：明日无项改发 `[]`、不再 `None`——不改则 AC6 话术永不触发；齿双向钉（`[]`→话术 / `null`→未达提示）。TECH-260928 school_start 同参齿保绿勿重复修（plan R5）。
  ④ **空态三态状态机（plan 要点 4，本包最重设计点）**：day_rollover 跨 0:00 清缓存后转数据未达态（清 `s_has_day` 或等价）——否则零点推送间隙误显「今天接下来没有安排啦」，验收齿必须覆盖。
  ⑤ **板端过渡容忍**：`kup_parse_preview` 遇旧单对象 `{time,name}` 容忍为 1 项列表（部署缓冲，一个过渡版，CCP 触点②；不入契约文本）。
  ⑥ **资产重烘 + 烧录**：chip「明早」→「明天」重烘（spiffs_root p1_chip_mingzao.bin 46x21 换内容 + p1_label_mingzao.bin 同批核，沿文件名换内容最省——efa8513 帽标先例）；重烘后全量 bin git diff 核证仅 chip 一枚 diff；烧录 = **app+art 双分区**（spiffs 资产变更，flash-asset-partition-rule，K4 教训）。
  ⑦ **设计稿母版内容级同步归 builder**（plan facePlan 顺序4）：kidpal-ui-design-v5.html 三处（:246 注 / :761-763 明天区块 / :784-785 规格列表）——「上学」行示例置首 + 空态话术占位 + v1.57 旧口径句改判为 v1.61 全量口径。
  ⑧ **riskTier high**：supervisor 必升 human，合并须人批；真机轮 human 在场持板为硬前置（空态两态 + 明天区多项 + chip「明天」看屏；ECS front 部署先行——ecs-front-deploy-pattern）。
  ⑨ **V5 范围收窄知悉**（spine openQ q3）：home_p1.c 已是行流 14px + 英文日期 + 黑帽标（260926 对齐轮在位），本批不做大字/倒计时重排——scoping 勿被 v1.57 过时对账注误导（spec 面勘误已登 spine openQ q4 deferred）。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
  ※ 本机实证坑（261010 台账）：`--dangerously-skip-permissions` 被平台分类器硬拦 → 实跑用**无旗标形态** `claude -p --settings .claude/dev-loop.settings.json --agent planner`。
