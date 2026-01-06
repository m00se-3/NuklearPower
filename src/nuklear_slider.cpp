#include <nk/nuklear.hpp>
#include <algorithm>

namespace nk {
  /* ===============================================================
   *
   *                              SLIDER
   *
   * ===============================================================*/
  NK_LIB float
  slider_behavior(flag* state, rectf* logical_cursor,
                  rectf* visual_cursor, input* in,
                  const rectf bounds, float slider_min, float slider_max, float slider_value,
                  float slider_step, float slider_steps) {

    /* check if visual cursor is being dragged */
    nk::widget_state_reset(state);
    const int left_mouse_down = in && in->mouse.buttons[NK_BUTTON_LEFT].down;
    const int left_mouse_click_in_cursor = in && input_has_mouse_click_down_in_rect(in,
                                                                              NK_BUTTON_LEFT, *visual_cursor, true);

    if (left_mouse_down && left_mouse_click_in_cursor) {
      float ratio = 0;
      const float d = in->mouse.pos.x - (visual_cursor->x + visual_cursor->w * 0.5f);
      const float pxstep = bounds.w / slider_steps;

      /* only update value if the next slider step is reached */
      *state = NK_WIDGET_STATE_ACTIVE;
      if (NK_ABS(d) >= pxstep) {
        const float steps = (float) ((int) (NK_ABS(d) / pxstep));
        slider_value += (d > 0) ? (slider_step * steps) : -(slider_step * steps);
        slider_value = std::clamp(slider_min, slider_value, slider_max);
        ratio = (slider_value - slider_min) / slider_step;
        logical_cursor->x = bounds.x + (logical_cursor->w * ratio);
        in->mouse.buttons[NK_BUTTON_LEFT].clicked_pos.x = logical_cursor->x;
      }
    }

    /* slider widget state */
    if (input_is_mouse_hovering_rect(in, bounds))
      *state = NK_WIDGET_STATE_HOVERED;
    if (*state & NK_WIDGET_STATE_HOVER &&
        !input_is_mouse_prev_hovering_rect(in, bounds))
      *state |= NK_WIDGET_STATE_ENTERED;
    else if (input_is_mouse_prev_hovering_rect(in, bounds))
      *state |= NK_WIDGET_STATE_LEFT;
    return slider_value;
  }
  NK_LIB void
  draw_slider(command_buffer* out, const flag state,
              const style_slider* style, const rectf* bounds,
              const rectf* visual_cursor, float min, float value, float max) {
    rectf fill;
    rectf bar;
    const style_item* background;

    /* select correct slider images/colors */
    color bar_color;
    const style_item* cursor;

    NK_UNUSED(min);
    NK_UNUSED(max);
    NK_UNUSED(value);

    if (state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->active;
      bar_color = style->bar_active;
      cursor = &style->cursor_active;
    } else if (state & NK_WIDGET_STATE_HOVER) {
      background = &style->hover;
      bar_color = style->bar_hover;
      cursor = &style->cursor_hover;
    } else {
      background = &style->normal;
      bar_color = style->bar_normal;
      cursor = &style->cursor_normal;
    }

    /* calculate slider background bar */
    bar.x = bounds->x;
    bar.y = (visual_cursor->y + visual_cursor->h / 2) - bounds->h / 12;
    bar.w = bounds->w;
    bar.h = bounds->h / 6;

    /* filled background bar style */
    fill.w = (visual_cursor->x + (visual_cursor->w / 2.0f)) - bar.x;
    fill.x = bar.x;
    fill.y = bar.y;
    fill.h = bar.h;

    /* draw background */
    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        draw_image(out, *bounds, &background->data.image, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        draw_nine_slice(out, *bounds, &background->data.slice, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        fill_rect(out, *bounds, style->rounding, rgb_factor(background->data.color, style->color_factor));
        stroke_rect(out, *bounds, style->rounding, style->border, rgb_factor(style->border_color, style->color_factor));
        break;
    }

    /* draw slider bar */
    fill_rect(out, bar, style->rounding, rgb_factor(bar_color, style->color_factor));
    fill_rect(out, fill, style->rounding, rgb_factor(style->bar_filled, style->color_factor));

    /* draw cursor */
    if (cursor->type == style_item_type::STYLE_ITEM_IMAGE)
      draw_image(out, *visual_cursor, &cursor->data.image, rgb_factor(white, style->color_factor));
    else
      fill_circle(out, *visual_cursor, rgb_factor(cursor->data.color, style->color_factor));
  }
  NK_LIB float
  do_slider(flag* state,
            command_buffer* out, rectf bounds,
            float min, float val, float max, float step,
            const style_slider* style, input* in,
            const user_font* font) {

    rectf visual_cursor;
    rectf logical_cursor;

    NK_ASSERT(style);
    NK_ASSERT(out);
    if (!out || !style)
      return 0;

    /* remove padding from slider bounds */
    bounds.x = bounds.x + style->padding.x;
    bounds.y = bounds.y + style->padding.y;
    bounds.h = std::max(bounds.h, 2 * style->padding.y);
    bounds.w = std::max(bounds.w, 2 * style->padding.x + style->cursor_size.x);
    bounds.w -= 2 * style->padding.x;
    bounds.h -= 2 * style->padding.y;

    /* optional buttons */
    if (style->show_buttons) {
      flag ws;
      rectf button;
      button.y = bounds.y;
      button.w = bounds.h;
      button.h = bounds.h;

      /* decrement button */
      button.x = bounds.x;
      if (do_button_symbol(&ws, out, button, style->dec_symbol, btn_behavior::BUTTON_DEFAULT,
                           &style->dec_button, in, font))
        val -= step;

      /* increment button */
      button.x = (bounds.x + bounds.w) - button.w;
      if (do_button_symbol(&ws, out, button, style->inc_symbol, btn_behavior::BUTTON_DEFAULT,
                           &style->inc_button, in, font))
        val += step;

      bounds.x = bounds.x + button.w + style->spacing.x;
      bounds.w = bounds.w - (2 * button.w + 2 * style->spacing.x);
    }

    /* remove one cursor size to support visual cursor */
    bounds.x += style->cursor_size.x * 0.5f;
    bounds.w -= style->cursor_size.x;

    /* make sure the provided values are correct */
    float slider_max = std::max(min, max);
    float slider_min = std::min(min, max);
    float slider_value = std::clamp(slider_min, val, slider_max);
    float slider_range = slider_max - slider_min;
    float slider_steps = slider_range / step;
    float cursor_offset = (slider_value - slider_min) / step;

    /* calculate cursor
    Basically you have two cursors. One for visual representation and interaction
    and one for updating the actual cursor value. */
    logical_cursor.h = bounds.h;
    logical_cursor.w = bounds.w / slider_steps;
    logical_cursor.x = bounds.x + (logical_cursor.w * cursor_offset);
    logical_cursor.y = bounds.y;

    visual_cursor.h = style->cursor_size.y;
    visual_cursor.w = style->cursor_size.x;
    visual_cursor.y = (bounds.y + bounds.h * 0.5f) - visual_cursor.h * 0.5f;
    visual_cursor.x = logical_cursor.x - visual_cursor.w * 0.5f;

    slider_value = slider_behavior(state, &logical_cursor, &visual_cursor,
                                   in, bounds, slider_min, slider_max, slider_value, step, slider_steps);
    visual_cursor.x = logical_cursor.x - visual_cursor.w * 0.5f;

    /* draw slider */
    if (style->draw_begin)
      style->draw_begin(out, style->userdata);
    draw_slider(out, *state, style, &bounds, &visual_cursor, slider_min, slider_value, slider_max);
    if (style->draw_end)
      style->draw_end(out, style->userdata);
    return slider_value;
  }
  NK_API bool
  slider_float(context* ctx, float min_value, float* value, float max_value,
               float value_step) {

    const int ret = 0;
    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    NK_ASSERT(value);
    if (!ctx || !ctx->current || !ctx->current->layout || !value)
      return ret;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const panel* layout = win->layout;

    const widget_layout_states state = widget(&bounds, ctx);
    if (!state)
      return ret;
    input* in = (/*state == NK_WIDGET_ROM || */ state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;

    float old_value = *value;
    *value = do_slider(&ctx->last_widget_state, &win->buffer, bounds, min_value,
                       old_value, max_value, value_step, &style->slider, in, style->font);
    return (old_value > *value || old_value < *value);
  }
  NK_API float
  slide_float(context* ctx, float min, float val, float max, float step) {
    slider_float(ctx, min, &val, max, step);
    return val;
  }
  NK_API int
  slide_int(context* ctx, const int min, const int val, const int max, const int step) {
    float value = (float) val;
    slider_float(ctx, (float) min, &value, (float) max, (float) step);
    return (int) value;
  }
  NK_API bool
  slider_int(context* ctx, const int min, int* val, const int max, const int step) {
    float value = (float) *val;
    const int ret = slider_float(ctx, (float) min, &value, (float) max, (float) step);
    *val = (int) value;
    return ret;
  }
} // namespace nk
