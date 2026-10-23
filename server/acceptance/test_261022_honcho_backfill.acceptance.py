"""Frozen acceptance tests — TaskPackage 261022-m5-k4-history-corpus-backfill
（K4-E1..E6 host 面；rigor=full；riskTier=high；gateProfile=[migration, security,
real-db]；subsystem = server/deploy/honcho/backfill（历史 session-corpus → Honcho
记忆库迁移：照记全量、幂等重跑、前后计数报告、清库回滚）。姊妹件 =
test_261022_honcho_memory.acceptance.py（K3 栈生产化）与
test_261022_memory_isolation.acceptance.py（插件接线 / 资格门口径换代）。）

上游 = docs/requirements.md S-F5.1-AC5（历史照记：旧内容与新聊天一视同仁）+
docs/architecture.md §6.7「历史 backfill」行（2026-09-30 前 session-corpus 全量
照记；幂等可重跑、前后计数报告、清库回滚）+ workstream PROPOSAL §3.1 R6 /
§3.2 OQ-3（human 2026-10-22 终拍：不区分回合性质、含第三方姓名、不带 PII
过滤器）+ arch review PROPOSAL T4（历史照记豁免 vs 新增回合资格门两条口径
不互串）+ ADR-016（2026-10-23 面 C，supersedes ADR-014 ②③：设备级 session
键族 agent:kidpal:openai-user:<device_id> + 家庭单 owner 画像）+ OQ-10（备份 =
不备份靠重建，重建源 = session-corpus 源语料 + OpenClaw 会话存储，backfill 即
重建路径）+ NFR-SEC-2 ⑤（出网两条记账）+ NFR-PRIV-1（删除边界三保留）。

背景（TECH-261022 第 4 包）：Honcho 生产接线已由 K3/批 B 落地（插件
@honcho-ai/openclaw-honcho@1.7.0 在写，live ~30 条消息），但历史语料仍在
session-corpus 里不在画像中——孩子 9-30 讲过的「王一诺」等事实需一次性补录
才有跨会话回忆价值。本包把迁移工具落盘 server/deploy/honcho/backfill/**
（K3 期 A4 禁区已于 2026-10-23 显式解除，K3 冻结件同步锚换代）；执行面归
human（ECS 全量跑），Pod 只建工具与离线契约。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O；stdlib-only）

  A) 资产落位（server/deploy/honcho/backfill/**，恰两件工具，stdlib-only、
     可被 importlib 按路径导入、模块顶层零副作用零网络 I/O）:

       · backfill_honcho.py —— 迁移 runner；
       · rollback_honcho_backfill.py —— 清库回滚 runner。

  B) runner 模块冻结接缝（离线桩真跑契约的注入面，同族先例 =
     test_261022_memory_isolation 的 --openclaw-bin 桩纪律）:

       · 模块级常量：DEFAULT_API = "http://127.0.0.1:18100"（Honcho loopback，
         arch §1.2 / K3 消费位）；SESSION_BATCH_KEY = "kidpalBackfillBatch"；
         MESSAGE_FP_KEY = "kidpalBackfillFingerprint"；
         SOURCE_KINDS 涵盖 {"session-corpus", "openclaw-transcript"}（OQ-10
         重建双源）。
       · 模块级 `build_client(api: str)` —— 恰一位置参，返回实现下列鸭型
         协议的客户端（真实现 = Honcho REST 适配；验收以记录桩注入）:
             snapshot_counts() -> {"peers","sessions","messages","observations"}
                                  四 int 键
             list_backfill_batches() -> set[str]   # session metadata 既有 batch 标记
             list_fingerprints() -> set[str]       # message metadata 既有指纹
             create_session(peer, session_key, metadata) -> None
             add_message(session_key, role, content, metadata) -> None
             delete_sessions(session_keys) -> int
       · 模块级 `main(argv: list | None = None) -> int` —— 经模块级
         build_client 取客户端（可注入）；exit 0 = 成功、2 = 用法错
         （缺 --device-id 或零数据源；不得裸异常崩溃）。
       · CLI（argparse 形态自由、旗标语义冻结）:
             --corpus-dir <dir>   session-corpus 源语料目录（只读）
             --openclaw-db <path> kidpal agent sqlite（transcript_events，只读）
             --api <url>          缺省 DEFAULT_API
             --device-id <id>     设备键族归属（必填）
             --peer <name>        缺省 "owner"（家庭单 owner peer，ADR-016）
             --report-out <path>  计数报告 JSON 落盘（K4-E3）
             --apply              显式才写；缺省 = plan 干跑（零写副作用）
         两源至少其一；负锚：零过滤类旗标（--redact/--mask/--anonymize/
         --pii*/--filter-* 不在场——照记全量，不带 PII 过滤器，human 终拍）。

  C) 源格式冻结（host 无 ECS 真语料样本，host 面钉最小可测格式；真语料
     适配 = live 轮计数报告对账承载，见缺口申报）:

       · session-corpus 件：逐非空行一条消息，行格式 `[<role>] <content>`；
         **未识别行不得静默丢弃**——兜底收编为一条消息，并在计数报告
         backfill.unparsed 披露计数（照记全量、零丢弃）。
       · transcript_events：sqlite 表 `transcript_events`，至少列
         id / role / content（列超集容忍；缺表缺列 = 红）。
       · **零过滤**：解析出的每一行/每行事件都必须恰好产生一条计划消息
         （或幂等跳过计数）——不分回合性质（[front] 类旧设备流程回合行照记）、
         不滤第三方姓名（S-F5.1-AC5 / R6 human 终拍）。

  D) K4-E2 身份口径（ADR-016 现行口径，勿用拆包期「设备级 peer」旧表述）:

       · 灌入 session key 全部落在既有设备 session 键族：前缀恰
         `agent:kidpal:openai-user:<device_id>`（后缀自由，建议含日期/batch）。
       · 目标 peer 恒 = 家庭单 owner（缺省 "owner"）；**零新 peer 创建**
         （尤其零设备名 peer / 零多孩子身份）；换设备 = 新 session 家族、
         画像不变（记录为已知后果，arch §1.3）。

  E) K4-E3 幂等 + 与 live 共生（notes ④ 判据定形）:

       · 指纹标记：每条灌入消息 metadata 携带 MESSAGE_FP_KEY =
         `v1:<source-kind>:<sha256hex>`（64 hex 小写）；source-kind ∈
         SOURCE_KINDS；指纹 = 源锚身份（源件 + 行/事件序 + 内容），同一源
         重跑指纹确定不变。
       · session 标记：backfill 自建 session metadata 携带 SESSION_BATCH_KEY
         （值 = run batch 标记，形态自由）。
       · 幂等判据：写入前先取既有指纹集合，命中即跳过（重复执行零重复
         observation / 零语义变更——二次 apply 恰 ingested=0、skipped=全量）。
       · **已灌 vs live 新增的区分**：live 插件写的消息/session 无 backfill
         标记 → 永不进指纹集合、永不被判已灌、永不被回滚触碰；backfill
         零写 live 在用 session。
       · 计数报告 JSON（--report-out，plan/apply 两态都落盘）顶层键冻结：
             {"mode": "plan"|"apply",
              "peers": {"before": int, "after": int},
              "sessions": {"before": int, "after": int},
              "messages": {"before": int, "after": int},
              "observations": {"before": int, "after": int},
              "backfill": {"ingested": int, "skipped": int, "unparsed": int,
                           "perSource": {...}}}
         （observations 计数 = 写入时点快照；deriver 异步增量在 live 轮
         对账——K4-E6。）

  F) K4-E4 清库回滚（rollback_honcho_backfill.py）:

       · CLI：--api / --peer / --report-out / --apply（缺省 = plan 预演，
         零删除；显式 --apply 才删）；**零 --corpus-dir / --openclaw-db
         旗标**——回滚面只在记忆库侧。
       · 删除范围 = 恰携带 SESSION_BATCH_KEY 标记的 session（连带其消息与
         派生 observation，cascade 归 Honcho 服务端语义）；无标记数据
         （live 在用 session / 消息）零触碰。
       · 回滚报告同 E 计数 schema，backfill 块含 "deleted" 计数。
       · 负锚（NFR-PRIV-1 三保留）：回滚不删 OpenClaw 完整日志、不删
         session-corpus 源语料、不删设备诊断日志；源语料只读零改写
         （runner 同锚——corpus/db 文件在 plan/apply 全程字节不变）。

  G) K4-E5 资格门不互串（T4 / decisionCoverage「记忆口径分层」）:

       · backfill 属「历史照记」显式豁免——runner 头注声明豁免依据（AC5
         历史照记、不分回合性质）；
       · backfill 树零资格门配置引用（noisePatterns / captureSystemRuns
         零出现）——豁免路径不得复用/回改 P3 插件工作台的资格门判据，
         新增回合仍走资格门（P3 声明单源不动，must-stay-green）。

  H) 登记与核验（261013 纪律，manifest / verify-deploy 与 P1/P3 串行共享）:

       · server/deploy/manifest.yaml：backfill 资产登记条目 ≥2（两工具
         全覆盖），src 在仓内存在、dst 在 /opt/honcho/backfill/ 消费位、
         verify 字段在场。
       · server/deploy/verify-deploy.sh：新增 backfill 资产核验段；P3 锚
         （127.0.0.1:18100）与 deploy-drift 告警面不回退（must-stay-green）。
       · 安全（gateProfile security）：backfill 树零内联 secret（sk- 长键
         = 红）、零记账外外部端点（负锚词表 K4_FORBIDDEN_HOSTS，同 K3
         家族）、出网只经 --api loopback（NFR-SEC-2 ⑤：deriver→DeepSeek /
         embedding→DashScope 在 Honcho 侧既有记账内，backfill 不新增）。

  K4-E6 申报（live structural 项，零 host 自动断言、挂 human 部署轮
     awaiting-human:live-deploy）：① human 在 ECS 执行 backfill 全量跑 +
     计数报告过目（peers/sessions/messages/observations 前后对照，与源
     语料行数对账——真语料全量照记的最终对账半）；② 幂等二次跑零增量；
     ③ 清库回滚演练一次（测试库或快照窗口）；④ 抽查画像命中（如「王一诺」
     相关旧事回忆）。本包 host 面转绿后停门等 human，不自行部署。

===========================================================================
host/live 分工边界（riskTier=high + Honcho DB / 源语料在 ECS、host 面不可
直达，如实申报）：

  · host 面（本件）= 迁移工具的**离线行为契约**（注入桩真跑：解析零丢弃 /
    身份口径 / 指纹幂等 / 计数报告 / 回滚范围 / 源只读 / 安全负锚）+
    K4-E6 头注申报自检；**不证真语料全量对账**。
  · live/运维面 = ECS 全量跑 + 计数报告过目（真语料逐件行数对账——host
    fixture 是冻结最小格式、非真语料）+ 幂等二次跑 + 回滚演练 + 画像抽查
    = K4-E6（structural 申报，挂 human 部署轮 awaiting-human:live-deploy，
    Pod 不自行上 ECS）。

强度申报（契约条目 → 承载断言 → 性质档）：

  · E1 照记全量零过滤（不分回合性质 / 不滤姓名）→ test_k4_a1_full_ingest_...
    → 行为级（注入桩真跑：逐行收编 + 第三方姓名 / front 类行逐字在场）
  · E1 零 PII 过滤器                         → 同上 + 旗标负锚        → 机制级（负锚）
  · E2 设备 session 键族 + 家庭单 owner 零新 peer → test_k4_a2_...    → 行为级
  · E3 指纹幂等二次跑零增量                   → test_k4_a3_...        → 行为级
  · E3 已灌 vs live 新增区分（live 零误判零触碰）→ test_k4_a3_live_... → 行为级
  · E3 计数报告 schema + 算术自洽             → test_k4_a3_counting_... → 机制级
  · E3 plan 干跑零写副作用                    → test_k4_a3_plan_...    → 行为级
  · E4 回滚恰删标记 session、live 零触碰      → test_k4_a4_rollback_... → 行为级
  · E4 源语料只读零改写 + 回滚零源旗标        → 同上 + 负锚            → 机制级
  · E5 资格门不互串（豁免声明 + 配置负锚）    → test_k4_a5_...         → 机制级（负锚）
  · E6 申报面                                 → test_k4_a6_header_...   → 机制级（申报自检）
  · 安全（零 secret / 零记账外端点 / loopback）→ test_k4_a6_security_... → 源锚代理
  · manifest 登记 + verify-deploy 核验        → test_k4_a6_manifest_... /
                                                 test_k4_a6_verify_...   → 机制级

缺口（承载断言强度低于契约要求处，逐条；2026-10-23 申报收敛——原「REST
wire 形状 = 最佳知识、未定形」申报撤销，见文尾申报收敛注记）：

  · REST wire 形状已落定（2026-10-23，不再按「最佳知识 / 未定形」口吻在案）：
    builder 首轮 /v2 形态错误经主线程对照审查（ECS /opt/honcho 现读）实锤，
    已按 upstream main@7478be29 本地克隆源码对读修复，与现读特征逐条吻合
    ——全 router prefix=/v3、sessions/peers/messages/conclusions 全
    workspace 作用域（/v3/workspaces/{wid}/...）、MessageBatchCreate
    ≤100 分块、RESOURCE_NAME_PATTERN 禁 `:` 故 backfill session 走确定性
    可逆 wire 名编码（kidpal-bf-<base64url>，canonical 键保真在鸭型协议
    层）。冻结全量 333 passed 零变化（断言与鸭型协议零改动）。

  仍挂 live 全量跑首验点的诚实申报（收窄为四项；真栈行为仍待 live 对账）：
    ① message metadata 逐消息可达性——指纹幂等的真栈前提；若 metadata
       不可达，builder 须在实现期以等价源锚机制（如 session 侧指纹清单）
       替代并回改本冻结件（tighten-only 纪律反审）。
    ② 真语料格式适配——host fixture 是冻结最小行格式（ECS 真语料格式
       host 无样本），真语料逐件解析与行数对账 = live 轮计数报告过目
       （K4-E6 ①）。行为等价假设：最小格式下的零丢弃 / 兜底收编行为 ≈
       真语料解析零丢弃（显式化，不写即默认不成立；真语料解析缺陷会在
       live 计数对账时暴露）。
    ③ DELETE 202 异步删除窗差——真栈 DELETE session 返回 202（inactive
       即时标记、真删异步级联），host 桩按同步「删 session 连带计数」
       建模存在窗差；回滚演练（K4-E6 ③）对账。
    ④ conclusions 异步派生等待窗——deriver 异步产出 conclusions /
       observations 存在等待窗，「二次跑零重复 observation」的语义半与
       前后计数取窗 = observations 计数对账（K4-E6 ② 幂等二次跑）。

  · device_id 取值口径（MAC vs 其他设备标识）沿 P2/P3 既有 user 语义，
    工具以 --device-id 透传不裁定；live 跑时 human 按 ECS 现网 device id
    传参（既有注记保留，不属上述四项 live 对账点）。

fixture 自含性声明：全部断言用 tempfile 自建 fixture（corpus 两个文本件 /
sqlite 一个临时库）+ 注入桩客户端；零网络 I/O（stub 客户端纯内存）、零
外部资产、零 datetime.now 依赖、零环境前提；evidence 类扫描断言显式以
REPO_ROOT = Path(__file__).resolve().parents[2] 推导仓根，不依赖调用点 cwd
（acceptance-evidence「evidence 类扫描断言的 root 注入」节）。

行为 seam 驱动方对账：runner/rollback → 真实驱动方 = human ECS 部署轮
（live-manual，K4-E6）；本件 host 断言以注入桩真跑工具进程为驱动方，对
「真 Honcho REST / 真语料格式」两个 seam 为差集覆盖（差集已显式申报并
分流到 live 面）。

缺陷修复注记（2026-10-23，tighten-only 非语义变更）：冻结件自身缺陷修复——
  test_k4_a4_rollback_marked_only_live_survives_sources_readonly 驱动 rb 前漏注
  _patch_client（同族 8 处全有、唯此处漏），离线机连接拒绝致 rc==0 断言空红；
  补 `self._patch_client(rb, stub)` 一行，断言零改动（K3 姊妹件「锚换代注记」
  留痕先例格式）。

申报收敛注记（2026-10-23，tighten-only 非语义变更）：头注「缺口申报」节
  收敛——builder 首轮 REST /v2 形态错误经主线程对照审查（ECS /opt/honcho
  现读）实锤、按 upstream main@7478be29 本地克隆源码对读修复后，原「REST
  wire 形状（metadata 是否逐消息可达）= live 全量跑首验点」的未定形申报
  撤销，端点与请求/响应形状按源码对读落定；诚实申报收窄为四项 live 对账点
  （metadata 逐消息可达 / 真语料格式适配 / DELETE 202 异步删除窗差 /
  conclusions 异步派生等待窗）。断言、鸭型协议、coverage 标记零改动
  （上方「缺陷修复注记」同日留痕先例格式）。

Run protocol（文件名两点须 importlib 模式）:
  cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance
  （standalone: python 本文件）
"""

from __future__ import annotations

import hashlib
import importlib.util
import json
import re
import sqlite3
import tempfile
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DEPLOY = REPO_ROOT / "server" / "deploy"
HONCHO_DIR = DEPLOY / "honcho"
BACKFILL_DIR = HONCHO_DIR / "backfill"
BACKFILL_PY = BACKFILL_DIR / "backfill_honcho.py"
ROLLBACK_PY = BACKFILL_DIR / "rollback_honcho_backfill.py"
MANIFEST_YAML = DEPLOY / "manifest.yaml"
VERIFY_SH = DEPLOY / "verify-deploy.sh"

# --------------------------------------------------------------------------- #
# 冻结字面（变更 = 语义放宽/收紧，走 tighten-only 纪律反审）
# --------------------------------------------------------------------------- #

K4_DEVICE_FAMILY_PREFIX = "agent:kidpal:openai-user:"
K4_OWNER_PEER = "owner"
K4_DEFAULT_API = "http://127.0.0.1:18100"
K4_SESSION_BATCH_KEY = "kidpalBackfillBatch"
K4_MESSAGE_FP_KEY = "kidpalBackfillFingerprint"
K4_SOURCE_KINDS = ("session-corpus", "openclaw-transcript")
K4_FP_RE = re.compile(r"^v1:(session-corpus|openclaw-transcript):[0-9a-f]{64}$")
# 出网负锚词表（NFR-SEC-2 记账外外部端点；同 K3 家族词表）。
K4_FORBIDDEN_HOSTS = (
    "api.openai.com",
    "api.anthropic.com",
    "openrouter.ai",
    "googleapis.com",
    "openai.azure.com",
    "amazonaws.com",
    "api.moonshot.cn",
    "api.mistral.ai",
    "api.groq.com",
    "api.together.xyz",
    "api.x.ai",
)
K4_SECRET_KEY_RE = re.compile(r"sk-[A-Za-z0-9]{16,}")
# 资格门配置负锚（K4-E5：豁免路径不互串 P3 资格门判据）。
K4_GATE_CONFIG_NEEDLES = ("noisePatterns", "captureSystemRuns")
# 过滤类旗标负锚（K4-E1：零 PII 过滤器，human 终拍）。
K4_FILTER_FLAG_RE = re.compile(r"--(redact|mask|anonymize|pii|filter-[a-z-]+)\b")


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _iter_tree_text(root: Path, skip_prefix: str | None = None):
    """递归产出 (path, text)；跳过二进制迹象件（NUL 字节）与 skip 子树。"""
    if not root.is_dir():
        return
    for p in sorted(root.rglob("*")):
        if not p.is_file():
            continue
        if skip_prefix and p.relative_to(root).as_posix().startswith(skip_prefix):
            continue
        txt = _read_text(p)
        if "\x00" in txt:
            continue
        yield p, txt


def _sha256_file(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


# --------------------------------------------------------------------------- #
# 注入桩（冻结鸭型客户端协议的记录实现，纯内存、零网络）
# --------------------------------------------------------------------------- #


class _StubHoncho:

    def __init__(self, peers: int = 1, observations: int = 0):
        self._peers = peers
        self._observations = observations
        self.sessions: dict[str, dict] = {}  # key -> {peer, metadata, messages}
        self.calls: list[tuple] = []         # (op, args...) 审计

    def snapshot_counts(self) -> dict:
        return {
            "peers": self._peers,
            "sessions": len(self.sessions),
            "messages": sum(len(s["messages"]) for s in self.sessions.values()),
            "observations": self._observations,
        }

    def list_backfill_batches(self) -> set:
        return {
            s["metadata"][K4_SESSION_BATCH_KEY]
            for s in self.sessions.values()
            if K4_SESSION_BATCH_KEY in s["metadata"]
        }

    def list_fingerprints(self) -> set:
        return {
            m[2][K4_MESSAGE_FP_KEY]
            for s in self.sessions.values()
            for m in s["messages"]
            if K4_MESSAGE_FP_KEY in m[2]
        }

    def create_session(self, peer, session_key, metadata):
        self.calls.append(("create_session", peer, session_key, dict(metadata)))
        self.sessions.setdefault(
            session_key, {"peer": peer, "metadata": dict(metadata), "messages": []})

    def add_message(self, session_key, role, content, metadata):
        self.calls.append(("add_message", session_key, role, content, dict(metadata)))
        self.sessions.setdefault(
            session_key, {"peer": "", "metadata": {}, "messages": []})
        self.sessions[session_key]["messages"].append((role, content, dict(metadata)))

    def delete_sessions(self, session_keys):
        keys = list(session_keys)
        self.calls.append(("delete_sessions", tuple(keys)))
        for k in keys:
            self.sessions.pop(k, None)
        return len(keys)

    # 断言辅助 ------------------------------------------------------------ #
    def write_calls(self) -> list[tuple]:
        return [c for c in self.calls
                if c[0] in ("create_session", "add_message", "delete_sessions")]


def _live_session_key(device_id: str) -> str:
    return K4_DEVICE_FAMILY_PREFIX + device_id


def _seed_live_session(stub: _StubHoncho, device_id: str) -> None:
    """预置 live 在用 session（插件在写形态：无 backfill 标记）。"""
    key = _live_session_key(device_id)
    stub.sessions[key] = {
        "peer": K4_OWNER_PEER,
        "metadata": {"openclawSessionKey": key},  # live 形态、零 batch 标记
        "messages": [
            ("user", "live 新增消息一（无 backfill 标记）", {}),
            ("assistant", "live 新增消息二（无 backfill 标记）", {}),
        ],
    }


# --------------------------------------------------------------------------- #
# fixture（冻结最小源格式；真语料格式对账 = live 轮，见头注缺口申报）
# --------------------------------------------------------------------------- #

CORPUS_FILE_1 = (
    "[user] 我最喜欢的颜色是紫色，我的小狗叫豆豆\n"
    "[assistant] 紫色真好看呀，豆豆是什么品种呀\n"
    "[user] 我的同学王一诺说他家有一只猫\n"
    "[assistant] 嗯嗯\n"
    "[front] 孩子到目前为止讲的内容：（旧设备流程回合语料——照记，不滤）\n"
)
CORPUS_FILE_2 = (
    "[user] 今天学校运动会我跑了第二名\n"
    "无角色前缀的旧语料残行（兜底收编路径）\n"
)
CORPUS_EXPECTED_MESSAGES = 7   # 逐非空行全量收编（含 front 类行 + 兜底残行）
CORPUS_EXPECTED_UNPARSED = 1   # 无角色前缀残行：兜底收编 + 披露
TRANSCRIPT_ROWS = (
    (1, "user", "P2 持久会话起的事件一"),
    (2, "assistant", "P2 持久会话起的事件二"),
)
TRANSCRIPT_EXPECTED_MESSAGES = len(TRANSCRIPT_ROWS)
ALL_EXPECTED_MESSAGES = CORPUS_EXPECTED_MESSAGES + TRANSCRIPT_EXPECTED_MESSAGES
DEVICE_ID = "aa:bb:cc:dd:ee:ff"


def _make_corpus(tmp: Path) -> Path:
    d = tmp / "session-corpus"
    d.mkdir()
    (d / "2026-09-30.txt").write_text(CORPUS_FILE_1, encoding="utf-8")
    (d / "2026-09-28.txt").write_text(CORPUS_FILE_2, encoding="utf-8")
    return d


def _make_transcript_db(tmp: Path) -> Path:
    db = tmp / "openclaw-kidpal.db"
    con = sqlite3.connect(str(db))
    try:
        con.execute(
            "CREATE TABLE transcript_events "
            "(id INTEGER PRIMARY KEY, role TEXT, content TEXT, created_at TEXT)")
        con.executemany(
            "INSERT INTO transcript_events (id, role, content, created_at) "
            "VALUES (?, ?, ?, ?)",
            [(i, r, c, "2026-10-23T00:00:00Z") for i, r, c in TRANSCRIPT_ROWS])
        con.commit()
    finally:
        con.close()
    return db


class K4BaseCase(unittest.TestCase):

    maxDiff = None

    def _load_module(self, path: Path, name: str):
        self.assertTrue(
            path.is_file(),
            f"RED: {path} 不在世 —— backfill 工具落盘面缺失"
            f"（server/deploy/honcho/backfill/**，K4-E1/E3/E4 工具承载）。")
        spec = importlib.util.spec_from_file_location(name, path)
        self.assertIsNotNone(spec, f"RED: {path} 无法定位 import spec。")
        module = importlib.util.module_from_spec(spec)
        try:
            spec.loader.exec_module(module)
        except Exception as e:  # noqa: BLE001 —— 导入失败是 RED，不是 crash
            self.fail(
                f"RED: {name} 导入失败 —— 模块顶层必须零副作用、零网络 I/O、"
                f"stdlib-only（家族纪律）: {e!r}")
        return module

    def _patch_client(self, module, stub: _StubHoncho) -> None:
        self.assertTrue(
            hasattr(module, "build_client"),
            "RED: 模块缺冻结接缝 build_client(api)（头注 B 节：离线桩注入面）。")
        module.build_client = lambda api: stub


# --------------------------------------------------------------------------- #
# K4-A1 · 照记全量零过滤（K4-E1 / S-F5.1-AC5）
# --------------------------------------------------------------------------- #


class K4FullIngestAcceptance(K4BaseCase):

    def _common_args(self, corpus: Path, db: Path, report: Path) -> list:
        return [
            "--corpus-dir", str(corpus),
            "--openclaw-db", str(db),
            "--device-id", DEVICE_ID,
            "--report-out", str(report),
        ]

    def test_k4_a1_full_ingest_zero_drop_no_filtering(self):
        """coverage: K4-E1, S-F5.1-AC5 — 照记全量：apply 态对 fixture 双源
        恰 {ALL_EXPECTED_MESSAGES} 条 add_message（零丢弃）；第三方姓名
        「王一诺」与旧设备流程回合行（[front]）逐字在场——不分回合性质、
        不滤第三方姓名（human 2026-10-22 终拍 R6/OQ-3，零 PII 过滤器）。"""
        tag = "K4-E1"
        mod = self._load_module(BACKFILL_PY, "k4_backfill_honcho")
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            corpus = _make_corpus(tmp)
            db = _make_transcript_db(tmp)
            stub = _StubHoncho()
            self._patch_client(mod, stub)
            rc = mod.main(self._common_args(corpus, db, tmp / "report.json") + ["--apply"])
            self.assertEqual(
                rc, 0, f"{tag} RED: apply 态 exit={rc}（须 0）。")
            msgs = [c for c in stub.calls if c[0] == "add_message"]
            self.assertEqual(
                len(msgs), ALL_EXPECTED_MESSAGES,
                f"{tag} RED: add_message 数 = {len(msgs)} ≠ {ALL_EXPECTED_MESSAGES}"
                " —— 解析出的每一行/每行事件都必须恰好一条计划消息（照记全量、"
                "零丢弃、零回合性质过滤、零姓名过滤）。")
            contents = "\n".join(c[3] for c in msgs)
            self.assertIn(
                "王一诺", contents,
                f"{tag} RED: 第三方姓名「王一诺」未逐字进写入计划 —— 历史照记"
                "不含 PII 过滤器（孩子社交事实是记忆目标，R6 human 终拍；"
                "S-F5.1-AC5「含同学姓名」）。")
            self.assertIn(
                "孩子到目前为止讲的内容", contents,
                f"{tag} RED: [front] 类旧设备流程回合行被丢弃 —— 历史照记不区分"
                "回合性质（AC5：旧内容与新聊天一视同仁；设备流程回合边界不回溯"
                "历史）。")
            self.assertIn(
                "无角色前缀的旧语料残行", contents,
                f"{tag} RED: 未识别行被静默丢弃 —— 兜底收编（照记全量零丢弃；"
                "计数报告 backfill.unparsed 披露）。")

    def test_k4_a1_usage_error_missing_source_or_device(self):
        """coverage: K4-E1（边界路径）— 用法错不裸崩：缺数据源或缺 --device-id
        → exit 2（不抛未捕获异常、不写任何副作用）。"""
        tag = "K4-E1"
        mod = self._load_module(BACKFILL_PY, "k4_backfill_honcho_usage")
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            stub = _StubHoncho()
            self._patch_client(mod, stub)
            cases = [
                [],  # 零源零设备
                ["--device-id", DEVICE_ID],  # 零源
                ["--corpus-dir", str(_make_corpus(tmp))],  # 缺 device-id
            ]
            for argv in cases:
                rc = mod.main(list(argv))
                self.assertEqual(
                    rc, 2,
                    f"{tag} RED: argv={argv!r} exit={rc}（缺源/缺设备须 exit 2、"
                    "不得裸异常崩溃——迁移工具的用法错必须显式可判）。")
            self.assertEqual(
                stub.write_calls(), [],
                f"{tag} RED: 用法错路径出现写调用 —— 用法错必须先于任何写入。")


# --------------------------------------------------------------------------- #
# K4-A2 · 身份口径（K4-E2 / ADR-016）
# --------------------------------------------------------------------------- #


class K4IdentityAcceptance(K4BaseCase):

    def test_k4_a2_device_session_family_owner_peer_no_new_peer(self):
        """coverage: K4-E2, ADR-016 — 灌入 session key 全部落既有设备 session
        键族（前缀恰 agent:kidpal:openai-user:<device_id>）；目标 peer 恒 =
        家庭单 owner；零新 peer 创建（零设备名 peer / 零多设备多孩子身份）。"""
        tag = "K4-E2"
        mod = self._load_module(BACKFILL_PY, "k4_backfill_honcho_identity")
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            corpus = _make_corpus(tmp)
            db = _make_transcript_db(tmp)
            stub = _StubHoncho()
            self._patch_client(mod, stub)
            rc = mod.main([
                "--corpus-dir", str(corpus), "--openclaw-db", str(db),
                "--device-id", DEVICE_ID, "--report-out", str(tmp / "r.json"),
                "--apply"])
            self.assertEqual(rc, 0, f"{tag} RED: apply exit={rc}。")
            creates = [c for c in stub.calls if c[0] == "create_session"]
            self.assertTrue(
                creates, f"{tag} RED: 零 create_session —— 历史语料无落库计划。")
            bad_keys = [c[2] for c in creates
                        if not str(c[2]).startswith(
                            K4_DEVICE_FAMILY_PREFIX + DEVICE_ID)]
            self.assertEqual(
                bad_keys, [],
                f"{tag} RED: session key 越出设备键族: {bad_keys} —— 历史session "
                f"归宿 = 既有设备 session 键族（前缀恰 {K4_DEVICE_FAMILY_PREFIX}"
                f"<device_id>，ADR-016 / K4-E2；本次 --device-id={DEVICE_ID}）。")
            bad_peers = [c[1] for c in creates if c[1] != K4_OWNER_PEER]
            self.assertEqual(
                bad_peers, [],
                f"{tag} RED: 非家庭单 owner 的 session 落点: {bad_peers} —— 画像 "
                f"= 家庭单 owner peer（ADR-016：DB peers 只有 owner / agent-kidpal，"
                "零设备级 peer、零多孩子身份；拆包期「设备级 peer」表述已废）。")
            for c in creates:
                self.assertIn(
                    K4_SESSION_BATCH_KEY, c[3],
                    f"{tag} RED: session {c[2]} metadata 缺 {K4_SESSION_BATCH_KEY}"
                    " —— backfill 自建 session 必须携带 batch 标记（幂等 / 回滚"
                    "范围的判定锚，K4-E3/E4）。")
            adds = [c for c in stub.calls if c[0] == "add_message"]
            for c in adds:
                fp = c[4].get(K4_MESSAGE_FP_KEY, "")
                self.assertRegex(
                    fp, K4_FP_RE,
                    f"{tag} RED: 消息指纹 {fp!r} 不合冻结格式 v1:<source-kind>:"
                    f"<64hex>（metadata 键 {K4_MESSAGE_FP_KEY}）—— 幂等判据锚。")
            fps = {c[4][K4_MESSAGE_FP_KEY] for c in adds}
            kinds = {K4_FP_RE.match(f).group(1) for f in fps}
            self.assertEqual(
                kinds, set(K4_SOURCE_KINDS),
                f"{tag} RED: 指纹 source-kind 覆盖 = {sorted(kinds)} —— 双源"
                f"（{K4_SOURCE_KINDS}）各自 source-kind 锚定（OQ-10 重建双源；"
                "跨源不去重、各自幂等）。")


# --------------------------------------------------------------------------- #
# K4-A3 · 幂等 + 计数报告 + live 共生（K4-E3）
# --------------------------------------------------------------------------- #


class K4IdempotencyAcceptance(K4BaseCase):

    def _args(self, tmp: Path, corpus: Path, db: Path, report: Path) -> list:
        return [
            "--corpus-dir", str(corpus), "--openclaw-db", str(db),
            "--device-id", DEVICE_ID, "--report-out", str(report)]

    def _load(self):
        return self._load_module(BACKFILL_PY, "k4_backfill_honcho_idem")

    def test_k4_a3_idempotent_second_run_zero_ingest(self):
        """coverage: K4-E3 — 幂等可重跑：同一存储二次 apply 恰 ingested=0、
        skipped=全量、零写调用（重复执行零重复写入）；指纹对同一源确定不变
        （重跑指纹集合逐位相等）。"""
        tag = "K4-E3"
        mod = self._load()
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            corpus = _make_corpus(tmp)
            db = _make_transcript_db(tmp)
            stub = _StubHoncho()
            self._patch_client(mod, stub)
            rc1 = mod.main(self._args(tmp, corpus, db, tmp / "r1.json") + ["--apply"])
            self.assertEqual(rc1, 0, f"{tag} RED: 首跑 exit={rc1}。")
            fps_first = stub.list_fingerprints()
            self.assertEqual(
                len(fps_first), ALL_EXPECTED_MESSAGES,
                f"{tag} RED: 首跑指纹数 {len(fps_first)} ≠ {ALL_EXPECTED_MESSAGES}"
                " —— 每条灌入消息恰一指纹（同源内零碰撞：指纹 = 源锚身份而非"
                "纯内容，重复内容消息不互吞）。")
            n_msgs_first = stub.snapshot_counts()["messages"]
            rc2 = mod.main(self._args(tmp, corpus, db, tmp / "r2.json") + ["--apply"])
            self.assertEqual(rc2, 0, f"{tag} RED: 二跑 exit={rc2}。")
            self.assertEqual(
                stub.snapshot_counts()["messages"], n_msgs_first,
                f"{tag} RED: 二跑消息数增量 ≠ 0 —— 重复执行产生重复写入"
                "（K4-E3 幂等判据失守）。")
            self.assertEqual(
                [c for c in stub.calls if c[0] == "add_message"][ALL_EXPECTED_MESSAGES:],
                [],
                f"{tag} RED: 二跑出现 add_message —— 幂等跳过未生效。")
            rep2 = json.loads((tmp / "r2.json").read_text(encoding="utf-8"))
            self.assertEqual(
                rep2["backfill"]["ingested"], 0,
                f"{tag} RED: 二跑报告 ingested ≠ 0 —— 幂等判据须能区分「已灌」"
                "与「未灌」（notes ④ / K4-E3）。")
            self.assertEqual(
                rep2["backfill"]["skipped"], ALL_EXPECTED_MESSAGES,
                f"{tag} RED: 二跑报告 skipped = {rep2['backfill']['skipped']} ≠ "
                f"{ALL_EXPECTED_MESSAGES} —— 既有指纹须全量命中跳过。")
            self.assertEqual(
                stub.list_fingerprints(), fps_first,
                f"{tag} RED: 重跑指纹集合漂移 —— 指纹对同一源必须确定不变"
                "（源锚身份逐位稳定，K4-E3）。")

    def test_k4_a3_live_messages_never_mistaken_or_touched(self):
        """coverage: K4-E3（notes ④）— 已灌 vs live 新增的区分：live 在用
        session（无 backfill 标记）不被误判已灌（backfill 照常全量灌入）、
        不被写入（live 消息零增量、零指纹沾染）、roll 回滚面外。"""
        tag = "K4-E3"
        mod = self._load()
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            corpus = _make_corpus(tmp)
            db = _make_transcript_db(tmp)
            stub = _StubHoncho()
            _seed_live_session(stub, DEVICE_ID)
            self._patch_client(mod, stub)
            live_key = _live_session_key(DEVICE_ID)
            rc = mod.main(self._args(tmp, corpus, db, tmp / "r.json") + ["--apply"])
            self.assertEqual(rc, 0, f"{tag} RED: apply exit={rc}。")
            live = stub.sessions[live_key]
            self.assertEqual(
                len(live["messages"]), 2,
                f"{tag} RED: live session 消息数被改写 —— backfill 零写 live "
                "在用 session（与 live 共生红线）。")
            self.assertFalse(
                any(K4_MESSAGE_FP_KEY in m[2] for m in live["messages"]),
                f"{tag} RED: live 消息被沾染 backfill 指纹 —— live 新增数据无"
                "标记，永不进指纹集合（notes ④ 幂等判据的隔离半）。")
            adds = [c for c in stub.calls if c[0] == "add_message"]
            self.assertEqual(
                len(adds), ALL_EXPECTED_MESSAGES,
                f"{tag} RED: live 数据在场时灌入数 {len(adds)} ≠ "
                f"{ALL_EXPECTED_MESSAGES} —— live 消息不得被误判「已灌」而"
                "跳过历史源（幂等判据只认 backfill 标记，不认内容撞形）。")
            self.assertEqual(
                [c for c in adds if c[1] == live_key], [],
                f"{tag} RED: 出现指向 live session 的写入调用。")

    def test_k4_a3_plan_mode_zero_side_effect_report_prediction(self):
        """coverage: K4-E3 — plan 干跑：零写副作用（零 create/add/delete）、
        计数报告仍落盘且为预测形态（after = before + ingested 算术自洽）。"""
        tag = "K4-E3"
        mod = self._load()
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            corpus = _make_corpus(tmp)
            db = _make_transcript_db(tmp)
            stub = _StubHoncho()
            _seed_live_session(stub, DEVICE_ID)
            self._patch_client(mod, stub)
            before = stub.snapshot_counts()
            rc = mod.main(self._args(tmp, corpus, db, tmp / "r.json"))
            self.assertEqual(rc, 0, f"{tag} RED: plan exit={rc}。")
            self.assertEqual(
                stub.write_calls(), [],
                f"{tag} RED: plan 态出现写调用 —— 缺省必须干跑（迁移安全铁律："
                "显式 --apply 才写）。")
            self.assertEqual(
                stub.snapshot_counts(), before,
                f"{tag} RED: plan 态计数漂移。")
            rep = json.loads((tmp / "r.json").read_text(encoding="utf-8"))
            self.assertEqual(
                rep["messages"]["after"],
                rep["messages"]["before"] + rep["backfill"]["ingested"],
                f"{tag} RED: plan 报告 messages after ≠ before + ingested —— "
                "计数报告须前后自洽（K4-E3 对账件）。")
            self.assertEqual(
                rep["backfill"]["ingested"], ALL_EXPECTED_MESSAGES,
                f"{tag} RED: plan 预测灌入 {rep['backfill']['ingested']} ≠ "
                f"{ALL_EXPECTED_MESSAGES}。")

    def test_k4_a3_counting_report_schema_and_arithmetic(self):
        """coverage: K4-E3 — apply 态计数报告：冻结 schema（mode + 四计数器
        before/after + backfill.ingested/skipped/unparsed/perSource）、写后
        算术自洽（sessions/messages after-before = 实际写量）、unparsed 披露
        兜底收编计数。"""
        tag = "K4-E3"
        mod = self._load()
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            corpus = _make_corpus(tmp)
            db = _make_transcript_db(tmp)
            stub = _StubHoncho()
            self._patch_client(mod, stub)
            rc = mod.main(self._args(tmp, corpus, db, tmp / "r.json") + ["--apply"])
            self.assertEqual(rc, 0, f"{tag} RED: apply exit={rc}。")
            rep = json.loads((tmp / "r.json").read_text(encoding="utf-8"))
            self.assertEqual(
                rep.get("mode"), "apply", f"{tag} RED: mode 键缺失/非 apply。")
            for counter in ("peers", "sessions", "messages", "observations"):
                self.assertIn(
                    counter, rep,
                    f"{tag} RED: 计数报告缺 {counter} —— K4-E3 点名四计数器"
                    "（peers/sessions/messages/observations）。")
                self.assertIn(
                    "before", rep[counter], f"{tag} RED: {counter}.before 缺失。")
                self.assertIn(
                    "after", rep[counter], f"{tag} RED: {counter}.after 缺失。")
            bf = rep.get("backfill") or {}
            for key in ("ingested", "skipped", "unparsed", "perSource"):
                self.assertIn(
                    key, bf, f"{tag} RED: backfill.{key} 缺失（unparsed = 兜底"
                    "收编披露，perSource = 分源计数，K4-E3）。")
            self.assertEqual(
                bf["ingested"] + bf["skipped"], ALL_EXPECTED_MESSAGES,
                f"{tag} RED: ingested+skipped = {bf['ingested']+bf['skipped']} ≠ "
                f"{ALL_EXPECTED_MESSAGES} —— 零丢弃不变式（每条源消息要么灌入"
                "要么幂等跳过）。")
            self.assertEqual(
                bf["unparsed"], CORPUS_EXPECTED_UNPARSED,
                f"{tag} RED: unparsed = {bf['unparsed']} ≠ "
                f"{CORPUS_EXPECTED_UNPARSED} —— 无角色前缀残行的兜底收编必须"
                "披露计数（照记全量的可审计半）。")
            self.assertIn(
                "session-corpus", bf["perSource"],
                f"{tag} RED: perSource 缺 session-corpus —— 分源计数须按 "
                "source-kind 锚定。")
            creates = [c for c in stub.calls if c[0] == "create_session"]
            adds = [c for c in stub.calls if c[0] == "add_message"]
            self.assertEqual(
                rep["sessions"]["after"] - rep["sessions"]["before"],
                len(creates),
                f"{tag} RED: sessions after-before ≠ create_session 数 —— "
                "报告算术须与真实写量一致（M5 收口轮对账依据）。")
            self.assertEqual(
                rep["messages"]["after"] - rep["messages"]["before"],
                len(adds), f"{tag} RED: messages after-before ≠ add_message 数。")
            self.assertEqual(
                rep["peers"]["after"], rep["peers"]["before"],
                f"{tag} RED: peers 计数变化 —— backfill 零新 peer（K4-E2）。")


# --------------------------------------------------------------------------- #
# K4-A4 · 清库回滚 + 源只读（K4-E4 / OQ-10 / NFR-PRIV-1）
# --------------------------------------------------------------------------- #


class K4RollbackAcceptance(K4BaseCase):

    def _apply_backfill(self, mod, stub, tmp: Path, corpus: Path, db: Path):
        self._patch_client(mod, stub)
        rc = mod.main([
            "--corpus-dir", str(corpus), "--openclaw-db", str(db),
            "--device-id", DEVICE_ID, "--report-out", str(tmp / "bf.json"),
            "--apply"])
        self.assertEqual(rc, 0, "K4-E4 RED: 前置 backfill apply 失败。")

    def _source_hashes(self, corpus: Path, db: Path) -> dict:
        return {
            str(p.relative_to(corpus)): _sha256_file(p)
            for p in sorted(corpus.rglob("*")) if p.is_file()
        } | {"<db>": _sha256_file(db)}

    def test_k4_a4_rollback_marked_only_live_survives_sources_readonly(self):
        """coverage: K4-E4, OQ-10, NFR-PRIV-1 — 回滚恰删带 batch 标记的
        session（plan 预演零删 → apply 恰删）；live 在用 session 零触碰；
        回滚全程源语料 / OpenClaw db 字节不变（只读零改写）；回滚报告含
        deleted 计数与四计数器前后自洽。"""
        tag = "K4-E4"
        bf = self._load_module(BACKFILL_PY, "k4_backfill_honcho_rb")
        rb = self._load_module(ROLLBACK_PY, "k4_rollback_honcho")
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            corpus = _make_corpus(tmp)
            db = _make_transcript_db(tmp)
            hashes_before = self._source_hashes(corpus, db)
            stub = _StubHoncho()
            _seed_live_session(stub, DEVICE_ID)
            self._apply_backfill(bf, stub, tmp, corpus, db)
            # 注桩（2026-10-23 缺陷修复，非语义变更）：同族 8 处 runner 驱动点
            # 全有 _patch_client，唯独本测试对 rb 漏注——真实 REST 客户端连
            # loopback 18100，离线机连接拒绝致 rc==0 断言空红。补注桩零断言
            # 改动（tighten-only：修自fixture 缺陷，非放宽）。
            self._patch_client(rb, stub)
            live_key = _live_session_key(DEVICE_ID)
            marked_keys = {k for k in stub.sessions if k != live_key}
            self.assertTrue(marked_keys, "K4-E4 RED: 前置 backfill 未产生标记 session。")

            # plan 预演：零删。
            rc = rb.main([
                "--api", K4_DEFAULT_API, "--peer", K4_OWNER_PEER,
                "--report-out", str(tmp / "rb_plan.json")])
            self.assertEqual(rc, 0, f"{tag} RED: 回滚 plan exit={rc}。")
            self.assertEqual(
                [c for c in stub.calls if c[0] == "delete_sessions"], [],
                f"{tag} RED: plan 预演执行了删除 —— 缺省必须零删（迁移铁律："
                "回滚路径先验证、显式 --apply 才删）。")

            # apply：恰删标记 session。
            rc = rb.main([
                "--api", K4_DEFAULT_API, "--peer", K4_OWNER_PEER,
                "--report-out", str(tmp / "rb_apply.json"), "--apply"])
            self.assertEqual(rc, 0, f"{tag} RED: 回滚 apply exit={rc}。")
            self.assertEqual(
                set(stub.sessions), {live_key},
                f"{tag} RED: 回滚后 session 集 ≠ {live_key!r} —— 恰删带 batch "
                "标记的 session、live 零触碰（NFR-PRIV-1 删除边界）。")
            self.assertEqual(
                len(stub.sessions[live_key]["messages"]), 2,
                f"{tag} RED: live session 消息在回滚后被删改。")
            rep = json.loads((tmp / "rb_apply.json").read_text(encoding="utf-8"))
            self.assertIn(
                "deleted", rep.get("backfill") or {},
                f"{tag} RED: 回滚报告缺 backfill.deleted 计数。")
            self.assertEqual(
                rep["backfill"]["deleted"], len(marked_keys),
                f"{tag} RED: deleted = {rep['backfill']['deleted']} ≠ 标记 "
                f"session 数 {len(marked_keys)}。")
            for counter in ("peers", "sessions", "messages", "observations"):
                self.assertIn(
                    counter, rep, f"{tag} RED: 回滚报告缺 {counter} 计数器。")
            self.assertEqual(
                self._source_hashes(corpus, db), hashes_before,
                f"{tag} RED: 回滚全程源语料 / OpenClaw db 字节变化 —— 源只读"
                "零改写（K4-E4 / NFR-PRIV-1 三保留：不删 OpenClaw 完整日志、"
                "历史补录源语料、设备诊断日志）。")
            self.assertEqual(
                self._source_hashes(corpus, db),
                hashes_before,
                "K4-E4 RED: （复核）源哈希二次取值不一致 —— fixture 读取非幂等。")

    def test_k4_a4_backfill_sources_readonly_across_full_cycle(self):
        """coverage: K4-E4 — backfill plan/apply 全程源只读：corpus 逐文件与
        transcript db 在干跑 + 真跑两态后字节不变（源语料零改写 / 零移动 /
        零清理）。"""
        tag = "K4-E4"
        mod = self._load_module(BACKFILL_PY, "k4_backfill_honcho_ro")
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            corpus = _make_corpus(tmp)
            db = _make_transcript_db(tmp)
            hashes_before = self._source_hashes(corpus, db)
            stub = _StubHoncho()
            self._patch_client(mod, stub)
            base = ["--corpus-dir", str(corpus), "--openclaw-db", str(db),
                    "--device-id", DEVICE_ID, "--report-out", str(tmp / "r.json")]
            self.assertEqual(mod.main(list(base)), 0, "K4-E4 RED: plan 失败。")
            self.assertEqual(
                mod.main(base + ["--apply"]), 0, "K4-E4 RED: apply 失败。")
            self.assertEqual(
                self._source_hashes(corpus, db), hashes_before,
                f"{tag} RED: backfill plan/apply 全程源文件字节变化 —— 源语料"
                "只读零改写（K4-E4 铁律：只增不删源）。")

    def test_k4_a4_rollback_no_source_flags_no_delete_surface(self):
        """coverage: K4-E4, NFR-PRIV-1 — 回滚面只在记忆库侧：rollback 工具
        零 --corpus-dir / --openclaw-db / 删除源类旗标（不删 OpenClaw 完整
        日志 / 源语料 / 诊断日志的机制面负锚）。"""
        tag = "K4-E4"
        rb = self._load_module(ROLLBACK_PY, "k4_rollback_honcho_flags")
        src = _read_text(ROLLBACK_PY)
        for flag in ("--corpus-dir", "--openclaw-db", "--purge-source",
                     "--delete-corpus", "--wipe-logs"):
            self.assertNotIn(
                flag, src,
                f"{tag} RED: rollback 工具出现源侧旗标 {flag!r} —— 回滚面只在"
                "记忆库侧（NFR-PRIV-1：不删 OpenClaw 完整日志 / 源语料 / 诊断"
                "日志；各按既有接受口径保留）。")
        self.assertTrue(
            hasattr(rb, "build_client") and hasattr(rb, "main"),
            f"{tag} RED: rollback 模块缺冻结接缝（build_client / main）。")


# --------------------------------------------------------------------------- #
# K4-A5 · 资格门不互串（K4-E5 / T4）
# --------------------------------------------------------------------------- #


class K4GateBoundaryAcceptance(K4BaseCase):

    def test_k4_a5_exemption_declared_gate_config_not_shared(self):
        """coverage: K4-E5, T4 — 历史照记豁免显式声明 + 资格门配置零互串：
        runner 头注声明 AC5 历史照记豁免（不分回合性质）；backfill 树零
        noisePatterns / captureSystemRuns 引用——豁免路径不复用/回改 P3 插件
        资格门判据，新增回合仍走资格门（两条口径不互串）。"""
        tag = "K4-E5"
        self._load_module(BACKFILL_PY, "k4_backfill_honcho_gate")
        src = _read_text(BACKFILL_PY)
        for token in ("历史照记", "豁免", "AC5"):
            self.assertIn(
                token, src,
                f"{tag} RED: runner 头注缺豁免声明锚 {token!r} —— backfill 属"
                "「历史照记」显式豁免（AC5：不分回合性质），豁免依据必须落盘"
                "可审（K4-E5）。")
        corpus = "\n".join(
            txt.lower() for _p, txt in _iter_tree_text(BACKFILL_DIR)).lower()
        hits = [n for n in K4_GATE_CONFIG_NEEDLES if n.lower() in corpus]
        self.assertEqual(
            hits, [],
            f"{tag} RED: backfill 树出现资格门配置锚 {hits} —— 豁免路径不得"
            "复用/回改 P3 插件资格门判据（T4 / decisionCoverage「记忆口径分层」："
            "backfill 全量照记 vs 新增回合资格门两条口径不互串；P3 声明单源 "
            "server/deploy/261022-openclaw-honcho-plugin/ 不动）。")


# --------------------------------------------------------------------------- #
# K4-A6 · K4-E6 申报面 + 安全 + 登记/核验
# --------------------------------------------------------------------------- #


class K4DeclarationsAndOpsAcceptance(K4BaseCase):

    def test_k4_a6_header_live_declaration_k4_e6(self):
        """coverage: K4-E6 — 申报面自检（防申报段被静默删除）：头注含 K4-E6
        live structural 项四件（ECS 全量跑 + 计数报告过目 / 幂等二次跑零增量 /
        清库回滚演练 / 画像抽查）与 awaiting-human:live-deploy 停门标记 +
        K4-E2 已知后果（换设备 = 新 session 家族、画像不变）。"""
        tag = "K4-E6"
        src = _read_text(Path(__file__))
        for token in ("K4-E6", "awaiting-human:live-deploy", "ECS 全量跑",
                      "计数报告过目", "幂等二次跑零增量", "清库回滚演练"):
            self.assertIn(
                token, src,
                f"{tag} RED: 头注 K4-E6 live 申报丢失 {token!r}（live 运维项 = "
                "human 部署轮；零 host 自动断言，申报在场 = structural 承载）。")
        for token in ("换设备 = 新 session 家族", "画像不变"):
            self.assertIn(
                token, src,
                f"{tag} RED: 头注 K4-E2 已知后果申报丢失 {token!r}（ADR-016："
                "换设备 = 新 session 家族、画像不变，记录为已知后果）。")

    def test_k4_a6_security_no_secret_no_unaccounted_hosts_loopback(self):
        """coverage: K4-E6, NFR-SEC-2 ⑤, gateProfile security — backfill 树
        零内联 secret（sk- 长键 = 红）、零记账外外部端点（负锚词表零命中；
        出网只经 --api loopback，deriver/embedding 两条记账在 Honcho 侧既有
        体内）、DEFAULT_API = 127.0.0.1:18100；零过滤类旗标。"""
        tag = "K4-E6/security"
        self._load_module(BACKFILL_PY, "k4_backfill_honcho_sec")
        self._load_module(ROLLBACK_PY, "k4_rollback_honcho_sec")
        corpus = "\n".join(
            txt for _p, txt in _iter_tree_text(BACKFILL_DIR))
        self.assertTrue(
            corpus.strip(), "K4-E6/security RED: backfill 树零文本工件。")
        leaks = K4_SECRET_KEY_RE.findall(corpus)
        self.assertEqual(
            leaks, [],
            f"{tag} RED: backfill 树内联 secret（sk- 长键）—— 凭据零内联"
            "（backfill 走本机 loopback 无凭据，K3 同族纪律）。")
        low = corpus.lower()
        hits = [h for h in K4_FORBIDDEN_HOSTS if h in low]
        self.assertEqual(
            hits, [],
            f"{tag} RED: backfill 树出现记账外出网端点 {hits} —— 出网只走"
            "已记账 deriver→DeepSeek / embedding→DashScope 两条（NFR-SEC-2 ⑤；"
            "backfill 不新增出网路径）。")
        mod = self._load_module(BACKFILL_PY, "k4_backfill_honcho_sec_api")
        self.assertEqual(
            getattr(mod, "DEFAULT_API", None), K4_DEFAULT_API,
            f"{tag} RED: DEFAULT_API ≠ {K4_DEFAULT_API} —— backfill 数据面只经 "
            "Honcho loopback（arch §1.2 消费位）。")
        for name in (BACKFILL_PY.name, ROLLBACK_PY.name):
            self.assertNotRegex(
                _read_text(BACKFILL_DIR / name), K4_FILTER_FLAG_RE,
                f"{tag} RED: {name} 出现过滤类旗标 —— 照记全量、不带 PII 过滤器"
                "（human 终拍；不得以「隐私」为由自行缩圈）。")

    def test_k4_a6_manifest_registers_backfill_assets(self):
        """coverage: K4-E6, 261013 纪律 — manifest.yaml 对 backfill 资产登记
        ≥2 条（两工具全覆盖），src 在仓内存在、dst 在 /opt/honcho/backfill/
        消费位、verify 字段在场（部署单一事实源）。"""
        tag = "K4-E6/manifest"
        self._load_module(BACKFILL_PY, "k4_backfill_honcho_manifest")
        self.assertTrue(
            MANIFEST_YAML.is_file(), f"{tag} RED: manifest.yaml 不在场。")
        text = _read_text(MANIFEST_YAML)
        entries = []
        for line in text.splitlines():
            m = re.match(r"\s*-\s*src:\s*(\S+)", line)
            if m and "backfill" in m.group(1):
                entries.append(m.group(1))
        self.assertGreaterEqual(
            len(entries), 2,
            f"{tag} RED: manifest 对 backfill 资产登记 {len(entries)} < 2 —— "
            "迁移两工具须全量入部署单一事实源（261013 纪律；live 全量跑在 ECS "
            "消费登记位）。")
        missing = [s for s in entries if not (REPO_ROOT / s).is_file()]
        self.assertEqual(
            missing, [], f"{tag} RED: manifest 登记的 backfill 源缺失: {missing}。")
        self.assertTrue(
            any("/backfill/" in s or s.startswith("server/deploy/honcho/backfill")
                for s in entries),
            f"{tag} RED: 登记条目未锚 backfill 子树: {entries}。")
        # dst / verify 块逐条目随行核对（manifest 条目块内）。
        blocks = re.split(r"(?m)^\s*-\s*src:", text)[1:]
        covered = 0
        for block in blocks:
            if "backfill" not in block.splitlines()[0]:
                continue
            covered += 1
            self.assertIn(
                "/opt/honcho/backfill", block,
                f"{tag} RED: backfill 条目 dst 越出 /opt/honcho/backfill/ 消费位:"
                f" {block.splitlines()[0].strip()}。")
            self.assertRegex(
                block, r"(?m)^\s*verify\s*:\s*\S",
                f"{tag} RED: backfill 条目缺 verify 字段 —— 零漂移核验"
                "（261013 纪律）。")
        self.assertEqual(
            covered, len(entries),
            f"{tag} RED: backfill 条目块解析数 {covered} ≠ src 行数 {len(entries)}。")

    def test_k4_a6_verify_deploy_backfill_section_must_stay_green(self):
        """coverage: K4-E6 — verify-deploy.sh 新增 backfill 资产核验段；
        P3 Honcho 锚（127.0.0.1:18100）与 deploy-drift 告警面不回退
        （must-stay-green：与 P1/P3 串行共享件零回退）。"""
        tag = "K4-E6/verify"
        src = _read_text(VERIFY_SH)
        self.assertTrue(src.strip(), f"{tag} RED: verify-deploy.sh 不在场/为空。")
        self.assertIn(
            "backfill", src.lower(),
            f"{tag} RED: verify-deploy.sh 零 backfill 标识 —— 迁移资产核验段"
            "未落（部署后自检与每日 drift timer 共用本脚本）。")
        self.assertIn(
            "127.0.0.1:18100", src,
            f"{tag} RED: verify-deploy.sh 丢失 127.0.0.1:18100 锚 —— P3 Honcho "
            "健康核验被本包改动回退（must-stay-green）。")
        self.assertIn(
            "deploy-drift", src,
            f"{tag} RED: verify-deploy.sh 丢失 deploy-drift 告警面 —— 既有漂移"
            "报警机制被回退（must-stay-green）。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
