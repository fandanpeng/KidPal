# Launch Card — 260924-m4-k3-record-session-live

跑 260924-m4-k3-record-session-live（m4-star-notebook，mergeOrder 40）。

- **基线锚**：`delivery/workstreams/m4-star-notebook/progress.md`「2026-09-30 · K3 launch prep」段带 SHA 行——server 面（引用 K2 prep 锚 117 passed @ 5575a23，launch 批至 HEAD 对 `server/` 零差异，判停引用不重跑）+ firmware host 面（K2 merged 后重锚 @ 2c7343a，本树实测，数值见锚行）。intake 差异判停：以「2026-09-30 · K3 launch prep」段锚行 SHA 为基准对各自面域 `git diff --quiet <anchor> HEAD -- <面域>`，零差异即引用锚行不重跑，有差异重跑（命令同锚行）。
- **上一包留根**：`delivery/changes/dev/260924-m4-k2-p3-star-notebook-ui/pod-retro.md` + `delivery/changes/dev/260924-m4-k1-story-cloud-link/pod-retro.md` + `delivery/changes/dev/260928-story-connect-push-defer/pod-retro.md`（story 链直接前驱，K3 crossPackageBehaviors 逐条引用），intake 逐条评估（构成 blockingDecision → fail-closed 上报）。
- **固件严格模式默认开**：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。
- **intake 特别注意**：①时点①（建连即推 story_list）恢复语义归本包重裁，是拆包侧明示的「真机轮前置必答题」——设备每会话 WS 重连与推送时点冲突形态先想清再冻齿；②K2 冻结面（story_page 消费改形 + 输入表语义位 + mcp_server story 路由 + kidpal_story_delete 发送原语）rebase 后零偏移，session_ctx 发送 = 同族扩展不改 K2 断言；③prompt v7 = 新文件版本化演进，不回改 v6，OPENCLAW_SYSTEM_PROMPT_CARRIER 声明翻转 + provider.config 三键同步；④板端零新增计时器（60s/120s 设备侧 timer 不复活，120s 全在 front）；⑤KEY 长按=保存语义反转负锚（旧冻结表长按=丢弃）；⑥拆包侧 brief 已撤回，Pod planner Step 7 自产；⑦live 确认轮 = human 在场持板（LP4，三源取证、单真机不撞车）——设备轮前核对真机空闲。
- **命令行**（两形态并列，标注适用条件）：
  ① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
  ② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）
