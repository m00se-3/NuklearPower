#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ==============================================================
   *
   *                          COLOR PICKER
   *
   * ===============================================================*/
  NK_LIB bool
  color_picker_behavior(flag *state,
      const struct rectf *bounds, const struct rectf *matrix,
      const struct rectf *hue_bar, const struct rectf *alpha_bar,
      struct colorf *color, const struct input *in)
  {
    float hsva[4];
    bool value_changed = 0;
    bool hsv_changed = 0;

    NK_ASSERT(state);
    NK_ASSERT(matrix);
    NK_ASSERT(hue_bar);
    NK_ASSERT(color);

    /* color matrix */
    colorf_hsva_fv(hsva, *color);
    if (button_behavior(state, *matrix, in, btn_behavior::BUTTON_REPEATER)) {
      hsva[1] = NK_SATURATE((in->mouse.pos.x - matrix->x) / (matrix->w-1));
      hsva[2] = 1.0f - NK_SATURATE((in->mouse.pos.y - matrix->y) / (matrix->h-1));
      value_changed = hsv_changed = 1;
    }
    /* hue bar */
    if (button_behavior(state, *hue_bar, in, btn_behavior::BUTTON_REPEATER)) {
      hsva[0] = NK_SATURATE((in->mouse.pos.y - hue_bar->y) / (hue_bar->h-1));
      value_changed = hsv_changed = 1;
    }
    /* alpha bar */
    if (alpha_bar) {
      if (button_behavior(state, *alpha_bar, in, btn_behavior::BUTTON_REPEATER)) {
        hsva[3] = 1.0f - NK_SATURATE((in->mouse.pos.y - alpha_bar->y) / (alpha_bar->h-1));
        value_changed = 1;
      }
    }
    widget_state_reset(state);
    if (hsv_changed) {
      *color = hsva_colorfv(hsva);
      *state = NK_WIDGET_STATE_ACTIVE;
    }
    if (value_changed) {
      color->a = hsva[3];
      *state = NK_WIDGET_STATE_ACTIVE;
    }
    /* set color picker widget state */
    if (input_is_mouse_hovering_rect(in, *bounds))
      *state = NK_WIDGET_STATE_HOVERED;
    if (*state & NK_WIDGET_STATE_HOVER && !input_is_mouse_prev_hovering_rect(in, *bounds))
      *state |= NK_WIDGET_STATE_ENTERED;
    else if (input_is_mouse_prev_hovering_rect(in, *bounds))
      *state |= NK_WIDGET_STATE_LEFT;
    return value_changed;
  }
  NK_LIB void
  draw_color_picker(struct command_buffer *o, const struct rectf *matrix,
      const struct rectf *hue_bar, const struct rectf *alpha_bar,
      struct colorf col)
  {
    NK_STORAGE const struct color black = {0,0,0,255};
    NK_STORAGE const struct color white = {255, 255, 255, 255};
    NK_STORAGE const struct color black_trans = {0,0,0,0};

    const float crosshair_size = 7.0f;
    struct color temp;
    float hsva[4];
    float line_y;
    int i;

    NK_ASSERT(o);
    NK_ASSERT(matrix);
    NK_ASSERT(hue_bar);

    /* draw hue bar */
    colorf_hsva_fv(hsva, col);
    for (i = 0; i < 6; ++i) {
      static const struct color hue_colors[] = {
        {255, 0, 0, 255}, {255,255,0,255}, {0,255,0,255}, {0, 255,255,255},
        {0,0,255,255}, {255, 0, 255, 255}, {255, 0, 0, 255}
      };
      fill_rect_multi_color(o,
          rect(hue_bar->x, hue_bar->y + (float)i * (hue_bar->h/6.0f) + 0.5f,
              hue_bar->w, (hue_bar->h/6.0f) + 0.5f), hue_colors[i], hue_colors[i],
              hue_colors[i+1], hue_colors[i+1]);
    }
    line_y = (float)(int)(hue_bar->y + hsva[0] * matrix->h + 0.5f);
    stroke_line(o, hue_bar->x-1, line_y, hue_bar->x + hue_bar->w + 2,
        line_y, 1, rgb(255,255,255));

    /* draw alpha bar */
    if (alpha_bar) {
      float alpha = NK_SATURATE(col.a);
      line_y = (float)(int)(alpha_bar->y +  (1.0f - alpha) * matrix->h + 0.5f);

      fill_rect_multi_color(o, *alpha_bar, white, white, black, black);
      stroke_line(o, alpha_bar->x-1, line_y, alpha_bar->x + alpha_bar->w + 2,
          line_y, 1, rgb(255,255,255));
    }

    /* draw color matrix */
    temp = hsv_f(hsva[0], 1.0f, 1.0f);
    fill_rect_multi_color(o, *matrix, white, temp, temp, white);
    fill_rect_multi_color(o, *matrix, black_trans, black_trans, black, black);

    /* draw cross-hair */
    {struct vec2f p; float S = hsva[1]; float V = hsva[2];
      p.x = (float)(int)(matrix->x + S * matrix->w);
      p.y = (float)(int)(matrix->y + (1.0f - V) * matrix->h);
      stroke_line(o, p.x - crosshair_size, p.y, p.x-2, p.y, 1.0f, white);
      stroke_line(o, p.x + crosshair_size + 1, p.y, p.x+3, p.y, 1.0f, white);
      stroke_line(o, p.x, p.y + crosshair_size + 1, p.x, p.y+3, 1.0f, white);
      stroke_line(o, p.x, p.y - crosshair_size, p.x, p.y-2, 1.0f, white);}
  }
  NK_LIB bool
  do_color_picker(flag *state,
      struct command_buffer *out, struct colorf *col,
      enum color_format fmt, struct rectf bounds,
      struct vec2f padding, const struct input *in,
      const struct user_font *font)
  {
    int ret = 0;
    struct rectf matrix;
    struct rectf hue_bar;
    struct rectf alpha_bar;
    float bar_w;

    NK_ASSERT(out);
    NK_ASSERT(col);
    NK_ASSERT(state);
    NK_ASSERT(font);
    if (!out || !col || !state || !font)
      return ret;

    bar_w = font->height;
    bounds.x += padding.x;
    bounds.y += padding.x;
    bounds.w -= 2 * padding.x;
    bounds.h -= 2 * padding.y;

    matrix.x = bounds.x;
    matrix.y = bounds.y;
    matrix.h = bounds.h;
    matrix.w = bounds.w - (3 * padding.x + 2 * bar_w);

    hue_bar.w = bar_w;
    hue_bar.y = bounds.y;
    hue_bar.h = matrix.h;
    hue_bar.x = matrix.x + matrix.w + padding.x;

    alpha_bar.x = hue_bar.x + hue_bar.w + padding.x;
    alpha_bar.y = bounds.y;
    alpha_bar.w = bar_w;
    alpha_bar.h = matrix.h;

    ret = color_picker_behavior(state, &bounds, &matrix, &hue_bar,
        (fmt == color_format::RGBA) ? &alpha_bar:0, col, in);
    draw_color_picker(out, &matrix, &hue_bar, (fmt == color_format::RGBA) ? &alpha_bar:0, *col);
    return ret;
  }
  NK_API bool
  color_pick(struct context * ctx, struct colorf *color,
      enum color_format fmt)
  {
    struct window *win;
    struct panel *layout;
    const struct style *config;
    const struct input *in;

    enum widget_layout_states state;
    struct rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(color);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !color)
      return 0;

    win = ctx->current;
    config = &ctx->style;
    layout = win->layout;
    state = widget(&bounds, ctx);
    if (!state) return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    return do_color_picker(&ctx->last_widget_state, &win->buffer, color, fmt, bounds,
                vec2_from_floats(0,0), in, config->font);
  }
  NK_API struct colorf
  color_picker(struct context *ctx, struct colorf color,
      enum color_format fmt)
  {
    color_pick(ctx, &color, fmt);
    return color;
  }
}
