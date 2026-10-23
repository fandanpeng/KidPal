"""Frozen acceptance tests — TaskPackage 261022-m5-k2b-persistent-session
（K2-E1..E6 host 面；rigor=full；subsystem = server/deploy engine-patch 持久
会话补丁：请求携 user=<device_id> + 只发本回合；docker-compose bind-mount；
启动标记行漂移护栏）。

上游 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/
PROPOSAL.md T2（payload 恰一条本回合 user message / 无 history 重放 /
user=device_id）+ ADR-015 双历史单一权威 + T6 契约包（interface-contract.md
§四「会话连续性承载」定案：省略 user 回落旧行为、向后兼容）+
delivery/workstreams/device-memory-overhaul/PROPOSAL.md §3.1 R1 + docs/debt/
tech/ARQ-260907-engine-deploy-patch-drift.md（补丁漂移护栏先例）+
delivery/taskpackages/261022-m5-k2b-persistent-session/plan.md（跨包事实五条
+ 铁律五条）+ launch-card.md（纪律红线四条）。

背景（TECH-261022 第 2 包）：openai-http 不带 user 字段 → 每请求
`openai:<randomUUID>` 新 session，跨会话事实答不出（S-F5.1-AC1/AC2 无底座）；
同时引擎每轮全量重放 RAM 历史，与未来的服务端持久会话并存必造「同一段话
重放两次」双历史漂移。本包把承载反转：请求携 user=<device_id>、只发本回合，
OpenClaw 按该标识维护服务端持久会话（键 agent:kidpal:openai-user:<device_id>），
该会话成为对话历史唯一权威；引擎 RAM Dialogue 降为本地非权威副本（只作本地
簿记，一律不向 OpenClaw 重放）；前端不持对话历史。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 /
导入期零网络 I/O；connection.py 与 openai.py 均**不可离线 import**——引擎
依赖不在仓，验收走 AST 抽取真实源切片 + 受限命名空间 exec 行为驱动，ET/
record_dialogue_guide 先例）

  A) server/deploy/engine-patch/openai.py（新增，部署副本补丁，docker-compose
     bind-mount 覆盖镜像内 core/providers/llm/openai/openai.py）:

     基于 ECS 部署版 stock openai.py 打补丁（仓内最近似基线 = 参考树
     05_XiaoZhiServer main/xiaozhi-server/core/providers/llm/openai/openai.py；
     部署版若有差异以部署版为准，补丁面只到本契约所列）。冻结面：

       · response(self, session_id, dialogue, *, user=None, **kwargs) 与
         response_with_functions(self, session_id, dialogue, functions=None,
         *, user=None, **kwargs)：两方法签名均含**命名参数 user**（缺省
         None——命名参数是漂移探针 _kidpal_provider_supports_user 的判据，
         **kwargs 吞参形态不满足冻结契约）。
       · 请求装配仍走 self.client.chat.completions.create(**request_params)
         （payload 观察缝，stock 形态；验收经 mock self.client 捕获实参）。
       · user 真值（非 None 且非空串）→ request_params["user"] = user
         （additive 字段、字段名冻结 = "user"；OpenClaw 按
         agent:kidpal:openai-user:<device_id> 键维护持久会话）。
       · user 缺省 / None / 空串 → request_params **不含** "user" 键
         （向后兼容负锚：省略 user = 旧行为，每请求随机 session、调用方
         自携带历史——契约 §四定案）。
       · messages 原样透传（provider 零历史注入、零裁剪——历史策略全部归
         connection 侧，provider 是 payload 忠实面）。
       · max_tokens 维持 stock 语义（仅显式给出才进请求）——契约 §四
         「对 OpenClaw 流式请求禁带 max_tokens」硬约束不回退。
       · thinking 维持关闭（stock _apply_thinking_disabled 钩子保留并照常
         施加于 request_params——R9 裁定零触碰）。

  B) server/deploy/engine-patch/connection.py（既有部署副本补丁增量）:

       · 模块级纯函数 _kidpal_turn_messages(assembled, actual_start,
         keep_tail_count) -> list（名字与签名冻结；本回合切片唯一装配点）：
         assembled = dialogue.get_llm_dialogue_with_memory 的全量装配产物
         （[system, fewshot…, 实际历史…]）；返回 = 会话级前缀
         assembled[:actual_start]（system + fewshot——prompt 装配层不动，
         memory/voiceprint/speaker 注入保持）+ 本回合尾部
         assembled[len(assembled)-keep_tail_count:]；先前的实际历史消息
         一律不进返回值（无 history 重放——ADR-015 硬语义）。参数非法
         （actual_start<0 / keep_tail_count<0 / 区间越界）→ ValueError。
         keep_tail_count == 0 → 仅返回前缀（无本回合消息的边界）。
       · 模块级纯函数 _kidpal_provider_supports_user(llm) -> bool（名字
         冻结；补丁在位探针）：判定 llm.response 是否以命名参数接受
         "user"（inspect.signature 判「命名参数 user 在 parameters」即可，
         实现细节自由）；llm 无 response / 反射失败 → False（绝不抛）。
       · chat() 内全部 LLM 发送腿（self.llm.response 与
         self.llm.response_with_functions 调用点）：messages 实参**可静态
         追溯到 _kidpal_turn_messages 调用**（实参子树直接含该调用，或实参
         为本函数内由含该调用的赋值中转的局部变量）；messages 实参**不得**
         是裸 get_llm_dialogue_with_memory 调用（「每轮透传历史 + user」
         双写形态 = ADR-015 明令禁止，验收红）；每个调用点携带
         keyword 参数 user=self.device_id（device_id 缺席时 provider 侧
         按 A 项回落，connection 侧恒透传）。
       · chat() 的 self.dialogue.put(Message(role="user", ...)) 照旧保留
         （引擎 RAM Dialogue = 本地非权威副本继续记账：标题生成 / 调试
         日志——ADR-015 降级语义，不是删除）。
       · 启动标记行（补丁在位唯一机械报警锚，ARQ-260907 同族）：
         _initialize_components 内的 marker 字面
         "kidpal persistent-session patch active"，且该行**必须由
         _kidpal_provider_supports_user 探针门控**——探针通过才打 marker，
         探针失败打 error 级报警（openai.py bind-mount 丢失时 stock
         provider 静默吞掉 user kwargs = 全功能回退，无门控的 marker 是
         假绿报警锚）。既有三条 marker（display-strip / stop-truncation /
         schedule-summary）与 P5 全部接线零回退。

  C) server/deploy/docker-compose.yml: volumes 新增
       - ./engine-patch/openai.py:/opt/xiaozhi-esp32-server/core/providers/llm/openai/openai.py
     （connection.py 与 kidpal_display_strip.py 既有两行零回退）。

  D) server/deploy/manifest.yaml: 新增 openai.py 登记条目（src =
     server/deploy/engine-patch/openai.py，dst =
     /opt/kidpal/xiaozhi-esp32-server/deploy/engine-patch/openai.py，
     verify: md5，restart: engine，note 含「docker compose up -d」人工
     生效步提示——照 docker-compose.yml 条目既有纪律：新增 bind-mount 后
     首次生效须 docker compose up -d 重建容器（= 设备全断、人工窗口），
     deploy.sh 只做 docker restart 不自动重建）。verify-deploy.sh 恒
     manifest 驱动、对新条目零特例——md5 核验接线 = 本条目本身，脚本
     预期零 diff（changeManifest 在册但零改动属预期，申报给 planner）。

  K2-E2 的前端半（前端不持对话历史）：本包零触碰 front（changeManifest 无
    front 文件）；front 一次性回合本就无状态（arch T4 证据
    front_app.py:170-186 messages 逐请求内联构造），不落断言，仅此申报。
  K2-E4 的「第二轮答对第一轮信息」host 半 = 持久会话存储中第一轮事实对
    第二轮请求可见（离线仿真 OpenClaw 会话存储 + 部署版 bundle
    resolveSessionKey 同形解析器）；模型真的答对 = live 观测（K2-E6），
    offline 不造假绿。
  K2-E5 = 既有冻结验收零新增红：本件不含对 test_261010_engine_decision21_
    overlay_persist.acceptance.py / test_prompt_v7_round_routing.acceptance.py /
    test_261022_m5_k1_prompt_carrier_migration.acceptance.py /
    test_261001_openclaw_model_name_overlay_drift.acceptance.py 的任何断言
    改动（纪律钉行：builder/fixer 不得触碰这四件；server 全量基线由
    redBaseline 命令守——既有 278 passed 不得降）。若实现中发现某既有断言
    与本包语义冲突必须换代：停、报 planner 裁 supersede，不得自行改既有
    冻结件。
  K2-E6 = live 观测项（非自动断言，structural 申报）：human 部署后真机跨
    WS 新会话仍记得此前讲过的事实；持久会话历史增长与 auto-compaction 成本
    记入观察（arch §8.1 风险 14），不在本包定阈值。挂 human 部署轮
    （launch-card awaiting-human:live-deploy），本件只落 host 面可闭环部分。

===========================================================================
强度申报（契约条目 → 承载断言 → 性质档）：

  · E1 payload 携 user=<device_id>     → test_k2_e1_openai_patch_payload_shape_with_user → 行为级
    （ET 真跑补丁方法体、mock create 捕获实参）
  · E1 省略 user 回落旧行为（负锚）    → test_k2_e1_openai_patch_omit_user_legacy_fallback → 行为级
  · E1 命名参数 user（探针判据面）     → test_k2_e1_openai_patch_present_and_signatures → 机制级
  · E1 发送腿接线（user + 本回合切片） → test_k2_e1_connection_send_path_user_and_turn_scope → 源锚代理
  · E1 恰一条本回合 user message、
    无 history 重放（装配语义）        → test_k2_e1_turn_scope_pure_function_excludes_history → 行为级
    （ET 真跑切片纯函数；「请求 payload 整体形状」由本断言 + test 2 合成承载）
  · E2 RAM 副本保留、发送路径零重放    → test_k2_e2_ram_copy_retained_send_path_replay_free → 机制级
    （结构断言承载 host 面；前端不持历史 = 申报，见头注）
  · E3 bind-mount                      → test_k2_e3_compose_bind_mount_openai_patch → 机制级
  · E3 启动标记行 + 探针门控           → test_k2_e3_connection_marker_gated_by_provider_probe → 机制级
    （探针判据行为半 = 行为级：stock 签名 False / 补丁签名 True / 异常 False）
  · E3 manifest md5 登记 + 生效步提示  → test_k2_e3_manifest_registers_openai_patch_asset → 机制级
  · E4 两轮追问连续性（loopback）      → test_k2_e4_two_round_continuity_loopback_probe → 行为级
  · E4 省略 user 降级 + 双写形态判别
    （负例）                           → test_k2_e4_omit_user_degrades_and_double_write_rejected → 行为级
  · E5 R9 意图/思考档零触碰            → test_k2_e5_r9_intent_and_thinking_untouched → 机制级
    （thinking 半归 test 2 的 _apply_thinking_disabled 钩子断言）
  · E5 P5 接线零回退（must-stay-green）→ test_k2_e3_p5_schedule_wiring_not_regressed → 机制级
  · E5/E6 申报面自检                   → test_k2_e5_e6_declarations_exist → 机制级

缺口（承载断言强度低于契约要求处，逐条）：

  · connection.py 不可离线 import，发送腿接线为源锚代理：「chat() 内可静态
    追溯到切片调用 + user kwarg 在场」≠「运行时每轮真发恰一条本回合」；
    运行时行为由 E4 loopback 探针（以真实切片函数 + 真实补丁 payload 驱动
    仿真会话存储）+ K2-E6 live 轮双层兜底。行为等价假设显式化：① AST 调用
    点形态 ≈ 运行时数据流（内联与局部变量中转两种形态均容忍，其余包装形态
    会被判红——builder 须按头注契约接线）；② 补丁方法体对 mock client 的
    create 实参 ≈ 对真实 OpenClaw 的请求体（create(**request_params) 观察
    缝为 stock 形态，base 版漂移若改此缝属验收判法升级，走 tighten-only
    窗口经 planner 调整，不得静默绕过）。
  · OpenClaw 侧 resolveSessionKey / 持久会话存储行为不可离线实证——E4 仿真
    解析器照录部署版 bundle 语义（`user ? `${prefix}-user:${user}` :
    `${prefix}:${randomUUID()}``，键前缀 agent:kidpal:openai-user:）；真实
    键形与跨 WS 回忆命中归 K2-E6 live 轮。
  · 前端不持对话历史零断言（本包零触碰 front，见头注 K2-E2 申报）。
  · probe 门控的 marker 结构断言为源锚代理（「marker 所在函数含探针调用」
    ≈「探针失败不打 marker」；判据行为半由探针函数行为断言承载）。

fixture 自含性声明：全部断言直读 repo 内制品（connection.py / openai.py /
docker-compose.yml / manifest.yaml）+ 本文件内嵌消息 fixture；零网络 I/O、
零外部资产、零 datetime.now 依赖、零环境前提（PyYAML 仅 manifest 验收测试
case 用，与 verify-deploy.sh 同一解析器；缺席 = infra 红，与 K1 同口径）。
行为 seam 驱动方对账：切片纯函数驱动方 = 本件 host 断言；引擎→OpenClaw
请求的运行时驱动方 = 设备 WS 建连（offline 无驱动方 = 已申报盲区，E4 以
仿真存储 + 真实补丁 payload 驱动闭环 host 半）；「第二轮答对」模型行为
驱动方 = 真机轮（live-manual，K2-E6）。
host/真机近似假设：host 面证明「payload 形状正确 + 会话存储键形正确 +
本回合事实对第二轮可见」，是真机行为的差集覆盖（真机另证「模型真答对、
跨 WS 新会话真命中」）——差集已显式申报。

Run protocol（两点文件名须 importlib 模式）:
  cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance
  （standalone: python 本文件）

RED 基线（冻结时点树，2026-10-23，builder 未动工）：
  冻结前既有套件实跑（本树）= 278 passed / 4 deselected / 33 subtests
  passed 全绿（P5 后锚逐字命中）。
  设计态红（真实缺失行为，两个缺失源）：
    ① engine-patch/openai.py 不在世（补丁未建）→ E1 签名 / payload 携
       user / 省略回落 / thinking 钩子断言红；
    ② connection.py 持久会话接线不在世（切片函数 / 探针 / chat 接线 /
       门控 marker 未落）→ E1 发送腿接线 / 切片行为 / E2 / E3 标记行与
       探针 / E4 两轮与降级断言红；
    ③ 部署面登记不在世 → E3 compose bind-mount / manifest 条目断言红。
  must-stay-green = R9 nointent 字面 / P5 课表接线锚 / 申报面自检。
  冻结后实跑（2026-10-23 回填）：本件 11 failed / 3 passed / 0 error
  （红全部为上述三源设计态红，成因映射见上；执行通道 error 为零——
  §3 冻结即冒烟 + §4 C 类实编判据达成：收集零 error）；全量
  289 passed（278 既有 + 3 must-stay-green + 8 新增绿不计——实数见回填
  报告）/ 11 failed / 4 deselected / 33 subtests，既有套件零新增红。
  转绿路径已推演（§6b 断言设计透镜）：按 FROZEN OFFLINE CONTRACT 的
  合规实现（A/B/C/D 四面）逐条转绿——A 面落 openai.py 补丁转绿 4 条、
  B 面落 connection 接线转绿 5 条、C/D 面落部署面转绿 2 条。
  变异自检（surrogate 实现二向推演，2026-10-23 实跑在案：合规 8 项全 PASS、
  变异 9 项全 FAIL、0 error）：合规实现（切片函数 / 探针 / chat 接线 /
  门控 marker / openai 两腿）逐条全绿；变异 M1「每轮透传全量历史 + user」
  （双写禁区）新爆发送腿接线断言红（E1/E2 各一）；M2「切片函数返全量」
  新爆切片行为红 + 两轮探针「第二轮 payload 携带第一轮事实」红；M2b「切片
  丢 system 前缀」新爆前缀保留红；M3「openai 补丁丢 user 注入」新爆
  payload 红；M4「user=None 也写键」新爆回落负锚红；M5「max_tokens 无条件
  入请求」新爆禁带红；M6「marker 不经探针门控」新爆门控结构红。断言敏感
  性在案；自检过程中 harness 自身两处缺陷（exec 命名空间回传丢失 / E4 第二
  轮 fixture 未含 RAM 历史）已修正后复跑。

RED 基线命令（taskpackage redBaselineCommand 同源）:
  cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance
"""

from __future__ import annotations

import ast
import json
import re
import textwrap
import unittest
import uuid
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock

try:
    import yaml  # manifest 验收测试 case 前置；缺席 = infra 红（K1 同口径）
    _HAS_YAML = True
except ImportError:  # pragma: no cover
    _HAS_YAML = False

REPO_ROOT = Path(__file__).resolve().parents[2]
DEPLOY = REPO_ROOT / "server" / "deploy"
OPENAI_PATCH = DEPLOY / "engine-patch" / "openai.py"
CONNECTION_PY = DEPLOY / "engine-patch" / "connection.py"
COMPOSE_YML = DEPLOY / "docker-compose.yml"
MANIFEST_YAML = DEPLOY / "manifest.yaml"

# 冻结字面（变更 = 语义放宽/收紧，走 tighten-only 纪律反审）
K2_MARKER = "kidpal persistent-session patch active"
K2_PROBE_FN = "_kidpal_provider_supports_user"
K2_TURN_FN = "_kidpal_turn_messages"
K2_USER_FIELD = "user"
K2_OPENAI_CONTAINER_DST = "/opt/xiaozhi-esp32-server/core/providers/llm/openai/openai.py"
K2_MANIFEST_SRC = "server/deploy/engine-patch/openai.py"
K2_MANIFEST_DST = "/opt/kidpal/xiaozhi-esp32-server/deploy/engine-patch/openai.py"
# OpenClaw 持久会话键形（ADR-015 / 部署版 bundle resolveSessionKey：
# `const mainKey = user ? `${prefix}-user:${user}` : `${prefix}:${randomUUID()}``
# ，prefix = "agent:kidpal:openai"）。
K2_SESSION_KEY_PREFIX = "agent:kidpal:openai-user:"
K2_RESOLVER_PREFIX = "agent:kidpal:openai"

# 既有冻结验收件（K2-E5 零新增红纪律对象；仅头注申报 + 自检引用，零触碰）。
K2_FROZEN_NEIGHBORS = (
    "test_261010_engine_decision21_overlay_persist.acceptance.py",
    "test_prompt_v7_round_routing.acceptance.py",
    "test_261022_m5_k1_prompt_carrier_migration.acceptance.py",
    "test_261001_openclaw_model_name_overlay_drift.acceptance.py",
)


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


class _PermissiveGlobals(dict):
    """受限命名空间 exec 的宽容 globals：未提供的模块级名字回落 Mock
    （补丁方法体可能引用仓外引擎模块级名——base 版漂移容忍，payload 观察缝
    才是断言面）。"""


class _EmptyStream:
    """mock 流：可迭代零 chunk + close()（response/response_with_functions
    的 finally 均调 close；list 无 close 会假 error）。"""

    def __iter__(self):
        return self

    def __next__(self):
        raise StopIteration

    def close(self):
        pass


class _ProviderStub:
    """补丁方法体的 self 桩：已知属性给真值（payload 断言面），未知属性
    回落 Mock（base 版漂移容忍——头注缺口②申报）。"""

    def __init__(self):
        self.model_name = "deepseek-v4"
        self.max_tokens = None
        self.temperature = None
        self.top_p = None
        self.frequency_penalty = None
        self.create_calls = []
        self.thinking_calls = []
        client = Mock()
        client.chat.completions.create.side_effect = self._create
        self.client = client

    def _create(self, **kwargs):
        self.create_calls.append(kwargs)
        return _EmptyStream()

    def normalize_dialogue(self, dialogue):
        return list(dialogue)

    def _apply_thinking_disabled(self, request_params):
        self.thinking_calls.append(request_params)

    def __getattr__(self, name):  # base 版漂移容忍（头注缺口②）
        if name.startswith("_test_"):
            raise AttributeError(name)
        return Mock()


def _parse_ast(testcase, path: Path, tag: str):
    src = _read_text(path)
    testcase.assertTrue(
        src.strip(),
        f"{tag} RED: {path} 不在场/为空——冻结契约承载面缺失（设计态红，"
        "非收集错）。")
    try:
        return src, ast.parse(src)
    except SyntaxError as e:  # 补丁件必须可编译（§4 C 类实编同源判据）
        testcase.fail(f"{tag} RED: {path} 语法不可编译: {e}（C 类实编判据）。")


def _find_function(tree, name):
    for node in ast.walk(tree):
        if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)) \
                and node.name == name:
            return node
    return None


def _exec_segment(src: str, node, path: Path, extra_ns: dict | None = None):
    seg = ast.get_source_segment(src, node)
    ns = _PermissiveGlobals()
    if extra_ns:
        ns.update(extra_ns)
    exec(compile(textwrap.dedent(seg), str(path), "exec"), ns)  # noqa: S102
    return ns


def _named_param_names(fn_node):
    """签名命名参数名集（含 kwonly；**kwargs 不算命名参数——探针判据面）。"""
    a = fn_node.args
    names = [x.arg for x in list(a.args) + list(a.kwonlyargs)]
    return names


def _is_self_attr(node, attr):
    return (isinstance(node, ast.Attribute) and node.attr == attr
            and isinstance(node.value, ast.Name) and node.value.id == "self")


def _load_connection(testcase, tag: str):
    if not CONNECTION_PY.is_file():
        testcase.fail(f"{tag} RED(infra): {CONNECTION_PY} 不在世。")
    src, tree = _parse_ast(testcase, CONNECTION_PY, tag)
    return src, tree


# --------------------------------------------------------------------------- #
# 消息 fixture（E1 切片行为 / E4 两轮连续性共用；第一轮事实 = 「紫色」）
# --------------------------------------------------------------------------- #

K2_SYS = {"role": "system",
          "content": "动态块：2026-10-23 课表摘要（略）；静态语义在 AGENTS.md。"}
K2_U_TURN1 = {"role": "user", "content": "我最喜欢的颜色是紫色。"}
K2_A_TURN1 = {"role": "assistant", "content": "紫色记下啦！"}
K2_U_TURN2 = {"role": "user", "content": "你还记得我最喜欢什么颜色吗？"}
K2_TURN1_FACT = "紫色"


def _openclaw_resolve_session_key(payload, uuid_fn=uuid.uuid4):
    """离线仿真 OpenClaw 会话键解析器（部署版 bundle 语义照录）：
    const mainKey = user ? `${prefix}-user:${user}` : `${prefix}:${randomUUID()}`"""
    user = payload.get(K2_USER_FIELD)
    if user:
        return f"{K2_RESOLVER_PREFIX}-user:{user}"
    return f"{K2_RESOLVER_PREFIX}:{uuid_fn()}"


# --------------------------------------------------------------------------- #
# K2-E1 · payload 形状（openai.py 部署副本补丁）
# --------------------------------------------------------------------------- #


class K2PersistentSessionAcceptance(unittest.TestCase):

    maxDiff = None

    # ---------------- openai.py 补丁（签名 + payload 行为） ---------------- #

    def _load_openai_methods(self, tag, names):
        """AST 抽取 openai.py 补丁方法体 + 受限命名空间 exec（ET 先例）。
        缺文件 / 缺方法 = 设计态 fail（非 error）。"""
        src, tree = _parse_ast(self, OPENAI_PATCH, tag)
        ns = _PermissiveGlobals()
        for name in names:
            node = _find_function(tree, name)
            self.assertIsNotNone(
                node,
                f"{tag} RED: engine-patch/openai.py 无方法 {name} —— 补丁面"
                "缺失（部署副本补丁须含 response 与 response_with_functions "
                "两发送腿）。")
            ns = _exec_segment(src, node, OPENAI_PATCH, ns)
            self.assertTrue(
                callable(ns.get(name)),
                f"{tag} RED(infra): {name} exec 后不可调用。")
        return ns

    def test_k2_e1_openai_patch_present_and_signatures(self):
        """coverage: K2-E1 — engine-patch/openai.py 在世、可编译；response 与
        response_with_functions 签名均含命名参数 user（缺省形态不作值约束，
        命名在位是漂移探针 _kidpal_provider_supports_user 的判据——**kwargs
        吞参不满足）。"""
        tag = "K2-E1"
        src, tree = _parse_ast(self, OPENAI_PATCH, tag)
        for name in ("response", "response_with_functions"):
            node = _find_function(tree, name)
            self.assertIsNotNone(
                node,
                f"{tag} RED: openai.py 无方法 {name} —— 两发送腿须都打补丁。")
            params = _named_param_names(node)
            self.assertIn(
                K2_USER_FIELD, params,
                f"{tag} RED: {name} 签名无命名参数 {K2_USER_FIELD!r}（实得 "
                f"{params}）——**kwargs 吞 user 会被 stock provider 静默丢弃，"
                "持久会话键形退化为随机 session（探针判据面失守）。")

    def test_k2_e1_openai_patch_payload_shape_with_user(self):
        """coverage: K2-E1 — 真跑补丁方法体（mock client 捕获 create 实参）：
        携 user → request_params["user"] 原样在位；messages 原样透传（零历史
        注入零裁剪）；stream True；model 在位；零 max_tokens（契约 §四禁带
        硬约束）；thinking 关闭钩子照常施加（R9）。"""
        tag = "K2-E1"
        ns = self._load_openai_methods(tag, ["response", "response_with_functions"])
        dialogue = [dict(K2_SYS), dict(K2_U_TURN2)]
        device_id = "dev-pad-7"

        for name, extra in (("response", {}),
                            ("response_with_functions",
                             {"functions": [{"type": "function",
                                             "function": {"name": "t"}}]})):
            stub = _ProviderStub()
            gen = ns[name](stub, "sess-1", dialogue, user=device_id, **extra)
            for _ in gen:  # 驱动生成器（空流，零 chunk）
                pass
            self.assertEqual(
                len(stub.create_calls), 1,
                f"{tag} RED: {name} 未恰一次调用 create —— 请求装配缝偏移。")
            kw = stub.create_calls[0]
            self.assertEqual(
                kw.get(K2_USER_FIELD), device_id,
                f"{tag} RED: {name} 请求体未携 user={device_id!r}（实得 "
                f"{kw.get(K2_USER_FIELD)!r}）—— OpenClaw 持久会话键无标识，"
                "跨会话回忆底座失效（K2-E1 本体）。")
            self.assertEqual(
                kw.get("messages"), dialogue,
                f"{tag} RED: {name} messages 未原样透传（provider 侧注入/裁剪"
                "历史 = payload 忠实面被破，历史策略必须全归 connection 侧）。")
            self.assertIs(kw.get("stream"), True,
                          f"{tag} RED: {name} stream 非 True。")
            self.assertIn("model", kw, f"{tag} RED: {name} 缺 model。")
            self.assertNotIn(
                "max_tokens", kw,
                f"{tag} RED: {name} 请求携带 max_tokens —— 契约 §四「对 "
                "OpenClaw 的流式请求禁带 max_tokens（实测触发 agent "
                "incomplete-turn 报错）」硬约束被回退。")
            self.assertEqual(
                len(stub.thinking_calls), 1,
                f"{tag} RED: {name} 未施加 thinking 关闭钩子 —— R9（thinking "
                "维持关闭）零触碰被破。")

    def test_k2_e1_openai_patch_omit_user_legacy_fallback(self):
        """coverage: K2-E1 — 向后兼容负锚（契约 §四定案）：省略 user /
        user=None / user="" → 请求体**不含** user 键（OpenClaw 回落旧行为：
        每请求随机 session、调用方自携带历史）。「没报错就算过」不成立——
        断言键的缺席本身。"""
        tag = "K2-E1"
        ns = self._load_openai_methods(tag, ["response", "response_with_functions"])
        dialogue = [dict(K2_SYS), dict(K2_U_TURN1)]
        for name in ("response", "response_with_functions"):
            for kwargs in ({}, {K2_USER_FIELD: None}, {K2_USER_FIELD: ""}):
                stub = _ProviderStub()
                gen = ns[name](stub, "sess-1", dialogue, **kwargs)
                for _ in gen:
                    pass
                self.assertEqual(len(stub.create_calls), 1,
                                 f"{tag} RED(infra): {name} 零 create 调用。")
                kw = stub.create_calls[0]
                self.assertNotIn(
                    K2_USER_FIELD, kw,
                    f"{tag} RED: {name} 在 user={kwargs.get(K2_USER_FIELD)!r} "
                    "时仍向请求写 user 键 —— 省略 user 必须回落旧行为（每请求"
                    "随机 session），把 None/空串写成键会给 OpenClaw 留下可被"
                    "当作用户身份的脏键（向后兼容负锚）。")

    # ---------------- connection.py 发送腿接线（源锚 + 切片行为） ---------- #

    def _chat_llm_call_sites(self, testcase, tag):
        src, tree = _load_connection(self, tag)
        chat = _find_function(tree, "chat")
        self.assertIsNotNone(chat, f"{tag} RED(infra): 无 chat 方法。")
        sites = []
        for node in ast.walk(chat):
            if not isinstance(node, ast.Call):
                continue
            func = node.func
            if (isinstance(func, ast.Attribute)
                    and func.attr in ("response", "response_with_functions")
                    and isinstance(func.value, ast.Attribute)
                    and func.value.attr == "llm"
                    and isinstance(func.value.value, ast.Name)
                    and func.value.value.id == "self"):
                sites.append(node)
        testcase.assertTrue(
            sites,
            f"{tag} RED(infra): chat() 零 self.llm.response(_with_functions) "
            "调用点——LLM 发送腿失位。")
        return src, tree, chat, sites

    def _turn_scope_wired(self, chat, arg_node):
        """messages 实参可静态追溯到 _kidpal_turn_messages 调用：子树直接含
        该调用，或实参为「由含该调用的赋值中转的同名局部变量」。"""
        for sub in ast.walk(arg_node):
            if isinstance(sub, ast.Call):
                f = sub.func
                if (isinstance(f, (ast.Name, ast.Attribute))
                        and getattr(f, "id", getattr(f, "attr", None))
                        == K2_TURN_FN):
                    return True
        if isinstance(arg_node, ast.Name):
            for node in ast.walk(chat):
                if isinstance(node, ast.Assign):
                    for t in node.targets:
                        if isinstance(t, ast.Name) and t.id == arg_node.id:
                            for sub in ast.walk(node.value):
                                if isinstance(sub, ast.Call):
                                    f = sub.func
                                    if (isinstance(f, (ast.Name, ast.Attribute))
                                            and getattr(
                                                f, "id",
                                                getattr(f, "attr", None))
                                            == K2_TURN_FN):
                                        return True
        return False

    def test_k2_e1_connection_send_path_user_and_turn_scope(self):
        """coverage: K2-E1 — chat() 全部 LLM 发送腿：messages 实参可静态追溯
        到 _kidpal_turn_messages（本回合切片唯一装配点）且不得是裸
        get_llm_dialogue_with_memory（「每轮透传历史 + user」双写禁区，
        ADR-015 明令禁止）；每个调用点携 user=self.device_id。"""
        tag = "K2-E1"
        _src, _tree, _chat, sites = self._chat_llm_call_sites(self, tag)
        for idx, site in enumerate(sites, 1):
            user_kw = [k for k in site.keywords if k.arg == K2_USER_FIELD]
            self.assertTrue(
                user_kw,
                f"{tag} RED: 第 {idx} 个 LLM 发送腿（{site.func.attr}）未携 "
                f"{K2_USER_FIELD}= 参数 —— OpenClaw 持久会话键无标识"
                "（K2-E1 本体）。")
            self.assertTrue(
                _is_self_attr(user_kw[0].value, "device_id"),
                f"{tag} RED: 第 {idx} 个发送腿 {K2_USER_FIELD}= 实参不是 "
                "self.device_id —— 键来源必须为设备 id（全列车共享常量，"
                "禁 sentinel user 变体）。")
            self.assertGreaterEqual(
                len(site.args), 2,
                f"{tag} RED(infra): 第 {idx} 个发送腿位置实参不足 2（session_id "
                "+ messages）。")
            arg = site.args[1]
            self.assertFalse(
                isinstance(arg, ast.Call)
                and isinstance(arg.func, ast.Attribute)
                and arg.func.attr == "get_llm_dialogue_with_memory",
                f"{tag} RED: 第 {idx} 个发送腿的 messages 实参是裸 "
                "get_llm_dialogue_with_memory —— 「每轮透传全量历史 + user」"
                "双写形态（ADR-015 明令禁止：同一段话重放两次的双历史漂移）。")
            self.assertTrue(
                self._turn_scope_wired(_chat, arg),
                f"{tag} RED: 第 {idx} 个发送腿的 messages 实参不可静态追溯到 "
                f"{K2_TURN_FN} 调用 —— 本回合切片装配点未接线（历史重放禁令"
                "无承载；FROZEN OFFLINE CONTRACT B 项：直接内联或本函数内局部"
                "变量中转均可）。")

    def test_k2_e1_turn_scope_pure_function_excludes_history(self):
        """coverage: K2-E1 — _kidpal_turn_messages 行为级（ET 真跑）：
        返回 = 会话级前缀（system 等）+ 恰本回合尾部消息；先前回合的 user/
        assistant 消息一律不进返回值（无 history 重放——ADR-015 硬语义）；
        正常提问轮恰一条本回合 user message；在途 tool 链（keep_tail_count>1）
        整段保留；非法下标 ValueError。"""
        tag = "K2-E1"
        _src, tree = _load_connection(self, tag)
        node = _find_function(tree, K2_TURN_FN)
        self.assertIsNotNone(
            node,
            f"{tag} RED: connection.py 无模块级纯函数 {K2_TURN_FN} —— 本回合"
            "切片装配点缺失（FROZEN OFFLINE CONTRACT B 项，名字与签名冻结）。")
        ns = _exec_segment(_read_text(CONNECTION_PY), node, CONNECTION_PY)
        fn = ns[K2_TURN_FN]

        # 两轮场景：assembled = [system, u1, a1, u2]；只保留前缀 + 本回合 u2。
        assembled = [dict(K2_SYS), dict(K2_U_TURN1), dict(K2_A_TURN1),
                     dict(K2_U_TURN2)]
        out = fn(assembled, 1, 1)
        self.assertEqual(
            out, [dict(K2_SYS), dict(K2_U_TURN2)],
            f"{tag} RED: 切片产物 {out} ≠ [system, 本回合 user] —— 恰一条本"
            "回合 user message、零 history 重放被破。")
        users = [m for m in out if m.get("role") == "user"]
        self.assertEqual(
            len(users), 1,
            f"{tag} RED: 切片产物含 {len(users)} 条 user message —— 引擎请求"
            "须恰一条本回合 user message（K2-E1 判据本体）。")
        self.assertEqual(
            users[0].get("content"), K2_U_TURN2["content"],
            f"{tag} RED: 唯一 user message 不是本回合提问。")
        dumped = json.dumps(out, ensure_ascii=False)
        self.assertNotIn(
            K2_TURN1_FACT, dumped,
            f"{tag} RED: 切片产物携带先前回合事实（{K2_TURN1_FACT!r}）—— "
            "引擎向 OpenClaw 重放历史（双历史漂移，ADR-015 硬语义）。")
        self.assertTrue(
            out and out[0].get("role") == "system",
            f"{tag} RED: 切片产物丢会话级前缀（system）—— prompt 装配层"
            "（动态块 / memory / speaker 注入）被切片误伤。")

        # 在途 tool 链：keep_tail_count=3 整段保留（assistant tool_calls +
        # tool + 后续），先前回合仍排除。
        tool_chain = [
            {"role": "assistant",
             "tool_calls": [{"id": "t1", "function": {"name": "read"}}]},
            {"role": "tool", "tool_call_id": "t1", "content": "内容"},
        ]
        assembled3 = [dict(K2_SYS), dict(K2_U_TURN1), dict(K2_A_TURN1),
                      dict(K2_U_TURN2)] + tool_chain
        out3 = fn(assembled3, 1, 3)
        self.assertEqual(
            out3, [dict(K2_SYS), dict(K2_U_TURN2)] + tool_chain,
            f"{tag} RED: 本回合在途 tool 链被切片截断（{out3}）—— 递归子轮"
            "消息从未持久化到 OpenClaw，截断 = 模型失上下文。")

        # 边界：keep_tail_count=0 → 仅前缀；非法下标 → ValueError。
        self.assertEqual(fn(assembled, 1, 0), [dict(K2_SYS)],
                         f"{tag} RED: keep_tail_count=0 应仅返前缀。")
        with self.assertRaises(ValueError):
            fn(assembled, -1, 1)
        with self.assertRaises(ValueError):
            fn(assembled, 1, 99)

    # ------------------------------------------------------------------ #
    # K2-E2 · 双历史单一权威（host 面 = 引擎发送路径零重放，结构承载）
    # ------------------------------------------------------------------ #

    def test_k2_e2_ram_copy_retained_send_path_replay_free(self):
        """coverage: K2-E2 — ADR-015 降级语义结构承载：① chat() 的
        dialogue.put 照旧保留（引擎 RAM Dialogue = 本地非权威副本继续记账：
        标题生成 / 调试日志——是降级不是删除）；② 装配源 get_llm_dialogue_
        with_memory 仍在 chat() 内被调用（memory/voiceprint/speaker 注入的
        prompt 装配层零触碰，切片只作用于其产物）；③ 发送腿接线为切片产物
        （全量装配产物不直连 LLM 腿 = RAM 历史不重放；与 K2-E1 接线断言同
        缘，E2 独立复断以自立）。"""
        tag = "K2-E2"
        src, tree, chat, sites = self._chat_llm_call_sites(self, tag)
        put_calls = [
            n for n in ast.walk(chat)
            if isinstance(n, ast.Call) and isinstance(n.func, ast.Attribute)
            and n.func.attr == "put" and _is_self_attr(n.func.value, "dialogue")]
        self.assertTrue(
            put_calls,
            f"{tag} RED: chat() 零 dialogue.put —— RAM 副本被删（ADR-015 是"
            "「降级为非权威副本」，本地簿记仍须记账：标题生成/调试日志）。")
        asm_calls = [
            n for n in ast.walk(chat)
            if isinstance(n, ast.Call) and isinstance(n.func, ast.Attribute)
            and n.func.attr == "get_llm_dialogue_with_memory"]
        self.assertTrue(
            asm_calls,
            f"{tag} RED: chat() 不再调用 get_llm_dialogue_with_memory —— "
            "prompt 装配层（memory/voiceprint/speaker 注入）被旁路（本包只改"
            "历史与 user 字段、不改装配层；装配产物仍须是切片的输入源）。")
        for idx, site in enumerate(sites, 1):
            arg = site.args[1]
            self.assertFalse(
                isinstance(arg, ast.Call)
                and isinstance(arg.func, ast.Attribute)
                and arg.func.attr == "get_llm_dialogue_with_memory",
                f"{tag} RED: 第 {idx} 个发送腿直连全量装配产物 —— RAM 历史"
                "向 OpenClaw 重放（双历史单一权威被破）。")
            self.assertTrue(
                self._turn_scope_wired(chat, arg),
                f"{tag} RED: 第 {idx} 个发送腿未走本回合切片 —— 唯一可重放"
                "历史必须只在 OpenClaw 持久会话（键 "
                f"{K2_SESSION_KEY_PREFIX}<device_id>），引擎侧发送路径零 "
                "history 重放。")

    # ------------------------------------------------------------------ #
    # K2-E3 · 补丁漂移护栏（bind-mount / 启动标记行 / manifest）
    # ------------------------------------------------------------------ #

    def test_k2_e3_compose_bind_mount_openai_patch(self):
        """coverage: K2-E3 — docker-compose.yml 新增 openai.py bind-mount
        （宿主部署副本 → 镜像内 core/providers/llm/openai/openai.py）；既有
        connection.py 与 kidpal_display_strip.py 两行零回退。"""
        tag = "K2-E3"
        src = _read_text(COMPOSE_YML)
        self.assertTrue(src.strip(), f"{tag} RED: docker-compose.yml 不在场。")
        lines = [ln.strip() for ln in src.splitlines()]
        mount = [ln for ln in lines
                 if "engine-patch/openai.py" in ln
                 and K2_OPENAI_CONTAINER_DST in ln]
        self.assertTrue(
            mount,
            f"{tag} RED: docker-compose.yml 零 openai.py bind-mount（须为 "
            "./engine-patch/openai.py → {K2_OPENAI_CONTAINER_DST}）—— 补丁"
            "不在容器内真实运行，持久会话全功能静默回退（ARQ-260907 同族）。",
        )
        for required in ("engine-patch/connection.py", "kidpal_display_strip.py"):
            self.assertTrue(
                any(required in ln and "/opt/xiaozhi-esp32-server/core/" in ln
                    for ln in lines),
                f"{tag} RED: docker-compose.yml 丢失既有 bind-mount {required!r}"
                " —— P1/P5 部署面被回退。")

    def test_k2_e3_connection_marker_gated_by_provider_probe(self):
        """coverage: K2-E3 — 启动标记行（补丁在位唯一机械报警锚，ARQ-260907
        同族）：_initialize_components 含 marker 字面
        "kidpal persistent-session patch active"，且 marker 所在函数调用
        _kidpal_provider_supports_user 探针（openai.py bind-mount 丢失时
        stock provider 静默吞 user kwargs = 全功能回退，无门控的 marker 是
        假绿报警锚）；探针函数行为级断言：补丁签名 True / stock 签名 False /
        无 response False（绝不抛）。"""
        tag = "K2-E3"
        src, tree = _load_connection(self, tag)
        self.assertIn(
            K2_MARKER, src,
            f"{tag} RED: connection.py 零启动标记行 {K2_MARKER!r} —— 补丁在位"
            "唯一机械报警锚缺失（ARQ-260907 同族护栏）。")
        init = _find_function(tree, "_initialize_components")
        self.assertIsNotNone(
            init, f"{tag} RED(infra): 无 _initialize_components。")
        init_src = ast.get_source_segment(src, init) or ""
        self.assertIn(
            K2_MARKER, init_src,
            f"{tag} RED: 启动标记行不在 _initialize_components 内 —— 须随组件"
            "初始化打印（既有三条 marker 同位先例）。")
        self.assertIn(
            K2_PROBE_FN, src,
            f"{tag} RED: connection.py 无 {K2_PROBE_FN} —— 补丁在位探针缺失"
            "（openai.py bind-mount 丢失 = stock provider 静默吞 user kwargs，"
            "全功能回退而标记照打 = 假绿报警锚）。")
        self.assertIn(
            K2_PROBE_FN, init_src,
            f"{tag} RED: _initialize_components 未调用 {K2_PROBE_FN} —— 启动"
            "标记行未由探针门控（必须探针通过才打 marker、失败打 error 报警）。")

        # 探针行为级（ET 真跑）。
        probe_node = _find_function(tree, K2_PROBE_FN)
        self.assertIsNotNone(probe_node,
                             f"{tag} RED(infra): {K2_PROBE_FN} 无函数定义。")
        pns = _exec_segment(src, probe_node, CONNECTION_PY,
                            {"inspect": __import__("inspect")})
        probe = pns[K2_PROBE_FN]

        class _Patched:
            def response(self, session_id, dialogue, *, user=None, **kwargs):
                return user

        class _Stock:
            def response(self, session_id, dialogue, **kwargs):
                return dialogue

        class _Broken:
            pass

        self.assertTrue(
            probe(_Patched()),
            f"{tag} RED: 探针对补丁签名（命名 user 参数）误判 False —— 补丁在位"
            "却报警 = 假红。")
        self.assertFalse(
            probe(_Stock()),
            f"{tag} RED: 探针对 stock 签名（**kwargs 吞参）误判 True —— "
            "openai.py bind-mount 丢失时标记照打 = 假绿报警锚（护栏本体失守）。")
        self.assertFalse(
            probe(_Broken()),
            f"{tag} RED: 探针对无 response 对象抛异常/误判 —— 反射失败必须"
            "回落 False（绝不抛）。")

    def test_k2_e3_manifest_registers_openai_patch_asset(self):
        """coverage: K2-E3 — manifest.yaml 登记新资产（verify: md5、
        restart: engine、note 含 docker compose up -d 人工生效步提示——照
        docker-compose.yml 条目既有纪律：新增 bind-mount 首次生效须容器重建，
        deploy.sh 只做 docker restart 不自动重建）；既有 engine-patch 两件
        登记零回退。verify-deploy.sh 恒 manifest 驱动，md5 核验接线 = 本条目。"""
        tag = "K2-E3"
        self.assertTrue(
            _HAS_YAML or self.fail(
                f"{tag} RED(infra): PyYAML 不在位——manifest 解析前置缺失。"))
        self.assertTrue(MANIFEST_YAML.is_file(),
                        f"{tag} RED: manifest.yaml 不在场。")
        try:
            doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
        except Exception as e:  # noqa: BLE001 —— 解析错误是 RED，不是 crash
            self.fail(f"{tag} RED: manifest.yaml 解析失败: {e!r}")
        entries = [e for e in (doc or {}).get("entries", []) or []
                   if isinstance(e, dict)]
        target = [e for e in entries if str(e.get("src") or "") == K2_MANIFEST_SRC]
        self.assertEqual(
            len(target), 1,
            f"{tag} RED: manifest 对 {K2_MANIFEST_SRC} 的登记条目 "
            f"{len(target)} != 1 —— 新部署资产未入部署单一事实源（261013 纪律："
            "登记并接入 verify-deploy.sh md5 核验）。")
        entry = target[0]
        self.assertEqual(
            entry.get("dst"), K2_MANIFEST_DST,
            f"{tag} RED: manifest dst = {entry.get('dst')!r}，须为 "
            f"{K2_MANIFEST_DST!r}（deploy clone 内 bind-mount 源位）。")
        self.assertEqual(
            entry.get("verify"), "md5",
            f"{tag} RED: manifest verify = {entry.get('verify')!r}，须为 md5"
            "（verify-deploy.sh 行尾归一 md5 比对——核验接线的本体）。")
        self.assertEqual(
            entry.get("restart"), "engine",
            f"{tag} RED: manifest restart = {entry.get('restart')!r}，须为 "
            "engine（补丁生效面 = 引擎重启）。")
        self.assertIn(
            "docker compose up -d", str(entry.get("note") or ""),
            f"{tag} RED: 条目 note 未含「docker compose up -d」人工生效步提示"
            " —— 新增 bind-mount 后首次生效须容器重建（= 设备全断、人工窗口），"
            "deploy.sh 只做 docker restart 不自动重建（既有纪律，生效步失报"
            " = 部署轮全功能回退）。")
        srcs = {str(e.get("src") or "") for e in entries}
        for required in ("server/deploy/engine-patch/connection.py",
                         "server/deploy/engine-patch/kidpal_display_strip.py"):
            self.assertIn(
                required, srcs,
                f"{tag} RED: manifest 丢失既有登记 {required!r} —— P1/P5 "
                "部署面被回退。")

    # ------------------------------------------------------------------ #
    # K2-E4 · 两轮追问连续性探针（loopback / 离线仿真 OpenClaw 会话存储）
    # ------------------------------------------------------------------ #

    def test_k2_e4_two_round_continuity_loopback_probe(self):
        """coverage: K2-E4 — 两轮追问连续性（host 半，行为级）：同 device_id
        两轮——每轮 payload 由真实切片函数装配（零 history 重放、恰一条本回合
        user message）；仿真 OpenClaw 按 user 键解析持久会话键（部署版 bundle
        resolveSessionKey 同形）并累积会话消息；第二轮键 == 第一轮键，第一轮
        事实在第二轮请求时已在持久会话存储内可见（「第二轮答对第一轮信息」的
        host 可证半——模型真的答对 = K2-E6 live）。"""
        tag = "K2-E4"
        _src, tree = _load_connection(self, tag)
        node = _find_function(tree, K2_TURN_FN)
        self.assertIsNotNone(
            node, f"{tag} RED: {K2_TURN_FN} 不在世 —— 两轮探针无切片装配点。")
        fn = _exec_segment(_read_text(CONNECTION_PY), node,
                           CONNECTION_PY)[K2_TURN_FN]

        device_id = "dev-pad-7"
        store: dict = {}

        def server_round(payload):
            key = _openclaw_resolve_session_key(payload)
            store.setdefault(key, []).extend(
                m for m in payload["messages"] if m.get("role") != "system")
            return key

        # 第一轮：孩子说事实（assembled = [system, u1]）。
        payload1 = {
            K2_USER_FIELD: device_id,
            "messages": fn([dict(K2_SYS), dict(K2_U_TURN1)], 1, 1),
        }
        key1 = server_round(payload1)
        self.assertEqual(
            key1, K2_SESSION_KEY_PREFIX + device_id,
            f"{tag} RED: 持久会话键 {key1!r} ≠ {K2_SESSION_KEY_PREFIX}"
            f"{device_id!r} —— user=<device_id> 语义与键形失配（ADR-015）。")
        self.assertTrue(
            any(K2_TURN1_FACT in json.dumps(m, ensure_ascii=False)
                for m in store[key1]),
            f"{tag} RED: 第一轮事实未入持久会话存储 —— 仿真失真（测试自身"
            "缺陷）。")

        # 第二轮：同 user 追问第一轮信息（AC1 同会话形态——引擎 RAM 装配
        # 产物仍含第一轮消息，切片必须把它裁掉；跨 WS 新会话的键保持由
        # 两轮同键断言 + K2-E6 live 复证）。
        assembled2 = [dict(K2_SYS), dict(K2_U_TURN1), dict(K2_A_TURN1),
                      dict(K2_U_TURN2)]
        payload2 = {
            K2_USER_FIELD: device_id,
            "messages": fn(assembled2, 1, 1),
        }
        dumped2 = json.dumps(payload2["messages"], ensure_ascii=False)
        self.assertNotIn(
            K2_TURN1_FACT, dumped2,
            f"{tag} RED: 第二轮 payload 携带第一轮事实 —— 引擎重放历史（"
            "双历史漂移；连续性必须由服务端持久会话承载）。")
        key2 = server_round(payload2)
        self.assertEqual(
            key2, key1,
            f"{tag} RED: 同 user 两轮落不同持久会话键（{key1!r} vs {key2!r}）"
            " —— 跨会话连续性断裂。")
        self.assertTrue(
            any(K2_TURN1_FACT in json.dumps(m, ensure_ascii=False)
                for m in store[key2]),
            f"{tag} RED: 第二轮请求时第一轮事实不在持久会话存储内 —— 第二轮"
            "答对第一轮信息无料可答（host 半失守）。")

    def test_k2_e4_omit_user_degrades_and_double_write_rejected(self):
        """coverage: K2-E4 — 降级路径与负例：① 省略 user → resolveSessionKey
        回落随机 session，两轮键互异、零连续性（旧行为不崩线）；② 显式负例
        「每轮透传历史 + user」形态（双写）不满足冻结 payload 判据——第二轮
        payload 须恰一条 user message，双写形态的 user message 计数 > 1，
        被本判据判别在场。"""
        tag = "K2-E4"
        _src, tree = _load_connection(self, tag)
        node = _find_function(tree, K2_TURN_FN)
        self.assertIsNotNone(node, f"{tag} RED: {K2_TURN_FN} 不在世。")
        fn = _exec_segment(_read_text(CONNECTION_PY), node,
                           CONNECTION_PY)[K2_TURN_FN]

        # ① 省略 user：随机 session、零连续性。
        store: dict = {}

        def server_round(payload, uuid_fn):
            key = _openclaw_resolve_session_key(payload, uuid_fn)
            store.setdefault(key, []).extend(
                m for m in payload["messages"] if m.get("role") != "system")
            return key

        seq = iter(["uuid-a", "uuid-b", "uuid-c"])
        legacy1 = {"messages": fn([dict(K2_SYS), dict(K2_U_TURN1)], 1, 1)}
        legacy2 = {"messages": fn([dict(K2_SYS), dict(K2_U_TURN2)], 1, 1)}
        key1 = server_round(legacy1, lambda: next(seq))
        key2 = server_round(legacy2, lambda: next(seq))
        self.assertNotIn(
            K2_SESSION_KEY_PREFIX, key1,
            f"{tag} RED: 省略 user 的请求键仍带 user 段 —— 降级路径失真。")
        self.assertNotEqual(
            key1, key2,
            f"{tag} RED: 省略 user 两轮落同键 —— 降级语义应为每请求随机 "
            "session（旧行为）。")
        self.assertFalse(
            any(K2_TURN1_FACT in json.dumps(m, ensure_ascii=False)
                for m in store[key2]),
            f"{tag} RED: 降级路径第二轮竟见第一轮事实 —— 随机 session 语义"
            "被仿真失真（测试自身缺陷）。")

        # ② 双写禁区负例：「每轮透传历史 + user」的第二轮 payload（全量
        # assembled + user 键）违反冻结判据「恰一条本回合 user message」。
        assembled_full = [dict(K2_SYS), dict(K2_U_TURN1), dict(K2_A_TURN1),
                          dict(K2_U_TURN2)]
        forbidden = {K2_USER_FIELD: "dev-pad-7", "messages": assembled_full}
        forbidden_users = [m for m in forbidden["messages"]
                           if m.get("role") == "user"]
        self.assertNotEqual(
            len(forbidden_users), 1,
            f"{tag} RED(fixture 自检): 双写形态 user message 计数恰为 1 —— "
            "负例构造失真（测试自身缺陷）。")
        compliant = {K2_USER_FIELD: "dev-pad-7",
                     "messages": fn(assembled_full, 1, 1)}
        self.assertEqual(
            len([m for m in compliant["messages"]
                 if m.get("role") == "user"]), 1,
            f"{tag} RED: 合规切片产物 user message 计数 ≠ 1 —— 判据面失真。")

    # ------------------------------------------------------------------ #
    # K2-E5 · 既有验收零新增红 / R9 零触碰 / P5 接线零回退
    # ------------------------------------------------------------------ #

    def test_k2_e5_r9_intent_and_thinking_untouched(self):
        """coverage: K2-E5 — R9 裁定（Intent 维持 nointent、thinking 维持
        关闭、分回合 thinking 不纳入）零触碰：connection.py 缺省
        intent_type = "nointent" 字面在场（本包不改意图档）；thinking 关闭半
        由 test_k2_e1_openai_patch_payload_shape_with_user 的
        _apply_thinking_disabled 钩子断言承载。"""
        tag = "K2-E5"
        src, tree = _load_connection(self, tag)
        self.assertIn(
            'self.intent_type = "nointent"',
            src,
            f"{tag} RED: connection.py 丢失 self.intent_type = \"nointent\" "
            "缺省 —— R9（意图档零触碰）被破。")
        init = _find_function(tree, "_initialize_intent")
        self.assertIsNotNone(
            init, f"{tag} RED(infra): 无 _initialize_intent（意图档装配面"
            "失位）。")

    def test_k2_e3_p5_schedule_wiring_not_regressed(self):
        """coverage: K2-E5 — P5 课表机械注入接线零回退（plan.md 跨包事实：
        本包 rebase 须保留 P5 动态装配段，不得整体覆盖）：connection.py 保留
        schedule-summary 三条标记 / marker / 占位 / 帧字面；compose 既有两件
        bind-mount 零回退（与 E3 compose 断言互补，独立复断）。"""
        tag = "K2-E5"
        src = _read_text(CONNECTION_PY)
        for anchor in ("kidpal schedule-summary patch active",
                       "kidpal_schedule_summary",
                       "{{schedule_summary}}"):
            self.assertIn(
                anchor, src,
                f"{tag} RED: connection.py 丢失 P5 接线锚 {anchor!r} —— 课表"
                "机械注入被本包覆盖回退（跨包共享冻结面禁令）。")

    def test_k2_e5_e6_declarations_exist(self):
        """coverage: K2-E5, K2-E6 — 申报面自检（机制级，防申报段被静默删除）：
        文件头注含 K2-E5 零新增红纪律钉行（四件既有冻结件名在场）与 K2-E6
        live 观测项申报（真机跨 WS 回忆 + 持久会话增长/auto-compaction 观察，
        挂 human 部署轮）。本断言只守申报在位。"""
        tag = "K2-E5/E6"
        src = _read_text(Path(__file__))
        self.assertIn("K2-E5", src,
                      f"{tag} RED: 头注 K2-E5 申报段被删。")
        self.assertIn("K2-E6", src,
                      f"{tag} RED: 头注 K2-E6 live 观测项申报被删。")
        for frozen in K2_FROZEN_NEIGHBORS:
            self.assertIn(
                frozen, src,
                f"{tag} RED: 头注零触碰钉行丢失 {frozen} —— K2-E5 纪律对象"
                "失指。")
        self.assertIn("auto-compaction", src,
                      f"{tag} RED: K2-E6 申报丢失 auto-compaction 观察口径。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
