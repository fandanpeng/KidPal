---
id: ARQ-260929-competence-anchor-rework-evidence
title: 能力透镜锚缺位——acceptance-author 断言可满足性/seam 闭环返工复发 + portfolio-planner 影响面枚举漏扫，def 无专业透镜行
domain: gov
status: blocker
severity: SHOULD
owner: architect
trigger: 260929-gov-review-agent-def-alignment 批 T8（与 ARQ-260929-agent-def-reality-drift / ARQ-260929-agent-def-provenance-noise 同车；human 裁 OQ5 定锚点集）
opened: 2026-09-29
closed: ""
closedBy: ""
hardExclusions: true
paths: [.claude/agents/acceptance-author.md, .claude/agents/portfolio-planner.md]
source: "主线程 2026-09-29 返工普查（pod-events 全量 57 条 friction + 25 条 escalation 聚合 + verifier-evidence 轮次红数 + pod-retro / delivery/changes/retro/2026-09-23-loop-retro.md 主题对读）"
---

**证据**（2026-09-29 实测聚合；判据 = human 260929 裁定「该角色的返工实测集中在哪个能力段，就锚哪段」）：

- **acceptance-author 三段返工复发**：① 冻结齿两向皆红 / 自相矛盾 ×3——`260829-schedule-reminder` ESC-ambiguous-acceptance（钢琴课期望簇落在已过一周，日历真值双法交叉才裁）、`260924-m4-k1-story-cloud-link` ESC-ambiguous-acceptance（test_a6_restart_survival fixture 缺 feed，空累积 stop 依既有语义返 None，两向皆红）、`260911-screen-unification` ESC-ambiguous-acceptance（冻结验收钉 VOICE>SERVER_PAGE 与 PROPOSAL §5 冲突）；② seam 闭环漏钉——`260917-m2-k3-char-window-overlay` scan-miss（5 分钟驻留的设备侧驱动零齿覆盖，seam 只钉纯逻辑模块）；③ host/真机覆盖差集——`260915-m1-n1b-p1-home-battery` scan-miss（fixture 默认不挂 art 资产，imgfont 真字形溢出路径 host 零覆盖；该包 5 红/12 轮）。另 `260915-m1-k3-voice-round-ui` 红基线判读缺口（3 红/15 轮）。
- **portfolio-planner 影响面枚举漏扫**：`260917-m2-k3-char-window-overlay` scan-miss（拆包期 sourceRefs 遗漏四处真实改动面）+ loop-retro-2026-09-23 主题 T2「拆包产物有保质期的快照，元数据与磁盘现实腐坏，无闸可拦」跨棒复发（m2-k2 / m3-k4 实证；planner 1f 与 260924 批 T5 已成流程步，但拆包期枚举本身仍靠脑内清单）。
- **反证（不该加的角色）**：planner 的 ambiguous-acceptance 4 条里 3 条 = 验收↔方案冲突（Step 8b 承载）、落点超写域（C5 承载）；builder/fixer 的 scan-miss 7 条全由 C5 prompt 完整性承载（fixer 2c 对偶在册）；fixer 深轮次（m1-n1b 5/12、m1-k3 3/15）主因环境 / 闸工具类非能力段；over-scan 50 条 = 超读申报机制在工作、纯记账非返工。
- **story-author 零运行数据**（260928 新立）——按判据本批不加锚，留根观察。

**影响面**：acceptance 是 loop 判分器——齿的两向皆红 / 自相矛盾每次都烧一整轮 ambiguous-acceptance 升级 + human 拍板；seam 漏钉转成真机轮才暴露（m1-n1b 形态）；拆包漏扫转成 Pod 内返工或 1f 期漂移修正。三者皆已复发 ≥2 次，def 现文只有流程义务、无专业透镜行，降档模型（Pod glm-5.3-flash）下缺口放大。

**建议改动**（候选，交 review 不自批；Leading Words 一行式，不写方法论正文）：acceptance-author.md Process 第 6 步（红基线）旁加**断言设计透镜**一行——两向可满足性推演（红基线为何红 + 转绿路径存在，fixture 自含性显式声明）+ seam 闭环（每个行为 seam 列驱动方与消费方，缺驱动方 = 盲区申报）+ host/真机近似假设显式化；portfolio-planner.md Step 2 旁加**影响面机械辅证**一行——sourceRefs / docRefs 枚举须 trace_path / get_architecture 交叉验证，不信脑内清单。细节语义仍引 acceptance-evidence.md / task-intake-and-slicing.md，不复制。

**风险**：riskTier: low——两文件各加 1–2 行 G2 文本、零行为语义；锚点集经 OQ5 human 裁定。review profile：**随 260929 批 full-adversarial + human**（批次混含 self-def / review-loop def 第五阀 + G3 scope-map，硬排除承袭）。便宜验证：lint-descriptions / route-regression-check 复跑不劣化 + 逐文件人读 diff。
