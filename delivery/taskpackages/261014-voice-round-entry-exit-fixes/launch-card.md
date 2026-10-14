# launch-card · 261014-voice-round-entry-exit-fixes

跑 `261014-voice-round-entry-exit-fixes`（自建形状，非 workstream，无 mergeOrder；planner `/build-feature <id>` 从 sibling `plan.md` 读 reviewed plan 做 intake）。

## 运行位

- worktree = 本树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`，分支 `dev/260915-m1-k1a-key-layer-dispatch`（human 指示：本包 commit 到本分支，不新建 worktree；不 push）。
- sourceRefs = 三条 TECH CRQ：`TECH-261014-wakeword-connecting-voice-layer` / `TECH-261014-post-speak-listening-lingering` / `TECH-261014-kidpal-speak-await-nonetype`（触发留痕已 append 进各条目）。
- 执行边界（human 已裁，随包留痕）：engine-patch 改动**只 commit 不部署 ECS**；固件烧录 = 本包 host 转绿后由主线程执行（human 已预授权本轮自主烧板），真机复验轮等 human 回来联测。

## 基线锚（2026-10-14 主线程现跑，HEAD bf96904，树净）

- **server 面**：计数 = **164 passed / 0 failed / 4 deselected**（现役面已非 2026-09-16 锚的空态，本行为新锚）；命令 = `python -m pytest -q --import-mode=importlib -m "not live" --ignore=server/acceptance/archive <repo>/server/acceptance`；日期 = 2026-10-14；量测 HEAD = bf96904；面域 = server。⚠️ 量测口径警示沿袭：裸 pytest（不带 --ignore）会递归收集 archive/，必须带。
- **firmware host 面**：计数 = **165 Tests / 0 Failures / 0 Ignored**；命令 = `wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && wsl -d Ubuntu-24.04 -- bash -c "cd firmware/main/test/host_app && ./build/host_infra_test.elf"`；日期 = 2026-10-14；量测 HEAD = bf96904；面域 = firmware。
- consumer 差异判停：Pod intake 时 `git diff --quiet bf96904 HEAD -- <面域>`，零差异引用本卡两行（SHA + 命令 + 计数三件齐），有差异重跑该面（命令同记录）。acceptance-author 冻新齿时以 host 全量首跑为有效基线落账点（新测试无缓存、恒现跑）。

## 上一包留根

`delivery/changes/dev/261013-p1-tomorrow-row-sliver-fix/pod-retro.md`——intake 逐条评估（构成 blockingDecision → fail-closed 上报）。

## 固件严格模式

默认开：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。

## 命令行（两形态并列）

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
② 无人值守 headless：`claude -p --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`；⚠️ 2026-10 实测 `--dangerously-skip-permissions` 被平台分类器硬拦，本机一律用无旗标形态——本包 launch 即按②无旗标执行）
