#ifndef DIAL_H
#define DIAL_H

#include <pebble.h>

void draw_hour_markers(GContext *ctx, GPoint center);
void draw_quarter_hour_markers(GContext *ctx, GPoint center);
void draw_time_hand(GContext *ctx, GPoint center);
void draw_sun_or_moon(GContext *ctx, GPoint center,
                      int sunrise_min, int sunset_min);
void draw_center_disc(GContext *ctx, GPoint center);

#endif
