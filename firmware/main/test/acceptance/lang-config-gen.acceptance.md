# 冻结验收 — TaskPackage 261013-lang-config-thinking-fix（subsystem: firmware/lang-generation-chain）

> **授权链**：TaskPackage `delivery/taskpackages/261013-lang-config-thinking-fix/taskpackage.json`（sourceType=regression，upstream = TECH-261013-lang-config-thinking-key-regen-landmine）+ plan.md（architect /triage-change reviewed plan，2026-10-13，同会话 return；human 触点①确认 2026-10-13）。本文件由 acceptance-author 冻结于 2026-10-13（HEAD 0ee420c，分支 dev/261013-lang-config-thinking-fix）。
> **状态：FROZEN（冻结即冒烟已执行——见 §1 红基线节，对账命令与 host 生成命令当场实跑，执行通道 error = 0）**。
> 本文件是 builder/fixer 只读的冻结验收。「做完」= 下述每条断言的可机跑命令落地判绿。本包为**编译层修复**（locale 源 + gen_lang.py + main/CMakeLists.txt 生成链），无 `.acceptance.c` 承载面——验收断言 = 可机跑命令 + 精确期望值，逐条绑 plan T1/T2/T3/T4。
>
> **真机轮豁免声明**：真机验证 deferred，不进本包验收线（plan T4 已裁）——本修复为编译层修复，行为面与手补头存活时代等价（`8a3fc07` 接线已在）；思考页真机表现归 Phase-1 语音轮 workstream（requirements v1.12 §2.2 已标 Phase 1 修复项）。
>
> **事实修正注记（冻结实测）**：plan 记「include 形态枚举（19 处）」，冻结时点实测全树 `#include "assets/lang_config.h"` 站点 = **62 处**（11 处 main 子树 + 51 处 boards/，`grep -rln` 口径），且**字面量形态唯一**（`grep -rhoE` 去重 = 仅一种）。断言按「字面量形态唯一 + 站点零改动」钉（LG-A2），不按具体数钉；62 为冻结时点快照，builder 落地后重数即得比对基线。
>
> **契约条目 ↔ 齿的强度申报**：本包 sourceRefs/contractRefs 不含任何被消费的契约条目（`docs/contracts/interface-contract.md` 为 bare 整文件引用，locale JSON 与 lang_config.h 均为板端私有资产，无 wire/契约面承载）——本包断言全部为**构建链行为级**断言（对账差集 / 生成产物内容 / 构建排序副作用），无机制级契约齿申报义务；行为等价假设：生成头迁 build 目录后 include 解析路径由「源树相对 main/」变「binary dir」，二者对同一字面量的解析结果等价，该假设由 LG-A8/LG-A9 全量编译绿实测兜住，非仅凭推理。

---

## 0. Coverage map（plan 任务 → 小节）

| plan 任务 | 本文件条目 | 断言内容 |
|---|---|---|
| T1 正源补 THINKING 键 | LG-A1 | zh-CN `"THINKING": "思考中..."` + en-US `"THINKING": "Thinking..."`，逐字判定 |
| T2-① 生成头迁 build 目录 | LG-A2 | `LANG_HEADER` = `${CMAKE_CURRENT_BINARY_DIR}/assets/lang_config.h` + `INCLUDE_DIRS` 追加 binary dir + 62 处 include 站点零改动 |
| T2-② gen_lang.py `--assets-dir` | LG-A3 | 可选参在位 + 向后兼容（骨架双跑） + CMake 命令行传参 |
| T2-③ DEPENDS 补全 + 排序接线 | LG-A4 | DEPENDS 补 en-US json + `target_sources`/`set_source_files_properties` 挂组件目标 + `add_custom_target(lang_header ALL)` 撤除 |
| T2-④ 收尾（删手补头 + 撤 ignore 行） | LG-A5 | 源树 `firmware/main/assets/lang_config.h` 不存在 + `.gitignore:11` 忽略行已撤 |
| T3 全量对账 | LG-A6 | 引用集 ⊆ 可产出集，差集 = `[]` |
| T4 host 生成复现 | LG-A7 | 骨架目录 `--assets-dir` 产物含 THINKING + 不传参向后兼容 |
| T4-① 增量复现 | LG-A8 | touch en-US json → 增量构建 → build 目录生成头再生成 |
| T4-② 全量首建 | LG-A9 | 删 `firmware/build` → 全量首建绿 + build 目录生成头含 THINKING |
| （边界/错误/变异） | §3 B1–B4 | 键摘除 / DEPENDS 摘除 / 生成失败语义保留 / 形态回归必须红 |

---

## 1. 红基线（acceptance-author 独立复跑，2026-10-13，HEAD 0ee420c）

### 1.1 T3 对账红（本雷本体）

**命令**（TaskPackage `redBaselineCommand`，cwd = 仓根）：

```bash
python -c "import json,os; refs={l.strip().split('::')[-1] for l in os.popen('grep -rhoE \"Lang::(Strings|Sounds)::[A-Z_0-9]+\" firmware/main')}; prod=set(); [prod.update(k.upper() for k in json.load(open(f'firmware/main/assets/locales/{loc}/language.json',encoding='utf-8')).get('strings',{})) for loc in ['zh-CN','en-US']]; [prod.update('OGG_'+os.path.splitext(f)[0].upper() for f in os.listdir(sub) if f.endswith('.ogg')) for sub in ['firmware/main/assets/locales/zh-CN','firmware/main/assets/locales/en-US','firmware/main/assets/locales/common','firmware/main/assets/common'] if os.path.isdir(sub)]; print('refs:',len(refs)); print('producible:',len(prod)); print('UNDEFINED:', sorted(refs-prod))"
```

**逐字输出**（独立复跑）：

```
refs: 49
producible: 70
UNDEFINED: ['THINKING']
```

**判定：红（确认本雷）**。差集 = `['THINKING']`，与 triage 期及 planner intake 基线逐字一致。为什么红对应本雷：`Lang::Strings::THINKING` 被两处引用（`firmware/main/application.cc:747` = `display->SetStatus(Lang::Strings::THINKING)`；`firmware/main/display/lcd_display.cc:2180` = `strcmp(status, Lang::Strings::THINKING)`），而 zh-CN / en-US 两 locale 源均无该键（`grep THINKING firmware/main/assets/locales/{zh-CN,en-US}/language.json` 零命中，实测 False/False）——生成器（en-US 基准 ∪ 当前语言覆盖）不可产出该键，现编译存活**仅**依赖 untracked 手补生成头 `firmware/main/assets/lang_config.h:60` 的 `constexpr const char* THINKING = "思考中...";`（该文件 `git log` 空 + `git check-ignore` 命中 `firmware/.gitignore:11`），任何触发重新生成的干净构建（新 worktree 首建 / json mtime 更新）必编译红。

### 1.2 T4 host 生成红（反向复现）

**命令**（骨架目录跑现行 gen_lang.py，cwd = 仓根）：

```bash
rm -rf /c/tmp/lg-accept-red && mkdir -p /c/tmp/lg-accept-red/main/assets/locales/zh-CN /c/tmp/lg-accept-red/main/assets/locales/en-US && cp firmware/main/assets/locales/zh-CN/language.json /c/tmp/lg-accept-red/main/assets/locales/zh-CN/ && cp firmware/main/assets/locales/en-US/language.json /c/tmp/lg-accept-red/main/assets/locales/en-US/ && python firmware/scripts/gen_lang.py --language zh-CN --output /c/tmp/lg-accept-red/main/assets/lang_config.h && grep -c "THINKING" /c/tmp/lg-accept-red/main/assets/lang_config.h
```

**逐字输出（关键行）**：

```
Loaded base language en-US with 49 strings
Language zh-CN string statistics:
  - Base language (en-US): 49 strings
  - User language: 48 strings
  - Total: 49 strings
Successfully generated language config file: C:/tmp/lg-accept-red/main/assets/lang_config.h
grep: (标准输入)... → 退出码 1（THINKING 命中数 = 0）
```

**判定：红（确认本雷）**。生成命令本身成功（exit 0）但产物中 THINKING 命中数 = 0——生成器从现行 locale 源不可产出 THINKING，与 §1.1 差集互证。骨架产物 vs 手补头的 **strings 节 diff = 恰一行** `59a60 > constexpr const char* THINKING = "思考中...";`（sounds 节差异为骨架未拷 .ogg 的 fixture 假象，非本雷）。同一骨架运行同时实证现行 gen_lang.py 的「output 路径反推 assets 目录」逻辑（gen_lang.py:54-60）——output 不落在 `<main>/assets/` 形态下即报错，T2-② 迁 build 目录必炸，故 LG-A3 钉 `--assets-dir`。

**红基线性质**：本包红基线 = 「对账红 + host 生成红 + 形态缺陷（untracked 手补头 + ignore 行）」，非「编译红」——净 HEAD 现态下手补头存活，firmware-build 闸应绿；builder 落地后以 LG-A6 差集空 + LG-A7 产物含键 + LG-A9 全量首建绿三面判定转绿。

---

## 2. 验收条目（逐条可机跑）

> IDF 构建入口（全条目统一）：`cd firmware && "C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py build`——**必须用该 venv python 调 wrapper，勿裸 python**。全量首建 ~9 分钟，长命令后台跑轮询。`firmware/build/` 不进仓。对账类命令 cwd = 仓根。

### LG-A1 — T1：两 locale 正源补 THINKING 键
`coverage: T1`
> coverage: LG-A1 (T1)marker — 判据映射见 §6.1

**命令**（cwd = 仓根）：

```bash
python -c "import json; zh=json.load(open('firmware/main/assets/locales/zh-CN/language.json',encoding='utf-8'))['strings']; en=json.load(open('firmware/main/assets/locales/en-US/language.json',encoding='utf-8'))['strings']; print(repr(zh.get('THINKING'))); print(repr(en.get('THINKING')))"
grep -n "THINKING" firmware/main/assets/locales/zh-CN/language.json firmware/main/assets/locales/en-US/language.json
```

**通过判据**：
1. 第一条命令输出恰两行：`'思考中...'` 与 `'Thinking...'`（None 即红）；
2. zh-CN 值与手补头 `firmware/main/assets/lang_config.h:60` 逐字一致：`constexpr const char* THINKING = "思考中...";`（含行尾三点省略号形态）；
3. 两键落在各自 `strings` 节内（grep 行号须在 `"strings"` 节起止之间）。

**当前状态：红**（两 locale 源均无该键）。en-US 为 gen_lang.py 基准（gen_lang.py:32-45 fallback 机制），补 en-US 即覆盖其余 35 个 locale，其余 locale 无需逐个补；zh-CN 为当前构建语言（`firmware/sdkconfig:607` `CONFIG_LANGUAGE_ZH_CN=y`）。

### LG-A2 — T2-①：生成头迁 build 目录 + include 站点零改动
`coverage: T2-①`
> coverage: LG-A2 (T2-①)marker — 判据映射见 §6.2

**命令**（cwd = 仓根）：

```bash
grep -n "LANG_HEADER" firmware/main/CMakeLists.txt
grep -n "INCLUDE_DIRS" firmware/main/CMakeLists.txt
grep -rhoE '#include\s+[^ ]*lang_config\.h' firmware/main | sort | uniq -c
grep -rln '#include "assets/lang_config.h"' firmware/main | wc -l
```

**通过判据**：
1. `set(LANG_HEADER ...)` 的路径为 `${CMAKE_CURRENT_BINARY_DIR}/assets/lang_config.h`（不再指向 `${CMAKE_CURRENT_SOURCE_DIR}` 下）；
2. `idf_component_register` 的 `INCLUDE_DIRS` 含 `${CMAKE_CURRENT_BINARY_DIR}` 追加项；
3. 第 3 条命令输出**恰一行** `62 #include "assets/lang_config.h`（字面量形态唯一，去重计数 = 1）；
4. 第 4 条命令计数 = 62（与冻结时点快照一致，即**全部 include 站点零改动**——迁移后解析路径由「相对 main/ 源树」切到 binary dir，include 字面量不许动）。

**当前状态：红**（`LANG_HEADER` 现指源树 `firmware/main/CMakeLists.txt:775`，`INCLUDE_DIRS` 无 binary dir 项）。

### LG-A3 — T2-②：gen_lang.py `--assets-dir` 可选参 + 向后兼容
`coverage: T2-②`
> coverage: LG-A3 (T2-②)marker — 判据映射见 §6.3

**命令**（cwd = 仓根；骨架用 §1.2 的 `/c/tmp/lg-accept-red` 结构，含两 locale json）：

```bash
python firmware/scripts/gen_lang.py --help 2>&1 | grep -c "assets-dir"
python firmware/scripts/gen_lang.py --language zh-CN --output /c/tmp/lg-accept-red/main/assets/lang_config.h --assets-dir /c/tmp/lg-accept-red/main/assets && grep -c "THINKING" /c/tmp/lg-accept-red/main/assets/lang_config.h
rm -rf /c/tmp/lg-accept-compat && mkdir -p /c/tmp/lg-accept-compat && cp -r /c/tmp/lg-accept-red/main /c/tmp/lg-accept-compat/main && python firmware/scripts/gen_lang.py --language zh-CN --output /c/tmp/lg-accept-compat/main/assets/lang_config.h && grep -c "THINKING" /c/tmp/lg-accept-compat/main/assets/lang_config.h
grep -n "assets-dir" firmware/main/CMakeLists.txt
```

**通过判据**：
1. `--help` 含 `assets-dir`（可选参在位，argparse 不带 `required=True`）；
2. 显式 `--assets-dir` 路径产物正常生成（判据细则见 LG-A7，本条只验参数通道通）；
3. **不传 `--assets-dir` 时旧反推逻辑保留**（向后兼容）：compat 目录命令 exit 0 且产物存在——旧调用方（output 落 `<main>/assets/` 形态）行为不变；
4. CMake 的 `add_custom_command` COMMAND 行给 gen_lang.py 传 `--assets-dir ${CMAKE_CURRENT_SOURCE_DIR}/assets`。

**当前状态：红**（gen_lang.py argparse 仅 `--language`/`--output` 两参；CMake 命令行无该参）。

### LG-A4 — T2-③：DEPENDS 补全 + 编译排序接线
`coverage: T2-③`
> coverage: LG-A4 (T2-③)marker — 判据映射见 §6.4

**命令**（cwd = 仓根）：

```bash
sed -n '/add_custom_command(/,/^)/p' firmware/main/CMakeLists.txt
grep -n "add_custom_target(lang_header" firmware/main/CMakeLists.txt
grep -nE "target_sources\(\s*\\\$\{COMPONENT_LIB\}|set_source_files_properties" firmware/main/CMakeLists.txt
```

**通过判据**：
1. `add_custom_command` 的 `DEPENDS` 列表含**三项**：`${LANG_JSON}`（当前语言 json）、`${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/en-US/language.json`（en-US 基准 json——现行缺此项，改 en-US 不触发再生成）、`${PROJECT_DIR}/scripts/gen_lang.py`；
2. `add_custom_target(lang_header ALL ...)` **不存在**（grep 零命中）——冗余 ALL custom target 撤，排序接线走组件目标单机制，勿双挂；
3. 存在 `target_sources(${COMPONENT_LIB} PRIVATE ${LANG_HEADER})` 挂接 + `set_source_files_properties(${LANG_HEADER} PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)`（或同语义 GENERATED/HEADER_FILE_ONLY 组合）——使对象文件编译排序在生成完成之后，封死「ninja 编译与生成无序」编译红。

**当前状态：红**（DEPENDS 两项漏 en-US；接线 grep 全文件零命中；`add_custom_target(lang_header ALL)` 在 `firmware/main/CMakeLists.txt:844`）。

### LG-A5 — T2-④：收尾（删源树手补头 + 撤 ignore 行）
`coverage: T2-④`
> coverage: LG-A5 (T2-④)marker — 判据映射见 §6.5

**命令**（cwd = 仓根）：

```bash
test ! -e firmware/main/assets/lang_config.h && echo "GONE"
grep -c "main/assets/lang_config.h" firmware/.gitignore
```

**通过判据**：
1. 源树 `firmware/main/assets/lang_config.h` 不存在（untracked 手补生成物删除，无历史损失——`git log` 空）；
2. `firmware/.gitignore` 中 `main/assets/lang_config.h` 行已撤（计数 = 0；冻结时点在 `:11`）——此后源树再塞生成物以 untracked 噪音显形。

**当前状态：红**（手补头在位存活中；ignore 行在位）。

### LG-A6 — T3：全量对账差集为空
`coverage: T3`
> coverage: LG-A6 (T3)marker — 判据映射见 §6.6

**命令**：TaskPackage `redBaselineCommand` 原文（§1.1，cwd = 仓根）。

**通过判据**：`UNDEFINED: []`（空列表），且 refs/producible 计数打印不报错。判定标准 = 固件全树 `Lang::Strings::*` / `Lang::Sounds::*` 引用集 ⊆ 生成器可产出集（en-US ∪ zh-CN strings 键大写 ∪ `OGG_<词干>.upper()`）。已核对照：firmware 全树 vs main 子树引用 diff = 空（main 之外零引用）；Sounds 九个引用常量全部可产出零缺口；10 个可产出未引用 strings 键（ACTIVATION / BATTERY_CHARGING / BATTERY_FULL / BATTERY_LOW / CHECKING_NEW_VERSION / CHECK_NEW_VERSION_FAILED / CONNECTION_SUCCESSFUL / HELLO_MY_FRIEND / INFO / WARNING）为无害余量，不在本包范围。

**当前状态：红**（差集 = `['THINKING']`，§1.1）。

### LG-A7 — T4：host 生成复现（骨架 + `--assets-dir` 产物含 THINKING）
`coverage: T4 host`
> coverage: LG-A7 (T4 host)marker — 终验复放形态见 §6.7

**命令**（cwd = 仓根；骨架沿用 §1.2 的 `/c/tmp/lg-accept-red`，落地后两 locale json 已含 LG-A1 的键）：

```bash
python firmware/scripts/gen_lang.py --language zh-CN --output /c/tmp/lg-accept-red/main/assets/lang_config.h --assets-dir /c/tmp/lg-accept-red/main/assets && grep "THINKING" /c/tmp/lg-accept-red/main/assets/lang_config.h
diff <(grep -E "constexpr const char\*" /c/tmp/lg-accept-red/main/assets/lang_config.h) <(grep -E "constexpr const char\*" /dev/stdin <<'EOF'
        constexpr const char* THINKING = "思考中...";
EOF
) | grep THINKING
```

**通过判据**：
1. 生成 exit 0，产物含 `constexpr const char* THINKING = "思考中...";`（zh-CN 值）；
2. en-US 语言重跑（`--language en-US`）产物含 `constexpr const char* THINKING = "Thinking...";`（基准语言自身可产出）；
3. strings 常量区（`constexpr const char*` 行集）相对「仅缺 THINKING 的旧产物」的增量 = 恰一行 THINKING（triage 反向复现的镜像，无其它键意外增删）。

**当前状态：红**（§1.2 实测产物 THINKING 命中 = 0）。

### LG-A8 — T4-①：增量复现（touch en-US → 再生成，验 DEPENDS 补全）
`coverage: T4-①`
> coverage: LG-A8 (T4-①)marker — 终验复放形态见 §6.8

**命令**（cwd = `firmware/`；先全量首建一次拿基线，见 LG-A9；此后）：

```bash
touch main/assets/locales/en-US/language.json
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py build
stat -c '%Y' build/esp-idf/main/assets/lang_config.h
```

**通过判据**：
1. 增量构建 exit 0；
2. `build/esp-idf/main/assets/lang_config.h`（= `${CMAKE_CURRENT_BINARY_DIR}/assets/lang_config.h`，main 组件 binary dir）的 mtime **晚于** touch 时刻——即 en-US json（非当前语言、是基准）改动触发了 lang_config.h 再生成，DEPENDS 补 en-US 基准 json 生效；产物中 THINKING 键仍在（`grep -c THINKING build/esp-idf/main/assets/lang_config.h` = 1）。

**当前状态：红**（现行 DEPENDS 无 en-US 项，touch en-US 后 ninja 不重跑生成命令——builder 落地 LG-A4 后本条转绿）。

### LG-A9 — T4-②：全量首建绿（新 worktree 首建形态 = 本雷触发条件）
`coverage: T4-②`
> coverage: LG-A9 (T4-②)marker — 终验复放形态见 §6.9

**命令**（cwd = `firmware/`）：

```bash
rm -rf build
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py build
test -s build/esp-idf/main/assets/lang_config.h && grep -c "THINKING" build/esp-idf/main/assets/lang_config.h
```

**通过判据**：
1. 全量首建 exit 0（~9 分钟量级，长命令后台跑轮询）；
2. build 目录生成头存在且非空，THINKING 命中 = 1——**删 build 后首建必然自生成含 THINKING 的头**，本雷触发条件（干净环境首建）正式封死；
3. 源树 `firmware/main/assets/lang_config.h` 在首建前后均不存在（LG-A5 保持，生成物不回潮源树）。

**当前状态：红**（生成链不满足：现行形态下删 build 后重新生成产物必缺 THINKING——§1.1/§1.2 实证；LG-A1 至 LG-A5 全落地后本条方可转绿）。

---

## 3. 边界 / 错误断言（必须红的情形）

| 断言 | 触发条件 | 判红方式 |
|---|---|---|
| B1 键摘除（变异 perAcceptance[0]） | 从 en-US 与 zh-CN `language.json` 摘除 THINKING 键（撤销 T1），重跑 LG-A6 对账 + 增量重生成 | LG-A6 差集复现 `['THINKING']`；LG-A5 已删手补头形态下增量重编译 = `'THINKING' is not a member of 'Lang::Strings'`（编译器即时接住，未定义键不静默过） |
| B2 DEPENDS 摘除（变异 perAcceptance[1]） | `CMakeLists.txt` DEPENDS 摘除 en-US `language.json` 条目（撤销 T2-③①），touch en-US json 后增量构建 | `build/esp-idf/main/assets/lang_config.h` mtime 不更新（不再生成）——LG-A8 断言红 |
| B3 生成失败语义保留 | `gen_lang.py --language <不存在语言>`（骨架目录） | exit 1 + 报错输出——加 `--assets-dir` 重构**不得吞错**（现行 `raise FileNotFoundError` → `exit(1)` 语义保留，否则坏输入静默产空头） |
| B4 形态回归 | `firmware/main/assets/lang_config.h` 源树文件回潮，或 `firmware/.gitignore` 忽略行回潮，或 include 字面量出现第二种形态 | LG-A2 第 3 条去重计数 ≠ 1、或 LG-A5 判据不满足 → 红（untracked 手补形态不得以任何方式回归） |

B1/B2 同时是 TaskPackage `mutationRequired.perAcceptance` 的两个变异判别机制：摘键（对账差集 + 编译器双红）与摘 DEPENDS（增量不生成单红），builder 落地后按 mutationEvidence 流程实测回填 `turnedRed`。

---

## 4. qualityNotes（作者自检）

1. **断言两向可满足性**：每条红基线均对应 story 缺失的真实行为且转绿路径明确——LG-A1 补键（T1）、LG-A2/A3/A4 三处构建链正形（T2）、LG-A5 收尾、LG-A6 差集随之清空（T3 = T1 的机械投影）、LG-A7 产物含键（T1 的生成器投影）、LG-A8/A9 构建副作用（T2 的 ninja 投影）。无「永远绿」条目：B1/B2 变异重放可把 LG-A6/LG-A8 各自打红。
2. **fixture 自含性**：LG-A3/A7 的骨架 fixture（`/c/tmp/lg-accept-red`）仅依赖仓内两 locale json + gen_lang.py 自身，时间/环境无关；LG-A8/A9 依赖本机 IDF venv（`C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env`）与 `c:/tmp/idf_run.py` wrapper——host 近似假设显式声明：本包无真机面（编译层修复），IDF 全量首建即最接近真实环境的验证通道，无「fixture 缺资产 = host 覆盖差集」情形。
3. **真实副作用证据，非 mock**：LG-A8/A9 断言的是 ninja 真实构建排序与再生成 mtime/产物内容，LG-A6 是源码级引用集 vs 生成器产出集的真实对账——全链无 mock。未定义键的真实接住机制 = 编译器（B1），不靠测试替身。
4. **「补丁存活」不作为通过依据**：手补头在 LG-A5 后物理删除，任何断言不再依赖它；骨架 vs 手补头的「恰一行」diff 仅作红基线证据，不是通过判据。
5. **plan「19 处」计数修正**：冻结实测 include 站点 = 62 处（字面量形态仍唯一），LG-A2 按「形态唯一 + 计数一致」钉，具体数以冻结时点快照 62 为基线、重数比对，不写死进断言语义。
6. **tighten-only 窗口**：本文件冻结后可纯 additive 补断言 / 修自身 fixture 缺陷，禁放宽（如把 `UNDEFINED: []` 放宽成白名单）、禁改断言语义；五件防滥用件（删除行 0 / 原变异重放转红 / sha256 核销 / 范围帽 / 意图锚定）按 `docs/DEV-GOVERNANCE.md`「验收测试纪律」§6 执行。

---

## 5. 修订记录（append-only）

- **2026-10-13 修订（tighten-only 窗口行使，授权 = §4 qualityNotes 第 6 条「修自身 fixture 缺陷」）**：修正 LG-A3 判据 3 fixture 命令的路径 bug——原命令 `cp -r /c/tmp/lg-accept-red /c/tmp/lg-accept-compat/` 把源目录拷进已存在的 compat 目录内，落在 `/c/tmp/lg-accept-compat/lg-accept-red/main/assets/...`，随后命令读 `/c/tmp/lg-accept-compat/main/assets/lang_config.h` 必 FileNotFoundError → 逐字命令恒 exit 1（旧代码同样失败，非实现缺陷）。修正为 `cp -r /c/tmp/lg-accept-red/main /c/tmp/lg-accept-compat/main`。判据语义不变（不传 `--assets-dir` 时旧反推逻辑 exit 0 + 产物含 THINKING）；builder（commit a10f142cb0e753b1a）落地验证期发现，已按判据意图用修正版验证绿；作者本日实跑修正后命令确认 exit 0 + THINKING 命中 = 1。未弱化任何其它断言，未改其它条目。
- **2026-10-13 第二次修订（机检适配，纯 additive，授权 = verifier r1 闸反馈 + §4 第 6 条 tighten-only 窗口同款授权链）**：verifier round 1 `acceptance-coverage` 机检闸红（`verifier-evidence/rounds/r1.json`：`Acceptance test file has no recognizable assertion`——本文件断言全为 shell 判定命令与中文判据行文，未命中机检调用形态正则）。修订内容：① §2 九个条目小节各补一行机检覆盖标记（LG-A1..LG-A9 全在场；形如 `> coverage: LG-A1 (T1)marker — …`——机检 acceptanceId 实为 `LG-Ax (Tx)` 整段且标记正则尾随 `\b`，须以 ASCII 词无空格续接，形状约束注记见 §6 前言）；② 新增 §6 机检断言汇总——LG-A1..LG-A9 各配一条 python -c assert 形态可机跑断言命令（`assert(` 形态命中 `.claude/scripts/acceptance-coverage-check.mjs` 的 `\bassert\s*[.(]` 分支），判定语义与对应条目既有判据逐字等价。防自欺自查：未删改任何既有断言行/判据/「当前状态」行；无数值/路径放宽；六条源码态断言（A1–A6）作者当场实跑全 PASS（逐字输出见 §6.10）。

---

## 6. 机检断言汇总（verifier 闸适配，additive，2026-10-13 第二次修订落盘）

> **来历**：verifier round 1 `acceptance-coverage` 机检闸红（`delivery/taskpackages/261013-lang-config-thinking-fix/verifier-evidence/rounds/r1.json`：`Acceptance test file has no recognizable assertion`）——本文件 §2 判定均为 shell 判定命令 + 中文判据行文，未命中 `.claude/scripts/acceptance-coverage-check.mjs` 的调用形态断言正则；§2 的 `coverage: T-x` 标记亦非机检所需 `coverage: <acceptanceId>` 形态。本节按 §4 第 6 条 tighten-only 窗口**纯 additive** 补齐：条目级机检覆盖标记已落 §2 各小节（9 id 全在场）；本节给 LG-A1..LG-A9 各配一条 **python -c assert 形态**的可机跑断言命令——`assert(` 写法命中机检正则 `\bassert\s*[.(]` 分支，且真实可跑（非占位）。断言强度语义依 `.claude/references/acceptance-evidence.md`：本节为 §2 判据的**机检等价重述**，不替代 §2 命令，也不放大验收范围。
>
> **覆盖标记形态注记（机检形状约束）**：本 TaskPackage `acceptance[]` 条目为整句形态（`LG-Ax (Tx): 描述`），机检 `extractShortId` 冒号提取不命中、`shortAcceptanceRef` 截冒号前段 → 机检 acceptanceId = `LG-Ax (Tx)`（含括号后缀）；标记正则以 `\b` 收尾，而 id 末字符 `)` 非 word char——标记行必须在 `(Tx)` 后**无空格直接续接 ASCII 词**（本文件统一用 `marker`）才能命中（`> coverage: LG-A1 (T1)marker` ✓；`> coverage: LG-A1` / `> coverage: LG-A1 (T1)` ✗，后者因尾随 `\b` 不成立）。故 §2 标记行形如 `> coverage: LG-A1 (T1)marker — 判据映射见 §6.1`，`marker` 为纯机检形状胶水词，非断言语义。
>
> **执行约定（本节全部命令适用）**：
> 1. cwd = 仓根；`PYTHONUTF8=1` 前缀为 cp1252 控制台下断言/打印中文字面量所需，勿省；
> 2. 命令外层为 bash 双引号，引用 CMake 变量写作 `\${...}`、内嵌双引号写作 `\"`（按本文原样拷贝执行即可，bash 解析后 python 源码收到 `${...}` 与 `"`）；
> 3. 判据中依赖构建/骨架运行的面（LG-A3 判据 2/3 骨架双跑、LG-A8/A9 的构建动作本身）仍按 §2 命令执行；本节只钉可机跑的静态/产物断言面，逐条注明与 §2 判据的对应关系。

### 6.1 LG-A1（等价判据 1/2/3：两键值逐字 + strings 节归属）

```bash
PYTHONUTF8=1 python -c "import json; zh=json.load(open('firmware/main/assets/locales/zh-CN/language.json',encoding='utf-8'))['strings']; en=json.load(open('firmware/main/assets/locales/en-US/language.json',encoding='utf-8'))['strings']; assert(zh['THINKING']=='思考中...'); assert(en['THINKING']=='Thinking...'); print('LG-A1 PASS')"
```

`['strings']` 直接取键即判据 3（键不在 strings 节即 KeyError 红）；两处 assert 值逐字即判据 1/2（zh 值 = 手补头 `lang_config.h:60` 的 `思考中...` 字面量，缺键 `zh['THINKING']` 即红，比 §2 的 `get()` 命令更严——语义相同、失败面更早）。

### 6.2 LG-A2（等价判据 1/2/3/4：LANG_HEADER 指 binary dir + INCLUDE_DIRS 追加 binary dir + 字面量形态唯一去重 = 1 + 源站点 62 零改动）

```bash
PYTHONUTF8=1 python -c "import os,re; cmake=open('firmware/main/CMakeLists.txt',encoding='utf-8').read(); lh=[l for l in cmake.splitlines() if 'set(LANG_HEADER' in l]; assert(lh and '\${CMAKE_CURRENT_BINARY_DIR}/assets/lang_config.h' in lh[0] and '\${CMAKE_CURRENT_SOURCE_DIR}' not in lh[0]); idl=[l for l in cmake.splitlines() if 'INCLUDE_DIRS' in l]; assert(any('\${CMAKE_CURRENT_BINARY_DIR}' in l for l in idl)); paths=[(r+'/'+f).replace(os.sep,'/') for r,d,fs in os.walk('firmware/main') for f in fs]; paths=[p for p in paths if '/test/acceptance/' not in p]; ms=[m for p in paths for m in re.findall(r'#include\s+\S*lang_config\.h',open(p,encoding='utf-8',errors='ignore').read())]; assert(set(ms)=={'#include \"assets/lang_config.h'} and len(ms)==62); print('LG-A2 PASS sites=%d (dedup=1)'%len(ms))"
```

**自引用污染注记**：本验收文件自身（`firmware/main/test/acceptance/lang-config-gen.acceptance.md`）行文中引用 include 字面量形态（头注「事实修正注记」+ §2 命令），不排除会把站点计数抬到 63 文件 / 65 命中——故本命令的源站点口径显式排除 `/test/acceptance/` 子树，62 = 11 main 子树 + 51 boards，与冻结快照口径逐字一致（头注同）；`os.walk` 扫描与 §2 的 `grep -rhoE`/`grep -rln` 口径等价（作者实测同值 62、同单一形态）。

### 6.3 LG-A3（等价判据 1/4：`--help` 含 assets-dir 且非 required + CMake COMMAND 行传参）

```bash
PYTHONUTF8=1 python -c "import sys,subprocess; h=subprocess.run([sys.executable,'firmware/scripts/gen_lang.py','--help'],capture_output=True,text=True).stdout; assert('assets-dir' in h); src=open('firmware/scripts/gen_lang.py',encoding='utf-8').read(); arg=[l for l in src.splitlines() if 'add_argument(\"--assets-dir\"' in l]; assert(arg and 'required=True' not in arg[0]); cmake=open('firmware/main/CMakeLists.txt',encoding='utf-8').read(); cmd=[l for l in cmake.splitlines() if '--assets-dir' in l]; assert(len(cmd)==1 and '--assets-dir \"\${CMAKE_CURRENT_SOURCE_DIR}/assets\"' in cmd[0]); print('LG-A3 PASS')"
```

判据 2/3（显式传参骨架生成通 + 不传参向后兼容 exit 0）为运行时行为面，按 §2 命令执行（§5 首条修订记录：修正版 compat 命令作者实跑 exit 0 + THINKING 命中 1）。

### 6.4 LG-A4（等价判据 1/2/3：DEPENDS 三项 + `add_custom_target(lang_header` 零命中 + 接线两行在位）

```bash
PYTHONUTF8=1 python -c "cmake=open('firmware/main/CMakeLists.txt',encoding='utf-8').read(); i=cmake.find('add_custom_command('); j=cmake.find('DEPENDS',i); k=cmake.find('COMMENT',j); dep=cmake[j:k]; assert('\${LANG_JSON}' in dep and 'assets/locales/en-US/language.json' in dep and 'scripts/gen_lang.py' in dep); assert('add_custom_target(lang_header' not in cmake); props=[l for l in cmake.splitlines() if 'set_source_files_properties' in l]; assert(props and '\${LANG_HEADER}' in props[0] and 'GENERATED' in props[0] and 'HEADER_FILE_ONLY' in props[0]); assert(any('target_sources' in l and '\${COMPONENT_LIB}' in l and '\${LANG_HEADER}' in l for l in cmake.splitlines())); print('LG-A4 PASS')"
```

`dep` 取首个 `add_custom_command(` 块的 `DEPENDS` 段（至 `COMMENT` 止），三项断言与判据 1 的三项清单逐字对应；`add_custom_target(lang_header` 全文件零命中即判据 2；`set_source_files_properties` 行含 `${LANG_HEADER}` + `GENERATED` + `HEADER_FILE_ONLY`、`target_sources` 行含 `${COMPONENT_LIB}` + `${LANG_HEADER}` 即判据 3 的 GENERATED/HEADER_FILE_ONLY 组合挂接。

### 6.5 LG-A5（等价判据 1/2：源树生成头不存在 + ignore 行计数 = 0）

```bash
PYTHONUTF8=1 python -c "import os; assert(not os.path.exists('firmware/main/assets/lang_config.h')); gi=open('firmware/.gitignore',encoding='utf-8').read(); assert(sum('main/assets/lang_config.h' in l for l in gi.splitlines())==0); print('LG-A5 PASS')"
```

### 6.6 LG-A6（等价判据：TaskPackage `redBaselineCommand` 计算原样 + 差集空断言）

```bash
PYTHONUTF8=1 python -c "import json,os; refs={l.strip().split('::')[-1] for l in os.popen('grep -rhoE \"Lang::(Strings|Sounds)::[A-Z_0-9]+\" firmware/main')}; prod=set(); [prod.update(k.upper() for k in json.load(open(f'firmware/main/assets/locales/{loc}/language.json',encoding='utf-8')).get('strings',{})) for loc in ['zh-CN','en-US']]; [prod.update('OGG_'+os.path.splitext(f)[0].upper() for f in os.listdir(sub) if f.endswith('.ogg')) for sub in ['firmware/main/assets/locales/zh-CN','firmware/main/assets/locales/en-US','firmware/main/assets/locales/common','firmware/main/assets/common'] if os.path.isdir(sub)]; bad=sorted(refs-prod); assert(not bad); print('LG-A6 PASS refs=%d producible=%d UNDEFINED: []'%(len(refs),len(prod)))"
```

refs/producible 计算与 TaskPackage `redBaselineCommand` 逐字同式（§1.1），仅把「打印差集」升级为「assert 差集为空」——判定语义与 LG-A6 判据（`UNDEFINED: []`）一致。本验收文件自身行文中的 `Lang::Strings::THINKING` 提及会进 refs 集合，但不影响判定（THINKING 落地后已入可产出集，差集仍空）。

### 6.7 LG-A7（终验复放形态——骨架产物路径参数化）

```bash
PYTHONUTF8=1 python -c "t=open('<骨架>/main/assets/lang_config.h',encoding='utf-8').read(); assert('constexpr const char* THINKING = \"思考中...\";' in t); print('LG-A7 PASS (zh)')"
```

`<骨架>` 执行时以实际骨架根的 **Windows python 可解析绝对路径**代入（本仓实测 = `C:/tmp/lg-accept-red`；git-bash 形态 `/c/tmp/...` python 不解析）；en-US 复放（判据 2）把断言值换成 `"Thinking..."`。**builder 落地实测已绿**（回执 = `delivery/taskpackages/261013-lang-config-thinking-fix/mutation-evidence.md` verbatim 记录：B1 还原复绿构建 exit 0 + THINKING 命中 1；`verify-runs.jsonl` r1 `firmware-build` 闸 build-pass），本节 assert 为终验复放形态，作者本修订日以 `C:/tmp/lg-accept-red` 实跑 PASS。

### 6.8 LG-A8（终验复放形态——对 build 产物断言）

```bash
PYTHONUTF8=1 python -c "import os; p='firmware/build/esp-idf/main/assets/lang_config.h'; assert(os.path.exists(p) and os.path.getmtime(p)>os.path.getmtime('firmware/main/assets/locales/en-US/language.json')); t=open(p,encoding='utf-8').read(); assert(t.count('THINKING')==1); print('LG-A8 PASS')"
```

mtime 判定以 en-US json 的 mtime 为 touch 时刻代理（`touch` 即改其 mtime），与判据 2「mtime 晚于 touch 时刻」等价；执行前提 = touch 后已完成一次增量构建（构建命令见 §2 LG-A8，本节不代跑构建）。builder 落地实测已绿（回执同 §6.7——mutation-evidence.md B2 还原复验绿：mtime 1791904889 > touch + REGENERATED + THINKING 命中 1），作者本修订日对现 build 产物实跑 PASS。

### 6.9 LG-A9（终验复放形态——对全量首建产物断言）

```bash
PYTHONUTF8=1 python -c "import os; p='firmware/build/esp-idf/main/assets/lang_config.h'; assert(os.path.isfile(p) and os.path.getsize(p)>0); t=open(p,encoding='utf-8').read(); assert(t.count('THINKING')==1); assert(not os.path.exists('firmware/main/assets/lang_config.h')); print('LG-A9 PASS')"
```

执行前提 = `rm -rf firmware/build` 后全量首建完成（构建命令见 §2 LG-A9，本节不代跑构建）；`build` 目录生成头非空 + THINKING 命中 1 即判据 2，源树零回潮 assert 即判据 3。builder 落地实测已绿（回执同 §6.7——B1 还原复绿构建 exit 0；r1 `firmware-build` build-pass），作者本修订日对现 build 产物实跑 PASS。

### 6.10 实跑记录（作者，2026-10-13 第二次修订日，工作区 = builder 落地后）

源码态可跑六条（§6.1–6.6）当场各跑一遍，全部 PASS；三条终验复放形态（§6.7–6.9）按各自前提注记对现产物实跑，全部 PASS：

| 断言 | 逐字输出 |
|---|---|
| LG-A1 | `LG-A1 PASS` |
| LG-A2 | `LG-A2 PASS sites=62 (dedup=1)` |
| LG-A3 | `LG-A3 PASS` |
| LG-A4 | `LG-A4 PASS` |
| LG-A5 | `LG-A5 PASS` |
| LG-A6 | `LG-A6 PASS refs=49 producible=71 UNDEFINED: []` |
| LG-A7 复放 | `LG-A7 PASS (zh)`（对 `C:/tmp/lg-accept-red` 骨架产物） |
| LG-A8 复放 | `LG-A8 PASS`（对现 `firmware/build` 产物） |
| LG-A9 复放 | `LG-A9 PASS`（对现 `firmware/build` 产物） |

**additive 自查**：未删改任何既有断言行/判据/「当前状态」行（§2 全部「当前状态：红」行原样保留——其语义为冻结时点 HEAD 0ee420c 的相对状态，不因本次修订改写）；新增 assert 判定语义与 §2 对应判据逐字等价，无数值/路径放宽；§2 既有命令与判据零触碰。
