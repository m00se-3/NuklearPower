#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ==============================================================
   *
   *                          CONTEXTUAL
   *
   * ===============================================================*/
  NK_API bool
  contextual_begin(struct context *ctx, flag flags, struct vec2f size,
      struct rectf trigger_bounds)
  {
    struct window *win;
    struct window *popup;
    struct rectf body;
    struct input* in;

    NK_STORAGE const struct rectf null_rect = {-1,-1,0,0};
    int is_clicked = 0;
    int is_open = 0;
    int ret = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    ++win->popup.con_count;
    if (ctx->current != ctx->active)
      return 0;

    /* check if currently active contextual is active */
    popup = win->popup.win;
    is_open = (popup && win->popup.type == panel_type::PANEL_CONTEXTUAL);
    in = win->widgets_disabled ? 0 : &ctx->input;
    if (in) {
      is_clicked = input_mouse_clicked(in, NK_BUTTON_RIGHT, trigger_bounds);
      if (win->popup.active_con && win->popup.con_count != win->popup.active_con)
        return 0;
      if (!is_open && win->popup.active_con)
        win->popup.active_con = 0;
      if ((!is_open && !is_clicked))
        return 0;

      /* calculate contextual position on click */
      win->popup.active_con = win->popup.con_count;
      if (is_clicked) {
        body.x = in->mouse.pos.x;
        body.y = in->mouse.pos.y;
      } else {
        body.x = popup->bounds.x;
        body.y = popup->bounds.y;
      }

      body.w = size.x;
      body.h = size.y;

      /* start nonblocking contextual popup */
      ret = nonblock_begin(ctx, flags | static_cast<decltype(flags)>(panel_flags::WINDOW_NO_SCROLLBAR), body,
          null_rect, panel_type::PANEL_CONTEXTUAL);
      if (ret) win->popup.type = panel_type::PANEL_CONTEXTUAL;
      else {
        win->popup.active_con = 0;
        win->popup.type = panel_type::PANEL_NONE;
        if (win->popup.win)
          win->popup.win->flags = 0;
      }
    }
    return ret;
  }
  NK_API bool
  contextual_item_text(struct context *ctx, const char *text, int len,
      flag alignment)
  {
    struct window *win;
    const struct input *in;
    const struct style *style;

    struct rectf bounds;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    style = &ctx->style;
    state = widget_fitting(&bounds, ctx, style->contextual_button.padding);
    if (!state) return false;

    in = (state == NK_WIDGET_ROM || win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    if (do_button_text(&ctx->last_widget_state, &win->buffer, bounds,
        text, len, alignment, btn_behavior::BUTTON_DEFAULT, &style->contextual_button, in, style->font)) {
      contextual_close(ctx);
      return true;
        }
    return false;
  }
  NK_API bool
  contextual_item_label(struct context *ctx, const char *label, flag align)
  {
    return contextual_item_text(ctx, label, strlen(label), align);
  }
  NK_API bool
  contextual_item_image_text(struct context *ctx, struct image img,
      const char *text, int len, flag align)
  {
    struct window *win;
    const struct input *in;
    const struct style *style;

    struct rectf bounds;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    style = &ctx->style;
    state = widget_fitting(&bounds, ctx, style->contextual_button.padding);
    if (!state) return false;

    in = (state == NK_WIDGET_ROM || win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    if (do_button_text_image(&ctx->last_widget_state, &win->buffer, bounds,
        img, text, len, align, btn_behavior::BUTTON_DEFAULT, &style->contextual_button, style->font, in)){
      contextual_close(ctx);
      return true;
        }
    return false;
  }
  NK_API bool
  contextual_item_image_label(struct context *ctx, struct image img,
      const char *label, flag align)
  {
    return contextual_item_image_text(ctx, img, label, strlen(label), align);
  }
  NK_API bool
  contextual_item_symbol_text(struct context *ctx, enum symbol_type symbol,
      const char *text, int len, flag align)
  {
    struct window *win;
    const struct input *in;
    const struct style *style;

    struct rectf bounds;
    enum widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    style = &ctx->style;
    state = widget_fitting(&bounds, ctx, style->contextual_button.padding);
    if (!state) return false;

    in = (state == NK_WIDGET_ROM || win->layout->flags & static_cast<decltype(win->layout->flags)>(window_flags::WINDOW_ROM)) ? 0 : &ctx->input;
    if (do_button_text_symbol(&ctx->last_widget_state, &win->buffer, bounds,
        symbol, text, len, align, btn_behavior::BUTTON_DEFAULT, &style->contextual_button, style->font, in)) {
      contextual_close(ctx);
      return true;
        }
    return false;
  }
  NK_API bool
  contextual_item_symbol_label(struct context *ctx, enum symbol_type symbol,
      const char *text, flag align)
  {
    return contextual_item_symbol_text(ctx, symbol, text, strlen(text), align);
  }
  NK_API void
  contextual_close(struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout) return;
    popup_close(ctx);
  }
  NK_API void
  contextual_end(struct context *ctx)
  {
    struct window *popup;
    struct panel *panel;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return;

    popup = ctx->current;
    panel = popup->layout;
    NK_ASSERT(popup->parent);
    NK_ASSERT((int)panel->type & (int) panel_set::PANEL_SET_POPUP);
    if (panel->flags & static_cast<decltype(panel->flags)>(window_flags::WINDOW_DYNAMIC)) {
      /* Close behavior
      This is a bit of a hack solution since we do not know before we end our popup
      how big it will be. We therefore do not directly know when a
      click outside the non-blocking popup must close it at that direct frame.
      Instead it will be closed in the next frame.*/
      struct rectf body = {0,0,0,0};
      if (panel->at_y < (panel->bounds.y + panel->bounds.h)) {
        struct vec2f padding = panel_get_padding(&ctx->style, panel->type);
        body = panel->bounds;
        body.y = (panel->at_y + panel->footer_height + panel->border + padding.y + panel->row.height);
        body.h = (panel->bounds.y + panel->bounds.h) - body.y;
      }
      {int pressed = input_is_mouse_pressed(&ctx->input, NK_BUTTON_LEFT);
        int in_body = input_is_mouse_hovering_rect(&ctx->input, body);
        if (pressed && in_body)
          popup->flags |= static_cast<decltype(panel->flags)>(window_flags::WINDOW_HIDDEN);
      }
    }
    if (popup->flags & static_cast<decltype(popup->flags)>(window_flags::WINDOW_HIDDEN))
      popup->seq = 0;
    popup_end(ctx);
    return;
  }
}
