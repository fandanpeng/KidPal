# progress · 261103-star-points-story-cover（workstream 活文档，append-only）

> 拆包顺序链与 launch 面事实的家。设计权威 = [PROPOSAL.md](PROPOSAL.md)（FROZEN v1.0）；评审 = [review.md](review.md)；顺序链 = [task_plan.md](task_plan.md)。

## 2026-11-04 · 拆包与首批 launch 面（K0a ∥ K0 并行对）

**调度判定**：K0a（261104-k0a-four-page-skeleton，mergeOrder 70）与 K0（261104-k0-points-e2e-spike，71）无互相依赖（dependsOn 均 []），按 human 2026-11-04 对齐的并行对同时 launch；merge 序钉死 K0a 先合，K0 的共享 CMake 追加 / host build / device build / 真机终验 / ready-to-merge 全部后置到 K0a merged 后（LP8）。K0a/K0 合并后回 WIP=1：K1→K2→K3→K4→K5。树分配：K0a → 池树 `C:/AI/KidPal-pod-m2`（host 构建热缓存，固件骨架最合适）；K0 → 池树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`（server 为主）。两树 2026-11-04 交接卫生完成：tracked 净、常驻分支复位到 main=22b30654（旧分叉均为已 merged 包的树本地历史，board 在案）、`firmware/main/test/host_app/sdkconfig` 改名 `sdkconfig.pre-launch-bak-20261104`（可逆，防 TECH-261014 陈旧假红；确认重生成无误后可删）。专属图索引随树复用：`C-AI-KidPal-pod-m2` / `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 均在册（list_projects 实查 2026-11-04）。

**门结果**：拆包门 1 两轮 = BLOCK（F-01..F-05）→ planner 六项修正 → 聚焦复检 **GREEN**（verdict 落 [review.md](review.md)「门 1」节）；settings-lint 4 files 0 RED 0 WARN；check-launch 两包 blocking 均空（--base-ref main）；human 过目切法通过并授权 commit（2026-11-04）；commit 22b30654。

**基线锚（带锚引用三件齐；判停 = 消费前 git diff --quiet <SHA> HEAD -- <面域>）**：

- firmware host 面 = **265 Tests / 3 Failures / 0 Ignored** @ SHA `2f2c5871`（2026-11-02 实测；命令 = WSL `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` + `wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf`；3 红全 p1_auxbar AB-A1/A2/A4 存量族；落盘锚 = `delivery/workstreams/261028-font-typography-scaleup/progress.md` 2026-11-02 P3 段）。判停 2026-11-04：`git diff --quiet 2f2c5871 HEAD -- firmware/` exit 0 → 引用不重跑。
- server 面 = **395 passed / 0 failed / 33 subtests passed / 9 deselected** @ SHA `6ce77123`（2026-11-02 实测；命令 = `python -m pytest -q --import-mode=importlib -m "not live" <pod 树>/server/acceptance`；落盘锚 = 同上 progress.md 2026-11-02 P2 段 server 锚行）。判停 2026-11-04：`git diff --quiet 6ce77123 HEAD -- server/` exit 0 → 引用不重跑。
- 本两包验收红基线（acceptance-author 新冻结齿）恒现跑，无缓存可言。

**human 遗留项**：① launch 命令 human 自跑（两张启动卡已落各包目录）；② K0 的两笔真实 QQ canary 需 human 从真实家长 QQ 发送（LP7 裁定路径：自包含明确指令 + canary token；禁直编 state 文件/禁终端 CLI 冒充/禁本地注入/禁提前建 K3 skill）；③ K0a merged 后 K0 才做共享 CMake 追加与真机终验（LP8）；④ K0 真机轮与 QQ 轮的设备操作窗口需 human 在场。
