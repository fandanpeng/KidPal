---
id: ARQ-261117-change-manifest-cutoff-projection-bypass
title: change-manifest 闸的 pre-cutoff 判定读 current/ 投影文件的首提日 → cutoff 之后的所有包被判 pre-cutoff 静默跳过，changeManifest 对账等于没跑
domain: gov
status: deferred
severity: MAJOR
owner: architect（评审裁量落点；改闸归 dev-loop 执行面）
trigger: 下一个动 `.claude/scripts/change-manifest-check.mjs` 或 cutoff 谓词的治理批；或再出现一次「manifest 缺项却 verify 全绿」
opened: 2026-11-17
closed: ""
closedBy: ""
paths: [".claude/scripts/change-manifest-check.mjs", ".claude/scripts/taskpackage-required-check.mjs"]
source: ["delivery/taskpackages/261031-openclaw-cron-trigger-gating/pod-events.jsonl event 3ef694bb-7a50-4631-ba96-d28d2b223279", "delivery/retro/2026-11-17-loop-retro.md §二 T1"]
---

**证据**：261031-openclaw-cron-trigger-gating 的 friction 事件 `3ef694bb-7a50-4631-ba96-d28d2b223279` 逐字：「`change-manifest-check` 默认读 current/taskpackage.json 取 gitFirstCommitDate，因该指针文件自 2026-09-11 起就存在，任何新包都会被判 pre-cutoff 跳过（本包实测 skipped）」；workaround 逐字：「用显式路径调 change-manifest-check.mjs delivery/taskpackages/<id>/taskpackage.json 才真正对账（13 面文件全在 manifest）」。脚本实核：`.claude/scripts/change-manifest-check.mjs:53-58` 的 `taskPackagePath()` 在无显式参数时返回 `resolveCurrentTaskPackage(...).path`（即 `delivery/taskpackages/current/taskpackage.json`），`:137-139` 对该路径跑 `gitFirstCommitDate(tpPath)`，`MANIFEST_CUTOFF = '2026-09-28'`；current/ 投影文件首提于 2026-09-11 < cutoff，于是 **cutoff 后每个包都命中 `skipped: 'pre-cutoff'` 早退**。对照同谓词姊妹闸 `.claude/scripts/taskpackage-required-check.mjs:146-150` 用 `gitFn(\`delivery/taskpackages/${cid}\`)`（包目录首提日）——同一 cutoff 谓词、两种路径基准，只有 change-manifest 侧错。

**影响面**：2026-09-28 之后起包的**全部 dev-loop 包**。`verify.mjs` 的 change-manifest 闸会报 `ok:true / skipped:'pre-cutoff'`，changeManifest 的「清单外实现面文件 fail-closed」从未生效——这是静默假绿，且 fail-open 方向（跳过 = 绿）恰好掩盖 manifest 起草缺口。261031-cron 是首个显式发现者；同批 261104-k2 / k5 的「manifest 起草漏编译单元 / 漏生成件命名律」若当时对上账本会被闸直接咬住。

**建议改动**（候选，交 review、不自批）：① change-manifest-check 的 cutoff 判定改用包目录路径（与 taskpackage-required-check 同基准：从 `taskPackage.changeId` 拼 `delivery/taskpackages/<cid>`，或从解析出的路径上溯两级）；② 闸自带负向复证 case：cutoff 后包 + 缺失 manifest 条目必须红；③ 若担心与 `--follow` 的 renamed 文件语义交互，至少在报告里回显实际取到的路径与首提日，别只回显 `skipped`。

**风险**：riskTier: **low**——只改 cutoff 判定的路径基准，不动 cutoff 日期、不动 manifest glob 判据、不动其它闸。但触 `.claude/scripts/**` → 按 `/gov-change` 判据恒 Track B 主路径。验证方式 = 取一个本批已被跳过的包（261031-cron 或 261104-k5）跑修复前后对比：修前 `skipped:'pre-cutoff'`，修后应进入真正对账并按 manifest 结果出 ok/red。