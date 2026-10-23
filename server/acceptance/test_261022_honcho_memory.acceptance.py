"""Frozen acceptance tests — TaskPackage 261022-m5-k3-honcho-memory-production
（K3-E1 / K3-E3 / K3-E4 / K3-E7 host 面；rigor=full；riskTier=high；
subsystem = server/deploy/honcho 自托管五容器生产化 + workspace-kidpal/AGENTS.md
v12 记忆守则。隔离判据与 openclaw.json 声明式 patch 见姊妹件
test_261022_memory_isolation.acceptance.py。）

上游 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/
PROPOSAL.md T1（Honcho 落位）+ T4（资格门 / §6.7 六行表）+ ADR-014 + workstream
PROPOSAL §3.1 R3/R4 + docs/requirements.md S-F5.1（AC3/AC4）+ NFR-PRIV-1（边界/
保留/删除三阈值）+ NFR-SEC-2 ⑤（出网两条记账）+ docs/debt/tech/
TECH-261022-device-memory-prompt-overhaul.md 证据 7（EMBEDDING_VECTOR_DIMENSIONS
=1536 / DERIVER_FLUSH_ENABLED=true / peer 键 agent:kidpal:openai-user:<device_id>
）+ launch-card 纪律红线（A4 backfill 禁区 / 出网红线 / 删除语义 / AGENTS.md 单向
追加）。

背景（TECH-261022 第 3 包）：Honcho 探针 profile 已端到端验证（五容器
127.0.0.1:18100、deriver→DeepSeek 抽 observation、representation 产事实、
before_prompt_build 跨 session 注入生效），但生产 profile 未装插件、v11 隐私
守则过度泛化（实测拒记同学真名/宠物名）、写入资格门未建。本包把栈生产化落盘
`server/deploy/honcho/**`（compose + 配置 + apply/rollback + 健康检查）并把 v12
记忆守则追加进 AGENTS.md 静态语义（孩子世界事实必须记 / 家长隐私不记）。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O）

  A) server/deploy/honcho/docker-compose.yml（栈本体声明，A4 backfill 禁区
     已于 2026-10-23 锚换代解除（K4 包 261022-m5-k4 创建 backfill 子树），
     栈本体零 backfill 引用的边界半保留——见文末「锚换代注记」）:

       · services 恰 **5** 个（五容器，arch §5.3.2 / E1）。
       · api 容器发布 127.0.0.1:18100（loopback；arch §1.2 / §5.3.3）。
       · **全部 published 端口显式绑定 127.0.0.1**（出网红线：五容器端口全
         loopback；短式 "18100" 或无 host_ip 的长式 = 绑 0.0.0.0 = 设计态红；
         容器间互通用 `expose:`，不进本判据）。`network_mode: host` = 红。
       · 恰一个 service 的 image 含 pgvector（postgres + pgvector），且该
         service 声明本机持久卷（volume 挂载目标含 postgres/pgdata 路径——
         NFR-PRIV-1 ② 自托管存于 ECS 本机）。

  B) server/deploy/honcho/ 配置工件（*.env.example / config* / compose
     environment 均可，扫描整树文本）:

       · EMBEDDING_VECTOR_DIMENSIONS = **1536**（debt 证据 7：dashscope
         text-embedding-v4 须显式维度，pgvector 列固定 1536——值漂移 =
         embedding 写入失败）。
       · DERIVER_FLUSH_ENABLED 真值（=true / =1）。
       · Honcho api baseUrl = 127.0.0.1:18100（指本机 loopback）。
       · 出网仅 NFR-SEC-2 ⑤ 两条记账管线：deriver→DeepSeek、embedding→
         DashScope——整树含 deepseek 与 dashscope（ci）锚；**不得出现记账外
         外部端点**（负锚词表冻结于 K3_FORBIDDEN_HOSTS，命中即红）。
       · 凭据纪律：DASHSCOPE_API_KEY 以**名字引用**（env 注入），整树零内联
         secret（sk- 长键 = 红；openclaw.json 同族纪律：凭据不入仓）。

  C) server/deploy/honcho/ 运维工件：apply 脚本 + rollback 脚本 + 健康检查
     （api healthcheck 或独立 health 脚本二选一形态均可）各 ≥1 件。

  D) server/deploy/manifest.yaml：server/deploy/honcho/** 登记条目 ≥4
     （compose + 配置 + apply + rollback + 健康检查全覆盖），src 在仓内存在、
     dst 在 /opt/honcho/ 消费位、verify 字段在场（verify-deploy.sh 恒 manifest
     驱动、对新条目零特例）。

  E) server/deploy/verify-deploy.sh：新增 Honcho 端口/健康核验段——
     127.0.0.1:18100 loopback 探针 + 五容器健康门（compose ps 路径）；
     arch §5.3.3 第③条「记忆子系统 api 18100 仅本机可达、五容器其余端口公网
     不可达」的机检承载。

  F) server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md（v12 追加，
     **单向追加红线**）:

       · 新增记忆守则节（heading 含「记忆」）：孩子世界的事实（喜好 / 宠物 /
         同学 / 经历过的事）**必须记**（v11 实测拒记同学真名/宠物名的
         过度泛化在本节解开）；家长隐私信息（姓名 / 联系方式 / 家庭安排）
         **不记、不进记忆画像**。
       · P1 迁移段语义**逐字保留、零回改**（冻结锚集 K3_P1_AGENTS_ANCHORS）；
         纪律**不得**放回引擎动态块（kidpal-dynamic-context.v1.txt 零记忆
         纪律锚——负锚）。

  G) K3-E4 生命周期 / K3-E5 降级 / K3-E7 live 观测项 = 头注申报面（本件
     test_k3_e4_e5_e7_declarations_in_header 守申报在场）+ 机制负锚：
     honcho 树零 pg_dump / 零远端备份端点（OQ-10 不备份靠重建）、零自助
     memory-delete 面（删除 = 运维面手工、范围 = 画像与 observation/消息
     存储、不删 OpenClaw 完整日志 / 回填源语料 / 诊断日志）。

  K3-E4 申报（删除/重建语义，机制半见上）：删除 = 运维面手工，范围 = 记忆
     画像与 observation/消息存储；不删 OpenClaw 完整日志、历史补录源语料、
     设备诊断日志（各按既有接受口径保留）；备份 = 不备份靠重建（OQ-10 human
     终裁），重建源 = session-corpus 源语料 + OpenClaw 会话存储；删除后画像
     为空验证 = 运维轮（NFR-PRIV-1 验证口径第二半）。

  K3-E5 申报（降级语义）：记忆服务不可用 → 本轮无画像注入、对话与回复照常，
     不阻塞、不静默重试风暴；记忆失败不进 §6.3 用户可见兜底（用户无感知）；
     记忆数据不进任何日志导出与展示面。承载 = 插件侧 bounded 失败护栏
     （姊妹件 test_261022_memory_isolation 的 K3-E5 断言）+ live 真栈实证。

  K3-E7 申报（live 观测项，structural、零 host 自动断言、挂 human 部署轮
     awaiting-human:live-deploy）：① 新 WS 新会话问「我最喜欢的颜色 / 我的
     小狗叫什么」答对（全新 user id 对照答不出——S-F5.1-AC2 live 半）；②
     2C/4G 基线下 prep 税 / 内存读数前后对照，event_loop_delay 无 38s 级退化，
     复测读数回填 TECH-261022-openclaw-prep-tax-memory-pressure；③ arch
     §6.7 抽取策略实测成形后回填（deriver 提示可配置性；若需改 spec 走
     /arch-change 逃生门）。

  overlay 锚注记（launch-card 条件项）：Honcho 栈为独立 compose（/opt/honcho），
  **不经** xiaozhi `.config.yaml` overlay 渲染——P1 的动态块锚集零触碰、本包
  不追加 overlay 锚（P1 的 test_261010_engine_decision21_overlay_persist 零新增
  红 = must-stay-green）。

===========================================================================
host/live 分工边界（按 riskTier=high + Honcho DB 在 ECS postgres、host 面不可
直达，如实申报）：

  · host 面（本件）= 栈/配置/登记/守则的**结构断言**（机制级 + 措辞级守卫）+
    生命周期机制负锚；**不证 live 生效**。
  · live/部署轮面 = marker 探针 DB+日志双证（主链路正证 / 一次性回合负证）、
    seeded 负向探针集 100% 零命中（S-F5.1-AC3 / NFR-PRIV-1 的量化口径）、
    降级真栈实证、跨会话回忆——由姊妹件 test_261022_memory_isolation 的
    @pytest.mark.live 证据判定齿 + K3-E7 live 观测项承载（structural 申报，
    挂 human 部署轮 awaiting-human:live-deploy，Pod 不自行上 ECS）。

强度申报（契约条目 → 承载断言 → 性质档）：

  · E1 五容器 / api loopback 18100 / 全端口 loopback → test_k3_e1_compose_... → 机制级
  · E1 postgres+pgvector 本机持久卷          → 同上（volume 断言）          → 机制级
  · E1 出网仅两条记账 + 凭据不入仓           → test_k3_e1_config_...          → 源锚代理
    （负锚词表扫描 ≈「无记账外出网」；运行时出网观测 = K3-E7 live）
  · E1 apply/rollback/healthcheck 在场        → test_k3_e1_apply_rollback_...  → 机制级
  · E1 manifest 登记 + verify-deploy 核验     → test_k3_e1_manifest_... /
                                                 test_k3_e1_verify_deploy_...   → 机制级
  · E1 A4 backfill 禁区（锚换代：栈本体零 backfill 引用边界半）
                                             → test_k3_e1_no_backfill_...     → 机制级（负锚）
  · E3 v12 守则（孩子事实必须记 / 家长隐私不记）→ test_k3_e3_agents_md_...      → 措辞级
    （guard-class: wording——静态语义落盘措辞守卫，钉语义锚集合合理；
    模型真的「记孩子事实 / 拒家长隐私」= live seeded 负向探针集承载）
  · E3 P1 段单向追加零回改                    → test_k3_e3_p1_sections_...     → 机制级（逐字锚）
  · E3 纪律不入引擎动态块                     → test_k3_e3_dynamic_context_... → 机制级（负锚）
  · E4 删除/备份语义（机制半）                → test_k3_e4_lifecycle_...       → 源锚代理（负锚）
  · E4/E5/E7 申报面                           → test_k3_e4_e5_e7_...           → 机制级（申报自检）

缺口（承载断言强度低于契约要求处，逐条）：

  · E1「数据本机不出网」的运行时无出网：负锚词表扫描是**代理**（扫描词表外
    的新出网形态不可见）；运行时出网面 = K3-E7 live 轮 + 部署轮端口/防火墙
    核验（verify-deploy 段）。行为等价假设：compose/config 无外部端点声明 ≈
    运行时不出网（显式化，不写即默认不成立）。
  · E3 措辞守卫只证「守则文本在场且极性正确」，不证模型服从——服从面由
    live seeded 负向探针集 100% 零命中承载（NFR-PRIV-1 量化口径，姊妹件
    live 齿 + K3-E7）。
  · E4 删除 = 运维面手工属**无机制面语义**（Phase 1 无自助界面、无删除代码
    路径是设计事实）：host 面只能负锚「未引入自动化删除/备份」，真实删除
    演练与删除后画像为空验证 = live/运维轮（NFR-PRIV-1 验证口径第二半）。
  · 「五容器 services 恰 5」钉的是 arch/PROPOSAL 反复钉死的容器数；service
    名不作冻结（实现自由），api 以「发布 18100 的 service」识别。

fixture 自含性声明：全部断言直读 repo 内制品（server/deploy/honcho/** /
manifest.yaml / verify-deploy.sh / AGENTS.md / kidpal-dynamic-context.v1.txt /
本件头注）；零网络 I/O、零外部资产、零 datetime.now 依赖、零环境前提
（PyYAML 仅 compose/manifest 验收测试 case 用，与 verify-deploy.sh 同一解析
器；缺席 = infra 红，K1/K2 同口径）。evidence 类扫描断言显式以
REPO_ROOT = Path(__file__).resolve().parents[2] 推导仓根，不依赖调用点 cwd
（acceptance-evidence「evidence 类扫描断言的 root 注入」节）。

行为 seam 驱动方对账：compose/config → 栈的运行时驱动方 = 部署轮
（`docker compose up`，live-manual）；AGENTS.md 守则 → 模型行为驱动方 =
真机轮（K3-E7）；本件 host 断言对这两个 seam 均为差集覆盖（差集已显式
申报并分流到 live 面）。

锚换代注记（2026-10-23，显式治理动作·非静默放宽）：A4 backfill 禁区由包
  261022-m5-k4-history-corpus-backfill（K4）解除——server/deploy/honcho/backfill/**
  归 K4 创建，本件原 test_k3_e1_no_backfill_subtree_a4_redline 的「子树禁在 /
  manifest 零 backfill 条目」半退役，保留「K3 栈本体零 backfill 引用」边界半
  （栈本体不吞并迁移工具，换代后测试名 test_k3_e1_no_backfill_in_k3_stack_assets）。
  换代由 K4 acceptance-author 执行（Pod 内 acceptance 唯一写点）、K4 冻结件
  test_261022_honcho_backfill.acceptance.py 承接 backfill 面断言。

Run protocol（文件名两点须 importlib 模式）:
  cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance
  （standalone: python 本文件）
"""

from __future__ import annotations

import re
import unittest
from pathlib import Path

try:
    import yaml  # compose / manifest 验收测试 case 前置；缺席 = infra 红（K1/K2 同口径）
    _HAS_YAML = True
except ImportError:  # pragma: no cover
    _HAS_YAML = False

REPO_ROOT = Path(__file__).resolve().parents[2]
DEPLOY = REPO_ROOT / "server" / "deploy"
HONCHO_DIR = DEPLOY / "honcho"
COMPOSE_YML = HONCHO_DIR / "docker-compose.yml"
MANIFEST_YAML = DEPLOY / "manifest.yaml"
VERIFY_SH = DEPLOY / "verify-deploy.sh"
AGENTS_MD = DEPLOY / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md"
DYN_CTX = DEPLOY / "openclaw-agents" / "kidpal-dynamic-context.v1.txt"

# --------------------------------------------------------------------------- #
# 冻结字面（变更 = 语义放宽/收紧，走 tighten-only 纪律反审）
# --------------------------------------------------------------------------- #

K3_API_LOOPBACK = "127.0.0.1:18100"
# 出网负锚词表（NFR-SEC-2 记账外外部端点；记账内 = deepseek / dashscope，不进表）。
K3_FORBIDDEN_HOSTS = (
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
K3_SECRET_KEY_RE = re.compile(r"sk-[A-Za-z0-9]{16,}")

# v12 守则锚（措辞级语义锚；builder 撰写自由、锚集合冻结）。
K3_MEMORY_SECTION_RE = re.compile(r"(?m)^##\s+[^\n]*记忆")
K3_KID_FACTS_NEEDLES = ("孩子世界的事实", "宠物", "同学")
K3_PRIVACY_NEEDLES = ("家长隐私", "家庭安排")
K3_MUST_RECORD_FORMS = ("必须记", "要记", "照记", "记入", "记进", "记得住", "主动记")
K3_DENY_FORMS = ("不记", "不进", "不入", "不复述", "绝不")
# 动态块负锚（纪律不回引擎动态块）。
K3_DYNAMIC_DISCIPLINE_NEEDLES = ("孩子世界的事实", "家长隐私", "记忆画像")

# P1 迁移段逐字锚（单向追加红线：v12 不得回改 P1 段语义；选点 = P1 承载反转
# 自声明 + v11 底线隐私行 + 回合路由 + 三不变量标题行）。
K3_P1_AGENTS_ANCHORS = (
    "## 静态语义单一事实源",
    "以本文件为准",
    "## 底线（超越一切，任何时候都有效）",
    "查字回合绝对禁工具",
    "回合类型与纪律路由",
    "不引用、不复述任何家长隐私（姓名、联系方式、家庭安排细节）",
)


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _iter_tree_text(root: Path):
    """递归产出 (path, text)；跳过二进制迹象件（NUL 字节）。"""
    if not root.is_dir():
        return
    for p in sorted(root.rglob("*")):
        if p.is_file():
            txt = _read_text(p)
            if "\x00" in txt:
                continue
            yield p, txt


# --------------------------------------------------------------------------- #
# K3-E1 · Honcho 自托管栈生产化（compose / 配置 / 运维工件 / 登记 / 核验）
# --------------------------------------------------------------------------- #


class K3HonchoStackAcceptance(unittest.TestCase):

    maxDiff = None

    def _require_honcho_dir(self, tag):
        self.assertTrue(
            HONCHO_DIR.is_dir(),
            f"{tag} RED: {HONCHO_DIR} 不在世 —— Honcho 自托管栈生产化落盘面缺失"
            "（K3-E1 本体：compose + 配置 + apply/rollback + 健康检查）。")

    def _compose_doc(self, tag):
        self.assertTrue(
            COMPOSE_YML.is_file(),
            f"{tag} RED: {COMPOSE_YML} 不在世 —— 栈本体声明缺失（K3-E1）。")
        self.assertTrue(
            _HAS_YAML or self.fail(
                f"{tag} RED(infra): PyYAML 不在位——compose 解析前置缺失。"))
        src = _read_text(COMPOSE_YML)
        try:
            doc = yaml.safe_load(src)
        except Exception as e:  # noqa: BLE001 —— 解析错误是 RED，不是 crash
            self.fail(f"{tag} RED: docker-compose.yml 解析失败: {e!r}")
        self.assertIsInstance(
            doc, dict, f"{tag} RED: docker-compose.yml 顶层不是 mapping。")
        return src, doc

    def test_k3_e1_compose_five_services_api_loopback_only(self):
        """coverage: K3-E1 — compose 五容器恰 5 services；api 发布
        127.0.0.1:18100；**全部 published 端口显式绑 127.0.0.1**（出网红线：
        五容器端口全 loopback；短式/无 host_ip 长式 = 绑 0.0.0.0 = 红；
        network_mode: host = 红）；postgres+pgvector 容器 + 本机持久卷。"""
        tag = "K3-E1"
        self._require_honcho_dir(tag)
        src, doc = self._compose_doc(tag)
        services = doc.get("services")
        self.assertIsInstance(
            services, dict, f"{tag} RED: 无 services mapping（非 compose 栈）。")
        self.assertEqual(
            len(services), 5,
            f"{tag} RED: services 数 = {len(services)}（{sorted(services)}），"
            "须恰 5 —— 五容器（arch §5.3.2 / K3-E1；容器间通信用 expose，"
            "不计 published 面）。")
        self.assertNotRegex(
            src, r"(?m)^\s*network_mode\s*:\s*['\"]?host",
            f"{tag} RED: compose 声明 network_mode: host —— 容器端口直通宿主"
            "全部网卡，loopback 红线失守。")

        # published 端口逐一判 loopback。
        published: list[tuple[str, str, str]] = []  # (service, host_ip, host_port)
        for name, svc in services.items():
            svc = svc or {}
            self.assertNotEqual(
                str(svc.get("network_mode") or ""), "host",
                f"{tag} RED: service {name} network_mode: host —— 容器端口直通宿主"
                "全部网卡，五容器 loopback 红线被绕过（出网红线）。")
            for entry in svc.get("ports") or []:
                if isinstance(entry, dict):
                    host_ip = str(entry.get("host_ip") or "")
                    host_port = str(entry.get("published") or "")
                    if host_ip != "127.0.0.1":
                        self.fail(
                            f"{tag} RED: service {name} published 端口 {entry!r} "
                            "未显式绑定 127.0.0.1（host_ip 缺省 = 绑 0.0.0.0 全网卡"
                            "）—— 出网红线：五容器端口全 127.0.0.1（NFR-SEC-2 / "
                            "arch §5.3.3「五容器其余端口公网不可达」）。")
                    published.append((name, host_ip, host_port))
                else:
                    spec = str(entry)
                    parts = spec.split(":")
                    if len(parts) < 2 or parts[0] != "127.0.0.1":
                        self.fail(
                            f"{tag} RED: service {name} 端口声明 {spec!r} 未显式以 "
                            "127.0.0.1: 开头 —— 短式/无 host 绑定 = 暴露到全部网卡"
                            "（出网红线；容器间互通请用 expose:）。")
                    published.append((name, parts[0], parts[1]))

        api_pub = [p for p in published if p[2].split("/")[0] == "18100"]
        self.assertTrue(
            api_pub,
            f"{tag} RED: 零 service 发布 host 端口 18100 —— Honcho api 消费位"
            f"（{K3_API_LOOPBACK}，arch §1.2 / debt 证据 7）缺位。")
        self.assertTrue(
            all(p[1] == "127.0.0.1" for p in api_pub),
            f"{tag} RED: 18100 未绑 127.0.0.1 —— api 必须仅本机可达"
            f"（{K3_API_LOOPBACK}）。")

        # postgres + pgvector + 本机持久卷。
        pg = [(n, s or {}) for n, s in services.items()
              if "pgvector" in str((s or {}).get("image") or "")]
        self.assertEqual(
            len(pg), 1,
            f"{tag} RED: image 含 pgvector 的 service 数 = {len(pg)} —— 须恰一个"
            " postgres + pgvector 容器（K3-E1：向量列承载）。")
        pg_name, pg_svc = pg[0]
        vol_targets = " ".join(
            (v.get("target") or "") if isinstance(v, dict) else str(v)
            for v in (pg_svc.get("volumes") or []))
        self.assertRegex(
            vol_targets, r"postgres|pgdata",
            f"{tag} RED: pgvector 容器（{pg_name}）无 postgres 数据目录持久卷挂载"
            f"（volumes = {pg_svc.get('volumes')!r}）—— NFR-PRIV-1 ②「记忆数据自"
            "托管存于 ECS 本机」持久面缺失（容器重建即丢画像 = 数据生命周期失守）。")
        self.assertTrue(
            src.strip() and K3_API_LOOPBACK in src,
            f"{tag} RED: compose 文本零 {K3_API_LOOPBACK} 字面 —— api 消费位"
            "未以 loopback 形态声明。")

    def test_k3_e1_config_loopback_outbound_accounted_no_secret(self):
        """coverage: K3-E1, NFR-SEC-2 ⑤ — Honcho 配置工件：EMBEDDING_VECTOR_
        DIMENSIONS=1536（debt 证据 7，pgvector 列固定 1536）、DERIVER_FLUSH_
        ENABLED 真值、api baseUrl 127.0.0.1:18100 指本机；出网仅两条记账管线
        （deepseek + dashscope 锚在场；记账外外部端点负锚词表零命中）；凭据
        名字引用零内联 secret（sk- 长键 = 红）。"""
        tag = "K3-E1"
        self._require_honcho_dir(tag)
        corpus = "\n".join(txt for _p, txt in _iter_tree_text(HONCHO_DIR))
        self.assertTrue(
            corpus.strip(), f"{tag} RED: {HONCHO_DIR} 树零文本工件（配置面缺失）。")
        self.assertIn(
            K3_API_LOOPBACK, corpus,
            f"{tag} RED: Honcho 配置零 {K3_API_LOOPBACK} —— 插件 baseUrl 须指本机"
            " loopback（debt 证据 7「baseUrl 指本机」）。")
        self.assertRegex(
            corpus, r"EMBEDDING_VECTOR_DIMENSIONS\s*[=:]\s*[\"']?1536\b",
            f"{tag} RED: EMBEDDING_VECTOR_DIMENSIONS 未显式 = 1536 —— dashscope "
            "text-embedding-v4 须显式维度、pgvector 列固定 1536（debt 证据 7）；"
            "值漂移 = embedding 写入失败（记忆管线静默断粮）。")
        self.assertRegex(
            corpus, r"DERIVER_FLUSH_ENABLED\s*[=:]\s*[\"']?(true|1|True)\b",
            f"{tag} RED: DERIVER_FLUSH_ENABLED 未置真值 —— deriver 不冲刷则 "
            "observation 永不产出（debt 证据 7 配置要点）。")
        self.assertIn(
            "DASHSCOPE_API_KEY", corpus,
            f"{tag} RED: 配置零 DASHSCOPE_API_KEY 名字引用 —— embedding 凭据"
            "须以 env 名字注入（凭据沿用 ASR 同源 key，不入仓）。")
        low = corpus.lower()
        self.assertIn(
            "deepseek", low,
            f"{tag} RED: 配置零 deepseek 锚 —— deriver→DeepSeek（NFR-SEC-2 ⑤ "
            "第 1 条记账管线）未声明。")
        self.assertIn(
            "dashscope", low,
            f"{tag} RED: 配置零 dashscope 锚 —— embedding→DashScope（NFR-SEC-2 ⑤ "
            "第 2 条记账管线）未声明。")
        hits = [h for h in K3_FORBIDDEN_HOSTS if h in low]
        self.assertEqual(
            hits, [],
            f"{tag} RED: Honcho 配置出现记账外出网端点 {hits} —— 出网红线："
            "仅 deriver→DeepSeek + embedding→DashScope 两条已记账管线，"
            "不得新增未记账出网（NFR-SEC-2 ⑤ / launch-card 红线）。")
        leaks = K3_SECRET_KEY_RE.findall(corpus)
        self.assertEqual(
            leaks, [],
            f"{tag} RED: Honcho 配置树内联 secret（sk- 长键）—— 凭据不入仓"
            "（openclaw.json 同族纪律），以 env/名字引用注入。")

    def test_k3_e1_apply_rollback_healthcheck_present(self):
        """coverage: K3-E1 — 栈本体运维工件三件在场：apply 脚本、rollback 脚本、
        健康检查（compose healthcheck 或独立 health 脚本二选一形态）。"""
        tag = "K3-E1"
        self._require_honcho_dir(tag)
        names = [str(p.relative_to(HONCHO_DIR)).lower()
                 for p in HONCHO_DIR.rglob("*") if p.is_file()]
        self.assertTrue(
            any(n.startswith("apply") for n in names),
            f"{tag} RED: {HONCHO_DIR} 无 apply* 脚本 —— 栈生产化 apply 面缺失"
            "（K3-E1「apply/rollback + 健康检查」；261017/261022 声明式 patch"
            "先例同族：apply + rollback 双面成对）。")
        self.assertTrue(
            any(n.startswith("rollback") for n in names),
            f"{tag} RED: {HONCHO_DIR} 无 rollback* 脚本 —— 栈回滚面缺失"
            "（K3-E1；只有 apply 没有回滚 = 生产化不完整）。")
        compose_src = _read_text(COMPOSE_YML)
        has_compose_health = "healthcheck" in compose_src
        has_health_script = any("health" in n for n in names)
        self.assertTrue(
            has_compose_health or has_health_script,
            f"{tag} RED: compose 零 healthcheck 且树内零 health 脚本 —— K3-E1 "
            "健康检查面缺失（部署后核验三件之一，arch §5.3.3）。")

    def test_k3_e1_manifest_registers_honcho_assets(self):
        """coverage: K3-E1 — manifest.yaml 对 server/deploy/honcho/** 登记 ≥4 条
        （compose + 配置 + apply + rollback + 健康检查全覆盖），src 在仓内存在、
        dst 在 /opt/honcho/ 消费位、verify 字段在场（部署单一事实源，261013
        纪律；verify-deploy.sh 恒 manifest 驱动零特例）。"""
        tag = "K3-E1"
        self._require_honcho_dir(tag)
        self.assertTrue(
            _HAS_YAML or self.fail(
                f"{tag} RED(infra): PyYAML 不在位——manifest 解析前置缺失。"))
        self.assertTrue(MANIFEST_YAML.is_file(), f"{tag} RED: manifest.yaml 不在场。")
        try:
            doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
        except Exception as e:  # noqa: BLE001
            self.fail(f"{tag} RED: manifest.yaml 解析失败: {e!r}")
        entries = [e for e in (doc or {}).get("entries", []) or []
                   if isinstance(e, dict)
                   and str(e.get("src") or "").startswith("server/deploy/honcho/")]
        self.assertGreaterEqual(
            len(entries), 4,
            f"{tag} RED: manifest 对 server/deploy/honcho/** 的登记条目 "
            f"{len(entries)} < 4 —— 栈资产须全量入部署单一事实源（compose + 配置 "
            "+ apply + rollback + 健康检查；261013 纪律）。")
        missing = [str(e.get("src")) for e in entries
                   if not (REPO_ROOT / str(e.get("src"))).is_file()]
        self.assertEqual(
            missing, [],
            f"{tag} RED: manifest 登记的 honcho 仓内源缺失: {missing}。")
        bad_dst = [str(e.get("dst")) for e in entries
                   if not str(e.get("dst") or "").startswith("/opt/honcho/")]
        self.assertEqual(
            bad_dst, [],
            f"{tag} RED: honcho 登记条目 dst 越出 /opt/honcho/ 消费位: {bad_dst}"
            "（arch §5.3.2：栈落位 ECS /opt/honcho）。")
        no_verify = [str(e.get("src")) for e in entries
                     if not str(e.get("verify") or "").strip()]
        self.assertEqual(
            no_verify, [],
            f"{tag} RED: honcho 条目缺 verify 字段: {no_verify} —— verify-deploy.sh "
            "恒 manifest 驱动，无 verify = 该资产零漂移核验（261013 纪律）。")

    def test_k3_e1_verify_deploy_checks_honcho_health_and_loopback(self):
        """coverage: K3-E1 — verify-deploy.sh 新增 Honcho 端口/健康核验段：
        127.0.0.1:18100 loopback 探针 + 五容器健康门（compose ps 路径）——
        arch §5.3.3 第③条「记忆子系统 api 18100 仅本机可达、五容器其余端口
        公网不可达」的机检承载；漂移告警面（deploy-drift）不回退。"""
        tag = "K3-E1"
        src = _read_text(VERIFY_SH)
        self.assertTrue(src.strip(), f"{tag} RED: verify-deploy.sh 不在场/为空。")
        self.assertIn(
            K3_API_LOOPBACK, src,
            f"{tag} RED: verify-deploy.sh 零 {K3_API_LOOPBACK} 探针 —— api 仅本机"
            "可达核验缺失（arch §5.3.3 端口核验三件之一，K3-E1 要求 verify-deploy "
            "端口/健康核验）。")
        for token in ("docker", "compose", "ps"):
            self.assertIn(
                token, src,
                f"{tag} RED: verify-deploy.sh 零 {token!r} —— 五容器健康门未走 "
                "compose ps 路径（容器级 health/wait 核验缺失，端口探针不能替代"
                "容器健康）。")
        self.assertIn(
            "honcho", src.lower(),
            f"{tag} RED: verify-deploy.sh 零 honcho 标识 —— Honcho 健康核验段"
            "未落（部署后自检与每日 drift timer 共用本脚本，缺段 = 记忆栈漂移"
            "无报警）。")
        self.assertIn(
            "deploy-drift", src,
            f"{tag} RED: verify-deploy.sh 丢失 deploy-drift 告警面 —— 既有漂移"
            "报警机制被本包改动回退（must-stay-green）。")

    def test_k3_e1_no_backfill_in_k3_stack_assets(self):
        """coverage: K3-E1 — A4 禁区锚换代（2026-10-23，显式治理动作·非静默
        放宽，见头注「锚换代注记」）：原「backfill 子树禁在 / manifest 零
        backfill 条目」半由包 261022-m5-k4-history-corpus-backfill 解除退役
        （backfill/** 归 K4 创建并由 K4 冻结件
        test_261022_honcho_backfill.acceptance.py 承接断言），保留边界半——
        K3 栈本体工件（compose / env / apply / rollback / healthcheck）零
        backfill 引用：栈本体不吞并迁移工具，P4 资产独立落
        server/deploy/honcho/backfill/**。"""
        tag = "K3-E1"
        self._require_honcho_dir(tag)
        k3_assets = [p for p in HONCHO_DIR.iterdir() if p.is_file()]
        hits = [p.name for p in k3_assets
                if "backfill" in _read_text(p).lower()]
        self.assertEqual(
            hits, [],
            f"{tag} RED: K3 栈本体工件出现 backfill 引用 {hits} —— 栈本体不"
            "吞并迁移工具：backfill 子树归 P4（261022-m5-k4），独立落 "
            "server/deploy/honcho/backfill/**（A4 锚换代后保留的边界半）。")


# --------------------------------------------------------------------------- #
# K3-E3 · v12 记忆守则（AGENTS.md 单向追加 + 动态块负锚）
# --------------------------------------------------------------------------- #


class K3V12GuidelinesAcceptance(unittest.TestCase):

    maxDiff = None

    def _agents_md(self, tag) -> str:
        src = _read_text(AGENTS_MD)
        self.assertTrue(
            src.strip(),
            f"{tag} RED: {AGENTS_MD} 不在场/为空 —— 静态语义单一事实源镜像缺失"
            "（P1 承载反转基座，must-stay-green 前置）。")
        return src

    def _memory_section(self, tag, src: str) -> str:
        m = K3_MEMORY_SECTION_RE.search(src)
        self.assertIsNotNone(
            m,
            f"{tag} RED: AGENTS.md 无「## …记忆…」节 —— v12 记忆守则未追加"
            "（K3-E3 本体：孩子世界事实必须记 / 家长隐私不记，落 AGENTS.md "
            "静态语义单一事实源）。")
        rest = src[m.start() + 1:]
        nxt = re.search(r"(?m)^##\s+", rest)
        return rest[:nxt.start()] if nxt else rest

    def test_k3_e3_agents_md_v12_memory_section(self):
        """coverage: K3-E3, S-F5.1-AC3, S-F5.1-AC4, NFR-PRIV-1 — v12 记忆守则节
        在场且极性正确：孩子世界的事实（含宠物 / 同学——v11 实测拒记的两个
        具体类）必须记；家长隐私（含家庭安排）不记 / 不进画像。措辞级守卫
        （guard-class: wording）。"""
        tag = "K3-E3"
        src = self._agents_md(tag)
        section = self._memory_section(tag, src)
        for needle in K3_KID_FACTS_NEEDLES:
            self.assertIn(
                needle, section,
                f"{tag} RED: v12 记忆节缺锚 {needle!r} —— 「孩子世界的事实」"
                f"范畴未具体化（{needle} 正是 v11 过度泛化实测拒记的类："
                "「家庭小私事不特别记下来」，TECH-261022 证据 5；v12 必须解开）。")
        for needle in K3_PRIVACY_NEEDLES:
            self.assertIn(
                needle, section,
                f"{tag} RED: v12 记忆节缺隐私锚 {needle!r} —— 家长隐私（姓名 / "
                "联系方式 / 家庭安排）不记的边界未落（S-F5.1-AC3 / NFR-PRIV-1 ①）。")
        self.assertTrue(
            any(form in section for form in K3_MUST_RECORD_FORMS),
            f"{tag} RED: v12 记忆节零「必须记」极性语（{K3_MUST_RECORD_FORMS} 任"
            "一）—— 孩子世界的事实必须记的指令极性缺失（只写类别不写极性 = "
            "v11 泛化守则原样留存，记忆目标仍被拒记）。")
        self.assertTrue(
            any(form in section for form in K3_DENY_FORMS),
            f"{tag} RED: v12 记忆节零「不记」极性语（{K3_DENY_FORMS} 任一）—— "
            "家长隐私不记的否定极性缺失（AC3 隐私负向的守则面失守）。")
        self.assertNotIn(
            "次要承载", src,
            f"{tag} RED: AGENTS.md 出现「次要承载」自声明 —— P1 承载反转（静态"
            "语义主承载）被回退（must-stay-green，K1-E1）。")

    def test_k3_e3_p1_sections_untouched_single_direction_append(self):
        """coverage: K3-E3 — AGENTS.md 单向追加红线：P1 迁移段语义逐字保留
        （冻结锚集零回改：承载反转自声明 / 底线节 / 回合路由 / 查字禁工具 /
        v11 家长隐私不复述行）；v12 只追加、不回改。"""
        tag = "K3-E3"
        src = self._agents_md(tag)
        for anchor in K3_P1_AGENTS_ANCHORS:
            self.assertIn(
                anchor, src,
                f"{tag} RED: AGENTS.md 丢失 P1 冻结锚 {anchor!r} —— 单向追加红线"
                "被破：v12 记忆守则只追加、不得回改 P1 迁移段语义（launch-card "
                "纪律红线；静态语义单一事实源的既有语义零漂移）。")

    def test_k3_e3_dynamic_context_free_of_memory_discipline(self):
        """coverage: K3-E3 — 纪律不回引擎动态块（launch-card 红线）：
        kidpal-dynamic-context.v1.txt 零记忆纪律锚（动态块 = 有界回合内信息，
        静态纪律唯一在 AGENTS.md——ADR-001 两面接线不变式）。"""
        tag = "K3-E3"
        src = _read_text(DYN_CTX)
        self.assertTrue(
            src.strip(),
            f"{tag} RED: {DYN_CTX} 不在场/为空 —— 动态 context 模板缺失"
            "（P1 K1-E2 基座，must-stay-green 前置）。")
        hits = [n for n in K3_DYNAMIC_DISCIPLINE_NEEDLES if n in src]
        self.assertEqual(
            hits, [],
            f"{tag} RED: 引擎动态块出现记忆纪律锚 {hits} —— 纪律不得放回引擎"
            "动态块（launch-card AGENTS.md 红线；纪律进动态块 = 静态语义单一"
            "事实源被破、缓存边界下每轮重发）。")


# --------------------------------------------------------------------------- #
# K3-E4 / K3-E5 / K3-E7 · 生命周期机制负锚 + 申报面自检
# --------------------------------------------------------------------------- #


class K3LifecycleAndDeclarationsAcceptance(unittest.TestCase):

    maxDiff = None

    def test_k3_e4_lifecycle_no_backup_no_self_delete(self):
        """coverage: K3-E4, NFR-PRIV-1, OQ-10 — 生命周期机制半：honcho 树零
        pg_dump / 零远端备份端点（备份 = 不备份靠重建，OQ-10 human 终裁，重建
        源 = session-corpus 源语料 + OpenClaw 会话存储）；零自助 memory-delete
        面（删除 = 运维面手工、范围 = 画像与 observation/消息存储、不删 OpenClaw
        完整日志 / 回填源语料 / 诊断日志；Phase 1 无自助删除界面）。"""
        tag = "K3-E4"
        self.assertTrue(
            HONCHO_DIR.is_dir(),
            f"{tag} RED: {HONCHO_DIR} 不在世 —— 生命周期负锚无承载面"
            "（K3-E1 未落，非本断言独立缺陷，先修 E1）。")
        corpus = "\n".join(txt for _p, txt in _iter_tree_text(HONCHO_DIR)).lower()
        for forbidden in ("pg_dump", "s3://", "oss://", "cos.ap-", "amazonaws"):
            self.assertNotIn(
                forbidden, corpus,
                f"{tag} RED: honcho 树出现 {forbidden!r} —— 备份 = 不备份靠重建"
                "（OQ-10 human 终裁）：不得引入 pg_dump 本机副本或出网备份；"
                "重建源 = session-corpus 源语料 + OpenClaw 会话存储。")
        # 自助删除面负锚（front + 栈 + 插件工作台）。
        front_src = _read_text(REPO_ROOT / "server" / "gateway" / "front" / "front_app.py")
        workbench = DEPLOY / "261022-openclaw-honcho-plugin"
        wb_corpus = "\n".join(
            txt for _p, txt in _iter_tree_text(workbench)) if workbench.is_dir() else ""
        for surface in (corpus, wb_corpus.lower(), front_src.lower()):
            self.assertNotIn(
                "memory_delete", surface,
                f"{tag} RED: 出现 memory_delete 面 —— Phase 1 无自助删除界面"
                "（NFR-PRIV-1 ③：删除 = 运维面手工；记忆删除是独立运维动作，"
                "契约现行边界 kidpal_story_delete 仅 story MD 不触对话记忆）。")

    def test_k3_e4_e5_e7_declarations_in_header(self):
        """coverage: K3-E4, K3-E5, K3-E7 — 申报面自检（机制级，防申报段被静默
        删除）：头注含 E4 删除/重建语义、E5 降级语义、E7 live 观测项三项
        （新 WS 新会话回忆对照 / prep 税与内存复测回填 debt / arch §6.7 抽取
        策略实测成形）与 awaiting-human:live-deploy 停门标记。"""
        tag = "K3-E4/E5/E7"
        src = _read_text(Path(__file__))
        for token in ("K3-E4", "运维面手工", "不删 OpenClaw 完整日志"):
            self.assertIn(token, src, f"{tag} RED: 头注 K3-E4 申报丢失 {token!r}。")
        for token in ("K3-E5", "不阻塞", "用户可见"):
            self.assertIn(
                token, src,
                f"{tag} RED: 头注 K3-E5 降级申报丢失 {token!r}（记忆服务不可用 → "
                "本轮无画像注入、对话照常、不静默重试风暴、记忆失败不进用户"
                "可见兜底）。")
        for token in ("K3-E7", "awaiting-human:live-deploy",
                      "TECH-261022-openclaw-prep-tax-memory-pressure",
                      "抽取策略", "§6.7"):
            self.assertIn(
                token, src,
                f"{tag} RED: 头注 K3-E7 live 观测项申报丢失 {token!r}。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
