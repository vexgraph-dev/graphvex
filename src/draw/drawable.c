#include "draw/drawable.h"

#include <stdlib.h>

#include "nio/mem.h"
#include "oop/type.h"
#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: Drawable (draw/drawable.c)
 * LEVEL: L3 — Module Code (single-layer raster board behavior)
 * ============================================================================
 * Single-layer raster board owning one Image. CPU stub: every fill/draw
 * verb null-guards and marks the dirty flag without painting pixels yet;
 * actual GPU recording comes later. No Vulkan includes here.
 *
 * STRUCT FIELDS (Mirroring draw/drawable.h):
 * ----------------------------------------------------------------------------
 *   Drawable {
 *     Image *owned;    // OWNED raster backing (never borrowed); freed by Drawable_free
 *     uint64_t typeId; // block-header type id (TYPE_DRAWABLE_SINGLETON)
 *     bool dirty;      // true once any fill/draw/clear verb recorded or backing replaced
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - Drawable()       : Drawable_0()
 *   - Drawable(w, h)   : Drawable_2(w, h)
 *
 * Core Functions:
 *   - Drawable_fillRect(self, x, y, w, h, brush)
 *   - Drawable_drawRect(self, x, y, w, h, stroke)
 *   - Drawable_fillCircle(self, cx, cy, r, brush)
 *   - Drawable_drawCircle(self, cx, cy, r, stroke)
 *   - Drawable_clear(self, color)
 *   - Drawable_free(self)
 *
 * Setters:
 *   - Drawable_setImage(self, img)
 *   - Drawable_setDirty(self, dirty)
 *
 * Getters:
 *   - Drawable_getImage(self)
 *   - Drawable_isDirty(self)
 *   - Drawable_getWidth(self)
 *   - Drawable_getHeight(self)
 * ============================================================================
 */

// draw/drawable.c — Single-layer raster board implementation (CPU stub).

static void drawableFreeStorage(Drawable *self) {
    if (Memory_length(self) != 0)
        Memory_free(self);
    else
        free(self);
}

static Drawable *drawableCreate(uint32_t w, uint32_t h) {
    if (w == 0 || h == 0)
        return nullptr;
    Drawable *self = (Drawable*) Memory_alloc(TYPE_DRAWABLE_SINGLETON, sizeof(Drawable));
    if (!self)
        self = (Drawable*) calloc(1, sizeof(Drawable));
    if (!self)
        return nullptr;
    Image *backing = Image_2(w, h);
    if (!backing) {
        drawableFreeStorage(self);
        return nullptr;
    }
    (*self).owned = backing;
    (*self).typeId = TYPE_DRAWABLE_SINGLETON;
    (*self).dirty = false;
    return self;
}

// CONSTRUCTORS
Drawable *Drawable_0(void) {
    return drawableCreate(1, 1);
}

Drawable *Drawable_2(uint32_t w, uint32_t h) {
    return drawableCreate(w, h);
}

// CORE FUNCTIONS
void Drawable_fillRect(Drawable *self, float x, float y, float w, float h, const Brush *brush) {
    if (!self)
        return;
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)brush;
    (*self).dirty = true;
}

void Drawable_drawRect(Drawable *self, float x, float y, float w, float h, const Stroke *stroke) {
    if (!self)
        return;
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)stroke;
    (*self).dirty = true;
}

void Drawable_fillCircle(Drawable *self, float cx, float cy, float r, const Brush *brush) {
    if (!self)
        return;
    (void)cx;
    (void)cy;
    (void)r;
    (void)brush;
    (*self).dirty = true;
}

void Drawable_drawCircle(Drawable *self, float cx, float cy, float r, const Stroke *stroke) {
    if (!self)
        return;
    (void)cx;
    (void)cy;
    (void)r;
    (void)stroke;
    (*self).dirty = true;
}

void Drawable_clear(Drawable *self, uint32_t color) {
    if (!self)
        return;
    (void)color;
    (*self).dirty = true;
}

void Drawable_free(Drawable *self) {
    if (!self)
        return;
    Image_free((*self).owned);
    (*self).owned = nullptr;
    drawableFreeStorage(self);
}

// SETTERS
void Drawable_setImage(Drawable *self, Image *img) {
    if (!self)
        return;
    if (img == (*self).owned)
        return;
    Image_free((*self).owned);
    (*self).owned = img;
    (*self).dirty = true;
}

void Drawable_setDirty(Drawable *self, bool dirty) {
    if (!self)
        return;
    (*self).dirty = dirty;
}

// GETTERS
Image *Drawable_getImage(const Drawable *self) {
    return self ? (*self).owned : nullptr;
}

bool Drawable_isDirty(const Drawable *self) {
    return self ? (*self).dirty : false;
}

uint32_t Drawable_getWidth(const Drawable *self) {
    if (!self)
        return 0;
    return Image_getWidth((*self).owned);
}

uint32_t Drawable_getHeight(const Drawable *self) {
    if (!self)
        return 0;
    return Image_getHeight((*self).owned);
}
