# architect-landing-summary.template · Track B landing 摘要模板（canonical · 260802-gov-review-track-b-hardening T2 落地，OQ2=(b) 选址 docs/templates/）

> 用法：复制为 `delivery/changes/arch/<change-id>/architect-landing-summary.md`，替换 `<尖括号>` 占位。
> 文体：散文节按 AGENTS.md「输出标准」节写通顺完整的句子；manifest 围栏块、packet-id 行等机器校验结构保持精确格式；证据引用保持逐字精确。
> **五件组件 = packet 绑定面**（FROZEN PROPOSAL packet-binding 行声明的组件清单）：缺一即结构门 / landing-verifier 合法打回理由。
> 形制来源：Track B 乘客 summary 实际形状归纳 + unit-gate F4（缺 `landing-manifest` 围栏块 → B2 RED；「packet-binding」非字面「packet-id」 → 正则漏）教训固化。

---

# architect-landing-summary · <change-id>

packet-id：<FROZEN PROPOSAL §2 改法集（T 编号范围）+ §5 验证集> · 评审轮次 <R1…Rn，GREEN 轮注明> · change-id <gov-review-…>

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| <T1> | <path> | <一句结果> |

## 2. 验证证据

<逐条：验证集编号 → 命令 → 实际输出关键行 → 绿/红；真空绿规约遵守情况（重放先断言输入非空 / 自检输出含新增自检名 / 环境红基线）如实写>

## 3. 诚实注记

<ADVISORY / 允差 / 残留 / 降级 / 未落项（留根去向）——如实登记；无则写「无」>

枚举式披露示例（批 1 A1 教训固化）：WARNING/ADVISORY 类披露枚举计数与逐项，不点名代表。

```landing-manifest
<canonical 路径一行一个；恰 = FROZEN §2 目标文件锚清单并集（条件锚按 OQ 裁断裁剪），与 §1 表文件列一致>
```
