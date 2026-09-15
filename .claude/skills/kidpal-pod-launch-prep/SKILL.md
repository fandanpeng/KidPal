---
name: kidpal-pod-launch-prep
description: Prepare the launch surface for the next batch of kidpal-mvp TaskPackages (dev-loop Pods) — scheduling call from board state + dependsOn, check-launch gate, worktree/branch creation, firmware env equivalence, per-worktree CodebaseMemory graph index, board prefill, and launch prompt authoring. Use when a batch of Pods (e.g. after a merge to main) is ready to have its dev environments prepared; does not launch, merge, or push.
---

# KidPal Pod Launch Prep（task package 开发环境准备）

把「下一批 TaskPackage 的 launch 面」从调度判定一路备到 human 可自跑 launch 命令。适用于 kidpal-mvp workstream 多 Pod 模式（PROPOSAL FROZEN v0.4，15 包，WIP=3）；单 Pod 形态是子集。

## 产物（完成标准）

1. 调度判定记录（哪几包、为什么）→ 落 `delivery/workstreams/kidpal-mvp/progress.md` 追加段
2. 每个包：`--check-launch` 机械门绿
3. 每个包：worktree `C:/AI/KidPal-pod-<suffix>` + branch `dev/<changeId>`（base = 当前 main）
4. 每个包：固件环境等价化（sdkconfig / dependencies.lock 镜像 + managed_components 共享软链 + bootstrap 跑过）
5. 每个包：专属图索引 `C-AI-KidPal-pod-<suffix>`（moderate + persistence）——**launch 面必办，不得降级读文件**（ARQ-260905 规则钉死）
6. board（`devloop/pods/<changeId>.json`）预填 worktreePath
7. launch prompt 落 `.git/devloop/launch/prompt-<changeId>.txt`
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
node .claude/scripts/portfolio-reconcile.mjs --check-launch <changeId> --base-ref main
```

blocking 非空即停——回 portfolio-planner 补 commit，不绕过。

**「上游已知红」计数（260909-gov-review-gate-truthfulness T5）**：机械来源 = 现跑命令，禁手写散文（存量反例：`260829-story-p3/taskpackage.json` 的 knownRedFloor 手写段）。闸已按条目级两态真跑（`-m "not live"`），launch 前在**仓根**跑：

```
python -m pytest -q --import-mode=importlib -m "not live" "<repo 绝对路径>/server/acceptance"
```

- 用**绝对路径**调（与 `git -C` 同法消 cwd 敏感）；取末行 summary（如 `8 failed, 119 passed, 41 deselected`）的 failed 数即上游已知红基线；固件 host 面另数 `firmware/main/test/host_app` 的 ELF 产物/证据（同用绝对路径）。
- 输出落 handoff 制品 = 本 skill §8 的 progress.md 追加一行（计数 + 命令 + 日期，append-only）；taskpackage 的 `knownRedFloor` 字段**引用该行**而非手写数字。
- 已知红 ≠ 本包红：launch prompt 里标清基线数字，Pod 首轮 verifier 见红先对基线（新增红才是本包责任）；基线本身的消解走独立批，不塞给被 launch 的包。

### 3. worktree + branch

```
git worktree add C:/AI/KidPal-pod-<suffix> -b dev/<changeId> main
```

- `<suffix>` = changeId 去日期前缀（`260829-voice-roundtrip` → `voice-roundtrip`），与既有 `KidPal-pod-device-ws-switch` 等命名一致。
- base = **当前 main HEAD**（先例：85cc0c6 / ad1611c 各批取当时 main）。

### 4. 固件环境等价化

```
cp firmware/sdkconfig firmware/dependencies.lock <worktree>/firmware/
ln -s /c/AI/KidPal/firmware/managed_components <worktree>/firmware/managed_components
cd <worktree> && node .claude/scripts/worktree-bootstrap.mjs --source C:/AI/KidPal
```

- **managed_components 用 git bash `ln -s` 软链到主 checkout**（多 worktree 共享组件缓存，勿删勿重建）。`cmd //c mklink /J` 在 git bash 引号转义下会报 `Invalid switch`——别用。
- 软链建完用原生 python 验：`python -c "import os; print(os.path.isdir(r'<worktree>\firmware\managed_components'))"` 须 `True`（idf.py 走原生 python，MSYS 假链会坑它）。
- 主 checkout 的 managed_components 可能为空：首次 `idf.py build` 由 component manager 按 dependencies.lock 填充到共享目标，属预期。
- bootstrap 跳过 `.env` / shim 面 = 按设计（源不存在 / shim 已退役），不是故障。

### 5. 专属图索引（必办）

```
list_projects                                    # 先核无既有同名项目
index_repository(repo_path=<worktree>, mode=moderate, persistence=true)
```

- 项目名按路径自动派生 = `C-AI-KidPal-pod-<suffix>`。
- main 已 commit 快照 artifact（`.codebase-memory/graph.db.zst`，b7aa3df 起）→ worktree 索引 = 快照导入 + 增量，分钟级、近零成本。
- 返回值 `nodes/edges == expected_nodes/expected_edges` 才算建完；`skipped/parse_partial` 按 best-effort 信号如实记录。

### 6. board 预填

```
node .claude/scripts/portfolio-reconcile.mjs --set-pod <changeId> --status queued --worktree-path C:/AI/KidPal-pod-<suffix>
```

只预填 queued + worktreePath；`--status running` 归 Pod intake（planner Step 1）做。

### 7. launch prompt

写 `.git/devloop/launch/prompt-<changeId>.txt`（**在 git common dir，不入版本控制**）。模板要素（抄 `.git/devloop/launch/prompt-260903-device-ws-switch.txt` 骨架）：

- launch-argument 形状声明 + planner 身份 + **运行形态**：默认 human 在 VS Code 交互会话起（AskUserQuestion 可用、升级走正常 ruling）；headless 无人值守为备选形态（AskUserQuestion 不可用 → 严格非交互 fail-closed：pending-human-ruling.md + needs-triage）。Step 1 handoff 探测三处（taskpackage / PROPOSAL 定位 / pods json → `--set-pod running`）。
- 本包定位：mergeOrder、dependsOn（标清哪些已 merged）、PROPOSAL §4 共享面/碰撞面条款、spec/契约锚点。
- 环境既成事实：IDF 通路 `python c:\tmp\idf_run.py`（丢失按 pod-s-handoff/README.md §3.1 重写）、sdkconfig/deps/软链/图索引项目名、上游 merged 面（server/gateway 等）、ECS 访问 `c:\tmp\ssh_helper.py`、live 拓扑指针（ops skill `kidpal-ecs-gateway`）。① **固件严格模式默认开**（260909 T3/T5，OQ1=(b)）：launch prompt 环境段注入 `VERIFY_FW_BUILD_REQUIRED=1`——firmware-build 闸双路探测（PATH idf.py ∨ venv python + `c:/tmp/idf_run.py` wrapper）不通即红；显式关闭须在 prompt 留痕理由。② **260908 批 input-brief 机制接入**：builder/验收起草前跑 `node .claude/scripts/brief-inputs.mjs --change-id <id> --role acceptance-author` / `--role builder` 取输入面；`crossEnd` 判断先补——判不了向严填 `counterparts`，**不填 `exempt`**。③ C 侧遗留指针（只登记不改码，自有实现面走 dev-loop）：`firmware/main/test/**.acceptance.c` 读 `delivery/changes/dev/**` 证据按相对路径、root 定位吃 cwd（`voice_roundtrip_b.acceptance.c:878` 类），消解归 dev-loop 包，见 260909-gov-review-gate-truthfulness §6 留根。
- **上一批收口留根的 live 偏差逐条移入 intake 评估**（构成 blockingDecision → fail-closed 上报，不静默推进；先例：Intent:nointent 待 OpenClaw tools 修复、ASR 终选凭据未裁定）。
- 纪律提醒：运行形态对应的 fail-closed 口径、契约/验收冻结面、pod-events 实时 append、合并只走 merge-driver 本地 --no-ff、不直推 main 不 push。

### 8. 记录与交接

- progress.md 追加段：调度判定、门结果、环境事实、遗留 human 项（该文件 append-only，不覆盖旧行）。
- 向 human 交付：每包一份 launch prompt（human 在对应 worktree 的 VS Code 会话自跑；headless 命令形态为备选）。

## 坑（实测代价换来的）

- **旧 prompt 的过期环境注记会传染**：照抄上一批 prompt 前逐项核地面真值（list_projects / worktree list / 文件存在性）——图索引漏建事故根因即抄旧文（2026-09-05 追因）。
- **图索引是必办不是优化**：worktree 被跟踪文件互相不可见，主索引 root_path 指主 checkout，查 worktree 新增文件非专属索引不可。
- **不越权**：本 skill 只管 launch 面。launch 后的一切（intake/builder/verifier/integrator/合并门）归 planner Pod；跨 Pod 拆包/调度/建 workstream 归 portfolio-planner；契约面归契约门。
