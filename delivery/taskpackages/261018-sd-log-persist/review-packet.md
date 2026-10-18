# Review Packet · 261018-sd-log-persist

> integrator · 2026-10-18 · rigor=full / riskTier=high / sourceType=proposal · 分支 `dev/pod-m2` · **无 BLOCKING finding，可交 planner 跑 supervisor 机械闸；integrator 不 push、不 merge、不自行判定通过**。本包 UI 面 = 零（无 UIUX 工作，design-review 证据节不适用）。

## Change Size

- **实现面 + 验收面（9 件，vs main）**：`firmware/main/diagnostics/sd_log.h`（新 +134）、`sd_log.c`（新 +341）、`sd_log_fs.cc`（新 +260）、`firmware/main/main.cc`（+11/-0）、`firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+9/-1）、`firmware/main/CMakeLists.txt`（+8）、`firmware/main/test/host_app/main/CMakeLists.txt`（+8）、`firmware/main/test/sd_log.acceptance.c`（新 +1930 冻结验收）、`firmware/main/test/sd_card_mount.acceptance.c`（+44/-7 跨包重锚）。合计 **9 files, +2745 / -8**（`git diff main --stat -- firmware/` 实测）。
- **文档面**：本包零新增 canonical 文档改动——`docs/architecture.md` / `docs/ADR.md` 的改动属 arch landing commit `5a37b001`、`TECH-261016-sd-story-confidentiality.md` +2 属 `27b4ecd5`，均先于本包实现面；本包实现提交不改 `docs/`（详见 `docs-delta.md`）。
- **闭环规模**：分支 `dev/pod-m2` vs main 全量 = 28 files, +4389 / -294（含 delivery 簿记与架构面先行落地件；不含本 packet / docs-delta / test-scope 等 closeout 制品自身）。
- **Scope**：板端诊断日志采集（esp_log vprintf 链式钩子）+ PSRAM 128KB 环形缓冲 + 低优先级写盘任务 + 8.3 有界轮转（`/sdcard/logs/KPLOGnnn.TXT`，512KB × 32 ≈ 16MB 封顶）+ 早启动留存 + 无卡/坏卡降级；零 wire 形状改动、零 `server/**` diff、零分区表 / sdkconfig 改动、零新增依赖。
- **Split recommendation**：**不拆**（单包 Full；架构面 T1–T8 已随 `5a37b001` 先行 landing；跨包重锚是单点窄口径事项，已随本包扩单收口）。

## Verification Evidence

- **前置绿**：scoped verifier round 1（invocationId `d6193334-18bf-4847-a4ad-fe865ccb9fa6`）= **8 闸全 PASS**（`r1.json`，`ok:true`，`observedGateExit:0`；含 firmware-build 真编译 build-pass）。
- **Final 相位（bearing）**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261018-sd-log-persist/test-scope.txt --change-id 261018-sd-log-persist --round 2 --invocation-id 0f3e6b1a-7c52-4a9d-9f61-2b8e4d7c5a10` → `{"ok":true,"phase":"final","round":2,"evidenceRef":"…/verifier-evidence/rounds/r2.json","observedGateExit":0}`；**10 闸全跑全绿**：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（真编译 build-pass）/ contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit（executor-run）/ acceptance（oracle-pass + executor-run）。
- **`verifier-evidence-check`**：`--phase final --round 2 --invocation-id 0f3e6b1a-7c52-4a9d-9f61-2b8e4d7c5a10` → `{"ok":true,"gate":"verifier-evidence","factOk":true,"observedGateExit":0,"phase":"final","round":2}`（exit 0）。**承重判断只认 r2 fact**；`current/verdict.json` = verify.mjs 脚本 fs-write 的 non-bearing projection，包目录 `verdict.json` / `verification.json` = integrator 从 r2 fact 程序化导出的 advisory 投影（可被 closeout-check 判缺失，但不作门）。
- **设备编译闸**：`firmware-build` **真编译 build-pass**（工具链可用；`xiaozhi.bin binary size 0x322180 bytes`、最小 app 分区余量 9%；新源 `diagnostics/sd_log.c` 与 `sd_log_fs.cc` 均经 `firmware/main/CMakeLists.txt` 显式登记参与编译）。
- **功能闸（按 test-scope 圈定真跑）**：`unit` 与 `acceptance` 两闸均 `executor-run`：**26 passed**（scope 内 2 件 server acceptance；0 deselected / 0 live）；`acceptance` 同时 `oracle-pass`（firmwareAcceptanceFiles 22 / serverAcceptanceFiles 2）。
- **integrator 独立全量交叉核对（闸外，2026-10-18 本 worktree 实测）**：WSL host_app 全量构建 + `host_infra_test.elf` 实跑 = **203 Tests 3 Failures exit 3**；3 条失败全部为 `p1_auxbar` **AB-A1 / AB-A2 / AB-A4 先存红**（既有、与本包正交）；本包 `sd_log` **8 齿全 PASS**（SLP-A1×2 / A2 / A3 / A4×3 / A5），跨包重锚后 `sd_card_mount` **4 齿全 PASS**（SD-A1/SD-A2/SD-A3/SD-A4）——零新增红，SLP-X1 成立。
- **Mutation check**：`mutation-check` PASS；TaskPackage `mutationEvidence.perAcceptance` 六条（SLP-A1..A5 各一 + 跨包重锚 XPKG 一条），每条均 `turnedRed:true` / `sensitive:true` 并带 redVerbatim 与 restored 证据；跨包重锚变异（max_files 8→5）同时打红本包 SLP-A4 与 261016 SD-A2 tooth 6，证明重锚齿敏感。
- **Security gate**：`security` PASS；`securityReviewRequired=true`，三轴证据在 TaskPackage `securityAxes` / `securityEvidence`（data-at-rest 明文对话原文 = human 有意识接受 + TECH-261016 记账；路径安全 = 固定 8.3 模板零用户输入；降级不崩 = 无卡零 fopen / 30s 重试 / 不 panic 不 abort）。
- **Docs sync**：`docs-sync` PASS；`docRefs` 七条全部在盘；本包零新增 canonical 文档改动（口径与核证见 `docs-delta.md` §1）。

## test-scope 清单（final 相位范围圈定）

- **落盘路径**：`delivery/taskpackages/261018-sd-log-persist/test-scope.txt`（`verify.mjs --phase=final --scope-file` 消费，round 2）。**本行即「清单路径记入 results 元数据必含行」的落点**——r2 fact 的 `results[]` schema 固定（无 scopeFile 字段），scope 路径由本 packet + 各闸 stdout 承载。**共 27 条**（25 件 firmware 测试 + 2 件 server acceptance）。
- **圈定方式 = 正常路径（非降级）**：per-Pod 专属图索引 `C-AI-KidPal-pod-m2` 可用，`detect_changes`（base=main，merge_base `21360279`，inbound depth=2）读到 changed_files 31（含 delivery 簿记）/ seed_symbols 214 / **impacted_total 24**（firmware/main 18 + server/acceptance 6）；无 trace_path 降级、无「图不可用」举手。
- **范围构成**：① `acceptanceTestsFile` = `sd_log.acceptance.c`；② 跨包重锚冻结件 = `sd_card_mount.acceptance.c`（强制覆盖，TaskPackage `impactedTestsSeed` 明文要求）；③ 共享 seam = 扫描本包改动源 `main.cc` / board `.cc` 的既有冻结源锚齿（boot_auto_connect / char_stroke / char_window_overlay / key_layer_dispatch / p1_home_battery / record_session_repair / reminder_overlay_unify / schedule_grid / story_cache / story_page_v2 / time_sync / voice_round_ui）；④ 安全余量 = `HOST_ACCEPTANCE_SRCS` 登记的其余现役冻结验收 + 3 件 builder 单测（同 ELF 同源编译，新登记影响链接/注册序面，故全量保留，不缩到只剩本包档）；⑤ detect_changes 影响集中的非 archive server 测试 2 件（向严纳入真跑）；⑥ 设备编译面由 `firmware-build` 闸（timing=scoped、final 恒跑、不消费 scope-file）覆盖。
- **排除申报**：`server/acceptance/archive/` 三件（图影响集内）= 已退役面（闸枚举 SKIP_DIRS 硬排除），非现役测试面；其余 `server/acceptance/*.py` / `server/tests/*.py` 不读本包改动源；设备运行时调用链（application/audio/mcp_server/protocols）host 不可达，由 firmware-build + SLP-A6 真机轮承载。
- **floor**：本包 `acceptanceTestsFile` ∪ 跨包重锚冻结件 ∪ blast radius 映射 ∪ 共享 seam ∪ host 全量安全余量（单包，无 workstream 对端冻结件）。

## 跨包冻结齿重锚 + 授权链 + 双绿证据

- **事实**：`firmware/main/test/sd_card_mount.acceptance.c`（261016 已合并冻结件）SD-A2 tooth 6 期望字面 `max_files` 5→8 + 四处文字镜像（`:49/:94/:556/:559`）+ tooth 6 注释/红名文本 + 文件尾 REVISION LOG append（不覆盖 2026-10-16 既有两条）；判别机制与其余断言零改动（`git diff --numstat` = 44 / 7，删除行恰为上述 7 行）。
- **授权链（可逐环锚定）**：人 escalation `8c1db24e-a926-47ca-868a-35854500a40d` → **human-decision `88a08bbd-c542-41e5-bdc4-fb2739dc24ef`**（2026-10-18，「批准 A」，窄口径：仅 tooth 6 期望值 5→8 + 四处文字镜像 + REVISION LOG，其余断言与 261016 收尾制品零改动；pod-events 在盘、`originalInputRef` 保留原话）→ architect 1g 正式扩单回评（2026-10-18）→ planner 先扩 `changeManifest` / `impactedTestsSeed` 后由 acceptance-author 动文件（顺序合规）。
- **同族前置冲突已消解**：261016 冻结件钉 5 vs 本包 plan 钉 8 的互斥在重锚后双绿——host 全量 203 Tests 里 `sd_card_mount` 4 齿 + `sd_log` 8 齿同时 PASS；变异证据（板 `.cc` max_files 8→5）同时打红两侧齿。
- **防先例**：REVISION LOG 已逐字写入「一次性窄口径授权，不构成 plan 提到某值即可改任何钉该值的冻结齿的先例」；261016 的 taskpackage / review-packet / mutation 记录保留历史原文不动。

## 集成结果（收拢 / 冲突 / 回滚）

- **分支与范围**：全部改动收拢于本树常驻分支 `dev/pod-m2`；实现面 9 件 + `sd_card_mount` 重锚 1 件 + delivery 簿记 / closeout 制品。本包为**单包集成，无跨包合并冲突**（架构面 landing 已先于实现面落地；`current/taskpackage.json` 与 canonical 字节相等实核）。
- **closed set 实核**：`git status --porcelain` + `git diff main --stat` 的改动面 == `changeManifest` ∪ 冻结验收重锚 ∪ delivery 簿记；清单外实现面文件 = 0（`change-manifest` 闸 PASS；本包首提交日期 < manifest cutoff，该闸按存量放行，integrator 另做人工对账）。
- **零触碰面**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`docs/ADR.md`（本包实现提交）、`server/**`、`firmware/partitions*`、`firmware/sdkconfig*` 实测零 diff；261016 其他收尾制品零触碰。
- **回滚路径**：本包尚未 merge——回滚 = 丢弃 `dev/pod-m2` 分支或 revert 本收口 commit；无数据迁移、无不可逆制品（`firmware/build` 为可再生构建产物）。合并动作由 planner 主线程在 supervisor APPROVE + 前置升级闸放行后执行，integrator 不 push、不 merge。

## 架构 / 依赖卫生检查摘要

- **分层与同源**：`diagnostics/sd_log.c` 纯逻辑（零 FATFS / 零 FreeRTOS / 零 LVGL / 零芯片依赖，host_app linux target 与设备树同源编译）；芯片面 `sd_log_fs.cc`（esp_log 钩子 / PSRAM / FreeRTOS 写盘任务）不进 host 构建。`main.cc` 与板 `.cc` 只经 `sd_log.h` 对外声明调用，无反向依赖、无循环依赖。
- **接线顺序**：`app_main` 最早期 `KidpalSdLogInit()` → `esp_log_set_vprintf(KidpalSdLogVprintf)`（旧指针交回、钩子链式转调）→ 均早于 `nvs_flash_init()`；板级仅挂载成功分支 `KidpalSdLogAttach(SD_MMC_MOUNT_ROOT)`（`s_sd_mounted=true` 后、`RunBootRoundtripProbe()` 前）；`max_files` 5→8 与重锚齿同值。
- **递归守卫**：写盘任务函数窗内零 `ESP_LOG*`（实测 grep = none；SLP-A4 结构齿也钉住）。
- **依赖卫生**：零新增第三方依赖 / 零 managed_components 变更；两处 CMakeLists 仅显式登记本包自有源（main 目录非 GLOB，已按 plan findings F1 补齐）。
- **重复实现 / 本地类型**：无重复实现；无绕过 contracts 的本地类型（本包零 wire 面）。数值单源 = `sd_log.h` 宏（RECORD_MAX / BUFFER_BYTES / FILE_BYTES / FILE_COUNT / FLUSH_* / RETRY_MS / TASK_PRIO），冻结验收 TEST 8 扫宏字面防漂移。

## 本包效果对账（SLP-A1..A6）

| 验收 | 承载 | 实测结果 |
|---|---|---|
| SLP-A1 环形缓冲有界 / 丢最旧 / 多写者不撕裂 / drain 指针推进 | `sd_log.acceptance.c` TEST 1/2 | 绿（host 实跑 PASS；变异「拒绝新写」双齿打红） |
| SLP-A2 8.3 命名 KPLOG000..031 wrap / 512KB 分片 / 32 文件封顶 16MB | TEST 3 | 绿（变异「KPLO%04u」打红） |
| SLP-A3 早启动留存（attach 不清缓冲、从最旧 drain） | TEST 4 | 绿（变异「attach 清空」打红） |
| SLP-A4 接线结构五件（钩子 / attach / max_files=8 / PSRAM 128KB / 写盘任务零 ESP_LOG） | TEST 7 + 结构齿 | 绿（变异 max_files 8→5 打红本包与跨包两侧） |
| SLP-A5 无卡零 fopen / 30s 重试 / 不 panic / 恢复 drain | TEST 5/8 | 绿（变异 RETRY_MS 30000→1000 打红） |
| SLP-A6 真机轮四项（live-manual） | human 持板 | **未跑，停门等 human**（本 Pod 零烧录） |
| SLP-X2 设备编译闸 | firmware-build（final） | 绿（真编译 build-pass） |
| SLP-X3 分区表 / sdkconfig 零改动 | `git diff main...HEAD -- firmware/partitions* firmware/sdkconfig*` | 空（成立） |
| SLP-X5 docs-sync 完成或显式裁定不需要 | `docs-delta.md` | 完成（零新增 canonical 改动，声明 + 核证） |

- **demo / roadmap 对账行**：本包无 roadmap 效果段（`deliveryImpact = none`、`planningImpact = none`、`planningWorkstreamRequired = false`，无 workstream）；效果面 = 板端可观测性能力，真机效果观测归 SLP-A6。

## 未覆盖面申报（逐条一行）

- ① **SLP-A6 四项 live-manual 未跑** —— 现状 = 本 Pod 零烧录、零真机；去向 = human 持板轮（`/sdcard/logs/KPLOG*.TXT` 含 STT/LLM 原文 / 串口 COM5 不中断 / 32×512KB 覆盖最旧且目录 ≤16MB / 拔卡坏卡不崩 + 30s 恢复）。
- ② **真实 ISR 上下文写入未测** —— 现状 = host 只以 pthread 多写者模拟并发（SLP-A1 口径「ISR 语义模拟」）；实现用 `__atomic` 自旋锁，非关中断临界区；去向 = 真机轮观察 + `sd_log.c:15` minimal 记账（若出现 ISR 内 ESP_LOG 需求，升级为芯片面 `portENTER_CRITICAL` 包装）。
- ③ **真实 FATFS / 坏卡 / 拔卡 I/O 路径未跑** —— 现状 = chip 面 `sd_log_fs.cc` 不进 host 构建，host 仅验纯逻辑决策表（`should_flush` / `retry_due`）与源码结构；去向 = 真机轮 SLP-A6 ③④。
- ④ **真实 PSRAM 分配与内存压力未跑** —— 现状 = host 用调用方存储模拟环形缓冲；`heap_caps_malloc(MALLOC_CAP_SPIRAM)` 只在设备编译期检查符号可用；去向 = 真机轮观察日志链是否在 PSRAM 紧张时不拖垮语音链路。
- ⑤ **真实串口链式输出连续性未观测** —— 现状 = 结构齿断言「保存旧指针 + 先转调」，host 无 UART；去向 = 真机轮 COM5 观察（SLP-A6 ②）。
- ⑥ **真实文件系统下 8.3 命名 / 16MB 封顶 / 跨重启续写未跑** —— 现状 = 纯逻辑命名与轮转记账已 host 测；`stat` 播种续写路径只在设备可达；去向 = 真机轮 SLP-A6 ③。
- ⑦ **`p1_auxbar_style` 三条先存红（AB-A1/A2/A4）未修** —— 现状 = 既有红、与本包正交（本包零新增红）；去向 = 其所属修复包 / 既有跟踪，不在本包范围。
- ⑧ **跨包重锚文件名最终字节 sha256 未写入 REVISION LOG 自指** —— 现状 = REVISION LOG 记录重锚后、日志 append 前的哈希 `ec74a68a…`，含日志的最终字节哈希在 261018 提交记录留存（自指不可闭合，文件内已如实说明）；去向 = 审计以 `git show` 本收口 commit 为准。
- （零缺口项明写）**none** —— 除上述 ①–⑧ 外无其它未覆盖条目。

## UI design-review 文本证据

- **不适用**：本包零 UIUX 工作（板端诊断日志，零渲染面 / 零页面版本 / 零视口改动）。viewport / a11y / design-drift / external artifact 四项均无对象；无 residual UI risk。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| `firmware/main/diagnostics/sd_log.c:15` | 环形缓冲用 `__atomic` 自旋锁（两次 memcpy 临界区），不是关中断临界区 | 任务上下文多写者安全；ISR 内写入在「任务持锁 → ISR 自旋」形态下可能自旋至看门狗 | 出现「ISR 内 ESP_LOG」的真实需求 → 芯片面在钩子外再套 `portENTER_CRITICAL`（本层零 FreeRTOS 红线不动） |

- 说明：`waveshare-s3-rlcd-4.2.cc:448` 亦有一处 `minimal:` 标记，但属本包未触碰的既有行（PCF85063 无年字段），不计入本包简化债务。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | `firmware/main/diagnostics/sd_log.c:53-63` | 环形缓冲临界区是自旋锁，不是关中断临界区；若 `ESP_LOG` 真的从 ISR 上下文进入而某任务正持锁，ISR 会自旋等待一个无法被调度恢复的锁 | changed-code hunk（`sd_log_lock` 自旋实现）+ `minimal:` 标记；heuristic：spinlock-with-preemption-boundary；实际暴露面 = ESP-IDF `esp_log` 本身非 ISR-safe，ISR 内打日志非常规用法 | **accepted-with-rationale**：已记入 Simplification Debt 与未覆盖面 ②；升级触发 = ISR 内 ESP_LOG 成为真实需求时改 `portENTER_CRITICAL` |
| NIT | judgement-call | `firmware/main/diagnostics/sd_log_fs.cc:222` ↔ `sd_log.c:101-116` | `s_pending_bytes` 在记录入库时累加，但环形缓冲满溢「丢最旧」消费路径不递减；长期高频日志下该计数会单调偏大，使 `should_flush` 的 4KB 批量门更早触发 | changed-code hunk + heuristic：counter-not-decremented-on-drop；影响仅为刷盘节奏（仍受 1s 门控、且 `pending_records==0` 短路），不影响落盘正确性 | **accepted-with-rationale**：低优先级单写者下无功能影响；如后续真机轮观察到刷盘过频，可在 `ring_write` 返回被丢弃字节数后对齐扣减 |
| NIT | judgement-call | `firmware/main/diagnostics/sd_log_fs.cc:112-115` | `DrainPending` 逐条 `fwrite + fflush`，不是按 4KB 块合并刷；SD 卡每次 fflush 都是真实写事务，可能放大写延迟与磨损 | changed-code hunk + heuristic：flush-per-record；写盘任务低优先级、批量门控只决定何时开始 drain | **accepted-with-rationale**：真机轮观察写盘对语音链路的影响；如确有影响可在 drain 循环内改为按块 flush |
| NIT | judgement-call | `firmware/main/diagnostics/sd_log_fs.cc:202-228` | 钩子对每条日志在调用栈上放 512B 缓冲并跑 `vsnprintf`；小栈任务（音频 / 渲染）里打日志会额外占用栈 | changed-code hunk + heuristic：fixed-stack-format-buffer-in-hook；IDF 默认任务栈通常 ≥2KB，512B 为可控上界 | **accepted-with-rationale**：真机轮观察有无栈溢出/看门狗；如出现，可改为芯片面静态复用缓冲（单写者以外需加锁） |

> 无 BLOCKING finding、无 hard-violation；结构闸 PASS 不复述为 finding。**本包无未决 BLOCKING ⇒ 可进入 supervisor 机械闸。**

## Five-Axis Review

1. **Correctness**：环形缓冲「满 → 丢最旧再接纳」、FIFO drain（peek 不推进 / commit 才推进）、未 attach 零消耗；8.3 命名 000..031 wrap、512KB 分片、32 文件封顶 16MB；attach 只切标志不清缓冲（早启动留存）；钩子先链式转调旧 vprintf 再入库（串口不中断）；写失败 MarkWriteFailed → 30s 重试 → 恢复 drain 且不丢最旧条目；无卡路径由板级降级 return 挡在 attach 之前（零 fopen）；写盘任务零 ESP_LOG（防递归）。以上均由冻结验收 + 变异敏感度双证。**未执行面**：真实 ISR / FATFS / PSRAM / UART（见未覆盖面 ②–⑥）。
2. **Test quality**：1930 行冻结验收，8 个 TEST_CASE 断言真实函数返回值与真实工作树源码结构（非「没报错就算过」）；五条 SLP 变异 + 一条跨包重锚变异全部 `turnedRed/sensitive`，redVerbatim 逐字留痕、restored 均以 `git checkout --` 复原并复跑；负锚齐（max_files 非 5 / RETRY_MS 非 1000 / attach 清空即红 / 写盘任务 ESP_LOG 即红）；注册序与 smoke 末位守卫沿用 host_app 既有形态。诚实边界 = SLP-A6 真机四项无自动断言（structural/live-manual 明示）。
3. **Contract/boundary safety**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`docs/ADR.md`（本包实现提交）零 diff；零 wire 形状改动、`server/**` 零 diff；零新增依赖；无跨层调用 / 无循环依赖；无绕过 contracts 的本地类型；分区表与 sdkconfig 零改动（SLP-X3 实核）。
4. **Operability/security**：隐私面（STT/LLM 对话原文明文落可拔卡）为 human 有意识接受，TECH-261016 记账在案，本批不写加密/脱敏断言；路径由固定 8.3 模板生成，零用户输入，无注入面；降级不崩（无卡 / 坏卡 / 写失败均不 panic、不 abort、不阻断语音主链路）；日志目录创建失败即整层失效并 WARN；残余 = 自旋锁 ISR 边界、刷盘粒度、钩子栈占用（均 NIT 记账）。
5. **Maintainability/simplicity**：纯逻辑 / 芯片面按 `story_cache` 先例分离；冻结数值单源为 `sd_log.h` 宏并被 TEST 8 钉住；两处 CMakeLists 显式登记并附注释锚包号与理由；代码注释逐处锚 [C1]-[C5] 与 plan 决策编号；零过早抽象、零 `minimal:` 之外的额外简化；`minimal:` 一处已入债务表。

## Residual Risk

- **`decisionCoverage` risk-accepted 条目**：无——本包 `landsIn: dropped` 的条目为 D11（panic/OOPS 不落卡）/ D12（裸 printf 与 bootloader 日志不经钩子）/ D13（不做热插拔检测），其 `reason` 均以「v1 明确边界」起头，**不是 `risk-accepted`**，故本节无 risk-accepted 行可列；三条边界本身已在 plan 与 TaskPackage 中显式声明。
- **Known limitation**：① SLP-A6 真机四项未跑（本 Pod 零烧录）；② 真实 ISR / FATFS / PSRAM / UART 运行时未在 host 复现（chip 面不可达）；③ `p1_auxbar` AB-A1/A2/A4 先存红仍在（与本包正交）；④ 自旋锁 ISR 边界、pending_bytes 溢出差、逐条 flush、钩子栈占用四项 NIT 已记账。
- **Follow-up recommended**：human 持板轮按 SLP-A6 ①②③④ 逐项观测并留痕；如真机轮观察到刷盘过频或栈压力，按 Findings 表升级触发处理；pod-retro 由 planner 收尾相位落 `pod-retro.md`（含 intake `scan-miss` friction 收口）。
- **Human decision needed**：SLP-A6 真机轮放行与验收结论；隐私面（对话原文明文落卡）已拍板接受，如有变更需求另立批。
