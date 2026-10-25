---
changeId: 261024-memory-recall-tool-enable
title: 记忆回忆回合启用——放行 honcho_search_conclusions(四项→五项)+ AGENTS.md 记忆回忆纪律
rigor: lite
riskTier: high
sourceType: arch-review
reviewed-by: architect(k4-recall-architecture-20261024.md §B 选项 3)+ human 2026-10-24 四裁决
date: 2026-10-24
gateProfile: ["security"]
dependsOn: ["261022-m5-k4-history-corpus-backfill", "261024-m5-k4b-corpus-parser-v2"]
mergeOrder: 56
---

# 背景(live 实证,architect 分析件 §0)

- backfill 344 条 owner 自我结论已可检索(实测「王一诺是谁」第一条即 9-30 原句);被动注入窗口只覆盖 10-13 至 10-24,历史事实在窗口外,重灌也救不了(窗口仍是 recent-100)。
- 模型不查 = 三重锁:①tools.allow 四项白名单滤掉插件 5 个 honcho_* 工具(日志实证;OQ-3 反转:allow 管得住插件工具);②AGENTS.md L11「先查一下是违规」+ L90「想不起来就说记不清了」;③工具不可用时插件不注入工具指引。
- 本批 = P0(选项 3):放行 **honcho_search_conclusions 一个**(不放 honcho_search_messages——原始消息面含家长代述/流程内容),AGENTS.md 增「记忆回忆回合」纪律。

# 实现面三件

1. **声明工件**(261022-openclaw-kidpal-tools):allow 四项→五项;drift_guard 扩「升级基准」——现声明 expect unset,live 已四项覆写,不扩基准则 apply STOP exit 3;**apply 增 `--gen-patch`**(批 B `apply_honcho_plugin.py` 先例:patch 生成器 + `openclaw config patch --dry-run` 校验 + `--file` 落,零直写路径保留),live 应用走 OpenClaw 自有写入口。
2. **AGENTS.md**:回合路由表新增「记忆回忆回合」行(课表问答之外第二个被允许用工具的对话回合;与孩子对话回合禁工具关系显式写清);记忆节保留原句「你不需要写任何文件、也不调用任何工具去记」(工具不用于「记」仍真)+ 一句检索澄清。
3. **验收**:K1-E4 锚换代(四项→五项,显式留痕)+ 新检索回合/工具可用性/gen-patch/drift 基准断言。

# 锚兼容红线(三方既有冻结件必须继续绿)

- K1-E1 回合路由五行全锚(加行 additive 不动既有行);
- K3_P1_AGENTS_ANCHORS 七串逐字(含「回合类型与纪律路由」heading);
- 261023 A3 记忆节正锚四串(不需要写任何文件 / **不调用任何工具** / 记不清 / 绝不编造)+ 对象锁定行逐字;
- K1-E4 换代须显式留痕(不动其余 K1-E2/E3/E5 锚)。

# 红线

- 不进 docs/contracts、不碰插件本体、不碰 Honcho 栈、零数据迁移(不再重灌)、zero 新增出网。
- live 面(openclaw.json 实应用 + 真机「王一诺是谁」+ 日志工具调用)归主线程经 human 授权;Pod 产命令单。

# demo 证据形态

host:验收改代后全量套件绿(基线 340)+ gen-patch 产物断言;live:主线程按命令单 patch --dry-run → --file → 真机问答 + 日志工具调用实证。
