# gate-tooling-gap · change-manifest 闸经 current/ 指针解析时恒判 pre-cutoff 放行

> 发现相位：`261116-channel-resident-core` 真机轮 B 修复轮（planner，2026-11-18）。
> 性质 = **gate 工具缺口**（不是本包证据缺失）；修复面在 `.claude/**`，归主线程，本 Pod 零写。
> pod-events 已记 `friction`（frictionType=`degraded`，eventId `15651860-8b1f-4130-99e6-81048fbfb4b2`）。

## 现象

`node .claude/scripts/change-manifest-check.mjs` 在本 Pod 恒输出：

```
{ "ok": true, "gate": "change-manifest", "skipped": "pre-cutoff",
  "message": "gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28 → 存量放行" }
```

而本包 changeId = `261116-…`（2026-11-16 起包），**不是**存量包；本轮实际改了 7 个 `firmware/**` 文件（其中 3 个是新建文件）。

## 根因（代码实证）

- 闸的存量兼容谓词（`change-manifest-check.mjs:145-150`）：`gitFirstCommitDate(tpPath)`，`tpPath` = `taskPackagePath()` 的解析结果。
- `taskPackagePath()`（`:113-119`）无显式参数时走 `resolveCurrentTaskPackage(...)`，解析结果是 **`delivery/taskpackages/current/taskpackage.json`**（current/ 投影是跨包复用的同一个文件）。
- 该文件自 2026-09-11 起就在版本控制里（`git log --diff-filter=A` 首提交日期），因此**任何经 current/ 指针解析的包**都会拿到 `2026-09-11 < 2026-09-28` → 恒判 pre-cutoff 放行。
- 结果：本闸在正常 Pod 流程（无显式 `--change-id` / 位置参数）下**结构性失效**，`changeManifest` 对账与「清单外实现面即红」两条判据都不会执行。

## 影响面

- 本轮及此前各轮 scoped / final 的 `change-manifest` 闸都报 ok（skipped），**不能作为「清单已对账」的证据**。
- 同类风险的姊妹闸：`taskpackage-required-check.mjs` 也共享同一 cutoff 谓词（看它解析的是哪个路径——本次实测它正常执行并报 `ok:true`，因为它的解析路径不同，但若将来同样改成 current/ 解析会复现）。

## planner 本轮的人工补位（不代替闸修复）

按闸本应执行的规则**手工对账**（`git status --porcelain` × `changeManifest[]` 逐条前缀/精确匹配）：

- 对账面 7 件：`firmware/main/application.cc`、`firmware/main/CMakeLists.txt`、`firmware/main/session_voice_watch.c`、`firmware/main/session_voice_watch.h`、`firmware/main/test/host_app/main/CMakeLists.txt`、`firmware/main/test/session_channel_resident.acceptance.c`、`firmware/main/test/session_voice_watch.test.c`。
- 结果：**清单外 0 件**；其中 `firmware/main/test/session_voice_watch.test.c` 系本轮 builder 单元测试新件，已回填 `changeManifest[]`（planner 扩单通道，留痕于 taskpackage `notes`）。

## 建议修复方向（主线程判）

1. 优先取「显式 changeId 的目录」判定，而不是解析后的投影文件路径——例如用 `delivery/taskpackages/<changeId>/taskpackage.json` 的**目录首提交**，或直接读 `taskpackage.json` 的 `changeId` 前缀日期（`261116-` → 2026-11-16）与 cutoff 比。current/ 投影文件绝不能作为包龄判据。
2. 加一条自检：当解析路径含 `taskpackages/current/` 时报 advisory（提示判据可能失真），防静默回归。
3. 修复后建议补一个「新建文件也必须覆盖」的用例：`git status --porcelain` 的 `??` 行必须参与对账（现实现已包含未跟踪文件，逻辑本身正确，缺的只是 cutoff 谓词）。