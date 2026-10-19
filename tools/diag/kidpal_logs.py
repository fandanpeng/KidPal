#!/usr/bin/env python3
"""kidpal_logs.py — 诊断日志按需 USB 串口取回 host 脚本。

TaskPackage 261018-sd-log-usb-read（plan 设计要点 §8）。设备插 USB（COM5，原生
USB-Serial/JTAG）后，本脚本按需发一条**只读**命令（kplog list|tail|cat），收集设备
stdout 的 KPLOG BEGIN..END 帧内容并落本地文件。不自动 dump、不上云、不改正常日志流。

单进程独占串口（与串口监控 / 烧录互斥，跑前关其它串口工具）。打开端口后立即
dtr=False / rts=False（防复位伪影，遵循 kidpal-serial-monitor 纪律）。

依赖：IDF python env 的 pyserial（import serial）。

用法：
  python tools/diag/kidpal_logs.py --port COM5 --cmd list --out list.txt
  python tools/diag/kidpal_logs.py --port COM5 --cmd tail --n 50 --out tail.txt
  python tools/diag/kidpal_logs.py --port COM5 --cmd cat --index 0 --out kplog000.txt

退出码：0 成功；2 参数 / 依赖错；3 串口打开失败（占用 / 设备不在）；4 收帧超时；
        5 设备返回错误帧（KPLOG ERROR <code>）。
"""

import argparse
import sys
import time

try:
    import serial  # pyserial（IDF python env 自带）
except ImportError:
    sys.stderr.write(
        "error: 未找到 pyserial。请用 IDF python env 运行本脚本"
        "（source $IDF_PATH/export.sh 后的 python，或 "
        "~/.espressif/python_env/idf5.5_py3.12_env/bin/python3）。\n"
    )
    sys.exit(2)

BEGIN_MARK = "=== KPLOG BEGIN"
END_MARK = "=== KPLOG END ==="
ERROR_MARK = "KPLOG ERROR "


def build_command(args):
    """把 CLI 参数映射成设备行协议命令（严格 "kplog " 前缀 + 单空格 token）。"""
    if args.cmd == "list":
        return "kplog list"
    if args.cmd == "tail":
        if args.n is None:
            return "kplog tail"  # 缺省走设备默认 200
        if args.n <= 0:
            sys.stderr.write("error: --n 必须为正整数\n")
            sys.exit(2)
        return "kplog tail %d" % args.n
    if args.cmd == "cat":
        if args.index is None:
            sys.stderr.write("error: --cmd cat 需要 --index（0..31）\n")
            sys.exit(2)
        if args.index < 0 or args.index > 31:
            sys.stderr.write("error: --index 越界（合法 0..31）\n")
            sys.exit(2)
        return "kplog cat %d" % args.index
    sys.stderr.write("error: 未知 --cmd %r\n" % args.cmd)
    sys.exit(2)


def read_frame(port, timeout):
    """读串口直到 KPLOG BEGIN..END 帧完整；返回 payload（BEGIN 行与 END 行之间的字节）。

    丢弃 BEGIN 之前的普通日志（设备正常 esp_log 输出）。超时返回 None。"""
    begin_bytes = BEGIN_MARK.encode("ascii")
    end_bytes = END_MARK.encode("ascii")
    buf = bytearray()
    begin = -1
    deadline = time.monotonic() + timeout

    while time.monotonic() < deadline:
        chunk = port.read(256)
        if chunk:
            buf.extend(chunk)
        if begin < 0:
            i = buf.find(begin_bytes)
            if i >= 0:
                j = buf.find(b"\n", i)
                if j >= 0:
                    begin = j + 1  # BEGIN 行之后即 payload 起点
        if begin >= 0:
            k = buf.find(end_bytes, begin)
            if k >= 0:
                return bytes(buf[begin:k])
        if not chunk:
            time.sleep(0.05)
    return None


def main(argv):
    ap = argparse.ArgumentParser(description="KidPal 诊断日志按需 USB 串口取回")
    ap.add_argument("--port", default="COM5", help="串口设备（默认 COM5）")
    ap.add_argument("--cmd", required=True, choices=["list", "tail", "cat"],
                    help="只读命令")
    ap.add_argument("--n", type=int, default=None,
                    help="tail 行数（缺省走设备默认 200，上限 1000）")
    ap.add_argument("--index", type=int, default=None, help="cat 文件索引 0..31")
    ap.add_argument("--out", default=None, help="输出文件（缺省写 stdout）")
    ap.add_argument("--timeout", type=float, default=10.0, help="收帧超时秒（默认 10）")
    ap.add_argument("--baud", type=int, default=115200,
                    help="波特率（原生 USB-CDC 忽略，兼容旧桥片）")
    args = ap.parse_args(argv)

    command = build_command(args)

    port = serial.Serial()
    port.port = args.port
    port.baudrate = args.baud
    port.timeout = 0.2
    port.dtr = False  # 打开端口前先拉低，防复位伪影
    port.rts = False
    try:
        port.open()
    except serial.SerialException as exc:
        sys.stderr.write(
            "error: 无法打开串口 %s（%s）。串口单进程独占——请先关闭串口监控 / 烧录工具，"
            "或确认设备已插好。\n" % (args.port, exc)
        )
        return 3

    try:
        port.reset_input_buffer()  # 丢弃打开瞬间的旧日志
        port.write((command + "\n").encode("ascii"))
        port.flush()
        payload = read_frame(port, args.timeout)
    finally:
        port.close()

    if payload is None:
        sys.stderr.write(
            "error: 超时未收到完整 KPLOG 帧（%.1fs）。设备是否在线 / 命令是否被识别？\n"
            % args.timeout
        )
        return 4

    text = payload.decode("utf-8", errors="replace")

    # 错误帧统一解析：KPLOG ERROR <code>（BEGIN 帧已 echo 码名，此处取 reason）
    for raw_line in text.splitlines():
        if raw_line.startswith(ERROR_MARK):
            code = raw_line[len(ERROR_MARK):].strip()
            sys.stderr.write("error: 设备返回错误帧：%s\n" % code)
            return 5

    if args.out:
        with open(args.out, "w", encoding="utf-8", errors="replace") as fh:
            fh.write(text)
        sys.stderr.write("已写入 %s（%d 字节）\n" % (args.out, len(text)))
    else:
        sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
