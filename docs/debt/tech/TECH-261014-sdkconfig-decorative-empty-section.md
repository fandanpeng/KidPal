---
id: TECH-261014-sdkconfig-decorative-empty-section
title: firmware/sdkconfig 每次固件构建被追加装饰性空节（「Image Effects Configuration」块）——Kconfig 构建期回写使跟踪文件常驻脏、git status 噪音与误提交风险
domain: tech
status: deferred
severity: NIT
owner: dev 批（固件面）
trigger: 下个固件 sdkconfig / 构建治理批顺带；或 sdkconfig 脏噪再次干扰收口对账（人工排除复现）时
opened: 2026-10-14
closed: ""
closedBy: ""
paths: ["firmware/sdkconfig"]
source: TECH-261013-lang-config-thinking-key-regen-landmine 收口（261013-lang-config-thinking-fix，dev-loop integrator closeout 2026-10-14）连带第 6 条划出——原条目收口时按范围帽不进本包 commit，残留单独立账
---

**证据**（2026-10-14 本 worktree 实测）：`git diff firmware/sdkconfig` = 恒定 5 行追加——`# Image Effects Configuration` 空节（节头 + 注释行 + `# end of` 行 + 空行），落在 `CONFIG_CODEC_CJC8910_SUPPORT` 与 `# ESP IO Expander` 节之间。任何一次 IDF 固件构建（含增量）都会触发该回写：某组件 Kconfig 新注册的菜单项无任何用户可见选项，`sdkconfig` 重生成时以空节形态落盘。非功能缺陷——零运行时影响，代价纯工程卫生：跟踪文件常驻脏 → git status 对账干扰、`git add -A` 误提交风险（261013-lang-config-thinking-fix 收口即人工排除该文件，为本雷触发面的现场实例）。

**影响面**：仅仓库卫生面；不影响固件行为、不影响其它任何构建产物。

**建议改动**（候选，交 dev 批 review、不自批）：① 源头消除 = 定位注册「Image Effects Configuration」菜单的组件 Kconfig，补齐其条目默认值定义使回写收敛为空（正解，随下次动固件构建链的批顺带）；② 或评估 sdkconfig 治理惯例（`sdkconfig.defaults` 路线 / 构建后 checkout 还原脚本化），把「每构建必脏」收敛为可预期态；③ 短期纪律 = 收口/commit 前人工排除 `firmware/sdkconfig` 装饰性差异（本条目即为其留痕锚）。

**风险**：riskTier: low——仓库卫生问题，零功能面；不处理也只是持续脏噪。owner = dev 批（固件面）。
