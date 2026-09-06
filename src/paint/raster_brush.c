#include "paint/raster_brush.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "nio/mem.h"
#include "oop/type.h"
#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: RasterBrush (paint/raster_brush.c)
 * LEVEL: L2 — Behavior (stamp brush behavior API)
 * ============================================================================
 * Stamp brush: an embedded 32x32 R8 SDF tip (shape-only mask, 255 = ink)
 * plus per-dab dynamics (size, stamp alpha, spacing, jitter, softness).
 * Embeds Brush first so a RasterBrush upcasts to Brush by address. Tint
 * comes from the base color; effective alpha is base opacity times stamp
 * opacity (setters keep both in sync). tipImage is an optional borrowed
 * textured-tip override (never freed, null = procedural tip). Tips bake
 * once with pure CPU distance math; no GPU handles live here. The struct
 * rides the vexspoke arena via Memory_alloc(TYPE_RASTER_BRUSH_SINGLETON)
 * with a calloc fallback for standalone builds.
 *
 * STRUCT FIELDS (Mirroring paint/raster_brush.h):
 * ----------------------------------------------------------------------------
 *   RasterBrush {
 *     // --- RasterBrush core (owner fields: base first for upcast) ---
 *     Brush base;              // embed-first base (color + opacity + typeId)
 *     // --- Tip part (embedded 32x32 R8 SDF stamp, shape-only) ---
 *     uint8_t tip[1024];       // 32x32 R8 mask bytes, row-major, 255 = ink
 *     // --- Dynamics part (per-dab stamp controls) ---
 *     float size;              // stamp diameter in pixels (> 0)
 *     float opacity;           // per-dab stamp alpha [0..1], mirrors base opacity
 *     float spacing;           // dab advance as fraction of size (0.25 = smooth)
 *     float jitter;            // positional jitter amount [0..1] (0 = off)
 *     float softness;          // stamping edge softness [0..1]
 *     // --- Tip image view (borrowed override, detach-only) ---
 *     Image *tipImage;         // borrowed textured tip (null = procedural); never freed
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - RasterBrush()                : RasterBrush_0()
 *   - RasterBrush(preset)          : RasterBrush_1(preset)
 *
 * Core Functions:
 *   - RasterBrush_free(self)
 *   - RasterBrush_makeRound(dest)
 *   - RasterBrush_makeSoft(dest)
 *   - RasterBrush_makeSquare(dest)
 *   - RasterBrush_asBrush(self)
 *   - RasterBrush_constBrush(self)
 *
 * Setters:
 *   - RasterBrush_setColor(self, color)
 *   - RasterBrush_setSize(self, size)
 *   - RasterBrush_setOpacity(self, opacity)
 *   - RasterBrush_setSpacing(self, spacing)
 *   - RasterBrush_setJitter(self, jitter)
 *   - RasterBrush_setSoftness(self, softness)
 *   - RasterBrush_setTipPixel(self, x, y, v)
 *   - RasterBrush_setTip(src1024, dest)
 *   - RasterBrush_setTipImage(self, img)
 *
 * Getters:
 *   - RasterBrush_getColor(self)
 *   - RasterBrush_getTypeId(self)
 *   - RasterBrush_getSize(self)
 *   - RasterBrush_getOpacity(self)
 *   - RasterBrush_getSpacing(self)
 *   - RasterBrush_getJitter(self)
 *   - RasterBrush_getSoftness(self)
 *   - RasterBrush_getTipPixel(self, x, y)
 *   - RasterBrush_tip(self)
 *   - RasterBrush_constTip(self)
 *   - RasterBrush_getTipImage(self)
 *   - RasterBrush_getTip(self, out1024)
 * ============================================================================
 */

// paint/raster_brush.c — Stamp brush implementation.

static void rasterBrushFreeStorage(RasterBrush *self) {
    if (Memory_length(self) != 0)
        Memory_free(self);
    else
        free(self);
}

static void rasterBrushInitScalars(RasterBrush *self) {
    Brush *b = &(*self).base;
    (*b).color = 0xFF000000u;
    (*b).opacity = 1.0f;
    (*b).typeId = TYPE_RASTER_BRUSH_SINGLETON;
    (*self).size = 16.0f;
    (*self).opacity = 1.0f;
    (*self).spacing = 0.25f;
    (*self).jitter = 0.0f;
    (*self).softness = 0.5f;
    (*self).tipImage = nullptr;
}

static uint8_t edgeToAlpha(float t) {
    if (t >= 0.5f)
        return 255u;
    if (t <= -0.5f)
        return 0u;
    return (uint8_t) ((t + 0.5f) * 255.0f);
}

static void bakeRound(RasterBrush *dest) {
    float cx = 15.5f;
    float cy = 15.5f;
    float radius = 15.0f;
    for (uint32_t y = 0; y < RASTER_BRUSH_TIP_DIM; y++) {
        for (uint32_t x = 0; x < RASTER_BRUSH_TIP_DIM; x++) {
            float dx = (float) x - cx;
            float dy = (float) y - cy;
            float dist = sqrtf(dx * dx + dy * dy);
            float t = radius - dist;
            (*dest).tip[y * RASTER_BRUSH_TIP_DIM + x] = edgeToAlpha(t);
        }
    }
}

static void bakeSoft(RasterBrush *dest) {
    float cx = 15.5f;
    float cy = 15.5f;
    float radius = 15.0f;
    for (uint32_t y = 0; y < RASTER_BRUSH_TIP_DIM; y++) {
        for (uint32_t x = 0; x < RASTER_BRUSH_TIP_DIM; x++) {
            float dx = (float) x - cx;
            float dy = (float) y - cy;
            float dist = sqrtf(dx * dx + dy * dy);
            float d = dist / radius;
            float f = 1.0f - d;
            if (f < 0.0f)
                f = 0.0f;
            f = f * f;
            (*dest).tip[y * RASTER_BRUSH_TIP_DIM + x] = (uint8_t) (f * 255.0f);
        }
    }
}

static void bakeSquare(RasterBrush *dest) {
    float cx = 15.5f;
    float cy = 15.5f;
    float half = 15.0f;
    for (uint32_t y = 0; y < RASTER_BRUSH_TIP_DIM; y++) {
        for (uint32_t x = 0; x < RASTER_BRUSH_TIP_DIM; x++) {
            float dx = fabsf((float) x - cx);
            float dy = fabsf((float) y - cy);
            float d = dx > dy ? dx : dy;
            float t = half - d;
            (*dest).tip[y * RASTER_BRUSH_TIP_DIM + x] = edgeToAlpha(t);
        }
    }
}

// CONSTRUCTORS

RasterBrush *RasterBrush_0(void) {
    return RasterBrush_1(RASTER_BRUSH_ROUND);
}

RasterBrush *RasterBrush_1(uint32_t preset) {
    RasterBrush *self = (RasterBrush*) Memory_alloc(TYPE_RASTER_BRUSH_SINGLETON, sizeof(RasterBrush));
    if (!self)
        self = (RasterBrush*) calloc(1, sizeof(RasterBrush));
    if (!self)
        return nullptr;
    rasterBrushInitScalars(self);
    if (preset == RASTER_BRUSH_SOFT)
        bakeSoft(self);
    else if (preset == RASTER_BRUSH_SQUARE)
        bakeSquare(self);
    else
        bakeRound(self);
    return self;
}

// CORE FUNCTIONS

void RasterBrush_free(RasterBrush *self) {
    if (!self)
        return;
    (*self).tipImage = nullptr;
    rasterBrushFreeStorage(self);
}

void RasterBrush_makeRound(RasterBrush *dest) {
    if (!dest)
        return;
    bakeRound(dest);
}

void RasterBrush_makeSoft(RasterBrush *dest) {
    if (!dest)
        return;
    bakeSoft(dest);
}

void RasterBrush_makeSquare(RasterBrush *dest) {
    if (!dest)
        return;
    bakeSquare(dest);
}

Brush *RasterBrush_asBrush(RasterBrush *self) {
    return (Brush*) self;
}

const Brush *RasterBrush_constBrush(const RasterBrush *self) {
    return (const Brush*) self;
}

// SETTERS

void RasterBrush_setColor(RasterBrush *self, uint32_t color) {
    if (!self)
        return;
    Brush *b = &(*self).base;
    (*b).color = color;
}

void RasterBrush_setSize(RasterBrush *self, float size) {
    if (!self)
        return;
    (*self).size = size;
}

void RasterBrush_setOpacity(RasterBrush *self, float opacity) {
    if (!self)
        return;
    (*self).opacity = opacity;
    Brush *b = &(*self).base;
    (*b).opacity = opacity;
}

void RasterBrush_setSpacing(RasterBrush *self, float spacing) {
    if (!self)
        return;
    (*self).spacing = spacing;
}

void RasterBrush_setJitter(RasterBrush *self, float jitter) {
    if (!self)
        return;
    (*self).jitter = jitter;
}

void RasterBrush_setSoftness(RasterBrush *self, float softness) {
    if (!self)
        return;
    (*self).softness = softness;
}

void RasterBrush_setTipPixel(RasterBrush *self, uint32_t x, uint32_t y, uint8_t v) {
    if (!self)
        return;
    if (x >= RASTER_BRUSH_TIP_DIM || y >= RASTER_BRUSH_TIP_DIM)
        return;
    (*self).tip[y * RASTER_BRUSH_TIP_DIM + x] = v;
}

void RasterBrush_setTip(const uint8_t *src1024, RasterBrush *dest) {
    if (!src1024 || !dest)
        return;
    memcpy((*dest).tip, src1024, RASTER_BRUSH_TIP_BYTES);
}

void RasterBrush_setTipImage(RasterBrush *self, Image *img) {
    if (!self)
        return;
    (*self).tipImage = img;
}

// GETTERS

uint32_t RasterBrush_getColor(const RasterBrush *self) {
    if (!self)
        return 0u;
    const Brush *b = &(*self).base;
    return (*b).color;
}

uint64_t RasterBrush_getTypeId(const RasterBrush *self) {
    if (!self)
        return 0;
    const Brush *b = &(*self).base;
    return (*b).typeId;
}

float RasterBrush_getSize(const RasterBrush *self) {
    return self ? (*self).size : 0.0f;
}

float RasterBrush_getOpacity(const RasterBrush *self) {
    return self ? (*self).opacity : 0.0f;
}

float RasterBrush_getSpacing(const RasterBrush *self) {
    return self ? (*self).spacing : 0.0f;
}

float RasterBrush_getJitter(const RasterBrush *self) {
    return self ? (*self).jitter : 0.0f;
}

float RasterBrush_getSoftness(const RasterBrush *self) {
    return self ? (*self).softness : 0.0f;
}

uint8_t RasterBrush_getTipPixel(const RasterBrush *self, uint32_t x, uint32_t y) {
    if (!self || x >= RASTER_BRUSH_TIP_DIM || y >= RASTER_BRUSH_TIP_DIM)
        return 0u;
    return (*self).tip[y * RASTER_BRUSH_TIP_DIM + x];
}

uint8_t *RasterBrush_tip(RasterBrush *self) {
    if (!self)
        return nullptr;
    return (*self).tip;
}

const uint8_t *RasterBrush_constTip(const RasterBrush *self) {
    if (!self)
        return nullptr;
    return (*self).tip;
}

Image *RasterBrush_getTipImage(const RasterBrush *self) {
    return self ? (*self).tipImage : nullptr;
}

void RasterBrush_getTip(const RasterBrush *self, uint8_t *out1024) {
    if (!self || !out1024)
        return;
    memcpy(out1024, (*self).tip, RASTER_BRUSH_TIP_BYTES);
}
