# docs-delta · 261018-sd-log-persist

> integrator · 2026-10-18 · 分支 `dev/pod-m2`。本文件是 closeout 的 docs-sync 记账与承接声明：本包**零新增 canonical 文档改动**，下文逐面声明口径、证据与去向，不重复改写任何 canonical 文档。

## 1. 结论：本包零新增文档改动

本包 `docsSyncRequired = true`，`docsOwner = integrator`；按 TaskPackage `docsNoChangeReason` 与 reviewed plan 的既定口径，docs-sync 的落地方式 = **声明 + 核证**，不是再写一遍文档：

| 面 | 状态 | 落地位置 / 理由 |
|---|---|---|
| 架构面 T1–T8（决策 24 白名单增列「设备诊断日志」+ 六处镜像 + ADR-012） | 已先行落地（不在本包 diff） | commit `5a37b001`（261018 架构面 landing；landing-verdict GREEN）。本包只消费其形态边界：常开 + 有界轮转 + 16MB 封顶 + 可删除 + 含对话原文 |
| 隐私记账（STT/LLM 对话原文明文落可拔 SD 卡） | 已先行落地（不在本包 diff） | `docs/debt/tech/TECH-261016-sd-story-confidentiality.md` append，commit `27b4ecd5`（2 行） |
| 契约面 | 零改动 | `docs/contracts/**` 零 diff；FROZEN PROPOSAL 契约四分类 = `NO_CONTRACT_CHANGE` |
| 需求 / 架构 spec 面 | 零改动 | `docs/requirements.md` 零 diff；`docs/architecture.md` 自 landing 后零 diff（实测 `git diff 5a37b001..HEAD -- docs/` 仅 TECH append 2 行 + 本次无新增） |

**核证命令与结果**（2026-10-18，本 worktree 实跑）：

- `git diff main...HEAD -- 'firmware/partitions*' 'firmware/sdkconfig*'` → 空（SLP-X3 分区表与 sdkconfig 零改动成立）。
- `git diff 5a37b001..HEAD -- docs/` → 仅 `TECH-261016-sd-story-confidentiality.md` +2（已落地记账）；本包实现面零 docs diff。
- `docs-sync` 闸（final round 2）PASS：`docRefs` 七条全部在盘。

## 2. 跨包冻结齿重锚（唯一「冻结件被改」事项，证据链完整）

本包改动了一件**他包已合并的冻结验收**，属 TaskPackage 扩单事项，非 docs 改动；在此留完整授权链，供 review / supervisor / 事后审计一次性核对：

- **文件**：`firmware/main/test/sd_card_mount.acceptance.c`（261016-sd-card-mount 冻结件）。
- **改动**：SD-A2 tooth 6 期望字面 `max_files` 5→8 + 四处文字镜像（`:49/:94/:556/:559`）+ tooth 6 注释与红名文本 + 文件尾 REVISION LOG append（不覆盖 2026-10-16 既有两条）；**判别机制与其余断言零改动**（比较运算符、扫描窗、helper 语义、TEST_CASE 数与 acceptance id 全部未动；`git diff --numstat` = 44 / 7，删除行恰为上述 7 行）。
- **起因**：本包 plan 设计要点 4 要求 `mount_config.max_files = 8`（FD 预算给日志文件 + story_sd_io + 启动探针留头），与旧齿字面 5 互斥，host 全量因此新增 1 红。
- **授权链**：人 escalation `8c1db24e-a926-47ca-868a-35854500a40d`（planner，category=scope-or-permission）→ human-decision `88a08bbd-c542-41e5-bdc4-fb2739dc24ef`（2026-10-18，批准 A，窄口径：仅 tooth 6 期望值 5→8 + 四处文字镜像 + REVISION LOG，其余断言与 261016 收尾制品零改动）→ architect 1g 正式扩单回评（2026-10-18）→ planner 先扩 `changeManifest` 后动文件 → acceptance-author 执行。防先例语句已逐字写入 REVISION LOG。
- **双绿证据**（2026-10-18 本 worktree 实测，WSL host_app 全量 ELF 实跑）：
  - `sd_card_mount` 四 TEST_CASE 全 PASS（SD-A1/SD-A2/SD-A3/SD-A4；`:493/:560/:687/:766`）；
  - `sd_log` 八 TEST_CASE 全 PASS（SLP-A1×2 / A2 / A3 / A4×3 / A5）；
  - 全量 203 Tests 3 Failures，3 条全部为 `p1_auxbar` AB-A1/AB-A2/AB-A4 **先存红**（与本包正交，本包零新增红）。
- **边界**：本条是一次性窄口径授权，不构成「plan 提到某值即可改任何钉该值的冻结齿」的先例；261016 的 taskpackage / review-packet / mutation 记录保留原文不动（历史快照），现行期望以 live 冻结件 + 其 REVISION LOG 为准。

## 3. 本包零改动面（显式声明）

| 面 | 状态 | 理由 |
|---|---|---|
| `docs/contracts/**` | 零 diff | 纯板端本地诊断日志采集与落卡：零 wire 形状改动（帧类型 / 字段类型与承载 / 回执语义 / 序列化形态全不动），不新增端云消息、不做远程开关；契约四分类 = `NO_CONTRACT_CHANGE` |
| `docs/requirements.md` | 零 diff | 需求语义（产品是什么）不变；本包承载的是架构面已裁的「设备诊断日志 = 介质禁令白名单增列」实现 |
| `docs/architecture.md` | 零 diff（自 landing 起） | T1–T8 已随 `5a37b001` 落地；本包不重复改写 |
| `docs/ADR.md` | 零 diff（自 landing 起） | ADR-012 已随 `5a37b001` 落地 |
| `docs/debt/tech/**`（本轮） | 零 diff | 隐私记账已在 `27b4ecd5` append；本包不再改 debt（TaskPackage 注 ⑤） |
| `server/**` | 零 diff | 纯板端包；crossEnd = exempt 成立（`server/**` 零 diff 实测） |
| 分区表 / sdkconfig | 零 diff | 8.3 命名与 PSRAM 分配均不需改分区或 sdkconfig（SLP-X3） |
| `261016` 其他收尾制品 | 零 diff | 重锚仅触 `sd_card_mount.acceptance.c`；261016 的 taskpackage / review-packet / mutation 记录原文保留 |

## 4. 未随本包完成、需承接的项（不表述为已完成）

1. **SLP-A6 真机轮（live-manual）** —— human 持板四项观测（`/sdcard/logs/KPLOG*.TXT` 出现且含当轮 STT/LLM 原文片段；串口 COM5 输出不中断；写满 32×512KB 后最旧被覆盖且目录 ≤16MB；拔卡/坏卡不崩、30s 重试后恢复写盘）。本 Pod **零烧录、零真机**，host 面与设备编译面转绿后停门等 human。
2. **设备侧真实 FATFS / PSRAM / ISR 运行时行为** —— host 不可达（chip 面 `sd_log_fs.cc` 不进 host 构建），由设备编译闸 + 真机轮共同承载；host 侧只验纯逻辑 seam 与源码结构锚（见 review-packet 未覆盖面申报）。
3. **`p1_auxbar_style` 三条先存红（AB-A1/A2/A4）** —— 与本包正交的既有红，不在本包修复面；本包承诺的是「零新增红」，该承诺已实测成立。
