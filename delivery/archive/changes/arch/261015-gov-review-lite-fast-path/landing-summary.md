# Landing Summary · 261015-gov-review-lite-fast-path

> 2026-10-15 · 主线程 packet-bound 落地。packet-id = FROZEN v0.3 PROPOSAL（commit 8d130a6）§2 改法集 T1–T6 + §5 验证集。
> 落地范围 = landing-manifest 17 件（全「改」、零新增零删除）；契约面零触；scope-map.json / settings 零触。

## 逐文件落地记录（17 件 ≡ manifest，顺序按 manifest）

| # | 文件 | 工作项 | 落了什么 |
|---|---|---|---|
| 1 | `.claude/agents/acceptance-author.md` | T4 | 输入面引用改单份 `input-brief.md`（:24 输入清单 + :30 步骤 1 + :48 第六条）；story 投影描述改「proposal/story 照录、bug 类指针形态」 |
| 2 | `.claude/agents/builder.md` | T4 | 同上（:24 输入清单 + :46 第六条）；§①c 主要消费方标注 + bug 类指令源半句 |
| 3 | `.claude/agents/planner.md` | T2+T4 | Step 1② rigor 感知句（notExecuted 设计态 + 中段设备面需求停）；Step 1e :47 同源限定半句；Step 5 ③ brief 单份投影；Step 7 双份改单份；Step 8 硬前置改单份在场；Step 9 引用单份；JOURNAL-APPEND over-scan 单份引用 |
| 4 | `.claude/hooks/enforce-scope.mjs` | T5 | `locateGitTree`（.git 文件/目录纯 fs 定位，不 spawn git）+ `samePath` + `anchorForTarget`（同 common dir 在册兄弟树重锚、该树 scope-map 判定、缺失 fail-closed）；Bash 与 Edit/Write 两分支接入重锚；新建 self-check（假树六臂：夹具非空 / 兄弟树工作面放行 / 兄弟树红线拦截〔反向洞〕/ 仓外 agent 拦 / 仓外主线程放行 / 本树不变） |
| 5 | `.claude/hooks/supervisor-verdict-gate.mjs` | T4 | 三处注释补更名注记（旧名保留为合成回归锚、agent 名存续保护判据不变）；self-check 增一臂 `fp-path-input-brief-single-name-not-hit`（新名不触发）；判定逻辑零改动 |
| 6 | `.claude/hooks/verify.mjs` | T3 | `shouldDeferGate` 纯函数（final 硬判双保险）+ `readCurrentPackageRigor`（fail-open）+ runGateChild env 透传（`VERIFY_GATE_DEFER` + `_REASON`，final 永不注入）；self-check 增五纯判定臂 + 接缝臂（`defer-seam-executor-skipped-lifted`） |
| 7 | `.claude/references/change-triage.md` | T1 | §2 四档表 Lite 行补「⇒ 快路径默认适用（§7）」；§7 增「lite 快路径」段（判注义务 + 复议触发器） |
| 8 | `.claude/references/dev-loop-operating-system.md` | T4 | over-scan 触发定义 `input-brief-<role>.md` → `input-brief.md` |
| 9 | `.claude/references/dev-loop.md` | T3+T4 | §2 brief-inputs 脚本表行单份化；§3 变更制品 brief 条目单份化；§5 闸表 firmware-build 行补 lite scoped 后置注记 |
| 10 | `.claude/references/pod-coordination.md` | T3+T6 | §6 冒烟注记一句同步（lite 包后置 final）；增 §2d「叫停协议」节（顺序条款 + 树处置两形态〔R1-F2〕+ 三件齐） |
| 11 | `.claude/references/task-intake-and-slicing.md` | T1 | 「rigor 字段与 Lite 字段级定义」节补快路径交叉引用句（定义本体一字未动） |
| 12 | `.claude/scripts/brief-inputs.mjs` | T4 | renderStoryRoad 基 id 去重（AC 标签并列）+ bug 类降级（`BUG_SOURCE_TYPES` ∧ planRef → 指针形态）；renderBrief 去 role（头部单份化 + ①c 无条件渲染标主要消费方 + 头部改动指令源行）；`--role` 退役（unknown argument fail-loud）；输出 `input-brief.md`；self-check 43/43（含基 id 去重 / bug 降级 / proposal 负向 / 单份头部新臂） |
| 13 | `.claude/scripts/firmware-build-check.mjs` | T3 | decide() deferred 臂（`VERIFY_GATE_DEFER=1` 优先于一切含严格模式，`phase:"executor-skipped"` + reason 点名，短路先于 probeToolchain）；main 入口 defer 短路；self-check 增 deferred 臂 + defer>required 优先级臂 |
| 14 | `.claude/scripts/taskpackage-required-check.mjs` | T4 | finalize 态「两份 input-brief-<role>.md」→「单份 input-brief.md」（:12-15 头注 + :545-553 判定 + 报错文案） |
| 15 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | T2+T6 | 产物清单第 4 件 lite 限定词（R1-F3）；增 §3b lite 快路径（池优先 + 新树免做唯一件 = 环境等价化后置 + 图索引必办不变 + 执行顺序注记）；步骤 3 交接卫生补 host_app/sdkconfig 换包即删（TECH-261014）；步骤 4 镜像清单增 host_app sdkconfig 行 + lite 后置入口句；步骤 7 启动卡五要素改六要素（会话标识 + lite 真机轮预检行 + defer>required 注记）；坑节增两条（树内干活纪律 + 叫停顺序，与 pod-coordination 互指） |
| 16 | `.claude/verify-gates.json` | T3 | 顶层 _doc 增 defer 段 + 冒烟注记口径同步；firmware-build 条目增 `"defer": { whenRigor:["lite"], phases:["scoped"] }` + 条目 _doc |
| 17 | `docs/DEV-GOVERNANCE.md` | T4（G1，OQ-2 已裁） | 第六条①「一次产双份（两个文件名）」改「一次产单份 `input-brief.md`」一句级 |

## 验证集执行结果（§5 V1–V5）

- **V1 六脚本自检**：brief-inputs 43/43 / verify 16/16 / firmware-build-check 8/8 / taskpackage-required-check 18/18 / supervisor-verdict-gate 74/74 / enforce-scope 6/6——全 exit 0；新臂名单逐字在输出（brief 4 新臂、verify 5 defer 臂 + 接缝臂、enforce-scope 5 功能臂）。
- **V2 defer 入口 hermetic 冒烟**：`VERIFY_GATE_DEFER=1 node .claude/scripts/firmware-build-check.mjs` → exit 0，stdout 含 `"phase": "executor-skipped"` + reason 点名 rigor=lite scoped 后置；未触发工具链探测。
- **V3 fail-open 回归**：verify.mjs self-check「包上下文缺席 → 不 defer」「闸无 defer 字段 → 不 defer」纯判定臂承载（V1 内绿）；现状包 scoped 真跑验证按 §5 留根改下一非 lite 包首跑（在飞 lite 包命中 defer 属预期，见 §5 V3 原文注记）。
- **V4 authoring 守卫**：lint-descriptions scanned 16 / flagged 0；route-regression-check --self-check 104 ok / 0 fail。
- **V5 preflight**：`landing-fidelity-check.mjs --preflight` → ok:true、findings []、advisories []、anchorUnion 17 ≡ landing-manifest。

## 条件触发验证（留根，§5 V6/V7 + §6）

- V6（下一 lite 包首跑全链冒烟）、V7（下一 worktree 包跨树实测）无可跑窗口，由该包 pod-retro 回填。
- 图索引建库健康度留根：任一包实测 >~10 分钟 → tech 域债条目（OQ-3 裁定口径）。
- pod-stop.mjs 自动化：OQ-1 裁 (a)，procedural 先行。

## 关联账本动作（落地后由主线程/渲染器执行）

- `ARQ-261015-lite-package-execution-overhead` / `ARQ-261015-launch-prep-host-only-env-defer`：随批闭合，状态翻转 + debt-render 重渲染。
- `TECH-261014-host-app-sdkconfig-env-drift`：修法方向①落地（T2 镜像行 + 交接卫生），随批结项。
- changes-index：draft → frozen → landed 投影刷新。
