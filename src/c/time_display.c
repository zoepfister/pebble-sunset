#include "time_display.h"
#include "layout.h"

static TextLayer *s_time_layer;
static TextLayer *s_info_layer;
static TextLayer *s_date_layer;


static TextLayer *s_time_layer_top;
static TextLayer *s_time_layer_bottom;


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

void time_display_create_rect(Layer *root, GRect bounds, GPoint center) {
    int time_y = center.y - TIME_LAYER_HEIGHT + 5; // makeshift centering
    int info_y = bounds.size.h - 14;
    // compensate for the font-height, I cannot believe this works.
    int date_y = -4;
    APP_LOG(APP_LOG_LEVEL_INFO, "time_display_create_rect: time_y = %d, info_y = %d, date_y = %d", time_y, info_y, date_y);

    s_time_layer_top = create_text_layer(
        root,
        GRect(0, time_y, bounds.size.w, TIME_LAYER_HEIGHT),
        TIME_FONT_KEY, GColorWhite);
    s_time_layer_bottom = create_text_layer(
        root,
        GRect(0, time_y + TIME_LAYER_HEIGHT - 10, bounds.size.w, TIME_LAYER_HEIGHT),
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


void time_display_create_round(Layer *root, GRect bounds, GPoint center) {
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

void time_display_create(Layer *root, GRect bounds, GPoint center) {
    #ifdef PBL_RECT
        time_display_create_rect(root, bounds, center);
    #else
        time_display_create_round(root, bounds, center);
    #endif
}

void time_display_destroy(void) {
    text_layer_destroy(s_time_layer);
    s_time_layer = NULL;

    text_layer_destroy(s_info_layer);
    s_info_layer = NULL;

    text_layer_destroy(s_date_layer);
    s_date_layer = NULL;

    text_layer_destroy(s_time_layer_top);
    s_time_layer_top = NULL;

    text_layer_destroy(s_time_layer_bottom);
    s_time_layer_bottom = NULL;
}

void time_display_update_time(void) {
    time_t    now = time(NULL);
    struct tm *t  = localtime(&now);
    if (!t) return;

    #ifdef PBL_RECT
        static char buffer_top[3];
        static char buffer_bottom[3];
        strftime(buffer_top, sizeof(buffer_top), "%H", t);
        text_layer_set_text(s_time_layer_top, buffer_top);
        strftime(buffer_bottom, sizeof(buffer_bottom), "%M", t);
        text_layer_set_text(s_time_layer_bottom, buffer_bottom);
    #else
        static char buffer[8];
        strftime(buffer, sizeof(buffer), "%H:%M", t);
        text_layer_set_text(s_time_layer, buffer);
    #endif
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
    snprintf(buffer, sizeof(buffer), "%02d:%02d \u2022 %02d:%02d",
             sunrise_min / 60, sunrise_min % 60,
             sunset_min  / 60, sunset_min  % 60);
    text_layer_set_text(s_info_layer, buffer);
}
