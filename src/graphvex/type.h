#ifndef GRAPHVEX_TYPE_H
#define GRAPHVEX_TYPE_H

#include "oop/type.h"

// graphvex/type.h — the graphvex project's type registry.
//
// OWNERSHIP: every graphvex class ID lives here, not in vexspoke's
// oop/type.h. Included by graphvex sources that stamp headers with
// graphvex-owned types (font atlases today; texture/vfs classes next).
//
// CLASS-SPACE CONTRACT (Rule 17): graphvex classes live in 0x0100-0x01FF.
// All new graphvex IDs MUST land in that range — the central parent chain
// and arch switch in vexspoke oop/type.c dispatch on ranges and may not
// include this file (upstream builds standalone).

// --- GRAPHVEX GPU OBJECTS ---
#define ID_FONT 0x0100u

#define TYPE_FONT_SINGLETON (PROJ_GRAPHVEX | FORM_SINGLETON | ID_FONT)

#endif
