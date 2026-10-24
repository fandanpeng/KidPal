# docs-delta · 261024-memory-recall-tool-enable

> closeout 制品（blocking 必交）。生成：integrator，2026-10-24，final round 2 收拢相位。

## 结论：docs/** 零 diff 声明

TaskPackage `docsSyncRequired=false`（docsSyncReason：「本批 = 声明工件 + AGENTS.md 静态语义 + 验收锚换代;spec/契约面零 diff(检索优先属实现面能力放行,story 语义不变)。」），integrator 逐条核实成立：

1. **git 事实**：`git status --porcelain docs/` 为空、`git status --porcelain firmware/` 为空——工作树对 docs 面（含契约面 `docs/contracts/**`）与固件面零改动。本包实际 diff 面 = changeManifest 五件中的四件（`kidpal-agent-tools-declaration.json` / `apply_agent_tools.py` / `workspace-kidpal/AGENTS.md` / 冻结验收件 `test_261022_openclaw_kidpal_tools.acceptance.py`）；`rollback_agent_tools.py` 在 manifest 登记但零 diff（语义与项数无关，见 review-packet §4）。其余改动 = TaskPackage 目录制品与 `delivery/taskpackages/current/` 投影（verify.mjs 程序 fs-write）。
2. **spec 面口径已在现行段落**：`docs/requirements.md` v1.67 的 `S-F5.1` AC2（跨会话回忆）语义不变——本包不新增 spec 承诺，只是解除实现面的一层封锁（工具白名单四项→五项 + 回合纪律新增「记忆回忆回合」）；`docs/architecture.md` 与 `docs/contracts/interface-contract.md` 零触碰。`contract-lint` 闸的既有告警（`260923-arch-review-schedule-grid-cold-boot` 一条历史相对链接断链）为存量告警，与本包无关。
3. **AGENTS.md 属部署资产而非 spec 面**：`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` 是工作区提示词镜像（manifest 登记，dst = `/root/.openclaw/workspace-kidpal/AGENTS.md`），不是 spec / 契约文件。本次改动为加法：回合路由表新增「记忆回忆回合」一行，记忆节新增一段「记与查旧事是两件事」的检索澄清；既有记忆节原句「你不需要写任何文件、也不调用任何工具去记」逐字保留，261023 A3 的四条正锚与对象锁定行逐字不动（冻结验收件 `test_k1_e4_b_anchor_compat_no_regression` 与全量套件共同承载）。
4. **声明工件属部署资产而非契约**：`kidpal-agent-tools-declaration.json` 是无 secret 的部署声明（openclaw.json 含 gateway 凭据、整文件不入仓），不与 `docs/contracts/**` 的接口契约同面。allow 四项→五项是显式换代（`_note` 与验收件头注修订块双向留痕），不是静默放宽。

## spec 漂移检出

none——实现与 `docs/requirements.md` v1.67、`docs/architecture.md`、`docs/contracts/interface-contract.md` 现行文本无冲突；无需要交 architect 的 spec 面漂移。integrator 对两份 spec 文件无写权，本批亦无触发。

## 后续 docs / spec 触碰触发位（声明性注记，非本包动作）

- **运行时键形与契约假设不符时**：验收件头注「行为等价假设」已申报 `agents.entries.kidpal.tools.allow` ⇔ 运行时工具收窄的等价关系以部署轮实读为准；若实测不符，属验收判法升级，走 tighten-only 窗口经 planner 调整，不就地改 spec。
- **「检索优先」若成为正式记忆模型的长久措辞**：本包只落实现面能力放行；若 human 要求把它写进需求/架构正文（例如新增一条 AC 或改记忆模型描述），走 `/arch-change` loop，不在本包补写。
- **串包陈旧基线**：`261022-openclaw-honcho-plugin` 声明的 drift_guard 仍锚「四项已应用态」，`manifest.yaml` 第 251 / 313 / 317 行与 `apply_honcho_plugin.py` 头注仍写「四项」字面——这些是部署资产与部署单一事实源，不属 docs 面，也不在本包 changeManifest；建议 follow-up Lite 包换代（细节见 review-packet §4 第 5、6 条）。
- **孩子隐私红线**：本包零数据面动作（不重灌、不清毒、不改 Honcho 栈），无新增语料入仓面，docs 无隐私相关改动。
