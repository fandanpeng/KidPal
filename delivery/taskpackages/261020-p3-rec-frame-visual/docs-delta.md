# docs-delta · 261020-p3-rec-frame-visual

- 日期：2026-10-20（integrator 收口相位据实申报）
- 作者：integrator
- 结论：**零 canonical 文档改动**（声明口径留痕件）。

## 申报明细

| 面 | 改动 | 依据 |
|---|---|---|
| `docs/requirements.md` | 零改动 | 本包 = 设备端本地像素布局微调，不改需求语义：S-F2.2 元素集（AC1）与回显常驻/键位（AC2）零变化——REC 闪点、「REC」文案、录音时长、声波、回显五件全保留，「在记」语义（WF-F2-02 / v7「在记信号三帧常驻」）不动。 |
| `docs/architecture.md` | 零改动 | 无新架构决策、无新组件/依赖/跨层调用；`adrRequired=false`，diff 只动 `kidpal_story_page_render` 内 RECORDING 分支的绘制坐标常量与分支头注释，函数签名与本体不动。 |
| `docs/contracts/**` | **零改动、零触碰**（本包恒只读） | architect 逃生门自查 `NO_CONTRACT_CHANGE`（rules/contracts.md 四分类表明列「设备端本地渲染细节」归此类）：display 统一响应块是 server→device wire schema，设备端本地像素布局不在其列；server/** 零改动（`crossEnd.exempt` 在册）。 |
| `docs/ui/kidpal-ui-design-v7.html` + WF-F2-02 线框 | 零改动（**后补账在册，不阻塞本包**） | 本包落地后 v7 设计稿 :945–979 两帧位式与实现漂移——按观感直修先例（2026-10-16 human 反馈）钉为真机观感终判后回同步（plan「后补账提示」节）；本包不预改设计稿（真机终判可能再调坐标，预改即二次返工）。 |

## 实现侧文档承载（非 canonical，随代码落位）

- `firmware/main/display/story_page.c` RECORDING 分支头注释：位式注释整体改写承载改动依据——「在记」一行起点 200 = 帧 B 引导气泡右缘 192 + 8px 安全距、声波缩绘 212/256（≈0.83）左上 pivot 落位 x=8..179 / y=191..219、兜底跳动条同位式（居中 x=32..154、中心 y=205 = 缩绘后声波中线）——设计理由全部随代码注释落位，无需晋级 canonical。

## docs-sync 闸状态

verifier scoped round 5 与 final 相位 docs-sync 闸均 PASS（`docsNoChangeReason` 申报字段在册，本文件即该申报的展开留痕）。

## 升级评估

docs-sync 复核未发现契约需要变化、spec 漂移或 canonical 缺口。v7 设计稿回同步若最终要动 `docs/ui/kidpal-ui-design-v7.html`，属设计面后补账（观感直修先例通道），不构成本包 docs-sync 缺口。**本包零 canonical 写点成立。**
