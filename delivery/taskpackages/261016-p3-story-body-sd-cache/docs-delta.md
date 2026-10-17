# docs-delta · 261016-p3-story-body-sd-cache

> integrator closeout 裁定 · 2026-10-17 · `docsSyncRequired=true`（taskpackage.json `docsSyncReason` 同源）· `adrRequired=false`（ADR-011 已随上游 261017 arch 批落地）· 契约面 / spec 面本包零改动。

## 1. `docs/debt/tech/TECH-261013-p3-reader-entry-no-text-silent-noop.md` —— 已结项（integrator 落盘 + 渲染器归位）

- **frontmatter**：`status: deferred → closed`、`closed: 2026-10-17`、`closedBy: 261016-p3-story-body-sd-cache`。
- **正文 append 结项行**（只追加、不改既有段）：三层缺陷逐条落点（层 1 可见降级 = SC-A5 / 层 2 按需取回通路 = SC-A1..A4 + SC-A7 + SC-A8 / ② id 错配 = SC-A6）+ 实测基线（host `192 Tests 3 Failures`〔上游已知红 AB-A1/A2/A4，新增红 0〕、server `181 passed / 4 deselected / 33 subtests`、final 相位圈定复跑 unit 86 passed + acceptance 51 passed、设备真编 PASS、变异 perAcceptance 全条 turnedRed 且还原复绿）+ 收口口径（自动化面全绿 = 代码面兑现；SC-A9 真机 live-manual 轮仍为本包收口前置）。末行 `consumed-by: 261016-p3-story-body-sd-cache`。**格式对齐先例** = `docs/debt/gov/_archive/2026/ARQ-261016-intake-entry-identity-unstated.md`（同类结项行 + consumed-by 末行）。
- **`node .claude/scripts/debt-render.mjs` 实跑**（exit 0）：条目按状态纯函数归位 `docs/debt/tech/TECH-261013-p3-reader-entry-no-text-silent-noop.md` → `docs/debt/tech/_archive/2026/TECH-261013-p3-reader-entry-no-text-silent-noop.md`；`docs/debt/tech/INDEX.md` 与 `docs/debt/tech/_archive/2026/INDEX.md` 由渲染器再生（**未手改任何 INDEX.md**）。gov / requirement 两域「未变」。

## 2. `docs/notes/board-hardware-esp32-s3-rlcd-4.2.md:62` —— **待主线程落笔（docs/notes，无 agent 写权）**

**本项不由 integrator 落笔**：scope-map 实证 integrator allow 不含 `docs/notes/**`（`allow` = firmware/server/docs/templates/docs/ARCH-REVIEW-QUEUE.md/docs/debt/docs/ADR.md/delivery/taskpackages/delivery/workstreams），仓内亦无其它 agent 的 allow 覆盖该路径（上包同款通道脱节，pod-events friction 在案）。故以「待主线程（human 主会话，无 agent_type）落笔」移交，exact text 存本节。

**为什么必须改**：该行现文写「业务数据不落板、决策 24 介质禁令不变」——本包起 story 正文按决策 24 **窄例外**落 `/sdcard/story/<id>.txt`（只读镜像），「不变」二字已成 stale 表述；不改则「文档写了真机不存在」的反向形态（真机存在、文档说没有）。

改前（当前 :62 逐字）：

```
**Micro SD 卡槽** 已随 261016-sd-card-mount 接入（SDMMC 1-bit 挂载 `/sdcard` 能力就绪，2026-10-16；业务数据不落板、决策 24 介质禁令不变）。art 资产用的是内部 Flash 的 SPIFFS art 分区，非 SD 卡。
```

改后（ready-to-apply 逐字，**保留原行首/行尾与 CRLF**）：

```
**Micro SD 卡槽** 已随 261016-sd-card-mount 接入（SDMMC 1-bit 挂载 `/sdcard` 能力就绪，2026-10-16）。art 资产用的是内部 Flash 的 SPIFFS art 分区，非 SD 卡。**SD 卡上只放 story 正文的只读镜像**（`/sdcard/story/<id>.txt`，随 261016-p3-story-body-sd-cache 接入，2026-10-17；决策 24 窄例外——云端 story 归档为唯一真相源、板端不承载任何写入，卡可换）；提醒 / 课表 / 日志等其余业务数据**仍不落板**。
```

**板级 SD 目录说明是否需要**：建议**并入本行**（上改后文本已覆盖目录落点、只读语义与边界），**不另立 docs/notes 新文件**——理由：信息量一句可承载，另立文件会与 board-hardware 盘点行双源；若 human 要求独立说明，再按 `docs/notes/` 正式文档补。本文件为 CRLF（保持原样，勿 EOL 归一）。

## 3. `docs/ui/UIUX-DESIGN-BRIEF.md` —— 已在 intake commit 落盘（planner），本次核对通过

- 本分支 diff = 撤除 v5 头注中的「随批 `261016-p3-story-body-sd-cache` 在途……未经 human 终拍、下游禁引……随批冻结后本行撤」两行（commit `c0f0297e`，planner 于 intake 时落笔）。
- **核对结论 = 一致**：该警示行的撤除条件（需求 frozen + human 2026-10-17 看图终拍）已满足；brief 现文 v5 头注保留冻结事实（v4→v5 对齐 requirements v1.64、WF-F2-06 唯一新增态、需求权威 = requirements.md）。无附加动作、无遗留在途警示。

## 4. 契约面 / spec 面 / ADR —— 零改动、零悬空句

- **契约面**：`git diff main...HEAD -- docs/contracts/` = **空**（零 diff 实证）。§四 `kidpal_story_fetch` 与 §1.3④ `story_text` 消费语义已随上游 `/arch-change` 批 LANDED；本包实现逐字消费、零新触点，无需 `architect-reviewer` 四分类、无 `/arch-change` loop 触发。
- **spec 面**（`docs/requirements.md` / `docs/architecture.md`）：上游 261017 批与需求 v1.64 已把事实态写在同批（arch §2.1 / §2.4 原则 1·5 / §3.1 / §4 / §5.3.1 / §5.4 / §6.2 / §10 决策 24 史实更正 + 窄例外 / §12 changeId 行；requirements §S-F2.1 AC2/AC7 + 补充规则 + §2.3 矩阵新行 + WF-F2-06 + NFR-OFF-1 邻位注记）。逐条核对**无「随实现落地翻既成」的悬空句**（无「尚未落地 / 未实现 / 待接入」类与本研究对象相关的注记）——本包**无 spec 面待改清单**，integrator 未写、不需派 architect（`ARQ-260929-docs-sync-spec-handoff` 触发条件未命中）。
- **ADR**：`adrRequired=false`；ADR-011 已在 `docs/ADR.md:94` 在册（决策 24 窄例外 + 契约首个设备发起请求-应答环），本包无新耐久决策、无需归档新 ADR。
- **唯一 stale 落点** = §2 的 `docs/notes` 盘点行（事实态盘点，非 spec 面），交主线程落笔。

## 附注

- 本包实现未引入新依赖（`server/requirements*.txt` / `firmware/dependencies.lock` / `idf_component.yml` 零 diff）、未新增出网面、无跨层调用、无绕过 contracts 的本地类型（纯逻辑层消费 `story_page.h` 在世类型、不另立镜像）。
- `firmware/sdkconfig` 实跑面变更 = `CONFIG_FATFS_LFN_NONE → CONFIG_FATFS_LFN_HEAP`（OQ-4 裁定）；构建再生附带 `CONFIG_FATFS_MAX_LFN=255` / `CONFIG_FATFS_API_ENCODING_ANSI_OEM=y` 与空段 `Image Effects Configuration`（IDF Kconfig 默认派生项，随 final 构建再生）已一并收口提交，避免常驻脏树。
