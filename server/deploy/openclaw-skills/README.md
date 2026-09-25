# OpenClaw workspace skills 镜像（仓内可 diff 副本）

本目录镜像部署在 ECS OpenClaw workspace 的 skills（live 位置见各子目录 README 注记）。
**live 部署面目前是手动的**（直接写入 `/root/.openclaw/workspace-kidpal/skills/` 等），
不经 `apply-xiaozhi-overlay.sh` 渲染——镜像的目的是让活体 skill 可 diff、可追溯，
ECS 重装/迁移时从本目录恢复。

- `workspace-kidpal/kidpal-schedule-answer/` → live `/root/.openclaw/workspace-kidpal/skills/kidpal-schedule-answer/`
  （板侧 kidpal agent 的课表问答流程 skill，2026-09-25 落地，loopback 探针实测生效；
  main workspace 的同名 skill 服务家庭侧/QQ 录入路径，两者分工不同、并存不互斥）

修改 skill 后：先改本仓镜像（进 review），再同步到 ECS live 路径——顺序别反。
