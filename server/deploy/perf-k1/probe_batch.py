#!/usr/bin/env python3
"""PERF-K1 live probe-batch executor.

冻结契约（FROZEN OFFLINE CONTRACT §5）：live 探针批执行器（发 HTTP、N≥3、计时、
主机快照、session 计数）；须 `--check-prompt` 模式：离线解析生产 prompt + 打印其
路径与归一 sha256，exit 0，零网络。

离线自检:
    python probe_batch.py --check-prompt

live（须在 ECS 上跑，因 127.0.0.1:18789 为 loopback）:
    python probe_batch.py --segment pre-p1 --layer a1 --n 3 \
        --out /tmp/perf-k1-run/probe-batches/pre-p1.json

探针必带生产 v6 system prompt 逐字（评审 D1）；题面标准 3 题（闲聊/课表/天气）。
批次产出 schema == probe_gate.evaluate_batch 的入参 batch。
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import re
import subprocess
import sys
import time
import urllib.request
from pathlib import Path

def _default_repo_root() -> Path:
    """从本文件向上找仓根（含 server/deploy/provider.config.yaml）；找不到时退回本目录。"""
    here = Path(__file__).resolve()
    for parent in here.parents:
        if (parent / "server" / "deploy" / "provider.config.yaml").is_file():
            return parent
    return here.parent


REPO_ROOT = _default_repo_root()
DEFAULT_ENDPOINT = "http://127.0.0.1:18789/v1/chat/completions"
DEFAULT_MODEL = "openclaw/default"

# 标准 3 题（冻结口径）。
QUESTIONS = {
    "chat": "你好呀",
    "schedule": "明天有什么课程?",
    "weather": "今天西安天气怎么样?",
}

# 天气真实检索的弱信号（温度/天气词）——只作 auto 兜底，真证据仍以网关 tavily 轮
# 日志为准（执行者须交叉核验，勿仅凭此判 retrieval）。
_WEATHER_HINTS = re.compile(
    r"(℃|度|霾|小雨|中雨|大雨|雷|多云|阴|晴|风|湿度|空气质量|紫外线|气温)")
_NOTE_HINTS = re.compile(r"(备注|带|记得|钢琴|琴谱)")


def _load_gate():
    """按路径加载同目录 probe_gate.py（离线可 import，勿依赖 sys.path）。"""
    path = Path(__file__).resolve().parent / "probe_gate.py"
    spec = importlib.util.spec_from_file_location("kidpal_probe_gate_perf_k1_cli", path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


# --------------------------------------------------------------------------- #
# --check-prompt（离线、零网络）
# --------------------------------------------------------------------------- #

def check_prompt(prompt_file=None) -> int:
    gate = _load_gate()
    artifact = Path(prompt_file) if prompt_file else gate.resolve_prompt_artifact(REPO_ROOT)
    sha = gate.prompt_sha256(artifact)
    print(f"repo_root: {REPO_ROOT}")
    print(f"prompt name: {artifact.name}")
    print(f"prompt path: {artifact}")
    print(f"prompt sha256 (LF-normalized): {sha}")
    print(f"prompt size bytes: {artifact.stat().st_size}")
    return 0


# --------------------------------------------------------------------------- #
# 主机快照 / session 计数（ECS 侧）
# --------------------------------------------------------------------------- #

def _host_snapshot() -> dict:
    snap = {"free_m": "", "swap_used_mb": None, "mem_available_mb": None}
    try:
        out = subprocess.run(["free", "-m"], capture_output=True, text=True,
                             timeout=15).stdout
        snap["free_m"] = out.strip()
        for line in out.splitlines():
            if line.lower().startswith("mem:"):
                parts = line.split()
                if len(parts) >= 7:
                    snap["mem_available_mb"] = int(parts[6])
            if line.lower().startswith("swap:"):
                parts = line.split()
                if len(parts) >= 4:
                    snap["swap_used_mb"] = int(parts[2])
    except Exception as e:  # noqa: BLE001
        snap["free_m"] = f"<free failed: {e!r}>"
    snap["memory_pressure_warnings_30m"] = _count_pressure_warnings()
    return snap


def _count_pressure_warnings() -> int:
    try:
        out = subprocess.run(
            ["bash", "-lc",
             "journalctl -u openclaw-gateway --since '30 min ago' 2>/dev/null "
             "| grep -ci 'memory pressure'"],
            capture_output=True, text=True, timeout=30).stdout.strip()
        return int(out) if out.isdigit() else 0
    except Exception:  # noqa: BLE001
        return 0


def _session_count() -> int:
    try:
        out = subprocess.run(["openclaw", "sessions", "list"], capture_output=True,
                             text=True, timeout=60).stdout
        m = re.search(r"Sessions listed:\s*\d+\s*of\s*(\d+)", out)
        if m:
            return int(m.group(1))
        m = re.search(r"Sessions listed:\s*(\d+)", out)
        if m:
            return int(m.group(1))
    except Exception:  # noqa: BLE001
        pass
    return -1


# --------------------------------------------------------------------------- #
# 单次 HTTP 探针
# --------------------------------------------------------------------------- #

def _one_probe(endpoint: str, token: str, model: str, system_prompt: str,
               question: str, timeout: int = 180):
    body = json.dumps({
        "model": model,
        "stream": True,
        "messages": [{"role": "system", "content": system_prompt},
                     {"role": "user", "content": question}],
    }).encode("utf-8")
    headers = {"Content-Type": "application/json"}
    if token:
        headers["Authorization"] = "Bearer " + token
    req = urllib.request.Request(endpoint, data=body, headers=headers)
    t0 = time.time()
    content = ""
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        for raw in resp:
            s = raw.decode("utf-8", "replace").strip()
            if not s.startswith("data:"):
                continue
            payload = s[5:].strip()
            if payload == "[DONE]":
                break
            try:
                d = json.loads(payload)
            except ValueError:
                continue
            choices = d.get("choices") or [{}]
            delta = choices[0].get("delta") or {}
            content += delta.get("content") or ""
    return time.time() - t0, content


# --------------------------------------------------------------------------- #
# live 批次
# --------------------------------------------------------------------------- #

def run_batch(args) -> int:
    gate = _load_gate()
    if args.prompt_file:
        artifact = Path(args.prompt_file)
    else:
        artifact = gate.resolve_prompt_artifact(REPO_ROOT)
    system_prompt = artifact.read_text(encoding="utf-8")
    sha = gate.prompt_sha256(artifact)

    token = args.token or ""
    if not token and args.token_file:
        token = Path(args.token_file).read_text(encoding="utf-8").strip()
    if not token:
        print("WARN: 未提供 token —— 若端点要求 auth 将失败（凭证不入档）。",
              file=sys.stderr)

    topics = args.topics.split(",") if args.topics else ["chat", "schedule", "weather"]

    host_before = _host_snapshot()
    sessions_before = _session_count()

    samples: dict[str, list] = {}
    for topic in topics:
        base_q = QUESTIONS.get(topic, topic)
        q = (args.user_prefix + " " + base_q) if args.user_prefix else base_q
        runs = []
        for i in range(args.n):
            try:
                total_s, reply = _one_probe(args.endpoint, token, args.model,
                                            system_prompt, q, timeout=args.timeout)
                entry = {"total_s": round(total_s, 3), "reply": reply}
                if topic == "weather":
                    entry["retrieval"] = _resolve_flag(
                        args.weather_retrieval, _WEATHER_HINTS.search(reply) is not None)
                if topic == "schedule":
                    entry["note_readback"] = _resolve_flag(
                        args.note_readback, _NOTE_HINTS.search(reply) is not None)
                runs.append(entry)
                print(json.dumps({"topic": topic, "i": i, "total_s": entry["total_s"],
                                  "len": len(reply),
                                  "display": reply.count("```display")},
                                 ensure_ascii=False), flush=True)
            except Exception as e:  # noqa: BLE001
                print(json.dumps({"topic": topic, "i": i, "error": str(e)[:200]},
                                 ensure_ascii=False), flush=True)
                runs.append({"total_s": None, "reply": "", "error": str(e)[:200]})
        samples[topic] = runs

    host_after = _host_snapshot()
    sessions_after = _session_count()

    batch = {
        "segment": args.segment,
        "layer": args.layer,
        "samples": samples,
        "host": {"before": host_before, "after": host_after},
        "sessions": {"before": sessions_before, "after": sessions_after},
        "prompt_sha256": sha,
        "prompt_name": artifact.name,
        "endpoint": args.endpoint,
        "model": args.model,
        "n": args.n,
        "user_prefix": args.user_prefix,
    }

    decision = None
    try:
        decision = gate.evaluate_batch(batch)
        batch["decision"] = decision
    except Exception as e:  # noqa: BLE001 — 判定失败不丢采样
        print(f"WARN: evaluate_batch 失败（{e!r}）；批次仍落盘（无 decision）。",
              file=sys.stderr)

    if args.out:
        out = Path(args.out)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(batch, ensure_ascii=False, indent=2),
                       encoding="utf-8")
        print(f"wrote batch -> {out}")

    if decision is not None:
        print(json.dumps({"segment": args.segment, "verdict": decision["verdict"],
                          "medians": decision["medians"],
                          "bottom_pass": decision["bottom_pass"],
                          "target_pass": decision["target_pass"],
                          "quality_pass": decision["quality"]["quality_pass"]},
                         ensure_ascii=False))
    return 0


def _resolve_flag(value: str, heuristic: bool) -> bool:
    if value == "yes":
        return True
    if value == "no":
        return False
    return bool(heuristic)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description="PERF-K1 live probe-batch executor.")
    ap.add_argument("--check-prompt", action="store_true",
                    help="离线：解析生产 prompt + 打印路径与归一 sha256，零网络")
    ap.add_argument("--prompt-file", default="",
                    help="显式生产 prompt 路径（ECS 侧渲染件；默认走仓内结构化解析）")
    ap.add_argument("--segment", default="unlabeled", help="归因段名（探针批次号 = 关联键）")
    ap.add_argument("--layer", default="a1", choices=["a1", "a3"])
    ap.add_argument("--n", type=int, default=3, help="每题样本数（门 = N≥3 中位数）")
    ap.add_argument("--topics", default="", help="逗号分隔覆盖默认 3 题")
    ap.add_argument("--endpoint", default=DEFAULT_ENDPOINT)
    ap.add_argument("--model", default=DEFAULT_MODEL)
    ap.add_argument("--token", default="", help="Bearer token（勿落档；优选用 --token-file）")
    ap.add_argument("--token-file", default="", help="从文件读 token（ECS 侧/仓外）")
    ap.add_argument("--user-prefix", default="",
                    help="内联指令前缀（如 /think:off，B 案最小验证）")
    ap.add_argument("--timeout", type=int, default=180)
    ap.add_argument("--weather-retrieval", default="auto", choices=["auto", "yes", "no"])
    ap.add_argument("--note-readback", default="auto", choices=["auto", "yes", "no"])
    ap.add_argument("--out", default="", help="批次 JSON 输出路径")
    args = ap.parse_args(argv)

    if args.check_prompt:
        return check_prompt(args.prompt_file)
    return run_batch(args)


if __name__ == "__main__":
    sys.exit(main())