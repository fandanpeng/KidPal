# Launch Card · 260921-m3-k4-p1-cjk-dynamic-text

跑 260921-m3-k4-p1-cjk-dynamic-text（workstream m3-schedule，mergeOrder 36，riskTier medium / rigor lite，triage ②单包 Lite human 确认）。

## 基线锚（m3-schedule progress.md 带锚行 · 260915 T3 差异判停协议）

- **server 面**：K3 实现批动了 server/ → 已重跑：`python -m pytest -q --import-mode=importlib -m "not live" <repo>/server/tests <repo>/server/acceptance` → **95 passed / 0 failed**（2026-09-22 @ dc6094a；K3 新增 20 齿并入基线）。零红。
- **firmware host 面**：`git diff --quiet 91b3b5c HEAD -- firmware/` 实测零差异 → **引用不重跑**：74 Tests / 0 Failures / 0 Ignored，exit 0（2026-09-21 @ 91b3b5c，面域 firmware，progress.md「K3 launch 面就绪」段 verbatim）。
- 本包验收红基线（acceptance-author 新冻结齿）**恒现跑**，无缓存可言。
- intake 消费：对相应面域先 `git diff --quiet <锚SHA> HEAD -- <面域>`，零差异引用 progress.md 锚行、有差异同命令重跑。

## 上一包留根

`delivery/changes/dev/260921-m3-k3-voice-schedule-entry-live/pod-retro.md`——intake 逐条评估（构成 blockingDecision → fail-closed 上报）。`delivery/taskpackages/current/taskpackage.json` 指针已刷新指向本包。

## 固件构建闸

固件严格模式默认开：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。

## 命令行（两形态并列）

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）

## 交接注记（K3 retro / K3 合并落账随包交接项）

- **plan.md 边角（builder 易漏，architect review 实证）**：home_upcoming.c:393–396 按 31 字节截 name 会截半 UTF-8 多字节字符——P1 解码器须对不完整尾序列安全停笔（截半不画、不越界、不崩）；解法放 P1 侧，不动 K2 已合的 home_upcoming。
- **E2 铁律**：hp1_item_t 与 11 个冻结契约函数零偏移、home_upcoming.{c,h} 零改、contracts/spec 零改、schedule_page/K2 面零触碰、固定标签烘焙资产维持现状。
- **字体双分支实证（plan.md 收敛段）**：设备 `font_puhui_14_1`（= 对话页同款，human 裁定）+ host `lv_font_source_han_sans_sc_14_cjk`，照抄 `ksg_text_font()` 双分支。
- **真机目测点挂设备轮窗口**（课程名可见/与 time 基线协调/14px 1bpp 可读性），可独立开窗或随下次设备轮；阈值微调属设计迭代不触契约。
- **调度**：树池第 11 棒，同树同常驻分支续跑；分支上收尾 commit 领先 main 属树池惯例（树内超集），合并时随批回 main。
- **在案欠账（非本包义务，勿吸）**：trigger=immediate ECS 取证（K1 部署面）；设备时钟/WS 掉线族（K1 热修轮独立缺口）；loop-retro 阈值 advisory（主线程）。
- dispatch-stamp 恒传 `--task-package-id 260921-m3-k4-p1-cjk-dynamic-text`。
