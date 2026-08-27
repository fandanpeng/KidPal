---
name: codebase-memory-impact
description: "Use when the user wants to know what will break if they change something, or needs safety analysis before editing code. Examples: \"Is it safe to change X?\", \"What depends on this?\", \"What will break?\". Routes: trace_path for blast radius, detect_changes for pre-commit check."
---

# Impact Analysis with CodebaseMemory

## When to Use

- "Is it safe to change this function?"
- "What will break if I modify X?"
- "Show me the blast radius"
- "Who uses this code?"
- Before making non-trivial code changes
- Before committing — to understand what your changes affect

## Workflow

```
1. trace_path({function_name: "X", direction: "inbound"})  → What depends on this
2. trace_path({function_name: "X", direction: "outbound"}) → What this depends on
3. detect_changes()                                          → Map git changes to affected symbols
4. Assess impact and report to user
```

## Tools

**trace_path** — the primary tool for blast radius:

```
trace_path({
  function_name: "buildDisplayJson",
  direction: "inbound",
  depth: 3
})

→ callers (hop 1):
  - handler (register-all.ts)
  - POST (leads/route.ts)
  - m0.5-cutover-db.spec (test)
```

**detect_changes** — git-diff based impact analysis:

```
detect_changes()

→ changed_files: [7 files]
→ impacted_symbols: [12 symbols]
→ depth: 2
```

## Example: "What breaks if I change buildDisplayJson?"

```
1. trace_path({function_name: "buildDisplayJson", direction: "inbound"})
   → callers: handler, POST (direct callers)
2. detect_changes()
   → Verify changes only hit expected symbols
3. Report: 2 direct callers, recommend re-running lead tests
```
