# Launch Card · 260921-m3-k2-p2-week-grid-page

跑 260921-m3-k2-p2-week-grid-page（workstream m3-schedule，mergeOrder 34，riskTier high / rigor full）。

## 基线锚（m3-schedule progress.md 带锚行 · 260915 T3 差异判停协议）

- **firmware host 面**：`git diff --quiet 40aefa4 HEAD -- firmware/` 实测零差异（2026-09-21 launch-prep 实跑）→ **引用不重跑**：63 Tests / 0 Failures / 0 Ignored，exit 0（2026-09-21 @ 40aefa4，面域 firmware，progress.md 2026-09-21 launch 面就绪段 verbatim）。
- **server 面**：`git diff --quiet 40aefa4 HEAD -- server/` 有差异（K1 实现批 +1349 行）→ **已重跑**：`python -m pytest -q --import-mode=importlib -m "not live" <repo>/server/tests <repo>/server/acceptance` → **75 passed / 0 failed**（2026-09-21 @ e6a62a9，面域 server；launch 批仅动 delivery/ 与 current/ 指针，server 面无差异可直接续引）。零红。
- 本包验收红基线（acceptance-author 新冻结齿）**恒现跑**，无缓存可言。

## 上一包留根

`delivery/changes/dev/260921-m3-k1-cloud-schedule-producer/pod-retro.md`——intake 逐条评估（构成 blockingDecision → fail-closed 上报）。已刷新 `delivery/taskpackages/current/taskpackage.json` 指针到本包（launch-prep 侧办结，retro 改进项②）。

## 固件构建闸

固件严格模式默认开：`VERIFY_FW_BUILD_REQUIRED=1`（无关闭留痕即开，260909 T3/T5 OQ1=(b)——firmware-build 闸读进程环境变量，未设即降级，此声明行随卡走不得省）。

## 命令行（两形态并列）

① 人在场交互式：`claude --settings .claude/dev-loop.settings.json --agent planner`
② 无人值守 headless：`claude -p --dangerously-skip-permissions --settings .claude/dev-loop.settings.json --agent planner`（另加 `CLAUDE_CODE_PRINT_BG_WAIT_CEILING_MS=0`）

## 交接注记（K1 retro / K1 落账随包交接项）

- **dispatch-stamp 恒传 `--task-package-id`**（current/ 指针已刷新为兜底，但恒传显式 id 是纪律本体，retro 改进项③）。
- **mutation-check 回填前先读 checker 的 id 提取器/匹配语义**（raw-id 坑会变形：ARQ-097 口径 = 按 acceptance 全文提取 id + sensitive 信号才算命中；机制不变、坑会变形）。
- **preview.time 裁决点**：契约 §1.3⑥ 示例 `preview.time="明早"` 与 home_p1.h seam `hp1_item_t.time ≤5 字节` 冲突——K1 落账采认口径：载荷钉 `HH:MM`（明日首项），「明早」为设备渲染标签由 **K2 落**；若 human 契约门本意是载荷直发标签，需回裁（K1 pod-retro 留观项）。
- **M3K2-A7 live 半 structural deferred** 归 K3 设备轮（单真机不两轮撞车），K2 侧 host 齿 + device build 闸先行。
- K2 acceptance-author 冻齿前置已解：契约门 HUMAN_APPROVED 且契约 diff 已落地（24bb1b8），新形状齿可冻。
