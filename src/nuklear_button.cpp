#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ==============================================================
   *
   *                          BUTTON
   *
   * ===============================================================*/
  NK_LIB void
  draw_symbol(struct command_buffer* out, enum symbol_type type,
              struct rectf content, struct color background, struct color foreground,
              float border_width, const struct user_font* font) {
    switch (type) {
      case symbol_type::SYMBOL_X:
      case symbol_type::SYMBOL_UNDERSCORE:
      case symbol_type::SYMBOL_PLUS:
      case symbol_type::SYMBOL_MINUS: {
        /* single character text symbol */
        const char* X = (type == symbol_type::SYMBOL_X) ? "x" : (type == symbol_type::SYMBOL_UNDERSCORE) ? "_"
                                                            : (type == symbol_type::SYMBOL_PLUS)         ? "+"
                                                                                                         : "-";
        struct text text;
        text.padding = vec2_from_floats(0, 0);
        text.background = background;
        text.txt = foreground;
        widget_text(out, content, X, 1, &text, NK_TEXT_CENTERED, font);
      } break;
      case symbol_type::SYMBOL_CIRCLE_SOLID:
      case symbol_type::SYMBOL_CIRCLE_OUTLINE:
      case symbol_type::SYMBOL_RECT_SOLID:
      case symbol_type::SYMBOL_RECT_OUTLINE: {
        /* simple empty/filled shapes */
        if (type == symbol_type::SYMBOL_RECT_SOLID || type == symbol_type::SYMBOL_RECT_OUTLINE) {
          fill_rect(out, content, 0, foreground);
          if (type == symbol_type::SYMBOL_RECT_OUTLINE)
            fill_rect(out, shrirect(content, border_width), 0, background);
        } else {
          fill_circle(out, content, foreground);
          if (type == symbol_type::SYMBOL_CIRCLE_OUTLINE)
            fill_circle(out, shrirect(content, 1), background);
        }
      } break;
      case symbol_type::SYMBOL_TRIANGLE_UP:
      case symbol_type::SYMBOL_TRIANGLE_DOWN:
      case symbol_type::SYMBOL_TRIANGLE_LEFT:
      case symbol_type::SYMBOL_TRIANGLE_RIGHT: {
        enum heading heading;
        struct vec2f points[3];
        heading = (type == symbol_type::SYMBOL_TRIANGLE_RIGHT) ? heading::RIGHT : (type == symbol_type::SYMBOL_TRIANGLE_LEFT) ? heading::LEFT
                                                                              : (type == symbol_type::SYMBOL_TRIANGLE_UP)     ? heading::UP
                                                                                                                              : heading::DOWN;
        triangle_from_direction(points, content, 0, 0, heading);
        fill_triangle(out, points[0].x, points[0].y, points[1].x, points[1].y,
                      points[2].x, points[2].y, foreground);
      } break;
      case symbol_type::SYMBOL_TRIANGLE_UP_OUTLINE:
      case symbol_type::SYMBOL_TRIANGLE_DOWN_OUTLINE:
      case symbol_type::SYMBOL_TRIANGLE_LEFT_OUTLINE:
      case symbol_type::SYMBOL_TRIANGLE_RIGHT_OUTLINE: {
        enum heading heading;
        struct vec2f points[3];
        heading = (type == symbol_type::SYMBOL_TRIANGLE_RIGHT_OUTLINE) ? heading::RIGHT : (type == symbol_type::SYMBOL_TRIANGLE_LEFT_OUTLINE) ? heading::LEFT
                                                                                      : (type == symbol_type::SYMBOL_TRIANGLE_UP_OUTLINE)     ? heading::UP
                                                                                                                                              : heading::DOWN;
        triangle_from_direction(points, content, 0, 0, heading);
        stroke_triangle(out, points[0].x, points[0].y, points[1].x, points[1].y,
                        points[2].x, points[2].y, border_width, foreground);
      } break;
      default:
      case symbol_type::SYMBOL_NONE:
      case symbol_type::SYMBOL_MAX:
        break;
    }
  }
  NK_LIB bool
  button_behavior(flag* state, struct rectf r,
                  const struct input* i, btn_behavior behavior) {
    int ret = 0;
    widget_state_reset(state);
    if (!i)
      return 0;
    if (input_is_mouse_hovering_rect(i, r)) {
      *state = NK_WIDGET_STATE_HOVERED;
      if (input_is_mouse_down(i, NK_BUTTON_LEFT))
        *state = NK_WIDGET_STATE_ACTIVE;
      if (input_has_mouse_click_in_button_rect(i, NK_BUTTON_LEFT, r)) {
        ret = (behavior != btn_behavior::BUTTON_DEFAULT) ? input_is_mouse_down(i, NK_BUTTON_LEFT) :
#ifdef NK_BUTTON_TRIGGER_ON_RELEASE
                                                         input_is_mouse_released(i, NK_BUTTON_LEFT);
#else
                                                         input_is_mouse_pressed(i, NK_BUTTON_LEFT);
#endif
      }
    }
    if (*state & NK_WIDGET_STATE_HOVER && !input_is_mouse_prev_hovering_rect(i, r))
      *state |= NK_WIDGET_STATE_ENTERED;
    else if (input_is_mouse_prev_hovering_rect(i, r))
      *state |= NK_WIDGET_STATE_LEFT;
    return ret;
  }
  NK_LIB const struct style_item*
  draw_button(struct command_buffer* out,
              const struct rectf* bounds, flag state,
              const struct style_button* style) {
    const struct style_item* background;
    if (state & NK_WIDGET_STATE_HOVER)
      background = &style->hover;
    else if (state & NK_WIDGET_STATE_ACTIVED)
      background = &style->active;
    else
      background = &style->normal;

    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        draw_image(out, *bounds, &background->data.image, rgb_factor(white, style->color_factor_background));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        draw_nine_slice(out, *bounds, &background->data.slice, rgb_factor(white, style->color_factor_background));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        fill_rect(out, *bounds, style->rounding, rgb_factor(background->data.color, style->color_factor_background));
        stroke_rect(out, *bounds, style->rounding, style->border, rgb_factor(style->border_color, style->color_factor_background));
        break;
    }
    return background;
  }
  NK_LIB bool
  do_button(flag* state, struct command_buffer* out, struct rectf r,
            const struct style_button* style, const struct input* in,
            btn_behavior behavior, struct rectf* content) {
    struct rectf bounds;
    NK_ASSERT(style);
    NK_ASSERT(state);
    NK_ASSERT(out);
    if (!out || !style)
      return false;

    /* calculate button content space */
    content->x = r.x + style->padding.x + style->border + style->rounding;
    content->y = r.y + style->padding.y + style->border + style->rounding;
    content->w = r.w - (2 * (style->padding.x + style->border + style->rounding));
    content->h = r.h - (2 * (style->padding.y + style->border + style->rounding));

    /* execute button behavior */
    bounds.x = r.x - style->touch_padding.x;
    bounds.y = r.y - style->touch_padding.y;
    bounds.w = r.w + 2 * style->touch_padding.x;
    bounds.h = r.h + 2 * style->touch_padding.y;
    return button_behavior(state, bounds, in, behavior);
  }
  NK_LIB void
  draw_button_text(struct command_buffer* out,
                   const struct rectf* bounds, const struct rectf* content, flag state,
                   const struct style_button* style, const char* txt, int len,
                   flag text_alignment, const struct user_font* font) {
    struct text text;
    const struct style_item* background;
    background = draw_button(out, bounds, state, style);

    /* select correct colors/images */
    if (background->type == style_item_type::STYLE_ITEM_COLOR)
      text.background = background->data.color;
    else
      text.background = style->text_background;
    if (state & NK_WIDGET_STATE_HOVER)
      text.txt = style->text_hover;
    else if (state & NK_WIDGET_STATE_ACTIVED)
      text.txt = style->text_active;
    else
      text.txt = style->text_normal;

    text.txt = rgb_factor(text.txt, style->color_factor_text);

    text.padding = vec2_from_floats(0, 0);
    widget_text(out, *content, txt, len, &text, text_alignment, font);
  }
  NK_LIB bool
  do_button_text(flag* state,
                 struct command_buffer* out, struct rectf bounds,
                 const char* string, int len, flag align, btn_behavior behavior,
                 const struct style_button* style, const struct input* in,
                 const struct user_font* font) {
    struct rectf content;
    int ret = false;

    NK_ASSERT(state);
    NK_ASSERT(style);
    NK_ASSERT(out);
    NK_ASSERT(string);
    NK_ASSERT(font);
    if (!out || !style || !font || !string)
      return false;

    ret = do_button(state, out, bounds, style, in, behavior, &content);
    if (style->draw_begin)
      style->draw_begin(out, style->userdata);
    draw_button_text(out, &bounds, &content, *state, style, string, len, align, font);
    if (style->draw_end)
      style->draw_end(out, style->userdata);
    return ret;
  }
  NK_LIB void
  draw_button_symbol(struct command_buffer* out,
                     const struct rectf* bounds, const struct rectf* content,
                     flag state, const struct style_button* style,
                     enum symbol_type type, const struct user_font* font) {
    struct color sym, bg;
    const struct style_item* background;

    /* select correct colors/images */
    background = draw_button(out, bounds, state, style);
    if (background->type == style_item_type::STYLE_ITEM_COLOR)
      bg = background->data.color;
    else
      bg = style->text_background;

    if (state & NK_WIDGET_STATE_HOVER)
      sym = style->text_hover;
    else if (state & NK_WIDGET_STATE_ACTIVED)
      sym = style->text_active;
    else
      sym = style->text_normal;

    sym = rgb_factor(sym, style->color_factor_text);
    draw_symbol(out, type, *content, bg, sym, 1, font);
  }
  NK_LIB bool
  do_button_symbol(flag* state,
                   struct command_buffer* out, struct rectf bounds,
                   enum symbol_type symbol, btn_behavior behavior,
                   const struct style_button* style, const struct input* in,
                   const struct user_font* font) {
    int ret;
    struct rectf content;

    NK_ASSERT(state);
    NK_ASSERT(style);
    NK_ASSERT(font);
    NK_ASSERT(out);
    if (!out || !style || !font || !state)
      return false;

    ret = do_button(state, out, bounds, style, in, behavior, &content);
    if (style->draw_begin)
      style->draw_begin(out, style->userdata);
    draw_button_symbol(out, &bounds, &content, *state, style, symbol, font);
    if (style->draw_end)
      style->draw_end(out, style->userdata);
    return ret;
  }
  NK_LIB void
  draw_button_image(struct command_buffer* out,
                    const struct rectf* bounds, const struct rectf* content,
                    flag state, const struct style_button* style, const struct image* img) {
    draw_button(out, bounds, state, style);
    draw_image(out, *content, img, rgb_factor(white, style->color_factor_background));
  }
  NK_LIB bool
  do_button_image(flag* state,
                  struct command_buffer* out, struct rectf bounds,
                  struct image img, btn_behavior b,
                  const struct style_button* style, const struct input* in) {
    int ret;
    struct rectf content;

    NK_ASSERT(state);
    NK_ASSERT(style);
    NK_ASSERT(out);
    if (!out || !style || !state)
      return false;

    ret = do_button(state, out, bounds, style, in, b, &content);
    content.x += style->image_padding.x;
    content.y += style->image_padding.y;
    content.w -= 2 * style->image_padding.x;
    content.h -= 2 * style->image_padding.y;

    if (style->draw_begin)
      style->draw_begin(out, style->userdata);
    draw_button_image(out, &bounds, &content, *state, style, &img);
    if (style->draw_end)
      style->draw_end(out, style->userdata);
    return ret;
  }
  NK_LIB void
  draw_button_text_symbol(struct command_buffer* out,
                          const struct rectf* bounds, const struct rectf* label,
                          const struct rectf* symbol, flag state, const struct style_button* style,
                          const char* str, int len, enum symbol_type type,
                          const struct user_font* font) {
    struct color sym;
    struct text text;
    const struct style_item* background;

    /* select correct background colors/images */
    background = draw_button(out, bounds, state, style);
    if (background->type == style_item_type::STYLE_ITEM_COLOR)
      text.background = background->data.color;
    else
      text.background = style->text_background;

    /* select correct text colors */
    if (state & NK_WIDGET_STATE_HOVER) {
      sym = style->text_hover;
      text.txt = style->text_hover;
    } else if (state & NK_WIDGET_STATE_ACTIVED) {
      sym = style->text_active;
      text.txt = style->text_active;
    } else {
      sym = style->text_normal;
      text.txt = style->text_normal;
    }

    sym = rgb_factor(sym, style->color_factor_text);
    text.txt = rgb_factor(text.txt, style->color_factor_text);
    text.padding = vec2_from_floats(0, 0);
    draw_symbol(out, type, *symbol, style->text_background, sym, 0, font);
    widget_text(out, *label, str, len, &text, NK_TEXT_CENTERED, font);
  }
  NK_LIB bool
  do_button_text_symbol(flag* state,
                        struct command_buffer* out, struct rectf bounds,
                        enum symbol_type symbol, const char* str, int len, flag align,
                        btn_behavior behavior, const struct style_button* style,
                        const struct user_font* font, const struct input* in) {
    int ret;
    struct rectf tri = {0, 0, 0, 0};
    struct rectf content;

    NK_ASSERT(style);
    NK_ASSERT(out);
    NK_ASSERT(font);
    if (!out || !style || !font)
      return false;

    ret = do_button(state, out, bounds, style, in, behavior, &content);
    tri.y = content.y + (content.h / 2) - font->height / 2;
    tri.w = font->height;
    tri.h = font->height;
    if (align & NK_TEXT_ALIGN_LEFT) {
      tri.x = (content.x + content.w) - (2 * style->padding.x + tri.w);
      tri.x = NK_MAX(tri.x, 0);
    } else
      tri.x = content.x + 2 * style->padding.x;

    /* draw button */
    if (style->draw_begin)
      style->draw_begin(out, style->userdata);
    draw_button_text_symbol(out, &bounds, &content, &tri,
                            *state, style, str, len, symbol, font);
    if (style->draw_end)
      style->draw_end(out, style->userdata);
    return ret;
  }
  NK_LIB void
  draw_button_text_image(struct command_buffer* out,
                         const struct rectf* bounds, const struct rectf* label,
                         const struct rectf* image, flag state, const struct style_button* style,
                         const char* str, int len, const struct user_font* font,
                         const struct image* img) {
    struct text text;
    const struct style_item* background;
    background = draw_button(out, bounds, state, style);

    /* select correct colors */
    if (background->type == style_item_type::STYLE_ITEM_COLOR)
      text.background = background->data.color;
    else
      text.background = style->text_background;
    if (state & NK_WIDGET_STATE_HOVER)
      text.txt = style->text_hover;
    else if (state & NK_WIDGET_STATE_ACTIVED)
      text.txt = style->text_active;
    else
      text.txt = style->text_normal;

    text.txt = rgb_factor(text.txt, style->color_factor_text);
    text.padding = vec2_from_floats(0, 0);
    widget_text(out, *label, str, len, &text, NK_TEXT_CENTERED, font);
    draw_image(out, *image, img, rgb_factor(white, style->color_factor_background));
  }
  NK_LIB bool
  do_button_text_image(flag* state,
                       struct command_buffer* out, struct rectf bounds,
                       struct image img, const char* str, int len, flag align,
                       btn_behavior behavior, const struct style_button* style,
                       const struct user_font* font, const struct input* in) {
    int ret;
    struct rectf icon;
    struct rectf content;

    NK_ASSERT(style);
    NK_ASSERT(state);
    NK_ASSERT(font);
    NK_ASSERT(out);
    if (!out || !font || !style || !str)
      return false;

    ret = do_button(state, out, bounds, style, in, behavior, &content);
    icon.y = bounds.y + style->padding.y;
    icon.w = icon.h = bounds.h - 2 * style->padding.y;
    if (align & NK_TEXT_ALIGN_LEFT) {
      icon.x = (bounds.x + bounds.w) - (2 * style->padding.x + icon.w);
      icon.x = NK_MAX(icon.x, 0);
    } else
      icon.x = bounds.x + 2 * style->padding.x;

    icon.x += style->image_padding.x;
    icon.y += style->image_padding.y;
    icon.w -= 2 * style->image_padding.x;
    icon.h -= 2 * style->image_padding.y;

    if (style->draw_begin)
      style->draw_begin(out, style->userdata);
    draw_button_text_image(out, &bounds, &content, &icon, *state, style, str, len, font, &img);
    if (style->draw_end)
      style->draw_end(out, style->userdata);
    return ret;
  }
  NK_API void
  button_set_behavior(struct context* ctx, btn_behavior behavior) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    ctx->button_behavior = behavior;
  }
  NK_API bool
  button_push_behavior(struct context* ctx, btn_behavior behavior) {
    struct config_stack_button_behavior* button_stack;
    struct config_stack_button_behavior_element* element;

    NK_ASSERT(ctx);
    if (!ctx)
      return 0;

    button_stack = &ctx->stacks.button_behaviors;
    NK_ASSERT(button_stack->head < (int) NK_LEN(button_stack->elements));
    if (button_stack->head >= (int) NK_LEN(button_stack->elements))
      return 0;

    element = &button_stack->elements[static_cast<std::size_t>(button_stack->head++)];
    element->address = &ctx->button_behavior;
    element->old_value = ctx->button_behavior;
    ctx->button_behavior = behavior;
    return 1;
  }
  NK_API bool
  button_pop_behavior(struct context* ctx) {
    struct config_stack_button_behavior* button_stack;
    struct config_stack_button_behavior_element* element;

    NK_ASSERT(ctx);
    if (!ctx)
      return 0;

    button_stack = &ctx->stacks.button_behaviors;
    NK_ASSERT(button_stack->head > 0);
    if (button_stack->head < 1)
      return 0;

    element = &button_stack->elements[static_cast<std::size_t>(--button_stack->head)];
    *element->address = element->old_value;
    return 1;
  }
  NK_API bool
  button_text_styled(struct context* ctx,
                     const struct style_button* style, const char* title, int len) {
    struct window* win;
    struct panel* layout;
    const struct input* in;

    struct rectf bounds;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(style);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!style || !ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    layout = win->layout;
    state = widget(&bounds, ctx);

    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    return do_button_text(&ctx->last_widget_state, &win->buffer, bounds,
                          title, len, style->text_alignment, ctx->button_behavior,
                          style, in, ctx->style.font);
  }
  NK_API bool
  button_text(struct context* ctx, const char* title, int len) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    return button_text_styled(ctx, &ctx->style.button, title, len);
  }
  NK_API bool button_label_styled(struct context* ctx,
                                  const struct style_button* style, const char* title) {
    return button_text_styled(ctx, style, title, strlen(title));
  }
  NK_API bool button_label(struct context* ctx, const char* title) {
    return button_text(ctx, title, strlen(title));
  }
  NK_API bool
  button_color(struct context* ctx, struct color color) {
    struct window* win;
    struct panel* layout;
    const struct input* in;
    struct style_button button;

    int ret = 0;
    struct rectf bounds;
    struct rectf content;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    layout = win->layout;

    state = widget(&bounds, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;

    button = ctx->style.button;
    button.normal = style_item_color(color);
    button.hover = style_item_color(color);
    button.active = style_item_color(color);
    ret = do_button(&ctx->last_widget_state, &win->buffer, bounds,
                    &button, in, ctx->button_behavior, &content);
    draw_button(&win->buffer, &bounds, ctx->last_widget_state, &button);
    return ret;
  }
  NK_API bool
  button_symbol_styled(struct context* ctx,
                       const struct style_button* style, enum symbol_type symbol) {
    struct window* win;
    struct panel* layout;
    const struct input* in;

    struct rectf bounds;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    layout = win->layout;
    state = widget(&bounds, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    return do_button_symbol(&ctx->last_widget_state, &win->buffer, bounds,
                            symbol, ctx->button_behavior, style, in, ctx->style.font);
  }
  NK_API bool
  button_symbol(struct context* ctx, enum symbol_type symbol) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    return button_symbol_styled(ctx, &ctx->style.button, symbol);
  }
  NK_API bool
  button_image_styled(struct context* ctx, const struct style_button* style,
                      struct image img) {
    struct window* win;
    struct panel* layout;
    const struct input* in;

    struct rectf bounds;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    layout = win->layout;

    state = widget(&bounds, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    return do_button_image(&ctx->last_widget_state, &win->buffer, bounds,
                           img, ctx->button_behavior, style, in);
  }
  NK_API bool
  button_image(struct context* ctx, struct image img) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    return button_image_styled(ctx, &ctx->style.button, img);
  }
  NK_API bool
  button_symbol_text_styled(struct context* ctx,
                            const struct style_button* style, enum symbol_type symbol,
                            const char* text, int len, flag align) {
    struct window* win;
    struct panel* layout;
    const struct input* in;

    struct rectf bounds;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    layout = win->layout;

    state = widget(&bounds, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    return do_button_text_symbol(&ctx->last_widget_state, &win->buffer, bounds,
                                 symbol, text, len, align, ctx->button_behavior,
                                 style, ctx->style.font, in);
  }
  NK_API bool
  button_symbol_text(struct context* ctx, enum symbol_type symbol,
                     const char* text, int len, flag align) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    return button_symbol_text_styled(ctx, &ctx->style.button, symbol, text, len, align);
  }
  NK_API bool button_symbol_label(struct context* ctx, enum symbol_type symbol,
                                  const char* label, flag align) {
    return button_symbol_text(ctx, symbol, label, strlen(label), align);
  }
  NK_API bool button_symbol_label_styled(struct context* ctx,
                                         const struct style_button* style, enum symbol_type symbol,
                                         const char* title, flag align) {
    return button_symbol_text_styled(ctx, style, symbol, title, strlen(title), align);
  }
  NK_API bool
  button_image_text_styled(struct context* ctx,
                           const struct style_button* style, struct image img, const char* text,
                           int len, flag align) {
    struct window* win;
    struct panel* layout;
    const struct input* in;

    struct rectf bounds;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    layout = win->layout;

    state = widget(&bounds, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    return do_button_text_image(&ctx->last_widget_state, &win->buffer,
                                bounds, img, text, len, align, ctx->button_behavior,
                                style, ctx->style.font, in);
  }
  NK_API bool
  button_image_text(struct context* ctx, struct image img,
                    const char* text, int len, flag align) {
    return button_image_text_styled(ctx, &ctx->style.button, img, text, len, align);
  }
  NK_API bool button_image_label(struct context* ctx, struct image img,
                                 const char* label, flag align) {
    return button_image_text(ctx, img, label, strlen(label), align);
  }
  NK_API bool button_image_label_styled(struct context* ctx,
                                        const struct style_button* style, struct image img,
                                        const char* label, flag text_alignment) {
    return button_image_text_styled(ctx, style, img, label, strlen(label), text_alignment);
  }
} // namespace nk
