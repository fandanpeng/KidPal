# Docs Delta · 260920-m2-charwindow-display-fix

> integrator 收拢制品（2026-09-20）。本件回答「本包是否需要在 canonical docs / spec 面留痕」；结论与逐面核对如下。

## 结论

- **对 TaskPackage 声明的 `docsSyncRequired=false`：在本包责任范围内成立**——语义权威（`docs/requirements.md` S-F3.2 右栏五行 / S-F3.3 提示条 / 「播报什么显示什么」human 口径）现文即目标形状（中文释义 / 组词本就在线框里），本包是**实现向 spec 收敛**（遮挡修复 + 空行不显示 + join + 云面 meaning merge + 附录剥离），**需求面零改动、本包只消费**。
- **零契约 / 零 spec / 零 agent-gate 改动（MCD-E2 铁律守住）**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`.claude/**`（agent 定义 / gate）在本包全生命周期（launch 基线 `d174a05` → worktree）**逐面零触碰**（`git diff --numstat d174a05 -- docs/ .claude/` 零命中，见「核对方法」）。
- **云面改动全落 `server/deploy/**` 部署工件面，不触契约**：display 块 JSON 的 `meaning` 字段**既有**于 D3 契约（`docs/contracts/interface-contract.md` §1.3① char data 键集含 meaning——本轮是让云端把已播报的中文解释**填进既有字段**，播报内容零变化、零新增字段、零预造通用容器）。

## 声明理由（引用 taskpackage docsSyncReason 逐字）

> spec 侧 S-F3.2 与 WF-F3-01 线框现文即目标形状（中文释义/组词本就在线框里），本轮是实现向 spec 收敛——零 docs 面改动。

## 文档债（如实登记，不扩面、不在本包修）

prompt v5 bump（本包）使两处既有版本字面**进一步滞后**，均为既有漂移、按登记不修处理：

| 位置 | 现文 | 实况 | 处置 |
|---|---|---|---|
| `docs/architecture.md` §5.5.5（:296 等 3 处） | 「现状 = v4」 | bump 后 = v5 | **新增漂移（本包 bump 使 v4 字面过期）**——canonical spec 面非本包可改（spec 面零触碰铁律），交后续批随 spec 面更新落账 |
| `server/deploy/README.md` :44 | 「现状 = v3」 | bump 后 = v5 | 既有漂移（architecture.md 既有「已知漂移」行早已在案），交后续批 |
| `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` :141/:158 | 引 v3 prompt | bump 后 = v5 | 既有漂移（本包未触该文件），交后续部署批一并刷新 |

**为什么机器守卫不受字面漂移影响**：prompt 两面一致性守卫齿沿 ADR-001「解析当前版、禁版本字面断言」纪律（声明面 carrier / 渲染面 overlay / 三键 provider.config 三处互指解析），版本字面滞后不产生假绿/假红——云面 10 齿全绿实证（r3 `acceptance` 闸）。

## 逐面核对

| 面 | 文件 | 本包动作 | 核对证据 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md`（D3 char data 含 meaning / words 字段） | 零触碰（只读） | contract-lint 闸绿（r3，exit 0）；`git diff d174a05..worktree -- docs/contracts/` 零命中。meaning/words **既有**于 D3，本包零契约改动、零新增字段（MCD-E3：落法 B 不预造） |
| spec 面·需求 | `docs/requirements.md` | 零触碰（本包只消费） | docs-sync 闸绿（r3，`docsSyncRequired=false`）；`git diff d174a05..worktree -- docs/requirements.md` 零命中。S-F3.2 五行 / S-F3.3 提示条现文即目标形状 |
| spec 面·架构 | `docs/architecture.md` | 零触碰 | `git diff d174a05..worktree -- docs/architecture.md` 零命中。本包不触系统边界（char_page 单文件内容让位不动 chrome；网关 strip/merge 沿既有双出口注入点 additive）。**prompt 版本字面漂移见上「文档债」节** |
| docRefs | `docs/requirements.md` | 只读引用（S-F3.2 / F3） | 无需更新——需求行语义未变 |
| ADR | `docs/ADR.md` | 零触碰、无需新增 | 内容让位修法（不动 chrome）、meaning merge 单源同字、v5 bump 链均沿既有架构纪律（ADR-001 版本无关守卫 + 版本化 prompt bump 既有先例 v1→v2→v3→v4），无新系统边界 / 契约取舍。详见 Review Packet「Five-Axis Review」 |
| agent 定义 / gate | `.claude/**` | 零触碰 | `git diff d174a05..worktree -- .claude/` 零命中 |

## 核对方法

- `git diff --numstat d174a05 -- firmware/ server/ docs/ .claude/ delivery/workstreams/`：命中 = firmware 2 文件（char_page.cc 实现 + char_window_overlay.acceptance.c 冻结件，前序 commit `93bb20e`）+ server 6 文件（网关 2 + carrier/overlay/provider.config 3 接线 + 云面新验收件 1，另新文件 `openclaw-system-prompt.v5.md` 未跟踪不计入 numstat）+ progress.md +10（planner 立项登记）；`docs/**`、`.claude/` **零命中**（2026-09-20 integrator 收拢时实跑；`d174a05` = 本包 launch 基线，即真机轮记账 commit）。
- final r3（invocationId `fdf34129-032d-4e25-a5a5-d9165e6d7755`，`ok:true` / `observedGateExit:0`，2026-09-20）contract-lint / docs-sync / taskpackage-required 三闸 exit 0；`verifier-evidence-check.mjs` integrator 实跑 `factOk:true`、exit 0（r2 verifier 终审同样复验 PASS）。

## 实现侧注记承载位置（改动的 why 随代码注释承载，非 spec 面）

- **右栏让位 + 定宽换行 + 空行不显示 + 组词 join + UTF-8 封顶**：`firmware/main/display/char_page.cc`（cardlabel y8→30 / set_width 160 / LONG_WRAP；radical/words 判空条件拼装；words「、」join + 仅截断时 UTF-8 半字回退——不动 card_build 96B 字节截断冻结语义）。
- **网关附录剥离**：`server/deploy/openclaw-patch/make_openclaw_patch.py`（`k1cStripAppendix` + 流式/非流式两出口接线——只认真实开闭锚、有开无闭透传不误伤）。
- **meaning merge**：`make_openclaw_patch.py`（TS `k1cLookupBlock`）+ `char_stroke_lib.py`（python 镜像 `replace_minimal_block`，浅拷贝不毒化共享 cache）——双路径同一语义。
- **prompt v5 bump 链**：新文件 `openclaw-system-prompt.v5.md` + `OPENCLAW_SYSTEM_PROMPT_CARRIER`（声明面）+ `apply-xiaozhi-overlay.sh`（PROMPT_SRC/DST 渲染面）+ `provider.config.yaml`（artifact/source/deploy_target 三键）。
- **冻结验收指针收编**：`char_stroke_lib.py` docstring 指针改指现役件 `server/acceptance/test_char_meaning_merge.acceptance.py`（原指 `test_char_stroke.acceptance.py` 已归档）。
