#include "dial.h"
#include "layout.h"
#include "geometry.h"
#include "draw_utils.h"

// ---------------------------------------------------------------------------
// Sun / Moon indicator
// ---------------------------------------------------------------------------

static void draw_sun(GContext *ctx, GPoint pos) {
    graphics_context_set_fill_color(ctx, GColorYellow);
    graphics_fill_circle(ctx, pos, SUN_DOT_RADIUS + SUN_GLOW_EXTRA);
    graphics_context_set_fill_color(ctx, GColorPastelYellow);
    graphics_fill_circle(ctx, pos, SUN_DOT_RADIUS);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, pos, SUN_DOT_RADIUS - SUN_CORE_INSET);
}

static void draw_moon(GContext *ctx, GPoint pos) {
    graphics_context_set_fill_color(ctx, GColorLightGray);
    graphics_fill_circle(ctx, pos, SUN_DOT_RADIUS - MOON_OUTER_INSET);
    graphics_context_set_fill_color(ctx, GColorDarkGray);
    graphics_fill_circle(ctx, pos, SUN_DOT_RADIUS - MOON_INNER_INSET);
}

void draw_sun_or_moon(GContext *ctx, GPoint center,
                      int sunrise_min, int sunset_min) {
    int     now   = current_time_in_minutes();
    int32_t angle = minutes_to_angle(now);
    GPoint  pos   = point_on_circle(center, angle, SUN_DOT_ORBIT_RADIUS);

    if (now >= sunrise_min && now <= sunset_min) {
        draw_sun(ctx, pos);
    } else {
        draw_moon(ctx, pos);
    }
}

// ---------------------------------------------------------------------------
// Time-position hand (thin white line from centre disc to tick ring)
// ---------------------------------------------------------------------------

void draw_time_hand(GContext *ctx, GPoint center) {
    int32_t angle = minutes_to_angle(current_time_in_minutes());
    GPoint  tip   = point_on_circle(center, angle, TICK_OUTER_RADIUS);
    GPoint  base  = point_on_circle(center, angle,
                                    CENTER_DISC_RADIUS + HAND_BASE_GAP);

    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, base, tip);
}

// ---------------------------------------------------------------------------
// Dark centre disc (background for the digital time readout)
// ---------------------------------------------------------------------------

void draw_center_disc(GContext *ctx, GPoint center) {
    graphics_context_set_fill_color(ctx, GColorOxfordBlue);
    graphics_fill_circle(ctx, center, CENTER_DISC_RADIUS);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, center,
                         CENTER_DISC_RADIUS - CENTER_DISC_INNER_INSET);

    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_circle(ctx, center, CENTER_DISC_RADIUS);
}

// ---------------------------------------------------------------------------
// Hour markers and numbers
// ---------------------------------------------------------------------------

static void draw_hour_tick(GContext *ctx, GPoint center, int hour) {
    int32_t angle = minutes_to_angle(hour * 60);
    bool    even  = (hour % 2 == 0);

    GPoint inner = point_on_circle(center, angle, TICK_INNER_EVEN_RADIUS);
    GPoint outer = point_on_circle(center, angle, TICK_OUTER_RADIUS);

    draw_outlined_line(ctx, inner, outer,
                       even ? 2 : 1, GColorWhite, GColorBlack);
}

static void draw_hour_label(GContext *ctx, GPoint center,
                            int hour, GFont font) {
    int display_hour = (hour == 0) ? 24 : hour;
    char label[3];
    snprintf(label, sizeof(label), "%d", display_hour);

    int32_t angle = minutes_to_angle(hour * 60);
    GPoint  lc    = point_on_circle(center, angle, HOUR_NUMBER_RADIUS);
    GRect   box   = GRect(lc.x - HOUR_LABEL_HALF_W,
                          lc.y - HOUR_LABEL_HALF_H,
                          HOUR_LABEL_HALF_W * 2,
                          HOUR_LABEL_HALF_H * 2);

    draw_outlined_text(ctx, label, font, box, GColorWhite, GColorBlack);
}

void draw_hour_markers(GContext *ctx, GPoint center) {
    GFont font = fonts_get_system_font(HOUR_NUMBER_FONT_KEY);

    for (int hour = 0; hour < 24; hour++) {
        draw_hour_tick(ctx, center, hour);
        if (hour % 2 == 0) {
            draw_hour_label(ctx, center, hour, font);
        }
    }
}

// ---------------------------------------------------------------------------
// 15-minute tick marks
// ---------------------------------------------------------------------------

void draw_quarter_hour_markers(GContext *ctx, GPoint center) {
    for (int minute = 0; minute < MINUTES_PER_DAY; minute += 15) {
        int32_t angle = minutes_to_angle(minute);
        GPoint  inner = point_on_circle(center, angle, TICK_INNER_ODD_RADIUS);
        GPoint  outer = point_on_circle(center, angle, TICK_OUTER_RADIUS);

        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_stroke_width(ctx, 1);
        graphics_draw_line(ctx, inner, outer);
    }
}
