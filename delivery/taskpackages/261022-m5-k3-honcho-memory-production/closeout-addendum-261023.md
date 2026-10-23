# closeout addendum · 261022-m5-k3-honcho-memory-production（2026-10-23，批 B 收口记账）

> append-only 增补，不改本包冻结证据。依据 = 261023-arch-review-memory-wiring-verified-behavior PROPOSAL §3「历史证据」行与批 B 收口义务。

本包冻结验收与制品中「device 级 peer / peer 键不另造」表述（K3-E2/E6 判法、声明工件 peerPrefix 锚、live 核对锚）经部署轮实测修正口径：**设备身份载体 = session key 家族 `agent:kidpal:openai-user:<device_id>`（session metadata），不是 Honcho peer**——peer = 家庭单 `owner`（+ agent 侧 `agent-kidpal`），换设备 = 新 session 家族、画像不隔离。原判法把 session key 当 peer 键（debt 证据 7 缩略写法之误）。规格权威 = arch §6.7（261023 批改后版）+ ADR-016；实现面换代 = 批 B `261023-honcho-memory-wiring-restoration`（declaration 5 键 + 验收锚换代 + carrier 弃用）。本包 host 面验收在原判法下仍成立（session key 家族锚逐字保留于换代件），live 核对锚（K3-E7）按新口径判读。
