#include <nk/nuklear.hpp>

namespace nk {
  /* ===============================================================
   *
   *                              MENU
   *
   * ===============================================================*/
  NK_API void
  menubar_begin(context* ctx) {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    panel* layout = ctx->current->layout;
    NK_ASSERT(layout->at_y == layout->bounds.y);
    /* if this assert triggers you allocated space between begin and menubar_begin.
    If you want a menubar the first nuklear function after `begin` has to be a
    `menubar_begin` call. Inside the menubar you then have to allocate space for
    widgets (also supports multiple rows).
    Example:
        if (begin(...)) {
            menubar_begin(...);
                layout_xxxx(...);
                button(...);
                layout_xxxx(...);
                button(...);
            menubar_end(...);
        }
        end(...);
    */
    if (layout->flags & window_flags::WINDOW_HIDDEN || layout->flags & window_flags::WINDOW_MINIMIZED)
      return;

    layout->menu.x = layout->at_x;
    layout->menu.y = layout->at_y + layout->row.height;
    layout->menu.w = layout->bounds.w;
    layout->menu.offset.x = *layout->offset_x;
    layout->menu.offset.y = *layout->offset_y;
    *layout->offset_y = 0;
  }
  NK_API void
  menubar_end(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* win = ctx->current;
    command_buffer* out = &win->buffer;
    panel* layout = win->layout;
    if (layout->flags & window_flags::WINDOW_HIDDEN || layout->flags & window_flags::WINDOW_MINIMIZED)
      return;

    layout->menu.h = layout->at_y - layout->menu.y;
    layout->menu.h += layout->row.height + ctx->style.window.spacing.y;

    layout->bounds.y += layout->menu.h;
    layout->bounds.h -= layout->menu.h;

    *layout->offset_x = layout->menu.offset.x;
    *layout->offset_y = layout->menu.offset.y;
    layout->at_y = layout->bounds.y - layout->row.height;

    layout->clip.y = layout->bounds.y;
    layout->clip.h = layout->bounds.h;
    push_scissor(out, layout->clip);
  }
  INTERN int
  menu_begin(context* ctx, window* win,
             const char* id, const int is_clicked, const rectf header, const vec2f size) {
    int is_open = 0;
    int is_active = 0;
    rectf body;
    const hash hsh = murmur_hash(id, (int) strlen(id), static_cast<hash>(panel_type::PANEL_MENU));

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    body.x = header.x;
    body.w = size.x;
    body.y = header.y + header.h;
    body.h = size.y;

    const window* popup = win->popup.win;
    is_open = popup ? true : false;
    is_active = (popup && (win->popup.name == hsh) && win->popup.type == panel_type::PANEL_MENU);
    if ((is_clicked && is_open && !is_active) || (is_open && !is_active) ||
        (!is_open && !is_active && !is_clicked))
      return 0;
    if (!nonblock_begin(ctx, static_cast<flag>(panel_flags::WINDOW_NO_SCROLLBAR), body, header, panel_type::PANEL_MENU))
      return 0;

    win->popup.type = panel_type::PANEL_MENU;
    win->popup.name = hsh;
    return 1;
  }
  NK_API bool
  menu_begin_text(context* ctx, const char* title, const int len,
                  const flag align, const vec2f size) {
    rectf header;
    int is_clicked = false;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const flag state = widget(&header, ctx);
    if (!state)
      return 0;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_text(&ctx->last_widget_state, &win->buffer, header,
                       title, len, align, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button, in, ctx->style.font))
      is_clicked = true;
    return menu_begin(ctx, win, title, is_clicked, header, size);
  }
  NK_API bool menu_begin_label(context* ctx,
                               const char* text, const flag align, const vec2f size) {
    return menu_begin_text(ctx, text, strlen(text), align, size);
  }
  NK_API bool
  menu_begin_image(context* ctx, const char* id, struct image img,
                   const vec2f size) {
    rectf header;
    int is_clicked = false;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const flag state = widget(&header, ctx);
    if (!state)
      return 0;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_image(&ctx->last_widget_state, &win->buffer, header,
                        img, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button, in))
      is_clicked = true;
    return menu_begin(ctx, win, id, is_clicked, header, size);
  }
  NK_API bool
  menu_begin_symbol(context* ctx, const char* id,
                    const symbol_type sym, const vec2f size) {
    rectf header;
    int is_clicked = false;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const flag state = widget(&header, ctx);
    if (!state)
      return 0;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_symbol(&ctx->last_widget_state, &win->buffer, header,
                         sym, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button, in, ctx->style.font))
      is_clicked = true;
    return menu_begin(ctx, win, id, is_clicked, header, size);
  }
  NK_API bool
  menu_begin_image_text(context* ctx, const char* title, const int len,
                        const flag align, struct image img, const vec2f size) {
    rectf header;
    int is_clicked = false;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const flag state = widget(&header, ctx);
    if (!state)
      return 0;
    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_text_image(&ctx->last_widget_state, &win->buffer,
                             header, img, title, len, align, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button,
                             ctx->style.font, in))
      is_clicked = true;
    return menu_begin(ctx, win, title, is_clicked, header, size);
  }
  NK_API bool
  menu_begin_image_label(context* ctx,
                         const char* title, const flag align, struct image img, const vec2f size) {
    return menu_begin_image_text(ctx, title, strlen(title), align, img, size);
  }
  NK_API bool
  menu_begin_symbol_text(context* ctx, const char* title, const int len,
                         const flag align, const symbol_type sym, const vec2f size) {
    rectf header;
    int is_clicked = false;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const flag state = widget(&header, ctx);
    if (!state)
      return 0;

    const input* in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_text_symbol(&ctx->last_widget_state, &win->buffer,
                              header, sym, title, len, align, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button,
                              ctx->style.font, in))
      is_clicked = true;
    return menu_begin(ctx, win, title, is_clicked, header, size);
  }
  NK_API bool
  menu_begin_symbol_label(context* ctx,
                          const char* title, const flag align, const symbol_type sym, const vec2f size) {
    return menu_begin_symbol_text(ctx, title, strlen(title), align, sym, size);
  }
  NK_API bool
  menu_item_text(context* ctx, const char* title, const int len, const flag align) {
    return contextual_item_text(ctx, title, len, align);
  }
  NK_API bool
  menu_item_label(context* ctx, const char* label, const flag align) {
    return contextual_item_label(ctx, label, align);
  }
  NK_API bool
  menu_item_image_label(context* ctx, struct image img,
                        const char* label, const flag align) {
    return contextual_item_image_label(ctx, img, label, align);
  }
  NK_API bool
  menu_item_image_text(context* ctx, struct image img,
                       const char* text, const int len, const flag align) {
    return contextual_item_image_text(ctx, img, text, len, align);
  }
  NK_API bool menu_item_symbol_text(context* ctx, const symbol_type sym,
                                    const char* text, const int len, const flag align) {
    return contextual_item_symbol_text(ctx, sym, text, len, align);
  }
  NK_API bool menu_item_symbol_label(context* ctx, const symbol_type sym,
                                     const char* label, const flag align) {
    return contextual_item_symbol_label(ctx, sym, label, align);
  }
  NK_API void menu_close(context* ctx) {
    contextual_close(ctx);
  }
  NK_API void
  menu_end(context* ctx) {
    contextual_end(ctx);
  }
} // namespace nk
