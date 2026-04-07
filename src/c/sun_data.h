#ifndef SUN_DATA_H
#define SUN_DATA_H

#include <pebble.h>

typedef void (*SunDataCallback)(int sunrise_min, int sunset_min);

// Open AppMessage channel and register inbox/outbox handlers.
// The callback fires whenever new sunrise/sunset data arrives.
void sun_data_init(SunDataCallback on_update);
void sun_data_deinit(void);

int  sun_data_get_sunrise(void);
int  sun_data_get_sunset(void);

// Ask the phone for fresh GPS sun data.
void sun_data_request_update(void);

#endif
