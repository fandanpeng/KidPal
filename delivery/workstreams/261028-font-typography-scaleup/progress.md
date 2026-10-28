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
