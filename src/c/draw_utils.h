#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H

#include <pebble.h>

// graphics_fill_radial that correctly handles arcs wrapping past 0°.
void fill_radial_wrapped(GContext *ctx, GRect rect, int inset,
                         int32_t angle_start, int32_t angle_end);

// Text with a 1px dark outline (8 offset copies + foreground).
void draw_outlined_text(GContext *ctx, const char *text, GFont font,
                        GRect box, GColor fg, GColor outline);

// Line with a dark border (wider stroke underneath, then foreground).
void draw_outlined_line(GContext *ctx, GPoint a, GPoint b,
                        int width, GColor fg, GColor outline);

#endif
