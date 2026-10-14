# docs-delta — TaskPackage 260929-char-window-v5-trim（subsystem: firmware/main/display，查字窗 v5 视觉收敛）

> 读者是人（supervisor / human / 后续 wrap-up）。本包 docsOwner = integrator，本文记录
> 「docs-sync 判定：零 docs 改动」的对账结论，docsSyncRequired=false。

## 1. 变更摘要

查字窗 v5 视觉收敛（requirements v1.60 S-F3.2-AC1 四行口径；依据 = human 2026-09-29
真机轮裁定，口径收敛非缺陷修复）四件：①右栏底部装饰星整删（`charpage_art_star_dsc()`
三静态 + 整函数 + show() 消费块干净删除，`ic_star_22.bin` 转孤儿账面注记零删）；
②组词行整删（words 解析/join/拼装整删，`info_o.words` 传空串满足 card_build 非 NULL，
`kidpal_char_card.words` 成员保留、char_stroke.c 冻结面零触碰）——右栏口径收敛四行
（拼音带声调 / 共 N 画 / 部首 / 释义）；③米字引导线灰度 0x99/0xbb 改纯黑点线（RLCD
阈值化后灰度不可见、真机呈空斜框）+ 画布自绘灰边框撤除（重烘框资产接管外圈，避免
双框）；④cardlabel 让位 x 228 → 244、width 160 → 144，清出重烘圆角框右缘 235。外框
`frm_236_236.bin` 重烘为正圆角矩形（gen_assets.mjs HAND_R 八值不对称改对称 14px，
v5 设计稿 .rbox 口径），simulator 中间母版（assets.c / png / 9slice×8）与 report
snapshots×2 同步刷新，spiffs 其余 46 枚 bin 零字节漂移。

## 2. docs-sync 判定（docsSyncRequired=false 的对账结论）

- **spec 面 v1.60 已由需求面档 1 先行落稿，本包零 docs 改动**：requirements.md v1.60
  （S-F3.2 五行收敛四行、五处枚举同步、§8 已砍登记、实现侧对账 ⑥ 登记）已随需求面档 1
  于 2026-09-29 落稿（story-author → requirement-reviewer 四查全 PASS → 主线程落稿，
  spec-anchor-check ok:true 复跑，全程见
  `delivery/changes/intake/260929-char-window-v5-trim.md` anchors.req 回填）。实现与
  spec 口径一致性核对：v1.60 四行口径（去组词行）与 char_page.cc 落地 diff 逐条对读
  一致。
- **零触 `docs/contracts/**`**：本包纯板端呈现面与本地资产改动（spine crossEnd exempt
  申报；server payload words 继续下发零改、wire 形状 D3 笔顺 JSON / E1 display 响应块
  零触碰）——architect 分流 review 四分类 = 无需改（NO_CONTRACT_CHANGE），anchors.contract
  已钉。docs/ui 设计母版 `docs/ui/kidpal-ui-design-v5.html` 也不在本包改动面（见 residual②）。
- **架构面零改动**：纯呈现面删改 + 资产重烘，architecture.md 无对应表述面，anchors.arch
  = 不动。

## 3. docs 面 residual（不改 docs/**，呈主线程 / human）

1. **实现侧对账 ⑥ 状态收账**：requirements.md v1.60 已登记「实现侧对账 ⑥ 归本包收账」，
   本包落地后该对账项的状态收账（登记→已完成）属需求面/账面维护——planner / integrator
   对 docs/requirements.md 无写权（scope-map 实况），走主线程收账。本包侧事实 = 实现已
   落地（char_page.cc 四件删改 + 验收齿全绿，commit 见 review-packet）。
2. **设计母版 v5 HTML 查字窗段仍画组词行 / 星**（spine openQ q3，human 2026-09-29 裁
   deferred，非 blocking）：`docs/ui/kidpal-ui-design-v5.html` 查字窗段与 v1.60 定稿
   口径漂移（仍含组词行与装饰星），实现已按 v1.60 收敛。母版更新与否属设计稿面，
   随 ui-align 账面注记一并呈 human 裁（decisionCoverage DC-5 landsIn=dropped）。
