# Review Packet — 260929-char-window-v5-trim

- 日期：2026-09-29
- 分支：dev/260915-m1-k1a-key-layer-dispatch（Pod worktree，未 push）
- 来源 spine：delivery/changes/intake/260929-char-window-v5-trim.md（需求面 v1.60 定稿落稿；architect 分流 review 四分类 = 无需改；q6 真机烧录授权 = human 预授权离场、主线程代烧）
- TaskPackage：delivery/taskpackages/260929-char-window-v5-trim/taskpackage.json（rigor=lite · riskTier=medium · gateProfile=[test-quality]）
- plan：delivery/taskpackages/260929-char-window-v5-trim/plan.md（architect /triage-change reviewed；human 触点①改判框方案 = 重烘资产）
- Integrator：dev-loop integrator 本包收口（不跑 supervisor、不跑 merge——归 planner）

## 1. 变更摘要

查字窗 v5 视觉收敛四件（依据 = human 2026-09-29 真机轮裁定，运行时 words 恒空、口径
收敛 S-F3.2 四行，非缺陷修复）：①删右栏底部装饰星（ic_star_22 消费链整删）；②删组词
行（words 解析/join/拼装整删、info_o.words 传空串；card.words 成员保留、char_stroke.c
零触）；③引导线灰度改纯黑点线 + 画布自绘灰边框撤除；④cardlabel 让位 (228,160) →
(244,144)。外框 frm_236_236.bin 重烘正圆角矩形（gen_assets.mjs HAND_R 对称 14px）+
simulator 中间母版 + report snapshots×2。验收件修订（acceptance-author 正规 authorship）：
test 10 MCD-A1 几何齿改让位区间（A1-x x>=236 + A1b x+W<=388）、test 11 MCD-A4 整组
回撤（数据面 guard must-stay-green 保留）、新增 test 14 CWVT-A1/A2/A3/A4/A6 负向删改
核证齿、seam [14] + REVISION LOG append-only 追记。改动面与 changeManifest 十文件 +
planner 簿记对账一致。

## 2. 范围圈定（终验复核）

- **test-scope 圈定清单**（= acceptanceTestsFile ∪ impactedTests 种子；detect_changes
  图索引本 Pod worktree 条目未跑——本包零新增跨符号调用面、改动主体为删改 + 资产
  重烘，圈定以 changeManifest + 零改动核证面逐项复核承载，降级口径如实留痕）：
  1. `firmware/main/test/char_window_overlay.acceptance.c` — 修订（义务红→绿）
  2. `firmware/main/test/char_stroke.acceptance.c` — 零改确认（git diff 零行；words
     成员保留，:1022/:1033/:1061 verbatim 齿零触）
  3. `firmware/main/test/char_window.test.c` — 零改确认（git diff 零行）
- **零改动核证面逐项复核**（`git diff HEAD` 逐项，全零 diff）：char_stroke.c、
  char_stroke.h、char_window.h、char_window.c、kidpal_render_node.cc
  （firmware/main/display/）、mcp_server.cc（firmware/main/）、application.cc、
  firmware/main/boards/**、server/**、docs/**（含 docs/contracts/**）——全绿。
- **实际 diff 面 = changeManifest 逐项对账**：char_page.cc / char_window_overlay.acceptance.c /
  frm_236_236.bin / gen_assets.mjs / simulator assets.c + png + 9slice×8 / report
  snapshots×2，另加 planner 簿记（taskpackage/briefs/spine/plan.md）——无清单外源码面。
- **重烘零漂移核证**：`git diff HEAD --name-only -- firmware/main/assets/spiffs_root/`
  仅 frm_236_236.bin 一枚 diff，其余 46 枚 bin 零字节漂移。
- **全量回归 = 人工触发位，不跑**（launch 指令；本包 host 面覆盖由 scoped 轮承载）。

## 3. 验证证据汇总

- **红基线**（acceptance-author 2026-09-29 冻结相位实跑）：host runner 全量
  **107 Tests / 2 Failures**，唯二红 = 本包设计态义务红：① char_window_overlay :2128
  （CWVT-A1 星齿，ic_star_22 消费仍在世）② :1736（MCD-A1 A1-x 让位齿，x=228 < 236）；
  既有冻结齿同跑零新增红。已知遮蔽如实记：Unity abort-on-fail 下 CWVT-A2/A3/A4 未在
  红基线轮实跑到位（现状源面必然命中），随 builder 落地在 verifier 轮兑现。
- **绿态**：planner scoped verifier r1（verify.mjs，invocationId
  11f25f66-a013-40c2-be13-a0b442395968，fact = delivery/taskpackages/260929-char-window-v5-trim/
  verifier-evidence/rounds/r1.json，bearing）：host **107/0** 全绿 + firmware-build 过
  （真编译，xiaozhi.bin 0x30f1f0 / app 分区 11% free + art.bin spiffsgen 过）+
  contract-lint / acceptance-coverage（CWVT-A1..A6 + MCD-A4 全覆盖）/ mutation-check
  （skip 理由在案）/ docs-sync（false）/ security 八闸全绿，gatesNotExecuted=[]。
- verify-runs.jsonl / pod-events.jsonl 随包在世（post-cutoff 口径，advisory 检查通过，
  缺失不阻塞——本包均在世）。
- **final 相位说明**：按 planner 指令本包终验 = scoped r1 承载（全量回归 = 人工触发位
  不跑）；integrator 未重跑 `verify.mjs --phase=final`。final bearing round（如
  supervisor 门要求）归 planner 主线程决定补跑；test-scope 圈定清单见 §2（scope-file
  未单独落盘文件，以本节行内容承载）。

## 4. 关键取舍（decisionCoverage 对账）

| DC | 决策 | 落地 |
|---|---|---|
| DC-1 | 组词行删除性回撤、card.words 成员保留（删成员触 char_stroke.c FROZEN 冻结面 + verbatim 齿，零行为收益换大爆炸半径） | acceptance:CWVT-A2 ✓（test 14 负向齿） |
| DC-2 | 外框走重烘 frm_236_236 资产案（human 触点①裁，不采程序化画框推荐案）；消费链 charpage_art_frame_dsc 保留 | acceptance:CWVT-A6 ✓（test 14 guard） |
| DC-3 | 引导线保留改纯黑点线（灰度 RLCD 阈值化不可见为根因）；点距/虚实真机轮调 | acceptance:CWVT-A4 ✓ |
| DC-4 | cardlabel 让位数值终判归 builder + 真机轮，原则 = 文本左缘不与框右缘重叠 | acceptance:CWVT-A5 ✓（builder 取 244/144，区间齿承载） |
| DC-5 | 设计母版 v5 HTML 查字窗段仍画组词行/星——不同批更新（spine q3 human 裁 deferred） | dropped——review-packet residual 呈 human |
| DC-6 | ic_star_22.bin 孤儿不删文件（烧录面最小变更），账面注记 | acceptance:CWVT-A1 ✓ + §6 资产账 |

builder 侧三件自主裁（plan 授权位）：canvas 自绘 1px 0xbb 边框撤除（重烘框已含 3px
纯黑边，保留会双框）；cardlabel 取 244/144（x+ W = 388 恰出画布右缘内、清出框右缘
235）；HAND_R 对称 14px 对齐 v5 .rbox（HAND_R_SM / frm_sm / frm_flat 不波及，diff 核证
仅 HAND_R 一行 + 注释）。

## 5. Findings（五轴语义 review）

结构闸（scoped r1 八闸）PASS/FAIL 不复述为 finding；以下为 author 语义 review 产出。
**无 BLOCKING、无 hard-violation**。每条 Kind = judgement-call（Basis = changed-code
hunk + heuristic，不伪造 rule citation）：

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | firmware/display | CharPageState 四死成员（char_label/pinyin_label/stroke_total_label/words_label）零引用 + show() 内 `ch` set-but-unused 编译警告（既有，非本包引入；words_label 死成员为组词行删除的直接后继） | char_page.cc struct CharPageState hunk + grep 零引用；本包删组词行使 words_label 转死的连带面 | 后续包死成员清理（plan.md 残余登记已在案）；`ch` 警告随清理同刀 |
| SHOULD（后续批） | judgement-call | tools/asset-pipeline | verify_art_assets.mjs / make_snapshots.mjs 硬编码主 checkout 绝对路径、非 worktree 感知——本 Pod worktree 不能直跑（本轮经临时副本跑通，仓内脚本零改） | gen/verify 脚本路径常量 hunk；worktree 路径实况 | 后续批 `__dirname` 相对化改造（builder 移交 residual①） |
| NIT | judgement-call | tools/asset-pipeline | report/budget.md 陈旧（A9.2 既有红：total 599,698 ≠ data_size 求和 729,078）——与本包无关，本包 data_size 逐值不变 | report/budget.md 既有内容；frm_236_236.bin 7096B 前后一致 | 保持既有 A9.2 账，不并入本包 |
| NIT | judgement-call | tools/asset-pipeline | report/snapshots 整体陈旧：frames_stamp_dots.png 内嵌 frm_236_236 已陈旧（不在 changeManifest）、icons_grid.png 历史陈旧——本包只刷新了 changeManifest 内两枚 stretch 快照 | report/snapshots 目录 diff 范围 = changeManifest 两枚；其余未刷 | 后续批快照全量重刷或陈旧账注记（builder 移交 residual③） |
| INFO | judgement-call | firmware/assets | pose_sleep_96.png 源注释称 v1.14 已裁但资产仍在世（informational，不阻塞） | gen_assets.mjs 注释 hunk | 无——与 ic_star_22 孤儿同类的资产账注记候选 |

安全轴：纯呈现面删改 + 本地资产重烘，无输入面、无内存面新增（words join 的 UTF-8
半字回退逻辑随删除面消失；card_build 96B 截断冻结语义经 test 11 数据面 guard
must-stay-green 验证）。契约轴：server/** 零改、wire 形状零触碰（D3/E1），spiffs 资产
为本地 bin。边界轴：让位几何区间齿（x>=236 / x+W<=388）防回归。可运维轴：烧录纪律
app+art 双分区（K4 教训）入包并见 §7。

## 6. 资产账注记（归 ui-v5-align findings §3 账面与收口 summary，主线程落账）

- **ic_star_22.bin 孤儿在世零删**：装饰星消费链删除后资产转孤儿（82B 在世）。本包
  不删文件（DC-6：烧录面最小变更、删文件扩大烧录 diff 面且零行为收益）——主线程
  随 ui-v5-align findings §3 资产账登记。
- **frm_236_236.bin 重烘后形状对齐 v5**（斜框 → 正圆角矩形 rbox 14px）：ui-v5-align
  findings §2 查字窗行的资产侧账面随之更新（主线程）。

## 7. 未覆盖面申报（真机轮 / 真机腿，不阻塞 supervisor）

1. **真机轮目检**（spine q6：human 2026-09-29 预授权离场，headless 转绿后主线程代烧，
   目检五项留 human 起床后真机确认，不构成 closeout 真机销账）：①无星；②右栏四行
   （拼音带声调/共 N 画/部首/释义，无组词行）；③重烘圆角框无斜边、1bpp 阈值化无锯齿
   /断线（重点 = 圆角与直边相接处）；④米字黑点线可见；⑤右栏不被压（cardlabel 244/144
   呈现）。烧录纪律 = **app+art 双分区**（spiffs 资产变更，K4 教训）；烧录异常按
   kidpal-flash-device 症状表处置并留痕。
2. **frm_236_236 重烘像素形状**：host 面测不出 bin 像素（CWVT-A6 仅 guard 消费链在世），
   阈值化目检归真机轮（申报位同上③）。
3. **引导线点距/虚实参数**：现值点距 3px（1 on 2 off），母版 dasharray 3 on 9 off——
   像素级调参归真机轮，齿不钉（CWVT-A4 只钉纯黑在世）。

## 8. Simplification debt 台账

grep `minimal:` 零命中——本包无标记收债项。死成员清理（§5 finding 1）与资产孤儿
（§6）以 residual 形式登记，均非本包引入的新债（words_label 死成员为口径收敛的
直接后继，随后续包清理）。

## 9. Residual Risk（decisionCoverage 对账）

- decisionCoverage 中 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目：**无**。
- DC-5（landsIn=dropped，reason = spine q3 deferred 非 risk-accepted 前缀）以 residual
  形式呈 human：设计母版 v5 HTML 查字窗段仍画组词行/星，与 v1.60 定稿口径漂移——
  母版更新与否呈 human 裁（见 docs-delta.md §3.2）。
- 其余 residual = §7 未覆盖面（真机轮目检/代烧/调参）+ §5 findings 表的后续批项 +
  §6 资产账注记，均 non-blocking。
- S2 exploratory：TaskPackage 声明 s2ExploratoryRequired=false——未执行、无 S2 证据项。
- mutation：mutationRequired=false（skip 理由在案，先例 260928 同口径）。

## 10. 交付与回滚

- commit：本包 feature 分支冻结 diff（含 planner 簿记 + closeout 两制品）单 commit，
  hash 见回执（commit 后由 integrator 回填 pod-events / 本节由 planner 复核时比对）。
  回滚方式 = 分支级 revert 该 commit（纯删改 + 资产重烘，无 schema/契约迁移面，revert
  无残留状态）。
- 不 push、不 merge main；supervisor 裁断 + 前置升级闸 + merge-driver 归 planner 主线程。
- 机器证据投影：final bearing verdict.json / verification.json 晋升归 final round 落地时
  （scoped r1 fact 已随包目录 commit，`verifier-evidence/rounds/r1.json` 为承重机器件）。
- 状态：**等 supervisor 裁断**。
