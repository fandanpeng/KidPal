#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""backfill 工具族单元测试（TaskPackage 261022-m5-k4）。

覆盖冻结验收之外的分支与边界：语料行解析（角色透传 / 兜底收编 / 空行跳过）、
指纹确定性与锚敏感性（同内容不同行不互吞）、transcript_events 缺表缺列 = 红、
用法错 / 数据错 exit 码分档、单源运行、部分幂等补灌、回滚恰删标记 session。
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

    def add_message(self, session_key, role, content, metadata):
        self.calls.append(("add_message", session_key, role, content))
        self.sessions.setdefault(
            session_key, {"peer": "", "metadata": {}, "messages": []})
        self.sessions[session_key]["messages"].append(
            (role, content, dict(metadata)))

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
        messages, unparsed = self.mod.parse_corpus_dir(corpus)
        self.assertEqual(len(messages), 4)
        self.assertEqual(unparsed, 1)
        self.assertTrue(messages[2]["unparsed"])
        self.assertEqual(messages[0]["role"], "user")
        self.assertEqual(messages[1]["role"], "assistant")

    def test_corpus_empty_dir_zero_planned(self):
        corpus = self.root / "empty"
        corpus.mkdir()
        messages, unparsed = self.mod.parse_corpus_dir(corpus)
        self.assertEqual((messages, unparsed), ([], 0))

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


if __name__ == "__main__":
    unittest.main(verbosity=2)
