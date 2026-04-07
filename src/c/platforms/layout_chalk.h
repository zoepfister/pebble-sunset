#ifndef LAYOUT_CHALK_H
#define LAYOUT_CHALK_H

// Pebble Time Round (Chalk) — 180×180 round, 64-color
// Scaled from gabbro by 90/130 ≈ 0.69

// Dial geometry (radii from screen center)
#define HOUR_NUMBER_RADIUS       74
#define TICK_OUTER_RADIUS        90
#define TICK_INNER_EVEN_RADIUS   83
#define TICK_INNER_ODD_RADIUS    87
#define SKY_DISC_INSET            0
#define SUN_DOT_ORBIT_RADIUS     55
#define SUN_DOT_RADIUS            5
#define CENTER_DISC_RADIUS       39

// Hour label bounding box (half-extents)
#define HOUR_LABEL_HALF_W        14
#define HOUR_LABEL_HALF_H        10

// Digital time text layout
#define TIME_Y_OFFSET            22
#define TIME_LAYER_HEIGHT        45
#define INFO_Y_GAP               38
#define INFO_LAYER_HEIGHT        18
#define DATE_Y_OFFSET            10
#define DATE_LAYER_HEIGHT        18

// Fonts (smaller for 180px display)
#define TIME_FONT_KEY            FONT_KEY_BITHAM_34_MEDIUM_NUMBERS
#define HOUR_NUMBER_FONT_KEY     FONT_KEY_GOTHIC_14_BOLD
#define INFO_FONT_KEY            FONT_KEY_GOTHIC_14
#define DATE_FONT_KEY            FONT_KEY_GOTHIC_14

#endif
