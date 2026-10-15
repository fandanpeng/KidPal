# Review Packet · 260921-m3-k2-p2-week-grid-page

> workstream m3-schedule ｜ M3-K2 板端 P2 一周总览网格页重写 + 首页「接下来」注入接线（S-F1.2 AC1–AC4 + S-F1.1 AC1–AC2 消费接线半）｜ mergeOrder 34 ｜ rigor full ｜ riskTier high ｜ subsystem firmware ｜ sourceType delivery-story
> integrator 收拢制品（2026-09-21）；合并门判读归 planner 主线程 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。基线含 K1 契约落地批 `24bb1b8`（契约门 HUMAN_APPROVED）与 K1 实现批 merge `ce56940`。

## Change Size

- **实现（builder 未提交改动 → 本次收口 commit）**：`firmware/main` 下 8 文件改 + 4 文件新建，合计 +701/−640（实现面），另冻结验收 1 文件 + 脚手架 wrapper 3 件：
  - `firmware/main/display/schedule_page.c` +924/−改（858 行现值）——P2 整页重写：旧列表版式（260829-schedule-p2 五函数、`{title,items}` 消费）整体退役 → 两个 FROZEN OFFLINE CONTRACT 函数（`kidpal_schedule_grid_parse` / `kidpal_schedule_grid_render`）。自足式最小 JSON 扫描（无外部库；`day` 值域 0..6、`slot` 0..2、`flags` 0..3 逐项值域拒绝，cap 不足 −1 绝不溢出/静默截断，薄载荷 `data:{}` −1，旧列表形状无 `week` 键 −1）；渲染面 7 列（周一~周日）×3 行（上午/下午/晚上）整屏网格 + 表头/槽位标签/表格线家具 + 今天列整列边框高亮（越域不画假「今天」）+ ※/★ 标记（字形可用走真字形、缺字形走程序化兜底，两者墨迹均锁本格带）+ 格内多课逐门竖排（行 pitch 16px ≥ NFR-READ-1 的 14px 下限）+ `KSG_GRID_BOTTOM=251`（y≥252 键帽条带零墨）。
  - `firmware/main/display/schedule_page.h` +116/−85——网格契约常量区（`KSG_NAME_CAP/KSG_FLAG_TEMP/KSG_FLAG_STAR/KSG_COURSE_CAP=84/KSG_GRID_BOTTOM/KSG_FONT_MIN_PX=14`，逐项注释 == 冻结验收文件头）+ `kidpal_schedule_course_t` 类型 + 两函数声明；旧五函数/`ksp_day_t` 声明撤除。
  - `firmware/main/display/home_upcoming.c/.h`（新文件，607+38 行）——A5 首页「接下来」注入接线层（纯逻辑零芯片依赖）：`kidpal_home_upcoming_apply` 消费 home 路由 `data.upcoming` 经 `kidpal_home_p1_schedule_inject` 冻结 seam 注入（seam 零修改，铁律）；缺/空 data 返回 0 且 seam 零变化（§1.3⑥ additive 向后兼容）；`preview:null` 容忍（明日无项 = 无预告）；`preview.time` 非 HH:MM（含「明早」字面）−1（K1 裁决口径：载荷钉明日首项 HH:MM，自然语言标签不入冻结 seam）；任何畸形/越界/seam 拒收 −1 且 seam 零变化（绝不半写）。
  - `firmware/main/display/kidpal_render_node.cc/.h` +93/+38——`cache_schedule` 网格化（旧 16 项列表缓存作废 → `kidpal_render_node_cache_schedule_grid`，`s_grid_courses[84]`，n>cap 只拷前 cap 条绝不溢出，n≤0 清空=空态）；星期注入缝 `kidpal_render_node_inject_weekday`（inject_rtc/inject_env 家族第四缝，mon0 基，越域忽略）+ `kidpal_render_node_today_col` + `kidpal_render_node_home_on_screen`（A5 重渲染判据单一权威）；`cycle_next` P2 分支改读网格缓存渲染。
  - `firmware/main/mcp_server.cc` +108/−——VRT_ROUTE_HOME 接线（data 非空 → apply → 成功且 P1 在屏 → 重渲染首页日程主体；P1 不在屏 seam 已更新不抢前台；apply −1 显式降级 ESP_LOGW 带 corr_id）；VRT_ROUTE_SCHEDULE 改新形状（`courses[84]` 上堆，「ssl_receive 栈纪律」沿用；解析成功渲染 + 填充周网格缓存；失败友好降级）。
  - `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` +34/−——PCF85063 `ReadRtcTime` 连读扩 6 字节（0x08 Weekdays 带出：二进制 0..6 掩保留位，PCF85063 口径 0=周日 → 周一=0 基映射，>6 非法 −1）；装配点 `kidpal_render_node_inject_weekday(s_rtc_wday)`（inject_rtc 同一装配点）；`s_rtc_wday` 读失败/非法保持 −1。
  - `firmware/main/CMakeLists.txt` +13/−、`firmware/main/test/host_app/main/CMakeLists.txt` +15——`display/home_upcoming.c` 入两侧编译（host linux target 与设备树同源可编）+ 注记更新。
  - `firmware/main/test/schedule_grid.acceptance.c`（新文件，1485 行，11 齿）——冻结验收：A1 版式区钉（布局常量/墨迹结构/空格空行语义）、A2 今天列（注入/切换/越域）、A3 标记（※/★ 渲染与保真）、A4 解析降级（畸形/缺字段/越界 cap/薄载荷）、A5 注入接线（seam 达达/preview 容忍/明早拒收）、A6 回归靶（键帽条带零墨）。文件头为版式区钉与齿强度语义权威。
  - `firmware/main/test/acceptance/red-baseline/ksg_baseline_wrapper.sh` / `ksg_red_wrapper.sh` / `ksg_integ_wrapper.sh`（非冻结脚手架）——WSL 退出码脚本文件直取探针（wsl.exe 内联串伪 0 陷阱防）。
- **回填与过程账**：`delivery/taskpackages/260921-m3-k2-p2-week-grid-page/taskpackage.json`（redBaselineResult / mutationEvidence 14 齿 RED→GREEN 回填）+ `pod-events.jsonl`（human-decision / friction / dispatch-completed）+ `verifier-evidence/rounds/r1–r3.json` + `verify-runs.jsonl`（verify.mjs 程序 fs-write 过程态）+ `delivery/workstreams/m3-schedule/progress.md` K2 完成段。
- **Scope**：`firmware/**` + taskpackage 过程账 + closeout 制品；`server/**` 零命中（producer 面归 K1）；`docs/**` 零命中（消费冻结形状，见 docs-delta.md）；`.claude/**` 零命中；`home_p1.{c,h}` 与 `home_p1.acceptance.c` 零修改（冻结 seam 仅消费，M3K2-E2 铁律守住——`git diff` 实证）。
- **Split recommendation**：不拆——网格页重写、缓存网格化、mcp 路由消费、home 注入接线是同一条「板端消费新形状」链路的四段（形状齿/缓存齿/路由齿/接线齿互相咬合：A5 齿断言 apply→seam 计数、A1 齿断言渲染墨迹、A4 齿断言 parse→render 链路 flags 保真），拆开任一段都无法独立验收。

## Verification Evidence

- **final round 3 九闸全绿（integrator，`6375d877-217c-4b40-ac52-38875ba87de5`，2026-09-21，`ok:true` / `observedGateExit:0`）**：evidenceRef `delivery/taskpackages/260921-m3-k2-p2-week-grid-page/verifier-evidence/rounds/r3.json`（immutable verifier fact，本 packet 只引用不复制）。scoped r2（verifier，`c7a2e8d1-…`，七闸全绿）+ r1（红：taskpackage-required current/ 指针未刷新，纯治理投影时序）在案。
  - taskpackage-required / contract-lint / docs-sync（`docsSyncRequired:false` 与 taskpackage 同口径）/ security：全绿。
  - firmware-build：**integrator final 轮真编译 exit 0**（r3 闸内 9.8s 增量收口；全量编译证据 = r2 scoped 闸 78s：`Successfully created esp32s3 image.` / `xiaozhi.bin binary size 0x30c2a0 bytes. Smallest app partition is 0x370000 bytes. 0x63d60 bytes (11%) free.`——Windows 原生 IDF 5.5 通道，两轮均 verify.mjs 机器留痕，非 builder 转述）。
  - unit：**75 passed**（server/tests 面，本包零 server 改动 = 上游基线保持）。
  - acceptance：oracle-pass + executor-run **27 passed**（offline 全量真跑）。
  - mutation-check / acceptance-coverage：functional = M3K2-A1…A6 全部在册（14 齿 RED→GREEN，counterEvidence 已回填 taskpackage.json mutationEvidence）；structural = M3K2-A7（deferred K3）。
- **机械校验**：`.claude/scripts/verifier-evidence-check.mjs --change-id 260921-m3-k2-p2-week-grid-page --phase final --round 3 --invocation-id 6375d877-…` integrator 实跑 **exit 0、`factOk:true`、`observedGateExit:0`**——final evidence 有效，可进 closeout。
- **integrator 独立复跑 host runner 全量（demo 命令复核）**：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/acceptance/red-baseline/ksg_integ_wrapper.sh`（wrapper 内联 `VERIFY_FW_BUILD_REQUIRED=1` 前缀于 build 与 elf 两步；退出码经脚本文件直取——`/c/tmp/ksg_integ_build_exit.txt`=**0**、`/c/tmp/ksg_integ_run_exit.txt`=**0**，不经 `wsl.exe bash -c '…; echo $?'` 内联串）→ build `ninja: no work to do`（与 builder 同树态，非伪造——对象齐则零工可做）+ **`74 Tests 0 Failures 0 Ignored / OK`**。构成 = 既有 63 齿零偏移 + 本包新 11 齿全绿；run log 内 schedule_grid 断言行 11 条全 PASS（`/c/tmp/ksg_integ_run.log` 与 `/c/tmp/ksg_integ_build.log` 仓外临时区留痕）。与 builder 终态交接（`C:/tmp/ksg_mut2/run_final_green.log` 74/0/0）独立一致。
- **红基线链**：既有齿基线 63 Tests 0 Failures exit 0（acceptance-author @ 36d1519 实跑）+ stub 红基线 7 Tests 5 Failures exit 5（红全在真实缺失行为）→ 实现后 74/0/0。
- **Impact audit（worktree 降级形态）**：detect_changes **N/A (worktree)**（Pod worktree diff 对主索引不可见）——替代核对：① `git diff --stat` 闭集核对（见 Change Size；docs/ / server/ / .claude/ / home_p1.* 零命中，2026-09-21 integrator 实跑）；② 关键共享符号抽查：`kidpal_home_upcoming_apply` 生产调用点闭集 = mcp_server.cc:556 唯一一处（grep 实证；CodebaseMemory 图 caller 边经 `app.Schedule` lambda/队列间接未解析——如实记录为图解析缺口，非面外消费）；`kidpal_schedule_grid_render` 调用点闭集 = render_node.cc:619（cycle_next P2 分支）+ mcp_server.cc:639（schedule 路由）两处；`kidpal_home_p1_schedule_inject` 图边确认 apply→seam 消费单源（trace_path 实跑）；③ 渲染节点既有消费面（key_layer_dispatch/schedule_reminder_b/keybar_three_slot 既有齿）74 齿全量零偏移背书。
- **Docs sync**：`docsSyncRequired=false` 成立（零 docs 面改动 + 消费冻结形状事实锚）——详见同目录 `docs-delta.md`。
- **S2 exploratory**：TaskPackage 未声明 `s2ExploratoryRequired` → 无 S2 official scan（非遗漏）。
- **Loop metrics / Pod Journal（post-cutoff dual-format）**：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl`；`pod-events.jsonl` 在场（advisory 检查通过：human launch 授权 / acceptance-author+builder dispatch-completed / friction over-scan 两条已锚），缺失不阻塞 closeout。
- **机器证据投影**：`current/verdict.json`（verify.mjs 程序 fs-write，已刷新至 final r3 口径）+ 按既有惯例晋升本 closeout 目录 `verdict.json`（round fact 副本）/ `verification.json`（advisory 投影，只消费不重跑）。

## Demo 对账行（roadmap §3 本包条目证据形态 vs 效果段逐项核对）

taskpackage 权威口径（`testHost` 逐字）：**「host runner（WSL linux target）+ device build 闸；真机目测 deferred 归 M3-K3 设备轮」**；roadmap §3 K2 条目 demo 证据形态 = **「host runner（WSL linux target）渲染齿 + device build 闸；真机网格目测 + 翻页手感 deferred 归 K3 设备轮」**——两者同口径。证据实况：host runner 全量 74/0/0（含新 11 渲染齿，integrator 独立复跑一致）+ device build 闸两轮机器留痕 exit 0。逐项对账（roadmap §3 K2 效果段承诺 vs 本包证据）：

| 效果段承诺 | 本包证据 | 对账 |
|---|---|---|
| 整周一眼谱 7 列 × 3 行（学校课逐节、课外课逐门、格内多课竖排、无格内滚动） | A1 版式齿：7×3 布局常量 + 逐门墨迹结构（含空格空行语义）断言；`ksg_cell_row` 格内多课自上而下堆叠（`schedule_grid.acceptance.c` KSG-A1 段）；格内溢出 LV_LABEL_LONG_CLIP 截断降级不滚动 | 一致 |
| 今天那列高亮 | A2 今天列齿：板级 RTC 0x08 星期寄存器 → inject_weekday → `today_col` 整列边框高亮断言（含 host 注入跨天切换 + 越域不画假「今天」）；mutation KSG_TODAY_NOOP / KSG_HIGHLIGHT_BLEED / RN_WDAY_SEAM_UNCALL 三齿背书 | 一致（板端半；跨天零点重读未落本包——见未覆盖面②） |
| 临时安排带 ※ | A3 标记齿：flags bit0 渲染断言 + parse→render flags 保真齿（KSG_FLAG_TEMP_DROP / KSG_FLAG_MASK_WIPE 背书）；host 缺字形走程序化兜底、墨迹锁本格带 | 一致（真字形观感归 K3 目测——见未覆盖面③） |
| 马上要上的课带星标呼应首页 | A3 星标齿：flags bit1 渲染断言（KSG_FLAG_STAR_DROP 背书）；呼应语义单源 = producer group_upcoming 当前项 bit1（K1 齿侧）与板端渲染位（A3 齿）经契约 §1.3② flags 字段对齐；首页侧当前项显示经 A5 注入 seam 承载 | 一致（两半单源对齐；「呼应」的视觉同屏目击归 K3 真机轮） |
| 字再小也不低于 14px 可读线（NFR-READ-1） | `KSG_ROW_PITCH=16 ≥ KSG_FONT_MIN_PX=14`（产线 font_body.bin 14px 同档）；mutation KSG_PITCH_SHRINK（pitch 16→12 即红）背书下限被齿钉死 | 一致（1bpp 真机小字目测归 K3——M3K2-A7 deferred 本即口径内） |
| 首页「接下来」从课表数据出——云端一推，首页日程主体跟着换 | A5 注入接线齿：view=home 携 data → `kidpal_home_upcoming_apply` → 冻结 seam item count 断言（HUP_INJECT_SKIP 背书）+ P1 在屏重渲染（`kidpal_render_node_home_on_screen` 判据）；producer 单源 = K1 `home_upcoming_payload`（对端齿 75 passed 全绿） | 一致（offline 接线半；真机三触发刷新目击归 K3） |
| 课表数据没有或坏了的时候页面友好降级，绝不花屏 | A4 解析降级齿：畸形/缺字段/越界 cap/薄载荷/旧形状一律 −1（KSG_DEGRADE_BLIND / KSG_CAP_OVERFLOW 背书）；渲染面参数校验先行（n<0/courses NULL −1，LVGL 未初始化 −1 不崩）；apply 失败整包拒 seam 零变化 | 一致 |

**一致性结论：demo 证据 == roadmap 效果段承诺。** 效果段七件事在 host 渲染齿 + device build 闸可验半全部落地且逐项对上；真机半（网格目测/翻页手感/三触发目击）按 roadmap 本条口径显式 deferred 归 K3 设备轮，非缺口。

## 未覆盖面申报（对照 acceptance[] M3K2-A1…A7 + 契约 §1.3②/§1.3⑥ + roadmap §3 效果段，逐条列「本次无齿/未跑的设计性质 + 现状 + 去向」）

| # | 未覆盖面 | 设计性质与现状 | 去向 |
|---|---|---|---|
| 1 | **M3K2-A7 真机半**——WF-F1-02 版式真机 1bpp 目测（14px 小字可读）、BOOT 翻页进出 P2 手感、今天列高亮真机观感 | structural 条目在册（acceptance-coverage/mutation-check 闸 structural 列表实证）、本包未跑：host 渲染齿钉布局常量与墨迹结构，钉不到真机 1bpp 呈现与手感；单真机不两轮撞车（K2 host 齿 + device build 闸先行的既定拆分，crossPackageBehaviors 第 3 条） | **M3-K3 设备轮**（M3K3-A3 挂账） |
| 2 | **零点跨天周期性重读 RTC 未落本包**——今天列跨天自动切换的完整机制 | 板级 RTC（PCF85063）仅在装配点读一次（`ReadRtcTime` 调用点闭集 = init 装配单次，grep 实证），`s_rtc_wday` 装配后不随零点刷新——**设备连续供电跨天后，今天列在下次重启前不切换**（渲染面按「每次落屏重取注入值」实现，缺的是板级周期重读半）；`kidpal_render_node.cc:98` 注释「本地零点跨天后板级重读 RTC 即切换」口述的是机制意图，本包只落了注入缝与渲染面重取，重读触发器未接线（Findings 第 1 条同源）；断电重启即恢复正确 | **升级路径 K3 设备轮**（真机轮一并核跨天行为；重读机制可随 K3 或后续小批补——板级装配点单点改动，齿已把注入缝/越域语义钉住，补线不动冻结面） |
| 3 | **host 子集缺 ※/★ 字形走程序化兜底、设备走全量真字形**——两侧行为分支不一致 | host 侧 LVGL 内置 Source Han Sans SC 14 子集无 U+203B/U+2605 字形 → `ksg_glyph_available` 探测后走十字+四角点（※）/实心菱形（★）程序化兜底（10×10，两画法真帧可区分）；设备侧 font_puhui_14_1 走真字形。墨迹落本格带语义两分支等价（A3 齿对两分支均断言格带归属），但**兜底画法与真字形的观感一致性**无齿可钉 | **K3 真机目测**（M3K2-A7 同轮核标记观感；兜底分支 host 常驻，不随包退役） |
| 4 | **格内 >4 门静默不画**——渲染面防御分支（`rows[d][s] >= 4 → continue`） | parse 层 cap=84 总量拒绝（越总量 −1，绝不静默截断），但 ≤84 总量下**单格分布 >4 门时**渲染面画前 4 门、其余静默不画（防御性裁剪非 −1）：spec 口径学校课 4+2 节 + 格内多课正常分布触不到此分支，属 producer 侧异常分布的兜底面；「静默不画」与「整包 −1 降级」之间取了前者（保已解析课程可见性） | 无行动——契约未钉单格上限分布语义；如 K3 真机或后续批认定应整包拒绝，回契约门裁口径后随包扩齿 |
| 5 | **device build 后未上真机**——xiaozhi.bin 只到 build 闸（`0x30c2a0 bytes, 0x63d60 bytes (11%) free`），未 flash | device build 闸 = 本包口径内的设备半验证（M3K2-E3「页面组件面必触设备编译」已满足）；真机 flash/联调本就不在本包口径（live 半 deferred 既定） | **K3 设备轮**（human 持板确认轮统一覆盖） |
| 6 | **preview.time 裁决点的渲染标签半**——K1 留观：契约 §1.3⑥ 示例 `preview.time="明早"` vs seam HH:MM | 板端已按 K1 裁决口径落：载荷钉明日首项 HH:MM（`preview.time` 非 HH:MM 拒收，A5 齿 HUP_MINGZAO_PASS 背书）；「明早」判设备渲染标签 = **K2 落**的部分，本包 preview 渲染走 home_p1 冻结 seam 既有显示路径（seam 内 preview 项显示机制为 P1 既有面，非本包新增）——若 human 契约门本意是载荷直发标签需回裁 | **K3 真机轮目击 preview 显示形态**；撞 seam 即升级回契约门（progress.md K1 落账段留观 1 条在案） |

以上 6 条**零一条是本包义务内缺口**（1/3/5 为口径内 deferred 且 taskpackage/plan 申报在前，2/4/6 为实现面如实申报的边界/机制半），故不写 `none`，如实挂账。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc | **今天列跨天切换的板级重读半缺失且注释口述过强**——RTC 仅装配点单次读取，连续供电跨零点后 `s_rtc_wday` 陈旧、今天列不切换（未覆盖面②）；`kidpal_render_node.cc:98` 注释「本地零点跨天后板级重读 RTC 即切换」把未接线的重读机制写成了已完成语义，读注释者会误判机制已闭环 | changed-code hunk（`s_rtc_wday` 装配单次写 + kidpal_render_node.cc:98 注记句）+ 启发式：RTC 写点闭集 grep 全量核对仅装配一处、板级文件零周期 timer | 不 BLOCKING 的理由：spec「今天按设备本地日期零点切换」的语义锚已被齿钉住（注入缝/渲染重取/越域兜底全在），缺的是板级触发器单点；重启即恢复正确；单真机不两轮撞车的既定拆分下真机跨天观察本就归 K3。Required：K3 前修注释或落重读线（板级 lv_timer 单点改动）——随 K3 或后续小批，不阻合并 |
| NIT | judgement-call | firmware/main/display/schedule_page.c | 格内 >4 门防御分支静默不画（未覆盖面④）——cap 语义两层（总量 −1 vs 单格静默裁剪）并存，语义双轨 | changed-code hunk（`rows[d][s] >= 4 → continue`）+ 启发式：同文件 parse 层「绝不静默截断」纪律（cap 不足 −1）与渲染层静默跳过形成对照 | 可随 K3 批加一行 ESP_LOG（或齿注记）显式化；当前契约未钉单格分布语义，不阻合并 |
| NIT | judgement-call | firmware/main/mcp_server.cc | `kidpal_home_upcoming_apply` 失败降级只出 ESP_LOGW（`upcoming_apply degrade rc=%d`），无计数/上报面——云端无观察点（与 K1 packet NIT「marker 落笔失败无日志痕迹」同型的镜像面） | changed-code hunk（VRT_ROUTE_HOME apply −1 分支）+ 启发式：既有 mcp 降级路径同构（schedule 路由解析失败同为本地日志） | 可随 K3 部署批并入日志口径统一；非义务 |

**结构闸结果**：final r3 九闸全绿（结构闸 PASS/FAIL 不复述为 finding）；结构闸之外，author 五轴语义 review 段完整执行（见下）。

## Five-Axis Review（author 语义审查）

1. **正确性**：周网格解析逐值域断言（day 0..6 / slot 0..2 / flags 0..3 / name 非空 / cap 拒绝）行为级钉住（A4 段）；渲染面墨迹结构钉住（A1 段：格归属带/行 pitch/键帽条零墨/空格空行语义）；今天列三变异背书（不画/越带/装配点断线各有一齿必红）；A5 接线全路径钉住（注入计数/preview:null 容忍/明早拒收/缺 data 零偏移）；mutation 14/14 RED→GREEN + 还原确认（终态 diff 无变异残留 + 恢复后 74/0/0）。已知边界 = Findings 三条 + 未覆盖面 2/4/6。
2. **边界**：解析器全量畸形拒绝（尾部垃圾/空 entry/缺 week/非数字/越值域一律 −1）；`courses[84]` 上堆守「ssl_receive 栈纪律」；渲染面参数校验先行（不动屏不崩）；apply 整包拒（绝不半写 seam）；缓冲逐字节 safe-truncate 恒 NUL；越域注入忽略不画假值——V27 纪律全链沿用。
3. **契约**：契约面零触碰（消费 §1.3②/§1.3⑥ 冻结形状，M3K2-E2 铁律守住，`git diff` docs/ 零命中）；flags 位/容量常量/降级语义与契约门定稿逐项对齐（schedule_page.h 常量区注释 == 冻结验收文件头）；home_p1 冻结 seam 零修改（仅消费，`home_p1.acceptance.c` 不动）；旧形状退役 = 契约门 2026-09-21 废止决定的板端执行（非本包自裁）。
4. **安全**：`securityReviewRequired=false`；payload 源 = K1 producer 机械计算（非 LLM 生成），板端自足式解析无外部 JSON 库攻击面（手写扫描器对嵌套/转义全拒绝或 skip 受控）；无动态内存新增（数组定容 + 上堆 shared_ptr）；I2C 读扩字节掩保留位（bit7..3），非法值 −1 不画假「今天」；无新增依赖（host/device 两侧编译面零新库）。
5. **可运维性**：corr_id 链路（mcp 路由/RTC 读）ESP_LOGI/W 带因，云端下发失败可经串口日志归因；缓存空态 = 友好空态降级画面（R-S0.2 纪律）；字体缺字形走探测+兜底（不崩不空屏）；detect_changes N/A (worktree) 降级核对已如实记录（见 Verification Evidence）；device build 11% 分区余量（空间无虞）。

## Simplification Debt

`grep "minimal:"` 本包实现/测试文件全量扫描 → **1 命中**：`firmware/main/display/schedule_page.c:641`——「版式常量按冻结验收文件头区钉定坐标（400x300 减键帽条 chrome）；真机 1bpp 目测/翻页手感 deferred 归 M3-K3 设备轮（M3K2-A7 挂账）」。台账：**1 条，no-upgrade**（真机目测是证据形态 deferred 非实现简化，悬挂点 = K3 设备轮 M3K3-A3，同 acceptance structural 条目一致，无升级触发）。其余文件零命中。

## Residual Risk

- **decisionCoverage**：TaskPackage `decisionCoverage: []`（空）——无 `landsIn: dropped && reason: risk-accepted` 条目，无 human 冒险接受项需列示。
- 残余风险实质 = 未覆盖面申报表 6 条（真机半 + 板级重读半 + 字形分支 + 防御边界），无新增高风险敞口：真机半全部口径内 deferred 且 host 侧行为级齿垫底；板级重读缺失已被 Findings SHOULD 条显式化（重启即恢复正确、齿已钉住语义锚），不存在静默漂移路径。
- 集成期零冲突（单包批次、树池第 9 棒独占本树）；对端消费面（K1 producer 齿 75 passed）在本包消费实现落地后保持全绿。

## 合并门输入摘要

- 前置依赖：`dependsOn: [260921-m3-k1-cloud-schedule-producer]`——**已 merged**（`ce56940`，human-approval-ref `36840095`），解锁条件满足。
- 回滚方式：单 commit revert 本包实现批即可整体回退（firmware 面自包含：`home_upcoming.{c,h}` 删文件 + 其余文件回 revert；CMakeLists 两处同步回退；无 server/docs 面连带——契约批 `24bb1b8` 独立在前不受影响）。
- supervisor 判读输入：final r3 `ok:true` + evidence-check PASS + host runner 74/0/0 integrator 独立复跑一致 + demo 对账七项全一致 + 6 条未覆盖面全部口径内挂账（1 条 SHOULD 非阻断留 K3）+ findings 零 BLOCKING（1 SHOULD + 2 NIT）+ mutation 14/14 counterEvidence 在册。
