---
id: TECH-261013-lang-config-thinking-key-regen-landmine
title: 固件 lang_config.h 生成物与 locale 源不一致——Lang::Strings::THINKING 键只存在于手工补丁的未跟踪生成头，任何触发重新生成的干净构建（新 worktree 首建 / json mtime 更新后构建）必编译红
domain: tech
status: deferred
severity: MAJOR
owner: dev 批（固件面）
trigger: 下次固件批动工前对账处理；或新 worktree/clone 首建踩雷时
opened: 2026-10-13
closed: ""
closedBy: ""
paths: ["firmware/main/assets/lang_config.h", "firmware/scripts/gen_lang.py", "firmware/main/assets/locales/zh-CN/language.json", "firmware/main/assets/locales/en-US/language.json", "firmware/main/display/lcd_display.cc", "firmware/main/application.cc"]
source: 2026-10-13 主线程 dev-loop 承重轮（261013-qq-schedule-write-closure，server-only 零固件改动包）踩中：verify.mjs final 相位 firmware-build 闸真编译红，build 日志实录见下
handoff: dev 批 / 固件面（本条目只登记不实施）
---

**证据**（2026-10-13 实录）：

1. **编译错误**（`firmware/build/log/idf_py_stdout_output_12932:2456`）：`C:/AI/KidPal-pod-m2/firmware/main/display/lcd_display.cc:2180:50: error: 'THINKING' is not a member of 'Lang::Strings'`；引用点 = [lcd_display.cc:2180](../../../firmware/main/display/lcd_display.cc)（`strcmp(status, Lang::Strings::THINKING)`）+ [application.cc:747](../../../firmware/main/application.cc)（`display->SetStatus(Lang::Strings::THINKING)`）。
2. **引用来源**：两处引用由 `8a3fc07`（260915-m1-k3-voice-round-ui）引入——实查该 commit **零 locale/lang 文件改动**（`git show --stat 8a3fc07 | grep -iE 'lang|locale|assets'` 空），即引用加进来时没加 locale 键。
3. **locale 源缺键**：`grep THINKING firmware/main/assets/locales/**/language.json` = 零命中（全部 locale 皆无）；`firmware/scripts/gen_lang.py` 生成源 = en-US 基准 + locale 覆盖，故**重新生成必不产 THINKING**。
4. **旧生成头靠手工补丁存活**：生成头 `firmware/main/assets/lang_config.h` 是 **untracked 自动生成物**（文件头「Auto-generated」；`git log -- <该文件>` 空）。同仓 pod-m1 worktree 的旧副本含第 60 行 `constexpr const char* THINKING = "思考中...";`——与生成器输出唯一的逐行差异（full diff = 单行），即当年以**手工插入生成物**方式绕过（untracked 文件，git 不可见、无留痕）。
5. **触发爆炸的条件**：generated header mtime 落后于 locale json mtime 时，构建期 CMake 重新生成 → 覆盖掉手补行 → 编译红。本次实录：json mtime 2026-10-13 09:35（本 worktree 检出所致）> 旧 header → 承重轮构建 12:34:41 重新生成 → 错误爆发。**推论：任何新 clone/worktree 首次构建（json 全新检出、header 缺席或陈旧）都会踩中**——Pod launch 面首建、运维重建的必炸雷。
6. **连带**：构建亦对跟踪文件 `firmware/sdkconfig` 追加空节（「Image Effects Configuration」块，Kconfig 重新生成的装饰性差异）——每构建一次即脏，主线程已 checkout 还原；属同次构建的副产物、非本源缺陷。

**影响面**：拦死一切干净环境的固件编译；表现为「别的批次都能过、某次突然炸」的归因困难型故障（本次即一个零固件改动的 server 包承重轮踩中，白烧约 9 分钟构建 + 归因链）。手工补丁式绕过使缺陷对 git 全不可见、跨 worktree 靠拷贝传播（pod-m1 → pod-m2 的修复拷贝亦然）。

**建议改动**（候选，交 dev 批 review、不自批）：① 正源修复 = 把 `THINKING` 键补进 `locales/{zh-CN,en-US}/language.json`（与 LISTENING/SPEAKING/STANDBY 同节），使生成器可再产出——顺带做一次**系统性对账**：枚举固件源码全部 `Lang::Strings::*` 引用 vs 生成器产出键集，扫同型「已用未定义」漏键（本键大概率不是唯一一颗）；② 生成物治理 = 评估 `lang_config.h` 改为构建期必生成（CMake add_custom_command 挂 locale json 依赖，保证任何环境自洽）或把生成头入版本控制留制作纪律——两者取一，避免「untracked 手补」形态再现；③ 在固件构建 checklist / launch-prep skill 加一句首建自检（header 缺 THINKING 键即停并指向本条目）。

**风险**：riskTier: low-medium——纯固件源码/生成链修补，不触契约、不触 `.claude` 治理面；但需真机/构建复证（host 构建绿 + 至少一次全量编译）。owner = dev 批（固件面）。
