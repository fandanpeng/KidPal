# OpenClaw workspace skills 镜像（仓内可 diff 副本 · manifest 管理）

本目录镜像部署在 ECS OpenClaw workspace 的 skills（live 位置见下表）。261013-gov-review-deploy-single-source-loop 起，本目录全部条目由 [manifest.yaml](../manifest.yaml) 登记、`deploy.sh` 铺放、`verify-deploy.sh` 每日核验——不再是「手动写入 live、镜像仅供恢复」形态。OpenClaw 运行态（DREAMS.md / memory / sessions）不入镜像、部署不碰。

| 仓内镜像 | live 位置（ECS） | 说明 |
|---|---|---|
| `workspace-main/AGENTS.md` | `/root/.openclaw/workspace/AGENTS.md` | 家庭侧 main agent 工作区护栏（QQ 录入路径） |
| `workspace-main/kidpal-board-context/` | `/root/.openclaw/workspace/skills/kidpal-board-context/` | 板卡上下文 skill |
| `workspace-main/kidpal-char-strokes/` | `/root/.openclaw/workspace/skills/kidpal-char-strokes/` | 查字 skill |
| `workspace-main/kidpal-reminder-semantic/` | `/root/.openclaw/workspace/skills/kidpal-reminder-semantic/` | 提醒语义回合 skill：cron `kidpal-reminder-semantic` 每 15 分钟唤起 main agent 产 semantic.json（ARQ-260925-reminder-semantic-round-not-live 与 DEPLOY-RUNBOOK §8.3b） |
| `workspace-main/kidpal-schedule-answer/` | `/root/.openclaw/workspace/skills/kidpal-schedule-answer/` | **main 侧**课表问答 skill（含改课表运维须知段）——与下行 kidpal 侧同名 skill 分工不同、各占一镜像位 |
| `workspace-main/qqbot-reply-diagnosis/` | `/root/.openclaw/workspace/skills/qqbot-reply-diagnosis/` | QQ 回复诊断 skill |
| `workspace-kidpal/kidpal-schedule-answer/` | `/root/.openclaw/workspace-kidpal/skills/kidpal-schedule-answer/` | **板侧** kidpal agent 的课表问答流程 skill（2026-09-25 落地，loopback 探针实测生效） |

修改 skill 后：先改本仓镜像（进 review），再 `deploy.sh` 铺放到 ECS live 路径——顺序别反。新增 live skill 时同步加 manifest 条目 + 本表一行。
