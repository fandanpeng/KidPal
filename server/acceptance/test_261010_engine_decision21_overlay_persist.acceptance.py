"""Frozen acceptance tests — TECH-261010-engine-decision21-keys-not-persistent
(subsystem: server/deploy; anchor: docs/crq/tech/TECH-261010-engine-decision21-keys-not-persistent.md).

human 直接授权直修（2026-10-10 会话在案；跳过 dev-loop、如实记录）：真机使用反馈
两条 bug（静置 2 分钟告别语复出 / 重启后 WS 推不下通知）取证定根因 = 引擎决策 21 两键
（close_connection_no_voice_time: 86400 + end_prompt.enable: false）从未进入 overlay
渲染链——260914 的 live 修正只落在活配置，任何整体再生（最近一次 1001 overlay 重跑）
即丢失，引擎回落源码默认值（120s 无语音告别语 + 180s 硬断，receiveAudioHandle.py:114/123）
→ 决策 21「设备常驻在线（提醒推送可达）」被击穿。

本件 = 仓内根治齿（对齐 261001-openclaw-model-name-overlay-drift 先例形态）：
钉「决策 21 两键有持久渲染面」，防下次重部署复现。

===========================================================================
齿清单（三条，均机制级文本正钉）：

  D21-A1 渲染面正钉：apply-xiaozhi-overlay.sh 的 .config.yaml heredoc 含两键
        （close_connection_no_voice_time: 86400 与 end_prompt: enable: false）。
        渲染面是 live .config.yaml 的唯一真源——heredoc 缺键 = 再生即丢（本次病根）。
  D21-A2 声明面正钉：provider.config.yaml（渲染源的机器可读声明）含同两键
        （engine_timeout 声明块）。
  D21-A3 声明面↔渲染面一致性：两键的值在声明面与渲染面逐字一致
        （86400 / enable: false）。

缺口（强度申报，逐条）：
  · offline 齿不证引擎 live 真的以 24h 兜底跑——live 行为由部署演练承载
    （主线程 261010 在 ECS 重跑 overlay 渲染 + 引擎日志核验，授权 = 同一 human 指示）。
  · 齿只钉渲染/声明两面文本，不覆盖 .config.yaml 全量再生语义（heredoc 重写全文）——
    与 261001 MNF-A1 行为等价假设同构：heredoc 直写为唯一 live 真源。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_261010_engine_decision21_overlay_persist.acceptance.py
"""

from __future__ import annotations

import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY = SERVER / "deploy"

PROVIDER_DECL_YAML = DEPLOY / "provider.config.yaml"
OVERLAY_SH = DEPLOY / "apply-xiaozhi-overlay.sh"

# 决策 21 两键（arch §6.4）：值 = 决策原文，不得「顺手调优」。
D21_NO_VOICE_KEY = "close_connection_no_voice_time: 86400"
D21_END_PROMPT_RENDER = "end_prompt:"
D21_END_PROMPT_ENABLE = "  enable: false"


class D21OverlayPersistAcceptance(unittest.TestCase):

    maxDiff = None

    # ------------------------------------------------------------------ #
    # D21-A1 · 渲染面正钉（heredoc 是 live .config.yaml 唯一真源）
    # ------------------------------------------------------------------ #

    def test_d21_a1_overlay_renders_decision21_keys(self):
        """coverage: D21-A1 — apply-xiaozhi-overlay.sh 渲染面含决策 21 两键。"""
        self.assertTrue(
            OVERLAY_SH.is_file(),
            f"D21-A1 RED: 渲染面不在场 —— 要求 {OVERLAY_SH}。",
        )
        src = OVERLAY_SH.read_text(encoding="utf-8", errors="replace")
        self.assertIn(
            D21_NO_VOICE_KEY, src,
            "D21-A1 RED: 渲染面缺 close_connection_no_voice_time: 86400 —— heredoc 不写此键，"
            "下次 overlay 再生会把引擎冲回源码默认 120s 无语音断连（决策 21 复现病根）。",
        )
        self.assertIn(
            D21_END_PROMPT_ENABLE, src,
            "D21-A1 RED: 渲染面缺 end_prompt.enable: false —— 缺键即引擎默认告别语复活"
            "（receiveAudioHandle.py:123 enable 缺省 True）。",
        )

    # ------------------------------------------------------------------ #
    # D21-A2 · 声明面正钉（provider.config.yaml 机器可读声明）
    # ------------------------------------------------------------------ #

    def test_d21_a2_declaration_carries_decision21_keys(self):
        """coverage: D21-A2 — provider.config.yaml 声明面含决策 21 两键（engine_timeout 块）。"""
        self.assertTrue(
            PROVIDER_DECL_YAML.is_file(),
            f"D21-A2 RED: 声明面不在场 —— 要求 {PROVIDER_DECL_YAML}。",
        )
        src = PROVIDER_DECL_YAML.read_text(encoding="utf-8", errors="replace")
        self.assertIn(
            "close_connection_no_voice_time: 86400", src,
            "D21-A2 RED: 声明面缺 close_connection_no_voice_time: 86400 —— 渲染源声明不完整。",
        )
        self.assertIn(
            "enable: false", src,
            "D21-A2 RED: 声明面缺 end_prompt enable: false —— 渲染源声明不完整。",
        )

    # ------------------------------------------------------------------ #
    # D21-A3 · 声明面 ↔ 渲染面一致性（两键值逐字同）
    # ------------------------------------------------------------------ #

    def test_d21_a3_declaration_render_values_agree(self):
        """coverage: D21-A3 — 两键值在声明面与渲染面逐字一致（防半改）。"""
        decl = PROVIDER_DECL_YAML.read_text(encoding="utf-8", errors="replace")
        rend = OVERLAY_SH.read_text(encoding="utf-8", errors="replace")
        for needle in ("close_connection_no_voice_time: 86400", "enable: false"):
            in_decl = needle in decl
            in_rend = needle in rend
            self.assertEqual(
                in_decl, in_rend,
                f"D21-A3 RED: 键 {needle!r} 在声明面/渲染面仅单侧在场（decl={in_decl}, "
                f"render={in_rend}）——半改状态，两面须同步落。",
            )


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
