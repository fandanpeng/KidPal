# Review Packet · 261018-sd-log-usb-read

> integrator · 2026-10-19 · rigor=lite / riskTier=medium / sourceType=human-ad-hoc · 分支 `dev/pod-m2` · **无 BLOCKING finding，可交 planner 跑 supervisor 机械闸；integrator 不 push、不 merge、不自行判定通过**。本包 UI 面 = 零（无 UIUX 工作，design-review 证据节不适用）。

## Change Size

- **实现面 + 验收面（10 件，= changeManifest 闭集）**：`firmware/main/diagnostics/log_cmd.h`（新 +132）、`log_cmd.c`（新 +406 纯逻辑）、`log_cmd_port.cc`（新 +294 芯片面）、`sd_log.h`（+5 只读访问器声明）、`sd_log_fs.cc`（+14 只读访问器实现）、`main.cc`（+7 接线）、`firmware/main/CMakeLists.txt`（+9）、`firmware/main/test/host_app/main/CMakeLists.txt`（+10）、`firmware/main/test/log_cmd.acceptance.c`（新 +1616 冻结验收，已随 commit `efd76954` 先行落盘）、`tools/diag/kidpal_logs.py`（新 +158）。
- **文档面**：零新增 canonical 文档改动（详见 `docs-delta.md`；本包实现提交不改 `docs/`）。
- **Scope**：诊断日志按需 USB 串口取回——只读命令 `kplog list / tail / cat`（/dev/secondary VFS 非阻塞轮询读，不自动 dump、不上云、不改正常日志流）+ host 取回脚本；不触 `server/**`、零端云 wire、零分区表 / sdkconfig 改动、零新增依赖。
- **Split recommendation**：**不拆**（单包 Lite；决策 24 白名单与 §6.6 已随 persist 架构面 `5a37b001` 先行落地）。

## Verification Evidence

- **scoped verifier round 1（bearing，唯一既有 verifier fact）**：invocationId `56a4e75e-79c4-4c95-ab75-1282fe90835d` → `r1.json` `ok:true` / `observedGateExit:0`，**8 闸全 PASS**（taskpackage-required / change-manifest pre-cutoff 存量放行 / firmware-build notExecuted=true（rigor=lite scoped 相位后置，261015-gov-review-lite-fast-path 设计态）/ contract-lint / acceptance-coverage（LUR-A1..A7 全覆盖）/ mutation-check（9 变异证据全 `turnedRed:true`、`sensitive:true`、redVerbatim 逐字留痕）/ docs-sync / security `securityReviewRequired:false`）。
- **final 相位 round**：**未跑**——verifier fact 按「只消费不重跑」纪律由 planner/verifier 执行；scope-file 已备好 = `delivery/taskpackages/261018-sd-log-usb-read/scope-final.md`，待 final round 消费（含 firmware-build 真编译补跑，LUR-X2 的闸面证据落在该轮）。**当前 evidence 面 = scoped r1 + integrator 闸外实测（下两条）**。
- **integrator 独立全量交叉核对（闸外，2026-10-19 本 worktree 实测）**：WSL host_app 全量构建 + `host_infra_test.elf` 实跑 = **210 Tests 3 Failures exit 3**；3 条失败全部为 `p1_auxbar` **AB-A1 / AB-A2 / AB-A4 先存红**（既有、与本包正交，与 persist 红基线同批同三条）；本包 `log_cmd` **7 齿全 PASS**（LUR-A1/A2/A3/A4/A5 + LUR-A6×2），persist 包 `sd_log` **8 齿全 PASS**（`KidpalLogCmdStart` 钩子后加位下 TEST 6 main.cc 结构锚保持绿）——**零新增红，LUR-X1 host 半成立**。
- **LUR-X3 边界零改动实测**：`git diff HEAD -- 'firmware/partitions*' 'firmware/sdkconfig*'` 空；`git diff HEAD -- docs/` 空；`git status --porcelain -- docs/contracts/ server/` 空。
- **Mutation check**：TaskPackage `mutationEvidence.perAcceptance` 七条（LUR-A1..A6 各一，A6 含 M5..M8 四条独立变异；M9 补 TEST 2 缺口），9/9 单变异恰好只命中目标断言、无误伤其它 TEST，红行逐字取证、还原后复跑 7/0 绿（acceptance-author 两轮实跑 2026-10-19）。
- **Security gate**：`securityReviewRequired=false`（只读白名单 + 物理访问威胁模型与拔卡同族未加宽，architect 比例化 review 裁定）；注入拒绝面由 LUR-A2 行为齿承载（路径字符 / 非数字 / 越界即 BAD_INDEX/BAD_CMD，拒绝即短路、文件访问层零触达——TEST 2 以 log_cmd.c 可执行源零文件 I/O needle 钉死结构半）。

## test-scope 清单（final 相位范围圈定）

- **落盘路径**：`delivery/taskpackages/261018-sd-log-usb-read/scope-final.md`（`verify.mjs --phase=final --scope-file` 可消费，形态仿 persist 包同位产物 `test-scope.txt`）。**本行即「清单路径一行记入 final verifier fact / review-packet」的落点**——r1 fact 的 `results[]` schema 固定（无 scopeFile 字段），scope 路径由本 packet 承载，final 轮消费时由其 results 元数据与 packet 双留痕。**共 28 条**（26 件 firmware host 测试 + 2 件非 archive server acceptance）。
- **圈定方式 = 正常路径（非降级）**：per-Pod 专属图索引 `C-AI-KidPal-pod-m2` 可用，`detect_changes`（base=main，merge_base `21360279`，inbound depth=2）读到 changed_files 52（含 persist 包与 delivery 簿记）/ seed_symbols 264 / **impacted_total 19**；无 trace_path 降级、无「图不可用」举手。
- **范围构成**：① `acceptanceTestsFile` = `log_cmd.acceptance.c`（7 TEST）；② 相邻冻结 must-stay-green = `sd_log.acceptance.c`（8 TEST，persist 全齿 keep-green）；③ 共享 seam / 安全余量 = host_app 既有全量套件同 ELF 单命令天然覆盖（新登记影响链接/注册序面，全量保留、不缩水）；④ detect_changes 影响集内非 archive server 测试 2 件按向严纳入；⑤ 设备编译面由 `firmware-build` 闸（final 恒跑、不消费 scope-file）覆盖。
- **floor**：本包 `acceptanceTestsFile` ∪ 相邻冻结件 ∪ blast radius 映射 ∪ host 全量安全余量（单包，无 workstream 对端冻结件）。

## 集成结果（收拢 / 冲突 / 回滚）

- **分支与范围**：全部改动收拢于本树常驻分支 `dev/pod-m2`；本包为单包集成，**无跨包合并冲突**（persist 包改动已先行 commit 在同分支，本包在其上叠加，共享文件 `sd_log.h`/`sd_log_fs.cc`/`main.cc` 为纯增量、无交错改写）。
- **closed set 实核**：`git status --porcelain` + `git diff --name-only` 的改动面 == `changeManifest`（10 件）∪ delivery 簿记（pod-events / verifier-evidence / verify-runs / current/ 投影）∪ closeout 制品；清单外实现面文件 = 0（`change-manifest` 闸 PASS 按存量放行，integrator 另做人工对账）。
- **零触碰面**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`server/**`、`firmware/partitions*`、`firmware/sdkconfig*` 实测零 diff；persist 包冻结件 `sd_log.acceptance.c` 零触碰（相邻 must-stay-green 实测 8/8 绿）。
- **回滚路径**：本包尚未 merge——回滚 = revert 本包两个收口 commit（实现件 commit + closeout 制品 commit，单分支多 commit 可逐个 revert；冻结验收件随 `efd76954` 已 commit，如需整体回滚连同 revert 该 commit）；无数据迁移、无不可逆制品。合并动作由 planner 主线程在 supervisor APPROVE + 前置升级闸放行后执行，integrator 不 push、不 merge。**合并顺序受 OQ-1 主线程裁定约束**：persist 真机四项验完合 main 后本包 rebase 立合，persist 未验完前两包都不合。

## 架构 / 依赖卫生检查摘要

- **分层与同源**：`log_cmd.c` 纯逻辑（零 FATFS / 零 FreeRTOS / 零 LVGL，host_app linux target 与设备树同源编译）；芯片面 `log_cmd_port.cc`（/dev/secondary VFS 读任务 + FATFS 只读 + stdout）不进 host 构建。`main.cc` 只经 `log_cmd.h` 声明调用；`log_cmd.c` 经 `sd_log.h` 复用 8.3 命名单源（`kidpal_sd_log_file_name`）与 `KIDPAL_SD_LOG_FILE_COUNT`，无反向依赖、无循环依赖、无绕过 seams 的本地类型。
- **接线顺序**：`app_main` 中 `KidpalLogCmdStart()` 在 `KidpalSdLogChainPreviousVprintf(...)` 之后（冻结契约 [S8]，防止与钩子安装竞态）、早于 `nvs_flash_init()`；结构齿 TEST 6 源锚钉死该顺序。
- **递归守卫**：读任务函数体内零 `ESP_LOG*`（防 vprintf 钩子递归回灌环形缓冲）、启动函数零打印——均由结构齿源锚 + 变异 M5 双证；响应只经 stdout（fwrite/fputc）。
- **安全面**：文件名只经 `kidpal_sd_log_file_name` 或受校验整数构造（禁用户串直进路径拼接，LUR-A6①）；端口只 open `/dev/secondary`（无其它设备路径字面，LUR-A6②）；cat 索引闭集 0..31 拒绝越界；`kplog` 前缀白名单外输入静默零输出（防串口噪声触发）。
- **依赖卫生**：零新增第三方依赖 / 零 managed_components 变更；host 脚本用 IDF python env 既有 pyserial；两处 CMakeLists 仅显式登记本包自有源（main 目录非 GLOB，persist 教训登记位已补齐）。
- **冻结数值单源**：`log_cmd.h` 宏（LINE_MAX 256 / CMD_MAX 64 / TAIL_DEFAULT 200 / TAIL_MAX 1000 / CAT_COUNT→sd_log.h 单源），冻结验收 TEST 7 扫宏字面防漂移（EQ-2）。

## 本包效果对账（LUR-A1..A7）

| 验收 | 承载 | 实测结果 |
|---|---|---|
| LUR-A1 命令解析（合法形态 / tail 默认 200 上限 1000 / cat 闭集 0..31） | log_cmd.acceptance.c TEST 1 | 绿（host 实跑 PASS；变异 M1 钳制移除、M9 闭集放宽各自打红） |
| LUR-A2 非法输入拒绝（拒绝即短路、文件访问层零触达） | TEST 2 | 绿（变异 M9 打红；零文件 I/O needle 结构半同 TEST 承载） |
| LUR-A3 行装配（跨块拼行 / CRLF / 超长行丢弃 / 非前缀静默） | TEST 3 | 绿（变异 M4 超长行标记移除打红） |
| LUR-A4 帧格式化（BEGIN/END 字面 / list 行 + TOTAL / 错误帧） | TEST 4 | 绿（变异 M2 BEGIN 字面漂移打红） |
| LUR-A5 尾部窗口选取（末 N 行边界 / CRLF / 无结尾换行） | TEST 5 | 绿（变异 M3 off-by-one 打红） |
| LUR-A6 接线结构齿（文件名单源 / 只 /dev/secondary / Start 零打印 / 读任务零 ESP_LOG） | TEST 6 + TEST 7 | 绿（变异 M5..M8 四条各打红互异断言） |
| LUR-A7 真机轮九项（live-manual）+ 降级链 A/B | human 持板 | **未跑，awaiting-human:real-device**（本 Pod 零烧录；单命令 smoke 先行，A7①–⑨ + 降级链 A/B 待 human 持板逐项判定） |
| LUR-X1 host 齿全绿 + 既有套件零新增红 | host 实跑 210/3 | host 半成立（实测）；final 轮 unit/acceptance 闸求交为闸面证据 |
| LUR-X2 设备编译闸 | firmware-build（final） | **待 final 轮真编译**（scoped 相位 lite 设计态后置；持久包同位实测 9% 分区余量，本包仅 +708B 源增量，无分区风险） |
| LUR-X3 边界零改动 | `git diff HEAD` 实核 | 成立（分区表 / sdkconfig / 决策 24 白名单 / §6.6 / persist 写路径全部零 diff；sd_log.h/sd_log_fs.cc 只增只读访问器） |
| LUR-X4 真机轮 human 留痕 | awaiting-human | 停门等 human（本 Pod 不自行烧录） |
| LUR-X5 docs-sync 完成或显式裁定不需要 | `docs-delta.md` | 完成（零新增 canonical 改动，声明 + 核证） |
| LUR-X6 host 取回脚本落地且自描述 | `tools/diag/kidpal_logs.py` | 成立（--port/--cmd/--n/--index/--out/--timeout 齐；DTR/RTS 打开即拉低防复位伪影；单命令 smoke 可直接调用） |

- **demo / roadmap 对账行**：本包无 roadmap 效果段（`deliveryImpact=none`、`planningImpact=none`）；效果面 = 诊断取回工具能力，真机效果观测归 LUR-A7。

## 未覆盖面申报（逐条一行）

- ① **LUR-A7 真机九项 + 降级链 A/B 未跑** —— 现状 = 本 Pod 零烧录、零真机；去向 = human 持板轮（单命令 smoke 先行：`python tools/diag/kidpal_logs.py --port COM5 --cmd list --out list.txt`；静默性 / 帧完整 / 中文 UTF-8 / 读不干扰写 / 拔卡 no-card / 烧录零回归逐项判定；smoke 失败进降级 A=HAL 直读 / B=console primary 须回 triage 升档）。
- ② **真实 /dev/secondary VFS 可读性未在 host 验证** —— 现状 = 芯片面 `log_cmd_port.cc` 不进 host 构建，host 只验纯逻辑决策表 + 源锚结构；去向 = 真机轮 A7① smoke + A7②。
- ③ **真实 FATFS 只读并发（FS_LOCK=0 下与写盘任务并存）未跑** —— 现状 = host 不可达；去向 = 真机轮 A7⑥（读不干扰写）。
- ④ **tail 整文件读入堆缓冲（≤512KB）在真实 PSRAM/堆压力下未测** —— 现状 = `log_cmd_port.cc` `EmitTail` minimal 记账（诊断命令低频、分片上限 512KB）；去向 = 真机轮观察；升级路径 = 分块流式 + 回溯定位。
- ⑤ **host 脚本 pyserial 链路未实连验证** —— 现状 = 脚本自描述、退出码面完整，无自动齿；去向 = 真机轮 A7① 首验。
- ⑥ **`p1_auxbar_style` 三条先存红（AB-A1/A2/A4）未修** —— 现状 = 既有红、与本包正交（本包零新增红）；去向 = 其所属修复包 / 既有跟踪。
- （零缺口项明写）**none** —— 除上述 ①–⑥ 外无其它未覆盖条目。

## UI design-review 文本证据

- **不适用**：本包零 UIUX 工作（串口命令通道 + host CLI 脚本，零渲染面 / 零页面改动）。viewport / a11y / design-drift / external artifact 四项均无对象；无 residual UI risk。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| `firmware/main/diagnostics/log_cmd_port.cc:155`（EmitTail） | tail 整文件读入一块堆缓冲（≤512KB）再取尾部窗口 | 诊断命令低频 + 单文件分片上限 512KB 下内存峰值可控；malloc 失败 / 超上限退化为 NO_FILE / 零输出、帧仍完整 | 真机轮观察到堆压力或需要 tail 更大窗口 → 改分块流式 + 回溯定位 |
| `firmware/main/diagnostics/log_cmd_port.cc:45`（KIDPAL_LOGCMD_LOG_DIR） | 日志目录以设备路径字面 `/sdcard/logs` 固化，不经板级 config.h | 板级挂载根改动时此字面需同步（当前与 `sd_log_fs.cc` 的 `SD_MMC_MOUNT_ROOT`+"/logs" 形态一致） | 板级挂载根变更 → 引 config.h 单源或由 `KidpalSdLogAttach` 侧暴露目录访问器 |

- 说明：grep `minimal:` 本包新增面共 1 处（EmitTail，上表第一行）；无其它简化债务；无升级触发（no-trigger）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | `firmware/main/diagnostics/log_cmd_port.cc:174-179`（EmitTail） | 文件超过 512KB 上限时静默零输出（帧仍完整但 payload 空）；若未来分片参数变更使单文件超限，tail 会无提示地返回空 | changed-code hunk + heuristic：silent-degrade-on-limit；当前由 `KIDPAL_LOGCMD_TAIL_MAX_BYTES` 与 persist 分片 512KB 锁死，不可达 | **accepted-with-rationale**：真机轮 A7④ 观察 tail 输出完整性；如出现可在超限分支回错误帧替代零输出 |
| NIT | judgement-call | `firmware/main/diagnostics/log_cmd_port.cc:45` | `/sdcard/logs` 目录字面与 `sd_log_fs.cc` 的挂载根拼接逻辑存在两处来源（各自拼接），板级根变更时需两处同步 | changed-code hunk + heuristic：duplicated-path-construction；当前均由同一板级常量派生、字面一致 | **accepted-with-rationale**：已入 Simplification Debt 第二行；触发即收敛单源 |
| NIT | judgement-call | `tools/diag/kidpal_logs.py:113-114` | `port.dtr = False / rts = False` 在 `open()` 前赋值，依赖 pyserial 版本对打开前属性赋值的容忍（旧版偶见 SerialException 路径差异） | changed-code hunk + heuristic：driver-specific-attribute-ordering；IDF python env pyserial 实测支持 | **accepted-with-rationale**：真机轮 A7① smoke 即首验；如报错改用 `port.open()` 后立即拉低 + 复位补偿 |

> 无 BLOCKING finding、无 hard-violation；结构闸 PASS 不复述为 finding。**本包无未决 BLOCKING ⇒ 可进入 supervisor 机械闸**（final 相位 round 待 planner/verifier 消费 scope-final.md 后补闸面证据）。

## Five-Axis Review

1. **Correctness**：解析白名单严格小写 `kplog ` 前缀 + 单空格 token（空 token / 多 token / 非数字 / 越界全拒绝）；tail 钳制 1000、cat 闭集 0..31；行装配跨 feed 边界拼行、CRLF 剥 `\r`、超长行整行丢弃（report-once 标记）；帧字面精确；尾部窗口数行 + 定位边界（N≥行数整缓冲 / n=0 空窗口 / 空缓冲 0）；拒绝即短路零文件触达。以上由冻结验收 7 TEST + 9 变异双证。**未执行面**：真实 VFS / FATFS / USB-Serial/JTAG（见未覆盖面 ①–⑤）。
2. **Test quality**：1616 行冻结验收，weak 哨兵设计态红 + 强符号落地后覆盖（红灯先行的双向可满足性已由 acceptance-author 取证：/tmp 合成树 7/0 绿 + 9 处定向变异逐字红行）；结构齿扫注释剥离后可执行源（防注释假绿）+ anti-vacuity 登记守（M8：注销 CMakeLists 登记行即红）；负锚齐（超长行丢弃标记 / 钳制 / 闭集 / 字面 / 接线序 / 零 ESP_LOG / 登记行）。诚实边界 = LUR-A7 live-manual 无自动断言（structural 明示，SLP-A6 同款口径）。
3. **Contract/boundary safety**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` 零 diff；零 wire 形状改动（KPLOG 帧为本地串口诊断协议，生产端/消费端同包闭环）；`server/**` 零 diff；零新增依赖；分区表 / sdkconfig 零改动（LUR-X3 实核）；persist 写路径行为零改动（只增只读访问器，sd_log 8/8 实测绿）。
4. **Operability/security**：只读白名单命令面（list/tail/cat，无写删格式化、无任意路径读取、不上云）；不自动输出（无命令零输出，不干扰串口监控与烧录——A7②⑨ 待真机确认）；注入面拒绝即短路（路径字符 / 越界 / 超长全拒）；无卡只回 no-card 错误帧不崩（`KidpalSdLogCurrentIndex` 未 attach 返 -1 降级面）；隐私面 = 回取内容含 STT/LLM 原文，与 persist 落卡同族、human 已接受（TECH-261016 在案），本包不新增数据类、不出网。
5. **Maintainability/simplicity**：纯逻辑 / 芯片面按 story_cache 与 sd_log 双先例分离；冻结数值单源宏被 TEST 7 钉住；CAT_COUNT 引 `KIDPAL_SD_LOG_FILE_COUNT` 禁写字面 32；8.3 命名单源复用 `kidpal_sd_log_file_name`；代码注释逐处锚 [S1]-[S8] 与决策编号 D1-D10；`minimal:` 一处已入债务表。

## Residual Risk

- **`decisionCoverage` risk-accepted 条目**：无——本包 `landsIn: dropped` 的条目仅 D9（v1 明确边界：自动 dump / USB MSC / UART0 通道等不做），其 `reason` 以「v1 明确边界」起头，**不是 `risk-accepted`**，故本节无 risk-accepted 行可列。
- **Known limitation**：① LUR-A7 真机九项 + 降级链 A/B 未跑（本 Pod 零烧录，awaiting-human:real-device）；② 真实 /dev/secondary、FATFS 并发读、USB-Serial/JTAG 行为未在 host 复现（chip 面不可达）；③ tail 整文件读入的堆峰值与目录字面双源两项 NIT 已入债务表；④ `p1_auxbar` AB-A1/A2/A4 先存红仍在（与本包正交）；⑤ final 相位 verifier round 未跑（scoped r1 为当前唯一 verifier fact），LUR-X2 设备编译闸面证据待 final 轮补。
- **Follow-up recommended**：① human 持板轮按 LUR-A7 ①–⑨ 逐项观测并留痕，单命令 smoke 先行，失败进降级链；② planner/verifier 跑 final 相位 round（`--scope-file delivery/taskpackages/261018-sd-log-usb-read/scope-final.md`）补 firmware-build 真编译与 unit/acceptance 闸面证据；③ pod-retro 由 planner 收尾相位落 `pod-retro.md`。
- **Human decision needed**：LUR-A7 真机轮放行与九项验收结论（本 Pod 停门等 human 持板）；合并顺序按 OQ-1 主线程裁定执行（persist 验完合 main 后本包 rebase 立合）。

## 真机轮判定补记（2026-10-19 resume）

### LUR-A7 九项逐项判定

| # | 项目 | 判定 | 证据 / 说明 |
|---|---|---|---|
| 1 | 单命令 smoke（`kplog list` 经 kidpal_logs.py） | PASS | 启动日志 `c:/tmp/lur-boot.log`；命令响应帧完整、零 ESP_LOG 污染 |
| 2 | 无命令零输出（不干扰监控/烧录） | PASS | boot 日志静默面确认 |
| 3 | `kplog list` 列出分片文件与字节数 | PASS | 基线 21,939 字节序列起点 |
| 4 | `kplog cat 0` 回取 STT/LLM 原文 | PASS | `c:/tmp/lur-cat0.txt` 内容与落卡一致 |
| 5 | 写入增长后 cat 字节数同步增长 | PASS | 序列 21,939 → 22,246 → 34,467 |
| 6 | `kplog tail n` 尾部窗口与定位边界 | PASS | 窗口数行与边界行为符合验收 |
| 7 | 拔卡后错误帧（no-card 不崩） | NOT-EXECUTABLE | SD 卡物理难拔，本机不可执行；降级路径由 host 齿覆盖（无卡 `KidpalSdLogCurrentIndex` 返 -1 错误帧面已在 host 验收 TEST 覆盖） |
| 8 | 非法命令拒绝（越界/超长/坏 token） | PASS | 拒绝即短路，零文件触达 |
| 9 | 帧完整性（DTR/RTS 时序下无半帧） | PASS | 全轮未复现半帧/截断 |

证据文件：`c:/tmp/lur-boot.log`（boot + 命令响应全程日志）、`c:/tmp/lur-cat0.txt`（cat 0 回取原文快照）。

### 降级 A 过程事实

首版 /dev/secondary VFS 直读 smoke 失败（10s 零响应）——Kconfig 帮助文本「secondary 仅输出」的警告成立，VFS 读路径不可用。降级 A（HAL 直读 `usb_serial_jtag_ll_read_rxfifo`，commit ef09aa97）重测通过，确认为可用路径。此事实对后续涉及 USB-Serial/JTAG 读取的包有直接参考价值：该外设的 secondary 通道按 IDF Kconfig 语义仅输出，读侧必须走 HAL 直读。

### 结论

- LUR-X4 成立：真机轮由 human 持板执行并留痕（boot/cat 证据文件在案）。
- LUR-A7 判定 8/9 PASS + 1 NOT-EXECUTABLE（第 7 项拔卡，host 齿覆盖降级路径）。
- 人批引用：pod-events human-decision `056d1338-2dde-49b9-af52-5532fdc93f99`（payload.decision=approve-escalated-merge）。
