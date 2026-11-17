---
id: ARQ-261117-dispatch-brief-operational-facts
title: 派单 / 起包的操作性事实没有前置——目标文件死活、manifest 起草面、只读预评边界、写通道、设备编译通道、上一包状态事实源
domain: gov
status: deferred
severity: SHOULD
owner: architect（派单模板与 brief 文面）+ dev-loop（执行面）
trigger: 下一个动 planner 派单模板 / brief-inputs / launch-card 文面的治理批
opened: 2026-11-17
closed: ""
closedBy: ""
paths: [".claude/agents/planner.md", ".claude/agents/acceptance-author.md", ".claude/references/dev-loop.md", ".claude/scripts/brief-inputs.mjs"]
source: ["delivery/taskpackages/261104-k2-points-device-ui/pod-retro.md 踩坑 2", "delivery/taskpackages/261104-k4-reward-goal-config/pod-retro.md 踩坑 3", "delivery/taskpackages/261104-k5-story-cover-nav/pod-retro.md 踩坑 5", "delivery/taskpackages/261104-k3-qq-points-channel/pod-retro.md 踩坑 2 / 4", "delivery/taskpackages/261028-m6-k3-p3-story-typography/pod-retro.md near-miss ①", "delivery/taskpackages/261104-k3-qq-points-channel/pod-events.jsonl event 07dffb14-48fc-43f3-9da3-1abe15c47eb6", "delivery/retro/2026-11-17-loop-retro.md §二 T5"]
---

**证据（六种操作性事实，均在派单时缺位、由执行者自觉兜住）**：
1. **目标文件死活**。261104-k2 踩坑 2 逐字：「派 acceptance-author 扩 artt_expected 前没核 ui_art_integration.acceptance.c 已归档（260915），派单即死件。与 K1 教训『路由语义先读权威文件』同型第三次」。建议的固定动作 = 派单目标文件先 `git ls-files` + 读其 README / 头注现役声明。
2. **manifest 起草面**。261104-k4 踩坑 3 逐字：「拆包期把契约逻辑标识直接写成文件名，五个里五个错」；261104-k5 踩坑 5 逐字：「manifest 起草连续两包漏『实现会动的 TU』（K4 漏生成件命名律、K5 漏类声明头文件）：粗 glob 只列 .cc 会漏 .h」。建议 = manifest 按「这次会改哪些编译单元与其声明」展开，含生成件时对照生成器命名律。
3. **只读预评无界**。261104-k3 踩坑 2 逐字：「architect 首轮给了 11 项必读清单 + 一条追加消息，40 分钟未收口」。同包第二轮的正向做法：硬帽 12 次工具调用 + 明确产出格式 + 禁止继续扩读 + 已知事实前置，10 分钟收口。
4. **写通道**。261104-k3 踩坑 4 逐字：「子 agent 对 `delivery/taskpackages/**` 的 Bash 写会被 universal.bashDenyWrite 拦：acceptance-author 超预算执行里有一次 `cat >` 被拦，改 Write 才落盘」。建议 = 派单写明包目录只准 Write/Edit，不用 Bash 重定向。
5. **设备编译通道**。261028-m6-k3 near-miss ① 逐字：「builder device build 发现 brief 误写『WSL 下跑 device build』——skill 铁律 device build = Windows venv + `c:/tmp/idf_run.py`，builder 按 skill 正路执行并如实申报」。建议 = launch 卡固件严格模式行固定一句通道声明。
6. **上一包状态事实源**。261104-k3-qq human-correction `07dffb14-48fc-43f3-9da3-1abe15c47eb6` 逐字：planner 误记「K2 真机轮待跑」，human 纠正「K2 的真机轮跑过了呀」；lesson 原文：「核上一包真机轮状态时读 `device-round-evidence.md` 与 pod-events 尾部（`reverification-pass` / `merge-completed`），不只读 progress 冻结时点的段落」。

**影响面**：每个 dev-loop 包的派单面。单条成本看起来都是一次往返或一次失败写，但六条合起来解释了本周期大量「派单即死件 / 一轮白跑 / advisory 吃时钟」的摩擦；且同型第三次（目标文件）、第二次（manifest 编译单元）已经证明靠人记得不可靠。

**建议改动**（候选，交 review、不自批）：在 planner 派单模板（或 brief-inputs 的固定输出节）增加一组**操作性事实块**：① 目标文件现役核验结果（`git ls-files` + 头注 / README 现役声明）；② manifest 覆盖的编译单元与声明（`.cc` + `.h`）及生成件命名律；③ 只读 advisory 的三件套（工具调用上限、产出格式、禁止扩读）与已知事实前置；④ 包目录写通道（Write/Edit only）；⑤ 设备编译通道（Windows venv + `idf_run.py`，host 验收在 WSL）；⑥ 上一包状态事实源优先级（`device-round-evidence.md` / pod-events 尾部 > progress 段落）。各项可分别判轻 / 主路径。

**风险**：riskTier: **low**——全部为派单 / brief 文面，无脚本、无门判据改动；唯一机械搭车是 `brief-inputs.mjs` 多输出一节，属 Track B 文面批可承接。若判官认为六条过宽，可只取①②⑥（已两次以上复发）先落，③④⑤作为同一节的注记。