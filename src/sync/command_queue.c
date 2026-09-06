#include "sync/command_queue.h"

#include <stdlib.h>

#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * CLASS: CommandQueue (sync/command_queue.c)
 * LEVEL: L2 — Behavior (CPU-side queue-family handle stubs)
 * ============================================================================
 * Queue-family handle for one GRAPHICS/COMPUTE/COPY family. CPU-side stubs
 * only: begin/end/reset/submit validate handles and touch no
 * Vulkan/Metal/Direct backend.
 *
 * STRUCT FIELDS (Mirroring sync/command_queue.h):
 * ----------------------------------------------------------------------------
 *   CommandQueue {
 *     uint32_t family; // queue family (COMMAND_FAMILY_* constant)
 *     uint64_t typeId; // reserved stub type stamp (0 until registered)
 *   }
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Constructors:
 *   - CommandQueue()                 : CommandQueue_0()
 *   - CommandQueue(family)           : CommandQueue_1(family)
 *
 * Core Functions:
 *   - CommandQueue_begin(self)
 *   - CommandQueue_end(self)
 *   - CommandQueue_reset(self)
 *   - CommandQueue_submit(self, cmd)
 *   - CommandQueue_free(self)
 *
 * Setters:
 *   - CommandQueue_setFamily(self, family)
 *
 * Getters:
 *   - CommandQueue_getFamily(self)
 *   - CommandQueue_getTypeId(self)
 * ============================================================================
 */


// CONSTRUCTORS

CommandQueue *CommandQueue_0(void) {
    return CommandQueue_1(COMMAND_FAMILY_GRAPHICS);
}

CommandQueue *CommandQueue_1(uint32_t family) {
    CommandQueue *self = (CommandQueue*) calloc(1, sizeof(CommandQueue));
    if (!self)
        return nullptr;
    if (family > COMMAND_FAMILY_COPY)
        family = COMMAND_FAMILY_GRAPHICS;
    (*self).family = family;
    (*self).typeId = 0;
    return self;
}

// CORE FUNCTIONS

bool CommandQueue_begin(CommandQueue *self) {
    if (!self)
        return false;
    return true;
}

bool CommandQueue_end(CommandQueue *self) {
    if (!self)
        return false;
    return true;
}

bool CommandQueue_reset(CommandQueue *self) {
    if (!self)
        return false;
    return true;
}

bool CommandQueue_submit(CommandQueue *self, const struct CommandBuffer *cmd) {
    if (!self)
        return false;
    if (!cmd)
        return false;
    return true;
}

void CommandQueue_free(CommandQueue *self) {
    if (!self)
        return;
    free(self);
}

// SETTERS

void CommandQueue_setFamily(CommandQueue *self, uint32_t family) {
    if (!self)
        return;
    if (family > COMMAND_FAMILY_COPY)
        family = COMMAND_FAMILY_GRAPHICS;
    (*self).family = family;
}

// GETTERS

uint32_t CommandQueue_getFamily(const CommandQueue *self) {
    if (!self)
        return COMMAND_FAMILY_GRAPHICS;
    return (*self).family;
}

uint64_t CommandQueue_getTypeId(const CommandQueue *self) {
    if (!self)
        return 0;
    return (*self).typeId;
}
