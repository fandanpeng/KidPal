# 启动卡 · 261019-p3-record-dialogue-guide

跑 261019-p3-record-dialogue-guide（spine 跨面变更实现面，②单包 Full / riskTier=high / rigor=full）。
基线锚：`delivery/workstreams/m4-star-notebook/progress.md` :294-296 双面基线锚 @ b6cc8945——**差异判停双面命中（server / firmware 自锚起均有差异：本批 req/arch 面 + 261016 实现 + main 漂移），两域基线均须重跑**，不引用旧数。server 面 = `python -m pytest -q --import-mode=importlib -m "not live" C:/AI/KidPal-pod-m2/server/acceptance`；firmware host 面 = espidf_wsl.sh build + host_infra_test.elf（命令原文见 progress.md :295-296）。旧锚已知红 3 条（p1_auxbar_style AB-A1/A2/A4）——重跑后以新实跑为准，基线消解另立批不塞本包。
上一包留根：`delivery/taskpackages/261016-p3-story-body-sd-cache/pod-retro.md`，intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
固件严格模式默认开：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开，260909 T3/T5 OQ1=(b)——声明行随卡走不得省）。
会话标识：主线程 headless 拉起，后台任务 id bs5gha3hv，日志 /c/tmp/build-261019-p3-record-dialogue-guide.log，启动 2026-10-19 20:52±（CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0；身份三件套 = changeId + --agent planner + dev-loop.settings.json）。叫停时按 pod-coordination「叫停协议」核进程再处置树。
环境事实（launch-prep 2026-10-19 实测）：复用树 C:/AI/KidPal-pod-m2 全绿——settings-lint 0 RED；专属图索引 C-AI-KidPal-pod-m2 在册；固件环境四标记齐（sdkconfig / dependencies.lock / host_app sdkconfig / managed_components 软链原生 isdir=True）；board 已预填 queued + worktreePath。包内两件结构性义务（plan §结构性风险）：display_schema 自推分叉、stop-truncation 豁免重绑——builder 动工前必读。
命令行（两形态并列，标注适用条件）：
① 人在场交互式：claude --settings .claude/dev-loop.settings.json --agent planner
② 无人值守 headless：claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner（另加 CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0）
——本次实跑 = 形态②（缺 --dangerously-skip-permissions；若会话因权限拒转停摆，按形态②补旗重拉，恢复点 = spine + 盘上制品）。
