#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <pebble.h>

// Convert wall-clock minutes (0–1439) to an angle on the 24h dial.
// Noon is at 0° (top), midnight at 180° (bottom).
int32_t minutes_to_angle(int wall_minutes);

// Current local time as minutes since midnight.
int current_time_in_minutes(void);

// Point at the given angle and radius from center.
GPoint point_on_circle(GPoint center, int32_t angle, int radius);

#endif
