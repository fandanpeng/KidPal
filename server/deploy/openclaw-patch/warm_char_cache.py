#!/usr/bin/env python3
"""KidPal C1: warm the static char cache (char_cache.json) from the makemeahanzi dataset.

Reads ${CHAR_DATA_DIR}/graphics.txt + dictionary.txt (line-delimited JSON, one record
per line) and produces a char_cache.json: { char: <assemble_char full D3 data> } for
EVERY char present in both records (9574 字全量, 不抽样). Reuses the frozen assemble
semantics via server/deploy/openclaw-patch/char_stroke_lib.assemble_char so the
deployment cache format == the offline-tested format == the gateway patch reads it.

Usage (ECS python3, offline, no third-party import):
  python3 warm_char_cache.py /root/.openclaw/workspace/kidpal/chardata [out.json]
Default out.json = same dir as CHAR_DATA_DIR/char_cache.json, keyed by char.
"""

# NOTE (260920-m2-charwindow-display-fix rework, python 3.6 compat): this script runs under
# the ECS system python3 (CPython 3.6). Must stay python 3.6-compatible: `from __future__
# import annotations` removed (PEP 563 = 3.7+ SyntaxError); `cache: dict[str, dict]` PEP 585
# builtin subscript removed (3.9+ TypeError on 3.6) -> plain `cache = {}`. Keep the existing
# CPython-3.6 dict-insertion-order dependency (standard; assert order not specified in spec).

import csv  # noqa: F401 (record delimiter for tab/semicolon tolerant parse-path, unused)
import json
import os
import sys

from char_stroke_lib import assemble_char, FULL_DATA_KEYS, REQUIRED_DATA_KEYS


def _load_index(path: str, key_col: str):
    """Return {character: record} from a line-delimited JSON file."""
    out: dict = {}
    if not os.path.isfile(path):
        return out
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                rec = json.loads(line)
            except (ValueError, TypeError):
                continue
            ch = rec.get("character")
            if ch:
                out[ch] = rec
    return out


def main() -> int:
    data_dir = sys.argv[1] if len(sys.argv) > 1 else (
        os.environ.get("CHAR_DATA_DIR", "/root/.openclaw/workspace/kidpal/chardata"))
    out_path = sys.argv[2] if len(sys.argv) > 2 else os.path.join(data_dir, "char_cache.json")

    graphics = _load_index(os.path.join(data_dir, "graphics.txt"), "character")
    dictionary = _load_index(os.path.join(data_dir, "dictionary.txt"), "character")
    print(f"loaded graphics={len(graphics)} dictionary={len(dictionary)} from {data_dir}")

    cache: dict = {}
    skipped_mismatch = 0
    for ch, g in graphics.items():
        d = dictionary.get(ch)
        if d is None:
            continue
        # guards: 必需键齐全 + graphics/dictionary 字符一致 (assemble_char 内会再次校验)。
        # pinyin/radical 为字典必需键; strokes(笔画数) 与 structure 由 assemble_char 按
        # 数据集实况回退 (metrics ⊕ path 数 / decomposition), 故不在此强制。
        if not set(("strokes", "medians")) <= set(g):
            continue
        if not set(("pinyin", "radical")) <= set(d):
            continue
        try:
            data = assemble_char(g, d)
        except ValueError:
            skipped_mismatch += 1
            continue
        if set(data.keys()) != FULL_DATA_KEYS:
            continue
        if not REQUIRED_DATA_KEYS <= set(data):
            continue
        cache[ch] = data

    os.makedirs(os.path.dirname(out_path) or ".", exist_ok=True)
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(cache, f, ensure_ascii=False, separators=(",", ":"))
    print(f"wrote {len(cache)} chars -> {out_path} (skipped_mismatch={skipped_mismatch})")
    return 0


if __name__ == "__main__":
    sys.exit(main())