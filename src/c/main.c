/*
 * Solar 24H — A 24-hour sun-tracking watchface for Pebble.
 *
 * The dial maps 24 hours onto 360°, with noon at the top (0°) and
 * midnight at the bottom (180°).  Colored arcs show daylight, twilight
 * and night; a sun/moon dot tracks the current time around the ring.
 *
 * Sunrise and sunset are fetched via GPS + Open-Meteo on the phone
 * side (src/pkjs/index.js) and sent here over AppMessage.
 */

#include <pebble.h>
#include "layout.h"
#include "geometry.h"
#include "sky.h"
#include "dial.h"
#include "sun_data.h"
#include "time_display.h"

static Window *s_main_window;
static Layer  *s_canvas_layer;
static GPoint  s_center;

// ---------------------------------------------------------------------------
// Canvas drawing — composites all visual layers in painter's order
// ---------------------------------------------------------------------------

static void canvas_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    int   rise   = sun_data_get_sunrise();
    int   set    = sun_data_get_sunset();

    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);

    draw_sky(ctx, bounds, rise, set);
    draw_time_hand(ctx, s_center);
    draw_sun_or_moon(ctx, s_center, rise, set);
    draw_center_disc(ctx, s_center);
    draw_quarter_hour_markers(ctx, s_center);
    draw_hour_markers(ctx, s_center);
}

// ---------------------------------------------------------------------------
// Event handlers
// ---------------------------------------------------------------------------

static void on_sun_data_updated(int sunrise_min, int sunset_min) {
    time_display_update_sun_info(sunrise_min, sunset_min);
    if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    time_display_update_time();
    if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);

    if (tick_time->tm_min == 0) {
        sun_data_request_update();
    }
}

// ---------------------------------------------------------------------------
// Window lifecycle
// ---------------------------------------------------------------------------

static void main_window_load(Window *window) {
    Layer *root   = window_get_root_layer(window);
    GRect  bounds = layer_get_bounds(root);
    s_center = GPoint(bounds.size.w / 2, bounds.size.h / 2);

    s_canvas_layer = layer_create(bounds);
    layer_set_update_proc(s_canvas_layer, canvas_update_proc);
    layer_add_child(root, s_canvas_layer);

    time_display_create(root, bounds, s_center);
    time_display_update_time();
    time_display_update_date();
    time_display_update_sun_info(
        sun_data_get_sunrise(), sun_data_get_sunset());
}

static void main_window_unload(Window *window) {
    time_display_destroy();

    layer_destroy(s_canvas_layer);
    s_canvas_layer = NULL;
}

// ---------------------------------------------------------------------------
// App lifecycle
// ---------------------------------------------------------------------------

static void init(void) {
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorBlack);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load   = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    sun_data_init(on_sun_data_updated);
}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    sun_data_deinit();
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
    return 0;
}
