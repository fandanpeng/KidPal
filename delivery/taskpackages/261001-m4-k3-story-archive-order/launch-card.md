# Launch Card — 261001-m4-k3-story-archive-order

跑 261001-m4-k3-story-archive-order（m4-star-notebook，mergeOrder 43；纯 front/server 面：录音归档时序修复）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-10-01 · B1/C launch prep」段带 SHA 行——server 面（127 passed / 0 failed / 4 deselected @60744cb）+ firmware host 面（147 Tests / 0 Failures @60744cb，命令见锚行）。intake 差异判停：以该段锚行 SHA 为基准对各自面域跑 `git diff --quiet <anchor> HEAD -- <面域>`，零差异即引用锚行不重跑，有差异重跑（命令同锚行）。
- **上一包留根**：`delivery/changes/dev/261001-m4-k3-record-session-live-repair/pod-retro.md`（K3-repair 真机轮复盘，intake 逐条评估；构成 blockingDecision → fail-closed 上报）。其中两条与本包直接相关：① 该 retro §四 对 D3 根因的表述**已被主线程复核订正**（真因 = 归档早于转写，见 spine openQ q8）——勿按 retro 原表述修；② §二「以齿代现象」教训（host 齿全绿 ≠ 真机现象对）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：
  ① **本包纯 front（server 面）**：`server/gateway/front/front_app.py` + `server/gateway/story.py` + 验收；不触固件、不触契约/spec。
  ② **要改已 merged 包（K1 `260924-m4-k1-story-cloud-link`）的冻结验收时序假设**——`server/acceptance/test_m4k1_story_cloud_link.acceptance.py` 的归档链齿固化 `stt → listen-stop` 序（真机是 `listen-stop → stt`），是本缺陷逃过验收的根因。改写 + 新增负锚齿 SAO-A2 的签字成本经 closeout 承载（plan §5）。
  ③ **与 Package B（D3-甲）边界**：本包只交付「KEY 兜底收尾在 manual 模式下可用」；逐句回显、说「我说完了」自动收尾、120s 静默 sweep 复活均不在本包（后者在 manual 模式下结构性不可用）。pending-stop 判据须含「stop 时 accum 非空 → 立即归档」分支，避免 B 落地 auto 后返工。
  ④ **未覆盖面如实申报**：收尾后引擎仍回答并播报（`<< …`）属 B3 面，不在本包。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
