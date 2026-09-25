"""260925-prompt-v7-round-routing — v7 回合路由制 prompt 齿（offline）。

冻结范围（human 260925 拍板批）：
  ① v7 落盘锚齿 + 「制品本体无版本沿革前言」结构齿（首行即 `## 角色`）；
  ② 回合类型路由表锚（孩子对话 / 查字 / 课表问答 / 提醒 cron 四行 + 兜底句 + 只读边界 +
     课表反编造覆盖句——2026-09-25 live 编造事故的生成端根治锚）；
  ③ 禁产 story JSON 锚（2026-09-24 契约门 Q7 对齐：relay 机械拒绝引擎侧
     story_list/story_text，prompt 不得再教模型产这两块）；
  ④ 课表段 m3k3 同组语义锚（问答框 / 不切页 / 备注读回 / 意图薄载荷 / 网格不由 LLM 生成）；
  ⑤ v6 历史版本 sha256 零回改齿（E2 铁律同形态；v1..v5 由既有各包齿钉）。

强度申报：
  1) 内容驱动选节（并集窗口，不钉标题措辞；措辞换写不误红、语义被删才红——m3k3 ② 同族）；
  2) 本齿**不**断言「当前接线 = v7」版本字面（ADR-001 / ARQ-260908 纪律：三面接线一致性由
     test_m3k3_voice_schedule_entry.acceptance.py::test_a1_prompt_wired_three_faces_version_agnostic
     与 test_perf_k1_roundtrip_latency.acceptance.py 的版本无关形态承载，bump 不假红）；
  3) v7 本体零回改 = 后继 bump 时新增 `.vN` sha256 齿（同本文件 ⑤ 形态），不改本齿。
"""

from __future__ import annotations

import hashlib
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DEPLOY_DIR = REPO_ROOT / "server" / "deploy"

V7_NAME = "openclaw-system-prompt.v7.md"
V6_NAME = "openclaw-system-prompt.v6.md"

# 冻结时点 v6 内容 hash（raw bytes 口径，v6 为 LF 落盘；v7 bump 与一切后续包不得改 v6 本体）。
# 计算式 = sha256(read_bytes())。
V6_SHA256 = "bf5c6b4c11fd155241a0c5e51adbe2cbcce82768426e12e1a135dfa573f6197b"


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _sha256_raw(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _section_window(body: str, title_keywords: tuple[str, ...],
                    extra_line_keywords: tuple[str, ...] = ()) -> str:
    """内容驱动选节：标题含任一关键词的 `## ` 节全文 + 其它节内含 extra 行的并集。"""
    lines = body.splitlines()
    heads = [i for i, l in enumerate(lines) if l.startswith("## ")]
    out: list[str] = []
    for k, i in enumerate(heads):
        j = heads[k + 1] if k + 1 < len(heads) else len(lines)
        if any(w in lines[i] for w in title_keywords):
            out.extend(lines[i:j])
        else:
            out.extend(l for l in lines[i:j]
                       if any(w in l for w in extra_line_keywords))
    if not heads:  # 无节结构（异常文件）——退化为全文
        out = lines
    return "\n".join(out)


class TestPromptV7RoundRouting(unittest.TestCase):
    """260925-prompt-v7-round-routing（offline，静态锚齿）。"""

    @classmethod
    def setUpClass(cls):
        cls.v7_path = DEPLOY_DIR / V7_NAME
        cls.v7 = _read_text(cls.v7_path)
        cls.v6_path = DEPLOY_DIR / V6_NAME

    # -- ① 落盘 + 无前言结构齿 ------------------------------------------------

    def test_a1_v7_prompt_file_lands(self):
        """coverage: V7-A1 — v7 落盘非空（版本化演进，历史版本共存可 diff）。"""
        self.assertTrue(
            self.v7_path.is_file(),
            f"V7-A1 RED: {V7_NAME} 未落盘——制品缺失即接线悬空（E4 族）。",
        )
        self.assertGreater(
            len(self.v7.strip()), 0,
            "V7-A1 RED: v7 文件为空。",
        )

    def test_a1_v7_no_version_preamble(self):
        """coverage: V7-A1 — 制品本体无版本沿革前言：首行即 `## ` 节标题。

        v6 及以前的前言（20.8% / 对模型零操作语义 + 免检区行为语义载体）随
        260925-prompt-v7-round-routing 外置 OPENCLAW_SYSTEM_PROMPT_CARRIER；本齿防回潮。"""
        first = self.v7.splitlines()[0].strip() if self.v7.splitlines() else ""
        self.assertEqual(
            first, "## 角色",
            "V7-A1 RED: v7 首行不是 `## 角色`——版本沿革前言回潮或角色节被移位"
            "（沿革只允许住 OPENCLAW_SYSTEM_PROMPT_CARRIER）。",
        )
        self.assertFalse(
            self.v7.startswith("#") and not self.v7.startswith("## "),
            "V7-A1 RED: v7 顶层出现 H1 前言标题（前言回潮）。",
        )

    # -- ② 回合路由表锚 --------------------------------------------------------

    def test_a1_v7_round_routing_anchors(self):
        """coverage: V7-A1 — 回合类型路由表：三类回合行 + 查字绝对禁工具 + 兜底句 +
        只读边界 + 课表反编造覆盖句。P0-1 根治语义必须整体在场。"""
        window = _section_window(self.v7, ("回合类型",), ("回合", "课表", "schedule"))

        for kw, why in (
            ("孩子对话回合", "对话回合（一回合作答禁工具）行缺失"),
            ("查字回合", "查字回合（绝对禁工具）行缺失——human 260925 复审补行要求"),
            ("课表问答回合", "课表问答回合（先读真课表）行缺失"),
            ("提醒 cron 回合", "提醒 cron 回合（语义 JSON）行缺失"),
            ("判不清", "兜底句（判不清按对话回合处理）缺失"),
            ("严禁凭印象", "课表反编造覆盖句缺失——9/25 live 编造事故的生成端封堵锚"),
            ("只读", "kidpal/、stories/ 只读边界声明缺失（QQ 录入归家庭侧 agent 的拓扑声明）"),
            ("kidpal-schedule-answer", "课表问答回合未指 skill 流程（prompt↔skill 许可结构）"),
        ):
            self.assertIn(kw, window, f"V7-A1 RED: 回合路由面无「{kw}」——{why}。")
        # 一回合作答禁令强度（对百科回合的延迟保护，NFR-PERF-1）。
        self.assertTrue(
            any(w in window for w in ("不调用任何工具", "不读取任何文件")),
            "V7-A1 RED: 对话回合「一回合作答」禁令强度被稀释——禁工具/禁读文件表述缺失。",
        )

    # -- ③ 禁产 story JSON 锚 --------------------------------------------------

    def test_a1_v7_story_json_ban_anchor(self):
        """coverage: V7-A1 — 星星记录本禁产 story_list/story_text display 块
        （2026-09-24 契约门 Q7：relay 机械拒绝引擎侧来源）。"""
        window = _section_window(self.v7, ("故事",), ("story",))
        self.assertIn("story_list", window, "V7-A1 RED: story 面无 story_list 措辞。")
        self.assertIn("story_text", window, "V7-A1 RED: story 面无 story_text 措辞。")
        self.assertTrue(
            any(w in window for w in ("严禁生成", "不要生成", "禁生成", "不生成", "你不生成")),
            "V7-A1 RED: story 面无「禁产 story JSON」禁令——模型被教产出 relay 机械拒绝的块。",
        )
        self.assertTrue(
            any(w in window for w in ("文字答复", "只用文字", "text view")),
            "V7-A1 RED: story 面无「只用文字答复」正路口径。",
        )

    # -- ④ 课表段 m3k3 同组语义锚 ----------------------------------------------

    def test_a1_v7_schedule_anchors(self):
        """coverage: V7-A1 — 课表段语义锚（m3k3 锚组并集：问答框 / 不切页 / 备注读回 /
        意图薄载荷 / 网格不由 LLM 生成）。课表段跨批不得丢语义。"""
        window = _section_window(self.v7, ("课表",), ("课表", "schedule"))
        self.assertIn("问答框", window, "V7-A1 RED: 课表段无「问答框」锚。")
        self.assertTrue(
            any(w in window for w in ("不切", "不滚动", "不定位")),
            "V7-A1 RED: 课表段无「不切页/不定位」锚。")
        self.assertIn("备注", window, "V7-A1 RED: 课表段无「备注」锚。")
        self.assertTrue(
            any(w in window for w in ("答复", "读回", "念出")),
            "V7-A1 RED: 课表段无「答复/读回」锚。")
        self.assertTrue(
            any(w in window for w in ("帮我显示课表", "打开课表")),
            "V7-A1 RED: 课表段无显示意图锚。")
        self.assertTrue(
            "薄载荷" in window or "{}" in window,
            "V7-A1 RED: 课表段无薄载荷 data:{} 锚。")
        self.assertTrue(
            any(w in window for w in ("不生成", "不由 LLM", "不产", "机械计算", "front", "网关")),
            "V7-A1 RED: 课表段无「网格数据不由 LLM 生成」锚。")

    # -- ⑤ v6 历史版本零回改 ----------------------------------------------------

    def test_a1_v6_prompt_file_immutable(self):
        """coverage: V7-A1 — v6 本体零回改（E2 铁律同形态；v1..v5 由既有各包齿钉）。
        v6 冻结时点 sha256 = bf5c6b4c…（260925-prompt-v7-round-routing 批冻结）。"""
        self.assertTrue(
            self.v6_path.is_file(),
            "V7-A1 RED: v6 文件被删除——历史版本共存可 diff 铁律被破坏。",
        )
        self.assertEqual(
            _sha256_raw(self.v6_path), V6_SHA256,
            "V7-A1 RED: v6 本体被回改——历史版本一律不改（版本化落盘惯例）。",
        )


if __name__ == "__main__":
    unittest.main(verbosity=2)
