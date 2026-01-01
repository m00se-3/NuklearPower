#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              TOGGLE
   *
   * ===============================================================*/
  NK_LIB bool
  toggle_behavior(const input* in, const rectf select,
                  flag* state, bool active) {
    nk::widget_state_reset(state);
    if (button_behavior(state, select, in, btn_behavior::BUTTON_DEFAULT)) {
      *state = NK_WIDGET_STATE_ACTIVE;
      active = !active;
    }
    if (*state & NK_WIDGET_STATE_HOVER && !input_is_mouse_prev_hovering_rect(in, select))
      *state |= NK_WIDGET_STATE_ENTERED;
    else if (input_is_mouse_prev_hovering_rect(in, select))
      *state |= NK_WIDGET_STATE_LEFT;
    return active;
  }
  NK_LIB void
  draw_checkbox(command_buffer* out,
                const flag state, const style_toggle* style, const bool active,
                const rectf* label, const rectf* selector,
                const rectf* cursors, const char* string, const int len,
                const user_font* font, const flag text_alignment) {
    const style_item* background;
    const style_item* cursor;
    text text;

    /* select correct colors/images */
    if (state & NK_WIDGET_STATE_HOVER) {
      background = &style->hover;
      cursor = &style->cursor_hover;
      text.txt = style->text_hover;
    } else if (state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->hover;
      cursor = &style->cursor_hover;
      text.txt = style->text_active;
    } else {
      background = &style->normal;
      cursor = &style->cursor_normal;
      text.txt = style->text_normal;
    }

    text.txt = rgb_factor(text.txt, style->color_factor);
    text.padding.x = 0;
    text.padding.y = 0;
    text.background = style->text_background;
    widget_text(out, *label, string, len, &text, text_alignment, font);

    /* draw background and cursor */
    if (background->type == style_item_type::STYLE_ITEM_COLOR) {
      fill_rect(out, *selector, 0, rgb_factor(style->border_color, style->color_factor));
      fill_rect(out, shrirect(*selector, style->border), 0, rgb_factor(background->data.color, style->color_factor));
    } else
      draw_image(out, *selector, &background->data.image, rgb_factor(white, style->color_factor));
    if (active) {
      if (cursor->type == style_item_type::STYLE_ITEM_IMAGE)
        draw_image(out, *cursors, &cursor->data.image, rgb_factor(white, style->color_factor));
      else
        fill_rect(out, *cursors, 0, cursor->data.color);
    }
  }
  NK_LIB void
  draw_option(command_buffer* out,
              const flag state, const style_toggle* style, const bool active,
              const rectf* label, const rectf* selector,
              const rectf* cursors, const char* string, const int len,
              const user_font* font, const flag text_alignment) {
    const style_item* background;
    const style_item* cursor;
    text text;

    /* select correct colors/images */
    if (state & NK_WIDGET_STATE_HOVER) {
      background = &style->hover;
      cursor = &style->cursor_hover;
      text.txt = style->text_hover;
    } else if (state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->hover;
      cursor = &style->cursor_hover;
      text.txt = style->text_active;
    } else {
      background = &style->normal;
      cursor = &style->cursor_normal;
      text.txt = style->text_normal;
    }

    text.txt = rgb_factor(text.txt, style->color_factor);
    text.padding.x = 0;
    text.padding.y = 0;
    text.background = style->text_background;
    widget_text(out, *label, string, len, &text, text_alignment, font);

    /* draw background and cursor */
    if (background->type == style_item_type::STYLE_ITEM_COLOR) {
      fill_circle(out, *selector, rgb_factor(style->border_color, style->color_factor));
      fill_circle(out, shrirect(*selector, style->border), rgb_factor(background->data.color, style->color_factor));
    } else
      draw_image(out, *selector, &background->data.image, rgb_factor(white, style->color_factor));
    if (active) {
      if (cursor->type == style_item_type::STYLE_ITEM_IMAGE)
        draw_image(out, *cursors, &cursor->data.image, rgb_factor(white, style->color_factor));
      else
        fill_circle(out, *cursors, cursor->data.color);
    }
  }
  NK_LIB bool
  do_toggle(flag* state,
            command_buffer* out, rectf r,
            bool* active, const char* str, const int len, const toggle_type type,
            const style_toggle* style, const input* in,
            const user_font* font, const flag widget_alignment, const flag text_alignment) {
    rectf bounds;
    rectf select;
    rectf cursor;
    rectf label;

    NK_ASSERT(style);
    NK_ASSERT(out);
    NK_ASSERT(font);
    if (!out || !style || !font || !active)
      return 0;

    r.w = std::max(r.w, font->height + 2 * style->padding.x);
    r.h = std::max(r.h, font->height + 2 * style->padding.y);

    /* add additional touch padding for touch screen devices */
    bounds.x = r.x - style->touch_padding.x;
    bounds.y = r.y - style->touch_padding.y;
    bounds.w = r.w + 2 * style->touch_padding.x;
    bounds.h = r.h + 2 * style->touch_padding.y;

    /* calculate the selector space */
    select.w = font->height;
    select.h = select.w;

    if (widget_alignment & NK_WIDGET_ALIGN_RIGHT) {
      select.x = r.x + r.w - font->height;

      /* label in front of the selector */
      label.x = r.x;
      label.w = r.w - select.w - style->spacing * 2;
    } else if (widget_alignment & NK_WIDGET_ALIGN_CENTERED) {
      select.x = r.x + (r.w - select.w) / 2;

      /* label in front of selector */
      label.x = r.x;
      label.w = (r.w - select.w - style->spacing * 2) / 2;
    } else { /* Default: NK_WIDGET_ALIGN_LEFT */
      select.x = r.x;

      /* label behind the selector */
      label.x = select.x + select.w + style->spacing;
      label.w = std::max(r.x + r.w, label.x) - label.x;
    }

    if (widget_alignment & NK_WIDGET_ALIGN_TOP) {
      select.y = r.y;
    } else if (widget_alignment & NK_WIDGET_ALIGN_BOTTOM) {
      select.y = r.y + r.h - select.h - 2 * style->padding.y;
    } else { /* Default: NK_WIDGET_ALIGN_MIDDLE */
      select.y = r.y + r.h / 2.0f - select.h / 2.0f;
    }

    label.y = select.y;
    label.h = select.w;

    /* calculate the bounds of the cursor inside the selector */
    cursor.x = select.x + style->padding.x + style->border;
    cursor.y = select.y + style->padding.y + style->border;
    cursor.w = select.w - (2 * style->padding.x + 2 * style->border);
    cursor.h = select.h - (2 * style->padding.y + 2 * style->border);

    /* update selector */
    const int was_active = *active;
    *active = toggle_behavior(in, bounds, state, *active);

    /* draw selector */
    if (style->draw_begin)
      style->draw_begin(out, style->userdata);
    if (type == NK_TOGGLE_CHECK) {
      draw_checkbox(out, *state, style, *active, &label, &select, &cursor, str, len, font, text_alignment);
    } else {
      draw_option(out, *state, style, *active, &label, &select, &cursor, str, len, font, text_alignment);
    }
    if (style->draw_end)
      style->draw_end(out, style->userdata);
    return (was_active != *active);
  }
  /*----------------------------------------------------------------
   *
   *                          CHECKBOX
   *
   * --------------------------------------------------------------*/
  NK_API bool
  check_text(context* ctx, const char* text, const int len, bool active) {

    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return active;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const panel* layout = win->layout;

    const widget_layout_states state = widget(&bounds, ctx);
    if (!state)
      return active;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    do_toggle(&ctx->last_widget_state, &win->buffer, bounds, &active,
              text, len, NK_TOGGLE_CHECK, &style->checkbox, in, style->font, NK_WIDGET_LEFT, NK_TEXT_LEFT);
    return active;
  }
  NK_API bool
  check_text_align(context* ctx, const char* text, const int len, bool active, const flag widget_alignment, const flag text_alignment) {

    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return active;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const panel* layout = win->layout;

    const widget_layout_states state = widget(&bounds, ctx);
    if (!state)
      return active;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    do_toggle(&ctx->last_widget_state, &win->buffer, bounds, &active,
              text, len, NK_TOGGLE_CHECK, &style->checkbox, in, style->font, widget_alignment, text_alignment);
    return active;
  }
  NK_API unsigned int
  check_flags_text(context* ctx, const char* text, const int len,
                   unsigned int flags, const unsigned int value) {
    NK_ASSERT(ctx);
    NK_ASSERT(text);
    if (!ctx || !text)
      return flags;
    const int old_active = (int) ((flags & value) & value);
    if (check_text(ctx, text, len, old_active))
      flags |= value;
    else
      flags &= ~value;
    return flags;
  }
  NK_API bool
  checkbox_text(context* ctx, const char* text, const int len, bool* active) {
    NK_ASSERT(ctx);
    NK_ASSERT(text);
    NK_ASSERT(active);
    if (!ctx || !text || !active)
      return 0;
    const int old_val = *active;
    *active = check_text(ctx, text, len, *active);
    return old_val != *active;
  }
  NK_API bool
  checkbox_text_align(context* ctx, const char* text, const int len, bool* active, const flag widget_alignment, const flag text_alignment) {
    NK_ASSERT(ctx);
    NK_ASSERT(text);
    NK_ASSERT(active);
    if (!ctx || !text || !active)
      return 0;
    const int old_val = *active;
    *active = check_text_align(ctx, text, len, *active, widget_alignment, text_alignment);
    return old_val != *active;
  }
  NK_API bool
  checkbox_flags_text(context* ctx, const char* text, const int len,
                      unsigned int* flags, const unsigned int value) {
    bool active;
    NK_ASSERT(ctx);
    NK_ASSERT(text);
    NK_ASSERT(flags);
    if (!ctx || !text || !flags)
      return 0;

    active = (int) ((*flags & value) & value);
    if (checkbox_text(ctx, text, len, &active)) {
      if (active)
        *flags |= value;
      else
        *flags &= ~value;
      return 1;
    }
    return 0;
  }
  NK_API bool check_label(context* ctx, const char* label, const bool active) {
    return check_text(ctx, label, strlen(label), active);
  }
  NK_API unsigned int check_flags_label(context* ctx, const char* label,
                                        const unsigned int flags, const unsigned int value) {
    return check_flags_text(ctx, label, strlen(label), flags, value);
  }
  NK_API bool checkbox_label(context* ctx, const char* label, bool* active) {
    return checkbox_text(ctx, label, strlen(label), active);
  }
  NK_API bool checkbox_label_align(context* ctx, const char* label, bool* active, const flag widget_alignment, const flag text_alignment) {
    return checkbox_text_align(ctx, label, strlen(label), active, widget_alignment, text_alignment);
  }
  NK_API bool checkbox_flags_label(context* ctx, const char* label,
                                   unsigned int* flags, const unsigned int value) {
    return checkbox_flags_text(ctx, label, strlen(label), flags, value);
  }
  /*----------------------------------------------------------------
   *
   *                          OPTION
   *
   * --------------------------------------------------------------*/
  NK_API bool
  option_text(context* ctx, const char* text, const int len, bool is_active) {

    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return is_active;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const panel* layout = win->layout;

    const widget_layout_states state = widget(&bounds, ctx);
    if (!state)
      return (int) state;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    do_toggle(&ctx->last_widget_state, &win->buffer, bounds, &is_active,
              text, len, NK_TOGGLE_OPTION, &style->option, in, style->font, NK_WIDGET_LEFT, NK_TEXT_LEFT);
    return is_active;
  }
  NK_API bool
  option_text_align(context* ctx, const char* text, const int len, bool is_active, const flag widget_alignment, const flag text_alignment) {

    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return is_active;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const panel* layout = win->layout;

    const widget_layout_states state = widget(&bounds, ctx);
    if (!state)
      return (int) state;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    do_toggle(&ctx->last_widget_state, &win->buffer, bounds, &is_active,
              text, len, NK_TOGGLE_OPTION, &style->option, in, style->font, widget_alignment, text_alignment);
    return is_active;
  }
  NK_API bool
  radio_text(context* ctx, const char* text, const int len, bool* active) {
    NK_ASSERT(ctx);
    NK_ASSERT(text);
    NK_ASSERT(active);
    if (!ctx || !text || !active)
      return 0;
    const int old_value = *active;
    *active = option_text(ctx, text, len, old_value);
    return old_value != *active;
  }
  NK_API bool
  radio_text_align(context* ctx, const char* text, const int len, bool* active, const flag widget_alignment, const flag text_alignment) {
    NK_ASSERT(ctx);
    NK_ASSERT(text);
    NK_ASSERT(active);
    if (!ctx || !text || !active)
      return 0;
    const int old_value = *active;
    *active = option_text_align(ctx, text, len, old_value, widget_alignment, text_alignment);
    return old_value != *active;
  }
  NK_API bool
  option_label(context* ctx, const char* label, const bool active) {
    return option_text(ctx, label, strlen(label), active);
  }
  NK_API bool
  option_label_align(context* ctx, const char* label, const bool active, const flag widget_alignment, const flag text_alignment) {
    return option_text_align(ctx, label, strlen(label), active, widget_alignment, text_alignment);
  }
  NK_API bool
  radio_label(context* ctx, const char* label, bool* active) {
    return radio_text(ctx, label, strlen(label), active);
  }
  NK_API bool
  radio_label_align(context* ctx, const char* label, bool* active, const flag widget_alignment, const flag text_alignment) {
    return radio_text_align(ctx, label, strlen(label), active, widget_alignment, text_alignment);
  }
} // namespace nk
