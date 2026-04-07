#ifndef LAYOUT_EMERY_H
#define LAYOUT_EMERY_H

// Pebble Time 2 (Emery) — 200×228 rectangular, 64-color

// Dial geometry (radii from screen center)
#define HOUR_NUMBER_RADIUS       85
#define TICK_OUTER_RADIUS       109
#define TICK_INNER_EVEN_RADIUS   99
#define TICK_INNER_ODD_RADIUS   104
#define SKY_DISC_INSET            0
#define SUN_DOT_ORBIT_RADIUS     76
#define SUN_DOT_RADIUS            9
#define CENTER_DISC_RADIUS       48

// Hour label bounding box (half-extents)
#define HOUR_LABEL_HALF_W        16
#define HOUR_LABEL_HALF_H        12

// Digital time text layout
#define TIME_Y_OFFSET            26
#define TIME_LAYER_HEIGHT        50
#define INFO_Y_GAP               48
#define INFO_LAYER_HEIGHT        20
#define DATE_Y_OFFSET            12
#define DATE_LAYER_HEIGHT        20

// Fonts
#define TIME_FONT_KEY            FONT_KEY_BITHAM_42_BOLD
#define HOUR_NUMBER_FONT_KEY     FONT_KEY_GOTHIC_18_BOLD
#define INFO_FONT_KEY            FONT_KEY_GOTHIC_14
#define DATE_FONT_KEY            FONT_KEY_GOTHIC_18

#endif
