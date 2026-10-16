# retro · 260909-gov-review-gate-truthfulness（Track B 全环：立项 ack → R1 NEEDS-WORK → v0.3 修订 → R2 GREEN → FROZEN → landing → verifier RED→GREEN）

> 主线程第一人称热记忆 · 2026-09-09 · Track B 乘客序列新员 · harness 形态：判官 = architect 子 agent（R1/R2 同实例 resume），核验官 = landing-verifier 子 agent（零上下文独立实例）· 跨乘客对照：与 260908 两批同属「fidelity 格式门首轮 RED」家族，但本批 R1 锚点零漂移（前两批 R1 均锚点失真打回）。

## 1. 时间账

无逐相位时钟戳的机械采集，锚点 = commit mtime 链 + 子 agent 返回时长：

| 相位 | 内容 | 锚点 |
|---|---|---|
| 起草 | PROPOSAL v0.1（前一主线程会话） | commit 2d419e1 |
| 立项 ack | human「立项，按推荐 ack」→ v0.2 落文 | 本会话 |
| 评审 R1 | NEEDS-WORK（B1+S1–S5+C1–C4），判官独立重跑五条量化声明簇全数复现 | architect-review-r1.md，子 agent 约 22 分钟 |
| 修订 | v0.3 十条 findings 逐条回灌 | 本会话连续 |
| 评审 R2 | GREEN（0/0/0） | architect-review-r1.md R2 节 |
| FROZEN | v1.0 + INDEX upsert（旧判据 `frozen\|yes` 留证） | commit c110956 |
| landing | T1–T7 落 8 canonical + V1–V6 全绿 + 首真编 build-pass | commit 757ba61 |
| 核验 | RED（格式两项）→ 修复 → GREEN | landing-verdict.md，修复 commit 489e3dc |

对照：260908 两批评审各自拖到 R2 且 R1 都含锚点/引文失真；本批 R1 的 BLOCKING 是历史归因失真（写作事实错），锚点层零漂移——「每条量化断言命令随文可重跑」的起草纪律（§6 自律件）见效。

## 2. Token 账

计量方式诚实声明：无全链路 token 表；子 agent 三次调用有精确值（判官 R1+R2 同实例合计 usage 见会话记录，R1 = 121,662 tokens / 31 工具调用；核验官两轮同实例，首轮 71,762 tokens / 39 工具调用），主线程侧无分项计量，估计占大头（起草修订 + 8 文件落地 + 验证集执行），误差界 ±50%。花费最大处 = 判官 R1 的实测重跑（五条声明簇全跑而非抽样三条，含自写 census 遍历脚本）——这是本批最值的单笔花费：它把「评审」从读文本变成重跑实验。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · P5 历史归因失真险落 canonical**：现象 = 初稿把 3 例 gov 批 `--no-ff` 双亲合并（8bd4a84/8c1cba3/33d8009）误归因 merge-driver dev 通道，T7 拟落文本的「与既有实务一致」按字面为假 → 根因 = 起草时只看了 `git reflog` 近期 5 条（全 ff），没跑 `git log --merges` 全量核对历史反例 → 证据锚 = architect-review-r1.md §C-B1（判官 `git show -s 8bd4a84` 亲核）。教训：写「实务一贯如此」类句子前，`reflog` 近期窗口不等于历史，`git log --first-parent --merges` 才是全量面。
- **F2 · landing-fidelity 三条格式契约只活在门脚本错误消息里**：现象 = landing 首轮 RED，两项全是格式（summary 缺 manifest 围栏块 B2、缺 packet-binding 引述 A9；§2 四条锚行连写触 B7 行首形态 fail-loud）→ 根因 = B2/A9/B7 三条格式契约未进 PROPOSAL 模板与本批 §10 硬条款文本，起草者无法先知 → 证据锚 = 489e3dc 修复 commit + PROPOSAL §8。这是连续第三批踩同族坑（260908 两批各有首轮 RED），格式契约双向可见性是真病灶。
- **F3 · V4 字面在 packet 边界内物理不可满足**：现象 = frozen V4 要「gatesNotExecuted 进 round fact」，但 fact 顶层键白名单封闭（verifier-evidence-check.mjs:281）且该文件不在 manifest → 根因 = 起草验证集时没核 fact schema 的白名单约束 → 证据锚 = landing-summary D1 + verifier D1 判定（等价载体信息零丢失，可接受）。教训：验证集条款的每个名词（round fact / verdict.json / payload）在起草期就得对到具体文件的键集约束。
- **near-miss（最险一刻）**：T3 落地后首跑主闸，venv 探针通过 → 闸直接进入**真编译**（本仓历史上第一次），我起初把 120 秒超时误判成「探针卡死」差点去改探测逻辑——实际是 `idf.py build` 正常跑了几分钟并 build-pass。没出事是因为先读了后台输出再动手。教训：向严落地的「第一次真跑」会改变你熟悉的耗时分布，别把「变慢」读成「坏了」。

## 4. 优化建议（按 ROI 排序）

1. **格式契约三条款（B2 双侧含块 / A9 packet-binding / B7 锚行行首形态）写进 `docs/templates/PROPOSAL.template.md` 与 PROPOSAL §10 硬条款段**：治 F2 · 载体 = 固化候选 → ARQ 收件箱 · 预期收益 = 消灭「fidelity 首轮 RED 全是格式项」家族（已连续三批）。
2. **fact 顶层键白名单扩展（gatesNotExecuted/gatesRun 进 round fact 顶层）连同 verifier-evidence-check.mjs 同批**：治 F3 / D1 留根 · 载体 = 留批次（与 OQ4=(b) 的 degraded 机械消费批合并做，见 §6）· 预期收益 = 汇总字段单载体化，verifier/仪表盘读一处。
3. **起草期「历史全量核对」纪律进 PROPOSAL 模板 §1**：凡写「实务/一贯/既有」类句子，附全量命令（`git log --merges` 类）而非近期窗口 · 治 F1 · 载体 = 登记不硬治（进模板一句话，不加门）。
4. **V7 首轮真跑红的消解排程**：acceptance 闸现恒红 8 条（test_smoke_e2e 7 + test_voice_roundtrip_b 1）· 载体 = 留批次（dev-loop 或独立批；三个待 launch Pod 的 knownRedFloor 基线 = 8，launch 面 T5 机制已就位承接）· 预期收益 = 红基线清零前，verifier 每轮都要对一次基线，越早消越省。

## 5. 一句总结

成本：判官约 12.2 万 tokens（R1，含全量重跑）+ 核验官约 7.2 万（首轮）· human 触点 2 次（立项 ack + 本 retro 后 merge 前的 §8 追认确认，皆在册）· 制品 5 件（PROPOSAL/评审记录/landing-summary/landing-verdict/retro）· 对照 260908 乘客：评审轮次同为 R2 收敛但锚点零漂移、fidelity RED 同族但修复零语义 · 最险一刻 = F1 的误归因差点以「与既有实务一致」落进 canonical protocol 文本 · **保险丝判定：两方独立结构接住了作者偏差**——判官 R1 用独立命令重跑抓住 F1（我没发现的写作事实错），核验官首轮 RED 抓住 F2（格式缺口），两层都接在落地进 main 之前 · 最值得带走的一条：**「每条量化断言命令随文可重跑」是起草期最贵的纪律，也是评审期最省的投资**——判官全数复现后，评审争议面直接塌缩到写作事实与格式层。

## 6. 本环遗留

- **D1 留根**：fact 顶层键白名单扩展 → 与 OQ4=(b) degraded 机械消费批合并（建议 4.2）。
- **acceptance 闸恒红 8 条**：V7 首证，三个待 launch Pod（ui-art-integration / hardware-validation / audio-bargein-hw）的 knownRedFloor 基线 = 8；消解走 dev-loop/独立批，不塞给被 launch 的包（T5 步骤 2 已成文「已知红 ≠ 本包红」）。
- **OQ3=(c) 尾巴**：`server/pytest.ini` addopts 真单源化另登 ARQ（自有实现面，dev-loop 或 human 一次性授权）。
- **C 侧验收证据 root 定位吃 cwd**：`firmware/main/test/**.acceptance.c` 相对路径根因走 dev-loop（PROPOSAL §6 留根，launch 面已写指针）。
- **§8 追认确认**：human merge 前确认 PROPOSAL §8 两条格式修复（零语义改动）。
- **治理静默期建议**（human 2026-09-09 主线程讨论裁定方向）：本批后非证据触发不开新 gov 批；剩余留根均由三个 Pod 实跑证据或 cadence 复盘承接。
