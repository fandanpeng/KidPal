"""Frozen acceptance tests — 261022-m5-k1-prompt-carrier-migration K1-E4
（**2026-10-24 锚换代**）+ 261024-memory-recall-tool-enable K1-E4-B
(subsystem: server/deploy/261022-openclaw-kidpal-tools/ +
server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md；TaskPackage =
delivery/taskpackages/261024-memory-recall-tool-enable/taskpackage.json；
上游 = delivery/review/261022-arch-review-device-memory-prompt-overhaul/
k4-recall-architecture-20261024.md §0 / §B 选项 3 / §E + human 2026-10-24 四裁决
（P0：放行 honcho_search_conclusions + AGENTS.md 记忆回忆回合纪律））。

===========================================================================
锚换代注记（2026-10-24 · 261024-memory-recall-tool-enable）
===========================================================================

本文件原冻结契约 = K1-E4「allow 恰四项 {read, write, exec, web_search}」
（261022-m5-k1-prompt-carrier-migration 于 2026-10-23 冻结）。

换代事由（architect advisory §0/§B 选项 3 实证）：历史事实（9/30「王一诺」）已
在 owner 自我视角可检索（backfill 344 条结论，实测第一条即原句），但 kidpal agent
的 tools.allow 四项白名单把插件 honcho_* 工具全部滤掉——日志实证「No callable
tools remain after resolving explicit tool allowlist (agents.kidpal.tools.allow:
…)」；模型按现行指令只能答「记不清了」，行为完全符合旧指令。这不是模型缺陷，
是三重锁里最硬的一层（工具白名单）。

换代内容（**显式换代，不是静默放宽**；旧断言逐条被替换，见本节末对照）：

  · K1-E4（声明面）：allow 四项 → **恰五项**
    {read, write, exec, web_search, honcho_search_conclusions}，无重复。
    honcho_search_messages 仍**不放行**（原始消息面含家长代述 / 流程内容，
    advisory §B 选项 3 明确；TaskPackage notes ⑤）。
  · K1-E4（drift_guard）：现状锚从「工具键 unset」扩为「**升级基准**」——须同时
    接受 ① 新机态（键缺席）与 ② live 既有态（上一代四项覆写已应用）；目标态
    （五项）由 apply 的幂等分支承载（幂等 SKIP 先于 drift-guard）。旧声明在 live
    （四项覆写态）上会 exit 3，升级根本落不了地——这是扩基准的直接事由
    （TaskPackage notes ②）。
  · K1-E4（apply）：增 `--gen-patch <out>` 形态（261023 批 B
    `apply_honcho_plugin.py` 先例：patch 生成器产出 `openclaw config patch`
    消费文件，配置只经 OpenClaw 自己的写入口落盘）；**直写形态行为零回退**
    （既有幂等 / drift / 备份 / 只改一键四态断言全部同步换代保留）。
  · K1-E4-B（新条目）：AGENTS.md 回合路由表新增「记忆回忆回合」行（additive；
    既有五行零改动）+ 记忆节保留原句「你不需要写任何文件、也不调用任何工具去记」
    （工具不用于「记」仍真）+ 一句检索澄清；既有冻结锚兼容在本件内以自含副本
    新断言承载（K1-E1 回合路由五行 / K3_P1_AGENTS_ANCHORS / 261023 A3 记忆节四锚
    + 对象锁定行逐字）。

旧 → 新断言对照（显式留痕，禁静默放宽）：
  · test_k1_e4_allowlist_exactly_four → test_k1_e4_allowlist_exactly_five
    （项数 4→5、期望集加 honcho_search_conclusions、新增 honcho_search_messages
      负锚）
  · test_k1_e4_declaration_present_and_schema（保留；drift_guard 结构扩
    expect | expect_any）
  · 【新增】test_k1_e4_drift_guard_accepts_unset_and_legacy_four（升级基准）
  · test_k1_e4_apply_drift_guard_exit3（保留；用例集扩「六项态」真漂移）
  · 【新增】test_k1_e4_apply_accepts_legacy_four_baseline（live 现状可升级）
  · 【新增】test_k1_e4_gen_patch_product_exactly_target_key_and_zero_config_write
  · 【新增】K1-E4-B 三条（回合行语义 / 记忆节原句 + 检索澄清 / 既有锚兼容）
  · K1-E4 其余（apply 真跑四态 / 备份 / 只改一键 / 回滚 / 零入仓 / manifest）
    零语义回退，仅随五项目标态同步期望值。

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O；仿 server/acceptance/test_261017_openclaw_kidpal_skills.acceptance.py
头注 FROZEN OFFLINE CONTRACT 同形制）

落位目录 = server/deploy/261022-openclaw-kidpal-tools/

  1) kidpal-agent-tools-declaration.json —— 声明工件（**JSON**，json.loads 解析，
     零额外依赖）。必需顶层键：case / change / drift_guard / backup / rollback。
       · case : 非空字符串（本包只有一种收窄策略，取值不作枚举约束）。
       · change : {"path": "agents.entries.kidpal.tools.allow",
                   "allow": ["read", "write", "exec", "web_search",
                             "honcho_search_conclusions"]}
           - path 必须恰为 "agents.entries.kidpal.tools.allow"（apply / rollback /
             drift_guard 定位键家族中 apply 的写点）；
           - allow 必须**恰五项**、无重复、集合 = {read, write, exec, web_search,
             honcho_search_conclusions}（顺序不作语义）；
           - honcho_search_messages 不得出现（原始消息面负锚）。
       · drift_guard : 非空 list，每项 object 须有非空点分 path，且**恰含
         expect 或 expect_any 之一**：
           - expect : 字符串；"unset" 判键缺席，否则判值相等（既有语义保留）。
           - expect_any : 非空 list；任一元素命中即通过。元素 "unset" 判键缺席，
             其余（object / list / 标量）判值相等。
         "unset" 的语义 = **键缺席**；键在场但形状不对（如 tools 为字符串）=
         真漂移（fail-closed，不得按 unset 放行后静默覆盖畸形配置）。
         必须存在一条锚定**基线键**的条目，同时接受两种基线态：
           ① 新机态 = 键缺席（"unset"）；
           ② live 既有态 = kidpal agent 的上一代四项覆写已应用
              （tools 对象形态 {"allow": ["read","write","exec","web_search"]}，
              或 tools.allow 路径 + 四项列表形态——两形皆可，语义一致）。
         **目标态（五项）不进基线集**——由 apply ①幂等分支承载（幂等 SKIP 先于
         drift-guard；见 §2）。真漂移（六项 / 异值 / 缺键异值）必须仍被判 drift。
       · backup : {"dir": "<ECS 备份目录>", "pattern": "<含 'bak' 的命名模式>"}；
         pattern 是 config 同目录下备份文件的 glob（<config basename>.bak-...-*）。
       · rollback : {"script": "rollback_agent_tools.py"}（= 本目录下回滚脚本名）。

  2) apply_agent_tools.py —— **双形态**（261023 批 B apply_honcho_plugin.py 先例；
     全离线 `sys.executable <script> …` 可真跑）。
     形态① 直写应用（既有语义保留，语义序 = 261017 PV-A6 同形制）：
       ① 幂等优先：config 现状已等于声明目标态（五项）→ 打印 SKIP 并 exit 0
          （不写、不再备份）；目标态命中即 SKIP，**不经 drift-guard**；
       ② drift-guard：逐条核对 drift_guard（expect / expect_any 语义见 §1），任一
          条目不中 → 打印 STOP 信息并 **exit 3**（不写、不备份）；
       ③ 备份：写前建时间戳备份，**落 config 同目录**，文件名 = config basename +
          ".bak-..." + UTC `yyyymmddTHHMMSS`（内容 = 原始 config；须匹配声明
          backup.pattern 的 glob）；
       ④ 应用：把 change.allow 写到 change.path；**只改该键**，其余键（含
          gateway.token、agents.entries 下其它 agent、261017 skills 白名单键）
          原样保留。成功 exit 0。
     形态② patch 生成器：`--gen-patch <out>`（配合 --decl）从声明生成
       `openclaw config patch` 消费的 patch 文件——JSON 子集形态（stdlib json 可
       解析；JSON5 超集允许但不得破坏 stdlib 解析）；展平叶键集**恰 = 一个叶键**
       {change.path: change.allow}（嵌套树或扁平点分映射两形皆可，展平语义
       一致）；**不读 config、不写 config、零外部命令、零备份**；成功 exit 0。
     CLI/参数：--config（默认 /root/.openclaw/openclaw.json）、--decl（默认同目录
     kidpal-agent-tools-declaration.json）、--gen-patch <out>。
     用 stdlib json（**不依赖 jq**）。
     退出码：直写形态缺 config / 两形态缺声明 / 非法 JSON → exit 2
     （--gen-patch 不读 config，故 config 缺失不构成错误）。

  3) rollback_agent_tools.py —— 一步回滚（Python，语义零换代）。CLI：--config、
     --backup（默认取 config 旁最新声明 backup.pattern 匹配件）、--dry-run（只
     打印意图、exit 0、不写）。非 dry-run：把 backup 内容还原到 config，exit 0。

  4) openclaw.json 整文件不入仓（含 gateway.token）；声明工件内不得出现
     gateway.token 字样、不得出现 token 键。

  5) manifest.yaml 登记：261022-openclaw-kidpal-tools 声明 / apply / rollback
     三件至少各一条（dst = /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/
     下），部署单一事实源（261013 纪律）。

  6) AGENTS.md（server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md）——
     「回合类型与纪律路由」**表内**新增「记忆回忆回合」行（additive：既有五行
     零改动，行序不作约束）：
       · 识别特征：孩子问「记不记得 / 我记得/我以前说过 / XX 是谁」类旧事回忆
         问题；
       · 纪律（一条行内须齐）：
           ① 先调一次 `honcho_search_conclusions`（唯一放行的检索工具；
              `honcho_search_messages` 不得出现在本行 = 不给原始消息面指路）；
           ② 查到 → 自然转述（口语化、**不逐字复述**检索原文、**不带内部流程**、
              **家长隐私不回忆**）；
           ③ 查不到 → 才说「记不清了」；
           ④ 过渡语播种（「我去翻翻我记得的事」类，不静默干等）；
           ⑤ 与「孩子对话回合禁工具」的关系**显式写清**：记忆回忆回合 =
              课表问答之外第二个被允许用工具的对话回合（或以「不属于孩子对话
              回合 / 按记忆回忆回合处理」等价表述消歧）。
       · 记忆节（`## …记忆…` 节）：
           - 原句「你不需要写任何文件、也不调用任何工具去记」的关键半句
             「不调用任何工具去记」**逐字保留**（工具不用于「记」仍真；261023
             A3「不调用任何工具」正锚不得被删改）；
           - 「不需要写任何文件」保留；
           - A3 记忆节正锚四串继续在场：「不需要写任何文件」「不调用任何工具」
             「记不清」「绝不编造」；
           - **新增一句检索澄清**（记忆节内须出现检索口径字面：如「检索」/
             「查一下记忆」/「我去翻一翻」/「honcho_search_conclusions」），把
             「记」与「查旧事」分开——记仍自动、旧事回忆走记忆回忆回合。
       · 既有锚兼容红线（本件以自含副本断言，跨件跑绿二次确认）：K1-E1 回合
         路由五行全锚（孩子对话回合 / 实时事实回合 / 查字回合 / 课表问答回合 /
         提醒 cron 回合，且孩子对话回合行仍含禁工具锚组）；K3_P1_AGENTS_ANCHORS
         逐字；261023 A3 对象锁定行逐字（含 K3 隐私锚串 + S3 澄清两半）。

===========================================================================
强度申报（契约条目 → 承载断言 → 性质档）：

  · 声明在位 / schema / allow 恰五项      → test_k1_e4_declaration_present_and_schema /
                                               test_k1_e4_allowlist_exactly_five → 机制级
  · drift_guard 升级基准（unset + live
    四项覆写双接受）                        → test_k1_e4_drift_guard_accepts_unset_and_legacy_four → 机制级
  · apply 真跑：unset 基线 → 五项目标态    → test_k1_e4_apply_applies_then_idempotent → 行为级（真跑）
  · apply 真跑：live 四项基线 → 可升级     → test_k1_e4_apply_accepts_legacy_four_baseline → 行为级（真跑）
  · 幂等（二次零语义变更、零新备份）        → test_k1_e4_apply_applies_then_idempotent → 行为级（真跑）
  · 真 drift（异值 / 六项）拒绝（exit 3
    零写零备份）                            → test_k1_e4_apply_drift_guard_exit3 → 行为级（真跑）
  · 写前时间戳备份（内容 = 原始 config）    → test_k1_e4_apply_timestamped_backup → 行为级（真跑）
  · 只改 tools.allow 键、261017 skills
    组合态与 gateway.token 零触碰           → test_k1_e4_apply_preserves_other_keys_and_composes_with_skills_patch → 行为级（真跑）
  · gen-patch 产物（恰一键 / 五项 / 零
    config 读写）                            → test_k1_e4_gen_patch_product_exactly_target_key_and_zero_config_write → 行为级（真跑）
  · 回滚 dry-run / 一步还原                → test_k1_e4_rollback_dry_run_and_restore → 行为级（真跑）
  · openclaw.json 零入仓 / 声明零 secret   → test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration → 机制级
  · manifest 三件登记                      → test_k1_e4_manifest_registers_the_patch_workbench → 机制级
  · K1-E4-B 回合行语义（查一次 → 自然转述
    → 查不到才记不清 / 过渡语 / 关系消歧）  → test_k1_e4_b_memory_recall_round_row_semantics → 源锚代理（prompt 措辞 = 交付物本身）
  · K1-E4-B 记忆节原句 + 检索澄清          → test_k1_e4_b_memory_section_keeps_no_tool_for_recording_sentence → 源锚代理
  · K1-E4-B 既有锚兼容（K1-E1 五行 /
    K3 锚串 / 261023 A3 四锚 + 对象锁定）  → test_k1_e4_b_anchor_compat_no_regression → 源锚代理

缺口（承载断言强度低于契约要求处，逐条）：

  · offline 只钉「脚本对临时 fixture 的行为」与「制品静态形状」；ECS 上真实
    openclaw.json 的 apply / 回滚 / `openclaw` 审计可见工具 = 5 项（含
    honcho_search_conclusions）、57KB schema 消失前四项基线 = live 观测项
    （human 部署轮，本 Pod 不自行上 ECS），offline 不证 live 生效。
  · gen-patch 产物「可被 `openclaw config patch --dry-run` 接受」= live 面
    （OpenClaw 自有 schema 校验），offline 只证 JSON 子集形状与叶键集；真机
    写入口行为归部署轮命令单（--dry-run 校验后 --file 落）。
  · **LIVE 面零 host 断言（显式申报）**：openclaw.json 实应用（命令单）+
    真机问「王一诺是谁」答出 9/30 事实 + 日志出现 honcho_search_conclusions
    工具调用且无「记不清」自述——三项均归 human 部署轮，本文件不落自动断言
    （marker 不设 live 用例；host 面转绿后停门 awaiting-human:live-deploy）。
  · drift_guard「live 既有态 = 四项覆写」的**真实性**由部署轮实读承载；offline
    只证声明接受该基线形态、apply 在该 fixture 上可升级（261017 PV-A6 同款）。
  · AGENTS.md 是**提示词措辞**交付物：断言钉「语义锚组在场 / 负锚不出现」，
    不断言句式；模型实际是否照回合路由执行（真机调工具）属 live 观测项。
  · 行为等价假设（显式化，不写即默认不成立）：
      - change.path = "agents.entries.kidpal.tools.allow" ⇔ OpenClaw 运行时按
        agent 级 tools.allow 列表收窄可见工具（**含插件工具**——advisory §B
        选项 3 日志实证 OQ-3：allow 管得住插件工具；运行时键形以部署轮工具
        列表观测为准；若实跑发现键形不符，属验收判法升级，走 tighten-only
        窗口经 planner 调整，不得静默改结构凑合）；
      - drift_guard 的 tools 对象形态比对 = **值相等**（live 的 tools 对象若
        出现 allow 之外的多余键，apply 会 fail-closed STOP 而非静默覆盖——这是
        设计意图，不是缺陷；真出现应经 planner 复核声明基线）；
      - 「检索式召回生效」以声明 + 提示词 + live 工具调用的组合为代理——
        OpenClaw 运行时是否真注入并执行该工具归部署轮。
  · fixture 自含性声明：全部断言直读 repo 内制品 + tempfile fixture，无外部
    资产 / 时间真值 / 环境前提依赖（PyYAML 仅 manifest 断言用，与
    verify-deploy.sh 同一解析器）。行为 seam（声明 / 提示词 → OpenClaw 运行时
    工具注入与回合行为）的驱动方 = human 部署轮，offline 侧无驱动方 = 已申报盲区。

Run protocol（文件名两点须 importlib 模式）:
  python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/
  （standalone: python 本文件）

RED 基线（冻结时点树，2026-10-24，builder 尚未动工；实跑取证 = 全量
`-m "not live"` acceptance：**346 收集 / 7 failed / 339 passed / 7 deselected /
33 subtests**；同树冻结前基线 = 340 passed / 7 deselected / 33 subtests）：

  设计态红 7 条（全部指向当前真实缺失，非环境/测试自身错误）：
    ① test_k1_e4_allowlist_exactly_five —— 声明 allow 仍四项；
    ② test_k1_e4_drift_guard_accepts_unset_and_legacy_four —— drift_guard 只有
       expect unset，无「升级基准」（live 四项覆写会被自己拦死）；
    ③ test_k1_e4_apply_script_present_and_consumes_declaration —— 无 --gen-patch；
    ④ test_k1_e4_apply_accepts_legacy_four_baseline —— 四项基线上升级 exit 3；
    ⑤ test_k1_e4_gen_patch_product_exactly_target_key_and_zero_config_write ——
       生成器形态不存在；
    ⑥ test_k1_e4_b_memory_recall_round_row_semantics —— 无「记忆回忆回合」行；
    ⑦ test_k1_e4_b_memory_section_keeps_no_tool_for_recording_sentence —— 记忆节
       无检索澄清句（原句与 A3 四锚已绿：本断言只等「加法」那半）。
  零回退：既有非本件断言 339 全绿（K1-E4 的 apply 四态 / 备份 / 只改一键 /
  回滚 / 零入仓 / manifest 与 K1/K3/261023 等兄弟冻结件均绿——apply 四态随声明
  驱动，声明改五后自动跟随，不构成回退面）。
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

try:
    import yaml  # noqa: F401  (lazy import in helper; 环境前提缺失 = infra 红)
    _HAS_YAML = True
except ImportError:
    _HAS_YAML = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY_DIR = SERVER / "deploy"
TOOLS_DIR = DEPLOY_DIR / "261022-openclaw-kidpal-tools"
DECL_PATH = TOOLS_DIR / "kidpal-agent-tools-declaration.json"
APPLY_SCRIPT = TOOLS_DIR / "apply_agent_tools.py"
ROLLBACK_SCRIPT = TOOLS_DIR / "rollback_agent_tools.py"
MANIFEST_YAML = DEPLOY_DIR / "manifest.yaml"
AGENTS_MD = (DEPLOY_DIR / "openclaw-agents" / "workspace-kidpal" / "AGENTS.md")

TARGET_PATH = "agents.entries.kidpal.tools.allow"
DRIFT_ANCHOR_PATH = "agents.entries.kidpal.tools"
# 2026-10-24 换代：四项 → 恰五项（+honcho_search_conclusions）。
EXPECTED_TOOLS = ("read", "write", "exec", "web_search",
                  "honcho_search_conclusions")
# 上一代已应用态（live 现状 = drift_guard 必须接受的「升级基准」之一）。
LEGACY_TOOLS_FOUR = ("read", "write", "exec", "web_search")
# 安全负锚：原始消息面工具明确不放行（advisory §B 选项 3；TaskPackage notes ⑤）。
FORBIDDEN_TOOL = "honcho_search_messages"
MANIFEST_DIR_TOKEN = "261022-openclaw-kidpal-tools"
TIMESTAMP_RE = re.compile(r"\d{8}T\d{6}")

_MISSING = object()


# --------------------------------------------------------------------------- #
# 读盘 / 进程 helpers（仓根由 __file__ 显式推导，不依赖调用点 cwd）
# --------------------------------------------------------------------------- #

def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _load_json(path: Path):
    txt = _read_text(path)
    if not txt:
        return None, f"{path} 缺失或为空"
    try:
        return json.loads(txt), None
    except ValueError as e:  # noqa: BLE001
        return None, f"{path} 非法 JSON: {e!r}"


def _write_json(path: Path, obj) -> None:
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2), encoding="utf-8")


def _run_py(script: Path, args, timeout: int = 120):
    """用当前解释器跑 python 脚本（离线可跑，不经 bash/ssh）；脚本缺失 → rc 127。"""
    if not script.is_file():
        return 127, "", f"{script} 不在位（冻结契约工件缺失）"
    proc = subprocess.run(
        [sys.executable, str(script), *[str(a) for a in args]],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
        timeout=timeout,
    )
    return proc.returncode, proc.stdout, proc.stderr


# --------------------------------------------------------------------------- #
# 点分路径读写（openclaw.json 形状）
# --------------------------------------------------------------------------- #

def _json_get(obj, dotted: str):
    cur = obj
    for part in dotted.split("."):
        if not isinstance(cur, dict) or part not in cur:
            return _MISSING
        cur = cur[part]
    return cur


def _json_set(obj: dict, dotted: str, value) -> None:
    parts = dotted.split(".")
    cur = obj
    for p in parts[:-1]:
        nxt = cur.get(p)
        if not isinstance(nxt, dict):
            nxt = {}
            cur[p] = nxt
        cur = nxt
    cur[parts[-1]] = value


def _flatten_leaves(obj, prefix: str = ""):
    """把 patch 产物（嵌套树或扁平点分映射两形皆可）展平为
    (点分叶路径, 标量值) 序列——展平语义一致，故两种形态可同一判法。"""
    if isinstance(obj, dict):
        for k, v in obj.items():
            yield from _flatten_leaves(v, f"{prefix}.{k}" if prefix else str(k))
    else:
        yield prefix, obj


# --------------------------------------------------------------------------- #
# fixture（组合态：261017 skills 白名单已应用 + kidpal 无 tools 覆写 = 新机基线）
# --------------------------------------------------------------------------- #

def _fixture_pre() -> dict:
    """变更前现状（新机基线）：kidpal agent 无 tools 覆写（漂移基准 = 键缺席）；
    skills 白名单 = 261017 已应用组合态（plan.md：本包先落、P3 drift 锚以本包
    已应用态为准——组合共存是声明式 patch 的设计前提，非顺带）。"""
    return {
        "gateway": {"token": "FIXTURE-GATEWAY-TOKEN-0123456789"},
        "tools": {"profile": "coding"},
        "agents": {
            "defaults": {"model": {"primary": "openclaw/kidpal"}},
            "entries": {
                "main": {"skills": ["kidpal-schedule-write"]},
                "kidpal": {
                    "prompt": "kidpal/base",
                    "skills": ["kidpal-schedule-answer", "weather", "tavily"],
                },
            },
        },
        "modelPolicy": {"allow": ["openclaw/kidpal"]},
    }


def _fixture_legacy_four() -> dict:
    """live 既有态（升级基准②）：上一代四项覆写已应用——本包必须能在此态上升级。"""
    state = _fixture_pre()
    _json_set(state, TARGET_PATH, list(LEGACY_TOOLS_FOUR))
    return state


def _fixture_mismatch() -> dict:
    """真漂移①：tools.allow 已被设成别的白名单（异值）。"""
    state = _fixture_pre()
    _json_set(state, TARGET_PATH, ["bash", "browser"])
    return state


def _fixture_six_items() -> dict:
    """真漂移②：tools.allow 已被扩到六项（含原始消息面工具）——基线集以外的
    状态不得被当作可升级态（fail-closed）。"""
    state = _fixture_pre()
    _json_set(state, TARGET_PATH, list(LEGACY_TOOLS_FOUR) + list(EXPECTED_TOOLS[4:])
              + [FORBIDDEN_TOOL])
    return state


def _fixture_malformed_tools() -> dict:
    """真漂移③：tools 键在场但不是对象（结构畸形）。键「在场且形状不对」≠键
    缺席——按 unset 放行等于静默覆盖畸形配置，须 fail-closed 判漂移。"""
    state = _fixture_pre()
    _json_set(state, DRIFT_ANCHOR_PATH, "disabled")
    return state


def _target_state(pre: dict, allow) -> dict:
    state = json.loads(json.dumps(pre))
    _json_set(state, TARGET_PATH, list(allow))
    return state


def _backups(td: Path, pattern: str) -> list[Path]:
    return sorted(td.glob(pattern))


# --------------------------------------------------------------------------- #
# drift_guard 基线判法（expect | expect_any；两形 path 皆可）
# --------------------------------------------------------------------------- #

def _drift_alternatives(decl: dict):
    """产出 (path, alternative) 序列：expect 单项 + expect_any 展开。

    基线键两形皆可：tools 对象形态（path = agents.entries.kidpal.tools）或
    allow 列表形态（path = agents.entries.kidpal.tools.allow）。"""
    out = []
    dg = decl.get("drift_guard")
    if not isinstance(dg, list):
        return out
    for entry in dg:
        if not isinstance(entry, dict):
            continue
        path = entry.get("path")
        if not isinstance(path, str) or not path.strip():
            continue
        if "expect" in entry:
            out.append((path, entry.get("expect")))
        ea = entry.get("expect_any")
        if isinstance(ea, list):
            for alt in ea:
                out.append((path, alt))
    return out


def _is_unset_alt(path: str, alt) -> bool:
    return alt == "unset" and path in (DRIFT_ANCHOR_PATH, TARGET_PATH)


def _is_legacy_four_alt(path: str, alt) -> bool:
    if path == DRIFT_ANCHOR_PATH and isinstance(alt, dict):
        allow = alt.get("allow")
        return (isinstance(allow, list) and len(allow) == len(LEGACY_TOOLS_FOUR)
                and sorted(allow) == sorted(LEGACY_TOOLS_FOUR))
    if path == TARGET_PATH and isinstance(alt, list):
        return (len(alt) == len(LEGACY_TOOLS_FOUR)
                and sorted(alt) == sorted(LEGACY_TOOLS_FOUR))
    return False


# --------------------------------------------------------------------------- #
# AGENTS.md 锚组（K1-E4-B；自含副本，跨冻结件 import 会把冻结件焊死）
# --------------------------------------------------------------------------- #

ROUTING_HEADING_NEEDLE = "回合类型与纪律路由"
MEMORY_ROUND_NAME = "记忆回忆回合"
MEMORY_SECTION_NEEDLE = "记忆"

# 「记忆回忆回合」行须齐的语义锚组（每组至少一词；措辞换写可、语义删除不可）。
MEMORY_ROUND_ANCHORS = {
    "唯一放行工具字面 honcho_search_conclusions": (
        "honcho_search_conclusions",),
    "识别特征（记不记得 / 我以前说过 / 是谁）": (
        "记不记得", "还记得", "我以前说过", "以前说过", "说过的事", "是谁"),
    "先查一次": (
        "先调", "先查", "查一次", "调用一次", "先检索", "检索一次", "先搜"),
    "查到后自然转述": (
        "自然", "转述", "用自己的话说", "像聊天", "口语"),
    "不逐字复述": (
        "不逐字", "不要逐字", "不得逐字", "不复述", "不照搬", "不原样"),
    "不带内部流程": (
        "内部流程", "不带流程", "不暴露", "不叙述", "不提工具", "不说工具"),
    "家长隐私不回忆": ("家长隐私",),
    "查不到才说记不清": ("记不清",),
    "过渡语播种": (
        "我去翻", "翻翻", "过渡语", "先播", "先说一句"),
    "与孩子对话回合禁工具的关系显式写清": (
        "第二个", "第二处", "第二类", "唯二", "除课表问答",
        "不属于孩子对话回合", "不走孩子对话回合", "不按孩子对话回合"),
}

# K1-E1 回合路由五行（additive 红线：新行不得挤掉既有行；孩子对话回合行
# 禁工具锚组零回退）。
K1E1_ROUND_NAMES = (
    "孩子对话回合", "实时事实回合", "查字回合", "课表问答回合", "提醒 cron 回合")
K1E1_CHILD_ROUND_TOOL_BAN = (
    "不调用任何工具", "禁工具", "禁用工具", "不读取任何文件", "禁读文件")

# K3_P1_AGENTS_ANCHORS 逐字副本（源 = server/acceptance/test_261022_honcho_memory
# .acceptance.py:233-240 的六串；复制不引用，理由同上）。
K3_P1_AGENTS_ANCHORS_COMPAT = (
    "## 静态语义单一事实源",
    "以本文件为准",
    "## 底线（超越一切，任何时候都有效）",
    "查字回合绝对禁工具",
    "回合类型与纪律路由",
    "不引用、不复述任何家长隐私（姓名、联系方式、家庭安排细节）",
)

# 261023 A3 记忆节正锚四串（逐字副本）。
A3_MEMORY_POSITIVES_COMPAT = (
    "不需要写任何文件", "不调用任何工具", "记不清", "绝不编造")
A3_OBJECT_LOCK_KEY = "对象锁定"
# 261023 A3 对象锁定行 S3 澄清两半（逐字副本）。
A3_S3_CLARIFIES = ("孩子自己的日程信息", "不算复述家长隐私")

# 记忆节检索澄清句（把「记」与「查旧事」分开）。
MEMORY_RETRIEVAL_CLARIFY = (
    "检索", "查一下记忆", "翻一翻", "翻我", "honcho_search_conclusions")


def _section_span(text: str, heading_needle: str):
    """取「标题含 heading_needle 的 `## ` 节」窗口：自该标题行起、至下一个
    `## ` 标题行或 EOF。节不存在 → None（调用方判设计态红）。"""
    lines = text.splitlines()
    start = next((i for i, l in enumerate(lines)
                  if l.startswith("## ") and heading_needle in l), None)
    if start is None:
        return None
    rest = lines[start + 1:]
    end = next((j for j, l in enumerate(rest) if l.startswith("## ")), len(rest))
    return "\n".join([lines[start]] + rest[:end])


class OpenclawKidpalToolsAcceptance(unittest.TestCase):
    """261022-m5-k1-prompt-carrier-migration K1-E4（offline，五态验收；2026-10-24
    换代至五项 + drift 升级基准 + gen-patch 形态）。

    live-manual 观测项（**非自动断言**，structural 申报，本类不落测试）：
      human 部署轮在 ECS 核验——`openclaw` 工具面 audit 可见工具 = read / write /
      exec / web_search / honcho_search_conclusions 恰五项、57KB schema 消失
      （K1-E7 的一部分）；`openclaw config patch --dry-run` 接受 gen-patch 产物后
      `--file` 落盘；本包 apply 在真实 openclaw.json（四项覆写态）上的升级回执
      与 261017 skills patch 组合共存；真机问「王一诺是谁」答出 9/30 事实、日志
      出现 honcho_search_conclusions 工具调用。非确定性来源：运行时键形、工具
      注入与回合行为都是部署后观测量，offline 不可机判（行为等价假设见头注缺口
      节）。host 面转绿后停门等 human（awaiting-human:live-deploy），Pod 不自行
      上 ECS。"""

    maxDiff = None

    def _decl(self) -> dict:
        self.assertTrue(
            DECL_PATH.is_file(),
            f"K1-E4 RED: 声明工件不在位 —— 冻结契约要求 {DECL_PATH}。"
            "openclaw.json 含 gateway.token 整文件不入仓，只入声明（K1-E4 铁律）。")
        decl, err = _load_json(DECL_PATH)
        self.assertIsNotNone(decl, f"K1-E4 RED: 声明工件不可解析 —— {err}。")
        self.assertIsInstance(decl, dict, "K1-E4 RED: 声明工件顶层必须是 JSON object。")
        return decl

    def _backup_pattern(self, decl: dict) -> str:
        pattern = (decl.get("backup") or {}).get("pattern") if isinstance(
            decl.get("backup"), dict) else None
        self.assertTrue(
            isinstance(pattern, str) and pattern.strip(),
            "K1-E4 RED: 声明缺 backup.pattern（前置）。")
        self.assertNotIn(
            "/", pattern,
            f"K1-E4 RED: backup.pattern = {pattern!r} 含路径分隔符 —— 须为 config "
            "同目录下备份文件的相对 glob（<config basename>.bak-...-*）。")
        return pattern

    def _declared_allow(self, decl: dict):
        allow = (decl.get("change") or {}).get("allow")
        self.assertIsInstance(
            allow, list, "K1-E4 RED: change.allow 不是 list（目标白名单）。")
        return allow

    # ------------------------------------------------------------------ #
    # K1-E4 · 声明工件在位 / schema / allow 恰五项（机制级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_declaration_present_and_schema(self):
        """coverage: K1-E4 — 声明工件在位 + JSON 可解析 + 五顶层键齐 + 各段结构
        合法 + drift_guard 每项 path 齐且恰含 expect 或 expect_any。"""
        decl = self._decl()
        for key in ("case", "change", "drift_guard", "backup", "rollback"):
            self.assertIn(
                key, decl,
                f"K1-E4 RED: 声明工件缺必需顶层键 {key!r} —— 冻结契约结构 = "
                "case / change / drift_guard / backup / rollback（仿 261017 PV-A6）。")
        self.assertIsInstance(decl.get("case"), str, "K1-E4 RED: case 必须是字符串。")
        self.assertTrue(str(decl.get("case", "")).strip(), "K1-E4 RED: case 不得为空。")

        change = decl.get("change")
        self.assertIsInstance(change, dict, "K1-E4 RED: change 必须是 object。")
        self.assertEqual(
            change.get("path"), TARGET_PATH,
            f"K1-E4 RED: change.path = {change.get('path')!r}，须恰为 "
            f"{TARGET_PATH!r}（apply 写点）。")

        dg = decl.get("drift_guard")
        self.assertIsInstance(dg, list, "K1-E4 RED: drift_guard 必须是 list。")
        self.assertTrue(dg, "K1-E4 RED: drift_guard 不得为空。")
        for i, entry in enumerate(dg):
            self.assertIsInstance(entry, dict,
                                  f"K1-E4 RED: drift_guard[{i}] 必须是 object。")
            self.assertTrue(
                isinstance(entry.get("path"), str) and entry.get("path", "").strip(),
                f"K1-E4 RED: drift_guard[{i}].path 必须是非空字符串。")
            has_expect = "expect" in entry
            has_any = "expect_any" in entry
            self.assertTrue(
                has_expect or has_any,
                f"K1-E4 RED: drift_guard[{i}] 既无 expect 也无 expect_any —— "
                "基线判定面缺失（换代契约：expect | expect_any 恰含其一）。")
            self.assertFalse(
                has_expect and has_any,
                f"K1-E4 RED: drift_guard[{i}] 同时含 expect 与 expect_any —— "
                "两形互斥，语义不明（换代契约：恰含其一）。")
            if has_expect:
                self.assertTrue(
                    isinstance(entry.get("expect"), str)
                    and entry.get("expect", "").strip(),
                    f"K1-E4 RED: drift_guard[{i}].expect 必须是非空字符串。")
            if has_any:
                ea = entry.get("expect_any")
                self.assertIsInstance(
                    ea, list,
                    f"K1-E4 RED: drift_guard[{i}].expect_any 必须是 list。")
                self.assertTrue(
                    ea,
                    f"K1-E4 RED: drift_guard[{i}].expect_any 不得为空（基线集）。")

        backup = decl.get("backup")
        self.assertIsInstance(backup, dict, "K1-E4 RED: backup 必须是 object。")
        self.assertTrue(
            isinstance(backup.get("dir"), str) and backup.get("dir", "").strip(),
            "K1-E4 RED: backup.dir 必须是非空字符串（ECS 备份目录）。")
        self.assertTrue(
            isinstance(backup.get("pattern"), str) and "bak" in backup.get("pattern", ""),
            "K1-E4 RED: backup.pattern 必须是含 'bak' 的字符串（config 同目录 glob）。")

        rollback = decl.get("rollback")
        self.assertIsInstance(rollback, dict, "K1-E4 RED: rollback 必须是 object。")
        script = rollback.get("script")
        self.assertTrue(
            isinstance(script, str) and script.strip(),
            "K1-E4 RED: rollback.script 必须是非空字符串。")
        self.assertTrue(
            (TOOLS_DIR / script).is_file(),
            f"K1-E4 RED: rollback.script 指向的 {script!r} 不在 {TOOLS_DIR} —— "
            "声明面与回滚面不一致（回滚不可跑）。")

    def test_k1_e4_allowlist_exactly_five(self):
        """coverage: K1-E4 — 工具白名单恰五项、无重复、集合 = read / write / exec /
        web_search / honcho_search_conclusions（顺序不作语义）；原始消息面工具
        honcho_search_messages 不得放行（安全负锚）。"""
        decl = self._decl()
        allow = self._declared_allow(decl)
        self.assertEqual(
            len(allow), 5,
            f"K1-E4 RED: 白名单项数 = {len(allow)}（{allow}），须恰五项 —— "
            "2026-10-24 换代：四项 + honcho_search_conclusions（历史事实检索面；"
            "architect advisory §B 选项 3）。")
        self.assertEqual(
            len(set(allow)), 5,
            f"K1-E4 RED: 白名单有重复项（{allow}）—— 重复项不构成额外能力。")
        self.assertEqual(
            sorted(allow), sorted(EXPECTED_TOOLS),
            f"K1-E4 RED: 白名单 {sorted(allow)} ≠ 契约五项 {sorted(EXPECTED_TOOLS)} "
            "—— 噪音未收敛干净、漏放检索工具或误收多余能力。")
        self.assertNotIn(
            FORBIDDEN_TOOL, allow,
            f"K1-E4 RED: 白名单含 {FORBIDDEN_TOOL!r} —— 原始消息面（家长代述 / "
            "流程内容）明确不放行（advisory §B 选项 3；TaskPackage notes ⑤）。")

    def test_k1_e4_drift_guard_accepts_unset_and_legacy_four(self):
        """coverage: K1-E4 — drift_guard 升级基准：基线键须同时接受 ① 新机态
        （键缺席）与 ② live 既有态（上一代四项覆写）；目标态五项由幂等分支
        承载、不进基线集。tools 对象形态与 tools.allow 列表形态皆可。"""
        decl = self._decl()
        alts = _drift_alternatives(decl)
        self.assertTrue(
            alts,
            "K1-E4 RED: drift_guard 未产出任何可判基线（expect / expect_any 均缺）"
            "—— apply 无法判漂移。")
        self.assertTrue(
            any(_is_unset_alt(p, a) for p, a in alts),
            f"K1-E4 RED: drift_guard 不接受新机态（键缺席 'unset'，基线键 = "
            f"{DRIFT_ANCHOR_PATH} 或 {TARGET_PATH}）—— 新机上 apply 会被自己的 "
            "drift-guard 拦死（exit 3）。实得基线集："
            f"{[(p, a) for p, a in alts]}。")
        self.assertTrue(
            any(_is_legacy_four_alt(p, a) for p, a in alts),
            "K1-E4 RED: drift_guard 不接受 live 既有态（上一代四项覆写 "
            f"{list(LEGACY_TOOLS_FOUR)}）—— 这就是本包升级落不了地的直接原因"
            "（TaskPackage notes ②：现声明 expect unset，live 已四项覆写，apply "
            "STOP exit 3）。须扩「升级基准」：例如 "
            '{"path": "agents.entries.kidpal.tools", "expect_any": ["unset", '
            '{"allow": ["read","write","exec","web_search"]}]}。实得基线集：'
            f"{[(p, a) for p, a in alts]}。")

    def test_k1_e4_apply_script_present_and_consumes_declaration(self):
        """coverage: K1-E4 — apply 脚本在位、消费声明工件名、用 stdlib json
        （不依赖 jq）、暴露 --gen-patch 形态（换代新增）。"""
        self.assertTrue(
            APPLY_SCRIPT.is_file(),
            f"K1-E4 RED: apply 脚本不在位 —— 冻结契约要求 {APPLY_SCRIPT}。")
        src = _read_text(APPLY_SCRIPT)
        self.assertIn(
            "kidpal-agent-tools-declaration.json", src,
            "K1-E4 RED: apply 脚本未引用声明工件名 —— 声明面与消费面不一致。")
        self.assertIsNotNone(
            re.search(r"(?m)^\s*(?:import\b[^\n]*\bjson\b|from\s+json\s+import\b)", src),
            "K1-E4 RED: apply 脚本未用 stdlib json（冻结契约：零额外依赖、不依赖 jq）。")
        self.assertIn(
            "--gen-patch", src,
            "K1-E4 RED: apply 脚本未暴露 --gen-patch 形态 —— live 应用须走 "
            "OpenClaw 自有写入口（config patch），零直写路径保留（261023 批 B "
            "先例；TaskPackage notes ⑤）。")

    # ------------------------------------------------------------------ #
    # K1-E4 · apply 真跑（幂等 / 双基线 / drift / 备份 / 只改目标键）（行为级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_apply_applies_then_idempotent(self):
        """coverage: K1-E4 — apply 真跑①：新机基线（键缺席）→ 声明目标态（五项）；
        ②：再跑 → 幂等 no-op（同值、零新备份、rc 0）；目标态命中即 SKIP、不经
        drift-guard。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        allow = self._declared_allow(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_pre()
            _write_json(cfg, pre)
            want = _target_state(pre, allow)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: apply 首次运行 rc={rc}（应 0）。stdout={out!r} stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "K1-E4 RED: apply 未产生声明目标态（kidpal tools.allow = 契约五项）。")
            backups_after_first = _backups(td, pattern)
            self.assertTrue(
                backups_after_first,
                "K1-E4 RED: 首次 apply 未建备份（前置，供下方幂等对比）。")

            rc2, out2, err2 = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            got2, _ = _load_json(cfg)
            self.assertEqual(
                rc2, 0,
                f"K1-E4 RED: apply 二次运行 rc={rc2}（幂等应 0）。"
                f"stdout={out2!r} stderr={err2!r}")
            self.assertEqual(
                got2, want, "K1-E4 RED: apply 二次运行改动了已达标态（非幂等）。")
            self.assertEqual(
                len(_backups(td, pattern)), len(backups_after_first),
                "K1-E4 RED: apply 二次运行又新建了备份（幂等应不重备份）。")

    def test_k1_e4_apply_accepts_legacy_four_baseline(self):
        """coverage: K1-E4 — apply 真跑：live 既有态（上一代四项覆写）→ 可升级到
        五项目标态（rc 0），备份内容 = 四项变更前态。不扩 drift 基准则本断言
        设计态红（exit 3 升级落不了地）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        allow = self._declared_allow(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_legacy_four()
            _write_json(cfg, pre)
            want = _target_state(pre, allow)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: live 四项覆写基线上升级 rc={rc}（应 0）—— "
                "drift_guard 升级基准未落地（TaskPackage notes ②）。"
                f"stdout={out!r} stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                got, want,
                "K1-E4 RED: 四项基线升级后 ≠ 五项目标态（其它键须原样保留）。")
            backups = _backups(td, pattern)
            self.assertTrue(backups, "K1-E4 RED: 升级未建备份（前置）。")
            saved, _ = _load_json(backups[0])
            self.assertEqual(
                saved, pre,
                "K1-E4 RED: 备份内容 ≠ 四项变更前态（备份不及时/被覆盖）。")

    def test_k1_e4_apply_drift_guard_exit3(self):
        """coverage: K1-E4 — 真漂移拒绝：异值白名单（bash/browser）、六项态
        （基线集以外、含原始消息面工具）、tools 键结构畸形（在场非对象 ≠ 键
        缺席）→ exit 3 且零写入、零备份（fail-closed）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        for label, drifted in (("异值白名单", _fixture_mismatch()),
                               ("六项态", _fixture_six_items()),
                               ("tools 结构畸形", _fixture_malformed_tools())):
            with tempfile.TemporaryDirectory() as td:
                td = Path(td)
                cfg = td / "openclaw.json"
                _write_json(cfg, drifted)

                rc, out, err = _run_py(APPLY_SCRIPT,
                                       ["--config", cfg, "--decl", DECL_PATH])
                self.assertEqual(
                    rc, 3,
                    f"K1-E4 RED: {label} 应判 drift → exit 3（实得 rc={rc}）—— "
                    "基线集以外的状态不得被当作可升级态（fail-closed）。"
                    f"stdout={out!r} stderr={err!r}")
                after, _ = _load_json(cfg)
                self.assertEqual(
                    after, drifted,
                    f"K1-E4 RED: {label} 被拒后仍改写了 config。")
                self.assertEqual(
                    _backups(td, pattern), [],
                    f"K1-E4 RED: {label} 被拒后仍建了备份（拒绝 = 完全 no-op）。")

    def test_k1_e4_apply_timestamped_backup(self):
        """coverage: K1-E4 — 写前建时间戳备份（落 config 同目录、匹配声明 pattern、
        文件名含 yyyymmddTHHMMSS、内容 = 原始 config）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        pre = _fixture_pre()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"K1-E4 RED: apply rc={rc}。stderr={err!r}")
            backups = _backups(td, pattern)
            self.assertTrue(
                backups,
                "K1-E4 RED: apply 未建匹配 backup.pattern 的备份 —— openclaw.json "
                "定点 patch 回滚先决缺失。")
            self.assertRegex(
                backups[0].name, TIMESTAMP_RE.pattern,
                f"K1-E4 RED: 备份名 {backups[0].name!r} 不含 UTC 时间戳"
                "（yyyymmddTHHMMSS）—— 非时间戳备份无法确定先后。")
            saved, _ = _load_json(backups[0])
            self.assertEqual(
                saved, pre, "K1-E4 RED: 备份内容 ≠ 变更前 config（备份不及时/被覆盖）。")

    def test_k1_e4_apply_preserves_other_keys_and_composes_with_skills_patch(self):
        """coverage: K1-E4 — apply 只改 tools.allow 键：gateway.token、261017
        skills 白名单组合态、其它 agent、modelPolicy 原样保留（secret 零触碰、
        非目标字段零扰动、声明式 patch 组合共存）。"""
        self.assertTrue(APPLY_SCRIPT.is_file(), "K1-E4 RED: apply 脚本不在位（前置）。")
        pre = _fixture_pre()
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, pre)

            rc, out, err = _run_py(APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH])
            self.assertEqual(rc, 0, f"K1-E4 RED: apply rc={rc}。stderr={err!r}")
            got, gerr = _load_json(cfg)
            self.assertIsNone(gerr, f"K1-E4 RED: apply 后 config 不可解析: {gerr}")
            self.assertEqual(
                _json_get(got, "gateway.token"), _json_get(pre, "gateway.token"),
                "K1-E4 RED: apply 改动了 gateway.token —— 定点 patch 越界触碰 secret"
                "（冻结契约：只改 agents.entries.kidpal.tools.allow）。")
            self.assertEqual(
                _json_get(got, "agents.entries.kidpal.skills"),
                _json_get(pre, "agents.entries.kidpal.skills"),
                "K1-E4 RED: apply 抹掉了 261017 skills 白名单组合态 —— 声明式 patch "
                "组合共存被破坏（plan.md 跨包事实：本包与 261017/P3 作用于同一文件，"
                "均不得整文件改写）。")
            self.assertEqual(
                _json_get(got, "agents.entries.main"), _json_get(pre, "agents.entries.main"),
                "K1-E4 RED: apply 改动了其它 agent 条目 —— 定点 patch 越界。")
            self.assertEqual(
                _json_get(got, "tools"), _json_get(pre, "tools"),
                "K1-E4 RED: apply 改动了全局 tools 块 —— 定点 patch 越界（只收窄 "
                "kidpal agent，不动全局 profile）。")
            self.assertEqual(
                _json_get(got, "modelPolicy"), _json_get(pre, "modelPolicy"),
                "K1-E4 RED: apply 改动了 modelPolicy —— 定点 patch 越界。")

    # ------------------------------------------------------------------ #
    # K1-E4 · gen-patch 形态（patch 生成器，纯离线）（行为级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_gen_patch_product_exactly_target_key_and_zero_config_write(self):
        """coverage: K1-E4 — --gen-patch：rc 0 产出 JSON 子集文件、展平叶键集恰 =
        {change.path: change.allow}（五项）一个叶键；不写 config（字节不变、
        零备份）；不读 config（config 路径不存在时仍可生成）。"""
        decl = self._decl()
        pattern = self._backup_pattern(decl)
        allow = self._declared_allow(decl)
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            pre = _fixture_legacy_four()
            _write_json(cfg, pre)
            before = cfg.read_bytes()
            out = td / "kidpal-tools.patch.json"

            rc, out_text, err = _run_py(
                APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH, "--gen-patch", out])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: --gen-patch rc={rc}（应 0）—— patch 生成器形态缺失"
                f"（TaskPackage notes ⑤，261023 批 B 先例）。stdout={out_text!r} "
                f"stderr={err!r}")
            self.assertTrue(
                out.is_file(), "K1-E4 RED: --gen-patch 未产出 patch 文件。")
            patch, perr = _load_json(out)
            self.assertIsNone(
                perr,
                f"K1-E4 RED: patch 产物不可被 stdlib json 解析: {perr}（冻结契约："
                "JSON 子集形态，JSON5 超集不得破坏 stdlib 解析）。")
            leaves = sorted(
                (p, json.dumps(v, sort_keys=True, ensure_ascii=False))
                for p, v in _flatten_leaves(patch))
            want = [(TARGET_PATH,
                     json.dumps(list(allow), sort_keys=True, ensure_ascii=False))]
            self.assertEqual(
                leaves, want,
                "K1-E4 RED: patch 产物展平叶键集 ≠ 声明目标键逐字（须恰一个叶键 "
                f"{TARGET_PATH} = 五项 allow；不多不少）。\n实得：{leaves}")
            self.assertIn(
                "honcho_search_conclusions",
                json.dumps(patch, ensure_ascii=False),
                "K1-E4 RED: patch 产物不含 honcho_search_conclusions —— 检索工具"
                "未进写入面。")

            self.assertEqual(
                cfg.read_bytes(), before,
                "K1-E4 RED: --gen-patch 改写了 config —— 生成器形态须零 config 写入"
                "（配置只经 OpenClaw 自有写入口落盘）。")
            self.assertEqual(
                _backups(td, pattern), [],
                "K1-E4 RED: --gen-patch 建了备份 —— 生成器形态须零副作用。")

            # 不读 config：指向不存在的 config 也能生成（生成器 = 纯声明消费）
            out2 = td / "kidpal-tools.patch2.json"
            rc2, out2_text, err2 = _run_py(
                APPLY_SCRIPT, ["--config", td / "does-not-exist.json",
                               "--decl", DECL_PATH, "--gen-patch", out2])
            self.assertEqual(
                rc2, 0,
                f"K1-E4 RED: --gen-patch 在 config 不存在时 rc={rc2}（应 0）—— "
                "生成器形态不得依赖 config（冻结契约：不读 config）。"
                f"stdout={out2_text!r} stderr={err2!r}")
            self.assertTrue(
                out2.is_file(), "K1-E4 RED: config 缺失时 --gen-patch 未产出文件。")

    # ------------------------------------------------------------------ #
    # K1-E4 · 回滚（dry-run + 一步还原）（行为级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_rollback_dry_run_and_restore(self):
        """coverage: K1-E4 — 回滚脚本 --dry-run 可跑（exit 0）且零写入；非
        dry-run 一步还原 backup 内容（exit 0）。"""
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"K1-E4 RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        with tempfile.TemporaryDirectory() as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            bak = td / "openclaw.json.bak-kidpal-tools-20261023T000000"
            cur = {"applied": True}
            _write_json(cfg, cur)
            _write_json(bak, {"applied": False})

            rc, out, err = _run_py(
                ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak, "--dry-run"])
            self.assertEqual(
                rc, 0,
                f"K1-E4 RED: rollback --dry-run rc={rc}（应 0）。"
                f"stdout={out!r} stderr={err!r}")
            after, _ = _load_json(cfg)
            self.assertEqual(after, cur, "K1-E4 RED: --dry-run 改写了 config。")

            rc2, out2, err2 = _run_py(ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak])
            self.assertEqual(
                rc2, 0, f"K1-E4 RED: rollback rc={rc2}。stdout={out2!r} stderr={err2!r}")
            after2, _ = _load_json(cfg)
            self.assertEqual(
                after2, {"applied": False}, "K1-E4 RED: 回滚未还原 backup 内容。")

    # ------------------------------------------------------------------ #
    # K1-E4 · openclaw.json 零入仓 / 声明零 secret（机制级）
    # ------------------------------------------------------------------ #

    def test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration(self):
        """coverage: K1-E4 — openclaw.json 整文件不入仓（含 gateway.token）；声明
        工件内不得出现 gateway.token 字样、不得出现 token 键。"""
        leaked = [str(p) for p in SERVER.rglob("openclaw.json") if p.is_file()]
        self.assertEqual(
            leaked, [],
            "K1-E4 RED: 仓内出现 openclaw.json（含 gateway.token secret）—— "
            "只入无 secret 的声明工件，整文件零入仓。实得："
            f"{leaked}")
        decl_text = _read_text(DECL_PATH)
        self.assertTrue(decl_text.strip(), "K1-E4 RED: 声明工件不在位/为空（前置）。")
        self.assertNotIn(
            "gateway.token", decl_text,
            "K1-E4 RED: 声明工件出现 gateway.token 字样 —— 声明面不得承载 secret 键。")
        self.assertIsNone(
            re.search(r'"token"\s*:', decl_text),
            "K1-E4 RED: 声明工件出现 token 键 —— 声明面不得承载 secret。")

    # ------------------------------------------------------------------ #
    # K1-E4 · manifest 登记（部署单一事实源，261013 纪律）
    # ------------------------------------------------------------------ #

    def test_k1_e4_manifest_registers_the_patch_workbench(self):
        """coverage: K1-E4 — manifest 至少三条 261022-openclaw-kidpal-tools 条目
        （声明 / apply / rollback），src 在仓内存在、dst 在
        /opt/kidpal/openclaw-patch/261022-openclaw-kidpal-tools/ 消费位。"""
        self.assertTrue(_HAS_YAML or self.fail(
            "K1-E4 RED(infra): PyYAML 不在位——manifest 解析前置缺失。"))
        self.assertTrue(
            MANIFEST_YAML.is_file(), "K1-E4 RED: manifest.yaml 不在场。")
        try:
            doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
        except Exception as e:  # noqa: BLE001 — 解析错误是 RED，不是 crash
            self.fail(f"K1-E4 RED: manifest.yaml 解析失败: {e!r}")
        entries = [
            e for e in (doc or {}).get("entries", []) or []
            if isinstance(e, dict) and MANIFEST_DIR_TOKEN in (
                str(e.get("src") or "") + str(e.get("dst") or ""))]
        self.assertGreaterEqual(
            len(entries), 3,
            f"K1-E4 RED: manifest 对 {MANIFEST_DIR_TOKEN} 的登记条目 "
            f"{len(entries)} < 3 —— 声明 / apply / rollback 三件须各入部署单一"
            "事实源（261013 纪律：新资产必须登记并接入 verify-deploy.sh）。")
        srcs = [str(e.get("src") or "") for e in entries]
        missing_src = [s for s in srcs if s and not (REPO_ROOT / s).is_file()]
        self.assertEqual(
            missing_src, [],
            f"K1-E4 RED: manifest 登记的仓内源缺失: {missing_src}。")
        bad_dst = [str(e.get("dst") or "") for e in entries
                   if not str(e.get("dst") or "").startswith(
                       "/opt/kidpal/openclaw-patch/" + MANIFEST_DIR_TOKEN)]
        self.assertEqual(
            bad_dst, [],
            f"K1-E4 RED: 登记条目 dst 越出 openclaw-patch 消费位: {bad_dst}。")


class MemoryRecallRoundAcceptance(unittest.TestCase):
    """261024-memory-recall-tool-enable K1-E4-B（AGENTS.md 静态语义，offline）。

    被测面 = server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md（静态语义
    单一事实源）。断言口径 = 语义锚组（措辞换写不误红、纪律语义被删才红）。
    live 面（真机问「王一诺是谁」+ 日志工具调用）= human 部署轮，本类零 host
    断言（头注缺口节已申报）。"""

    maxDiff = None

    def _agents_body(self) -> str:
        self.assertTrue(
            AGENTS_MD.is_file(),
            f"K1-E4-B RED: {AGENTS_MD} 不在场 —— 静态语义单一事实源镜像缺失。")
        body = _read_text(AGENTS_MD)
        self.assertTrue(body.strip(), "K1-E4-B RED: AGENTS.md 为空。")
        return body

    def test_k1_e4_b_memory_recall_round_row_semantics(self):
        """coverage: K1-E4-B — 「回合类型与纪律路由」表内新增「记忆回忆回合」行：
        识别特征 + 先调一次 honcho_search_conclusions + 查到自然转述（不逐字 /
        不带内部流程 / 家长隐私不回忆）+ 查不到才说记不清 + 过渡语 + 与孩子对话
        回合禁工具的关系显式写清；honcho_search_messages 不得出现在本行。"""
        body = self._agents_body()
        section = _section_span(body, ROUTING_HEADING_NEEDLE)
        self.assertIsNotNone(
            section,
            f"K1-E4-B RED: AGENTS.md 无「{ROUTING_HEADING_NEEDLE}」节 —— 回合路由"
            "承载面缺失（K3 冻结锚串同缺）。")
        rows = [l for l in section.splitlines() if MEMORY_ROUND_NAME in l]
        self.assertTrue(
            rows,
            f"K1-E4-B RED: 回合路由节内无「{MEMORY_ROUND_NAME}」行 —— 记忆回忆纪律"
            "未落（模型仍按「孩子对话回合禁工具」执行，历史事实永远答不出）。")
        self.assertTrue(
            any(l.lstrip().startswith("|") for l in rows),
            f"K1-E4-B RED: 「{MEMORY_ROUND_NAME}」不在回合路由**表**内（须为表格行）"
            "—— 纪律落点须与其它回合同表，避免与「孩子对话回合」抢判定。")
        row = "\n".join(rows)
        for what, group in MEMORY_ROUND_ANCHORS.items():
            self.assertTrue(
                any(w in row for w in group),
                f"K1-E4-B RED: 「{MEMORY_ROUND_NAME}」行缺纪律锚组「{what}」"
                f"（任一即可：{group}）—— 语义删除或稀释。")
        self.assertNotIn(
            FORBIDDEN_TOOL, row,
            f"K1-E4-B RED: 「{MEMORY_ROUND_NAME}」行出现 {FORBIDDEN_TOOL!r} —— "
            "原始消息面（家长代述 / 流程内容）不得进入儿童端回合纪律。")

    def test_k1_e4_b_memory_section_keeps_no_tool_for_recording_sentence(self):
        """coverage: K1-E4-B — 记忆节保留原句关键半句「不调用任何工具去记」
        （工具不用于「记」仍真；不得为放行检索而删改）+ 「不需要写任何文件」+
        261023 A3 四锚继续在场 + 新增一句检索澄清（记仍自动、旧事回忆走检索）。"""
        body = self._agents_body()
        mem = _section_span(body, MEMORY_SECTION_NEEDLE)
        self.assertIsNotNone(
            mem,
            f"K1-E4-B RED: AGENTS.md 无标题含「{MEMORY_SECTION_NEEDLE}」的 `## ` 节 "
            "—— 记忆守则节被删/改名（261023 A3 承载面缺失）。")
        self.assertIn(
            "不调用任何工具去记", mem,
            "K1-E4-B RED: 记忆节原句半句「不调用任何工具去记」不逐字在场 —— "
            "为放行检索而删改记忆守则会同时破 261023 A3「不调用任何工具」正锚；"
            "正确改法是保留原句（工具不用于「记」仍真）+ 新增检索澄清句。")
        for positive in A3_MEMORY_POSITIVES_COMPAT:
            self.assertIn(
                positive, mem,
                f"K1-E4-B RED: 记忆节缺 A3 正锚 {positive!r}（261023 冻结面"
                "零回退：平台自动承载 / 想不起来诚实说记不清 / 绝不编造）。")
        self.assertTrue(
            any(w in mem for w in MEMORY_RETRIEVAL_CLARIFY),
            "K1-E4-B RED: 记忆节无检索澄清锚（任一即可："
            f"{MEMORY_RETRIEVAL_CLARIFY}）—— 「自动记」与「旧事要查」两个口径"
            "未分开，模型会在「不需要工具」与「先查一次」之间自相矛盾。")

    def test_k1_e4_b_anchor_compat_no_regression(self):
        """coverage: K1-E4-B — 既有锚兼容红线（additive 换代）：K1-E1 回合路由
        五行零回退（孩子对话回合行禁工具锚组仍在）；K3_P1_AGENTS_ANCHORS 逐字；
        261023 A3 对象锁定行 K3 隐私锚串 + S3 澄清两半逐字。"""
        body = self._agents_body()
        for round_name in K1E1_ROUND_NAMES:
            self.assertTrue(
                [l for l in body.splitlines() if round_name in l],
                f"K1-E4-B RED: AGENTS.md 无含「{round_name}」的行 —— K1-E1 回合"
                "路由五行被新行挤掉（换代须 additive）。")
        child_row = "\n".join(l for l in body.splitlines() if "孩子对话回合" in l)
        self.assertTrue(
            any(w in child_row for w in K1E1_CHILD_ROUND_TOOL_BAN),
            "K1-E4-B RED: 孩子对话回合行禁工具锚组消失 —— K1-E1 核心纪律被削弱"
            "（查询放行只许加法：新回合行承载，不动既有回合）。")
        for anchor in K3_P1_AGENTS_ANCHORS_COMPAT:
            self.assertIn(
                anchor, body,
                f"K1-E4-B RED: K3_P1_AGENTS_ANCHORS 锚串不逐字在场：{anchor!r} —— "
                "P1 迁移段单向追加红线被破（test_261022_honcho_memory 同锚）。")
        lock_lines = [l for l in body.splitlines() if A3_OBJECT_LOCK_KEY in l]
        self.assertTrue(
            lock_lines,
            f"K1-E4-B RED: AGENTS.md 无「{A3_OBJECT_LOCK_KEY}」行 —— 261023 A3 "
            "对象锁定条缺位。")
        for line in lock_lines:
            self.assertIn(
                K3_P1_AGENTS_ANCHORS_COMPAT[-1], line,
                "K1-E4-B RED: 对象锁定行 K3 隐私锚串不逐字在场（含右括号）—— "
                "261023 A3 冻结锚被改。")
            for clarify in A3_S3_CLARIFIES:
                self.assertIn(
                    clarify, line,
                    f"K1-E4-B RED: 对象锁定行缺 S3 澄清 {clarify!r} —— 261023 A3 "
                    "甲案裁决在场面被删。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
