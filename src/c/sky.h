#ifndef SKY_H
#define SKY_H

#include <pebble.h>

// Paint the layered sky disc (night, twilight, daylight, golden hour, etc.)
// using a painter's-algorithm stack of colored arcs.
void draw_sky(GContext *ctx, GRect bounds, int sunrise_min, int sunset_min);

#endif
