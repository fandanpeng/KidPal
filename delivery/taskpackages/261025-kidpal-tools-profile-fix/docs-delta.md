# docs-delta · 261025-kidpal-tools-profile-fix

> closeout 制品（blocking 必交）。生成：integrator，2026-10-25，final round 2 收拢相位。

## 结论：docs/** 零 diff 声明

TaskPackage `docsSyncRequired=false`（docsSyncReason：「本批 = 云端部署声明工件 / apply-drift 脚本 / 验收锚换代；spec（requirements/architecture）与契约面零 diff，docs/ 树零改动，无文档面需同步。」），integrator 逐条核实成立：

1. **git 事实**：`git status --porcelain docs/` 为空、`git status --porcelain firmware/` 为空——工作树对 docs 面（含契约面 `docs/contracts/**`）与固件面零改动。本包实际 diff 面 = changeManifest 八件中的七件（`kidpal-agent-tools-declaration.json` / `apply_agent_tools.py` / `honcho-plugin-declaration.json` / `apply_honcho_plugin.py` / `manifest.yaml` / 冻结验收件 `test_261022_openclaw_kidpal_tools.acceptance.py` / 冻结验收件 `test_261022_memory_isolation.acceptance.py`）；`rollback_agent_tools.py` 在 manifest 登记但零 diff（备份整档还原机制与键数无关，核验零改申报，见 review-packet §1 / §4）。其余改动 = 本包 TaskPackage 目录制品与 `delivery/taskpackages/current/` 投影（verify.mjs 程序 fs-write）。
   - 附注：final 相位 `firmware-build` 闸真编译会由 CMake 重新生成 `firmware/sdkconfig`（新增「Image Effects Configuration」空块）——这是闸副作用而非本包改动，收口 commit 前已 `git checkout -- firmware/sdkconfig` 还原，firmware 面维持零 diff。
2. **spec 面口径已在现行段落**：`docs/requirements.md` 的 `S-F5.1`（AC2 跨会话回忆）语义不变——本包不新增 spec 承诺，只是把 live 已验证的「per-agent `tools.profile="full"` 覆盖全局 `coding` + `allow` 恰五项」形态固化进部署工件；`docs/architecture.md` 与 `docs/contracts/interface-contract.md` 零触碰。`contract-lint` 闸的既有告警（`260923-arch-review-schedule-grid-cold-boot` 一条历史相对链接断链）为存量告警，与本包无关。
3. **声明工件属部署资产而非契约**：`kidpal-agent-tools-declaration.json` 是无 secret 的部署声明（`openclaw.json` 含 gateway 凭据、整文件不入仓），不与 `docs/contracts/**` 的接口契约同面。写入面由单键 `change{path,allow}` 扩为双叶键 `changes[{path,value}]`，是显式换代（`_note` 与验收件头注双向留痕），不是静默放宽。
4. **验收件换代属冻结件内部留痕**：两份验收件的改动（K1-E4 / K1-E4-C 换代与新增、K3-E6 跨包锚显式换代）在各自头注留痕，属于验收面而非 spec / 契约面。

## 已知残留陈旧行申报（本包不改写，换代权威在别处）

- `docs/debt/tech/TECH-261023-openclaw-tool-face-prompt-only-readonly.md:27` 仍写「K3 `261022-openclaw-honcho-plugin/` 声明工件的 drift_guard 现状锚（钉死 P1 已应用态 tools.allow **四项**——先改工具面不改此锚 = apply exit 3）」。该口径自本包起过时：honcho 声明的 `expect` 已换代到**五项**。
  - 不改理由：该文件是 append-only 技术债账本条目（TECH-261023 的原始诊断文本），改写历史结论会破坏账本的可追溯性；账本退役 / 换代按自身生命周期走。
  - **换代权威**（两处，均为本包同批落盘的现行口径）：① `server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json` 的 `_note`（「drift_guard 现状锚 = P1 已应用态（agents.entries.kidpal.tools.allow 恰五项在位：read / write / exec / web_search / honcho_search_conclusions；2026-10-25 随 261025-kidpal-tools-profile-fix 的 P1 两叶写入面显式换代…顺序纪律：P1 tools applier 未跑的机器上 allow 项数/顺序不符即 drift，本包 apply 必须 exit 3，逼先跑 P1 applier」）；② `server/acceptance/test_261022_memory_isolation.acceptance.py` 头注「锚换代（261025-kidpal-tools-profile-fix）」段 + `K3_P1_TOOLS_EXPECT` 常量注释。
  - 同族历史措辞：同文件 `:20` 记录「live `tools.allow=["read","write","exec","web_search"]`」——那是该条目撰写时点的 live 实况描述（历史事实），同样 append-only 不改。
- 无其它 docs/** 陈旧行需要申报。

## spec 漂移检出

none——实现与 `docs/requirements.md`、`docs/architecture.md`、`docs/contracts/interface-contract.md` 现行文本无冲突；无需要交 architect 的 spec 面漂移。integrator 对两份 spec 文件无写权，本批亦无触发。

## 后续 docs / spec 触碰触发位（声明性注记，非本包动作）

- **live 面收窄的记账位**：本包 live 面只有「`--gen-patch` 产物对 live 做一次 `openclaw config patch --dry-run --json`」一项（零写零重启），归 human 部署轮；若 dry-run 结果与仓内假设不符，属部署面事实校正，不动 docs。
- **`tools.profile="full"` 若要写进架构正文**：本包只落实现面工件；若 human 要求把「per-agent profile 覆盖全局 profile」写进架构文档（例如工具过滤链口径），走 `/arch-change` loop，不在本包补写。
- **TECH-261023 账本条目的换代**：`:20` / `:27` 的「四项」字面属历史记录；若后续批次给该账本条目补换代注记或退役条目，按账本自身纪律处理，不在本包内。
- **孩子隐私红线**：本包零数据面动作（不动 Honcho 栈 / backfill / 库写），无新增语料入仓面，docs 无隐私相关改动。
