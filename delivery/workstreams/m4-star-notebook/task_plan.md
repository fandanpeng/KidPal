---
workstream: 阶段四·星星记录本（P3 四态 + F2 改名对账）
slug: m4-star-notebook
status: active
opened: 2026-09-24
closed:
summary: M4-star-notebook 里程碑拆包——P3 列表（S-F2.1，含删除确认覆盖层）+ 记录会话双入口链路（S-F2.2）+ 星星徽章盖章（S-F2.4）+ 阅读态（S-F2.3）+ F2 改名对账四项；契约三触点（story_list 语义拆分 / story_text seq 承载 / 删除载体新增）合一走契约门、随 K1 同 PR
owner: main-thread
area: delivery
changes:
  - 260924-m4-k1-story-cloud-link
  - 260924-m4-k2-p3-star-notebook-ui
  - 260924-m4-k3-record-session-live
---

# m4-star-notebook · task_plan

> 拆包执行体注记（如实记录）：本批拆包由主线程以 portfolio-planner 纪律执行（本会话非 `claude --agent portfolio-planner` 形态启动）；readiness 调查 = Explore 子代理（图索引项目 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，18040 nodes）+ 主线程对磁盘逐条核对。门 1（architect 轻量 checklist）与契约预审（architect-reviewer 四分类）由主线程派发独立判官，裁决记录见 review.md。

## 目标

孩子翻到 P3 看到星星记录本列表（星星计数 ×N、每条星星+标题+日期、选中带边框、分屏翻页、BOOT 长按删除带整屏确认弹窗、删除云端同步计数 −1）；
在 P3 按 KEY（或任意页说「我要讲故事」/「帮我记下来」）即进记录语境会话——引导态「需要我帮你记点什么？」、开口即录音态（REC+大时长+声波+整段累积回显、AI 不逐句回复）、说「我说完了」/再按 KEY/120 秒无语音自动收尾、BOOT 丢弃；
保存成功那一瞬全屏星星徽章盖章「星星徽章·第 N 颗已入册！」（N = 入册流水号只增不减）定格 0.8 秒，回列表计数 +1；
选中一篇 KEY 双击进阅读态（标题+正文分屏、BOOT 翻屏 wrap 永不离页、KEY 长按退出回列表，纯阅读无朗读/会话入口）。
同时收口 F2 改名对账四项（盖章文案/页名/提示缺第二指令/选中反白→边框）——代码追上 requirements v1.26–v1.52 已冻结口径。

## 设计权威

- 需求权威 = `docs/requirements.md` F2 节现文（S-F2.1/S-F2.2/S-F2.3/S-F2.4 + WF-F2-01~05 五线框 + F2 intro 改名对账块 v1.33）+ §2.1 记录会话段 + §2.3 按键手势全矩阵（P3 五行 + 单双击消歧 dev 注记）+ §2.4 P3 页面结构 + §6 阶段四 + NFR-READ-1；
  拆包输入 = `delivery/milestones/milestones.jsonl` 行 `M4-star-notebook` nextAction 两块（sourceType=delivery-story，无 PROPOSAL 输入——m1/m2/m3 同款先例）。
- 支撑锚 = `docs/architecture.md` §5.1.1（story_page 四态/层栈/按键路由现状）、§5.1.2（front 故事接线/story.py 归档）、§5.2 场景④（记录归档链路）、§5.4（故事数据行——删除【链路载体待契约】）、§5.5.4（记录会话演出已实现 + 改名对账四项【修复项】）、§7 契约门在办②④、to-be ⑥（session_ctx 接线点）。
- 契约面 = `docs/contracts/interface-contract.md` §1.3③ story_list / §1.3④ story_text / §1.4 control（start_recording 既有）/ §四 kidpal_session_ctx（**已 LANDED**，CDQ-260914-kidpal-session-ctx，本批零契约改动、只落实现）。**本 workstream 触契约变更三触点**（story_list total 语义拆分 + story_text seq 承载与行为句清理 + 删除载体新增），合一走 architect-reviewer 四分类 + 人类契约门，K1 承载（launchPreconditions 第 1 条），CONTRACT-CHANGE-PACKET 草案在 K1 包目录。
- 设计稿权威 = `docs/ui/kidpal-ui-design-v5.html` P3 组（列表/录音/盖章/阅读/删除确认五面齐，v1.51 起对账通过；提示条双指令文案「说『我要讲故事 / 帮我记下来』开录新的」以 v5 为准）。

## 相位

1. **拆包**（2026-09-24，本批）：三包 K1/K2/K3 + workstream 四件套 + CCP 草案落盘；board 预填 queued（mergeOrder 38/39/40）。
2. **门 1 + 契约预审**：architect 轻量 checklist review（切法）+ architect-reviewer 四分类预审（CCP 草案）——并行派发，裁决落 review.md。
3. **human 两判断点**：① 过目切法（门 1 GREEN 后）；② 人类契约门裁 CCP Human Question（K1 launch 硬前置）。
4. **commit-before-launch + launch prep**：常驻分支同步 main → 拆包产物 commit → `base/m4-star-notebook` ref → kidpal-pod-launch-prep（check-launch / launch-card / 图索引 / 环境等价化）。K1 先行（依赖图 K1→K2→K3 串行，单常驻树接力棒形态，M3 先例）。
5. **执行与收口**：各包 dev-loop 闭环 → K3 live 真机轮一次性收口（含 K1/K2 live 半 deferred 复证）→ integrator 收口填 closed → 归档。
6. **重拆**（2026-09-29，human 指示「M4 只开发 K1，剩下的按新定义的拆包规则重新拆包、覆盖」）：K1 维持不动（已 merged 本地 main，merge-completed e12a8b94）；剩余 K2/K3 按 260928/260929 拆包新规覆盖重写——切法维持两包边界（09-24 门 1 + human 三包切法维持，尺寸维四判据复核通过：story_page.{c,h} 文件面 K2 独占、fixBudget=2 验证预算内、N=2 无并行度争议），制品形态翻新（acceptance 空骨架化→acceptance-author 唯一写点 / plan.md 五件骨架收窄 / 拆包侧 brief 撤回→Pod planner Step 7 自产 / changeManifest 粗填（1g 档精化）/ K2 uiux L2 纯消费挂图 docs/ui/V5 P3 五面 PNG / sourceRefs 补 S-Fx.y-ACn 复合句法）；defer 包 260928-story-connect-push-defer 后果吸收（①建连即推摘除→恢复语义重裁落 K3；C4 arch 收口半随 K3 docs-sync）。门 1 重审 + human 重过目切法 → 重拆批 commit → K2/K3 依序 launch。cutoff 诚实注记：目录 git 首提 09-24 < 09-28 机器齿 cutoff，两包机器放行、制品自觉按 post-cutoff 全量合规（DC _doc 重拆记录⑧）。

## 决策（拆包级取舍点，human 判断点材料）

- **D1 切法 = M3 镜像三包**（云端+契约 / 板端展示面 / 记录会话跨端+live 收口）。备选「K2 再拆二」（列表+删除 / 阅读+盖章）评估后不取——同触 story_page.c 单文件族，拆开徒增同文件合并冲突面；K2 内以验收齿分块（A2~A6）保留阶段性。human 过目切法时可改判。
- **D2 删除载体拟甲案** = §四新增端→网关单向私有消息 `kidpal_story_delete`（kidpal_session_ctx 对偶先例；删除结果不走 RPC 应答、靠 story_list 重推对账）。备选乙（HTTP /command 复用——设备无 HTTP 客户端面）/丙（control 块反向——方向语义不符）见 CCP §2，随契约门裁。
- **D3 盖章流水号 N 承载** = `story_text.data.seq`（additive；盖章触发点 = fresh_save 判定即 story_text 回传时，单点携带最省）；`story_list.data.total` 语义收窄 = 现存篇数（删除即 −1）。两口径拆分 = requirements v1.47 冻结（S-F2.4 AC2）。
- **D4 120s 录音自动收尾归 front**（累积态静默检测；板端零新增计时器——设备侧 60s/120s timer 生产者接线是 M1 遗留 to-be ②，不在 M4 复活）；板端收尾演出由 story_text+seq 下发驱动。取舍点留 K1 plan.md，随门 1 过目。
- **D5 收尾词（「我说完了/讲完啦」）= front 机械检测**（累积态 stt 帧文本扫描——AI 不逐句回复 ⇒ 词不可能经 LLM 判定）；StoryAccumulator 既有 routing/recover 面先查现状、已有语义零偏移复用。
- **D6 story_list 推送触发** = WS 建连 + 归档成功后 + 删除执行后（行为策略归 arch §5.2④ 扩展、不进契约——M3 upcoming 三触发同款分界）；板端「进页即见列表」靠 RAM 缓存（决策 24 介质禁令不变），无 restore 类冷启动保障（如需 = arch 面另裁，CCP Q4 问过目）。
- **D7 星星徽章美术** = 先以既有 art 资产（ic_star_48/38 等 47 资产集内）对齐 v5 设计稿 P3 组；若 v5 核对需新形态资产，资产管线子任务在 K2 内（**双分区烧录规则**：app+art 同烧，K4 空框教训）。
- **D8 prompt v7 归 K3**（第二指令「帮我记下来」意图判定 + 故事分工核对；M3 先例 prompt bump 归语音包）；v7 新文件不回改 v6（版本化演进 + ADR-001 接线不变式）。
