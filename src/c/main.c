/*
 * Solar 24H — A 24-hour sun-tracking watchface for Pebble Round 2.
 *
 * The dial maps 24 hours onto 360°, with noon at the top (0°) and
 * midnight at the bottom (180°).  Colored arcs show daylight, twilight
 * and night; a sun/moon dot tracks the current time around the ring.
 *
 * Sunrise and sunset are fetched via GPS + Open-Meteo on the phone
 * side (src/pkjs/index.js) and sent here over AppMessage.
 */

#include <pebble.h>

// ============================================================================
// Layout constants (pixel radii measured from screen center)
//
//   ┌─ hour numbers ─────── 85 px
//   │  ┌─ tick marks ────── 99–109 px
//   │  │  ┌─ sky disc ───── 0–108 px  (inset 22 from 130)
//   │  │  │      ┌ sun dot  76 px
//   │  │  │      │  ┌ inner 48 px
//   │  │  │      │  │
//   ▼  ▼  ▼      ▼  ▼
// ============================================================================

#define PBL_ROUND 1

#ifdef PBL_ROUND
#define HOUR_NUMBER_RADIUS     107
#define TICK_OUTER_RADIUS      130
#define TICK_INNER_EVEN_RADIUS  120
#define TICK_INNER_ODD_RADIUS  125
#define SKY_DISC_INSET          0
#define SUN_DOT_ORBIT_RADIUS    80
#define SUN_DOT_RADIUS           7
#define CENTER_DISC_RADIUS      57

#else
#define HOUR_NUMBER_RADIUS     85
#define TICK_OUTER_RADIUS      109
#define TICK_INNER_EVEN_RADIUS  99
#define TICK_INNER_ODD_RADIUS  104
#define SKY_DISC_INSET          0
#define SUN_DOT_ORBIT_RADIUS    76
#define SUN_DOT_RADIUS           9
#define CENTER_DISC_RADIUS      48

#endif

// Sun/moon indicator ring sizes (offsets from SUN_DOT_RADIUS)
#define SUN_GLOW_EXTRA          2
#define SUN_CORE_INSET          4
#define MOON_OUTER_INSET        2
#define MOON_INNER_INSET        5

// Centre disc inner ring inset
#define CENTER_DISC_INNER_INSET 0
// Hand base starts just outside the centre disc
#define HAND_BASE_GAP           2

// Hour-label bounding box (half-width/half-height)
#define HOUR_LABEL_HALF_W      16
#define HOUR_LABEL_HALF_H      12

// Sky-layer timing thresholds (minutes)
#define TWILIGHT_DURATION       75
#define CLEAR_SKY_INSET         45
#define GOLDEN_HOUR_INNER       12
#define GOLDEN_HOUR_OUTER       45
#define HORIZON_GLOW_INNER      12
#define HORIZON_GLOW_OUTER      18
#define MIN_DAY_FOR_GOLDEN_HOUR  90
#define MIN_DAY_FOR_CLEAR_SKY   120
#define MIN_DAY_FOR_BRIGHT_SKY  180
#define MIN_DAY_FOR_NOON_ZENITH 240

// Digital clock layout (pixels)
#define TIME_FONT_HEIGHT        42
#define TIME_LAYER_HEIGHT       50
#define TIME_Y_OFFSET           26
#define INFO_Y_GAP              48
#define INFO_LAYER_HEIGHT       20
#define DATE_LAYER_HEIGHT       20
#define DATE_Y_OFFSET           12

// AppMessage buffer sizes
#define APPMSG_INBOX_SIZE      256
#define APPMSG_OUTBOX_SIZE      64

// ============================================================================
// Global state
// ============================================================================

static Window    *s_main_window;
static Layer     *s_canvas_layer;
static TextLayer *s_time_layer;
static TextLayer *s_sunrise_sunset_layer;
static TextLayer *s_date_layer;

// Sunrise / sunset stored as minutes since midnight (0–1439).
// Defaults to 06:00 / 18:00 until the phone sends real GPS data.
#define DEFAULT_SUNRISE  (6 * 60)
#define DEFAULT_SUNSET  (18 * 60)

static int    s_sunrise_minutes = DEFAULT_SUNRISE;
static int    s_sunset_minutes  = DEFAULT_SUNSET;
static GPoint s_center;

// ============================================================================
// Angle helpers
//
// The watch dial places **noon at 0°** (top) and midnight at 180°.
// Angles increase clockwise, matching Pebble's native trig convention.
//
//            12 (noon)
//              0°
//       10 /       \ 14
//     270°           90°
//        8 \       / 16
//            180°
//          0/24 (midnight)
//
// Formula: angle = ((wall_minutes + 720) mod 1440) / 1440 × TRIG_MAX_ANGLE
// ============================================================================

static int32_t minutes_to_angle(int wall_minutes) {
    int normalised = ((wall_minutes % MINUTES_PER_DAY) + MINUTES_PER_DAY)
                     % MINUTES_PER_DAY;
    int offset = (normalised + (MINUTES_PER_DAY / 2)) % MINUTES_PER_DAY;
    return (int32_t)offset * TRIG_MAX_ANGLE / MINUTES_PER_DAY;
}

static int current_time_in_minutes(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    return t ? (t->tm_hour * 60 + t->tm_min) : 0;
}

static GPoint point_on_circle(int32_t angle, int radius) {
    return (GPoint) {
        s_center.x + (sin_lookup(angle) * radius) / TRIG_MAX_RATIO,
        s_center.y - (cos_lookup(angle) * radius) / TRIG_MAX_RATIO
    };
}

// ============================================================================
// Sky drawing
//
// The sky is painted with a layered "painter's algorithm" approach:
// each successive layer overwrites the one below.  This avoids the
// sub-pixel gap artefacts that appear when tiling many small segments.
//
//  Layer  Colour          Region
//  ─────  ──────────────  ───────────────────────────────
//    1    OxfordBlue      full circle  (deep night)
//    2    DukeBlue        ±75 min of sunrise/sunset  (twilight)
//    3    CobaltBlue      sunrise → sunset  (dim daylight)
//    4    VividCerulean   sunrise+45 → sunset-45  (clear sky)
//    5    PictonBlue      middle third of day  (bright sky)
//    6    Celeste         middle seventh of day  (noon zenith)
//    7    Orange          sunrise+12…+45 / sunset-45…-12  (golden hour)
//    8    ChromeYellow    sunrise±15 / sunset±15  (horizon glow)
// ============================================================================

// graphics_fill_radial draws nothing useful when start > end (the arc
// wraps past 0°/noon).  This helper splits it into two draws.
static void fill_radial_wrapped(GContext *ctx, GRect rect, int inset,
                                int32_t angle_start, int32_t angle_end) {
    if (angle_start <= angle_end) {
        graphics_fill_radial(ctx, rect, GOvalScaleModeFitCircle,
                             inset, angle_start, angle_end);
    } else {
        graphics_fill_radial(ctx, rect, GOvalScaleModeFitCircle,
                             inset, angle_start, TRIG_MAX_ANGLE);
        if (angle_end > 0) {
            graphics_fill_radial(ctx, rect, GOvalScaleModeFitCircle,
                                 inset, 0, angle_end);
        }
    }
}

static void draw_sky(GContext *ctx, GRect bounds) {
    GRect disc = grect_inset(bounds, GEdgeInsets(SKY_DISC_INSET));
    int   fill = disc.size.w / 2;   // inset that fills the whole disc

    int rise       = s_sunrise_minutes;
    int set        = s_sunset_minutes;
    int day_length = set - rise;
    int noon       = (rise + set) / 2;

    // 1. Night base — covers the whole disc
    graphics_context_set_fill_color(ctx, GColorOxfordBlue);
    graphics_fill_radial(ctx, disc, GOvalScaleModeFitCircle,
                         fill, 0, TRIG_MAX_ANGLE);

    // 2. Twilight band before sunrise & after sunset
    graphics_context_set_fill_color(ctx, GColorDukeBlue);
    fill_radial_wrapped(ctx, disc, fill,
                        minutes_to_angle(rise - TWILIGHT_DURATION),
                        minutes_to_angle(rise));
    fill_radial_wrapped(ctx, disc, fill,
                        minutes_to_angle(set),
                        minutes_to_angle(set + TWILIGHT_DURATION));

    // 3. Daylight base — sunrise to sunset
    graphics_context_set_fill_color(ctx, GColorCobaltBlue);
    fill_radial_wrapped(ctx, disc, fill,
                        minutes_to_angle(rise),
                        minutes_to_angle(set));

    // 4. Clear sky — inset from each edge
    if (day_length > MIN_DAY_FOR_CLEAR_SKY) {
        graphics_context_set_fill_color(ctx, GColorVividCerulean);
        fill_radial_wrapped(ctx, disc, fill,
                            minutes_to_angle(rise + CLEAR_SKY_INSET),
                            minutes_to_angle(set  - CLEAR_SKY_INSET));
    }

    // 5. Bright sky — middle third of daylight
    if (day_length > MIN_DAY_FOR_BRIGHT_SKY) {
        int third = day_length / 3;
        graphics_context_set_fill_color(ctx, GColorPictonBlue);
        fill_radial_wrapped(ctx, disc, fill,
                            minutes_to_angle(noon - third),
                            minutes_to_angle(noon + third));
    }

    // 6. Noon zenith — middle seventh of daylight
    if (day_length > MIN_DAY_FOR_NOON_ZENITH) {
        int seventh = day_length / 7;
        graphics_context_set_fill_color(ctx, GColorCeleste);
        fill_radial_wrapped(ctx, disc, fill,
                            minutes_to_angle(noon - seventh),
                            minutes_to_angle(noon + seventh));
    }

    // 7. Golden hour — warm orange near sunrise/sunset
    if (day_length > MIN_DAY_FOR_GOLDEN_HOUR) {
        graphics_context_set_fill_color(ctx, GColorOrange);
        fill_radial_wrapped(ctx, disc, fill,
                            minutes_to_angle(rise + GOLDEN_HOUR_INNER),
                            minutes_to_angle(rise + GOLDEN_HOUR_OUTER));
        fill_radial_wrapped(ctx, disc, fill,
                            minutes_to_angle(set  - GOLDEN_HOUR_OUTER),
                            minutes_to_angle(set  - GOLDEN_HOUR_INNER));
    }

    // 8. Horizon glow — bright warm band right at sunrise/sunset
    graphics_context_set_fill_color(ctx, GColorChromeYellow);
    fill_radial_wrapped(ctx, disc, fill,
                        minutes_to_angle(rise - HORIZON_GLOW_INNER),
                        minutes_to_angle(rise + HORIZON_GLOW_OUTER));
    fill_radial_wrapped(ctx, disc, fill,
                        minutes_to_angle(set  - HORIZON_GLOW_OUTER),
                        minutes_to_angle(set  + HORIZON_GLOW_INNER));

    // Thin border around the sky disc
    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_arc(ctx, disc, GOvalScaleModeFitCircle,
                      0, TRIG_MAX_ANGLE);
}

// ============================================================================
// Sun / moon indicator dot
// ============================================================================

static void draw_sun_or_moon(GContext *ctx) {
    int     now_minutes = current_time_in_minutes();
    int32_t angle       = minutes_to_angle(now_minutes);
    GPoint  position    = point_on_circle(angle, SUN_DOT_ORBIT_RADIUS);

    bool is_daytime = (now_minutes >= s_sunrise_minutes
                    && now_minutes <= s_sunset_minutes);

    if (is_daytime) {
        graphics_context_set_fill_color(ctx, GColorYellow);
        graphics_fill_circle(ctx, position, SUN_DOT_RADIUS + SUN_GLOW_EXTRA);
        graphics_context_set_fill_color(ctx, GColorPastelYellow);
        graphics_fill_circle(ctx, position, SUN_DOT_RADIUS);
        graphics_context_set_fill_color(ctx, GColorWhite);
        graphics_fill_circle(ctx, position, SUN_DOT_RADIUS - SUN_CORE_INSET);
    } else {
        graphics_context_set_fill_color(ctx, GColorLightGray);
        graphics_fill_circle(ctx, position, SUN_DOT_RADIUS - MOON_OUTER_INSET);
        graphics_context_set_fill_color(ctx, GColorDarkGray);
        graphics_fill_circle(ctx, position, SUN_DOT_RADIUS - MOON_INNER_INSET);
    }
}

// ============================================================================
// Time-position hand (thin white line from inner circle to tick ring)
// ============================================================================

static void draw_time_hand(GContext *ctx) {
    int32_t angle = minutes_to_angle(current_time_in_minutes());
    GPoint  tip   = point_on_circle(angle, TICK_OUTER_RADIUS);
    GPoint  base  = point_on_circle(angle, CENTER_DISC_RADIUS + HAND_BASE_GAP);

    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, base, tip);
}

// ============================================================================
// Dark centre disc (background for the digital time readout)
// ============================================================================

static void draw_center_disc(GContext *ctx) {
    graphics_context_set_fill_color(ctx, GColorOxfordBlue);
    graphics_fill_circle(ctx, s_center, CENTER_DISC_RADIUS);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, s_center, CENTER_DISC_RADIUS - CENTER_DISC_INNER_INSET);

    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_circle(ctx, s_center, CENTER_DISC_RADIUS);
}

// ============================================================================
// 24-hour tick marks and hour numbers around the dial edge
// ============================================================================

static void draw_hour_markers(GContext *ctx) {
    GFont number_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

    for (int hour = 0; hour < 24; hour++) {
        int32_t angle    = minutes_to_angle(hour * 60);
        bool    is_even  = (hour % 2 == 0);
        int     inner_r  = TICK_INNER_EVEN_RADIUS;

        GPoint tick_inner = point_on_circle(angle, inner_r);
        GPoint tick_outer = point_on_circle(angle, TICK_OUTER_RADIUS);

        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_stroke_width(ctx, is_even ? 2 : 1);
        graphics_draw_line(ctx, tick_inner, tick_outer);

        if (is_even) {
            int display_hour = (hour == 0) ? 24 : hour;
            char label[3];
            snprintf(label, sizeof(label), "%d", display_hour);

            GPoint label_centre = point_on_circle(angle, HOUR_NUMBER_RADIUS);
            GRect  label_box    = GRect(
                label_centre.x - HOUR_LABEL_HALF_W,
                label_centre.y - HOUR_LABEL_HALF_H,
                HOUR_LABEL_HALF_W * 2,
                HOUR_LABEL_HALF_H * 2);

            graphics_context_set_text_color(ctx, GColorWhite);
            graphics_draw_text(ctx, label, number_font, label_box,
                               GTextOverflowModeTrailingEllipsis,
                               GTextAlignmentCenter, NULL);
        }
    }
}

// ============================================================================
// 15-minute tick marks
// ============================================================================

static void draw_15_minute_markers(GContext *ctx) {
    for (int minute = 0; minute < MINUTES_PER_DAY; minute += 15) {
        int32_t angle = minutes_to_angle(minute);

        GPoint tick_inner = point_on_circle(angle, TICK_INNER_ODD_RADIUS);
        GPoint tick_outer = point_on_circle(angle, TICK_OUTER_RADIUS);

        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_stroke_width(ctx, 1);
        graphics_draw_line(ctx, tick_inner, tick_outer);

    }
}

// ============================================================================
// Canvas compositing (called once per minute via layer_mark_dirty)
// ============================================================================

static void canvas_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);

    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);

    draw_sky(ctx, bounds);
    draw_time_hand(ctx);
    draw_sun_or_moon(ctx);
    draw_center_disc(ctx);
    draw_15_minute_markers(ctx);
    draw_hour_markers(ctx);
    
}

// ============================================================================
// Text updates (digital time + sunrise/sunset info)
// ============================================================================

static void update_time_display(void) {
    time_t    now  = time(NULL);
    struct tm *t   = localtime(&now);
    if (!t) return;

    static char buffer[8];
    strftime(buffer, sizeof(buffer), "%H:%M", t);
    text_layer_set_text(s_time_layer, buffer);

    if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
}

static void update_sunrise_sunset_display(void) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d:%02d \u2022 %d:%02d",
             s_sunrise_minutes / 60, s_sunrise_minutes % 60,
             s_sunset_minutes  / 60, s_sunset_minutes  % 60);
    text_layer_set_text(s_sunrise_sunset_layer, buffer);
}

static void update_date_display(void) {
    time_t    now  = time(NULL);
    struct tm *t   = localtime(&now);
    if (!t) return;

    static char buffer[11];
    strftime(buffer, sizeof(buffer), "%d.%m.%Y", t);
    text_layer_set_text(s_date_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time_display();

    // Ask the phone for fresh sun data once per hour
    if (tick_time->tm_min == 0) {
        DictionaryIterator *iter;
        if (app_message_outbox_begin(&iter) == APP_MSG_OK) {
            dict_write_uint8(iter, MESSAGE_KEY_REQUEST_SUN_DATA, 1);
            app_message_outbox_send();
        }
    }
}

// ============================================================================
// AppMessage — receive sunrise/sunset from the phone (via pkjs + Open-Meteo)
// ============================================================================

static void on_message_received(DictionaryIterator *iterator, void *context) {
    Tuple *rise_hour = dict_find(iterator, MESSAGE_KEY_SUNRISE_HOUR);
    Tuple *rise_min  = dict_find(iterator, MESSAGE_KEY_SUNRISE_MIN);
    Tuple *set_hour  = dict_find(iterator, MESSAGE_KEY_SUNSET_HOUR);
    Tuple *set_min   = dict_find(iterator, MESSAGE_KEY_SUNSET_MIN);

    if (rise_hour && rise_min && set_hour && set_min) {
        s_sunrise_minutes = (int)rise_hour->value->int32 * 60
                          + (int)rise_min->value->int32;
        s_sunset_minutes  = (int)set_hour->value->int32 * 60
                          + (int)set_min->value->int32;

        if (s_sunrise_minutes < 0)
            s_sunrise_minutes = 0;
        if (s_sunset_minutes > MINUTES_PER_DAY - 1)
            s_sunset_minutes  = MINUTES_PER_DAY - 1;
        if (s_sunset_minutes <= s_sunrise_minutes)
            s_sunset_minutes  = s_sunrise_minutes + 60;

        APP_LOG(APP_LOG_LEVEL_INFO, "Sun: rise=%d:%02d set=%d:%02d",
                s_sunrise_minutes / 60, s_sunrise_minutes % 60,
                s_sunset_minutes  / 60, s_sunset_minutes  % 60);

        update_sunrise_sunset_display();
        if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
    }
}

static void on_message_dropped(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Inbox message dropped: %d", reason);
}

static void on_send_failed(DictionaryIterator *iter,
                            AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed: %d", reason);
}

static void on_send_succeeded(DictionaryIterator *iter, void *context) {
    (void)iter; (void)context;
}

// ============================================================================
// Window load / unload
// ============================================================================

static void main_window_load(Window *window) {
    Layer *root   = window_get_root_layer(window);
    GRect  bounds = layer_get_bounds(root);
    s_center = GPoint(bounds.size.w / 2, bounds.size.h / 2);

    // Full-screen canvas for all custom drawing
    s_canvas_layer = layer_create(bounds);
    layer_set_update_proc(s_canvas_layer, canvas_update_proc);
    layer_add_child(root, s_canvas_layer);

    // Large digital clock in the centre
    int time_y = s_center.y - TIME_Y_OFFSET;
    s_time_layer = text_layer_create(
        GRect(0, time_y, bounds.size.w, TIME_LAYER_HEIGHT));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_font(s_time_layer,
        fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(root, text_layer_get_layer(s_time_layer));

    // Small sunrise/sunset readout below the clock
    int info_y = time_y + INFO_Y_GAP;
    s_sunrise_sunset_layer = text_layer_create(
        GRect(0, info_y, bounds.size.w, INFO_LAYER_HEIGHT));
    text_layer_set_background_color(s_sunrise_sunset_layer, GColorClear);
    text_layer_set_text_color(s_sunrise_sunset_layer, GColorLightGray);
    text_layer_set_font(s_sunrise_sunset_layer,
        fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_sunrise_sunset_layer,
        GTextAlignmentCenter);
    layer_add_child(root, text_layer_get_layer(s_sunrise_sunset_layer));

    // Date layer
    int date_info_y = time_y - DATE_Y_OFFSET;
    s_date_layer = text_layer_create(
        GRect(0, date_info_y, bounds.size.w, DATE_LAYER_HEIGHT));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorLightGray);
    text_layer_set_font(s_date_layer,
        fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(s_date_layer,
        GTextAlignmentCenter);
    layer_add_child(root, text_layer_get_layer(s_date_layer));

    update_sunrise_sunset_display();
    update_date_display();
    update_time_display();
}

static void main_window_unload(Window *window) {
    layer_destroy(s_canvas_layer);
    s_canvas_layer = NULL;

    text_layer_destroy(s_time_layer);
    s_time_layer = NULL;

    text_layer_destroy(s_sunrise_sunset_layer);
    s_sunrise_sunset_layer = NULL;
}

// ============================================================================
// App lifecycle
// ============================================================================

static void init(void) {
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorBlack);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load   = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    app_message_register_inbox_received(on_message_received);
    app_message_register_inbox_dropped(on_message_dropped);
    app_message_register_outbox_failed(on_send_failed);
    app_message_register_outbox_sent(on_send_succeeded);
    app_message_open(APPMSG_INBOX_SIZE, APPMSG_OUTBOX_SIZE);
}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
    return 0;
}
