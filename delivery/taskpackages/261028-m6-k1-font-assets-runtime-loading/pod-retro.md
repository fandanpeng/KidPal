# pod-retro · 261028-m6-k1-font-assets-runtime-loading

> 主线程（planner）第一人称热记忆 · 2026-10-28 · dev-loop Pod（VS Code 扩展会话，settings-presence=extension-session，changeId 直启）。

**【R1】verifier 第 4 轮 8 闸全绿后即写；合并前落盘。**

**【R2】计量诚实**：子 agent tokens 有计量（dispatch 通知回传）：acceptance-author 三轮 60k + 123k + 178k；builder 三段（含两次配额中断重续）197k + 360k + 380k；integrator 87k。主线程 tokens 无计量，估 ~150k（±40%）。时长：本会话自 launch-prep 起约 8 小时墙钟，其中配额等待两段约 2 小时（不计入有效工时）。

## 1. 时间账（有效工时，墙钟估）

| 相位 | 内容 | 时长（估） |
|---|---|---|
| launch 面 | 五件预检 + 基线锚实测 + 启动卡 | ~50min |
| 验收冻结 | acceptance-author 三轮（影子树中断 / fresh 重派 / 预基线修正） | ~2.5h |
| 实现 | builder（v1 接线 + step-0 阻塞 + 双裁 + v2 + 实现到绿，含设备 build） | ~3h |
| verifier 内循环 | 4 轮（1 机械指针刷新 + 2 变异证据格式 + 1 全绿） | ~20min |
| 收口 | integrator + supervisor 闸 + retro | ~1h |

对照：单包 bug 修复先例（mirror-desync）约半天；本包 full + spike-first + 两次设计级纠偏，时长合理偏重。

## 2. Token 账

花费最大处 = builder（~937k，三段含中断重续的重入上下文）。可压缩分析：若无配额中断，builder 单段约可省 20%（重入后重新读树）。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · acceptance-author 自创影子树机制被停**：现象 = 验收 agent 为让运行时测试读源码树搭镜像拷贝，伪红 + 段错误 → 根因 = 「树扫断言」先例形态（p1_cjk_name 等 cwd 相对读真实工作树）在本包被推到需镜像整树的极端 → 证据锚 = 被 human 停时的残留 `tools/fas_tmp_shadow_setup.sh`（已删）+ 继承稿 981 行树扫段。教训：机制选型偏离既有先例且复杂度自涨时，agent 应先回报再动手。
- **F2 · 冻结验收两处未验证前提**：现象 = step-0 红基线 TEST 2 段错误，builder 追出矛盾 A（标准 lv_font_conv 通用 bin ≠ cbin 格式）与矛盾 B（cbin_font_create 32-bit 内存镜像直读在 64-bit aarch64 host 必崩，dsc@24 vs @40）→ 根因 = PROPOSAL F-5/F-7 的「gen_fonts.sh 同产线扩产即可被 cbin 装载」「host 跑 cbin 装载」两个前提在设计期未实测 → 证据锚 = builder 报告 hex 头对照 + `node lv_font_conv.js --format cbin` CLI 报错 + addr2line 崩溃栈。**step-0 门 + builder fail-closed 停手接住了它**——若直接放实现，会做完全套才发现不可验。human 双裁（78 fork / host 宽度感知解码器）后预基线修正一轮收敛。
- **F3 · 验收接线撞机械 scope 门**：现象 = acceptance-author 写 host CMakeLists 被 enforce-scope 拦（其边界只有 *.acceptance.c）→ 根因 = /build-feature 流程「红基线先于 builder」与 per-agent scope 划分冲突；而继承稿 BUILDER DUTY 5) 本就把接线划给 builder → 证据锚 = acceptance-author 报告第五节 + scope 边界文本。human 裁 builder step-0 落线（设计内路径），流程字面冲突以红基线随 step-0 现跑化解。
- **F4 · 变异证据转录格式两轮返工**：现象 = 我把 builder 变异证据转录进 taskpackage，mutation-check 连红两轮 → 根因 = 闸按 acceptance **全文长串**匹配记录（mentionsId 全文 includes），首版只写了裸 id → 证据锚 = `.claude/scripts/mutation-check.mjs:90`（evidenceMatchesId）+ mirror-desync 房型（每条带 `acceptance` 全文 + turnedRed）。教训：回填机读字段先看闸的匹配逻辑或抄最近同闸绿包房型。
- **near-miss**：TEST 2 段错误杀整条 runner——若判读者只看「FAIL 数变了」而没注意崩溃截断（FAS-A1 与存量 242 未跑），会把 9 红误读成设计态红放行实现。接住它的是 builder 的 pty 捕获 + 逐测试名单核对。环境配额中断两次（换号续跑），无状态损失（worktree 持态）。

## 4. 优化建议（按 ROI）

1. **FAS-MANIFEST 机制抽通用候选**：治 F1 · 载体 = ARQ 收件箱（gov-arq 快路径：结构面断言 configure 期采集 / 运行时只读构建产物，替代运行时读源码树；本包 human 定向裁定的推广候选）· 预期收益 = 消树扫镜像类伪红 + 段错误面。
2. **验收接线归属对齐**：治 F3 · 载体 = ARQ 收件箱（dev-loop 流程 SSOT 注记：changeManifest 含 host CMakeLists 的接线类改动，红基线未成立前由 builder step-0 落线，acceptance-author 边界不含接线）· 预期收益 = 消一次 human 裁定点。
3. **变异证据房型速查**：治 F4 · 载体 = 登记不硬治（本 retro §3-F4 即房型记录；planner 回填前先读最近同闸绿包）。
4. **78 fork 依赖入 package.json**：登记不硬治（Review Packet SHOULD 条已记；运行期守卫已收敛风险）。

## 5. 一句总结

成本 ≈ 1.26M 子 agent tokens + ~7h 有效墙钟，human 触点 4 次（影子树叫停 / 弃树方案裁定 / step-0 双裁 / 合并门待裁）· 最险一刻 = TEST 2 段错误假红风险（pty 名单核对接住）· **保险丝判定：两方独立结构接住了作者偏差**——acceptance 冻结 + step-0 设计态红门在实现前拦下两个 PROPOSAL 级未验证前提（F2），机械 verifier 在收口前拦下证据缺位与格式错（mutation-check 两轮）；最值得带走的一条 = 「未验证前提要用最便宜的一步实测先行」——step-0 红基线正是那个最便宜的一步。

## 6. 本环遗留

- **FAR-E3 真机轮未执行**（P0 收口要件）：探针页看屏 + 串口日志 + 缺 assets 降级开机 + host 解码器↔cbin_font_create 等价性——归 planner/human 真机轮；P1/P2/P3 在其收口前不 launch（spike-first 硬边界）。
- FAS-A2 canary 注释 0x732D/0x733E 笔误：builder 以超集字表化解，冻结件不改，acceptance-author 复核归后续（Review Packet NIT）。
- 78 fork 入 package.json（SHOULD，可选）。
- verification.json 机器件本批无脚本产出（advisory-only 留根）。
- loop-retro 阈值到（18 ≥ 3）——主线程择机发起 /loop-retro。
