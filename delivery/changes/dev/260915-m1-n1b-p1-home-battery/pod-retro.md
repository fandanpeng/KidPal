# pod-retro · 260915-m1-n1b-p1-home-battery（第一人称热记忆，planner）

> 标签：第一人称过程体感，只记主观感受与改进项。tokens/gates/rounds/verdict/test results 以 machine artifacts（verifier-evidence/rounds、pod-events、taskpackage）为权威，此处不复述、不冠权威。接棒重启后为收尾相位重写版（前一版体感已吸收进「下次改进」，原始件见 git 历史 f5d3e9f）。

## 卡点与体感

1. **接棒第一课：信实录、验工作树，但别信「看起来完成了」**。前一任在我这个位置被误停时，最后一轮 builder 刚收工、核验缺位。我接棒后靠 pod-events 全量回放把时间线拼出来，再去实核 diff——发现两件实录里没有的事：imgfont 生成脚本在 NTFS 上大小写碰撞（52 写 26 存、内容全小写），和 human 追加的 WiFi 小标裁定根本没派出去。**pod-events 告诉我「派了什么」，只有工作树告诉我「落了什么」**——两个都查了才敢往下走。

2. **「host 截图绿 ≠ 真机绿」这个包的主题，在接棒轮给了我第三集**。前两集是 host fixture 不挂资产走兜底描边（前一手发现的），第三集是 NTFS 大小写不敏感让 host fopen 侥幸命中、设备 SPIFFS 大小写敏感必缺字形——同一主题的三个变体全是「host 环境语义与设备不等价」。我裁方案时特意选了「文件名编码大小写」而非「开目录大小写敏感」或「改大写 SEP」，理由是仓库要能在 Windows 上检出、且 human 裁的 "16 Sep" 口径不该被实现侧倒逼改口径——跨平台仓库里，资产命名从一开始就该避开 OS 文件系统语义差异面。

3. **手绘基元迭代不如一次走对管线**。% 记号人工修形两轮（2x2 点+1px 斜杠 → 3x3 点+2px 斜杠）human 都不认，最后换 Baloo 母版真字形一步到位。「像素画越修越像、字体一次就对」——有现成字形管线时，手工基元只该是最后手段，这个判断我该在第一轮就做。

4. **视觉终验的分工被 human 的工作法纠偏救了**。human 明令「host 截图自查核到位才上板、不用真机轮迭代视觉」之后，节奏明显变好：我亲自读图逐元素核中线（±1px 表）、chrome 面照不到就给数值同档依据、真机一轮收口四项全过。此前真机轮被当迭代工具用，烧录-看屏-返工的成本全压在 human 的眼睛上——这个工作法值得固化为视觉类包的默认纪律。

5. **scope-map 拦我写 device-round-record.md 拦得对**。human 收尾裁定要求如实分列真机证据，我又想自己上手写——hook 拦下，integrator 转手把四轮烧录、四份日志、三项挂起观察项写成了像样的档案。职责分派比抄近路干净，这轮再次验证。

6. **opus 池熔断（前一任手记转存）**：acceptance-author 派发先 400 再 429，human 换号恢复。降格记录动作（friction 留痕）做了，但「备用通道探测」当时没有预案，这次接棒配额熔断预案里补上了。

## verifier↔fixer 绕轮原因的主观判断

接棒后两轮修复都不在 verifier 相位里触发（触发源是真机看屏），修复后回补 scoped 轮——「真机发现直通 fixer、回补 scoped、攒够一批再上板」的路径前一手走出来，我这轮沿用顺畅，两轮 fixer 各只烧一轮、上板即过。r1 的 mutation 落账缺口类合规绕行本轮没有复现。

## 别扭的 acceptance / 帮倒忙的钩子

- journal-append 不收 agent-escalation、pod-events-append 必带 --source：两个记账口各管一半，我第一次接棒留痕就吃了一记 retry。记账入口的参数闭集若能统一些，接棒场景会少踩一脚。
- closeout-check 的 verdict/verification advisory（脚本产、本相位无可识别晋升脚本）如实挂着——advisory 不拦路，是对的。

## escalation themes（cross-ref pod-events）

- dfda0852（agent-escalation / environment / VRT-A2 归档漂移）：final r11 复现同一条，归属裁定已到 human 面前（归档效应落账 or 在册登记 owner 判读）。
- f6132282 / 45910f60（human-decision：视觉四项 PASS + 充电/断网切换跳过留观察）：PH-A6 以附带条件 PASS 收口。
- 83180e57（human-correction：对齐 + % 返工）：fix-align 一轮消账。

## 下次改进

- intake 追问「host fixture 与设备真实路径的差异面」（资产挂载、字体、分辨率）**加上「资产生成与消费两侧的文件系统大小写语义」**——跨平台仓库冻结前问一次。
- 失败判读逐条点名，禁止只看 stdout 头部摘要（前一任被 integrator 补住的教训，转存为我的肌肉记忆）。
- agent 被截断后：先验制品完整度，再决定续跑/重派/降档。
- 有字形管线可用的记号（%、°C 一类），第一版就走管线，手工基元留作管线不可行时的兜底。
- 视觉类包默认工作法：host 截图自查（含逐元素数值表）→ planner 亲自读图 → 一轮上板 human 终验；chrome 面数值依据 + 真机兜底。
