#include "scene/pipeline.h"

#include <stdlib.h>
#include <string.h>

#include "annotation/overview.h"
#include "nio/mem.h"
#include "oop/type.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: Pipeline (scene/pipeline.c)
 * LEVEL: L2 — Behavior (compiled raster and compute state)
 * ============================================================================
 * Compiled raster and compute state (shaders, blend modes, depth states).
 * Backend agnostic.
 *
 * STRUCT FIELDS (Mirroring scene/pipeline.h):
 * ----------------------------------------------------------------------------
 *   Pipeline {
 *     uint32_t stageMask;     // pipeline execution stage mask
 *     uint32_t blendMode;     // color blending configuration mode
 *     uint32_t cullMode;      // face culling mode flags
 *     bool depthTest;         // true if depth testing enabled
 *     bool depthWrite;        // true if depth writing enabled
 *     char shaderName[64];    // shader name or entry point identifier
 *     uint64_t typeId;        // block-header type id (TYPE_PIPELINE_SINGLETON)
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - Pipeline()                           : Pipeline_0()
 *   - Pipeline(shaderName)                 : Pipeline_1(shaderName)
 *
 * Core Functions:
 *   - Pipeline_free(self)
 *
 * Setters:
 *   - Pipeline_setStageMask(self, stageMask)
 *   - Pipeline_setBlendMode(self, blendMode)
 *   - Pipeline_setCullMode(self, cullMode)
 *   - Pipeline_setDepthTest(self, depthTest)
 *   - Pipeline_setDepthWrite(self, depthWrite)
 *   - Pipeline_setShaderName(self, shaderName)
 *   - Pipeline_setTypeId(self, typeId)
 *
 * Getters:
 *   - Pipeline_getStageMask(self)
 *   - Pipeline_getBlendMode(self)
 *   - Pipeline_getCullMode(self)
 *   - Pipeline_isDepthTest(self)
 *   - Pipeline_isDepthWrite(self)
 *   - Pipeline_getShaderName(self)
 *   - Pipeline_getTypeId(self)
 * ============================================================================
 */

// CONSTRUCTORS

Pipeline *Pipeline_0(void) {
    Pipeline *self = (Pipeline*) Memory_alloc(TYPE_PIPELINE_SINGLETON, sizeof(Pipeline));
    if (!self)
        self = (Pipeline*) calloc(1, sizeof(Pipeline));
    if (!self)
        return nullptr;
    (*self).stageMask = 0;
    (*self).blendMode = 0;
    (*self).cullMode = 0;
    (*self).depthTest = false;
    (*self).depthWrite = false;
    (*self).shaderName[0] = '\0';
    (*self).typeId = TYPE_PIPELINE_SINGLETON;
    return self;
}

Pipeline *Pipeline_1(const char *shaderName) {
    Pipeline *self = Pipeline_0();
    if (!self)
        return nullptr;
    Pipeline_setShaderName(self, shaderName);
    return self;
}

// CORE FUNCTIONS

void Pipeline_free(Pipeline *self) {
    if (!self)
        return;
    if (Memory_length(self) != 0)
        Memory_free(self);
    else
        free(self);
}

// SETTERS

void Pipeline_setStageMask(Pipeline *self, uint32_t stageMask) {
    if (!self)
        return;
    (*self).stageMask = stageMask;
}

void Pipeline_setBlendMode(Pipeline *self, uint32_t blendMode) {
    if (!self)
        return;
    (*self).blendMode = blendMode;
}

void Pipeline_setCullMode(Pipeline *self, uint32_t cullMode) {
    if (!self)
        return;
    (*self).cullMode = cullMode;
}

void Pipeline_setDepthTest(Pipeline *self, bool depthTest) {
    if (!self)
        return;
    (*self).depthTest = depthTest;
}

void Pipeline_setDepthWrite(Pipeline *self, bool depthWrite) {
    if (!self)
        return;
    (*self).depthWrite = depthWrite;
}

void Pipeline_setShaderName(Pipeline *self, const char *shaderName) {
    if (!self)
        return;
    if (!shaderName) {
        (*self).shaderName[0] = '\0';
        return;
    }
    strncpy((*self).shaderName, shaderName, sizeof((*self).shaderName) - 1);
    (*self).shaderName[sizeof((*self).shaderName) - 1] = '\0';
}

void Pipeline_setTypeId(Pipeline *self, uint64_t typeId) {
    if (!self)
        return;
    (*self).typeId = typeId;
}

// GETTERS

uint32_t Pipeline_getStageMask(const Pipeline *self) {
    return self ? (*self).stageMask : 0;
}

uint32_t Pipeline_getBlendMode(const Pipeline *self) {
    return self ? (*self).blendMode : 0;
}

uint32_t Pipeline_getCullMode(const Pipeline *self) {
    return self ? (*self).cullMode : 0;
}

bool Pipeline_isDepthTest(const Pipeline *self) {
    return self ? (*self).depthTest : false;
}

bool Pipeline_isDepthWrite(const Pipeline *self) {
    return self ? (*self).depthWrite : false;
}

const char *Pipeline_getShaderName(const Pipeline *self) {
    return self ? (*self).shaderName : nullptr;
}

uint64_t Pipeline_getTypeId(const Pipeline *self) {
    return self ? (*self).typeId : 0;
}
