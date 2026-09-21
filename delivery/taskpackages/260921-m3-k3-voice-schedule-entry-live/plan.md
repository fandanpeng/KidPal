---
rigor: full
riskTier: medium
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-21
---

# M3-K3 语音问课表口径（prompt v6 + 显示课表意图链）+ M3 live 设备确认轮

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M3-schedule`（state=planning）nextAction 块①后半 + 块②/块③ live 复证。
> story 语义权威 = `docs/requirements.md` 现文（S-F1.3 AC1–AC4 + S-F1.5 AC1–AC2 + NFR-READ-1）；契约形状权威 = K1 契约门定稿。

## 背景摘录（新建型锚「现状与差距」）

- **现状·prompt v5 课表 producer 段**：`server/deploy/openclaw-system-prompt.v5.md`（现役接线版，carrier 声明 + overlay 渲染 + provider.config 三面一致，ADR-001 接线不变式）课表段仍让 LLM 按「今天星期几/当前时间」筛选作答 + 产旧形状 `{title,items}` display 块；录入段（QQ 改写 schedule.md）已有。
- **差距**：① S-F1.3 AC1/AC4 口径——答复与百科问答同一问答框（不切页不定位）+ **答复包含备注内容**（带物读回），v5 未显式钉备注入答复（备注解析约定现归提醒段）；② AC2「帮我显示课表」意图 → 切 P2 周网格，v5 无此意图路由；③ 周网格后 LLM 不再产课表 display 数据（K1 已定分工：front 机械计算、LLM 意图信号化）——三缺口合一 = prompt v6 bump。
- **差距·live 面**：K1 producer（三触发）与 K2 网格页/首页注入的 host 半可 offline 验收，但 M3 全部 story 的最终判据是真机目击——本包尾部挂 human 持板确认轮（M2-K3 先例：单真机不两轮撞车，K1/K2 真机半 structural deferred 到此收口）。

## 改法取舍

- **v6 只动课表段、其余逐字保留**：版本化演进规则（新文件落盘、历史可 diff 不回改）；改动面 = 课表 producer 段重写（答复口径 + 意图信号化 + 薄载荷形态按 K1 契约门取舍点 A 裁定）+ 「帮我显示课表」意图路由；故事 producer / 查字 / 提醒段零触碰。
- **意图链路的 offline 验证形态**：prompt 文本锚齿（v6 文件 + carrier + overlay 三面接线断言）+ front 截获展开路径的单元齿（甲案：薄载荷 → front 用 schedule.md 展开网格下发）；LLM 意图判定本身不可 offline 机判——归真机确认轮目击（AC2 真机清单项），不造假绿。
- **真机确认轮清单制**：一次长捕获会话（单一长捕获、中途不关端口、DTR/RTS 复位伪影识别——kidpal-serial-monitor 既有节），human 就绪信号开窗、实时口令钉动作点；清单 = S-F1.2 AC1–4 / S-F1.3 AC1–4 / S-F1.5 AC1–2 / 决策 26 三触发 / NFR-READ-1 目测，逐项判定留设备轮记录。
- prompt v6 槽位预约：建包时核 `server/deploy/` 无并行 v6 占用（ARQ-260908 槽位碰撞规则；v3 抢槽先例在册）。

## 阶段划分

1. acceptance-author 冻齿：v6 锚齿 + 接线齿（carrier/overlay/provider 三面）+ 意图路径单元齿；实跑红基线落账。
2. builder：openclaw-system-prompt.v6.md 落盘 + carrier 声明切换 + apply-xiaozhi-overlay.sh 渲染引用改 v6 + front 截获展开路径实现（若甲案）。
3. verifier↔fixer：pytest offline 全量零新增红。
4. human 持板真机确认轮（M3K3-A3）：清单目击 + 三源日志取证 + K1/K2 deferred 真机半收口。
5. integrator/supervisor 闭环。

## 铁律约束

- 不改 `docs/contracts/**`；不改 spec、不改 agent 定义/gate。
- prompt v5 及以下历史版本零回改（版本化演进）；接线不变式（渲染面 == 声明面 == 被测面）保持。
- live 探针不新钉 v6 版本字面（ARQ-260908 版本字面耦合在册，offline 守卫用解析两面得出版本再断言的既有形态）。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## demo 证据形态复述

offline：v6/接线/意图路径齿 RED→GREEN 全程留痕；live：human 持板确认轮（M3K3-A3）——设备轮记录 + 串口/front/引擎三源日志 + 清单逐项判定，K1/K2 deferred 真机半（M3K1-A5/M3K2-A7）随轮对账收口。这是 Phase 1 课表闭环的最终目击证据。
