"""Frozen acceptance tests — TaskPackage 261025-openclaw-dreaming-disable
（Lite / riskTier high / gateProfile security / sourceType human-ad-hoc）

subsystem = server/deploy/261025-openclaw-dreaming-disable/（声明 + apply +
rollback 三件）+ server/deploy/manifest.yaml（三件登记）+
server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md（升级检查清单钉一行）。

语义权威 = 诊断件 §4 architect 裁决（方案 A：全局关闭 memory-core dreaming）
+ §5 human 2026-10-25 四裁决（采纳 A / 清理归 human 运维轮 / spec 修订并入面 C
/ 回归防线先记 debt + runbook 钉一行）；debt 条目 =
docs/debt/tech/TECH-261025-dreaming-capture-pollutes-honcho.md。技术形态裁决 =
plan.md「技术方案（planner+architect 预评）」五条（architect 1g）。

被测资产（builder 创建；冻结时点均不存在 → 本件全红为设计态红）：

  · server/deploy/261025-openclaw-dreaming-disable/dreaming-disable-declaration.json
  · server/deploy/261025-openclaw-dreaming-disable/apply_dreaming_disable.py
  · server/deploy/261025-openclaw-dreaming-disable/rollback_dreaming_disable.py
  · server/deploy/manifest.yaml（既有，增三件登记）
  · server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md（既有，增升级清单一行）

===========================================================================
FROZEN OFFLINE CONTRACT（builder MUST implement；纯 python / 离线可跑 / 导入期
零网络 I/O；形制承 261022 姊妹件 test_261022_memory_isolation 的
K3OpenclawPatchAcceptance 与 test_261022_openclaw_kidpal_tools）

落位目录 = server/deploy/261025-openclaw-dreaming-disable/

  1) dreaming-disable-declaration.json（JSON，stdlib json 解析）——DD-A1 / DD-A5
     必需顶层键：case / changes / drift_guard / backup / rollback / _note。
       · case    : 非空字符串。
       · changes : list，**恰 1 条** {path, value}：
             path  = plugins.entries.memory-core.config.dreaming.enabled（逐字）
             value = **布尔 false**（严格类型判：`0` / `"false"` / 缺省值都不是
                     false；Python 下 0 == False，故实现与验收都须用 `is` 判）
       · drift_guard : list，每项 object 持非空点分 path 且**恰含 expect 或
         expect_any 之一**；判定面 = **四条叶路径 AND**（路径集恰此四条，多一条
         少一条都红）：
             - plugins.entries.memory-core.config.dreaming.enabled
                   expect_any ["unset", false]（fresh 机 / 目标态；显式 true →
                   drift；布尔候选须为真布尔 false，不容 0 冒充）
             - agents.entries.kidpal.tools.deny
                   expect_any ["unset", []]（工具面基线内 deny 非空 = drift）
             - agents.entries.kidpal.tools.profile
                   候选集恰 {"full"} —— **不接受 "unset"**：本包是 261025 工具面
                   的消费方（非生产者），接受 unset 即把「工具面回退」放过；
                   fresh 机由 exit 3 报文指引先跑 261025（与 honcho 声明「P1 未跑
                   即 drift」同族）
             - agents.entries.kidpal.tools.allow
                   候选集恰 {五项逐字（顺序敏感）} —— 五项 = read / write / exec /
                   web_search / honcho_search_conclusions；旧四项态 = 工具面回退
         "unset" 语义 = **键缺席**；父键在场但形状不对（如 tools 为字符串
         "disabled"）= 真漂移（fail-closed；实现须把「父键非对象」与「键缺席」
         拆开——复制 apply_agent_tools.py 的 _MALFORMED 哨兵 + expect_any 形制，
         不复制 honcho 版 drift_problems）。
       · backup  : {"pattern": 含 "bak"、不含路径分隔符的 config 同目录 glob}。
       · rollback: {"script": "rollback_dreaming_disable.py"}。
       · _note   : 非空字符串。
       · **顶层不得有 plugins 镜像**（声明唯一消费者读 changes / drift_guard；
         镜像造第二事实源，architect 1g 裁决 ⑤）。
       · **DD-A5 负锚**：changes 路径集恰一条；drift_guard 里凡 path 落在
         plugins.entries.memory-core.* 的只允许 dreaming.enabled 一个叶；
         声明 JSON 任何字符串值不得携带第二个 memory-core 路径。

  2) apply_dreaming_disable.py —— 双形态（全离线 `sys.executable <script> …`
     可真跑）。
     形态① patch 生成器：`--decl <decl> --gen-patch <out>` —— 从声明生成
       `openclaw config patch` 消费的 patch 文件；产物 stdlib json 可解析；
       展平叶键集**恰 1 个**（点分路径与值逐字 = 声明 change）；**零 config
       读写**（不读 config：config 路径不存在时仍可生成；不写 config：config
       字节不变、零备份）；成功 exit 0。
     形态② 安全应用编排（SKIP 先于 drift，architect 1g 裁决 ②——照
       apply_honcho_plugin.py；与 kidpal-tools 的「联合幂等」**有意不同**）：
         ⓪ 幂等预检：config 现状已达声明目标（dreaming 已 false）→ 打印 SKIP、
            exit 0，零写零备份零外部命令。SKIP 分支若工具面基线漂移，须把
            drift_problems 打印为 **WARN**（stdout；exit 0 不变，非静默）。
         ⓪' drift-guard：逐条核对 drift_guard，任一不中 → 打印 STOP、exit 3，
            零写零备份**零外部命令**。
         ① 写前时间戳备份：落 config 同目录，文件名 = config basename + ".bak-…"
            + UTC yyyymmddTHHMMSS，匹配声明 backup.pattern，内容 = 原始 config。
         ② patch 生成 → `openclaw config patch --file <p> --dry-run --json`
            （首个外部命令；须显式 `--dry-run`；schema:true 必核）。
         ③ 正式 `openclaw config patch --file <p>`（**无 --dry-run**；OpenClaw
            自有写入口 = 唯一允许的配置改动路径）。
         ④ `openclaw config validate`。
         ⑤ `systemctl restart openclaw-gateway`。
         ⑥ `openclaw health` → 其后附 NRestarts 崩循环核对（K3 exit 78 教训）。
            任一步失败 → 非零退出、立即停、其后步骤零调用、config 全程不变。
         **零 plugins install / inspect / doctor**（memory-core 为 bundled 插件；
         architect 1g 裁决 ③）。
     注入通道（离线桩真跑契约）：`--openclaw-bin <path>` —— **全部**外部副作用
       命令（openclaw CLI 与 systemctl）经该注入桩执行，桩收到完整 argv
       （argv[0] = "openclaw" 或 "systemctl"）；.py 桩经 sys.executable 调起。
       **冻结负锚：脚本内零「直写 openclaw.json」路径**——行为承载 = 逐调用
       config-hash 协议（见 test_dd_apply_safe_orchestration_call_order）；源锚 =
       脚本源码含 `config patch` 与 `--dry-run` 字面、且无 config 文件 write_text
       直写路径（源锚为代理，行为真证在 hash 协议）。

  3) rollback_dreaming_disable.py —— 备份整档还原 + 重启面：
       · CLI：--config / --backup（默认取 config 旁最新声明 backup.pattern 匹配
         件）/ --dry-run / 注入桩（`--systemctl-bin`，或与 apply 同形的
         `--openclaw-bin` 单桩兼收 openclaw 与 systemctl 两类 argv）。
       · --dry-run：只打印意图、exit 0、不写、不重启、零外部命令。
       · 非 dry-run：先还原 backup 内容 → `openclaw config validate` 通过才
         `systemctl restart openclaw-gateway`（调用序：validate 在前、restart 在
         后）；validate 失败则不重启并报错（exit 非 0）。
       · 头注须显式区分「还原已知合法快照」与「禁脚本拼装新目标态绕过 zod」
         （源锚见本件缺口节：本件不钉头注措辞）。

  4) server/deploy/manifest.yaml —— 对本包登记**恰 3 条**（声明 / apply /
     rollback），src 在仓内存在，dst 在
     /opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/ 下，
     三字段 owner: deploy.sh / verify: md5 / restart: none。

  5) server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md —— 升级检查清单钉一行，
     语义锚组须在场（措辞可换、口径不可删）：dreaming-narrative 家族 +
     cron: / heartbeat: / subagent: 三个家族 token + 「必须为空」或等价口径 +
     引 TECH-261025-dreaming-capture-pollutes-honcho。

  6) openclaw.json 整文件零入仓（含 gateway 凭据）；声明零 secret
     （无 gateway.token 字样、无 token 键、无 sk- 长键）。

===========================================================================
强度申报（契约条目 → 承载断言 → 性质档）：

  · DD-A1 声明 shape 恰一叶键 + 严格布尔 false + 无顶层 plugins 镜像
        → test_dd_a1_declaration_exactly_one_leaf_and_strict_false → 机制级
  · DD-A2 gen-patch 单叶 + 零 config 读写
        → test_dd_a2_gen_patch_single_leaf_zero_config_io → 行为级（真跑）
  · DD-A3 drift-guard 判定面恰四条叶路径 AND（候选集逐字）
        → test_dd_a3_drift_guard_acceptance_surface → 机制级
  · DD-A3 接受 fresh（dreaming unset + 工具面升级态）→ 可升级
        → test_dd_a3_accepts_fresh_with_tool_face_upgraded → 行为级（真跑）
  · DD-A3 拒绝六态（dreaming 显式 true / allow 回退四项 / profile 缺席 /
        profile 非 full / deny 非空 / 父键畸形）→ exit 3 零写零备份零外部命令
        → test_dd_a3_rejects_true_and_tool_face_regressions → 行为级（真跑）
  · DD-A4 幂等 SKIP（逐字节不变 / 零备份 / 零外部命令）
        → test_dd_a4_target_state_skips_bytewise_noop → 行为级（真跑）
  · DD-A4 子 case：SKIP 先于 drift + drift WARN 非静默（exit 0 不变）
        → test_dd_a4_skip_before_drift_warns_on_tool_face_drift → 行为级（真跑）
  · apply 安全编排调用序 + 逐调用 config-hash 零直写 + 终态目标态 + 备份
        → test_dd_apply_safe_orchestration_call_order → 行为级（真跑）
  · apply 失败即停 / 失败序零写（dry-run 失败 / 正式 patch 失败）
        → test_dd_apply_failure_stops_and_writes_nothing → 行为级（真跑）
  · 禁脚本直写 openclaw.json（源锚）
        → test_dd_apply_source_has_patch_entry_and_no_direct_config_write → 源锚代理
  · rollback --dry-run 零写零重启
        → test_dd_rollback_dry_run_writes_nothing → 行为级（真跑）
  · rollback 先 validate 后 restart + 整档还原 + validate 失败不重启
        → test_dd_rollback_restores_and_validates_before_restart → 行为级（真跑）
  · manifest 三件登记（src 在场 / dst 消费位 / owner·verify·restart）
        → test_dd_manifest_registers_three_assets → 机制级
  · runbook 升级检查清单语义锚组
        → test_dd_runbook_upgrade_checklist_anchors → 源锚代理
  · openclaw.json 零入仓 / 声明零 secret
        → test_dd_no_openclaw_json_in_repo_and_no_secret → 机制级

缺口（承载断言强度低于契约要求处，逐条；不猜、不静默）：

  · gen-patch 产物「可被真实 OpenClaw `config patch --dry-run --json` 接受」
    = live 面（OpenClaw 自有 zod schema），offline 只钉 JSON 子集形状与叶键集；
    真机写入口行为归 human 部署轮（plan.md「demo 证据形态」live 侧）。
  · `config patch` 对 `plugins.entries.memory-core` 的合并语义（整对象替换 vs
    逐叶合并）仓内无取证——部署轮须 patch 前后 `openclaw config get
    plugins.entries.memory-core` 比对、差异只允许 dreaming.enabled。本件零 host
    断言（plan.md 部署轮残余风险申报）。
  · 「零直写 openclaw.json」的行为证明存在离线残余盲区（与 K3-E6 同款）：若
    编排器在桩的正式 config patch **之后**做冗余直写，逐调用 hash 协议不可分辨
    （终态相同）；但 dry-run 失败 / 正式 patch 失败两个失败序场景下任何直写都会
    被终态与逐调用 hash 双查捕获。源锚（无 config write_text 路径）为第二道。
  · SKIP 分支 WARN 只钉「stdout 出现 WARN 且漂移路径可辨」，不钉句式与完整
    drift_problems 文本（措辞随实现，语义不可删）。
  · runbook 只钉语义锚组 + 一处 ±900 字符窗口内聚，不钉行数、不钉清单位置
    （"钉一行"的物理行数不是可机判语义）。
  · 声明 `_note` 只钉「为非空字符串」，不钉三句内容口径（全局关闭 / 无
    per-agent 作用域 / 重评触发）——措辞归 build 与人工 review，验收不制造
    脆性句式断言；声明顶层额外键亦被容忍（只钉必需六键在位 + 零 plugins 镜像）。
  · 幂等预检 / drift-guard「先于任何外部命令」离线由注入桩零记录证；ECS 上
    实际 openclaw.json 是否已为工具面升级态，由 apply 时 STOP / SKIP 回执承载
    （261017 PV-A6 同款缺口）。
  · rollback 的注入桩 CLI 名存在实现自由度（`--systemctl-bin` 为 plan 口径，
    姊妹 apply 命名 `--openclaw-bin`）：本件接受二者其一（见
    _run_rollback_injected），钉的是「注入通道存在 + 调用序 + 还原语义」，
    不钉旗标拼写。**若两者都缺 → 红**（离线不可跑）。
  · live 面零 host 断言（显式申报）：`openclaw config patch --dry-run` 接受产物 /
    `openclaw.json` 实落态 / 重启后 `openclaw health` / 一个调度周期无新增
    `dreaming-narrative-*` 会话 / 真机旧事抽问不劣化——均归 human 部署轮，
    本文件不设 live 用例（host 面转绿后停门 awaiting-human:live-deploy）。

fixture 自含性声明：全部断言直读 repo 内制品 + tempfile fixture（工具面升级态
组合态与 openclaw 桩脚本均内嵌本文件生成）；零网络 I/O、零外部资产、零环境前提
（PyYAML 仅 manifest 断言用，与 verify-deploy.sh 同一解析器）。evidence 类断言
仓根由 __file__ 显式推导（REPO_ROOT / SERVER），不依赖调用点 cwd。行为 seam
驱动方对账：声明 changes → OpenClaw 运行时配置的驱动方 = 部署轮（offline 由注入
桩模拟写入口，真写入口 = live 盲区已申报）；工具面现状锚 → 261025 批 live 已部署
态（本件 fixture 固化该形态，实时漂移由 apply STOP 回执承载）；runbook 清单 →
升级后人工执行（无自动驱动方，源锚代理，已申报）。

Run protocol（文件名两点须 importlib 模式）:
  cd server && python -m pytest -q --import-mode=importlib -m "not live" \
      acceptance/test_261025_openclaw_dreaming_disable.acceptance.py
  （standalone: python 本文件）

冻结 RED 基线（2026-10-25，本树 builder 尚未动工；实测）：

  · 本件单跑：**17 failed / 0 passed / 1.02s**（17 收集，全部设计态红）。
    红因分组（逐条均指向「本包未实现」，无环境 / 测试自身错误）：
      - 声明工件缺失（DD-A1 / DD-A2 / DD-A3 判定面 / DD-A4 两态 / DD-A5）：
        `dreaming-disable-declaration.json` 不在位；
      - apply 脚本缺失（接受 fresh / 编排序 / 失败即停 / 源锚）：
        `apply_dreaming_disable.py` 不在位；
      - rollback 脚本缺失（dry-run / 还原序 / 源锚）：
        `rollback_dreaming_disable.py` 不在位；
      - manifest 对本包登记条目 = 0（应恰 3）；
      - runbook 无 `dreaming-narrative` 家族盘点行；
      - 声明工件缺位 ⇒ 零 secret 负锚无承载面（red）。
  · 全量 offline：**17 failed / 353 passed / 7 deselected / 33 subtests
    passed**（30.68s）；冻结前同树基线 = 353 passed / 7 deselected / 33
    subtests passed ⇒ **既有 353 条零回退**，新增 17 条全红 = 本包设计态红。

双向可满足性证据（不是推理，是实测）：冻结过程在**仓外临时树**（不碰仓内
任何文件）搭了一份参考实现（声明 / apply / rollback / manifest 三行 / runbook
一行），同一本件跑出 **17 passed**；随后两条反向变异各让对应断言转红——
① 声明 profile 锚放宽为 `expect_any ["unset", "full"]` → DD-A3 判定面红；
② rollback 取消 `openclaw config validate` 调用 → 回滚调用序断言红。证明断言
既非空过、也非不可满足；参考实现为临时脚手架，已删除，不构成实现方案约束
（实现自由仍归 builder：仅在冻结语义边界内）。
"""

from __future__ import annotations

import hashlib
import json
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

try:
    import yaml  # manifest 断言前置；缺席 = infra 红（K3 / K1 同口径）
    _HAS_YAML = True
except ImportError:  # pragma: no cover
    _HAS_YAML = False

REPO_ROOT = Path(__file__).resolve().parents[2]
SERVER = REPO_ROOT / "server"
DEPLOY = SERVER / "deploy"
WORKBENCH = DEPLOY / "261025-openclaw-dreaming-disable"
DECL_PATH = WORKBENCH / "dreaming-disable-declaration.json"
APPLY_SCRIPT = WORKBENCH / "apply_dreaming_disable.py"
ROLLBACK_SCRIPT = WORKBENCH / "rollback_dreaming_disable.py"
MANIFEST_YAML = DEPLOY / "manifest.yaml"
RUNBOOK = DEPLOY / "openclaw-patch" / "DEPLOY-RUNBOOK.md"

# --------------------------------------------------------------------------- #
# 冻结字面（变更 = 语义放宽/收紧，走 tighten-only 纪律反审）
# --------------------------------------------------------------------------- #

WORKBENCH_TOKEN = "261025-openclaw-dreaming-disable"
DECL_NAME = "dreaming-disable-declaration.json"
APPLY_NAME = "apply_dreaming_disable.py"
ROLLBACK_NAME = "rollback_dreaming_disable.py"

DREAMING_PATH = "plugins.entries.memory-core.config.dreaming.enabled"
MEMORY_CORE_PREFIX = "plugins.entries.memory-core"

TOOLS_OBJECT_PATH = "agents.entries.kidpal.tools"
TOOLS_PROFILE_PATH = "agents.entries.kidpal.tools.profile"
TOOLS_ALLOW_PATH = "agents.entries.kidpal.tools.allow"
TOOLS_DENY_PATH = "agents.entries.kidpal.tools.deny"

# 261025 工具面 live 升级态（本包 drift_guard 前置基线；dependsOn 前账）
TOOLS_PROFILE = "full"
UPGRADED_ALLOW = ("read", "write", "exec", "web_search",
                  "honcho_search_conclusions")
LEGACY_ALLOW_FOUR = ("read", "write", "exec", "web_search")

DEPLOY_DST_PREFIX = "/opt/kidpal/openclaw-patch/" + WORKBENCH_TOKEN
DEBT_ID = "TECH-261025-dreaming-capture-pollutes-honcho"

SECRET_KEY_RE = re.compile(r"sk-[A-Za-z0-9]{16,}")
TIMESTAMP_RE = re.compile(r"\d{8}T\d{6}")
GATEWAY_UNIT = "openclaw-gateway"

# 源锚：禁直写 config 的形态（写 openclaw.json 只许经 OpenClaw 写入口）
_DIRECT_CONFIG_WRITE_RES = (
    re.compile(r"\b(?:config_path|config_file|cfg_path|cfg_file)\.write_text\s*\("),
    re.compile(r"open\(\s*[A-Za-z_]*config[A-Za-z_]*\s*,\s*['\"]w"),
)

_MISSING = object()
_ABSENT = object()


# --------------------------------------------------------------------------- #
# 读盘 / 进程 helpers（仓根由 __file__ 显式推导，不依赖调用点 cwd）
# --------------------------------------------------------------------------- #

def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def _sha_file(path: Path) -> str:
    try:
        return hashlib.sha256(path.read_bytes()).hexdigest()
    except OSError:
        return "MISSING"


def _run_py(script: Path, args, timeout: int = 180):
    """用当前解释器跑 python 脚本（离线可跑，不经 bash/ssh）；脚本缺失 → rc 127。"""
    if not script.is_file():
        return 127, "", f"{script} 不在位（冻结契约工件缺失）"
    proc = subprocess.run(
        [sys.executable, str(script), *[str(a) for a in args]],
        capture_output=True, text=True, encoding="utf-8", errors="replace",
        timeout=timeout,
    )
    return proc.returncode, proc.stdout, proc.stderr


def _load_json(path: Path):
    txt = _read_text(path)
    if not txt:
        return None, f"{path} 缺失或为空"
    try:
        return json.loads(txt), None
    except ValueError as e:  # noqa: BLE001
        return None, f"{path} 非法 JSON: {e!r}"


def _write_json(path: Path, obj) -> None:
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2),
                    encoding="utf-8")


def _require_decl(tag: str) -> dict:
    if not DECL_PATH.is_file():
        raise AssertionError(
            f"{tag} RED: 声明工件不在位 —— 冻结契约要求 {DECL_PATH}。"
            "openclaw.json 含 gateway 凭据整文件不入仓，只入无 secret 声明。")
    decl, err = _load_json(DECL_PATH)
    if decl is None:
        raise AssertionError(f"{tag} RED: 声明工件不可解析 —— {err}。")
    if not isinstance(decl, dict):
        raise AssertionError(f"{tag} RED: 声明工件顶层必须是 JSON object。")
    return decl


def _require_changes(decl: dict, tag: str) -> list:
    changes = decl.get("changes")
    if not isinstance(changes, list) or not changes:
        raise AssertionError(
            f"{tag} RED: 声明缺非空 changes list —— 写入面 = changes "
            "[{path, value}]（恰一条 dreaming 叶键）。")
    return changes


def _require_backup_pattern(decl: dict, tag: str) -> str:
    backup = decl.get("backup")
    pattern = backup.get("pattern") if isinstance(backup, dict) else None
    if not (isinstance(pattern, str) and pattern.strip()):
        raise AssertionError(f"{tag} RED: 声明缺 backup.pattern（前置）。")
    if "/" in pattern or "\\" in pattern:
        raise AssertionError(
            f"{tag} RED: backup.pattern = {pattern!r} 含路径分隔符 —— 须为 config "
            "同目录下备份文件的相对 glob。")
    return pattern


# --------------------------------------------------------------------------- #
# 点分路径读写 / 展平（openclaw.json 形状）
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
    """嵌套树 → (点分叶路径, 标量值) 序列（list 作叶子）。"""
    if isinstance(obj, dict):
        for k, v in obj.items():
            yield from _flatten_leaves(v, f"{prefix}.{k}" if prefix else str(k))
    else:
        yield prefix, obj


def _leaf_delta(pre, post) -> dict:
    """pre → post 展平叶差集：{叶路径: (pre 值 | _ABSENT, post 值 | _ABSENT)}。"""
    a = dict(_flatten_leaves(pre))
    b = dict(_flatten_leaves(post))
    delta = {}
    for key in sorted(set(a) | set(b)):
        before, after = a.get(key, _ABSENT), b.get(key, _ABSENT)
        if before != after:
            delta[key] = (before, after)
    return delta


def _iter_strings(obj):
    if isinstance(obj, str):
        yield obj
    elif isinstance(obj, dict):
        for k, v in obj.items():
            yield str(k)
            yield from _iter_strings(v)
    elif isinstance(obj, (list, tuple)):
        for item in obj:
            yield from _iter_strings(item)


def _canon_alt(alt) -> str:
    """drift 候选规范形（判「候选集恰 = 」用；标量类型敏感——0 与 false 不同）。"""
    if alt == "unset":
        return "str:unset"
    return json.dumps(alt, sort_keys=True, ensure_ascii=False)


def _drift_alts_by_path(decl: dict) -> dict:
    """{点分 path: [候选...]}；expect 单项 + expect_any 展开。"""
    out: dict = {}
    dg = decl.get("drift_guard")
    if not isinstance(dg, list):
        return out
    for entry in dg:
        if not isinstance(entry, dict):
            continue
        path = entry.get("path")
        if not isinstance(path, str) or not path.strip():
            continue
        bucket = out.setdefault(path, [])
        if "expect" in entry:
            bucket.append(entry.get("expect"))
        ea = entry.get("expect_any")
        if isinstance(ea, list):
            bucket.extend(ea)
    return out


# --------------------------------------------------------------------------- #
# fixture（261025 工具面 live 升级态 = 本包 drift 前置基线；memory-core 键未设 =
# 本包 target 写入面所在分支）
# --------------------------------------------------------------------------- #

def _fixture_tools_upgraded() -> dict:
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
                    "tools": {
                        "profile": TOOLS_PROFILE,
                        "allow": list(UPGRADED_ALLOW),
                        "deny": [],
                    },
                },
            },
        },
        "modelPolicy": {"allow": ["openclaw/kidpal"]},
    }


def _fixture_dreaming_disabled() -> dict:
    """目标态：dreaming false + 工具面升级态。"""
    state = _fixture_tools_upgraded()
    _json_set(state, DREAMING_PATH, False)
    return state


def _fixture_dreaming_true() -> dict:
    state = _fixture_tools_upgraded()
    _json_set(state, DREAMING_PATH, True)
    return state


def _fixture_allow_legacy_four() -> dict:
    state = _fixture_tools_upgraded()
    _json_set(state, TOOLS_ALLOW_PATH, list(LEGACY_ALLOW_FOUR))
    return state


def _fixture_profile_absent() -> dict:
    state = _fixture_tools_upgraded()
    _json_set(state, TOOLS_OBJECT_PATH,
              {"allow": list(UPGRADED_ALLOW), "deny": []})
    return state


def _fixture_profile_not_full() -> dict:
    state = _fixture_tools_upgraded()
    _json_set(state, TOOLS_OBJECT_PATH,
              {"profile": "coding", "allow": list(UPGRADED_ALLOW), "deny": []})
    return state


def _fixture_deny_nonempty() -> dict:
    state = _fixture_tools_upgraded()
    _json_set(state, TOOLS_OBJECT_PATH,
              {"profile": TOOLS_PROFILE, "allow": list(UPGRADED_ALLOW),
               "deny": ["bash"]})
    return state


def _fixture_tools_malformed() -> dict:
    """父键在场但非对象（_MALFORMED 类）——不得按「键缺席」放行。"""
    state = _fixture_tools_upgraded()
    _json_set(state, TOOLS_OBJECT_PATH, "disabled")
    return state


def _fixture_skip_with_toolface_drift() -> dict:
    """目标态已达标（dreaming false）但工具面回退（allow 四项）——SKIP 先于
    drift 的验收测试 case：exit 0 仍 SKIP，但 stdout 须打 drift WARN。"""
    state = _fixture_dreaming_disabled()
    _json_set(state, TOOLS_ALLOW_PATH, list(LEGACY_ALLOW_FOUR))
    return state


def _target_state(pre: dict, changes) -> dict:
    state = json.loads(json.dumps(pre))
    for change in changes:
        _json_set(state, change["path"], json.loads(json.dumps(change["value"])))
    return state


def _backups(td: Path, pattern: str) -> list:
    return sorted(td.glob(pattern))


# --------------------------------------------------------------------------- #
# 注入桩（--openclaw-bin 通道的桩一侧）。冻结契约：桩收到完整 argv
# （argv[0] = "openclaw"|"systemctl"）；每次调用起点把 {argv, config_sha256}
# 追加进 jsonl（逐调用 hash 协议 = 「零直写 openclaw.json」负锚的行为承载）。
# fail_mode 注入三个失败点：dry-run schema 拒 / 正式 patch 失败 / validate 失败。
# 桩的成功路径 = 模拟 OpenClaw 写入口：正式 config patch 把 patch 文件展平深合并
# 进 config（JSON 子集，嵌套树或扁平点分映射两形皆收）。
# --------------------------------------------------------------------------- #

_STUB_TEMPLATE = '''
import hashlib
import json
import sys

LOG = %(log)r
CFG = %(cfg)r
FAIL_MODE = %(fail_mode)r


def _sha(path):
    try:
        with open(path, "rb") as fh:
            return hashlib.sha256(fh.read()).hexdigest()
    except OSError:
        return "MISSING"


argv = sys.argv[1:]
with open(LOG, "a", encoding="utf-8") as fh:
    fh.write(json.dumps({"argv": argv, "config_sha256": _sha(CFG)},
                        ensure_ascii=False) + "\\n")

fail = False
if FAIL_MODE == "fail_dry_run" and "patch" in argv and "--dry-run" in argv:
    fail = True
if (FAIL_MODE == "fail_patch" and "patch" in argv
        and "--dry-run" not in argv and "--file" in argv):
    fail = True
if FAIL_MODE == "fail_validate" and "validate" in argv:
    fail = True
if fail:
    print('{"ok": false, "checks": {"schema": false}}')
    print("stub: injected failure", file=sys.stderr)
    sys.exit(1)

if argv and argv[0] == "openclaw":
    rest = argv[1:]
    if rest[:2] == ["config", "patch"]:
        if "--dry-run" in rest:
            print('{"ok": true, "operations": 1, "checks": {"schema": true}}')
        else:
            idx = rest.index("--file")
            with open(rest[idx + 1], encoding="utf-8") as fh:
                patch = json.load(fh)
            with open(CFG, encoding="utf-8") as fh:
                cfg = json.load(fh)

            def flat(obj, prefix=""):
                if isinstance(obj, dict):
                    for k, v in obj.items():
                        yield from flat(v, (prefix + "." + k) if prefix else k)
                else:
                    yield prefix, obj

            for dotted, val in flat(patch):
                cur = cfg
                parts = dotted.split(".")
                for p in parts[:-1]:
                    nxt = cur.get(p)
                    if not isinstance(nxt, dict):
                        nxt = {}
                        cur[p] = nxt
                    cur = nxt
                cur[parts[-1]] = val
            with open(CFG, "w", encoding="utf-8") as fh:
                json.dump(cfg, fh, ensure_ascii=False, indent=2)
            print("Config patched")
    elif rest[:2] == ["config", "validate"]:
        print("Config valid")
    elif rest[:1] == ["health"]:
        print("Health: ok")
    else:
        print("ok")
else:
    if "show" in argv and any("NRestarts" in a for a in argv):
        print("NRestarts=0")
    else:
        print("ok")
'''


def _oc_cmd(argv, sub) -> bool:
    """argv[0] == "openclaw" 且其后恰以 sub 子序列开头。"""
    return bool(argv) and argv[0] == "openclaw" and tuple(
        argv[1:1 + len(sub)]) == tuple(sub)


def _argv_text(argv) -> str:
    return " ".join(argv)


def _records(log: Path) -> list:
    if not log.is_file():
        return []
    out = []
    for line in log.read_text(encoding="utf-8").splitlines():
        if line.strip():
            out.append(json.loads(line))
    return out


class _OrchestrationRun:
    """一次 apply 真跑的结果包（调用方负责 cleanup()）。"""

    def __init__(self, td: Path, cfg: Path, log: Path, pre: dict,
                 pre_bytes: bytes, pre_sha: str, rc: int, out: str, err: str,
                 records: list):
        self.td = td
        self.cfg = cfg
        self.log = log
        self.pre = pre
        self.pre_bytes = pre_bytes
        self.pre_sha = pre_sha
        self.rc = rc
        self.out = out
        self.err = err
        self.records = records

    @property
    def argvs(self) -> list:
        return [r["argv"] for r in self.records]

    def cleanup(self) -> None:
        shutil.rmtree(self.td, ignore_errors=True)


def _orchestrate(pre: dict, fail_mode: str = "ok") -> _OrchestrationRun:
    """工具面升级态 fixture + 注入桩真跑 apply 全序（含备份与终态事实）。"""
    if not APPLY_SCRIPT.is_file():
        raise AssertionError(
            f"RED: apply 脚本不在位 —— 冻结契约要求 {APPLY_SCRIPT}。")
    td = Path(tempfile.mkdtemp(prefix="dd-orch-"))
    cfg = td / "openclaw.json"
    _write_json(cfg, pre)
    pre_bytes = cfg.read_bytes()
    pre_sha = _sha_file(cfg)
    log = td / "stub-invocations.jsonl"
    stub = td / "stub_ext.py"
    stub.write_text(
        _STUB_TEMPLATE % {"log": str(log), "cfg": str(cfg),
                          "fail_mode": fail_mode},
        encoding="utf-8")
    rc, out, err = _run_py(
        APPLY_SCRIPT,
        ["--config", cfg, "--decl", DECL_PATH, "--openclaw-bin", stub])
    return _OrchestrationRun(td, cfg, log, pre, pre_bytes, pre_sha, rc, out,
                             err, _records(log))


# --------------------------------------------------------------------------- #
# DD-A1 / DD-A2 / DD-A5 · 声明形状与 patch 生成器
# --------------------------------------------------------------------------- #

class DreamingDisableDeclarationAcceptance(unittest.TestCase):

    maxDiff = None

    def test_dd_a1_declaration_exactly_one_leaf_and_strict_false(self):
        """coverage: DD-A1 — 声明工件可解析、必需顶层键齐（case / changes /
        drift_guard / backup / rollback / _note）、changes 恰 1 条且 path 逐字 =
        plugins.entries.memory-core.config.dreaming.enabled、value **严格布尔
        false**（`assertIs(..., False)`：0 / "false" 均不得冒充）、顶层零
        plugins 镜像（architect 1g 裁决 ⑤）。"""
        tag = "DD-A1"
        decl = _require_decl(tag)
        for key in ("case", "changes", "drift_guard", "backup", "rollback",
                    "_note"):
            self.assertIn(
                key, decl,
                f"{tag} RED: 声明工件缺必需顶层键 {key!r} —— 冻结契约结构 = "
                "case / changes / drift_guard / backup / rollback / _note"
                "（与姊妹 honcho / kidpal-tools 同族形态）。")
        self.assertIsInstance(decl.get("case"), str, f"{tag} RED: case 须为字符串。")
        self.assertTrue(str(decl.get("case", "")).strip(), f"{tag} RED: case 为空。")
        self.assertIsInstance(decl.get("_note"), str,
                              f"{tag} RED: _note 须为字符串。")
        self.assertTrue(str(decl.get("_note", "")).strip(),
                        f"{tag} RED: _note 为空 —— 全局关闭 / 无 per-agent 作用域 / "
                        "重评触发的留痕无承载面。")
        self.assertNotIn(
            "plugins", decl,
            f"{tag} RED: 声明带顶层 plugins 镜像 —— 唯一消费者读 changes / "
            "drift_guard，镜像造第二事实源（architect 1g 裁决 ⑤，禁加镜像）。")

        changes = _require_changes(decl, tag)
        self.assertEqual(
            len(changes), 1,
            f"{tag} RED: changes 条数 = {len(changes)}，须恰 1 —— 写入面 = 唯一"
            f"叶键 {DREAMING_PATH}（关做梦是一个布尔开关，不得夹带其它键）。")
        change = changes[0]
        self.assertIsInstance(change, dict, f"{tag} RED: changes[0] 须为 object。")
        self.assertEqual(
            change.get("path"), DREAMING_PATH,
            f"{tag} RED: changes[0].path = {change.get('path')!r}，须逐字 = "
            f"{DREAMING_PATH!r}。")
        self.assertIn("value", change, f"{tag} RED: changes[0] 缺 value。")
        self.assertIsInstance(
            change.get("value"), bool,
            f"{tag} RED: dreaming 叶值类型 = {type(change.get('value')).__name__}，"
            "须为布尔（\"false\" 字符串不被 OpenClaw schema 接受）。")
        self.assertIs(
            change.get("value"), False,
            f"{tag} RED: dreaming 叶值 = {change.get('value')!r}，须恰为 false —— "
            "严格 `is` 判：Python 下 0 == False，`==` 比较会放过 0 / 畸形值"
            "（architect 1g 裁决 ① 布尔叶严格判）。")

        dg = decl.get("drift_guard")
        self.assertIsInstance(dg, list, f"{tag} RED: drift_guard 须为 list。")
        self.assertTrue(dg, f"{tag} RED: drift_guard 不得为空 —— 无现状锚 = "
                            "apply 无法 fail-closed。")
        for i, entry in enumerate(dg):
            self.assertIsInstance(entry, dict,
                                  f"{tag} RED: drift_guard[{i}] 须为 object。")
            self.assertTrue(
                isinstance(entry.get("path"), str) and entry.get("path", "").strip(),
                f"{tag} RED: drift_guard[{i}].path 须为非空点分路径。")
            has_expect = "expect" in entry
            has_any = "expect_any" in entry
            self.assertTrue(
                has_expect or has_any,
                f"{tag} RED: drift_guard[{i}] 既无 expect 也无 expect_any —— "
                "基线判定面缺失。")
            self.assertFalse(
                has_expect and has_any,
                f"{tag} RED: drift_guard[{i}] 同时含 expect 与 expect_any —— "
                "两形互斥（照 apply_agent_tools.py 的 drift_problems 形制）。")

        pattern = _require_backup_pattern(decl, tag)
        self.assertIn("bak", pattern,
                      f"{tag} RED: backup.pattern = {pattern!r} 不含 'bak'。")
        rollback = decl.get("rollback")
        self.assertIsInstance(rollback, dict, f"{tag} RED: rollback 须为 object。")
        self.assertEqual(
            rollback.get("script"), ROLLBACK_NAME,
            f"{tag} RED: rollback.script = {rollback.get('script')!r}，冻结契约 = "
            f"{ROLLBACK_NAME!r}。")
        self.assertTrue(
            (WORKBENCH / str(rollback.get("script"))).is_file(),
            f"{tag} RED: 回滚脚本不在工作台 —— 声明面与回滚面不一致（回滚不可跑）。")

    def test_dd_a2_gen_patch_single_leaf_zero_config_io(self):
        """coverage: DD-A2 — `--decl <decl> --gen-patch <out>`：rc 0；产物可被
        stdlib json 解析；展平叶键集**恰 1 个**（点分路径与值逐字 = 声明 change）；
        不写 config（字节不变、零备份）；不读 config（config 路径不存在时仍可
        生成）。"""
        tag = "DD-A2"
        decl = _require_decl(tag)
        changes = _require_changes(decl, tag)
        pattern = _require_backup_pattern(decl, tag)
        with tempfile.TemporaryDirectory(prefix="dd-genpatch-") as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, _fixture_tools_upgraded())
            before = cfg.read_bytes()
            out = td / "dreaming-disable.patch.json"

            rc, out_text, err = _run_py(
                APPLY_SCRIPT, ["--config", cfg, "--decl", DECL_PATH,
                               "--gen-patch", out])
            self.assertEqual(
                rc, 0,
                f"{tag} RED: patch 生成器 rc={rc}（应 0）—— 双形态 P1 契约："
                f"--gen-patch 是 live 应用的唯一入口。stdout={out_text!r} "
                f"stderr={err!r}")
            self.assertTrue(out.is_file(), f"{tag} RED: --gen-patch 未产出 patch 文件。")
            patch, perr = _load_json(out)
            self.assertIsNone(
                perr,
                f"{tag} RED: patch 产物不可被 stdlib json 解析: {perr}（冻结契约："
                "JSON 子集形态，JSON5 超集不得破坏 stdlib 解析）。")
            leaves = sorted(
                (p, json.dumps(v, sort_keys=True, ensure_ascii=False))
                for p, v in _flatten_leaves(patch))
            want = sorted(
                (c["path"], json.dumps(c["value"], sort_keys=True,
                                       ensure_ascii=False))
                for c in changes)
            self.assertEqual(
                len(leaves), 1,
                f"{tag} RED: patch 产物展平叶键数 = {len(leaves)}，须恰 1 —— "
                f"关做梦只写 dreaming.enabled 一个叶键。实得：{leaves}")
            self.assertEqual(
                leaves, want,
                f"{tag} RED: patch 产物展平叶键集 ≠ 声明 change 逐字。\n"
                f"实得：{leaves}\n期望：{want}")

            self.assertEqual(
                cfg.read_bytes(), before,
                f"{tag} RED: --gen-patch 改写了 config —— 生成器形态须零 config "
                "写入（配置只经 OpenClaw 自有写入口落盘）。")
            self.assertEqual(
                _backups(td, pattern), [],
                f"{tag} RED: --gen-patch 建了备份 —— 生成器形态须零副作用。")

            # 不读 config：指向不存在的 config 也能生成（纯声明消费）
            out2 = td / "dreaming-disable.patch2.json"
            rc2, out2_text, err2 = _run_py(
                APPLY_SCRIPT, ["--config", td / "does-not-exist.json",
                               "--decl", DECL_PATH, "--gen-patch", out2])
            self.assertEqual(
                rc2, 0,
                f"{tag} RED: --gen-patch 在 config 不存在时 rc={rc2}（应 0）—— "
                "生成器形态不得依赖 config（冻结契约：不读 config）。"
                f"stdout={out2_text!r} stderr={err2!r}")
            self.assertTrue(
                out2.is_file(),
                f"{tag} RED: config 缺失时 --gen-patch 未产出文件。")

    def test_dd_a5_negative_anchor_no_other_memory_core_keys(self):
        """coverage: DD-A5 — 声明不得引入 memory-core 其它键：changes 路径集恰
        一条；drift_guard 路径集恰 dreaming 叶 + 工具面三叶；凡 path 落在
        plugins.entries.memory-core.* 的只允许 dreaming.enabled；声明 JSON 内
        任何字符串值不得携带第二个 memory-core 路径。"""
        tag = "DD-A5"
        decl = _require_decl(tag)
        changes = _require_changes(decl, tag)
        change_paths = [c.get("path") for c in changes]
        self.assertEqual(
            sorted(map(str, change_paths)), [DREAMING_PATH],
            f"{tag} RED: changes 路径集 = {sorted(map(str, change_paths))}，须恰 "
            f"{{{DREAMING_PATH!r}}} —— 负锚：关做梦不得夹带 memory-core 其它键"
            "（dreaming 之外的 memory-core 子键被本包覆写 = 越权）。")

        alts = _drift_alts_by_path(decl)
        expected_paths = {DREAMING_PATH, TOOLS_DENY_PATH, TOOLS_PROFILE_PATH,
                          TOOLS_ALLOW_PATH}
        self.assertEqual(
            set(alts), expected_paths,
            f"{tag} RED: drift_guard 路径集 = {sorted(alts)}，须恰 "
            f"{sorted(expected_paths)} —— 四条叶路径 AND（dreaming 叶 + 工具面"
            " deny / profile / allow）；多出的 memory-core 路径即负锚违规。")
        memory_core_paths = [p for p in alts if p.startswith(MEMORY_CORE_PREFIX)]
        self.assertEqual(
            memory_core_paths, [DREAMING_PATH],
            f"{tag} RED: drift_guard 内 memory-core 路径 = {memory_core_paths}，"
            "只允许 dreaming.enabled 一个叶——其它 memory-core 路径出现即红。")

        offenders = sorted({
            s for s in _iter_strings(decl)
            if MEMORY_CORE_PREFIX in s and s != DREAMING_PATH
        })
        self.assertEqual(
            offenders, [],
            f"{tag} RED: 声明 JSON 携带 dreaming 之外的 memory-core 字符串 "
            f"{offenders} —— 负锚：memory-core 只可出现 dreaming.enabled 一个叶。")


# --------------------------------------------------------------------------- #
# DD-A3 / DD-A4 / apply 编排 · 行为级真跑
# --------------------------------------------------------------------------- #

class DreamingDisableApplyAcceptance(unittest.TestCase):

    maxDiff = None

    def test_dd_a3_drift_guard_acceptance_surface(self):
        """coverage: DD-A3 — drift-guard 判定面恰四条叶路径 AND，候选集逐字：
        dreaming {unset, false（严格布尔）} / deny {unset, []} /
        profile 恰 {"full"}（**不接受 unset**：本包是工具面消费方）/ allow 恰
        {五项逐字，顺序敏感}。"""
        tag = "DD-A3"
        decl = _require_decl(tag)
        alts = _drift_alts_by_path(decl)
        expected = {
            DREAMING_PATH: {"str:unset", _canon_alt(False)},
            TOOLS_DENY_PATH: {"str:unset", _canon_alt([])},
            TOOLS_PROFILE_PATH: {_canon_alt(TOOLS_PROFILE)},
            TOOLS_ALLOW_PATH: {_canon_alt(list(UPGRADED_ALLOW))},
        }
        for path, want in expected.items():
            self.assertIn(
                path, alts,
                f"{tag} RED: drift_guard 无锚定 {path!r} 的条目 —— 四叶路径 AND "
                f"判定面缺一条（实得路径集：{sorted(alts)}）。")
            got = {_canon_alt(a) for a in alts[path]}
            self.assertEqual(
                got, want,
                f"{tag} RED: {path} 的基线候选集 = {sorted(got)}，须恰 "
                f"{sorted(want)}。\n"
                "  · dreaming：须含 'unset'（fresh 机）与**布尔 false**（目标态；"
                "0 / \"false\" 都不算）；显式 true 不在集内 = drift；\n"
                "  · deny：须含 'unset'（键缺席基线）与 []（工具面升级态）；"
                "deny 非空 = drift；\n"
                "  · profile：**只接受 \"full\"**——不得含 'unset'：本包是 261025 "
                "工具面的消费方不是生产者，接受缺席即放过工具面回退（fresh 机由 "
                "exit 3 报文指引先跑 261025）；\n"
                "  · allow：**只接受五项逐字**——不得含 'unset' / 旧四项；列表"
                "顺序敏感（顺序不一致 = drift）。")
        # 布尔候选须为真布尔 false（不被 0 冒充）
        dream_candidates = [a for a in alts.get(DREAMING_PATH, [])
                            if a != "unset"]
        self.assertTrue(
            any(c is False for c in dream_candidates),
            f"{tag} RED: dreaming 叶候选集无真布尔 false（实得 "
            f"{dream_candidates!r}）—— 显式目标态无法被接受。")

    def test_dd_a3_accepts_fresh_with_tool_face_upgraded(self):
        """coverage: DD-A3 — 接受 fresh（dreaming 叶 unset + 工具面升级态）：
        apply rc 0，dreaming 落 false；展平叶差集恰 {dreaming.enabled: (缺席 →
        false)}（工具面 / gateway / skills / 全局 tools 原样保留）。"""
        tag = "DD-A3"
        decl = _require_decl(tag)
        changes = _require_changes(decl, tag)
        run = _orchestrate(_fixture_tools_upgraded())
        try:
            self.assertEqual(
                run.rc, 0,
                f"{tag} RED: fresh 基线（dreaming unset + 工具面升级态）apply "
                f"rc={run.rc}（应 0）—— 本包唯一常规升级路径被拒。"
                f"stdout={run.out!r} stderr={run.err!r}")
            got, gerr = _load_json(run.cfg)
            self.assertIsNone(gerr, f"{tag} RED: apply 后 config 不可解析: {gerr}")
            self.assertIs(
                _json_get(got, DREAMING_PATH), False,
                f"{tag} RED: apply 后 dreaming 叶 = "
                f"{_json_get(got, DREAMING_PATH)!r}，须恰为布尔 false。")
            self.assertEqual(
                _json_get(got, TOOLS_PROFILE_PATH), TOOLS_PROFILE,
                f"{tag} RED: 工具面 profile 被改动 —— 本包不改写工具面。")
            self.assertEqual(
                _json_get(got, TOOLS_ALLOW_PATH), list(UPGRADED_ALLOW),
                f"{tag} RED: 工具面 allow 被改动 —— 261025 升级态须原样保留。")
            delta = _leaf_delta(run.pre, got)
            self.assertEqual(
                sorted(delta), [DREAMING_PATH],
                f"{tag} RED: pre → 终态展平叶差集 = {sorted(delta)}，须恰 "
                f"{{dreaming.enabled}} —— 只关一个布尔叶键，不多不少。")
            self.assertEqual(
                delta[DREAMING_PATH], (_ABSENT, False),
                f"{tag} RED: dreaming 叶差 = {delta[DREAMING_PATH]!r}，须为"
                "（缺席 → false）。")
            self.assertEqual(
                run.out.find("STOP"), -1,
                f"{tag} RED: fresh 接受路径打印了 STOP（stdout={run.out!r}）。")
        finally:
            run.cleanup()

    def test_dd_a3_rejects_true_and_tool_face_regressions(self):
        """coverage: DD-A3 — 拒绝六态（exit 3、零写零备份零外部命令）：
        ① dreaming 显式 true（工具面正常）② allow 回退四项 ③ profile 缺席
        ④ profile 非 "full" ⑤ deny 非空 ⑥ 父键畸形（tools = "disabled"，不得
        按「键缺席」放行）。③④ 两态下 dreaming 叶为 unset，确认 rc 仍 3。"""
        tag = "DD-A3"
        decl = _require_decl(tag)
        pattern = _require_backup_pattern(decl, tag)
        cases = (
            ("dreaming 显式 true", _fixture_dreaming_true()),
            ("261025 工具面回退：allow 四项", _fixture_allow_legacy_four()),
            ("261025 工具面回退：profile 缺席", _fixture_profile_absent()),
            ("261025 工具面回退：profile 非 full", _fixture_profile_not_full()),
            ("deny 非空（基线集以外）", _fixture_deny_nonempty()),
            ("父键畸形（tools = \"disabled\"）", _fixture_tools_malformed()),
        )
        for label, pre in cases:
            with tempfile.TemporaryDirectory(prefix="dd-drift-") as td:
                td = Path(td)
                cfg = td / "openclaw.json"
                _write_json(cfg, pre)
                before = cfg.read_bytes()
                log = td / "stub-invocations.jsonl"
                stub = td / "stub_ext.py"
                stub.write_text(
                    _STUB_TEMPLATE % {"log": str(log), "cfg": str(cfg),
                                      "fail_mode": "ok"},
                    encoding="utf-8")

                rc, out, err = _run_py(
                    APPLY_SCRIPT,
                    ["--config", cfg, "--decl", DECL_PATH,
                     "--openclaw-bin", stub])
                self.assertEqual(
                    rc, 3,
                    f"{tag} RED: drift 态「{label}」应 exit 3（实得 rc={rc}）—— "
                    "fail-closed：基线集以外不得当可升级态。"
                    f"stdout={out!r} stderr={err!r}")
                self.assertEqual(
                    cfg.read_bytes(), before,
                    f"{tag} RED: drift 态「{label}」被拒后仍改写了 config。")
                self.assertEqual(
                    _backups(td, pattern), [],
                    f"{tag} RED: drift 态「{label}」被拒后仍建了备份 —— 拒绝 = "
                    "完全 no-op。")
                self.assertEqual(
                    _records(log), [],
                    f"{tag} RED: drift 态「{label}」被拒前已调用外部命令 "
                    f"（{_records(log)}）—— drift-guard 须先于任何外部副作用。")

    def test_dd_a4_target_state_skips_bytewise_noop(self):
        """coverage: DD-A4 — 幂等预检：dreaming 已 false + 工具面升级态 →
        打印 SKIP、rc 0、config **逐字节**不变、零备份、零外部命令。"""
        tag = "DD-A4"
        decl = _require_decl(tag)
        pattern = _require_backup_pattern(decl, tag)
        with tempfile.TemporaryDirectory(prefix="dd-skip-") as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, _fixture_dreaming_disabled())
            before = cfg.read_bytes()
            log = td / "stub-invocations.jsonl"
            stub = td / "stub_ext.py"
            stub.write_text(
                _STUB_TEMPLATE % {"log": str(log), "cfg": str(cfg),
                                  "fail_mode": "ok"},
                encoding="utf-8")

            rc, out, err = _run_py(
                APPLY_SCRIPT,
                ["--config", cfg, "--decl", DECL_PATH, "--openclaw-bin", stub])
            self.assertEqual(
                rc, 0,
                f"{tag} RED: 目标态上 apply rc={rc}（应 0 = SKIP）。"
                f"stdout={out!r} stderr={err!r}")
            self.assertIn(
                "SKIP", out,
                f"{tag} RED: 目标态未打印 SKIP（stdout={out!r}）—— 幂等预检"
                "未命中（live 常态路径应为 SKIP，不是 exit 3）。")
            self.assertEqual(
                cfg.read_bytes(), before,
                f"{tag} RED: SKIP 路径改写了 config（须逐字节不变）。")
            self.assertEqual(
                _backups(td, pattern), [],
                f"{tag} RED: SKIP 路径建了备份 —— 幂等应零副作用。")
            self.assertEqual(
                _records(log), [],
                f"{tag} RED: SKIP 路径调用了外部命令（{_records(log)}）—— "
                "幂等预检须先于任何外部副作用。")

    def test_dd_a4_skip_before_drift_warns_on_tool_face_drift(self):
        """coverage: DD-A4 — 子 case（architect 1g 裁决 ②）：dreaming 已 false
        但工具面漂移（allow 回退四项）→ 按 SKIP 先于 drift 仍 rc 0 SKIP，config
        逐字节不变、零备份、零外部命令；但 stdout 须打印 drift WARN（非静默，
        exit 0 不变）。"""
        tag = "DD-A4"
        decl = _require_decl(tag)
        pattern = _require_backup_pattern(decl, tag)
        with tempfile.TemporaryDirectory(prefix="dd-skip-warn-") as td:
            td = Path(td)
            cfg = td / "openclaw.json"
            _write_json(cfg, _fixture_skip_with_toolface_drift())
            before = cfg.read_bytes()
            log = td / "stub-invocations.jsonl"
            stub = td / "stub_ext.py"
            stub.write_text(
                _STUB_TEMPLATE % {"log": str(log), "cfg": str(cfg),
                                  "fail_mode": "ok"},
                encoding="utf-8")

            rc, out, err = _run_py(
                APPLY_SCRIPT,
                ["--config", cfg, "--decl", DECL_PATH, "--openclaw-bin", stub])
            self.assertEqual(
                rc, 0,
                f"{tag} RED: SKIP 分支（dreaming 达标 + 工具面漂移）rc={rc}"
                "（应 0）—— SKIP 先于 drift 的设计决定被破（本包写目标仅 dreaming"
                " 一叶，工具面锚是前置基线；与 kidpal-tools 联合幂等有意不同）。"
                f"stdout={out!r} stderr={err!r}")
            self.assertIn(
                "SKIP", out,
                f"{tag} RED: SKIP 分支未打印 SKIP（stdout={out!r}）。")
            self.assertIn(
                "WARN", out,
                f"{tag} RED: SKIP 分支未在 stdout 打印 drift WARN（stdout="
                f"{out!r}）—— 工具面基线漂移被完全静默，运维看不到「dreaming "
                "已关但工具面回退」的组合态（architect 1g 裁决 ② 要求 WARN，"
                "exit 0 不变）。")
            self.assertTrue(
                "tools" in (out + err) or "allow" in (out + err),
                f"{tag} RED: WARN 未携带漂移锚点信息（stdout={out!r} "
                f"stderr={err!r}）—— 至少须能看出漂移落在工具面（drift_problems "
                "的 path 文本）。")
            self.assertEqual(
                cfg.read_bytes(), before,
                f"{tag} RED: SKIP 分支改写了 config（须逐字节不变）。")
            self.assertEqual(
                _backups(td, pattern), [],
                f"{tag} RED: SKIP 分支建了备份 —— 幂等应零副作用。")
            self.assertEqual(
                _records(log), [],
                f"{tag} RED: SKIP 分支调用了外部命令（{_records(log)}）。")

    def test_dd_apply_safe_orchestration_call_order(self):
        """coverage: DD-A3 真跑承载 · apply 安全编排 — 首个外部命令 =
        config patch --dry-run；序链 dry-run → 正式 patch(--file，无 --dry-run)
        → config validate → systemctl restart openclaw-gateway → openclaw health
        → NRestarts 核对；零 plugins install / inspect / doctor；逐调用
        config-hash 协议（正式 patch 及其前全部 == 变更前、其后全部 == 终态 =
        零直写证明）；终态 == 目标态（工具面升级态原样保留）；写前时间戳备份
        内容 == 变更前 config。"""
        tag = "DD-orchestration"
        decl = _require_decl("DD-A3")
        changes = _require_changes(decl, "DD-A3")
        pattern = _require_backup_pattern(decl, "DD-A3")
        run = _orchestrate(_fixture_tools_upgraded())
        try:
            self.assertEqual(
                run.rc, 0,
                f"{tag} RED: 编排 rc={run.rc}（应 0）。stdout={run.out!r} "
                f"stderr={run.err!r}")
            argvs = run.argvs
            self.assertTrue(
                argvs,
                f"{tag} RED: 注入桩零调用记录 —— 外部副作用命令未经 "
                "--openclaw-bin 注入通道执行（冻结契约：openclaw CLI 与 "
                "systemctl 全部经桩，桩收到完整 argv）。")

            def idx_of(pred, label):
                hits = [i for i, a in enumerate(argvs) if pred(a)]
                self.assertTrue(
                    hits,
                    f"{tag} RED: 编排序缺「{label}」调用（实得序 = "
                    f"{[_argv_text(a) for a in argvs]}）。")
                return hits[0]

            i_dry = idx_of(
                lambda a: _oc_cmd(a, ["config", "patch"]) and "--dry-run" in a,
                "config patch --dry-run")
            i_formal = idx_of(
                lambda a: _oc_cmd(a, ["config", "patch"])
                and "--dry-run" not in a and "--file" in a,
                "正式 config patch --file")
            i_validate = idx_of(
                lambda a: _oc_cmd(a, ["config", "validate"]), "config validate")
            i_restart = idx_of(
                lambda a: bool(a) and a[0] == "systemctl" and "restart" in a
                and GATEWAY_UNIT in a,
                f"systemctl restart {GATEWAY_UNIT}")
            i_health = idx_of(
                lambda a: _oc_cmd(a, ["health"]), "openclaw health")
            i_nrestarts = idx_of(
                lambda a: any("NRestarts" in x for x in a),
                "NRestarts 崩循环核对")

            self.assertEqual(
                i_dry, 0,
                f"{tag} RED: 首个外部命令不是 config patch --dry-run（实得 "
                f"{_argv_text(argvs[0])!r}）—— 写前 schema 校验必须先于任何写。")
            self.assertEqual(
                [i_dry, i_formal, i_validate, i_restart, i_health, i_nrestarts],
                sorted([i_dry, i_formal, i_validate, i_restart, i_health,
                        i_nrestarts]),
                f"{tag} RED: 编排序违反冻结契约（dry-run → 正式 patch → validate "
                f"→ restart → health → NRestarts；实得序 = "
                f"{[_argv_text(a) for a in argvs]}）。")

            forbidden = [a for a in argvs
                         if _oc_cmd(a, ["plugins"])
                         and a[2:3] in (["install"], ["inspect"], ["doctor"])]
            self.assertEqual(
                forbidden, [],
                f"{tag} RED: 编排出现 plugins install / inspect / doctor 调用 "
                f"（{[_argv_text(a) for a in forbidden]}）—— memory-core 为 "
                "bundled 插件，三步只服务 npm 第三方插件安装与加载核对"
                "（architect 1g 裁决 ③）。")

            # 逐调用 hash 协议：正式 patch 调用及其之前 == 变更前；其后 == 终态
            final_sha = _sha_file(run.cfg)
            for i, record in enumerate(run.records):
                if i <= i_formal:
                    self.assertEqual(
                        record["config_sha256"], run.pre_sha,
                        f"{tag} RED: 第 {i} 次调用（{_argv_text(argvs[i])!r}）时 "
                        "config 内容已被改写 —— 正式 OpenClaw 写入口之前零直写"
                        "（K3 崩因 = 脚本直写绕过 zod → 网关 exit 78 崩循环）。")
                else:
                    self.assertEqual(
                        record["config_sha256"], final_sha,
                        f"{tag} RED: 第 {i} 次调用（{_argv_text(argvs[i])!r}）时 "
                        "config 内容 ≠ 编排终态 —— 正式 patch 之后出现额外改写"
                        "（含任何脚本直写路径）。")

            got, gerr = _load_json(run.cfg)
            self.assertIsNone(gerr, f"{tag} RED: config 不可解析: {gerr}")
            self.assertEqual(
                got, _target_state(run.pre, changes),
                f"{tag} RED: 编排终态 ≠ 声明目标态（经桩的 OpenClaw 写入口消费 "
                "patch 失败或 patch 产物形状不符）。")
            self.assertIs(_json_get(got, DREAMING_PATH), False,
                          f"{tag} RED: 终态 dreaming 叶不是布尔 false。")
            self.assertEqual(
                _json_get(got, TOOLS_ALLOW_PATH), list(UPGRADED_ALLOW),
                f"{tag} RED: 终态工具面 allow 被破坏 —— 升级态须原样保留。")

            backups = _backups(run.td, pattern)
            self.assertTrue(
                backups,
                f"{tag} RED: apply 未建匹配 backup.pattern 的备份 —— 一键回滚"
                "先决缺失。")
            self.assertRegex(
                backups[0].name, TIMESTAMP_RE.pattern,
                f"{tag} RED: 备份名 {backups[0].name!r} 不含 UTC 时间戳"
                "（yyyymmddTHHMMSS）—— 非时间戳备份无法确定先后。")
            self.assertEqual(
                backups[0].read_bytes(), run.pre_bytes,
                f"{tag} RED: 备份内容 ≠ 变更前 config（须为原始 config 快照）。")
        finally:
            run.cleanup()

    def test_dd_apply_failure_stops_and_writes_nothing(self):
        """coverage: DD-A3 真跑承载 · apply 失败即停 — 两个失败点：
        ① dry-run schema 拒 → 其后零正式 patch / validate / restart / health /
        NRestarts；② 正式 patch 失败 → 其后零 validate / restart / health /
        NRestarts。两场景 config 全程不变（逐调用 hash 全 == 变更前、终态字节
        不变）。"""
        tag = "DD-apply-failure"
        for fail_mode, banned_labels in (
                ("fail_dry_run",
                 ("正式 patch", "config validate", "systemctl restart",
                  "openclaw health", "NRestarts")),
                ("fail_patch",
                 ("config validate", "systemctl restart", "openclaw health",
                  "NRestarts"))):
            run = _orchestrate(_fixture_tools_upgraded(), fail_mode=fail_mode)
            try:
                self.assertNotEqual(
                    run.rc, 0,
                    f"{tag} RED: 失败点 {fail_mode} 编排仍 exit 0（任一步失败即停"
                    f"被破）。stdout={run.out!r}")
                self.assertTrue(
                    run.records,
                    f"{tag} RED: 失败场景注入桩零记录（前置）—— 若 apply 在外部"
                    "命令前就 STOP（exit 3），说明 drift-guard 把工具面升级态判成"
                    "漂移：drift_guard 接受集须含 dreaming unset + 工具面升级态。")
                argvs = run.argvs

                def hits(pred):
                    return [i for i, a in enumerate(argvs) if pred(a)]

                if fail_mode == "fail_dry_run":
                    self.assertTrue(
                        hits(lambda a: _oc_cmd(a, ["config", "patch"])
                             and "--dry-run" in a),
                        f"{tag} RED: dry-run 失败场景却无 dry-run 调用（前置）。")
                else:
                    self.assertTrue(
                        hits(lambda a: _oc_cmd(a, ["config", "patch"])
                             and "--dry-run" not in a and "--file" in a),
                        f"{tag} RED: 正式 patch 失败场景却无正式 patch 调用"
                        "（前置）。")

                banned_preds = {
                    "正式 patch": lambda a: _oc_cmd(a, ["config", "patch"])
                    and "--dry-run" not in a and "--file" in a,
                    "config validate": lambda a: _oc_cmd(a, ["config", "validate"]),
                    "systemctl restart": lambda a: bool(a)
                    and a[0] == "systemctl" and "restart" in a,
                    "openclaw health": lambda a: _oc_cmd(a, ["health"]),
                    "NRestarts": lambda a: any("NRestarts" in x for x in a),
                }
                for label in banned_labels:
                    self.assertEqual(
                        hits(banned_preds[label]), [],
                        f"{tag} RED: 失败点 {fail_mode} 之后仍出现「{label}」调用"
                        " —— 任一步失败即停被破（失败序零写）。")

                for i, record in enumerate(run.records):
                    self.assertEqual(
                        record["config_sha256"], run.pre_sha,
                        f"{tag} RED: 失败场景第 {i} 次调用时 config 已被改写 —— "
                        "失败序零写被破（含任何脚本直写路径）。")
                self.assertEqual(
                    run.cfg.read_bytes(), run.pre_bytes,
                    f"{tag} RED: 失败场景终态 config ≠ 变更前（失败序零写被破）。")
            finally:
                run.cleanup()

    def test_dd_apply_source_has_patch_entry_and_no_direct_config_write(self):
        """coverage: DD-A3 真跑承载 · 禁直写（源锚代理）— apply 脚本源码须含
        `config patch` 与 `--dry-run` 字面（写入口纪律），且不得出现直写
        openclaw.json 的代码路径（config 变量 write_text / open(..., 'w')）。"""
        tag = "DD-source"
        self.assertTrue(
            APPLY_SCRIPT.is_file(),
            f"{tag} RED: apply 脚本不在位 —— 冻结契约要求 {APPLY_SCRIPT}。")
        src = _read_text(APPLY_SCRIPT)
        self.assertIn(
            "config patch", src,
            f"{tag} RED: apply 脚本零 OpenClaw 写入口字面（config patch）—— "
            "openclaw.json 只经 OpenClaw 自己的写入口写（K3 崩因：直写绕过 zod "
            "→ 网关 exit 78 崩循环）。")
        self.assertIn(
            "--dry-run", src,
            f"{tag} RED: apply 脚本零 --dry-run 字面 —— 写前必 --dry-run schema "
            "校验（checks.schema === true）。")
        self.assertIn(
            DECL_NAME, src,
            f"{tag} RED: apply 脚本未引用声明工件名 —— 声明面与消费面不一致。")
        self.assertIsNotNone(
            re.search(r"(?m)^\s*(?:import\b[^\n]*\bjson\b|from\s+json\s+import\b)",
                      src),
            f"{tag} RED: apply 脚本未用 stdlib json（冻结契约：零额外依赖）。")
        hits = []
        for lineno, line in enumerate(src.splitlines(), 1):
            for regex in _DIRECT_CONFIG_WRITE_RES:
                if regex.search(line):
                    hits.append((lineno, line.strip()))
        self.assertEqual(
            hits, [],
            f"{tag} RED: apply 脚本出现直写 config 的代码路径 {hits} —— 配置"
            "改动只经注入的外部写入口（config patch）；脚本自写 openclaw.json = "
            "K3 崩因复现（源锚为代理，行为真证见逐调用 hash 协议）。")


# --------------------------------------------------------------------------- #
# rollback · 行为级真跑
# --------------------------------------------------------------------------- #

def _run_rollback_injected(base_args) -> tuple:
    """rollback 注入桩调用（旗标名容差）：先试 plan 口径 --systemctl-bin，
    若 argparse 报未识别参数再试姊妹形态 --openclaw-bin。返回
    (rc, out, err, used_flag)。两者皆不识别 → 原样返回（红）。"""
    stub = None
    if base_args and isinstance(base_args[-1], Path):
        stub = base_args[-1]
    argv = [*base_args[:-1]]
    for flag in ("--systemctl-bin", "--openclaw-bin"):
        rc, out, err = _run_py(ROLLBACK_SCRIPT, [*argv, flag, stub])
        if rc == 2 and "unrecognized arguments" in (err or ""):
            continue
        return rc, out, err, flag
    return rc, out, err, None


class DreamingDisableRollbackAcceptance(unittest.TestCase):

    maxDiff = None

    def _stage(self, td: Path, cur: dict, restored: dict):
        cfg = td / "openclaw.json"
        bak = td / "openclaw.json.bak-kidpal-dreaming-20261025T030000"
        _write_json(cfg, cur)
        _write_json(bak, restored)
        return cfg, bak

    def test_dd_rollback_dry_run_writes_nothing(self):
        """coverage: rollback — `--dry-run` rc 0、零写（config 字节不变）、零
        重启（不注入桩即证明不调外部命令：真 openclaw / systemctl 不在 PATH 时
        任何调用都会让 rc ≠ 0）。"""
        tag = "DD-rollback"
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"{tag} RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        with tempfile.TemporaryDirectory(prefix="dd-rb-dry-") as td:
            td = Path(td)
            cur = _fixture_dreaming_disabled()
            restored = _fixture_tools_upgraded()
            cfg, bak = self._stage(td, cur, restored)
            before = cfg.read_bytes()

            rc, out, err = _run_py(
                ROLLBACK_SCRIPT, ["--config", cfg, "--backup", bak, "--dry-run"])
            self.assertEqual(
                rc, 0,
                f"{tag} RED: rollback --dry-run rc={rc}（应 0）。"
                f"stdout={out!r} stderr={err!r}")
            self.assertEqual(
                cfg.read_bytes(), before,
                f"{tag} RED: --dry-run 改写了 config。")

    def test_dd_rollback_restores_and_validates_before_restart(self):
        """coverage: rollback — 非 dry-run：先还原 backup 内容（config 内容 ==
        备份）→ `openclaw config validate` → 通过才 `systemctl restart
        openclaw-gateway`（注入桩验证调用序：validate 在 restart 之前）；
        validate 失败则不重启并报错（exit 非 0）。"""
        tag = "DD-rollback"
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"{tag} RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        for fail_mode, expect_restart in (("ok", True), ("fail_validate", False)):
            with tempfile.TemporaryDirectory(prefix="dd-rb-") as td:
                td = Path(td)
                cur = _fixture_dreaming_disabled()
                restored = _fixture_tools_upgraded()
                cfg, bak = self._stage(td, cur, restored)
                log = td / "stub-invocations.jsonl"
                stub = td / "stub_ext.py"
                stub.write_text(
                    _STUB_TEMPLATE % {"log": str(log), "cfg": str(cfg),
                                      "fail_mode": fail_mode},
                    encoding="utf-8")

                rc, out, err, used_flag = _run_rollback_injected(
                    [Path("--config"), cfg, Path("--backup"), bak, stub])
                self.assertIsNotNone(
                    used_flag,
                    f"{tag} RED: rollback 无可用的注入桩旗标（--systemctl-bin / "
                    "--openclaw-bin 均不被识别）—— 重启面离线不可真跑。"
                    f"stdout={out!r} stderr={err!r}")
                records = _records(log)
                argvs = [r["argv"] for r in records]
                validated = [i for i, a in enumerate(argvs)
                             if _oc_cmd(a, ["config", "validate"])]
                restarted = [i for i, a in enumerate(argvs)
                             if a and a[0] == "systemctl" and "restart" in a]

                if expect_restart:
                    self.assertEqual(
                        rc, 0,
                        f"{tag} RED: rollback rc={rc}（应 0）。"
                        f"stdout={out!r} stderr={err!r}")
                    got, gerr = _load_json(cfg)
                    self.assertIsNone(gerr,
                                      f"{tag} RED: 还原后 config 不可解析: {gerr}")
                    self.assertEqual(
                        got, restored,
                        f"{tag} RED: 回滚未还原 backup 内容（整档还原语义）。")
                    self.assertTrue(
                        validated,
                        f"{tag} RED: 回滚未调用 openclaw config validate —— "
                        "还原后须先校验再重启。")
                    self.assertTrue(
                        restarted,
                        f"{tag} RED: 回滚未调用 systemctl restart "
                        f"{GATEWAY_UNIT}（重启面缺失）。实得调用序 = "
                        f"{[_argv_text(a) for a in argvs]}")
                    self.assertLess(
                        validated[0], restarted[0],
                        f"{tag} RED: 调用序违反冻结契约：config validate 须在 "
                        f"systemctl restart 之前（实得序 = "
                        f"{[_argv_text(a) for a in argvs]}）。")
                else:
                    self.assertNotEqual(
                        rc, 0,
                        f"{tag} RED: validate 失败场景 rollback 仍 exit 0（应非 0 "
                        f"并报错）。stdout={out!r} stderr={err!r}")
                    self.assertTrue(
                        validated,
                        f"{tag} RED: validate 失败场景却无 config validate 调用"
                        "（前置）。")
                    self.assertEqual(
                        restarted, [],
                        f"{tag} RED: validate 失败后仍 systemctl restart —— "
                        "「校验通过才重启」被破（网关仍跑内存态是其接受形态）。")

    def test_dd_rollback_source_mentions_validate_and_distinction(self):
        """coverage: rollback — 源锚：回滚脚本含 `config validate` 与
        `restart` 字面（重启面承载），且头注显式区分「还原已知合法快照」与
        「禁脚本拼装新目标态」口径（语义锚组任一即可）。"""
        tag = "DD-rollback"
        self.assertTrue(
            ROLLBACK_SCRIPT.is_file(),
            f"{tag} RED: 回滚脚本不在位 —— 冻结契约要求 {ROLLBACK_SCRIPT}。")
        src = _read_text(ROLLBACK_SCRIPT)
        self.assertIn(
            "config validate", src,
            f"{tag} RED: 回滚脚本零 config validate 字面 —— 还原后先校验再重启"
            "的序链无源锚。")
        self.assertIn(
            "restart", src,
            f"{tag} RED: 回滚脚本零 restart 字面 —— 重启面无承载。")
        anchor_groups = (
            ("还原已知合法快照", ("已知合法", "合法快照", "合法快照还原", "备份快照")),
            ("禁脚本直写新目标态", ("直写", "绕过", "拼装", "新目标态")),
        )
        for what, group in anchor_groups:
            self.assertTrue(
                any(w in src for w in group),
                f"{tag} RED: 回滚脚本头注缺口径「{what}」（任一即可：{group}）"
                "—— 须显式区分「还原 apply 前快照」与「脚本直写新目标态绕过 zod」"
                "（K3 崩因）。")


# --------------------------------------------------------------------------- #
# manifest / runbook / 零 secret · 部署面
# --------------------------------------------------------------------------- #

class DreamingDisableDeploySurfaceAcceptance(unittest.TestCase):

    maxDiff = None

    def test_dd_manifest_registers_three_assets(self):
        """coverage: DD-A1..A5 部署面 — manifest.yaml 对本包登记**恰 3 条**
        （声明 / apply / rollback），src 在仓内存在、dst 在
        /opt/kidpal/openclaw-patch/261025-openclaw-dreaming-disable/ 下；
        三字段 owner=deploy.sh / verify=md5 / restart=none（部署单一事实源，
        261013 纪律）。"""
        tag = "DD-manifest"
        self.assertTrue(
            _HAS_YAML or self.fail(
                f"{tag} RED(infra): PyYAML 不在位——manifest 解析前置缺失。"))
        self.assertTrue(MANIFEST_YAML.is_file(), f"{tag} RED: manifest.yaml 不在场。")
        try:
            doc = yaml.safe_load(MANIFEST_YAML.read_text(encoding="utf-8"))
        except Exception as e:  # noqa: BLE001 — 解析错误是 RED，不是 crash
            self.fail(f"{tag} RED: manifest.yaml 解析失败: {e!r}")
        entries = [
            e for e in (doc or {}).get("entries", []) or []
            if isinstance(e, dict)
            and WORKBENCH_TOKEN in (str(e.get("src") or "")
                                    + str(e.get("dst") or ""))
        ]
        self.assertEqual(
            len(entries), 3,
            f"{tag} RED: manifest 对 {WORKBENCH_TOKEN} 的登记条目 = {len(entries)}，"
            "须恰 3 —— 声明 / apply / rollback 三件各一条（新资产必须登记并接入"
            "verify-deploy.sh）。")
        expected_srcs = {
            f"{WORKBENCH_TOKEN}/{DECL_NAME}",
            f"{WORKBENCH_TOKEN}/{APPLY_NAME}",
            f"{WORKBENCH_TOKEN}/{ROLLBACK_NAME}",
        }
        got_srcs = set()
        for entry in entries:
            src = str(entry.get("src") or "").replace("\\", "/")
            got_srcs.add("/".join(src.split("/")[-2:]))
        self.assertEqual(
            got_srcs, expected_srcs,
            f"{tag} RED: 登记 src 集合 = {sorted(got_srcs)}，须恰 "
            f"{sorted(expected_srcs)}（三件齐、零多余）。")
        missing = [str(e.get("src")) for e in entries
                   if not (REPO_ROOT / str(e.get("src") or "")).is_file()]
        self.assertEqual(missing, [], f"{tag} RED: 登记的仓内源缺失: {missing}。")
        bad_dst = [str(e.get("dst") or "") for e in entries
                   if not str(e.get("dst") or "").startswith(DEPLOY_DST_PREFIX + "/")]
        self.assertEqual(
            bad_dst, [],
            f"{tag} RED: 登记条目 dst 越出消费位 {DEPLOY_DST_PREFIX}/: {bad_dst}。")
        bad_meta = [
            (str(e.get("src")), e.get("owner"), e.get("verify"), e.get("restart"))
            for e in entries
            if str(e.get("owner")) != "deploy.sh"
            or str(e.get("verify")) != "md5"
            or str(e.get("restart")) != "none"
        ]
        self.assertEqual(
            bad_meta, [],
            f"{tag} RED: 登记条目字段不符 owner=deploy.sh / verify=md5 / "
            f"restart=none: {bad_meta}")

    def test_dd_runbook_upgrade_checklist_anchors(self):
        """coverage: DD-A1..A5 部署面 — DEPLOY-RUNBOOK.md 升级检查清单语义锚组
        在场：dreaming-narrative 家族 + cron: / heartbeat: / subagent: 三家族 +
        「必须为空」或等价口径（同窗口内聚）+ 引 debt 条目
        TECH-261025-dreaming-capture-pollutes-honcho。"""
        tag = "DD-runbook"
        self.assertTrue(RUNBOOK.is_file(), f"{tag} RED: {RUNBOOK} 不在场。")
        body = _read_text(RUNBOOK)
        self.assertTrue(body.strip(), f"{tag} RED: runbook 为空。")
        idx = body.find("dreaming-narrative")
        self.assertGreaterEqual(
            idx, 0,
            f"{tag} RED: runbook 无 dreaming-narrative 家族盘点行 —— human 四裁决 "
            "④「部署 runbook 升级清单钉一行」未落。")
        window = body[max(0, idx - 900): idx + 900]
        for token in ("cron:", "heartbeat:", "subagent:"):
            self.assertIn(
                token, window,
                f"{tag} RED: 升级清单窗口缺家族 token {token!r} —— 会话家族盘点须"
                "同时覆盖 dreaming-narrative-* / cron:* / heartbeat:* / subagent:*"
                "（后三条 = isSystemRun 同类结构缺口，debt 条目「相邻同类缺口」）。")
        self.assertIsNotNone(
            re.search(r"(必须|须|应|需|要)为空|为空|不得出现|零出现", window),
            f"{tag} RED: 升级清单窗口缺「必须为空」或等价口径 —— 盘点没有判据"
            "（违反 = 采集策略漂移）。")
        self.assertIn(
            DEBT_ID, body,
            f"{tag} RED: runbook 未引 debt 条目 {DEBT_ID!r} —— 升级清单须指回"
            "债务来源（human 裁决 ④：先记 debt + runbook 钉一行）。")

    def test_dd_no_openclaw_json_in_repo_and_no_secret(self):
        """coverage: DD-A5 部署面 — openclaw.json 整文件零入仓（含 gateway
        凭据）；声明工件零 secret（无 gateway.token 字样、无 token 键、无
        sk- 长键）。仓根由 __file__ 推导，不依赖调用 cwd。"""
        tag = "DD-secret"
        leaked = [str(p) for p in SERVER.rglob("openclaw.json") if p.is_file()]
        self.assertEqual(
            leaked, [],
            f"{tag} RED: 仓内出现 openclaw.json（含 gateway 凭据）—— 整文件零"
            f"入仓，只入无 secret 声明。实得：{leaked}")
        decl_text = _read_text(DECL_PATH)
        self.assertTrue(
            decl_text.strip(),
            f"{tag} RED: 声明工件不在位/为空（前置）—— 零 secret 负锚无承载面。")
        self.assertNotIn(
            "gateway.token", decl_text,
            f"{tag} RED: 声明工件出现 gateway.token 字样 —— 声明面不得承载 secret。")
        self.assertIsNone(
            re.search(r'"token"\s*:', decl_text),
            f"{tag} RED: 声明工件出现 token 键 —— 声明面不得承载 secret。")
        self.assertEqual(
            SECRET_KEY_RE.findall(decl_text), [],
            f"{tag} RED: 声明工件内联 secret（sk- 长键）。")


if __name__ == "__main__":  # standalone: python 本文件
    unittest.main(verbosity=2)
