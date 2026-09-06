---
name: kidpal-nvs-ops
description: Inspect and surgically patch the KidPal ESP32-S3 NVS partition — correct page/entry layout, the ACTIVE-page magic false-positive that made a patch silently no-op, and the websocket-namespace version semantics. Use when reading/patching NVS (url/token/version), diagnosing protocol-version issues, or verifying a flash actually landed.
---

# KidPal NVS Ops

## Partition
- NVS at flash offset **`0x9000`**, size **`0x4000`** (16 KB = 4 pages of 4096).
- Read/flash with esptool (see [[kidpal-idf-env]]):
  `"$IDFPY" -m esptool --chip esp32s3 --port COM5 read_flash 0x9000 0x4000 nvs.bin`

## Page & entry layout (verified by byte-level dump)
- **ACTIVE page: first byte `0xfe`** (state word `0xfffffffe` little-endian → bytes `fe ff ff ff`).
  - ⚠️ **FALSE-POSITIVE LESSON:** an earlier patch script gated on `state[:2] == b"\xfe\x83"`. Real bytes are `\xfe\xff`, which never matches → every page skipped → **patch silently did nothing** while logs implied success. Always gate on `data[base] == 0xfe`, and **verify the patch landed by re-reading** (don't trust the script's own "patched" print).
- Entries start at `base + 64` (32-byte page header + 32-byte bitmap), each **32 bytes**, up to 126/page.
- Entry layout: `[0]=ns_idx [1]=type [2]=span [3]=chunk [4:8]=CRC32 [8:24]=key(16B) [24:32]=data(8B)`.
- Namespace declaration entry: `type==0x01`, `ns_idx==0`, key = namespace name; its data byte assigns the namespace index (e.g. `websocket` → ns 2).
- Type codes: `0x14`=i32 (what `nvs_get_i32` / `Settings::GetInt` reads), `0x21`=string (blob-like, spans multiple entries), `0x01`=u8/namespace.

## websocket namespace (the keys that matter)
`url` (string, the wss gateway), `token` (string, device Bearer), `version` (**i32**).
- **`Settings::GetInt` uses `nvs_get_i32`** → only reads `type==0x14`. A `version` written as `u8` (`0x01`) is **unreadable** → GetInt returns 0 → firmware keeps default `version_=1`.

## Protocol version semantics (direction-B)
- `websocket_protocol.cc`: `version_` defaults to 1; `OpenAudioChannel()` reads NVS `version` and overrides only if non-zero.
- **Official xiaozhi server 0.9.6 sends RAW opus over ws** (no v2 binary header on the ws path; headers only for MQTT gateway). Native devices therefore use **version 1** (OTA response carries no version field).
- version **2/3** = device expects a binary frame header → it parses raw opus as a header → garbage `payload_size` → (before hardening) `bad_alloc` crash; (after hardening in OnData) `WS v2 frame payload_size invalid` and **all TTS audio dropped**.
- **Correct value = 1.** If you see `with version: 2` + `payload_size invalid` floods, the NVS version is wrong — flash it to i32 `1`.
- Governance note: whether `version` should stay a settable NVS key at all (vs a code-pinned constant, since the direction-B gateway always speaks v1) is tracked in `docs/ARCH-REVIEW-QUEUE.md` ARQ-260906-protocol-version-pinned-constant.

## Patch recipe (surgical, preserves WiFi creds)
Read live → patch only the version entry's i32 → recompute entry CRC → write back. CRC self-calibrates against a known-good entry: `zlib.crc32(e[8:32], zlib.crc32(e[0:4], 0xFFFFFFFF))`. Working script: `c:/tmp/patch_nvs_version.py` (fix its page-magic to `0xfe` before trusting it). Then **re-read and re-dump to confirm value==1** before declaring done.
