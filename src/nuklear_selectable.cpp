#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              SELECTABLE
   *
   * ===============================================================*/
  NK_LIB void
  draw_selectable(struct command_buffer *out,
      flag state, const struct style_selectable *style, bool active,
      const rectf *bounds,
      const rectf *icon, const struct image *img, enum symbol_type sym,
      const char *string, int len, flag align, const struct user_font *font)
  {
    const struct style_item *background;
    struct text text;
    text.padding = style->padding;

    /* select correct colors/images */
    if (!active) {
      if (state & NK_WIDGET_STATE_ACTIVED) {
        background = &style->pressed;
        text.txt = style->text_pressed;
      } else if (state & NK_WIDGET_STATE_HOVER) {
        background = &style->hover;
        text.txt = style->text_hover;
      } else {
        background = &style->normal;
        text.txt = style->text_normal;
      }
    } else {
      if (state & NK_WIDGET_STATE_ACTIVED) {
        background = &style->pressed_active;
        text.txt = style->text_pressed_active;
      } else if (state & NK_WIDGET_STATE_HOVER) {
        background = &style->hover_active;
        text.txt = style->text_hover_active;
      } else {
        background = &style->normal_active;
        text.txt = style->text_normal_active;
      }
    }

    text.txt = rgb_factor(text.txt, style->color_factor);

    /* draw selectable background and text */
    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        text.background = rgba(0, 0, 0, 0);
        draw_image(out, *bounds, &background->data.image, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        text.background = rgba(0, 0, 0, 0);
        draw_nine_slice(out, *bounds, &background->data.slice, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        text.background = background->data.color;
        fill_rect(out, *bounds, style->rounding, background->data.color);
        break;
    }
    if (icon) {
      if (img) draw_image(out, *icon, img, rgb_factor(white, style->color_factor));
      else draw_symbol(out, sym, *icon, text.background, text.txt, 1, font);
    }
    widget_text(out, *bounds, string, len, &text, align, font);
  }
  NK_LIB bool
  do_selectable(flag *state, struct command_buffer *out,
      rectf bounds, const char *str, int len, flag align, bool *value,
      const struct style_selectable *style, const struct input *in,
      const struct user_font *font)
  {
    int old_value;
    rectf touch;

    NK_ASSERT(state);
    NK_ASSERT(out);
    NK_ASSERT(str);
    NK_ASSERT(len);
    NK_ASSERT(value);
    NK_ASSERT(style);
    NK_ASSERT(font);

    if (!state || !out || !str || !len || !value || !style || !font) return 0;
    old_value = *value;

    /* remove padding */
    touch.x = bounds.x - style->touch_padding.x;
    touch.y = bounds.y - style->touch_padding.y;
    touch.w = bounds.w + style->touch_padding.x * 2;
    touch.h = bounds.h + style->touch_padding.y * 2;

    /* update button */
    if (button_behavior(state, touch, in, btn_behavior::BUTTON_DEFAULT))
      *value = !(*value);

    /* draw selectable */
    if (style->draw_begin) style->draw_begin(out, style->userdata);
    draw_selectable(out, *state, style, *value, &bounds, 0,0, symbol_type::SYMBOL_NONE, str, len, align, font);
    if (style->draw_end) style->draw_end(out, style->userdata);
    return old_value != *value;
  }
  NK_LIB bool
  do_selectable_image(flag *state, struct command_buffer *out,
      rectf bounds, const char *str, int len, flag align, bool *value,
      const struct image *img, const struct style_selectable *style,
      const struct input *in, const struct user_font *font)
  {
    bool old_value;
    rectf touch;
    rectf icon;

    NK_ASSERT(state);
    NK_ASSERT(out);
    NK_ASSERT(str);
    NK_ASSERT(len);
    NK_ASSERT(value);
    NK_ASSERT(style);
    NK_ASSERT(font);

    if (!state || !out || !str || !len || !value || !style || !font) return 0;
    old_value = *value;

    /* toggle behavior */
    touch.x = bounds.x - style->touch_padding.x;
    touch.y = bounds.y - style->touch_padding.y;
    touch.w = bounds.w + style->touch_padding.x * 2;
    touch.h = bounds.h + style->touch_padding.y * 2;
    if (button_behavior(state, touch, in, btn_behavior::BUTTON_DEFAULT))
      *value = !(*value);

    icon.y = bounds.y + style->padding.y;
    icon.w = icon.h = bounds.h - 2 * style->padding.y;
    if (align & NK_TEXT_ALIGN_LEFT) {
      icon.x = (bounds.x + bounds.w) - (2 * style->padding.x + icon.w);
      icon.x = NK_MAX(icon.x, 0);
    } else icon.x = bounds.x + 2 * style->padding.x;

    icon.x += style->image_padding.x;
    icon.y += style->image_padding.y;
    icon.w -= 2 * style->image_padding.x;
    icon.h -= 2 * style->image_padding.y;

    /* draw selectable */
    if (style->draw_begin) style->draw_begin(out, style->userdata);
    draw_selectable(out, *state, style, *value, &bounds, &icon, img, symbol_type::SYMBOL_NONE, str, len, align, font);
    if (style->draw_end) style->draw_end(out, style->userdata);
    return old_value != *value;
  }
  NK_LIB bool
  do_selectable_symbol(flag *state, struct command_buffer *out,
      rectf bounds, const char *str, int len, flag align, bool *value,
      enum symbol_type sym, const struct style_selectable *style,
      const struct input *in, const struct user_font *font)
  {
    int old_value;
    rectf touch;
    rectf icon;

    NK_ASSERT(state);
    NK_ASSERT(out);
    NK_ASSERT(str);
    NK_ASSERT(len);
    NK_ASSERT(value);
    NK_ASSERT(style);
    NK_ASSERT(font);

    if (!state || !out || !str || !len || !value || !style || !font) return 0;
    old_value = *value;

    /* toggle behavior */
    touch.x = bounds.x - style->touch_padding.x;
    touch.y = bounds.y - style->touch_padding.y;
    touch.w = bounds.w + style->touch_padding.x * 2;
    touch.h = bounds.h + style->touch_padding.y * 2;
    if (button_behavior(state, touch, in, btn_behavior::BUTTON_DEFAULT))
      *value = !(*value);

    icon.y = bounds.y + style->padding.y;
    icon.w = icon.h = bounds.h - 2 * style->padding.y;
    if (align & NK_TEXT_ALIGN_LEFT) {
      icon.x = (bounds.x + bounds.w) - (2 * style->padding.x + icon.w);
      icon.x = NK_MAX(icon.x, 0);
    } else icon.x = bounds.x + 2 * style->padding.x;

    icon.x += style->image_padding.x;
    icon.y += style->image_padding.y;
    icon.w -= 2 * style->image_padding.x;
    icon.h -= 2 * style->image_padding.y;

    /* draw selectable */
    if (style->draw_begin) style->draw_begin(out, style->userdata);
    draw_selectable(out, *state, style, *value, &bounds, &icon, 0, sym, str, len, align, font);
    if (style->draw_end) style->draw_end(out, style->userdata);
    return old_value != *value;
  }

  NK_API bool
  selectable_text(struct context *ctx, const char *str, int len,
      flag align, bool *value)
  {
    struct window *win;
    struct panel *layout;
    const struct input *in;
    const struct style *style;

    enum widget_layout_states state;
    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(value);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !value)
      return 0;

    win = ctx->current;
    layout = win->layout;
    style = &ctx->style;

    state = widget(&bounds, ctx);
    if (!state) return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    return do_selectable(&ctx->last_widget_state, &win->buffer, bounds,
                str, len, align, value, &style->selectable, in, style->font);
  }
  NK_API bool
  selectable_image_text(struct context *ctx, struct image img,
      const char *str, int len, flag align, bool *value)
  {
    struct window *win;
    struct panel *layout;
    const struct input *in;
    const struct style *style;

    enum widget_layout_states state;
    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(value);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !value)
      return 0;

    win = ctx->current;
    layout = win->layout;
    style = &ctx->style;

    state = widget(&bounds, ctx);
    if (!state) return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    return do_selectable_image(&ctx->last_widget_state, &win->buffer, bounds,
                str, len, align, value, &img, &style->selectable, in, style->font);
  }
  NK_API bool
  selectable_symbol_text(struct context *ctx, enum symbol_type sym,
      const char *str, int len, flag align, bool *value)
  {
    struct window *win;
    struct panel *layout;
    const struct input *in;
    const struct style *style;

    enum widget_layout_states state;
    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(value);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !value)
      return 0;

    win = ctx->current;
    layout = win->layout;
    style = &ctx->style;

    state = widget(&bounds, ctx);
    if (!state) return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    return do_selectable_symbol(&ctx->last_widget_state, &win->buffer, bounds,
                str, len, align, value, sym, &style->selectable, in, style->font);
  }
  NK_API bool
  selectable_symbol_label(struct context *ctx, enum symbol_type sym,
      const char *title, flag align, bool *value)
  {
    return selectable_symbol_text(ctx, sym, title, strlen(title), align, value);
  }
  NK_API bool select_text(struct context *ctx, const char *str, int len,
      flag align, bool value)
  {
    selectable_text(ctx, str, len, align, &value);return value;
  }
  NK_API bool selectable_label(struct context *ctx, const char *str, flag align, bool *value)
  {
    return selectable_text(ctx, str, strlen(str), align, value);
  }
  NK_API bool selectable_image_label(struct context *ctx,struct image img,
      const char *str, flag align, bool *value)
  {
    return selectable_image_text(ctx, img, str, strlen(str), align, value);
  }
  NK_API bool select_label(struct context *ctx, const char *str, flag align, bool value)
  {
    selectable_text(ctx, str, strlen(str), align, &value);return value;
  }
  NK_API bool select_image_label(struct context *ctx, struct image img,
      const char *str, flag align, bool value)
  {
    selectable_image_text(ctx, img, str, strlen(str), align, &value);return value;
  }
  NK_API bool select_image_text(struct context *ctx, struct image img,
      const char *str, int len, flag align, bool value)
  {
    selectable_image_text(ctx, img, str, len, align, &value);return value;
  }
  NK_API bool
  select_symbol_text(struct context *ctx, enum symbol_type sym,
      const char *title, int title_len, flag align, bool value)
  {
    selectable_symbol_text(ctx, sym, title, title_len, align, &value);return value;
  }
  NK_API bool
  select_symbol_label(struct context *ctx, enum symbol_type sym,
      const char *title, flag align, bool value)
  {
    return select_symbol_text(ctx, sym, title, strlen(title), align, value);
  }
}
