# review · m3-schedule

> workstream 级门审裁决耐久记录（append-only；行不覆盖）。

## 门 1 · 拆包切法轻量 checklist review（2026-09-21）

- **终裁决**：**verdict: GREEN**（首轮 BLOCK(F1) → 主线程机械修复 → architect 抽查复核转绿，2026-09-21 同日）。
- **审者**：architect（只读轻量 checklist review；human/主线程派发）。
- **被审制品**：本 workstream 四件（roadmap.md / decision-coverage.json / progress.md / task_plan.md）+ 三包 taskpackage.json/plan.md + K1 包内 CONTRACT-CHANGE-PACKET.md 草案 + 上游锚 `delivery/milestones/milestones.jsonl` 行 `M3-schedule` + `docs/requirements.md` F1 节（S-F1.1/S-F1.2/S-F1.3/S-F1.5）/ §6 阶段三 / §8 已砍清单 / NFR-READ-1 + `docs/contracts/interface-contract.md` §1.3②⑥/§二 B1。

### 过程记录

- **首轮（BLOCK(F1)，派发 2026-09-21）**：checklist 五项中四项过（切片逻辑/依赖序与 WIP/decision-coverage 完备/demo 证据形态可行），唯一阻塞 F1 = 三包 taskpackage `story` 字段写成技术方案句、违反「story 字段承锚点引用」冻结纪律（task-intake-and-slicing「拆包链」节）。实现现状四主张（front 零课表接线 / schedule.py 既有解析器 / schedule_page.c 旧列表版 / 旧冻结验收板云两侧已 archive）全部抽查属实。
- **修复（主线程，同日）**：三包 story 字段改锚点引用形态（移用 roadmap ③ 回指行文本 + 技术描述「见 plan.md」指针）；ADV-1 一并处置 = K1 plan.md 取舍点 C 补「决策 26 字面偏差注记」（执行体 OpenClaw→front 偏移 + 即时上界 ≈60 秒 + 回退乙案路径），供 human 契约门过目。
- **复核（GREEN，2026-09-21）**：F1 修复确认（三包逐一抽查合规）；四项过结论维持；ADV-1 核实已补。

### checklist 五项终态

1. **切片逻辑 — 过**：三包垂直切片（K1 云端数据能力 L0 offline 可独立验证 / K2 页面能力 host+device 可演示 / K3 语音能力 + live 收口），云板分界有显式理由。
2. **依赖序与 WIP 合理性 — 过**：DAG（K1→K2、K1→K3、K2→K3）、mergeOrder 33/34/35 与依赖边一致；真机半 structural deferred 挂 K3 设备轮声明清晰；契约门时序成立（launchPreconditions 第 1 条 = K1 包级前置 + K2 形状齿依赖 K1 契约 diff）。
3. **decision-coverage 完备 — 过**：nextAction 三块全落包（含块③ producer/消费两半跨包显式注明）；dropped 两条 reason 达地板且实质成立；crossPackageBehaviors 三条 canonical 四键齐全（第 2 条 owner=K1/carriedBy=K2 = 合规活语义）。
4. **每包意图陈述一致 — 过**（F1 修复后）：v1.19 口径逐条对核无误；story 引用全为锚点引用形态；intake 两道机查主张成立。
5. **demo 证据形态可行 — 过**：offline 齿 + device build 闸 + 单真机轮配额仅 K3；「LLM 意图判定不可 offline 机判归真机目击」诚实声明在案。

### workType 分流

| 包 | workType | 说明 |
|---|---|---|
| 260921-m3-k1-cloud-schedule-producer | 新建型 | producer 链路全新（front 零接线查证）；取舍点 A/B/C + producer 归属（Q4）均有方案、理由与倾向 |
| 260921-m3-k2-p2-week-grid-page | 新建型 | P2 整页重写（布局模型质变，只留工具函数与降级纪律）；seam 零改只消费；今天列板端推导 |
| 260921-m3-k3-voice-schedule-entry-live | 新建型 + 验证尾 | prompt v6 只动课表段；意图判定归真机不造假绿；真机轮清单制 |

### 附注

- **ADV-2（非阻塞提示）**：K1 riskTier=medium 但同 PR 携带契约面 diff——supervisor 合并 K1 时按 M3K1-A1 + 契约门十节 gate evidence（本包 `architecture-gate-evidence.md`）核验后放行。
- **契约预审（并行，2026-09-21）**：architect-reviewer 四分类裁定 = 触点①（schedule view 周网格）SPEC_CONTRACT_DRIFT、触点②（home 可选 data）CONTRACT_CHANGE_REQUIRED，预审状态 PRECHECKED；漏问点 a–f 已并入 CONTRACT-CHANGE-PACKET v2（Human Question 扩为五问）；预审裁定表收编于 K1 包 `architecture-gate-evidence.md` §3。**待 human 契约门裁五问**（K1 launch 前置）。
- 无触 Track A 升级判据：契约触点已按设计内路径路由（architect-reviewer 四分类 + human 契约门）。

