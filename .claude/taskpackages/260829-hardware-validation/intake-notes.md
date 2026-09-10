# Intake Notes · 260829-hardware-validation（包 9b）

> planner 记录 · 2026-09-09 · Pod 置 running（portfolio-reconcile 已落 16 pods，mergeOrder 13，dependsOn 2/3/4/5/6/7/8/U 全 merged）

## Step 1 handoff 探测（完成）

- launch 前置闸：dependsOn 首项 260901-ui-art-integration（U）已 merged 入 main @ `6ae9f30`（merge-driver，human-approval-ref 在案；closeout 事件 `fc14e89`）。门通过。
- worktree `dev/260829-hardware-validation` ff 同步 main tip `fc14e89`（base `3ea319f` → tip）。U 包 47 枚资产 + `art_assets` 冻结契约 + `ui_art_integration.acceptance.c` 随并入树。
- 权威输入 `taskpackage.json` 已读：story（物理冒烟 + 端到端真机 + v0.4 新增续航基准/常驻 WS 功耗/证书握手冒烟）、HWV-A1~A8、exitCriteria HWV-E1~E4、testHost=device、rigor=full、riskTier=high、gateProfile=[types, evidence-integrity, docs-sync]。
- workstream 设计权威 `.planning/kidpal-mvp/PROPOSAL.md`（FROZEN v0.4，raw-content 归一 pin）已读：§2 包 9b 行、§3 关键路径 `…→U→9b→9a`、§7 留根⑥「char 热字缓议 / 4 页循环翻页 9b 兜底 / 9b 两条真机项」。
- board 已 `--set-pod --status running`；`brief-inputs` acceptance-author ok:true（adv 2 条：契约/PROPOSAL 无锚点退化整文件引用）；crossEnd 骨架已补（counterparts 待 freeze 钉实）。

## 红基线（上游已知红，260909 T5 机械来源口径 @ main 3ea319f）

- ① server 验收 `python -m pytest -q --import-mode=importlib -m "not live" c:/AI/KidPal/server/acceptance` → **8 failed / 119 passed / 41 deselected**（test_smoke_e2e 7 条 + test_voice_roundtrip_b 1 条，与 260909 D3 落地首证同名单）。
- ② host ELF 新编现跑（WSL linux target，`firmware/main/test/host_app/build/host_infra_test.elf`）→ **86 Tests / 3 Failures / 0 Ignored**，三红 = VRT-A2:878 / WSA-A6:830 / WSA-A4:663 证据齿（本地-only 证据从未入仓）。
- **判读口径**：本包 device 面为主；**新增红才是本包责任**。计数出处 = progress.md 2026-09-09 段。taskpackage 的 knownRedFloor 字段引用本行，不手写数字。
- **ff 后重测修正（acceptance-author 实测 @ fc14e89，2026-09-09）**：U 定向处置批已消解 8 条既有 server 红（4 SMK 架构过时 superseded + 4 证据回收）→ 现 server = **4 failed / 122 passed / 6 skipped / 45 deselected**（4 全为本包新冻 F2 齿、红=真实缺失接线；122=121+1 回归守卫；41→45=+4 新 live 齿 deselected）。C 侧 stub runner BUILD_CLEAN + **4 Tests / 3 Failures**（红=真实缺失结构：F2 设备侧零生产调用 / RTC·SHTC3 driver 未绑定 / 未接线 HOST_ACCEPTANCE_SRCS）。host ELF 闸 runner 未重编（交 builder 接线后 verifier 复跑）。device 面红 by construction（未开跑，待 human 持板）。

## blockingDecision 判读（intake 必办）

### Intent:nointent（ARQ-260907-openclaw-tools-rejection-rootcause）→ 不阻塞

riskTier=low「能力缺失非缺陷，已有 front 发起 relay 兜底」。语音主流程（F4 问答 / 语音查字 / 语音问课表）走 设备⇄wss⇄网关⇄OpenClaw 会话→display relay，**不依赖 LLM tool calling**（提醒=cron、display=front relay、story=网关侧）。9b 全功能端到端不踩该缺口。登记 risk-accepted 已知偏差，不 fail-closed。诊断资源 `c:/AI/OpenSourceReference/openclaw-main`（只读）搁置，不属本包。

### story-p3 §9 留根 F4/F6/F7 → human 裁定「扩界接线」

- **F4（SHOULD）**：prompt v3 让 LLM 逐字复述整段转写并回填拿不到的 `id`，云侧 producer（`story.py::archive_story/story_text/list_stories`）生产路径零调用 —— 同 char-stroke 109s 病灶。修法 = 分工改回「网关 `archive_story` 落盘 → 网关 `story_text(record)` 组块下发，模型一回合只出 title」。
- **F6（NIT）**：`front_app.py:260` session_id `conn-{id(websocket)}` 用 CPython 对象地址，连接关闭可复用、`StoryAccumulator` 不 pop → 跨会话串味。修法 = uuid4 或连接强引用 + WS 断开钩子 pop。
- **F7（FYI）**：设备页内输入（`kidpal_story_page_input`/`_on_saved`/`_rec_tick`/`_stamp_spec`）与云侧停录归档触发（`stop/feed/routing/recover/redo`）零生产调用者。→ 9b 显式收这四项为必接面。

**裁定（human 2026-09-09）**：9b 扩界承接 F2「录制→归档→盖章」生产接线。依据 = 上游 F4/F7 已把接线殿后指向 9b，mergeOrder 13 后无后续包。

### 包 7 留根 live 五齿 → human 裁定「承接」

live 面 5 条 `@pytest.mark.live` 齿 L1-L5（cron 真在册且真跑过 / pending.json 真形状 / 真推到设备 / 真渲染回执出队 / 真大模型语义产物）需 ECS 部署（cron 声明 + 生产者脚本 + prompt v3 overlay + 引擎补丁认领 `kidpal_speak`）+ human 批准。runbook = `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` §8。

**裁定（human 2026-09-09）**：9b 承接该 ECS 部署窗口，连同真机轮一并安排。

## intake 必办殿后项汇集（逐条 disposition）

| 殿后项 | 来源 | 落点 | disposition |
|---|---|---|---|
| RTC 走时 + SHTC3 直读真机（打盹阈值默认 15，本包 owns 调定） | 包 5 留根 P1-E2 | HWV-A4 | 承接，真机测 |
| 麦克风采音真机（录音清晰、可上 ASR 转写） | 包 8 留根 | HWV-A3 | 承接，真机测 |
| 4 页循环翻页跨页端到端兜底 | 包 3 留根 / HWV-E3 | HWV-A6 | 承接，真机测 |
| 真实提醒音效/亮屏/1bpp 网点观感 + 弹窗与 TTS 并发时序 | 包 7 留根 ① | HWV-A6 | 承接，真机测（依赖 ECS cron 真在跑） |
| live 五齿 L1-L5 ECS 部署 | 包 7 留根 ② | — | 承接（human 裁） |
| F2 录制→归档→盖章生产接线（F4/F6/F7） | story-p3 §9 | 新增验收 | 扩界（human 裁） |
| 16px 以下 CJK 1bpp 真机可读性（ARQ-260831-small-cjk-1bpp-readability 承载点，直接决定 V3 字号下限） | 9b 原项 | HWV-A7 | 承接，真机测，**结论回写设计面** |
| 前光确认（反射屏暗处可见性，spec §5.6） | 9b 原项 | HWV-A8 | 承接，真机测 |
| 实机续航基准（决策 21 DEFER 三要素：触发点/预警阈值/owner 本包）+ 常驻 WS 功耗观察 | 9b v0.4 新增 | 新增验收 | 承接，真机实测数据落盘 |
| 证书真实设备握手冒烟（CA pinning 真机首连） | 9b v0.4 新增 | 新增验收 | 承接，真机测 |
| spec.md:332（V20）「预合成缓存」vs 实际「到点现合成」口径差 | 包 7 closeout 登记 | 非本包职责 | 真机验证按「到点现合成」实况核对，不按旧措辞判红 |

## 实现面定向（acceptance-author / builder 各自实读）

- **F2 接线三面**：设备 `firmware/main/display/story_page.c`（视图模型 + 渲染）+ `firmware/main/mcp_server.cc`（case2/5/6 wiring）；云侧 `server/gateway/story.py`（StoryAccumulator / archive_story / story_text / list_stories 零生产调用者）+ `server/gateway/front_app.py:260`（session_id 地址复用）；prompt `server/deploy/openclaw-system-prompt.v3.md`（F4 分工病灶）。F4 修法 = prompt v3.1（删「回填 text / id 系统给定」两句，模型一回合只出 title）+ 网关 story_text 组块下发。
- **ECS 部署面**：`server/deploy/openclaw-patch/`（DEPLOY-RUNBOOK §8 + apply-openclaw-patch.sh + reminder_schedule_lib.py + make_openclaw_patch.py）。ECS 访问 `c:\tmp\ssh_helper.py`，live 拓扑读 ops skill `kidpal-ecs-gateway`。
- **设备验收证据纪律（HWV-E2）**：烧录日志 + 逐项勾验记录落盘，「没报错就算过」禁止；probe ≠ exercise（DEV-GOVERNANCE §2）。
- **上游既成事实（勿重复排障）**：IDF 通路 `firmware/` 目录下 `python c:\tmp\idf_run.py build`（canonical = venv python）；烧录/串口/NVS 分别读 skill `kidpal-idf-env` / `kidpal-serial-monitor`（COM5 单进程）/ `kidpal-nvs-ops`；live 排障 `kidpal-live-smoke-debug`；ASR 终选 Qwen3ASRFlash + dashscope SK（human 2026-09-06）。
- **C 侧遗留指针（只登记不改码）**：`firmware/main/test/**.acceptance.c` 证据读取 root 定位吃 cwd（`voice_roundtrip_b.acceptance.c:878` 类）——判读时钉明 cwd。

## 环境既成事实

- 严格模式 `VERIFY_FW_BUILD_REQUIRED=1` 已注入（设备真编是本包验收承重面，本 prompt 无关闭记录 = 开，不得关闭）。
- firmware/sdkconfig + dependencies.lock 已镜像；managed_components 软链指主 checkout（勿删勿重建）；worktree-bootstrap 已跑（.env/shim 面源不存在，按设计跳过）。
- 本 worktree CodebaseMemory 项目名 = `C-AI-KidPal-pod-hardware-validation`（本包新增文件）；跨包/主仓历史面用主索引 `KidPal`。
- `.codebase-memory/{artifact.json,graph.db.zst}` 常驻脏，不追交不 stage 不 stash 不 checkout 还原。
- 合并只走 merge-driver.mjs 本地 --no-ff（high → 大概率 ESCALATE，human 批准路径 = --human-approval-ref）；任何路径不得直推 main、不得 push remote。