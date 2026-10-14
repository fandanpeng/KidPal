---
id: ARQ-260906-shell-scripts-lf-gitattributes
title: "仓内 shell 脚本（如 `espidf_wsl.sh`）以 CRLF 入库，WSL 下 bash 直接解析崩（`$'\r': command not found`），而 host 验收依赖 WSL linux target"
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-06
closed: 2026-09-06
closedBy: ""
paths: []
source: ""
---

仓根 `.gitattributes` 新增 `*.sh text eol=lf`（checkout 侧消灭 CRLF）；五件 `.sh` 一次性归一为工作树 LF（rm + checkout 强制重写路径），`git ls-files --eol` 五件全 `i/lf w/lf` + WSL live `cat -A` 实证行尾无 `\r`（[260906-gov-review-arq-governance-batch](../changes/arch/260906-gov-review-arq-governance-batch/)，T7）。
