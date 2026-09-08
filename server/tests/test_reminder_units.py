"""KidPal 包 7（260829-schedule-reminder）服务端单元测试 — 提醒排程与投递实现自带测。

与冻结验收的分工：冻结件（server/acceptance/test_schedule_reminder.acceptance.py）是
**规格**，本文件是实现自带的分支/边界与错误路径测（builder 义务：单测覆盖分支、边界与
至少一个错误路径，不只 happy path）。两处刻意不重复的地方也在此说明：

- 冻结齿 test_a4_retry_to_ack_60s_x3_then_void 的 sink 收集形状自相矛盾（见本包
  landing/交付证据的 planner 回报项），其「60s×3 后静默作废 + 审计轨迹 + 重推复用同一
  传输 id」的语义在本文件 test_retry_to_ack_sequence_is_bounded_and_audited 里独立钉住
  —— 行为本身是真跑的，不因冻结齿的采集写法缺陷而失去证据。
- 冻结 fixture 里「钢琴课三档落在 now 之前」的日期簇与「已过去档位自动跳过」互斥（同
  一条回报项）；本文件按 spec 口径（决策 20「已过去的档位自动跳过」）钉下一次 occurrence
  的三档时刻与跳过行为。

纯 stdlib + 双工假腿，零网络、零 ECS 依赖，可在任何 python3 上跑：
  cd server && python -m pytest -q tests/test_reminder_units.py
"""

from __future__ import annotations

import asyncio
import importlib.util
import json
import os
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
REPO_ROOT = SERVER.parent
if str(SERVER) not in sys.path:
    sys.path.insert(0, str(SERVER))

REMINDER_LIB = SERVER / "deploy" / "openclaw-patch" / "reminder_schedule_lib.py"


def _load_by_path(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None, f"{path} not loadable"
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


lib = _load_by_path("kidpal_reminder_schedule_lib_ut", REMINDER_LIB)
gw = _load_by_path("kidpal_gateway_reminders_ut", SERVER / "gateway" / "reminders.py")
schedule6 = _load_by_path("kidpal_gateway_schedule_ut", SERVER / "gateway" / "schedule.py")

B1 = """---
child: 小明
timezone: Asia/Shanghai
remind_before_min: 15
---

# 小明的课表

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周三 | 19:00-20:00 | 英语培训 | 新东方 | |
| 周六 | 10:00-11:30 | 美术班 | 少年宫 | 带画笔 |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-08-30 | 14:00-15:00 | 牙医复诊 | 口腔医院 | |
| 2026-09-16 | 09:00 | 开学报到 | 学校 | 带暑假作业 |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 |
|------|-------|-------|-------|
| 周一 | 语文 | 数学 | 自习 |
"""

# 2026-09-08 = 周二（datetime 核对）。now 落在周二 07:00 -> 下一次周一 = 09-14。
NOW = "2026-09-08T07:00:00+08:00"


def _entry(bid="rmd-3f2a1b9c4d5e6f70", fire_at="2026-09-09T16:15:00+08:00", speech="该上课了"):
    return {"id": bid, "fire_at": fire_at, "title": "钢琴课 16:30 开始前", "speech": speech,
            "view": {"view": "text", "data": {"text": "16:30 钢琴课 @琴行 · 带琴谱"}}}


class _FakeDevice:
    def __init__(self):
        self.sent_text = []
        self.sent_bytes = []

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):  # 出现即违反裁定二红线
        self.sent_bytes.append(data)


class _FakeEngine:
    def __init__(self):
        self.sent = []

    async def send(self, message):
        self.sent.append(message)


# --------------------------------------------------------------------------- #
# 生产侧：三档算术与范围（决策 20）
# --------------------------------------------------------------------------- #

class ProducerArithmetic(unittest.TestCase):
    def setUp(self):
        self.model = schedule6.parse_schedule_md(B1)

    def _piano(self, **kw):
        return sorted(e["fire_at"] for e in lib.plan_reminders(self.model, NOW, **kw)
                      if "钢琴课" in e["title"])

    def test_three_ladders_for_next_occurrence(self):
        """前晚档 = 事件日前一天 night_at；早档 = 当天 morning_at；临期 = 开始 - before_min。"""
        self.assertEqual(self._piano(), ["2026-09-13T20:30:00+08:00",
                                          "2026-09-14T07:45:00+08:00",
                                          "2026-09-14T16:15:00+08:00"])

    def test_ladder_times_come_from_config(self):
        cfg = {"night_at": "19:45", "morning_at": "06:30", "before_min": 30}
        self.assertEqual(self._piano(config=cfg), ["2026-09-13T19:45:00+08:00",
                                                    "2026-09-14T06:30:00+08:00",
                                                    "2026-09-14T16:00:00+08:00"])

    def test_ladder_config_carrier_matches_spec_constants(self):
        shipped = lib.load_ladder_config(SERVER / "deploy" / "openclaw-reminder.config.yaml")
        self.assertEqual((shipped["night_at"], shipped["morning_at"], shipped["before_min"]),
                         ("20:30", "07:45", 15))
        missing = lib.load_ladder_config(SERVER / "no" / "such.yaml")
        self.assertEqual(missing, lib.DEFAULT_LADDER_CONFIG)  # 缺载体不停摆

    def test_cross_hour_subtraction_and_weekend_class(self):
        plan = lib.plan_reminders(self.model, NOW)
        english = sorted(e["fire_at"] for e in plan if "英语培训" in e["title"])
        self.assertEqual(english, ["2026-09-08T20:30:00+08:00",   # 周三事件的前晚档 = 今天 20:30
                                   "2026-09-09T07:45:00+08:00",
                                   "2026-09-09T18:45:00+08:00"])   # 19:00 - 15min 跨小时减法
        art = sorted(e["fire_at"] for e in plan if "美术班" in e["title"])
        self.assertEqual(art, ["2026-09-11T20:30:00+08:00",
                               "2026-09-12T07:45:00+08:00",
                               "2026-09-12T09:45:00+08:00"])       # 周末班照样每周重复

    def test_school_block_never_reminds_and_onetime_scope(self):
        # 一次性安排 09-16 在 14 天窗口内（默认 7 天窗口不含它 = 另一齿断言）
        corpus = json.dumps(lib.plan_reminders(self.model, NOW, horizon_days=14),
                            ensure_ascii=False)
        for course in ("语文", "数学", "自习"):
            self.assertNotIn(course, corpus)      # 在校例行不提醒
        self.assertNotIn("牙医", corpus)          # 过期一次性安排不生成（契约 §2.3）
        self.assertIn("开学报到", corpus)         # 未来一次性安排生成

    def test_past_ladders_are_skipped(self):
        """spec 明写「已过去的档位自动跳过」：周一晚 19:00 时，当天三档（20:30 前晚/07:45
        早档/16:15 临期）一个都不发 —— 当晚才录入「明天」的事，前晚档不发同理。"""
        late = lib.plan_reminders(self.model, "2026-09-14T19:00:00+08:00")
        self.assertEqual([e["fire_at"] for e in late if "钢琴课" in e["title"]], [])
        # 窗口拉长到下一次 occurrence（09-21 周一）时，三档又齐全（每周重复生成）
        next_week = lib.plan_reminders(self.model, "2026-09-14T19:00:00+08:00", horizon_days=8)
        self.assertEqual(sorted(e["fire_at"] for e in next_week if "钢琴课" in e["title"]),
                         ["2026-09-20T20:30:00+08:00", "2026-09-21T07:45:00+08:00",
                          "2026-09-21T16:15:00+08:00"])
        # 部分已过只留未过：07:50 时当天的 07:45 早档不发、16:15 临期档照发
        midday = [e["fire_at"] for e in lib.plan_reminders(self.model, "2026-09-14T07:50:00+08:00")
                  if "钢琴课" in e["title"] and e["fire_at"].startswith("2026-09-14")]
        self.assertEqual(midday, ["2026-09-14T16:15:00+08:00"])

    def test_remind_before_min_is_never_consumed(self):
        mutated = schedule6.parse_schedule_md(B1.replace("remind_before_min: 15",
                                                        "remind_before_min: 9999"))
        self.assertEqual({e["id"]: e["fire_at"] for e in lib.plan_reminders(self.model, NOW)},
                         {e["id"]: e["fire_at"] for e in lib.plan_reminders(mutated, NOW)})

    def test_missing_sections_do_not_crash(self):
        """真 ECS schedule.md 只有「每周固定课表」一节 -> 缺区块必须正常产出。"""
        only_weekly = schedule6.parse_schedule_md(
            "## 每周固定课表\n| 星期 | 时间 | 课程 | 地点 | 备注 |\n"
            "|---|---|---|---|---|\n| 周二 | 19:00-20:00 | 篮球 | 体育中心 | |\n")
        self.assertEqual(only_weekly["onetime"], [])
        self.assertEqual(only_weekly["school"], [])
        plan = lib.plan_reminders(only_weekly, NOW)
        self.assertTrue(plan)
        self.assertTrue(all("带" not in e["title"] for e in plan))  # 备注空 -> 不发明带物

    def test_payload_shape_is_contract_exact(self):
        for entry in lib.plan_reminders(self.model, NOW):
            self.assertEqual(set(entry), {"id", "fire_at", "title", "speech", "view"})
            self.assertTrue(entry["fire_at"].endswith("+08:00"))
            self.assertEqual(entry["view"]["view"], "text")   # §1.3 白名单，不发明提醒 view

    def test_id_is_deterministic_across_recompute_and_config_edits(self):
        a = {e["id"] for e in lib.plan_reminders(self.model, NOW)}
        b = {e["id"] for e in lib.plan_reminders(self.model, "2026-09-08T07:01:00+08:00")}
        c = {e["id"] for e in lib.plan_reminders(
            self.model, NOW, config={"night_at": "21:00", "morning_at": "07:45", "before_min": 15})}
        self.assertEqual(a, b)
        self.assertEqual(a, c)                                  # 改时刻 -> id 复用
        m1 = {e["id"]: e["fire_at"] for e in lib.plan_reminders(self.model, NOW)}
        m2 = {e["id"]: e["fire_at"] for e in lib.plan_reminders(
            self.model, NOW, config={"night_at": "21:00", "morning_at": "07:45", "before_min": 15})}
        self.assertTrue(any(m1[i] != m2[i] for i in m1))        # 但 fire_at 跟着配置变

    def test_id_dimensions_are_orthogonal(self):
        key = lib.entry_key("weekly", self.model["weekly"][0])
        ids = {lib.reminder_id(key, lib.LADDER_NIGHT, "2026-09-14"),
               lib.reminder_id(key, lib.LADDER_MORNING, "2026-09-14"),
               lib.reminder_id(key, lib.LADDER_NIGHT, "2026-09-21"),
               lib.reminder_id(lib.entry_key("weekly", self.model["weekly"][1]),
                               lib.LADDER_NIGHT, "2026-09-14")}
        self.assertEqual(len(ids), 4)
        for value in ids:
            self.assertRegex(value, r"^[0-9A-Za-z._-]{1,40}$")  # 设备 id[40] 可承载
            self.assertTrue(value.startswith("rmd-"))

    def test_entry_key_shape_matches_frozen_spec(self):
        self.assertEqual(lib.entry_key("weekly", self.model["weekly"][0]),
                         "weekly|1|16:30-17:30|钢琴课|琴行")
        self.assertEqual(lib.entry_key("onetime", self.model["onetime"][1]),
                         "onetime|2026-09-16|09:00|开学报到|学校")   # end 缺省 -> 省略
        with self.assertRaises(ValueError):
            lib.entry_key("school", {"weekday": 1})                  # 学校区块无提醒身份


class ProducerArtifacts(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="krm_unit_"))
        self.addCleanup(shutil.rmtree, self.dir, True)
        self.model = schedule6.parse_schedule_md(B1)

    def test_pending_write_is_atomic_and_stable(self):
        plan = lib.plan_reminders(self.model, NOW)
        path = self.dir / "alerts" / "pending.json"
        lib.write_pending_atomic(path, plan)
        self.assertEqual([p.name for p in self.dir.rglob("*") if p.suffix == ".tmp"], [])
        first = json.loads(path.read_text(encoding="utf-8"))
        self.assertEqual(first["version"], 1)
        self.assertEqual([e["id"] for e in first["alerts"]],
                         sorted(e["id"] for e in first["alerts"]))
        lib.write_pending_atomic(path, lib.plan_reminders(self.model, "2026-09-08T07:05:00+08:00"))
        second = json.loads(path.read_text(encoding="utf-8"))
        self.assertEqual(json.dumps(first["alerts"], sort_keys=True, ensure_ascii=False),
                         json.dumps(second["alerts"], sort_keys=True, ensure_ascii=False))
        entries, rejected = gw.read_pending(path)
        self.assertEqual(rejected, [])                             # 生产/投递两侧对齐
        self.assertEqual(len(entries), len(plan))

    def test_semantic_override_and_degrade(self):
        good = self.dir / "semantic.json"
        good.write_text(json.dumps({"k1": {"speech": "文案一", "bring": ["琴谱", ""]},
                                     "k2": "not-an-object"}, ensure_ascii=False), encoding="utf-8")
        sem, err = lib.load_semantic(good)
        self.assertIsNone(err)
        self.assertEqual(sem["k1"]["bring"], ["琴谱"])             # 去空串
        self.assertNotIn("k2", sem)                                # 坏条目单丢，不连坐
        missing, err_missing = lib.load_semantic(self.dir / "nope.json")
        self.assertIsNone(missing)
        self.assertTrue(err_missing)                               # 缺文件必须可归因
        broken = self.dir / "broken.json"
        broken.write_text("{oops", encoding="utf-8")
        bad, err_bad = lib.load_semantic(broken)
        self.assertIsNone(bad)
        self.assertTrue(err_bad)
        root_list = self.dir / "list.json"
        root_list.write_text("[1,2]", encoding="utf-8")
        odd, err_odd = lib.load_semantic(root_list)
        self.assertIsNone(odd)
        self.assertTrue(err_odd)                                   # 顶层非对象

    def test_bring_items_priority_and_no_invention(self):
        self.assertEqual(lib.bring_items("带琴谱", {"bring": ["琴谱", "压泥板"]}),
                         ["琴谱", "压泥板"])                       # 语义优先
        self.assertEqual(lib.bring_items("带琴谱", None), ["带琴谱"])  # 机械兜底 = 整条备注
        self.assertEqual(lib.bring_items("   ", None), [])         # 空备注 -> 不发明
        self.assertEqual(lib.bring_items("", {"bring": []}), [])

    def test_night_fallback_text_carries_note(self):
        """携带内容由备注列承载的可机判落点：night 档兜底文案必须带上备注原文。"""
        night = [e for e in lib.plan_reminders(self.model, NOW)
                 if "钢琴课" in e["title"] and e["fire_at"].endswith("T20:30:00+08:00")]
        self.assertTrue(night)
        self.assertIn("带琴谱", night[0]["speech"] + night[0]["title"])
        morning = [e for e in lib.plan_reminders(self.model, NOW) if "英语培训" in e["title"]]
        self.assertTrue(all("带" not in e["speech"] for e in morning if "备注" not in e["speech"]))

    def test_semantic_speech_wins_over_fallback(self):
        key = lib.entry_key("weekly", self.model["weekly"][0])
        plan = lib.plan_reminders(self.model, NOW,
                                 semantic={key: {"speech": "明天要上钢琴课啦，记得把琴谱放进书包！",
                                                  "bring": ["琴谱"]}})
        night = [e for e in plan if "钢琴课" in e["title"] and e["fire_at"].endswith("T20:30:00+08:00")]
        self.assertTrue(night)
        self.assertIn("书包", night[0]["speech"])

    def test_ladder_config_cleans_values_and_drops_unknown_keys(self):
        cfg_path = self.dir / "weird.yaml"
        cfg_path.write_text("# 只有注释\n\nnot_a_key_line\nnight_at: \"  21:05 \"\n"
                            "morning_at:  nonsense\nunknown_key: 42\n", encoding="utf-8")
        cfg = lib.load_ladder_config(cfg_path)
        self.assertEqual(cfg["night_at"], "21:05")     # 值清洗
        self.assertEqual(cfg["morning_at"], "07:45")    # 坏值 -> 该项回默认（不瞎猜）
        self.assertNotIn("unknown_key", cfg)            # 未知键不进生效配置


# --------------------------------------------------------------------------- #
# 投递侧：重试到确认 / 有界 / 节拍 / 校验
# --------------------------------------------------------------------------- #

class DispatcherStateMachine(unittest.TestCase):
    def _dispatcher(self, log):
        return gw.AlertDispatcher(sink=lambda bid, envelope: log.append((bid, envelope)))

    def test_retry_to_ack_sequence_is_bounded_and_audited(self):
        """冻结齿 test_a4_retry_to_ack 的 sink 采集形状自相矛盾（见模块 docstring）；
        本齿独立钉住它要钉的行为：60s×3 后静默作废、审计轨迹精确、重推复用同一传输 id。"""
        pushed = []
        dispatcher = self._dispatcher(pushed)
        entry = _entry()
        dispatcher.tick([entry], "2026-09-09T16:15:00+08:00")
        self.assertEqual([bid for bid, _ in pushed], [entry["id"]])
        self.assertEqual(dispatcher.attempts(entry["id"]), 1)
        dispatcher.tick([entry], "2026-09-09T16:15:30+08:00")   # 超时窗未到
        self.assertEqual(len(pushed), 1)
        dispatcher.tick([entry], "2026-09-09T16:16:00+08:00")
        dispatcher.tick([entry], "2026-09-09T16:17:00+08:00")
        self.assertEqual(len(pushed), 3)
        dispatcher.tick([entry], "2026-09-09T16:18:00+08:00")   # 预算耗尽
        self.assertEqual(len(pushed), 3, "超过 3 次上限仍在重推 = 追催")
        self.assertEqual(dispatcher.state_of(entry["id"]), "void")
        self.assertEqual([event["action"] for event in dispatcher.events],
                         ["push", "push", "push", "void"])
        self.assertEqual({env["id"] for _, env in pushed},
                         {gw.alert_jsonrpc_id(entry["id"])})     # 同一数字 id 跨重推
        self.assertEqual({env["params"]["arguments"]["id"] for _, env in pushed}, {entry["id"]})
        dispatcher.tick([entry], "2026-09-09T16:20:00+08:00")   # 作废后不复活
        self.assertEqual(len(pushed), 3)

    def test_receipt_dequeues_and_reconnect_never_repumps(self):
        pushed = []
        dispatcher = self._dispatcher(pushed)
        entry = _entry()
        dispatcher.tick([entry], entry["fire_at"])
        numeric = gw.build_alert_envelope(entry)["id"]
        self.assertEqual(gw.consume_receipts(dispatcher, [numeric]), [entry["id"]])
        self.assertEqual(dispatcher.state_of(entry["id"]), "acked")
        self.assertEqual(gw.consume_receipts(dispatcher, [entry["id"]]), [])  # 幂等
        dispatcher.tick([entry], "2026-09-09T16:16:00+08:00")
        self.assertEqual(len(pushed), 1, "已出队的档位又被推送 = 重复弹窗")
        self.assertEqual(dispatcher.on_ws_connected(), [])       # 不做重连即重推
        self.assertIsNone(dispatcher.on_receipt("rmd-never-seen"))

    def test_expired_before_first_push_is_voided_silently(self):
        pushed = []
        dispatcher = self._dispatcher(pushed)
        stale = _entry(fire_at="2026-09-09T16:15:00+08:00")
        dispatcher.tick([stale], "2026-09-09T19:15:00+08:00")
        self.assertEqual(pushed, [])                             # 断网期间未送达 -> 不补发
        self.assertEqual(dispatcher.state_of(stale["id"]), "void")
        self.assertEqual([event["action"] for event in dispatcher.events], ["void"])

    def test_illegal_entry_is_rejected_not_forwarded(self):
        pushed = []
        dispatcher = self._dispatcher(pushed)
        bad = {"id": "rmd-bad", "fire_at": "昨天", "title": "", "speech": None, "view": "x"}
        dispatcher.tick([bad], "2026-09-09T16:15:00+08:00")
        self.assertEqual(pushed, [])
        self.assertIn("reject", [event["action"] for event in dispatcher.events])
        self.assertEqual(dispatcher.state_of("rmd-bad"), "unseen")

    def test_tracking_cache_stays_bounded(self):
        dispatcher = gw.AlertDispatcher(sink=lambda bid, envelope: None)
        many = []
        for i in range(gw.MAX_TRACKED + 60):
            item = _entry(bid=f"rmd-bnd-{i:05d}")
            item["title"] = f"条目{i}"
            many.append(item)
        dispatcher.tick(many, "2026-09-09T16:15:00+08:00")
        self.assertLessEqual(dispatcher.tracked_count(), gw.MAX_TRACKED)
        dispatcher.tick(many, "2026-09-09T16:16:00+08:00")
        self.assertLessEqual(dispatcher.tracked_count(), gw.MAX_TRACKED)

    def test_missing_speech_still_delivers_popup_frame(self):
        entry = _entry(speech="")
        device, engine = _FakeDevice(), _FakeEngine()
        result = asyncio.run(gw.deliver_alert(entry, device, engine))
        self.assertTrue(result["delivered"])                     # 弹窗必达（V20）
        self.assertEqual(engine.sent, [])                        # 无文案 -> 零注入
        self.assertEqual(device.sent_bytes, [])                  # 裁定二红线
        self.assertIsNone(gw.build_speech_injection({"speech": "  "}))


class _BrokenDevice:
    """设备腿断掉的错误路径（K6 ErrorEnvelope 归因）。"""

    def __init__(self):
        self.sent_bytes = []

    async def send_text(self, text):
        raise RuntimeError("leg closed")

    async def send_bytes(self, data):
        self.sent_bytes.append(data)


class DeliveryErrorPaths(unittest.TestCase):
    def test_closed_device_leg_reports_error_envelope(self):
        device, engine = _BrokenDevice(), _FakeEngine()
        result = asyncio.run(gw.deliver_alert(_entry(), device, engine))
        self.assertFalse(result["delivered"])              # 断腿绝不静默算送达
        self.assertEqual(result["device_frames"], 0)
        self.assertEqual(engine.sent, [])                  # 设备没收到就不再注入语音
        envelope = result["error"]
        self.assertIsInstance(envelope, dict)
        for field in ("category", "fingerprint", "failingStep", "message"):
            self.assertIn(field, envelope)                 # K6 ErrorEnvelope 形状
        self.assertEqual(envelope["fingerprint"], "reminders.device-leg-send-failed")

    def test_speech_leg_failure_does_not_block_popup(self):
        class _BrokenEngine:
            def __init__(self):
                self.attempts = 0

            async def send(self, message):
                self.attempts += 1
                raise RuntimeError("engine leg down")

        device, engine = _FakeDevice(), _BrokenEngine()
        result = asyncio.run(gw.deliver_alert(_entry(), device, engine))
        self.assertTrue(result["delivered"])               # 弹窗必达（V20 语音是增强）
        self.assertFalse(result["speech"])
        self.assertEqual(len(device.sent_text), 1)
        self.assertEqual(engine.attempts, 1)


class TransportFace(unittest.TestCase):
    def test_numeric_id_deterministic_in_range_and_collision_free(self):
        self.assertEqual(gw.alert_jsonrpc_id("rmd-x"), gw.alert_jsonrpc_id("rmd-x"))
        ids = {gw.alert_jsonrpc_id(f"rmd-{i:06d}") for i in range(2000)}
        self.assertEqual(len(ids), 2000)
        self.assertTrue(all(isinstance(v, int) and 1 <= v <= 2 ** 31 - 1 for v in ids))

    def test_envelope_keeps_business_id_and_stringifies_view(self):
        entry = _entry()
        envelope = gw.build_alert_envelope(entry)
        args = envelope["params"]["arguments"]
        self.assertEqual(set(args), {"id", "fire_at", "title", "speech", "view"})
        self.assertEqual(args["id"], entry["id"])                # 字符串业务 id 不被数字顶替
        self.assertIsInstance(args["view"], str)                 # 设备 Property 三型限制
        self.assertEqual(json.loads(args["view"]), entry["view"])
        self.assertEqual(envelope["method"], "tools/call")
        self.assertEqual(envelope["params"]["name"], "kidpal/alert")

    def test_relay_and_reminders_agree_on_the_wire_shape(self):
        """mcp_relay.alert_tool_call（上游明文把数字化划归本包）与 reminders 单一事实源。"""
        entry = _entry()
        relay = _load_by_path("kidpal_mcp_relay_ut", SERVER / "gateway" / "mcp_relay.py")
        envelope = relay.alert_tool_call(entry)
        self.assertIsInstance(envelope["id"], int)
        self.assertNotIsInstance(envelope["id"], bool)
        self.assertEqual(envelope["params"]["arguments"]["id"], entry["id"])
        self.assertEqual(envelope["params"]["name"], "kidpal/alert")
        # display 路径与 relay_ack 冻结语义零改动
        display = relay.display_tool_call({"view": "text", "data": {"text": "hi"}})
        self.assertEqual(display["params"]["name"], "kidpal/display")
        acked, rid = relay.relay_ack({"jsonrpc": "2.0", "id": 7, "result": {"ack": True}})
        self.assertTrue(acked and rid == 7)

    def test_due_predicate_is_inclusive_and_refuses_dirty(self):
        entry = _entry()
        self.assertEqual([e["id"] for e in gw.due_entries([entry], entry["fire_at"])], [entry["id"]])
        self.assertEqual(gw.due_entries([entry], "2026-09-09T16:14:00+08:00"), [])
        self.assertEqual(gw.due_entries([{"id": "bad"}], entry["fire_at"]), [])
        self.assertEqual(gw.due_entries([entry], "not-a-time"), [])   # 坏 now 不猜

    def test_poll_delay_alignment(self):
        self.assertEqual(gw.POLL_INTERVAL_SEC, 60)
        self.assertEqual(gw.next_poll_delay(0.0), 60.0)
        self.assertEqual(gw.next_poll_delay(-5.0), 60.0)
        self.assertEqual(gw.next_poll_delay(60.0), 60.0)              # 整分钟点不忙轮询
        self.assertEqual(gw.next_poll_delay(90.0), 30.0)
        self.assertTrue(0 < gw.next_poll_delay(12345.6) <= 60)

    def test_validation_matrix(self):
        valid_cases = [
            _entry(),
            {k: v for k, v in _entry().items() if k != "speech"},   # speech 缺省合法
        ]
        for case in valid_cases:
            ok, reason = gw.validate_alert_entry(case)
            self.assertTrue(ok, reason)
        invalid_cases = [
            None, {}, {"id": 5}, _entry(bid=""), _entry(bid="rmd 空格"), _entry(bid="x" * 60),
            _entry(speech=7), {**_entry(), "fire_at": "2026-09-09T16:15:00"},   # 无偏移
            {**_entry(), "fire_at": "2026-09-09 16:15"},
            {**_entry(), "view": {"view": "reminder", "data": {}}},              # 白名单外
            {**_entry(), "title": "  "},
        ]
        for case in invalid_cases:
            ok, reason = gw.validate_alert_entry(case)
            self.assertFalse(ok, f"竟然通过机械校验: {case!r}")
            self.assertTrue(isinstance(reason, str) and reason.strip())          # 必给原因

    def test_read_pending_missing_file_is_quiet(self):
        self.assertEqual(gw.read_pending(Path(tempfile.gettempdir()) / "krm-absent.json"),
                         ([], []))

    def test_pending_path_is_env_configurable(self):
        old = {k: os.environ.get(k) for k in ("KIDPAL_ALERTS_DIR", "KIDPAL_PENDING_JSON")}
        try:
            os.environ["KIDPAL_ALERTS_DIR"] = "/tmp/krm-handoff"
            os.environ.pop("KIDPAL_PENDING_JSON", None)
            self.assertEqual(gw.pending_path(), os.path.join("/tmp/krm-handoff", "pending.json"))
            os.environ["KIDPAL_PENDING_JSON"] = "/elsewhere/pending.json"
            self.assertEqual(gw.pending_path(), "/elsewhere/pending.json")
            self.assertNotIn("/root/.openclaw", gw.pending_path())
        finally:
            for key, value in old.items():
                if value is None:
                    os.environ.pop(key, None)
                else:
                    os.environ[key] = value


class FrontPollOnceIntegration(unittest.TestCase):
    """front 每分钟轮询的一次真闭环（假双腿 + 真 pending.json 落盘）。

    证明四点位不是装饰：read_pending -> due -> AlertDispatcher -> deliver_alert（设备腿
    一条 mcp 帧、引擎腿一条 kidpal_speak 注入、设备腿 send_bytes 恒空）-> 渲染回执经
    consume_receipts 接回出队方（g-relay-fix 缺口的接法）。"""

    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="krm_front_"))
        self.addCleanup(shutil.rmtree, self.dir, True)
        self.front = self._load_front()
        self.model = schedule6.parse_schedule_md(B1)
        plan = lib.plan_reminders(self.model, NOW)
        self.pending = self.dir / "pending.json"
        lib.write_pending_atomic(self.pending, plan)
        os.environ["KIDPAL_PENDING_JSON"] = str(self.pending)
        self.addCleanup(os.environ.pop, "KIDPAL_PENDING_JSON", None)
        self.front._alert_dispatcher = None
        self.front._alert_legs.clear()
        self.front._alert_receipts.clear()
        self.device, self.engine = _FakeDevice(), _FakeEngine()
        self.front._register_alert_legs(self.device, self.engine)

    @staticmethod
    def _load_front():
        try:
            import fastapi  # noqa: F401
        except ImportError:
            from types import ModuleType
            from unittest.mock import Mock

            class _Permissive(ModuleType):
                def __getattr__(self, name):  # noqa: D105
                    return Mock()

            sys.modules.setdefault("fastapi", _Permissive("fastapi"))
            sys.modules["fastapi"].WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
            sys.modules["fastapi"].HTTPException = type("HTTPException", (Exception,), {})
            sys.modules.setdefault("fastapi.responses", _Permissive("fastapi.responses"))
        return _load_by_path("kidpal_front_app_ut", SERVER / "gateway" / "front" / "front_app.py")

    def test_one_poll_cycle_pushes_and_receipt_dequeues(self):
        due = [e for e in lib.plan_reminders(self.model, NOW)
               if e["fire_at"] <= "2026-09-13T20:30:00+08:00"]
        self.assertTrue(due)
        os.environ["KIDPAL_PENDING_JSON"] = str(self.pending)
        # 让本轮的 now 正好落在第一个前晚档时刻上（闭区间算到点）
        target = sorted(due, key=lambda e: e["fire_at"])[0]
        report = asyncio.run(self.front.run_alert_poll_once(
            now_epoch=gw.to_epoch_sec(target["fire_at"])))
        self.assertGreaterEqual(len(report["pushed"]), 1)
        self.assertEqual(len(self.device.sent_text), len(report["pushed"]))
        self.assertEqual(self.device.sent_bytes, [])           # 裁定二红线
        frames = [json.loads(f) for f in self.device.sent_text]
        self.assertTrue(all(f.get("type") == "mcp" for f in frames))
        self.assertTrue(all(f["payload"]["params"]["name"] == "kidpal/alert" for f in frames))
        self.assertTrue(all(isinstance(f["payload"]["id"], int) for f in frames))
        # 有 speech 的档位必须真的向引擎腿注入（空列表不得 vacuous 通过）
        injections = [json.loads(m) for m in self.engine.sent]
        self.assertEqual(len(injections), len(report["pushed"]))
        self.assertTrue(all(i.get("type") == "kidpal_speak" and i.get("text")
                            for i in injections))

        # 设备渲染回执（"ack": true + reminder_id）-> 只走渲染回执通道出队
        numeric = frames[0]["payload"]["id"]
        bid = frames[0]["payload"]["params"]["arguments"]["id"]
        accepted = {"type": "mcp", "payload": {"jsonrpc": "2.0", "id": numeric,
                                               "result": {"ack": False, "state": "accepted"}}}
        rendered = {"type": "mcp", "payload": {"jsonrpc": "2.0", "id": numeric,
                                               "result": {"ack": True, "reminder_id": bid}}}
        self.assertIsNone(gw.alert_receipt_id(json.dumps(accepted)))   # accepted 不算送达
        self.assertEqual(gw.alert_receipt_id(json.dumps(rendered)), bid)
        self.front._queue_alert_receipt(gw.alert_receipt_id(json.dumps(rendered)))
        second = asyncio.run(self.front.run_alert_poll_once(
            now_epoch=gw.to_epoch_sec(target["fire_at"]) + 5))
        self.assertEqual(second["acked"], [bid])
        self.assertEqual(second["pushed"], [], "已出队的档位又被推 = 重复弹窗")

    def test_no_online_leg_does_not_raise(self):
        self.front._alert_legs.clear()
        plan = lib.plan_reminders(self.model, NOW)
        target = sorted(plan, key=lambda e: e["fire_at"])[0]
        report = asyncio.run(self.front.run_alert_poll_once(
            now_epoch=gw.to_epoch_sec(target["fire_at"])))
        self.assertGreaterEqual(len(report["pushed"]), 1)  # 记账照走（预算耗尽后自然作废）
        self.assertEqual(self.device.sent_text, [])

    def test_broken_pending_file_is_attributable(self):
        (self.dir / "junk.json").write_text("{{{ 半截", encoding="utf-8")
        os.environ["KIDPAL_PENDING_JSON"] = str(self.dir / "junk.json")
        report = asyncio.run(self.front.run_alert_poll_once(now_epoch=gw.to_epoch_sec(NOW)))
        self.assertEqual(report["pending"], 0)
        self.assertGreaterEqual(report["rejected"], 1)         # 坏输入可归因，不抛异常


if __name__ == "__main__":
    unittest.main(verbosity=2)
