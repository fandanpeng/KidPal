---
id: TECH-261017-xiaozhi-template-residue-deploy-ssot-gap
title: 引擎通用小智模板残留 + 三个部署面键无仓内源——agent-base-prompt.txt 与 v10 对冲（工具语义/禁 Markdown）、system_error_response 兜底语播「小智」、kidpal skill 白名单未收（18 bundled 噪音），三者均未接入 manifest.yaml 单一事实源
domain: tech
status: deferred
severity: SHOULD
owner: dev 批（server/deploy 部署面）
trigger: 下一次动 server/deploy 部署面的批（与 TECH-261017-schedule-answer-skip-read-hallucination 同批）
opened: 2026-10-17
closed: ""
closedBy: ""
paths: ["server/deploy/apply-xiaozhi-overlay.sh", "server/deploy/provider.config.yaml", "server/deploy/manifest.yaml", "server/deploy/kidpal-agent-prompt.txt"]
source: 2026-10-17 ECS 排查（docker inspect 挂载：agent-base-prompt.txt 镜像内置非 bind-mount；.config.yaml 无 prompt_template/system_error_response 键；openclaw.json agents.entries.kidpal 无 skills；manifest.yaml 无这三条目；openclaw skills list --agent kidpal 报 19 ready 其中 18 bundled/extra）
handoff: 与 TECH-261017-schedule-answer-skip-read-hallucination 合批走部署面 loop；openclaw.json skills 走 perf-k1 定点 patch 先例
---

**证据（ECS 实查）**：① 引擎容器 `agent-base-prompt.txt`（`/opt/xiaozhi-esp32-server/agent-base-prompt.txt`，镜像内置、非 bind-mount）是通用小智模板，被 prompt_manager 套在 v10 外面（`build_enhanced_prompt` 渲染 11,310 字符）；内含 `play_music/hass tools/handle_exit_intent` 工具语义（`tool_and_knowledge` 段「工具不匹配就直答」）、emoji 前缀、`speaker_recognition`、`tts_format_constraints` 段「禁 ``` 代码块」——与 v10 的「末尾必须带 ```display 块」直接对冲。② `core/utils/util.py:609` `system_error_response` 默认值「主人，小智现在有点忙，我们稍后再试吧」，.config.yaml 未覆盖 → 设备出错时对孩子播「小智」。③ `openclaw skills list --agent kidpal` = 19 ready，其中仅 `kidpal-schedule-answer` 一个自有，其余 18 个（browser-automation/canvas/diagram-maker/taskflow 等）OpenClaw bundled/extra、全 modelVisible。④ `manifest.yaml` 无 `prompt_template`/`system_error_response`/openclaw.json skills 三条目；openclaw.json 含 secret（gateway.token）不能整文件入仓。

**影响面**：① 小智模板的「工具不匹配就直答」是课表幻觉的诱因之一（与 v10「先读 schedule.md」对冲，模型须自裁决哪边赢）；禁 Markdown 与 v10 display 块对立。② 设备出错播「小智」——孩子面向产品的身份错位。③ 18 个 bundled skill 挤占每次注入的 prompt、增加误触发面。④ `prompt_template`/`system_error_response`/skills 三键均「手改 live 无仓内源」，下次 redeploy/重建即丢——同款前科 = 260930 model_name clobber（TECH 记忆）、TECH-261010-engine-decision21-keys-not-persistent（决策 21 两键丢渲染面）。

**建议改动（四件，交 review 不自批）**：
1. 换 KidPal 极简模板：新建 `server/deploy/kidpal-agent-prompt.txt`，只保留 `<identity>`(=`{{base_prompt}}`) + `<context>`(时间/日期/星期/天气/位置注入——课表算「后天星期几」靠它) + 中文输出约束，删工具语义/emoji/speaker/反 AI 味；.config.yaml 顶层加 `prompt_template: /opt/xiaozhi-esp32-server/data/kidpal-agent-prompt.txt`（data 为 bind-mount），由 apply-xiaozhi-overlay.sh 渲染。
2. .config.yaml 加 `system_error_response` 覆盖为 KidPal 兜底语（禁「小智」字样）。
3. openclaw.json 加 `agents.entries.kidpal.skills = ["kidpal-schedule-answer","weather","tavily"]`（allowlist 显式列表完全替换继承，其余 16 个从 kidpal 视角消失、main 不受影响）——仿 perf-k1 `openclaw-model-declaration.json` + `apply_model_declaration.py` 的声明+应用脚本+drift_guard+backup+rollback 定点 patch，不整文件入仓。
4. 三者接入 manifest.yaml 单一事实源：模板新增条目（owner overlay、verify md5）+ .config.yaml marker 锚集加 `prompt_template`/`system_error_response` 两锚 + openclaw skills 声明文件条目（owner deploy.sh）。

**风险**：riskTier: medium——同时动生产 openclaw.json（含 secret 的 gateway 主配置）+ 引擎 config + manifest 单一事实源 + 新增渲染面，且 .config.yaml 为 overlay 生成件（改错即引擎起不来）；但四件各自可独立验证、可回滚，爆炸半径可控在部署面。
