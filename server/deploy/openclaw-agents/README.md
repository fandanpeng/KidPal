# openclaw-agents — OpenClaw agent 工作区仓内镜像

本目录镜像 ECS 上 OpenClaw agent 工作区里**随本仓 review 演进**的文本（护栏纪律 / 工作区说明），
使这些改动先经仓内 review 再同步 live，而非直接在 ECS 上改。

## workspace-kidpal/

| 仓内镜像（本目录） | live 位置（ECS） |
|---|---|
| `workspace-kidpal/AGENTS.md` | `/root/.openclaw/workspace-kidpal/AGENTS.md` |

## workspace-main（261013-gov-review-deploy-single-source-loop 补齐）

| 仓内镜像（`../openclaw-skills/workspace-main/AGENTS.md`） | live 位置（ECS） |
|---|---|
| `workspace-main/AGENTS.md` | `/root/.openclaw/workspace/AGENTS.md` |

两 workspace 的 AGENTS.md 均由 [manifest.yaml](../manifest.yaml) 登记、`deploy.sh` 铺放（`verify-deploy.sh` 每日核验）。

护栏主阵地 = 每次会话注入的 system prompt（渲染源 `server/deploy/openclaw-system-prompt.vN.md`，
经 `apply-xiaozhi-overlay.sh` 上 ECS）；两侧 `AGENTS.md` 是**次要承载**，其自声明
「冲突时以注入 prompt 为准」。

## 同步纪律（顺序别反）

**先改仓内镜像进 review，再同步 live**——不要反过来直接在 ECS 上改 `/root/.openclaw/...`：
直接改 live 会绕过 review 门，且下次同步被仓内旧版覆盖，造成两源漂移。变更顺序恒为：
① 改本目录镜像文件 → ② 进 review / 合并 → ③ 再 `scp`（或 `ecs_put.py`）同步到上表 live 路径。
