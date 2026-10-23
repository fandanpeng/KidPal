#!/usr/bin/env python3
"""PERF-K1 probe gate — pure decider / helper module.

冻结契约（FROZEN OFFLINE CONTRACT §4）：**无网络、无 fastapi，离线可 import**。
本模块可在导入期零网络 I/O；所有函数为浏览/判断纯逻辑。

对外 API（冻结）:
    resolve_prompt_artifact(repo_root) -> Path
        从结构化指向（provider.config.yaml 的 system_prompt.source|artifact +
        apply-xiaozhi-overlay.sh 的 PROMPT_SRC/DST）解析当前被接线的生产 system
        prompt 文件 —— 版本无关，禁硬写 vN 字面（ARQ-260908 纪律）。
    prompt_sha256(path) -> str
        LF 归一后的 sha256（与生产渲染件 hash 口径一致）。
    build_probe_messages(prompt_text, question) -> list[dict]
        探针必带生产 prompt（评审 D1）。
    median(samples) -> float
    extract_display_blocks(text) -> list[str]
    evaluate_batch(batch) -> dict
        两层门（保底/目标）+ 质量判定，纯逻辑。
"""

from __future__ import annotations

import hashlib
import importlib.util
import json
import re
import statistics
from pathlib import Path

# --------------------------------------------------------------------------- #
# 层阈值常量（LANE 冻结；评审 Q5 校准）
# --------------------------------------------------------------------------- #

BOTTOM_A1 = {"chat": 6.0, "schedule": 9.0, "weather": 14.0}
TARGET_A1 = {"chat": 4.0, "schedule": 6.0, "weather": 10.0}
BOTTOM_A3 = {"chat": 8.0, "schedule": 10.0}
TARGET_A3 = {"chat": 5.0, "schedule": 7.0}

_PROMPT_FILE_RE = re.compile(r"openclaw-system-prompt\.v\d+\.md")
_DISPLAY_RE = re.compile(r"```display[ \t]*\r?\n(.*?)\r?\n?[ \t]*```", re.DOTALL)

_VALIDATE_DISPLAY = None


# --------------------------------------------------------------------------- #
# prompt 接线解析（版本无关）
# --------------------------------------------------------------------------- #

def _read_text(path) -> str:
    try:
        return Path(path).read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


class _PosixDisplayPath(type(Path())):
    """`__str__` 走 posix 形态的 Path（跨平台显示口径）。

    `probe_batch.py --check-prompt` 打印生产 prompt 路径，冻结验收断言其输出含
    forward-slash 形态的仓内相对路径（`server/deploy/.../AGENTS.md`）：Linux 天然如此，
    Windows 检出（本仓 Pod 在 Windows 跑 offline 验收）需归一，否则探针输出与路径断言
    假红。文件系统语义不变（stat / read_text / relative_to 仍走原生 Path）。
    """

    def __str__(self) -> str:
        return super().__str__().replace("\\", "/")


def resolve_prompt_artifact(repo_root) -> Path:
    """解析当前生产 system prompt 承载（261022-m5-k1 承载反转换代）。

    新拓扑（arch §5.5.5 / ADR-001 重定义）：静态语义单一事实源 =
    `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`，OpenClaw 作为 stable
    project context 组装进 system prompt 缓存边界之上；探针被测面 = 直读同一
    AGENTS.md（K1-E3 静态面集合相等）。v11 终点版仍由 provider `system_prompt` 三键
    与 overlay `PROMPT_SRC/PROMPT_DST` 声明（历史存档面，供版本无关守卫解析），但
    不再是被注入引擎的 prompt —— 引擎注入面只余有界动态 context 块。
    """
    repo_root = Path(repo_root)
    carrier = (
        repo_root / "server" / "deploy" / "openclaw-agents"
        / "workspace-kidpal" / "AGENTS.md"
    )
    if not carrier.is_file():
        raise FileNotFoundError(
            "静态语义主承载不在场：server/deploy/openclaw-agents/workspace-kidpal/"
            "AGENTS.md（261022-m5-k1 承载反转后的生产 prompt 源）")
    return _PosixDisplayPath(str(carrier.resolve()))


def prompt_sha256(path) -> str:
    """LF 归一后的 sha256（read_text().replace('\\r\\n','\\n') → utf-8）。"""
    txt = Path(path).read_text(encoding="utf-8")
    return hashlib.sha256(txt.replace("\r\n", "\n").encode("utf-8")).hexdigest()


def build_probe_messages(prompt_text: str, question: str) -> list[dict]:
    """探针 messages = [system(生产 prompt 逐字), user(题)]（评审 D1）。"""
    return [
        {"role": "system", "content": prompt_text},
        {"role": "user", "content": question},
    ]


# --------------------------------------------------------------------------- #
# 纯 helper
# --------------------------------------------------------------------------- #

def median(samples) -> float:
    """N 样本中位数。"""
    return float(statistics.median(list(samples)))


def extract_display_blocks(text: str) -> list[str]:
    """取 ```display fenced JSON 块原文（块内文本）。"""
    if not text:
        return []
    return [m.group(1) for m in _DISPLAY_RE.finditer(text)]


def _display_schema_candidates():
    """display_schema.py 的候选位置：仓内布局 + 脚本同级（ECS 部署布局）。"""
    here = Path(__file__).resolve()
    cands = [here.parent / "gateway" / "display_schema.py",
             here.parent / "display_schema.py"]
    for parent in here.parents:
        cands.append(parent / "gateway" / "display_schema.py")
    return cands


def _load_display_validator():
    """惰性加载真门 server/gateway/display_schema.py::validate_display（纯 stdlib）。"""
    global _VALIDATE_DISPLAY
    if _VALIDATE_DISPLAY is not None:
        return _VALIDATE_DISPLAY
    path = next((c for c in _display_schema_candidates() if c.is_file()), None)
    if path is None:
        raise FileNotFoundError(
            "display_schema.py 不在候选位置（仓内 server/gateway/ 或脚本同级）")
    spec = importlib.util.spec_from_file_location(
        "kidpal_display_schema_perf_k1", path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    _VALIDATE_DISPLAY = mod.validate_display
    return _VALIDATE_DISPLAY


def _payload_valid(block_text: str) -> bool:
    try:
        payload = json.loads(block_text)
    except ValueError:
        return False
    validate = _load_display_validator()
    ok, _reason = validate(payload)
    return bool(ok)


# --------------------------------------------------------------------------- #
# 判定器（两层门 + 质量）
# --------------------------------------------------------------------------- #

def evaluate_batch(batch: dict) -> dict:
    """两层门（保底/目标）+ 质量判定，纯逻辑。

    batch = {"segment": str, "layer": "a1"|"a3",
             "samples": {topic -> [ {total_s, reply, retrieval?, note_readback?}, ...]},
             "host": {...}, "sessions": {"before": int, "after": int}}
    """
    segment = batch.get("segment", "")
    layer = batch.get("layer", "a1")
    samples = batch.get("samples") or {}

    if layer == "a3":
        bottom_thr, target_thr = BOTTOM_A3, TARGET_A3
    else:
        bottom_thr, target_thr = BOTTOM_A1, TARGET_A1

    topics = [t for t, v in samples.items() if isinstance(v, list)]
    counts = {t: len(samples[t]) for t in topics}
    n_min = min(counts.values()) if counts else 0
    n_ok = bool(counts) and all(c >= 3 for c in counts.values())

    medians: dict[str, float] = {}
    for t in topics:
        vals = [s.get("total_s") for s in samples[t]
                if isinstance(s, dict) and isinstance(s.get("total_s"), (int, float))]
        medians[t] = float(statistics.median(vals)) if vals else float("inf")

    # 天气题附加条件：全部 weather 样本 retrieval 真才计 bottom 真。
    weather_samples = samples.get("weather") or []
    weather_retrieval_ok = (
        all(bool(s.get("retrieval")) for s in weather_samples)
        if weather_samples else True
    )

    bottom: dict[str, bool] = {}
    target: dict[str, bool] = {}
    for t in topics:
        if t not in bottom_thr:
            continue
        m = medians[t]
        ok_bottom = m <= bottom_thr[t]
        if t == "weather":
            ok_bottom = ok_bottom and weather_retrieval_ok
        bottom[t] = ok_bottom
        if t in target_thr:
            target[t] = m <= target_thr[t]

    bottom_pass = bool(bottom) and all(bottom.values())
    target_pass = bool(target) and all(target.values())

    if not n_ok or not bottom_pass:
        verdict = "FAIL"
    elif target_pass:
        verdict = "PASS"
    else:
        verdict = "BOTTOM_ONLY"

    # 质量守卫（全量判定）：display 块出现率 + schema 合法率 均须 1.0
    total = sum(counts.values())
    with_block = 0
    valid_resp = 0
    for t in topics:
        for s in samples[t]:
            if not isinstance(s, dict):
                continue
            blocks = extract_display_blocks(s.get("reply", "") or "")
            if blocks:
                with_block += 1
                if all(_payload_valid(b) for b in blocks):
                    valid_resp += 1
    display_rate = (with_block / total) if total else 0.0
    schema_rate = (valid_resp / total) if total else 0.0

    sched_samples = samples.get("schedule") or []
    note_readback_ok = (
        all(bool(s.get("note_readback")) for s in sched_samples)
        if sched_samples else True
    )

    quality_pass = (
        display_rate == 1.0
        and schema_rate == 1.0
        and weather_retrieval_ok
        and note_readback_ok
    )

    return {
        "segment": segment,
        "layer": layer,
        "n_min": n_min,
        "n_ok": n_ok,
        "medians": medians,
        "bottom": bottom,
        "bottom_pass": bottom_pass,
        "target": target,
        "target_pass": target_pass,
        "quality": {
            "n": total,
            "display_rate": display_rate,
            "schema_rate": schema_rate,
            "quality_pass": quality_pass,
            "weather_retrieval_ok": weather_retrieval_ok,
            "note_readback_ok": note_readback_ok,
        },
        "verdict": verdict,
    }