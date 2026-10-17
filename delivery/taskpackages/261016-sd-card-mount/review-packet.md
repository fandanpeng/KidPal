# Review Packet · 261016-sd-card-mount

> integrator · 2026-10-16 · rigor=full / riskTier=medium / sourceType=human-ad-hoc · 分支 `dev/261016-sd-card-mount` · **等待 supervisor 裁断（不自行判定通过、不 push、不 merge）**

## Change Size

- **Files changed**：实现面 4 件 —— `firmware/main/boards/waveshare-s3-rlcd-4.2/config.h`（+10/-1）、`firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+148/-0）、`firmware/main/test/host_app/main/CMakeLists.txt`（+6/-0）、`firmware/main/test/sd_card_mount.acceptance.c`（+828/-0，新建冻结验收件）；doc 面 1 件 —— `docs/architecture.md`（+1/-1，单行盘点）。另有 closeout / delivery 簿记制品（本 review-packet、docs-delta、device-round-record、device-round-sd-a5-boot.log、test-scope-final.txt、verifier-evidence r5/r6/r7 等，不入实现面计数）。
- **Net diff**：实现面 + doc 面净 **+993/-2（5 文件）**；对比基准 = `main...HEAD`（merge-base `bd4beac`，**非**本地 `main` 指针 `a3a182d`——上游 gov 批已把 main 推进数提交，本包与那些提交正交，`git diff main` 会误显其反向 diff，已如实甄别）。
- **Scope**：板级 SDMMC 1-bit 挂载能力就绪 —— config.h 四引脚/根宏 + `.cc` `InitializeSdCard()` / `RunBootRoundtripProbe()` / `SdCardMounted()` / `s_sd_mounted` + 新建冻结验收件 + host 接线一行 + architecture.md 盘点行。**零 server diff、零契约 diff、零依赖新增、零跨层调用**。UIUX Level 0（无用户可见行为变更）。
- **Split recommendation**：**不拆** —— 单包已由 human 收窄（只做挂载能力就绪；story 存储 / 日志落盘明确不在本包，human 原话钉死）。

## Verification Evidence

- **Structural gates**：`firmware-build` 真编译 `build-pass`（xiaozhi.bin `0x31e9f0` = 3275248 字节，最小 app 分区余 9%；**本次 final 无 sdkconfig 再生噪音 diff**，地雷①未触发）；`contract-lint` PASS（warning = 一条上游断链 `../../delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md`，非本包引入）。**final 相位 10 闸全绿（r7）**，`observedGateExit=0`、fact `ok:true`。
- **Acceptance / unit**：`acceptance` 闸 `oracle-pass`（scope 内 `firmwareAcceptanceFiles=1` = 本包冻结件、`serverAcceptanceFiles=0`——非静默跳过）；`unit` 闸 scope 内 0 条可跑（`executor-skipped` 诚实绿跳过，本包无 unit 件）。真机轮 = `device-round-record.md`（SD-A5 有卡轮真机 PASS + SD-A6 无卡轮本机不可执行豁免 + 结构性证据替代）。
- **Security gate**：`securityReviewRequired=false`（无 secret / 权限 / 输入面）；`security` 闸 PASS。
- **Mutation check**：m1–m4 全数 `turnedRed=true`（① 引脚宏回退/删除/注释抹除 ② 挂载参数回退 `format_if_mount_failed→true` / 宽度 1→4 / max_files 5→12 / 构造窗调用点删除 / 引脚宏消费移出 ③ 降级路径破坏 `WITHOUT_ABORT→`裸 `ESP_ERROR_CHECK` / 置标志与 `print_info` 删除 ④ accessor 删除或常量 stub），靶位全对、还原复绿（终态 4T/0F）；证据 `mutation-evidence/builder-run-2026-10-16.md`；`mutation-check` 闸 PASS。
- **Docs sync**：`docs-sync` 闸 PASS；`docs/architecture.md` 已落盘（architect 代写）integrator 抽查一致；`docs/notes:61-62` **待主线程落笔**（无 agent 写权，见 docs-delta.md §2 与下方「未覆盖面申报」）。

## test-scope 清单（final 相位范围圈定）

- **落盘路径**：`delivery/taskpackages/261016-sd-card-mount/test-scope-final.txt`（`verify.mjs --phase=final --scope-file` 消费，round 7）。
- **内容摘要**：`detect_changes`（per-Pod 专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 可用，base=main，merge_base `bd4beac`，inbound depth=2）= **33 seed symbols / 1 impacted**（`firmware/main/test/sd_card_mount.acceptance.c` 模块，hop 1；零跨模块越界、零 server 面）∪ `impactedTestsSeed` 两件（新冻结验收件 `new-frozen-acceptance-file` + `CMakeLists.txt` `wiring-add`）∪ blast radius 映射测试（同上游件）。`trace_path` 抽查：`InitializeSdCard` inbound callers_total=1（`CustomBoard` 构造函数，同文件同模块）——无跨子系统、无 wire 面调用边，与 `crossEnd.exempt` 一致。
- **floor**：本包无对端包 → floor = 本包 `acceptanceTestsFile` ∪ blast radius = `sd_card_mount.acceptance.c`（已在清单内）。**圈定降级 = 无**（正常圈定路径，非降级）。共享 seam = 无（板级 .cc / config.h 不入 host 编译面，host 面唯一改动 = 新验收件接线一行）。

## demo 对账行（真机轮 demo 证据 ⟷ 本包效果段一致性命结论）

- **demo 证据** = 真机轮串口日志 `delivery/taskpackages/261016-sd-card-mount/device-round-sd-a5-boot.log`（249 行 / 14653 字节，原样字节拷贝自 `C:/tmp/sd-boot-a5b.log`）。
- **本包效果段**（plan §建什么）：「卡在位时 `/sdcard` 可读写；启动期 SDMMC 1-bit 挂载，成功置标志；卡不在位 / 坏卡 / 挂载失败 → 降级标志 + WARN、继续跑，不 panic、不碰语音主链路」。
- **一致性结论 = 一致**：有卡正向路径逐项兑现 —— 卡信息打印（L75-79 `Name: asdfg / Type: SDHC` + `CSD: ver=2 ... capacity=30535680`）、挂载成功（L81 `SD card mounted at /sdcard (SDMMC 1-bit)`）、1-bit 兑现（L80 `SSR: bus_width=1` + config.h `slot.width=1`）、`/sdcard` 可读写（L82 `SD ROUNDTRIP PASS ... bytes=63`；写-读-比对-删全链通过）、不碰语音主链路（L182 `Network connected` → L190 `activating -> idle` → L228 `WS: Session ID` 语音链路正常起来）。降级路径本机不可真机观察（无卡态不可达），由代码面静态结构断言（SD-A3 承载）作替代证据，**不冒充真机观察**。demo 证据与 plan 效果段无缺口、无超范围。

## 未覆盖面申报（对照 `acceptance[]` + 契约条目 + plan 效果段，逐条一行，零缺口明写 none）

- ① **SD-A6 真机无卡态本机不可达** —— 本次无验收断言（真机观察面）；现状 = 结构性证据替代（SD-A3 失败降级路径结构齿 + SD-A4 查询入口观察锚，代码面静态断言）；去向 = **已豁免留痕**（human-decision `416e5c42`），不落 C 齿、不表述为真机已验证。
- ② **原 SD-A6 观察项③「KEY 发起会话冒烟（语音主链路不受影响）」** —— 本次无验收断言；现状 = **未验证**（本机不可得、无结构齿承载，属真实覆盖面差集）；去向 = **如实列差集、不粉饰**（登记于 `device-round-record.md §3` 与本节）。旁证：有卡轮日志 L182→L228 语音链路正常起来，但那是**有卡**路径，不等价于无卡降级路径下语音不受影响。
- ③ **`docs/notes/board-hardware-esp32-s3-rlcd-4.2.md:61-62`** —— 本次无即时断言；现状 = **待主线程落笔**（无任何 agent 写权，friction `26636485`）；去向 = 主线程（human 主会话，无 agent_type）按 docs-delta.md §2 ready-to-apply 文本落笔后闭合（exitCriteria 未闭项）。
- ④ **原理图三根脚目视核对**（原 SD-A5 观察项③） —— 本次无目视核对断言；现状 = human 未单独执行「config.h 宏取值 + 真机挂载成功」实证反证；去向 = 已在 `device-round-record.md §4` 如实声明非目视核对。
- ⑤ 契约条目（`docs/contracts/interface-contract.md`） —— 本包**零契约暴露面**（NO_CONTRACT_CHANGE，plan 四面探测实证），无契约断言需求。
- ⑥ 其它 acceptance 条目（SD-A1/A2/A3/A4 + SD-A3-GATE） —— 全部有断言且绿（源锚结构齿 + 设备编译闸），**无缺口**。
- （零缺口项明写）**none** —— 除上述 ①–④ 外，`acceptance[]` 与 plan 效果段无未覆盖条目。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包 diff 与新建冻结件扫描零新增 `minimal:` 标记） | — | — | no-trigger |

> 邻域注记：`waveshare-s3-rlcd-4.2.cc:447` 一处既有 `minimal:`（PCF85063 无年字段、`year_ok` 入参用 RTC 日期字段 plausibility 代理）**不在本包 diff hunk 内**（上游 K5 RTC 代码既有标记），非本包债务、不入本表。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| （无 BLOCKING） | — | — | — | — | — |
| NIT | judgement-call | firmware/main/test/sd_card_mount.acceptance.c | 冻结件 SD-A5/SD-A6 措辞修订**仅注释面**（C 齿逻辑零改动），SD-A6 替代证据的「结构性担保」语义承载在 SD-A3/SD-A4 两齿上——注释面修订与断言面解耦，读者须读 coverage 行才知映射 | changed-code hunk：`sd_card_mount.acceptance.c` [5] 节 + coverage map SD-A5/SD-A6 行；heuristic: 冻结件措辞与断言面解耦属可接受形态（human 裁定留痕 `416e5c42`），但需在 review-packet 点明 | 无需动作（映射与差集已在 [5] 节 + coverage 行 + `device-round-record.md §3` 三处显式声明） |
| NIT | judgement-call | docs/notes | `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md:61-62` 盘点行 stale（KEY 键「未接入」随 260911-key-route-a1 变 stale、SD 卡槽「未接入」随本包变 stale），待主线程落笔 | hard-rule: scope-map 无任何 agent allow 覆盖 `docs/notes/**`（本 Pod 实测 friction `26636485`）；改动为事实态盘点同步 | 主线程（human 主会话）按 docs-delta.md §2 ready-to-apply 逐字文本落笔 |
| FYI | judgement-call | firmware/main/test/p1_auxbar_style.acceptance.c | 既有 host 套件 3 红（AB-A1/A2/A4）在**上游包 261015-p1-auxbar-style-fix 冻结件**上，**非本包引入** | taskpackage.json `redBaselineResult` 实证：3 红在净 HEAD 即为同值，全部位于 `p1_auxbar_style.acceptance.c`，归因 = `home_p1.c` 在该包冻结后继续演进（battery 已切 `ic_batt_20`、`ic_wifi_14.bin` 已停止消费、temp x0=218 越授权域），属上游包冻结件与落地态漂移 | 归上游包 owner 的 tighten-only 窗口（已留话）；**勿记本包头上**（本包 `sd_card_mount.acceptance.c` 四齿全绿） |

> 无 `hard-violation`。结构闸（firmware-build / contract-lint）PASS 不复述为 finding（归 verifier evidence）。

## Five-Axis Review

1. **Correctness**：本包无 `S-Fx.y` 回引（sourceType=human-ad-hoc，行为权威 = reviewed plan.md）——对照 plan 效果段四件逐项兑现：卡在位 `/sdcard` 可读写（真机 SD-A5 roundtrip PASS + SD-A1/A2 源锚齿）、降级不崩（SD-A3 `WITHOUT_ABORT` + panic 形态计数=0 负向齿）、标志供查询（SD-A4 `SdCardMounted` 读 `s_sd_mounted`）、不碰语音主链路（真机 L182→L228 语音链路正常）。1-bit 义务双证（源码 `slot.width = 1` 字面 + 真机 L80 `SSR: bus_width=1`）。边界条件齐（返回码 + 卡句柄判空双检；`format_if_mount_failed=false` 坏卡不销毁数据）。
2. **Test quality**：mutation m1–m4 四族 `turnedRed` + 还原复绿 = 齿灵敏度闭环；源锚齿读**可执行源字面**（注释剥离后断言，非注释假绿）；负向齿咬合力自证（panic 形态计数=0、`format_if_mount_failed=true` 计数=0）。真机轮 = live-manual 一等豁免（SD-A5 PASS / SD-A6 豁免 + 结构替代，明确不冒充真机观察）。
3. **Contract/boundary safety**：`docs/contracts/**` 全程只读、零 diff（NO_CONTRACT_CHANGE）；wire 形状零触（`crossEnd.exempt` 实证）；无新依赖、无跨层调用（板级 .cc 调 ESP-IDF 同层 API）、无绕过 contracts 的本地类型。
4. **Operability/security**：降级不崩（不 panic、不阻断启动）、`ESP_LOGW` WARN 继续不静默（可运维）；探针 `corr_id=kdp-...` 进日志（可观测，与既有 `kdp-` 时间戳口径同源）；无 secret / 权限 / 输入面（`securityReviewRequired=false`）；**决策 24 介质禁令遵守**（临时探针 8.3 名 `SDTEST.TXT`、验后即删不驻留，不写业务/身份/事项状态数据）。
5. **Maintainability/simplicity**：改动局部（单编译单元内 + 冻结件 + 接线一行）；对偶 `InitializeArt` 既有范式（`format_if_mount_failed=false` 同构）；无过早抽象（零消费方 `SdCardMounted()` 为**防死代码误删观察锚**，human 裁定 + mutation m4 定向取证定义，非投机灵活性；`deletion test` 判据：删之则「能力就绪」无查询缝、复杂度回流调用方 —— 深模组信号，留）；`RunBootRoundtripProbe` = human 裁定（`db8b1406`）的最小实现测试义务，非业务消费。

## Residual Risk

- **Known limitation**：① SD-A6 真机无卡态本机不可达（结构替代，不冒充真机观察）；② 原 SD-A6 观察项③「KEY 会话冒烟在无卡降级路径下不受影响」本机不可得、无结构齿承载（真实差集）；③ `docs/notes:61-62` 待主线程落笔（exitCriteria 未闭项）；④ 原理图三根脚目视核对未单独执行（实证反证形态）。
- **Follow-up recommended**：上游包 261015-p1-auxbar-style-fix 的 3 红归其 owner tighten-only 窗口；`docs/notes` 行待主线程闭合。
- **Human decision needed**：**无新增**（SD-A5 / SD-A6 裁定已由 human 下达，`0263a7a3` / `416e5c42`）。
- **decisionCoverage 裁决遗留**：本包 `taskpackage.json` **无 `decisionCoverage` 字段**（非 proposal 来源）——无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目，本节按空处理（零 `risk-accepted` 条目）。

## 集成执行回报

- `detect_changes` 已跑（per-Pod 专属图索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 可用，无降级）；`trace_path` 抽查同一结论。
- 本树**无跨包冲突、无 rebase、无 Merge-Chain 触发**（单包、无对端、worktree 常驻分支 `dev/261016-sd-card-mount`）；无 `supervisor-verdict.json` 等机械闸 fs-write 产物待收口（supervisor 门由 planner 侧机械闸跑）。
- `.codebase-memory/artifact.json` 与 `graph.db.zst` 常驻脏不入 commit（AGENTS.md 纪律）。承重机器件 = final verifier-round fact `verifier-evidence/rounds/r7.json`（`ok:true`，`verifier-evidence-check` PASS）；`verdict.json` / `verification.json` 按 closeout-hardening SSOT 为**投影·advisory 且永不晋升包目录**（changes-index.mjs :160 实证），`current/verdict.json` 仅 non-bearing compatibility projection——closeout 判断来自 validated final fact，非该投影。
