"""KidPal 260924-m4-k1-story-cloud-link 服务端单元测试 — story.py 增量面 /
display_schema Q8 story floor 实现自带测（分支、边界与错误路径；不只 happy path）。

与冻结验收（server/acceptance/test_m4k1_story_cloud_link.acceptance.py）的分工：冻结件
是**规格**（26 齿，全绿）；本文件是实现自带测，覆盖规格未逐字段钉的边界/错误路径——
账本损坏（非缺失）重建分支、显式 limit 分页、offset 超界、delete_story 非 str/空串 id、
卡壳计时/引导轮次纯决策族的边界与「查询不改状态」（120s 语义退役后接位，261019
p3-record-dialogue-guide builder 配套义务①）、Q8 floor 的边界样本。

纯 stdlib + 按路径加载，零网络、零 ECS 依赖，任何 python3 可跑：
  cd server && python -m pytest -q tests/test_story_units.py
"""

from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from datetime import datetime, timedelta, timezone as dt_timezone
from pathlib import Path

SERVER = Path(__file__).resolve().parents[1]
if str(SERVER) not in sys.path:
    sys.path.insert(0, str(SERVER))

GATEWAY_DIR = SERVER / "gateway"


def _load_by_path(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None, f"{path} not loadable"
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


story = _load_by_path("kidpal_gateway_story_m4k1_ut", GATEWAY_DIR / "story.py")
ds = _load_by_path("kidpal_gateway_display_schema_m4k1_ut", GATEWAY_DIR / "display_schema.py")


def _seed(tmp, n, *, start_day=1):
    made = []
    for i in range(n):
        made.append(story.archive_story(
            str(tmp), f"正文-{i}", title=f"故事{i}", slug=f"s{i:02d}",
            date=f"2026-09-{start_day + i:02d}"))
    return made


class StoryListWindowUnits(unittest.TestCase):
    """list_stories 窗口/计数独立 + 分页边界。"""

    def test_window_caps_at_16_total_keeps_full_count(self):
        with tempfile.TemporaryDirectory() as tmp:
            _seed(Path(tmp), 20)
            payload = story.list_stories(str(tmp))
            self.assertEqual(len(payload["data"]["items"]), story.STORY_LIST_WINDOW)
            self.assertEqual(payload["data"]["total"], 20)

    def test_explicit_limit_overrides_window(self):
        with tempfile.TemporaryDirectory() as tmp:
            _seed(Path(tmp), 20)
            payload = story.list_stories(str(tmp), limit=5)
            self.assertEqual(len(payload["data"]["items"]), 5)
            self.assertEqual(payload["data"]["total"], 20)  # total 与分页无关

    def test_offset_beyond_range_items_empty_total_intact(self):
        with tempfile.TemporaryDirectory() as tmp:
            _seed(Path(tmp), 4)
            payload = story.list_stories(str(tmp), offset=99)
            self.assertEqual(payload["data"]["items"], [])
            self.assertEqual(payload["data"]["total"], 4)

    def test_missing_dir_is_empty_not_fabricated(self):
        with tempfile.TemporaryDirectory() as tmp:
            payload = story.list_stories(str(Path(tmp) / "nope"))
            self.assertEqual(payload["data"], {"total": 0, "items": []})

    def test_item_keys_exactly_three_no_seq(self):
        with tempfile.TemporaryDirectory() as tmp:
            _seed(Path(tmp), 2)
            items = story.list_stories(str(tmp))["data"]["items"]
            for it in items:
                self.assertEqual(set(it), {"id", "title", "date"})


class SeqLedgerUnits(unittest.TestCase):
    """入册流水号持久账本：单调、幂等、删除不回退、损坏重建。"""

    def test_monotonic_and_ledger_file_present(self):
        with tempfile.TemporaryDirectory() as tmp:
            made = _seed(Path(tmp), 3)
            self.assertEqual([r["seq"] for r in made], [1, 2, 3])
            self.assertTrue((Path(tmp) / story._LEDGER_NAME).is_file())

    def test_rearchive_same_id_keeps_seq_and_count(self):
        with tempfile.TemporaryDirectory() as tmp:
            r1 = story.archive_story(str(tmp), "第一版", title="T", slug="same",
                                     date="2026-09-20")
            r2 = story.archive_story(str(tmp), "第二版", title="T", slug="same",
                                     date="2026-09-20")
            self.assertEqual(r1["id"], r2["id"])
            self.assertEqual(r1["seq"], r2["seq"])
            self.assertEqual(story.list_stories(str(tmp))["data"]["total"], 1)

    def test_delete_does_not_rollback_seq(self):
        with tempfile.TemporaryDirectory() as tmp:
            _a, b, _c = _seed(Path(tmp), 3)
            self.assertTrue(story.delete_story(str(tmp), b["id"]))
            self.assertEqual(story.list_stories(str(tmp))["data"]["total"], 2)
            d = story.archive_story(str(tmp), "D", title="D", slug="d", date="2026-09-23")
            self.assertEqual(d["seq"], 4)

    def test_corrupt_ledger_rebuilds_without_rollback(self):
        with tempfile.TemporaryDirectory() as tmp:
            _seed(Path(tmp), 3)
            (Path(tmp) / story._LEDGER_NAME).write_text("{not json", encoding="utf-8")
            nxt = story.archive_story(str(tmp), "新篇", title="N", slug="new",
                                      date="2026-09-25")
            self.assertGreater(nxt["seq"], 3)

    def test_story_text_adds_seq_only_when_integer(self):
        self.assertEqual(set(story.story_text({"id": "x", "title": "t", "text": "b"})["data"]),
                         {"id", "title", "text"})
        self.assertEqual(story.story_text(
            {"id": "x", "title": "t", "text": "b", "seq": 7})["data"]["seq"], 7)


class DeleteStoryUnits(unittest.TestCase):
    """delete_story 机械面：合法/不存在/穿越/无哨兵/畸形 id。"""

    def test_valid_then_idempotent_false(self):
        with tempfile.TemporaryDirectory() as tmp:
            rec = story.archive_story(str(tmp), "正文", title="T", slug="x",
                                      date="2026-09-20")
            self.assertTrue(story.delete_story(str(tmp), rec["id"]))
            self.assertFalse((Path(tmp) / f"{rec['id']}.md").exists())
            self.assertFalse(story.delete_story(str(tmp), rec["id"]))  # 幂等 no-op

    def test_traversal_and_outside_file_untouched(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            stories = root / "stories"
            stories.mkdir()
            outside = root / "outside.md"
            outside.write_text("不应被删\n", encoding="utf-8")
            self.assertFalse(story.delete_story(str(stories), "../outside"))
            self.assertTrue(outside.exists())

    def test_no_sentinel_junk_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "notes.md").write_text("杂物无哨兵\n", encoding="utf-8")
            self.assertFalse(story.delete_story(str(tmp), "notes"))
            self.assertTrue((Path(tmp) / "notes.md").exists())

    def test_malformed_ids_rejected_no_raise(self):
        with tempfile.TemporaryDirectory() as tmp:
            for bad in (None, 123, "", "   ", "a/b", "a\\b", "..", "../x"):
                self.assertFalse(story.delete_story(str(tmp), bad))


class FinishPhraseUnits(unittest.TestCase):
    """收尾词检测边界。"""

    def test_positives(self):
        for t in ("我说完了", "好啦，讲完啦", "我说的故事讲完啦。", "  我说完了。 "):
            self.assertTrue(story.is_finish_phrase(t), t)

    def test_negatives(self):
        for t in ("讲完了", "我讲完", "", "   ", None, 123, "今天去公园"):
            self.assertFalse(story.is_finish_phrase(t), repr(t))


class StuckGuideDecisionUnits(unittest.TestCase):
    """卡壳计时/引导轮次纯决策（120s 语义退役后的接位决策族；261019-p3-record-dialogue-guide
    builder 配套义务①——原 test_boundary_and_exclusions 的 AUTO_STOP_SECONDS==120 +
    timed_out_sessions 边界断言随语义退役重写）：

    - 退役守卫：AUTO_STOP_SECONDS / StoryAccumulator.timed_out_sessions 不在世（复活 =
      语义回退，M4K1-A5 重钉同口径）；
    - stuck_due_sessions：>= 边界 / 等待锚（guide_since 优先、缺省 last_activity）/
      feed 重置 / 非累积态排除 / 查询不改状态；
    - auto_finish_due_sessions：轮数 2 已拍 + 第 2 轮计时到 + 收尾归档不丢弃；
    - mark_guide_round：suppressed 亦计轮（q6 收声不停摆）、last_activity 不被污染。"""

    def test_retired_120s_semantics_absent(self):
        self.assertFalse(hasattr(story, "AUTO_STOP_SECONDS"),
                         "AUTO_STOP_SECONDS 复活 = 120s 语义回退（requirements v1.65）")
        self.assertFalse(hasattr(story.StoryAccumulator, "timed_out_sessions"),
                         "timed_out_sessions 复活 = 120s 纯决策面回退")

    def test_stuck_due_boundary_and_exclusions(self):
        acc = story.StoryAccumulator(str(Path(tempfile.gettempdir()) / "rdg_ut_acc"))
        t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s-old", now=t0)
        acc.feed("s-old", "内容", now=t0)
        acc.start("s-fresh", now=t0 + timedelta(seconds=30))
        acc.feed("s-fresh", "内容", now=t0 + timedelta(seconds=30))
        acc.feed("s-chat", "闲聊", now=t0)              # 无 start -> chat
        acc.start("s-stopped", now=t0)
        acc.feed("s-stopped", "内容", now=t0)
        acc.stop("s-stopped", now=t0)                  # 已 stop -> 不再累积态
        self.assertGreater(story.STUCK_GUIDE_SECONDS, 0)  # 名字冻结、数值不冻结
        # 边界语义 = 严格大于（恰阈值不计入、过阈值触发；RDG-A2 边界②同口径）。
        self.assertNotIn("s-old", acc.stuck_due_sessions(t0 + timedelta(seconds=8),
                                                         timeout_s=8))
        self.assertIn("s-old", acc.stuck_due_sessions(t0 + timedelta(seconds=8.5),
                                                      timeout_s=8))
        at30 = acc.stuck_due_sessions(t0 + timedelta(seconds=30), timeout_s=8)
        self.assertIn("s-old", at30)
        self.assertNotIn("s-fresh", at30)
        self.assertNotIn("s-chat", at30)
        self.assertNotIn("s-stopped", at30)
        self.assertEqual(acc.routing("s-old"), "accumulate")  # 查询不改状态

    def test_feed_resets_streak_and_guide_since_anchor(self):
        acc = story.StoryAccumulator(str(Path(tempfile.gettempdir()) / "rdg_ut_acc2"))
        t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s", now=t0)
        acc.feed("s", "开头", now=t0)
        acc.mark_guide_round("s", now=t0 + timedelta(seconds=5))
        self.assertEqual(acc.guide_rounds("s"), 1)
        self.assertIn("s", acc.stuck_due_sessions(t0 + timedelta(seconds=11),
                                                  timeout_s=5))
        # 孩子接着说 = 回应：streak 清零 + 计时锚随 feed 前移。
        acc.feed("s", "然后它回家了", now=t0 + timedelta(seconds=6))
        self.assertEqual(acc.guide_rounds("s"), 0)
        self.assertNotIn("s", acc.stuck_due_sessions(t0 + timedelta(seconds=11),
                                                     timeout_s=5))
        self.assertIn("s", acc.stuck_due_sessions(t0 + timedelta(seconds=11.5),
                                                  timeout_s=5))

    def test_mark_guide_round_suppressed_still_counts_and_keeps_activity(self):
        acc = story.StoryAccumulator(str(Path(tempfile.gettempdir()) / "rdg_ut_acc3"))
        t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s", now=t0)
        acc.feed("s", "内容", now=t0 + timedelta(seconds=1))
        anchor_before = t0 + timedelta(seconds=1)
        acc.mark_guide_round("s", now=t0 + timedelta(seconds=5), suppressed=True)
        self.assertEqual(acc.guide_rounds("s"), 1)
        acc.mark_guide_round("s", now=t0 + timedelta(seconds=10), suppressed=True)
        self.assertEqual(acc.guide_rounds("s"), 2)
        # 计时锚 = 轮发出时刻（guide_since），last_activity 不被引导污染。
        self.assertIn("s", acc.auto_finish_due_sessions(t0 + timedelta(seconds=16),
                                                        timeout_s=5))
        self.assertIn("s", acc.stuck_due_sessions(t0 + timedelta(seconds=16),
                                                  timeout_s=5))
        self.assertEqual(acc.guide_rounds("s"), 2)  # 查询不改状态

    def test_auto_finish_needs_two_rounds_then_stop_archives(self):
        acc = story.StoryAccumulator(str(Path(tempfile.gettempdir()) / "rdg_ut_acc4"))
        t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s", now=t0)
        acc.feed("s", "从前有一只小猫", now=t0)
        acc.mark_guide_round("s", now=t0 + timedelta(seconds=5))
        self.assertEqual(story.GUIDE_ROUND_LIMIT, 2)
        self.assertNotIn("s", acc.auto_finish_due_sessions(t0 + timedelta(seconds=12),
                                                           timeout_s=5))
        acc.mark_guide_round("s", now=t0 + timedelta(seconds=10))
        self.assertNotIn("s", acc.auto_finish_due_sessions(t0 + timedelta(seconds=14),
                                                           timeout_s=5))
        self.assertIn("s", acc.auto_finish_due_sessions(t0 + timedelta(seconds=15),
                                                        timeout_s=5))
        # 自动收尾 = 归档不丢弃（断言方向钉死）。
        rec = acc.stop("s", now=t0 + timedelta(seconds=15))
        self.assertIsNotNone(rec)
        self.assertIn("从前有一只小猫", rec["text"])

    def test_rounds_override_and_new_session_streak_clean(self):
        acc = story.StoryAccumulator(str(Path(tempfile.gettempdir()) / "rdg_ut_acc5"))
        t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s", now=t0)
        acc.feed("s", "内容", now=t0)
        acc.mark_guide_round("s", now=t0 + timedelta(seconds=5))
        acc.mark_guide_round("s", now=t0 + timedelta(seconds=10))
        # rounds 覆盖（1 轮即判）。
        self.assertIn("s", acc.auto_finish_due_sessions(t0 + timedelta(seconds=16),
                                                        timeout_s=5, rounds=1))
        # 新会话沿：上一会话的引导 streak 不污染（stop -> start 重置）。
        acc.stop("s", now=t0 + timedelta(seconds=16))
        acc.start("s2", now=t0 + timedelta(seconds=20))
        self.assertEqual(acc.guide_rounds("s2"), 0)
        self.assertNotIn("s2", acc.auto_finish_due_sessions(t0 + timedelta(seconds=30),
                                                            timeout_s=5))

    def test_build_organizing_input_readonly_and_none(self):
        acc = story.StoryAccumulator(str(Path(tempfile.gettempdir()) / "rdg_ut_acc6"))
        t0 = datetime(2026, 10, 19, 12, 0, 0, tzinfo=dt_timezone.utc)
        self.assertIsNone(acc.build_organizing_input("nope"))
        acc.start("s", now=t0)
        self.assertIsNone(acc.build_organizing_input("s"))  # 空累积
        acc.feed("s", "第一句", now=t0)
        acc.feed("s", "Thank you.", now=t0 + timedelta(seconds=1))  # 幻听串不滤
        self.assertEqual(acc.build_organizing_input("s"), "第一句\nThank you.")
        self.assertEqual(acc.guide_rounds("s"), 0)  # 只读不改状态


class DisplayFloorUnits(unittest.TestCase):
    """Q8 story 形状 floor 边界 + 非 story view 零变化。"""

    def test_positive_samples_pass(self):
        self.assertTrue(ds.validate_display(
            {"view": "story_list", "data": {"total": 0, "items": []}})[0])
        self.assertTrue(ds.validate_display(
            {"view": "story_text", "data": {"id": "a", "title": "t", "text": "x", "seq": 1}})[0])

    def test_story_list_bad_total_and_items(self):
        items_ok = [{"id": "a", "title": "t", "date": "09-20"}]
        for bad in ("5", -1, 2.5, True):
            self.assertFalse(ds.validate_display(
                {"view": "story_list", "data": {"total": bad, "items": items_ok}})[0])
        self.assertFalse(ds.validate_display(
            {"view": "story_list", "data": {"total": 1, "items": "x"}})[0])
        self.assertFalse(ds.validate_display(
            {"view": "story_list", "data": {"total": 1, "items": ["nope"]}})[0])
        self.assertFalse(ds.validate_display(
            {"view": "story_list", "data": {"total": 1, "items": [{"id": "a"}]}})[0])

    def test_story_text_bad_seq(self):
        for bad in ("6", 6.5, True):
            self.assertFalse(ds.validate_display(
                {"view": "story_text",
                 "data": {"id": "a", "title": "t", "text": "x", "seq": bad}})[0])

    def test_other_views_unchanged(self):
        self.assertTrue(ds.validate_display({"view": "text", "data": {"text": "你好"}})[0])
        self.assertTrue(ds.validate_display({"view": "schedule", "data": {}})[0])


class StripFinishWordsUnits(unittest.TestCase):
    """归档正文剔尾随收尾词（plan §2 / SAO-A4）：只剥尾随、正文中间不误伤。"""

    def test_trailing_words_stripped(self):
        for tail in ("我说完了", "讲完啦", "说完了"):
            self.assertEqual(
                story.strip_trailing_finish_words(f"从前有一只小猫。{tail}"),
                "从前有一只小猫。", tail)

    def test_trailing_punctuation_after_word_stripped(self):
        self.assertEqual(story.strip_trailing_finish_words("小猫回家了。我说完了。"),
                         "小猫回家了。")

    def test_middle_word_and_normal_text_untouched(self):
        # 正文中间出现的收尾词不剥（不误伤叙述内容）
        self.assertEqual(story.strip_trailing_finish_words("我说完了之后又讲了一段。"),
                         "我说完了之后又讲了一段。")
        self.assertEqual(story.strip_trailing_finish_words("从前有一只小猫去公园玩。"),
                         "从前有一只小猫去公园玩。")

    def test_non_str_passthrough(self):
        self.assertIsNone(story.strip_trailing_finish_words(None))
        self.assertEqual(story.strip_trailing_finish_words(""), "")


class PendingStopUnits(unittest.TestCase):
    """pending-stop 挂起态机（261001-m4-k3-story-archive-order）：mark/routing/超时/释放/
    与 stop·cancel 互斥；stop(strip_finish) 剔词与默认零偏移。"""

    def _acc(self, tag):
        return story.StoryAccumulator(str(Path(tempfile.gettempdir()) / f"sao_ut_{tag}"))

    def test_mark_pending_routing_not_chat_and_has_accum_false(self):
        acc = self._acc("mark")
        t0 = datetime(2026, 10, 1, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s", now=t0)
        acc.mark_pending("s", now=t0)
        self.assertEqual(acc.routing("s"), "pending_stop")   # SAO-A2 负锚：!= chat
        self.assertNotEqual(acc.routing("s"), "chat")
        self.assertFalse(acc.has_accum("s"))

    def test_pending_timeout_boundary_and_exclusions(self):
        acc = self._acc("tmo")
        t0 = datetime(2026, 10, 1, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("p", now=t0)
        acc.mark_pending("p", now=t0)
        acc.start("a", now=t0)
        acc.feed("a", "内容", now=t0)                 # accumulate（非 pending）
        acc.feed("c", "闲聊", now=t0)                 # chat
        self.assertNotIn("p", acc.pending_timed_out_sessions(t0 + timedelta(seconds=9)))
        self.assertIn("p", acc.pending_timed_out_sessions(t0 + timedelta(seconds=10)))
        at20 = acc.pending_timed_out_sessions(t0 + timedelta(seconds=20))
        self.assertIn("p", at20)
        self.assertNotIn("a", at20)                   # 非 pending 会话不返回
        self.assertNotIn("c", at20)
        self.assertEqual(acc.routing("p"), "pending_stop")  # 查询不改状态

    def test_pending_timeout_s_override(self):
        acc = self._acc("tmo2")
        t0 = datetime(2026, 10, 1, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("p", now=t0)
        acc.mark_pending("p", now=t0)
        self.assertNotIn("p", acc.pending_timed_out_sessions(
            t0 + timedelta(seconds=2), timeout_s=3))
        self.assertIn("p", acc.pending_timed_out_sessions(
            t0 + timedelta(seconds=3), timeout_s=3))

    def test_mark_pending_idempotent_keeps_since(self):
        acc = self._acc("idem")
        t0 = datetime(2026, 10, 1, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s", now=t0)
        acc.mark_pending("s", now=t0)
        acc.mark_pending("s", now=t0 + timedelta(seconds=5))   # 重复不重置挂起时刻
        self.assertNotIn("s", acc.pending_timed_out_sessions(t0 + timedelta(seconds=9)))
        self.assertIn("s", acc.pending_timed_out_sessions(t0 + timedelta(seconds=10)))

    def test_release_pending_resets_to_chat_and_idempotent(self):
        acc = self._acc("rel")
        t0 = datetime(2026, 10, 1, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s", now=t0)
        acc.mark_pending("s", now=t0)
        self.assertTrue(acc.release_pending("s"))
        self.assertEqual(acc.routing("s"), "chat")     # SAO-A3②：释放回闲聊态
        self.assertFalse(acc.release_pending("s"))     # 非 pending -> False

    def test_pending_feed_accumulates_then_stop_archives_stripped(self):
        with tempfile.TemporaryDirectory() as tmp:
            acc = story.StoryAccumulator(tmp)
            acc.start("s")
            acc.mark_pending("s")
            acc.feed("s", "从前有一只小猫。我说完了")   # pending 态仍进 accum（真机序）
            self.assertTrue(acc.has_accum("s"))
            rec = acc.stop("s", strip_finish=True)
            self.assertIsInstance(rec, dict)
            self.assertEqual(rec["text"], "从前有一只小猫。")   # 正文剔尾随收尾词
            self.assertEqual(acc.routing("s"), "chat")          # stop 清 pending 挂起态
            self.assertEqual(story.list_stories(tmp)["data"]["total"], 1)

    def test_stop_default_does_not_strip(self):
        with tempfile.TemporaryDirectory() as tmp:
            acc = story.StoryAccumulator(tmp)
            acc.start("s")
            acc.feed("s", "从前有一只小猫。我说完了")
            rec = acc.stop("s")                                  # 默认 strip_finish=False
            self.assertEqual(rec["text"], "从前有一只小猫。我说完了")  # 零偏移

    def test_stop_strip_to_empty_no_archive(self):
        with tempfile.TemporaryDirectory() as tmp:
            acc = story.StoryAccumulator(tmp)
            acc.start("s")
            acc.feed("s", "我说完了")                            # 整段只有收尾词
            self.assertIsNone(acc.stop("s", strip_finish=True))  # 不写空故事
            self.assertEqual(story.list_stories(tmp)["data"]["total"], 0)
            self.assertEqual(acc.routing("s"), "chat")

    def test_cancel_pending_releases_session(self):
        acc = self._acc("cancel")
        t0 = datetime(2026, 10, 1, 12, 0, 0, tzinfo=dt_timezone.utc)
        acc.start("s", now=t0)
        acc.mark_pending("s", now=t0)
        acc.cancel("s", reason="test_discard")
        self.assertNotIn("s", acc.active_sessions())            # 挂起态不泄漏
        self.assertEqual(acc.routing("s"), "chat")
        self.assertNotIn("s", acc.pending_timed_out_sessions(t0 + timedelta(seconds=60)))


if __name__ == "__main__":
    unittest.main()