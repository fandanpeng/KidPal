---
id: TECH-261014-host-app-sdkconfig-env-drift
title: host_app sdkconfig 为 gitignored 环境态且只在构建发生的 worktree 演进——其它树（含 main）陈旧配置致冻结齿成批假红（实测 main 冷构建 10 红：CJK 字体未链接 → P1 渲染齿「渲染缝断裂」假象），非代码回归
domain: tech
status: closed
closed: 2026-10-15
severity: SHOULD
owner: dev-loop
trigger: 下次同样假红出现时按本条处置；或启动面/环境等价化批顺带把 host_app sdkconfig 纳入镜像清单
opened: 2026-10-14
paths: ["firmware/main/test/host_app/sdkconfig（gitignored 环境态）"]
source: 2026-10-14 合并后 §10a 纪律 4 复跑（main 冷构建 168/10，红全部 p1_cjk_name/p1_home_battery 渲染齿，红因逐条「渲染缝断裂/字体探针 NULL」；与分支同码 168/0 对照 → 环境差；钉死 diff = `CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK=y` vs `is not set`）
handoff: 处置口径本条自载（拷贝构建发生树的 sdkconfig + 全量重构建即愈）；升级为机制（纳入等价化镜像清单）归启动面批
---

**现象（2026-10-14 合并后冻结件复跑实测）**：merge 后在 main worktree 复跑固件 host 套件得 **168/10**，红全部集中在 `p1_cjk_name.acceptance.c` / `p1_home_battery.acceptance.c` 渲染齿，红因措辞均为「渲染缝断裂 / host 子集字体探针 NULL（runner 装配面被拆）」——与分支同码套件 168/0 矛盾。逐层排查（字体资产两树一致 → LVGL 组件两树一致 → **host_app `sdkconfig`（gitignored）diff 4 行**）钉死根因：`CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK` 在 pod worktree 为 `=y`（近期包构建时配置、随环境态滞留该树），main 侧陈旧为 `is not set` → 冷构建链接面缺 CJK 字体 → 渲染齿成批假红。

**根因**：`firmware/main/test/host_app/sdkconfig` 是 gitignored 环境态文件，**只在「发生过构建」的 worktree 演进**；launch 面的固件环境等价化（kidpal-pod-launch-prep 步骤 4）只镜像 `firmware/sdkconfig`（设备面）与 dependencies.lock，**不含 host_app 子项目 sdkconfig** → 任何新树/main 冷构建都可能带着陈旧 host 配置跑出成批假红。

**处置口径（已实测有效，2026-10-14）**：从「最近发生过构建」的 worktree 拷贝 `firmware/main/test/host_app/sdkconfig` → 全量重构建（增量对 sdkconfig 变更会触发重配置）→ 复跑。本次执行后 main 168/0 ✓。同族注意：`git checkout` 对 stat 相同文件跳过重写，eol=lf 属性回填类归一须先删后检（同日 prompt 冻结齿陈旧 CRLF 检出同款教训，TECH-261013-lf-hash-teeth-vs-windows-crlf-checkout 同族）。

**修法方向（若升级为机制）**：① 把 host_app sdkconfig 纳入 launch 面等价化镜像清单（一行 cp，同 firmware/sdkconfig 先例）；② 或把该配置项转正入仓（评估 gitignore 理由是否仍成立）。排期归启动面批，本条先记账防复发误判「代码回归」。

**风险**：riskTier: low——纯环境态口径，无代码改动；Track B 三要件齐（执行测出 / 修法明确无分叉 / 爆炸半径 = 假红误导排查时间，本日实测约 20 分钟）。

---

**结项（2026-10-15）**：治理批 261015-gov-review-lite-fast-path 落地修法方向①——launch-prep SKILL 步骤 4 镜像清单增 `firmware/main/test/host_app/sdkconfig` 一行（lite 新树随步骤 4 整体后置真机轮前），池树交接卫生补「换包即删 host_app/sdkconfig、强制从 tracked sdkconfig.defaults 重生成」（消陈旧生成态暴露面；新树冷构建从 defaults 重生成即正确，实测 defaults 37 行含 CJK 字体项）。方向②（配置转正入仓）未评、随镜像机制落地后暂无必要。凭据：PROPOSAL FROZEN v0.3（8d130a6）+ landing-verdict PASS。
