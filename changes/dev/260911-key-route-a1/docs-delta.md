# docs-delta — 260911-key-route-a1（PKG-1 键位路线落地 · KEY(GPIO18) 三手势接线 + 全局路由 dispatch + BOOT 回归翻页）

> integrator docs-sync 结果清单 · 2026-09-11 · 分支 `dev/260911-key-route-a1`（base = main `d087fb7`）。
> 性质：固件板级接线 + host 验收注册包（subsystem=firmware，rigor=full，riskTier=high，gateProfile=host-teeth/device-round/state-machine，mergeOrder 18）。
> `docsSyncRequired=false`。**核心结论：canonical 面（`docs/contracts/**`、`docs/spec.md`）与全部 docs 面零改动**；
> 本包实现侧注记随代码注释与 changes/dev closeout 制品承载。

## A. docsSyncRequired=false 判定（integrator，承 taskpackage.docsSyncReason）

本包为固件板级接线 + host 验收注册（`config.h` / `waveshare-s3-rlcd-4.2.cc` / `host_app/main/CMakeLists.txt`），零 docs 面改动：

- **spec §4.0/§5.3 零修订**：OQ4=A 已裁（补接 KEY 落地 spec 两键分工，spec 无需改，见 `.planning/260911-display-interaction-sovereignty/PROPOSAL.md §2.4/§3`）；冻结齿 canary 钉住（`key_route_a1.acceptance.c` test 5 tooth 2 检测 `docs/spec.md` 与 `docs/contracts/interface-contract.md` 仍在）。
- **契约面只读**：本包对契约零消费改动（`contractRefs=["docs/contracts/interface-contract.md"]` 仅为来源锚引用）；`contract-lint` 闸 r2/r3 `ok:true`（闸对象 = interface-contract.md）。
- **无新增约定文档面**：无新 view、无字段增删、无页面/协议语义变化——本包只做「接线 + dispatch + BOOT 去对话化」的机制落地，语义权威全部落在既有冻结纯逻辑（`key_gesture_parser.h` 全局路由表，包 3 session-logic 冻结）。

## B. 实现侧注记承载处（本包未改 docs 面，注记随实现代码注释）

| 承载处 | 注记 | 消费方 |
|---|---|---|
| `firmware/main/boards/waveshare-s3-rlcd-4.2/config.h` | `KEY_BUTTON_GPIO = GPIO_NUM_18` 定义旁注释：两键分工不抢占 BOOT 脚 + 有效电平低（GP18_Active 0，KR-A5 真机确认后回填/冻结 `active_high=false`） | 板级实现面自注释 |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` | `keya_dispatch_global_action` 头注释（冻结路由输出→应用动作 dispatch 的**单一权威**消费点，防 BOOT≈KEY 假绿）+ `key_button_` 构造参数注释（消抖复用 180 钉死）+ BOOT 单击去对话化 / BOOT 双击过渡移除的接缝解释 | 板级实现面自注释 |
| `firmware/main/test/key_route_a1.acceptance.c` 头 | 接缝契约（Seam contract pinned by this file [1]-[5]）+ 红基线双通道判读口径 | 冻结验收件（不可改的机制齿） |
| `changes/dev/260911-key-route-a1/device-round-record.md` | KR-A5 真机轮（烧录事实 + 日志↔观察互证 + active_high 回填 + HWV-A2 顺带 + 两观察项归 PKG-2） | closeout human 证据 |

## C. 明确没动的面（及为什么）

1. **`docs/spec.md` 零改动**：OQ4=A 裁定 spec 零修订（`taskpackage.decisionCoverage` 条目「SPEC-1-本批不触发」`landsIn: exitCriteria:KR-E2`）。实现中若发现与 spec §4.0/§5.3 冲突走逃生门（Track B spec-lane），本包全程零触发（KR-E3）。
2. **`docs/contracts/**` 零改动**：只源锚引用 interface-contract.md，不改契约文字。`git diff --name-only` 对 `docs/` 全空（见 §D 机器核据）。
3. **页内特例零触碰留根**：P2 问课程 / P3 光标+双击进阅读 / P3 念给我听 / P4 播笔顺 / 提醒弹窗 KEY 关闭 = A2 随后另立项（`decisionCoverage` 条目「OQ4-A2-页内特例随后」`landsIn: exitCriteria:KR-E1`）；冻结齿 test 4 tooth 2 只守 BOOT 长按故事翻屏一项板级接缝。

## D. 机器可核据（integrator 本相位实跑）

```
git diff --name-only d087fb7..HEAD -- docs/  → 空输出（canonical docs 零触碰，见交付报告）
contract-lint 闸 r2/r3 ok:true             → 闸对象 docs/contracts/interface-contract.md
node .claude/scripts/closeout-check.mjs --update-index  → 见 review-packet §Closeout（本相位实跑）
```

> 本包 docs-delta 结论 = **零 docs 面改动**，与 `docsSyncRequired=false` 一致，无遗留同步项。
> 唯一「观察项」（渲染节点循环序 + 语音 active 期 HOME 让位）已登记在 review-packet §观察项并归 PKG-2 归一屏参考，不构成本包的 spec/契约漂移。