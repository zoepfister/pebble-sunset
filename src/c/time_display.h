#ifndef TIME_DISPLAY_H
#define TIME_DISPLAY_H

#include <pebble.h>

// Create and add the digital clock, date, and sunrise/sunset text layers.
void time_display_create(Layer *root, GRect bounds, GPoint center);
void time_display_destroy(void);

void time_display_update_time(void);
void time_display_update_date(void);
void time_display_update_sun_info(int sunrise_min, int sunset_min);

#endif
