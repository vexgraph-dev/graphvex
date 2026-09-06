#include "image/image.h"

#include <stdlib.h>
#include <string.h>

#include "nio/mem.h"
#include "oop/type.h"
#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: Image (image/image.c)
 * LEVEL: L2 — Behavior (GPU image behavior API, CPU-shadow stub)
 * ============================================================================
 * Backend-agnostic GPU image: successor to the Texture_* bindless registry
 * and VkIOSurface_* bridge concepts. No Vulkan includes here: pixels live
 * in an owned CPU RGBA8 shadow (width*height*4 bytes, zeroed = transparent
 * black) until a backend claims the handle. The struct rides the vexspoke
 * arena via Memory_alloc(TYPE_IMAGE_SINGLETON) with a calloc fallback for
 * standalone builds; the pixel shadow is always calloc-owned system memory.
 *
 * STRUCT FIELDS (Mirroring image/image.h):
 * ----------------------------------------------------------------------------
 *   Image {
 *     uint32_t width;   // pixels across (>= 1)
 *     uint32_t height;  // pixels down (>= 1)
 *     uint32_t format;  // backend-agnostic pixel format code (0 = RGBA8 stub default)
 *     uint32_t usage;   // backend-agnostic usage flags (0 = none)
 *     uint64_t typeId;  // block-header type id (TYPE_IMAGE_SINGLETON)
 *     uint8_t *rgba;    // OWNED CPU shadow, width*height*4 bytes RGBA8 (null = no backing); freed by Image_free, never borrowed
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - Image()                      : Image_0()
 *   - Image(w, h)                  : Image_2(w, h)
 *   - Image(w, h, format, usage)   : Image_4(w, h, format, usage)
 *
 * Core Functions:
 *   - Image_free(img)
 *   - Image_upload(rgba, w, h, dest)
 *
 * Setters:
 *   - Image_setWidth(img, w)
 *   - Image_setHeight(img, h)
 *   - Image_setFormat(img, format)
 *   - Image_setUsage(img, usage)
 *
 * Getters:
 *   - Image_getWidth(img)
 *   - Image_getHeight(img)
 *   - Image_getFormat(img)
 *   - Image_getUsage(img)
 * ============================================================================
 */

// image.c — Backend-agnostic GPU image implementation (CPU-shadow stub).

static bool imageByteCount(uint32_t w, uint32_t h, size_t *outBytes) {
    if (!outBytes)
        return false;
    size_t n = (size_t)w * (size_t)h;
    if (w != 0 && n / (size_t)w != (size_t)h)
        return false;
    if (n > SIZE_MAX / 4)
        return false;
    *outBytes = n * 4;
    return true;
}

static void imageFreeStorage(Image *img) {
    if (Memory_length(img) != 0)
        Memory_free(img);
    else
        free(img);
}

static void imageResize(Image *img, uint32_t w, uint32_t h) {
    size_t bytes = 0;
    if (!imageByteCount(w, h, &bytes))
        return;
    uint8_t *pixels = (uint8_t*) calloc(bytes, 1);
    if (!pixels)
        return;
    free((*img).rgba);
    (*img).rgba = pixels;
    (*img).width = w;
    (*img).height = h;
}

static Image *imageCreate(uint32_t w, uint32_t h, uint32_t format, uint32_t usage) {
    if (w == 0 || h == 0)
        return nullptr;
    size_t bytes = 0;
    if (!imageByteCount(w, h, &bytes))
        return nullptr;
    Image *img = (Image*) Memory_alloc(TYPE_IMAGE_SINGLETON, sizeof(Image));
    if (!img)
        img = (Image*) calloc(1, sizeof(Image));
    if (!img)
        return nullptr;
    uint8_t *pixels = (uint8_t*) calloc(bytes, 1);
    if (!pixels) {
        imageFreeStorage(img);
        return nullptr;
    }
    (*img).width = w;
    (*img).height = h;
    (*img).format = format;
    (*img).usage = usage;
    (*img).typeId = TYPE_IMAGE_SINGLETON;
    (*img).rgba = pixels;
    return img;
}

// CONSTRUCTORS
Image *Image_0(void) {
    return imageCreate(1, 1, 0, 0);
}

Image *Image_2(uint32_t w, uint32_t h) {
    return imageCreate(w, h, 0, 0);
}

Image *Image_4(uint32_t w, uint32_t h, uint32_t format, uint32_t usage) {
    return imageCreate(w, h, format, usage);
}

// CORE FUNCTIONS
void Image_free(Image *img) {
    if (!img)
        return;
    free((*img).rgba);
    (*img).rgba = nullptr;
    imageFreeStorage(img);
}

bool Image_upload(const uint8_t *rgba, uint32_t w, uint32_t h, Image *dest) {
    if (!rgba || !dest)
        return false;
    if (w == 0 || h == 0)
        return false;
    if (w != (*dest).width || h != (*dest).height || !(*dest).rgba)
        imageResize(dest, w, h);
    if (!(*dest).rgba || w != (*dest).width || h != (*dest).height)
        return false;
    memcpy((*dest).rgba, rgba, (size_t)w * (size_t)h * 4);
    return true;
}

// SETTERS
void Image_setWidth(Image *img, uint32_t w) {
    if (!img)
        return;
    if (w == 0 || w == (*img).width)
        return;
    imageResize(img, w, (*img).height);
}

void Image_setHeight(Image *img, uint32_t h) {
    if (!img)
        return;
    if (h == 0 || h == (*img).height)
        return;
    imageResize(img, (*img).width, h);
}

void Image_setFormat(Image *img, uint32_t format) {
    if (!img)
        return;
    (*img).format = format;
}

void Image_setUsage(Image *img, uint32_t usage) {
    if (!img)
        return;
    (*img).usage = usage;
}

// GETTERS
uint32_t Image_getWidth(const Image *img) {
    return img ? (*img).width : 0;
}

uint32_t Image_getHeight(const Image *img) {
    return img ? (*img).height : 0;
}

uint32_t Image_getFormat(const Image *img) {
    return img ? (*img).format : 0;
}

uint32_t Image_getUsage(const Image *img) {
    return img ? (*img).usage : 0;
}
