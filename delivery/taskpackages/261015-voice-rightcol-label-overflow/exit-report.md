# 退出报告 · 261015-voice-rightcol-label-overflow（真机门停等 human）

> 2026-10-15，planner 重派会话。host 面全绿 + 机械闸 APPROVE 后按 human 指示停 VR-A6 真机门，交回 human 持板。不 push、不合并 main（合并链待真机轮后由 human 触发）。

## 一句话状态

**host 面闭环完成：实现 3 行、4 新齿转绿、172/0 全绿、mutation 三族取证闭环、verifier final r4 十闸全绿、机械 supervisor 闸 APPROVE（medium）、前置升级闸 PASS——剩 VR-A6 真机轮，停门等 human 持板。**

## 证据链（逐门）

| 相位 | 结果 | 权威制品 |
|---|---|---|
| acceptance 冻结 | 在途存量零改动续用（重派 acceptance-author 逐条核对合格 + 独立冒烟重取证：4 Tests 4 Failures 设计态红，红名与冻结申报逐字一致） | 验收件 568 行 + acceptance-author 回执（pod-events） |
| 红基线 | 172 Tests / 4 Failures——4 红全为新齿设计态红（W=264 超上界 ×3 + 264 销账 ×1），既有 168 齿全绿零新增红 | `/c/tmp/vrl-redbaseline-r1.log` |
| builder | lcd_display.cc 三处 lv_obj_set_width 字面 264→196（header/echo/answer），其余零触碰；CMakeLists 接线核对 | commit 46bf631 |
| mutation 取证 | m1/m2/m3 三族回退变异各自同源双红 + 还原复绿；销账 grep 净（仅注释 2 处提及 264） | `mutation-evidence/m1~m3-*.txt` |
| verifier | scoped r1 红（mutation-check 元数据缺口，TaskPackage 补条目后）→ r2 scoped 八闸绿 → final r3 红（scope-file 形态不符 fail-closed，integrator 落了 JSON、闸要纯文本清单）→ r4 final 十闸全绿 | `verifier-evidence/rounds/r1~r4.json` |
| integrator | closeout 双制品 + verifier-scope（JSON 富信息 + 纯文本清单双载体） + commit 46bf631（22 files） | `delivery/changes/dev/261015-voice-rightcol-label-overflow/**` |
| supervisor | 机械闸 APPROVE / medium / evidenceComplete / rounds=4；supervisor-gate-check exit 0；pre-merge-escalation-gate PASS；合并链按 human 指示停 | `supervisor-verdict.json` |
| 合并链 | 按 human 指示停：不 push、不合并 main；VR-A6 真机轮后由 human 触发 | dev/pod-m2 @ 6d199f8 |

## supervisor ESCALATE 留痕案（verdict#1，噪音误判已消解）

第一次机械闸 ESCALATE：G6 重算把随包提交的 `firmware/sdkconfig` 再生噪音（Image Effects 空节头，+5 行零行为内容，不属 changeManifest）按 critical-path 判 critical → ESCALATE。处置 = 剥离噪音（commit 2498271）→ 显式移除 stale verdict（闸文 FIX-3/FIX-5 许可 + pod-events 留痕）→ 重跑 APPROVE。**注意：每次 Windows idf.py 构建（firmware-build 闸）会再生这个噪音 diff——下包若见同款 diff 勿误判代码回归，处置 = 先剥后判；建议登记 tech debt（同族 TECH-261014）。**

## human 持板清单（VR-A6，真机轮）

烧录 = app 分区单烧（纯代码变更、无 spiffs 资产变更）。本树设备构建可用（firmware-build 闸实证 Windows idf.py 真编译通过）；备选 = C:/AI/KidPal 主 checkout 构建烧录。**builder 取 196（区间上界）**，若真机观感不佳（滚动条遮盖右缘），让位自由度 188~190 已在冻结区间内、改值不触齿。

1. ① 语音轮三态各发一段长文本，目测右缘：echo（LISTEN/THINK）与 answer（TALK）行尾不再被裁（与修复前对比留证：修复前行尾 1~2 字被裁）；
2. ② THINK 态让位（echo_box y104/高 148）下换行一致性；
3. ③ 标头文案显示不回归；
4. ④ 振动条瞬态遮盖观感可接受（不佳 → 让位 188~190，改值后 host 齿仍绿免重跑冻结链）；
5. ⑤ 烧录 = app 分区单烧。

## 未尽事项 / advisory

- **dev INDEX 行未登记**：dev/INDEX.md 无本包行——upsert 属合并链动作，本包未合并、留待合并链补（closeout-check 只读跑过、advisory 在案）。
- **loop-retro 阈值触达**（closeout-check advisory：5 pending + 3 waiting = 8 ≥ 3）——发起责任 = 主线程，建议 human 收口后跑 /loop-retro。
- 技术债建议：登记「firmware/sdkconfig idf.py 再生噪音」tech debt 条目（同族 TECH-261014）；「sonnet 档 API 400 经 cc-switch」环境问题建议单独排查。
- 合并触发面：dev/pod-m2 分支头 = 6d199f8（+本次退出报告 commit），真机轮通过后合并链（INDEX 行 + merge-driver）由 human 触发。
