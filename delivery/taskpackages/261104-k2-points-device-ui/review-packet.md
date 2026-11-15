# Review Packet — 261104-k2-points-device-ui（v2 · 修复批后收口）

> integrator 收口 v2（2026-11-14，本树 dev/pod-m2）。包级 base ref = b96b978d（首轮），
> 首轮收口 = 53944b5b，真机轮 + 修复批增量 = 53944b5b..073b5050（实读 8 提交）。
> 真机轮与复验轮证据见 `device-round-evidence.md`；human 收口原话（2026-11-14）
> 「可以了，合并吧」。承重 final = round 4（invocationId 420cf1ea-f428-4a7a-8ccc-b5ef351f3bd2，
> ok:true，10/10 门全绿）；round 3 红闸（b7c4636b declaration 回归）留痕保留，处置见 Findings。
> 本包交 supervisor 合并门终裁，integrator 不 push、不合并。

## Change Size

- **首轮实现面**（b96b978d..53944b5b）：1609 insertions / 73 deletions，23 路径 changeManifest
  （设备积分本四态主页/进度线/详细账本/删除确认/keyset 取页/积分语境/语音闭环 + front 积分意图
  消费与分页步进修正；零契约/requirements/architecture 改动）。
- **修复批**（53944b5b..073b5050，实读 7 提交）：实现面 20 文件 +1250/−60，交付制品面另 6 件。
  分布：firmware display/mcp（kidpal_render_node.cc、keybar.c、mcp_server.cc）+ 三枚资产
  （pt_cat_34_28 / sc_wheel_42_42 / ic_coin_36_36，24px 版删除superseded）+ 两冻结件同值重锚；
  server front（front_app.py 建连初推 + dedup 语音反馈）、engine-patch（connection.py 认领
  kidpal_points_context + prompt 占位）、openclaw 部署面（AGENTS 路由许可 + kidpal-points-answer
  SKILL + declaration 四项 + manifest/README）、test_261017 重锚（+51/−17，三→四项，非放宽）；
  tools 资产管线（gen_art_spiffs / gen_assets / 模拟器 assets.c）。
- **Scope**：设备积分本正式化 + 真机轮缺陷收口（F1-F11）；零契约/requirements/architecture
  改动；K1 权威账本零重算零本地持久化（K2D-A4 单写入口锚）。
- **Split recommendation**：不拆——K2 单包承载设备积分本全链，mergeOrder 73（依赖 K1 已合）。

## Verification Evidence

- **首轮 verifier 内循环**：8/8 GREEN（两轮收敛；第 1 轮红 = perAcceptance 空骨架，fixer 补证后
  第 2 轮全绿）。
- **首轮 final（r2，承重，已由修复批前的历史收口确认）**：invocationId
  3fb1729b-b9fa-47bd-9629-d71f696776f2，ok:true，10 闸全跑零 notExecuted（详见 r1/r2 留痕与
  verify-runs.jsonl）。
- **修复批 final 范围圈定（Step 4①）**：
  - 清单落盘路径一行锚 = `delivery/taskpackages/261104-k2-points-device-ui/test-scope-final-fix.txt`
    （本修复批终验全部跑测以该清单为范围）。
  - detect_changes（project=C-AI-KidPal-pod-m2，since=53944b5b，inbound，depth 2）：图索引可用
    → 正常圈定非降级；changed_files=26，seed_symbols=85，impacted_total=50；impacted_modules =
    firmware/main 14 / server/acceptance 33 / server/gateway 3。范围 = 本包 acceptanceTestsFile ∪
    修复批变更测试档 ∪ blast radius 映射测试 ∪ impactedTestsSeed ∪ 共享 seam/安全余量 = 24 个
    测试档（firmware 9 / server acceptance 12 / server units 3）。
- **修复批 final round 3（红，留痕，不修 fact）**：invocationId
  bf593558-7e37-496e-9580-7bba40998e1e，ok:false，10 门 8 绿 2 红。红 = `unit`（`4 failed,
  205 passed, 1 warning, 3 subtests passed`）与 `acceptance`（`4 failed, 124 passed, 1 warning,
  3 subtests passed`），四条失败同在 261017 冻结件（declaration drift_guard 回归，根因与处置见
  Findings F12）。matcher 校验：`verifier-evidence-check` 输出
  `{"ok":true,…,"factOk":false,"observedGateExit":1,"phase":"final","round":3}`（结构有效、事实红）。
- **A 案修复 073b5050**：declaration `drift_guard[0].expect` 回退 `"unset"`（261017 冻结契约形状），
  live 四项实况与备份路径沉淀 `_note`。fixer 实跑在案：test_261017 单件 10 passed、server 全量
  415 基线零新增红。
- **修复批 final round 4（承重，绿）**：invocationId 420cf1ea-f428-4a7a-8ccc-b5ef351f3bd2，
  `verify.mjs --evidence-mode=round --phase=final --scope-file=<上述清单>` → **ok:true，
  10 门全跑零 notExecuted**：taskpackage-required ✓ / change-manifest ✓（pre-cutoff 存量放行）/
  firmware-build ✓（**真编译 fresh：xiaozhi.bin 0x328aa0 bytes、app 分区余量 8%**）/
  contract-lint ✓（1 条存量 warning：arch 历史批相对链接断链，非本包引入）/ acceptance-coverage ✓ /
  mutation-check ✓ / docs-sync ✓（docsSyncRequired=false）/ security ✓ /
  unit ✓（**209 passed**，1 warning，3 subtests）/ acceptance ✓（**128 passed**，executor-run，
  12 档、liveDeselected=0，1 warning，3 subtests）。
  matcher 校验：`verifier-evidence-check --round 4` → `{"ok":true,…,"factOk":true,
  "observedGateExit":0,"phase":"final","round":4}`，exit 0 —— fact ok:true，进 closeout。
- **server 全量与 host 基线引用**：server 全量 415 = fixer/builder fresh 证据（修复批
  fixer-complete + 073b5050 fixer 实跑，pod-events 在案）＋本轮 scope 内 209/128 实跑佐证；
  host 全量 287/3 = 修复批 builder-complete fresh 证据（3 红全 p1_auxbar AB-A1/A2/A4 存量族，
  非本包引入）；设备编译由 round 4 firmware-build 真编译 + 真机轮烧录 hash 校验双重承载。
- **Security**：securityReviewRequired=false，闸绿。结论一句：无扩大写权——设备零本地账本写盘
  （K2D-A4 单写入口锚），删除只走按键确认 + 私有上行；F1 结构化通道给模型的积分 skill 明列
  「零写权、state 只读、不调写命令」；openclaw.json（含 gateway 凭据）整文件不入仓，仅入无 secret
  声明；零新增依赖。

## Demo 对账（K2D-E1..E5 逐条刷新）

| Exit criteria | 判定 | 在手证据 |
|---|---|---|
| K2D-E1 冻结验收覆盖四态/最近三条/越目标/负余额/单行与全量分页/键位/删除确认/冷启动恢复；红基线、变异证据、修订留痕齐全 | **绿** | host 冻结件 K2PTS-P1..P7 7/7 绿（redBaseline host 7/7 RUN_RED 实跑在案）；mutation 11/11 turnedRed；修订留痕 = K0PTS-A5 重钉（ruling5）+ 两件同值重锚（ruling1）+ retract_p4 扩单（ruling14），pod-events 在案 |
| K2D-E2 host 与 server 离线证据：restore/显式打开分流、revision 重置清缓存、selected_id 只读注入、删除幂等对账、不完整句零写入；story/schedule 路由零新增红 | **绿** | server 冻结件 K2D-A1..A4 4/4 绿；修复批 round 4 scope 内 server acceptance 128 passed、unit 209 passed 零误伤（含 m4k1 / record_dialogue_guide / K0PTS-A5 重钉件 / record_cancel / record_front_polish 等 blast radius 命中族）；host 287/3 判停（3 红存量族） |
| K2D-E3 一轮真机覆盖语音加分/兑换、只提事件不记账、金额不匹配与余额不足拒答、短窗重复、断网可见失败、删除重算与负余额、冷启动恢复；看屏与三源日志互证 | **绿（真机轮 PASS）** | `device-round-evidence.md` §一 场景 PASS 台账八项（L1.1/L2.1/L2.2/L2.3/K1L-L1/L1.2/L1.3/种子清理，三源互证）＋ §五 复验轮 PASS（F10/F11/F9 机制/F1/断网/兑换反路径，human 逐项确认）；L1.2 短窗重复零写入 + takeover 照推；兑换正向归 K4（§三 环境限制） |
| K2D-E4 六张 V9 积分帧 design review；长原因/大分值/越目标/负余额不破坏 400×300 1bpp 版面；NFR-PERF-4 只记录实测 | **绿（真机看屏 PASS）** | §五 human 看屏确认：金币 36px 定档 + 右缘位置（数字右对齐贴右缘 388、金币贴数字左 6px、同排视觉居中）、pt_cat 小伴、sc_wheel 摩天轮（默认目标=游乐场位）、键帽「详·双击」、删胶囊、F7 长原因截尾不溢框；六帧结构按 V9 落。**「.coin vs ic-coin」取舍申报**：V9 稿资产标识 `.coin`（24px）与实现资产 `ic_coin_36_36`（36px）命名与尺寸均偏离——24px 星形消失根因 = gen_assets 的 svgUse 只注 defs 不注 symbol（ic-coin 内 use 悬空）；36 档外环/内圈/实心星三者可辨（32 档断线、40 档权重压摩天轮）；取舍 = 尺寸以看屏三档肉眼对账取 36px、块名沿用管线实际名 ic_coin_36_36（V9 标识为设计示意、非管线命名契约），human 看屏确认。NFR-PERF-4：语音首轮延迟偏大，只记录不设阈 |
| K2D-E5 diff ⊆ changeManifest；K1 权威账本不被设备重算或持久化；demo 对账与未覆盖面申报；K3/K4 复用本包缝 | **绿** | change-manifest 闸（pre-cutoff 存量放行，见下「manifest 扩单记录」）；K2D-A4 单写入口断言 + K2PTS-P4/P5 设备只读消费断言；restore/进度线缝在 kidpal_render_node 与 points_page 公开头文件在位 |

## Findings（修复批后状态；F1-F11 逐条含处置/落点提交/遗留）

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | launch-card / 合并链 | deviceOutcome 原申报 hostUncompilablePaths 含 protocol.cc，不在登记集会被收口闸判 BLOCK | planner ruling6 + 收口闸 device-outcome-host-path-unregistered 实证 | **已闭合**：launch-card 收窄为实际 touched 四路径（mcp_server.cc / application.cc / lcd_display.cc / kidpal_render_node.cc），不动登记集、不改实现 |
| NIT | judgement-call | server/acceptance | K0PTS-A5 wire 半重钉（K0 冻结件钉固定步进 1，与一屏 6 行观感冲突） | changed hunk test_points_e2e_spike.acceptance.py +66/−20 + planner ruling5 | **已闭合**：重钉后单跑 6 passed + 全量 415 passed；supervisor 复核扩单 |
| NIT | judgement-call | firmware/display | 主页目标名以文字行替代场景资产图标 | builder 申报 + taskpackage decisionCoverage D-09 | **部分闭合**：ruling9 本批接 sc_wheel 为默认目标位场景标（回应「游乐场没有」）；四预置切换/自定义伸缩仍归 K4 D-09 |
| NIT | judgement-call | firmware/display | story_page.c 语境映射落点自由度 | changed hunk story_page.c + planner ruling3 | no-trigger |
| **BLOCKING → 已闭合** | **hard-violation** | server/deploy/openclaw | b7c4636b 把 `kidpal-agent-skills-declaration.json` 的 `drift_guard[0].expect` 由字符串 `"unset"` 改为三项数组，违反 261017 冻结契约（expect 必须非空字符串且须含 `{"expect":"unset"}` 锚；apply 夹具现状=未设 skills → apply 退 3），round 3 红 4 条 | `server/acceptance/test_261017_openclaw_kidpal_skills.acceptance.py` 头注冻结契约节 + b7c4636b declaration diff + b1fa2f3f/431cdf4b 两版 expect 均为 `"unset"`（回归时间线） | **已闭合 073b5050（A 案）**：expect 回退 `"unset"`，live 四项实况与备份路径沉淀 `_note`；round 4 10/10 绿 |
| 观感 | judgement-call | firmware（F1） | 引擎补丁缺 kidpal_points_context 认领，front 注入语境被引擎丢弃，模型收不到余额/目标/最近分录 | device-round-evidence §二 F1 + front/引擎三源日志 | **已闭合**：873a3f17 引擎补丁认领 + prompt 占位（261022-m5-n2a 先例）；复验发现模型仍「看不到」→ human 裁定走 OpenClaw skill 结构化通道，431cdf4b 落 kidpal-points-answer + AGENTS 路由/禁令例外 + declaration 四项 + manifest/README + test_261017 重锚（ruling15/16），b7c4636b→073b5050 锚处置；human 确认问分链路可用 |
| 观感（F2/F5） | judgement-call | firmware/display | 进度线小伴标记为 16×16 实心方块；积分图标 ★ 应为金币 | device-round-evidence §二 + changed hunk kidpal_render_node.cc | **已闭合**：ba83a58d pt_cat_34_28 换方块（失败回落）+ ic_coin 换星；0426d670 定档 36px + 右缘位置 + svgUse 注 symbol 根治漏画；human 看屏确认 |
| 观感（F3） | judgement-call | firmware/display | 主页「最近得到」行缺 before→after | device-round-evidence §二 + changed hunk | **已闭合**：ba83a58d 主页 recent 补 before→after（列位对齐账本页）；看屏确认 |
| 观感（F4） | judgement-call | firmware/display / 键帽 | 「详细·双击看」胶囊占位，文字压缩 | device-round-evidence §二 + planner ruling11（措辞待 human 定） | **已闭合**：ba83a58d 详细提示移入 KEY 键帽（「详·双击」）+ 删胶囊；keybar_three_slot / retract_p4 同值重锚（ruling14）；看屏确认 |
| 资产（F6） | judgement-call | firmware/资产管线 | 管线白名单 68 枚闭集无金币/小伴/场景标 | device-round-evidence §二 + changed hunk gen_art_spiffs/gen_assets/assets.c | **已闭合**：ba83a58d 母版纯追加 573 行 + 白名单 75→78 + 三 bin（KAP1 校验、零搅动）；ruling12 gen_art_spiffs 头注单源修正；ruling13 资产白名单闭集验收自 260915 归档后悬空→挂账（loop-retro/ARQ 候选） |
| 观感（F7） | judgement-call | firmware/display | 删除确认覆盖层长原因折行溢出圆角框 | device-round-evidence §二（看屏照片） | **已闭合**：ba83a58d 行高=单行行高触发 DOT 截尾 + 删除框 reason 定宽 72；看屏确认不溢框 |
| 轻微行为（F8） | judgement-call | firmware/display | 末屏 BOOT 双击回绕重拉首页，回绕 or 停住冻结件未钉 | device-round-evidence §二 + §五 遗留 | **遗留挂账**：默认保留回绕，待 K4 或后续裁 |
| 行为/UX（F9） | judgement-call | server/front | dedup 分支缺语音反馈 | device-round-evidence §二 + changed hunk front_app.py | **已闭合**：873a3f17 dedup 注入 kidpal_speak「这条刚记过啦」（话术单源）；复验机制 PASS（rev 不变 + 引擎 TTS 逐字 14:26:39），human 首感「没有去重」= 模型回声句在前的顺序困惑→记 UX 观察不动机机制 |
| 功能（F10） | judgement-call | firmware/application | 积分页问一问走 StartListening/ManualStop，点按开启永不 stop、无回显无回复 | device-round-evidence §二（listen start mode=manual 后引擎零 ASR 实证） | **已闭合**：ba83a58d 积分两处 VOICE_SESSION→ToggleChatState（:423/:467，与问课程同语义入口）；复验 PASS（mode=auto、回显+语音回复+跳页） |
| 功能（F11） | judgement-call | server/front | WS 建连积分初推缺失，重启后进积分页零请求零推送、滞留未达占位 | device-round-evidence §二 + landed 架构积分面「WS 建连」行 | **已闭合**：873a3f17 `_push_points_connect` 建连恰一帧 points_home restore=true + 基线短路 + 重连必推 + 缺席/损坏双护栏（保守不推、防假 0）；复验 PASS |

结构闸 PASS/FAIL 不复述为 finding（约定级约束）；gate 全绿仍完整执行五轴语义 review（下节）。
无未闭合 BLOCKING、无未闭合 hard-violation。

## Five-Axis Review

- **类型/边界**：分页纪律三层钉死（reset=true 清旧装首页、同版本续页累加、跨版本 reset=false
  拒绝且零扰动；next_cursor 沿游标循环 ≤10 跳防不收敛）；selected_id 只读注入三态判据、设备不造
  id；负余额数字如实、画法钳位两不混。修复批新增边界：F10 以 ToggleChatState 统一「对话开关」
  语义（不再分 manual/auto 两路）；F11 建连推送以「恰一帧 restore + 基线短路」限流；asset 管线
  零搅动（其余 77 块逐字节一致）。
- **错误处理**：删除确认子态三不原则（其余手势不响应、idle 10 秒静默取消、resend 上限 3）；隐式
  ack 对账三分支；非法 id 拒绝审计；语境未知值记 WARNING 绝不回退 story。修复批：F10 点按入口
  改对话开关后不再出现「永不 stop」死路；F11 建连推送缺席/损坏双护栏不推假 0；dedup 命中零新账
  且恰一帧语音反馈；apply_agent_skills 幂等 SKIP / drift-guard exit 3 / 写前时间戳备份 / 只改
  目标键四段语义经 10 passed 钉死。
- **契约/单源**：键位决策表逐格对 §2.3 手势矩阵；键帽逐字对 V9；三私有帧逐字段对契约 §四；
  单写入口锚 gw_points.add_points( 恰 1 处。修复批新增单源：积分问答口径单源 = kidpal-points-answer
  skill（摘要优先 + state 只读兜底 + 零写权 + text view 不抢屏），AGENTS 仅在「积分问答回合」开
  例外、其余回合禁令不变（课表 P0-1 同型）；declaration 白名单四项与 261017 冻结件同批重锚
  （非放宽）；资产闭集单源 = gen_art_spiffs 管线自身白名单（ruling12）。
- **并发/资源**：设备侧零本地持久化（RAM 缓存可恢复重建、不写 NVS/SD 账本或离线队列）；
  删除幂等以权威重推为准；F11 建连恢复恰一帧且置观察基线，避免与 story 推送抢峰；语音首轮延迟
  偏大记 NFR-PERF-4 观察（未定阈不修）。
- **安全**：见 Verification Evidence security 行——无扩大写权、零新增依赖；删除只走按键确认 +
  私有上行（儿童误触防线 = 三手势取消 + 10 秒静默取消 + 重发上限）；积分 skill 零写权（不执行
  加分/兑换、不写 state、不调写命令，兑换只报够/不够 + 差多少分）；openclaw.json 整文件不入仓、
  声明零 secret。

## Simplification Debt

`minimal:` 标记收成：首轮 diff 移除 1 处存量 minimal: 标记（points_page.c K0「最小渲染」注释，
随正式渲染三分派退役，upgrade 兑现而非新增债）；首轮与修复批均零新增 minimal: 标记（diff grep
实证）。

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| points_page.c 原 K0 minimal: 最小渲染（首轮 diff 内移除） | K0 spike 单帧直绘升级为正式三分派渲染 | 已兑现（首轮） | 无——已升级退役 |
| kidpal_render_node.cc 积分渲染挂全局 LVGL 定时器刷新路径 | 积分页刷新复用既有 render-node 节拍，未单开专用刷新通道 | 真机刷新观感（NFR-PERF-4 实测） | 真机轮看屏流畅度——复验轮未报翻屏卡顿，观察延续 |
| 无 upgrade trigger 的其余条目：无。 | | | |

## Residual Risk

- **遗留挂账四项（逐字）**：
  1. **F8 末屏翻屏回绕待裁**（默认保留回绕；冻结件未钉回绕或停住）。
  2. **兑换正向归 K4**（本轮只测「无激活目标拒答」反路径；正向需 points.py 目标写入子命令）。
  3. **语音首轮延迟 NFR-PERF-4 观察**（复验首轮延迟偏大，只记录实测、未定阈不修）。
  4. **tech-debt 两笔**：母版 assets.c 平台复现漂移（ic_wifi_14 / p1_chip_mingzao，ruling10 并案；
     本批守零搅动已还原旧 bin，未来整目录重生成会改它）+ verify_art_assets.mjs 路径硬编码。
- **资产闭集对账验收悬空**（ruling13）：资产白名单闭集验收自 260915 归档后无验收承载（48→75→78
  增长），记 loop-retro / 快路径 ARQ 候选；skill/资产部署登记类 guard 建议（fixer 申报⑥）同列
  候选，不阻断本包。
- **manifest 与资产实况的小偏差（advisory）**：changeManifest 仍列 `ic_coin_24_24.bin`，但该片
  在 0426d670 已被 36px 版取代并删除（HEAD 仅存 `ic_coin_36_36.bin`）。change-manifest 闸为
  pre-cutoff 存量放行（不红）；manifest 为规划制品，清理归 planner，不影响实现与验收。
- **全量回归 = 人工触发位**：final 跑圈定清单（server scope 内 209/128、host 287/3 引用 fresh
  证据），server 全量 415 引用 fixer 实跑；K1/K0a 收口同惯例。
- **观察窗**：`kidpal_points_ui_selected_id` 跨任务无锁理论撕裂窗低概率（builder 观察项）；
  F9 的「回声句在前、提醒在后」顺序困惑记 UX 观察。
- **Follow-up recommended**：K3 QQ 正式通道（复用 restore 缝）、K4 目标切换与场景资产刻度联动
  （复用进度线缝，含四预置场景标与 D-09 裁决）、K5 story_cover（K0a 页位不回退）。
- **Human decision needed**：本包无 `decisionCoverage` 中 `landsIn: dropped` 且 `reason` 以
  `risk-accepted` 开头的条目（八条 decision 全部 landsIn 到 exitCriteria 或 K3/K4/K5 承接包），
  residual risk 节无 risk-accepted 台账项可列。

## 修复批与 skill 链（rulings 7-16 摘要 + 提交清单）

- **ruling7（修复批立项）**：A 批 = F10（语音改 ToggleChatState）/ F3（主页 before→after）/ F4
  （详细提示移键帽）/ F7（长文字溢框）/ F2+F5+F6（V9 母版重生成三枚 + 白名单 + 渲染换图形）/
  F9（front dedup 注入 kidpal_speak）/ F11（建连初推）+ F1（引擎补丁认领 kidpal_points_context +
  prompt 占位）；manifest 扩 8 路径；资产守零搅动。
- **ruling9**：sc_wheel_42_42 接成进度线默认目标位场景标（回应「游乐场没有」），四预置切换仍归
  K4。**ruling10**：p1_chip_mingzao 既存漂移记 tech-debt（本批守零搅动还原旧 bin）。**ruling11**：
  F4 键帽压缩措辞待 human 现场定，KKB-A3 重锚随措辞走 acceptance-author。
- **ruling12**：gen_art_spiffs 头注「= acceptance artt_expected[]」悬空自指修正为指向管线自身
  白名单（管线即闭集单源，归档件不动）。**ruling13**：现役 spiffs_root==白名单闭集对账验收不进
  本包，记 governance 候选。
- **ruling14**：retract_p4_three_page.acceptance.c 与 keybar_three_slot 同批判据扩入 manifest；
  F10/F3/F4/F7/F2/F5/F6/F9/F11/F1 全落地（F4=详·双击）。
- **ruling15**：skill 三项接线全准——AGENTS 增积分问答回合路由行 + 第 66/77 行禁令开例外；
  declaration 加 kidpal-points-answer 且 apply 跑 live（test_261017 EXPECTED_SKILLS 3→4 同批
  重锚）；manifest 条目 + README 表行照先例。
- **ruling16**：skill 落位采纳 fixer 申报的 openclaw-skills/workspace-kidpal/ 先例路径（派单路径
  openclaw-agents 无部署条目=死件）；text view 答复形态采纳（AGENTS 每次回复带 display 块 +
  text view 不抢积分页实证）。
- **注**：ruling8 未使用（编号跳格，7→9），无对应事件。
- **修复批提交**：`ba83a58d`（固件半：语音入口/主页 before→after/键帽详·双击/溢框截尾/三资产）
  → `873a3f17`（front+引擎半：建连初推/去重语音/引擎补丁认领积分语境）→ `12b2311f`（真机轮证据
  与修复批制品）→ `0426d670`（金币 36px + 右缘，svgUse 注 symbol）→ `431cdf4b`（skill 全链）→
  `b1fa2f3f`（复验轮与 skill 链制品）→ `b7c4636b`（drift_guard 锚校正，round 3 红根因）→
  `073b5050`（A 案回退 unset 锚）。

## UI design-review 证据（uiuxRequired=true，level 2；bootstrap 期纯文本）

- **viewport coverage summary**：六 V9 积分帧（points-home/-empty/-nodata/-negative/points-ledger/
  points-delete-confirm）在 400×300 1bpp 版面经复验轮看屏确认，四态结构/进度线/账本双栏/删除
  覆盖层按 V9 落；金币/小伴/摩天轮三资产在位。
- **a11y summary**：键帽逐字 host 断言绿（「问一问/下移」类）+ 看屏确认（「详·双击」「删掉/算了」）；
  字形覆盖度观察项（`→`(U+2192) 与「M月D日」puhui 16px cbin）在本轮看屏中未见缺字，观察延续。
- **design-drift summary**：`.coin vs ic-coin` 命名/尺寸偏离已在 K2D-E4 行如实申报（取舍 = 36px
  实测 + 沿用管线块名）；主页目标名文字行 vs 场景资产图标 = 本批以 sc_wheel 默认场景标部分收敛，
  四预置仍归 K4 D-09。
- **external artifact reference**：无（未产截图/视频/HTML，repoVisualArtifactRequired=false；
  看屏证据以 human 现场确认 + 三源日志文本摘要承载）。
- **residual risk**：语音首轮延迟 NFR-PERF-4 观察；F8 回绕待裁。
- **UI finding severity**：无 UI BLOCKING；观察项按 code-review-gate 既有轴判读，未新增 gate。

## 未覆盖面申报（诚实挂起清单）

1. F8 末屏回绕行为待裁（默认保留回绕）。
2. 兑换正向场景（K4 目标写入器落地后补测；本轮只测无激活目标拒答反路径）。
3. 语音首轮延迟 NFR-PERF-4（只记录实测，未定阈不修）。
4. 四预置目标场景资产与刻度联动（K4 D-09 承接）。
5. 资产白名单闭集对账验收现役化（ruling13 governance 候选）。
6. `kidpal_points_ui_selected_id` 跨任务无锁理论撕裂窗低概率观察。
7. 字形覆盖度（`→` U+2192 /「M月D日」puhui 16px cbin）延续观察。

## manifest 扩单记录

- planner ruling3：+5 渲染/键路由路径（story_page.c/.h、kidpal_render_node.cc/.h、lcd_display.cc）。
- planner ruling5：+1 K0PTS-A5 重钉件 test_points_e2e_spike.acceptance.py。
- planner ruling7（修复批）：+8 路径（引擎补丁 connection.py / prompt txt / 资产管线 / 模拟器
  assets.c / 三资产 bin 等）。
- planner ruling14：+1 retract_p4_three_page.acceptance.c。
- ruling15/16（skill 链）：+5 路径（SKILL.md / manifest.yaml / README / declaration /
  test_261017）+ AGENTS.md。
- 终态 changeManifest = 39 路径。change-manifest 闸对本包为预 cutoff 存量放行（`skipped:"pre-cutoff"`，
  gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28）。注：清单仍列 `ic_coin_24_24.bin`
  （已被 36px 版取代），见 Residual Risk 的 advisory 行。

## 集成执行回报

未触发 merge-chain 冲突消解（本包 Pod 分支无冲突 abort + blocked 事件，无消解执行）。

## 设备轮证据

deviceOutcome 触发面：launch-card 申报 userVisible=true（S-F6.1/S-F6.2/S-F6.3），
touchedHostPaths 实集 = application.cc / mcp_server.cc / lcd_display.cc / kidpal_render_node.cc。
`delivery/taskpackages/261104-k2-points-device-ui/device-round-evidence.md` 已建并含三形态要件：
§一 场景 PASS 台账八项（human 在场 + 三源日志互证 + 看屏）、§五 复验轮十项 PASS 与 human 收口原话
「可以了，合并吧」；烧录 hash 校验在案（首轮 xiaozhi.bin @53944b5b；复验轮 0426d670/b7c4636b 双分区
烧录 hash 过）。全程 human 目击，不伪造 PASS。完整原始日志留仓外，包内为最小语义段摘录。

## 制品清单（closeout）

- `verifier-evidence/rounds/r1.json`（首轮留痕：scope 注记格式缺陷首跑）、`r2.json`（首轮承重
  final，invocationId 3fb1729b-…，factOk:true）、**`r3.json`（修复批红闸留痕，invocationId
  bf593558-…，factOk:false，保留不删）**、**`r4.json`（修复批承重 final，invocationId
  420cf1ea-…，factOk:true）**。
- `verify-runs.jsonl`（全程 append，含 scope-file 元数据行）。
- `test-scope-final.txt`（首轮圈定清单）+ `test-scope-final-fix.txt`（修复批圈定清单）。
- `overlap-probe.json`（收口实跑快照；.gitignore 全局忽略不入库，K1 先例同此）。
- `docs-delta.md`（零文档变更申报 + 修复批零文档面复核 + OpenClaw 部署面归属说明）。
- `device-round-evidence.md`（真机轮 + 复验轮证据）。
- `pod-events.jsonl`（全程事件流，含 round 3 红闸 integrator agent-escalation 与 pod-closed）。
- 本 `review-packet.md`（v2）。
- `verdict.json` / `verification.json` 机器产线件：`current/verdict.json` 为 r4 程序投影
  （non-bearing）；包目录内未另产，closeout 判断以 validated final fact r4.json 为准。

## 回滚路径

- **代码**：`git revert 073b5050`（declaration 锚）→ `0426d670`（金币 36px/右缘 + svgUse）→
  `431cdf4b`（skill 全链 + test_261017 重锚）→ `873a3f17`（front 建连初推/去重语音/引擎补丁）→
  `ba83a58d`（固件半修复批）；单包整体回滚 = revert 本包范围至 53944b5b（首轮收口点），或从 main
  丢弃本 Pod 合并 diff。
- **设备**：重烧首轮镜像 `xiaozhi.bin @53944b5b`（ota_0@0x20000 保 NVS）+ `art.bin` 相应版本；
  复验轮镜像是 0426d670/b7c4636b（firmware 内容同 0426d670），回滚烧回前版即可。
- **云端**：ECS deploy-backups 快照 `20261106-090605`（首轮）与 `20261114-150216`（复验轮
  skill+金币部署）；front/points/openclaw 部署面按快照还原；openclaw skills apply 备份
  `openclaw.json.bak-kidpal-skills-20261114T070232`；`rollback_agent_skills.py` 一步回滚。
- **声明面**：073b5050 已是 A 案回退态；如需再退，回到 b7c4636b 会重新引入 round 3 红，不推荐。