# pod-retro · 261022-m5-n2a-schedule-mechanical-injection

> 第一人称热记忆（planner，2026-10-23，integrator closeout 后 / supervisor 闸前）。只记过程体感，不载 tokens/gates/rounds/verdict/test results（权威在机器制品）。

## 顺的地方

- lite 包跑得快：一轮冻结（8 设计态红）→ 一轮 builder 全绿 → scoped+final 双轮绿 → integrator 一次收口，零 fixer 轮。冻结契约把通道形态钉到可实现粒度（派生接缝签名、帧 token、窗口值）是关键——builder 没有「往哪钉」的自由度也就没有分叉。
- acceptance-author 的「判不了就停、把扩单判据写清」纪律这次运转丝滑：通道形态排查（front_app.py 不在 manifest）在我预设的 launch-card 停等判据上命中，扩单一句话裁定，没烧返工。
- builder 的占位填充「单收口 + 三时序自证」设计超出预期——change_system_prompt 收口顺带把未来 P3 要加的记忆守则装配也留好了缝。

## 卡点与体感

- **manifest 输入面成对登记的教训在 K1 刚发生，P5 就轮到自己受益**：builder 主动核对了 connection.py 部署链的 manifest note 并补登记，deploy 面这次预计无对账轮（connection.py 本来就在清单）。但渲染脚本/部署件「谁把它弄上 ECS」的问题值得在 /loop-retro 成文——每次新部署件都要问一句「它的输入面和它自己，manifest 都登记了吗」。
- **P1 遗留占位观察（{{today_date}} 等可能字面进 prompt）是 builder 超读参考树发现的**——这又是「清单外阅读」立功的例子。这类跨包渲染链问题在包边界内冻结验收探不到，靠实现者的好奇心兜底。若 live 实证确认，P1 收尾小批的判据要在立项时写清（候选落点 connection.py 同缝，改动极小）。
- requiredLayers 的 unit 与 changeManifest 不含 server/tests 的张力这次靠 K1 先例（既有套件零新增红承载）消解，但两个包都靠「先例口径」而非成文判据——第三包再遇到就该成文（task-intake 参考 or requiredLayers 字段注释）。

## escalation themes

- 本包零 agent-escalation、零 human-decision/human-correction 事件；扩单一项（front_app.py）为 planner 权内决定。
- loop-retro 欠账在涨（closeout-check advisory 连续两包提示）——K1+P5 两个部署实证教训（manifest 输入面对、渲染脚本登记）都值得进 cadence retro。
