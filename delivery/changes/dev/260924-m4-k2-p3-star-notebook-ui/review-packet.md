# Review Packet · 260924-m4-k2-p3-star-notebook-ui

> integrator 收拢制品（2026-09-30），供 supervisor 机械合并门复核、高风险时供人类 review。本包 = M4-K2 板端 P3 星星记录本展示面重构（story_page 四态重写 + P3 键路由补全 + 300ms 消歧 + 删除确认覆盖层 + 隐式 ack 重发 + 盖章 seq 对账 + CJK 字体接线），**server 零触碰**、`docs/contracts/**` 零触碰、spec 两文件只出 patch delta（`docs-delta.md §4`，待 human spec 批）。闭环口径：`SPV2-A1`~`C1` 13 齿 + exitCriteria `M4K2-E1`~`E3`。

## Change Size

- **实现面（tracked 16 文件，`git diff --stat -- firmware/`）**：**16 files changed, +1635 / −513**——主体 `story_page.c`（重写两翼：输入表 v1.47 决策表 + 渲染结构；1233 行变更，全文件 1839 行）/ `story_page.h`（+197，旧 FROZEN 头注退役声明 + 视图结构 additive）/ `mcp_server.cc`（+344，story 路由双通道拆分 + 删除/重发/idle/消歧四针眼消费）/ `kidpal_render_node.cc/.h`（+103/+47，覆盖层夹钳消费 + 子态 getter）/ `waveshare-s3-rlcd-4.2.cc`（±68，板级 HandleStoryKey 分流 + 前置拦截）/ `keybar.c/.h`（+31/+16，additive 并行键帽缝）/ `kidpal_layer.c/.h`（+15/+30，层值符号级插入 + transient 集与 front_is_overlay 闭集纳入）/ `lcd_display.cc`（±13，story 键帽分支）/ `protocol.h/.cc`（+6/+10）+ `application.h/.cc`（+4/+11，发送委托）。
- **冻结验收面（新增 2 件）**：`firmware/main/test/story_page_v2.acceptance.c`（**13 TEST_CASE / 1954 行**，注册序置于 host_infra_smoke 之前）+ `story_page_v2.test.c`（8 TEST_CASE / 201 行）——两件均未 track、随本收口批首提。
- **Scope**：单一子系统（firmware display/输入/协议面）+ 单页（P3）承载；server/ 零触碰（`detect_changes` changed_files 零 server 文件）；spec/契约零直改。
- **Split recommendation**：无需拆分。渲染面（story_page）、接线面（mcp_server/board/protocol）、层值面（kidpal_layer）为同一条 v1.47 矩阵落缝，切开任一段都会出现「齿引用未落地符号」的假红窗口。

## Verification Evidence

- **Bearing 机器件（final 相位，本包派单内 final 轮实跑）**：`verifier-evidence/rounds/r4.json`（`authority:bearing`、`producedBy:verify.mjs`、`phase:final`、`round:4`、`invocationId:2322f6f7-2608-40c7-88e3-de91b0f1ea73`、`observedGateExit:0`、**ok:true**）——**十闸全绿**（taskpackage-required / change-manifest〔pre-cutoff 存量放行〕/ firmware-build〔真编译 build-pass、`xiaozhi.bin` 0x310b20B / app 分区余 **11%**〕/ contract-lint / acceptance-coverage〔SPV2-A1~C1 全 functional〕/ mutation-check / docs-sync / security + full 档 `unit` / `acceptance`〔scope-file 求交〕）。
- **final evidence checker**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 260924-m4-k2-p3-star-notebook-ui --phase final --round 4 --invocation-id 2322f6f7-2608-40c7-88e3-de91b0f1ea73` → **`{"ok":true,"factOk":true,"observedGateExit":0}`（exit 0）**。
- **test-scope 圈定清单落盘路径**：`delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/test-scope-final-r4.txt`（impactedTestsSeed ∪ detect_changes blast radius 映射测试合成，14 个现役测试文件 + 排除项留痕；archive 面 `happy_flow_capture` 依 sweep 移位即退役口径不入 scope；server 面 = seed not-impacted、scope 交集空诚实绿）。
- **host 全量回归（integrator 终验实跑，真实退出码方法学：WSL 侧末命令 = 被测 ELF、外层捕获 `$?`）**：
  - `MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf` → **`128 Tests 0 Failures 0 Ignored` / `EXIT_CODE=0`**。
  - 构成 = 上游基线 107T0F @ 5575a23（launch prep 重锚）+ 本包新齿 21（acceptance 13 + unit 8）全绿；**既有齿零偏移**（0 Failures，seed 面九件回归靶 + blast radius 增量靶全在同一 ELF 全量内跑过）。
- **Mutation 取证 13/13**：canonical = taskpackage `mutationEvidence`（A1–A9 行为齿逐条红名 verbatim + B2/B3/C1 结构扫描齿 + **B1 走摘源清单敏感路径**：`story_page.c` 摘出 host runner 源清单 → `frozen seam NOT implemented` 哨兵红 @128/17，还原位 128/0；**旁注如实**：抽字体接线 mutation 在 host 不可咬——`LV_USE_FONT_PLACEHOLDER` 豆腐块自带墨，墨点探针分不清真字形/占位块，字体接线子义务由 SPV2-B2 结构计数齿（`font_puhui_14_1` >=5 处）承载）。取证全表 = `delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/mutation-evidence-story-page-v2.md`（**该 .md 为补做轮前的代存投影、尾部两条「未实做」注记已陈旧——canonical 以 taskpackage counterEvidence 为准**，见 Findings #1）。
- **轮次史（如实）**：r1 红（acceptance-coverage）→ r2 红（mutation-check）→ **r3 绿（scoped，2026-09-29）** → **r4 绿（final，2026-09-30，本批）**；两轮红均为冻结件修订前口径（fixture 缺陷三处经 acceptance-author 修订轮闭合，pod-events `89fcc195` / `d3417607` 在案）。

## Acceptance 逐锚结果（SPV2-A1~C1）

| 锚 | 内容 | 结果 |
|---|---|---|
| **A1** | LIST 态 v1.47 决策表——六手势动作码闭集 KSPV2_ACT_* 重钉 + 空列表特例 | **绿**（mutation 1/1 转红：旧 CURSOR_NEXT 残留） |
| **A2** | LIST 分屏纯逻辑——屏底翻屏落该屏首项 / wrap 永不离页 / 分屏指示 | **绿**（mutation 1/1） |
| **A3** | 300ms 消歧——仅 LIST 态门控、其余态零延迟直发；300<350 恒断言 | **绿**（mutation 1/1：全局生效即红） |
| **A4** | 删除确认子态——KEY 确认 / BOOT 取消 / GLOBAL 漏网负锚 / 10s idle-step 静默取消 | **绿**（mutation 1/1：GLOBAL(-2) 漏网即红；builder 补做轮） |
| **A5** | 隐式 ack 重发决策表——5s 间隔上限 3 次 / 放弃停发 / story_list 到达对账冲销 | **绿**（mutation 1/1：tries<=MAX 无界即红） |
| **A6** | 阅读态决策表——末屏 wrap 永不离页 / KEY 短按不响应（朗读退役负锚）/ 长按回列表 / >12 屏截断回收 | **绿**（mutation 1/1：钳制旧语义即红） |
| **A7** | 计数双口径 guard——恒信 total（23/16 钉 23）/ 删除 −1 / kidpal_story_counter 在世零改 | **绿**（mutation 1/1：items.length 顶替即红——契约 §1.3③ load-bearing） |
| **A8** | 盖章 seq 消费 + 文案逐字二值——带 N / 缺省降级不带 N / N 只增不减 | **绿**（mutation 1/1：降级分支删除即红） |
| **A9** | 层值接线——STORY_DELETE=6 符号级插入 + REMINDER→7 / PROVISION→8 上移 + 序不变 + #ifndef fallback | **绿**（mutation 1/1：直接追加 8 即红） |
| **B1** | CJK 非空白真帧探针——列表标题带 + 盖章文案带 | **绿**（mutation 走摘源清单敏感路径 1/1 转红 @128/17；tofu 遮蔽旁注如实申报，见 Findings #2） |
| **B2** | 渲染面结构齿——font_puhui_14_1 >=5 处 / 双指令 / 页内无标题 / 反白退役 / 空态单源 / FROZEN 头注退役声明 | **绿**（mutation 1/1：单指令化即红） |
| **B3** | 删除链路接线结构齿——SendKidpalStoryDelete 原语 + mcp_server 四针眼消费 + stamp_index 退役负锚 | **绿**（mutation 1/1：四针眼失效即红） |
| **C1** | 注册序守卫——story_page_v2 接线置于 host_infra_smoke 之前 + canary + 扫描器自检 | **绿**（mutation 1/1：摘注册行即红） |

## exitCriteria 铁律逐项复核

- **M4K2-E1（host 全量齿零红 + device build 闸）**：新冻结齿 build+run exit 0（**128/0，integrator 终验实跑逐字**，既有齿零偏移）；device build 真编译 build-pass、`xiaozhi.bin` 0x310b20 / app 分区 0x370000 余 **0x5f4e0（11%）**。**遵**。
- **M4K2-E2（铁律）**：① `docs/contracts/**` 零改（`git status --porcelain docs/contracts/` 零输出、contract-lint 绿）；② spec 两文件零直改（delta 待批）；③ `story_page.h` FROZEN OFFLINE CONTRACT 头注**退役声明**落文件头（新冻结面 = 本包新齿，B2 齿承载）；④ `key_layer_dispatch.acceptance.c` **既有断言零触碰**——实证 = 该文件不在本包 diff 中（层值断言 SPV2-A9 落本包新齿，未 append 旧齿文件）；⑤ M1 基建 seam 触碰面 = **plan「技术方案」节裁读授权面逐一对应**（层栈符号级插入 + pop_transients/`front_is_overlay` 纳入 = 1g 裁读①；keybar additive 并行缝 = 决策点 4、260928 查字窗先例；kidpal_voice_states / 覆盖层夹钳 / PAGE_COUNT / 提醒·查字·课表·P1 四页渲染路径零触碰——`char_window_overlay` / `reminder_overlay_unify` / `p1_*` / `schedule_grid` 等靶 128/0 零红背书）。**遵**。
- **M4K2-E3（真机目测半 deferred 挂账 + docs-sync 面在案）**：真机 1bpp 目测（列表/弹窗/阅读/盖章四面）+ 300ms 消歧手感 **deferred 挂 M4-K3 设备轮**（单真机不两轮撞车，human 在场一次性目击）；docs-sync 面 = `docs-delta.md §4` 七 hunk patch 成文 + gate evidence C2 翻 done / C3 K2 半 done 注记。**遵**。

## demo 对账（workstream 包必含 · pod-coordination §11）

- **本包 demo 证据形态（roadmap ③ 本包条目逐字）** = 「host runner（WSL linux target）渲染/输入/覆盖层齿 + device build 闸；真机 1bpp 目测 + 手势手感 deferred 归 K3 设备轮」。
- **本包效果段**（roadmap ③「完成后人能看到什么」）= P3 整页焕新且中文真正可见（CJK 接线修复）：列表（星星图标+标题+日期 / 选中带边框 / ×N 计数 / 分屏指示 / 页名消失 / 双指令提示）、BOOT 手势全家（300ms 消歧仅列表态 / 永不离页）、删除确认整屏弹窗（KEY 删掉 / BOOT 算了 / 10 秒静默取消 / 计数 −1）、KEY 双击进阅读（排版达标 / wrap / 长按退回 / 短按装死）、盖章「星星徽章·第 N 颗已入册！」（N 来自云端 seq / 0.8 秒定格）。
- **demo 证据（本包实跑）**：host 全量 **128/0（exit 0）**——13 acceptance 齿 + 8 unit 齿对上表逐锚、九件回归靶零偏移；device build 绿 + 11% 体积余量；mutation 13/13 转红还原；final bearing fact r4 十闸全绿 + checker PASS。
- **对账结论 = 一致**：效果段六目的（①列表 ②删除确认 ③按 KEY/触发词开录的板端输入缝半 ④盖章 seq ⑤阅读态 ⑥改名对账四项 + CJK）与齿覆盖一一对应；形态差异 = 效果段「会话动作」（引导态 / 累积回显 / 收尾三式执行）按缝切分归 K3（plan §K2/K3 缝切分 + crossPackageBehaviors 承载条），非本包未做声明面。真机半 = roadmap 逐字 deferred 归 K3，无「声明 live 而未跑」落差。

## 未覆盖面申报（覆盖断言覆盖轴的设计面差集 · pod-coordination §11）

1. **300ms 消歧真机手感** — 性质：真机半（host 只可钉纯逻辑决策表与 300<350 恒断言）；现状：SPV2-A3 全绿；去向：**M4-K3 设备轮**（human 在场目击）。
2. **键帽子态文案真机目测**（[收尾]/[结束]、[长按退出]/[翻屏]、[删掉]/[算了] 四 WF 面 1bpp 目测） — 性质：真机半；现状：keybar additive 值域单源备好 + lcd_display 接线（minimal 注记在案）；去向：**M4-K3 设备轮**。
3. **wire 真发面**（`SendKidpalStoryDelete` 真机 WS 发送 → front 幂等承接 → 重推对账闭环） — 性质：跨端 live 半；现状：host 钉发送原语 + 重发决策表 + 对账冲销消费（SPV2-A5/B3），front 承接面由 K1 冻结齿担保；去向：**M4-K3 设备轮**（live 复证）。
4. **「进页即见列表」时点保障** — 性质：跨包重裁项（建连即推被 260928-story-connect-push-defer 摘除，现役两时点）；现状：K2 消费面按契约冻结形状、零建连时点预钉（决策点 6 已守，SPV2-C1 全文件零该时点断言）；去向：**M4-K3 重裁**（恢复时机 = 引导态会话建立同拍候选）。
5. **录音态会话动作**（session_ctx 发送、引导态、stt 回显消费、收尾/丢弃会话侧执行） — 性质：缝切分归属（K2 只落渲染 + 输入表语义位）；现状：输入表 v1.47 语义位全落（A1）；去向：**M4-K3**（`M4K3` 面，mergeOrder 40 串行双写）。
6. **盖章内圈真实虚线** — 性质：观感细节（LVGL9 无 obj 级 dash setter，`story_page.c:1295` minimal 注记）；去向：**设计殿后**（roadmap §4 挂账不拆）。
7. **SPV2-B1 抽字体接线 mutation 不可咬**（tofu 遮蔽） — 性质：测试强度边界；现状：B1 走摘源清单敏感路径取证 + B2 结构计数齿承载字体接线子义务；去向：**如实申报**（如需行为级咬合需测试专用关闭 placeholder 的编译面 = 新基建，评估归 loop-retro，见 Findings #2）。
8. **星星徽章美术新形态** — 性质：资产判定；现状：决策点 5 判定零新增（47 资产集全覆盖 + art 分区已烧），builder 对 v5 PNG 未发现需新形态（扩单通道未触发）；去向：**none**（已闭合）。
9. **跨子系统 e2e / correlationId 链路验证** — 性质：跨端面；现状：本包纯固件、server 零触碰；去向：**none**——本包无本批需证的跨端对偶面（跨端 live 链归 M4-K3 设备轮三源关联取证）。

> **真机目测半 deferred 挂账声明（M4K2-E3）**：列表/弹窗/阅读/盖章四面 1bpp 目测 + 300ms 手感 = 挂 **M4-K3 设备轮一次性收口**（crossPackageBehaviors 第 5 条 live 承接条）——本包 demo 口径 = host 全量绿 + device build 闸 + 本声明，不产真机证据。

## UI design-review 证据（uiux L2 · 纯文本，bootstrap 期不入仓截图/HTML）

- **viewport coverage summary**：P3 五面（列表 canvas-p3 / 录音 p3-recording / 盖章 p3-stamp-frame / 阅读 p3-reader / 删除确认 p3-delete-confirm）全部实现并以冻结齿断言承载；layout 常量按 host seam 400×300 定坐标（`story_page.c:976` minimal 注记：真实 RLCD 屏观感殿后 K3 目测）；列表/阅读/盖章 = P3 本地页内容（页点可见）、录音态 = 会话覆盖路径（SERVER_PAGE、无页点自动成立）、删除确认 = 新层值覆盖层（`front_is_overlay` 闭集、页点消失）——与线框口径逐面吻合。
- **a11y summary**：中文可见性主项 = CJK 字体接线缺陷修复（`font_puhui_14_1` 全 label 显式接线 >=5 处结构齿 SPV2-B2 + 非空白真帧探针 SPV2-B1——列表标题带与盖章文案带真墨）；16px 以下 1bpp 真机可读性 = NFR-READ-1 全局挂账项、非本包新增缺口。
- **design-drift summary**（brief↔实现）：页内无标题 / 星星计数 ×N 右上角裸排（v1.57 去框）/ 选中带边框不反白 / 分屏指示「第 x 屏 · 共 y 屏」/ 双指令提示 / 盖章 -8° 微倾定格 0.8s / 文案逐字「星星徽章·第 %d 颗已入册！」——均由冻结齿逐字/结构断言承载，**对账四项销账后无已知 drift**；盖章内圈虚线观感归设计殿后（第 6 条）。
- **external artifact reference**：`docs/ui/V5/canvas-p3.png`、`p3-recording.png`、`p3-stamp-frame.png`、`p3-reader.png`、`p3-delete-confirm.png`（frozen 逐页 PNG，launch LP7 verified 在盘）。
- **residual risk**：真机 1bpp 实渲染与 300ms 手感未目击（K3）；host seam 坐标与真机渲染的偏差面未目击（同挂 K3）。

## Detect Changes / 影响面核对（Step 9b 记账 · 范围判定归 Step 4）

- **detect_changes**（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，`since=base/m4-star-notebook`）= changed_files **32**、seed_symbols **163**、**impacted_total 34**（全在 `firmware/main`，单模块 rollup）；impacted 测试模块 = 本包新齿（story_page_v2 acceptance+test）+ 既有靶 char_window_overlay / reminder_overlay_unify / key_layer_dispatch / host_infra_smoke / schedule_grid / p1_cjk_name / p1_home_battery / char_window——全部落入 test-scope 清单并在 host 全量 128/0 内零红。
- **关键共享符号核对**：`lcd_display`（键帽 sync / voice chrome / 三 Display 子类）与 `kidpal_render_node`（tick / dismiss_front_overlay / cycle_next）、`waveshare` 板级（keya_overlay_consume / keya_front_route_top / keya_dispatch_global_action）为本次改动符号的 inbound 消费面——全为符号引用面（无数值断言依赖），既有齿零红背书；`app_main` / `Application::Initialize` 为下游初始化链（接线点在 diff 内、语义 additive）。
- **跨层/循环依赖卫生**：无新跨层调用方向反转（story_page 纯逻辑 ← mcp_server 消费 ← protocol 发送，单向）；无新增依赖、无绕过 contracts 的本地类型（story_list/story_text/delete 均按契约 §1.3③④/§四冻结形状）；重复实现面 = keybar additive 并行函数（既有值域零改、260928 先例框架）——非第二真相源。
- **范围圈定合法性声明**：三分法核过——per-Pod 图索引可用（detect_changes 正常圈定）→ **正常圈定路径**，非降级；圈定清单落盘 `test-scope-final-r4.txt`，未默认全量、未缩水到本包档（scope 含 9 件 seed 回归靶 + 3 件 blast radius 增量靶，floor 面完整）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/display/story_page.c:976 | 布局常量按 host seam 400×300 定坐标 | 真实 RLCD 屏渲染观感 | M4-K3 真机目测轮（已挂账）——观感偏差即触发 |
| firmware/main/display/story_page.c:1221 | 阅读超长文退化为底部数字指示 | 逐屏页点/滚动条类真分页指示 | 产品提出真分页指示需求（现 12 屏截断回收已钉齿） |
| firmware/main/display/story_page.c:1295 | 盖章内圈虚线省略（LVGL9 无 obj 级 dash setter） | 真实虚线描边 | 设计侧给出虚线资产/绘制方案（roadmap §4 挂账不拆） |
| firmware/main/display/story_page.c:1339 | story 影子随视图整体重装刷新、不跨视图持久 | 跨视图增量缓存 | 列表数据量/刷新频次成为实测瓶颈（现两时点推送下无损） |
| firmware/main/display/lcd_display.cc:1366 | 录音态键帽 [收尾]/[结束] 值域复用 keybar 单源备好值 | 独立子态键帽渲染路径 | 键帽值域需脱离 keybar 单源（违反单源纪律，不预期触发） |

- `grep -rn "minimal:"` 改动面 11 文件 → **5 处**（上表全量），无未登记标记。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 簿记面 / 投影一致性 | `mutation-evidence-story-page-v2.md`（planner 代存投影）为补做轮前快照——末节仍写「已实做 11/13、SPV2-A4/SPV2-B1 未实做 + 冻结件缺陷三处待修订」，与 canonical（taskpackage `mutationEvidence` A4/B1 counterEvidence「builder 补做轮实做」+ 修订后 128/0）漂移。 | changed-artifact 锚：`delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/mutation-evidence-story-page-v2.md` 末节 vs 同目录 `taskpackage.json` mutationEvidence[3].counterEvidence / [9].counterEvidence（本批实读）；heuristic = 代存投影未随 canonical 回填同步（无实害——机械闸 `mutation-check` 只读 taskpackage，r2 红到 r3 绿的闭合即走 canonical） | planner 择机同步 .md 尾节或在文件头注「投影时点 + canonical 指针」；不阻断合并 |
| NIT | judgement-call | Test strength | SPV2-B1「抽字体接线」mutation 在 host 不可咬——`LV_USE_FONT_PLACEHOLDER` 豆腐块自带墨（baseline 1940 vs mutant 768 墨点均 > 阈值 30），墨点探针分不清真字形/占位块；取证改走摘源清单敏感路径（哨兵红 @128/17）。 | `taskpackage.json` mutationEvidence SPV2-B1 counterEvidence 旁注（builder 诊断实证）+ pod-events `290e5f3e` / `c2adeb58`（mutation-b1-evidence 派发轮）；heuristic = 行为级咬合需测试专用关闭 placeholder 的编译面 = 新基建，超出本包范围 | 如实申报（本表 + 未覆盖面第 7 条）；字体接线子义务由 B2 结构计数齿（>=5 处）承载已闭环；行为级咬合评估归 loop-retro 素材 |
| NIT | judgement-call | Governance / write-scope | spec 面（requirements/architecture）对 integrator 结构性不可直达（scope-map allow 不含两文件、却含已废弃 `docs/systems/**`）——本包 docs-sync 义务降级为 patch delta 待批。 | `.claude/scope-map.json` `agents.integrator.allow`（本批实读）+ `AGENTS.md`「docs 平铺」句；heuristic = 声明义务 vs 授权写路径不匹配（K1 review-packet Findings #1 同族延续） | 已按 K1 先例处置（delta 成文 `docs-delta.md §4`，合并后 human spec 批）；scope-map 修订方向归 loop-retro/human（不新登记 ARQ、不动 `docs/crq/**`） |

> 结构闸 PASS/FAIL 不复述为本表 finding；本批**无 hard-violation finding**（十闸全绿、零契约/spec 直改、零既有齿偏移）；gate 全绿仍完整执行上五轴语义 review。

## Five-Axis Review

1. **Correctness**：13 齿全绿且 mutation 13/13 逐条转红（弱齿为零——最弱位 = B1 的敏感路径取证，已申报）；边界条件钉死：空列表特例 / wrap 永不离页（列表+阅读双侧）/ GLOBAL 漏网负锚 / tries=3 不重发 / total=23 窗口 16 计数=23 / seq 缺省降级不带 N / 10s idle 静默取消 / 300<350 恒断言 / >12 屏截断回收。层值插入 + 上移方案经 1g 裁读与全仓符号引用核实（is_stackable 上界自动跟随、STACK_MAX 余量）。
2. **Test quality**：决策表齿断言真实动作码/页态/文案逐字（非「调用了就算过」）；时间敏感逻辑时间戳显式注入（同 rec_tick 纪律）；注册序守卫 + 需求契约 canary + 扫描器自检（C1）防静默摘除；既有 107 齿零偏移背书共享面零回归。
3. **Contract/boundary safety**：消费契约 §1.3③④/§四冻结形状、零自定字段（items.length 不顶替 total = load-bearing 齿）；`docs/contracts/**` 零触碰；id 闭集纪律（只发 story_list 下发过的 id，不造 id）；降级语义沿用 V27（畸形/缺字段友好降级、seq 缺省降级演出）。
4. **Operability/security**：删除 = 用户明确手势 + 确认子态 + BOOT 兜底 + 10s 静默取消（破坏性动作全兜底）；重发上限 3 次防消息风暴；本地即时移除 + 云端对账冲销（最终一致、单真相不造第二状态机）；无 secret、无注入面（id 闭集 + 畸形 JSON 扫描器 surrogate-pair 防御既有）；日志面 = 结构齿 + mcp_server 消费留痕。
5. **Maintainability/simplicity**：`ksy_*` 已验证扫描器底座逐字保留、重写两翼（非整文件推倒）；纯逻辑层（story_page）/设备接线层（board/lcd）/契约发送层（protocol/application/mcp_server）三段切分清晰；键帽 additive 并行缝沿 260928 先例不破三槽单源；`minimal:` 5 处全部受控登记（上表）。

## Residual Risk

- **Known limitation**：① spec 对账 delta 待 human spec 批落地（`docs-delta.md §4` 七 hunk）——未落地前两 spec 文件仍写对账前形态（⚠️ 修复项未销账、AC3 旧引文）；② 真机目测半 + 手感 + wire 真发面 + 「进页即见列表」时点挂 M4-K3（未覆盖面 1–5 条）；③ B1 mutation 行为级不可咬（tofu 遮蔽，Findings #2）；④ 代存 mutation .md 投影陈旧（Findings #1）。
- **历史（已闭合）**：冻结件缺陷三处（SPV2-A4 fixture 双按不可满足 / SPV2-A6 tooth 4 off-by-one / SPV2-B1 seam 返回值契约矛盾）经 acceptance-author 修订轮闭合（pod-events `89fcc195` + `d3417607`；r2 mutation-check 红 → r3 绿闭合实证）——非未决风险。
- **Follow-up recommended**：① M4-K3（会话动作接线 + 设备轮 live 目击 + 推送时点①重裁 + prompt v7 文面收口 C5）；② loop-retro（scope-map integrator allow 修订 + 行为级字体 mutation 基建评估 + 代存投影同步纪律）。
- **Human decision needed**：① **spec 批落地**（本件 docs-delta §4 七 hunk + ARQ-260918 剩余处如仍未落地可同批——合并后）；② milestones M4 行 state 沿 K1 遗留同批裁量；③ remote push（恒 human）。
- **decisionCoverage dropped/risk-accepted**：本 TaskPackage `decisionCoverage` = 空数组 `[]`，**无** `landsIn === 'dropped' && reason.startsWith('risk-accepted')` 条目——**none**。

## 回滚方式

- **本仓改动回滚**：本 Pod 分支未 push、未 merge；弃用 = 丢弃本收口 commit + 回退实现批（工作树态 `git checkout -- firmware/ && git clean -f firmware/main/test/story_page_v2.*` 可还原到 5575a23 基线）；closeout 制品与 pod 簿记为新增文件、无既有面破坏。
- **gate evidence 回滚**：`architecture-gate-evidence.md` C2/C3 处置行 revert 即回 pending 态（单文件、K1 包目录内、与实现回滚独立）。
- **运行时面回滚**：无 ECS/部署面触碰、无数据迁移、无配置改动——固件包回滚 = 纯代码面；层值 STORY_DELETE=6 插入随代码回滚自动消失（符号级、无持久化状态）。
- **合并门**：本批 pending supervisor 机械闸复核；supervisor APPROVE + 前置升级闸放行后由 planner 主线程 `merge-driver --pod` 本地合并（integrator 不 push、不 merge）。

## pod-events 交叉引用（escalation themes / friction 留痕锚）

- **升级/阻塞类**：SPV2-B1 mutation 不可观测发现与补取证轮 = `290e5f3e`（builder 诊断：豆腐块遮蔽实证）+ `c2adeb58`（mutation-b1-evidence 派发，摘源清单敏感路径取证）；冻结件缺陷三处修订轮 = `89fcc195` + `d3417607`（acceptance-revision）。
- **friction / over-scan 申报**：architect 预评超读 10 行 = `11e6717c`；builder 迭代1 超读 9 行 = `770a0cfb`；acceptance-author 修订轮超读 5 行 = `d3417607`；planner 探 pod-events-append.mjs 误产假事件即删（含教训注记）= `0f2a1c33`。
- **派发遥测**：architect `a28d9aa740e45c832`（15bc4225）/ acceptance `a0964ebf5ea8a6791`（30d2e164）/ build `a38480423f2bd4792`（38029c98）/ acceptance-revision `a1fb5eb0afbcb9500`（89fcc195）/ mutation-backfill `a923aa4265b10db04`（0a175af2）/ coverage-marker-fix `a62be8e6e6817c01e`（653f49b2）/ mutation-b1-evidence `a62f2bf147038994e`（c2adeb58）。
- 本包 pod-events 共 12 事件（`delivery/taskpackages/260924-m4-k2-p3-star-notebook-ui/pod-events.jsonl`）；无未闭合 escalation（升级闸红档零）。
