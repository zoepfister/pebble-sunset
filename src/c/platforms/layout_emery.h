#ifndef LAYOUT_EMERY_H
#define LAYOUT_EMERY_H

// Dial geometry (radii from screen center)
#define HOUR_NUMBER_RADIUS       83
#define TICK_OUTER_RADIUS        100
#define TICK_INNER_EVEN_RADIUS   96
#define TICK_INNER_ODD_RADIUS    99
#define SKY_DISC_INSET            1
#define SUN_DOT_ORBIT_RADIUS     62
#define SUN_DOT_RADIUS            4
#define CENTER_DISC_RADIUS       45

// Hour label bounding box (half-extents)
#define HOUR_LABEL_HALF_W        12
#define HOUR_LABEL_HALF_H        12

// Digital time text layout
#define TIME_Y_OFFSET            22
#define TIME_LAYER_HEIGHT        40
#define INFO_Y_GAP               36
#define INFO_LAYER_HEIGHT        18
#define DATE_Y_OFFSET            11
#define DATE_LAYER_HEIGHT        18

// Fonts (smaller for 144×168 display)
#define TIME_FONT_KEY            FONT_KEY_BITHAM_34_MEDIUM_NUMBERS
#define HOUR_NUMBER_FONT_KEY     FONT_KEY_GOTHIC_18_BOLD
#define INFO_FONT_KEY            FONT_KEY_GOTHIC_14
#define DATE_FONT_KEY            FONT_KEY_GOTHIC_14

#endif
