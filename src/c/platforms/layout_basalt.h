#ifndef LAYOUT_BASALT_H
#define LAYOUT_BASALT_H

// Pebble Time / Pebble 2 Duo (Basalt/Flint) — 144×168 rectangular, 64-color
// Also used as fallback for Diorite and Aplite.
// Scaled from emery by 72/100 ≈ 0.72

// Dial geometry (radii from screen center)
#define HOUR_NUMBER_RADIUS       61
#define TICK_OUTER_RADIUS        78
#define TICK_INNER_EVEN_RADIUS   71
#define TICK_INNER_ODD_RADIUS    75
#define SKY_DISC_INSET            0
#define SUN_DOT_ORBIT_RADIUS     55
#define SUN_DOT_RADIUS            6
#define CENTER_DISC_RADIUS       35

// Hour label bounding box (half-extents)
#define HOUR_LABEL_HALF_W        12
#define HOUR_LABEL_HALF_H        10

// Digital time text layout
#define TIME_Y_OFFSET            18
#define TIME_LAYER_HEIGHT        40
#define INFO_Y_GAP               36
#define INFO_LAYER_HEIGHT        18
#define DATE_Y_OFFSET            10
#define DATE_LAYER_HEIGHT        18

// Fonts (smaller for 144×168 display)
#define TIME_FONT_KEY            FONT_KEY_BITHAM_34_MEDIUM_NUMBERS
#define HOUR_NUMBER_FONT_KEY     FONT_KEY_GOTHIC_14_BOLD
#define INFO_FONT_KEY            FONT_KEY_GOTHIC_14
#define DATE_FONT_KEY            FONT_KEY_GOTHIC_14

#endif
