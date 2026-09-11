#include "image/frame_importer.h"

#include <stdlib.h>

#include "nio/mem.h"
#include "graphvex/type.h"
#include "annotation/intention.h"
#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: FrameImporter (image/frame_importer.c)
 * LEVEL: L2 — Behavior (R1.5 decode seam: pure bytes→Image upload)
 * ============================================================================
 * Pure bytes→Image upload for the R1.5 layer. Takes caller-decoded RGBA8
 * frames and lands them in a backend-agnostic Image via Image_upload.
 * Owns NO decoder and spawns NOTHING: the CALLER runs the external
 * decoder binary through the vexspoke ProcessSpawn shape and hands raw
 * frames here. No popen, no libav* include/link, no threads, and never
 * called from tick/render paths — frame decode is caller-scheduled work
 * outside the steady-state loop (zero steady-state allocation here).
 *
 * STRUCT FIELDS (Mirroring image/frame_importer.h — exactly this
 * file's class):
 * ----------------------------------------------------------------------------
 *   FrameImporter {
 *     uint32_t lastWidth;   // width of the last accepted frame (0 = none)
 *     uint32_t lastHeight;  // height of the last accepted frame (0 = none)
 *     uint64_t typeId;      // block-header type id (TYPE_FRAME_IMPORTER_SINGLETON)
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - FrameImporter()  : FrameImporter_0()
 *
 * Core Functions:
 *   - FrameImporter_free(self)
 *   - FrameImporter_rgbaToTexture(self, w, h, bytes, dest)
 *
 * Setters:
 *   - FrameImporter_setLastWidth(self, w)
 *   - FrameImporter_setLastHeight(self, h)
 *
 * Getters:
 *   - FrameImporter_getLastWidth(self)
 *   - FrameImporter_getLastHeight(self)
 * ============================================================================
 */
;;INTENTION("decode via binary spawn, no libav link: caller decodes with an external binary through ProcessSpawn, this class only uploads raw RGBA8")

// image/frame_importer.c — FrameImporter port. Upload-only seam; the
// decoder lives in the caller's process table, never here.

// CONSTRUCTORS
FrameImporter *FrameImporter_0(void) {
    FrameImporter *self = (FrameImporter*) Memory_alloc(TYPE_FRAME_IMPORTER_SINGLETON, sizeof(FrameImporter));
    if (!self)
        self = (FrameImporter*) calloc(1, sizeof(FrameImporter));
    if (!self)
        return nullptr;
    (*self).lastWidth = 0;
    (*self).lastHeight = 0;
    (*self).typeId = TYPE_FRAME_IMPORTER_SINGLETON;
    return self;
}

// CORE FUNCTIONS
void FrameImporter_free(FrameImporter *self) {
    if (!self)
        return;
    if (Memory_length(self) != 0)
        Memory_free(self);
    else
        free(self);
}

bool FrameImporter_rgbaToTexture(FrameImporter *self, uint32_t w, uint32_t h,
                                 const uint8_t *bytes, Image *dest) {
    if (!self || !bytes || !dest)
        return false;
    if (w == 0 || h == 0)
        return false;
    if (!Image_upload(bytes, w, h, dest))
        return false;
    (*self).lastWidth = w;
    (*self).lastHeight = h;
    return true;
}

// SETTERS
void FrameImporter_setLastWidth(FrameImporter *self, uint32_t w) {
    if (!self)
        return;
    (*self).lastWidth = w;
}

void FrameImporter_setLastHeight(FrameImporter *self, uint32_t h) {
    if (!self)
        return;
    (*self).lastHeight = h;
}

// GETTERS
uint32_t FrameImporter_getLastWidth(const FrameImporter *self) {
    if (!self)
        return 0;
    return (*self).lastWidth;
}

uint32_t FrameImporter_getLastHeight(const FrameImporter *self) {
    if (!self)
        return 0;
    return (*self).lastHeight;
}
