# mutation 取证 · 261013-lang-config-thinking-fix（B1/B2）

> 执行者 = builder（agentId a10f142cb0e753b1a，2026-10-13）；builder 无 delivery/ 写权，本文由 planner 按 [派单书③] 代存（回执全文转录，未改写证据正文）。判据来源 = 冻结验收 `firmware/main/test/acceptance/lang-config-gen.acceptance.md` §3（B1/B2 与 TaskPackage mutationEvidence.perAcceptance 一一对应）。correlationId = 261013-lang-config-thinking-fix。

## B1 — 摘 THINKING 键（撤销 T1 / LG-A1）

**红证据（对账）**：
```
$ python -c "...redBaselineCommand 原文..."
refs: 49
producible: 70
UNDEFINED: ['THINKING']
```

**红证据（增量重生成后重编译，编译失败 verbatim）**：
```
$ python.exe c:/tmp/idf_run.py build
[17/26] Building CXX object esp-idf/main/CMakeFiles/__idf_main.dir/application.cc.obj
FAILED: esp-idf/main/CMakeFiles/__idf_main.dir/application.cc.obj 
C:/AI/KidPal-pod-m2/firmware/main/application.cc: In lambda function:
C:/AI/KidPal-pod-m2/firmware/main/application.cc:747:59: error: 'THINKING' is not a member of 'Lang::Strings'
  747 |                         display->SetStatus(Lang::Strings::THINKING);
        |                                                           ^~~~~~~~
ninja: build stopped: subcommand failed.
ninja failed with exit code 1
=== B1 BUILD EXIT: 2 ===
```
编译器即时接住未定义键，不静默过（LG-A5 形态下无手补头兜底）。

**还原复绿证据**：
```
$ python -c "...对账..."   →  UNDEFINED: []
$ python.exe c:/tmp/idf_run.py build
=== B1 RESTORE BUILD EXIT: 0 ===
THINKING hit: 1
```

## B2 — 摘 CMakeLists DEPENDS 的 en-US 条目（撤销 T2-③ / LG-A4）

**红证据（touch en-US 后增量构建，生成头不更新）**：
```
B2 T0 header mtime: 1791904596 (23:16:36)
B2 en-US touched:   1791904722 (23:18:42)
$ python.exe c:/tmp/idf_run.py build   # DEPENDS 已摘 en-US
=== B2 BUILD EXIT: 0 ===
B2 T1 header mtime: 1791904596 (23:16:36)   → mtime 不变
B2 RESULT: header NOT regenerated (mtime unchanged) -> RED as expected
B2 THINKING hit: 1
```
touch en-US 后 ninja 不重跑生成命令（生成头 mtime 与内容均不变）——LG-A8 断言红。

**还原复绿证据（LG-A8 复验）**：
```
LG-A8 re-verify: header mtime before: 1791904596 (23:16:36)
LG-A8 re-verify: en-US touched:        1791904825 (23:20:25 附近)
=== LG-A8 RE-VERIFY BUILD EXIT: 0 ===
LG-A8 re-verify: header mtime after: 1791904889 (23:21:29)   → after > touch
LG-A8 RE-VERIFY: REGENERATED (after > touch) -> GREEN
THINKING hit: 1
```

## MUTATION 残留自查

```
$ grep -rn "MUTATION" firmware/main/CMakeLists.txt firmware/scripts/gen_lang.py firmware/main/assets/locales/{zh-CN,en-US}/language.json firmware/.gitignore
（无输出）  MUTATION grep exit: 1 → 零残留 ✓
```
两处变异均为临时编辑后还原，终态 diff 与意图态逐字一致。

---

# 追加变异取证 · M-A2/A3/A5/A7（2026-10-13，builder aba59c5632165d674，planner 代存）

> mutation-check（verifier r2）要求每个 acceptance id 均有变异判别。planner 裁定：LG-A1/A4/A9 由 B1/B2 同源承载；LG-A2/A3/A5/A7 真实变异实跑如下。四变异均临时编辑后精确还原（终态 git diff --stat 与变异前基线逐字一致：12 文件同集 286 insertions/487 deletions 不变，MUTATION 残留 grep 零命中）。

## M-A2（撤销 T2-①）— LANG_HEADER 回退源树 → 承载 LG-A2（兼证 LG-A5 判据 1）

红证据（增量构建 exit 0，生成物写源树）：
```
[1/7] Generating zh-CN language config
Output file path: C:/AI/KidPal-pod-m2/firmware/main/assets/lang_config.h
Successfully generated language config file: C:/AI/KidPal-pod-m2/firmware/main/assets/lang_config.h
=== M-A2 BUILD EXIT: 0 ===
```
红证据（源树回潮 + LG-A2/LG-A5 §6 assert 红）：
```
$ ls -la firmware/main/assets/lang_config.h
-rw-r--r-- 1 15802 197609 10697 Oct 13 23:57 firmware/main/assets/lang_config.h
THINKING hit in source tree header: 1
$ LG-A2 §6.2 assert
AssertionError
=== M-A2 LG-A2 assert exit: 1 ===
$ LG-A5 §6.5 assert
AssertionError
=== M-A2 LG-A5 assert exit: 1 ===
```
还原：CMake 行改回 ${CMAKE_CURRENT_BINARY_DIR} + rm -f 源树生成物。复绿：
```
=== M-A2 RESTORE BUILD EXIT: 0 ===
LG-A2 PASS sites=62 (dedup=1)   (exit 0)
LG-A5 PASS                       (exit 0)
$ ls firmware/main/assets/lang_config.h → No such file or directory
```

## M-A3（撤销 T2-②）— 摘除 gen_lang.py --assets-dir → 承载 LG-A3

红证据（增量构建红，CMake 仍传 --assets-dir）：
```
FAILED: esp-idf/main/assets/lang_config.h
gen_lang.py: error: unrecognized arguments: --assets-dir C:/AI/KidPal-pod-m2/firmware/main/assets
ninja: build stopped: subcommand failed.
=== M-A3 BUILD EXIT: 2 ===
```
还原：add_argument 行原样加回。复绿：
```
[1/26] Generating zh-CN language config
Successfully generated language config file: C:/AI/KidPal-pod-m2/firmware/build/esp-idf/main/assets/lang_config.h
Project build complete. ...
=== M-A3 RESTORE BUILD EXIT: 0 ===
```

## M-A5（撤销 T2-④ ignore 撤行）— 承载 LG-A5

红证据（.gitignore 回填 main/assets/lang_config.h 于 :12）：
```
$ grep -n "main/assets/lang_config.h" firmware/.gitignore
12:main/assets/lang_config.h
$ LG-A5 §6.5 assert
AssertionError
=== M-A5 LG-A5 assert exit: 1 ===
```
还原：删该行。复绿：
```
$ grep -n "main/assets/lang_config.h" firmware/.gitignore   → grep exit 1（零命中）
$ LG-A5 §6.5 assert → LG-A5 PASS（exit 0）
```

## M-A7（撤销 T2-② host 面）— 承载 LG-A7

红证据（用 M-A3 摘除态 gen_lang.py）：
```
$ python firmware/scripts/gen_lang.py --language zh-CN --output C:/tmp/lg-ma7/main/assets/lang_config.h --assets-dir C:/tmp/lg-accept-red/main/assets
usage: gen_lang.py [-h] --language LANGUAGE --output OUTPUT
gen_lang.py: error: unrecognized arguments: --assets-dir C:/tmp/lg-accept-red/main/assets
=== M-A7 HOST EXIT: 2 ===
```
还原：随 M-A3 还原。复绿（同命令复跑）：
```
=== M-A7 HOST EXIT: 0 ===
THINKING hit: 1
THINKING line:         constexpr const char* THINKING = "思考中...";
```

## 终态核对

grep MUTATION 零残留（exit 1 无输出）；收尾 §6.1–6.6 六条 assert 复跑全 PASS（LG-A6 PASS refs=49 producible=71 UNDEFINED: []）。
