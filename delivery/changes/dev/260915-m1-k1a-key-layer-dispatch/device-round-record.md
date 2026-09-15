# Device Round Record — 260915-m1-k1a-key-layer-dispatch（KD-A5 真机确认轮）

> **状态：PENDING**（2026-09-15 integrator 落笔）。本文件 = KD-A5（真机确认轮，kind=structural，human 持板）的证据留位，**当前不含任何真机证据**——human 持板轮未跑，零伪造。

## 待确认三行为（KD-A5 冻结口径）

1. 覆盖层内（提醒弹窗 / 查字窗 / 语音轮演出为顶层）BOOT 单击 = 关闭顶层回原页、**不产翻页动作**；
2. 主页面 BOOT 单击 = 翻页（主页面矩阵行零回归）；
3. KEY 长按 = 返回上一层（覆盖层顶层时关层回原页；主页面时回首页）。

## 证据落位约定（human 持板轮完成后由记录者补写）

- 烧录方式 / COM 口 / app@0x20000 hash verified 记录；
- 三行为逐条日志互证（device-round-*.log 归档本目录）；
- human PASS/FAIL 裁定 + pod-events 锚定 eventId；
- 参考先例 = `delivery/changes/dev/260911-key-route-a1/device-round-record.md` 同款结构。

## 门口径

- 本包其余义务（KD-A1~A4/A6）已由 host 齿 + r1 fact 覆盖；KD-A5 是合并门放行前唯一待补 structural 义务——planner 排 human 持板轮，或 human 显式裁「KD-A5 后置 + 风险接受」（裁决留痕）。
