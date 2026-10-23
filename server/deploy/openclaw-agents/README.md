# openclaw-agents — OpenClaw agent 工作区仓内镜像

本目录镜像 ECS 上 OpenClaw agent 工作区里**随本仓 review 演进**的文本（护栏纪律 / 工作区说明），
使这些改动先经仓内 review 再同步 live，而非直接在 ECS 上改。

## workspace-kidpal/

| 仓内镜像（本目录） | live 位置（ECS） |
|---|---|
| `workspace-kidpal/AGENTS.md` | `/root/.openclaw/workspace-kidpal/AGENTS.md` |

## 动态 context 承载（每轮注入引擎，缓存边界之下）

| 仓内镜像（本目录） | 角色 / 渲染消费位 |
|---|---|
| `kidpal-dynamic-context.v1.txt` | 引擎装配壳（`prompt_template` 指向 data/ 本文件，只做 `{{base_prompt}}` 包裹）；overlay 拷入 ECS `data/` bind-mount |

动态 context 内容源 = `server/deploy/kidpal-agent-prompt.txt`（本包原地改写定形）——由
`apply-xiaozhi-overlay.sh` 以 2 空格缩进注入引擎 `.config.yaml` 顶层 `prompt:` 键；有界
（本回合时间 / 日期、课表摘要等回合内事实占位），**不含静态纪律**。静态纪律只进上面的
`AGENTS.md`。课表机械注入内容归 P5（本目录只建占位面）。

## workspace-main（261013-gov-review-deploy-single-source-loop 补齐）

| 仓内镜像（`../openclaw-skills/workspace-main/AGENTS.md`） | live 位置（ECS） |
|---|---|
| `workspace-main/AGENTS.md` | `/root/.openclaw/workspace/AGENTS.md` |

两 workspace 的 AGENTS.md 均由 [manifest.yaml](../manifest.yaml) 登记、`deploy.sh` 铺放（`verify-deploy.sh` 每日核验）。

护栏与静态语义承载（`261022-device-memory-prompt-overhaul` 起反转原口径）：

- **kidpal 侧（设备语音链路）**：静态语义单一事实源 = `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`
  （manifest 登记、`deploy.sh` 铺放、`verify-deploy.sh` 每日核验）；每次会话注入的只是动态 context 块
  （时间 / 日期、课表摘要等，不含纪律），冲突以 `AGENTS.md` 为准。
- **main 侧（家长 QQ / cron）**：护栏承载 = `server/deploy/openclaw-skills/workspace-main/AGENTS.md` + workspace skills
  （不经引擎注入）。

## 同步纪律（顺序别反）

**先改仓内镜像进 review，再同步 live**——不要反过来直接在 ECS 上改 `/root/.openclaw/...`：
直接改 live 会绕过 review 门，且下次同步被仓内旧版覆盖，造成两源漂移。变更顺序恒为：
① 改本目录镜像文件 → ② 进 review / 合并 → ③ 再 `scp`（或 `ecs_put.py`）同步到上表 live 路径。
