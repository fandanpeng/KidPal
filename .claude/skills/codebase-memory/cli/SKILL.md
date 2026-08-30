---
name: codebase-memory-cli
description: "Use when the user needs to run CodebaseMemory CLI commands like index/re-index a repo, check status, or list indexed projects. Examples: \"Index this repo\", \"Re-index the codebase\", \"Check index status\""
---

# CodebaseMemory CLI Commands

All commands use `codebase-memory-mcp cli <tool> <args>`.

## Commands

### index_repository — Build or refresh the index

```bash
codebase-memory-mcp cli index_repository '{"repo_path":"C:/AI/KidPal","mode":"full"}'
```

Modes:
- `full` — all files + similarity/semantic edges
- `moderate` — filtered files + similarity/semantic
- `fast` — filtered files, no similarity/semantic

Run from the project root. This parses all source files, builds the knowledge graph, and stores it in `~/.cache/codebase-memory-mcp/`.

### list_projects — Show all indexed projects

```bash
codebase-memory-mcp cli list_projects
```

Returns each project with node/edge counts, git info, and database size.

### index_status — Check if a project is indexed

```bash
codebase-memory-mcp cli index_status '{"project":"KidPal"}'
```

### delete_project — Remove a project from the index

```bash
codebase-memory-mcp cli delete_project '{"project":"KidPal"}'
```

## Configuration

```bash
codebase-memory-mcp config list                # View all config
codebase-memory-mcp config set auto_index true # Auto-index on session start
codebase-memory-mcp config reset auto_index    # Reset to defaults
```

## Environment Variables

| Variable | Default | Description |
|---|---|---|
| `CBM_CACHE_DIR` | `~/.cache/codebase-memory-mcp` | Database storage |
| `CBM_LOG_LEVEL` | `info` | `debug` / `info` / `warn` / `error` / `none` |
| `CBM_WORKERS` | auto | Parallel indexing threads (1-256) |

## Troubleshooting

- **Index seems stale**: Re-run `index_repository` with `mode: "full"`
- **MCP tools not available**: Restart Claude Code after indexing
- **Permission prompts**: Ensure `mcp__codebase-memory-mcp__*` is in settings allow list
