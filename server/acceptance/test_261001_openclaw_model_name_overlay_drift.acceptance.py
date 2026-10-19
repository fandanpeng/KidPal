"""Frozen acceptance tests — TaskPackage 261001-openclaw-model-name-overlay-drift
(subsystem: server/deploy; rigor lite; riskTier low; sourceType drift-report;
gateProfile ["test-quality", "docs-sync"]).

需求锚 / 漂移报告锚：docs/crq/tech/ARQ-261001-openclaw-model-name-overlay-drift.md
（status deferred；trigger = 本包落地后转 closed）。变更计划权威 =
delivery/taskpackages/261001-openclaw-model-name-overlay-drift/plan.md（reviewed，§2
changeManifest 六件 / §5 验证方案 / §7 技术方案 / §8 起包修正）。

背景（drift，非某次改动引入的回归）：OpenClaw 由单 agent `main` 演进为
`main` + `kidpal` 双 agent（`ownership: explicit`），引擎 `.config.yaml` LLM 段的
`model_name` 必须点名 agent 才能唯一定位答主；仓内声明面/渲染面/探针默认值仍钉旧值
（openclaw 后接 default 后缀），下次 overlay 重部署会把 live 冲回旧值 → 引擎 400
「Multiple agents are configured, but this operation has no explicit owner」。活配置
已于 261001 热修（ECS 侧），本包 = 仓内回流，防下次部署复现。

===========================================================================
Scope of record（taskpackage acceptance MNF-A1 / MNF-A2；MNF-A3 见下）：

  MNF-A1（基线与模板一致）：仓内旧值零残留——声明面 / 渲染面 / 探针默认值 / provider
    默认值四处均为 openclaw/kidpal（历史变更记录类文本除外）。承载齿：
      · test_mnf_a1_live_declaration_surfaces_pin_kidpal —— 四处显式正钉（机制级）；
      · test_mnf_a1_no_stale_model_residue_on_live_surfaces —— server 现役代码/配置面
        残留扫描（机制级）。
  MNF-A2（冻结验收重钉）：PERF-K1 的 PERF-E1/E2 齿期望值改为 openclaw/kidpal 且转绿；
    同额订正 :917/:939 的失准文案（原称「契约 §四钉」model_name——契约对 model_name 零
    命中）。承载齿：
      · test_mnf_a2_perf_k1_frozen_teeth_repinned —— 钉冻结件文本已重钉 + 失准文案已消
        （落盘/措辞级，见头注 guard-class）。
    （值重钉的行为侧红基线 = 重钉后 PERF-K1 件 test_e1_provider_llm_segment_unchanged /
    test_e1_overlay_llm_segment_unchanged 因仓内配置仍为旧值而转红；builder 改配置后转绿。）
  MNF-A3（部署演练，可选 human 授权）：overlay 在 ECS 重跑一次渲染，核验引擎 .config.yaml
    落 openclaw/kidpal 且不覆盖其它段——**非可断言**（需 ECS + human 授权），不落本件齿；
    建议按 A10 以结构性标记承载（见交付回执）。

===========================================================================
强度申报（acceptance-evidence「契约条目 ↔ 齿 的强度申报」T2；映射 / 缺口 / 等价假设）

  映射（设计条目 → 承载齿 → 性质档）：
   · MNF-A1 声明面/渲染面/探针/provider 四处 = openclaw/kidpal  → a1_live_surfaces 齿 → 机制级
   · MNF-A1 现役 server 代码/配置面旧值零残留                    → a1_no_stale_residue 齿 → 机制级
   · MNF-A2 冻结验收件已重钉 + 失准文案已消                      → a2_repinned 齿 → 落盘/措辞级

  缺口（承载齿强度低于所需处，逐条）：
   · **offline 齿不证明引擎 live 真的以 openclaw/kidpal 发起请求**——那需在 ECS 重跑
     overlay 渲染 + 探针对 `127.0.0.1:18789`（loopback，仓内不可达）真发一次请求。本件只证
     「仓内声明面/渲染面文本已重钉」，live 行为归 MNF-A3 部署演练（human 授权）。
   · **残留扫描范围 = server 现役代码/配置面**（见 _iter_live_surface_files）：不含
     `server/acceptance/**`（验收件按设计在断言里引用值字面；其唯一承载 PERF-K1 件由
     MNF-A2 齿显式覆盖）、不含退役面（`server/shim.py` 头注 ARCHIVED / `server/kidpal-shim.service`
     / `server/acceptance/archive/**`）、不含历史记录类（`docs/crq/**`、`docs/decisions/ADR.md` ADR-006
     正文、`delivery/**` 过程证据、`.claude/skills/kidpal-ecs-gateway/SKILL.md` 历史解释）。
     故本齿是「现役 live 面零残留」而非字面「全仓零残留」——全仓扫描会扫进大量历史/过程
     文本而长期假红。

  行为等价假设（显式化，不写即默认不成立）：
   · 「仓内 overlay 渲染面文本 model_name = openclaw/kidpal」⇔「下次 overlay 重部署后引擎
     .config.yaml 落 openclaw/kidpal」——假设渲染面无其它覆盖（apply-xiaozhi-overlay.sh 的
     heredoc 直写 LLM 段为唯一 live 真源，plan §7 architect 预评坐实）。
   · provider.py 的 `OPENCLAW_MODEL` 默认值「无 live 消费方」（front 只用 corr_id() / 常量）
     ——其默认值改动 = 纯声明一致，本齿只钉文本已同步，不断言 live 行为。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/
"""

from __future__ import annotations

import unittest
from pathlib import Path

try:
    import pytest  # noqa: F401  (markers only; standalone unittest also works)
    _HAS_PYTEST = True
except ImportError:
    _HAS_PYTEST = False

# --------------------------------------------------------------------------- #
# Paths（仓根由 __file__ 显式推导，不依赖调用点 cwd——acceptance-evidence root 注入纪律）
# --------------------------------------------------------------------------- #

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY = SERVER / "deploy"
GATEWAY = SERVER / "gateway"

PROVIDER_DECL_YAML = DEPLOY / "provider.config.yaml"
OVERLAY_SH = DEPLOY / "apply-xiaozhi-overlay.sh"
PROBE_BATCH_PY = DEPLOY / "perf-k1" / "probe_batch.py"
GATEWAY_PROVIDER_PY = GATEWAY / "provider.py"
PERF_K1_FROZEN = SERVER / "acceptance" / "test_perf_k1_roundtrip_latency.acceptance.py"

STALE_MODEL = "openclaw/default"
BASELINE_MODEL = "openclaw/kidpal"

# --------------------------------------------------------------------------- #
# MNF-A1 · 现役 live 面清单（声明面 / 渲染面 / 探针默认值 / provider 默认值）
#   每项 = (人读标签, 文件, 变更后应含的子串)。子串取到足以定位该行、又不至于过脆。
# --------------------------------------------------------------------------- #

LIVE_SURFACES = [
    ("provider.config.yaml 声明面 model",
     PROVIDER_DECL_YAML, 'model: "openclaw/kidpal"'),
    ("apply-xiaozhi-overlay.sh 渲染面 model_name",
     OVERLAY_SH, "model_name: openclaw/kidpal"),
    ("perf-k1/probe_batch.py 探针默认值 DEFAULT_MODEL",
     PROBE_BATCH_PY, 'DEFAULT_MODEL = "openclaw/kidpal"'),
    ("gateway/provider.py provider 默认值 OPENCLAW_MODEL",
     GATEWAY_PROVIDER_PY, 'os.environ.get("OPENCLAW_MODEL", "openclaw/kidpal")'),
]

# 现役残留扫描范围：server 现役代码/配置文书（.py/.sh/.yaml/.yml/.json/.service/.toml/.cfg/.ini）
_SCAN_SUFFIXES = (".py", ".sh", ".yaml", ".yml", ".json", ".service", ".toml", ".cfg", ".ini")
# 排除目录（相对 server/ 的任一路径段命中即跳过）：acceptance = 验收件（按设计引用值字面；
# PERF-K1 件由 MNF-A2 齿显式覆盖）。
_SCAN_SKIP_DIR_SEGMENTS = {"acceptance"}
# 排除文件（相对 server/ 的 posix 相对路径）：退役面（ARCHIVED 头注 / 退役 systemd 单）。
_SCAN_SKIP_REL_FILES = {"shim.py", "kidpal-shim.service"}


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _iter_live_surface_files(root: Path):
    """产出 server 现役代码/配置面文件 (path, rel_posix)，排除验收件与退役面 carve-out。"""
    for p in sorted(root.rglob("*")):
        if not p.is_file() or p.suffix.lower() not in _SCAN_SUFFIXES:
            continue
        rel = p.relative_to(root)
        if any(seg in _SCAN_SKIP_DIR_SEGMENTS for seg in rel.parts[:-1]):
            continue
        if rel.as_posix() in _SCAN_SKIP_REL_FILES:
            continue
        yield p, rel.as_posix()


class MnfOfflineAcceptance(unittest.TestCase):

    maxDiff = None

    # ------------------------------------------------------------------ #
    # MNF-A1 · 四处 live/声明面显式正钉（机制级）
    # ------------------------------------------------------------------ #

    def test_mnf_a1_live_declaration_surfaces_pin_kidpal(self):
        """coverage: MNF-A1 — 声明面/渲染面/探针/provider 四处 live 面均为 openclaw/kidpal（机制级）。

        每处同时断言：① 变更后目标值在场；② 旧值（openclaw 后接 default 后缀）不残留。
        """
        for label, path, needle in LIVE_SURFACES:
            with self.subTest(surface=label):
                src = _read_text(path)
                self.assertTrue(src, f"MNF-A1 RED: {path} 不在场或为空（{label}）。")
                self.assertIn(
                    needle, src,
                    f"MNF-A1 RED: {label} 未重钉为 openclaw/kidpal —— 期望子串 {needle!r} 不在 "
                    f"{path} 中。双 agent（ownership: explicit）下旧值无法唯一定位答主，引擎返 400。",
                )
                self.assertNotIn(
                    STALE_MODEL, src,
                    f"MNF-A1 RED: {label} 仍残留旧值 {STALE_MODEL!r} —— 下次 overlay 重部署会把它 "
                    f"写回引擎 .config.yaml（drift 复现路径）。文件 {path}。",
                )

    # ------------------------------------------------------------------ #
    # MNF-A1 · 现役 server 代码/配置面旧值零残留（机制级；扫描齿 root 注入不依赖 cwd）
    # ------------------------------------------------------------------ #

    def test_mnf_a1_no_stale_model_residue_on_live_surfaces(self):
        """coverage: MNF-A1 — server 现役代码/配置面（排除验收件与退役面）无旧值残留（机制级）。

        扫描范围见 _iter_live_surface_files：acceptance/** 与退役面（shim.py /
        kidpal-shim.service）为显式 carve-out。
        """
        offenders = []
        for path, rel in _iter_live_surface_files(SERVER):
            if STALE_MODEL in _read_text(path):
                offenders.append(rel)
        self.assertEqual(
            offenders, [],
            "MNF-A1 RED: server 现役 live 面仍残留旧值 "
            f"{STALE_MODEL!r} —— 涉 {offenders}。四处变更点之外若有遗漏 live 面（或未来新增），"
            "重部署同样会漂移回旧值（drift 复现）。",
        )

    # ------------------------------------------------------------------ #
    # MNF-A2 · PERF-K1 冻结验收件已重钉 + 失准文案已消（落盘/措辞级）
    # ------------------------------------------------------------------ #

    def test_mnf_a2_perf_k1_frozen_teeth_repinned(self):
        """coverage: MNF-A2 — PERF-K1 冻结件 E1/E2 期望值已重钉 openclaw/kidpal 且旧值/失准文案已消。

        值重钉的行为侧红基线 = 重钉后该件 test_e1_provider_llm_segment_unchanged /
        test_e1_overlay_llm_segment_unchanged 因仓内配置仍为旧值而转红；builder 改配置后转绿
        （见本件头注 Scope）。本齿钉「期望值文本已重钉 + 失准文案（误称契约 §四钉 model_name）
        已消」，防回退。
        """
        self.assertTrue(
            PERF_K1_FROZEN.is_file(),
            f"MNF-A2 RED: PERF-K1 冻结件不在位 —— 要求 {PERF_K1_FROZEN}。",
        )
        src = _read_text(PERF_K1_FROZEN)

        # 期望值已重钉（声明面 model / 渲染面 model_name 两处）。
        self.assertIn(
            'model: "openclaw/kidpal"', src,
            "MNF-A2 RED: PERF-E1 齿期望值未重钉 —— 声明面期望子串不在冻结件中。",
        )
        self.assertIn(
            "model_name: openclaw/kidpal", src,
            "MNF-A2 RED: PERF-E2 齿期望值未重钉 —— 渲染面期望子串不在冻结件中。",
        )

        # 旧期望值不得残留（重钉未完成的信号）。
        self.assertNotIn(
            'model: "openclaw/default"', src,
            "MNF-A2 RED: PERF-E1 齿旧期望值仍残留（重钉未完成）。",
        )
        self.assertNotIn(
            "model_name: openclaw/default", src,
            "MNF-A2 RED: PERF-E2 齿旧期望值仍残留（重钉未完成）。",
        )

        # 失准文案：不得再称契约 §四钉 model_name（契约对 model_name 零命中）。
        self.assertNotIn(
            "§四钉 openclaw", src,
            "MNF-A2 RED: 失准文案（误称契约 §四钉旧值）未订正 —— 契约 §四不钉 model_name，"
            "§四只钉 base_url/loopback + 禁 max_tokens。",
        )


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
