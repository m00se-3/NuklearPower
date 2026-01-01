#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ==============================================================
   *
   *                          COMBO
   *
   * ===============================================================*/
  INTERN bool
  combo_begin(context* ctx, window* win,
              const vec2f size, const bool is_clicked, const rectf header) {
    int is_open = 0;
    int is_active = 0;
    rectf body;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    const window* popup = win->popup.win;
    body.x = header.x;
    body.w = size.x;
    body.y = header.y + header.h - ctx->style.window.combo_border;
    body.h = size.y;

    const hash hsh = win->popup.combo_count++;
    is_open = (popup) ? true : false;
    is_active = (popup && (win->popup.name == hsh) && win->popup.type == panel_type::PANEL_COMBO);
    if ((is_clicked && is_open && !is_active) || (is_open && !is_active) ||
        (!is_open && !is_active && !is_clicked))
      return false;
    if (!nonblock_begin(ctx, 0, body,
                        (is_clicked && is_open) ? rect(0, 0, 0, 0) : header, panel_type::PANEL_COMBO))
      return 0;

    win->popup.type = panel_type::PANEL_COMBO;
    win->popup.name = hsh;
    return 1;
  }
  NK_API bool
  combo_begin_text(context* ctx, const char* selected, const int len,
                   const vec2f size) {

    int is_clicked = false;
    rectf header;
    const style_item* background;
    text text;

    NK_ASSERT(ctx);
    NK_ASSERT(selected);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !selected)
      return 0;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const widget_layout_states s = widget(&header, ctx);
    if (s == NK_WIDGET_INVALID)
      return 0;

    const input* in = (win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM) || s == NK_WIDGET_DISABLED || s == NK_WIDGET_ROM) ? 0 : &ctx->input;
    if (button_behavior(&ctx->last_widget_state, header, in, btn_behavior::BUTTON_DEFAULT))
      is_clicked = true;

    /* draw combo box header background and border */
    if (ctx->last_widget_state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->combo.active;
      text.txt = style->combo.label_active;
    } else if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER) {
      background = &style->combo.hover;
      text.txt = style->combo.label_hover;
    } else {
      background = &style->combo.normal;
      text.txt = style->combo.label_normal;
    }

    text.txt = rgb_factor(text.txt, style->combo.color_factor);

    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        text.background = rgba(0, 0, 0, 0);
        draw_image(&win->buffer, header, &background->data.image, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        text.background = rgba(0, 0, 0, 0);
        draw_nine_slice(&win->buffer, header, &background->data.slice, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        text.background = background->data.color;
        fill_rect(&win->buffer, header, style->combo.rounding, rgb_factor(background->data.color, style->combo.color_factor));
        stroke_rect(&win->buffer, header, style->combo.rounding, style->combo.border, rgb_factor(style->combo.border_color, style->combo.color_factor));
        break;
    }
    {
      /* print currently selected text item */
      rectf label;
      rectf button;
      rectf content;

      symbol_type sym;
      if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
        sym = style->combo.sym_hover;
      else if (is_clicked)
        sym = style->combo.sym_active;
      else
        sym = style->combo.sym_normal;

      /* represents whether or not the combo's button symbol should be drawn */
      const int draw_btn_symbol = sym != symbol_type::SYMBOL_NONE;

      /* calculate button */
      button.w = header.h - 2 * style->combo.button_padding.y;
      button.x = (header.x + header.w - header.h) - style->combo.button_padding.x;
      button.y = header.y + style->combo.button_padding.y;
      button.h = button.w;

      content.x = button.x + style->combo.button.padding.x;
      content.y = button.y + style->combo.button.padding.y;
      content.w = button.w - 2 * style->combo.button.padding.x;
      content.h = button.h - 2 * style->combo.button.padding.y;

      /* draw selected label */
      text.padding = vec2_from_floats(0, 0);
      label.x = header.x + style->combo.content_padding.x;
      label.y = header.y + style->combo.content_padding.y;
      label.h = header.h - 2 * style->combo.content_padding.y;
      if (draw_btn_symbol)
        label.w = button.x - (style->combo.content_padding.x + style->combo.spacing.x) - label.x;
      else
        label.w = header.w - 2 * style->combo.content_padding.x;
      widget_text(&win->buffer, label, selected, len, &text,
                  NK_TEXT_LEFT, ctx->style.font);

      /* draw open/close button */
      if (draw_btn_symbol)
        draw_button_symbol(&win->buffer, &button, &content, ctx->last_widget_state,
                           &ctx->style.combo.button, sym, style->font);
    }
    return combo_begin(ctx, win, size, is_clicked, header);
  }
  NK_API bool
  combo_begin_label(context* ctx, const char* selected, const vec2f size) {
    return combo_begin_text(ctx, selected, strlen(selected), size);
  }
  NK_API bool
  combo_begin_color(context* ctx, const color color, const vec2f size) {

    rectf header;
    int is_clicked = false;
    const style_item* background;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const widget_layout_states s = widget(&header, ctx);
    if (s == NK_WIDGET_INVALID)
      return 0;

    const input* in = (win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM) || s == NK_WIDGET_DISABLED || s == NK_WIDGET_ROM) ? 0 : &ctx->input;
    if (button_behavior(&ctx->last_widget_state, header, in, btn_behavior::BUTTON_DEFAULT))
      is_clicked = true;

    /* draw combo box header background and border */
    if (ctx->last_widget_state & NK_WIDGET_STATE_ACTIVED)
      background = &style->combo.active;
    else if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
      background = &style->combo.hover;
    else
      background = &style->combo.normal;

    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        draw_image(&win->buffer, header, &background->data.image, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        draw_nine_slice(&win->buffer, header, &background->data.slice, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        fill_rect(&win->buffer, header, style->combo.rounding, rgb_factor(background->data.color, style->combo.color_factor));
        stroke_rect(&win->buffer, header, style->combo.rounding, style->combo.border, rgb_factor(style->combo.border_color, style->combo.color_factor));
        break;
    }
    {
      rectf content;
      rectf button;
      rectf bounds;

      symbol_type sym;
      if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
        sym = style->combo.sym_hover;
      else if (is_clicked)
        sym = style->combo.sym_active;
      else
        sym = style->combo.sym_normal;

      /* represents whether or not the combo's button symbol should be drawn */
      const int draw_btn_symbol = sym != symbol_type::SYMBOL_NONE;

      /* calculate button */
      button.w = header.h - 2 * style->combo.button_padding.y;
      button.x = (header.x + header.w - header.h) - style->combo.button_padding.x;
      button.y = header.y + style->combo.button_padding.y;
      button.h = button.w;

      content.x = button.x + style->combo.button.padding.x;
      content.y = button.y + style->combo.button.padding.y;
      content.w = button.w - 2 * style->combo.button.padding.x;
      content.h = button.h - 2 * style->combo.button.padding.y;

      /* draw color */
      bounds.h = header.h - 4 * style->combo.content_padding.y;
      bounds.y = header.y + 2 * style->combo.content_padding.y;
      bounds.x = header.x + 2 * style->combo.content_padding.x;
      if (draw_btn_symbol)
        bounds.w = (button.x - (style->combo.content_padding.x + style->combo.spacing.x)) - bounds.x;
      else
        bounds.w = header.w - 4 * style->combo.content_padding.x;
      fill_rect(&win->buffer, bounds, 0, rgb_factor(color, style->combo.color_factor));

      /* draw open/close button */
      if (draw_btn_symbol)
        draw_button_symbol(&win->buffer, &button, &content, ctx->last_widget_state,
                           &ctx->style.combo.button, sym, style->font);
    }
    return combo_begin(ctx, win, size, is_clicked, header);
  }
  NK_API bool
  combo_begin_symbol(context* ctx, const symbol_type symbol, const vec2f size) {

    rectf header;
    int is_clicked = false;
    const style_item* background;
    color sym_background;
    color symbol_color;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const widget_layout_states s = widget(&header, ctx);
    if (s == NK_WIDGET_INVALID)
      return 0;

    const input* in = (win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM) || s == NK_WIDGET_DISABLED || s == NK_WIDGET_ROM) ? 0 : &ctx->input;
    if (button_behavior(&ctx->last_widget_state, header, in, btn_behavior::BUTTON_DEFAULT))
      is_clicked = true;

    /* draw combo box header background and border */
    if (ctx->last_widget_state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->combo.active;
      symbol_color = style->combo.symbol_active;
    } else if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER) {
      background = &style->combo.hover;
      symbol_color = style->combo.symbol_hover;
    } else {
      background = &style->combo.normal;
      symbol_color = style->combo.symbol_hover;
    }

    symbol_color = rgb_factor(symbol_color, style->combo.color_factor);

    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        sym_background = rgba(0, 0, 0, 0);
        draw_image(&win->buffer, header, &background->data.image, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        sym_background = rgba(0, 0, 0, 0);
        draw_nine_slice(&win->buffer, header, &background->data.slice, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        sym_background = background->data.color;
        fill_rect(&win->buffer, header, style->combo.rounding, rgb_factor(background->data.color, style->combo.color_factor));
        stroke_rect(&win->buffer, header, style->combo.rounding, style->combo.border, rgb_factor(style->combo.border_color, style->combo.color_factor));
        break;
    }
    {
      rectf bounds = {0, 0, 0, 0};
      rectf content;
      rectf button;

      symbol_type sym;
      if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
        sym = style->combo.sym_hover;
      else if (is_clicked)
        sym = style->combo.sym_active;
      else
        sym = style->combo.sym_normal;

      /* calculate button */
      button.w = header.h - 2 * style->combo.button_padding.y;
      button.x = (header.x + header.w - header.h) - style->combo.button_padding.y;
      button.y = header.y + style->combo.button_padding.y;
      button.h = button.w;

      content.x = button.x + style->combo.button.padding.x;
      content.y = button.y + style->combo.button.padding.y;
      content.w = button.w - 2 * style->combo.button.padding.x;
      content.h = button.h - 2 * style->combo.button.padding.y;

      /* draw symbol */
      bounds.h = header.h - 2 * style->combo.content_padding.y;
      bounds.y = header.y + style->combo.content_padding.y;
      bounds.x = header.x + style->combo.content_padding.x;
      bounds.w = (button.x - style->combo.content_padding.y) - bounds.x;
      draw_symbol(&win->buffer, symbol, bounds, sym_background, symbol_color,
                  1.0f, style->font);

      /* draw open/close button */
      draw_button_symbol(&win->buffer, &bounds, &content, ctx->last_widget_state,
                         &ctx->style.combo.button, sym, style->font);
    }
    return combo_begin(ctx, win, size, is_clicked, header);
  }
  NK_API bool
  combo_begin_symbol_text(context* ctx, const char* selected, int len,
                          symbol_type symbol, vec2f size) {
    window* win;
    style* style;
    input* in;

    rectf header;
    int is_clicked = false;
    widget_layout_states s;
    const style_item* background;
    color symbol_color;
    text text;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    style = &ctx->style;
    s = widget(&header, ctx);
    if (!s)
      return 0;

    in = (win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM) || s == NK_WIDGET_DISABLED || s == NK_WIDGET_ROM) ? 0 : &ctx->input;
    if (button_behavior(&ctx->last_widget_state, header, in, btn_behavior::BUTTON_DEFAULT))
      is_clicked = true;

    /* draw combo box header background and border */
    if (ctx->last_widget_state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->combo.active;
      symbol_color = style->combo.symbol_active;
      text.txt = style->combo.label_active;
    } else if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER) {
      background = &style->combo.hover;
      symbol_color = style->combo.symbol_hover;
      text.txt = style->combo.label_hover;
    } else {
      background = &style->combo.normal;
      symbol_color = style->combo.symbol_normal;
      text.txt = style->combo.label_normal;
    }

    text.txt = rgb_factor(text.txt, style->combo.color_factor);
    symbol_color = rgb_factor(symbol_color, style->combo.color_factor);

    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        text.background = rgba(0, 0, 0, 0);
        draw_image(&win->buffer, header, &background->data.image, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        text.background = rgba(0, 0, 0, 0);
        draw_nine_slice(&win->buffer, header, &background->data.slice, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        text.background = background->data.color;
        fill_rect(&win->buffer, header, style->combo.rounding, rgb_factor(background->data.color, style->combo.color_factor));
        stroke_rect(&win->buffer, header, style->combo.rounding, style->combo.border, rgb_factor(style->combo.border_color, style->combo.color_factor));
        break;
    }
    {
      rectf content;
      rectf button;
      rectf label;
      rectf image;

      symbol_type sym;
      if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
        sym = style->combo.sym_hover;
      else if (is_clicked)
        sym = style->combo.sym_active;
      else
        sym = style->combo.sym_normal;

      /* calculate button */
      button.w = header.h - 2 * style->combo.button_padding.y;
      button.x = (header.x + header.w - header.h) - style->combo.button_padding.x;
      button.y = header.y + style->combo.button_padding.y;
      button.h = button.w;

      content.x = button.x + style->combo.button.padding.x;
      content.y = button.y + style->combo.button.padding.y;
      content.w = button.w - 2 * style->combo.button.padding.x;
      content.h = button.h - 2 * style->combo.button.padding.y;
      draw_button_symbol(&win->buffer, &button, &content, ctx->last_widget_state,
                         &ctx->style.combo.button, sym, style->font);

      /* draw symbol */
      image.x = header.x + style->combo.content_padding.x;
      image.y = header.y + style->combo.content_padding.y;
      image.h = header.h - 2 * style->combo.content_padding.y;
      image.w = image.h;
      draw_symbol(&win->buffer, symbol, image, text.background, symbol_color,
                  1.0f, style->font);

      /* draw label */
      text.padding = vec2_from_floats(0, 0);
      label.x = image.x + image.w + style->combo.spacing.x + style->combo.content_padding.x;
      label.y = header.y + style->combo.content_padding.y;
      label.w = (button.x - style->combo.content_padding.x) - label.x;
      label.h = header.h - 2 * style->combo.content_padding.y;
      widget_text(&win->buffer, label, selected, len, &text, NK_TEXT_LEFT, style->font);
    }
    return combo_begin(ctx, win, size, is_clicked, header);
  }
  NK_API bool
  combo_begin_image(context* ctx, struct image img, const vec2f size) {

    rectf header;
    int is_clicked = false;
    const style_item* background;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const style* style = &ctx->style;
    const widget_layout_states s = widget(&header, ctx);
    if (s == NK_WIDGET_INVALID)
      return 0;

    const input* in = (win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM) || s == NK_WIDGET_DISABLED || s == NK_WIDGET_ROM) ? 0 : &ctx->input;
    if (button_behavior(&ctx->last_widget_state, header, in, btn_behavior::BUTTON_DEFAULT))
      is_clicked = true;

    /* draw combo box header background and border */
    if (ctx->last_widget_state & NK_WIDGET_STATE_ACTIVED)
      background = &style->combo.active;
    else if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
      background = &style->combo.hover;
    else
      background = &style->combo.normal;

    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        draw_image(&win->buffer, header, &background->data.image, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        draw_nine_slice(&win->buffer, header, &background->data.slice, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        fill_rect(&win->buffer, header, style->combo.rounding, rgb_factor(background->data.color, style->combo.color_factor));
        stroke_rect(&win->buffer, header, style->combo.rounding, style->combo.border, rgb_factor(style->combo.border_color, style->combo.color_factor));
        break;
    }
    {
      rectf bounds = {0, 0, 0, 0};
      rectf content;
      rectf button;

      symbol_type sym;
      if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
        sym = style->combo.sym_hover;
      else if (is_clicked)
        sym = style->combo.sym_active;
      else
        sym = style->combo.sym_normal;

      /* represents whether or not the combo's button symbol should be drawn */
      const int draw_btn_symbol = sym != symbol_type::SYMBOL_NONE;

      /* calculate button */
      button.w = header.h - 2 * style->combo.button_padding.y;
      button.x = (header.x + header.w - header.h) - style->combo.button_padding.y;
      button.y = header.y + style->combo.button_padding.y;
      button.h = button.w;

      content.x = button.x + style->combo.button.padding.x;
      content.y = button.y + style->combo.button.padding.y;
      content.w = button.w - 2 * style->combo.button.padding.x;
      content.h = button.h - 2 * style->combo.button.padding.y;

      /* draw image */
      bounds.h = header.h - 2 * style->combo.content_padding.y;
      bounds.y = header.y + style->combo.content_padding.y;
      bounds.x = header.x + style->combo.content_padding.x;
      if (draw_btn_symbol)
        bounds.w = (button.x - style->combo.content_padding.y) - bounds.x;
      else
        bounds.w = header.w - 2 * style->combo.content_padding.x;
      draw_image(&win->buffer, bounds, &img, rgb_factor(white, style->combo.color_factor));

      /* draw open/close button */
      if (draw_btn_symbol)
        draw_button_symbol(&win->buffer, &bounds, &content, ctx->last_widget_state,
                           &ctx->style.combo.button, sym, style->font);
    }
    return combo_begin(ctx, win, size, is_clicked, header);
  }
  NK_API bool
  combo_begin_image_text(context* ctx, const char* selected, int len,
                         struct image img, vec2f size) {
    window* win;
    style* style;
    input* in;

    rectf header;
    int is_clicked = false;
    widget_layout_states s;
    const style_item* background;
    text text;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    style = &ctx->style;
    s = widget(&header, ctx);
    if (!s)
      return 0;

    in = (win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM) || s == NK_WIDGET_DISABLED || s == NK_WIDGET_ROM) ? 0 : &ctx->input;
    if (button_behavior(&ctx->last_widget_state, header, in, btn_behavior::BUTTON_DEFAULT))
      is_clicked = true;

    /* draw combo box header background and border */
    if (ctx->last_widget_state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->combo.active;
      text.txt = style->combo.label_active;
    } else if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER) {
      background = &style->combo.hover;
      text.txt = style->combo.label_hover;
    } else {
      background = &style->combo.normal;
      text.txt = style->combo.label_normal;
    }

    text.txt = rgb_factor(text.txt, style->combo.color_factor);

    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        text.background = rgba(0, 0, 0, 0);
        draw_image(&win->buffer, header, &background->data.image, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        text.background = rgba(0, 0, 0, 0);
        draw_nine_slice(&win->buffer, header, &background->data.slice, rgb_factor(white, style->combo.color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        text.background = background->data.color;
        fill_rect(&win->buffer, header, style->combo.rounding, rgb_factor(background->data.color, style->combo.color_factor));
        stroke_rect(&win->buffer, header, style->combo.rounding, style->combo.border, rgb_factor(style->combo.border_color, style->combo.color_factor));
        break;
    }
    {
      rectf content;
      rectf button;
      rectf label;
      rectf image;
      int draw_btn_symbol;

      symbol_type sym;
      if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
        sym = style->combo.sym_hover;
      else if (is_clicked)
        sym = style->combo.sym_active;
      else
        sym = style->combo.sym_normal;

      /* represents whether or not the combo's button symbol should be drawn */
      draw_btn_symbol = sym != symbol_type::SYMBOL_NONE;

      /* calculate button */
      button.w = header.h - 2 * style->combo.button_padding.y;
      button.x = (header.x + header.w - header.h) - style->combo.button_padding.x;
      button.y = header.y + style->combo.button_padding.y;
      button.h = button.w;

      content.x = button.x + style->combo.button.padding.x;
      content.y = button.y + style->combo.button.padding.y;
      content.w = button.w - 2 * style->combo.button.padding.x;
      content.h = button.h - 2 * style->combo.button.padding.y;
      if (draw_btn_symbol)
        draw_button_symbol(&win->buffer, &button, &content, ctx->last_widget_state,
                           &ctx->style.combo.button, sym, style->font);

      /* draw image */
      image.x = header.x + style->combo.content_padding.x;
      image.y = header.y + style->combo.content_padding.y;
      image.h = header.h - 2 * style->combo.content_padding.y;
      image.w = image.h;
      draw_image(&win->buffer, image, &img, rgb_factor(white, style->combo.color_factor));

      /* draw label */
      text.padding = vec2_from_floats(0, 0);
      label.x = image.x + image.w + style->combo.spacing.x + style->combo.content_padding.x;
      label.y = header.y + style->combo.content_padding.y;
      label.h = header.h - 2 * style->combo.content_padding.y;
      if (draw_btn_symbol)
        label.w = (button.x - style->combo.content_padding.x) - label.x;
      else
        label.w = (header.x + header.w - style->combo.content_padding.x) - label.x;
      widget_text(&win->buffer, label, selected, len, &text, NK_TEXT_LEFT, style->font);
    }
    return combo_begin(ctx, win, size, is_clicked, header);
  }
  NK_API bool
  combo_begin_symbol_label(context* ctx,
                           const char* selected, const symbol_type type, const vec2f size) {
    return combo_begin_symbol_text(ctx, selected, strlen(selected), type, size);
  }
  NK_API bool
  combo_begin_image_label(context* ctx,
                          const char* selected, struct image img, const vec2f size) {
    return combo_begin_image_text(ctx, selected, strlen(selected), img, size);
  }
  NK_API bool
  combo_item_text(context* ctx, const char* text, const int len, const flag align) {
    return contextual_item_text(ctx, text, len, align);
  }
  NK_API bool
  combo_item_label(context* ctx, const char* label, const flag align) {
    return contextual_item_label(ctx, label, align);
  }
  NK_API bool
  combo_item_image_text(context* ctx, struct image img, const char* text,
                        const int len, const flag alignment) {
    return contextual_item_image_text(ctx, img, text, len, alignment);
  }
  NK_API bool
  combo_item_image_label(context* ctx, struct image img,
                         const char* text, const flag alignment) {
    return contextual_item_image_label(ctx, img, text, alignment);
  }
  NK_API bool
  combo_item_symbol_text(context* ctx, const symbol_type sym,
                         const char* text, const int len, const flag alignment) {
    return contextual_item_symbol_text(ctx, sym, text, len, alignment);
  }
  NK_API bool
  combo_item_symbol_label(context* ctx, const symbol_type sym,
                          const char* label, const flag alignment) {
    return contextual_item_symbol_label(ctx, sym, label, alignment);
  }
  NK_API void combo_end(context* ctx) {
    contextual_end(ctx);
  }
  NK_API void combo_close(context* ctx) {
    contextual_close(ctx);
  }
  NK_API int
  combo(context* ctx, const char* const* items, const int count,
        int selected, const int item_height, vec2f size) {
    int i = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(items);
    NK_ASSERT(ctx->current);
    if (!ctx || !items || !count)
      return selected;

    const vec2f item_spacing = ctx->style.window.spacing;
    const vec2f window_padding = panel_get_padding(&ctx->style, ctx->current->layout->type);
    int max_height = count * item_height + count * (int) item_spacing.y;
    max_height += (int) item_spacing.y * 2 + (int) window_padding.y * 2;
    size.y = std::min(size.y, (float) max_height);
    if (combo_begin_label(ctx, items[selected], size)) {
      layout_row_dynamic(ctx, (float) item_height, 1);
      for (i = 0; i < count; ++i) {
        if (combo_item_label(ctx, items[i], NK_TEXT_LEFT))
          selected = i;
      }
      combo_end(ctx);
    }
    return selected;
  }
  NK_API int
  combo_separator(context* ctx, const char* items_separated_by_separator,
                  const int separator, int selected, const int count, const int item_height, vec2f size) {
    int i;
    const char* iter;
    int length = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(items_separated_by_separator);
    if (!ctx || !items_separated_by_separator)
      return selected;

    /* calculate popup window */
    const vec2f item_spacing = ctx->style.window.spacing;
    const vec2f window_padding = panel_get_padding(&ctx->style, ctx->current->layout->type);
    int max_height = count * item_height + count * (int) item_spacing.y;
    max_height += (int) item_spacing.y * 2 + (int) window_padding.y * 2;
    size.y = std::min(size.y, (float) max_height);

    /* find selected item */
    const char* current_item = items_separated_by_separator;
    for (i = 0; i < count; ++i) {
      iter = current_item;
      while (*iter && *iter != separator)
        iter++;
      length = (int) (iter - current_item);
      if (i == selected)
        break;
      current_item = iter + 1;
    }

    if (combo_begin_text(ctx, current_item, length, size)) {
      current_item = items_separated_by_separator;
      layout_row_dynamic(ctx, (float) item_height, 1);
      for (i = 0; i < count; ++i) {
        iter = current_item;
        while (*iter && *iter != separator)
          iter++;
        length = (int) (iter - current_item);
        if (combo_item_text(ctx, current_item, length, NK_TEXT_LEFT))
          selected = i;
        current_item = current_item + length + 1;
      }
      combo_end(ctx);
    }
    return selected;
  }
  NK_API int
  combo_string(context* ctx, const char* items_separated_by_zeros,
               const int selected, const int count, const int item_height, const vec2f size) {
    return combo_separator(ctx, items_separated_by_zeros, '\0', selected, count, item_height, size);
  }
  NK_API int
  combo_callback(context* ctx, void (*item_getter)(void*, int, const char**),
                 void* userdata, int selected, const int count, const int item_height, vec2f size) {
    const char* item;

    NK_ASSERT(ctx);
    NK_ASSERT(item_getter);
    if (!ctx || !item_getter)
      return selected;

    /* calculate popup window */
    const vec2f item_spacing = ctx->style.window.spacing;
    const vec2f window_padding = panel_get_padding(&ctx->style, ctx->current->layout->type);
    int max_height = count * item_height + count * (int) item_spacing.y;
    max_height += (int) item_spacing.y * 2 + (int) window_padding.y * 2;
    size.y = std::min(size.y, (float) max_height);

    item_getter(userdata, selected, &item);
    if (combo_begin_label(ctx, item, size)) {
      layout_row_dynamic(ctx, (float) item_height, 1);
      for (int i = 0; i < count; ++i) {
        item_getter(userdata, i, &item);
        if (combo_item_label(ctx, item, NK_TEXT_LEFT))
          selected = i;
      }
      combo_end(ctx);
    }
    return selected;
  }
  NK_API void
  combobox(context* ctx, const char* const* items, const int count,
           int* selected, const int item_height, const vec2f size) {
    *selected = combo(ctx, items, count, *selected, item_height, size);
  }
  NK_API void
  combobox_string(context* ctx, const char* items_separated_by_zeros,
                  int* selected, const int count, const int item_height, const vec2f size) {
    *selected = combo_string(ctx, items_separated_by_zeros, *selected, count, item_height, size);
  }
  NK_API void
  combobox_separator(context* ctx, const char* items_separated_by_separator,
                     const int separator, int* selected, const int count, const int item_height, const vec2f size) {
    *selected = combo_separator(ctx, items_separated_by_separator, separator,
                                *selected, count, item_height, size);
  }
  NK_API void
  combobox_callback(context* ctx,
                    void (*item_getter)(void* data, int id, const char** out_text),
                    void* userdata, int* selected, const int count, const int item_height, const vec2f size) {
    *selected = combo_callback(ctx, item_getter, userdata, *selected, count, item_height, size);
  }
} // namespace nk
