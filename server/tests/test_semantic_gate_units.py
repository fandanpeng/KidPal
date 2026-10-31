"""KidPal 261031-openclaw-cron-trigger-gating — 判定器 A（reminder_semantic_gate.py）单测。

冻结验收 server/acceptance/test_261031_openclaw_cron_trigger_gating.acceptance.py 是规格；
本文件实现自带测：真值表分支、60s 竞态余量两侧边界、四输入各自落后、全新鲜、schedule
缺失终局守卫、message 可辨原因、数值型 env 配置污染 fail-loud（错误路径）。

纯 stdlib、零网络、零外部资产，可在任何 python3 上跑：
  python -m pytest -q server/tests/test_semantic_gate_units.py
"""

from __future__ import annotations

import importlib.util
import json
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
GATE = SERVER / "deploy" / "openclaw-patch" / "reminder_semantic_gate.py"

BASE = 1_700_000_000


def _load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None, f"{path} not loadable"
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


gate = _load("kidpal_reminder_semantic_gate_ut", GATE)


def _mk(td: Path, name: str, text: str, epoch: float) -> Path:
    p = td / name
    p.write_text(text, encoding="utf-8")
    os.utime(str(p), (epoch, epoch))
    return p


def _fixture(td: Path, *, schedule_ok=True, semantic="{}",
             semantic_mtime=BASE, schedule_mtime=BASE - 3600,
             skill_mtime=BASE - 1800, parser_mtime=BASE - 1800,
             lib_mtime=BASE - 1800):
    paths = {}
    if schedule_ok:
        paths["schedule"] = _mk(td, "schedule.md", "# 课表\n", schedule_mtime)
    else:
        paths["schedule"] = td / "schedule.md"
    if semantic is None:
        paths["semantic"] = td / "semantic.json"
    else:
        paths["semantic"] = _mk(td, "semantic.json", semantic, semantic_mtime)
    paths["skill"] = _mk(td, "SKILL.md", "skill", skill_mtime)
    paths["parser"] = _mk(td, "schedule.py", "parser", parser_mtime)
    paths["lib"] = _mk(td, "reminder_schedule_lib.py", "lib", lib_mtime)
    return paths


def _compute(paths, margin=60):
    return gate.compute(
        str(paths["schedule"]), str(paths["semantic"]), str(paths["skill"]),
        str(paths["parser"]), str(paths["lib"]), margin)


class SemanticGateUnits(unittest.TestCase):

    def test_missing_semantic_fires_with_reason(self):
        with tempfile.TemporaryDirectory() as d:
            obj = _compute(_fixture(Path(d), semantic=None))
        self.assertIs(obj["fire"], True)
        self.assertIn("semantic", obj["message"].lower())

    def test_invalid_json_fires(self):
        with tempfile.TemporaryDirectory() as d:
            obj = _compute(_fixture(Path(d), semantic='{"broken": '))
        self.assertIs(obj["fire"], True)
        self.assertIn("json", obj["message"].lower())

    def test_schedule_missing_is_terminal_guard(self):
        # 即便下游 skill 变新，无课表也不 fire（skill 约定不写、保留旧产物）
        with tempfile.TemporaryDirectory() as d:
            obj = _compute(_fixture(
                Path(d), schedule_ok=False, skill_mtime=BASE + 30))
        self.assertIs(obj["fire"], False)
        self.assertNotIn("message", obj)

    def test_race_margin_boundary_inclusive(self):
        with tempfile.TemporaryDirectory() as d:
            obj = _compute(_fixture(
                Path(d), schedule_mtime=BASE - 60, skill_mtime=BASE - 1000,
                parser_mtime=BASE - 1000, lib_mtime=BASE - 1000))
        self.assertIs(obj["fire"], True)
        self.assertIn("schedule", obj["message"].lower())
        with tempfile.TemporaryDirectory() as d:
            obj = _compute(_fixture(
                Path(d), schedule_mtime=BASE - 61, skill_mtime=BASE - 1000,
                parser_mtime=BASE - 1000, lib_mtime=BASE - 1000))
        self.assertIs(obj["fire"], False)

    def test_schedule_newer_than_semantic_fires(self):
        # 差值为负同样 <= margin：课表在 semantic 之后被改
        with tempfile.TemporaryDirectory() as d:
            obj = _compute(_fixture(
                Path(d), schedule_mtime=BASE + 1, skill_mtime=BASE - 1000,
                parser_mtime=BASE - 1000, lib_mtime=BASE - 1000))
        self.assertIs(obj["fire"], True)

    def test_each_downstream_input_triggers(self):
        cases = [
            ("skill", {"skill_mtime": BASE}),          # 等值 = 落后
            ("parser", {"parser_mtime": BASE + 30}),
            ("lib", {"lib_mtime": BASE + 30}),
        ]
        tokens = {"skill": ("skill", "技能"), "parser": ("schedule.py", "parser", "解析"),
                  "lib": ("lib", "reminder", "排程", "库")}
        for label, kwargs in cases:
            with self.subTest(label=label), tempfile.TemporaryDirectory() as d:
                obj = _compute(_fixture(Path(d), **kwargs))
                self.assertIs(obj["fire"], True)
                self.assertTrue(
                    any(t.lower() in obj["message"].lower() for t in tokens[label]),
                    f"{label}: message 未含可辨 token：{obj['message']!r}")

    def test_all_fresh_no_fire(self):
        with tempfile.TemporaryDirectory() as d:
            obj = _compute(_fixture(Path(d)))
        self.assertIs(obj["fire"], False)
        self.assertNotIn("message", obj)

    def test_message_differs_by_reason(self):
        with tempfile.TemporaryDirectory() as d:
            race = _compute(_fixture(
                Path(d), schedule_mtime=BASE - 60, skill_mtime=BASE - 1000,
                parser_mtime=BASE - 1000, lib_mtime=BASE - 1000))
        with tempfile.TemporaryDirectory() as d:
            skill = _compute(_fixture(Path(d), skill_mtime=BASE + 30))
        self.assertNotEqual(race["message"], skill["message"])

    def test_env_int_rejects_non_numeric(self):
        old = os.environ.get("KIDPAL_UNIT_MARGIN")
        os.environ["KIDPAL_UNIT_MARGIN"] = "not-a-number"
        try:
            with self.assertRaises(ValueError):
                gate._env_int("KIDPAL_UNIT_MARGIN", 60)
        finally:
            if old is None:
                os.environ.pop("KIDPAL_UNIT_MARGIN", None)
            else:
                os.environ["KIDPAL_UNIT_MARGIN"] = old

    def test_main_internal_error_exit_nonzero_no_half_json(self):
        env = dict(os.environ)
        env.update({
            "KIDPAL_SCHEDULE_MD": str(Path("nope") / "schedule.md"),
            "KIDPAL_SEMANTIC_JSON": str(Path("nope") / "semantic.json"),
            "KIDPAL_REMINDER_SEMANTIC_SKILL": str(Path("nope") / "SKILL.md"),
            "KIDPAL_SCHEDULE_PARSER": str(Path("nope") / "schedule.py"),
            "KIDPAL_REMINDER_LIB": str(Path("nope") / "lib.py"),
            "KIDPAL_SEMANTIC_GATE_MARGIN_SEC": "not-a-number",
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
