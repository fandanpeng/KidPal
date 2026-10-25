# docs-delta · 261024-oneshot-user-uniformity

> integrator 制品 · 2026-10-24 · TaskPackage `docsSyncRequired=false`（零 canonical spec/contracts 改动）。本文件承载申报义务：spec 过时面全清单、部署注记、残余清单。

## 一、spec 过时面全清单（plan.md 残余申报 #4，逐条挂「归在途 /requirement-change & arch 批」）

launch 指令明裁：本批不改 arch/requirements 本体（两文件 integrator 无写权）；下列条目全部归**在途 /requirement-change & arch 批**，由该批原子落地。

| # | 位置 | 现文过时点 | 过时原因（本包换代后） | 去向 |
|---|---|---|---|---|
| 1 | `docs/architecture.md:364-365`（§6.7 第 1/2 行） | 记忆写入范围仍按「设备流程回合（引导/整理等一次性回合）不进画像」的隔离口径表述 | human 2026-10-24 裁 R7/OQ-4 隔离口径退役：一次性回合统一携 `user=<device_id>`，与主链路同落 `agent:kidpal:openai-user:<device_id>` 设备 session 键族，处置由「隔离」改判「统一归属」 | 在途 /requirement-change & arch 批 |
| 2 | `docs/architecture.md:155`（NFR-PRIV-1） | 隐私非功能需求以「设备流程回合不入画像」为边界表述 | 同上——隔离口径退役，NFR-PRIV-1 边界须改写为统一设备 session 键族口径 | 在途 /requirement-change & arch 批 |
| 3 | `docs/architecture.md:360`（§5.1.2 记忆写入范围段） | 记忆写入范围段列举的一次性回合豁免表述 | 同上 | 在途 /requirement-change & arch 批 |
| 4 | `docs/requirements.md:598`（S-F5.1 AC5） | AC5「设备流程回合不进画像」 | 同上——验收口径反转 | 在途 /requirement-change & arch 批 |
| 5 | `docs/requirements.md:601`（补充规则） | 补充规则同段的回合隔离表述 | 同上 | 在途 /requirement-change & arch 批 |
| 6 | `docs/requirements.md:654`（NFR-PRIV-1 边界段） | 隐私边界段的设备流程回合豁免表述 | 同上 | 在途 /requirement-change & arch 批 |

补充（非 spec 过时面、随批已落）：`server/gateway/provider.py` 模块头注 Frozen offline contract 行已同步补可选 `user` 一句（archive 下 test_gateway_directionB 不收集，头注可改——plan.md ① 预授权）。

## 二、部署注记

1. **manifest 新登记两条 prompt 条目的 first-run 语义**：`server/deploy/manifest.yaml` 本批新增 `server/gateway/front/prompts/faithful-organize.v1.md` 与 `guide-round.v1.md` 两条（`restart: none` / `verify: md5`）。指令挪 request 级 system 后，live 指令全在制品里；**两文件在 live 缺席时，部署前置漂移核验会判漂移 REFUSE**——该次部署须 `--first-run` 或先手工置入两文件。缺位运行态兜底 = `_load_prompt_artifact` 返 "" → user 消息回填现行完整包装形态 + warning 留痕（最坏退回旧 live 行为）。本登记同时偿还 261022 polish 改 guide-round.v1.md 但 manifest 未登记的欠账。
2. **noisePatterns 退役实证 = 部署轮实读 openclaw.json**（无机械验证面，已在声明 `_note` 如实申报、不设 `expect:unset` 假牙）：部署轮实读 `/root/.openclaw/openclaw.json`（或 `openclaw config get plugins.entries.openclaw-honcho.config.noisePatterns`）确认第 5 键已从 live 消失。live noisePatterns 本体已由 human 在 ECS 清空（主线程已做），本批只做声明对齐 + manifest/verify 一致性核对；apply 幂等 SKIP 先于 drift-guard，live 残留时 target_state 相等即 SKIP——机械面证不了删除，只有实读能证。
3. **指令挪 system 输出质量 = 真机 3–5 例抽检**（离线不可证，见残余清单 #1）。

## 三、残余清单（离线不可证，各有归属轮）

1. **指令挪 system 的输出质量**——「最近指令」效应离线不可证。真机 3–5 例抽检：整理回合（首行标题 / 正文分段 / 忠实度 / **不得混入既往篇目内容**——user 入设备 session 后读侧检索范围变为设备 session 自身历史，忠实整理可能被自家既往故事带跑）；引导回合（≤20 字、不代编）。去向：真机轮。
2. **多腿归属单设备 MVP 口径**——映射缺席退 `_alert_legs` 时身份取第一条腿（决策 22 单设备 MVP 成立）；多设备将来会错归属。去向：多设备需求立项时解，不在本包解。
3. **引导+整理双回合重复入记忆**——同段孩子内容先进引导、再进整理回合 → 消息库两遍、observation 可能重复膨胀——检索质量面。去向：部署轮看检索。
4. **noisePatterns live 退役实证**——见上文部署注记 2。去向：部署轮实读。

## 四、R2 修正轮申报（live 证伪回退 · 2026-10-24 human-correction f13a53a8）

**「指令挪 request 级 system」修正案被 live 证伪**，本文件如实申报：2026-10-24 10:10-10:11 真实记录会话中，整理/引导回合模型不跟 request 级 system 指令（OpenClaw openai-http 路径以 AGENTS.md 载体为主，请求级 system 疑似被替换/忽略），整理产出聊天腔 + 坏归档件 `stories/2026-10-24-sc4238164`（留待重录覆盖或 human 手删，本包不碰生产数据）。这是**离线盲区实证命中**——host 侧全绿（payload 断言、声明锚、闸链全绿）不能证 LLM 对 system 载体的遵从性，即本包 plan.md 残余申报 #1（「最近指令」效应离线不可证）的兑现形态，且以更彻底的方式发生（不是质量劣化，是载体不被跟）。R2 回退：user 消息恢复「指令 + 素材」**恒定包装**（验证过的形态），request 级 system 制品装载保留并存、manifest 登记保留（为制品本身到达 live，无害），携 user 半不动。

**部署注记不变**（本文件 §二 1/2/3 照旧有效）：--first-run 语义、noisePatterns 部署轮实读、真机抽检清单全部照旧；唯一语义更新 = **§二 3 与 §三 1 的「真机 3–5 例抽检」口径由「指令挪 system 输出质量」更新为「恒定包装形态下整理输出质量」**（整理：首行标题/正文分段/忠实度/不混入既往篇目；引导：≤20 字、不代编）——R2 后指令已回 user 消息验证过形态，抽检验证的是恒定包装在统一携 user 新通道下的输出质量。**live 复证 = 下一部署轮 + 真机轮**（离线不可证）；检索质量残余（回复全量进记忆库含聊天腔）与双回合重复入记忆（§三 3）继续在册，部署轮看检索。
