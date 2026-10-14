---
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect（/triage-change 比例化 review，2026-10-13，同会话 return；四档裁决 ② 单包 Lite + reviewed plan 四件，human 触点①确认 2026-10-13）
date: 2026-10-13
changeId: 261013-lang-config-thinking-fix
origin:
  type: regression
  upstreamType: regression
  upstreamRef: docs/crq/tech/TECH-261013-lang-config-thinking-key-regen-landmine.md
  upstreamFrozen: ""
  workstream: ""
---

# plan · 261013-lang-config-thinking-fix

> 承载 TECH-261013-lang-config-thinking-key-regen-landmine（固件 lang_config.h 生成物与 locale 源不一致）。本 plan = /triage-change architect reviewed plan 搬运（主线程零改写）；逃生门结论 = 不触契约/spec/agent 面，留在 dev 分流道。

## T1 · 正源补 THINKING 键

`firmware/main/assets/locales/zh-CN/language.json` strings 节补 `"THINKING": "思考中..."`（与手补行逐字一致），`firmware/main/assets/locales/en-US/language.json` 补 `"THINKING": "Thinking..."`。en-US 是 gen_lang.py 的 fallback 基准（gen_lang.py:80-86），补它即覆盖其余 35 个 locale；zh-CN 是当前构建语言（`CONFIG_LANGUAGE_ZH_CN=y`）。其余 locale 无需逐个补。

## T2 · 生成链加固（正形：生成头迁 build 目录 + 全依赖化 + 编译排序接线）

`firmware/main/CMakeLists.txt:831-846` 现行三个结构缺陷的正形修法：

1. **生成头落 build 目录**：`LANG_HEADER` 改 `${CMAKE_CURRENT_BINARY_DIR}/assets/lang_config.h`；`INCLUDE_DIRS` 追加 `${CMAKE_CURRENT_BINARY_DIR}`——全部 include 形态枚举（19 处）均为同一字面量 `#include "assets/lang_config.h"`（相对 main/ 解析），加 binary dir 后零 include 站点改动。迁移后「untracked 手补生成物」形态与跨 worktree 拷贝传播载体物理消失，每个 worktree 首建必然自生成。
2. **gen_lang.py 加 `--assets-dir` 可选参**：现行脚本从 output 路径反推 assets 目录（gen_lang.py:54-60，实测 output 指别处即报错），迁 build 目录必炸——加显式 `--assets-dir`（不传时保留旧反推逻辑，向后兼容）；CMake 命令行传 `--assets-dir ${CMAKE_CURRENT_SOURCE_DIR}/assets`。
3. **DEPENDS 补全 + 编译排序接线**（编译红两个机制根因）：① 现行 DEPENDS（CMakeLists.txt:837-839）漏 en-US 基准 json——改 en-US 不触发再生成，补；② `add_custom_target(lang_header ALL)`（:844）与组件库零 `add_dependencies` 接线（grep 全文件零命中），ninja 编译与生成无序——正形 = 把 OUTPUT 以 `target_sources(${COMPONENT_LIB} PRIVATE ${LANG_HEADER})` + `set_source_files_properties(... GENERATED TRUE HEADER_FILE_ONLY TRUE)` 挂进组件目标，使对象文件编译排序在生成完成之后；冗余 ALL custom target 撤（单机制，勿双挂）。
4. **收尾**：删磁盘陈旧 `firmware/main/assets/lang_config.h`（untracked，无历史损失）；`firmware/.gitignore:11` 的 `main/assets/lang_config.h` 忽略行一并撤——此后源树再塞生成物以 untracked 噪音显形。

已驳回替代形（留档）：保留源树 output 只补 DEPENDS + 排序——改动更小但保全「untracked 手补」形态与拷贝载体，不取。

## T3 · 全量对账（验收基线在案）

判定标准 = 固件源码全部 `Lang::Strings::*` / `Lang::Sounds::*` 引用集 ⊆ 生成器可产出集，零差集。对账命令：

```bash
# 引用侧（firmware 全树 vs main 子树 diff = 空，即 main 之外零引用）
grep -rhoE "Lang::(Strings|Sounds)::[A-Z_0-9]+" firmware/main | sort -u
# 可产出侧 = en-US ∪ zh-CN strings 键（大写）∪ OGG_<词干>.upper()（locales/{zh-CN,en-US}/*.ogg + common/*.ogg）
```

triage 期实测基线：引用未定义 = **仅 THINKING 一颗**；Sounds 九个引用常量全部可产出零缺口；10 个可产出未引用 strings 键（ACTIVATION / BATTERY_CHARGING / BATTERY_FULL / BATTERY_LOW / CHECKING_NEW_VERSION / CHECK_NEW_VERSION_FAILED / CONNECTION_SUCCESSFUL / HELLO_MY_FRIEND / INFO / WARNING）无害余量不修。包内验收时重跑一次（防窗口期新引用）。

## T4 · 验证

- **host 生成复现**（快速回路）：temp 骨架目录跑 `python firmware/scripts/gen_lang.py --language zh-CN --output <tmp>/main/assets/lang_config.h --assets-dir <骨架>/assets`，断言 THINKING 在产物中（triage 期已复现反向版：骨架产物 vs 手补头 strings 节 diff = 恰一行 THINKING）。
- **全量编译（包内执行，不 defer）**：入口 = kidpal-idf-env skill——`cd firmware && "C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py build`（IDF venv python 调 wrapper，勿裸 python）。**两次干净复现**：① 改 en-US json mtime（touch）后增量构建 → 断言触发再生成（验证新 DEPENDS）；② 删 build 目录全量首建 → 断言绿（模拟新 worktree 首建 = 本雷触发条件）。
- **真机轮：deferred**，不进本包验收线——本修复为编译层修复，行为面与手补头存活时代等价（接线 8a3fc07 已在）；思考页真机表现归 Phase-1 语音轮 workstream（requirements v1.12 §2.2 已标 Phase 1 修复项）。

## 范围外残留（不进本包）

- TECH 条目连带第 6 条（构建对 `firmware/sdkconfig` 追加空节「Image Effects Configuration」装饰性噪音）：另开轻量 CRQ 或并入将来 housekeeping 批。
- TECH 建议③（launch-prep skill 首建自检）：**裁撤**——T2 正形落地后自检对象不存在；单键硬编码自检必腐化；未定义键由编译器即时指名接住。绑定条件：若实施退回「源树 output」替代形则恢复必要性（正形已裁，不预期发生）。
