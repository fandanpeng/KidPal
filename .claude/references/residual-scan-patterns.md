# residual-scan-patterns · 退役语面复扫形态参照清单（canonical · 260824-gov-review-track-b-drafting-hardening-2 T4 落地，OQ-C1=(a) prose 清单）

> 用法：起草 PROPOSAL 硬项 1c（退役语面清扫「全形态枚举 + 完备性论证」）时，把本批退役词套进下面的维度模板逐一展开，不靠起草人现场记忆。纯 prose 参照、非机检；机检化（`--residual-scan` 脚本扩展）留后续候选（本批 §6 留根）。

## 1. 维度模板（退役词 × 四维展开）

| 维 | 分解 | 实证例族 |
|---|---|---|
| 词形变体 | 单复数 / 时态 / 缩写 / **派生复合** | 派生复合族 = 派 / 派生 / 派发身份 / 派遣身份——即 verifier-dispatch R1→R3 扩模与 FIX-11 实际漏检的形态族（「派生 `supervisor`」为 FIX-11 漏检实例） |
| 有无 backtick | `` `word` `` / `word` | backtick 盲区实证链：FIX-11 → unit-gate 同族一环（转述）→ verifier-dispatch R1 漏、R3 扩模才抓出（判官 5 轮合计 874,620 tokens） |
| 大小写 | 原形 / 全大写 / 首字母大写等 | 随退役词语料逐形展开 |
| 目标上下文词 | 承载退役词的上下文词形 | 实证例示：派 X / Agent(X) / 命令形（verifier-dispatch R1 实际模式含「派 verifier」与「Agent(verifier)」两类上下文） |

## 2. 填法指引

把退役词代入维度模板逐维展开成模式集，对每一模式给出清扫命令（Grep/Glob，注明命令）与命中数；超长匹配行遇「Omitted long matching line」类工具省略显示时逐条展开再计数（硬项 1b 义务），枚举声明注明展开取证处。

## 3. 完备性论证（本清单自身，硬项 1c 率先垂范）

本清单维度 = 历史实际漏检形态族的归纳枚举：① FIX-11 漏「派生 `supervisor`」（派生复合 × backtick 组合形态）；② verifier-dispatch R1→R3 扩模史（「派生 `X`」类派生复合 × backtick 组合，R1 漏检、R3 扩模抓出）；③ unit-gate 同族一环（据 verifier-dispatch retro §4-#1/§5 转述，其本环 retro 无记录）。**本清单非封闭穷尽集**：遇清单外新形态，先扩本清单并注实例，再执行清扫——清单随实证增长。
