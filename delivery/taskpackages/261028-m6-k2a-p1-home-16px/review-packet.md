# Review Packet · 261028-m6-k2a-p1-home-16px

- integrator 收口（2026-10-29）；base ref = `08cc46c4`（包首提交父，包级 diff 恒用此 ref + 工作树未提交态）；分支 `dev/pod-m2`（feature 分支，未 push，不在 main commit）。
- test-scope 清单落盘路径：`delivery/taskpackages/261028-m6-k2a-p1-home-16px/test-scope.md`（范围 = 影响面圈定清单：本包冻结件 P16-A1..A3 + 相邻锚 p1_cjk_name / p1_home_battery + schedule_grid 回归 + host_infra_smoke 守卫 + host 接线面；detect_changes 正常圈定、非降级；安全余量 = host 全量 253 tests + 设备 build 真编译，已按该范围执行）。

## Change Size

- 代码面（`git diff 08cc46c4` 含工作树）：实现五文件 = `home_p1.c`（[U1] 临时标记剥离/下划线 + [G1] text_yoff 4→3）、`kidpal_p1_font.h`（[S7] 声明，P0 冻结面外本包 manifest 内新建）、`kidpal_render_node.cc`（[S7] 设备强定义 + [S1] 切 body16 + NULL 回落 font_puhui_14_1）、`host_infra_seam.c`（host 同款）、`host_app/main/CMakeLists.txt`（P1 接线：HOST_ACCEPTANCE_SRCS + FAS 结构七键）；另 P0-chain 修复轮 + 验收冻结面（8239aee7 检查点，已入库）= `host_app/sdkconfig.defaults`（CONFIG_LV_FONT_FMT_TXT_LARGE）+ `font_assets_runtime.acceptance.c`（FAS-A2 additive）+ `p1_cjk_name.acceptance.c` / `p1_home_battery.acceptance.c`（收紧型 supersede）+ `p1_home_16px.acceptance.c`（新冻结，1110 行）。
- 净量级：全 diff +2024/−249 行（20 文件，含治理制品）。
- Scope：固件 display 首页 P1 帧渲染 + 设备/host 字体消费 seam；零 server diff、零 producer diff（wire 不变，F-9 裁定）。
- Split recommendation：不拆——P0/P1 串行共享 seam（human 已裁 P0+P1 一起合并，真机统一轮），P1 交付不可再分。

## Verification Evidence

- **设计态红 → 绿链**：builder step-0 接线后红跑 = **253 Tests / 7 Failures**（存量 AB-A1/A2/A4 三条 + P0-chain 在册红 PCN-A11 设计态 + P16-A1/A2/A3 设计态齿；首红 tooth 全为预期设计态，非 infra 误红；MUST-STAY-GREEN 键位零红）；绿跑 = **253 Tests / 3 Failures**（仅存量 p1_auxbar AB-A1/A2/A4，零新增红，P16 全绿 + PCN-A11 转绿）。
- **变异证据**：mutationEvidence 5 条全 turned red（P16-A1 face=NULL 回落；P16-A2 摘下划线 / 画回 ※ / 2px→3px；P16-A3 yoff 3→4），FINAL 干净跑 253/3；M1 连带 P1CJK-A11 外溢为可达断言外溢，目标验收确红即满足。
- **设备 build**：firmware-build 闸真编译绿——xiaozhi.bin 0x324c20 bytes，app 分区 0x370000，余 0x4b3e0（9%）。
- **机械闸**：verify.mjs 8 闸全绿（taskpackage-required / change-manifest〔pre-cutoff 存量放行〕/ firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security），error:null——`delivery/taskpackages/current/verdict.json` + `verify-runs.jsonl`（2026-10-29T13:11:08Z）。contract-lint 仅存量 warning（历史批断链，非本包）。
- **范围圈定终验**：detect_changes（项目 C-AI-KidPal-pod-m2，base 08cc46c4，inbound depth 2）= changed_files 21 / seed 130 / impacted_total 7 符号，全部收敛 firmware/main，无 server 面、无清单外符号外溢。
- Security gate：securityReviewRequired=false，闸 PASS。

### Scope 圈定结论（清单内外对照）

- **清单内已触**：home_p1.c、kidpal_render_node.cc、kidpal_p1_font.h（新）、p1_home_16px.acceptance.c（新，acceptance-author 冻结）、p1_cjk_name.acceptance.c / p1_home_battery.acceptance.c（supersede，builder 不得直改条款已守）。清单内登记未触（manifest 为上限）：home_p1.h、kidpal_render_node.h——零 diff（负向键 p1_home_p1_h_no_temp_field 过，home_p1.h 不扩 flags 临时标记字段）。
- **清单外实现面文件（如实列记）**：`firmware/main/test/host_app/main/CMakeLists.txt`、`firmware/main/test/host_app/main/host_infra_seam.c`、`firmware/main/test/host_app/sdkconfig.defaults`、`firmware/main/test/font_assets_runtime.acceptance.c`。定性：P0-chain 修复轮（sdkconfig LARGE + FAS-A2 additive）与 P1 接线增量（CMakeLists / host seam [S7]），由 acceptance-author `acceptance-wiring-patch.md` 授权（P0 先例延续）+ 8239aee7 检查点（architect 裁定在案）入库；host seam 与 CMakeLists 是 P16-A1/A3 冻结验收的 DUT 面，不属清单缺漏。**非静默外溢**：四文件全部可追溯到授权制品。
- 清单外其余改动均为治理制品（delivery/taskpackages/**、docs/debt/tech/ARQ-261029 + INDEX、workstream progress.md），不属实现面清单语义。

## 架构 / 依赖卫生

- **跨子系统**：零 server diff（`git diff 08cc46c4 -- server/` 空）——home_upcoming_payload producer 零触，wire 形状不变；render_node 仅透传消费（kidpal_p1_font_face() → kidpal_render_node_body_font()），零排版逻辑，跨层调用不成立。
- **重复实现**：p1_blit_flow 与 p1_blit_flow_marked 行流语义逐字同构（后者仅增名墨盒记录 + 2px 线），是刻意选择（固定路径行为零改，diff 可隔离评审）；见 Simplification debt NIT-1。
- **契约绕行**：无——[U1] 消费 name 首字符 0x203B 是 wire 上唯一零扩字段通道，与契约 §1.3⑥ {time,name} 一致；home_p1.h 未扩临时标记字段（FAS 负向键钉死）。
- **新增依赖**：无；kidpal_p1_font.h 前向声明 `struct _lv_font_t` 避免把 LVGL 头拉进纯声明头（home_p1.c 等零 LVGL 消费方不受影响）。
- **循环依赖**：无——kidpal_p1_font.h（声明）← kidpal_render_node.cc / host_infra_seam.c（各自强定义）← home_p1.c（消费），单向。

## UI design review 证据（P1H-E4 host 半，纯文本）

- 布局权威 = `docs/ui/V8/canvas-p1.png`；证据载体 = P16-A1/A2 host 渲染断言（p1_home_16px.acceptance.c，host_infra_test.elf 真跑）：① 16px 字面句柄真资产（非 14px、16 档 [20..25]），[S1](cp)==[S6](kidpal_font_body(), cp) 逐字节；② 行带 155-176 / 177-198 / 199-220 三行带各墨 ≥130——16px 行容量零损失、line_h=22 不变式保持；③ text_yoff=3 = (22−16)/2 行带垂直居中（[G1] 重基线，P16-A3 结构键 + 变异 yoff 3→4 转红双钉）；④ 临时事件名 ※ 退役：temp 帧 vs fixed 帧 diff 全落 item0 行带、离散 2 行、各行 diff ∈ [12..60]——2px 下划线随名宽（名墨盒跨度含端点）；⑤ 卡底之下负断言带零墨（containment，不越卡、不压辅助条）。
- viewport coverage / a11y summary：设备 400×300 墨水屏单帧渲染，本包改动面（正文 16px + 下划线）全部落入 host 帧墨断言覆盖；辅助条 14px 维持（F-1 边界，MUST-STAY-GREEN 键位零红佐证）。
- design-drift summary：V8 canvas-p1 与实现无已知漂移（host 断言全部按 V8 基线通过）；缺 assets 降级路径（[S7] NULL→14px 回落）host 断言在世。
- external artifact reference：无仓外 artifact（repoVisualArtifactRequired=false，bootstrap 期不入 screenshot/HTML 重资产）。
- residual risk：**真机半（观感终判 + 缺 assets 降级复证）显式后置 P3 终端收口轮（P1H-E4 / §5），不以 host 字形替代真机终判**——本包 UI review 为 host 半证据，不冒充已终判。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| BLOCKING（真机轮前置，非本包缺陷） | judgement-call | 跨包 / producer | F-9 上游 ※ 写入缺口：现网 producer（front/OpenClaw）不往 name 写 ※ 前缀，本包 [U1] 消费半真机永不触发；human 已裁上游补写入侧路由 | changed-code hunk：`firmware/main/display/home_p1.c` p1_strip_temp_marker（[U1] 依赖 name 首字符 0x203B 约定）+ heuristic「cross-package dependency on absent producer behavior」；在账 `docs/debt/tech/ARQ-261029-home-temp-marker-producer.md`（ARQ-261029） | 真机轮（P0+P1 统一轮）启动前上游写入侧落位；owner = planner 派单 / human 跟踪 |
| NIT | judgement-call | firmware display | p1_draw_item_line 临时路径内联了 time+name 拼接循环，与 p1_join_time_name 部分重复（固定路径仍走原函数，行为零改） | changed-code hunk：home_p1.c p1_draw_item_line + heuristic「duplication for diff isolation」 | 不要求本包改；后续触碰 home_p1.c 时顺手评估合流 |
| NIT | judgement-call | governance | contract-lint 存量 warning（260923 历史批 CONTRACT-CHANGE-PACKET.md 断链） | 闸输出 warning 行 + heuristic「pre-existing, unrelated to changed hunks」 | 无需行动；存量债不追 |

结构闸（verify 8 闸）PASS/FAIL 不复述为 finding；gate 全绿仍完整执行 author 五轴语义 review（下节）。

## author 五轴语义 review

1. **行为语义**：[U1] 语义链完整——检测（E2 80 BB 字面字节）→ 剥离（跳 3 字节，※ 不画不占笔进）→ 行流（与固定路径同形）→ 下划线（名墨底缘下相邻行起 2px、宽 = 名墨跨度含端点、clip 进卡区与帧缓冲、色 = 行墨色）。固定日程零变化（不进 marked 分支）；三态空态语义零触。
2. **契约一致**：wire 零扩字段（home_p1.h 零 diff、负向键钉死）；name/time 语义零改；[S1] 签名与字形语义零改（共享核心 p1_glyph_render_core / p1_glyph_core 不变，仅 face 来源切 [S7]）。
3. **边界与降级**：下划线画线逐像素 clip（cy0/cy1/cx0/cx1 + P1_FB_W/H 双界）；[S7] NULL 回落两侧各保 canary 引用（设备 font_puhui_14_1 / host lv_font_source_han_sans_sc_14_cjk），缺 assets 降级在世；mount(missing reader) 复位 14px 兜底防跨文件状态泄漏（P16-A1 钉）。
4. **可运维性**：降级路径可观测（自诊断红文字 P0-chain 修复轮已校正）；设备 build 体积余 9% 不变恶化。
5. **测试质量**：判别力负控（puhui14 vs body16 可判别）、P0 链完整性 gate（cbin glyph 度量合理性——P0 假绿缺陷的对症钉）、变异 5 条覆盖三验收各主断言轴、负向结构键（home_p1.h 不扩 flags）防契约面回涨。

## Simplification debt

- `minimal:` 标记：全 diff **零命中**，台账空，无升级触发条目（全部 no-trigger）。
- NIT-1（p1_draw_item_line 拼接重复）记入上表，不构成台账升级。

## Residual Risk

- decisionCoverage 筛查：`landsIn === 'dropped' && reason.startsWith('risk-accepted')` 条目 = **0 条**（六条决策全落 exitCriteria 或前置包），无 risk-accepted 残余需列。
- 实质性残余：①真机半观感与降级终判后置 P3（结构性安排，非风险敞口遗漏）；②ARQ-261029 上游缺口（真机轮前置，见 Findings）；③存量 p1_auxbar 三红（AB-A1/A2/A4，跨包存量，非本包引入）。

## 升级项史（摘要）

- ① P0 链缺陷（blocker，已修复）：78 fork cbin writer LARGE=1 误读 → host sdkconfig.defaults 开 CONFIG_LV_FONT_FMT_TXT_LARGE（8239aee7，干净配置复验 250/4）+ FAS-A2 additive 防假绿；architect 裁定 host 单侧错配、设备 LARGE=1 在案。
- ② ARQ-261029（F-9 上游 ※ 写入缺口）：human 裁上游补写入侧，debt 在账，真机轮前置（见 Findings）。
- ③ FAS-A8 反向评估：P0 冻结件零修订（needle 在世、回落分支设计保绿）。

## Supersede 记录指针

- p1_home_battery.acceptance.c / p1_cjk_name.acceptance.c 收紧型 supersede：REVISION LOG 在两文件内 + `acceptance-wiring-patch.md`；line_h=22 与 preview 三态未重锚（P16 冻结条款）。supersede 落盘于 8239aee7 检查点（acceptance-author 冻结轮）。

## 可回滚路径 / Merge Packet

- 回滚锚 = `git reset --hard 08cc46c4`（P1 全部在途改动丢弃，回到 launch 面准备态；P0 内容在 789c0244/8239aee7 不受影响——包级 diff 基准即 08cc46c4）。
- Merge packet：P0+P1 合并按 human 裁定一起走（真机看效果后）；supervisor 闸 + 前置升级闸归 planner 主线程，本包不做 merge/push。
- 集成执行回报：本包无 Pod 分支冲突消解轮（Merge-Chain Execution 未触发），「双方冻结件 floor」以本包冻结件 + 相邻锚替代（见 test-scope.md floor 说明）。

## 遗留与后续

- pod-retro.md 归 planner 收尾相位（本包不写）；post-cutoff Pod 不产 session-digest / loop-health / loop-metrics / pod-journal——本仓 gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF，change-manifest 闸按 pre-cutoff 存量放行，本包制品面沿用 P0 先例（pod-events + verify-runs + verdict 投影）。
- pod-events.jsonl advisory 检查：在件（2 事件，settings-presence + pod-started）；closeout 后由 integrator 追加 pod-closed。
- P3 终端收口轮前置条件：P1 必须 merged（crossPackageBehaviors M6 四页真机收口轮）。

**等待 supervisor 裁断**——本包不自行判定通过，不 merge、不 push。
