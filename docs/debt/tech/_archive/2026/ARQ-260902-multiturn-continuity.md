---
id: ARQ-260902-multiturn-continuity
title: OpenClaw 多轮连续性「自带」vs「待部署验证」相位矛盾 + 协议剖面无会话连续性承载（S2）
domain: tech
status: closed
severity: SHOULD
owner: ""
trigger: 方向 B 落地首验（spec §6.3「部署期第一件事」即触发点）定案；失败则 A4/S-F3.3 指代降级单轮问答并 ACCEPT-AS-RISK 记账。重切已钉（2026-09-03）：承载 = G 包 GWA-A8 部署期首验双探针（mergeOrder 3）
opened: 2026-09-02
closed: 2026-10-12
closedBy: 主线程（2026-10-12 tech 域 CRQ 盘点，human 指示；证据见各条目收口节）
paths: []
source: 260902-arch-review-kidpal-spec-v0.2（findings T2-F9×T5-F3，adjudications V21）
---

spec A4:104/§5.2:459 写「OpenClaw 自带」无 DEFER 标注，§6.3:556 写「待部署验证·部署期第一件事」——同一能力两处相位相反；`type:openai` 配置级接入默认无状态，session_id 维持 conversation 的承载方案（网关侧状态 vs 每轮透传历史）未定，协议剖面只定义 hello 带 session_id、未定义历史承载。若验证失败 A4/S-F3.3「那『星』字呢」指代变幽灵需求。forcing-function 已具备（spec 标「部署期第一件事」），缺的是承载方案定夺与失败降级记账。

---

## 收口（2026-10-12 主线程盘点）

触发条件「方向 B 落地首验」已履职定案：G 包 260903-gateway-directionB 的 V21 多轮连续性探针（`delivery/changes/dev/260903-gateway-directionB/v21-multiturn-continuity-probe.md`，GWA-A8）实证 stateless 不续、每轮透传历史续 → 承载方案「服务端每轮透传历史」定案。spec A4/§5.2 与 §6.3 的相位矛盾由定案消解；后续 live 多轮对话（261010/261012 真机轮）正常运作，无需失败降级记账。关闭。
