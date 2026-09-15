"""Frozen acceptance tests for TaskPackage 260829-hardware-validation (subsystem: server).

Scope of record (taskpackage + human 裁定「扩界接线」+「承接」live 五齿):
F2 故事生产接线（human-corrected 扩界）：云侧停录归档触发（StoryAccumulator.stop/feed
经网关切累积态、archive_story 真落盘 stories/*.md）、story_text 组块下发（网关
story_text(record) 分块下发正文——F4 分工修正的网关半）、F6 会话键（front_app.py
弃 id(websocket) 地址复用、改不可复用键 + WS 断开钩子 pop 会话）、F4 prompt 分工修正
（模型一回合只出 title、正文由网关 story_text(record) 下发——删「回填 text/id 系统
给定」）。v0.4 新增三要素（实机续航基准三要素 / 常驻 WS 连接功耗观察 / 证书真实设备
握手冒烟）+ ECS 部署承接（包 7 live 五齿 L1-L5 的真机轮执行窗口）以 @pytest.mark.live
落 evidence 齿，不 auto-run。

语义权威：gateway/story.py 的 frozen offline contract（archive_story / list_stories /
story_text / StoryAccumulator{start,feed,stop,cancel,redo,recover,routing}）已由
260829-story-p3 的 server/acceptance/test_story_p3.acceptance.py 冻结；本文件**只钉
生产接线**，不重造契约、不改 story.py（回归守卫钉 story.py 契约面不被本包改坏）。

FROZEN wiring facts (offline, needle-level on the production source):
  1) front_app.py 必须在生产 ws proxy 里驱动 feed/stop（累积 + 停录归档）——
     现只有 .start(（_story_route_display_payload 的 control/start_recording）;
     feed/stop/routing/recover/redo 全是零生产调用者（story-p3 §9 F7 留根）。
  2) 网关生产面必须消费 story_text(（组块下发正文，设备侧 story_text 消费由
     story_p3 冻结）——现零生产调用者。
  3) front_app.py 的会话键必须弃 id(websocket)（CPython 对象地址, 连接关闭可复用）
     + WS 断开钩子释放累积会话（StoryAccumulator.cancel 或新 drop 原语）。
  4) 接线中的 system prompt（PROMPT_SRC 指向的版本化 carrier）不得再让模型
     「回填 text / id 系统给定」——模型一回合只出 title、正文由网关 story_text
     组块下发。v3 现含缺陷行「只需回填 title + text（正文逐字来自…）」-> RED。

Live faces (@pytest.mark.live, NOT auto-run — conftest 机械 skip 闸, ARQ-260901-g3):
真机轮的落盘证据断言，证据下 delivery/changes/dev/260829-hardware-validation/（本包专属）：
  - story 录制→归档→盖章端到端（archive_story 真落盘 + story_text 组块下发 + 设备
    盖章/保存成功回执）——probe ≠ exercise：必须真跑一轮真实端到端路径。
  - 证书真实设备握手冒烟（CA pinning 真机首连, spec 决策 22 / 契约 §四 wss）。
  - 实机续航基准（决策 21 DEFER 三要素：触发点在 9b、预警阈值、owner）+ 常驻 WS
    连接功耗观察。
  - ECS 部署真落下（DEPLOY-RUNBOOK §8）+ 包 7 留根 live 五齿 L1-L5（真 cron/真推送/
    真回执/真语义产物一轮）evidence 交班（真机轮执行）。

Coverage map (新增 acceptance ID 建议 / sourceRef -> test method):
  HWV-A10 (F2 云侧停录归档生产接线; sourceRefs=story-p3 §9 F7 + intake 裁定扩展)
        -> test_f2_cloud_stop_feed_production_wired
  HWV-A12 (F4 分工修正: story_text 组块下发 + prompt 只出 title; sourceRefs=story-p3 §9 F4)
        -> test_f2_story_text_chunk_production_wired
        -> test_f4_wired_prompt_no_backfill_and_story_division
  HWV-A11 (F6 会话键不可复用 + 断开 pop; sourceRefs=story-p3 §9 F6)
        -> test_f6_session_key_non_reusable_and_disconnect_teardown
  HWV-A10/A12 live -> test_live_story_wiring_evidence (live)
  HWV-A15 (证书真实设备握手冒烟; v0.4) -> test_live_cert_handshake_smoke_evidence (live)
  HWV-A13/A14 (续航基准三要素 + 常驻 WS 功耗观察; v0.4) -> test_live_battery_baseline_evidence (live)
  HWV-A16 (ECS 部署 + 包 7 live 五齿承接) -> test_live_ecs_deploy_and_reminder_five_evidence (live)
  boundary/regression -> test_story_module_contract_surface_unchanged (story.py 契约面不破)

Red baseline (freeze state, 2026-09-09 acceptance-author 实测):
front_app.py 只消费 .start(（control/start_recording），feed/stop/routing/recover/redo
零生产调用者；story_text 零生产调用者；session_id 仍是 f"conn-{id(websocket)}"；接线
prompt v3 仍含「回填」缺陷行——所有离线 wiring 齿红在真实缺失接线（守卫齿绿）。收集
零错误（§3 冻结即冒烟；本档可收集、红在接线缺失不在语法）。device 面 red-by-construction
（真机轮未跑, human 持板执行, 不伪造已测证据）。

Run protocol:
  cd server && python -m pytest -q --import-mode=importlib acceptance/test_story_wiring.acceptance.py
  （live 类被 conftest 默认 skip; RUN_LIVE=1 或 -m live 显式点名才跑）
  standalone: python acceptance/test_story_wiring.acceptance.py
"""

from __future__ import annotations

import os
import re
import sys
import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
DEPLOY_DIR = SERVER / "deploy"

FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
STORY_MODULE = GATEWAY_DIR / "story.py"
OVERLAY_WIRING = DEPLOY_DIR / "apply-xiaozhi-overlay.sh"
PROVIDER_DECLARE = DEPLOY_DIR / "provider.config.yaml"

# 本包专属真机轮证据目录（live/device evidence; human + integrator 落盘）。
LIVE_EVIDENCE_DIR = REPO_ROOT / "delivery" / "changes" / "dev" / "260829-hardware-validation"
# 包 7 留根 live 五齿的既有证据目录（9b 承接 ECS 部署窗口后真跑的那一轮）。
REMINDER_LIVE_EVIDENCE_DIR = REPO_ROOT / "delivery" / "changes" / "dev" / "260829-schedule-reminder"


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _gateway_production_corpus(exclude=frozenset({"story.py"})):
    """server/gateway/*.py 生产源码并集（不含 acceptance；story.py 单独排除——
    它是被消费的冻结尾模块，不是「生产调用者」判定面）。"""
    if not GATEWAY_DIR.is_dir():
        return ""
    parts = []
    for p in sorted(GATEWAY_DIR.rglob("*.py")):
        rel = p.relative_to(GATEWAY_DIR)
        if any(part in exclude for part in rel.parts):
            continue
        parts.append(_read_text(p))
    return "\n".join(parts)


def _wired_prompt_carrier() -> tuple[Path | None, str]:
    """解析当前真正被 overlay 渲染面接线的那一版 prompt（版本无关, story_p3 同款
    PROMPT_SRC regex）。返回 (carrier_path|None, reason)。"""
    overlay = _read_text(OVERLAY_WIRING)
    if not overlay.strip():
        return None, f"{OVERLAY_WIRING.name} missing/empty — 无法核 prompt 渲染接线"
    m = re.search(r"PROMPT_SRC\s*=.*openclaw-system-prompt\.v(\d+)\.md", overlay)
    if not m:
        return None, ("overlay 无 PROMPT_SRC=openclaw-system-prompt.vN.md — 渲染面"
                      "接线不可辨, 「验收所测=生产所用」无法证明")
    carrier = DEPLOY_DIR / f"openclaw-system-prompt.v{int(m.group(1))}.md"
    if not carrier.is_file():
        return None, f"被接线的 {carrier.name} 未落盘（孤儿声明）"
    body = _read_text(carrier)
    if not body.strip():
        return None, f"被接线的 {carrier.name} 是空文件"
    return carrier, body


class StoryWiringOfflineAcceptance(unittest.TestCase):
    """离线 wiring 齿：F2 停录归档生产接线 / story_text 组块 / F6 会话键 / F4 prompt
    分工。红在真实缺失接线（缺生产调用者/缺陷字面仍被执），不在 import/collection。"""

    maxDiff = None

    # --- HWV-A10: F2 云侧停录归档生产接线 --------------------------------------
    def test_f2_cloud_stop_feed_production_wired(self):
        """coverage: HWV-A10 / story-p3 §9 F7 — front_app.py 的真实 ws proxy 必须驱动
        StoryAccumulator.feed（累积转写）与 .stop（停录整段归档→archive_story 落盘），
        且网关生产面（story.py 之外）有 archive_story 的到达面（直接调用或经 .stop）。
        「只 import / 只 start」不算接线。"""
        self.assertTrue(FRONT_APP.is_file(), "HWV-A10 RED(infra): front_app.py missing")
        front = _read_text(FRONT_APP)
        self.assertIn(
            ".feed(", front,
            "HWV-A10 RED: front_app.py 从不调用 StoryAccumulator.feed( —— 录音的逐句转写"
            "没有喂给累积缓冲, 整段归档无内容可落盘（story-p3 §9 F7 零生产调用者接线缺口）")
        self.assertIn(
            ".stop(", front,
            "HWV-A10 RED: front_app.py 从不调用 StoryAccumulator.stop( —— 停录整段转写"
            "归档链路没有生产触发点, archive_story 永不落盘 stories/*.md（F7 缺口）")
        gw = _gateway_production_corpus(exclude=frozenset({"story.py"}))
        self.assertTrue(
            (".stop(" in gw) or ("archive_story(" in gw),
            "HWV-A10 RED: server/gateway 生产面（story.py 之外）没有任何 archive_story/"
            "stop 到达面 —— 归档的 production caller 不存在（非 import/构造旁路）")

    # --- HWV-A12 (网关半): story_text 组块下发 ---------------------------------
    def test_f2_story_text_chunk_production_wired(self):
        """coverage: HWV-A12 / story-p3 §9 F4 — 网关生产面必须消费 story_text(（把归档
        正文组块下发 device；设备侧 story_text 消费由 story_p3 冻结）。现零生产调用者。"""
        gw = _gateway_production_corpus(exclude=frozenset({"story.py"}))
        self.assertIn(
            "story_text(", gw,
            "HWV-A12 RED: server/gateway 生产面没有任何 story_text( 调用 —— F4 分工"
            "「正文由网关 story_text(record) 组块下发」的网关半不在世, 模型一回合只出"
            "title 后正文无人下发给设备")

    # --- HWV-A11: F6 会话键不可复用 + 断开 pop --------------------------------
    def test_f6_session_key_non_reusable_and_disconnect_teardown(self):
        """coverage: HWV-A11 / story-p3 §9 F6 — front_app.py 弃 id(websocket)（CPython
        对象地址, 连接关闭即复用 -> 跨会话累积串味）, 改不可复用键；WS 断开钩子释放
        累积会话（StoryAccumulator.cancel 或新 drop/discard 原语 pop 会话）。"""
        self.assertTrue(FRONT_APP.is_file(), "HWV-A11 RED(infra): front_app.py missing")
        front = _read_text(FRONT_APP)
        self.assertNotIn(
            "id(websocket)", front,
            "HWV-A11 RED: front_app.py 仍以 id(websocket)（CPython 对象地址）作会话键"
            "—— 连接关闭地址可复用, StoryAccumulator 会话跨连接串味（F6 缺陷字面仍在）")
        self.assertTrue(
            re.search(r"\.(cancel|drop|discard)\(", front) is not None,
            "HWV-A11 RED: front_app.py 无 WS 断开钩子释放累积会话（.cancel(/.drop(/.discard("
            "）—— 累积缓冲不 pop, 常驻连接断开后会话态泄漏（F6 half 缺口）")

    # --- HWV-A12 (prompt 半): F4 分工修正 ------------------------------------
    def test_f4_wired_prompt_no_backfill_and_story_division(self):
        """coverage: HWV-A12 / story-p3 §9 F4 — 接线中的 prompt 不得再让模型「回填
        text / id 系统给定」（模型一回合只出 title）, 且 story_text 分工仍在。版本无关
        （PROMPT_SRC 解析当接线版）；v3 现含缺陷行「只需回填 title + text…」-> RED。"""
        carrier, body = _wired_prompt_carrier()
        if carrier is None:
            self.fail("HWV-A12 RED: " + body)
        self.assertNotIn(
            "回填", body,
            "HWV-A12 RED: 接线 prompt 仍含「回填」——F4 缺陷（模型逐字复述整段转写并"
            "回填拿不到的 id）未修；应删「回填 text / id 系统给定」、模型一回合只出"
            "title、正文由网关 story_text(record) 组块下发")
        self.assertTrue(
            ("story_text" in body) and ("title" in body),
            "HWV-A12 RED: 接线 prompt 丢 story_text/title 分工标记 —— 停录起标题 + "
            "story_text 下发正文的分工约定不可辨")

    # --- regression：story.py 契约面不被本包改坏 ------------------------------
    def test_story_module_contract_surface_unchanged(self):
        """coverage: regression — gateway/story.py 是 story_p3 冻结的离线契约模块;
        本包只钉接线、不改契约。守卫 archive_story/list_stories/story_text/
        StoryAccumulator 契约面在世（若 builder 越界改 story.py 会在此红）。"""
        self.assertTrue(STORY_MODULE.is_file(), "HWV RED(infra): gateway/story.py missing")
        story = _read_text(STORY_MODULE)
        for surface, what in (
            ("def archive_story(", "归档落盘契约面"),
            ("def list_stories(", "story_list 下发契约面"),
            ("def story_text(", "story_text 下发契约面"),
            ("class StoryAccumulator", "manual 聆听累积态契约面"),
        ):
            self.assertIn(surface, story, f"HWV RED: gateway/story.py 丢失 {surface} — "
                                          f"{what} 被本包/兄弟包改坏（story_p3 冻结契约面）")


class StoryWiringLiveAcceptance(unittest.TestCase):
    """live teeth: 真机轮落盘证据断言（@pytest.mark.live, 不 auto-run）。probe !=
    exercise —— 必须真跑一轮 real path（真录一段故事→真归档→真组块下发→真盖章；
    真机证书首连握手；真续航/功耗观测；真 ECS 部署 + 真 cron 提醒关闭）并落 evidence
    下 delivery/changes/dev/260829-hardware-validation/（及交班既有 260829-schedule-reminder/）。

    device 面 red-by-construction（真机轮未跑, human 持板执行）——本类只断言 evidence
    「在案且真跑了」, 不伪造已测绿。"""

    maxDiff = None
    if _HAS_PYTEST:
        pytestmark = pytest.mark.live

    def _corpus(self, evidence_dir: Path):
        if not evidence_dir.is_dir():
            return None
        parts = []
        for p in sorted(evidence_dir.rglob("*")):
            if p.is_file() and p.suffix in (".md", ".log", ".txt", ".json"):
                parts.append(_read_text(p))
        return "\n".join(parts) if parts else None

    def _require(self, evidence_dir: Path, what: str) -> str:
        corpus = self._corpus(evidence_dir)
        self.assertTrue(
            corpus,
            f"live RED: {evidence_dir.relative_to(REPO_ROOT)} 下无落盘证据 — {what} 必须"
            "真跑一轮并留证（probe ≠ exercise, DEV-GOVERNANCE §2）")
        return corpus

    def test_live_story_wiring_evidence(self):
        """coverage: HWV-A10/A12 live — 录制→归档→盖章端到端真跑证据。"""
        corpus = self._require(LIVE_EVIDENCE_DIR, "F2 故事录制→归档→盖章端到端真机轮")
        self.assertTrue(("archive" in corpus) or ("归档" in corpus) or ("stories/" in corpus),
                        "live RED: 证据未摘录 archive_story 真落盘 stories/*.md 事实")
        self.assertTrue(("story_text" in corpus) or ("正文" in corpus),
                        "live RED: 证据未摘录 story_text 组块下发正文事实")
        self.assertTrue(("盖章" in corpus) or ("stamp" in corpus) or ("保存成功" in corpus),
                        "live RED: 证据未摘录设备侧盖章/保存成功回执事实")

    def test_live_cert_handshake_smoke_evidence(self):
        """coverage: HWV-A15 live — 证书真实设备握手冒烟（CA pinning 真机首连）。"""
        corpus = self._require(LIVE_EVIDENCE_DIR, "证书真实设备握手冒烟（CA pinning 首连）")
        self.assertTrue(("证书" in corpus) or ("cert" in corpus.lower()) or ("CA" in corpus),
                        "live RED: 证据未摘录真实设备 CA pinning 证书握手事实（spec 决策 22）")
        self.assertTrue(("握手" in corpus) or ("handshake" in corpus.lower()) or ("wss" in corpus.lower())
                        or ("首连" in corpus),
                        "live RED: 证据未摘录真机首次 wss 握手成立事实")

    def test_live_battery_baseline_evidence(self):
        """coverage: HWV-A13/A14 live — 实机续航基准（决策 21 三要素）+ 常驻 WS 功耗观察。"""
        corpus = self._require(LIVE_EVIDENCE_DIR, "实机续航基准 + 常驻 WS 功耗观测")
        self.assertTrue(("续航" in corpus) or ("battery" in corpus.lower()) or ("续航基准" in corpus),
                        "live RED: 证据未摘录实机续航基准实测数据（决策 21 三要素承载）")
        self.assertTrue(("常驻" in corpus) or ("功耗" in corpus) or ("WS" in corpus),
                        "live RED: 证据未摘录常驻 WS 连接功耗观察数据")

    def test_live_ecs_deploy_and_reminder_five_evidence(self):
        """coverage: HWV-A16 live — ECS 部署真落（DEPLOY-RUNBOOK §8）+ 包 7 留根 live
        五齿 L1-L5 真跑一轮的证据交班。"""
        corpus = self._require(LIVE_EVIDENCE_DIR, "ECS 部署 + 包 7 live 五齿承接")
        self.assertTrue(("DEPLOY-RUNBOOK" in corpus) or ("RUNBOOK" in corpus) or ("部署" in corpus)
                        or ("cron" in corpus),
                        "live RED: 证据未摘录 ECS 部署真落事实（DEPLOY-RUNBOOK §8）")
        reminder = self._corpus(REMINDER_LIVE_EVIDENCE_DIR)
        self.assertTrue(
            reminder,
            "live RED: delivery/changes/dev/260829-schedule-reminder/ 下仍无 live 五齿证据 — 包 7 "
            "留根的 L1-L5（真 cron/真 pending.json/真推送/真渲染回执出队/真语义产物）未真跑"
            "一轮; 9b 承接 ECS 部署窗口后必须真 exercise 一次端到端（probe ≠ exercise）")


if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(StoryWiringOfflineAcceptance)
    if os.environ.get("RUN_LIVE") == "1" or os.environ.get("HWV_RUN_LIVE") == "1":
        suite.addTests(loader.loadTestsFromTestCase(StoryWiringLiveAcceptance))
    runner = unittest.TextTestRunner(verbosity=2)
    sys.exit(0 if runner.run(suite).wasSuccessful() else 1)