#include "annotation/overview.h"

#include <stdio.h>

#include "buffer/buffer.h"
#include "buffer/color_buffer.h"
#include "buffer/depth_buffer.h"

;;OVERVIEW
/**
 * ============================================================================
 * MODULE: BufferTest (tests/buffer_test.c — graphvex buffer smoke test)
 * LEVEL: L3 — Module Code (standalone verification harness)
 * ============================================================================
 * Relocated with the buffer/ move from vexspoke (ex-`anti` harness):
 * round-trips ColorBuffer RGBA and DepthBuffer float through set/get and
 * exits nonzero on any mismatch.
 *
 * STRUCT FIELDS: none — procedural (allocates, checks, frees, returns).
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Core Functions:
 *   - main(void)
 * ============================================================================
 */

int main(void) {
    int failures = 0;

    Buffer *colorBuf = ColorBuffer(64, 64);
    if (!colorBuf) {
        printf("FAIL: ColorBuffer alloc\n");
        return 1;
    }
    ColorBuffer_setRGBA(colorBuf, 10, 20, 255, 128, 64, 255);
    uint8_t cr = 0, cg = 0, cb = 0, ca = 0;
    ColorBuffer_getRGBA(colorBuf, 10, 20, &cr, &cg, &cb, &ca);
    printf("color[10,20] = rgba(%u,%u,%u,%u)\n", cr, cg, cb, ca);
    if (cr != 255 || cg != 128 || cb != 64 || ca != 255) {
        printf("FAIL: ColorBuffer round-trip\n");
        failures++;
    }
    Buffer_free(colorBuf);

    Buffer *depthBuf = DepthBuffer(64, 64);
    if (!depthBuf) {
        printf("FAIL: DepthBuffer alloc\n");
        return 1;
    }
    DepthBuffer_set(depthBuf, 10, 20, 0.75f);
    float d = DepthBuffer_get(depthBuf, 10, 20);
    printf("depth[10,20] = %.2f\n", (double)d);
    if (d < 0.74f || d > 0.76f) {
        printf("FAIL: DepthBuffer round-trip\n");
        failures++;
    }
    Buffer_free(depthBuf);

    if (failures == 0)
        printf("buffer_test: ok\n");
    return failures != 0;
}
