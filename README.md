# graphvex — R1.5 GPU compute (supervised by R0 hotcwap Kernel)
all graphics computing lies here, bespoken

## Boilerplate (shared, not vendored)
Foundation layer (Rule 17): graphvex depends solely on `vexspoke`.
Shared infrastructure resolves from `vexspoke/src` via the PUBLIC link —
never copied here, so include strings stay single-sourced:

| include | resolves to |
|---|---|
| `annotation/overview.h` | `vexspoke/src/annotation/overview.h` |
| `c23/constructor.h`, `c23/overload.h` | `vexspoke/src/c23/*` |
| `oop/type.h` (`Type_*`, `ID_*`, `ARCH_*`) | `vexspoke/src/oop/type.h` |
| `nio/mem.h`, `struct/*`, `lang/*` | `vexspoke/src/...` |

graphvex-owned code lives under `src/` in subsystem folders — `buffer/`
(off-heap 2D raster: `Buffer`, `ColorBuffer`, `Depth/Stencil/Frame/Height/
Normal/Shadow`), `font/`, `io/`, `vulkan/`, plus `src/graphvex/` core;
new subsystem folders here must use filenames unique across the include
path — never shadow a `vexspoke` filename.
