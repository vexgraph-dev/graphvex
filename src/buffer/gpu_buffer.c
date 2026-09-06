#include "buffer/gpu_buffer.h"

#include <string.h>

#include "nio/mem.h"
#include "graphvex/type.h"
#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: GpuBuffer (buffer/gpu_buffer.c)
 * LEVEL: L2 — Behavior (device buffer behavior API)
 * ============================================================================
 * Device-visible storage/uniform/vertex/index buffer with a CPU-shadow stub.
 * The struct owns a byte mirror of the device range so upload/download paths
 * stay testable with no GPU backend linked; a real driver wires the shadow
 * to mapped device memory later. Usage flags select the device role
 * (storage / uniform / vertex / index) and are opaque to this stub.
 *
 * STRUCT FIELDS (Mirroring buffer/gpu_buffer.h):
 * ----------------------------------------------------------------------------
 *   GpuBuffer {
 *     size_t size;      // device range length in bytes
 *     uint32_t usage;   // opaque device-role flags (storage/uniform/vertex/index)
 *     uint64_t typeId;  // block-header type id (TYPE_GPU_BUFFER_SINGLETON)
 *     uint8_t *shadow;  // owned CPU mirror, nullptr when empty
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - GpuBuffer()                : GpuBuffer_0()
 *   - GpuBuffer(size, usage)     : GpuBuffer_2(size, usage)
 *
 * Core Functions:
 *   - GpuBuffer_free(self)
 *   - GpuBuffer_upload(data, n, dest)
 *   - GpuBuffer_download(self, data, n)
 *
 * Setters:
 *   - GpuBuffer_setSize(self, size)
 *   - GpuBuffer_setUsage(self, usage)
 *
 * Getters:
 *   - GpuBuffer_getSize(self)
 *   - GpuBuffer_getUsage(self)
 * ============================================================================
 */

// gpu_buffer.c — device buffer with CPU-shadow stub implementation.

// CONSTRUCTORS
GpuBuffer *GpuBuffer_0(void) {
    GpuBuffer *self = (GpuBuffer*) Memory_alloc(TYPE_GPU_BUFFER_SINGLETON, sizeof(GpuBuffer));
    if (!self)
        return nullptr;
    (*self).size = 0;
    (*self).usage = 0;
    (*self).typeId = TYPE_GPU_BUFFER_SINGLETON;
    (*self).shadow = nullptr;
    return self;
}

GpuBuffer *GpuBuffer_2(size_t size, uint32_t usage) {
    GpuBuffer *self = (GpuBuffer*) Memory_alloc(TYPE_GPU_BUFFER_SINGLETON, sizeof(GpuBuffer));
    if (!self)
        return nullptr;
    (*self).size = 0;
    (*self).usage = usage;
    (*self).typeId = TYPE_GPU_BUFFER_SINGLETON;
    (*self).shadow = nullptr;
    if (size > 0) {
        uint8_t *shadow = (uint8_t*) Memory_alloc(TYPE_GPU_BUFFER_SINGLETON, size);
        if (!shadow) {
            Memory_free(self);
            return nullptr;
        }
        memset(shadow, 0, size);
        (*self).shadow = shadow;
        (*self).size = size;
    }
    return self;
}

// CORE FUNCTIONS
void GpuBuffer_free(GpuBuffer *self) {
    if (!self)
        return;
    uint8_t *shadow = (*self).shadow;
    if (shadow)
        Memory_free(shadow);
    Memory_free(self);
}

bool GpuBuffer_upload(const uint8_t *data, size_t n, GpuBuffer *dest) {
    if (!dest)
        return false;
    if (n == 0)
        return true;
    if (!data)
        return false;
    uint8_t *shadow = (*dest).shadow;
    if (!shadow)
        return false;
    if (n > (*dest).size)
        return false;
    memcpy(shadow, data, n);
    return true;
}

bool GpuBuffer_download(const GpuBuffer *self, uint8_t *data, size_t n) {
    if (!self)
        return false;
    if (n == 0)
        return true;
    if (!data)
        return false;
    uint8_t *shadow = (*self).shadow;
    if (!shadow)
        return false;
    if (n > (*self).size)
        return false;
    memcpy(data, shadow, n);
    return true;
}

// SETTERS
void GpuBuffer_setSize(GpuBuffer *self, size_t size) {
    if (!self)
        return;
    if (size == (*self).size)
        return;
    if (size == 0) {
        uint8_t *old = (*self).shadow;
        if (old)
            Memory_free(old);
        (*self).shadow = nullptr;
        (*self).size = 0;
        return;
    }
    uint8_t *old = (*self).shadow;
    if (!old) {
        uint8_t *fresh = (uint8_t*) Memory_alloc(TYPE_GPU_BUFFER_SINGLETON, size);
        if (!fresh)
            return;
        memset(fresh, 0, size);
        (*self).shadow = fresh;
        (*self).size = size;
        return;
    }
    uint8_t *grown = (uint8_t*) Memory_realloc(old, size);
    if (!grown)
        return;
    size_t kept = (*self).size;
    if (size > kept)
        memset(grown + kept, 0, size - kept);
    (*self).shadow = grown;
    (*self).size = size;
}

void GpuBuffer_setUsage(GpuBuffer *self, uint32_t usage) {
    if (!self)
        return;
    (*self).usage = usage;
}

// GETTERS
size_t GpuBuffer_getSize(const GpuBuffer *self) {
    return self ? (*self).size : 0;
}

uint32_t GpuBuffer_getUsage(const GpuBuffer *self) {
    return self ? (*self).usage : 0;
}
