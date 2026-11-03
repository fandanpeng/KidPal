# pod-retro · 261028-m6-k3-p3-story-typography

> 主线程（planner）第一人称热记忆 · 2026-11-02 · dev-loop Pod（VS Code 扩展会话，池树 pod-m2 直启，同列车接棒 P2 merged）。

**【R1】verify 全链绿 + integrator 收口后即写；supervisor 闸前落盘。**

**【R2】计量诚实**：子 agent tokens 有计量——acceptance-author ~303k（两轮 296k+307k 累计口径）；builder ~250k（两轮 239k+250k 累计口径）；integrator ~84k。主线程估 ~95k（±40%）。时长：launch-prep + 验收冻结 + 实现 + 修订复测 + 收口约 5.5h 有效墙钟。模型面：acceptance-author/integrator 走 glm-5-3-flash-260828、builder 走 deepseek-v4-1-flash-261017（dispatch-stamp transcript 派生）。

## 1. 时间账（估）

| 相位 | 内容 | 时长（估） |
|---|---|---|
| launch-prep | 分支并入 main + fw 锚重测 258/3 + 启动卡 | ~40min |
| 验收冻结 | STY-A1..A7 新冻结 + 两件发现面 supersede + 红基线 258/5 | ~1.5h |
| 实现 | builder step-0 接线 + 实现 + 七变异取证 + device build | ~1.5h |
| 验收修订轮 | STY-A2 夹具单行化 + STY-A6 带避印章圈 + FAS 键名漂移收敛（revision 6） | ~30min |
| MASKED 复测轮 | M-STY2/M-STY6 转正 + 终态 265/3 | ~20min |
| 收口 | verify r1→r2 + integrator final r3 + closeout + 制品 + commit | ~50min |

## 2. Token 账

acceptance-author 两轮 ~303k 与 builder 两轮 ~250k 打平延续 P2 形状；本包新增成本 = 验收修订轮 + MASKED 复测小轮（两小轮合计 ~40min / ~65k），是 P2 F1/F3 教训的直接兑现成本——比 P2 的对抗轮便宜（P2 修订轮 6 处缺陷，本包 2 处 + 键名收敛）。

## 3. 摩擦与卡点

- **F1 · 冻结件夹具几何缺陷二连（P2 F1 同型，规模减半）**：STY-A2 多行带跨 2 自然行恒越 [13..16]（与 STY-A4 WRAP 折行互斥 = 对任何实现不可满足）+ STY-A6 STAMP 带 y0=150 与印章圈底缘相切（基线态即红）。builder 全程未碰冻结件——诊断实证（基线对照 + 上移探针转绿）→ 回派 acceptance-author revision 6 收紧型修订 → 复测闭环。**P2 F1 教训兑现**：acceptance-author 本轮已实测校准预判窗（墨高/行位实测先行），但仍漏了「夹具内容物与既有渲染家具的空间关系」——实测校准惯例需扩一条：**带窗定义时把带内既有家具（印章圈、竖线、带底线）一并量进排除面**。
- **F2 · FAS 键名漂移（接线补丁 vs FAS-A8 revision 3）**：`story_h_reader_lines3` vs `story_reader_lines3`——acceptance-author 冻结轮自造键与自写接线补丁不同名，builder 不得不用别名桥接。根因 = 键名无单一 owner：冻结件头注 [M] 节与 wiring patch 是两处独立命名点。**教训**：FAS 新键命名应一次成文（wiring patch 从冻结件头注复制键名，禁止手抄两份）；本包以 REVISION LOG 申报两键并存为准收口（避免第三轮往返，代价 = CMake 永久多一行别名）。
- **F3 · mutation-check 红一轮（取证做了、投影没写）**：verify r1 红 = mutation-check 报 7 条 missingSensitive——builder 变异取证齐全，但 taskpackage.mutationEvidence.perAcceptance 投影没人写（builder 不写 taskpackage、acceptance-author 不掌变异、planner finalize 时点早于变异取证完成）。**教训**：投影义务应钉在 builder 交付报告模板（perAcceptance 条目草案随报告交）或 planner 收 builder 后第一步投影，否则 verify 必红一轮。
- **F4 · 义务红/绿名单判读按名单不按总数**（P2 F4 延续兑现）：本包红名单三度变化（258/3 → 258/5 → 265/5 → 265/3），每次逐条对表零误判；builder step-0 后 11→5 的收敛判读也按名单走。
- **near-miss**：①builder device build 发现 brief 误写「WSL 下跑 device build」——skill 铁律 device build = Windows venv + c:/tmp/idf_run.py，builder 按 skill 正路执行并如实申报（brief 写错未酿成行为错）；②acceptance-author r1 想写 acceptance-wiring-patch.md 被 enforce-scope 拦（scope-map 收窄后 acceptance-author 写边界不含该路径）——主线程代落盘，内容零改。

## 4. 优化建议

1. **带窗排除面扩展进冻结惯例**：治 F1 · 预判窗实测清单加「带内既有渲染家具空间关系」（印章圈/竖线/带底线坐标量进排除面），P2 F1 + 本包 F1 同型二连佐证。
2. **FAS 键名单一 owner**：治 F2 · wiring patch 键名段从冻结件 [M] 节逐字复制 + 主线程落盘 patch 时机械比对键名集，分叉即打回。
3. **perAcceptance 投影时点钉死**：治 F3 · builder 交付报告模板加 perAcceptance 草案节，或 planner 收 builder 报告后第一步先投影再跑 verify（本包实际这么救的，把它变成明义务）。
4. **device build 通道写进 launch 卡**：near-miss ① · 启动卡固件严格模式行加一句「device build = Windows venv + idf_run.py（skill 铁律），host acceptance = WSL」防 brief 手误。

## 5. 一句总结

成本 ≈ 0.65M 子 agent tokens + ~5.5h，human 触点 0 次 · 最险一刻 = STY-A2 与 STY-A4 的互斥不可满足若被 builder 就地歪修（摘 WRAP 或缩窗凑数），16px 折行语义将整体失效 · **保险丝判定：接住**（builder 诊断实证 + 回派修订 + MASKED 复测三段接力，P2 打法复用成本减半）· 最值得带走的一条 = **「冻结件实测校准要连带内既有家具一起量——带窗缺陷与断言窗缺陷是两类病，后者治了前者还在」**。

## 6. 本环遗留

- P3T-E4 终端真机轮 + human 终判（四页并集 + 翻页手感 + design review 终判）——M6 收口标准，归主线程/human。
- 远程发布待 human（release-pr --publish 后 human 合 PR）。
- ARQ-261029 上游 ※ 写入侧待派单（终端轮下划线观测受限预期在案）。
- 存量 p1_auxbar 三红（AB-A1/A2/A4）跨包在册，非本包义务。
- §4 优化建议 4 条待 loop-retro 收敛（含 P2 遗留的 commit 前变异态核查机械闸候选——本包 F3 投影时点是其近亲）。
