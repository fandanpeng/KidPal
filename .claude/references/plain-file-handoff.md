# Plain-File Handoff Reference

> Use when sending design/architecture work to another repo or another machine where commit hashes may not match.

The handoff package must be understandable without the author present. A zip that relies on memory, chat context, or matching git history is invalid.

## Package Shape

Use this layout for every cross-repo handoff zip. The zip is a **generated transfer artifact** and should be written outside the repo by default, for example `D:\AI-work\<handoff-id>.zip`; do not commit/store it under `delivery/changes/` unless a human explicitly asks to archive the binary.

```text
<handoff-id>.zip
  HANDOFF-NOTE.md
  DELETE-LIST.txt
  files/
    <repo-relative final-state snapshots>
```

Rules:

- `HANDOFF-NOTE.md` is the only first-entry document.
- `DELETE-LIST.txt` contains one repo-relative path per line. If nothing must be deleted, include the file with `# none`.
- `files/` mirrors repo-relative paths. The target repo copies `files/*` into its root.
- Do not put payload files at the zip root except `HANDOFF-NOTE.md` and `DELETE-LIST.txt`.
- Do not include git commits, `git format-patch`, or `git am` instructions for cross-repo handoff unless the user explicitly asks for patch mode.
- Keep any auditable source notes in the repo if needed; keep the generated zip outside the repo.

## HANDOFF-NOTE Required Sections

1. **What This Package Is**: source repo, target repo, date, mode, and what the package does not mean.
2. **Zip Layout**: exact expected structure.
3. **What Changed**: file-by-file behavior/design delta, grouped by surface.
4. **Apply To Target Repo**: copy steps, delete steps, merge-vs-overwrite warnings, and target-only edits.
5. **Verify The Sync**: commands and expected outputs.
6. **Gate State**: contract/architecture gate status, conditions, and what is not closed.
7. **Execution Order**: what dev should do first, second, third.
8. **Excluded / Not Included**: files intentionally left out and why.

The note must avoid vague phrases like "pull this commit" or "as discussed". It should be executable by someone who did not read the chat.

## Validation Before Delivery

Before handing off, run a zip layout check equivalent to:

```powershell
$entries = tar -tf '<handoff-id>.zip'
$must = @('HANDOFF-NOTE.md', 'DELETE-LIST.txt')
foreach ($entry in $must) { if ($entries -notcontains $entry) { throw "missing $entry" } }
$badRoot = $entries | Where-Object { $_ -notmatch '^(HANDOFF-NOTE\.md|DELETE-LIST\.txt|files/)' }
if ($badRoot) { throw "unexpected root entries: $($badRoot -join ', ')" }
```

Also validate any executable surface that the handoff changes; for contract handoffs re-read the changed section verbatim against the consumer expectations.

## Cross-Repo Safety

- Do not assume branches, commit hashes, or line endings match.
- Prefer final-state snapshots over diffs.
- If a file is likely to diverge locally (`scope-map.json`, queue files, settings), say "merge this section" instead of "overwrite".
- Put raw review/command output under the copied change folder's `evidence/`, not only in `HANDOFF-NOTE.md`.
- Rebuild the zip after every late documentation or rules change; otherwise the folder and zip drift.

## Minimal Build Recipe

```powershell
$stage = Join-Path $env:TEMP '<handoff-id>'
Remove-Item -Recurse -Force $stage -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path (Join-Path $stage 'files') | Out-Null
Copy-Item '<change-folder>/HANDOFF-NOTE.md' (Join-Path $stage 'HANDOFF-NOTE.md')
Copy-Item '<change-folder>/DELETE-LIST.txt' (Join-Path $stage 'DELETE-LIST.txt')
# Copy repo-relative snapshots into $stage/files/...
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath 'D:\AI-work\<handoff-id>.zip' -Force
```