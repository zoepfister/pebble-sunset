#include "time_display.h"
#include "layout.h"

static TextLayer *s_time_layer;
static TextLayer *s_info_layer;
static TextLayer *s_date_layer;

static TextLayer *create_text_layer(Layer *root, GRect frame,
                                    const char *font_key, GColor color) {
    TextLayer *layer = text_layer_create(frame);
    text_layer_set_background_color(layer, GColorClear);
    text_layer_set_text_color(layer, color);
    text_layer_set_font(layer, fonts_get_system_font(font_key));
    text_layer_set_text_alignment(layer, GTextAlignmentCenter);
    layer_add_child(root, text_layer_get_layer(layer));
    return layer;
}

void time_display_create(Layer *root, GRect bounds, GPoint center) {
    int time_y = center.y - TIME_Y_OFFSET;
    int info_y = time_y + INFO_Y_GAP;
    int date_y = time_y - DATE_Y_OFFSET;

    s_time_layer = create_text_layer(
        root,
        GRect(0, time_y, bounds.size.w, TIME_LAYER_HEIGHT),
        TIME_FONT_KEY, GColorWhite);

    s_info_layer = create_text_layer(
        root,
        GRect(0, info_y, bounds.size.w, INFO_LAYER_HEIGHT),
        INFO_FONT_KEY, GColorLightGray);

    s_date_layer = create_text_layer(
        root,
        GRect(0, date_y, bounds.size.w, DATE_LAYER_HEIGHT),
        DATE_FONT_KEY, GColorLightGray);
}

void time_display_destroy(void) {
    text_layer_destroy(s_time_layer);
    s_time_layer = NULL;

    text_layer_destroy(s_info_layer);
    s_info_layer = NULL;

    text_layer_destroy(s_date_layer);
    s_date_layer = NULL;
}

void time_display_update_time(void) {
    time_t    now = time(NULL);
    struct tm *t  = localtime(&now);
    if (!t) return;

    static char buffer[8];
    strftime(buffer, sizeof(buffer), "%H:%M", t);
    text_layer_set_text(s_time_layer, buffer);
}

void time_display_update_date(void) {
    time_t    now = time(NULL);
    struct tm *t  = localtime(&now);
    if (!t) return;

    static char buffer[11];
    strftime(buffer, sizeof(buffer), "%d.%m.%Y", t);
    text_layer_set_text(s_date_layer, buffer);
}

void time_display_update_sun_info(int sunrise_min, int sunset_min) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d:%02d \u2022 %d:%02d",
             sunrise_min / 60, sunrise_min % 60,
             sunset_min  / 60, sunset_min  % 60);
    text_layer_set_text(s_info_layer, buffer);
}
