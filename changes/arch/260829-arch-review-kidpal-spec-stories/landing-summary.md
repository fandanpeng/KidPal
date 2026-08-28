# Landing Summary · 260829-arch-review-kidpal-spec-stories

> 主线程 packet-bound landing 记录。packet = FROZEN PROPOSAL v0.2 + landing-manifest + 本 summary + 契约门证据(CONTRACT-CHANGE-PACKET + architecture-gate-evidence)。

## 落地范围(对照 landing-manifest)
| 文件 | 落地内容 |
|---|---|
| `docs/systems/kidpal/spec.md` | §4 整节重写为「功能需求→用户 Story」:§4.0 共享底座(7 状态+双层手势表)+ §4.1 课表 5 story + §4.2 故事 5 story + §4.3 查字 3 story(共 13 条);顶部加已砍功能声明;§5.3/§5.4/§5.6 最小同步(键位注/P2·P3·P4 条目/查字尺寸) |
| `docs/contracts/interface-contract.md` | §1.3 ③ story_list.data 新增 additive 可选 `total` 字段(契约门批准) |
| `docs/systems/kidpal/ui/kidpal-ui-design-v2.html` | R1-m5 废弃标注(P2 当前课反白 ×3、P3 生字反白 ×2)+ R2-B1 键位收口(gesture-table KEY长按/双击/BOOT长按、P4 底部、P3 底部) |

## 评审轨迹
- R1(architect):打回 7 项(1 BLOCKER B1 + 3 MAJOR M1/M2/M3 + 3 MINOR m4/m5)。
- human 三裁定:B1=KEY长按回首页 / M2=P3 KEY双击进阅读 / m5=改设计稿加废弃标注。
- R2:6 项修复确认,B1 设计稿侧残留。
- R3:B1 设计稿 5 处收口确认 → **GREEN**。
- 契约门:architect-reviewer 四分类 = CONTRACT_CHANGE_REQUIRED → human 批准 → 落地 → evidence 台账 CLOSED。

## 验证集结果
- V1 锚点:19/19 命中 ✓
- V2 契约 diff 仅 story_list 一处 ✓
- V3 改动文件 = spec + 契约 + 设计稿(+ 本包制品 + INDEX)✓
- V6 story_list JSON 样例合法 ✓
- V4/V5 人读项:13 story 结构齐备、契约一致(landing-verifier 复核)

## 留根(进 retro)
OpenClaw 定时能力待确认 / P3 语音问生字实测 / total 云端实算与固件读取(留 dev-loop)/ F3.2 组词数据源核对。
