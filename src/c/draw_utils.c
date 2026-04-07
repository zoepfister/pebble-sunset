#include "draw_utils.h"

#define OUTLINE_OFFSET 1

static const int8_t s_outline_dx[] = { -1,  0,  1, -1, 1, -1, 0, 1 };
static const int8_t s_outline_dy[] = { -1, -1, -1,  0, 0,  1, 1, 1 };

void fill_radial_wrapped(GContext *ctx, GRect rect, int inset,
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

void draw_outlined_text(GContext *ctx, const char *text, GFont font,
                        GRect box, GColor fg, GColor outline) {
    for (int i = 0; i < 8; i++) {
        GRect offset_box = GRect(
            box.origin.x + s_outline_dx[i] * OUTLINE_OFFSET,
            box.origin.y + s_outline_dy[i] * OUTLINE_OFFSET,
            box.size.w, box.size.h);
        graphics_context_set_text_color(ctx, outline);
        graphics_draw_text(ctx, text, font, offset_box,
                           GTextOverflowModeTrailingEllipsis,
                           GTextAlignmentCenter, NULL);
    }
    graphics_context_set_text_color(ctx, fg);
    graphics_draw_text(ctx, text, font, box,
                       GTextOverflowModeTrailingEllipsis,
                       GTextAlignmentCenter, NULL);
}

void draw_outlined_line(GContext *ctx, GPoint a, GPoint b,
                        int width, GColor fg, GColor outline) {
    graphics_context_set_stroke_color(ctx, outline);
    graphics_context_set_stroke_width(ctx, width + OUTLINE_OFFSET * 2);
    graphics_draw_line(ctx, a, b);

    graphics_context_set_stroke_color(ctx, fg);
    graphics_context_set_stroke_width(ctx, width);
    graphics_draw_line(ctx, a, b);
}
