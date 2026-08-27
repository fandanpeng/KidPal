---
name: codebase-memory-guide
description: "Use when the user asks about CodebaseMemory itself — available tools, how to query the knowledge graph, graph schema, or workflow reference. Examples: \"What tools are available?\", \"How do I use CodebaseMemory?\""
---

# CodebaseMemory Guide

Quick reference for all CodebaseMemory MCP tools, the knowledge graph schema, and available skills.

## Always Start Here

1. **Check `index_status`** — verify the project is indexed and ready
2. **Match your task to a skill below** and **read that skill file**
3. **Follow the skill's workflow**

## Skills

| Task | Skill to read |
|---|---|
| Understand architecture / "How does X work?" | `codebase-memory-exploring` |
| Blast radius / "What breaks if I change X?" | `codebase-memory-impact` |
| Tools, schema, workflow reference | `codebase-memory-guide` (this file) |
| Index, re-index, status CLI commands | `codebase-memory-cli` |

## Tools Reference

| Tool | What it gives you |
|---|---|
| `search_graph` | Find symbols by name/pattern/natural language query |
| `search_code` | Graph-augmented text search (grep + graph enrichment) |
| `trace_path` | Inbound/outbound call chains — who calls X, what X calls |
| `get_code_snippet` | Read source for a specific symbol by qualified name |
| `get_architecture` | Languages, packages, routes, clusters in one call |
| `query_graph` | Complex Cypher queries (multi-hop, aggregations) |
| `detect_changes` | Git diff → affected symbols |
| `manage_adr` | Persist Architecture Decision Records |
| `index_repository` | Build or update the knowledge graph |
| `list_projects` | List all indexed projects |
| `index_status` | Check indexing status |
| `delete_project` | Remove a project from the index |
| `ingest_traces` | Ingest runtime traces to enrich the graph |
| `get_graph_schema` | Node labels, edge types, properties |

## Graph Schema

**Node labels:** Section, Variable, Function, Field, File, Module, Type, Interface, Folder, Class, Method, Enum, Branch, Project

**Edge types:** DEFINES, USAGE, CALLS, IMPORTS, CONTAINS_FILE, CONTAINS_FOLDER, SEMANTICALLY_RELATED, WRITES, SIMILAR_TO, DEFINES_METHOD, RAISES, FILE_CHANGES_WITH, INHERITS, CONFIGURES, TESTS_FILE, THROWS, HAS_BRANCH

Query the authoritative schema: `get_graph_schema()`

### Query Graph (Cypher)

```cypher
MATCH (f:Function {name: "buildDisplayJson"})-[r:CALLS]->(g)
RETURN g.name, g.file_path

MATCH (f:Function)-[:CALLS]->(f)  -- callers
WHERE f.name CONTAINS "Lead"
RETURN f.qualified_name, f.file_path
```

## CLI Quick Reference

```bash
codebase-memory-mcp cli index_repository '{"repo_path":".","mode":"full"}'  # Index/re-index
codebase-memory-mcp cli list_projects                                       # Check status
codebase-memory-mcp cli search_graph '{"query":"auth"}'                     # Search
codebase-memory-mcp --version                                               # Version check
```
