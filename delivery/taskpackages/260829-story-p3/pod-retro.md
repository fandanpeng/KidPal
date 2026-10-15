# Pod Retro · 260829-story-p3（第一人称热记忆）

> planner 收尾相位手写、integrator closeout 之后 / supervisor gate 之前。只记我在场的主观过程体感与下次改进；tokens/闸轮次/verdict/测试计数以机器制品（verifier-evidence、pod-events、Review Packet）为权威，此处不复述。

## 整体体感

这是我在 kidpal-mvp 里跑得最「顺又不顺」的一包。顺在主线：闸 A 一次判 ④、acceptance 一次冻结、builder 一次转绿、verifier 零 fixer 轮——双面 14+5 个冻结契约函数 + prompt v3 + 设备构建，一遍过没烧修复预算，这在该 workstream 里算罕见。不顺在**边边角角全是真问题**：验收文件自身两处缺陷、一处 main 既有破口、一次跨包 forward-bump 撞车、一条我自己包的 live 齿结构性假绿。每一个都不是「实现没做出来」，而是「声称绿的东西到底绿没绿」——这包把我大量精力花在了**证伪**上，值。

## 卡点与绕轮（主观判读）

- **acceptance-author 头一次 stall 是本轮最阴的一刀**。它烧了将近 19 分钟、几十次工具调用，把 spec/契约/先例读了个遍，然后回一句「Now I have all the inputs. Let me write the C-side file」就停了——磁盘零落件。若我信了那句收尾文本、直接进 finalize，整个 oracle 就是空的。**教训固化**：子 agent 的「完成」文本一律不算数，先 `git status`/探盘再说。续跑同实例（不重派）是对的，省了它已读的全部上下文；第二次交付质量极高。
- **`git checkout` 还原对我新建的实现文件静默 no-op**。变异实验我用 `git checkout -- story_page.c` 复原，但它压根没进过 HEAD（builder 产物只提了验收检查点、实现未提），checkout 对未跟踪文件什么都不做，sha256 当场漂了。所幸我核了哈希、改用内存逆变换精确复原回基线。**通用坑**：任何「改后即还原」的取证动作，对象若是本会话新产且未 commit 的文件，`git checkout` 不可依赖，得逆变换或先 commit 基线。已记 friction。
- **mutation 那步被 auto-mode 分类器拦了**，判我「不可逆破坏 builder 产物」。它看不到「我下一步就 checkout」的意图，拦得其实合理。停下来问你要写权限、你选了主线程直跑，我才继续——这一步没自作聪明绕开，对了。
- **两处「结构闸看着绿、真跑才见红」让我对绿保持警惕**：verifier 的 unit/acceptance 两闸按设计根本不跑别的包的验收（executor-skipped、不担保实机），固件 build 闸因本仓走 venv+wrapper 而恒 degraded。也就是说**机械 verifier 全绿 ≠ 真绿**。我于是自己从仓库根跑了 host ELF、跑了全量 server 套、第一方真跑了 idf.py build——正是这几个「多跑的一步」抓到了跨包回归和设备破口。

## 帮倒忙 / 该收紧的规则与钩子

- 起包格式期望前置（DEV-GOVERNANCE 五条）这次在**变异证据**上咬了我一轮：portfolio 起草的 taskpackage 不带 mutationEvidence 骨架，scoped 闸红一轮我才回补。是这条清单一号条文的既成复现（G/WS/session-logic 都栽过）。不算钩子的错，是拆包侧一直没带骨架的老毛病，已撞第 N 次。
- 我一度想用 `changes-index --upsert` 给本包钉 dev 行，报「缺写槽 token」——那 upsert 是 arch 专用的，dev 行走全量重建。**文档/习惯容易把「状态转移时跑 --upsert」误读成通用**，下次记得 dev=全量重建、arch=--upsert。

## 人工纠偏体感

- 只有两处真停下来问了你：① 主线程能不能直接改实现跑变异（你放行）；② prompt v2→v3 撞红 char-stroke 的冻结字面 green-guard，两条 oracle 版本字面互斥、且要动已 merged 包的件（你裁「重指版本无关」）。两处都**该**问：一个触「主 AI 不直改实现面」的字面精神，一个触「冻结验收不得改 + 跨包」。其余全自主推进没惊动你。
- integrator 反过来给我上了一课：它五轴 review 逮出**我自己包**的 live 齿 `test_live_story_roundtrip_evidence` 语料就是 closeout 目录本身——等于「如实描述本包就算 live 通过」。这种 vacuous 是我派 acceptance-author 写、builder 兑现、verifier 结构闸放行，一路没人按 live 跑过（它机械 skip），所以藏得住。integrator 两次实测复现假绿才逼出来。**这提醒我**：殿后 skip 的齿不是一等公民，但「形状不成立的证据齿」终究是自家的债——当场 tighten 了（语料收窄 live-evidence/ + 要结构化事实），并给 9b 留了核销三件套投递清单，免得 9b 相位撞红才发现要补投。

## 下次改进（给同 workstream 后继包，尤其 9b/U）

1. **forward-bump 版本化制品前先 grep 谁把版本名钉进了断言**。char-stroke 那条字面守卫本就是给 v2 写的，v3 一来必撞；schedule-p2 的 P2-A6、voice_roundtrip 的 v1 针是同款，已并证进 ARQ，9b/后续包 launch 前该先扫一眼。
2. **设备构建闸恒 degraded 是本仓系统性盲区**（门探裸 idf.py、通路在 wrapper）。在它按 ARQ-260908 修好前，**每个动 device-only 文件（mcp_server.cc 这类 host 不编的）的包，务必主线程第一方跑一次 idf.py build**，别信那枚 degraded 绿。char-stroke 就是信了它、把设备破口合进了 main。
3. 子 agent「完成文本不可信、先验盘」——尤其 stall 检测，看挂钟 vs 落盘。
4. 未 commit 产物做破坏性取证，还原用逆变换、别用 checkout。

## escalation themes（按 eventId 交叉引用 pod-events）

- `775212aa`→`d28f127b`：acceptance harness 缺陷（_ts 求值期崩），human 未惊动（acceptance-author 职权内 tighten，断言零变）。
- `80deedcb`→`85206e1c` + human `c54bbcde`：跨包版本字面 green-guard 撞车，唯一升级 human 的包治理项，重指版本无关。
- `221bb868`：human 授权主线程直跑变异（写权限）。
- friction `0ebaf461`（acceptance stall）、`c8055cf7`（checkout 未跟踪 no-op）。
- 非升级、当场处置：integrator F3（live 齿 prose 假绿）tighten；F4/F6/F7 留根 9b（见 taskpackage notes + Review Packet §9）。
