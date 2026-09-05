#include "graphvex/version.h"

#include "annotation/overview.h"

;;OVERVIEW
/**
 * ============================================================================
 * MODULE: Version (graphvex/version.c)
 * LEVEL: L1 — File Metadata (skeleton placeholder)
 * ============================================================================
 * Skeleton placeholder so the graphvex target always has one TU.
 * Wave 1 landed: font/ (atlas + baking), io/vfs.c, vulkan/ (sdf_gpu,
 * vk_scene, vk_view, vk_iosurface, texture/) live here now.
 * Next: GPU compute callers switch to the GraphicsDevice seam (device.h).
 *
 * STRUCT FIELDS: none — procedural (version string only).
 *
 * FUNCTION REGISTRY:
 * ----------------------------------------------------------------------------
 * Core Functions:
 *   - Graphvex_version()
 * ============================================================================
 */

const char *Graphvex_version(void) {
    return "0.0.1-skeleton";
}
