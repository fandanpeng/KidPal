# pod-retro · 261017-prompt-v11-schedule-recall（第一人称热记忆）

> 标签：第一人称热记忆——planner 全程在场的主观过程体感，2026-10-18 收尾相位写（integrator closeout 后、supervisor 机械闸前）。不复述、不冠权威：tokens / gates / rounds / verdict / test results 以 machine artifacts / pod-events / verifier-evidence 为权威，本文不载数值。

## 本次跑法体感

这是我近期跑得最「有先例可抄」的一包：同域上一包（v10 实时事实回合）把 prompt 版本化 bump、四面接线、版本无关守卫、structural live-manual 豁免、ECS 人工部署门这一整套路标都留好了；reviewed plan 又把四段改法（A/B/C/D）和风险写得很细。结果是整包几乎没有方向性返工——唯一一次 final 红不是实现问题，而是我这一侧收口顺序的引用漂移（见下）。

## 卡点与绕轮原因的主观判断

1. **check-launch 的 base-ref 语义与启动卡措辞有落差**。启动卡说「intake 首提交后即消」，但机械上 `--base-ref main` 检的是产物是否可达 main；Pod 分支首提交后只对 `dev/pod-m2` 可达。我没有为了「看起来消了」去改判或跳过，而是把两条 ref 的实测结果都写进 TaskPackage notes：分支可达 = 绿，main 可达 = 等 merge-driver。体感上这是启动卡一句 shorthand 与机械语义之间的缝，值得在后续启动卡里改成「首提交后对承载分支可达，合并后对 main 可达」，省掉下一任 planner 的自我怀疑。
2. **验收载体比 plan manifest 多了一件**。plan 的 changeManifest 只列了两个既有验收文件，但 §C 的 skills 验收要点需要独立离线契约载体；我判断「测试载体落位 ≠ 实现面扩单」，新增了第三个 acceptance 文件并在 manifest/notes 里写明理由。这步做得对，但过程里有一瞬间犹豫——如果 plan 起包时就把「验收载体预告」列出来，这类犹豫本可以没有。
3. **final 轮的 docs-sync 红是收口顺序问题，不是代码问题**。integrator 先把两条 debt 翻 closed，`debt-render.mjs` 按「位置 = 状态纯函数」自动把条目搬进 `_archive/2026/`，而 TaskPackage 的 docRefs 仍指旧顶层路径——于是 final 红在 missingDocs。integrator 用引用精度修正（改指归档现址、保留 origin.upstreamRef 历史事实）修掉并用新一轮复验，处置是对的；但这个「归档搬迁 → 引用漂移」的坑对任何消费 debt 条目的包都会复现，值得写进起包自查：**凡本包要结项 debt，docRefs / sourceRefs 就要预留「结项后归位到 _archive」的路径形态**。
4. **host 无 jinja2**。模板渲染只能做等价代理（模板本身无控制流），真实 `Template.render` 留给部署轮。这是诚实边界，不是缺陷；但它提醒我：凡是「引擎侧渲染」的验收，host 面永远只能代理，live 门不可省。

## 别扭的 acceptance / 钩子体感

- docs-sync 闸要求每个 docRef 正文里出现验收 id，否则要靠 `docsSyncReason` 兜底。对「只改指针与债务结项」的包，这个「文档必须回引验收 id」的默认语义不贴——我在 intake 阶段就预写了 `docsSyncReason`，避免了一轮必然的红。规则本身没错，但它默认假设「文档面会写行为」，对纯引用类文档不成立。
- acceptance-author 为 PV-A2 做的口径判定（LF 归一 canonical blob sha256，而不是工作树 raw bytes）非常关键：Windows CRLF 检出会让字面 raw 口径随环境假红。这个判断如果留给 verifier 轮去发现，就会白烧一轮。先例文件里其实已有同类注记，说明「先例密度」再次决定 Lite 包的流畅度。

## 人工纠偏体感

本轮 Pod 内零人工纠偏——human 的三处裁决（单包 Lite、arch 面迁出、start card 两个人工位）在起包前已经钉死，spine 与 plan 都写清楚了。我没有在 Pod 内向外提问一次。收尾停在两处 human 门（ECS 部署 + 真机轮）是按启动卡执行的停等，不是卡点。

## escalation themes（post-cutoff，交叉引用 pod-events）

- 本轮零 `agent-escalation` 事件：acceptance-author / builder / integrator 三次派发均一次交付，无 Red Flag、无 out-of-scope。
- 派发账见 pod-events 的 `dispatch-completed` 事件（acceptance / build / integrate 三条）；`round-outcome` 按 post-cutoff 规则自动 DROP，verifier evidence 为权威。
- 待人到门（非 Pod 内 escalation）：① ECS 部署轮（overlay 真再生 + verify-deploy 三时机 + skills 首次真 apply）；② PV-A8 真机持板轮；③ merge 后独立 architect 会话翻 `docs/architecture.md` 两处指针。若 supervisor 机械闸按 all-mock 重算 riskTier 并向严升级，也属既有门的人到面，不另立 escalation。

## 下次改进

1. 启动卡模板：把 check-launch 的「承载分支可达 / main 可达」两段式写清，避免 shorthand 误导。
2. plan 模板：验收要点若需要新建载体（如声明工件契约件），在 changeManifest 里直接预告该路径，减少 intake 期「这算不算扩单」的判断成本。
3. 起包自查加一条：本包要结项 debt 时，docRefs/sourceRefs 预留 `_archive/<年>/` 归位形态，或至少在 final 前先跑一次 `debt-render.mjs --check`。
4. 部署面包一律在 review-packet 的未覆盖面段固定写「引擎真实渲染 vs host 代理」边界——本包 integrator 做到了，这个形状值得固化。
