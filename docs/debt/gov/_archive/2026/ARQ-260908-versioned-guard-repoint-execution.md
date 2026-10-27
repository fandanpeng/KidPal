---
id: ARQ-260908-versioned-guard-repoint-execution
title: 重指执行未做——4 件既有冻结件的版本无关重指待逐件 human 授权
domain: gov
status: closed
severity: SHOULD
owner: ""
trigger: 发起 = 下一批触碰对应冻结件时（P7 对齐）或 human 专项批
opened: 2026-09-08
closed: 2026-10-27
closedBy: 261027-gov-review-p5-spike-anchor-cluster
paths: []
source: 260908-gov-review-merge-chain-hardening T7③/B-8（2026-09-08 landing）
---

versioned-guard ARQ 规约面已随 260908-gov-review-merge-chain-hardening T7 落地（acceptance-evidence.md 撰写约定 + guard-class token + 重指清单在册于 PROPOSAL 附录 A.3）；本行承接重指执行——待重指 4 件 = schedule_p2 P2-A6 wiring 齿 / VRT live 探针（A4/A7）/ CHR-A8/A9 live 探针 / schedule_p2 PROMPT_CARRIER_V2 常量，触他人冻结件逐件 human 授权

**2026-10-27 · 结项（`261027-gov-review-p5-spike-anchor-cluster`，纯记账、未执行任何重指）**：原义务 = 对附录 A.3 在册的 4 件待重指项逐件取 human 授权；权威清单 = `delivery/archive/changes/arch/260908-gov-review-merge-chain-hardening/PROPOSAL.md` 附录 A.3（归档只读）。复证（2026-10-27 本批起草期实测）：① A.3 的 4 件落点已被后续批 `ARQ-260916-server-mvp-teeth-sweep`（commit `77b6b0cf`，2026-09-16，MVP / 验证期 server 面冻结验收 10 件整体归档）消化——三份被引冻结件 `test_char_stroke.acceptance.py` / `test_schedule_p2.acceptance.py` / `test_voice_roundtrip_b.acceptance.py` 已移入 `server/acceptance/archive/`，归档件由 `server/pytest.ini` 的 `norecursedirs = … archive` 排除出裸收集；② live 验收套件（`server/acceptance/`，排除 archive）对 `PROMPT_CARRIER_V2` / `P2-A6` / `CHR-A8` / `CHR-A9` / `VRT-A4` / `VRT-A7` 全部 grep 命中 0，即无第二个执行面消费这些版本钉死常量；③ 继任覆盖在场：`server/acceptance/test_prompt_v7_round_routing.acceptance.py` 按版本演进逐版钉（v7 → v9 → v10 → v11：落盘结构断言 + 制品首行无版本前言 + 前版 immutable 负锚 + 三面接线一致性），live 探针一律显式声明为「live 观测项——非自动断言」（PN-A5 / RF-A5 / PV-A8），A.3 当年担心的「版本钉死假绿」在继任面已结构性消解；④ 归档树 append-only 禁改写，对已归档 case 做重指无意义。human 2026-10-27 逐件授权（AskUserQuestion 四问四拍，主线程记录随本批任务书携带）：① schedule_p2 P2-A6 接线（A.3 #3）② VRT live 探针 A4/A7（A.3 #6）③ CHR-A8/A9 live 探针（A.3 #7）④ schedule_p2 `PROMPT_CARRIER_V2` 常量（A.3 #8）——四件均裁「确认结项，不再重指」。无留根。
