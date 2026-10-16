# retro.template · Track B retro 模板（canonical · 260802-gov-review-track-b-permanentize T10 落地，human OQ-7）

> 用法：复制本件为 `delivery/review/<change-id>/retro.md`，替换 `<尖括号>` 占位；**带【硬项】的小节是义务不是建议**——缺失即判官 / 核验官打回的合法理由。
> 文体：散文节按 AGENTS.md「输出标准」节写通顺完整的句子；计量表与证据引用保持精确。
> **retro = Track B 第 6 步闭环件**（制品 4 件之一：PROPOSAL + architect-landing-summary + landing-verdict + retro）；landing-verdict 绿后由主线程趁热撰写、**先于 human 合并完成**（合并仅剩分支搬移）；retro 不含合并锚——落点由 `delivery/review/INDEX.md` `mergedInMain` 列机械现推；共性建议登记 ARQ 收件箱，决策日志行挂 retro 链接。
> 形制来源：4 份真实 retro 形状归纳（auth-axis / unit-gate / verifier-dispatch / claude-consistency，260802-gov-review-track-b-permanentize T10）。

---

# retro · <change-id>（Track B 全环：<相位摘要>）

> 主线程第一人称热记忆 · <date> · 乘客 #<n> · harness 形态一句话（如「判官 / 核验官 = 零上下文 coder 子 agent 降级形态」）· 跨乘客对照定位（如适用：「第 N 次踩同类坑」）。

**【硬项 R1 · 环后趁热、第一人称】** verdict 绿后即写，第一人称热记忆；隔场补记须声明。

**【硬项 R2 · 计量诚实】** 无 token / 时长计量时，§2 必须诚实声明估计方式与误差界（±），不得装精确。

**【硬项 R3 · 摩擦带根因 + 证据锚】** §3 每条 F 必带 现象 → 根因 → 证据锚（file:line / 实测输出）。

**【硬项 R4 · 建议必标载体】** §4 每条建议必标载体（固化候选 → ARQ 收件箱 / 登记不硬治 / 留批次）——「建议不固化 → 下一乘客复发」是 4 乘客实证流程债（unit-gate retro §5）。

**【硬项 R5 · 保险丝判定】** §5 必含「两方独立结构（判官独立 + verifier 独立）是否接住作者偏差」的明确判定。

## 1. 时间账

硬数据锚点表（判官各轮 verdict / 时长 / tokens、verifier、制品 mtime 链）：

| 相位 | 内容 | 开始 | 结束 | 时长 | tokens |
|---|---|---|---|---|---|
| 起草 | <PROPOSAL v0.1→vX.Y> | | | | |
| 评审 R1…Rn | <判官 verdict 逐轮> | | | | |
| FROZEN | <凭据齐> | | | | |
| landing | <T1…Tn 落地> | | | | |
| 核验 | <landing-verifier verdict> | | | | |
| **合计** | | | | | |

对照：<前几乘客 / 基线数据，一句话>。

## 2. Token 账

计量方式诚实声明：<有计量 → 来源；无计量 → 估计法 + 误差界 ±>。

| 载体 | tokens | 占比 | 备注 |
|---|---|---|---|
| 主线程（起草 + 落地） | | | |
| 判官（R1…Rn） | | | |
| 核验官 | | | |

花费最大处：<…>。可压缩分析：<…>。

## 3. 摩擦与卡点（按痛感排序）

- **F1 · <标题>**：现象 <…> → 根因 <…> → 证据锚 <file:line / 实测输出>。
- **F2 · <标题>**：…
- **near-miss（最险一刻）**：<最险的一次差点出事的瞬间 + 为什么没出 + 教训>。（显式标注，不可省。）

## 4. 优化建议（按 ROI 排序）

1. **<建议>**：治 <F 编号> · 载体 = <固化候选 → ARQ 收件箱 / 登记不硬治 / 留批次（去向写清）> · 预期收益 <…>。
2. …

## 5. 一句总结

<成本（tokens / 时长）/ 触点（human 几次）/ 制品数 / 对照前几乘客> · 最险一刻 <…> · **保险丝判定：两方独立结构（判官独立 + verifier 独立）<接住 / 未接住>作者偏差**（一句证成）· 最值得带走的一条 <…>。

## 6. 本环遗留（条件节：批次型变更特有；无则省略）

留根 / ADVISORY 处置 / 不关的根：<逐条 + 去向>。
