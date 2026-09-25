# launch-card · 260924-m4-k1-story-cloud-link

跑 260924-m4-k1-story-cloud-link（m4-star-notebook，mergeOrder 38）。

基线锚：`delivery/workstreams/m4-star-notebook/progress.md` 2026-09-24「launch 面就绪」段双面锚——server 面 **80 passed / 0 failed / 4 deselected**（命令逐字 `python -m pytest -q --import-mode=importlib -m "not live" "C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch/server/acceptance"`，@ `5e862d0`）；firmware host 面 **103 Tests / 0 Failures / 0 Ignored，exit 0**（`espidf_wsl.sh build` + `host_infra_test.elf`，@ `5e862d0`）。差异判停：`git diff --quiet 5e862d0 HEAD -- server/`（或 `-- firmware/`）零差异 → 引用锚行不重跑；有差异 → 同命令重跑该面。本包验收红基线（acceptance-author 新冻齿）恒现跑，不适用缓存。

**intake 特别注意（契约已先行落地）**：契约 diff 已于 launch 前落常驻分支（landing commit `5e862d0`，CCP §5 十问 HUMAN_APPROVED、Q2=丙案）——intake 将其计入基线；story.py 三处钉行已更新（doc 零行为）；builder 不再触碰契约耦合钉行；Q7 relay 收紧 + Q8 story floor 的机械执行面 = 本包实现义务（gate evidence 条件 C1）。

上一包留根：`delivery/changes/dev/260924-perf-k1-roundtrip-latency/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。

固件严格模式默认开：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省；本包 subsystem=server，固件面零触碰时闸的适用按 verifier 既有裁决纪律）。

命令行（两形态并列，标注适用条件）：
① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
