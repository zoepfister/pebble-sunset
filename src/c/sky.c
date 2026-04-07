#include "sky.h"
#include "layout.h"
#include "geometry.h"
#include "draw_utils.h"

// Sky-layer timing thresholds (minutes)
#define TWILIGHT_DURATION        75
#define CLEAR_SKY_INSET          45
#define GOLDEN_HOUR_INNER        12
#define GOLDEN_HOUR_OUTER        45
#define HORIZON_GLOW_INNER       12
#define HORIZON_GLOW_OUTER       18

// Minimum day length (minutes) required to show each sky layer
#define MIN_DAY_FOR_GOLDEN_HOUR   90
#define MIN_DAY_FOR_CLEAR_SKY    120
#define MIN_DAY_FOR_BRIGHT_SKY   180
#define MIN_DAY_FOR_NOON_ZENITH  240

typedef struct {
    int rise;
    int set;
    int day_length;
    int noon;
} SunTiming;

static SunTiming compute_sun_timing(int sunrise, int sunset) {
    return (SunTiming) {
        .rise       = sunrise,
        .set        = sunset,
        .day_length = sunset - sunrise,
        .noon       = (sunrise + sunset) / 2
    };
}

static void fill_sky_band(GContext *ctx, GRect disc, int fill,
                          GColor color, int from_min, int to_min) {
    graphics_context_set_fill_color(ctx, color);
    fill_radial_wrapped(ctx, disc, fill,
                        minutes_to_angle(from_min),
                        minutes_to_angle(to_min));
}

static void draw_night_base(GContext *ctx, GRect disc, int fill) {
    graphics_context_set_fill_color(ctx, GColorOxfordBlue);
    graphics_fill_radial(ctx, disc, GOvalScaleModeFitCircle,
                         fill, 0, TRIG_MAX_ANGLE);
}

static void draw_twilight(GContext *ctx, GRect disc, int fill, SunTiming t) {
    fill_sky_band(ctx, disc, fill, GColorDukeBlue,
                  t.rise - TWILIGHT_DURATION, t.rise);
    fill_sky_band(ctx, disc, fill, GColorDukeBlue,
                  t.set, t.set + TWILIGHT_DURATION);
}

static void draw_daylight(GContext *ctx, GRect disc, int fill, SunTiming t) {
    fill_sky_band(ctx, disc, fill, GColorCobaltBlue, t.rise, t.set);
}

static void draw_clear_sky(GContext *ctx, GRect disc, int fill, SunTiming t) {
    if (t.day_length <= MIN_DAY_FOR_CLEAR_SKY) return;
    fill_sky_band(ctx, disc, fill, GColorVividCerulean,
                  t.rise + CLEAR_SKY_INSET, t.set - CLEAR_SKY_INSET);
}

static void draw_bright_sky(GContext *ctx, GRect disc, int fill, SunTiming t) {
    if (t.day_length <= MIN_DAY_FOR_BRIGHT_SKY) return;
    int third = t.day_length / 3;
    fill_sky_band(ctx, disc, fill, GColorPictonBlue,
                  t.noon - third, t.noon + third);
}

static void draw_noon_zenith(GContext *ctx, GRect disc, int fill, SunTiming t) {
    if (t.day_length <= MIN_DAY_FOR_NOON_ZENITH) return;
    int seventh = t.day_length / 7;
    fill_sky_band(ctx, disc, fill, GColorCeleste,
                  t.noon - seventh, t.noon + seventh);
}

static void draw_golden_hour(GContext *ctx, GRect disc, int fill, SunTiming t) {
    if (t.day_length <= MIN_DAY_FOR_GOLDEN_HOUR) return;
    fill_sky_band(ctx, disc, fill, GColorOrange,
                  t.rise + GOLDEN_HOUR_INNER, t.rise + GOLDEN_HOUR_OUTER);
    fill_sky_band(ctx, disc, fill, GColorOrange,
                  t.set - GOLDEN_HOUR_OUTER, t.set - GOLDEN_HOUR_INNER);
}

static void draw_horizon_glow(GContext *ctx, GRect disc, int fill, SunTiming t) {
    fill_sky_band(ctx, disc, fill, GColorChromeYellow,
                  t.rise - HORIZON_GLOW_INNER, t.rise + HORIZON_GLOW_OUTER);
    fill_sky_band(ctx, disc, fill, GColorChromeYellow,
                  t.set - HORIZON_GLOW_OUTER, t.set + HORIZON_GLOW_INNER);
}

static void draw_sky_border(GContext *ctx, GRect disc) {
    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_arc(ctx, disc, GOvalScaleModeFitCircle, 0, TRIG_MAX_ANGLE);
}

void draw_sky(GContext *ctx, GRect bounds, int sunrise_min, int sunset_min) {
    GRect     disc   = grect_inset(bounds, GEdgeInsets(SKY_DISC_INSET));
    int       fill   = disc.size.w / 2;
    SunTiming timing = compute_sun_timing(sunrise_min, sunset_min);

    draw_night_base(ctx, disc, fill);
    draw_twilight(ctx, disc, fill, timing);
    draw_daylight(ctx, disc, fill, timing);
    draw_clear_sky(ctx, disc, fill, timing);
    draw_bright_sky(ctx, disc, fill, timing);
    draw_noon_zenith(ctx, disc, fill, timing);
    draw_golden_hour(ctx, disc, fill, timing);
    draw_horizon_glow(ctx, disc, fill, timing);
    draw_sky_border(ctx, disc);
}
