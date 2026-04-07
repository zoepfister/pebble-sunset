#ifndef LAYOUT_H
#define LAYOUT_H

// Platform-specific constants are in separate headers, selected at build time
// by the SDK's platform macros.

// #define PBL_PLATFORM_GABBRO 1

#if defined(PBL_PLATFORM_GABBRO)
  #include "platforms/layout_gabbro.h"
#elif defined(PBL_PLATFORM_CHALK)
  #include "platforms/layout_chalk.h"
#elif defined(PBL_PLATFORM_EMERY)
  #include "platforms/layout_emery.h"
#elif defined(PBL_ROUND)
  #include "platforms/layout_chalk.h"
#else
  #include "platforms/layout_basalt.h"
#endif

// Shared constants (identical across all platforms)

// Sun/moon indicator ring sizes
#define SUN_GLOW_EXTRA           2
#define SUN_CORE_INSET           4
#define MOON_OUTER_INSET         2
#define MOON_INNER_INSET         5

// Centre disc inner ring offset
#define CENTER_DISC_INNER_INSET  0

// Hand base offset from centre disc edge
#define HAND_BASE_GAP            2

#endif
