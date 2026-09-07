#include "paint/brush.h"

#include <stdlib.h>

#include "nio/mem.h"
#include "oop/type.h"
#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: Brush (paint/brush.c)
 * LEVEL: L2 — Behavior (base solid brush API)
 * ============================================================================
 * Base solid brush: color + opacity. Embed-first base record —
 * VectorBrush/RasterBrush declare a Brush as their first member (owned
 * elsewhere, never created here). No GPU handles: pipelines resolve the
 * solid fill from color * opacity. The struct rides the vexspoke arena
 * via Memory_alloc(TYPE_BRUSH_SINGLETON) with a calloc fallback for
 * standalone builds.
 *
 * STRUCT FIELDS (Mirroring paint/brush.h):
 * ----------------------------------------------------------------------------
 *   Brush {
 *     uint32_t color;   // packed base color (0xAARRGGBB)
 *     float opacity;    // master alpha multiplier [0..1]
 *     uint64_t typeId;  // block-header type id (TYPE_BRUSH_SINGLETON)
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - Brush()                  : Brush_0()
 *   - Brush(color, opacity)    : Brush_2(color, opacity)
 *
 * Core Functions:
 *   - Brush_free(self)
 *
 * Setters:
 *   - Brush_setColor(self, color)
 *   - Brush_setOpacity(self, opacity)
 *
 * Getters:
 *   - Brush_getColor(self)
 *   - Brush_getOpacity(self)
 *   - Brush_getTypeId(self)
 * ============================================================================
 */

// paint/brush.c — Base solid brush implementation.

// CONSTRUCTORS

Brush *Brush_0(void) {
    return Brush_2(0xFF000000u, 1.0f);
}

Brush *Brush_2(uint32_t color, float opacity) {
    Brush *self = (Brush*) Memory_alloc(TYPE_BRUSH_SINGLETON, sizeof(Brush));
    if (!self)
        self = (Brush*) calloc(1, sizeof(Brush));
    if (!self)
        return nullptr;
    (*self).color = color;
    (*self).opacity = opacity;
    (*self).typeId = TYPE_BRUSH_SINGLETON;
    return self;
}

// CORE FUNCTIONS

void Brush_free(Brush *self) {
    if (!self)
        return;
    if (Memory_length(self) != 0)
        Memory_free(self);
    else
        free(self);
}

// SETTERS

void Brush_setColor(Brush *self, uint32_t color) {
    if (!self)
        return;
    (*self).color = color;
}

void Brush_setOpacity(Brush *self, float opacity) {
    if (!self)
        return;
    (*self).opacity = opacity;
}

// GETTERS

uint32_t Brush_getColor(const Brush *self) {
    return self ? (*self).color : 0u;
}

float Brush_getOpacity(const Brush *self) {
    return self ? (*self).opacity : 0.0f;
}

uint64_t Brush_getTypeId(const Brush *self) {
    return self ? (*self).typeId : 0;
}
