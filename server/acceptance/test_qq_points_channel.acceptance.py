"""Frozen acceptance tests — TaskPackage 261104-k3-qq-points-channel（server 面）。

承载 story / 语义权威 = docs/requirements.md S-F6.4（QQ 家长记账与更正，AC1–AC5）+
S-F6.3（设备语音记账与兑换，AC3 只认激活目标分值 / AC4 真实兑现后扣 / AC5 短窗去重）+
S-F6.2（浏览详细账本与删除误记，AC4 删除重算与负余额）+ NFR-PERF-4（记账反馈时延，
本件只钉单帧推送通路存在，实测时延留真实 QQ 轮、不写死阈值）。契约只读消费 =
docs/contracts/interface-contract.md §1.3⑦⑧（points_home / points_ledger restore 通道）/
§四（kidpal_points_delete 幂等 / state 路径单源）。工程先例 = server/deploy/
openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md（skill 静态纪律）+
server/acceptance/test_261013_qq_schedule_write_closure.acceptance.py（部署三面）。

Scope of record（K3Q-A1..A7 → 本文件测试方法）:

  K3Q-A1 CLI 子命令面（server/gateway/points.py）：
    add / redeem / delete --entry-id|--index / show / recent / targets 分派；success 回执
    逐字段精确（含 revision 与 command_id）；usage-shape 错误与 KIDPAL_POINTS_STATE 缺失
    → exit 2（stderr 说明）。
      · test_k3q_a1_add_receipt_exact_fields              （行为级）
      · test_k3q_a1_show_recent_targets_readbacks         （行为级）
      · test_k3q_a1_redeem_active_target_gate_negative    （行为级）
      · test_k3q_a1_usage_and_env_errors_exit_2           （行为级）

  K3Q-A2 拒绝不伪造成功：非法值 / 歧义删除定位 → exit 1、说明写 stderr、stdout 无成功
    回执、state 文件**字节不变**（零写入）。
      · test_k3q_a2_invalid_values_exit_1_zero_write      （行为级 / 错误路径）
      · test_k3q_a2_ambiguous_delete_locator_zero_write   （行为级 / 错误路径）

  K3Q-A3 幂等（传输幂等权威在 K1 CLI，skill 不得自行吞重）：
      · test_k3q_a3_command_id_replay_returns_first_result（行为级）
      · test_k3q_a3_short_window_duplicate_receipt        （行为级）

  K3Q-A4 稳定定位删除（S-F6.4-AC3 + S-F6.2-AC4 删除重算）：
      · test_k3q_a4_recent_index_entry_id_same_line       （行为级）
      · test_k3q_a4_delete_by_index_exact_middle          （行为级）
      · test_k3q_a4_delete_by_entry_id_and_oor            （行为级 + 幂等 no-op）
      · test_k3q_a4_index_resolution_locked_source_anchor （源锚代理）

  K3Q-A5 kidpal-points-write SKILL.md 静态纪律 + 负向断言（文件当前不存在 → 天然红）：
      · test_k3q_a5_pins_machine_surface                  （机制级）
      · test_k3q_a5_negative_ask_user_absent              （机制级 / 负向）
      · test_k3q_a5_forbids_state_edit_schedule_residual  （机制级 / 负向）
      · test_k3q_a5_pins_always_call_cli_oneshot_error    （机制级）

  K3Q-A6 离线串链（tmp state 驱动 CLI 写入 → front 观察缝恰推一帧 restore）：
      · test_k3q_a6_cli_write_single_restore_push         （行为级）
      · test_k3q_a6_delete_noop_does_not_push             （行为级 / 幂等 no-op）

  K3Q-A7 部署三面（manifest 新 skill src/dst 对 + deploy/verify 表驱动）：
      · test_k3q_a7_manifest_registers_points_skill       （机制级）
      · test_k3q_a7_deploy_verify_manifest_driven         （机制级 / must-stay-green 回归）

===========================================================================
FROZEN OFFLINE CONTRACT (builder MUST implement) — 逐条对齐 plan.md「技术方案」节：

  A. server/gateway/points.py CLI（`python -m gateway.points`，先 cd /opt/kidpal/front）:
     1) 子命令 add | redeem | delete | show | recent | targets 分派。
     2) state 路径单源 = os.environ["KIDPAL_POINTS_STATE"]；缺失/空 -> exit 2，stderr
        明文含 KIDPAL_POINTS_STATE，零写入（无第二账本）。
     3) 形状/用法错误（无子命令 / 未知子命令 / 参数缺失或多余 / 非法选项）-> exit 2，
        usage 写 stderr，零写入。
     4) add：`--points N --reason TEXT --command-id ID`；同时**兼容既有位置参数**
        `add <points> <reason>`（K0/K1 调用形态零放宽）。成功回执单行 key=value，须含
        entry_id(pt_<hex>)/delta(+N)/balance/reason/revision/command_id 全部字段。
     5) redeem：`--points N --command-id ID`；只接受当前激活目标分值（不符 -> exit 1，
        零写入）；事由由执行器取「兑换<激活目标名>」，模型不提供；落负向分录。
     6) delete：`--entry-id ID` 与 `--index N` 互斥且**恰给一个**（都缺/都给 -> exit 1
        一次性普通文本报错）；index 须正整数；越界/空账 -> exit 1，零写入；entry-id
        形状合法但不存在 -> 幂等 no-op exit 0 `deleted=false`（revision 不变、零写入）；
        存在 -> 删除、revision+1、全量重算（允许负余额，OQ-1）。
        序号→id 换算与删除须在**同一 _critical 临界区**一次做完：拆内部
        `_delete_entry_locked(...)`，公开 `delete_entry` 转调（防 TOCTOU 删错账）。
     7) show：无参；单行含 balance / revision / entries_total / 激活目标 id·name·points
        （四键）；只读、不创建/改写 state 文件。
     8) recent `--limit N`（缺省 5，区间 1–50）：每条一行，**index 与 entry_id 同行**，
        index=1 = 时间倒序最新一条；换算权在执行器、不收进模型。
     9) targets：无参；只读列举预置四目标（+自定义）含 id/name/points。
    10) 拒绝语义统一：exit 1、说明写 stderr、stdout 无成功回执、state 文件字节不变。
    11) command_id 传输幂等 + 短窗语义去重委托 K1（add_points/redeem_points）；CLI 与
        skill 均**不得自行判断重复就不调**。

  B. server/deploy/openclaw-skills/workspace-main/kidpal-points-write/SKILL.md（新建）：
     静态纪律见 K3Q-A5 断言（钉死解释器 / state env / -m gateway.points 调用形 / 全子
     命令面 / 恒调 CLI 不吞重 / 一次性普通文本报错 / 禁 ask_user / 禁直写 state.json /
     无 schedule parser 机检残留 / workspace-kidpal 只读边界）。

  C. server/deploy/manifest.yaml：新增 kidpal-points-write 条目
     {src: <POINTS_SKILL_SRC>, dst: /root/.openclaw/workspace/skills/kidpal-points-write/SKILL.md,
      owner: deploy.sh, verify: md5, restart: none}。deploy.sh / verify-deploy.sh 保持
     manifest entries 表驱动（本包零脚本改动即可生效）。

  D. server/gateway/front/front_app.py：观察缝（_observe_points_revision /
     run_points_observe_once）已接线，本包**不新增设备渲染分支**——K3Q-A6 只复证
     QQ/CLI 写入经该缝恰推一帧 points_home restore=true（S-F6.4-AC5 不抢前台）。

==================== 契约条目 ↔ 验收断言 的强度申报 ====================

| 契约 / 决策条目 | 承载断言 | 性质档 |
|---|---|---|
| S-F6.4-AC1 记账同构（CLI add 任意分+事由） | test_k3q_a1_add_receipt_exact_fields / test_k3q_a5_pins_machine_surface | 行为级 + 机制级 |
| S-F6.4-AC2 查账真实状态 | test_k3q_a1_show_recent_targets_readbacks | 行为级 |
| S-F6.4-AC3 删除稳定定位 + 歧义普通文本 | test_k3q_a4_* / test_k3q_a2_ambiguous_delete_locator_zero_write | 行为级 |
| S-F6.4-AC4 一次性可判（不依赖多轮澄清） | test_k3q_a2_* / test_k3q_a5_negative_ask_user_absent / test_k3q_a5_pins_always_call_cli_oneshot_error | 行为级 + 机制级 |
| S-F6.4-AC5 推送不抢前台 | test_k3q_a6_cli_write_single_restore_push（restore=true）/ test_k3q_a6_delete_noop_does_not_push | 行为级 |
| S-F6.3-AC3 兑换只认激活目标分值 | test_k3q_a1_redeem_active_target_gate_negative | 行为级 |
| S-F6.3-AC4 真实兑现后落负向分录（形状半） | test_k3q_a1_redeem_active_target_gate_negative | 行为级 |
| S-F6.3-AC5 短窗去重 / 兑换不重复扣 | test_k3q_a3_short_window_duplicate_receipt / test_k3q_a3_command_id_replay_returns_first_result | 行为级 |
| S-F6.2-AC4 删除重算（含负余额，重算形状半） | test_k3q_a4_delete_by_index_exact_middle | 行为级 |
| NFR-PERF-4 记账反馈时延（通路存在半） | test_k3q_a6_cli_write_single_restore_push（恰一帧 + 权威 revision） | 行为级（不在本件测时延值） |
| TECH-261030 ask_user 卡死（承接） | test_k3q_a5_negative_ask_user_absent | 机制级 |
| TECH-261023 / 模型只读（承接） | test_k3q_a5_forbids_state_edit_schedule_residual | 机制级 |

弱于契约处（显式缺口，不得当已兑现）:
  GAP-K3Q-1 「模型真实遵守 SKILL.md」离线不可证——本件只证护栏文本写对且含
            load-bearing token，不证 OpenClaw main agent 读到后按此执行 → 真实 QQ 轮。
  GAP-K3Q-2 NFR-PERF-4 实测时延（QQ 消息→积分页原地刷新）只记值、不写死阈值 → 真实
            QQ 轮 + 设备看屏/串口（K3Q-E4）。
  GAP-K3Q-3 S-F6.3-AC4「真实兑现后才扣」的人工兑换确认流程属运营过程，离线只钉「redeem
            只认激活目标分值 + 落负向分录」，不提前扣的流程约束 → 真实 QQ 轮。
  GAP-K3Q-4 「其他页不抢前台、积分页原地刷新」的设备演出半不落本件；本件只证下行帧
            restore=true / 单帧 → 设备看屏（K3Q-E4）。
  GAP-K3Q-5 command_id 平台消息 id 真实可得性与降级哈希路径 → 真实 QQ 轮。

行为等价假设（显式；不写即默认不成立）:
  EQ-1 `python -m gateway.points`（cwd=server）= ECS `cd /opt/kidpal/front && python -m
       gateway.points` 同一导入/执行语义（gateway 包可导入）。
  EQ-2 _FakeDeviceSocket / _FakeEngineLeg = 设备腿 / 引擎腿真实 send 语义（K2D EQ-2
       同口径；wire = kidpal/display mcp 信封 + view/data）。
  EQ-3 tmp 目录 state.json = /opt/kidpal/points/state.json 同一文件语义（K1L EQ-3）。
  EQ-4 直调 run_points_observe_once = reminder 轮询腿真实调用（front_app :2363/:2371）。

Coverage map（acceptanceRef -> test）:
  K3Q-A1 -> 四个 test_k3q_a1_*    K3Q-A2 -> 两个 test_k3q_a2_*
  K3Q-A3 -> 两个 test_k3q_a3_*    K3Q-A4 -> 四个 test_k3q_a4_*
  K3Q-A5 -> 四个 test_k3q_a5_*    K3Q-A6 -> 两个 test_k3q_a6_*
  K3Q-A7 -> 两个 test_k3q_a7_*
  coverage: K3Q-L1 —— live-manual 真实 QQ 加兑查删 + 歧义/重复/定位不清 + 不抢前台 +
            时延实测（载体 device-round-evidence.md / K3Q-E4），不落自动断言、不计入
            本文件通过面。

exitCriteria 映射：K3Q-E2（server 离线：skill 静态纪律 + CLI 参数回执 + 稳定定位 + 错误
回复 + command_id + 部署三面）-> A1..A7；K3Q-E4（真实 QQ + 设备轮）-> K3Q-L1 live-manual。

Run protocol（文件名两点须 importlib 模式；root 由 __file__ 显式推导，不依赖调用点 cwd）:
  python -m pytest -q --import-mode=importlib server/acceptance/test_qq_points_channel.acceptance.py
  (standalone: python 本文件)

REVISION LOG
  2026-11-14 初版冻结（acceptance-author）。设计态红 = points.py 六子命令 CLI（add 旗标形 /
             redeem --points / delete / show / recent / targets / revision+command_id 回执 /
             _delete_entry_locked）缺席；kidpal-points-write/SKILL.md 不在位；manifest 无
             points skill 条目。A7 的 deploy/verify 表驱动与 A2 的部分 usage 断言为
             must-stay-green 回归守卫（绿现状即既有语义，非空过）。
"""

from __future__ import annotations

import asyncio
import contextlib
import importlib.util
import inspect
import json
import os
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from types import ModuleType
from unittest.mock import Mock

# --------------------------------------------------------------------------- #
# Paths（仓根由 __file__ 显式推导）                                              #
# --------------------------------------------------------------------------- #

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
GATEWAY_DIR = SERVER / "gateway"
FRONT_APP = GATEWAY_DIR / "front" / "front_app.py"
POINTS_PY = GATEWAY_DIR / "points.py"

POINTS_STATE_ENV = "KIDPAL_POINTS_STATE"
PINNED_INTERPRETER = "/opt/kidpal/venv/bin/python3"

POINTS_SKILL_SRC = "server/deploy/openclaw-skills/workspace-main/kidpal-points-write/SKILL.md"
POINTS_SKILL_DST = "/root/.openclaw/workspace/skills/kidpal-points-write/SKILL.md"
POINTS_SKILL_PATH = REPO_ROOT / POINTS_SKILL_SRC
MANIFEST = SERVER / "deploy" / "manifest.yaml"
DEPLOY_SH = SERVER / "deploy" / "deploy.sh"
VERIFY_SH = SERVER / "deploy" / "verify-deploy.sh"

# 有效但不存在于账本的分录 id（pt_ + 32 hex，形状合法 -> 幂等 no-op）。
ABSENT_ENTRY_ID = "pt_" + ("d" * 32)


# --------------------------------------------------------------------------- #
# 工具                                                                          #
# --------------------------------------------------------------------------- #


def _new_state_path(tc) -> Path:
    d = tempfile.mkdtemp(prefix="k3q_")
    tc.addCleanup(lambda: __import__("shutil").rmtree(d, ignore_errors=True))
    return Path(d) / "state.json"


def _parse_kv(line: str) -> dict:
    out = {}
    for tok in line.split():
        if "=" in tok:
            key, val = tok.split("=", 1)
            out[key] = val
    out["_line"] = line.strip()
    return out


def _receipt_lines(proc) -> list:
    return [_parse_kv(ln) for ln in proc.stdout.splitlines() if ln.strip()]


def _receipt(proc) -> dict:
    lines = _receipt_lines(proc)
    return lines[0] if lines else {}


def _cli_env(state_path):
    env = dict(os.environ)
    env.pop(POINTS_STATE_ENV, None)
    if state_path is not None:
        env[POINTS_STATE_ENV] = str(state_path)
    env["PYTHONIOENCODING"] = "utf-8"
    env["PYTHONUTF8"] = "1"
    return env


def _run_cli(args, state_path):
    return subprocess.run(
        [sys.executable, "-m", "gateway.points", *args],
        cwd=str(SERVER),
        env=_cli_env(state_path),
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=120,
    )


# --------------------------------------------------------------------------- #
# 模块装载（缺失/不可导入 = 该行为的 RED，不是崩溃）                              #
# --------------------------------------------------------------------------- #

_points_cache = None
_front_cache = None


def _points_module():
    global _points_cache
    if _points_cache is not None:
        return _points_cache
    if not POINTS_PY.is_file():
        raise AssertionError("K3Q RED: server/gateway/points.py 不在世 —— K1 权威账本丢失。")
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    import gateway.points as pts  # noqa: PLC0415

    _points_cache = pts
    return pts


def _ensure_fastapi_stub() -> None:
    try:
        import fastapi  # noqa: F401

        if isinstance(getattr(fastapi, "FastAPI", None), type):
            return
    except ImportError:
        pass

    class _PermissiveModule(ModuleType):
        def __getattr__(self, name):  # noqa: D105
            return Mock()

    class _StubFastAPI:
        def __init__(self, *args, **kwargs):  # noqa: D107
            pass

        def _identity(self, *args, **kwargs):
            def deco(fn):
                return fn

            return deco

        get = post = websocket = _identity

    stub = _PermissiveModule("fastapi")
    stub.WebSocketDisconnect = type("WebSocketDisconnect", (Exception,), {})
    stub.HTTPException = type("HTTPException", (Exception,), {})
    stub.FastAPI = _StubFastAPI
    sys.modules["fastapi"] = stub
    if "fastapi.responses" not in sys.modules:
        sys.modules["fastapi.responses"] = _PermissiveModule("fastapi.responses")


def _front_module():
    global _front_cache
    if _front_cache is not None:
        return _front_cache
    if not FRONT_APP.is_file():
        raise AssertionError("K3Q RED: server/gateway/front/front_app.py 不在世。")
    _ensure_fastapi_stub()
    if str(SERVER) not in sys.path:
        sys.path.insert(0, str(SERVER))
    saved = os.environ.get(POINTS_STATE_ENV)
    os.environ[POINTS_STATE_ENV] = str(
        Path(tempfile.gettempdir()) / "kidpal_absent_points_state.json")
    try:
        spec = importlib.util.spec_from_file_location("kidpal_front_app_k3q", FRONT_APP)
        if spec is None or spec.loader is None:
            raise AssertionError("K3Q RED: front_app.py 不可装载")
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
    finally:
        if saved is None:
            os.environ.pop(POINTS_STATE_ENV, None)
        else:
            os.environ[POINTS_STATE_ENV] = saved
    _front_cache = mod
    return mod


@contextlib.contextmanager
def _front_points(front, state_path):
    old = getattr(front, "POINTS_STATE_PATH", None)
    front.POINTS_STATE_PATH = str(state_path)
    try:
        yield front
    finally:
        if old is None:
            front.__dict__.pop("POINTS_STATE_PATH", None)
        else:
            front.POINTS_STATE_PATH = old


def _manifest_flow_block(text: str, needle: str):
    m = re.search(r"\{[^{}]*?" + re.escape(needle) + r"[^{}]*?\}", text, re.DOTALL)
    return m.group(0) if m else None


# I/O doubles（K1/K2 冻结缝同款形态）                                            #


class _FakeDeviceSocket:
    def __init__(self):
        self.sent_text = []
        self.sent_bytes = []

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(data)


class _FakeEngineLeg:
    def __init__(self):
        self.sent_text = []
        self.sent_bytes = []

    async def send_text(self, text):
        self.sent_text.append(text)

    async def send_bytes(self, data):
        self.sent_bytes.append(data)


def _extract_display_envelope(obj):
    """在 wire 信封里定位同时含 view/data 的 dict（view/data 可能双 String）。"""
    if isinstance(obj, dict):
        if "view" in obj and "data" in obj:
            return obj
        for val in obj.values():
            found = _extract_display_envelope(val)
            if found is not None:
                return found
    elif isinstance(obj, list):
        for val in obj:
            found = _extract_display_envelope(val)
            if found is not None:
                return found
    return None


def _decode_frames(device) -> list:
    frames = []
    for text in device.sent_text:
        try:
            obj = json.loads(text)
        except (ValueError, TypeError):
            continue
        env = _extract_display_envelope(obj)
        if env is None:
            continue
        data = env.get("data")
        if isinstance(data, str):
            try:
                data = json.loads(data)
            except ValueError:
                data = {}
        frames.append((env.get("view"), data if isinstance(data, dict) else {}))
    return frames


def _drive_observe(front, device, engine) -> int:
    front._alert_legs.clear()
    front._alert_dispatcher = None
    front._register_alert_legs(device, engine)
    return asyncio.run(front.run_points_observe_once())


# =========================================================================== #
# K3Q-A1 — CLI 子命令面与逐字段回执                                             #
# =========================================================================== #


class K3qA1CliSurface(unittest.TestCase):
    maxDiff = None

    def test_k3q_a1_add_receipt_exact_fields(self):
        """coverage: K3Q-A1 / S-F6.4-AC1 —— add 回执逐字段精确（含 revision 与 command_id）。"""
        state = _new_state_path(self)
        cmd_id = "qq:20261114T101500:abc123"
        proc = _run_cli(
            ["add", "--points", "3", "--reason", "老师表扬", "--command-id", cmd_id], state)
        self.assertEqual(
            proc.returncode, 0,
            f"K3Q-A1 RED: add 应 exit 0；stdout={proc.stdout!r} stderr={proc.stderr!r}")
        r = _receipt(proc)
        self.assertTrue(str(r.get("entry_id", "")).startswith("pt_"),
                        f"K3Q-A1 RED: 回执缺 entry_id=pt_<hex>；receipt={r}")
        self.assertEqual(r.get("delta"), "+3", f"K3Q-A1 RED: delta 非 +3；receipt={r}")
        self.assertEqual(r.get("balance"), "3", f"K3Q-A1 RED: balance 非 3；receipt={r}")
        self.assertEqual(r.get("reason"), "老师表扬", f"K3Q-A1 RED: reason 失真；receipt={r}")
        self.assertEqual(r.get("revision"), "1", f"K3Q-A1 RED: 回执缺 revision=1；receipt={r}")
        self.assertEqual(r.get("command_id"), cmd_id,
                         f"K3Q-A1 RED: 回执缺/错 command_id；receipt={r}")
        st = _points_module().read_state(str(state))
        self.assertEqual((st["revision"], st["balance"], len(st["entries"])), (1, 3, 1))

    def test_k3q_a1_show_recent_targets_readbacks(self):
        """coverage: K3Q-A1 / S-F6.4-AC2 —— show / recent / targets 回答真实账本状态。"""
        state = _new_state_path(self)
        # 空账 show：只读、给出零态与出厂激活目标、不创建 state 文件
        proc = _run_cli(["show"], state)
        self.assertEqual(proc.returncode, 0, f"K3Q-A1 RED: show 应 exit 0；stderr={proc.stderr!r}")
        r = _receipt(proc)
        self.assertEqual(r.get("balance"), "0", f"K3Q-A1 RED: show balance 非 0；receipt={r}")
        self.assertEqual(r.get("revision"), "0", f"K3Q-A1 RED: show revision 非 0；receipt={r}")
        self.assertEqual(r.get("entries_total"), "0",
                         f"K3Q-A1 RED: show 缺 entries_total；receipt={r}")
        self.assertIn("preset-playground", r["_line"])
        self.assertIn("游乐场", r["_line"])
        self.assertIn("40", r["_line"])
        self.assertFalse(state.exists(), "K3Q-A1 RED: show 只读，不得创建/改写 state 文件")
        # targets：预置四目标只读列举
        proc = _run_cli(["targets"], state)
        self.assertEqual(proc.returncode, 0, f"K3Q-A1 RED: targets 应 exit 0；stderr={proc.stderr!r}")
        for needle in ("巧克力", "看电影", "游乐场", "礼物", "preset-playground"):
            self.assertIn(needle, proc.stdout,
                          f"K3Q-A1 RED: targets 缺 {needle!r}；stdout={proc.stdout!r}")
        self.assertFalse(state.exists(), "K3Q-A1 RED: targets 只读，不得创建 state 文件")
        # 两笔后 recent：时间倒序，index 1 = 最新
        self.assertEqual(_run_cli(
            ["add", "--points", "1", "--reason", "alpha", "--command-id", "c1"], state).returncode, 0)
        self.assertEqual(_run_cli(
            ["add", "--points", "2", "--reason", "beta", "--command-id", "c2"], state).returncode, 0)
        proc = _run_cli(["recent"], state)
        self.assertEqual(proc.returncode, 0, f"K3Q-A1 RED: recent 应 exit 0；stderr={proc.stderr!r}")
        lines = _receipt_lines(proc)
        self.assertEqual(len(lines), 2, f"K3Q-A1 RED: recent 默认应回 2 行；实得 {lines}")
        self.assertEqual(lines[0].get("reason"), "beta", f"K3Q-A1 RED: 首行非最新；{lines}")
        self.assertEqual(lines[1].get("reason"), "alpha", f"K3Q-A1 RED: 未按时间倒序；{lines}")
        self.assertEqual(lines[0].get("index"), "1", f"K3Q-A1 RED: index 起点非 1；{lines}")
        self.assertEqual(lines[1].get("index"), "2", f"K3Q-A1 RED: index 递增错；{lines}")

    def test_k3q_a1_redeem_active_target_gate_negative(self):
        """coverage: K3Q-A1 / S-F6.3-AC3、AC4 —— 兑换只认激活目标分值、落负向分录。"""
        # 金额不符 -> exit 1 零写入
        state = _new_state_path(self)
        self.assertEqual(_run_cli(
            ["add", "--points", "50", "--reason", "攒分", "--command-id", "fund1"], state).returncode, 0)
        before = state.read_bytes()
        bad = _run_cli(["redeem", "--points", "20", "--command-id", "rd-bad"], state)
        self.assertEqual(
            bad.returncode, 1,
            f"K3Q-A1 RED: 兑换金额不符激活目标(40)应 exit 1；stderr={bad.stderr!r}")
        self.assertTrue(bad.stderr.strip(), "K3Q-A1 RED: 拒绝说明应写 stderr")
        self.assertEqual(state.read_bytes(), before, "K3Q-A1 RED: 拒绝兑换不得改写 state")
        # 正确 40 -> 成功、负向分录、事由含激活目标名
        ok = _run_cli(["redeem", "--points", "40", "--command-id", "rd-ok"], state)
        self.assertEqual(ok.returncode, 0, f"K3Q-A1 RED: 正确兑换应 exit 0；stderr={ok.stderr!r}")
        r = _receipt(ok)
        self.assertEqual(r.get("delta"), "-40", f"K3Q-A1 RED: 兑换分录非负向；receipt={r}")
        self.assertEqual(r.get("balance"), "10", f"K3Q-A1 RED: 兑换后余额应 10；receipt={r}")
        self.assertEqual(r.get("revision"), "2", f"K3Q-A1 RED: 兑换 revision 应 2；receipt={r}")
        self.assertIn("游乐场", r.get("reason", ""),
                      f"K3Q-A1 RED: 兑换事由应取「兑换<激活目标名>」；receipt={r}")
        # 余额不足 -> exit 1 零写入（新账本，避免短窗去重先命中）
        state2 = _new_state_path(self)
        self.assertEqual(_run_cli(
            ["add", "--points", "10", "--reason", "零钱", "--command-id", "small"], state2).returncode, 0)
        before2 = state2.read_bytes()
        short = _run_cli(["redeem", "--points", "40", "--command-id", "rd-short"], state2)
        self.assertEqual(
            short.returncode, 1,
            f"K3Q-A1 RED: 余额不足兑换应 exit 1；stderr={short.stderr!r}")
        self.assertEqual(state2.read_bytes(), before2, "K3Q-A1 RED: 余额不足不得改写 state")

    def test_k3q_a1_usage_and_env_errors_exit_2(self):
        """coverage: K3Q-A1 / S-F6.4-AC4 —— 用法/环境错误 exit 2（非语义拒绝）。"""
        state = _new_state_path(self)
        cases = [
            [],                                                       # 无子命令
            ["frobnicate"],                                           # 未知子命令
            ["add", "--points", "3"],                                 # 缺 --reason/--command-id
            ["show", "extra"],                                        # show 多余参数
        ]
        for args in cases:
            proc = _run_cli(args, state)
            self.assertEqual(
                proc.returncode, 2,
                f"K3Q-A1 RED: 用法错误 {args} 应 exit 2；实得 {proc.returncode} "
                f"stderr={proc.stderr!r}")
        # KIDPAL_POINTS_STATE 缺失 -> exit 2，stderr 点名 env
        proc = _run_cli(["show"], None)
        self.assertEqual(proc.returncode, 2,
                         f"K3Q-A1 RED: 缺 KIDPAL_POINTS_STATE 应 exit 2；stderr={proc.stderr!r}")
        self.assertIn(POINTS_STATE_ENV, proc.stderr,
                      f"K3Q-A1 RED: 缺 env 说明须点名 {POINTS_STATE_ENV}；stderr={proc.stderr!r}")


# =========================================================================== #
# K3Q-A2 — 拒绝不伪造成功                                                       #
# =========================================================================== #


class K3qA2RejectNotFake(unittest.TestCase):
    maxDiff = None

    def _seeded(self):
        state = _new_state_path(self)
        _points_module().add_points(str(state), 5, "seed")
        return state, state.read_bytes()

    def test_k3q_a2_invalid_values_exit_1_zero_write(self):
        """coverage: K3Q-A2 / S-F6.4-AC4 —— 非法输入 exit 1、stderr 说明、stdout 无成功回执、state 字节不变。"""
        cases = [
            ["add", "--points", "0", "--reason", "x", "--command-id", "c0"],
            ["add", "--points", "-3", "--reason", "x", "--command-id", "cneg"],
            ["add", "--points", "3", "--reason", "", "--command-id", "cempty"],
            ["add", "--points", "abc", "--reason", "x", "--command-id", "cabc"],
        ]
        for args in cases:
            state, before = self._seeded()
            proc = _run_cli(args, state)
            self.assertEqual(
                proc.returncode, 1,
                f"K3Q-A2 RED: 非法值 {args} 应 exit 1；实得 {proc.returncode} "
                f"stderr={proc.stderr!r}")
            self.assertTrue(proc.stderr.strip(), f"K3Q-A2 RED: {args} 拒绝说明应写 stderr")
            self.assertNotIn("entry_id=", proc.stdout,
                             f"K3Q-A2 RED: {args} 拒绝却输出成功回执；stdout={proc.stdout!r}")
            self.assertEqual(state.read_bytes(), before,
                             f"K3Q-A2 RED: {args} 拒绝不得改写 state（零写入）")

    def test_k3q_a2_ambiguous_delete_locator_zero_write(self):
        """coverage: K3Q-A2 / S-F6.4-AC4 —— 删除定位缺失/歧义/越界/畸形 -> exit 1 一次性普通文本、零写入。"""
        state, before = self._seeded()
        strict_cases = [
            ["delete", "--command-id", "d0"],                                    # 都缺
            ["delete", "--entry-id", ABSENT_ENTRY_ID, "--index", "1",
             "--command-id", "d1"],                                              # 都给
            ["delete", "--index", "0", "--command-id", "d2"],                    # 非正整数
            ["delete", "--index", "abc", "--command-id", "d3"],                  # 非整数
            ["delete", "--index", "9", "--command-id", "d4"],                    # 越界（仅 1 条）
            ["delete", "--entry-id", "   ", "--command-id", "d5"],               # 空白 id
            ["delete", "--entry-id", "hello", "--command-id", "d6"],             # 畸形（无 pt_ 前缀）
        ]
        for args in strict_cases:
            proc = _run_cli(args, state)
            self.assertEqual(
                proc.returncode, 1,
                f"K3Q-A2 RED: 歧义/畸形删除 {args} 应 exit 1；实得 {proc.returncode} "
                f"stdout={proc.stdout!r} stderr={proc.stderr!r}")
            self.assertTrue(proc.stderr.strip(), f"K3Q-A2 RED: {args} 应一次性普通文本报错")
            self.assertNotIn("deleted=true", proc.stdout,
                             f"K3Q-A2 RED: {args} 拒绝却回成功删除；stdout={proc.stdout!r}")
            self.assertEqual(state.read_bytes(), before, f"K3Q-A2 RED: {args} 零写入")
        # 按金额/事由定位不受支持（用法/语义错误皆可）——只要求失败 + 零写入
        proc = _run_cli(["delete", "--amount", "5", "--command-id", "d7"], state)
        self.assertNotEqual(proc.returncode, 0,
                            f"K3Q-A2 RED: 按金额定位应失败；stdout={proc.stdout!r}")
        self.assertEqual(state.read_bytes(), before, "K3Q-A2 RED: 按金额定位零写入")


# =========================================================================== #
# K3Q-A3 — 幂等（传输 + 短窗）                                                  #
# =========================================================================== #


class K3qA3Idempotency(unittest.TestCase):
    maxDiff = None

    def test_k3q_a3_command_id_replay_returns_first_result(self):
        """coverage: K3Q-A3 / S-F6.3-AC5、S-F6.4-AC1 —— 同 command_id 重放返首次结果、不新增分录、revision 不变。"""
        state = _new_state_path(self)
        cmd = ["add", "--points", "4", "--reason", "洗碗",
               "--command-id", "qq:20261114T090000:h7"]
        p1 = _run_cli(cmd, state)
        self.assertEqual(p1.returncode, 0, f"K3Q-A3 RED: 首次 add 应 exit 0；stderr={p1.stderr!r}")
        r1 = _receipt(p1)
        p2 = _run_cli(cmd, state)
        self.assertEqual(p2.returncode, 0, f"K3Q-A3 RED: 重放应 exit 0；stderr={p2.stderr!r}")
        r2 = _receipt(p2)
        self.assertEqual(r2.get("entry_id"), r1.get("entry_id"),
                         f"K3Q-A3 RED: 重放须返首次 entry_id；r1={r1} r2={r2}")
        self.assertEqual(r2.get("revision"), r1.get("revision"),
                         f"K3Q-A3 RED: 重放 revision 须不变；r1={r1} r2={r2}")
        self.assertEqual(r2.get("balance"), r1.get("balance"))
        st = _points_module().read_state(str(state))
        self.assertEqual(len(st["entries"]), 1, "K3Q-A3 RED: 重放不得新增分录")
        self.assertEqual(st["revision"], 1, "K3Q-A3 RED: 重放不得递增 revision")

    def test_k3q_a3_short_window_duplicate_receipt(self):
        """coverage: K3Q-A3 / S-F6.3-AC5 —— 不同 command_id 同分值同事由短窗重复 -> duplicate=true 不新增。"""
        state = _new_state_path(self)
        p1 = _run_cli(["add", "--points", "3", "--reason", "扫地",
                       "--command-id", "qq:20261114T090100:a"], state)
        self.assertEqual(p1.returncode, 0, f"K3Q-A3 RED: 首笔应 exit 0；stderr={p1.stderr!r}")
        p2 = _run_cli(["add", "--points", "3", "--reason", "扫地",
                       "--command-id", "qq:20261114T090500:b"], state)
        self.assertEqual(p2.returncode, 0, f"K3Q-A3 RED: 短窗重复应 exit 0（回执而非报错）；stderr={p2.stderr!r}")
        r2 = _receipt(p2)
        self.assertEqual(r2.get("duplicate"), "true",
                         f"K3Q-A3 RED: 短窗重复应 duplicate=true；receipt={r2}")
        st = _points_module().read_state(str(state))
        self.assertEqual(len(st["entries"]), 1, "K3Q-A3 RED: 短窗重复不得记第二条")
        self.assertEqual(st["revision"], 1, "K3Q-A3 RED: 短窗重复不得递增 revision")


# =========================================================================== #
# K3Q-A4 — 稳定定位删除                                                         #
# =========================================================================== #


class K3qA4StableDelete(unittest.TestCase):
    maxDiff = None

    def _seed_three(self, state):
        for points, reason, cid in ((1, "alpha", "c1"), (2, "beta", "c2"), (3, "gamma", "c3")):
            proc = _run_cli(
                ["add", "--points", str(points), "--reason", reason, "--command-id", cid], state)
            self.assertEqual(proc.returncode, 0,
                             f"K3Q-A4 RED: 种分录失败 {reason}；stderr={proc.stderr!r}")
        return _points_module().read_state(str(state))

    def test_k3q_a4_recent_index_entry_id_same_line(self):
        """coverage: K3Q-A4 / S-F6.4-AC3 —— recent 回执 index 与 entry_id 同行、index 1 最新。"""
        state = _new_state_path(self)
        self._seed_three(state)
        proc = _run_cli(["recent", "--limit", "3"], state)
        self.assertEqual(proc.returncode, 0, f"K3Q-A4 RED: recent 应 exit 0；stderr={proc.stderr!r}")
        lines = _receipt_lines(proc)
        self.assertEqual(len(lines), 3, f"K3Q-A4 RED: recent --limit 3 应回 3 行；{lines}")
        for ln in lines:
            self.assertRegex(ln["_line"], r"\bindex=\d+\b",
                             f"K3Q-A4 RED: 行内缺 index；line={ln['_line']!r}")
            self.assertRegex(ln["_line"], r"\bentry_id=pt_[0-9a-f]+\b",
                             f"K3Q-A4 RED: index 与 entry_id 须同行且 id 形状合法；line={ln['_line']!r}")
        self.assertEqual(lines[0].get("reason"), "gamma", f"K3Q-A4 RED: 首行非最新；{lines}")
        self.assertEqual(lines[0].get("index"), "1")
        self.assertEqual(lines[2].get("reason"), "alpha", f"K3Q-A4 RED: 末行非最早；{lines}")
        self.assertEqual(lines[2].get("index"), "3")

    def test_k3q_a4_delete_by_index_exact_middle(self):
        """coverage: K3Q-A4 / S-F6.4-AC3、S-F6.2-AC4 —— delete --index 2 恰删时间倒序中间那条并重算余额。"""
        state = _new_state_path(self)
        st0 = self._seed_three(state)
        recent = _receipt_lines(_run_cli(["recent", "--limit", "3"], state))
        self.assertEqual(recent[1].get("reason"), "beta",
                         f"K3Q-A4 前置破坏：中间条非 beta；{recent}")
        middle_id = recent[1].get("entry_id")
        keep_ids = {recent[0].get("entry_id"), recent[2].get("entry_id")}
        proc = _run_cli(["delete", "--index", "2", "--command-id", "del2"], state)
        self.assertEqual(proc.returncode, 0, f"K3Q-A4 RED: delete --index 应 exit 0；stderr={proc.stderr!r}")
        r = _receipt(proc)
        self.assertEqual(r.get("deleted"), "true", f"K3Q-A4 RED: 未标记删除成功；receipt={r}")
        self.assertEqual(r.get("entry_id"), middle_id,
                         f"K3Q-A4 RED: delete --index 2 须删 recent index 2 那条；receipt={r}")
        st1 = _points_module().read_state(str(state))
        ids = {e["id"] for e in st1["entries"]}
        self.assertNotIn(middle_id, ids, "K3Q-A4 RED: 中间条未删")
        self.assertEqual(ids, keep_ids, "K3Q-A4 RED: 其余分录须保留不清零")
        self.assertEqual(st1["balance"], 4, "K3Q-A4 RED: +1/+3 剩余 -> 余额 4")
        self.assertEqual(st1["revision"], st0["revision"] + 1, "K3Q-A4 RED: 删除 revision 应 +1")
        ordered = st1["entries"]  # 时间倒序：gamma(+3) 在 alpha(+1) 前
        self.assertEqual(ordered[0]["delta"], 3)
        self.assertEqual((ordered[0]["balance_before"], ordered[0]["balance_after"]), (1, 4),
                         "K3Q-A4 RED: 删除后逐条前后余额链须按时间序重算")
        self.assertEqual((ordered[1]["balance_before"], ordered[1]["balance_after"]), (0, 1))

    def test_k3q_a4_delete_by_entry_id_and_oor(self):
        """coverage: K3Q-A4 / S-F6.4-AC3 —— entry-id 删除 / 不存在幂等 no-op / 越界零写入。"""
        state = _new_state_path(self)
        self._seed_three(state)
        recent = _receipt_lines(_run_cli(["recent", "--limit", "3"], state))
        newest_id = recent[0].get("entry_id")
        proc = _run_cli(["delete", "--entry-id", newest_id, "--command-id", "delid"], state)
        self.assertEqual(proc.returncode, 0, f"K3Q-A4 RED: delete --entry-id 应 exit 0；stderr={proc.stderr!r}")
        self.assertEqual(_receipt(proc).get("deleted"), "true")
        st1 = _points_module().read_state(str(state))
        self.assertNotIn(newest_id, {e["id"] for e in st1["entries"]})
        self.assertEqual(len(st1["entries"]), 2)
        # 形状合法但不存在 -> 幂等 no-op exit 0、deleted=false、零写入
        before = state.read_bytes()
        noop = _run_cli(["delete", "--entry-id", ABSENT_ENTRY_ID, "--command-id", "delnoop"], state)
        self.assertEqual(noop.returncode, 0,
                         f"K3Q-A4 RED: 不存在但形状合法 id 应幂等 no-op exit 0；stderr={noop.stderr!r}")
        self.assertEqual(_receipt(noop).get("deleted"), "false",
                         f"K3Q-A4 RED: no-op 须 deleted=false；receipt={_receipt(noop)}")
        self.assertEqual(state.read_bytes(), before, "K3Q-A4 RED: no-op 不得写账")
        # 越界 -> exit 1 零写入
        before2 = state.read_bytes()
        oor = _run_cli(["delete", "--index", "9", "--command-id", "deloor"], state)
        self.assertEqual(oor.returncode, 1, f"K3Q-A4 RED: 越界删除应 exit 1；stderr={oor.stderr!r}")
        self.assertEqual(state.read_bytes(), before2, "K3Q-A4 RED: 越界删除零写入")

    def test_k3q_a4_index_resolution_locked_source_anchor(self):
        """coverage: K3Q-A4 / S-F6.4-AC3 —— 序号→id 换算与删除同 _critical 临界区（源锚代理）。

        plan 技术方案要求拆出内部 `_delete_entry_locked(...)`、公开 `delete_entry` 转调，
        否则「先读序号再删」之间存 TOCTOU 窗口会让序号静默漂移、删错账。本断言为源锚代理
        （弱于行为级）：钉该结构在 points.py 内存在，具体实现归 builder。
        """
        pts = _points_module()
        src = inspect.getsource(pts)
        self.assertIn(
            "_delete_entry_locked", src,
            "K3Q-A4 RED: points.py 未拆出 `_delete_entry_locked(...)` —— 序号→id 换算须与删除"
            "在同一 _critical 临界区内一次做完（plan 技术方案；防 TOCTOU 删错账）。")
        self.assertIn("def delete_entry", src, "K3Q-A4 RED: 公开 delete_entry 转调缺失")


# =========================================================================== #
# K3Q-A5 — SKILL.md 静态纪律与负向断言                                          #
# =========================================================================== #


class K3qA5SkillDiscipline(unittest.TestCase):
    maxDiff = None

    def setUp(self):
        self.assertTrue(
            POINTS_SKILL_PATH.is_file(),
            f"K3Q-A5 RED: kidpal-points-write SKILL.md 不在位 —— 要求 {POINTS_SKILL_PATH}。"
            "缺它 = QQ 家长积分通道无纪律护栏（模型可能自行吞重 / 直写 state / 多轮追问卡死）。")
        self.text = POINTS_SKILL_PATH.read_text(encoding="utf-8")
        self.assertTrue(self.text.strip(), f"K3Q-A5 RED: {POINTS_SKILL_PATH} 为空。")

    def test_k3q_a5_pins_machine_surface(self):
        """coverage: K3Q-A5 / S-F6.4-AC1、AC2 —— 钉死解释器 / state env / 调用形 / 全子命令面。"""
        pairs = (
            ("钉死解释器", PINNED_INTERPRETER),
            ("state env 名", POINTS_STATE_ENV),
            ("-m gateway.points 调用形", "-m gateway.points"),
            ("add 子命令", "add"),
            ("redeem 子命令", "redeem"),
            ("delete 子命令", "delete"),
            ("show 子命令", "show"),
            ("recent 子命令", "recent"),
            ("targets 子命令", "targets"),
            ("稳定定位 --entry-id", "--entry-id"),
            ("稳定定位 --index", "--index"),
            ("传输幂等 command_id", "command_id"),
            ("权威版本 revision", "revision"),
        )
        missing = [label for label, needle in pairs if needle not in self.text]
        self.assertEqual(
            missing, [],
            f"K3Q-A5 RED: skill 缺机器面字形 —— {missing}。缺它 = 模型无法按确定性 CLI 面执行。")

    def test_k3q_a5_negative_ask_user_absent(self):
        """coverage: K3Q-A5 / S-F6.4-AC4、TECH-261030 —— 不得出现 ask_user（会卡死会话）。"""
        self.assertNotIn(
            "ask_user", self.text,
            "K3Q-A5 RED: skill 出现 ask_user —— QQ 插件 ask_user 会卡死会话（TECH-261030）；"
            "积分通道禁用追问卡死，歧义必须一次性普通文本报错。")

    def test_k3q_a5_forbids_state_edit_schedule_residual(self):
        """coverage: K3Q-A5 / S-F6.4-AC1、TECH-261012 —— 禁直写 state.json / 无 schedule 残留 / workspace-kidpal 只读。"""
        write_forms = [
            r">\s*[^\n]{0,40}points/state\.json",
            r"json\.dump\([^\n]{0,60}state",
            r"open\([^\n]{0,60}state\.json[^\n]{0,20}['\"]w",
            r"Path\([^\n]{0,80}state\.json[^\n]{0,40}\)\.write_",
            r"(编辑|修改|写入|覆盖)[^\n]{0,20}/opt/kidpal/points/state\.json",
        ]
        hits = [p for p in write_forms if re.search(p, self.text)]
        self.assertEqual(
            hits, [],
            f"K3Q-A5 RED: skill 含直接写 state.json 的指示（{hits}）—— 唯一 writer 是 points.py CLI，"
            "skill 不得直接编辑/写 /opt/kidpal/points/state.json（模型只读防线）。")
        for residual in ("parse_schedule_md", "group_upcoming",
                         "reminder_schedule_lib.py", "spec_from_file_location"):
            self.assertNotIn(
                residual, self.text,
                f"K3Q-A5 RED: skill 残留课表 parser 机检段 token {residual!r} —— 积分 skill 不得照抄课表写后机检。")
        self.assertIsNone(
            re.search(r">\s*[^\n]{0,60}schedule\.md", self.text),
            "K3Q-A5 RED: skill 残留 schedule.md 写路径（重定向写入）。")
        self.assertIn(
            "workspace-kidpal", self.text,
            "K3Q-A5 RED: skill 未声明 workspace-kidpal 边界（板侧只读零写权 = K2 F1 既定结论）。")
        self.assertTrue(
            re.search(r"(只读|不可写|不要写|不得写|禁止|不碰|零写权)", self.text),
            "K3Q-A5 RED: skill 未明文 workspace-kidpal/数据目录只读边界。")

    def test_k3q_a5_pins_always_call_cli_oneshot_error(self):
        """coverage: K3Q-A5 / S-F6.4-AC1、AC4 —— 恒调 CLI 不自行吞重；歧义一次性普通文本报错不追问。"""
        always_cli = (
            re.search(r"(恒|总是|一律|每次|必须|永远)[^\n]{0,24}(调|调用|执行)[^\n]{0,16}(CLI|points)", self.text)
            or re.search(r"(不|禁止|不得|永不)[^\n]{0,24}(自行|自己)?[^\n]{0,12}(判断|吞|跳过|省略|去重)", self.text)
        )
        self.assertTrue(
            always_cli,
            "K3Q-A5 RED: skill 未写死「恒调 CLI、永不自行判断像重复就不调」—— 传输幂等/短窗去重"
            "权威在 K1 CLI，skill 自行吞重会让重复成为不可发现的事实。")
        oneshot = (
            re.search(r"(普通|纯)?文本[^\n]{0,12}(回复|报错|说明)", self.text)
            or re.search(r"一次性[^\n]{0,24}(可判|报错|说明|回复)", self.text)
            or re.search(r"(不|禁止|不得)[^\n]{0,16}(追问|多轮|澄清)", self.text)
        )
        self.assertTrue(
            oneshot,
            "K3Q-A5 RED: skill 未明文「歧义一次性普通文本报错、不依赖多轮澄清问答」。")


# =========================================================================== #
# K3Q-A6 — 离线串链（CLI 写入 -> front 观察缝推送）                              #
# =========================================================================== #


class K3qA6OfflineChain(unittest.TestCase):
    maxDiff = None

    def tearDown(self):
        try:
            _front_module()._alert_legs.clear()
        except Exception:  # noqa: BLE001
            pass

    def test_k3q_a6_cli_write_single_restore_push(self):
        """coverage: K3Q-A6 / S-F6.4-AC5、NFR-PERF-4 —— CLI 写入经观察缝恰推一帧 restore、revision 权威。"""
        front = _front_module()
        state = _new_state_path(self)
        device, engine = _FakeDeviceSocket(), _FakeEngineLeg()
        with _front_points(front, state):
            pushed0 = _drive_observe(front, device, engine)
            self.assertEqual(pushed0, 0, "K3Q-A6 前置：首次观察应只建基线、推 0 帧")
            self.assertEqual(_decode_frames(device), [], "K3Q-A6 前置：首次观察不应推帧")
            proc = _run_cli(
                ["add", "--points", "7", "--reason", "跑步", "--command-id", "qq:20261114T120000:zz"],
                state)
            self.assertEqual(proc.returncode, 0, f"K3Q-A6 RED: CLI add 应成功；stderr={proc.stderr!r}")
            pushed1 = asyncio.run(front.run_points_observe_once())
            self.assertEqual(pushed1, 1, "K3Q-A6 RED: QQ/CLI 写入经观察缝应恰推 1 个连接")
            frames = _decode_frames(device)
            self.assertEqual(len(frames), 1,
                             f"K3Q-A6 RED: 应恰推一帧；实得 {len(frames)} 帧：{device.sent_text}")
            view, data = frames[0]
            self.assertEqual(view, "points_home",
                             f"K3Q-A6 RED: 应推 points_home；实得 view={view!r}")
            self.assertIs(data.get("restore"), True,
                          f"K3Q-A6 RED: QQ 到账须 data.restore=true 不抢前台（S-F6.4-AC5）；data={data}")
            self.assertEqual(data.get("revision"), 1,
                             f"K3Q-A6 RED: 推送 revision 应为权威值 1；data={data}")
            self.assertEqual(data.get("balance"), 7,
                             f"K3Q-A6 RED: 推送 balance 应为权威值 7；data={data}")

    def test_k3q_a6_delete_noop_does_not_push(self):
        """coverage: K3Q-A6 / S-F6.4-AC5 —— 删除 no-op（不存在 id）不写账、不推帧。"""
        front = _front_module()
        state = _new_state_path(self)
        _points_module().add_points(str(state), 2, "seed")
        device, engine = _FakeDeviceSocket(), _FakeEngineLeg()
        with _front_points(front, state):
            pushed0 = _drive_observe(front, device, engine)
            self.assertEqual(pushed0, 0, "K3Q-A6 前置：基线观察应 0 帧")
            noop = _run_cli(
                ["delete", "--entry-id", ABSENT_ENTRY_ID, "--command-id", "delnoop"], state)
            self.assertEqual(noop.returncode, 0,
                             f"K3Q-A6 RED: 不存在 id 应幂等 no-op exit 0；stderr={noop.stderr!r}")
            self.assertEqual(_receipt(noop).get("deleted"), "false",
                             f"K3Q-A6 RED: no-op 须 deleted=false；receipt={_receipt(noop)}")
            pushed1 = asyncio.run(front.run_points_observe_once())
            self.assertEqual(pushed1, 0,
                             "K3Q-A6 RED: no-op 未改 revision/分录数，不应推帧")
            self.assertEqual(_decode_frames(device), [], "K3Q-A6 RED: no-op 不得推帧")


# =========================================================================== #
# K3Q-A7 — 部署三面                                                             #
# =========================================================================== #


class K3qA7Deploy(unittest.TestCase):
    maxDiff = None

    def test_k3q_a7_manifest_registers_points_skill(self):
        """coverage: K3Q-A7 / S-F6.4-AC1 —— manifest 登记 kidpal-points-write src/dst 对（部署生效前置）。"""
        self.assertTrue(MANIFEST.is_file(), f"K3Q-A7 RED: {MANIFEST} 不在位。")
        text = MANIFEST.read_text(encoding="utf-8")
        block = _manifest_flow_block(text, POINTS_SKILL_SRC)
        self.assertIsNotNone(
            block,
            f"K3Q-A7 RED: manifest 无 kidpal-points-write 条目（src {POINTS_SKILL_SRC!r}）。"
            "缺条目 = verify-deploy.sh 不铺放该 skill、live 无正式积分通道。")
        self.assertIn(POINTS_SKILL_DST, block,
                      f"K3Q-A7 RED: 条目 dst 非 {POINTS_SKILL_DST!r}；block={block!r}")
        for field in ("owner: deploy.sh", "verify: md5", "restart: none"):
            self.assertIn(field, block, f"K3Q-A7 RED: 条目缺字段 {field!r}；block={block!r}")
        self.assertTrue(POINTS_SKILL_PATH.is_file(),
                        f"K3Q-A7 RED: manifest 引用的 src 不存在 {POINTS_SKILL_PATH}")

    def test_k3q_a7_deploy_verify_manifest_driven(self):
        """coverage: K3Q-A7 —— deploy.sh / verify-deploy.sh 表驱动（must-stay-green 回归；改 manifest 即生效）。"""
        for path in (DEPLOY_SH, VERIFY_SH):
            self.assertTrue(path.is_file(), f"K3Q-A7 RED: {path} 不在位")
            text = path.read_text(encoding="utf-8", errors="replace")
            self.assertIn("manifest.yaml", text,
                          f"K3Q-A7 RED: {path.name} 须从 manifest.yaml 读部署面")
            self.assertIn("entries", text,
                          f"K3Q-A7 RED: {path.name} 须按 manifest entries 表驱动（零脚本改动即生效）")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)