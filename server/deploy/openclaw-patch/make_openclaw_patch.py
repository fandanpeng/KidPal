#!/usr/bin/env python3
"""KidPal C1: generate the OpenClaw char-stroke hold-and-replace TS patch (mechanism B).

Targets the DEPLOYMENT dist bundle (OpenClaw 2026.8.1 npm global install):
  /root/.npm-global/lib/node_modules/openclaw/dist/openai-http-<hash>.js   (ESM chunk)
The local reference snapshot lacks src/gateway, so anchors below are the 2026.8.1 dist
reply-exit shapes reported by the live probe:
  - NON-STREAM reply exit: `const content = resolveAgentResponseCommentary(result) || ...`
  - STREAM  delta exit   : `const content = typeof text === "string" && text.startsWith(streamedAssistantText) ? text.slice(...`
This generator is DRIFT-GUARDED: it greps each required anchor and REFUSES (exit 3) if a
required anchor is not present — the apply step (DEPLOY-RUNBOOK.md) must `grep` first and
STOP on mismatch rather than produce a property-binding diff.

Semantics mirror server/deploy/openclaw-patch/char_stroke_lib.py (the offline-tested
frozen contract): extract minimal char block -> static 字库 LRU -> replace with assembled
FULL D3 JSON. HOLD-AND-REPLACE: the gateway never emits the minimal block as-is.
"""

from __future__ import annotations

import sys

MARKER = "// [KidPal C1] char-backed hold-and-replace (mechanism B, dist 2026.8.1)"

# Anchors (substring, case-sensitive) into the 2026.8.1 dist chunk.
NONSTREAM_ANCHOR = "resolveAgentResponseCommentary(result)"
STREAM_ANCHOR = "text.startsWith(streamedAssistantText)"

# 2026.8.1 前置 guard anchors: 若 dist 连这些都不含, 必是版本漂移, 直接停。
GUARD_ANCHORS = [
    "resolveAgentResponseCommentary(result)",
    "streamedAssistantText",
]

# 三处真实绑定（2026.8.1 dist 实测形状；每处替换前 assert 唯一命中, 对不上 exit 3）。
# 1) 纯非流式响应构造 (live 探针走它, 唯一):
#    `content: resolveAgentResponseText(result)` -> 包 k1cRewriteText
NONSTREAM_SITE_A = "content: resolveAgentResponseText(result)"
NEW_NONSTREAM_SITE_A = "content: k1cRewriteText(resolveAgentResponseText(result))"

# 2) 流式处理器 `!sawAssistantDelta` 收尾 flush (唯一):
#    `const content = resolveAgentResponseCommentary(result) || bufferedReplaceableAssistantContent
#      || resolveAgentResponseText(result) || "No response from OpenClaw.";`
#    -> 整个 RHS 包 k1cRewriteText
NONSTREAM_SITE_B = ("resolveAgentResponseCommentary(result) || bufferedReplaceableAssistantContent"
                    " || resolveAgentResponseText(result) || \"No response from OpenClaw.\"")
NEW_NONSTREAM_SITE_B = ("k1cRewriteText(resolveAgentResponseCommentary(result)"
                        " || bufferedReplaceableAssistantContent || resolveAgentResponseText(result)"
                        " || \"No response from OpenClaw.\")")

# 3) 流式 delta 真实写出点 (唯一; ECS 实测为多行 TAB 缩进块, 前导 sawAssistantDelta = true;
#    用它一起锚定防与其他同形多行 writeAssistantContentChunk 块混淆):
#        sawAssistantDelta = true;
#        writeAssistantContentChunk(res, {
#           ...streamIdentity,
#           content
#        });
STREAM_SITE = ("sawAssistantDelta = true;\n"
                "\t\t\twriteAssistantContentChunk(res, {\n"
                "\t\t\t\t...streamIdentity,\n"
                "\t\t\t\tcontent\n"
                "\t\t\t});")
NEW_STREAM_SITE = ("sawAssistantDelta = true;\n"
                   "\t\t\tconst c1Delta = k1cStreamStep({ id: streamIdentity.runId }, content);\n"
                   "\t\t\tif (!c1Delta) return; /* fence 未闭合, 缓冲中 */\n"
                   "\t\t\twriteAssistantContentChunk(res, {\n"
                   "\t\t\t\t...streamIdentity,\n"
                   "\t\t\t\tcontent: c1Delta\n"
                   "\t\t\t});")


def _apply_site(src: str, old: str, new: str, what: str) -> str:
    """唯一命中替换; 0/多命中 -> 版本漂移 exit 3, 不在错误版本上产出 binding diff。"""
    count = src.count(old)
    if count != 1:
        print(f"ERR: binding site {what!r} matched {count}x (expect 1) — OpenClaw dist 形状不符; "
              "版本漂移 → 停止, 不产出 binding diff", file=sys.stderr)
        sys.exit(3)
    return src.replace(old, new, 1)


def _helper_ts() -> str:
    """The ESM-safe TS helper injected at the top level (after a marker)."""
    return """/* ============ [KidPal C1 patch BEGIN] ============
 * $MARKER
 * HOLD-AND-REPLACE: 网关从不在回复出口发出最小 char 块成品; 查静态字库拼装为全量 D3。
 * 语义与 server/deploy/openclaw-patch/char_stroke_lib.py 一致。字库来自 warm_char_cache.py
 * 生成的 char_cache.json (默认 /root/.openclaw/workspace/kidpal/chardata/char_cache.json)。
 * ESM 安全: 本 chunk 是 ESM, 用 createRequire(import.meta.url) 取 fs/path。
 */
import { createRequire } from "node:module";
const k1cRequire = createRequire(import.meta.url);
const k1cFs = k1cRequire("node:fs");
const k1cPath = k1cRequire("node:path");

const k1cCharCache = new Map();
function k1cLoadCache() {
  if (k1cCharCache.size > 0) return k1cCharCache;
  const src = k1cPath.resolve(
    process.env.CHAR_CACHE_JSON ||
    "/root/.openclaw/workspace/kidpal/chardata/char_cache.json");
  try {
    const rows = JSON.parse(k1cFs.readFileSync(src, "utf-8"));
    for (const [ch, data] of Object.entries(rows)) k1cCharCache.set(ch, data);
  } catch { /* 字库未就绪 -> 原样放行, 设备侧防御降级 */ }
  return k1cCharCache;
}

/* 提取第一条 ```display fenced view=="char" 块 (返回完整 fenced 块文本) */
const k1cFenceRe = /```display\\s*(\\{.*?\\})\\s*```/s;
function k1cExtractBlock(text) {
  const m = k1cFenceRe.exec(text || "");
  if (!m) return null;
  try { const o = JSON.parse(m[1]); return o && o.view === "char" ? m[1] : null; }
  catch { return null; }
}

/* 整段非流式改写: 把首个 char 最小块替换为全量 D3 JSON (含 view/data 信封) */
function k1cRewriteText(text) {
  if (typeof text !== "string" || !text.includes("```display")) return text;
  const minimal = k1cExtractBlock(text);
  if (!minimal) return text;
  let ch = null;
  try { ch = JSON.parse(minimal)?.data?.char; } catch { return text; }
  if (!ch || !k1cLoadCache().has(ch)) return text;      // 未命中字库 -> 亮原样
  // 缓存值 = data 层, emit 前必须包 view/data 信封 (契约 §1.3①) —— 否则设备 view 路由拿不到 view 字段
  const enveloped = { view: "char", data: k1cLoadCache().get(ch) };
  return text.replace(k1cFenceRe,
    "```display\\n" + safeJson(enveloped) + "\\n```");
}

/* 流式 hold-and-replace 状态机: per-request accumulator。
 * 每个 delta 调 k1cStreamStep(req, deltaText); 跨 delta 缓冲到 ```display fence 闭合,
 * 闭合时以"拼装全量 JSON 单 delta"替换吐出 —— 网关永不见最小块成品。
 * 返回三态:
 *   - 透传文本: 该增量无尚未闭合的 fence (整段原样);
 *   - 空串:     已有 fence 开头但未闭合 -> 缓冲中, 调用方应 return (不写半成品 delta);
 *   - 含替换块的整段: fence 刚闭合 -> 其中 char 块已被全量 D3 JSON 替换。
 * 调用方绑定: const c1Delta = k1cStreamStep({ id: streamIdentity.runId }, content);
 *             if (!c1Delta) return; writeAssistantContentChunk(..., content: c1Delta); */
const k1cStreams = new Map();
function k1cStreamKey(req) {
  // apply 步骤绑定: 优先 request.id, 兜底单槽 (OpenClaw 顺序处理一个 agent run)。
  const id = (req && (req.id !== undefined ? req.id : req.requestId)) ?? "";
  return id === "" ? "k1c-single" : ("k1c:" + id);
}
function k1cStreamStep(req, deltaText) {
  const key = k1cStreamKey(req);
  let s = k1cStreams.get(key);
  if (!s) { s = { pending: "" }; k1cStreams.set(key, s); }
  s.pending = (s.pending || "") + (deltaText || "");
  const out = [];
  let head = 0;
  while (true) {
    const fm = k1cFenceRe.exec(s.pending.substring(head));
    if (!fm) break;
    const abs = head + fm.index;
    out.push(s.pending.slice(head, abs));               // fence 之前的原文
    const blockEnd = abs + fm[0].length;
    const full = k1cLookupBlock(fm[1]);
    out.push(full !== null ? "```display\\n" + safeJson(full) + "\\n```" : fm[0]);
    head = blockEnd;
  }
  // 剩余: 若含未闭合 fence 开头则整体缓冲 (不吐半成品), 否则直接吐
  const rest = s.pending.substring(head);
  const openIdx = rest.indexOf("```display");
  if (openIdx >= 0) {
    out.push(rest.slice(0, openIdx));
    s.pending = rest.slice(openIdx);
  } else {
    out.push(rest);
    s.pending = "";
  }
  return out.join("");
}
function k1cLookupBlock(jsonText) {
  try {
    const o = JSON.parse(jsonText);
    if (!o || o.view !== "char" || !o.data || typeof o.data.char !== "string") return null;
    const ch = o.data.char;
    if (!k1cLoadCache().has(ch)) return null;
    // 缓存值 = data 层 (char_cache.json = assemble_char 产物, 不带信封); emit 前必须包
    // view/data 信封 (契约 §1.3①) —— k1cStreamStep/k1cRewriteText 两路径一致。
    return { view: "char", data: k1cLoadCache().get(ch) };
  } catch { return null; }
}
/* JSON.stringify with null-replacer, ensure_ascii irrelevant here (TS native unicode ok) */
function safeJson(v) {
  try { return JSON.stringify(v); } catch { return String(v); }
}
/* ============ [KidPal C1 patch END] ============ */
"""


def _check_guards(src: str) -> None:
    for a in GUARD_ANCHORS:
        if a not in src:
            print(f"ERR: required anchor missing ({a!r}) — OpenClaw dist is NOT 2026.8.1 "
                  "matching shape; STOP, do not produce a binding diff. "
                  "(grep 锚点对不上就停, 防版本漂移)", file=sys.stderr)
            sys.exit(3)


def apply(target: str, stream_anchor: str = STREAM_ANCHOR) -> None:
    with open(target, "r", encoding="utf-8") as f:
        src = f.read()
    if MARKER in src:
        print(f"{target} already patched (marker present).")
        return

    _check_guards(src)   # 版本漂移闸: 前置锚点对不上就停

    helper = _helper_ts().replace("$MARKER", MARKER)
    src += "\n" + helper

    # 三处真实绑定 (每处唯一命中, 对不上 exit 3)
    src = _apply_site(src, NONSTREAM_SITE_A, NEW_NONSTREAM_SITE_A,
                      "非流式纯响应 `content: resolveAgentResponseText(result)`")
    src = _apply_site(src, NONSTREAM_SITE_B, NEW_NONSTREAM_SITE_B,
                      "流式 !sawAssistantDelta 收尾 flush RHS")
    src = _apply_site(src, STREAM_SITE, NEW_STREAM_SITE,
                      "流式 delta 写出点 writeAssistantContentChunk")

    # 先落盘, 再打印摘要: 底层 markdown/中文 print 在非 UTF-8 控制台可能失败, 不阻塞补丁落地。
    with open(target, "w", encoding="utf-8", newline="\n") as f:
        f.write(src)

    print(f"patched {target}: non-stream A/B + stream delta 三处绑定落地; "
          "ESM helper 追加 (marker 幂等); bind per DEPLOY-RUNBOOK.md section 3.1 复核")

    print(f"stream-anchor-present={'yes' if stream_anchor in src else 'NO'}"
          " (three bindings via unique-hit; stream_anchor only recorded)")

    if stream_anchor not in src:
        print("cache: stream_anchor not present in post-patch source (informational)")


if __name__ == "__main__":
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("usage: make_openclaw_patch.py <dist-chunk.js> [stream-anchor]", file=sys.stderr)
        sys.exit(2)
    apply(sys.argv[1], sys.argv[2] if len(sys.argv) == 3 else STREAM_ANCHOR)


if __name__ == "__main__":
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("usage: make_openclaw_patch.py <dist-chunk.js> [stream-anchor]", file=sys.stderr)
        sys.exit(2)
    apply(sys.argv[1], sys.argv[2] if len(sys.argv) == 3 else STREAM_ANCHOR)