/*
 * Frozen acceptance tests — TaskPackage 260829-host-test-infra (subsystem: firmware).
 * Host test infrastructure bootstrap package (mergeOrder 2).
 *
 * Nature: HOST-SIDE unit smoke (ESP-IDF linux target / component unit-test convention,
 * NOT on-target device tests). These TEST_CASEs run on the PC as a native executable via
 * the ESP-IDF unity runner. See Red baseline + run protocol below.
 *
 * Scope discipline: this file must NOT pass vacuously. Every INF-A2 / INF-A3 assertion
 * holds a concrete REAL result. The builder wires the infra (linux target + unity test
 * build + LVGL SDL host backend + render seam); this file is the frozen contract that
 * wiring must satisfy. Implementation of the infra body itself is NOT this file's job.
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> assertion):
 *   coverage: INF-A1  structural   -> this file exists at firmware/main/test/ + coverage map
 *   coverage: INF-A2  host runner  -> test_host_runner_executes_sample_logic
 *   coverage: INF-A3  host render  -> test_host_sdl_renders_frame_to_virtual_screen
 *   coverage: INF-A4  conventions  -> comment-registered checkpoint block below (verifier-manual)
 *   boundary          -> test_host_render_seam_signals_nothing_rendered
 *
 * ---------------------------------------------------------------------------
 * RED baseline (2026-09-04 acceptance-author 实测):
 *   - firmware/main/test/ does NOT exist (this file is the first / no test build).
 *   - sdkconfig.defaults* carries only chip targets (esp32/esp32c3/esp32c5/esp32c6/
 *     esp32p4/esp32s3) — no linux target config.
 *   - No CMakeLists wiring under main/test/ (no unity test app build configured).
 *   - LVGL SDL host backend seam (host_infra_render_frame / host_infra_read_pixel) absent.
 *   Because the host test build as a whole does not exist, this file cannot be built or
 *   run -> guaranteed UNIFIED RED until the builder closes it. No per-assertion sensitivity,
 *   all four TEST_CASE entries fail for the same missing-behaviour reason (infra absent);
 *   closeout MUST be mutation-checked (break a real behavior, expect red) to stay non-vacuous.
 *
 * Environment precondition for builder/verifier (probed 2026-09-04): ESP-IDF v5.5.1 is
 * installed at C:\esp\v5.5.1\esp-idf with the ESP-IDF tools under %USERPROFILE%\.espressif
 * (idf-env.json). toolchain-linux.cmake is PRESENT in the framework -> linux target is
 * supported. BUT the IDF env is not on PATH this session: `idf.py` / `export.sh` must be
 * sourced from C:\esp\v5.5.1\esp-idf\export.sh AND `idf.py fullclean` run for the target
 * switch to take effect. The red baseline below is therefore "environment not activated +
 * infra absent", NOT a false red from a broken test.
 *
 * Run protocol (expected, final authority is the directory-convention doc, INF-E2/INF-A4):
 *     idf.py --preview set-target linux
 *     idf.py -T main build            # builds the main component unity test app
 *     <build>/<project>.<runner>      # run the native unity runner binary; must exit 0
 *                                     # and report "run: 3 tests" / all passed
 *   Every TEST_CASE below must execute and pass; the unity runner must exit 0.
 */

#include <unity.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* --------------------------------------------------------------------------
 * INF-A2 — host single-test runner compiles on linux target AND executes a real
 * sample logic assertion. This TEST_CASE is the "at least one sample assertion
 * passes" proof. It uses only plain C so it must build under the unity test app
 * once the builder wires the host test build; it asserts concrete computed values
 * (not "no error"), so a wrong result fails the run. NOTE: this case is target-
 * agnostic; it is intentionally minimal so it stays buildable as a pure-logic
 * smoke for later logic packages (2/3/4/5/6/8) to hang their own TEST_CASEs off.
 * ------------------------------------------------------------------------ */
static uint32_t host_infra_bit_reverse_byte(uint32_t b)
{
    /* Defensive invariant: this helper only ever reverses one byte of state. A real
     * fail-stop guard (not just "no error"): feeding >0xFF would be a caller bug and
     * the acceptance run must not silently pass it. */
    assert(b <= 0xFFu);
    /* sbox-free, deterministic pure logic -> real result to assert, not a mock. */
    uint32_t out = 0;
    for (uint32_t i = 0; i < 8; i++) {
        if (b & (1u << i)) {
            out |= (uint32_t)(0x80u >> i);
        }
    }
    return out;
}

TEST_CASE("host infra: host unity runner executes sample logic (INF-A2)", "[host_infra][INF-A2]")
{
    /* Real, concrete computed values: 0b00000001 -> 0b10000000, 0b10000001 -> 0b10000001. */
    TEST_ASSERT_EQUAL_UINT32(0x80u, host_infra_bit_reverse_byte(0x01u));
    TEST_ASSERT_EQUAL_UINT32(0x01u, host_infra_bit_reverse_byte(0x80u));
    TEST_ASSERT_EQUAL_UINT32(0xFFu, host_infra_bit_reverse_byte(0xFFu));
    TEST_ASSERT_EQUAL_UINT32(0x42u, host_infra_bit_reverse_byte(0x42u)); /* 0100_0010 <-> 0100_0010 */

    /* A distinct full cycle proves the runner really looped and really ran 8 iterations:
     * the value that takes a known non-trivial round trip must round-trip. */
    const uint32_t v = 0x7Bu; /* 0111_1011 */
    TEST_ASSERT_EQUAL_UINT32(v, host_infra_bit_reverse_byte(host_infra_bit_reverse_byte(v)));

    /* Memory sanity: a small deterministic buffer transform must hold (real data path on host).
     * NOTE: use a NAMED array, not a compound literal — unity's
     * TEST_ASSERT_EQUAL_UINT8_ARRAY is a 3-arg macro, and `{...}` is not a preprocessor
     * argument group (only `(...)` is), so an inline literal would be split into 6 args. */
    uint8_t buf[4] = {1, 2, 3, 4};
    const uint8_t expected_row[4] = {65, 66, 67, 68};
    for (size_t i = 0; i < sizeof(buf) / sizeof(buf[0]); i++) {
        buf[i] = (uint8_t)(buf[i] + 64u);
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_row, buf, 4);
}

/* --------------------------------------------------------------------------
 * INF-A3 — LVGL SDL host backend starts on the PC and renders ONE FRAME to a
 * virtual screen. "Compiles" is NOT enough: a real frame with a known color must
 * be produced and probed. The builder owns the seam below (owned symbols, named
 * per whatever the directory-convention doc fixes; this file pins the CONTRACT).
 *
 * Seam contract (builder must provide, for the linux target test build only):
 *   int  host_infra_render_frame(void);
 *        Init LVGL SDL (software/offscreen renderer — a headless run must not
 *        require a physical window), draw one full-frame solid fill in the color
 *        INF_RENDER_COLOR, flush ONE frame. Return 0 == a frame was flushed;
 *        non-zero == failed to render.
 *   int32_t host_infra_read_pixel(int x, int y);
 *        Return the currently rendered framebuffer pixel at (x,y) packed 0xRRGGBB,
 *        or -1 (HOST_INFRA_NOT_RENDERED sentinel) if NO frame has been flushed yet.
 *
 * Because these symbols are only declared under CONFIG_IDF_TARGET_LINUX, they can
 * never accidentally compile into a device target build. Before the builder wires
 * them there is no symbol -> the test app does not link -> red (infra absent).
 * ------------------------------------------------------------------------ */
#if defined(CONFIG_IDF_TARGET_LINUX)
#define HOST_INFRA_NOT_RENDERED     ((int32_t)-1)
#define HOST_INFRA_FRAME_W          (400)
#define HOST_INFRA_FRAME_H          (300)
#define HOST_INFRA_COLOR            ((uint32_t)0x336699u) /* RRGGBB, distinctive fill */

/* Seam prototypes (declared here; defined by builder wiring -> see doc). */
int     host_infra_render_frame(void);
int32_t host_infra_read_pixel(int x, int y);

TEST_CASE("host infra: LVGL SDL renders a real frame to virtual screen (INF-A3)",
          "[host_infra][INF-A3][linux]")
{
    /* 1) Render one frame; the seam must report success. */
    TEST_ASSERT_EQUAL_INT(0, host_infra_render_frame());

    /* 2) Probe the rendered framebuffer — must NOT be the "nothing rendered" sentinel,
     *    proving a frame actually reached the virtual screen (non-vacuous). */
    const uint32_t expected = HOST_INFRA_COLOR;
    TEST_ASSERT_NOT_EQUAL(HOST_INFRA_NOT_RENDERED, host_infra_read_pixel(0, 0));

    /* 3) The whole frame must be a real, known solid fill — probe center + all four
     *    corners + two interior points. Any pixel differing proves no real full-frame
     *    render happened -> red. */
    TEST_ASSERT_EQUAL_UINT32(expected, (uint32_t)host_infra_read_pixel(0, 0));
    TEST_ASSERT_EQUAL_UINT32(expected, (uint32_t)host_infra_read_pixel(HOST_INFRA_FRAME_W - 1, 0));
    TEST_ASSERT_EQUAL_UINT32(expected, (uint32_t)host_infra_read_pixel(0, HOST_INFRA_FRAME_H - 1));
    TEST_ASSERT_EQUAL_UINT32(expected, (uint32_t)host_infra_read_pixel(HOST_INFRA_FRAME_W - 1,
                                                                       HOST_INFRA_FRAME_H - 1));
    TEST_ASSERT_EQUAL_UINT32(expected, (uint32_t)host_infra_read_pixel(HOST_INFRA_FRAME_W / 2,
                                                                       HOST_INFRA_FRAME_H / 2));
    TEST_ASSERT_EQUAL_UINT32(expected, (uint32_t)host_infra_read_pixel(HOST_INFRA_FRAME_W / 4,
                                                                       (3 * HOST_INFRA_FRAME_H) / 4));
}

TEST_CASE("host infra: render seam must distinguish nothing-rendered from a real frame (boundary)",
          "[host_infra][boundary][linux]")
{
    /* Boundary / error-path guard (acceptance-evidence "happy-path only" + vacuity guard):
     * the seam must expose the not-rendered state as a DISTINCT sentinel, so the tolerance
     * in INF-A3 cannot be cheated by always returning a static color. Read before any frame
     * is flushed must yield the sentinel (-1), i.e. the failure event is real and detectable. */
    TEST_ASSERT_EQUAL_INT32(HOST_INFRA_NOT_RENDERED, host_infra_read_pixel(0, 0));

    /* And after a fresh render the same read must be an actual color (not the sentinel),
     * i.e. the two states are observably different. Re-render then probe. */
    TEST_ASSERT_EQUAL_INT(0, host_infra_render_frame());
    TEST_ASSERT_NOT_EQUAL(HOST_INFRA_NOT_RENDERED, host_infra_read_pixel(0, 0));
}
#endif /* CONFIG_IDF_TARGET_LINUX */

/* ---------------------------------------------------------------------------
 * INF-A4 — host test directory convention (placement + naming + run command
 * documented). STRUCTURAL / DOC — cannot be asserted at runtime here; these
 * checkpoints are registered for VERIFIER-manual execution at closeout, per
 * planner intake (INF-E2 compatibility matrix also lands in the same doc).
 *
 * Checkpoints that MUST be true at closeout (verifier verifies, not the runner):
 *   [C4.1] A host-test directory-convention doc EXISTS under firmware/ that a
 *         future logic package can follow (planner intake names it the host test
 *         directory-convention document; INF-E2's host-vs-device compile matrix
 *         must land in the SAME doc, "不预判结论", measured by builder — see INF-E2).
 *   [C4.2] That doc records TEST-CASE PLACEMENT: e.g. firmware/main/test/ plus any
 *         per-module <subsystem>/test dirs, and how a package hangs its own
 *         TEST_CASE next to this smoke file.
 *   [C4.3] That doc records NAMING CONVENTION: this file's *.acceptance.c suffix +
 *         `[INF-x]` tag style is the first froze example; doc states the convention
 *         so later packages match it.
 *   [C4.4] That doc records the RUN COMMAND for the host test suite (expected shape:
 *         `idf.py --preview set-target linux` + `idf.py -T main build` + run the
 *         native unity runner; final exact form is per builder's wire-up).
 *   [C4.5] INF-E2 host-vs-device compile matrix is IN that doc and honestly reflects
 *         what the builder actually measured for ESP-SR AFE etc. (not a pre-assumed
 *         conclusion, per planner intake note ⑥).
 * Actual path/name of the doc is decided by the builder during wire-up; verifier
 * records where it lives. This file does not enforce a path, but the doc MUST exist.
 * --------------------------------------------------------------------------- */

/* The ESP-IDF unity test framework supplies the runner main() when this file is
 * compiled under a component test app; no app_main() is provided here. */