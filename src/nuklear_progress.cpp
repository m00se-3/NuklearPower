#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                          PROGRESS
   *
   * ===============================================================*/
  NK_LIB std::size_t
  progress_behavior(flag* state, struct input* in,
                    rectf r, rectf cursor, std::size_t max, std::size_t value, bool modifiable) {
    int left_mouse_down = 0;
    int left_mouse_click_in_cursor = 0;

    nk::widget_state_reset(state);
    if (!in || !modifiable)
      return value;
    left_mouse_down = in && in->mouse.buttons[NK_BUTTON_LEFT].down;
    left_mouse_click_in_cursor = in && input_has_mouse_click_down_in_rect(in,
                                                                          NK_BUTTON_LEFT, cursor, true);
    if (input_is_mouse_hovering_rect(in, r))
      *state = NK_WIDGET_STATE_HOVERED;

    if (in && left_mouse_down && left_mouse_click_in_cursor) {
      if (left_mouse_down && left_mouse_click_in_cursor) {
        float ratio = NK_MAX(0, (float) (in->mouse.pos.x - cursor.x)) / (float) cursor.w;
        value = (std::size_t) NK_CLAMP(0, (float) max * ratio, (float) max);
        in->mouse.buttons[NK_BUTTON_LEFT].clicked_pos.x = cursor.x + cursor.w / 2.0f;
        *state |= NK_WIDGET_STATE_ACTIVE;
      }
    }
    /* set progressbar widget state */
    if (*state & NK_WIDGET_STATE_HOVER && !input_is_mouse_prev_hovering_rect(in, r))
      *state |= NK_WIDGET_STATE_ENTERED;
    else if (input_is_mouse_prev_hovering_rect(in, r))
      *state |= NK_WIDGET_STATE_LEFT;
    return value;
  }
  NK_LIB void
  draw_progress(struct command_buffer* out, flag state,
                const struct style_progress* style, const rectf* bounds,
                const rectf* scursor, std::size_t value, std::size_t max) {
    const struct style_item* background;
    const struct style_item* cursor;

    NK_UNUSED(max);
    NK_UNUSED(value);

    /* select correct colors/images to draw */
    if (state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->active;
      cursor = &style->cursor_active;
    } else if (state & NK_WIDGET_STATE_HOVER) {
      background = &style->hover;
      cursor = &style->cursor_hover;
    } else {
      background = &style->normal;
      cursor = &style->cursor_normal;
    }

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

    /* draw cursor */
    switch (cursor->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        draw_image(out, *scursor, &cursor->data.image, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        draw_nine_slice(out, *scursor, &cursor->data.slice, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        fill_rect(out, *scursor, style->rounding, rgb_factor(cursor->data.color, style->color_factor));
        stroke_rect(out, *scursor, style->rounding, style->border, rgb_factor(style->border_color, style->color_factor));
        break;
    }
  }
  NK_LIB std::size_t
  do_progress(flag* state,
              struct command_buffer* out, rectf bounds,
              std::size_t value, std::size_t max, bool modifiable,
              const struct style_progress* style, struct input* in) {
    float prog_scale;
    std::size_t prog_value;
    rectf cursor;

    NK_ASSERT(style);
    NK_ASSERT(out);
    if (!out || !style)
      return 0;

    /* calculate progressbar cursor */
    cursor.w = NK_MAX(bounds.w, 2 * style->padding.x + 2 * style->border);
    cursor.h = NK_MAX(bounds.h, 2 * style->padding.y + 2 * style->border);
    cursor = pad_rect(bounds, vec2_from_floats(style->padding.x + style->border, style->padding.y + style->border));
    prog_scale = (float) value / (float) max;

    /* update progressbar */
    prog_value = NK_MIN(value, max);
    prog_value = progress_behavior(state, in, bounds, cursor, max, prog_value, modifiable);
    cursor.w = cursor.w * prog_scale;

    /* draw progressbar */
    if (style->draw_begin)
      style->draw_begin(out, style->userdata);
    draw_progress(out, *state, style, &bounds, &cursor, value, max);
    if (style->draw_end)
      style->draw_end(out, style->userdata);
    return prog_value;
  }
  NK_API bool
  progress(struct context* ctx, std::size_t* cur, std::size_t max, bool is_modifyable) {
    struct window* win;
    struct panel* layout;
    const struct style* style;
    struct input* in;

    rectf bounds;
    enum widget_layout_states state;
    std::size_t old_value;

    NK_ASSERT(ctx);
    NK_ASSERT(cur);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !cur)
      return 0;

    win = ctx->current;
    style = &ctx->style;
    layout = win->layout;
    state = widget(&bounds, ctx);
    if (!state)
      return 0;

    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    old_value = *cur;
    *cur = do_progress(&ctx->last_widget_state, &win->buffer, bounds,
                       *cur, max, is_modifyable, &style->progress, in);
    return (*cur != old_value);
  }
  NK_API std::size_t
  prog(struct context* ctx, std::size_t cur, std::size_t max, bool modifyable) {
    progress(ctx, &cur, max, modifyable);
    return cur;
  }
} // namespace nk
