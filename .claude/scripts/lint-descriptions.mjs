#!/usr/bin/env node
// lint-descriptions.mjs — warning-level description linter (§H 硬门5 / F-D1 / RC-2 / [G1])
// Flags process-narrative smells + gross over-length in agent frontmatter descriptions,
// per the obra/superpowers "description = when-to-use, not workflow" discipline.
// WARNING-LEVEL (policy-as-code, NOT a gate): dirty → flag + name file + EXIT 0.
//   exit != 0 ONLY on the linter's own crash. It is never a per-item human gate (RC-2).
// D-C: scans .claude/agents/** but HARD-EXCLUDES architect.md (self-recursion boundary).
// [拍板2]: supervisor is on the explicit exemption whitelist (already-compliant, verify-only).
// [G1]: supervisor + architect-reviewer must clean-exit (no flags) — proven by lint-fixtures.
// owner=architect (G3). Landing-time/manual tool (no CI/pre-commit per human 2026-07-08).
import { readFileSync, readdirSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join, basename } from 'node:path';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
const dirArg = process.argv.indexOf('--dir');
const SCAN_DIR = dirArg !== -1 ? process.argv[dirArg + 1] : join(ROOT, '.claude', 'agents');

// D-C hard-exclude + [拍板2] explicit exemption whitelist.
const HARD_EXCLUDE = new Set(['architect']);        // never scanned (self-recursion)
const EXEMPT = new Set(['supervisor']);             // scanned but whitelisted clean
const MAX_DESC_CHARS = 320;                          // gross over-length backstop (non-whitespace)

function descriptionOf(txt) {
  const m = txt.match(/^description:\s*(.*)$/m);
  return m ? m[1].trim().replace(/^["']|["']$/g, '') : null;
}
// Process-narrative smells (the real superpowers concern) — FORM, not raw length:
// directional workflow chains, explicit ordered sequences, output-listing "+" chains.
// Length is only a generous backstop (set above already-compliant agents per [G1]).
function smells(desc) {
  const out = [];
  if (/->|→/.test(desc)) out.push('workflow-chain arrow (→/->)');
  if (/先[^。；]{0,50}再[^。；]{0,50}/.test(desc)) out.push('ordered "先…再…" sequence');
  if ((desc.match(/\+/g) || []).length >= 3) out.push('output-listing "+" chain (≥3)');
  const chars = desc.replace(/\s/g, '').length;
  if (chars > MAX_DESC_CHARS) out.push(`over-length (${chars} chars > ${MAX_DESC_CHARS})`);
  return out;
}

const flagged = [];
let scanned = 0, exempt = 0, excluded = 0;
for (const f of readdirSync(SCAN_DIR).filter((f) => f.endsWith('.md')).sort()) {
  const name = basename(f, '.md');
  if (HARD_EXCLUDE.has(name)) { excluded++; continue; }
  const desc = descriptionOf(readFileSync(join(SCAN_DIR, f), 'utf8'));
  if (desc == null) continue;
  scanned++;
  if (EXEMPT.has(name)) { exempt++; continue; }
  const s = smells(desc);
  if (s.length) flagged.push({ file: f, smells: s });
}

console.log(`lint-descriptions [warn-level]: scanned ${scanned}, exempt ${exempt}, excluded ${excluded} (architect.md), flagged ${flagged.length}`);
for (const { file, smells } of flagged) console.log(`  ⚠ ${file}: ${smells.join('; ')}`);
if (flagged.length === 0) console.log('  ✓ clean');
process.exit(0); // warning-level: always exit 0 (except on crash)
