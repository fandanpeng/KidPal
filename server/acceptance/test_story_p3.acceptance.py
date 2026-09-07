"""Frozen acceptance tests for TaskPackage 260829-story-p3 (subsystem: server / PC 云端).

Scope of record (taskpackage acceptance/exitCriteria + 闸 A 判读 + notes 硬约束):
P3-A4 云端半边 (停录后**整段转写**归档 stories/*.md — 我方引擎落盘, OpenClaw 一回合
不读写文件、仅起标题), P3-A2 云端半边 (story_list 下发: total = 故事果**全集**计数、
items 分页/截断时不得以 items.length 顶替), P3-A5 云端半边 (story_text 下发
{id,title,text} 形状, 无生字标注字段 — P3-E4 已裁), V31 **四条显式回退路径**
(录音中取消 discard / 停录 10 秒内「重录」撤销上一归档 / 漏判「刚才那段是故事」
挽回归档 / 「讲错啦」回闲聊丢弃), 以及 notes.blockingDecision_memoryPressure 的
**累积缓冲有界**硬约束 (上限 N、仅活动会话; 防 char-stroke §8.3 内存叠压先例)。
设备半边 (列表/光标/录音演出/回看回听/盖章帧) 在 firmware/main/test/story_p3.acceptance.c,
两面证据合起来才是 P3-E2。

真实麦克风采音 / 真 ASR 引擎 / TTS 声学 **不在本文件 auto-run 面** (P3-E2: 采音为
硬件强绑定子能力, 显式殿后包 9b); 本文件用**注入转写文本 + 真实落盘**断言归档与
下发语义, 用 @pytest.mark.live 记录真链路义务 (conftest 机械闸不 auto-run)。

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement, pure python, NO fastapi /
gateway-front imports at import time — offline-importable anywhere; recommended
home: server/gateway/story.py, same pure-module precedent as
gateway/schedule.py and gateway/display_schema.py):

  archive_story(stories_dir, transcript, *, title=None, slug=None, date=None) -> dict
      把**整段转写**归档为一个故事文件 <stories_dir>/<id>.md (真实落盘)。
      返回 record = {"id","title","date","text","path"}:
        id    = "<YYYY-MM-DD>-<slug>"; slug 给定则用之 (ASCII/小写/短横),
                否则由 title 生成安全 slug, 无可用 ASCII 时用稳定散列
                "s<8hex>" (由 title 决定, 可复现);
        date  = 显式 date ("YYYY-MM-DD") 优先, 否则取当日 (Asia/Shanghai);
        title = 显式 title 优先; 缺省时**非空回退** = 正文首个标点前的片段
                截断至 <=12 字符 (S-F2.5 起标题归云端/OpenClaw, 但列表行不能空标题);
        text  = transcript 原文 (逐字, 首尾空白剥离);
        path  = 落盘文件路径 (str)。
      transcript 为 None / 空 / 仅空白 -> 抛 ValueError (空故事不入库, 大声拒绝,
      不静默产出空归档). 同 id 再归档 -> 覆盖同一文件 (幂等重写, 不产生第二颗)。

  read_story_file(path) -> dict | None
      读回一个归档文件为 record; 非故事记录文件 (无本模块记录头) -> None。

  list_stories(stories_dir, *, limit=None, offset=0) -> dict
      契约 §1.3 ③ story_list **producer 形态**下发块:
        {"view": "story_list",
         "data": {"total": <故事果全集计数>,
                  "items": [{"id","title","date"} , ...]}}
      - total = 目录内**全部**可解析故事记录数 (云端算, 全集), 与 limit/offset 无关;
        **绝不**用 len(items) 顶替 (契约 §一 story_list 硬约束)。
      - items 按**完整日期**倒序 (最新在前; 同日按 id 倒序稳定), date 显示为 "MM-DD"
        (排序用完整日期 — "MM-DD" 跨年不可比)。
      - item keys 恰为 {"id","title","date"} (零增减, 契约 data 形状不锁扩展位但
        producer 不发明字段)。
      - limit/offset 只切 items 页, total 不动; offset 超界 -> items [] 且 total 原值。
      - 空目录 / 只有杂物文件 -> total 0, items [] (非 0 items 假装 3 颗)。
      - 必须整包过 gateway.display_schema.validate_display (真实校验器, 断言在下)。

  story_text(record) -> dict
      契约 §1.3 ④ story_text 下发块: {"view":"story_text",
      "data":{"id","title","text"}} — data keys 恰为 {id,title,text}, 无生字标注
      /高亮字段 (P3-E4)。record 可为 archive_story/list_stories 回读的记录。
      必须过 validate_display。

  class StoryAccumulator:
      # manual 聆听累积态 + 归档 + V31 四条回退 (契约 §1.4 start_recording:
      # 「网关切该会话为累积态(不按句送对话)」+ spec S-F2.2 V31)。纯逻辑 + 真实落盘。
      # 不 import fastapi/websockets; 会话标识由调用方 (front/engine) 传入。

      StoryAccumulator(stories_dir, *, sentence_window=24, char_window=4000,
                       max_sessions=8, redo_window_s=10.0)
          sentence_window / char_window = **每会话**「近 N 句转写」缓冲硬上限
              (句数与字节数双上限, 超限 FIFO 淘汰最旧句 — notes 硬约束: 缓冲有界);
          max_sessions = 网关同时持有的会话上限 (LRU 淘汰最久未活动会话的缓冲,
              使整体内存有界: 绝不因会话数增长而叠压常驻内存);
          redo_window_s = V31「停录后 10 秒内说重录」窗口 (秒)。

      routing(session_id, now=None) -> str
          "accumulate" (累积态: 转写只进缓冲、**不按句送对话**) | "chat" (闲聊态)。
          默认 "chat"; start() 后 "accumulate"; stop()/cancel() 后回到 "chat"。

      start(session_id, now=None) -> None
          收到 control/start_recording -> 该会话切累积态。重复 start 幂等
          (不重复开会话、不清空已在累积的内容)。

      feed(session_id, text, now=None) -> int
          注入一句上行转写。返回该会话「近 N 句」缓冲当前句数。
          - 累积态: 文本进**累积缓冲** (供停录整段归档), 同时进有界近句环;
            **不**逐句送对话 (routing=="accumulate" 时 chat 侧不得收到该句 —
            由 routing() 与该返回值共同表达)。
          - 闲聊态: 文本只进有界近句环 (供 V31 漏判挽回), 不进累积缓冲。
          - text 空/仅空白 -> 不入库, 返回当前深度 (不抛, 但也不计一句)。

      stop(session_id, now=None, *, title=None, slug=None) -> dict | None
          「讲完啦」-> 整段转写(各句以 \n 连接) 经 archive_story 落盘, 返回 record;
          会话回闲聊态、累积缓冲**释放**。累积缓冲为空 -> 返回 None 且**不写文件**。
          记录 (撤销凭据): 成功后内部记住该会话的上一归档 record + 停录时刻,
          供 redo 在窗口内撤销。

      cancel(session_id, now=None, *, reason="") -> bool
          V31 取消/discard: 录音中 KEY 长按取消 **或** 说「讲错啦」(reason 携文案)
          -> 丢弃已累积内容、不归档、会话回闲聊态。返回是否有累积内容被丢弃。
          关键区分: 被 cancel 丢弃的内容 **不可** 再被 recover 挽回
          (V31「已累积内容丢弃」; 只有闲聊态进入近句环的句子可挽回)。

      redo(session_id, now=None, *, title=None, slug=None) -> dict | None
          V31「停录后 10 秒内说重录」-> 撤销上一归档 (删除刚写的文件) + 把整段
          转写放回该会话累积缓冲 + 会话回累积态; 返回被撤销的 record。
          无上一归档 / 已超出 redo 窗口 -> None 且**归档保持不动**。
          窗口判定用注入时刻 (now), 不用墙钟 (可测、可复现)。

      recover(session_id, now=None, *, title=None, slug=None) -> dict | None
          V31「刚才那段是故事」漏判挽回 -> 把该会话近句环保留的转写整段归档,
          返回 record, 并清空该环 (第二次 recover 必须 None — 不重复入册)。
          环为空 / 内容已被 cancel 丢弃 -> None (不编造归档)。

      recent_depth(session_id) -> int        当前近句环保留句数 (<= sentence_window)
      recent_chars(session_id) -> int        当前近句环字符数 (<= char_window)
      active_sessions() -> list[str]         仍持有缓冲/会话态的会话 id
      total_buffer_bytes() -> int            全部会话缓冲的字符总量 (内存叠压可测面)

  契约零改动红线 (闸 A 判 ④ NO_CONTRACT_CHANGE): 本包**不**新增 view、**不**给
  story_list/story_text 加字段; 若实现需要 story_text 增 total 或生字标注字段,
  那是 ① 契约触点, 必须回 planner 重走契约门, 不得在本包内自行改。
===========================================================================

Coverage map (acceptanceRef / exitCriteria / decisionCoverage -> test):
  P3-A1 (server 半边, structural) -> test_a1_story_module_landed_and_reached
  P3-A2 (cloud: story_list 下发)  -> test_a2_list_total_is_full_set_not_page_slice
                                     -> test_a2_list_ordering_item_shape_and_junk_ignored
                                     -> test_a2_list_empty_dir_is_zero_not_fake
  P3-A4 (cloud: 整段转写归档)     -> test_a4_archive_writes_real_md_and_roundtrips
                                     -> test_a4_archive_rejects_empty_transcript
                                     -> test_a4_archive_title_fallback_is_nonempty
                                     -> test_a4_accumulate_mode_not_sentence_by_chat
                                     -> test_a4_stop_archives_whole_transcript_once
  P3-A5 (cloud: story_text 下发)  -> test_a5_story_text_shape_no_newword_field
  P3-A6 (N 与计数同步的云侧承载)  -> test_a2_list_total_* (total=全集 = 盖章「第 N
                                     颗」的权威源) + decision contract-§一 齿
  V31 四条回退                    -> test_v31_cancel_discards_without_archiving   (KEY 长按取消)
                                     -> test_v31_wrong_take_returns_to_chat       (「讲错啦」)
                                     -> test_v31_redo_within_window_undoes_archive(10s「重录」)
                                     -> test_v31_redo_after_window_keeps_archive
                                     -> test_v31_recover_rescues_missed_story     (漏判挽回)
                                     -> test_v31_cancelled_content_not_recoverable
  缓冲有界 (notes 硬约束/内存压力) -> test_buffer_bounded_per_session_ring
                                     -> test_buffer_bounded_across_sessions
  prompt 载体 (scopePromptV3, 验收所测=生产所用)
                                  -> test_prompt_carrier_v3_landed_and_wired
  P3-E4 (不做生字反白高亮)        -> test_a5_story_text_shape_no_newword_field
  P3-E3 (润色后置) + 契约零改动    -> test_boundary_contract_surface_untouched
  P3-E2 (两面证据 + 既有验收回归)  -> test_e2_existing_directionb_acceptance_green
  live (不 auto-run, 机械闸 skip)  -> StoryP3LiveAcceptance.test_live_story_roundtrip_evidence

Red baseline (freeze state, 2026-09-08 acceptance-author 实测): server/gateway has
NO story module — 每条 offline 齿红在「冻结离线契约未实现 / producer 载体未落盘」
的真实缺失行为上; 收集零错误 (§3 冻结即冒烟: pytest collection error = 0);
display_schema 白名单/上限守卫与 directionB 回归守卫必须保持绿。实测计数见
firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md。

Run protocol:
  cd server && wsl -d Ubuntu-24.04 -- python -m pytest acceptance/test_story_p3.acceptance.py -q --import-mode=importlib
  (two-dot filename needs --import-mode=importlib, G 包先例; live class skipped
  unless RUN_LIVE=1 / KSP3_RUN_LIVE=1 or -m live explicitly requested)
  standalone: python acceptance/test_story_p3.acceptance.py
"""

from __future__ import annotations

import importlib.util
import json
import os
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; the standalone runner also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
STORY_MODULE = SERVER / "gateway" / "story.py"
DEPLOY_DIR = SERVER / "deploy"
OVERLAY_WIRING = DEPLOY_DIR / "apply-xiaozhi-overlay.sh"
PROVIDER_DECLARE = DEPLOY_DIR / "provider.config.yaml"
LIVE_EVIDENCE_DIR = REPO_ROOT / "changes" / "dev" / "260829-story-p3"

# 冻结契约入口符号 (structural 齿: 定义 + 服务路径消费)
STORY_ENTRY_NEEDLES = ("StoryAccumulator(", "gateway.story import", ".story import")

# --------------------------------------------------------------------------- #
# Offline loading of the frozen story module (deterministic red on missing)     #
# --------------------------------------------------------------------------- #

_module_cache: dict = {}


def _load_story_module():
    """Import server/gateway/story.py by path, offline.

    A missing/unimportable module is a RED for missing behaviour (self.fail),
    never a collection crash.
    """
    if "mod" in _module_cache:
        return _module_cache["mod"], _module_cache["err"]
    if not STORY_MODULE.is_file():
        err = ("server/gateway/story.py missing — the 整段转写归档 / story_list·"
               "story_text 下发 / V31 四回退 / 有界累积缓冲 pure module is not "
               "implemented (frozen offline contract).")
        _module_cache["mod"], _module_cache["err"] = None, err
        return None, err
    try:
        spec = importlib.util.spec_from_file_location("kidpal_gateway_story", STORY_MODULE)
        if spec is None or spec.loader is None:
            return None, "server/gateway/story.py not loadable"
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _module_cache["mod"], _module_cache["err"] = mod, None
        return mod, None
    except Exception as e:  # noqa: BLE001 — an import error here is a RED, not a crash
        err = (f"server/gateway/story.py failed to import: {e!r}. The frozen story "
               "module MUST be pure python (no fastapi/gateway-front imports at import "
               "time) so this acceptance runs offline anywhere.")
        _module_cache["mod"], _module_cache["err"] = None, err
        return None, err


def _story_attr(name: str):
    """Fetch one frozen-contract entry point; (callable|None, err)."""
    mod, err = _load_story_module()
    if mod is None:
        return None, err
    attr = getattr(mod, name, None)
    if attr is None:
        return None, (f"story.py has no module-level {name} — frozen offline contract "
                      "not implemented.")
    return attr, None


def _display_schema():
    """The REAL frozen gateway display validator (V27) — shape gate of record."""
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.display_schema as ds  # noqa: PLC0415 — offline import, cached by python
    return ds


def _code_files_referring(needles, *, exclude_self=True):
    """Distinct CODE files (.py/.sh) under server/ whose text contains ANY needle.

    Restricted to code files on purpose: prose (.md) 前瞻注记 once turned a
    deploy-wiring tooth green without any real wiring (schedule-p2 lesson) — and
    the same trap would false-green the consumption tooth here.
    """
    hits = set()
    for p in sorted(SERVER.rglob("*")):
        if not p.is_file() or p.suffix not in (".py", ".sh"):
            continue
        if "__pycache__" in p.parts or "acceptance" in p.relative_to(SERVER).parts:
            continue
        if exclude_self and p == STORY_MODULE:
            # 仍计入 definition 侧: 由调用方自行判断 (story.py 定义类 -> 匹配 needle)
            pass
        try:
            text = p.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        if any(n in text for n in needles):
            hits.add(str(p.relative_to(REPO_ROOT).as_posix()))
    return hits


def _ts(day: str, hour: int = 0, minute: int = 0, second: float = 0.0):
    """Deterministic injected clock (the frozen contract takes explicit `now`).

    入参做进位归一：把 day 零点 + (hour,minute,second) 偏移量用 timedelta 表达,
    使 second/minute 允许 >=60 (如 second=60 表「停录后 60 秒」), 而不会让
    datetime(..., second=60) 在**实参求值阶段** ValueError 抛崩 (那是线束崩溃、
    非被测行为缺失, 任何实现都规避不了)。既有 0..59 范围内的全部调用结果
    逐字不变 —— 只修时钟构造, 不碰任何断言、阈值或语义。
    """
    import datetime as _dt

    d = _dt.date.fromisoformat(day)
    offset = hour * 3600 + minute * 60 + float(second)
    return _dt.datetime(d.year, d.month, d.day) + _dt.timedelta(seconds=offset)


class _TmpDir:
    """Context-manager temp dir for REAL archive writes."""

    def __enter__(self):
        self._path = tempfile.mkdtemp(prefix="ksp3_")
        return Path(self._path)

    def __exit__(self, *exc):
        import shutil  # noqa: PLC0415

        shutil.rmtree(self._path, ignore_errors=True)
        return False


# --------------------------------------------------------------------------- #
# Offline structural / behavioral acceptance — the red baseline teeth           #
# --------------------------------------------------------------------------- #

class StoryP3OfflineAcceptance(unittest.TestCase):
    """Offline teeth: 归档落盘、story_list/story_text 下发、V31 四回退、缓冲有界、
    prompt 载体接线、契约零改动红线、既有验收回归守卫。"""

    maxDiff = None

    # --- P3-A1 (server 半边, structural) ------------------------------------
    def test_a1_story_module_landed_and_reached(self):
        """coverage: P3-A1 server 半边 — story 模块落盘 且 被真实服务路径消费
        (定义 + 消费 >= 2 个 code 文件; 只定义不调用 = 浮模块, 不算接入)。"""
        self.assertTrue(
            STORY_MODULE.is_file(),
            "P3-A1 RED: server/gateway/story.py missing — 云端故事面（归档/下发/"
            "V31 回退）未落地，故事录制链路只有设备半边 (契约 §1.4 start_recording "
            "「网关切该会话为累积态」).")
        text = STORY_MODULE.read_text(encoding="utf-8", errors="replace")
        self.assertTrue(text.strip(), "P3-A1 RED: story.py is empty.")
        for entry in ("archive_story", "list_stories", "story_text", "StoryAccumulator"):
            self.assertIn(
                entry, text,
                f"P3-A1 RED: story.py carries no {entry} — 冻结离线契约入口缺失。")
        hits = _code_files_referring(STORY_ENTRY_NEEDLES)
        consumer_hits = {h for h in hits if not h.endswith("gateway/story.py")}
        self.assertTrue(
            consumer_hits,
            "P3-A1 RED: no serving-path code file references the story module "
            "(StoryAccumulator(...) / gateway.story import) — 归档/累积态未接入真实"
            "服务路径（front_app / engine-patch / shim 之一），仅定义不调用不算接入 "
            "(「验收所测 = 生产所用」)。")

    # --- P3-A4 云端半边: 整段转写归档 --------------------------------------
    def test_a4_archive_writes_real_md_and_roundtrips(self):
        """coverage: P3-A4 云端 — 停录后整段转写真实落盘 stories/*.md 并可读回;
        同 id 幂等重写不产生第二颗。"""
        archive, err = _story_attr("archive_story")
        if archive is None:
            self.fail("P3-A4 RED: " + err)
        list_stories, err2 = _story_attr("list_stories")
        if list_stories is None:
            self.fail("P3-A4 RED: " + err2)
        read_one, err3 = _story_attr("read_story_file")
        if read_one is None:
            self.fail("P3-A4 RED: " + err3)

        with _TmpDir() as d:
            text = "从前有一只小猫。它去了森林。遇见了一只大狗。"
            rec = archive(str(d), text, title="小猫去森林", slug="xiaomao",
                          date="2026-09-08")
            self.assertIsInstance(rec, dict,
                                  "P3-A4 RED: archive_story must return the record dict.")
            for key in ("id", "title", "date", "text", "path"):
                self.assertIn(key, rec,
                              f"P3-A4 RED: record missing key {key!r} — 归档记录形状 "
                              "{id,title,date,text,path} 是列表/正文下发的共同源。")
            self.assertEqual(rec["id"], "2026-09-08-xiaomao",
                             f"P3-A4 RED: id must be '<date>-<slug>', got {rec['id']!r} "
                             "(契约 §1.3 ③ 示例 id 形态)。")
            self.assertEqual(rec["title"], "小猫去森林", "P3-A4 RED: title not carried.")
            self.assertIn("遇见了一只大狗", rec["text"],
                          "P3-A4 RED: text must carry the WHOLE transcript (整段转写), "
                          "not the last sentence only.")
            path = Path(rec["path"])
            self.assertTrue(path.is_file() and path.stat().st_size > 0,
                            f"P3-A4 RED: no real story file on disk at {path} — mock "
                            "调用形状不算归档 (acceptance-evidence: 数据路径须真落盘)。")
            self.assertIn(d.resolve(), path.resolve().parents,
                          f"P3-A4 RED: the archive landed outside stories_dir "
                          f"({path} not under {d}) — 归档路径由调用方注入。")
            on_disk = path.read_text(encoding="utf-8")
            self.assertIn("小猫去森林", on_disk,
                          "P3-A4 RED: archived MD does not contain the title.")
            self.assertIn("从前有一只小猫", on_disk,
                          "P3-A4 RED: archived MD does not contain the transcript text.")

            back = read_one(str(path))
            self.assertIsInstance(back, dict,
                                  "P3-A4 RED: read_story_file could not read back the "
                                  "record it just wrote (归档自读不闭环)。")
            self.assertEqual(back.get("id"), "2026-09-08-xiaomao",
                             "P3-A4 RED: read-back id mismatch.")
            self.assertEqual(back.get("title"), "小猫去森林", "P3-A4 RED: read-back title mismatch.")
            self.assertIn("遇见了一只大狗", back.get("text", ""),
                          "P3-A4 RED: read-back text mismatch (正文丢失)。")

            # 幂等: 同 id 再归档 -> 覆盖同一文件, 计数不涨 (回执重放不重复发徽章)
            again = archive(str(d), text, title="小猫去森林", slug="xiaomao",
                            date="2026-09-08")
            self.assertEqual(again["id"], rec["id"], "P3-A4 RED: id not reproducible.")
            payload = list_stories(str(d))
            self.assertEqual(payload["data"]["total"], 1,
                             "P3-A4 RED: re-archiving the same story inflated the "
                             "故事果 count (mcp 回执重试/重写不得重复入册)。")
            self.assertEqual(len(list(d.glob("*.md"))), 1,
                             "P3-A4 RED: two archive files for one story id.")

    def test_a4_archive_rejects_empty_transcript(self):
        """coverage: P3-A4 边界/错误路径 — 空/仅空白转写必须大声拒绝, 不静默产出空故事。"""
        archive, err = _story_attr("archive_story")
        if archive is None:
            self.fail("P3-A4 RED: " + err)
        with _TmpDir() as d:
            for bad in ("", "   \n\t ", None):
                with self.assertRaises(ValueError,
                                       msg=f"P3-A4 RED: an empty transcript ({bad!r}) "
                                           "must raise ValueError — 空归档会给孩子盖上"
                                           "一颗不存在的故事果。"):
                    archive(str(d), bad, title="空", date="2026-09-08")
            self.assertEqual(list(d.glob("*.md")), [],
                             "P3-A4 RED: a refused archive still wrote a file.")

    def test_a4_archive_title_fallback_is_nonempty(self):
        """coverage: P3-A4 边界 — title 缺省时仍产出**非空**标题 (列表行不能空白)。"""
        archive, err = _story_attr("archive_story")
        if archive is None:
            self.fail("P3-A4 RED: " + err)
        with _TmpDir() as d:
            rec = archive(str(d), "从前有座山，山里有座庙。", date="2026-09-08")
            title = rec.get("title")
            self.assertIsInstance(title, str,
                                  "P3-A4 RED: title must be a str even when the caller "
                                  "omits it (S-F2.5 起标题归云端, 但落盘标题不可空)。")
            self.assertTrue(title.strip(),
                            "P3-A4 RED: fallback title is empty — P3 列表会出现空行标题。")
            self.assertLessEqual(len(title), 12,
                                 f"P3-A4 RED: fallback title {title!r} longer than the "
                                 "frozen 12-char cap (400×300 列表行显示位)。")
            # 无 ASCII 可转写时 id 仍需稳定可复现
            rec2 = archive(str(d), "第二句正文。", date="2026-09-09")
            self.assertRegex(
                str(rec2.get("id", "")), r"^2026-09-09-.+$",
                "P3-A4 RED: id must keep the '<YYYY-MM-DD>-<slug>' shape even for a "
                "pure-CJK title (散列回退可接受, 空 slug 不可)。")

    def test_a4_accumulate_mode_not_sentence_by_chat(self):
        """coverage: P3-A4 云端语义 (契约 §1.4) — start_recording 后该会话切累积态,
        转写不按句送对话; 停录/取消后回闲聊态。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("P3-A4 RED: " + err)
        with _TmpDir() as d:
            acc = acc_cls(str(d))
            self.assertEqual(acc.routing("sess-1"), "chat",
                             "P3-A4 RED: a fresh session must start in 闲聊态.")
            acc.start("sess-1")
            self.assertEqual(acc.routing("sess-1"), "accumulate",
                             "P3-A4 RED: control/start_recording did not switch the "
                             "session to 累积态 (契约 §1.4「网关切该会话为累积态、"
                             "不按句切分送对话」)。")
            depth = acc.feed("sess-1", "我要讲一个关于星星的故事。")
            self.assertEqual(depth, 1,
                             f"P3-A4 RED: feed did not retain the sentence (got {depth}).")
            acc.feed("sess-1", "星星住在一座高山上。")
            rec = acc.stop("sess-1", title="星星与高山", slug="xingxing",
                           date="2026-09-08")
            self.assertIsInstance(rec, dict,
                                  "P3-A4 RED: stop must archive the accumulated 整段转写.")
            self.assertIn("星星住在一座高山上", rec["text"],
                          "P3-A4 RED: stop archived only part of the accumulated text.")
            self.assertEqual(acc.routing("sess-1"), "chat",
                             "P3-A4 RED: after 停录 the session must leave 累积态 "
                             "(否则后续闲聊也被吞进不存在的录音)。")
            self.assertIsNone(acc.stop("sess-1"),
                              "P3-A4 RED: a second stop with an empty accumulation must "
                              "return None and not write a second story.")
            # record 必须可 JSON 序列化 (它是下发块的数据源)
            round_tripped = json.loads(json.dumps(rec, ensure_ascii=False))
            self.assertEqual(round_tripped["id"], rec["id"],
                             "P3-A4 RED: the archive record is not JSON round-trippable "
                             "— 下发块无法由它直接装配。")

    # --- P3-A2 云端半边: story_list 下发 -----------------------------------
    def test_a2_list_total_is_full_set_not_page_slice(self):
        """coverage: P3-A2 云端 / decision contract-§一-story_list-total —
        total = 故事果全集计数; items 分页/截断时**不得**以 items.length 顶替。"""
        archive, err = _story_attr("archive_story")
        if archive is None:
            self.fail("P3-A2 RED: " + err)
        list_stories, err2 = _story_attr("list_stories")
        if list_stories is None:
            self.fail("P3-A2 RED: " + err2)

        with _TmpDir() as d:
            for i in range(5):
                archive(str(d), f"故事 {i} 的正文内容。", title=f"故事{i}",
                        slug=f"s{i}", date=f"2026-09-0{i + 1}")
            full = list_stories(str(d))
            self.assertEqual(
                full["view"], "story_list",
                f"P3-A2 RED: view must be story_list, got {full.get('view')!r}.")
            self.assertEqual(full["data"]["total"], 5,
                             f"P3-A2 RED: total must be the FULL archive count 5, got "
                             f"{full['data'].get('total')!r}.")
            self.assertEqual(len(full["data"]["items"]), 5,
                             "P3-A2 RED: un-paginated call must return all 5 items.")

            page = list_stories(str(d), limit=2, offset=0)
            self.assertEqual(len(page["data"]["items"]), 2,
                             "P3-A2 RED: limit=2 must slice the page to 2 items.")
            self.assertEqual(
                page["data"]["total"], 5,
                f"P3-A2 RED: a truncated page reported total={page['data'].get('total')!r} "
                "— 契约 §一 硬约束: total 是 stories 全集计数, items 分页/截断时不得以 "
                "items.length 顶替 (板子右上角 ×N 与盖章「第 N 颗」据此渲染)。")
            tail = list_stories(str(d), limit=2, offset=4)
            self.assertEqual(len(tail["data"]["items"]), 1,
                             "P3-A2 RED: offset=4 limit=2 must yield the 5th item only.")
            self.assertEqual(tail["data"]["total"], 5,
                             "P3-A2 RED: total must be stable across pages.")
            beyond = list_stories(str(d), offset=99)
            self.assertEqual(beyond["data"]["items"], [],
                             "P3-A2 RED: an out-of-range page must be empty, not all.")
            self.assertEqual(beyond["data"]["total"], 5,
                             "P3-A2 RED: total must survive an empty page too.")

            # 盖章同步不变式的云侧权威源: 新归档后 total 单调 +1
            archive(str(d), "第六个故事的正文。", title="故事6", slug="s6",
                    date="2026-09-09")
            after = list_stories(str(d))
            self.assertEqual(after["data"]["total"], 6,
                              "P3-A2 RED: total did not follow the newly archived story "
                              "— 设备盖章本地推 +1 后靠本字段刷新对齐 (闸 A 留记①)。")

    def test_a2_list_ordering_item_shape_and_junk_ignored(self):
        """coverage: P3-A2 — items 最新在前 (按完整日期) + item keys 恰为
        {id,title,date} + date 显示 "MM-DD" + 杂物文件不冒充故事果。"""
        archive, err = _story_attr("archive_story")
        if archive is None:
            self.fail("P3-A2 RED: " + err)
        list_stories, err2 = _story_attr("list_stories")
        if list_stories is None:
            self.fail("P3-A2 RED: " + err2)

        with _TmpDir() as d:
            archive(str(d), "旧的正文。", title="旧故事", slug="old", date="2026-08-20")
            archive(str(d), "新的正文。", title="新故事", slug="new", date="2026-09-05")
            # 杂物: 非故事记录文件 (README / .txt) 不得计入 total (孩子的徽章数不可被杂物灌水)
            (d / "README.md").write_text("# 这里放孩子录的故事\n", encoding="utf-8")
            (d / "notes.txt").write_text("杂物\n", encoding="utf-8")

            payload = list_stories(str(d))
            items = payload["data"]["items"]
            self.assertEqual(len(items), 2,
                             f"P3-A2 RED: junk files inflated the list: {items!r}")
            self.assertEqual(payload["data"]["total"], 2,
                             "P3-A2 RED: total must count 故事 records only.")
            self.assertEqual([i["title"] for i in items], ["新故事", "旧故事"],
                             f"P3-A2 RED: items must be newest-first, got {items!r} "
                             "(跨年比较按完整日期, 不按 MM-DD)。")
            for item in items:
                self.assertEqual(
                    set(item), {"id", "title", "date"},
                    f"P3-A2 RED: item keys {sorted(item)} != {{id,title,date}} — 契约 "
                    "§1.3 ③ 形状零增减 (不发明新字段, 闸 A 判 ④ 零契约改动)。")
                self.assertRegex(item["date"], r"^\d{2}-\d{2}$",
                                 f"P3-A2 RED: display date {item['date']!r} must be "
                                 "'MM-DD' per the contract example。")

            ds = _display_schema()
            ok, reason = ds.validate_display(payload)
            self.assertTrue(ok,
                            f"P3-A2 RED: the producer payload failed the REAL gateway "
                            f"schema gate ({reason}) — 下发块必须原样过机械校验。")

    def test_a2_list_empty_dir_is_zero_not_fake(self):
        """coverage: P3-A2 边界 — 还没录过故事时 total=0/items=[], 不是缺省、不是崩溃。"""
        list_stories, err = _story_attr("list_stories")
        if list_stories is None:
            self.fail("P3-A2 RED: " + err)
        with _TmpDir() as d:
            payload = list_stories(str(d))
            self.assertEqual(payload["view"], "story_list", "P3-A2 RED: wrong view.")
            self.assertEqual(payload["data"]["total"], 0,
                             "P3-A2 RED: an empty archive must read total=0 (零故事是"
                             "真实状态, 与「缺 total」不同 — 设备侧回退 items.length 亦 0)。")
            self.assertEqual(payload["data"]["items"], [],
                             "P3-A2 RED: empty archive must issue an empty items page.")
            ds = _display_schema()
            ok, reason = ds.validate_display(payload)
            self.assertTrue(ok, f"P3-A2 RED: empty story_list failed schema ({reason}).")

    # --- P3-A5 云端半边: story_text 下发 + P3-E4 ----------------------------
    def test_a5_story_text_shape_no_newword_field(self):
        """coverage: P3-A5 云端 (契约 §1.3 ④) + P3-E4 — story_text data 恰为
        {id,title,text}, 无生字标注字段; 过真实 display_schema 门。"""
        archive, err = _story_attr("archive_story")
        if archive is None:
            self.fail("P3-A5 RED: " + err)
        story_text_fn, err2 = _story_attr("story_text")
        if story_text_fn is None:
            self.fail("P3-A5 RED: " + err2)

        with _TmpDir() as d:
            rec = archive(str(d), "从前有一只小猫，它很勇敢。", title="勇敢的小猫",
                          slug="yonggan", date="2026-08-27")
            payload = story_text_fn(rec)
            self.assertEqual(
                payload,
                {"view": "story_text",
                 "data": {"id": "2026-08-27-yonggan", "title": "勇敢的小猫",
                          "text": "从前有一只小猫，它很勇敢。"}},
                f"P3-A5 RED: story_text payload not the contract §1.3 ④ form: {payload!r}")
            self.assertEqual(
                set(payload["data"]), {"id", "title", "text"},
                f"P3-A5 RED: story_text data keys {sorted(payload['data'])} — 不得出现"
                "生字标注/高亮字段 (P3-E4 2026-08-29 human 裁定: 需契约增生字字段、"
                "价值不抵复杂度, 已裁不做); 出现即契约面漂移。")
            self.assertNotIn("生字", json.dumps(payload, ensure_ascii=False),
                             "P3-A5 RED: a 生字标注 payload field leaked into story_text "
                             "(P3-E4 已裁)。")
            ds = _display_schema()
            ok, reason = ds.validate_display(payload)
            self.assertTrue(ok,
                            f"P3-A5 RED: story_text payload failed the REAL schema gate "
                            f"({reason}).")
            # 回看路径: 从磁盘记录直接下发 (归档 -> 回读 -> 下发 闭环)
            list_stories, err3 = _story_attr("list_stories")
            if list_stories is None:
                self.fail("P3-A5 RED: " + err3)
            page = list_stories(str(d))
            listed_id = page["data"]["items"][0]["id"]
            self.assertEqual(listed_id, rec["id"],
                             "P3-A5 RED: the list id must be usable to fetch the text "
                             "(回看/回听取正文的键闭环断裂)。")

    # --- V31 四条显式回退 ---------------------------------------------------
    def test_v31_cancel_discards_without_archiving(self):
        """coverage: V31 (KEY 长按取消) — 录音中取消 -> discard、不归档、回闲聊态。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("V31 RED: " + err)
        list_stories, err2 = _story_attr("list_stories")
        if list_stories is None:
            self.fail("V31 RED: " + err2)
        with _TmpDir() as d:
            acc = acc_cls(str(d))
            acc.start("s-cancel")
            acc.feed("s-cancel", "我讲一半不想讲了。")
            dropped = acc.cancel("s-cancel", reason="key-long-press")
            self.assertTrue(dropped,
                            "V31 RED: cancel must report that accumulated content was "
                            "discarded (有内容时返回真值)。")
            self.assertEqual(list_stories(str(d))["data"]["total"], 0,
                             "V31 RED: a cancelled recording was still archived — "
                             "「已累积内容丢弃」缺失, 孩子会凭空多一颗故事果 (S-F2.2 V31)。")
            self.assertEqual(list(d.glob("*.md")), [],
                             "V31 RED: cancel wrote a story file to disk.")
            self.assertEqual(acc.routing("s-cancel"), "chat",
                             "V31 RED: cancel must return the session to 闲聊态.")
            self.assertEqual(acc.recent_depth("s-cancel"), 0,
                             "V31 RED: cancelled 累积内容 still resident in the session "
                             "buffer — 「已累积内容丢弃」须同时释放缓冲 (内存 + 语义两面)。")

    def test_v31_wrong_take_returns_to_chat(self):
        """coverage: V31 (「讲错啦」回闲聊丢弃) — reason 携文案的 cancel 与按键取消同语义。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("V31 RED: " + err)
        with _TmpDir() as d:
            acc = acc_cls(str(d))
            acc.start("s-wrong")
            acc.feed("s-wrong", "从前有一只小猫，不对不对，我讲错啦。")
            acc.cancel("s-wrong", reason="讲错啦")
            self.assertEqual(acc.routing("s-wrong"), "chat",
                             "V31 RED:「讲错啦」后必须回闲聊态 (契约 §1.4/S-F2.2)。")
            self.assertIsNone(acc.stop("s-wrong"),
                              "V31 RED: after「讲错啦」丢弃, stop must not archive anything.")

    def test_v31_redo_within_window_undoes_archive(self):
        """coverage: V31 (停录 10 秒内「重录」) — 撤销上一归档 (真删文件) + 转写回累积态。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("V31 RED: " + err)
        list_stories, err2 = _story_attr("list_stories")
        if list_stories is None:
            self.fail("V31 RED: " + err2)
        with _TmpDir() as d:
            acc = acc_cls(str(d), redo_window_s=10.0)
            acc.start("s-redo", now=_ts("2026-09-08", 0, 0, 0))
            acc.feed("s-redo", "从前有只兔子。", now=_ts("2026-09-08", 0, 0, 1))
            acc.feed("s-redo", "兔子找朋友。", now=_ts("2026-09-08", 0, 0, 2))
            rec = acc.stop("s-redo", now=_ts("2026-09-08", 0, 0, 3), title="兔子找朋友",
                           slug="tuzi", date="2026-09-08")
            self.assertIsInstance(rec, dict, "V31 RED(setup): stop must archive.")
            self.assertEqual(list_stories(str(d))["data"]["total"], 1,
                             "V31 RED(setup): one story after stop.")
            undone = acc.redo("s-redo", now=_ts("2026-09-08", 0, 0, 9))
            self.assertIsInstance(undone, dict,
                                  "V31 RED:「重录」within the 10s window must undo the "
                                  "just-archived story (返回被撤销的记录)。")
            self.assertEqual(list_stories(str(d))["data"]["total"], 0,
                             "V31 RED: the undo did not remove the archived story from "
                             "the 全集计数 (「误收尾可撤销」缺失, S-F2.2 V31)。")
            self.assertFalse(Path(rec["path"]).exists(),
                             "V31 RED: the undone story file is still on disk.")
            self.assertEqual(acc.routing("s-redo"), "accumulate",
                             "V31 RED: after「重录」the session must be back in 累积态 "
                             "(孩子重新说一遍即可, 不必再走 start_recording)。")
            acc.feed("s-redo", "兔子又找朋友。", now=_ts("2026-09-08", 0, 0, 20))
            rec2 = acc.stop("s-redo", now=_ts("2026-09-08", 0, 0, 21), title="兔子找朋友",
                            slug="tuzi", date="2026-09-08")
            self.assertIsInstance(rec2, dict,
                                  "V31 RED: the re-recording must archive on the next stop.")
            self.assertEqual(list_stories(str(d))["data"]["total"], 1,
                             "V31 RED: redo+re-stop must leave exactly one story.")

    def test_v31_redo_after_window_keeps_archive(self):
        """coverage: V31 边界 — 超出 10 秒窗口的「重录」不得撤销已入册的故事。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("V31 RED: " + err)
        list_stories, err2 = _story_attr("list_stories")
        if list_stories is None:
            self.fail("V31 RED: " + err2)
        with _TmpDir() as d:
            acc = acc_cls(str(d), redo_window_s=10.0)
            acc.start("s-late", now=_ts("2026-09-08", 0, 0, 0))
            acc.feed("s-late", "一个讲完了的故事。", now=_ts("2026-09-08", 0, 0, 1))
            rec = acc.stop("s-late", now=_ts("2026-09-08", 0, 0, 2), title="讲完了",
                           slug="done", date="2026-09-08")
            self.assertIsInstance(rec, dict, "V31 RED(setup): stop must archive.")
            late = acc.redo("s-late", now=_ts("2026-09-08", 0, 0, 60))
            self.assertIsNone(late,
                              "V31 RED: redo 60s after stop must be refused — 撤销窗口"
                              "严格 10 秒 (S-F2.2 V31), 否则孩子已入册的故事可被抹掉。")
            self.assertEqual(list_stories(str(d))["data"]["total"], 1,
                             "V31 RED: an out-of-window redo still removed the archive.")
            self.assertTrue(Path(rec["path"]).is_file(),
                            "V31 RED: the archived file was deleted out of window.")
            self.assertIsNone(acc.redo("s-never-recording", now=_ts("2026-09-08", 0, 1, 0)),
                              "V31 RED: redo with no previous archive must be None.")

    def test_v31_recover_rescues_missed_story(self):
        """coverage: V31 (漏判「刚才那段是故事」挽回) — 网关保留近 N 句转写并挽回归档;
        挽回一次即清空, 不重复入册。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("V31 RED: " + err)
        list_stories, err2 = _story_attr("list_stories")
        if list_stories is None:
            self.fail("V31 RED: " + err2)
        with _TmpDir() as d:
            acc = acc_cls(str(d))
            # 孩子真讲了一段, 但被当成闲聊 (routing 全程 chat)
            for line in ("从前有一座彩虹桥，", "桥下面住着一只小乌龟，", "乌龟飞上了天。"):
                acc.feed("s-miss", line)
            self.assertEqual(acc.routing("s-miss"), "chat",
                             "V31 RED(setup): a never-started session is 闲聊态.")
            self.assertGreaterEqual(acc.recent_depth("s-miss"), 3,
                                    "V31 RED: 网关未保留近 N 句转写 — 漏判无从挽回 "
                                    "(S-F2.2 V31「网关保留近 N 句转写」)。")
            rec = acc.recover("s-miss", title="彩虹桥", slug="caihongqiao",
                              date="2026-09-08")
            self.assertIsInstance(rec, dict,
                                  "V31 RED:「刚才那段是故事」未挽回归档 (漏判兜底缺失)。")
            self.assertIn("小乌龟", rec["text"],
                          "P3-A4 RED: the recovered story lost the retained sentences.")
            self.assertEqual(list_stories(str(d))["data"]["total"], 1,
                             "V31 RED: a recovered story is not counted in the 全集.")
            again = acc.recover("s-miss", title="彩虹桥", slug="caihongqiao2",
                                date="2026-09-08")
            self.assertIsNone(again,
                              "V31 RED: the same ring rescued twice — recover must clear "
                              "the retained buffer (一次挽回只入册一次)。")
            self.assertEqual(list_stories(str(d))["data"]["total"], 1,
                             "V31 RED: repeated recover inflated the 故事果 count.")
            self.assertIsNone(acc.recover("s-empty"),
                              "V31 RED: recovering from an empty ring must return None, "
                              "not fabricate a story.")

    def test_v31_cancelled_content_not_recoverable(self):
        """coverage: V31 关键区分 — 取消(discard) 的内容不得经「刚才那段是故事」捞回来。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("V31 RED: " + err)
        list_stories, err2 = _story_attr("list_stories")
        if list_stories is None:
            self.fail("V31 RED: " + err2)
        with _TmpDir() as d:
            acc = acc_cls(str(d))
            acc.start("s-c")
            acc.feed("s-c", "这段是孩子主动取消的内容。")
            acc.cancel("s-c", reason="key-long-press")
            rec = acc.recover("s-c", date="2026-09-08")
            self.assertIsNone(rec,
                              "V31 RED: cancelled content was recovered —「已累积内容"
                              "丢弃」与「漏判挽回」两条路径必须可区分 (丢弃即丢弃)。")
            self.assertEqual(list_stories(str(d))["data"]["total"], 0,
                             "V31 RED: discarded recording reached the archive anyway.")

    # --- 缓冲有界 (notes.blockingDecision_memoryPressure 硬约束) -------------
    def test_buffer_bounded_per_session_ring(self):
        """coverage: notes 硬约束 — 近 N 句缓冲**有界** (句数与字符双上限, FIFO 淘汰);
        防 char-stroke §8.3 内存叠压先例。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("boundary RED: " + err)
        with _TmpDir() as d:
            window = 5
            acc = acc_cls(str(d), sentence_window=window, char_window=60)
            for i in range(200):
                acc.feed("s-b", f"第{i}句转写内容。")
            self.assertLessEqual(acc.recent_depth("s-b"), window,
                                 f"boundary RED: recent ring grew to "
                                 f"{acc.recent_depth('s-b')} sentences past the cap "
                                 f"{window} — 「缓冲上限 N」不变式被破坏。")
            self.assertLessEqual(acc.recent_chars("s-b"), 60,
                                 "boundary RED: the recent ring exceeded its char cap — "
                                 "仅按句数封顶不足以约束内存 (KB 级文本缓冲须双上限)。")
            # FIFO 方向必须是「淘汰最旧、保留最新」—— 用行为面表达 (recover 归档的
            # 就是环内保留内容), 不新增可选符号、不给实现留「少实现一个方法也算过」的空门
            rec = acc.recover("s-b", date="2026-09-08")
            self.assertIsInstance(rec, dict,
                                  "boundary RED(setup): recover must archive the retained "
                                  "recent ring so its contents are observable.")
            self.assertIn("第199句", rec["text"],
                          "boundary RED: FIFO dropped the NEWEST sentences instead of the "
                          "oldest — 「刚才那段是故事」挽回必须能拿到最后几句。")
            self.assertNotIn("第0句转写内容", rec["text"],
                             "boundary RED: the ring kept the oldest sentences past the "
                             "cap (有界不变式只在数量上成立、内容方向反了)。")

    def test_buffer_bounded_across_sessions(self):
        """coverage: notes 硬约束 — 会话面也有界 (max_sessions + LRU), 且**仅活动会话**
        持缓冲; 会话结束/取消后不残留。"""
        acc_cls, err = _story_attr("StoryAccumulator")
        if acc_cls is None:
            self.fail("boundary RED: " + err)
        with _TmpDir() as d:
            max_sessions = 3
            acc = acc_cls(str(d), sentence_window=4, char_window=200,
                          max_sessions=max_sessions)
            for i in range(20):
                acc.feed(f"s-{i}", f"会话{i}的一句。")
            self.assertLessEqual(len(acc.active_sessions()), max_sessions,
                                 f"boundary RED: the gateway retains "
                                 f"{len(acc.active_sessions())} sessions past the "
                                 f"max_sessions cap {max_sessions} — 常驻会话表无界增长"
                                 "即 char-stroke §8.3 内存叠压同型缺陷。")
            self.assertLessEqual(acc.total_buffer_bytes(), max_sessions * 200,
                                 "boundary RED: total buffered text exceeds the frozen "
                                 "global bound (max_sessions × char_window)。")
            # 取消/停录后不留活动态 (仅活动会话持缓冲)
            acc.start("s-end")
            acc.feed("s-end", "要取消的一段。")
            acc.cancel("s-end")
            self.assertNotIn("s-end", acc.active_sessions(),
                             "boundary RED: a cancelled session still holds gateway state "
                             "— 「仅活动会话」不变式被破坏 (缓冲须释放)。")

    # --- prompt 载体 (notes.scopePromptV3: 验收所测 = 生产所用) --------------
    def test_prompt_carrier_v3_landed_and_wired(self):
        """coverage: P3-A4/P3-A1 载体 — 故事 producer 意图 + 停录起标题 + V31 回退判断
        的 system prompt 增量按版本化惯例以**新文件**落盘, 且 overlay 渲染面与声明面
        都切到该版本 (内容不锁, 锁的是「生产用的是被测的这一块」)。"""
        self.assertTrue(OVERLAY_WIRING.is_file(),
                        "P3-A4 RED(infra): server/deploy/apply-xiaozhi-overlay.sh missing — "
                        "无法核 prompt 渲染接线。")
        overlay = OVERLAY_WIRING.read_text(encoding="utf-8", errors="replace")
        match = re.search(r"PROMPT_SRC\s*=.*openclaw-system-prompt\.v(\d+)\.md", overlay)
        self.assertIsNotNone(
            match,
            "P3-A4 RED: the overlay render wiring has no PROMPT_SRC pointing at a "
            "versioned openclaw-system-prompt.vN.md — 渲染面接线不可辨, 无法证明"
            "「验收所测=生产所用」。")
        version = int(match.group(1))
        self.assertGreaterEqual(
            version, 3,
            f"P3-A4 RED: the wired prompt carrier is v{version}; 本包要求故事 producer "
            "意图/归档/V31 回退判断的版本化增量落在 v3 或更高新文件 (可 diff 历史, "
            "v1/v2 不改)。")
        carrier = DEPLOY_DIR / f"openclaw-system-prompt.v{version}.md"
        self.assertTrue(carrier.is_file(),
                        f"P3-A4 RED: wired carrier {carrier.name} is not on disk.")
        body = carrier.read_text(encoding="utf-8", errors="replace")
        self.assertTrue(body.strip(), f"P3-A4 RED: {carrier.name} is empty.")
        self.assertTrue(("我要讲故事" in body) or ("start_recording" in body),
                        f"P3-A4 RED: {carrier.name} carries no 故事录音意图指令 "
                        "(「我要讲故事」→ control/start_recording) — producer 侧意图"
                        "判定的机器承载缺失。")
        self.assertTrue(any(m in body for m in ("重录", "讲错啦", "刚才那段是故事", "回退")),
                        f"P3-A4 RED: {carrier.name} carries no V31 回退判断指令 "
                        "(取消/重录/漏判挽回/讲错啦 四条), 误判兜底无承载。")
        self.assertTrue(any(m in body for m in ("归档", "起标题", "story_list", "stories")),
                        f"P3-A4 RED: {carrier.name} carries no 停录整段归档/起标题 "
                        "instruction — OpenClaw 一回合只起标题、不读写文件的分工"
                        "未写进生产 prompt。")
        declare = PROVIDER_DECLARE.read_text(encoding="utf-8", errors="replace")
        self.assertIn(
            carrier.name, declare,
            f"P3-A4 RED: the declared面 (provider.config.yaml) still points elsewhere — "
            "渲染/声明两面须同时切到 {0} (schedule-p2 教训: 扫全 corpus 会被前瞻"
            "注记打出假绿, 故只认这两面)。".format(carrier.name))

    # --- P3-E3 / 契约零改动红线 (boundary) ---------------------------------
    def test_boundary_contract_surface_untouched(self):
        """coverage: P3-E3 + 闸 A ④ 零契约改动 — view 白名单/大小上限不回退,
        S-F2.5 润色不进云侧必做面, 归档路径不进网关契约面。"""
        ds = _display_schema()
        self.assertEqual(
            ds.VALID_VIEWS,
            {"char", "schedule", "story_list", "story_text", "text", "home", "control"},
            f"boundary RED: the view whitelist drifted: {ds.VALID_VIEWS!r} — 本包"
            "「零契约改动」红线 (不发明新 view; 若确需新 view 属 ① 契约触点, "
            "回 planner 走契约门)。")
        self.assertEqual(
            ds.MAX_DATA_BYTES, 65536,
            "boundary RED: MAX_DATA_BYTES drifted — V27 机械校验冻结面被动。")
        self.assertIn("start_recording", ds.FORWARDABLE_CONTROL_ACTIONS,
                      "boundary RED: control/start_recording no longer forwardable — "
                      "契约 §1.4 冻结动作面被破坏 (包 8 录音入口)。")
        self.assertIn("end_session", ds.GATEWAY_DECISION_CONTROL_ACTIONS,
                      "boundary RED: end_session 不再是网关决定项 — §1.4 伪造拦截"
                      "语义被动 (与本包无关的既有红线)。")
        # P3-E3: 润色为可选后置 — 云侧故事模块不得把润色当必做面写死 (仅约束本模块)
        mod_text = ""
        if STORY_MODULE.is_file():
            mod_text = STORY_MODULE.read_text(encoding="utf-8", errors="replace")
        self.assertNotIn(
            "polish_required", mod_text,
            "P3-E3 RED: story.py hard-codes a 润色 (polish) required path — S-F2.5 "
            "云端润色为可选项、MVP 后置, 不进本包必做验收 (PROPOSAL §7 留根)。")
        hits = _code_files_referring(("润色",))
        self.assertEqual(hits, set(),
                         f"P3-E3 RED: server code implements 润色 in this package: "
                         f"{sorted(hits)} — 后置项不得混入必做面。")

    # --- P3-E2: 既有冻结验收回归守卫 ---------------------------------------
    def test_e2_existing_directionb_acceptance_green(self):
        """coverage: P3-E2 — 既有冻结验收回归守卫: gateway-directionB offline 套件
        独立复跑 exit 0 (本包新文件不得破坏并行验收面)。守卫目标选 directionB
        (schedule-p2 同款): voice_roundtrip_b 套件的 VRT-A2 证据齿对 runner cwd 敏感
        (其 helper 以 repo 相对路径 stat), 与本包无关, 不选它作守卫以免恒红。"""
        target = SERVER / "acceptance" / "test_gateway_directionB.acceptance.py"
        proc = subprocess.run(
            [sys.executable, str(target)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=600, cwd=str(SERVER))
        self.assertEqual(
            proc.returncode, 0,
            "P3-E2 RED: the existing 260903-gateway-directionB acceptance regressed.\n"
            f"stdout tail: {proc.stdout[-1500:]!r}\nstderr tail: {proc.stderr[-800:]!r}")


# --------------------------------------------------------------------------- #
# P3-A4/A6 live face — 真机/真网关跑通一轮的证据（不 auto-run, ARQ-260901-g3）   #
# --------------------------------------------------------------------------- #

class StoryP3LiveAcceptance(unittest.TestCase):
    """live evidence teeth: the real round trip (真说「我要讲故事」→ 真采音上行 →
    停录 → 云端整段转写归档 → 列表/盖章真出现) cannot be scripted here. 证据落
    changes/dev/260829-story-p3/ (operator-produced)。
    probe ≠ exercise 纪律: 必须真走一轮端到端并记录事实, 只探测网关可达不算 (DEV-GOVERNANCE
    §2)。麦克风采音本身是硬件强绑定子能力 — P3-E2 显式殿后包 9b; 本齿在**本包台架相位
    或 9b 相位**执行并核销, 不参与冻结红基线计数 (机械闸 skip)。"""

    maxDiff = None

    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _evidence_corpus(self):
        if not LIVE_EVIDENCE_DIR.is_dir():
            return None
        parts = []
        for p in sorted(LIVE_EVIDENCE_DIR.rglob("*")):
            if p.is_file() and p.suffix in (".md", ".log", ".txt", ".json"):
                try:
                    parts.append(p.read_text(encoding="utf-8", errors="replace"))
                except OSError:
                    continue
        return "\n".join(parts) if parts else None

    def test_live_story_roundtrip_evidence(self):
        """coverage: P3-A4/A6 live — 四事实: 意图→start_recording 下发 / 真采音整段
        转写归档 stories/*.md / 列表回读含新故事 / 盖章帧真出现。"""
        corpus = self._evidence_corpus()
        self.assertTrue(
            corpus,
            "P3-A4 live RED: no live evidence 落盘 under changes/dev/260829-story-p3/ — "
            "真录一轮（说「我要讲故事」→录→停→归档→回看/盖章）的证据必须先落盘, "
            "四事实齿据此核销 (probe ≠ exercise; 采音殿后 9b 时由 9b 相位执行并回填)。")
        fact_intent = ("我要讲故事", "start_recording")
        self.assertTrue(any(m in corpus for m in fact_intent),
                        "P3-A4 live RED: evidence does not record the intent -> "
                        "control/start_recording fact (① 意图下发事实缺失).")
        fact_asr = ("转写", "ASR", "asr", "stt")
        self.assertTrue(any(m in corpus for m in fact_asr),
                        "P3-A4 live RED: evidence does not record the 整段转写 fact "
                        "(② 停录后云端转写事实缺失).")
        fact_archive = ("stories", ".md", "归档")
        self.assertTrue(all(m in corpus for m in fact_archive),
                        "P3-A4 live RED: evidence does not record the stories/*.md "
                        "归档落盘 fact (③ 归档事实缺失).")
        fact_stamp = ("徽章", "盖章", "story_list")
        self.assertTrue(any(m in corpus for m in fact_stamp),
                        "P3-A6 live RED: evidence does not record the 列表回读/盖章帧 "
                        "fact (④ N 同步与盖章演出事实缺失).")
        for marker in ("归档失败", "未收到", "识别为空", "列表不更新", "无徽章"):
            self.assertNotIn(
                marker, corpus,
                f"P3-A4 live RED: evidence records a FAILURE outcome ({marker!r}) — "
                "录制→归档→回看链路存在真实缺陷, 不得按通过交付.")


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(StoryP3OfflineAcceptance)
    if os.environ.get("KSP3_RUN_LIVE") == "1" or os.environ.get("RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(StoryP3LiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)
