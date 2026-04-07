#ifndef LAYOUT_GABBRO_H
#define LAYOUT_GABBRO_H

// Pebble Round 2 (Gabbro) — 260×260 round, 64-color

// Dial geometry (radii from screen center)
#define HOUR_NUMBER_RADIUS      107
#define TICK_OUTER_RADIUS       130
#define TICK_INNER_EVEN_RADIUS  120
#define TICK_INNER_ODD_RADIUS   125
#define SKY_DISC_INSET            0
#define SUN_DOT_ORBIT_RADIUS     80
#define SUN_DOT_RADIUS            7
#define CENTER_DISC_RADIUS       57

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
