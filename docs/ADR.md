# ADR · 架构决策记录（Architecture Decision Log）

> 性质：架构决策的长期留档——每条记「选了什么 / 为什么 / 放弃了哪些备选」。区别于 `changes/**`（过程证据，按 change 归档）与 `docs/notes/`（经验与策略记录）：ADR 面向「拍板过就不该丢」的决策本体，新会话 / 新成员不考古即可查。
> 落点凭据：human 2026-09-07 裁「放 docs 根目录、和 spec 同级」（OQ3，260907-gov-review-arq-governance-batch-2 T7）——单文件平铺，承本仓单文件 markdown 注册表惯例（对偶 ARCH-REVIEW-QUEUE.md / DEV-GOVERNANCE.md）。
> 写入者：integrator（dev-loop 收口落新增条目）+ 主线程（治理批 landing 落条目）。**append-only：不覆盖、不改写既有条目**；推翻旧决策 = 新增一条并注明 supersedes ADR-XXX。
> 条目格式：每条 = `## ADR-NNN · <决策一句话>`（NNN 三位递增）+ 固定小节：**日期** / **决策** / **理由** / **备选与放弃原因** / **关联**（change-id 或制品锚）。

（尚无条目——存量散落的决策不做考古搬迁，自 260907 批 landing 起新决策按本格式登记。）

## ADR-001 · 版本化部署制品的接线不变式：渲染面 == 声明面 == 被测面，且守卫必须版本无关

- **日期**：2026-09-08（TaskPackage `260829-story-p3` 收口；决策本体由 human 于同批裁定，pod-events 决策事件 `c54bbcde-582f-42c2-a337-5260f890a799`，升级单 `80deedcb-1ea8-4550-b1fa-f20c2f0376d7`）
- **决策**：任何以 `.vN` 文件形态版本化落盘的部署制品（当前实例：`server/deploy/openclaw-system-prompt.vN.md`），其「已接线」类冻结守卫必须满足两条：① 守卫从**结构化指向**解析出「当前被接线的那一版」，而不是把版本字面文件名写进断言；② 解析出的渲染面（`apply-xiaozhi-overlay.sh` 的 `PROMPT_SRC`）与声明面（`provider.config.yaml` 的 `system_prompt.{artifact,source,deploy_target}`）必须**集合相等**，且该版文件确实落盘非空、正文逐字携带历史各包的语义锚点。散文提及与前瞻注记**不算**接线。
- **理由**：本包把 prompt 从 v2 forward-bump 到 v3，撞上已 merged 包 char-stroke 的字面断言（要求 `.v2` 被引用）由绿转红——而 v3 逐字继承了 char C1 段落（实测锚点 13 命中 + v2 基座 32 命中），行为完好、只有版本字样变了。两条冻结 oracle 在同一 deploy 面上互斥，不可能同时为真，只能靠一次 human ruling 解开。若沿用字面耦合形状，v4/v5 及每个动版本化制品的包都会复发同款假红 + 同款升级。更坏的一面是「反向失效」：把语料取成整个目录全文 grep 版本字面的守卫（`test_schedule_p2.acceptance.py:583-586` 现状）在 bump 后**仍绿**，因为它靠被引用文件自己的头注和无关散文维持——守卫声称检验「渲染源已切到 vN」而实际什么都没检验，谎报比报红更贵。本不变式的落点是「什么被生产所用」这一事实本身，而非某个版本号字符串。
- **备选与放弃原因**：① 每次 bump 由后继包把前序包的版本字面守卫逐条改指新版——放弃：改已 merged 包的冻结 oracle 需 human 逐次签字（本包即走此路，成本已实付一次），且改的过程正是弱化点；② 前序包只钉「vN 文件存在 + 内容锚点」、完全不钉接线——放弃：会丢掉原守卫真正防的缺陷（prompt 落盘成孤儿文件，overlay 渲染静默 WARN，生产用旧版），实测该缺陷是 char-stroke E3 立齿的初衷；③ 不做版本化、单一 `openclaw-system-prompt.md` 原地改——放弃：丢掉「历史可 diff / 一版一包」的追溯面（v1→v2→v3 演进链是 live 延迟收敛等结论的证据来源，见 char-stroke packet §4）；④ 让 bump 撞红即 fail-closed 停并升级 human 每次裁决——放弃：等于把复发成本永久化，且升级噪声会稀释真跨系统变更的注意力（对偶 ARQ-260907-escalation-cross-system-overfire 的教训）。
- **关联**：`docs/ARCH-REVIEW-QUEUE.md` ARQ-260908-versioned-guard-version-literal-coupling（把本不变式写入 acceptance-author 撰写约定 + 是否设机检门 = 待裁的落地动作，owner=architect+acceptance-author+human）；范例形状 = `server/acceptance/test_story_p3.acceptance.py::test_prompt_carrier_v3_landed_and_wired`（版本 ≥N 容忍 + 两面同指）与 `server/acceptance/test_char_stroke.acceptance.py::test_e3_prompt_v2_wired_into_deploy`（版本无关重指后形态，含两面相等与内容锚点）；载体声明 = `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER`；现状与漂移清单 = `server/deploy/README.md`「OpenClaw system prompt 版本化接线（现状 = v3）」节；过程证据 = `changes/dev/260829-story-p3/review-packet.md` §6。
