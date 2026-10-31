"""KidPal 261031-openclaw-cron-trigger-gating — 判定器 B（schedule_health_gate.py）单测。

冻结验收是规格；本文件实现自带测：四分支 + 恢复动作 + 指纹逐字 + checked_at 两种 ISO
形态 + 坏 checked_at 退 mtime + 冷却窗两侧 + ack 缺失/损坏 + 告警件不可解析 + state 仅
承载 lastFire（不含 ack 去重键）+ 数值型 env 配置污染 fail-loud（错误路径）。

纯 stdlib、零网络、零外部资产：
  python -m pytest -q server/tests/test_schedule_health_gate_units.py
"""

from __future__ import annotations

import importlib.util
import json
import os
import subprocess
import sys
import tempfile
import time
import unittest
from datetime import datetime, timedelta, timezone
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
GATE = SERVER / "deploy" / "openclaw-patch" / "schedule_health_gate.py"


def _load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None, f"{path} not loadable"
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


gate = _load("kidpal_schedule_health_gate_ut", GATE)


def _now():
    return datetime.now(timezone.utc)


def _fp(error, detail):
    return error + "|" + detail


def _write_error(td: Path, checked_at, *, error="ScheduleParseError",
                 detail="日期列格式错误: 2026-10-32 周一", mtime=None, raw=None):
    p = td / "schedule-parse-error.json"
    if raw is not None:
        p.write_text(raw, encoding="utf-8")
    else:
        p.write_text(json.dumps({"checked_at": checked_at, "error": error,
                                 "detail": detail}, ensure_ascii=False), encoding="utf-8")
    if mtime is not None:
        os.utime(str(p), (mtime, mtime))
    return p


def _write_ack(td: Path, fp, *, raw=None):
    p = td / "schedule-parse-health.json"
    if raw is not None:
        p.write_text(raw, encoding="utf-8")
    else:
        p.write_text(json.dumps({"last_notified_fingerprint": fp,
                                 "last_notified_at": _now().isoformat()},
                                ensure_ascii=False), encoding="utf-8")
    return p


def _compute(td: Path, *, stale=180, cooldown=3600, state=None, now=None):
    return gate.compute(str(td / "schedule-parse-error.json"),
                        str(td / "schedule-parse-health.json"),
                        stale, cooldown, state, now)


class ScheduleHealthGateUnits(unittest.TestCase):

    def test_absent_alert_no_ack_no_fire(self):
        with tempfile.TemporaryDirectory() as d:
            obj = _compute(Path(d))
        self.assertIs(obj["fire"], False)

    def test_absent_alert_with_ack_recovery_fires(self):
        with tempfile.TemporaryDirectory() as d:
            _write_ack(Path(d), "OldError|old detail")
            obj = _compute(Path(d))
        self.assertIs(obj["fire"], True)
        self.assertTrue(obj["message"].strip())

    def test_stale_alert_two_sided(self):
        stale = (_now() - timedelta(seconds=3600)).isoformat()
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), stale)
            _write_ack(Path(d), "OldError|old detail")
            self.assertIs(_compute(Path(d))["fire"], True)
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), stale)
            self.assertIs(_compute(Path(d))["fire"], False)

    def test_fresh_same_fingerprint_suppressed(self):
        err, det = "ScheduleParseError", "坏 token 2026-10-32"
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), _now().isoformat(), error=err, detail=det)
            _write_ack(Path(d), _fp(err, det))
            self.assertIs(_compute(Path(d))["fire"], False)

    def test_fresh_new_fingerprint_fires_self_contained(self):
        err, det = "ScheduleParseError", "日期列格式错误: 2026-10-32 周一"
        fp = _fp(err, det)
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), _now().isoformat(), error=err, detail=det)
            _write_ack(Path(d), "OtherError|different detail")
            obj = _compute(Path(d))
        self.assertIs(obj["fire"], True)
        self.assertEqual(obj["state"]["lastFire"]["fingerprint"], fp)
        self.assertGreater(obj["state"]["lastFire"]["atMs"], 0)

    def test_checked_at_two_iso_forms(self):
        err, det = "ScheduleParseError", "坏 token 2026-10-32"
        fp = _fp(err, det)
        now = _now()
        for ct in (now.isoformat(timespec="seconds"),
                   now.strftime("%Y-%m-%dT%H:%M:%SZ")):
            with self.subTest(ct=ct), tempfile.TemporaryDirectory() as d:
                _write_error(Path(d), ct, error=err, detail=det)
                _write_ack(Path(d), fp)
                self.assertIs(_compute(Path(d))["fire"], False)

    def test_bad_checked_at_falls_back_to_mtime(self):
        err, det = "ScheduleParseError", "坏 token 2026-10-32"
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), "not-a-timestamp", error=err, detail=det,
                         mtime=int(time.time()))
            _write_ack(Path(d), _fp(err, det))
            self.assertIs(_compute(Path(d))["fire"], False)

    def test_unparseable_alert_fires(self):
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), None, raw="not json {")
            obj = _compute(Path(d))
        self.assertIs(obj["fire"], True)
        self.assertTrue(any(t in obj["message"] for t in ("解析", "无法", "边界")))

    def test_ack_missing_or_corrupt_fires(self):
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), _now().isoformat())
            self.assertIs(_compute(Path(d))["fire"], True)
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), _now().isoformat())
            _write_ack(Path(d), "x|y", raw="not json {")
            self.assertIs(_compute(Path(d))["fire"], True)

    def test_cooldown_window_both_sides(self):
        err, det = "ScheduleParseError", "坏 token 2026-10-32"
        fp = _fp(err, det)
        now_ms = int(time.time() * 1000)
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), _now().isoformat(), error=err, detail=det)
            _write_ack(Path(d), "OtherError|different detail")
            recent = {"lastFire": {"fingerprint": fp, "atMs": now_ms - 60_000}}
            self.assertIs(_compute(Path(d), cooldown=3600, state=recent)["fire"], False)
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), _now().isoformat(), error=err, detail=det)
            _write_ack(Path(d), "OtherError|different detail")
            old = {"lastFire": {"fingerprint": fp, "atMs": now_ms - 2 * 3600 * 1000}}
            self.assertIs(_compute(Path(d), cooldown=3600, state=old)["fire"], True)

    def test_state_carries_only_antispam_fields(self):
        err, det = "ScheduleParseError", "日期列格式错误: 2026-10-32 周一"
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), _now().isoformat(), error=err, detail=det)
            obj = _compute(Path(d))
        state = obj["state"]
        self.assertNotIn("last_notified_fingerprint", state)
        self.assertNotIn("last_notified_at", state)
        self.assertEqual(set(state), {"lastFire"})
        self.assertEqual(set(state["lastFire"]), {"fingerprint", "atMs"})

    def test_corrupt_state_snapshot_is_ignored(self):
        # state 只作反刷屏：损坏快照不得崩，也不得抑制真实告警
        err, det = "ScheduleParseError", "坏 token 2026-10-32"
        with tempfile.TemporaryDirectory() as d:
            _write_error(Path(d), _now().isoformat(), error=err, detail=det)
            self.assertIs(_compute(Path(d), state={"lastFire": "bad"})["fire"], True)

    def test_env_int_rejects_non_numeric(self):
        old = os.environ.get("KIDPAL_UNIT_STALE")
        os.environ["KIDPAL_UNIT_STALE"] = "not-a-number"
        try:
            with self.assertRaises(ValueError):
                gate._env_int("KIDPAL_UNIT_STALE", 180)
        finally:
            if old is None:
                os.environ.pop("KIDPAL_UNIT_STALE", None)
            else:
                os.environ["KIDPAL_UNIT_STALE"] = old

    def test_main_internal_error_exit_nonzero_no_half_json(self):
        env = dict(os.environ)
        env.update({
            "KIDPAL_PARSE_ERROR_JSON": str(Path("nope") / "err.json"),
            "KIDPAL_PARSE_HEALTH_JSON": str(Path("nope") / "health.json"),
            "KIDPAL_HEALTH_STALE_SEC": "not-a-number",
            "PYTHONUTF8": "1",
            "PYTHONIOENCODING": "utf-8",
        })
        proc = subprocess.run([sys.executable, str(GATE)], env=env,
                              capture_output=True, text=True, timeout=90)
        self.assertNotEqual(proc.returncode, 0)
        out = (proc.stdout or "").strip()
        parsed_ok = False
        try:
            parsed_ok = type(json.loads(out).get("fire")) is bool
        except (ValueError, AttributeError):
            parsed_ok = False
        self.assertFalse(parsed_ok, f"内部异常仍输出完整判决 JSON：{out!r}")


if __name__ == "__main__":
    unittest.main(verbosity=2)
