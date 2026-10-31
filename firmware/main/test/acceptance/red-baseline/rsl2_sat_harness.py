#!/usr/bin/env python3
# RSL2 bidirectional-satisfiability harness (acceptance-author, 2026-10-28) —
# script file form (scope: firmware/**/acceptance/**; same home as the red
# wrapper family). Builds synthetic trees under /mnt/c/tmp/rsl2_work (NEVER
# touches the worktree), applies the one-shot reference implementation of the
# M1/M2/M3 seams + targeted mutations, compiles the frozen acceptance file
# standalone (KIDPAL_REPO_ROOT injection) and reports per-variant pass/fail.
# Usage: wsl -d Ubuntu-24.04 -- python3 firmware/main/test/acceptance/red-baseline/rsl2_sat_harness.py
import os, shutil, subprocess, sys

ROOT = '/mnt/c/AI/KidPal-pod-m2'
WORK = '/mnt/c/tmp/rsl2_work'
RB   = ROOT + '/firmware/main/test/acceptance/red-baseline'
ACC  = 'firmware/main/test/record_session_lifecycle.acceptance.c'

COPY = [
    'firmware/main/display/kidpal_render_node.cc',
    'firmware/main/application.cc',
    'firmware/main/mcp_server.cc',
    ACC,
    'firmware/main/test/host_app/main/CMakeLists.txt',
    'firmware/main/display/story_page.h',
    'firmware/main/key_gesture_parser.h',
]

def read(p):
    with open(p, 'rb') as f:
        return f.read()

def write(p, b):
    os.makedirs(os.path.dirname(p), exist_ok=True)
    with open(p, 'wb') as f:
        f.write(b)

def sub1(data, old, new):
    """bytes replace tolerant to CRLF/LF; returns None if anchor missing."""
    for o, n in ((old, new),
                 (old.replace(b'\n', b'\r\n'), new.replace(b'\n', b'\r\n'))):
        if o in data:
            return data.replace(o, n, 1)
    return None

REF_IMPL = r'''/* one-shot reference impl mirroring display/story_page.c [R2] frozen semantics
 * (green-tree satisfiability harness only; never written to the worktree). */
int kidpal_story_session_end(int page_state, int *out_page)
{
    if (out_page == 0) { return -1; }
    if (page_state == 2) { *out_page = 0; return 1; }
    return 0;
}
'''

MCP_SYNC_DEF = (b'void KidpalStorySessionEndSync() { '
                b'kidpal_render_node_story_session_page(g_story_view.page); }\n')

def build(name, m1=False, m2=False, m3=False, reg=True, mut=None):
    d = os.path.join(WORK, 'tree-' + name)
    shutil.rmtree(d, ignore_errors=True)
    for rel in COPY:
        write(os.path.join(d, rel), read(os.path.join(ROOT, rel)))
    write(os.path.join(d, 'firmware/main/CMakeLists.txt'), b'# root probe\n')

    rn = read(os.path.join(d, 'firmware/main/display/kidpal_render_node.cc'))
    app = read(os.path.join(d, 'firmware/main/application.cc'))
    mcp = read(os.path.join(d, 'firmware/main/mcp_server.cc'))
    cmk = read(os.path.join(d, 'firmware/main/test/host_app/main/CMakeLists.txt'))

    if m1:
        anchor = b'if (v != NULL) { memcpy(&s_story_view, v, sizeof(s_story_view)); }'
        rn = sub1(rn, anchor, anchor + b'\n    s_story_session_page = v->page;   /* M1 mirror follows authoritative view page */')
        assert rn is not None, 'M1 anchor missing'

    if m2:
        a = b'int KidpalStoryViewPageState() {'
        mcp = sub1(mcp, a, MCP_SYNC_DEF + b'\n' + a)
        assert mcp is not None
        a = (b'                    KidpalStorySessionEndApply(story_end_page);\n'
             b'                }\n'
             b'            }')
        app = sub1(app, a, a + b'\n            KidpalStorySessionEndSync();')
        assert app is not None, 'M2 app anchor missing'

    if m3:
        # scope injections to HandleStartListeningEvent (the file has another
        # structurally identical WifiConfiguring branch at :993 — the frozen
        # acceptance file slices by function window for the same reason)
        hsl = app.index(b'void Application::HandleStartListeningEvent(')
        head, tail = app[:hsl], app[hsl:]
        for marker in (b'state == kDeviceStateWifiConfiguring) {\r\n',
                       b'state == kDeviceStateWifiConfiguring) {\n'):
            if marker in tail:
                tail = tail.replace(marker, marker + b'        kidpal_story_session_ctx_set(0);\n', 1)
                break
        a = b'ESP_LOGE(TAG, "Protocol not initialized");'
        tail = sub1(tail, a, a + b'\n        kidpal_story_session_ctx_set(0);')
        assert tail is not None
        a = b'ESP_LOGI(TAG, "AutoConnect in-flight, ignore manual start listening");'
        tail = sub1(tail, a, a + b'\n        kidpal_story_session_ctx_set(0);')
        assert tail is not None
        app = head + tail

    # ---- targeted mutations (each = single deviation from green tree) ----
    if mut == 'extra_assign':
        a = b'int page = s_story_session_page;'
        rn = sub1(rn, a, a + b' s_story_session_page = 3;')
        assert rn is not None
    elif mut == 'sync_after_voice_end':
        a = b'            kidpal_render_node_voice_end();'
        app = sub1(app, a, a + b'\n            KidpalStorySessionEndSync();')
        assert app is not None
        # drop the in-order call inserted by m2 (keep only the late one)
        for pat in (b'\n            KidpalStorySessionEndSync();\n',
                    b'\r\n            KidpalStorySessionEndSync();\r\n'):
            if pat in app:
                app = app.replace(pat, b'\n', 1)
                break
        assert app.count(b'KidpalStorySessionEndSync();') == 1, 'late-only call expected'
    elif mut == 'sync_calls_cache':
        mcp = mcp.replace(MCP_SYNC_DEF,
                          b'void KidpalStorySessionEndSync() { '
                          b'kidpal_render_node_cache_story(&g_story_view, &g_story_list, &g_story_text); }\n', 1)
    elif mut == 'omit_autoclient_clear':
        for nl in (b'\n', b'\r\n'):
            a = b'ESP_LOGI(TAG, "AutoConnect in-flight, ignore manual start listening");' + nl + b'        kidpal_story_session_ctx_set(0);'
            if a in app:
                app = app.replace(a, b'ESP_LOGI(TAG, "AutoConnect in-flight, ignore manual start listening");', 1)
                break
    elif mut == 'unregistered':
        reg = False

    if reg:
        a = b'"../../host_infra_smoke.acceptance.c"'
        cmk = sub1(cmk, a, b'"../../record_session_lifecycle.acceptance.c"\n    ' + a)
        assert cmk is not None

    write(os.path.join(d, 'firmware/main/display/kidpal_render_node.cc'), rn)
    write(os.path.join(d, 'firmware/main/application.cc'), app)
    write(os.path.join(d, 'firmware/main/mcp_server.cc'), mcp)
    write(os.path.join(d, 'firmware/main/test/host_app/main/CMakeLists.txt'), cmk)
    return d

def run_variant(d):
    exe = '/tmp/rsl2_sat_runner'
    cmd = ['gcc', '-std=c11', '-Wall', '-Wextra', '-I', RB, '-o', exe,
           os.path.join(d, ACC), os.path.join(RB, 'runner.c'),
           os.path.join(WORK, 'ref_impl.c')]
    c = subprocess.run(cmd, capture_output=True, text=True)
    if c.returncode != 0:
        return ('COMPILE_FAIL', c.stderr[:800])
    r = subprocess.run([exe], capture_output=True, text=True,
                       cwd='/mnt/c/tmp', env={**os.environ, 'KIDPAL_REPO_ROOT': d})
    lines = [l for l in r.stdout.strip().splitlines() if l.strip()]
    return ('RUN', r.returncode, lines[-1] if lines else '', r.stdout)

def main():
    write(os.path.join(WORK, 'ref_impl.c'), REF_IMPL.encode())
    variants = [
        ('green',            dict(m1=True, m2=True, m3=True),                          '0 Failures'),
        ('mut-no-m1',        dict(m2=True, m3=True),                                   'TEST 1 red'),
        ('mut-extra-assign', dict(m1=True, m2=True, m3=True, mut='extra_assign'),      'TEST 1 red'),
        ('mut-sync-late',    dict(m1=True, m3=True, mut='sync_after_voice_end'),       'TEST 2 red'),
        ('mut-sync-cache',   dict(m1=True, m2=True, m3=True, mut='sync_calls_cache'),  'TEST 2 red'),
        ('mut-omit-clear',   dict(m1=True, m2=True, m3=True, mut='omit_autoclient_clear'), 'TEST 3 red'),
        ('mut-unregistered', dict(m1=True, m2=True, m3=True, mut='unregistered'),      'TEST 5 red'),
    ]
    ok = True
    for name, kw, expect in variants:
        d = build(name, **kw)
        res = run_variant(d)
        if res[0] == 'RUN':
            print('== variant %-18s expect=%-12s RUN exit=%d  %s' % (name, expect, res[1], res[2]))
            for line in res[3].splitlines():
                if 'FAIL:' in line:
                    print('    ', line.strip()[:160])
        else:
            print('== variant %-18s expect=%-12s %s: %s' % (name, expect, res[0], res[1]))
        if (expect == '0 Failures' and not (res[0] == 'RUN' and res[1] == 0)) or \
           (expect != '0 Failures' and not (res[0] == 'RUN' and res[1] != 0)):
            ok = False
            print('    *** UNEXPECTED (expect %s) ***' % expect)
    print('SAT-HARNESS', 'OK' if ok else 'MISMATCH')
    return 0 if ok else 1

if __name__ == '__main__':
    sys.exit(main())
