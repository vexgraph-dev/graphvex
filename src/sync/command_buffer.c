#include "sync/command_buffer.h"

#include <stdlib.h>

#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: CommandBuffer (sync/command_buffer.c)
 * LEVEL: L2 — Behavior (CPU-side command record lifecycle stubs)
 * ============================================================================
 * Per-thread command record handle. A CommandBuffer is owned by exactly one
 * thread for its whole lifetime and must never be shared across threads.
 * CPU-side stubs only: begin/end/reset/submit track the recording flag and
 * touch no Vulkan/Metal/Direct backend.
 *
 * STRUCT FIELDS (Mirroring sync/command_buffer.h):
 * ----------------------------------------------------------------------------
 *   CommandBuffer {
 *     uint32_t queueFamily; // target queue family (COMMAND_FAMILY_* constant)
 *     bool recording; // true between begin and end
 *     uint64_t typeId; // reserved stub type stamp (0 until registered)
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - CommandBuffer()                : CommandBuffer_0()
 *   - CommandBuffer(family)          : CommandBuffer_1(family)
 *
 * Core Functions:
 *   - CommandBuffer_begin(self)
 *   - CommandBuffer_end(self)
 *   - CommandBuffer_reset(self)
 *   - CommandBuffer_submit(self, queue)
 *   - CommandBuffer_free(self)
 *
 * Setters:
 *   - CommandBuffer_setFamily(self, family)
 *   - CommandBuffer_setRecording(self, recording)
 *
 * Getters:
 *   - CommandBuffer_getFamily(self)
 *   - CommandBuffer_getRecording(self)
 *   - CommandBuffer_isRecording(self)
 *   - CommandBuffer_getTypeId(self)
 * ============================================================================
 */


// CONSTRUCTORS

CommandBuffer *CommandBuffer_0(void) {
    return CommandBuffer_1(COMMAND_FAMILY_GRAPHICS);
}

CommandBuffer *CommandBuffer_1(uint32_t family) {
    CommandBuffer *self = (CommandBuffer*) calloc(1, sizeof(CommandBuffer));
    if (!self)
        return nullptr;
    if (family > COMMAND_FAMILY_COPY)
        family = COMMAND_FAMILY_GRAPHICS;
    (*self).queueFamily = family;
    (*self).recording = false;
    (*self).typeId = 0;
    return self;
}

// CORE FUNCTIONS

bool CommandBuffer_begin(CommandBuffer *self) {
    if (!self)
        return false;
    if ((*self).recording)
        return false;
    (*self).recording = true;
    return true;
}

bool CommandBuffer_end(CommandBuffer *self) {
    if (!self)
        return false;
    if (!(*self).recording)
        return false;
    (*self).recording = false;
    return true;
}

bool CommandBuffer_reset(CommandBuffer *self) {
    if (!self)
        return false;
    (*self).recording = false;
    return true;
}

bool CommandBuffer_submit(CommandBuffer *self, struct CommandQueue *queue) {
    if (!self)
        return false;
    if (!queue)
        return false;
    return true;
}

void CommandBuffer_free(CommandBuffer *self) {
    if (!self)
        return;
    free(self);
}

// SETTERS

void CommandBuffer_setFamily(CommandBuffer *self, uint32_t family) {
    if (!self)
        return;
    if (family > COMMAND_FAMILY_COPY)
        family = COMMAND_FAMILY_GRAPHICS;
    (*self).queueFamily = family;
}

void CommandBuffer_setRecording(CommandBuffer *self, bool recording) {
    if (!self)
        return;
    (*self).recording = recording;
}

// GETTERS

uint32_t CommandBuffer_getFamily(const CommandBuffer *self) {
    if (!self)
        return COMMAND_FAMILY_GRAPHICS;
    return (*self).queueFamily;
}

bool CommandBuffer_getRecording(const CommandBuffer *self) {
    if (!self)
        return false;
    return (*self).recording;
}

bool CommandBuffer_isRecording(const CommandBuffer *self) {
    return CommandBuffer_getRecording(self);
}

uint64_t CommandBuffer_getTypeId(const CommandBuffer *self) {
    if (!self)
        return 0;
    return (*self).typeId;
}
