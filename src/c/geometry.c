#include "geometry.h"

int32_t minutes_to_angle(int wall_minutes) {
    int normalised = ((wall_minutes % MINUTES_PER_DAY) + MINUTES_PER_DAY)
                     % MINUTES_PER_DAY;
    int offset = (normalised + (MINUTES_PER_DAY / 2)) % MINUTES_PER_DAY;
    return (int32_t)offset * TRIG_MAX_ANGLE / MINUTES_PER_DAY;
}

int current_time_in_minutes(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    return t ? (t->tm_hour * 60 + t->tm_min) : 0;
}

GPoint point_on_circle(GPoint center, int32_t angle, int radius) {
    return (GPoint) {
        center.x + (sin_lookup(angle) * radius) / TRIG_MAX_RATIO,
        center.y - (cos_lookup(angle) * radius) / TRIG_MAX_RATIO
    };
}
