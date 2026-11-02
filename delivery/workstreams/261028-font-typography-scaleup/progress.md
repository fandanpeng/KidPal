# progress · 261028-font-typography-scaleup

> append-only。本文件记调度判定、launch 面事实与带锚基线行（producer 五要素：计数 + 命令 + 日期 + 量测 HEAD SHA + 面域）。

## 2026-10-28 · P0（m6-k1）launch 面准备 + 基线锚

- **调度判定**：本批单包 = `261028-m6-k1-font-assets-runtime-loading`（P0 spike，dependsOn=[]，mergeOrder 60）。P0 阻塞 P1/P2/P3（spike-first 硬边界：P0 未以消费端真机证据收口前 P1/P2/P3 不得 launch）；P1 另有 P0/P1 共享 `kidpal_render_node.cc`/host seam 串行边。本会话 = human 在池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`）直启 `/build-feature <changeId>`，launch 面预检五件缺 ②③④ → 按预检路由补 launch-prep。
- **互斥租用检查**：同树上一包 `261028-p3-record-session-mirror-desync-fix` board 残记 `awaiting-human:merge-gate`，实测已随 357bc5fc 合入 main（0886bd43 为 main 祖先，合并提交信息明载 human 合并门随批批准）→ board 修正为 merged，树释放；扫描 `devloop/pods/*.json` 无其它活跃 Pod 指向本树或 `dev/pod-m2`。
- **机械门**：`--check-launch 261028-m6-k1-font-assets-runtime-loading --base-ref dev/pod-m2` ok=true blocking 空（LP4 门 1 GREEN + human 过目、LP5 commit-before-launch 两条 workstream 级前置实际已由 de3a9f4c 满足：拆包四件套 + 门 1 裁决已 commit）；`settings-lint.mjs` 3 files 0 RED 0 WARN。
- **环境事实**：复用树交接卫生绿——tracked 净、零 untracked、host_app sdkconfig 在位（同日同树连续包，未发生跨树交接，保留并作为本次基线实测配置）、managed_components 软链原生 python 验真 True、host build 热缓存（ninja no work to do）。
- **firmware host 面基线锚**：**242 Tests / 3 Failures / 0 Ignored** @ SHA `589e01275aced4d2f04bc22d4ea796eb25257d8f`（2026-10-28 本树实测；命令 = WSL `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` + `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`；3 条红全为 p1_auxbar AB-A1/A2/A4 存量族，与 261022 锚 `0df2c0c3`、261028 锚 `aecded8b`（237/3）同族——测试总数 237→242 属 mirror-desync 批新增，红名单零变化）。落盘锚 = 本文件本行。
- **server 面基线锚（引用不重跑）**：**370 passed / 7 deselected / 33 subtests passed / 0 failed** @ SHA `6ea7769b`（2026-10-28 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`；落盘锚 = `delivery/taskpackages/261028-p3-record-session-mirror-desync-fix/launch-card.md` 基线锚行）。差异判停 2026-10-28 本树实测：`git diff --quiet 6ea7769b HEAD -- server/` exit 0 → 引用不重跑。

## 2026-10-28 · P0（m6-k1）integrator 收口

- **收口态**：verify 第 4 轮 8 闸全绿（250 Tests / 3 Failures 仅存量 p1_auxbar 族，零新增红；设备 build 绿 xiaozhi.bin 0x324950 / assets 余 4,655,633B）；Review Packet + docs-delta + test-scope 清单落盘 `delivery/taskpackages/261028-m6-k1-font-assets-runtime-loading/`；ADR-017 实测注记 append-only 回填（376,568B / 306,860B / line_height 23·28）。改动 commit 于 dev/pod-m2（feature 分支，未 push），base ref 9f8b2170 可整包回滚。
- **待主线程**：supervisor 合并门（supervisor-verdict-gate）→ human 升级闸 → merge-driver；M6 milestones 行 state 翻账（planning→in_delivery）建议随合并执行；P0 真机轮（FAR-E3）收口前 P1/P2/P3 保持不 launch。

## 2026-10-29 · P0（m6-k1）合并门后置裁定 + P1 接棒

- **human 裁定（pod-events human-merge-gate-deferred）**：P0 不单独合并——P1（261028-m6-k2a-p1-home-16px）开发完成后真机看效果，P0+P1 一起合并；FAR-E3 真机轮随统一轮执行（P0 探针页 + P1 首页消费 16px 同轮观测）；P1 在 P0 已 commit 冻结面（dev/pod-m2 @ 789c0244）上开发，串行边成立。P0 board = verifier-green。
- **P0 收尾态**：verify 第 4 轮 8 闸全绿；commit 789c0244（24 files）；supervisor-verdict ESCALATE（riskTier=high 申报升级）；pod-retro / review-packet / docs-delta / test-scope 落盘；回滚锚 = 9f8b2170。

## 2026-10-29 · P1 验收冻结轮 + 两升级项

- P1 验收：p1_home_16px.acceptance.c 新冻结（P16-A1..A3）+ p1_home_battery / p1_cjk_name 收紧型 supersede（REVISION LOG 在件）；FAS-A8 反向评估零触；红基线通道① 250/4（存量 3 + P0-chain 在册红）、通道②语法实编绿；接线增量 acceptance-wiring-patch.md 落盘待 builder step-0。
- **升级①（blocker·P0 链缺陷）**：78 fork cbin writer 恒产 LARGE=1 glyph dsc 布局，host/device 两侧按 LARGE=0 解读 → 字形度量垃圾；P0 冻结件假绿（只钉返回码）；修复方向待 architect 裁（详见 P1 taskpackage notes①）。
- **升级②（F-9 上游缺口）**：现网 producer 不写 ※ 前缀进 name，[U1] 真机永不触发风险；路由待 human 裁（上游写入侧任务 / wire 契约变更 / 需求重裁）。

## 2026-10-29 · P1 收口 + 双包待统一真机轮

- **P1 收口态**：verify 8 闸全绿（253/3 仅存量 auxbar）；commit 550f8d21（实现五文件 + 治理制品）；supervisor-verdict ESCALATE（riskTier critical——P1 包 diff 范围含 8239aee7 的 sdkconfig.defaults critical-path 扫描命中）；pod-retro / review-packet / docs-delta / test-scope 落盘；回滚锚 = 08cc46c4。
- **合并门状态（human 后置裁定在案）**：P0（verifier-green @ 77c7debd 收尾）+ P1（verifier-green @ 550f8d21）均不合并——统一真机轮（P0 探针页 + P1 首页 16px + 缺 assets 降级）human 过目后一起合并。
- **真机轮前置 blocker**：ARQ-261029 上游 ※ 写入侧任务待派单（server/front/OpenClaw 侧，wire 不变）；无它下划线观测无样本。

## 2026-10-29 · 真机统一轮（P0+P1）——核心观测通过

- 烧录：全量（xiaozhi.bin 0x324C20 + generated_assets.bin 3,732,975B @0x800000），hash 校验过，NVS 未动。
- 启动：SD 卡日志时间戳单调推进、状态机到 idle、`kidpal_rn: render home 400x300 render_rc=0`（P1 渲染路径真机无崩）；assets 分区 checksum 315ms 通过。
- **human 看屏确认：「字体对了」**——首页 16px 正文真机字形来自运行时资产（P0 FAR-E3 核心消费端证据 + P1 P1H-E1 真机半首证）。下划线项休眠（ARQ-261029 上游未落，预期内）。
- 待办：缺 assets 降级开机测试（FAR-E3 第二观测，可选）；P0+P1 合并门（human 已看核心效果）。

- **human 裁定（2026-10-29）**：豁免缺 assets 降级真机观测（FAR-E3 第二子项记豁免非通过）——理由 = 14px 常显真机正常 + host 半 FAS-A4 已验回退逻辑 + 设备触发场景低概率（全量烧录恒带 assets、OTA 不动 assets 分区）。

## 2026-10-29 · P0+P1 合并入本地 main（人类合并门批准，随批）

- P0：merge-driver --pod（human-approval-ref=158948be）→ rebase → quick-check 2 闸绿 → merge OK → 本地 main @ 4912575c（簿记 de865411）。
- P1：**随批并入**——P0 merge 的 rebase 已把共享分支上 P1 全部改动带入 main；前置闸对 P1 off-main 判 BLOCK（原 SHA 经 rebase 不复存在、无独立分支可合），按「就地记录不改写历史」处置：P1 pod-events merge-completed 事件 + board merged，不重复 merge。human 裁定「P0+P1 一起合并」已兑现。
- 远程发布（release-pr --publish + PR 合并）仍待 human；PR 合并后跑 --sync 追平。

## 2026-11-02 · P2（m6-k2b）launch 面准备 + 基线重测锚

- **调度判定**：本批单包 = `261028-m6-k2b-p2-schedule-16px`（列车 P2，mergeOrder 62，dependsOn=[P0 已 merged 4912575c/93b92a6b]）。P0/P1 已入 main、真机统一轮核心观测过（「字体对了」），P2 launchPreconditions 三条满足（上游 phase 完成 / anchors v1.69+§12 冻结 / UI-ready 在位）。human 在池树 `C:/AI/KidPal-pod-m2`（常驻分支 `dev/pod-m2`）直启 `/build-feature <changeId>`，预检五件缺 ②③ → 复用本树补齐（③ 树常驻随会话在位、④ 图索引 `C-AI-KidPal-pod-m2` 在册随树复用）。
- **互斥租用检查**：同树上一包 `261031-openclaw-cron-trigger-gating` board=merged（fcd66e1f），树释放；扫描 `devloop/pods/*.json` 无活跃 Pod 指向本树或 `dev/pod-m2`。分支 ff：`dev/pod-m2` 14831834 → main 6ce77123（ff-only，0 领先 2 落后）。
- **机械门**：`settings-lint.mjs` 3 files 0 RED 0 WARN；check-launch 于 prep commit 后跑（结果见下条交付记录）。
- **交接卫生**：tracked 净、零 untracked；host_app sdkconfig 按换包协议删除（中间隔 261031 批 + P0/P1 合并后强制从 tracked defaults 重生，消陈旧假红）；managed_components 软链原生 python 验真 True；host build 热缓存。
- **firmware host 面基线锚（重测产物）**：**253 Tests / 3 Failures / 0 Ignored** @ SHA `6ce77123`（2026-11-02 本树实测；命令 = WSL `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` + `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`；3 红全为 p1_auxbar AB-A1/A2/A4 存量族，与 P0 锚 `589e0127`（242/3）、P1 收口（253/3）红名单零变化）。差异判停：P1 锚 `789c0244` 对 `firmware/` 有 diff（P0+P1 合并）→ 强制重测，本行为 P2 起合法引用锚。落盘锚 = 本行。
- **server 面基线锚（重测产物）**：**395 passed / 9 deselected / 33 subtests passed / 0 failed** @ SHA `6ce77123`（2026-11-02 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`）。差异判停：旧锚 `6ea7769b` 对 `server/` 有 diff（261031 批）→ 强制重测，本行为 P2 起合法引用锚。落盘锚 = 本行。
- **遗留 human 项**：ARQ-261029 上游 ※ 写入侧待派单（P2 真机轮下划线观测样本依赖，观测受限不记失败）；远程发布（release-pr --publish）仍待 human。

## 2026-11-02 · P2（m6-k2b）真机轮（P2S-E5）——发现截断 → 现场裁定修订 → 复看通过

- **第一轮看屏**：pitch 19 紧凑观感成立、16px 字形正确、4/4 容量成立；发现**上午带第 4 行墨底被带底线截 1-2px**（周一「科学」、周二「体育」等）。planner 诊断：行 y = band_top+2+r*19，第 4 行墨底 = 89+(23-5)-14+1+13 恰压 y=107，零余量。下划线/★/长课观测受限（当周课表无样本；ARQ-261029 上游写入侧在册）。
- **human 现场裁定（原话逐字）**：「上午显示4行 最后一行，还是有一点被截断了，你可以看照片，如果是放不下的原因，我们可以把晚上保留两行就行了」→ 版式 78/78/69→88/88/49、容量 4/4/3→4/4/2；acceptance-author 冻结件真机轮条修订（授权 = human 原话）+ builder 常量同步；verify scoped 8 闸绿 + final bearing r2 全绿（r2.json）；supervisor verdict 换新 ESCALATE、gate-check 绿。commit 10f13a60。
- **第二轮看屏（复烧 0x20000 后）**：human 裁「可以了」——上午第 4 行完整、晚上带正常显示现有 2 门、其余回归面无异常。**P2S-E5 真机轮 PASS**。
- **需求面**：容量 4/4/2 同步起草中（story-author，v1.70），requirement-reviewer 审稿后随合并批落。

## 2026-11-02 · P2（m6-k2b）合并入本地 main（human 合并门批准，随批需求同步 v1.70）

- P2：merge-driver --pod（human-approval-ref=2f611c92）→ rebase → quick-check 2 闸绿 → merge OK → 本地 main @ 335af886（簿记 4ee8f796）。需求同步 v1.70（容量 4/4/2 + 带高 88/88/49）随批 frozen（93dc1df5，story-author + requirement-reviewer 四查 PASS）。
- 真机轮 P2S-E5 两轮收口：第一轮发现上午第 4 行墨底截断 → human 现场裁定（88/88/49 + 4/4/2）→ 修订轮全绿复烧 → 第二轮 human 裁「可以了」。
- **列车状态**：P0/P1/P2 均 merged；剩 P3（261028-m6-k3-p3-story-typography，mergeOrder 63）待 launch——P3 收口轮兼 workstream 四页并集终端收口（P3 前 P2 必须 merged ✓）。遗留：ARQ-261029 上游 ※ 写入侧待派单；远程发布（release-pr --publish）待 human。
