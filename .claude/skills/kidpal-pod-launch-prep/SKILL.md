---
name: kidpal-pod-launch-prep
description: Prepare the launch surface for the next batch of kidpal-mvp TaskPackages (dev-loop Pods) — scheduling call from board state + dependsOn, check-launch gate, worktree/branch creation, firmware env equivalence, per-worktree CodebaseMemory graph index, board prefill, and launch prompt authoring. Use when a batch of Pods (e.g. after a merge to main) is ready to have its dev environments prepared; does not launch, merge, or push.
---

# KidPal Pod Launch Prep（task package 开发环境准备）

把「下一批 TaskPackage 的 launch 面」从调度判定一路备到 human 可自跑 launch 命令。适用于 kidpal-mvp workstream 多 Pod 模式（PROPOSAL FROZEN v0.4，15 包，WIP=3）；单 Pod 形态是子集。

> **执行主体 = 主线程（human 会话）**；本手册 = [`docs/DEV-GOVERNANCE.md`](../../../docs/DEV-GOVERNANCE.md) §4 Dev Loop 图的 **② launch 面准备** 阶段操作手册（260921-gov-review-launch-pipeline T1 立此坐标）。对偶触发源：`/build-feature` 的 launch 面预检（changeId 直启形状核 board 条目 / 启动卡 / 运行时 worktree / 专属图索引四件，任一缺即路由到本手册补齐再回 intake，260924-gov-review-planner-orchestration-align T1）。

## 产物（完成标准）

1. 调度判定记录（哪几包、为什么）→ 落 `delivery/workstreams/kidpal-mvp/progress.md` 追加段
2. 每个包：`--check-launch` 机械门绿
3. 每个包：运行时 worktree（**复用空闲树 ∨ 按需新建**，一树一常驻分支——见步骤 3 换包协议）
4. 每个包：固件环境等价化（sdkconfig / dependencies.lock / host_app sdkconfig 镜像 + managed_components 共享软链 + bootstrap 跑过；**lite/nano 包新树：后置真机轮前**，见步骤 3b——261015-gov-review-lite-fast-path）
5. 每个包：专属图索引 `C-AI-KidPal-pod-<suffix>`（full + persistence）——**launch 面必办，不得降级读文件**（ARQ-260905 规则钉死）
6. board（`devloop/pods/<changeId>.json`）预填 worktreePath
7. 启动卡落 `delivery/taskpackages/<changeId>/launch-card.md`
8. **不代跑 launch、不合并、不 push**——launch 命令交 human 自跑

## 步骤

### 0. 读地面真值（不照抄旧注记）

- board：`cat $(git rev-parse --git-common-dir)/devloop/pods/*.json`（linked worktree 下 common dir 是绝对路径；主 worktree 是相对 `.git`，脚本已处理，手动读时先 `cd` 主 checkout）。
- 既有 worktree：`git worktree list`；mergeToken holder。
- 每包 `dependsOn` / `mergeOrder` / `riskTier`：`delivery/taskpackages/<id>/taskpackage.json`。
- 调度规则权威：`delivery/workstreams/kidpal-mvp/PROPOSAL.md` §3（关键路径 + 并行调度 + WIP=3）。

### 1. 调度判定

- 规则 = dependsOn 全部 merged（或 PROPOSAL §3 明示的并行放宽条款）∧ mergeOrder 升序 ∧ WIP 余量。
- **依赖图说了算，不硬凑并行**：WIP=3 但依赖未就绪时本批就是单包（先例：2026-09-06 WS merged 后，全 queued 包 dependsOn 都含 voice-roundtrip → 本批仅包 2）。判定写在 progress.md，不私下扩批。

### 2. launch 机械门（每包）

```
node .claude/scripts/portfolio-reconcile.mjs --check-launch <changeId> --base-ref <专用 base 分支>
```

（`<专用 base 分支>` = 本 workstream 拆包产物所在基准 ref，如 `base/m1-home-qa`——F-05 裁定专用 base ref 非 main，pod-coordination §2b 同口径。）

blocking 非空即停——回 portfolio-planner 补 commit，不绕过。

**settings 合法性机检（260924-gov-review-settings-deny-contract-merge T2）**：`node .claude/scripts/settings-lint.mjs`——扫 settings 族 permissions 规则的已知无效形态（`Write(path)` 文件权限规则被现行 CLI 判非法：零执行力且硬阻塞 headless launch exit 4，ARQ-260924 实证）。RED 即停、先修 settings 再走后续步骤（判停口径与 check-launch blocking 同）；CLI auto-update 常开，本机检是版本漂移的 launch 面前置防线。本机检在 launch 前静态校验 settings 文件内容，不依赖 CLI 各 launch 形态的行为差异，启动卡命令行①②两形态均覆盖（CLI 侧报错打印发生在 settings 加载期——2026-09-24 冒烟实证于 `-p` 无旗标与非 TTY 两形态；真交互 TTY 形态未测，见 ARQ-260924 追注）。

**「上游已知红」计数（260909-gov-review-gate-truthfulness T5 + 260915-gov-review-worktree-pool-reuse T3 锚点/差异判停协议）**：机械来源 = 现跑命令**或带锚引用既有实测**，禁手写散文（存量反例：`260829-story-p3/taskpackage.json` 的 knownRedFloor 手写段）。闸已按条目级两态真跑（`-m "not live"`），launch 前在**仓根**跑：

```
python -m pytest -q --import-mode=importlib -m "not live" "<repo 绝对路径>/server/acceptance"
```

- 用**绝对路径**调（与 `git -C` 同法消 cwd 敏感）；取末行 summary（如 `8 failed, 119 passed, 41 deselected`）的 failed 数即上游已知红基线；固件 host 面另数 `firmware/main/test/host_app` 的 ELF 产物/证据（同用绝对路径）。
- **producer 记锚（五要素）**：progress.md 追加行 = 计数 + 命令 + 日期 + **量测时 HEAD commit SHA** + **面域**（host 面 = `firmware`、server 面 = `server`，两面分别记锚——一面变另一面可继续引用；append-only）。
- **consumer 差异判停**：Pod intake / 后续 launch 拿到基线记录后先跑 `git diff --quiet <anchor-sha> HEAD -- <面域>`：零差异 → 引用该 progress.md 行（SHA + 落盘锚 verbatim），**不重跑**；有差异 → 重跑该面（命令同记录）。
- **带锚引用三件齐判据**（第三态定义，纪律不放松）：量测 SHA + 命令 + 落盘锚三件齐才算合法引用，缺一即视同手写散文。
- **边界**：本协议只管上游已知红基线；本包验收红基线（acceptance-author 新冻结齿）恒现跑——新测试无缓存可言。
- 已知红 ≠ 本包红：Pod 首轮 verifier 见红先对基线（新增红才是本包责任）；基线本身的消解走独立批，不塞给被 launch 的包。taskpackage 的 `knownRedFloor` 字段引用 progress.md 行而非手写数字。

### 3. 运行时 worktree（复用空闲树 ∨ 按需新建）

换包协议四条（260915-gov-review-worktree-pool-reuse；语义 SSOT = pod-coordination「worktree 池复用」节）：

- **空闲判读**：树上有会话在跑即占用（一个目录一个会话，并行同树在操作层不可达）；board 的 worktreePath 是**记录不是锁**（merge-driver 定位用），不设独占校验门（human 2026-09-15 裁定）。
- **一树一常驻分支**：每棵树一个常驻分支（命名自由，如 `dev/pool-1`；git 禁同分支两树，多树天然分流）。包与包之间不换不删——上一包 **merged** → 直接续跑（merge-driver 的 rebase 步已把常驻分支 rebase 到 main，树内治理面即为最新）；上一包 **abandoned** → 先 main→base 同步（显式前置一步），再 `git reset --hard <同步后的专用 base 分支>` 复位清半成品——复位目标**非 main**（main..base 含未 launch 包的拆包制品，字面复位 main 会抹掉它们、下一包 intake 即 fail-closed）。
- **交接卫生**：换包前 tracked 改动须净、untracked 清理，**白名单保留 `build/` 与 `managed_components` 软链**（保住热编译与组件缓存——host build 热缓存随树，换包增量编译）；`firmware/main/test/host_app/sdkconfig` 属 gitignored 再生面、不随树保留——换包交接时**删除之**，强制下次构建从 tracked `sdkconfig.defaults` 重生成（消 TECH-261014 陈旧假红类；新树无此险，冷构建从 defaults 重生成即正确）。
- **按需扩容**：建新树决策归 human（评估并行收益 vs 一棵树的 host 冷编译成本）。新建时：

```
git worktree add C:/AI/KidPal-pod-<树名> -b <常驻分支名> <专用 base 分支>
```

（树名是纯标识符，运行时绑定唯一 = board worktreePath；既有按包名命名的树一律不改名——改名会孤儿化路径派生的图索引项目。）

### 3b. lite/nano 快路径（`rigor ∈ {lite, nano}` 包；261015-gov-review-lite-fast-path，nano 扩 = 261016-gov-review-nano-tier FROZEN v1.1）

触发 = 包 `rigor ∈ {lite, nano}` 且 triage 未声明「快路径不适用」（资格判注义务与例外声明 = [change-triage.md](../../../.claude/references/change-triage.md) §7 lite 判注与「档 0 nano 判据」节 nano 判注）。分叉两条：

- **池优先**：先按步骤 3 换包协议复用空闲池树——复用树自带热环境（build/ 缓存 + managed_components 软链 + 既有图索引），环境后置对它无意义，launch 只剩 board 预填 + 基线差异判停 + 写启动卡；无空闲树才经 human 决策建新树（步骤 3 第四条不变）。
- **新树免做的只有一件 = 固件环境等价化**（步骤 4 全件）——后置为真机轮前置条件，启动卡钉预检行「烧录前补齐固件环境等价化（步骤 4 配方），或退主 checkout 构建烧录」。其余 launch 面步骤照旧：**专属图索引维持「必办 + intake 前在册」原样**（ARQ-260905 零修订），仅加执行顺序注记——新树建成后**立即发起**步骤 5 图索引（快照导入 + 增量，分钟级），与 board 预填 / 启动卡撰写自然交叠。

### 4. 固件环境等价化（仅新建树；复用树跳过——环境已就位，换包时只做交接卫生核对。**lite/nano 包新树：整步骤后置真机轮前**，见 3b——届时一次性执行本步骤全件，或真机轮直接退主 checkout 构建烧录）

```
cp firmware/sdkconfig firmware/dependencies.lock <worktree>/firmware/
cp firmware/main/test/host_app/sdkconfig <worktree>/firmware/main/test/host_app/sdkconfig
ln -s /c/AI/KidPal/firmware/managed_components <worktree>/firmware/managed_components
cd <worktree> && node .claude/scripts/worktree-bootstrap.mjs --source C:/AI/KidPal
```

- host_app sdkconfig 镜像行 = TECH-261014 修法方向①落地（261015-gov-review-lite-fast-path T2 随批结项）：host_app 的生成态 sdkconfig 随树镜像，防 host 冷构建从 defaults 重生成后与树内验收基线漂移出假红。

- **managed_components 用 git bash `ln -s` 软链到主 checkout**（多 worktree 共享组件缓存，勿删勿重建）。`cmd //c mklink /J` 在 git bash 引号转义下会报 `Invalid switch`——别用。
- 软链建完用原生 python 验：`python -c "import os; print(os.path.isdir(r'<worktree>\firmware\managed_components'))"` 须 `True`（idf.py 走原生 python，MSYS 假链会坑它）。
- 主 checkout 的 managed_components 可能为空：首次 `idf.py build` 由 component manager 按 dependencies.lock 填充到共享目标，属预期。
- bootstrap 跳过 `.env` / shim 面 = 按设计（源不存在 / shim 已退役），不是故障。

### 5. 专属图索引（必办；新建树）

```
list_projects                                    # 先核无既有同名项目
index_repository(repo_path=<worktree>, mode=full, persistence=true)
```

- 项目名按路径自动派生；**复用树沿用其既有项目名**（随树复用、自动增量刷新——ARQ-260905 规则按 worktree 粒度，与包无关）。
- main 已 commit 快照 artifact（`.codebase-memory/graph.db.zst`，b7aa3df 起）→ worktree 索引 = 快照导入 + 增量，分钟级、近零成本。
- 返回值 `nodes/edges == expected_nodes/expected_edges` 才算建完；`skipped/parse_partial` 按 best-effort 信号如实记录。
- **不要传 `moderate`**（261026 批 T2）：实测 moderate 比 full 少约 44% 节点（削减几乎全在 Variable 面），且 mode 只在从零解析时生效——对已建项目换 mode 重跑是空转；改档须先 `delete_project` 再按 full 重建。

### 6. board 预填

```
node .claude/scripts/portfolio-reconcile.mjs --set-pod <changeId> --status queued --worktree-path <运行时 worktree 绝对路径>
```

只预填 queued + worktreePath（复用时 = 重指到所复用树的路径）；`--status running` 归 Pod intake（planner Step 1）做。

### 7. 启动卡（260915-gov-review-worktree-pool-reuse T1⑤：prompt 收缩）

写 `delivery/taskpackages/<changeId>/launch-card.md`（**Git-tracked**，随 commit-before-launch 进基准 ref——Pod 建树即天然拿到。落点迁移见 260921-gov-review-launch-pipeline T2）——**数行启动卡，六要素**：

```
跑 <changeId>（<workstream>，mergeOrder <N>）。
基线锚：<workstream progress.md 带 SHA 行>——各面域判停结论（可引用 / 须重跑，按步骤 2 差异判停）。
上一包留根：<delivery/taskpackages/<上一包changeId>/pod-retro.md>，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
固件严格模式默认开：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省；261015 批 T3 起 defer > required——lite/nano 包 scoped 相位 defer 短路优先于本行，final 相位恒跑）。
会话标识：<pid / 终端标识 / 启动时刻>（点火后由 launch 执行者回填——叫停时核「进程确实已死」用，见 pod-coordination「叫停协议」节）。
（lite/nano 包新树加一行）真机轮预检：烧录前补齐固件环境等价化（步骤 4 配方），或退主 checkout 构建烧录。
命令行（两形态并列，标注适用条件）：
① 人在场交互式：claude --settings .claude/dev-loop.settings.json --agent planner
② 无人值守 headless：claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner（另加 CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0）
```

- 身份探测 / 包事实 / 环境事实 / 教训全文**不再搬运**：planner 凭 changeId 裸起步自探测四处（taskpackage / upstreamRef / pods json / 本包启动卡——planner.md Step 1）；包事实在 taskpackage.json + plan.md + decision-coverage；环境事实的耐久家 = skills（kidpal-idf-env / kidpal-serial-monitor / kidpal-ecs-gateway / kidpal-live-smoke-debug）；基线家 = progress.md 带锚行；教训家 = 上一包 pod-retro.md；input-brief 与 C 侧 cwd 敏感判读等既有指针由 planner/verifier 自身定义与 260909 批制品承载。
- launch 侧动作收敛为三件：board worktreePath 重指（步骤 6）+ 基线差异判停（步骤 2）+ 写启动卡。
- **存量卡位置注记（260921-gov-review-launch-pipeline T2）**：落点迁移前的启动卡留在 git common dir 的 `.git/devloop/launch/prompt-<changeId>.txt`（**历史制品，不回改、不迁移**）。按新路径找不到旧卡时，**不等同于"拆包产物缺失"**——先去该目录查。

### 8. 记录与交接

- progress.md 追加段：调度判定、门结果、环境事实、遗留 human 项（该文件 append-only，不覆盖旧行）。
- 向 human 交付：每包一份启动卡（路径见步骤 7；human 在对应 worktree 的会话自跑，命令行见卡内第五要素）。

## 坑（实测代价换来的）

- **会话从哪棵树启动就在哪棵树干活**（261015 D 事故）：从 pod-m2 启动的会话去兄弟树派发/写文件，被 enforce-scope 按越界全拦（烧探针后退化成跨树双写互踩）；T5 修复后跨树写按目标树 scope-map 重锚判定（红线面仍拦），但治理语义不变——**树隔离是池模型的承重墙**，跨树干活的合法形态 = 在该树起自己的会话。
- **叫停 = 先停会话进程、再处置树与分支**（261015 E 事故：树删了、headless 会话裸跑 2h）：顺序条款与树处置两形态见 `pod-coordination.md`「叫停协议」节，此处不双写正文。
- **过期的环境注记会传染**（260915 起 prompt 已收缩为启动卡，此坑移位到 skills / progress.md 锚行）：引用任何环境事实或基线前逐项核地面真值（list_projects / worktree list / 文件存在性 / anchor SHA 可达）——图索引漏建事故根因即抄旧文（2026-09-05 追因）。
- **复用树的交接卫生**（260915 树池模型新增）：换包前不核对 tracked 净 + untracked 清理，上一包的证据残留/半成品会漏给下一包（intake 误判、基线误读）；白名单只保 `build/` 与 `managed_components` 软链。
- **图索引是必办不是优化**：worktree 被跟踪文件互相不可见，主索引 root_path 指主 checkout，查 worktree 新增文件非专属索引不可。
- **不越权**：本 skill 只管 launch 面。launch 后的一切（intake/builder/verifier/integrator/合并门）归 planner Pod；跨 Pod 拆包/调度/建 workstream 归 portfolio-planner；契约面归契约门。
