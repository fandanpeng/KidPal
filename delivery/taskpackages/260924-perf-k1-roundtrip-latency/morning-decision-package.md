# morning-decision-package · 260924-perf-k1-roundtrip-latency

> integrator 成文版（2026-09-24，dev-loop 收口相位，plan.md 阶段划分 4 指定由 integrator 成文）。整合 plan.md「晨间 human 决策点」6 项 + `evidence/latency-baseline-260924.md` §7~§12 实测数据。**A2 质量门破线升为最高优先决策点置于首位**。原主线程夜间监督版（2026-09-24 08:20 CST）逐字留档于文末「附录 A」，不回改。

## 决策点 1（最高优先）：A2 质量门破线 —— 接受现状 / 回滚 B / 升 k2 首要（三选一）

- **事实**：`PERF-A2` 要求 display 块出现率与 schema 合法率**均 100%**。实测（判定器全量判定，非抽检）**P1 前后均不达标**：pre-P1 `display_rate=0.444` → post-P1 `0.222~0.444`，`quality_pass=False` 贯穿**全部 8 个探针批**（含基线）。课表备注读回在部分批次亦 `note_readback_ok=False`。
- **基线对照**：带生产 v6 的**基线探针同样复现**（`night-baseline-remeasure` display 0.667 → 仍 <1）——判据对新旧配置同等严苛，**非 P1 引入**。
- **根因**：基线固有的 v6 system prompt 遵循度不稳 / workspace「David」人设渗漏——实测泄漏形态含「你好呀,David」称呼、工具前导 `I'll check the schedule file`、内部注释 `<!-- project: path:/root/.openclaw/workspace -->`、v6 明禁的 `<details>` HTML 块；来源 = `tools.profile="coding"` + `DREAMS.md` 42117B 等 bootstrap 注入。**根因归 k2**，非本 Pod 范围。
- **planner 已裁（§12.1-1）**：plan.md `A2` 回滚触发线原文是「**切 A 后**任一批 display 合法率 <100% → 当晚回滚」，human `B2` 裁「A2 照 plan.md 原文执行」→ 该触发线是 **A 案作用域**，本 Pod 走 **B 案**故不触发；且 A2 未达标为基线固有，回滚既不能修复质量、又退回更慢状态（v4-flash + high，课表 10.7s）。→ **暂留 B 已应用，破线本身不豁免，上浮 human 拍板**。
- **你的三个选项**：
  1. **接受现状**（保留 B 案 `thinking=off`，A2 破线作 k2 首要立项依据）；
  2. **回滚 B**（一条命令：`python server/deploy/perf-k1/rollback_model_declaration.py --config /root/.openclaw/openclaw.json`，退回 v4-flash + thinking=high，课表 10.7s，**质量面不回改善**——A2 基线同样红）；
  3. **升 k2 为最高优先**（David 人设渗漏治理 + 静态上下文瘦身，优先于其它 k2 议题）。
- **备注**：B 案已带来实质延迟收益（A1 保底门全绿，见决策点 2/3 数据），回滚代价 = 退回 2 倍课表延迟且不解决质量；故 planner 倾向 1 或 3，但**裁决权在你**。

> **裁定（2026-09-24 午间，human 已定，本节即结论）**：human 原话「那个 display 块这个事情，**我们先观察一下效果吧，先不改它**，你这轮收尾吧」。
> 落法 = 选项 **1（接受现状）的加强版**：保留 B 案 `thinking=off`；**契约 §1.5/§1.6 与 A2 验收条文、decider、冻结齿一律不动**（不重冻）；**A2 照实判红、不触发回滚**；本包以**带债合并**收口；**先观察生产效果**，归 **k2** 收口（k2 二选一：改 prompt 让模型守住 `display` 格式，或按产品判断走**契约门**改契约）。
> planner 汇报的判据事实（已核实到源，全文见 `launch-prep-record.md`「human A2 裁定」节）：① 契约 §1.6 逐字要求「**每次回复**分两部分…末尾附一个 ```display 代码块」，**缺块即违反契约**，场景化的只有载荷厚薄；② 闲聊/天气该走的是 **§1.3⑤ `text` 视图**（把回答文字上屏，主要信息靠 TTS），契约给 `text` 配的示例句本身就是一句天气回答；③ **缺块的真实代价 = 屏幕上少一行字，答案照念给 TTS**（故为观感缺口，非「答不上来」）；④ 前端**坏块有兜底、缺块无兜底**（`front_app.py:231/244`）；⑤ 契约**第 268 行**把「能否稳定输出 ```display 块」列为待实测复验项——本包 0.222~0.667 即该待验项的答案：不稳定。

## 决策点 2：A3 真机轮（human 持板，会话早段，三源取证）

- **门**：引擎日志「大模型收到用户消息」→「发送第一段语音」，保底 闲聊≤8s / 课表≤10s；目标 闲聊≤5s / 课表≤7s（对齐公式 A3 = A1 + [1.5,2.5]s TTS 首句 + 转发）。三源取证（串口 COM5 / front journalctl / 引擎 docker logs）。
- **同日对照锚**（§10 pre-P1 真机基线轮，human 持板 09:41~09:43）：闲聊 **17.22s** / 课表 **13.64s** / 天气 **15.05s**——超 A3 保底门 1.7~2.2 倍；该轮**不替代 A3**（A3 门判定对象是 P1 后配置）。
- **P1 后引擎侧预期**：A1 探针（engineless 直连）post-P1 闲聊 4.14s / 课表 5.37s；叠加 TTS 首句 + 转发（约 1.5~2.5s）→ 预期 A3 闲聊 ≈5.6~6.6s、课表 ≈6.9~7.9s（保底门内、目标门边缘）。**引擎已重启给干净起点**（§12 步⑧，容器 11:29 重启）。
- **价值**：本决策点是「优化是否落到真机儿童体验」的唯一验收，也是 k2 立项的真机佐证。

## 决策点 3：模型定案拍板（B 案批准为生产常态 / 或改走 A）

- **本 Pod 现状**：live = `deepseek/deepseek-v4-flash` + `agents.defaults.thinkingDefault: off`（**模型未换**，B 案）。改档由网关 `[reload] config hot reload applied` 生效，**无需重启 gateway**。
- **实测支撑**（A1 两层门，各题 N=3 中位，带 v6 prompt 逐字）：
  - **B（off）三批保底门全绿**：闲聊 4.135 / 4.784 / 4.258（≤6 ✓）、课表 5.369 / 5.992 / 5.274（≤9 ✓）、天气 8.054 / 9.553 / 5.971（≤14 ✓）。
  - **目标门**：课表（≤6 ✓）与天气（≤10 ✓）达标，**闲聊（≤4）差 0.1~0.8s 未达**——plan 原文「目标门未达 = k2 立项依据、不算当晚失败」，非判红项。
  - **反证杠杆归属**：`thinking=low` 实测无收益（课表 11.868s）；内联 `/think:off` 不改配置**无效**（课表 10.49s）；**唯配置档 `off` 是真杠杆**。
- **若维持 B（推荐）**：模型未变 → **无 `docs/architecture.md` §2 依赖表 / 决策 19 型号名漂移**，docsSync 义务已履行（落点 = `docs/ADR.md` ADR-006）、**不需另起型号同步**。
- **若改走 A（换非推理模型 `deepseek-chat`/`deepseek-flash`）**：须**另起** architecture.md 型号名 docsSync 或 ARQ 记账（二选一不许静默漂移）；成本注记 = `deepseek-chat` 相对 v4-flash input 约 2x / output 约 1.5x（单设备量级绝对成本微小）；且须同时改 `agents.defaults.model.primary` 与 `modelPolicy.allow`（qqbot 与 voice 共 main agent，一并波及）。
- **待澄清**：A 案 `allow` 路径形状不一致（冻结齿用顶层 `modelPolicy.allow`，生产为 `agents.defaults.modelPolicy.allow`）——若走 A 须先收口该缺口（见 review-packet Findings #2），避免齿绿而生产接线错路径的假绿。

## 决策点 4：共 agent 回归（QQ 录入 1 条 + 提醒 semantic.json 形状抽查）

- **背景**：qqbot 与 voice **共用同一 main agent**（实证：唯一 agent、零 routing）——改 primary/thinking 档必同时波及 qqbot。本 Pod 未做结构隔离（B 案零结构隔离），代偿 = 提醒 semantic.json 形状冒烟 + QQ 录入回归。
- **待办**：QQ 录入 1 条测试安排（查 MD 改写 + 三档重算触发 + 提醒文案质量）；提醒 `semantic.json` 产物形状抽查 1 次。
- **已做的降级证据**：P0 五项核验中 cron `kidpal-reminder-plan` 在册、qqbot 通道重连 READY（§7.3 + §12 改配置后复跑全绿）——不等于录入回归，录入语义面仍待晨间。

## 决策点 5：P3 —— ECS 4GB 升级 / 过渡项取舍

- **夜间数据**：ECS 2 vCPU / **1870MB RAM**；P0 重启后 swap 734→**251MB**、memory pressure 告警清零（§7.3）；但基线期 swap 常态 250~790MB、同题半小时 2 倍方差（§4/§6）——P0 只清方差放大器，**不解决多轮工具路径**（课表 11.2s 不受重启影响）。
- **human `B3` 已裁**：4GB 升级**暂缓**（需购订阅，「实在不行再升级」）；零成本过渡项候选（每夜 gateway 重启 cron + sessions 定时维护）归 k2 批次议。
- **判据**：方差如再放大即升级依据；可与 OpenClaw 升级窗口（决策点 7）合并停机。

## 决策点 6：k2 立项（携本包 P1 数据）

- **范围候选**（plan 原列 + 本包实证加权）：
  1. **「David」人设渗漏治理**（**升为最高优先**，决策点 1 破线根因）：`tools.profile="coding"` + workspace bootstrap（`DREAMS.md` 42117B / `AGENTS.md` 9795B / `USER.md`「David」）——既治质量渗漏又减上下文。
  2. **静态上下文瘦身**（~25k tokens/请求：v6 22.5KB + bootstrap 注入）——社区经验「~80% 慢回复第一成因」佐证为第一杠杆。
  3. **session store retention**（511 会话 / 93MB，生产固有速率增长，slow SQLite hold 活跃）。
  4. **课表 3 轮工具循环收敛**（prompt v7，槽位预约先行；注意 v6「一回合作答:不调用任何工具」与「读 schedule.md 筛选」的内部张力）。
  5. **ttfc == total 流式结构调研**（28/28 样本 run 完才吐流——缓冲点在 agent loop 还是 openai-http 层）。
  6. 引擎侧历史治理 / per-channel 模型隔离与 `openclaw/default` 名称路由语义。
- **立项材料**：本包 §12 三段归因 + 8 批结构化探针 + A2 全量判定 + ★新实证 = David 渗漏为**带 v6 也复现的生产面问题**。

## 决策点 7：OpenClaw 2.0.4（npm）升级窗口

- 须过 `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` §0 版本漂移闸（char-stroke dist 锚点）；可与 P3 停机窗口合并。
- 版本口径双号并存：npm 2.0.3→2.0.4；产品 `openclaw --version` = OpenClaw 2026.8.1 (ea80657)（§6 勘误）。

## 附：簿记漂移（顺手报，非决策点）

- board K7 行 status=blocked 而其 pod-events 有 merge-completed（2026-09-23T12:50Z）——人工路径合并未回写 board，待治理批或顺手修正。
- 昨晚 catalog 更新（22:43「remote model catalog updated」）已被 P0 重启应用；startup 显示模型/thinking 档当时未变，留档。

---

## 附录 A：主线程夜间监督版（2026-09-24 08:20 CST，历史留档，不回改）

> 说明：以下为 dev-loop launch 前的夜间临时版，保留以求过程账完整；其中「决策点 0（settings 修复）」已由 human 同日终裁关闭（`launch-prep-record.md`「human 终裁补记」：settings 两行不删、headless 冻结、launch 一律交互式）。原「晨间决策点 6 项」已由本件正文整合、更新并重排。

### 决策点 0（阻塞项，最优先）：批准 settings 两行修复 → Pod 才能 launch（**已关闭**）

- **事实**：headless launch exit 4，CLI settings 校验拒绝 `Write(./docs/contracts/**)` deny 规则（逐字回执：「…not matched by file permission checks — only Edit(path) rules are. Use Edit(./docs/contracts/**) instead (Edit rules cover all file-editing tools).」）。受影响：`.claude/dev-loop.settings.json:25` + `.claude/review-loop.settings.json:14`（review loop 下次跑同样会炸）。
- **修复**：两文件各删一行 `Write(./docs/contracts/**)`（deny 段的 `Edit(./docs/contracts/**)` 行保留——按 CLI 口径它覆盖所有文件编辑工具；scope-map bashDenyWrite + enforce-scope 钩子双拦不动）。**契约冻结语义零损失**（论证详见 ARQ-260924-settings-write-deny-rule-cli-drift 行，main 017b24a）。
- **为何需你**：主线程两种修复形态均被 auto-mode classifier 拦截（Self-Modification：移除 deny 规则须 human 本人提出）——拦截正确，机械强制层不 agent 自批。
- **批准后动作**：你说一声「批准删这两行」→ 主线程（或你亲手）删行、commit main、cherry-pick 到 Pod 分支 → 按启动卡命令行②重跑 launch（prep 九件全有效，零重做）。

### 夜间已完成（不需要你操作，只需知悉）

1. **延迟根因定案**（三源证据）：10~15s 的大头 = OpenClaw 网关内部——reasoning 模型（v4-flash **thinking=high**）单轮生成 + agent 多轮工具循环（课表/天气各 3 次上游）+ 每请求 preflight；ASR≈0、TTS 首句 1.2~2s、设备/front/引擎转发无责。
2. **architect advisory 评审**收口（Track B/medium/不触契约；D1~D4 修正吸收，plan.md FROZEN v1.0）。
3. **TaskPackage + launch prep 九件**全绿（taskpackage-required-check ok / check-launch ok / 基线锚 server 面 57 passed 0 failed @1746c14 / 图索引 full 17952==expected / board queued）。
4. **P0 止血已执行**（主线程代跑，归属注记在 launch-prep-record）：gateway 重启 08:12:57，五项核验全绿，swap 734→251MB，pressure 告警清零；重启前/后/稳定性三批带 v6 探针（N=3 中位数）落账 evidence §7。

### 夜间新发现（改变部分晨间议题的权重）

- **A. 「David」人设渗漏已实证为 live 生产面问题**：带 v6 prompt 的探针里，闲聊轮双峰——快样本（4.4~9.3s）全部滑向 workspace「David」人设且丢 display 块，慢样本（9~17s）才守「小伴」纪律。**质量与延迟耦合：v6 遵循 = 慢路径**。k2「人设注入质检」升为高优先（儿童生产会话可能收到「David」称呼+无 display 块答复）。
- **B. thinking=high 是活配置**（startup 日志逐字在档）——B 案（v4-flash 关/降 thinking）有具体抓手，Pod 步⑤ timebox 内裁 A/B。
- **C. 天气题 display 块 0/6**（两批全缺；昨晚 live 轮有块）——display 纪律在天气路径现配置已不稳；A2 门（100%）对现状同样红，定案时带此认知。
- **D. ttfc==total（28/28 样本）**：OpenClaw run 完才吐流——引擎无法提前 TTS 首句；优化标的 = 首轮 agent run 总时长（k2 结构调研：缓冲点在 agent loop 还是 openai-http 层）。
- **E. session store 529 且生产固有速率增长**（每轮 ~25k token 行）——k2 retention 议题实证 +1；重启后 error|warn 11 行未逐条归因（留 Pod）。

### 晨间决策点（architect §7 六项，按夜间数据更新）

1. **A3 真机轮**（你持板，Pod 落地 P1 后跑，会话早段）：保底闲聊≤8s/课表≤10s，目标 5s/7s；三源取证。
2. **共 agent 回归**：QQ 录入 1 条测试安排 + 提醒 semantic.json 形状抽查（qqbot 与 voice 共 main agent，模型切换必然波及）。
3. **模型定案拍板**（P1 数据出来后）：A（deepseek-chat）/B（v4-flash thinking off）批准为生产常态 + 成本注记（deepseek-chat input 2x/output 1.5x）+ architecture.md §2 依赖表与决策 19 型号名 docsSync 或 ARQ 二选一。
4. **P3 拍板**：ECS 4GB 升级（停机 resize 窗口）vs 过渡项（每夜 gateway 重启 cron + sessions 维护定时）——夜间数据：2GB 下 swap 常态 250~790MB、pressure 告警 5min 一次、同题 2 倍方差。
5. **k2 立项**（携当晚数据）：静态上下文瘦身（~25k tokens/请求）/ **David 人设渗漏治理（升优先）** / session store retention / 课表 3 轮工具循环收敛（prompt v7，槽位预约先行）/ ttfc==total 流式结构调研 / per-channel 模型隔离 / openclaw/default 路由语义。
6. **OpenClaw 2.0.4（npm）升级窗口**：过 runbook §0 版本漂移闸（char-stroke dist 锚点），可与 P3 停机窗口合并。

### 簿记漂移（顺手报）

- board K7 行 status=blocked 而其 pod-events 有 merge-completed（2026-09-23T12:50Z）——人工路径合并未回写 board，待治理批或顺手修正。
- 昨晚 catalog 更新（22:43「remote model catalog updated; restart the Gateway to apply it」）已被本次 P0 重启应用；startup 显示模型/thinking 档未变，无需动作，留档。