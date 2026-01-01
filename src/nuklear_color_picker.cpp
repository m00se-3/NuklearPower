#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ==============================================================
   *
   *                          COLOR PICKER
   *
   * ===============================================================*/
  NK_LIB bool
  color_picker_behavior(flag* state,
                        const rectf* bounds, const rectf* matrix,
                        const rectf* hue_bar, const rectf* alpha_bar,
                        colorf* color, const input* in) {
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
      hsva[1] = NK_SATURATE((in->mouse.pos.x - matrix->x) / (matrix->w - 1));
      hsva[2] = 1.0f - NK_SATURATE((in->mouse.pos.y - matrix->y) / (matrix->h - 1));
      value_changed = hsv_changed = 1;
    }
    /* hue bar */
    if (button_behavior(state, *hue_bar, in, btn_behavior::BUTTON_REPEATER)) {
      hsva[0] = NK_SATURATE((in->mouse.pos.y - hue_bar->y) / (hue_bar->h - 1));
      value_changed = hsv_changed = 1;
    }
    /* alpha bar */
    if (alpha_bar) {
      if (button_behavior(state, *alpha_bar, in, btn_behavior::BUTTON_REPEATER)) {
        hsva[3] = 1.0f - NK_SATURATE((in->mouse.pos.y - alpha_bar->y) / (alpha_bar->h - 1));
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
  draw_color_picker(command_buffer* o, const rectf* matrix,
                    const rectf* hue_bar, const rectf* alpha_bar,
                    const colorf col) {
    NK_STORAGE const color black = {0, 0, 0, 255};
    NK_STORAGE const color white = {255, 255, 255, 255};
    NK_STORAGE const color black_trans = {0, 0, 0, 0};

    const float crosshair_size = 7.0f;
    float hsva[4];

    NK_ASSERT(o);
    NK_ASSERT(matrix);
    NK_ASSERT(hue_bar);

    /* draw hue bar */
    colorf_hsva_fv(hsva, col);
    for (int i = 0; i < 6; ++i) {
      static const color hue_colors[] = {
          {255, 0, 0, 255}, {255, 255, 0, 255}, {0, 255, 0, 255}, {0, 255, 255, 255}, {0, 0, 255, 255}, {255, 0, 255, 255}, {255, 0, 0, 255}};
      fill_rect_multi_color(o,
                            rect(hue_bar->x, hue_bar->y + (float) i * (hue_bar->h / 6.0f) + 0.5f,
                                 hue_bar->w, (hue_bar->h / 6.0f) + 0.5f),
                            hue_colors[i], hue_colors[i],
                            hue_colors[i + 1], hue_colors[i + 1]);
    }
    float line_y = (float) (int) (hue_bar->y + hsva[0] * matrix->h + 0.5f);
    stroke_line(o, hue_bar->x - 1, line_y, hue_bar->x + hue_bar->w + 2,
                line_y, 1, rgb(255, 255, 255));

    /* draw alpha bar */
    if (alpha_bar) {
      float alpha = NK_SATURATE(col.a);
      line_y = (float) (int) (alpha_bar->y + (1.0f - alpha) * matrix->h + 0.5f);

      fill_rect_multi_color(o, *alpha_bar, white, white, black, black);
      stroke_line(o, alpha_bar->x - 1, line_y, alpha_bar->x + alpha_bar->w + 2,
                  line_y, 1, rgb(255, 255, 255));
    }

    /* draw color matrix */
    const color temp = hsv_f(hsva[0], 1.0f, 1.0f);
    fill_rect_multi_color(o, *matrix, white, temp, temp, white);
    fill_rect_multi_color(o, *matrix, black_trans, black_trans, black, black);

    /* draw cross-hair */
    {
      vec2f p;
      float S = hsva[1];
      float V = hsva[2];
      p.x = (float) (int) (matrix->x + S * matrix->w);
      p.y = (float) (int) (matrix->y + (1.0f - V) * matrix->h);
      stroke_line(o, p.x - crosshair_size, p.y, p.x - 2, p.y, 1.0f, white);
      stroke_line(o, p.x + crosshair_size + 1, p.y, p.x + 3, p.y, 1.0f, white);
      stroke_line(o, p.x, p.y + crosshair_size + 1, p.x, p.y + 3, 1.0f, white);
      stroke_line(o, p.x, p.y - crosshair_size, p.x, p.y - 2, 1.0f, white);
    }
  }
  NK_LIB bool
  do_color_picker(flag* state,
                  command_buffer* out, colorf* col,
                  const color_format fmt, rectf bounds,
                  const vec2f padding, const input* in,
                  const user_font* font) {
    int ret = 0;
    rectf matrix;
    rectf hue_bar;
    rectf alpha_bar;

    NK_ASSERT(out);
    NK_ASSERT(col);
    NK_ASSERT(state);
    NK_ASSERT(font);
    if (!out || !col || !state || !font)
      return ret;

    float bar_w = font->height;
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
                                (fmt == color_format::RGBA) ? &alpha_bar : 0, col, in);
    draw_color_picker(out, &matrix, &hue_bar, (fmt == color_format::RGBA) ? &alpha_bar : 0, *col);
    return ret;
  }
  NK_API bool
  color_pick(context* ctx, colorf* color,
             const color_format fmt) {

    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(color);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !color)
      return 0;

    window* win = ctx->current;
    const style* config = &ctx->style;
    panel* layout = win->layout;
    const widget_layout_states state = widget(&bounds, ctx);
    if (!state)
      return 0;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    return do_color_picker(&ctx->last_widget_state, &win->buffer, color, fmt, bounds,
                           vec2_from_floats(0, 0), in, config->font);
  }
  NK_API colorf
  color_picker(context* ctx, colorf color,
               const color_format fmt) {
    color_pick(ctx, &color, fmt);
    return color;
  }
} // namespace nk
