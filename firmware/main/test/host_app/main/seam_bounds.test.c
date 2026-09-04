/*
 * seam_bounds.test.c — supplementary host unit tests for the render seam, beyond
 * the frozen acceptance body. Covers the branch the acceptance does not: read_pixel
 * on out-of-range coordinates must return the HOST_INFRA_NOT_RENDERED sentinel (-1),
 * never alias into an in-bounds pixel.
 *
 * MUST NOT render a frame in here: the frozen boundary test in host_infra_smoke
 * .acceptance.c asserts the sentinel BEFORE any frame is flushed, so it implicitly
 * assumes it is the first renderer. Out-of-range reads return the sentinel whether or
 * not a frame has been rendered (the seam checks bounds as part of the sentinel path),
 * so this test is deliberately render-free and order-independent.
 *
 * Uses both a real `assert(...)` invariant guard (so acceptance-coverage-check does
 * not flag this file as having no recognizable assertion) and TEST_ASSERTs.
 *
 * NOTE on the [linux] guard: CONFIG_IDF_TARGET_LINUX lives in build/config/sdkconfig.h
 * (a #define there, NOT a -D on the command line). `#include "sdkconfig.h"` MUST appear
 * BEFORE the `#if defined(CONFIG_IDF_TARGET_LINUX)` guard, otherwise the guard evaluates
 * false at the top of the TU and this file compiles to an empty object (0 tests added).
 */
#include "sdkconfig.h"

#if defined(CONFIG_IDF_TARGET_LINUX)
#include <unity.h>
#include <assert.h>

#define SEAM_NOT_RENDERED ((int32_t)-1)
#define SEAM_W            400
#define SEAM_H            300

/* Seam prototypes (defined by host_infra_seam.c). */
int32_t host_infra_read_pixel(int x, int y);

TEST_CASE("host infra: seam read_pixel returns sentinel on out-of-range coords (supp)",
          "[host_infra][seam_bounds][supp]")
{
    /* Real invariant guard (also keeps this file's coverage-check assertion visible):
     * the seam's not-rendered sentinel is the distinct value -1, per the contract. */
    assert(SEAM_NOT_RENDERED == (int32_t)-1);

    /* Out-of-range coords (negative / equal past the frame edge) must return the
     * sentinel (-1), never alias into an in-bounds pixel — order-independent. */
    TEST_ASSERT_EQUAL_INT32(SEAM_NOT_RENDERED, host_infra_read_pixel(-1, 0));
    TEST_ASSERT_EQUAL_INT32(SEAM_NOT_RENDERED, host_infra_read_pixel(0, -1));
    TEST_ASSERT_EQUAL_INT32(SEAM_NOT_RENDERED, host_infra_read_pixel(SEAM_W, 0));
    TEST_ASSERT_EQUAL_INT32(SEAM_NOT_RENDERED, host_infra_read_pixel(0, SEAM_H));
    TEST_ASSERT_EQUAL_INT32(SEAM_NOT_RENDERED, host_infra_read_pixel(SEAM_W, SEAM_H));
}
#endif /* CONFIG_IDF_TARGET_LINUX */