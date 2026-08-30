# 评审记录 R3 · architect（spec story / T1 主审，增量复审）

> 评审对象：`docs/kidpal/ui/kidpal-ui-design-v2.html` 设计稿侧五处收口（R2 唯一剩余打回项 B1 + 连带 N2② M2 设计稿残留）。
> 性质：**增量复审**——只核 R2 打回项（设计稿侧 KEY 长按 / P3 进入阅读手势未收口）是否补齐，并防新引入矛盾；不全面重审。R1/R2 上下文见同目录 review-r1-architect.md / review-r2-architect.md。
> human 裁定（沿用）：B1（KEY 长按=回首页，P4 查下一个字走语音）、M2（P3 列表→阅读/回听=KEY 双击）。
> 证据基准：`ui/kidpal-ui-design-v2.html` × `spec.md` §4.0（spec.md:270-281），file:line 逐字引用。
> 结论：**GREEN（可 FROZEN）**。

---

## 结论

**GREEN。** R2 唯一剩余打回项（B1 设计稿侧 KEY 长按语义未收口，连带 N2② M2 设计稿 P3「BOOT 听」残留）已补齐。主线程改的设计稿五处全部真实落地、与 human 裁定（B1=回首页 / M2=KEY 双击）一致、与 spec §4.0 手势表逐行对齐，且未引入新矛盾。spec↔设计稿在键位手势上现已定一尊（§4.0 为权威源，设计稿 gesture-table 同步收口），B1 的「同一颗键长按到底干什么」二义性彻底消除。

---

## 五处收口逐项核对（coordinator 报告 → 实际落地）

### ① gesture-table KEY 长按行 —— 已落地 ✓
- 现状：`<span class="kd">KEY</span> 长按 | 回首页<!-- 【修订·2026-08-29】原为「预留/P4查下一个字」，human 裁定 B1 改为回首页；查下一个字走语音 --> | 所有页面一致`（kidpal-ui-design-v2.html:621）。
- 核对：全局行为=「回首页」、页内特例=「所有页面一致」，与 spec §4.0 `KEY 长按 | 回首页 | 所有页面一致`（spec.md:276）逐字对齐；修订注释完整交代了「原为预留/P4 查下一个字 → human 裁定 B1 改回首页 → 查下一个字走语音」的来龙去脉。**R2 B1 设计稿侧残留消除。**✓

### ② gesture-table 新增 KEY 双击行 —— 已落地 ✓
- 现状：`<span class="kd">KEY</span> 双击<!-- 【新增·2026-08-29】human 裁定 M2 --> | 无全局语义 | P3 列表进入阅读／播放选中`（kidpal-ui-design-v2.html:618）。
- 核对：与 spec §4.0 `KEY 双击 | （无全局语义） | P3 列表=进入阅读/播放选中`（spec.md:275）语义一致（「无全局语义」vs「（无全局语义）」仅括号差异，无歧义）；行位置在 KEY 短按（:617）之后、BOOT 短按（:619）之前，键序合理。**M2 双击维度在设计稿侧补登。**✓

### ③ gesture-table BOOT 长按行 —— 已落地 ✓
- 现状：`BOOT 长按 | 无全局语义 | P2 今天⇄明天切换（也可直接说「明天的课」）· P3 阅读翻下一屏`（kidpal-ui-design-v2.html:620）。
- 核对：补上了 P3 阅读翻下一屏特例，与 spec §4.0 `BOOT 长按 | （无全局语义） | P2=今天⇄明天切换 · P3 阅读=翻下一屏`（spec.md:278）一致，也与 S-F2.3「BOOT 长按翻下一屏」（spec.md:362）一致。**设计稿 gesture-table 与 §4.0 在 BOOT 长按特例上对齐。**✓

### ④ P4 底部键帽 —— 已落地 ✓
- 现状：`回首页<span class="kd">KEY</span><span class="kcap-note">长按</span><!-- 【修订·2026-08-29】原为「下一个 KEY长按」，B1 裁定 KEY长按=回首页；查下一个字走语音 -->`（kidpal-ui-design-v2.html:1003）。
- 核对：原「下一个 KEY长按」已改为「回首页 KEY长按」，与 human B1 裁定一致；修订注释交代了改动缘由。**R2 指的 P4 画布 :1002 旧「下一个 KEY 长按」残留消除。**✓

### ⑤ P3 底部键帽 —— 已落地 ✓
- 现状：`读<span class="kd">KEY</span><span class="kcap-note">双击</span><!-- 【修订·2026-08-29】原为「听 BOOT」，M2 裁定 P3 列表→阅读=KEY 双击；BOOT 短按专职翻页 -->`（kidpal-ui-design-v2.html:881）。
- 核对：原「听 BOOT」已改为「读 KEY双击」，与 human M2 裁定一致；注释说明「BOOT 短按专职翻页」，与 gesture-table BOOT 短按行「P1→P2→P3→P4 循环翻页 / 所有页面一致」（:619）自洽。**R2 N2② 设计稿 P3 主视图「BOOT 听」残留消除。**✓

---

## 防新引入矛盾复核

- **键序一致性**：设计稿 gesture-table 现行序为 KEY 短按(:617) / KEY 双击(:618) / BOOT 短按(:619) / BOOT 长按(:620) / KEY 长按(:621) / PWR(:622)。双击行插在 KEY 短按之后，KEY 长按行因表格重排移至 :621（R2 时为 :620），属正常重排，无逻辑问题。
- **同一键无重复/冲突定义**：KEY 长按仅出现一次（:621 回首页），KEY 双击仅出现一次（:618 无全局/P3 进入阅读），无一行多义、无跨行矛盾。
- **gesture-table 与各页画布键帽一致**：P3 主视图底部「读 KEY双击」(:881) 与 gesture-table KEY 双击行(:618) 一致；P4 底部「回首页 KEY长按」(:1003) 与 gesture-table KEY 长按行(:621) 一致；P3 阅读态底部 `BOOT 长按 翻屏`（:924，未动）与 gesture-table BOOT 长按 P3 阅读特例(:620) 一致。无新冲突。
- **与 spec §4.0 整体对齐**：五行（KEY 短按/KEY 双击/KEY 长按/BOOT 短按/BOOT 长按）全局行为 + 页内特例逐行对得上 spec.md:274-278，spec↔设计稿定一尊。✓

---

## 打回项闭环

| 来源 | 项 | R3 状态 | 证据 |
|---|---|---|---|
| R2-B1 | 设计稿 gesture-table KEY 长按仍是「预留/P4 查下一个字」 | **已收口** | kidpal-ui-design-v2.html:621（改为回首页） |
| R2-B1 | 设计稿 P4 底部「下一个 KEY 长按」 | **已收口** | kidpal-ui-design-v2.html:1003（改为回首页 KEY 长按） |
| R2-N2② | 设计稿 P3 底部「听 BOOT」 | **已收口** | kidpal-ui-design-v2.html:881（改为读 KEY 双击） + gesture-table 补 KEY 双击行 :618 |

R1 七条打回项（B1/M1/M2/M3/m4/m5/m6）至此全部闭环：B1 在 R2 完成 spec 侧、R3 完成设计稿侧；M1/M2/M3/m4/m5 在 R2 已修复，M2 设计稿残留在 R3 补齐；m6 R1 已核通过无回归。

---

## 复审范围声明

本轮为增量复审，仅核 R2 唯一剩余打回项（设计稿侧 B1 + 连带 N2②）的五处收口与防新引入矛盾。spec §4 其余维度、契约对齐、story 覆盖完整性等沿用 R1/R2 结论，未重核。

**最终结论：GREEN，可 FROZEN。** 后续 landing 仍须走主线程 packet-bound + landing-verifier 独立核验；`story_list.total` 的契约新增属 T2 契约门范畴，本评审链不预判其结论。
