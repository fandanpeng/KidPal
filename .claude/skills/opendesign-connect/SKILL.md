---
name: opendesign-connect
description: "Use when: OpenDesign MCP 失联排障与恢复——od_* 工具报 OD daemon unreachable / fetch failed、open design 连不上、设计稿生成前确认联通、opendesign 端口失效。触发词: open design 连不上, OD daemon unreachable, fetch failed, opendesign 端口, 设计稿生成, open design MCP。边界: 只做联通诊断与恢复指引，不做设计生成规约（那在 .claude/references/uiux-opendesign-loop.md）；修复最后一步（/mcp 重连）需 human 操作。"
---

# OpenDesign MCP 联通排障

Open Design 桌面应用的 daemon 端口是**动态**的：应用每次重启换端口，而 MCP 配置（`~/.claude.json` 的 `OD_DAEMON_URL`）是静态的——失联几乎总是「配置指向旧端口」。实证：2026-07 practice（`changes/arch/gov-review-uiux-opendesign/PRACTICE-S5-OPENDESIGN.md` §2「不要硬编码端口」）+ 2026-08-18 S5 首刀实测（50274 → 57299）。

## 失败信号识别

- `OD daemon unreachable: fetch failed`（任一 `od_*` 工具返回）= daemon 端口失联，MCP server 进程本身在线。
- 先确认桌面应用在跑：`tasklist | grep "Open Design.exe"` 无结果 → 先起应用，不起应用后面全白搭。

## 端口发现三步（每步有可检查终点）

1. **找进程 PID** — `tasklist | grep "Open Design.exe" | awk '{print $3}'`（进程名含空格，PID 在第 3 列）。终点：一组 PID。
2. **找监听端口** — `netstat -ano | grep LISTENING | grep -E " (<pid1>|<pid2>|...)$"`。终点：若干 `127.0.0.1:<port>` 候选。
3. **认领 daemon** — 对每个候选 `curl -s -m 5 "http://127.0.0.1:<port>/api/mcp/install-info"`：返回 JSON 且含 `daemonUrl` 的 = daemon（`daemonUrl` 字段即权威端口，两个 sidecar 都会报同一个值）；返回 HTML 的是 web sidecar，不是 daemon。终点：拿到当前 `daemonUrl`。

## 配置更新与生效

4. **改配置** — `~/.claude.json` 里 `open-design` MCP 的 `env.OD_DAEMON_URL` 改为新端口；顶层 `mcpServers` 与 `projects[*].mcpServers` 两段都查。终点：`grep -o "<旧端口>\|<新端口>" ~/.claude.json` 只剩新端口；仓库级 `.mcp.json` 也 grep 一遍确认无残留配置源。
5. **生效（需 human）** — 已运行的 MCP server 进程持旧 env，改配置不热生效：请用户 `/mcp` 重连 open-design（或重启会话）。终点：`od_list_projects` 返回项目列表 = 绿。

## 约束（must / never）

- 永不把**当前/实际** daemon 端口硬编码进仓库内任何文件（含 docs/reference/skill）——端口只存在于用户级 `~/.claude.json` 与本 skill 的排障流程；上文「50274 → 57299」类历史实证引文仅作源锚，不是可复用的端口值。
- daemon 端口发现全失败 / install-info 全不响应 → 交 human（可能是应用版本问题），不猜端口。

## 生成期速览（联通恢复后常用事实）

- `od_generate_design` 单页 5–10 分钟级长时延；多页可并行发起（同 projectId 下多个 conversation）。（源锚：S5 首刀实测 2026-08-18/19）
- `od_save_project_file` 同名再存 = 覆盖更新（mtime/size 变）。（源锚：S5 首刀实测 2026-08-18/19）
- Open Design 应用内项目列表可见性 eventually-consistent：交接留直链（projectId + 文件路径），不依赖用户在应用里翻到。（源锚：PRACTICE-S5-OPENDESIGN.md §3）
