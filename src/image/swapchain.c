#include "image/swapchain.h"

#include <stdlib.h>

#include "nio/mem.h"
#include "oop/type.h"
#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: Swapchain (image/swapchain.c)
 * LEVEL: L4 — Self-Management (display surface lifecycle and presentation)
 * ============================================================================
 * Backend-agnostic swapchain managing a pool of 1 to 4 CPU shadow framebuffer
 * images for presentation to a native display surface (NSWindow, CAMetalLayer,
 * etc.). Supports double/triple buffering, bounded acquire/present cadences,
 * and dynamic surface resize. The struct rides the vexspoke arena via
 * Memory_alloc(TYPE_SWAPCHAIN_SINGLETON) with a calloc fallback for standalone
 * builds. Framebuffer images are owned Image instances.
 *
 * STRUCT FIELDS (Mirroring image/swapchain.h):
 * ----------------------------------------------------------------------------
 *   Swapchain {
 *     void *nativeHandle;    // borrowed native handle (NSWindow, CAMetalLayer, etc.)
 *     uint32_t width;        // swapchain surface width in pixels
 *     uint32_t height;       // swapchain surface height in pixels
 *     uint32_t imageCount;   // buffer count (1 to 4)
 *     uint32_t currentIndex; // current acquired/active image index
 *     uint64_t typeId;       // block-header type id (TYPE_SWAPCHAIN_SINGLETON)
 *     bool vsync;            // vertical sync enabled flag
 *     Image *images[4];      // owned CPU shadow framebuffer images
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - Swapchain()                                   : Swapchain_0()
 *   - Swapchain(nativeHandle)                       : Swapchain_1(nativeHandle)
 *   - Swapchain(nativeHandle, w, h)                 : Swapchain_3(nativeHandle, w, h)
 *   - Swapchain(nativeHandle, w, h, count)          : Swapchain_4(nativeHandle, w, h, count)
 *
 * Core Functions:
 *   - Swapchain_free(self)
 *   - Swapchain_acquire(self)
 *   - Swapchain_present(self)
 *   - Swapchain_resize(self, w, h)
 *   - Swapchain_currentImage(self)
 *   - Swapchain_getImage(self, index)
 *
 * Setters:
 *   - Swapchain_setWidth(self, width)
 *   - Swapchain_setHeight(self, height)
 *   - Swapchain_setImageCount(self, count)
 *   - Swapchain_setCurrentIndex(self, index)
 *   - Swapchain_setVsync(self, vsync)
 *   - Swapchain_setNativeHandle(self, nativeHandle)
 *   - Swapchain_setTypeId(self, typeId)
 *
 * Getters:
 *   - Swapchain_getWidth(self)
 *   - Swapchain_getHeight(self)
 *   - Swapchain_getImageCount(self)
 *   - Swapchain_getCurrentIndex(self)
 *   - Swapchain_isVsync(self)
 *   - Swapchain_getNativeHandle(self)
 *   - Swapchain_getTypeId(self)
 * ============================================================================
 */

// image/swapchain.c — Backend-agnostic presentation swapchain implementation.

// CONSTRUCTORS

Swapchain *Swapchain_0(void) {
    return Swapchain_4(nullptr, 1, 1, 2);
}

Swapchain *Swapchain_1(void *nativeHandle) {
    return Swapchain_4(nativeHandle, 1, 1, 2);
}

Swapchain *Swapchain_3(void *nativeHandle, uint32_t w, uint32_t h) {
    return Swapchain_4(nativeHandle, w, h, 2);
}

Swapchain *Swapchain_4(void *nativeHandle, uint32_t w, uint32_t h, uint32_t count) {
    if (count < 1)
        count = 1;
    if (count > 4)
        count = 4;
    Swapchain *self = (Swapchain*) Memory_alloc(TYPE_SWAPCHAIN_SINGLETON, sizeof(Swapchain));
    if (!self)
        self = (Swapchain*) calloc(1, sizeof(Swapchain));
    if (!self)
        return nullptr;
    (*self).nativeHandle = nativeHandle;
    (*self).width = w;
    (*self).height = h;
    (*self).imageCount = count;
    (*self).currentIndex = 0;
    (*self).typeId = TYPE_SWAPCHAIN_SINGLETON;
    (*self).vsync = true;
    for (uint32_t i = 0; i < 4; i++) {
        if (i < count && w > 0 && h > 0) {
            (*self).images[i] = Image_2(w, h);
            if (!(*self).images[i]) {
                Swapchain_free(self);
                return nullptr;
            }
        } else {
            (*self).images[i] = nullptr;
        }
    }
    return self;
}

// CORE FUNCTIONS

void Swapchain_free(Swapchain *self) {
    if (!self)
        return;
    for (uint32_t i = 0; i < 4; i++) {
        if ((*self).images[i]) {
            Image_free((*self).images[i]);
            (*self).images[i] = nullptr;
        }
    }
    if (Memory_length(self) != 0)
        Memory_free(self);
    else
        free(self);
}

int32_t Swapchain_acquire(Swapchain *self) {
    if (!self || (*self).imageCount == 0)
        return -1;
    if ((*self).currentIndex >= (*self).imageCount)
        (*self).currentIndex = 0;
    return (int32_t) (*self).currentIndex;
}

bool Swapchain_present(Swapchain *self) {
    if (!self || (*self).imageCount == 0)
        return false;
    (*self).currentIndex = ((*self).currentIndex + 1) % (*self).imageCount;
    return true;
}

bool Swapchain_resize(Swapchain *self, uint32_t w, uint32_t h) {
    if (!self || w == 0 || h == 0)
        return false;
    if ((*self).width == w && (*self).height == h)
        return true;
    Image *newImages[4] = { nullptr, nullptr, nullptr, nullptr };
    for (uint32_t i = 0; i < (*self).imageCount; i++) {
        newImages[i] = Image_2(w, h);
        if (!newImages[i]) {
            for (uint32_t j = 0; j < i; j++)
                Image_free(newImages[j]);
            return false;
        }
    }
    for (uint32_t i = 0; i < (*self).imageCount; i++) {
        if ((*self).images[i])
            Image_free((*self).images[i]);
        (*self).images[i] = newImages[i];
    }
    (*self).width = w;
    (*self).height = h;
    (*self).currentIndex = 0;
    return true;
}

Image *Swapchain_currentImage(Swapchain *self) {
    if (!self || (*self).imageCount == 0)
        return nullptr;
    if ((*self).currentIndex >= (*self).imageCount)
        return nullptr;
    return (*self).images[(*self).currentIndex];
}

Image *Swapchain_getImage(const Swapchain *self, uint32_t index) {
    if (!self || index >= (*self).imageCount || index >= 4)
        return nullptr;
    return (*self).images[index];
}

// SETTERS

void Swapchain_setWidth(Swapchain *self, uint32_t width) {
    if (!self)
        return;
    if (width != (*self).width && (*self).height > 0 && width > 0)
        Swapchain_resize(self, width, (*self).height);
    else
        (*self).width = width;
}

void Swapchain_setHeight(Swapchain *self, uint32_t height) {
    if (!self)
        return;
    if (height != (*self).height && (*self).width > 0 && height > 0)
        Swapchain_resize(self, (*self).width, height);
    else
        (*self).height = height;
}

void Swapchain_setImageCount(Swapchain *self, uint32_t count) {
    if (!self)
        return;
    if (count < 1)
        count = 1;
    if (count > 4)
        count = 4;
    if (count == (*self).imageCount)
        return;
    if (count < (*self).imageCount) {
        for (uint32_t i = count; i < (*self).imageCount; i++) {
            if ((*self).images[i]) {
                Image_free((*self).images[i]);
                (*self).images[i] = nullptr;
            }
        }
    } else {
        for (uint32_t i = (*self).imageCount; i < count; i++) {
            if ((*self).width > 0 && (*self).height > 0)
                (*self).images[i] = Image_2((*self).width, (*self).height);
            else
                (*self).images[i] = nullptr;
        }
    }
    (*self).imageCount = count;
    if ((*self).currentIndex >= count)
        (*self).currentIndex = 0;
}

void Swapchain_setCurrentIndex(Swapchain *self, uint32_t index) {
    if (!self)
        return;
    if ((*self).imageCount > 0 && index < (*self).imageCount)
        (*self).currentIndex = index;
}

void Swapchain_setVsync(Swapchain *self, bool vsync) {
    if (!self)
        return;
    (*self).vsync = vsync;
}

void Swapchain_setNativeHandle(Swapchain *self, void *nativeHandle) {
    if (!self)
        return;
    (*self).nativeHandle = nativeHandle;
}

void Swapchain_setTypeId(Swapchain *self, uint64_t typeId) {
    if (!self)
        return;
    (*self).typeId = typeId;
}

// GETTERS

uint32_t Swapchain_getWidth(const Swapchain *self) {
    return self ? (*self).width : 0;
}

uint32_t Swapchain_getHeight(const Swapchain *self) {
    return self ? (*self).height : 0;
}

uint32_t Swapchain_getImageCount(const Swapchain *self) {
    return self ? (*self).imageCount : 0;
}

uint32_t Swapchain_getCurrentIndex(const Swapchain *self) {
    return self ? (*self).currentIndex : 0;
}

bool Swapchain_isVsync(const Swapchain *self) {
    return self ? (*self).vsync : false;
}

void *Swapchain_getNativeHandle(const Swapchain *self) {
    return self ? (*self).nativeHandle : nullptr;
}

uint64_t Swapchain_getTypeId(const Swapchain *self) {
    return self ? (*self).typeId : 0;
}
