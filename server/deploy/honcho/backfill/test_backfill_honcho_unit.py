#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""backfill 工具族单元测试（TaskPackage 261022-m5-k4）。

覆盖冻结验收之外的分支与边界：语料行解析（角色透传 / 兜底收编 / 空行跳过）、
解析器 v2（261024-m5-k4b-corpus-parser-v2：续行归组 / 内嵌 turn 拆分 / 包装
标记剥离 / created_at 回填 / toolResult 路由 / 段级指纹公式 / additive 报告
键 / rollback --source-kind 选择性删除）、指纹确定性与锚敏感性（同内容不同
行不互吞）、transcript_events 缺表缺列 = 红、用法错 / 数据错 exit 码分档、
单源运行、部分幂等补灌、回滚恰删标记 session。
本文件属部署树内开发工件，不入 deploy manifest（不被铺放 ECS）；
验收全量命令只扫 acceptance/ 目录，本件不参与其收集。
"""

from __future__ import annotations

import importlib.util
import io
import json
import os
import sqlite3
import tempfile
import unittest
import unittest.mock as mock
import urllib.error
from pathlib import Path

DIR = Path(__file__).resolve().parent
BACKFILL_PY = DIR / "backfill_honcho.py"
ROLLBACK_PY = DIR / "rollback_honcho_backfill.py"
DEVICE_ID = "aa:bb:cc:dd:ee:ff"
FP_KEY = "kidpalBackfillFingerprint"
BATCH_KEY = "kidpalBackfillBatch"
ENV_EXAMPLE = DIR.parent / "honcho.env.example"


def load_module(path: Path, name: str):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class _StubClient:
    """最小鸭型协议记录桩（独立实现，与验收桩互不依赖）。"""

    def __init__(self):
        self.sessions = {}
        self.calls = []

    def snapshot_counts(self):
        return {
            "peers": 1,
            "sessions": len(self.sessions),
            "messages": sum(len(s["messages"]) for s in self.sessions.values()),
            "observations": 0,
        }

    def list_backfill_batches(self):
        return {s["metadata"][BATCH_KEY] for s in self.sessions.values()
                if BATCH_KEY in s["metadata"]}

    def list_fingerprints(self):
        return {m[2][FP_KEY] for s in self.sessions.values()
                for m in s["messages"] if FP_KEY in m[2]}

    def create_session(self, peer, session_key, metadata):
        self.calls.append(("create_session", peer, session_key))
        self.sessions.setdefault(
            session_key, {"peer": peer, "metadata": dict(metadata), "messages": []})

    def add_message(self, session_key, role, content, metadata,
                    created_at=None):
        # created_at 可选参（解析器 v2 鸭型协议扩展）；消息元组第 4 位承载。
        self.calls.append(("add_message", session_key, role, content))
        self.sessions.setdefault(
            session_key, {"peer": "", "metadata": {}, "messages": []})
        self.sessions[session_key]["messages"].append(
            (role, content, dict(metadata), created_at))

    def delete_sessions(self, session_keys):
        keys = list(session_keys)
        self.calls.append(("delete_sessions", tuple(keys)))
        for key in keys:
            self.sessions.pop(key, None)
        return len(keys)

    def write_calls(self):
        return [c for c in self.calls
                if c[0] in ("create_session", "add_message", "delete_sessions")]


class FingerprintCase(unittest.TestCase):

    def test_fingerprint_format_and_determinism(self):
        mod = load_module(BACKFILL_PY, "unit_backfill_fp")
        fp1 = mod.fingerprint("session-corpus", "a.txt", "3", "[user] 你好")
        fp2 = mod.fingerprint("session-corpus", "a.txt", "3", "[user] 你好")
        self.assertEqual(fp1, fp2)  # 同源重跑确定不变
        self.assertRegex(fp1, r"^v1:session-corpus:[0-9a-f]{64}$")

    def test_fingerprint_anchor_sensitive_same_content_different_line(self):
        mod = load_module(BACKFILL_PY, "unit_backfill_fp2")
        a = mod.fingerprint("session-corpus", "a.txt", "1", "[user] 同句")
        b = mod.fingerprint("session-corpus", "a.txt", "7", "[user] 同句")
        c = mod.fingerprint("session-corpus", "b.txt", "1", "[user] 同句")
        d = mod.fingerprint("openclaw-transcript", "x.db", "1", "user", "同句")
        self.assertNotEqual(a, b)  # 行序锚——重复内容消息不互吞
        self.assertNotEqual(a, c)  # 源件锚
        self.assertNotEqual(a, d)  # source-kind 命名空间隔离（跨源不去重）


class CorpusLineCase(unittest.TestCase):

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_line")

    def test_role_prefix_passthrough(self):
        role, content, ok = self.mod.split_corpus_line("[user] 我喜欢紫色")
        self.assertEqual((role, content, ok), ("user", "我喜欢紫色", True))
        role, content, ok = self.mod.split_corpus_line("[front] 旧流程回合")
        self.assertEqual((role, content, ok), ("front", "旧流程回合", True))

    def test_unrecognized_line_falls_back_whole(self):
        role, content, ok = self.mod.split_corpus_line("无前缀残行")
        self.assertEqual((role, content, ok), ("user", "无前缀残行", False))

    def test_malformed_bracket_is_unrecognized(self):
        role, content, ok = self.mod.split_corpus_line("[broken 没有闭括号")
        self.assertEqual((role, content, ok), ("user", "[broken 没有闭括号", False))
        role, content, ok = self.mod.split_corpus_line("[] 空角色")
        self.assertEqual(ok, False)

    # -- 双层机器格式（2026-10-24 解析归因修复；live 实证双层前缀）-------- #
    def test_double_layer_role_attribution(self):
        # 归因正确性 = 本次 bug 回归锚：首层方括号是会话源引用（剥掉），
        # 角色取第二层 User:/Assistant: 且 assistant 恰为归一键（路由 agent）。
        role, content, ok = self.mod.split_corpus_line(
            "[kidpal/sessions/0b1e#L5] User: 我喜欢紫色")
        self.assertEqual((role, content, ok), ("user", "我喜欢紫色", True))
        role, content, ok = self.mod.split_corpus_line(
            "[kidpal/sessions/0b1e#L7] Assistant: 紫色真好看")
        self.assertEqual((role, content, ok), ("assistant", "紫色真好看", True))

    def test_double_layer_assistant_content_with_colon_preserved(self):
        role, content, ok = self.mod.split_corpus_line(
            "[kidpal/sessions/0b1e#L9] Assistant: 我建议: 用红色更好")
        self.assertEqual(ok, True)
        self.assertEqual(role, "assistant")  # 只剥第二层 token，content 内冒号不误判
        self.assertEqual(content, "我建议: 用红色更好")

    def test_double_layer_role_case_insensitive_normalized(self):
        role, content, ok = self.mod.split_corpus_line(
            "[src#L1] ASSISTANT: 变体大小写")
        self.assertEqual((role, content, ok), ("assistant", "变体大小写", True))

    def test_double_layer_with_old_device_flow_turn_passthrough(self):
        # 双层行与单层旧设备流程回合（[front]）共存时各自解析正确。
        role, content, ok = self.mod.split_corpus_line(
            "[sessions/a#L3] Assistant: 机器回合")
        self.assertEqual((role, content, ok), ("assistant", "机器回合", True))
        role, content, ok = self.mod.split_corpus_line(
            "[front] 孩子到目前为止讲的内容")
        self.assertEqual((role, content, ok), ("front", "孩子到目前为止讲的内容", True))

    def test_double_layer_multi_source_reference_layers_peeled(self):
        # 理论上多层源引用层按循环剥除（非特殊支持，按源引用层处理）。
        role, content, ok = self.mod.split_corpus_line(
            "[outer#L1] [inner#L2] User: 内容")
        self.assertEqual((role, content, ok), ("user", "内容", True))

    def test_single_layer_compat_verbatim(self):
        # 单层旧格式逐字兼容（现有行为不得漂移）：
        # content 自带 `词:` 不被误判成角色前缀。
        role, content, ok = self.mod.split_corpus_line("[user] 内容: 带冒号")
        self.assertEqual((role, content, ok), ("user", "内容: 带冒号", True))
        role, content, ok = self.mod.split_corpus_line("[user] Hello: world")
        self.assertEqual((role, content, ok), ("user", "Hello: world", True))
        # content 以方括号起（非源引用）仍归单层旧格式。
        role, content, ok = self.mod.split_corpus_line("[user] [note] 内容")
        self.assertEqual((role, content, ok), ("user", "[note] 内容", True))

    def test_second_layer_non_role_token_not_peeled(self):
        # 第二层非 User:/Assistant: token（如 Child:）保守不剥：首层方括号
        # 仍按单层旧格式透传为 role，content 完整保留（不误删源引用后残段）。
        role, content, ok = self.mod.split_corpus_line("[src#L1] Child: 内容")
        self.assertEqual((role, content, ok), ("src#L1", "Child: 内容", True))


class SourceParseCase(unittest.TestCase):

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_src")
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def test_corpus_blank_lines_skipped_unparsed_counted(self):
        corpus = self.root / "corpus"
        corpus.mkdir()
        (corpus / "a.txt").write_text(
            "[user] 一\n\n[assistant] 二\n残行\n", encoding="utf-8")
        (corpus / "sub").mkdir()
        (corpus / "sub" / "b.txt").write_text("[user] 三\n", encoding="utf-8")
        messages, unparsed, stats = self.mod.parse_corpus_dir(corpus)
        self.assertEqual(len(messages), 4)
        self.assertEqual(unparsed, 1)
        self.assertTrue(messages[2]["unparsed"])
        self.assertEqual(messages[0]["role"], "user")
        self.assertEqual(messages[1]["role"], "assistant")
        # v2 消息形状：created_at（非日期文件名 → None）+ segment_index。
        self.assertIsNone(messages[0]["created_at"])
        self.assertEqual(messages[0]["segment_index"], 0)

    def test_corpus_empty_dir_zero_planned(self):
        corpus = self.root / "empty"
        corpus.mkdir()
        messages, unparsed, stats = self.mod.parse_corpus_dir(corpus)
        self.assertEqual((messages, unparsed), ([], 0))
        self.assertEqual(stats["groups"], 0)
        self.assertEqual(stats["wrapperMarkersStripped"], 0)
        self.assertEqual(stats["corpusFiles"], [])

    def test_corpus_double_layer_attribution_end_to_end(self):
        # 双层前缀语料（live 实证形态）：逐行角色归因正确，assistant 行
        # role == "assistant"（→ add_message 路由 agent peer），零 unparsed。
        corpus = self.root / "corpus2"
        corpus.mkdir()
        (corpus / "s.txt").write_text(
            "[kidpal/sessions/u#L5] User: 我喜欢紫色\n"
            "[kidpal/sessions/u#L7] Assistant: 紫色真好看\n"
            "[kidpal/sessions/u#L9] Assistant: 我建议: 用红色\n"
            "无前缀残行\n", encoding="utf-8")
        messages, unparsed, stats = self.mod.parse_corpus_dir(corpus)
        self.assertEqual(len(messages), 4)
        self.assertEqual(unparsed, 1)  # 仅无前缀残行兜底，双层行全识别
        self.assertEqual(
            [m["role"] for m in messages[:3]], ["user", "assistant", "assistant"])
        self.assertEqual(messages[2]["content"], "我建议: 用红色")
        # 源引用（首层方括号）不进 content。
        self.assertNotIn("[kidpal/sessions/u#L5]", messages[0]["content"])
        # 异 ref 各自成组（#L5/#L7/#L9 非续行）：groups = 3 双层组 + 1 残行组。
        self.assertEqual(stats["groups"], 4)

    def test_transcript_superset_columns_ok_missing_table_red(self):
        db = self.root / "ok.db"
        con = sqlite3.connect(str(db))
        con.execute("CREATE TABLE transcript_events "
                    "(id INTEGER PRIMARY KEY, role TEXT, content TEXT, extra TEXT)")
        con.execute("INSERT INTO transcript_events VALUES (1, 'user', '内容', 'x')")
        con.commit()
        con.close()
        messages = self.mod.parse_transcript_db(db)
        self.assertEqual(len(messages), 1)
        self.assertEqual(messages[0]["content"], "内容")

        bad = self.root / "bad.db"
        con = sqlite3.connect(str(bad))
        con.execute("CREATE TABLE other (id INTEGER)")
        con.commit()
        con.close()
        with self.assertRaises(self.mod.BackfillError):
            self.mod.parse_transcript_db(bad)  # 缺表 = 红

        missing_col = self.root / "nc.db"
        con = sqlite3.connect(str(missing_col))
        con.execute("CREATE TABLE transcript_events (id INTEGER PRIMARY KEY)")
        con.commit()
        con.close()
        with self.assertRaises(self.mod.BackfillError):
            self.mod.parse_transcript_db(missing_col)  # 缺列 = 红

    def test_transcript_missing_file_red(self):
        with self.assertRaises(self.mod.BackfillError):
            self.mod.parse_transcript_db(self.root / "ghost.db")


class ParserV2GroupingCase(unittest.TestCase):
    """解析器 v2 续行归组（review §2.1 ② / F2 修复；K4B-E1）。"""

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_v2_group")
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def _parse(self, files: dict) -> tuple:
        corpus = self.root / "corpus"
        corpus.mkdir(exist_ok=True)
        for name, text in files.items():
            (corpus / name).write_text(text, encoding="utf-8")
        return self.mod.parse_corpus_dir(corpus)

    def test_consecutive_same_ref_same_role_merge_newlines_kept(self):
        # 真语料形态（review §1.1：1652 非空行全部带第二层 role 前缀）：
        # 续行同样重复 User:/Assistant: 前缀，同 (ref, role) 连续行归并。
        messages, unparsed, stats = self._parse({
            "a.txt": ("[kidpal/sessions/u#L5] User: 我喜欢紫色\n"
                      "[kidpal/sessions/u#L5] User: 和蓝色\n"
                      "[kidpal/sessions/u#L5] User: 还有绿色\n"),
        })
        self.assertEqual((len(messages), unparsed, stats["groups"]),
                         (1, 0, 1))
        self.assertEqual(messages[0]["content"], "我喜欢紫色\n和蓝色\n还有绿色")
        self.assertEqual((messages[0]["role"], messages[0]["segment_index"]),
                         ("user", 0))

    def test_cross_file_not_grouped(self):
        # 同 ref + 同 role 跨文件不归组（组键含「同文件」）。
        messages, _u, stats = self._parse({
            "a.txt": "[ref#L1] User: 一\n",
            "b.txt": "[ref#L1] User: 二\n",
        })
        self.assertEqual(len(messages), 2)
        self.assertEqual(stats["groups"], 2)
        self.assertEqual([m["content"] for m in messages], ["一", "二"])

    def test_role_change_breaks_group_and_third_user_restarts(self):
        # 跨 role 不归组；role 交替后同 role 再现 = 新组（不回连首组）。
        messages, _u, stats = self._parse({
            "a.txt": ("[ref#L1] User: 一\n"
                      "[ref#L1] Assistant: 二\n"
                      "[ref#L1] User: 三\n"),
        })
        self.assertEqual(stats["groups"], 3)
        self.assertEqual([m["role"] for m in messages],
                         ["user", "assistant", "user"])

    def test_ref_change_breaks_group(self):
        messages, _u, stats = self._parse({
            "a.txt": "[a#L1] User: 一\n[b#L1] User: 二\n",
        })
        self.assertEqual((stats["groups"], len(messages)), (2, 2))

    def test_prefixless_continuation_stays_single_layer_conservative(self):
        # 已知保守歧义（review §1.5 留痕）：双层形态 ref 但缺第二层 role
        # 前缀的行 → 单层旧格式透传（role = ref 文本），不静默并组。
        messages, _u, stats = self._parse({
            "a.txt": ("[a#L1] User: 一\n"
                      "[a#L1] 无 role 前缀续行\n"),
        })
        self.assertEqual((stats["groups"], len(messages)), (2, 2))
        self.assertEqual(messages[0]["role"], "user")
        self.assertEqual(messages[1]["role"], "a#L1")
        self.assertEqual(messages[1]["content"], "无 role 前缀续行")

    def test_blank_line_does_not_break_group(self):
        # 组键连续性只看非空行序列（空行跳过、不隔断组）——真语料 1007 组
        # 锚定数在该口径下成立（快照审计对账）。
        messages, _u, stats = self._parse({
            "a.txt": "[a#L1] User: 一\n\n[a#L1] User: 二\n",
        })
        self.assertEqual((stats["groups"], len(messages)), (1, 1))
        self.assertEqual(messages[0]["content"], "一\n二")

    def test_single_layer_lines_not_grouped_frozen_per_line(self):
        # 单层旧格式不参与归组：冻结 K4「逐非空行一条」行为逐字保持。
        messages, _u, stats = self._parse({
            "a.txt": "[user] 一\n[user] 二\n",
        })
        self.assertEqual((stats["groups"], len(messages)), (2, 2))
        self.assertEqual([m["content"] for m in messages], ["一", "二"])


class ParserV2SplitCase(unittest.TestCase):
    """解析器 v2 内嵌 turn 拆分 + 包装标记剥离（review §2.1 ③ / F1 修复）。"""

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_v2_split")
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def _parse(self, text: str) -> tuple:
        corpus = self.root / "corpus"
        corpus.mkdir(exist_ok=True)
        (corpus / "a.txt").write_text(text, encoding="utf-8")
        return self.mod.parse_corpus_dir(corpus)

    def test_first_segment_keeps_outer_role_embedded_marker_assigns(self):
        messages, unparsed, _s = self._parse(
            "[a#L1] User: 好的\n"
            "[a#L1] User: 那你知道 Assistant: 是紫色呀\n")
        self.assertEqual(unparsed, 0)
        self.assertEqual(len(messages), 2)
        self.assertEqual(messages[0]["role"], "user")   # 首段沿用外层 role
        self.assertEqual(messages[0]["content"], "好的\n那你知道")
        self.assertEqual(messages[1]["role"], "assistant")  # 内嵌标记定 role
        self.assertEqual(messages[1]["content"], "是紫色呀")

    def test_wrapper_markers_counted_retained_in_content(self):
        # 保留变体（review §2.1 ③「不剥、只计数」；对账基准 = review 探针
        # split_probe2.py——剥离会使 243 个包装标载体段变空、冻结锚定数
        # 2879 不可达）：标记保留在段文本中、wrapperMarkersStripped 计数。
        messages, _u, stats = self._parse(
            "[a#L1] User: [Chat messages since your last reply - for context] "
            "我喜欢紫色\n"
            "[b#L1] User: [Current message - respond to this] 好的\n"
            "[c#L1] User: 那 Assistant: 紫色真好看\n")
        self.assertEqual(stats["wrapperMarkersStripped"], 2)  # 如实计数
        self.assertEqual(
            [(m["role"], m["content"]) for m in messages],
            [("user",
              "[Chat messages since your last reply - for context] 我喜欢紫色"),
             ("user", "[Current message - respond to this] 好的"),
             ("user", "那"),
             ("assistant", "紫色真好看")])

    def test_wrapper_carrier_lead_group_counts_as_segment(self):
        # 冻结锚定数关键形态（真语料 243 例）：组首段恰为包装标记载体
        # （body = "[wrapper] User: 实文"）——载体段计入消息数（user），
        # 实文按内嵌标记成段。1007 组 → 2879 条的差额半即此形态。
        messages, _u, stats = self._parse(
            "[a#L1] User: [Current message - respond to this] User: "
            "今天西安的天气怎么样？\n")
        self.assertEqual(stats["wrapperMarkersStripped"], 1)
        self.assertEqual(
            [(m["role"], m["content"]) for m in messages],
            [("user", "[Current message - respond to this]"),
             ("user", "今天西安的天气怎么样？")])

    def test_word_colon_not_misplit(self):
        # content 自带 `词:` / 非 User|Assistant token 不误拆（切段正则只认
        # 两机器 token，且须前置空白）。
        messages, _u, _s = self._parse(
            "[a#L1] User: 他说: 我喜欢紫色\n"
            "[a#L1] User: Child: 我也是\n")
        self.assertEqual(len(messages), 1)
        self.assertEqual(messages[0]["content"], "他说: 我喜欢紫色\nChild: 我也是")

    def test_assistant_group_marker_split_honestly_no_loss(self):
        # assistant 组实读零标记；若出现标记按同规则处理并如实分段（不丢内容）。
        messages, _u, _s = self._parse(
            "[a#L1] Assistant: 好的\n"
            "[a#L1] Assistant: 补充 User: 哦\n")
        self.assertEqual([(m["role"], m["content"]) for m in messages],
                         [("assistant", "好的\n补充"), ("user", "哦")])

    def test_single_layer_line_never_split(self):
        # 单层旧格式逐字保持：content 内嵌 User:/Assistant: 不拆（冻结 K4）。
        messages, _u, _s = self._parse("[user] 引用 User: 原话\n")
        self.assertEqual(len(messages), 1)
        self.assertEqual(messages[0]["content"], "引用 User: 原话")


class CreatedAtBackfillCase(unittest.TestCase):
    """created_at 回填（human D3；K4B-E4）。"""

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_created_at")

    def test_date_filename_noon_shanghai_is_04_utc(self):
        # 口径钉死：该日 12:00 Asia/Shanghai = 04:00 UTC，ISO 8601 带 +00:00。
        self.assertEqual(self.mod.corpus_created_at("2026-09-30"),
                         "2026-09-30T04:00:00+00:00")
        self.assertEqual(self.mod.corpus_created_at("2026-10-24"),
                         "2026-10-24T04:00:00+00:00")

    def test_invalid_filename_none(self):
        for stem in ("notes", "2026-13-99", "2026-09", "", "20260930"):
            self.assertIsNone(self.mod.corpus_created_at(stem),
                              f"stem={stem!r} 须 None（非日期/非法日期）")

    def test_end_to_end_date_file_backfills_all_messages(self):
        with tempfile.TemporaryDirectory() as td:
            corpus = Path(td) / "corpus"
            corpus.mkdir()
            (corpus / "2026-09-30.txt").write_text(
                "[a#L1] User: 一\n[b#L1] User: 二\n", encoding="utf-8")
            (corpus / "a.txt").write_text("[a#L1] User: 三\n", encoding="utf-8")
            messages, _u, _s = self.mod.parse_corpus_dir(corpus)
            self.assertEqual(
                [m["created_at"] for m in messages],
                ["2026-09-30T04:00:00+00:00",
                 "2026-09-30T04:00:00+00:00",
                 None])  # 非日期文件名 → None 走服务端缺省


class FingerprintV2Case(unittest.TestCase):
    """corpus 段级指纹公式 v2（review §2.1 ⑤；K4B-E4）。"""

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_fp_v2")
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def _parse(self, text: str) -> list:
        corpus = self.root / "corpus"
        corpus.mkdir(exist_ok=True)
        (corpus / "a.txt").write_text(text, encoding="utf-8")
        messages, _u, _s = self.mod.parse_corpus_dir(corpus)
        return messages

    def test_formula_shape_and_bit_stable_across_runs(self):
        text = "[a#L1] User: 一\n[a#L1] Assistant: 二\n"
        first = [m["fingerprint"] for m in self._parse(text)]
        second = [m["fingerprint"] for m in self._parse(text)]
        self.assertEqual(first, second)  # 同源重跑逐位稳定
        for fp in first:
            self.assertRegex(fp, r"^v1:session-corpus:[0-9a-f]{64}$")

    def test_segment_position_participates_no_cross_swallow(self):
        # 同内容不同锚行 / 不同段序不互吞（指纹含首行行号 + 段序 + role）。
        msgs = self._parse("[a#L1] User: 同句\n[b#L1] User: 同句\n"
                           "[a#L2] User: 同句\n")
        self.assertEqual(len(msgs), 3)  # 不同 ref/锚行 → 三组三段
        fps = [m["fingerprint"] for m in msgs]
        self.assertEqual(len(set(fps)), 3)
        # 同 ref 续行归并；归组内拆段后段序参与 → 同文本段指纹不同。
        msgs2 = self._parse("[a#L1] User: 同句\n[a#L1] User: 同句\n")
        self.assertEqual(len(msgs2), 1)  # 同组归并
        split_same = self._parse("[a#L1] User: 前半\n"
                                 "[a#L1] User: 同句 User: 同句\n")
        fps3 = [m["fingerprint"] for m in split_same]
        self.assertEqual(len(set(fps3)), len(fps3))  # 段级指纹零碰撞

    def test_transcript_formula_unchanged(self):
        fp = self.mod.fingerprint("openclaw-transcript", "x.db", "1",
                                  "user", "同句")
        self.assertRegex(fp, r"^v1:openclaw-transcript:[0-9a-f]{64}$")


class DuckCreatedAtCase(unittest.TestCase):
    """鸭型协议 created_at 兼容探测（review §2.1 ④ 向后兼容半）。"""

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_duck")

    def test_probe_old_stub_false_new_stub_true_kwargs_true(self):
        def old_stub(session_key, role, content, metadata):
            pass

        def new_stub(session_key, role, content, metadata, created_at=None):
            pass

        def kwargs_stub(*args, **kwargs):
            pass

        self.assertFalse(self.mod.client_supports_created_at(old_stub))
        self.assertTrue(self.mod.client_supports_created_at(new_stub))
        self.assertTrue(self.mod.client_supports_created_at(kwargs_stub))

    def test_runner_omits_created_at_for_legacy_stub(self):
        # 未同步旧四参桩：runner 省略 created_at、行为与旧版逐位一致（不抛
        # TypeError——冻结 K4 验收桩在 acceptance-author 同步前保持绿）。
        mod = load_module(BACKFILL_PY, "unit_backfill_legacy_stub")

        class LegacyStub:
            def __init__(self):
                self.calls = []

            def snapshot_counts(self):
                return {"peers": 1, "sessions": 0, "messages": 0,
                        "observations": 0}

            def list_backfill_batches(self):
                return set()

            def list_fingerprints(self):
                return set()

            def create_session(self, peer, session_key, metadata):
                self.calls.append(("create_session", peer, session_key))

            def add_message(self, session_key, role, content, metadata):
                self.calls.append(("add_message", session_key, role, content))

            def delete_sessions(self, session_keys):
                return 0

        stub = LegacyStub()
        mod.build_client = lambda api: stub
        with tempfile.TemporaryDirectory() as td:
            corpus = Path(td) / "corpus"
            corpus.mkdir()
            (corpus / "2026-09-30.txt").write_text("[a#L1] User: 一\n",
                                                   encoding="utf-8")
            rc = mod.main(["--corpus-dir", str(corpus),
                           "--device-id", DEVICE_ID, "--apply"])
        self.assertEqual(rc, 0)
        self.assertEqual(len(stub.calls), 2)  # create + add，未因 5 参崩

    def test_runner_passes_created_at_to_supporting_stub(self):
        mod = load_module(BACKFILL_PY, "unit_backfill_ca_pass")
        stub = _StubClient()
        mod.build_client = lambda api: stub
        with tempfile.TemporaryDirectory() as td:
            corpus = Path(td) / "corpus"
            corpus.mkdir()
            (corpus / "2026-09-30.txt").write_text(
                "[a#L1] User: 一\n", encoding="utf-8")
            rc = mod.main(["--corpus-dir", str(corpus),
                           "--device-id", DEVICE_ID, "--apply"])
        self.assertEqual(rc, 0)
        adds = [c for c in stub.calls if c[0] == "add_message"]
        self.assertEqual(len(adds), 1)
        stored = stub.sessions[adds[0][1]]["messages"][0]
        self.assertEqual(stored[3], "2026-09-30T04:00:00+00:00")  # created_at 出网


class ReportV2KeysCase(unittest.TestCase):
    """additive 报告扩展（K4B-E3：perSource 新键 + perRoleTotal + 顶层零破坏）。"""

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_report_v2")
        self.stub = _StubClient()
        self.mod.build_client = lambda api: self.stub
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self._seed_sources()

    def _seed_sources(self):
        corpus = self.root / "corpus"
        corpus.mkdir(exist_ok=True)
        (corpus / "2026-09-30.txt").write_text(
            "[kidpal/sessions/u#L5] User: 我喜欢紫色\n"
            "[kidpal/sessions/u#L5] User: 和蓝色\n"
            "[kidpal/sessions/u#L7] Assistant: 紫色真好看\n",
            encoding="utf-8")
        db = self.root / "t.db"
        con = sqlite3.connect(str(db))
        con.execute("CREATE TABLE transcript_events "
                    "(id INTEGER PRIMARY KEY, role TEXT, content TEXT)")
        con.execute("INSERT INTO transcript_events VALUES (1, 'user', '一')")
        con.execute("INSERT INTO transcript_events VALUES "
                    "(2, 'toolResult', '课表正文')")
        con.commit()
        con.close()
        return corpus, db

    def _run(self, extra: list) -> dict:
        report = self.root / ("r-%s.json" % ("-".join(extra) or "plan"))
        rc = self.mod.main(["--corpus-dir", str(self.root / "corpus"),
                            "--openclaw-db", str(self.root / "t.db"),
                            "--device-id", DEVICE_ID,
                            "--report-out", str(report)] + extra)
        self.assertEqual(rc, 0)
        return json.loads(report.read_text(encoding="utf-8"))

    def _assert_shape(self, rep: dict) -> None:
        # 顶层键形状零破坏（冻结 K4-E3 schema + additive）。
        self.assertEqual(
            sorted(rep.keys()),
            ["backfill", "messages", "mode", "observations", "peers",
             "sessions"])
        bf = rep["backfill"]
        for key in ("ingested", "skipped", "unparsed", "perSource",
                    "perRoleTotal", "batch", "targetPeer",
                    "deviceSessionFamily", "sessionKeys"):
            self.assertIn(key, bf)
        corpus_stat = bf["perSource"]["session-corpus"]
        for key in ("planned", "ingested", "skipped", "unparsed", "perRole",
                    "groups", "segments", "wrapperMarkersStripped",
                    "corpusFiles"):
            self.assertIn(key, corpus_stat)
        self.assertEqual(corpus_stat["perRole"], {"user": 1, "assistant": 1,
                                                  "other": 0})
        self.assertEqual(corpus_stat["groups"], 2)     # u#L5 user 组 + u#L7 组
        self.assertEqual(corpus_stat["segments"], 2)
        self.assertEqual(corpus_stat["wrapperMarkersStripped"], 0)
        files = corpus_stat["corpusFiles"]
        self.assertEqual(len(files), 1)
        self.assertEqual(files[0]["name"], "2026-09-30.txt")
        self.assertEqual(files[0]["lines"], 3)
        self.assertRegex(files[0]["sha256"], r"^[0-9a-f]{64}$")
        transcript_stat = bf["perSource"]["openclaw-transcript"]
        self.assertEqual(transcript_stat["perRole"], {"user": 1, "assistant": 0,
                                                      "other": 1})
        self.assertEqual(transcript_stat["corpusFiles"], [])
        self.assertEqual(bf["perRoleTotal"], {"user": 2, "assistant": 1,
                                              "other": 1})

    def test_plan_and_apply_both_emit_extended_keys(self):
        self._assert_shape(self._run([]))          # plan
        self._assert_shape(self._run(["--apply"]))  # apply


class RollbackSourceKindCase(unittest.TestCase):
    """rollback --source-kind 选择性删除（K4B-E2 / review §2.1 ⑧）。"""

    def setUp(self):
        self.bf = load_module(BACKFILL_PY, "unit_rbsk_backfill")
        self.rb = load_module(ROLLBACK_PY, "unit_rbsk_rollback")
        self.stub = _StubClient()
        self.bf.build_client = lambda api: self.stub
        self.rb.build_client = lambda api: self.stub
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        corpus = self.root / "corpus"
        corpus.mkdir()
        (corpus / "a.txt").write_text("[user] 一\n", encoding="utf-8")
        db = self.root / "t.db"
        con = sqlite3.connect(str(db))
        con.execute("CREATE TABLE transcript_events "
                    "(id INTEGER PRIMARY KEY, role TEXT, content TEXT)")
        con.execute("INSERT INTO transcript_events VALUES (1, 'user', '一')")
        con.commit()
        con.close()
        self.assertEqual(self.bf.main(
            ["--corpus-dir", str(corpus), "--openclaw-db", str(db),
             "--device-id", DEVICE_ID, "--apply"]), 0)
        keys = sorted(self.stub.sessions)
        self.corpus_key = [k for k in keys if k.endswith(":session-corpus")][0]
        self.transcript_key = [k for k in keys
                               if k.endswith(":openclaw-transcript")][0]
        # live 在用 session（无标记）——零触碰锚。
        self.live_key = "agent:kidpal:openai-user:" + DEVICE_ID
        self.stub.sessions[self.live_key] = {
            "peer": "owner", "metadata": {"openclawSessionKey": "live"},
            "messages": [("user", "live 消息", {}, None)],
        }

    def test_plan_selective_lists_matched_and_skipped(self):
        report = self.root / "plan.json"
        rc = self.rb.main(["--source-kind", "session-corpus",
                           "--report-out", str(report)])
        self.assertEqual(rc, 0)
        rep = json.loads(report.read_text(encoding="utf-8"))
        self.assertEqual(rep["backfill"]["deleted"], 1)  # plan 预测 = matched 数
        self.assertEqual(rep["backfill"]["matchedSessions"], [self.corpus_key])
        self.assertEqual(rep["backfill"]["skippedSessions"],
                         [self.transcript_key])
        self.assertEqual(rep["backfill"]["sourceKind"], "session-corpus")
        self.assertNotIn("delete_sessions", [c[0] for c in self.stub.calls])

    def test_apply_selective_deletes_matched_only(self):
        report = self.root / "apply.json"
        rc = self.rb.main(["--source-kind", "openclaw-transcript",
                           "--report-out", str(report), "--apply"])
        self.assertEqual(rc, 0)
        self.assertEqual(set(self.stub.sessions),
                         {self.corpus_key, self.live_key})  # transcript 恰删
        rep = json.loads(report.read_text(encoding="utf-8"))
        self.assertEqual(rep["backfill"]["deleted"], 1)
        self.assertEqual(rep["backfill"]["matchedSessions"],
                         [self.transcript_key])
        self.assertEqual(rep["backfill"]["skippedSessions"],
                         [self.corpus_key])

    def test_default_all_kinds_old_behavior(self):
        # 缺省 = 全部（旧行为逐字保持）：matched = 全部标记键、skipped 空。
        report = self.root / "all.json"
        rc = self.rb.main(["--report-out", str(report), "--apply"])
        self.assertEqual(rc, 0)
        self.assertEqual(set(self.stub.sessions), {self.live_key})
        rep = json.loads(report.read_text(encoding="utf-8"))
        self.assertEqual(rep["backfill"]["deleted"], 2)
        self.assertIsNone(rep["backfill"]["sourceKind"])
        self.assertEqual(rep["backfill"]["skippedSessions"], [])

    def test_unknown_source_kind_usage_error(self):
        with self.assertRaises(SystemExit):
            self.rb.build_arg_parser().parse_args(
                ["--source-kind", "not-a-kind"])


class MainExitCodeCase(unittest.TestCase):

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_main")
        self.stub = _StubClient()
        self.mod.build_client = lambda api: self.stub
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def test_usage_errors_exit2_zero_writes(self):
        corpus = self.root / "corpus"
        corpus.mkdir()
        for argv in ([], ["--device-id", DEVICE_ID],
                     ["--corpus-dir", str(corpus)]):
            with self.subTest(argv=argv):
                self.assertEqual(self.mod.main(list(argv)), 2)
        self.assertEqual(self.stub.write_calls(), [])

    def test_data_errors_exit1_before_writes(self):
        rc = self.mod.main(["--corpus-dir", str(self.root / "ghost"),
                            "--device-id", DEVICE_ID, "--apply"])
        self.assertEqual(rc, 1)
        self.assertEqual(self.stub.write_calls(), [])
        ghost_db = self.root / "ghost.db"
        rc = self.mod.main(["--openclaw-db", str(ghost_db),
                            "--device-id", DEVICE_ID, "--apply"])
        self.assertEqual(rc, 1)
        self.assertEqual(self.stub.write_calls(), [])
        bad_db = self.root / "bad.db"
        con = sqlite3.connect(str(bad_db))
        con.execute("CREATE TABLE not_transcript (id INTEGER)")
        con.commit()
        con.close()
        rc = self.mod.main(["--openclaw-db", str(bad_db),
                            "--device-id", DEVICE_ID, "--apply"])
        self.assertEqual(rc, 1)
        self.assertEqual(self.stub.write_calls(), [])

    def test_single_source_run_and_session_identity(self):
        corpus = self.root / "corpus"
        corpus.mkdir()
        (corpus / "a.txt").write_text("[user] 一\n", encoding="utf-8")
        report = self.root / "r.json"
        rc = self.mod.main(["--corpus-dir", str(corpus),
                            "--device-id", DEVICE_ID,
                            "--report-out", str(report), "--apply"])
        self.assertEqual(rc, 0)
        rep = json.loads(report.read_text(encoding="utf-8"))
        self.assertIn("session-corpus", rep["backfill"]["perSource"])
        self.assertNotIn("openclaw-transcript", rep["backfill"]["perSource"])
        creates = [c for c in self.stub.calls if c[0] == "create_session"]
        self.assertEqual(len(creates), 1)
        self.assertTrue(creates[0][2].startswith(
            "agent:kidpal:openai-user:" + DEVICE_ID))
        self.assertEqual(creates[0][1], "owner")

    def test_partial_idempotency_reingests_exactly_missing(self):
        corpus = self.root / "corpus"
        corpus.mkdir()
        (corpus / "a.txt").write_text("[user] 一\n[user] 二\n[user] 三\n",
                                      encoding="utf-8")
        base = ["--corpus-dir", str(corpus), "--device-id", DEVICE_ID]
        self.assertEqual(self.mod.main(base + ["--apply"]), 0)
        session = next(iter(self.stub.sessions.values()))
        removed = session["messages"].pop(1)  # 模拟中间一条丢失
        self.stub.calls.clear()
        self.assertEqual(self.mod.main(base + ["--apply"]), 0)
        adds = [c for c in self.stub.calls if c[0] == "add_message"]
        self.assertEqual(len(adds), 1)  # 恰补缺失一条
        self.assertEqual(adds[0][3], removed[1])
        self.assertEqual(adds[0][2] and
                         self.stub.sessions[adds[0][1]]["messages"][-1][2][FP_KEY],
                         removed[2][FP_KEY])  # 指纹逐位一致
        self.assertEqual(sum(len(s["messages"])
                             for s in self.stub.sessions.values()), 3)


class RollbackCase(unittest.TestCase):

    def setUp(self):
        self.bf = load_module(BACKFILL_PY, "unit_rb_backfill")
        self.rb = load_module(ROLLBACK_PY, "unit_rb_rollback")
        self.stub = _StubClient()
        self.bf.build_client = lambda api: self.stub
        self.rb.build_client = lambda api: self.stub
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        corpus = self.root / "corpus"
        corpus.mkdir()
        (corpus / "a.txt").write_text("[user] 一\n[user] 二\n", encoding="utf-8")
        self.bf.main(["--corpus-dir", str(corpus), "--device-id", DEVICE_ID,
                      "--apply"])
        self.stub.sessions["agent:kidpal:openai-user:" + DEVICE_ID] = {
            "peer": "owner", "metadata": {"openclawSessionKey": "live"},
            "messages": [("user", "live 消息", {})],
        }

    def test_plan_predicts_zero_delete(self):
        report = self.root / "plan.json"
        rc = self.rb.main(["--report-out", str(report)])
        self.assertEqual(rc, 0)
        self.assertNotIn("delete_sessions",
                         [c[0] for c in self.stub.calls])
        rep = json.loads(report.read_text(encoding="utf-8"))
        self.assertEqual(rep["mode"], "plan")
        self.assertEqual(rep["backfill"]["deleted"], 1)
        self.assertEqual(rep["sessions"]["before"], rep["sessions"]["after"])

    def test_apply_deletes_marked_only(self):
        report = self.root / "apply.json"
        rc = self.rb.main(["--report-out", str(report), "--apply"])
        self.assertEqual(rc, 0)
        live_key = "agent:kidpal:openai-user:" + DEVICE_ID
        self.assertEqual(set(self.stub.sessions), {live_key})
        self.assertEqual(len(self.stub.sessions[live_key]["messages"]), 1)
        rep = json.loads(report.read_text(encoding="utf-8"))
        self.assertEqual(rep["backfill"]["deleted"], 1)
        for counter in ("peers", "sessions", "messages", "observations"):
            self.assertIn("before", rep[counter])
            self.assertIn("after", rep[counter])

    def test_no_marked_sessions_zero_delete(self):
        for key in [k for k, s in self.stub.sessions.items()
                    if BATCH_KEY in s["metadata"]]:
            self.stub.sessions.pop(key)
        rc = self.rb.main(["--apply"])
        self.assertEqual(rc, 0)
        self.assertNotIn("delete_sessions",
                         [c[0] for c in self.stub.calls])


class _FakeResp:
    """urllib urlopen 桩响应（Page JSON 体；上下文管理器形态）。"""

    def __init__(self, payload):
        self._raw = json.dumps(payload).encode("utf-8")

    def read(self, size=-1):
        return self._raw

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        return False


class RestWireCase(unittest.TestCase):
    """HonchoRestClient wire 形状单测（stub urllib 传输层；离线零网络）。

    断言形状全部对 upstream main@7478be29 源码实测：/v3 + workspace 作用域
    路径、SessionCreate/MessageCreate alias 字段、100 条分块、Page.items
    分页走全量、Bearer 凭据（HONCHO_JWT 环境变量名引用）、DELETE 202/404
    语义、canonical↔wire 名可逆编码（资源名 pattern ^[a-zA-Z0-9_-]+$）。
    """

    def setUp(self):
        self.mod = load_module(BACKFILL_PY, "unit_backfill_wire")
        self.canon = ("agent:kidpal:openai-user:aa:bb:cc:dd:ee:ff"
                      ":backfill:20261023T000000Z:session-corpus")
        self.wire = self.mod.wire_session_name(self.canon)
        self.client = self.mod.HonchoRestClient("http://127.0.0.1:18100/")
        self.calls = []

    # -- 传输层桩 -------------------------------------------------------------- #
    def _install(self, responder):
        def fake_urlopen(req, timeout=None):
            body = json.loads(req.data.decode("utf-8")) if req.data else None
            self.calls.append({
                "method": req.get_method(), "url": req.full_url,
                "body": body,
                "headers": {k.lower(): v for k, v in req.header_items()},
            })
            payload, status = responder()
            if status >= 400:
                raise urllib.error.HTTPError(
                    req.full_url, status, "err", None, io.BytesIO(b'{"d":1}'))
            return _FakeResp(payload)
        patcher = mock.patch.object(self.mod.urllib.request, "urlopen", fake_urlopen)
        patcher.start()
        self.addCleanup(patcher.stop)

    @staticmethod
    def _page(items, total=None):
        page = {"items": items, "page": 1, "size": 100}
        page["total"] = total if total is not None else len(items)
        return page

    # -- 断言 ------------------------------------------------------------------ #

    def test_wire_name_roundtrip_and_pattern(self):
        w = self.mod.wire_session_name(self.canon)
        self.assertRegex(w, r"^[A-Za-z0-9_-]+$")  # 资源名 pattern 合规（api.py:46）
        self.assertTrue(w.startswith("kidpal-bf-"))
        self.assertEqual(self.mod.canonical_session_name(w), self.canon)
        self.assertEqual(self.mod.wire_session_name(self.canon), w)  # 确定不变
        self.assertIsNone(self.mod.canonical_session_name("plugin-session-abc"))

    def test_create_session_v3_workspace_alias_body(self):
        self._install(lambda: (self._page([]), 200))
        self.client.create_session(
            "owner", self.canon, {BATCH_KEY: "agent:kidpal:openai-user:x"})
        call = self.calls[0]
        self.assertEqual(call["method"], "POST")
        self.assertEqual(
            call["url"], "http://127.0.0.1:18100/v3/workspaces/kidpal/sessions")
        self.assertEqual(call["body"]["peers"], {"owner": {}})  # peer_names alias
        self.assertEqual(
            call["body"]["metadata"],
            {BATCH_KEY: "agent:kidpal:openai-user:x"})
        wire_id = call["body"]["id"]  # SessionCreate.name 的 alias id
        self.assertEqual(wire_id, self.wire)
        self.assertRegex(wire_id, r"^[A-Za-z0-9_-]+$")

    def test_add_message_single_batch_alias_and_peer_routing(self):
        self._install(lambda: ([], 201))
        self.client.add_message(self.canon, "user", "你好",
                                {FP_KEY: "v1:session-corpus:" + "0" * 64})
        call = self.calls[0]
        self.assertEqual(call["method"], "POST")
        self.assertEqual(
            call["url"],
            "http://127.0.0.1:18100/v3/workspaces/kidpal/sessions/"
            + self.wire + "/messages")
        self.assertEqual(call["body"], {"messages": [{
            "content": "你好", "peer_id": "owner",
            "metadata": {FP_KEY: "v1:session-corpus:" + "0" * 64}}]})

    def test_add_message_assistant_routes_agent_peer(self):
        self._install(lambda: ([], 201))
        self.client.add_message(self.canon, "assistant", "回", {})
        self.assertEqual(self.calls[0]["body"]["messages"][0]["peer_id"],
                         "agent-kidpal")

    def test_add_message_toolresult_and_tool_route_agent_peer(self):
        # review §2.1 ⑦ / F4：toolResult（含未来 tool*）路由 agent-kidpal
        # （课表正文类 toolResult 归 owner 会派生「owner has a 课表」伪事实）。
        self._install(lambda: ([], 201))
        for role in ("toolResult", "tool", "TOOLRESULT"):
            self.client.add_message(self.canon, role, "课表正文", {})
        peers = [c["body"]["messages"][0]["peer_id"] for c in self.calls]
        self.assertEqual(peers, ["agent-kidpal", "agent-kidpal",
                                 "agent-kidpal"])  # 小写归一后路由
        self.client.add_message(self.canon, "user", "孩子话", {})
        self.assertEqual(self.calls[-1]["body"]["messages"][0]["peer_id"],
                         "owner")

    def test_add_message_created_at_wire_row_and_omission(self):
        # created_at 非 None → MessageCreate.created_at 出网；None → 键缺席
        # （服务端写入时刻缺省，向后兼容旧 wire 形状）。
        self._install(lambda: ([], 201))
        self.client.add_message(self.canon, "user", "一", {},
                                created_at="2026-09-30T04:00:00+00:00")
        row = self.calls[0]["body"]["messages"][0]
        self.assertEqual(row["created_at"], "2026-09-30T04:00:00+00:00")
        self.client.add_message(self.canon, "user", "二", {})
        self.assertNotIn("created_at",
                         self.calls[1]["body"]["messages"][0])

    def test_add_messages_chunks_over_100(self):
        self._install(lambda: ([], 201))
        rows = [{"content": str(i), "peer_id": "owner", "metadata": {}}
                for i in range(250)]
        self.client._add_messages(self.canon, rows)
        posts = [c for c in self.calls if c["method"] == "POST"]
        self.assertEqual(len(posts), 3)
        self.assertEqual(
            [len(c["body"]["messages"]) for c in posts], [100, 100, 50])
        for c in posts:
            self.assertEqual(
                c["url"], "http://127.0.0.1:18100/v3/workspaces/kidpal/sessions/"
                + self.wire + "/messages")
            self.assertLessEqual(len(c["body"]["messages"]),
                                 self.mod.HonchoRestClient.MESSAGE_BATCH_LIMIT)
        flattened = [m for c in posts for m in c["body"]["messages"]]
        self.assertEqual(flattened, rows)  # 分块零丢序

    def test_snapshot_counts_page_walk_and_endpoints(self):
        mod = self.mod

        class _Small(mod.HonchoRestClient):
            LIST_PAGE_SIZE = 2

        client = _Small("http://127.0.0.1:18100/")
        plan = iter([
            self._page([{"id": "owner"}, {"id": "agent-kidpal"}], total=3),
            self._page([{"id": "p3"}], total=3),        # peers 第 2 页 → 走满
            self._page([{"id": self.wire, "metadata": {}}], total=1),
            self._page([{}, {}, {}], total=3),          # session 消息页
            self._page([{}, {}], total=2),              # conclusions 页
        ])
        self._install(lambda: (next(plan), 200))
        counts = client.snapshot_counts()
        self.assertEqual(counts, {"peers": 3, "sessions": 1,
                                  "messages": 3, "observations": 2})
        urls = [c["url"] for c in self.calls]
        self.assertEqual(urls, [
            "http://127.0.0.1:18100/v3/workspaces/kidpal/peers/list?page=1&size=2",
            "http://127.0.0.1:18100/v3/workspaces/kidpal/peers/list?page=2&size=2",
            "http://127.0.0.1:18100/v3/workspaces/kidpal/sessions/list?page=1&size=2",
            "http://127.0.0.1:18100/v3/workspaces/kidpal/sessions/"
            + self.wire + "/messages/list?page=1&size=2",
            "http://127.0.0.1:18100/v3/workspaces/kidpal/conclusions/list?page=1&size=2",
        ])

    def test_list_backfill_batches_and_fingerprints_read_metadata(self):
        mod = self.mod

        class _Small(mod.HonchoRestClient):
            LIST_PAGE_SIZE = 2

        client = _Small("http://127.0.0.1:18100/")
        plan = iter([
            self._page([{"id": self.wire,
                         "metadata": {BATCH_KEY: self.canon}}], total=1),
            self._page([{"id": self.wire,
                         "metadata": {BATCH_KEY: self.canon}}], total=1),
            self._page([{"metadata": {FP_KEY: "fp1"}}], total=1),
        ])
        self._install(lambda: (next(plan), 200))
        self.assertEqual(client.list_backfill_batches(), {self.canon})
        self.assertEqual(client.list_fingerprints(), {"fp1"})

    def test_delete_sessions_202_counts(self):
        self._install(lambda: ({"message": "ok"}, 202))
        deleted = self.client.delete_sessions([self.canon])
        self.assertEqual(deleted, 1)
        self.assertEqual(self.calls[0]["method"], "DELETE")
        self.assertEqual(
            self.calls[0]["url"],
            "http://127.0.0.1:18100/v3/workspaces/kidpal/sessions/" + self.wire)

    def test_bearer_token_from_env_name_reference(self):
        self._install(lambda: ([], 201))
        with mock.patch.dict(os.environ, {self.mod.HonchoRestClient.JWT_ENV_VAR:
                                          "tok-123"}):
            self.client._request("GET", "/probe")
        self.assertEqual(
            self.calls[0]["headers"].get("authorization"), "Bearer tok-123")
        self.calls.clear()
        self.client._request("GET", "/probe")
        self.assertNotIn("authorization", self.calls[0]["headers"])  # 零凭据形态

    def test_delete_404_idempotent_skip(self):
        self.calls.clear()
        state = {"n": 0}

        def responder():
            state["n"] += 1
            return ({"message": "ok"}, 202) if state["n"] == 1 else (
                {"detail": "nf"}, 404)

        self._install(responder)
        deleted = self.client.delete_sessions([self.canon, self.canon])
        self.assertEqual(deleted, 1)  # 202 计数、404 幂等跳过
        self.assertEqual(self.calls[0]["method"], "DELETE")


class EnvExampleSyncCase(unittest.TestCase):
    """honcho.env.example 两键同步（K4B-E5）+ K3 既有锚零回退。"""

    def test_k4b_two_keys_present_with_k3_anchors_intact(self):
        src = ENV_EXAMPLE.read_text(encoding="utf-8")
        # D4 键：DashScope 兼容端点 batch 上限 10（真值在模板）。
        self.assertIn("EMBEDDING_MODEL_CONFIG__MAX_BATCH_SIZE=10", src)
        self.assertIn("src/embedding_client.py:250", src)   # 锚注记在场
        # LLM_DEFAULT_MAX_TOKENS=8000 注释口径（ECS live 已改未入仓；模板
        # 只放注释模板行，不设值）。
        self.assertIn("# LLM_DEFAULT_MAX_TOKENS=8000", src)
        self.assertNotRegex(src, r"(?m)^LLM_DEFAULT_MAX_TOKENS=")
        # K3 既有锚零回退。
        for anchor in ("EMBEDDING_VECTOR_DIMENSIONS=1536",
                       "DERIVER_FLUSH_ENABLED=true",
                       "DERIVER_LLM_BASE_URL=https://api.deepseek.com/v1",
                       "DERIVER_LLM_MODEL=deepseek-v4-flash",
                       "DASHSCOPE_BASE_URL=https://dashscope.aliyuncs.com"
                       "/compatible-mode/v1",
                       "DASHSCOPE_EMBEDDING_MODEL=text-embedding-v4"):
            self.assertIn(anchor, src, f"K3 锚回退: {anchor!r}")


if __name__ == "__main__":
    unittest.main(verbosity=2)
