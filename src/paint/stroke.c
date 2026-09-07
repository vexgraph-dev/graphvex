#include "paint/stroke.h"

#include <stdlib.h>

#include "nio/mem.h"
#include "oop/type.h"
#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: Stroke (paint/stroke.c)
 * LEVEL: L2 — Behavior (plain-data stroke style API)
 * ============================================================================
 * Plain-data stroke style: width + dash + cap + join + color. No GPU
 * handles, no owned pointers — the struct rides the vexspoke arena via
 * Memory_alloc(TYPE_STROKE_SINGLETON) with a calloc fallback for
 * standalone builds. Vector pipelines copy the record; nothing is borrowed.
 *
 * STRUCT FIELDS (Mirroring paint/stroke.h):
 * ----------------------------------------------------------------------------
 *   Stroke {
 *     float width;      // stroke width in pixels (>= 0; 0 = hairline)
 *     float dash;       // dash segment length in pixels (0 = solid)
 *     uint32_t cap;     // line cap (STROKE_CAP_*)
 *     uint32_t join;    // line join (STROKE_JOIN_*)
 *     uint32_t color;   // packed color (0xAARRGGBB)
 *     uint64_t typeId;  // block-header type id (TYPE_STROKE_SINGLETON)
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - Stroke()                 : Stroke_0()
 *   - Stroke(width, color)     : Stroke_2(width, color)
 *
 * Core Functions:
 *   - Stroke_free(self)
 *
 * Setters:
 *   - Stroke_setWidth(self, width)
 *   - Stroke_setDash(self, dash)
 *   - Stroke_setCap(self, cap)
 *   - Stroke_setJoin(self, join)
 *   - Stroke_setColor(self, color)
 *
 * Getters:
 *   - Stroke_getWidth(self)
 *   - Stroke_getDash(self)
 *   - Stroke_getCap(self)
 *   - Stroke_getJoin(self)
 *   - Stroke_getColor(self)
 *   - Stroke_getTypeId(self)
 * ============================================================================
 */

// paint/stroke.c — Plain-data stroke style implementation.

// CONSTRUCTORS

Stroke *Stroke_0(void) {
    return Stroke_2(1.0f, 0xFF000000u);
}

Stroke *Stroke_2(float width, uint32_t color) {
    Stroke *self = (Stroke*) Memory_alloc(TYPE_STROKE_SINGLETON, sizeof(Stroke));
    if (!self)
        self = (Stroke*) calloc(1, sizeof(Stroke));
    if (!self)
        return nullptr;
    (*self).width = width;
    (*self).dash = 0.0f;
    (*self).cap = STROKE_CAP_BUTT;
    (*self).join = STROKE_JOIN_MITER;
    (*self).color = color;
    (*self).typeId = TYPE_STROKE_SINGLETON;
    return self;
}

// CORE FUNCTIONS

void Stroke_free(Stroke *self) {
    if (!self)
        return;
    if (Memory_length(self) != 0)
        Memory_free(self);
    else
        free(self);
}

// SETTERS

void Stroke_setWidth(Stroke *self, float width) {
    if (!self)
        return;
    (*self).width = width;
}

void Stroke_setDash(Stroke *self, float dash) {
    if (!self)
        return;
    (*self).dash = dash;
}

void Stroke_setCap(Stroke *self, uint32_t cap) {
    if (!self)
        return;
    (*self).cap = cap;
}

void Stroke_setJoin(Stroke *self, uint32_t join) {
    if (!self)
        return;
    (*self).join = join;
}

void Stroke_setColor(Stroke *self, uint32_t color) {
    if (!self)
        return;
    (*self).color = color;
}

// GETTERS

float Stroke_getWidth(const Stroke *self) {
    return self ? (*self).width : 0.0f;
}

float Stroke_getDash(const Stroke *self) {
    return self ? (*self).dash : 0.0f;
}

uint32_t Stroke_getCap(const Stroke *self) {
    return self ? (*self).cap : 0u;
}

uint32_t Stroke_getJoin(const Stroke *self) {
    return self ? (*self).join : 0u;
}

uint32_t Stroke_getColor(const Stroke *self) {
    return self ? (*self).color : 0u;
}

uint64_t Stroke_getTypeId(const Stroke *self) {
    return self ? (*self).typeId : 0;
}
